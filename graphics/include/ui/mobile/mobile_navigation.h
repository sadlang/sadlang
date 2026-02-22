/**
 * @file mobile_navigation.h
 * @brief مركبات التنقل للتطبيقات المحمولة - نظام تنقل شامل
 * 
 * يحتوي على مركبات التنقل الأساسية:
 * - شريط التطبيق العلوي AppBar
 * - شريط التنقل السفلي BottomNavigationBar
 * - شريط التبويبات TabBar
 * - الدرج الجانبي Drawer
 * - سكة التنقل NavigationRail
 * - مسار التنقل Breadcrumbs
 * - المعالج خطوة بخطوة Stepper
 * - عرض الصفحات PageView
 * - شريط التنقل الحديث NavigationBar
 * - التنقل المتداخل NestedNavigator
 * 
 * جميع المركبات مصممة للعمل مع الهاتف وسطح المكتب
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
// 1. شريط التطبيق العلوي - AppBar
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نمط شريط التطبيق
 * يحدد مظهر شريط التطبيق العلوي
 */
enum class AppBarStyle {
    Standard,    // قياسي - شريط ثابت
    Large,       // كبير - مع عنوان كبير
    Medium,      // متوسط
    CenterAligned, // محاذاة وسطية
    Transparent, // شفاف
    Collapsing   // قابل للطي عند التمرير
};

/**
 * @class AppBar
 * @brief شريط التطبيق العلوي - مثل Material Design AppBar
 * 
 * يوفر:
 * - عنوان التطبيق أو الشاشة
 * - أزرار إجراءات (بحث، إعدادات، إلخ)
 * - زر الرجوع أو القائمة
 * - دعم الطي عند التمرير
 */
class AppBar : public Widget {
public:
    // --- الخصائص الأساسية ---
    std::string title;              // عنوان الشريط
    std::string subtitle;           // عنوان فرعي اختياري
    Color backgroundColor;          // لون الخلفية
    Color foregroundColor;          // لون النص والأيقونات
    float elevation = 4.0f;         // ظل الارتفاع
    AppBarStyle style = AppBarStyle::Standard;
    float height = 56.0f;           // ارتفاع الشريط
    float expandedHeight = 200.0f;  // الارتفاع عند التوسع (للطي)
    bool centerTitle = false;       // توسيط العنوان
    bool automaticallyImplyLeading = true; // إضافة زر رجوع تلقائياً

    // --- أيقونة البداية (يسار/يمين حسب الاتجاه) ---
    std::string leadingIcon;        // أيقونة البداية (مثل: قائمة أو رجوع)
    std::function<void()> onLeadingPressed; // حدث الضغط على أيقونة البداية

    // --- الإجراءات ---
    struct ActionItem {
        std::string icon;           // أيقونة الإجراء
        std::string tooltip;        // تلميح عند التحويم
        std::function<void()> onPressed;
        bool visible = true;
    };
    std::vector<ActionItem> actions; // أزرار الإجراءات

    // --- شريط البحث المدمج ---
    bool searchEnabled = false;
    std::string searchHint = "بحث...";
    std::function<void(const std::string&)> onSearch;

    // --- الطي ---
    bool pinned = true;             // يبقى ظاهراً عند التمرير
    bool floating = false;          // يظهر عند التمرير للأعلى
    bool snap = false;              // ينتقل بسرعة لوضع كامل

