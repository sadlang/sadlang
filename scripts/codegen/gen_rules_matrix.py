#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: gen_rules_matrix.py
الوصف: مولّد ومحاسب مصفوفة تداخل قواعد لغة ص (Pairwise Interaction Matrix).

(AR) الغرض (RFC مصفوفة المطابقة الشاملة + TEST-008):
     نريد أن نُثبت — بالأدلة لا بالادعاء (GR-01) — أن:
       ① كل قاعدة من قواعد language-truth/grammar/ مُغطّاة منفردةً (basic/edge/negative).
       ② كل زوج قاعدتين «قابل للتركيب نحويًا» مُغطّى باختبار تداخل واحد على الأقل.

     «قابل للتركيب نحويًا» يُشتق من مصدر الحقيقة نفسه: القاعدة A تحتوي B إن أشارت
     إنتاجاتها (alternatives) إلى B عبر { kind: nonterminal, ref: gr.B }. هذا يولّد
     رسمًا موجّهًا (graph) حوافُه = أزواج التداخل الواقعية فقط — لا حاصل ضرب أعمى
     104×104 (5356 زوجًا أغلبها بلا معنى)، بل ما يمكن لِلُغة أن تركّبه فعلًا.

(EN) Generator + accountant for the Sad grammar pairwise-interaction matrix.
     The composability graph is derived from the SoT (nonterminal refs inside
     `alternatives`), so the required interaction pairs are exactly the grammar
     edges — a risk-based, structurally-valid 2-way coverage, not a blind N×N.

─── ماذا يفعل ──────────────────────────────────────────────────────────────────
    • يبني رسم قابلية التركيب من القواعد (احتواء مباشر + تجاور).
    • يحسب «الأزواج المطلوبة» = حواف الرسم (غير مرتّبة).
    • يمسح tests/behavior/rules_matrix/ ويحسب التغطية الفعلية (منفردة + أزواج).
    • يكتب تقرير مصفوفة مقروء (أثر بناء — لا يُتتبَّع في git).
    • (اختياري) يُهيكل ملفات الاختبار الناقصة: يبذّر «basic» من حقل example حيث
      وُجد، ويُنشئ هياكل للتداخل الناقص موسومة بـ@rule الصحيح.

─── الاستخدام ──────────────────────────────────────────────────────────────────
    python scripts/codegen/gen_rules_matrix.py                 # تقرير الفجوات (لا يكتب)
    python scripts/codegen/gen_rules_matrix.py --check           # حارس CI (سلامة مرجعيّة)
    python scripts/codegen/gen_rules_matrix.py --matrix-md PATH  # + تقرير مصفوفة مقروء
    python scripts/codegen/gen_rules_matrix.py --json PATH       # + أثر بناء JSON
    python scripts/codegen/gen_rules_matrix.py --scaffold --layer 50_patterns  # هيكلة طبقة
    python scripts/codegen/gen_rules_matrix.py --scaffold --pairs --layer 50_patterns
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import json
import os
import re
import sys
from collections import defaultdict
from itertools import combinations
from pathlib import Path

try:
    import yaml
except ImportError:
    print("❌ مطلوب PyYAML: pip install pyyaml", file=sys.stderr)
    sys.exit(2)

# ─── المسارات (متطابقة مع check_grammar_conformance.py) ───────────────────────
ROOT = Path(__file__).resolve().parent.parent.parent
GRAMMAR_DIR = ROOT / "language-truth" / "grammar"
RULES_MATRIX_DIR = ROOT / "tests" / "behavior" / "rules_matrix"

CATEGORIES = ("basic", "edge", "negative", "interaction")
DEFAULT_BUDGET = {"basic": 3, "edge": 0, "negative": 0, "interaction": 0}

_RE_RULE_TAG = re.compile(r"^#\s*@rule:?\s+(.+)$")

