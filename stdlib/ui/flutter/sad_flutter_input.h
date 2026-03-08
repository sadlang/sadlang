// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_input.h — مكونات الإدخال بأسلوب Flutter
//  Input Widgets: TextField, Checkbox, Switch, Radio, Slider, etc.
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"
#include <functional>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  TextEditingController — متحكم النص
// ═══════════════════════════════════════════════════════════════════════════════

/// متحكم لإدارة نص حقل الإدخال
class TextEditingController {
public:
    TextEditingController(const std::string& text = "")
        : text_(text)
    {}
    
    /// الحصول على النص
    const std::string& text() const { return text_; }
    
    /// تعيين النص
    void setText(const std::string& text) {
        text_ = text;
        notifyListeners();
    }
    
    /// مسح النص
    void clear() {
        text_.clear();
        notifyListeners();
    }
    
    /// إضافة مستمع
    void addListener(std::function<void()> listener) {
        listeners_.push_back(std::move(listener));
    }
    
    /// إزالة جميع المستمعين
    void clearListeners() {
        listeners_.clear();
    }
    
private:
    void notifyListeners() {
        for (auto& listener : listeners_) {
            listener();
        }
    }
    
    std::string text_;
    std::vector<std::function<void()>> listeners_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  InputDecoration — تزيين حقل الإدخال
// ═══════════════════════════════════════════════════════════════════════════════

/// أنواع حدود حقل الإدخال
enum class InputBorderType {
    None,       // بدون حدود
    Outline,    // حدود خارجية
    Underline   // خط سفلي
};

/// تزيين حقل الإدخال
struct InputDecoration {
    std::string labelText;          // نص التسمية
    std::string hintText;           // نص التلميح
    std::string helperText;         // نص المساعدة
    std::string errorText;          // نص الخطأ
    std::string prefixText;         // نص قبل الإدخال
    std::string suffixText;         // نص بعد الإدخال
    
    WidgetPtr prefixIcon;           // أيقونة قبل
    WidgetPtr suffixIcon;           // أيقونة بعد
    
    InputBorderType borderType = InputBorderType::Underline;
    Color borderColor = Color::grey();
    Color focusedBorderColor = Color::blue();
    Color errorBorderColor = Color::red();
    float borderRadius = 4;
    float borderWidth = 1;
    
    Color fillColor = Color::transparent();
    bool filled = false;
    
    EdgeInsets contentPadding = EdgeInsets::symmetric(12, 16);
    
    bool enabled = true;
    
    /// بناء سريع
    static InputDecoration outlined(const std::string& label = "", const std::string& hint = "") {
        InputDecoration d;
        d.labelText = label;
        d.hintText = hint;
        d.borderType = InputBorderType::Outline;
        return d;
    }
    
    static InputDecoration underlined(const std::string& label = "", const std::string& hint = "") {
        InputDecoration d;
        d.labelText = label;
        d.hintText = hint;
        d.borderType = InputBorderType::Underline;
        return d;
    }
    
