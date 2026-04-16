/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: ios_renderer.mm
 * المسار: sad_ui/backends/ios/src/ios_renderer.mm
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مُصيّر iOS — يرسم مباشرة عبر CoreGraphics
 *
 * يستخدم:
 *   - CGContextRef للرسم الأساسي (مستطيلات، دوائر، خطوط)
 *   - CoreText (CTFrame/CTLine) للنصوص العربية مع دعم RTL
 *   - UIImage / CGImage للصور
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/ios/ios_renderer.h"

#ifdef SAD_IOS_AVAILABLE
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>
#import <UIKit/UIKit.h>
#endif

#include <cmath>

namespace sad {
namespace ui {
namespace ios {

// ═══════════════════════════════════════════════════════════════════════════════
// IOSRenderer
// ═══════════════════════════════════════════════════════════════════════════════

IOSRenderer::IOSRenderer() = default;
IOSRenderer::~IOSRenderer() { shutdown(); }

bool IOSRenderer::initialize(void* nativeSurface,
                              const PlatformRenderOptions& options) {
    options_ = options;
#ifdef SAD_IOS_AVAILABLE
    // nativeSurface يمكن أن يكون UIView*
    if (nativeSurface) {
        UIView* view = (__bridge UIView*)nativeSurface;
        viewWidth_ = static_cast<int>(view.bounds.size.width * iosOpts_.screenScale);
        viewHeight_ = static_cast<int>(view.bounds.size.height * iosOpts_.screenScale);
    }
#else
    (void)nativeSurface;
#endif
    initialized_ = true;
    return true;
}

void IOSRenderer::shutdown() {
    cgContext_ = nullptr;
    initialized_ = false;
}

bool IOSRenderer::isReady() const {
    return initialized_ && cgContext_ != nullptr;
}

bool IOSRenderer::loadFont(const std::string& /*fontPath*/, float /*fontSize*/) {
    return true;
}

// ── حلقة الرسم ──

void IOSRenderer::beginFrame(const Color& clearColor) {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;

    // مسح بلون الخلفية
    CGRect fullRect = CGRectMake(0, 0, viewWidth_, viewHeight_);
    CGContextSetRGBFillColor(ctx, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    CGContextFillRect(ctx, fullRect);
#else
    (void)clearColor;
#endif
}

void IOSRenderer::endFrame() {
    // CoreGraphics: لا حاجة لـ swap — UIView يعرض مباشرة
}

// ════════════════════════════════════════════════════════════════════════════════
// دعم الدوران — CoreGraphics SaveGState/TranslateCTM/RotateCTM
// ════════════════════════════════════════════════════════════════════════════════

void IOSRenderer::pushRotation(float centerX, float centerY, float angleDegrees) {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    float angleRad = angleDegrees * M_PI / 180.0f;
    CGContextSaveGState(ctx);
    CGContextTranslateCTM(ctx, centerX, centerY);
    CGContextRotateCTM(ctx, angleRad);
    CGContextTranslateCTM(ctx, -centerX, -centerY);
#else
    (void)centerX; (void)centerY; (void)angleDegrees;
#endif
}

void IOSRenderer::popRotation() {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextRestoreGState(ctx);
#endif
}

void IOSRenderer::pushScale(float centerX, float centerY, float scaleX, float scaleY) {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextSaveGState(ctx);
    CGContextTranslateCTM(ctx, centerX, centerY);
    CGContextScaleCTM(ctx, scaleX, scaleY);
    CGContextTranslateCTM(ctx, -centerX, -centerY);
#else
    (void)centerX; (void)centerY; (void)scaleX; (void)scaleY;
#endif
}

void IOSRenderer::popScale() {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextRestoreGState(ctx);
#endif
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال الرسم الأساسية
// ════════════════════════════════════════════════════════════════════════════════

void IOSRenderer::drawFilledRect(float x, float y, float w, float h,
                                  const Color& color) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextSetRGBFillColor(ctx, c.r, c.g, c.b, c.a);
    CGContextFillRect(ctx, CGRectMake(x, y, w, h));
#else
    (void)x; (void)y; (void)w; (void)h; (void)color;
#endif
}

void IOSRenderer::drawRoundedRect(float x, float y, float w, float h,
                                   const Color& color, float radius) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;

    radius = std::min(radius, std::min(w, h) / 2.0f);
    CGRect rect = CGRectMake(x, y, w, h);
    CGPathRef path = CGPathCreateWithRoundedRect(rect, radius, radius, nullptr);
    CGContextSetRGBFillColor(ctx, c.r, c.g, c.b, c.a);
    CGContextAddPath(ctx, path);
    CGContextFillPath(ctx);
    CGPathRelease(path);
#else
    (void)x; (void)y; (void)w; (void)h; (void)color; (void)radius;
#endif
}

void IOSRenderer::drawRectOutline(float x, float y, float w, float h,
                                   const Color& color, float thickness) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextSetRGBStrokeColor(ctx, c.r, c.g, c.b, c.a);
    CGContextSetLineWidth(ctx, thickness);
    CGContextStrokeRect(ctx, CGRectMake(x, y, w, h));
#else
    (void)x; (void)y; (void)w; (void)h; (void)color; (void)thickness;
#endif
}

std::pair<float, float> IOSRenderer::drawText(const std::string& text,
                                                float x, float y,
                                                const Color& color,
                                                float fontSize) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return {0, 0};

