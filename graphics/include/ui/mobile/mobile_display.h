/**
 * @file mobile_display.h
 * @brief مركبات العرض والمحتوى للتطبيقات المحمولة
 * 
 * يحتوي على 15 مركب عرض:
 * - البطاقة Card
 * - الشريحة Chip
 * - الشارة Badge
 * - الصورة الرمزية Avatar
 * - التلميح Tooltip
 * - شريط التقدم ProgressBar
 * - مؤشر التحميل الدائري CircularProgress
 * - الهيكل العظمي Skeleton
 * - الفاصل Divider
 * - عنصر القائمة ListTile
 * - بطاقة قابلة للتوسع ExpansionTile
 * - جدول البيانات DataTable
 * - العرض الدوار Carousel
 * - الخط الزمني Timeline
 * - حالة فارغة EmptyState
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
// 1. البطاقة - Card
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع البطاقة
 */
enum class CardVariant {
    Elevated,   // مرتفعة بظل
    Filled,     // ملونة بخلفية
    Outlined    // محاطة بإطار
};

/**
 * @class Card
 * @brief بطاقة عرض المحتوى - حاوية مرتفعة مع ظل
 * 
 * يوفر:
 * - أنماط متعددة (مرتفعة، ملونة، محاطة)
 * - زوايا دائرية
 * - ظل قابل للتخصيص
 * - قابلة للنقر
 * - صورة غلاف اختيارية
 */
class Card : public Widget {
public:
    CardVariant variant = CardVariant::Elevated;
    Color backgroundColor = Color(1, 1, 1, 1);
    Color borderColor = Color(0.8f, 0.8f, 0.8f, 1);
    float borderWidth = 1.0f;
    float borderRadius = 12.0f;    // زوايا دائرية
    float elevation = 2.0f;        // ارتفاع الظل
    EdgeInsets padding = EdgeInsets(16);
    EdgeInsets margin = EdgeInsets(8);
    bool clickable = false;
    bool hoverable = true;
    float hoverElevation = 8.0f;   // ارتفاع عند التحويم

    // --- صورة الغلاف ---
    std::string coverImage;
    float coverHeight = 0;         // 0 = بدون صورة
    bool coverOnTop = true;        // الصورة في الأعلى

    // --- المحتوى ---
    std::string title;
    std::string subtitle;
    std::string body;
    std::vector<std::shared_ptr<Widget>> children;

    // --- الإجراءات ---
    struct CardAction {
        std::string text;
        std::function<void()> onPressed;
    };
    std::vector<CardAction> actions;

    // --- الأحداث ---
    std::function<void()> onTap;
    std::function<void()> onLongPress;

    void addAction(const std::string& text, std::function<void()> callback) {
        actions.push_back({text, callback});
    }

