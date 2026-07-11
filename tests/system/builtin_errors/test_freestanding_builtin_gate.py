"""
============================================================================
(AR) اختبارات منع انحدار لبوّابة سلامة الوضع الحرّ SEM019 (متابعة #184/#185):
     1) المدمجات غير الآمنة حرًّا (ملفّات/دخل/طرفيّة/عشوائيّة) تُرفَض زمن الترجمة
        بتشخيص SEM019 يسمّيها + خروج غير صفريّ — لا فشل ربط غامض.
     2) عائلة التأكيد (تأكد/اهلع/...) لها مسار حرّ سليم: abort (رمز libc غائب
        على المعدن) يُستبدل بـ __sad_panic (weak؛ النواة تتجاوزه) — تُفحص عبر
        ‎--emit-llvm‎ في IR الناتج.
     3) حرّاس ضدّ الإفراط: البوّابة مقصورة على --freestanding (المستضاف بلا أثر).
(EN) Regression tests for the freestanding-safety gate SEM019 (#184/#185
     follow-up): unsafe builtins (files/stdin/terminal/randomness) are rejected
     at compile time with a named SEM019 + non-zero exit; the assert family gets
     a sound freestanding path (abort → __sad_panic) checked via --emit-llvm IR;
     over-blocking guards prove hosted mode is untouched.
----------------------------------------------------------------------------
(AR) لماذا --emit-llvm؟ البوّابة تعيش في توليد LLVM (emitInstruction) وبوّابة
     السائق hasErrors تُحبط بعد generate() وقبل الإخراج، فـ--emit-llvm يشغّل
     مسار البوّابة كاملًا دون الاعتماد على lld-link/LIB (عزل عن بيئة الربط).
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) مواقع محتملة لـ sad-build.exe (يُفضَّل Debug ثمّ Release)
_CANDIDATES = [
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
]
SAD_BUILD = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

# (AR) بصمة تشخيص البوّابة كما يطبعها reportError (نصّ brief القانونيّ من SoT:
#      SEM_FREESTANDING_BUILTIN/SEM019 في semantic.yaml — المعرّف الرقميّ نفسه لا
#      يُطبع في قناة أخطاء التوليد، فنطابق العبارة القانونيّة الثابتة).
SEM019 = "غير متاحة في الوضع الحرّ"
# (AR) راية الوضع الحرّ + راية إخراج IR (تشغّل التوليد حيث البوّابة، بلا رابط)
FREESTANDING = "--freestanding"
EMIT_LLVM = "--emit-llvm"
# (AR) رموز IR المفحوصة لمسار التأكيد الحرّ (أسماء ABI — مستثناة من قاعدة SoT).
#      ⚠️ نطابق «موضع النداء» لا مجرّد الاسم: تعريف __sad_panic الـweak يُبثّ دائمًا
#      في emitFreestandingRuntime حتّى ببرنامج بلا تأكيدات، فمطابقة الاسم وحده
#      فارغة الدلالة.
PANIC_CALL = "call void @__sad_panic"
ABORT_SYM = "@abort"
# (AR) رمزا runtime المستضاف (sad_embedded_runtime.c) — يجب ألّا يظهرا في IR حرّ
#      (‏-nostdlib لا يربطهما ⇒ فشل ربط غامض)
AEQ_STR_RUNTIME_SYM = "sad_security_assert_equal_str"


def _compile(source: str, *extra_flags: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر بـ sad-build مع الرايات المعطاة؛ يعيد (رمز الخروج،
    الخرج المدموج، نصّ IR الناتج إن صدر)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), *extra_flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=120,
            cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


# ─────────────── 1) البوّابة: مدمجات غير آمنة تُرفَض حرًّا بـ SEM019 ───────────────

def test_random_gated_freestanding():
    """(AR) عشوائي() تُصدر rand() (libc غائبة حرًّا، بلا مصدر عشوائيّة على المعدن)
    — يجب أن تُرفَض حرًّا بتشخيص SEM019 يسمّيها وخروج غير صفريّ."""
    code, out, _ = _compile("متغير س = عشوائي()\nاطبع(س)\n", FREESTANDING)
    assert code != 0, "عشوائي() نجحت حرًّا — البوّابة لا تغطّيها (سيفشل الربط غامضًا):\n" + out
    assert SEM019 in out, "الرفض بلا تشخيص SEM019 قانونيّ:\n" + out
    assert "عشوائي" in out, "تشخيص SEM019 لا يسمّي «عشوائي»:\n" + out


def test_read_line_gated_freestanding():
    """(AR) حارس انحدار #184: قراءة_سطر (دخل قياسيّ) تُرفَض حرًّا بـ SEM019."""
    code, out, _ = _compile("متغير س = قراءة_سطر()\nاطبع(س)\n", FREESTANDING)
    assert code != 0, "قراءة_سطر() نجحت حرًّا — انحدار بوّابة #184:\n" + out
    assert SEM019 in out, "الرفض بلا تشخيص SEM019:\n" + out


