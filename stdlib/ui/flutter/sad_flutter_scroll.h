// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_scroll.h — مكونات التمرير
//  Scrolling Widgets
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_SCROLL_H
#define SAD_FLUTTER_SCROLL_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  ScrollController — متحكم التمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) متحكم التمرير
 * @brief (EN) Scroll controller
 */
class ScrollController {
public:
    ScrollController(double initialScrollOffset = 0.0,
                     bool keepScrollOffset = true)
        : offset_(initialScrollOffset), keepScrollOffset_(keepScrollOffset) {}
    
    double offset() const { return offset_; }
    void setOffset(double offset) { offset_ = offset; notifyListeners(); }
    
    void jumpTo(double value) { offset_ = value; notifyListeners(); }
    
    void animateTo(double value, Duration duration, Curve curve = Curves::ease) {
        // TODO: تطبيق الحركة
        offset_ = value;
        notifyListeners();
    }
    
    void addListener(std::function<void()> listener) {
        listeners_.push_back(listener);
    }
    
    void removeListener(std::function<void()> listener) {
        // TODO: implement removal
    }
    
    bool hasClients() const { return hasClients_; }
    void attach() { hasClients_ = true; }
    void detach() { hasClients_ = false; }
    
private:
    double offset_ = 0.0;
    bool keepScrollOffset_ = true;
    bool hasClients_ = false;
    std::vector<std::function<void()>> listeners_;
    
    void notifyListeners() {
        for (auto& l : listeners_) l();
    }
};

using متحكم_تمرير = ScrollController;

// ═══════════════════════════════════════════════════════════════════════════════
//  ScrollPhysics — فيزياء التمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فيزياء التمرير
 * @brief (EN) Scroll physics
 */
class ScrollPhysics {
public:
    virtual ~ScrollPhysics() = default;
    
    virtual double applyPhysicsToUserOffset(double offset) const {
        return offset;
    }
    
    virtual bool shouldAcceptUserOffset() const { return true; }
    
    virtual double applyBoundaryConditions(double value, double min, double max) const {
        if (value < min) return value - min;
        if (value > max) return value - max;
        return 0.0;
    }
};

using فيزياء_تمرير = ScrollPhysics;

/**
 * @brief (AR) فيزياء ارتداد (iOS style)
 * @brief (EN) Bouncing scroll physics
 */
class BouncingScrollPhysics : public ScrollPhysics {
public:
    double applyPhysicsToUserOffset(double offset) const override {
        return offset * 0.5; // تقليل السرعة عند الحدود
    }
};

using فيزياء_ارتداد = BouncingScrollPhysics;

/**
 * @brief (AR) فيزياء تثبيت (Android style)
 * @brief (EN) Clamping scroll physics
 */
class ClampingScrollPhysics : public ScrollPhysics {
public:
    double applyBoundaryConditions(double value, double min, double max) const override {
        return std::clamp(value, min, max) - value;
    }
};

using فيزياء_تثبيت = ClampingScrollPhysics;

/**
 * @brief (AR) فيزياء بدون تمرير
 * @brief (EN) Never scrollable physics
 */
class NeverScrollableScrollPhysics : public ScrollPhysics {
public:
    bool shouldAcceptUserOffset() const override { return false; }
};

using فيزياء_بدون_تمرير = NeverScrollableScrollPhysics;

/**
 * @brief (AR) فيزياء تمرير دائم
 * @brief (EN) Always scrollable physics
 */
class AlwaysScrollableScrollPhysics : public ScrollPhysics {
public:
    bool shouldAcceptUserOffset() const override { return true; }
};

using فيزياء_تمرير_دائم = AlwaysScrollableScrollPhysics;

// ═══════════════════════════════════════════════════════════════════════════════
//  Axis & Direction
// ═══════════════════════════════════════════════════════════════════════════════

enum class Axis {
    Horizontal,
    Vertical
};

using محور = Axis;

enum class AxisDirection {
    Up,
    Down,
    Left,
    Right
};

