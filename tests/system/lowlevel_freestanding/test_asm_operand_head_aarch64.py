"""
============================================================================
(AR) اختبارات منع انحدار: `operand_head` — معاملات نصّيّة ثابتة **تسبق**
     معاملات ص في نصّ المُجمِّع.

     لماذا وُجد الحقل: كتابة سجلّ نظام على AArch64 تكتب المُسجَّل أوّلًا —
     «msr vbar_el1, x0». و`operand_tail` يُلحِق **بعدُ** لا قبلُ، فما كان
     لمعجم aarch64 أن يعبّر عن أيّ كتابة سجلّ نظام: لا جدول متّجهات ولا
     مؤقّت ولا عنوان عودة من استثناء. وهي عصب شفرة النواة.

     🔑 ولماذا يُقاس هنا لا في مصفوفة القواعد: هذه التعليمات **تُعطِب في رتبة
     المستخدم**. اختبار سلوكيّ يشغّلها يموت بإشارة، ولا يقيس شيئًا. فالمقيس
     هو **النصّ المُصدَر** في IR — الطبقة التي يسكنها العطب لو وقع.

     الحرّاس (والاتّجاه السالب هو الحاسم):
       - «msr vbar_el1, $N» موجود: الرأس قبل المعامل.
       - «msr $N, vbar_el1» **غير** موجود: أي أنّ الرأس لم يُعامَل معاملة ذيل.
         بلا هذا الشرط يمرّ التبديلُ أخضرَ — كلا النصّين يحوي الاسمين معًا.
       - «mrs $N, elr_el1» موجود: الذيل ما زال يقع بعدُ (عدم انحدار).
       - منمنمة بلا رأس ولا ذيل (انقل) لا يتسرّب إليها شيء.
(EN) Regression tests for operand_head: fixed operands that PRECEDE the Sad
     operands. System-register writes on AArch64 name the register first
     (msr vbar_el1, x0), which operand_tail cannot express. These faults at
     EL0, so the measurement is the emitted IR text, not a behavioural run.
----------------------------------------------------------------------------
(AR) --أظهر-llvm يُخرج IR بلا اعتماد على رابط — نمط test_task_register_i686.
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
SAD_BUILD = max(
    (p for p in _CANDIDATES if p.exists()),
    # (AR) أحدثُ الموجودَين لا أوّلُهما: قِيسَ ثنائيُّ Release عمرُه ١٣ يومًا
    #      يسبق Debug طازجًا في القائمة، فولّدت الأداةُ عطبًا وهميًّا.
    # (EN) Newest, not first: a 13-day-old Release binary preceded a fresh
    #      Debug one and the instrument fabricated a defect.
    key=lambda p: p.stat().st_mtime,
    default=None,
)

pytestmark = pytest.mark.skipif(
    SAD_BUILD is None,
    reason="sad-build.exe غير مبني — يتطلب: cmake --build build --target sad-build",
)

FREESTANDING = "--حرّ"
EMIT_LLVM = "--أظهر-llvm"
AARCH64 = "--هدف=aarch64-unknown-elf"


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


# (AR) ثلاث كتل لا كتلة واحدة: م١ تدعم متغيّر خرج واحدًا لكلّ كتلة (SEM028)،
#      والثلاثة هنا خرجان ودخل. فالفصل شرط اللغة لا تفضيلًا في الصياغة.
#      ① كتابة برأس (بلا خرج) · ② قراءة بذيل (خرج) · ③ نقل بلا أيّهما (خرج).
_SYSREG = (
    "دالة نصّب_الجدول()\n"
    "    متغير عنوان = 4096\n"
    "    متغير عودة = 0\n"
    "    متغير نسخة = 0\n"
    "    تجميع\n"
    "        اضبط_جدول_المتّجهات {عنوان}\n"
    "    نهاية\n"
    "    تجميع\n"
    "        اقرأ_عنوان_العودة {عودة}\n"
    "    نهاية\n"
    "    تجميع\n"
    "        انقل {نسخة}، {عنوان}\n"
    "    نهاية\n"
    "نهاية\n"
)

_HEAD_BEFORE = re.compile(r"msr vbar_el1, \$\d+")
_HEAD_AS_TAIL = re.compile(r"msr \$\d+, vbar_el1")
_TAIL_AFTER = re.compile(r"mrs \$\d+, elr_el1")


def test_head_precedes_the_sad_operand():
    """(AR) الرأس يقع **قبل** معامل ص: «msr vbar_el1, $N»."""
    code, out, ir = _compile(_SYSREG, AARCH64)
    assert code == 0, "ترجمة سجلّات النظام الحرّة لـaarch64 فشلت:\n" + out
    assert _HEAD_BEFORE.search(ir), (
        "IR بلا `msr vbar_el1, $N` — الرأس لم يُصدَر قبل المعامل:\n" + ir[:2000]
    )


def test_head_is_not_emitted_as_a_tail():
    """(AR) 🔑 الاتّجاه السالب: «msr $N, vbar_el1» يجب ألّا يظهر.

    بلا هذا الشرط يمرّ التبديلُ أخضرَ: كلا الترتيبين يحوي «msr» و«vbar_el1»
    معًا، فمطابقةُ الوجودِ وحدَها لا تفرّق بين رأسٍ وذيل — وهي بعينها العلّة
    التي وُضِع الحقلُ لها.
    """
    code, out, ir = _compile(_SYSREG, AARCH64)
    assert code == 0, out
    match = _HEAD_AS_TAIL.search(ir)
    assert match is None, (
        "المُسجَّل صدر **بعد** المعامل (`" + (match.group(0) if match else "") +
        "`) — عومل الرأسُ معاملةَ الذيل، والمُجمِّع يقرؤها كتابةً إلى وجهةٍ خطأ"
    )


def test_tail_still_follows_the_sad_operand():
    """(AR) عدم انحدار: الذيل ما زال يقع بعدُ — «mrs $N, elr_el1»."""
    code, out, ir = _compile(_SYSREG, AARCH64)
    assert code == 0, out
    assert _TAIL_AFTER.search(ir), (
        "IR بلا `mrs $N, elr_el1` — انحدر مسارُ الذيل مع إضافة الرأس:\n" + ir[:2000]
    )


def test_plain_mnemonic_gets_neither():
    """(AR) منمنمة بلا رأس ولا ذيل لا يتسرّب إليها اسمُ سجلّ نظام."""
    code, out, ir = _compile(_SYSREG, AARCH64)
    assert code == 0, out
    for line in ir.splitlines():
        if "mov $" in line:
            assert "vbar_el1" not in line and "elr_el1" not in line, (
                "تسرّب اسم سجلّ نظام إلى منمنمة بلا رأس/ذيل:\n  " + line
            )
