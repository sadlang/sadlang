"""
============================================================================
(AR) **`أي` نوعٌ عند الفاحصِ الصارم — لا تعليقٌ يُقرأ ويُهمَل.**

     الفائدةُ الوحيدةُ التي تُميّز `أي` عن **حذفِ النوع** هي أن يعرفَ الفاحصُ
     أنّ الكاتبَ **قرّر** الديناميّة، فيُجيزَ فيها ما يرفضه في خانةٍ استُنتِج
     نوعُها. وقبلَ هذه البوّابةِ لم تكن تُشترى تلك الفائدةُ البتّة: قِيس
     (٢٠٢٦-٠٨-١٧) أنّ الثلاثةَ تُخفِق **بالرسالةِ نفسِها حرفًا بحرف**:

         أي س = 5    ثمّ  س = "نص"   ⇒  متوقّع 'رقم' لكن وُجد 'نص'
         متغير س = 5 ثمّ  س = "نص"   ⇒  متوقّع 'رقم' لكن وُجد 'نص'
         رقم س = 5   ثمّ  س = "نص"   ⇒  متوقّع 'رقم' لكن وُجد 'نص'

     أي أنّ `أي` كانت عند الفاحصِ **تعليقًا لا نوعًا** — في الوضعِ الوحيدِ
     الذي يُفترَض أن تحمل فيه وزنًا دلاليًّا.

     🔑 والعلّةُ لم تكن في سلطةِ التوافق: `areTypesCompatible` تُجيز `Any` مع
     كلِّ شيءٍ منذ البداية. كانت في `sadKindToTypePtr` — لا ذراعَ لـ`Any`
     فيُرَدُّ `Unknown`، فيُهمِله `visitVarDeclStmt` («المُصرَّحُ مجهولٌ ⇒ خُذ
     المُستنتَج») ويُقيَّد المتغيّرُ بنوعِ مُهيِّئه. ⇒ **البابُ مفتوحٌ والطريقُ
     إليه مقطوع** — ولذلك لا تكشف العطبَ قراءةُ سلطةِ التوافق.

     ⚠️ **والضابطان ليسا زينة**: فاحصٌ معطوبٌ يقبل كلَّ شيءٍ يجتاز فحصَ `أي`
     وحدَه اجتيازًا تامًّا. فيلزم أن يُقاس في النداءِ نفسِه أنّ `متغير` و`رقم`
     ما تزالان **تُرفَضان** — وإلّا صار «قبولُ `أي`» شهادةً على أنّ الفاحصَ
     صامتٌ لا على أنّه يفهم.

     ⚠️ وثالثُ إسنادٍ مقصودٌ: التتبّعُ التدفّقيُّ كان يُعيد ربطَ الخانةِ بنوعِ
     آخرِ قيمةٍ أُسنِدت، فلو بقي على حالِه لَقُبِل الإسنادُ الثاني وأُخفِق
     الثالثُ — أي صار القبولُ رهنَ **عددِ** الإسناداتِ لا معناها، وهو أسوأُ
     من الرفضِ الصريحِ لأنّه يُقرَأ عشوائيًّا. وبذرةٌ بإسنادٍ واحدٍ لا تراه.

     ⚠️ و`--أنواع-صارمة` كان **بلا مُنادٍ واحدٍ في الشجرةِ كلِّها** قبل هذا
     الملفّ (لا اختبارَ سلوكٍ ولا اختبارَ نظامٍ ولا خطوةَ CI). فالوضعُ الصارمُ
     لم يكن أخضرَ — كان **غيرَ مقيسٍ**، وهو غيرُه.

(EN) `أي` must be a TYPE to the strict checker, not a comment. Its only edge
     over omitting the type is that the checker knows the author CHOSE dynamic
     typing. Before this gate that edge did not exist: all three declarations
     failed with a byte-identical message, i.e. the checker saw no difference
     between `أي`, `متغير` and `رقم`. The fault was not in areTypesCompatible
     (which has always accepted Any) but in sadKindToTypePtr, which had no arm
     for Any and returned Unknown — so visitVarDeclStmt replaced it with the
     initializer's type. The door was open; only the road to it was cut.
     The two controls are essential: a checker broken open would pass the `أي`
     assertion alone. The third assignment is deliberate: flow tracking used to
     rebind the slot to the last assigned type, which would accept assignment 2
     and reject assignment 3 — acceptance by assignment COUNT. And
     `--أنواع-صارمة` had zero callers anywhere in the tree before this file:
     strict mode was not green, it was UNMEASURED.
============================================================================
"""
from __future__ import annotations

