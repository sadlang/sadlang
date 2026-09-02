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
#   ./install.sh --components full               # المفسر + المترجم + كل الأدوات
#   ./install.sh --components compiler           # المترجم فقط (يتطلب LLVM)
#   ./install.sh --components interpreter        # المفسر فقط
#   ./install.sh --version 1.2.0                 # إصدار محدد
#   ./install.sh --dir /opt/sad-lang             # مجلد مخصص
#   ./install.sh --no-path                       # لا يضيف لمتغير PATH
#   ./install.sh --no-gui                        # إجبار وضع الطرفية النصية
#   ./install.sh --llvm-path /usr/lib/llvm-18    # مسار LLVM مباشرة
#   ./install.sh --uninstall                     # إزالة التثبيت
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
FORCE_TUI=0
LLVM_PATH=""
LLVM_SKIP=0
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
        --llvm-path)    LLVM_PATH="$2";  shift 2 ;;
        --help|-h)
            printf "Usage: install.sh [OPTIONS]\n"
            printf "  --components X   interpreter|compiler|full\n"
            printf "  --version VER    تثبيت إصدار محدد\n"
            printf "  --dir DIR        مجلد التثبيت (افتراضي: ~/.sad-lang)\n"
            printf "  --no-path        لا يضيف لـ PATH\n"
            printf "  --no-gui         وضع الطرفية النصية فقط\n"
            printf "  --llvm-path DIR  مسار مجلد LLVM\n"
            printf "  --uninstall      إزالة التثبيت\n"
            exit 0 ;;
        *) die "خيار غير معروف: $1" ;;
    esac
done

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
            "interpreter" "المفسر (sad) + المكتبة القياسية ← موصى به للمبتدئين" \
            "compiler"    "المترجم (sadc) — يحوّل .ص إلى ملف تنفيذي (يتطلب LLVM 14+)" \
            "full"        "الحزمة الكاملة — المفسر + المترجم + LSP + REPL + مدير الحزم" \
        ) 2>/dev/null || choice="interpreter"
        case "$choice" in
            compiler) COMPONENTS="compiler" ;;
            full)     COMPONENTS="full"     ;;
            *)        COMPONENTS="interpreter" ;;
        esac
    else
        printf "  %s╔═══════════════════════════════════════════════════════╗%s\n" "$CYAN" "$RESET"
        printf "  %s║  اختر ما تريد تثبيته / Choose what to install:     ║%s\n" "$CYAN" "$RESET"
        printf "  %s╠═══════════════════════════════════════════════════════╣%s\n" "$CYAN" "$RESET"
        printf "  ║  %s[1]%s المفسر فقط (interpreter)                    ║\n" "$GREEN" "$RESET"
        printf "  ║  %s    sad + المكتبة القياسية ← الأفضل للمبتدئين%s     ║\n" "$DIM" "$RESET"
        printf "  ║  %s[2]%s المترجم فقط (compiler) — يتطلب LLVM 14+     ║\n" "$YELLOW" "$RESET"
        printf "  ║  %s    sadc يحوّل .ص إلى ملف تنفيذي أصلي%s            ║\n" "$DIM" "$RESET"
        printf "  ║  %s[3]%s الحزمة الكاملة (full)                        ║\n" "$CYAN" "$RESET"
        printf "  ║  %s    المفسر + المترجم + LSP + REPL + مدير الحزم%s    ║\n" "$DIM" "$RESET"
        printf "  %s╚═══════════════════════════════════════════════════════╝%s\n\n" "$CYAN" "$RESET"
        printf "  اختر رقم (1/2/3) [الافتراضي: 1]: "
        read -r choice
        case "$choice" in
            2) COMPONENTS="compiler"    ;;
            3) COMPONENTS="full"        ;;
            *) COMPONENTS="interpreter" ;;
        esac
    fi

    case "$COMPONENTS" in
        interpreter) ok "تم اختيار: المفسر (sad)" ;;
        compiler)    ok "تم اختيار: المترجم (sadc)" ;;
        full)        ok "تم اختيار: الحزمة الكاملة" ;;
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
# فحص وكشف LLVM / LLVM Detection & Validation
# ══════════════════════════════════════════════════════════════════════

