/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: feedback_widgets.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/feedback_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * عناصر التغذية الراجعة: Alert، Dialog، SnackBar، Badge، ProgressBar
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_FEEDBACK_WIDGETS_H
#define SAD_UI_WIDGETS_FEEDBACK_WIDGETS_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"

#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// AlertBuilder — نافذة تنبيه
// ═══════════════════════════════════════════════════════════════════════════════

class AlertBuilder {
public:
    explicit AlertBuilder(const std::string& title = "");

    AlertBuilder& title(const std::string& t);
    AlertBuilder& message(const std::string& m);
    AlertBuilder& addAction(const std::string& label, const std::string& onTap = "");
    AlertBuilder& onDismiss(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string title_;
    std::optional<std::string> message_;
    struct AlertAction {
        std::string label;
        std::string onTap;
    };
    std::vector<AlertAction> actions_;
    std::optional<std::string> onDismissExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// DialogBuilder — نافذة حوار مخصصة
// ═══════════════════════════════════════════════════════════════════════════════

class DialogBuilder {
public:
    explicit DialogBuilder(const std::string& title = "");

    DialogBuilder& title(const std::string& t);
    DialogBuilder& content(std::shared_ptr<UINode> c);
    DialogBuilder& addAction(const std::string& label, const std::string& onTap = "");
    DialogBuilder& onDismiss(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string title_;
    std::shared_ptr<UINode> content_;
    struct DialogAction {
        std::string label;
        std::string onTap;
    };
    std::vector<DialogAction> actions_;
    std::optional<std::string> onDismissExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// SnackBarBuilder — رسالة مؤقتة أسفل الشاشة
// ═══════════════════════════════════════════════════════════════════════════════

class SnackBarBuilder {
public:
    explicit SnackBarBuilder(const std::string& message = "");

    SnackBarBuilder& message(const std::string& m);
    SnackBarBuilder& actionLabel(const std::string& label);
    SnackBarBuilder& onAction(const std::string& expression);
    SnackBarBuilder& duration(int milliseconds);

    std::shared_ptr<UINode> build() const;

private:
    std::string message_;
    std::optional<std::string> actionLabel_;
    std::optional<std::string> onActionExpr_;
    int duration_ = 3000;
};

// ═══════════════════════════════════════════════════════════════════════════════
// BadgeBuilder — شارة إشعار
// ═══════════════════════════════════════════════════════════════════════════════

class BadgeBuilder {
public:
    BadgeBuilder();

    BadgeBuilder& count(int c);
    BadgeBuilder& color(Color c);
    BadgeBuilder& child(std::shared_ptr<UINode> ch);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<int> count_;
    std::optional<Color> color_;
    std::shared_ptr<UINode> child_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ProgressBarBuilder — شريط تقدم
// ═══════════════════════════════════════════════════════════════════════════════

class ProgressBarBuilder {
public:
    ProgressBarBuilder();

    ProgressBarBuilder& value(float v);
    ProgressBarBuilder& indeterminate(bool ind);
    ProgressBarBuilder& tintColor(Color c);
    ProgressBarBuilder& trackColor(Color c);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> value_;
    bool indeterminate_ = false;
    std::optional<Color> tintColor_;
    std::optional<Color> trackColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeAlert(const std::string& title, const std::string& message = "");
std::shared_ptr<UINode> makeDialog(const std::string& title = "");
std::shared_ptr<UINode> makeSnackBar(const std::string& message);
std::shared_ptr<UINode> makeBadge(int count = 0);
std::shared_ptr<UINode> makeProgressBar(float value = -1.0f);

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_FEEDBACK_WIDGETS_H
