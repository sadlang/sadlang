/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.h
 * المسار: features/graphics/backends/android/include/sad_ui/android/system_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * جسر أندرويد (Android System Bridge).
 *
 * يربط الأنظمة الثمانية بـ Jetpack Compose + Android APIs:
 * - Theme → MaterialTheme / Compose Color
 * - Gesture → Modifier.pointerInput / GestureDetector
 * - ScrollPhysics → LazyColumn / rememberScrollState
 * - Accessibility → AccessibilityNodeInfo / Semantics
 * - FormValidation → Compose TextField validation
 * - Focus → FocusRequester / FocusManager
 * - ImageCache → Coil / Glide integration
 * - Notification → NotificationManager / NotificationChannel
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_ANDROID_SYSTEM_BRIDGE_H
#define SAD_UI_ANDROID_SYSTEM_BRIDGE_H

#include "sad_ui/system_bridge.h"

#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sad {
namespace ui {
namespace android {

// ═══════════════════════════════════════════════════════════════════════════════
// جسر السمات — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidThemeBridge : public ThemeBridge {
public:
    void applyColorScheme(const ColorScheme& scheme) override;
    void applyTextTheme(const TextTheme& textTheme) override;
    void setDarkMode(bool dark) override;
    bool isSystemDarkMode() const override;
    uint32_t toNativeColor(const ThemeColor& color) const override;
    void applyTheme(const ThemeProvider& provider) override;

    /// توليد Compose MaterialTheme Kotlin code
    std::string generateMaterialTheme() const;

