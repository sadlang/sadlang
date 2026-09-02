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

     سبعُ طبقاتٍ لا يقيسُ اتّساقَها بناءٌ ولا مُصرِّف — وهذا شرطُ الحارس:
       ① language-truth/cli_flags.yaml      (YAML)  ← الاسمُ القانونيّ
       ② مصدرُ كلِّ أداة                     (C++)   ← أتستشيرُ الجدولَ؟
       ③ scripts/ci/release_tools.sh        (شِل)   ← SAD_VERSION_FLAGS
       ④ tests/system/hub/CMakeLists.txt    (CMake) ← أعلامُ الدُّخان
       ⑤ الوثائقُ والمهاراتُ والمصادرُ (شجر_النثر) ← أعلامُ الأمثلة
          (عربيّةً وإنجليزيّةً: العَلَمُ العربيُّ هو الأرجحُ خطأً بعدَ التعريب)
       ⑥ الترويسةُ × سطرُ الطباعةِ × FindSad × الجدول ← سطرُ الإصدارِ المطبوع
          (ويُقاسُ الطرَفانِ معًا: العَلَمُ الذي يُنادى به، والتعبيرُ الذي يقرأ)
       ⑦ print_help في compiler_driver_cli.cpp   ← نثرُ الاستعمالِ في الثنائيّ

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

     Seven layers whose agreement no build ever checks — the guard's point.