    AppBar() {
        backgroundColor = Color(0.25f, 0.32f, 0.71f, 1.0f); // أزرق Material
        foregroundColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void addAction(const std::string& icon, const std::string& tooltip,
                   std::function<void()> callback) {
        actions.push_back({icon, tooltip, callback, true});
    }

    void setSearchMode(bool enabled) {
        searchEnabled = enabled;
    }

    void render(Renderer2D& renderer) override {
        // رسم خلفية الشريط
        float currentHeight = (style == AppBarStyle::Collapsing) ? expandedHeight : height;
        renderer.fillRect(position.x, position.y, size.x, currentHeight, backgroundColor);

        // رسم الظل
        if (elevation > 0) {
            Color shadowColor(0, 0, 0, 0.2f);
            renderer.fillRect(position.x, position.y + currentHeight,
                            size.x, elevation, shadowColor);
        }

        // رسم العنوان
        float titleX = centerTitle ? position.x + size.x / 2 : position.x + 72;
        float titleY = position.y + currentHeight / 2;
        renderer.drawText(title, titleX, titleY, foregroundColor);
    }

    void handleEvent(const SDL_Event& event) override {
        // معالجة أحداث اللمس والنقر
    }
};

// ═══════════════════════════════════════════════════════════════
// 2. شريط التنقل السفلي - BottomNavigationBar
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع شريط التنقل السفلي
 */
enum class BottomNavType {
    Fixed,    // ثابت - جميع العناصر مرئية
    Shifting  // متحرك - العنصر المحدد يتوسع
};

/**
 * @class BottomNavigationBar
 * @brief شريط التنقل السفلي - للتبديل بين الشاشات الرئيسية
 * 
 * يوفر:
 * - حتى 5 عناصر تنقل
 * - أيقونات ونصوص
 * - تأثيرات حركية عند التبديل
 * - شارات إشعارات على الأيقونات
 */
class BottomNavigationBar : public Widget {
public:
    struct NavItem {
        std::string icon;           // أيقونة العنصر
        std::string activeIcon;     // أيقونة عند التحديد
        std::string label;          // نص العنصر
        int badgeCount = 0;         // عدد الإشعارات
        bool showBadge = false;     // إظهار شارة بدون رقم
        Color badgeColor = Color(1, 0, 0, 1); // لون الشارة
    };

    std::vector<NavItem> items;     // عناصر التنقل
    int selectedIndex = 0;          // العنصر المحدد حالياً
    BottomNavType type = BottomNavType::Fixed;
    Color backgroundColor = Color(1, 1, 1, 1);
    Color selectedColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color unselectedColor = Color(0.5f, 0.5f, 0.5f, 1);
    float height = 56.0f;
    float elevation = 8.0f;
    bool showLabels = true;         // إظهار النصوص
    bool showUnselectedLabels = true;
    float iconSize = 24.0f;
    float fontSize = 12.0f;

    // --- الأحداث ---
    std::function<void(int)> onTap; // عند النقر على عنصر

    void addItem(const std::string& icon, const std::string& label) {
        items.push_back({icon, icon, label, 0, false});
    }

    void setBadge(int index, int count) {
        if (index >= 0 && index < (int)items.size()) {
            items[index].badgeCount = count;
            items[index].showBadge = (count > 0);
        }
    }

