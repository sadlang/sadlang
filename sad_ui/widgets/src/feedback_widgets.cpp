/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: feedback_widgets.cpp
 * المسار: sad_ui/widgets/src/feedback_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عناصر التغذية الراجعة: Alert، Dialog، SnackBar، Badge، ProgressBar
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/feedback_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// AlertBuilder
// ═══════════════════════════════════════════════════════════════════════════════

AlertBuilder::AlertBuilder(const std::string& title) : title_(title) {}

AlertBuilder& AlertBuilder::title(const std::string& t) { title_ = t; return *this; }
AlertBuilder& AlertBuilder::message(const std::string& m) { message_ = m; return *this; }
AlertBuilder& AlertBuilder::addAction(const std::string& label, const std::string& onTap) {
    actions_.push_back({label, onTap});
    return *this;
}
AlertBuilder& AlertBuilder::onDismiss(const std::string& expression) { onDismissExpr_ = expression; return *this; }

std::shared_ptr<UINode> AlertBuilder::build() const {
    auto node = UINode::create(UINodeType::Alert);
    node->setPrimaryArg(title_);
    if (message_) {
        auto msgNode = UINode::create(UINodeType::Text);
        msgNode->setPrimaryArg(*message_);
        node->addChild(msgNode);
    }
    for (const auto& action : actions_) {
        auto btn = UINode::create(UINodeType::Button);
        btn->setPrimaryArg(action.label);
        if (!action.onTap.empty()) btn->addModifier(Modifier::event(ModifierType::OnTap, action.onTap));
        node->addChild(btn);
    }
    if (onDismissExpr_) node->addModifier(Modifier::event(ModifierType::OnDisappear, *onDismissExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DialogBuilder
// ═══════════════════════════════════════════════════════════════════════════════

DialogBuilder::DialogBuilder(const std::string& title) : title_(title) {}

DialogBuilder& DialogBuilder::title(const std::string& t) { title_ = t; return *this; }
DialogBuilder& DialogBuilder::content(std::shared_ptr<UINode> c) { content_ = std::move(c); return *this; }
DialogBuilder& DialogBuilder::addAction(const std::string& label, const std::string& onTap) {
    actions_.push_back({label, onTap});
    return *this;
}
DialogBuilder& DialogBuilder::onDismiss(const std::string& expression) { onDismissExpr_ = expression; return *this; }

std::shared_ptr<UINode> DialogBuilder::build() const {
    auto node = UINode::create(UINodeType::Dialog);
    node->setPrimaryArg(title_);
    if (content_) node->addChild(content_);
    for (const auto& action : actions_) {
        auto btn = UINode::create(UINodeType::Button);
        btn->setPrimaryArg(action.label);
        if (!action.onTap.empty()) btn->addModifier(Modifier::event(ModifierType::OnTap, action.onTap));
        node->addChild(btn);
    }
    if (onDismissExpr_) node->addModifier(Modifier::event(ModifierType::OnDisappear, *onDismissExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SnackBarBuilder
// ═══════════════════════════════════════════════════════════════════════════════

SnackBarBuilder::SnackBarBuilder(const std::string& message) : message_(message) {}

SnackBarBuilder& SnackBarBuilder::message(const std::string& m) { message_ = m; return *this; }
SnackBarBuilder& SnackBarBuilder::actionLabel(const std::string& label) { actionLabel_ = label; return *this; }
SnackBarBuilder& SnackBarBuilder::onAction(const std::string& expression) { onActionExpr_ = expression; return *this; }
SnackBarBuilder& SnackBarBuilder::duration(int milliseconds) { duration_ = milliseconds; return *this; }

std::shared_ptr<UINode> SnackBarBuilder::build() const {
    auto node = UINode::create(UINodeType::SnackBar);
    node->setPrimaryArg(message_);
    if (actionLabel_) {
        auto btn = UINode::create(UINodeType::Button);
        btn->setPrimaryArg(*actionLabel_);
        if (onActionExpr_) btn->addModifier(Modifier::event(ModifierType::OnTap, *onActionExpr_));
        node->addChild(btn);
    }
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BadgeBuilder
// ═══════════════════════════════════════════════════════════════════════════════

BadgeBuilder::BadgeBuilder() = default;

BadgeBuilder& BadgeBuilder::count(int c) { count_ = c; return *this; }
BadgeBuilder& BadgeBuilder::color(Color c) { color_ = c; return *this; }
BadgeBuilder& BadgeBuilder::child(std::shared_ptr<UINode> ch) { child_ = std::move(ch); return *this; }

std::shared_ptr<UINode> BadgeBuilder::build() const {
    auto node = UINode::create(UINodeType::Badge);
    if (count_) node->setPrimaryArg(std::to_string(*count_));
    if (color_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *color_));
    if (child_) node->addChild(child_);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ProgressBarBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ProgressBarBuilder::ProgressBarBuilder() = default;

ProgressBarBuilder& ProgressBarBuilder::value(float v) { value_ = v; return *this; }
ProgressBarBuilder& ProgressBarBuilder::indeterminate(bool ind) { indeterminate_ = ind; return *this; }
ProgressBarBuilder& ProgressBarBuilder::tintColor(Color c) { tintColor_ = c; return *this; }
ProgressBarBuilder& ProgressBarBuilder::trackColor(Color c) { trackColor_ = c; return *this; }

std::shared_ptr<UINode> ProgressBarBuilder::build() const {
    auto node = UINode::create(UINodeType::ProgressBar);
    if (value_) node->addModifier(Modifier::numeric(ModifierType::OnValueChange, *value_));
    if (tintColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *tintColor_));
    if (trackColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *trackColor_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeAlert(const std::string& title, const std::string& message) {
    auto builder = AlertBuilder(title);
    if (!message.empty()) builder.message(message);
    return builder.build();
}

std::shared_ptr<UINode> makeDialog(const std::string& title) {
    return DialogBuilder(title).build();
}

std::shared_ptr<UINode> makeSnackBar(const std::string& message) {
    return SnackBarBuilder(message).build();
}

std::shared_ptr<UINode> makeBadge(int count) {
    return BadgeBuilder().count(count).build();
}

std::shared_ptr<UINode> makeProgressBar(float value) {
    auto builder = ProgressBarBuilder();
    if (value >= 0) builder.value(value);
    else builder.indeterminate(true);
    return builder.build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
