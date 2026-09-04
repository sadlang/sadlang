#!/bin/sh
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# سكريبت تثبيت لغة ص — Linux / macOS
# Sad Programming Language Installer — Linux / macOS  v2.0
# ═══════════════════════════════════════════════════════════════════════════════
#
# الاستخدام / Usage:
#   curl -fsSL https://sad-lang.org/install.sh | sh
#
# خيارات متقدمة / Advanced:
#   ./install.sh                                 # تثبيت تفاعلي (GUI إن توفر)
#   ./install.sh --components standard           # المترجم + المكتبة القياسية + الأدوات الأساسية
#   ./install.sh --components full               # المترجم + كل الأدوات
#   ./install.sh --version 1.2.0                 # إصدار محدد
#   ./install.sh --dir /opt/sad-lang             # مجلد مخصص
#   ./install.sh --no-path                       # لا يضيف لمتغير PATH
#   ./install.sh --no-gui                        # إجبار وضع الطرفية النصية
#   ./install.sh --uninstall                     # إزالة التثبيت
# ═══════════════════════════════════════════════════════════════════════════════

set -e

# ──────────────────────────────────────────────────────────────────────
# الإعدادات / Configuration
# ──────────────────────────────────────────────────────────────────────
# (AR) 🔑 المستودعُ العلنيُّ الذي يصدرُ منه الإصدارُ الرسميّ.
#      وكان يشيرُ إلى مستودعٍ صارَ **خاصًّا**، فكانت كلُّ محاولةِ تثبيتٍ تقعُ
#      على 404، واسمُ مستودعٍ خاصٍّ مكتوبٌ في سكربتٍ يُشحَنُ للناس.
# (EN) 🔑 The public repository the official release is issued from. This
#      pointed at a repository since made PRIVATE, so every install attempt
#      would 404 — and a private repo name sat in a script shipped to users.
REPO_OWNER="sadlang"
REPO_NAME="sadlang"
GITHUB_API="https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}"
DEFAULT_INSTALL_DIR="$HOME/.sad-lang"
VERSION="latest"
COMPONENTS=""
NO_PATH=0
UNINSTALL=0
FORCE_TUI=0
GUI_TOOL=""
GUI_PROGRESS_PID=""

# ──────────────────────────────────────────────────────────────────────
# الألوان / Colors
# ──────────────────────────────────────────────────────────────────────
if [ -t 1 ] && command -v tput >/dev/null 2>&1; then
    RED=$(tput setaf 1); GREEN=$(tput setaf 2); YELLOW=$(tput setaf 3)
    BLUE=$(tput setaf 4); CYAN=$(tput setaf 6); DIM=$(tput dim)
    BOLD=$(tput bold); RESET=$(tput sgr0)
else
    RED=""; GREEN=""; YELLOW=""; BLUE=""; CYAN=""; DIM=""; BOLD=""; RESET=""
fi

logo() {
    printf "\n"
    printf "  %s╔═══════════════════════════════════════════════╗%s\n" "$CYAN" "$RESET"
    printf "  %s║%s     لغة ص — Sad Programming Language        %s║%s\n" "$CYAN" "$RESET" "$CYAN" "$RESET"
    printf "  %s║%s          مُثبّت الإصدار v1.0                %s║%s\n" "$CYAN" "$RESET" "$CYAN" "$RESET"
    printf "  %s╚═══════════════════════════════════════════════╝%s\n\n" "$CYAN" "$RESET"
}

step()  { printf "  %s[●]%s %s\n" "$BLUE"   "$RESET" "$1"; }
ok()    { printf "  %s[✓]%s %s\n" "$GREEN"  "$RESET" "$1"; }
warn()  { printf "  %s[⚠]%s %s\n" "$YELLOW" "$RESET" "$1"; }
err()   { printf "  %s[✗]%s %s\n" "$RED"    "$RESET" "$1"; }
info()  { printf "  %s[→] %s%s\n" "$DIM" "$1" "$RESET"; }
die()   { err "$1"; exit 1; }

# ──────────────────────────────────────────────────────────────────────
# تحليل الوسائط / Parse Arguments
# ──────────────────────────────────────────────────────────────────────
INSTALL_DIR="$DEFAULT_INSTALL_DIR"