using اتجاه_محور = AxisDirection;

enum class ScrollDirection {
    Idle,
    Forward,
    Reverse
};

using اتجاه_تمرير = ScrollDirection;

// ═══════════════════════════════════════════════════════════════════════════════
//  ScrollPosition — موقع التمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) موقع التمرير
 * @brief (EN) Scroll position
 */
struct ScrollPosition {
    double pixels = 0.0;
    double minScrollExtent = 0.0;
    double maxScrollExtent = 0.0;
    double viewportDimension = 0.0;
    AxisDirection axisDirection = AxisDirection::Down;
    
    bool atEdge() const {
        return pixels <= minScrollExtent || pixels >= maxScrollExtent;
    }
    
    double extentBefore() const { return pixels - minScrollExtent; }
    double extentAfter() const { return maxScrollExtent - pixels; }
    double extentInside() const { return viewportDimension; }
};

using موقع_تمرير = ScrollPosition;

// ═══════════════════════════════════════════════════════════════════════════════
//  ScrollNotification — إشعار التمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إشعار التمرير
 * @brief (EN) Scroll notification
 */
struct ScrollNotification {
    ScrollPosition metrics;
    int depth = 0;
};

struct ScrollStartNotification : ScrollNotification {};
struct ScrollUpdateNotification : ScrollNotification {
    double scrollDelta = 0.0;
};
struct ScrollEndNotification : ScrollNotification {};
struct OverscrollNotification : ScrollNotification {
    double overscroll = 0.0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  SingleChildScrollView — عرض تمرير بطفل واحد
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عرض تمرير بطفل واحد
 * @brief (EN) Single child scroll view
 */
class SingleChildScrollView : public StatefulWidget {
public:
    SingleChildScrollView(WidgetPtr child,
                          Axis scrollDirection = Axis::Vertical,
                          bool reverse = false,
                          EdgeInsets padding = EdgeInsets{},
                          std::shared_ptr<ScrollController> controller = nullptr,
                          std::shared_ptr<ScrollPhysics> physics = nullptr,
                          bool primary = false)
        : child_(child), scrollDirection_(scrollDirection), reverse_(reverse),
          padding_(padding), controller_(controller), physics_(physics),
          primary_(primary) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    WidgetPtr child_;
    Axis scrollDirection_;
    bool reverse_;
    EdgeInsets padding_;
    std::shared_ptr<ScrollController> controller_;
    std::shared_ptr<ScrollPhysics> physics_;
    bool primary_;
};

using عرض_تمرير = SingleChildScrollView;

// ═══════════════════════════════════════════════════════════════════════════════
//  ListView — قائمة قابلة للتمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) قائمة قابلة للتمرير
 * @brief (EN) Scrollable list view
 */
class ListView : public StatefulWidget {
public:
    // Constructor for static children
    ListView(std::vector<WidgetPtr> children,
             Axis scrollDirection = Axis::Vertical,
             bool reverse = false,
             std::shared_ptr<ScrollController> controller = nullptr,
             std::shared_ptr<ScrollPhysics> physics = nullptr,
             bool shrinkWrap = false,
             EdgeInsets padding = EdgeInsets{},
             double itemExtent = 0.0,
             bool addAutomaticKeepAlives = true,
             bool addRepaintBoundaries = true)
        : children_(std::move(children)), scrollDirection_(scrollDirection),
          reverse_(reverse), controller_(controller), physics_(physics),
          shrinkWrap_(shrinkWrap), padding_(padding), itemExtent_(itemExtent),
          addAutomaticKeepAlives_(addAutomaticKeepAlives),
          addRepaintBoundaries_(addRepaintBoundaries) {}
    
    // Builder constructor
    static std::shared_ptr<ListView> builder(
        int itemCount,
        std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
        Axis scrollDirection = Axis::Vertical,
        bool reverse = false,
        std::shared_ptr<ScrollController> controller = nullptr,
        std::shared_ptr<ScrollPhysics> physics = nullptr,
        bool shrinkWrap = false,
        EdgeInsets padding = EdgeInsets{},
        double itemExtent = 0.0
    );
    