def test_clear_screen_gated_freestanding():
    """(AR) حارس انحدار #185: مسح_الشاشة (تحكّم طرفيّة/system) تُرفَض حرًّا بـ SEM019."""
    code, out, _ = _compile("مسح_الشاشة()\n", FREESTANDING)
    assert code != 0, "مسح_الشاشة() نجحت حرًّا — انحدار بوّابة #185:\n" + out
    assert SEM019 in out, "الرفض بلا تشخيص SEM019:\n" + out


def test_file_read_gated_freestanding():
    """(AR) حارس انحدار #184: اقرأ_ملف (نظام ملفّات/fopen) تُرفَض حرًّا بـ SEM019."""
    code, out, _ = _compile('متغير س = اقرأ_ملف("م.txt")\nاطبع(س)\n', FREESTANDING)
    assert code != 0, "اقرأ_ملف() نجحت حرًّا — انحدار بوّابة #184:\n" + out
    assert SEM019 in out, "الرفض بلا تشخيص SEM019:\n" + out


def test_assert_type_gated_freestanding():
    """(AR) تأكد_نوع يستدعي sad_security_assert_type من runtime المستضاف المضمَّن
    (لا يُربط مع ‎-nostdlib‎) — يجب أن تُرفَض حرًّا بـ SEM019 يسمّيها."""
    code, out, _ = _compile('تأكد_نوع("نص", "نص")\n', FREESTANDING)
    assert code != 0, "تأكد_نوع() نجحت حرًّا — سيفشل الربط غامضًا (رمز runtime مستضاف):\n" + out
    assert SEM019 in out, "الرفض بلا تشخيص SEM019:\n" + out
    assert "تأكد_نوع" in out, "تشخيص SEM019 لا يسمّي «تأكد_نوع»:\n" + out


