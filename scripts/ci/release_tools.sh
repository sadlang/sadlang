#!/usr/bin/env bash
# ============================================================================
# (AR) **مصدرُ حقيقةٍ واحدٌ لأدواتِ الإصدار: الهدفُ ↔ اسمُ المُخرَجِ ↔ الاسمُ
#      المنشور.** والعطبُ الذي ولّد هذا الملفَّ أنّ الثلاثةَ كانت مكتوبةً
#      باليدِ في release.yml في سبعةِ مواضعَ منفصلة، فانجرفت:
#
#      - cmake --build --target sad-lsp — و«sad-lsp» **اسمُ مُخرَجٍ لا اسمُ
#        هدف**، والهدفُ «sad-lsp-server». فالبناءُ يُخفِقُ ويبتلعُه «|| true»،
#        و«sad-lsp.exe» يغيبُ عن كلِّ حزمة. وهذا وحدَه يكسرُ محرابًا:
#        الإكمالُ والتحويمُ والتشخيصُ ثلاثتُها تسقطُ عند المستخدمِ النهائيّ.
#      - cp build/bin/sadc — ولا هدفَ اسمُه «sadc» في الشجرةِ منذ إعادةِ
#        التسمية؛ الهدفُ «sad-build» والاسمُ المنشورُ «sadc».
#      - «sad_check» هدفًا و«sad-check» مُخرَجًا — ولا يَرِدُ ذكرُه في مجرى
#        الإصدارِ ولا مرّةً واحدة.
#      - خطوةُ حزمةِ المفسّرِ تبني «sad-run» ثمّ تنسخُ «sad» — هدفانِ
#        مختلفانِ من ملفَّي CMakeLists مختلفَين.
#
#      وكلُّ نسخةٍ كانت مذيّلةً بـ«|| true» أو «-ErrorAction SilentlyContinue»،
#      وخطوةُ «التحقّقِ من اكتمالِ الحزم» تعدُّ الأرشيفاتِ ولا تفتحُ واحدًا —
#      فأرشيفٌ فارغٌ تمامًا يجتازُها. ⇒ مجرًى أخضرُ وحزمةٌ ناقصةٌ بلا إشارة.
#
#      🔑 والعلاجُ ليس تصحيحَ الأسماءِ السبعةِ بل **إلغاءَ النسخِ السبع**:
#      الجدولُ هنا وحدَه، والمجرى يسألُه. وأيُّ نقصٍ يُسقِطُ الشوطَ بصوتٍ عالٍ.
#
# (EN) Single source of truth for release tools: target ↔ output name ↔
#      published name. These three were hand-written in seven separate places
#      in release.yml and drifted: `--target sad-lsp` (an OUTPUT name, not a
#      target — the target is sad-lsp-server) failed and was swallowed by
#      `|| true`, so sad-lsp.exe was absent from every package, which is
#      exactly what breaks Mihrab; `sadc` has not been a target since the
#      rename to sad-build; sad_check/sad-check is never mentioned at all; the
#      interpreter job builds sad-run and copies sad. Every copy was suffixed
#      with `|| true`, and the "verify artifacts" step counts archives without
#      opening one — a completely empty archive passes it. The remedy is not to
#      fix seven names but to delete seven copies: this table is asked instead.
# ============================================================================
set -euo pipefail

