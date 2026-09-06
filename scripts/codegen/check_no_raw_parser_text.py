#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) حارس «لا نصَّ تشخيصٍ خامّ في المحلّل النحويّ».
#      كلّ رسالة خطأ/تحذير يجب أن تُركَّب مركزيًّا من كتالوج الأخطاء المولَّد من
#      language-truth/errors/*.yaml عبر errorCatalog/errorCatalogExpected/warnCatalog،
#      لا أن تُكتب حرفيًّا في الكود. النصّ المكتوب يدويًّا يفقد رمز الخطأ، فلا يراه
#      sad-check --json ولا الـLSP، ولا يُترجَم ولا يخضع لمستوى الشرح.
#
#      ⭐ لا يبحث عن نصّ عربيّ: أحد المواضع مكتوب بايتات سداسيّة ("\xD9\x85...")
#      هربًا من مشكلة ترميز، فيُفلت من أيّ بحث حرفيّ. الحارس يفحص **البنية**:
#      وسيط سلسلةٍ حرفيّةٍ يبلغ مصرفًا تشخيصيًّا — أيًّا كان ترميز محتواه.
#
#      نمط الدَّين: يبدأ بخطّ أساسٍ مُعلَن (BASELINE) بالمواضع المعروفة الباقية،
#      فيفشل على **الجديد** فقط. كلّما كُنِس موضع أُنقص من خطّ الأساس، ولا يُزاد
#      إليه أبدًا — هو سقف ينحدر إلى الصفر لا رصيد يُسحب منه.
#
# (EN) Guard: no hand-written diagnostic text in the parser. Every message must be
#      rendered centrally from the SoT-generated error catalog. Structural (not
#      text-based) so hex-escaped literals cannot hide. Ratchet baseline: fails on
#      new violations only; the baseline may shrink, never grow.
# ======================================================================
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **شبكةُ رمزِ الخروجِ قلبٌ واحدٌ** — انظر ترويسةَ الوحدة.
from _lib.guard_exit import رمز_الخروج  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SCOPE = ROOT / "shared" / "parser" / "src"

# (AR) المصارف التشخيصيّة التي يُمنع أن تبلغها سلسلةٌ حرفيّة.
#      errorCatalog* و warnCatalog مستثناة: وسائطها معرّفات رموز وحشوات، لا نثر.
#      errorWithFixIt وسيطه الأوّل رسالةٌ نصّيّة كذلك، فهو مصرفٌ حقيقيّ يجب حراسته.
SINKS = ("error", "errorAt", "warn", "warnAt", "errorWithFixIt")
# (AR) 🔑 **ومصرفانِ منها نثرُهما الوسيطُ الثاني لا الأوّل.** `errorAt(tok,
#      "نصّ")` و`warnAt` يأخذانِ الموضعَ أوّلًا، و`_CALL` يفحصُ الوسيطَ الأوّلَ
#      وحدَه — فكانا **مُعلَنَين في الصفِّ ولا يراهما الحارسُ أبدًا**، وهي عينُ
#      العلّةِ التي سُدَّت لـ`consume` وتُرِكَت لهما. (كشفَها العيارُ الذاتيُّ
#      حينَ صارت لكلِّ مصرفٍ عيّنةٌ موجبة: أخفقَ على عيّنةِ `errorAt`.)
#      ولا موضعَ نداءٍ لهما في النطاقِ اليومَ، فسدُّ البابِ لا يُنمّي خطَّ الأساس.
#      🔑 **والصفُّ يُشتَقُّ باللاحقةِ ولا يُهجّى**: نسخةٌ ثانيةٌ باليدِ تعني أنّ
#         مصرفًا ثالثًا ينتهي بـ`At` يُضافُ غدًا إلى `SINKS` وحدَه فيمرُّ عيارُه
#         **أخضرَ** (عيّنتُه المُشتقّةُ تضعُ النثرَ في الوسيطِ الأوّل) ونثرُه
#         الحقيقيُّ لا يراه قارئٌ واحد. (قِيسَ بحقنِ `noteAt`.)
SINKS_AT = tuple(name for name in SINKS if name.endswith("At"))

# (AR) نداء مصرف يعقبه — بعد أقواس/فراغ/أسطر — سلسلةٌ حرفيّة (تُلتقط بمحتواها).
_CALL = re.compile(
    r'(?<![A-Za-z0-9_])(?:' + "|".join(SINKS) + r')\s*\(\s*R?"((?:\\.|[^"\\])*)"'
)