while [ $# -gt 0 ]; do
    case "$1" in
        --version|-v)   VERSION="$2";    shift 2 ;;
        --dir|-d)       INSTALL_DIR="$2"; shift 2 ;;
        --components|-c) COMPONENTS="$2"; shift 2 ;;
        --no-path)      NO_PATH=1;       shift ;;
        --no-gui)       FORCE_TUI=1;     shift ;;
        --uninstall|-u) UNINSTALL=1;     shift ;;
        --help|-h)
            printf "Usage: install.sh [OPTIONS]\n"
            printf "  --components X   standard|full\n"
            printf "  --version VER    تثبيت إصدار محدد\n"
            printf "  --dir DIR        مجلد التثبيت (افتراضي: ~/.sad-lang)\n"
            printf "  --no-path        لا يضيف لـ PATH\n"
            printf "  --no-gui         وضع الطرفية النصية فقط\n"
            printf "  --uninstall      إزالة التثبيت\n"
            exit 0 ;;
        *) die "خيار غير معروف: $1" ;;
    esac
done

# (AR) 🔑 المكوّنُ يُحاكَمُ هنا كما يُحاكِمُه التوأم. `install.ps1` يحرسُه
#      بـ`[ValidateSet("standard","full")]`، وهذا كان يقبلُ أيَّ قيمةٍ صامتًا:
#      فرعاه الافتراضيّانِ يسقطانِ إلى `PREFIX="sad"` و`REQUIRED_TOOLS="sad"`،
#      فتُنزَّلُ الحزمةُ القياسيّةُ باسمِ شيءٍ آخرَ ثمّ يُقالُ «تمّ» فوقَ تثبيتٍ
#      لم يُحاكَمْ إلّا على أداةٍ واحدة. والقيمةُ القديمةُ `compiler` أو
#      `interpreter` في نصٍّ قديمٍ تسلكُ هذا المسلكَ بعينِه.
# (EN) 🔑 The component is judged here as its twin judges it. install.ps1
#      guards it with a ValidateSet; this accepted anything silently — its two
#      default arms fall back to PREFIX="sad" and REQUIRED_TOOLS="sad", so the
#      standard package downloads under another name and then reports success
#      over an install verified against a single tool. A stale `compiler` or
#      `interpreter` value in an old script takes exactly that path.
if [ -n "$COMPONENTS" ]; then
    case "$COMPONENTS" in
        standard|full) ;;
        *) die "مكوّن غير معروف: '$COMPONENTS' — المتاح: standard|full" ;;
    esac
fi

# ══════════════════════════════════════════════════════════════════════
# اكتشاف أداة الواجهة الرسومية / Detect GUI Tool
# ══════════════════════════════════════════════════════════════════════
detect_gui_tool() {
    if [ "$FORCE_TUI" = "1" ]; then
        GUI_TOOL="none"; return
    fi
    # لا شاشة = لا GUI
    if [ -z "${DISPLAY:-}" ] && [ -z "${WAYLAND_DISPLAY:-}" ]; then
        GUI_TOOL="none"; return
    fi
    if command -v zenity >/dev/null 2>&1; then
        GUI_TOOL="zenity"
    elif command -v kdialog >/dev/null 2>&1; then
        GUI_TOOL="kdialog"
    else
        GUI_TOOL="none"
    fi
}

# ──────────────────────────────────────────────────────────────────────
# دوال واجهة رسومية موحدة / Unified GUI Wrappers
# ──────────────────────────────────────────────────────────────────────
gui_info() {
    local title="$1" msg="$2"
    case "$GUI_TOOL" in
        zenity)  zenity --info --title="$title" --text="$msg" --width=480 2>/dev/null || true ;;
        kdialog) kdialog --title "$title" --msgbox "$msg" 2>/dev/null || true ;;
    esac
}

gui_yesno() {
    local title="$1" msg="$2"
    case "$GUI_TOOL" in
        zenity)  zenity --question --title="$title" --text="$msg" --width=460 2>/dev/null ;;
        kdialog) kdialog --title "$title" --yesno "$msg" 2>/dev/null ;;
        *) return 1 ;;
    esac
}

gui_list() {
    local title="$1" msg="$2"
    shift 2
    case "$GUI_TOOL" in
        zenity)
            zenity --list --title="$title" --text="$msg" \
                   --column="الاختيار" --column="الوصف" \
                   --height=320 --width=560 "$@" 2>/dev/null ;;
        kdialog)
            kdialog --title "$title" --menu "$msg" "$@" 2>/dev/null ;;
        *) return 1 ;;
    esac
}