# (AR) هدف:مُخرَج:منشور — الثلاثةُ صريحةٌ لأنّ اختلافَها هو عينُ المصيدة.
#      و«المنشور» هو ما يجدُه المستخدمُ في bin/، وهو ما تبحثُ عنه
#      distribution/install.sh و install.ps1. ولذلك «sad-build» يُنشَرُ
#      «sadc»: المثبِّتانِ يبحثانِ عن «sadc» ولا يعرفانِ اسمَ الهدف.
# (EN) target:output:published. Published is what the user finds in bin/ and
#      what the installers look for — hence sad-build ships as sadc.
#
# (AR) ⚠️ **الأمرُ المنشورُ «sad» ليس المفسّر.** الشجرةُ تقولُها بنفسِها في
#      tools/hub/src/main.cpp: «sad-hub.exe، يُنشَر أيضاً كـ sad.exe» — فالأمرُ
#      العامُ مركزُ أدواتٍ يُرسِلُ إلى إخوتِه (sad run · sad build …)، ويجدُهم
#      بمسحِ مجلّدِه نفسِه. ⇒ وجودُ «sad» وحدَه لا يكفي: غيابُ أخٍ
#      يجعلُ الأمرَ الموعودَ به غيرَ موجودٍ عندَ المستخدِم، وهذا عينُ ما
#      وقعَ في v1.0.0. ولذلكَ قائمةُ الإلزامِ تحملُ المركزَ وإخوتَه معًا.
#      ⚠️ والهدفُ المسمّى «sad» في tools/build/CMakeLists.txt مشروعٌ قائمٌ
#      بذاتِه (project(sad_build)) **لا يُضيفُه أيُّ add_subdirectory في المستودعِ
#      كلِّه** — فهو غيرُ مبنيٍّ منذُ زمنٍ، وليس هو «sad» المنشور.
# (EN) The published command `sad` is the tool HUB, not the interpreter: the
#      tree says so itself in tools/hub/src/main.cpp ("sad-hub.exe, also
#      deployed as sad.exe"). It dispatches to its siblings (sad run, sad
#      build ...) by scanning its own directory, so shipping `sad` alone is
#      not enough — a missing sibling makes a promised command absent, which
#      is exactly what v1.0.0 shipped. The target that used to be literally
#      named `sad` lives in tools/build/, a self-contained project() that no
#      add_subdirectory ever adds; it was renamed so the hub could take the
#      name it actually publishes.
# (AR) 🔑 الجدولُ الآنَ **مُطابَقةٌ**: الهدفُ = المُخرَجُ = المنشورُ في كلِّ صفٍّ
#      إلّا صفَّ لقبٍ واحدًا مُعلَنًا. وثلاثةُ تباعُداتٍ أُزيلت من أصلِها لا
#      رُقِّعت في الجدول: «sad_hub→sad-hub» و«sad_check→sad-check» و
#      «sad-lsp-server→sad-lsp» صارت أهدافُها تحملُ أسماءَ مُخرَجاتِها، فسقطَت
#      `OUTPUT_NAME` من ترويسةِ كلِّ واحدٍ منها، وسقطت معها نسخةُ اللقبِ
#      `sad-hub → sad` في POST_BUILD. فمن أرادَ اسمًا فليُسمِّ الهدفَ به.
#      والباقي **لقبٌ واحدٌ لا يُوحَّد**: `sad-build` يُنشَرُ أيضًا باسمِ `sadc`
#      — ثنائيٌّ واحدٌ باسمَينِ عمدًا، وتعليلُه أسفلَه.
# (EN) The table is now an IDENTITY — target = output = published on every row
#      but one declared alias. Three divergences were removed at their source,
#      not patched here: the three targets now carry their output names, so
#      OUTPUT_NAME is gone from each, and so is the POST_BUILD `sad-hub → sad`
#      copy. One alias remains and cannot be unified: sad-build also ships as
#      sadc — one binary, two published names, rationale below.
SAD_TOOL_TABLE="
sad:sad:sad
sad-lsp:sad-lsp:sad-lsp
sad-check:sad-check:sad-check
sad-pkg:sad-pkg:sad-pkg
sad-repl:sad-repl:sad-repl
sad-fmt:sad-fmt:sad-fmt
sad-build:sad-build:sad-build
sad-build:sad-build:sadc
"

# (AR) قوائمُ الإلزام: ما **يجبُ** أن يوجدَ في كلِّ حزمةٍ وإلّا سقطَ الشوط.
#      وما ليس هنا يُنسَخُ إن وُجِد ولا يُلزِم.
#      ⚠️ والمترجمُ يُنشَرُ **باسمَين**: «sadc» لأنّ المثبِّتَينِ والوثائقَ تَعِدُ
#      به، و«sad-build» لأنّ مركزَ الأدواتِ يجدُ إخوتَه بمسحِ الأسماءِ ذاتِ
#      البادئةِ «sad-» — فاسمٌ بلا شرطةٍ لا يُسجَّلُ عنده، و«sad build» يصيرُ
#      أمرًا موعودًا لا وجودَ له.
# (EN) Hard-required members per package; anything else is best-effort.
#      The compiler ships under BOTH names: `sadc` because the installers and
#      the docs promise it, and `sad-build` because the hub finds its siblings
#      by scanning for the `sad-` prefix — a name without the dash is never
#      registered, and `sad build` becomes a promised command that is absent.
# (AR) 🔑 **صنفان لا ثلاثة.** كانت القسمةُ «مفسّرٌ ↔ مترجِمٌ ↔ كلاهما»،
#      فلمّا حُذِفَ المفسّرُ زالَ أحدُ طرفَيها: لم يبقَ لصنفِ «المترجمِ وحدَه» ما
#      يميّزُه عن القياسيّة، وصارَ اسمُ «المفسّر» يَعِدُ بما لا يوجد.
#      وقد أُضيفَ `sad-build` إلى القائمةِ الأولى قبلَ هذا لأنّ المكوّنَ كان لا
#      يشحنُ ما يُشغّلُ برنامجَ ص — والنيّةُ سليمةٌ والتنفيذُ كان ناقصًا: أُضيفَ
#      الهدفُ ولم تُبدَّلْ تهيئتُه من `ENABLE_LLVM_BACKEND=OFF`، وهو لا يوجدُ
#      إلّا بـ`ON`. فبُدِّلت التهيئةُ في `release.yml` وتقاعدَ الصنفُ الثالث.
# (EN) 🔑 Two classes, not three. The split was interpreter ↔ compiler ↔
#      both; deleting the interpreter removed one side, so a compiler-only class
#      has nothing left to distinguish it and «interpreter» promised what is
#      absent. sad-build had already been added to the first list because the
#      component shipped nothing that runs a ص program — right intent, incomplete
#      execution: the target was added but its configure stayed
#      ENABLE_LLVM_BACKEND=OFF, where that target does not exist. The configure
#      is fixed in release.yml and the third class is retired.
SAD_REQUIRED_STANDARD="sad sad-build sad-lsp sad-check"
SAD_REQUIRED_FULL="sad sad-lsp sad-check sadc sad-build"

