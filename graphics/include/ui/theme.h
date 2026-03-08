/**
 * =============================================================================
 * ملف: theme.h
 * الوصف: نظام السمات — فاتح/داكن + ألوان مخصصة
 * (AR) @brief نظام سمات مع سمة فاتحة وداكنة وقابلية التخصيص
 * (EN) @brief Theme system with Light/Dark themes and custom colors
 * المهمة: T309 - كامل: Theme system
 * =============================================================================
 */

#ifndef GRAPHICS_UI_THEME_H
#define GRAPHICS_UI_THEME_H

#include "widget.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace sad {
namespace graphics {
namespace ui {

// ============================================================================
// ألوان السمة / Theme Colors
// ============================================================================

struct ThemeColors {
    // ألوان أساسية / Primary colors
    Color primary;             ///< اللون الأساسي
    Color primaryLight;        ///< اللون الأساسي الفاتح
    Color primaryDark;         ///< اللون الأساسي الداكن
    Color accent;              ///< لون التأكيد
    
    // خلفيات / Backgrounds
    Color background;          ///< خلفية التطبيق
    Color surface;             ///< سطح البطاقات
    Color surfaceVariant;      ///< سطح بديل
    
    // نصوص / Text
    Color textPrimary;         ///< نص أساسي
    Color textSecondary;       ///< نص ثانوي
    Color textDisabled;        ///< نص معطّل
    Color textOnPrimary;       ///< نص فوق اللون الأساسي
    
    // حدود / Borders
    Color border;              ///< حدود عادية
    Color borderFocused;       ///< حدود مركّز
    
    // حالات / States
    Color hover;               ///< تحويم
    Color pressed;             ///< ضغط
    Color selected;            ///< محدد
    Color disabled;            ///< معطّل
    
    // نظام / System
    Color error;               ///< خطأ
    Color warning;             ///< تحذير
    Color success;             ///< نجاح
    Color info;                ///< معلومة
    
    // ظلال / Shadows
    Color shadow;              ///< ظل
    Color overlay;             ///< طبقة تغطية
};

// ============================================================================
// قيم التنسيق / Theme Metrics
// ============================================================================

struct ThemeMetrics {
    float borderRadius = 4.0f;       ///< نصف قطر الزوايا
    float borderWidth = 1.0f;        ///< عرض الحدود
    float padding = 8.0f;            ///< حشوة
    float spacing = 4.0f;            ///< تباعد
    float fontSize = 14.0f;          ///< حجم الخط
    float fontSizeSmall = 12.0f;     ///< حجم خط صغير
    float fontSizeLarge = 18.0f;     ///< حجم خط كبير
    float fontSizeTitle = 24.0f;     ///< حجم عنوان
    float buttonHeight = 36.0f;      ///< ارتفاع الزر
    float inputHeight = 32.0f;       ///< ارتفاع حقل الإدخال
    float headerHeight = 48.0f;      ///< ارتفاع الرأس
    float iconSize = 24.0f;          ///< حجم الأيقونة
    float shadowRadius = 4.0f;       ///< نصف قطر الظل
    float opacity = 1.0f;            ///< الشفافية
};

// ============================================================================
// السمة / Theme
// ============================================================================

class Theme {
public:
    Theme(const std::string& name = "مخصص");
    
    const std::string& getName() const { return name_; }
    
    const ThemeColors& getColors() const { return colors_; }
    ThemeColors& getColors() { return colors_; }
    void setColors(const ThemeColors& c) { colors_ = c; }
    
    const ThemeMetrics& getMetrics() const { return metrics_; }
    ThemeMetrics& getMetrics() { return metrics_; }
    void setMetrics(const ThemeMetrics& m) { metrics_ = m; }
    
    /**
     * (AR) تطبيق السمة على عنصر
     * (EN) Apply theme to widget
     */
    void applyTo(Widget* widget) const;
    
    /**
     * (AR) السمة الفاتحة المدمجة
     * (EN) Built-in light theme
     */
    static Theme lightTheme();
    
    /**
     * (AR) السمة الداكنة المدمجة
     * (EN) Built-in dark theme
     */
    static Theme darkTheme();

private:
    std::string name_;
    ThemeColors colors_;
    ThemeMetrics metrics_;
};

// ============================================================================
// مدير السمات / Theme Manager
// ============================================================================

class ThemeManager {
public:
    static ThemeManager& getInstance();
    
    /**
     * (AR) تسجيل سمة
     */
    void registerTheme(const std::string& name, const Theme& theme);
    
    /**
     * (AR) تعيين السمة النشطة
     */
    void setActiveTheme(const std::string& name);
    
    /**
     * (AR) الحصول على السمة النشطة
     */
    const Theme& getActiveTheme() const;
    
    /**
     * (AR) الحصول على سمة بالاسم
     */
    const Theme* getTheme(const std::string& name) const;
    
    /**
     * (AR) تطبيق السمة النشطة على جميع الأبناء
     */
    void applyToTree(Widget* root);
    
    /**
     * (AR) مستمع تغيير السمة
     */
    using ThemeChangeCallback = std::function<void(const Theme& newTheme)>;
    void onThemeChanged(ThemeChangeCallback cb) { onChanged_ = cb; }
    
    /**
     * (AR) قائمة السمات المسجلة
     */
    std::vector<std::string> getThemeNames() const;

private:
    ThemeManager();
    
    std::unordered_map<std::string, Theme> themes_;
    std::string activeThemeName_;
    ThemeChangeCallback onChanged_;
};

} // namespace ui
} // namespace graphics
} // namespace sad

#endif // GRAPHICS_UI_THEME_H
