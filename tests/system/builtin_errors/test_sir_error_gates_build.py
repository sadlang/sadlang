"""
============================================================================
(AR) اختبارات منع انحدار لبوّابة أخطاء بناء SIR في المترجم (RFC: فصل التحذيرات عن
     الأخطاء في بانِي SIR). تُثبِت أنّ برنامجًا خاطئًا بنيويًّا **يُفشِل الترجمة** بدل
     أن «ينجح صامتًا خطأً» (exit 0). كان بانِي SIR يُرجع وحدةً غير فارغة رغم تسجيل
     الأخطاء، والسائق يفحص !sir_module_ فقط، فتُهمَل أخطاء بناء SIR صمتًا.
(EN) Regression tests for the compiler's SIR-build error gate (RFC: separate
     warnings from errors in SIRBuilder). Proves a structurally-invalid program
     *fails compilation* instead of "silently succeeding" (exit 0). The builder
     used to return a non-null module while errors were recorded, and the driver
     checked only !sir_module_, dropping SIR-build errors silently.
----------------------------------------------------------------------------
(AR) نستعمل --emit-sir: يشغّل الواجهة الأماميّة (حيث البوّابة) ويتوقّف قبل التوليد/
     الربط، فلا يعتمد الاختبار على lld-link/LIB (يعزله عن بيئة الربط). الفخّ الذي
     يسدّه: harness الـ@expect_error يتوقّف عند خطأ المفسّر ولا يشغّل المترجم، فيبقى
     تباعد «المفسّر يرفض / المترجم يقبل» خفيًّا؛ هذه الاختبارات تفحص المترجم مباشرة.
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) مواقع محتملة لـ sad-build.exe (يُفضَّل Debug ثمّ Release)
_CANDIDATES = [
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
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

# (AR) راية «واجهة أماميّة فقط»: تبني SIR (حيث البوّابة) وتتوقّف قبل التوليد/الربط.
EMIT_SIR = "--أظهر-sir"

# (AR) شاهدان نصّيّان لتشخيص SEM036: الأوّلُ يميّز «رُفِض للاستنفاد» عن «رُفِض
#      لسببٍ آخر»، والثاني يشترط أن يُسمّي التشخيصُ المُعامِلَ الناقصَ بعينِه.
#      وبلا الثاني يمرُّ تشخيصٌ يقول «غير مستنفِدة» ولا يدلّ على ما ينقص.
NON_EXHAUSTIVE_MARK = "غير مستنفِدة"
MISSING_VARIANT = "أزرق"



def _frontend(source: str) -> tuple[int, str]:
    """(AR) يمرّر المصدر عبر واجهة المترجم الأماميّة (--emit-sir)؛ يعيد (رمز الخروج، الخرج)."""
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), EMIT_SIR],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=60,
            cwd=work,
        )
        return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


# ─────────────────────── البوّابة: أخطاء حقيقيّة تُفشِل ───────────────────────

def test_wrong_builtin_arg_count_fails():
    """(AR) اطبع(طول()) — طول تتطلّب وسيطًا واحدًا. يجب أن تفشل (لا نجاح صامت)."""
    code, out = _frontend("اطبع(طول())\n")
    assert code != 0, (
        "الترجمة نجحت خطأً لبرنامج بعدد وسائط خاطئ — عادت ثغرة «النجاح الصامت»:\n" + out
    )


def test_another_wrong_builtin_arg_count_fails():
    """(AR) اطبع(لرقم()) — لرقم تتطلّب وسيطًا واحدًا. حارس ثانٍ لتوسيع التغطية."""
    code, out = _frontend("اطبع(لرقم())\n")
    assert code != 0, (
        "الترجمة نجحت خطأً لـ لرقم() بلا وسائط — النجاح الصامت لم يُسدّ كاملًا:\n" + out
    )


def test_unknown_member_base_fails():
    """(AR) وصولٌ لعضوٍ على قاعدةٍ مجهولة (مجهول.عضو) — يجب أن يفشل فعلًا (حارس deferBase:
    لا يُخفي خطأً حقيقيًّا بعد إصلاح الخطأ الكاذب لأسماء الأنواع)."""
    code, out = _frontend("اطبع(مجهول.عضو)\n")
    assert code != 0, (
        "قاعدة عضوٍ مجهولة نجحت خطأً — حارس deferBase يُخفي خطأً حقيقيًّا:\n" + out
    )


# ─────────────────── لا فشل مفرط: برامج صالحة تُبنى (حرّاس deferBase/التحذير) ───────────────────

def test_valid_program_builds():
    """(AR) برنامج صالح بسيط يجب أن يجتاز الواجهة الأماميّة (حارس ضدّ الإفراط)."""
    code, out = _frontend('اطبع("مرحبا يا عالم")\n')
    assert code == 0, ("الواجهة الأماميّة فشلت لبرنامج صالح (انحدار إفراط):\n" + out)


def test_enum_member_access_builds():
    """(AR) وصولٌ صالح لعضو تعداد (لون.أحمر) يجب أن يُبنى: إصلاح الخطأ الكاذب لقاعدة الوصول
    (deferBase) يجب ألّا يُفشِل الوصول الصحيح لاسم النوع."""
    src = "تعداد لون\n    أحمر\n    أخضر\nنهاية\nاطبع_سطر(لون.أحمر)\n"
    code, out = _frontend(src)
    assert code == 0, ("وصول عضو تعداد صالح فشل — إصلاح deferBase صار مفرطًا:\n" + out)


def test_non_exhaustive_match_is_rejected():
    """(AR) مطابقةُ تعدادٍ تنقصها حالةٌ (بلا افتراضيّ) **تُرفَض** بـSEM036.

    ⚠️ **هذا الاختبارُ مقلوبٌ عمدًا عن سابقِه.** كان اسمُه
    ``test_non_exhaustive_match_builds`` ويؤكّد أنّ المطابقةَ غيرَ الشاملةِ
    **تُبنى** (تحذيرٌ لا خطأ). وقرارُ مالكٍ لاحقٌ نقضَ ذلك ودُوِّن في مصدرِ
    الحقيقةِ صراحةً — ``language-truth/errors/semantic.yaml`` تحت SEM036:
    «تُرفَض المطابقة هنا زمن التحليل بدل ترك قيمةٍ بلا فرعٍ مطابِق زمن
    التنفيذ» — لبنةً من حاجزِ الأنواعِ الجبريّةِ في خارطةِ الاستضافةِ الذاتيّة.
    فالاختبارُ القديمُ صار يؤكّد **عكسَ المواصفة**.

    🔑 **ولماذا عاش مقلوبًا**: هذا المجلّدُ كلُّه كان **بلا مُنادٍ في CI**،
    فلم يُقَس التناقضُ يومَ وقع. وهذا بعينِه ثمنُ حارسٍ بلا مستهلِك: لا
    يُخفِق فحسب، بل **يوثّق خلافَ المواصفةِ ويُقرَأ**.

    ⚠️ **وسؤالٌ مفتوحٌ للمالك يُسجَّل ولا يُحسَم هنا**: وصفُ SEM036 في مصدرِ
    الحقيقةِ وفي ``error_codes.h`` يقول «تعداد **بحمولة**»، والتعدادُ هنا
    **بلا حمولة** ومع ذلك يُرفَض (مقيس). فإمّا أنّ الاستنفادَ مقصودٌ لكلّ
    التعدادات والوصفُ هو المتأخّر، وإمّا أنّ الفحصَ يتجاوز نطاقَه المُعلَن.
    وهذا الاختبارُ يثبّت **المقيس**؛ فإن حُسِم أنّ المقصودَ الحمولةُ وحدَها،
    فهذا الملفّ هو أوّلُ ما يتغيّر.
    """
    src = (
        "تعداد لون\n    أحمر\n    أخضر\n    أزرق\nنهاية\n"
        "دالة ص(س)\n    طابق (س)\n"
        "        عندما لون.أحمر:\n            ارجع 1\n"
        "        عندما لون.أخضر:\n            ارجع 2\n"
        "    نهاية\nنهاية\nص(لون.أحمر)\n"
    )
    code, out = _frontend(src)
    assert code != 0, (
        "مطابقةٌ غيرُ مستنفِدةٍ بُنيَت — SEM036 لم يُطلَق؛ والمواصفةُ تقول "
        "«تُرفَض زمن التحليل». وقيمةٌ بلا فرعٍ مطابِقٍ زمنَ التنفيذِ هي "
        "بالضبط ما وُضِع لأجله:\n" + out
    )
    # (AR) والرفضُ وحده لا يكفي: رفضٌ لأيِّ سببٍ آخرَ (خطأُ نحوٍ، تعدادٌ مجهولٌ،
    #      مدمجةٌ غائبة) يجعل الاختبارَ أخضرَ **للسببِ الخطأ** — وهو صنفُ
    #      الاختبارِ الذي يُصادق على نفسِه. فيُشترَط أن يُسمّيَ التشخيصُ
    #      المُعامِلَ الناقصَ بعينِه (أزرق) لا أن يرفضَ فحسب.
    assert NON_EXHAUSTIVE_MARK in out, (
        "الرفضُ بلا تشخيصِ SEM036 — رُفِضَ لسببٍ آخرَ، فالاختبارُ أخضرُ "
        "للسببِ الخطأ:\n" + out
    )
    assert MISSING_VARIANT in out, (
        "تشخيصُ SEM036 لا يُسمّي المُعامِلَ الناقصَ «" + MISSING_VARIANT +
        "» — رسالةٌ بلا دلالةٍ تجعل الإصلاحَ تخمينًا:\n" + out
    )
