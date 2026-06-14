#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: check_grammar_conformance.py
الوصف: الفاحص الشامل لمطابقة قواعد لغة ص — يربط كل قاعدة إنتاج في
       language-truth/grammar/*.yaml باختباراتها المنظَّمة في
       tests/behavior/rules_matrix/، ويُثبت بالأدلة هل القاعدة مدعومة فعلاً في
       المفسر والمترجم معاً أم لا.

(AR) المبدأ (GR-01): لا ندّعي دعم قاعدة بلا دليل. الدليل = اختبار .ص موسوم بـ@rule
     يمرّ عبر runner.py في المفسر (sad-run) والمترجم (sadc) بنفس المخرج.

(EN) Comprehensive grammar-conformance checker. Links every production to its
     organized tests and proves — with evidence — that each rule works in BOTH
     the interpreter and the compiler.

─── بنية الاختبارات (قابلة للتوسّع لآلاف الاختبارات) ───────────────────────────
    tests/behavior/rules_matrix/
      <NN>_<area>/                 ← مثل 00_program, 10_statements
        <rule_id>/                 ← مثل gr.stmt.if  (اسم المجلد = معرّف القاعدة)
          basic/      NNN_*.ص      ← الأشكال الأساسية الصحيحة
          edge/       NNN_*.ص      ← حالات حدّية (فراغ، تعشيش، حدود)
          negative/   NNN_*.ص      ← خطأ متوقَّع (@expect_error)
      _interactions/               ← تداخل عبر قواعد (وسم @rule متعدد)
        <theme>/      NNN_*.ص      ← مثل شرط_متغير_نطاق

─── الفئة تُشتق من المسار ──────────────────────────────────────────────────────
    _interactions/* → interaction | .../basic/* → basic | edge | negative

─── ثلاث بوّابات ───────────────────────────────────────────────────────────────
    ① التغطية:   كل قاعدة تستوفي ميزانية كل فئة (conformance.test_budget)
    ② التماسك:   @rule يطابق مجلد القاعدة؛ لا اختبار بلا وسم؛ لا وسم ليتيم
    ③ المزدوج (--run): كل اختبار يمرّ في المفسر والمترجم بنفس المخرج

الاستخدام:
    python scripts/codegen/check_grammar_conformance.py            # ①②
    python scripts/codegen/check_grammar_conformance.py --run      # ①②③ + أدلة
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import json
import os
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

try:
    import yaml
except ImportError:
    print("❌ مطلوب PyYAML: pip install pyyaml", file=sys.stderr)
    sys.exit(2)


# ─── المسارات الأساسية ───────────────────────────────────────────────────────
ROOT = Path(__file__).resolve().parent.parent.parent
GRAMMAR_DIR = ROOT / "language-truth" / "grammar"
RULES_MATRIX_DIR = ROOT / "tests" / "behavior" / "rules_matrix"
RUNNER = ROOT / "tests" / "runner.py"
DUAL_REPORT = ROOT / "build" / "_dual_report.json"
# (AR) الأدلة أثر بناء — مثل تقرير runner تماماً (build/ متجاهَل في git)
EVIDENCE_OUT = ROOT / "build" / "_grammar_conformance.json"

# (AR) الفئات المعروفة (تُشتق من اسم المجلد)
CATEGORIES = ("basic", "edge", "negative", "interaction")
# (AR) ميزانية افتراضية حين لا يصرّح conformance.test_budget
DEFAULT_BUDGET = {"basic": 3, "edge": 0, "negative": 0, "interaction": 0}

# (AR) وسم ربط الاختبار بقاعدته — مثال: # @rule: gr.program.block, gr.decl.variable
_RE_RULE_TAG = re.compile(r"^#\s*@rule:?\s+(.+)$")


def _utf8_console() -> None:
    """(AR) فرض UTF-8 على المخرجات لتجنّب أخطاء الترميز على ويندوز."""
    os.environ.setdefault("PYTHONUTF8", "1")
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            try:
                stream.reconfigure(encoding="utf-8", errors="replace")
            except Exception:
                pass


# ═══════════════════════════════════════════════════════════════════════════════════
# ① تحميل قواعد الإنتاج + ميزانياتها
# ═══════════════════════════════════════════════════════════════════════════════════

def load_productions() -> dict:
    """(AR) يحمّل كل قواعد الإنتاج من language-truth/grammar/*.yaml.
    (EN) Load all productions; returns dict[rule_id -> info].
    """
    productions: dict[str, dict] = {}
    if not GRAMMAR_DIR.is_dir():
        return productions

    for yf in sorted(GRAMMAR_DIR.glob("*.yaml")):
        if yf.name.startswith("_"):          # (AR) ملفات الميتا ليست قواعد
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
            }
    return productions


# ═══════════════════════════════════════════════════════════════════════════════════
# ② مسح الاختبارات: استخراج (القاعدة، الفئة، مجلد القاعدة) لكل ملف
# ═══════════════════════════════════════════════════════════════════════════════════

def _category_of(path: Path) -> str:
    """(AR) يشتق فئة الاختبار من مساره."""
    parts = set(p.lower() for p in path.parts)
    if "_interactions" in path.parts:
        return "interaction"
    for cat in ("basic", "edge", "negative"):
        if cat in parts:
            return cat
    return "uncategorized"


def _folder_rule_of(path: Path) -> str | None:
    """(AR) معرّف القاعدة من مجلد الاختبار (الجزء الذي يبدأ بـgr.) — None للتداخل."""
    for part in path.parts:
        if part.startswith("gr."):
            return part
    return None


def _extract_rule_ids(filepath: Path) -> list[str]:
    """(AR) يستخرج معرّفات القواعد من وسم @rule (يقبل قائمة بفواصل عربية/لاتينية)."""
    ids: list[str] = []
    try:
        for i, line in enumerate(open(filepath, encoding="utf-8")):
            if i >= 30:
                break
            m = _RE_RULE_TAG.match(line.strip())
            if m:
                ids.extend(r.strip() for r in re.split(r"[,،]", m.group(1)) if r.strip())
    except OSError:
        pass
    return ids


def scan_tests() -> list[dict]:
    """(AR) يمسح كل ملفات .ص ويُرجع سجلاً لكل ملف.
    (EN) Scan all test files; one record per file.
    """
    records: list[dict] = []
    if not RULES_MATRIX_DIR.is_dir():
        return records
    for tf in sorted(RULES_MATRIX_DIR.rglob("*.ص")):
        rel = tf.relative_to(RULES_MATRIX_DIR)
        records.append({
            "path": tf,
            "rel": str(rel),
            "name": tf.name,
            "category": _category_of(rel),
            "folder_rule": _folder_rule_of(rel),
            "rule_ids": _extract_rule_ids(tf),
        })
    return records


# ═══════════════════════════════════════════════════════════════════════════════════
# ③ البوّابتان ①+②: التغطية والتماسك
# ═══════════════════════════════════════════════════════════════════════════════════

def check_coverage_and_linkage(productions: dict, records: list[dict]):
    """(AR) يُرجع (أخطاء التغطية، أخطاء التماسك، عدّاد[rule][cat])."""
    counts: dict = defaultdict(lambda: defaultdict(int))
    linkage_errors: list[str] = []

    for rec in records:
        if not rec["rule_ids"]:
            linkage_errors.append(f"  ✗ اختبار بلا وسم @rule: {rec['rel']}")
            continue
        # (AR) داخل مجلد قاعدة: الوسم يجب أن يحوي اسم المجلد
        if rec["folder_rule"] and rec["folder_rule"] not in rec["rule_ids"]:
            linkage_errors.append(
                f"  ✗ وسم @rule لا يطابق مجلد القاعدة ({rec['folder_rule']}): {rec['rel']}")
        for rid in rec["rule_ids"]:
            if rid not in productions:
                linkage_errors.append(f"  ✗ @rule يشير لقاعدة غير موجودة: {rid}  ({rec['rel']})")
            else:
                counts[rid][rec["category"]] += 1

    coverage_errors: list[str] = []
    for rid, info in sorted(productions.items()):
        if info["status"] == "planned":
            continue
        for cat in CATEGORIES:
            need = info["budget"].get(cat, 0)
            have = counts[rid].get(cat, 0)
            if have < need:
                coverage_errors.append(
                    f"  ✗ {rid} [{cat}] — {have}/{need}  ({info['file']})")

    return coverage_errors, linkage_errors, counts


# ═══════════════════════════════════════════════════════════════════════════════════
# ④ البوّابة ③: التنفيذ المزدوج عبر runner.py
# ═══════════════════════════════════════════════════════════════════════════════════

def run_dual_execution(args) -> dict:
    """(AR) يُشغّل runner.py على rules_matrix ويُرجع تقرير JSON المُحلَّل."""
    cmd = [sys.executable, str(RUNNER), "--dir", "rules_matrix", "--report"]
    if args.interpreter:
        cmd += ["--interp", args.interpreter]   # (AR) runner.py يستخدم --interp لا --interpreter
    if args.compiler:
        cmd += ["--compiler", args.compiler]
    print(f"▶ {' '.join(cmd)}\n")
    proc = subprocess.run(cmd, cwd=str(ROOT))
    if not DUAL_REPORT.exists():
        print(f"❌ لم يُنتَج تقرير: {DUAL_REPORT} (exit={proc.returncode})", file=sys.stderr)
        return {}
    return json.load(open(DUAL_REPORT, encoding="utf-8"))


def _support(status: str, mode: str) -> dict:
    """(AR) حالة runner → دعم كل محرّك."""
    i = c = "?"
    if status == "PASS":
        i, c = "ok", ("n/a" if mode == "interpreter_only" else "ok")
    elif status == "FAIL_OUTPUT":
        i = c = "diverge"
    elif status == "FAIL_INTERP":
        i, c = "fail", "?"
    elif status in ("FAIL_COMPILE", "FAIL_RUNTIME"):
        i, c = "ok", "fail"
    elif status == "FAIL_TIMEOUT":
        i = c = "timeout"
    elif status == "SKIP":
        i = c = "skip"
    return {"interpreter": i, "compiler": c, "status": status}


def derive_matrix(productions: dict, records: list[dict], report: dict) -> dict:
    """(AR) يجمع نتائج التنفيذ لكل قاعدة من تقرير runner."""
    by_name = {Path(e["file"]).name: e for e in report.get("tests", [])}
    rule_results: dict = defaultdict(list)
    for rec in records:
        entry = by_name.get(rec["name"])
        res = {"test": rec["rel"], "category": rec["category"]}
        if entry is None:
            res.update({"interpreter": "?", "compiler": "?", "status": "NOT_RUN"})
        else:
            res.update(_support(entry["status"], entry.get("mode", "dual_parity")))
        for rid in rec["rule_ids"]:
            if rid in productions:
                rule_results[rid].append(res)

    matrix: dict = {}
    for rid, info in sorted(productions.items()):
        results = rule_results.get(rid, [])
        matrix[rid] = {
            "layer": info["layer"],
            "compiler_optional": info["compiler_optional"],
            "verdict": _verdict(results, info),
            "tests": results,
        }
    return matrix


def _verdict(results: list[dict], info: dict) -> str:
    if not results:
        return "no_tests"
    if any(r["interpreter"] in ("fail", "diverge", "timeout") for r in results):
        return "broken"
    if any(r["compiler"] in ("fail", "diverge", "timeout") for r in results):
        return "interp_only" if info["compiler_optional"] else "compiler_gap"
    if any(r["status"] == "NOT_RUN" for r in results):
        return "not_run"
    return "dual_ok"


def write_evidence(matrix: dict, counts: dict) -> None:
    """(AR) يكتب أثر بناء build/_grammar_conformance.json (لا يُتتبَّع في git)."""
    import time
    summary = {v: sum(1 for m in matrix.values() if m["verdict"] == v)
               for v in ("dual_ok", "compiler_gap", "interp_only", "broken", "no_tests", "not_run")}
    out = {
        "generated_at": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "generated_by": "scripts/codegen/check_grammar_conformance.py",
        "source_report": str(DUAL_REPORT.relative_to(ROOT)),
        "total_rules": len(matrix),
        "summary": summary,
        "rules": {rid: {**m, "test_counts": dict(counts.get(rid, {}))}
                  for rid, m in matrix.items()},
    }
    EVIDENCE_OUT.parent.mkdir(parents=True, exist_ok=True)
    json.dump(out, open(EVIDENCE_OUT, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(f"\n📝 أدلة (أثر بناء): {EVIDENCE_OUT.relative_to(ROOT)}")


# (AR) أسماء الحالات بالعربية للتقرير المقروء
_STATUS_AR = {
    "PASS": "تطابق ✅", "FAIL_OUTPUT": "تباعد المخرجات ⚠️", "FAIL_INTERP": "فشل المفسر ❌",
    "FAIL_COMPILE": "فشل الترجمة ❌", "FAIL_RUNTIME": "تعطّل التنفيذي ❌",
    "FAIL_TIMEOUT": "تجاوز المهلة ⏱", "SKIP": "تخطٍّ", "NOT_RUN": "لم يُشغَّل",
}
_VERDICT_AR = {
    "dual_ok": "مطلقة (مفسر≡مترجم)", "compiler_gap": "فجوة مترجم", "interp_only": "مفسر فقط (مُعفاة)",
    "broken": "مكسورة", "no_tests": "بلا اختبارات", "not_run": "لم تُشغَّل",
}


def write_markdown(matrix: dict, counts: dict, report: dict, records: list[dict], out_path: Path) -> None:
    """(AR) يكتب تقريرين: ملخّص (out_path) + تفصيل لكل اختبار (out_path_detail.md)
    يُظهران مقارنة المفسر بالمترجم، الأزمنة، أسماء الاختبارات، والتباعدات.
    (EN) Writes a summary report + a per-test detail report (names, times, status).
    """
    import time
    tests = report.get("tests", [])
    by_name = {Path(e["file"]).name: e for e in tests}
    total = len(tests)
    passed = sum(1 for e in tests if e["status"] == "PASS")
    # (AR) أزمنة التنفيذ الكلية
    tot_interp = sum(e.get("interp_time_ms", 0) for e in tests)
    tot_comp = sum(e.get("compiler_time_ms", 0) for e in tests)
    # (AR) جمع كل التباعدات/الإخفاقات (هنا «نعرف الاختلافات لنصحّحها»)
    diffs = [e for e in tests if e["status"] not in ("PASS", "SKIP")]

    L = []
    L.append("# تقرير مطابقة قواعد لغة ص — مقارنة المفسر والمترجم")
    L.append("")
    L.append("> **مُولَّد آلياً** بـ`scripts/codegen/check_grammar_conformance.py --run`. لا يُحرَّر يدوياً.")
    L.append(f"> التوليد: {time.strftime('%Y-%m-%d %H:%M:%S')}")
    L.append("")
    L.append("كل اختبار يُشغَّل عبر **المفسر** (sad-run) و**المترجم** (sadc) ويُقارَن مخرجاهما:")
    L.append("`تطابق` = المخرجان متطابقان؛ `تباعد` = اختلفا (هنا يظهر ما يجب تصحيحه).")
    L.append("")
    L.append("## الملخص")
    L.append("")
    L.append(f"- إجمالي الاختبارات: **{total}** — تطابق مزدوج: **{passed}** — تباعد/إخفاق: **{len(diffs)}**")
    sm = {v: sum(1 for m in matrix.values() if m["verdict"] == v)
          for v in ("dual_ok", "compiler_gap", "interp_only", "broken", "no_tests", "not_run")}
    L.append(f"- القواعد: {len(matrix)} — مطلقة: **{sm['dual_ok']}** · "
             f"فجوة مترجم: {sm['compiler_gap']} · مكسورة: {sm['broken']} · بلا اختبارات: {sm['no_tests']}")
    avg_i = tot_interp / total if total else 0
    avg_c = tot_comp / total if total else 0
    L.append(f"- زمن التنفيذ: المفسر **{tot_interp/1000:.1f}s** (متوسط {avg_i:.0f}ms/اختبار) · "
             f"المترجم **{tot_comp/1000:.1f}s** (متوسط {avg_c:.0f}ms/اختبار)")
    L.append(f"- التفصيل الكامل لكل اختبار: [`{out_path.stem}_detail.md`](./{out_path.stem}_detail.md)")
    L.append("")
    # ── قسم التباعدات (الأهم: «كيف نعرف الاختلافات لنصحّحها») ──
    L.append("## التباعدات والإخفاقات (للتصحيح)")
    L.append("")
    if not diffs:
        L.append("✅ **لا تباعد** — كل الاختبارات أعطت مخرجاً متطابقاً في المفسر والمترجم.")
    else:
        L.append("| الاختبار | الحالة | مفسر(ms) | مترجم(ms) | مخرج المفسر | مخرج المترجم |")
        L.append("|---|---|---|---|---|---|")
        for e in diffs:
            io = (e.get("interp_output", "") or "").replace("\n", "⏎")[:50]
            co = (e.get("compiler_output", "") or "").replace("\n", "⏎")[:50]
            L.append(f"| `{e['file']}` | {_STATUS_AR.get(e['status'], e['status'])} | "
                     f"{e.get('interp_time_ms',0):.0f} | {e.get('compiler_time_ms',0):.0f} | `{io}` | `{co}` |")
    L.append("")
    # ── جدول القواعد ──
    L.append("## المقارنة المزدوجة لكل قاعدة")
    L.append("")
    L.append("| القاعدة | الطبقة | اختبارات | الحُكم |")
    L.append("|---|---|---|---|")
    for rid, m in sorted(matrix.items()):
        tot = sum(counts.get(rid, {}).values())
        L.append(f"| `{rid}` | {m['layer']} | {tot} | {_VERDICT_AR.get(m['verdict'], m['verdict'])} |")
    L.append("")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text("\n".join(L), encoding="utf-8")
    print(f"📄 تقرير مقروء: {out_path}")

    # ── التقرير التفصيلي: كل اختبار بكل معلوماته ──
    D = []
    D.append("# تفصيل اختبارات مطابقة القواعد — كل اختبار بكل معلوماته")
    D.append("")
    D.append("> مُولَّد آلياً مع [التقرير الملخّص](./" + out_path.name + "). أعمدة: الرقم، الاختبار،")
    D.append("> القاعدة، الفئة، الحالة (نتيجة مقارنة المفسر بالمترجم)، زمن المفسر، زمن المترجم.")
    D.append("")
    D.append("| # | الاختبار | القاعدة | الفئة | الحالة | مفسر(ms) | مترجم(ms) |")
    D.append("|---|---|---|---|---|---|---|")
    for i, rec in enumerate(sorted(records, key=lambda r: r["rel"]), start=1):
        e = by_name.get(rec["name"], {})
        rid = rec.get("folder_rule") or (rec["rule_ids"][0] if rec["rule_ids"] else "—")
        st = _STATUS_AR.get(e.get("status", "NOT_RUN"), e.get("status", "؟"))
        D.append(f"| {i} | `{rec['rel']}` | `{rid}` | {rec['category']} | {st} | "
                 f"{e.get('interp_time_ms',0):.0f} | {e.get('compiler_time_ms',0):.0f} |")
    D.append("")
    detail_path = out_path.with_name(out_path.stem + "_detail.md")
    detail_path.write_text("\n".join(D), encoding="utf-8")
    print(f"📄 تفصيل كل اختبار: {detail_path}")


# ═══════════════════════════════════════════════════════════════════════════════════
# ⑤ نقطة الدخول
# ═══════════════════════════════════════════════════════════════════════════════════

def main() -> int:
    _utf8_console()
    ap = argparse.ArgumentParser(description="الفاحص الشامل لمطابقة قواعد لغة ص")
    ap.add_argument("--run", action="store_true", help="تشغيل التنفيذ المزدوج + توليد الأدلة")
    ap.add_argument("--interpreter", help="مسار مفسر مخصص")
    ap.add_argument("--compiler", help="مسار مترجم مخصص")
    ap.add_argument("--report-md", dest="report_md",
                    help="مسار كتابة تقرير Markdown مقروء (مقارنة المفسر/المترجم + التباعدات)")
    args = ap.parse_args()

    productions = load_productions()
    records = scan_tests()
    tagged = sum(1 for r in records if r["rule_ids"])

    print("═" * 70)
    print(f"  مطابقة القواعد — {len(productions)} قاعدة، {len(records)} ملف اختبار "
          f"({tagged} موسوم)")
    print("═" * 70)

    cov_err, link_err, counts = check_coverage_and_linkage(productions, records)

    print("\n① التغطية (ميزانية كل فئة لكل قاعدة):")
    print("  ✅ كل الميزانيات مستوفاة" if not cov_err else "\n".join(cov_err))
    print("\n② التماسك (الوسوم تطابق المجلدات وتشير لقواعد موجودة):")
    print("  ✅ كل الوسوم سليمة" if not link_err else "\n".join(link_err))

    gates_ok = not cov_err and not link_err

    if args.run:
        print("\n③ التنفيذ المزدوج (مفسر ≡ مترجم):")
        report = run_dual_execution(args)
        if report:
            matrix = derive_matrix(productions, records, report)
            write_evidence(matrix, counts)
            if args.report_md:
                write_markdown(matrix, counts, report, records, Path(args.report_md))
            bad = sorted(r for r, m in matrix.items()
                         if m["verdict"] in ("broken", "compiler_gap", "not_run"))
            for rid in bad:
                print(f"  ✗ {rid} — {matrix[rid]['verdict']}")
            print("  ✅ كل القواعد مطلقة في المفسر والمترجم" if not bad else "")
            gates_ok = gates_ok and not bad
        else:
            gates_ok = False
    else:
        print("\n③ التنفيذ المزدوج: (تخطٍّ — مرّر --run)")

    print("\n" + "═" * 70)
    print("  ✅ نجحت كل البوّابات" if gates_ok else "  ❌ فشلت بوّابة أو أكثر")
    return 0 if gates_ok else 1


if __name__ == "__main__":
    sys.exit(main())