# (AR) 🔑 الألقابُ المُعلَنةُ — الصفوفُ الوحيدةُ التي يجوزُ فيها أن يخالفَ
#      الاسمُ المنشورُ هدفَه. وكلُّ صفٍّ خارجَها **يجبُ** أن يكونَ مُطابَقةً.
#      وبغيرِ هذا الحارسِ يعودُ الانجرافُ بأوّلِ `OUTPUT_NAME` يكتبُها أحدٌ:
#      اسمٌ ثانٍ يُولَدُ صامتًا، ثمّ يُهجَّأُ في سبعةِ مواضعَ فتتباعد.
# (EN) Declared aliases — the only rows where the published name may differ
#      from its target. Every other row MUST be an identity. Without this
#      guard the drift returns with the first OUTPUT_NAME anyone writes.
SAD_DECLARED_ALIASES="sad-build:sadc"

# (AR) يُحاكِمُ الجدولَ نفسَه: هدف = مُخرَج = منشور، إلّا لقبًا مُعلَنًا.
# (EN) Judges the table itself: target = output = published, except a declared alias.
sad_require_identity_table() {
    local line target output published bad=""
    while IFS= read -r line; do
        [ -n "$line" ] || continue
        IFS=: read -r target output published <<< "$line"
        [ "$target" = "$output" ] || bad="$bad
  هدفٌ يخالفُ مُخرَجَه / target != output: $target != $output"
        if [ "$output" != "$published" ]; then
            case " $SAD_DECLARED_ALIASES " in
                *" $target:$published "*) ;;
                *) bad="$bad
  لقبٌ غيرُ مُعلَن / undeclared alias: $target → $published" ;;
            esac
        fi
    done <<< "$SAD_TOOL_TABLE"
    if [ -n "$bad" ]; then
        echo "::error::جدولُ الأدواتِ انجرف / tool table drifted:$bad"
        return 1
    fi
    echo "✅ الجدولُ مُطابَقةٌ / table is an identity (aliases: $SAD_DECLARED_ALIASES)"
}

# (AR) الأهدافُ خلفَ قائمةِ أسماءٍ منشورة — تُشتقُّ من الجدولِ لا تُكتَبُ ثانيةً.
# (EN) Targets behind a published-name list; derived, never re-typed.
_sad_targets_for() {
    local published_list="$1" line target output published
    while IFS= read -r line; do
        [ -n "$line" ] || continue
        IFS=: read -r target output published <<< "$line"
        case " $published_list " in
            *" $published "*) printf '%s\n' "$target" ;;
        esac
    done <<< "$SAD_TOOL_TABLE"
}

# (AR) يبني الأهدافَ اللازمةَ لقائمةِ أسماءٍ منشورة.
# (EN) Build the targets behind a published-name list.
sad_build_tools() {
    local build_dir="$1" config="$2" published_list="$3" target
    for target in $(_sad_targets_for "$published_list" | sort -u); do
        echo "🔨 $target"
        cmake --build "$build_dir" --config "$config" --target "$target"
    done
}

# (AR) يعثرُ على المُخرَجِ في تخطيطِ يونكس (bin/) أو ويندوز (bin/<config>/).
# (EN) Locate the output under the Unix or the multi-config layout.
_sad_find_output() {
    local build_dir="$1" config="$2" output="$3" candidate
    for candidate in \
        "$build_dir/bin/$config/$output.exe" \
        "$build_dir/bin/$config/$output" \
        "$build_dir/bin/$output.exe" \
        "$build_dir/bin/$output"; do
        if [ -f "$candidate" ]; then printf '%s\n' "$candidate"; return 0; fi
    done
    return 1
}