# (AR) دفعة التبويب الشاملة (متابعة #188): كلّ مدمجة هنا تُصدر نداء رمز مستضاف
#      (libc/‏OS/‏runtime مضمَّن) غائب مع ‎-nostdlib‎ — تُرفَض حرًّا بـ SEM019 يسمّيها.
#      (الاسم القانونيّ من SoT، مصدر البرنامج المصغّر يذكر النداء فقط.)
_GATED_BUILTINS = [
    # (اسم قانونيّ، مصدر مصغّر، الرمز المستضاف الموثَّق)
    ("نم", "نم(1)\n", "sad_rt_sleep_ms"),
    # (AR) الوجه غير المتزامن لـ«نم» — opcode مستقلّ (ASYNC_SLEEP) بنفس رمز runtime،
    #      فيُختبَر استقلالًا (إضافة مراجعة Amelia: كان مبوَّبًا بلا اختبار يمثّله).
    ("نوم_غير_متزامن", "نوم_غير_متزامن(1)\n", "sad_rt_sleep_ms"),
    ("اخرج", "اخرج(0)\n", "exit"),
    # (AR) الموسّعات (لوغ/جتا/اقتطاع) تحتاج «استورد رياضيات» ليعرفها فاحص الدلالات.
    ("لوغ", "استورد رياضيات\nمتغير س = لوغ(1)\nاطبع(س)\n", "log"),
    # (AR) بقيّة عائلة libm المبوَّبة — كلّ opcode مبوَّب يُمثَّل باختبار يسمّيه
    #      (إضافة مراجعة Amelia: لوغ2/لوغ10/المعكوسات كانت مبوَّبة بلا تمثيل).
    ("لوغ2", "استورد رياضيات\nمتغير س = لوغ2(8)\nاطبع(س)\n", "log2"),
    ("لوغ10", "استورد رياضيات\nمتغير س = لوغ10(100)\nاطبع(س)\n", "log10"),
    ("جيب", "متغير س = جيب(1)\nاطبع(س)\n", "sin"),
    ("جتا", "استورد رياضيات\nمتغير س = جتا(1)\nاطبع(س)\n", "cos"),
    ("ظل", "متغير س = ظل(1)\nاطبع(س)\n", "tan"),
    ("معكوس_جيب", "استورد رياضيات\nمتغير س = معكوس_جيب(0)\nاطبع(س)\n", "asin"),
    ("معكوس_جتا", "استورد رياضيات\nمتغير س = معكوس_جتا(1)\nاطبع(س)\n", "acos"),
    ("معكوس_ظل", "استورد رياضيات\nمتغير س = معكوس_ظل(0)\nاطبع(س)\n", "atan"),
    ("أرضية", "متغير س = أرضية(1.5)\nاطبع(س)\n", "floor"),
    ("سقف", "متغير س = سقف(1.2)\nاطبع(س)\n", "ceil"),
    ("تقريب", "متغير س = تقريب(1.5)\nاطبع(س)\n", "round"),
    ("اقتطاع", "استورد رياضيات\nمتغير س = اقتطاع(1.5)\nاطبع(س)\n", "trunc"),
    # (AR) صيغة الطريقة م.رتب() — المدمجة «رتب(م)» تتطلّب استيراد وحدة مصفوفات.ص
    #      (وفيها إشارات append/remove غير معرّفة تشوّش الاختبار)؛ الصيغتان تُخفضان
    #      إلى BUILTIN_ARRAY_SORT نفسه (qsort).
    ("رتب", "متغير م = [3, 1, 2]\nم.رتب()\nاطبع(م)\n", "qsort"),
    ("الآن", "متغير س = الآن()\nاطبع(س)\n", "time"),
    ("عشوائي_آمن", "متغير س = عشوائي_آمن(1, 10)\nاطبع(س)\n", "BCryptGenRandom"),
    ("هاش", 'متغير س = هاش("نص")\nاطبع(س)\n', "sad_security_hash"),
    ("شفر", 'متغير س = شفر("نص", "مفتاح")\nاطبع(س)\n', "sad_security_encrypt"),
    ("فك_تشفير", 'متغير س = فك_تشفير("نص", "مفتاح")\nاطبع(س)\n', "sad_security_decrypt"),
    ("نظف", 'متغير س = نظف("نص")\nاطبع(س)\n', "sad_security_sanitize"),
    ("ترميز_64", 'متغير س = ترميز_64("نص")\nاطبع(س)\n', "sad_security_base64_encode"),
]


@pytest.mark.parametrize(
    "name,source,hosted_symbol",
    _GATED_BUILTINS,
    ids=[name for name, _, _ in _GATED_BUILTINS],
)
def test_hosted_dependent_builtin_gated_freestanding(name, source, hosted_symbol):
    """(AR) المدمجة تُصدر نداء رمز مستضاف غائب حرًّا — يجب أن تُرفَض حرًّا بتشخيص
    SEM019 يسمّيها وخروج غير صفريّ (لا فشل ربط غامضًا لاحقًا)."""
    code, out, _ = _compile(source, FREESTANDING)
    assert code != 0, (
        name + "() نجحت حرًّا رغم اعتمادها على " + hosted_symbol + " (سيفشل الربط غامضًا):\n" + out
    )
    assert SEM019 in out, "الرفض بلا تشخيص SEM019 قانونيّ:\n" + out
    assert name in out, "تشخيص SEM019 لا يسمّي «" + name + "»:\n" + out


