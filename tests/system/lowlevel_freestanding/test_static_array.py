"""
============================================================================
(AR) اختبارات منع انحدار للّبنة م-لغة.3.16: مصفوفة تخزين ساكن مصفَّرة في .bss —
     «متغير ساكن اسم مصفوفة[N]» ⇒ ‏[N x i8] zeroinitializer.
     - قابلة للكتابة (global لا constant)، محاذاة 16، رمز @رمز مُصدَّر أو داخليّ.
     - نهلة تعنونها عبر عنوان_رمز + اكتب_ذاكرة32/اقرأ_ذاكرة32 بإزاحات (بايتيّة).
     - SEM023: الحجم عدد صحيح حرفيّ موجب.
(EN) Regression tests for milestone 3.16: named zero-filled static .bss array.
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ التخصيص .bss ومحاذاته يعيشان في إصدار LLVM. أسماء @رمز
     ASCII (كرموز النواة) — LLVM يهرّب غير-ASCII.
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


# ─────────────── @رمز: مصفوفة .bss مصفَّرة مُصدَّرة ───────────────

def test_static_array_exports_zeroed_bss():
    """(AR) @رمز("bee_tbl") متغير ساكن جدول مصفوفة[2048] ⇒ @bee_tbl = global [2048 x i8] zeroinitializer."""
    code, out, ir = _compile('@رمز("bee_tbl") متغير ساكن جدول مصفوفة[2048]\n', FREESTANDING)
    assert code == 0, 'مصفوفة ساكنة فشلت:\n' + out
    assert "@bee_tbl = global [2048 x i8] zeroinitializer" in ir, (
        "لا مصفوفة .bss مصفَّرة مُصدَّرة [2048 x i8]:\n" + ir[:2000]
    )
    assert "@bee_tbl = constant" not in ir, "المصفوفة الساكنة صارت constant (يجب أن تكون قابلة للكتابة .bss)"


def test_static_array_internal_when_no_symbol():
    """(AR) بلا @رمز ⇒ رمز داخليّ (internal global) — لا تسريب تصدير."""
    code, out, ir = _compile('متغير ساكن ذاكرة مصفوفة[64]\n', FREESTANDING)
    assert code == 0, out
    assert "internal global [64 x i8] zeroinitializer" in ir, (
        "المصفوفة بلا @رمز ليست internal مصفَّرة:\n" + ir[:1500]
    )


def test_static_array_aligned_16():
    """(AR) المحاذاة 16 (تسمح وصولًا i32/i64 مُحاذًى — نهلة تكتب بـاكتب_ذاكرة32)."""
    code, out, ir = _compile('@رمز("bee_al") متغير ساكن م مصفوفة[128]\n', FREESTANDING)
    assert code == 0, out
    assert "align 16" in ir, "المصفوفة الساكنة غير محاذاة على 16:\n" + ir[:1500]


# ─────────────── تكامل عنوان_رمز: العنونة بالرمز ───────────────

def test_static_array_addressable_by_symbol():
    """(AR) عنوان_رمز يأخذ عنوان المصفوفة الساكنة ⇒ نهلة تعنونها (تكامل 3.13+3.16)."""
    code, out, ir = _compile(
        '@رمز("bee_pmm") متغير ساكن خريطة مصفوفة[131072]\n'
        'متغير ع = عنوان_رمز("bee_pmm")\nاكتب_ذاكرة32(ع، 5)\n', FREESTANDING)
    assert code == 0, out
    assert "@bee_pmm = global [131072 x i8] zeroinitializer" in ir, "لا مصفوفة 128KB:\n" + ir[:1500]
    assert "ptrtoint (ptr @bee_pmm" in ir or "@bee_pmm" in ir, "عنوان_رمز لم يأخذ عنوان المصفوفة"


# ─────────────── SEM023: حجم موجب ───────────────

def test_static_array_zero_size_errors():
    """(AR) مصفوفة[0] ⇒ خطأ SEM023 (الحجم يجب أن يكون موجبًا)."""
    code, out, _ = _compile('متغير ساكن خ مصفوفة[0]\n', FREESTANDING)
    assert code != 0, "مصفوفة ساكنة بحجم صفر نجحت:\n" + out
    assert "SEM023" in out or "موجب" in out or "موجبًا" in out, "ليست رسالة SEM023:\n" + out


# ─────────────── حارس التصادم SEM022 (مُعاد من 3.14) ───────────────

def test_static_array_reserved_name_errors():
    """(AR) @رمز("memset") على مصفوفة ساكنة ⇒ SEM022 (اسم محجوز لزمن التشغيل)."""
    code, out, _ = _compile('@رمز("memset") متغير ساكن خ مصفوفة[16]\n', FREESTANDING)
    assert code != 0, "@رمز باسم محجوز على مصفوفة ساكنة نجح:\n" + out
    assert "محجوز" in out or "SEM022" in out or "memset" in out, "ليست رسالة تصادم الرمز:\n" + out


# ─────────────── حارس: المستوى الأعلى حصرًا (رصد Amelia) ───────────────

def test_static_array_inside_function_rejected():
    """(AR) مصفوفة ساكنة داخل دالّة ⇒ رفض صريح (تخزين .bss عالميّ فقط، لا توليد خاطئ صامت)."""
    code, out, _ = _compile(
        'دالة ف()\n    متغير ساكن محلي مصفوفة[8]\nنهاية\n', FREESTANDING)
    assert code != 0, "مصفوفة ساكنة داخل دالّة نجحت (يجب رفضها — تخزين عالميّ فقط):\n" + out
    assert "المستوى الأعلى" in out or "SEM023" in out, "ليست رسالة حصر المستوى الأعلى:\n" + out