# استخراج الإصدار الرئيسي
_llvm_major() { printf '%s' "$1" | grep -oE '^[0-9]+' | head -1; }

# التحقق من مجلد LLVM — يعيد 0=نجاح، 1=فشل
# يطبع النتيجة: "ok|VERSION|BINS_FOUND|HAS_LIBS|PATH"
# أو "fail|REASON"
validate_llvm_dir() {
    local dir="$1"
    [ -d "$dir" ] || { printf "fail|المجلد غير موجود: %s" "$dir"; return 1; }

    local bindir="$dir/bin"
    [ -d "$bindir" ] || bindir="$dir"

    local found_bins="" bin_count=0
    for b in clang llc opt llvm-ar llvm-config llvm-link lld; do
        if [ -x "$bindir/$b" ]; then
            found_bins="$found_bins$b "
            bin_count=$((bin_count + 1))
        fi
    done

    [ "$bin_count" -lt 2 ] && {
        printf "fail|ملفات تنفيذية غير كافية (وُجد %s من 7)" "$bin_count"
        return 1
    }

    # استخراج الإصدار
    local ver="unknown"
    if [ -x "$bindir/llvm-config" ]; then
        ver=$("$bindir/llvm-config" --version 2>/dev/null | head -1 || echo "unknown")
    elif [ -x "$bindir/clang" ]; then
        local raw
        raw=$("$bindir/clang" --version 2>/dev/null | head -1 || echo "")
        ver=$(printf '%s' "$raw" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1 || echo "unknown")
    fi

    # تحقق من الإصدار ≥ 14
    local major
    major=$(_llvm_major "$ver")
    if [ -n "$major" ] && [ "$major" -gt 0 ] 2>/dev/null && [ "$major" -lt 14 ] 2>/dev/null; then
        printf "fail|الإصدار %s قديم جداً — يجب LLVM 14+" "$ver"
        return 1
    fi

    local has_libs="لا"
    if find "$dir/lib" \( -name "libLLVM*.a" -o -name "LLVM*.lib" \) 2>/dev/null | grep -q .; then
        has_libs="نعم"
    fi

    printf "ok|%s|%s|%s|%s" "$ver" "$found_bins" "$has_libs" "$dir"
    return 0
}

# البحث التلقائي في مسارات شائعة
find_llvm() {
    step "البحث التلقائي عن LLVM..."

    # 1) llvm-config في PATH
    local cfgtool
    for cfgtool in llvm-config llvm-config-18 llvm-config-17 llvm-config-16 llvm-config-15 llvm-config-14; do
        if command -v "$cfgtool" >/dev/null 2>&1; then
            local prefix
            prefix=$("$cfgtool" --prefix 2>/dev/null || echo "")
            if [ -n "$prefix" ]; then
                local res
                res=$(validate_llvm_dir "$prefix" 2>/dev/null) && {
                    local ver
                    ver=$(printf '%s' "$res" | cut -d'|' -f2)
                    ok "LLVM موجود في PATH ($cfgtool): $prefix  الإصدار: $ver"
                    LLVM_PATH="$prefix"
                    return 0
                }
            fi
        fi
    done

    # 2) مسارات Linux / macOS شائعة
    local p
    for p in \
        /usr/lib/llvm-18 /usr/lib/llvm-17 /usr/lib/llvm-16 \
        /usr/lib/llvm-15 /usr/lib/llvm-14 /usr/lib/llvm \
        /usr/local/lib/llvm /usr/local/lib/llvm-18 \
        /usr/local/opt/llvm@18 /usr/local/opt/llvm \
        /opt/homebrew/opt/llvm@18 /opt/homebrew/opt/llvm \
        /opt/llvm-18 /opt/llvm \
        "$HOME/.local/llvm" "$HOME/llvm"; do
        if [ -d "$p" ]; then
            local res ver
            res=$(validate_llvm_dir "$p" 2>/dev/null) && {
                ver=$(printf '%s' "$res" | cut -d'|' -f2)
                ok "LLVM موجود: $p  الإصدار: $ver"
                LLVM_PATH="$p"
                return 0
            }
        fi
    done

    warn "لم يُعثر على LLVM تلقائياً"
    return 1
}

