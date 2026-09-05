#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «المُعلَنُ يعرفُ مَن يشهدُ له، والشاهدُ يعرفُ لِمَن يشهد».

     مصدرُ الحقيقةِ يُعلِنُ بنودًا في خمسِ عائلات — والعددُ يُقاسُ ويُطبَعُ في
     كلِّ تشغيلةٍ ولا يُنثَرُ ههنا: نسختانِ باليدِ لحقيقةٍ واحدةٍ تتباعدان.
     (وقد تباعدتا فعلًا: كُتِبَ ٢٥١١ عددَ **البنودِ**، والحارسُ يقيسُ عددَ
      **العناوين** — والفرقُ هو البنودُ الذائبةُ في بندِ ③.)
     والبذرةُ تشهدُ لبندٍ منها بوسمِ **مرساة**. ولا تكونُ المرساةُ مرساةً حتّى
     تُقاسَ في **الاتّجاهَين**:

       ① **مرساةٌ يتيمة**: وسمٌ يُسمّي بندًا لا يعرفُه مصدرُ الحقيقة ⇒ **صفر**.
          🔑 وكان يُقاسُ في مجلَّدٍ واحدٍ (`tests/behavior/rules_matrix/`) والوسمُ
             مستعمَلٌ في الشجرةِ كلِّها — فثلاثُ بذورٍ خارجَه كانت تُسمّي
             `gr.decl.constant` و`gr.expr.binary` و`gr.stmt.assign`، وهي أسماءٌ
             **مخترَعةٌ تُشبِهُ المُعلَنَ ولا توجدُ فيه**. مرساةٌ ترسو على لا شيء
             تُوهِمُ بشهادةٍ لا وجودَ لها.

       ② **مُعلَنٌ بلا مرساة**: بندٌ لا تشهدُ له بذرةٌ واحدة ⇒ سقفٌ **نازلٌ**
          لكلِّ عائلة. وليس عيبًا في نفسِه — بل **دَينٌ مقيسٌ** يُنفَقُ منه ولا
          يُرفَع. وكان يُطبَعُ في تقريرٍ ولا يُحمِّرُ شيئًا: «بلا اختبارات: 2»
          سطرًا في `CONFORMANCE_REPORT.md` بلا سقفٍ ولا بوّابة.

     🔑 **والعنوانُ مؤهَّلٌ لا عارٍ.** المعرِّفُ وحدَه لا يكفي: قِيسَ أنّ ٢٦ معرِّفَ
        مدمجٍ يتكرّرُ عبرَ النطاقاتِ (‏`LAST_ERROR` ثلاثًا) و٣٥ في الأوپكودات
        و٤ في المكتبةِ القياسيّة. ومطابقةُ المعرِّفِ عاريًا هي عينُ ما وسمَ سبعةً
        وثلاثينَ مدمجًا منفَّذًا دَينًا كاذبًا في دفعةٍ سابقة.

(EN) Anchor integrity in both directions: every anchor resolves to a declared
     id (zero tolerance, measured tree-wide — not in one directory as before),
     and every declared item's missing witness is counted under a descending
     per-family ceiling. Addresses are namespace-qualified, never bare ids.
