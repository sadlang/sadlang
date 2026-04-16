/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.h
 * المسار: sad_ui/backends/web/include/sad_ui/web/system_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * جسر الويب (Web System Bridge).
 *
 * يربط الأنظمة الثمانية بـ HTML5/CSS3/JavaScript:
 * - Theme → CSS custom properties / CSS variables
 * - Gesture → PointerEvent / TouchEvent APIs
 * - ScrollPhysics → CSS scroll-behavior / JS scroll
 * - Accessibility → ARIA attributes / role / aria-label
 * - FormValidation → HTML5 constraint validation API
 * - Focus → tabindex / document.activeElement / focus()
 * - ImageCache → Image() + Canvas / createImageBitmap
 * - Notification → Notification API / DOM toast overlay
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WEB_SYSTEM_BRIDGE_H
#define SAD_UI_WEB_SYSTEM_BRIDGE_H

#include "sad_ui/system_bridge.h"

#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sad {
namespace ui {
namespace web {

// ═══════════════════════════════════════════════════════════════════════════════
// جسر السمات — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebThemeBridge : public ThemeBridge {
public:
    void applyColorScheme(const ColorScheme& scheme) override;
    void applyTextTheme(const TextTheme& textTheme) override;
    void setDarkMode(bool dark) override;
    bool isSystemDarkMode() const override;
    uint32_t toNativeColor(const ThemeColor& color) const override;
    void applyTheme(const ThemeProvider& provider) override;

    /// توليد CSS custom properties
    std::string generateCSSVariables() const;

    /// تحويل ThemeColor إلى CSS rgba()
    std::string toCSSColor(const ThemeColor& color) const;

private:
    ColorScheme currentScheme_;
    TextTheme currentTextTheme_;
    bool darkMode_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإيماءات — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebGestureBridge : public GestureBridge {
public:
    bool translateNativeEvent(void* nativeEvent, GestureEvent& outEvent) override;
    void registerDetector(const std::string& nodeId,
                          std::shared_ptr<GestureDetector> detector) override;
    void unregisterDetector(const std::string& nodeId) override;
    void setGestureEnabled(GestureType type, bool enabled) override;
    bool supportsGesture(GestureType type) const override;

    /// توليد JS event handlers
    std::string generateEventHandlers(const std::string& elementId) const;

private:
    std::unordered_map<std::string, std::shared_ptr<GestureDetector>> detectors_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر فيزياء التمرير — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebScrollPhysicsBridge : public ScrollPhysicsBridge {
public:
    std::shared_ptr<ScrollPhysics> getPlatformScrollPhysics() const override;
    void applyNativeScroll(const std::string& nodeId,
                           const ScrollMetrics& metrics) override;
    void animateScrollTo(const std::string& nodeId,
                         float targetOffset, float durationMs) override;
    bool translateScrollEvent(void* nativeEvent,
                              ScrollMetrics& outMetrics) override;

    /// توليد CSS scroll-behavior
    std::string generateScrollCSS(const std::string& elementId) const;

    /// توليد JS scrollTo
    std::string generateScrollJS(const std::string& elementId,
                                  float offset, bool smooth) const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر إمكانية الوصول — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebAccessibilityBridge : public AccessibilityBridge {
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

    /// تحويل SemanticRole إلى HTML role attribute
    static std::string roleToARIA(SemanticRole role);

    /// توليد ARIA attributes لعنصر
    std::string generateARIAAttributes(const std::string& nodeId) const;

private:
    std::unordered_map<std::string, SemanticNode> nodeCache_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التحقق من النماذج — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebFormValidationBridge : public FormValidationBridge {
public:
    void bindFormField(const std::string& nodeId,
                       const FormField& field) override;
    void showValidationError(const std::string& nodeId,
                             const std::string& errorMessage) override;
    void clearValidationError(const std::string& nodeId) override;
    void applyFormState(const FormState& state) override;
    void enableNativeValidation(const std::string& nodeId, bool enable) override;

    /// توليد HTML5 validation attributes
    std::string generateValidationAttrs(const std::string& nodeId) const;

    /// توليد JS validation script
    std::string generateValidationJS(const std::string& formId) const;

private:
    std::unordered_map<std::string, FormField> boundFields_;
    std::unordered_map<std::string, std::string> errorMessages_;
    std::unordered_map<std::string, bool> nativeValidation_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التركيز — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebFocusBridge : public FocusBridge {
public:
    void requestFocus(const std::string& nodeId) override;
    void clearFocus() override;
    void moveFocus(FocusDirection direction) override;
    void setTabIndex(const std::string& nodeId, int tabIndex) override;
    void createFocusTrap(const std::string& scopeId) override;
    void releaseFocusTrap(const std::string& scopeId) override;
    std::string getFocusedNodeId() const override;

    /// توليد JS focus management
    std::string generateFocusJS() const;

    /// توليد tabindex attribute
    std::string generateTabIndex(const std::string& nodeId) const;

private:
    std::string focusedNodeId_;
    std::unordered_map<std::string, int> tabIndices_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر ذاكرة الصور — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebImageCacheBridge : public ImageCacheBridge {
public:
    bool loadImage(const std::string& path,
                   std::function<void(const ImageData&)> callback) override;
    void* toNativeTexture(const ImageData& data) override;
    void releaseNativeTexture(void* texture) override;
    void enableDiskCache(const std::string& cacheDir, size_t maxSize) override;
    void clearCache() override;
    void prefetchImages(const std::vector<std::string>& urls) override;

    /// توليد JS image preload
    std::string generatePreloadJS(const std::vector<std::string>& urls) const;

    /// توليد HTML <link rel="preload">
    std::string generatePreloadHTML(const std::vector<std::string>& urls) const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإشعارات — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebNotificationBridge : public NotificationBridge {
public:
    void showNotification(const NotificationData& data) override;
    void dismissNotification(const std::string& id) override;
    void dismissAll() override;
    void requestPermission(std::function<void(bool)> callback) override;
    bool hasPermission() const override;

    /// توليد JS Notification API
    std::string generateNotificationJS(const NotificationData& data) const;

    /// توليد CSS لـ toast overlay
    std::string generateToastCSS() const;

    /// توليد HTML لـ toast overlay
    std::string generateToastHTML(const NotificationData& data) const;

private:
    std::vector<NotificationData> activeNotifications_;
    bool permissionGranted_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// الجسر الرئيسي — Web
// ═══════════════════════════════════════════════════════════════════════════════

class WebSystemBridge : public SystemBridge {
public:
    WebSystemBridge();
    ~WebSystemBridge();

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
    WebThemeBridge theme_;
    WebGestureBridge gesture_;
    WebScrollPhysicsBridge scrollPhysics_;
    WebAccessibilityBridge accessibility_;
    WebFormValidationBridge formValidation_;
    WebFocusBridge focus_;
    WebImageCacheBridge imageCache_;
    WebNotificationBridge notification_;
};

} // namespace web
} // namespace ui
} // namespace sad

#endif // SAD_UI_WEB_SYSTEM_BRIDGE_H
