"""
============================================================================
(AR) اختبارات منع انحدار: مسار فشل فحص الحدّ (مصفوفة/وسم تعداد) لا يُصدر تشخيصًا
     إنجليزيًّا (printf/bc.fmt) في **الوضع الحرّ** — __sad_panic وحده يحمل التشخيص
     عبر لافتة الهلع العربيّة السياديّة في تعريف النواة. المستضاف يُبقي التشخيص
     الإنجليزيّ للمطوّر + exit(1). (دَين مسار الهلع العربيّ الموحَّد.)
(EN) Regression: the bounds-check fail path emits no English printf in
     freestanding (only __sad_panic; the kernel's Arabic banner is the sole
     diagnostic). Hosted keeps the English developer diagnostic + exit(1).
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ كتلة الفشل ونداءاتها تعيشان في إصدار LLVM.
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ومسارات بلا ‎.exe‎ للينكس/ماك: بدونها يبتلع ‎skipif‎ الملفَّ أخضرَ صامتًا في CI.
_CANDIDATES = [
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build",
    ROOT / "build" / "bin" / "Debug" / "sad-build",
    ROOT / "build" / "bin" / "sad-build",
]
SAD_BUILD = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
NO_MAIN = "--بلا-رئيسية"
EMIT_LLVM = "--أظهر-llvm"

# (AR) مصدر يُطلق فحص حدّ مصفوفة: فهرسة بمتغيّر ⇒ يزرع المترجم كتلة فشل.
ARRAY_OOB_SRC = (
    "متغير ساكن جدول مصفوفة[4]\n"
    "دالة رقم اقرأ_خانة(رقم فهرس)\n"
    "    ارجع جدول[فهرس]\n"
    "نهاية\n"
)

# (AR) مصدر يُطلق فخّ «وصول حقل لحالة تعداد خاطئة»: «ك.عرض» على «دائرة» بينما
#      «عرض» حقل «مستطيل» ⇒ trapBB في enum_ops.
ENUM_WRONGVAR_SRC = (
    "تعداد شكل\n"
    "    دائرة(نق)\n"
    "    مستطيل(عرض، طول)\n"
    "نهاية\n"
    "دالة رقم جرب()\n"
    "    متغير ك = شكل.دائرة(7)\n"
    "    ارجع ك.عرض\n"
    "نهاية\n"
)

# (AR) مصدر يُطلق فحص عامل التأكيد «مؤكَّد» على قيمة قد تكون عدمًا (متغيّر اختياريّ)
#      ⇒ na.fail في arith_main (رمز سبب مميَّز kSadPanicNullAssert=2).
#
# @صيغة-مرفوضة — التعليقُ يقتبِس الصيغةَ المحذوفةَ ليشرحَ العطبَ لا ليُعلّمَها
# (AR) 🔑 ⚠️ **كان هذا المِجَسُّ يكتب «متغير س: رقم؟» — وهي صيغةٌ حُذِفت من
#      اللغة** (بذرةُ `VE053_colon_type_form_is_deleted` تُثبِّت رفضَها). فصار
#      المِجَسُّ يُخفِق تحليلًا بـSYN001 ويخرج المصرِّفُ بـ١.
#
#      ⇒ ثلاثةُ حرّاسٍ لعقدِ رموزِ الهلع صاروا **غيرَ مقيسين** من يومِ حذفِ
#      الصيغة، لا لأنّ العقدَ انكسر بل لأنّ **مصدرَ المِجَسِّ لم يعد يُصرَّف**.
#
#      🔑 وصخبُ الإخفاقِ كان **رهنَ ترتيبِ التوكيدات** — وهو المقيسُ حرفيًّا
#      (٢٠٢٦-٠٨-١٧، بإعادةِ تشغيلِ النسخةِ السابقة):
#
#      · الاثنان اللذان يبدآن بـ`assert code == 0, out` أخفقا **جهرًا**:
#        الرسالةُ تحمل نصَّ SYN001 كاملًا، فيُقرآن على وجهِهما.
#      · و`test_panic_reason_codes_are_distinct` **لا توكيدَ فيه على رمزِ
#        الخروجِ أصلًا** (يُهمِله بـ`_`)، فسقط على
#        `assert '@__sad_panic(i64 2)' in ''` برسالةِ «التأكيد يجب أن يمرّر
#        رمز 2 حصرًا» — أي يُقرأ **انحدارًا في عقدِ الهلع** وهو انحدارٌ في
#        **صيغةِ الاختبار**. وحارسٌ يُخفق لسببٍ غيرِ الذي وُضِع له أسوأُ من
#        غيابِه.
#
#      ⇒ فلا يصحّ الاتّكالُ على «سيُخفِق جهرًا»: جهرُه صدفةُ ترتيبٍ لا خاصّةُ
#      تصميم. ولذلك وُضِع حارسٌ يُصرِّف المِجَسّاتِ نفسَها:
#      `scripts/codegen/check_sad_snippets_are_real_syntax.py`.
#
#      ⚠️ والدرسُ الأعمّ: **حذفُ صيغةٍ من اللغةِ يجب أن يُتبَع بمسحِ مستهلِكيها
#      خارجَ مجموعةِ السلوك.** مسحُ `tests/behavior` وحدَه لا يراهم: هؤلاء
#      مِجَسّاتٌ **داخلَ ملفّات بايثون**، لا يقع عليها أيُّ نحوٍ يفحص `.ص`.
#
#      والصيغةُ الباقيةُ هي الصفةُ اللفظيّة «عدمي» (قرارُ المالك 2026-08-13):
#      قِيس أنّها تُنتِج `@__sad_panic(i64 2)` عينَه، فالعقدُ المحروسُ لم يتبدّل.
NULL_ASSERT_SRC = (
    "دالة رقم جرب()\n"
    "    رقم عدمي س = لاشيء\n"
    "    ارجع س مؤكد\n"
    "نهاية\n"
)


# (AR) مصدر يُطلق فشل تأكيد المستخدم «تأكد» (مسار CheckViolation غير-الحدّ) ⇒
#      وضع حرّ: __sad_panic(1) بلا سطر إنجليزيّ؛ مستضاف: abort. يحرس أنّ رمز
#      السبب لمواقع الانتهاك البنيويّ غير-الحدّ يبقى 1.
ASSERT_FAIL_SRC = (
    "دالة رقم جرب(رقم ب)\n"
    "    تأكد(ب > 0)\n"
    "    ارجع ب\n"
    "نهاية\n"
)


def _compile(source: str, *extra_flags: str) -> tuple[int, str, str]:
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), *extra_flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def test_freestanding_bounds_fail_no_english_diagnostic():
    """(AR) وضع حرّ: كتلة فشل الحدّ تنادي __sad_panic بلا printf/رسالة إنجليزيّة."""
    code, out, ir = _compile(ARRAY_OOB_SRC, FREESTANDING, NO_MAIN)
    assert code == 0, out
    assert "call void @__sad_panic" in ir, "لم يُصدَر نداء __sad_panic في الوضع الحرّ:\n" + ir[:2000]
    # (AR) رمز سبب الهلع = kSadPanicCheckViolation (1) لفحص بنيويّ
    assert "@__sad_panic(i64 1)" in ir, "رمز سبب الهلع للحدّ ليس 1 (فحص بنيويّ):\n" + ir[:2000]
    assert "out of bounds" not in ir, \
        "تسرّبت رسالة إنجليزيّة (out of bounds) في الوضع الحرّ:\n" + ir[:2000]
    assert "@bc.fmt" not in ir, "أُصدرت سلسلة bc.fmt في الوضع الحرّ:\n" + ir[:2000]
    # (AR) لا نداء printf في كتلة الفشل الحرّة (قد يبقى printf ضعيفًا مُصدَرًا لأسباب
    #      أخرى، لكن لا نداء له من مسار الحدّ) — نتحقّق بغياب صيغة الحدّ تحديدًا.


def test_hosted_bounds_fail_keeps_english_diagnostic():
    """(AR) مستضاف: كتلة فشل الحدّ تُبقي التشخيص الإنجليزيّ للمطوّر + exit."""
    code, out, ir = _compile(ARRAY_OOB_SRC, NO_MAIN)
    assert code == 0, out
    assert "@bc.fmt" in ir, "غاب التشخيص الإنجليزيّ في المستضاف (يجب أن يبقى):\n" + ir[:2000]
    assert "out of bounds" in ir, "غابت رسالة الحدّ الإنجليزيّة في المستضاف:\n" + ir[:2000]
    assert "call void @exit" in ir, "المستضاف يجب أن ينادي exit في كتلة الفشل:\n" + ir[:2000]


def test_freestanding_enum_wrongvariant_no_english_diagnostic():
    """(AR) وضع حرّ: فخّ حقل الحالة الخاطئة ينادي __sad_panic بلا تشخيص إنجليزيّ."""
    code, out, ir = _compile(ENUM_WRONGVAR_SRC, FREESTANDING, NO_MAIN)
    assert code == 0, out
    assert "call void @__sad_panic" in ir, "لم يُصدَر __sad_panic لفخّ التعداد في الحرّ:\n" + ir[:2000]
    assert "@adt.wrongvar.fmt" not in ir, "أُصدرت سلسلة adt.wrongvar.fmt في الحرّ:\n" + ir[:2000]
    assert "RUN005" not in ir, "تسرّبت رسالة RUN005 الإنجليزيّة في الحرّ:\n" + ir[:2000]


def test_hosted_enum_wrongvariant_keeps_english_diagnostic():
    """(AR) مستضاف: فخّ حقل الحالة الخاطئة يُبقي التشخيص الإنجليزيّ + exit."""
    code, out, ir = _compile(ENUM_WRONGVAR_SRC, NO_MAIN)
    assert code == 0, out
    assert "@adt.wrongvar.fmt" in ir, "غاب تشخيص التعداد الإنجليزيّ في المستضاف:\n" + ir[:2000]
    assert "call void @exit" in ir, "المستضاف يجب أن ينادي exit في فخّ التعداد:\n" + ir[:2000]


def test_freestanding_null_assert_distinct_code_no_arabic_line():
    """(AR) وضع حرّ: فشل «مؤكَّد» ينادي __sad_panic برمز التأكيد المميَّز (2) بلا
    سطر RUN056 العربيّ السابق (اللافتةُ هي التشخيصُ الوحيد)."""
    code, out, ir = _compile(NULL_ASSERT_SRC, FREESTANDING, NO_MAIN)
    assert code == 0, out
    assert "@__sad_panic(i64 2)" in ir, \
        "رمز سبب الهلع لفشل التأكيد ليس 2 (kSadPanicNullAssert):\n" + ir[:2500]
    assert "@na.fmt" not in ir, "أُصدر سطر RUN056 العربيّ في الوضع الحرّ (يجب حذفه):\n" + ir[:2500]
    assert "RUN056" not in ir, "تسرّبت رسالة RUN056 في الوضع الحرّ:\n" + ir[:2500]
    # (AR) توكيد مضادّ: الوضع الحرّ يجب ألّا ينادي exit (مسار المستضاف) — يحرس تسرّبًا عكسيًّا.
    assert "call void @exit" not in ir, "تسرّب نداء exit المستضاف إلى الوضع الحرّ:\n" + ir[:2500]


def test_hosted_null_assert_keeps_arabic_diagnostic():
    """(AR) مستضاف: فشل «مؤكَّد» يُبقي تشخيص RUN056 العربيّ للمطوّر + exit."""
    code, out, ir = _compile(NULL_ASSERT_SRC, NO_MAIN)
    assert code == 0, out
    assert "@na.fmt" in ir, "غاب تشخيص RUN056 العربيّ في المستضاف (يجب أن يبقى):\n" + ir[:2500]
    assert "call void @exit" in ir, "المستضاف يجب أن ينادي exit لفشل التأكيد:\n" + ir[:2500]
    # (AR) توكيد مضادّ: المستضاف يجب ألّا ينادي __sad_panic (مسار الوضع الحرّ) — يقفل تسرّبًا عكسيًّا.
    assert "@__sad_panic" not in ir, "تسرّب نداء __sad_panic الحرّ إلى المستضاف:\n" + ir[:2500]


def test_panic_reason_codes_are_distinct():
    """(AR) عقد رموز السبب: الحدّ (1) ≠ التأكيد (2) في الوضع الحرّ — لا خلط."""
    _, _, ir_bounds = _compile(ARRAY_OOB_SRC, FREESTANDING, NO_MAIN)
    _, _, ir_assert = _compile(NULL_ASSERT_SRC, FREESTANDING, NO_MAIN)
    assert "@__sad_panic(i64 1)" in ir_bounds and "@__sad_panic(i64 2)" not in ir_bounds, \
        "الحدّ يجب أن يمرّر رمز 1 حصرًا"
    assert "@__sad_panic(i64 2)" in ir_assert and "@__sad_panic(i64 1)" not in ir_assert, \
        "التأكيد يجب أن يمرّر رمز 2 حصرًا"


def test_freestanding_user_assert_check_violation_no_english():
    """(AR) وضع حرّ: فشل «تأكد» (مسار الأمن sec.fail) ينادي __sad_panic برمز
    الانتهاك البنيويّ (1) بلا exit — يحرس أنّ مواقع الانتهاك غير-الحدّ تبقى رمز 1.
    ملاحظة: «تأكد» تُخفَض إلى BUILTIN_SECURITY_ASSERT (abort مستضافًا) لا
    BUILTIN_ASSERT؛ فتوكيد غياب «Assertion failed» صحيح بداهةً (ذلك السطر حصريّ
    لمسار emitBuiltinAssert الكامن) ويُبقى كحارس دفاعيّ ضدّ إعادة توجيه مستقبليّة."""
    code, out, ir = _compile(ASSERT_FAIL_SRC, FREESTANDING, NO_MAIN)
    assert code == 0, out
    assert "@__sad_panic(i64 1)" in ir, \
        "رمز سبب الهلع لفشل «تأكد» ليس 1 (kSadPanicCheckViolation):\n" + ir[:2500]
    assert "Assertion failed" not in ir, \
        "تسرّب سطر «Assertion failed» الإنجليزيّ في الوضع الحرّ:\n" + ir[:2500]
    assert "call void @exit" not in ir, "تسرّب نداء exit المستضاف إلى الوضع الحرّ:\n" + ir[:2500]


def test_hosted_user_assert_uses_abort():
    """(AR) مستضاف: فشل «تأكد» يتوقّف بـ abort() (لا __sad_panic الحرّ)."""
    code, out, ir = _compile(ASSERT_FAIL_SRC, NO_MAIN)
    assert code == 0, out
    assert "call void @abort" in ir, "المستضاف يجب أن ينادي abort لفشل «تأكد»:\n" + ir[:2500]
    assert "@__sad_panic" not in ir, "تسرّب نداء __sad_panic الحرّ إلى المستضاف:\n" + ir[:2500]
