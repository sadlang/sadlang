// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_theme.h — نظام الثيمات
//  Theme System
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_THEME_H
#define SAD_FLUTTER_THEME_H

#include "sad_flutter_core.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  ColorScheme — مخطط الألوان
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) السطوع
 * @brief (EN) Brightness
 */
enum class Brightness {
    Light,
    Dark
};

using سطوع = Brightness;

/**
 * @brief (AR) مخطط الألوان
 * @brief (EN) Color scheme
 */
struct ColorScheme {
    Brightness brightness = Brightness::Light;
    
    Color primary = Color(103, 80, 164);          // اللون الأساسي
    Color onPrimary = Color(255, 255, 255);       // على الأساسي
    Color primaryContainer = Color(234, 221, 255);
    Color onPrimaryContainer = Color(33, 0, 93);
    
    Color secondary = Color(98, 91, 113);         // اللون الثانوي
    Color onSecondary = Color(255, 255, 255);
    Color secondaryContainer = Color(232, 222, 248);
    Color onSecondaryContainer = Color(29, 25, 43);
    
    Color tertiary = Color(125, 82, 96);          // اللون الثالثي
    Color onTertiary = Color(255, 255, 255);
    Color tertiaryContainer = Color(255, 216, 228);
    Color onTertiaryContainer = Color(49, 17, 29);
    
    Color error = Color(179, 38, 30);             // لون الخطأ
    Color onError = Color(255, 255, 255);
    Color errorContainer = Color(249, 222, 220);
    Color onErrorContainer = Color(65, 14, 11);
    
    Color background = Color(255, 251, 254);      // الخلفية
    Color onBackground = Color(28, 27, 31);
    
    Color surface = Color(255, 251, 254);         // السطح
    Color onSurface = Color(28, 27, 31);
    Color surfaceVariant = Color(231, 224, 236);
    Color onSurfaceVariant = Color(73, 69, 79);
    
    Color outline = Color(121, 116, 126);         // الحدود
    Color outlineVariant = Color(202, 196, 208);
    
    Color shadow = Color(0, 0, 0);                // الظل
    Color scrim = Color(0, 0, 0);
    
    Color inverseSurface = Color(49, 48, 51);
    Color onInverseSurface = Color(244, 239, 244);
    Color inversePrimary = Color(208, 188, 255);
    
    Color surfaceTint = Color(103, 80, 164);
    
    // مصنع فاتح
    static ColorScheme light(Color seedColor = Color(103, 80, 164)) {
        ColorScheme scheme;
        scheme.brightness = Brightness::Light;
        scheme.primary = seedColor;
        return scheme;
    }
    
    // مصنع داكن
    static ColorScheme dark(Color seedColor = Color(103, 80, 164)) {
        ColorScheme scheme;
        scheme.brightness = Brightness::Dark;
        scheme.primary = seedColor;
        scheme.background = Color(28, 27, 31);
        scheme.onBackground = Color(230, 225, 229);
        scheme.surface = Color(28, 27, 31);
        scheme.onSurface = Color(230, 225, 229);
        return scheme;
    }
};

using مخطط_الوان = ColorScheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  TextTheme — ثيم النصوص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم النصوص
 * @brief (EN) Text theme
 */
struct TextTheme {
    TextStyle displayLarge = TextStyle{.fontSize = 57, .fontWeight = FontWeight::Normal};
    TextStyle displayMedium = TextStyle{.fontSize = 45, .fontWeight = FontWeight::Normal};
    TextStyle displaySmall = TextStyle{.fontSize = 36, .fontWeight = FontWeight::Normal};
    
    TextStyle headlineLarge = TextStyle{.fontSize = 32, .fontWeight = FontWeight::Normal};
    TextStyle headlineMedium = TextStyle{.fontSize = 28, .fontWeight = FontWeight::Normal};
    TextStyle headlineSmall = TextStyle{.fontSize = 24, .fontWeight = FontWeight::Normal};
    
    TextStyle titleLarge = TextStyle{.fontSize = 22, .fontWeight = FontWeight::W500};
    TextStyle titleMedium = TextStyle{.fontSize = 16, .fontWeight = FontWeight::W500};
    TextStyle titleSmall = TextStyle{.fontSize = 14, .fontWeight = FontWeight::W500};
    