gui_folder() {
    local title="$1" default="$2"
    case "$GUI_TOOL" in
        zenity)  zenity --file-selection --directory --title="$title" --filename="$default/" 2>/dev/null ;;
        kdialog) kdialog --title "$title" --getexistingdirectory "$default" 2>/dev/null ;;
        *) echo "$default" ;;
    esac
}

# ══════════════════════════════════════════════════════════════════════
# اختيار المكونات / Component Selection
# ══════════════════════════════════════════════════════════════════════
show_menu() {
    if [ -n "$COMPONENTS" ]; then return; fi

    if [ "$GUI_TOOL" != "none" ]; then
        local choice
        choice=$(gui_list "مُثبّت لغة ص — اختر المكونات" \
            "اختر المكونات التي تريد تثبيتها:" \
            "standard" "المترجم (sad) + المكتبة القياسية ← موصى به للمبتدئين" \
            "full"     "الحزمة الكاملة — المترجم + LSP + REPL + مدير الحزم + المنسّق" \
        ) 2>/dev/null || choice="standard"
        case "$choice" in
            full)     COMPONENTS="full"     ;;
            *)        COMPONENTS="standard" ;;
        esac
    else
        printf "  %s╔═══════════════════════════════════════════════════════╗%s\n" "$CYAN" "$RESET"
        printf "  %s║  اختر ما تريد تثبيته / Choose what to install:     ║%s\n" "$CYAN" "$RESET"
        printf "  %s╠═══════════════════════════════════════════════════════╣%s\n" "$CYAN" "$RESET"
        printf "  ║  %s[1]%s الحزمة القياسية (standard)                  ║\n" "$GREEN" "$RESET"
        printf "  ║  %s    sad + المكتبة القياسية ← الأفضل للمبتدئين%s     ║\n" "$DIM" "$RESET"
        printf "  ║  %s[2]%s الحزمة الكاملة (full)                        ║\n" "$CYAN" "$RESET"
        printf "  ║  %s    + LSP + REPL + مدير الحزم + المنسّق%s           ║\n" "$DIM" "$RESET"
        printf "  %s╚═══════════════════════════════════════════════════════╝%s\n\n" "$CYAN" "$RESET"
        printf "  اختر رقم (1/2) [الافتراضي: 1]: "
        read -r choice
        case "$choice" in
            2) COMPONENTS="full"     ;;
            *) COMPONENTS="standard" ;;
        esac
    fi

    case "$COMPONENTS" in
        full) ok "تم اختيار: الحزمة الكاملة" ;;
        *)    ok "تم اختيار: الحزمة القياسية (sad)" ;;
    esac
    printf "\n"
}

# ──────────────────────────────────────────────────────────────────────
# تحديد النظام / Detect OS & Architecture
# ──────────────────────────────────────────────────────────────────────
detect_platform() {
    OS="$(uname -s)"
    ARCH="$(uname -m)"
    case "$OS" in
        Linux)   PLATFORM="linux" ;;
        Darwin)  PLATFORM="macos" ;;
        MINGW*|MSYS*|CYGWIN*)
            err "أنت على Windows — استخدم PowerShell:"
            info "irm https://sad-lang.org/install.ps1 | iex"
            exit 1 ;;
        *) die "نظام غير مدعوم: $OS" ;;
    esac
    case "$ARCH" in
        x86_64|amd64)  ARCH="x86_64"  ;;
        aarch64|arm64) ARCH="aarch64" ;;
        armv7l)        ARCH="armv7"   ;;
        *) die "بنية غير مدعومة: $ARCH" ;;
    esac
    ok "النظام: $PLATFORM ($ARCH)"
}

