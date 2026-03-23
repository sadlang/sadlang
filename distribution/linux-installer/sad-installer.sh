#!/bin/bash
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# مثبت لغة ص الرسومي — Linux GUI Installer
# Sad Programming Language — Graphical Linux Installer v1.0.0
# ═══════════════════════════════════════════════════════════════════════════════
#
# الاستخدام:
#   ./sad-installer.sh              # وضع رسومي (zenity/kdialog)
#   ./sad-installer.sh --tui        # وضع طرفية (dialog/whiptail)
#   ./sad-installer.sh --cli        # وضع سطر الأوامر فقط
#
# ═══════════════════════════════════════════════════════════════════════════════

set -e

VERSION="1.0.0"
PACKAGE_NAME="sad-lang"
DEFAULT_INSTALL_DIR="$HOME/.sad-lang"
INSTALL_DIR=""
COMPONENTS="full"
GUI_TOOL=""
FORCE_MODE=""

# ──────────────────────────────────────────────────────────────────────
# الألوان
# ──────────────────────────────────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# ──────────────────────────────────────────────────────────────────────
# كشف أداة الواجهة الرسومية
# ──────────────────────────────────────────────────────────────────────
detect_gui_tool() {
    if [ "$FORCE_MODE" = "cli" ]; then
        GUI_TOOL="cli"
        return
    fi
    if [ "$FORCE_MODE" = "tui" ]; then
        if command -v dialog >/dev/null 2>&1; then
            GUI_TOOL="dialog"
        elif command -v whiptail >/dev/null 2>&1; then
            GUI_TOOL="whiptail"
        else
            GUI_TOOL="cli"
        fi
        return
    fi
    # رسومي أولاً
    if [ -n "$DISPLAY" ] || [ -n "$WAYLAND_DISPLAY" ]; then
        if command -v zenity >/dev/null 2>&1; then
            GUI_TOOL="zenity"
            return
        elif command -v kdialog >/dev/null 2>&1; then
            GUI_TOOL="kdialog"
            return
        fi
    fi
    # طرفية ثانياً
    if command -v dialog >/dev/null 2>&1; then
        GUI_TOOL="dialog"
    elif command -v whiptail >/dev/null 2>&1; then
        GUI_TOOL="whiptail"
    else
        GUI_TOOL="cli"
    fi
}

# ──────────────────────────────────────────────────────────────────────
# تحليل الأوامر
# ──────────────────────────────────────────────────────────────────────
while [ $# -gt 0 ]; do
    case "$1" in
        --tui)    FORCE_MODE="tui" ;;
        --cli)    FORCE_MODE="cli" ;;
        --dir)    INSTALL_DIR="$2"; shift ;;
        --help|-h)
            echo "مثبت لغة ص — Linux Installer v${VERSION}"
            echo ""
            echo "الاستخدام: $0 [خيارات]"
            echo "  --tui       وضع الطرفية النصية"
            echo "  --cli       وضع سطر الأوامر"
            echo "  --dir DIR   مجلد التثبيت"
            echo "  --help      عرض المساعدة"
            exit 0
            ;;
        *) echo "خيار غير معروف: $1"; exit 1 ;;
    esac
    shift
done

detect_gui_tool

