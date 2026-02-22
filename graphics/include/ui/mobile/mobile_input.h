// mobile_input.h - مركبات الإدخال للتطبيقات الهاتفية
// Mobile Input Components
//
// الوصف: مجموعة شاملة من مركبات الإدخال للتطبيقات الهاتفية
// Description: Comprehensive input components for mobile apps
// مشابه لـ Flutter TextField, DatePicker, etc.
//
// المركبات: TextField, PasswordField, SearchBar, OTPInput, PinInput,
//           DatePicker, TimePicker, DateTimePicker, ColorPicker, FilePicker,
//           ImagePicker, Autocomplete, Dropdown, MultiSelect, RatingInput

#ifndef SAD_GRAPHICS_MOBILE_INPUT_H
#define SAD_GRAPHICS_MOBILE_INPUT_H

#include "mobile_core.h"
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include <chrono>

namespace sad {
namespace graphics {
namespace ui {
namespace mobile {

// ============================================================================
// الثوابت والأنماط / Constants and Styles
// ============================================================================

/// نوع لوحة المفاتيح / Keyboard type
enum class KeyboardType {
    Text,           // نص عادي / Normal text
    Number,         // أرقام فقط / Numbers only
    Decimal,        // أرقام عشرية / Decimal numbers
    Phone,          // رقم هاتف / Phone number
    Email,          // بريد إلكتروني / Email
    Url,            // رابط / URL
    Password,       // كلمة مرور / Password
    Multiline       // متعدد الأسطر / Multiline
};

/// إجراء لوحة المفاتيح / Keyboard action
enum class TextInputAction {
    None,       // لا شيء / None
    Done,       // تم / Done
    Go,         // اذهب / Go
    Search,     // بحث / Search
    Send,       // إرسال / Send
    Next,       // التالي / Next
    Previous,   // السابق / Previous
    Continue,   // متابعة / Continue
    Join,       // انضمام / Join
    Route,      // مسار / Route
    NewLine     // سطر جديد / New line
};

/// نمط حقل النص / Text field style
enum class TextFieldStyle {
    Outlined,       // محاط بإطار / Outlined border
    Filled,         // ممتلئ / Filled background
    Underlined,     // خط سفلي / Underlined
    None            // بدون تزيين / No decoration
};

/// تزيين حقل الإدخال / Input decoration
struct InputDecoration {
    std::string labelText;          // نص التسمية / Label text
    std::string hintText;           // نص التلميح / Hint text
    std::string helperText;         // نص المساعدة / Helper text
    std::string errorText;          // نص الخطأ / Error text
    std::string prefixText;         // نص البادئة / Prefix text
    std::string suffixText;         // نص اللاحقة / Suffix text
    std::string counterText;        // نص العداد / Counter text
    
    // رموز / Icons
    std::optional<UInt32> prefixIcon;   // رمز البادئة / Prefix icon
    std::optional<UInt32> suffixIcon;   // رمز اللاحقة / Suffix icon
    
    // ألوان / Colors
    Color labelColor;
    Color hintColor;
    Color errorColor;
    Color focusColor;
    Color fillColor;
    Color borderColor;
    
    // أبعاد / Dimensions
    EdgeInsets contentPadding;
    Float32 borderRadius;
    Float32 borderWidth;
    
    // نمط / Style
    TextFieldStyle style;
    bool isDense;               // مكثف / Dense
    bool filled;                // ممتلئ / Filled
    bool enabled;               // ممكّن / Enabled
    
    InputDecoration()
        : labelColor(Color::Gray),
          hintColor(Color(128, 128, 128, 180)),
          errorColor(Color::Red),
          focusColor(Color::Blue),
          fillColor(Color(245, 245, 245)),
          borderColor(Color::Gray),
          contentPadding(EdgeInsets::All(12)),
          borderRadius(8),
          borderWidth(1),
          style(TextFieldStyle::Outlined),
          isDense(false),
          filled(false),
          enabled(true) {}
};

// ============================================================================
// 1. TextField - حقل النص
// ============================================================================

/// حقل إدخال النص الأساسي
/// Basic text input field - مشابه لـ Flutter TextField
class TextField : public Widget {
public:
    // الخصائص الأساسية / Basic properties
    std::string text;                   // النص الحالي / Current text
    std::string placeholder;            // نص العنصر النائب / Placeholder text
    InputDecoration decoration;         // التزيين / Decoration
    
    // إعدادات لوحة المفاتيح / Keyboard settings
    KeyboardType keyboardType;
    TextInputAction textInputAction;
    bool autocorrect;                   // تصحيح تلقائي / Auto correct
    bool enableSuggestions;             // اقتراحات / Suggestions
    
    // قيود / Constraints
    Int32 maxLength;                    // الحد الأقصى للطول / Max length
    Int32 maxLines;                     // الحد الأقصى للأسطر / Max lines
    Int32 minLines;                     // الحد الأدنى للأسطر / Min lines
    
    // حالة / State
    bool readOnly;                      // قراءة فقط / Read only
    bool obscureText;                   // نص مخفي / Obscured text
    bool autofocus;                     // تركيز تلقائي / Auto focus
    bool showCursor;                    // إظهار المؤشر / Show cursor
    
    // أحداث / Events
    std::function<void(const std::string&)> onChanged;      // عند التغيير / On change
    std::function<void(const std::string&)> onSubmitted;    // عند الإرسال / On submit
    std::function<void()> onEditingComplete;                // عند اكتمال التحرير / On editing complete
    std::function<void()> onTap;                            // عند النقر / On tap

    TextField()
        : keyboardType(KeyboardType::Text),
          textInputAction(TextInputAction::Done),
          autocorrect(true),
          enableSuggestions(true),
          maxLength(-1),
          maxLines(1),
          minLines(1),
          readOnly(false),
          obscureText(false),
          autofocus(false),
          showCursor(true) {}

