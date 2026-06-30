/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.cpp
 * المسار: features/graphics/backends/web/src/system_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ جسر الويب — يولّد CSS/JS/HTML لربط الأنظمة الثمانية.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/system_bridge.h"

#include <algorithm>
#include <cstdio>
#include <iomanip>

namespace sad {
namespace ui {
namespace web {

// ═══════════════════════════════════════════════════════════════════════════════
// WebThemeBridge
// ═══════════════════════════════════════════════════════════════════════════════

void WebThemeBridge::applyColorScheme(const ColorScheme& scheme) {
    currentScheme_ = scheme;
}

void WebThemeBridge::applyTextTheme(const TextTheme& textTheme) {
    currentTextTheme_ = textTheme;
}

void WebThemeBridge::setDarkMode(bool dark) {
    darkMode_ = dark;
    if (dark) {
        currentScheme_ = ColorScheme::dark();
    } else {
        currentScheme_ = ColorScheme::light();
    }
}

bool WebThemeBridge::isSystemDarkMode() const {
    // في الويب: window.matchMedia('(prefers-color-scheme: dark)').matches
    return darkMode_;
}

uint32_t WebThemeBridge::toNativeColor(const ThemeColor& color) const {
    uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
    uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void WebThemeBridge::applyTheme(const ThemeProvider& provider) {
    applyColorScheme(provider.colorScheme());
    applyTextTheme(provider.textTheme());
}

std::string WebThemeBridge::toCSSColor(const ThemeColor& color) const {
    int r = static_cast<int>(color.r * 255.0f);
    int g = static_cast<int>(color.g * 255.0f);
    int b = static_cast<int>(color.b * 255.0f);
    char buf[64];
    if (color.a < 1.0f) {
        std::snprintf(buf, sizeof(buf), "rgba(%d, %d, %d, %.2f)", r, g, b, color.a);
    } else {
        std::snprintf(buf, sizeof(buf), "rgb(%d, %d, %d)", r, g, b);
    }
    return buf;
}

std::string WebThemeBridge::generateCSSVariables() const {
    std::ostringstream css;
    css << ":root {\n";
    css << "  --sad-primary: " << toCSSColor(currentScheme_.primary) << ";\n";
    css << "  --sad-on-primary: " << toCSSColor(currentScheme_.onPrimary) << ";\n";
    css << "  --sad-secondary: " << toCSSColor(currentScheme_.secondary) << ";\n";
    css << "  --sad-on-secondary: " << toCSSColor(currentScheme_.onSecondary) << ";\n";
    css << "  --sad-background: " << toCSSColor(currentScheme_.background) << ";\n";
    css << "  --sad-on-background: " << toCSSColor(currentScheme_.onBackground) << ";\n";
    css << "  --sad-surface: " << toCSSColor(currentScheme_.surface) << ";\n";
    css << "  --sad-on-surface: " << toCSSColor(currentScheme_.onSurface) << ";\n";
    css << "  --sad-error: " << toCSSColor(currentScheme_.error) << ";\n";
    css << "  --sad-on-error: " << toCSSColor(currentScheme_.onError) << ";\n";
    css << "  --sad-outline: " << toCSSColor(currentScheme_.outline) << ";\n";
    css << "  --sad-success: " << toCSSColor(currentScheme_.success) << ";\n";
    css << "  --sad-warning: " << toCSSColor(currentScheme_.warning) << ";\n";
    css << "}\n";
    return css.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebGestureBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool WebGestureBridge::translateNativeEvent(void* /*nativeEvent*/, GestureEvent& /*outEvent*/) {
    // في الويب: التحويل يتم في JavaScript — هذا placeholder
    return false;
}

void WebGestureBridge::registerDetector(const std::string& nodeId,
                                         std::shared_ptr<GestureDetector> detector) {
    detectors_[nodeId] = std::move(detector);
}

void WebGestureBridge::unregisterDetector(const std::string& nodeId) {
    detectors_.erase(nodeId);
}

void WebGestureBridge::setGestureEnabled(GestureType /*type*/, bool /*enabled*/) {
    // معالج في JavaScript
}

bool WebGestureBridge::supportsGesture(GestureType type) const {
    // الويب يدعم جميع الإيماءات عبر PointerEvent API
    switch (type) {
        case GestureType::Tap:
        case GestureType::DoubleTap:
        case GestureType::LongPress:
        case GestureType::Pan:
        case GestureType::HorizontalDrag:
        case GestureType::VerticalDrag:
        case GestureType::Scale:
        case GestureType::Rotation:
        case GestureType::Fling:
            return true;
        case GestureType::ForceTap:
        case GestureType::EdgeDrag:
            return false;
        default:
            return false;
    }
}

std::string WebGestureBridge::generateEventHandlers(const std::string& elementId) const {
    std::ostringstream js;
    js << "const el_" << elementId << " = document.getElementById('" << elementId << "');\n";
    js << "if (el_" << elementId << ") {\n";
    js << "  el_" << elementId << ".addEventListener('pointerdown', (e) => {\n";
    js << "    sadGesture.handlePointerDown('" << elementId << "', e);\n";
    js << "  });\n";
    js << "  el_" << elementId << ".addEventListener('pointermove', (e) => {\n";
    js << "    sadGesture.handlePointerMove('" << elementId << "', e);\n";
    js << "  });\n";
    js << "  el_" << elementId << ".addEventListener('pointerup', (e) => {\n";
    js << "    sadGesture.handlePointerUp('" << elementId << "', e);\n";
    js << "  });\n";
    js << "}\n";
    return js.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebScrollPhysicsBridge
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<ScrollPhysics> WebScrollPhysicsBridge::getPlatformScrollPhysics() const {
    // الويب يستخدم سلوك المتصفح الافتراضي — BouncingScrollPhysics
    return std::make_shared<BouncingScrollPhysics>();
}

void WebScrollPhysicsBridge::applyNativeScroll(const std::string& /*nodeId*/,
                                                const ScrollMetrics& /*metrics*/) {
    // يعتمد على CSS overflow: auto/scroll
}

void WebScrollPhysicsBridge::animateScrollTo(const std::string& /*nodeId*/,
                                              float /*targetOffset*/,
                                              float /*durationMs*/) {
    // يعتمد على element.scrollTo({ behavior: 'smooth' })
}

bool WebScrollPhysicsBridge::translateScrollEvent(void* /*nativeEvent*/,
                                                   ScrollMetrics& /*outMetrics*/) {
    return false;  // معالج في JavaScript
}

std::string WebScrollPhysicsBridge::generateScrollCSS(const std::string& elementId) const {
    return "#" + elementId + " { overflow: auto; scroll-behavior: smooth; "
           "-webkit-overflow-scrolling: touch; }\n";
}

std::string WebScrollPhysicsBridge::generateScrollJS(const std::string& elementId,
                                                      float offset, bool smooth) const {
    std::ostringstream js;
    js << "document.getElementById('" << elementId << "')?.scrollTo({"
       << " top: " << offset
       << ", behavior: '" << (smooth ? "smooth" : "instant") << "'"
       << " });\n";
    return js.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebAccessibilityBridge
// ═══════════════════════════════════════════════════════════════════════════════

void WebAccessibilityBridge::applySemanticTree(const SemanticTree& tree) {
    nodeCache_.clear();
    for (const auto& node : tree.getNodes()) {
        nodeCache_[node.id] = node;
    }
}

void WebAccessibilityBridge::updateSemanticNode(const SemanticNode& node) {
    nodeCache_[node.id] = node;
}

void WebAccessibilityBridge::announce(const std::string& /*message*/,
                                       LiveRegion /*priority*/) {
    // في الويب: نستخدم aria-live region
    // التنفيذ يتم في JavaScript عبر إضافة عنصر aria-live
}

void WebAccessibilityBridge::setNodeLabel(const std::string& nodeId,
                                           const std::string& label) {
    nodeCache_[nodeId].label = label;
}

void WebAccessibilityBridge::setNodeRole(const std::string& nodeId,
                                          SemanticRole role) {
    nodeCache_[nodeId].role = role;
}

bool WebAccessibilityBridge::isScreenReaderActive() const {
    // لا يمكن كشف ذلك في الويب بشكل موثوق
    return false;
}

std::string WebAccessibilityBridge::roleToARIA(SemanticRole role) {
    switch (role) {
        case SemanticRole::Button:       return "button";
        case SemanticRole::Link:         return "link";
        case SemanticRole::Image:        return "img";
        case SemanticRole::Heading:      return "heading";
        case SemanticRole::TextField:    return "textbox";
        case SemanticRole::Checkbox:     return "checkbox";
        case SemanticRole::RadioButton:  return "radio";
        case SemanticRole::Slider:       return "slider";
        case SemanticRole::Tab:          return "tab";
        case SemanticRole::TabPanel:     return "tabpanel";
        case SemanticRole::List:         return "list";
        case SemanticRole::ListItem:     return "listitem";
        case SemanticRole::Dialog:       return "dialog";
        case SemanticRole::Alert:        return "alert";
        case SemanticRole::Menu:         return "menu";
        case SemanticRole::MenuItem:     return "menuitem";
        case SemanticRole::Navigation:   return "navigation";
        case SemanticRole::Search:       return "search";
        case SemanticRole::ProgressBar:  return "progressbar";
        case SemanticRole::ScrollBar:    return "scrollbar";
        case SemanticRole::Switch:       return "switch";
        case SemanticRole::Table:        return "table";
        case SemanticRole::Row:          return "row";
        case SemanticRole::Cell:         return "cell";
        case SemanticRole::Tooltip:      return "tooltip";
        default:                         return "generic";
    }
}

std::string WebAccessibilityBridge::generateARIAAttributes(const std::string& nodeId) const {
    auto it = nodeCache_.find(nodeId);
    if (it == nodeCache_.end()) return "";

    const auto& node = it->second;
    std::ostringstream attrs;
    attrs << " role=\"" << roleToARIA(node.role) << "\"";
    if (!node.label.empty()) {
        attrs << " aria-label=\"" << node.label << "\"";
    }
    if (!node.hint.empty()) {
        attrs << " aria-describedby=\"hint_" << nodeId << "\"";
    }
    if (!node.enabled) {
        attrs << " aria-disabled=\"true\"";
    }
    if (node.liveRegion != LiveRegion::Off) {
        attrs << " aria-live=\""
              << (node.liveRegion == LiveRegion::Assertive ? "assertive" : "polite")
              << "\"";
    }
    return attrs.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebFormValidationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void WebFormValidationBridge::bindFormField(const std::string& nodeId,
                                             const FormField& field) {
    boundFields_[nodeId] = field;
}

void WebFormValidationBridge::showValidationError(const std::string& nodeId,
                                                   const std::string& errorMessage) {
    errorMessages_[nodeId] = errorMessage;
}

void WebFormValidationBridge::clearValidationError(const std::string& nodeId) {
    errorMessages_.erase(nodeId);
}

void WebFormValidationBridge::applyFormState(const FormState& state) {
    for (const auto& field : state.getFields()) {
        boundFields_[field.getName()] = field;
    }
}

void WebFormValidationBridge::enableNativeValidation(const std::string& nodeId, bool enable) {
    nativeValidation_[nodeId] = enable;
}

std::string WebFormValidationBridge::generateValidationAttrs(const std::string& nodeId) const {
    auto it = boundFields_.find(nodeId);
    if (it == boundFields_.end()) return "";

    std::ostringstream attrs;
    // HTML5 validation attributes based on field validators
    auto nativeIt = nativeValidation_.find(nodeId);
    if (nativeIt != nativeValidation_.end() && !nativeIt->second) {
        attrs << " novalidate";
    }
    return attrs.str();
}

std::string WebFormValidationBridge::generateValidationJS(const std::string& formId) const {
    std::ostringstream js;
    js << "document.getElementById('" << formId << "')?.addEventListener('submit', (e) => {\n";
    js << "  e.preventDefault();\n";
    js << "  let valid = true;\n";
    for (const auto& [nodeId, field] : boundFields_) {
        js << "  const f_" << nodeId << " = document.getElementById('" << nodeId << "');\n";
        js << "  if (f_" << nodeId << " && !f_" << nodeId << ".checkValidity()) {\n";
        js << "    valid = false;\n";
        js << "    sadForm.showError('" << nodeId << "', f_" << nodeId << ".validationMessage);\n";
        js << "  }\n";
    }
    js << "  if (valid) sadForm.onSubmit();\n";
    js << "});\n";
    return js.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebFocusBridge
// ═══════════════════════════════════════════════════════════════════════════════

void WebFocusBridge::requestFocus(const std::string& nodeId) {
    focusedNodeId_ = nodeId;
}

void WebFocusBridge::clearFocus() {
    focusedNodeId_.clear();
}

void WebFocusBridge::moveFocus(FocusDirection /*direction*/) {
    // معالج بواسطة المتصفح عبر Tab/Shift+Tab
}

void WebFocusBridge::setTabIndex(const std::string& nodeId, int tabIndex) {
    tabIndices_[nodeId] = tabIndex;
}

void WebFocusBridge::createFocusTrap(const std::string& /*scopeId*/) {
    // يتم عبر JS focus trap library
}

void WebFocusBridge::releaseFocusTrap(const std::string& /*scopeId*/) {}

std::string WebFocusBridge::getFocusedNodeId() const {
    return focusedNodeId_;
}

std::string WebFocusBridge::generateFocusJS() const {
    std::ostringstream js;
    if (!focusedNodeId_.empty()) {
        js << "document.getElementById('" << focusedNodeId_ << "')?.focus();\n";
    }
    return js.str();
}

std::string WebFocusBridge::generateTabIndex(const std::string& nodeId) const {
    auto it = tabIndices_.find(nodeId);
    if (it != tabIndices_.end()) {
        return " tabindex=\"" + std::to_string(it->second) + "\"";
    }
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebImageCacheBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool WebImageCacheBridge::loadImage(const std::string& /*path*/,
                                     std::function<void(const ImageData&)> callback) {
    // في الويب: نستخدم new Image() في JavaScript
    if (callback) {
        ImageData emptyData;
        callback(emptyData);
    }
    return true;
}

void* WebImageCacheBridge::toNativeTexture(const ImageData& /*data*/) {
    return nullptr;  // غير مطبق في الويب — نستخدم HTML <img> أو Canvas
}

void WebImageCacheBridge::releaseNativeTexture(void* /*texture*/) {}

void WebImageCacheBridge::enableDiskCache(const std::string& /*cacheDir*/, size_t /*maxSize*/) {
    // في الويب: نعتمد على HTTP cache headers
}

void WebImageCacheBridge::clearCache() {
    // في الويب: لا يمكن مسح cache المتصفح برمجياً
}

void WebImageCacheBridge::prefetchImages(const std::vector<std::string>& /*urls*/) {
    // يُنفّذ عبر <link rel="preload"> في HTML
}

std::string WebImageCacheBridge::generatePreloadJS(const std::vector<std::string>& urls) const {
    std::ostringstream js;
    js << "const sadImageCache = {};\n";
    for (const auto& url : urls) {
        js << "{\n  const img = new Image();\n  img.src = '" << url << "';\n";
        js << "  sadImageCache['" << url << "'] = img;\n}\n";
    }
    return js.str();
}

std::string WebImageCacheBridge::generatePreloadHTML(const std::vector<std::string>& urls) const {
    std::ostringstream html;
    for (const auto& url : urls) {
        html << "<link rel=\"preload\" as=\"image\" href=\"" << url << "\">\n";
    }
    return html.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebNotificationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void WebNotificationBridge::showNotification(const NotificationData& data) {
    activeNotifications_.push_back(data);
}

void WebNotificationBridge::dismissNotification(const std::string& id) {
    activeNotifications_.erase(
        std::remove_if(activeNotifications_.begin(), activeNotifications_.end(),
                        [&id](const NotificationData& n) { return n.id == id; }),
        activeNotifications_.end());
}

void WebNotificationBridge::dismissAll() {
    activeNotifications_.clear();
}

void WebNotificationBridge::requestPermission(std::function<void(bool)> callback) {
    // يتم في JavaScript: Notification.requestPermission()
    if (callback) callback(permissionGranted_);
}

bool WebNotificationBridge::hasPermission() const {
    return permissionGranted_;
}

std::string WebNotificationBridge::generateNotificationJS(const NotificationData& data) const {
    std::ostringstream js;
    js << "if (Notification.permission === 'granted') {\n";
    js << "  new Notification('" << data.title << "', {\n";
    js << "    body: '" << data.message << "',\n";
    js << "    icon: '" << data.icon << "',\n";
    js << "    dir: 'rtl',\n";
    js << "    tag: '" << data.id << "'\n";
    js << "  });\n";
    js << "}\n";
    return js.str();
}

std::string WebNotificationBridge::generateToastCSS() const {
    return R"CSS(
.sad-toast-container {
    position: fixed;
    z-index: 10000;
    display: flex;
    flex-direction: column;
    gap: 8px;
    padding: 16px;
    direction: rtl;
}
.sad-toast-container.top-right { top: 0; right: 0; }
.sad-toast-container.top-left { top: 0; left: 0; }
.sad-toast-container.bottom-right { bottom: 0; right: 0; }
.sad-toast-container.bottom-center { bottom: 0; left: 50%; transform: translateX(-50%); }
.sad-toast {
    padding: 12px 24px;
    border-radius: 8px;
    color: white;
    font-family: inherit;
    box-shadow: 0 4px 12px rgba(0,0,0,0.3);
    animation: sadToastIn 0.3s ease-out;
}
.sad-toast.info { background: var(--sad-primary); }
.sad-toast.success { background: var(--sad-success); }
.sad-toast.warning { background: var(--sad-warning); }
.sad-toast.error { background: var(--sad-error); }
@keyframes sadToastIn {
    from { opacity: 0; transform: translateY(-20px); }
    to { opacity: 1; transform: translateY(0); }
}
)CSS";
}

std::string WebNotificationBridge::generateToastHTML(const NotificationData& data) const {
    std::string typeClass;
    switch (data.type) {
        case NotificationType::Toast:   typeClass = "info"; break;
        case NotificationType::Snackbar: typeClass = "info"; break;
        case NotificationType::Banner:  typeClass = "warning"; break;
        case NotificationType::Dialog:  typeClass = "info"; break;
        case NotificationType::Push:    typeClass = "info"; break;
    }

    std::ostringstream html;
    html << "<div class=\"sad-toast " << typeClass << "\" id=\"toast_" << data.id << "\">\n";
    if (!data.title.empty()) {
        html << "  <strong>" << data.title << "</strong><br>\n";
    }
    html << "  <span>" << data.message << "</span>\n";
    for (const auto& action : data.actions) {
        html << "  <button onclick=\"sadNotify.action('" << data.id << "', '"
             << action.label << "')\">" << action.label << "</button>\n";
    }
    html << "</div>\n";
    return html.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// WebSystemBridge
// ═══════════════════════════════════════════════════════════════════════════════

WebSystemBridge::WebSystemBridge() = default;
WebSystemBridge::~WebSystemBridge() {
    if (ready_) shutdown();
}

PlatformInfo WebSystemBridge::getPlatformInfo() const {
    PlatformInfo info;
    info.type = PlatformType::Web;
    info.name = "Web (HTML5/CSS/JS)";
    info.version = "1.0.0";
    info.scaleFactor = 1.0f;
    info.supportsTouch = true;
    info.supportsMouse = true;
    info.supportsKeyboard = true;
    info.supportsHaptic = false;
    info.supportsForceTap = false;
    info.isRTL = true;
    return info;
}

bool WebSystemBridge::initialize() {
    ready_ = true;
    theme_.applyColorScheme(ColorScheme::light());
    return true;
}

void WebSystemBridge::shutdown() {
    notification_.dismissAll();
    ready_ = false;
}

bool WebSystemBridge::isReady() const {
    return ready_;
}

} // namespace web
} // namespace ui
} // namespace sad
