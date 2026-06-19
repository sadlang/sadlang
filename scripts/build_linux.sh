#!/bin/bash
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
#  سكريبت بناء لغة ص على Linux
#  Sad Language Linux Build Script
# ═══════════════════════════════════════════════════════════════════════════════

set -e

# ألوان للإخراج
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

# ─────────────────────────────────────────────────────────────────────
# فحص المتطلبات
# ─────────────────────────────────────────────────────────────────────
check_requirements() {
    print_header "فحص المتطلبات / Checking Requirements"
    
    # CMake
    if command -v cmake &> /dev/null; then
        CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
        print_success "CMake found: $CMAKE_VERSION"
    else
        print_error "CMake not found. Installing..."
        sudo apt-get update && sudo apt-get install -y cmake
    fi
    
    # GCC/G++
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "G++ found: $GCC_VERSION"
    else
        print_error "G++ not found. Installing..."
        sudo apt-get install -y build-essential
    fi
    
    # SDL2
    if pkg-config --exists sdl2 2>/dev/null; then
        SDL_VERSION=$(pkg-config --modversion sdl2)
        print_success "SDL2 found: $SDL_VERSION"
    else
        print_warning "SDL2 not found. Installing..."
        sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
    fi
    
    # OpenGL
    if pkg-config --exists gl 2>/dev/null; then
        print_success "OpenGL found"
    else
        print_warning "OpenGL not found. Installing..."
        sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
    fi
    
    # LLVM (اختياري للمترجم sadc)
    if command -v llvm-config &> /dev/null; then
        LLVM_VERSION=$(llvm-config --version)
        print_success "LLVM found: $LLVM_VERSION"
        HAVE_LLVM=true
    else
        print_warning "LLVM not found. sadc compiler will not be built."
        print_warning "To install LLVM: sudo apt-get install llvm-18-dev"
        HAVE_LLVM=false
    fi
    
    echo ""
}

# ─────────────────────────────────────────────────────────────────────
# تثبيت المكتبات الاختيارية
# ─────────────────────────────────────────────────────────────────────
install_optional() {
    print_header "تثبيت المكتبات الاختيارية / Installing Optional Libraries"
    
    echo "هل تريد تثبيت LLVM 18 لدعم المترجم؟ (y/n)"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        # إضافة مستودع LLVM
        wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
        
        # تحديد إصدار Ubuntu
        UBUNTU_VERSION=$(lsb_release -cs)
        echo "deb http://apt.llvm.org/$UBUNTU_VERSION/ llvm-toolchain-$UBUNTU_VERSION-18 main" | sudo tee /etc/apt/sources.list.d/llvm.list
        
        sudo apt-get update
        sudo apt-get install -y llvm-18 llvm-18-dev clang-18 lld-18
        
        print_success "LLVM 18 installed successfully"
        HAVE_LLVM=true
    fi
    
    echo ""
}

# ─────────────────────────────────────────────────────────────────────
# البناء
# ─────────────────────────────────────────────────────────────────────
build_project() {
    print_header "بناء المشروع / Building Project"
    
    BUILD_DIR="build_linux"
    
    # تنظيف البناء القديم إن وجد
    if [ -d "$BUILD_DIR" ]; then
        echo "حذف مجلد البناء القديم..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # تكوين CMake
    CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release"
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DBUILD_TESTS=OFF"
    
    if [ "$HAVE_LLVM" = true ]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DENABLE_LLVM_BACKEND=ON"
        # تحديد مسار LLVM إذا لم يكتشف تلقائياً
        if [ -d "/usr/lib/llvm-18" ]; then
            CMAKE_OPTIONS="$CMAKE_OPTIONS -DLLVM_DIR=/usr/lib/llvm-18/cmake"
        fi
    else
        CMAKE_OPTIONS="$CMAKE_OPTIONS -DENABLE_LLVM_BACKEND=OFF"
    fi
    
    echo "تشغيل CMake..."
    cmake $CMAKE_OPTIONS ..
    
    # البناء
    echo "بناء المفسر (sad)..."
    cmake --build . --target sad-run --parallel $(nproc)
    
    if [ "$HAVE_LLVM" = true ]; then
        echo "بناء المترجم (sadc)..."
        cmake --build . --target sad-build --parallel $(nproc)
    fi
    
    cd ..
    
    print_success "تم البناء بنجاح!"
    echo ""
}

# ─────────────────────────────────────────────────────────────────────
# التثبيت
# ─────────────────────────────────────────────────────────────────────
install_system() {
    print_header "تثبيت في النظام / System Installation"
    
    INSTALL_DIR="/usr/local/bin"
    LIB_DIR="/usr/local/lib/sad"
    
    echo "تثبيت في $INSTALL_DIR..."
    
    sudo cp "build_linux/bin/sad" "$INSTALL_DIR/sad"
    sudo chmod +x "$INSTALL_DIR/sad"
    
    if [ -f "build_linux/bin/sadc" ]; then
        sudo cp "build_linux/bin/sadc" "$INSTALL_DIR/sadc"
        sudo chmod +x "$INSTALL_DIR/sadc"
    fi
    
    # نسخ المكتبة القياسية
    sudo mkdir -p "$LIB_DIR"
    sudo cp -r stdlib/* "$LIB_DIR/"
    
    print_success "تم التثبيت بنجاح!"
    echo ""
    echo "يمكنك الآن تشغيل:"
    echo "  sad ملف.ص        # تفسير"
    if [ -f "build_linux/bin/sadc" ]; then
        echo "  sadc ملف.ص       # ترجمة"
    fi
}

# ─────────────────────────────────────────────────────────────────────
# اختبار
# ─────────────────────────────────────────────────────────────────────
run_test() {
    print_header "تشغيل اختبار / Running Test"
    
    echo "اختبار المفسر..."
    ./build_linux/bin/sad examples/flutter/تطبيق_الملاحظات.ص || true
    
    if [ -f "build_linux/bin/sadc" ]; then
        echo ""
        echo "اختبار المترجم..."
        ./build_linux/bin/sadc examples/flutter/تطبيق_الملاحظات.ص -o notes_app || true
    fi
    
    echo ""
}

# ─────────────────────────────────────────────────────────────────────
# القائمة الرئيسية
# ─────────────────────────────────────────────────────────────────────
main() {
    print_header "🔨 بناء لغة ص على Linux"
    
    echo "اختر الإجراء:"
    echo "  1) فحص المتطلبات فقط"
    echo "  2) تثبيت المتطلبات + البناء"
    echo "  3) بناء فقط (المتطلبات مثبتة)"
    echo "  4) تثبيت في النظام"
    echo "  5) تشغيل اختبار"
    echo "  6) الكل (فحص + بناء + تثبيت + اختبار)"
    echo ""
    read -p "اختيارك (1-6): " choice
    
    case $choice in
        1)
            check_requirements
            ;;
        2)
            check_requirements
            install_optional
            build_project
            ;;
        3)
            HAVE_LLVM=false
            if command -v llvm-config &> /dev/null; then
                HAVE_LLVM=true
            fi
            build_project
            ;;
        4)
            install_system
            ;;
        5)
            run_test
            ;;
        6)
            check_requirements
            install_optional
            build_project
            install_system
            run_test
            ;;
        *)
            echo "اختيار غير صالح"
            exit 1
            ;;
    esac
    
    print_success "انتهى!"
}

# Auto mode (للتكامل المستمر)
if [ "$1" = "--auto" ]; then
    HAVE_LLVM=false
    if command -v llvm-config &> /dev/null; then
        HAVE_LLVM=true
    fi
    check_requirements
    build_project
    exit 0
fi

main
