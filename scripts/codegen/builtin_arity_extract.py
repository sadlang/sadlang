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
_CONSTANT_CALL = re.compile(
    _ARITY_HELPER + r"\([^;]*?(?:Ar|Sad::Builtins::Arity)::(\w+)::(\w+)")
# (AR) صيغةٌ ثانيةٌ للفرض: **جدولٌ** يقرن الاسمَ بالأوپكودِ وبالمدى في مدخلٍ واحد
#      (`{Bn::UICore::GEN_WEB, …, Ar::UICore::GEN_WEB}`) ثمّ تدورُ عليه حلقةٌ
#      تفحصُ رتبةَ ما طابق. لا `funcName ==` فيها، فكانت أربعةُ مدمجاتٍ تُقرأ
#      **مُعلَنةً غيرَ مفروضة** وهي مفروضةٌ فعلًا — مسحٌ ميكانيكيٌّ يفوته الشكلُ
#      الثاني للصيغةِ نفسِها. والقرانُ ههنا صريحٌ في سطرٍ واحد فيُقرأ مباشرةً.
_TABLE_ROW = re.compile(
    r"\{\s*\w+::(\w+)::(\w+)\s*,[^{}]*?(?:Ar|Sad::Builtins::Arity)::(\w+)::(\w+)\s*\}")


@dataclass
class ArityCheck:
    """(AR) موضعُ فحصِ رتبةٍ واحد."""
    file: str
    line: int
    names: list[tuple[str, str]] = field(default_factory=list)
    literal: tuple[int, int] | None = None
    constant: tuple[str, str] | None = None


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
            events.append((m.start(), "constant", m.group(1), m.group(2)))
        # (AR) الموضعُ الواحدُ قد يطابقُ الصيغتين لو تداخلت التعابير؛ الأسبقُ
        #      بدايةً يُحتسَب مرّةً واحدةً بحسبِ موضعِه.
        # (AR) مدخلُ الجدولِ موضعُ فرضٍ قائمٌ بذاتِه: اسمُه ومداه في المدخلِ نفسِه،
        #      فلا يُعلَّق ولا يُلغى بحدِّ ذراع. ويُسجَّل قبل الفرزِ ليأخذَ موضعَه.
        for m in _TABLE_ROW.finditer(text):
            sites.append(ArityCheck(file=str(path.relative_to(ROOT)),
                                    line=text[:m.start()].count("\n") + 1,
                                    names=[(m.group(1), m.group(2))],
                                    constant=(m.group(3), m.group(4))))
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
            if kind == "literal":
                site.literal = (int(a), int(b))
            else:
                site.constant = (str(a), str(b))
            sites.append(site)
            pending = []
    return sites


def enforced_names() -> set[tuple[str, str]]:
    """(AR) كلُّ مدمجٍ تفرضُ الأماميّةُ رتبتَه (بأيِّ صيغة)."""
    guarded: set[tuple[str, str]] = set()
    for site in arity_checks():
        guarded.update(site.names)
    return guarded


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sites = arity_checks()
    literal = [s for s in sites if s.literal]
    print(f"مواضعُ فحصِ رتبة: {len(sites)} · بثابتٍ مُولَّد: "
          f"{len(sites) - len(literal)} · برقمٍ حرفيّ: {len(literal)}")
    print(f"مدمجاتٌ محروسةُ الرتبة: {len(enforced_names())}")