# (AR) القواعد «الموزِّعة» (dispatchers) التي تشير لكل بدائلها — تداخلها مع كل
#      فرع تلقائيّ وبلا قيمة اختباريّة مستقلّة (gr.program.statement, ...,
#      gr.expr.expression). نستبعدها من «حواف التداخل المطلوبة» لتفادي تضخّم بلا
#      معنى، لكنها تبقى مُغطّاة منفردةً. تُعلَن أيضًا في القاعدة عبر budget.interaction=0.
DISPATCHER_RULES = {
    "gr.program.program", "gr.program.declaration", "gr.program.statement",
    "gr.expr.expression", "gr.pattern.pattern",
}


def _utf8_console() -> None:
    os.environ.setdefault("PYTHONUTF8", "1")
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            try:
                stream.reconfigure(encoding="utf-8", errors="replace")
            except Exception:
                pass


# ═══════════════════════════════════════════════════════════════════════════════════
# ① تحميل القواعد + بناء رسم قابلية التركيب
# ═══════════════════════════════════════════════════════════════════════════════════

def load_productions() -> dict:
    """(AR) يحمّل كل قواعد الإنتاج: المعرّف، الطبقة، الميزانية، الحالة، المثال، الملف،
    والمراجع المباشرة (nonterminal refs) لبناء رسم قابلية التركيب."""
    productions: dict = {}
    for yf in sorted(GRAMMAR_DIR.glob("*.yaml")):
        if yf.name.startswith("_"):
            continue
        data = yaml.safe_load(open(yf, encoding="utf-8")) or {}
        layer = data.get("layer", "?")
        for prod in data.get("productions", []) or []:
            rid = prod.get("id")
            if not rid:
                continue
            conf = prod.get("conformance", {}) or {}
            budget = dict(DEFAULT_BUDGET)
            budget.update(conf.get("test_budget", {}) or {})
            productions[rid] = {
                "layer": layer,
                "status": prod.get("status", "stable"),
                "compiler_optional": bool(conf.get("compiler_optional", False)),
                "budget": budget,
                "file": yf.name,
                "example": prod.get("example"),
                "desc": prod.get("description_ar", ""),
                "refs": _collect_refs(prod.get("alternatives", []) or []),
            }
    return productions


def _collect_refs(node) -> set:
    """(AR) يجمع كل { kind: nonterminal, ref: gr.X } بشكل تعاودي من شجرة alternatives."""
    refs: set = set()
    if isinstance(node, dict):
        if node.get("kind") == "nonterminal" and isinstance(node.get("ref"), str):
            if node["ref"].startswith("gr."):
                refs.add(node["ref"])
        for v in node.values():
            refs |= _collect_refs(v)
    elif isinstance(node, list):
        for item in node:
            refs |= _collect_refs(item)
    return refs


def _alive(productions: dict, rid: str) -> bool:
    """(AR) قاعدة موجودة وغير مُؤجَّلة وغير موزِّعة (مرشّحة لتكون طرفًا في زوج)."""
    return (rid in productions and rid not in DISPATCHER_RULES
            and productions[rid]["status"] != "planned")


def build_required_pairs(productions: dict) -> tuple:
    """(AR) الأزواج المطلوبة «القابلة للتركيب نحويًا» من مصدر الحقيقة، نوعان:
      ① احتواء مباشر (containment): A تشير إلى B في إنتاجاتها (مثل إذا→كتلة، طابق→نمط).
      ② تجاور (co-nesting): قاعدتان فرعيّتان تظهران كبديلين لنفس القاعدة الموزِّعة،
         أي تستطيعان السكنى في الحاوية نفسها (مثل إذا/بينما داخل كتلة، حقل/طريقة داخل صنف).
    (EN) Required pairs from the SoT in two flavors: direct containment edges, and
    sibling co-nesting (two leaf rules listed as alternatives of the same dispatcher).
    Returns (all_pairs, containment_pairs, conest_pairs)."""
    containment: set = set()
    for rid, info in productions.items():
        if not _alive(productions, rid):
            continue
        for ref in info["refs"]:
            if ref != rid and _alive(productions, ref):
                containment.add(frozenset((rid, ref)))

    # (AR) مجموعات الإخوة: بدائل كل قاعدة موزِّعة هي إخوة متجاورون
    conest: set = set()
    for disp in DISPATCHER_RULES:
        if disp not in productions:
            continue
        siblings = sorted(r for r in productions[disp]["refs"] if _alive(productions, r))
        for a, b in combinations(siblings, 2):
            conest.add(frozenset((a, b)))

    return (containment | conest), containment, conest


