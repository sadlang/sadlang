# -*- coding: utf-8 -*-
"""(AR) حارسٌ: تقريرُ المطابقةِ المُودَعُ يشهد لملفّاتٍ موجودةٍ ولا يُغفِل موجودًا.

العلّةُ المقيسة (٢٠٢٦-٠٨-١١): `CONFORMANCE_REPORT.md` و`CONFORMANCE_REPORT_detail.md`
ملفّان **مُودَعان** يُعلنان في ترويستِهما أنّهما «مُولَّدان آليًّا لا يُحرَّران يدويًّا»،
ويُستشهَد بهما حكمًا على تغطيةِ القواعد. ولا شيءَ كان يقابل محتواهما بالقرص:
- شهد التقريرُ لقاعدةِ `gr.oop.new` بثلاثةَ عشرَ اختبارًا ناجحًا وسرَدها بأسمائها،
  بينما **أحدَ عشرَ** منها لم تدخل المستودعَ قطّ (ابتلعها `.gitignore`). فالاستنساخُ
  النظيفُ لا يُعيد إنتاجَ الشهادةِ أصلًا.
- وثلاثةُ صفوفٍ تُسمّي ملفّاتٍ أُعيدت تسميتُها فبقيت الأسماءُ القديمةُ شاهدةً على
  اختباراتٍ لا وجودَ لها.

والسببُ البنيويّ: `check_grammar_conformance.py` لا يُستدعى في `x.py gen --check`
إطلاقًا — مسارُه الوحيدُ `x.py conformance` وهو ليس في التكامل المستمرّ. فالانجرافُ
لا حدَّ له في الزمن. وهذا الحارسُ يضع الحدَّ **بلا تشغيلِ الاختبارات**: مقارنةُ
أسماءِ الصفوفِ بالقرصِ مسحٌ لحظيّ، أمّا تحديثُ الأزمنةِ والحالاتِ فيلزمه توليدٌ
كامل بـ`--run` (ساعات) — ولذلك يُقاس هنا ما يُقاس رخيصًا، ويُعلَن الباقي دَينًا.

الحكم:
- **يتيمة**: صفٌّ يُسمّي ملفًّا غيرَ موجودٍ على القرص — شهادةٌ كاذبةٌ صريحة.
- **غائبة**: ملفٌّ على القرصِ بلا صفٍّ — تغطيةٌ غيرُ مشهودٍ لها.
- **فاصلُ ويندوز**: `\\` في عمودِ المسار — يجعل الملفَّ المُودَعَ ينجرف بين المنصّتَين
  عند كلِّ توليد (عولج عند المنبع في `scan_tests`؛ هذا حارسُ الانحدار).

الدَّينُ المُعلَنُ أدناه سقفٌ ينحدر: يُسدَّد بأوّلِ توليدٍ كامل، ولا يتّسع.
"""
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) تُستورَد دوالُّ المولِّدِ نفسِه لا تُعادُ كتابتُها: عمودا «القاعدة» و«الفئة»
#      يُشتقّان بمنطقٍ له حالاتٌ حدّيّة (وسومُ @rule متعدّدةٌ بفواصل · عمقُ مسارِ
#      _interactions)، ومقارِنٌ ساذجٌ يُولّد مئاتِ الانحرافاتِ الوهميّة.
# (EN) Import the generator's own helpers rather than reimplementing them.
from check_grammar_conformance import (  # noqa: E402
    _category_of, _extract_rule_ids, _folder_rule_of,
)

for _تيّار in (sys.stdout, sys.stderr):
    if hasattr(_تيّار, "reconfigure"):
        _تيّار.reconfigure(encoding="utf-8")

الجذر = Path(__file__).resolve().parents[2]
مجلد_العيّنات = الجذر / "tests" / "behavior" / "rules_matrix"
مسار_التقرير = الجذر / "language-truth" / "grammar" / "CONFORMANCE_REPORT_detail.md"
مسار_الملخّص = الجذر / "language-truth" / "grammar" / "CONFORMANCE_REPORT.md"