# (AR) مصرفٌ يأخذُ الموضعَ أوّلًا ثمّ النثر — كـ`consume` سواء.
_CALL_AT = re.compile(
    r'(?<![A-Za-z0-9_])(?:' + "|".join(SINKS_AT) +
    r')\s*\(\s*[^,()]*(?:\([^()]*\))?[^,()]*,\s*R?"((?:\\.|[^"\\])*)"')

# (AR) ‏consume(نوع، "رسالة") — المصرف الذي كُنِست منه ٩٧ رسالة. وسيطُه الثاني هو
#      النثر، فلا يلتقطه _CALL (يفحص الوسيط الأوّل). بدونه كان الحارس يحرس
#      كلَّ شيءٍ إلّا البابَ الذي دخل منه الدَّين أصلًا.
#      ⚠️ وكان مكتوبًا هنا «كلّ مواضعه اليوم "" فارغة فإضافتُه لا تُنمّي خطّ
#         الأساس البتّة» — وهي **دعوى تنقضُها بياناتُ الملفِّ نفسِه**: `consume`
#         أكبرُ مُسهِمٍ في خطِّ الأساس، والسطرُ الذي يليها يقولُ ذلك صراحةً.
# (EN) consume(type, "msg") — the very sink the sweep emptied. The prose is the
#      SECOND argument, so _CALL (first-arg only) never saw it.
_CONSUME = re.compile(
    r'(?<![A-Za-z0-9_])consume\s*\(\s*[^,()]*(?:\([^()]*\))?[^,()]*,\s*R?"((?:\\.|[^"\\])*)"'
)
# (AR) std::cerr << "..."  — لافتة مطبوعة يدويًّا بلا تسجيل مركزيّ.
_CERR = re.compile(r'std::cerr\s*<<\s*(?:\n\s*)?"((?:\\.|[^"\\])*)"')

# (AR) تجريد التعليقات وسلاسل الأحرف حتّى لا تُحسب أمثلةُ التوثيق خرقًا.
#      ⚠ الفرع الأوّل (سلسلةٌ مزدوجة) لا يُجرَّد بل يُترك كما هو: بغيره كان
#      «//» داخل سلسلةٍ (مثل "انظر http://…") يُقرأ تعليقًا فيُمحى بقيّةُ السطر
#      ويُفلت الخرق. ترتيبُ البدائل هو الحارس: السلسلةُ تُلتقط قبل «//».
_STRIP = re.compile(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'', re.S)


# (AR) نداءُ `consume` **باسمِه** بلا اشتراطِ سلسلةٍ بعدَه — أرضيّةُ الحضورِ
#      البنيويّ. انظر `FLOOR_CONSUME_CALLS`.
_ANY_CONSUME = re.compile(r'(?<![A-Za-z0-9_])consume\s*\(')


def _blank(m):
    """(AR) يمحو التعليقَ/سلسلةَ الأحرف مبقيًا الأسطر، ويُبقي السلاسلَ المزدوجة."""
    s = m.group(0)
    if s.startswith('"'):
        return s
    return re.sub(r'[^\n]', ' ', s)

# (AR) بايتات سداسيّة داخل السلسلة — بها كُتب نصٌّ عربيٌّ هربًا من مشكلة ترميز.
_HEX = re.compile(r'\\x([0-9A-Fa-f]{2})')


def is_prose(literal: str) -> bool:
    """(AR) هل السلسلة نثرٌ موجَّهٌ للقارئ؟ الفواصلُ والرموزُ التزيينيّة ليست نثرًا.

    يفكّ ‎\\xNN إلى بايتاتها ثمّ يفكّ UTF-8، فيلتقط النصّ العربيّ المُرمَّز سداسيًّا
    كما يلتقط المكتوب مباشرةً. المعيار: وجود حرفٍ هجائيٍّ واحدٍ فأكثر.
    """
    had_hex = bool(_HEX.search(literal))
    raw = _HEX.sub(lambda m: chr(int(m.group(1), 16)), literal)
    # (AR) إسقاط ما تبقّى من محارف الهروب (\n \t \" \\) — حرف 'n' فيها ليس نثرًا.
    raw = re.sub(r'\\.', '', raw)
    # (AR) فكُّ UTF-8 يخصّ المكتوبَ بايتاتٍ سداسيّةً وحده. تطبيقُه على نصٍّ عربيٍّ
    #      مكتوبٍ مباشرةً كان يمحوه (latin-1 + ignore لا يمثّل ما فوق U+00FF)
    #      فيُفلت أيُّ تشخيصٍ عربيٍّ خالصٍ لا حرفَ لاتينيًّا فيه.
    if had_hex:
        try:
            raw = raw.encode("latin-1", "ignore").decode("utf-8", "ignore")
        except Exception:
            pass
    return any(ch.isalpha() for ch in raw)