    // Separated constructor
    static std::shared_ptr<ListView> separated(
        int itemCount,
        std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
        std::function<WidgetPtr(BuildContext*, int)> separatorBuilder,
        Axis scrollDirection = Axis::Vertical,
        bool reverse = false,
        std::shared_ptr<ScrollController> controller = nullptr,
        std::shared_ptr<ScrollPhysics> physics = nullptr,
        bool shrinkWrap = false,
        EdgeInsets padding = EdgeInsets{}
    );
    
    std::unique_ptr<State_Base> createState() override;
    
    std::vector<WidgetPtr> children_;
    int itemCount_ = -1;
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder_;
    std::function<WidgetPtr(BuildContext*, int)> separatorBuilder_;
    Axis scrollDirection_;
    bool reverse_;
    std::shared_ptr<ScrollController> controller_;
    std::shared_ptr<ScrollPhysics> physics_;
    bool shrinkWrap_;
    EdgeInsets padding_;
    double itemExtent_;
    bool addAutomaticKeepAlives_;
    bool addRepaintBoundaries_;
};

using قائمة_عرض = ListView;

// ═══════════════════════════════════════════════════════════════════════════════
//  GridView — شبكة قابلة للتمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تفويض الشبكة
 * @brief (EN) Grid delegate
 */
class SliverGridDelegate {
public:
    virtual ~SliverGridDelegate() = default;
    virtual int getCrossAxisCount(double crossAxisExtent) const = 0;
    virtual double getMainAxisExtent(double crossAxisExtent) const = 0;
};

/**
 * @brief (AR) تفويض شبكة ثابت العدد
 * @brief (EN) Fixed cross axis count grid delegate
 */
class SliverGridDelegateWithFixedCrossAxisCount : public SliverGridDelegate {
public:
    SliverGridDelegateWithFixedCrossAxisCount(
        int crossAxisCount,
        double mainAxisSpacing = 0.0,
        double crossAxisSpacing = 0.0,
        double childAspectRatio = 1.0
    ) : crossAxisCount_(crossAxisCount), mainAxisSpacing_(mainAxisSpacing),
        crossAxisSpacing_(crossAxisSpacing), childAspectRatio_(childAspectRatio) {}
    
    int getCrossAxisCount(double) const override { return crossAxisCount_; }
    double getMainAxisExtent(double crossAxisExtent) const override {
        return crossAxisExtent / childAspectRatio_;
    }
    
    int crossAxisCount_;
    double mainAxisSpacing_;
    double crossAxisSpacing_;
    double childAspectRatio_;
};

using تفويض_شبكة_ثابت = SliverGridDelegateWithFixedCrossAxisCount;

/**
 * @brief (AR) تفويض شبكة ثابت المدى
 * @brief (EN) Fixed max cross axis extent grid delegate
 */
class SliverGridDelegateWithMaxCrossAxisExtent : public SliverGridDelegate {
public:
    SliverGridDelegateWithMaxCrossAxisExtent(
        double maxCrossAxisExtent,
        double mainAxisSpacing = 0.0,
        double crossAxisSpacing = 0.0,
        double childAspectRatio = 1.0
    ) : maxCrossAxisExtent_(maxCrossAxisExtent), mainAxisSpacing_(mainAxisSpacing),
        crossAxisSpacing_(crossAxisSpacing), childAspectRatio_(childAspectRatio) {}
    
    int getCrossAxisCount(double crossAxisExtent) const override {
        return std::max(1, static_cast<int>(crossAxisExtent / maxCrossAxisExtent_));
    }
    
    double getMainAxisExtent(double crossAxisExtent) const override {
        int count = getCrossAxisCount(crossAxisExtent);
        double itemWidth = crossAxisExtent / count;
        return itemWidth / childAspectRatio_;
    }
    
