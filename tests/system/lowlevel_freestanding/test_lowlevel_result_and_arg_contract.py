"""
============================================================================
(AR) اختبارات منع انحدارٍ لعقدِ المدمجاتِ المنخفضةِ (ح٥/أ + دَينُ الأنواع):
     ① **ربطُ الناتج**: سبعون باعثًا لـUEFI/ACPI/APIC كانت تُصدر النداءَ ولا
        تربطُ قيمتَه بسجلِّها الناتج، فيسقطُ المصرّفُ بـ«Undefined register:%0»
        عندَ أوّلِ استهلاكٍ للعائد (إسنادٌ أو إرجاع). الربطُ اليومَ في طبقةِ
        الإرسالِ وحدَها فيعمُّ الباعثينَ كلَّهم.
     ② **ملاءمةُ الوسائط**: الباعثُ قد يبني وسائطَ i64 بينما يُصرَّحُ الرمزُ
        التشغيليُّ i32، فيسقطُ verifyModule («تعارضُ توقيعات»). المقيسُ:
        `apic_تهيئة_مؤقت` بوسائطَ صريحة.
(EN) Regression tests for the low-level builtin contract (gap ح٥/a + type debt):
     result binding at the dispatch layer, and argument coercion to the declared
     runtime parameter types.
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

_CANDIDATES = [
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build",
    ROOT / "build" / "bin" / "sad-build",
    ROOT / "build" / "bin" / "Debug" / "sad-build",
]
# (AR) أحدثُ الموجودَين لا أوّلُهما (فخُّ الثنائيِّ العتيق — نمطُ أخوات الملفّ).
SAD_BUILD = max(
    (p for p in _CANDIDATES if p.exists()),
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
NO_MAIN = "--بلا-رئيسية"
EMIT_LLVM = "--أظهر-llvm"
TARGET_X64 = "--هدف=x86_64-unknown-elf"


def _compile(source: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر في الوضع الحرّ؛ يعيد (رمز الخروج، الخرج، IR)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), FREESTANDING, NO_MAIN, "-O0",
             TARGET_X64, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _assert_clean(code: int, out: str, who: str) -> None:
    assert "Undefined register" not in out, f"{who}: الناتج لم يُربَط بسجلّه (انحدار ح٥/أ):\n{out}"
    assert "verifyModule" not in out, f"{who}: verifyModule سقط (انحدار عقد الوسائط):\n{out}"
    assert code == 0, f"{who}: التصريف فشل:\n{out}"


# ─────────────────── ① ربطُ الناتج (ح٥/أ) ───────────────────

def test_uefi_init_return_is_consumable():
    """(AR) `متغير ح = uefi_تهيئة()` ثمّ إرجاعُه: عائدُ UEFI يُستهلَك بلا سقوط."""
    code, out, ir = _compile(
        "دالة رقم جرب()\n"
        "    متغير ح = uefi_تهيئة()\n"
        "    ارجع ح\n"
        "نهاية\n"
    )
    _assert_clean(code, out, "uefi_تهيئة")
    assert "@sad_ll_uefi_init" in ir, "لم يُصدَر نداءُ التهيئة:\n" + ir[:1500]


def test_acpi_version_return_is_consumable():
    """(AR) عائدُ ACPI يُستهلَك كذلك — الربطُ في طبقةِ الإرسالِ يعمُّ العائلةَ كلَّها."""
    code, out, _ = _compile(
        "دالة رقم جرب()\n"
        "    متغير ن = acpi_إصدار()\n"
        "    ارجع ن\n"
        "نهاية\n"
    )
    _assert_clean(code, out, "acpi_إصدار")


def test_apic_id_return_is_consumable():
    """(AR) وعائدُ APIC — ثالثُ العائلاتِ التي كانت بلا ربط."""
    code, out, _ = _compile(
        "دالة رقم جرب()\n"
        "    متغير م = apic_معرّف()\n"
        "    ارجع م\n"
        "نهاية\n"
    )
    _assert_clean(code, out, "apic_معرّف")


def test_cpu_read_cr_return_still_consumable():
    """(AR) حارسُ عدمِ الانحدار: باعثٌ كان يربطُ ناتجَه بنفسِه يبقى سليمًا (لا ازدواج)."""
    code, out, _ = _compile(
        "دالة رقم جرب()\n"
        "    متغير ق = اقرأ_سجل_تحكم(3)\n"
        "    ارجع ق\n"
        "نهاية\n"
    )
    _assert_clean(code, out, "اقرأ_سجل_تحكم")


# ─────────────────── ② ملاءمةُ الوسائط للتوقيع ───────────────────

def test_apic_init_timer_explicit_args_match_declaration():
    """(AR) `apic_تهيئة_مؤقت(32، 0، 3)`: الوسائطُ تُلاءَم i32 كما يُصرَّحُ الرمزُ."""
    code, out, ir = _compile(
        "دالة اضبط()\n"
        "    apic_تهيئة_مؤقت(32، 0، 3)\n"
        "نهاية\n"
    )
    _assert_clean(code, out, "apic_تهيئة_مؤقت")
    assert "@sad_ll_apic_init_timer(i32" in ir, (
        "الوسائطُ لم تُلاءَم لنوعِ المعاملِ المصرَّح:\n" + ir[:1500]
    )


def test_i64_typed_runtime_call_unchanged():
    """(AR) لا إفراطَ: نداءٌ معاملاتُه i64 أصلًا يبقى i64 بلا تحويلٍ مُقحَم."""
    code, out, ir = _compile(
        "دالة اضبط()\n"
        "    رحل_صفحة(4096، 8192)\n"
        "نهاية\n"
    )
    _assert_clean(code, out, "رحل_صفحة")
    assert "@sad_ll_paging_map(i64" in ir, "توقيعُ الترحيلِ تبدّل:\n" + ir[:1500]


# ─────────────────── ③ اسمٌ قانونيٌّ قابلٌ للنداء ───────────────────

@pytest.mark.parametrize("call", [
    "apic_معرف()",
    "apic_عطل_pic()",
    "apic_وجه_irq(1، 32، 0)",
])
def test_apic_shadda_free_names_are_callable(call):
    """(AR) ثلاثُ مدمجاتٍ معلَنةٍ `stable` كان اسمُها القانونيُّ يحملُ شدّةً بينما
    يجرّدها المُشكِّلُ من المعرّفات ⇒ «استدعاء دالة غير معرّفة» أبدًا. صُحِّح
    الاسمُ في مصدرِ الحقيقةِ ووُلِّد السجلّ؛ والحارسُ البنيويُّ في x.py يمنعُ العود."""
    code, out, _ = _compile(f"دالة اضبط()\n    {call}\nنهاية\n")
    assert "undefined function call" not in out and "غير معرّفة" not in out, (
        f"{call}: الاسمُ القانونيُّ غيرُ قابلٍ للنداء (انحدارُ الشدّة):\n{out}"
    )
    _assert_clean(code, out, call)