# ══════════════════════════════════════════════════════════════════════
# تثبيت LLVM تلقائياً / Auto-Install LLVM
# ══════════════════════════════════════════════════════════════════════
install_llvm_auto() {
    printf "\n"
    step "تثبيت LLVM تلقائياً عبر مدير الحزم..."

    _ask_confirm() {
        if [ "$GUI_TOOL" != "none" ]; then
            gui_yesno "تثبيت LLVM" "$1" && return 0 || return 1
        else
            printf "  %s\n  هل تريد المتابعة؟ (y/n) [y]: " "$1"
            read -r _ans
            case "$_ans" in n|N) return 1;; esac
            return 0
        fi
    }

    if [ "$PLATFORM" = "linux" ]; then

        if command -v apt-get >/dev/null 2>&1; then
            _ask_confirm "Ubuntu/Debian — تثبيت LLVM 18 عبر سكريبت apt.llvm.org الرسمي (~200MB)" || { LLVM_SKIP=1; return; }
            step "تحميل سكريبت تثبيت LLVM الرسمي..."
            local tmp_sh
            tmp_sh=$(mktemp /tmp/llvm.XXXXXX.sh)
            if command -v curl >/dev/null 2>&1; then
                curl -fsSL "https://apt.llvm.org/llvm.sh" -o "$tmp_sh" || { warn "فشل التحميل"; rm -f "$tmp_sh"; LLVM_SKIP=1; return; }
            else
                wget -qO "$tmp_sh" "https://apt.llvm.org/llvm.sh" || { warn "فشل التحميل"; rm -f "$tmp_sh"; LLVM_SKIP=1; return; }
            fi
            chmod +x "$tmp_sh"
            if sudo bash "$tmp_sh" 18 all 2>&1; then
                ok "تم تثبيت LLVM 18 بنجاح"; rm -f "$tmp_sh"
                find_llvm && return
            else
                warn "فشل سكريبت LLVM — محاولة apt مباشرة..."
                rm -f "$tmp_sh"
                sudo apt-get install -y llvm-18 clang-18 lld-18 2>/dev/null && {
                    LLVM_PATH="/usr/lib/llvm-18"
                    ok "تم تثبيت LLVM 18 عبر apt"
                    find_llvm && return
                }
            fi

        elif command -v dnf >/dev/null 2>&1; then
            _ask_confirm "Fedora/RHEL — تثبيت LLVM عبر dnf" || { LLVM_SKIP=1; return; }
            sudo dnf install -y llvm-devel clang lld 2>&1 && ok "تم تثبيت LLVM" && find_llvm && return

        elif command -v pacman >/dev/null 2>&1; then
            _ask_confirm "Arch Linux — تثبيت LLVM عبر pacman" || { LLVM_SKIP=1; return; }
            sudo pacman -S --noconfirm llvm clang lld 2>&1 && ok "تم تثبيت LLVM" && find_llvm && return

        elif command -v zypper >/dev/null 2>&1; then
            _ask_confirm "openSUSE — تثبيت LLVM عبر zypper" || { LLVM_SKIP=1; return; }
            sudo zypper install -y llvm-devel clang lld 2>&1 && ok "تم تثبيت LLVM" && find_llvm && return

        else
            warn "لم يُتعرف على مدير الحزم"
        fi

    elif [ "$PLATFORM" = "macos" ]; then
        if command -v brew >/dev/null 2>&1; then
            _ask_confirm "macOS — تثبيت LLVM 18 عبر Homebrew (~500MB)" || { LLVM_SKIP=1; return; }
            brew install llvm@18 && ok "تم تثبيت LLVM" && find_llvm && return
        else
            warn "Homebrew غير مثبت"
            info "ثبّت Homebrew أولاً: https://brew.sh"
        fi
    fi

    warn "تعذّر التثبيت التلقائي"
    LLVM_SKIP=1
}