# ──────────────────────────────────────────────────────────────────────
# (AR) خطّ الأساس: الدَّين المعروف يوم كتابة الحارس. مسموحٌ إنقاصه لا زيادته.
#      المفتاح مسار نسبيّ، والقيمة عدد المواضع المتبقّية في الملفّ.
# (EN) Baseline: known debt at guard-authoring time. May shrink, never grow.
# ──────────────────────────────────────────────────────────────────────
BASELINE = {
    # (AR) لافتاتُ «{ } في الطرق/الباني» ورسالةُ استدعاء الأساس — نثرٌ بلا رمز خطأ.
    "shared/parser/src/declarations/parser_oop.cpp": 3,
    # (AR) 1 رسالةُ «بعد كلمة النوع…» + 3 آثارِ تنقيحٍ إنجليزيّةٍ محروسةٍ بمتغيّر بيئة.
    "shared/parser/src/core/parser_main.cpp": 4,
    # (AR) ✅ كُنِس: «حالة» داخل «طابق» كانت تُطبَع بلا رمزٍ فلا يراها sad-check --json
    #      ولا الـLSP. صارت SYN031 بحشواتٍ كلُّها تهجئةُ معجمٍ من kw().
    "shared/parser/src/statements/parser_advanced.cpp": 0,
    # (AR) «متوقع اسم متغير بعد &» — مكتوبةٌ بايتاتٍ سداسيّةً فتُفلت من البحث النصّيّ.
    "shared/parser/src/core/parser_expressions.cpp": 1,
    # (AR) تلميحاتُ الاسترداد («🔧 هل تقصد…»، «✓ تمّ الاسترداد…») — تحتاج صنفَ
    #      «ملاحظة» في الكتالوج قبل أن تُكنَس، فهي ليست أخطاءً ولا تحذيرات.
    "shared/parser/src/core/parser_helpers.cpp": 10,
    # (AR) ١٩ رسالةَ consume متعدّدةَ الأسطر. (كانت ٢٠: كُنِست لافتةُ تحذيرِ
    #      الودجات المُهمَلة مع حذفِ خريطةِ المُهمَل كلِّها — لا توافقَ خلفيًّا.)
    #      ⚠ الرقم كان «1» قبل حراسة consume — لا لأنّ الدَّين ازداد بل لأنّ الحارسَ
    #      كان أعمى عن المصرف. الملفّ لم يُكنَس أصلًا؛ كنسُه يحتاج رفعَ رسائلِ
    #      consume إلى كتالوج الأخطاء (لا كتالوجَ ودجاتٍ ثانيًا — مصدرُ أسماءِ
    #      العُقَد صار language-truth/ui_nodes.yaml وحدَه).
    "shared/parser/src/ui/parser_ui.cpp": 19,
}

# (AR) 🔑 **«لا يُزاد إليه أبدًا» كان دعوى بلا قياس.** الحارسُ يُخفِقُ على
#      انحدارِ ملفٍّ عن رقمِه، لكنّ **رفعَ الرقمِ نفسِه** كان بابًا مفتوحًا:
#      تُكتَبُ خمسُ رسائلَ خامّةٍ ويُرفَعُ العددُ خمسًا فيخضرُّ الحارسُ في الحال —
#      وهو عينُ ما وُجِدَ ليمنعَه. فالمجموعُ تحتَ سقفٍ نازلٍ يُقاس.
CEILING_BASELINE_TOTAL = 37

