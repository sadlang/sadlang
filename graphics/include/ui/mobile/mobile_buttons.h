// mobile_buttons.h - أزرار التطبيقات الهاتفية
// Mobile Button Components
//
// الوصف: مجموعة شاملة من الأزرار للتطبيقات الهاتفية
// Description: Comprehensive button collection for mobile apps
// مشابه لـ Flutter Material buttons
//
// المركبات: ElevatedButton, TextButton, OutlinedButton, IconButton,
//           FloatingActionButton, ExtendedFAB, ToggleButton, SegmentedButton,
//           DropdownButton, PopupMenuButton

#ifndef SAD_GRAPHICS_MOBILE_BUTTONS_H
#define SAD_GRAPHICS_MOBILE_BUTTONS_H

#include "mobile_core.h"
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <memory>
#include <algorithm>

namespace sad {
namespace graphics {
namespace ui {
namespace mobile {

// ============================================================================
// أنماط الأزرار / Button Styles
// ============================================================================

/// شكل الزر / Button shape
enum class ButtonShape {
    Rectangle,      // مستطيل / Rectangle
    RoundedRect,    // مستطيل دائري / Rounded rectangle
    Stadium,        // ملعب (نصف دائرة) / Stadium shape
    Circle,         // دائرة / Circle
    BeveledRect     // مستطيل مشطوف / Beveled rectangle
};

/// حجم الزر / Button size
enum class ButtonSize {
    Small,      // صغير / Small
    Medium,     // متوسط / Medium
    Large,      // كبير / Large
    Custom      // مخصص / Custom
};

/// نمط الزر المرئي / Visual button style
struct ButtonStyle {
    // الألوان / Colors
    std::optional<Color> backgroundColor;
    std::optional<Color> foregroundColor;
    std::optional<Color> overlayColor;      // لون التراكب عند الضغط / Overlay on press
    std::optional<Color> shadowColor;
    std::optional<Color> surfaceTintColor;
    
    // الأبعاد / Dimensions
    std::optional<Float32> elevation;
    std::optional<EdgeInsets> padding;
    std::optional<Float32> minimumWidth;
    std::optional<Float32> minimumHeight;
    std::optional<Float32> maximumWidth;
    std::optional<Float32> fixedWidth;
    std::optional<Float32> fixedHeight;
    
    // الشكل / Shape
    ButtonShape shape;
    std::optional<BorderRadius> borderRadius;
    std::optional<BorderSide> side;
    
    // النص / Text
    std::optional<Float32> fontSize;
    std::optional<std::string> fontFamily;
    bool textUppercase;
    Float32 letterSpacing;
    
    // الحركة / Animation
    Float32 animationDuration;  // بالمللي ثانية / In milliseconds
    
    ButtonStyle()
        : shape(ButtonShape::RoundedRect),
          textUppercase(false),
          letterSpacing(0),
          animationDuration(200) {}
    
    /// نسخ مع تعديل / Copy with modification
    ButtonStyle CopyWith(const ButtonStyle& other) const {
        ButtonStyle result = *this;
        if (other.backgroundColor) result.backgroundColor = other.backgroundColor;
        if (other.foregroundColor) result.foregroundColor = other.foregroundColor;
        if (other.elevation) result.elevation = other.elevation;
        if (other.padding) result.padding = other.padding;
        if (other.borderRadius) result.borderRadius = other.borderRadius;
        return result;
    }
};

// ============================================================================
// 26. ElevatedButton - زر مرتفع / Elevated Button
// ============================================================================

/// زر مرتفع بظل / Elevated button with shadow
/// مشابه لـ ElevatedButton في Flutter
class ElevatedButton : public Widget {
public:
    std::string text;
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> icon;
    std::function<void()> onPressed;
    std::function<void()> onLongPress;
    ButtonStyle style;
    bool enabled;
    bool loading;  // حالة التحميل / Loading state
    
    ElevatedButton()
        : enabled(true), loading(false) {
        // إعدادات افتراضية / Default settings
        style.backgroundColor = Color(98, 0, 238);  // Primary
        style.foregroundColor = Color::White;
        style.elevation = 2.0f;
        style.borderRadius = BorderRadius::Circular(4);
        style.padding = EdgeInsets::Symmetric(16, 8);
    }
    