    /// تعيين النص / Set text
    TextField& SetText(const std::string& value) {
        text = value;
        if (onChanged) onChanged(text);
        return *this;
    }

    /// تعيين التلميح / Set placeholder
    TextField& SetPlaceholder(const std::string& value) {
        placeholder = value;
        decoration.hintText = value;
        return *this;
    }

    /// تعيين التسمية / Set label
    TextField& SetLabel(const std::string& value) {
        decoration.labelText = value;
        return *this;
    }

    /// تعيين نوع لوحة المفاتيح / Set keyboard type
    TextField& SetKeyboardType(KeyboardType type) {
        keyboardType = type;
        return *this;
    }

    /// تعيين الحد الأقصى للطول / Set max length
    TextField& SetMaxLength(Int32 length) {
        maxLength = length;
        return *this;
    }

    /// تعيين قراءة فقط / Set read only
    TextField& SetReadOnly(bool value) {
        readOnly = value;
        return *this;
    }

    /// مسح النص / Clear text
    void Clear() {
        text.clear();
        if (onChanged) onChanged(text);
    }

    void Update(Float32 deltaTime) override {
        // تحديث حالة المؤشر والتحديد / Update cursor and selection state
    }

    void Draw() override {
        // رسم الحقل حسب النمط / Draw field based on style
    }
};

// ============================================================================
// 2. PasswordField - حقل كلمة المرور
// ============================================================================

/// حقل إدخال كلمة المرور
/// Password input field with visibility toggle
class PasswordField : public TextField {
public:
    bool showPassword;              // إظهار كلمة المرور / Show password
    bool showToggleButton;          // إظهار زر التبديل / Show toggle button
    UInt32 hiddenChar;              // الحرف المخفي / Hidden character (●)
    
    // متطلبات القوة / Strength requirements
    struct StrengthRequirements {
        Int32 minLength;            // الحد الأدنى للطول / Min length
        bool requireUppercase;      // حرف كبير / Uppercase required
        bool requireLowercase;      // حرف صغير / Lowercase required
        bool requireDigit;          // رقم / Digit required
        bool requireSpecial;        // رمز خاص / Special char required
        
        StrengthRequirements()
            : minLength(8),
              requireUppercase(true),
              requireLowercase(true),
              requireDigit(true),
              requireSpecial(false) {}
    };
    
    StrengthRequirements requirements;
    bool showStrengthIndicator;     // إظهار مؤشر القوة / Show strength indicator
    
    // أحداث / Events
    std::function<void(Float32)> onStrengthChanged;  // عند تغير القوة (0-1) / On strength change

    PasswordField()
        : showPassword(false),
          showToggleButton(true),
          hiddenChar(0x25CF),  // ●
          showStrengthIndicator(false) {
        obscureText = true;
        keyboardType = KeyboardType::Password;
    }

    /// تبديل إظهار كلمة المرور / Toggle password visibility
    void ToggleVisibility() {
        showPassword = !showPassword;
        obscureText = !showPassword;
    }

    /// حساب قوة كلمة المرور / Calculate password strength
    Float32 CalculateStrength() const {
        if (text.empty()) return 0.0f;
        
        Float32 score = 0.0f;
        Int32 checks = 0;
        
        // طول / Length
        if (text.length() >= static_cast<size_t>(requirements.minLength)) {
            score += 0.25f;
        }
        checks++;
        
        // حرف كبير / Uppercase
        bool hasUpper = false;
        for (char c : text) {
            if (c >= 'A' && c <= 'Z') { hasUpper = true; break; }
        }
        if (hasUpper) score += 0.25f;
        checks++;
        
        // حرف صغير / Lowercase
        bool hasLower = false;
        for (char c : text) {
            if (c >= 'a' && c <= 'z') { hasLower = true; break; }
        }
        if (hasLower) score += 0.25f;
        checks++;
        
        // رقم / Digit
        bool hasDigit = false;
        for (char c : text) {
            if (c >= '0' && c <= '9') { hasDigit = true; break; }
        }
        if (hasDigit) score += 0.25f;
        
        return score;
    }

    /// هل كلمة المرور صالحة / Is password valid
    bool IsValid() const {
        return CalculateStrength() >= 0.75f;
    }
};

// ============================================================================
// 3. SearchBar - شريط البحث
// ============================================================================

/// شريط البحث مع اقتراحات
/// Search bar with suggestions - مشابه لـ Flutter SearchBar
class SearchBar : public Widget {
public:
    std::string query;                  // نص البحث / Search query
    std::string placeholder;            // نص العنصر النائب / Placeholder
    
    // إعدادات / Settings
    bool showCancelButton;              // إظهار زر الإلغاء / Show cancel button
    bool showClearButton;               // إظهار زر المسح / Show clear button
    bool autofocus;                     // تركيز تلقائي / Auto focus
    Float32 debounceMs;                 // تأخير البحث / Debounce delay
    
    // نمط / Style
    Color backgroundColor;
    Color textColor;
    Color iconColor;
    Float32 borderRadius;
    Float32 height;
    EdgeInsets padding;
    
    // اقتراحات / Suggestions
    std::vector<std::string> suggestions;
    bool showSuggestions;
    Int32 maxSuggestions;
    
    // أحداث / Events
    std::function<void(const std::string&)> onChanged;
    std::function<void(const std::string&)> onSubmitted;
    std::function<void()> onCancelled;
    std::function<void(const std::string&)> onSuggestionSelected;

