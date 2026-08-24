"""
============================================================================
(AR) اختبارات مُعدِّل «دالة مقاطعة» (RFC 0059 — الفجوة ح٢ في جرد نواة 64-بت):
     معالجُ المقاطعةِ المكتوبُ بلغةِ ص كان **يدخلُ ولا يستطيعُ الرجوع** — فلا
     مقاطعةَ مؤقّتٍ، فلا جدولة، فلا نظامَ تشغيل. المُعدِّلُ يخفّضُ الدالّةَ
     باتّفاقيّةِ `x86_intrcc` فيتولّدُ كعبُ الدخولِ والخروجِ كاملًا (حفظُ
     السجلّاتِ المتطايرة، وتسويةُ المكدّسِ لرمزِ الخطأ، والعودةُ بـ`iretq`).
     المقيسُ هنا: الاتّفاقيّةُ في الـIR، وسمةُ `byval` على إطارِ المقاطعة،
     و`noredzone`، والعائدُ فراغ؛ وحرّاسُ العقدِ في الطبقةِ الدلاليّةِ المشتركة.
(EN) Tests for the «دالة مقاطعة» modifier (RFC 0059, gap ح٢): x86_intrcc lowering
     with the mandatory byval interrupt frame and noredzone, plus the shared
     semantic contract guards.
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
TARGET_I686 = "--هدف=i686-unknown-none-elf"

# (AR) مرساةُ تشخيصِ SEM046: مسارُ فاحصِ الأنواعِ لا يطبعُ رمزَ الخطأِ نصًّا (قِيس:
#      يطبعُ «[خطأ نوع]» وحدَه)، فمرساةُ «SEM046» تأكيدٌ ميّتٌ لا يحمرُّ إلّا خطأً.
#      المرساةُ الحيّةُ سطرُ التبعةِ الآتي من كتالوجِ الأخطاءِ في مصدرِ الحقيقة.
# (EN) The type-checker path prints no code string; anchor on the SoT consequence line.
SEM046_ANCHOR = "دالة مقاطعة اسم"

HANDLER = (
    "دالة مقاطعة عالج_المؤقت(رقم إطار_المقاطعة)\n"
    "    اكتب_منفذ(32، 32)\n"
    "نهاية\n"
)


def _compile(source: str, *flags: str) -> tuple[int, str, str]:
    """(AR) يترجم المصدر بالرايات المعطاة؛ يعيد (رمز الخروج، الخرج، IR)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), *flags, EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=120, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _free(source: str) -> tuple[int, str, str]:
    return _compile(source, FREESTANDING, NO_MAIN, "-O0", TARGET_X64)


# ─────────────── ① الخفض: الاتّفاقيّةُ وسماتُها الإلزاميّة ───────────────

def test_interrupt_handler_lowers_to_x86_intrcc():
    """(AR) المعالجُ يُخفَّض باتّفاقيّةِ x86_intrcc — منها يأتي `iretq` لا من تعليمةٍ
    في الجسم (بوّابةُ المقاطعةِ خاصيّةُ دالّةٍ لا تعليمة)."""
    code, out, ir = _compile(HANDLER, FREESTANDING, NO_MAIN, "-O0", TARGET_X64)
    assert code == 0, "تصريفُ المعالجِ فشل:\n" + out
    assert "x86_intrcc" in ir, "الاتّفاقيّةُ لم تُطبَّق (انحدار RFC 0059):\n" + ir[:2500]


def test_interrupt_frame_is_byval_pointer():
    """(AR) المعاملُ الأوّلُ مؤشّرٌ بسمةِ `byval` — شرطُ مُصادِقِ LLVM لا خيار."""
    code, out, ir = _free(HANDLER)
    assert code == 0, "تصريفُ المعالجِ فشل:\n" + out
    assert "byval" in ir, "إطارُ المقاطعةِ بلا سمةِ byval (يردُّه المُصادِق):\n" + ir[:2500]


def test_interrupt_handler_is_noredzone_and_void():
    """(AR) `noredzone` لأنّ الاتّفاقيّةَ لا تضبطُها وتداخلُ NMI يُفسدُ المنطقةَ الحمراء؛
    والعائدُ فراغٌ لأنّ `iretq` لا تحملُ قيمة."""
    code, out, ir = _free(HANDLER)
    assert code == 0, "تصريفُ المعالجِ فشل:\n" + out
    assert "noredzone" in ir, "سمةُ noredzone غائبة:\n" + ir[:2500]
    # (AR) التعريفُ يحملُ الاتّفاقيّةَ قبلَ نوعِ العائد: `define x86_intrcc void @…`.
    assert "x86_intrcc void @" in ir, (
        "المعالجُ لا يُعرَّف باتّفاقيّةِ المقاطعةِ وعائدِ فراغ:\n" + ir[:2500]
    )


