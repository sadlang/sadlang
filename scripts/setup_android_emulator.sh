#!/bin/bash
# ═══════════════════════════════════════════════════════════════════════════
# setup_android_emulator.sh - إعداد محاكي أندرويد للاختبار
# Android Emulator Setup Script for Testing Sad Apps (Linux/macOS)
# ═══════════════════════════════════════════════════════════════════════════

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Defaults
API_LEVEL="${API_LEVEL:-34}"
NDK_VERSION="${NDK_VERSION:-25.2.9519653}"
AVD_NAME="${AVD_NAME:-Sad_Test_Device}"

# ═══════════════════════════════════════════════════════════════════════════
# Functions
# ═══════════════════════════════════════════════════════════════════════════

print_header() {
    echo ""
    echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
    echo -e "${YELLOW}  $1${NC}"
    echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
}

print_step() {
    echo ""
    echo -e "${GREEN}[$1] $2${NC}"
}

print_success() {
    echo -e "  ${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "  ${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "  ${RED}✗ $1${NC}"
}

# ═══════════════════════════════════════════════════════════════════════════
# Find Android SDK
# ═══════════════════════════════════════════════════════════════════════════

find_android_sdk() {
    # 1. ANDROID_SDK_ROOT
    if [ -n "$ANDROID_SDK_ROOT" ] && [ -d "$ANDROID_SDK_ROOT" ]; then
        echo "$ANDROID_SDK_ROOT"
        return 0
    fi
    
    # 2. ANDROID_HOME
    if [ -n "$ANDROID_HOME" ] && [ -d "$ANDROID_HOME" ]; then
        echo "$ANDROID_HOME"
        return 0
    fi
    
    # 3. Common paths
    local paths=(
        "$HOME/Android/Sdk"
        "$HOME/Library/Android/sdk"
        "/opt/android-sdk"
        "/usr/local/android-sdk"
    )
    
    for path in "${paths[@]}"; do
        if [ -d "$path" ]; then
            echo "$path"
            return 0
        fi
    done
    
    return 1
}

# ═══════════════════════════════════════════════════════════════════════════
# Main
# ═══════════════════════════════════════════════════════════════════════════

print_header "إعداد محاكي أندرويد للغة ص"
echo "  Android Emulator Setup for Sad Language Testing"

# 1. Find SDK
print_step "1/5" "البحث عن Android SDK..."

SDK_PATH="${SDK_PATH:-$(find_android_sdk)}"

if [ -z "$SDK_PATH" ]; then
    print_error "لم يتم العثور على Android SDK!"
    echo ""
    echo "  يرجى تثبيت Android Studio أو Command Line Tools من:"
    echo "  https://developer.android.com/studio"
    echo ""
    echo "  أو قم بتعيين مسار SDK:"
    echo "  SDK_PATH=/path/to/sdk ./setup_android_emulator.sh"
    exit 1
fi

print_success "تم العثور على SDK: $SDK_PATH"

export ANDROID_SDK_ROOT="$SDK_PATH"

# Set tool paths
SDKMANAGER="$SDK_PATH/cmdline-tools/latest/bin/sdkmanager"
AVDMANAGER="$SDK_PATH/cmdline-tools/latest/bin/avdmanager"
EMULATOR="$SDK_PATH/emulator/emulator"

[ ! -f "$SDKMANAGER" ] && SDKMANAGER="$SDK_PATH/tools/bin/sdkmanager"
[ ! -f "$AVDMANAGER" ] && AVDMANAGER="$SDK_PATH/tools/bin/avdmanager"

if [ ! -f "$SDKMANAGER" ]; then
    print_error "لم يتم العثور على sdkmanager!"
    echo "  يرجى تثبيت Command Line Tools من SDK Manager"
    exit 1
fi

# 2. Install SDK components
print_step "2/5" "تثبيت مكونات SDK المطلوبة..."

# Determine system image based on OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS - use ARM for Apple Silicon or x86_64 for Intel
    if [[ $(uname -m) == "arm64" ]]; then
        SYSTEM_IMAGE="system-images;android-$API_LEVEL;google_apis;arm64-v8a"
    else
        SYSTEM_IMAGE="system-images;android-$API_LEVEL;google_apis;x86_64"
    fi
else
    SYSTEM_IMAGE="system-images;android-$API_LEVEL;google_apis;x86_64"
fi

components=(
    "platform-tools"
    "platforms;android-$API_LEVEL"
    "build-tools;$API_LEVEL.0.0"
    "ndk;$NDK_VERSION"
    "$SYSTEM_IMAGE"
    "emulator"
)

for component in "${components[@]}"; do
    echo "  - تثبيت $component..."
    yes | "$SDKMANAGER" --install "$component" >/dev/null 2>&1 && \
        print_success "$component" || \
        print_warning "فشل تثبيت $component (قد يكون مثبتاً)"
done

# 3. Create AVD
print_step "3/5" "إنشاء جهاز افتراضي (AVD)..."

# Delete old AVD if exists
"$AVDMANAGER" delete avd -n "$AVD_NAME" 2>/dev/null || true

# Create new AVD
echo "  إنشاء $AVD_NAME..."
echo "no" | "$AVDMANAGER" create avd \
    -n "$AVD_NAME" \
    -k "$SYSTEM_IMAGE" \
    -d "pixel_6" \
    --force >/dev/null 2>&1

if [ $? -eq 0 ]; then
    print_success "تم إنشاء AVD: $AVD_NAME"
else
    print_error "فشل إنشاء AVD"
    exit 1
fi

# 4. Set environment variables
print_step "4/5" "تعيين متغيرات البيئة..."

NDK_PATH="$SDK_PATH/ndk/$NDK_VERSION"
if [ -d "$NDK_PATH" ]; then
    export ANDROID_NDK_HOME="$NDK_PATH"
    print_success "ANDROID_NDK_HOME = $NDK_PATH"
    
    # Add to shell profile
    SHELL_PROFILE="$HOME/.bashrc"
    [ -f "$HOME/.zshrc" ] && SHELL_PROFILE="$HOME/.zshrc"
    
    if ! grep -q "ANDROID_NDK_HOME" "$SHELL_PROFILE" 2>/dev/null; then
        echo "" >> "$SHELL_PROFILE"
        echo "# Android SDK for Sad Language" >> "$SHELL_PROFILE"
        echo "export ANDROID_SDK_ROOT=\"$SDK_PATH\"" >> "$SHELL_PROFILE"
        echo "export ANDROID_NDK_HOME=\"$NDK_PATH\"" >> "$SHELL_PROFILE"
        print_success "تم حفظ متغيرات البيئة في $SHELL_PROFILE"
    fi
else
    print_warning "NDK غير موجود في المسار المتوقع"
fi

# 5. Test setup
print_step "5/5" "اختبار الإعداد..."

ADB="$SDK_PATH/platform-tools/adb"
if [ -f "$ADB" ]; then
    ADB_VERSION=$("$ADB" version 2>&1 | head -1)
    print_success "ADB: $ADB_VERSION"
else
    print_warning "ADB غير موجود"
fi

if [ -f "$EMULATOR" ]; then
    print_success "المحاكي جاهز: $EMULATOR"
else
    print_warning "المحاكي غير موجود"
fi

# ═══════════════════════════════════════════════════════════════════════════
# Summary
# ═══════════════════════════════════════════════════════════════════════════

print_header "اكتمل الإعداد!"

echo ""
echo "  الخطوات التالية:"
echo ""
echo "  1. شغّل المحاكي:"
echo -e "     ${CYAN}emulator -avd $AVD_NAME${NC}"
echo ""
echo "  2. ابنِ تطبيق ص:"
echo -e "     ${CYAN}sadc build android تطبيقي.ص${NC}"
echo ""
echo "  3. ثبّت على المحاكي:"
echo -e "     ${CYAN}adb install -r build_android/app/build/outputs/apk/debug/app-debug.apk${NC}"
echo ""

# Create quick run script
cat > "$(dirname "$0")/run_emulator.sh" << EOF
#!/bin/bash
# تشغيل محاكي أندرويد
"$EMULATOR" -avd $AVD_NAME
EOF
chmod +x "$(dirname "$0")/run_emulator.sh"
print_success "تم إنشاء سكريبت التشغيل: run_emulator.sh"

echo ""
echo -e "${CYAN}═══════════════════════════════════════════════════════════════${NC}"