    /// تعيين النص / Set text
    ElevatedButton& SetText(const std::string& t) {
        text = t;
        return *this;
    }
    
    /// تعيين الأيقونة / Set icon
    ElevatedButton& SetIcon(std::shared_ptr<Widget> i) {
        icon = i;
        return *this;
    }
    
    /// تعيين دالة الضغط / Set on pressed
    ElevatedButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
    
    /// تعيين النمط / Set style
    ElevatedButton& SetStyle(const ButtonStyle& s) {
        style = s;
        return *this;
    }
    
    /// تعيين لون الخلفية / Set background color
    ElevatedButton& SetBackgroundColor(const Color& c) {
        style.backgroundColor = c;
        return *this;
    }
    
    /// مصنع مع أيقونة / Factory with icon
    static ElevatedButton WithIcon(const std::string& text,
                                   std::shared_ptr<Widget> icon) {
        ElevatedButton btn;
        btn.text = text;
        btn.icon = icon;
        return btn;
    }
};

// ============================================================================
// 27. TextButton - زر نصي / Text Button
// ============================================================================

/// زر نصي بدون ظل / Text button without shadow
/// مشابه لـ TextButton في Flutter
class TextButton : public Widget {
public:
    std::string text;
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> icon;
    std::function<void()> onPressed;
    std::function<void()> onLongPress;
    ButtonStyle style;
    bool enabled;
    
    TextButton() : enabled(true) {
        style.backgroundColor = Color::Transparent;
        style.foregroundColor = Color(98, 0, 238);  // Primary
        style.elevation = 0;
        style.padding = EdgeInsets::Symmetric(16, 8);
    }
    
    /// تعيين النص / Set text
    TextButton& SetText(const std::string& t) {
        text = t;
        return *this;
    }
    
    /// تعيين دالة الضغط / Set on pressed
    TextButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
    
    /// تعيين لون النص / Set text color
    TextButton& SetTextColor(const Color& c) {
        style.foregroundColor = c;
        return *this;
    }
};

// ============================================================================
// 28. OutlinedButton - زر محدد / Outlined Button
// ============================================================================

/// زر بحدود بدون خلفية / Button with border, no background
/// مشابه لـ OutlinedButton في Flutter
class OutlinedButton : public Widget {
public:
    std::string text;
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> icon;
    std::function<void()> onPressed;
    std::function<void()> onLongPress;
    ButtonStyle style;
    bool enabled;
    
    OutlinedButton() : enabled(true) {
        style.backgroundColor = Color::Transparent;
        style.foregroundColor = Color(98, 0, 238);
        style.side = BorderSide(Color(98, 0, 238), 1.0f);
        style.elevation = 0;
        style.borderRadius = BorderRadius::Circular(4);
        style.padding = EdgeInsets::Symmetric(16, 8);
    }
    
    /// تعيين النص / Set text
    OutlinedButton& SetText(const std::string& t) {
        text = t;
        return *this;
    }
    
    /// تعيين لون الحدود / Set border color
    OutlinedButton& SetBorderColor(const Color& c) {
        style.side = BorderSide(c, style.side->width);
        style.foregroundColor = c;
        return *this;
    }
    
    /// تعيين دالة الضغط / Set on pressed
    OutlinedButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
};

// ============================================================================
// 29. IconButton - زر أيقونة / Icon Button
// ============================================================================

/// زر دائري بأيقونة / Circular button with icon
/// مشابه لـ IconButton في Flutter
class IconButton : public Widget {
public:
    std::string iconName;
    std::shared_ptr<Widget> icon;
    std::function<void()> onPressed;
    Color iconColor;
    Float32 iconSize;
    Float32 splashRadius;
    std::optional<Color> backgroundColor;
    EdgeInsets padding;
    bool enabled;
    std::string tooltip;
    
    IconButton()
        : iconColor(Color::Black),
          iconSize(24.0f),
          splashRadius(20.0f),
          padding(EdgeInsets::All(8)),
          enabled(true) {}
    
    /// تعيين الأيقونة / Set icon
    IconButton& SetIcon(std::shared_ptr<Widget> i) {
        icon = i;
        return *this;
    }
    
    /// تعيين اسم الأيقونة / Set icon name
    IconButton& SetIconName(const std::string& name) {
        iconName = name;
        return *this;
    }
    
