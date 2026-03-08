// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_cupertino.cpp — تطبيق مكونات iOS
//  Cupertino Widgets Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_cupertino.h"
#include "sad_flutter_dialogs.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoButton State
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoButtonState : public State<CupertinoButton> {
public:
    void initState() override {
        isPressed_ = false;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        Color bgColor = w->color_;
        float opacity = isPressed_ ? w->pressedOpacity_ : 1.0f;
        
        BoxDecoration decoration;
        if (w->color_.a > 0) {
            decoration.setColor(bgColor);
            decoration.setBorderRadius(w->borderRadius_);
        }
        
        return std::make_shared<GestureDetector>(
            std::make_shared<Opacity>(
                std::make_shared<Container>(
                    w->child_,
                    std::nullopt,
                    std::optional<float>(w->minSize_),
                    w->padding_,
                    EdgeInsets{},
                    decoration
                ),
                opacity
            ),
            [this]{ 
                if (widget()->onPressed_) widget()->onPressed_(); 
            },
            [this]{ isPressed_ = true; setState([]{}); },  // onTapDown
            [this]{ isPressed_ = false; setState([]{}); }  // onTapUp
        );
    }
    
private:
    bool isPressed_ = false;
    
    CupertinoButton* widget() const {
        return static_cast<CupertinoButton*>(widget_);
    }
};

