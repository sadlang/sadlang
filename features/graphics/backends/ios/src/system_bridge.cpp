/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.cpp
 * المسار: features/graphics/backends/ios/src/system_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ جسر iOS — يولّد Swift/SwiftUI لربط الأنظمة الثمانية.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/ios/system_bridge.h"

#include <algorithm>
#include <cstdio>

namespace sad {
namespace ui {
namespace ios {

// ═══════════════════════════════════════════════════════════════════════════════
// IOSThemeBridge
// ═══════════════════════════════════════════════════════════════════════════════

void IOSThemeBridge::applyColorScheme(const ColorScheme& scheme) {
    currentScheme_ = scheme;
}

void IOSThemeBridge::applyTextTheme(const TextTheme& textTheme) {
    currentTextTheme_ = textTheme;
}

void IOSThemeBridge::setDarkMode(bool dark) {
    darkMode_ = dark;
    currentScheme_ = dark ? ColorScheme::dark() : ColorScheme::light();
}

bool IOSThemeBridge::isSystemDarkMode() const {
    return darkMode_;
}

uint32_t IOSThemeBridge::toNativeColor(const ThemeColor& color) const {
    uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
    uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void IOSThemeBridge::applyTheme(const ThemeProvider& provider) {
    applyColorScheme(provider.colorScheme());
    applyTextTheme(provider.textTheme());
}

std::string IOSThemeBridge::toSwiftUIColor(const ThemeColor& color) const {
    char buf[128];
    std::snprintf(buf, sizeof(buf),
                  "Color(red: %.3f, green: %.3f, blue: %.3f, opacity: %.3f)",
                  color.r, color.g, color.b, color.a);
    return buf;
}

std::string IOSThemeBridge::generateSwiftUITheme() const {
    std::ostringstream swift;
    swift << "struct SadTheme {\n";
    swift << "    let primary = " << toSwiftUIColor(currentScheme_.primary) << "\n";
    swift << "    let onPrimary = " << toSwiftUIColor(currentScheme_.onPrimary) << "\n";
    swift << "    let secondary = " << toSwiftUIColor(currentScheme_.secondary) << "\n";
    swift << "    let background = " << toSwiftUIColor(currentScheme_.background) << "\n";
    swift << "    let onBackground = " << toSwiftUIColor(currentScheme_.onBackground) << "\n";
    swift << "    let surface = " << toSwiftUIColor(currentScheme_.surface) << "\n";
    swift << "    let onSurface = " << toSwiftUIColor(currentScheme_.onSurface) << "\n";
    swift << "    let error = " << toSwiftUIColor(currentScheme_.error) << "\n";
    swift << "    let success = " << toSwiftUIColor(currentScheme_.success) << "\n";
    swift << "    let warning = " << toSwiftUIColor(currentScheme_.warning) << "\n";
    swift << "}\n\n";
    swift << "extension EnvironmentValues {\n";
    swift << "    var sadTheme: SadTheme {\n";
    swift << "        get { self[SadThemeKey.self] }\n";
    swift << "        set { self[SadThemeKey.self] = newValue }\n";
    swift << "    }\n";
    swift << "}\n";
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSGestureBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool IOSGestureBridge::translateNativeEvent(void* /*nativeEvent*/, GestureEvent& /*outEvent*/) {
    return false;  // معالج بواسطة SwiftUI
}

void IOSGestureBridge::registerDetector(const std::string& nodeId,
                                         std::shared_ptr<GestureDetector> detector) {
    detectors_[nodeId] = std::move(detector);
}

void IOSGestureBridge::unregisterDetector(const std::string& nodeId) {
    detectors_.erase(nodeId);
}

void IOSGestureBridge::setGestureEnabled(GestureType /*type*/, bool /*enabled*/) {}

bool IOSGestureBridge::supportsGesture(GestureType type) const {
    // iOS يدعم جميع الإيماءات
    (void)type;
    return true;
}

std::string IOSGestureBridge::generateGestureModifier(const std::string& /*elementId*/) const {
    std::ostringstream swift;
    swift << ".gesture(\n";
    swift << "    TapGesture().onEnded { sadGesture.onTap() }\n";
    swift << ")\n";
    swift << ".gesture(\n";
    swift << "    DragGesture()\n";
    swift << "        .onChanged { value in sadGesture.onDrag(value.translation) }\n";
    swift << "        .onEnded { value in sadGesture.onDragEnd(value.predictedEndTranslation) }\n";
    swift << ")\n";
    swift << ".gesture(\n";
    swift << "    MagnificationGesture()\n";
    swift << "        .onChanged { value in sadGesture.onScale(value) }\n";
    swift << ")\n";
    swift << ".gesture(\n";
    swift << "    RotationGesture()\n";
    swift << "        .onChanged { value in sadGesture.onRotation(value) }\n";
    swift << ")\n";
    swift << ".gesture(\n";
    swift << "    LongPressGesture(minimumDuration: 0.5)\n";
    swift << "        .onEnded { _ in sadGesture.onLongPress() }\n";
    swift << ")\n";
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSScrollPhysicsBridge
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<ScrollPhysics> IOSScrollPhysicsBridge::getPlatformScrollPhysics() const {
    // iOS يستخدم BouncingScrollPhysics (ارتداد مرن)
    return std::make_shared<BouncingScrollPhysics>();
}

void IOSScrollPhysicsBridge::applyNativeScroll(const std::string& /*nodeId*/,
                                                const ScrollMetrics& /*metrics*/) {}

void IOSScrollPhysicsBridge::animateScrollTo(const std::string& /*nodeId*/,
                                              float /*targetOffset*/, float /*durationMs*/) {}

bool IOSScrollPhysicsBridge::translateScrollEvent(void* /*nativeEvent*/,
                                                   ScrollMetrics& /*outMetrics*/) {
    return false;
}

std::string IOSScrollPhysicsBridge::generateScrollView(const std::string& /*elementId*/) const {
    return "ScrollView(.vertical, showsIndicators: true) {\n    // محتوى\n}\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSAccessibilityBridge
// ═══════════════════════════════════════════════════════════════════════════════

void IOSAccessibilityBridge::applySemanticTree(const SemanticTree& tree) {
    nodeCache_.clear();
    for (const auto& node : tree.getNodes()) {
        nodeCache_[node.id] = node;
    }
}

void IOSAccessibilityBridge::updateSemanticNode(const SemanticNode& node) {
    nodeCache_[node.id] = node;
}

void IOSAccessibilityBridge::announce(const std::string& /*message*/,
                                       LiveRegion /*priority*/) {
    // iOS: UIAccessibility.post(notification: .announcement, argument: message)
}

void IOSAccessibilityBridge::setNodeLabel(const std::string& nodeId,
                                           const std::string& label) {
    nodeCache_[nodeId].label = label;
}

void IOSAccessibilityBridge::setNodeRole(const std::string& nodeId,
                                          SemanticRole role) {
    nodeCache_[nodeId].role = role;
}

bool IOSAccessibilityBridge::isScreenReaderActive() const {
    // iOS: UIAccessibility.isVoiceOverRunning
    return false;
}

std::string IOSAccessibilityBridge::roleToiOSTrait(SemanticRole role) {
    switch (role) {
        case SemanticRole::Button:      return ".isButton";
        case SemanticRole::Link:        return ".isLink";
        case SemanticRole::Image:       return ".isImage";
        case SemanticRole::Heading:     return ".isHeader";
        case SemanticRole::TextField:   return ".isSearchField";
        case SemanticRole::StaticText:  return ".isStaticText";
        default:                        return "";
    }
}

std::string IOSAccessibilityBridge::generateAccessibilityModifier(const std::string& nodeId) const {
    auto it = nodeCache_.find(nodeId);
    if (it == nodeCache_.end()) return "";

    const auto& node = it->second;
    std::ostringstream swift;
    if (!node.label.empty()) {
        swift << ".accessibilityLabel(\"" << node.label << "\")\n";
    }
    if (!node.hint.empty()) {
        swift << ".accessibilityHint(\"" << node.hint << "\")\n";
    }
    auto trait = roleToiOSTrait(node.role);
    if (!trait.empty()) {
        swift << ".accessibilityAddTraits(" << trait << ")\n";
    }
    if (!node.enabled) {
        swift << ".accessibilityRemoveTraits(.isEnabled)\n";
    }
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSFormValidationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void IOSFormValidationBridge::bindFormField(const std::string& nodeId,
                                             const FormField& field) {
    boundFields_[nodeId] = field;
}

void IOSFormValidationBridge::showValidationError(const std::string& nodeId,
                                                   const std::string& errorMessage) {
    errorMessages_[nodeId] = errorMessage;
}

void IOSFormValidationBridge::clearValidationError(const std::string& nodeId) {
    errorMessages_.erase(nodeId);
}

void IOSFormValidationBridge::applyFormState(const FormState& state) {
    for (const auto& field : state.getFields()) {
        boundFields_[field.getName()] = field;
    }
}

void IOSFormValidationBridge::enableNativeValidation(const std::string& /*nodeId*/,
                                                      bool /*enable*/) {}

std::string IOSFormValidationBridge::generateValidatedField(const std::string& nodeId) const {
    auto errorIt = errorMessages_.find(nodeId);

    std::ostringstream swift;
    swift << "VStack(alignment: .leading) {\n";
    swift << "    TextField(\"" << nodeId << "\", text: $" << nodeId << ")\n";
    swift << "        .textFieldStyle(.roundedBorder)\n";
    if (errorIt != errorMessages_.end()) {
        swift << "        .border(Color.red)\n";
        swift << "    Text(\"" << errorIt->second << "\")\n";
        swift << "        .font(.caption)\n";
        swift << "        .foregroundColor(.red)\n";
    }
    swift << "}\n";
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSFocusBridge
// ═══════════════════════════════════════════════════════════════════════════════

void IOSFocusBridge::requestFocus(const std::string& nodeId) {
    focusedNodeId_ = nodeId;
}

void IOSFocusBridge::clearFocus() {
    focusedNodeId_.clear();
}

void IOSFocusBridge::moveFocus(FocusDirection /*direction*/) {}

void IOSFocusBridge::setTabIndex(const std::string& nodeId, int tabIndex) {
    tabIndices_[nodeId] = tabIndex;
}

void IOSFocusBridge::createFocusTrap(const std::string& /*scopeId*/) {}
void IOSFocusBridge::releaseFocusTrap(const std::string& /*scopeId*/) {}

std::string IOSFocusBridge::getFocusedNodeId() const {
    return focusedNodeId_;
}

std::string IOSFocusBridge::generateFocusState() const {
    std::ostringstream swift;
    swift << "enum FocusedField: Hashable {\n";
    for (const auto& [nodeId, idx] : tabIndices_) {
        swift << "    case " << nodeId << "\n";
    }
    swift << "}\n\n";
    swift << "@FocusState var focusedField: FocusedField?\n";
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSImageCacheBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool IOSImageCacheBridge::loadImage(const std::string& /*path*/,
                                     std::function<void(const ImageData&)> callback) {
    if (callback) {
        ImageData emptyData;
        callback(emptyData);
    }
    return true;
}

void* IOSImageCacheBridge::toNativeTexture(const ImageData& /*data*/) {
    return nullptr;  // UIImage/CGImage in Objective-C++
}

void IOSImageCacheBridge::releaseNativeTexture(void* /*texture*/) {}
void IOSImageCacheBridge::enableDiskCache(const std::string& /*cacheDir*/, size_t /*maxSize*/) {}
void IOSImageCacheBridge::clearCache() {}
void IOSImageCacheBridge::prefetchImages(const std::vector<std::string>& /*urls*/) {}

std::string IOSImageCacheBridge::generateAsyncImage(const std::string& url) const {
    std::ostringstream swift;
    swift << "AsyncImage(url: URL(string: \"" << url << "\")) { image in\n";
    swift << "    image.resizable().aspectRatio(contentMode: .fit)\n";
    swift << "} placeholder: {\n";
    swift << "    ProgressView()\n";
    swift << "}\n";
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSNotificationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void IOSNotificationBridge::showNotification(const NotificationData& data) {
    activeNotifications_.push_back(data);
}

void IOSNotificationBridge::dismissNotification(const std::string& id) {
    activeNotifications_.erase(
        std::remove_if(activeNotifications_.begin(), activeNotifications_.end(),
                        [&id](const NotificationData& n) { return n.id == id; }),
        activeNotifications_.end());
}

void IOSNotificationBridge::dismissAll() {
    activeNotifications_.clear();
}

void IOSNotificationBridge::requestPermission(std::function<void(bool)> callback) {
    if (callback) callback(permissionGranted_);
}

bool IOSNotificationBridge::hasPermission() const {
    return permissionGranted_;
}

std::string IOSNotificationBridge::generateNotificationCode(const NotificationData& data) const {
    std::ostringstream swift;
    swift << "let content = UNMutableNotificationContent()\n";
    swift << "content.title = \"" << data.title << "\"\n";
    swift << "content.body = \"" << data.message << "\"\n";
    switch (data.priority) {
        case NotificationPriority::Urgent:
            swift << "content.interruptionLevel = .critical\n"; break;
        case NotificationPriority::High:
            swift << "content.interruptionLevel = .timeSensitive\n"; break;
        default:
            swift << "content.interruptionLevel = .active\n"; break;
    }
    swift << "let request = UNNotificationRequest(identifier: \""
          << data.id << "\", content: content, trigger: nil)\n";
    swift << "UNUserNotificationCenter.current().add(request)\n";
    return swift.str();
}

std::string IOSNotificationBridge::generateSwiftUIAlert(const NotificationData& data) const {
    std::ostringstream swift;
    swift << ".alert(\"" << data.title << "\", isPresented: $showAlert_"
          << data.id << ") {\n";
    for (const auto& action : data.actions) {
        swift << "    Button(\"" << action.label << "\") { /* عمل */ }\n";
    }
    swift << "    Button(\"إلغاء\", role: .cancel) { }\n";
    swift << "} message: {\n";
    swift << "    Text(\"" << data.message << "\")\n";
    swift << "}\n";
    return swift.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSSystemBridge
// ═══════════════════════════════════════════════════════════════════════════════

IOSSystemBridge::IOSSystemBridge() = default;
IOSSystemBridge::~IOSSystemBridge() {
    if (ready_) shutdown();
}

PlatformInfo IOSSystemBridge::getPlatformInfo() const {
    PlatformInfo info;
    info.type = PlatformType::iOS;
    info.name = "iOS (SwiftUI)";
    info.version = "1.0.0";
    info.scaleFactor = 2.0f;  // Retina
    info.supportsTouch = true;
    info.supportsMouse = false;
    info.supportsKeyboard = false;
    info.supportsHaptic = true;
    info.supportsForceTap = true;
    info.isRTL = true;
    return info;
}

bool IOSSystemBridge::initialize() {
    ready_ = true;
    theme_.applyColorScheme(ColorScheme::light());
    return true;
}

void IOSSystemBridge::shutdown() {
    notification_.dismissAll();
    ready_ = false;
}

bool IOSSystemBridge::isReady() const {
    return ready_;
}

} // namespace ios
} // namespace ui
} // namespace sad