# حفظ LLVM في متغيرات البيئة
export_llvm_env() {
    local llvm_root="$1"
    local llvm_bin="$llvm_root/bin"

    local rc_file="$HOME/.bashrc"
    [ -f "$HOME/.zshrc" ] && rc_file="$HOME/.zshrc"

    # حذف أي تعريف قديم
    if [ -w "$rc_file" ]; then
        grep -v 'LLVM_DIR\|# sad-llvm' "$rc_file" > "${rc_file}.tmp.sad" 2>/dev/null && mv "${rc_file}.tmp.sad" "$rc_file"
        {
            printf '\n# sad-llvm — مسار LLVM للمترجم sadc (مُضاف تلقائياً)\n'
            printf 'export LLVM_DIR="%s"\n' "$llvm_root"
            printf 'export PATH="%s:$PATH"\n' "$llvm_bin"
        } >> "$rc_file"
        ok "تم حفظ LLVM_DIR در $rc_file"
    fi
    export LLVM_DIR="$llvm_root"
    export PATH="$llvm_bin:$PATH"
}

# ══════════════════════════════════════════════════════════════════════
# معالجة LLVM الرئيسية (GUI أو TUI)
# ══════════════════════════════════════════════════════════════════════
handle_llvm() {
    if [ "$COMPONENTS" != "compiler" ] && [ "$COMPONENTS" != "full" ]; then return; fi

    step "التحقق من LLVM لتشغيل المترجم sadc..."

    # مسار قُدّم عبر --llvm-path
    if [ -n "$LLVM_PATH" ]; then
        local res
        res=$(validate_llvm_dir "$LLVM_PATH" 2>/dev/null) && {
            local ver
            ver=$(printf '%s' "$res" | cut -d'|' -f2)
            ok "LLVM محدد يدوياً: $LLVM_PATH (الإصدار: $ver)"
            export_llvm_env "$LLVM_PATH"
            return 0
        }
        warn "المسار المحدد لا يحتوي LLVM صالحاً — سيتم الكشف التلقائي"
        LLVM_PATH=""
    fi

    # كشف تلقائي
    if find_llvm; then
        local res ver
        res=$(validate_llvm_dir "$LLVM_PATH" 2>/dev/null) && {
            ver=$(printf '%s' "$res" | cut -d'|' -f2)
            ok "تم التحقق: LLVM $ver في $LLVM_PATH"
            export_llvm_env "$LLVM_PATH"
            return 0
        }
    fi

    # LLVM غير موجود — عرض خيارات
    if [ "$GUI_TOOL" != "none" ]; then
        _llvm_gui
    else
        _llvm_tui
    fi
}

