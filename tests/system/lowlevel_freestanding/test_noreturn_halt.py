"""
============================================================================
(AR) اختبارات منع انحدار للّبنة م-لغة.3.15: «دالة لا_ترجع» + إعادة تسمية مدمج
     الإسبات (hlt).
     - أسبت_المعالج() ⇒ تعليمة hlt (المدمج المُعاد تسميته من «توقف» المتصادم
       مع break). «توقف» عادت break حصرًا.
     - دالة لا_ترجع اسم(...) ⇒ سمة LLVM noreturn (المُعدِّل يلي «دالة» قبل الاسم،
       نمط «متغير متطاير»، بلا سيجيل @).
----------------------------------------------------------------------------
(AR) لماذا فرق العدّ لا الوجود المطلق؟ إصدار الوضع الحرّ يحوي دائمًا التعريف
     الضعيف __sad_panic بسمة noreturn، فـ"noreturn in ir" صحيح تفاهةً. نقارن
     نفس الجسم بمُعدِّل وبدونه: الفارق = أثر المُعدِّل وحده. ونفس الجسم بمدمج
     الإسبات مقابل break: الفارق = أثر hlt وحده.
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
SAD_BUILD = next((p for p in _CANDIDATES if p.exists()), None)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"

# (AR) أجسام متطابقة عدا المتغيّر المدروس — لعزل أثر المُعدِّل/المدمج وحده.
_LOOP_HALT = "دالة قف()\n    بينما (صحيح)\n        أسبت_المعالج()\n    نهاية\nنهاية\n"
_LOOP_BREAK = "دالة قف()\n    بينما (صحيح)\n        توقف\n    نهاية\nنهاية\n"
_LOOP_NORETURN = "دالة لا_ترجع قف()\n    بينما (صحيح)\n        أسبت_المعالج()\n    نهاية\nنهاية\n"


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


# ─────────────── إعادة تسمية مدمج الإسبات: أسبت_المعالج ⇒ hlt ───────────────

def test_halt_cpu_lowers_to_hlt_and_break_does_not():
    """(AR) أسبت_المعالج ⇒ hlt؛ «توقف» (break) لا ⇒ فرق عدّ hlt موجب."""
    ch, oh, ir_h = _compile(_LOOP_HALT, FREESTANDING)
    cb, ob, ir_b = _compile(_LOOP_BREAK, FREESTANDING)
    assert ch == 0, 'أسبت_المعالج فشلت الترجمة:\n' + oh
    assert cb == 0, '«توقف» كـbreak فشلت الترجمة:\n' + ob
    # (AR) الوضع الحرّ يولّد __sad_panic ضعيفًا بحلقة hlt (خطّ أساس ثابت)؛ فنقيس
    #      الزيادة لا الوجود: أسبت_المعالج تضيف hlt فوق خطّ الأساس، «توقف» (break) لا.
    assert ir_h.count("hlt") > ir_b.count("hlt"), (
        "أسبت_المعالج لم تُضِف hlt فوق نسخة break — لم تُخفَّض إسباتًا:\n" + ir_h[:1500]
    )


# ─────────────── دالة لا_ترجع ⇒ سمة noreturn (فرق عدّ معزول) ───────────────

def test_noreturn_modifier_adds_exactly_one_noreturn():
    """(AR) نفس الجسم بمُعدِّل «لا_ترجع» يضيف noreturn واحدة بالضبط (السمة على قف)."""
    c_plain, o_plain, ir_plain = _compile(_LOOP_HALT, FREESTANDING)
    c_nr, o_nr, ir_nr = _compile(_LOOP_NORETURN, FREESTANDING)
    assert c_plain == 0, o_plain
    assert c_nr == 0, 'دالة لا_ترجع فشلت الترجمة:\n' + o_nr
    added = ir_nr.count("noreturn") - ir_plain.count("noreturn")
    assert added >= 1, (
        "المُعدِّل «لا_ترجع» لم يُضِف سمة noreturn (فرق العدّ = %d):\n%s" % (added, ir_nr[:2000])
    )


def test_noreturn_infinite_loop_compiles_clean_with_hlt():
    """(AR) دالة لا_ترجع بحلقة أبديّة (بلا ارجع) تُترجَم نظيفةً وتحوي hlt."""
    code, out, ir = _compile(_LOOP_NORETURN, FREESTANDING)
    assert code == 0, 'حلقة الإسبات الأبديّة فشلت الترجمة:\n' + out
    assert ir.count("hlt") >= 1, "لا hlt في دالّة الإسبات الأبديّة:\n" + ir[:1500]


def test_plain_function_gets_no_noreturn_over_baseline():
    """(AR) حارس ضدّ التسريب: الجسم العاديّ (بلا مُعدِّل) لا يزيد عدّ noreturn عن
        دالّة تافهة بلا حلقة — أيْ لا يكتسب السمة تلقائيًّا."""
    c_base, o_base, ir_base = _compile("دالة ت()\n    ارجع\nنهاية\n", FREESTANDING)
    c_plain, o_plain, ir_plain = _compile(_LOOP_HALT, FREESTANDING)
    assert c_base == 0 and c_plain == 0, o_base + o_plain
    assert ir_plain.count("noreturn") == ir_base.count("noreturn"), (
        "الجسم العاديّ اكتسب noreturn دون مُعدِّل (تسريب):\n" + ir_plain[:1500]
    )
