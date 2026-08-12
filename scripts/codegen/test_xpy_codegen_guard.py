"""
============================================================================
(AR) test_xpy_codegen_guard.py — اختبارات حارس انجراف المصدر المولَّد
     (المرحلة 1 من sadlang-rfcs#10). يثبّت عقد `x.py gen --check`:
       - جدول النطاقات يغطّي الملفّات المولَّدة بالضبط (24 ملفًّا)، وكل مولّد موجود.
       - جدول حرّاس الفحص الخالصين (SOT_CHECK_GUARDS) سكربتاته موجودة.
       - منطق القرار (تطبيع نهايات الأسطر) يتجاهل CRLF↔LF ويلتقط فرق المحتوى.
       - الحارس يمرّ على الشجرة النظيفة (تكامل: وسائط الجدول صحيحة + لا انحراف).
(EN) Tests for the generated-source drift guard (Phase 1 of sadlang-rfcs#10).
     Pins the `x.py gen --check` contract: domain table covers exactly the six
     generated files, the EOL-normalizing decision predicate ignores CRLF↔LF but
     catches content drift, and the guard passes on a clean tree end-to-end.
============================================================================
"""
from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]


def _load_xpy():
    """(AR) يحمّل x.py من جذر المستودع كوحدة. / Load root-level x.py as a module."""
    spec = importlib.util.spec_from_file_location("xpy_under_test", ROOT / "x.py")
    assert spec and spec.loader, "تعذّر تحميل x.py"
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


xpy = _load_xpy()