# ═══════════════════════════════════════════════════════════════════════════════════
# ② مسح الاختبارات القائمة (يعيد استخدام منطق الفاحص)
# ═══════════════════════════════════════════════════════════════════════════════════

def _category_of(path: Path) -> str:
    if "_interactions" in path.parts:
        return "interaction"
    parts = set(p.lower() for p in path.parts)
    for cat in ("basic", "edge", "negative"):
        if cat in parts:
            return cat
    return "uncategorized"


def _extract_rule_ids(filepath: Path) -> list:
    ids: list = []
    try:
        # (AR) errors="replace": بايتات غير UTF-8 لا تُسقط الحارس بtraceback خام —
        #      إن أصابت وسم @rule تحوّلت لقاعدة مجهولة فيشخّصها --check باسم الملفّ.
        # (EN) errors="replace": non-UTF-8 bytes must not crash the guard with a raw
        #      traceback; a corrupted @rule tag degrades to an unknown rule id that
        #      --check then diagnoses with the offending file name.
        for i, line in enumerate(open(filepath, encoding="utf-8", errors="replace")):
            if i >= 30:
                break
            m = _RE_RULE_TAG.match(line.strip())
            if m:
                ids.extend(r.strip() for r in re.split(r"[,،]", m.group(1)) if r.strip())
    except OSError:
        pass
    return ids


def scan_tests() -> list:
    records: list = []
    if not RULES_MATRIX_DIR.is_dir():
        return records
    for tf in sorted(RULES_MATRIX_DIR.rglob("*.ص")):
        rel = tf.relative_to(RULES_MATRIX_DIR)
        records.append({
            "rel": str(rel),
            "category": _category_of(rel),
            "rule_ids": _extract_rule_ids(tf),
        })
    return records


def measure_coverage(productions: dict, records: list):
    """(AR) يُرجع: عدّاد[rule][cat]، الأزواج المُغطّاة (frozenset)."""
    counts: dict = defaultdict(lambda: defaultdict(int))
    covered_pairs: set = set()
    for rec in records:
        for rid in rec["rule_ids"]:
            if rid in productions:
                counts[rid][rec["category"]] += 1
        if rec["category"] == "interaction":
            valid = [r for r in rec["rule_ids"] if r in productions]
            for a, b in combinations(sorted(set(valid)), 2):
                covered_pairs.add(frozenset((a, b)))
    return counts, covered_pairs


# ═══════════════════════════════════════════════════════════════════════════════════
# ③ تحليل الفجوات
# ═══════════════════════════════════════════════════════════════════════════════════

def analyze(productions: dict, counts: dict, required_pairs: set, covered_pairs: set):
    """(AR) فجوات منفردة لكل قاعدة/فئة + أزواج مطلوبة غير مُغطّاة."""
    single_gaps: list = []
    for rid, info in sorted(productions.items()):
        if info["status"] == "planned":
            continue
        for cat in ("basic", "edge", "negative"):
            need, have = info["budget"].get(cat, 0), counts[rid].get(cat, 0)
            if have < need:
                single_gaps.append((rid, cat, have, need))
    missing_pairs = sorted(required_pairs - covered_pairs, key=lambda p: sorted(p))
    return single_gaps, missing_pairs


# ═══════════════════════════════════════════════════════════════════════════════════
# ④ الهيكلة (scaffolding)
# ═══════════════════════════════════════════════════════════════════════════════════

def _area_dir(productions: dict, rid: str) -> str:
    """(AR) مجلد المنطقة من اسم ملف القاعدة: 50_patterns.yaml → 50_patterns."""
    return productions[rid]["file"].rsplit(".", 1)[0]


