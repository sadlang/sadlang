// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_widgets.cpp — تنفيذ المكونات الأساسية
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_widgets.h"
#include <cmath>
#include <algorithm>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  TextRenderObject
// ═══════════════════════════════════════════════════════════════════════════════

void TextRenderObject::layout(float maxWidth, float maxHeight) {
    // Simple text measurement (platform-specific implementation needed)
    // This is a placeholder that estimates size
    float charWidth = style.fontSize * 0.6f;  // approximate
    float lineHeight = style.fontSize * style.height;
    
    size_t textLen = text.size();
    // For RTL/Arabic, count actual characters (UTF-8)
    // Simplified: assume average 2 bytes per Arabic character
    size_t approxChars = textLen / 2;
    
    float totalWidth = approxChars * charWidth;
    
    if (totalWidth <= maxWidth) {
        // Single line
        size_.width = totalWidth;
        size_.height = lineHeight;
    } else {
        // Multiple lines
        int lines = static_cast<int>(std::ceil(totalWidth / maxWidth));
        if (maxLines > 0 && lines > maxLines) {
            lines = maxLines;
        }
        size_.width = maxWidth;
        size_.height = lines * lineHeight;
    }
}

void TextRenderObject::paint() {
    // Platform-specific text rendering
    // Will be implemented by each backend (SDL, Metal, Canvas, etc.)
    // For now, this is a stub
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ContainerRenderObject
// ═══════════════════════════════════════════════════════════════════════════════

void ContainerRenderObject::layout(float maxWidth, float maxHeight) {
    float availableWidth = maxWidth - margin.left - margin.right;
    float availableHeight = maxHeight - margin.top - margin.bottom;
    
    // If explicit size is specified, use it
    float contentWidth = width.value_or(availableWidth) - padding.left - padding.right;
    float contentHeight = height.value_or(availableHeight) - padding.top - padding.bottom;
    
    // Layout children
    for (auto& child : children_) {
        child->layout(contentWidth, contentHeight);
    }
    
    // Calculate final size
    if (width) {
        size_.width = *width;
    } else if (!children_.empty()) {
        size_.width = children_[0]->getSize().width + padding.left + padding.right;
    } else {
        size_.width = 0;
    }
    
    if (height) {
        size_.height = *height;
    } else if (!children_.empty()) {
        size_.height = children_[0]->getSize().height + padding.top + padding.bottom;
    } else {
        size_.height = 0;
    }
}

void ContainerRenderObject::paint() {
    // Draw background if color is set
    if (decoration.color) {
        // Platform-specific: drawRect(position_, size_, *decoration.color)
    }
    
    // Draw border if set
    if (decoration.border && decoration.border->width > 0) {
        // Platform-specific: drawBorder(...)
    }
    
    // Draw shadows
    for (const auto& shadow : decoration.boxShadow) {
        // Platform-specific: drawShadow(...)
    }
    
    // Paint children
    for (auto& child : children_) {
        child->paint();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SizedBox
// ═══════════════════════════════════════════════════════════════════════════════

class SizedBoxRenderObject : public RenderObject {
public:
    std::optional<float> width, height;
    
    void layout(float maxWidth, float maxHeight) override {
        float w = width.value_or(0);
        float h = height.value_or(0);
        
        // Constrain to max
        size_.width = std::min(w, maxWidth);
        size_.height = std::min(h, maxHeight);
        
        // Layout child if any
        if (!children_.empty()) {
            children_[0]->layout(size_.width, size_.height);
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> SizedBox::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<SizedBoxRenderObject>();
    ro->width = width_;
    ro->height = height_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GestureDetector
// ═══════════════════════════════════════════════════════════════════════════════

class GestureRenderObject : public RenderObject {
public:
    VoidCallback onTap;
    VoidCallback onDoubleTap;
    VoidCallback onLongPress;
    TapCallback onTapDown, onTapUp;
    
    void layout(float maxWidth, float maxHeight) override {
        // Pass through to child
        if (!children_.empty()) {
            children_[0]->layout(maxWidth, maxHeight);
            size_ = children_[0]->getSize();
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
    
    bool hitTest(float x, float y) override {
        // Check if point is within bounds
        auto& pos = position_;
        auto& sz = size_;
        return x >= pos.x && x <= pos.x + sz.width &&
               y >= pos.y && y <= pos.y + sz.height;
    }
};

std::unique_ptr<RenderObject> GestureDetector::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<GestureRenderObject>();
    ro->onTap = onTap_;
    ro->onDoubleTap = onDoubleTap_;
    ro->onLongPress = onLongPress_;
    ro->onTapDown = onTapDown_;
    ro->onTapUp = onTapUp_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ElevatedButton
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr ElevatedButton::build(BuildContext* ctx) {
    BoxDecoration deco;
    deco.setColor(backgroundColor_)
        .setRadius(borderRadius_.topLeft)
        .addShadow(BoxShadow{Color(0,0,0,64), 4, 0, 0, 2});
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            child_,
            std::nullopt, std::nullopt,
            padding_, EdgeInsets(),
            deco
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  TextButton
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr TextButton::build(BuildContext* ctx) {
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            child_,
            std::nullopt, std::nullopt,
            padding_
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Icon
// ═══════════════════════════════════════════════════════════════════════════════

void IconRenderObject::layout(float maxWidth, float maxHeight) {
    size_.width = std::min(size, maxWidth);
    size_.height = std::min(size, maxHeight);
}

void IconRenderObject::paint() {
    // Platform-specific icon rendering
    // Will draw the icon glyph at position_ with size and color
}

// ═══════════════════════════════════════════════════════════════════════════════
//  IconButton
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr IconButton::build(BuildContext* ctx) {
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Icon>(icon_, size_, color_),
            splashRadius_ * 2, splashRadius_ * 2,
            EdgeInsets::all((splashRadius_ * 2 - size_) / 2)
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Divider
// ═══════════════════════════════════════════════════════════════════════════════

class DividerRenderObject : public RenderObject {
public:
    float height, thickness, indent, endIndent;
    Color color;
    
    void layout(float maxWidth, float maxHeight) override {
        size_.width = maxWidth - indent - endIndent;
        size_.height = height;
    }
    
    void paint() override {
        // Draw horizontal line
        // Platform-specific: drawLine or fillRect
    }
};

std::unique_ptr<RenderObject> Divider::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<DividerRenderObject>();
    ro->height = height_;
    ro->thickness = thickness_;
    ro->indent = indent_;
    ro->endIndent = endIndent_;
    ro->color = color_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Spacer
// ═══════════════════════════════════════════════════════════════════════════════

class SpacerRenderObject : public RenderObject {
public:
    int flex = 1;
    
    void layout(float maxWidth, float maxHeight) override {
        // Size is determined by parent layout (Flex)
        size_.width = 0;
        size_.height = 0;
    }
    
    void paint() override {
        // Nothing to paint
    }
};

std::unique_ptr<RenderObject> Spacer::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<SpacerRenderObject>();
    ro->flex = flex_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Opacity
// ═══════════════════════════════════════════════════════════════════════════════

class OpacityRenderObject : public RenderObject {
public:
    float opacity = 1.0f;
    
    void layout(float maxWidth, float maxHeight) override {
        if (!children_.empty()) {
            children_[0]->layout(maxWidth, maxHeight);
            size_ = children_[0]->getSize();
        }
    }
    
    void paint() override {
        // Apply opacity transformation
        // Platform-specific: pushOpacity(opacity)
        for (auto& child : children_) {
            child->paint();
        }
        // Platform-specific: popOpacity()
    }
};

std::unique_ptr<RenderObject> Opacity::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<OpacityRenderObject>();
    ro->opacity = opacity_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Visibility
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Visibility::build(BuildContext* ctx) {
    if (!visible_) {
        if (maintainSize_) {
            // Keep size but make invisible
            return std::make_shared<Opacity>(0.0f, child_);
        } else {
            // Take no space
            return std::make_shared<SizedBox>(0.0f, 0.0f);
        }
    }
    return child_;
}

} // namespace flutter
} // namespace sad