# (AR) الملفّات المولَّدة التي يحرسها x.py gen --check (مصدر الحقيقة = YAML).
#   الأربعة الأصليّة (types/keywords/builtins/errors) + نطاقات الواجهة الستّة
#   (ui_props/ui_modifiers + مفردات animations/easings/events + colors).
EXPECTED_OUTPUTS = {
    "shared/types/generated/sad_type_kind_generated.h",
    "shared/lexer/generated/keywords_generated.h",
    "shared/lexer/generated/keywords_generated.cpp",
    "shared/builtins/generated/builtin_registry_generated.h",
    "shared/errors/generated/error_messages_generated.h",
    "shared/errors/generated/error_messages_generated.cpp",
    "features/graphics/core/include/sad_ui/prop_keys.h",
    "features/graphics/core/include/sad_ui/ui_modifiers.h",
    "features/graphics/core/include/sad_ui/generated/animation_vocab_generated.h",
    "features/graphics/core/include/sad_ui/generated/easing_vocab_generated.h",
    "features/graphics/core/include/sad_ui/generated/event_vocab_generated.h",
    "features/graphics/core/include/sad_ui/generated/propagation_vocab_generated.h",
    "features/graphics/core/include/sad_ui/generated/color_prelude_generated.h",
    "features/graphics/core/include/sad_ui/generated/color_table_generated.h",
    # (AR) أنواعُ عُقَدِ الواجهة — قائمتا X-macro من language-truth/ui_nodes.yaml
    #      يُبنى منهما تعدادُ UINodeType وجدولا الاسم داخلَ مكتبةِ الرسومات.
    # (EN) UI node types — X-macro lists from ui_nodes.yaml; build the UINodeType
    #      enum and both name tables inside the graphics library.
    "features/graphics/core/include/sad_ui/generated/node_types_generated.h",
    # (AR) مفرداتُ لغةِ ص كما يراها المُلوِّنُ النحويُّ في «كتلة_كود» — من
    #      keywords.yaml وbuiltins/core.yaml. رأسٌ **ذاتيُّ الاكتفاء** عمدًا:
    #      مكتبةُ الرسوماتِ طرفيّةٌ فلا تستهلك رأسَ المعجم (يضمّ token.h).
    # (EN) Highlighter vocabulary — self-contained by design: the graphics leaf
    #      library must not depend on the lexer layer. Same pattern as prop_keys.h.
    "features/graphics/core/include/sad_ui/generated/syntax_keywords_generated.h",
    # (AR) نظيرُه للمحلّل: الأوّليّات/الحاويات/الأحداث — الطبقةُ الأساسُ لا تضمّ
    #      رأسًا من مكتبة الرسومات، فمصدرٌ واحدٌ ومخرَجان.
    # (EN) Parser-side twin (primitives/containers/events) — foundation layer
    #      must not include a graphics header.
    "shared/parser/include/generated/ui_parser_nodes_generated.h",
    # (AR) نظام «مصدر حقيقة الأدوات» — أوّل أداة sad-repl (كتالوج أخطاء/رسائل/أوامر).
    # (EN) Tools' Source-of-Truth — first tool sad-repl (errors/messages/commands catalog).
    "tools/repl/generated/repl_sot_generated.h",
    "tools/repl/generated/repl_sot_generated.cpp",
    # (AR) أعلام واجهة الأوامر — مصدر حقيقة cli_flags.yaml ⇒ رأس C++ + وثيقة عربيّة.
    # (EN) CLI flags — cli_flags.yaml source-of-truth ⇒ C++ header + Arabic doc.
    "shared/cli/include/cli_flags_generated.h",
    "docs/cli_flags/_generated/cli_flags.ar.md",
    # (AR) لهجة التجميع العربيّ — جدول المنمنمات/السجلّات المولَّد من dialects/ (م١ من rfcs#43).
    # (EN) Arabic assembly dialect — mnemonic/register table generated from dialects/ (rfcs#43 M1).
    "shared/dialects/generated/asm_dialect_generated.h",
    # (AR) توثيق قواعد المحلّل — Markdown مولَّد من language-truth/grammar/*.yaml
    #      (8 طبقات + فهرس)، مُلتزَم تحت docs/parser_rule/_generated.
    # (EN) Parser-grammar docs — generated Markdown (8 layers + INDEX) committed
    #      under docs/parser_rule/_generated.
    "docs/parser_rule/_generated/INDEX.md",
    "docs/parser_rule/_generated/00_program.md",
    "docs/parser_rule/_generated/10_statements.md",
    "docs/parser_rule/_generated/20_declarations.md",
    "docs/parser_rule/_generated/30_oop.md",
    "docs/parser_rule/_generated/40_expressions.md",
    "docs/parser_rule/_generated/50_patterns.md",
    "docs/parser_rule/_generated/60_advanced.md",
    "docs/parser_rule/_generated/70_lexical.md",
    # (AR) جدول ترميز الخلفيّة السياديّة x86-64 (عائلة variable) المولَّد من
    #      language-truth/backend/x86_64/instructions.yaml — تحقيقُ «table-driven».
    # (EN) Sovereign backend x86-64 (variable family) encoding table, generated
    #      from the SoT instruction-encoding YAML — the table-driven principle.
    "compiler/include/backend/native/generated/x86_64_encoding_generated.h",
    # (AR) الهدف الثاني: جدول ترميز AArch64 (عائلة fixed32) من نفس المولّد.
    # (EN) Second target: AArch64 (fixed32) encoding table from the same generator.
    "compiler/include/backend/native/generated/arm64_encoding_generated.h",
    # (AR) وسومُ تشخيصِ التخفيض الأصليّ (حمولاتُ {detail}) — ثوابتُ مسمّاةٌ من SoT.
    # (EN) Native-lowering diagnostic tags ({detail} payloads) — SoT named constants.
    "compiler/include/backend/native/generated/native_diagnostics_generated.h",
    # (AR) تمثيلُ القيمة زمنَ التشغيل (وسومُ SadDyn + نصوصُ عرضِ لاشيء/صحيح/خطأ) — مصدرٌ واحدٌ للطبقات الثلاث.
    # (EN) Runtime value representation (SadDyn tags + display texts) — single source for all three engines.
    "shared/types/generated/value_repr_generated.h",
}


def test_domain_table_covers_exactly_the_expected_files():
    covered = {
        f"{d['out_dir']}/{fname}"
        for d in xpy.CODEGEN_DOMAINS
        for fname in d["outputs"]
    }
    assert covered == EXPECTED_OUTPUTS


def test_every_domain_generator_script_exists():
    for d in xpy.CODEGEN_DOMAINS:
        script = ROOT / "scripts" / "codegen" / d["script"]
        assert script.exists(), f"مولّد مفقود / missing generator: {script}"


