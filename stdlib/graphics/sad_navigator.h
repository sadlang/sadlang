// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_navigator.h — نظام التنقل والتوجيه بين الصفحات
// ─────────────────────────────────────────────────────────────────────────────
// يوفر نظام تنقل كامل مستوحى من Flutter Navigator 2.0:
//   - مكدس صفحات (Page Stack) مع push/pop
//   - مسارات مسمّاة (Named Routes)
//   - انتقالات متحركة (Page Transitions) مع أنيميشن
//   - تاريخ التنقل (Back/Forward)
//   - حوارات (Dialogs) ونوافذ سفلية (Bottom Sheets)
//   - Tab Navigation
//   - Deep Linking (روابط عميقة)
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace sad { namespace nav {

// ═══════════════════════════════════════════════════════════════════
// أنواع الانتقال بين الصفحات
// ═══════════════════════════════════════════════════════════════════
enum class TransitionType {
    None,           // بدون انتقال — تبديل فوري
    Fade,           // تلاشي — الصفحة القديمة تختفي والجديدة تظهر
    SlideRight,     // انزلاق من اليمين (RTL — الافتراضي للعربية)
    SlideLeft,      // انزلاق من اليسار (LTR)
    SlideUp,        // انزلاق من الأسفل
    SlideDown,      // انزلاق من الأعلى
    Scale,          // تكبير من المركز
    ScaleAndFade,   // تكبير + تلاشي
    Rotation,       // دوران ثلاثي الأبعاد (محاكاة)
    Flip            // انقلاب (محاكاة)
};

// ═══════════════════════════════════════════════════════════════════
// صفحة في مكدس التنقل
// ═══════════════════════════════════════════════════════════════════
struct Page {
    std::string name;               // اسم الصفحة (المسار)
    int rootWidgetId = 0;           // ودجت الجذر لهذه الصفحة
    TransitionType transition = TransitionType::SlideRight;
    float transitionProgress = 1.0f; // 0 = مختفية، 1 = ظاهرة بالكامل
    bool isAnimating = false;
    
    // بيانات الصفحة (arguments)
    std::unordered_map<std::string, std::string> args;
};

// ═══════════════════════════════════════════════════════════════════
// نوع بنّاء الصفحة — دالة تُعيد معرّف ودجت الجذر
// ═══════════════════════════════════════════════════════════════════
using PageBuilder = std::function<int(const std::unordered_map<std::string, std::string>& args)>;

// ═══════════════════════════════════════════════════════════════════
// إدارة المسارات (Routes)
// ═══════════════════════════════════════════════════════════════════

// تسجيل مسار مسمّى مع دالة بناء
// مثال: registerRoute("الرئيسية", buildHomePage);
void registerRoute(const std::string& routeName, PageBuilder builder);

// تسجيل مسار مع نوع انتقال مخصص
void registerRoute(const std::string& routeName, PageBuilder builder,
                   TransitionType transition);

// إلغاء تسجيل مسار
void unregisterRoute(const std::string& routeName);

// هل المسار مسجّل؟
bool hasRoute(const std::string& routeName);

// جميع المسارات المسجّلة
std::vector<std::string> getRegisteredRoutes();

// ═══════════════════════════════════════════════════════════════════
// التنقل الأساسي — مكدس الصفحات
// ═══════════════════════════════════════════════════════════════════

// الانتقال إلى صفحة جديدة (تُضاف فوق المكدس)
// يُرجع true إذا نجح الانتقال
bool push(const std::string& routeName);

// الانتقال مع بيانات (arguments)
bool push(const std::string& routeName,
          const std::unordered_map<std::string, std::string>& args);

// الانتقال مع ودجت مباشر (بدون مسار مسمّى)
bool pushWidget(int widgetId, const std::string& pageName = "");

// العودة للصفحة السابقة
bool pop();

// العودة مع قيمة مُرجعة
bool popWithResult(const std::string& result);

// استبدال الصفحة الحالية بأخرى (بدون إضافة للمكدس)
bool replace(const std::string& routeName);

// مسح جميع الصفحات والانتقال لصفحة واحدة
bool pushAndClearAll(const std::string& routeName);

// العودة لصفحة محددة في المكدس
bool popUntil(const std::string& routeName);

// ═══════════════════════════════════════════════════════════════════
// استعلامات المكدس
// ═══════════════════════════════════════════════════════════════════

// الصفحة الحالية (أعلى المكدس)
std::string currentRoute();

// هل يمكن العودة؟
bool canPop();

// عدد الصفحات في المكدس
int stackSize();

// الحصول على بيانات الصفحة الحالية
std::string getArg(const std::string& key, const std::string& defaultVal = "");

// الحصول على ودجت الصفحة الحالية
int currentPageWidget();

// تاريخ التنقل
std::vector<std::string> getNavigationHistory();

// ═══════════════════════════════════════════════════════════════════
// الانتقالات المتحركة
// ═══════════════════════════════════════════════════════════════════

// تعيين نوع الانتقال الافتراضي
void setDefaultTransition(TransitionType type);

// تعيين مدة الانتقال (بالمللي ثانية)
void setTransitionDuration(int durationMs);

// تحديث الانتقالات — يُستدعى كل إطار من app_update()
void updateTransitions(double deltaMs);

// هل هناك انتقال جارٍ؟
bool isTransitioning();

// الحصول على تقدم الانتقال الحالي (0-1)
float getTransitionProgress();

// ═══════════════════════════════════════════════════════════════════
// الحوارات والنوافذ المنبثقة
// ═══════════════════════════════════════════════════════════════════

// عرض حوار فوق الصفحة الحالية
int showDialog(int dialogWidgetId);

// إخفاء الحوار
void dismissDialog();

// هل يوجد حوار ظاهر؟
bool isDialogShowing();

// عرض نافذة سفلية (Bottom Sheet)
int showBottomSheet(int widgetId);

// إخفاء النافذة السفلية
void dismissBottomSheet();

// ═══════════════════════════════════════════════════════════════════
// التنقل بالتبويب (Tab Navigation)
// ═══════════════════════════════════════════════════════════════════

// إنشاء متنقل تبويب بعدد معين من التبويبات
int createTabNavigator(int tabCount);

// الانتقال لتبويب محدد
void switchTab(int tabIndex);

// التبويب الحالي
int currentTab();

// تعيين ودجت لتبويب محدد
void setTabWidget(int tabIndex, int widgetId);

// الحصول على ودجت تبويب
int getTabWidget(int tabIndex);

// ═══════════════════════════════════════════════════════════════════
// تنظيف
// ═══════════════════════════════════════════════════════════════════

// مسح جميع المسارات والمكدس
void clearAll();

// إعادة تعيين المتنقل
void reset();

}} // namespace sad::nav