_llvm_gui() {
    local choice
    choice=$(gui_list "LLVM مطلوب للمترجم sadc" \
        "لم يُعثر على LLVM على جهازك.\nالمترجم sadc يحتاج LLVM 14+ لتحويل .ص إلى ملفات تنفيذية.\nاختر ما تريد فعله:" \
        "auto"   "تثبيت LLVM تلقائياً عبر مدير الحزم (apt/dnf/pacman/brew)" \
        "manual" "تحديد مسار مجلد LLVM المثبت يدوياً" \
        "skip"   "تخطي — تثبيت sad فقط بدون sadc (يمكن إضافة LLVM لاحقاً)" \
    ) 2>/dev/null || choice="skip"

    case "$choice" in
        auto)
            install_llvm_auto
            if [ "$LLVM_SKIP" = "1" ] || [ -z "$LLVM_PATH" ]; then
                gui_info "تنبيه — LLVM" "تعذّر تثبيت LLVM.\nسيُثبَّت sad بدون sadc.\nيمكنك إضافة LLVM لاحقاً وإعادة تشغيل المثبت."
                [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
            else
                local res ver
                res=$(validate_llvm_dir "$LLVM_PATH" 2>/dev/null) && {
                    ver=$(printf '%s' "$res" | cut -d'|' -f2)
                    gui_info "✓ LLVM جاهز" "تم تثبيت LLVM $ver بنجاح في:\n$LLVM_PATH\n\nساعد المترجم sadc جاهز للاستخدام."
                    export_llvm_env "$LLVM_PATH"
                }
            fi
            ;;
        manual)
            local entered
            entered=$(gui_folder "حدد مجلد LLVM" "/usr/lib")
            if [ -n "$entered" ]; then
                local res
                res=$(validate_llvm_dir "$entered" 2>/dev/null)
                case "$(printf '%s' "$res" | cut -d'|' -f1)" in
                    ok)
                        local ver bins
                        ver=$(printf '%s' "$res" | cut -d'|' -f2)
                        bins=$(printf '%s' "$res" | cut -d'|' -f3)
                        LLVM_PATH="$entered"
                        ok "LLVM صالح: $entered (الإصدار: $ver)"
                        gui_info "✓ LLVM مقبول" "تم التحقق من LLVM بنجاح!\n\nالمسار: $entered\nالإصدار: $ver\nالملفات: $bins"
                        export_llvm_env "$LLVM_PATH"
                        ;;
                    *)
                        local reason
                        reason=$(printf '%s' "$res" | cut -d'|' -f2)
                        gui_info "✗ مسار غير صالح" "لم يُعثر على LLVM صالح في المجلد:\n$entered\n\nالسبب: $reason\n\nسيُثبَّت sad بدون sadc."
                        [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
                        ;;
                esac
            else
                [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
            fi
            ;;
        *)
            gui_info "LLVM متخطى" "سيُثبَّت sad بدون المترجم sadc.\n\nيمكنك لاحقاً:\n  1. تثبيت LLVM (apt install llvm-18)\n  2. إعادة تشغيل: ./install.sh --components compiler"
            [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
            ;;
    esac
}

_llvm_tui() {
    printf "\n"
    printf "  %s╔══════════════════════════════════════════════════════════╗%s\n" "$YELLOW" "$RESET"
    printf "  %s║  LLVM غير موجود — sadc يتطلب LLVM 14+                ║%s\n" "$YELLOW" "$RESET"
    printf "  %s╠══════════════════════════════════════════════════════════╣%s\n" "$YELLOW" "$RESET"
    printf "  ║  %s[1]%s تثبيت LLVM تلقائياً (apt/dnf/pacman/brew)           ║\n" "$GREEN"  "$RESET"
    printf "  ║  %s[2]%s تحديد مسار LLVM يدوياً                              ║\n" "$CYAN"   "$RESET"
    printf "  ║  %s[3]%s تخطي — تثبيت sad بدون sadc                          ║\n" "$DIM"    "$RESET"
    printf "  %s╚══════════════════════════════════════════════════════════╝%s\n\n" "$YELLOW" "$RESET"
    printf "  اختر (1/2/3) [الافتراضي: 1]: "
    read -r ch
    case "$ch" in
        2)
            printf "  أدخل مسار مجلد LLVM (مثال: /usr/lib/llvm-18): "
            read -r entered
            entered=$(printf '%s' "$entered" | tr -d "\"'")
            if [ -n "$entered" ]; then
                local res
                res=$(validate_llvm_dir "$entered" 2>/dev/null)
                case "$(printf '%s' "$res" | cut -d'|' -f1)" in
                    ok)
                        local ver bins has_libs
                        ver=$(printf '%s' "$res" | cut -d'|' -f2)
                        bins=$(printf '%s' "$res" | cut -d'|' -f3)
                        has_libs=$(printf '%s' "$res" | cut -d'|' -f4)
                        LLVM_PATH="$entered"
                        ok "LLVM صالح: $LLVM_PATH"
                        info "الإصدار: $ver | الملفات: $bins | مكتبات: $has_libs"
                        export_llvm_env "$LLVM_PATH"
                        ;;
                    *)
                        local reason
                        reason=$(printf '%s' "$res" | cut -d'|' -f2)
                        warn "المسار غير صالح: $reason"
                        warn "سيُثبَّت sad بدون sadc"
                        [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
                        ;;
                esac
            else
                [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
            fi
            ;;
        3)
            info "تخطي LLVM — سيُثبَّت sad بدون sadc"
            [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
            ;;
        *)
            install_llvm_auto
            if [ "$LLVM_SKIP" = "1" ] || [ -z "$LLVM_PATH" ]; then
                warn "فشل التثبيت التلقائي — سيُثبَّت sad بدون sadc"
                [ "$COMPONENTS" = "compiler" ] && COMPONENTS="interpreter"
            fi
            ;;
    esac
}

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
        interpreter) PREFIX="sad"      ;;
        compiler)    PREFIX="sadc"     ;;
        full)        PREFIX="sad-full" ;;
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
    for tool in sad sad-run sadc sad-build sad-check sad-lsp sad-pkg sad-repl sad-fmt; do
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
        interpreter) REQUIRED_TOOLS="sad sad-run sad-lsp sad-check" ;;
        compiler)    REQUIRED_TOOLS="sadc sad-build" ;;
        full)        REQUIRED_TOOLS="sad sad-run sad-lsp sad-check sadc sad-build" ;;
        *)           REQUIRED_TOOLS="sad" ;;
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
  "llvmPath":    "$LLVM_PATH",
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
            sad)            TDESC="المفسر — يشغّل ملفات .ص مباشرة" ;;
            sadc)           TDESC="المترجم — يحوّل .ص إلى ملف تنفيذي أصلي (LLVM)" ;;
            sad-lsp*)       TDESC="خادم LSP — تكامل VS Code / Vim / Neovim" ;;
            sad-pkg)        TDESC="مدير الحزم — تثبيت مكتبات لغة ص" ;;
            sad-repl)       TDESC="بيئة تفاعلية" ;;
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
handle_llvm
get_release_info
install_sad