@pytest.mark.parametrize(
    "name,source,hosted_symbol",
    _GATED_BUILTINS,
    ids=[name for name, _, _ in _GATED_BUILTINS],
)
def test_hosted_dependent_builtin_builds_hosted(name, source, hosted_symbol):
    """(AR) حارس ضدّ الإفراط: المدمجة نفسها تُبنى مستضافًا بنجاح وبلا أثر SEM019
    وبلا بلاغ «Unsupported opcode» زائف (حارس الإشارة المميّزة)."""
    code, out, _ = _compile(source)
    assert code == 0, name + "() فشلت مستضافًا — تسريب بوّابة خارج الوضع الحرّ:\n" + out
    assert SEM019 not in out, "SEM019 ظهر مستضافًا — تسريب بوّابة:\n" + out
    assert "Unsupported opcode" not in out, (
        "بلاغ «Unsupported opcode» زائف مستضافًا (انحدار القيمة الإشاريّة):\n" + out
    )


# ─────── 2) مسار حرّ سليم: عائلة التأكيد تستبدل abort بـ __sad_panic ───────


def test_sqrt_freestanding_uses_intrinsic_not_libm():
    """(AR) جذر() حرًّا: مسار حرّ أصيل (قرار «ب») — البناء ينجح وIR يستعمل
    intrinsic ‏llvm.sqrt (تعليمة عتاد) بلا نداء رمز libm ‏sqrt خارجيّ."""
    code, out, ir = _compile("متغير س = جذر(4)\nاطبع(س)\n", FREESTANDING)
    assert code == 0, "جذر() فشلت حرًّا — إفراط في البوّابة (لها مسار intrinsic):\n" + out
    assert "llvm.sqrt" in ir, "IR الحرّ لـ جذر() بلا intrinsic llvm.sqrt:\n" + out
    assert "call double @sqrt(" not in ir, "IR الحرّ يستدعي رمز libm sqrt — سيفشل الربط على المعدن"


def test_abs_freestanding_uses_intrinsic_not_libm():
    """(AR) مطلق() حرًّا: مسار حرّ أصيل (قرار «ب») — البناء ينجح وIR يستعمل
    intrinsic ‏llvm.fabs (مسح بتّ الإشارة) بلا نداء رمز libm ‏fabs خارجيّ."""
    code, out, ir = _compile("متغير س = مطلق(0 - 3)\nاطبع(س)\n", FREESTANDING)
    assert code == 0, "مطلق() فشلت حرًّا — إفراط في البوّابة (لها مسار intrinsic):\n" + out
    assert "llvm.fabs" in ir, "IR الحرّ لـ مطلق() بلا intrinsic llvm.fabs:\n" + out
    assert "call double @fabs(" not in ir, "IR الحرّ يستدعي رمز libm fabs — سيفشل الربط على المعدن"


def test_pow_freestanding_uses_in_module_pow():
    """(AR) أس() حرًّا: لا تُبوَّب — نسخة pow حرّة تُبثّ داخل الوحدة
    (emitFreestandingPow) فلا رمز libm خارجيّ."""
    code, out, ir = _compile("متغير س = أس(2, 3)\nاطبع(س)\n", FREESTANDING)
    assert code == 0, "أس() فشلت حرًّا — إفراط في البوّابة (لها نسخة حرّة داخل الوحدة):\n" + out
    assert "define" in ir and "@pow" in ir, "IR الحرّ لـ أس() بلا تعريف pow داخل الوحدة:\n" + out

