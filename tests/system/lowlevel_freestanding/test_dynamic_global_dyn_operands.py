"""
============================================================================
(AR) اختبارات منع انحدار لدَين أشقّاء «علّة قسمة العامّ» (ح٦ في جرد نواة 64-بت):
     عامٌّ يُرقَّى إلى `%SadDyn` بإسنادِ ناتجِ قسمةٍ ديناميكيّةٍ (`//`) ثمّ يُمرَّر
     إلى مدمجةٍ منخفضةٍ. قبل السدّ كان الخفضُ يقصُّ البنيةَ عمياءَ عن وسمِها
     (`zext %SadDyn to i64`) أو يمرِّرُها لمعاملٍ i64 — فيسقط verifyModule
     برسالةِ «علّة مترجم — أبلِغ» ويُحظَر استعمالُ القسمةِ في كودِ النواة.
     المقيسُ هنا مسارُ نواةِ النحلةِ نفسُه: كتابةُ CR3، وإبطالُ صفحةٍ في TLB،
     وتحميلُ واصفِ جدولٍ، ونداءُ وقتِ تشغيلٍ حرٌّ (ترحيلُ صفحة).
(EN) Regression tests for the sibling debt of the global-division bug (gap ح٦):
     a global promoted to %SadDyn by a dynamic `//` result, passed to low-level
     builtins. Used to fail verifyModule; now unboxed tag-respecting.
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

# (AR) مقدّمةٌ مشتركة: عامٌّ يصير ديناميكيًّا بناتجِ `//` على نداءِ دالّة (لا ثابتٍ
#      يطويه المصرّف) — وهو المحفِّزُ المقيسُ في نواةِ النحلة حرفيًّا.
PRELUDE = (
    "متغير عام_قيمة = 0\n"
    "متغير عام_حجم = 8388608\n"
    "\n"
    "دالة رقم خطوة()\n"
    "    ارجع 4096\n"
    "نهاية\n"
    "\n"
)


def _compile(body: str) -> tuple[int, str, str]:
    """(AR) يترجم المقدّمةَ مع الجسم في الوضع الحرّ؛ يعيد (رمز الخروج، الخرج، IR)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(PRELUDE + body, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), FREESTANDING, NO_MAIN, "-O0",
             TARGET_X64, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _assert_verified(code: int, out: str, who: str) -> None:
    assert "verifyModule" not in out, f"{who}: verifyModule سقط على %SadDyn (انحدار ح٦):\n{out}"
    assert "ZExt only operates on integer" not in out, f"{who}: قصٌّ أعمى عن الوسم (انحدار ح٦):\n{out}"
    assert code == 0, f"{who}: التصريف فشل (انحدار ح٦):\n{out}"


def test_dynamic_global_into_write_cr_verifies():
    """(AR) اكتب_سجل_تحكم(3، عامٌّ ديناميكيّ): بوّابةُ إدارةِ الذاكرة — تُفَكُّ بوسمِها."""
    body = (
        "دالة اضبط()\n"
        "    عام_قيمة = عام_حجم // خطوة()\n"
        "    اكتب_سجل_تحكم(3، عام_قيمة)\n"
        "نهاية\n"
    )
    code, out, ir = _compile(body)
    _assert_verified(code, out, "اكتب_سجل_تحكم")
    assert "mov $0, %cr3" in ir, "لم تُصدَر كتابةُ CR3:\n" + ir[:2000]


def test_dynamic_global_into_invlpg_verifies():
    """(AR) ابطل_صفحة(عامٌّ ديناميكيّ): إبطالُ TLB — لا يقصُّ البنيةَ عمياء."""
    body = (
        "دالة اضبط()\n"
        "    عام_قيمة = عام_حجم // خطوة()\n"
        "    ابطل_صفحة(عام_قيمة)\n"
        "نهاية\n"
    )
    code, out, ir = _compile(body)
    _assert_verified(code, out, "ابطل_صفحة")
    assert "invlpg" in ir, "لم تُصدَر invlpg:\n" + ir[:2000]


def test_dynamic_global_into_idt_load_verifies():
    """(AR) حمل_جدول_مقاطعات(عامٌّ ديناميكيّ): واصفُ الجدولِ يُقَصُّ لعرضِ سجلِّ الهدف."""
    body = (
        "دالة اضبط()\n"
        "    عام_قيمة = عام_حجم // خطوة()\n"
        "    حمل_جدول_مقاطعات(عام_قيمة)\n"
        "نهاية\n"
    )
    code, out, _ = _compile(body)
    _assert_verified(code, out, "حمل_جدول_مقاطعات")


def test_dynamic_global_into_runtime_call_arg_verifies():
    """(AR) رحل_صفحة بمعاملٍ ديناميكيّ: نداءُ وقتِ تشغيلٍ معاملُه i64 — لا تُمرَّر بنيةٌ."""
    body = (
        "دالة اضبط()\n"
        "    عام_قيمة = عام_حجم // خطوة()\n"
        "    رحل_صفحة(عام_قيمة، 4096)\n"
        "نهاية\n"
    )
    code, out, _ = _compile(body)
    _assert_verified(code, out, "رحل_صفحة")


def test_static_operand_lowering_unchanged():
    """(AR) لا إفراطَ في السدّ: معاملٌ ساكنُ النوعِ يبقى خفضُه كما كان (هويّة)."""
    body = (
        "دالة اضبط()\n"
        "    اكتب_سجل_تحكم(3، 4096)\n"
        "    ابطل_صفحة(8192)\n"
        "نهاية\n"
    )
    code, out, ir = _compile(body)
    _assert_verified(code, out, "معاملٌ ساكن")
    assert "extractvalue %SadDyn" not in ir, (
        "معاملٌ ساكنٌ مرَّ بفكِّ وسمٍ لا لزومَ له (إفراط في السدّ):\n" + ir[:2000]
    )
