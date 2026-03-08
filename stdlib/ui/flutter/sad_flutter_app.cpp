// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_app.cpp — تنفيذ نظام التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_app.h"
#include "sad_flutter_arabic.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Navigator
// ═══════════════════════════════════════════════════════════════════════════════

NavigatorState* Navigator::of(BuildContext* context) {
    // Find the Navigator element in the tree
    auto* element = context->findAncestorElementOfType<StatefulElement>();
    while (element) {
        auto* state = element->state();
        if (auto* navState = dynamic_cast<NavigatorState*>(state)) {
            return navState;
        }
        // Move up
        element = element->parent() ? 
            dynamic_cast<StatefulElement*>(element->parent()) : nullptr;
    }
    return nullptr;
}

std::unique_ptr<State_Base> Navigator::createState() {
    return std::make_unique<NavigatorState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AppBar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr AppBar::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    // Leading (back button or menu)
    if (leading_) {
        rowChildren.push_back(leading_);
    } else if (automaticallyImplyLeading_) {
        // Check if we can pop
        auto* nav = Navigator::of(ctx);
        if (nav && nav->canPop()) {
            rowChildren.push_back(
                std::make_shared<IconButton>(
                    Icons::ArrowForward,  // RTL: forward arrow for back
                    [nav]{ nav->pop(); },
                    24,
                    foregroundColor_
                )
            );
        }
    }
    
    // Title
    rowChildren.push_back(std::make_shared<Expanded>(
        titleWidget_ ? titleWidget_ : نص(title_, TextStyle().setColor(foregroundColor_).setFontSize(20))
    ));
    
    // Actions
    for (auto& action : actions_) {
        rowChildren.push_back(action);
    }
    
    BoxDecoration deco;
    deco.setColor(backgroundColor_);
    if (elevation_ > 0) {
        deco.addShadow(BoxShadow{Color(0,0,0,32), elevation_, 0, 0, elevation_/2});
    }
    
    return std::make_shared<Container>(
        std::make_shared<Row>(
            std::move(rowChildren),
            MainAxisAlignment::Start,
            MainAxisSize::Max,
            CrossAxisAlignment::Center
        ),
        std::nullopt,  // width
        56.0f,         // height (standard AppBar height)
        EdgeInsets::symmetric(0, 16),  // padding
        EdgeInsets(),
        deco
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  FloatingActionButton
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr FloatingActionButton::build(BuildContext* ctx) {
    BoxDecoration deco;
    deco.setColor(backgroundColor_)
        .setRadius(28)  // Circular
        .addShadow(BoxShadow{Color(0,0,0,48), elevation_, 0, 0, elevation_/2});
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            child_ ? child_ : ايقونة(Icons::Add, 24, foregroundColor_),
            56.0f, 56.0f,  // Standard FAB size
            EdgeInsets::all(16),
            EdgeInsets(),
            deco
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  BottomNavigationBar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr BottomNavigationBar::build(BuildContext* ctx) {
    std::vector<WidgetPtr> itemWidgets;
    
    for (size_t i = 0; i < items_.size(); ++i) {
        const auto& item = items_[i];
        bool selected = static_cast<int>(i) == currentIndex_;
        Color itemColor = selected ? selectedItemColor_ : unselectedItemColor_;
        
        auto itemWidget = std::make_shared<Expanded>(
            std::make_shared<GestureDetector>(
                عمود({
                    ايقونة(item.icon, 24, itemColor),
                    فراغ_عمودي(4),
                    نص(item.label, TextStyle().setFontSize(12).setColor(itemColor))
                }, MainAxisAlignment::Center, CrossAxisAlignment::Center),
                [this, i]{ if (onTap_) onTap_(static_cast<int>(i)); }
            )
        );
        itemWidgets.push_back(itemWidget);
    }
    
    BoxDecoration deco;
    deco.setColor(backgroundColor_)
        .addShadow(BoxShadow{Color(0,0,0,32), 8, 0, 0, -4});
    
    return std::make_shared<Container>(
        std::make_shared<Row>(
            std::move(itemWidgets),
            MainAxisAlignment::SpaceAround,
            MainAxisSize::Max,
            CrossAxisAlignment::Center
        ),
        std::nullopt,  // width
        56.0f,         // height
        EdgeInsets::symmetric(8, 0),
        EdgeInsets(),
        deco
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Scaffold
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Scaffold::build(BuildContext* ctx) {
    std::vector<WidgetPtr> columnChildren;
    
    // AppBar at top
    if (appBar_) {
        columnChildren.push_back(appBar_);
    }
    
    // Body (expanded to fill remaining space)
    if (body_) {
        columnChildren.push_back(std::make_shared<Expanded>(body_));
    }
    
    // Bottom navigation bar
    if (bottomNavigationBar_) {
        columnChildren.push_back(bottomNavigationBar_);
    }
    
    // Main column
    WidgetPtr mainContent = std::make_shared<Column>(
        std::move(columnChildren),
        MainAxisAlignment::Start,
        MainAxisSize::Max,
        CrossAxisAlignment::Stretch
    );
    
    // Wrap in Stack if we have FAB
    if (floatingActionButton_) {
        mainContent = std::make_shared<Stack>(
            std::vector<WidgetPtr>{
                mainContent,
                موقع(
                    floatingActionButton_,
                    16.0f,  // left (RTL: right side)
                    std::nullopt,
                    std::nullopt,
                    bottomNavigationBar_ ? 72.0f : 16.0f  // bottom
                )
            }
        );
    }
    
    // Background color
    BoxDecoration deco;
    deco.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        mainContent,
        std::nullopt, std::nullopt,
        EdgeInsets(),
        EdgeInsets(),
        deco
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  WidgetsApp State
// ═══════════════════════════════════════════════════════════════════════════════

class WidgetsAppState : public State<WidgetsApp> {
public:
    WidgetPtr build(BuildContext* ctx) override {
        auto w = widget();
        if (!w) return std::make_shared<SizedBox>();
        
        // Build route map with home
        auto routes = w->routes();
        if (w->home()) {
            routes["/"] = [home = w->home()](BuildContext*) { return home; };
        }
        
        // Wrap content in Theme and MediaQuery
        WidgetPtr content = std::make_shared<Navigator>(
            w->initialRoute(),
            w->onGenerateRoute(),
            routes
        );
        
        // Add MediaQuery
        content = std::make_shared<MediaQuery>(
            WidgetsBinding::instance().mediaQueryData(),
            content
        );
        
        // Add Theme
        content = std::make_shared<Theme>(w->theme(), content);
        
        // Debug banner (top-left corner)
        if (w->debugShowCheckedModeBanner()) {
            content = std::make_shared<Stack>(
                std::vector<WidgetPtr>{
                    content,
                    موقع(
                        std::make_shared<Container>(
                            نص("DEBUG", TextStyle().setColor(Color::white()).setFontSize(10)),
                            std::nullopt, std::nullopt,
                            EdgeInsets::symmetric(2, 8),
                            EdgeInsets(),
                            BoxDecoration().setColor(Color::red())
                        ),
                        std::nullopt, 0.0f, 0.0f, std::nullopt  // top-right for RTL
                    )
                }
            );
        }
        
        return content;
    }
};

std::unique_ptr<State_Base> WidgetsApp::createState() {
    return std::make_unique<WidgetsAppState>();
}

} // namespace flutter
} // namespace sad