def test_frame_parameter_is_bridged_to_integer():
    """(AR) التصريحُ السطحيُّ `رقم` يُجسَّرُ بـptrtoint في المقدّمة، فيرى الجسمُ عنوانًا
    خامًّا كما اعتادَ كاتبُ ص في مدمجاتِ العتاد."""
    code, out, ir = _free(HANDLER)
    assert code == 0, "تصريفُ المعالجِ فشل:\n" + out
    assert "intr.frame.addr" in ir, "جسرُ الإطارِ لم يُصدَر:\n" + ir[:2500]


# ─────────────── ② حرّاسُ العقدِ الدلاليّة ───────────────

def test_handler_with_no_parameter_is_rejected():
    """(AR) إطارُ المقاطعةِ معاملٌ أوّلُ إلزاميّ — معالجٌ بلا معاملٍ يُرفَض."""
    code, out, _ = _free("دالة مقاطعة عالج()\n    اكتب_منفذ(32، 32)\nنهاية\n")
    assert code != 0, "معالجٌ بلا إطارِ مقاطعةٍ قُبل:\n" + out


def test_handler_with_return_value_is_rejected():
    """(AR) العائدُ فراغٌ حصرًا — `iretq` لا مكانَ فيها لقيمةِ عائد."""
    code, out, _ = _free(
        "دالة مقاطعة رقم عالج(رقم إطار)\n    ارجع 1\nنهاية\n"
    )
    assert code != 0, "معالجٌ ذو عائدٍ قُبل:\n" + out


def test_calling_a_handler_is_rejected():
    """(AR) المعالجُ لا يُنادى: يُدخَل بإطارٍ يدفعُه العتادُ ويُغادَر بـiretq، فنداؤه
    يعودُ على مكدّسٍ مكسور — انهيارٌ صامتٌ لا يمسكُه اختبار."""
    code, out, _ = _free(HANDLER + "دالة شغل()\n    عالج_المؤقت(0)\nنهاية\n")
    assert code != 0, "نداءُ معالجِ المقاطعةِ قُبل:\n" + out


def test_noreturn_and_interrupt_clash_is_rejected():
    """(AR) تنافرٌ تصريفيّ: المقاطعةُ تعودُ بـiretq و«لا_ترجع» تُعلن أنّها لا تعود."""
    code, out, _ = _free(
        "دالة مقاطعة لا_ترجع عالج(رقم إطار)\n    اكتب_منفذ(32، 32)\nنهاية\n"
    )
    assert code != 0, "اجتماعُ «مقاطعة» و«لا_ترجع» قُبل:\n" + out


# ─────────────── ③ بوّابتا الهدفِ والوضع ───────────────

def test_interrupt_handler_rejected_on_i686():
    """(AR) المرحلةُ الأولى x86_64 حصرًا: عقدُ i386 مختلفٌ (رمزُ خطأٍ i32 وعودةٌ
    بـiret) — والرفضُ أصدقُ من خفضٍ باتّفاقيّةٍ لا يفهمُها الهدف."""
    code, out, _ = _compile(HANDLER, FREESTANDING, NO_MAIN, "-O0", TARGET_I686)
    assert code != 0, "المعالجُ خُفِّض على i686 (بوّابةُ المعماريّةِ لا تشتعل):\n" + out


def test_interrupt_handler_rejected_when_hosted():
    """(AR) لا معنى لبوّابةِ عتادٍ في وحدةٍ مستضافةٍ فوق نظامِ تشغيل."""
    code, out, _ = _compile(HANDLER, NO_MAIN, "-O0", TARGET_X64)
    assert code != 0, "المعالجُ خُفِّض مستضافًا (بوّابةُ الوضعِ لا تشتعل):\n" + out


# ─────────────── ④ لا إفراطَ في السدّ ───────────────

def test_plain_function_unaffected():
    """(AR) دالّةٌ عاديّةٌ لا تُمسّ: لا اتّفاقيّةَ ولا byval ولا noredzone."""
    code, out, ir = _free("دالة رقم اجمع(رقم أ، رقم ب)\n    ارجع أ + ب\nنهاية\n")
    assert code == 0, "الدالّةُ العاديّةُ فشلت (إفراط في السدّ):\n" + out
    assert "x86_intrcc" not in ir, "اتّفاقيّةُ المقاطعةِ سرت على دالّةٍ عاديّة:\n" + ir[:2000]


