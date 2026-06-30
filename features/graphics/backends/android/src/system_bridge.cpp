/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.cpp
 * المسار: features/graphics/backends/android/src/system_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ جسر أندرويد — يولّد Kotlin/Compose لربط الأنظمة الثمانية.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/android/system_bridge.h"

#include <algorithm>
#include <cstdio>
#include <iomanip>

namespace sad {
namespace ui {
namespace android {

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidThemeBridge
// ═══════════════════════════════════════════════════════════════════════════════

void AndroidThemeBridge::applyColorScheme(const ColorScheme& scheme) {
    currentScheme_ = scheme;
}

void AndroidThemeBridge::applyTextTheme(const TextTheme& textTheme) {
    currentTextTheme_ = textTheme;
}

void AndroidThemeBridge::setDarkMode(bool dark) {
    darkMode_ = dark;
    currentScheme_ = dark ? ColorScheme::dark() : ColorScheme::light();
}

bool AndroidThemeBridge::isSystemDarkMode() const {
    return darkMode_;
}

uint32_t AndroidThemeBridge::toNativeColor(const ThemeColor& color) const {
    uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
    uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
    return (a << 24) | (r << 16) | (g << 8) | b;  // Android ARGB format
}

void AndroidThemeBridge::applyTheme(const ThemeProvider& provider) {
    applyColorScheme(provider.colorScheme());
    applyTextTheme(provider.textTheme());
}

std::string AndroidThemeBridge::toComposeColor(const ThemeColor& color) const {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "Color(0x%02X%02X%02X%02X)",
                  static_cast<int>(color.a * 255),
                  static_cast<int>(color.r * 255),
                  static_cast<int>(color.g * 255),
                  static_cast<int>(color.b * 255));
    return buf;
}

std::string AndroidThemeBridge::generateMaterialTheme() const {
    std::ostringstream kt;
    kt << "@Composable\n";
    kt << "fun SadTheme(content: @Composable () -> Unit) {\n";
    kt << "    val colorScheme = " << (darkMode_ ? "darkColorScheme" : "lightColorScheme") << "(\n";
    kt << "        primary = " << toComposeColor(currentScheme_.primary) << ",\n";
    kt << "        onPrimary = " << toComposeColor(currentScheme_.onPrimary) << ",\n";
    kt << "        secondary = " << toComposeColor(currentScheme_.secondary) << ",\n";
    kt << "        onSecondary = " << toComposeColor(currentScheme_.onSecondary) << ",\n";
    kt << "        background = " << toComposeColor(currentScheme_.background) << ",\n";
    kt << "        onBackground = " << toComposeColor(currentScheme_.onBackground) << ",\n";
    kt << "        surface = " << toComposeColor(currentScheme_.surface) << ",\n";
    kt << "        onSurface = " << toComposeColor(currentScheme_.onSurface) << ",\n";
    kt << "        error = " << toComposeColor(currentScheme_.error) << ",\n";
    kt << "        onError = " << toComposeColor(currentScheme_.onError) << "\n";
    kt << "    )\n";
    kt << "    MaterialTheme(colorScheme = colorScheme, content = content)\n";
    kt << "}\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidGestureBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool AndroidGestureBridge::translateNativeEvent(void* /*nativeEvent*/, GestureEvent& /*outEvent*/) {
    return false;  // معالج بواسطة Compose
}

void AndroidGestureBridge::registerDetector(const std::string& nodeId,
                                             std::shared_ptr<GestureDetector> detector) {
    detectors_[nodeId] = std::move(detector);
}

void AndroidGestureBridge::unregisterDetector(const std::string& nodeId) {
    detectors_.erase(nodeId);
}

void AndroidGestureBridge::setGestureEnabled(GestureType /*type*/, bool /*enabled*/) {}

bool AndroidGestureBridge::supportsGesture(GestureType type) const {
    // أندرويد يدعم جميع الإيماءات
    (void)type;
    return true;
}

std::string AndroidGestureBridge::generateGestureModifier(const std::string& /*elementId*/) const {
    std::ostringstream kt;
    kt << "Modifier.pointerInput(Unit) {\n";
    kt << "    detectTapGestures(\n";
    kt << "        onTap = { offset -> sadGesture.onTap(offset) },\n";
    kt << "        onDoubleTap = { offset -> sadGesture.onDoubleTap(offset) },\n";
    kt << "        onLongPress = { offset -> sadGesture.onLongPress(offset) }\n";
    kt << "    )\n";
    kt << "}.pointerInput(Unit) {\n";
    kt << "    detectDragGestures { change, dragAmount ->\n";
    kt << "        change.consume()\n";
    kt << "        sadGesture.onDrag(dragAmount)\n";
    kt << "    }\n";
    kt << "}.pointerInput(Unit) {\n";
    kt << "    detectTransformGestures { _, pan, zoom, rotation ->\n";
    kt << "        sadGesture.onTransform(pan, zoom, rotation)\n";
    kt << "    }\n";
    kt << "}\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidScrollPhysicsBridge
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<ScrollPhysics> AndroidScrollPhysicsBridge::getPlatformScrollPhysics() const {
    // أندرويد يستخدم ClampingScrollPhysics (overscroll glow)
    return std::make_shared<ClampingScrollPhysics>();
}

void AndroidScrollPhysicsBridge::applyNativeScroll(const std::string& /*nodeId*/,
                                                    const ScrollMetrics& /*metrics*/) {}

void AndroidScrollPhysicsBridge::animateScrollTo(const std::string& /*nodeId*/,
                                                  float /*targetOffset*/,
                                                  float /*durationMs*/) {}

bool AndroidScrollPhysicsBridge::translateScrollEvent(void* /*nativeEvent*/,
                                                       ScrollMetrics& /*outMetrics*/) {
    return false;
}

std::string AndroidScrollPhysicsBridge::generateScrollModifier(const std::string& /*elementId*/) const {
    return "Modifier.verticalScroll(rememberScrollState())";
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidAccessibilityBridge
// ═══════════════════════════════════════════════════════════════════════════════

void AndroidAccessibilityBridge::applySemanticTree(const SemanticTree& tree) {
    nodeCache_.clear();
    for (const auto& node : tree.getNodes()) {
        nodeCache_[node.id] = node;
    }
}

void AndroidAccessibilityBridge::updateSemanticNode(const SemanticNode& node) {
    nodeCache_[node.id] = node;
}

void AndroidAccessibilityBridge::announce(const std::string& /*message*/,
                                           LiveRegion /*priority*/) {
    // Android: AccessibilityManager.announce()
}

void AndroidAccessibilityBridge::setNodeLabel(const std::string& nodeId,
                                               const std::string& label) {
    nodeCache_[nodeId].label = label;
}

void AndroidAccessibilityBridge::setNodeRole(const std::string& nodeId,
                                              SemanticRole role) {
    nodeCache_[nodeId].role = role;
}

bool AndroidAccessibilityBridge::isScreenReaderActive() const {
    return false;  // يتطلب JNI على Android
}

std::string AndroidAccessibilityBridge::roleToAndroid(SemanticRole role) {
    switch (role) {
        case SemanticRole::Button:      return "Role.Button";
        case SemanticRole::Image:       return "Role.Image";
        case SemanticRole::Checkbox:    return "Role.Checkbox";
        case SemanticRole::RadioButton: return "Role.RadioButton";
        case SemanticRole::Switch:      return "Role.Switch";
        case SemanticRole::Tab:         return "Role.Tab";
        case SemanticRole::Slider:      return "Role.Slider";
        case SemanticRole::ProgressBar: return "Role.ProgressIndicator";
        default:                        return "Role.Button";
    }
}

std::string AndroidAccessibilityBridge::generateSemanticsModifier(const std::string& nodeId) const {
    auto it = nodeCache_.find(nodeId);
    if (it == nodeCache_.end()) return "";

    const auto& node = it->second;
    std::ostringstream kt;
    kt << "Modifier.semantics {\n";
    if (!node.label.empty()) {
        kt << "    contentDescription = \"" << node.label << "\"\n";
    }
    kt << "    role = " << roleToAndroid(node.role) << "\n";
    if (!node.enabled) {
        kt << "    disabled()\n";
    }
    if (node.liveRegion != LiveRegion::Off) {
        kt << "    liveRegion = LiveRegionMode."
           << (node.liveRegion == LiveRegion::Assertive ? "Assertive" : "Polite")
           << "\n";
    }
    kt << "}\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidFormValidationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void AndroidFormValidationBridge::bindFormField(const std::string& nodeId,
                                                 const FormField& field) {
    boundFields_[nodeId] = field;
}

void AndroidFormValidationBridge::showValidationError(const std::string& nodeId,
                                                       const std::string& errorMessage) {
    errorMessages_[nodeId] = errorMessage;
}

void AndroidFormValidationBridge::clearValidationError(const std::string& nodeId) {
    errorMessages_.erase(nodeId);
}

void AndroidFormValidationBridge::applyFormState(const FormState& state) {
    for (const auto& field : state.getFields()) {
        boundFields_[field.getName()] = field;
    }
}

void AndroidFormValidationBridge::enableNativeValidation(const std::string& /*nodeId*/,
                                                          bool /*enable*/) {}

std::string AndroidFormValidationBridge::generateValidatedTextField(const std::string& nodeId) const {
    auto fieldIt = boundFields_.find(nodeId);
    auto errorIt = errorMessages_.find(nodeId);

    std::ostringstream kt;
    kt << "OutlinedTextField(\n";
    kt << "    value = state_" << nodeId << ",\n";
    kt << "    onValueChange = { state_" << nodeId << " = it },\n";
    if (fieldIt != boundFields_.end()) {
        kt << "    label = { Text(\"" << fieldIt->second.getName() << "\") },\n";
    }
    if (errorIt != errorMessages_.end()) {
        kt << "    isError = true,\n";
        kt << "    supportingText = { Text(\"" << errorIt->second << "\") },\n";
    }
    kt << ")\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidFocusBridge
// ═══════════════════════════════════════════════════════════════════════════════

void AndroidFocusBridge::requestFocus(const std::string& nodeId) {
    focusedNodeId_ = nodeId;
}

void AndroidFocusBridge::clearFocus() {
    focusedNodeId_.clear();
}

void AndroidFocusBridge::moveFocus(FocusDirection direction) {
    (void)direction;  // Compose FocusManager handles this
}

void AndroidFocusBridge::setTabIndex(const std::string& nodeId, int tabIndex) {
    tabIndices_[nodeId] = tabIndex;
}

void AndroidFocusBridge::createFocusTrap(const std::string& /*scopeId*/) {}
void AndroidFocusBridge::releaseFocusTrap(const std::string& /*scopeId*/) {}

std::string AndroidFocusBridge::getFocusedNodeId() const {
    return focusedNodeId_;
}

std::string AndroidFocusBridge::generateFocusModifier(const std::string& nodeId) const {
    std::ostringstream kt;
    kt << "val focusRequester_" << nodeId << " = remember { FocusRequester() }\n";
    kt << "Modifier.focusRequester(focusRequester_" << nodeId << ")";
    auto it = tabIndices_.find(nodeId);
    if (it != tabIndices_.end()) {
        kt << ".focusOrder { next = focusRequester_next }";
    }
    kt << "\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidImageCacheBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool AndroidImageCacheBridge::loadImage(const std::string& /*path*/,
                                         std::function<void(const ImageData&)> callback) {
    if (callback) {
        ImageData emptyData;
        callback(emptyData);
    }
    return true;
}

void* AndroidImageCacheBridge::toNativeTexture(const ImageData& /*data*/) {
    return nullptr;  // يتم عبر OpenGL ES أو Compose
}

void AndroidImageCacheBridge::releaseNativeTexture(void* /*texture*/) {}

void AndroidImageCacheBridge::enableDiskCache(const std::string& /*cacheDir*/,
                                               size_t /*maxSize*/) {}

void AndroidImageCacheBridge::clearCache() {}

void AndroidImageCacheBridge::prefetchImages(const std::vector<std::string>& /*urls*/) {}

std::string AndroidImageCacheBridge::generateAsyncImage(const std::string& url,
                                                         const std::string& modifier) const {
    std::ostringstream kt;
    kt << "AsyncImage(\n";
    kt << "    model = \"" << url << "\",\n";
    kt << "    contentDescription = null,\n";
    kt << "    modifier = " << modifier << ",\n";
    kt << "    contentScale = ContentScale.Fit\n";
    kt << ")\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidNotificationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void AndroidNotificationBridge::showNotification(const NotificationData& data) {
    activeNotifications_.push_back(data);
}

void AndroidNotificationBridge::dismissNotification(const std::string& id) {
    activeNotifications_.erase(
        std::remove_if(activeNotifications_.begin(), activeNotifications_.end(),
                        [&id](const NotificationData& n) { return n.id == id; }),
        activeNotifications_.end());
}

void AndroidNotificationBridge::dismissAll() {
    activeNotifications_.clear();
}

void AndroidNotificationBridge::requestPermission(std::function<void(bool)> callback) {
    if (callback) callback(permissionGranted_);
}

bool AndroidNotificationBridge::hasPermission() const {
    return permissionGranted_;
}

void AndroidNotificationBridge::setNotificationChannel(const std::string& channelId,
                                                         const std::string& channelName) {
    channelId_ = channelId;
    channelName_ = channelName;
}

std::string AndroidNotificationBridge::generateNotificationCode(const NotificationData& data) const {
    std::ostringstream kt;
    kt << "val builder = NotificationCompat.Builder(context, \"" << channelId_ << "\")\n";
    kt << "    .setSmallIcon(R.drawable.ic_notification)\n";
    kt << "    .setContentTitle(\"" << data.title << "\")\n";
    kt << "    .setContentText(\"" << data.message << "\")\n";

    switch (data.priority) {
        case NotificationPriority::Low:
            kt << "    .setPriority(NotificationCompat.PRIORITY_LOW)\n"; break;
        case NotificationPriority::Normal:
            kt << "    .setPriority(NotificationCompat.PRIORITY_DEFAULT)\n"; break;
        case NotificationPriority::High:
            kt << "    .setPriority(NotificationCompat.PRIORITY_HIGH)\n"; break;
        case NotificationPriority::Urgent:
            kt << "    .setPriority(NotificationCompat.PRIORITY_MAX)\n"; break;
    }

    for (const auto& action : data.actions) {
        kt << "    .addAction(0, \"" << action.label << "\", pendingIntent_"
           << action.label << ")\n";
    }

    kt << "NotificationManagerCompat.from(context).notify("
       << "\"" << data.id << "\".hashCode(), builder.build())\n";
    return kt.str();
}

std::string AndroidNotificationBridge::generateSnackbar(const NotificationData& data) const {
    std::ostringstream kt;
    kt << "Snackbar.make(view, \"" << data.message << "\", ";
    if (data.duration > 5000) {
        kt << "Snackbar.LENGTH_LONG";
    } else {
        kt << "Snackbar.LENGTH_SHORT";
    }
    kt << ")";
    if (!data.actions.empty()) {
        kt << "\n    .setAction(\"" << data.actions[0].label << "\") { /* action */ }";
    }
    kt << "\n    .show()\n";
    return kt.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// AndroidSystemBridge
// ═══════════════════════════════════════════════════════════════════════════════

AndroidSystemBridge::AndroidSystemBridge() = default;
AndroidSystemBridge::~AndroidSystemBridge() {
    if (ready_) shutdown();
}

PlatformInfo AndroidSystemBridge::getPlatformInfo() const {
    PlatformInfo info;
    info.type = PlatformType::Android;
    info.name = "Android (Compose)";
    info.version = "1.0.0";
    info.scaleFactor = 1.0f;
    info.supportsTouch = true;
    info.supportsMouse = false;
    info.supportsKeyboard = false;
    info.supportsHaptic = true;
    info.supportsForceTap = false;
    info.isRTL = true;
    return info;
}

bool AndroidSystemBridge::initialize() {
    ready_ = true;
    theme_.applyColorScheme(ColorScheme::light());
    return true;
}

void AndroidSystemBridge::shutdown() {
    notification_.dismissAll();
    ready_ = false;
}

bool AndroidSystemBridge::isReady() const {
    return ready_;
}

} // namespace android
} // namespace ui
} // namespace sad