# (AR) 🔑 **وصفرُ نداءٍ ليس سلامةً بل عدمَ نظر.** ولو عمِيَ قارئٌ لَخلَت
#      `found`، ولَقرأَ الحارسُ ذلك **تقدُّمًا**: «كُنِست مواضعُ ولم يُنقَص خطُّ
#      الأساس» — دعوى إنجازٍ كاذبةٌ عن مسحٍ لم يقعْ أصلًا.
#
#      ⚠️ **والأرضيّةُ على عددِ النداءاتِ كانت علاجًا خاطئًا، وقِيسَ لِمَ:**
#      نداءاتُ `error`/`warn` و`std::cerr` **هي الدَّينُ نفسُه** الذي وُجِدَ
#      الحارسُ ليكنسَه، فتؤولُ إلى صفرٍ بعملٍ مشروعٍ تمامًا — وأرضيّةٌ عليها
#      رفضٌ كاذبٌ مؤجَّلٌ يقعُ يومَ يكتملُ الكنس، وهو نقيضُ غايةِ الحارس.
#      وأرضيّةٌ على **المجموع** أسوأُ: `consume` وحدَه أكثرُ من ثمانيةِ أعشارِه
#      فيُخفيها، فيموتُ قارئا `error` و`cerr` معًا — وهما قرابةُ نصفِ المخالفاتِ
#      المُودَعة — والمجموعُ فوقَ أرضيّتِه. (كشفَته مراجعةٌ خصميّة.)
#
#      🔑 **فالعلاجُ عيارٌ ذاتيٌّ لا عدَّاد**: يُقابَلُ كلُّ قارئٍ في كلِّ تشغيلةٍ
#      بعيّنتَينِ ثابتتَين — موجبةٍ يجبُ أن يراها، وسالبةٍ يجبُ ألّا يراها —
#      فيُثبِتُ أنّه يُبصِرُ **بلا اعتمادٍ على حجمِ الدَّينِ الباقي**. والسالبةُ
#      لئلّا يُرضيَ العيارَ نمطٌ وُسِّعَ حتّى صارَ يطابقُ كلَّ شيء.
#      ⚠️ **والعيّنةُ الموجبةُ لكلِّ اسمِ مصرفٍ حرفًا، لا لواحدٍ منها.** كانت
#         `error` وحدَها تُعايَرُ و`SINKS` خمسةٌ، فحذفُ الأربعةِ الأُخَر — ومنها
#         `errorWithFixIt` وله مواضعُ نداءٍ حيّةٌ نصَّت الترويسةُ على حراستِها —
#         كان يمرُّ **٣/٣ وأخضرَ تمامًا**. والصفُّ يُقابَلُ بـ`SINKS` فلا يبقى
#         اسمٌ بلا عيّنة. (كشفَته مراجعةٌ خصميّة.)
#      ⚠️ **والسالبةُ تُختارُ لتعضَّ لا لتُفلِتَ بالصدفة.** كانت
#         `errorCatalog(Code::X, {});` وهي تُفلِتُ من **كلِّ** توسيعٍ واقعيٍّ
#         جُرِّبَ (إسقاطُ اللقطةِ الخلفيّةِ · إسقاطُ اشتراطِ القوسِ · إسقاطُ
#         اشتراطِ السلسلة) فلا تحرسُ حدًّا. وصارت صورتَينِ مقيستَينِ تعضّان.
_SINK_SAMPLES = tuple(
    (name, ('%s(tok, "زز");' % name) if name in SINKS_AT
     else ('%s("زز");' % name),
     "_CALL_AT" if name in SINKS_AT else "_CALL")
    for name in SINKS)
_SELF_TEST = (
    ("_CALL", 'error("زز");', 'errorCatalog("SYN001", {});'),
    ("_CALL", 'error("زز");', 'reporterror("زز");'),
    ("_CALL_AT", 'errorAt(tok, "زز");', "errorAt(tok, code);"),
    ("_CONSUME", 'consume(TT::SEMICOLON, "زز");', "consume(TT::SEMICOLON);"),
    ("_CERR", 'std::cerr << "زز";', "std::cout << 1;"),
    ("_ANY_CONSUME", "consume(TT::SEMICOLON);", "zzconsume(TT::X);"),
)

# (AR) وأرضيّةٌ **بنيويّةٌ** واحدة: `consume` مفردةٌ من مفرداتِ المحلّلِ لا من
#      دَينِه (الغالبيّةُ العظمى منه بلا نثرٍ أصلًا)، فعددُها لا ينزلُ بالكنس.
#      والعدُّ **باسمِ النداءِ وحدَه** لا بوجودِ سلسلةٍ بعدَه، وإلّا نزلَ العدَّادُ
#      بحذفِ وسيطٍ ميّتٍ مشروع — وأكثرُ نداءاتِ `consume` وسيطُها الثاني `""`.
#      والمقيسُ يُطبَعُ إلى جوارِ أرضيّتِه في كلِّ تشغيلة.
FLOOR_CONSUME_CALLS = 60