# (AR) ينسخُ كلَّ أداةٍ في القائمةِ إلى dest/ باسمِها **المنشور**.
#      ولا «|| true» هنا: النقصُ يُبلَّغُ ويُحاكَمُ في sad_require_tools.
# (EN) Copy each tool to dest/ under its PUBLISHED name. No `|| true`.
sad_copy_tools() {
    local build_dir="$1" config="$2" dest="$3" published_list="$4"
    local line target output published src ext
    mkdir -p "$dest"
    while IFS= read -r line; do
        [ -n "$line" ] || continue
        IFS=: read -r target output published <<< "$line"
        case " $published_list " in *" $published "*) ;; *) continue ;; esac
        if src="$(_sad_find_output "$build_dir" "$config" "$output")"; then
            ext=""
            case "$src" in *.exe) ext=".exe" ;; esac
            cp "$src" "$dest/$published$ext"
            echo "📦 $published$ext ← $src"
        else
            echo "⚠️ غير موجود / not found: $output (هدف $target)"
        fi
    done <<< "$SAD_TOOL_TABLE"
}

# (AR) الحكم: كلُّ اسمٍ منشورٍ في القائمةِ موجودٌ في dest/ وغيرُ فارغ.
#      ويُعدَّدُ **كلُّ** الناقصِ قبلَ السقوطِ — تقريرٌ واحدٌ خيرٌ من سبعِ دورات.
# (EN) Judgement: every published name present and non-empty. All missing
#      names are listed before failing — one report beats seven rounds.
sad_require_tools() {
    local dest="$1" published_list="$2" published found missing=""
    for published in $published_list; do
        found=""
        if [ -s "$dest/$published" ]; then found="$dest/$published"; fi
        if [ -s "$dest/$published.exe" ]; then found="$dest/$published.exe"; fi
        if [ -z "$found" ]; then missing="$missing $published"; fi
    done
    if [ -n "$missing" ]; then
        echo "::error::الحزمةُ ناقصة — أدواتٌ موعودةٌ غائبةٌ عن $dest:$missing"
        echo "::error::Package incomplete — promised tools missing from $dest:$missing"
        echo "── الموجودُ فعلًا / actually present ──"
        ls -l "$dest" || true
        return 1
    fi
    echo "✅ كلُّ ما وُعد به موجود / all promised tools present: $published_list"
}

# (AR) يفتحُ الأرشيفَ فعلًا ويطلبُ الأعضاء. الفحصُ القديمُ كان يعدُّ الأرشيفاتِ
#      في المجلّد، فأرشيفٌ فارغٌ يجتازُه — وهو ما تشهدُ به حزمةُ v1.0.0.
# (EN) Actually open the archive and require members. The old check counted
#      archives in a directory, so an empty archive passed — as v1.0.0 shows.
# (AR) فهرسةُ الأرشيفِ في موضعٍ واحد — كانت داخلَ دالّةٍ واحدةٍ فقط، وأيُّ
#      حُكمٍ ثانٍ كان سيَنسخُها. نسختانِ من قراءةِ أرشيفٍ تتباعدان.
# (EN) One place that lists an archive. It used to live inside a single
#      function, so any second judgement would have copied it.
_sad_archive_listing() {
    local archive="$1"
    if [ ! -f "$archive" ]; then
        echo "::error::أرشيفٌ مفقود / missing archive: $archive" >&2
        return 1
    fi
    case "$archive" in
        *.zip)    unzip -Z1 "$archive" ;;
        *.tar.gz) tar -tzf "$archive" ;;
        *)
            echo "::error::صيغةُ أرشيفٍ غيرُ معروفة / unknown archive format: $archive" >&2
            return 1
            ;;
    esac
}

# (AR) 🔑 **الحكمُ كان يقفُ عندَ `bin/`.** فحزمةٌ فيها التنفيذيّاتُ السّتُّ
#      كلُّها ومجلّدُ `stdlib` **فارغٌ** تجتازُ خضراء — والمفسّرُ فيها لا
#      يستوردُ سطرًا واحدًا. ونسخُ المكتبةِ القياسيّةِ في مجرى الإصدارِ
#      يجري بـ`|| true` على يونكس وبـ`-ErrorAction SilentlyContinue` على
#      ويندوز، أي بلا حُكمٍ من أصلِه. هذا نظيرُ عطبِ v1.0.0 بعينِه، في
#      طبقةٍ أخرى.
#      والعدُّ لا الاسمُ: أسماءُ وحداتِ المكتبةِ عربيّةٌ، وترميزُ ما يُخرِجُه
#      `unzip -Z1` لأرشيفِ ويندوزَ ليس مضمونًا — فمِرساةُ الاسمِ تحمرُّ كذبًا.
# (EN) The judgement stopped at bin/. A package with all six executables and
#      an EMPTY stdlib passed green, and its interpreter cannot import a
#      single line. The stdlib copy runs under `|| true` / SilentlyContinue,
#      i.e. unjudged. Count, do not name-match: stdlib module names are
#      Arabic and the encoding of `unzip -Z1` output is not guaranteed, so a
#      name anchor would red falsely.
sad_require_archive_dir() {
    local archive="$1" dir="$2" minimum="$3" listing found
    listing="$(_sad_archive_listing "$archive")" || return 1
    found="$(printf '%s
' "$listing" | grep -cE "(^|/)${dir}/.+" || true)"
    if [ "$found" -lt "$minimum" ]; then
        echo "::error::$archive — «${dir}» فيه $found عضوًا والمطلوبُ $minimum على الأقلّ / has $found members, needs at least $minimum"
        printf '%s
' "$listing" | head -40
        return 1
    fi
    echo "✅ $archive — «${dir}» فيه $found عضوًا / $found members"
}

