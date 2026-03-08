// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_layout.cpp — تنفيذ مكونات التخطيط
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_layout.h"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  FlexRenderObject
// ═══════════════════════════════════════════════════════════════════════════════

void FlexRenderObject::layout(float maxWidth, float maxHeight) {
    if (direction == Axis::Vertical) {
        layoutVertical(maxWidth, maxHeight);
    } else {
        layoutHorizontal(maxWidth, maxHeight);
    }
}

void FlexRenderObject::layoutVertical(float maxWidth, float maxHeight) {
    // Phase 1: Measure non-flex children
    float totalFixedHeight = 0;
    float maxChildWidth = 0;
    int flexCount = 0;
    int totalFlex = 0;
    
    std::vector<float> childHeights(children_.size(), 0);
    std::vector<int> flexValues(children_.size(), 0);
    
    for (size_t i = 0; i < children_.size(); ++i) {
        auto& child = children_[i];
        
        // Check if this is an Expanded/Flexible (simplified check via size)
        // In real impl, we'd check the Element/Widget type
        // For now, assume flex=0 for non-Expanded children
        int flex = 0;  // TODO: get from parentData
        
        if (flex > 0) {
            flexCount++;
            totalFlex += flex;
            flexValues[i] = flex;
        } else {
            child->layout(maxWidth, maxHeight);
            childHeights[i] = child->getSize().height;
            totalFixedHeight += childHeights[i];
            maxChildWidth = std::max(maxChildWidth, child->getSize().width);
        }
    }
    
    // Add spacing
    float totalSpacing = spacing * (children_.size() > 0 ? children_.size() - 1 : 0);
    
    // Phase 2: Distribute remaining space to flex children
    float remainingHeight = maxHeight - totalFixedHeight - totalSpacing;
    float heightPerFlex = (totalFlex > 0 && remainingHeight > 0) 
                          ? remainingHeight / totalFlex : 0;
    
    for (size_t i = 0; i < children_.size(); ++i) {
        if (flexValues[i] > 0) {
            float allocatedHeight = heightPerFlex * flexValues[i];
            children_[i]->layout(maxWidth, allocatedHeight);
            childHeights[i] = allocatedHeight;
            maxChildWidth = std::max(maxChildWidth, children_[i]->getSize().width);
        }
    }
    
    // Phase 3: Position children
    float totalContentHeight = std::accumulate(childHeights.begin(), childHeights.end(), 0.0f)
                               + totalSpacing;
    
    float startY = 0;
    float extraSpace = maxHeight - totalContentHeight;
    float spaceBetween = 0;
    
    switch (mainAxisAlignment) {
        case MainAxisAlignment::Start:
            startY = 0;
            break;
        case MainAxisAlignment::End:
            startY = extraSpace;
            break;
        case MainAxisAlignment::Center:
            startY = extraSpace / 2;
            break;
        case MainAxisAlignment::SpaceBetween:
            if (children_.size() > 1) {
                spaceBetween = extraSpace / (children_.size() - 1);
            }
            break;
        case MainAxisAlignment::SpaceAround:
            if (children_.size() > 0) {
                float spaceUnit = extraSpace / children_.size();
                startY = spaceUnit / 2;
                spaceBetween = spaceUnit;
            }
            break;
        case MainAxisAlignment::SpaceEvenly:
            if (children_.size() > 0) {
                float spaceUnit = extraSpace / (children_.size() + 1);
                startY = spaceUnit;
                spaceBetween = spaceUnit;
            }
            break;
    }
    
    float currentY = startY;
    for (size_t i = 0; i < children_.size(); ++i) {
        auto& child = children_[i];
        auto childSize = child->getSize();
        
        // Cross axis alignment
        float childX = 0;
        switch (crossAxisAlignment) {
            case CrossAxisAlignment::Start:
                childX = 0;
                break;
            case CrossAxisAlignment::End:
                childX = maxWidth - childSize.width;
                break;
            case CrossAxisAlignment::Center:
                childX = (maxWidth - childSize.width) / 2;
                break;
            case CrossAxisAlignment::Stretch:
                childX = 0;
                // Re-layout with full width
                child->layout(maxWidth, childHeights[i]);
                break;
            default:
                break;
        }
        
        child->setPosition({position_.x + childX, position_.y + currentY});
        currentY += childHeights[i] + spacing + spaceBetween;
    }
    
    // Set our size
    if (mainAxisSize == MainAxisSize::Max) {
        size_.height = maxHeight;
    } else {
        size_.height = totalContentHeight;
    }
    size_.width = crossAxisAlignment == CrossAxisAlignment::Stretch ? maxWidth : maxChildWidth;
}