def test_every_domain_args_reference_only_its_outputs():
    # (AR) وسائط كل نطاق يجب أن تذكر ملفّاته المولَّدة داخل مجلّد الإخراج المُمرَّر —
    #      أو تمرّر مجلّد الإخراج كاملًا (نمط --out-dir حيث يشتقّ المولّد الأسماء
    #      من مصدر الحقيقة نفسه، كنطاق parser_grammar_docs).
    for d in xpy.CODEGEN_DOMAINS:
        args = d["args"]("OUTDIR")
        if "OUTDIR" in args:  # (AR) نمط تمرير المجلّد كاملًا / whole-out-dir style
            continue
        rendered = " ".join(args)
        for fname in d["outputs"]:
            assert f"OUTDIR/{fname}" in rendered, f"{d['name']}: {fname} غير مُمرَّر"


def test_every_check_guard_script_exists():
    # (AR) حرّاس الفحص الخالصون (token_catalog/rules_matrix) سكربتاتهم موجودة.
    # (EN) Pure check-only guards' scripts must exist.
    for g in xpy.SOT_CHECK_GUARDS:
        script = ROOT / "scripts" / "codegen" / g["script"]
        assert script.exists(), f"حارس مفقود / missing guard: {script}"


def test_norm_collapses_crlf_only():
    # (AR) فرق نهايات الأسطر وحده لا يُعدّ انحرافًا (git يعامل الملفّات eol=lf).
    a = b"// header\r\nint x;\r\n"
    b = b"// header\nint x;\n"
    assert xpy._norm(a) == xpy._norm(b)


def test_norm_preserves_content_difference():
    # (AR) فرق المحتوى الحقيقيّ يبقى ظاهرًا بعد التطبيع ⇒ يُكتشَف كانحراف.
    a = b"int x;\n"
    b = b"int y;\n"
    assert xpy._norm(a) != xpy._norm(b)


@pytest.mark.skipif(
    not all((ROOT / p).exists() for p in EXPECTED_OUTPUTS),
    reason="ملفّات مولَّدة غير موجودة في الشجرة",
)
def test_gen_check_passes_on_clean_tree():
    # (AR) تكامل: يعيد توليد النطاقات الأربعة لمؤقّت ويقارن — يجب ألّا يفشل (لا انحراف).
    #      يثبت ضمنًا أنّ وسائط الجدول الأربعة مقبولة من المولّدات وتُنتج الملفّات الستّة.
    try:
        xpy._gen_check()  # noqa: SLF001 — exercising the guard end-to-end
    except SystemExit as exc:  # _fail() ⇒ sys.exit(1) عند الانحراف
        pytest.fail(f"الحارس فشل على شجرة يُفترَض أنّها نظيفة (rc={exc.code})")


# ═══════════════════════════════════════════════════════════════════════════════
# (AR) انحدارٌ: وصلُ نتيجةِ التشغيلِ بصفِّ التقريرِ يكون **بالمسار** لا بالاسم.
# (EN) Regression: the run-result ⟷ report-row join must be by path, not by name.
# ═══════════════════════════════════════════════════════════════════════════════

