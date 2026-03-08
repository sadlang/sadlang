// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_arabic.h — واجهة عربية لنظام Flutter
//  Arabic API Wrappers for Flutter-like UI System
// ═══════════════════════════════════════════════════════════════════════════════
//
//  هذا الملف يوفر دوال عربية سهلة الاستخدام لبناء واجهات المستخدم
//  جميع الدوال تُعيد WidgetPtr (مؤشر ذكي لـ Widget)
//
//  مثال الاستخدام:
//  ```cpp
//  auto واجهة = عمود({
//      نص("مرحباً بالعالم")،
//      زر("اضغط هنا"، []{ /* ... */ })،
//      صف({
//          ايقونة(رموز::نجمة)،
//          نص("مفضل")
//      })
//  });
//  ```
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"
#include <initializer_list>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Type Aliases — أسماء بديلة للأنواع
// ═══════════════════════════════════════════════════════════════════════════════

using مكون = Widget;
using مكون_ذكي = WidgetPtr;
using سياق_بناء = BuildContext;
using مفتاح = Key;
using لون = Color;
using حواف = EdgeInsets;
using انحناء = BorderRadius;
using حدود = Border;
using ظل = BoxShadow;
using نمط_نص = TextStyle;
using قيود_صندوق = BoxConstraints;
using تزيين_صندوق = BoxDecoration;
using محاذاة_نص = TextAlign;
using وزن_خط = FontWeight;
using محور = Axis;
using محاذاة_رئيسية = MainAxisAlignment;
using محاذاة_عرضية = CrossAxisAlignment;
using حجم_رئيسي = MainAxisSize;
using محاذاة = Alignment;
using طريقة_قص = Clip;

// ═══════════════════════════════════════════════════════════════════════════════
//  Icon Enum Aliases — رموز الأيقونات
// ═══════════════════════════════════════════════════════════════════════════════

namespace رموز {
    constexpr Icons اضافة = Icons::Add;
    constexpr Icons حذف = Icons::Remove;
    constexpr Icons تعديل = Icons::Edit;
    constexpr Icons مسح = Icons::Delete;
    constexpr Icons حفظ = Icons::Save;
    constexpr Icons اغلاق = Icons::Close;
    constexpr Icons قائمة = Icons::Menu;
    constexpr Icons رئيسية = Icons::Home;
    constexpr Icons اعدادات = Icons::Settings;
    constexpr Icons بحث = Icons::Search;
    constexpr Icons شخص = Icons::Person;
    constexpr Icons نجمة = Icons::Star;
    constexpr Icons مفضل = Icons::Favorite;
    constexpr Icons صح = Icons::Check;
    constexpr Icons الغاء = Icons::Clear;
    constexpr Icons رجوع = Icons::ArrowBack;
    constexpr Icons تقدم = Icons::ArrowForward;
    constexpr Icons المزيد = Icons::MoreVert;
    constexpr Icons اظهار = Icons::Visibility;
    constexpr Icons اخفاء = Icons::VisibilityOff;
    constexpr Icons قفل = Icons::Lock;
    constexpr Icons فتح_قفل = Icons::LockOpen;
    constexpr Icons تحديث = Icons::Refresh;
    constexpr Icons مشاركة = Icons::Share;
    constexpr Icons تحميل = Icons::Download;
    constexpr Icons رفع = Icons::Upload;
    constexpr Icons كاميرا = Icons::Camera;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Color Helpers — ألوان جاهزة
// ═══════════════════════════════════════════════════════════════════════════════

namespace الوان {
    inline Color اسود()    { return Color::black(); }
    inline Color ابيض()    { return Color::white(); }
    inline Color احمر()    { return Color::red(); }
    inline Color اخضر()    { return Color::green(); }
    inline Color ازرق()    { return Color::blue(); }
    inline Color اصفر()    { return Color::yellow(); }
    inline Color برتقالي() { return Color::orange(); }
    inline Color بنفسجي()  { return Color::purple(); }
    inline Color رمادي()   { return Color::grey(); }
    inline Color شفاف()    { return Color::transparent(); }
    
