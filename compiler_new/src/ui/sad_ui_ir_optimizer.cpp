/**
 * ==========================================================================
 * ملف: sad_ui_ir_optimizer.cpp
 * الوصف: تنفيذ مُحسّن SadUI IR
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/sad_ui_ir_optimizer.hpp"
#include <algorithm>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// التحسين الرئيسي: شاشة
// ═══════════════════════════════════════════════════════════════════════════

void IROptimizer::optimize(ScreenIR& screen) {
    stats_ = {};
    if (screen.rootNode) {
        screen.rootNode = optimize(screen.rootNode);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// التحسين الرئيسي: عقدة
// ═══════════════════════════════════════════════════════════════════════════

UINodeIRPtr IROptimizer::optimize(UINodeIRPtr node) {
    if (!node) return nullptr;

    // 1. تحسين الأبناء أولاً (bottom-up)
    for (auto& child : node->children) {
        child = optimize(child);
    }

    // حذف الأبناء الفارغين
    node->children.erase(
        std::remove_if(node->children.begin(), node->children.end(),
                        [](const UINodeIRPtr& n) { return n == nullptr; }),
        node->children.end()
    );

    // 2. إزالة العقد المخفية
    if (removeHidden_) {
        node = removeHiddenNodes(node);
        if (!node) return nullptr;
    }

    // 3. إزالة الحاويات الفارغة
    node = removeEmptyContainers(node);
    if (!node) return nullptr;

    // 4. تسطيح المجموعات المتداخلة
    if (flattenGroups_) {
        node = flattenNestedGroups(node);
    }

    // 5. تبسيط ابن وحيد
    node = simplifySingleChild(node);

    return node;
}


// ═══════════════════════════════════════════════════════════════════════════
// التحسين الرئيسي: تطبيق
// ═══════════════════════════════════════════════════════════════════════════

void IROptimizer::optimize(AppIR& app) {
    stats_ = {};
    for (auto& screen : app.screens) {
        optimize(screen);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// إزالة الحاويات الفارغة
// ═══════════════════════════════════════════════════════════════════════════

UINodeIRPtr IROptimizer::removeEmptyContainers(UINodeIRPtr node) {
    if (!node) return nullptr;

    // إذا كانت حاوية فارغة بدون أحداث ولا أنماط
    if (node->isContainer() && node->children.empty()
        && !node->hasEvents() && !node->style.hasAnyStyle()) {

        // بعض الحاويات لها معنى حتى وهي فارغة
        if (node->type == WidgetType::Spacer ||
            node->type == WidgetType::Divider ||
            node->type == WidgetType::Scaffold) {
            return node; // لا تحذف
        }

        stats_.removedNodes++;
        return nullptr;
    }

    return node;
}


// ═══════════════════════════════════════════════════════════════════════════
// تسطيح المجموعات المتداخلة
// ═══════════════════════════════════════════════════════════════════════════

UINodeIRPtr IROptimizer::flattenNestedGroups(UINodeIRPtr node) {
    if (!node) return nullptr;

    // عمود/صف/مجموعة يحتوي على ابن واحد من نفس النوع → دمج
    if ((node->type == WidgetType::Group) && !node->style.hasAnyStyle() && !node->hasEvents()) {
        // بدلاً من Group(children) → نرجع الأبناء مباشرة
        // لكن يمكننا فقط إرجاع عقدة واحدة، فنفعل هذا فقط إذا كان هناك ابن واحد
        if (node->children.size() == 1) {
            stats_.flattenedGroups++;
            return node->children[0];
        }
    }

    // Column(Column(children)) → Column(children) إذا لا أنماط مختلفة
    if ((node->type == WidgetType::Column || node->type == WidgetType::Row)) {
        std::vector<UINodeIRPtr> newChildren;
        bool changed = false;

        for (auto& child : node->children) {
            if (child && child->type == node->type
                && !child->style.hasAnyStyle() && !child->hasEvents()) {
                // دمج أبناء الابن
                for (auto& grandchild : child->children) {
                    newChildren.push_back(std::move(grandchild));
                }
                stats_.flattenedGroups++;
                changed = true;
            } else {
                newChildren.push_back(std::move(child));
            }
        }

        // Always reassign — children were moved into newChildren
        node->children = std::move(newChildren);
    }

    return node;
}


// ═══════════════════════════════════════════════════════════════════════════
// تبسيط ابن وحيد
// ═══════════════════════════════════════════════════════════════════════════

UINodeIRPtr IROptimizer::simplifySingleChild(UINodeIRPtr node) {
    if (!node) return nullptr;

    // Container(child) بدون أنماط مميزة → child
    if (node->type == WidgetType::Container
        && node->children.size() == 1
        && !node->style.hasAnyStyle()
        && !node->hasEvents()) {
        stats_.simplifiedNesting++;
        return node->children[0];
    }

    // Box(child) بدون أنماط → child
    if (node->type == WidgetType::Box
        && node->children.size() == 1
        && !node->style.hasAnyStyle()
        && !node->hasEvents()) {
        stats_.simplifiedNesting++;
        return node->children[0];
    }

    // Center(child) → نقل contentAlignment إلى الابن (إذا ممكن)
    // لا نُبسّط Center لأن لها معنى دلالي (محاذاة الوسط)

    return node;
}


// ═══════════════════════════════════════════════════════════════════════════
// إزالة العقد المخفية
// ═══════════════════════════════════════════════════════════════════════════

UINodeIRPtr IROptimizer::removeHiddenNodes(UINodeIRPtr node) {
    if (!node) return nullptr;

    // إزالة العقد المخفية (hidden = true)
    if (node->style.hidden) {
        stats_.removedNodes++;
        return nullptr;
    }

    // إزالة العقد الشفافة تمامًا
    if (node->style.opacity == 0.0f && !node->hasEvents()) {
        stats_.removedNodes++;
        return nullptr;
    }

    // إزالة الأبناء المخفيين
    node->children.erase(
        std::remove_if(node->children.begin(), node->children.end(),
                        [this](UINodeIRPtr& child) -> bool {
                            if (!child) return true;
                            if (child->style.hidden) {
                                stats_.removedNodes++;
                                return true;
                            }
                            return false;
                        }),
        node->children.end()
    );

    return node;
}


// ═══════════════════════════════════════════════════════════════════════════
// تطبيق الثيمة
// ═══════════════════════════════════════════════════════════════════════════

void IROptimizer::applyTheme(UINodeIRPtr node, const ThemeIR& theme) {
    if (!node) return;
    applyThemeRecursive(node, theme);
}

void IROptimizer::applyThemeRecursive(UINodeIRPtr node, const ThemeIR& theme) {
    if (!node) return;

    auto& style = node->style;

    // تطبيق ألوان الثيمة إذا لم تكن محددة
    switch (node->type) {
        case WidgetType::Text:
            if (!style.foregroundColor.has_value()) {
                style.foregroundColor = theme.textPrimary;
            }
            if (!style.fontSize.has_value()) {
                style.fontSize = theme.fontSizeBody;
            }
            break;

        case WidgetType::Button:
        case WidgetType::FloatingButton:
            if (!style.backgroundColor.has_value()) {
                style.backgroundColor = theme.primaryColor;
            }
            if (!style.foregroundColor.has_value()) {
                style.foregroundColor = theme.textOnPrimary;
            }
            if (style.borderRadius == 0) {
                style.borderRadius = theme.borderRadiusMD;
            }
            break;

        case WidgetType::OutlineButton:
        case WidgetType::TextButton:
            if (!style.foregroundColor.has_value()) {
                style.foregroundColor = theme.primaryColor;
            }
            break;

        case WidgetType::Card:
            if (!style.backgroundColor.has_value()) {
                style.backgroundColor = theme.surfaceColor;
            }
            if (style.borderRadius == 0) {
                style.borderRadius = theme.borderRadiusMD;
            }
            break;

        case WidgetType::Scaffold:
            if (!style.backgroundColor.has_value()) {
                style.backgroundColor = theme.backgroundColor;
            }
            break;

        case WidgetType::AppBar:
            if (!style.backgroundColor.has_value()) {
                style.backgroundColor = theme.primaryColor;
            }
            if (!style.foregroundColor.has_value()) {
                style.foregroundColor = theme.textOnPrimary;
            }
            break;

        case WidgetType::TextField:
        case WidgetType::TextArea:
        case WidgetType::PasswordField:
            if (style.borderRadius == 0) {
                style.borderRadius = theme.borderRadiusSM;
            }
            break;

        default:
            break;
    }

    // تطبيق RTL
    style.isRTL = theme.isRTL;

    // تحسين الأبناء
    for (auto& child : node->children) {
        applyThemeRecursive(child, theme);
    }

    stats_.mergedStyles++;
}


} // namespace ir
} // namespace ui
} // namespace sad
