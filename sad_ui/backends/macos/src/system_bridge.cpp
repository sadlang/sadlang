/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.cpp
 * المسار: sad_ui/backends/macos/src/system_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ جسر macOS — يربط الأنظمة الثمانية بـ AppKit.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/macos/system_bridge.h"

#include <algorithm>
#include <cstdio>

namespace sad {
namespace ui {
namespace macos {

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSThemeBridge
// ═══════════════════════════════════════════════════════════════════════════════

void MacOSThemeBridge::applyColorScheme(const ColorScheme& scheme) {
    currentScheme_ = scheme;
}

void MacOSThemeBridge::applyTextTheme(const TextTheme& textTheme) {
    currentTextTheme_ = textTheme;
}

void MacOSThemeBridge::setDarkMode(bool dark) {
    darkMode_ = dark;
    currentScheme_ = dark ? ColorScheme::dark() : ColorScheme::light();
}

bool MacOSThemeBridge::isSystemDarkMode() const {
    // macOS: [[NSAppearance currentAppearance].name isEqualToString: NSAppearanceNameDarkAqua]
    return darkMode_;
}

uint32_t MacOSThemeBridge::toNativeColor(const ThemeColor& color) const {
    uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
    uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void MacOSThemeBridge::applyTheme(const ThemeProvider& provider) {
    applyColorScheme(provider.colorScheme());
    applyTextTheme(provider.textTheme());
}

std::string MacOSThemeBridge::toNSColor(const ThemeColor& color) const {
    char buf[128];
    std::snprintf(buf, sizeof(buf),
                  "[NSColor colorWithRed:%.3f green:%.3f blue:%.3f alpha:%.3f]",
                  color.r, color.g, color.b, color.a);
    return buf;
}

std::string MacOSThemeBridge::generateAppKitTheme() const {
    std::ostringstream objc;
    objc << "// SadUI macOS Theme — Generated\n";
    objc << "@interface SadTheme : NSObject\n";
    objc << "@property (nonatomic, strong) NSColor *primary;\n";
    objc << "@property (nonatomic, strong) NSColor *secondary;\n";
    objc << "@property (nonatomic, strong) NSColor *background;\n";
    objc << "@property (nonatomic, strong) NSColor *surface;\n";
    objc << "@property (nonatomic, strong) NSColor *error;\n";
    objc << "@end\n\n";
    objc << "@implementation SadTheme\n";
    objc << "- (instancetype)init {\n";
    objc << "    self = [super init];\n";
    objc << "    if (self) {\n";
    objc << "        _primary = " << toNSColor(currentScheme_.primary) << ";\n";
    objc << "        _secondary = " << toNSColor(currentScheme_.secondary) << ";\n";
    objc << "        _background = " << toNSColor(currentScheme_.background) << ";\n";
    objc << "        _surface = " << toNSColor(currentScheme_.surface) << ";\n";
    objc << "        _error = " << toNSColor(currentScheme_.error) << ";\n";
    objc << "    }\n";
    objc << "    return self;\n";
    objc << "}\n";
    objc << "@end\n";
    return objc.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSGestureBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool MacOSGestureBridge::translateNativeEvent(void* /*nativeEvent*/, GestureEvent& /*outEvent*/) {
    return false;  // معالج بواسطة AppKit NSEvent
}

void MacOSGestureBridge::registerDetector(const std::string& nodeId,
                                           std::shared_ptr<GestureDetector> detector) {
    detectors_[nodeId] = std::move(detector);
}

void MacOSGestureBridge::unregisterDetector(const std::string& nodeId) {
    detectors_.erase(nodeId);
}

void MacOSGestureBridge::setGestureEnabled(GestureType /*type*/, bool /*enabled*/) {}

bool MacOSGestureBridge::supportsGesture(GestureType type) const {
    // macOS يدعم: ماوس، trackpad gestures
    switch (type) {
        case GestureType::Tap:
        case GestureType::DoubleTap:
        case GestureType::Pan:
        case GestureType::HorizontalDrag:
        case GestureType::VerticalDrag:
        case GestureType::Scale:       // trackpad pinch
        case GestureType::Rotation:    // trackpad rotation
        case GestureType::Fling:
        case GestureType::ForceTap:    // Force Touch trackpad
            return true;
        case GestureType::LongPress:
        case GestureType::EdgeDrag:
            return false;
        default:
            return false;
    }
}

std::string MacOSGestureBridge::generateGestureRecognizers(const std::string& viewId) const {
    std::ostringstream objc;
    objc << "NSClickGestureRecognizer *click_" << viewId
         << " = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(handleClick:)];\n";
    objc << "[" << viewId << " addGestureRecognizer:click_" << viewId << "];\n";
    objc << "NSPanGestureRecognizer *pan_" << viewId
         << " = [[NSPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];\n";
    objc << "[" << viewId << " addGestureRecognizer:pan_" << viewId << "];\n";
    objc << "NSMagnificationGestureRecognizer *mag_" << viewId
         << " = [[NSMagnificationGestureRecognizer alloc] initWithTarget:self action:@selector(handleMagnify:)];\n";
    objc << "[" << viewId << " addGestureRecognizer:mag_" << viewId << "];\n";
    objc << "NSRotationGestureRecognizer *rot_" << viewId
         << " = [[NSRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotation:)];\n";
    objc << "[" << viewId << " addGestureRecognizer:rot_" << viewId << "];\n";
    return objc.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSScrollPhysicsBridge
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<ScrollPhysics> MacOSScrollPhysicsBridge::getPlatformScrollPhysics() const {
    // macOS يستخدم BouncingScrollPhysics (مثل iOS)
    return std::make_shared<BouncingScrollPhysics>();
}

void MacOSScrollPhysicsBridge::applyNativeScroll(const std::string& /*nodeId*/,
                                                  const ScrollMetrics& /*metrics*/) {}

void MacOSScrollPhysicsBridge::animateScrollTo(const std::string& /*nodeId*/,
                                                float /*targetOffset*/, float /*durationMs*/) {}

bool MacOSScrollPhysicsBridge::translateScrollEvent(void* /*nativeEvent*/,
                                                     ScrollMetrics& /*outMetrics*/) {
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSAccessibilityBridge
// ═══════════════════════════════════════════════════════════════════════════════

void MacOSAccessibilityBridge::applySemanticTree(const SemanticTree& tree) {
    nodeCache_.clear();
    for (const auto& node : tree.getNodes()) {
        nodeCache_[node.id] = node;
    }
}

void MacOSAccessibilityBridge::updateSemanticNode(const SemanticNode& node) {
    nodeCache_[node.id] = node;
}

void MacOSAccessibilityBridge::announce(const std::string& /*message*/,
                                         LiveRegion /*priority*/) {
    // macOS: NSAccessibilityPostNotification
}

void MacOSAccessibilityBridge::setNodeLabel(const std::string& nodeId,
                                             const std::string& label) {
    nodeCache_[nodeId].label = label;
}

void MacOSAccessibilityBridge::setNodeRole(const std::string& nodeId,
                                            SemanticRole role) {
    nodeCache_[nodeId].role = role;
}

bool MacOSAccessibilityBridge::isScreenReaderActive() const {
    // macOS: CFPreferencesCopyValue("voiceOverOnOffKey")
    return false;
}

std::string MacOSAccessibilityBridge::roleToNSAccessibility(SemanticRole role) {
    switch (role) {
        case SemanticRole::Button:      return "NSAccessibilityButtonRole";
        case SemanticRole::Link:        return "NSAccessibilityLinkRole";
        case SemanticRole::Image:       return "NSAccessibilityImageRole";
        case SemanticRole::Heading:     return "NSAccessibilityHeadingRole";
        case SemanticRole::TextField:   return "NSAccessibilityTextFieldRole";
        case SemanticRole::Checkbox:    return "NSAccessibilityCheckBoxRole";
        case SemanticRole::RadioButton: return "NSAccessibilityRadioButtonRole";
        case SemanticRole::Slider:      return "NSAccessibilitySliderRole";
        case SemanticRole::Tab:         return "NSAccessibilityTabRole";
        case SemanticRole::Table:       return "NSAccessibilityTableRole";
        case SemanticRole::Row:         return "NSAccessibilityRowRole";
        case SemanticRole::Cell:        return "NSAccessibilityCellRole";
        case SemanticRole::Menu:        return "NSAccessibilityMenuRole";
        case SemanticRole::MenuItem:    return "NSAccessibilityMenuItemRole";
        case SemanticRole::List:        return "NSAccessibilityListRole";
        case SemanticRole::ProgressBar: return "NSAccessibilityProgressIndicatorRole";
        case SemanticRole::ScrollBar:   return "NSAccessibilityScrollBarRole";
        case SemanticRole::Dialog:      return "NSAccessibilityDialogRole";
        default:                        return "NSAccessibilityUnknownRole";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSFormValidationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void MacOSFormValidationBridge::bindFormField(const std::string& nodeId,
                                               const FormField& field) {
    boundFields_[nodeId] = field;
}

void MacOSFormValidationBridge::showValidationError(const std::string& nodeId,
                                                     const std::string& errorMessage) {
    errorMessages_[nodeId] = errorMessage;
}

void MacOSFormValidationBridge::clearValidationError(const std::string& nodeId) {
    errorMessages_.erase(nodeId);
}

void MacOSFormValidationBridge::applyFormState(const FormState& state) {
    for (const auto& field : state.getFields()) {
        boundFields_[field.getName()] = field;
    }
}

void MacOSFormValidationBridge::enableNativeValidation(const std::string& /*nodeId*/,
                                                        bool /*enable*/) {}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSFocusBridge
// ═══════════════════════════════════════════════════════════════════════════════

void MacOSFocusBridge::requestFocus(const std::string& nodeId) {
    focusedNodeId_ = nodeId;
}

void MacOSFocusBridge::clearFocus() {
    focusedNodeId_.clear();
}

void MacOSFocusBridge::moveFocus(FocusDirection /*direction*/) {
    // macOS: [window selectNextKeyView] or [window selectPreviousKeyView]
}

void MacOSFocusBridge::setTabIndex(const std::string& nodeId, int tabIndex) {
    tabIndices_[nodeId] = tabIndex;
}

void MacOSFocusBridge::createFocusTrap(const std::string& /*scopeId*/) {}
void MacOSFocusBridge::releaseFocusTrap(const std::string& /*scopeId*/) {}

std::string MacOSFocusBridge::getFocusedNodeId() const {
    return focusedNodeId_;
}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSImageCacheBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool MacOSImageCacheBridge::loadImage(const std::string& /*path*/,
                                       std::function<void(const ImageData&)> callback) {
    if (callback) {
        ImageData emptyData;
        callback(emptyData);
    }
    return true;
}

void* MacOSImageCacheBridge::toNativeTexture(const ImageData& /*data*/) {
    return nullptr;  // NSImage / CGImage
}

void MacOSImageCacheBridge::releaseNativeTexture(void* /*texture*/) {}
void MacOSImageCacheBridge::enableDiskCache(const std::string& /*cacheDir*/, size_t /*maxSize*/) {}
void MacOSImageCacheBridge::clearCache() {}
void MacOSImageCacheBridge::prefetchImages(const std::vector<std::string>& /*urls*/) {}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSNotificationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void MacOSNotificationBridge::showNotification(const NotificationData& data) {
    activeNotifications_.push_back(data);
}

void MacOSNotificationBridge::dismissNotification(const std::string& id) {
    activeNotifications_.erase(
        std::remove_if(activeNotifications_.begin(), activeNotifications_.end(),
                        [&id](const NotificationData& n) { return n.id == id; }),
        activeNotifications_.end());
}

void MacOSNotificationBridge::dismissAll() {
    activeNotifications_.clear();
}

void MacOSNotificationBridge::requestPermission(std::function<void(bool)> callback) {
    if (callback) callback(permissionGranted_);
}

bool MacOSNotificationBridge::hasPermission() const {
    return permissionGranted_;
}

// ═══════════════════════════════════════════════════════════════════════════════
// MacOSSystemBridge
// ═══════════════════════════════════════════════════════════════════════════════

MacOSSystemBridge::MacOSSystemBridge() = default;
MacOSSystemBridge::~MacOSSystemBridge() {
    if (ready_) shutdown();
}

PlatformInfo MacOSSystemBridge::getPlatformInfo() const {
    PlatformInfo info;
    info.type = PlatformType::macOS;
    info.name = "macOS (AppKit)";
    info.version = "1.0.0";
    info.scaleFactor = 2.0f;  // Retina
    info.supportsTouch = false;
    info.supportsMouse = true;
    info.supportsKeyboard = true;
    info.supportsHaptic = true;   // Taptic Engine
    info.supportsForceTap = true; // Force Touch trackpad
    info.isRTL = true;
    return info;
}

bool MacOSSystemBridge::initialize() {
    ready_ = true;
    theme_.applyColorScheme(ColorScheme::light());
    return true;
}

void MacOSSystemBridge::shutdown() {
    notification_.dismissAll();
    ready_ = false;
}

bool MacOSSystemBridge::isReady() const {
    return ready_;
}

} // namespace macos
} // namespace ui
} // namespace sad
