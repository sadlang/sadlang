#!/bin/sh
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# سكريبت تثبيت لغة ص — Linux / macOS
# Sad Programming Language Installer — Linux / macOS
# ═══════════════════════════════════════════════════════════════════════════════
#
# الاستخدام / Usage:
#   curl -fsSL https://sad-lang.org/install.sh | sh
#
# خيارات متقدمة / Advanced:
#   ./install.sh                                 # تثبيت تفاعلي
#   ./install.sh --components full               # المفسر + المترجم + كل الأدوات
#   ./install.sh --components compiler           # المترجم فقط
#   ./install.sh --components interpreter        # المفسر فقط
#   ./install.sh --version 1.2.0                 # إصدار محدد
#   ./install.sh --dir /opt/sad-lang             # مجلد مخصص
#   ./install.sh --no-path                       # لا يضيف لمتغير PATH
#   ./install.sh --uninstall                     # إزالة التثبيت
#
# ═══════════════════════════════════════════════════════════════════════════════
#
# مصدر التحميل / Download Source:
#   يتم تحميل الملفات من GitHub Releases:
#   https://github.com/SalehKadah/s-programming-language/releases
#
#   ملفات التحميل المتاحة لكل إصدار:
#   ┌─────────────────────────────────────────────────────────────────┐
#   │ sad-v{VER}-{OS}-{ARCH}.tar.gz       المفسر + الأدوات الأساسية │
#   │ sad-full-v{VER}-{OS}-{ARCH}.tar.gz  المفسر + المترجم + كل شيء │
#   │ sadc-v{VER}-{OS}-{ARCH}.tar.gz      المترجم فقط (LLVM)        │
#   │                                                                │
#   │ {OS}:   linux, macos                                           │
#   │ {ARCH}: x86_64, aarch64                                       │
#   └─────────────────────────────────────────────────────────────────┘
#
# ═══════════════════════════════════════════════════════════════════════════════

set -e

# ──────────────────────────────────────────────────────────────────────
# الإعدادات / Configuration
# ──────────────────────────────────────────────────────────────────────
REPO_OWNER="SalehKadah"
REPO_NAME="s-programming-language"
GITHUB_API="https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}"
DEFAULT_INSTALL_DIR="$HOME/.sad-lang"
VERSION="latest"
COMPONENTS=""
NO_PATH=0
UNINSTALL=0

# ──────────────────────────────────────────────────────────────────────
# الألوان / Colors
# ──────────────────────────────────────────────────────────────────────
if [ -t 1 ] && command -v tput >/dev/null 2>&1; then
    RED=$(tput setaf 1); GREEN=$(tput setaf 2); YELLOW=$(tput setaf 3)
    BLUE=$(tput setaf 4); CYAN=$(tput setaf 6); DIM=$(tput dim); RESET=$(tput sgr0)
else
    RED="" GREEN="" YELLOW="" BLUE="" CYAN="" DIM="" RESET=""
fi

logo() {
    printf "\n"
    printf "  ${CYAN}╔═══════════════════════════════════════════════╗${RESET}\n"
    printf "  ${CYAN}║${RESET}     لغة ص — Sad Programming Language        ${CYAN}║${RESET}\n"
    printf "  ${CYAN}║${RESET}          مُثبّت الإصدار v1.0                ${CYAN}║${RESET}\n"
    printf "  ${CYAN}╚═══════════════════════════════════════════════╝${RESET}\n\n"
}

step()  { printf "  ${BLUE}[●]${RESET} %s\n" "$1"; }
ok()    { printf "  ${GREEN}[✓]${RESET} %s\n" "$1"; }
warn()  { printf "  ${YELLOW}[⚠]${RESET} %s\n" "$1"; }
err()   { printf "  ${RED}[✗]${RESET} %s\n" "$1"; }
info()  { printf "  ${DIM}[→] %s${RESET}\n" "$1"; }
die()   { err "$1"; exit 1; }

# ──────────────────────────────────────────────────────────────────────
# تحليل الوسائط / Parse Arguments
# ──────────────────────────────────────────────────────────────────────
INSTALL_DIR="$DEFAULT_INSTALL_DIR"

while [ $# -gt 0 ]; do
    case "$1" in
        --version|-v)      VERSION="$2"; shift 2 ;;
        --dir|-d)          INSTALL_DIR="$2"; shift 2 ;;
        --components|-c)   COMPONENTS="$2"; shift 2 ;;
        --no-path)         NO_PATH=1; shift ;;
        --uninstall|-u)    UNINSTALL=1; shift ;;
        --help|-h)
            printf "Usage: install.sh [OPTIONS]\n"
            printf "  --components X  interpreter|compiler|full (default: interactive)\n"
            printf "  --version VER   تثبيت إصدار محدد (default: latest)\n"
            printf "  --dir DIR       مجلد التثبيت (default: ~/.sad-lang)\n"
            printf "  --no-path       لا يضيف لمتغير PATH\n"
            printf "  --uninstall     إزالة التثبيت\n"
            exit 0 ;;
        *) die "خيار غير معروف: $1" ;;
    esac
done

# ──────────────────────────────────────────────────────────────────────
# قائمة الاختيار التفاعلية / Interactive Component Selection
# ──────────────────────────────────────────────────────────────────────
show_menu() {
    if [ -n "$COMPONENTS" ]; then return; fi

    printf "  ╔═══════════════════════════════════════════════════════╗\n"
    printf "  ║  اختر ما تريد تثبيته / Choose what to install:     ║\n"
    printf "  ╠═══════════════════════════════════════════════════════╣\n"
    printf "  ║                                                     ║\n"
    printf "  ║  ${GREEN}[1]${RESET} المفسر فقط (interpreter)                    ║\n"
    printf "  ║  ${DIM}    sad + المكتبة القياسية + الأدوات${RESET}              ║\n"
    printf "  ║  ${GREEN}    ← الأفضل لمعظم المستخدمين${RESET}                   ║\n"
    printf "  ║                                                     ║\n"
    printf "  ║  ${YELLOW}[2]${RESET} المترجم فقط (compiler)                      ║\n"
    printf "  ║  ${DIM}    sadc — يحوّل .ص إلى ملف تنفيذي أصلي${RESET}          ║\n"
    printf "  ║  ${YELLOW}    ⚠ يتطلب LLVM 18${RESET}                             ║\n"
    printf "  ║                                                     ║\n"
    printf "  ║  ${CYAN}[3]${RESET} الحزمة الكاملة (full)                        ║\n"
    printf "  ║  ${DIM}    المفسر + المترجم + LSP + REPL + مدير الحزم${RESET}    ║\n"
    printf "  ║  ${CYAN}    ← كل شيء في حزمة واحدة${RESET}                       ║\n"
    printf "  ║                                                     ║\n"
    printf "  ╚═══════════════════════════════════════════════════════╝\n\n"

    printf "  اختر رقم (1/2/3) [الافتراضي: 1]: "
    read -r choice
    case "$choice" in
        2) COMPONENTS="compiler" ;;
        3) COMPONENTS="full" ;;
        *) COMPONENTS="interpreter" ;;
    esac

    case "$COMPONENTS" in
        interpreter) ok "تم اختيار: المفسر (sad)" ;;
        compiler)    ok "تم اختيار: المترجم (sadc)" ;;
        full)        ok "تم اختيار: الحزمة الكاملة (sad + sadc + أدوات)" ;;
    esac
    printf "\n"
}

# ──────────────────────────────────────────────────────────────────────
# تحديد النظام والبنية / Detect OS & Architecture
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
        x86_64|amd64)   ARCH="x86_64" ;;
        aarch64|arm64)  ARCH="aarch64" ;;
        armv7l)         ARCH="armv7" ;;
        *) die "بنية غير مدعومة: $ARCH" ;;
    esac

    ok "النظام: $PLATFORM ($ARCH)"
}

