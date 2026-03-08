// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_cupertino.h — مكونات iOS بأسلوب Cupertino
//  Cupertino (iOS-style) Widgets
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"
#include "sad_flutter_animation.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Cupertino Colors — ألوان iOS
// ═══════════════════════════════════════════════════════════════════════════════

namespace CupertinoColors {
    // System Colors
    inline const Color systemBlue{0, 122, 255};
    inline const Color systemGreen{52, 199, 89};
    inline const Color systemIndigo{88, 86, 214};
    inline const Color systemOrange{255, 149, 0};
    inline const Color systemPink{255, 45, 85};
    inline const Color systemPurple{175, 82, 222};
    inline const Color systemRed{255, 59, 48};
    inline const Color systemTeal{90, 200, 250};
    inline const Color systemYellow{255, 204, 0};
    
    // Label Colors
    inline const Color label{0, 0, 0};
    inline const Color secondaryLabel{60, 60, 67, 153};
    inline const Color tertiaryLabel{60, 60, 67, 76};
    inline const Color quaternaryLabel{60, 60, 67, 45};
    
    // Fill Colors
    inline const Color systemFill{120, 120, 128, 51};
    inline const Color secondarySystemFill{120, 120, 128, 40};
    inline const Color tertiarySystemFill{118, 118, 128, 30};
    inline const Color quaternarySystemFill{116, 116, 128, 20};
    
    // Background Colors
    inline const Color systemBackground{255, 255, 255};
    inline const Color secondarySystemBackground{242, 242, 247};
    inline const Color tertiarySystemBackground{255, 255, 255};
    
    // Separator
    inline const Color separator{60, 60, 67, 73};
    inline const Color opaqueSeparator{198, 198, 200};
    
    // Active Blue
    inline const Color activeBlue{0, 122, 255};
    inline const Color activeGreen{52, 199, 89};
    inline const Color activeOrange{255, 149, 0};
    
    // Dark Mode Colors
    namespace dark {
        inline const Color systemBackground{0, 0, 0};
        inline const Color secondarySystemBackground{28, 28, 30};
        inline const Color label{255, 255, 255};
        inline const Color secondaryLabel{235, 235, 245, 153};
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoThemeData — بيانات ثيم iOS
// ═══════════════════════════════════════════════════════════════════════════════

struct CupertinoThemeData {
    Color primaryColor = CupertinoColors::activeBlue;
    Color primaryContrastingColor = Color(255, 255, 255);
    Color barBackgroundColor = Color(249, 249, 249, 242);
    Color scaffoldBackgroundColor = CupertinoColors::systemBackground;
    
    TextStyle textStyle = TextStyle{
        .fontSize = 17,
        .color = CupertinoColors::label
    };
    
    TextStyle navTitleTextStyle = TextStyle{
        .fontSize = 17,
        .fontWeight = FontWeight::W600,
        .color = CupertinoColors::label
    };
    
    TextStyle navLargeTitleTextStyle = TextStyle{
        .fontSize = 34,
        .fontWeight = FontWeight::Bold,
        .color = CupertinoColors::label
    };
    