def _load_conformance():
    """(AR) يحمّل check_grammar_conformance.py وحدةً (يتطلّب pyyaml)."""
    yaml = pytest.importorskip("yaml")  # noqa: F841 — المولّد يستورده عند التحميل
    path = ROOT / "scripts" / "codegen" / "check_grammar_conformance.py"
    spec = importlib.util.spec_from_file_location("conformance_under_test", path)
    assert spec and spec.loader, "تعذّر تحميل check_grammar_conformance.py"
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def test_run_results_join_by_path_not_by_filename():
    """(AR) العلّةُ المقيسة (٢٠٢٦-٠٨-١٢): كان الوصلُ `Path(file).name`، وفي الشجرةِ
    ٨٢ اسمًا مكرَّرًا يغطّي ٢٧٩ ملفًّا (`033_extra_33.ص` في تسعةِ مجلّداتٍ مختلفة).
    فحملت تسعةُ صفوفٍ حالةَ وزمنَ واحدٍ منها — قِيسَ ذلك: ٨٢/٨٢ مجموعةً أعضاؤها
    بحالةٍ وزمنَين متطابقَين بايتيًّا، وهي استحالةٌ إحصائيّة. أي أنّ ٩٫٥٪ من عمودَي
    الحالةِ والزمنِ شهادةٌ منقولةٌ لا مقيسة، وانقلابُ حالةٍ فيها مكتومٌ بالبناء.

    (AR) البذرةُ هنا اسمان متطابقان في مجلّدَين مختلفَين بحالتَين مختلفتَين: الوصلُ
    بالاسمِ يجعلهما صفًّا واحدًا (فيضيع أحدُهما ويُنسَخ الآخر)، والوصلُ بالمسارِ
    يُبقيهما اثنَين. الاختبارُ يفشل على الشيفرةِ ما قبلَ الإصلاح.
    """
    conf = _load_conformance()
    base = conf.RULES_MATRIX_DIR
    مكرَّر = "033_extra_33.ص"
    tests = [
        {"file": str(base / "10_statements" / "gr.stmt.for" / "edge" / مكرَّر),
         "status": "PASS", "mode": "dual_parity",
         "interp_time_ms": 190, "compiler_time_ms": 4256},
        {"file": str(base / "20_declarations" / "gr.decl.function" / "basic" / مكرَّر),
         "status": "FAIL_OUTPUT", "mode": "dual_parity",
         "interp_time_ms": 67, "compiler_time_ms": 3224},
    ]
    فهرس = conf._index_by_rel(tests)  # noqa: SLF001 — الدالّةُ محلُّ الاختبار

    assert len(فهرس) == 2, (
        "الاسمُ المكرَّرُ طوى صفَّين في واحد — الوصلُ عاد إلى الاسمِ المجرَّد")
    أوّل = "10_statements/gr.stmt.for/edge/" + مكرَّر
    ثانٍ = "20_declarations/gr.decl.function/basic/" + مكرَّر
    assert set(فهرس) == {أوّل, ثانٍ}, "المفاتيحُ ليست مساراتٍ نسبيّةً بصيغةِ posix"
    # (AR) والحالةُ لا تُنسَخ من الشقيق: لكلِّ مسارٍ حالتُه وزمنُه هو.
    assert فهرس[أوّل]["status"] == "PASS"
    assert فهرس[ثانٍ]["status"] == "FAIL_OUTPUT"
    assert فهرس[ثانٍ]["compiler_time_ms"] == 3224


def test_unjoinable_run_result_is_reported_not_swallowed(capsys):
    """(AR) صمتُ الوصلِ هو العلّةُ عينُها، فلا يُستبدَل بصمتٍ آخر: صفٌّ خارجَ شجرةِ
    العيّنات يجب أن يُذكَر على stderr لا أن يسقط بلا أثر."""
    conf = _load_conformance()
    فهرس = conf._index_by_rel(  # noqa: SLF001
        [{"file": str(ROOT / "لا" / "وجود" / "له.ص"), "status": "PASS"}])
    assert فهرس == {}
    assert "تعذّر وصلُه" in capsys.readouterr().err


def _سجلّ(rel: str, rule: str, cat: str) -> dict:
    """(AR) سجلُّ عيّنةٍ مصطنعٌ بالشكلِ الذي يُنتجه scan_tests."""
    import pathlib as _pl
    return {"path": _pl.Path(rel), "rel": rel, "name": rel.rsplit("/", 1)[-1],
            "category": cat, "folder_rule": rule, "rule_ids": [rule]}


