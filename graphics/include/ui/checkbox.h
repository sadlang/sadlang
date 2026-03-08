// ============================================================================
// checkbox.h - عنصر مربع الاختيار (Checkbox Widget)
// Checkbox widget for boolean selections
// ============================================================================
// الوصف: عنصر تفاعلي لاختيار نعم/لا مع تسمية اختيارية
// Description: Interactive checkbox widget with optional label
// ============================================================================

#ifndef GRAPHICS_UI_CHECKBOX_H
#define GRAPHICS_UI_CHECKBOX_H

#include "widget.h"  // الفئة الأساسية / Base class
#include "../text/font.h"  // للخطوط / For fonts

namespace sad {
namespace graphics {
    namespace ui {

        // تعريف دالة رد عند تغيير الحالة / State change callback type
        using CheckStateCallback = std::function<void(bool)>;

        // ============================================================================
        // Checkbox - عنصر مربع الاختيار
        // Checkbox widget class
        // ============================================================================
        class Checkbox : public Widget {
        public:
            // البناء والتدمير / Construction and destruction
            Checkbox();                                         // البناء الافتراضي / Default constructor
            Checkbox(const std::string& label);                 // البناء مع تسمية / Constructor with label
            Checkbox(const std::string& label, bool checked);   // البناء مع تسمية وحالة / Constructor with label and state
            virtual ~Checkbox();                                // المدمر / Destructor

            // إدارة الحالة / State management
            void SetChecked(bool checked);                      // ضبط الحالة / Set checked state
            bool IsChecked() const { return m_checked; }        // هل محدد؟ / Is checked?
            void Toggle();                                      // تبديل الحالة / Toggle state

            // إدارة التسمية / Label management
            void SetLabel(const std::string& label);            // ضبط التسمية / Set label
            const std::string& GetLabel() const { return m_label; } // الحصول على التسمية / Get label

            // إدارة الخط / Font management
            void SetFont(FontRef font);                         // ضبط الخط / Set font
            FontRef GetFont() const { return m_font; }          // الحصول على الخط / Get font

            // إدارة الألوان / Color management
            void SetCheckColor(const Color& color) { m_checkColor = color; }     // لون العلامة / Check color
            void SetLabelColor(const Color& color) { m_labelColor = color; }     // لون التسمية / Label color
            void SetBoxColor(const Color& color) { m_boxColor = color; }         // لون المربع / Box color

            // حجم المربع / Box size
            void SetBoxSize(float size);                        // ضبط حجم المربع / Set box size
            float GetBoxSize() const { return m_boxSize; }      // الحصول على حجم المربع / Get box size

            // دوال الأحداث / Event callbacks
            void SetOnCheckChange(CheckStateCallback callback) { m_onCheckChange = callback; } // عند تغيير الحالة / On state change

            // الرسم / Drawing
            virtual void Draw(SadGraphics::Renderer2D* renderer) override;   // رسم مربع الاختيار / Draw checkbox

            // معالجة الأحداث / Event handling
            virtual bool OnMouseDown(int x, int y, int button) override;   // ضغط زر الفأرة / Mouse button down
            virtual bool OnKeyDown(int keyCode) override;                  // ضغط مفتاح / Key down

        protected:
            bool m_checked;                      // هل محدد؟ / Is checked?
            std::string m_label;                 // النص التوضيحي / Label text
            FontRef m_font;                      // الخط المستخدم / Font used
            
            Color m_checkColor;                  // لون العلامة / Check mark color
            Color m_labelColor;                  // لون التسمية / Label color
            Color m_boxColor;                    // لون المربع / Box color

            float m_boxSize;                     // حجم المربع / Box size
            
            CheckStateCallback m_onCheckChange;  // دالة عند تغيير الحالة / State change callback

            // دوال مساعدة / Helper functions
            bool IsPointInBox(float x, float y) const;  // هل النقطة داخل المربع؟ / Is point in box?
        };

    } // namespace ui
} // namespace graphics
} // namespace sad

#endif // GRAPHICS_UI_CHECKBOX_H