بادئة_العيّنات = "tests/behavior/rules_matrix/"
بادئة_الثغرات = "tests/behavior/grammar_gaps/"

نمط_الصفّ = re.compile(r"^\| \d+ \| `([^`]+)` \| `([^`]+)` \| ([^|]+?) \|")
# (AR) الإحالةُ تُلتقَط من **العمودِ الأوّلِ وحدَه** لا من الملفِّ كلِّه: جدولُ
#      التباعداتِ يكتب مخرَجَ المفسّرِ والمترجمِ الخامَّ بين علامتَي اقتباسٍ خلفيّتَين
#      (write_markdown: `{io}` و`{co}`)، ونصُّ تشخيصٍ ينتهي باسمِ ملفٍّ `.ص` نمطٌ
#      واقعيٌّ جدًّا. فمسحُ الملفِّ كلِّه يحوّل نثرَ التشخيصِ إلى شرطٍ مُلزِمٍ ويُحمِّر
#      الحارسَ زورًا عند أوّلِ تباعد — وهو أخضرُ اليومَ لأنّ الجدولَ فارغٌ لا لأنّ
#      المنطقَ محصَّن.
# (EN) Refs are read from the first column only: the diffs table quotes raw engine
#      output in backticks, and diagnostics ending in a .ص name are commonplace.
نمط_العمود_الأوّل = re.compile(r"^\|\s*`([^`]+)`\s*\|")
نمط_الفاصل = re.compile(r"^\|[\s\-:|]+\|\s*$")
عناوين_الجداول = ("التباعدات", "المتخطّى", "ثغرات")

# (AR) دَينٌ مُعلَنٌ بأسمائه لا بعدده: العددُ وحدَه يسمح لانجرافٍ جديدٍ أن يحلّ محلّ
#      قديمٍ صامتًا. هذه صفوفٌ تُسمّي ملفّاتٍ أُعيدت تسميتُها (الاسمُ الجديدُ حاضرٌ
#      في «غائبة» أدناه)، ويزول الدَّينُ بأوّلِ توليدٍ كامل.
يتيمة_معلَنة = {
    "30_oop/gr.oop.impl/negative/060_impl_without_target.ص",
    "30_oop/gr.oop.member/negative/060_member_outside_class.ص",
    "30_oop/gr.oop.trait/negative/060_trait_method_unimplemented.ص",
}

# (AR) سقفٌ للإعلانِ نفسِه: الرَّتشُ أحاديُّ الاتّجاهِ يمنع تجمُّدَ الدَّينِ عند سدادِه
#      ولا يمنع اتّساعَه — وبما أنّ إضافةَ أيِّ عيّنةٍ تُفشِل الحارسَ والبديلَ توليدٌ
#      بساعات، فالمسارُ المريحُ لكلِّ مساهمٍ «أضِف اسمَك إلى المُعلَن»، فيصير الإعلانُ
#      بعد عشرِ رقعاتٍ هو التقريرَ الحقيقيّ. السقفُ يجعل توسيعَه قرارًا صريحًا.
# (EN) A ceiling on the declaration itself, so declaring debt stays an escape hatch.
سقف_اليتيمة = 3
سقف_الغائبة = 16

# (AR) شقُّ الثغراتِ بلا دَينٍ اليومَ (١١٠ ⟷ ١١٠) — و«صفرُ دَينٍ» يعني أيضًا «صفرَ
#      مخرج»: إضافةُ ملفٍّ مساعِدٍ إلى grammar_gaps (نظيرُ وحدة_مصدر.ص في العيّنات،
#      وهي وحدةُ مصدرٍ لا اختبار) تُحمِّر الحارسَ بلا علاجٍ غيرِ توليدٍ بساعات.
#      فالمخرجُ موجودٌ وفارغ، وملؤه قرارٌ صريحٌ يُرى في الفرق.
# (EN) The gaps side carries no debt today; the hatch exists but is empty on purpose.
ثغرات_معفاة = set()

