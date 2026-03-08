// ============================================================================
// button.h - عنصر الزر (Button Widget)
// Button widget for user interaction
// ============================================================================
// الوصف: عنصر زر تفاعلي مع دعم الأحداث والحالات المختلفة
// Description: Interactive button widget with event support and states
// ============================================================================

#ifndef GRAPHICS_UI_BUTTON_H
#define GRAPHICS_UI_BUTTON_H

#include "label.h"  // نرث من Label لأن الزر يحتوي على نص / Inherit from Label as button contains text

namespace sad {
namespace graphics {
    namespace ui {

        // ============================================================================
        // Button - عنصر الزر
        // Button widget class
        // ============================================================================
        class Button : public Label {
        public:
            // البناء والتدمير / Construction and destruction
            Button();                                       // البناء الافتراضي / Default constructor
            Button(const std::string& text);                // البناء بنص / Constructor with text
            Button(const std::string& text, EventCallback onClick);  // البناء بنص ودالة / Constructor with text and callback
            virtual ~Button();                              // المدمر / Destructor

            // إدارة الأحداث / Event management
            void SetOnClick(EventCallback callback) { m_onClick = callback; }      // ضبط دالة النقر / Set click callback
            void SetOnHover(EventCallback callback) { m_onHover = callback; }      // ضبط دالة التحويم / Set hover callback
            void SetOnPress(EventCallback callback) { m_onPress = callback; }      // ضبط دالة الضغط / Set press callback
            void SetOnRelease(EventCallback callback) { m_onRelease = callback; }  // ضبط دالة الرفع / Set release callback

            // إدارة النمط حسب الحالة / State-based styling
            void SetNormalStyle(const WidgetStyle& style) { m_normalStyle = style; }      // نمط الحالة العادية / Normal state style
            void SetHoverStyle(const WidgetStyle& style) { m_hoverStyle = style; }        // نمط حالة التحويم / Hover state style
            void SetPressedStyle(const WidgetStyle& style) { m_pressedStyle = style; }    // نمط حالة الضغط / Pressed state style
            void SetDisabledStyle(const WidgetStyle& style) { m_disabledStyle = style; }  // نمط الحالة المعطلة / Disabled state style

            // معالجة الأحداث / Event handling
            virtual bool OnMouseMove(int x, int y) override;        // حركة الفأرة / Mouse move
            virtual bool OnMouseDown(int x, int y, int button) override;  // ضغط زر الفأرة / Mouse button down
            virtual bool OnMouseUp(int x, int y, int button) override;    // رفع زر الفأرة / Mouse button up

            // الرسم / Drawing
            virtual void Draw(SadGraphics::Renderer2D* renderer) override;  // رسم الزر / Draw button

        protected:
            // دوال الأحداث / Event callbacks
            EventCallback m_onClick;     // دالة عند النقر / Click callback
            EventCallback m_onHover;     // دالة عند التحويم / Hover callback
            EventCallback m_onPress;     // دالة عند الضغط / Press callback
            EventCallback m_onRelease;   // دالة عند الرفع / Release callback

            // أنماط الحالات / State styles
            WidgetStyle m_normalStyle;    // نمط الحالة العادية / Normal state style
            WidgetStyle m_hoverStyle;     // نمط حالة التحويم / Hover state style
            WidgetStyle m_pressedStyle;   // نمط حالة الضغط / Pressed state style
            WidgetStyle m_disabledStyle;  // نمط الحالة المعطلة / Disabled state style

            // حالة داخلية / Internal state
            bool m_isPressed;  // هل الزر مضغوط؟ / Is button pressed?

            // دوال محمية / Protected functions
            virtual void OnStateChanged() override;  // عند تغيير الحالة / When state changes
            void UpdateStyleForState();              // تحديث النمط حسب الحالة / Update style for state
        };

    } // namespace ui
} // namespace graphics
} // namespace sad

#endif // GRAPHICS_UI_BUTTON_H