void FlexRenderObject::layoutHorizontal(float maxWidth, float maxHeight) {
    // Similar to vertical but with x/y swapped
    float totalFixedWidth = 0;
    float maxChildHeight = 0;
    int totalFlex = 0;
    
    std::vector<float> childWidths(children_.size(), 0);
    std::vector<int> flexValues(children_.size(), 0);
    
    for (size_t i = 0; i < children_.size(); ++i) {
        auto& child = children_[i];
        int flex = 0;  // TODO: get from parentData
        
        if (flex > 0) {
            totalFlex += flex;
            flexValues[i] = flex;
        } else {
            child->layout(maxWidth, maxHeight);
            childWidths[i] = child->getSize().width;
            totalFixedWidth += childWidths[i];
            maxChildHeight = std::max(maxChildHeight, child->getSize().height);
        }
    }
    
    float totalSpacing = spacing * (children_.size() > 0 ? children_.size() - 1 : 0);
    float remainingWidth = maxWidth - totalFixedWidth - totalSpacing;
    float widthPerFlex = (totalFlex > 0 && remainingWidth > 0) 
                         ? remainingWidth / totalFlex : 0;
    
    for (size_t i = 0; i < children_.size(); ++i) {
        if (flexValues[i] > 0) {
            float allocatedWidth = widthPerFlex * flexValues[i];
            children_[i]->layout(allocatedWidth, maxHeight);
            childWidths[i] = allocatedWidth;
            maxChildHeight = std::max(maxChildHeight, children_[i]->getSize().height);
        }
    }
    
    // Position children
    float totalContentWidth = std::accumulate(childWidths.begin(), childWidths.end(), 0.0f)
                              + totalSpacing;
    
    float startX = 0;
    float extraSpace = maxWidth - totalContentWidth;
    float spaceBetween = 0;
    
    switch (mainAxisAlignment) {
        case MainAxisAlignment::Start:
            startX = 0;
            break;
        case MainAxisAlignment::End:
            startX = extraSpace;
            break;
        case MainAxisAlignment::Center:
            startX = extraSpace / 2;
            break;
        case MainAxisAlignment::SpaceBetween:
            if (children_.size() > 1) {
                spaceBetween = extraSpace / (children_.size() - 1);
            }
            break;
        case MainAxisAlignment::SpaceAround:
            if (children_.size() > 0) {
                float spaceUnit = extraSpace / children_.size();
                startX = spaceUnit / 2;
                spaceBetween = spaceUnit;
            }
            break;
        case MainAxisAlignment::SpaceEvenly:
            if (children_.size() > 0) {
                float spaceUnit = extraSpace / (children_.size() + 1);
                startX = spaceUnit;
                spaceBetween = spaceUnit;
            }
            break;
    }
    
    float currentX = startX;
    for (size_t i = 0; i < children_.size(); ++i) {
        auto& child = children_[i];
        auto childSize = child->getSize();
        
        float childY = 0;
        switch (crossAxisAlignment) {
            case CrossAxisAlignment::Start:
                childY = 0;
                break;
            case CrossAxisAlignment::End:
                childY = maxHeight - childSize.height;
                break;
            case CrossAxisAlignment::Center:
                childY = (maxHeight - childSize.height) / 2;
                break;
            case CrossAxisAlignment::Stretch:
                childY = 0;
                child->layout(childWidths[i], maxHeight);
                break;
            default:
                break;
        }
        
        child->setPosition({position_.x + currentX, position_.y + childY});
        currentX += childWidths[i] + spacing + spaceBetween;
    }
    
    if (mainAxisSize == MainAxisSize::Max) {
        size_.width = maxWidth;
    } else {
        size_.width = totalContentWidth;
    }
    size_.height = crossAxisAlignment == CrossAxisAlignment::Stretch ? maxHeight : maxChildHeight;
}

