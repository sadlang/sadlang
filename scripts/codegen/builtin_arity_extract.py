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
_NAME_COMPARE = re.compile(r"funcName\s*==\s*Bn::(\w+)::(\w+)")
_LITERAL_CALL = re.compile(
    r"check\w*Arity\(\s*[\w.\->_]+\s*,\s*\w+\s*,\s*(\d+)\s*,\s*(\d+)")
_CONSTANT_CALL = re.compile(
    r"check\w*Arity\([^;]*?(?:Ar|Sad::Builtins::Arity)::(\w+)::(\w+)")


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
        events: list[tuple[int, str, object, object]] = []
        for m in _NAME_COMPARE.finditer(text):
            events.append((m.start(), "name", m.group(1), m.group(2)))
        for m in _LITERAL_CALL.finditer(text):
            events.append((m.start(), "literal", int(m.group(1)), int(m.group(2))))
        for m in _CONSTANT_CALL.finditer(text):
            events.append((m.start(), "constant", m.group(1), m.group(2)))
        # (AR) الموضعُ الواحدُ قد يطابقُ الصيغتين لو تداخلت التعابير؛ الأسبقُ
        #      بدايةً يُحتسَب مرّةً واحدةً بحسبِ موضعِه.
        events.sort(key=lambda e: (e[0], e[1] != "name"))
        pending: list[tuple[str, str]] = []
        seen_positions: set[int] = set()
        for pos, kind, a, b in events:
            if kind == "name":
                pending.append((str(a), str(b)))
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