def scan(text: str) -> tuple:
    """(AR) يُعيد (المخالفات، عددَ نداءاتِ `consume`) — والثاني حضورٌ بنيويٌّ
    لا عدَّادُ مصارف: النثرُ صفرٌ في ملفٍّ مكنوسٍ **وفي ملفٍّ لم يُقرأْ سواء**،
    و`consume` مفردةٌ من مفرداتِ المحلّلِ لا من دَينِه فيُفرِّقُ بينهما.
    وبصرُ المصارفِ نفسِها يُقاسُ في `_self_test` لا ههنا."""
    # (AR) نُبقي أطوال النصّ كما هي (نستبدل بفراغات) ليظلّ رقم السطر صحيحًا.
    clean = _STRIP.sub(_blank, text)
    hits = []
    calls = len(_ANY_CONSUME.findall(clean))
    for rx in (_CALL, _CALL_AT, _CONSUME, _CERR):
        for m in rx.finditer(clean):
            if not is_prose(m.group(1)):
                continue  # فاصلٌ أو رمزٌ تزيينيّ لا نثرَ فيه
            ln = clean.count("\n", 0, m.start()) + 1
            snip = text[m.start():m.start() + 70].replace("\n", " ").strip()
            hits.append((ln, snip))
    return sorted(hits), calls


