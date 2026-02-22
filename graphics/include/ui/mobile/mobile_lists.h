/**
 * @file mobile_lists.h
 * @brief مركبات القوائم والتمرير للتطبيقات المحمولة
 * 
 * يحتوي على 10 مركبات قوائم:
 * - القائمة الرأسية ListView
 * - الشبكة GridView
 * - القائمة المتحركة AnimatedList
 * - القائمة المعاد ترتيبها ReorderableList
 * - الشجرة TreeView
 * - القائمة الافتراضية VirtualizedList
 * - القائمة المجمّعة GroupedList
 * - التمرير اللانهائي InfiniteScroll
 * - القائمة القابلة للسحب SwipeableList
 * - القائمة المتداخلة NestedScrollView
 * 
 * @author فريق لغة ص
 * @date 2026
 */
#pragma once

#include "mobile_core.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace sad {
namespace graphics {
namespace ui {
namespace mobile {

// ═══════════════════════════════════════════════════════════════
// 1. القائمة الرأسية - ListView
// ═══════════════════════════════════════════════════════════════

/**
 * @class ListView
 * @brief قائمة عناصر قابلة للتمرير - المركب الأساسي لعرض القوائم
 * 
 * يوفر:
 * - تمرير سلس مع قصور ذاتي
 * - بناء كسول (يبني فقط العناصر المرئية)
 * - سحب للتحديث (Pull to Refresh)
 * - تحميل المزيد عند الوصول للنهاية
 * - فواصل بين العناصر
 * - رأس وذيل اختياريان
 */
class ListView : public Widget {
public:
    // --- بناء العناصر ---
    std::function<std::shared_ptr<Widget>(int index)> itemBuilder; // بناء عنصر
    int itemCount = 0;             // عدد العناصر
    float itemExtent = 0;          // ارتفاع ثابت (0 = تلقائي)

    // --- التمرير ---
    float scrollOffset = 0.0f;     // إزاحة التمرير الحالية
    float scrollVelocity = 0.0f;   // سرعة التمرير (للقصور الذاتي)
    float friction = 0.95f;        // معامل الاحتكاك
    bool bouncing = true;          // تأثير الارتداد عند الحدود
    bool scrollEnabled = true;
    float overscrollDistance = 0;   // مسافة التمرير الزائد

    /**
     * @brief اتجاه التمرير
     */
    enum class ScrollDirection { Vertical, Horizontal } direction = ScrollDirection::Vertical;

    // --- سحب للتحديث ---
    bool pullToRefreshEnabled = false;
    float refreshThreshold = 80.0f;   // المسافة المطلوبة للتحديث
    bool isRefreshing = false;
    std::function<void()> onRefresh;

    // --- تحميل المزيد ---
    bool loadMoreEnabled = false;
    float loadMoreThreshold = 200.0f; // المسافة من النهاية
    bool isLoadingMore = false;
    std::function<void()> onLoadMore;

    // --- الفواصل ---
    bool showSeparators = false;
    Color separatorColor = Color(0.9f, 0.9f, 0.9f, 1);
    float separatorHeight = 1.0f;
    float separatorIndent = 0.0f;

    // --- الرأس والذيل ---
    std::shared_ptr<Widget> header;
    std::shared_ptr<Widget> footer;

    // --- التخزين المؤقت ---
    int cacheExtent = 3;           // عدد العناصر المخزنة خارج الشاشة

    // --- الأحداث ---
    std::function<void(float)> onScroll;
    std::function<void()> onScrollEnd;

    void scrollTo(float offset) {
        scrollOffset = offset;
        if (onScroll) onScroll(scrollOffset);
    }

    void scrollToIndex(int index) {
        if (itemExtent > 0) {
            scrollTo(index * itemExtent);
        }
    }

    void update(float dt) {
        // تطبيق القصور الذاتي
        if (std::abs(scrollVelocity) > 0.1f) {
            scrollOffset += scrollVelocity * dt;
            scrollVelocity *= friction;
        } else {
            scrollVelocity = 0;
            if (onScrollEnd) onScrollEnd();
        }

        // حدود التمرير
        float maxScroll = itemCount * (itemExtent > 0 ? itemExtent : 60.0f) - size.y;
        if (scrollOffset < 0) {
            if (bouncing) {
                scrollOffset *= 0.5f; // تأثير مطاطي
            } else {
                scrollOffset = 0;
            }
        }
        if (scrollOffset > maxScroll) {
            if (bouncing) {
                float excess = scrollOffset - maxScroll;
                scrollOffset = maxScroll + excess * 0.5f;
            } else {
                scrollOffset = maxScroll;
            }
        }

        // تحميل المزيد
        if (loadMoreEnabled && !isLoadingMore &&
            scrollOffset > maxScroll - loadMoreThreshold) {
            isLoadingMore = true;
            if (onLoadMore) onLoadMore();
        }
    }