    /// تحويل ThemeColor إلى Compose Color
    std::string toComposeColor(const ThemeColor& color) const;

private:
    ColorScheme currentScheme_;
    TextTheme currentTextTheme_;
    bool darkMode_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإيماءات — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidGestureBridge : public GestureBridge {
public:
    bool translateNativeEvent(void* nativeEvent, GestureEvent& outEvent) override;
    void registerDetector(const std::string& nodeId,
                          std::shared_ptr<GestureDetector> detector) override;
    void unregisterDetector(const std::string& nodeId) override;
    void setGestureEnabled(GestureType type, bool enabled) override;
    bool supportsGesture(GestureType type) const override;

    /// توليد Compose gesture modifier
    std::string generateGestureModifier(const std::string& elementId) const;

private:
    std::unordered_map<std::string, std::shared_ptr<GestureDetector>> detectors_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر فيزياء التمرير — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidScrollPhysicsBridge : public ScrollPhysicsBridge {
public:
    std::shared_ptr<ScrollPhysics> getPlatformScrollPhysics() const override;
    void applyNativeScroll(const std::string& nodeId,
                           const ScrollMetrics& metrics) override;
    void animateScrollTo(const std::string& nodeId,
                         float targetOffset, float durationMs) override;
    bool translateScrollEvent(void* nativeEvent,
                              ScrollMetrics& outMetrics) override;

    /// توليد Compose scroll modifier
    std::string generateScrollModifier(const std::string& elementId) const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر إمكانية الوصول — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidAccessibilityBridge : public AccessibilityBridge {
public:
    void applySemanticTree(const SemanticTree& tree) override;
    void updateSemanticNode(const SemanticNode& node) override;
    void announce(const std::string& message,
                  LiveRegion priority = LiveRegion::Polite) override;
    void setNodeLabel(const std::string& nodeId,
                      const std::string& label) override;
    void setNodeRole(const std::string& nodeId,
                     SemanticRole role) override;
    bool isScreenReaderActive() const override;

    /// توليد Compose Semantics modifier
    std::string generateSemanticsModifier(const std::string& nodeId) const;

    /// تحويل SemanticRole إلى Android Role
    static std::string roleToAndroid(SemanticRole role);

private:
    std::unordered_map<std::string, SemanticNode> nodeCache_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التحقق من النماذج — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidFormValidationBridge : public FormValidationBridge {
public:
    void bindFormField(const std::string& nodeId,
                       const FormField& field) override;
    void showValidationError(const std::string& nodeId,
                             const std::string& errorMessage) override;
    void clearValidationError(const std::string& nodeId) override;
    void applyFormState(const FormState& state) override;
    void enableNativeValidation(const std::string& nodeId, bool enable) override;

    /// توليد Compose TextField مع validation
    std::string generateValidatedTextField(const std::string& nodeId) const;

private:
    std::unordered_map<std::string, FormField> boundFields_;
    std::unordered_map<std::string, std::string> errorMessages_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التركيز — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidFocusBridge : public FocusBridge {
public:
    void requestFocus(const std::string& nodeId) override;
    void clearFocus() override;
    void moveFocus(FocusDirection direction) override;
    void setTabIndex(const std::string& nodeId, int tabIndex) override;
    void createFocusTrap(const std::string& scopeId) override;
    void releaseFocusTrap(const std::string& scopeId) override;
    std::string getFocusedNodeId() const override;

    /// توليد Compose FocusRequester
    std::string generateFocusModifier(const std::string& nodeId) const;

private:
    std::string focusedNodeId_;
    std::unordered_map<std::string, int> tabIndices_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر ذاكرة الصور — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidImageCacheBridge : public ImageCacheBridge {
public:
    bool loadImage(const std::string& path,
                   std::function<void(const ImageData&)> callback) override;
    void* toNativeTexture(const ImageData& data) override;
    void releaseNativeTexture(void* texture) override;
    void enableDiskCache(const std::string& cacheDir, size_t maxSize) override;
    void clearCache() override;
    void prefetchImages(const std::vector<std::string>& urls) override;

    /// توليد Compose AsyncImage
    std::string generateAsyncImage(const std::string& url,
                                    const std::string& modifier) const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإشعارات — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidNotificationBridge : public NotificationBridge {
public:
    void showNotification(const NotificationData& data) override;
    void dismissNotification(const std::string& id) override;
    void dismissAll() override;
    void requestPermission(std::function<void(bool)> callback) override;
    bool hasPermission() const override;
    void setNotificationChannel(const std::string& channelId,
                                 const std::string& channelName) override;

    /// توليد Kotlin notification code
    std::string generateNotificationCode(const NotificationData& data) const;

    /// توليد Compose Snackbar
    std::string generateSnackbar(const NotificationData& data) const;

private:
    std::vector<NotificationData> activeNotifications_;
    bool permissionGranted_ = false;
    std::string channelId_ = "sad_default";
    std::string channelName_ = "إشعارات التطبيق";
};

// ═══════════════════════════════════════════════════════════════════════════════
// الجسر الرئيسي — Android
// ═══════════════════════════════════════════════════════════════════════════════

class AndroidSystemBridge : public SystemBridge {
public:
    AndroidSystemBridge();
    ~AndroidSystemBridge();

    PlatformInfo getPlatformInfo() const override;
    bool initialize() override;
    void shutdown() override;
    bool isReady() const override;

    ThemeBridge& theme() override { return theme_; }
    GestureBridge& gesture() override { return gesture_; }
    ScrollPhysicsBridge& scrollPhysics() override { return scrollPhysics_; }
    AccessibilityBridge& accessibility() override { return accessibility_; }
    FormValidationBridge& formValidation() override { return formValidation_; }
    FocusBridge& focus() override { return focus_; }
    ImageCacheBridge& imageCache() override { return imageCache_; }
    NotificationBridge& notification() override { return notification_; }

private:
    bool ready_ = false;
    AndroidThemeBridge theme_;
    AndroidGestureBridge gesture_;
    AndroidScrollPhysicsBridge scrollPhysics_;
    AndroidAccessibilityBridge accessibility_;
    AndroidFormValidationBridge formValidation_;
    AndroidFocusBridge focus_;
    AndroidImageCacheBridge imageCache_;
    AndroidNotificationBridge notification_;
};

} // namespace android
} // namespace ui
} // namespace sad

#endif // SAD_UI_ANDROID_SYSTEM_BRIDGE_H
