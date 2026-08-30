#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ يربطُ قوائمَ المُثبِّتِ بجدولِ الأدواتِ الواحد.

     `distribution/install.sh` يُشحَنُ وحدَه إلى المستخدمِ فلا يستطيعُ
     استيرادَ `scripts/ci/release_tools.sh`. فقوائمُه نسخةٌ ثانيةٌ من
     الحقيقة — وقد انجرفت فعلًا: كانت «compiler» تطلبُ `sadc` وحدَه
     و«full» تُغفِلُ `sad-build`، بينما يشترطُهما حَكَمُ الإصدار. أي
     مُثبِّتٌ يقبلُ حزمةً يرفضُها المُصدِر، فيقولُ للمستخدمِ «تمّ» وهو ناقص.

     ولمّا تعذّرَ إلغاءُ النسخةِ الثانية، يُربَطُ الطرفانِ بحارسٍ يقيسُهما
     معًا. والطرفانِ من ملفَّينِ مختلفَين — لا من أصلٍ واحدٍ يُنتِجُهما،
     وإلّا لكان الحارسُ يقيسُ نفسَه.

(EN) Bind the installer's tool lists to the single tool table.

     distribution/install.sh ships standalone to users and cannot source
     scripts/ci/release_tools.sh, so its lists are a second copy of the
     truth — and they had already drifted: "compiler" required only sadc
     and "full" omitted sad-build, both required by the release judge. An
     installer that accepts a package the publisher rejects tells the user
     "done" over an incomplete install.

     The second copy cannot be removed, so the two ends are measured
     against each other. They come from two different files — not from one
     generator, which would make the guard measure itself.
"""

import io
import re
import sys
from pathlib import Path

جذر = Path(__file__).resolve().parents[2]
مسار_الجدول = جذر / "scripts" / "ci" / "release_tools.sh"
مسار_المثبت = جذر / "distribution" / "install.sh"

# (AR) المكوّنُ في المُثبِّتِ ← المتغيّرُ المقابلُ في الجدول.
# (EN) Installer component -> the table variable it must equal.
الاقتران = {
    "interpreter": "SAD_REQUIRED_INTERPRETER",
    "compiler": "SAD_REQUIRED_COMPILER",
    "full": "SAD_REQUIRED_FULL",
}


def اقرأ(مسار):
    if not مسار.is_file():
        print("::error::ملفٌّ مفقود / missing file: " + str(مسار))
        sys.exit(1)
    return io.open(مسار, encoding="utf-8").read()


def استخرج_الجدول(نص):
    نتيجة = {}
    for اسم in set(الاقتران.values()):
        مطابقة = re.search(r'^' + اسم + r'="([^"]*)"', نص, re.MULTILINE)
        if not مطابقة:
            print("::error::متغيّرٌ غيرُ موجودٍ في الجدول / not in table: " + اسم)
            sys.exit(1)
        نتيجة[اسم] = مطابقة.group(1).split()
    return نتيجة


def استخرج_المثبت(نص):
    نتيجة = {}
    for مكوّن in الاقتران:
        مطابقة = re.search(
            r'^\s*' + مكوّن + r'\)\s*REQUIRED_TOOLS="([^"]*)"', نص, re.MULTILINE
        )
        if not مطابقة:
            print("::error::مكوّنٌ غيرُ موجودٍ في المُثبِّت / not in installer: " + مكوّن)
            sys.exit(1)
        نتيجة[مكوّن] = مطابقة.group(1).split()
    return نتيجة


def main():
    الجدول = استخرج_الجدول(اقرأ(مسار_الجدول))
    المثبت = استخرج_المثبت(اقرأ(مسار_المثبت))

    خلاف = []
    for مكوّن, اسم_المتغيّر in الاقتران.items():
        متوقّع = sorted(الجدول[اسم_المتغيّر])
        موجود = sorted(المثبت[مكوّن])
        if متوقّع != موجود:
            ناقص = sorted(set(متوقّع) - set(موجود))
            زائد = sorted(set(موجود) - set(متوقّع))
            سطر = "  " + مكوّن + " ≠ " + اسم_المتغيّر
            if ناقص:
                سطر += " · ينقصُه / missing: " + " ".join(ناقص)
            if زائد:
                سطر += " · يزيدُ عنه / extra: " + " ".join(زائد)
            خلاف.append(سطر)

    if خلاف:
        print(
            "::error file=distribution/install.sh::"
            "قوائمُ المُثبِّتِ تخالفُ جدولَ الأدوات / installer lists differ from the tool table"
        )
        for سطر in خلاف:
            print(سطر)
        print(
            "   الإصلاح: اجعلْ REQUIRED_TOOLS في distribution/install.sh "
            "مطابقًا لـSAD_REQUIRED_* في scripts/ci/release_tools.sh"
        )
        return 1

    print(
        "✅ حارسُ قوائمِ المُثبِّت: "
        + str(len(الاقتران))
        + " مكوّنًا مطابقٌ لجدولِ الأدوات / components match the tool table"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