def _header(rule_tag: str, desc: str, expected, negative: bool) -> str:
    lines = [f"# @rule: {rule_tag}", f"# @description: {desc}", "# @priority: P2",
             "# @requires: اطبع_سطر"]
    if negative:
        lines.append("# @expect_error")
    elif expected is not None:
        lines.append(f"# @expected: {expected}")
    else:
        lines.append("# TODO(بذرة): املأ @expected بتشغيل المفسّر ثم تحقّق من تطابق المترجم")
    return "\n".join(lines) + "\n\n"


def scaffold_single(productions: dict, counts: dict, layer: str, force: bool) -> int:
    """(AR) يهيكل ملفات basic/edge/negative الناقصة لقواعد طبقة. يبذّر basic من example."""
    written = 0
    for rid, info in sorted(productions.items()):
        if _area_dir(productions, rid) != layer or info["status"] == "planned":
            continue
        if rid in DISPATCHER_RULES:
            continue
        base = RULES_MATRIX_DIR / layer / rid
        for cat in ("basic", "edge", "negative"):
            need, have = info["budget"].get(cat, 0), counts[rid].get(cat, 0)
            for i in range(have + 1, need + 1):
                d = base / cat
                d.mkdir(parents=True, exist_ok=True)
                fp = d / f"{i:03d}_{cat}_seed.ص"
                if fp.exists() and not force:
                    continue
                body = ""
                if cat == "basic" and info["example"]:
                    body = info["example"].rstrip() + "\n"
                hdr = _header(rid, f"{info['desc'][:40]} — {cat}", None, cat == "negative")
                fp.write_text(hdr + (body or "# TODO: جسد الاختبار\n"), encoding="utf-8")
                written += 1
    return written


def scaffold_pairs(productions: dict, missing_pairs: list, layer) -> int:
    """(AR) يهيكل اختبار تداخل لكل زوج مطلوب غير مُغطّى (تحت _interactions/_generated)."""
    written = 0
    out_root = RULES_MATRIX_DIR / "_interactions" / "_generated"
    seq: dict = defaultdict(int)
    for pair in missing_pairs:
        a, b = sorted(pair)
        if layer and _area_dir(productions, a) != layer and _area_dir(productions, b) != layer:
            continue
        theme = f"{a}__{b}".replace("gr.", "")
        seq[theme] += 1
        d = out_root / theme
        d.mkdir(parents=True, exist_ok=True)
        fp = d / f"{seq[theme]:03d}_{theme}.ص"
        if fp.exists():
            continue
        hdr = _header(f"{a}, {b}", f"تداخل {productions[a]['desc'][:25]} + {productions[b]['desc'][:25]}",
                      None, False)
        fp.write_text(hdr + "# TODO: ركّب القاعدتين في برنامج واحد ثم ابذر @expected\n",
                      encoding="utf-8")
        written += 1
    return written


# ═══════════════════════════════════════════════════════════════════════════════════
# ⑤ التقارير
# ═══════════════════════════════════════════════════════════════════════════════════

