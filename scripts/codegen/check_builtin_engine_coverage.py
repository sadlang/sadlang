#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «تغطيةُ المحرّكَين تُقاس ولا تُدَّعى».

     مدمجٌ معلَنٌ في مصدرِ الحقيقةِ قد يعيشُ في محرّكٍ واحدٍ دونَ الآخر:

       • **معلَنٌ بلا ذراعٍ في المترجم** ⇒ يُخفقُ مُصرَّفًا بـSEM047 المسمّى.
         سطحُ لغةٍ نصفُه موجود، ومن قرأ `status: stable` وحدَه ظنّه كاملًا.

     ⚠️ **ولا يُقاس ههنا الجانبُ المقابل** (مسجَّلٌ في المترجمِ بلا المفسّر):
        مسحُ `registerBuiltinFunction` **لا يراه كلَّه** — قِيس أنّ `خروج`
        يعملُ مُفسَّرًا وهو غيرُ مسجَّلٍ بهذا الشكل، فالمفسّرُ يحلُّ أسماءً
        بطرقٍ أخرى. ونشرُ رقمٍ من أداةٍ لم تُعايَر أسوأُ من السكوت:
        يُقرأ حقيقةً ويُبنى عليه. يُقاس ما بُرهن، ويُسكَت عمّا لم يُبرهَن.

     🔑 والغرضُ **العددُ لا المنعُ**: التباعدُ حقيقةٌ قائمةٌ تُسمّى بعددِها فلا
        تنمو صامتةً. الحارسُ يحمرُّ إن **زاد** عن السقفِ المُودَع، لا إن وُجِد —
        فسقفٌ يُخفَّض بالعملِ خيرٌ من دعوى «مدعومٌ» في حقلٍ لا يقرؤه أحد.
(EN) Guard: measure how many declared builtins the compiler cannot dispatch.
     Fails when the count grows past the committed ceiling. The interpreter
     side is deliberately NOT measured: the scan is uncalibrated.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"
BUILDERS = ROOT / "compiler" / "src" / "frontend" / "builders"
INTERP = ROOT / "interpreter" / "src" / "builtins"

# (AR) السقفُ المُودَع — يُخفَّض كلّما وُصِل مدمجٌ، ولا يُرفَع إلّا بقرارٍ مكتوب.
# (AR) عُويِر بعشرةِ مجسّاتٍ منفَّذة (٥ متوقَّعةً بلا ذراعٍ و٥ بذراع): ١٠/١٠.
#
# (AR) رفعٌ مُوثَّقٌ واحد: ٥٥٧ ← ٥٦٤ (+٧). والسبعُ دوالُّ SadNet كانت **منفَّذةً
#      في المفسّرِ وغيرَ معلَنة**: لامبداتٌ مكتملةٌ تموتُ محلّيّةً بلا تسجيل،
#      خلفَ `#ifdef HAS_SADNET` لم يصدُقْ قطّ. فالتباعدُ لم ينمُ بإعلانِها بل
#      **صار مقيسًا**: كان قائمًا في المحرّكِ وغيرَ مرئيٍّ لأيّ حارس. ولا ذراعَ
#      لها في المترجّمِ اليوم — كحالِ الخمسَ عشرةَ المعلَنةِ قبلَها (صفرُ ذكرٍ
#      لـSadNet في compiler/src)، فتُخفِقُ مُصرَّفةً بـSEM047 المسمّى لا تتبخّر.
#      🔑 والقاعدةُ التي يحرسُها هذا السقفُ سليمةٌ ههنا: الرفعُ **بقرارٍ مكتوبٍ
#         يُسمّي الأسماءَ السبعةَ وسببَها**، لا بتسكينِ أحمرَ.
# (EN) One documented raise: 557 → 564 (+7) — SadNet builtins that were already
#      implemented in the interpreter but undeclared (dead #ifdef). Declaring them
#      made an existing divergence measurable rather than creating a new one.
CEILING_COMPILER_MISSING = 564



def _declared() -> set[tuple[str, str]]:
    names: set[tuple[str, str]] = set()
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            names.add((fn["namespace"], fn["cpp_id"]))
    return names


def _dispatched_in_compiler(declared: set[tuple[str, str]]) -> set[tuple[str, str]]:
    """(AR) الاسمُ مذكورٌ في بانياتِ الأماميّةِ بأيِّ صيغةٍ من صيغِ الإرسال."""
    blob = "\n".join(p.read_text(encoding="utf-8") for p in BUILDERS.glob("*.cpp"))
    present = {m for m in re.findall(r"\b[A-Z][A-Z0-9_]{2,}\b", blob)}
    return {(ns, cid) for ns, cid in declared if cid in present}


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    declared = _declared()
    comp = _dispatched_in_compiler(declared)
    missing = sorted(declared - comp)

    print("حارس «تغطيةُ المترجمِ تُقاس»:")
    print(f"  معلَنٌ في مصدرِ الحقيقة: {len(declared)}")
    print(f"  له ذراعُ إرسالٍ في المترجم: {len(comp)}")
    print(f"  معلَنٌ بلا ذراع: {len(missing)} (السقف {CEILING_COMPILER_MISSING})"
          " — يُخفقُ مُصرَّفًا بـSEM047 المسمّى، لا يتبخّر")

    if len(missing) > CEILING_COMPILER_MISSING:
        print(f"  ✗ التباعدُ نما: {len(missing)} > {CEILING_COMPILER_MISSING} —"
              " مدمجٌ معلَنٌ جديدٌ بلا نظيرٍ في المترجم.")
        return 1
    print("  ✓ لم ينمُ التباعدُ عن السقفِ المُودَع.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
