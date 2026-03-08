// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_input.cpp — تنفيذ مكونات الإدخال
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_input.h"
#include "sad_flutter_arabic.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  TextField State
// ═══════════════════════════════════════════════════════════════════════════════

class TextFieldState : public State<TextField> {
public:
    bool focused_ = false;
    bool showPassword_ = false;
    
    void initState() override {
        // Listen to controller changes
        auto w = widget();
        if (w && w->controller()) {
            w->controller()->addListener([this]{
                setState([]{});
            });
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto w = widget();
        if (!w) return std::make_shared<SizedBox>();
        
        auto& deco = w->decoration();
        bool hasError = !deco.errorText.empty();
        
        // Build content
        std::vector<WidgetPtr> children;
        
        // Label (floats when focused or has text)
        bool showLabel = !deco.labelText.empty();
        bool labelFloating = focused_ || !w->controller()->text().empty();
        
        // Prefix icon
        if (deco.prefixIcon) {
            children.push_back(deco.prefixIcon);
            children.push_back(فراغ_افقي(8));
        }
        
        // Main input area
        std::vector<WidgetPtr> inputStack;
        
        // Hint text (shown when empty and not focused)
        if (!deco.hintText.empty() && w->controller()->text().empty()) {
            inputStack.push_back(
                نص(deco.hintText, TextStyle().setFontSize(14).setColor(Color(158, 158, 158)))
            );
        }
        
        // Actual text
        std::string displayText = w->controller()->text();
        if (w->obscureText() && !showPassword_) {
            displayText = std::string(displayText.size(), '*');
        }
        
        inputStack.push_back(
            نص(displayText, TextStyle().setFontSize(14).setColor(
                deco.enabled ? Color::black() : Color::grey()
            ))
        );
        
        children.push_back(std::make_shared<Expanded>(
            std::make_shared<Stack>(std::move(inputStack))
        ));
        
        // Suffix icon or password toggle
        if (w->obscureText()) {
            children.push_back(
                std::make_shared<IconButton>(
                    showPassword_ ? Icons::Visibility : Icons::VisibilityOff,
                    [this]{ setState([this]{ showPassword_ = !showPassword_; }); },
                    20,
                    Color::grey()
                )
            );
        } else if (deco.suffixIcon) {
            children.push_back(deco.suffixIcon);
        }
        
        // Input row
        auto inputRow = std::make_shared<Row>(
            std::move(children),
            MainAxisAlignment::Start,
            MainAxisSize::Max,
            CrossAxisAlignment::Center
        );
        
        // Determine border color
        Color borderColor = deco.borderColor;
        if (hasError) {
            borderColor = deco.errorBorderColor;
        } else if (focused_) {
            borderColor = deco.focusedBorderColor;
        }
        
        // Build decoration
        BoxDecoration boxDeco;
        if (deco.filled) {
            boxDeco.setColor(deco.fillColor);
        }
        
        if (deco.borderType == InputBorderType::Outline) {
            boxDeco.setRadius(deco.borderRadius)
                   .setBorder(Border::all(focused_ ? 2 : deco.borderWidth, borderColor));
        }
        
        auto field = std::make_shared<GestureDetector>(
            std::make_shared<Container>(
                inputRow,
                std::nullopt, std::nullopt,
                deco.contentPadding,
                EdgeInsets(),
                boxDeco
            ),
            [this]{
                setState([this]{ focused_ = true; });
                // Platform: show keyboard
            }
        );
        
        // Add label, helper, error text in a column
        std::vector<WidgetPtr> columnChildren;
        
        if (showLabel && labelFloating) {
            columnChildren.push_back(
                نص(deco.labelText, TextStyle()
                    .setFontSize(12)
                    .setColor(hasError ? Color::red() : (focused_ ? Color::blue() : Color::grey()))
                )
            );
        }
        
        columnChildren.push_back(field);
        
        // Underline for underline style
        if (deco.borderType == InputBorderType::Underline) {
            columnChildren.push_back(
                std::make_shared<Container>(
                    nullptr,
                    std::nullopt, focused_ ? 2.0f : 1.0f,
                    EdgeInsets(),
                    EdgeInsets(),
                    BoxDecoration().setColor(borderColor)
                )
            );
        }
        
        // Helper or error text
        if (!deco.errorText.empty()) {
            columnChildren.push_back(فراغ_عمودي(4));
            columnChildren.push_back(
                نص(deco.errorText, TextStyle().setFontSize(12).setColor(Color::red()))
            );
        } else if (!deco.helperText.empty()) {
            columnChildren.push_back(فراغ_عمودي(4));
            columnChildren.push_back(
                نص(deco.helperText, TextStyle().setFontSize(12).setColor(Color::grey()))
            );
        }
        
        return عمود(std::move(columnChildren), MainAxisAlignment::Start, CrossAxisAlignment::Stretch);
    }
};

std::unique_ptr<State_Base> TextField::createState() {
    return std::make_unique<TextFieldState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Checkbox
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Checkbox::build(BuildContext* ctx) {
    BoxDecoration deco;
    deco.setRadius(4)
        .setBorder(Border::all(2, value_ ? activeColor_ : Color::grey()));
    
    if (value_) {
        deco.setColor(activeColor_);
    }
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            value_ ? ايقونة(Icons::Check, 16, checkColor_) : nullptr,
            20.0f, 20.0f,
            EdgeInsets::all(2),
            EdgeInsets(),
            deco
        ),
        [this]{ if (onChanged_) onChanged_(!value_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CheckboxListTile
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr CheckboxListTile::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    // Checkbox
    rowChildren.push_back(
        std::make_shared<Checkbox>(value_, onChanged_)
    );
    rowChildren.push_back(فراغ_افقي(16));
    
    // Title and subtitle
    std::vector<WidgetPtr> textChildren;
    textChildren.push_back(نص(title_));
    if (!subtitle_.empty()) {
        textChildren.push_back(فراغ_عمودي(4));
        textChildren.push_back(نص_صغير(subtitle_));
    }
    
    rowChildren.push_back(std::make_shared<Expanded>(
        عمود(std::move(textChildren), MainAxisAlignment::Center, CrossAxisAlignment::Start)
    ));
    
    // Secondary widget
    if (secondary_) {
        rowChildren.push_back(secondary_);
    }
    
    return std::make_shared<GestureDetector>(
        حشو(
            EdgeInsets::symmetric(12, 16),
            صف(std::move(rowChildren), MainAxisAlignment::Start, CrossAxisAlignment::Center)
        ),
        [this]{ if (onChanged_) onChanged_(!value_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Switch
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Switch::build(BuildContext* ctx) {
    // Track
    BoxDecoration trackDeco;
    trackDeco.setColor(value_ ? activeTrackColor_ : inactiveTrackColor_)
             .setRadius(7);
    
    auto track = std::make_shared<Container>(
        nullptr,
        36.0f, 14.0f,
        EdgeInsets(),
        EdgeInsets(),
        trackDeco
    );
    
    // Thumb
    BoxDecoration thumbDeco;
    thumbDeco.setColor(value_ ? activeColor_ : inactiveThumbColor_)
             .setRadius(10);
    thumbDeco.addShadow(BoxShadow{Color(0,0,0,32), 2, 0, 0, 1});
    
    auto thumb = موقع(
        std::make_shared<Container>(
            nullptr,
            20.0f, 20.0f,
            EdgeInsets(),
            EdgeInsets(),
            thumbDeco
        ),
        value_ ? 0.0f : 16.0f,  // left position (RTL adjusted)
        -3.0f                   // top (centered vertically)
    );
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Stack>(
            std::vector<WidgetPtr>{track, thumb}
        ),
        [this]{ if (onChanged_) onChanged_(!value_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SwitchListTile
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SwitchListTile::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    // Secondary widget (leading)
    if (secondary_) {
        rowChildren.push_back(secondary_);
        rowChildren.push_back(فراغ_افقي(16));
    }
    
    // Title and subtitle
    std::vector<WidgetPtr> textChildren;
    textChildren.push_back(نص(title_));
    if (!subtitle_.empty()) {
        textChildren.push_back(فراغ_عمودي(4));
        textChildren.push_back(نص_صغير(subtitle_));
    }
    
    rowChildren.push_back(std::make_shared<Expanded>(
        عمود(std::move(textChildren), MainAxisAlignment::Center, CrossAxisAlignment::Start)
    ));
    
    // Switch
    rowChildren.push_back(
        std::make_shared<Switch>(value_, onChanged_)
    );
    
    return std::make_shared<GestureDetector>(
        حشو(
            EdgeInsets::symmetric(12, 16),
            صف(std::move(rowChildren), MainAxisAlignment::Start, CrossAxisAlignment::Center)
        ),
        [this]{ if (onChanged_) onChanged_(!value_); }
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Radio
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
WidgetPtr Radio<T>::build(BuildContext* ctx) {
    bool selected = (value_ == groupValue_);
    
    BoxDecoration outerDeco;
    outerDeco.setRadius(10)
             .setBorder(Border::all(2, selected ? activeColor_ : Color::grey()));
    
    WidgetPtr inner = nullptr;
    if (selected) {
        BoxDecoration innerDeco;
        innerDeco.setColor(activeColor_).setRadius(5);
        inner = std::make_shared<Container>(
            nullptr,
            10.0f, 10.0f,
            EdgeInsets(),
            EdgeInsets(),
            innerDeco
        );
    }
    
    return std::make_shared<GestureDetector>(
        std::make_shared<Container>(
            inner ? توسيط(inner) : nullptr,
            20.0f, 20.0f,
            EdgeInsets(),
            EdgeInsets(),
            outerDeco
        ),
        [this]{ if (onChanged_) onChanged_(value_); }
    );
}

// Explicit instantiations
template class Radio<int>;
template class Radio<std::string>;

// ═══════════════════════════════════════════════════════════════════════════════
//  RadioListTile
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
WidgetPtr RadioListTile<T>::build(BuildContext* ctx) {
    std::vector<WidgetPtr> rowChildren;
    
    // Radio
    rowChildren.push_back(
        std::make_shared<Radio<T>>(value_, groupValue_, onChanged_)
    );
    rowChildren.push_back(فراغ_افقي(16));
    
    // Title and subtitle
    std::vector<WidgetPtr> textChildren;
    textChildren.push_back(نص(title_));
    if (!subtitle_.empty()) {
        textChildren.push_back(فراغ_عمودي(4));
        textChildren.push_back(نص_صغير(subtitle_));
    }
    
    rowChildren.push_back(std::make_shared<Expanded>(
        عمود(std::move(textChildren), MainAxisAlignment::Center, CrossAxisAlignment::Start)
    ));
    
    // Secondary widget
    if (secondary_) {
        rowChildren.push_back(secondary_);
    }
    
    return std::make_shared<GestureDetector>(
        حشو(
            EdgeInsets::symmetric(12, 16),
            صف(std::move(rowChildren), MainAxisAlignment::Start, CrossAxisAlignment::Center)
        ),
        [this]{ if (onChanged_) onChanged_(value_); }
    );
}

template class RadioListTile<int>;
template class RadioListTile<std::string>;

// ═══════════════════════════════════════════════════════════════════════════════
//  Slider
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr Slider::build(BuildContext* ctx) {
    float range = max_ - min_;
    float ratio = (range > 0) ? (value_ - min_) / range : 0;
    
    // Track
    auto inactiveTrack = std::make_shared<Container>(
        nullptr,
        std::nullopt, 4.0f,
        EdgeInsets(),
        EdgeInsets(),
        BoxDecoration().setColor(inactiveColor_).setRadius(2)
    );
    
    // Active portion
    auto activeTrack = موقع(
        std::make_shared<Container>(
            nullptr,
            std::nullopt, 4.0f,
            EdgeInsets(),
            EdgeInsets(),
            BoxDecoration().setColor(activeColor_).setRadius(2)
        ),
        std::nullopt, std::nullopt,  // Dynamic width based on ratio
        0.0f, std::nullopt
    );
    
    // Thumb
    BoxDecoration thumbDeco;
    thumbDeco.setColor(activeColor_).setRadius(10);
    thumbDeco.addShadow(BoxShadow{Color(0,0,0,32), 2, 0, 0, 1});
    
    auto thumb = موقع(
        std::make_shared<Container>(
            nullptr,
            20.0f, 20.0f,
            EdgeInsets(),
            EdgeInsets(),
            thumbDeco
        ),
        std::nullopt,  // left - calculated by ratio
        -8.0f          // top - centered
    );
    
    // Label if divisions
    WidgetPtr labels = nullptr;
    if (divisions_ > 0 && !label_.empty()) {
        // Show value label above thumb
        // Simplified: just show current value
    }
    
    return std::make_shared<GestureDetector>(
        حشو(
            EdgeInsets::symmetric(8, 0),
            std::make_shared<Stack>(
                std::vector<WidgetPtr>{inactiveTrack, activeTrack, thumb}
            )
        ),
        nullptr,  // onTap - handled by drag
        nullptr,  // onDoubleTap
        nullptr   // onLongPress
        // TODO: implement drag handling for slider
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  RangeSlider
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr RangeSlider::build(BuildContext* ctx) {
    // Similar to Slider but with two thumbs
    // Simplified implementation
    return std::make_shared<Container>(
        نص("RangeSlider - TODO"),
        std::nullopt, 48.0f
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DropdownButton
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
WidgetPtr DropdownButton<T>::build(BuildContext* ctx) {
    // Find current item text
    std::string displayText = hint_;
    for (const auto& item : items_) {
        if (item.value == value_) {
            displayText = item.text;
            break;
        }
    }
    
    std::vector<WidgetPtr> rowChildren;
    
    // Selected value text
    rowChildren.push_back(
        isExpanded_ 
            ? std::make_shared<Expanded>(نص(displayText))
            : نص(displayText)
    );
    
    // Dropdown icon
    rowChildren.push_back(
        icon_ ? icon_ : ايقونة(Icons::ArrowForward, iconSize_, Color::grey())
    );
    
    return std::make_shared<GestureDetector>(
        حشو(
            EdgeInsets::symmetric(8, 12),
            صف(std::move(rowChildren))
        ),
        [this, ctx]{
            // TODO: Show dropdown menu
            // This requires overlay/popup support
        }
    );
}

template class DropdownButton<int>;
template class DropdownButton<std::string>;

} // namespace flutter
} // namespace sad
