#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ: كلُّ نداءِ `apt` في سير العمل يمرُّ عبرَ `scripts/ci/apt_retry.sh`.

     🔑 الدافعُ مقيسٌ لا مُفترَض. كُتِبَ `apt_retry.sh` في ٢٠٢٦-٠٨-١٨ بعدَ أن
     علقت ثلاثُ وظائفَ على مرآةِ أوبونتو، لكنّه وُصِلَ بـ`ci.yml` وحدَه. وفي
     اليومِ التالي — ٢٠٢٦-٠٨-١٩ — علقت خطوةُ `SDL2 (Linux)` في بوّابةِ
     الرسوماتِ على `apt-get update` **ستّينَ دقيقةً** حتّى قطعَها حدُّ الوظيفة،
     فقُرِئت الحمرةُ «إلغاءً» لا قياسًا: صفرُ بناءٍ وصفرُ اختبار.

     فالعلّةُ ليست غيابَ الرقعة، بل أنّ الرقعةَ سُدَّت في ملفٍّ وتُرِكَ إخوتُه.
     وهذا ما يحرسُه هذا الملفّ: يُحاكَمُ في الاتّجاهَين — يُحمِّرُ نداءَ `apt`
     عاريًا في سير العمل، ويُحمِّرُ كذلك اختفاءَ المُغلِّفِ نفسِه أو انفكاكَه.

(EN) Guard: every apt invocation in .github/workflows must go through
     scripts/ci/apt_retry.sh. Judged in both directions — a bare apt call
     reddens, and so does the wrapper going missing or unreferenced.
"""

import re
import sys
from pathlib import Path

# (AR) المرساةُ هي **الفعلُ** لا هجاؤه: استدعاءُ apt بأمرٍ من أوامرِه.
# (EN) The anchor is the act, not its spelling: apt invoked with a subcommand.
ACT = re.compile(
    r"(?<![\w./-])apt(?:-get|itude)?\s+"
    r"(?:update|upgrade|dist-upgrade|full-upgrade|install|remove|purge|build-dep)"
    r"(?![\w-])"
)

WRAPPER = Path("scripts/ci/apt_retry.sh")
WORKFLOWS = Path(".github/workflows")


def اجمع_المخالفات(الجذر):
    """(AR) يُعيدُ قائمةَ (المسار، رقمُ السطر، النصّ) لكلِّ نداءِ apt عارٍ."""
    مخالفات = []
    for ملف in sorted((الجذر / WORKFLOWS).glob("*.y*ml")):
        نص = ملف.read_text(encoding="utf-8", errors="replace")
        for رقم, سطر in enumerate(نص.splitlines(), start=1):
            مجرد = سطر.strip()
            # (AR) التعليقُ لا يُنفَّذ — فلا يُحاسَبُ عليه.
            if مجرد.startswith("#"):
                continue
            if not ACT.search(مجرد):
                continue
            # (AR) المرورُ عبرَ المُغلِّفِ هو الإذنُ الوحيد.
            if WRAPPER.name in مجرد:
                continue
            مخالفات.append((ملف.relative_to(الجذر).as_posix(), رقم, مجرد))
    return مخالفات


def افحص_المغلف(الجذر):
    """(AR) الاتّجاهُ الثاني: المُغلِّفُ موجودٌ وموصولٌ فعلًا."""
    أعطاب = []
    مسار = الجذر / WRAPPER
    if not مسار.is_file():
        أعطاب.append("المُغلِّفُ " + WRAPPER.as_posix() + " غيرُ موجود")
        return أعطاب
    وصلات = 0
    for ملف in sorted((الجذر / WORKFLOWS).glob("*.y*ml")):
        نص = ملف.read_text(encoding="utf-8", errors="replace")
        وصلات += نص.count(WRAPPER.as_posix())
    if وصلات == 0:
        أعطاب.append("المُغلِّفُ موجودٌ ولا يستدعيه سيرُ عملٍ واحد — رقعةٌ ميّتة")
    return أعطاب


def main():
    الجذر = Path(__file__).resolve().parents[2]
    مخالفات = اجمع_المخالفات(الجذر)
    أعطاب = افحص_المغلف(الجذر)

    if not مخالفات and not أعطاب:
        print("✅ حارسُ apt: كلُّ نداءٍ يمرُّ عبرَ " + WRAPPER.as_posix())
        return 0

    for عطب in أعطاب:
        print("::error::حارسُ apt: " + عطب)
    for مسار, رقم, نص in مخالفات:
        print(
            "::error file=" + مسار + ",line=" + str(رقم)
            + "::نداءُ apt عارٍ — مرِّرْه عبرَ " + WRAPPER.as_posix()
            + " │ " + نص
        )

    print("")
    print("❌ حارسُ apt: " + str(len(مخالفات)) + " نداءً عاريًا و"
          + str(len(أعطاب)) + " عطبًا في المُغلِّف")
    print("   الإصلاح: bash " + WRAPPER.as_posix() + " update")
    print("            bash " + WRAPPER.as_posix() + " install -y <الحزم>")
    return 1


if __name__ == "__main__":
    sys.exit(main())
