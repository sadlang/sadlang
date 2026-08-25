#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُشتَقُّ الرتبةِ من المحرّكِ الآخر.

     الأماميّةُ تُمرِّرُ `argOperands` جملةً إلى بانياتِ الشبكةِ والواجهةِ ولا
     تفهرسُ شيئًا، فلا أساسَ فيها يُشتَقُّ منه عقدٌ — ومن اخترعَ عقدًا هناك
     كتبَ **دعوى** لا قياسًا. لكنّ العقدَ **مقيسٌ في المفسّر**: كلُّ مدمجٍ
     مسجَّلٍ هناك يفهرسُ `args[k]` ويحرسُ `args.size() < N`، وهما — بحكمِ
     تكافؤِ المحرّكَين — عقدُ اللغةِ نفسُه لا اجتهادَ ملفٍّ واحد.

       • **الأدنى**: العددُ في شرطِ الرفضِ (`args.size() < N`) إن وُجِد،
         وإلّا فأعلى فهرسٍ **غيرِ محروسٍ** يُقرَأ بلا شرط ⇒ لا يُخمَّن صفرًا.
       • **الأعلى**: أعلى `args[k]` مقروءٍ في الجسمِ كلِّه + ١.

     وما لا فهرسَ له ولا شرطَ **يُترَك بلا عقد** ويُعلَن عددُه. الأداةُ
     تقيسُ ولا تخترع؛ وإن لم تجدْ أساسًا سكتت عن ذاك المدمجِ صراحةً.
(EN) Derive builtin arity from the interpreter's own argument use, since the
     compiler's network/UI arms pass argOperands wholesale (no basis).
     min = guard constant (or highest unguarded index+1); max = highest index+1.
     No basis -> no contract, counted and reported.
