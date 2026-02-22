# بسم الله الرحمن الرحيم
# Build Script for Sad Language Projects
# نص بناء لمشاريع لغة Sad
# Phase 7.3: Build System Integration

#[=======================================================================[
This is a cross-platform build script for Sad language projects.
It automatically detects the build system and runs the appropriate commands.

هذا نص بناء متعدد المنصات لمشاريع لغة Sad.
يكتشف تلقائياً نظام البناء ويشغل الأوامر المناسبة.

Usage / الاستخدام:
  ./build.sh [options]
  
Options / الخيارات:
  --help, -h        Show help message / عرض المساعدة
  --clean, -c       Clean build / تنظيف البناء
  --debug, -d       Build with debug info / بناء مع معلومات التنقيح
  --release, -r     Build with optimizations / بناء مع تحسينات
  --test, -t        Run tests / تشغيل الاختبارات
  --install, -i     Install after building / تثبيت بعد البناء
  --verbose, -v     Verbose output / إخراج مفصل
  --jobs, -j N      Parallel jobs (default: auto) / مهام متوازية
#]=======================================================================]

#!/bin/bash

# ============================================================================
# الحمد لله - Configuration / الإعدادات
# ============================================================================

# Script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Build configuration
BUILD_TYPE="Release"
BUILD_DIR="${PROJECT_ROOT}/build"
INSTALL_PREFIX="/usr/local"
RUN_TESTS=false
DO_INSTALL=false
DO_CLEAN=false
VERBOSE=false
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# ============================================================================
# Helper Functions / دوال مساعدة
# ============================================================================

print_header() {
    echo -e "${CYAN}${BOLD}"
    echo "═══════════════════════════════════════════════"
    echo "$1"
    echo "═══════════════════════════════════════════════"
    echo -e "${NC}"
}

print_success() {
    echo -e "${GREEN}${BOLD}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}${BOLD}✗ $1${NC}"
}

print_info() {
    echo -e "${BLUE}→ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# ============================================================================
# Parse Arguments / تحليل المعاملات
# ============================================================================

show_help() {
    print_header "الحمد لله - Sad Build Script"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help        Show this help message"
    echo "  -c, --clean       Clean build directory"
    echo "  -d, --debug       Build with debug info (-O0 -g)"
    echo "  -r, --release     Build with optimizations (-O3)"
    echo "  -t, --test        Run tests after building"
    echo "  -i, --install     Install after building"
    echo "  -v, --verbose     Enable verbose output"
    echo "  -j, --jobs N      Number of parallel jobs (default: $JOBS)"
    echo "  --prefix PATH     Installation prefix (default: $INSTALL_PREFIX)"
    echo ""
    echo "Examples:"
    echo "  $0                     # Build with default settings"
    echo "  $0 -r -t              # Release build + tests"
    echo "  $0 -d -v              # Debug build with verbose"
    echo "  $0 -c                 # Clean build"
    echo "  $0 -i --prefix=/opt   # Install to /opt"
    echo ""
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -c|--clean)
            DO_CLEAN=true
            shift
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -t|--test)
            RUN_TESTS=true
            shift
            ;;
        -i|--install)
            DO_INSTALL=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# ============================================================================
# الحمد لله - Clean / التنظيف
# ============================================================================

if [ "$DO_CLEAN" = true ]; then
    print_header "الحمد لله - Cleaning build directory"
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned"
    else
        print_info "Build directory doesn't exist, nothing to clean"
    fi
    
    exit 0
fi

# ============================================================================
# Detect Build System / اكتشاف نظام البناء
# ============================================================================

print_header "الحمد لله - Detecting build system"

USE_CMAKE=false
USE_MAKE=false

if [ -f "${PROJECT_ROOT}/CMakeLists.txt" ]; then
    USE_CMAKE=true
    print_info "Found CMakeLists.txt - using CMake"
elif [ -f "${PROJECT_ROOT}/Makefile" ]; then
    USE_MAKE=true
    print_info "Found Makefile - using Make"