    void render(Renderer2D& renderer) override {
        // رسم الخلفية
        float y = position.y;
        renderer.fillRect(position.x, y, size.x, height, backgroundColor);

        // رسم العناصر
        float itemWidth = size.x / items.size();
        for (size_t i = 0; i < items.size(); i++) {
            Color color = (i == (size_t)selectedIndex) ? selectedColor : unselectedColor;
            float ix = position.x + i * itemWidth + itemWidth / 2;
            float iy = y + height / 2;
            // رسم الأيقونة والنص
            if (showLabels) {
                renderer.drawText(items[i].label, ix, iy + 12, color);
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float itemWidth = size.x / items.size();
            int clickedIndex = (int)((event.button.x - position.x) / itemWidth);
            if (clickedIndex >= 0 && clickedIndex < (int)items.size()) {
                selectedIndex = clickedIndex;
                if (onTap) onTap(selectedIndex);
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 3. شريط التبويبات - TabBar
// ═══════════════════════════════════════════════════════════════

/**
 * @class TabBar
 * @brief شريط تبويبات قابل للتمرير - للتبديل بين المحتويات
 * 
 * يوفر:
 * - تبويبات بنص وأيقونة
 * - تمرير أفقي عند كثرة التبويبات
 * - مؤشر متحرك تحت التبويب المحدد
 * - دعم السحب للتبديل
 */
class TabBar : public Widget {
public:
    struct Tab {
        std::string text;           // نص التبويب
        std::string icon;           // أيقونة اختيارية
        bool enabled = true;        // مفعّل أم لا
    };

    std::vector<Tab> tabs;          // التبويبات
    int selectedIndex = 0;          // التبويب المحدد
    bool isScrollable = false;      // قابل للتمرير
    Color indicatorColor = Color(1, 1, 1, 1);     // لون المؤشر
    float indicatorWeight = 3.0f;   // سمك المؤشر
    Color labelColor = Color(1, 1, 1, 1);         // لون النص المحدد
    Color unselectedLabelColor = Color(0.7f, 0.7f, 0.7f, 1); // لون النص غير المحدد
    float height = 48.0f;
    Color backgroundColor = Color(0.25f, 0.32f, 0.71f, 1);
    bool dividerEnabled = true;     // خط فاصل تحت الشريط

    // --- التحريك ---
    float indicatorAnimProgress = 0.0f; // تقدم تحريك المؤشر
    float indicatorX = 0.0f;       // موضع المؤشر الحالي

    // --- الأحداث ---
    std::function<void(int)> onTabChanged;

    void addTab(const std::string& text, const std::string& icon = "") {
        tabs.push_back({text, icon, true});
    }

    void selectTab(int index) {
        if (index >= 0 && index < (int)tabs.size() && tabs[index].enabled) {
            selectedIndex = index;
            if (onTabChanged) onTabChanged(index);
        }
    }

    void render(Renderer2D& renderer) override {
        // رسم خلفية الشريط
        renderer.fillRect(position.x, position.y, size.x, height, backgroundColor);

        float tabWidth = isScrollable ? 120.0f : size.x / tabs.size();
        for (size_t i = 0; i < tabs.size(); i++) {
            Color color = (i == (size_t)selectedIndex) ? labelColor : unselectedLabelColor;
            float tx = position.x + i * tabWidth + tabWidth / 2;
            float ty = position.y + height / 2;
            renderer.drawText(tabs[i].text, tx, ty, color);
        }

        // رسم المؤشر
        float indX = position.x + selectedIndex * tabWidth;
        renderer.fillRect(indX, position.y + height - indicatorWeight,
                         tabWidth, indicatorWeight, indicatorColor);
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float tabWidth = isScrollable ? 120.0f : size.x / tabs.size();
            int clicked = (int)((event.button.x - position.x) / tabWidth);
            selectTab(clicked);
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 4. الدرج الجانبي - Drawer
// ═══════════════════════════════════════════════════════════════

/**
 * @brief موضع الدرج
 */
enum class DrawerPosition {
    Start,  // البداية (يسار في LTR، يمين في RTL)
    End     // النهاية
};

/**
 * @class Drawer
 * @brief درج جانبي منزلق - للقوائم والإعدادات
 * 
 * يوفر:
 * - قائمة عناصر مع أيقونات
 * - رأس الدرج (صورة المستخدم مثلاً)
 * - قسم مع فواصل
 * - تأثير انزلاق سلس
 * - دعم RTL تلقائي
 */
class Drawer : public Widget {
public:
    struct DrawerItem {
        std::string icon;
        std::string title;
        std::string subtitle;       // نص فرعي اختياري
        bool selected = false;
        bool enabled = true;
        int badgeCount = 0;
        std::function<void()> onTap;
    };

    struct DrawerSection {
        std::string title;          // عنوان القسم (اختياري)
        std::vector<DrawerItem> items;
        bool showDivider = true;    // فاصل بعد القسم
    };

    // --- الخصائص ---
    std::string headerTitle;        // عنوان الرأس
    std::string headerSubtitle;     // عنوان فرعي
    std::string headerImagePath;    // صورة الرأس
    Color headerBackground = Color(0.25f, 0.32f, 0.71f, 1);
    float headerHeight = 160.0f;

    std::vector<DrawerSection> sections;
    float width = 304.0f;          // عرض الدرج (Material: 256-400)
    Color backgroundColor = Color(1, 1, 1, 1);
    DrawerPosition drawerPosition = DrawerPosition::Start;
    float elevation = 16.0f;
    bool isOpen = false;
    float openProgress = 0.0f;     // 0 = مغلق، 1 = مفتوح
    bool scrimEnabled = true;      // طبقة شفافة خلف الدرج
    Color scrimColor = Color(0, 0, 0, 0.5f);

    // --- الأحداث ---
    std::function<void(bool)> onOpenChanged;
    std::function<void(int, int)> onItemTap; // (قسم، عنصر)

    void open() {
        isOpen = true;
        if (onOpenChanged) onOpenChanged(true);
    }

    void close() {
        isOpen = false;
        if (onOpenChanged) onOpenChanged(false);
    }

    void toggle() {
        isOpen ? close() : open();
    }

    void addSection(const std::string& title = "") {
        sections.push_back({title, {}, true});
    }

    void addItem(int sectionIndex, const std::string& icon,
                 const std::string& title, std::function<void()> callback) {
        if (sectionIndex >= 0 && sectionIndex < (int)sections.size()) {
            sections[sectionIndex].items.push_back(
                {icon, title, "", false, true, 0, callback});
        }
    }

    void render(Renderer2D& renderer) override {
        if (!isOpen && openProgress <= 0) return;

        // رسم طبقة الخلفية الشفافة
        if (scrimEnabled) {
            Color scrim = scrimColor;
            scrim.a *= openProgress;
            renderer.fillRect(0, 0, 9999, 9999, scrim);
        }

        // حساب موضع الدرج
        float drawerX = (drawerPosition == DrawerPosition::Start)
            ? position.x - width * (1.0f - openProgress)
            : position.x + size.x - width * openProgress;

        // رسم الدرج
        renderer.fillRect(drawerX, position.y, width, size.y, backgroundColor);

        // رسم الرأس
        renderer.fillRect(drawerX, position.y, width, headerHeight, headerBackground);
        renderer.drawText(headerTitle, drawerX + 16, position.y + headerHeight - 40,
                         Color(1, 1, 1, 1));
    }

    void handleEvent(const SDL_Event& event) override {
        // معالجة السحب والنقر
    }
};

// ═══════════════════════════════════════════════════════════════
// 5. سكة التنقل الجانبية - NavigationRail
// ═══════════════════════════════════════════════════════════════

/**
 * @class NavigationRail
 * @brief سكة تنقل جانبية - للشاشات العريضة (سطح المكتب والتابلت)
 * 
 * يوفر:
 * - عناصر تنقل رأسية مع أيقونات
 * - دعم التوسع لإظهار النصوص
 * - زر إجراء عائم اختياري
 * - مثالي للتخطيط المتجاوب
 */
class NavigationRail : public Widget {
public:
    struct RailItem {
        std::string icon;
        std::string selectedIcon;
        std::string label;
        int badgeCount = 0;
    };

    std::vector<RailItem> items;
    int selectedIndex = 0;
    float width = 72.0f;           // عرض مطوي
    float expandedWidth = 256.0f;  // عرض موسع
    bool isExpanded = false;
    bool showLabels = true;
    Color backgroundColor = Color(0.96f, 0.96f, 0.96f, 1);
    Color selectedColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color unselectedColor = Color(0.45f, 0.45f, 0.45f, 1);
    Color indicatorColor = Color(0.88f, 0.88f, 1.0f, 1);
    float indicatorRadius = 16.0f;
    bool hasFab = false;           // زر إجراء عائم
    std::string fabIcon;
    std::function<void()> onFabPressed;

    // --- محاذاة العناصر ---
    enum class Alignment { Top, Center, Bottom } alignment = Alignment::Center;

    // --- الأحداث ---
    std::function<void(int)> onDestinationSelected;

    void addItem(const std::string& icon, const std::string& label) {
        items.push_back({icon, icon, label, 0});
    }

    void toggleExpansion() {
        isExpanded = !isExpanded;
    }

    float getCurrentWidth() const {
        return isExpanded ? expandedWidth : width;
    }

    void render(Renderer2D& renderer) override {
        float w = getCurrentWidth();
        renderer.fillRect(position.x, position.y, w, size.y, backgroundColor);

        float itemHeight = 56.0f;
        float startY = position.y;
        if (alignment == Alignment::Center)
            startY = position.y + (size.y - items.size() * itemHeight) / 2;

        for (size_t i = 0; i < items.size(); i++) {
            float iy = startY + i * itemHeight;
            bool selected = (i == (size_t)selectedIndex);

            if (selected) {
                renderer.fillRect(position.x + (w - 56) / 2, iy + 4,
                                 56, 32, indicatorColor);
            }

            Color color = selected ? selectedColor : unselectedColor;
            if (isExpanded) {
                renderer.drawText(items[i].label, position.x + 72, iy + itemHeight / 2, color);
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float itemHeight = 56.0f;
            float startY = position.y;
            if (alignment == Alignment::Center)
                startY = position.y + (size.y - items.size() * itemHeight) / 2;
            
            int clicked = (int)((event.button.y - startY) / itemHeight);
            if (clicked >= 0 && clicked < (int)items.size()) {
                selectedIndex = clicked;
                if (onDestinationSelected) onDestinationSelected(clicked);
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 6. مسار التنقل - Breadcrumbs
// ═══════════════════════════════════════════════════════════════

/**
 * @class Breadcrumbs
 * @brief مسار التنقل - يوضح الموقع الحالي في التسلسل الهرمي
 * 
 * يوفر:
 * - عناصر مسار قابلة للنقر
 * - فاصل قابل للتخصيص بين العناصر
 * - طي تلقائي عند كثرة العناصر
 * - دعم RTL
 */
class Breadcrumbs : public Widget {
public:
    struct BreadcrumbItem {
        std::string text;
        std::string icon;           // أيقونة اختيارية
        std::function<void()> onTap;
        bool isLast = false;        // العنصر الأخير (الحالي)
    };

    std::vector<BreadcrumbItem> items;
    std::string separator = " > "; // الفاصل بين العناصر
    Color activeColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color inactiveColor = Color(0.5f, 0.5f, 0.5f, 1);
    Color separatorColor = Color(0.7f, 0.7f, 0.7f, 1);
    float fontSize = 14.0f;
    int maxVisibleItems = 0;       // 0 = بلا حد
    bool collapsible = true;       // طي العناصر الوسطى

    void addItem(const std::string& text, std::function<void()> callback = nullptr) {
        // تحديث العنصر الأخير السابق
        if (!items.empty()) items.back().isLast = false;
        items.push_back({text, "", callback, true});
    }

    void clear() { items.clear(); }

    void render(Renderer2D& renderer) override {
        float x = position.x;
        float y = position.y + size.y / 2;

        for (size_t i = 0; i < items.size(); i++) {
            Color color = items[i].isLast ? activeColor : inactiveColor;
            renderer.drawText(items[i].text, x, y, color);
            x += items[i].text.length() * fontSize * 0.6f;

            if (!items[i].isLast) {
                renderer.drawText(separator, x, y, separatorColor);
                x += separator.length() * fontSize * 0.6f;
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 7. المعالج خطوة بخطوة - Stepper
// ═══════════════════════════════════════════════════════════════

/**
 * @brief نوع المعالج
 */
enum class StepperType {
    Horizontal,  // أفقي
    Vertical     // رأسي
};

/**
 * @brief حالة الخطوة
 */
enum class StepState {
    Indexed,    // رقم الخطوة
    Editing,    // قيد التحرير
    Complete,   // مكتملة
    Disabled,   // معطلة
    Error       // خطأ
};

/**
 * @class Stepper
 * @brief معالج خطوة بخطوة - لنماذج متعددة الخطوات
 * 
 * يوفر:
 * - خطوات مرقمة مع عناوين
 * - تنقل للأمام والخلف
 * - تحقق من صحة كل خطوة
 * - أفقي ورأسي
 * - حالات مختلفة (مكتمل، خطأ، معطل)
 */
class Stepper : public Widget {
public:
    struct Step {
        std::string title;          // عنوان الخطوة
        std::string subtitle;       // وصف اختياري
        StepState state = StepState::Indexed;
        bool isActive = false;
        std::function<bool()> validator; // دالة التحقق
        std::shared_ptr<Widget> content; // محتوى الخطوة
    };

    std::vector<Step> steps;
    int currentStep = 0;
    StepperType type = StepperType::Horizontal;
    Color activeColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color inactiveColor = Color(0.7f, 0.7f, 0.7f, 1);
    Color errorColor = Color(0.9f, 0.2f, 0.2f, 1);
    Color completeColor = Color(0.3f, 0.7f, 0.3f, 1);
    float circleRadius = 16.0f;    // حجم دائرة الخطوة
    float lineThickness = 2.0f;    // سمك خط الاتصال
    bool controlsVisible = true;   // إظهار أزرار التالي/السابق

    // --- الأحداث ---
    std::function<void(int)> onStepChanged;
    std::function<void()> onCompleted;     // عند إكمال جميع الخطوات

    void addStep(const std::string& title, const std::string& subtitle = "") {
        steps.push_back({title, subtitle, StepState::Indexed, false, nullptr, nullptr});
        if (steps.size() == 1) steps[0].isActive = true;
    }

    bool nextStep() {
        // التحقق من صحة الخطوة الحالية
        if (steps[currentStep].validator && !steps[currentStep].validator()) {
            steps[currentStep].state = StepState::Error;
            return false;
        }
        steps[currentStep].state = StepState::Complete;
        steps[currentStep].isActive = false;

        if (currentStep < (int)steps.size() - 1) {
            currentStep++;
            steps[currentStep].isActive = true;
            if (onStepChanged) onStepChanged(currentStep);
            return true;
        } else {
            if (onCompleted) onCompleted();
            return true;
        }
    }

    bool previousStep() {
        if (currentStep > 0) {
            steps[currentStep].isActive = false;
            currentStep--;
            steps[currentStep].isActive = true;
            steps[currentStep].state = StepState::Editing;
            if (onStepChanged) onStepChanged(currentStep);
            return true;
        }
        return false;
    }

    void render(Renderer2D& renderer) override {
        if (type == StepperType::Horizontal) {
            float stepWidth = size.x / steps.size();
            for (size_t i = 0; i < steps.size(); i++) {
                float cx = position.x + i * stepWidth + stepWidth / 2;
                float cy = position.y + 24;

                // تحديد اللون حسب الحالة
                Color c = inactiveColor;
                if (steps[i].state == StepState::Complete) c = completeColor;
                else if (steps[i].state == StepState::Error) c = errorColor;
                else if (steps[i].isActive) c = activeColor;

                // رسم الدائرة
                renderer.fillRect(cx - circleRadius, cy - circleRadius,
                                 circleRadius * 2, circleRadius * 2, c);

                // رسم الخط بين الخطوات
                if (i < steps.size() - 1) {
                    Color lineColor = (steps[i].state == StepState::Complete)
                        ? activeColor : inactiveColor;
                    renderer.fillRect(cx + circleRadius, cy - lineThickness / 2,
                                     stepWidth - circleRadius * 2, lineThickness, lineColor);
                }

                // رسم العنوان
                renderer.drawText(steps[i].title, cx, cy + circleRadius + 16,
                                 steps[i].isActive ? activeColor : inactiveColor);
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {}
};

// ═══════════════════════════════════════════════════════════════
// 8. عرض الصفحات - PageView
// ═══════════════════════════════════════════════════════════════

/**
 * @brief اتجاه التمرير في عرض الصفحات
 */
enum class PageScrollDirection {
    Horizontal, // أفقي
    Vertical    // رأسي
};

/**
 * @class PageView
 * @brief عرض صفحات قابل للتمرير - للعروض التقديمية والمعالجات
 * 
 * يوفر:
 * - تمرير أفقي ورأسي بين الصفحات
 * - مؤشر نقاط الصفحات
 * - تأثيرات انتقال مخصصة
 * - تمرير تلقائي (للإعلانات مثلاً)
 */
class PageView : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> pages; // الصفحات
    int currentPage = 0;
    PageScrollDirection scrollDirection = PageScrollDirection::Horizontal;
    bool pageSnapping = true;      // الانتقال لصفحة كاملة
    bool autoScroll = false;       // تمرير تلقائي
    float autoScrollInterval = 3.0f; // فترة التمرير (ثانية)
    bool loop = false;             // تكرار الصفحات

    // --- المؤشر ---
    bool showIndicator = true;
    Color activeIndicatorColor = Color(0.25f, 0.32f, 0.71f, 1);
    Color inactiveIndicatorColor = Color(0.7f, 0.7f, 0.7f, 1);
    float indicatorSize = 8.0f;
    float indicatorSpacing = 8.0f;
    float indicatorBottom = 20.0f; // المسافة من الأسفل

    // --- التحريك ---
    float scrollOffset = 0.0f;    // إزاحة التمرير
    float animSpeed = 5.0f;       // سرعة التحريك
    float autoScrollTimer = 0.0f;

    // --- الأحداث ---
    std::function<void(int)> onPageChanged;

    void addPage(std::shared_ptr<Widget> page) {
        pages.push_back(page);
    }

    void goToPage(int index) {
        if (index >= 0 && index < (int)pages.size()) {
            currentPage = index;
            if (onPageChanged) onPageChanged(currentPage);
        }
    }

    void nextPage() {
        if (currentPage < (int)pages.size() - 1) {
            goToPage(currentPage + 1);
        } else if (loop) {
            goToPage(0);
        }
    }

    void previousPage() {
        if (currentPage > 0) {
            goToPage(currentPage - 1);
        } else if (loop) {
            goToPage((int)pages.size() - 1);
        }
    }

    void update(float dt) {
        // تحديث التمرير التلقائي
        if (autoScroll) {
            autoScrollTimer += dt;
            if (autoScrollTimer >= autoScrollInterval) {
                autoScrollTimer = 0;
                nextPage();
            }
        }
    }

    void render(Renderer2D& renderer) override {
        // رسم الصفحة الحالية
        if (currentPage >= 0 && currentPage < (int)pages.size()) {
            pages[currentPage]->render(renderer);
        }

        // رسم مؤشر الصفحات
        if (showIndicator && pages.size() > 1) {
            float totalWidth = pages.size() * indicatorSize +
                              (pages.size() - 1) * indicatorSpacing;
            float startX = position.x + (size.x - totalWidth) / 2;
            float y = position.y + size.y - indicatorBottom;

            for (size_t i = 0; i < pages.size(); i++) {
                Color c = (i == (size_t)currentPage) ?
                    activeIndicatorColor : inactiveIndicatorColor;
                float dotSize = (i == (size_t)currentPage) ?
                    indicatorSize * 1.5f : indicatorSize;
                renderer.fillRect(startX - dotSize / 2, y - dotSize / 2,
                                 dotSize, dotSize, c);
                startX += indicatorSize + indicatorSpacing;
            }
        }
    }

    void handleEvent(const SDL_Event& event) override {
        // معالجة السحب للتبديل بين الصفحات
    }
};

// ═══════════════════════════════════════════════════════════════
// 9. شريط التنقل الحديث - NavigationBar
// ═══════════════════════════════════════════════════════════════

/**
 * @class NavigationBar
 * @brief شريط تنقل حديث (Material 3) - بديل محسن لـ BottomNavigationBar
 * 
 * يوفر:
 * - تصميم Material 3 حديث
 * - مؤشر حبة الدواء عند التحديد
 * - تأثيرات لمس متموجة
 * - شارات إشعارات
 */
class NavigationBar : public Widget {
public:
    struct Destination {
        std::string icon;
        std::string selectedIcon;
        std::string label;
        int badgeCount = 0;
        bool hasBadge = false;
    };

    std::vector<Destination> destinations;
    int selectedIndex = 0;
    float height = 80.0f;
    Color backgroundColor = Color(0.97f, 0.97f, 0.97f, 1);
    Color indicatorColor = Color(0.88f, 0.88f, 1.0f, 1);
    Color selectedIconColor = Color(0.1f, 0.1f, 0.1f, 1);
    Color unselectedIconColor = Color(0.45f, 0.45f, 0.45f, 1);
    float elevation = 3.0f;
    float indicatorWidth = 64.0f;
    float indicatorHeight = 32.0f;
    float indicatorRadius = 16.0f;

    // --- الأحداث ---
    std::function<void(int)> onDestinationSelected;

    void addDestination(const std::string& icon, const std::string& label) {
        destinations.push_back({icon, icon, label, 0, false});
    }

    void render(Renderer2D& renderer) override {
        renderer.fillRect(position.x, position.y, size.x, height, backgroundColor);

        float itemWidth = size.x / destinations.size();
        for (size_t i = 0; i < destinations.size(); i++) {
            float cx = position.x + i * itemWidth + itemWidth / 2;
            float cy = position.y + 16;

            // مؤشر التحديد (شكل حبة الدواء)
            if (i == (size_t)selectedIndex) {
                renderer.fillRect(cx - indicatorWidth / 2, cy,
                                 indicatorWidth, indicatorHeight, indicatorColor);
            }

            Color color = (i == (size_t)selectedIndex) ?
                selectedIconColor : unselectedIconColor;
            renderer.drawText(destinations[i].label, cx, cy + 40, color);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float itemWidth = size.x / destinations.size();
            int clicked = (int)((event.button.x - position.x) / itemWidth);
            if (clicked >= 0 && clicked < (int)destinations.size()) {
                selectedIndex = clicked;
                if (onDestinationSelected) onDestinationSelected(clicked);
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// 10. التنقل المتداخل - NestedNavigator
// ═══════════════════════════════════════════════════════════════

/**
 * @class NestedNavigator
 * @brief مدير تنقل متداخل - لإدارة مكدس الشاشات
 * 
 * يوفر:
 * - مكدس شاشات (Push/Pop)
 * - تنقل بالأسماء (Named Routes)
 * - تمرير بيانات بين الشاشات
 * - تأثيرات انتقال مخصصة
 * - حراسة المسارات (Route Guards)
 */
class NestedNavigator : public Widget {
public:
    struct Route {
        std::string name;           // اسم المسار
        std::function<std::shared_ptr<Widget>()> builder; // بناء الشاشة
        std::function<bool()> guard; // حارس المسار (يسمح أو يمنع)
        std::unordered_map<std::string, std::string> params; // معاملات المسار
    };

    /**
     * @brief نوع تأثير الانتقال
     */
    enum class TransitionType {
        SlideRight,  // انزلاق يمين
        SlideLeft,   // انزلاق يسار
        SlideUp,     // انزلاق أعلى
        SlideDown,   // انزلاق أسفل
        Fade,        // تلاشي
        Scale,       // تكبير
        None         // بدون تأثير
    };

    // --- المكدس والمسارات ---
    std::vector<Route> routeStack;  // مكدس المسارات
    std::unordered_map<std::string, Route> namedRoutes; // مسارات مسماة
    std::string initialRoute = "/";
    TransitionType defaultTransition = TransitionType::SlideRight;
    float transitionDuration = 0.3f;

    // --- حالة الانتقال ---
    float transitionProgress = 1.0f; // 0 = بداية، 1 = نهاية
    bool isTransitioning = false;

    // --- الأحداث ---
    std::function<void(const std::string&)> onRouteChanged;
    std::function<bool(const std::string&)> onWillPop; // هل يسمح بالرجوع

    void registerRoute(const std::string& name,
                      std::function<std::shared_ptr<Widget>()> builder,
                      std::function<bool()> guard = nullptr) {
        namedRoutes[name] = {name, builder, guard, {}};
    }

    bool pushNamed(const std::string& name,
                  const std::unordered_map<std::string, std::string>& params = {}) {
        auto it = namedRoutes.find(name);
        if (it == namedRoutes.end()) return false;

        // التحقق من حارس المسار
        if (it->second.guard && !it->second.guard()) return false;

        Route route = it->second;
        route.params = params;
        routeStack.push_back(route);

        isTransitioning = true;
        transitionProgress = 0.0f;

        if (onRouteChanged) onRouteChanged(name);
        return true;
    }

    bool pop() {
        if (routeStack.size() <= 1) return false;

        // التحقق من إمكانية الرجوع
        if (onWillPop && !onWillPop(routeStack.back().name)) return false;

        routeStack.pop_back();
        isTransitioning = true;
        transitionProgress = 0.0f;

        if (onRouteChanged && !routeStack.empty())
            onRouteChanged(routeStack.back().name);
        return true;
    }

    void popUntil(const std::string& name) {
        while (routeStack.size() > 1 && routeStack.back().name != name) {
            routeStack.pop_back();
        }
    }

    void pushReplacementNamed(const std::string& name) {
        if (!routeStack.empty()) routeStack.pop_back();
        pushNamed(name);
    }

    std::string currentRoute() const {
        return routeStack.empty() ? "" : routeStack.back().name;
    }

    bool canPop() const { return routeStack.size() > 1; }
    int stackDepth() const { return (int)routeStack.size(); }

    void update(float dt) {
        if (isTransitioning) {
            transitionProgress += dt / transitionDuration;
            if (transitionProgress >= 1.0f) {
                transitionProgress = 1.0f;
                isTransitioning = false;
            }
        }
    }

    void render(Renderer2D& renderer) override {
        if (routeStack.empty()) return;
        // رسم الشاشة الحالية مع تأثير الانتقال
        auto& currentRoute = routeStack.back();
        if (currentRoute.builder) {
            auto widget = currentRoute.builder();
            if (widget) widget->render(renderer);
        }
    }

    void handleEvent(const SDL_Event& event) override {
        // تمرير الأحداث للشاشة الحالية
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad
