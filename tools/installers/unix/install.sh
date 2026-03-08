#!/bin/bash
# سكريبت تثبيت لغة ص لـ Linux/macOS
# Sad Programming Language Installer

set -e

VERSION="1.0.0"
INSTALL_DIR="${SAD_INSTALL_DIR:-/usr/local}"
BIN_DIR="$INSTALL_DIR/bin"
LIB_DIR="$INSTALL_DIR/lib/sad"
SHARE_DIR="$INSTALL_DIR/share/sad"

# الألوان
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_banner() {
    echo -e "${BLUE}"
    echo "╔══════════════════════════════════════════════════════╗"
    echo "║          لغة ص - Sad Programming Language            ║"
    echo "║                   الإصدار $VERSION                      ║"
    echo "╚══════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

detect_os() {
    case "$(uname -s)" in
        Linux*)     OS=linux;;
        Darwin*)    OS=macos;;
        MINGW*|MSYS*|CYGWIN*) OS=windows;;
        *)          OS=unknown;;
    esac
    
    case "$(uname -m)" in
        x86_64|amd64)   ARCH=x64;;
        arm64|aarch64)  ARCH=arm64;;
        *)              ARCH=unknown;;
    esac
    
    echo "النظام المكتشف: $OS-$ARCH"
}

check_dependencies() {
    echo -e "${YELLOW}التحقق من المتطلبات...${NC}"
    
    local missing=()
    
    # curl or wget
    if ! command -v curl &> /dev/null && ! command -v wget &> /dev/null; then
        missing+=("curl أو wget")
    fi
    
    # tar
    if ! command -v tar &> /dev/null; then
        missing+=("tar")
    fi
    
    if [ ${#missing[@]} -ne 0 ]; then
        echo -e "${RED}المتطلبات المفقودة: ${missing[*]}${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ جميع المتطلبات متوفرة${NC}"
}

download_file() {
    local url=$1
    local output=$2
    
    if command -v curl &> /dev/null; then
        curl -fsSL "$url" -o "$output"
    else
        wget -q "$url" -O "$output"
    fi
}

install_sad() {
    local tmp_dir=$(mktemp -d)
    local archive="sad-$VERSION-$OS-$ARCH.tar.gz"
    local url="https://github.com/sad-lang/sad/releases/download/v$VERSION/$archive"
    
    echo -e "${YELLOW}تحميل لغة ص $VERSION...${NC}"
    download_file "$url" "$tmp_dir/$archive"
    
    echo -e "${YELLOW}فك الضغط...${NC}"
    tar -xzf "$tmp_dir/$archive" -C "$tmp_dir"
    
    echo -e "${YELLOW}التثبيت في $INSTALL_DIR...${NC}"
    
    # قد يحتاج صلاحيات root
    if [ -w "$BIN_DIR" ]; then
        install_files "$tmp_dir/sad-$VERSION"
    else
        echo "يتطلب صلاحيات المدير..."
        sudo_install_files "$tmp_dir/sad-$VERSION"
    fi
    
    # تنظيف
    rm -rf "$tmp_dir"
    
    echo -e "${GREEN}✓ تم التثبيت بنجاح!${NC}"
}

install_files() {
    local src=$1
    
    mkdir -p "$BIN_DIR" "$LIB_DIR" "$SHARE_DIR"
    
    # الملفات التنفيذية
    install -m 755 "$src/bin/sad" "$BIN_DIR/"
    install -m 755 "$src/bin/sadc" "$BIN_DIR/"
    install -m 755 "$src/bin/sad-pkg" "$BIN_DIR/"
    install -m 755 "$src/bin/sad-fmt" "$BIN_DIR/"
    
    # المكتبة القياسية
    cp -r "$src/stdlib/"* "$LIB_DIR/"
    
    # التوثيق والأمثلة
    cp -r "$src/docs" "$SHARE_DIR/"
    cp -r "$src/examples" "$SHARE_DIR/"
}

sudo_install_files() {
    local src=$1
    
    sudo mkdir -p "$BIN_DIR" "$LIB_DIR" "$SHARE_DIR"
    
    sudo install -m 755 "$src/bin/sad" "$BIN_DIR/"
    sudo install -m 755 "$src/bin/sadc" "$BIN_DIR/"
    sudo install -m 755 "$src/bin/sad-pkg" "$BIN_DIR/"
    sudo install -m 755 "$src/bin/sad-fmt" "$BIN_DIR/"
    
    sudo cp -r "$src/stdlib/"* "$LIB_DIR/"
    sudo cp -r "$src/docs" "$SHARE_DIR/"
    sudo cp -r "$src/examples" "$SHARE_DIR/"
}

setup_path() {
    if [[ ":$PATH:" != *":$BIN_DIR:"* ]]; then
        echo -e "${YELLOW}إضافة $BIN_DIR إلى PATH...${NC}"
        
        local shell_rc=""
        case "$SHELL" in
            */bash)
                shell_rc="$HOME/.bashrc"
                ;;
            */zsh)
                shell_rc="$HOME/.zshrc"
                ;;
            */fish)
                shell_rc="$HOME/.config/fish/config.fish"
                ;;
        esac
        
        if [ -n "$shell_rc" ]; then
            echo "" >> "$shell_rc"
            echo "# Sad Programming Language" >> "$shell_rc"
            echo "export PATH=\"\$PATH:$BIN_DIR\"" >> "$shell_rc"
            echo "export SAD_STDLIB=\"$LIB_DIR\"" >> "$shell_rc"
            
            echo -e "${GREEN}✓ تمت إضافة PATH إلى $shell_rc${NC}"
            echo "شغّل الأمر التالي أو أعد فتح الطرفية:"
            echo "  source $shell_rc"
        fi
    fi
}

