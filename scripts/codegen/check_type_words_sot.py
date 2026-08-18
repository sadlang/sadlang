#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
(AR) حارس «ألفاظ الأنواع من مصدر الحقيقة» — لا جدولَ ألفاظٍ مُصلَّبًا ينجرف.

     المشكلة التي وُلد لها: المحلّل النحويّ كان يحمل نسخةً مُصلَّبةً من أسماء
     الأنواع السطحيّة، فبقيت فيها «مضاعف» بعدما أُزيلت من اللغة (٢٠٢٦-٠٨-١١).
     نسخةٌ لا يقرؤها أحدٌ من المصدر تنجرف بصمت، والأخضرُ لا يكشفها لأنّ السلوك
     الظاهر لم يتغيّر.

     ما يقيسه: يستخرج **جدول اللفظ ← النوع الفعليّ** من الشيفرة المكتوبة بيد
     (نمط `name == "لفظ"` ثمّ `SadTypeKind::نوع` في نافذةٍ قصيرة)، ويقارنه
     بمصدر الحقيقة `language-truth/types.yaml`:

       ① كلّ لفظٍ يُخفَّض إلى نوعٍ يجب أن يكون سطحيًّا في SoT (surface: true)،
          أو مُعلَنًا في `removed_type_words` (لفظٌ أُزيل ويُبلَّغ عنه SYN014).
       ② كلّ لفظٍ سطحيٍّ يجب أن يُخفَّض إلى `kind` الذي يُعلنه SoT — لا سواه.

     ⚠️ الاستثناءات الموثَّقة تعيش في `DECLARED_DIVERGENCES` أدناه بسببٍ مكتوب،
     لا في صمتٍ: خطُّ أساسٍ يُنقَص لا سقفٌ يُرفَع.

(EN) Guard: type words must come from the SoT (language-truth/types.yaml), not
     from hand-maintained copies. Extracts the hand-written word→SadTypeKind
     table and diffs it against the SoT surface types.