    TextStyle bodyLarge = TextStyle{.fontSize = 16, .fontWeight = FontWeight::Normal};
    TextStyle bodyMedium = TextStyle{.fontSize = 14, .fontWeight = FontWeight::Normal};
    TextStyle bodySmall = TextStyle{.fontSize = 12, .fontWeight = FontWeight::Normal};
    
    TextStyle labelLarge = TextStyle{.fontSize = 14, .fontWeight = FontWeight::W500};
    TextStyle labelMedium = TextStyle{.fontSize = 12, .fontWeight = FontWeight::W500};
    TextStyle labelSmall = TextStyle{.fontSize = 11, .fontWeight = FontWeight::W500};
    
    // تطبيق لون على جميع الأنماط
    TextTheme apply(Color color) const {
        TextTheme copy = *this;
        copy.displayLarge.color = color;
        copy.displayMedium.color = color;
        copy.displaySmall.color = color;
        copy.headlineLarge.color = color;
        copy.headlineMedium.color = color;
        copy.headlineSmall.color = color;
        copy.titleLarge.color = color;
        copy.titleMedium.color = color;
        copy.titleSmall.color = color;
        copy.bodyLarge.color = color;
        copy.bodyMedium.color = color;
        copy.bodySmall.color = color;
        copy.labelLarge.color = color;
        copy.labelMedium.color = color;
        copy.labelSmall.color = color;
        return copy;
    }
    
    // دمج مع ثيم آخر
    TextTheme merge(const TextTheme& other) const {
        TextTheme merged = *this;
        // TODO: دمج الخصائص
        return merged;
    }
};

using ثيم_نصوص = TextTheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  AppBarTheme — ثيم شريط التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم شريط التطبيق
 * @brief (EN) App bar theme
 */
struct AppBarTheme {
    Color backgroundColor = Color(0, 0, 0, 0);
    Color foregroundColor = Color(0, 0, 0);
    float elevation = 4.0f;
    Color shadowColor = Color(0, 0, 0, 64);
    Color surfaceTintColor = Color(0, 0, 0, 0);
    ShapeBorder shape = ShapeBorder{};
    IconThemeData iconTheme = IconThemeData{};
    IconThemeData actionsIconTheme = IconThemeData{};
    bool centerTitle = false;
    float titleSpacing = 16.0f;
    float toolbarHeight = 56.0f;
    TextStyle titleTextStyle = TextStyle{.fontSize = 20, .fontWeight = FontWeight::W500};
};

using ثيم_شريط_تطبيق = AppBarTheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  ButtonTheme — ثيم الأزرار
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نمط الزر
 * @brief (EN) Button style
 */
struct ButtonStyle {
    Color backgroundColor = Color(0, 0, 0, 0);
    Color foregroundColor = Color(0, 0, 0);
    Color overlayColor = Color(0, 0, 0, 20);
    Color shadowColor = Color(0, 0, 0, 64);
    Color surfaceTintColor = Color(0, 0, 0, 0);
    float elevation = 2.0f;
    EdgeInsets padding = EdgeInsets::symmetric(24, 0);
    Size minimumSize = Size{64, 36};
    Size maximumSize = Size{std::numeric_limits<float>::infinity(), 
                            std::numeric_limits<float>::infinity()};
    BorderSide side = BorderSide{};
    ShapeBorder shape = ShapeBorder{BorderRadius::all(4)};
    TextStyle textStyle = TextStyle{.fontSize = 14, .fontWeight = FontWeight::W500};
    AlignmentGeometry alignment = AlignmentGeometry{0, 0};
    Duration animationDuration = Duration::milliseconds(200);
    bool enableFeedback = true;
};

using نمط_زر = ButtonStyle;

/**
 * @brief (AR) ثيم الأزرار المرتفعة
 * @brief (EN) Elevated button theme
 */
struct ElevatedButtonThemeData {
    ButtonStyle style;
};

using ثيم_زر_مرتفع = ElevatedButtonThemeData;

/**
 * @brief (AR) ثيم الأزرار النصية
 * @brief (EN) Text button theme
 */
struct TextButtonThemeData {
    ButtonStyle style;
};

using ثيم_زر_نصي = TextButtonThemeData;

/**
 * @brief (AR) ثيم الأزرار المحددة
 * @brief (EN) Outlined button theme
 */
struct OutlinedButtonThemeData {
    ButtonStyle style;
};

using ثيم_زر_محدد = OutlinedButtonThemeData;

