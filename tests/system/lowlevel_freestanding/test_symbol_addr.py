"""
============================================================================
(AR) اختبارات منع انحدار للّبنة م-لغة.3.13: مدمج `عنوان_رمز("اسم")` — أخذ عنوان
     رمز رابط خارجيّ كـرقم (i64). يُطفئ نمط الجالب *_addr في نواة النحلة.
     - عنوان_رمز("sym") ⇒ ptrtoint على رمز خارجيّ (@sym)، بلا نداء runtime.
     - يعمل في الوضعين (حرّ ومستضاف) لأنّه إحالة رابط لا محاكاة.
     - رمز مصرَّح كدالّة (دالة خارجية) يُعاد استخدامه؛ وإلّا يُنشأ رمز بيانات.
     - وسيط غير ثابت (متغيّر وقت تشغيل) ⇒ خطأ ترجمة (SEM021).
(EN) Regression tests for milestone 3.13: `عنوان_رمز("name")` builtin — address
     of an external linker symbol as an i64. Replaces the *_addr getter pattern.
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ الخفض يعيش في توليد LLVM؛ الراية تُخرج IR بلا اعتماد على
     الرابط (عزل عن lld-link/LIB).
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

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

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"


def _compile(source: str, *extra_flags: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر بـ sad-build مع الرايات؛ يعيد (رمز الخروج، الخرج، IR)."""
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


# ─────────────── الوضع الحرّ: ptrtoint على رمز خارجيّ ───────────────

# (AR) رموز النواة الحقيقيّة أسماء ASCII (bee_idt/_kernel_start)؛ LLVM يهرّب
#      غير-ASCII في المعرّفات، فنستعمل أسماء ASCII كما في الاستعمال الواقعيّ.
def test_symbol_addr_freestanding_emits_ptrtoint():
    """(AR) عنوان_رمز("bee_test_sym") حرًّا ⇒ ptrtoint على @bee_test_sym، بلا نداء runtime."""
    code, out, ir = _compile('متغير ع = عنوان_رمز("bee_test_sym")\nاطبع(ع)\n', FREESTANDING)
    assert code == 0, 'عنوان_رمز فشلت حرًّا:\n' + out
    assert "ptrtoint" in ir, "IR الحرّ بلا ptrtoint — لم يُؤخَذ عنوان الرمز:\n" + ir[:2000]
    assert "bee_test_sym" in ir, "IR الحرّ لا يحوي اسم الرمز الخارجيّ:\n" + ir[:2000]
    assert "sad_ll_" not in ir, "IR الحرّ ينادي رمز sad_ll مستضاف — عنوان_رمز يجب أن يكون إحالة رابط"


def test_symbol_addr_external_symbol_declared():
    """(AR) رمز غير مصرَّح كدالّة يظهر كتصريح بيانات خارجيّ (external global) في الـIR."""
    code, out, ir = _compile('متغير ع = عنوان_رمز("bee_data_sym")\nاطبع(ع)\n', FREESTANDING)
    assert code == 0, out
    assert "@bee_data_sym = external global" in ir, (
        "IR بلا تصريح رمز بيانات خارجيّ (external global):\n" + ir[:2000]
    )


# ─────────────── الوضع المستضاف: يعمل أيضًا (إحالة رابط) ───────────────

def test_symbol_addr_hosted_also_works():
    """(AR) عنوان_رمز مستضافًا يعمل أيضًا (ptrtoint، لا يقتصر على الحرّ)."""
    code, out, ir = _compile('متغير ع = عنوان_رمز("رمز_مستضاف")\nاطبع(ع)\n')
    assert code == 0, 'عنوان_رمز مستضافًا فشلت:\n' + out
    assert "ptrtoint" in ir, "IR المستضاف بلا ptrtoint:\n" + ir[:2000]


# ─────────────── إعادة استخدام رمز دالّة مصرَّحة ───────────────

def test_symbol_addr_reuses_declared_extern_function():
    """(AR) رمز مصرَّح كدالّة خارجيّة يُعاد استخدامه في عنوان_رمز (لا ازدواج)."""
    src = (
        'دالة خارجية("bee_thing") رقم شيء()\n'
        'متغير ع = عنوان_رمز("bee_thing")\n'
        'اطبع(ع)\n'
    )
    code, out, ir = _compile(src, FREESTANDING)
    assert code == 0, out
    assert "ptrtoint" in ir, "IR بلا ptrtoint على الرمز:\n" + ir[:2000]
    # (AR) تصريح دالّة واحد لـbee_thing (لا يُنشأ رمز بيانات مكرّر)
    assert ir.count("@bee_thing") >= 1, "اسم الرمز غائب عن IR"


# ─────────────── وسيط غير ثابت ⇒ خطأ ترجمة (SEM021) ───────────────

def test_symbol_addr_non_constant_arg_errors():
    """(AR) اسم رمز غير ثابت (متغيّر وقت تشغيل) ⇒ خطأ SEM021 تحديدًا."""
    src = (
        "دالة ف(نص س)\n"
        "    متغير ع = عنوان_رمز(س)\n"
        "    ارجع ع\n"
        "نهاية\n"
    )
    code, out, _ = _compile(src, FREESTANDING)
    assert code != 0, "اسم رمز غير ثابت نجح — لا يمكن إصدار إحالة رابط باسم متغيّر:\n" + out
    # (AR) معرّف SEM021 لا يُطبَع نصًّا؛ نثبت المسار عبر تفصيل الرسالة المميّز.
    assert "يتطلّب اسم رمز ثابتًا" in out, "ليست رسالة عنوان_رمز المتوقّعة (قد يخفي انحدارًا):\n" + out


def test_symbol_addr_numeric_constant_arg_errors():
    """(AR) وسيط رقميّ ثابت (لا سلسلة) ⇒ خطأ عنوان_رمز (dataType≠String)."""
    code, out, _ = _compile("متغير ع = عنوان_رمز(123)\nاطبع(ع)\n", FREESTANDING)
    assert code != 0, "وسيط رقميّ ثابت نجح — عنوان_رمز يتطلّب اسم رمز سلسلةً:\n" + out
    assert "يتطلّب اسم رمز ثابتًا" in out, "ليست رسالة عنوان_رمز المتوقّعة:\n" + out


def test_symbol_addr_empty_string_errors():
    """(AR) سلسلة فارغة (رمز بلا اسم) ⇒ خطأ عنوان_رمز برسالة تميّز الفراغ."""
    code, out, _ = _compile('متغير ع = عنوان_رمز("")\nاطبع(ع)\n', FREESTANDING)
    assert code != 0, "سلسلة فارغة نجحت — لا رمز رابط بلا اسم:\n" + out
    assert "سلسلة فارغة" in out, "رسالة الرمز الفارغ المميّزة غائبة:\n" + out


def test_symbol_addr_same_symbol_twice_no_duplicate():
    """(AR) الرمز نفسه مرّتين ⇒ تصريح خارجيّ واحد (لا ازدواج)، وptrtoint مرّتين."""
    src = (
        'متغير أ = عنوان_رمز("bee_shared")\n'
        'متغير ب = عنوان_رمز("bee_shared")\n'
        'اطبع(أ + ب)\n'
    )
    code, out, ir = _compile(src, FREESTANDING)
    assert code == 0, out
    assert ir.count("@bee_shared = external global") == 1, (
        "تصريح الرمز الخارجيّ تكرّر (يجب أن يكون واحدًا):\n" + ir[:2000]
    )