# ──────────────────────────────────────────────────────────────────────
# دوال الواجهة الرسومية (zenity)
# ──────────────────────────────────────────────────────────────────────
gui_welcome() {
    case "$GUI_TOOL" in
        zenity)
            zenity --info --title="مثبت لغة ص" --width=500 --height=300 \
                --text="<b>مرحباً بك في مثبت لغة ص v${VERSION}</b>\n\nلغة ص هي لغة برمجة عربية حديثة.\n\nسيتم تثبيت:\n• المفسر (sad)\n• مدير الحزم (sad-pkg)\n• خادم LSP (sad-lsp)\n• REPL التفاعلي (sad-repl)\n• منسّق الكود (sad-fmt)\n• المكتبة القياسية\n\nالناشر: Saleh Kadah" 2>/dev/null
            ;;
        kdialog)
            kdialog --msgbox "مرحباً بك في مثبت لغة ص v${VERSION}\n\nسيتم تثبيت المفسر والأدوات والمكتبة القياسية." --title "مثبت لغة ص" 2>/dev/null
            ;;
        dialog)
            dialog --title "مثبت لغة ص" --msgbox "\n  مرحباً بك في مثبت لغة ص v${VERSION}\n\n  لغة ص هي لغة برمجة عربية حديثة.\n\n  سيتم تثبيت:\n  • المفسر (sad)\n  • مدير الحزم (sad-pkg)\n  • خادم LSP (sad-lsp)\n  • REPL (sad-repl)\n  • منسّق الكود (sad-fmt)\n  • المكتبة القياسية\n\n  الناشر: Saleh Kadah" 18 55
            ;;
        whiptail)
            whiptail --title "مثبت لغة ص" --msgbox "\n  مرحباً بك في مثبت لغة ص v${VERSION}\n\n  سيتم تثبيت:\n  • المفسر والأدوات\n  • المكتبة القياسية\n\n  الناشر: Saleh Kadah" 16 50
            ;;
        cli)
            echo -e "${BOLD}${CYAN}"
            echo "╔═══════════════════════════════════════════════╗"
            echo "║     مثبت لغة ص v${VERSION}                        ║"
            echo "╠═══════════════════════════════════════════════╣"
            echo "║  لغة برمجة عربية حديثة                       ║"
            echo "║  الناشر: Saleh Kadah                         ║"
            echo "╚═══════════════════════════════════════════════╝"
            echo -e "${NC}"
            echo -e "سيتم تثبيت: المفسر، مدير الحزم، LSP، REPL، المنسّق"
            echo ""
            ;;
    esac
}

gui_select_dir() {
    case "$GUI_TOOL" in
        zenity)
            INSTALL_DIR=$(zenity --file-selection --directory \
                --title="اختر مجلد التثبيت" \
                --filename="$DEFAULT_INSTALL_DIR/" 2>/dev/null) || INSTALL_DIR="$DEFAULT_INSTALL_DIR"
            ;;
        kdialog)
            INSTALL_DIR=$(kdialog --getexistingdirectory "$DEFAULT_INSTALL_DIR" 2>/dev/null) || INSTALL_DIR="$DEFAULT_INSTALL_DIR"
            ;;
        dialog)
            INSTALL_DIR=$(dialog --title "مجلد التثبيت" --inputbox "\nأدخل مجلد التثبيت:" 10 60 "$DEFAULT_INSTALL_DIR" 3>&1 1>&2 2>&3) || INSTALL_DIR="$DEFAULT_INSTALL_DIR"
            ;;
        whiptail)
            INSTALL_DIR=$(whiptail --title "مجلد التثبيت" --inputbox "\nأدخل مجلد التثبيت:" 10 60 "$DEFAULT_INSTALL_DIR" 3>&1 1>&2 2>&3) || INSTALL_DIR="$DEFAULT_INSTALL_DIR"
            ;;
        cli)
            echo -en "${YELLOW}مجلد التثبيت [${DEFAULT_INSTALL_DIR}]: ${NC}"
            read -r user_dir
            INSTALL_DIR="${user_dir:-$DEFAULT_INSTALL_DIR}"
            ;;
    esac
}

gui_confirm() {
    local msg="سيتم تثبيت لغة ص في:\n${INSTALL_DIR}\n\nالمساحة المطلوبة: ~15 MB\n\nهل تريد المتابعة؟"
    case "$GUI_TOOL" in
        zenity)
            zenity --question --title="تأكيد التثبيت" --width=400 \
                --text="$msg" 2>/dev/null
            return $?
            ;;
        kdialog)
            kdialog --yesno "$msg" --title "تأكيد التثبيت" 2>/dev/null
            return $?
            ;;
        dialog)
            dialog --title "تأكيد التثبيت" --yesno "\n$msg" 12 50
            return $?
            ;;
        whiptail)
            whiptail --title "تأكيد التثبيت" --yesno "\n$msg" 12 50
            return $?
            ;;
        cli)
            echo -en "${YELLOW}تأكيد التثبيت في ${INSTALL_DIR}؟ [ن/ل] (نعم): ${NC}"
            read -r confirm
            [ "$confirm" != "ل" ] && [ "$confirm" != "لا" ] && [ "$confirm" != "n" ] && [ "$confirm" != "N" ]
            return $?
            ;;
    esac
}

