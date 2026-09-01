#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ يربطُ إملاءَ عَلَمِ الإصدارِ بمصدرِ حقيقتِه.

     العطبُ الذي ولّد هذا الحارس: ماكرو الدُّخانِ في
     `tests/system/hub/CMakeLists.txt` كان يكتبُ `--version` حرفيًّا لكلِّ
     أداة، فكان يقيسُ **إملاءَه هو** لا عقدَ الأداة — ستُّ خاناتٍ حمراءُ
     بعلّةٍ واحدة. فصارَ الإملاءُ مُعامَلًا، ثمّ صارَ جدولًا في
     `scripts/ci/release_tools.sh` (SAD_VERSION_FLAGS).

     🔑 ولكنّ الجدولَ **نسخةٌ ثالثةٌ** من حقيقةٍ مكتوبةٍ في
     `language-truth/cli_flags.yaml`. و`sad_require_version_flags` يقرأُ
     طرفَيه من **ملفٍّ واحد** — فلا يمسكُ إلّا خطأً مطبعيًّا داخلَه، ولا
     يرى انجرافًا عن مصدرِ الحقيقةِ إطلاقًا. وحارسٌ طرفاه من أصلٍ واحدٍ
     يقيسُ نفسَه.

     🔑 والقاعدةُ هنا **مشتقّةٌ لا مُعلَنة**: لا قائمةَ «محرّكات» تُكتَبُ
     باليدِ في هذا الملفّ. فالأداةُ التي يستشيرُ مصدرُها جدولَ الأعلامِ
     المولَّدَ (`FlagAction`) ملزَمةٌ بالاسمِ القانونيِّ العربيِّ، والتي
     تُحلِّلُ سلاسلَ حرفيّةً ممنوعةٌ من ادّعائِه. فلو نُقِلت أداةٌ إلى
     الجدولِ أو أُخرِجت منه، تحوّلَ حُكمُ الحارسِ معها بلا تعديلٍ هنا.

     خمسةُ ملفّاتٍ بخمسةِ مُحلِّلاتٍ مختلفة — وهذا شرطُ الحارس:
       ① language-truth/cli_flags.yaml      (YAML)  ← الاسمُ القانونيّ
       ② مصدرُ كلِّ أداة                     (C++)   ← أتستشيرُ الجدولَ؟
       ③ scripts/ci/release_tools.sh        (شِل)   ← SAD_VERSION_FLAGS
       ④ tests/system/hub/CMakeLists.txt    (CMake) ← أعلامُ الدُّخان
       ⑤ المهاراتُ والوثائقُ وtools/build/*.cmake (نثر) ← أعلامُ الأمثلة

     🔑 والمُحلِّلُ الخامسُ وُلدَ من عطبٍ مقيس: `references/build-test.md`
     كان يوصي بـ`--emit-llvm` و`sad-os-coding/SKILL.md` بـ`--freestanding`
     و`--module` — ولا مرادفَ إنجليزيًّا في اللغةِ أصلًا («ONE canonical
     Arabic name — no aliases»). فالوكيلُ الذي يتبعُ المهارةَ يكتبُ أمرًا
     يفشل. ومعها `tools/build/FindSad.cmake` كان ينفّذُ `--version`، وهو
     خيارٌ غيرُ معروفٍ يبتلعُه `ERROR_QUIET` فتبقى `Sad_VERSION` فارغةً
     والكتلةُ كلُّها ميّتةٌ صامتة. المثالُ المنثورُ نسخةٌ ثانيةٌ لعقدٍ
     مكتوبٍ في `cli_flags.yaml`، ولم يكن أحدٌ يقيسُ المسافةَ بينهما.

(EN) Bind the version-flag spelling to its source of truth.

     The defect behind this guard: the smoke macro wrote --version literally
     for every tool, measuring its own spelling rather than the tool's
     contract — six red cells from one cause. The spelling became a parameter,
     then a table in release_tools.sh (SAD_VERSION_FLAGS).

     But that table is a THIRD copy of a fact written in cli_flags.yaml, and
     sad_require_version_flags reads both its ends from ONE file — so it can
     only catch a typo inside that file and can never see drift from the
     source of truth. A guard with both ends from one source measures itself.

     The rule here is DERIVED, not declared: there is no hand-written list of
     "engines". A tool whose source consults the generated flag table
     (FlagAction) is bound to the canonical Arabic name; a tool that parses
     string literals is forbidden from claiming it. Move a tool onto or off
     the table and this guard's verdict moves with it, untouched.

     Four files, four different parsers — which is the guard's whole point.
"""

import io
import re
import sys
from pathlib import Path

جذر = Path(__file__).resolve().parents[2]

مسار_مصدر_الحقيقة = جذر / "language-truth" / "cli_flags.yaml"
مسار_جدول_الأدوات = جذر / "scripts" / "ci" / "release_tools.sh"
مسار_دخان_الهب = جذر / "tests" / "system" / "hub" / "CMakeLists.txt"

# (AR) مُوَضِّعٌ لا حقيقة: أين يعيشُ مُحلِّلُ وسائطِ كلِّ أداةٍ منشورة. وإن
#      انتقلَ ملفٌّ سقطَ الحارسُ بصوتٍ عالٍ ولم يتخطَّ الأداةَ صامتًا.
# (EN) A locator, not a fact: where each published tool's argument parser
#      lives. If a file moves the guard fails loudly instead of silently
#      skipping that tool.
مصادر_الأدوات = {
    "sad": "tools/hub/src/main.cpp",
    "sad-run": "apps/sad-run/main.cpp",
    "sad-lsp": "tools/lsp/src/transport/lsp_main.cpp",
    "sad-check": "tools/check/src/main.cpp",
    "sadc": "tools/compiler/compiler_driver_cli.cpp",
    "sad-build": "tools/compiler/compiler_driver_cli.cpp",
}

# (AR) أثرُ استشارةِ الجدولِ المولَّدِ في المصدر. المفسّرُ يستعملُ
#      `sad::cli::FlagAction` والمترجمُ `cli_gen::FlagAction` — والمشتركُ
#      بينهما النوعُ نفسُه، فالبحثُ عنه لا عن مسارِ فضاءِ الأسماء.
# (EN) The trace of consulting the generated table. The interpreter uses
#      sad::cli::FlagAction and the compiler cli_gen::FlagAction — the shared
#      part is the type itself, so that is what is searched for.
أثر_الجدول_المولد = re.compile(r"\bFlagAction\b")

# ── ⑤ أعلامُ الأمثلةِ المنثورة ─────────────────────────────────────────────────
# (AR) مُوَضِّعٌ لا حقيقة: أين تُكتَبُ أوامرُ المحرّكاتِ نثرًا. الأعلامُ نفسُها
#      تُقرأُ من `cli_flags.yaml` دائمًا، فلا قائمةَ إذنٍ تُكتَبُ هنا لتتعفّن.
# (EN) A locator, not a fact: where engine commands are written in prose. The
#      flags themselves always come from cli_flags.yaml — no allowlist here.
شجر_النثر = (".github/skills", "docs")
ملفات_نثر_إضافية = ("tools/build/FindSad.cmake",
                    "tools/build/SadProject.cmake",
                    "tools/build/project_template.cmake")
المحرّكات_في_النثر = re.compile(
    r"(?:^|[\s`(]|\$\{SADC_EXECUTABLE\})"
    r"(?:sad-build-native|sad-build|sad-run|sadc)(?=[\s`)،,.:]|$)")
# (AR) نستثني أسطرَ أدواتٍ أخرى تحمل الاسمَ عرَضًا (cmake --build --target sad-build).
# (EN) Exclude lines where the name is a build target, not an invocation.
سطر_بناء = re.compile(r"\bcmake\b|--target\b|add_executable|find_program")
عَلَم_ascii = re.compile(r"(?<![\w-])(--[A-Za-z][A-Za-z0-9-]*)")


def استخرج_كل_الاعلام_القانونية(نص):
    """كلُّ اسمٍ قانونيٍّ وكلُّ عَلَمٍ قصيرٍ في مصدرِ الحقيقة."""
    أسماء = set(re.findall(r'canonical:\s*"([^"]+)"', نص))
    أسماء |= set(re.findall(r'name:\s*"(-[^"]+)"', نص))
    return أسماء


def امسح_النثر(قانونية):
    """يعيد قائمةَ (ملفّ، سطر، عَلَم) لكلِّ عَلَمٍ منثورٍ لا يطابقُ مصدرَ الحقيقة."""
    مسارات = []
    for شجرة in شجر_النثر:
        جذر_الشجرة = جذر / شجرة
        if جذر_الشجرة.is_dir():
            مسارات.extend(sorted(جذر_الشجرة.rglob("*.md")))
    for ملف in ملفات_نثر_إضافية:
        p = جذر / ملف
        if p.is_file():
            مسارات.append(p)
    خلاف = []
    for مسار in مسارات:
        try:
            سطور = مسار.read_text(encoding="utf-8").splitlines()
        except (OSError, UnicodeDecodeError):
            continue
        for رقم, سطر in enumerate(سطور, 1):
            if not المحرّكات_في_النثر.search(سطر) or سطر_بناء.search(سطر):
                continue
            for عَلَم in عَلَم_ascii.findall(سطر):
                if عَلَم not in قانونية:
                    خلاف.append((مسار.relative_to(جذر).as_posix(), رقم, عَلَم))
    return خلاف


def اقرأ(مسار):
    if not مسار.is_file():
        print("::error::ملفٌّ مفقود / missing file: " + str(مسار))
        sys.exit(1)
    return io.open(مسار, encoding="utf-8").read()


def استخرج_الاسم_القانوني(نص, معرف):
    """(AR) الاسمُ القانونيُّ لعَلَمٍ من مصدرِ الحقيقة. (EN) Canonical name from the SoT."""
    مطابقة = re.search(
        r"^\s*-\s*id:\s*" + re.escape(معرف) + r"\s*\n\s*canonical:\s*\"([^\"]+)\"",
        نص,
        re.MULTILINE,
    )
    if not مطابقة:
        print("::error::لا اسمَ قانونيًّا لـ / no canonical name for: " + معرف)
        sys.exit(1)
    return مطابقة.group(1)


def استخرج_جدول_الاعلام(نص):
    """(AR) SAD_VERSION_FLAGS من سكريبت الشِّل. (EN) SAD_VERSION_FLAGS from the shell script."""
    مطابقة = re.search(r'^SAD_VERSION_FLAGS="([^"]*)"', نص, re.MULTILINE)
    if not مطابقة:
        print("::error::SAD_VERSION_FLAGS غيرُ موجودٍ / not found in release_tools.sh")
        sys.exit(1)
    نتيجة = {}
    for زوج in مطابقة.group(1).split():
        اسم, _, علم = زوج.partition(":")
        نتيجة[اسم] = علم
    return نتيجة


def استخرج_قائمة_الالزام(نص):
    مطابقة = re.search(r'^SAD_REQUIRED_FULL="([^"]*)"', نص, re.MULTILINE)
    if not مطابقة:
        print("::error::SAD_REQUIRED_FULL غيرُ موجودٍ / not found in release_tools.sh")
        sys.exit(1)
    return مطابقة.group(1).split()


def استخرج_دخان_الهب(نص):
    """(AR) أعلامُ الماكرو: sad_add_tool_smoke_tests(هدف اسم "نمط" علم_إصدار علم_مساعدة).
    (EN) Macro flags: sad_add_tool_smoke_tests(target name "regex" version_flag help_flag)."""
    نتيجة = {}
    for مطابقة in re.finditer(
        r"sad_add_tool_smoke_tests\(\s*(\S+)\s+\S+\s+\"[^\"]*\"\s+(\S+)\s+(\S+)\s*\)", نص
    ):
        نتيجة[مطابقة.group(1)] = مطابقة.group(2)
    if not نتيجة:
        print("::error::لا نداءَ واحدًا للماكرو / no macro call found in the hub CMakeLists")
        sys.exit(1)
    return نتيجة


def يستشير_الجدول(اسم_منشور):
    مسار_نسبي = مصادر_الأدوات.get(اسم_منشور)
    if مسار_نسبي is None:
        print(
            "::error::أداةٌ موعودةٌ بلا مُوَضِّعِ مصدرٍ في هذا الحارس / "
            "promised tool with no source locator here: " + اسم_منشور
        )
        sys.exit(1)
    return bool(أثر_الجدول_المولد.search(اقرأ(جذر / مسار_نسبي)))


def main():
    نص_الحقيقة = اقرأ(مسار_مصدر_الحقيقة)
    نص_الجدول = اقرأ(مسار_جدول_الأدوات)

    قانوني = استخرج_الاسم_القانوني(نص_الحقيقة, "flag.version")
    أعلام = استخرج_جدول_الاعلام(نص_الجدول)
    مطلوبة = استخرج_قائمة_الالزام(نص_الجدول)
    دخان = استخرج_دخان_الهب(اقرأ(مسار_دخان_الهب))
    قانونية = استخرج_كل_الاعلام_القانونية(نص_الحقيقة)

    خلاف = []
    محرّكات = 0

    for منشور in مطلوبة:
        if منشور not in أعلام:
            خلاف.append(
                "  release_tools.sh · لا إملاءَ لأداةٍ موعودة / no spelling for a promised tool: "
                + منشور
            )
            continue

        محرّك = يستشير_الجدول(منشور)
        محرّكات += 1 if محرّك else 0

        # (AR) الطرفُ الثاني: ما يُهجّئُه جدولُ الإصدار.
        # (EN) End two: what the release table spells.
        if محرّك and أعلام[منشور] != قانوني:
            خلاف.append(
                "  release_tools.sh · " + منشور + " يستشيرُ الجدولَ المولَّدَ فيلزمُه «"
                + قانوني + "» ووجدنا «" + أعلام[منشور] + "»"
                " / consults the generated table, must use the canonical name"
            )
        if not محرّك and أعلام[منشور] == قانوني:
            خلاف.append(
                "  release_tools.sh · " + منشور + " لا يستشيرُ الجدولَ المولَّدَ فلا يقبلُ «"
                + قانوني + "» / does not consult the generated table, cannot accept it"
            )

        # (AR) الطرفُ الثالث: ما يُهجّئُه ماكرو الدُّخان. والأداةُ قد لا تُختبَرُ
        #      هناك أصلًا، وهذا مسموح؛ الممنوعُ أن تُختبَرَ بإملاءٍ آخر.
        # (EN) End three: what the smoke macro spells. A tool need not be tested
        #      there at all; what is forbidden is testing it with another spelling.
        if منشور in دخان and دخان[منشور] != أعلام[منشور]:
            خلاف.append(
                "  hub/CMakeLists.txt · " + منشور + " يُختبَرُ بـ«" + دخان[منشور]
                + "» وجدولُ الإصدارِ يقولُ «" + أعلام[منشور] + "»"
                " / smoke-tested with a spelling the release table contradicts"
            )

    # (AR) حارسٌ لا يرى محرّكًا واحدًا لم يقسْ شيئًا: لو تغيّرَ اسمُ النوعِ
    #      المولَّدِ لصارَ كلُّ أداةٍ «غيرَ محرّك» وسكتَ الحارسُ أخضرَ عن كلِّ شيء.
    # (EN) A guard that sees zero engines measured nothing: if the generated
    #      type were renamed, every tool would read as "not an engine" and the
    #      guard would fall silent, green, about everything.
    # (AR) ⑤ المثالُ المنثورُ عقدٌ يقرؤه إنسانٌ أو وكيلٌ ثمّ ينفّذه. فإن خالفَ
    #      مصدرَ الحقيقةِ فهو أمرٌ يفشل — والنثرُ لا يُبنى فلا يُمسِكُه مُصرِّف.
    # (EN) A prose example is a contract a human or agent then executes. Prose
    #      is not compiled, so no compiler catches its drift.
    for ملف, رقم, عَلَم in امسح_النثر(قانونية):
        خلاف.append(
            "  " + ملف + ":" + str(رقم) + " · عَلَمٌ لا وجودَ له «" + عَلَم
            + "» — لا مرادفَ إنجليزيًّا في اللغة"
            " / flag absent from the SoT; the language has no English aliases"
        )

    # (AR) حارسٌ لا يقرأُ اسمًا قانونيًّا واحدًا لم يقسْ شيئًا: لو تغيّرَ مفتاحُ
    #      `canonical` لخلَتِ المجموعةُ وصارَ كلُّ عَلَمٍ منثورٍ «مخالفًا».
    # (EN) Zero canonical names read means the extractor broke, not that every
    #      prose flag is wrong.
    if not قانونية:
        print(
            "::error::لم يُقرَأْ اسمٌ قانونيٌّ واحدٌ من cli_flags.yaml — المُستخرِجُ "
            "انكسرَ ولم ينكسرِ النثرُ / no canonical name parsed; the extractor broke"
        )
        return 1

    if محرّكات == 0:
        print(
            "::error::لا أداةَ واحدةً تستشيرُ الجدولَ المولَّد — أثرُ «FlagAction» لم يعُدْ "
            "يُطابِقُ شيئًا، فالحارسُ يقيسُ فراغًا / no tool consults the generated table; "
            "the FlagAction trace matches nothing and this guard is measuring emptiness"
        )
        return 1

    if خلاف:
        print(
            "::error file=scripts/ci/release_tools.sh::"
            "إملاءُ عَلَمِ الإصدارِ يخالفُ مصدرَ الحقيقة / version-flag spelling contradicts the SoT"
        )
        for سطر in خلاف:
            print(سطر)
        print(
            "   مصدرُ الحقيقة: language-truth/cli_flags.yaml · flag.version · canonical = "
            + قانوني
        )
        return 1

    print(
        "✅ حارسُ الأعلام: " + str(len(قانونية))
        + " اسمًا قانونيًّا، وكلُّ عَلَمٍ منثورٍ في المهاراتِ والوثائقِ يطابقُه"
        " / canonical names; every prose flag matches the SoT"
    )
    print(
        "✅ حارسُ إملاءِ عَلَمِ الإصدار: "
        + str(len(مطلوبة))
        + " أداةً موعودةً، منها "
        + str(محرّكات)
        + " محرّكًا ملزَمًا بـ«"
        + قانوني
        + "» / promised tools, of which engines are bound to the canonical name"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