# رسالة الانتهاء
case "$COMPONENTS" in
    interpreter) COMP_NAME="المفسر (sad)" ;;
    compiler)    COMP_NAME="المترجم (sadc)" ;;
    full)        COMP_NAME="الحزمة الكاملة" ;;
esac

printf "\n"
printf "  %s═══════════════════════════════════════════════%s\n" "$GREEN" "$RESET"
printf "  %s✓ تم تثبيت %s v%s بنجاح!%s\n" "$GREEN" "$COMP_NAME" "$ACTUAL_VERSION" "$RESET"
printf "  %s═══════════════════════════════════════════════%s\n\n" "$GREEN" "$RESET"
printf "  للبدء:\n"

if [ "$COMPONENTS" = "interpreter" ] || [ "$COMPONENTS" = "full" ]; then
    printf "    %ssad --help%s          عرض المساعدة\n"   "$DIM" "$RESET"
    printf "    %ssad script.ص%s       تشغيل ملف\n"      "$DIM" "$RESET"
fi
if [ "$COMPONENTS" = "compiler" ] || [ "$COMPONENTS" = "full" ]; then
    printf "    %ssadc script.ص%s      ترجمة إلى ملف تنفيذي\n" "$DIM" "$RESET"
fi
if [ "$COMPONENTS" = "full" ]; then
    printf "    %ssad-pkg init%s       إنشاء مشروع جديد\n" "$DIM" "$RESET"
    printf "    %ssad-repl%s           بيئة تفاعلية\n"     "$DIM" "$RESET"
fi

if [ -n "$LLVM_PATH" ]; then
    printf "\n  %s[LLVM]%s LLVM_DIR=%s\n" "$GREEN" "$RESET" "$LLVM_PATH"
fi

CURRENT_SHELL=$(basename "${SHELL:-bash}")
printf "\n  %s⚡ فعّل التغييرات:%s\n" "$YELLOW" "$RESET"
printf "    %ssource ~/.%src%s\n"         "$DIM" "$CURRENT_SHELL" "$RESET"
printf "    %sأو أعد فتح الطرفية%s\n\n"  "$DIM" "$RESET"

if [ "$GUI_TOOL" != "none" ]; then
    gui_info "✓ تم التثبيت بنجاح!" "تم تثبيت لغة ص $COMP_NAME v$ACTUAL_VERSION بنجاح!\n\nالمجلد: $INSTALL_DIR\n\nأعد فتح الطرفية لتفعيل الأوامر."
fi