================================================================================
"""

from __future__ import annotations

import io
import pathlib
import re
import sys

import yaml

ROOT = pathlib.Path(__file__).resolve().parents[2]
TYPES_YAML = ROOT / "language-truth" / "types.yaml"

# (AR) الملفّات التي تحمل جدول ألفاظٍ مكتوبًا بيد. إضافة ملفٍّ هنا توسّع التغطية.
# (EN) Files carrying a hand-written word table. Add files to widen coverage.
#      `full_table=True` ⇒ الملفّ يحمل جدولَ «اللفظ ⇒ النوع» كاملًا، فيُطالَب بتغطية
#      كلِّ لفظٍ سطحيٍّ ومُزال؛ وغيرُه يُفحَص صحّةً لا اكتمالًا.
#      العدد = كم جدولًا يُتوقَّع في الملفّ (0 ⇒ لا اكتمالَ مطلوبًا، صحّةٌ فقط).
#      🔑 تثبيتُ **العدد** ضروريّ: بلا هذا يكفي تهديمُ جدولٍ حتّى يهبط دون عتبة
#      MIN_TABLE_WORDS فيسقط من الفحص ويبقى «سليم» — قِيس أنّ حذف ٨ فروعٍ كان يمرّ
#      أخضرَ بينما حذفُ فرعٍ واحدٍ يحمرّ: انعكاسٌ يكشف الضررَ الصغيرَ ويعمى عن الكبير.
#      و`0` ليست إعفاءً: تعني «يجب ألّا يحمل هذا الملفُّ جدولًا» — فإعادةُ بناءِ
#      جدولٍ فيه تُحمِّر الحارس. (قِيس: جدولٌ سادسٌ في `parser_advanced.cpp` عاش
#      سنينَ منجرفًا لأنّه خارجَ أيّ نطاقِ فحص، وقد وُحِّد الآن.)
#      ⚠️ **صفرٌ في `parser_helpers.cpp` منذ سدِّ ISSUE-113**: جداولُه الثلاثةُ
#      المكتوبةُ بيدٍ أُبدلت بالتفويضِ إلى المُولَّد `sadTypeKindFromArabicName`.
#      وهذا يقلب معنى «صفر» هنا خطرًا جديدًا: حذفُ التفويضِ كلِّه يُرضي «صفرَ
#      جداول» فيمرّ أخضر. ولذلك وُلد فحصُ التفويضِ في `check_delegation` أدناه —
#      العدّادُ وحدَه لم يعد كافيًا.
#      🔑 و«صفرٌ» تُقرأ الآن **صفرَ روابطَ بأيِّ اصطلاح** لا صفرَ عناقيدَ فوق العتبة
#      (`any_idiom` في `extract_table`): أثبتت ثلاثُ بذورٍ خصوميّةٍ أنّ القفلَ السابقَ
#      يُهزَم بردِّ ثلاثةِ فروعٍ (دون MIN_TABLE_WORDS)، أو بجدولِ `std::map` لا تراه
#      WORD_RE، أو بحذفِ التفويضِ مع إبقاءِ الاسمِ في سلسلةٍ حرفيّة — وكلُّها كانت
#      خضراءَ وقد أُبطل الإصلاح. والثلاثُ حمراءُ الآن، مقيسةً لا مُدّعاة.
SCANNED = [
    (pathlib.Path("shared") / "parser" / "src" / "core" / "parser_helpers.cpp", 0),
    (pathlib.Path("shared") / "parser" / "src" / "declarations" / "parser_declarations.cpp", 0),
    (pathlib.Path("shared") / "parser" / "src" / "statements" / "parser_advanced.cpp", 0),
]

# (AR) تباعُداتٌ مُعلَنةٌ بسببها — كلٌّ منها دَينٌ مرصود، لا رخصةُ صمت.
# (EN) Declared divergences, each with a written reason — tracked debt, not silence.
#      ⚠️ **فارغةٌ عمدًا.** كان فيها مدخلٌ واحدٌ («عدم» ⇒ Void بدل Null، ISSUE-113)
#      وقد سُدَّ الدَّينُ بالقياس: `عدم` نوعٌ مستقلٌّ الآن في المحلّلِ والسجلّ معًا.
#      وحذفُ المدخلِ شرطُ الإصلاحِ لا زينتُه — لو بقي لَقَبِلَ الحارسُ ارتدادَ
#      «عدم» إلى Void غدًا وهو أخضر، فيصير الإصلاحُ بلا قفل.
DECLARED_DIVERGENCES: dict[tuple[str, str, str], str] = {}

# (AR) الطرفان مقبولان: `name == "لفظ"` و`"لفظ" == name` — الترتيبُ المعكوسُ
#      (نمطُ Yoda) كان يُخفي الجدولَ كلَّه عن الاستخراج.
WORD_RE = re.compile(r'(?:==\s*"([؀-ۿ_٠-٩0-9]+)"|"([؀-ۿ_٠-٩0-9]+)"\s*==)')

# (AR) 🔑 اللفظُ المكتوبُ **بمحارفَ سُداسيّة** — `rn == "\xD9\x85\xD8\xB6…"`.
#      بلا هذا كان أخطرُ انجرافٍ في المستودعِ غيرَ مرئيٍّ للحارس: أعادت مراجعةٌ
#      خصوميّة بناءَ الجدولِ السُداسيِّ المنجرفِ نفسِه في `parser_advanced.cpp`
#      فبقي الحارسُ **أخضر** (صفرُ عناقيدَ يطابق «المتوقَّع صفر»)، بينما أثبت
#      البناءُ الحيُّ عودةَ العطب: `ترجع مضاعف` ⇒ `1.5` بخروجٍ ٠ بلا تشخيص.
#      فالحارسُ الذي يبحث عن ألفاظٍ عربيّةٍ وحدَها يُكافئ إخفاءَ اللفظ.
# (EN) A word written as hex escapes. Without decoding these, re-introducing the
#      exact drifted table went green — the guard rewarded hiding the word.
#      وتُقبَل الصيغةُ **الثمانيّةُ** أيضًا (`\331\205`) لأنّها بديلٌ مكافئٌ للإخفاء.
HEX_WORD_RE = re.compile(
    r'(?:==\s*"((?:\\(?:x[0-9A-Fa-f]{2}|[0-7]{1,3}))+)"'
    r'|"((?:\\(?:x[0-9A-Fa-f]{2}|[0-7]{1,3}))+)"\s*==)'
)


# (AR) 🔑 اللفظُ المُزالُ لا يُعفى لمجرّدِ أنّه مُعلَنٌ مُزالًا — **أثرُه** هو مسوّغُه.
#      قِيس: حذفُ `errorCatalog(SYN_REMOVED_SYNTAX)` من فرعِ «مضاعف» مع بقاءِ الفرعِ
#      يمرّ أخضرَ في حارسٍ يكتفي بـ`continue`، بينما يفقد المستعمِلُ التشخيصَ الودّيَّ
#      كلَّه — وهو بعينِه التدهورُ الذي وُلد `removed_type_words` لمنعِه.
#      ⚠️ **ولا يكفي البحثُ عن اسمِ الرمز**: مراجعةٌ خصوميّةٌ أثبتت أنّ إبقاءَ السطرِ
#      **تعليقًا** بعد حذفِ النداء يُبقي الحارسَ أخضرَ والتشخيصَ ساقطًا — أي أنّ
#      أشهرَ صيغةِ تعطيلٍ («علِّقْه مؤقّتًا») تهزم الحارسَ الذي وُلد لمنعِها. فيُشترَط
#      **نداءٌ فعليٌّ** `errorCatalog(... SYN_REMOVED_SYNTAX`، وتُجرَّد التعليقاتُ أوّلًا.
# (EN) Matching the symbol name is not enough: commenting the call out kept the
#      guard green while the diagnostic died. Require a real call, comments stripped.
REMOVED_DIAG_RE = re.compile(r"errorCatalog\s*\([^;]{0,200}?SYN_REMOVED_SYNTAX")

LINE_COMMENT_RE = re.compile(r"//.*$", re.MULTILINE)
BLOCK_COMMENT_RE = re.compile(r"/\*.*?\*/", re.DOTALL)


STRING_LITERAL_RE = re.compile(r'"(?:[^"\\\n]|\\.)*"')


def strip_comments(text: str) -> str:
    """(AR) يُسقط التعليقاتِ كي لا يُحسَب المُعطَّلُ عاملًا. (EN) Drop comments."""
    return LINE_COMMENT_RE.sub("", BLOCK_COMMENT_RE.sub("", text))


def strip_comments_and_strings(text: str) -> str:
    """(AR) يُسقط التعليقاتِ **والسلاسلَ الحرفيّة** معًا — لفحوصِ «هل يُنادى فعلًا؟».

    🔑 وُلدت هذه من بذرةٍ خصوميّةٍ مرّت خضراء: حُذف التفويضُ كلُّه إلى المُولَّد،
    وأُبقي اسمُ الدالّةِ في **سلسلةٍ حرفيّة** (`const char *k = "sadTypeKind…";`).
    فـ`strip_comments` يجرّد التعليقاتِ ولا يجرّد السلاسل، والفحصُ كان يبحث عن
    **ورودِ الاسمِ** لا عن نداءٍ — فبقي أخضرَ وقد زال الحلُّ كلُّه. والدرسُ أعمُّ من
    الثغرة: أشهرُ صيغتَي تعطيلٍ هما التعليقُ والسلسلةُ الحرفيّة، فمن جرّد إحداهما
    وترك الأخرى لم يسدَّ الباب بل نقله.
    (EN) Born from an adversarial seed: deleting every delegation call while keeping
    the function name inside a string literal kept the guard green. Comments were
    stripped; strings were not; and the check tested occurrence, not invocation.
    """
    return STRING_LITERAL_RE.sub('""', strip_comments(text))


ESCAPE_RE = re.compile(r"\\(?:x([0-9A-Fa-f]{2})|([0-7]{1,3}))")


def decode_hex_word(raw: str) -> str:
    """(AR) «\\xD9\\x85…» أو «\\331\\205» ⇒ «مضاعف». (EN) Decode escaped UTF-8."""
    out = bytearray()
    for hexv, octv in ESCAPE_RE.findall(raw):
        out.append(int(hexv, 16) if hexv else int(octv, 8))
    return bytes(out).decode("utf-8", errors="replace")
# (AR) 🔑 الاستخراج مقيَّدٌ باصطلاحِ **الإسنادِ أو الإرجاع** — لا بأيّ ذكرٍ للنوع في
#      الفرع. بلا هذا القيد التقطَ الحارسُ فحصًا نحويًّا محضًا في
#      `parser_declarations.cpp` (لفظُ «مصفوفة» في `متغير ساكن اسم مصفوفة[N]`)
#      وربطَه بـ`SadTypeKind::Integer` تجميليٍّ يبعد ١٨ سطرًا ⇒ إيجابٌ كاذب.
#      واصطلاحان مدعومان لأنّ المحلّل يحمل جدولَين: أحدهما يُسنِد `SadTypeKind::X`
#      والآخر يستدعي `reg.getX()` — وتجاهُل الثاني كان ثغرةَ تغطيةٍ صامتة.
# (EN) Extraction is anchored to an assignment/return of a kind — two idioms,
#      since the parser carries two tables (`SadTypeKind::X` and `reg.getX()`).
KIND_RE = re.compile(
    r"(?:resolved\s*=|return)[^;]*?(?:SadTypeKind::([A-Za-z_][A-Za-z0-9_]*)"
    r"|reg\.(?:get|make)([A-Za-z_][A-Za-z0-9_]*)\s*\()"
)

# (AR) مُنشِئاتُ السجلّ للأنواع المركّبة تُسمّى `makeArray`/`makeMap` لا `getArray`.
#      إغفالُها جعل فرعَ «خريطة» يمتدّ بحثُه إلى `getOrCreateClass` البعيدة
#      فيُبلَّغ نوعٌ وهميٌّ اسمه «OrCreateClass» — إيجابٌ كاذبٌ ثانٍ من الجنسِ نفسِه.
# (EN) Registry constructors for composites are makeArray/makeMap, not getArray.

# (AR) 🔑 حدُّ البحثِ يمتدّ حتّى **اللفظِ التالي** لا عددًا ثابتًا من الأسطر. النافذةُ
#      الثابتةُ (٤ أسطر) أخفقت في أوّل اختبارِ بذرة: فرعُ «مضاعف» يفصله مسارُ
#      `errorCatalog` عن إسنادِ النوعِ بخمسةِ أسطر، فلم يُستخرَج أصلًا — والحارسُ
#      خضِر بلا أن يرى اللفظَ الذي كُتب لأجله. السقفُ أدناه شبكةُ أمانٍ لا أكثر.
# (EN) The scan runs to the NEXT word match, not a fixed line count: a fixed
#      4-line window silently skipped the «مضاعف» branch (its errorCatalog
#      recovery path pushes the assignment 5 lines away), so the guard went
#      green without ever seeing the word it exists for.
MAX_BRANCH_LINES = 40

# (AR) عنقودٌ بأقلَّ من هذا ليس جدولًا (فحوصٌ عابرةٌ للفظٍ واحدٍ أو اثنين).
#      ⚠️ **حدٌّ مُعلَنٌ لا مُنكَر**: جدولٌ منجرفٌ بأربعةِ ألفاظٍ يمرّ أخضرَ — قِيس
#      ببذرةٍ خصوميّة. خفضُ العتبةِ يُدخِل إيجابًا كاذبًا على الفحوصِ النحويّةِ
#      العابرة، فالثمنُ مقصود؛ والحدُّ الحقيقيُّ يُغلَق بالتفويضِ لا بالعتبة.
MIN_TABLE_WORDS = 5

# (AR) ⚠️ **حدودٌ مُعلَنةٌ أخرى** (مقيسةٌ ببذورٍ خصوميّةٍ تمرّ خضراء، فلا تُنسَب للحارس
#      قدرةٌ لا يملكها):
#        · المسحُ الشجريُّ يغطّي `shared/parser/**` وحدَه؛ جدولٌ في `shared/lexer`
#          أو `shared/semantic` لا يُرى.
#        · التغطيةُ تُقاس على النصِّ لا على الأثر: فرعٌ سليمُ النصِّ في مسارٍ ميّتٍ
#          يمرّ. الأثرُ يُحرَس بالاختباراتِ السالبةِ لا بهذا الحارس.


def load_sot():
    """(AR) يعيد: ألفاظ سطحيّة {لفظ: kind}، وألفاظ مُزالة {لفظ: بديلها}."""
    data = yaml.safe_load(io.open(TYPES_YAML, encoding="utf-8"))
    surface = {
        t["word"]: t["kind"] for t in data["types"] if t.get("surface") is True
    }
    removed = {
        r["word"]: r.get("replacement", "") for r in (data.get("removed_type_words") or [])
    }
    return surface, removed


# (AR) 🔑 **لفظٌ عربيٌّ حرفيٌّ مهما كان الاصطلاح** — لا `== "لفظ"` ولا سواه. WORD_RE
#      مقيَّدةٌ باصطلاحِ المقارنةِ عمدًا (تقليلًا للإيجابِ الكاذبِ في الملفّاتِ التي
#      يُتوقَّع فيها جدول)، وأثبتت بذرةٌ خصوميّةٌ ثمنَ ذلك: جدولٌ يدويٌّ مُعادٌ بصيغةِ
#      `std::map<std::string، SadTypeKind>` بألفاظٍ عربيّةٍ صريحة **لا تراه** WORD_RE
#      فيمرّ أخضرَ وهو يُبطِل التفويضَ كلَّه. فتُستعمَل هذه في الملفّاتِ المُعلَنِ
#      فيها **صفرُ جداول**: هناك لا إيجابَ كاذبًا يُخشى، إذ القاعدةُ «لا لفظَ يُربَط
#      بنوعٍ هنا أصلًا» — والاصطلاحُ لا يشتري إعفاءً.
# (EN) Any Arabic/hex literal, in ANY idiom. Used where zero tables are declared:
#      there the rule is "no word maps to a kind here at all", so idiom buys no exemption.
ANY_WORD_RE = re.compile(
    r'"([؀-ۿ_٠-٩0-9]+)"|"((?:\\(?:x[0-9A-Fa-f]{2}|[0-7]{1,3}))+)"'
)

# (AR) ونظيرُها على طرفِ النوع: KIND_RE مقيَّدةٌ بـ`resolved =`/`return` فلا ترى
#      مدخلَ قاموسٍ `{"عدم"، SadTypeKind::Void}` ولا إسنادًا إلى حقلٍ. وفي وضعِ
#      «صفرِ الجداول» القيدُ ضررٌ محض: أيُّ ذكرٍ لنوعٍ بجوارِ لفظٍ هو الربطُ المُحرَّم.
# (EN) The kind side, unanchored: a map entry binds a word to a kind just as an
#      assignment does, and where zero tables are declared any binding is the violation.
ANY_KIND_RE = re.compile(
    r"SadTypeKind::([A-Za-z_][A-Za-z0-9_]*)"
    r"|reg\.(?:get|make)([A-Za-z_][A-Za-z0-9_]*)\s*\("
)


def extract_table(path: pathlib.Path, any_idiom: bool = False):
    """(AR) يستخرج **كلَّ** مواضع `name == "لفظ"` ⇒ نوع، لا أوّلَ موضعٍ لكلّ لفظ.

    🔑 كانت النتيجة قاموسًا بـ`setdefault`، فابتلع الجدولَ الثاني كلَّه: كلُّ لفظٍ
    يظهر مرّتين (جدول `SadTypeKind::X` ثمّ جدول `reg.getX()`)، والقاموسُ يحتفظ
    بالأوّل ويُسقط الثاني بصمت — فتمرّ بذرةُ عطبٍ في الجدول الثاني خضراء.

    `any_idiom=True` ⇒ يُلتقَط أيُّ لفظٍ حرفيٍّ يُجاور نوعًا مهما كان الاصطلاح
    (انظر ANY_WORD_RE) — للملفّاتِ المُعلَنِ فيها صفرُ جداول.
    """
    raw = io.open(path, encoding="utf-8", errors="ignore").read()
    # (AR) في وضعِ «أيُّ اصطلاح» تُجرَّد التعليقاتُ **قبل** تحديدِ المواضع لا بعده:
    #      وإلّا صار تعليقٌ يذكر لفظًا بين علامتَي اقتباسٍ إيجابًا كاذبًا. والتجريدُ
    #      يحفظ عددَ الأسطر كي تبقى أرقامُ السطورِ صادقةً في التشخيص.
    # (EN) Strip comments before locating candidates in any-idiom mode, line-preservingly.
    if any_idiom:
        raw = BLOCK_COMMENT_RE.sub(lambda m: "\n" * m.group(0).count("\n"), raw)
        raw = LINE_COMMENT_RE.sub("", raw)
    lines = raw.splitlines()

    def word_at(ln):
        if any_idiom:
            a = ANY_WORD_RE.search(ln)
            if a:
                return a.group(1) or decode_hex_word(a.group(2))
            return None
        m = WORD_RE.search(ln)
        if m:
            return m.group(1) or m.group(2)
        h = HEX_WORD_RE.search(ln)
        return decode_hex_word(h.group(1) or h.group(2)) if h else None

    starts = [i for i, ln in enumerate(lines) if word_at(ln) is not None]
    found = []
    for pos, i in enumerate(starts):
        word = word_at(lines[i])
        # (AR) الفرعُ ينتهي عند اللفظِ التالي (أو عند السقف) — لا عند عدٍّ ثابت.
        end = starts[pos + 1] if pos + 1 < len(starts) else len(lines)
        end = min(end, i + MAX_BRANCH_LINES, len(lines))
        body = strip_comments("\n".join(lines[i:end]))
        k = (ANY_KIND_RE if any_idiom else KIND_RE).search(body)
        if k:
            found.append(
                (
                    word,
                    k.group(1) or k.group(2),
                    i + 1,
                    enclosing_function(lines, i),
                    REMOVED_DIAG_RE.search(body) is not None,
                )
            )
    return found


# (AR) تعريفُ دالّةٍ عضوٍ: `النوع Class::name(` — يُلتقَط اسمُها لنسبِ الجدولِ إليها.
FUNC_DEF_RE = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*::([A-Za-z_][A-Za-z0-9_]*)\s*\(")


def enclosing_function(lines, index: int) -> str:
    """(AR) اسمُ الدالّةِ الحاويةِ للسطر — هويّةُ الجدولِ بدل قُربِ السطور.

    🔑 العنقدةُ بفجوةِ سطورٍ ثابتةٍ هشّةٌ من الطرفَين: حشوُ جدولٍ سليمٍ بستّين سطرًا
    يشطره جدولَين (إيجابٌ كاذب)، وتقاربُ جدولَين حقيقيَّين يدمجهما (سلبٌ كاذب —
    وقد وقع فعلًا حين وُحِّدت السلسلةُ في دالّةٍ جوارَ أختِها). الحدُّ الطبيعيُّ
    للجدولِ هو الدالّةُ التي يعيش فيها، لا مسافةٌ نُقدّرها.
    """
    for j in range(index, -1, -1):
        m = FUNC_DEF_RE.search(lines[j])
        if m and not lines[j].lstrip().startswith(("//", "*", "/*")):
            return m.group(1)
    return "<ملفّ>"


def cluster_tables(entries):
    """(AR) يقسم المواضع المستخرَجة إلى **جداولَ** (عناقيدَ متقاربةٍ في السطور).

    🔑 التغطيةُ لكلّ *ملفّ* لا تكفي: المحلّل يحمل جدولَين لكلّ لفظ، فكسرُ فرعٍ في
    الجدول الأوّل يبقى مستورًا بوجود اللفظِ في الثاني — وقد مرّت بذورُ «حذفِ فرعٍ»
    و«الإسنادِ عبر وسيط» و«التعليقِ الطويل» خضراءَ لهذا السبب. فالتغطيةُ تُطالَب
    **بكلّ جدولٍ على حدة**.
    """
    clusters = []
    by_func = {}
    for word, _kind, lineno, func, _diag in sorted(entries, key=lambda e: e[2]):
        current = by_func.get(func)
        if current is None:
            current = {"first": lineno, "last": lineno, "words": set(), "func": func}
            by_func[func] = current
            clusters.append(current)
        current["last"] = lineno
        current["words"].add(word)
    # (AR) عنقودٌ صغيرٌ ليس جدولًا بل فحصٌ عابر ⇒ يُفحَص صحّةً لا اكتمالًا.
    return [c for c in clusters if len(c["words"]) >= MIN_TABLE_WORDS]


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    surface, removed = load_sot()
    problems = []
    declared_hits = []
    scanned_words = 0

    print("حارس «ألفاظ الأنواع من مصدر الحقيقة»:")
    print("  المصدر:   language-truth/types.yaml")
    print("  سطحيّة:   %d لفظًا · مُزالة مُعلَنة: %d" % (len(surface), len(removed)))

    seen_per_file = {}
    for rel, expected_tables in SCANNED:
        path = ROOT / rel
        if not path.exists():
            problems.append("ملفٌّ مفقودٌ من نطاق الفحص: %s" % rel.as_posix())
            continue
        # (AR) 🔑 «صفرٌ» تعني **صفرَ روابط**، لا «صفرَ عناقيدَ تبلغ العتبة». وهذا هو
        #      سدُّ أخطرِ ثغرتَين أثبتتهما بذورٌ خصوميّة: ردُّ ثلاثةِ فروعٍ فقط
        #      (`عدم ⇒ Void`) يبقى دون MIN_TABLE_WORDS فيمرّ أخضرَ وقد أُبطل الإصلاحُ
        #      كلُّه؛ وجدولٌ بصيغةِ `std::map` لا تراه WORD_RE أصلًا. فحيث لا جدولَ
        #      مُعلَنًا يُمسَح الملفُّ **بأيِّ اصطلاح**، ولا يُعفى لفظٌ سطحيٌّ ولو كان
        #      تخفيضُه مطابقًا لمصدرِ الحقيقةِ اليومَ: المطابقُ اليومَ هو المنجرفُ غدًا،
        #      والسببُ الذي وُلد له التفويضُ هو **وجودُ نسخةٍ** لا خطؤُها.
        # (EN) Zero means zero bindings, not zero clusters above a threshold. Restoring
        #      three branches stayed under MIN_TABLE_WORDS and went green; a std::map
        #      table was invisible to WORD_RE. A word mapped correctly today is the
        #      drifted copy tomorrow — delegation exists to forbid the copy, not its error.
        #      ⚠️ وحدُّ هذا الوضعِ المُعلَنُ: يُقتصَر على الألفاظِ التي **يعرفها SoT**
        #      (سطحيّةً أو مُزالة). بلا هذا القيدِ يلتقط المسحُ نصوصَ التشخيصِ نفسَها
        #      («المتغير»، «الدالة»، «حقل») فتُقرأ جدولًا وهي رسائلُ — قِيس: ١١ إيجابًا
        #      كاذبًا من ١٣. والثمنُ مُعلَن: جدولٌ يُبنى بلفظٍ ليس في SoT أصلًا لا يُرى
        #      هنا، وهو مقبولٌ لأنّ لفظًا خارجَ SoT ليس نسخةً تنجرف عنه.
        # (EN) Restricted to words the SoT knows; otherwise diagnostic message text
        #      («المتغير», «حقل») reads as a table — measured: 11 false positives of 13.
        zero_declared = expected_tables == 0
        table = extract_table(path, any_idiom=zero_declared)
        if zero_declared:
            table = [e for e in table if e[0] in surface or e[0] in removed]
        scanned_words += len(table)
        # (AR) يُسجَّل كلُّ ملفٍّ — حتّى المتوقَّعُ خاليًا — كي يكشفَ العددُ جدولًا
        #      أُعيد بناؤه حيث لا يجوز، لا أن يمرَّ لأنّ الفحصَ لم يشمله.
        seen_per_file[rel.as_posix()] = (cluster_tables(table), expected_tables)
        for word, kind, lineno, _func, has_diag in table:
            loc = "%s:%d" % (rel.as_posix(), lineno)
            if word in removed:
                # (AR) لفظٌ مُزالٌ مُعلَن: وجودُه مقصودٌ — **بشرطِ أن يُبلّغ**.
                #      فرعٌ يقبله صامتًا هو الانحدارُ عينُه لا استثناءً مشروعًا.
                if not has_diag:
                    problems.append(
                        "%s — «%s» لفظٌ مُزالٌ يُقبَل **بلا تشخيص** "
                        "(لا SYN_REMOVED_SYNTAX في فرعه): قبولٌ صامتٌ لا مسارَ إرشاد"
                        % (loc, word)
                    )
                continue
            if word not in surface:
                problems.append(
                    "%s — «%s» يُخفَّض إلى SadTypeKind::%s وهو ليس لفظًا سطحيًّا في SoT "
                    "ولا مُعلَنًا في removed_type_words" % (loc, word, kind)
                )
                continue
            if zero_declared:
                problems.append(
                    "%s — «%s» يُربَط بـSadTypeKind::%s بيدٍ في ملفٍّ مُعلَنٍ بصفرِ جداول: "
                    "نسخةٌ يدويّةٌ عادت مكانَ التفويضِ إلى «sadTypeKindFromArabicName» "
                    "(ولو طابقت SoT اليومَ — الوجودُ هو المُحرَّم لا الخطأ)"
                    % (loc, word, kind)
                )
                continue
            expected = surface[word]
            if kind != expected:
                key = (word, kind, expected)
                if key in DECLARED_DIVERGENCES:
                    declared_hits.append(
                        "%s — «%s» ⇒ %s بدل %s (مُعلَن: %s)"
                        % (loc, word, kind, expected, DECLARED_DIVERGENCES[key])
                    )
                else:
                    problems.append(
                        "%s — «%s» يُخفَّض إلى SadTypeKind::%s ومصدرُ الحقيقة يقول %s"
                        % (loc, word, kind, expected)
                    )

    # ────────────────────────────────────────────────────────────────────────
    # (AR) 🔑 تأكيدُ **التغطية** — الفحصُ أعلاه أحاديُّ الجانب: يفحص ما استُخرج
    #      ولا يفحص أنّ كلَّ ما يجب استخراجُه استُخرج. وقد أثبتت مراجعةٌ خصوميّة
    #      أنّ الحارسَ يعمى صمتًا في ثلاث حالات — إسنادٌ عبر متغيّرٍ وسيط،
    #      وتعليقٌ يتجاوز MAX_BRANCH_LINES، و**حذفُ فرعِ لفظٍ كلّيًّا** — لأنّ
    #      العدّاد ينخفض ويبقى «سليم». فالغيابُ هنا خطأٌ لا صمت.
    # (EN) Coverage assertion: the checks above only validate what was extracted.
    #      A missing branch (or one written in an unrecognised idiom) silently
    #      lowered the count and stayed green — an adversarial review proved it.
    # ────────────────────────────────────────────────────────────────────────
    expected = set(surface) | set(removed)
    for rel, (clusters, expected_tables) in seen_per_file.items():
        if len(clusters) != expected_tables:
            problems.append(
                "%s — عدد جداول «لفظ ⇒ نوع» %d والمتوقَّع %d (جدولٌ هُدِم دون العتبة، "
                "أو أُضيف جدولٌ لم يُعلَن)" % (rel, len(clusters), expected_tables)
            )
        if not expected_tables:
            continue  # (AR) لا اكتمالَ يُطالَب به حيث لا جدولَ مُعلَن.
        for c in clusters:
            missing = sorted(expected - c["words"])
            if missing:
                problems.append(
                    "%s:%d-%d — جدولٌ ناقصٌ %d لفظًا (فرعٌ محذوفٌ أو اصطلاحٌ غير معروف): %s"
                    % (rel, c["first"], c["last"], len(missing), "، ".join(missing))
                )

    # ────────────────────────────────────────────────────────────────────────
    # (AR) 🔑 **اكتشافٌ لا قائمة.** `SCANNED` تُعلن ما نعرفه، وبذرةٌ خصوميّةٌ أثبتت
    #      أنّ جدولًا منجرفًا في ملفٍّ **خارجَ القائمة** يمرّ أخضرَ لأنّ الحارسَ لا
    #      يفتحه أصلًا — وهو تمامًا كيف عاش الجدولُ السادسُ سنينَ. فيُمسَح شجرُ
    #      المحلّلِ كلُّه: أيُّ ملفٍّ يحمل جدولًا وليس مُعلَنًا �⇒ خطأ، لا صمت.
    # (EN) Discovery, not a list: any parser file carrying a table must be declared.
    # ────────────────────────────────────────────────────────────────────────
    #      ⚠️ ووُسِّع المسحُ إلى `shared/types/` مع سدِّ ISSUE-113: أخطرُ نسخةٍ
    #      منجرفةٍ (`SadTypeRegistry::fromArabicName`) عاشت هناك — خارجَ كلِّ
    #      نطاقٍ — فطوت «عدم» و«لاشيء» في Void وأسقطت «طبيعي64» رأسًا، ولم
    #      يحمرَّ لها حارس. والمُولَّدُ يُستثنى فهو المصدرُ لا نسخةٌ عنه.
    declared = {rel.as_posix() for rel, _ in SCANNED}
    tree = sorted(
        p
        for root in (ROOT / "shared" / "parser", ROOT / "shared" / "types")
        for ext in ("*.cpp", "*.h", "*.hpp")
        for p in root.rglob(ext)
        if "generated" not in p.parts
    )
    for path in tree:
        rel_p = path.relative_to(ROOT).as_posix()
        if rel_p in declared:
            continue
        if cluster_tables(extract_table(path)):
            problems.append(
                "%s — يحمل جدولَ «لفظ ⇒ نوع» غيرَ مُعلَنٍ في SCANNED "
                "(نسخةٌ سابعةٌ تنجرف بصمت): أعلِنْه أو فوّضْ إلى resolveTypeWordName" % rel_p
            )

    # ────────────────────────────────────────────────────────────────────────
    # (AR) 🔑 **الاشتقاقُ نفسُه محروس.** الجداولُ أعلاه تُفحَص، أمّا `isTypeToken`
    #      فيقرأ المصفوفتَين المُولَّدتَين — وإسقاطُ `REMOVED_TYPE_NAMES` منه كان
    #      يمرّ أخضرَ وهو **بعينِه** التدهورُ الموثَّقُ في ISSUE-114: يسقط SYN014
    #      («أُزيلت — استخدم عشري») إلى SYN016 («الاسم يحتوي مسافة») الذي ينصح
    #      باستعمالِ اللفظِ المُزالِ اسمًا للمتغيّر. فوجودُ المصدرِ شرطٌ لا زينة.
    # (EN) Guard the derivation itself: dropping REMOVED_TYPE_NAMES from
    #      isTypeToken silently downgrades SYN014 to SYN016 (ISSUE-114).
    # ────────────────────────────────────────────────────────────────────────
    helpers = ROOT / "shared" / "parser" / "src" / "core" / "parser_helpers.cpp"
    if helpers.exists():
        src = strip_comments_and_strings(io.open(helpers, encoding="utf-8", errors="ignore").read())
        for arr in ("SURFACE_TYPE_NAMES", "REMOVED_TYPE_NAMES"):
            if arr not in src:
                problems.append(
                    "shared/parser/src/core/parser_helpers.cpp — «%s» المُولَّدةُ غيرُ "
                    "مستعمَلة: تعرُّفُ المحلّلِ لم يعد مشتقًّا من SoT (ISSUE-114)" % arr
                )

    # ────────────────────────────────────────────────────────────────────────
    # (AR) 🔑 **فحصُ التفويض** — وُلد مع سدِّ ISSUE-113. بعدما صار المتوقَّعُ «صفرَ
    #      جداولٍ مكتوبةٍ بيد»، صار الأخضرُ يُنال بطريقَين: بالتفويضِ إلى المُولَّد
    #      (المقصود)، أو **بحذفِ الحلِّ رأسًا** (كارثة). فيُشترَط أمران:
    #        ① المستهلِكون ينادون `sadTypeKindFromArabicName` فعلًا؛
    #        ② الجدولُ المُولَّدُ نفسُه يطابق SoT لفظًا بلفظ — فمُولِّدٌ منجرفٌ
    #           يُنتِج جدولًا منجرفًا في كلِّ المستهلِكين دفعةً واحدة، وهو ضررٌ
    #           أوسعُ من أيِّ نسخةٍ يدويّةٍ كانت.
    # (EN) Delegation check: with zero hand tables expected, green is also reachable
    #      by deleting the logic. Require real calls into the generated map, and
    #      require the generated map itself to match the SoT word for word.
    # ────────────────────────────────────────────────────────────────────────
    GENERATED_FN = "sadTypeKindFromArabicName"
    # (AR) ⚠️ حُذف `shared/types/include/sad_type_system.h` من المستهلِكين مع حذفِ
    #      `SadTypeRegistry::fromArabicName` نفسِها (صفرُ مُنادٍ، ISSUE-113): اشتراطُ
    #      نداءٍ في ملفٍّ لم يعد فيه ما يُنادي يجعل الحارسَ يطلب إحياءَ الميّتِ لا
    #      حراسةَ الحيّ. والمستهلِكُ الحقيقيُّ الوحيدُ هو مسارُ تصريحِ المتغيّر.
    # (EN) The header was dropped from CONSUMERS together with the dead function it
    #      hosted; demanding a call in a file with nothing to call resurrects dead code.
    CONSUMERS = [
        pathlib.Path("shared") / "parser" / "src" / "core" / "parser_helpers.cpp",
    ]
    # (AR) 🔑 **نداءٌ لا ورود.** `GENERATED_FN in src` يُرضيه اسمٌ في سلسلةٍ حرفيّةٍ
    #      أو في مُعرِّفٍ آخرَ يحتويه — وقد قِيست البذرةُ خضراءَ والحلُّ محذوف. فيُشترَط
    #      قوسُ نداءٍ، وتُجرَّد السلاسلُ مع التعليقات.
    # (EN) Invocation, not occurrence: require a call paren, on string-stripped source.
    CALL_RE = re.compile(re.escape(GENERATED_FN) + r"\s*\(")
    for rel in CONSUMERS:
        path = ROOT / rel
        if not path.exists():
            problems.append("ملفٌّ مفقودٌ من فحص التفويض: %s" % rel.as_posix())
            continue
        src = strip_comments_and_strings(
            io.open(path, encoding="utf-8", errors="ignore").read()
        )
        if not CALL_RE.search(src):
            problems.append(
                "%s — لا ينادي «%s» المُولَّدة نداءً فعليًّا: جدولُ الألفاظِ لم يعد "
                "مشتقًّا من SoT (حذفُ التفويضِ يُرضي «صفرَ جداول» فيمرّ صامتًا — ISSUE-113)"
                % (rel.as_posix(), GENERATED_FN)
            )

    gen_header = ROOT / "shared" / "types" / "generated" / "sad_type_kind_generated.h"
    if not gen_header.exists():
        problems.append("الرأسُ المُولَّدُ مفقود: shared/types/generated/sad_type_kind_generated.h")
    else:
        gen_src = io.open(gen_header, encoding="utf-8", errors="ignore").read()
        body = gen_src.split(GENERATED_FN, 1)[1] if GENERATED_FN in gen_src else ""
        body = body.split("return SadTypeKind::Unknown", 1)[0]
        gen_map = {}
        for raw, kind in re.findall(
            r'word\s*==\s*"((?:\\x[0-9A-Fa-f]{2})+)"\s*\)\s*return\s+SadTypeKind::([A-Za-z_]\w*)',
            body,
        ):
            gen_map[decode_hex_word(raw)] = kind
        if not gen_map:
            problems.append(
                "sad_type_kind_generated.h — «%s» غائبةٌ أو فارغة: المُولِّدُ لم "
                "يُصدِر جدولَ «اللفظ ⇒ النوع» (gen_types.py)" % GENERATED_FN
            )
        for word, kind in sorted(surface.items()):
            if word not in gen_map:
                problems.append(
                    "sad_type_kind_generated.h — «%s» لفظٌ سطحيٌّ في SoT وغائبٌ عن "
                    "الجدولِ المُولَّد: يُقرأ اسمَ صنفٍ لا نوعًا" % word
                )
            elif gen_map[word] != kind:
                problems.append(
                    "sad_type_kind_generated.h — «%s» ⇒ SadTypeKind::%s ومصدرُ "
                    "الحقيقة يقول %s" % (word, gen_map[word], kind)
                )
        for word in sorted(set(gen_map) - set(surface)):
            problems.append(
                "sad_type_kind_generated.h — «%s» في الجدولِ المُولَّدِ وليس لفظًا "
                "سطحيًّا في SoT: توسيعٌ صامتٌ للمقبول" % word
            )

    print("  الملفّات: %d · ألفاظ مفحوصة: %d" % (len(SCANNED), scanned_words))
    for d in declared_hits:
        print("  ⚠️ تباعُدٌ مُعلَن: %s" % d)

    if problems:
        print("  ❌ انجرافٌ عن مصدر الحقيقة (%d):" % len(problems))
        for p in problems:
            print("     - %s" % p)
        return 1

    print("  ✓ سليم — كلّ لفظ نوعٍ في الشيفرة مشتقٌّ من types.yaml أو مُعلَنٌ مُزالًا.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
