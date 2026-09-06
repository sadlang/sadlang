#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «حجمُ النوعِ يُشتَقُّ ولا يُكتَب».

     قِيس قبل هذا الحارس: `@حجم` كان **جدولَين مكتوبَين باليدِ** بسلاسلَ
     عربيّةٍ خامّة — `if/else if` في المترجّمِ و`unordered_map` ساكنةً في
     المفسّر — فتباعدا في **٦ من ١١** نوعًا (خريطة ٤٨/٢٤ · بايت ٠/٨ ·
     حرف ٠/١ · أي ٧٢/٨ · فراغ ٠/٨ · عدم ٠/٨)، و**افتراضاهما يكذبان**:
     نوعٌ مجهولٌ يُخرِجُ ٨ في المترجّمِ و٠ في المفسّرِ بلا تشخيص.

 🔑  والحارسُ يقيسُ **جسمَ الدالّةِ نفسِه** لا الملفَّ كلَّه: الملفُّ فيه
     تشخيصاتٌ عربيّةٌ مشروعةٌ ودوالُّ أُخرى، فمسحٌ على الملفِّ يحمرُّ كذبًا
     ويُطفَأ. يُقتطَعُ الجسمُ بموازنةِ الأقواسِ من توقيعِ الدالّةِ، ثمّ:
       ① لا سلسلةَ عربيّةً فيه البتّة — أيُّ لفظِ نوعٍ فيه جدولٌ يعود؛
       ② ولا بدَّ من ذكرِ `sadTypeKindSizeBytes` — فالاشتقاقُ يُثبَتُ حضورًا
          لا غيابًا وحدَه، وإلّا مرَّ جسمٌ فارغٌ أخضرَ.

(EN) Guard: @حجم must derive its number from the generated SoT table. It
     measures the FUNCTION BODY (brace-balanced), not the whole file: the
     files carry legitimate Arabic diagnostics elsewhere, and a file-wide
     scan would go red falsely and be switched off.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

# (AR) (المسار، توقيعُ الدالّة) — بابُ `@حجم` في المترجم.
SITES = (
    (ROOT / "compiler" / "src" / "frontend" / "builders" / "expression_lowlevel.cpp",
     "BuildResult ExpressionBuilder::buildExprSizeof(AST::SizeofExpr *sizeofExpr)"),
    # (AR) 🔑 زال موقعُ المفسّر مع محرّكِه. والحارسُ **لم يصر خاويًا**: الشرطُ
    #      (نداءُ `sadTypeKindSizeBytes` ولا جدولَ أسماءٍ عربيٍّ مكتوبًا باليد)
    #      يُقاسُ على موقعِ المترجمِ الباقي ويمكنُ أن يكذبَ عليه.
    # (EN) The interpreter site went with its engine. The guard is NOT vacuous:
    #      its condition is still measured on — and falsifiable at — the compiler site.
)

REQUIRED_CALL = "sadTypeKindSizeBytes"

# (AR) سلسلةٌ فيها حرفٌ عربيٌّ واحدٌ على الأقلّ — وهي وحدَها المقيسة.
#      والتعليقاتُ تُنزَع أوّلًا فلا تُتّهم شروحُ العلّةِ بأنّها جدول.
_ARABIC_STRING = re.compile(r'"[^"\n]*[؀-ۿ][^"\n]*"')
# (AR) 🔑 **وكسحُ التعليقاتِ يُورَثُ من الحارسِ الشقيقِ ولا يُهجّى ثانيةً.**
#      وتعبيرانِ نمطيّانِ لا يكفيان: `//` **داخلَ سلسلةٍ** يبترُ بقيّةَ السطرِ
#      فيُخفي عربيًّا بعدَه، و`/*` داخلَ سلسلةٍ يمدُّ الحذفَ عبرَ شفرةٍ حقيقيّة.
#      والشقيقُ آلةُ حالاتٍ تُميِّزُ السلسلةَ من التعليقِ وهي **مُعايَرةٌ ٦/٦**.
# (EN) Inherit the sibling guard's string-aware comment scrubber; two regexes
#      mis-handle `//` and `/*` inside string literals.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from check_builtin_engine_coverage import _strip_comments  # noqa: E402

# (AR) وعددٌ حرفيٌّ في الجسمِ جدولٌ في صورةٍ أخرى: `case Map: return 48;` بلا
#      سلسلةٍ عربيّةٍ واحدة. والسقفُ صفرٌ — رفعُه قرارٌ صريحٌ يُرى في الفرق.
#      (قِيسَ: صفرُ حرفِ رقمٍ في الجسمِ القائم.)
CEILING_LITERAL_NUMBERS = 0
_LITERAL_NUMBER = re.compile(r"(?<![\w.])[0-9]+(?![\w.])")