# ──────────────────────────────────────────────────────────────────────
# التحقق من التبعيات / Check Dependencies
# ──────────────────────────────────────────────────────────────────────
check_deps() {
    step "التحقق من الأدوات المطلوبة..."

    if command -v curl >/dev/null 2>&1; then
        DOWNLOADER="curl"; ok "curl موجود"
    elif command -v wget >/dev/null 2>&1; then
        DOWNLOADER="wget"; ok "wget موجود"
    else
        die "يجب تثبيت curl أو wget"
    fi

    command -v tar >/dev/null 2>&1 || die "يجب تثبيت tar"

    # تحذير LLVM
    if [ "$COMPONENTS" = "compiler" ] || [ "$COMPONENTS" = "full" ]; then
        step "التحقق من وجود LLVM..."
        if command -v llvm-config >/dev/null 2>&1; then
            LLVM_VER=$(llvm-config --version 2>/dev/null || echo "unknown")
            ok "LLVM موجود: $LLVM_VER"
        else
            warn "LLVM غير مثبت — sadc يحتاج LLVM 18"
            info "Ubuntu/Debian: sudo apt install llvm-18-dev"
            info "macOS: brew install llvm@18"
            info "المفسر sad يعمل بدون LLVM"
        fi
    fi
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
# جلب الإصدار / Get Version Info
# ──────────────────────────────────────────────────────────────────────
get_release_info() {
    step "البحث عن الإصدار من GitHub Releases..."

    if [ "$VERSION" = "latest" ]; then
        RELEASE_URL="$GITHUB_API/releases/latest"
    else
        RELEASE_URL="$GITHUB_API/releases/tags/v${VERSION}"
    fi

    RELEASE_JSON=$(fetch_json "$RELEASE_URL") || die "فشل في جلب معلومات الإصدار"

    ACTUAL_VERSION=$(printf '%s' "$RELEASE_JSON" | grep -o '"tag_name"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | sed 's/.*"v\{0,1\}\([^"]*\)".*/\1/')
    [ -z "$ACTUAL_VERSION" ] && die "لم يُعثر على إصدار"

    ok "الإصدار: v${ACTUAL_VERSION}"
}