# (AR) ملفّاتٌ حيّةٌ أُضيفت بعد آخِرِ توليدٍ كامل فلا صفَّ لها بعدُ.
غائبة_معلَنة = {
    "20_declarations/gr.decl.variable/basic/060_typed_form_int.ص",
    "20_declarations/gr.decl.variable/basic/061_typed_form_string.ص",
    "20_declarations/gr.decl.variable/edge/060_typed_form_no_init.ص",
    "20_declarations/gr.decl.variable/negative/060_typed_form_rejects_modifier.ص",
    "20_declarations/gr.decl.variable/negative/061_removed_word_double_typed_form.ص",
    "20_declarations/gr.decl.variable/negative/062_removed_word_double_keyword_form.ص",
    "30_oop/gr.oop.impl/negative/060_impl_without_end.ص",
    "30_oop/gr.oop.member/negative/060_method_without_param_list.ص",
    "30_oop/gr.oop.new/negative/057_new_without_operand.ص",
    "30_oop/gr.oop.trait/negative/060_trait_without_end.ص",
    "40_expressions/gr.expr.postfix/edge/075_nested_map_write.ص",
    "40_expressions/gr.expr.postfix/edge/076_map_numeric_key_write.ص",
    "40_expressions/gr.expr.postfix/edge/077_nested_map_numeric_key.ص",
    "40_expressions/gr.expr.postfix/edge/078_tagged_array_write_dyn_index.ص",
    "60_advanced/gr.adv.template_decl/basic/002_template_return_any.ص",
    "60_advanced/gr.adv.template_decl/negative/051_removed_word_double_return_type.ص",
}


def اقرأ_صفوف_التقرير() -> dict:
    """(AR) يُرجع {المسار: (القاعدة، الفئة)} — لا الأسماءَ وحدَها."""
    نصّ = مسار_التقرير.read_text(encoding="utf-8")
    صفوف = {}
    for سطر in نصّ.splitlines():
        م = نمط_الصفّ.match(سطر)
        if م:
            صفوف[م.group(1)] = (م.group(2), م.group(3).strip())
    return صفوف


def امسح_الشجرة_كاملة() -> set:
    """(AR) كلُّ ملفّاتِ `.ص` تحت tests/behavior كما يراها git — لا كما هي على القرص.

    وهذا هو بيتُ القصيد: العطبُ الأصليُّ أنّ أحدَ عشرَ ملفًّا كانت **على قرصِ
    المطوّرِ** وليست في المستودع (ابتلعها .gitignore). فمسحُ القرصِ بـrglob كان
    سيُظهِر الحارسَ أخضرَ على آلتِه ويكشف العطبَ في التكامل المستمرِّ وحدَه.
    و`--cached --others --exclude-standard` = المتعقَّبُ + غيرُ المتعقَّبِ غيرُ
    المُستبعَد — أي ما سيصل إلى استنساخٍ نظيفٍ بعد الإيداع، والمُستبعَدُ يسقط.
    (EN) Scan what git sees, not the disk: the original defect was 11 files present
    on the developer's disk yet excluded from the repo by .gitignore.
    """
    مخرَج = subprocess.run(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z",
         "--", "tests/behavior"],
        cwd=الجذر, capture_output=True, text=True, encoding="utf-8")
    if مخرَج.returncode != 0:
        raise RuntimeError(مخرَج.stderr.strip() or "git ls-files أخفق")
    return {س for س in مخرَج.stdout.split("\0") if س.endswith(".ص")}