"""

import io
import re
import unicodedata
import sys
from pathlib import Path

# (AR) المُشغِّلُ قد يكون ترميزُه cp1252/cp1255، فتُسقِطُ «✅» السكربتَ
#      بـUnicodeEncodeError ويُقرَأُ الانهيارُ إخفاقَ حراسة. يُثبَّتُ الترميزُ
#      هنا لا في سيرِ العمل: الأداةُ تحملُ شرطَ تشغيلِها معها.
# (EN) The console may be cp1252; a check mark would crash the script and the
#      crash would read as a guard failure. Pin the encoding in the tool itself.
for _قناة in (sys.stdout, sys.stderr):
    if hasattr(_قناة, "reconfigure"):
        _قناة.reconfigure(encoding="utf-8")

جذر = Path(__file__).resolve().parents[2]

مسار_مصدر_الحقيقة = جذر / "language-truth" / "cli_flags.yaml"
مسار_جدول_الأدوات = جذر / "scripts" / "ci" / "release_tools.sh"
مسار_دخان_الهب = جذر / "tests" / "system" / "hub" / "CMakeLists.txt"
مسار_اسم_المترجم = جذر / "tools" / "compiler" / "compiler_driver.h"
مسار_باحث_cmake = جذر / "tools" / "build" / "FindSad.cmake"
مسار_طباعة_الإصدار = جذر / "tools" / "compiler" / "compiler_driver_cli.cpp"

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
# (AR) 🔑 الحدُّ مقيسٌ لا مفترَض. كان المسحُ مُرشَّحًا بالامتدادِ `.md` وحدَه
#      فأفلتَ ملفٌّ يحملُ أمرًا: `runtime/.../kernel_x86_64.ld` كان يُعلِّمُ
#      `sadc --freestanding --linker-script=…` ولا وجودَ لأيٍّ منهما اليوم.
#      وقِيسَ التوسيعُ قبلَ كتابتِه، فرُدَّت ثلاثُ توسعاتٍ بأثرٍ لا برأي:
#      · إسقاطُ شرطِ اسمِ المحرّك ⇒ خلافٌ بآلافٍ صدارتُه ليست أعلامًا أصلًا:
#        فواصلُ Markdown (`---`) وأخواتُها من الشُّرَط، ثمّ أعلامُ أدواتٍ
#        أخرى (`--grep` · `--debug`). فالشرطُ يبقى. ولا يُكتَبُ العدد.
#      · `_bmad-output/**` ⇒ سجلُّ تشغيلٍ ماضٍ لا أمرٌ يُنفَّذ، وسطورُه تخلطُ
#        أعلامَ `runner.py` باسمِ المحرّك (`--level` · `--dir`) — خارجَ المسح.
#      · سكربتاتُ الشِّلِّ للمثبِّتَين ⇒ تُجرِّبُ الإملاءَين عمدًا وتشرحُ ذلك
#        نثرًا، فالمسحُ يقرأُ شرحَ العطبِ عطبًا. خارجَ المسح، وعُولجت باليد.
# (EN) The boundary is measured, not assumed. Filtering by the `.md` extension
#      alone let a file carrying a command escape. Three widenings were
#      measured and rejected by evidence: dropping the engine-name condition
#      (thousands of hits, led by Markdown rules `---` and then by other
#      tools' flags such as --grep — not by ours); _bmad-output (a record, not an
#      instruction); installer shell scripts (they try both spellings on
#      purpose and say so in prose, which the sweep would read as a defect).
# (AR) 🔑 والمصدرُ نثرٌ أيضًا: خمسةُ أعلامٍ ميّتةٍ في أربعةِ ملفّاتِ ترويسةٍ
#      عاشت خارجَ المسح — `--freestanding` في `stdlib/.../uefi.h` وفي
#      `runtime/freestanding/sad_lowlevel_rt.h`، و`--emit-llvm` في
#      `apps/sad-build/main.cpp` (وهو عينُ العَلَمِ الذي وُلدَ هذا المُحلِّلُ
#      من أجلِه، سُدَّ في `.md` وتُرِكَ في `.cpp`)، و`--gc/--learn/--prod` في
#      `compiler_driver.h`. وتعليقُ الرأسِ أخطرُ من الوثيقة: يقرؤه من يُعدِّل.
#      🔑 ثمّ سدَّ التوسيعُ الترويسةَ وتركَ **شقيقَها في المجلَّدِ نفسِه**:
#      `sad_lowlevel_rt.c` بجانبِ `.h` بالنصِّ ذاتِه، لأنّ `.c` لم تكن لاحقةً
#      ممسوحة. ولاحقةُ `CMakeLists.txt` هي `.txt` لا `.cmake`، فكلُّ نثرِ
#      CMake كان أعمى — وفيه `--prod` حيًّا في ملفَّي `shared/`، وجدَتْهما
#      اليدُ لا الحارس. فيُسأَلُ الاسمُ كما تُسأَلُ اللاحقة.
#      ولا يُكتَبُ عددُ الملفّاتِ هنا: الحارسُ يطبعُ العددَ الحيَّ في سطرِه.
# (EN) Source is prose too: five dead flags in four header files lived outside
#      the sweep, one of them the very flag this analyzer was born for. Then
#      the widening sealed a header and left its SIBLING in the same folder
#      (.c was not a swept suffix), and CMakeLists.txt ends in .txt, not
#      .cmake — so every CMake comment was blind, and --prod lived on in two
#      shared/ files that a human, not this guard, found. Names are asked for
#      alongside suffixes. No file count is written here; the guard prints the
#      live one.
شجر_النثر = (".github/skills", "docs", "tests", "runtime",
             "stdlib", "apps", "shared", "compiler", "tools")
لواحق_النثر = (".md", ".ld", ".h", ".c", ".cpp", ".cmake")
أسماء_النثر = ("CMakeLists.txt",)
# (AR) الأرشيفُ يصفُ ماضيًا عمدًا؛ إدانتُه تُلزِمُ بتزييفِ سجلّ. وهو اليومَ
#      **وقائيٌّ لا علاجيّ**: قِيسَ بتعطيلِه فبقيَ الحارسُ أخضر — أي إنّ
#      المؤرشَفَ المشمولَ بالمسحِ لا عطبَ فيه. يُبقى لأنّ الأرشيفَ يُكتَبُ
#      ليُحفَظ لا ليُصحَّح. والمطابقةُ على **جزءٍ كاملٍ من المسار** فلا تبتلعُ
#      «legacy_archive».
#      🔑 و`language-truth/` ليست في `شجر_النثر` — **لا سطرَ يستثنيها**، وهذا
#      وصفُ حالٍ لا عقد. فمن وسّعَ إليها غدًا فليعلمْ أنّ
#      `grammar/DISCOVERED_ISSUES.md` يذكرُ `--freestanding` **ليَنفيَه** («غيرُ
#      معروفٍ اليوم»)، فيلزمُه استثناءٌ مكتوبٌ لا عزمٌ منثور.
# (EN) An archive deliberately describes the past. Measured by disabling it:
#      the guard stayed green, so this is preventive, not curative. Matching is
#      on a whole path part, so "legacy_archive" is not swallowed. For the same
#      reason, note that language-truth/ is simply absent from شجر_النثر —
#      no line excludes it. Whoever widens there will need a written exclusion
#      for grammar/DISCOVERED_ISSUES.md, which names --freestanding to deny it.
جزء_مستثنى = "_archive"
# (AR) 🔑 وحُذِفَ هنا مُوَضِّعٌ مستقلٌّ لملفّاتِ `tools/build/*.cmake`: يومَ
#      كُتِبَ كانت `tools` خارجَ الأشجارِ و`.cmake` خارجَ اللواحق، فلمّا دخلتا
#      صارَ تقاطعُه بالمسحِ **ثلاثةً من ثلاثة**. وأثرُه مقيسٌ بالتنفيذِ لا
#      بالنظر: عَلَمٌ وهميٌّ واحدٌ في `SadProject.cmake` كان يُنتِجُ **تعليقَين**
#      على السطرِ نفسِه، والعددُ المطبوعُ يزيدُ ثلاثةً على المقيسِ فعلًا. أي
#      إنّ الفرعَ الميّتَ لم يسكتْ فحسبُ، بل كذّبَ العدَّ وضاعفَ الإدانة.
# (EN) A separate locator for tools/build/*.cmake was removed here: it was
#      written when tools/ and .cmake were both outside the sweep, and once
#      they were added it overlapped 3-of-3. Measured by execution: one seeded
#      flag produced TWO comments on the same line, and the printed file count
#      ran three ahead of what was actually scanned.
# (AR) قِيسَ: بديلُ `${SADC_EXECUTABLE}` صفرُ إصابةٍ عبرَ كلِّ الملفّاتِ
#      الممسوحة، ولا يلتقطُ حتّى السطرَ الذي كُتِبَ له (يشترطُ اسمَ الأداةِ
#      ملاصقًا لـ`}`). وذاك السطرُ محروسٌ في مكانِه بـ`استخرج_علم_الباحث`،
#      فالبديلُ فرعٌ يدّعي غرضًا لا يؤدّيه — وحُذف.
# (EN) Measured: the ${SADC_EXECUTABLE} alternative never matched anything,
#      not even the line it was written for; that line is guarded by
#      استخرج_علم_الباحث instead. A branch claiming a purpose it never served.
المحرّكات_في_النثر = re.compile(
    r"(?:^|[\s`(])"
    r"(?:sad-build|sad-run|sadc)(?=[\s`)،,.:]|$)")
# (AR) الأوامرُ الفرعيّةُ (`sadc pkg …` · `sadc ui …`) تُحلَّلُ في مُرسِلٍ آخرَ
#      ولا وجودَ لها في `cli_flags.yaml`، فأعلامُها خارجَ عهدةِ هذا الحارس.
#      🔑 وقائمتُها **تُشتَقُّ من مواضعِ الإرسالِ نفسِها** لا تُكتَبُ يدًا. كانت
#      مكتوبةً، فكانت خاطئةً في الاتّجاهَين معًا — وهو ما تُدينُه ترويسةُ هذا
#      الملفِّ حرفيًّا:
#        · زائدةً: فيها `run` و`init` و`add` و`test` و`bench` ولا مُرسِلَ
#          لواحدٍ منها في المستودعِ كلِّه. فكانت تبتلعُ السطرَ كلَّه، ويُفلِتُ
#          `sadc test x.ص --emit-llvm` أخضرَ — عَلَمٌ ميّتٌ حقيقيّ.
#        · ناقصةً: ولا واحدٌ من المرادفاتِ العربيّة (`حزم` · `واجهة` · `بناء`)،
#          والمستودعُ يعرّبُ أوامرَه، فأوّلُ مثالٍ عربيٍّ يُشعِلُ إنذارًا كاذبًا
#          علاجُه مستحيلٌ: «خُذِ الاسمَ من cli_flags.yaml» وليس فيه.
#      🔑 والاستثناءُ يقعُ على **الأمرِ لا على السطر**: سطرٌ فيه `sadc pkg …`
#          و`sadc --gc …` معًا كان يُفلِتُ كلُّه. فلا يُستثنى إلّا إن كان
#          **كلُّ** ذكرٍ لمحرّكٍ في السطرِ متبوعًا بفعلِ إرسال.
# (EN) Subcommand verbs are DERIVED from the dispatch sites, not hand-listed.
#      The hand-written list was wrong in both directions: it carried five
#      verbs no dispatcher parses (so a real dead flag escaped on their lines)
#      and none of the Arabic synonyms (so the first Arabic example would
#      raise a false alarm whose remedy does not exist). And the exclusion now
#      applies to the COMMAND, not the line: a line mentioning both a
#      subcommand and a bare compiler invocation is no longer swallowed whole.
مواضع_إرسال_الأوامر = (
    "apps/sad-build/main.cpp",
    "tools/compiler/src/cli_commands.cpp",
)
فعل_مُرسَل = re.compile(r'(?:arg1|arg2|sub)\s*==\s*(?:u8)?"([^"]+)"')


def أفعال_الأوامر_الفرعية():
    """(AR) أفعالُ الإرسالِ كما يقرؤها المصدرُ اليوم — لا كما نتذكّرُها.

    (EN) The dispatch verbs as the source spells them today.
    """
    أفعال = set()
    for موضع in مواضع_إرسال_الأوامر:
        مسار = جذر / موضع
        if not مسار.is_file():
            return None
        أفعال |= set(فعل_مُرسَل.findall(مسار.read_text(encoding="utf-8")))
    return أفعال or None
# (AR) أوّلُ لفظٍ في سطرِ استعمالٍ داخلَ `print_help` — هو الأمرُ الموصى به.
#      وبه يُقاسُ أنّه اسمٌ منشور. ولا يُقاسُ بتعبيرِ المحرّكاتِ أعلاه: ذاك
#      قائمةٌ مكتوبةٌ يدًا لا مرجعَ لها في `SAD_TOOL_TABLE`، فيقيسُ إملاءَ
#      نفسِه لا الجدول.
# (EN) The first token of a usage line in print_help is the recommended
#      command. It is not checked against the engine regex: that regex is a
#      hand-written list with no reference in SAD_TOOL_TABLE, so it would
#      measure its own spelling rather than the table.
أمر_في_سطر_استعمال = re.compile(r'"\s{2,}([A-Za-z][A-Za-z0-9_-]*)[ \[]')

# (AR) نستثني أسطرَ أدواتٍ أخرى تحمل الاسمَ عرَضًا (cmake --build --target sad-build).
# (EN) Exclude lines where the name is a build target, not an invocation.
سطر_بناء = re.compile(r"\bcmake\b|--target\b|add_executable|find_program")
# (AR) 🔑 أعلامُ اللغةِ كلُّها عربيّة، وماسحُ ASCII وحدَه يحرسُ النثرَ ضدَّ
#      المرادفِ الإنجليزيِّ فقط ويتركُه **مكشوفًا صفرًا** أمامَ خطأٍ إملائيٍّ
#      عربيّ — وهو الأرجحُ بعدَ تعريبِ الأمثلةِ في هذه الرقعةِ نفسِها.
#      🔑 والجسمُ يُلتقَطُ بصنفٍ **موجَب** لا بـ«كلِّ شيءٍ إلّا»: الثانيةُ تنسى
#      فاصلًا فتُدينُ عَلَمًا قانونيًّا كُتِبَ بينَ «…» أو تلاه «؛» أو «؟».
#      وماسحٌ واحدٌ للإملاءَين يمنعُ ازدواجَ الإدانةِ وكذِبَ العلّةِ معه.
#      وتُضَمُّ العلاماتُ اللاصقةُ صراحةً: الشدّةُ ليست `\w`، ولو سقطت من
#      الالتقاطِ لصارَ «--حرّ» القانونيُّ يُقرَأُ «--حر» فيُدان. والعكسُ أخطر:
#      علامةٌ خارجَ الصنفِ **تبترُ** العَلَمَ إلى قانونيٍّ فيسكتُ الحارس. ولا
#      يصلحُ مثالًا لذلك «--إصدارٰ» بالألفِ الخنجريّة: هي **داخلَ** الصنفِ
#      فيُلتقَطُ العَلَمُ كاملًا ويُدانُ بمخالفتِه القائمةَ، لا بالبتر. والحالةُ
#      الحيّةُ مقيسةٌ بـ`U+0888` (النقطةُ المرفوعة، فئةُ `Sk`) — كانت تمرُّ
#      أخضرَ حتّى صارَ الحكمُ بالفاصلِ المشروعِ أدناه.
#      🔑 والتعدادُ اليدويُّ لمدياتِ Unicode قائمةُ إذنٍ تتعفّنُ مع كلِّ إصدار.
#      وسؤالُ **فئةِ** التالي (Mn · Mc · Me · Cf) قائمةُ إذنٍ أخرى بثوبٍ آخر:
#      قِيسَ فأفلتَ منها `Sk` — «النقطةُ العربيّةُ المرفوعة» (U+0888) وأسرةُ
#      U+FBB2‥FBC2 — وكلُّها تلتصقُ بصريًّا ويمرّرها الصَّدفُ داخلَ الوسيطِ
#      نفسِه، فيصلُ إلى `parse_option` وسيطٌ يخالفُ `canonical` بايتيًّا.
#      🔑 فالمرساةُ الفعلُ لا إملاؤه: العَلَمُ **مبتورٌ ما لم** يتلُه فاصلٌ
#      مشروعٌ (فراغٌ أو ترقيمٌ أو نهايةُ سطر). قائمةُ الفواصلِ تُدينُ حينَ
#      تنسى — والنسيانُ في الاتّجاهِ الآمن — بخلافِ قائمةِ اللواصقِ التي
#      تسكتُ حينَ تنسى. قِيسَ المقلوبُ على المسحِ كلِّه: صفرُ ضجيجٍ جديد.
#      🔑 وعلاماتُ الاتّجاهِ مستثناةٌ صراحةً (200E · 200F · 061C · 2066‥2069):
#      لا تصلُ إلى `argv` أصلًا، وهي طبيعيّةٌ في نصٍّ عربيٍّ يحيطُ بمقطعٍ
#      لاتينيّ. قِيسَ: 130 ملفًّا فيها RLM — وبدونِ الاستثناءِ يحمرُّ الحارسُ
#      على توثيقٍ سليمٍ برسالةٍ تقولُ إنّ `--إصدار` لا وجودَ له، والمحرفُ
#      الجاني غيرُ مرئيٍّ لقارئِ التعليق. ولذا يُطبَعُ رمزُه معه.
#      🔑 ولا تُجرَّدُ الشدّةُ عندَ المقارنة: `CommandLineParser::parse_option`
#      يقارنُ الوسيطَ بـ`spec.canonical` مقارنةَ `std::string` بايتًا ببايت،
#      فـ«--حر» خيارٌ غيرُ معروفٍ لا إملاءٌ آخرُ لـ«--حرّ». وتطبيعُ الشدّةِ
#      درسٌ من المعجمِ لا من سطرِ الأمر.
# (EN) Every canonical flag is Arabic; an ASCII-only scanner leaves the prose
#      wholly unguarded against an Arabic misspelling. The body is matched by a
#      POSITIVE class, not by "anything but": the latter forgets a terminator
#      and then condemns a canonical flag written inside quotes. Arabic
#      combining marks are included explicitly — the shadda is not \w, and
#      dropping one TRUNCATES a flag down to a canonical prefix and silences
#      the guard. A hand-written list of Unicode ranges is an allowlist that
#      rots — and asking the CATEGORY of what follows is the same allowlist in
#      other clothes: Sk (U+0888, U+FBB2..FBC2) escaped it, measured. So the
#      anchor is the act, not its spelling: a flag is TRUNCATED unless a
#      legitimate separator follows. A separator list errs toward condemning;
#      a combining-mark list errs toward silence. Direction marks are exempt —
#      they never reach argv — and the offending code point is printed.
عَلَم_منثور = re.compile(
    r"(?<![\w-])(--[\w\u0610-\u061a\u064b-\u065f\u0670\u06d6-\u06ed-]+)")


def استخرج_كل_الاعلام_القانونية(نص):
    """كلُّ اسمٍ قانونيٍّ وكلُّ عَلَمٍ قصيرٍ في مصدرِ الحقيقة."""
    أسماء = set(re.findall(r'canonical:\s*"([^"]+)"', نص))
    أسماء |= set(re.findall(r'name:\s*"(-[^"]+)"', نص))
    return أسماء


# (AR) ما يجوزُ أن يتلوَ عَلَمًا في نثر: فراغٌ أو ترقيمٌ أو محدِّدُ اقتباس.
# (EN) What may legitimately follow a flag in prose.
فاصل_مشروع = frozenset(
    " \t\r\n`\"'()[]{}<>,;:.!?/\\|=+*&^%$#@~«»؛،؟…—–-"
)
# (AR) علاماتُ اتّجاهٍ لا تصلُ إلى `argv`، فليست بترًا.
# (EN) Direction marks never reach argv, so they are not truncation.
علامات_الاتجاه = frozenset(
    "\u200e\u200f\u061c\u2066\u2067\u2068\u2069"
)


def بُتِرَ_العَلَم(سطر, نهاية):
    """(AR) المحرفُ الذي بترَ العَلَمَ، أو None إن تلاه فاصلٌ مشروع.

    (EN) The character that truncated the flag, or None if a legitimate
    separator followed. Judged by the act (was it terminated?) rather than
    by a rotting list of combining marks.
    """
    if نهاية >= len(سطر):
        return None
    تال = سطر[نهاية]
    if تال in علامات_الاتجاه or تال in فاصل_مشروع:
        return None
    return تال


def وصف_العلم(عَلَم, جانٍ):
    """(AR) وصفُ الإدانةِ يُسمّي المحرفَ الجانيَ حينَ لا يراه قارئُ التعليق.

    🔑 وبدونِه يقرأُ المرءُ على GitHub سطرًا يبدو سليمًا وحُكمًا بالإدانة،
    فيُصلِحُ ما ليس مكسورًا.
    (EN) The verdict names the offending character when it is invisible;
    without it a reader sees an apparently correct line and a condemnation.
    """
    if جانٍ:
        return ("عَلَمٌ مبتورٌ «" + عَلَم + "» بمحرفٍ لاصقٍ " + جانٍ
                + " / flag truncated by " + جانٍ)
    return "عَلَمٌ لا وجودَ له «" + عَلَم + "» / unknown flag"


def أعلام_السطر(سطر, قانونية):
    """(AR) كلُّ عَلَمٍ في السطرِ لا يطابقُ مصدرَ الحقيقة — عربيًّا كان أو إنجليزيًّا.

    (EN) Every flag on the line that contradicts the SoT, Arabic or English.
    """
    خارجة = []
    for مطابقة in عَلَم_منثور.finditer(سطر):
        عَلَم = مطابقة.group(1)
        جانٍ = بُتِرَ_العَلَم(سطر, مطابقة.end())
        if جانٍ is not None:
            خارجة.append((عَلَم, "U+%04X" % ord(جانٍ)))
        elif عَلَم not in قانونية:
            خارجة.append((عَلَم, None))
    return خارجة


def امسح_النثر(قانونية):
    """(AR) (ملفّ، سطر، عَلَم) لكلِّ عَلَمٍ منثورٍ لا يطابقُ مصدرَ الحقيقة.

    🔑 وشجرةٌ مفقودةٌ ليست شجرةً سليمة، و**شجرةٌ موجودةٌ ليست ملفّاتٍ مُسِحت**:
    مجلَّدٌ فارغٌ أو لاحقةٌ لا تطابقُ شيئًا يُطفئانِ المُحلِّلَ الخامسَ كلَّه
    ويُبقيانِ الشوطَ أخضر. فيُرفَعُ الغيابُ وتُرفَعُ القائمةُ الفارغةُ معًا،
    ويُعادُ العددُ ليقولَ الأخضرُ ما قاسَه لا أنّه اطمأنّ.
    (EN) A missing tree is not a healthy tree, and an existing tree is not
    scanned files: an empty directory or a suffix matching nothing switches
    this analyzer off. Both are raised, and the count is returned so the green
    line states what it measured.
    """
    مسارات = []
    مفقودة = [شجرة for شجرة in شجر_النثر if not (جذر / شجرة).is_dir()]
    if مفقودة:
        raise FileNotFoundError(", ".join(مفقودة))
    for شجرة in شجر_النثر:
        جذر_الشجرة = جذر / شجرة
        مسارات.extend(
            مسار_مرشَّح for مسار_مرشَّح in sorted(جذر_الشجرة.rglob("*"))
            if (مسار_مرشَّح.suffix in لواحق_النثر
                or مسار_مرشَّح.name in أسماء_النثر)
            and جزء_مستثنى not in مسار_مرشَّح.parts
        )
    # (AR) 🔑 والأرضيّةُ **حاصلُ ضربٍ لا مجموع**: مجموعُ الملفّاتِ يُنقِذُه
    #      أكبرُ موضِّعٍ فيه. قِيسَ مرّتَين: ملفّاتُ الـ.cmake الثلاثةُ الثابتةُ
    #      كانت تُنقِذُ شرطَ الأشجار، ثمّ شجرةُ المهاراتِ كانت تُنقِذُ إسقاطَ
    #      لاحقةِ `.ld` كلِّها — وهي سببُ وجودِ هذا التوسيعِ أصلًا. فيُسأَلُ
    #      كلُّ موضِّعٍ على حدةٍ: شجرةٌ بصفرِ ملفّاتٍ أو لاحقةٌ بصفرِ إصاباتٍ
    #      تعني أنّ جزءًا من العقدِ لم يُقَسْ، لا أنّه سليم.
    #      🔑 والمطابقةُ على **المسارِ كاملًا** لا على جزئِه الأوّل: قِيسَ
    #      بحقنِ `.github/workflows` — وفيها صفرُ ملفّاتٍ بلواحقِ المسح — فمرَّ
    #      أخضرَ لأنّ `.github/skills` أنقذَته بجزئِهما الأوّلِ المشترك. أي إنّ
    #      الأرضيّةَ التي كُتِبَت لتمنعَ «أكبرَ موضِّعٍ يُنقِذُ المجموع» كانت
    #      تحملُ العطبَ نفسَه داخلَها.
    # (EN) The floor is a PRODUCT, not a sum: a total is rescued by its largest
    #      locator. Measured twice — three fixed .cmake files rescued the tree
    #      condition, then the skills tree rescued dropping the .ld suffix
    #      entirely, the very suffix this widening exists for. And matching is
    #      on the WHOLE path, not its first part: measured by injecting
    #      .github/workflows (zero matching files), which passed green because
    #      .github/skills rescued it through their shared first part — the very
    #      defect this floor was written to prevent, inside the floor itself.
    def تحت_الشجرة(نسبي, شجرة):
        return نسبي == شجرة or نسبي.startswith(شجرة + "/")

    نسبيّة = [مسار.relative_to(جذر).as_posix() for مسار in مسارات]
    فارغة = [شجرة for شجرة in شجر_النثر
             if not any(تحت_الشجرة(ن, شجرة) for ن in نسبيّة)]
    بلا_ملفّ = [موضِّع for موضِّع in لواحق_النثر + أسماء_النثر
                if not any(مسار.suffix == موضِّع or مسار.name == موضِّع
                           for مسار in مسارات)]
    if not مسارات or فارغة or بلا_ملفّ:
        raise FileNotFoundError(
            "; ".join(filter(None, (
                "شجرةٌ بصفرِ ملفّات: " + ", ".join(فارغة) if فارغة else "",
                "مُوَضِّعٌ بصفرِ ملفّات: " + ", ".join(بلا_ملفّ) if بلا_ملفّ else "",
                "لا ملفَّ واحدًا ممسوحًا" if not مسارات else "",
            )))
        )
    أفعال = أفعال_الأوامر_الفرعية()
    if not أفعال:
        raise FileNotFoundError(
            "لا فعلَ إرسالٍ واحدًا في " + ", ".join(مواضع_إرسال_الأوامر)
        )
    # (AR) يُطابَقُ ما **يلي** اسمَ المحرّكِ مباشرةً، فيُحكَمُ على كلِّ ذكرٍ
    #      على حدةٍ لا على السطرِ كتلةً واحدة.
    # (EN) Matched against what directly FOLLOWS the engine name, so each
    #      mention is judged on its own rather than the line as a block.
    أمر_فرعي = re.compile(
        r"\s+(?:" + "|".join(sorted(map(re.escape, أفعال))) + r")(?![\w-])"
    )
    خلاف = []
    for مسار in مسارات:
        try:
            سطور = مسار.read_text(encoding="utf-8").splitlines()
        except (OSError, UnicodeDecodeError):
            continue
        for رقم, سطر in enumerate(سطور, 1):
            ذكر = list(المحرّكات_في_النثر.finditer(سطر))
            if not ذكر or سطر_بناء.search(سطر):
                continue
            if all(أمر_فرعي.match(سطر, م.end()) for م in ذكر):
                continue
            for عَلَم, جانٍ in أعلام_السطر(سطر, قانونية):
                خلاف.append((مسار.relative_to(جذر).as_posix(), رقم, عَلَم, جانٍ))
    return خلاف, len(مسارات)


def استخرج_اسم_المترجم_المطبوع(نص):
    """(AR) الاسمُ الذي يطبعُه الثنائيُّ في سطرِ الإصدار. (EN) The name the binary prints."""
    مطابقة = re.search(
        r"get_compiler_name\(\)\s*\{\s*return\s+\"([^\"]+?)\s+-\s+Sad Compiler\"", نص
    )
    return مطابقة.group(1) if مطابقة else None


def استخرج_عائد_اسم_المترجم(نص):
    """(AR) العائدُ **كاملًا** من `get_compiler_name()` — لا اسمُ الأداةِ وحدَه.

    🔑 وبه يُركَّبُ السطرُ المتوقَّع، فلا تُلصَقُ لاحقةُ « - Sad Compiler» يدًا
    مرّةً أخرى: كانت نسخةً رابعةً من السلسلةِ داخلَ أداةٍ تُدينُ النُّسَخ.
    والاسمُ المنشورُ يبقى مقصوصًا لأنّ `SAD_TOOL_TABLE` تنشرُ «sadc» لا العائد.
    (EN) The FULL return of get_compiler_name(), used to compose the expected
    line so its suffix is not re-pasted by hand — that was a fourth copy of
    the same string inside a tool written to condemn copies. The published
    name stays trimmed, since SAD_TOOL_TABLE publishes "sadc", not the return.
    """
    مطابقة = re.search(
        r"get_compiler_name\(\)\s*\{\s*return\s+\"([^\"]+)\"", نص
    )
    return مطابقة.group(1) if مطابقة else None


def استخرج_اصدار_المترجم(نص):
    """(AR) الإصدارُ الذي يطبعُه الثنائيّ. (EN) The version the binary prints."""
    مطابقة = re.search(r"get_version\(\)\s*\{\s*return\s+\"([^\"]+)\"", نص)
    return مطابقة.group(1) if مطابقة else None


def استخرج_مدى_المساعدة(نص):
    """(AR) مدى أسطرِ `print_help` المكتوبةِ يدًا — قبلَ الحلقاتِ المولَّدة.

    🔑 وأسطرُ الاستعمالِ والأمثلةِ فيها **نثرٌ داخلَ الثنائيّ**: يقرؤها
    المستخدمُ ثمّ ينفّذُها، فهي عقدٌ كنثرِ المهاراتِ سواءً بسواء. وقسمُ
    «الخيارات» تحتَها مُشتقٌّ من الجدولِ المولَّدِ فلا يُقاس — لا يُكتَبُ يدًا.
    (EN) The usage/examples block is prose INSIDE the binary: a user reads it
    and then runs it. The options section below it is derived from the
    generated table, so it is not hand-written and not measured here.
    """
    بداية = نص.find("void CompilerDriver::print_help")
    if بداية < 0:
        return None
    مؤشر = نص.find("Options / الخيارات", بداية)
    if مؤشر < 0:
        return None
    سطر_البداية = نص.count("\n", 0, بداية) + 1
    نهاية = نهاية_الجسم(نص, سطر_البداية)
    if نهاية is None:
        return None
    # (AR) 🔑 والمرساةُ لا تُصدَّقُ على علّاتِها: لو نُقِلَ قسمُ «الخيارات» خارجَ
    #      الدالّةِ لالتُقِطَ ورودُه في دالّةٍ تالية، فامتدَّ المدى عابرًا
    #      دالّتَين وعدَّ أسطرًا ليست من `print_help` أصلًا — فيصيرُ المقيسُ
    #      **أكثرَ** من الكاملِ ويمرُّ شرطُ «أقلّ» أخضر. فيُقَصُّ عندَ القوسِ.
    # (EN) The anchor is not trusted on its own: if the Options section moves
    #      out of the function, its next occurrence stretches the range across
    #      two functions and the measured count exceeds the body's — which a
    #      "fewer than" test passes green. Clamp to the closing brace instead.
    return (سطر_البداية, min(نص.count("\n", 0, مؤشر) + 1, نهاية))


# (AR) القوسُ داخلَ سلسلةٍ نصّيّةٍ ليس قوسَ كتلة. وسطرُ المساعدةِ مليءٌ
#      بالسلاسل، فعدُّ الأقواسِ خامًا يُنهي الجسمَ حيثُ لا ينتهي.
# (EN) A brace inside a string literal is not a block brace.
سلسلة_في_سطر = re.compile(r'"(?:[^"\\]|\\.)*"')


def نهاية_الجسم(نص, سطر_البداية):
    """(AR) السطرُ المُغلِقُ لجسمِ الدالّة، أو None إن لم يُغلَقْ داخلَ الملفّ.

    (EN) The line closing the function body, or None if it never closes.
    """
    سطور = نص.splitlines()
    عمق = 0
    بدأ = False
    for رقم in range(سطر_البداية, len(سطور) + 1):
        سطر = سلسلة_في_سطر.sub('""', سطور[رقم - 1])
        عمق += سطر.count("{") - سطر.count("}")
        if not بدأ and "{" in سطر:
            بدأ = True
        elif بدأ and عمق <= 0:
            return رقم
    return None


def نهاية_الدالّة_التالية(نص, سطر_البداية):
    """(AR) 🔑 مقامٌ **مستقلٌّ** عن `نهاية_الجسم`: أوّلُ تعريفِ دالّةٍ تالية.

    وبه يُقاسُ ما يجبُ أن يُقرَأ، فلو خدعَ قوسٌ `نهاية_الجسم` هبطَ البسطُ
    وحدَه واحمرَّ الحارس. وكانت الدالّتانِ تستدعيانِ `نهاية_الجسم` نفسَها،
    فيهبطُ الطرَفانِ معًا إلى ٠ = ٠ ويمرُّ أخضرَ وقد قاسَ صفرًا من ستّة —
    وهو عينُ «حارسٍ طرفاه من أصلٍ واحدٍ يقيسُ نفسَه» في ترويسةِ هذا الملفّ.
    (EN) A denominator INDEPENDENT of نهاية_الجسم: the next function definition.
    Both used to come from the same source, so a brace inside a string collapsed
    numerator and denominator together to 0 == 0 and the guard passed green.
    """
    سطور = نص.splitlines()
    for رقم in range(سطر_البداية + 1, len(سطور) + 1):
        if re.match(r"\s*(?:[\w:<>,&*~\s]+\s)?CompilerDriver::", سطور[رقم - 1]):
            return رقم - 1
    return len(سطور)


def عدّ_أسطر_الاستعمال(نص, سطر_البداية):
    """(AR) أسطرُ الاستعمالِ في **جسمِ** `print_help` كلِّه، حتّى قوسِه المُطابِق.

    (EN) Usage lines in the whole print_help body, to its matching brace — the
    denominator the measured range is judged against.
    """
    سطور = نص.splitlines()
    نهاية = نهاية_الدالّة_التالية(نص, سطر_البداية)
    return sum(
        1 for رقم in range(سطر_البداية, min(نهاية, len(سطور)) + 1)
        if أمر_في_سطر_استعمال.search(سطور[رقم - 1])
    )


def امسح_المساعدة(نص, مدى, قانونية, منشورة):
    """(AR) خلافاتُ نثرِ المساعدةِ وعددُ أسطرِ الاستعمالِ التي قِيسَتْ فعلًا.

    🔑 والعدَدانِ ليسا زينة: المرساةُ «Options / الخيارات» تلتقطُ **أوّلَ**
    ورودٍ، فإن انتقلَ القسمُ المولَّدُ إلى صدرِ الدالّةِ انهارَ المدى وسقطَتْ
    أمثلةٌ خارجَه. وأرضيّةُ «صفر» لا تكفي: مدًى يبتلعُ سطرًا واحدًا من ستّةٍ
    يمرُّ موجَبًا وهو لم يقرأْ خمسةً. فيُقاسُ **جسمُ الدالّةِ كلُّه** أيضًا،
    وأيُّ نقصانٍ بينَ العدَدَين يستحقُّ «لم يُقَسْ» لا «سليم».
    (EN) Returns both how many usage lines the range measured and how many the
    whole function body holds. A zero floor is not enough: a range that
    swallows one of six lines passes positive having read neither the rest.
    """
    خلافات = []
    مقيسة = 0
    سطور = نص.splitlines()
    كامل = عدّ_أسطر_الاستعمال(نص, مدى[0])
    for رقم in range(مدى[0], min(مدى[1], len(سطور)) + 1):
        سطر = سطور[رقم - 1]
        أمر = أمر_في_سطر_استعمال.search(سطر)
        if أمر:
            مقيسة += 1
        if أمر and أمر.group(1) not in منشورة:
            خلافات.append(
                (رقم, "يوصي بأمرٍ «" + أمر.group(1) + "» ليس اسمًا مُسجَّلًا في "
                 "SAD_TOOL_TABLE. العلاج: في release_tools.sh · SAD_TOOL_TABLE"
                 " / recommends a command name that is not published; remedy: release_tools.sh · SAD_TOOL_TABLE")
            )
        if not المحرّكات_في_النثر.search(سطر):
            continue
        for عَلَم, جانٍ in أعلام_السطر(سطر, قانونية):
            خلافات.append(
                (رقم, وصف_العلم(عَلَم, جانٍ) + " في نثرِ المساعدة."
                 " العلاج: خُذِ الاسمَ من language-truth/cli_flags.yaml · canonical"
                 " / help prose names a flag absent from the SoT; remedy:"
                 " language-truth/cli_flags.yaml · field `canonical`")
            )
    return خلافات, مقيسة, كامل


def استخرج_مفصل_الطباعة(نص):
    """(AR) ما يفصلُ الاسمَ عن الإصدارِ في السطرِ المطبوعِ فعلًا.

    🔑 والترويسةُ ليست السطرَ المطبوع: `get_compiler_name()` تسكنُ في
    `compiler_driver.h` والتركيبُ يقعُ في `compiler_driver_cli.cpp`. فحارسٌ
    يقرأُ الترويسةَ وحدَها يحرسُ نصفَ العقد — من بدّلَ المفصلَ إلى " v" أو
    أسقطَ `get_version()` أصمتَ الباحثَ والحارسُ أخضر. (طبقةٌ تحتَ التي يقرؤها.)
    (EN) The header is not the printed line: the composition happens in the .cpp.
    A guard reading only the header guards half the contract.
    """
    مطابقة = re.search(
        r"get_compiler_name\(\)[^;]*?<<\s*\"([^\"]*)\"\s*<<[^;]*?get_version\(\)", نص
    )
    return مطابقة.group(1) if مطابقة else None


def استخرج_علم_الباحث(نص):
    """(AR) كلُّ عَلَمٍ طويلٍ يُنادي به `FindSad.cmake` المترجمَ فعلًا.

    🔑 وهو الطرَفُ الذي انكسرَ من قبلُ — يشهدُ به تعليقُ الملفِّ نفسِه: كان
    `--version` فيبتلعُ `ERROR_QUIET` خطأَه، فتبقى `Sad_VERSION` فارغةً
    والكتلةُ كلُّها ميّتةً صامتة. وقياسُ `MATCHES` وحدَه يحرسُ نصفَ العقد:
    تعبيرٌ سليمٌ على مخرَجٍ لا يُطبَعُ أصلًا.
    (EN) The flag the finder actually invokes — the end that did break.
    Measuring only the MATCHES regex guards half the contract: a correct
    pattern over output that is never produced.

    🔑 وتُقرَأُ **كلُّ** النداءات: في الملفِّ اليومَ نداءانِ، فالتقاطُ الأوّلِ
    وحدَه حُكمٌ يتبعُ ترتيبَ الأسطرِ لا العقد. والوسيطُ الذي لا يبدأُ بـ«--»
    ليس عَلَمًا (`-c` مثلًا) فلا يُدانُ بإملاءِ عَلَمِ الإصدار.
    (EN) Every invocation is read: judging only the first makes the verdict
    depend on line order. Arguments not starting with "--" are not flags.
    """
    أعلام_منادى_بها = []
    for مطابقة in re.finditer(
        r"(?<![\w])COMMAND\s+\$\{SADC_EXECUTABLE\}\s+([^\s)]+)", نص
    ):
        وسيط = مطابقة.group(1)
        if وسيط.startswith("--"):
            أعلام_منادى_بها.append(وسيط)
    return أعلام_منادى_بها or None


# (AR) تعليقاتُ CMake تُطرَحُ قبلَ القياس: الملفُّ يشرحُ العطبَ الذي أُصلِح
#      («كان `--shared`»)، فقراءةُ التعليقِ إدانةٌ للشرح.
# (EN) CMake comments are stripped first: the file explains the defect it
#      fixed, and reading that explanation would condemn the explanation.
تعليق_cmake = re.compile(r"#.*$", re.MULTILINE)


def أعلام_الباحث_المنفَّذة(نص, قانونية):
    """(AR) كلُّ عَلَمٍ طويلٍ في **كودِ** `FindSad.cmake` — لا في تعليقاتِه.

    🔑 من الأعلامِ التي عُرِّبت في هذه الرقعةِ واحدٌ فقط **يُنفَّذ**:
    `set(SHARED_FLAG --مشترك)`. والبقيّةُ تعليقات. وكان هو وحدَه بلا حارس:
    `أعلام_الباحث` لا يقرأُ إلّا ما يلي `COMMAND ${SADC_EXECUTABLE}` مباشرةً،
    وهذا يُضَمُّ إلى `COMPILE_COMMAND`؛ والمُحلِّلُ الخامسُ يشترطُ اسمَ محرّكٍ
    في السطر، وسطرُ `set(...)` لا يحملُ واحدًا. فكان `--مشتركك` يمرُّ أخضرَ
    وكلُّ `sad_add_library(... SHARED)` يُنتِجُ أمرًا يرفضُه المترجم — أي إنّ
    الرقعةَ حرسَت النثرَ وتركَت العَلَمَ الذي يُنفَّذ.
    (EN) Of the flags this patch arabized, exactly one is EXECUTED, and it was
    the only one nothing guarded: it is appended to COMPILE_COMMAND rather
    than following COMMAND, and its set() line carries no engine name, so both
    existing analyzers missed it. Prose was guarded; the running flag was not.
    """
    # (AR) وتُستثنى سلاسلُ الشُّرَطِ الفاصلة: `-------` ليست عَلَمًا. فيُشتَرَطُ
    #      محرفُ كلمةٍ واحدٌ على الأقلّ بعدَ «--».
    # (EN) Dash rules are not flags: at least one word character is required.
    return [عَلَم for عَلَم, _ in أعلام_السطر(تعليق_cmake.sub("", نص), قانونية)
            if re.search(r"[^\W_]", عَلَم)]


def استخرج_تعبير_الباحث(نص):
    """(AR) التعبيرُ النمطيُّ الذي يقرأُ به FindSad.cmake مخرَجَ الإصدار."""
    مطابقة = re.search(r"MATCHES\s+\"((?:[^\"\\]|\\.)*Sad Compiler[^\"]*)\"", نص)
    if not مطابقة:
        return None
    # (AR) CMake يكتبُ الشرطةَ المائلةَ مضاعفةً داخلَ السلسلة.
    # (EN) CMake doubles the backslash inside the string literal.
    return مطابقة.group(1).replace("\\\\", "\\")


def استخرج_الاسماء_المنشورة(نص):
    """(AR) العمودُ الثالثُ من SAD_TOOL_TABLE. (EN) Third column of SAD_TOOL_TABLE."""
    مطابقة = re.search(r'^SAD_TOOL_TABLE="\n(.*?)^"', نص, re.MULTILINE | re.DOTALL)
    if not مطابقة:
        return set()
    return {س.split(":")[-1] for س in مطابقة.group(1).split() if ":" in س}


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

    # (AR) قناةٌ واحدةٌ: كلُّ خلافٍ زوجٌ (ملفُّه، رسالتُه معَ علاجِها). وكانت
    #      خلافاتُ جدولِ الإصدارِ تُطبَعُ تحتَ ترويسةٍ ثابتةٍ على
    #      `release_tools.sh` وإن كان موضعُها `hub/CMakeLists.txt` — وهو عينُ
    #      العطبِ الذي تُدينُه هذه الرقعةُ في المُحلِّلِ الخامس، تُرِكَ في شقيقِه.
    # (EN) One channel: every offence is a (file, message-with-remedy) pair. The
    #      version-table offences used to print under a fixed header pinning
    #      them all on release_tools.sh even when they lived in the hub's
    #      CMakeLists — the very defect this patch fixes elsewhere.
    مواضع = []
    محرّكات = 0

    for منشور in مطلوبة:
        if منشور not in أعلام:
            مواضع.append(
                ("scripts/ci/release_tools.sh",
                 "لا إملاءَ لأداةٍ موعودة «" + منشور + "»."
                 " العلاج: أضِفْ زوجَها إلى SAD_VERSION_FLAGS"
                 " / no version-flag spelling declared; remedy: release_tools.sh · SAD_VERSION_FLAGS")
            )
            continue

        محرّك = يستشير_الجدول(منشور)
        محرّكات += 1 if محرّك else 0

        # (AR) الطرفُ الثاني: ما يُهجّئُه جدولُ الإصدار.
        # (EN) End two: what the release table spells.
        if محرّك and أعلام[منشور] != قانوني:
            مواضع.append(
                ("scripts/ci/release_tools.sh",
                 منشور + " يستشيرُ الجدولَ المولَّدَ فيلزمُه «" + قانوني
                 + "» ووجدنا «" + أعلام[منشور] + "»."
                 " العلاج: في SAD_VERSION_FLAGS، والاسمُ من cli_flags.yaml · flag.version"
                 " / consults the generated table, must use the canonical name; remedy: release_tools.sh · SAD_VERSION_FLAGS")
            )
        if not محرّك and أعلام[منشور] == قانوني:
            مواضع.append(
                ("scripts/ci/release_tools.sh",
                 منشور + " لا يستشيرُ الجدولَ المولَّدَ فلا يقبلُ «" + قانوني + "»."
                 " العلاج: إمّا إملاءٌ إنجليزيٌّ في SAD_VERSION_FLAGS، وإمّا"
                 " أن تستشيرَ الأداةُ الجدولَ في مصدرِها"
                 " / does not consult the generated table, cannot accept it; remedy: release_tools.sh, or make the tool consult it")
            )

        # (AR) الطرفُ الثالث: ما يُهجّئُه ماكرو الدُّخان. والأداةُ قد لا تُختبَرُ
        #      هناك أصلًا، وهذا مسموح؛ الممنوعُ أن تُختبَرَ بإملاءٍ آخر.
        # (EN) End three: what the smoke macro spells. A tool need not be tested
        #      there at all; what is forbidden is testing it with another spelling.
        if منشور in دخان and دخان[منشور] != أعلام[منشور]:
            مواضع.append(
                ("tests/system/hub/CMakeLists.txt",
                 منشور + " يُختبَرُ بـ«" + دخان[منشور] + "» وجدولُ الإصدارِ يقولُ «"
                 + أعلام[منشور] + "»."
                 " العلاج: في sad_add_tool_smoke_tests، لا في release_tools.sh"
                 " / smoke-tested with a spelling the release table contradicts; remedy: hub/CMakeLists.txt · sad_add_tool_smoke_tests")
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
    # (AR) ⑥ الاسمُ الذي يطبعُه المترجمُ في سطرِ الإصدارِ نسخةٌ من عمودِ
    #      «المنشور» في SAD_TOOL_TABLE، و`FindSad.cmake` نسخةٌ ثالثةٌ منه:
    #      يقرأُ المخرَجَ بتعبيرٍ نمطيٍّ يبدأُ بالاسمِ نفسِه. ولا رابطَ بين
    #      الثلاثة — فمن يبدّلُ السلسلةَ في C++ يُصمِتُ الباحثَ صامتًا:
    #      Sad_VERSION تبقى فارغةً وتنكسرُ find_package(Sad 1.0.0) بلا رسالة.
    # (EN) The name the compiler prints is a copy of the "published" column,
    #      and FindSad.cmake is a third copy: it parses the output with a regex
    #      beginning with that same name. Nothing binds the three, so changing
    #      the C++ literal silently blinds the finder.
    نص_الترويسة = اقرأ(مسار_اسم_المترجم)
    اسم_مطبوع = استخرج_اسم_المترجم_المطبوع(نص_الترويسة)
    إصدار_مطبوع = استخرج_اصدار_المترجم(نص_الترويسة)
    مفصل = استخرج_مفصل_الطباعة(اقرأ(مسار_طباعة_الإصدار))
    نص_الباحث = اقرأ(مسار_باحث_cmake)
    تعبير_باحث = استخرج_تعبير_الباحث(نص_الباحث)
    أعلام_الباحث = استخرج_علم_الباحث(نص_الباحث)
    عائد_الاسم = استخرج_عائد_اسم_المترجم(نص_الترويسة)
    منشورة = استخرج_الاسماء_المنشورة(نص_الجدول)

    # (AR) قياساتٌ من أربعةِ ملفّات: كلٌّ يُعلِّمُ على ملفِّه. ولا يُكتَبُ
    #      عددُها — عددٌ منثورٌ فوقَ جدولٍ يتعفّنُ بأوّلِ صفٍّ يُضاف.
    # (EN) Measurements from four files, each naming its own. No count is
    #      written: a number above a table rots on the first row added.
    for قيمة, ملف, ما in (
        (اسم_مطبوع, "tools/compiler/compiler_driver.h", "اسمَ المترجمِ المطبوع"),
        (عائد_الاسم, "tools/compiler/compiler_driver.h", "عائدَ get_compiler_name"),
        (إصدار_مطبوع, "tools/compiler/compiler_driver.h", "رقمَ الإصدار"),
        (مفصل, "tools/compiler/compiler_driver_cli.cpp", "مفصلَ سطرِ الطباعة"),
        (تعبير_باحث, "tools/build/FindSad.cmake", "تعبيرَ قراءةِ الإصدار"),
        (أعلام_الباحث, "tools/build/FindSad.cmake", "العَلَمَ الذي يُنادى به"),
        (منشورة or None, "scripts/ci/release_tools.sh", "عمودَ الأسماءِ المنشورة"),
    ):
        if قيمة is None:
            print(
                "::error file=" + ملف + "::تعذّرَ قياسُ " + ما
                + " — انتقلَ تعريفٌ أو تغيّرَ شكلُه، فلم يُقَسْ، لا أنّه سليم"
                " / could not measure it here; unmeasured, not green"
            )
            return 1

    # (AR) العقدُ الحقيقيُّ سطرٌ واحدٌ يُركَّبُ من ثلاثةِ مواضع، ويقرؤه
    #      FindSad.cmake بتعبيرٍ نمطيّ. فيُركَّبُ هنا كما يُطبَعُ ثمّ يُعرَضُ على
    #      التعبيرِ نفسِه — لا تُقارَنُ الأسماءُ وحدَها.
    # (EN) The real contract is one composed line, read by a regex. Compose it
    #      exactly as printed and run that very regex over it.
    # (AR) 🔑 ويُؤخَذُ العائدُ **كاملًا**: كان يُقَصُّ عندَ « - Sad Compiler»
    #      ثمّ تُعادُ اللاحقةُ لصقًا هنا — نسخةٌ رابعةٌ من السلسلةِ نفسِها داخلَ
    #      أداةٍ كُتِبَت لإدانةِ النُّسَخ. فإن غُيِّرَتِ اللاحقةُ في الترويسةِ
    #      تحرّكَ السطرُ المتوقَّعُ معها بلا تعديلٍ هنا.
    # (EN) The full return value is captured: it used to be cut at
    #      " - Sad Compiler" and re-pasted here — a fourth copy of the same
    #      string inside a tool written to condemn copies.
    سطر_متوقع = عائد_الاسم + مفصل + إصدار_مطبوع
    if not re.search(تعبير_باحث, سطر_متوقع):
        مواضع.append(
            ("tools/build/FindSad.cmake",
             "تعبيرُه لا يطابقُ السطرَ الذي يطبعُه الثنائيّ «" + سطر_متوقع
             + "» ⇒ Sad_VERSION تبقى فارغةً وتنكسرُ find_package(Sad) صامتةً."
             " العلاج: في FindSad.cmake · MATCHES، أو في get_compiler_name"
             " / its regex does not match the line the binary prints; remedy: FindSad.cmake · MATCHES, or get_compiler_name")
        )
    # (AR) وكلُّ عَلَمٍ في **كودِ** الباحثِ يجبُ أن يكونَ قانونيًّا، لا الذي
    #      يلي `COMMAND` وحدَه: `SHARED_FLAG` يُضَمُّ إلى `COMPILE_COMMAND`،
    #      فكان يفلتُ من الموضِّعَين معًا — وهو العَلَمُ الوحيدُ في هذه الرقعةِ
    #      الذي **يُنفَّذ**. وتُطرَحُ التعليقاتُ أوّلًا لئلّا يُدانَ شرحُ العطب.
    # (EN) Every flag in the finder's CODE must be canonical, not only the one
    #      following COMMAND: SHARED_FLAG is appended to COMPILE_COMMAND and so
    #      escaped both locators — the one flag here that actually runs.
    for عَلَم in أعلام_الباحث_المنفَّذة(نص_الباحث, قانونية):
        مواضع.append(
            ("tools/build/FindSad.cmake",
             "يبني أمرًا بعَلَمٍ لا وجودَ له «" + عَلَم + "» ⇒ المترجمُ يردُّه"
             " «خيارًا غيرَ معروف»، فينكسرُ كلُّ sad_add_library بهذا الوضع."
             " العلاج: خُذِ الاسمَ من language-truth/cli_flags.yaml · canonical"
             " / builds a command with a flag the compiler rejects; remedy:"
             " language-truth/cli_flags.yaml · field `canonical`")
        )
    for عَلَم in أعلام_الباحث:
        if عَلَم != أعلام.get("sadc"):
            مواضع.append(
                ("tools/build/FindSad.cmake",
                 "ينادي المترجمَ بـ«" + عَلَم + "» وإملاؤه المُعلَنُ «"
                 + أعلام.get("sadc", "—") + "» — ERROR_QUIET يبتلعُ الخطأَ فتبقى "
                 "Sad_VERSION فارغةً، ولا يُطبَعُ سطرٌ ليطابقَه التعبيرُ أصلًا."
                 " العلاج: في FindSad.cmake · execute_process"
                 " / invokes the compiler with a flag it does not accept; remedy: FindSad.cmake · execute_process")
            )
    if اسم_مطبوع not in منشورة:
        مواضع.append(
            ("tools/compiler/compiler_driver.h",
             "يطبعُ «" + اسم_مطبوع + "» — وليس اسمًا مُسجَّلًا في SAD_TOOL_TABLE."
             " العلاج: في release_tools.sh · SAD_TOOL_TABLE، لا في cli_flags.yaml"
             " / prints a name that is not published; remedy: release_tools.sh · SAD_TOOL_TABLE")
        )

    # (AR) ⑦ نثرُ المساعدةِ داخلَ الثنائيّ — يُقرَأُ ثمّ يُنفَّذ.
    # (EN) The binary's own help prose is read and then executed.
    نص_المساعدة = اقرأ(مسار_طباعة_الإصدار)
    مدى_المساعدة = استخرج_مدى_المساعدة(نص_المساعدة)
    if مدى_المساعدة is None:
        print(
            "::error file=tools/compiler/compiler_driver_cli.cpp::تعذّرَ بلوغُ نثرِ المساعدة — انتقلَ `print_help` أو تغيّرَ شكلُه، "
            "فلم يُقَسْ / could not locate the help prose; unmeasured, not green"
        )
        return 1
    خلاف_المساعدة, أسطر_مقيسة, أسطر_الجسم = امسح_المساعدة(
        نص_المساعدة, مدى_المساعدة, قانونية, منشورة
    )
    if أسطر_الجسم == 0:
        print(
            "::error file=tools/compiler/compiler_driver_cli.cpp::لا سطرَ استعمالٍ "
            "واحدًا في `print_help` — تغيّرَ شكلُ الأمثلةِ فلم يعُدْ يُطابِقُها "
            "شيء، فلم يُقَسْ، لا أنّه سليم. العلاج: في print_help · أسطرُ الأمثلة"
            " / no usage line at all; unmeasured, not green; remedy:"
            " print_help, the example lines"
        )
        return 1
    if أسطر_مقيسة != أسطر_الجسم:
        print(
            "::error file=tools/compiler/compiler_driver_cli.cpp::مدى نثرِ المساعدةِ "
            "قاسَ " + str(أسطر_مقيسة) + " من " + str(أسطر_الجسم)
            + " من أسطرِ الاستعمالِ في `print_help` — انتقلَ قسمُ «الخيارات» أو "
            "تغيّرَ شكلُ الأمثلة، فالفرقُ لم يُقَسْ، لا أنّه سليم"
            " / the range measured a different number of usage lines than the function body holds;"
            " the remainder is unmeasured, not green"
        )
        return 1
    for رقم, رسالة in خلاف_المساعدة:
        مواضع.append(("tools/compiler/compiler_driver_cli.cpp", "السطر " + str(رقم) + " · " + رسالة))

    # (AR) التعليلُ يتبعُ العَلَمَ لا يفترضُه: الإنجليزيُّ مرادفٌ ملغًى،
    #      والعربيُّ خطأٌ إملائيّ — والمقارنةُ بايتيّةٌ في الحالَين.
    # (EN) The reason follows the flag: an English one is a retired alias, an
    #      Arabic one is a misspelling. Either way the CLI compares bytes.
    try:
        خلاف_النثر, ملفات_ممسوحة = امسح_النثر(قانونية)
    except FileNotFoundError as غياب:
        print(
            "::error file=scripts/ci/check_version_flags.py::مُوَضِّعُ نثرٍ لا وجودَ "
            "له «" + str(غياب) + "» — المُحلِّلُ الخامسُ لم يمسحْ شيئًا، فلم يُقَسْ، "
            "لا أنّه سليم. العلاج: أصلحْ `شجر_النثر` أو `لواحق_النثر`"
            " / a prose locator resolved to nothing; the fifth analyzer scanned"
            " no file — unmeasured, not green; remedy: fix شجر_النثر,"
            " لواحق_النثر or أسماء_النثر in this file"
        )
        return 1
    for ملف, رقم, عَلَم, جانٍ in خلاف_النثر:
        علة = ("محرفٌ لاصقٌ يبترُه فلا يصلُ إلى `argv` كما يُقرَأ"
               " / a combining character truncates it before argv"
               if جانٍ
               else "لا مرادفَ إنجليزيًّا في اللغة"
                    " / the language has no English aliases"
               if re.fullmatch(r"--[\x21-\x7e]+", عَلَم)
               else "خطأٌ إملائيّ — المترجمُ يقارنُ الاسمَ بايتًا ببايت"
                    " / a misspelling; the CLI compares the name byte for byte")
        مواضع.append(
            (ملف, "السطر " + str(رقم) + " · " + وصف_العلم(عَلَم, جانٍ)
             + " — " + علة
             + ". العلاج: خُذِ الاسمَ من language-truth/cli_flags.yaml · canonical"
             " / remedy: take the name from language-truth/cli_flags.yaml,"
             " field `canonical`")
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

    # (AR) 🔑 كلُّ خلافٍ يُنسَبُ إلى ملفِّه ويحملُ علاجَه. وكانت ترويسةٌ واحدةٌ
    #      ثابتةٌ تنسبُ كلَّ خلافٍ إلى `release_tools.sh` ولو كان موضعُه
    #      `.ld` أو `hub/CMakeLists.txt`، فتعليقُ GitHub يقعُ على ملفٍّ بريء.
    # (EN) Each offender is annotated on its own file and carries its remedy;
    #      one fixed header used to pin every offence on release_tools.sh.
    if مواضع:
        print(
            "::error::خلافٌ عن مصدرِ الحقيقة — كلُّ موضعٍ مُعلَّمٌ على ملفِّه، ومعه علاجُه."
            " مصدرُ الحقيقة: language-truth/cli_flags.yaml · flag.version · canonical = "
            + قانوني
            + " / each offender is annotated on its own file, with its remedy"
        )
        for ملف, رسالة in مواضع:
            print("::error file=" + ملف + "::" + رسالة)
        return 1

    print(
        "✅ سطرُ الإصدارِ المطبوع: «" + سطر_متوقع
        + "» — اسمُه منشورٌ ويقرؤه FindSad.cmake"
        " / the printed version line is published and parsed by the finder"
    )
    print(
        "✅ نثرُ المساعدةِ في الثنائيّ: أسطرُ الاستعمالِ والأمثلةِ في "
        "compiler_driver_cli.cpp ("
        + str(مدى_المساعدة[0]) + "–" + str(مدى_المساعدة[1]) + "، "
        + str(أسطر_مقيسة) + " من أسطرِ الاستعمالِ قِيسَتْ كلُّها)"
        " تُوصي بأمرٍ منشورٍ وبأعلامٍ قانونيّة"
        " / the binary's usage prose names a published command and canonical flags"
    )
    print(
        "✅ حارسُ الأعلام: " + str(len(قانونية))
        + " اسمًا قانونيًّا، وكلُّ عَلَمٍ منثورٍ في " + str(ملفات_ممسوحة)
        + " ملفًّا ممسوحًا يطابقُه"
        " / canonical names; every prose flag in the scanned files matches"
    )
    print(
        "✅ حارسُ إملاءِ عَلَمِ الإصدار: "
        + str(len(مطلوبة))
        + " أدواتٍ موعودةً، منها "
        + str(محرّكات)
        + " محرّكاتٍ ملزَمةً بـ«"
        + قانوني
        + "» / promised tools, of which engines are bound to the canonical name"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
