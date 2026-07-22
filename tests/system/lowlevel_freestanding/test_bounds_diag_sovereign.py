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

_CANDIDATES = [
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
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