/**
 * @brief (AR) ثيم FAB
 * @brief (EN) Floating action button theme
 */
struct FloatingActionButtonThemeData {
    Color backgroundColor = Color(0, 0, 0, 0);
    Color foregroundColor = Color(0, 0, 0);
    float elevation = 6.0f;
    float focusElevation = 6.0f;
    float hoverElevation = 8.0f;
    float highlightElevation = 12.0f;
    ShapeBorder shape = ShapeBorder{BorderRadius::all(16)};
    bool enableFeedback = true;
    float sizeConstraint = 56.0f;
    float smallSizeConstraint = 40.0f;
    float largeSizeConstraint = 96.0f;
    float extendedSizeConstraint = 48.0f;
    EdgeInsets extendedPadding = EdgeInsets::symmetric(20, 0);
    TextStyle extendedTextStyle = TextStyle{.fontSize = 14, .fontWeight = FontWeight::W500};
    float extendedIconLabelSpacing = 8.0f;
};

using ثيم_زر_عائم = FloatingActionButtonThemeData;

// ═══════════════════════════════════════════════════════════════════════════════
//  InputDecorationTheme — ثيم حقول الإدخال
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نوع حدود الإدخال
 * @brief (EN) Input border type
 */
enum class InputBorderType {
    None,
    Outline,
    Underline
};

/**
 * @brief (AR) ثيم تزيين الإدخال
 * @brief (EN) Input decoration theme
 */
struct InputDecorationTheme {
    TextStyle labelStyle = TextStyle{.fontSize = 16};
    TextStyle floatingLabelStyle = TextStyle{.fontSize = 12};
    TextStyle helperStyle = TextStyle{.fontSize = 12};
    int helperMaxLines = 1;
    TextStyle hintStyle = TextStyle{.fontSize = 16, .color = Color(128, 128, 128)};
    Duration hintFadeDuration = Duration::milliseconds(200);
    TextStyle errorStyle = TextStyle{.fontSize = 12, .color = Color(179, 38, 30)};
    int errorMaxLines = 1;
    bool hasFloatingPlaceholder = true;
    Color fillColor = Color(0, 0, 0, 0);
    Color focusColor = Color(103, 80, 164);
    Color hoverColor = Color(0, 0, 0, 10);
    Color errorBorderColor = Color(179, 38, 30);
    Color focusedBorderColor = Color(103, 80, 164);
    Color focusedErrorBorderColor = Color(179, 38, 30);
    Color disabledBorderColor = Color(200, 200, 200);
    Color enabledBorderColor = Color(150, 150, 150);
    InputBorderType borderType = InputBorderType::Underline;
    float borderWidth = 1.0f;
    BorderRadius borderRadius = BorderRadius::all(4);
    EdgeInsets contentPadding = EdgeInsets::symmetric(12, 16);
    bool isDense = false;
    bool isCollapsed = false;
    Color iconColor = Color(0, 0, 0, 138);
    Color prefixIconColor = Color(0, 0, 0, 138);
    Color suffixIconColor = Color(0, 0, 0, 138);
    TextStyle prefixStyle = TextStyle{.fontSize = 16};
    TextStyle suffixStyle = TextStyle{.fontSize = 16};
    TextStyle counterStyle = TextStyle{.fontSize = 12};
    bool filled = false;
    bool alignLabelWithHint = false;
};

using ثيم_ادخال = InputDecorationTheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  CardTheme — ثيم البطاقات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم البطاقات
 * @brief (EN) Card theme
 */
struct CardTheme {
    Color color = Color(255, 255, 255);
    Color shadowColor = Color(0, 0, 0, 64);
    Color surfaceTintColor = Color(0, 0, 0, 0);
    float elevation = 1.0f;
    ShapeBorder shape = ShapeBorder{BorderRadius::all(12)};
    EdgeInsets margin = EdgeInsets{};
    bool clipBehavior = true;
};

using ثيم_بطاقة = CardTheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  DialogTheme — ثيم الحوارات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم الحوارات
 * @brief (EN) Dialog theme
 */