sad_require_archive() {
    local archive="$1" published_list="$2" listing published missing=""
    listing="$(_sad_archive_listing "$archive")" || return 1
    for published in $published_list; do
        if ! printf '%s\n' "$listing" | grep -qE "(^|/)bin/${published}(\.exe)?$"; then
            missing="$missing $published"
        fi
    done
    if [ -n "$missing" ]; then
        echo "::error::$archive ينقصُه / is missing:$missing"
        printf '%s\n' "$listing" | head -60
        return 1
    fi
    echo "✅ $archive يحوي كلَّ ما وُعد / contains every promised tool"
}

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **عَلَمُ الإصدارِ إملاءٌ مُعلَنٌ لا افتراض.**
#      `language-truth/cli_flags.yaml` يجعلُ `--إصدار` الاسمَ الطويلَ الوحيدَ
#      للمحرّكَين (`engines: [compiler, interpreter]`) وقد أُلغيت ثمانيةَ عشرَ
#      مرادفًا إنجليزيًّا عمدًا؛ أمّا الهُبُّ والأدواتُ المساعدةُ فخارجَ الجدولِ
#      وتبقى إنجليزيّة. وماكرو الدُّخانِ كان يكتبُ `--version` حرفيًّا لكلِّ
#      أداة، فكان يقيسُ **إملاءَه هو** لا عقدَ الأداة — ستُّ خاناتٍ حمراءُ في
#      الطلب #438 بعلّةٍ واحدة. فجدولٌ إذًا، لا افتراض.
# (EN) The version spelling is a DECLARED datum, not an assumption.
#      cli_flags.yaml makes --إصدار the only long name for the two engines
#      (18 English synonyms deliberately abolished); the hub and the auxiliary
#      tools sit outside that table and keep English. A smoke macro that wrote
#      --version literally was measuring its own spelling, not the tool's
#      contract — six red cells in PR #438 from one cause.
# ══════════════════════════════════════════════════════════════════════════
SAD_VERSION_FLAGS="sad:--version sadc:--إصدار sad-build:--إصدار sad-check:--version sad-lsp:--version"

sad_version_flag() {
    local name="$1" pair
    for pair in $SAD_VERSION_FLAGS; do
        case "$pair" in
            "$name:"*) printf '%s' "${pair#*:}"; return 0 ;;
        esac
    done
    echo "::error::لا عَلَمَ إصدارٍ مُعلَنٌ لـ / no declared version flag for: $name" >&2
    return 1
}

# (AR) شرطٌ مسبقٌ لا حارسُ انجراف: يتأكّدُ أنّ كلَّ أداةٍ موعودةٍ لها إملاءٌ،
#      فلا تسقطُ واحدةٌ من الفحصِ صامتةً ويبقى الشوطُ أخضرَ وهو لم يقسْها.
#      ⚠️ وطرفاه من هذا الملفِّ نفسِه، فلا يرى انجرافًا عن مصدرِ الحقيقة —
#      ذاكَ عملُ `scripts/ci/check_version_flags.py`، وهو يقرأُ أربعةَ ملفّاتٍ
#      بأربعةِ مُحلِّلاتٍ ويشتقُّ قاعدتَه من المصادرِ لا من قائمةٍ مكتوبة.
# (EN) A precondition, not a drift guard: it checks every promised tool has a
#      spelling, so none drops out of the smoke test silently. Both its ends
#      come from THIS file, so it cannot see drift from the source of truth —
#      that is check_version_flags.py's job, which reads four files with four
#      parsers and derives its rule from the sources rather than a written list.
sad_require_version_flags() {
    local published missing=""
    for published in $SAD_REQUIRED_FULL; do
        sad_version_flag "$published" >/dev/null 2>&1 || missing="$missing $published"
    done
    if [ -n "$missing" ]; then
        echo "::error::أدواتٌ موعودةٌ بلا إملاءِ إصدارٍ مُعلَن / promised tools with no declared version flag:$missing"
        return 1
    fi
    echo "✅ كلُّ أداةٍ موعودةٍ لها إملاءٌ مُعلَن / every promised tool has a declared spelling"
}

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **الفرقُ بين «الأرشيفُ يحويه» و«الثنائيُّ يعمل».**
#      `sad_require_archive` يفتحُ الأرشيفَ ويطلبُ أعضاءَه بالاسم — وهذا يمنعُ
#      حزمةً فارغةً، ولا يمنعُ ثنائيًّا لا يبدأُ العملَ أصلًا: مكتبةٌ مشتركةٌ
#      مفقودةٌ، أو رمزٌ غيرُ محلول، أو glibc أحدثَ من أرضيّةِ الهدف. ولم
#      يُشغَّلْ من الحزمةِ المنشورةِ ثنائيٌّ واحدٌ قطُّ قبلَ هذه الدالّة.
#      و**الصمتُ التامُّ مع خروجٍ صفريٍّ أحمرُ هنا**: أداةٌ تُنهي بلا كلمةٍ
#      لم تُقَسْ، والأخضرُ عندها يعني «لم يُقَسْ» لا «سليم». ودقّةُ العبارةِ
#      مقصودة: الالتقاطُ يجمعُ `stderr` إلى `stdout`، فالذراعُ لا تنطلقُ
#      لأداةٍ تكتبُ في `stderr` وحدَه — تنطلقُ للصامتِ في القناتَينِ معًا.
#      فلا تُحسَبْ هذه الذراعُ تغطيةً: العملُ الحقيقيُّ على رمزِ الخروج.
# (EN) The gap between "the archive contains it" and "the binary runs".
#      sad_require_archive opens the archive and requires members by name,
#      which stops an empty package but not a binary that cannot start: a
#      missing shared library, an unresolved symbol, a glibc newer than the
#      target floor. Until this function, not one binary from a published
#      package had ever been executed.
#      Empty stdout with exit 0 is RED here: a tool that ends without a word
#      was not measured, and green would mean "unmeasured", not "healthy".
# ══════════════════════════════════════════════════════════════════════════
sad_smoke_package() {
    local bindir="$1" published_list="$2"
    local published flag bin out rc snippet bad=""
    for published in $published_list; do
        bin="$bindir/$published"
        [ -f "$bin" ] || bin="$bindir/$published.exe"
        if [ ! -f "$bin" ]; then
            bad="$bad
  غيرُ موجودٍ في bin/ / absent from bin/: $published"
            continue
        fi
        flag="$(sad_version_flag "$published")" || return 1
        # (AR) 🔑 الالتقاطُ داخلَ `if` لا في إسنادٍ عارٍ: خطواتُ ﭼِتهَب تعملُ
        #      بـ`bash -eo pipefail`، وإسنادٌ من أمرٍ ساقطٍ يقتلُ الخطوةَ **قبلَ**
        #      أن يُطبَعَ التشخيص — فيصيرُ العطبُ إخفاقًا بلا كلمةٍ تُسمّيه.
        #      وشرطُ `if` مستثنًى من `-e` بحكمِ الصَّدَفةِ نفسِها.
        # (EN) Capture inside an `if`, not a bare assignment: GitHub steps run
        #      under bash -eo pipefail, where assigning from a failing command
        #      kills the step BEFORE the diagnostic prints — turning the defect
        #      into a wordless failure. An `if` condition is exempt from -e.
        if out="$("$bin" "$flag" 2>&1)"; then rc=0; else rc=$?; fi
        if [ "$rc" -ne 0 ]; then
            # (AR) 🔑 لا `| head` هنا: تحتَ `-eo pipefail` يُغلِقُ `head` القناةَ
            #      بعدَ ثلاثةِ أسطرٍ فيسقطُ المُنتِجُ بـSIGPIPE (141)، فترجعُ
            #      الاستبدالةُ 141 ويرجعُ **الإسنادُ** 141، فتموتُ الخطوةُ قبلَ
            #      سطرِ `::error::` أسفلَه — أي العطبُ الذي يشرحُه التعليقُ فوقَه
            #      يعودُ من بابِ التشخيصِ نفسِه. والقصُّ يقعُ في الصَّدَفةِ بلا قناة.
            # (EN) No `| head` here: under -eo pipefail, head closes the pipe
            #      after three lines, the producer dies with SIGPIPE (141), the
            #      substitution returns 141 and so does the ASSIGNMENT — killing
            #      the step before the ::error:: below. The very defect the note
            #      above describes, re-entering through the diagnostic itself.
            #      Truncate in the shell, with no pipe involved.
            snippet="${out%%$'\n'*}"
            bad="$bad
  $published $flag ⇒ رمزُ خروجٍ / exit $rc :: ${snippet:0:200}"
        elif [ -z "$out" ]; then
            bad="$bad
  $published $flag ⇒ خرجَ صفرًا وصمتَ في القناتَين / exited 0, silent on both streams"
        else
            echo "✅ $published $flag ⇒ $(printf '%s' "$out" | head -1)"
        fi
    done
    if [ -n "$bad" ]; then
        echo "::error::أدواتٌ من الحزمةِ لا تعمل / tools from the package do not run:$bad"
        return 1
    fi
    echo "✅ كلُّ أداةٍ في الحزمةِ بدأت العملَ وأجابت / every packaged tool started and answered"
}

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **خادمُ اللغةِ يُصافَحُ لا يُستدعى بـ`--version` وحدَه.**
#      `sad-lsp --version` يطبعُ سطرًا ويخرج قبلَ أن يُبنى المحرّكُ أصلًا،
#      فيمرُّ خادمٌ لا يُجيبُ طلبًا واحدًا. المُصافحةُ تُرسِلُ `initialize`
#      مؤطَّرًا بـ`Content-Length` وتطلبُ `capabilities` في الجواب — وهي
#      أقلُّ ما يفعلُه محرّرٌ حقيقيٌّ عندَ أوّلِ فتحِ ملفّ.
# (EN) The language server is handshaked, not merely asked for its version:
#      `--version` prints a line and exits before the engine is ever built, so
#      a server that answers no request would pass. The handshake sends a
#      Content-Length framed `initialize` and requires `capabilities` back —
#      the least a real editor does when it opens the first file.
# ══════════════════════════════════════════════════════════════════════════
sad_smoke_lsp() {
    local bin="$1" body resp
    [ -f "$bin" ] || bin="$1.exe"
    if [ ! -f "$bin" ]; then
        echo "::error::خادمُ اللغةِ غيرُ موجود / language server absent: $1"
        return 1
    fi
    body='{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"processId":null,"rootUri":null,"capabilities":{}}}'
    # (AR) خادمٌ يسقطُ عندَ الإقلاعِ يجبُ أن يُشخَّصَ لا أن يقتلَ الخطوةَ صامتًا
    #      تحتَ `-eo pipefail` — انظرَ التعليلَ في `sad_smoke_package`.
    # (EN) A server that dies on startup must be diagnosed, not silently kill
    #      the step under -eo pipefail — see the note in sad_smoke_package.
    if ! resp="$(printf 'Content-Length: %d\r\n\r\n%s' "${#body}" "$body" | "$bin" --stdio 2>&1)"; then
        echo "::error::خادمُ اللغةِ سقطَ عندَ الإقلاع / the language server died on startup: $bin"
        printf '%s\n' "$resp" | head -20
        return 1
    fi
    case "$resp" in
        *'"capabilities"'*)
            echo "✅ خادمُ اللغةِ صافحَ وأعادَ قدراتِه / language server handshook and returned capabilities"
            return 0 ;;
    esac
    echo "::error::خادمُ اللغةِ لم يُجِبْ مُصافحةَ initialize / no initialize response from the language server"
    printf '%s\n' "$resp" | head -20
    return 1
}

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **`sad --version` أضعفُ ما يقولُه الهُبُّ عن نفسِه.**
#      عَلَمُ الإصدارِ في الهُبِّ يطبعُ ثابتًا وقتَ الترجمةِ ثمّ يخرج، ولا
#      يستشيرُ نتيجةَ الاكتشافِ إطلاقًا. فهُبٌّ لا يعثرُ على شقيقٍ واحدٍ يطبعُ
#      إصدارَه ويخرجُ صفرًا — وهو **عينُ العطبِ الذي شُحِنَ في v1.0.0**:
#      حزمةٌ فيها الهُبُّ وليس فيها ما يُنادِيه، فكلُّ أمرٍ موعودٍ غائب.
#      و`--list` يستنطقُ الاكتشافَ نفسَه، فيُقاسُ ما وُعِدَ به لا ما يُعلَن.
#      والمطلوبُ من القائمةِ ما يبدأُ بـ`sad-` وحدَه: الهُبُّ يمسحُ الشقيقاتِ
#      بهذه البادئةِ حصرًا، فـ`sadc` (بلا شَرطة) لا يُسجَّلُ أصلًا — وهو موثَّقٌ
#      في تعليلِ الجدولِ أعلاه، لا مفاجأةٌ نتغاضى عنها.
# (EN) `sad --version` is the least the hub can say about itself: the hub's
#      version flag prints a compile-time constant and exits without ever
#      consulting the discovery result. A hub that finds zero siblings prints
#      its version and exits 0 — exactly the v1.0.0 defect: a package with the
#      hub and nothing for it to dispatch to, so every promised command is
#      absent. `--list` interrogates discovery itself.
#      Only `sad-` prefixed names are required: the hub scans for that prefix
#      alone, so `sadc` (no dash) is never registered — documented in the tool
#      table's rationale above, not an omission we are glossing over.
# ══════════════════════════════════════════════════════════════════════════
sad_smoke_hub_dispatch() {
    local bindir="$1" published_list="$2"
    local hub out published missing=""
    hub="$bindir/sad"
    [ -f "$hub" ] || hub="$bindir/sad.exe"
    if [ ! -f "$hub" ]; then
        echo "::error::الهُبُّ غيرُ موجود / hub absent: $bindir/sad"
        return 1
    fi
    if ! out="$("$hub" --list 2>&1)"; then
        echo "::error::الهُبُّ سقطَ عندَ سردِ أدواتِه / the hub failed to list its tools"
        printf '%s\n' "$out" | head -20
        return 1
    fi
    for published in $published_list; do
        case "$published" in
            sad-*) ;;
            *) continue ;;
        esac
        printf '%s' "$out" | grep -q -- "$published" || missing="$missing $published"
    done
    if [ -n "$missing" ]; then
        echo "::error::الهُبُّ لا يكتشفُ أدواتٍ موعودةً بجوارِه / the hub does not discover promised siblings:$missing"
        printf '%s\n' "$out" | head -30
        return 1
    fi
    echo "✅ الهُبُّ اكتشفَ كلَّ شقيقٍ موعودٍ / the hub discovered every promised sibling"
}

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **الرابطُ الديناميكيُّ عقدٌ لا يُصرِّحُ به أحد.**
#      شجرةُ البناءِ تربطُ ما تجدُه: `cmake/llvm.cmake` يربطُ `libLLVM-18.so`
#      المشتركةَ متى وُجدت، و`sad_graphics` يربطُ `SDL2::SDL2` علنيًّا فيرثُها
#      `sad-run`. ولا `RPATH` في الشجرةِ كلِّها، والتحزيمُ على يونكسَ لا ينسخُ
#      مكتبةً مشتركةً واحدة. فالحزمةُ تحملُ عقدًا خفيًّا: «عندَك هذه المكتباتُ
#      أم لا تعمل» — عقدًا لم يُكتَبْ في وثيقةٍ ولم يقسْه فحص.
#      و`ldd` يُنطِقُه: كلُّ سطرِ «not found» مكتبةٌ تَعِدُ الحزمةُ ضمنًا بوجودِها
#      على جهازِ المستخدم. وهذا يُقاسُ **قبلَ** التشغيل، لأنّ التشغيلَ يُخفِقُ
#      برمزٍ عامٍّ (127) لا يُسمّي المكتبةَ الناقصةَ في كلِّ الأحوال.
# (EN) The dynamic linker is a contract nobody declares. The build links what
#      it finds: cmake/llvm.cmake links shared libLLVM-18.so when present, and
#      sad_graphics links SDL2::SDL2 PUBLICly so sad-run inherits it. There is
#      no RPATH anywhere in the tree, and Unix packaging copies no shared
#      library at all. The package therefore ships a hidden contract — "have
#      these libraries or it will not run" — written in no document and
#      measured by no check. ldd states it: every "not found" line is a library
#      the package implicitly promises exists on the user's machine. Measured
#      BEFORE execution, because execution fails with a generic 127 that does
#      not always name the missing library.
# ══════════════════════════════════════════════════════════════════════════
sad_require_shared_deps() {
    local bindir="$1" allowed="${2:-}"
    local bin out line lib missing="" checked=0
    command -v ldd >/dev/null 2>&1 || {
        echo "ℹ️ لا ldd على هذه المنصّة — يُتخطّى فحصُ التبعيّاتِ المشتركة / no ldd here, skipping"
        return 0
    }
    for bin in "$bindir"/*; do
        [ -f "$bin" ] || continue
        case "$bin" in *.dll|*.so|*.dylib) continue ;; esac
        if ! out="$(ldd "$bin" 2>/dev/null)"; then continue; fi
        checked=$((checked + 1))
        while IFS= read -r line; do
            case "$line" in
                *"not found"*)
                    lib="$(printf '%s' "$line" | awk '{print $1}')"
                    # (AR) قائمةُ إذنٍ صريحةٌ لمكتبةٍ يَعِدُ بها المُثبِّتُ ووثائقُه.
                    # (EN) Explicit allowlist for a library the installer and its
                    #      documentation genuinely promise the user will have.
                    case " $allowed " in
                        *" $lib "*) ;;
                        *) missing="$missing
  $(basename "$bin") ⇐ $lib" ;;
                    esac ;;
            esac
        done <<< "$out"
    done
    if [ "$checked" -eq 0 ]; then
        echo "::error::لم يُفحَصْ ثنائيٌّ واحدٌ في / no binary examined in: $bindir"
        return 1
    fi
    if [ -n "$missing" ]; then
        echo "::error::الحزمةُ تشترطُ مكتباتٍ مشتركةً لا تحملُها ولا تُعلِنُها / the package requires shared libraries it neither ships nor declares:$missing"
        echo "   العلاجُ أحدُ ثلاثة: شحنُ المكتبةِ في الحزمةِ مع RPATH، أو فتحُها"
        echo "   بـdlopen عندَ الحاجةِ لا ربطُها، أو إعلانُها شرطًا صريحًا يُثبِّتُه المُثبِّت."
        return 1
    fi
    echo "✅ $checked ثنائيًّا: كلُّ تبعيّةٍ مشتركةٍ محلولة / binaries: every shared dependency resolves"
}