============================================================================
"""
from __future__ import annotations

import collections
import re
import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
LT = ROOT / "language-truth"
TESTS = ROOT / "tests"

# ═══ فضاءُ العناوين ═════════════════════════════════════════════════════════
# (AR) (العائلة، المجلَّد، مفتاحُ القائمة، مفتاحُ المعرِّف، مفتاحُ النطاق أو None،
#       البادئة). والبادئةُ جزءٌ من العنوانِ لا زينة: بها يُفصَلُ `PRINT` المكتبةِ
#       القياسيّةِ عن `PRINT` مدمجٍ لو تشابها.
# (AR) والنحوُ بلا بادئةٍ مضافةٍ لأنّ معرِّفاتِه تبدأُ بـ`gr.` أصلًا — فالوسمُ
#       القديمُ `@rule: gr.stmt.if` يبقى عنوانًا صالحًا بلا تحويل.
FAMILIES = (
    ("النحو",              "grammar",  "productions", "id",     None,        ""),
    ("رموزُ الأخطاء",      "errors",   "errors",      "id",     None,        "err."),
    ("المدمجات",           "builtins", "functions",   "cpp_id", "namespace", "bi."),
    ("أوپكوداتُ الخلفيّة", "backend",  "opcodes",     "name",   None,        "op."),
    ("المكتبةُ القياسيّة", "stdlib",   "functions",   "cpp_id", "namespace", "std."),
)

# ═══ صيغُ الرسوّ ════════════════════════════════════════════════════════════
# (AR) `@rule:` صيغةٌ قديمةٌ للنحوِ وحدَه، تبقى صالحةً ولا تُكسَر (٣١١٢ بذرة).
#      `@مُعلَن:` الصيغةُ المعمَّمةُ لكلِّ عائلة.
#      `@expect_error:` يُسمّي رمزَ خطأٍ — وكان يُقرأُ **علامةَ «بذرةٌ سالبة» فقط**
#      ولا يُقابَلُ بالكتالوجِ قطُّ، فبذرةٌ تكتبُ `SEM999` تمرُّ صامتة.
_ANCHOR_FORMS = (
    (re.compile(r"^#\s*@rule:?\s+(.+)$", re.M), ""),
    (re.compile(r"^#\s*@مُعلَن:?\s+(.+)$", re.M), ""),
    # (AR) 🔑 **ثلاثةُ هجاءاتٍ لا اثنان.** `compile_` يقعُ **بين** `expect_`
    #      و`error`، فلا لاحقةَ `[a-z_]*` تبلغُه مهما اتّسعت — ولذا بقيَ
    #      `@expect_compile_error` غيرَ مقروءٍ بعدَ إصلاحِ النقطتَين.
    #      وليس هجاءً شاذًّا: قِيسَ ٧٥ موضعًا في الشجرة، وحارسانِ مُودَعانِ
    #      يعدّانِه ندًّا للأوّل (`check_asm_dialect_arch.py:70` ·
    #      `check_test_artifacts.py:60`). فبذرةٌ تكتبُ رمزًا مخترَعًا تحتَه
    #      كانت **تُدمَجُ خضراء** — وهي عينُ العلّةِ المُعلَنِ سدُّها.
    # (AR) 🔑 والنقطتانِ **اختيارٌ** ههنا كما في أختَيها. كانت مشترَطةً،
    #      والصيغةُ الخاليةُ منها هي **الأغلبيّةُ المُودَعةُ في الشجرة**: قِيسَ
    #      ١٣١ موضعًا بلا نقطتَين (`@expect_error RUN059`) مقابلَ ١٠١ بها —
    #      فكان الحارسُ يقرأُ أقلَّ من نصفِ ما يزعمُ حراستَه، والدعوى المكتوبةُ
    #      أعلاه («بذرةٌ تكتبُ SEM999 تمرُّ صامتة») تبقى صحيحةً بالهجاءِ السائد.
    #      و`\s+` يمنعُ التصاقَ الرمزِ بالوسم؛ وقِيسَ أنّه لا وجودَ لصيغةٍ
    #      ملتصقةٍ في الشجرة، فلا انحدار.
    (re.compile(r"^#\s*@expect(?:_compile)?_error[a-z_]*:?\s+(.+)$", re.M), "err."),
)
_SPLIT = re.compile(r"[,،]")

# (AR) وسمُ الخطأِ يلتقطُ بقيّةَ السطرِ ثمّ يُنتزَعُ الرمزُ من **صدرِ** كلِّ جزء:
#      فيُقرأُ `SEM001, SEM999` قائمةً تناظرًا مع `@rule:`، ويبقى
#      `SEM999 وصفٌ نثريّ` مقروءًا كما كان. وما لا يبدأُ برمزٍ يُهمَل.
_CODE_HEAD = re.compile(r"\s*([A-Za-z]+[0-9]+)")

# (AR) `_archive` شجرةٌ محفوظةٌ **لا تُشغَّل**، بلهجةٍ قديمةٍ ماتت. وهو مستثنًى
#      في ثلاثةِ حرّاسَ قبلَ هذا بالسببِ نفسِه (`check_sad_snippets…:367` ·
#      `measure_seed_contract_gap:40` · `check_version_flags:176`) — فاستثناؤه
#      ههنا اتّباعٌ لعُرفٍ مُدوَّنٍ لا تسهيلٌ على الحارس. وفيه إحدى وعشرونَ بذرةً
#      تحملُ وسمَ رسوٍّ بلهجةٍ ميّتة، ولو مُسِحت لأنتجت مراسيَ يتيمةً لا معنى لها.
#      🔑 **والعددُ المتروكُ يُطبَع**: كنسٌ يسكتُ عمّا تركَ يُقرأُ «غطّى الكلَّ».
SKIP_PARTS = ("_archive",)

# (AR) 🔑 **طبقةُ قيدٍ لا مصدرُ إعلان.** `arch_specific_opcodes.yaml` يُعلِنُ في
#      رأسِه لامتغيِّرَه: «كلُّ اسمٍ هنا موجودٌ في `sir_opcodes.yaml`» — فبنودُه
#      الخمسةُ والثلاثون **توأمُ أسماءٍ مُعلَنةٍ سلفًا** لا بنودٌ جديدة، وهو يَعرِضُ
#      قيدَ معماريّةٍ على المُعلَنِ ولا يُعلِنُ شيئًا.
#      وعدُّه مصدرَ إعلانٍ أنتجَ عطبَين متقابلَين:
#        • **رفضٌ كاذبٌ لا صورةَ امتثالٍ له**: لامتغيِّرُه الثاني **يوجِبُ نموَّه**
#          («كلُّ مُصدِرٍ يحوي أسمبليَّ x86 أوپكودُه مُعلَنٌ هنا») — وكلُّ مدخلٍ
#          صحيحٍ فيه مكرَّرٌ بتعريفِه، فنموُّه المشروعُ كان يضربُ سقفَ الذوبان.
#          والرفضُ الكاذبُ أسوأُ من الانهيارِ الذي حلَّ محلَّه.
#        • **وثغرةٌ مقابلة**: حذفُ أوپكودٍ من `sir` له توأمٌ ههنا لا يُنقِصُ
#          مجموعةَ العناوين، فلا تعضُّ الأرضيّة — انكماشٌ حقيقيٌّ يُقرأُ سكونًا.
#      وباستثنائِه صارَ ذوبانُ الأوپكوداتِ صفرًا، والأرضيّةُ تقيسُ `sir` وحدَه.
SKIP_SOURCES = ("arch_specific_opcodes.yaml",)

# ═══ السقوفُ المُودَعة ═══════════════════════════════════════════════════════
# (AR) ① لا سقفَ للمرساةِ اليتيمة. الثابتُ توثيقٌ لا تسامح.
CEILING_ORPHAN_ANCHORS = 0

# (AR) ② دَينُ الشهادة، مُقاسٌ في ٢٠٢٦-٠٩-٠٥ لكلِّ عائلةٍ على حدة.
#      **ينزلُ بالعملِ ولا يُرفَع.** وفصلُه بالعائلاتِ مقصود: سقفٌ واحدٌ جامعٌ
#      يسمحُ بأن يُسدَّ دَينُ عائلةٍ وينمو دَينُ أخرى فيبقى المجموعُ ثابتًا،
#      فيخضرُّ الحارسُ على انحدارٍ حقيقيّ.
# (AR) 🔑 **أرضيّةُ المُعلَنِ لكلِّ عائلة — تصعدُ ولا تهبط.**
#      السقفُ يحرسُ نموَّ الدَّينِ وحدَه، **وانكماشُ المُعلَنِ نفسِه يُقرأُ تقدُّمًا**:
#      مفتاحٌ يُعادُ تسميتُه في yaml، أو مجلَّدٌ يُنقَل، أو بندٌ يفقدُ معرِّفَه —
#      فتسقطُ العائلةُ صامتةً و`0 ≤ السقف` فيخضرُّ الحارس.
#      بُرهنَ بالحقن: `functions:` ← `functions_RENAMED:` في المكتبةِ القياسيّةِ
#      ⇒ `0 / 0 · بلا شهادة: 0 (السقف 193)` **ورمزُ صفر**، وقناريّةُ المجموعِ
#      لا تلتقطُها (يبقى ٢٢٨٣ > ٢٠٠٠). وأرضيّةٌ جامعةٌ واحدةٌ لخمسِ عائلاتٍ هي
#      عينُ ما جُنِّبَ في السقوفِ عمدًا ثمّ عادَ ههنا.
FLOOR_DECLARED = {
    "النحو": 107,
    "رموزُ الأخطاء": 191,
    "المدمجات": 1205,
    "أوپكوداتُ الخلفيّة": 780,
    "المكتبةُ القياسيّة": 193,
}

# (AR) 🔑 **البندُ الذائبُ: مُعلَنٌ بلا عنوانٍ متميّزٍ فلا يُعَدُّ ولا يُطالَبُ
#      بشاهد.** العنوانُ مجموعة، فبندٌ جديدٌ باسمٍ موجودٍ لا يزيدُ العددَ ولا
#      الدَّين. بُرهنَ بالحقن: أوپكودٌ اسمُه `BUILTIN_CLI` ⇒ `780/780` ورمزُ صفر،
#      بينما اسمٌ فريدٌ ⇒ `781 > 780` أحمر.
#      ⚠️ **والمقياسُ عددُ البنودِ الذائبةِ لا عددُ الأسماءِ المكرَّرة.** أوّلُ
#         صياغةٍ عدَّت «أسماءً لها تكرار»، وإضافةُ تكرارٍ **ثالثٍ** لاسمٍ مكرَّرٍ
#         سلفًا لا تغيّرُ ذلك العدد — فمرَّ المجسُّ ⑭ أخضرَ. والفرقُ
#         `عددُ البنودِ − عددُ العناوين` يلتقطُه.
#      🔑 وبعدَ التأهيلِ بالنطاقِ **لا تذوبُ المدمجاتُ ولا المكتبةُ القياسيّة**:
#         ٢٦ و٤ تصادمًا في المعرِّفِ العاري تصيرُ صفرًا في العنوانِ المؤهَّل — وهو
#         عينُ ما التأهيلُ له. والأوپكوداتُ وحدَها بلا نطاقٍ في مصدرِ الحقيقة،
#         فذوبانُها ٣٥ حقيقيٌّ ودَينٌ قائم.
CEILING_DISSOLVED = {
    "النحو": 0,
    "رموزُ الأخطاء": 0,
    "المدمجات": 0,
    "أوپكوداتُ الخلفيّة": 0,
    "المكتبةُ القياسيّة": 0,
}

CEILING_UNWITNESSED = {
    "النحو": 2,
    # (AR) ١٦٦ ← ١٥٣ بقراءةِ الهجاءِ الخالي من النقطتَين، ثمّ ١٥٣ ← ١٥١
    #      بقراءةِ `@expect_compile_error`. والسقفُ ينزلُ بالقياسِ كما ينزلُ
    #      بالعمل — وكلُّ نزلةٍ ههنا كانت شهادةً قائمةً لا تُرى.
    "رموزُ الأخطاء": 151,
    "المدمجات": 1205,
    "أوپكوداتُ الخلفيّة": 780,
    "المكتبةُ القياسيّة": 193,
}


def _declared() -> tuple[dict[str, set[str]], dict[str, int]]:
    """(AR) العناوينُ المُعلَنةُ لكلِّ عائلة، وعددُ البنودِ الذائبةِ فيها."""
    by_family: dict[str, set[str]] = {}
    dissolved: dict[str, int] = {}
    for fam, sub, key, idk, nsk, prefix in FAMILIES:
        base = LT / sub
        addrs: set[str] = set()
        seen = 0
        for path in sorted(base.rglob("*.yaml")):
            if "_schemas" in path.parts or path.name in SKIP_SOURCES:
                continue
            doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
            if not isinstance(doc, dict):
                continue
            for entry in (doc.get(key) or []):
                if not isinstance(entry, dict) or entry.get(idk) is None:
                    continue
                ident = str(entry[idk])
                seen += 1
                if nsk and entry.get(nsk):
                    addrs.add(f"{prefix}{entry[nsk]}.{ident}")
                else:
                    addrs.add(f"{prefix}{ident}")
        by_family[fam] = addrs
        # (AR) بندٌ لا يُضيفُ عنوانًا جديدًا **ذائبٌ**: عنوانُه عينُ عنوانِ غيرِه.
        dissolved[fam] = seen - len(addrs)
    return by_family, dissolved


def _anchors() -> tuple[collections.Counter, int, int]:
    """(AR) العناوينُ المرسوُّ إليها في **الشجرةِ كلِّها** لا في مجلَّدٍ واحد."""
    used = collections.Counter()
    seeds = 0
    skipped = 0
    for path in sorted(TESTS.rglob("*.ص")):
        if any(part in SKIP_PARTS for part in path.parts):
            skipped += 1
            continue
        seeds += 1
        # (AR) ولا `errors="replace"`: ملفٌّ ليس UTF-8 **لم يُقَسْ**، فيصعدُ
        #      الاستثناءُ إلى غلافِ الرمز ٢. والإبدالُ الصامتُ يجعلُ بذرةً
        #      بترميزٍ آخرَ تُقرأُ «بلا مرساة» وهي مرسًى إليها.
        #      (قِيسَ: بذرةٌ واحدةٌ UTF-16 LE في الشجرة، وهي في `_archive`.)
        # (AR) 🔑 `utf-8-sig` لا `utf-8`: بادئةُ BOM تسبقُ `#` فلا يطابقُها `^#`،
        #      فتُسقَطُ **مرساةُ السطرِ الأوّلِ صامتةً**. وقِيسَ في الشجرة: ٨٦ بذرةً
        #      ببادئةِ BOM، أربعٌ منها مرساتُها في السطرِ الأوّلِ فكانت غيرَ
        #      مرئيّةٍ للحارس. وبذرةٌ ببادئةٍ ترسو على اسمٍ مخترَعٍ كانت **تُدمَجُ
        #      خضراءَ** بينما البذرةُ نفسُها بلا بادئةٍ تُحمِّر. والبادئةُ في
        #      الكائنِ المُودَعِ لا في الشجرةِ العاملةِ وحدَها.
        #      و`utf-8-sig` لا يُضعِفُ الرمزَ ٢: ملفٌّ لا يُفَكُّ ما زال يُلقي.
        text = path.read_text(encoding="utf-8-sig")
        for rx, prefix in _ANCHOR_FORMS:
            for match in rx.finditer(text):
                for part in _SPLIT.split(match.group(1)):
                    if prefix == "err.":
                        head = _CODE_HEAD.match(part)
                        if head:
                            used[f"{prefix}{head.group(1)}"] += 1
                        continue
                    part = part.strip()
                    if part:
                        used[f"{prefix}{part}"] += 1
    return used, seeds, skipped


def _where(address: str, limit: int = 2) -> list[str]:
    """(AR) البذورُ التي ترسو على عنوانٍ — للتشخيصِ لا للقياس."""
    found: list[str] = []
    needle = address.split(".", 1)[-1] if address.startswith("err.") else address
    for path in sorted(TESTS.rglob("*.ص")):
        if any(part in SKIP_PARTS for part in path.parts):
            continue
        try:
            if needle in path.read_text(encoding="utf-8-sig"):
                found.append(path.relative_to(ROOT).as_posix())
                if len(found) >= limit:
                    break
        except (OSError, UnicodeDecodeError):
            continue
    return found


# ═══ عيارُ الأداةِ نفسِها ═══════════════════════════════════════════════════
# (AR) قناريّتانِ: واحدةٌ **موجبةٌ** (عنوانٌ مُعلَنٌ ومرسوٌّ إليه قطعًا) وواحدةٌ
#      **سالبةٌ** (عنوانٌ مخترَعٌ يجبُ ألّا يُرى). ودعوى الوجودِ وحدَها لا تكفي:
#      حارسٌ يزعمُ أنّ كلَّ عنوانٍ مُعلَنٌ يجتازُها كلَّها.
POSITIVE_CANARY = "gr.stmt.if"
NEGATIVE_CANARY = "gr.stmt.zz_never_declared"


def _calibrate(addresses: set[str], used: collections.Counter,
               seeds: int) -> list[str]:
    """(AR) خمسةُ مجسّاتٍ تُنفَّذُ في كلِّ تشغيل. إخفاقُ أحدِها = عطبُ آلة."""
    fails: list[str] = []
    if POSITIVE_CANARY not in addresses:
        fails.append(f"المُعلَنُ لا يحوي {POSITIVE_CANARY} وهو قاعدةٌ مُعلَنةٌ قطعًا")
    if NEGATIVE_CANARY in addresses:
        fails.append(f"المُعلَنُ يحوي {NEGATIVE_CANARY} وهو اسمٌ مخترَع"
                     " — قارئٌ يوسِّعُ لا يقرأ")
    if used.get(POSITIVE_CANARY, 0) < 1:
        fails.append(f"لا بذرةَ ترسو على {POSITIVE_CANARY} — ماسحُ المراسي أعمى")
    if seeds < 1000:
        fails.append(f"المسحُ عادَ بـ{seeds} بذرةً — والشجرةُ فيها آلاف")
    if len(addresses) < 2000:
        fails.append(f"المُعلَنُ عادَ بـ{len(addresses)} عنوانًا — والمُعلَنُ آلاف")
    return fails


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    by_family, dissolved = _declared()
    all_addresses = set().union(*by_family.values())
    used, seeds, skipped = _anchors()

    probes = 5
    fails = _calibrate(all_addresses, used, seeds)
    print("حارس «المُعلَنُ يعرفُ مَن يشهدُ له، والشاهدُ يعرفُ لِمَن يشهد»:")
    if fails:
        print(f"  ✗ عيارُ الأداة: {probes - len(fails)}/{probes}"
              " — لا يُبنى على قراءتِها:")
        for msg in fails:
            print(f"      · {msg}")
        return 2
    print(f"  عيارُ الأداة: {probes}/{probes} ✓"
          f" (بذورٌ ممسوحة: {seeds} · عناوينُ مُعلَنة: {len(all_addresses)}"
          f" · متروكٌ في {'/'.join(SKIP_PARTS)}: {skipped})")

    orphans = sorted(addr for addr in used if addr not in all_addresses)

    print(f"  ① مرساةٌ يتيمة (ترسو على غيرِ مُعلَن): {len(orphans)}"
          f" (المسموح {CEILING_ORPHAN_ANCHORS} — لا سقفَ له)")
    print("  ② مُعلَنٌ بلا مرساة — دَينُ شهادةٍ نازلٌ لا يُرفَع:")
    failed = False
    for fam, _s, _k, _i, _n, _p in FAMILIES:
        addrs = by_family[fam]
        missing = sorted(addr for addr in addrs if addr not in used)
        ceiling = CEILING_UNWITNESSED[fam]
        flag = "" if len(missing) <= ceiling else "  ✗"
        print(f"      {fam:<22} {len(addrs) - len(missing):>5} / {len(addrs):<5}"
              f" · بلا شهادة: {len(missing):>4} (السقف {ceiling}){flag}")
        if len(missing) > ceiling:
            for addr in missing[:8]:
                print(f"          · {addr}")
            if len(missing) > 8:
                print(f"          … و{len(missing) - 8} غيرُها")
            failed = True
    print("  ③ أرضيّةُ المُعلَنِ والبنودُ الذائبة:")
    for fam, _s, _k, _i, _ns, _p in FAMILIES:
        count = len(by_family[fam])
        floor = FLOOR_DECLARED[fam]
        melt = dissolved[fam]
        melt_ceiling = CEILING_DISSOLVED[fam]
        marks = ""
        if count < floor:
            marks += "  ✗ انكمشَ المُعلَن"
            failed = True
        if melt > melt_ceiling:
            marks += "  ✗ نما الذوبان"
            failed = True
        print(f"      {fam:<22} عناوين: {count:>5} (الأرضيّة {floor})"
              f" · ذائبة: {melt:>3} (السقف {melt_ceiling}){marks}")
    if failed:
        print("      🔑 انكماشُ المُعلَنِ ليس تقدُّمًا: بندٌ يسقطُ من الحسابِ لا"
              " يُقاسُ دَينُه. وبندٌ **ذائبٌ** عنوانُه عينُ عنوانِ غيرِه، فلا"
              " يُعَدُّ ولا يُطالَبُ بشاهدٍ ولو كان مُعلَنًا.")

    if len(orphans) > CEILING_ORPHAN_ANCHORS:
        print(f"  ✗ مرساةٌ يتيمة: {len(orphans)} — وسمٌ يشهدُ لبندٍ لا يعرفُه"
              " مصدرُ الحقيقة. يُصحَّحُ الاسمُ أو يُعلَنُ البند:")
        for addr in orphans[:12]:
            seen = _where(addr)
            where = seen[0] if seen else "؟"
            print(f"      · {addr}   ({where})")
        if len(orphans) > 12:
            print(f"      … و{len(orphans) - 12} غيرُها")
        failed = True

    if failed:
        return 1
    print("  ✓ لا مرساةَ يتيمة، ولم ينمُ دَينُ الشهادةِ في أيِّ عائلة.")
    return 0


if __name__ == "__main__":
    # (AR) 🔑 انهيارٌ ليس حكمًا — كأخواتِه. ملفٌّ لا يُقرأُ، أو yaml لا يُحلَّل،
    #      **لم يقِسْ شيئًا**، فرمزُه ٢ لا ١.
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"  ✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