def اقرأ_إحالات_الملخّص() -> tuple:
    """(AR) يقرأ العمودَ الأوّلَ من جداولِ الاختباراتِ وحدَها في التقريرِ الملخّص.

    يُرجع (الإحالات، الصفوفُ المشوّهة، أفيه قسمُ ثغرات). والصفُّ «مشوَّهٌ» إن كان
    داخلَ جدولِ اختباراتٍ ولم يبدأ باسمِ ملفِّ `.ص` مقتبَسًا — وهذا يُبلَّغ صراحةً
    لأنّ حارسًا يفحص ما استخرجه فقط أحاديُّ الجانب: لو فقد الجدولُ اقتباسَه لسقط
    الفحصُ كلُّه بلا عَرَض.
    (EN) Read column one of the test tables only, and flag malformed rows — a guard
    that only checks what it managed to extract fails silently when parsing breaks.
    """
    إحالات, مشوّهة = set(), []
    داخل_قسمٍ_مَعنيّ = False
    داخل_جدول = False
    أقسامٌ_مرئيّة = set()

    for سطر in مسار_الملخّص.read_text(encoding="utf-8").splitlines():
        if سطر.startswith("## "):
            مطابِقة = {ع for ع in عناوين_الجداول if ع in سطر}
            أقسامٌ_مرئيّة |= مطابِقة
            داخل_قسمٍ_مَعنيّ = bool(مطابِقة)
            داخل_جدول = False
        elif not سطر.startswith("|"):
            داخل_جدول = False
        elif نمط_الفاصل.match(سطر):
            # (AR) سطرُ الفصلِ يفصل الترويسةَ عمّا بعدها، فما بعده بياناتٌ لا ترويسة
            داخل_جدول = داخل_قسمٍ_مَعنيّ
        elif داخل_جدول:
            م = نمط_العمود_الأوّل.match(سطر)
            if م and م.group(1).endswith(".ص"):
                إحالات.add(م.group(1))
            else:
                مشوّهة.append(سطر[:80])

    return إحالات, مشوّهة, أقسامٌ_مرئيّة