    double maxCrossAxisExtent_;
    double mainAxisSpacing_;
    double crossAxisSpacing_;
    double childAspectRatio_;
};

using تفويض_شبكة_مدى = SliverGridDelegateWithMaxCrossAxisExtent;

/**
 * @brief (AR) شبكة قابلة للتمرير
 * @brief (EN) Grid view
 */
class GridView : public StatefulWidget {
public:
    GridView(std::vector<WidgetPtr> children,
             std::shared_ptr<SliverGridDelegate> gridDelegate,
             Axis scrollDirection = Axis::Vertical,
             bool reverse = false,
             std::shared_ptr<ScrollController> controller = nullptr,
             std::shared_ptr<ScrollPhysics> physics = nullptr,
             bool shrinkWrap = false,
             EdgeInsets padding = EdgeInsets{})
        : children_(std::move(children)), gridDelegate_(gridDelegate),
          scrollDirection_(scrollDirection), reverse_(reverse),
          controller_(controller), physics_(physics),
          shrinkWrap_(shrinkWrap), padding_(padding) {}
    
    // Count constructor
    static std::shared_ptr<GridView> count(
        int crossAxisCount,
        std::vector<WidgetPtr> children,
        double mainAxisSpacing = 0.0,
        double crossAxisSpacing = 0.0,
        double childAspectRatio = 1.0,
        Axis scrollDirection = Axis::Vertical,
        EdgeInsets padding = EdgeInsets{}
    );
    
    // Extent constructor
    static std::shared_ptr<GridView> extent(
        double maxCrossAxisExtent,
        std::vector<WidgetPtr> children,
        double mainAxisSpacing = 0.0,
        double crossAxisSpacing = 0.0,
        double childAspectRatio = 1.0,
        Axis scrollDirection = Axis::Vertical,
        EdgeInsets padding = EdgeInsets{}
    );
    
    // Builder constructor
    static std::shared_ptr<GridView> builder(
        std::shared_ptr<SliverGridDelegate> gridDelegate,
        int itemCount,
        std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
        Axis scrollDirection = Axis::Vertical,
        bool reverse = false,
        std::shared_ptr<ScrollController> controller = nullptr,
        EdgeInsets padding = EdgeInsets{}
    );
    
    std::unique_ptr<State_Base> createState() override;
    
