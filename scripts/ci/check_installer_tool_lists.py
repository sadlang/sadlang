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

     🔑 **والمُثبِّتُ ليس مَن يبني الحزمة.** كان هذا الحارسُ يقيسُ
        `install.sh` و`install.ps1` وحدَهما — وهما ما يُشحَنُ للمستخدمِ
        ليجلبَ أرشيفًا جاهزًا. أمّا الأرشيفُ نفسُه فتبنيه مساراتٌ أخرى لم
        يكنْ يراها أحد: `build-installers.ps1` (AppImage · deb · رزمة rpm ·
        حزمة macOS) و`macos/build-pkg.sh` و`rpm/sad.spec` والمُثبِّتانِ
        الرسوميّان. وقد كانت هذه المساراتُ تنسخُ `sadc` — وهو **لا يُنتِجُه
        أيُّ هدفِ بناء** — وتُغفِلُ `sad-build` الموجود، وتُغفِلُ `sad-check`
        المُلزَمَ في `SAD_REQUIRED_STANDARD`. فحزمةٌ تُبنى ناقصةً ثمّ يقبلُها
        مُثبِّتٌ «مطابقٌ للجدول»: الحارسُ أخضرُ والمنتَجُ ناقص.
        فصارَ يقيسُ الطرفَين: قوائمَ المُثبِّتِ **ومساراتِ بناءِ الحزمة**.

     ⚠️ وشرطٌ ثانٍ على المُثبِّتَين الرسوميَّين: أداةٌ مُلزَمةٌ لا تُنسَخُ
        بـ`skipifsourcedoesntexist`. فالعَلَمُ يجعلُ غيابَ الملفِّ صامتًا،
        فيُقالُ للمستخدمِ «تمّ» ولا يُنسَخُ شيء — وهي عينُ الكذبةِ التي
        شحنَها مكوّنُ REPL سنينَ. المُلزَمُ يُنسَخُ أو يُخفِقُ البناء.

(EN) The installer is not what builds the package. This guard measured only
     install.sh and install.ps1 — what ships to a user to fetch a ready
     archive. The archive itself is built by paths nobody was watching:
     build-installers.ps1 (AppImage, deb, rpm tarball, macOS pkg),
     macos/build-pkg.sh, rpm/sad.spec and the two GUI installers. Those paths
     copied `sadc`, which NO build target produces, skipped `sad-build`, which
     does exist, and skipped `sad-check`, which SAD_REQUIRED_STANDARD makes
     mandatory: a package built incomplete and then accepted by an installer
     that matches the table — guard green, product short. It now measures both
     ends. Second rule for the GUI installers: a required tool may not be
     copied with skipifsourcedoesntexist, which makes a missing file silent —
     the very lie the REPL component shipped for years.

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

# (AR) مساراتُ بناءِ الحزمِ: كلٌّ منها يجبُ أن يشحنَ كلَّ مُلزَمٍ في
#      SAD_REQUIRED_STANDARD. والمفتاحُ اسمُ المسارِ كما يظهرُ في التشخيص.
# (EN) Package-building paths; each must ship every SAD_REQUIRED_STANDARD tool.
مسارات_البناء = {
    "build-installers.ps1": جذر / "distribution" / "build-installers.ps1",
    "macos/build-pkg.sh": جذر / "distribution" / "macos" / "build-pkg.sh",
    "rpm/sad.spec": جذر / "distribution" / "rpm" / "sad.spec",
    "installer/sad-setup.iss": جذر / "distribution" / "installer" / "sad-setup.iss",
    "installer/sad-setup-v2.iss": جذر / "distribution" / "installer" / "sad-setup-v2.iss",
}

# (AR) ⚠️ **اللقبُ لا يُغني عن الأصل.** كُتِبَ هنا أوّلًا أنّ `sadc` هو
#      `sad-build` «فوجودُ أيِّهما يكفي» — وبرهانُ الحقنِ أثبتَ خطأَه: حُذف
#      `%{_bindir}/sad-build` من `%files` فبقيَ الحارسُ أخضرَ لأنّ `sadc`
#      ما زال مذكورًا. والاسمانِ ليسا متكافئَين: مركزُ الأدواتِ يجدُ إخوتَه
#      بمسحِ البادئةِ `sad-`، فاسمٌ بلا شرطةٍ لا يُسجَّلُ عندَه و`sad build`
#      يصيرُ أمرًا موعودًا لا وجودَ له. فالمطلوبُ الاسمُ الحرفيّ.
# (EN) An alias does not substitute for the name. This first said sadc IS
#      sad-build "so either one suffices"; injection proved it wrong — deleting
#      %{_bindir}/sad-build from %files left the guard green because sadc was
#      still listed. The two are not interchangeable: the hub finds its
#      siblings by scanning for the sad- prefix, so a dashless name is never
#      registered and `sad build` becomes a promised command that is absent.
#      The literal name is what is required.

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


def استخرج_أدوات_مسار_البناء(اسم_المسار, نص):
    """(AR) قوائمُ الثنائيّاتِ في مسارِ بناءٍ واحد — قائمةٌ لكلِّ حزمةٍ مستقلّة.

    🔑 يُرجِعُ **قائمةَ مجموعاتٍ** لا مجموعةً واحدة. و`build-installers.ps1`
       يبني أربعَ حزمٍ (AppImage · deb · رزمة rpm · حزمة macOS) لكلٍّ حلقةُ
       نسخٍ خاصّةٌ بها؛ فجمعُها في مجموعةٍ واحدةٍ يجعلُ أداةً موجودةً في حلقةٍ
       تُغطّي غيابَها عن الثلاثِ الباقية. برهانُ الحقنِ أثبتَه: حُذفت
       `sad-check` من حلقةٍ واحدةٍ فبقيَ الحارسُ أخضر.
       أمّا `macos/build-pkg.sh` فحلقاتُه حزمةٌ واحدةٌ (مُلزِمةٌ ثمّ أفضلُ
       جهد) فتُجمَع.
    (EN) Returns a LIST OF SETS, not one set. build-installers.ps1 builds four
       packages, each with its own copy loop, so unioning them lets a tool
       present in one loop mask its absence from the other three — injection
       proved it: removing sad-check from a single loop left the guard green.
       macos/build-pkg.sh, whose loops are one package (mandatory then
       best-effort), is unioned.
    """
    أسماء = set()
    if اسم_المسار.endswith(".iss"):
        # (AR) 🔑 **الاسمُ الواقعُ في الحزمةِ هو `DestName` إن وُجد.**
        #      كان اسمُ المصدرِ يُجمَعُ معه، فسطرٌ مصدرُه `sad-build.exe`
        #      ووجهتُه `sadc.exe` يُحسَبُ شحنًا للاسمَين وهو يشحنُ الثاني
        #      وحدَه. وبرهانُ الحقنِ أظهرَه: تبديلُ `DestName` إلى اسمٍ آخرَ
        #      بقيَ أخضرَ لأنّ المصدرَ لم يتبدّل.
        # (EN) What lands in the package is DestName when present. The source
        #      basename used to be counted alongside it, so a line whose source
        #      is sad-build.exe and whose DestName is sadc.exe counted as
        #      shipping BOTH names while it ships only the second. Injection
        #      showed it: changing DestName stayed green because the source did
        #      not change.
        for سطر in نص.split(chr(10)):
            if not سطر.lstrip().startswith("Source:"):
                continue
            وجهة = re.search(r'DestName:\s*"([A-Za-z0-9_-]+)\.exe"', سطر)
            if وجهة:
                أسماء.add(وجهة.group(1))
                continue
            مصدر = re.search(r'Source:[^\n]*?\\([A-Za-z0-9_-]+)\.exe"', سطر)
            if مصدر:
                أسماء.add(مصدر.group(1))
    elif اسم_المسار.endswith(".spec"):
        كتلة = نص.split("%files", 1)
        if len(كتلة) > 1:
            أسماء |= set(re.findall(r"%\{_bindir\}/([A-Za-z0-9_-]+)", كتلة[1]))
    else:
        قوائم = []
        for قائمة in re.findall(r"for\s+_?bin\s+in\s+([^;\n]+?)(?:;|\s*do)", نص):
            أدوات = set(w for w in قائمة.split()
                        if re.match(r"^sad[A-Za-z0-9_-]*$", w))
            # (AR) حلقةٌ لا اسمَ أداةٍ فيها ليست قائمةَ أدوات — مثل
            #      `for bin in bin/*` التي تمشي على ملفّاتٍ لا على أسماء.
            # (EN) A loop with no tool literal is not a tool list — e.g.
            #      `for bin in bin/*`, which walks files, not names.
            if أدوات:
                قوائم.append(أدوات)
        # (AR) سطرُ اللقب: `cp .../sad-build .../sadc` وما شابهَه
        ألقاب = set(re.findall(r"/(sad[A-Za-z0-9_-]*)\"?\s*$", نص, re.MULTILINE))
        if not قوائم:
            return [ألقاب]
        if اسم_المسار == "build-installers.ps1":
            return [قائمة | ألقاب for قائمة in قوائم]
        اتّحاد = set()
        for قائمة in قوائم:
            اتّحاد |= قائمة
        return [اتّحاد | ألقاب]
    return [أسماء]


def أدواتٌ_مُلزَمةٌ_بعَلَمِ_التخطّي(نص, مُلزَم):
    """(AR) أداةٌ مُلزَمةٌ تُنسَخُ بـskipifsourcedoesntexist: غيابُها يصيرُ صامتًا."""
    مخالِفة = []
    for سطر in نص.split("\n"):
        if not سطر.startswith("Source:") or "skipifsourcedoesntexist" not in سطر:
            continue
        مطابقة = re.search(r'DestName:\s*"([A-Za-z0-9_-]+)\.exe"', سطر)
        if not مطابقة:
            مطابقة = re.search(r'Source:[^\n]*?\\([A-Za-z0-9_-]+)\.exe"', سطر)
        if not مطابقة:
            continue
        if مطابقة.group(1) in مُلزَم:
            مخالِفة.append(مطابقة.group(1))
    return sorted(set(مخالِفة))


def قارن_مسارات_البناء(الجدول):
    مُلزَم = set(الجدول["SAD_REQUIRED_STANDARD"])
    خلاف = []
    for اسم_المسار, مسار in sorted(مسارات_البناء.items()):
        نص = اقرأ(مسار)
        قوائم = استخرج_أدوات_مسار_البناء(اسم_المسار, نص)
        if not قوائم or not any(قوائم):
            # (AR) قائمةٌ فارغةٌ تعني أنّ المستخرِجَ لم يرَ شيئًا — وذاك انجرافُ
            #      صيغةٍ لا حزمةٌ سليمة. الصمتُ هنا أخطرُ من الاختلاف.
            # (EN) An empty list means the extractor saw nothing — a syntax drift,
            #      not a healthy package. Silence here is worse than a mismatch.
            خلاف.append(
                "  " + اسم_المسار + " لم يُستخرَجْ منه أيُّ اسمِ أداة (انجرافُ صيغة) / "
                "no tool names extracted (format drift)"
            )
            continue
        for رقم in range(len(قوائم)):
            ناقص = sorted(مُلزَم - قوائم[رقم])
            if ناقص:
                لاحقة = "" if len(قوائم) == 1 else " (قائمة " + str(رقم + 1) + ")"
                خلاف.append(
                    "  " + اسم_المسار + لاحقة
                    + " لا يشحنُ مُلزَمًا / does not ship required: "
                    + " ".join(ناقص)
                )
        if اسم_المسار.endswith(".iss"):
            صامتة = أدواتٌ_مُلزَمةٌ_بعَلَمِ_التخطّي(نص, مُلزَم)
            if صامتة:
                خلاف.append(
                    "  " + اسم_المسار
                    + " يَنسَخُ مُلزَمًا بـskipifsourcedoesntexist (غيابٌ صامت) / "
                      "required tool copied with skipifsourcedoesntexist: "
                    + " ".join(صامتة)
                )
    return خلاف


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
    خلاف += قارن_مسارات_البناء(الجدول)

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
            "scripts/ci/release_tools.sh، ولْيَشحنْ كلُّ مسارِ بناءٍ كلَّ ما في "
            "SAD_REQUIRED_STANDARD بلا عَلَمِ تخطٍّ"
        )
        return 1

    print(
        "✅ حارسُ الأدوات: "
        + str(len(الاقتران) * 2)
        + " قائمةَ مُثبِّتٍ و"
        + str(len(مسارات_البناء))
        + " مسارَ بناءٍ مطابقةٌ لجدولِ الأدوات / installer lists and package-build "
          "paths match the tool table"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