# ──────────────────────────────────────────────────────────────────────
# تحميل وتثبيت / Download & Install
# ──────────────────────────────────────────────────────────────────────
install_sad() {
    # بادئة اسم الملف حسب المكون
    case "$COMPONENTS" in
        interpreter) PREFIX="sad" ;;
        compiler)    PREFIX="sadc" ;;
        full)        PREFIX="sad-full" ;;
    esac

    ARCHIVE_NAME="${PREFIX}-v${ACTUAL_VERSION}-${PLATFORM}-${ARCH}.tar.gz"

    # البحث عن رابط التحميل
    DOWNLOAD_URL=$(printf '%s' "$RELEASE_JSON" | grep -o '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]*'"${ARCHIVE_NAME}"'"' | head -1 | sed 's/.*"\(https[^"]*\)".*/\1/')

    if [ -z "$DOWNLOAD_URL" ]; then
        # محاولة أسماء بديلة
        for alt in \
            "${PREFIX}-v${ACTUAL_VERSION}-${PLATFORM}-${ARCH}.tar.gz" \
            "${PREFIX}-${ACTUAL_VERSION}-${PLATFORM}-${ARCH}.tar.gz" \
            "${PREFIX}-${PLATFORM}-${ARCH}.tar.gz"; do
            DOWNLOAD_URL=$(printf '%s' "$RELEASE_JSON" | grep -o '"browser_download_url"[[:space:]]*:[[:space:]]*"[^"]*'"${alt}"'"' | head -1 | sed 's/.*"\(https[^"]*\)".*/\1/')
            [ -n "$DOWNLOAD_URL" ] && ARCHIVE_NAME="$alt" && break
        done
    fi

    if [ -z "$DOWNLOAD_URL" ]; then
        err "لم يُعثر على ملف تحميل لـ '$COMPONENTS' على: ${PLATFORM}-${ARCH}"
        info "الملفات المتاحة:"
        printf '%s' "$RELEASE_JSON" | grep -o '"name"[[:space:]]*:[[:space:]]*"[^"]*"' | sed 's/"name"[[:space:]]*:[[:space:]]*"/  - /;s/"$//'
        exit 1
    fi

    TEMP_DIR=$(mktemp -d)
    trap 'rm -rf "$TEMP_DIR"' EXIT

    step "تحميل $ARCHIVE_NAME..."
    info "المصدر: GitHub Releases (github.com/$REPO_OWNER/$REPO_NAME)"
    info "الرابط: $DOWNLOAD_URL"
    download "$DOWNLOAD_URL" "$TEMP_DIR/$ARCHIVE_NAME"

    FILE_SIZE=$(du -h "$TEMP_DIR/$ARCHIVE_NAME" 2>/dev/null | cut -f1)
    ok "تم التحميل (${FILE_SIZE})"

    step "فك الضغط والتثبيت..."
    [ -d "$INSTALL_DIR" ] && rm -rf "$INSTALL_DIR"
    mkdir -p "$INSTALL_DIR/bin"

    tar -xzf "$TEMP_DIR/$ARCHIVE_NAME" -C "$INSTALL_DIR"

    BIN_DIR="$INSTALL_DIR/bin"

    # نقل الملفات التنفيذية لـ bin إن لم تكن هناك
    for tool in sad sadc sad-lsp sad-lsp-server sad-pkg sad-repl sad-fmt; do
        TOOL_BIN=$(find "$INSTALL_DIR" -name "$tool" -type f ! -path "*/bin/*" 2>/dev/null | head -1)
        if [ -n "$TOOL_BIN" ]; then
            cp "$TOOL_BIN" "$BIN_DIR/"
            chmod +x "$BIN_DIR/$tool"
        fi
    done

    # تعيين الصلاحيات
    chmod +x "$BIN_DIR"/* 2>/dev/null

    ok "تم تثبيت الملفات في: $INSTALL_DIR"

    # عرض المكونات المثبتة
    step "المكونات المثبتة:"
    for tool in "$BIN_DIR"/*; do
        [ ! -x "$tool" ] && continue
        name=$(basename "$tool")
        case "$name" in
            sad)            desc="المفسر — يشغل ملفات .ص مباشرة" ;;
            sadc)           desc="المترجم — يحوّل .ص إلى ملف تنفيذي أصلي (LLVM)" ;;
            sad-lsp*)       desc="خادم LSP — تكامل مع المحررات" ;;
            sad-pkg)        desc="مدير الحزم — تثبيت المكتبات" ;;
            sad-repl)       desc="بيئة تفاعلية" ;;
            sad-fmt)        desc="أداة تنسيق الكود" ;;
            *)              desc="$name" ;;
        esac
        info "  $name — $desc"
    done

    # إضافة PATH
    if [ "$NO_PATH" -eq 0 ]; then
        step "إضافة لغة ص لمتغير PATH..."
        PATH_LINE="export PATH=\"$BIN_DIR:\$PATH\""
        COMMENT="# لغة ص — Sad Programming Language"

        for rc in "$HOME/.bashrc" "$HOME/.zshrc" "$HOME/.profile"; do
            if [ -f "$rc" ] && ! grep -q "sad-lang" "$rc" 2>/dev/null; then
                printf '\n%s\n%s\n' "$COMMENT" "$PATH_LINE" >> "$rc"
            fi
        done

        # fish
        if [ -d "$HOME/.config/fish" ]; then
            FISH_CFG="$HOME/.config/fish/config.fish"
            if [ ! -f "$FISH_CFG" ] || ! grep -q "sad-lang" "$FISH_CFG" 2>/dev/null; then
                mkdir -p "$HOME/.config/fish"
                printf '\n%s\nset -gx PATH "%s" $PATH\n' "$COMMENT" "$BIN_DIR" >> "$FISH_CFG"
            fi
        fi

        export PATH="$BIN_DIR:$PATH"
        ok "تمت إضافة $BIN_DIR لمتغير PATH"
    fi

    # كتابة معلومات التثبيت
    cat > "$INSTALL_DIR/install-info.json" <<EOF
{
    "version": "${ACTUAL_VERSION}",
    "components": "${COMPONENTS}",
    "platform": "${PLATFORM}",
    "arch": "${ARCH}",
    "installDir": "${INSTALL_DIR}",
    "binDir": "${BIN_DIR}",
    "installedAt": "$(date '+%Y-%m-%d %H:%M:%S')",
    "installedBy": "install.sh",
    "source": "GitHub Releases (github.com/${REPO_OWNER}/${REPO_NAME})"
}
EOF

    # التحقق
    step "التحقق من التثبيت..."
    for exe in sad sadc; do
        if [ -x "$BIN_DIR/$exe" ]; then
            VER=$("$BIN_DIR/$exe" --version 2>&1 || true)
            [ -n "$VER" ] && ok "$exe: $VER" || ok "$exe: موجود ✓"
        fi
    done
}

# ──────────────────────────────────────────────────────────────────────
# إزالة التثبيت / Uninstall
# ──────────────────────────────────────────────────────────────────────
uninstall_sad() {
    logo
    printf "  ${YELLOW}إزالة لغة ص...${RESET}\n\n"

    [ ! -d "$INSTALL_DIR" ] && { warn "لغة ص غير مثبتة في: $INSTALL_DIR"; exit 0; }

    step "إزالة من ملفات PATH..."
    for rc in "$HOME/.bashrc" "$HOME/.zshrc" "$HOME/.profile"; do
        [ -f "$rc" ] && { sed -i.bak '/sad-lang/d' "$rc" 2>/dev/null || sed -i '' '/sad-lang/d' "$rc" 2>/dev/null || true; rm -f "${rc}.bak"; }
    done
    FISH_CFG="$HOME/.config/fish/config.fish"
    [ -f "$FISH_CFG" ] && { sed -i.bak '/sad-lang/d' "$FISH_CFG" 2>/dev/null || sed -i '' '/sad-lang/d' "$FISH_CFG" 2>/dev/null || true; rm -f "${FISH_CFG}.bak"; }
    ok "تمت الإزالة من PATH"

    step "حذف الملفات..."
    rm -rf "$INSTALL_DIR"
    ok "تم حذف $INSTALL_DIR"

    printf "\n  ${GREEN}✓ تمت إزالة لغة ص بنجاح${RESET}\n"
    printf "    ${DIM}أعد فتح الطرفية لتحديث PATH${RESET}\n\n"
}

# ══════════════════════════════════════════════════════════════════════
# نقطة الدخول / Entry Point
# ══════════════════════════════════════════════════════════════════════
logo

if [ "$UNINSTALL" -eq 1 ]; then uninstall_sad; exit 0; fi

printf "  مرحباً! سيتم تثبيت لغة ص على جهازك.\n"
printf "  ${DIM}المصدر: GitHub Releases (github.com/$REPO_OWNER/$REPO_NAME)${RESET}\n"
printf "  ${DIM}المجلد: $INSTALL_DIR${RESET}\n\n"

show_menu
detect_platform
check_deps
get_release_info
install_sad

# رسالة الانتهاء
case "$COMPONENTS" in
    interpreter) COMP_NAME="المفسر (sad)" ;;
    compiler)    COMP_NAME="المترجم (sadc)" ;;
    full)        COMP_NAME="الحزمة الكاملة" ;;
esac

printf "\n"
printf "  ${GREEN}═══════════════════════════════════════════════${RESET}\n"
printf "  ${GREEN}✓ تم تثبيت %s v%s بنجاح!${RESET}\n" "$COMP_NAME" "$ACTUAL_VERSION"
printf "  ${GREEN}═══════════════════════════════════════════════${RESET}\n\n"
printf "  للبدء:\n"

case "$COMPONENTS" in
    interpreter|full)
        printf "    ${DIM}sad --help              عرض المساعدة${RESET}\n"
        printf "    ${DIM}sad script.ص           تشغيل ملف${RESET}\n"
        ;;&
    compiler|full)
        printf "    ${DIM}sadc script.ص          ترجمة إلى ملف تنفيذي${RESET}\n"
        ;;&
    full)
        printf "    ${DIM}sad-pkg init            إنشاء مشروع جديد${RESET}\n"
        printf "    ${DIM}sad-repl                بيئة تفاعلية${RESET}\n"
        ;;
esac

CURRENT_SHELL=$(basename "$SHELL" 2>/dev/null || echo "bash")
printf "\n  ${YELLOW}⚡ فعّل التغييرات:${RESET}\n"
printf "    ${DIM}source ~/.${CURRENT_SHELL}rc${RESET}\n"
printf "    ${DIM}أو أعد فتح الطرفية${RESET}\n\n"