    std::vector<WidgetPtr> children_;
    int itemCount_ = -1;
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder_;
    std::shared_ptr<SliverGridDelegate> gridDelegate_;
    Axis scrollDirection_;
    bool reverse_;
    std::shared_ptr<ScrollController> controller_;
    std::shared_ptr<ScrollPhysics> physics_;
    bool shrinkWrap_;
    EdgeInsets padding_;
};

using شبكة_عرض = GridView;

// ═══════════════════════════════════════════════════════════════════════════════
//  CustomScrollView — عرض تمرير مخصص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) Sliver قاعدة
 * @brief (EN) Base sliver widget
 */
class SliverWidget : public Widget {
public:
    virtual WidgetPtr build(BuildContext* ctx) = 0;
};

/**
 * @brief (AR) SliverList
 * @brief (EN) Sliver list
 */
class SliverList : public SliverWidget {
public:
    SliverList(std::function<WidgetPtr(BuildContext*, int)> delegate,
               int childCount = -1)
        : delegate_(delegate), childCount_(childCount) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
    std::function<WidgetPtr(BuildContext*, int)> delegate_;
    int childCount_;
};

using قائمة_شريحة = SliverList;

/**
 * @brief (AR) SliverGrid
 * @brief (EN) Sliver grid
 */
class SliverGrid : public SliverWidget {
public:
    SliverGrid(std::function<WidgetPtr(BuildContext*, int)> delegate,
               std::shared_ptr<SliverGridDelegate> gridDelegate,
               int childCount = -1)
        : delegate_(delegate), gridDelegate_(gridDelegate), childCount_(childCount) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
    std::function<WidgetPtr(BuildContext*, int)> delegate_;
    std::shared_ptr<SliverGridDelegate> gridDelegate_;
    int childCount_;
};

using شبكة_شريحة = SliverGrid;

/**
 * @brief (AR) SliverAppBar
 * @brief (EN) Sliver app bar
 */
class SliverAppBar : public SliverWidget {
public:
    SliverAppBar(WidgetPtr title = nullptr,
                 bool floating = false,
                 bool pinned = false,
                 bool snap = false,
                 float expandedHeight = 0.0f,
                 WidgetPtr flexibleSpace = nullptr,
                 Color backgroundColor = Color(100, 149, 237),
                 WidgetPtr leading = nullptr,
                 std::vector<WidgetPtr> actions = {})
        : title_(title), floating_(floating), pinned_(pinned), snap_(snap),
          expandedHeight_(expandedHeight), flexibleSpace_(flexibleSpace),
          backgroundColor_(backgroundColor), leading_(leading),
          actions_(std::move(actions)) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
    WidgetPtr title_;
    bool floating_;
    bool pinned_;
    bool snap_;
    float expandedHeight_;
    WidgetPtr flexibleSpace_;
    Color backgroundColor_;
    WidgetPtr leading_;
    std::vector<WidgetPtr> actions_;
};

using شريط_تطبيق_شريحة = SliverAppBar;

/**
 * @brief (AR) SliverToBoxAdapter
 * @brief (EN) Sliver to box adapter
 */
class SliverToBoxAdapter : public SliverWidget {
public:
    explicit SliverToBoxAdapter(WidgetPtr child) : child_(child) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
    WidgetPtr child_;
};

using محول_شريحة = SliverToBoxAdapter;

/**
 * @brief (AR) SliverPadding
 * @brief (EN) Sliver padding
 */
class SliverPadding : public SliverWidget {
public:
    SliverPadding(EdgeInsets padding, std::shared_ptr<SliverWidget> sliver)
        : padding_(padding), sliver_(sliver) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
    EdgeInsets padding_;
    std::shared_ptr<SliverWidget> sliver_;
};

using حشو_شريحة = SliverPadding;

/**
 * @brief (AR) SliverFillRemaining
 * @brief (EN) Sliver fill remaining
 */
class SliverFillRemaining : public SliverWidget {
public:
    SliverFillRemaining(WidgetPtr child,
                        bool hasScrollBody = true,
                        bool fillOverscroll = false)
        : child_(child), hasScrollBody_(hasScrollBody), fillOverscroll_(fillOverscroll) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
    WidgetPtr child_;
    bool hasScrollBody_;
    bool fillOverscroll_;
};

using ملء_شريحة = SliverFillRemaining;

/**
 * @brief (AR) عرض تمرير مخصص
 * @brief (EN) Custom scroll view
 */
class CustomScrollView : public StatefulWidget {
public:
    CustomScrollView(std::vector<std::shared_ptr<SliverWidget>> slivers,
                     Axis scrollDirection = Axis::Vertical,
                     bool reverse = false,
                     std::shared_ptr<ScrollController> controller = nullptr,
                     std::shared_ptr<ScrollPhysics> physics = nullptr,
                     bool shrinkWrap = false)
        : slivers_(std::move(slivers)), scrollDirection_(scrollDirection),
          reverse_(reverse), controller_(controller), physics_(physics),
          shrinkWrap_(shrinkWrap) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    std::vector<std::shared_ptr<SliverWidget>> slivers_;
    Axis scrollDirection_;
    bool reverse_;
    std::shared_ptr<ScrollController> controller_;
    std::shared_ptr<ScrollPhysics> physics_;
    bool shrinkWrap_;
};

using عرض_تمرير_مخصص = CustomScrollView;

// ═══════════════════════════════════════════════════════════════════════════════
//  PageView — عرض صفحات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) متحكم الصفحات
 * @brief (EN) Page controller
 */
class PageController : public ScrollController {
public:
    PageController(int initialPage = 0,
                   bool keepPage = true,
                   double viewportFraction = 1.0)
        : ScrollController(initialPage * 1.0, keepPage),
          initialPage_(initialPage), viewportFraction_(viewportFraction),
          currentPage_(initialPage) {}
    