gui_progress() {
    local pct="$1"
    local msg="$2"
    case "$GUI_TOOL" in
        zenity)
            echo "$pct"
            echo "# $msg"
            ;;
        cli)
            local bar_len=40
            local filled=$((pct * bar_len / 100))
            local empty=$((bar_len - filled))
            printf "\r  [${GREEN}"
            printf '%0.s█' $(seq 1 $filled 2>/dev/null) 2>/dev/null || true
            printf "${NC}"
            printf '%0.s░' $(seq 1 $empty 2>/dev/null) 2>/dev/null || true
            printf "] %3d%% %s" "$pct" "$msg"
            ;;
    esac
}

gui_done() {
    local msg="تم تثبيت لغة ص v${VERSION} بنجاح!\n\nالمسار: ${INSTALL_DIR}\n\nللبدء:\n  sad --help\n  sad script.ص\n\nقد تحتاج لإعادة فتح الطرفية لتفعيل PATH."
    case "$GUI_TOOL" in
        zenity)
            zenity --info --title="تم التثبيت بنجاح!" --width=450 \
                --text="<b>✅ $msg</b>" 2>/dev/null
            ;;
        kdialog)
            kdialog --msgbox "✅ $msg" --title "تم التثبيت بنجاح!" 2>/dev/null
            ;;
        dialog)
            dialog --title "تم التثبيت بنجاح!" --msgbox "\n✅ $msg" 14 55
            ;;
        whiptail)
            whiptail --title "تم التثبيت بنجاح!" --msgbox "\n✅ $msg" 14 55
            ;;
        cli)
            echo ""
            echo -e "${GREEN}${BOLD}"
            echo "╔═══════════════════════════════════════════════╗"
            echo "║  ✅ تم التثبيت بنجاح!                        ║"
            echo "╠═══════════════════════════════════════════════╣"
            echo "║  المسار: ${INSTALL_DIR}"
            echo "║  الأوامر: sad --help                          ║"
            echo "║           sad script.ص                        ║"
            echo "╚═══════════════════════════════════════════════╝"
            echo -e "${NC}"
            ;;
    esac
}

gui_error() {
    local msg="$1"
    case "$GUI_TOOL" in
        zenity)  zenity --error --title="خطأ" --text="$msg" 2>/dev/null ;;
        kdialog) kdialog --error "$msg" 2>/dev/null ;;
        dialog)  dialog --title "خطأ" --msgbox "\n$msg" 8 50 ;;
        whiptail) whiptail --title "خطأ" --msgbox "\n$msg" 8 50 ;;
        cli)     echo -e "${RED}خطأ: $msg${NC}" ;;
    esac
}

# ──────────────────────────────────────────────────────────────────────
# استخراج البيانات المُضمنة
# ──────────────────────────────────────────────────────────────────────
ARCHIVE_LINE=$(awk '/^__ARCHIVE_BELOW__$/{print NR + 1; exit 0;}' "$0")

extract_archive() {
    local dest="$1"
    tail -n +"$ARCHIVE_LINE" "$0" | tar xzf - -C "$dest" 2>/dev/null
}

