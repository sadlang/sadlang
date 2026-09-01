#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) قياسُ الرتبةِ التي يقبلُها **تنفيذُ المفسّرِ فعلًا** — ساكنًا بلا تشغيل.

     ¶ لماذا هذا المقياسُ ضروريّ

       حقلُ `arity` في مصدرِ الحقيقةِ **دعوى**، وتنفيذُ المدمَجِ في المفسّرِ
       **واقع**، وقد اختلفا. وحين وُصِلت بوّابةُ الرتبةِ بالمفسّرِ صارت الدعوى
       تُفرَض، فرُفِضت نداءاتٌ صحيحةٌ كانت تعمل — قِيس:
         · `تأكد_أكبر(5، 3، "رسالة")` ⇒ التنفيذُ يقرأُ `args[2]` وSoT يقول (2,2)
         · `ارسم_خط(...)` ⇒ التنفيذُ يشترطُ `args.size() >= 6` وSoT يقول (5,5)
           — أي أنّ المدمَجَ **غيرُ قابلٍ للنداءِ أصلًا** في المفسّر.

       ولا يُصلَحُ ذلك بتوسيعِ SoT بلا تروٍّ: `ارسم_خط` في المترجّمِ أوپكودُ
       إطارِ عرضٍ (`LOWLEVEL_FB_DRAW_LINE`) للوضعِ الحرّ لا نداءُ لوحةٍ، فهما
       تنفيذانِ مختلفانِ تحت اسمٍ واحد. توسيعُ الرتبةِ يغيّرُ مسارَ النواةِ
       صامتًا. فالتناقضُ **يُسمّى ويُقاس**، ولا تُفرَضُ رتبةٌ على اسمٍ يُختلَفُ
       فيه — أثرًا للقاعدةِ نفسِها التي حُذف بها `استبدل`.

     ¶ ماذا يقيسُ الماسحُ وما حدُّه

       يربطُ `registerBuiltinFunction(std::string(CONST), var)` بثابتِ الاسمِ
       العربيِّ من `builtin_registry_generated.h`، ثمّ يقرأُ جسمَ اللامدا
       `auto var = [...]` بموازنةِ الأقواسِ ويستخرجُ منه:
         · أعلى فهرسٍ مقروء `args[K]` ⇒ الحدُّ الأعلى المُلاحَظ = K+1
         · أدنى حجمٍ مشروط `args.size() < N` / `>= N` ⇒ الحدُّ الأدنى المُلاحَظ

     ¶ 🔑 ولا يقفُ عند لامدا التسجيل — فالعقدُ طبقةً أعمق

       أوّلُ نسخةٍ قرأت جسمَ اللامدا وحدَه، **فسكتت عن خمسةِ مدمَجاتٍ صارت
       نداءاتُها الصحيحةُ مرفوضةً** بعدَ وصلِ بوّابةِ الرتبةِ بالمفسّر. والعلّةُ
       أنّ الصيغةَ الغالبةَ لامدا لا تلمسُ `args[K]` أصلًا:

           auto array_pop_func = [](BuiltinContext &ctx) {
               ...
               return ...(StdLib::Core::ArrayFunctions::pop(plainArgs));  ← العقدُ هناك
           };

       وهناك: `validateArguments(args, 1, 1)`. فكان الماسحُ يُرجِعُ «لا معلومة»
       فيمرُّ الاسمُ «مطابَقًا» وهو مخالف — أداةُ قياسٍ تقيسُ **الطبقةَ الخطأ**.

       فيُتبَعُ الآن **مستوًى واحدٌ من الإحالة**: يُلتقَطُ نداءُ دالّةِ المكتبةِ
       من جسمِ اللامدا، ويُقرأُ `validateArguments(args, MIN, MAX)` من تعريفِها
       في `stdlib/` و`shared/builtins/src/`. والمقيسُ خمسة: `بحث` `دمج` `رتب`
       `فهرس_مصفوفة` `إزالة_عنصر` — وآخرُها كان **غيرَ قابلٍ للنداءِ بأيِّ رتبة**.

       ⚠️ حدُّه مُعلَن: لا يبلغُ ما سُجِّل بماكرو (`MAKE_WIDGET_*`) ولا
          `defineBuiltInFunction` ولا إحالةً بمستويَين. فما لم يُقَس **لا
          يُقصى** — الإقصاءُ على المُلاحَظِ وحدَه، وهو فشلٌ آمِنٌ: أسوأُ ما يقع
          أن تُحرَسَ أسماءٌ أقلّ، لا أن تُرفَضَ نداءاتٌ صحيحة.