def write_matrix_md(productions: dict, counts: dict, required_pairs: set,
                    covered_pairs: set, single_gaps: list, missing_pairs: list,
                    out_path: Path) -> None:
    import time
    by_layer: dict = defaultdict(lambda: {"rules": 0, "covered": 0})
    for rid, info in productions.items():
        if info["status"] == "planned":
            continue
        by_layer[info["layer"]]["rules"] += 1
        has = sum(counts[rid].get(c, 0) for c in ("basic", "edge", "negative")) > 0
        by_layer[info["layer"]]["covered"] += 1 if has else 0

    L = ["# مصفوفة تداخل قواعد لغة ص — تقرير التغطية الثنائية",
         "",
         "> **مُولَّد آليًا** بـ`scripts/codegen/gen_rules_matrix.py`. لا يُحرَّر يدويًا.",
         f"> التوليد: {time.strftime('%Y-%m-%d %H:%M:%S')}",
         "",
         "## الملخّص",
         "",
         f"- القواعد (غير planned): **{sum(l['rules'] for l in by_layer.values())}**",
         f"- قواعد لها ≥1 اختبار منفرد: **{sum(l['covered'] for l in by_layer.values())}**",
         f"- الأزواج المطلوبة (قابلة للتركيب نحويًا): **{len(required_pairs)}**",
         f"- الأزواج المُغطّاة: **{len(covered_pairs & required_pairs)}**",
         f"- الأزواج الناقصة: **{len(missing_pairs)}**",
         f"- فجوات منفردة (قاعدة×فئة): **{len(single_gaps)}**",
         "",
         "## التغطية المنفردة لكل طبقة",
         "",
         "| الطبقة | القواعد | لها اختبار منفرد |",
         "|---|---|---|"]
    for layer in sorted(by_layer):
        d = by_layer[layer]
        L.append(f"| {layer} | {d['rules']} | {d['covered']} |")
    L += ["", "## الأزواج المطلوبة الناقصة (أول 200)", "",
          "| القاعدة أ | القاعدة ب |", "|---|---|"]
    for pair in missing_pairs[:200]:
        a, b = sorted(pair)
        L.append(f"| `{a}` | `{b}` |")
    if len(missing_pairs) > 200:
        L.append(f"| … | (+{len(missing_pairs) - 200} زوجًا) |")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(L) + "\n", encoding="utf-8")
    print(f"📄 تقرير المصفوفة: {out_path}")


# ═══════════════════════════════════════════════════════════════════════════════════
# ⑥ حارس CI (--check) — سلامة مرجعيّة بين اختبارات المصفوفة ومصدر الحقيقة
# ═══════════════════════════════════════════════════════════════════════════════════

def run_check(productions: dict, records: list) -> int:
    """(AR) وضع الحارس (بنمط بقيّة المولّدات، لبوّابة `x.py gen --check`) — لا يكتب شيئًا.
        يفشل عند انجراف مرجعيّ بين tests/behavior/rules_matrix/ وlanguage-truth/grammar/:
          ① وسم `# @rule:` يشير إلى معرّف قاعدة غير موجود في مصدر الحقيقة.
          ② قاعدة موزِّعة مُعلنة هنا (DISPATCHER_RULES) اختفت من مصدر الحقيقة.
          ③ مصدر الحقيقة فارغ (فشل تحميل — عطل بنيويّ لا فجوة تغطية).
        ملحوظة: فجوات التغطية (أزواج/منفردة) **ليست** فشل حارس — هي دَين مقصود
        يُتابَع عبر التقرير؛ الحارس يمنع الانجراف المرجعيّ فقط.
    (EN) Non-mutating guard mode for the `x.py gen --check` gate: fails only on
        referential drift (unknown @rule ids, vanished dispatchers, empty SoT) —
        coverage gaps are tracked debt, not guard failures."""
    if not productions:
        print("❌ [rules_matrix --check] مصدر الحقيقة فارغ — لم تُحمَّل أيّ قاعدة من "
              f"{GRAMMAR_DIR}", file=sys.stderr)
        return 1

    problems: list = []
    ghost_dispatchers = sorted(d for d in DISPATCHER_RULES if d not in productions)
    for d in ghost_dispatchers:
        problems.append(f"قاعدة موزِّعة غير موجودة في مصدر الحقيقة: {d}")

    unknown: dict = defaultdict(list)
    for rec in records:
        for rid in rec["rule_ids"]:
            if rid not in productions:
                unknown[rid].append(rec["rel"])
    for rid in sorted(unknown):
        files = unknown[rid]
        sample = "، ".join(files[:3]) + (f" (+{len(files) - 3})" if len(files) > 3 else "")
        problems.append(f"وسم @rule يشير إلى قاعدة مجهولة: {rid} — في: {sample}")

    if problems:
        print("❌ [rules_matrix --check] انجراف مرجعيّ بين اختبارات المصفوفة ومصدر الحقيقة:",
              file=sys.stderr)
        for p in problems:
            print(f"   • {p}", file=sys.stderr)
        print("   الإصلاح: صحّح وسوم @rule أو حدّث language-truth/grammar/ — "
              "لا تُخفِ الفشل بحذف الوسم.", file=sys.stderr)
        return 1

    print(f"✓ [rules_matrix --check] سلامة مرجعيّة: {len(productions)} قاعدة · "
          f"{len(records)} ملف اختبار · لا وسوم @rule مجهولة.")
    return 0