    /// تعيين دالة الضغط / Set on pressed
    IconButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
    
    /// تعيين اللون / Set color
    IconButton& SetColor(const Color& c) {
        iconColor = c;
        return *this;
    }
    
    /// تعيين الحجم / Set size
    IconButton& SetSize(Float32 s) {
        iconSize = s;
        return *this;
    }
    
    /// تعيين تلميح / Set tooltip
    IconButton& SetTooltip(const std::string& t) {
        tooltip = t;
        return *this;
    }
};

// ============================================================================
// 30. FloatingActionButton - زر عائم / Floating Action Button
// ============================================================================

/// زر إجراء عائم / Floating action button
/// مشابه لـ FloatingActionButton في Flutter
enum class FABSize {
    Small,      // صغير (40x40) / Small
    Regular,    // عادي (56x56) / Regular
    Large,      // كبير (96x96) / Large
    Extended    // ممتد / Extended (with label)
};

class FloatingActionButton : public Widget {
public:
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> icon;
    std::string label;      // للـ extended / For extended
    std::function<void()> onPressed;
    Color backgroundColor;
    Color foregroundColor;
    Float32 elevation;
    FABSize size;
    ButtonShape shape;
    std::string tooltip;
    std::string heroTag;    // للحركة بين الصفحات / For hero animation
    bool mini;
    bool isExtended;
    
    FloatingActionButton()
        : backgroundColor(Color(98, 0, 238)),
          foregroundColor(Color::White),
          elevation(6.0f),
          size(FABSize::Regular),
          shape(ButtonShape::Circle),
          mini(false),
          isExtended(false) {}
    
    /// تعيين الأيقونة / Set icon
    FloatingActionButton& SetIcon(std::shared_ptr<Widget> i) {
        icon = i;
        return *this;
    }
    
    /// تعيين دالة الضغط / Set on pressed
    FloatingActionButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
    
    /// إنشاء صغير / Create mini
    FloatingActionButton& SetMini(bool m = true) {
        mini = m;
        size = m ? FABSize::Small : FABSize::Regular;
        return *this;
    }
    
    /// إنشاء ممتد / Create extended
    static FloatingActionButton Extended(const std::string& label,
                                         std::shared_ptr<Widget> icon = nullptr) {
        FloatingActionButton fab;
        fab.label = label;
        fab.icon = icon;
        fab.isExtended = true;
        fab.size = FABSize::Extended;
        fab.shape = ButtonShape::Stadium;
        return fab;
    }
    
    /// تعيين الألوان / Set colors
    FloatingActionButton& SetColors(const Color& bg, const Color& fg) {
        backgroundColor = bg;
        foregroundColor = fg;
        return *this;
    }
};

// ============================================================================
// 31. ToggleButton - زر تبديل / Toggle Button
// ============================================================================

/// زر قابل للتبديل / Toggleable button
class ToggleButton : public Widget {
public:
    std::shared_ptr<Widget> child;
    bool isSelected;
    std::function<void(bool)> onChanged;
    Color selectedColor;
    Color unselectedColor;
    Color selectedBorderColor;
    Color unselectedBorderColor;
    Float32 borderWidth;
    BorderRadius borderRadius;
    bool enabled;
    
    ToggleButton()
        : isSelected(false),
          selectedColor(Color(98, 0, 238, 50)),
          unselectedColor(Color::Transparent),
          selectedBorderColor(Color(98, 0, 238)),
          unselectedBorderColor(Color(150, 150, 150)),
          borderWidth(1.0f),
          borderRadius(BorderRadius::Circular(4)),
          enabled(true) {}
    
    /// تعيين الحالة / Set selection state
    ToggleButton& SetSelected(bool s) {
        isSelected = s;
        return *this;
    }
    
    /// تبديل الحالة / Toggle state
    void Toggle() {
        isSelected = !isSelected;
        if (onChanged) onChanged(isSelected);
    }
    
