// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_dialogs.cpp — تطبيق الحوارات والنوافذ المنبثقة
//  Dialogs Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_dialogs.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Dialog
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Dialog::build(BuildContext* ctx) {
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(borderRadius_);
    
    return std::make_shared<Center>(
        std::make_shared<Container>(
            child_,
            std::nullopt, // عرض
            std::nullopt, // ارتفاع
            EdgeInsets::all(24),
            insetPadding_,
            decoration
        )
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AlertDialog
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr AlertDialog::build(BuildContext* ctx) {
    std::vector<WidgetPtr> children;
    
    // الأيقونة (إن وجدت)
    if (icon_) {
        children.push_back(std::make_shared<Center>(
            std::make_shared<Padding>(icon_, EdgeInsets::only(0, 0, 16, 0))
        ));
    }
    
    // العنوان
    if (title_.has_value()) {
        children.push_back(std::make_shared<Padding>(
            *title_,
            titlePadding_
        ));
    }
    
    // المحتوى
    if (content_.has_value()) {
        children.push_back(std::make_shared<Padding>(
            *content_,
            contentPadding_
        ));
    }
    
    // الأزرار
    if (!actions_.empty()) {
        std::vector<WidgetPtr> actionsCopy;
        for (auto& action : actions_) {
            actionsCopy.push_back(action);
        }
        
        children.push_back(std::make_shared<Padding>(
            std::make_shared<Row>(
                std::move(actionsCopy),
                actionsAlignment_
            ),
            actionsPadding_
        ));
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(borderRadius_);
    
    return std::make_shared<Dialog>(
        std::make_shared<Column>(
            std::move(children),
            MainAxisAlignment::Start,
            MainAxisSize::Min,
            CrossAxisAlignment::Stretch
        ),
        backgroundColor_,
        EdgeInsets::symmetric(40, 24),
        borderRadius_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SimpleDialogOption
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SimpleDialogOption::build(BuildContext* ctx) {
    return std::make_shared<GestureDetector>(
        std::make_shared<Padding>(child_, padding_),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SimpleDialog
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SimpleDialog::build(BuildContext* ctx) {
    std::vector<WidgetPtr> allChildren;
    
    // العنوان
    if (title_.has_value()) {
        allChildren.push_back(std::make_shared<Padding>(*title_, titlePadding_));
    }
    
    // الخيارات
    for (auto& child : children_) {
        allChildren.push_back(child);
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(BorderRadius::all(28));
    
    return std::make_shared<Dialog>(
        std::make_shared<Column>(
            std::move(allChildren),
            MainAxisAlignment::Start,
            MainAxisSize::Min,
            CrossAxisAlignment::Stretch
        ),
        backgroundColor_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  BottomSheet State
// ═══════════════════════════════════════════════════════════════════════════════

class BottomSheetState : public State<BottomSheet> {
public:
    WidgetPtr build(BuildContext* ctx) override {
        BoxDecoration decoration;
        decoration.setColor(widget()->backgroundColor());
        decoration.setBorderRadius(widget()->borderRadius());
        
        return std::make_shared<Container>(
            widget()->builder()(ctx),
            std::nullopt,
            std::nullopt,
            EdgeInsets::all(16),
            EdgeInsets{},
            decoration
        );
    }
};

std::unique_ptr<State_Base> BottomSheet::createState() {
    return std::make_unique<BottomSheetState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SnackBarAction
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SnackBarAction::build(BuildContext* ctx) {
    return std::make_shared<TextButton>(
        std::make_shared<Text>(label_, TextStyle{.color = textColor_}),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SnackBar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SnackBar::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    rowChildren.push_back(std::make_shared<Expanded>(content_));
    
    if (action_) {
        rowChildren.push_back(action_);
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    decoration.setBorderRadius(borderRadius_);
    
    WidgetPtr snackBarContent = std::make_shared<Container>(
        std::make_shared<Row>(std::move(rowChildren)),
        std::nullopt,
        std::nullopt,
        padding_,
        (behavior_ == SnackBarBehavior::Floating) ? margin_ : EdgeInsets{},
        decoration
    );
    
    return snackBarContent;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ScaffoldMessenger
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<State_Base> ScaffoldMessenger::createState() {
    return std::make_unique<ScaffoldMessengerState>();
}

ScaffoldMessengerState* ScaffoldMessenger::of(BuildContext* ctx) {
    // TODO: البحث في شجرة الـ Widget
    return nullptr;
}

WidgetPtr ScaffoldMessengerState::build(BuildContext* ctx) {
    std::vector<WidgetPtr> stackChildren;
    stackChildren.push_back(widget()->child());
    
    // إضافة الـ SnackBar الحالي (إن وجد)
    if (currentSnackBar_) {
        stackChildren.push_back(
            std::make_shared<Positioned>(
                currentSnackBar_,
                std::nullopt, // left
                std::nullopt, // top
                std::nullopt, // right
                0.0f          // bottom
            )
        );
    }
    
    return std::make_shared<Stack>(std::move(stackChildren));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PopupMenuItem
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
WidgetPtr PopupMenuItem<T>::build(BuildContext* ctx) {
    Color bgColor = enabled_ ? Color::transparent() : Color(200, 200, 200, 50);
    
    BoxDecoration decoration;
    decoration.setColor(bgColor);
    
    return std::make_shared<Container>(
        child_,
        std::nullopt,
        48.0f, // ارتفاع ثابت
        padding_,
        EdgeInsets{},
        decoration
    );
}

// Explicit instantiation for common types
template class PopupMenuItem<int>;
template class PopupMenuItem<std::string>;

// ═══════════════════════════════════════════════════════════════════════════════
//  PopupMenuButton State
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class PopupMenuButtonState : public State<PopupMenuButton<T>> {
public:
    void showMenu() {
        isMenuOpen_ = true;
        this->setState([]{});
    }
    
    void hideMenu() {
        isMenuOpen_ = false;
        this->setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = static_cast<PopupMenuButton<T>*>(this->widget_);
        
        WidgetPtr trigger = w->child_;
        if (!trigger) {
            trigger = w->icon_ ? w->icon_ : 
                std::make_shared<Icon>(IconData{"more_vert", 0xE5D4});
        }
        
        return std::make_shared<GestureDetector>(
            trigger,
            [this]{ showMenu(); }
        );
    }
    
private:
    bool isMenuOpen_ = false;
};

template<typename T>
std::unique_ptr<State_Base> PopupMenuButton<T>::createState() {
    return std::make_unique<PopupMenuButtonState<T>>();
}

// Explicit instantiation
template class PopupMenuButton<int>;
template class PopupMenuButton<std::string>;

// ═══════════════════════════════════════════════════════════════════════════════
//  Tooltip State
// ═══════════════════════════════════════════════════════════════════════════════

class TooltipState : public State<Tooltip> {
public:
    void initState() override {
        isVisible_ = false;
    }
    
    void showTooltip() {
        isVisible_ = true;
        setState([]{});
        // TODO: إخفاء بعد showDuration_
    }
    
    void hideTooltip() {
        isVisible_ = false;
        setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = static_cast<Tooltip*>(widget_);
        
        // نستخدم Stack لوضع التلميح فوق الطفل
        std::vector<WidgetPtr> children;
        
        // الطفل الأصلي
        children.push_back(
            std::make_shared<GestureDetector>(
                w->child_,
                nullptr, // onTap
                [this]{ showTooltip(); }, // onLongPress (نستخدم Tap مؤقتاً)
                nullptr  // onDoubleTap
            )
        );
        
        // التلميح (إذا كان مرئياً)
        if (isVisible_) {
            BoxDecoration decoration;
            decoration.setColor(w->backgroundColor_);
            decoration.setBorderRadius(BorderRadius::all(4));
            
            children.push_back(
                std::make_shared<Positioned>(
                    std::make_shared<Container>(
                        std::make_shared<Text>(w->message_, w->textStyle_),
                        std::nullopt,
                        std::nullopt,
                        w->padding_,
                        w->margin_,
                        decoration
                    ),
                    std::nullopt,
                    -w->verticalOffset_ // top (above child)
                )
            );
        }
        
        return std::make_shared<Stack>(std::move(children));
    }
    
private:
    bool isVisible_ = false;
};

std::unique_ptr<State_Base> Tooltip::createState() {
    return std::make_unique<TooltipState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DatePickerDialog State
// ═══════════════════════════════════════════════════════════════════════════════

class DatePickerDialogState : public State<DatePickerDialog> {
public:
    void initState() override {
        auto* w = static_cast<DatePickerDialog*>(widget_);
        selectedDate_ = w->initialDate_;
        currentMonth_ = w->initialDate_.month;
        currentYear_ = w->initialDate_.year;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = static_cast<DatePickerDialog*>(widget_);
        
        std::vector<WidgetPtr> children;
        
        // نص المساعدة
        children.push_back(std::make_shared<Padding>(
            std::make_shared<Text>(w->helpText_, TextStyle{.fontSize = 12}),
            EdgeInsets::all(16)
        ));
        
        // شريط التحكم بالشهر
        std::vector<WidgetPtr> monthNavigation;
        monthNavigation.push_back(std::make_shared<IconButton>(
            std::make_shared<Icon>(IconData{"chevron_left", 0xE5CB}),
            [this]{ previousMonth(); }
        ));
        
        std::string monthYear = std::to_string(currentMonth_) + "/" + std::to_string(currentYear_);
        monthNavigation.push_back(std::make_shared<Text>(monthYear, TextStyle{.fontSize = 16}));
        
        monthNavigation.push_back(std::make_shared<IconButton>(
            std::make_shared<Icon>(IconData{"chevron_right", 0xE5CC}),
            [this]{ nextMonth(); }
        ));
        
        children.push_back(std::make_shared<Row>(
            std::move(monthNavigation),
            MainAxisAlignment::SpaceBetween
        ));
        
        // شبكة الأيام (مبسطة)
        std::vector<WidgetPtr> dayButtons;
        int daysInMonth = getDaysInMonth(currentYear_, currentMonth_);
        for (int day = 1; day <= daysInMonth; ++day) {
            bool isSelected = (selectedDate_.year == currentYear_ && 
                              selectedDate_.month == currentMonth_ && 
                              selectedDate_.day == day);
            
            int d = day;
            dayButtons.push_back(std::make_shared<GestureDetector>(
                std::make_shared<Container>(
                    std::make_shared<Center>(
                        std::make_shared<Text>(std::to_string(day))
                    ),
                    36.0f, 36.0f,
                    EdgeInsets{},
                    EdgeInsets::all(2),
                    isSelected ? 
                        BoxDecoration().setColor(Color(33, 150, 243)).setBorderRadius(BorderRadius::all(18)) :
                        BoxDecoration()
                ),
                [this, d]{ selectDay(d); }
            ));
        }
        
        children.push_back(std::make_shared<Wrap>(
            std::move(dayButtons),
            Axis::Horizontal,
            WrapAlignment::Start
        ));
        
        // أزرار الإجراءات
        std::vector<WidgetPtr> actions;
        actions.push_back(std::make_shared<TextButton>(
            std::make_shared<Text>(w->cancelText_),
            nullptr // سيغلق الحوار
        ));
        actions.push_back(std::make_shared<ElevatedButton>(
            std::make_shared<Text>(w->confirmText_),
            [this, w]{ 
                if (w->onDateSelected_) {
                    w->onDateSelected_(selectedDate_);
                }
            }
        ));
        
        children.push_back(std::make_shared<Row>(
            std::move(actions),
            MainAxisAlignment::End
        ));
        
        return std::make_shared<AlertDialog>(
            std::nullopt,
            std::make_optional<WidgetPtr>(std::make_shared<Column>(std::move(children)))
        );
    }
    
private:
    void previousMonth() {
        currentMonth_--;
        if (currentMonth_ < 1) {
            currentMonth_ = 12;
            currentYear_--;
        }
        setState([]{});
    }
    
    void nextMonth() {
        currentMonth_++;
        if (currentMonth_ > 12) {
            currentMonth_ = 1;
            currentYear_++;
        }
        setState([]{});
    }
    
    void selectDay(int day) {
        selectedDate_ = Date{currentYear_, currentMonth_, day};
        setState([]{});
    }
    
    int getDaysInMonth(int year, int month) {
        static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && isLeapYear(year)) return 29;
        return days[month - 1];
    }
    
    bool isLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
    
    Date selectedDate_;
    int currentMonth_, currentYear_;
};

std::unique_ptr<State_Base> DatePickerDialog::createState() {
    return std::make_unique<DatePickerDialogState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  TimePickerDialog State
// ═══════════════════════════════════════════════════════════════════════════════

class TimePickerDialogState : public State<TimePickerDialog> {
public:
    void initState() override {
        auto* w = static_cast<TimePickerDialog*>(widget_);
        selectedTime_ = w->initialTime_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = static_cast<TimePickerDialog*>(widget_);
        
        std::vector<WidgetPtr> children;
        
        // نص المساعدة
        children.push_back(std::make_shared<Padding>(
            std::make_shared<Text>(w->helpText_, TextStyle{.fontSize = 12}),
            EdgeInsets::all(16)
        ));
        
        // عرض الوقت
        std::string timeStr = (selectedTime_.hour < 10 ? "0" : "") + 
                              std::to_string(selectedTime_.hour) + ":" +
                              (selectedTime_.minute < 10 ? "0" : "") + 
                              std::to_string(selectedTime_.minute);
        
        children.push_back(std::make_shared<Center>(
            std::make_shared<Text>(timeStr, TextStyle{.fontSize = 48})
        ));
        
        // أزرار التعديل
        std::vector<WidgetPtr> hourRow;
        hourRow.push_back(std::make_shared<IconButton>(
            std::make_shared<Icon>(IconData{"remove", 0xE15B}),
            [this]{ decrementHour(); }
        ));
        hourRow.push_back(std::make_shared<Text>("ساعة"));
        hourRow.push_back(std::make_shared<IconButton>(
            std::make_shared<Icon>(IconData{"add", 0xE145}),
            [this]{ incrementHour(); }
        ));
        
        children.push_back(std::make_shared<Row>(
            std::move(hourRow),
            MainAxisAlignment::Center
        ));
        
        std::vector<WidgetPtr> minuteRow;
        minuteRow.push_back(std::make_shared<IconButton>(
            std::make_shared<Icon>(IconData{"remove", 0xE15B}),
            [this]{ decrementMinute(); }
        ));
        minuteRow.push_back(std::make_shared<Text>("دقيقة"));
        minuteRow.push_back(std::make_shared<IconButton>(
            std::make_shared<Icon>(IconData{"add", 0xE145}),
            [this]{ incrementMinute(); }
        ));
        
        children.push_back(std::make_shared<Row>(
            std::move(minuteRow),
            MainAxisAlignment::Center
        ));
        
        // أزرار الإجراءات
        std::vector<WidgetPtr> actions;
        actions.push_back(std::make_shared<TextButton>(
            std::make_shared<Text>(w->cancelText_),
            nullptr
        ));
        actions.push_back(std::make_shared<ElevatedButton>(
            std::make_shared<Text>(w->confirmText_),
            [this, w]{ 
                if (w->onTimeSelected_) {
                    w->onTimeSelected_(selectedTime_);
                }
            }
        ));
        
        children.push_back(std::make_shared<Padding>(
            std::make_shared<Row>(std::move(actions), MainAxisAlignment::End),
            EdgeInsets::all(8)
        ));
        
        return std::make_shared<AlertDialog>(
            std::nullopt,
            std::make_optional<WidgetPtr>(std::make_shared<Column>(std::move(children)))
        );
    }
    
private:
    void incrementHour() {
        selectedTime_.hour = (selectedTime_.hour + 1) % 24;
        setState([]{});
    }
    
    void decrementHour() {
        selectedTime_.hour = (selectedTime_.hour + 23) % 24;
        setState([]{});
    }
    
    void incrementMinute() {
        selectedTime_.minute = (selectedTime_.minute + 1) % 60;
        setState([]{});
    }
    
    void decrementMinute() {
        selectedTime_.minute = (selectedTime_.minute + 59) % 60;
        setState([]{});
    }
    
    TimeOfDay selectedTime_;
};

std::unique_ptr<State_Base> TimePickerDialog::createState() {
    return std::make_unique<TimePickerDialogState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Dialog Functions
// ═══════════════════════════════════════════════════════════════════════════════

void showModalBottomSheet(
    BuildContext* ctx,
    std::function<WidgetPtr(BuildContext*)> builder,
    Color backgroundColor,
    bool isDismissible,
    bool enableDrag,
    BorderRadius borderRadius
) {
    // TODO: تطبيق مع Navigator و Modal Route
}

void showDatePicker(
    BuildContext* ctx,
    Date initialDate,
    Date firstDate,
    Date lastDate,
    std::function<void(Date)> onDatePicked
) {
    // TODO: showDialog مع DatePickerDialog
}

void showTimePicker(
    BuildContext* ctx,
    TimeOfDay initialTime,
    std::function<void(TimeOfDay)> onTimePicked
) {
    // TODO: showDialog مع TimePickerDialog
}

} // namespace flutter
} // namespace sad
