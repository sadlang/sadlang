"""
============================================================================
(AR) بوّابةُ **حجمِ خانةِ النوعِ العدميّ** — تُبنى قبلَ الهجرة لا بعدَها.

     الدَّينُ الذي تحرسه: «لاشيء» اليومَ يُمثَّل بحارسٍ **داخل نطاقِ القيمة**
     (`kSadNullSentinel`)، فالخانةُ ثمانيةُ بايتاتٍ لكلِّ نوعٍ عدميّ. وأيُّ
     انتقالٍ إلى وسمٍ **خارجَ** النطاق (وهو ما تقتضيه م‑ب/م‑ج/م‑د) يضاعف
     الخانةَ — ومضاعفةُ خانةٍ لا تُخفق بناءً ولا اختبارَ سلوكٍ ولا تُغيّر
     مخرَجًا واحدًا. تُرى في استهلاكِ الذاكرةِ في مستودعٍ آخرَ بعد أسابيع.

     🔑 **ولماذا قبلَ الهجرةِ لا بعدَها**: بوّابةٌ تُبنى بعد التضاعفِ لا تفرّق
     «تضاعفَ بالتصميمِ» من «تضاعفَ صدفةً» — إنّما تُصادِق الحالَ الراهنَ خطَّ
     أساس. فتُكتَب اليومَ على **٨**، فتمرَّ الآنَ وتُخفِقَ يومَ الهجرة، ويكون
     تحديثُها حينَها **قرارًا صريحًا** بالحجمِ الجديدِ لا مصادقةً بأثرٍ رجعيّ.

     ✅ **وقد وقع ذلك فعلًا مرّتين**: أخفقت البوّابةُ يومَ م‑ب على
     `منطقي ⇒ %SadDyn` في المسارَين، ثمّ يومَ م‑ج على `عشري` كذلك — أي أدّت
     وظيفتَها بالضبط في المرّتين. فحُدِّثت
     **بجدولِ توقُّعٍ لكلِّ نوعٍ** لا بتوسيعِ قائمةِ الأنواعِ المقبولة: القائمةُ
     المُوسَّعةُ تبتلع الشكلَ الجديدَ صامتةً وتفقد القدرةَ على كشفِ **العودة**؛
     والجدولُ يُخفِق في الاتّجاهين. الحجمُ المُسجَّلُ للمُهاجَر: **١٦ بايتًا**
     (‏`{ i8, i64 }` بمحاذاةِ ٨) — ضِعفُ ما كان، وهو ثمنُ العقد (أ) مُعلَنًا.

     ⚠️ والفخُّ المُدوَّن في هذه الشجرة: *تأكيدٌ بايتيٌّ بصفرِ مطابقةٍ يقيس
     المُحسِّنَ لا المُصدِر*. فيُؤكَّد هنا **عددُ** الخانات المُلتقَطةِ أيضًا،
     لا وجودُ كلِّ واحدةٍ منها: صفرُ مطابقةٍ إخفاقٌ لا نجاح.

     ⚠️ وقياسُ الحجمِ من مخرَجِ **التخفيضِ** (‎--أظهر-llvm‎) لا من ‎sizeof‎ في
     اختبارِ وحدة: ‎sizeof‎ يقيس بنيةَ C++ لا الخانةَ المُخفَّضة — وهي التي
     تتضاعف؛ ويلزمه بناءٌ كامل.

(EN) Slot-size gate for nullable types, built BEFORE the migration.
     Null is currently an in-band sentinel, so every nullable slot is 8 bytes.
     Moving to an out-of-band tag doubles the slot without failing any build,
     any behavioral test, or changing a single output — it only shows up as
     memory growth elsewhere, weeks later. Pinned to 8 today so it passes now
     and fails on the day of the migration, making the update a deliberate
     decision rather than a retroactive rubber stamp. The match COUNT is
     asserted too: a zero-match byte assertion measures the optimizer, not
     the emitter.
============================================================================
"""
from __future__ import annotations

import re
import subprocess
import tempfile
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[3]