    /// لون مخصص RGB
    inline Color مخصص(uint8_t ر, uint8_t خ, uint8_t ز, uint8_t ش = 255) {
        return Color(ر, خ, ز, ش);
    }
    
    /// لون من HEX
    inline Color من_هكس(uint32_t قيمة) {
        return Color::fromHex(قيمة);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Basic Widgets — المكونات الأساسية
// ═══════════════════════════════════════════════════════════════════════════════

/// نص — عرض نص
inline WidgetPtr نص(
    const std::string& محتوى,
    TextStyle نمط = {},
    TextAlign محاذاة = TextAlign::Right,
    int اقصى_سطور = 0
) {
    return std::make_shared<Text>(محتوى, نمط, محاذاة, اقصى_سطور);
}

/// نص مع حجم وخط
inline WidgetPtr نص_كبير(const std::string& محتوى) {
    TextStyle نمط;
    نمط.fontSize = 24;
    نمط.fontWeight = FontWeight::Bold;
    return std::make_shared<Text>(محتوى, نمط);
}

inline WidgetPtr نص_متوسط(const std::string& محتوى) {
    TextStyle نمط;
    نمط.fontSize = 18;
    return std::make_shared<Text>(محتوى, نمط);
}

inline WidgetPtr نص_صغير(const std::string& محتوى) {
    TextStyle نمط;
    نمط.fontSize = 12;
    نمط.color = Color::grey();
    return std::make_shared<Text>(محتوى, نمط);
}

/// حاوية — صندوق مع تزيين
inline WidgetPtr حاوية(
    WidgetPtr طفل = nullptr,
    std::optional<float> عرض = std::nullopt,
    std::optional<float> ارتفاع = std::nullopt,
    EdgeInsets حشو = {},
    EdgeInsets هامش = {},
    BoxDecoration تزيين = {}
) {
    return std::make_shared<Container>(
        std::move(طفل), عرض, ارتفاع, حشو, هامش, std::move(تزيين)
    );
}

/// صندوق — حاوية بلون خلفية
inline WidgetPtr صندوق(
    WidgetPtr طفل,
    Color خلفية = Color::white(),
    float انحناء_حواف = 0,
    EdgeInsets حشو = {}
) {
    BoxDecoration تزيين;
    تزيين.setColor(خلفية).setRadius(انحناء_حواف);
    return std::make_shared<Container>(std::move(طفل), std::nullopt, std::nullopt, حشو, {}, تزيين);
}

/// بطاقة — حاوية مع ظل
inline WidgetPtr بطاقة(
    WidgetPtr طفل,
    Color خلفية = Color::white(),
    float انحناء_حواف = 8,
    EdgeInsets حشو = EdgeInsets::all(16)
) {
    BoxDecoration تزيين;
    تزيين.setColor(خلفية)
         .setRadius(انحناء_حواف)
         .addShadow(BoxShadow{Color(0,0,0,32), 8, 0, 0, 4});
    return std::make_shared<Container>(std::move(طفل), std::nullopt, std::nullopt, حشو, {}, تزيين);
}

/// حجم_ثابت — صندوق بحجم محدد
inline WidgetPtr حجم_ثابت(
    float عرض,
    float ارتفاع,
    WidgetPtr طفل = nullptr
) {
    return std::make_shared<SizedBox>(عرض, ارتفاع, std::move(طفل));
}

/// فراغ — مسافة فارغة
inline WidgetPtr فراغ_افقي(float عرض) {
    return std::make_shared<SizedBox>(عرض, std::nullopt);
}

inline WidgetPtr فراغ_عمودي(float ارتفاع) {
    return std::make_shared<SizedBox>(std::nullopt, ارتفاع);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Buttons — الأزرار
// ═══════════════════════════════════════════════════════════════════════════════

/// زر — زر مرتفع
inline WidgetPtr زر(
    const std::string& عنوان,
    VoidCallback عند_الضغط,
    Color خلفية = Color::blue(),
    Color نص_لون = Color::white()
) {
    return std::make_shared<ElevatedButton>(
        نص(عنوان, TextStyle().setColor(نص_لون)),
        std::move(عند_الضغط),
        خلفية,
        نص_لون
    );
}

/// زر_نصي — زر بدون خلفية
inline WidgetPtr زر_نصي(
    const std::string& عنوان,
    VoidCallback عند_الضغط,
    Color لون_النص = Color::blue()
) {
    return std::make_shared<TextButton>(
        نص(عنوان, TextStyle().setColor(لون_النص)),
        std::move(عند_الضغط),
        لون_النص
    );
}

/// زر_ايقونة — زر بأيقونة
inline WidgetPtr زر_ايقونة(
    Icons رمز,
    VoidCallback عند_الضغط,
    float حجم = 24,
    Color لون = Color::black()
) {
    return std::make_shared<IconButton>(رمز, std::move(عند_الضغط), حجم, لون);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Icons — الأيقونات
// ═══════════════════════════════════════════════════════════════════════════════

/// ايقونة — عرض أيقونة
inline WidgetPtr ايقونة(Icons رمز, float حجم = 24, Color لون = Color::black()) {
    return std::make_shared<Icon>(رمز, حجم, لون);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Layout — التخطيط
// ═══════════════════════════════════════════════════════════════════════════════

/// عمود — ترتيب عمودي
inline WidgetPtr عمود(
    std::vector<WidgetPtr> ابناء,
    MainAxisAlignment محاذاة_رئيسية = MainAxisAlignment::Start,
    CrossAxisAlignment محاذاة_عرضية = CrossAxisAlignment::Start,
    float تباعد = 0
) {
    return std::make_shared<Column>(
        std::move(ابناء), محاذاة_رئيسية, MainAxisSize::Max, محاذاة_عرضية, تباعد
    );
}

/// عمود — بناء من initializer_list
inline WidgetPtr عمود(std::initializer_list<WidgetPtr> ابناء) {
    return std::make_shared<Column>(std::vector<WidgetPtr>(ابناء));
}

/// صف — ترتيب أفقي
inline WidgetPtr صف(
    std::vector<WidgetPtr> ابناء,
    MainAxisAlignment محاذاة_رئيسية = MainAxisAlignment::Start,
    CrossAxisAlignment محاذاة_عرضية = CrossAxisAlignment::Center,
    float تباعد = 0
) {
    return std::make_shared<Row>(
        std::move(ابناء), محاذاة_رئيسية, MainAxisSize::Max, محاذاة_عرضية, تباعد
    );
}

/// صف — بناء من initializer_list
inline WidgetPtr صف(std::initializer_list<WidgetPtr> ابناء) {
    return std::make_shared<Row>(std::vector<WidgetPtr>(ابناء));
}

/// تكديس — طبقات فوق بعضها
inline WidgetPtr تكديس(
    std::vector<WidgetPtr> ابناء,
    AlignmentGeometry محاذاة = AlignmentGeometry::topRight()
) {
    return std::make_shared<Stack>(std::move(ابناء), محاذاة);
}

/// تكديس — بناء من initializer_list
inline WidgetPtr تكديس(std::initializer_list<WidgetPtr> ابناء) {
    return std::make_shared<Stack>(std::vector<WidgetPtr>(ابناء));
}

/// موقع — تحديد موقع في تكديس
inline WidgetPtr موقع(
    WidgetPtr طفل,
    std::optional<float> يسار = std::nullopt,
    std::optional<float> اعلى = std::nullopt,
    std::optional<float> يمين = std::nullopt,
    std::optional<float> اسفل = std::nullopt
) {
    return std::make_shared<Positioned>(std::move(طفل), يسار, اعلى, يمين, اسفل);
}

/// توسيط — توسيط المحتوى
inline WidgetPtr توسيط(WidgetPtr طفل) {
    return std::make_shared<Center>(std::move(طفل));
}

/// محاذاة_مكون — محاذاة المحتوى
inline WidgetPtr محاذاة_مكون(
    WidgetPtr طفل,
    AlignmentGeometry محاذاة = AlignmentGeometry::center()
) {
    return std::make_shared<Align>(std::move(طفل), محاذاة);
}

/// حشو — إضافة حشو
inline WidgetPtr حشو(EdgeInsets مقدار, WidgetPtr طفل) {
    return std::make_shared<Padding>(مقدار, std::move(طفل));
}

/// توسيع — ملء المساحة المتاحة في Flex
inline WidgetPtr توسيع(WidgetPtr طفل, int نسبة = 1) {
    return std::make_shared<Expanded>(std::move(طفل), نسبة);
}

/// مرن — مرونة محدودة
inline WidgetPtr مرن(WidgetPtr طفل, int نسبة = 1) {
    return std::make_shared<Flexible>(std::move(طفل), نسبة);
}

/// فاصل_مرن — فاصل يملأ المساحة
inline WidgetPtr فاصل_مرن(int نسبة = 1) {
    return std::make_shared<Spacer>(نسبة);
}

/// خط_فاصل — خط أفقي
inline WidgetPtr خط_فاصل(float سمك = 1, Color لون = Color::grey()) {
    return std::make_shared<Divider>(1, سمك, لون);
}

/// التفاف — التفاف العناصر
inline WidgetPtr التفاف(
    std::vector<WidgetPtr> ابناء,
    float تباعد = 8,
    float تباعد_صفوف = 8
) {
    return std::make_shared<Wrap>(std::move(ابناء), Axis::Horizontal, WrapAlignment::Start, تباعد, تباعد_صفوف);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Scrolling — التمرير
// ═══════════════════════════════════════════════════════════════════════════════

/// قائمة — قائمة قابلة للتمرير
inline WidgetPtr قائمة(
    std::vector<WidgetPtr> عناصر,
    EdgeInsets حشو = {}
) {
    return std::make_shared<ListView>(std::move(عناصر), Axis::Vertical, حشو);
}

/// شبكة — شبكة من العناصر
inline WidgetPtr شبكة(
    std::vector<WidgetPtr> عناصر,
    int اعمدة,
    float تباعد = 8
) {
    return std::make_shared<GridView>(std::move(عناصر), اعمدة, تباعد, تباعد);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Effects — التأثيرات
// ═══════════════════════════════════════════════════════════════════════════════

/// شفافية — تطبيق شفافية
inline WidgetPtr شفافية(float قيمة, WidgetPtr طفل) {
    return std::make_shared<Opacity>(قيمة, std::move(طفل));
}

/// اظهار_اخفاء — إظهار أو إخفاء
inline WidgetPtr اظهار_اخفاء(bool مرئي, WidgetPtr طفل) {
    return std::make_shared<Visibility>(std::move(طفل), مرئي);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Gesture — اللمس
// ═══════════════════════════════════════════════════════════════════════════════

/// عند_الضغط — كاشف النقر
inline WidgetPtr عند_الضغط(WidgetPtr طفل, VoidCallback دالة) {
    return std::make_shared<GestureDetector>(std::move(طفل), std::move(دالة));
}

/// عند_الضغط_المطول — كاشف الضغط المطول
inline WidgetPtr عند_الضغط_المطول(WidgetPtr طفل, VoidCallback دالة) {
    return std::make_shared<GestureDetector>(std::move(طفل), nullptr, nullptr, std::move(دالة));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Constraints — القيود
// ═══════════════════════════════════════════════════════════════════════════════

/// قيود — تطبيق قيود الحجم
inline WidgetPtr قيود(BoxConstraints قيود_الحجم, WidgetPtr طفل) {
    return std::make_shared<ConstrainedBox>(قيود_الحجم, std::move(طفل));
}

/// نسبة_عرض_ارتفاع — الحفاظ على نسبة
inline WidgetPtr نسبة_عرض_ارتفاع(float نسبة, WidgetPtr طفل = nullptr) {
    return std::make_shared<AspectRatio>(نسبة, std::move(طفل));
}

/// حجم_نسبي — حجم كنسبة من الأب
inline WidgetPtr حجم_نسبي(
    WidgetPtr طفل,
    std::optional<float> نسبة_عرض = std::nullopt,
    std::optional<float> نسبة_ارتفاع = std::nullopt
) {
    return std::make_shared<FractionallySizedBox>(std::move(طفل), نسبة_عرض, نسبة_ارتفاع);
}

} // namespace flutter
} // namespace sad