    static InputDecoration filled(const std::string& label = "", const std::string& hint = "") {
        InputDecoration d;
        d.labelText = label;
        d.hintText = hint;
        d.filled = true;
        d.fillColor = Color(245, 245, 245);
        d.borderType = InputBorderType::None;
        return d;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TextField — حقل النص
// ═══════════════════════════════════════════════════════════════════════════════

/// نوع لوحة المفاتيح
enum class TextInputType {
    Text,           // نص عام
    Number,         // أرقام
    Phone,          // رقم هاتف
    Email,          // بريد إلكتروني
    Url,            // رابط
    Multiline,      // متعدد الأسطر
    Password,       // كلمة مرور
    VisiblePassword // كلمة مرور مرئية
};

/// فعل لوحة المفاتيح
enum class TextInputAction {
    None,
    Done,       // تم
    Go,         // ذهاب
    Search,     // بحث
    Send,       // إرسال
    Next,       // التالي
    Previous,   // السابق
    NewLine     // سطر جديد
};

class TextField : public StatefulWidget {
public:
    TextField(
        std::shared_ptr<TextEditingController> controller = nullptr,
        InputDecoration decoration = {},
        TextInputType keyboardType = TextInputType::Text,
        TextInputAction textInputAction = TextInputAction::Done,
        bool obscureText = false,
        int maxLines = 1,
        int maxLength = 0,
        bool readOnly = false,
        bool autofocus = false,
        TextAlign textAlign = TextAlign::Right,
        std::function<void(const std::string&)> onChanged = nullptr,
        std::function<void(const std::string&)> onSubmitted = nullptr,
        std::function<void()> onTap = nullptr,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , controller_(controller ? controller : std::make_shared<TextEditingController>())
        , decoration_(std::move(decoration))
        , keyboardType_(keyboardType)
        , textInputAction_(textInputAction)
        , obscureText_(obscureText)
        , maxLines_(maxLines)
        , maxLength_(maxLength)
        , readOnly_(readOnly)
        , autofocus_(autofocus)
        , textAlign_(textAlign)
        , onChanged_(std::move(onChanged))
        , onSubmitted_(std::move(onSubmitted))
        , onTap_(std::move(onTap))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "TextField"; }
    
    std::shared_ptr<TextEditingController> controller() const { return controller_; }
    const InputDecoration& decoration() const { return decoration_; }
    bool obscureText() const { return obscureText_; }
    TextAlign textAlign() const { return textAlign_; }
    int maxLines() const { return maxLines_; }
    bool readOnly() const { return readOnly_; }
    
    std::function<void(const std::string&)> onChanged() const { return onChanged_; }
    std::function<void(const std::string&)> onSubmitted() const { return onSubmitted_; }
    
private:
    std::shared_ptr<TextEditingController> controller_;
    InputDecoration decoration_;
    TextInputType keyboardType_;
    TextInputAction textInputAction_;
    bool obscureText_;
    int maxLines_;
    int maxLength_;
    bool readOnly_;
    bool autofocus_;
    TextAlign textAlign_;
    std::function<void(const std::string&)> onChanged_;
    std::function<void(const std::string&)> onSubmitted_;
    std::function<void()> onTap_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Checkbox — مربع الاختيار
// ═══════════════════════════════════════════════════════════════════════════════

class Checkbox : public StatelessWidget {
public:
    Checkbox(
        bool value,
        std::function<void(bool)> onChanged,
        Color activeColor = Color::blue(),
        Color checkColor = Color::white(),
        bool tristate = false,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , activeColor_(activeColor)
        , checkColor_(checkColor)
        , tristate_(tristate)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Checkbox"; }
    
private:
    bool value_;
    std::function<void(bool)> onChanged_;
    Color activeColor_, checkColor_;
    bool tristate_;
};

/// CheckboxListTile — مربع اختيار مع نص
class CheckboxListTile : public StatelessWidget {
public:
    CheckboxListTile(
        bool value,
        std::function<void(bool)> onChanged,
        std::string title,
        std::string subtitle = "",
        WidgetPtr secondary = nullptr,
        bool isThreeLine = false,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , title_(std::move(title))
        , subtitle_(std::move(subtitle))
        , secondary_(std::move(secondary))
        , isThreeLine_(isThreeLine)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "CheckboxListTile"; }
    
private:
    bool value_;
    std::function<void(bool)> onChanged_;
    std::string title_, subtitle_;
    WidgetPtr secondary_;
    bool isThreeLine_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Switch — مفتاح التبديل
// ═══════════════════════════════════════════════════════════════════════════════

class Switch : public StatelessWidget {
public:
    Switch(
        bool value,
        std::function<void(bool)> onChanged,
        Color activeColor = Color::blue(),
        Color activeTrackColor = Color(100, 181, 246),
        Color inactiveThumbColor = Color(250, 250, 250),
        Color inactiveTrackColor = Color(189, 189, 189),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , activeColor_(activeColor)
        , activeTrackColor_(activeTrackColor)
        , inactiveThumbColor_(inactiveThumbColor)
        , inactiveTrackColor_(inactiveTrackColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Switch"; }
    
private:
    bool value_;
    std::function<void(bool)> onChanged_;
    Color activeColor_, activeTrackColor_;
    Color inactiveThumbColor_, inactiveTrackColor_;
};

/// SwitchListTile — مفتاح تبديل مع نص
class SwitchListTile : public StatelessWidget {
public:
    SwitchListTile(
        bool value,
        std::function<void(bool)> onChanged,
        std::string title,
        std::string subtitle = "",
        WidgetPtr secondary = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , title_(std::move(title))
        , subtitle_(std::move(subtitle))
        , secondary_(std::move(secondary))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "SwitchListTile"; }
    
private:
    bool value_;
    std::function<void(bool)> onChanged_;
    std::string title_, subtitle_;
    WidgetPtr secondary_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Radio — زر الاختيار
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class Radio : public StatelessWidget {
public:
    Radio(
        T value,
        T groupValue,
        std::function<void(T)> onChanged,
        Color activeColor = Color::blue(),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(std::move(value))
        , groupValue_(std::move(groupValue))
        , onChanged_(std::move(onChanged))
        , activeColor_(activeColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Radio"; }
    
private:
    T value_, groupValue_;
    std::function<void(T)> onChanged_;
    Color activeColor_;
};

/// RadioListTile — زر اختيار مع نص
template<typename T>
class RadioListTile : public StatelessWidget {
public:
    RadioListTile(
        T value,
        T groupValue,
        std::function<void(T)> onChanged,
        std::string title,
        std::string subtitle = "",
        WidgetPtr secondary = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(std::move(value))
        , groupValue_(std::move(groupValue))
        , onChanged_(std::move(onChanged))
        , title_(std::move(title))
        , subtitle_(std::move(subtitle))
        , secondary_(std::move(secondary))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "RadioListTile"; }
    
private:
    T value_, groupValue_;
    std::function<void(T)> onChanged_;
    std::string title_, subtitle_;
    WidgetPtr secondary_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Slider — شريط التمرير
// ═══════════════════════════════════════════════════════════════════════════════

class Slider : public StatelessWidget {
public:
    Slider(
        float value,
        std::function<void(float)> onChanged,
        float min = 0,
        float max = 1,
        int divisions = 0,
        std::string label = "",
        Color activeColor = Color::blue(),
        Color inactiveColor = Color(189, 189, 189),
        std::function<void(float)> onChangeStart = nullptr,
        std::function<void(float)> onChangeEnd = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(value)
        , onChanged_(std::move(onChanged))
        , min_(min)
        , max_(max)
        , divisions_(divisions)
        , label_(std::move(label))
        , activeColor_(activeColor)
        , inactiveColor_(inactiveColor)
        , onChangeStart_(std::move(onChangeStart))
        , onChangeEnd_(std::move(onChangeEnd))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Slider"; }
    
private:
    float value_, min_, max_;
    int divisions_;
    std::string label_;
    std::function<void(float)> onChanged_, onChangeStart_, onChangeEnd_;
    Color activeColor_, inactiveColor_;
};

/// RangeSlider — شريط نطاق
class RangeSlider : public StatelessWidget {
public:
    struct RangeValues {
        float start, end;
    };
    
    RangeSlider(
        RangeValues values,
        std::function<void(RangeValues)> onChanged,
        float min = 0,
        float max = 1,
        int divisions = 0,
        Color activeColor = Color::blue(),
        Color inactiveColor = Color(189, 189, 189),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , values_(values)
        , onChanged_(std::move(onChanged))
        , min_(min)
        , max_(max)
        , divisions_(divisions)
        , activeColor_(activeColor)
        , inactiveColor_(inactiveColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "RangeSlider"; }
    
private:
    RangeValues values_;
    std::function<void(RangeValues)> onChanged_;
    float min_, max_;
    int divisions_;
    Color activeColor_, inactiveColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  DropdownButton — قائمة منسدلة
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
struct DropdownMenuItem {
    T value;
    std::string text;
    WidgetPtr child;  // Optional custom widget
    bool enabled = true;
};

template<typename T>
class DropdownButton : public StatelessWidget {
public:
    DropdownButton(
        T value,
        std::vector<DropdownMenuItem<T>> items,
        std::function<void(T)> onChanged,
        std::string hint = "",
        WidgetPtr icon = nullptr,
        float iconSize = 24,
        bool isExpanded = false,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , value_(std::move(value))
        , items_(std::move(items))
        , onChanged_(std::move(onChanged))
        , hint_(std::move(hint))
        , icon_(std::move(icon))
        , iconSize_(iconSize)
        , isExpanded_(isExpanded)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "DropdownButton"; }
    
private:
    T value_;
    std::vector<DropdownMenuItem<T>> items_;
    std::function<void(T)> onChanged_;
    std::string hint_;
    WidgetPtr icon_;
    float iconSize_;
    bool isExpanded_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Input Widget Aliases
// ═══════════════════════════════════════════════════════════════════════════════

using متحكم_نص = TextEditingController;
using تزيين_ادخال = InputDecoration;
using حقل_نص = TextField;
using مربع_اختيار = Checkbox;
using قائمة_مربع_اختيار = CheckboxListTile;
using مفتاح = Switch;
using قائمة_مفتاح = SwitchListTile;
using شريط_قيمة = Slider;
using شريط_نطاق = RangeSlider;

// Arabic factories
inline WidgetPtr حقل_ادخال(
    std::shared_ptr<TextEditingController> متحكم = nullptr,
    const std::string& تسمية = "",
    const std::string& تلميح = "",
    bool كلمة_سر = false,
    std::function<void(const std::string&)> عند_التغيير = nullptr
) {
    InputDecoration تزيين;
    تزيين.labelText = تسمية;
    تزيين.hintText = تلميح;
    
    return std::make_shared<TextField>(
        متحكم,
        تزيين,
        TextInputType::Text,
        TextInputAction::Done,
        كلمة_سر,
        1, 0, false, false,
        TextAlign::Right,
        std::move(عند_التغيير)
    );
}

inline WidgetPtr اختيار(
    bool قيمة,
    std::function<void(bool)> عند_التغيير,
    Color لون_نشط = Color::blue()
) {
    return std::make_shared<Checkbox>(قيمة, std::move(عند_التغيير), لون_نشط);
}

inline WidgetPtr تبديل(
    bool قيمة,
    std::function<void(bool)> عند_التغيير,
    Color لون_نشط = Color::blue()
) {
    return std::make_shared<Switch>(قيمة, std::move(عند_التغيير), لون_نشط);
}

inline WidgetPtr منزلق(
    float قيمة,
    std::function<void(float)> عند_التغيير,
    float ادنى = 0,
    float اقصى = 100,
    int اقسام = 0
) {
    return std::make_shared<Slider>(
        قيمة, std::move(عند_التغيير), ادنى, اقصى, اقسام
    );
}

} // namespace flutter
} // namespace sad
