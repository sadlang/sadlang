/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.h
 * المسار: features/graphics/backends/macos/include/sad_ui/macos/system_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * جسر macOS (macOS System Bridge).
 *
 * يربط الأنظمة الثمانية بـ AppKit + CoreGraphics:
 * - Theme → NSAppearance / NSColor
 * - Gesture → NSGestureRecognizer / NSEvent
 * - ScrollPhysics → NSScrollView (elastic scroll)
 * - Accessibility → NSAccessibility protocol
 * - FormValidation → NSTextField validation
 * - Focus → NSWindow key view / First Responder
 * - ImageCache → NSImage / CGImage
 * - Notification → NSUserNotificationCenter / UNUserNotificationCenter
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_MACOS_SYSTEM_BRIDGE_H
#define SAD_UI_MACOS_SYSTEM_BRIDGE_H

#include "sad_ui/system_bridge.h"

#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sad {
namespace ui {
namespace macos {

// ═══════════════════════════════════════════════════════════════════════════════
// جسر السمات — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSThemeBridge : public ThemeBridge {
public:
    void applyColorScheme(const ColorScheme& scheme) override;
    void applyTextTheme(const TextTheme& textTheme) override;
    void setDarkMode(bool dark) override;
    bool isSystemDarkMode() const override;
    uint32_t toNativeColor(const ThemeColor& color) const override;
    void applyTheme(const ThemeProvider& provider) override;

    /// توليد NSColor code
    std::string toNSColor(const ThemeColor& color) const;

    /// توليد AppKit theme code
    std::string generateAppKitTheme() const;

private:
    ColorScheme currentScheme_;
    TextTheme currentTextTheme_;
    bool darkMode_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإيماءات — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSGestureBridge : public GestureBridge {
public:
    bool translateNativeEvent(void* nativeEvent, GestureEvent& outEvent) override;
    void registerDetector(const std::string& nodeId,
                          std::shared_ptr<GestureDetector> detector) override;
    void unregisterDetector(const std::string& nodeId) override;
    void setGestureEnabled(GestureType type, bool enabled) override;
    bool supportsGesture(GestureType type) const override;

    /// توليد NSGestureRecognizer code
    std::string generateGestureRecognizers(const std::string& viewId) const;

private:
    std::unordered_map<std::string, std::shared_ptr<GestureDetector>> detectors_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر فيزياء التمرير — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSScrollPhysicsBridge : public ScrollPhysicsBridge {
public:
    std::shared_ptr<ScrollPhysics> getPlatformScrollPhysics() const override;
    void applyNativeScroll(const std::string& nodeId,
                           const ScrollMetrics& metrics) override;
    void animateScrollTo(const std::string& nodeId,
                         float targetOffset, float durationMs) override;
    bool translateScrollEvent(void* nativeEvent,
                              ScrollMetrics& outMetrics) override;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر إمكانية الوصول — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSAccessibilityBridge : public AccessibilityBridge {
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

    /// تحويل SemanticRole إلى NSAccessibilityRole
    static std::string roleToNSAccessibility(SemanticRole role);

private:
    std::unordered_map<std::string, SemanticNode> nodeCache_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التحقق من النماذج — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSFormValidationBridge : public FormValidationBridge {
public:
    void bindFormField(const std::string& nodeId,
                       const FormField& field) override;
    void showValidationError(const std::string& nodeId,
                             const std::string& errorMessage) override;
    void clearValidationError(const std::string& nodeId) override;
    void applyFormState(const FormState& state) override;
    void enableNativeValidation(const std::string& nodeId, bool enable) override;

private:
    std::unordered_map<std::string, FormField> boundFields_;
    std::unordered_map<std::string, std::string> errorMessages_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التركيز — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSFocusBridge : public FocusBridge {
public:
    void requestFocus(const std::string& nodeId) override;
    void clearFocus() override;
    void moveFocus(FocusDirection direction) override;
    void setTabIndex(const std::string& nodeId, int tabIndex) override;
    void createFocusTrap(const std::string& scopeId) override;
    void releaseFocusTrap(const std::string& scopeId) override;
    std::string getFocusedNodeId() const override;

private:
    std::string focusedNodeId_;
    std::unordered_map<std::string, int> tabIndices_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر ذاكرة الصور — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSImageCacheBridge : public ImageCacheBridge {
public:
    bool loadImage(const std::string& path,
                   std::function<void(const ImageData&)> callback) override;
    void* toNativeTexture(const ImageData& data) override;
    void releaseNativeTexture(void* texture) override;
    void enableDiskCache(const std::string& cacheDir, size_t maxSize) override;
    void clearCache() override;
    void prefetchImages(const std::vector<std::string>& urls) override;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإشعارات — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSNotificationBridge : public NotificationBridge {
public:
    void showNotification(const NotificationData& data) override;
    void dismissNotification(const std::string& id) override;
    void dismissAll() override;
    void requestPermission(std::function<void(bool)> callback) override;
    bool hasPermission() const override;

private:
    std::vector<NotificationData> activeNotifications_;
    bool permissionGranted_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// الجسر الرئيسي — macOS
// ═══════════════════════════════════════════════════════════════════════════════

class MacOSSystemBridge : public SystemBridge {
public:
    MacOSSystemBridge();
    ~MacOSSystemBridge();

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
    MacOSThemeBridge theme_;
    MacOSGestureBridge gesture_;
    MacOSScrollPhysicsBridge scrollPhysics_;
    MacOSAccessibilityBridge accessibility_;
    MacOSFormValidationBridge formValidation_;
    MacOSFocusBridge focus_;
    MacOSImageCacheBridge imageCache_;
    MacOSNotificationBridge notification_;
};

} // namespace macos
} // namespace ui
} // namespace sad

#endif // SAD_UI_MACOS_SYSTEM_BRIDGE_H
