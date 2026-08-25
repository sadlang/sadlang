#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) استخراجُ **مواضعِ فرضِ الرتبةِ** لمدمجاتِ المترجم من أذرعِ الأماميّة.

     الرتبةُ (أقلُّ عددِ وسائطَ وأكثرُه) كانت مكتوبةً أرقامًا حرفيّةً في أذرعِ
     `builtins_*.cpp` ولا يعرفُها مصدرُ الحقيقةِ أصلًا — فالعقدُ الذي يراه
     الكاتبُ في الوثائقِ لا يقيسُه أحد، وتغييرُ الرقمِ في الشيفرةِ لا يحمرُّ.

     هذه الوحدةُ هي **أداةُ القياس** المشتركةُ بين البذرةِ والحارس: تردُّ لكلِّ
     موضعِ فحصٍ الأسماءَ التي تحرسُها الذراعُ وصيغةَ المدى — رقمينِ حرفيَّين
     (`literal`) أو ثابتًا مُولَّدًا من مصدرِ الحقيقة (`constant`).

     ⚠️ المدى مربوطٌ بمقارنةِ الاسمِ التي تسبقُ النداء (`funcName == Bn::NS::ID`)
     لأنّ موضعَ الفحصِ يمرّرُ المتغيّرَ `funcName` لا الثابت. والمقارناتُ قد
     تتكدّسُ (ذراعٌ واحدةٌ لعدّةِ أسماء) فتُجمَع كلُّها لا الأخيرةُ وحدَها.

     ⚠️ ولا تقرأ هذه الوحدةُ مصدرَ الحقيقةِ البتّة: أداةُ القياسِ لا تستمدُّ
     بصرَها من المقيس، وإلّا قارنت الشيءَ بنفسِه فلم تحمرَّ أبدًا.
(EN) Shared measuring instrument: returns every arity-enforcement site with the
     builtin names its arm guards and whether the range is a literal or a
     generated SoT constant. It never reads the SoT itself.