============================================================================
"""
from __future__ import annotations

import re
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INTERP = ROOT / "interpreter" / "src" / "builtins"

_ALIAS = re.compile(
    r"namespace\s+(\w+)\s*=\s*Sad::Builtins::Names::(\w+)\s*;")
_REGISTER = re.compile(
    r"registerBuiltinFunction\(\s*std::string\(\s*(\w+)::(\w+)\s*\)")
# (AR) الفهرسُ الحرفيُّ وحدَه يُقرَأ؛ `args[i]` في حلقةٍ لا يحدُّ رتبةً.
_INDEX = re.compile(r"\bargs\s*\[\s*(\d+)\s*\]")
_SIZE_GUARD = re.compile(r"\bargs\s*\.\s*size\(\)\s*<\s*(\d+)\b")
_SIZE_EQ = re.compile(r"\bargs\s*\.\s*size\(\)\s*(?:!=|==)\s*(\d+)\b")
_EMPTY = re.compile(r"(?<![!\w])args\s*\.\s*empty\(\)")
# (AR) `args.size() > k` قبلَ الفهرسِ **يجعلُه اختياريًّا**: يرفعُ الأعلى ولا
#      يرفعُ الأدنى. ومن خلطَ الاثنين اخترعَ إلزامًا حيث كان تخييرٌ مقيس.
#
# (AR) 🔑 وأُلحِقت بها ثلاثُ صيغٍ بعدَ أن قِيس أثرُ إغفالِها: `ذعر` بُذِرت
#      `min: 1` وذراعُها `if (!args.empty()) message = args[0]` — تخييرٌ محض،
#      فردَّ المصرّفُ `ذعر()` وهو نداءٌ صحيحٌ يقبلُه المحرّكان. أي أنّ عقدًا
#      وُضِع ليمنعَ التبخّرَ صار يمنعُ الصواب. والقاعدةُ: **يُصنَّفُ بالتبعةِ لا
#      بالشكل** — شرطٌ لا تكون تبعتُه رفضًا فهو مدًى لا حارس.
_OPTIONAL = re.compile(
    r"!\s*args\s*\.\s*empty\(\)"
    r"|\bargs\s*\.\s*empty\(\)\s*\?"
    r"|\bargs\s*\.\s*size\(\)\s*>=?\s*\d+"
    r"|\bargs\s*\.\s*size\(\)\s*<\s*\d+[^;]*\?")
_REJECTS = re.compile(r"\bthrowRuntime\b|\bctx\s*\.\s*error\b|\breturn\b"
                      r"|\berror\s*\(|\bthrow\b")


def _is_rejection(body: str, at: int) -> bool:
    """(AR) الشرطُ حارسُ نقصٍ متى كانت تبعتُه رفضًا — رميًا أو عودةً مبكّرة."""
    return bool(_REJECTS.search(body[at:at + 240]))


@dataclass(frozen=True)
class Derived:
    namespace: str
    cpp_id: str
    minimum: int
    maximum: int
    file: str
    line: int
    basis: str


def _aliases(text: str) -> dict[str, str]:
    return {m.group(1): m.group(2) for m in _ALIAS.finditer(text)}


def derive() -> tuple[list[Derived], list[tuple[str, str]]]:
    """(AR) يُعيد المشتقّاتِ، ثمّ المدمجاتِ المسجَّلةَ بلا أساسٍ يُقاس."""
    found: list[Derived] = []
    baseless: list[tuple[str, str]] = []
    for path in sorted(INTERP.glob("*.cpp")):
        text = path.read_text(encoding="utf-8")
        alias = _aliases(text)
        marks = list(_REGISTER.finditer(text))
        for order, mark in enumerate(marks):
            # (AR) جسمُ اللامدا يسبقُ سطرَ التسجيل؛ حدُّه الأعلى تسجيلُ ما قبلَه.
            start = marks[order - 1].end() if order else 0
            body = text[start:mark.start()]
            namespace = alias.get(mark.group(1))
            if not namespace:
                continue
            cpp_id = mark.group(2)
            required: list[int] = []
            indices: list[int] = []
            # (AR) والاختياريّةُ تسري على **الكتلةِ** لا على سطرِ الشرطِ وحدَه.
            depth = 0
            optional_depth: int | None = None
            pending = False
            for line in body.splitlines():
                opened = depth
                inside = optional_depth is not None and depth > optional_depth
                guarded = pending or inside or bool(_OPTIONAL.search(line))
                hits = [int(m.group(1)) for m in _INDEX.finditer(line)]
                depth += line.count("{") - line.count("}")
                if _OPTIONAL.search(line):
                    if line.count("{") > line.count("}"):
                        optional_depth, pending = opened, False
                    else:
                        pending = True
                elif pending:
                    if line.strip().startswith("{"):
                        optional_depth = opened
                    pending = False
                elif optional_depth is not None and depth <= optional_depth:
                    optional_depth = None
                if not hits:
                    continue
                indices += hits
                if not guarded:
                    required += hits
            guards = [int(m.group(1)) for m in _SIZE_GUARD.finditer(body)
                      if _is_rejection(body, m.end())]
            guards += [int(m.group(1)) for m in _SIZE_EQ.finditer(body)
                       if _is_rejection(body, m.end())]
            guards += [1 for m in _EMPTY.finditer(body)
                       if _is_rejection(body, m.end())]
            if not indices and not guards:
                baseless.append((namespace, cpp_id))
                continue
            maximum = max(indices) + 1 if indices else max(guards)
            if guards:
                minimum = min(guards)
            elif required:
                minimum = max(required) + 1
            else:
                # (AR) كلُّ فهرسٍ مشروطٌ بِحجمٍ ولا حارسَ نقصٍ ⇒ الوسائطُ كلُّها
                #      اختياريّةٌ بقيمٍ افتراضيّة: أدنًى صفرٌ **مقيسٌ** لا مُخمَّن،
                #      والحراسةُ تبقى على الأعلى — وهي تصدُّ الزائدَ لا الناقص.
                minimum = 0
            # (AR) شرطٌ أعلى من كلِّ فهرسٍ ⇒ الأعلى هو الشرطُ لا الفهرس.
            maximum = max(maximum, minimum)
            found.append(Derived(
                namespace, cpp_id, minimum, maximum,
                str(path.relative_to(ROOT)).replace("\\", "/"),
                text.count("\n", 0, mark.start()) + 1,
                "شرطُ رفضٍ + فهرس" if guards and indices
                else ("شرطُ رفضٍ" if guards else "فهرسٌ بلا شرط")))
    return found, baseless


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    found, baseless = derive()
    print(f"مُشتَقٌّ من المفسّر: {len(found)} مدمجًا")
    print(f"مسجَّلٌ بلا أساسٍ يُقاس: {len(baseless)} — لا يُخترَع لها عقد")
    for d in found[:int(sys.argv[1]) if len(sys.argv) > 1 else 0]:
        print(f"  {d.namespace}::{d.cpp_id}: min={d.minimum} max={d.maximum}"
              f"  ← {d.basis}  ({d.file}:{d.line})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