def test_generator_call_sites_join_by_path_not_by_filename(tmp_path):
    """(AR) يقود المولِّدَ من طرفِه (derive_matrix ثمّ write_markdown) ويحكم على
    الملفِّ المكتوبِ نفسِه، ولا يمسّ الدالّةَ المساعِدةَ إطلاقًا.

    (AR) لماذا لا يكفي اختبارُ `_index_by_rel` وحدَه: أعيدت العلّةُ الأصليّةُ حرفيًّا
    (مواضعُ الاستدعاءِ تعود إلى الوصلِ بالاسمِ والدالّةُ المساعِدةُ تبقى سليمةً بلا
    استدعاء) فمرّ الاختبارُ المساعِدُ **أخضرَ**. أي أنّ حمرتَه كانت غيابَ الدالّةِ لا
    كسرَ الوصل.

    (AR) وهو **مكمِّلٌ لا بديل**: لو صار الوصلُ بآخِرِ مكوّنَين (`فئة/اسم`) لمرَّ هذا
    الاختبارُ أخضرَ — وهي علّةٌ حيّةٌ لا فرضيّة: ٨٤ تصادُمَ (فئة/اسم) تغطّي ٢٥٠ ملفًّا
    في الشجرة. يمسكها الاختبارُ المساعِدُ وحدَه بتوكيدِ **شكلِ المفتاح**. فالاثنان
    يقيسان طبقتَين مختلفتَين ولا يُغني أحدُهما عن الآخر.
    """
    conf = _load_conformance()
    مكرَّر = "033_extra_33.ص"
    أوّل = f"10_statements/gr.stmt.for/edge/{مكرَّر}"
    ثانٍ = f"20_declarations/gr.decl.function/basic/{مكرَّر}"
    records = [_سجلّ(أوّل, "gr.stmt.for", "edge"), _سجلّ(ثانٍ, "gr.decl.function", "basic")]
    base = conf.RULES_MATRIX_DIR
    report = {"tests": [
        # (AR) `base / "أ/ب"` يفصل بفواصلِ المنصّةِ من نفسِه. وكان هنا
        #      `.replace("/", "\\")` فرضًا لفواصلِ ويندوز — وهي على لينكس محارفُ
        #      اسمٍ عاديّةٌ لا فواصل، فيصير المسارُ مكوّنًا واحدًا لا يطابق `rel`
        #      أبدًا ⇒ `not_run` ⇒ الاختبارُ أحمرُ على CI (ubuntu) وحدَها.
        {"file": str(base / أوّل), "status": "PASS",
         "mode": "dual_parity", "interp_time_ms": 190, "compiler_time_ms": 4256},
        {"file": str(base / ثانٍ), "status": "FAIL_OUTPUT",
         "mode": "dual_parity", "interp_time_ms": 67, "compiler_time_ms": 3224,
         "interp_output": "أ", "compiler_output": "ب"},
    ]}
    productions = {r: {"layer": "core", "compiler_optional": False, "budget": {},
                       "file": "اختبار.yaml"}
                   for r in ("gr.stmt.for", "gr.decl.function")}

    # ── ① derive_matrix: لكلِّ قاعدةٍ حكمُها هي، لا حكمُ شقيقتِها بالاسم ──
    matrix = conf.derive_matrix(productions, records, report)
    assert matrix["gr.stmt.for"]["verdict"] == "dual_ok", (
        "القاعدةُ السليمةُ ورثت إخفاقَ شقيقتِها بالاسم — الوصلُ عاد إلى الاسمِ المجرَّد")
    assert matrix["gr.decl.function"]["verdict"] == "broken", (
        "الإخفاقُ ابتُلع: القاعدةُ المكسورةُ ورثت نجاحَ شقيقتِها بالاسم")

    # ── ② الأزمنةُ خرجت من المُودَعِ فمقرُّها أثرُ البناء: تُحرَس حيث صارت ──
    نتائج = {ن["test"]: ن for ن in matrix["gr.stmt.for"]["tests"]
             + matrix["gr.decl.function"]["tests"]}
    assert نتائج[أوّل]["interp_ms"] == 190 and نتائج[أوّل]["compiler_ms"] == 4256
    assert نتائج[ثانٍ]["interp_ms"] == 67 and نتائج[ثانٍ]["compiler_ms"] == 3224, (
        "الأزمنةُ فُقِدت بإخراجِها من التقرير بدل أن تُنقَل إلى أثرِ البناء")

    # ── ③ write_markdown: الصفّان في الملفِّ المكتوبِ يحملان حالتَين مختلفتَين ──
    import re
    conf.write_markdown(matrix, {}, report, records, tmp_path / "تقرير.md")
    نصّ = (tmp_path / "تقرير_detail.md").read_text(encoding="utf-8")
    نمط = re.compile(r"^\|\s*\d+\s*\|\s*`([^`]+)`\s*\|[^|]*\|[^|]*\|\s*([^|]+?)\s*\|")
    صفوف = {m.group(1): m.group(2) for m in map(نمط.match, نصّ.splitlines()) if m}
    assert set(صفوف) == {أوّل, ثانٍ}, f"صفوفُ التفصيلِ ليست مسارَين متمايزَين: {set(صفوف)}"
    assert صفوف[أوّل] != صفوف[ثانٍ], (
        "صفّان لملفَّين مختلفَين بنفسِ الحالة — شهادةٌ منقولةٌ لا مقيسة")
    assert "تطابق" in صفوف[أوّل] and "تباعد" in صفوف[ثانٍ]
    # (AR) ولا عمودَ زمنٍ في المكتوب: الرَّتشُ يُقاس في المولِّدِ لا في المُودَعِ وحدَه.
    assert "(ms)" not in نصّ, "عمودُ زمنٍ عاد إلى التقريرِ التفصيليّ"