    // إنشاء سلسلة نصية منسّقة عبر CoreText
    @autoreleasepool {
        NSString* nsText = [NSString stringWithUTF8String:text.c_str()];
        if (!nsText) return {0, 0};

        // اختيار الخط (يدعم العربية تلقائياً)
        UIFont* font = [UIFont systemFontOfSize:fontSize];
        UIColor* uiColor = [UIColor colorWithRed:c.r
                                           green:c.g
                                            blue:c.b
                                           alpha:c.a];

        NSDictionary* attrs = @{
            NSFontAttributeName: font,
            NSForegroundColorAttributeName: uiColor
        };

        NSAttributedString* attrStr =
            [[NSAttributedString alloc] initWithString:nsText attributes:attrs];

        CTLineRef line = CTLineCreateWithAttributedString(
            (__bridge CFAttributedStringRef)attrStr);

        // قياس حجم النص
        CGFloat ascent, descent, leading;
        double lineWidth = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
        float textH = static_cast<float>(ascent + descent);

        // CoreGraphics y-axis مقلوب — قلب النص
        CGContextSaveGState(ctx);
        CGContextTranslateCTM(ctx, x, y + ascent);
        CGContextScaleCTM(ctx, 1.0, -1.0);
        CGContextSetTextPosition(ctx, 0, 0);
        CTLineDraw(line, ctx);
        CGContextRestoreGState(ctx);

        CFRelease(line);

        return {static_cast<float>(lineWidth), textH};
    }
#else
    (void)text; (void)x; (void)y; (void)color; (void)fontSize;
    return {fontSize * text.size() * 0.6f, fontSize * 1.2f};
#endif
}

std::pair<float, float> IOSRenderer::measureText(const std::string& text,
                                                    float fontSize) {
#ifdef SAD_IOS_AVAILABLE
    @autoreleasepool {
        NSString* nsText = [NSString stringWithUTF8String:text.c_str()];
        if (!nsText) return {0, 0};

        UIFont* font = [UIFont systemFontOfSize:fontSize];
        NSDictionary* attrs = @{NSFontAttributeName: font};
        CGSize size = [nsText sizeWithAttributes:attrs];
        return {static_cast<float>(size.width), static_cast<float>(size.height)};
    }
#else
    (void)text; (void)fontSize;
    return {fontSize * text.size() * 0.6f, fontSize * 1.2f};
#endif
}

void IOSRenderer::drawImage(const std::string& path,
                              float x, float y, float w, float h) {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;

    @autoreleasepool {
        NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
        UIImage* image = [UIImage imageNamed:nsPath];
        if (!image) image = [UIImage imageWithContentsOfFile:nsPath];
        if (!image) {
            // Placeholder
            drawFilledRect(x, y, w, h, {0.9f, 0.9f, 0.9f, 1.0f});
            return;
        }
        CGRect drawRect = CGRectMake(x, y, w, h);
        // القلب العمودي لأن CoreGraphics y-up
        CGContextSaveGState(ctx);
        CGContextTranslateCTM(ctx, x, y + h);
        CGContextScaleCTM(ctx, 1.0, -1.0);
        CGContextDrawImage(ctx, CGRectMake(0, 0, w, h), image.CGImage);
        CGContextRestoreGState(ctx);
    }
#else
    (void)path; (void)x; (void)y; (void)w; (void)h;
#endif
}

void IOSRenderer::drawCircle(float cx, float cy, float radius,
                               const Color& color) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGRect ellipseRect = CGRectMake(cx - radius, cy - radius,
                                    radius * 2, radius * 2);
    CGContextSetRGBFillColor(ctx, c.r, c.g, c.b, c.a);
    CGContextFillEllipseInRect(ctx, ellipseRect);
#else
    (void)cx; (void)cy; (void)radius; (void)color;
#endif
}