verify_installation() {
    echo -e "${YELLOW}التحقق من التثبيت...${NC}"
    
    if command -v sad &> /dev/null; then
        echo -e "${GREEN}✓ sad: $(sad --version 2>/dev/null || echo 'مثبت')${NC}"
    else
        echo -e "${YELLOW}⚠ sad غير موجود في PATH${NC}"
    fi
    
    if command -v sadc &> /dev/null; then
        echo -e "${GREEN}✓ sadc: $(sadc --version 2>/dev/null || echo 'مثبت')${NC}"
    fi
}

print_success() {
    echo ""
    echo -e "${GREEN}══════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}     لغة ص مثبتة بنجاح! 🎉${NC}"
    echo -e "${GREEN}══════════════════════════════════════════════════════${NC}"
    echo ""
    echo "للبدء:"
    echo "  sad                    # المفسر التفاعلي"
    echo "  sadc hello.ص -o hello  # الترجمة"
    echo "  sad hello.ص            # تشغيل مباشر"
    echo ""
    echo "الوثائق: https://sad-lang.org/docs"
    echo "Discord: https://discord.gg/sad-lang"
    echo ""
}

uninstall() {
    echo -e "${YELLOW}إلغاء تثبيت لغة ص...${NC}"
    
    local files=("$BIN_DIR/sad" "$BIN_DIR/sadc" "$BIN_DIR/sad-pkg" "$BIN_DIR/sad-fmt")
    
    for f in "${files[@]}"; do
        if [ -f "$f" ]; then
            rm -f "$f" 2>/dev/null || sudo rm -f "$f"
        fi
    done
    
    rm -rf "$LIB_DIR" 2>/dev/null || sudo rm -rf "$LIB_DIR"
    rm -rf "$SHARE_DIR" 2>/dev/null || sudo rm -rf "$SHARE_DIR"
    
    echo -e "${GREEN}✓ تم إلغاء التثبيت${NC}"
}

main() {
    print_banner
    
    case "${1:-install}" in
        install)
            detect_os
            check_dependencies
            install_sad
            setup_path
            verify_installation
            print_success
            ;;
        uninstall)
            uninstall
            ;;
        --version|-v)
            echo "Sad Installer v$VERSION"
            ;;
        --help|-h)
            echo "الاستخدام: $0 [install|uninstall|--help]"
            echo ""
            echo "متغيرات البيئة:"
            echo "  SAD_INSTALL_DIR   مجلد التثبيت (افتراضي: /usr/local)"
            ;;
        *)
            echo "أمر غير معروف: $1"
            echo "استخدم --help للمساعدة"
            exit 1
            ;;
    esac
}

main "$@"