std::unique_ptr<State_Base> CupertinoButton::createState() {
    return std::make_unique<CupertinoButtonState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoSwitch
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoSwitch::build(BuildContext* ctx) {
    Color trackColor = value_ ? activeColor_ : Color(142, 142, 147);
    
    // حجم مفتاح iOS: 51x31
    float width = 51.0f;
    float height = 31.0f;
    float thumbSize = 27.0f;
    float thumbOffset = value_ ? (width - thumbSize - 2) : 2;
    
    BoxDecoration trackDecoration;
    trackDecoration.setColor(trackColor);
    trackDecoration.setBorderRadius(BorderRadius::all(height / 2));
    
    BoxDecoration thumbDecoration;
    thumbDecoration.setColor(thumbColor_);
    thumbDecoration.setBorderRadius(BorderRadius::all(thumbSize / 2));
    // TODO: إضافة ظل
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Stack>(
                std::vector<WidgetPtr>{
                    // Thumb
                    std::make_shared<Positioned>(
                        std::make_shared<Container>(
                            nullptr,
                            thumbSize, thumbSize,
                            EdgeInsets{}, EdgeInsets{},
                            thumbDecoration
                        ),
                        thumbOffset,
                        2.0f
                    )
                }
            ),
            width, height,
            EdgeInsets{}, EdgeInsets{},
            trackDecoration
        ),
        [this]{ if (onChanged_) onChanged_(!value_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoSlider
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoSlider::build(BuildContext* ctx) {
    // نسبة التقدم
    double progress = (value_ - min_) / (max_ - min_);
    
    float trackHeight = 2.0f;
    float thumbSize = 28.0f;
    
    BoxDecoration activeTrackDecoration;
    activeTrackDecoration.setColor(activeColor_);
    activeTrackDecoration.setBorderRadius(BorderRadius::all(trackHeight / 2));
    
    BoxDecoration inactiveTrackDecoration;
    inactiveTrackDecoration.setColor(CupertinoColors::systemFill);
    inactiveTrackDecoration.setBorderRadius(BorderRadius::all(trackHeight / 2));
    
    BoxDecoration thumbDecoration;
    thumbDecoration.setColor(thumbColor_);
    thumbDecoration.setBorderRadius(BorderRadius::all(thumbSize / 2));
    // TODO: ظل للـ thumb
    
    // TODO: تطبيق السحب الفعلي
    return std::make_shared<Container>(
        std::make_shared<Stack>(
            std::vector<WidgetPtr>{
                // المسار الخامل
                std::make_shared<Positioned>(
                    std::make_shared<Container>(
                        nullptr,
                        std::nullopt, trackHeight,
                        EdgeInsets{}, EdgeInsets{},
                        inactiveTrackDecoration
                    ),
                    0.0f, (thumbSize - trackHeight) / 2,
                    0.0f
                ),
                // المسار النشط
                std::make_shared<Positioned>(
                    std::make_shared<Container>(
                        nullptr,
                        std::nullopt, trackHeight,
                        EdgeInsets{}, EdgeInsets{},
                        activeTrackDecoration
                    ),
                    0.0f, (thumbSize - trackHeight) / 2
                ),
                // الإصبع (Thumb)
                std::make_shared<Positioned>(
                    std::make_shared<Container>(
                        nullptr,
                        thumbSize, thumbSize,
                        EdgeInsets{}, EdgeInsets{},
                        thumbDecoration
                    ),
                    static_cast<float>(progress * 200), 0.0f // TODO: حساب موقع فعلي
                )
            }
        ),
        std::nullopt, thumbSize
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoTextField State
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoTextFieldState : public State<CupertinoTextField> {
public:
    void initState() override {
        auto* w = widget();
        if (w->controller_) {
            controller_ = w->controller_;
        } else {
            controller_ = std::make_shared<TextEditingController>();
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> rowChildren;
        
        // Prefix
        if (w->prefix_) {
            rowChildren.push_back(w->prefix_);
        }
        
        // حقل النص الفعلي (مبسط)
        std::string displayText = controller_->text();
        if (displayText.empty() && !w->placeholder_.empty()) {
            displayText = w->placeholder_;
        }
        
        if (w->obscureText_ && !controller_->text().empty()) {
            displayText = std::string(controller_->text().length(), '●');
        }
        
        TextStyle style;
        style.fontSize = 17;
        style.color = controller_->text().empty() 
            ? CupertinoColors::secondaryLabel 
            : CupertinoColors::label;
        
        rowChildren.push_back(
            std::make_shared<Expanded>(
                std::make_shared<Text>(displayText, style)
            )
        );
        
        // Clear button
        if (w->clearButtonMode_ == OverlayVisibilityMode::Always ||
            (w->clearButtonMode_ == OverlayVisibilityMode::Editing && !controller_->text().empty())) {
            rowChildren.push_back(
                std::make_shared<GestureDetector>(
                    std::make_shared<Icon>(
                        IconData{"cancel", 0xE5C9},
                        18.0f,
                        CupertinoColors::tertiaryLabel
                    ),
                    [this]{ 
                        controller_->setText(""); 
                        setState([]{});
                    }
                )
            );
        }
        
        // Suffix
        if (w->suffix_) {
            rowChildren.push_back(w->suffix_);
        }
        
        BoxDecoration decoration;
        decoration.setColor(w->backgroundColor_);
        decoration.setBorderRadius(w->borderRadius_);
        decoration.setBorder(Border{CupertinoColors::separator, 0.5f});
        
        return std::make_shared<Container>(
            std::make_shared<Row>(std::move(rowChildren)),
            std::nullopt, std::nullopt,
            w->padding_,
            EdgeInsets{},
            decoration
        );
    }
    
private:
    std::shared_ptr<TextEditingController> controller_;
    
    CupertinoTextField* widget() const {
        return static_cast<CupertinoTextField*>(widget_);
    }
};

std::unique_ptr<State_Base> CupertinoTextField::createState() {
    return std::make_unique<CupertinoTextFieldState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoNavigationBar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoNavigationBar::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    // Leading (زر الرجوع)
    if (leading_) {
        rowChildren.push_back(leading_);
    } else if (automaticallyImplyLeading_) {
        // زر رجوع تلقائي
        std::string backText = previousPageTitle_.empty() ? "رجوع" : previousPageTitle_;
        rowChildren.push_back(
            std::make_shared<GestureDetector>(
                std::make_shared<Row>(
                    std::vector<WidgetPtr>{
                        std::make_shared<Icon>(
                            IconData{"chevron_left", 0xE5CB},
                            28.0f,
                            CupertinoColors::activeBlue
                        ),
                        std::make_shared<Text>(
                            backText,
                            TextStyle{.color = CupertinoColors::activeBlue, .fontSize = 17}
                        )
                    }
                ),
                []{ /* TODO: Navigator.pop */ }
            )
        );
    } else {
        rowChildren.push_back(std::make_shared<SizedBox>(44.0f, std::nullopt));
    }
    
    // Middle (العنوان)
    if (middle_) {
        rowChildren.push_back(std::make_shared<Expanded>(
            std::make_shared<Center>(middle_)
        ));
    } else {
        rowChildren.push_back(std::make_shared<Expanded>(
            std::make_shared<SizedBox>()
        ));
    }
    
    // Trailing
    if (trailing_) {
        rowChildren.push_back(trailing_);
    } else {
        rowChildren.push_back(std::make_shared<SizedBox>(44.0f, std::nullopt));
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        std::make_shared<Column>(
            std::vector<WidgetPtr>{
                std::make_shared<Row>(std::move(rowChildren)),
                // الحد السفلي
                std::make_shared<Container>(
                    nullptr,
                    std::nullopt, 0.5f,
                    EdgeInsets{}, EdgeInsets{},
                    BoxDecoration().setColor(border_)
                )
            },
            MainAxisAlignment::End,
            MainAxisSize::Min
        ),
        std::nullopt, 44.0f + 0.5f,
        padding_,
        EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoSliverNavigationBar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoSliverNavigationBar::build(BuildContext* ctx) {
    TextStyle largeTitleStyle;
    largeTitleStyle.fontSize = 34;
    largeTitleStyle.fontWeight = FontWeight::Bold;
    largeTitleStyle.color = CupertinoColors::label;
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        std::make_shared<Column>(
            std::vector<WidgetPtr>{
                // الشريط العلوي الصغير
                std::make_shared<Row>(
                    std::vector<WidgetPtr>{
                        leading_ ? leading_ : std::make_shared<SizedBox>(44.0f, std::nullopt),
                        std::make_shared<Expanded>(
                            middle_ ? std::make_shared<Center>(middle_) : std::make_shared<SizedBox>()
                        ),
                        trailing_ ? trailing_ : std::make_shared<SizedBox>(44.0f, std::nullopt)
                    }
                ),
                // العنوان الكبير
                std::make_shared<Padding>(
                    std::make_shared<Text>(largeTitle_, largeTitleStyle),
                    EdgeInsets::fromLTRB(16, 0, 16, 8)
                )
            }
        ),
        std::nullopt, std::nullopt,
        EdgeInsets::only(0, 8, 0, 0),
        EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoTabBar
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoTabBar::build(BuildContext* ctx) {
    std::vector<WidgetPtr> tabItems;
    
    for (size_t i = 0; i < items_.size(); ++i) {
        bool isSelected = (static_cast<int>(i) == currentIndex_);
        Color itemColor = isSelected ? activeColor_ : inactiveColor_;
        
        WidgetPtr icon = isSelected && items_[i].activeIcon 
            ? items_[i].activeIcon 
            : items_[i].icon;
        
        int index = static_cast<int>(i);
        tabItems.push_back(
            std::make_shared<Expanded>(
                std::make_shared<GestureDetector>(
                    std::make_shared<Column>(
                        std::vector<WidgetPtr>{
                            // TODO: تلوين الأيقونة
                            icon,
                            std::make_shared<SizedBox>(std::nullopt, 2.0f),
                            std::make_shared<Text>(
                                items_[i].label,
                                TextStyle{.fontSize = 10, .color = itemColor}
                            )
                        },
                        MainAxisAlignment::Center,
                        MainAxisSize::Min,
                        CrossAxisAlignment::Center
                    ),
                    [this, index]{ if (onTap_) onTap_(index); }
                )
            )
        );
    }
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        std::make_shared<Column>(
            std::vector<WidgetPtr>{
                // الحد العلوي
                std::make_shared<Container>(
                    nullptr,
                    std::nullopt, 0.5f,
                    EdgeInsets{}, EdgeInsets{},
                    BoxDecoration().setColor(CupertinoColors::separator)
                ),
                std::make_shared<Padding>(
                    std::make_shared<Row>(std::move(tabItems)),
                    EdgeInsets::symmetric(0, 6)
                )
            }
        ),
        std::nullopt, 50.0f,
        EdgeInsets{}, EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoTabScaffold State
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoTabScaffoldState : public State<CupertinoTabScaffold> {
public:
    void initState() override {
        currentIndex_ = 0;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        BoxDecoration decoration;
        decoration.setColor(w->backgroundColor_);
        
        // تحديث TabBar مع index جديد
        auto tabBar = std::make_shared<CupertinoTabBar>(
            w->tabBar_->items_,
            currentIndex_,
            [this](int index) {
                currentIndex_ = index;
                setState([]{});
            },
            w->tabBar_->backgroundColor_,
            w->tabBar_->activeColor_,
            w->tabBar_->inactiveColor_
        );
        
        return std::make_shared<Container>(
            std::make_shared<Column>(
                std::vector<WidgetPtr>{
                    std::make_shared<Expanded>(
                        w->tabBuilder_(ctx, currentIndex_)
                    ),
                    tabBar
                }
            ),
            std::nullopt, std::nullopt,
            EdgeInsets{}, EdgeInsets{},
            decoration
        );
    }
    
private:
    int currentIndex_ = 0;
    
    CupertinoTabScaffold* widget() const {
        return static_cast<CupertinoTabScaffold*>(widget_);
    }
};

std::unique_ptr<State_Base> CupertinoTabScaffold::createState() {
    return std::make_unique<CupertinoTabScaffoldState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoPageScaffold
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoPageScaffold::build(BuildContext* ctx) {
    std::vector<WidgetPtr> columnChildren;
    
    if (navigationBar_) {
        columnChildren.push_back(navigationBar_);
    }
    
    columnChildren.push_back(std::make_shared<Expanded>(child_));
    
    BoxDecoration decoration;
    decoration.setColor(backgroundColor_);
    
    return std::make_shared<Container>(
        std::make_shared<Column>(std::move(columnChildren)),
        std::nullopt, std::nullopt,
        EdgeInsets{}, EdgeInsets{},
        decoration
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoDialogAction
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoDialogAction::build(BuildContext* ctx) {
    Color textColor = CupertinoColors::activeBlue;
    FontWeight weight = FontWeight::Normal;
    
    if (isDestructiveAction_) {
        textColor = CupertinoColors::systemRed;
    }
    if (isDefaultAction_) {
        weight = FontWeight::W600;
    }
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Center>(child_),
            std::nullopt, 44.0f,
            EdgeInsets::symmetric(8, 0)
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoAlertDialog
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoAlertDialog::build(BuildContext* ctx) {
    std::vector<WidgetPtr> dialogChildren;
    
    // العنوان
    if (title_) {
        dialogChildren.push_back(
            std::make_shared<Padding>(
                title_,
                EdgeInsets::fromLTRB(16, 20, 16, 2)
            )
        );
    }
    
    // المحتوى
    if (content_) {
        dialogChildren.push_back(
            std::make_shared<Padding>(
                content_,
                EdgeInsets::fromLTRB(16, 2, 16, 20)
            )
        );
    }
    
    // الفاصل
    dialogChildren.push_back(
        std::make_shared<Container>(
            nullptr,
            std::nullopt, 0.5f,
            EdgeInsets{}, EdgeInsets{},
            BoxDecoration().setColor(CupertinoColors::separator)
        )
    );
    
    // الأزرار
    if (actions_.size() == 2) {
        // زران جنباً إلى جنب
        std::vector<WidgetPtr> buttonRow;
        buttonRow.push_back(std::make_shared<Expanded>(actions_[0]));
        buttonRow.push_back(
            std::make_shared<Container>(
                nullptr, 0.5f, 44.0f,
                EdgeInsets{}, EdgeInsets{},
                BoxDecoration().setColor(CupertinoColors::separator)
            )
        );
        buttonRow.push_back(std::make_shared<Expanded>(actions_[1]));
        
        dialogChildren.push_back(std::make_shared<Row>(std::move(buttonRow)));
    } else {
        // أزرار عمودية
        for (size_t i = 0; i < actions_.size(); ++i) {
            if (i > 0) {
                dialogChildren.push_back(
                    std::make_shared<Container>(
                        nullptr,
                        std::nullopt, 0.5f,
                        EdgeInsets{}, EdgeInsets{},
                        BoxDecoration().setColor(CupertinoColors::separator)
                    )
                );
            }
            dialogChildren.push_back(actions_[i]);
        }
    }
    
    BoxDecoration decoration;
    decoration.setColor(Color(255, 255, 255, 230));
    decoration.setBorderRadius(BorderRadius::all(14));
    
    return std::make_shared<Center>(
        std::make_shared<Container>(
            std::make_shared<Column>(
                std::move(dialogChildren),
                MainAxisAlignment::Start,
                MainAxisSize::Min
            ),
            270.0f, std::nullopt,
            EdgeInsets{}, EdgeInsets{},
            decoration
        )
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoActionSheetAction
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoActionSheetAction::build(BuildContext* ctx) {
    Color textColor = CupertinoColors::activeBlue;
    
    if (isDestructiveAction_) {
        textColor = CupertinoColors::systemRed;
    }
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            std::make_shared<Center>(child_),
            std::nullopt, 57.0f
        ),
        onPressed_
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoActionSheet
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoActionSheet::build(BuildContext* ctx) {
    std::vector<WidgetPtr> sheetChildren;
    
    BoxDecoration mainDecoration;
    mainDecoration.setColor(Color(255, 255, 255, 230));
    mainDecoration.setBorderRadius(BorderRadius::all(14));
    
    std::vector<WidgetPtr> mainSectionChildren;
    
    // العنوان
    if (title_) {
        mainSectionChildren.push_back(
            std::make_shared<Padding>(
                title_,
                EdgeInsets::fromLTRB(16, 14, 16, 0)
            )
        );
    }
    
    // الرسالة
    if (message_) {
        mainSectionChildren.push_back(
            std::make_shared<Padding>(
                message_,
                EdgeInsets::all(14)
            )
        );
    }
    
    // فاصل قبل الإجراءات
    if (!mainSectionChildren.empty() && !actions_.empty()) {
        mainSectionChildren.push_back(
            std::make_shared<Container>(
                nullptr,
                std::nullopt, 0.5f,
                EdgeInsets{}, EdgeInsets{},
                BoxDecoration().setColor(CupertinoColors::separator)
            )
        );
    }
    
    // الإجراءات
    for (size_t i = 0; i < actions_.size(); ++i) {
        if (i > 0) {
            mainSectionChildren.push_back(
                std::make_shared<Container>(
                    nullptr,
                    std::nullopt, 0.5f,
                    EdgeInsets{}, EdgeInsets{},
                    BoxDecoration().setColor(CupertinoColors::separator)
                )
            );
        }
        mainSectionChildren.push_back(actions_[i]);
    }
    
    sheetChildren.push_back(
        std::make_shared<Container>(
            std::make_shared<Column>(
                std::move(mainSectionChildren),
                MainAxisAlignment::Start,
                MainAxisSize::Min
            ),
            std::nullopt, std::nullopt,
            EdgeInsets{}, EdgeInsets{},
            mainDecoration
        )
    );
    
    // زر الإلغاء (منفصل)
    if (cancelButton_) {
        BoxDecoration cancelDecoration;
        cancelDecoration.setColor(Color(255, 255, 255));
        cancelDecoration.setBorderRadius(BorderRadius::all(14));
        
        sheetChildren.push_back(std::make_shared<SizedBox>(std::nullopt, 8.0f));
        sheetChildren.push_back(
            std::make_shared<Container>(
                cancelButton_,
                std::nullopt, 57.0f,
                EdgeInsets{}, EdgeInsets{},
                cancelDecoration
            )
        );
    }
    
    return std::make_shared<Container>(
        std::make_shared<Column>(
            std::move(sheetChildren),
            MainAxisAlignment::End,
            MainAxisSize::Min
        ),
        std::nullopt, std::nullopt,
        EdgeInsets::all(8)
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoPicker State
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoPickerState : public State<CupertinoPicker> {
public:
    void initState() override {
        auto* w = widget();
        selectedIndex_ = w->initialItem_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        std::vector<WidgetPtr> items;
        
        for (size_t i = 0; i < w->children_.size(); ++i) {
            bool isSelected = (static_cast<int>(i) == selectedIndex_);
            float opacity = isSelected ? 1.0f : 0.5f;
            
            int index = static_cast<int>(i);
            items.push_back(
                std::make_shared<GestureDetector>(
                    std::make_shared<Opacity>(
                        std::make_shared<Container>(
                            std::make_shared<Center>(w->children_[i]),
                            std::nullopt, w->itemExtent_
                        ),
                        opacity
                    ),
                    [this, index]{ 
                        selectedIndex_ = index;
                        if (widget()->onSelectedItemChanged_) {
                            widget()->onSelectedItemChanged_(index);
                        }
                        setState([]{});
                    }
                )
            );
        }
        
        BoxDecoration decoration;
        decoration.setColor(w->backgroundColor_);
        
        return std::make_shared<Container>(
            std::make_shared<Column>(std::move(items)),
            std::nullopt, std::nullopt,
            EdgeInsets{}, EdgeInsets{},
            decoration
        );
    }
    
private:
    int selectedIndex_ = 0;
    
    CupertinoPicker* widget() const {
        return static_cast<CupertinoPicker*>(widget_);
    }
};

std::unique_ptr<State_Base> CupertinoPicker::createState() {
    return std::make_unique<CupertinoPickerState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoDatePicker State
// ═══════════════════════════════════════════════════════════════════════════════

class CupertinoDatePickerState : public State<CupertinoDatePicker> {
public:
    void initState() override {
        auto* w = widget();
        selectedDate_ = w->initialDateTime_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = widget();
        
        // TODO: تطبيق منتقي التاريخ بالكامل مع العجلات
        std::string dateStr = std::to_string(selectedDate_.day) + "/" +
                              std::to_string(selectedDate_.month) + "/" +
                              std::to_string(selectedDate_.year);
        
        BoxDecoration decoration;
        decoration.setColor(w->backgroundColor_);
        
        return std::make_shared<Container>(
            std::make_shared<Center>(
                std::make_shared<Text>(dateStr, TextStyle{.fontSize = 24})
            ),
            std::nullopt, 216.0f, // ارتفاع منتقي iOS
            EdgeInsets{}, EdgeInsets{},
            decoration
        );
    }
    
private:
    Date selectedDate_;
    
    CupertinoDatePicker* widget() const {
        return static_cast<CupertinoDatePicker*>(widget_);
    }
};

std::unique_ptr<State_Base> CupertinoDatePicker::createState() {
    return std::make_unique<CupertinoDatePickerState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CupertinoActivityIndicator
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CupertinoActivityIndicator::build(BuildContext* ctx) {
    // TODO: تطبيق الحركة الدوارة
    return std::make_shared<Container>(
        std::make_shared<Center>(
            std::make_shared<Text>("⟳", TextStyle{.fontSize = radius_ * 2, .color = color_})
        ),
        radius_ * 2, radius_ * 2
    );
}

} // namespace flutter
} // namespace sad