def test_assert_freestanding_uses_panic_not_abort():
    """(AR) تأكد(شرط) حرًّا يجب أن تُبنى (لا بوّابة — التأكيد مفهوم أصيل في النوى)
    وأن يستبدل IR الناتج abort (رمز libc غائب) بنداء __sad_panic (weak، تتجاوزه النواة)."""
    code, out, ir = _compile("تأكد(1 + 1 == 2)\n", FREESTANDING)
    assert code == 0, "تأكد() فشلت حرًّا — إفراط في البوّابة (التأكيد مساره حرّ سليم):\n" + out
    assert PANIC_CALL in ir, "IR الحرّ لعائلة التأكيد بلا نداء __sad_panic — مسار الفشل غير حرّ:\n" + out
    assert ABORT_SYM not in ir, "IR الحرّ ما زال يحوي abort — رمز libc سيُفشل الربط على المعدن"


def test_assert_inside_named_function_freestanding():
    """(AR) حالة حافّة: تأكد داخل دالّة مسمّاة (غير الرئيسيّة) حرًّا — البناء ينجح
    والنداء __sad_panic حاضر (الكتل sec.fail/sec.pass تُبنى في الدالّة الصحيحة)."""
    src = (
        "دالة تحقق()\n"
        "    تأكد(2 > 1)\n"
        "نهاية\n"
        "\n"
        "دالة رئيسية()\n"
        "    تحقق()\n"
        "نهاية\n"
    )
    code, out, ir = _compile(src, FREESTANDING)
    assert code == 0, "تأكد داخل دالّة مسمّاة فشل حرًّا:\n" + out
    assert PANIC_CALL in ir, "IR بلا نداء __sad_panic لتأكيد داخل دالّة مسمّاة:\n" + out
    assert ABORT_SYM not in ir, "IR الحرّ يحوي abort رغم مسار __sad_panic"


def test_panic_builtin_freestanding_uses_panic_not_abort():
    """(AR) ذعر() حرًّا: البناء ينجح وIR يستعمل نداء __sad_panic لا abort."""
    code, out, ir = _compile("ذعر()\n", FREESTANDING)
    assert code == 0, "ذعر() فشلت حرًّا — إفراط في البوّابة:\n" + out
    assert PANIC_CALL in ir, "IR الحرّ لـ ذعر() بلا نداء __sad_panic:\n" + out
    assert ABORT_SYM not in ir, "IR الحرّ لـ ذعر() ما زال يحوي abort (libc غائبة على المعدن)"


def test_panic_followed_by_code_verifies():
    """(AR) حالة حافّة panic.cont: ذعر() متبوعة بكود في الكتلة نفسها — التعليمات
    اللاحقة تهبط في كتلة استمرار بعد unreachable وإلّا فشل تحقّق الوحدة
    (INT_MODULE_VERIFY). تُفحص حرًّا ومستضافًا."""
    src = 'ذعر("عطل")\nاطبع("بعد")\n'
    code, out, _ = _compile(src, FREESTANDING)
    assert code == 0, "ذعر() متبوعة بكود أفشلت التحقّق حرًّا (panic.cont مكسور):\n" + out
    code, out, _ = _compile(src)
    assert code == 0, "ذعر() متبوعة بكود أفشلت التحقّق مستضافًا (panic.cont مكسور):\n" + out


def test_assert_equal_strings_freestanding_inlined():
    """(AR) تأكد_مساواة النصّيّة حرًّا: كانت تُصدر sad_security_assert_equal_str
    (رمز runtime مستضاف غائب مع ‎-nostdlib‎ ⇒ فشل ربط غامض). يجب أن تُضمَّن
    (strcmp داخل الوحدة + __sad_panic) بلا أيّ رمز runtime مستضاف في IR."""
    code, out, ir = _compile('تأكد_مساواة("أ", "أ")\n', FREESTANDING)
    assert code == 0, "تأكد_مساواة النصّيّة فشلت حرًّا:\n" + out
    assert AEQ_STR_RUNTIME_SYM not in ir, (
        "IR الحرّ يستدعي " + AEQ_STR_RUNTIME_SYM + " — رمز runtime مستضاف سيُفشل الربط"
    )
    assert PANIC_CALL in ir, "مسار فشل تأكد_مساواة النصّيّة الحرّ بلا نداء __sad_panic:\n" + out
    assert ABORT_SYM not in ir, "IR الحرّ لتأكد_مساواة يحوي abort"