# (AR) ⚠️ مساراتٌ بلا ‎.exe‎ أيضًا: على لينكس — وهي المنصّةُ التي تبني المترجمَ
#      فعلًا في CI — الثنائيُّ ‎build/bin/sad-build‎. بدونها يبتلع ‎skipif‎ الملفَّ
#      كلَّه **أخضرَ صامتًا**، وهو بعينه صنفُ الاختبارِ الذي يصادق على نفسِه.
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
    reason="sad-build غير مبنيّ — يتطلّب: cmake --build build --target sad-build",
)

EMIT_LLVM = "--أظهر-llvm"

# (AR) الأنواعُ العدميّةُ الستّةُ ومُعرِّفُ كلٍّ منها في العيّنة. الاسمُ حرفٌ
#      واحدٌ عمدًا: مفتاحُ الالتقاطِ في المخرَجِ هو بايتاتُ الاسمِ بترميز UTF-8،
#      فالقِصَرُ يُبقي النمطَ مقروءًا.
NULLABLE_DECLARATIONS = [
    ("ع", "عشري"),
    ("ن", "منطقي"),
    ("ب", "طبيعي8"),  # (AR) كان «بايت» — حُذِفَ اللفظُ ٢٧ آب ٢٠٢٦، وخَلَفُه «طبيعي8»
    ("ط", "طبيعي"),
    ("ر", "رقم"),
    ("س", "نص"),
]

# (AR) أنواعُ LLVM التي تشغل ثمانيةَ بايتاتٍ على هدفٍ بعرضِ ٦٤ بتًّا. القائمةُ
#      **صريحةٌ لا محسوبة**: المقصودُ التقاطُ أيِّ شكلٍ جديدٍ للخانة (بنيةٌ
#      موسومةٌ مثل ‎{ i8, double }‎) بوصفه إخفاقًا يُقرَأ، لا حسابُ حجمِه.
IN_BAND_SLOT_TYPES = {"i64", "double", "ptr"}
IN_BAND_SLOT_BYTES = 8

# (AR) شكلُ الخانةِ بعد الهجرةِ إلى وسمٍ **خارجَ** النطاق: ‎%SadDyn = { i8, i64 }‎.
#      محاذاةُ ‎i64‎ تدفع الحمولةَ إلى الإزاحةِ ٨، فالبنيةُ ١٦ بايتًا — أي
#      **ضِعفُ** الخانةِ داخلِ النطاق. وهذا هو الثمنُ المُعلَنُ للعقد (أ).
OUT_OF_BAND_SLOT_TYPE = "%SadDyn"
OUT_OF_BAND_SLOT_BYTES = 16

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 الأنواعُ التي **هاجرت فعلًا** — تُوسَّعُ مرحلةً بمرحلةٍ لا دفعةً واحدة.
#
#      «منطقي» هاجر في **م‑ب**، ومسوّغُه مقيسٌ لا مُدَّعًى: معامِلُ `منطقي؟` كان
#      يُخفَّض ‎i1‎، فوسيطُ `لاشيء` ووسيطُ `صحيح` **بتّاتٌ متطابقةٌ حرفيًّا** —
#      لا مستهلِكَ يفرّق بينهما، فأيُّ حارسٍ عند الطابعِ أو عند `طابق` رقعةٌ في
#      الطبقةِ الخطأ. والوسمُ خارجَ النطاقِ يُنهي التصادمَ من أصلِه.
#
#      ⚠️ وهذا الجدولُ **ثنائيُّ الاتّجاه** عمدًا: النوعُ المُدرَجُ يجب أن يكون
#      ‎%SadDyn‎، وغيرُ المُدرَجِ يجب أن **يبقى** ثمانيةَ بايتات. فيُخفِق:
#        ① نوعٌ هاجر بلا قرار (انجرافٌ يُسحَبُ مع تغييرٍ آخَر)، و
#        ② نوعٌ مُهاجَرٌ **عاد** إلى الحارسِ داخلَ النطاق (انحدارٌ صامت).
#      والاتّجاهُ الثاني هو ما تُفوّته «قائمةُ أنواعٍ مقبولةٍ» تُوسَّع كلَّما أخفقت.
# (EN) Kinds that HAVE migrated — extended stage by stage, never wholesale.
#      Boolean migrated in م‑ب: a `bool?` parameter lowered to `i1`, so the `null`
#      and `true` arguments were literally identical bits and no consumer could
#      tell them apart. The table is deliberately BIDIRECTIONAL: a listed kind must
#      be %SadDyn, an unlisted one must STILL be 8 bytes. So it fails both on a kind
#      that migrated without a decision AND on a migrated kind that silently
#      reverted — the second direction is exactly what a permissive "accepted types"
#      set, widened whenever it fails, can never catch.
# ══════════════════════════════════════════════════════════════════════════
MIGRATED_TO_OUT_OF_BAND = {"منطقي", "عشري", "طبيعي"}  # م‑ب ✅ م‑ج ✅ م‑د ✅