void IOSRenderer::drawCircleOutline(float cx, float cy, float radius,
                                     const Color& color, float thickness) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGRect ellipseRect = CGRectMake(cx - radius, cy - radius,
                                    radius * 2, radius * 2);
    CGContextSetRGBStrokeColor(ctx, c.r, c.g, c.b, c.a);
    CGContextSetLineWidth(ctx, thickness);
    CGContextStrokeEllipseInRect(ctx, ellipseRect);
#else
    (void)cx; (void)cy; (void)radius; (void)color; (void)thickness;
#endif
}

void IOSRenderer::drawLine(float x1, float y1, float x2, float y2,
                             const Color& color, float thickness) {
    auto c = applyOpacity(color);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextSetRGBStrokeColor(ctx, c.r, c.g, c.b, c.a);
    CGContextSetLineWidth(ctx, thickness);
    CGContextMoveToPoint(ctx, x1, y1);
    CGContextAddLineToPoint(ctx, x2, y2);
    CGContextStrokePath(ctx);
#else
    (void)x1; (void)y1; (void)x2; (void)y2; (void)color; (void)thickness;
#endif
}

void IOSRenderer::drawLinearGradient(float x, float y, float w, float h,
                                      const Color& startColor,
                                      const Color& endColor,
                                      bool vertical, float radius) {
    auto sc = applyOpacity(startColor);
    auto ec = applyOpacity(endColor);
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;

    CGContextSaveGState(ctx);

    // قص المنطقة
    if (radius > 0) {
        CGRect rect = CGRectMake(x, y, w, h);
        CGPathRef path = CGPathCreateWithRoundedRect(rect, radius, radius, nullptr);
        CGContextAddPath(ctx, path);
        CGContextClip(ctx);
        CGPathRelease(path);
    } else {
        CGContextClipToRect(ctx, CGRectMake(x, y, w, h));
    }

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGFloat components[] = {
        sc.r, sc.g, sc.b, sc.a,
        ec.r, ec.g, ec.b, ec.a
    };
    CGFloat locations[] = {0.0, 1.0};
    CGGradientRef gradient = CGGradientCreateWithColorComponents(
        colorSpace, components, locations, 2);

    CGPoint startPoint, endPoint;
    if (vertical) {
        startPoint = CGPointMake(x + w / 2, y);
        endPoint = CGPointMake(x + w / 2, y + h);
    } else {
        startPoint = CGPointMake(x, y + h / 2);
        endPoint = CGPointMake(x + w, y + h / 2);
    }

    CGContextDrawLinearGradient(ctx, gradient, startPoint, endPoint, 0);
    CGGradientRelease(gradient);
    CGColorSpaceRelease(colorSpace);

    CGContextRestoreGState(ctx);
#else
    (void)x; (void)y; (void)w; (void)h;
    (void)startColor; (void)endColor; (void)vertical; (void)radius;
#endif
}

