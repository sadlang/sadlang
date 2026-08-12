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
# (AR) منطقة الاختبارات الكاشفة للثغرات — منفصلة عن البوّابات (لا تُفشِل البناء)
GAPS_DIR = ROOT / "tests" / "behavior" / "grammar_gaps"
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
            # (AR) as_posix لا str: هذا الحقلُ يُكتَب حرفيًّا في CONFORMANCE_REPORT_detail.md
            #      (٢٩١٥ سطرًا) وهو أيضًا مفتاحُ الفرزِ الذي يرتّب جداولَه. وstr يُعطي
            #      `\` على ويندوز و`/` على لينكس، فينجرف الملفُّ المُودَعُ — ويتبدّل
            #      ترتيبُ صفوفِه معه — عند كلِّ توليدٍ على منصّةٍ مغايرة.
            # (EN) as_posix, not str: this field is written verbatim into the committed
            #      detail report and is also its sort key; str churns both across OSes.
            "rel": rel.as_posix(),
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

def run_dual_execution(args, subdir: str = "rules_matrix") -> dict:
    """(AR) يُشغّل runner.py على مجلد فرعي ويُرجع تقرير JSON المُحلَّل."""
    cmd = [sys.executable, str(RUNNER), "--dir", subdir, "--report"]
    if args.interpreter:
        cmd += ["--interp", args.interpreter]   # (AR) runner.py يستخدم --interp لا --interpreter
    if args.compiler:
        cmd += ["--compiler", args.compiler]
    print(f"▶ {' '.join(cmd)}\n")
    proc = subprocess.run(cmd, cwd=str(ROOT))
    if not DUAL_REPORT.exists():
        print(f"❌ لم يُنتَج تقرير: {DUAL_REPORT} (exit={proc.returncode})", file=sys.stderr)
        return {}
    تقرير = json.load(open(DUAL_REPORT, encoding="utf-8"))
    # (AR) العدّاءُ يكتب دائمًا إلى الاسمِ نفسِه، فتشغيلةُ الثغراتِ كانت تدهس شاهدَ
    #      تشغيلةِ العيّنات: بعد الانتهاء لا يبقى على القرصِ ما يُراجَع به التقريرُ
    #      المُودَع (١١٠ صفًّا مكانَ ٢٩٢٨). فتُحفَظ نسخةٌ باسمِ المجلّدِ لكلِّ تشغيلة.
    # (EN) The runner always writes the same filename, so the gaps run clobbered the
    #      fixtures run — keep a per-subdir copy so the audit trail survives.
    نسخة = DUAL_REPORT.with_name(f"_dual_report_{subdir.replace('/', '_')}.json")
    نسخة.write_text(json.dumps(تقرير, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"   ▸ شاهدٌ محفوظ: {نسخة.relative_to(ROOT).as_posix()}")
    return تقرير


def _خليّة(نصّ: str, حدّ: int = 0) -> str:
    """(AR) نصٌّ صالحٌ لخليّةِ جدولِ ماركداون.

    (AR) `|` غيرُ مهروبٍ يكسر الصفَّ إلى أعمدةٍ زائدةٍ فيُعرَض مشوَّهًا — وقارئُ الحارسِ
         يفحص العمودَ الأوّلَ فيمرّ الصفُّ المكسورُ أخضرَ. ولم يكن هروبٌ في أيٍّ من
         الخلايا الثلاثِ (مخرَجا التباعداتِ · سببُ التخطّي · وسمُ الثغرة)، والشاهدُ
         مُودَعٌ لا مفترَض: `CONFORMANCE_REPORT.md` عند 570b2692 يحمل الصفَّ
         `| تحقّق — أنبوب |> |` بستّةِ أعمدةٍ بدل خمسة، ومرّ أخضرَ.
    (AR) والحدُّ يُطبَّق قبلَ الهروب: القطعُ بعدَه قد يشطر `\\|` فيترك شرطةً معلّقةً،
         ويجعل الحدَّ الفعّالَ نصفَه لمخرَجٍ مملوءٍ بالأنابيب.
    (AR) والعلامةُ الخلفيّةُ تُبدَّل: الخليّةُ تُكتَب داخلَ `` ` ``، فمخرَجُ تشخيصٍ فيه
         علامةٌ خلفيّةٌ يُنهي المدى مبكّرًا — صنفُ عطبِ الأنبوبِ نفسُه في محرفٍ آخر،
         ولا يراه قارئُ الحارسِ كذلك لأنّه يفحص العمودَ الأوّلَ وحدَه. ولا تُهرَّب
         بشرطةٍ خلفيّةٍ (ماركداونُ لا يعرف ذلك داخلَ مدى شفرة) بل تُبدَّل بنظيرِها
         المرئيِّ ‹ʻ› فيبقى الصفُّ سليمًا والنصُّ مقروءًا.
    (EN) One safe-cell helper for every table: escaping one cell and forgetting its
         neighbour is exactly how a broken row reaches the reader. Truncate first,
         escape second — the reverse can split an escape pair.
    """
    نصّ = (نصّ or "").replace("\r\n", "⏎").replace("\n", "⏎").replace("\r", "⏎")
    if حدّ:
        نصّ = نصّ[:حدّ]
    return نصّ.replace("|", r"\|").replace("`", "ʻ")


def _rel_of_entry(entry: dict) -> str | None:
    """(AR) مسارُ صفٍّ من تقرير العدّاء نسبةً إلى rules_matrix بصيغةِ posix — أو None.

    (AR) لماذا لا الاسمُ المجرَّد: كان الوصلُ `Path(file).name`، وفي الشجرةِ ٨٢ اسمًا
         مكرَّرًا تغطّي ٢٧٩ ملفًّا (`033_extra_33.ص` في تسعةِ مجلّداتٍ مختلفة). فكانت
         تسعةُ صفوفٍ تحمل حالةَ وزمنَ صفٍّ واحدٍ منها — قِيسَ ذلك: ٨٢/٨٢ مجموعةً
         أعضاؤها بحالةٍ وزمنَين متطابقَين بايتيًّا، وهي استحالةٌ إحصائيّة. فكان
         ٩٫٥٪ من التقريرِ شهادةً منقولةً لا مقيسة، وانقلابُ حالةٍ في أيٍّ منها مكتومٌ
         بالبناء.
    (EN) Join by relative path, not bare filename: 82 duplicate names cover 279 files,
         so name-keying silently copied one sibling's verdict onto all of them.
    """
    try:
        return Path(entry["file"]).resolve().relative_to(RULES_MATRIX_DIR).as_posix()
    except (ValueError, OSError, KeyError):
        return None


def _index_by_rel(tests: list[dict]) -> dict:
    """(AR) فهرسُ نتائجِ العدّاء بالمسارِ النسبيّ — ويُخفِق بصوتٍ إن تعذّر وصلُ صفّ."""
    out: dict = {}
    يتيمة = []
    for e in tests:
        rel = _rel_of_entry(e)
        if rel is None:
            يتيمة.append(e.get("file", "?"))
            continue
        out[rel] = e
    مبتلَعة = len(tests) - len(يتيمة) - len(out)
    if مبتلَعة:
        # (AR) الاتّجاهُ المقابلُ للعلّةِ المُصلَحة، وكان صامتًا تمامًا: مسارٌ نسبيٌّ
        #      مكرَّرٌ يجعل صفًّا يدهس صفًّا فيختفي أحدُهما بلا أثر.
        print(f"  ⚠️ {مبتلَعة} صفًّا في تقرير العدّاء دهسه صفٌّ بنفسِ المسارِ النسبيّ",
              file=sys.stderr)
    if يتيمة:
        # (AR) صمتُ الوصلِ هو العلّةُ التي عولجت هنا، فلا يُستبدَل بصمتٍ آخر.
        print(f"  ⚠️ {len(يتيمة)} صفًّا في تقرير العدّاء تعذّر وصلُه بشجرةِ العيّنات "
              f"(أوّلها: {يتيمة[0]})", file=sys.stderr)
    return out


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
    by_rel = _index_by_rel(report.get("tests", []))
    rule_results: dict = defaultdict(list)
    for rec in records:
        entry = by_rel.get(rec["rel"])
        res = {"test": rec["rel"], "category": rec["category"]}
        if entry is None:
            res.update({"interpreter": "?", "compiler": "?", "status": "NOT_RUN"})
        else:
            res.update(_support(entry["status"], entry.get("mode", "dual_parity")))
            # (AR) الأزمنةُ خرجت من التقريرَين المُودَعَين (بيانٌ لا يُعاد إنتاجُه)،
            #      فمقرُّها هنا: أثرُ بناءٍ غيرُ متعقَّبٍ يُقرأ لتحليلِ الأداء. حذفُها
            #      من المُودَعِ دون إثباتِها هنا كان سيكون فقدًا لا نقلًا.
            res["interp_ms"] = entry.get("interp_time_ms", 0)
            res["compiler_ms"] = entry.get("compiler_time_ms", 0)
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
        "source_report": DUAL_REPORT.relative_to(ROOT).as_posix(),
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


_RE_GAP_TAG = re.compile(r"^#\s*@gap:?\s+(.+)$")


def _gap_tag(filepath: Path) -> str:
    """(AR) يقرأ وسم @gap من ملف اختبار كاشف."""
    try:
        for i, line in enumerate(open(filepath, encoding="utf-8")):
            if i >= 30:
                break
            m = _RE_GAP_TAG.match(line.strip())
            if m:
                return m.group(1).strip()
    except OSError:
        pass
    return "—"


def _report_path(raw: str) -> str:
    """(AR) يُصيّر مسارَ اختبارٍ نسبيًّا للجذر قبل كتابته في تقريرٍ يُودَع.

    مصدرُ `file` تقريرُ العدّاء، وقد يأتي مطلقًا أو نسبيًّا بحسب كيفيّةِ استدعائه.
    والمطلقُ يُسرِّب بنيةَ قرصِ المطوّرِ إلى مصدرِ الحقيقة، ويجعل التقريرَ غيرَ
    قابلٍ لإعادةِ الإنتاجِ على آلةٍ أخرى — وقد وقع ذلك فعلًا في جدول «المتخطّى».

    والفاصلُ يُوحَّد إلى `/` بـas_posix: النسبيَّةُ وحدَها لا تكفي، إذ يكتب ويندوز
    `tests\\behavior\\…` ولينكس `tests/behavior/…` فينجرف الملفُّ المُودَعُ بينهما
    عند كلِّ توليدٍ — وهو عينُ عطبِ «لا يُعاد إنتاجُه على آلةٍ أخرى» المقصودِ إصلاحُه.
    (EN) Render a test path relative to the repo root, with POSIX separators —
    relativizing alone still churns the committed file between Windows and Linux.
    """
    if not raw:
        # (AR) الفارغُ يُصيّره Path نقطةً، فيُطبَع «.» بدل أن يظهر الحقلُ ناقصًا
        # (EN) Path('') resolves to '.', hiding a missing field behind a plausible value
        return raw
    try:
        return Path(raw).resolve().relative_to(ROOT).as_posix()
    except (ValueError, OSError):
        return raw.replace("\\", "/")


def write_markdown(matrix: dict, counts: dict, report: dict, records: list[dict],
                   out_path: Path, gaps_report: dict | None = None) -> None:
    """(AR) يكتب تقريرين: ملخّص (out_path) + تفصيل لكل اختبار (out_path_detail.md)
    يُظهران مقارنة المفسر بالمترجم، الأزمنة، أسماء الاختبارات، والتباعدات.
    (EN) Writes a summary report + a per-test detail report (names, times, status).
    """
    import time
    tests = report.get("tests", [])
    by_rel = _index_by_rel(tests)
    total = len(tests)
    passed = sum(1 for e in tests if e["status"] == "PASS")
    # (AR) أزمنة التنفيذ الكلية
    tot_interp = sum(e.get("interp_time_ms", 0) for e in tests)
    tot_comp = sum(e.get("compiler_time_ms", 0) for e in tests)
    # (AR) جمع كل التباعدات/الإخفاقات (هنا «نعرف الاختلافات لنصحّحها»)
    diffs = [e for e in tests if e["status"] not in ("PASS", "SKIP")]
    # (AR) المتخطّى يُعَدّ صراحةً: كان يسقط من العدّادين معاً (لا في «تطابق» ولا في
    #      «تباعد») فيُقرأ الملخّصُ أخضرَ وفيه فجوةٌ غيرُ محسوبة. وبهذا صار
    #      المجموعُ مغلقًا: تطابقٌ + تباعدٌ + متخطًّى = الإجمالي.
    # (EN) Skips are counted explicitly: they used to fall out of both counters,
    #      so the summary read green with an uncounted hole. The three counters
    #      now close over the total.
    skipped = [e for e in tests if e["status"] == "SKIP"]

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
    L.append(f"- إجمالي الاختبارات: **{total}** — تطابق مزدوج: **{passed}** — "
             f"تباعد/إخفاق: **{len(diffs)}** — متخطًّى: **{len(skipped)}**")
    sm = {v: sum(1 for m in matrix.values() if m["verdict"] == v)
          for v in ("dual_ok", "compiler_gap", "interp_only", "broken", "no_tests", "not_run")}
    # (AR) كلُّ حُكمٍ يُطبَع ولو كان صفرًا لولا أنّ حذفَه يُخفي قاعدةً محسوبةً في
    #      المقام دونَ بسطٍ يُظهرها؛ فالمعفاةُ و«لم تُشغَّل» تظهران عند وجودهما.
    # (EN) Verdicts that used to be omitted are shown when nonzero, so a rule
    #      cannot sit in the denominator without appearing in any numerator.
    extra = "".join(f" · {ar}: {sm[k]}" for k, ar in
                    (("interp_only", "مُعفاة (مفسر فقط)"), ("not_run", "لم تُشغَّل")) if sm[k])
    L.append(f"- القواعد: {len(matrix)} — مطلقة: **{sm['dual_ok']}** · "
             f"فجوة مترجم: {sm['compiler_gap']} · مكسورة: {sm['broken']} · "
             f"بلا اختبارات: {sm['no_tests']}" + extra)
    # (AR) لا زمنَ في المُودَع — لا هنا ولا في الجداول: العدّاءُ يتوازى فالرقمُ دالّةُ
    #      ازدحامٍ لا دالّةُ اختبار. الأزمنةُ كلُّها في أثرِ البناءِ غيرِ المتعقَّب.
    L.append("- الأزمنة: في أثر البناء `build/_grammar_conformance.json` (غير متعقَّب) —"
             " لا تُودَع لأنّ العدّاء يتوازى فلا يُعاد إنتاجها.")
    L.append(f"- التفصيل الكامل لكل اختبار: [`{out_path.stem}_detail.md`](./{out_path.stem}_detail.md)")
    L.append("")
    # ── قسم التباعدات (الأهم: «كيف نعرف الاختلافات لنصحّحها») ──
    L.append("## التباعدات والإخفاقات (للتصحيح)")
    L.append("")
    if not diffs:
        L.append(f"✅ **لا تباعد** — كل اختبار **شُغِّل** أعطى مخرجاً متطابقاً في المفسر والمترجم"
                 + (f" (و**{len(skipped)}** لم يُشغَّل — انظر «المتخطّى» أدناه)." if skipped else "."))
    else:
        L.append("| الاختبار | الحالة | مخرج المفسر | مخرج المترجم |")
        L.append("|---|---|---|---|")
        # (AR) وهذا الجدولُ لم يكن مفروزًا إطلاقًا: يرث ترتيبَ تقريرِ العدّاء. وهو
        #      فارغٌ اليومَ فبرهانُ «قابلٍ لإعادةِ الإنتاج» لم يمسَّه — أي أنّ أهمَّ
        #      جداولِ التقريرِ (ما يُقرأ حين يقع خطب) هو أقلُّها اختبارًا. يُفرَز
        #      بالمفتاحِ نفسِه فيصير الجدولُ الوحيدُ المتبقّي حتميًّا كذلك.
        # (EN) The diffs table was never sorted; empty today, so the reproducibility
        #      proof never touched the one table that matters when something breaks.
        for e in sorted(diffs, key=lambda x: _report_path(x["file"])):
            io = _خليّة(e.get("interp_output", ""), 50)
            co = _خليّة(e.get("compiler_output", ""), 50)
            L.append(f"| `{_report_path(e['file'])}` | "
                     f"{_STATUS_AR.get(e['status'], e['status'])} | `{io}` | `{co}` |")
    L.append("")
    if skipped:
        L.append("## المتخطّى (لم يُشغَّل — غيرُ مقيسٍ لا ناجح)")
        L.append("")
        L.append("| الاختبار | سببُ التخطّي |")
        L.append("|---|---|")
        # (AR) الفرزُ بمسارٍ نسبيٍّ پوسكسيّ لا بالمطلقِ بفواصلِ المنصّة: المطلقُ يجعل
        #      ترتيبَ صفوفِ ملفٍّ مُودَعٍ دالّةَ نظامِ التشغيل. (لا فرقَ اليومَ — قِيسَ
        #      أنّ الترتيبَين متطابقان للـ٢٩٢٨ والـ١١٠ — وهذا إغلاقٌ وقائيّ.)
        for e in sorted(skipped, key=lambda x: _report_path(x["file"])):
            L.append(f"| `{_report_path(e['file'])}` | {_خليّة(e.get('error', '')) or '—'} |")
    L.append("")
    # ── قسم الاختبارات الكاشفة للثغرات (غير مُبوَّب) ──
    if gaps_report is not None:
        gtests = gaps_report.get("tests", [])
        gfail = [e for e in gtests if e["status"] != "PASS"]
        L.append("## اختبارات كاشفة للثغرات (Gaps) — غير مُبوَّبة (لا تُفشِل البناء)")
        L.append("")
        L.append(f"تتعمّد اختبار ميزات مشكوكة لكشف ما لا يعمل. كاشفة: **{len(gtests)}** — "
                 f"تكشف ثغرة: **{len(gfail)}**. (راجع [DISCOVERED_ISSUES.md](./DISCOVERED_ISSUES.md))")
        L.append("")
        L.append("| الاختبار | الثغرة | النتيجة |")
        L.append("|---|---|---|")
        for e in sorted(gtests, key=lambda x: _report_path(x["file"])):
            # (AR) ابحث عن الملف فعلياً (قد يكون في مجلد ثيمة فرعي) لقراءة وسم @gap.
            #      وهذا وصلٌ بالاسمِ المجرَّدِ عن قصد: صفُّ الجدولِ يعرض الاسمَ العاريَ
            #      كذلك، فتفرُّدُ أسماءِ grammar_gaps شرطٌ لا رفاهية — ويفرضه صراحةً
            #      check_conformance_report_fresh.py فيُحمِّر عند أوّلِ تكرار. فإن سقط
            #      ذلك الحارسُ يومًا وجب تحويلُ هذا الوصلِ إلى المسارِ كما في الأعلى.
            matches = list(GAPS_DIR.rglob(Path(e["file"]).name))
            tag = _خليّة(_gap_tag(matches[0])) if matches else "—"
            verdict = "تعمل ✅" if e["status"] == "PASS" else f"تكشف ثغرة ❌ ({_STATUS_AR.get(e['status'], e['status'])})"
            L.append(f"| `{Path(e['file']).name}` | {tag} | {verdict} |")
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
    D.append("> القاعدة، الفئة، الحالة (نتيجة مقارنة المفسر بالمترجم).")
    D.append(">")
    # (AR) لا عمودَ زمنٍ هنا عن قصد: قِيسَ أنّ ٢٩١٠ صفًّا من ٢٩١٢ (٩٩٫٩٪) كان يتبدّل
    #      فيها الزمنُ وحدَه بين توليدَين متتاليَين، بوسيطِ نسبةٍ ١٫٢٣× ومدًى
    #      ٠٫٥×–٣٫٣٧×، لأنّ العدّاءَ يتوازى على أنويةِ الآلة. فالعمودُ لم يكن يقيس
    #      الاختبارَ بل ازدحامَ الآلةِ ساعةَ القياس: بيانٌ لا يُعاد إنتاجُه في ملفٍّ
    #      يشهد ويُستشهَد به، وثمنُه ~٢٩٠٠ سطرِ فرقٍ عند كلِّ توليدٍ وتضاربُ دمجٍ
    #      مضمونٌ لأيِّ فرعَين يُوَلِّدان. الأزمنةُ لم تُحذَف: هي في أثرِ البناءِ
    #      build/_grammar_conformance.json حيث تنتمي — غيرِ متعقَّبٍ ولا مُستشهَدٍ به.
    # (EN) No timing column on purpose: 99.9% of rows churned on time alone because
    #      the runner parallelizes. Timings live in the untracked build evidence.
    D.append("> الأزمنة في أثر البناء `build/_grammar_conformance.json` (غير متعقَّب):")
    D.append("> عمودُ زمنٍ هنا يتبدّل في ٩٩٫٩٪ من الصفوف عند كلِّ توليد لأنّ العدّاء")
    D.append("> يتوازى — فهو يقيس ازدحامَ الآلة لا الاختبار.")
    D.append("")
    D.append("| # | الاختبار | القاعدة | الفئة | الحالة |")
    D.append("|---|---|---|---|---|")
    for i, rec in enumerate(sorted(records, key=lambda r: r["rel"]), start=1):
        e = by_rel.get(rec["rel"], {})
        rid = rec.get("folder_rule") or (rec["rule_ids"][0] if rec["rule_ids"] else "—")
        st = _STATUS_AR.get(e.get("status", "NOT_RUN"), e.get("status", "؟"))
        D.append(f"| {i} | `{rec['rel']}` | `{rid}` | {rec['category']} | {st} |")
    D.append("")
    detail_path = out_path.with_name(out_path.stem + "_detail.md")
    detail_path.write_text("\n".join(D), encoding="utf-8")
    print(f"📄 تفصيل كل اختبار: {detail_path}")


# ═══════════════════════════════════════════════════════════════════════════════════
# ⑤ نقطة الدخول
# ═══════════════════════════════════════════════════════════════════════════════════

def check_pairwise_coverage():
    """(AR) البوّابة ④: تغطية مصفوفة التداخل الثنائية. تُفوّض كامل حساب الأزواج «القابلة
    للتركيب نحويًا» إلى gen_rules_matrix (مصدر واحد لرسم التركيب — يحمّل حقل refs).
    (EN) Gate ④: pairwise interaction coverage. Fully delegates to gen_rules_matrix,
    which loads the `refs` field needed for the composability graph."""
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import gen_rules_matrix as grm
    prods = grm.load_productions()
    recs = grm.scan_tests()
    required, _containment, _conest = grm.build_required_pairs(prods)
    _counts, covered = grm.measure_coverage(prods, recs)
    missing = sorted(required - covered, key=lambda p: sorted(p))
    return missing, required, (covered & required)


def main() -> int:
    _utf8_console()
    ap = argparse.ArgumentParser(description="الفاحص الشامل لمطابقة قواعد لغة ص")
    ap.add_argument("--run", action="store_true", help="تشغيل التنفيذ المزدوج + توليد الأدلة")
    ap.add_argument("--interpreter", help="مسار مفسر مخصص")
    ap.add_argument("--compiler", help="مسار مترجم مخصص")
    ap.add_argument("--report-md", dest="report_md",
                    help="مسار كتابة تقرير Markdown مقروء (مقارنة المفسر/المترجم + التباعدات)")
    ap.add_argument("--pairs-gate", action="store_true",
                    help="اجعل نقص تغطية أزواج التداخل يُفشِل البوّابة (افتراضيًا: تحذير فقط)")
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

    # ── البوّابة ④: تغطية مصفوفة التداخل الثنائية (الأزواج القابلة للتركيب نحويًا) ──
    missing_pairs, required_pairs, covered_pairs = check_pairwise_coverage()
    print(f"\n④ تغطية الأزواج (تداخل قاعدتين قابلتين للتركيب نحويًا):")
    print(f"  مُغطّى {len(covered_pairs)}/{len(required_pairs)} — ناقص {len(missing_pairs)}"
          + ("" if not missing_pairs else "  (أوّل 10:)"))
    for pair in missing_pairs[:10]:
        a, b = sorted(pair)
        print(f"    ✗ {a} × {b}")
    if missing_pairs and not args.pairs_gate:
        print("  ⚠️ تحذير فقط (مرّر --pairs-gate لجعلها تُفشِل البناء)")

    gates_ok = not cov_err and not link_err
    if args.pairs_gate and missing_pairs:
        gates_ok = False

    if args.run:
        print("\n③ التنفيذ المزدوج (مفسر ≡ مترجم):")
        report = run_dual_execution(args)
        if report:
            matrix = derive_matrix(productions, records, report)
            write_evidence(matrix, counts)
            # (AR) تشغيل الاختبارات الكاشفة للثغرات منفصلاً (غير مُبوَّب)
            gaps_report = None
            if GAPS_DIR.is_dir():
                print("\n   ▸ تشغيل الاختبارات الكاشفة للثغرات (grammar_gaps)...")
                gaps_report = run_dual_execution(args, "grammar_gaps") or None
            if args.report_md:
                write_markdown(matrix, counts, report, records, Path(args.report_md), gaps_report)
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