    /// تعيين دالة التغيير / Set on changed
    ToggleButton& OnChanged(std::function<void(bool)> callback) {
        onChanged = callback;
        return *this;
    }
};

// ============================================================================
// 32. ToggleButtons - مجموعة أزرار تبديل / Toggle Buttons Group
// ============================================================================

/// مجموعة من أزرار التبديل / Group of toggle buttons
class ToggleButtons : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> children;
    std::vector<bool> isSelected;
    std::function<void(Int32)> onPressed;
    bool multiSelect;
    Color selectedColor;
    Color color;
    Color fillColor;
    Color selectedBorderColor;
    Color borderColor;
    Float32 borderWidth;
    BorderRadius borderRadius;
    bool renderBorder;
    Float32 spacing;
    
    ToggleButtons()
        : multiSelect(false),
          selectedColor(Color(98, 0, 238)),
          color(Color::Black),
          fillColor(Color(98, 0, 238, 30)),
          selectedBorderColor(Color(98, 0, 238)),
          borderColor(Color(150, 150, 150)),
          borderWidth(1.0f),
          borderRadius(BorderRadius::Circular(0)),
          renderBorder(true),
          spacing(0) {}
    
    /// إضافة زر / Add button
    ToggleButtons& AddButton(std::shared_ptr<Widget> child, bool selected = false) {
        children.push_back(child);
        isSelected.push_back(selected);
        return *this;
    }
    
    /// اختيار زر / Select button
    void Select(Int32 index) {
        if (index < 0 || index >= (Int32)isSelected.size()) return;
        
        if (!multiSelect) {
            // إلغاء اختيار الكل / Deselect all
            for (size_t i = 0; i < isSelected.size(); ++i) isSelected[i] = false;
        }
        isSelected[index] = !isSelected[index];
        
        if (onPressed) onPressed(index);
    }
    
    /// تعيين دالة الضغط / Set on pressed
    ToggleButtons& OnPressed(std::function<void(Int32)> callback) {
        onPressed = callback;
        return *this;
    }
};

// ============================================================================
// 33. SegmentedButton - زر مقسم / Segmented Button
// ============================================================================

/// زر مقسم لخيارات متعددة / Segmented button for multiple options
/// مشابه لـ SegmentedButton في Flutter Material 3
template<typename T>
class SegmentedButton : public Widget {
public:
    struct Segment {
        T value;
        std::string label;
        std::shared_ptr<Widget> icon;
        bool enabled;
        
        Segment() : enabled(true) {}
        Segment(T v, const std::string& l, std::shared_ptr<Widget> i = nullptr)
            : value(v), label(l), icon(i), enabled(true) {}
    };
    
    std::vector<Segment> segments;
    std::vector<T> selected;
    std::function<void(const std::vector<T>&)> onSelectionChanged;
    bool multiSelect;
    bool emptySelectionAllowed;
    Color selectedBackgroundColor;
    Color backgroundColor;
    Color selectedForegroundColor;
    Color foregroundColor;
    bool showSelectedIcon;
    
    SegmentedButton()
        : multiSelect(false),
          emptySelectionAllowed(false),
          selectedBackgroundColor(Color(98, 0, 238, 30)),
          backgroundColor(Color::Transparent),
          selectedForegroundColor(Color(98, 0, 238)),
          foregroundColor(Color::Black),
          showSelectedIcon(true) {}
    
    /// إضافة قسم / Add segment
    SegmentedButton& AddSegment(T value, const std::string& label,
                               std::shared_ptr<Widget> icon = nullptr) {
        segments.push_back(Segment(value, label, icon));
        return *this;
    }
    
    /// اختيار قيمة / Select value
    void Select(T value) {
        if (!multiSelect) {
            selected.clear();
        }
        
        auto it = std::find(selected.begin(), selected.end(), value);
        if (it != selected.end()) {
            if (emptySelectionAllowed || selected.size() > 1) {
                selected.erase(it);
            }
        } else {
            selected.push_back(value);
        }
        
        if (onSelectionChanged) onSelectionChanged(selected);
    }
    
    /// هل القيمة مختارة؟ / Is value selected?
    bool IsSelected(T value) const {
        return std::find(selected.begin(), selected.end(), value) != selected.end();
    }
};

// ============================================================================
// 34. DropdownButton - زر قائمة منسدلة / Dropdown Button
// ============================================================================

/// زر بقائمة منسدلة / Button with dropdown menu
template<typename T>
class DropdownButton : public Widget {
public:
    struct DropdownItem {
        T value;
        std::string text;
        std::shared_ptr<Widget> child;
        bool enabled;
        
        DropdownItem() : enabled(true) {}
        DropdownItem(T v, const std::string& t) : value(v), text(t), enabled(true) {}
    };
    
    std::vector<DropdownItem> items;
    std::optional<T> value;
    std::function<void(T)> onChanged;
    std::shared_ptr<Widget> hint;
    std::shared_ptr<Widget> disabledHint;
    std::shared_ptr<Widget> underline;
    std::shared_ptr<Widget> icon;
    Color iconEnabledColor;
    Color iconDisabledColor;
    Float32 iconSize;
    Float32 elevation;
    Float32 itemHeight;
    Float32 menuMaxHeight;
    bool isDense;
    bool isExpanded;
    bool enabled;
    EdgeInsets padding;
    BorderRadius borderRadius;
    Color dropdownColor;
    Color focusColor;
    
    DropdownButton()
        : iconEnabledColor(Color::Black),
          iconDisabledColor(Color(150, 150, 150)),
          iconSize(24.0f),
          elevation(8.0f),
          itemHeight(48.0f),
          menuMaxHeight(300.0f),
          isDense(false),
          isExpanded(false),
          enabled(true),
          padding(EdgeInsets::Symmetric(16, 8)),
          borderRadius(BorderRadius::Circular(4)),
          dropdownColor(Color::White),
          focusColor(Color(0, 0, 0, 10)) {}
    
    /// إضافة عنصر / Add item
    DropdownButton& AddItem(T value, const std::string& text) {
        items.push_back(DropdownItem(value, text));
        return *this;
    }
    
    /// تعيين القيمة / Set value
    DropdownButton& SetValue(T v) {
        value = v;
        return *this;
    }
    
    /// تعيين دالة التغيير / Set on changed
    DropdownButton& OnChanged(std::function<void(T)> callback) {
        onChanged = callback;
        return *this;
    }
    
    /// الحصول على النص الحالي / Get current text
    std::string GetCurrentText() const {
        if (!value) return "";
        for (const auto& item : items) {
            if (item.value == *value) return item.text;
        }
        return "";
    }
};

// ============================================================================
// 35. PopupMenuButton - زر قائمة منبثقة / Popup Menu Button
// ============================================================================

/// زر فتح قائمة منبثقة / Button that opens a popup menu
template<typename T>
class PopupMenuButton : public Widget {
public:
    struct PopupMenuItem {
        T value;
        std::string text;
        std::shared_ptr<Widget> child;
        bool enabled;
        bool isDivider;
        
        PopupMenuItem() : enabled(true), isDivider(false) {}
        PopupMenuItem(T v, const std::string& t) : value(v), text(t), enabled(true), isDivider(false) {}
        
        static PopupMenuItem Divider() {
            PopupMenuItem item;
            item.isDivider = true;
            return item;
        }
    };
    
    std::vector<PopupMenuItem> items;
    std::function<void(T)> onSelected;
    std::function<void()> onCanceled;
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> icon;
    T initialValue;
    Float32 elevation;
    EdgeInsets padding;
    Float32 splashRadius;
    Color color;
    Color iconColor;
    Float32 iconSize;
    bool enabled;
    std::string tooltip;
    Vec2 offset;
    
    PopupMenuButton()
        : elevation(8.0f),
          padding(EdgeInsets::All(8)),
          splashRadius(20.0f),
          color(Color::White),
          iconColor(Color::Black),
          iconSize(24.0f),
          enabled(true),
          offset(0, 0) {}
    
    /// إضافة عنصر / Add item
    PopupMenuButton& AddItem(T value, const std::string& text) {
        items.push_back(PopupMenuItem(value, text));
        return *this;
    }
    
    /// إضافة فاصل / Add divider
    PopupMenuButton& AddDivider() {
        items.push_back(PopupMenuItem::Divider());
        return *this;
    }
    
    /// تعيين دالة الاختيار / Set on selected
    PopupMenuButton& OnSelected(std::function<void(T)> callback) {
        onSelected = callback;
        return *this;
    }
};

// ============================================================================
// 36. FilledButton - زر ممتلئ / Filled Button (Material 3)
// ============================================================================

/// زر ممتلئ (Material 3) / Filled button (Material 3)
class FilledButton : public Widget {
public:
    std::string text;
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> icon;
    std::function<void()> onPressed;
    std::function<void()> onLongPress;
    ButtonStyle style;
    bool enabled;
    bool tonal;  // filled tonal variant
    
    FilledButton() : enabled(true), tonal(false) {
        style.backgroundColor = Color(98, 0, 238);
        style.foregroundColor = Color::White;
        style.elevation = 0;
        style.borderRadius = BorderRadius::Circular(20);  // M3 stadium shape
        style.padding = EdgeInsets::Symmetric(24, 10);
    }
    
    /// تعيين النص / Set text
    FilledButton& SetText(const std::string& t) {
        text = t;
        return *this;
    }
    
    /// إنشاء نسخة tonal / Create tonal variant
    static FilledButton Tonal(const std::string& text) {
        FilledButton btn;
        btn.text = text;
        btn.tonal = true;
        btn.style.backgroundColor = Color(98, 0, 238, 30);
        btn.style.foregroundColor = Color(98, 0, 238);
        return btn;
    }
    
    /// تعيين دالة الضغط / Set on pressed
    FilledButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
};

// ============================================================================
// 37. BackButton - زر الرجوع / Back Button
// ============================================================================

/// زر الرجوع للتنقل / Back navigation button
class BackButton : public Widget {
public:
    std::function<void()> onPressed;
    Color color;
    Float32 size;
    std::string tooltip;
    
    BackButton()
        : color(Color::Black),
          size(24.0f),
          tooltip("رجوع / Back") {}
    
    /// تعيين دالة الضغط / Set on pressed
    BackButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
    
    /// تعيين اللون / Set color
    BackButton& SetColor(const Color& c) {
        color = c;
        return *this;
    }
};

// ============================================================================
// 38. CloseButton - زر الإغلاق / Close Button
// ============================================================================

/// زر إغلاق / Close button
class CloseButton : public Widget {
public:
    std::function<void()> onPressed;
    Color color;
    Float32 size;
    std::string tooltip;
    
    CloseButton()
        : color(Color::Black),
          size(24.0f),
          tooltip("إغلاق / Close") {}
    
    /// تعيين دالة الضغط / Set on pressed
    CloseButton& OnPressed(std::function<void()> callback) {
        onPressed = callback;
        return *this;
    }
};

// ============================================================================
// 39. MenuAnchor - مرساة القائمة / Menu Anchor
// ============================================================================

/// مرساة لقائمة منبثقة / Anchor for popup menu
class MenuAnchor : public Widget {
public:
    std::shared_ptr<Widget> child;
    std::function<std::shared_ptr<Widget>()> menuBuilder;
    std::function<void()> onOpen;
    std::function<void()> onClose;
    bool consumeOutsideTap;
    bool alignmentOffset;
    bool crossAxisUnconstrained;
    
    MenuAnchor()
        : consumeOutsideTap(true),
          alignmentOffset(false),
          crossAxisUnconstrained(true) {}
    
    /// فتح القائمة / Open menu
    void Open() {
        if (onOpen) onOpen();
    }
    
    /// إغلاق القائمة / Close menu
    void Close() {
        if (onClose) onClose();
    }
};

// ============================================================================
// 40. SubmenuButton - زر قائمة فرعية / Submenu Button
// ============================================================================

/// زر يفتح قائمة فرعية / Button that opens a submenu
class SubmenuButton : public Widget {
public:
    std::string text;
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> leadingIcon;
    std::shared_ptr<Widget> trailingIcon;
    std::vector<std::shared_ptr<Widget>> menuChildren;
    std::function<void(bool)> onHover;
    std::function<void()> onFocusChange;
    bool alignmentOffset;
    EdgeInsets menuPadding;
    
    SubmenuButton()
        : alignmentOffset(false),
          menuPadding(EdgeInsets::Symmetric(0, 8)) {}
    
    /// تعيين النص / Set text
    SubmenuButton& SetText(const std::string& t) {
        text = t;
        return *this;
    }
    
    /// إضافة عنصر قائمة / Add menu item
    SubmenuButton& AddMenuItem(std::shared_ptr<Widget> item) {
        menuChildren.push_back(item);
        return *this;
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MOBILE_BUTTONS_H
