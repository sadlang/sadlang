"""
============================================================================
(AR) اختبارات منع انحدار لسدّ ح٤ (جرد فجوات المصرّف لنواة 64-بت): فرضُ رتبةِ
     مدمجاتِ النواة في builtins_os_core بالاتّجاهين.
     العطبُ المقيس قبل السدّ: `اكتب_سجل_تحكم(4096)` بوسيطٍ ناقصٍ كانت تخرج
     بصفرٍ **ولا تُصدر شيئًا** (يتبخّر النداء صامتًا: 66 نداءً بدل 67)،
     وبوسيطٍ زائدٍ تُقبَل ويُهمَل الزائد — أخطرُ صنفِ عيوبٍ في كودِ نواةٍ:
     سطرُ أمانٍ «يُنفَّذ» وهو غيرُ موجود.
     بعد السدّ: ناقصٌ = خطأ تصريف (SEM005 من الكتالوج)، زائدٌ = خطأ تصريف،
     والوسيطُ الاختياريُّ المعلَنُ يبقى اختياريًّا (حمل_جدول_واصفات/مقاطعات
     بلا وسيطٍ مستضافًا؛ رحل_صفحة بأعلامٍ أو بدونها).
(EN) Regression tests for gap ح٤: OS-core builtin arity is enforced in both
     directions (missing = compile error, excess = compile error), while
     declared-optional arguments stay optional.
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
EMIT_LLVM = "--أظهر-llvm"
TARGET_X64 = "--هدف=x86_64-unknown-elf"
# (AR) 🔑 الهدفُ المُستضافُ مُصَرَّحٌ أيضاً. هذه الحالةُ تحرُسُ المسارَ المُستضاف،
#      لكنَّ دعواها تخُصُّ عائلةَ x86 لا محالة، وبلا «--هدف» تقعُ على ثالوثِ
#      المُشَغِّل. فتُصَرَّحُ بثالوثٍ مُستضافٍ x86_64 كي يبقى المقيسُ مُستضافاً
#      والمعماريّةُ مُصَرَّحةً — لا مُورَثةً من الآلة.
# (EN) The hosted target is declared too: the case guards the hosted path but its
#      claim is x86-family, so it declares a hosted x86_64 triple instead of
#      inheriting the runner's.
TARGET_X64_HOSTED = "--هدف=x86_64-unknown-linux-gnu"


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


# ─────────────── ناقص = خطأ تصريف (لا تبخّرَ صامتًا بعد اليوم) ───────────────

def test_write_cr_missing_arg_is_compile_error():
    """(AR) اكتب_سجل_تحكم(4096) بوسيطٍ واحدٍ ⇒ خطأ تصريف، لا نداءً متبخّرًا بصفر."""
    code, out, ir = _compile("اكتب_سجل_تحكم(4096)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "الوسيط الناقص مرّ صامتًا (انحدار ح٤):\n" + out
    assert "اكتب_سجل_تحكم" in out, "التشخيص لا يسمّي المدمجة:\n" + out
    assert "%cr" not in ir, "صدر أسمبلي سجلّ تحكم رغم الخطأ"


def test_read_cr_no_args_is_compile_error():
    """(AR) اقرأ_سجل_تحكم() بلا وسيطٍ ⇒ خطأ تصريف."""
    code, out, _ = _compile("متغير ق = اقرأ_سجل_تحكم()\nاطبع(ق)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "اقرأ_سجل_تحكم بلا وسيط مرّت صامتة (انحدار ح٤):\n" + out


def test_register_isr_missing_arg_is_compile_error():
    """(AR) سجل_معالج_مقاطعة(32) بوسيطٍ واحدٍ ⇒ خطأ تصريف (يلزم رقمٌ ومعالِج)."""
    code, out, _ = _compile("سجل_معالج_مقاطعة(32)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "سجل_معالج_مقاطعة بوسيط واحد مرّت صامتة (انحدار ح٤):\n" + out


# ─────────────── زائد = خطأ تصريف (لا قبولَ بإهمالٍ بعد اليوم) ───────────────

def test_write_cr_extra_arg_is_compile_error():
    """(AR) اكتب_سجل_تحكم(3، 4096، 77) بوسيطٍ زائدٍ ⇒ خطأ تصريف، لا إهمالَ الزائد."""
    code, out, _ = _compile("اكتب_سجل_تحكم(3، 4096، 77)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "الوسيط الزائد قُبل وأُهمل (انحدار ح٤):\n" + out


def test_flush_tlb_extra_arg_is_compile_error():
    """(AR) افرغ_ذاكرة_ترجمة(5): مدمجةٌ بلا وسائطَ تُعطى وسيطًا ⇒ خطأ تصريف."""
    code, out, _ = _compile("افرغ_ذاكرة_ترجمة(5)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "الوسيط الزائد على مدمجة صفرية قُبل (انحدار ح٤):\n" + out


def test_idt_load_two_args_is_compile_error():
    """(AR) حمل_جدول_مقاطعات(1048576، 7): الوسيط الاختياري واحدٌ لا اثنان ⇒ خطأ."""
    code, out, _ = _compile("حمل_جدول_مقاطعات(1048576، 7)\n", FREESTANDING, TARGET_X64)
    assert code != 0, "وسيطان على حمل_جدول_مقاطعات قُبلا (انحدار ح٤):\n" + out


# ─────────────── الرتبة الصحيحة تبقى خضراء (لا إفراطَ في السدّ) ───────────────

def test_write_cr_correct_arity_still_compiles():
    """(AR) اكتب_سجل_تحكم(3، 4096) برتبتها ⇒ تصريفٌ ناجحٌ وأسمبلي %cr3 حاضر."""
    code, out, ir = _compile("اكتب_سجل_تحكم(3، 4096)\n", FREESTANDING, TARGET_X64)
    assert code == 0, "الرتبة الصحيحة فشلت (إفراط في السدّ):\n" + out
    assert "%cr3" in ir, "أسمبلي %cr3 غائب رغم النجاح:\n" + ir


def test_gdt_load_optional_arg_stays_optional_hosted():
    """(AR) حمل_جدول_واصفات() بلا وسيطٍ مستضافًا: الاختياريُّ المعلَن يبقى مقبولًا."""
    code, out, _ = _compile("حمل_جدول_واصفات()\n", TARGET_X64_HOSTED)
    assert code == 0, "الوسيط الاختياري صار إلزاميًّا (كسر عقد معلَن):\n" + out


def test_paging_map_with_and_without_flags():
    """(AR) رحل_صفحة بوسيطين وبثلاثةٍ كلاهما مقبول؛ بأربعةٍ خطأ."""
    code2, out2, _ = _compile("رحل_صفحة(4096، 8192)\n", FREESTANDING, TARGET_X64)
    assert code2 == 0, "رحل_صفحة بوسيطين فشلت (إفراط في السدّ):\n" + out2
    code3, out3, _ = _compile("رحل_صفحة(4096، 8192، 3)\n", FREESTANDING, TARGET_X64)
    assert code3 == 0, "رحل_صفحة بالأعلام فشلت (إفراط في السدّ):\n" + out3
    code4, out4, _ = _compile("رحل_صفحة(4096، 8192، 3، 9)\n", FREESTANDING, TARGET_X64)
    assert code4 != 0, "رحل_صفحة بأربعة وسائط قُبلت (انحدار ح٤):\n" + out4
