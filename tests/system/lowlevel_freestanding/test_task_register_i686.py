"""
============================================================================
(AR) اختبارات منع انحدار: منمنمتا لهجة التجميع ltr/str (حمّل/خزّن_سجل_المهمة)
     بمُعدِّل الحجم operand_width:16. معاملهما r/m16؛ المُجمِّع يرفض الصيغة
     32-بت (ltr %eax) فيلزم إصدار ${N:w} ⇒ السجلّ الفرعيّ 16-بت (ltrw %ax).
     تحميل سجلّ المهمة TR أساس انتقال الحلقة 3 (نطاق المستخدم، اللبنة 4.1).

     الحرّاس:
       - القالب المضمَّن يحوي ${N:w} لكلٍّ من ltr/str (لا $N المجرّد الذي
         يُنتج %eax فيرفضه المُجمِّع).
       - الترجمة الحرّة لـi686 تنجح (IR سليم).
       - عدم انحدار: منمنمة بمعامل سجلّ بلا operand_width (انقل) تبقى $N.
(EN) Regression tests for the ltr/str assembly-dialect mnemonics with the
     operand_width:16 size modifier: emit ${N:w} (16-bit sub-register) since
     the r/m16 operand rejects the 32-bit form. Basis of ring-3 transition.
----------------------------------------------------------------------------
(AR) لماذا --أظهر-llvm؟ القالب والقيود يعيشان في IR؛ الراية تُخرجه بلا اعتماد
     على lld-link (عزل عن بيئة الربط) — نمط test_cpu_ctl_i686.
============================================================================
"""
from __future__ import annotations

import re
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
I686 = "--هدف=i686-unknown-elf"


def _compile(source: str, *extra_flags: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر حرًّا مع إخراج IR؛ يعيد (رمز الخروج، الخرج، IR)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), FREESTANDING, *extra_flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


# (AR) كتلة «تجميع» تحمّل TR من منتقٍ ص وتقرؤه ثانيةً. دالّة عاديّة قصدًا:
#      تركيب «لا_ترجع + متغير» يصادف عيب محلّل سابقًا (خارج نطاق هذه اللبنة)،
#      والغرض هنا التحقّق من خفض المنمنمتين لا سمة NoReturn. المنتقي 0x28.
#      المنمنمتان تندمجان في كتلة InlineAsm واحدة؛ ltr مدخل (r) وstr خرج (=r)،
#      فرقم خانة كلٍّ يعتمد ترتيب القيود ⇒ نطابق ${N:w} بأيّ N (لا 0 صراحةً).
_LTR_STR = (
    "دالة حمّل_المهمة()\n"
    "    متغير منتقي = 40\n"
    "    متغير مقروء = 0\n"
    "    تجميع\n"
    "        حمّل_سجل_المهمة {منتقي}\n"
    "        خزّن_سجل_المهمة {مقروء}\n"
    "    نهاية\n"
    "نهاية\n"
)

_LTR_W = re.compile(r"ltr \$\{\d+:w\}")
_STR_W = re.compile(r"str \$\{\d+:w\}")


def test_ltr_emits_word_modifier_i686():
    """(AR) حمّل_سجل_المهمة ⇒ القالب `ltr ${N:w}` (لا $N المجرّد ⇒ %eax مرفوض)."""
    code, out, ir = _compile(_LTR_STR, I686)
    assert code == 0, "ترجمة ltr/str الحرّة لـi686 فشلت:\n" + out
    assert _LTR_W.search(ir), (
        "IR بلا `ltr ${N:w}` — لم يُصدَر مُعدِّل الحجم 16-بت ⇒ %eax يرفضه المُجمِّع:\n"
        + ir[:2000]
    )


def test_str_emits_word_modifier_i686():
    """(AR) خزّن_سجل_المهمة ⇒ القالب `str ${N:w}` بخرج =r (وجهة صِرفة)."""
    code, out, ir = _compile(_LTR_STR, I686)
    assert code == 0, out
    assert _STR_W.search(ir), "IR بلا `str ${N:w}`:\n" + ir[:2000]


def test_ltr_str_no_plain_dollar_operand():
    """(AR) لا يظهر `ltr $N`/`str $N` مجرّدًا (بلا :w) — سيُنتج %eax فيرفضه المُجمِّع."""
    code, _, ir = _compile(_LTR_STR, I686)
    assert code == 0
    # (AR) كلّ ظهور لـ ltr/str متبوعًا بـ$ يجب أن يكون ${N:w} لا $N.
    for line in ir.splitlines():
        for mn in ("ltr $", "str $"):
            if mn in line:
                assert ":w}" in line, (
                    f"منمنمة {mn.split()[0]} بمعامل $N مجرّد (بلا :w) ⇒ %eax:\n  " + line
                )


def test_plain_register_operand_unaffected():
    """(AR) عدم انحدار: منمنمة بمعامل سجلّ بلا operand_width (انقل) تبقى $N مجرّدًا."""
    src = (
        "متغير أ = 5\n"
        "متغير ب = 0\n"
        "تجميع\n"
        "    انقل {ب}، {أ}\n"
        "نهاية\n"
        "اطبع(ب)\n"
    )
    code, out, ir = _compile(src, I686)
    assert code == 0, out
    # (AR) mov القالب يستعمل $N بلا :w (العرض الافتراضيّ 32-بت).
    assert ":w}" not in ir, "منمنمة بلا operand_width أصدرت :w — تسريب مُعدِّل الحجم"
