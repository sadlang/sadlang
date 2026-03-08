// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_dialogs.h — الحوارات والنوافذ المنبثقة بأسلوب Flutter
//  Dialogs, Popups, BottomSheets, SnackBars
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"
#include "sad_flutter_animation.h"
#include <functional>
#include <queue>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Dialog Base — أساس الحوارات
// ═══════════════════════════════════════════════════════════════════════════════

/// حوار أساسي
class Dialog : public StatelessWidget {
public:
    Dialog(
        WidgetPtr child,
        Color backgroundColor = Color(255, 255, 255),
        EdgeInsets insetPadding = EdgeInsets::symmetric(40, 24),
        BorderRadius borderRadius = BorderRadius::all(28),
        float elevation = 6.0f,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , backgroundColor_(backgroundColor)
        , insetPadding_(insetPadding)
        , borderRadius_(borderRadius)
        , elevation_(elevation)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Dialog"; }
    
protected:
    WidgetPtr child_;
    Color backgroundColor_;
    EdgeInsets insetPadding_;
    BorderRadius borderRadius_;
    float elevation_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  AlertDialog — حوار تنبيه
// ═══════════════════════════════════════════════════════════════════════════════

class AlertDialog : public StatelessWidget {
public:
    AlertDialog(
        std::optional<WidgetPtr> title = std::nullopt,
        std::optional<WidgetPtr> content = std::nullopt,
        std::vector<WidgetPtr> actions = {},
        EdgeInsets titlePadding = EdgeInsets::fromLTRB(24, 24, 24, 0),
        EdgeInsets contentPadding = EdgeInsets::fromLTRB(24, 20, 24, 24),
        EdgeInsets actionsPadding = EdgeInsets::fromLTRB(8, 0, 8, 8),
        MainAxisAlignment actionsAlignment = MainAxisAlignment::End,
        Color backgroundColor = Color(255, 255, 255),
        BorderRadius borderRadius = BorderRadius::all(28),
        WidgetPtr icon = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , title_(title)
        , content_(content)
        , actions_(std::move(actions))
        , titlePadding_(titlePadding)
        , contentPadding_(contentPadding)
        , actionsPadding_(actionsPadding)
        , actionsAlignment_(actionsAlignment)
        , backgroundColor_(backgroundColor)
        , borderRadius_(borderRadius)
        , icon_(std::move(icon))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "AlertDialog"; }
    
private:
    std::optional<WidgetPtr> title_, content_;
    std::vector<WidgetPtr> actions_;
    EdgeInsets titlePadding_, contentPadding_, actionsPadding_;
    MainAxisAlignment actionsAlignment_;
    Color backgroundColor_;
    BorderRadius borderRadius_;
    WidgetPtr icon_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  SimpleDialog — حوار بسيط مع خيارات
// ═══════════════════════════════════════════════════════════════════════════════

class SimpleDialogOption : public StatelessWidget {
public:
    SimpleDialogOption(
        WidgetPtr child,
        std::function<void()> onPressed = nullptr,
        EdgeInsets padding = EdgeInsets::symmetric(24, 16),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , onPressed_(std::move(onPressed))
        , padding_(padding)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "SimpleDialogOption"; }
    
private:
    WidgetPtr child_;
    std::function<void()> onPressed_;
    EdgeInsets padding_;
};

class SimpleDialog : public StatelessWidget {
public:
    SimpleDialog(
        std::optional<WidgetPtr> title = std::nullopt,
        std::vector<WidgetPtr> children = {},
        EdgeInsets titlePadding = EdgeInsets::fromLTRB(24, 24, 24, 0),
        EdgeInsets contentPadding = EdgeInsets::fromLTRB(0, 12, 0, 16),
        Color backgroundColor = Color(255, 255, 255),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , title_(title)
        , children_(std::move(children))
        , titlePadding_(titlePadding)
        , contentPadding_(contentPadding)
        , backgroundColor_(backgroundColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "SimpleDialog"; }
    
private:
    std::optional<WidgetPtr> title_;
    std::vector<WidgetPtr> children_;
    EdgeInsets titlePadding_, contentPadding_;
    Color backgroundColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  BottomSheet — ورقة سفلية
// ═══════════════════════════════════════════════════════════════════════════════

class BottomSheet : public StatefulWidget {
public:
    BottomSheet(
        std::function<WidgetPtr(BuildContext*)> builder,
        std::function<void()> onClosing = nullptr,
        Color backgroundColor = Color(255, 255, 255),
        float elevation = 8.0f,
        BorderRadius borderRadius = BorderRadius::vertical(28, 0),
        bool enableDrag = true,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , builder_(std::move(builder))
        , onClosing_(std::move(onClosing))
        , backgroundColor_(backgroundColor)
        , elevation_(elevation)
        , borderRadius_(borderRadius)
        , enableDrag_(enableDrag)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "BottomSheet"; }
    
    const std::function<WidgetPtr(BuildContext*)>& builder() const { return builder_; }
    Color backgroundColor() const { return backgroundColor_; }
    BorderRadius borderRadius() const { return borderRadius_; }
    
private:
    std::function<WidgetPtr(BuildContext*)> builder_;
    std::function<void()> onClosing_;
    Color backgroundColor_;
    float elevation_;
    BorderRadius borderRadius_;
    bool enableDrag_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  SnackBar — شريط الإشعارات
// ═══════════════════════════════════════════════════════════════════════════════

/// سلوك SnackBar
enum class SnackBarBehavior {
    Fixed,   // ثابت في الأسفل
    Floating // عائم مع هامش
};

class SnackBarAction : public StatelessWidget {
public:
    SnackBarAction(
        std::string label,
        std::function<void()> onPressed,
        Color textColor = Color(187, 134, 252), // بنفسجي فاتح
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , label_(std::move(label))
        , onPressed_(std::move(onPressed))
        , textColor_(textColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "SnackBarAction"; }
    
private:
    std::string label_;
    std::function<void()> onPressed_;
    Color textColor_;
};

class SnackBar : public StatelessWidget {
public:
    SnackBar(
        WidgetPtr content,
        Duration duration = Duration::seconds(4),
        WidgetPtr action = nullptr,
        std::function<void()> onVisible = nullptr,
        Color backgroundColor = Color(50, 50, 50),
        SnackBarBehavior behavior = SnackBarBehavior::Fixed,
        EdgeInsets margin = EdgeInsets::all(8),
        EdgeInsets padding = EdgeInsets::symmetric(16, 14),
        BorderRadius borderRadius = BorderRadius::all(4),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , content_(std::move(content))
        , duration_(duration)
        , action_(std::move(action))
        , onVisible_(std::move(onVisible))
        , backgroundColor_(backgroundColor)
        , behavior_(behavior)
        , margin_(margin)
        , padding_(padding)
        , borderRadius_(borderRadius)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "SnackBar"; }
    
    Duration duration() const { return duration_; }
    
private:
    WidgetPtr content_;
    Duration duration_;
    WidgetPtr action_;
    std::function<void()> onVisible_;
    Color backgroundColor_;
    SnackBarBehavior behavior_;
    EdgeInsets margin_, padding_;
    BorderRadius borderRadius_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  ScaffoldMessenger — مدير إشعارات الـ Scaffold
// ═══════════════════════════════════════════════════════════════════════════════

class ScaffoldMessengerState;

class ScaffoldMessenger : public StatefulWidget {
public:
    ScaffoldMessenger(WidgetPtr child, Key key = Key())
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "ScaffoldMessenger"; }
    
    /// الحصول على الـ State من السياق
    static ScaffoldMessengerState* of(BuildContext* ctx);
    
    WidgetPtr child() const { return child_; }
    
private:
    WidgetPtr child_;
};

class ScaffoldMessengerState : public State<ScaffoldMessenger> {
public:
    /// إظهار SnackBar
    void showSnackBar(std::shared_ptr<SnackBar> snackBar) {
        snackBarQueue_.push(std::move(snackBar));
        if (!currentSnackBar_) {
            showNextSnackBar();
        }
    }
    
    /// إخفاء SnackBar الحالي
    void hideCurrentSnackBar() {
        currentSnackBar_ = nullptr;
        setState([this] { showNextSnackBar(); });
    }
    
    /// مسح جميع الـ SnackBars
    void clearSnackBars() {
        while (!snackBarQueue_.empty()) snackBarQueue_.pop();
        currentSnackBar_ = nullptr;
        setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override;
    
private:
    void showNextSnackBar() {
        if (!snackBarQueue_.empty()) {
            currentSnackBar_ = snackBarQueue_.front();
            snackBarQueue_.pop();
            setState([]{});
        }
    }
    
    std::queue<std::shared_ptr<SnackBar>> snackBarQueue_;
    std::shared_ptr<SnackBar> currentSnackBar_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  PopupMenuButton — زر القائمة المنبثقة
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
struct PopupMenuEntry {
    T value;
    WidgetPtr child;
    bool enabled = true;
};

template<typename T>
class PopupMenuItem : public StatelessWidget {
public:
    PopupMenuItem(
        T value,
        WidgetPtr child,
        bool enabled = true,
        EdgeInsets padding = EdgeInsets::symmetric(16, 0),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(std::move(value))
        , child_(std::move(child))
        , enabled_(enabled)
        , padding_(padding)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "PopupMenuItem"; }
    
    const T& value() const { return value_; }
    
private:
    T value_;
    WidgetPtr child_;
    bool enabled_;
    EdgeInsets padding_;
};

template<typename T>
class PopupMenuButton : public StatefulWidget {
public:
    PopupMenuButton(
        std::function<std::vector<std::shared_ptr<PopupMenuItem<T>>>(BuildContext*)> itemBuilder,
        std::function<void(T)> onSelected = nullptr,
        WidgetPtr child = nullptr,
        WidgetPtr icon = nullptr,
        std::optional<T> initialValue = std::nullopt,
        Color color = Color::transparent(),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , itemBuilder_(std::move(itemBuilder))
        , onSelected_(std::move(onSelected))
        , child_(std::move(child))
        , icon_(std::move(icon))
        , initialValue_(initialValue)
        , color_(color)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "PopupMenuButton"; }
    
private:
    std::function<std::vector<std::shared_ptr<PopupMenuItem<T>>>(BuildContext*)> itemBuilder_;
    std::function<void(T)> onSelected_;
    WidgetPtr child_;
    WidgetPtr icon_;
    std::optional<T> initialValue_;
    Color color_;
    
    template<typename U> friend class PopupMenuButtonState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Tooltip — تلميح
// ═══════════════════════════════════════════════════════════════════════════════

class Tooltip : public StatefulWidget {
public:
    Tooltip(
        std::string message,
        WidgetPtr child,
        Duration waitDuration = Duration::milliseconds(0),
        Duration showDuration = Duration::milliseconds(1500),
        EdgeInsets padding = EdgeInsets::symmetric(16, 4),
        EdgeInsets margin = EdgeInsets::all(0),
        float verticalOffset = 24,
        Color backgroundColor = Color(97, 97, 97),
        TextStyle textStyle = TextStyle{.color = Color(255, 255, 255), .fontSize = 12},
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , message_(std::move(message))
        , child_(std::move(child))
        , waitDuration_(waitDuration)
        , showDuration_(showDuration)
        , padding_(padding)
        , margin_(margin)
        , verticalOffset_(verticalOffset)
        , backgroundColor_(backgroundColor)
        , textStyle_(textStyle)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "Tooltip"; }
    
private:
    std::string message_;
    WidgetPtr child_;
    Duration waitDuration_, showDuration_;
    EdgeInsets padding_, margin_;
    float verticalOffset_;
    Color backgroundColor_;
    TextStyle textStyle_;
    
    friend class TooltipState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  DatePicker — منتقي التاريخ
// ═══════════════════════════════════════════════════════════════════════════════

struct Date {
    int year = 2024;
    int month = 1;  // 1-12
    int day = 1;    // 1-31
    
    bool operator==(const Date& o) const {
        return year == o.year && month == o.month && day == o.day;
    }
};

class DatePickerDialog : public StatefulWidget {
public:
    DatePickerDialog(
        Date initialDate,
        Date firstDate,
        Date lastDate,
        std::function<void(Date)> onDateSelected = nullptr,
        std::string helpText = "اختر التاريخ",
        std::string cancelText = "إلغاء",
        std::string confirmText = "موافق",
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , initialDate_(initialDate)
        , firstDate_(firstDate)
        , lastDate_(lastDate)
        , onDateSelected_(std::move(onDateSelected))
        , helpText_(std::move(helpText))
        , cancelText_(std::move(cancelText))
        , confirmText_(std::move(confirmText))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "DatePickerDialog"; }
    
private:
    Date initialDate_, firstDate_, lastDate_;
    std::function<void(Date)> onDateSelected_;
    std::string helpText_, cancelText_, confirmText_;
    
    friend class DatePickerDialogState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TimePicker — منتقي الوقت
// ═══════════════════════════════════════════════════════════════════════════════

struct TimeOfDay {
    int hour = 0;   // 0-23
    int minute = 0; // 0-59
    
    bool operator==(const TimeOfDay& o) const {
        return hour == o.hour && minute == o.minute;
    }
};

class TimePickerDialog : public StatefulWidget {
public:
    TimePickerDialog(
        TimeOfDay initialTime,
        std::function<void(TimeOfDay)> onTimeSelected = nullptr,
        std::string helpText = "اختر الوقت",
        std::string cancelText = "إلغاء",
        std::string confirmText = "موافق",
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , initialTime_(initialTime)
        , onTimeSelected_(std::move(onTimeSelected))
        , helpText_(std::move(helpText))
        , cancelText_(std::move(cancelText))
        , confirmText_(std::move(confirmText))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "TimePickerDialog"; }
    
private:
    TimeOfDay initialTime_;
    std::function<void(TimeOfDay)> onTimeSelected_;
    std::string helpText_, cancelText_, confirmText_;
    
    friend class TimePickerDialogState;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Dialog Functions — دوال إظهار الحوارات
// ═══════════════════════════════════════════════════════════════════════════════

/// إظهار حوار
template<typename T = void>
void showDialog(
    BuildContext* ctx,
    std::function<WidgetPtr(BuildContext*)> builder,
    bool barrierDismissible = true,
    Color barrierColor = Color(0, 0, 0, 128),
    std::function<void(T)> onResult = nullptr
) {
    // TODO: تطبيق فعلي مع Navigator
    auto dialog = builder(ctx);
}

/// إظهار ورقة سفلية Modal
void showModalBottomSheet(
    BuildContext* ctx,
    std::function<WidgetPtr(BuildContext*)> builder,
    Color backgroundColor = Color(255, 255, 255),
    bool isDismissible = true,
    bool enableDrag = true,
    BorderRadius borderRadius = BorderRadius::vertical(28, 0)
);

/// إظهار منتقي التاريخ
void showDatePicker(
    BuildContext* ctx,
    Date initialDate,
    Date firstDate,
    Date lastDate,
    std::function<void(Date)> onDatePicked
);

/// إظهار منتقي الوقت
void showTimePicker(
    BuildContext* ctx,
    TimeOfDay initialTime,
    std::function<void(TimeOfDay)> onTimePicked
);

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Aliases — أسماء عربية
// ═══════════════════════════════════════════════════════════════════════════════

using حوار = Dialog;
using حوار_تنبيه = AlertDialog;
using حوار_بسيط = SimpleDialog;
using خيار_حوار_بسيط = SimpleDialogOption;
using ورقة_سفلية = BottomSheet;
using شريط_اشعار = SnackBar;
using اجراء_شريط_اشعار = SnackBarAction;
using زر_قائمة_منبثقة = PopupMenuButton<std::string>;
using تلميح = Tooltip;
using تاريخ = Date;
using وقت = TimeOfDay;
using حوار_منتقي_تاريخ = DatePickerDialog;
using حوار_منتقي_وقت = TimePickerDialog;

// دوال مساعدة عربية
inline WidgetPtr حوار_تنبيه_بسيط(
    const std::string& عنوان,
    const std::string& محتوى,
    const std::string& نص_زر = "موافق",
    std::function<void()> عند_الضغط = nullptr
) {
    std::vector<WidgetPtr> actions;
    actions.push_back(std::make_shared<TextButton>(
        std::make_shared<Text>(نص_زر),
        std::move(عند_الضغط)
    ));
    
    return std::make_shared<AlertDialog>(
        std::make_optional(std::make_shared<Text>(عنوان)),
        std::make_optional(std::make_shared<Text>(محتوى)),
        std::move(actions)
    );
}

inline WidgetPtr حوار_تأكيد(
    const std::string& عنوان,
    const std::string& محتوى,
    std::function<void()> عند_التأكيد,
    std::function<void()> عند_الالغاء = nullptr,
    const std::string& نص_تأكيد = "نعم",
    const std::string& نص_الغاء = "لا"
) {
    std::vector<WidgetPtr> actions;
    actions.push_back(std::make_shared<TextButton>(
        std::make_shared<Text>(نص_الغاء),
        std::move(عند_الالغاء)
    ));
    actions.push_back(std::make_shared<ElevatedButton>(
        std::make_shared<Text>(نص_تأكيد),
        std::move(عند_التأكيد)
    ));
    
    return std::make_shared<AlertDialog>(
        std::make_optional(std::make_shared<Text>(عنوان)),
        std::make_optional(std::make_shared<Text>(محتوى)),
        std::move(actions)
    );
}

inline std::shared_ptr<SnackBar> شريط_اشعار_بسيط(
    const std::string& رسالة,
    Duration مدة = Duration::seconds(4)
) {
    return std::make_shared<SnackBar>(
        std::make_shared<Text>(رسالة, TextStyle{.color = Color(255, 255, 255)}),
        مدة
    );
}

inline WidgetPtr تلميح_بسيط(
    const std::string& رسالة,
    WidgetPtr طفل
) {
    return std::make_shared<Tooltip>(رسالة, std::move(طفل));
}

} // namespace flutter
} // namespace sad
