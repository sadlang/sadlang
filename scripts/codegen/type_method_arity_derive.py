#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُشتَقُّ رتبةِ **طرائقِ الأنواع** من أذرعِ المفسّر.

     `type_methods.yaml` سطحٌ كان بلا حقلِ `arity` البتّة — ١١٤ طريقةً رتبتُها
     رقمٌ محلّيٌّ في ذراعٍ لا يقيسُه أحد، وقِيس تبخّرُ `خ.عين("ك")` صامتًا.
     ولا تُخترَعُ الرتبةُ بذرًا يدويًّا: تُشتَقُّ بالقاعدةِ نفسِها التي أعطت
     رتبَ المدمجاتِ في `builtin_arity_derive.py` — لكنْ من مُقيِّمِ الطرائق.

     🔑 و`args` ههنا **لا تحوي المستقبِل** أصلًا (المستقبِلُ `str`/`arr` متغيّرٌ
        مستقلّ)، فالعدُّ المُشتَقُّ هو عدُّ الوسائطِ عينُه — وهو الاصطلاحُ
        المنصوصُ عليه في المخطَّط. أمّا متّجهُ المترجّمِ فيُدرجُ الموضوعَ أوّلًا
        ويطرحُه عند الفحص؛ فالفارقُ في المستهلكِ لا في العقد.

     والأساسُ ثلاثةٌ لا رابعَ لها، ومن لا أساسَ له **يُترَك بلا عقدٍ ويُعَدّ**:
       • شرطُ نقصٍ (`args.empty()` / `args.size() < N`) ⇒ الأدنى منه.
       • فهرسٌ غيرُ مشروطٍ (`args[k]`) ⇒ الأدنى `k+1` إن لم يكن ثَمَّ شرط.
       • فهرسٌ خلفَ `args.empty() ? … : args[k]` ⇒ **اختياريٌّ**: يرفعُ الأعلى
         وحدَه. ومن خلطَه بالإلزامِ اخترعَ عقدًا أضيقَ ممّا يقبلُه المحرّك.
(EN) Derive type-method arity from the interpreter's own argument use, with
     the same rules as builtin_arity_derive.py. `args` here already excludes
     the receiver. No basis -> no contract, counted and reported.
============================================================================
"""
from __future__ import annotations

import re
import sys
from dataclasses import dataclass
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
EVAL = ROOT / "interpreter" / "src" / "visitors"
TYPE_METHODS = ROOT / "language-truth" / "type_methods.yaml"

_EVAL_FILES = (
    "expression_evaluator_oop_array_methods.cpp",
    "expression_evaluator_oop_string_map_methods.cpp",
    "expression_evaluator_oop_concurrency.cpp",
)

# (AR) رأسُ الذراع: `if (m == TM::String::CONTAINS)` — الوسمُ هو المرساة،
#      لا هجاءُ الاسمِ العربيِّ ولا التعليقُ فوقَه (درسُ «مرساةُ الحارسِ الفعل»).
_ARM = re.compile(r"\bm\s*==\s*TM::(\w+)::(\w+)\b")
_INDEX = re.compile(r"\bargs\s*\[\s*(\d+)\s*\]")
_SIZE_GUARD = re.compile(r"\bargs\s*\.\s*size\(\)\s*<\s*(\d+)\b")
_SIZE_EQ = re.compile(r"\bargs\s*\.\s*size\(\)\s*(?:!=|==)\s*(\d+)\b")
_EMPTY_GUARD = re.compile(r"(?<![!\w])args\s*\.\s*empty\(\)")
# (AR) 🔑 يُصنَّفُ **بالتبعةِ لا بالشكل**: `if (!args.empty() && args[0]…)`
#      تخييرٌ لا رفض، و`args.empty()` وحدَها لا تعني شيئًا حتّى يُنظَرَ ما
#      يفعلُه جسمُها. قِيس: `رتب` خرجت `min=1` وهي تقبلُ النداءَ بلا وسيط.
_OPTIONAL = re.compile(
    r"!\s*args\s*\.\s*empty\(\)"
    r"|\bargs\s*\.\s*empty\(\)\s*\?"
    r"|\bargs\s*\.\s*size\(\)\s*>=?\s*\d+"
    # (AR) `args.size() < 2 ? افتراضٌ : args[1]` مدًى لا رفض — الفهرسُ اختياريّ.
    r"|\bargs\s*\.\s*size\(\)\s*<\s*\d+[^;]*\?")
# (AR) فهرسٌ بمتغيّرٍ أو مرورٌ على `args` كلِّها ⇒ **رتبةٌ مفتوحة**، لا «صفرُ
#      وسائط». قِيس: `عنصر` تفهرسُ `args[i]` في حلقةٍ و`أضف` تدفعُ الوسائطَ
#      جميعًا؛ فقراءةُ «لا فهرسَ حرفيًّا» صفرًا تخترعُ عقدًا يمنعُ نداءً صحيحًا.
_VARIADIC = re.compile(r"\bargs\s*\[\s*[A-Za-z_]"
                       r"|for\s*\([^)]*:\s*args\b"
                       r"|\bargs\s*\.\s*(?:begin|end|cbegin)\(\)")
_REJECTS = re.compile(r"\bthrowRuntime\b|\bctx\s*\.\s*error\b|\breturn\b"
                      r"|\berror\s*\(|\bthrow\b")


def _is_rejection(body: str, at: int) -> bool:
    """(AR) الشرطُ حارسُ نقصٍ متى كانت تبعتُه رفضًا — رميًا أو عودةً مبكّرة."""
    return bool(_REJECTS.search(body[at:at + 240]))


@dataclass(frozen=True)
class Derived:
    target: str
    cpp_id: str
    minimum: int
    maximum: int
    basis: str
    line: int
    variadic: bool = False


def _arm_body(text: str, start: int) -> str:
    """(AR) جسمُ الذراعِ من قوسِها المفتوحِ إلى نظيرِه — لا إلى الذراعِ التالية:
       ذراعٌ تُغلَقُ قبلَ أختِها بأسطر، فالقصُّ على «التالية» يبتلعُ شيفرةً
       ليست منها فيرفعُ رتبةً بفهرسٍ غريب."""
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
    return text[brace:]


def derive() -> tuple[dict[tuple[str, str], Derived], list[tuple[str, str]]]:
    found: dict[tuple[str, str], Derived] = {}
    baseless: list[tuple[str, str]] = []
    for name in _EVAL_FILES:
        path = EVAL / name
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8")
        for mark in _ARM.finditer(text):
            target, cpp_id = mark.group(1), mark.group(2)
            body = _arm_body(text, mark.end())
            required: list[int] = []
            indices: list[int] = []
            # (AR) الاختياريّةُ تسري على **الكتلةِ** لا على السطرِ وحدَه:
            #      `if (!args.empty() …)` ثمّ `args[0]` في السطرِ التالي فهرسٌ
            #      محروسٌ لا إلزام. القياسُ بالسطرِ أعطى `رتب` رتبةً دنياها ١
            #      وهي تقبلُ النداءَ بلا وسيطٍ أصلًا.
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
                        pending = True  # (AR) جسمٌ بسطرٍ واحدٍ بلا قوسين
                elif pending:
                    # (AR) القوسُ في سطرٍ مستقلٍّ (طرازُ أولمان) — الكتلةُ هي
                    #      جسمُ الشرطِ الاختياريِّ لا سطرٌ واحدٌ بعدَه.
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
            guards += [1 for m in _EMPTY_GUARD.finditer(body)
                       if _is_rejection(body, m.end())]
            key = (target, cpp_id)
            open_ended = bool(_VARIADIC.search(body))
            if not indices and not guards and not open_ended:
                # (AR) طريقةٌ بلا وسائطَ البتّة ⇒ رتبةٌ صفرٌ **مقيسة**، لا «مجهولة».
                found[key] = Derived(target, cpp_id, 0, 0, "لا وسائطَ في الجسم",
                                     text.count("\n", 0, mark.start()) + 1)
                continue
            if open_ended:
                minimum = min(guards) if guards else (
                    max(required) + 1 if required else 0)
                found[key] = Derived(
                    target, cpp_id, minimum, minimum,
                    "مرورٌ على الوسائطِ كلِّها ⇒ رتبةٌ مفتوحة",
                    text.count("\n", 0, mark.start()) + 1, variadic=True)
                continue
            maximum = max(indices) + 1 if indices else max(guards)
            if guards:
                minimum = min(guards)
            elif required:
                minimum = max(required) + 1
            else:
                minimum = 0
            maximum = max(maximum, minimum)
            prior = found.get(key)
            if prior:  # (AR) ذراعان لطريقةٍ واحدةٍ (نصٌّ ومصفوفة) ⇒ يُوسَّعُ المدى.
                minimum = min(minimum, prior.minimum)
                maximum = max(maximum, prior.maximum)
            found[key] = Derived(
                target, cpp_id, minimum, maximum,
                "شرطُ رفضٍ + فهرس" if guards and indices
                else ("شرطُ رفضٍ" if guards else "فهرسٌ بلا شرط"),
                text.count("\n", 0, mark.start()) + 1)
    return found, baseless


def _method_cpp_id(method: dict, used: dict[str, int]) -> str:
    """(AR) نسخةٌ طِبقُ الأصلِ ممّا في `gen_builtins_arity.py` — لو تباعدَ
       الاشتقاقان سمّى كلُّ مولّدٍ الشيءَ باسمٍ وهو انحرافٌ لا يحمرّ."""
    base = re.sub(r"[^a-zA-Z0-9]", "_",
                  str(method.get("method_en", ""))).upper().strip("_")
    if not base or base[0].isdigit() or not any(c.isalpha() for c in base):
        base = "METHOD"
    if base not in used:
        used[base] = 1
        return base
    count = used[base]
    used[base] = count + 1
    return f"{base}_{count}"


_TARGET_LABEL = {"ARRAY": "Array", "STRING": "String", "MAP": "Map",
                 "CHANNEL": "Channel", "ANY": "Any"}


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    found, _ = derive()
    doc = yaml.safe_load(TYPE_METHODS.read_text(encoding="utf-8")) or {}
    by_target: dict[str, list[dict]] = {}
    for method in doc.get("methods") or []:
        by_target.setdefault(method.get("target_type", "ANY"), []).append(method)

    matched, unmatched, conflict = [], [], []
    for target, methods in by_target.items():
        used: dict[str, int] = {}
        label = _TARGET_LABEL.get(target, target)
        for method in methods:
            cpp_id = _method_cpp_id(method, used)
            d = found.get((label, cpp_id))
            if not d:
                unmatched.append(f"{label}::{cpp_id} ({method['method']})")
                continue
            declared = method.get("arity")
            same_top = (bool(declared.get("variadic")) if declared else False) \
                if d.variadic else (declared or {}).get("max") == d.maximum
            if declared and (declared.get("min") != d.minimum or not same_top):
                conflict.append(
                    f"{label}::{cpp_id}: مُعلَنٌ "
                    f"({declared.get('min')},{declared.get('max')}) ≠ مُشتَقٌّ "
                    f"({d.minimum},{d.maximum})")
            matched.append((label, cpp_id, d, method["method"]))

    print(f"أذرعٌ مقروءةٌ في المفسّر: {len(found)}")
    print(f"طريقةٌ لها أساسٌ يُشتَقُّ منه: {len(matched)}")
    print(f"طريقةٌ بلا ذراعٍ تُقرَأ: {len(unmatched)} — لا يُخترَعُ لها عقد")
    if conflict:
        print(f"خلافٌ بين المُعلَنِ والمُشتَقّ: {len(conflict)}")
        for c in conflict:
            print(f"  ⚠ {c}")
    if "--list" in sys.argv:
        for label, cpp_id, d, canonical in sorted(matched):
            print(f"  {label}::{cpp_id} ({canonical}): "
                  f"min={d.minimum} " + ("max=∞" if d.variadic else f"max={d.maximum}") + f" ← {d.basis}")
        for u in sorted(unmatched):
            print(f"  — {u}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