    int page() const { return currentPage_; }
    
    void jumpToPage(int page) {
        currentPage_ = page;
        jumpTo(page * 1.0);
    }
    
    void animateToPage(int page, Duration duration, Curve curve = Curves::ease) {
        currentPage_ = page;
        animateTo(page * 1.0, duration, curve);
    }
    
    void nextPage(Duration duration, Curve curve = Curves::ease) {
        animateToPage(currentPage_ + 1, duration, curve);
    }
    
    void previousPage(Duration duration, Curve curve = Curves::ease) {
        animateToPage(currentPage_ - 1, duration, curve);
    }
    
    int initialPage_;
    double viewportFraction_;
    int currentPage_;
};

using متحكم_صفحات = PageController;

/**
 * @brief (AR) عرض صفحات
 * @brief (EN) Page view
 */
class PageView : public StatefulWidget {
public:
    PageView(std::vector<WidgetPtr> children,
             Axis scrollDirection = Axis::Horizontal,
             bool reverse = false,
             std::shared_ptr<PageController> controller = nullptr,
             std::shared_ptr<ScrollPhysics> physics = nullptr,
             std::function<void(int)> onPageChanged = nullptr,
             bool pageSnapping = true,
             bool allowImplicitScrolling = false)
        : children_(std::move(children)), scrollDirection_(scrollDirection),
          reverse_(reverse), controller_(controller), physics_(physics),
          onPageChanged_(onPageChanged), pageSnapping_(pageSnapping),
          allowImplicitScrolling_(allowImplicitScrolling) {}
    
    // Builder constructor
    static std::shared_ptr<PageView> builder(
        int itemCount,
        std::function<WidgetPtr(BuildContext*, int)> itemBuilder,
        Axis scrollDirection = Axis::Horizontal,
        std::shared_ptr<PageController> controller = nullptr,
        std::function<void(int)> onPageChanged = nullptr
    );
    
    std::unique_ptr<State_Base> createState() override;
    