def افحص_الملخّص(كلّ_الشجرة: set) -> list:
    """(AR) يفحص `CONFORMANCE_REPORT.md`: كلُّ إحالةٍ فيه تُسمّي ملفًّا تراه git،
    وكلُّ اختبارٍ كاشفٍ للثغراتِ له صفٌّ فيه.

    الملخّصُ هو الملفُّ الذي يُقرأ فعلًا (التفصيليُّ مرجعٌ عند الحاجة)، وكان خارجَ
    الفحصِ كلِّه. وإحالاتُه من صنفَين: مسارٌ كاملٌ في جدولَي التباعداتِ والمتخطّى،
    واسمٌ عارٍ في جدولِ كاشفةِ الثغرات — والاسمُ العاري لا يُحَلّ إلّا إن كانت
    أسماءُ `grammar_gaps` متفرّدة، فتفرّدُها شرطٌ يُفحَص لا يُفترَض.
    (EN) The summary is the file actually read; it cites tests both by full path and
    by bare basename, and a bare name only resolves if gaps basenames are unique.
    """
    إخفاقات = []
    if not مسار_الملخّص.exists():
        return ["تعذّر بلوغُ التقريرِ الملخّص"]

    ثغرات = {س for س in كلّ_الشجرة if س.startswith(بادئة_الثغرات)}
    أسماء_الثغرات = {}
    مكرّرة = set()
    for س in ثغرات:
        اسم = س.rsplit("/", 1)[-1]
        if اسم in أسماء_الثغرات:
            مكرّرة.add(اسم)
        أسماء_الثغرات[اسم] = س

    if مكرّرة:
        إخفاقات.append(
            f"{len(مكرّرة)} اسمَ ملفٍّ مكرَّرًا في grammar_gaps — والملخّصُ يُحيل "
            f"بالاسمِ العاري فتصير الإحالةُ غيرَ قابلةٍ للحلّ: {sorted(مكرّرة)[:5]}")

    نصّ_الملخّص = مسار_الملخّص.read_text(encoding="utf-8")
    إحالات, صفوف_مشوّهة, أقسامٌ_مرئيّة = اقرأ_إحالات_الملخّص()
    if صفوف_مشوّهة:
        إخفاقات.append(
            f"{len(صفوف_مشوّهة)} صفًّا في جداولِ الاختباراتِ عمودُه الأوّلُ ليس اسمَ "
            f"ملفِّ `.ص` مقتبَسًا — تغيّرت صيغةُ الجدول، ولو مرّ ذلك صامتًا لسقط "
            f"الفحصُ كلُّه بلا عَرَض: {صفوف_مشوّهة[:3]}")

    # (AR) غيابُ قسمٍ كاملٍ يُقرأ أخضرَ لو لم يُفحَص — فالقارئُ لا يجد ما يفحصه
    #      فيمرّ. فتُشترَط الأقسامُ بشرطِ إصدارِها في المولِّد: «التباعدات» غيرُ
    #      مشروط، و«الثغرات» مشروطٌ بوجودِ اختباراتٍ لها، و«المتخطّى» مشروطٌ
    #      بادّعاءِ الملخّصِ نفسِه أنّ ثمّة ما لم يُشغَّل — فيُربَط الادّعاءُ بدليله.
    # (EN) A missing section reads green unless checked; each is required exactly
    #      when the generator emits it — «المتخطّى» when the report itself claims it.
    if "التباعدات" not in أقسامٌ_مرئيّة:
        إخفاقات.append("قسمُ «التباعدات» غائبٌ من الملخّص — والمولِّدُ يُصدِره دائمًا")
    if ثغرات and "ثغرات" not in أقسامٌ_مرئيّة:
        إخفاقات.append(
            f"{len(ثغرات)} اختبارَ ثغراتٍ في الشجرةِ ولا قسمَ لها في الملخّصِ أصلًا")
    if "لم يُشغَّل" in نصّ_الملخّص and "المتخطّى" not in أقسامٌ_مرئيّة:
        إخفاقات.append(
            "الملخّصُ يُقرّ بأنّ ثمّة ما لم يُشغَّل ولا قسمَ «المتخطّى» فيه — "
            "ادّعاءٌ بلا الدليلِ الذي يُحيل إليه")
    if not إحالات:
        إخفاقات.append("لم تُقرَأ إحالةٌ واحدةٌ من جداولِ التقريرِ الملخّص")
        return إخفاقات

    بمسار = {س for س in إحالات if "/" in س or "\\" in س}
    باسم = إحالات - بمسار

    بفاصل_ويندوز = sorted(س for س in بمسار if "\\" in س)
    if بفاصل_ويندوز:
        إخفاقات.append(f"فاصلُ ويندوز في {len(بفاصل_ويندوز)} إحالةً بالملخّص: "
                       f"{بفاصل_ويندوز[:3]}")

    يتيمة_بمسار = sorted(س for س in بمسار if س not in كلّ_الشجرة)
    if يتيمة_بمسار:
        إخفاقات.append(f"{len(يتيمة_بمسار)} إحالةً بمسارٍ لا تراه git: {يتيمة_بمسار[:5]}")

    يتيمة_باسم = sorted(س for س in باسم if س not in أسماء_الثغرات)
    if يتيمة_باسم:
        إخفاقات.append(
            f"{len(يتيمة_باسم)} إحالةً باسمٍ لا يقابله ملفٌّ في grammar_gaps: "
            f"{يتيمة_باسم[:5]}")

    # (AR) يُقبَل الشاهدُ بأيِّ الصيغتَين: المولِّدُ يكتب الثغراتِ بالاسمِ العاري
    #      اليومَ، لكنّ توحيدَ الجداولِ على `_report_path` تحسينٌ مغرٍ يكسر الحارسَ
    #      لو رُبِط بصيغةٍ واحدة. الشرطُ أن يُشهَد للملفّ، لا كيف كُتب اسمُه.
    # (EN) Accept either citation form; the guard binds to attestation, not format.
    غائبة_ثغرات = sorted(اسم for اسم, مسار in أسماء_الثغرات.items()
                         if اسم not in باسم and مسار not in بمسار
                         and اسم not in ثغرات_معفاة)
    معفاة_بلا_داعٍ = sorted(ثغرات_معفاة - set(أسماء_الثغرات))
    if معفاة_بلا_داعٍ:
        إخفاقات.append(f"{len(معفاة_بلا_داعٍ)} اسمًا مُعفًى لا وجودَ له في "
                       f"grammar_gaps — الإعفاءُ يجب أن ينحدر: {معفاة_بلا_داعٍ[:5]}")
    if غائبة_ثغرات:
        إخفاقات.append(
            f"{len(غائبة_ثغرات)} اختبارَ ثغراتٍ حيًّا بلا صفٍّ في الملخّص: "
            f"{غائبة_ثغرات[:5]}")

    return إخفاقات