def test_assert_equal_ints_freestanding_uses_panic():
    """(AR) تأكد_مساواة الصحيحة حرًّا: مقارنة مضمّنة + __sad_panic (لا abort)."""
    code, out, ir = _compile("تأكد_مساواة(3, 3)\n", FREESTANDING)
    assert code == 0, "تأكد_مساواة الصحيحة فشلت حرًّا:\n" + out
    assert PANIC_CALL in ir, "IR الحرّ لتأكد_مساواة الصحيحة بلا نداء __sad_panic:\n" + out
    assert ABORT_SYM not in ir, "IR الحرّ لتأكد_مساواة الصحيحة يحوي abort"


def test_assert_greater_freestanding_uses_panic():
    """(AR) تأكد_أكبر حرًّا: مقارنة مضمّنة + __sad_panic (لا abort)."""
    code, out, ir = _compile("تأكد_أكبر(5, 3)\n", FREESTANDING)
    assert code == 0, "تأكد_أكبر فشلت حرًّا:\n" + out
    assert PANIC_CALL in ir, "IR الحرّ لتأكد_أكبر بلا نداء __sad_panic:\n" + out
    assert ABORT_SYM not in ir, "IR الحرّ لتأكد_أكبر يحوي abort"


# ──────────── 3) حرّاس ضدّ الإفراط: المستضاف بلا أيّ أثر للبوّابة ────────────

def test_random_builds_hosted():
    """(AR) عشوائي() مستضافًا تُبنى بنجاح — البوّابة مقصورة على --freestanding."""
    code, out, _ = _compile("متغير س = عشوائي()\nاطبع(س)\n")
    assert code == 0, "عشوائي() فشلت مستضافًا — البوّابة تسرّبت خارج الوضع الحرّ (انحدار):\n" + out
    assert SEM019 not in out, "SEM019 ظهر مستضافًا — تسريب بوّابة:\n" + out


def test_assert_hosted_keeps_abort():
    """(AR) تأكد() مستضافًا يبقى على abort (لا تغيير في السلوك المستضاف)."""
    code, out, ir = _compile("تأكد(1 + 1 == 2)\n")
    assert code == 0, "تأكد() فشلت مستضافًا (انحدار):\n" + out
    assert ABORT_SYM in ir, "IR المستضاف لـ تأكد() فقد abort — تغيّر سلوك مستضاف غير مقصود"


def test_assert_equal_strings_hosted_keeps_runtime_call():
    """(AR) تأكد_مساواة النصّيّة مستضافًا تبقى على نداء runtime المضمَّن
    sad_security_assert_equal_str (لا تغيير في السلوك المستضاف)."""
    code, out, ir = _compile('تأكد_مساواة("أ", "أ")\n')
    assert code == 0, "تأكد_مساواة النصّيّة فشلت مستضافًا (انحدار):\n" + out
    assert AEQ_STR_RUNTIME_SYM in ir, (
        "IR المستضاف فقد نداء " + AEQ_STR_RUNTIME_SYM + " — تغيّر سلوك مستضاف غير مقصود"
    )


def test_assert_type_hosted_builds_without_spurious_error():
    """(AR) تأكد_نوع النصّيّة مستضافًا: تُبنى بلا بلاغ INT زائف — كانت القيمة
    الإشاريّة nullptr تُسقط الموزّع فيطبع «Unsupported opcode:207» مضلِّلًا."""
    code, out, _ = _compile('تأكد_نوع("نص", "نص")\n')
    assert code == 0, "تأكد_نوع() فشلت مستضافًا (انحدار):\n" + out
    assert "Unsupported opcode" not in out, (
        "بلاغ «Unsupported opcode» زائف مستضافًا رغم إصدار النداء (انحدار القيمة الإشاريّة):\n" + out
    )
    assert SEM019 not in out, "SEM019 ظهر مستضافًا — تسريب بوّابة:\n" + out