def _body_of(text: str, signature: str, path: Path) -> str:
    """(AR) جسمُ الدالّةِ بموازنةِ الأقواسِ — لا بعددِ أسطرٍ يُخمَّن.

    🔑 **وتعذُّرُ بلوغِ الجسمِ عطبُ آلةٍ لا حكمٌ على المحتوى.** كان يُرفَعُ ههنا
       `SystemExit` بنصٍّ — ورمزُه **١** — و`x.py` يُفرِّقُ صراحةً: ٢ «لم يُقَسْ
       شيء» و١ حكمٌ صدرَ عن قياسٍ تمّ. فحارسٌ **لم يقرأْ جسمًا واحدًا** كان
       يُقرأُ في CI «وجدَ جدولَ أحجامٍ مكتوبًا باليد». والفرقُ عمليّ: الأوّلُ
       يُصلَحُ بتحديثِ التوقيعِ في الحارس، والثاني بحذفِ جدولٍ من المترجّم."""
    start = text.find(signature)
    if start < 0:
        raise AssertionError(
            f"لم يُعثَر على توقيعِ الدالّةِ في {path.name}: {signature} —"
            " إن أُعيدت تسميتُها فحدِّثِ الحارسَ؛ حارسٌ لا يجدُ هدفَه لا يحرس."
        )
    open_brace = text.find("{", start)
    if open_brace < 0:
        raise AssertionError(f"توقيعٌ بلا جسمٍ في {path.name}")
    depth = 0
    for i in range(open_brace, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[open_brace:i + 1]
    raise AssertionError(f"جسمٌ غيرُ مُغلَقٍ في {path.name}")


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    print("حارس «حجمُ النوعِ يُشتَقُّ ولا يُكتَب»:")
    # (AR) 🔑 **وصفرُ موقعٍ «لم يُقَسْ شيء» لا «لا جدولَ يدويًّا».** وقِيسَ:
    #      `SITES = ()` كان يُخرِجُ سطرَ النجاحِ ورمزَ ٠ — حارسٌ أخضرُ إلى
    #      الأبدِ وسجلُّ عيارٍ كاملٌ يشهدُ له. وقد نزلَ الصفُّ فعلًا من موقعَينِ
    #      إلى واحدٍ حينَ زالَ محرّكُ المفسّر، والترويسةُ تُطمئِنُ **نثرًا**.
    if not SITES:
        print("  ✗ عطبُ آلة: لا موقعَ يُقاس — صفُّ المواقعِ خالٍ")
        return 2
    failed = False
    for path, signature in SITES:
        if not path.is_file():
            # (AR) وغيابُ الملفِّ عطبُ آلةٍ كذلك: لم يُقَسْ موقعٌ واحد.
            print(f"  ✗ عطبُ آلة: ملفٌّ مفقود: {path}")
            return 2
        try:
            body = _body_of(path.read_text(encoding="utf-8"), signature, path)
        except AssertionError as exc:
            print(f"  ✗ عطبُ آلة: {exc}")
            return 2
        stripped = _strip_comments(body)
        literals = _ARABIC_STRING.findall(stripped)
        # (AR) 🔑 **نداءٌ لا ذِكر.** `REQUIRED_CALL in stripped` عضويّةٌ نصّيّة،
        #      فجسمٌ يعودُ إلى `switch (kind) { case Map: return 48; }` ويحملُ
        #      ذِكرًا ميّتًا واحدًا للاسمِ كان يمرُّ **أخضرَ على الشرطَين**.
        derives = (REQUIRED_CALL + "(") in stripped
        numbers = _LITERAL_NUMBER.findall(
            _ARABIC_STRING.sub("", stripped))
        status = "✓" if (not literals and derives
                         and len(numbers) <= CEILING_LITERAL_NUMBERS) else "✗"
        print(f"  {status} {path.name} — يشتقّ: {'نعم' if derives else 'لا'}"
              f" · سلاسلُ عربيّةٌ في الجسم: {len(literals)}"
              f" · أعدادٌ حرفيّةٌ: {len(numbers)}"
              f" (السقف {CEILING_LITERAL_NUMBERS})")
        if len(numbers) > CEILING_LITERAL_NUMBERS:
            print("    ⇐ عددٌ حرفيٌّ في جسمِ `@حجم` جدولٌ في صورةٍ أخرى:"
                  f" {sorted(set(numbers))[:8]}. الحجمُ من `size_bytes` وحدَه.")
            failed = True
        if literals:
            for lit in literals[:8]:
                print(f"      · {lit}")
            print("    ⇐ جدولُ أحجامٍ مكتوبٌ باليدِ عاد. الحجمُ من"
                  " `language-truth/types.yaml` (حقل `size_bytes`) وحدَه.")
            failed = True
        if not derives:
            print(f"    ⇐ لا نداءَ لـ`{REQUIRED_CALL}(` — الرقمُ لا يُشتَقُّ من"
                  " مصدرِ الحقيقة. (والذِّكرُ بلا نداءٍ ليس اشتقاقًا.)")
            failed = True
    if failed:
        return 1
    print(f"  ✓ المترجمُ ينادي الجدولَ المولَّد في {len(SITES)} موقعًا،"
          " ولا جدولَ يدويًّا فيه — لا بسلسلةٍ عربيّةٍ ولا بعددٍ حرفيّ.")
    return 0


if __name__ == "__main__":
    # (AR) كلُّ انهيارٍ عطبُ آلةٍ لا حكم — والرمزُ ١ محجوزٌ لحكمٍ صدرَ عن قياسٍ تمّ.
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"  ✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