def _self_test() -> list:
    """(AR) يردُّ أسماءَ القرّاءِ الذين أخفقوا في عيارِ أنفسِهم — والصفُّ فارغٌ
    يعني أنّ الأربعةَ يُبصِرونَ الموجبةَ ولا يرَونَ السالبة، وأنّ كلَّ اسمِ
    مصرفٍ مُعلَنٍ في `SINKS` له عيّنةٌ موجبةٌ يراها."""
    readers = {"_CALL": _CALL, "_CALL_AT": _CALL_AT, "_CONSUME": _CONSUME,
               "_CERR": _CERR, "_ANY_CONSUME": _ANY_CONSUME}
    blind = []
    for name, positive, negative in _SELF_TEST:
        rx = readers[name]
        if not rx.search(positive):
            blind.append("%s لا يرى عيّنتَه الموجبة" % name)
        elif rx.search(negative):
            blind.append("%s يرى عيّنتَه السالبة (وُسِّعَ حتّى صارَ يطابقُ كلَّ شيء)"
                         % name)
    # (AR) 🔑 **وكلُّ اسمٍ في `SINKS` يُقابَلُ بعيّنتِه** — وإلّا سقطَ مصرفٌ من
    #      الصفِّ بلا أن يُخفِقَ العيار. والعيّنةُ تُشتَقُّ من الاسمِ لا تُهجّى.
    for name, sample, reader in _SINK_SAMPLES:
        if not readers[reader].search(sample):
            blind.append("مصرفٌ مُعلَنٌ لا يراه %s: %s" % (reader, name))
    return blind


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    if not SCOPE.is_dir():
        print("حارس نصّ المحلّل: المجلّد غير موجود: %s" % SCOPE)
        return 2

    files = sorted(SCOPE.rglob("*.cpp"))
    found = {}
    consume_calls = 0
    for f in files:
        rel = f.relative_to(ROOT).as_posix()
        hits, calls = scan(f.read_text(encoding="utf-8", errors="surrogateescape"))
        consume_calls += calls
        if hits:
            found[rel] = hits

    print("حارس «لا نصّ تشخيص خامّ في المحلّل النحويّ»:")
    print("  النطاق:   %s" % SCOPE.relative_to(ROOT).as_posix())
    print("  الملفّات: %d · نداءاتُ `consume` قِيسَت: %d (الأرضيّة %d)"
          " · عيارُ القرّاء: %d/%d"
          % (len(files), consume_calls, FLOOR_CONSUME_CALLS,
             len(_SELF_TEST) + len(_SINK_SAMPLES) - len(_self_test()),
             len(_SELF_TEST) + len(_SINK_SAMPLES)))

    # (AR) 🔑 **والعمى يُقالُ أوّلًا وبرمزِ ٢، وإلّا قُرِئَ تقدُّمًا.** صفرُ
    #      ملفٍّ أو مصارفُ عمياءُ تُفرِغانِ `found`، فتُطبَعُ بنودُ خطِّ الأساسِ
    #      كلُّها «كُنِست مواضعُ» — دعوى إنجازٍ عن مسحٍ لم يقعْ أصلًا، ورمزُ ١
    #      يقرؤه `x.py` حكمًا على المحتوى.
    if not files:
        print("  ✗ عطبُ آلة: لا ملفَّ مصدرٍ في النطاقِ — لم يُقَسْ شيء")
        return 2
    if consume_calls < FLOOR_CONSUME_CALLS:
        print("  ✗ عطبُ آلة: نداءاتُ `consume` %d دونَ الأرضيّةِ %d — "
              "النطاقُ المقروءُ ليس محلّلًا نحويًّا، ولم يُقَسْ شيء"
              % (consume_calls, FLOOR_CONSUME_CALLS))
        return 2
    blind = _self_test()
    if blind:
        print("  ✗ عطبُ آلة: قارئٌ لا يُبصِر — %s. لم يُقَسْ شيء" % "، ".join(blind))
        return 2

    regressions, improvements = [], []
    for rel, hits in sorted(found.items()):
        allowed = BASELINE.get(rel, 0)
        if len(hits) > allowed:
            regressions.append((rel, allowed, hits))
        elif len(hits) < allowed:
            improvements.append((rel, allowed, len(hits)))
    for rel, allowed in sorted(BASELINE.items()):
        if rel not in found and allowed:
            improvements.append((rel, allowed, 0))

    total_debt = sum(len(h) for h in found.values())
    baseline_total = sum(BASELINE.values())
    # (AR) والحدُّ يُكتَبُ ملاصقًا لعددِه ليقرأَه قارئُ الحدودِ في مِحقنةِ العيار
    #      — حدٌّ لا يُقرَأُ لا يُشَدُّ ولا يُحاكَم.
    print("  الدَّين الحاليّ: %d موضعًا في %d ملفًّا · خطُّ الأساس %d (السقف %d)"
          % (total_debt, len(found), baseline_total, CEILING_BASELINE_TOTAL))

    if regressions:
        print("  ✗ انحدار (%d ملفًّا) — رسالةٌ جديدةٌ مكتوبةٌ في الكود:" % len(regressions))
        for rel, allowed, hits in regressions:
            print("     %s: %d موضعًا والمسموح %d" % (rel, len(hits), allowed))
            for ln, snip in hits[allowed:]:
                print("        :%d  %s" % (ln, snip))
        print("  الصواب: errorCatalog(Errors::ErrorCode::SYN_xxx, {{...}}) — "
              "والرموز من language-truth/errors/*.yaml.")
        return 1

    if improvements:
        print("  ⚠ كُنِست مواضعُ ولم يُنقَص خطّ الأساس — حدّثْ BASELINE في هذا الملفّ:")
        for rel, allowed, now in improvements:
            print("     %s: %d ⇐ %d" % (rel, now, allowed))
        return 1

    # (AR) وخطُّ الأساسِ نفسُه يُحاكَم: رفعُ رقمِ ملفٍّ كان بابًا مفتوحًا يُخضِرُ
    #      الحارسَ على الرسالةِ الخامّةِ التي وُجِدَ ليمنعَها.
    #      🔑 **ويُقاسُ بعدَ الانحدارِ والكنسِ لا قبلَهما**: شكوى المحتوى أقربُ
    #      إلى المُصلِحِ من شكوى الميزان، ولو تقدّمَت لَابتلعَت تشخيصَهما —
    #      ورفعُ رقمٍ يُشعِلُ الاثنَين معًا فيُقرَأُ الأبعدُ عن العلاج.
    if baseline_total > CEILING_BASELINE_TOTAL:
        print("  ✗ خطُّ الأساسِ %d فوقَ السقفِ %d: ينحدرُ ولا يُرفَع: ارفعِ "
              "الرسالةَ إلى كتالوجِ الأخطاءِ بدلَ رفعِ الرقم."
              % (baseline_total, CEILING_BASELINE_TOTAL))
        return 1

    if total_debt:
        print("  ✓ لا انحدار — الدَّين المتبقّي ضمن خطّ الأساس المُعلَن (وهو سقفٌ ينحدر).")
    else:
        print("  ✓ سليم — لا نصّ تشخيص خامّ في المحلّل إطلاقًا.")
    return 0


if __name__ == "__main__":
    sys.exit(رمز_الخروج(main))