    Brightness brightness = Brightness::Light;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoButton — زر iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoButton : public StatefulWidget {
public:
    CupertinoButton(
        WidgetPtr child,
        std::function<void()> onPressed,
        Color color = Color::transparent(),
        Color disabledColor = CupertinoColors::quaternarySystemFill,
        EdgeInsets padding = EdgeInsets::symmetric(16, 10),
        BorderRadius borderRadius = BorderRadius::all(8),
        float minSize = 44.0f,
        float pressedOpacity = 0.4f,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , onPressed_(std::move(onPressed))
        , color_(color)
        , disabledColor_(disabledColor)
        , padding_(padding)
        , borderRadius_(borderRadius)
        , minSize_(minSize)
        , pressedOpacity_(pressedOpacity)
    {}
    
    /// زر مملوء
    static std::shared_ptr<CupertinoButton> filled(
        WidgetPtr child,
        std::function<void()> onPressed,
        EdgeInsets padding = EdgeInsets::symmetric(16, 14),
        Key key = Key()
    ) {
        return std::make_shared<CupertinoButton>(
            std::move(child),
            std::move(onPressed),
            CupertinoColors::activeBlue,
            CupertinoColors::quaternarySystemFill,
            padding,
            BorderRadius::all(8),
            44.0f,
            0.4f,
            std::move(key)
        );
    }
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "CupertinoButton"; }
    
private:
    WidgetPtr child_;
    std::function<void()> onPressed_;
    Color color_, disabledColor_;
    EdgeInsets padding_;
    BorderRadius borderRadius_;
    float minSize_, pressedOpacity_;
    
    friend class CupertinoButtonState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoSwitch — مفتاح iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoSwitch : public StatelessWidget {
public:
    CupertinoSwitch(
        bool value,
        std::function<void(bool)> onChanged,
        Color activeColor = CupertinoColors::activeGreen,
        Color trackColor = Color::transparent(),
        Color thumbColor = Color(255, 255, 255),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , activeColor_(activeColor)
        , trackColor_(trackColor)
        , thumbColor_(thumbColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoSwitch"; }
    
private:
    bool value_;
    std::function<void(bool)> onChanged_;
    Color activeColor_, trackColor_, thumbColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoSlider — منزلق iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoSlider : public StatelessWidget {
public:
    CupertinoSlider(
        double value,
        std::function<void(double)> onChanged,
        double min = 0.0,
        double max = 1.0,
        int divisions = 0,
        Color activeColor = CupertinoColors::activeBlue,
        Color thumbColor = Color(255, 255, 255),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , min_(min), max_(max)
        , divisions_(divisions)
        , activeColor_(activeColor)
        , thumbColor_(thumbColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoSlider"; }
    
private:
    double value_, min_, max_;
    std::function<void(double)> onChanged_;
    int divisions_;
    Color activeColor_, thumbColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoTextField — حقل نص iOS
// ═══════════════════════════════════════════════════════════════════════════════

enum class OverlayVisibilityMode {
    Never,
    Editing,
    NotEditing,
    Always
};

class CupertinoTextField : public StatefulWidget {
public:
    CupertinoTextField(
        std::shared_ptr<TextEditingController> controller = nullptr,
        std::string placeholder = "",
        WidgetPtr prefix = nullptr,
        WidgetPtr suffix = nullptr,
        OverlayVisibilityMode clearButtonMode = OverlayVisibilityMode::Never,
        std::function<void(const std::string&)> onChanged = nullptr,
        std::function<void(const std::string&)> onSubmitted = nullptr,
        TextInputType keyboardType = TextInputType::Text,
        bool obscureText = false,
        bool autofocus = false,
        int maxLines = 1,
        EdgeInsets padding = EdgeInsets::all(6),
        Color backgroundColor = CupertinoColors::systemBackground,
        BorderRadius borderRadius = BorderRadius::all(5),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , controller_(std::move(controller))
        , placeholder_(std::move(placeholder))
        , prefix_(std::move(prefix))
        , suffix_(std::move(suffix))
        , clearButtonMode_(clearButtonMode)
        , onChanged_(std::move(onChanged))
        , onSubmitted_(std::move(onSubmitted))
        , keyboardType_(keyboardType)
        , obscureText_(obscureText)
        , autofocus_(autofocus)
        , maxLines_(maxLines)
        , padding_(padding)
        , backgroundColor_(backgroundColor)
        , borderRadius_(borderRadius)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "CupertinoTextField"; }
    
private:
    std::shared_ptr<TextEditingController> controller_;
    std::string placeholder_;
    WidgetPtr prefix_, suffix_;
    OverlayVisibilityMode clearButtonMode_;
    std::function<void(const std::string&)> onChanged_;
    std::function<void(const std::string&)> onSubmitted_;
    TextInputType keyboardType_;
    bool obscureText_, autofocus_;
    int maxLines_;
    EdgeInsets padding_;
    Color backgroundColor_;
    BorderRadius borderRadius_;
    
    friend class CupertinoTextFieldState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoNavigationBar — شريط تنقل iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoNavigationBar : public StatelessWidget {
public:
    CupertinoNavigationBar(
        WidgetPtr middle = nullptr,
        WidgetPtr leading = nullptr,
        WidgetPtr trailing = nullptr,
        std::string previousPageTitle = "",
        bool automaticallyImplyLeading = true,
        bool automaticallyImplyMiddle = true,
        Color backgroundColor = Color(249, 249, 249, 242),
        Color border = CupertinoColors::separator,
        EdgeInsets padding = EdgeInsets::symmetric(8, 0),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , middle_(std::move(middle))
        , leading_(std::move(leading))
        , trailing_(std::move(trailing))
        , previousPageTitle_(std::move(previousPageTitle))
        , automaticallyImplyLeading_(automaticallyImplyLeading)
        , automaticallyImplyMiddle_(automaticallyImplyMiddle)
        , backgroundColor_(backgroundColor)
        , border_(border)
        , padding_(padding)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoNavigationBar"; }
    
private:
    WidgetPtr middle_, leading_, trailing_;
    std::string previousPageTitle_;
    bool automaticallyImplyLeading_, automaticallyImplyMiddle_;
    Color backgroundColor_, border_;
    EdgeInsets padding_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoSliverNavigationBar — شريط تنقل كبير
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoSliverNavigationBar : public StatelessWidget {
public:
    CupertinoSliverNavigationBar(
        std::string largeTitle,
        WidgetPtr leading = nullptr,
        WidgetPtr trailing = nullptr,
        WidgetPtr middle = nullptr,
        std::string previousPageTitle = "",
        Color backgroundColor = Color(249, 249, 249, 242),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , largeTitle_(std::move(largeTitle))
        , leading_(std::move(leading))
        , trailing_(std::move(trailing))
        , middle_(std::move(middle))
        , previousPageTitle_(std::move(previousPageTitle))
        , backgroundColor_(backgroundColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoSliverNavigationBar"; }
    
private:
    std::string largeTitle_;
    WidgetPtr leading_, trailing_, middle_;
    std::string previousPageTitle_;
    Color backgroundColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoTabBar — شريط تبويبات iOS
// ═══════════════════════════════════════════════════════════════════════════════

struct BottomNavigationBarItem {
    WidgetPtr icon;
    WidgetPtr activeIcon;
    std::string label;
    Color backgroundColor;
    
    BottomNavigationBarItem(
        WidgetPtr i,
        const std::string& l,
        WidgetPtr ai = nullptr,
        Color bg = Color::transparent()
    )
        : icon(std::move(i))
        , activeIcon(std::move(ai))
        , label(l)
        , backgroundColor(bg)
    {}
};

class CupertinoTabBar : public StatelessWidget {
public:
    CupertinoTabBar(
        std::vector<BottomNavigationBarItem> items,
        int currentIndex = 0,
        std::function<void(int)> onTap = nullptr,
        Color backgroundColor = Color(249, 249, 249, 242),
        Color activeColor = CupertinoColors::activeBlue,
        Color inactiveColor = CupertinoColors::secondaryLabel,
        float iconSize = 30.0f,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , items_(std::move(items))
        , currentIndex_(currentIndex)
        , onTap_(std::move(onTap))
        , backgroundColor_(backgroundColor)
        , activeColor_(activeColor)
        , inactiveColor_(inactiveColor)
        , iconSize_(iconSize)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoTabBar"; }
    
private:
    std::vector<BottomNavigationBarItem> items_;
    int currentIndex_;
    std::function<void(int)> onTap_;
    Color backgroundColor_, activeColor_, inactiveColor_;
    float iconSize_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoTabScaffold — هيكل تبويبات iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoTabScaffold : public StatefulWidget {
public:
    CupertinoTabScaffold(
        std::shared_ptr<CupertinoTabBar> tabBar,
        std::function<WidgetPtr(BuildContext*, int)> tabBuilder,
        Color backgroundColor = CupertinoColors::systemBackground,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , tabBar_(std::move(tabBar))
        , tabBuilder_(std::move(tabBuilder))
        , backgroundColor_(backgroundColor)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "CupertinoTabScaffold"; }
    
private:
    std::shared_ptr<CupertinoTabBar> tabBar_;
    std::function<WidgetPtr(BuildContext*, int)> tabBuilder_;
    Color backgroundColor_;
    
    friend class CupertinoTabScaffoldState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoPageScaffold — هيكل صفحة iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoPageScaffold : public StatelessWidget {
public:
    CupertinoPageScaffold(
        WidgetPtr child,
        std::shared_ptr<CupertinoNavigationBar> navigationBar = nullptr,
        Color backgroundColor = CupertinoColors::systemBackground,
        bool resizeToAvoidBottomInset = true,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , navigationBar_(std::move(navigationBar))
        , backgroundColor_(backgroundColor)
        , resizeToAvoidBottomInset_(resizeToAvoidBottomInset)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoPageScaffold"; }
    
private:
    WidgetPtr child_;
    std::shared_ptr<CupertinoNavigationBar> navigationBar_;
    Color backgroundColor_;
    bool resizeToAvoidBottomInset_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoAlertDialog — حوار تنبيه iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoDialogAction : public StatelessWidget {
public:
    CupertinoDialogAction(
        WidgetPtr child,
        std::function<void()> onPressed = nullptr,
        bool isDefaultAction = false,
        bool isDestructiveAction = false,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , onPressed_(std::move(onPressed))
        , isDefaultAction_(isDefaultAction)
        , isDestructiveAction_(isDestructiveAction)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoDialogAction"; }
    
private:
    WidgetPtr child_;
    std::function<void()> onPressed_;
    bool isDefaultAction_, isDestructiveAction_;
};

class CupertinoAlertDialog : public StatelessWidget {
public:
    CupertinoAlertDialog(
        WidgetPtr title = nullptr,
        WidgetPtr content = nullptr,
        std::vector<WidgetPtr> actions = {},
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , title_(std::move(title))
        , content_(std::move(content))
        , actions_(std::move(actions))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoAlertDialog"; }
    
private:
    WidgetPtr title_, content_;
    std::vector<WidgetPtr> actions_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoActionSheet — ورقة إجراءات iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoActionSheetAction : public StatelessWidget {
public:
    CupertinoActionSheetAction(
        WidgetPtr child,
        std::function<void()> onPressed,
        bool isDefaultAction = false,
        bool isDestructiveAction = false,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , onPressed_(std::move(onPressed))
        , isDefaultAction_(isDefaultAction)
        , isDestructiveAction_(isDestructiveAction)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoActionSheetAction"; }
    
private:
    WidgetPtr child_;
    std::function<void()> onPressed_;
    bool isDefaultAction_, isDestructiveAction_;
};

class CupertinoActionSheet : public StatelessWidget {
public:
    CupertinoActionSheet(
        WidgetPtr title = nullptr,
        WidgetPtr message = nullptr,
        std::vector<WidgetPtr> actions = {},
        WidgetPtr cancelButton = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , title_(std::move(title))
        , message_(std::move(message))
        , actions_(std::move(actions))
        , cancelButton_(std::move(cancelButton))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoActionSheet"; }
    
private:
    WidgetPtr title_, message_;
    std::vector<WidgetPtr> actions_;
    WidgetPtr cancelButton_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoPicker — منتقي iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoPicker : public StatefulWidget {
public:
    CupertinoPicker(
        std::vector<WidgetPtr> children,
        std::function<void(int)> onSelectedItemChanged,
        float itemExtent = 32.0f,
        float diameterRatio = 1.07f,
        Color backgroundColor = Color::transparent(),
        float squeeze = 1.45f,
        int initialItem = 0,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , children_(std::move(children))
        , onSelectedItemChanged_(std::move(onSelectedItemChanged))
        , itemExtent_(itemExtent)
        , diameterRatio_(diameterRatio)
        , backgroundColor_(backgroundColor)
        , squeeze_(squeeze)
        , initialItem_(initialItem)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "CupertinoPicker"; }
    
private:
    std::vector<WidgetPtr> children_;
    std::function<void(int)> onSelectedItemChanged_;
    float itemExtent_, diameterRatio_;
    Color backgroundColor_;
    float squeeze_;
    int initialItem_;
    
    friend class CupertinoPickerState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoDatePicker — منتقي تاريخ iOS
// ═══════════════════════════════════════════════════════════════════════════════

enum class CupertinoDatePickerMode {
    Time,
    Date,
    DateAndTime
};

class CupertinoDatePicker : public StatefulWidget {
public:
    CupertinoDatePicker(
        std::function<void(Date)> onDateTimeChanged,
        CupertinoDatePickerMode mode = CupertinoDatePickerMode::DateAndTime,
        Date initialDateTime = Date{2024, 1, 1},
        Date minimumDate = Date{1900, 1, 1},
        Date maximumDate = Date{2100, 12, 31},
        int minuteInterval = 1,
        bool use24hFormat = true,
        Color backgroundColor = Color::transparent(),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , onDateTimeChanged_(std::move(onDateTimeChanged))
        , mode_(mode)
        , initialDateTime_(initialDateTime)
        , minimumDate_(minimumDate)
        , maximumDate_(maximumDate)
        , minuteInterval_(minuteInterval)
        , use24hFormat_(use24hFormat)
        , backgroundColor_(backgroundColor)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "CupertinoDatePicker"; }
    
private:
    std::function<void(Date)> onDateTimeChanged_;
    CupertinoDatePickerMode mode_;
    Date initialDateTime_, minimumDate_, maximumDate_;
    int minuteInterval_;
    bool use24hFormat_;
    Color backgroundColor_;
    
    friend class CupertinoDatePickerState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoActivityIndicator — مؤشر نشاط iOS
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoActivityIndicator : public StatelessWidget {
public:
    CupertinoActivityIndicator(
        bool animating = true,
        float radius = 10.0f,
        Color color = CupertinoColors::secondaryLabel,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , animating_(animating)
        , radius_(radius)
        , color_(color)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CupertinoActivityIndicator"; }
    
private:
    bool animating_;
    float radius_;
    Color color_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Aliases — أسماء عربية
// ═══════════════════════════════════════════════════════════════════════════════

using زر_كوبرتينو = CupertinoButton;
using مفتاح_كوبرتينو = CupertinoSwitch;
using منزلق_كوبرتينو = CupertinoSlider;
using حقل_نص_كوبرتينو = CupertinoTextField;
using شريط_تنقل_كوبرتينو = CupertinoNavigationBar;
using شريط_تبويب_كوبرتينو = CupertinoTabBar;
using هيكل_كوبرتينو = CupertinoPageScaffold;
using حوار_كوبرتينو = CupertinoAlertDialog;
using اجراء_حوار_كوبرتينو = CupertinoDialogAction;
using ورقة_اجراءات_كوبرتينو = CupertinoActionSheet;
using منتقي_كوبرتينو = CupertinoPicker;
using منتقي_تاريخ_كوبرتينو = CupertinoDatePicker;
using مؤشر_نشاط_كوبرتينو = CupertinoActivityIndicator;

// ألوان كوبرتينو عربية
namespace الوان_كوبرتينو {
    inline const auto& ازرق = CupertinoColors::systemBlue;
    inline const auto& اخضر = CupertinoColors::systemGreen;
    inline const auto& احمر = CupertinoColors::systemRed;
    inline const auto& برتقالي = CupertinoColors::systemOrange;
    inline const auto& اصفر = CupertinoColors::systemYellow;
    inline const auto& بنفسجي = CupertinoColors::systemPurple;
    inline const auto& وردي = CupertinoColors::systemPink;
}

// دوال مساعدة
inline WidgetPtr زر_ios(
    const std::string& نص,
    std::function<void()> عند_الضغط,
    bool مملوء = false
) {
    auto طفل = std::make_shared<Text>(نص);
    
    if (مملوء) {
        return CupertinoButton::filled(std::move(طفل), std::move(عند_الضغط));
    }
    
    return std::make_shared<CupertinoButton>(
        std::move(طفل),
        std::move(عند_الضغط),
        CupertinoColors::activeBlue
    );
}

inline WidgetPtr حقل_نص_ios(
    const std::string& تلميح = "",
    std::function<void(const std::string&)> عند_التغيير = nullptr,
    bool كلمة_سر = false
) {
    return std::make_shared<CupertinoTextField>(
        nullptr,
        تلميح,
        nullptr, nullptr,
        OverlayVisibilityMode::Editing,
        std::move(عند_التغيير),
        nullptr,
        TextInputType::Text,
        كلمة_سر
    );
}

} // namespace flutter
} // namespace sad