void FlexRenderObject::paint() {
    for (auto& child : children_) {
        child->paint();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Flex
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<RenderObject> Flex::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<FlexRenderObject>();
    ro->direction = direction_;
    ro->mainAxisAlignment = mainAxisAlignment_;
    ro->mainAxisSize = mainAxisSize_;
    ro->crossAxisAlignment = crossAxisAlignment_;
    ro->spacing = spacing_;
    return ro;
}

void Flex::updateRenderObject(BuildContext* ctx, RenderObject* ro) {
    auto* fro = static_cast<FlexRenderObject*>(ro);
    fro->direction = direction_;
    fro->mainAxisAlignment = mainAxisAlignment_;
    fro->mainAxisSize = mainAxisSize_;
    fro->crossAxisAlignment = crossAxisAlignment_;
    fro->spacing = spacing_;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Expanded & Flexible
// ═══════════════════════════════════════════════════════════════════════════════

class ExpandedRenderObject : public RenderObject {
public:
    int flex = 1;
    
    void layout(float maxWidth, float maxHeight) override {
        // Size comes from parent Flex calculation
        if (!children_.empty()) {
            children_[0]->layout(maxWidth, maxHeight);
            size_ = children_[0]->getSize();
        } else {
            size_ = {maxWidth, maxHeight};
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> Expanded::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<ExpandedRenderObject>();
    ro->flex = flex_;
    return ro;
}

class FlexibleRenderObject : public RenderObject {
public:
    int flex = 1;
    FlexFit fit = FlexFit::Loose;
    
    void layout(float maxWidth, float maxHeight) override {
        if (!children_.empty()) {
            if (fit == FlexFit::Tight) {
                children_[0]->layout(maxWidth, maxHeight);
                size_ = {maxWidth, maxHeight};
            } else {
                children_[0]->layout(maxWidth, maxHeight);
                size_ = children_[0]->getSize();
            }
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> Flexible::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<FlexibleRenderObject>();
    ro->flex = flex_;
    ro->fit = fit_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Stack
// ═══════════════════════════════════════════════════════════════════════════════

void StackRenderObject::layout(float maxWidth, float maxHeight) {
    float maxChildWidth = 0;
    float maxChildHeight = 0;
    
    for (auto& child : children_) {
        child->layout(maxWidth, maxHeight);
        auto childSize = child->getSize();
        maxChildWidth = std::max(maxChildWidth, childSize.width);
        maxChildHeight = std::max(maxChildHeight, childSize.height);
    }
    
    size_.width = (fit == StackFit::Expand) ? maxWidth : maxChildWidth;
    size_.height = (fit == StackFit::Expand) ? maxHeight : maxChildHeight;
    
    // Position children according to alignment
    for (auto& child : children_) {
        auto childSize = child->getSize();
        float offsetX = ((alignment.x + 1) / 2) * (size_.width - childSize.width);
        float offsetY = ((alignment.y + 1) / 2) * (size_.height - childSize.height);
        child->setPosition({position_.x + offsetX, position_.y + offsetY});
    }
}

void StackRenderObject::paint() {
    // Apply clipping if needed
    if (clipBehavior != Clip::None) {
        // Platform-specific: pushClip(position_, size_, clipBehavior)
    }
    
    for (auto& child : children_) {
        child->paint();
    }
    
    if (clipBehavior != Clip::None) {
        // Platform-specific: popClip()
    }
}

std::unique_ptr<RenderObject> Stack::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<StackRenderObject>();
    ro->alignment = alignment_;
    ro->fit = fit_;
    ro->clipBehavior = clipBehavior_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Positioned
// ═══════════════════════════════════════════════════════════════════════════════

class PositionedRenderObject : public RenderObject {
public:
    std::optional<float> left, top, right, bottom;
    std::optional<float> width, height;
    
    void layout(float maxWidth, float maxHeight) override {
        // Calculate size from constraints
        float calcWidth = maxWidth;
        float calcHeight = maxHeight;
        
        if (width) {
            calcWidth = *width;
        } else if (left && right) {
            calcWidth = maxWidth - *left - *right;
        }
        
        if (height) {
            calcHeight = *height;
        } else if (top && bottom) {
            calcHeight = maxHeight - *top - *bottom;
        }
        
        if (!children_.empty()) {
            children_[0]->layout(calcWidth, calcHeight);
            size_ = children_[0]->getSize();
        } else {
            size_ = {calcWidth, calcHeight};
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> Positioned::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<PositionedRenderObject>();
    ro->left = left_;
    ro->top = top_;
    ro->right = right_;
    ro->bottom = bottom_;
    ro->width = width_;
    ro->height = height_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Center
// ═══════════════════════════════════════════════════════════════════════════════

class CenterRenderObject : public RenderObject {
public:
    void layout(float maxWidth, float maxHeight) override {
        size_ = {maxWidth, maxHeight};
        
        if (!children_.empty()) {
            children_[0]->layout(maxWidth, maxHeight);
            auto childSize = children_[0]->getSize();
            float offsetX = (maxWidth - childSize.width) / 2;
            float offsetY = (maxHeight - childSize.height) / 2;
            children_[0]->setPosition({position_.x + offsetX, position_.y + offsetY});
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> Center::createRenderObject(BuildContext* ctx) {
    return std::make_unique<CenterRenderObject>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Align
// ═══════════════════════════════════════════════════════════════════════════════

class AlignRenderObject : public RenderObject {
public:
    AlignmentGeometry alignment;
    std::optional<float> widthFactor, heightFactor;
    
    void layout(float maxWidth, float maxHeight) override {
        if (!children_.empty()) {
            children_[0]->layout(maxWidth, maxHeight);
            auto childSize = children_[0]->getSize();
            
            size_.width = widthFactor ? childSize.width * *widthFactor : maxWidth;
            size_.height = heightFactor ? childSize.height * *heightFactor : maxHeight;
            
            float offsetX = ((alignment.x + 1) / 2) * (size_.width - childSize.width);
            float offsetY = ((alignment.y + 1) / 2) * (size_.height - childSize.height);
            children_[0]->setPosition({position_.x + offsetX, position_.y + offsetY});
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> Align::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<AlignRenderObject>();
    ro->alignment = alignment_;
    ro->widthFactor = widthFactor_;
    ro->heightFactor = heightFactor_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Padding
// ═══════════════════════════════════════════════════════════════════════════════

class PaddingRenderObject : public RenderObject {
public:
    EdgeInsets padding;
    
    void layout(float maxWidth, float maxHeight) override {
        float availableWidth = maxWidth - padding.left - padding.right;
        float availableHeight = maxHeight - padding.top - padding.bottom;
        
        if (!children_.empty()) {
            children_[0]->layout(availableWidth, availableHeight);
            auto childSize = children_[0]->getSize();
            children_[0]->setPosition({position_.x + padding.left, position_.y + padding.top});
            size_.width = childSize.width + padding.left + padding.right;
            size_.height = childSize.height + padding.top + padding.bottom;
        } else {
            size_ = {padding.left + padding.right, padding.top + padding.bottom};
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> Padding::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<PaddingRenderObject>();
    ro->padding = padding_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ConstrainedBox
// ═══════════════════════════════════════════════════════════════════════════════

class ConstrainedBoxRenderObject : public RenderObject {
public:
    BoxConstraints constraints;
    
    void layout(float maxWidth, float maxHeight) override {
        float w = std::max(constraints.minWidth, std::min(constraints.maxWidth, maxWidth));
        float h = std::max(constraints.minHeight, std::min(constraints.maxHeight, maxHeight));
        
        if (!children_.empty()) {
            children_[0]->layout(w, h);
            size_ = children_[0]->getSize();
        } else {
            size_ = {constraints.minWidth, constraints.minHeight};
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> ConstrainedBox::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<ConstrainedBoxRenderObject>();
    ro->constraints = constraints_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AspectRatio
// ═══════════════════════════════════════════════════════════════════════════════

class AspectRatioRenderObject : public RenderObject {
public:
    float aspectRatio = 1.0f;
    
    void layout(float maxWidth, float maxHeight) override {
        // Try to maximize width first
        float width = maxWidth;
        float height = width / aspectRatio;
        
        if (height > maxHeight) {
            height = maxHeight;
            width = height * aspectRatio;
        }
        
        size_.width = width;
        size_.height = height;
        
        if (!children_.empty()) {
            children_[0]->layout(width, height);
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> AspectRatio::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<AspectRatioRenderObject>();
    ro->aspectRatio = aspectRatio_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  FractionallySizedBox
// ═══════════════════════════════════════════════════════════════════════════════

class FractionallySizedBoxRenderObject : public RenderObject {
public:
    std::optional<float> widthFactor, heightFactor;
    AlignmentGeometry alignment;
    
    void layout(float maxWidth, float maxHeight) override {
        float childWidth = widthFactor ? maxWidth * *widthFactor : maxWidth;
        float childHeight = heightFactor ? maxHeight * *heightFactor : maxHeight;
        
        size_ = {maxWidth, maxHeight};
        
        if (!children_.empty()) {
            children_[0]->layout(childWidth, childHeight);
            auto childSize = children_[0]->getSize();
            
            float offsetX = ((alignment.x + 1) / 2) * (size_.width - childSize.width);
            float offsetY = ((alignment.y + 1) / 2) * (size_.height - childSize.height);
            children_[0]->setPosition({position_.x + offsetX, position_.y + offsetY});
        }
    }
    
    void paint() override {
        for (auto& child : children_) {
            child->paint();
        }
    }
};

std::unique_ptr<RenderObject> FractionallySizedBox::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<FractionallySizedBoxRenderObject>();
    ro->widthFactor = widthFactor_;
    ro->heightFactor = heightFactor_;
    ro->alignment = alignment_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Wrap
// ═══════════════════════════════════════════════════════════════════════════════

void WrapRenderObject::layout(float maxWidth, float maxHeight) {
    if (direction == Axis::Horizontal) {
        // Horizontal wrap
        float currentX = 0;
        float currentY = 0;
        float rowHeight = 0;
        
        for (auto& child : children_) {
            child->layout(maxWidth, maxHeight);
            auto childSize = child->getSize();
            
            if (currentX + childSize.width > maxWidth && currentX > 0) {
                // Wrap to next line
                currentX = 0;
                currentY += rowHeight + runSpacing;
                rowHeight = 0;
            }
            
            child->setPosition({position_.x + currentX, position_.y + currentY});
            currentX += childSize.width + spacing;
            rowHeight = std::max(rowHeight, childSize.height);
        }
        
        size_.width = maxWidth;
        size_.height = currentY + rowHeight;
    } else {
        // Vertical wrap
        float currentX = 0;
        float currentY = 0;
        float colWidth = 0;
        
        for (auto& child : children_) {
            child->layout(maxWidth, maxHeight);
            auto childSize = child->getSize();
            
            if (currentY + childSize.height > maxHeight && currentY > 0) {
                currentY = 0;
                currentX += colWidth + runSpacing;
                colWidth = 0;
            }
            
            child->setPosition({position_.x + currentX, position_.y + currentY});
            currentY += childSize.height + spacing;
            colWidth = std::max(colWidth, childSize.width);
        }
        
        size_.width = currentX + colWidth;
        size_.height = maxHeight;
    }
}

void WrapRenderObject::paint() {
    for (auto& child : children_) {
        child->paint();
    }
}

std::unique_ptr<RenderObject> Wrap::createRenderObject(BuildContext* ctx) {
    auto ro = std::make_unique<WrapRenderObject>();
    ro->direction = direction_;
    ro->alignment = alignment_;
    ro->spacing = spacing_;
    ro->runSpacing = runSpacing_;
    return ro;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ListView State
// ═══════════════════════════════════════════════════════════════════════════════

class ListViewState : public State<ListView> {
public:
    float scrollOffset = 0;
    
    WidgetPtr build(BuildContext* ctx) override {
        auto w = widget();
        
        std::vector<WidgetPtr> children;
        if (w->reverse()) {
            for (auto it = w->children().rbegin(); it != w->children().rend(); ++it) {
                children.push_back(*it);
            }
        } else {
            children = w->children();
        }
        
        // Wrap in a scrollable column/row
        if (w->scrollDirection() == Axis::Vertical) {
            return std::make_shared<Padding>(w->padding(),
                std::make_shared<Column>(std::move(children))
            );
        } else {
            return std::make_shared<Padding>(w->padding(),
                std::make_shared<Row>(std::move(children))
            );
        }
    }
};

std::unique_ptr<State_Base> ListView::createState() {
    return std::make_unique<ListViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SingleChildScrollView State
// ═══════════════════════════════════════════════════════════════════════════════

class SingleChildScrollViewState : public State<SingleChildScrollView> {
public:
    float scrollOffset = 0;
    
    WidgetPtr build(BuildContext* ctx) override {
        // For now, just return the child with padding
        // Full scroll behavior needs platform integration
        auto w = widget();
        if (w) {
            // Access private child_ via friend or getter
            // For simplicity, return a placeholder
        }
        return std::make_shared<SizedBox>();
    }
};

std::unique_ptr<State_Base> SingleChildScrollView::createState() {
    return std::make_unique<SingleChildScrollViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GridView State
// ═══════════════════════════════════════════════════════════════════════════════

class GridViewState : public State<GridView> {
public:
    WidgetPtr build(BuildContext* ctx) override {
        // Build grid using Wrap
        auto w = widget();
        if (!w) return std::make_shared<SizedBox>();
        
        // Create wrapped children
        return std::make_shared<Wrap>(
            w->children(),  // Note: needs accessor
            Axis::Horizontal,
            WrapAlignment::Start,
            0,  // spacing - needs accessor
            0   // runSpacing - needs accessor
        );
    }
};

std::unique_ptr<State_Base> GridView::createState() {
    return std::make_unique<GridViewState>();
}

} // namespace flutter
} // namespace sad