def _utf8_escape(name: str) -> str:
    """(AR) اسمٌ عربيٌّ ⇒ صيغةُ هروبِ LLVM لبايتاتِه: «ع» ⇒ ‎\\D8\\B9‎.

    يُشتقّ من الاسمِ نفسِه لا يُكتَب رقمًا: جدولُ هروبٍ منسوخٌ ينجرف عن الأسماء
    بلا أن يُخفِق شيء — يصير النمطُ بلا مطابقةٍ فيمرَّ التأكيدُ خاويًا.
    """
    return "".join("\\%02X" % byte for byte in name.encode("utf-8"))


def _compile_to_ir(source: str) -> tuple[int, str, str]:
    with tempfile.TemporaryDirectory() as work:
        src = Path(work) / "t.ص"
        out_ll = Path(work) / "t.ll"
        src.write_text(source, encoding="utf-8")
        proc = subprocess.run(
            [str(SAD_BUILD), str(src), EMIT_LLVM, "-o", str(out_ll)],
            capture_output=True, text=True, encoding="utf-8", errors="replace",
            timeout=180, cwd=work,
        )
        ir = out_ll.read_text(encoding="utf-8", errors="replace") if out_ll.exists() else ""
        return proc.returncode, (proc.stdout or "") + (proc.stderr or ""), ir


def _local_source() -> str:
    body = "".join(
        "    %s عدمي %s = لاشيء\n" % (type_name, var_name)
        for var_name, type_name in NULLABLE_DECLARATIONS
    )
    uses = "".join(
        "    اطبع_سطر(نص(%s))\n" % var_name
        for var_name, _ in NULLABLE_DECLARATIONS
    )
    return "دالة افحص()\n" + body + uses + "نهاية\n\nافحص()\n"


def _global_source() -> str:
    body = "".join(
        "%s عدمي %s = لاشيء\n" % (type_name, var_name)
        for var_name, type_name in NULLABLE_DECLARATIONS
    )
    uses = "".join(
        "اطبع_سطر(نص(%s))\n" % var_name
        for var_name, _ in NULLABLE_DECLARATIONS
    )
    return body + uses


def _slot_type_of_local(ir: str, var_name: str) -> "str | None":
    pattern = r'%"%' + re.escape(_utf8_escape(var_name)) + r'"\s*=\s*alloca\s+([^,]+),'
    match = re.search(pattern, ir)
    return match.group(1).strip() if match else None


def _slot_type_of_global(ir: str, var_name: str) -> "str | None":
    pattern = r'@"' + re.escape(_utf8_escape(var_name)) + r'"\s*=\s*(?:\S+\s+)*?global\s+(\S+)\s'
    match = re.search(pattern, ir)
    return match.group(1).strip() if match else None