    SearchBar()
        : placeholder("بحث..."),
          showCancelButton(true),
          showClearButton(true),
          autofocus(false),
          debounceMs(300),
          backgroundColor(Color(245, 245, 245)),
          textColor(Color::Black),
          iconColor(Color::Gray),
          borderRadius(20),
          height(40),
          padding(EdgeInsets::Symmetric(8, 12)),
          showSuggestions(false),
          maxSuggestions(5) {}

    /// تعيين الاستعلام / Set query
    SearchBar& SetQuery(const std::string& value) {
        query = value;
        if (onChanged) onChanged(query);
        return *this;
    }

    /// مسح البحث / Clear search
    void Clear() {
        query.clear();
        if (onChanged) onChanged(query);
    }

    /// إضافة اقتراح / Add suggestion
    void AddSuggestion(const std::string& suggestion) {
        if (suggestions.size() < static_cast<size_t>(maxSuggestions)) {
            suggestions.push_back(suggestion);
        }
    }

    /// مسح الاقتراحات / Clear suggestions
    void ClearSuggestions() {
        suggestions.clear();
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 4. OTPInput - إدخال رمز التحقق
// ============================================================================

/// حقل إدخال رمز التحقق (OTP)
/// One-Time Password input field
class OTPInput : public Widget {
public:
    std::string code;                   // الرمز الحالي / Current code
    Int32 length;                       // طول الرمز / Code length
    
    // نمط / Style
    Float32 boxSize;                    // حجم المربع / Box size
    Float32 spacing;                    // المسافة بين المربعات / Spacing
    Color boxColor;                     // لون المربع / Box color
    Color activeBoxColor;               // لون المربع النشط / Active box color
    Color textColor;                    // لون النص / Text color
    Color errorColor;                   // لون الخطأ / Error color
    Float32 borderRadius;               // نصف قطر الحدود / Border radius
    Float32 borderWidth;                // عرض الحدود / Border width
    
    // إعدادات / Settings
    bool obscureInput;                  // إخفاء الإدخال / Obscure input
    bool autoDismissKeyboard;           // إغلاق لوحة المفاتيح تلقائياً / Auto dismiss keyboard
    bool autoSubmit;                    // إرسال تلقائي / Auto submit
    
    // حالة / State
    Int32 currentIndex;                 // الفهرس الحالي / Current index
    bool hasError;                      // يوجد خطأ / Has error
    
    // أحداث / Events
    std::function<void(const std::string&)> onChanged;
    std::function<void(const std::string&)> onCompleted;

    OTPInput()
        : length(6),
          boxSize(50),
          spacing(10),
          boxColor(Color(240, 240, 240)),
          activeBoxColor(Color::Blue),
          textColor(Color::Black),
          errorColor(Color::Red),
          borderRadius(8),
          borderWidth(2),
          obscureInput(false),
          autoDismissKeyboard(true),
          autoSubmit(true),
          currentIndex(0),
          hasError(false) {}

    /// تعيين طول الرمز / Set code length
    OTPInput& SetLength(Int32 len) {
        length = len;
        return *this;
    }

    /// إدخال رقم / Input digit
    void InputDigit(char digit) {
        if (code.length() < static_cast<size_t>(length)) {
            code += digit;
            currentIndex++;
            if (onChanged) onChanged(code);
            
            if (code.length() == static_cast<size_t>(length) && autoSubmit) {
                if (onCompleted) onCompleted(code);
            }
        }
    }

    /// حذف آخر رقم / Delete last digit
    void DeleteDigit() {
        if (!code.empty()) {
            code.pop_back();
            currentIndex--;
            if (onChanged) onChanged(code);
        }
    }

    /// مسح الرمز / Clear code
    void Clear() {
        code.clear();
        currentIndex = 0;
        hasError = false;
        if (onChanged) onChanged(code);
    }

    /// تعيين حالة الخطأ / Set error state
    void SetError(bool error) {
        hasError = error;
    }

    /// هل الرمز مكتمل / Is code complete
    bool IsComplete() const {
        return code.length() == static_cast<size_t>(length);
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 5. PinInput - إدخال الرقم السري
// ============================================================================

/// حقل إدخال الرقم السري (PIN)
/// PIN input field with dots
class PinInput : public Widget {
public:
    std::string pin;                    // الرقم السري / PIN
    Int32 length;                       // طول الرقم السري / PIN length
    
    // نمط / Style
    Float32 dotSize;                    // حجم النقطة / Dot size
    Float32 spacing;                    // المسافة / Spacing
    Color emptyDotColor;                // لون النقطة الفارغة / Empty dot color
    Color filledDotColor;               // لون النقطة الممتلئة / Filled dot color
    Color errorColor;                   // لون الخطأ / Error color
    
    // إعدادات / Settings
    bool showDots;                      // إظهار النقاط فقط / Show dots only
    bool vibrate;                       // اهتزاز عند الضغط / Vibrate on press
    
    // حالة / State
    bool hasError;
    bool isLocked;                      // مقفل / Locked
    Int32 attemptsLeft;                 // المحاولات المتبقية / Attempts left
    
    // أحداث / Events
    std::function<void(const std::string&)> onChanged;
    std::function<void(const std::string&)> onCompleted;
    std::function<void()> onMaxAttemptsReached;

    PinInput()
        : length(4),
          dotSize(16),
          spacing(20),
          emptyDotColor(Color(200, 200, 200)),
          filledDotColor(Color::Black),
          errorColor(Color::Red),
          showDots(true),
          vibrate(true),
          hasError(false),
          isLocked(false),
          attemptsLeft(3) {}

    /// إدخال رقم / Input digit
    void InputDigit(char digit) {
        if (isLocked || pin.length() >= static_cast<size_t>(length)) return;
        
        pin += digit;
        if (onChanged) onChanged(pin);
        
        if (pin.length() == static_cast<size_t>(length)) {
            if (onCompleted) onCompleted(pin);
        }
    }

    /// حذف / Delete
    void Delete() {
        if (!pin.empty()) {
            pin.pop_back();
            if (onChanged) onChanged(pin);
        }
    }

    /// مسح / Clear
    void Clear() {
        pin.clear();
        hasError = false;
        if (onChanged) onChanged(pin);
    }

    /// تعيين خطأ / Set error
    void SetError() {
        hasError = true;
        attemptsLeft--;
        Clear();
        
        if (attemptsLeft <= 0) {
            isLocked = true;
            if (onMaxAttemptsReached) onMaxAttemptsReached();
        }
    }

    /// إعادة تعيين / Reset
    void Reset() {
        Clear();
        hasError = false;
        isLocked = false;
        attemptsLeft = 3;
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 6. DatePicker - منتقي التاريخ
// ============================================================================

/// بنية التاريخ / Date structure
struct Date {
    Int32 year;
    Int32 month;    // 1-12
    Int32 day;      // 1-31
    
    Date() : year(2024), month(1), day(1) {}
    Date(Int32 y, Int32 m, Int32 d) : year(y), month(m), day(d) {}
    
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    bool operator<=(const Date& other) const {
        return *this < other || (*this == other);
    }
    
    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
    
    /// التاريخ كنص / Date as string
    std::string ToString(const std::string& format = "yyyy-MM-dd") const {
        // تنسيق بسيط / Simple format
        return std::to_string(year) + "-" + 
               (month < 10 ? "0" : "") + std::to_string(month) + "-" +
               (day < 10 ? "0" : "") + std::to_string(day);
    }
    
    /// اليوم / Today
    static Date Today() {
        // سيتم تنفيذه باستخدام وقت النظام / Will use system time
        return Date(2024, 1, 1);
    }
};

/// وضع منتقي التاريخ / Date picker mode
enum class DatePickerMode {
    Calendar,   // تقويم / Calendar view
    Spinner,    // قوائم دوارة / Spinner wheels
    Input       // إدخال يدوي / Manual input
};

/// منتقي التاريخ
/// Date picker - مشابه لـ Flutter DatePicker
class DatePicker : public Widget {
public:
    Date selectedDate;              // التاريخ المختار / Selected date
    Date firstDate;                 // أول تاريخ متاح / First available date
    Date lastDate;                  // آخر تاريخ متاح / Last available date
    Date initialDate;               // التاريخ الابتدائي / Initial date
    
    // إعدادات / Settings
    DatePickerMode mode;
    bool showTodayButton;           // إظهار زر اليوم / Show today button
    bool showHeader;                // إظهار الرأس / Show header
    std::string locale;             // اللغة / Locale
    Int32 firstDayOfWeek;           // أول يوم في الأسبوع (0=أحد) / First day of week
    
    // نمط / Style
    Color selectedColor;
    Color todayColor;
    Color headerColor;
    Color disabledColor;
    Float32 daySize;
    
    // أحداث / Events
    std::function<void(const Date&)> onDateChanged;
    std::function<bool(const Date&)> selectableDayPredicate;  // هل اليوم قابل للاختيار

    DatePicker()
        : firstDate(1900, 1, 1),
          lastDate(2100, 12, 31),
          mode(DatePickerMode::Calendar),
          showTodayButton(true),
          showHeader(true),
          locale("ar"),
          firstDayOfWeek(6),  // السبت / Saturday
          selectedColor(Color::Blue),
          todayColor(Color(33, 150, 243)),
          headerColor(Color::Blue),
          disabledColor(Color(200, 200, 200)),
          daySize(40) {
        selectedDate = Date::Today();
        initialDate = selectedDate;
    }

    /// اختيار تاريخ / Select date
    void SelectDate(const Date& date) {
        if (IsDateSelectable(date)) {
            selectedDate = date;
            if (onDateChanged) onDateChanged(selectedDate);
        }
    }

    /// هل التاريخ قابل للاختيار / Is date selectable
    bool IsDateSelectable(const Date& date) const {
        if (date < firstDate || lastDate < date) return false;
        if (selectableDayPredicate) return selectableDayPredicate(date);
        return true;
    }

    /// الانتقال للشهر التالي / Go to next month
    void NextMonth() {
        initialDate.month++;
        if (initialDate.month > 12) {
            initialDate.month = 1;
            initialDate.year++;
        }
    }

    /// الانتقال للشهر السابق / Go to previous month
    void PreviousMonth() {
        initialDate.month--;
        if (initialDate.month < 1) {
            initialDate.month = 12;
            initialDate.year--;
        }
    }

    /// الذهاب لليوم / Go to today
    void GoToToday() {
        SelectDate(Date::Today());
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 7. TimePicker - منتقي الوقت
// ============================================================================

/// بنية الوقت / Time structure
struct Time {
    Int32 hour;     // 0-23
    Int32 minute;   // 0-59
    Int32 second;   // 0-59
    
    Time() : hour(0), minute(0), second(0) {}
    Time(Int32 h, Int32 m, Int32 s = 0) : hour(h), minute(m), second(s) {}
    
    /// الوقت كنص (12 ساعة) / Time as string (12-hour)
    std::string ToString12() const {
        Int32 h = hour % 12;
        if (h == 0) h = 12;
        std::string period = hour < 12 ? "ص" : "م";
        return std::to_string(h) + ":" + 
               (minute < 10 ? "0" : "") + std::to_string(minute) + " " + period;
    }
    
    /// الوقت كنص (24 ساعة) / Time as string (24-hour)
    std::string ToString24() const {
        return (hour < 10 ? "0" : "") + std::to_string(hour) + ":" +
               (minute < 10 ? "0" : "") + std::to_string(minute);
    }
    
    /// الآن / Now
    static Time Now() {
        return Time(12, 0, 0);
    }
};

/// وضع منتقي الوقت / Time picker mode
enum class TimePickerMode {
    Dial,       // قرص / Dial
    Input,      // إدخال / Input
    Spinner     // قوائم دوارة / Spinner
};

/// منتقي الوقت
/// Time picker - مشابه لـ Flutter TimePicker
class TimePicker : public Widget {
public:
    Time selectedTime;              // الوقت المختار / Selected time
    
    // إعدادات / Settings
    TimePickerMode mode;
    bool use24HourFormat;           // تنسيق 24 ساعة / 24-hour format
    bool showSeconds;               // إظهار الثواني / Show seconds
    Int32 minuteInterval;           // فاصل الدقائق (1, 5, 10, 15, 30) / Minute interval
    
    // نمط / Style
    Color dialColor;
    Color handColor;
    Color selectedColor;
    Color textColor;
    Float32 dialRadius;
    
    // أحداث / Events
    std::function<void(const Time&)> onTimeChanged;

    TimePicker()
        : mode(TimePickerMode::Dial),
          use24HourFormat(false),
          showSeconds(false),
          minuteInterval(1),
          dialColor(Color(240, 240, 240)),
          handColor(Color::Blue),
          selectedColor(Color::Blue),
          textColor(Color::Black),
          dialRadius(100) {
        selectedTime = Time::Now();
    }

    /// اختيار الوقت / Select time
    void SelectTime(const Time& time) {
        selectedTime = time;
        if (onTimeChanged) onTimeChanged(selectedTime);
    }

    /// تعيين الساعة / Set hour
    void SetHour(Int32 hour) {
        selectedTime.hour = hour % 24;
        if (onTimeChanged) onTimeChanged(selectedTime);
    }

    /// تعيين الدقيقة / Set minute
    void SetMinute(Int32 minute) {
        selectedTime.minute = (minute / minuteInterval) * minuteInterval;
        if (onTimeChanged) onTimeChanged(selectedTime);
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 8. DateTimePicker - منتقي التاريخ والوقت
// ============================================================================

/// بنية التاريخ والوقت / DateTime structure
struct DateTime {
    Date date;
    Time time;
    
    DateTime() {}
    DateTime(const Date& d, const Time& t) : date(d), time(t) {}
    
    std::string ToString() const {
        return date.ToString() + " " + time.ToString24();
    }
    
    static DateTime Now() {
        return DateTime(Date::Today(), Time::Now());
    }
};

/// منتقي التاريخ والوقت
/// DateTime picker combining date and time
class DateTimePicker : public Widget {
public:
    DateTime selectedDateTime;
    
    // مكونات فرعية / Sub components
    DatePicker datePicker;
    TimePicker timePicker;
    
    // إعدادات / Settings
    bool showDateFirst;             // إظهار التاريخ أولاً / Show date first
    
    // أحداث / Events
    std::function<void(const DateTime&)> onDateTimeChanged;

    DateTimePicker()
        : showDateFirst(true) {
        selectedDateTime = DateTime::Now();
        
        // ربط الأحداث / Link events
        datePicker.onDateChanged = [this](const Date& d) {
            selectedDateTime.date = d;
            if (onDateTimeChanged) onDateTimeChanged(selectedDateTime);
        };
        
        timePicker.onTimeChanged = [this](const Time& t) {
            selectedDateTime.time = t;
            if (onDateTimeChanged) onDateTimeChanged(selectedDateTime);
        };
    }

    void Update(Float32 deltaTime) override {
        datePicker.Update(deltaTime);
        timePicker.Update(deltaTime);
    }

    void Draw() override {
        if (showDateFirst) {
            datePicker.Draw();
            timePicker.Draw();
        } else {
            timePicker.Draw();
            datePicker.Draw();
        }
    }
};

// ============================================================================
// 9. ColorPicker - منتقي الألوان
// ============================================================================

/// وضع منتقي الألوان / Color picker mode
enum class ColorPickerMode {
    Wheel,      // عجلة / Color wheel
    Slider,     // شرائح / Sliders (RGB/HSV)
    Palette,    // لوحة / Palette grid
    Material    // ألوان Material / Material palette
};

/// منتقي الألوان
/// Color picker - مشابه لـ Flutter ColorPicker
class ColorPicker : public Widget {
public:
    Color selectedColor;            // اللون المختار / Selected color
    
    // إعدادات / Settings
    ColorPickerMode mode;
    bool showAlpha;                 // إظهار الشفافية / Show alpha
    bool showHex;                   // إظهار قيمة HEX / Show HEX value
    bool showRGB;                   // إظهار RGB / Show RGB values
    bool showHistory;               // إظهار السجل / Show history
    
    // سجل الألوان / Color history
    std::vector<Color> recentColors;
    Int32 maxHistory;
    
    // ألوان مخصصة / Custom colors
    std::vector<Color> customPalette;
    
    // نمط / Style
    Float32 wheelSize;
    Float32 sliderHeight;
    
    // أحداث / Events
    std::function<void(const Color&)> onColorChanged;

    ColorPicker()
        : selectedColor(Color::Blue),
          mode(ColorPickerMode::Wheel),
          showAlpha(true),
          showHex(true),
          showRGB(true),
          showHistory(true),
          maxHistory(10),
          wheelSize(200),
          sliderHeight(30) {}

    /// اختيار لون / Select color
    void SelectColor(const Color& color) {
        // إضافة للسجل / Add to history
        if (showHistory && recentColors.size() < static_cast<size_t>(maxHistory)) {
            recentColors.insert(recentColors.begin(), selectedColor);
        }
        
        selectedColor = color;
        if (onColorChanged) onColorChanged(selectedColor);
    }

    /// تعيين من HEX / Set from HEX
    void SetFromHex(const std::string& hex) {
        selectedColor = Color::FromHex(hex);
        if (onColorChanged) onColorChanged(selectedColor);
    }

    /// تعيين RGB / Set RGB
    void SetRGB(int r, int g, int b, int a = 255) {
        selectedColor = Color::FromBytes(r, g, b, a);
        if (onColorChanged) onColorChanged(selectedColor);
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 10. FilePicker - منتقي الملفات
// ============================================================================

/// نوع الملف / File type
enum class FilePickerType {
    Any,        // أي ملف / Any file
    Image,      // صورة / Image
    Video,      // فيديو / Video
    Audio,      // صوت / Audio
    Document,   // مستند / Document
    Custom      // مخصص / Custom
};

/// معلومات الملف / File info
struct FileInfo {
    std::string path;           // المسار / Path
    std::string name;           // الاسم / Name
    std::string extension;      // الامتداد / Extension
    UInt64 size;                // الحجم / Size in bytes
    std::string mimeType;       // نوع MIME / MIME type
    
    FileInfo() : size(0) {}
};

/// منتقي الملفات
/// File picker dialog
class FilePicker : public Widget {
public:
    // إعدادات / Settings
    FilePickerType type;
    std::vector<std::string> allowedExtensions;
    bool allowMultiple;             // السماح باختيار متعدد / Allow multiple
    UInt64 maxSizeBytes;            // الحد الأقصى للحجم / Max size
    std::string initialDirectory;   // المجلد الابتدائي / Initial directory
    
    // نتيجة / Result
    std::vector<FileInfo> selectedFiles;
    
    // أحداث / Events
    std::function<void(const std::vector<FileInfo>&)> onFilePicked;
    std::function<void()> onCancelled;
    std::function<void(const std::string&)> onError;

    FilePicker()
        : type(FilePickerType::Any),
          allowMultiple(false),
          maxSizeBytes(0) {}

    /// فتح منتقي الملفات / Open file picker
    void Open() {
        // سيتم تنفيذه حسب المنصة / Platform-specific implementation
    }

    /// تعيين الامتدادات المسموحة / Set allowed extensions
    FilePicker& SetAllowedExtensions(const std::vector<std::string>& extensions) {
        allowedExtensions = extensions;
        return *this;
    }

    /// تعيين نوع الملف / Set file type
    FilePicker& SetType(FilePickerType t) {
        type = t;
        switch (t) {
            case FilePickerType::Image:
                allowedExtensions = {"jpg", "jpeg", "png", "gif", "webp", "bmp"};
                break;
            case FilePickerType::Video:
                allowedExtensions = {"mp4", "avi", "mov", "mkv", "webm"};
                break;
            case FilePickerType::Audio:
                allowedExtensions = {"mp3", "wav", "ogg", "flac", "aac"};
                break;
            case FilePickerType::Document:
                allowedExtensions = {"pdf", "doc", "docx", "txt", "xls", "xlsx"};
                break;
            default:
                break;
        }
        return *this;
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 11. ImagePicker - منتقي الصور
// ============================================================================

/// مصدر الصورة / Image source
enum class ImageSource {
    Gallery,    // المعرض / Gallery
    Camera,     // الكاميرا / Camera
    Both        // كلاهما / Both (show chooser)
};

/// جودة الصورة / Image quality
enum class ImageQuality {
    Low,        // منخفضة / Low
    Medium,     // متوسطة / Medium
    High,       // عالية / High
    Original    // أصلية / Original
};

/// منتقي الصور
/// Image picker with camera and gallery support
class ImagePicker : public Widget {
public:
    // إعدادات / Settings
    ImageSource source;
    ImageQuality quality;
    Int32 maxWidth;                 // العرض الأقصى / Max width
    Int32 maxHeight;                // الارتفاع الأقصى / Max height
    bool allowCrop;                 // السماح بالقص / Allow crop
    Float32 cropAspectRatio;        // نسبة عرض القص / Crop aspect ratio (0 = free)
    bool allowMultiple;             // اختيار متعدد / Multiple selection
    Int32 maxImages;                // الحد الأقصى للصور / Max images
    
    // نتيجة / Result
    std::vector<FileInfo> selectedImages;
    
    // أحداث / Events
    std::function<void(const std::vector<FileInfo>&)> onImagePicked;
    std::function<void()> onCancelled;
    std::function<void(const std::string&)> onError;

    ImagePicker()
        : source(ImageSource::Both),
          quality(ImageQuality::High),
          maxWidth(0),
          maxHeight(0),
          allowCrop(false),
          cropAspectRatio(0),
          allowMultiple(false),
          maxImages(10) {}

    /// فتح منتقي الصور / Open image picker
    void Open() {
        // سيتم تنفيذه حسب المنصة / Platform-specific implementation
    }

    /// فتح الكاميرا / Open camera
    void OpenCamera() {
        source = ImageSource::Camera;
        Open();
    }

    /// فتح المعرض / Open gallery
    void OpenGallery() {
        source = ImageSource::Gallery;
        Open();
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 12. Autocomplete - الإكمال التلقائي
// ============================================================================

/// عنصر الإكمال التلقائي / Autocomplete item
struct AutocompleteItem {
    std::string value;              // القيمة / Value
    std::string label;              // التسمية / Label
    std::string subtitle;           // عنوان فرعي / Subtitle
    std::optional<UInt32> icon;     // رمز / Icon
    
    AutocompleteItem() {}
    AutocompleteItem(const std::string& v) : value(v), label(v) {}
    AutocompleteItem(const std::string& v, const std::string& l) : value(v), label(l) {}
};

/// الإكمال التلقائي
/// Autocomplete text field with suggestions
class Autocomplete : public Widget {
public:
    std::string text;                   // النص الحالي / Current text
    TextField textField;                // حقل النص / Text field
    
    // الاقتراحات / Suggestions
    std::vector<AutocompleteItem> suggestions;
    std::vector<AutocompleteItem> filteredSuggestions;
    Int32 maxSuggestions;               // الحد الأقصى للاقتراحات / Max suggestions
    
    // إعدادات / Settings
    bool showSuggestionsOnEmpty;        // إظهار الاقتراحات عند الفراغ / Show on empty
    Float32 debounceMs;                 // تأخير الفلترة / Filter delay
    Float32 suggestionHeight;           // ارتفاع الاقتراح / Suggestion height
    
    // نمط / Style
    Color suggestionBgColor;
    Color suggestionHoverColor;
    Color suggestionTextColor;
    Float32 maxDropdownHeight;
    
    // حالة / State
    bool isDropdownOpen;                // القائمة مفتوحة / Dropdown open
    Int32 highlightedIndex;             // الفهرس المميز / Highlighted index
    
    // أحداث / Events
    std::function<void(const std::string&)> onTextChanged;
    std::function<void(const AutocompleteItem&)> onSelected;
    std::function<std::vector<AutocompleteItem>(const std::string&)> optionsBuilder;

    Autocomplete()
        : maxSuggestions(5),
          showSuggestionsOnEmpty(false),
          debounceMs(300),
          suggestionHeight(48),
          suggestionBgColor(Color::White),
          suggestionHoverColor(Color(240, 240, 240)),
          suggestionTextColor(Color::Black),
          maxDropdownHeight(200),
          isDropdownOpen(false),
          highlightedIndex(-1) {
        
        // ربط حقل النص / Link text field
        textField.onChanged = [this](const std::string& value) {
            text = value;
            FilterSuggestions();
            if (onTextChanged) onTextChanged(value);
        };
    }

    /// فلترة الاقتراحات / Filter suggestions
    void FilterSuggestions() {
        filteredSuggestions.clear();
        
        if (text.empty() && !showSuggestionsOnEmpty) {
            isDropdownOpen = false;
            return;
        }
        
        // استخدام الباني المخصص / Use custom builder
        if (optionsBuilder) {
            filteredSuggestions = optionsBuilder(text);
        } else {
            // فلترة بسيطة / Simple filter
            for (const auto& item : suggestions) {
                if (item.label.find(text) != std::string::npos) {
                    filteredSuggestions.push_back(item);
                    if (filteredSuggestions.size() >= static_cast<size_t>(maxSuggestions)) {
                        break;
                    }
                }
            }
        }
        
        isDropdownOpen = !filteredSuggestions.empty();
        highlightedIndex = -1;
    }

    /// اختيار عنصر / Select item
    void SelectItem(Int32 index) {
        if (index >= 0 && index < static_cast<Int32>(filteredSuggestions.size())) {
            text = filteredSuggestions[index].value;
            textField.SetText(text);
            isDropdownOpen = false;
            if (onSelected) onSelected(filteredSuggestions[index]);
        }
    }

    /// التنقل للأعلى / Navigate up
    void NavigateUp() {
        if (highlightedIndex > 0) highlightedIndex--;
    }

    /// التنقل للأسفل / Navigate down
    void NavigateDown() {
        if (highlightedIndex < static_cast<Int32>(filteredSuggestions.size()) - 1) {
            highlightedIndex++;
        }
    }

    void Update(Float32 deltaTime) override {
        textField.Update(deltaTime);
    }

    void Draw() override {
        textField.Draw();
        if (isDropdownOpen) {
            // رسم قائمة الاقتراحات / Draw suggestions dropdown
        }
    }
};

// ============================================================================
// 13. Dropdown - القائمة المنسدلة
// ============================================================================

/// عنصر القائمة المنسدلة / Dropdown item
template<typename T>
struct DropdownItem {
    T value;                        // القيمة / Value
    std::string label;              // التسمية / Label
    bool enabled;                   // ممكّن / Enabled
    std::optional<UInt32> icon;     // رمز / Icon
    
    DropdownItem() : enabled(true) {}
    DropdownItem(const T& v, const std::string& l) : value(v), label(l), enabled(true) {}
};

/// القائمة المنسدلة
/// Dropdown select - مشابه لـ Flutter DropdownButton
template<typename T>
class Dropdown : public Widget {
public:
    std::vector<DropdownItem<T>> items;
    std::optional<T> selectedValue;
    
    // إعدادات / Settings
    std::string hint;                   // تلميح / Hint
    std::string disabledHint;           // تلميح عند التعطيل / Disabled hint
    bool isDense;                       // مكثف / Dense
    bool isExpanded;                    // ممتد / Expanded
    Float32 itemHeight;                 // ارتفاع العنصر / Item height
    Float32 dropdownMaxHeight;          // الارتفاع الأقصى / Max dropdown height
    
    // نمط / Style
    Color backgroundColor;
    Color dropdownColor;
    Color iconColor;
    Color textColor;
    Float32 borderRadius;
    EdgeInsets padding;
    
    // حالة / State
    bool isOpen;                        // مفتوح / Open
    Int32 highlightedIndex;
    
    // أحداث / Events
    std::function<void(const T&)> onChanged;

    Dropdown()
        : isDense(false),
          isExpanded(true),
          itemHeight(48),
          dropdownMaxHeight(300),
          backgroundColor(Color::White),
          dropdownColor(Color::White),
          iconColor(Color::Gray),
          textColor(Color::Black),
          borderRadius(4),
          padding(EdgeInsets::Symmetric(8, 12)),
          isOpen(false),
          highlightedIndex(-1) {}

    /// إضافة عنصر / Add item
    Dropdown& AddItem(const T& value, const std::string& label) {
        items.push_back(DropdownItem<T>(value, label));
        return *this;
    }

    /// اختيار قيمة / Select value
    void Select(const T& value) {
        selectedValue = value;
        isOpen = false;
        if (onChanged) onChanged(value);
    }

    /// الحصول على التسمية المختارة / Get selected label
    std::string GetSelectedLabel() const {
        if (!selectedValue) return hint;
        
        for (const auto& item : items) {
            if (item.value == *selectedValue) {
                return item.label;
            }
        }
        return hint;
    }

    /// فتح/إغلاق / Toggle
    void Toggle() {
        isOpen = !isOpen;
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 14. MultiSelect - الاختيار المتعدد
// ============================================================================

/// الاختيار المتعدد
/// Multi-select dropdown with chips
template<typename T>
class MultiSelect : public Widget {
public:
    std::vector<DropdownItem<T>> items;
    std::vector<T> selectedValues;
    
    // إعدادات / Settings
    std::string hint;
    Int32 maxSelections;                // الحد الأقصى للاختيارات / Max selections (0 = unlimited)
    bool showChips;                     // إظهار الشرائح / Show chips
    bool searchable;                    // قابل للبحث / Searchable
    
    // نمط / Style
    Color chipColor;
    Color chipTextColor;
    Float32 chipSpacing;
    
    // حالة / State
    bool isOpen;
    std::string searchQuery;
    
    // أحداث / Events
    std::function<void(const std::vector<T>&)> onChanged;

    MultiSelect()
        : maxSelections(0),
          showChips(true),
          searchable(true),
          chipColor(Color::Blue),
          chipTextColor(Color::White),
          chipSpacing(8),
          isOpen(false) {}

    /// تبديل الاختيار / Toggle selection
    void ToggleSelection(const T& value) {
        auto it = std::find(selectedValues.begin(), selectedValues.end(), value);
        
        if (it != selectedValues.end()) {
            // إزالة / Remove
            selectedValues.erase(it);
        } else {
            // إضافة / Add
            if (maxSelections == 0 || selectedValues.size() < static_cast<size_t>(maxSelections)) {
                selectedValues.push_back(value);
            }
        }
        
        if (onChanged) onChanged(selectedValues);
    }

    /// هل محدد / Is selected
    bool IsSelected(const T& value) const {
        return std::find(selectedValues.begin(), selectedValues.end(), value) != selectedValues.end();
    }

    /// مسح الكل / Clear all
    void ClearAll() {
        selectedValues.clear();
        if (onChanged) onChanged(selectedValues);
    }

    /// تحديد الكل / Select all
    void SelectAll() {
        selectedValues.clear();
        for (const auto& item : items) {
            if (maxSelections == 0 || selectedValues.size() < static_cast<size_t>(maxSelections)) {
                selectedValues.push_back(item.value);
            }
        }
        if (onChanged) onChanged(selectedValues);
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

// ============================================================================
// 15. RatingInput - إدخال التقييم
// ============================================================================

/// نوع رمز التقييم / Rating icon type
enum class RatingIconType {
    Star,       // نجمة / Star
    Heart,      // قلب / Heart
    Circle,     // دائرة / Circle
    Custom      // مخصص / Custom
};

/// إدخال التقييم
/// Rating input with stars/hearts - مشابه لـ Flutter RatingBar
class RatingInput : public Widget {
public:
    Float32 rating;                 // التقييم الحالي / Current rating
    Float32 maxRating;              // الحد الأقصى / Max rating
    
    // إعدادات / Settings
    bool allowHalfRating;           // السماح بنصف التقييم / Allow half rating
    bool readOnly;                  // قراءة فقط / Read only
    Int32 itemCount;                // عدد العناصر / Item count
    Float32 itemSize;               // حجم العنصر / Item size
    Float32 itemSpacing;            // المسافة بين العناصر / Item spacing
    RatingIconType iconType;        // نوع الرمز / Icon type
    
    // نمط / Style
    Color filledColor;              // لون الممتلئ / Filled color
    Color emptyColor;               // لون الفارغ / Empty color
    Color halfFilledColor;          // لون النصف ممتلئ / Half filled color
    
    // أحداث / Events
    std::function<void(Float32)> onRatingChanged;

    RatingInput()
        : rating(0),
          maxRating(5),
          allowHalfRating(true),
          readOnly(false),
          itemCount(5),
          itemSize(32),
          itemSpacing(4),
          iconType(RatingIconType::Star),
          filledColor(Color(255, 193, 7)),  // ذهبي / Gold
          emptyColor(Color(189, 189, 189)),  // رمادي / Gray
          halfFilledColor(Color(255, 193, 7)) {}

    /// تعيين التقييم / Set rating
    void SetRating(Float32 value) {
        if (readOnly) return;
        
        // تقريب إذا لم يكن نصف التقييم مسموحاً / Round if half rating not allowed
        if (!allowHalfRating) {
            value = std::round(value);
        } else {
            value = std::round(value * 2.0f) / 2.0f;  // تقريب لأقرب 0.5
        }
        
        // حدود / Clamp
        if (value < 0) value = 0;
        if (value > maxRating) value = maxRating;
        
        rating = value;
        if (onRatingChanged) onRatingChanged(rating);
    }

    /// زيادة التقييم / Increase rating
    void Increase(Float32 amount = 1.0f) {
        SetRating(rating + amount);
    }

    /// تقليل التقييم / Decrease rating
    void Decrease(Float32 amount = 1.0f) {
        SetRating(rating - amount);
    }

    /// إعادة تعيين / Reset
    void Reset() {
        SetRating(0);
    }

    void Update(Float32 deltaTime) override {}
    void Draw() override {}
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MOBILE_INPUT_H