# ──────────────────────────────────────────────────────────────────────
# عملية التثبيت
# ──────────────────────────────────────────────────────────────────────
do_install() {
    mkdir -p "$INSTALL_DIR"
    mkdir -p "$INSTALL_DIR/bin"
    mkdir -p "$INSTALL_DIR/lib/sad-lang"
    
    if [ "$GUI_TOOL" = "zenity" ]; then
        (
            gui_progress 10 "استخراج الملفات..."
            extract_archive "/tmp/sad-install-$$"
            
            gui_progress 30 "نسخ الملفات التنفيذية..."
            local src="/tmp/sad-install-$$/sad-1.0.0-linux-x64"
            cp "$src/bin/"* "$INSTALL_DIR/bin/" 2>/dev/null || true
            
            gui_progress 50 "نسخ المكتبة القياسية..."
            cp -r "$src/lib/sad-lang/stdlib" "$INSTALL_DIR/lib/sad-lang/" 2>/dev/null || true
            
            gui_progress 70 "إعداد المسارات..."
            setup_path
            
            gui_progress 90 "إنشاء الروابط..."
            create_desktop_entry
            
            gui_progress 100 "اكتمل التثبيت!"
            sleep 1
        ) | zenity --progress --title="تثبيت لغة ص" --width=400 \
            --auto-close --no-cancel 2>/dev/null
    else
        echo -e "\n${CYAN}  بدء التثبيت...${NC}\n"
        
        gui_progress 10 "استخراج الملفات..."
        mkdir -p "/tmp/sad-install-$$"
        extract_archive "/tmp/sad-install-$$"
        echo ""
        
        gui_progress 30 "نسخ الملفات التنفيذية..."
        local src="/tmp/sad-install-$$/sad-1.0.0-linux-x64"
        cp "$src/bin/"* "$INSTALL_DIR/bin/" 2>/dev/null || true
        echo ""
        
        gui_progress 50 "نسخ المكتبة القياسية..."
        cp -r "$src/lib/sad-lang/stdlib" "$INSTALL_DIR/lib/sad-lang/" 2>/dev/null || true
        echo ""
        
        gui_progress 70 "إعداد المسارات..."
        setup_path
        echo ""
        
        gui_progress 90 "إنشاء الروابط..."
        create_desktop_entry
        echo ""
        
        gui_progress 100 "اكتمل التثبيت!"
        echo ""
    fi
    
    # تنظيف
    rm -rf "/tmp/sad-install-$$"
}

# ──────────────────────────────────────────────────────────────────────
# إعداد PATH
# ──────────────────────────────────────────────────────────────────────
setup_path() {
    local bin_dir="$INSTALL_DIR/bin"
    local path_line="export PATH=\"$bin_dir:\$PATH\""
    local sad_home="export SAD_HOME=\"$INSTALL_DIR\""
    
    for rc_file in "$HOME/.bashrc" "$HOME/.zshrc" "$HOME/.profile"; do
        if [ -f "$rc_file" ]; then
            if ! grep -q "SAD_HOME" "$rc_file" 2>/dev/null; then
                echo "" >> "$rc_file"
                echo "# لغة ص / Sad Programming Language" >> "$rc_file"
                echo "$sad_home" >> "$rc_file"
                echo "$path_line" >> "$rc_file"
            fi
        fi
    done
    
    # تصدير للجلسة الحالية
    export PATH="$bin_dir:$PATH"
    export SAD_HOME="$INSTALL_DIR"
}

# ──────────────────────────────────────────────────────────────────────
# إنشاء اختصار سطح المكتب
# ──────────────────────────────────────────────────────────────────────
create_desktop_entry() {
    local desktop_dir="${XDG_DATA_HOME:-$HOME/.local/share}/applications"
    mkdir -p "$desktop_dir"
    
    cat > "$desktop_dir/sad-lang.desktop" <<EOF
[Desktop Entry]
Name=Sad Language REPL
Name[ar]=REPL لغة ص
Comment=Arabic Programming Language Interactive Console
Comment[ar]=الطرفية التفاعلية للغة ص
Exec=$INSTALL_DIR/bin/sad-repl
Icon=utilities-terminal
Terminal=true
Type=Application
Categories=Development;IDE;
Keywords=sad;programming;arabic;برمجة;عربي;
EOF
}

# ──────────────────────────────────────────────────────────────────────
# التنفيذ الرئيسي
# ──────────────────────────────────────────────────────────────────────
main() {
    # التحقق من وجود البيانات المُضمنة
    if [ -z "$ARCHIVE_LINE" ]; then
        echo -e "${RED}خطأ: هذا الملف ليس مثبتاً صالحاً — لا توجد بيانات مُضمنة.${NC}"
        exit 1
    fi
    
    gui_welcome
    
    if [ -z "$INSTALL_DIR" ]; then
        gui_select_dir
    fi
    
    if ! gui_confirm; then
        echo "تم إلغاء التثبيت."
        exit 0
    fi
    
    do_install
    gui_done
}

main
exit 0

# ═══════════════════════════════════════════════════════════════════════════════
# البيانات المضغوطة تُضاف أسفل هذا السطر بواسطة سكريبت البناء
# ═══════════════════════════════════════════════════════════════════════════════
__ARCHIVE_BELOW__
