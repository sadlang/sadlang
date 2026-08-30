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
#      is exactly what v1.0.0 shipped. Note also that the target literally
#      named `sad` lives in tools/build/, a self-contained project() that no
#      add_subdirectory ever adds: unbuilt for a long time, and not this.
SAD_TOOL_TABLE="
sad_hub:sad-hub:sad
sad-run:sad-run:sad-run
sad-lsp-server:sad-lsp:sad-lsp
sad_check:sad-check:sad-check
sad-pkg:sad-pkg:sad-pkg
sad-repl:sad-repl:sad-repl
sad-fmt:sad-fmt:sad-fmt
sad-build:sad-build:sadc
sad-build:sad-build:sad-build
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
SAD_REQUIRED_INTERPRETER="sad sad-run sad-lsp sad-check"
SAD_REQUIRED_COMPILER="sadc sad-build"
SAD_REQUIRED_FULL="sad sad-run sad-lsp sad-check sadc sad-build"

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
sad_require_archive() {
    local archive="$1" published_list="$2" listing published missing=""
    if [ ! -f "$archive" ]; then
        echo "::error::أرشيفٌ مفقود / missing archive: $archive"
        return 1
    fi
    case "$archive" in
        *.zip)    listing="$(unzip -Z1 "$archive")" ;;
        *.tar.gz) listing="$(tar -tzf "$archive")" ;;
        *)
            echo "::error::صيغةُ أرشيفٍ غيرُ معروفة / unknown archive format: $archive"
            return 1
            ;;
    esac
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
