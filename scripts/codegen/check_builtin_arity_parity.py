#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «حقلُ `arity` في مصدرِ الحقيقةِ يبلغُ المحرّكَين لا واحدًا».

     🔑 العلّةُ المقيسة: `language-truth/builtins/*.yaml` يُعلِنُ لكلِّ مدمَجٍ
        `arity`، ويُولَّدُ منه `shared/builtins/generated/builtin_arity_generated.h`.
        **يستهلكُه المترجّمُ في ٢١ ملفًّا** (عبر `shared/builtins/include/
        builtin_arity.h`)، و**لا مرجعَ له في `interpreter/` البتّة** — مقابلَ
        ٩٥٩ موضعَ تسجيلٍ بلا رتبةٍ فيه و١٤ برتبة.

        والسببُ في التوقيع: `FunctionManager::registerBuiltinFunction(name, func)`
        (`interpreter/include/managers/function_manager.h:440`) **لا يأخذُ رتبةً
        أصلًا**، بخلاف `defineBuiltInFunction(name, params, impl)`. فالرتبةُ
        المُعلَنةُ لا تصلُ إلى المفسّر، ويفحصُ كلُّ مدمَجٍ عددَ وسائطِه بيدِه —
        أو لا يفحص.

     ¶ ما يقيسُه هذا الحارسُ ولا يدّعي غيرَه

       عددَ مواضعِ التسجيلِ **بلا رتبة** في `interpreter/src/`. وهذا سؤالٌ
       بنيويٌّ لا اجتهاديّ: التوقيعُ ذو المعاملَين لا يستطيعُ حملَ رتبةٍ مهما
       كان الجسد. فليس هذا مسحًا يستنتجُ «هل يعملُ» — بل عدُّ استعمالاتِ
       دالّةٍ لا تقبلُ الرتبةَ في توقيعِها.

     ⚠️ الحدُّ المُعلَن: لا يقيسُ **سلوكَ** المفسّرِ عند رتبةٍ خاطئة. قِيس
        تشغيليًّا أنّ السلوكَ غيرُ منتظم: أسماءٌ معروفةٌ تُعطي «Too many
        arguments» (صحيح)، وأسماءٌ معروفةٌ تُعطي «غير معرفة» كالمخترَع (نفيٌ
        كاذب)، وأسماءٌ **تُنفَّذُ فعلًا** برتبةٍ زائدة. لكنّ قياسَ ذلك يستلزمُ
        تنفيذَ مدمَجاتٍ على جهازِ المستعمِل، وقد رُفض ذلك في المِجَسِّ لسببِه
        (`probe_builtin_engines.py::calibrate_interpreter`) فلا يُستأنَفُ هنا.

     السقفُ **ينحدرُ ولا يرتفع**: كلُّ تحويلٍ من `registerBuiltinFunction` إلى
     توقيعٍ يحملُ الرتبةَ يُنقِصُه. ورفعُه قرارٌ يُكتَبُ هنا صراحةً ويُراجَع.

(EN) The SoT `arity` field reaches only the compiler: 21 files consume the
     generated table, `interpreter/` none. Guarded by a descending ceiling on
     arity-less registrations, since the 2-arg signature cannot carry one.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

for _stream in (sys.stdout, sys.stderr):
    if hasattr(_stream, "reconfigure"):
        _stream.reconfigure(encoding="utf-8")

ROOT = Path(__file__).resolve().parents[2]
INTERP_SRC = ROOT / "interpreter" / "src"
COMPILER_SRC = ROOT / "compiler" / "src"
ARITY_HEADER = ROOT / "shared" / "builtins" / "include" / "builtin_arity.h"

# (AR) خطُّ الأساسِ المقيسُ يومَ إدخالِ الحارس — بعدَ إسقاطِ التعليقاتِ
#      والسلاسل. ينحدرُ ولا يرتفع. (والعدُّ الخامُّ بـgrep يقولُ ١٠٠١ لأنّه
#      يعُدُّ ذِكرَ الاسمِ في التعليقاتِ أيضًا؛ الرقمُ البنيويُّ هو المُعتمَد.)
ARITYLESS_BASELINE = 959
# (AR) وأرضيّةٌ للطرفِ الآخرِ: المترجّمُ يستهلكُ الجدولَ فعلًا. لو سقطت إلى صفرٍ
#      لَصار الحارسُ «أخضرَ» بلا أيِّ استهلاكٍ في المحرّكَين — أسوأُ من الحال.
COMPILER_CONSUMERS_FLOOR = 15

REGISTER_ARITYLESS = re.compile(r"\bregisterBuiltinFunction\s*\(")
REGISTER_WITH_ARITY = re.compile(r"\bdefineBuiltInFunction\s*\(")
ARITY_USE = re.compile(r"builtin_arity|BUILTIN_ARITY|builtinArity")