def افحص() -> int:
    if not مسار_التقرير.exists() or not مجلد_العيّنات.is_dir():
        print("[حارس] ❌ تعذّر بلوغُ التقريرِ أو مجلّدِ العيّنات — لا حكمَ بلا مرجع",
              file=sys.stderr)
        return 1

    صفوف = اقرأ_صفوف_التقرير()
    if not صفوف:
        print("[حارس] ❌ لم يُقرَأ صفٌّ واحدٌ من التقرير — تغيّرت صيغةُ الجدول؟",
              file=sys.stderr)
        return 1

    try:
        كلّ_الشجرة = امسح_الشجرة_كاملة()
    except (OSError, RuntimeError) as علّة:
        print(f"[حارس] ❌ تعذّر سؤالُ git عن العيّنات: {علّة}", file=sys.stderr)
        return 1

    في_الشجرة = {س[len(بادئة_العيّنات):] for س in كلّ_الشجرة
                 if س.startswith(بادئة_العيّنات)}
    إخفاقات_الملخّص = افحص_الملخّص(كلّ_الشجرة)

    يتيمة = set(صفوف) - في_الشجرة
    غائبة = في_الشجرة - set(صفوف)
    بفاصل_ويندوز = sorted(س for س in صفوف if "\\" in س)

    # (AR) عمودا «القاعدة» و«الفئة» يُشتقّان من الملفِّ ومساره، فهما رخيصان — خلافًا
    #      للحالةِ والزمنِ اللذَين يلزمهما تشغيلٌ مزدوجٌ كامل. وصفٌّ باسمٍ صحيحٍ
    #      وقاعدةٍ مختلَقةٍ شهادةٌ كاذبةٌ كذلك.
    # (EN) The rule and category columns are derivable from the file and its path.
    أعمدة_مغلوطة = []
    for مسار, (قاعدة_مكتوبة, فئة_مكتوبة) in sorted(صفوف.items()):
        if مسار in يتيمة:
            continue
        ملفّ = مجلد_العيّنات / مسار
        if not ملفّ.is_file():
            continue
        نسبيّ = Path(مسار)
        وسوم = _extract_rule_ids(ملفّ)
        قاعدة_متوقّعة = _folder_rule_of(نسبيّ) or (وسوم[0] if وسوم else "—")
        فئة_متوقّعة = _category_of(نسبيّ)
        if قاعدة_مكتوبة != قاعدة_متوقّعة or فئة_مكتوبة != فئة_متوقّعة:
            أعمدة_مغلوطة.append(
                f"{مسار}: القاعدة «{قاعدة_مكتوبة}» ⇐ «{قاعدة_متوقّعة}» · "
                f"الفئة «{فئة_مكتوبة}» ⇐ «{فئة_متوقّعة}»")

    يتيمة_جديدة = sorted(يتيمة - يتيمة_معلَنة)
    غائبة_جديدة = sorted(غائبة - غائبة_معلَنة)
    # (AR) ما سُدِّد من الدَّين يُشطَب من الإعلان — وإلّا تجمّد السقفُ فلم ينحدر
    يتيمة_مسدَّدة = sorted(يتيمة_معلَنة - يتيمة)
    غائبة_مسدَّدة = sorted(غائبة_معلَنة - غائبة)

    عدد_الثغرات = sum(1 for س in كلّ_الشجرة if س.startswith(بادئة_الثغرات))
    print("حارس «تقريرا المطابقة يشهدان لموجود»:")
    print(f"  التفصيليّ: {مسار_التقرير.relative_to(الجذر).as_posix()}")
    print(f"  الملخّص:   {مسار_الملخّص.relative_to(الجذر).as_posix()}")
    print(f"  صفوف: {len(صفوف)} · عيّناتٌ في شجرة git: {len(في_الشجرة)} · "
          f"كاشفةُ ثغرات: {عدد_الثغرات}")
    print(f"  دَينٌ مُعلَن: يتيمة {len(يتيمة_معلَنة)}/{سقف_اليتيمة} · "
          f"غائبة {len(غائبة_معلَنة)}/{سقف_الغائبة}")

    فشل = False

    if len(يتيمة_معلَنة) > سقف_اليتيمة or len(غائبة_معلَنة) > سقف_الغائبة:
        print(f"  ❌ الدَّينُ المُعلَنُ تجاوز سقفَه ({سقف_اليتيمة}/{سقف_الغائبة}) — "
              "الإعلانُ مخرجُ طوارئَ لا عادة. ارفعِ السقفَ بتعديلٍ صريحٍ مُبرَّرٍ في "
              "هذا الملفّ، أو سدِّدِ الدَّينَ بتوليدٍ كامل.", file=sys.stderr)
        فشل = True

    if إخفاقات_الملخّص:
        print(f"  ❌ التقريرُ الملخّصُ ({len(إخفاقات_الملخّص)} إخفاقًا):", file=sys.stderr)
        for س in إخفاقات_الملخّص:
            print(f"     {س}", file=sys.stderr)
        فشل = True

    if أعمدة_مغلوطة:
        print(f"  ❌ {len(أعمدة_مغلوطة)} صفًّا عمودُ قاعدتِه أو فئتِه لا يطابق الملفَّ:",
              file=sys.stderr)
        for س in أعمدة_مغلوطة[:10]:
            print(f"     {س}", file=sys.stderr)
        فشل = True

    if بفاصل_ويندوز:
        print(f"  ❌ فاصلُ ويندوز في {len(بفاصل_ويندوز)} صفًّا — الملفُّ سينجرف عند "
              "أوّلِ توليدٍ على منصّةٍ مغايرة:", file=sys.stderr)
        for س in بفاصل_ويندوز[:5]:
            print(f"     {س}", file=sys.stderr)
        فشل = True

    if يتيمة_جديدة:
        print(f"  ❌ {len(يتيمة_جديدة)} صفًّا يشهد لملفٍّ غيرِ موجود (شهادةٌ كاذبة):",
              file=sys.stderr)
        for س in يتيمة_جديدة:
            print(f"     {س}", file=sys.stderr)
        فشل = True

    if غائبة_جديدة:
        print(f"  ❌ {len(غائبة_جديدة)} ملفًّا حيًّا بلا صفٍّ في التقرير:", file=sys.stderr)
        for س in غائبة_جديدة:
            print(f"     {س}", file=sys.stderr)
        فشل = True

    if فشل:
        print("  ⇦ العلاج: توليدٌ كامل يُحدِّث التقرير:", file=sys.stderr)
        print("     py -3.12 scripts/codegen/check_grammar_conformance.py --run "
              "--report-md language-truth/grammar/CONFORMANCE_REPORT.md", file=sys.stderr)
        print("     ثمّ حدِّث الدَّينَ المُعلَنَ في هذا الحارس.", file=sys.stderr)
        return 1

    if يتيمة_مسدَّدة or غائبة_مسدَّدة:
        print(f"  ❌ دَينٌ مُعلَنٌ سُدِّد ولم يُشطَب من الحارس "
              f"({len(يتيمة_مسدَّدة)} يتيمة · {len(غائبة_مسدَّدة)} غائبة) — "
              "السقفُ يجب أن ينحدر:", file=sys.stderr)
        for س in (يتيمة_مسدَّدة + غائبة_مسدَّدة)[:10]:
            print(f"     {س}", file=sys.stderr)
        return 1

    print("  ✓ سليم — لا شهادةَ لملفٍّ لا تراه git، ولا ملفَّ بلا شهادة، "
          "والملخّصُ وكاشفةُ الثغراتِ بلا دَينٍ أصلًا، والفواصلُ موحَّدة "
          "(عدا الدَّينِ المُعلَنِ في التفصيليّ وهو سقفٌ ينحدر).")
    return 0


if __name__ == "__main__":
    sys.exit(افحص())