else
    print_error "No build system found (CMakeLists.txt or Makefile)"
    exit 1
fi

# ============================================================================
# الحمد لله - Build with CMake / البناء باستخدام CMake
# ============================================================================

build_cmake() {
    print_header "الحمد لله - Building with CMake"
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure
    print_info "Configuring project..."
    CMAKE_ARGS=(
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
    )
    
    if [ "$VERBOSE" = true ]; then
        CMAKE_ARGS+=(-DCMAKE_VERBOSE_MAKEFILE=ON)
    fi
    
    if ! cmake "${CMAKE_ARGS[@]}" "$PROJECT_ROOT"; then
        print_error "CMake configuration failed"
        exit 1
    fi
    
    print_success "Configuration successful"
    
    # Build
    print_info "Building project..."
    if ! cmake --build . --parallel "$JOBS"; then
        print_error "Build failed"
        exit 1
    fi
    
    print_success "Build successful"
    
    cd "$PROJECT_ROOT"
}

# ============================================================================
# الحمد لله - Build with Make / البناء باستخدام Make
# ============================================================================

build_make() {
    print_header "الحمد لله - Building with Make"
    
    cd "$PROJECT_ROOT"
    
    # Set optimization level
    if [ "$BUILD_TYPE" = "Debug" ]; then
        OPT_LEVEL="O0"
        DEBUG_FLAGS="-g"
    else
        OPT_LEVEL="O3"
        DEBUG_FLAGS=""
    fi
    
    # Build
    MAKE_ARGS=(
        OPT_LEVEL="-$OPT_LEVEL"
        DEBUG_FLAGS="$DEBUG_FLAGS"
    )
    
    if [ "$VERBOSE" = true ]; then
        MAKE_ARGS+=(VERBOSE="-v")
    fi
    
    print_info "Building project..."
    if ! make -j"$JOBS" "${MAKE_ARGS[@]}"; then
        print_error "Build failed"
        exit 1
    fi
    
    print_success "Build successful"
}

# ============================================================================
# Build / البناء
# ============================================================================

if [ "$USE_CMAKE" = true ]; then
    build_cmake
elif [ "$USE_MAKE" = true ]; then
    build_make
fi

# ============================================================================
# الحمد لله - Run Tests / تشغيل الاختبارات
# ============================================================================

if [ "$RUN_TESTS" = true ]; then
    print_header "الحمد لله - Running tests"
    
    if [ "$USE_CMAKE" = true ]; then
        cd "$BUILD_DIR"
        if ! ctest --output-on-failure; then
            print_error "Tests failed"
            exit 1
        fi
        cd "$PROJECT_ROOT"
    elif [ "$USE_MAKE" = true ]; then
        if ! make test; then
            print_error "Tests failed"
            exit 1
        fi
    fi
    
    print_success "All tests passed!"
fi

# ============================================================================
# Install / التثبيت
# ============================================================================

if [ "$DO_INSTALL" = true ]; then
    print_header "الحمد لله - Installing"
    
    if [ "$USE_CMAKE" = true ]; then
        cd "$BUILD_DIR"
        if ! cmake --install .; then
            print_error "Installation failed"
            exit 1
        fi
        cd "$PROJECT_ROOT"
    elif [ "$USE_MAKE" = true ]; then
        if ! make install PREFIX="$INSTALL_PREFIX"; then
            print_error "Installation failed"
            exit 1
        fi
    fi
    
    print_success "Installation successful to $INSTALL_PREFIX"
fi

# ============================================================================
# الحمد لله - Summary / الملخص
# ============================================================================

print_header "الحمد لله - Build Complete!"

echo ""
echo "Build Summary:"
echo "  Build Type:    $BUILD_TYPE"
echo "  Jobs:          $JOBS"
echo "  Tests Run:     $RUN_TESTS"
echo "  Installed:     $DO_INSTALL"
if [ "$DO_INSTALL" = true ]; then
    echo "  Install Path:  $INSTALL_PREFIX"
fi
echo ""

print_success "الحمد لله - جميع المراحل نجحت! ✨"
