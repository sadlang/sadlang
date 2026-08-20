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
SAD_BUILD = max(
    (p for p in _CANDIDATES if p.exists()),
    # (AR) أحدثُ الموجودَين لا أوّلُهما: قِيسَ ثنائيُّ Release عمرُه ١٣ يومًا
    #      يسبق Debug طازجًا في القائمة، فولّدت الأداةُ عطبًا وهميًّا.
    # (EN) Newest, not first: a 13-day-old Release binary preceded a fresh
    #      Debug one and the instrument fabricated a defect.
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

# (AR) بصمة تشخيص البوّابة كما يطبعها reportError (نصّ brief القانونيّ من SoT:
#      SEM_FREESTANDING_BUILTIN/SEM019 في semantic.yaml — المعرّف الرقميّ نفسه لا
#      يُطبع في قناة أخطاء التوليد، فنطابق العبارة القانونيّة الثابتة).
SEM019 = "غير متاحة في الوضع الحرّ"
# (AR) راية الوضع الحرّ + راية إخراج IR (تشغّل التوليد حيث البوّابة، بلا رابط)
FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"
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
    # (AR) ⚠️ «اخرج» صارت تلزمها «استورد أساسيات»، وستٌّ من عائلةِ الرياضيّات
    #      تلزمها «استورد رياضيات». والانجرافُ عاش لأنَّ هذا الملفَّ كان
    #      **بلا مُنادٍ في CI**. وأثرُه مرئيٌّ في الجدولِ نفسِه: «لوغ»
    #      أُضيف إليها الاستيرادُ و«جيب» تُرِكت — نصفُ تحديثٍ لا يراه أحد.
    ("اخرج", "استورد أساسيات\nاخرج(0)\n", "exit"),
    # (AR) الموسّعات (لوغ/جتا/اقتطاع) تحتاج «استورد رياضيات» ليعرفها فاحص الدلالات.
    ("لوغ", "استورد رياضيات\nمتغير س = لوغ(1)\nاطبع(س)\n", "log"),
    # (AR) بقيّة عائلة libm المبوَّبة — كلّ opcode مبوَّب يُمثَّل باختبار يسمّيه
    #      (إضافة مراجعة Amelia: لوغ2/لوغ10/المعكوسات كانت مبوَّبة بلا تمثيل).
    ("لوغ2", "استورد رياضيات\nمتغير س = لوغ2(8)\nاطبع(س)\n", "log2"),
    ("لوغ10", "استورد رياضيات\nمتغير س = لوغ10(100)\nاطبع(س)\n", "log10"),
    ("جيب", "استورد رياضيات\nمتغير س = جيب(1)\nاطبع(س)\n", "sin"),
    ("جتا", "استورد رياضيات\nمتغير س = جتا(1)\nاطبع(س)\n", "cos"),
    ("ظل", "استورد رياضيات\nمتغير س = ظل(1)\nاطبع(س)\n", "tan"),
    ("معكوس_جيب", "استورد رياضيات\nمتغير س = معكوس_جيب(0)\nاطبع(س)\n", "asin"),
    ("معكوس_جتا", "استورد رياضيات\nمتغير س = معكوس_جتا(1)\nاطبع(س)\n", "acos"),
    ("معكوس_ظل", "استورد رياضيات\nمتغير س = معكوس_ظل(0)\nاطبع(س)\n", "atan"),
    ("أرضية", "استورد رياضيات\nمتغير س = أرضية(1.5)\nاطبع(س)\n", "floor"),
    ("سقف", "استورد رياضيات\nمتغير س = سقف(1.2)\nاطبع(س)\n", "ceil"),
    ("تقريب", "استورد رياضيات\nمتغير س = تقريب(1.5)\nاطبع(س)\n", "round"),
    ("اقتطاع", "استورد رياضيات\nمتغير س = اقتطاع(1.5)\nاطبع(س)\n", "trunc"),
    # (AR) صيغة الطريقة م.رتب() — المدمجة «رتب(م)» تتطلّب استيراد وحدة مصفوفات.ص
    #      (وفيها إشارات append/remove غير معرّفة تشوّش الاختبار)؛ الصيغتان تُخفضان
    #      إلى BUILTIN_ARRAY_SORT نفسه (qsort).
    ("رتب", "متغير م = [3, 1, 2]\nم.رتب()\nاطبع(م)\n", "qsort"),
    # (AR) «الآن» لم تعُد هنا: رُفعت بوّابتها ﻷنّ لها الآن مسارًا حرًّا سليمًا لكلّ
    #      هدف — انظر اختبارَي العقد الموجبين أدناه (معدن ⇒ جسر CMOS مبثوث،
    #      هدف بنظام تشغيل ⇒ `time` خارجيّ توفّره libc).
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
    code, out, _ = _compile("استورد رياضيات\nمتغير س = عشوائي()\nاطبع(س)\n")
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


# ──── 4) عقد «الآن» الحرّ حسب الهدف: جسر عتاد على المعدن، libc على المستضاف ────
# (AR) رُفعت بوّابة SEM019 عن «الآن» ﻷنّ لها مسارًا حرًّا سليمًا في الحالتين، لكنّ
#      المسارين مختلفان جوهريًّا ويجب تثبيتهما:
#        • معدن عارٍ (بلا نظام تشغيل، الحلقة 0): يُبثّ تعريف `time` داخل الوحدة
#          يقرأ ساعة CMOS بمنافذ الدخل/الخرج — لا libc هناك.
#        • هدف بنظام تشغيل (الحلقة 3): تعليمتا in/out ممتازتان ⇒ #GP ⇒ SIGSEGV،
#          فيجب أن يبقى `time` **تصريحًا خارجيًّا** توفّره libc/CRT.
#      بثّ نسخة CMOS على هدف مستضاف انهيارٌ زمن تشغيل لا خطأ ترجمة، فلا يمسكه
#      إلّا اختبار عقد كهذا.
NOW_SOURCE = "استورد خرائط\nمتغير س = الآن()\nاطبع(س)\n"
BARE_METAL_TARGET = "--هدف=i686-unknown-elf"   # (AR) ثالوث نواة النحلة
HOSTED_TARGET = "--هدف=x86_64-linux-gnu"
LINUX32_TARGET = "--هدف=i686-linux-gnu"  # (AR) نطاق مستخدم لينكس 32-بت (نحلة م4)
WINDOWS_TARGET = "--هدف=x86_64-pc-windows-msvc"
CMOS_PORT_ASM = "outb"  # (AR) بصمة تجميع منافذ CMOS المُضمّن في جسر المعدن
IN_PORT_ASM = "inb"     # (AR) بصمة قراءة منفذ معزول (x86 حصرًا)
# (AR) بصمة بوّابة نداء نظام لينكس — التعليمة نفسها لا اسم قيمة الإرجاع (الأخير
#      سُمّي «sys.ret» عمدًا كي لا تظهر السلسلة "syscall" ولو حُذف التجميع):
SYSCALL_ASM = 'asm sideeffect "syscall"'  # x86_64
SYSCALL32_ASM = "int $$0x80"              # i386 (‏$$ في IR = بوّابة int 0x80)


def _time_definitions(ir: str) -> tuple[bool, bool]:
    """(AR) يعيد (مُعرَّفة داخل الوحدة، مُصرَّحة خارجيًّا) لرمز `time` في IR."""
    import re

    defined = re.search(r"^define[^\n]*@time\(", ir, re.MULTILINE) is not None
    declared = re.search(r"^declare[^\n]*@time\(", ir, re.MULTILINE) is not None
    return defined, declared


def test_now_freestanding_bare_metal_emits_cmos_bridge():
    """(AR) على المعدن العاري: «الآن» تُترجم حرًّا ويُبثّ تعريف `time` داخل الوحدة
    (جسر ساعة CMOS) — لا اعتماد على libc غائبة."""
    code, out, ir = _compile(NOW_SOURCE, FREESTANDING, BARE_METAL_TARGET)
    assert code == 0, "الآن() رُفضت حرًّا على المعدن رغم توفّر جسر CMOS:\n" + out
    assert SEM019 not in out, "بوّابة SEM019 ما زالت تحجب «الآن» على المعدن:\n" + out
    defined, _ = _time_definitions(ir)
    assert defined, "IR المعدن بلا تعريف `time` — جسر ساعة CMOS لم يُبثّ"
    assert CMOS_PORT_ASM in ir, "IR المعدن بلا تجميع منافذ مُضمّن — الجسر العتاديّ مفقود"


def test_now_freestanding_linux_uses_syscall_not_ports():
    """(AR) على لينكس x86_64: «الآن» تُترجم حرًّا ويُبثّ تعريف `time` داخل الوحدة،
    لكنْ عبر **نداء نظام** لا منافذ CMOS. هذا يحسم أمرين معًا: منافذ الدخل/الخرج
    ممتازة في الحلقة 3 (‏#GP ⇒ SIGSEGV)، والاعتماد على libc يكسر ‎-nostdlib‎."""
    code, out, ir = _compile(NOW_SOURCE, FREESTANDING, HOSTED_TARGET)
    assert code == 0, "الآن() رُفضت حرًّا على لينكس:\n" + out
    assert SEM019 not in out, "بوّابة SEM019 ما زالت تحجب «الآن» على لينكس:\n" + out
    defined, _ = _time_definitions(ir)
    assert defined, "IR لينكس بلا تعريف `time` — جسر نداء النظام لم يُبثّ (عاد الاعتماد على libc)"
    assert SYSCALL_ASM in ir, "IR لينكس بلا تعليمة نداء نظام — الجسر السياديّ مفقود"
    assert CMOS_PORT_ASM not in ir, (
        "IR لينكس يحوي منافذ دخل/خرج — ممتازة في الحلقة 3 ⇒ SIGSEGV زمن التشغيل"
    )


def test_now_freestanding_linux32_uses_int80_gate():
    """(AR) على لينكس i386: نداء النظام عبر بوّابة `int 0x80` لا `syscall` — تغطية
    مباشرة لواصف ABI الـ32-بت (بيئة نطاق مستخدم نحلة م4). خطأ في time=13 أو في
    تهريب $$ يُشحن بلا كشف دون هذا الاختبار."""
    code, out, ir = _compile(NOW_SOURCE, FREESTANDING, LINUX32_TARGET)
    assert code == 0, "الآن() رُفضت حرًّا على لينكس 32-بت:\n" + out
    defined, _ = _time_definitions(ir)
    assert defined, "IR لينكس 32-بت بلا تعريف `time` — جسر نداء النظام لم يُبثّ"
    assert SYSCALL32_ASM in ir, "IR لينكس 32-بت بلا بوّابة int 0x80 — واصف i386 مكسور"
    assert CMOS_PORT_ASM not in ir, "IR لينكس 32-بت يحوي منافذ CMOS — ممتازة في الحلقة 3"


def test_now_freestanding_other_os_leaves_time_external():
    """(AR) على نظام تشغيل لا نبثّ له نداء نظام (ويندوز): يبقى `time` تصريحًا
    خارجيًّا يحلّه CRT — لا منافذ ولا نداء نظام لينكس."""
    code, out, ir = _compile(NOW_SOURCE, FREESTANDING, WINDOWS_TARGET)
    assert code == 0, "الآن() رُفضت حرًّا على ويندوز:\n" + out
    defined, declared = _time_definitions(ir)
    assert not defined, "IR ويندوز يحوي تعريف `time` — لا جسر صالحًا هناك"
    assert declared, "IR ويندوز بلا تصريح `time` — لن يُحلّ الرمز من CRT"


# ─── 5) عمى المعمارية: لا تُبثّ تعليمات x86 على معدن بمعمارية أخرى ─────────────
# (AR) inb/outb **غير موجودتين في مجموعة تعليمات** aarch64/riscv64، والمنفذ
#      التسلسليّ هناك ذاكرة مُهيَّأة يختلف عنوانها بكلّ لوحة. كان المميِّز يقرأ
#      النظام وحده فيبثّ شيفرة x86 على أيّ معدن — تجميعٌ لا يُترجَم أصلًا.
ARM_BARE_TARGET = "--هدف=aarch64-unknown-none-elf"
RISCV_BARE_TARGET = "--هدف=riscv64-unknown-none-elf"
PRINT_SOURCE = 'اطبع("س")\n'
HALT_ASM = "hlt"


@pytest.mark.parametrize("target", [ARM_BARE_TARGET, RISCV_BARE_TARGET])
def test_non_x86_bare_metal_emits_no_x86_instructions(target):
    """(AR) معدن عارٍ بمعمارية بلا منافذ معزولة: أكعاب محايدة، صفر تعليمات x86."""
    code, out, ir = _compile(PRINT_SOURCE, FREESTANDING, target)
    assert code == 0, "الترجمة فشلت على معدن غير x86:\n" + out
    assert CMOS_PORT_ASM not in ir, (
        "IR معدن " + target + " يحوي outb — تعليمة x86 لا وجود لها في هذه المعمارية"
    )
    assert IN_PORT_ASM not in ir, "IR معدن غير x86 يحوي inb — تعليمة x86 غير موجودة"
    assert HALT_ASM not in ir, "IR معدن غير x86 يحوي hlt — تعليمة x86 غير موجودة"


# ─── 6) عقد time(tloc): المؤشّر غير العدميّ يُكتب عبره في كلّ الجسور ────────────
# (AR) كانت نسخة CMOS تتجاهل المعامل بينما libc تحترمه ⇒ تباعد صامت: كودٌ يقرأ
#      ‎*tloc‎ يعمل مستضافًا ويقرأ قمامةً حرًّا.
TLOC_STORE_BLOCK = "tloc.store"


@pytest.mark.parametrize("target", [BARE_METAL_TARGET, HOSTED_TARGET, ARM_BARE_TARGET])
def test_time_honours_tloc_pointer_in_every_bridge(target):
    """(AR) كلّ جسر يبثّ `time` يجب أن يحوي فرع الكتابة عبر المؤشّر."""
    code, out, ir = _compile(NOW_SOURCE, FREESTANDING, target)
    assert code == 0, "الترجمة فشلت:\n" + out
    defined, _ = _time_definitions(ir)
    assert defined, "لا تعريف `time` على " + target
    assert TLOC_STORE_BLOCK in ir, (
        "جسر `time` على " + target + " يتجاهل معامل tloc — تباعد عن عقد C ودالّة libc"
    )


# ─── 7) الهلع في الحلقة 3: إنهاء نظيف بنداء نظام لا تعليمة ممتازة ─────────────
# (AR) cli/hlt ممتازتان: على لينكس ينتهي الهلع بـ#GP ⇒ SIGSEGV بدل رمز خروج.
#      والبديل abort مرفوض — يكسر عقد «حرًّا: abort ⇒ __sad_panic» ويُضيع رمز
#      سبب الهلع (#248). نداء النظام لا يفعل أيًّا من الاثنين.
PANIC_SOURCE = "تأكد(1 + 1 == 3)\n"
# (AR) رقم نداء exit_group على x86_64 = 231 (i386 = 252). بصمة مميِّزة **للرقم**
#      لا لمجرّد "syscall": مسار الطبع في __sad_panic يبثّ نداء write أصلًا، فلا
#      يميّز وجودُ "syscall" وحده إنهاءَ exit_group من حلقة دوران — لا بدّ من الرقم.
EXITGROUP_NUM_X64 = "i64 231"


def test_panic_linux_exits_cleanly_without_privileged_instructions():
    """(AR) هلع حرّ على لينكس: بلا cli/hlt وبلا abort — نداء exit_group فقط.
    نؤكّد على **رقم** النداء لا مجرّد "syscall" (الطبع يبثّه أصلًا)."""
    code, out, ir = _compile(PANIC_SOURCE, FREESTANDING, HOSTED_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    assert PANIC_CALL in ir, "مسار التأكيد الحرّ لا يستدعي __sad_panic (انحدار)"
    assert HALT_ASM not in ir, "IR لينكس يحوي hlt — تعليمة ممتازة ⇒ #GP ⇒ SIGSEGV بدل خروج نظيف"
    assert ABORT_SYM not in ir, "IR حرّ يحوي abort — رمز libc غائب وعقد #248 مكسور"
    assert EXITGROUP_NUM_X64 in ir, (
        "IR لينكس بلا نداء exit_group (231) — الهلع يعلّق بدل إنهاء نظيف"
    )


def test_panic_bare_metal_keeps_halt_loop():
    """(AR) على المعدن يبقى cli/hlt: حارس ضدّ تعميم مسار لينكس على النواة."""
    code, out, ir = _compile(PANIC_SOURCE, FREESTANDING, BARE_METAL_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    assert HALT_ASM in ir, "IR المعدن فقد hlt — الهلع صار دورانًا يحرق المعالج"


def test_panic_other_os_traps_not_spins():
    """(AR) هلع حرّ على نظام بلا نداء (ويندوز): llvm.trap لا حلقة دوران. حلقة
    فارغة كانت ستبدّل انهيارًا صاخبًا بتعليق صامت ١٠٠٪ معالج — أسوأ للأدوات."""
    code, out, ir = _compile(PANIC_SOURCE, FREESTANDING, WINDOWS_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    assert "@llvm.trap" in ir, "IR ويندوز بلا llvm.trap — الهلع يعلّق بدل إنهاء فوريّ"


# ─── 8) الطبع الحرّ على لينكس: write بنداء نظام لا putchar من libc ─────────────
PUTCHAR_SYM = "@putchar"


def test_print_linux_uses_write_syscall_not_libc():
    """(AR) «اطبع» حرًّا على لينكس تمرّ بـ__sad_serial_putc: يجب أن يبثّ نداء
    write لا نداء putchar — وإلّا بقي الوضع الحرّ رهين libc ولا يُربط ‎-nostdlib‎."""
    code, out, ir = _compile(PRINT_SOURCE, FREESTANDING, HOSTED_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    assert SYSCALL_ASM in ir, "IR لينكس بلا نداء نظام — مسار الطبع لم يُحرَّر من libc"
    assert PUTCHAR_SYM not in ir, (
        "IR لينكس ما زال ينادي putchar — تبعيّة libc باقية في مسار الطبع الحرّ"
    )
    assert CMOS_PORT_ASM not in ir, "IR لينكس يحوي outb — منفذ COM1 ممتاز في الحلقة 3"


# ─── 9) الهدف المُستنتَج: تشخيص بالافتراض بدل فشل ربط صامت ─────────────────────
# (AR) راية الوضع الحرّ تصف غياب المكتبة القياسيّة لا حلقة الامتياز. بلا «--هدف»
#      يُورَث ثالوث المضيف، فيصنّفه السائق بنفس دالّة الخلفيّة:
#        • مضيف لينكس ⇒ **ملاحظة** (تُبثّ نداءات النظام ويُربط -nostdlib فعلًا)،
#          ليست تحذيرًا كي لا تنقلب خطأً تحت -Werror.
#        • مضيف بلا نداء (ويندوز/ماك) ⇒ **تحذير** (لا جسور، لا معدن، لا -nostdlib).
#      مضيف CI قد يكون أيًّا منها، فنطابق النصّ المشترك «الوضع الحرّ بلا».
INFERRED_TARGET_DIAG = "الوضع الحرّ بلا"
LINUX_USERSPACE_NOTE = "افتُرض لينكس نطاقَ مستخدم"
STUB_BRIDGE_NOTE = "حزمة دعم اللوحة"


def test_freestanding_without_explicit_target_is_diagnosed():
    """(AR) وضع حرّ بلا «--هدف» على مضيف ذي نظام تشغيل ⇒ تشخيص صريح بالافتراض
    (ملاحظة على لينكس، تحذير على غيره) — لا فشل صامت."""
    code, out, _ = _compile(PRINT_SOURCE, FREESTANDING)
    assert code == 0, "الترجمة فشلت (التشخيص يجب ألّا يكون خطأً):\n" + out
    assert INFERRED_TARGET_DIAG in out, (
        "لا تشخيص عند وضع حرّ بلا هدف صريح — يعود الفشل الصامت في بناء النوى:\n" + out
    )


def test_freestanding_with_explicit_bare_metal_target_is_silent():
    """(AR) حارس ضدّ الإفراط: هدف معدنيّ x86 صريح لا يستحقّ تشخيص الهدف المُستنتَج."""
    code, out, _ = _compile(PRINT_SOURCE, FREESTANDING, BARE_METAL_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    assert INFERRED_TARGET_DIAG not in out, (
        "تشخيص الهدف المُستنتَج ظهر رغم تمرير «--هدف» صراحةً (ضجيج):\n" + out
    )


def test_freestanding_explicit_linux_emits_userspace_note():
    """(AR) هدف لينكس صريح: ملاحظة «نطاق مستخدم» لا تحذير — كي لا يكسر -Werror
    بناء المضيف الحرّ. النصّ يذكر ربط -nostdlib صراحةً (لا فشلًا كاذبًا)."""
    code, out, _ = _compile(PRINT_SOURCE, FREESTANDING, HOSTED_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    # (AR) هدف صريح ⇒ لا تشخيص «هدف مُستنتَج» إطلاقًا (لا ملاحظة نطاق مستخدم هنا).
    assert LINUX_USERSPACE_NOTE not in out, (
        "ملاحظة «نطاق مستخدم» ظهرت رغم أنّ الهدف صريح — التشخيص للمُستنتَج وحده:\n" + out
    )


def test_freestanding_bare_metal_stub_arch_notes_bsp_requirement():
    """(AR) معدن عارٍ بمعمارية بلا جسر (ARM): ملاحظة صريحة أنّ جسري الإخراج/الوقت
    كعبان يحتاجان تعريف BSP — بدل نواة صامتة بلا خيط يدلّ على السبب."""
    code, out, _ = _compile(PRINT_SOURCE, FREESTANDING, ARM_BARE_TARGET)
    assert code == 0, "الترجمة فشلت:\n" + out
    assert STUB_BRIDGE_NOTE in out, (
        "لا ملاحظة BSP على معدن ARM — يبقى المطوّر أمام نواة صامتة بلا تشخيص:\n" + out
    )