    void render(Renderer2D& renderer) override {
        // تحديد العناصر المرئية
        float ih = itemExtent > 0 ? itemExtent : 60.0f;
        int firstVisible = std::max(0, (int)(scrollOffset / ih) - cacheExtent);
        int lastVisible = std::min(itemCount - 1,
            (int)((scrollOffset + size.y) / ih) + cacheExtent);

        // رسم العناصر المرئية
        for (int i = firstVisible; i <= lastVisible; i++) {
            float y = position.y + i * ih - scrollOffset;
            if (itemBuilder) {
                auto widget = itemBuilder(i);
                if (widget) {
                    widget->position = Vec2(position.x, y);
                    widget->size = Vec2(size.x, ih);
                    widget->render(renderer);
                }
            }

            // رسم الفاصل
            if (showSeparators && i < lastVisible) {
                renderer.fillRect(position.x + separatorIndent,
                                y + ih - separatorHeight,
                                size.x - separatorIndent * 2,
                                separatorHeight, separatorColor);
            }
        }

        // مؤشر التحديث
        if (isRefreshing) {
            renderer.drawText("جار التحديث...",
                            position.x + size.x / 2, position.y + 20,
                            Color(0.5f, 0.5f, 0.5f, 1));
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (!scrollEnabled) return;
        // معالجة التمرير بالعجلة أو اللمس
        if (event.type == SDL_MOUSEWHEEL) {
            scrollVelocity -= event.wheel.y * 60.0f;
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 2. الشبكة - GridView
// ═══════════════════════════════════════════════════════════════

/**
 * @class GridView
 * @brief شبكة عناصر قابلة للتمرير - لعرض محتوى بشكل شبكي
 * 
 * يوفر:
 * - عدد أعمدة ثابت أو تلقائي حسب العرض
 * - أحجام متساوية أو مخصصة
 * - تمرير سلس مع قصور ذاتي
 * - بناء كسول للعناصر
 */
class GridView : public Widget {
public:
    // --- بناء العناصر ---
    std::function<std::shared_ptr<Widget>(int index)> itemBuilder;
    int itemCount = 0;

    // --- الشبكة ---
    int crossAxisCount = 2;         // عدد الأعمدة (0 = تلقائي)
    float mainAxisSpacing = 8.0f;   // مسافة رأسية
    float crossAxisSpacing = 8.0f;  // مسافة أفقية
    float childAspectRatio = 1.0f;  // نسبة العرض للارتفاع
    float maxCrossAxisExtent = 0;   // أقصى عرض عمود (للوضع التلقائي)
    EdgeInsets padding = EdgeInsets(8);

    // --- التمرير ---
    float scrollOffset = 0.0f;
    float scrollVelocity = 0.0f;
    float friction = 0.95f;
    bool scrollEnabled = true;

    // --- الأحداث ---
    std::function<void(int)> onItemTap;
    std::function<void(float)> onScroll;

    int getColumnCount() const {
        if (crossAxisCount > 0) return crossAxisCount;
        if (maxCrossAxisExtent > 0) {
            float available = size.x - padding.left - padding.right;
            return std::max(1, (int)(available / maxCrossAxisExtent));
        }
        return 2;
    }

    float getItemWidth() const {
        int cols = getColumnCount();
        float available = size.x - padding.left - padding.right -
                         (cols - 1) * crossAxisSpacing;
        return available / cols;
    }

    float getItemHeight() const {
        return getItemWidth() / childAspectRatio;
    }

    void update(float dt) {
        if (std::abs(scrollVelocity) > 0.1f) {
            scrollOffset += scrollVelocity * dt;
            scrollVelocity *= friction;
        }

        // حدود
        int cols = getColumnCount();
        int rows = (itemCount + cols - 1) / cols;
        float maxScroll = rows * (getItemHeight() + mainAxisSpacing) - size.y;
        scrollOffset = std::max(0.0f, std::min(scrollOffset, maxScroll));
    }

    void render(Renderer2D& renderer) override {
        int cols = getColumnCount();
        float iw = getItemWidth();
        float ih = getItemHeight();

        int firstRow = std::max(0, (int)(scrollOffset / (ih + mainAxisSpacing)));
        int lastRow = (int)((scrollOffset + size.y) / (ih + mainAxisSpacing)) + 1;

        for (int row = firstRow; row <= lastRow; row++) {
            for (int col = 0; col < cols; col++) {
                int index = row * cols + col;
                if (index >= itemCount) break;

                float x = position.x + padding.left +
                         col * (iw + crossAxisSpacing);
                float y = position.y + padding.top +
                         row * (ih + mainAxisSpacing) - scrollOffset;

                if (itemBuilder) {
                    auto widget = itemBuilder(index);
                    if (widget) {
                        widget->position = Vec2(x, y);
                        widget->size = Vec2(iw, ih);
                        widget->render(renderer);
                    }
                }
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL && scrollEnabled) {
            scrollVelocity -= event.wheel.y * 60.0f;
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 3. القائمة المتحركة - AnimatedList
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع تأثير الإضافة/الحذف
 */
enum class ListAnimation {
    FadeIn,     // تلاشي للداخل
    SlideIn,    // انزلاق
    ScaleIn,    // تكبير
    SizeIn,     // توسع
    None        // بدون تأثير
};

/**
 * @class AnimatedList
 * @brief قائمة متحركة - تعرض تأثيرات عند إضافة أو حذف عناصر
 * 
 * يوفر:
 * - تأثيرات إضافة سلسة
 * - تأثيرات حذف سلسة
 * - إعادة ترتيب متحركة
 * - تأثيرات مخصصة
 */
class AnimatedList : public Widget {
public:
    struct AnimatedItem {
        std::shared_ptr<Widget> widget;
        float animProgress = 1.0f;  // 0 = مخفي، 1 = مرئي
        bool isRemoving = false;
        ListAnimation animation = ListAnimation::FadeIn;
    };

    std::vector<AnimatedItem> items;
    ListAnimation insertAnimation = ListAnimation::FadeIn;
    ListAnimation removeAnimation = ListAnimation::FadeIn;
    float animDuration = 0.3f;     // مدة التحريك (ثانية)
    float scrollOffset = 0.0f;
    float itemHeight = 56.0f;

    // --- الأحداث ---
    std::function<void(int)> onItemRemoved;
    std::function<void(int)> onItemInserted;

    void insertItem(int index, std::shared_ptr<Widget> widget) {
        AnimatedItem item;
        item.widget = widget;
        item.animProgress = 0.0f;
        item.animation = insertAnimation;

        if (index >= 0 && index <= (int)items.size()) {
            items.insert(items.begin() + index, item);
            if (onItemInserted) onItemInserted(index);
        }
    }

    void removeItem(int index) {
        if (index >= 0 && index < (int)items.size()) {
            items[index].isRemoving = true;
            items[index].animProgress = 1.0f;
            items[index].animation = removeAnimation;
            if (onItemRemoved) onItemRemoved(index);
        }
    }

    void update(float dt) {
        float speed = 1.0f / animDuration;

        for (auto it = items.begin(); it != items.end();) {
            if (it->isRemoving) {
                it->animProgress -= dt * speed;
                if (it->animProgress <= 0) {
                    it = items.erase(it);
                    continue;
                }
            } else if (it->animProgress < 1.0f) {
                it->animProgress += dt * speed;
                if (it->animProgress > 1.0f) it->animProgress = 1.0f;
            }
            ++it;
        }
    }

    void render(Renderer2D& renderer) override {
        float y = position.y - scrollOffset;
        for (auto& item : items) {
            if (item.animProgress > 0 && item.widget) {
                // تطبيق التأثير
                float alpha = item.animProgress;
                float scale = 1.0f;
                float offsetX = 0;

                switch (item.animation) {
                    case ListAnimation::SlideIn:
                        offsetX = (1.0f - alpha) * size.x;
                        break;
                    case ListAnimation::ScaleIn:
                        scale = alpha;
                        break;
                    default: break;
                }

                item.widget->position = Vec2(position.x + offsetX, y);
                item.widget->size = Vec2(size.x * scale, itemHeight * alpha);
                item.widget->render(renderer);

                y += itemHeight * alpha;
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 30.0f;
            scrollOffset = std::max(0.0f, scrollOffset);
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 4. القائمة المعاد ترتيبها - ReorderableList
// ═══════════════════════════════════════════════════════════════

/**
 * @class ReorderableList
 * @brief قائمة قابلة لإعادة الترتيب بالسحب والإفلات
 * 
 * يوفر:
 * - سحب وإفلات للعناصر
 * - مؤشر بصري لموضع الإفلات
 * - تحريك سلس عند إعادة الترتيب
 * - تأثير رفع العنصر المسحوب
 */
class ReorderableList : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> items;
    int dragIndex = -1;            // العنصر المسحوب (-1 = لا شيء)
    int hoverIndex = -1;           // موضع الإفلات المحتمل
    float itemHeight = 56.0f;
    float dragElevation = 8.0f;    // ظل العنصر المسحوب
    Color dragColor = Color(1, 1, 1, 0.9f);
    Color placeholderColor = Color(0.9f, 0.9f, 0.9f, 0.5f);
    bool hapticFeedback = true;    // اهتزاز عند السحب
    float scrollOffset = 0.0f;

    // --- الأحداث ---
    std::function<void(int, int)> onReorder; // (من، إلى)
    std::function<bool(int)> canDrag;        // هل يمكن سحب العنصر

    void reorder(int from, int to) {
        if (from < 0 || from >= (int)items.size() ||
            to < 0 || to >= (int)items.size()) return;

        auto item = items[from];
        items.erase(items.begin() + from);
        items.insert(items.begin() + to, item);

        if (onReorder) onReorder(from, to);
    }

    void render(Renderer2D& renderer) override {
        float y = position.y - scrollOffset;

        for (size_t i = 0; i < items.size(); i++) {
            if ((int)i == dragIndex) {
                // رسم العنصر النائب
                renderer.fillRect(position.x, y, size.x, itemHeight, placeholderColor);
            } else {
                if (items[i]) {
                    items[i]->position = Vec2(position.x, y);
                    items[i]->size = Vec2(size.x, itemHeight);
                    items[i]->render(renderer);
                }
            }
            y += itemHeight;
        }

        // رسم العنصر المسحوب فوق الكل
        if (dragIndex >= 0 && dragIndex < (int)items.size() && items[dragIndex]) {
            Color shadow(0, 0, 0, 0.2f);
            renderer.fillRect(position.x + 2, items[dragIndex]->position.y + dragElevation,
                            size.x, itemHeight, shadow);
            items[dragIndex]->render(renderer);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        // معالجة أحداث السحب والإفلات
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int index = (int)((event.button.y - position.y + scrollOffset) / itemHeight);
            if (index >= 0 && index < (int)items.size()) {
                if (!canDrag || canDrag(index)) {
                    dragIndex = index;
                }
            }
        }
        if (event.type == SDL_MOUSEBUTTONUP && dragIndex >= 0) {
            if (hoverIndex >= 0 && hoverIndex != dragIndex) {
                reorder(dragIndex, hoverIndex);
            }
            dragIndex = -1;
            hoverIndex = -1;
        }
        if (event.type == SDL_MOUSEMOTION && dragIndex >= 0) {
            hoverIndex = (int)((event.motion.y - position.y + scrollOffset) / itemHeight);
            hoverIndex = std::max(0, std::min(hoverIndex, (int)items.size() - 1));
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 5. الشجرة - TreeView
// ═══════════════════════════════════════════════════════════════

/**
 * @class TreeView
 * @brief عرض شجري هرمي - لعرض بيانات منظمة هرمياً
 * 
 * يوفر:
 * - عقد قابلة للطي والتوسع
 * - أيقونات مخصصة حسب النوع
 * - تحديد متعدد
 * - سحب وإفلات
 * - بحث وتصفية
 */
class TreeView : public Widget {
public:
    struct TreeNode {
        std::string id;             // معرف فريد
        std::string title;          // العنوان
        std::string icon;           // أيقونة
        bool expanded = false;      // موسع
        bool selected = false;
        bool isLeaf = false;        // عقدة ورقية (بدون أبناء)
        int depth = 0;             // العمق في الشجرة
        std::vector<TreeNode> children;
        std::function<void()> onTap;
    };

    TreeNode root;                  // جذر الشجرة
    float itemHeight = 36.0f;
    float indentWidth = 24.0f;     // عرض المسافة البادئة لكل مستوى
    Color selectedColor = Color(0.88f, 0.88f, 1.0f, 1);
    Color hoverColor = Color(0.95f, 0.95f, 0.95f, 1);
    Color expandIconColor = Color(0.5f, 0.5f, 0.5f, 1);
    Color textColor = Color(0.2f, 0.2f, 0.2f, 1);
    bool multiSelect = false;
    bool showLines = false;        // خطوط رابطة
    bool showIcons = true;
    float scrollOffset = 0.0f;
    std::string searchQuery;       // بحث في الشجرة

    // --- الأحداث ---
    std::function<void(const std::string&)> onNodeSelected;
    std::function<void(const std::string&, bool)> onNodeExpanded;

    int countVisibleNodes(const TreeNode& node) const {
        int count = 1;
        if (node.expanded) {
            for (const auto& child : node.children) {
                count += countVisibleNodes(child);
            }
        }
        return count;
    }

    void renderNode(Renderer2D& renderer, const TreeNode& node, float& y) {
        if (y > position.y + size.y) return; // خارج الشاشة
        if (y + itemHeight < position.y - scrollOffset) {
            y += itemHeight;
            if (node.expanded) {
                for (const auto& child : node.children) renderNode(renderer, child, y);
            }
            return;
        }

        float x = position.x + node.depth * indentWidth;
        float currentY = y - scrollOffset;

        // رسم الخلفية (عند التحديد)
        if (node.selected) {
            renderer.fillRect(position.x, currentY, size.x, itemHeight, selectedColor);
        }

        // رسم أيقونة التوسع
        if (!node.isLeaf && !node.children.empty()) {
            std::string arrow = node.expanded ? "▼" : "▶";
            renderer.drawText(arrow, x, currentY + itemHeight / 2, expandIconColor);
        }

        // رسم العنوان
        renderer.drawText(node.title, x + 24, currentY + itemHeight / 2, textColor);

        y += itemHeight;

        // رسم الأبناء
        if (node.expanded) {
            for (const auto& child : node.children) {
                renderNode(renderer, child, y);
            }
        }
    }

    void render(Renderer2D& renderer) override {
        float y = position.y;
        for (const auto& child : root.children) {
            renderNode(renderer, child, y);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 30.0f;
            scrollOffset = std::max(0.0f, scrollOffset);
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 6. القائمة الافتراضية - VirtualizedList
// ═══════════════════════════════════════════════════════════════

/**
 * @class VirtualizedList
 * @brief قائمة افتراضية محسنة للأداء - تعرض آلاف العناصر بسلاسة
 * 
 * يوفر:
 * - تقنية النافذة المنزلقة (فقط العناصر المرئية في الذاكرة)
 * - دعم أحجام متغيرة للعناصر
 * - تقدير ذكي للحجم الكلي
 * - تمرير سريع مع thumbnail
 */
class VirtualizedList : public Widget {
public:
    // --- بناء العناصر ---
    std::function<std::shared_ptr<Widget>(int)> itemBuilder;
    std::function<float(int)> itemHeightEstimator; // تقدير ارتفاع العنصر
    int totalItems = 0;

    // --- التمرير ---
    float scrollOffset = 0.0f;
    float scrollVelocity = 0.0f;
    float estimatedItemHeight = 60.0f;    // تقدير افتراضي
    float overscanCount = 5;       // عناصر إضافية خارج الشاشة

    // --- النافذة المنزلقة ---
    int firstVisibleIndex = 0;
    int lastVisibleIndex = 0;
    std::unordered_map<int, float> knownHeights; // أطوال محسوبة

    // --- شريط التمرير السريع ---
    bool showScrollbar = true;
    bool showScrollThumb = false;
    float thumbSize = 40.0f;
    Color thumbColor = Color(0.6f, 0.6f, 0.6f, 0.5f);

    // --- الأحداث ---
    std::function<void(int, int)> onVisibleRangeChanged;

    float getEstimatedTotalHeight() const {
        return totalItems * estimatedItemHeight;
    }

    float getItemHeight(int index) const {
        auto it = knownHeights.find(index);
        if (it != knownHeights.end()) return it->second;
        if (itemHeightEstimator) return itemHeightEstimator(index);
        return estimatedItemHeight;
    }

    void updateVisibleRange() {
        float y = 0;
        firstVisibleIndex = 0;
        for (int i = 0; i < totalItems; i++) {
            float h = getItemHeight(i);
            if (y + h >= scrollOffset) {
                firstVisibleIndex = i;
                break;
            }
            y += h;
        }

        y = 0;
        for (int i = firstVisibleIndex; i < totalItems; i++) {
            y += getItemHeight(i);
            if (y >= size.y + overscanCount * estimatedItemHeight) {
                lastVisibleIndex = i;
                break;
            }
            lastVisibleIndex = i;
        }
    }

    void update(float dt) {
        if (std::abs(scrollVelocity) > 0.5f) {
            scrollOffset += scrollVelocity * dt;
            scrollVelocity *= 0.95f;
        }
        scrollOffset = std::max(0.0f,
            std::min(scrollOffset, getEstimatedTotalHeight() - size.y));
        updateVisibleRange();
    }

    void render(Renderer2D& renderer) override {
        float y = 0;
        for (int i = 0; i < firstVisibleIndex; i++) {
            y += getItemHeight(i);
        }

        for (int i = firstVisibleIndex; i <= lastVisibleIndex && i < totalItems; i++) {
            float h = getItemHeight(i);
            float renderY = position.y + y - scrollOffset;

            if (itemBuilder) {
                auto widget = itemBuilder(i);
                if (widget) {
                    widget->position = Vec2(position.x, renderY);
                    widget->size = Vec2(size.x, h);
                    widget->render(renderer);
                }
            }
            y += h;
        }

        // شريط التمرير
        if (showScrollbar && totalItems > 0) {
            float totalH = getEstimatedTotalHeight();
            float thumbH = std::max(thumbSize, size.y * size.y / totalH);
            float thumbY = position.y + (scrollOffset / totalH) * (size.y - thumbH);
            renderer.fillRect(position.x + size.x - 6, thumbY, 4, thumbH, thumbColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollVelocity -= event.wheel.y * 80.0f;
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 7. القائمة المجمّعة - GroupedList
// ═══════════════════════════════════════════════════════════════

/**
 * @class GroupedList
 * @brief قائمة مجمّعة بأقسام - لتنظيم العناصر في مجموعات
 * 
 * يوفر:
 * - أقسام مع عناوين لاصقة
 * - فهرس جانبي سريع (A-Z)
 * - طي/توسع الأقسام
 * - عد العناصر في كل قسم
 */
class GroupedList : public Widget {
public:
    struct Section {
        std::string title;          // عنوان القسم
        std::string indexLabel;     // حرف الفهرس (اختياري)
        bool collapsed = false;
        std::vector<std::shared_ptr<Widget>> items;
        int itemCount = 0;
    };

    std::vector<Section> sections;
    float sectionHeaderHeight = 40.0f;
    float itemHeight = 56.0f;
    Color headerColor = Color(0.96f, 0.96f, 0.96f, 1);
    Color headerTextColor = Color(0.4f, 0.4f, 0.4f, 1);
    bool stickyHeaders = true;     // عناوين لاصقة
    bool collapsible = false;      // أقسام قابلة للطي
    bool showIndex = false;        // فهرس جانبي
    float scrollOffset = 0.0f;
    Color indexColor = Color(0.25f, 0.32f, 0.71f, 1);
    float indexWidth = 20.0f;

    // --- الأحداث ---
    std::function<void(int, int)> onItemTap; // (قسم، عنصر)

    void addSection(const std::string& title, const std::string& indexLabel = "") {
        sections.push_back({title, indexLabel, false, {}, 0});
    }

    void scrollToSection(int sectionIndex) {
        float y = 0;
        for (int i = 0; i < sectionIndex && i < (int)sections.size(); i++) {
            y += sectionHeaderHeight;
            if (!sections[i].collapsed) {
                y += sections[i].items.size() * itemHeight;
            }
        }
        scrollOffset = y;
    }

    void render(Renderer2D& renderer) override {
        float y = position.y - scrollOffset;

        for (size_t s = 0; s < sections.size(); s++) {
            // رسم رأس القسم
            float headerY = y;
            if (stickyHeaders && y < position.y && !sections[s].collapsed) {
                headerY = position.y; // لاصق
            }
            renderer.fillRect(position.x, headerY, size.x, sectionHeaderHeight, headerColor);
            renderer.drawText(sections[s].title, position.x + 16,
                            headerY + sectionHeaderHeight / 2, headerTextColor);

            y += sectionHeaderHeight;

            // رسم العناصر
            if (!sections[s].collapsed) {
                for (size_t i = 0; i < sections[s].items.size(); i++) {
                    if (y + itemHeight > position.y && y < position.y + size.y) {
                        if (sections[s].items[i]) {
                            sections[s].items[i]->position = Vec2(position.x, y);
                            sections[s].items[i]->size = Vec2(size.x, itemHeight);
                            sections[s].items[i]->render(renderer);
                        }
                    }
                    y += itemHeight;
                }
            }
        }

        // رسم الفهرس الجانبي
        if (showIndex) {
            float indexX = position.x + size.x - indexWidth;
            float indexItemH = size.y / sections.size();
            for (size_t i = 0; i < sections.size(); i++) {
                std::string label = sections[i].indexLabel.empty() ?
                    sections[i].title.substr(0, 1) : sections[i].indexLabel;
                renderer.drawText(label, indexX, position.y + i * indexItemH + indexItemH / 2,
                                indexColor);
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 40.0f;
            scrollOffset = std::max(0.0f, scrollOffset);
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 8. التمرير اللانهائي - InfiniteScroll
// ═══════════════════════════════════════════════════════════════

/**
 * @class InfiniteScroll
 * @brief تمرير لانهائي - يحمّل المزيد تلقائياً عند الوصول للنهاية
 * 
 * يوفر:
 * - تحميل تلقائي للصفحات
 * - مؤشر تحميل في النهاية
 * - إعادة المحاولة عند الفشل
 * - عد الصفحات
 */
class InfiniteScroll : public Widget {
public:
    // --- البناء ---
    std::function<std::shared_ptr<Widget>(int)> itemBuilder;
    int loadedCount = 0;           // عدد العناصر المحملة
    bool hasMore = true;           // يوجد المزيد
    int pageSize = 20;             // حجم الصفحة
    int currentPage = 0;

    // --- التحميل ---
    float loadThreshold = 200.0f;  // المسافة من النهاية لبدء التحميل
    bool isLoading = false;
    bool hasError = false;
    std::string errorMessage;
    std::string loadingText = "جار التحميل...";
    std::string noMoreText = "لا يوجد المزيد";

    // --- التمرير ---
    float scrollOffset = 0.0f;
    float itemHeight = 56.0f;

    // --- الأحداث ---
    std::function<void(int page)> onLoadMore;
    std::function<void()> onRetry;

    Color loadingColor = Color(0.5f, 0.5f, 0.5f, 1);
    Color errorColor = Color(0.9f, 0.2f, 0.2f, 1);

    void loadNextPage() {
        if (isLoading || !hasMore) return;
        isLoading = true;
        hasError = false;
        currentPage++;
        if (onLoadMore) onLoadMore(currentPage);
    }

    void onLoadComplete(int newItemCount) {
        isLoading = false;
        loadedCount += newItemCount;
        if (newItemCount < pageSize) hasMore = false;
    }

    void onLoadFailed(const std::string& error) {
        isLoading = false;
        hasError = true;
        errorMessage = error;
        currentPage--;
    }

    void retry() {
        hasError = false;
        loadNextPage();
    }

    void update(float dt) {
        float totalHeight = loadedCount * itemHeight;
        if (!isLoading && hasMore && !hasError &&
            scrollOffset + size.y >= totalHeight - loadThreshold) {
            loadNextPage();
        }
    }

    void render(Renderer2D& renderer) override {
        float y = position.y - scrollOffset;

        for (int i = 0; i < loadedCount; i++) {
            if (y + itemHeight > position.y && y < position.y + size.y) {
                if (itemBuilder) {
                    auto widget = itemBuilder(i);
                    if (widget) {
                        widget->position = Vec2(position.x, y);
                        widget->size = Vec2(size.x, itemHeight);
                        widget->render(renderer);
                    }
                }
            }
            y += itemHeight;
        }

        // مؤشر التحميل
        if (isLoading) {
            renderer.drawText(loadingText, position.x + size.x / 2, y + 20, loadingColor);
        } else if (hasError) {
            renderer.drawText(errorMessage, position.x + size.x / 2, y + 10, errorColor);
            renderer.drawText("اضغط لإعادة المحاولة",
                            position.x + size.x / 2, y + 30, loadingColor);
        } else if (!hasMore) {
            renderer.drawText(noMoreText, position.x + size.x / 2, y + 20, loadingColor);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 40.0f;
            scrollOffset = std::max(0.0f, scrollOffset);
        }
        if (hasError && event.type == SDL_MOUSEBUTTONDOWN) {
            retry();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 9. القائمة القابلة للسحب - SwipeableList
// ═══════════════════════════════════════════════════════════════

/**
 * @class SwipeableList
 * @brief قائمة بعناصر قابلة للسحب الجانبي - لإظهار إجراءات مخفية
 * 
 * يوفر:
 * - سحب يمين/يسار لكشف الإجراءات
 * - إجراءات مخصصة (حذف، أرشفة، إلخ)
 * - تأكيد الحذف
 * - تأثيرات بصرية سلسة
 */
class SwipeableList : public Widget {
public:
    struct SwipeAction {
        std::string icon;
        std::string label;
        Color backgroundColor;
        std::function<void(int)> onTriggered;
        float width = 80.0f;
    };

    struct SwipeItem {
        std::shared_ptr<Widget> content;
        std::vector<SwipeAction> leadingActions;   // إجراءات البداية
        std::vector<SwipeAction> trailingActions;  // إجراءات النهاية
        float swipeOffset = 0.0f;  // إزاحة السحب الحالية
    };

    std::vector<SwipeItem> items;
    float itemHeight = 56.0f;
    float scrollOffset = 0.0f;
    float swipeThreshold = 0.3f;   // نسبة السحب لتفعيل الإجراء
    bool dismissible = false;      // السحب الكامل يحذف العنصر
    int activeSwipeIndex = -1;     // العنصر المسحوب حالياً

    // --- الأحداث ---
    std::function<void(int)> onDismissed;    // عند حذف بالسحب

    void addItem(std::shared_ptr<Widget> content) {
        items.push_back({content, {}, {}, 0});
    }

    void addTrailingAction(int index, const std::string& icon,
                          const std::string& label, Color bg,
                          std::function<void(int)> callback) {
        if (index >= 0 && index < (int)items.size()) {
            items[index].trailingActions.push_back({icon, label, bg, callback, 80});
        }
    }

    void resetSwipe(int index) {
        if (index >= 0 && index < (int)items.size()) {
            items[index].swipeOffset = 0;
        }
    }

    void resetAllSwipes() {
        for (auto& item : items) item.swipeOffset = 0;
        activeSwipeIndex = -1;
    }

    void render(Renderer2D& renderer) override {
        float y = position.y - scrollOffset;

        for (size_t i = 0; i < items.size(); i++) {
            if (y + itemHeight > position.y && y < position.y + size.y) {
                // رسم الإجراءات الخلفية
                if (items[i].swipeOffset < 0) {
                    // إجراءات النهاية (سحب يسار)
                    float actionX = position.x + size.x + items[i].swipeOffset;
                    for (const auto& action : items[i].trailingActions) {
                        renderer.fillRect(actionX, y, action.width, itemHeight,
                                        action.backgroundColor);
                        renderer.drawText(action.label, actionX + action.width / 2,
                                        y + itemHeight / 2, Color(1, 1, 1, 1));
                        actionX += action.width;
                    }
                }

                // رسم المحتوى (مزاح)
                if (items[i].content) {
                    items[i].content->position = Vec2(position.x + items[i].swipeOffset, y);
                    items[i].content->size = Vec2(size.x, itemHeight);
                    items[i].content->render(renderer);
                }
            }
            y += itemHeight;
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollOffset -= event.wheel.y * 40.0f;
            scrollOffset = std::max(0.0f, scrollOffset);
            resetAllSwipes();
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 10. التمرير المتداخل - NestedScrollView
// ═══════════════════════════════════════════════════════════════

/**
 * @class NestedScrollView
 * @brief عرض تمرير متداخل - لدمج عناصر ثابتة ومتحركة
 * 
 * يوفر:
 * - رأس قابل للطي (مثل AppBar مع صورة)
 * - جسم قابل للتمرير
 * - تنسيق التمرير بين العناصر المتداخلة
 * - شريط تبويبات لاصق
 */
class NestedScrollView : public Widget {
public:
    // --- الرأس ---
    std::shared_ptr<Widget> headerWidget;
    float headerHeight = 200.0f;
    float minHeaderHeight = 56.0f; // الارتفاع عند الطي الكامل
    bool headerPinned = true;      // يبقى الحد الأدنى ظاهراً

    // --- عنصر لاصق ---
    std::shared_ptr<Widget> pinnedWidget; // مثل TabBar
    float pinnedHeight = 48.0f;

    // --- الجسم ---
    std::vector<std::shared_ptr<Widget>> bodyWidgets;
    float bodyScrollOffset = 0.0f;

    // --- التمرير ---
    float scrollOffset = 0.0f;
    float scrollVelocity = 0.0f;
    float friction = 0.95f;
    bool bouncing = true;
    Color backgroundColor = Color(1, 1, 1, 1);

    // --- حالة الرأس ---
    float headerExpandRatio() const {
        float range = headerHeight - minHeaderHeight;
        if (range <= 0) return 1.0f;
        float ratio = 1.0f - scrollOffset / range;
        return std::max(0.0f, std::min(1.0f, ratio));
    }

    bool isHeaderCollapsed() const { return headerExpandRatio() <= 0.01f; }
    bool isHeaderExpanded() const { return headerExpandRatio() >= 0.99f; }

    void update(float dt) {
        if (std::abs(scrollVelocity) > 0.5f) {
            scrollOffset += scrollVelocity * dt;
            scrollVelocity *= friction;
        }
        scrollOffset = std::max(0.0f, scrollOffset);
    }

    void render(Renderer2D& renderer) override {
        renderer.fillRect(position.x, position.y, size.x, size.y, backgroundColor);

        // حساب ارتفاع الرأس الحالي
        float currentHeaderH = headerHeight - scrollOffset;
        if (currentHeaderH < minHeaderHeight && headerPinned)
            currentHeaderH = minHeaderHeight;
        if (currentHeaderH < 0) currentHeaderH = 0;

        // رسم الرأس
        if (headerWidget && currentHeaderH > 0) {
            headerWidget->position = position;
            headerWidget->size = Vec2(size.x, currentHeaderH);
            headerWidget->render(renderer);
        }

        float y = position.y + currentHeaderH;

        // رسم العنصر اللاصق
        if (pinnedWidget) {
            pinnedWidget->position = Vec2(position.x, y);
            pinnedWidget->size = Vec2(size.x, pinnedHeight);
            pinnedWidget->render(renderer);
            y += pinnedHeight;
        }

        // رسم الجسم
        float bodyOffset = std::max(0.0f, scrollOffset - (headerHeight - minHeaderHeight));
        for (auto& widget : bodyWidgets) {
            if (widget) {
                widget->position = Vec2(position.x, y - bodyOffset);
                widget->render(renderer);
                y += widget->size.y;
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEWHEEL) {
            scrollVelocity -= event.wheel.y * 60.0f;
        }
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad
