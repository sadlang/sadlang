"""
============================================================================
(AR) تبعةُ SEM005 واحدةٌ مهما كانت الطبقةُ التي وقع فيها الخطأ.

     كانت حرّاسُ `builtins_ui.cpp` **تطبعُ التشخيصَ ويمضي البناءُ ناجحًا**:
     `عين_النص(1)` تُخرِج رسالةَ «عدد معاملات خاطئ» ثمّ يخرج المصرّفُ بصفرٍ
     ويُنتج ثنائيًّا **بلا النداء**. وهو صنفُ الفجوة ح٤ في جرد نواة 64-بت
     عينُه: سطرٌ «يُنفَّذ» وهو غيرُ موجود، لا يُخفِق فلا يُرى.

     🔑 والمقيسُ ههنا **رمزُ الخروج لا الطباعة**: التأكيدُ على نصِّ الرسالةِ
        وحدَه كان أخضرَ قبل السدّ وبعده — مرساةٌ لا تفصل بين الحالتين.

(EN) SEM005 must have one consequence regardless of which builder raised it.
     The UI guards used to print and let the build succeed with the call gone;
     these tests anchor on the exit code, not on the message text.
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

# (AR) مرساةُ التشخيصِ من كتالوجِ الأخطاءِ لا نصٌّ يدويّ: تبعةُ SEM005.
SEM005_ANCHOR = "من المعاملات"


def _build(source: str) -> tuple[int, str]:
    """(AR) يترجم المصدر؛ يعيد (رمز الخروج، الخرج الموحَّد)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), "--أظهر-llvm", "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


def test_ui_too_few_args_fails_the_build():
    """(AR) عين_النص بوسيطٍ واحدٍ من اثنين: يُردّ، لا يُطبَع وحسب."""
    code, out = _build("دالة رئيسية()\n    عين_النص(1)\nنهاية\n")
    assert code != 0, "بناءٌ ناجحٌ ونداءٌ متبخّر — تبعةُ SEM005 لم تُوحَّد:\n" + out
    assert SEM005_ANCHOR in out, "الإخفاقُ ليس إخفاقَ عددِ المعاملات:\n" + out


def test_ui_add_child_too_few_args_fails_the_build():
    """(AR) أخٌ ثانٍ من العائلةِ نفسِها — الرقعةُ لا تُسدُّ في موضعٍ وتُترك في أخيه."""
    code, out = _build("دالة رئيسية()\n    أضف_ابن(1)\nنهاية\n")
    assert code != 0, "أضف_ابن الناقصةُ ما زالت تمضي:\n" + out
    assert SEM005_ANCHOR in out, "الإخفاقُ ليس إخفاقَ عددِ المعاملات:\n" + out


def test_program_without_ui_builtins_still_builds():
    """(AR) ضابطٌ: السدُّ يردُّ الخطأَ ولا يردُّ الصحيح."""
    code, out = _build("دالة رئيسية()\n    اطبع(1)\nنهاية\n")
    assert code == 0, "برنامجٌ سليمٌ رُدَّ — السدُّ وسّع الرفض:\n" + out
