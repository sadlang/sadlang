"""
============================================================================
(AR) اختبارات منع انحدار للّبنة م-لغة.3.14: تخزين ساكن مسمّى — البدائيّتان
     الأساسيّتان @رمز + متطاير (لصيقة لاحقة).
     - @رمز("اسم") متغير/ثابت ⇒ رمز رابط ثابت مُصدَّر (ExternalLinkage) باسم
       مُعطًى بدل الاسم العربيّ الداخليّ المُشوَّه.
     - متغير متطاير ⇒ كلّ قراءة/كتابة للمخزن volatile (لصيقة لاحقة عربيّة سليمة).
     - حارس ضدّ الإفراط: المتغيّر العاديّ يبقى internal بلا volatile.
(EN) Regression tests for milestone 3.14: named static storage — @رمز + postfix متطاير.
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ سمات التخزين تعيش في إصدار LLVM (الربط/volatile)؛
     الراية تُخرج IR بلا اعتماد على الرابط.
     ملاحظة: أسماء الرموز في @رمز أسماء ASCII (كرموز النواة)؛ LLVM يهرّب الأسماء
     غير-ASCII في المعرّفات.
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


# ─────────────── @رمز: رمز رابط مُصدَّر مستقرّ ───────────────

def test_symbol_directive_exports_stable_name():
    """(AR) @رمز("bee_counter") متغير ⇒ @bee_counter عالميّ (لا internal، لا اسم مُشوَّه)."""
    code, out, ir = _compile('@رمز("bee_counter") متغير عداد = 0\n', FREESTANDING)
    assert code == 0, '@رمز فشلت:\n' + out
    assert "@bee_counter = global" in ir, (
        "لا رمز مُصدَّر @bee_counter (ExternalLinkage):\n" + ir[:2000]
    )
    assert "@bee_counter = internal" not in ir, "الرمز داخليّ — @رمز لم يُصدِّره"


def test_symbol_directive_const_rodata():
    """(AR) @رمز("bee_ro") ثابت ⇒ @bee_ro constant (‏.rodata) مُصدَّر."""
    code, out, ir = _compile('@رمز("bee_ro") ثابت قيمة = 42\n', FREESTANDING)
    assert code == 0, out
    assert "@bee_ro = constant" in ir, "الثابت المُصدَّر ليس constant (.rodata):\n" + ir[:2000]


# ─────────────── متغير متطاير: كلّ الوصول volatile ───────────────

def test_volatile_directive_marks_store_volatile():
    """(AR) متغير متطاير ⇒ الكتابة store volatile (لصيقة لاحقة)."""
    code, out, ir = _compile('@رمز("bee_v") متغير متطاير ع = 0\nع = 5\n', FREESTANDING)
    assert code == 0, out
    assert "store volatile" in ir, "الكتابة ليست volatile — @متطاير لم يُطبَّق:\n" + ir[:2000]


def test_volatile_directive_marks_read_volatile():
    """(AR) متطاير ⇒ القراءة في تعبير load volatile (التمريرة اللاحقة تعلّم كلّ حمل)."""
    code, out, ir = _compile(
        '@رمز("bee_vr") متغير متطاير ع = 0\nمتغير س = ع + 1\nاطبع(س)\n', FREESTANDING)
    assert code == 0, out
    assert "load volatile" in ir, "لا قراءة volatile — التمريرة اللاحقة لم تعلّم الحمل:\n" + ir[:2000]


# ─────────────── حارس ضدّ الإفراط: العاديّ لا يتأثّر ───────────────

def test_plain_global_stays_internal_nonvolatile():
    """(AR) متغيّر عاديّ (بلا @رمز/@متطاير) يبقى internal وبلا volatile."""
    code, out, ir = _compile('متغير عادي = 7\nعادي = عادي + 1\nاطبع(عادي)\n', FREESTANDING)
    assert code == 0, out
    assert "internal global" in ir, "المتغيّر العاديّ لم يعد internal (تسريب @رمز):\n" + ir[:2000]
    # (AR) لا كتابة/قراءة volatile للمتغيّر العاديّ
    assert "store volatile" not in ir, "المتغيّر العاديّ صار volatile (تسريب متطاير)"


# ─────────────── تركيب: @رمز فوق متطاير معًا ───────────────

def test_const_volatile_compose():
    """(AR) ثابت متطاير ⇒ لصيقة متطاير تعمل على الثابت أيضًا (قراءته load volatile)."""
    code, out, ir = _compile(
        '@رمز("bee_cv") ثابت متطاير ق = 5\nمتغير س = ق + 1\nاطبع(س)\n', FREESTANDING)
    assert code == 0, out
    assert "load volatile" in ir, "ثابت متطاير لم يُنتج قراءة volatile:\n" + ir[:1500]


# ─────────────── حارس تصادم رمز @رمز (SEM022) ───────────────

def test_symbol_reserved_runtime_name_errors():
    """(AR) @رمز("memset") ⇒ خطأ SEM022 (اسم محجوز لزمن التشغيل، يُفسد النداءات)."""
    code, out, _ = _compile('@رمز("memset") متغير خ = 0\n', FREESTANDING)
    assert code != 0, "@رمز باسم محجوز (memset) نجح — يُفسد زمن التشغيل:\n" + out
    assert "محجوز" in out or "SEM022" in out or "memset" in out, "ليست رسالة تصادم الرمز:\n" + out


def test_symbol_duplicate_name_errors():
    """(AR) رمزان @رمز بنفس الاسم ⇒ خطأ SEM022 (تكرار، الرابط يعيد التسمية صامتًا)."""
    src = '@رمز("bee_dup") متغير أ = 0\n@رمز("bee_dup") متغير ب = 1\n'
    code, out, _ = _compile(src, FREESTANDING)
    assert code != 0, "رمزان بنفس الاسم نجحا — تصادم صامت:\n" + out
    assert "تكرار" in out or "SEM022" in out or "موجود" in out, "ليست رسالة التكرار:\n" + out


# ─────────────── بايتات: كتلة .rodata مسمّاة ───────────────

def test_byte_blob_rodata_constant():
    """(AR) @رمز("bee_tbl") ثابت = بايتات(...) ⇒ @bee_tbl = constant [N x i8] في .rodata."""
    code, out, ir = _compile(
        '@رمز("bee_tbl") ثابت جدول = بايتات(0x53، 0x41، 0x44)\n', FREESTANDING)
    assert code == 0, 'بايتات فشلت:\n' + out
    assert "@bee_tbl = constant [3 x i8]" in ir, (
        "لا كتلة بايتات ثابتة [3 x i8] بالرمز المُصدَّر:\n" + ir[:2000]
    )
    assert 'c"SAD"' in ir, "محتوى الكتلة ليس c\"SAD\" (خطأ تحليل بايتات):\n" + ir[:2000]


def test_byte_blob_addressable_by_symbol():
    """(AR) عنوان_رمز يأخذ عنوان كتلة بايتات (تكامل 3.13+3.14)."""
    code, out, ir = _compile(
        '@رمز("bee_arch") ثابت أرشيف = بايتات(1، 2، 3، 4)\n'
        'متغير ع = عنوان_رمز("bee_arch")\nاطبع(ع)\n', FREESTANDING)
    assert code == 0, out
    assert "@bee_arch = constant [4 x i8]" in ir, "لا كتلة بايتات:\n" + ir[:1500]
    assert "ptrtoint (ptr @bee_arch" in ir or "@bee_arch" in ir, "عنوان_رمز لم يأخذ عنوان الكتلة"


def test_byte_blob_out_of_range_errors():
    """(AR) بايتات بقيمة > 255 ⇒ خطأ ترجمة (مدى [0,255])."""
    code, out, _ = _compile('ثابت خ = بايتات(300)\n', FREESTANDING)
    assert code != 0, "بايتات بقيمة خارج المدى نجحت:\n" + out
    assert "مدى" in out or "255" in out, "ليست رسالة المدى:\n" + out


def test_byte_blob_empty_errors():
    """(AR) بايتات() فارغة ⇒ خطأ (كتلة بلا معنى)."""
    code, out, _ = _compile('ثابت خ = بايتات()\n', FREESTANDING)
    assert code != 0, "بايتات() فارغة نجحت:\n" + out
    assert "فارغة" in out, "ليست رسالة القائمة الفارغة:\n" + out


def test_byte_blob_mutable_is_data_not_rodata():
    """(AR) متغير = بايتات(...) (غير ثابت) ⇒ [N x i8] قابل للكتابة (لا constant)."""
    code, out, ir = _compile('@رمز("bee_mut") متغير م = بايتات(1، 2)\n', FREESTANDING)
    assert code == 0, out
    assert "@bee_mut = global [2 x i8]" in ir, "غير الثابت ليس global قابلًا للكتابة:\n" + ir[:1500]
    assert "@bee_mut = constant" not in ir, "غير الثابت صار constant خطأً"


# ─────────────── تركيب: @رمز فوق متطاير معًا ───────────────

def test_symbol_and_volatile_compose():
    """(AR) @رمز("bee_pv") متغير متطاير ⇒ رمز مُصدَّر + كلّ الوصول volatile معًا."""
    code, out, ir = _compile(
        '@رمز("bee_pv") متغير متطاير عد = 0\nعد = عد + 1\n', FREESTANDING)
    assert code == 0, out
    assert "@bee_pv = global" in ir, "الرمز غير مُصدَّر عند التركيب:\n" + ir[:1500]
    assert "store volatile" in ir and "load volatile" in ir, (
        "الوصول غير متطاير عند التركيب:\n" + ir[:1500]
    )