    void render(Renderer2D& renderer) override {
        float x = position.x + margin.left;
        float y = position.y + margin.top;
        float w = size.x - margin.left - margin.right;
        float h = size.y - margin.top - margin.bottom;

        // رسم الظل
        if (variant == CardVariant::Elevated && elevation > 0) {
            Color shadow(0, 0, 0, 0.15f);
            renderer.fillRect(x + 2, y + elevation, w, h, shadow);
        }

        // رسم البطاقة
        renderer.fillRect(x, y, w, h, backgroundColor);

        // رسم الإطار (للنوع المحاط)
        if (variant == CardVariant::Outlined) {
            renderer.drawRect(x, y, w, h, borderColor);
        }

        // رسم العنوان
        if (!title.empty()) {
            renderer.drawText(title, x + padding.left, y + coverHeight + padding.top,
                            Color(0.1f, 0.1f, 0.1f, 1));
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (clickable && event.type == SDL_MOUSEBUTTONDOWN) {
            if (onTap) onTap();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 2. الشريحة - Chip
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع الشريحة
 */
enum class ChipType {
    Assist,     // مساعدة - اقتراحات إجراءات
    Filter,     // تصفية - لتصفية المحتوى
    Input,      // إدخال - قابلة للحذف
    Suggestion  // اقتراح - اقتراحات نصية
};

/**
 * @class Chip
 * @brief شريحة تفاعلية - للعلامات والتصفية والاقتراحات
 * 
 * يوفر:
 * - أنواع متعددة (مساعدة، تصفية، إدخال، اقتراح)
 * - أيقونة بداية ونهاية
 * - قابلة للتحديد والحذف
 * - حجم قابل للتخصيص
 */
class Chip : public Widget {
public:
    std::string label;              // نص الشريحة
    ChipType type = ChipType::Assist;
    bool selected = false;
    bool deletable = false;         // قابلة للحذف
    bool enabled = true;
    std::string leadingIcon;        // أيقونة البداية
    std::string avatar;            // صورة رمزية بدل الأيقونة
    Color backgroundColor = Color(0.93f, 0.93f, 0.93f, 1);
    Color selectedColor = Color(0.88f, 0.88f, 1.0f, 1);
    Color labelColor = Color(0.2f, 0.2f, 0.2f, 1);
    Color borderColor = Color(0.75f, 0.75f, 0.75f, 1);
    float height = 32.0f;
    float borderRadius = 16.0f;    // زوايا كاملة (حبة دواء)
    float fontSize = 14.0f;

    // --- الأحداث ---
    std::function<void()> onTap;
    std::function<void()> onDeleted;
    std::function<void(bool)> onSelected;

    void render(Renderer2D& renderer) override {
        Color bg = selected ? selectedColor : backgroundColor;
        renderer.fillRect(position.x, position.y, size.x, height, bg);
        renderer.drawText(label, position.x + 12, position.y + height / 2, labelColor);
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (type == ChipType::Filter) {
                selected = !selected;
                if (onSelected) onSelected(selected);
            }
            if (onTap) onTap();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 3. الشارة - Badge
// ═══════════════════════════════════════════════════════════════

/**
 * @class Badge
 * @brief شارة إشعار - تظهر فوق المركبات الأخرى
 * 
 * يوفر:
 * - عرض رقم أو نقطة
 * - موضع قابل للتخصيص
 * - تحريك عند الظهور/الاختفاء
 * - حد أقصى للعرض (99+)
 */
class Badge : public Widget {
public:
    int count = 0;                  // عدد الإشعارات
    bool showDot = false;           // إظهار نقطة فقط بدون رقم
    int maxCount = 99;              // الحد الأقصى (يعرض 99+)
    Color backgroundColor = Color(0.9f, 0.2f, 0.2f, 1);
    Color textColor = Color(1, 1, 1, 1);
    float dotSize = 8.0f;          // حجم النقطة
    float fontSize = 10.0f;
    float paddingH = 6.0f;         // حشو أفقي
    float paddingV = 2.0f;
    bool visible = true;
    float offsetX = 0;             // إزاحة أفقية
    float offsetY = 0;             // إزاحة رأسية

    // --- المركب الأب ---
    std::shared_ptr<Widget> child;

    std::string getDisplayText() const {
        if (showDot || count <= 0) return "";
        if (count > maxCount) return std::to_string(maxCount) + "+";
        return std::to_string(count);
    }

    void render(Renderer2D& renderer) override {
        // رسم المركب الأبن أولاً
        if (child) child->render(renderer);

        if (!visible || (count <= 0 && !showDot)) return;

        // حساب موضع الشارة
        float bx = position.x + size.x + offsetX;
        float by = position.y + offsetY;

        if (showDot) {
            renderer.fillRect(bx - dotSize / 2, by - dotSize / 2,
                            dotSize, dotSize, backgroundColor);
        } else {
            std::string text = getDisplayText();
            float w = text.length() * fontSize * 0.7f + paddingH * 2;
            float h = fontSize + paddingV * 2;
            renderer.fillRect(bx - w / 2, by - h / 2, w, h, backgroundColor);
            renderer.drawText(text, bx, by, textColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (child) child->handleEvent(event);
    }
};

// ═══════════════════════════════════════════════════════════════
// 4. الصورة الرمزية - Avatar
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع الصورة الرمزية
 */
enum class AvatarType {
    Image,      // صورة
    Text,       // حرف أو حرفين
    Icon        // أيقونة
};

/**
 * @class Avatar
 * @brief صورة رمزية دائرية - لعرض صورة المستخدم أو الحرف الأول
 * 
 * يوفر:
 * - صورة أو نص أو أيقونة
 * - أحجام متعددة (صغير، متوسط، كبير)
 * - حالة اتصال (أخضر/برتقالي/أحمر)
 * - حدود قابلة للتخصيص
 */
class Avatar : public Widget {
public:
    AvatarType type = AvatarType::Text;
    std::string imagePath;          // مسار الصورة
    std::string text;               // نص (حرف أو حرفين)
    std::string icon;               // أيقونة
    float radius = 20.0f;          // نصف القطر
    Color backgroundColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color foregroundColor = Color(1, 1, 1, 1);
    Color borderColor = Color(1, 1, 1, 1);
    float borderWidth = 0;          // عرض الحدود
    float fontSize = 16.0f;

    // --- حالة الاتصال ---
    bool showStatus = false;
    enum class Status { Online, Away, Busy, Offline } status = Status::Offline;
    float statusSize = 12.0f;

    // --- مجموعة ---
    bool isGroup = false;          // عرض كمجموعة (عدة صور)
    std::vector<std::string> groupImages;

    Color getStatusColor() const {
        switch (status) {
            case Status::Online: return Color(0.3f, 0.8f, 0.3f, 1);
            case Status::Away: return Color(1.0f, 0.7f, 0.0f, 1);
            case Status::Busy: return Color(0.9f, 0.2f, 0.2f, 1);
            default: return Color(0.6f, 0.6f, 0.6f, 1);
        }
    }

    void render(Renderer2D& renderer) override {
        float cx = position.x + radius;
        float cy = position.y + radius;

        // رسم الحدود
        if (borderWidth > 0) {
            renderer.fillRect(cx - radius - borderWidth, cy - radius - borderWidth,
                            (radius + borderWidth) * 2, (radius + borderWidth) * 2,
                            borderColor);
        }

        // رسم الدائرة
        renderer.fillRect(cx - radius, cy - radius, radius * 2, radius * 2, backgroundColor);

        // رسم النص
        if (type == AvatarType::Text && !text.empty()) {
            renderer.drawText(text, cx, cy, foregroundColor);
        }

        // رسم حالة الاتصال
        if (showStatus) {
            float sx = cx + radius * 0.7f;
            float sy = cy + radius * 0.7f;
            renderer.fillRect(sx - statusSize / 2, sy - statusSize / 2,
                            statusSize, statusSize, getStatusColor());
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 5. التلميح - Tooltip
// ═══════════════════════════════════════════════════════════════

/**
 * @brief موضع التلميح
 */
enum class TooltipPosition {
    Above,  // فوق المركب
    Below,  // تحت المركب
    Left,   // يسار المركب
    Right   // يمين المركب
};

/**
 * @class Tooltip
 * @brief تلميح معلوماتي - يظهر عند التحويم أو الضغط المطوّل
 * 
 * يوفر:
 * - نص تلميحي عند التحويم
 * - موضع تلقائي ذكي
 * - تأخير قبل الظهور
 * - دعم محتوى غني
 */
class Tooltip : public Widget {
public:
    std::string message;            // نص التلميح
    TooltipPosition preferredPosition = TooltipPosition::Above;
    Color backgroundColor = Color(0.2f, 0.2f, 0.2f, 0.9f);
    Color textColor = Color(1, 1, 1, 1);
    float fontSize = 12.0f;
    float padding = 8.0f;
    float borderRadius = 4.0f;
    float showDelay = 500.0f;      // تأخير الظهور (ملي ثانية)
    float hideDelay = 0.0f;
    float maxWidth = 200.0f;
    bool isVisible = false;
    bool richContent = false;      // محتوى غني (HTML بسيط)

    // --- المركب الأبن ---
    std::shared_ptr<Widget> child;

    // --- حالة داخلية ---
    float hoverTimer = 0.0f;
    bool isHovering = false;

    void show() { isVisible = true; }
    void hide() { isVisible = false; }

    void update(float dt) {
        if (isHovering) {
            hoverTimer += dt * 1000; // تحويل لملي ثانية
            if (hoverTimer >= showDelay) isVisible = true;
        }
    }

    void render(Renderer2D& renderer) override {
        if (child) child->render(renderer);

        if (!isVisible || message.empty()) return;

        // حساب حجم التلميح
        float tw = message.length() * fontSize * 0.6f + padding * 2;
        if (tw > maxWidth) tw = maxWidth;
        float th = fontSize + padding * 2;

        // حساب الموضع
        float tx = position.x + size.x / 2 - tw / 2;
        float ty = position.y;
        switch (preferredPosition) {
            case TooltipPosition::Above: ty -= th - 4; break;
            case TooltipPosition::Below: ty += size.y + 4; break;
            case TooltipPosition::Left: tx = position.x - tw - 4; ty = position.y; break;
            case TooltipPosition::Right: tx = position.x + size.x + 4; ty = position.y; break;
        }

        renderer.fillRect(tx, ty, tw, th, backgroundColor);
        renderer.drawText(message, tx + padding, ty + padding, textColor);
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEMOTION) {
            Vec2 mouse((float)event.motion.x, (float)event.motion.y);
            bool wasHovering = isHovering;
            isHovering = (mouse.x >= position.x && mouse.x <= position.x + size.x &&
                         mouse.y >= position.y && mouse.y <= position.y + size.y);
            if (!isHovering) {
                isVisible = false;
                hoverTimer = 0;
            }
            if (isHovering && !wasHovering) hoverTimer = 0;
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 6. شريط التقدم - ProgressBar
// ═══════════════════════════════════════════════════════════════

/**
 * @class ProgressBar
 * @brief شريط تقدم أفقي - لعرض نسبة الاكتمال
 * 
 * يوفر:
 * - تقدم محدد (نسبة معروفة)
 * - تقدم غير محدد (تحريك مستمر)
 * - ألوان متدرجة
 * - نص النسبة اختياري
 */
class ProgressBar : public Widget {
public:
    float value = 0.0f;            // القيمة (0.0 - 1.0)
    float minValue = 0.0f;
    float maxValue = 1.0f;
    bool indeterminate = false;     // تقدم غير محدد
    Color backgroundColor = Color(0.9f, 0.9f, 0.9f, 1);
    Color progressColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color bufferColor = Color(0.7f, 0.7f, 0.9f, 1);
    float height = 4.0f;
    float borderRadius = 2.0f;
    bool showLabel = false;         // إظهار نص النسبة
    float bufferValue = 0.0f;      // قيمة التخزين المؤقت
    float animationSpeed = 2.0f;

    // --- حالة التحريك ---
    float indeterminateOffset = 0.0f;

    float getProgress() const {
        if (maxValue <= minValue) return 0;
        return (value - minValue) / (maxValue - minValue);
    }

    void update(float dt) {
        if (indeterminate) {
            indeterminateOffset += dt * animationSpeed;
            if (indeterminateOffset > 2.0f) indeterminateOffset = -1.0f;
        }
    }

    void render(Renderer2D& renderer) override {
        // رسم الخلفية
        renderer.fillRect(position.x, position.y, size.x, height, backgroundColor);

        if (indeterminate) {
            // شريط متحرك
            float barWidth = size.x * 0.3f;
            float barX = position.x + indeterminateOffset * size.x;
            renderer.fillRect(barX, position.y, barWidth, height, progressColor);
        } else {
            // شريط التخزين المؤقت
            if (bufferValue > 0) {
                float bw = size.x * bufferValue;
                renderer.fillRect(position.x, position.y, bw, height, bufferColor);
            }

            // شريط التقدم
            float pw = size.x * getProgress();
            renderer.fillRect(position.x, position.y, pw, height, progressColor);

            // نص النسبة
            if (showLabel) {
                int percent = (int)(getProgress() * 100);
                renderer.drawText(std::to_string(percent) + "%",
                                 position.x + size.x + 8, position.y,
                                 Color(0.3f, 0.3f, 0.3f, 1));
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 7. مؤشر التحميل الدائري - CircularProgress
// ═══════════════════════════════════════════════════════════════

/**
 * @class CircularProgress
 * @brief مؤشر تحميل دائري - لعرض التحميل أو النسبة
 * 
 * يوفر:
 * - تحميل محدد وغير محدد
 * - حجم وسمك قابلين للتخصيص
 * - نص مركزي اختياري
 * - ألوان متدرجة
 */
class CircularProgress : public Widget {
public:
    float value = 0.0f;            // القيمة (0.0 - 1.0)
    bool indeterminate = false;
    float radius = 20.0f;
    float strokeWidth = 4.0f;      // سمك الحلقة
    Color progressColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color backgroundColor = Color(0.9f, 0.9f, 0.9f, 1);
    bool showLabel = false;
    std::string centerText;        // نص مركزي
    Color textColor = Color(0.3f, 0.3f, 0.3f, 1);
    float rotation = 0.0f;        // زاوية البدء
    float animSpeed = 3.0f;

    void update(float dt) {
        if (indeterminate) {
            rotation += dt * animSpeed * 360;
            if (rotation >= 360) rotation -= 360;
        }
    }

    void render(Renderer2D& renderer) override {
        float cx = position.x + radius;
        float cy = position.y + radius;

        // رسم حلقة الخلفية
        renderer.drawRect(cx - radius, cy - radius,
                         radius * 2, radius * 2, backgroundColor);

        // رسم النص المركزي
        if (showLabel || !centerText.empty()) {
            std::string text = centerText.empty() ?
                std::to_string((int)(value * 100)) + "%" : centerText;
            renderer.drawText(text, cx, cy, textColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 8. الهيكل العظمي - Skeleton (تأثير التحميل)
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع شكل الهيكل العظمي
 */
enum class SkeletonShape {
    Rectangle,  // مستطيل
    Circle,     // دائرة
    Text,       // خط نصي
    Card,       // بطاقة
    Avatar,     // صورة رمزية
    Paragraph   // فقرة (عدة أسطر)
};

/**
 * @class Skeleton
 * @brief هيكل عظمي للتحميل - يعرض شكل المحتوى أثناء التحميل
 * 
 * يوفر:
 * - أشكال متعددة (مستطيل، دائرة، نص)
 * - تأثير وميض أو موجة
 * - قوالب جاهزة (بطاقة، قائمة)
 * - قابل للتخصيص بالكامل
 */
class Skeleton : public Widget {
public:
    SkeletonShape shape = SkeletonShape::Rectangle;
    Color baseColor = Color(0.9f, 0.9f, 0.9f, 1);
    Color highlightColor = Color(0.95f, 0.95f, 0.95f, 1);
    float borderRadius = 4.0f;
    bool animate = true;
    float animSpeed = 1.5f;        // سرعة التحريك

    // --- للفقرة ---
    int lineCount = 3;             // عدد الأسطر
    float lineHeight = 16.0f;
    float lineSpacing = 8.0f;
    float lastLineWidth = 0.6f;    // عرض السطر الأخير (نسبة)

    // --- حالة التحريك ---
    float animProgress = 0.0f;

    void update(float dt) {
        if (animate) {
            animProgress += dt * animSpeed;
            if (animProgress > 1.0f) animProgress -= 1.0f;
        }
    }

    void render(Renderer2D& renderer) override {
        // حساب لون الوميض
        float t = (std::sin(animProgress * 6.28f) + 1.0f) / 2.0f;
        Color color;
        color.r = baseColor.r + (highlightColor.r - baseColor.r) * t;
        color.g = baseColor.g + (highlightColor.g - baseColor.g) * t;
        color.b = baseColor.b + (highlightColor.b - baseColor.b) * t;
        color.a = 1.0f;

        switch (shape) {
            case SkeletonShape::Rectangle:
            case SkeletonShape::Card:
                renderer.fillRect(position.x, position.y, size.x, size.y, color);
                break;
            case SkeletonShape::Circle:
            case SkeletonShape::Avatar: {
                float r = std::min(size.x, size.y) / 2;
                renderer.fillRect(position.x, position.y, r * 2, r * 2, color);
                break;
            }
            case SkeletonShape::Text:
                renderer.fillRect(position.x, position.y, size.x, lineHeight, color);
                break;
            case SkeletonShape::Paragraph:
                for (int i = 0; i < lineCount; i++) {
                    float w = (i == lineCount - 1) ? size.x * lastLineWidth : size.x;
                    float y = position.y + i * (lineHeight + lineSpacing);
                    renderer.fillRect(position.x, y, w, lineHeight, color);
                }
                break;
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 9. الفاصل - Divider
// ═══════════════════════════════════════════════════════════════

/**
 * @class Divider
 * @brief فاصل بصري - خط فاصل بين المحتويات
 * 
 * يوفر:
 * - فاصل أفقي ورأسي
 * - سمك ولون قابلين للتخصيص
 * - نص وسطي اختياري
 * - حشو داخلي
 */
class Divider : public Widget {
public:
    bool isVertical = false;        // عمودي أم أفقي
    Color color = Color(0.85f, 0.85f, 0.85f, 1);
    float thickness = 1.0f;
    float indent = 0.0f;           // حشو من البداية
    float endIndent = 0.0f;        // حشو من النهاية
    std::string label;             // نص وسطي اختياري
    Color labelColor = Color(0.5f, 0.5f, 0.5f, 1);
    float labelFontSize = 12.0f;

    void render(Renderer2D& renderer) override {
        if (isVertical) {
            float x = position.x + size.x / 2;
            renderer.fillRect(x, position.y + indent, thickness,
                            size.y - indent - endIndent, color);
        } else {
            float y = position.y + size.y / 2;
            if (label.empty()) {
                renderer.fillRect(position.x + indent, y,
                                size.x - indent - endIndent, thickness, color);
            } else {
                // رسم خط - نص - خط
                float textW = label.length() * labelFontSize * 0.6f;
                float halfLine = (size.x - textW - 16) / 2;
                renderer.fillRect(position.x + indent, y, halfLine, thickness, color);
                renderer.drawText(label, position.x + halfLine + 8, y, labelColor);
                renderer.fillRect(position.x + halfLine + textW + 16, y,
                                halfLine - endIndent, thickness, color);
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 10. عنصر القائمة - ListTile
// ═══════════════════════════════════════════════════════════════

/**
 * @class ListTile
 * @brief عنصر قائمة - سطر واحد يحتوي على أيقونة وعنوان ونص فرعي
 * 
 * يوفر:
 * - أيقونة بداية ونهاية
 * - عنوان ونص فرعي
 * - قابل للنقر والتحديد
 * - كثافة عرض قابلة للتخصيص
 * - فاصل سفلي اختياري
 */
class ListTile : public Widget {
public:
    std::string title;              // العنوان الرئيسي
    std::string subtitle;           // النص الفرعي
    std::string leadingIcon;        // أيقونة البداية
    std::string trailingIcon;       // أيقونة النهاية
    std::string trailingText;       // نص النهاية (بدل أيقونة)
    bool selected = false;
    bool enabled = true;
    bool dense = false;             // كثافة عالية (أصغر)
    bool isThreeLine = false;       // ثلاثة أسطر
    Color selectedColor = Color(0.88f, 0.88f, 1.0f, 1);
    Color tileColor = Color(1, 1, 1, 0);
    Color titleColor = Color(0.1f, 0.1f, 0.1f, 1);
    Color subtitleColor = Color(0.5f, 0.5f, 0.5f, 1);
    float height = 56.0f;          // ارتفاع (56 عادي، 72 بنص فرعي)
    EdgeInsets contentPadding = EdgeInsets(16, 0, 16, 0);
    bool showDivider = false;

    // --- المركبات المخصصة ---
    std::shared_ptr<Widget> leading;   // مركب بداية مخصص (بدل أيقونة)
    std::shared_ptr<Widget> trailing;  // مركب نهاية مخصص

    // --- الأحداث ---
    std::function<void()> onTap;
    std::function<void()> onLongPress;

    void render(Renderer2D& renderer) override {
        // تحديد الارتفاع
        float h = dense ? 48.0f : (subtitle.empty() ? 56.0f : 72.0f);

        // رسم الخلفية
        Color bg = selected ? selectedColor : tileColor;
        if (bg.a > 0) renderer.fillRect(position.x, position.y, size.x, h, bg);

        // رسم العنوان
        float textX = position.x + contentPadding.left + (leadingIcon.empty() ? 0 : 56);
        renderer.drawText(title, textX, position.y + (subtitle.empty() ? h / 2 : h / 3),
                         titleColor);

        // رسم النص الفرعي
        if (!subtitle.empty()) {
            renderer.drawText(subtitle, textX, position.y + h * 2 / 3, subtitleColor);
        }

        // رسم الفاصل
        if (showDivider) {
            renderer.fillRect(position.x + contentPadding.left, position.y + h - 1,
                            size.x - contentPadding.left, 1,
                            Color(0.9f, 0.9f, 0.9f, 1));
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (enabled && event.type == SDL_MOUSEBUTTONDOWN && onTap) {
            onTap();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 11. بطاقة قابلة للتوسع - ExpansionTile
// ═══════════════════════════════════════════════════════════════

/**
 * @class ExpansionTile
 * @brief بطاقة قابلة للطي والتوسع - تعرض محتوى إضافي عند النقر
 * 
 * يوفر:
 * - عنوان ونص فرعي
 * - محتوى يظهر/يختفي بسلاسة
 * - أيقونة سهم متحركة
 * - مجموعة توسع (واحد فقط مفتوح)
 */
class ExpansionTile : public Widget {
public:
    std::string title;
    std::string subtitle;
    std::string leadingIcon;
    bool isExpanded = false;
    bool initiallyExpanded = false;
    Color backgroundColor = Color(1, 1, 1, 1);
    Color collapsedBackgroundColor = Color(1, 1, 1, 1);
    Color expandedBackgroundColor = Color(0.98f, 0.98f, 0.98f, 1);
    Color iconColor = Color(0.5f, 0.5f, 0.5f, 1);
    Color textColor = Color(0.1f, 0.1f, 0.1f, 1);
    float headerHeight = 56.0f;
    EdgeInsets childrenPadding = EdgeInsets(16, 0, 16, 16);

    // --- المحتويات ---
    std::vector<std::shared_ptr<Widget>> children;

    // --- التحريك ---
    float expandProgress = 0.0f;   // 0 = مغلق، 1 = مفتوح
    float animSpeed = 5.0f;

    // --- مجموعة التوسع ---
    int groupId = -1;              // -1 = بدون مجموعة

    // --- الأحداث ---
    std::function<void(bool)> onExpansionChanged;

    void toggle() {
        isExpanded = !isExpanded;
        if (onExpansionChanged) onExpansionChanged(isExpanded);
    }

    void expand() { isExpanded = true; expandProgress = 1.0f; }
    void collapse() { isExpanded = false; expandProgress = 0.0f; }

    void update(float dt) {
        float target = isExpanded ? 1.0f : 0.0f;
        if (expandProgress < target)
            expandProgress = std::min(expandProgress + dt * animSpeed, 1.0f);
        else if (expandProgress > target)
            expandProgress = std::max(expandProgress - dt * animSpeed, 0.0f);
    }

    void render(Renderer2D& renderer) override {
        Color bg = isExpanded ? expandedBackgroundColor : collapsedBackgroundColor;
        float totalH = headerHeight + expandProgress * 100; // تقدير المحتوى

        renderer.fillRect(position.x, position.y, size.x, totalH, bg);
        renderer.drawText(title, position.x + 16, position.y + headerHeight / 2, textColor);

        // رسم السهم
        std::string arrow = isExpanded ? "▲" : "▼";
        renderer.drawText(arrow, position.x + size.x - 40,
                         position.y + headerHeight / 2, iconColor);
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float my = (float)event.button.y;
            if (my >= position.y && my <= position.y + headerHeight) {
                toggle();
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 12. جدول البيانات - DataTable
// ═══════════════════════════════════════════════════════════════

/**
 * @class DataTable
 * @brief جدول بيانات تفاعلي - لعرض البيانات المهيكلة
 * 
 * يوفر:
 * - أعمدة قابلة للترتيب
 * - صفوف قابلة للتحديد
 * - ترقيم الصفحات
 * - تمرير أفقي ورأسي
 * - تحرير مباشر للخلايا
 */
class DataTable : public Widget {
public:
    struct Column {
        std::string label;          // عنوان العمود
        float width = 100.0f;      // عرض العمود
        bool sortable = false;     // قابل للترتيب
        bool resizable = true;     // قابل لتغيير الحجم
        enum class Align { Left, Center, Right } align = Align::Left;
    };

    struct Cell {
        std::string text;
        Color textColor = Color(0.2f, 0.2f, 0.2f, 1);
        bool editable = false;
        std::shared_ptr<Widget> customWidget; // مركب مخصص
    };

    struct Row {
        std::vector<Cell> cells;
        bool selected = false;
        Color color = Color(1, 1, 1, 0); // شفاف = تبديل ألوان
        std::function<void()> onTap;
    };

    std::vector<Column> columns;
    std::vector<Row> rows;

    // --- الترتيب ---
    int sortColumnIndex = -1;
    bool sortAscending = true;

    // --- التحديد ---
    bool selectable = false;
    bool multiSelect = false;
    std::vector<int> selectedRows;

    // --- الترقيم ---
    bool paginationEnabled = false;
    int rowsPerPage = 10;
    int currentPage = 0;

    // --- المظهر ---
    Color headerColor = Color(0.96f, 0.96f, 0.96f, 1);
    Color headerTextColor = Color(0.3f, 0.3f, 0.3f, 1);
    Color alternateRowColor = Color(0.98f, 0.98f, 0.98f, 1);
    Color selectedRowColor = Color(0.88f, 0.88f, 1.0f, 1);
    Color borderColor = Color(0.9f, 0.9f, 0.9f, 1);
    float rowHeight = 48.0f;
    float headerHeight = 56.0f;
    bool showBorder = true;
    bool showCheckboxes = false;

    // --- الأحداث ---
    std::function<void(int, bool)> onSort;          // عمود، تصاعدي
    std::function<void(int)> onRowTap;
    std::function<void(std::vector<int>)> onSelectionChanged;

    void addColumn(const std::string& label, float width = 100.0f, bool sortable = false) {
        columns.push_back({label, width, sortable, true, Column::Align::Left});
    }

    void addRow(const std::vector<std::string>& cellTexts) {
        Row row;
        for (const auto& text : cellTexts) {
            row.cells.push_back({text});
        }
        rows.push_back(row);
    }

    int getTotalPages() const {
        if (!paginationEnabled || rowsPerPage <= 0) return 1;
        return ((int)rows.size() + rowsPerPage - 1) / rowsPerPage;
    }

    void render(Renderer2D& renderer) override {
        float x = position.x;
        float y = position.y;

        // رسم رأس الجدول
        renderer.fillRect(x, y, size.x, headerHeight, headerColor);
        float colX = x;
        for (const auto& col : columns) {
            renderer.drawText(col.label, colX + 16, y + headerHeight / 2, headerTextColor);
            colX += col.width;
        }
        y += headerHeight;

        // رسم الصفوف
        int startRow = paginationEnabled ? currentPage * rowsPerPage : 0;
        int endRow = paginationEnabled ?
            std::min(startRow + rowsPerPage, (int)rows.size()) : (int)rows.size();

        for (int i = startRow; i < endRow; i++) {
            Color rowBg = rows[i].selected ? selectedRowColor :
                         (i % 2 == 0 ? Color(1, 1, 1, 1) : alternateRowColor);
            renderer.fillRect(x, y, size.x, rowHeight, rowBg);

            colX = x;
            for (size_t j = 0; j < rows[i].cells.size() && j < columns.size(); j++) {
                renderer.drawText(rows[i].cells[j].text, colX + 16,
                                y + rowHeight / 2, rows[i].cells[j].textColor);
                colX += columns[j].width;
            }

            if (showBorder) {
                renderer.fillRect(x, y + rowHeight - 1, size.x, 1, borderColor);
            }
            y += rowHeight;
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float my = (float)event.button.y;
            if (my > position.y + headerHeight) {
                int row = (int)((my - position.y - headerHeight) / rowHeight);
                int startRow = paginationEnabled ? currentPage * rowsPerPage : 0;
                row += startRow;
                if (row >= 0 && row < (int)rows.size()) {
                    if (selectable) rows[row].selected = !rows[row].selected;
                    if (onRowTap) onRowTap(row);
                }
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 13. العرض الدوار - Carousel
// ═══════════════════════════════════════════════════════════════

/**
 * @class Carousel
 * @brief عرض دوار للمحتوى - للصور والبطاقات والإعلانات
 * 
 * يوفر:
 * - تمرير أفقي دوار
 * - تمرير تلقائي
 * - مؤشر نقاط
 * - تأثيرات تحجيم (العنصر المركزي أكبر)
 * - دعم لمس وسحب
 */
class Carousel : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> items; // عناصر العرض
    int currentIndex = 0;
    float itemWidth = 300.0f;      // عرض العنصر
    float itemSpacing = 16.0f;     // المسافة بين العناصر
    float viewportFraction = 0.8f; // نسبة العنصر من العرض
    bool autoPlay = false;
    float autoPlayInterval = 3.0f; // ثوان
    bool loop = true;              // تكرار لا نهائي
    bool enlargeCenter = true;     // تكبير العنصر المركزي
    float enlargeScale = 1.2f;     // نسبة التكبير

    // --- المؤشر ---
    bool showIndicator = true;
    Color activeIndicatorColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color inactiveIndicatorColor = Color(0.7f, 0.7f, 0.7f, 1);
    float indicatorSize = 8.0f;

    // --- حالة داخلية ---
    float scrollOffset = 0.0f;
    float autoPlayTimer = 0.0f;

    // --- الأحداث ---
    std::function<void(int)> onPageChanged;

    void addItem(std::shared_ptr<Widget> item) {
        items.push_back(item);
    }

    void goTo(int index) {
        if (items.empty()) return;
        if (loop) {
            currentIndex = ((index % (int)items.size()) + (int)items.size()) % (int)items.size();
        } else {
            currentIndex = std::max(0, std::min(index, (int)items.size() - 1));
        }
        if (onPageChanged) onPageChanged(currentIndex);
    }

    void next() { goTo(currentIndex + 1); }
    void previous() { goTo(currentIndex - 1); }

    void update(float dt) {
        if (autoPlay) {
            autoPlayTimer += dt;
            if (autoPlayTimer >= autoPlayInterval) {
                autoPlayTimer = 0;
                next();
            }
        }
    }

    void render(Renderer2D& renderer) override {
        // رسم العنصر الحالي
        if (currentIndex >= 0 && currentIndex < (int)items.size()) {
            items[currentIndex]->render(renderer);
        }

        // رسم المؤشر
        if (showIndicator && items.size() > 1) {
            float totalW = items.size() * indicatorSize + (items.size() - 1) * 6;
            float sx = position.x + (size.x - totalW) / 2;
            float sy = position.y + size.y - 24;

            for (size_t i = 0; i < items.size(); i++) {
                Color c = (i == (size_t)currentIndex) ?
                    activeIndicatorColor : inactiveIndicatorColor;
                renderer.fillRect(sx, sy, indicatorSize, indicatorSize, c);
                sx += indicatorSize + 6;
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 14. الخط الزمني - Timeline
// ═══════════════════════════════════════════════════════════════

/**
 * @class Timeline
 * @brief خط زمني عمودي - لعرض أحداث مرتبة زمنياً
 * 
 * يوفر:
 * - أحداث مع تاريخ ونص
 * - أيقونات وألوان مخصصة لكل حدث
 * - خط رابط بين الأحداث
 * - اتجاهات متبادلة (يمين/يسار)
 */
class Timeline : public Widget {
public:
    /**
     * @brief عنصر في الخط الزمني
     */
    struct TimelineItem {
        std::string title;          // العنوان
        std::string description;    // الوصف
        std::string time;           // الوقت/التاريخ
        std::string icon;           // أيقونة
        Color dotColor = Color(0.25f, 0.32f, 0.71f, 1); // لون النقطة
        Color contentColor = Color(1, 1, 1, 1);
        bool isFirst = false;
        bool isLast = false;
        std::shared_ptr<Widget> customContent; // محتوى مخصص
    };

    std::vector<TimelineItem> items;
    Color lineColor = Color(0.8f, 0.8f, 0.8f, 1);
    float lineWidth = 2.0f;
    float dotRadius = 8.0f;
    float itemSpacing = 24.0f;
    float itemHeight = 80.0f;
    bool alternate = false;        // تبديل الاتجاه (يمين/يسار)
    float lineOffsetX = 40.0f;     // موضع الخط من اليسار

    void addItem(const std::string& title, const std::string& desc,
                const std::string& time, Color dotColor = Color(0.25f, 0.32f, 0.71f, 1)) {
        bool first = items.empty();
        if (!items.empty()) items.back().isLast = false;
        items.push_back({title, desc, time, "", dotColor, Color(1,1,1,1),
                        first, true, nullptr});
    }

    void render(Renderer2D& renderer) override {
        float y = position.y;

        for (size_t i = 0; i < items.size(); i++) {
            float lineX = position.x + lineOffsetX;

            // رسم الخط (ما عدا قبل الأول وبعد الأخير)
            if (!items[i].isFirst) {
                renderer.fillRect(lineX - lineWidth / 2, y - itemSpacing,
                                lineWidth, itemSpacing, lineColor);
            }
            if (!items[i].isLast) {
                renderer.fillRect(lineX - lineWidth / 2, y + dotRadius * 2,
                                lineWidth, itemHeight - dotRadius * 2, lineColor);
            }

            // رسم النقطة
            renderer.fillRect(lineX - dotRadius, y,
                            dotRadius * 2, dotRadius * 2, items[i].dotColor);

            // رسم المحتوى
            float contentX = lineX + dotRadius * 2 + 16;
            renderer.drawText(items[i].title, contentX, y,
                            Color(0.1f, 0.1f, 0.1f, 1));
            renderer.drawText(items[i].description, contentX, y + 20,
                            Color(0.5f, 0.5f, 0.5f, 1));
            renderer.drawText(items[i].time, position.x, y,
                            Color(0.4f, 0.4f, 0.4f, 1));

            y += itemHeight + itemSpacing;
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 15. حالة فارغة - EmptyState
// ═══════════════════════════════════════════════════════════════

/**
 * @class EmptyState
 * @brief شاشة الحالة الفارغة - تعرض عند عدم وجود محتوى
 * 
 * يوفر:
 * - أيقونة أو صورة كبيرة
 * - عنوان ووصف
 * - زر إجراء رئيسي
 * - زر إجراء ثانوي اختياري
 * - مثالي لحالات: لا نتائج، لا إنترنت، خطأ
 */
class EmptyState : public Widget {
public:
    std::string icon;               // أيقونة كبيرة
    std::string imagePath;          // صورة بديلة
    std::string title;              // العنوان
    std::string description;        // الوصف
    std::string actionLabel;        // نص زر الإجراء
    std::string secondaryLabel;     // نص الزر الثانوي
    Color iconColor = Color(0.6f, 0.6f, 0.6f, 1);
    Color titleColor = Color(0.2f, 0.2f, 0.2f, 1);
    Color descColor = Color(0.5f, 0.5f, 0.5f, 1);
    Color actionColor = Color(0.25f, 0.32f, 0.71f, 1);
    float iconSize = 64.0f;
    float titleFontSize = 20.0f;
    float descFontSize = 14.0f;
    float spacing = 16.0f;

    // --- الأحداث ---
    std::function<void()> onAction;
    std::function<void()> onSecondaryAction;

    void render(Renderer2D& renderer) override {
        float cx = position.x + size.x / 2;
        float y = position.y + size.y / 3;

        // رسم الأيقونة
        // (في التطبيق الفعلي ستكون أيقونة حقيقية)
        renderer.fillRect(cx - iconSize / 2, y, iconSize, iconSize, iconColor);
        y += iconSize + spacing;

        // رسم العنوان
        if (!title.empty()) {
            renderer.drawText(title, cx, y, titleColor);
            y += titleFontSize + spacing / 2;
        }

        // رسم الوصف
        if (!description.empty()) {
            renderer.drawText(description, cx, y, descColor);
            y += descFontSize + spacing;
        }

        // رسم زر الإجراء
        if (!actionLabel.empty()) {
            float btnW = actionLabel.length() * 10 + 32;
            float btnH = 40;
            renderer.fillRect(cx - btnW / 2, y, btnW, btnH, actionColor);
            renderer.drawText(actionLabel, cx, y + btnH / 2, Color(1, 1, 1, 1));
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN && onAction) {
            onAction();
        }
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad
