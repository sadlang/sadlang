#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ يربطُ قوائمَ المُثبِّتِ بجدولِ الأدواتِ الواحد.

     `distribution/install.sh` يُشحَنُ وحدَه إلى المستخدمِ فلا يستطيعُ
     استيرادَ `scripts/ci/release_tools.sh`. فقوائمُه نسخةٌ ثانيةٌ من
     الحقيقة — وقد انجرفت فعلًا: كانت «compiler» تطلبُ `sadc` وحدَه
     و«full» تُغفِلُ `sad-build`، بينما يشترطُهما حَكَمُ الإصدار. أي
     مُثبِّتٌ يقبلُ حزمةً يرفضُها المُصدِر، فيقولُ للمستخدمِ «تمّ» وهو ناقص.

     ولمّا تعذّرَ إلغاءُ النسخةِ الثانية، تُربَطُ الأطرافُ بحارسٍ يقيسُها
     معًا. والأطرافُ من ملفّاتٍ مختلفة — لا من أصلٍ واحدٍ يُنتِجُها،
     وإلّا لكان الحارسُ يقيسُ نفسَه.

     🔑 والمُثبِّتُ مُثبِّتان: `install.sh` لِپوسِكس و`install.ps1` لويندوز.
        حارسٌ يقيسُ أحدَهما يقولُ «مطابق» ونصفُ المستخدمينَ على النصفِ
        المنجرف — وقد كان `install.ps1` منجرفًا الانجرافَ عينَه بعدَ سدِّ
        نظيرِه. فالسدُّ في ملفٍّ واحدٍ يترك أشقاءَه.

(EN) Bind the installers' tool lists to the single tool table.

     distribution/install.sh ships standalone to users and cannot source
     scripts/ci/release_tools.sh, so its lists are a second copy of the
     truth — and they had already drifted: "compiler" required only sadc
     and "full" omitted sad-build, both required by the release judge. An
     installer that accepts a package the publisher rejects tells the user
     "done" over an incomplete install.

     The copies cannot be removed, so every end is measured against the
     table. They come from different files — not from one generator, which
     would make the guard measure itself.

     There are TWO installers: install.sh (POSIX) and install.ps1 (Windows).
     A guard that checks one reports "match" while half the users get the
     drifted half — and install.ps1 had drifted identically after its POSIX
     twin was fixed. Sealing one file leaves its siblings.
"""

import io
import re
import sys
from pathlib import Path

جذر = Path(__file__).resolve().parents[2]
مسار_الجدول = جذر / "scripts" / "ci" / "release_tools.sh"
مسار_المثبت_بوسكس = جذر / "distribution" / "install.sh"
مسار_المثبت_ويندوز = جذر / "distribution" / "install.ps1"

# (AR) المكوّنُ في المُثبِّتِ ← المتغيّرُ المقابلُ في الجدول.
# (EN) Installer component -> the table variable it must equal.
الاقتران = {
    "standard": "SAD_REQUIRED_STANDARD",
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


def استخرج_المثبت_بوسكس(نص):
    نتيجة = {}
    for مكوّن in الاقتران:
        مطابقة = re.search(
            r'^\s*' + مكوّن + r'\)\s*REQUIRED_TOOLS="([^"]*)"', نص, re.MULTILINE
        )
        if not مطابقة:
            print("::error::مكوّنٌ غيرُ موجودٍ في install.sh / not in install.sh: " + مكوّن)
            sys.exit(1)
        نتيجة[مكوّن] = مطابقة.group(1).split()
    return نتيجة


def استخرج_المثبت_ويندوز(نص):
    # (AR) الصيغةُ في پاورشِل: "<مكوّن>" { @("sad", "sad-run", ...) }
    # (EN) PowerShell form: "<component>" { @("sad", "sad-run", ...) }
    نتيجة = {}
    for مكوّن in الاقتران:
        مطابقة = re.search(
            r'^\s*"' + مكوّن + r'"\s*{\s*@\(([^)]*)\)', نص, re.MULTILINE
        )
        if not مطابقة:
            print("::error::مكوّنٌ غيرُ موجودٍ في install.ps1 / not in install.ps1: " + مكوّن)
            sys.exit(1)
        نتيجة[مكوّن] = re.findall(r'"([^"]+)"', مطابقة.group(1))
    return نتيجة


def قارن(الجدول, المثبت, اسم_الملف):
    خلاف = []
    for مكوّن, اسم_المتغيّر in الاقتران.items():
        متوقّع = sorted(الجدول[اسم_المتغيّر])
        موجود = sorted(المثبت[مكوّن])
        if متوقّع != موجود:
            ناقص = sorted(set(متوقّع) - set(موجود))
            زائد = sorted(set(موجود) - set(متوقّع))
            سطر = "  " + اسم_الملف + " · " + مكوّن + " ≠ " + اسم_المتغيّر
            if ناقص:
                سطر += " · ينقصُه / missing: " + " ".join(ناقص)
            if زائد:
                سطر += " · يزيدُ عنه / extra: " + " ".join(زائد)
            خلاف.append(سطر)
    return خلاف


def main():
    الجدول = استخرج_الجدول(اقرأ(مسار_الجدول))

    خلاف = []
    خلاف += قارن(
        الجدول, استخرج_المثبت_بوسكس(اقرأ(مسار_المثبت_بوسكس)), "install.sh"
    )
    خلاف += قارن(
        الجدول, استخرج_المثبت_ويندوز(اقرأ(مسار_المثبت_ويندوز)), "install.ps1"
    )

    if خلاف:
        print(
            "::error file=distribution/install.sh::"
            "قوائمُ المُثبِّتَينِ تخالفُ جدولَ الأدوات / installer lists differ from the tool table"
        )
        for سطر in خلاف:
            print(سطر)
        print(
            "   الإصلاح: اجعلْ قوائمَ الأدواتِ في distribution/install.sh "
            "وdistribution/install.ps1 مطابقةً لـSAD_REQUIRED_* في "
            "scripts/ci/release_tools.sh"
        )
        return 1

    print(
        "✅ حارسُ قوائمِ المُثبِّتَين: "
        + str(len(الاقتران) * 2)
        + " قائمةً مطابقةٌ لجدولِ الأدوات / lists match the tool table"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