    std::vector<WidgetPtr> children_;
    int itemCount_ = -1;
    std::function<WidgetPtr(BuildContext*, int)> itemBuilder_;
    Axis scrollDirection_;
    bool reverse_;
    std::shared_ptr<PageController> controller_;
    std::shared_ptr<ScrollPhysics> physics_;
    std::function<void(int)> onPageChanged_;
    bool pageSnapping_;
    bool allowImplicitScrolling_;
};

using عرض_صفحات = PageView;

// ═══════════════════════════════════════════════════════════════════════════════
//  NestedScrollView — عرض تمرير متداخل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عرض تمرير متداخل
 * @brief (EN) Nested scroll view
 */
class NestedScrollView : public StatefulWidget {
public:
    NestedScrollView(
        std::function<std::vector<std::shared_ptr<SliverWidget>>(BuildContext*)> headerSliverBuilder,
        WidgetPtr body,
        std::shared_ptr<ScrollController> controller = nullptr,
        Axis scrollDirection = Axis::Vertical,
        bool reverse = false,
        std::shared_ptr<ScrollPhysics> physics = nullptr,
        bool floatHeaderSlivers = false)
        : headerSliverBuilder_(headerSliverBuilder), body_(body),
          controller_(controller), scrollDirection_(scrollDirection),
          reverse_(reverse), physics_(physics), floatHeaderSlivers_(floatHeaderSlivers) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    std::function<std::vector<std::shared_ptr<SliverWidget>>(BuildContext*)> headerSliverBuilder_;
    WidgetPtr body_;
    std::shared_ptr<ScrollController> controller_;
    Axis scrollDirection_;
    bool reverse_;
    std::shared_ptr<ScrollPhysics> physics_;
    bool floatHeaderSlivers_;
};

using عرض_تمرير_متداخل = NestedScrollView;

// ═══════════════════════════════════════════════════════════════════════════════
//  Scrollbar — شريط التمرير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) شريط التمرير
 * @brief (EN) Scrollbar widget
 */
class Scrollbar : public StatelessWidget {
public:
    Scrollbar(WidgetPtr child,
              std::shared_ptr<ScrollController> controller = nullptr,
              bool thumbVisibility = true,
              bool trackVisibility = false,
              double thickness = 6.0,
              Radius radius = Radius{3.0f, 3.0f},
              bool interactive = true,
              Color thumbColor = Color(128, 128, 128),
              Color trackColor = Color(200, 200, 200, 100))
        : child_(child), controller_(controller), thumbVisibility_(thumbVisibility),
          trackVisibility_(trackVisibility), thickness_(thickness), radius_(radius),
          interactive_(interactive), thumbColor_(thumbColor), trackColor_(trackColor) {}
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    WidgetPtr child_;
    std::shared_ptr<ScrollController> controller_;
    bool thumbVisibility_;
    bool trackVisibility_;
    double thickness_;
    Radius radius_;
    bool interactive_;
    Color thumbColor_;
    Color trackColor_;
};

using شريط_تمرير = Scrollbar;

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationListener — مستمع الإشعارات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مستمع إشعارات التمرير
 * @brief (EN) Scroll notification listener
 */
class NotificationListener : public StatelessWidget {
public:
    NotificationListener(WidgetPtr child,
                         std::function<bool(ScrollNotification)> onNotification)
        : child_(child), onNotification_(onNotification) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
private:
    WidgetPtr child_;
    std::function<bool(ScrollNotification)> onNotification_;
};

using مستمع_اشعارات = NotificationListener;

// ═══════════════════════════════════════════════════════════════════════════════
//  ReorderableListView — قائمة قابلة لإعادة الترتيب
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) قائمة قابلة لإعادة الترتيب
 * @brief (EN) Reorderable list view
 */
class ReorderableListView : public StatefulWidget {
public:
    ReorderableListView(std::vector<WidgetPtr> children,
                        std::function<void(int, int)> onReorder,
                        Axis scrollDirection = Axis::Vertical,
                        EdgeInsets padding = EdgeInsets{},
                        bool reverse = false,
                        std::shared_ptr<ScrollController> scrollController = nullptr,
                        bool buildDefaultDragHandles = true)
        : children_(std::move(children)), onReorder_(onReorder),
          scrollDirection_(scrollDirection), padding_(padding),
          reverse_(reverse), scrollController_(scrollController),
          buildDefaultDragHandles_(buildDefaultDragHandles) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    std::vector<WidgetPtr> children_;
    std::function<void(int, int)> onReorder_;
    Axis scrollDirection_;
    EdgeInsets padding_;
    bool reverse_;
    std::shared_ptr<ScrollController> scrollController_;
    bool buildDefaultDragHandles_;
};

using قائمة_قابلة_للترتيب = ReorderableListView;

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء قائمة مع فاصل
 * @brief (EN) Create separated list
 */
inline std::shared_ptr<ListView> انشاء_قائمة_مفصولة(
    int عدد_العناصر,
    std::function<WidgetPtr(BuildContext*, int)> باني_عنصر,
    std::function<WidgetPtr(BuildContext*, int)> باني_فاصل) {
    
    return ListView::separated(عدد_العناصر, باني_عنصر, باني_فاصل);
}

/**
 * @brief (AR) إنشاء شبكة ثابتة
 * @brief (EN) Create fixed grid
 */
inline std::shared_ptr<GridView> انشاء_شبكة(
    int عدد_الأعمدة,
    std::vector<WidgetPtr> ابناء,
    double تباعد = 8.0) {
    
    return GridView::count(عدد_الأعمدة, std::move(ابناء), تباعد, تباعد);
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_SCROLL_H