struct DialogTheme {
    Color backgroundColor = Color(255, 255, 255);
    float elevation = 24.0f;
    Color shadowColor = Color(0, 0, 0, 64);
    Color surfaceTintColor = Color(0, 0, 0, 0);
    ShapeBorder shape = ShapeBorder{BorderRadius::all(28)};
    AlignmentGeometry alignment = AlignmentGeometry{0, 0};
    Color iconColor = Color(0, 0, 0);
    TextStyle titleTextStyle = TextStyle{.fontSize = 24, .fontWeight = FontWeight::Normal};
    TextStyle contentTextStyle = TextStyle{.fontSize = 14};
    EdgeInsets actionsPadding = EdgeInsets::fromLTRB(24, 0, 24, 24);
    EdgeInsets insetPadding = EdgeInsets::symmetric(40, 24);
};

using ثيم_حوار = DialogTheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  BottomSheetTheme — ثيم الورقة السفلية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم الورقة السفلية
 * @brief (EN) Bottom sheet theme
 */
struct BottomSheetThemeData {
    Color backgroundColor = Color(255, 255, 255);
    Color surfaceTintColor = Color(0, 0, 0, 0);
    float elevation = 1.0f;
    Color modalBackgroundColor = Color(255, 255, 255);
    float modalElevation = 1.0f;
    ShapeBorder shape = ShapeBorder{BorderRadius::only(12, 12, 0, 0)};
    Color dragHandleColor = Color(128, 128, 128);
    Size dragHandleSize = Size{32, 4};
};

using ثيم_ورقة_سفلية = BottomSheetThemeData;

// ═══════════════════════════════════════════════════════════════════════════════
//  NavigationBarTheme — ثيم شريط التنقل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم شريط التنقل
 * @brief (EN) Navigation bar theme
 */
struct NavigationBarThemeData {
    float height = 80.0f;
    Color backgroundColor = Color(0, 0, 0, 0);
    float elevation = 3.0f;
    Color shadowColor = Color(0, 0, 0, 64);
    Color surfaceTintColor = Color(0, 0, 0, 0);
    Color indicatorColor = Color(103, 80, 164, 50);
    ShapeBorder indicatorShape = ShapeBorder{BorderRadius::all(16)};
    TextStyle labelTextStyle = TextStyle{.fontSize = 12};
    IconThemeData iconTheme = IconThemeData{};
    int labelBehavior = 0;  // 0=always, 1=selected, 2=never
};

using ثيم_شريط_تنقل = NavigationBarThemeData;

// ═══════════════════════════════════════════════════════════════════════════════
//  SnackBarTheme — ثيم شريط الإشعار
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم شريط الإشعار
 * @brief (EN) Snack bar theme
 */
struct SnackBarThemeData {
    Color backgroundColor = Color(50, 50, 50);
    Color actionTextColor = Color(100, 149, 237);
    Color disabledActionTextColor = Color(100, 100, 100);
    TextStyle contentTextStyle = TextStyle{.fontSize = 14, .color = Color(255, 255, 255)};
    float elevation = 6.0f;
    ShapeBorder shape = ShapeBorder{BorderRadius::all(4)};
    int behavior = 0;  // 0=fixed, 1=floating
    float width = 0.0f;  // 0 = full width
    EdgeInsets insetPadding = EdgeInsets::symmetric(15, 5);
    Duration showDuration = Duration::seconds(4);
};

using ثيم_شريط_اشعار = SnackBarThemeData;

// ═══════════════════════════════════════════════════════════════════════════════
//  ThemeData — الثيم الكامل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) بيانات الثيم الكاملة
 * @brief (EN) Complete theme data
 */
struct ThemeData {
    Brightness brightness = Brightness::Light;
    bool useMaterial3 = true;
    
    ColorScheme colorScheme;
    
    Color primaryColor = Color(103, 80, 164);
    Color primaryColorLight = Color(234, 221, 255);
    Color primaryColorDark = Color(33, 0, 93);
    
    Color scaffoldBackgroundColor = Color(255, 251, 254);
    Color canvasColor = Color(255, 251, 254);
    Color cardColor = Color(255, 255, 255);
    Color dividerColor = Color(202, 196, 208);
    Color disabledColor = Color(200, 200, 200);
    Color highlightColor = Color(0, 0, 0, 20);
    Color splashColor = Color(0, 0, 0, 30);
    Color hoverColor = Color(0, 0, 0, 10);
    Color focusColor = Color(0, 0, 0, 30);
    Color shadowColor = Color(0, 0, 0);
    Color unselectedWidgetColor = Color(128, 128, 128);
    Color indicatorColor = Color(103, 80, 164);
    Color secondaryHeaderColor = Color(234, 221, 255);
    