# ══════════════════════════════════════════════════════════════════════
# (AR) 🔑 **نُزعت آلةُ LLVM كاملةً — لأنّ شرطَها زال.**
#
#      كانت هنا ثلاثُ مئةٍ ونيّفٌ من الأسطر: كشفُ LLVM، والتحقّقُ من مجلّدِها،
#      وتثبيتُها آليًّا عبرَ خمسةِ مديري حزم، وقائمتانِ (رسوميّةٌ ونصّيّة)
#      تسألانِ المستخدمَ ماذا يفعلُ إن لم تُوجَد. وكلُّ ذلك كان لأنّ حزمةَ
#      المترجمِ على لينكس تربطُ `libLLVM-18.so` مشتركةً.
#
#      وقد صارَ الربطُ ساكنًا (`SAD_LLVM_STATIC=ON` في مجرى الإصدار)، فالحزمُ
#      مكتفيةٌ بذاتِها على المنصّاتِ كلِّها ولا تشترطُ شيئًا على جهازِ
#      المستخدم. فلو بقيت هذه الآلةُ لسألت عمّا لا يلزم، ولوعدت بشرطٍ لا
#      وجودَ له — وهو الكذبُ الذي تُنشَأُ الحرّاسُ لمنعِه.
#
#      ومعها زالَ مسارُ التراجعِ «إن غابت LLVM فثبّتِ المفسّرَ بدلًا» في ثمانيةِ
#      مواضع: لا مفسّرَ يُتراجَعُ إليه بعدَ حذفِه، وكلتا الحزمتَينِ تحملانِ
#      المترجِم.
#
#      والدعوى مقيسةٌ في مجرى الإصدار: فحصُ التبعيّاتِ المشتركةِ هناك لا يأذنُ
#      بـ`libLLVM` البتّة، فلو عادَ الربطُ مشتركًا احمرَّ التحزيمُ قبلَ النشر.
# (EN) 🔑 The whole LLVM subsystem is removed — its premise is gone.
#
#      Three hundred-odd lines lived here: detection, directory validation,
#      automatic installation across five package managers, and two menus (GUI
#      and TUI) asking the user what to do when LLVM is missing. All of it
#      existed because the Linux compiler package linked libLLVM-18.so.
#      Linking is now static (SAD_LLVM_STATIC=ON in the release workflow), so
#      the packages are self-contained on every platform and require nothing on
#      the user's machine. Keeping this machinery would ask about what is not
#      needed and promise a prerequisite that no longer exists.
#      With it goes the "no LLVM ⇒ install the interpreter instead" fallback in
#      eight places: there is no interpreter to fall back to, and both packages
#      carry the compiler.
#      The claim is measured in the release workflow: its shared-dependency
#      check allows no libLLVM at all, so a return to dynamic linking reddens
#      packaging before anything is published.
# ══════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# التحقق من أدوات التحميل / Check Download Tools
# ──────────────────────────────────────────────────────────────────────
check_deps() {
    step "التحقق من الأدوات المطلوبة..."

    if command -v curl >/dev/null 2>&1; then
        DOWNLOADER="curl"; ok "curl موجود"
    elif command -v wget >/dev/null 2>&1; then
        DOWNLOADER="wget"; ok "wget موجود"
    else
        die "يجب تثبيت curl أو wget أولاً"
    fi

    command -v tar >/dev/null 2>&1 || die "يجب تثبيت tar"
    ok "tar موجود"
}

download() {
    if [ "$DOWNLOADER" = "curl" ]; then curl -fsSL "$1" -o "$2"
    else wget -q "$1" -O "$2"; fi
}

fetch_json() {
    if [ "$DOWNLOADER" = "curl" ]; then curl -fsSL "$1"
    else wget -qO- "$1"; fi
}

# ──────────────────────────────────────────────────────────────────────
# جلب الإصدار / Get Release Info
# ──────────────────────────────────────────────────────────────────────
get_release_info() {
    step "البحث عن الإصدار من GitHub Releases..."

    if [ "$VERSION" = "latest" ]; then
        RELEASE_URL="$GITHUB_API/releases/latest"
    else
        RELEASE_URL="$GITHUB_API/releases/tags/v${VERSION}"
    fi

    RELEASE_JSON=$(fetch_json "$RELEASE_URL") || die "فشل في جلب معلومات الإصدار"

    ACTUAL_VERSION=$(printf '%s' "$RELEASE_JSON" | \
        grep -o '"tag_name"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | \
        sed 's/.*"v\{0,1\}\([^"]*\)".*/\1/')
    [ -z "$ACTUAL_VERSION" ] && die "لم يُعثر على إصدار"

    ok "الإصدار: v${ACTUAL_VERSION}"
}

