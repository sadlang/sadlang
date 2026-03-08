/**
 * =============================================================================
 * ملف: theme.cpp
 * الوصف: تنفيذ نظام السمات — فاتح/داكن + مخصص
 * (AR) @brief تنفيذ سمات فاتحة وداكنة ومدير السمات
 * (EN) @brief Light/Dark theme implementations and ThemeManager
 * المهمة: T309 - كامل: Theme system
 * =============================================================================
 */

#include "../include/ui/theme.h"

namespace sad {
namespace graphics {
namespace ui {

// ============================================================================
// Theme
// ============================================================================

Theme::Theme(const std::string& name) : name_(name) {}

void Theme::applyTo(Widget* widget) const {
    if (!widget) return;
    
    WidgetStyle style;
    style.backgroundColor = colors_.surface;
    style.foregroundColor = colors_.textPrimary;
    style.borderColor = colors_.border;
    style.borderWidth = metrics_.borderWidth;
    style.borderRadius = metrics_.borderRadius;
    style.padding = metrics_.padding;
    style.opacity = metrics_.opacity;
    widget->SetStyle(style);
}

Theme Theme::lightTheme() {
    Theme theme("فاتح");
    auto& c = theme.colors_;
    
    // ألوان أساسية
    c.primary       = Color(0.13f, 0.59f, 0.95f, 1.0f);  // أزرق
    c.primaryLight  = Color(0.39f, 0.71f, 0.97f, 1.0f);
    c.primaryDark   = Color(0.07f, 0.40f, 0.74f, 1.0f);
    c.accent        = Color(1.0f, 0.34f, 0.13f, 1.0f);    // برتقالي
    
    // خلفيات
    c.background    = Color(0.96f, 0.96f, 0.96f, 1.0f);   // رمادي فاتح
    c.surface       = Color(1.0f, 1.0f, 1.0f, 1.0f);      // أبيض
    c.surfaceVariant= Color(0.93f, 0.93f, 0.93f, 1.0f);
    
    // نصوص
    c.textPrimary   = Color(0.13f, 0.13f, 0.13f, 1.0f);   // أسود تقريباً
    c.textSecondary = Color(0.46f, 0.46f, 0.46f, 1.0f);
    c.textDisabled  = Color(0.62f, 0.62f, 0.62f, 1.0f);
    c.textOnPrimary = Color(1.0f, 1.0f, 1.0f, 1.0f);      // أبيض
    
    // حدود
    c.border        = Color(0.79f, 0.79f, 0.79f, 1.0f);
    c.borderFocused = Color(0.13f, 0.59f, 0.95f, 1.0f);
    
    // حالات
    c.hover         = Color(0.0f, 0.0f, 0.0f, 0.04f);
    c.pressed       = Color(0.0f, 0.0f, 0.0f, 0.08f);
    c.selected      = Color(0.13f, 0.59f, 0.95f, 0.12f);
    c.disabled      = Color(0.0f, 0.0f, 0.0f, 0.12f);
    
    // نظام
    c.error         = Color(0.83f, 0.18f, 0.18f, 1.0f);   // أحمر
    c.warning       = Color(0.93f, 0.69f, 0.13f, 1.0f);   // أصفر
    c.success       = Color(0.30f, 0.69f, 0.31f, 1.0f);   // أخضر
    c.info          = Color(0.13f, 0.59f, 0.95f, 1.0f);   // أزرق
    
    // ظلال
    c.shadow        = Color(0.0f, 0.0f, 0.0f, 0.15f);
    c.overlay       = Color(0.0f, 0.0f, 0.0f, 0.50f);
    
    return theme;
}

Theme Theme::darkTheme() {
    Theme theme("داكن");
    auto& c = theme.colors_;
    
    // ألوان أساسية
    c.primary       = Color(0.33f, 0.67f, 0.93f, 1.0f);   // أزرق فاتح
    c.primaryLight  = Color(0.53f, 0.78f, 0.96f, 1.0f);
    c.primaryDark   = Color(0.16f, 0.47f, 0.76f, 1.0f);
    c.accent        = Color(1.0f, 0.49f, 0.33f, 1.0f);    // برتقالي فاتح
    
    // خلفيات
    c.background    = Color(0.12f, 0.12f, 0.12f, 1.0f);   // رمادي داكن
    c.surface       = Color(0.18f, 0.18f, 0.18f, 1.0f);   // رمادي أغمق
    c.surfaceVariant= Color(0.22f, 0.22f, 0.22f, 1.0f);
    
    // نصوص
    c.textPrimary   = Color(0.93f, 0.93f, 0.93f, 1.0f);   // أبيض تقريباً
    c.textSecondary = Color(0.65f, 0.65f, 0.65f, 1.0f);
    c.textDisabled  = Color(0.45f, 0.45f, 0.45f, 1.0f);
    c.textOnPrimary = Color(0.0f, 0.0f, 0.0f, 1.0f);      // أسود
    
    // حدود
    c.border        = Color(0.33f, 0.33f, 0.33f, 1.0f);
    c.borderFocused = Color(0.33f, 0.67f, 0.93f, 1.0f);
    
    // حالات
    c.hover         = Color(1.0f, 1.0f, 1.0f, 0.06f);
    c.pressed       = Color(1.0f, 1.0f, 1.0f, 0.10f);
    c.selected      = Color(0.33f, 0.67f, 0.93f, 0.16f);
    c.disabled      = Color(1.0f, 1.0f, 1.0f, 0.12f);
    
    // نظام
    c.error         = Color(0.94f, 0.33f, 0.31f, 1.0f);
    c.warning       = Color(1.0f, 0.76f, 0.28f, 1.0f);
    c.success       = Color(0.40f, 0.80f, 0.42f, 1.0f);
    c.info          = Color(0.33f, 0.67f, 0.93f, 1.0f);
    
    // ظلال
    c.shadow        = Color(0.0f, 0.0f, 0.0f, 0.30f);
    c.overlay       = Color(0.0f, 0.0f, 0.0f, 0.70f);
    
    auto& m = theme.metrics_;
    m.borderRadius = 6.0f;
    m.shadowRadius = 6.0f;
    
    return theme;
}

// ============================================================================
// ThemeManager
// ============================================================================

ThemeManager::ThemeManager() {
    themes_["فاتح"] = Theme::lightTheme();
    themes_["داكن"] = Theme::darkTheme();
    activeThemeName_ = "فاتح";
}

ThemeManager& ThemeManager::getInstance() {
    static ThemeManager instance;
    return instance;
}

void ThemeManager::registerTheme(const std::string& name, const Theme& theme) {
    themes_[name] = theme;
}

void ThemeManager::setActiveTheme(const std::string& name) {
    auto it = themes_.find(name);
    if (it != themes_.end()) {
        activeThemeName_ = name;
        if (onChanged_) onChanged_(it->second);
    }
}

const Theme& ThemeManager::getActiveTheme() const {
    auto it = themes_.find(activeThemeName_);
    return it->second;
}

const Theme* ThemeManager::getTheme(const std::string& name) const {
    auto it = themes_.find(name);
    return it != themes_.end() ? &it->second : nullptr;
}

void ThemeManager::applyToTree(Widget* root) {
    if (!root) return;
    const auto& theme = getActiveTheme();
    theme.applyTo(root);
    for (const auto& child : root->GetChildren()) {
        applyToTree(child.get());
    }
}

std::vector<std::string> ThemeManager::getThemeNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : themes_) {
        names.push_back(name);
    }
    return names;
}

} // namespace ui
} // namespace graphics
} // namespace sad
