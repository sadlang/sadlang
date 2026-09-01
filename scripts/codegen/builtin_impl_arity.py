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

       ⚠️ حدُّه مُعلَن: لا يبلغُ ما سُجِّل بماكرو (`MAKE_WIDGET_*`) ولا
          `defineBuiltInFunction` ولا اللامدات ذات الأشكالِ الأخرى. فما لم
          يُقَس **لا يُقصى** — الإقصاءُ على المُلاحَظِ وحدَه، وهو فشلٌ آمِنٌ:
          أسوأُ ما يقع أن تُحرَسَ أسماءٌ أقلّ، لا أن تُرفَضَ نداءاتٌ صحيحة.

(EN) Statically measures the argument count the interpreter implementation
     actually accepts, so SoT-declared arity is enforced only where the two
     agree. No execution: reading source, not running builtins.
============================================================================
"""
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INTERP_SRC = ROOT / "interpreter" / "src"
REGISTRY = ROOT / "shared" / "builtins" / "generated" / "builtin_registry_generated.h"

_CONST_RE = re.compile(
    r'inline\s+constexpr\s+(?:std::)?string_view\s+([A-Z][A-Z0-9_]*)\s*=\s*"([^"]+)"')
_REGISTER_RE = re.compile(
    r'registerBuiltinFunction\(\s*std::string\(\s*(?:[A-Za-z_][\w:]*::)?'
    r'([A-Z][A-Z0-9_]*)\s*\)\s*,\s*([A-Za-z_]\w*)\s*\)')
_LAMBDA_RE = re.compile(r'\bauto\s+(\w+)\s*=\s*\[')
_INDEX_RE = re.compile(r'\bargs\[\s*(\d+)\s*\]')
_SIZE_GE_RE = re.compile(r'\bargs\.size\(\)\s*(<|>=)\s*(\d+)')
_SIZE_LT_RE = re.compile(r'\b(\d+)\s*(>|<=)\s*args\.size\(\)')


def _const_names() -> dict:
    """{المعرّف: الاسمُ العربيّ} من سِجِلِّ المدمَجاتِ المولَّد."""
    text = REGISTRY.read_text(encoding="utf-8", errors="replace")
    return {m.group(1): m.group(2) for m in _CONST_RE.finditer(text)}


def _lambda_body(text: str, start: int) -> str:
    """جسمُ اللامدا من `[` حتّى إغلاقِ `}` الموازن — لا بتعبيرٍ نمطيٍّ جشِع."""
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


def observed_ranges() -> dict:
    """{الاسمُ العربيّ: (أدنى مُلاحَظ أو None، أعلى مُلاحَظ أو None)}."""
    names = _const_names()
    found: dict = {}
    for path in sorted(INTERP_SRC.rglob("*.cpp")):
        text = path.read_text(encoding="utf-8", errors="replace")
        bodies = {m.group(1): _lambda_body(text, m.end())
                  for m in _LAMBDA_RE.finditer(text)}
        for m in _REGISTER_RE.finditer(text):
            arabic = names.get(m.group(1))
            body = bodies.get(m.group(2))
            if not arabic or not body:
                continue
            indices = [int(i) for i in _INDEX_RE.findall(body)]
            top = max(indices) + 1 if indices else None
            lows = [int(n) for op, n in
                    ((mm.group(1), mm.group(2)) for mm in _SIZE_GE_RE.finditer(body))]
            lows += [int(n) for n, op in
                     ((mm.group(1), mm.group(2)) for mm in _SIZE_LT_RE.finditer(body))]
            low = max(lows) if lows else None
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
        if hi is not None and seen_hi is not None and seen_hi > hi:
            out.append((name, lo, hi, seen_lo, seen_hi,
                        "التنفيذُ يقرأُ وسيطًا فوقَ الأعلى المُعلَن"))
        elif hi is not None and seen_lo is not None and seen_lo > hi:
            out.append((name, lo, hi, seen_lo, seen_hi,
                        "التنفيذُ يشترطُ أكثرَ من الأعلى المُعلَن — غيرُ قابلٍ للنداء"))
    return out