    TextTheme textTheme;
    TextTheme primaryTextTheme;
    
    AppBarTheme appBarTheme;
    ElevatedButtonThemeData elevatedButtonTheme;
    TextButtonThemeData textButtonTheme;
    OutlinedButtonThemeData outlinedButtonTheme;
    FloatingActionButtonThemeData floatingActionButtonTheme;
    InputDecorationTheme inputDecorationTheme;
    CardTheme cardTheme;
    DialogTheme dialogTheme;
    BottomSheetThemeData bottomSheetTheme;
    NavigationBarThemeData navigationBarTheme;
    SnackBarThemeData snackBarTheme;
    
    IconThemeData iconTheme = IconThemeData{.color = Color(0, 0, 0), .size = 24};
    IconThemeData primaryIconTheme = IconThemeData{.color = Color(255, 255, 255), .size = 24};
    
    std::string fontFamily = "";
    std::string package = "";
    
    // مصنعات
    static ThemeData light(ColorScheme colorScheme = ColorScheme::light()) {
        ThemeData theme;
        theme.brightness = Brightness::Light;
        theme.colorScheme = colorScheme;
        theme.primaryColor = colorScheme.primary;
        theme.scaffoldBackgroundColor = colorScheme.background;
        return theme;
    }
    
    static ThemeData dark(ColorScheme colorScheme = ColorScheme::dark()) {
        ThemeData theme;
        theme.brightness = Brightness::Dark;
        theme.colorScheme = colorScheme;
        theme.primaryColor = colorScheme.primary;
        theme.scaffoldBackgroundColor = colorScheme.background;
        theme.textTheme = theme.textTheme.apply(Color(255, 255, 255));
        return theme;
    }
    
    ThemeData copyWith() const {
        return *this;
    }
};

using بيانات_ثيم = ThemeData;

// ═══════════════════════════════════════════════════════════════════════════════
//  Theme Widget — مزود الثيم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مزود الثيم
 * @brief (EN) Theme provider widget
 */
class Theme : public InheritedWidget {
public:
    Theme(ThemeData data, WidgetPtr child)
        : InheritedWidget(child), data_(data) {}
    
    const ThemeData& data() const { return data_; }
    
    // الحصول على الثيم من السياق
    static const ThemeData& of(BuildContext* ctx) {
        // TODO: تطبيق البحث في الشجرة
        static ThemeData defaultTheme = ThemeData::light();
        return defaultTheme;
    }
    
    bool updateShouldNotify(const InheritedWidget* oldWidget) const override {
        // TODO: مقارنة عميقة
        return true;
    }
    
private:
    ThemeData data_;
};

using ثيم = Theme;

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedTheme — ثيم متحرك
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) ثيم متحرك
 * @brief (EN) Animated theme
 */
class AnimatedTheme : public StatefulWidget {
public:
    AnimatedTheme(ThemeData data,
                  WidgetPtr child,
                  Duration duration = Duration::milliseconds(200),
                  Curve curve = Curves::ease,
                  std::function<void()> onEnd = nullptr)
        : data_(data), child_(child), duration_(duration),
          curve_(curve), onEnd_(onEnd) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    ThemeData data_;
    WidgetPtr child_;
    Duration duration_;
    Curve curve_;
    std::function<void()> onEnd_;
};

using ثيم_متحرك = AnimatedTheme;

// ═══════════════════════════════════════════════════════════════════════════════
//  ThemeMode — وضع الثيم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) وضع الثيم
 * @brief (EN) Theme mode
 */
enum class ThemeMode {
    System,  // حسب النظام
    Light,   // فاتح
    Dark     // داكن
};

using وضع_ثيم = ThemeMode;

// ═══════════════════════════════════════════════════════════════════════════════
//  Material Colors — ألوان Material
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) درجات اللون
 * @brief (EN) Color swatch
 */
struct MaterialColor {
    Color shade50;
    Color shade100;
    Color shade200;
    Color shade300;
    Color shade400;
    Color shade500;  // الأساسي
    Color shade600;
    Color shade700;
    Color shade800;
    Color shade900;
    
    Color primary() const { return shade500; }
    Color light() const { return shade100; }
    Color dark() const { return shade700; }
    
    Color operator[](int shade) const {
        switch (shade) {
            case 50: return shade50;
            case 100: return shade100;
            case 200: return shade200;
            case 300: return shade300;
            case 400: return shade400;
            case 500: return shade500;
            case 600: return shade600;
            case 700: return shade700;
            case 800: return shade800;
            case 900: return shade900;
            default: return shade500;
        }
    }
};