# ═══════════════════════════════════════════════════════════════════════════════════
# ⑦ نقطة الدخول
# ═══════════════════════════════════════════════════════════════════════════════════

def main() -> int:
    _utf8_console()
    ap = argparse.ArgumentParser(description="مولّد ومحاسب مصفوفة تداخل قواعد لغة ص")
    ap.add_argument("--matrix-md", help="مسار كتابة تقرير المصفوفة المقروء")
    ap.add_argument("--json", help="مسار كتابة أثر بناء JSON")
    ap.add_argument("--scaffold", action="store_true", help="هيكلة ملفات الاختبار الناقصة")
    ap.add_argument("--pairs", action="store_true", help="مع --scaffold: هيكلة أزواج التداخل أيضًا")
    ap.add_argument("--layer", help="حصر الهيكلة بطبقة (مثل 50_patterns)")
    ap.add_argument("--force", action="store_true", help="استبدال الهياكل القائمة")
    ap.add_argument("--check", action="store_true",
                    help="حارس CI: سلامة مرجعيّة بين @rule ومصدر الحقيقة (لا يكتب)")
    args = ap.parse_args()

    productions = load_productions()
    records = scan_tests()

    if args.check:
        return run_check(productions, records)

    required_pairs, containment, conest = build_required_pairs(productions)
    counts, covered_pairs = measure_coverage(productions, records)
    single_gaps, missing_pairs = analyze(productions, counts, required_pairs, covered_pairs)

    cov = covered_pairs & required_pairs
    print("═" * 70)
    print(f"  مصفوفة القواعد — {len(productions)} قاعدة · "
          f"{len(required_pairs)} زوجًا مطلوبًا · {len(records)} ملف اختبار")
    print("═" * 70)
    print(f"  أزواج مطلوبة: {len(required_pairs)}  "
          f"(احتواء مباشر: {len(containment)} · تجاور: {len(conest)})")
    print(f"  أزواج مُغطّاة: {len(cov)}/{len(required_pairs)}"
          f"  ·  أزواج ناقصة: {len(missing_pairs)}")
    print(f"  فجوات منفردة (قاعدة×فئة): {len(single_gaps)}")

    if args.matrix_md:
        write_matrix_md(productions, counts, required_pairs, covered_pairs,
                        single_gaps, missing_pairs, Path(args.matrix_md))
    if args.json:
        out = {
            "total_rules": len([1 for i in productions.values() if i["status"] != "planned"]),
            "required_pairs": len(required_pairs),
            "containment_pairs": len(containment),
            "conest_pairs": len(conest),
            "covered_pairs": len(cov),
            "missing_pairs": [sorted(p) for p in missing_pairs],
            "single_gaps": [{"rule": r, "cat": c, "have": h, "need": n}
                            for (r, c, h, n) in single_gaps],
        }
        Path(args.json).parent.mkdir(parents=True, exist_ok=True)
        json.dump(out, open(args.json, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(f"📝 أثر بناء JSON: {args.json}")

    if args.scaffold:
        if not args.layer:
            print("⚠️ الهيكلة تتطلّب --layer لتفادي توليد آلاف الملفات دفعة واحدة.")
            return 1
        n1 = scaffold_single(productions, counts, args.layer, args.force)
        n2 = scaffold_pairs(productions, missing_pairs, args.layer) if args.pairs else 0
        print(f"🛠️  هُيكِل: {n1} ملف منفرد · {n2} ملف تداخل (طبقة {args.layer})")

    return 0


if __name__ == "__main__":
    sys.exit(main())