def test_safe_cell_escapes_pipes_and_all_newlines():
    """(AR) `_خليّة` هي العلاجُ الوحيدُ لصفٍّ يكسر جدولَه — ولا يراه أيُّ حارسٍ آخر:
    قارئُ check_conformance_report_fresh.py يفحص العمودَ الأوّلَ وحدَه، فالصفُّ ذو
    الأعمدةِ الستّةِ مرّ أخضرَ فعلًا في التقريرِ المُودَعِ عند 570b2692.
    """
    conf = _load_conformance()
    خليّة = getattr(conf, "_خليّة")

    assert خليّة("تحقّق — أنبوب |>") == r"تحقّق — أنبوب \|>"
    assert خليّة("أ|ب|ج").count("|") == خليّة("أ|ب|ج").count(r"\|") == 2
    # (AR) الأشكالُ الثلاثةُ لنهايةِ السطر — و`\r` وحدَه نهايةُ سطرٍ عند بعضِ المُصيّرات
    assert خليّة("سطر\r\nتالٍ") == خليّة("سطر\nتالٍ") == خليّة("سطر\rتالٍ") == "سطر⏎تالٍ"
    assert خليّة("") == "" and خليّة(None) == ""
    # (AR) والعلامةُ الخلفيّةُ تُنهي مدى الشفرةِ الذي يحيط بالخليّة — عطبٌ من صنفِ
    #      الأنبوبِ نفسِه، ولا يراه قارئُ الحارسِ (يفحص العمودَ الأوّلَ وحدَه).
    assert "`" not in خليّة("خطأ في `الرمز`") and "الرمز" in خليّة("خطأ في `الرمز`")
    # (AR) القطعُ قبلَ الهروب: لا شرطةَ معلّقةً، والحدُّ يَعُدُّ محارفَ المصدرِ لا الهروب
    مقطوعة = خليّة("|" * 40, 10)
    assert مقطوعة == r"\|" * 10, "القطعُ بعدَ الهروب: شطر زوجًا أو أنقص الحدَّ إلى نصفِه"


def test_free_text_cells_go_through_the_safe_cell():
    """(AR) بذرةُ «تهرب خليّةً وتنسى جارتَها»: المواضعُ الأربعةُ التي تكتب **نصًّا
    حرًّا** في خليّةٍ يجب أن تمرّ بـ`_خليّة`. الفحصُ نصّيٌّ عمدًا — الأثرُ لا يظهر إلّا
    عند أوّلِ تباعدٍ أو تخطٍّ بنصٍّ فيه `|`، وهو ما لا يقع في الشجرةِ الخضراءِ اليوم.

    (AR) ولا يغطّي الخلايا التي مصدرُها ليس نصًّا حرًّا: المسارُ والقاعدةُ والفئةُ
    والحالةُ في التفصيليّ، واسمُ ملفِّ الثغرة، والقاعدةُ والطبقةُ في جدولِ القواعد —
    مصادرُها أسماءُ ملفّاتٍ وقيمُ YAML وثوابتُ الحالات، لا مخرَجُ برنامج. فالاسمُ
    يقول «نصٌّ حرّ» لا «كلُّ خليّة».
    """
    مصدر = (ROOT / "scripts" / "codegen" / "check_grammar_conformance.py").read_text(
        encoding="utf-8")
    for حقل in ("interp_output", "compiler_output"):
        assert f'_خليّة(e.get("{حقل}"' in مصدر, f"خليّةُ {حقل} تكتب نصًّا خامًّا"
    assert "_خليّة(e.get('error'" in مصدر, "خليّةُ سببِ التخطّي تكتب نصًّا خامًّا"
    assert "_خليّة(_gap_tag(" in مصدر, "خليّةُ وسمِ الثغرةِ تكتب نصًّا خامًّا"


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-v"]))
