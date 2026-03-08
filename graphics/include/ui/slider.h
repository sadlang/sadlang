// ============================================================================
// slider.h - عنصر المنزلق (Slider Widget)
// Slider widget for value control
// ============================================================================
// الوصف: عنصر منزلق للتحكم في قيمة رقمية ضمن نطاق محدد
// Description: Slider widget for controlling numerical value within range
// ============================================================================

#ifndef GRAPHICS_UI_SLIDER_H
#define GRAPHICS_UI_SLIDER_H

#include "widget.h"         // الفئة الأساسية / Base class
#include <functional>       // للدوال الاستدعاء / For callbacks

namespace sad {
namespace graphics {
    namespace ui {

        // ============================================================================
        // SliderOrientation - اتجاه المنزلق
        // Slider orientation
        // ============================================================================
        enum class SliderOrientation {
            Horizontal,     // أفقي / Horizontal
            Vertical        // عمودي / Vertical
        };

        // ============================================================================
        // Slider - عنصر المنزلق
        // Slider widget class
        // ============================================================================
        class Slider : public Widget {
        public:
            // نوع دالة الاستدعاء عند تغيير القيمة / Value change callback type
            using ValueCallback = std::function<void(float)>;

            // البناء والتدمير / Construction and destruction
            Slider();                                                   // البناء الافتراضي / Default constructor
            Slider(float x, float y, float width, float height);        // البناء بموقع وحجم / Constructor with position and size
            Slider(float x, float y, float width, float height,
                   float minValue, float maxValue, float initialValue); // البناء الكامل / Full constructor
            virtual ~Slider();                                          // المدمر / Destructor

            // إدارة القيمة / Value management
            void SetValue(float value);                        // ضبط القيمة / Set value
            float GetValue() const { return m_value; }         // الحصول على القيمة / Get value

            void SetMinValue(float minValue);                  // ضبط القيمة الدنيا / Set minimum value
            float GetMinValue() const { return m_minValue; }   // الحصول على القيمة الدنيا / Get minimum value

            void SetMaxValue(float maxValue);                  // ضبط القيمة القصوى / Set maximum value
            float GetMaxValue() const { return m_maxValue; }   // الحصول على القيمة القصوى / Get maximum value

            void SetRange(float minValue, float maxValue);     // ضبط النطاق / Set range

            // حجم الخطوة / Step size
            void SetStepSize(float stepSize) { m_stepSize = stepSize; }  // ضبط حجم الخطوة / Set step size
            float GetStepSize() const { return m_stepSize; }             // الحصول على حجم الخطوة / Get step size

            // الاتجاه / Orientation
            void SetOrientation(SliderOrientation orientation);                  // ضبط الاتجاه / Set orientation
            SliderOrientation GetOrientation() const { return m_orientation; }   // الحصول على الاتجاه / Get orientation

            // حجم المقبض (Thumb) / Thumb size
            void SetThumbSize(float size) { m_thumbSize = size; }        // ضبط حجم المقبض / Set thumb size
            float GetThumbSize() const { return m_thumbSize; }           // الحصول على حجم المقبض / Get thumb size

            // الألوان / Colors
            void SetTrackColor(const Color& color) { m_trackColor = color; }         // لون المسار / Track color
            Color GetTrackColor() const { return m_trackColor; }

            void SetThumbColor(const Color& color) { m_thumbColor = color; }         // لون المقبض / Thumb color
            Color GetThumbColor() const { return m_thumbColor; }

            void SetFillColor(const Color& color) { m_fillColor = color; }           // لون التعبئة / Fill color
            Color GetFillColor() const { return m_fillColor; }

            void SetThumbHoverColor(const Color& color) { m_thumbHoverColor = color; }  // لون المقبض عند التحويم / Thumb hover color
            Color GetThumbHoverColor() const { return m_thumbHoverColor; }

            // عرض القيمة / Show value
            void SetShowValue(bool show) { m_showValue = show; }         // إظهار القيمة / Show value
            bool GetShowValue() const { return m_showValue; }            // هل تظهر القيمة؟ / Is value shown?

            void SetValueFormat(const std::string& format) { m_valueFormat = format; }  // تنسيق القيمة / Value format
            std::string GetValueFormat() const { return m_valueFormat; }

            // دوال الاستدعاء / Callbacks
            void SetOnValueChange(ValueCallback callback) { m_onValueChange = callback; }  // عند تغيير القيمة / On value change

            // الرسم والتحديث / Drawing and updating
            virtual void Draw(SadGraphics::Renderer2D* renderer) override;   // رسم المنزلق / Draw slider
            virtual void Update(float deltaTime) override;      // تحديث المنزلق / Update slider

            // معالجة الأحداث / Event handling
            virtual bool OnMouseMove(int x, int y) override;    // حركة الفأرة / Mouse move
            virtual bool OnMouseDown(int x, int y, int button) override;   // ضغط زر الفأرة / Mouse button down
            virtual bool OnMouseUp(int x, int y, int button) override;     // رفع زر الفأرة / Mouse button up
            virtual bool OnKeyDown(int keyCode) override;       // ضغط مفتاح / Key down

        protected:
            float m_value;                   // القيمة الحالية / Current value
            float m_minValue;                // القيمة الدنيا / Minimum value
            float m_maxValue;                // القيمة القصوى / Maximum value
            float m_stepSize;                // حجم الخطوة / Step size

            SliderOrientation m_orientation; // الاتجاه / Orientation
            float m_thumbSize;               // حجم المقبض / Thumb size

            Color m_trackColor;              // لون المسار / Track color
            Color m_thumbColor;              // لون المقبض / Thumb color
            Color m_fillColor;               // لون التعبئة / Fill color
            Color m_thumbHoverColor;         // لون المقبض عند التحويم / Thumb hover color

            bool m_showValue;                // إظهار القيمة / Show value
            std::string m_valueFormat;       // تنسيق القيمة / Value format

            bool m_isDragging;               // هل يتم السحب؟ / Is dragging?
            bool m_isThumbHovered;           // هل المقبض محوم؟ / Is thumb hovered?

            ValueCallback m_onValueChange;   // دالة الاستدعاء / Callback function

            // دوال مساعدة / Helper functions
            float GetThumbPosition() const;              // الحصول على موقع المقبض / Get thumb position
            Rect GetThumbRect() const;                   // الحصول على مستطيل المقبض / Get thumb rectangle
            Rect GetTrackRect() const;                   // الحصول على مستطيل المسار / Get track rectangle
            float ValueFromPosition(float position) const;  // القيمة من الموقع / Value from position
            void UpdateValueFromMouse(int x, int y);     // تحديث القيمة من الفأرة / Update value from mouse
            float ClampValue(float value) const;         // تقييد القيمة / Clamp value
            float SnapToStep(float value) const;         // محاذاة للخطوة / Snap to step
            std::string FormatValue(float value) const;  // تنسيق القيمة / Format value
        };

    } // namespace ui
} // namespace graphics
} // namespace sad

#endif // GRAPHICS_UI_SLIDER_H
