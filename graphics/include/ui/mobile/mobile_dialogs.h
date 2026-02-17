/**
 * @file mobile_dialogs.h
 * @brief مركبات الحوارات والإشعارات للتطبيقات المحمولة
 * 
 * يحتوي على 10 مركبات:
 * - حوار التنبيه AlertDialog
 * - اللوحة السفلية BottomSheet
 * - اللوحة السفلية النموذجية ModalBottomSheet
 * - رسالة سريعة Snackbar
 * - إشعار منبثق Toast
 * - حوار عام Dialog
 * - حوار بسيط SimpleDialog
 * - حوار ملء الشاشة FullScreenDialog
 * - لافتة Banner
 * - إشعار تطبيق AppNotification
 * 
 * @author فريق لغة ص
 * @date 2026
 */
#pragma once

#include "mobile_core.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace sad {
namespace graphics {
namespace ui {
namespace mobile {

// ═══════════════════════════════════════════════════════════════
// 1. حوار التنبيه - AlertDialog
// ═══════════════════════════════════════════════════════════════

/**
 * @class AlertDialog
 * @brief حوار تنبيه - لعرض رسائل تنبيه مع أزرار إجراء
 * 
 * يوفر:
 * - عنوان ومحتوى نصي
 * - أزرار إجراء (تأكيد/إلغاء)
 * - أيقونة اختيارية
 * - إغلاق بالنقر خارج الحوار
 * - تحريك دخول/خروج
 */
class AlertDialog : public Widget {
public:
    std::string title;              // عنوان الحوار
    std::string content;            // المحتوى النصي
    std::string icon;               // أيقونة (اختياري)
    Color iconColor = Color(0.25f, 0.32f, 0.71f, 1);

    // --- الأزرار ---
    struct DialogButton {
        std::string text;
        Color color = Color(0.25f, 0.32f, 0.71f, 1);
        Color textColor = Color(1, 1, 1, 1);
        bool isPrimary = false;     // زر رئيسي (ملون)
        std::function<void()> onPressed;
    };
    std::vector<DialogButton> buttons;

    // --- المظهر ---
    float width = 280.0f;
    float borderRadius = 28.0f;    // Material 3
    Color backgroundColor = Color(1, 1, 1, 1);
    Color scrimColor = Color(0, 0, 0, 0.5f);
    Color titleColor = Color(0.1f, 0.1f, 0.1f, 1);
    Color contentColor = Color(0.4f, 0.4f, 0.4f, 1);
    float elevation = 24.0f;
    EdgeInsets padding = EdgeInsets(24);
    bool dismissible = true;       // إغلاق بالنقر خارجاً
    bool isVisible = false;

    // --- التحريك ---
    float animProgress = 0.0f;     // 0 = مخفي، 1 = مرئي
    float animSpeed = 8.0f;

    // --- الأحداث ---
    std::function<void()> onDismissed;

    void show() {
        isVisible = true;
        animProgress = 0.0f;
    }

    void dismiss() {
        isVisible = false;
        if (onDismissed) onDismissed();
    }

    void addButton(const std::string& text, std::function<void()> callback,
                  bool primary = false) {
        DialogButton btn;
        btn.text = text;
        btn.onPressed = callback;
        btn.isPrimary = primary;
        if (primary) {
            btn.color = Color(0.25f, 0.32f, 0.71f, 1);
            btn.textColor = Color(1, 1, 1, 1);
        } else {
            btn.color = Color(0, 0, 0, 0);
            btn.textColor = Color(0.25f, 0.32f, 0.71f, 1);
        }
        buttons.push_back(btn);
    }

