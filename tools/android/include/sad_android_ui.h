// ═══════════════════════════════════════════════════════════════════════════
// sad_android_ui.h - نظام واجهة المستخدم لأندرويد
// ═══════════════════════════════════════════════════════════════════════════
//
// يوفر هذا الملف نظام بناء واجهة المستخدم من كود ص وتحويله إلى JSON
// يتم إرسال JSON إلى Kotlin لتوليد Compose UI
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef SAD_ANDROID_UI_H
#define SAD_ANDROID_UI_H

#ifdef SAD_ANDROID

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace sad {
namespace android {
namespace ui {

// ═══════════════════════════════════════════════════════════════════════════
// أنواع عناصر UI
// ═══════════════════════════════════════════════════════════════════════════

enum class WidgetType {
    // الحاويات
    COLUMN,         // عمود
    ROW,            // صف  
    BOX,            // صندوق
    CARD,           // بطاقة
    SCROLL,         // تمرير
    LIST,           // قائمة
    GRID,           // شبكة
    
    // العناصر الأساسية
    TEXT,           // نص
    BUTTON,         // زر
    INPUT,          // حقل إدخال
    IMAGE,          // صورة
    ICON,           // أيقونة
    SPACER,         // فاصل
    DIVIDER,        // خط فاصل
    
    // عناصر الإدخال
    CHECKBOX,       // مربع اختيار
    SWITCH,         // مفتاح
    SLIDER,         // منزلق
    DROPDOWN,       // قائمة منسدلة
    RADIO,          // زر راديو
    
    // عناصر متقدمة
    PROGRESS,       // شريط تقدم
    CIRCULAR_PROGRESS,  // تقدم دائري
    DIALOG,         // حوار
    SNACKBAR,       // شريط إشعار
    TABS,           // علامات تبويب
    
    // الرسوميات
    CANVAS,         // لوحة رسم
    CHART           // مخطط
};

// ═══════════════════════════════════════════════════════════════════════════
// خصائص العناصر
// ═══════════════════════════════════════════════════════════════════════════

struct Color {
    int r = 0, g = 0, b = 0, a = 255;
    
    Color() = default;
    Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
    
    static Color fromHex(const std::string& hex);
    std::string toHex() const;
    std::string toJson() const;
};

struct Padding {
    int top = 0, right = 0, bottom = 0, left = 0;
    
    Padding() = default;
    Padding(int all) : top(all), right(all), bottom(all), left(all) {}
    Padding(int vertical, int horizontal) 
        : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    Padding(int t, int r, int b, int l) : top(t), right(r), bottom(b), left(l) {}
    
    std::string toJson() const;
};

struct Size {
    enum class Type { FIXED, WRAP, FILL, PERCENT };
    Type type = Type::WRAP;
    float value = 0;
    
    static Size fixed(float v) { return {Type::FIXED, v}; }
    static Size wrap() { return {Type::WRAP, 0}; }
    static Size fill() { return {Type::FILL, 1.0f}; }
    static Size percent(float p) { return {Type::PERCENT, p}; }
    
    std::string toJson() const;
};

enum class Alignment {
    START, CENTER, END, 
    TOP, BOTTOM,
    TOP_START, TOP_CENTER, TOP_END,
    CENTER_START, CENTER_CENTER, CENTER_END,
    BOTTOM_START, BOTTOM_CENTER, BOTTOM_END
};

enum class FontWeight {
    THIN, LIGHT, NORMAL, MEDIUM, SEMI_BOLD, BOLD, EXTRA_BOLD, BLACK
};

// ═══════════════════════════════════════════════════════════════════════════
// معالجات الأحداث
// ═══════════════════════════════════════════════════════════════════════════

using ClickHandler = std::function<void()>;
using TextChangeHandler = std::function<void(const std::string&)>;
using ValueChangeHandler = std::function<void(double)>;
using BoolChangeHandler = std::function<void(bool)>;

// ═══════════════════════════════════════════════════════════════════════════
// الفئة الأساسية للعناصر
// ═══════════════════════════════════════════════════════════════════════════

class Widget {
public:
    virtual ~Widget() = default;
    
    // المعرف الفريد
    std::string id;
    
    // الخصائص المشتركة
    Size width = Size::wrap();
    Size height = Size::wrap();
    Padding padding;
    Padding margin;
    Color backgroundColor;
    float cornerRadius = 0;
    bool visible = true;
    bool enabled = true;
    float elevation = 0;
    
    // التحويل إلى JSON
    virtual std::string toJson() const = 0;
    virtual WidgetType getType() const = 0;
    
protected:
    std::string basePropsJson() const;
};

// ═══════════════════════════════════════════════════════════════════════════
// عناصر الحاويات
// ═══════════════════════════════════════════════════════════════════════════

class ContainerWidget : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> children;
    Alignment alignment = Alignment::TOP_START;
    int spacing = 0;
    
    void addChild(std::shared_ptr<Widget> child) {
        children.push_back(child);
    }
    
protected:
    std::string childrenJson() const;
};

class Column : public ContainerWidget {
public:
    WidgetType getType() const override { return WidgetType::COLUMN; }
    std::string toJson() const override;
};

class Row : public ContainerWidget {
public:
    WidgetType getType() const override { return WidgetType::ROW; }
    std::string toJson() const override;
};

class Box : public ContainerWidget {
public:
    WidgetType getType() const override { return WidgetType::BOX; }
    std::string toJson() const override;
};

class Card : public ContainerWidget {
public:
    float elevation = 4;
    WidgetType getType() const override { return WidgetType::CARD; }
    std::string toJson() const override;
};

class ScrollView : public ContainerWidget {
public:
    bool horizontal = false;
    WidgetType getType() const override { return WidgetType::SCROLL; }
    std::string toJson() const override;
};

// ═══════════════════════════════════════════════════════════════════════════
// عناصر النص والعرض
// ═══════════════════════════════════════════════════════════════════════════

class Text : public Widget {
public:
    std::string content;
    float fontSize = 16;
    FontWeight fontWeight = FontWeight::NORMAL;
    Color color = Color(0, 0, 0);
    Alignment textAlign = Alignment::START;
    int maxLines = 0;  // 0 = unlimited
    bool selectable = false;
    
    WidgetType getType() const override { return WidgetType::TEXT; }
    std::string toJson() const override;
};

class Image : public Widget {
public:
    std::string source;  // URL أو اسم ملف محلي
    enum class Fit { FILL, CONTAIN, COVER, NONE };
    Fit fit = Fit::CONTAIN;
    
    WidgetType getType() const override { return WidgetType::IMAGE; }
    std::string toJson() const override;
};

class Icon : public Widget {
public:
    std::string name;  // اسم الأيقونة من Material Icons
    float size = 24;
    Color color = Color(0, 0, 0);
    
    WidgetType getType() const override { return WidgetType::ICON; }
    std::string toJson() const override;
};

class Spacer : public Widget {
public:
    WidgetType getType() const override { return WidgetType::SPACER; }
    std::string toJson() const override;
};

class Divider : public Widget {
public:
    Color color = Color(200, 200, 200);
    float thickness = 1;
    
    WidgetType getType() const override { return WidgetType::DIVIDER; }
    std::string toJson() const override;
};

// ═══════════════════════════════════════════════════════════════════════════
// عناصر التفاعل
// ═══════════════════════════════════════════════════════════════════════════

class Button : public Widget {
public:
    std::string label;
    std::string iconName;
    Color color = Color(33, 150, 243);  // Material Blue
    Color textColor = Color(255, 255, 255);
    ClickHandler onClick;
    std::string onClickHandler;  // اسم الدالة في ص
    
    enum class Style { FILLED, OUTLINED, TEXT };
    Style style = Style::FILLED;
    
    WidgetType getType() const override { return WidgetType::BUTTON; }
    std::string toJson() const override;
};

class TextField : public Widget {
public:
    std::string value;
    std::string placeholder;
    std::string label;
    bool password = false;
    bool multiline = false;
    int maxLength = 0;
    TextChangeHandler onTextChange;
    std::string onChangeHandler;
    
    enum class Type { TEXT, NUMBER, EMAIL, PHONE, URL };
    Type inputType = Type::TEXT;
    
    WidgetType getType() const override { return WidgetType::INPUT; }
    std::string toJson() const override;
};

class Checkbox : public Widget {
public:
    bool checked = false;
    std::string label;
    BoolChangeHandler onCheckedChange;
    std::string onChangeHandler;
    
    WidgetType getType() const override { return WidgetType::CHECKBOX; }
    std::string toJson() const override;
};

class Switch : public Widget {
public:
    bool checked = false;
    std::string label;
    BoolChangeHandler onCheckedChange;
    std::string onChangeHandler;
    
    WidgetType getType() const override { return WidgetType::SWITCH; }
    std::string toJson() const override;
};

class Slider : public Widget {
public:
    double value = 0;
    double min = 0;
    double max = 100;
    double step = 1;
    ValueChangeHandler onValueChange;
    std::string onChangeHandler;
    
    WidgetType getType() const override { return WidgetType::SLIDER; }
    std::string toJson() const override;
};

class Dropdown : public Widget {
public:
    std::vector<std::string> options;
    int selectedIndex = -1;
    std::string placeholder;
    TextChangeHandler onSelectionChange;
    std::string onChangeHandler;
    
    WidgetType getType() const override { return WidgetType::DROPDOWN; }
    std::string toJson() const override;
};

// ═══════════════════════════════════════════════════════════════════════════
// عناصر التقدم
// ═══════════════════════════════════════════════════════════════════════════

class ProgressBar : public Widget {
public:
    double progress = 0;  // 0.0 - 1.0
    bool indeterminate = false;
    Color color = Color(33, 150, 243);
    
    WidgetType getType() const override { return WidgetType::PROGRESS; }
    std::string toJson() const override;
};

class CircularProgress : public Widget {
public:
    double progress = 0;
    bool indeterminate = false;
    float strokeWidth = 4;
    Color color = Color(33, 150, 243);
    
    WidgetType getType() const override { return WidgetType::CIRCULAR_PROGRESS; }
    std::string toJson() const override;
};

// ═══════════════════════════════════════════════════════════════════════════
// مدير واجهة المستخدم
// ═══════════════════════════════════════════════════════════════════════════

class UIManager {
public:
    static UIManager& getInstance() {
        static UIManager instance;
        return instance;
    }
    
    // تعيين الجذر
    void setRoot(std::shared_ptr<Widget> root);
    std::shared_ptr<Widget> getRoot() const { return rootWidget_; }
    
    // البحث عن عنصر
    std::shared_ptr<Widget> findById(const std::string& id);
    
    // تحويل UI إلى JSON
    std::string toJson() const;
    
    // تحديث من JSON (من Kotlin)
    void updateFromJson(const std::string& json);
    
    // معالجة الأحداث
    void handleButtonClick(const std::string& buttonId);
    void handleTextChange(const std::string& inputId, const std::string& text);
    void handleValueChange(const std::string& widgetId, double value);
    void handleBoolChange(const std::string& widgetId, bool value);
    
    // تسجيل معالجات
    void registerClickHandler(const std::string& id, ClickHandler handler);
    void registerTextHandler(const std::string& id, TextChangeHandler handler);
    
private:
    UIManager() = default;
    
    std::shared_ptr<Widget> rootWidget_;
    std::map<std::string, std::shared_ptr<Widget>> widgetMap_;
    std::map<std::string, ClickHandler> clickHandlers_;
    std::map<std::string, TextChangeHandler> textHandlers_;
    
    void buildWidgetMap(std::shared_ptr<Widget> widget);
};

// ═══════════════════════════════════════════════════════════════════════════
// مساعدات البناء
// ═══════════════════════════════════════════════════════════════════════════

// بناء سريع للعناصر
std::shared_ptr<Column> column();
std::shared_ptr<Row> row();
std::shared_ptr<Text> text(const std::string& content);
std::shared_ptr<Button> button(const std::string& label);
std::shared_ptr<TextField> textField(const std::string& placeholder = "");
std::shared_ptr<Image> image(const std::string& source);
std::shared_ptr<Spacer> spacer(int size = 16);

} // namespace ui
} // namespace android
} // namespace sad

#endif // SAD_ANDROID
#endif // SAD_ANDROID_UI_H