============================================================================
"""
from __future__ import annotations

import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILDERS = ROOT / "compiler" / "src" / "frontend" / "builders"

# (AR) التعليقاتُ تُمحى بمسافاتٍ تحفظُ المواضع — تعليقٌ يذكرُ مقارنةَ اسمٍ ليس
#      مقارنة، واحتسابُه يربطُ الرتبةَ بمدمجٍ لا علاقةَ له بها.
_COMMENT = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)
# (AR) المقارنةُ تُكتَب باسمٍ مستعارٍ يختارُه كلُّ ملفّ: `Bn::NS::ID` حيث
#      `Bn = Sad::Builtins::Names`، أو `Bm::ID` حيث `Bm = …::Names::Math`.
#      🔑 وكان هذا المستخرِجُ يعرفُ `Bn` وحدَه، فمئةُ ذراعٍ في الرياضيّاتِ
#         والنصوصِ والمصفوفاتِ والخرائطِ كانت **خارجَ بصرِه** — لا يراها
#         مفروضةً فيَعدُّها دَينًا، ويربطُ مقارنةً معلَّقةً بفحصٍ ليس لها.
#         فصارت الأسماءُ المستعارةُ **تُقرأ من الملفِّ نفسِه** لا تُسرَد هنا.
_ALIAS_DECL = re.compile(
    r"namespace\s+(\w+)\s*=\s*Sad::Builtins::Names(?:::(\w+))?\s*;")
_NAME_COMPARE = re.compile(r"funcName\s*==\s*(\w+)::(\w+)(?:::(\w+))?")
# (AR) 🔑 مرساةُ الرقمِ الحرفيِّ كانت `check\w*Arity\(` وحدَها، فحارسٌ سُمّي
#      `reportUiWrongArgCount` مرّ من تحتِها بسبعةٍ وعشرين رقمًا حرفيًّا
#      والحارسُ يعلنُ «✗ لا رقمَ حرفيًّا» صادقًا في حدودِ ما يرى. المرساةُ
#      على **الفعلِ** — حارسُ رتبةٍ أو عددِ وسائطَ أيًّا سُمّي — لا على هجاءٍ
#      واحدٍ منه؛ وإلّا كفى اسمٌ جديدٌ ليعودَ الأخضرُ كذبًا.
_ARITY_HELPER = r"(?:check|report|assert|verify|ensure)\w*(?:Arity|ArgCount|ArgsCount)"
_LITERAL_CALL = re.compile(
    _ARITY_HELPER + r"\(\s*[\w.\->_]+\s*,\s*[^,()]+\s*,\s*(\d+)\s*,\s*(?:(\d+)|[\w.\->_]+\s*\()")
# (AR) وثوابتُ **طرائقِ الأنواعِ** ثلاثيّةُ المستوى (`Ar::TypeMethods::Map::SET`)
#      لأنّ سطحَها مقسومٌ بالهدف. وقراءةُ مستويَين منها تُنتِج اسمًا لا وجودَ له
#      (`TypeMethods::Map`) فيحمرُّ الحارسُ «فرضٌ غيرُ مُعلَن» على حراسةٍ سليمة.
_CONSTANT_CALL = re.compile(
    _ARITY_HELPER + r"\([^;]*?(?:Ar|Sad::Builtins::Arity)::"
    r"(?:TypeMethods::(?P<tm_target>\w+)::(?P<tm_id>\w+)|(\w+)::(\w+))")
# (AR) صيغةٌ ثانيةٌ للفرض: **جدولٌ** يقرن الاسمَ بالأوپكودِ وبالمدى في مدخلٍ واحد
#      (`{Bn::UICore::GEN_WEB, …, Ar::UICore::GEN_WEB}`) ثمّ تدورُ عليه حلقةٌ
#      تفحصُ رتبةَ ما طابق. لا `funcName ==` فيها، فكانت أربعةُ مدمجاتٍ تُقرأ
#      **مُعلَنةً غيرَ مفروضة** وهي مفروضةٌ فعلًا — مسحٌ ميكانيكيٌّ يفوته الشكلُ
#      الثاني للصيغةِ نفسِها. والقرانُ ههنا صريحٌ في سطرٍ واحد فيُقرأ مباشرةً.
_TABLE_ROW = re.compile(
    r"\{\s*\w+::(\w+)::(\w+)\s*,[^{}]*?(?:Ar|Sad::Builtins::Arity)::(\w+)::(\w+)\s*\}")
# (AR) صيغةٌ ثالثةٌ: ذراعٌ واحدةٌ لعدّةِ أسماءٍ **تنتقي** ثابتَ المنادى بشرطٍ ثمّ
#      تمرّرُ المنتقى. والمداةُ لا تعبرُ الفاصلةَ المنقوطةَ فلا ترى الانتقاءَ،
#      فتُقرأ عقودُ الإخوةِ **إعلاناتٍ ميّتة** وهي مفروضةٌ باسمِها. والقرانُ
#      يُقرأ من الرابطِ نفسِه: مرجعٌ يُسنَد إليه ثوابتُ الرتبةِ ثمّ يُمرَّر.
_SELECTED_CONST = re.compile(
    r"(?:const\s+auto\s*&|auto\s*&&?)\s*(\w+)\s*=\s*([^;]*?"
    r"(?:Ar|Sad::Builtins::Arity)::\w+::\w+[^;]*?);")
_SELECTED_USE = re.compile(_ARITY_HELPER + r"\([^;]*?\b{}\b")
_CONST_IN_SELECTION = re.compile(
    r"(?:Ar|Sad::Builtins::Arity)::"
    r"(?:TypeMethods::(?P<tm_target>\w+)::(?P<tm_id>\w+)|(\w+)::(\w+))")


def _const_pair(m: "re.Match") -> tuple[str, str]:
    """(AR) زوجُ (الفضاء، المعرّف) من ثابتٍ ثنائيِّ المستوى أو ثلاثيّه."""
    if m.groupdict().get("tm_target"):
        return (f"TypeMethods::{m.group('tm_target')}", m.group("tm_id"))
    groups = [g for g in m.groups() if g is not None]
    return (groups[-2], groups[-1])
# (AR) صيغةٌ ثالثةٌ للفرض — وهي الأكثرُ عددًا: الرفضُ مكتوبٌ **في الشرطِ نفسِه**
#      (`if (argResults.size() < 2) { errors_.push_back(…); return …; }`) فالعددُ
#      لا يُمرَّر إلى حارسٍ بل يسكنُ المقارنة. هذه مواضعُ فرضٍ قائمةٌ فعلًا —
#      لا خطرَ تجاوزِ حدودٍ فيها — لكنّ عددَها خارجَ مصدرِ الحقيقة.
#
# 🔑    وكان الحارسُ لا يراها فيعدُّها **دَينًا**، فرقمُ الدَّينِ يبالغُ في السوء:
#       يخلطُ «لا أحدَ يفرض» بـ«يُفرَض برقمٍ محلّيّ». وهما حالان مختلفتان في
#       الخطرِ وفي العلاج، فتُفصَلان.
_INLINE_COMPARE = re.compile(
    r"\b(?:argResults|argOperands|args|arguments)\.size\(\)\s*"
    r"(<=|>=|<|>|!=|==)\s*(\d+)")
# (AR) لا يُقرَأ الشرطُ نقصًا إلّا بهذه المقارنات. و`>` نقيضُها: معالجةُ وسيطٍ
#      زائدٍ اختياريّ، وقراءتُها رفضًا هي عينُ الخطأِ الذي اختلق عقدَ
#      `معرف_المعالج`. والتمييزُ بالمعنى لا بوجودِ `return`.
_DEFICIENCY_OPS = {"<", "<=", "!="}
# 🔑 (AR) و`empty()` صيغةٌ ثانيةٌ للنقصِ نفسِه لا تذكرُ `size()` أصلًا، فكانت
#         الذراعُ الخامسةُ **عمياءَ عنها** — وقِيس تبخّرانِ حقيقيّانِ في
#         `اقرأ_منفذ16/32` تحتَ خضرةِ الحارس. أداةٌ لا ترى إلّا هجاءً واحدًا
#         للفعلِ تشهدُ بالسلامةِ حيثُ لم تنظر.
_EMPTY_COMPARE = re.compile(
    r"\b(?:argResults|argOperands|args|arguments)\.empty\(\)")


def _is_silent_rejection(op: str, block: str) -> bool:
    """(AR) هل هذه ذراعُ رفضٍ لا تسجّلُ خطأً فيتبخّرُ النداءُ صامتًا؟

    الشرطان معًا: مقارنةُ نقصٍ، وتبعةٌ **لا تُصدر عملًا** — لا تعليمةَ ولا
    نداءً لباني. ذراعٌ تُصدر عملًا ليست رفضًا مهما بدا شكلُها.
    """
    if op not in _DEFICIENCY_OPS or "return" not in block:
        return False
    # (AR) `return std::nullopt` **تفويضٌ لا رفض**: الذراعُ تقول «ليست لي»
    #      فيتلقّفُها مَن بعدَها في سلسلةِ الإرسال، والتشخيصُ يقعُ هناك. عدُّها
    #      تبخّرًا يتّهمُ موضعًا سليمًا — والأداةُ التي تُحمِّرُ الصحيحَ تُهجَر.
    if "std::nullopt" in block:
        return False
    emits = ("instructions.push_back", "SIRInstruction", "emit", "build")
    if any(token in block for token in emits):
        return False
    # (AR) 🔑 والعودةُ بقيمةٍ **صالحة** عند الفراغِ ليست رفضًا بل افتراضًا:
    #      `if (args.empty()) return SadTypeKind::Float;` استنتاجُ نوعٍ لا
    #      رفضُ نداء. التبخّرُ يُعرَف بأثرِه: سجلٌّ **فارغُ الاسمِ** أو عودةٌ
    #      عارية — وهو ما يجعلُ النداءَ بلا قيمةٍ ولا خطأ.
    vanishes = ('BuildResult("",', "BuildResult();", "return;")
    return any(token in block for token in vanishes)


def _consequence_body(text: str, start: int) -> str:
    """(AR) جسمُ التبعةِ الذي يلي المقارنة: كتلةٌ بين قوسين أو جملةٌ واحدة.

    🔑 التصنيفُ **بالتبعةِ لا بالهجاء**: `if (!argOperands.empty())` يشبهُ
       الرفضَ شكلًا وهو معالجةُ وسيطٍ اختياريّ — وقراءتُه رفضًا اختلقت لمدمجٍ
       عقدَ رتبةٍ كان سيرفضُ نداءَه الصحيح. فلا يُقرأ الشرطُ رفضًا إلّا إن
       **دفعت تبعتُه خطأً**. ولا نفترضُ حدًّا: البحثُ محدودُ المدى صراحةً.
    """
    window = text[start:start + 800]
    brace = window.find("{")
    semi = window.find(";")
    if brace == -1 or (semi != -1 and semi < brace):
        return window[:semi + 1] if semi != -1 else ""
    depth = 0
    for offset in range(brace, len(window)):
        if window[offset] == "{":
            depth += 1
        elif window[offset] == "}":
            depth -= 1
            if depth == 0:
                return window[brace:offset + 1]
    return ""  # (AR) كتلةٌ لم تُغلَق داخلَ المدى ⇒ لا حكم، ولا يُخمَّن


@dataclass
class ArityCheck:
    """(AR) موضعُ فحصِ رتبةٍ واحد."""
    file: str
    line: int
    names: list[tuple[str, str]] = field(default_factory=list)
    literal: tuple[int, int] | None = None
    constant: tuple[str, str] | None = None
    # (AR) ذراعٌ واحدةٌ لاسمَين تنتقي ثابتَ المنادى بشرطٍ ⇒ الثوابتُ الأخرى في
    #      النداءِ نفسِه فرضٌ لأصحابِها، وقراءةُ الأوّلِ وحدَه تجعلُ عقدَهم ميّتًا.
    also_constants: list[tuple[str, str]] = field(default_factory=list)
    # (AR) رفضٌ مكتوبٌ في الشرطِ نفسِه: مفروضٌ فعلًا، بعددٍ خارجَ مصدرِ الحقيقة.
    #      يُفصَل عن `literal` لأنّ ذاك انحدارٌ يجبُ أن يحمرَّ، وهذا دَينٌ مؤرَّخ.
    inline: bool = False
    # (AR) رفضٌ **لا يسجّلُ خطأً**: النداءُ يتبخّرُ والمصرّفُ يخرجُ بصفر. أخطرُ
    #      الأصنافِ لأنّه لا يُخفق فلا يُرى — وقيمتُه تسمّي وجهَ الصمت.
    silent: str | None = None


def _strip_comments(text: str) -> str:
    return _COMMENT.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)


def arity_checks() -> list[ArityCheck]:
    """(AR) كلُّ مواضعِ فحصِ الرتبةِ في الأماميّة، بأسمائِها وصيغةِ مداها."""
    sites: list[ArityCheck] = []
    for path in sorted(BUILDERS.glob("*.cpp")):
        text = _strip_comments(path.read_text(encoding="utf-8"))
        # (AR) خريطةُ الأسماءِ المستعارةِ لهذا الملفِّ: المستعارُ ⇒ فضاءٌ ثابتٌ
        #      (`Bm ⇒ Math`) أو None حين يكون جذرًا (`Bn` فالفضاءُ في المقارنة).
        aliases = {m.group(1): m.group(2) for m in _ALIAS_DECL.finditer(text)}
        events: list[tuple[int, str, object, object]] = []
        for m in _NAME_COMPARE.finditer(text):
            head, second, third = m.group(1), m.group(2), m.group(3)
            if head not in aliases:
                continue
            fixed = aliases[head]
            if fixed is None:
                if third is None:      # `Bn::ID` بلا فضاءٍ ليس مدمجًا مسمًّى
                    continue
                namespace, ident = second, third
            else:
                if third is not None:  # `Bm::NS::ID` لا معنى له
                    continue
                namespace, ident = fixed, second
            events.append((m.start(), "name", namespace, ident))
        for m in _LITERAL_CALL.finditer(text):
            # (AR) الطرفُ الثاني قد يكونَ نداءً (`argResults.size()`) لا رقمًا —
            #      والحكمُ لا يتعلّقُ به: ظهورُ رقمٍ حرفيٍّ واحدٍ في موضعِ المدى
            #      كافٍ ليكونَ العددُ مكتوبًا في السطرِ لا مقروءًا من مصدرِ الحقيقة.
            lo = int(m.group(1))
            events.append((m.start(), "literal", lo,
                           int(m.group(2)) if m.group(2) else lo))
        for m in _CONSTANT_CALL.finditer(text):
            _ = m  # (AR) القراءةُ الفعليّةُ أدناه على وسائطِ النداءِ كلِّها
            # (AR) قد تحملُ وسائطُ النداءِ الواحدِ ثابتَين بشرطٍ ثلاثيٍّ لاسمَين
            #      اثنين؛ فالقراءةُ لأوّلِهما وحدَه تجعلُ عقدَ الثاني ميّتًا وهو
            #      محروسٌ باسمِه. تُقرَأ وسائطُ النداءِ كلُّها إلى فاصلتِه المنقوطة.
            for c in _CONST_IN_SELECTION.finditer(
                    text[m.start():text.find(";", m.end()) + 1 or m.end()]):
                ns, ident = _const_pair(c)
                events.append((m.start(), "constant", ns, ident))
        for m in _SELECTED_CONST.finditer(text):
            # (AR) لا يُحتسَب الانتقاءُ فرضًا إلّا إن مُرِّرَ المرجعُ فعلًا إلى
            #      حارسٍ بعدَه — رابطٌ لا يُقرأ ليس حراسة.
            after = text[m.end():m.end() + 600]
            if not re.search(_SELECTED_USE.pattern.format(re.escape(m.group(1))),
                             after):
                continue
            for c in _CONST_IN_SELECTION.finditer(m.group(2)):
                ns, ident = _const_pair(c)
                events.append((m.start(), "constant", ns, ident))
        for m in _INLINE_COMPARE.finditer(text):
            blk = _consequence_body(text, m.end())
            if "errors_.push_back" in blk or "errors_.emplace_back" in blk:
                events.append((m.start(), "inline", int(m.group(2)), None))
                continue
            # (AR) 🔑 التبعةُ تعودُ بلا تسجيلِ خطأ ⇒ النداءُ **يتبخّرُ صامتًا**
            #      والمصرّفُ يخرجُ بصفرٍ ويُنتجُ ثنائيًّا بلا العمل. وهذا هو
            #      صنفُ ح٤ عينُه: سطرٌ «يُنفَّذ» وهو غيرُ موجود. والكتابةُ إلى
            #      `std::cerr` لا تُنجّي: لا رمزَ خروجٍ يحملُها ولا أداةَ تقرؤها.
            if _is_silent_rejection(m.group(1), blk):
                events.append((m.start(), "silent", int(m.group(2)),
                               "cerr" if "std::cerr" in blk else "صامت"))
        for m in _EMPTY_COMPARE.finditer(text):
            # (AR) `!empty()` معالجةُ وسيطٍ اختياريٍّ لا رفضًا — والنفيُ يسبقُ
            #      الاسمَ مباشرةً، فيُقرَأ من الحرفِ الذي قبلَه.
            if text[max(0, m.start() - 1):m.start()].strip().endswith("!"):
                continue
            blk = _consequence_body(text, m.end())
            if "errors_.push_back" in blk or "errors_.emplace_back" in blk:
                events.append((m.start(), "inline", 1, None))
                continue
            if _is_silent_rejection("<", blk):
                events.append((m.start(), "silent", 1,
                               "cerr" if "std::cerr" in blk else "صامت (empty)"))
        # (AR) الموضعُ الواحدُ قد يطابقُ الصيغتين لو تداخلت التعابير؛ الأسبقُ
        #      بدايةً يُحتسَب مرّةً واحدةً بحسبِ موضعِه.
        # (AR) مدخلُ الجدولِ موضعُ فرضٍ قائمٌ بذاتِه: اسمُه ومداه في المدخلِ نفسِه،
        #      فلا يُعلَّق ولا يُلغى بحدِّ ذراع. ويُسجَّل قبل الفرزِ ليأخذَ موضعَه.
        for m in _TABLE_ROW.finditer(text):
            sites.append(ArityCheck(file=str(path.relative_to(ROOT)),
                                    line=text[:m.start()].count("\n") + 1,
                                    names=[(m.group(1), m.group(2))],
                                    constant=(m.group(3), m.group(4))))
        extra_constants: dict[int, list[tuple[str, str]]] = {}
        for pos, kind, a, b in events:
            if kind == "constant":
                extra_constants.setdefault(pos, []).append((str(a), str(b)))
        events.sort(key=lambda e: (e[0], e[1] != "name"))
        pending: list[tuple[str, str]] = []
        seen_positions: set[int] = set()
        # (AR) 🔑 المقارنةُ المعلَّقةُ تُلغى عند حدِّ الذراع. ذراعٌ بلا فحصِ رتبةٍ
        #      كانت تتركُ اسمَها معلَّقًا فيلتصقُ بفحصِ الذراعِ التالية، فيُقرأ
        #      مدمجٌ لا رتبةَ له مفروضَ الرتبةِ برتبةِ جارِه. والحدُّ يُقاس: نصٌّ
        #      بين مقارنتين فيه `{` أو `;` يعني ذراعًا جديدةً لا `||` في شرطٍ واحد.
        last_name_end = 0
        for pos, kind, a, b in events:
            if kind == "name":
                between = text[last_name_end:pos]
                if pending and ("{" in between or ";" in between):
                    pending = []
                pending.append((str(a), str(b)))
                last_name_end = pos
                continue
            if pos in seen_positions:
                continue
            seen_positions.add(pos)
            site = ArityCheck(file=str(path.relative_to(ROOT)),
                              line=text[:pos].count("\n") + 1,
                              names=list(pending))
            if kind == "silent":
                # (AR) كالرفضِ في الشرط: لا يستهلكُ المقارنةَ المعلَّقة.
                site.silent = str(b)
                sites.append(site)
                continue
            if kind == "inline":
                # (AR) الرفضُ في الشرطِ **لا يستهلكُ** المقارنةَ المعلَّقة: الذراعُ
                #      قد تجمعُه وفحصًا بثابتٍ مُولَّد، فلو ابتلع الاسمَ لقُرئ
                #      المدمجُ غيرَ مفروضٍ وهو مفروضٌ — أي لأحمرَّ الحارسُ على
                #      موضعٍ سليمٍ بسببِ أداةِ القياسِ وحدَها.
                site.inline = True
                sites.append(site)
                continue
            if kind == "literal":
                site.literal = (int(a), int(b))
            else:
                site.constant = (str(a), str(b))
                site.also_constants = [c for c in extra_constants.get(pos, [])
                                       if c != site.constant]
            sites.append(site)
            pending = []
    return sites


def enforced_names() -> set[tuple[str, str]]:
    """(AR) كلُّ مدمجٍ تفرضُ الأماميّةُ رتبتَه **من مصدرِ الحقيقة**.

    الرفضُ في الشرطِ مستثنًى قصدًا: هو فرضٌ قائمٌ لكنّ عددَه محلّيّ، فعدُّه
    ههنا يُقرأ ختمًا وليس بختم. يُقاس على حدةٍ بـ`inline_names()`.
    """
    guarded: set[tuple[str, str]] = set()
    for site in arity_checks():
        if not site.inline and not site.silent:
            guarded.update(site.also_constants)
            if site.constant:
                guarded.add(site.constant)
            guarded.update(site.names)
    return guarded


def inline_names() -> set[tuple[str, str]]:
    """(AR) مدمجاتٌ رتبتُها مفروضةٌ برقمٍ في الشرطِ لا من مصدرِ الحقيقة."""
    named: set[tuple[str, str]] = set()
    for site in arity_checks():
        if site.inline:
            named.update(site.names)
    return named


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sites = arity_checks()
    literal = [s for s in sites if s.literal]
    print(f"مواضعُ فحصِ رتبة: {len(sites)} · بثابتٍ مُولَّد: "
          f"{len(sites) - len(literal)} · برقمٍ حرفيّ: {len(literal)}")
    print(f"مدمجاتٌ محروسةُ الرتبة: {len(enforced_names())}")