def strip_comments(text: str) -> str:
    """يُسقِطُ التعليقاتِ وحدَها: ذِكرُ اسمٍ في تعليقٍ ليس موضعَ تسجيل."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return re.sub(r"//[^\n]*", " ", text)


def strip_comments_and_strings(text: str) -> str:
    """🔑 ويُسقِطُ السلاسلَ أيضًا — للعدِّ فقط، لا للبحثِ عن إدراج.

    أوّلُ نسخةٍ من هذا الحارسِ أسقطت السلاسلَ في الحالتَين، فمحت مسارَ
    `#include "builtin_arity.h"` نفسَه فقالت «يستهلكُه المترجّمُ في ٠ ملفًّا»
    والواقعُ ٢١. أداةُ القياسِ تُفسِدُ قياسَها حين تُنظّفُ ما تبحثُ عنه —
    وهو الخطأُ نفسُه الذي كان في تمهيدِ المِجَسّ، بثوبٍ آخر.
    """
    return re.sub(r'"(?:[^"\\]|\\.)*"', '""', strip_comments(text))


def _sources(tree: Path):
    return sorted(tree.rglob("*.cpp")) + sorted(tree.rglob("*.h"))


def count_in(tree: Path, pattern: re.Pattern) -> int:
    return sum(len(pattern.findall(strip_comments_and_strings(
        path.read_text(encoding="utf-8", errors="replace"))))
        for path in _sources(tree))


def files_using(tree: Path, pattern: re.Pattern) -> list:
    return [path.relative_to(ROOT).as_posix() for path in _sources(tree)
            if pattern.search(strip_comments(
                path.read_text(encoding="utf-8", errors="replace")))]


def main() -> int:
    if not ARITY_HEADER.exists():
        print(f"✗ جدولُ الرتبِ غير موجود: {ARITY_HEADER}")
        print("  حارسٌ بلا مرجعٍ لا يحرس.")
        return 1

    arityless = count_in(INTERP_SRC, REGISTER_ARITYLESS)
    with_arity = count_in(INTERP_SRC, REGISTER_WITH_ARITY)
    comp_users = files_using(COMPILER_SRC, ARITY_USE)
    interp_users = files_using(INTERP_SRC, ARITY_USE)

    print("حارس «حقلُ arity يبلغُ المحرّكَين»:")
    print(f"  الجدولُ المولَّد: {ARITY_HEADER.relative_to(ROOT)}")
    print(f"  يستهلكُه المترجّم: {len(comp_users)} ملفًّا · "
          f"المفسّر: {len(interp_users)} ملفًّا")
    print(f"  تسجيلٌ بلا رتبة (registerBuiltinFunction): {arityless} "
          f"(خطُّ الأساس {ARITYLESS_BASELINE})")
    print(f"  تسجيلٌ برتبة (defineBuiltInFunction): {with_arity}")

    failed = False
    if arityless > ARITYLESS_BASELINE:
        print(f"  ✗ ازدادَ التسجيلُ بلا رتبةٍ {arityless - ARITYLESS_BASELINE} "
              "موضعًا — السقفُ ينحدرُ ولا يرتفع.")
        failed = True
    if len(comp_users) < COMPILER_CONSUMERS_FLOOR:
        print(f"  ✗ استهلاكُ المترجّمِ للجدولِ سقط إلى {len(comp_users)} ملفًّا "
              f"(الأرضيّةُ {COMPILER_CONSUMERS_FLOOR}) — لا يُسَدُّ التفاوتُ "
              "بتعطيلِ الطرفِ العامل.")
        failed = True

    if failed:
        print("\n  العلاج: حوّل التسجيلَ إلى توقيعٍ يحملُ الرتبةَ من الجدولِ")
        print("  المولَّد، أو حدّث خطَّ الأساسِ عمدًا في هذا الملفّ.")
        return 1

    if interp_users:
        print(f"  ✓ المفسّرُ صار يقرأُ الجدولَ في {len(interp_users)} ملفًّا — "
              "أنقِص خطَّ الأساسِ وثبّت المكسب.")
    elif arityless < ARITYLESS_BASELINE:
        print(f"  ✓ انحدرَ الدَّينُ {ARITYLESS_BASELINE - arityless} موضعًا — "
              "أنزِل خطَّ الأساسِ في هذا الملفّ ليثبت.")
    else:
        print("  ✓ لا انحدار — الدَّينُ ثابتٌ عند خطِّ الأساسِ المُعلَن.")
        print("    ⚠️ دَينٌ قائم: الرتبةُ المُعلَنةُ في SoT لا تبلغُ المفسّرَ، "
              "فحارسُ الرتبةِ فيه غيرُ منتظمٍ بخلافِ المترجّم.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