void IOSRenderer::setClipRect(float x, float y, float w, float h) {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextSaveGState(ctx);
    CGContextClipToRect(ctx, CGRectMake(x, y, w, h));
#else
    (void)x; (void)y; (void)w; (void)h;
#endif
}

void IOSRenderer::clearClipRect() {
#ifdef SAD_IOS_AVAILABLE
    CGContextRef ctx = static_cast<CGContextRef>(cgContext_);
    if (!ctx) return;
    CGContextRestoreGState(ctx);
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// IOSWindow
// ═══════════════════════════════════════════════════════════════════════════════

IOSWindow::IOSWindow()
    : renderer_(std::make_unique<IOSRenderer>())
    , layoutEngine_(std::make_unique<LayoutEngine>())
{
}

IOSWindow::~IOSWindow() { destroy(); }

bool IOSWindow::create(const PlatformWindowOptions& options) {
    options_ = options;
    width_ = options.width;
    height_ = options.height;
    layoutEngine_->setDirection(options.direction);
    layoutEngine_->setViewportSize(
        static_cast<float>(width_), static_cast<float>(height_));
    renderer_->initialize(nullptr);
    isOpen_ = true;
    needsRedraw_ = true;

    // ─── تهيئة معالج أحداث اللمس الموحد ───
    touchProcessor_.setViewportSize(width_, height_);
    touchProcessor_.setHitTestCallback(
        [this](float x, float y) -> const IRNode* { return hitTest(x, y); });
    touchProcessor_.setFireEventCallback(
        [this](IREventType type, const std::string& expr,
               const IRNode* node, const EventData& data) {
            fireEvent(type, expr, node, data);
        });
    touchProcessor_.setGetTimeMsCallback(
        [this]() -> uint32_t { return getCurrentTimeMs(); });

    // ─── تهيئة معالج أحداث لوحة المفاتيح الموحد ───
    kbProcessor_.setFireEventCallback(
        [this](IREventType type, const std::string& expr,
               const IRNode* node, const EventData& data) {
            fireEvent(type, expr, node, data);
        });
    kbProcessor_.setGetFocusedNodeCallback(
        [this]() -> const IRNode* { return focusedNode_; });
    kbProcessor_.setGetContentRootCallback(
        [this]() -> const IRNode* { return contentRoot_.get(); });
    kbProcessor_.setCollectFocusableNodesCallback(
        [this](std::vector<const IRNode*>& nodes) {
            collectFocusableNodes(contentRoot_.get(), nodes);
        });
    kbProcessor_.setSetFocusedNodeCallback(
        [this](const IRNode* node) { focusedNode_ = node; });
    kbProcessor_.setInvalidateCallback(
        [this]() { invalidate(); });
    kbProcessor_.setCloseWindowCallback(
        [this]() { isOpen_ = false; });

    // ─── تهيئة معالج أحداث الفأرة/المؤشر الموحد (iPadOS pointer) ───
    mouseProcessor_.setHitTestCallback(
        [this](float x, float y) -> const IRNode* { return hitTest(x, y); });
    mouseProcessor_.setFireEventCallback(
        [this](IREventType type, const std::string& expr,
               const IRNode* node, const EventData& data)
        { fireEvent(type, expr, node, data); });
    mouseProcessor_.setGetTimeMsCallback(
        [this]() -> uint32_t { return getCurrentTimeMs(); });
    mouseProcessor_.setInvalidateCallback(
        [this]() { invalidate(); });
    mouseProcessor_.setGetFocusedNodeCallback(
        [this]() -> const IRNode* { return focusedNode_; });
    mouseProcessor_.setSetFocusedNodeCallback(
        [this](const IRNode* node) { focusedNode_ = node; });
    mouseProcessor_.setFindNodeRectCallback(
        [this](const IRNode* node) -> LayoutRect { return findNodeRect(node); });
    mouseProcessor_.setFindScrollableAtCallback(
        [this](float x, float y) -> ScrollableHitInfo {
            auto hit = findScrollableAt(x, y);
            ScrollableHitInfo info;
            if (hit.node && hit.layout) {
                info.node = hit.node;
                info.viewHeight = hit.layout->rect.height;
                info.contentHeight = hit.layout->contentHeight;
                info.currentScrollOffset = renderer_->getScrollOffset(hit.node);
            }
            return info;
        });
    mouseProcessor_.setSetScrollOffsetCallback(
        [this](const IRNode* node, float offset)
        { renderer_->setScrollOffset(node, offset); });
    mouseProcessor_.setGetContentRootCallback(
        [this]() -> const IRNode* { return contentRoot_.get(); });
    mouseProcessor_.setSetCursorCallback(
        [](int /*cursorType*/) {
            // (AR) iOS لا يدعم تغيير شكل المؤشر مبدئياً
            //      iPadOS 13.4+ يدعم UIPointerStyle — يمكن إضافته مستقبلاً
        });

    return true;
}

void IOSWindow::destroy() {
    renderer_->shutdown();
    isOpen_ = false;
}

bool IOSWindow::isOpen() const { return isOpen_; }

void IOSWindow::setContent(std::shared_ptr<IRNode> root) {
    contentRoot_ = std::move(root);
    // ربط محرك الانيميشن بالمُصيّر وتهيئة الحركات
    renderer_->setAnimationEngine(&animationEngine_);
    renderer_->resetAnimationState();
    if (contentRoot_) {
        renderer_->initializeAnimations(contentRoot_, getCurrentTimeMs());
    }
    needsRedraw_ = true;
}

void IOSWindow::setStateStore(std::shared_ptr<StateStore> store) {
    stateStore_ = std::move(store);
}

void IOSWindow::applyPatches(size_t patchedNodes, bool needsRelayout) {
    // (AR) تحديث جزئي — الشجرة contentRoot_ تم تعديلها موضعياً بالفعل
    //      نحتاج فقط:
    //      1. إعادة تهيئة التحريكات للعقد الجديدة فقط (إذا تغيّر التخطيط)
    //      2. طلب إعادة رسم عبر UIView setNeedsDisplay
    // (EN) Partial update — contentRoot_ already patched in-place.
    //      Only reinit animations for new nodes if layout changed, and request redraw.

    if (!contentRoot_)
        return;

    // (AR) تهيئة تحريكات العقد الجديدة فقط
    if (needsRelayout && renderer_) {
        renderer_->initializeAnimations(contentRoot_, getCurrentTimeMs());
    }

    needsRedraw_ = true;
}

void IOSWindow::run() {
    // iOS: UIKit يدير حلقة الأحداث — لا حلقة يدوية
}

bool IOSWindow::runOneFrame() {
    if (!isOpen_) return isOpen_;

    // تحديث الانيميشن — إذا كانت حركات نشطة، نطلب إعادة رسم
    bool animating = animationEngine_.update(getCurrentTimeMs());
    if (animating)
        needsRedraw_ = true;

    if (!needsRedraw_) return isOpen_;

    if (contentRoot_) {
        LayoutConstraints constraints;
        constraints.maxWidth = static_cast<float>(width_);
        constraints.maxHeight = static_cast<float>(height_);
        layoutResult_ = layoutEngine_->layout(contentRoot_, constraints);
    }
    Color bg = {0.98f, 0.98f, 0.98f, 1.0f};
    renderer_->beginFrame(bg);
    if (contentRoot_ && layoutResult_) {
        renderer_->render(contentRoot_, layoutResult_);
    }
    renderer_->endFrame();
    needsRedraw_ = false;
    return isOpen_;
}

void IOSWindow::invalidate() { needsRedraw_ = true; }
int IOSWindow::getWidth() const { return width_; }
int IOSWindow::getHeight() const { return height_; }
PlatformRenderer* IOSWindow::getRenderer() { return renderer_.get(); }

void IOSWindow::onNativeTouchEvent(int action, int64_t touchId, int64_t fingerId,
                                    float x, float y, float pressure,
                                    float dx, float dy) {
    // (AR) تحويل أحداث UITouch من UIKit إلى معالج اللمس الموحد
    //      action: 0=touchesBegan, 1=touchesEnded/Cancelled, 2=touchesMoved
    switch (action) {
    case 0: // DOWN — touchesBegan
        touchProcessor_.onFingerDown(touchId, fingerId, x, y, pressure);
        break;
    case 1: // UP — touchesEnded / touchesCancelled
        touchProcessor_.onFingerUp(touchId, fingerId, x, y);
        break;
    case 2: // MOVE — touchesMoved
        touchProcessor_.onFingerMotion(touchId, fingerId, x, y, dx, dy, pressure);
        break;
    }
    needsRedraw_ = true;
}

void IOSWindow::onNativeKeyEvent(int action, int keyCode,
                                  const std::string& keyName,
                                  bool shift, bool ctrl, bool alt) {
    // (AR) تحويل حدث لوحة المفاتيح من UIKit إلى معالج موحد
    UnifiedKeyCode uKey = static_cast<UnifiedKeyCode>(keyCode);
    KeyModifiers mods;
    mods.shift = shift;
    mods.ctrl = ctrl;
    mods.alt = alt;

    if (action == 0)
        kbProcessor_.onKeyDown(uKey, keyName, mods);
    else if (action == 1)
        kbProcessor_.onKeyUp(uKey, keyName, mods);

    needsRedraw_ = true;
}

void IOSWindow::onNativeTextInput(const std::string& text) {
    // (AR) تمرير إدخال نص من لوحة المفاتيح الافتراضية iOS إلى معالج موحد
    kbProcessor_.onTextInput(text);
    needsRedraw_ = true;
}

// ─── أحداث الفأرة/المؤشر (iPadOS pointer / فأرة متصلة) ───

void IOSWindow::onNativeMouseEvent(int action, int button, float x, float y) {
    // (AR) تحويل حدث زر الفأرة من UIKit إلى معالج موحد
    MouseButton btn = MouseButton::Unknown;
    switch (button) {
        case 0: btn = MouseButton::Left; break;
        case 1: btn = MouseButton::Middle; break;
        case 2: btn = MouseButton::Right; break;
    }
    if (action == 0)
        mouseProcessor_.onMouseDown(btn, x, y);
    else if (action == 1)
        mouseProcessor_.onMouseUp(btn, x, y);
    needsRedraw_ = true;
}

void IOSWindow::onNativeMouseMove(float x, float y,
                                  bool leftPressed,
                                  bool middlePressed,
                                  bool rightPressed) {
    // (AR) تمرير حركة المؤشر إلى معالج موحد
    MouseButtonState buttons;
    buttons.leftPressed = leftPressed;
    buttons.middlePressed = middlePressed;
    buttons.rightPressed = rightPressed;
    mouseProcessor_.onMouseMove(x, y, buttons);
    needsRedraw_ = true;
}

void IOSWindow::onNativeMouseWheel(float deltaX, float deltaY,
                                   float mouseX, float mouseY,
                                   bool ctrlPressed) {
    // (AR) تمرير حدث التمرير إلى معالج موحد
    mouseProcessor_.onMouseWheel(deltaX, deltaY, mouseX, mouseY, ctrlPressed);
    needsRedraw_ = true;
}

} // namespace ios
} // namespace ui
} // namespace sad
