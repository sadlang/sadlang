/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.h
 * المسار: sad_ui/backends/ios/include/sad_ui/ios/system_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * جسر iOS (iOS System Bridge).
 *
 * يربط الأنظمة الثمانية بـ SwiftUI + UIKit:
 * - Theme → SwiftUI Color / UIColor / colorScheme
 * - Gesture → UIGestureRecognizer / SwiftUI DragGesture
 * - ScrollPhysics → UIScrollView physics (bouncing)
 * - Accessibility → UIAccessibility / VoiceOver
 * - FormValidation → SwiftUI TextField + validation
 * - Focus → @FocusState / FocusedField
 * - ImageCache → UIImage / AsyncImage
 * - Notification → UNUserNotificationCenter
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_IOS_SYSTEM_BRIDGE_H
#define SAD_UI_IOS_SYSTEM_BRIDGE_H

#include "sad_ui/system_bridge.h"

#include <string>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace sad {
namespace ui {
namespace ios {

// ═══════════════════════════════════════════════════════════════════════════════
// جسر السمات — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSThemeBridge : public ThemeBridge {
public:
    void applyColorScheme(const ColorScheme& scheme) override;
    void applyTextTheme(const TextTheme& textTheme) override;
    void setDarkMode(bool dark) override;
    bool isSystemDarkMode() const override;
    uint32_t toNativeColor(const ThemeColor& color) const override;
    void applyTheme(const ThemeProvider& provider) override;

    /// توليد SwiftUI Color
    std::string toSwiftUIColor(const ThemeColor& color) const;

    /// توليد SwiftUI Theme struct
    std::string generateSwiftUITheme() const;

private:
    ColorScheme currentScheme_;
    TextTheme currentTextTheme_;
    bool darkMode_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإيماءات — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSGestureBridge : public GestureBridge {
public:
    bool translateNativeEvent(void* nativeEvent, GestureEvent& outEvent) override;
    void registerDetector(const std::string& nodeId,
                          std::shared_ptr<GestureDetector> detector) override;
    void unregisterDetector(const std::string& nodeId) override;
    void setGestureEnabled(GestureType type, bool enabled) override;
    bool supportsGesture(GestureType type) const override;

    /// توليد SwiftUI Gesture modifier
    std::string generateGestureModifier(const std::string& elementId) const;

private:
    std::unordered_map<std::string, std::shared_ptr<GestureDetector>> detectors_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر فيزياء التمرير — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSScrollPhysicsBridge : public ScrollPhysicsBridge {
public:
    std::shared_ptr<ScrollPhysics> getPlatformScrollPhysics() const override;
    void applyNativeScroll(const std::string& nodeId,
                           const ScrollMetrics& metrics) override;
    void animateScrollTo(const std::string& nodeId,
                         float targetOffset, float durationMs) override;
    bool translateScrollEvent(void* nativeEvent,
                              ScrollMetrics& outMetrics) override;

    /// توليد SwiftUI ScrollView code
    std::string generateScrollView(const std::string& elementId) const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر إمكانية الوصول — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSAccessibilityBridge : public AccessibilityBridge {
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

    /// توليد SwiftUI accessibility modifiers
    std::string generateAccessibilityModifier(const std::string& nodeId) const;

    /// تحويل SemanticRole إلى AccessibilityTraits
    static std::string roleToiOSTrait(SemanticRole role);

private:
    std::unordered_map<std::string, SemanticNode> nodeCache_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التحقق من النماذج — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSFormValidationBridge : public FormValidationBridge {
public:
    void bindFormField(const std::string& nodeId,
                       const FormField& field) override;
    void showValidationError(const std::string& nodeId,
                             const std::string& errorMessage) override;
    void clearValidationError(const std::string& nodeId) override;
    void applyFormState(const FormState& state) override;
    void enableNativeValidation(const std::string& nodeId, bool enable) override;

    /// توليد SwiftUI validated TextField
    std::string generateValidatedField(const std::string& nodeId) const;

private:
    std::unordered_map<std::string, FormField> boundFields_;
    std::unordered_map<std::string, std::string> errorMessages_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر التركيز — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSFocusBridge : public FocusBridge {
public:
    void requestFocus(const std::string& nodeId) override;
    void clearFocus() override;
    void moveFocus(FocusDirection direction) override;
    void setTabIndex(const std::string& nodeId, int tabIndex) override;
    void createFocusTrap(const std::string& scopeId) override;
    void releaseFocusTrap(const std::string& scopeId) override;
    std::string getFocusedNodeId() const override;

    /// توليد SwiftUI @FocusState code
    std::string generateFocusState() const;

private:
    std::string focusedNodeId_;
    std::unordered_map<std::string, int> tabIndices_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر ذاكرة الصور — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSImageCacheBridge : public ImageCacheBridge {
public:
    bool loadImage(const std::string& path,
                   std::function<void(const ImageData&)> callback) override;
    void* toNativeTexture(const ImageData& data) override;
    void releaseNativeTexture(void* texture) override;
    void enableDiskCache(const std::string& cacheDir, size_t maxSize) override;
    void clearCache() override;
    void prefetchImages(const std::vector<std::string>& urls) override;

    /// توليد SwiftUI AsyncImage
    std::string generateAsyncImage(const std::string& url) const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// جسر الإشعارات — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSNotificationBridge : public NotificationBridge {
public:
    void showNotification(const NotificationData& data) override;
    void dismissNotification(const std::string& id) override;
    void dismissAll() override;
    void requestPermission(std::function<void(bool)> callback) override;
    bool hasPermission() const override;

    /// توليد Swift notification code
    std::string generateNotificationCode(const NotificationData& data) const;

    /// توليد SwiftUI alert/sheet
    std::string generateSwiftUIAlert(const NotificationData& data) const;

private:
    std::vector<NotificationData> activeNotifications_;
    bool permissionGranted_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// الجسر الرئيسي — iOS
// ═══════════════════════════════════════════════════════════════════════════════

class IOSSystemBridge : public SystemBridge {
public:
    IOSSystemBridge();
    ~IOSSystemBridge();

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
    IOSThemeBridge theme_;
    IOSGestureBridge gesture_;
    IOSScrollPhysicsBridge scrollPhysics_;
    IOSAccessibilityBridge accessibility_;
    IOSFormValidationBridge formValidation_;
    IOSFocusBridge focus_;
    IOSImageCacheBridge imageCache_;
    IOSNotificationBridge notification_;
};

} // namespace ios
} // namespace ui
} // namespace sad

#endif // SAD_UI_IOS_SYSTEM_BRIDGE_H