def _assert_slots(ir: str, resolver, scope_label: str) -> None:
    measured = []
    missing = []
    for var_name, type_name in NULLABLE_DECLARATIONS:
        slot_type = resolver(ir, var_name)
        if slot_type is None:
            missing.append(type_name)
        else:
            measured.append((type_name, slot_type))

    # (AR) ① عددُ المطابقات — قبل فحصِ الحجم. صفرُ مطابقةٍ يجعل ② تأكيدًا
    #      خاويًا يمرّ على أيِّ تمثيلٍ مهما كان.
    assert not missing, (
        "لم تُلتقط خانةُ الأنواعِ العدميّةِ الآتيةِ في %s: %s.\n"
        "التقطَ %d من %d. والتأكيدُ بصفرِ مطابقةٍ يمرُّ على أيِّ تمثيل، فالغيابُ "
        "هنا إخفاقٌ لا تخطٍّ: إمّا تغيّر شكلُ التصريحِ في المخفِّض، وإمّا صيغةُ "
        "أسماءِ LLVM. راجِع المخرَجَ:\n%s"
        % (scope_label, "، ".join(missing), len(measured),
           len(NULLABLE_DECLARATIONS), ir[:2000])
    )

    # (AR) ② الشكل — لكلِّ نوعٍ توقُّعُه، والاتّجاهانِ كلاهما إخفاق.
    migrated_early = [
        (type_name, slot_type)
        for type_name, slot_type in measured
        if type_name not in MIGRATED_TO_OUT_OF_BAND and slot_type not in IN_BAND_SLOT_TYPES
    ]
    assert not migrated_early, (
        "خانةُ نوعٍ عدميٍّ في %s لم تعد %d بايتاتٍ وهو **غيرُ مُدرَجٍ** في "
        "المُهاجَر: %s.\n"
        "🔑 مضاعفةُ الخانةِ أثرٌ متوقَّعٌ لهجرةِ «لاشيء» إلى وسمٍ خارجَ النطاق، "
        "لكنّها لا تُخفق بناءً ولا مخرَجًا — فوجب أن تُخفِق هنا كي تُقرَأ.\n"
        "إن كانت الهجرةُ مقصودةً فأضِف النوعَ إلى MIGRATED_TO_OUT_OF_BAND "
        "**عمدًا** مع مسوّغِه؛ ولا تُوسَّع IN_BAND_SLOT_TYPES لتبتلعَ الشكلَ "
        "الجديدَ صامتةً."
        % (scope_label, IN_BAND_SLOT_BYTES,
           "، ".join("%s ⇒ %s" % pair for pair in migrated_early))
    )

    reverted = [
        (type_name, slot_type)
        for type_name, slot_type in measured
        if type_name in MIGRATED_TO_OUT_OF_BAND and slot_type != OUT_OF_BAND_SLOT_TYPE
    ]
    assert not reverted, (
        "نوعٌ **مُهاجَرٌ** في %s عاد إلى خانةٍ داخلَ النطاق: %s (المتوقَّع %s).\n"
        "🔑 وهذا الاتّجاهُ هو الأخطر: العودةُ إلى الحارسِ داخلَ النطاقِ تُعيدُ "
        "التصادمَ الذي هاجرَ النوعُ لأجلِه — ولا تُخفِق بناءً ولا اختبارَ سلوكٍ "
        "إلّا في الحالاتِ التي يصادفُ فيها الحارسُ قيمةً مشروعة."
        % (scope_label, "، ".join("%s ⇒ %s" % pair for pair in reverted),
           OUT_OF_BAND_SLOT_TYPE)
    )


def test_local_nullable_slots_match_migration_table():
    """(AR) الخاناتُ المحلّيّةُ الستُّ — كلٌّ بشكلِه المُقرَّرِ في جدولِ الهجرة."""
    code, out, ir = _compile_to_ir(_local_source())
    assert code == 0, "فشلت الترجمة إلى LLVM IR:\n" + out
    _assert_slots(ir, _slot_type_of_local, "دالّةٍ")


def test_global_nullable_slots_match_migration_table():
    """(AR) وخاناتُ المستوى الأعلى — مسارُ تخفيضٍ آخَرُ يُقاس على حِدَة.

    ليست تكرارًا: قِيس أنّ «عشري عدمي» يُخفَّض إلى ‎alloca double‎ محلّيًّا
    وإلى ‎global i64‎ عامًّا — مساران مختلفان لتصريحٍ واحد، فبوّابةٌ على أحدهما
    تترك الآخَرَ بلا حارس.
    """
    code, out, ir = _compile_to_ir(_global_source())
    assert code == 0, "فشلت الترجمة إلى LLVM IR:\n" + out
    _assert_slots(ir, _slot_type_of_global, "المستوى الأعلى")
