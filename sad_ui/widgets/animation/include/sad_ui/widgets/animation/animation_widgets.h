/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: animation_widgets.h
 * المسار: sad_ui/widgets/animation/include/sad_ui/widgets/animation/animation_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مكونات الحركة: Skeleton، Shimmer، AnimatedList، Expandable، Collapsible، Swipeable
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_ANIMATION_H
#define SAD_UI_WIDGETS_ANIMATION_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"
#include <string>
#include <memory>
#include <optional>

namespace sad {
namespace ui {
namespace widgets {

// ── Skeleton ────────────────────────────────────────────────────────

class SkeletonBuilder {
public:
    SkeletonBuilder();
    SkeletonBuilder& width(float w);
    SkeletonBuilder& height(float h);
    SkeletonBuilder& borderRadius(float r);
    SkeletonBuilder& animated(bool a);
    std::shared_ptr<UINode> build() const;
private:
    std::optional<float> width_;
    std::optional<float> height_;
    float borderRadius_ = 4.0f;
    bool animated_ = true;
};

// ── Shimmer ─────────────────────────────────────────────────────────

class ShimmerBuilder {
public:
    ShimmerBuilder();
    ShimmerBuilder& baseColor(Color c);
    ShimmerBuilder& highlightColor(Color c);
    ShimmerBuilder& duration(float seconds);
    std::shared_ptr<UINode> build() const;
private:
    std::optional<Color> baseColor_;
    std::optional<Color> highlightColor_;
    float duration_ = 1.5f;
};

// ── AnimatedList ────────────────────────────────────────────────────

class AnimatedListBuilder {
public:
    AnimatedListBuilder();
    AnimatedListBuilder& animationType(const std::string& type);
    AnimatedListBuilder& duration(float seconds);
    std::shared_ptr<UINode> build() const;
private:
    std::string animationType_ = "fadeIn";
    float duration_ = 0.3f;
};

// ── Expandable ──────────────────────────────────────────────────────

class ExpandableBuilder {
public:
    explicit ExpandableBuilder(const std::string& title = "");
    ExpandableBuilder& expanded(bool e);
    ExpandableBuilder& iconPosition(const std::string& pos);
    std::shared_ptr<UINode> build() const;
private:
    std::string title_;
    bool expanded_ = false;
    std::string iconPosition_ = "end";
};

// ── Collapsible ─────────────────────────────────────────────────────

class CollapsibleBuilder {
public:
    explicit CollapsibleBuilder(const std::string& title = "");
    CollapsibleBuilder& collapsed(bool c);
    CollapsibleBuilder& headerColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    std::string title_;
    bool collapsed_ = true;
    std::optional<Color> headerColor_;
};

// ── Swipeable ───────────────────────────────────────────────────────

class SwipeableBuilder {
public:
    SwipeableBuilder();
    SwipeableBuilder& leftAction(const std::string& action);
    SwipeableBuilder& rightAction(const std::string& action);
    SwipeableBuilder& threshold(float t);
    std::shared_ptr<UINode> build() const;
private:
    std::optional<std::string> leftAction_;
    std::optional<std::string> rightAction_;
    float threshold_ = 0.3f;
};

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_ANIMATION_H