using لون_مادي = MaterialColor;

/**
 * @brief (AR) ألوان Material المعيارية
 * @brief (EN) Standard Material colors
 */
namespace Colors {
    // أحمر
    inline const MaterialColor red = {
        Color(255, 235, 238), Color(255, 205, 210), Color(239, 154, 154),
        Color(229, 115, 115), Color(239, 83, 80),   Color(244, 67, 54),
        Color(229, 57, 53),   Color(211, 47, 47),   Color(198, 40, 40),
        Color(183, 28, 28)
    };
    
    // وردي
    inline const MaterialColor pink = {
        Color(252, 228, 236), Color(248, 187, 208), Color(244, 143, 177),
        Color(240, 98, 146),  Color(236, 64, 122),  Color(233, 30, 99),
        Color(216, 27, 96),   Color(194, 24, 91),   Color(173, 20, 87),
        Color(136, 14, 79)
    };
    
    // بنفسجي
    inline const MaterialColor purple = {
        Color(243, 229, 245), Color(225, 190, 231), Color(206, 147, 216),
        Color(186, 104, 200), Color(171, 71, 188),  Color(156, 39, 176),
        Color(142, 36, 170),  Color(123, 31, 162),  Color(106, 27, 154),
        Color(74, 20, 140)
    };
    
    // أزرق
    inline const MaterialColor blue = {
        Color(227, 242, 253), Color(187, 222, 251), Color(144, 202, 249),
        Color(100, 181, 246), Color(66, 165, 245),  Color(33, 150, 243),
        Color(30, 136, 229),  Color(25, 118, 210),  Color(21, 101, 192),
        Color(13, 71, 161)
    };
    
    // أخضر
    inline const MaterialColor green = {
        Color(232, 245, 233), Color(200, 230, 201), Color(165, 214, 167),
        Color(129, 199, 132), Color(102, 187, 106), Color(76, 175, 80),
        Color(67, 160, 71),   Color(56, 142, 60),   Color(46, 125, 50),
        Color(27, 94, 32)
    };
    
    // برتقالي
    inline const MaterialColor orange = {
        Color(255, 243, 224), Color(255, 224, 178), Color(255, 204, 128),
        Color(255, 183, 77),  Color(255, 167, 38),  Color(255, 152, 0),
        Color(251, 140, 0),   Color(245, 124, 0),   Color(239, 108, 0),
        Color(230, 81, 0)
    };
    
    // رمادي
    inline const MaterialColor grey = {
        Color(250, 250, 250), Color(245, 245, 245), Color(238, 238, 238),
        Color(224, 224, 224), Color(189, 189, 189), Color(158, 158, 158),
        Color(117, 117, 117), Color(97, 97, 97),    Color(66, 66, 66),
        Color(33, 33, 33)
    };
    
    // ألوان أساسية
    inline const Color white = Color(255, 255, 255);
    inline const Color black = Color(0, 0, 0);
    inline const Color transparent = Color(0, 0, 0, 0);
}

// أسماء عربية
namespace الالوان {
    inline const MaterialColor& احمر = Colors::red;
    inline const MaterialColor& وردي = Colors::pink;
    inline const MaterialColor& بنفسجي = Colors::purple;
    inline const MaterialColor& ازرق = Colors::blue;
    inline const MaterialColor& اخضر = Colors::green;
    inline const MaterialColor& برتقالي = Colors::orange;
    inline const MaterialColor& رمادي = Colors::grey;
    inline const Color& ابيض = Colors::white;
    inline const Color& اسود = Colors::black;
    inline const Color& شفاف = Colors::transparent;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء ثيم من لون أساسي
 * @brief (EN) Create theme from seed color
 */
inline ThemeData انشاء_ثيم_من_لون(Color لون_أساسي, bool داكن = false) {
    if (داكن) {
        return ThemeData::dark(ColorScheme::dark(لون_أساسي));
    }
    return ThemeData::light(ColorScheme::light(لون_أساسي));
}

/**
 * @brief (AR) الحصول على الثيم الحالي
 * @brief (EN) Get current theme
 */
inline const ThemeData& الثيم_الحالي(BuildContext* ctx) {
    return Theme::of(ctx);
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_THEME_H