def test_function_named_interrupt_still_legal():
    """(AR) «دالة مقاطعة(...)» دالّةٌ *اسمُها* مقاطعة — استعمالُ الكلمةِ الناعمةِ اسمًا
    مشروعٌ ويبقى، فلا يبتلعُ المحلّلُ الاسمَ مُعدِّلًا."""
    code, out, ir = _free("دالة رقم مقاطعة(رقم أ)\n    ارجع أ\nنهاية\n")
    assert code == 0, "دالّةٌ اسمُها «مقاطعة» رُفضت (ابتلاعُ الاسمِ مُعدِّلًا):\n" + out
    assert "x86_intrcc" not in ir, "الاسمُ عُومل مُعدِّلًا:\n" + ir[:2000]


# ─────────────── ⑤ مدمج «حمل_سجل_المهمة» (ltr) ───────────────

def test_task_register_load_emits_ltr_with_word_operand():
    """(AR) `حمل_سجل_المهمة(40)` ⇒ `ltr` بمعاملٍ 16-بتّيّ: المُجمِّعُ يردُّ `ltr %eax`
    فيلزمُ المعدِّلُ `${0:w}` ليطبعَ نصفَ السجلِّ (`ltr %ax`)."""
    code, out, ir = _free("دالة هيئ()\n    حمل_سجل_المهمة(40)\nنهاية\n")
    assert code == 0, "حمل_سجل_المهمة فشلت:\n" + out
    assert "ltr ${0:w}" in ir, "ltr لم تُصدَر بمعاملٍ 16-بتّيّ:\n" + ir[:2000]
    assert "(i16 40)" in ir, "المنتقي لم يُقصَّ إلى 16 بتًّا:\n" + ir[:2000]


def test_task_register_load_requires_selector():
    """(AR) المنتقي إلزاميّ — بلا وسيطٍ خطأُ تصريفٍ (رتبةٌ مفروضةٌ بالاتّجاهين)."""
    code, out, _ = _free("دالة هيئ()\n    حمل_سجل_المهمة()\nنهاية\n")
    assert code != 0, "حمل_سجل_المهمة بلا منتقٍ قُبلت:\n" + out


def test_task_register_load_rejects_extra_argument():
    """(AR) والوسيطُ الزائدُ خطأٌ كذلك."""
    code, out, _ = _free("دالة هيئ()\n    حمل_سجل_المهمة(40، 8)\nنهاية\n")
    assert code != 0, "وسيطٌ زائدٌ على حمل_سجل_المهمة قُبل:\n" + out


# ─────────────── ⑥ ثغراتٌ كشفتها المراجعةُ العدائيّة (تحصينٌ بمرساةِ SEM046) ───────────────

def test_call_before_declaration_is_still_rejected():
    """(AR) حارسُ منعِ النداءِ كان يعتمدُ **ترتيبَ التصريح**: نداءٌ يسبقُ تعريفَ المعالجِ
    كان يمرُّ صامتًا (EXIT=0) ويُنزَّل `call` عاديًّا إلى دالّةٍ x86_intrcc بإطارٍ
    `ptr null` — الانهيارُ الثلاثيُّ الذي وُضع الحارسُ لمنعِه. المرورُ التمهيديُّ سدَّه."""
    code, out, _ = _free(
        "دالة شغل()\n    عالج_المؤقت(0)\nنهاية\n" + HANDLER
    )
    assert code != 0, "نداءٌ سابقٌ للتصريحِ مرَّ صامتًا (حارسٌ يعتمد الترتيب):\n" + out
    assert SEM046_ANCHOR in out, "التشخيصُ ليس تشخيصَ عقدِ المعالج:\n" + out


def test_taking_handler_as_value_is_rejected():
    """(AR) ولا يُؤخَذ قيمةً: `متغير م = عالج` ثمّ `م(0)` كان يُنزَّل نداءً مباشرًا
    للبوّابة (إزالةُ الافتراضيّةِ تعيدُ النداءَ اسميًّا)."""
    code, out, _ = _free(HANDLER + "دالة شغل()\n    متغير م = عالج_المؤقت\nنهاية\n")
    assert code != 0, "أخذُ المعالجِ قيمةً قُبل:\n" + out
    assert SEM046_ANCHOR in out, "التشخيصُ ليس تشخيصَ عقدِ المعالج:\n" + out


def test_class_named_interrupt_makes_modifier_ambiguous():
    """(AR) `دالة مقاطعة اسم(...)` تحتملُ قراءتين حين يوجدُ صنفٌ اسمُه «مقاطعة»:
    مُعدِّلَ بوّابةٍ أو نوعَ عائد. كانت تُحسَم صامتةً مُعدِّلًا فتصيرُ دالّةُ الكاتبِ
    بوّابةَ عتادٍ تعودُ بـiretq (EXIT=0، صفرُ تشخيص) — صارت تُشخَّص."""
    code, out, _ = _free(
        "صنف مقاطعة\n    دالة رقم قيمة()\n        ارجع 1\n    نهاية\nنهاية\n"
        "دالة مقاطعة انشئ(رقم أ)\n    ارجع لاشيء\nنهاية\n"
    )
    assert code != 0, "التباسُ الصنفِ والمُعدِّلِ حُسم صامتًا:\n" + out
    assert SEM046_ANCHOR in out, "التشخيصُ ليس تشخيصَ عقدِ المعالج:\n" + out


def test_modifier_on_class_method_is_rejected():
    """(AR) المُعدِّلُ على طريقةِ صنفٍ كان **يُسقَط صامتًا** (طريقةٌ عاديّةٌ بلا اتّفاقيّةٍ
    ولا byval) — قبولٌ-وتجاهلٌ أسوأُ من الرفض؛ فبوّابةُ العتادِ بلا مستقبِل."""
    code, out, ir = _free(
        "صنف س\n"
        "    دالة مقاطعة عالج(رقم إطار)\n"
        "        اكتب_منفذ(32، 32)\n"
        "    نهاية\n"
        "نهاية\n"
    )
    assert code != 0, "المُعدِّلُ على طريقةٍ أُسقط صامتًا:\n" + out
    assert "x86_intrcc" not in ir, "طريقةٌ خُفِّضت بوّابةً:\n" + ir[:1500]


def test_task_register_load_rejected_when_hosted():
    """(AR) `ltr` تعليمةُ حلقةٍ صفريّةٍ بلا ذراعٍ مستضافة (خلافَ lgdt/lidt): إصدارُها في
    وحدةِ حلقةٍ ثالثةٍ يعني ‎#GP‎ عند التنفيذ — فتُرفَض زمنَ الترجمة."""
    code, out, ir = _compile(
        "دالة هيئ()\n    حمل_سجل_المهمة(40)\nنهاية\n", NO_MAIN, "-O0", TARGET_X64
    )
    assert code != 0, "ltr صدرت في وحدةٍ مستضافة:\n" + out
    assert "ltr" not in ir, "تعليمةُ الحلقةِ الصفريّةِ سُرِّبت للمستضاف:\n" + ir[:1500]


def test_task_register_load_clobbers_memory():
    """(AR) `ltr` يقرأ واصفَ TSS ويكتبُه (بتُّ الانشغال 0x9⇒0xB)، فبلا `~{memory}`
    يجوزُ لـLLVM إغراقُ مخازنِ بناءِ الواصفِ تحتَ التعليمة (نظيرُ lgdt/lidt)."""
    code, out, ir = _free("دالة هيئ()\n    حمل_سجل_المهمة(40)\nنهاية\n")
    assert code == 0, "حمل_سجل_المهمة فشلت حرًّا:\n" + out
    assert '"r,~{memory}"' in ir, "ltr بلا clobber للذاكرة:\n" + ir[:1500]


def test_function_named_interrupt_with_paren_still_legal():
    """(AR) الحالةُ التي يحرسُها التمييزُ فعلًا: `دالة مقاطعة(رقم أ)` — دالّةٌ *اسمُها*
    مقاطعة (استعمالُ الكلمةِ الناعمةِ اسمًا). الاختبارُ السابقُ كان يصرّف
    `دالة رقم مقاطعة(...)` فلا يُقيَّم الشرطُ على الكلمةِ أصلًا: مرساةٌ ميتة."""
    code, out, ir = _free("دالة مقاطعة(رقم أ)\n    ارجع أ\nنهاية\n")
    assert code == 0, "دالّةٌ اسمُها «مقاطعة» رُفضت (ابتلاعُ الاسمِ مُعدِّلًا):\n" + out
    assert "x86_intrcc" not in ir, "الاسمُ عُومل مُعدِّلًا:\n" + ir[:1500]


def test_task_register_load_rejected_on_non_x86():
    """(AR) `ltr` منمنمةُ x86: بلا قيدِ عائلةٍ في `arch_specific_opcodes.yaml` لا تُفتَح
    بوّابةُ SEM038 أصلًا (البوّابةُ مبنيّةٌ على البحثِ في الجدول) فتُصدَر التعليمةُ
    لهدفِ aarch64 برمزِ خروجٍ صفر — درسُ SEM044: منمنمةٌ تُخفَّض لمعماريّةٍ أخرى إمّا
    يردُّها المُجمِّعُ وإمّا **يقبلُها بدلالةٍ أخرى**."""
    code, out, ir = _compile(
        "دالة هيئ()\n    حمل_سجل_المهمة(40)\nنهاية\n",
        FREESTANDING, NO_MAIN, "-O0", "--هدف=aarch64-unknown-none-elf",
    )
    assert code != 0, "ltr صدرت لهدفٍ غيرِ x86:\n" + out
    assert "ltr" not in ir, "منمنمةُ x86 سُرِّبت إلى aarch64:\n" + ir[:1500]
