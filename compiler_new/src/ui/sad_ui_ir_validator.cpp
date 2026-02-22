/**
 * ==========================================================================
 * ملف: sad_ui_ir_validator.cpp
 * الوصف: تنفيذ مُدقّق SadUI IR
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/sad_ui_ir_validator.hpp"
#include <sstream>

namespace sad {
namespace ui {
namespace ir {

IRValidator::IRValidator(TargetPlatform platform)
    : platform_(platform) {}

void IRValidator::setTargetPlatform(TargetPlatform platform) {
    platform_ = platform;
}

void IRValidator::setMaxDepth(int depth) {
    maxDepth_ = depth;
}

void IRValidator::setMaxNodes(int count) {
    maxNodes_ = count;
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق الرئيسي: شاشة
// ═══════════════════════════════════════════════════════════════════════════

ValidationResult IRValidator::validate(const ScreenIR& screen) {
    issues_.clear();
    currentPath_ = screen.name;

    // التحقق من وجود اسم
    if (screen.name.empty()) {
        addIssue(ValidationLevel::Error,
                 "\xd8\xa7\xd9\x84\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd8\xa7\xd8\xb3\xd9\x85",
                 "Screen has no name");
    }

    // التحقق من وجود شجرة مكونات
    if (!screen.rootNode) {
        addIssue(ValidationLevel::Error,
                 "\xd8\xa7\xd9\x84\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9 \xd9\x84\xd8\xa7 \xd8\xaa\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a \xd8\xb9\xd9\x84\xd9\x89 \xd9\x85\xd9\x83\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa",
                 "Screen has no component tree");
    } else {
        // التحقق من حجم الشجرة
        int size = screen.rootNode->subtreeSize();
        if (size > maxNodes_) {
            addIssue(ValidationLevel::Warning,
                     "\xd8\xa7\xd9\x84\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9 \xd8\xaa\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a \xd8\xb9\xd9\x84\xd9\x89 " + std::to_string(size) + " \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9",
                     "Screen has " + std::to_string(size) + " nodes (max " + std::to_string(maxNodes_) + ")");
        }

        validateNode(screen.rootNode, 0);
    }

    // التحقق من ربط الحالة
    for (const auto& binding : screen.stateBindings) {
        if (binding.name.empty()) {
            addIssue(ValidationLevel::Error,
                     "\xd8\xb1\xd8\xa8\xd8\xb7 \xd8\xad\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd8\xa7\xd8\xb3\xd9\x85",
                     "State binding has no name");
        }
    }

    ValidationResult result;
    result.issues = std::move(issues_);
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق الرئيسي: عقدة
// ═══════════════════════════════════════════════════════════════════════════

ValidationResult IRValidator::validate(const UINodeIRPtr& node) {
    issues_.clear();
    currentPath_ = "";

    if (node) {
        validateNode(node, 0);
    } else {
        addIssue(ValidationLevel::Error,
                 "\xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9",
                 "NULL node");
    }

    ValidationResult result;
    result.issues = std::move(issues_);
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق الرئيسي: تطبيق
// ═══════════════════════════════════════════════════════════════════════════

ValidationResult IRValidator::validate(const AppIR& app) {
    issues_.clear();

    if (app.appName.empty()) {
        addIssue(ValidationLevel::Error,
                 "\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd8\xa7\xd8\xb3\xd9\x85",
                 "App has no name");
    }

    if (app.screens.empty()) {
        addIssue(ValidationLevel::Error,
                 "\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa7\xd8\xaa",
                 "App has no screens");
    }

    // التحقق من كل شاشة
    for (const auto& screen : app.screens) {
        auto screenResult = validate(screen);
        for (auto& issue : screenResult.issues) {
            issues_.push_back(std::move(issue));
        }
    }

    // التحقق من الشاشة الأولى
    if (!app.initialScreenName.empty()) {
        bool found = false;
        for (const auto& screen : app.screens) {
            if (screen.name == app.initialScreenName) { found = true; break; }
        }
        if (!found) {
            addIssue(ValidationLevel::Error,
                     "\xd8\xa7\xd9\x84\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa3\xd9\x88\xd9\x84\xd9\x89 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd9\x88\xd8\xac\xd9\x88\xd8\xaf\xd8\xa9: " + app.initialScreenName,
                     "Initial screen not found: " + app.initialScreenName);
        }
    }

    // التحقق من إعدادات المنصة
    if (platform_ == TargetPlatform::Android || platform_ == TargetPlatform::All) {
        if (app.androidConfig.has_value()) {
            if (app.androidConfig->minSdk < 21) {
                addIssue(ValidationLevel::Warning,
                         "Jetpack Compose \xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac minSdk >= 21",
                         "Jetpack Compose requires minSdk >= 21");
            }
        }
    }

    ValidationResult result;
    result.issues = std::move(issues_);
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق التكراري من عقدة
// ═══════════════════════════════════════════════════════════════════════════

void IRValidator::validateNode(const UINodeIRPtr& node, int depth) {
    if (!node) return;

    // فحص العمق
    if (depth > maxDepth_) {
        addIssue(ValidationLevel::Error,
                 "\xd8\xaa\xd8\xac\xd8\xa7\xd9\x88\xd8\xb2 \xd8\xa7\xd9\x84\xd8\xad\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89 \xd9\x84\xd8\xb9\xd9\x85\xd9\x82 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 (" + std::to_string(depth) + ")",
                 "Max tree depth exceeded (" + std::to_string(depth) + ")",
                 node->id, node->sourceLine, node->sourceColumn);
        return;
    }

    // فحص خاص بالنوع
    switch (node->type) {
        case WidgetType::Text:
            // نص يجب أن يكون له محتوى
            if (node->constructorArgs.empty()) {
                addIssue(ValidationLevel::Warning,
                         "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd9\x86\xd8\xb5 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89",
                         "Text widget has no content",
                         node->id, node->sourceLine, node->sourceColumn);
            }
            break;

        case WidgetType::Button:
            // زر يجب أن يكون له معالج نقر أو محتوى
            if (node->eventHandlers.empty() && node->constructorArgs.empty() && node->children.empty()) {
                addIssue(ValidationLevel::Warning,
                         "\xd8\xb2\xd8\xb1 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89 \xd8\xa3\xd9\x88 \xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac \xd8\xad\xd8\xaf\xd8\xab",
                         "Button without content or event handler",
                         node->id, node->sourceLine, node->sourceColumn);
            }
            break;

        case WidgetType::TextField:
        case WidgetType::TextArea:
        case WidgetType::PasswordField:
            // حقل إدخال بدون ربط حالة = تحذير
            if (!node->stateBinding.has_value()) {
                addIssue(ValidationLevel::Suggestion,
                         "\xd8\xad\xd9\x82\xd9\x84 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd8\xb1\xd8\xa8\xd8\xb7 \xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
                         "Input field without state binding",
                         node->id, node->sourceLine, node->sourceColumn);
            }
            break;

        case WidgetType::Image:
            // صورة يجب أن يكون لها مصدر
            if (node->constructorArgs.empty() && node->properties.find("src") == node->properties.end()) {
                addIssue(ValidationLevel::Warning,
                         "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9 \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1",
                         "Image widget has no source",
                         node->id, node->sourceLine, node->sourceColumn);
            }
            break;

        default:
            break;
    }

    // فحص الأنماط
    validateStyle(node->style, node->type, node->id);

    // فحص الأحداث
    validateEvents(node->eventHandlers, node->type, node->id);

    // فحص توافق المنصة
    validatePlatformCompat(node->type, node->id);

    // فحص الأبناء
    validateChildren(node);

    // تكرار على الأبناء
    for (const auto& child : node->children) {
        validateNode(child, depth + 1);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق من الأنماط
// ═══════════════════════════════════════════════════════════════════════════

void IRValidator::validateStyle(const StyleIR& style, WidgetType type, int nodeId) {
    // العتامة يجب أن تكون بين 0 و 1
    if (style.opacity < 0.0f || style.opacity > 1.0f) {
        addIssue(ValidationLevel::Warning,
                 "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaa\xd8\xa7\xd9\x85\xd8\xa9 \xd9\x8a\xd8\xac\xd8\xa8 \xd8\xa3\xd9\x86 \xd8\xaa\xd9\x83\xd9\x88\xd9\x86 \xd8\xa8\xd9\x8a\xd9\x86 0 \xd9\x88 1",
                 "Opacity must be between 0 and 1",
                 nodeId);
    }

    // المقياس السالب = تحذير
    if (style.scale < 0) {
        addIssue(ValidationLevel::Warning,
                 "\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3 \xd8\xb3\xd8\xa7\xd9\x84\xd8\xa8 \xd9\x82\xd8\xaf \xd9\x8a\xd8\xb3\xd8\xa8\xd8\xa8 \xd8\xb3\xd9\x84\xd9\x88\xd9\x83 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9",
                 "Negative scale may cause unexpected behavior",
                 nodeId);
    }

    // فحص الأبعاد السالبة
    if (style.fontSize.has_value() && *style.fontSize <= 0) {
        addIssue(ValidationLevel::Error,
                 "\xd8\xad\xd8\xac\xd9\x85 \xd8\xa7\xd9\x84\xd8\xae\xd8\xb7 \xd9\x8a\xd8\xac\xd8\xa8 \xd8\xa3\xd9\x86 \xd9\x8a\xd9\x83\xd9\x88\xd9\x86 \xd9\x85\xd9\x88\xd8\xac\xd8\xa8",
                 "Font size must be positive",
                 nodeId);
    }

    if (style.borderRadius < 0 || style.borderWidth < 0) {
        addIssue(ValidationLevel::Warning,
                 "\xd8\xa7\xd9\x84\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf \xd9\x84\xd8\xa7 \xd9\x8a\xd9\x85\xd9\x83\xd9\x86 \xd8\xa3\xd9\x86 \xd8\xaa\xd9\x83\xd9\x88\xd9\x86 \xd8\xb3\xd8\xa7\xd9\x84\xd8\xa8\xd8\xa9",
                 "Border values cannot be negative",
                 nodeId);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق من الأحداث
// ═══════════════════════════════════════════════════════════════════════════

void IRValidator::validateEvents(const std::vector<EventHandlerIR>& events,
                                  WidgetType type, int nodeId) {
    for (const auto& ev : events) {
        // التحقق من وجود معالج
        if (ev.handlerName.empty() && ev.closureBody.empty()) {
            addIssue(ValidationLevel::Warning,
                     "\xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac \xd8\xad\xd8\xaf\xd8\xab \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa3\xd9\x88 \xd8\xac\xd8\xb3\xd9\x85",
                     "Event handler has no function name or closure body",
                     nodeId);
        }

        // TextChange فقط لحقول الإدخال
        if (ev.type == EventType::TextChange) {
            if (type != WidgetType::TextField &&
                type != WidgetType::TextArea &&
                type != WidgetType::PasswordField &&
                type != WidgetType::SearchField) {
                addIssue(ValidationLevel::Warning,
                         "\xd8\xad\xd8\xaf\xd8\xab \xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x86\xd8\xb5 \xd9\x85\xd8\xb1\xd8\xa8\xd9\x88\xd8\xb7 \xd8\xa8\xd9\x85\xd9\x83\xd9\x88\xd9\x86 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xb5\xd9\x8a",
                         "TextChange event bound to non-text widget",
                         nodeId);
            }
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق من توافق المنصة
// ═══════════════════════════════════════════════════════════════════════════

void IRValidator::validatePlatformCompat(WidgetType type, int nodeId) {
    if (platform_ == TargetPlatform::All) return;

    // WebView غير مدعوم على سطح المكتب (SDL2) حاليًا
    if (platform_ == TargetPlatform::Desktop) {
        if (type == WidgetType::WebView || type == WidgetType::MapView) {
            addIssue(ValidationLevel::Warning,
                     "\xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd9\x88\xd9\x86 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaf\xd8\xb9\xd9\x88\xd9\x85 \xd8\xb9\xd9\x84\xd9\x89 \xd8\xb3\xd8\xb7\xd8\xad \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8",
                     "Widget not supported on Desktop (SDL2)",
                     nodeId);
        }
    }

    // BottomSheet/Drawer تحتاج معالجة خاصة على سطح المكتب
    if (platform_ == TargetPlatform::Desktop) {
        if (type == WidgetType::BottomSheet || type == WidgetType::Drawer) {
            addIssue(ValidationLevel::Suggestion,
                     "\xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd9\x88\xd9\x86 \xd8\xb3\xd9\x8a\xd8\xb8\xd9\x87\xd8\xb1 \xd9\x83\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9 \xd8\xb9\xd9\x84\xd9\x89 \xd8\xb3\xd8\xb7\xd8\xad \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8",
                     "Widget will appear as a window overlay on Desktop",
                     nodeId);
        }
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// التحقق من الأبناء
// ═══════════════════════════════════════════════════════════════════════════

void IRValidator::validateChildren(const UINodeIRPtr& node) {
    if (!node) return;

    // التحقق من عقد غير حاوية تحتوي أبناء
    if (!node->isContainer() && node->hasChildren()) {
        addIssue(ValidationLevel::Warning,
                 widgetTypeToArabic(node->type) + " \xd9\x84\xd9\x8a\xd8\xb3 \xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9 \xd9\x84\xd9\x83\xd9\x86\xd9\x87 \xd9\x8a\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a \xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1",
                 widgetTypeToArabic(node->type) + " is not a container but has children",
                 node->id, node->sourceLine, node->sourceColumn);
    }

    // ForEach يجب أن يحتوي على ابن واحد بالضبط (القالب)
    if (node->type == WidgetType::ForEach && node->children.empty()) {
        addIssue(ValidationLevel::Error,
                 "\xd9\x84\xd9\x83\xd9\x84 \xd9\x8a\xd8\xac\xd8\xa8 \xd8\xa3\xd9\x86 \xd9\x8a\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a \xd8\xb9\xd9\x84\xd9\x89 \xd9\x82\xd8\xa7\xd9\x84\xd8\xa8",
                 "ForEach must have a template child",
                 node->id, node->sourceLine, node->sourceColumn);
    }

    // Scaffold يجب أن يحتوي على محتوى
    if (node->type == WidgetType::Scaffold && node->children.empty()) {
        addIssue(ValidationLevel::Warning,
                 "\xd8\xa7\xd9\x84\xd8\xb3\xd9\x82\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9",
                 "Scaffold has no content",
                 node->id, node->sourceLine, node->sourceColumn);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// مساعدات
// ═══════════════════════════════════════════════════════════════════════════

void IRValidator::addIssue(ValidationLevel level,
                            const std::string& messageAR,
                            const std::string& messageEN,
                            int nodeId,
                            size_t line, size_t col) {
    issues_.push_back({level, messageAR, messageEN, currentPath_, line, col, nodeId});
}


} // namespace ir
} // namespace ui
} // namespace sad