# ──────────────────────────────────────────────────────────────────────
# تحميل وتثبيت / Download & Install
# ──────────────────────────────────────────────────────────────────────
install_sad() {
    case "$COMPONENTS" in
        full) PREFIX="sad-full" ;;
        *)    PREFIX="sad"      ;;
    esac

    ARCHIVE_NAME="${PREFIX}-v${ACTUAL_VERSION}-${PLATFORM}-${ARCH}.tar.gz"

    DOWNLOAD_URL=$(printf '%s' "$RELEASE_JSON" | \
        grep -o '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]*'"${ARCHIVE_NAME}"'"' | \
        head -1 | sed 's/.*"\(https[^"]*\)".*/\1/')

    if [ -z "$DOWNLOAD_URL" ]; then
        for alt in \
            "${PREFIX}-v${ACTUAL_VERSION}-${PLATFORM}-${ARCH}.tar.gz" \
            "${PREFIX}-${ACTUAL_VERSION}-${PLATFORM}-${ARCH}.tar.gz" \
            "${PREFIX}-${PLATFORM}-${ARCH}.tar.gz"; do
            DOWNLOAD_URL=$(printf '%s' "$RELEASE_JSON" | \
                grep -o '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]*'"${alt}"'"' | \
                head -1 | sed 's/.*"\(https[^"]*\)".*/\1/')
            [ -n "$DOWNLOAD_URL" ] && ARCHIVE_NAME="$alt" && break
        done
    fi

    if [ -z "$DOWNLOAD_URL" ]; then
        err "لم يُعثر على ملف تحميل للمكون '$COMPONENTS' على ${PLATFORM}-${ARCH}"
        info "الملفات المتاحة:"
        printf '%s' "$RELEASE_JSON" | grep -o '"name"[[:space:]]*:[[:space:]]*"[^"]*"' | \
            sed 's/"name"[[:space:]]*:[[:space:]]*"/  - /;s/"$//'
        exit 1
    fi

    TEMP_DIR=$(mktemp -d)
    trap 'rm -rf "$TEMP_DIR"' EXIT INT TERM

    step "تحميل $ARCHIVE_NAME..."
    info "المصدر: github.com/$REPO_OWNER/$REPO_NAME"
    info "الرابط: $DOWNLOAD_URL"

    if [ "$GUI_TOOL" != "none" ]; then
        gui_info "جاري التحميل..." "تحميل $ARCHIVE_NAME من GitHub Releases...\n\nقد يستغرق ذلك بضع دقائق."
    fi

    download "$DOWNLOAD_URL" "$TEMP_DIR/$ARCHIVE_NAME" || die "فشل تحميل الملف"

    FILE_SIZE=$(du -h "$TEMP_DIR/$ARCHIVE_NAME" 2>/dev/null | cut -f1 || echo "؟")
    ok "تم التحميل (${FILE_SIZE})"

    step "فك الضغط والتثبيت..."
    [ -d "$INSTALL_DIR" ] && rm -rf "$INSTALL_DIR"
    mkdir -p "$INSTALL_DIR" "$TEMP_DIR/فك"

    tar -xzf "$TEMP_DIR/$ARCHIVE_NAME" -C "$TEMP_DIR/فك" || die "فشل فك الضغط"

    # (AR) الأرشيف يحمل مجلّداً أعلى واحداً (sad-vX-منصّة-معماريّة/) — يُقشَّر.
    #      وقبل هذا التقشير كان الفكّ يقع في $INSTALL_DIR كما هو، فيصير المسار
    #      $INSTALL_DIR/sad-vX-.../bin، بينما PATH يشير إلى $INSTALL_DIR/bin —
    #      وهو مجلّد أُنشئ فارغاً ويبقى فارغاً: حلقة النقل التالية كانت تستثني
    #      كلّ ما وقع تحت */bin/*، أي تستثني كلّ الأدوات. فالتثبيت "ينجح"
    #      ولا أمر واحد يعمل.
    # (EN) The archive carries a single top directory; strip it. Before this,
    #      extraction landed at $INSTALL_DIR/sad-vX.../bin while PATH pointed at
    #      $INSTALL_DIR/bin — created empty and left empty, because the move
    #      loop excluded everything under */bin/*, i.e. every tool. The install
    #      "succeeded" and not one command worked.
    EXTRACT_ROOT=$(find "$TEMP_DIR/فك" -mindepth 1 -maxdepth 1 -type d 2>/dev/null | head -1)
    if [ -n "$EXTRACT_ROOT" ] && [ -d "$EXTRACT_ROOT/bin" ]; then
        cp -R "$EXTRACT_ROOT/." "$INSTALL_DIR/" || die "فشل نسخ محتوى الحزمة"
    else
        cp -R "$TEMP_DIR/فك/." "$INSTALL_DIR/" || die "فشل نسخ محتوى الحزمة"
    fi

    BIN_DIR="$INSTALL_DIR/bin"
    mkdir -p "$BIN_DIR"

    # (AR) شبكة أمان لحزم قديمة لا تحمل bin/ — تُنقل التنفيذيّات إليها.
    # (EN) Safety net for older packages with no bin/ directory.
    for tool in sad sadc sad-build sad-check sad-lsp sad-pkg sad-fmt; do
        tool_bin=$(find "$INSTALL_DIR" -name "$tool" -type f ! -path "*/bin/*" 2>/dev/null | head -1)
        if [ -n "$tool_bin" ] && [ ! -f "$BIN_DIR/$tool" ]; then
            cp "$tool_bin" "$BIN_DIR/"
        fi
    done

    chmod +x "$BIN_DIR"/* 2>/dev/null || true

    # (AR) حكمٌ لا وصف: تثبيتٌ بلا أمرٍ واحدٍ قابلٍ للتشغيل إخفاقٌ يُعلَن،
    #      لا رسالةُ نجاحٍ يكتشف المستخدم كذبَها بعد ساعة.
    # (EN) A judgement, not a description: an install with no runnable command
    #      is a failure to announce, not a success message to be disproved later.
    # (AR) 🔑 هذه القوائمُ نسخةٌ رابعةٌ من جدولِ الأدوات، وقد انجرفت فعلًا:
    #      كانت «compiler» تطلبُ `sadc` وحدَه و«full» تُغفِلُ `sad-build`،
    #      بينما يشترطُهما حَكَمُ الإصدارِ في scripts/ci/release_tools.sh —
    #      أي مُثبِّتٌ يقبلُ حزمةً يرفضُها المُصدِر. والمُثبِّتُ يُشحَنُ وحدَه
    #      إلى المستخدمِ فلا يستطيعُ استيرادَ الجدول، فالرباطُ حارسٌ:
    #      scripts/ci/check_installer_tool_lists.py يُطابِقُ القائمتَين.
    # (EN) These lists are a FOURTH copy of the tool table and had already
    #      drifted: "compiler" required only sadc and "full" omitted sad-build,
    #      both of which the release judge requires — an installer accepting a
    #      package the publisher rejects. The installer ships standalone and
    #      cannot source the table, so the binding is a guard:
    #      scripts/ci/check_installer_tool_lists.py matches the two.
    case "$COMPONENTS" in
        # (AR) 🔑 كان المكوّنُ يُدعى «المفسّر» ويشحن sad وsad-lsp وsad-check —
        #      ولا واحدةَ منها تُشغّلُ برنامجَ ص، وsad-build ليس فيه. فمن اختارَه
        #      حصلَ على تثبيتٍ لا يُشغّلُ شيئًا ويجتازُ الحارسَ أخضرَ. أُضيف
        #      sad-build ثمّ زالَ اسمُ «المفسّر» نفسُه: صارَ المكوّنُ «القياسيّ».
        # (EN) The "interpreter" component shipped nothing that runs a ص program
        #      and omitted sad-build. sad-build was added, and then the name
        #      "interpreter" itself went: the component is now "standard".
        standard) REQUIRED_TOOLS="sad sad-build sad-lsp sad-check" ;;
        full)     REQUIRED_TOOLS="sad sad-lsp sad-check sadc sad-build" ;;
        *)        REQUIRED_TOOLS="sad" ;;
    esac
    MISSING_TOOLS=""
    for tool in $REQUIRED_TOOLS; do
        [ -s "$BIN_DIR/$tool" ] || MISSING_TOOLS="$MISSING_TOOLS $tool"
    done
    if [ -n "$MISSING_TOOLS" ]; then
        err "الحزمة ناقصة — أدوات موعودة غائبة عن $BIN_DIR:$MISSING_TOOLS"
        info "الموجود فعلاً:"
        ls -1 "$BIN_DIR" 2>/dev/null | sed 's/^/  - /'
        die "توقّف التثبيت — لا تُترك أدوات ناقصة على الجهاز بصمت"
    fi

    ok "تم تثبيت الملفات في: $INSTALL_DIR"

    # إضافة PATH
    if [ "$NO_PATH" = "0" ]; then
        step "إضافة لغة ص لمتغير PATH..."
        CURRENT_SHELL=$(basename "${SHELL:-bash}")
        RC_FILE="$HOME/.${CURRENT_SHELL}rc"
        [ ! -f "$RC_FILE" ] && RC_FILE="$HOME/.profile"

        if ! grep -q "$BIN_DIR" "$RC_FILE" 2>/dev/null; then
            {
                printf '\n# sad-lang — لغة ص\n'
                printf 'export PATH="%s:$PATH"\n' "$BIN_DIR"
                printf 'export SAD_HOME="%s"\n' "$INSTALL_DIR"
                printf 'export SAD_STDLIB="%s/stdlib"\n' "$INSTALL_DIR"
            } >> "$RC_FILE"
            export PATH="$BIN_DIR:$PATH"
            ok "تمت إضافة $BIN_DIR إلى PATH في $RC_FILE"
        else
            ok "المسار موجود بالفعل في PATH"
        fi
    fi

    # كتابة ملف معلومات التثبيت
    cat > "$INSTALL_DIR/install-info.json" <<EOF
{
  "version":     "$ACTUAL_VERSION",
  "components":  "$COMPONENTS",
  "platform":    "$PLATFORM",
  "arch":        "$ARCH",
  "installDir":  "$INSTALL_DIR",
  "binDir":      "$BIN_DIR",
  "installedAt": "$(date -u +"%Y-%m-%dT%H:%M:%SZ")",
  "installer":   "install.sh v2.0"
}
EOF

    # عرض المكونات المثبتة
    step "المكونات المثبتة:"
    for tool in "$BIN_DIR"/*; do
        [ -x "$tool" ] || continue
        TNAME=$(basename "$tool")
        case "$TNAME" in
            sad)            TDESC="مركز الأدوات — sad build/check/fmt" ;;
            sadc)           TDESC="المترجم — يحوّل .ص إلى ملف تنفيذي أصلي" ;;
            sad-lsp*)       TDESC="خادم LSP — تكامل VS Code / Vim / Neovim" ;;
            sad-pkg)        TDESC="مدير الحزم — تثبيت مكتبات لغة ص" ;;
            sad-fmt)        TDESC="أداة تنسيق الكود" ;;
            *)              TDESC="$TNAME" ;;
        esac
        info "  $TNAME — $TDESC"
    done

    # التحقق النهائي
    step "التحقق من التثبيت..."
    # (AR) 🔑 لا إملاءَ واحدًا للجميع. `language-truth/cli_flags.yaml` يجعلُ
    #      `--إصدار` الاسمَ الطويلَ الوحيدَ للمحرّكَين وقد أُلغيت المرادفاتُ
    #      الإنجليزيّةُ عمدًا، بينما الهُبُّ خارجَ ذلك الجدولِ ويقبلُ الإنجليزيّة.
    #      وكان السطرُ يكتبُ `--version` للاثنَين، فيردُّه المترجمُ «خيارًا
    #      غيرَ معروف» ويفشل — ولا يُؤخَذُ اسمَ ملفٍّ يُترجَم: ذاك مسارُ
    #      الوسائطِ التي لا تبدأُ بشَرْطة. فيطبعُ المُثبِّتُ فراغًا في موضعِ
    #      الإصدارِ ويقولُ «تمّ» فوقَه.
    #      والمُثبِّتُ يُشحَنُ وحدَه فلا يستطيعُ استيرادَ جدولِ الأدوات، ونسخُ
    #      الجدولِ هنا يصنعُ نسخةً رابعةً تنجرف — فيُجرَّبُ الإملاءانِ ويُؤخَذُ
    #      ما نجح. والتساهلُ هنا في محلِّه: هذا سطرُ عرضٍ لا حَكَم.
    # (EN) There is no single spelling. cli_flags.yaml makes --إصدار the only
    #      long name for the two engines (English synonyms deliberately
    #      abolished), while the hub sits outside that table and takes English.
    #      This line wrote --version for both, and the compiler rejects it as
    #      an unknown option and fails — the installer printed nothing where the
    #      version belongs and called the install done.
    #      The installer ships standalone and cannot import the tool table, and
    #      copying it here would create a fourth copy that drifts — so both
    #      spellings are tried and whichever answers is used. Being forgiving is
    #      right here: this is a display line, not a judgement.
    for exe in sad sadc; do
        if [ -x "$BIN_DIR/$exe" ]; then
            V=$("$BIN_DIR/$exe" --إصدار 2>/dev/null | head -1) || V=""
            [ -n "$V" ] || V=$("$BIN_DIR/$exe" --version 2>/dev/null | head -1) || V=""
            [ -n "$V" ] || V="موجود"
            ok "$exe: $V"
        fi
    done
}

# ──────────────────────────────────────────────────────────────────────
# إزالة التثبيت / Uninstall
# ──────────────────────────────────────────────────────────────────────
uninstall_sad() {
    logo
    printf "  %sإزالة لغة ص...%s\n\n" "$YELLOW" "$RESET"

    if [ ! -d "$INSTALL_DIR" ]; then
        warn "لغة ص غير مثبتة في: $INSTALL_DIR"
        exit 0
    fi

    step "إزالة من PATH..."
    BIN_DIR="$INSTALL_DIR/bin"
    for RC in "$HOME/.bashrc" "$HOME/.zshrc" "$HOME/.profile"; do
        if [ -f "$RC" ] && grep -q "sad-lang\|SAD_HOME\|sad-llvm" "$RC" 2>/dev/null; then
            grep -v "sad-lang\|SAD_HOME\|SAD_STDLIB\|sad-llvm\|LLVM_DIR.*sad\|$BIN_DIR\|$INSTALL_DIR" "$RC" > "${RC}.tmp.sad" && mv "${RC}.tmp.sad" "$RC"
            ok "تمت إزالة الإعدادات من $RC"
        fi
    done

    step "حذف الملفات..."
    rm -rf "$INSTALL_DIR"
    ok "تم حذف $INSTALL_DIR"

    printf "\n  %s✓ تمت إزالة لغة ص بنجاح%s\n" "$GREEN" "$RESET"
    printf "    %sأعد فتح الطرفية لتحديث PATH%s\n\n" "$DIM" "$RESET"
    exit 0
}

# ══════════════════════════════════════════════════════════════════════
# نقطة الدخول / Entry Point
# ══════════════════════════════════════════════════════════════════════
logo
detect_gui_tool

if [ "$GUI_TOOL" != "none" ]; then
    info "واجهة رسومية مكتشفة: $GUI_TOOL"
fi

if [ "$UNINSTALL" = "1" ]; then uninstall_sad; fi

printf "  مرحباً! سيتم تثبيت لغة ص على جهازك.\n"
printf "  المصدر: github.com/%s/%s\n" "$REPO_OWNER" "$REPO_NAME"
printf "  المجلد: %s\n\n" "$INSTALL_DIR"

show_menu
detect_platform
check_deps
get_release_info
install_sad

# رسالة الانتهاء
case "$COMPONENTS" in
    full) COMP_NAME="الحزمة الكاملة" ;;
    *)    COMP_NAME="الحزمة القياسية (sad)" ;;
esac

printf "\n"
printf "  %s═══════════════════════════════════════════════%s\n" "$GREEN" "$RESET"
printf "  %s✓ تم تثبيت %s v%s بنجاح!%s\n" "$GREEN" "$COMP_NAME" "$ACTUAL_VERSION" "$RESET"
printf "  %s═══════════════════════════════════════════════%s\n\n" "$GREEN" "$RESET"
printf "  للبدء:\n"

printf "    %ssad --help%s          عرض المساعدة\n"   "$DIM" "$RESET"
printf "    %ssad build script.ص%s  ترجمة إلى ملف تنفيذي\n" "$DIM" "$RESET"
if [ "$COMPONENTS" = "full" ]; then
    printf "    %ssadc script.ص%s      ترجمة إلى ملف تنفيذي\n" "$DIM" "$RESET"
fi
if [ "$COMPONENTS" = "full" ]; then
    printf "    %ssad-pkg init%s       إنشاء مشروع جديد\n" "$DIM" "$RESET"
fi

CURRENT_SHELL=$(basename "${SHELL:-bash}")
printf "\n  %s⚡ فعّل التغييرات:%s\n" "$YELLOW" "$RESET"
printf "    %ssource ~/.%src%s\n"         "$DIM" "$CURRENT_SHELL" "$RESET"
printf "    %sأو أعد فتح الطرفية%s\n\n"  "$DIM" "$RESET"

if [ "$GUI_TOOL" != "none" ]; then
    gui_info "✓ تم التثبيت بنجاح!" "تم تثبيت لغة ص $COMP_NAME v$ACTUAL_VERSION بنجاح!\n\nالمجلد: $INSTALL_DIR\n\nأعد فتح الطرفية لتفعيل الأوامر."
fi