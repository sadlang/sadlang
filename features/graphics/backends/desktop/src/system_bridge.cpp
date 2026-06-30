/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.cpp
 * المسار: features/graphics/backends/desktop/src/system_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ جسر سطح المكتب — يربط الأنظمة الثمانية بـ SDL2.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/desktop/system_bridge.h"

#ifdef SAD_UI_USE_SDL2
#include <SDL.h>
#endif

#include <algorithm>
#include <cstring>

namespace sad {
namespace ui {
namespace desktop {

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopThemeBridge
// ═══════════════════════════════════════════════════════════════════════════════

void DesktopThemeBridge::applyColorScheme(const ColorScheme& scheme) {
    currentScheme_ = scheme;
}

void DesktopThemeBridge::applyTextTheme(const TextTheme& textTheme) {
    currentTextTheme_ = textTheme;
}

void DesktopThemeBridge::setDarkMode(bool dark) {
    darkMode_ = dark;
    if (dark) {
        currentScheme_ = ColorScheme::dark();
    } else {
        currentScheme_ = ColorScheme::light();
    }
}

bool DesktopThemeBridge::isSystemDarkMode() const {
    // على سطح المكتب: نتحقق من إعدادات النظام
#ifdef _WIN32
    // Windows: قراءة من Registry (AppsUseLightTheme)
    // للتبسيط نعيد القيمة المحلية
    return darkMode_;
#elif defined(__APPLE__)
    return darkMode_;
#else
    // Linux: نتحقق من متغيرات البيئة GTK
    return darkMode_;
#endif
}

uint32_t DesktopThemeBridge::toNativeColor(const ThemeColor& color) const {
    uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
    uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void DesktopThemeBridge::applyTheme(const ThemeProvider& provider) {
    applyColorScheme(provider.colorScheme());
    applyTextTheme(provider.textTheme());
}

void DesktopThemeBridge::toSDLColor(const ThemeColor& color,
                                     uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {
    r = static_cast<uint8_t>(color.r * 255.0f);
    g = static_cast<uint8_t>(color.g * 255.0f);
    b = static_cast<uint8_t>(color.b * 255.0f);
    a = static_cast<uint8_t>(color.a * 255.0f);
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopGestureBridge
// ═══════════════════════════════════════════════════════════════════════════════

bool DesktopGestureBridge::translateNativeEvent(void* nativeEvent, GestureEvent& outEvent) {
    return processSDLEvent(nativeEvent, outEvent);
}

bool DesktopGestureBridge::processSDLEvent(const void* sdlEvent, GestureEvent& outEvent) {
#ifdef SAD_UI_USE_SDL2
    if (!sdlEvent) return false;

    const SDL_Event& event = *static_cast<const SDL_Event*>(sdlEvent);
    auto now = std::chrono::steady_clock::now();

    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            mouseDown_ = true;
            lastMousePos_ = {static_cast<float>(event.button.x),
                             static_cast<float>(event.button.y)};

            // كشف النقرة المزدوجة
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastClickTime_).count();
            float dist = lastMousePos_.distance(lastClickPos_);

            if (elapsed < 300 && dist < 10.0f) {
                clickCount_++;
            } else {
                clickCount_ = 1;
            }
            lastClickTime_ = now;
            lastClickPos_ = lastMousePos_;

            outEvent.type = (clickCount_ >= 2) ? GestureType::DoubleTap : GestureType::Tap;
            outEvent.state = GestureState::Began;
            outEvent.position = lastMousePos_;
            outEvent.startPosition = lastMousePos_;
            outEvent.pressure = 1.0f;
            return true;
        }

        case SDL_MOUSEBUTTONUP: {
            mouseDown_ = false;
            outEvent.type = GestureType::Tap;
            outEvent.state = GestureState::Ended;
            outEvent.position = {static_cast<float>(event.button.x),
                                 static_cast<float>(event.button.y)};
            return true;
        }

        case SDL_MOUSEMOTION: {
            GesturePoint current = {static_cast<float>(event.motion.x),
                                    static_cast<float>(event.motion.y)};
            if (mouseDown_) {
                outEvent.type = GestureType::Pan;
                outEvent.state = GestureState::Changed;
                outEvent.position = current;
                outEvent.delta = {static_cast<float>(event.motion.xrel),
                                  static_cast<float>(event.motion.yrel)};
                // حساب السرعة التقريبية
                outEvent.velocity.pixelsPerSecondX = event.motion.xrel * 60.0f;
                outEvent.velocity.pixelsPerSecondY = event.motion.yrel * 60.0f;
            }
            lastMousePos_ = current;
            return mouseDown_;
        }

        case SDL_MOUSEWHEEL: {
            outEvent.type = GestureType::Pan;
            outEvent.state = GestureState::Changed;
            outEvent.position = lastMousePos_;
            outEvent.delta = {static_cast<float>(event.wheel.x) * 20.0f,
                              static_cast<float>(event.wheel.y) * 20.0f};
            return true;
        }

        default:
            return false;
    }
#else
    (void)sdlEvent;
    (void)outEvent;
    return false;
#endif
}

void DesktopGestureBridge::registerDetector(const std::string& nodeId,
                                             std::shared_ptr<GestureDetector> detector) {
    detectors_[nodeId] = std::move(detector);
}

void DesktopGestureBridge::unregisterDetector(const std::string& nodeId) {
    detectors_.erase(nodeId);
}

void DesktopGestureBridge::setGestureEnabled(GestureType type, bool enabled) {
    if (enabled) {
        disabledGestures_.erase(type);
    } else {
        disabledGestures_.insert(type);
    }
}

bool DesktopGestureBridge::supportsGesture(GestureType type) const {
    // سطح المكتب يدعم: نقرة، نقرة مزدوجة، سحب، عجلة التمرير
    // لا يدعم: Scale (pinch)، Rotation، ForceTap، EdgeDrag
    switch (type) {
        case GestureType::Tap:
        case GestureType::DoubleTap:
        case GestureType::LongPress:
        case GestureType::Pan:
        case GestureType::HorizontalDrag:
        case GestureType::VerticalDrag:
        case GestureType::Fling:
            return true;
        default:
            return false;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopScrollPhysicsBridge
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<ScrollPhysics> DesktopScrollPhysicsBridge::getPlatformScrollPhysics() const {
    // سطح المكتب يستخدم ClampingScrollPhysics (بدون ارتداد)
    return std::make_shared<ClampingScrollPhysics>();
}

void DesktopScrollPhysicsBridge::applyNativeScroll(const std::string& nodeId,
                                                    const ScrollMetrics& metrics) {
    scrollStates_[nodeId] = metrics;
}

void DesktopScrollPhysicsBridge::animateScrollTo(const std::string& nodeId,
                                                  float targetOffset,
                                                  float /*durationMs*/) {
    auto it = scrollStates_.find(nodeId);
    if (it != scrollStates_.end()) {
        it->second.pixels = targetOffset;
    }
}

bool DesktopScrollPhysicsBridge::translateScrollEvent(void* nativeEvent,
                                                       ScrollMetrics& outMetrics) {
#ifdef SAD_UI_USE_SDL2
    if (!nativeEvent) return false;
    const SDL_Event& event = *static_cast<const SDL_Event*>(nativeEvent);
    if (event.type == SDL_MOUSEWHEEL) {
        outMetrics.pixels = static_cast<float>(event.wheel.y) * 40.0f;
        outMetrics.viewportDimension = 600.0f;  // قيمة افتراضية
        return true;
    }
#else
    (void)nativeEvent;
    (void)outMetrics;
#endif
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopAccessibilityBridge
// ═══════════════════════════════════════════════════════════════════════════════

void DesktopAccessibilityBridge::applySemanticTree(const SemanticTree& tree) {
    nodeCache_.clear();
    for (const auto& node : tree.getNodes()) {
        nodeCache_[node.id] = node;
    }
}

void DesktopAccessibilityBridge::updateSemanticNode(const SemanticNode& node) {
    nodeCache_[node.id] = node;
}

void DesktopAccessibilityBridge::announce(const std::string& /*message*/,
                                           LiveRegion /*priority*/) {
    // على سطح المكتب: نستخدم MSAA على Windows أو AT-SPI على Linux
    // للتبسيط: نخزن الإعلان (يمكن قراءته بواسطة المُصيّر لعرضه)
}

void DesktopAccessibilityBridge::setNodeLabel(const std::string& nodeId,
                                               const std::string& label) {
    nodeCache_[nodeId].label = label;
}

void DesktopAccessibilityBridge::setNodeRole(const std::string& nodeId,
                                              SemanticRole role) {
    nodeCache_[nodeId].role = role;
}

bool DesktopAccessibilityBridge::isScreenReaderActive() const {
#ifdef _WIN32
    // Windows: SystemParametersInfo(SPI_GETSCREENREADER)
    return false;
#else
    return false;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopFormValidationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void DesktopFormValidationBridge::bindFormField(const std::string& nodeId,
                                                 const FormField& field) {
    boundFields_[nodeId] = field;
}

void DesktopFormValidationBridge::showValidationError(const std::string& nodeId,
                                                       const std::string& errorMessage) {
    errorMessages_[nodeId] = errorMessage;
}

void DesktopFormValidationBridge::clearValidationError(const std::string& nodeId) {
    errorMessages_.erase(nodeId);
}

void DesktopFormValidationBridge::applyFormState(const FormState& state) {
    // ربط جميع حقول النموذج
    for (const auto& field : state.getFields()) {
        boundFields_[field.getName()] = field;
    }
}

void DesktopFormValidationBridge::enableNativeValidation(const std::string& /*nodeId*/,
                                                          bool /*enable*/) {
    // سطح المكتب لا يملك تحقق أصلي — كل التحقق يتم برمجياً
}

std::string DesktopFormValidationBridge::getError(const std::string& nodeId) const {
    auto it = errorMessages_.find(nodeId);
    return it != errorMessages_.end() ? it->second : "";
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopFocusBridge
// ═══════════════════════════════════════════════════════════════════════════════

void DesktopFocusBridge::requestFocus(const std::string& nodeId) {
    focusedNodeId_ = nodeId;
}

void DesktopFocusBridge::clearFocus() {
    focusedNodeId_.clear();
}

void DesktopFocusBridge::moveFocus(FocusDirection direction) {
    // البحث عن العنصر التالي حسب tabIndex
    if (tabIndices_.empty()) return;

    int currentTab = 0;
    auto it = tabIndices_.find(focusedNodeId_);
    if (it != tabIndices_.end()) {
        currentTab = it->second;
    }

    // جمع وترتيب حسب tabIndex
    std::vector<std::pair<int, std::string>> sorted;
    for (const auto& p : tabIndices_) {
        sorted.emplace_back(p.second, p.first);
    }
    std::sort(sorted.begin(), sorted.end());

    // البحث عن التالي/السابق
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (sorted[i].second == focusedNodeId_) {
            if (direction == FocusDirection::Next ||
                direction == FocusDirection::Down ||
                direction == FocusDirection::Right) {
                size_t next = (i + 1) % sorted.size();
                focusedNodeId_ = sorted[next].second;
            } else {
                size_t prev = (i == 0) ? sorted.size() - 1 : i - 1;
                focusedNodeId_ = sorted[prev].second;
            }
            return;
        }
    }

    // إذا لم يوجد تركيز حالي، ركّز على الأول
    if (!sorted.empty()) {
        focusedNodeId_ = sorted[0].second;
    }
}

void DesktopFocusBridge::setTabIndex(const std::string& nodeId, int tabIndex) {
    tabIndices_[nodeId] = tabIndex;
}

void DesktopFocusBridge::createFocusTrap(const std::string& scopeId) {
    focusTraps_.insert(scopeId);
}

void DesktopFocusBridge::releaseFocusTrap(const std::string& scopeId) {
    focusTraps_.erase(scopeId);
}

std::string DesktopFocusBridge::getFocusedNodeId() const {
    return focusedNodeId_;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopImageCacheBridge
// ═══════════════════════════════════════════════════════════════════════════════

DesktopImageCacheBridge::DesktopImageCacheBridge() = default;

DesktopImageCacheBridge::~DesktopImageCacheBridge() {
    clearCache();
}

bool DesktopImageCacheBridge::loadImage(const std::string& path,
                                         std::function<void(const ImageData&)> callback) {
    // تحميل صورة عبر SDL_LoadBMP أو stb_image
    // للتبسيط: نعيد صورة فارغة مع استدعاء callback
    ImageData data;
    // في التنفيذ الكامل: نقرأ الملف ونملأ data.pixels
    if (callback) {
        callback(data);
    }
    return true;
}

void* DesktopImageCacheBridge::toNativeTexture(const ImageData& data) {
#ifdef SAD_UI_USE_SDL2
    if (!renderer_ || data.width == 0 || data.height == 0 || data.pixels.empty())
        return nullptr;

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        const_cast<uint8_t*>(data.pixels.data()),
        data.width, data.height,
        data.channels * 8,
        data.width * data.channels,
        0x000000FF, 0x0000FF00, 0x00FF0000,
        data.channels == 4 ? 0xFF000000 : 0);

    if (!surface) return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);
    return texture;
#else
    (void)data;
    return nullptr;
#endif
}

void DesktopImageCacheBridge::releaseNativeTexture(void* texture) {
#ifdef SAD_UI_USE_SDL2
    if (texture) {
        SDL_DestroyTexture(static_cast<SDL_Texture*>(texture));
    }
#else
    (void)texture;
#endif
}

void DesktopImageCacheBridge::enableDiskCache(const std::string& cacheDir,
                                               size_t maxSize) {
    diskCacheDir_ = cacheDir;
    maxDiskSize_ = maxSize;
}

void DesktopImageCacheBridge::clearCache() {
#ifdef SAD_UI_USE_SDL2
    for (auto& [key, tex] : textureCache_) {
        if (tex) {
            SDL_DestroyTexture(static_cast<SDL_Texture*>(tex));
        }
    }
#endif
    textureCache_.clear();
}

void DesktopImageCacheBridge::prefetchImages(const std::vector<std::string>& urls) {
    for (const auto& url : urls) {
        loadImage(url, nullptr);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopNotificationBridge
// ═══════════════════════════════════════════════════════════════════════════════

void DesktopNotificationBridge::showNotification(const NotificationData& data) {
    // إضافة إلى القائمة النشطة (المُصيّر يعرضها كـ overlay)
    activeNotifications_.push_back(data);
}

void DesktopNotificationBridge::dismissNotification(const std::string& id) {
    activeNotifications_.erase(
        std::remove_if(activeNotifications_.begin(), activeNotifications_.end(),
                        [&id](const NotificationData& n) { return n.id == id; }),
        activeNotifications_.end());
}

void DesktopNotificationBridge::dismissAll() {
    activeNotifications_.clear();
}

void DesktopNotificationBridge::requestPermission(std::function<void(bool)> callback) {
    // سطح المكتب: الإذن ممنوح دائماً
    if (callback) callback(true);
}

bool DesktopNotificationBridge::hasPermission() const {
    return permissionGranted_;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DesktopSystemBridge
// ═══════════════════════════════════════════════════════════════════════════════

DesktopSystemBridge::DesktopSystemBridge() = default;
DesktopSystemBridge::~DesktopSystemBridge() {
    if (ready_) shutdown();
}

PlatformInfo DesktopSystemBridge::getPlatformInfo() const {
    PlatformInfo info;
    info.type = PlatformType::Desktop;
    info.name = "Desktop (SDL2)";
    info.version = "1.0.0";
    info.scaleFactor = 1.0f;
    info.supportsTouch = false;
    info.supportsMouse = true;
    info.supportsKeyboard = true;
    info.supportsHaptic = false;
    info.supportsForceTap = false;
    info.isRTL = true;
    return info;
}

bool DesktopSystemBridge::initialize() {
    ready_ = true;
    // تطبيق السمة الافتراضية
    theme_.applyColorScheme(ColorScheme::light());
    return true;
}

void DesktopSystemBridge::shutdown() {
    imageCache_.clearCache();
    notification_.dismissAll();
    ready_ = false;
}

bool DesktopSystemBridge::isReady() const {
    return ready_;
}

void DesktopSystemBridge::setRenderer(SDL_Renderer* renderer) {
    imageCache_.setRenderer(renderer);
}

} // namespace desktop
} // namespace ui
} // namespace sad