import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ⚠️ مساراتٌ بلا ‎.exe‎ أيضًا: على لينكس الثنائيُّ ‎build/bin/sad-build‎.
#      بدونها يبتلع ‎skipif‎ الملفَّ كلَّه **أخضرَ صامتًا** — وهو صنفُ الاختبارِ
#      الذي يصادق على نفسِه.
_CANDIDATES = [
    ROOT / "build" / "bin" / "Debug" / "sad-build.exe",
    ROOT / "build" / "bin" / "Release" / "sad-build.exe",
    ROOT / "build" / "bin" / "sad-build.exe",
    ROOT / "build" / "bin" / "Debug" / "sad-build",
    ROOT / "build" / "bin" / "Release" / "sad-build",
    ROOT / "build" / "bin" / "sad-build",
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
    reason="sad-build غير مبنيّ — يتطلّب: cmake --build build --target sad-build",
)

TYPE_CHECK = "--فحص-الأنواع"
STRICT_TYPES = "--أنواع-صارمة"

# (AR) شاهدُ الرفض: نصُّ عدمِ التطابقِ كما يبنيه `recordTypeError`. يُطابَق على
#      جزءٍ ثابتٍ منه لا على السطرِ كلِّه — السطرُ يحمل مسارًا وسطرًا وعمودًا.
MISMATCH_MARK = "عدم تطابق الأنواع"


def _check(source: str) -> tuple[int, str]:
    """(AR) يُمرّر المصدرَ على الفاحصِ الصارمِ ويُعيد (رمزَ الخروج، المخرَجَ كلَّه)."""
    with tempfile.TemporaryDirectory() as tmp:
        src = Path(tmp) / "probe.ص"
        src.write_text(source, encoding="utf-8")
        out = Path(tmp) / "probe.out"
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), TYPE_CHECK, STRICT_TYPES, "-o", str(out)],
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=120,
        )
        return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


# (AR) الجسدُ واحدٌ في الثلاثةِ عمدًا: لا يفترق المِجَسّاتُ إلّا في **كلمةِ
#      التصريحِ الأولى**، فيكون الفرقُ في النتيجةِ منسوبًا إليها وحدَها.
_BODY = 'س = "نص"\nس = 7\nاطبع_سطر(س)\n'

ACCEPTED = "أي س = 5\n" + _BODY
REJECTED_VAR = "متغير س = 5\n" + _BODY
REJECTED_INT = "رقم س = 5\n" + _BODY


def test_any_slot_accepts_cross_type_reassignment() -> None:
    """(AR) `أي` تُجيز إعادةَ الإسنادِ عبرَ الأنواعِ — وهي فائدتُها الوحيدةُ هنا."""
    code, output = _check(ACCEPTED)
    assert MISMATCH_MARK not in output, (
        "خانةُ «أي» رُفِضت في الوضعِ الصارم — أي أنّ الفاحصَ ما يزال يعاملها "
        "معاملةَ خانةٍ مُستنتَجة، فلا تشتري تصريحًا بالديناميّة:\n" + output
    )
    assert code == 0, "بناءُ خانةِ «أي» أخفق في الوضعِ الصارم:\n" + output


@pytest.mark.parametrize(
    "label,source",
    [("متغير", REJECTED_VAR), ("رقم", REJECTED_INT)],
)
def test_inferred_and_declared_slots_stay_rejected(label: str, source: str) -> None:
    """(AR) الضابطُ: بدونه يجتاز فاحصٌ صامتٌ اختبارَ «أي» اجتيازًا تامًّا."""
    _, output = _check(source)
    assert MISMATCH_MARK in output, (
        f"خانةُ «{label}» قُبِلت في الوضعِ الصارم — فالفاحصُ صامتٌ لا فاهم، "
        "واجتيازُ «أي» عندئذٍ لا يشهد بشيء:\n" + output
    )