(EN) Statically measures the argument count the interpreter implementation
     actually accepts, so SoT-declared arity is enforced only where the two
     agree. No execution: reading source, not running builtins.
============================================================================
"""
from __future__ import annotations

import functools
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INTERP_SRC = ROOT / "interpreter" / "src"
REGISTRY = ROOT / "shared" / "builtins" / "generated" / "builtin_registry_generated.h"

_CONST_RE = re.compile(
    r'inline\s+constexpr\s+(?:std::)?string_view\s+([A-Z][A-Z0-9_]*)\s*=\s*"([^"]+)"')
# (AR) المؤهِّلُ يُلتقَطُ ولا يُهمَل: هو مفتاحُ الفضاءِ الذي يُميّزُ `JOIN`
#      في `Strings` عنها في `Maps`.
_REGISTER_RE = re.compile(
    r'registerBuiltinFunction\(\s*std::string\(\s*(?:([A-Za-z_]\w*)::)?'
    r'([A-Z][A-Z0-9_]*)\s*\)\s*,\s*([A-Za-z_]\w*)\s*\)')
_LAMBDA_RE = re.compile(r'\bauto\s+(\w+)\s*=\s*\[')
_INDEX_RE = re.compile(r'\bargs\[\s*(\d+)\s*\]')
_SIZE_GE_RE = re.compile(r'\bargs\.size\(\)\s*(<|>=)\s*(\d+)')
_SIZE_LT_RE = re.compile(r'\b(\d+)\s*(>|<=)\s*args\.size\(\)')


_NAMESPACE_RE = re.compile(r'\bnamespace\s+(\w+)\s*\{')
_NS_ALIAS_RE = re.compile(r'\bnamespace\s+(\w+)\s*=\s*(?:::)?(?:[\w:]*::)?(\w+)\s*;')


def _const_names() -> dict:
    """{(الفضاء، المعرّف): الاسمُ العربيّ} من سِجِلِّ المدمَجاتِ المولَّد.

    🔑 والمفتاحُ **مُفَضَّأٌ** لا مُسطَّح: `JOIN` تعني «دمج» في `Strings` و«ربط»
    في `Maps` و«صل» في غيرِهما. وقاموسٌ مسطَّحٌ يفوزُ فيه الأخيرُ فيُنسَبُ جسمُ
    لامدا إلى **اسمٍ آخر**، فيُقاسُ عقدُ اسمٍ على تنفيذِ سواه.
    """
    text = REGISTRY.read_text(encoding="utf-8", errors="replace")
    table, stack, depth_of = {}, [], []
    depth = 0
    for line in text.splitlines():
        opened = _NAMESPACE_RE.findall(line)
        for name in opened:
            stack.append(name)
            depth_of.append(depth)
            depth += 1
        depth += line.count("{") - len(opened) - line.count("}")
        while depth_of and depth <= depth_of[-1]:
            stack.pop()
            depth_of.pop()
        for m in _CONST_RE.finditer(line):
            table[(stack[-1] if stack else "", m.group(1))] = m.group(2)
    return table


def _alias_map(text: str) -> dict:
    """`namespace Ba = …::Arrays;` ⇒ {Ba: Arrays} — لكلِّ ملفٍّ ألقابُه.

    🔑 و`Ba` تعني `Arrays` في ملفّاتِ النصوصِ و`UIAudio` في ملفّاتِ الواجهة،
    فلا يُقرأُ اللقبُ إلّا من الملفِّ الذي كُتِب فيه.
    """
    return {m.group(1): m.group(2) for m in _NS_ALIAS_RE.finditer(text)}


def _blank_noise(text: str) -> str:
    """يُعمّي التعليقاتِ والسلاسلَ والأحرفَ بمسافاتٍ مع حفظِ الأطوالِ والأسطر.

    🔑 موازنةُ الأقواسِ على النصِّ الخامِّ تُخطئ: `if (c == '{' || c == '[')` في
    `builtin_module_maps_json_xml.cpp` وسلسلةُ `":(){ :|:"` في
    `builtin_module_assertions.cpp` تجعلانِ الجسمَ المقروءَ أضعافَ الحقيقيِّ
    فيبتلعُ لامداتٍ تالية. والأطوالُ تُحفَظُ ليبقى كلُّ فهرسٍ صالحًا للنصِّ الأصل.
    """
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                out[i] = " "
                i += 1
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            while i < n and not (text[i] == "*" and i + 1 < n and text[i + 1] == "/"):
                if text[i] != "\n":
                    out[i] = " "
                i += 1
            for j in range(i, min(i + 2, n)):
                out[j] = " "
            i += 2
        elif c in "\"'":
            quote = c
            i += 1
            while i < n and text[i] != quote:
                if text[i] == "\\":
                    out[i] = " "
                    i += 1
                if i < n:
                    if text[i] != "\n":
                        out[i] = " "
                    i += 1
            i += 1
        else:
            i += 1
    return "".join(out)


def _lambda_body(text: str, start: int) -> str:
    """جسمُ اللامدا من `{` حتّى إغلاقِه الموازن — على نصٍّ مُعمًّى لا خامّ."""
    brace = text.find("{", start)
    if brace < 0:
        return ""
    depth, i = 0, brace
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[brace:i + 1]
        i += 1
    return ""


_DELEGATE_RE = re.compile(r'\b(?:[A-Za-z_]\w*::)+([A-Za-z_]\w*)\s*\(\s*(?:plainArgs|args)\s*\)')
_VALIDATE_RE = re.compile(r'validateArguments\s*\(\s*\w+\s*,\s*(\d+)\s*,\s*(\d+)\s*\)')
_LIB_DIRS = ("stdlib", "shared/builtins/src")


@functools.lru_cache(maxsize=1)
def _library_contracts() -> dict:
    """{اسمُ دالّةِ المكتبة: (أدنى، أعلى)} من `validateArguments` في تعريفِها.

    هذا هو **العقدُ الحقيقيّ**: لامدا التسجيلِ تُفوِّضُ، والحدُّ يُعلَنُ هنا.
    """
    out: dict = {}
    for rel in _LIB_DIRS:
        base = ROOT / rel
        if not base.is_dir():
            continue
        for path in sorted(base.rglob("*.cpp")):
            text = _blank_noise(path.read_text(encoding="utf-8", errors="replace"))
            for m in re.finditer(r'\b(?:[A-Za-z_]\w*::)*([A-Za-z_]\w*)\s*\([^;{)]*\)\s*\{', text):
                body = _lambda_body(text, m.end() - 1)
                v = _VALIDATE_RE.search(body)
                if v:
                    out.setdefault(m.group(1), (int(v.group(1)), int(v.group(2))))
    return out


def observed_ranges() -> dict:
    """{الاسمُ العربيّ: (أدنى مُلاحَظ أو None، أعلى مُلاحَظ أو None)}."""
    names = _const_names()
    contracts = _library_contracts()
    found: dict = {}
    for path in sorted(INTERP_SRC.rglob("*.cpp")):
        raw = path.read_text(encoding="utf-8", errors="replace")
        text = _blank_noise(raw)
        aliases = _alias_map(raw)
        # (AR) 🔑 مواضعُ اللامدات مرتَّبةً، ويُربَطُ التسجيلُ بـ**أقربِ تعريفٍ
        #      سابق** لا بقاموسٍ يفوزُ فيه الأخير: ستّةَ عشرَ ملفًّا تُعيدُ
        #      استعمالَ اسمِ المتغيّر (`auto f = [` خمسًا وأربعين مرّةً في
        #      ملفِّ HTTP وحدَه)، فكان جسمُ آخرِ لامدا يُنسَبُ إلى أوّلِ تسجيل.
        lambdas = [(m.start(), m.group(1), _lambda_body(text, m.end()))
                   for m in _LAMBDA_RE.finditer(text)]
        for m in _REGISTER_RE.finditer(text):
            qualifier, ident, var = m.group(1), m.group(2), m.group(3)
            ns = aliases.get(qualifier, qualifier) if qualifier else ""
            arabic = names.get((ns, ident)) or names.get(("", ident))
            if not arabic:
                continue
            body = next((b for pos, v, b in reversed(lambdas)
                         if v == var and pos < m.start()), None)
            if not body:
                continue
            indices = [int(i) for i in _INDEX_RE.findall(body)]
            top = max(indices) + 1 if indices else None
            low = None
            # (AR) العقدُ المُفوَّضُ يسبقُ المُلاحَظَ محلّيًّا: هو المُعلَنُ صراحةً.
            for call in _DELEGATE_RE.finditer(body):
                contract = contracts.get(call.group(1))
                if contract:
                    low, top = contract[0], max(contract[1], top or 0)
                    break
            prev = found.get(arabic)
            if prev:
                low = max([x for x in (low, prev[0]) if x is not None], default=None)
                top = max([x for x in (top, prev[1]) if x is not None], default=None)
            found[arabic] = (low, top)
    return found


def disagreements(sot_ranges: dict) -> list:
    """أسماءٌ يخالفُ فيها التنفيذُ المُلاحَظُ الرتبةَ المُعلَنة.

    `sot_ranges`: {الاسم: (min، max أو None لغيرِ المحدود)}.
    يُرجِعُ [(الاسم، أدنى مُعلَن، أعلى مُعلَن، أدنى مُلاحَظ، أعلى مُلاحَظ، العلّة)].
    """
    out = []
    observed = observed_ranges()
    for name, (lo, hi) in sorted(sot_ranges.items()):
        seen = observed.get(name)
        if not seen:
            continue                      # لم يُقَس ⇒ لا يُقصى
        seen_lo, seen_hi = seen
        # (AR) 🔑 أربعةُ اتّجاهاتٍ لا واحد. كان الفحصُ أعلى-فقط، فسكت عن
        #      `دمج` (التنفيذُ يقبلُ وسيطًا والدعوى تشترطُ اثنَين) وعن
        #      `إزالة_عنصر` — وهذا أخطرُها: عقدُه (1,1) والدعوى (2,2)، **لا
        #      تقاطعَ بينهما**، فصار غيرَ قابلٍ للنداءِ بأيِّ رتبةٍ بعدَ وصلِ
        #      البوّابة. حارسٌ أحاديُّ الاتّجاهِ يرى نصفَ الخلاف.
        why = None
        if hi is not None and seen_hi is not None and seen_hi < lo:
            why = "لا تقاطعَ بين الدعوى والتنفيذ — غيرُ قابلٍ للنداءِ بأيِّ رتبة"
        elif hi is not None and seen_lo is not None and seen_lo > hi:
            why = "التنفيذُ يشترطُ أكثرَ من الأعلى المُعلَن — غيرُ قابلٍ للنداء"
        elif hi is not None and seen_hi is not None and seen_hi > hi:
            why = "التنفيذُ يقبلُ وسيطًا فوقَ الأعلى المُعلَن"
        elif seen_lo is not None and seen_lo < lo:
            why = "التنفيذُ يقبلُ وسائطَ أقلَّ من الأدنى المُعلَن"
        if why:
            out.append((name, lo, hi, seen_lo, seen_hi, why))
    return out
