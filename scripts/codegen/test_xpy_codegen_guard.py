"""
============================================================================
(AR) test_xpy_codegen_guard.py — اختبارات حارس انجراف المصدر المولَّد
     (المرحلة 1 من sadlang-rfcs#10). يثبّت عقد `x.py gen --check`:
       - جدول النطاقات يغطّي الملفّات المولَّدة بالضبط (13 ملفًّا)، وكل مولّد موجود.
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
    "features/graphics/core/include/sad_ui/generated/color_prelude_generated.h",
    "features/graphics/core/include/sad_ui/generated/color_table_generated.h",
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
    # (AR) وسائط كل نطاق يجب أن تذكر ملفّاته المولَّدة داخل مجلّد الإخراج المُمرَّر.
    for d in xpy.CODEGEN_DOMAINS:
        rendered = " ".join(d["args"]("OUTDIR"))
        for fname in d["outputs"]:
            assert f"OUTDIR/{fname}" in rendered, f"{d['name']}: {fname} غير مُمرَّر"


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


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-v"]))
