// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_scroll.cpp — تطبيق مكونات التمرير
//  Scrolling Widgets Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_scroll.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  SingleChildScrollView State
// ═══════════════════════════════════════════════════════════════════════════════

class SingleChildScrollViewState : public State<SingleChildScrollView> {
public:
    void initState() override {
        auto* w = widget();
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<ScrollController>();
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        // TODO: تطبيق التمرير الفعلي
        WidgetPtr content = w->child_;
        
        if (w->padding_.top > 0 || w->padding_.bottom > 0 ||
            w->padding_.left > 0 || w->padding_.right > 0) {
            content = std::make_shared<Padding>(content, w->padding_);
        }
        
        return content;
    }
    
private:
    std::shared_ptr<ScrollController> controller_;
    
    SingleChildScrollView* widget() const {
        return static_cast<SingleChildScrollView*>(widget_);
    }
};

std::unique_ptr<State_Base> SingleChildScrollView::createState() {
    return std::make_unique<SingleChildScrollViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ListView Static Methods
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<ListView> ListView::builder(
    int itemCount,
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
    Axis scrollDirection,
    bool reverse,
    std::shared_ptr<ScrollController> controller,
    std::shared_ptr<ScrollPhysics> physics,
    bool shrinkWrap,
    EdgeInsets padding,
    double itemExtent) {
    
    auto list = std::make_shared<ListView>(
        std::vector<WidgetPtr>{},
        scrollDirection, reverse, controller, physics,
        shrinkWrap, padding, itemExtent
    );
    list->itemCount_ = itemCount;
    list->itemBuilder_ = itemBuilder;
    return list;
}

std::shared_ptr<ListView> ListView::separated(
    int itemCount,
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
    std::function<WidgetPtr(BuildContext*, int)> separatorBuilder,
    Axis scrollDirection,
    bool reverse,
    std::shared_ptr<ScrollController> controller,
    std::shared_ptr<ScrollPhysics> physics,
    bool shrinkWrap,
    EdgeInsets padding) {
    
    auto list = std::make_shared<ListView>(
        std::vector<WidgetPtr>{},
        scrollDirection, reverse, controller, physics,
        shrinkWrap, padding
    );
    list->itemCount_ = itemCount;
    list->itemBuilder_ = itemBuilder;
    list->separatorBuilder_ = separatorBuilder;
    return list;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ListView State
// ═══════════════════════════════════════════════════════════════════════════════

class ListViewState : public State<ListView> {
public:
    void initState() override {
        auto* w = widget();
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<ScrollController>();
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> items;
        
        if (w->itemBuilder_) {
            // Builder mode
            for (int i = 0; i < w->itemCount_; ++i) {
                items.push_back(w->itemBuilder_(ctx, i));
                
                if (w->separatorBuilder_ && i < w->itemCount_ - 1) {
                    items.push_back(w->separatorBuilder_(ctx, i));
                }
            }
        } else {
            // Static children
            items = w->children_;
        }
        
        WidgetPtr content;
        if (w->scrollDirection_ == Axis::Vertical) {
            content = std::make_shared<Column>(
                std::move(items),
                MainAxisAlignment::Start,
                w->shrinkWrap_ ? MainAxisSize::Min : MainAxisSize::Max,
                CrossAxisAlignment::Stretch
            );
        } else {
            content = std::make_shared<Row>(
                std::move(items),
                MainAxisAlignment::Start,
                w->shrinkWrap_ ? MainAxisSize::Min : MainAxisSize::Max,
                CrossAxisAlignment::Stretch
            );
        }
        
        if (w->padding_.top > 0 || w->padding_.bottom > 0 ||
            w->padding_.left > 0 || w->padding_.right > 0) {
            content = std::make_shared<Padding>(content, w->padding_);
        }
        
        return content;
    }
    
private:
    std::shared_ptr<ScrollController> controller_;
    
    ListView* widget() const {
        return static_cast<ListView*>(widget_);
    }
};

std::unique_ptr<State_Base> ListView::createState() {
    return std::make_unique<ListViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GridView Static Methods
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<GridView> GridView::count(
    int crossAxisCount,
    std::vector<WidgetPtr> children,
    double mainAxisSpacing,
    double crossAxisSpacing,
    double childAspectRatio,
    Axis scrollDirection,
    EdgeInsets padding) {
    
    auto delegate = std::make_shared<SliverGridDelegateWithFixedCrossAxisCount>(
        crossAxisCount, mainAxisSpacing, crossAxisSpacing, childAspectRatio
    );
    
    return std::make_shared<GridView>(
        std::move(children), delegate, scrollDirection,
        false, nullptr, nullptr, false, padding
    );
}

std::shared_ptr<GridView> GridView::extent(
    double maxCrossAxisExtent,
    std::vector<WidgetPtr> children,
    double mainAxisSpacing,
    double crossAxisSpacing,
    double childAspectRatio,
    Axis scrollDirection,
    EdgeInsets padding) {
    
    auto delegate = std::make_shared<SliverGridDelegateWithMaxCrossAxisExtent>(
        maxCrossAxisExtent, mainAxisSpacing, crossAxisSpacing, childAspectRatio
    );
    
    return std::make_shared<GridView>(
        std::move(children), delegate, scrollDirection,
        false, nullptr, nullptr, false, padding
    );
}

std::shared_ptr<GridView> GridView::builder(
    std::shared_ptr<SliverGridDelegate> gridDelegate,
    int itemCount,
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
    Axis scrollDirection,
    bool reverse,
    std::shared_ptr<ScrollController> controller,
    EdgeInsets padding) {
    
    auto grid = std::make_shared<GridView>(
        std::vector<WidgetPtr>{}, gridDelegate, scrollDirection,
        reverse, controller, nullptr, false, padding
    );
    grid->itemCount_ = itemCount;
    grid->itemBuilder_ = itemBuilder;
    return grid;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GridView State
// ═══════════════════════════════════════════════════════════════════════════════

class GridViewState : public State<GridView> {
public:
    void initState() override {
        auto* w = widget();
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<ScrollController>();
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> items;
        
        if (w->itemBuilder_) {
            for (int i = 0; i < w->itemCount_; ++i) {
                items.push_back(w->itemBuilder_(ctx, i));
            }
        } else {
            items = w->children_;
        }
        
        // TODO: تطبيق الشبكة الفعلية مع التفويض
        int crossAxisCount = 2;
        if (auto* fixed = dynamic_cast<SliverGridDelegateWithFixedCrossAxisCount*>(w->gridDelegate_.get())) {
            crossAxisCount = fixed->crossAxisCount_;
        }
        
        std::vector<WidgetPtr> rows;
        std::vector<WidgetPtr> currentRow;
        
        for (size_t i = 0; i < items.size(); ++i) {
            currentRow.push_back(std::make_shared<Expanded>(items[i]));
            
            if (currentRow.size() == static_cast<size_t>(crossAxisCount) || i == items.size() - 1) {
                // ملء الصف المتبقي
                while (currentRow.size() < static_cast<size_t>(crossAxisCount)) {
                    currentRow.push_back(std::make_shared<Expanded>(std::make_shared<SizedBox>()));
                }
                
                rows.push_back(std::make_shared<Row>(std::move(currentRow)));
                currentRow.clear();
            }
        }
        
        WidgetPtr content = std::make_shared<Column>(std::move(rows));
        
        if (w->padding_.top > 0 || w->padding_.bottom > 0 ||
            w->padding_.left > 0 || w->padding_.right > 0) {
            content = std::make_shared<Padding>(content, w->padding_);
        }
        
        return content;
    }
    
private:
    std::shared_ptr<ScrollController> controller_;
    
    GridView* widget() const {
        return static_cast<GridView*>(widget_);
    }
};

std::unique_ptr<State_Base> GridView::createState() {
    return std::make_unique<GridViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Sliver Widgets
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SliverList::build(BuildContext* ctx) {
    std::vector<WidgetPtr> items;
    
    int count = childCount_ > 0 ? childCount_ : 100; // افتراضي
    for (int i = 0; i < count; ++i) {
        WidgetPtr item = delegate_(ctx, i);
        if (!item) break;
        items.push_back(item);
    }
    
    return std::make_shared<Column>(std::move(items));
}

WidgetPtr SliverGrid::build(BuildContext* ctx) {
    // TODO: تطبيق كامل
    return std::make_shared<SizedBox>();
}

WidgetPtr SliverAppBar::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    if (leading_) {
        rowChildren.push_back(leading_);
    }
    
    if (title_) {
        rowChildren.push_back(std::make_shared<Expanded>(title_));
    } else {
        rowChildren.push_back(std::make_shared<Spacer>());
    }
    
    for (const auto& action : actions_) {
        rowChildren.push_back(action);
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    float height = expandedHeight_ > 0 ? expandedHeight_ : 56.0f;
    
    return std::make_shared<Container>(
        std::make_shared<Stack>(
            std::vector<WidgetPtr>{
                flexibleSpace_ ? flexibleSpace_ : std::make_shared<SizedBox>(),
                std::make_shared<Positioned>(
                    std::make_shared<Padding>(
                        std::make_shared<Row>(std::move(rowChildren)),
                        EdgeInsets::symmetric(16, 0)
                    ),
                    0.0f, std::nullopt, 0.0f, 0.0f
                )
            }
        ),
        std::nullopt, height,
        EdgeInsets{}, EdgeInsets{},
        decoration
    );
}

WidgetPtr SliverPadding::build(BuildContext* ctx) {
    return std::make_shared<Padding>(sliver_->build(ctx), padding_);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CustomScrollView State
// ═══════════════════════════════════════════════════════════════════════════════

class CustomScrollViewState : public State<CustomScrollView> {
public:
    void initState() override {
        auto* w = widget();
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<ScrollController>();
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> sliverWidgets;
        for (const auto& sliver : w->slivers_) {
            sliverWidgets.push_back(sliver->build(ctx));
        }
        
        return std::make_shared<Column>(std::move(sliverWidgets));
    }
    
private:
    std::shared_ptr<ScrollController> controller_;
    
    CustomScrollView* widget() const {
        return static_cast<CustomScrollView*>(widget_);
    }
};

std::unique_ptr<State_Base> CustomScrollView::createState() {
    return std::make_unique<CustomScrollViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PageView Static Methods
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<PageView> PageView::builder(
    int itemCount,
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
    Axis scrollDirection,
    std::shared_ptr<PageController> controller,
    std::function<void(int)> onPageChanged) {
    
    auto view = std::make_shared<PageView>(
        std::vector<WidgetPtr>{},
        scrollDirection, false, controller, nullptr, onPageChanged
    );
    view->itemCount_ = itemCount;
    view->itemBuilder_ = itemBuilder;
    return view;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PageView State
// ═══════════════════════════════════════════════════════════════════════════════

class PageViewState : public State<PageView> {
public:
    void initState() override {
        auto* w = widget();
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<PageController>();
        }
        currentPage_ = controller_->page();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        WidgetPtr currentPageWidget;
        
        if (w->itemBuilder_) {
            currentPageWidget = w->itemBuilder_(ctx, currentPage_);
        } else if (currentPage_ < static_cast<int>(w->children_.size())) {
            currentPageWidget = w->children_[currentPage_];
        } else {
            currentPageWidget = std::make_shared<SizedBox>();
        }
        
        // TODO: تطبيق التمرير والتبديل بين الصفحات
        return currentPageWidget;
    }
    
private:
    std::shared_ptr<PageController> controller_;
    int currentPage_ = 0;
    
    PageView* widget() const {
        return static_cast<PageView*>(widget_);
    }
};

std::unique_ptr<State_Base> PageView::createState() {
    return std::make_unique<PageViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  NestedScrollView State
// ═══════════════════════════════════════════════════════════════════════════════

class NestedScrollViewState : public State<NestedScrollView> {
public:
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        auto headerSlivers = w->headerSliverBuilder_(ctx);
        
        std::vector<WidgetPtr> allWidgets;
        for (const auto& sliver : headerSlivers) {
            allWidgets.push_back(sliver->build(ctx));
        }
        allWidgets.push_back(std::make_shared<Expanded>(w->body_));
        
        return std::make_shared<Column>(std::move(allWidgets));
    }
    
private:
    NestedScrollView* widget() const {
        return static_cast<NestedScrollView*>(widget_);
    }
};

std::unique_ptr<State_Base> NestedScrollView::createState() {
    return std::make_unique<NestedScrollViewState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Scrollbar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Scrollbar::build(BuildContext* ctx) {
    // TODO: تطبيق شريط التمرير الفعلي
    return child_;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ReorderableListView State
// ═══════════════════════════════════════════════════════════════════════════════

class ReorderableListViewState : public State<ReorderableListView> {
public:
    void initState() override {
        auto* w = widget();
        items_ = w->children_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> listItems;
        
        for (size_t i = 0; i < items_.size(); ++i) {
            WidgetPtr item = items_[i];
            
            if (w->buildDefaultDragHandles_) {
                // إضافة مقبض السحب
                item = std::make_shared<Row>(
                    std::vector<WidgetPtr>{
                        std::make_shared<Icon>(IconData{"drag_handle", 0xE25D}, 24.0f),
                        std::make_shared<Expanded>(item)
                    }
                );
            }
            
            listItems.push_back(item);
        }
        
        WidgetPtr content;
        if (w->scrollDirection_ == Axis::Vertical) {
            content = std::make_shared<Column>(std::move(listItems));
        } else {
            content = std::make_shared<Row>(std::move(listItems));
        }
        
        if (w->padding_.top > 0 || w->padding_.bottom > 0 ||
            w->padding_.left > 0 || w->padding_.right > 0) {
            content = std::make_shared<Padding>(content, w->padding_);
        }
        
        return content;
    }
    
private:
    std::vector<WidgetPtr> items_;
    
    ReorderableListView* widget() const {
        return static_cast<ReorderableListView*>(widget_);
    }
};

std::unique_ptr<State_Base> ReorderableListView::createState() {
    return std::make_unique<ReorderableListViewState>();
}

} // namespace flutter
} // namespace sad