    void update(float dt) {
        if (isVisible && animProgress < 1.0f)
            animProgress = std::min(1.0f, animProgress + dt * animSpeed);
        else if (!isVisible && animProgress > 0.0f)
            animProgress = std::max(0.0f, animProgress - dt * animSpeed);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        // رسم الخلفية الشفافة
        Color scrim = scrimColor;
        scrim.a *= animProgress;
        renderer.fillRect(0, 0, 9999, 9999, scrim);

        // حساب موضع الحوار (وسط الشاشة)
        float dialogH = 200.0f; // تقدير
        float scale = 0.8f + 0.2f * animProgress;
        float dx = (size.x - width * scale) / 2;
        float dy = (size.y - dialogH * scale) / 2;

        // رسم الحوار
        Color bg = backgroundColor;
        bg.a *= animProgress;
        renderer.fillRect(dx, dy, width * scale, dialogH * scale, bg);

        // رسم العنوان
        if (!title.empty()) {
            renderer.drawText(title, dx + padding.left, dy + padding.top, titleColor);
        }

        // رسم المحتوى
        if (!content.empty()) {
            renderer.drawText(content, dx + padding.left, dy + 60, contentColor);
        }

        // رسم الأزرار
        float btnY = dy + dialogH * scale - 52;
        float btnX = dx + width * scale - padding.right;
        for (auto it = buttons.rbegin(); it != buttons.rend(); ++it) {
            float bw = it->text.length() * 8 + 24;
            btnX -= bw + 8;
            if (it->isPrimary) {
                renderer.fillRect(btnX, btnY, bw, 36, it->color);
            }
            renderer.drawText(it->text, btnX + bw / 2, btnY + 18, it->textColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible) return;
        if (dismissible && event.type == SDL_MOUSEBUTTONDOWN) {
            // التحقق من النقر خارج الحوار
            float mx = (float)event.button.x;
            float my = (float)event.button.y;
            float dx = (size.x - width) / 2;
            float dy = (size.y - 200) / 2;
            if (mx < dx || mx > dx + width || my < dy || my > dy + 200) {
                dismiss();
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 2. اللوحة السفلية - BottomSheet
// ═══════════════════════════════════════════════════════════════

/**
 * @class BottomSheet
 * @brief لوحة سفلية قابلة للسحب - لعرض محتوى إضافي
 * 
 * يوفر:
 * - سحب للأعلى والأسفل
 * - نقاط توقف متعددة (مطوي، نصف، كامل)
 * - مقبض سحب مرئي
 * - محتوى قابل للتمرير
 */
class BottomSheet : public Widget {
public:
    // --- نقاط التوقف ---
    float minHeight = 0.0f;        // مطوي تماماً
    float midHeight = 0.0f;        // منتصف (0 = بدون)
    float maxHeight = 0.0f;        // مفتوح بالكامل
    float currentHeight = 0.0f;

    // --- المظهر ---
    Color backgroundColor = Color(1, 1, 1, 1);
    float borderRadius = 16.0f;    // زوايا علوية فقط
    float elevation = 16.0f;
    bool showHandle = true;        // مقبض السحب
    Color handleColor = Color(0.8f, 0.8f, 0.8f, 1);
    float handleWidth = 40.0f;
    float handleHeight = 4.0f;
    bool isVisible = false;

    // --- السحب ---
    bool isDragging = false;
    float dragStartY = 0;
    float dragStartHeight = 0;
    bool enableDrag = true;
    bool dismissible = true;       // قابل للإغلاق بالسحب للأسفل

    // --- المحتوى ---
    std::vector<std::shared_ptr<Widget>> children;
    EdgeInsets padding = EdgeInsets(16, 8, 16, 16);
    float contentScrollOffset = 0.0f;

    // --- الخلفية ---
    bool showScrim = true;
    Color scrimColor = Color(0, 0, 0, 0.5f);

    // --- الأحداث ---
    std::function<void(float)> onHeightChanged;
    std::function<void()> onDismissed;

    void show(float height = 0) {
        isVisible = true;
        currentHeight = (height > 0) ? height : (midHeight > 0 ? midHeight : maxHeight);
    }

    void dismiss() {
        isVisible = false;
        currentHeight = minHeight;
        if (onDismissed) onDismissed();
    }

    void snapToNearest() {
        // التقريب لأقرب نقطة توقف
        float distMin = std::abs(currentHeight - minHeight);
        float distMid = midHeight > 0 ? std::abs(currentHeight - midHeight) : 9999.0f;
        float distMax = std::abs(currentHeight - maxHeight);

        if (distMin <= distMid && distMin <= distMax) {
            if (dismissible) dismiss();
            else currentHeight = minHeight;
        } else if (distMid <= distMax) {
            currentHeight = midHeight;
        } else {
            currentHeight = maxHeight;
        }
    }

    void render(Renderer2D& renderer) override {
        if (!isVisible && currentHeight <= 0) return;

        // رسم الخلفية الشفافة
        if (showScrim) {
            float scrimAlpha = currentHeight / maxHeight;
            Color scrim = scrimColor;
            scrim.a *= scrimAlpha;
            renderer.fillRect(0, 0, 9999, 9999, scrim);
        }

        // رسم اللوحة
        float y = position.y + size.y - currentHeight;
        renderer.fillRect(position.x, y, size.x, currentHeight, backgroundColor);

        // رسم المقبض
        if (showHandle) {
            float hx = position.x + (size.x - handleWidth) / 2;
            float hy = y + 8;
            renderer.fillRect(hx, hy, handleWidth, handleHeight, handleColor);
        }

        // رسم المحتوى
        float contentY = y + (showHandle ? 20 : 0) + padding.top;
        for (auto& child : children) {
            if (child) {
                child->position = Vec2(position.x + padding.left, contentY);
                child->render(renderer);
                contentY += child->size.y;
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible || !enableDrag) return;

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float sheetY = position.y + size.y - currentHeight;
            if (event.button.y >= sheetY) {
                isDragging = true;
                dragStartY = (float)event.button.y;
                dragStartHeight = currentHeight;
            }
        }
        if (event.type == SDL_MOUSEMOTION && isDragging) {
            float delta = dragStartY - event.motion.y;
            currentHeight = std::max(minHeight, std::min(maxHeight, dragStartHeight + delta));
            if (onHeightChanged) onHeightChanged(currentHeight);
        }
        if (event.type == SDL_MOUSEBUTTONUP && isDragging) {
            isDragging = false;
            snapToNearest();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 3. اللوحة السفلية النموذجية - ModalBottomSheet
// ═══════════════════════════════════════════════════════════════

/**
 * @class ModalBottomSheet
 * @brief لوحة سفلية نموذجية - لعرض خيارات أو محتوى مؤقت
 * 
 * النسخة النموذجية تمنع التفاعل مع المحتوى خلفها
 * مثالية لقوائم الخيارات والمشاركة
 */
class ModalBottomSheet : public Widget {
public:
    struct SheetOption {
        std::string icon;
        std::string title;
        std::string subtitle;
        bool enabled = true;
        std::function<void()> onTap;
    };

    std::vector<SheetOption> options;
    std::string title;
    Color backgroundColor = Color(1, 1, 1, 1);
    float borderRadius = 16.0f;
    bool isVisible = false;
    bool showHandle = true;
    Color handleColor = Color(0.8f, 0.8f, 0.8f, 1);
    Color scrimColor = Color(0, 0, 0, 0.5f);
    float optionHeight = 56.0f;
    float animProgress = 0.0f;
    float animSpeed = 8.0f;

    // --- الأحداث ---
    std::function<void()> onDismissed;
    std::function<void(int)> onOptionSelected;

    void addOption(const std::string& icon, const std::string& title,
                  std::function<void()> callback) {
        options.push_back({icon, title, "", true, callback});
    }

    void show() { isVisible = true; }
    void dismiss() {
        isVisible = false;
        if (onDismissed) onDismissed();
    }

    void update(float dt) {
        if (isVisible && animProgress < 1.0f)
            animProgress = std::min(1.0f, animProgress + dt * animSpeed);
        else if (!isVisible && animProgress > 0.0f)
            animProgress = std::max(0.0f, animProgress - dt * animSpeed);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        // الخلفية الشفافة
        Color scrim = scrimColor;
        scrim.a *= animProgress;
        renderer.fillRect(0, 0, 9999, 9999, scrim);

        // اللوحة
        float sheetH = (title.empty() ? 0 : 56) + options.size() * optionHeight + 24;
        float y = position.y + size.y - sheetH * animProgress;

        renderer.fillRect(position.x, y, size.x, sheetH, backgroundColor);

        // المقبض
        if (showHandle) {
            renderer.fillRect(position.x + (size.x - 40) / 2, y + 8, 40, 4, handleColor);
        }

        float contentY = y + 20;

        // العنوان
        if (!title.empty()) {
            renderer.drawText(title, position.x + 16, contentY + 18,
                            Color(0.2f, 0.2f, 0.2f, 1));
            contentY += 56;
        }

        // الخيارات
        for (size_t i = 0; i < options.size(); i++) {
            Color textColor = options[i].enabled ?
                Color(0.2f, 0.2f, 0.2f, 1) : Color(0.6f, 0.6f, 0.6f, 1);
            renderer.drawText(options[i].title, position.x + 56,
                            contentY + optionHeight / 2, textColor);
            contentY += optionHeight;
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible) return;
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float sheetH = (title.empty() ? 0 : 56) + options.size() * optionHeight + 24;
            float sheetY = position.y + size.y - sheetH;
            if (event.button.y < sheetY) {
                dismiss();
            } else {
                int idx = (int)((event.button.y - sheetY - 20 -
                    (title.empty() ? 0 : 56)) / optionHeight);
                if (idx >= 0 && idx < (int)options.size() && options[idx].enabled) {
                    if (options[idx].onTap) options[idx].onTap();
                    if (onOptionSelected) onOptionSelected(idx);
                    dismiss();
                }
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 4. رسالة سريعة - Snackbar
// ═══════════════════════════════════════════════════════════════

/**
 * @class Snackbar
 * @brief رسالة سريعة في أسفل الشاشة - للإشعارات المؤقتة مع إجراء
 * 
 * يوفر:
 * - رسالة نصية مؤقتة
 * - زر إجراء (مثل: تراجع)
 * - اختفاء تلقائي بعد مدة
 * - قابلة للإغلاق بالسحب
 */
class Snackbar : public Widget {
public:
    std::string message;            // نص الرسالة
    std::string actionLabel;        // نص زر الإجراء
    Color backgroundColor = Color(0.2f, 0.2f, 0.2f, 1);
    Color textColor = Color(1, 1, 1, 1);
    Color actionColor = Color(0.5f, 0.7f, 1.0f, 1);
    float duration = 4000.0f;      // مدة العرض (ملي ثانية)
    float height = 48.0f;
    float borderRadius = 4.0f;
    float marginBottom = 16.0f;
    float marginH = 16.0f;
    bool isVisible = false;
    bool dismissOnAction = true;

    // --- التحريك ---
    float animProgress = 0.0f;
    float timer = 0.0f;

    // --- الأحداث ---
    std::function<void()> onAction;
    std::function<void()> onDismissed;

    void show(const std::string& msg, const std::string& action = "",
             std::function<void()> callback = nullptr) {
        message = msg;
        actionLabel = action;
        onAction = callback;
        isVisible = true;
        timer = 0;
        animProgress = 0;
    }

    void dismiss() {
        isVisible = false;
        if (onDismissed) onDismissed();
    }

    void update(float dt) {
        if (isVisible) {
            if (animProgress < 1.0f)
                animProgress = std::min(1.0f, animProgress + dt * 8);
            timer += dt * 1000;
            if (timer >= duration) dismiss();
        } else if (animProgress > 0) {
            animProgress = std::max(0.0f, animProgress - dt * 8);
        }
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        float w = size.x - marginH * 2;
        float x = position.x + marginH;
        float y = position.y + size.y - marginBottom - height * animProgress;

        renderer.fillRect(x, y, w, height, backgroundColor);
        renderer.drawText(message, x + 16, y + height / 2, textColor);

        if (!actionLabel.empty()) {
            renderer.drawText(actionLabel, x + w - 80, y + height / 2, actionColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible) return;
        if (event.type == SDL_MOUSEBUTTONDOWN && !actionLabel.empty()) {
            if (onAction) onAction();
            if (dismissOnAction) dismiss();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 5. إشعار منبثق - Toast
// ═══════════════════════════════════════════════════════════════

/**
 * @brief موضع الإشعار المنبثق
 */
enum class ToastPosition {
    Top,     // أعلى الشاشة
    Center,  // وسط الشاشة
    Bottom   // أسفل الشاشة
};

/**
 * @brief مدة عرض الإشعار
 */
enum class ToastDuration {
    Short = 2000,   // قصيرة (2 ثانية)
    Long = 3500,    // طويلة (3.5 ثانية)
    Infinite = 0    // لا نهائية (يدوي)
};

/**
 * @class Toast
 * @brief إشعار منبثق بسيط - رسالة مؤقتة بدون إجراء
 * 
 * أبسط من Snackbar، يظهر رسالة فقط ويختفي تلقائياً
 */
class Toast : public Widget {
public:
    std::string message;
    ToastPosition toastPosition = ToastPosition::Bottom;
    ToastDuration toastDuration = ToastDuration::Short;
    Color backgroundColor = Color(0.2f, 0.2f, 0.2f, 0.85f);
    Color textColor = Color(1, 1, 1, 1);
    float borderRadius = 20.0f;
    float fontSize = 14.0f;
    float paddingH = 24.0f;
    float paddingV = 12.0f;
    bool isVisible = false;
    float animProgress = 0.0f;
    float timer = 0.0f;

    // --- قائمة انتظار ---
    struct QueuedToast {
        std::string message;
        ToastDuration duration;
    };
    std::vector<QueuedToast> queue;

    void show(const std::string& msg, ToastDuration dur = ToastDuration::Short) {
        if (isVisible) {
            queue.push_back({msg, dur});
            return;
        }
        message = msg;
        toastDuration = dur;
        isVisible = true;
        timer = 0;
        animProgress = 0;
    }

    void dismiss() {
        isVisible = false;
        // إظهار التالي في القائمة
        if (!queue.empty()) {
            auto next = queue.front();
            queue.erase(queue.begin());
            show(next.message, next.duration);
        }
    }

    void update(float dt) {
        if (isVisible) {
            if (animProgress < 1.0f) animProgress = std::min(1.0f, animProgress + dt * 6);
            if ((int)toastDuration > 0) {
                timer += dt * 1000;
                if (timer >= (float)toastDuration) dismiss();
            }
        } else if (animProgress > 0) {
            animProgress = std::max(0.0f, animProgress - dt * 6);
        }
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        float w = message.length() * fontSize * 0.6f + paddingH * 2;
        float h = fontSize + paddingV * 2;
        float x = position.x + (size.x - w) / 2;
        float y;

        switch (toastPosition) {
            case ToastPosition::Top: y = position.y + 80; break;
            case ToastPosition::Center: y = position.y + (size.y - h) / 2; break;
            default: y = position.y + size.y - 100; break;
        }

        Color bg = backgroundColor;
        bg.a *= animProgress;
        renderer.fillRect(x, y, w, h, bg);

        Color tc = textColor;
        tc.a *= animProgress;
        renderer.drawText(message, x + w / 2, y + h / 2, tc);
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 6. حوار عام - Dialog
// ═══════════════════════════════════════════════════════════════

/**
 * @class Dialog
 * @brief حوار عام قابل للتخصيص - لبناء حوارات مخصصة
 * 
 * أكثر مرونة من AlertDialog، يسمح بمحتوى مخصص كامل
 */
class Dialog : public Widget {
public:
    std::string title;
    float width = 320.0f;
    float maxHeight = 0.0f;        // 0 = تلقائي
    Color backgroundColor = Color(1, 1, 1, 1);
    Color scrimColor = Color(0, 0, 0, 0.5f);
    float borderRadius = 28.0f;
    float elevation = 24.0f;
    EdgeInsets titlePadding = EdgeInsets(24, 24, 24, 0);
    EdgeInsets contentPadding = EdgeInsets(24);
    EdgeInsets actionsPadding = EdgeInsets(8, 16, 24, 24);
    bool dismissible = true;
    bool isVisible = false;
    float animProgress = 0.0f;

    // --- المحتوى المخصص ---
    std::shared_ptr<Widget> contentWidget;
    std::vector<std::shared_ptr<Widget>> actionWidgets;

    // --- الأحداث ---
    std::function<void()> onDismissed;

    void show() { isVisible = true; animProgress = 0; }
    void dismiss() {
        isVisible = false;
        if (onDismissed) onDismissed();
    }

    void update(float dt) {
        if (isVisible && animProgress < 1.0f)
            animProgress = std::min(1.0f, animProgress + dt * 8);
        else if (!isVisible && animProgress > 0)
            animProgress = std::max(0.0f, animProgress - dt * 8);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        Color scrim = scrimColor;
        scrim.a *= animProgress;
        renderer.fillRect(0, 0, 9999, 9999, scrim);

        float dialogH = maxHeight > 0 ? maxHeight : 300;
        float dx = (size.x - width) / 2;
        float dy = (size.y - dialogH) / 2;

        Color bg = backgroundColor;
        bg.a *= animProgress;
        renderer.fillRect(dx, dy, width, dialogH, bg);

        if (!title.empty()) {
            renderer.drawText(title, dx + titlePadding.left, dy + titlePadding.top + 12,
                            Color(0.1f, 0.1f, 0.1f, 1));
        }

        if (contentWidget) {
            contentWidget->position = Vec2(dx + contentPadding.left,
                                          dy + 50 + contentPadding.top);
            contentWidget->render(renderer);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (isVisible && dismissible && event.type == SDL_MOUSEBUTTONDOWN) {
            float dx = (size.x - width) / 2;
            float dy = (size.y - 300) / 2;
            if (event.button.x < dx || event.button.x > dx + width ||
                event.button.y < dy || event.button.y > dy + 300) {
                dismiss();
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 7. حوار بسيط - SimpleDialog
// ═══════════════════════════════════════════════════════════════

/**
 * @class SimpleDialog
 * @brief حوار اختيار بسيط - لعرض قائمة خيارات
 * 
 * مناسب لاختيار عنصر واحد من قائمة قصيرة
 */
class SimpleDialog : public Widget {
public:
    struct DialogOption {
        std::string icon;
        std::string text;
        bool enabled = true;
    };

    std::string title;
    std::vector<DialogOption> options;
    Color backgroundColor = Color(1, 1, 1, 1);
    Color scrimColor = Color(0, 0, 0, 0.5f);
    float width = 280.0f;
    float optionHeight = 56.0f;
    float borderRadius = 28.0f;
    bool isVisible = false;
    float animProgress = 0.0f;

    // --- الأحداث ---
    std::function<void(int)> onOptionSelected;
    std::function<void()> onDismissed;

    void addOption(const std::string& icon, const std::string& text) {
        options.push_back({icon, text, true});
    }

    void show() { isVisible = true; animProgress = 0; }
    void dismiss() {
        isVisible = false;
        if (onDismissed) onDismissed();
    }

    void update(float dt) {
        if (isVisible && animProgress < 1.0f)
            animProgress = std::min(1.0f, animProgress + dt * 8);
        else if (!isVisible && animProgress > 0)
            animProgress = std::max(0.0f, animProgress - dt * 8);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        Color scrim = scrimColor;
        scrim.a *= animProgress;
        renderer.fillRect(0, 0, 9999, 9999, scrim);

        float h = (title.empty() ? 0 : 56) + options.size() * optionHeight + 8;
        float dx = (size.x - width) / 2;
        float dy = (size.y - h) / 2;

        renderer.fillRect(dx, dy, width, h, backgroundColor);

        float y = dy;
        if (!title.empty()) {
            renderer.drawText(title, dx + 24, y + 28, Color(0.2f, 0.2f, 0.2f, 1));
            y += 56;
        }

        for (size_t i = 0; i < options.size(); i++) {
            Color tc = options[i].enabled ?
                Color(0.2f, 0.2f, 0.2f, 1) : Color(0.6f, 0.6f, 0.6f, 1);
            renderer.drawText(options[i].text, dx + 56, y + optionHeight / 2, tc);
            y += optionHeight;
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible) return;
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float h = (title.empty() ? 0 : 56) + options.size() * optionHeight + 8;
            float dx = (size.x - width) / 2;
            float dy = (size.y - h) / 2;

            if (event.button.x < dx || event.button.x > dx + width ||
                event.button.y < dy || event.button.y > dy + h) {
                dismiss();
            } else {
                int idx = (int)((event.button.y - dy - (title.empty() ? 0 : 56)) / optionHeight);
                if (idx >= 0 && idx < (int)options.size() && options[idx].enabled) {
                    if (onOptionSelected) onOptionSelected(idx);
                    dismiss();
                }
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 8. حوار ملء الشاشة - FullScreenDialog
// ═══════════════════════════════════════════════════════════════

/**
 * @class FullScreenDialog
 * @brief حوار ملء الشاشة - لنماذج الإدخال الكبيرة
 * 
 * يغطي الشاشة بالكامل مع شريط علوي وإجراءات حفظ/إغلاق
 */
class FullScreenDialog : public Widget {
public:
    std::string title;
    std::string saveLabel = "حفظ";
    Color backgroundColor = Color(1, 1, 1, 1);
    Color appBarColor = Color(1, 1, 1, 1);
    Color titleColor = Color(0.1f, 0.1f, 0.1f, 1);
    Color saveColor = Color(0.25f, 0.32f, 0.71f, 1);
    bool isVisible = false;
    float animProgress = 0.0f;
    float animSpeed = 6.0f;

    // --- المحتوى ---
    std::shared_ptr<Widget> contentWidget;

    // --- الأحداث ---
    std::function<void()> onSave;
    std::function<void()> onClose;

    void show() { isVisible = true; animProgress = 0; }
    void close() {
        isVisible = false;
        if (onClose) onClose();
    }

    void update(float dt) {
        if (isVisible && animProgress < 1.0f)
            animProgress = std::min(1.0f, animProgress + dt * animSpeed);
        else if (!isVisible && animProgress > 0)
            animProgress = std::max(0.0f, animProgress - dt * animSpeed);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        float slideY = (1.0f - animProgress) * size.y;

        // الخلفية
        renderer.fillRect(position.x, position.y + slideY, size.x, size.y, backgroundColor);

        // شريط التطبيق
        renderer.fillRect(position.x, position.y + slideY, size.x, 56, appBarColor);

        // زر الإغلاق
        renderer.drawText("✕", position.x + 16, position.y + slideY + 28, titleColor);

        // العنوان
        renderer.drawText(title, position.x + 72, position.y + slideY + 28, titleColor);

        // زر الحفظ
        renderer.drawText(saveLabel, position.x + size.x - 80,
                         position.y + slideY + 28, saveColor);

        // المحتوى
        if (contentWidget) {
            contentWidget->position = Vec2(position.x, position.y + slideY + 56);
            contentWidget->size = Vec2(size.x, size.y - 56);
            contentWidget->render(renderer);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!isVisible) return;
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.y < position.y + 56) {
                if (event.button.x < 56) close();
                else if (event.button.x > size.x - 100) {
                    if (onSave) onSave();
                }
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 9. لافتة - Banner
// ═══════════════════════════════════════════════════════════════

/**
 * @class Banner
 * @brief لافتة إعلامية - رسالة في أعلى المحتوى مع إجراءات
 * 
 * تبقى ظاهرة حتى يتخذ المستخدم إجراء
 * مناسبة لرسائل الحالة والتحديثات
 */
class Banner : public Widget {
public:
    std::string message;            // نص الرسالة
    std::string leadingIcon;        // أيقونة
    Color backgroundColor = Color(1, 0.98f, 0.93f, 1); // أصفر فاتح
    Color textColor = Color(0.3f, 0.25f, 0.1f, 1);
    Color iconColor = Color(0.8f, 0.6f, 0.0f, 1);
    float height = 0;              // 0 = تلقائي

    struct BannerAction {
        std::string text;
        Color color = Color(0.25f, 0.32f, 0.71f, 1);
        std::function<void()> onPressed;
    };
    std::vector<BannerAction> actions;

    bool isVisible = false;
    float animProgress = 0.0f;
    bool showDivider = true;

    // --- الأحداث ---
    std::function<void()> onDismissed;

    /**
     * @brief أنماط جاهزة
     */
    enum class BannerType { Info, Warning, Error, Success };

    void setType(BannerType type) {
        switch (type) {
            case BannerType::Info:
                backgroundColor = Color(0.9f, 0.95f, 1.0f, 1);
                iconColor = Color(0.25f, 0.32f, 0.71f, 1);
                textColor = Color(0.1f, 0.15f, 0.4f, 1);
                break;
            case BannerType::Warning:
                backgroundColor = Color(1, 0.98f, 0.93f, 1);
                iconColor = Color(0.8f, 0.6f, 0.0f, 1);
                textColor = Color(0.3f, 0.25f, 0.1f, 1);
                break;
            case BannerType::Error:
                backgroundColor = Color(1, 0.93f, 0.93f, 1);
                iconColor = Color(0.8f, 0.2f, 0.2f, 1);
                textColor = Color(0.4f, 0.1f, 0.1f, 1);
                break;
            case BannerType::Success:
                backgroundColor = Color(0.93f, 1, 0.93f, 1);
                iconColor = Color(0.2f, 0.7f, 0.2f, 1);
                textColor = Color(0.1f, 0.3f, 0.1f, 1);
                break;
        }
    }

    void show() { isVisible = true; }
    void dismiss() {
        isVisible = false;
        if (onDismissed) onDismissed();
    }

    void addAction(const std::string& text, std::function<void()> callback) {
        actions.push_back({text, Color(0.25f, 0.32f, 0.71f, 1), callback});
    }

    void update(float dt) {
        float target = isVisible ? 1.0f : 0.0f;
        if (animProgress < target) animProgress = std::min(1.0f, animProgress + dt * 6);
        else if (animProgress > target) animProgress = std::max(0.0f, animProgress - dt * 6);
    }

    void render(Renderer2D& renderer) override {
        if (animProgress <= 0) return;

        float h = height > 0 ? height : 72.0f;
        float y = position.y - h * (1.0f - animProgress);

        renderer.fillRect(position.x, y, size.x, h * animProgress, backgroundColor);
        renderer.drawText(message, position.x + 56, y + h / 3, textColor);

        float btnX = position.x + size.x;
        for (auto it = actions.rbegin(); it != actions.rend(); ++it) {
            float bw = it->text.length() * 8 + 16;
            btnX -= bw + 8;
            renderer.drawText(it->text, btnX + bw / 2, y + h * 2 / 3, it->color);
        }

        if (showDivider) {
            renderer.fillRect(position.x, y + h * animProgress - 1,
                            size.x, 1, Color(0.85f, 0.85f, 0.85f, 1));
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 10. إشعار التطبيق - AppNotification
// ═══════════════════════════════════════════════════════════════

/**
 * @brief مستوى أهمية الإشعار
 */
enum class NotificationLevel {
    Info,     // معلومات
    Success,  // نجاح
    Warning,  // تحذير
    Error     // خطأ
};

/**
 * @class AppNotification
 * @brief نظام إشعارات داخل التطبيق - يدير قائمة إشعارات متعددة
 * 
 * يوفر:
 * - إشعارات متراكمة
 * - مستويات متعددة (معلومات، نجاح، تحذير، خطأ)
 * - تأثيرات دخول وخروج
 * - قابلة للإغلاق بالسحب
 * - أيقونات وإجراءات
 */
class AppNotification : public Widget {
public:
    struct Notification {
        std::string id;
        std::string title;
        std::string message;
        std::string icon;
        NotificationLevel level = NotificationLevel::Info;
        float duration = 5000.0f;  // ملي ثانية (0 = دائم)
        std::function<void()> onTap;
        std::function<void()> onDismissed;

        // --- حالة داخلية ---
        float timer = 0.0f;
        float animProgress = 0.0f;
        bool isRemoving = false;
    };

    std::vector<Notification> notifications;
    int maxVisible = 3;             // أقصى عدد مرئي
    float notificationHeight = 72.0f;
    float spacing = 8.0f;
    float marginRight = 16.0f;
    float marginTop = 60.0f;
    float width = 350.0f;

    // --- موضع الإشعارات ---
    enum class Position { TopRight, TopLeft, BottomRight, BottomLeft, TopCenter }
        notifPosition = Position::TopRight;

    void notify(const std::string& title, const std::string& message,
               NotificationLevel level = NotificationLevel::Info,
               float duration = 5000.0f) {
        Notification n;
        n.id = std::to_string(notifications.size());
        n.title = title;
        n.message = message;
        n.level = level;
        n.duration = duration;
        notifications.push_back(n);

        // إزالة القديمة إذا تجاوزنا الحد
        while ((int)notifications.size() > maxVisible) {
            notifications.erase(notifications.begin());
        }
    }

    void dismissAll() {
        for (auto& n : notifications) n.isRemoving = true;
    }

    Color getLevelColor(NotificationLevel level) const {
        switch (level) {
            case NotificationLevel::Info: return Color(0.25f, 0.32f, 0.71f, 1);
            case NotificationLevel::Success: return Color(0.2f, 0.7f, 0.2f, 1);
            case NotificationLevel::Warning: return Color(0.9f, 0.7f, 0.0f, 1);
            case NotificationLevel::Error: return Color(0.9f, 0.2f, 0.2f, 1);
            default: return Color(0.5f, 0.5f, 0.5f, 1);
        }
    }

    void update(float dt) {
        for (auto it = notifications.begin(); it != notifications.end();) {
            if (it->isRemoving) {
                it->animProgress -= dt * 6;
                if (it->animProgress <= 0) {
                    if (it->onDismissed) it->onDismissed();
                    it = notifications.erase(it);
                    continue;
                }
            } else {
                if (it->animProgress < 1.0f)
                    it->animProgress = std::min(1.0f, it->animProgress + dt * 6);

                if (it->duration > 0) {
                    it->timer += dt * 1000;
                    if (it->timer >= it->duration) it->isRemoving = true;
                }
            }
            ++it;
        }
    }

    void render(Renderer2D& renderer) override {
        float y = marginTop;

        for (auto& n : notifications) {
            if (n.animProgress <= 0) continue;

            float x = size.x - width - marginRight;
            float offsetX = (1.0f - n.animProgress) * (width + marginRight);

            Color accentColor = getLevelColor(n.level);
            Color bg(1, 1, 1, n.animProgress);

            // رسم البطاقة
            renderer.fillRect(x + offsetX, y, width, notificationHeight, bg);

            // شريط اللون الجانبي
            renderer.fillRect(x + offsetX, y, 4, notificationHeight, accentColor);

            // العنوان والرسالة
            Color titleC(0.15f, 0.15f, 0.15f, n.animProgress);
            Color msgC(0.5f, 0.5f, 0.5f, n.animProgress);
            renderer.drawText(n.title, x + offsetX + 16, y + 20, titleC);
            renderer.drawText(n.message, x + offsetX + 16, y + 44, msgC);

            y += (notificationHeight + spacing) * n.animProgress;
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float y = marginTop;
            for (auto& n : notifications) {
                float x = size.x - width - marginRight;
                if (event.button.x >= x && event.button.x <= x + width &&
                    event.button.y >= y && event.button.y <= y + notificationHeight) {
                    if (n.onTap) n.onTap();
                    else n.isRemoving = true;
                    break;
                }
                y += notificationHeight + spacing;
            }
        }
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad
