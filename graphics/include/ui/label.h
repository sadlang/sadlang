// ============================================================================
// label.h - عنصر التسمية (Label Widget)
// Label widget for displaying text
// ============================================================================
// الوصف: عنصر لعرض النص الثابت مع دعم المحاذاة والتنسيق
// Description: Widget for displaying static text with alignment and styling
// ============================================================================

#ifndef GRAPHICS_UI_LABEL_H
#define GRAPHICS_UI_LABEL_H

#include "widget.h"                                // الفئة الأساسية / Base class
#include "../text/font.h"                          // لاستخدام الخطوط / For fonts
#include <memory>                                  // لاستخدام المؤشرات الذكية / For smart pointers

namespace Graphics {
    namespace UI {

        // ============================================================================
        // TextAlignment - محاذاة النص
        // Text alignment enumeration
        // ============================================================================
        enum class TextAlignment {
            Left,         // محاذاة لليسار / Left alignment
            Center,       // محاذاة للمركز / Center alignment
            Right,        // محاذاة لليمين / Right alignment
            TopLeft,      // أعلى يسار / Top-left
            TopCenter,    // أعلى وسط / Top-center
            TopRight,     // أعلى يمين / Top-right
            MiddleLeft,   // وسط يسار / Middle-left
            MiddleCenter, // وسط وسط / Middle-center
            MiddleRight,  // وسط يمين / Middle-right
            BottomLeft,   // أسفل يسار / Bottom-left
            BottomCenter, // أسفل وسط / Bottom-center
            BottomRight   // أسفل يمين / Bottom-right
        };

        // ============================================================================
        // Label - عنصر التسمية
        // Label widget class
        // ============================================================================
        class Label : public Widget {
        public:
            // البناء والتدمير / Construction and destruction
            Label();                                       // البناء الافتراضي / Default constructor
            Label(const std::string& text);                // البناء بنص / Constructor with text
            Label(const std::string& text, FontRef font);  // البناء بنص وخط / Constructor with text and font
            virtual ~Label();                              // المدمر / Destructor

            // إدارة النص / Text management
            void SetText(const std::string& text);         // ضبط النص / Set text
            const std::string& GetText() const { return m_text; }  // الحصول على النص / Get text

            // إدارة الخط / Font management
            void SetFont(FontRef font);                    // ضبط الخط / Set font
            FontRef GetFont() const { return m_font; }     // الحصول على الخط / Get font

            // إدارة اللون / Color management
            void SetTextColor(const Color& color) { m_textColor = color; }  // ضبط لون النص / Set text color
            const Color& GetTextColor() const { return m_textColor; }       // الحصول على لون النص / Get text color

            // المحاذاة / Alignment
            void SetAlignment(TextAlignment alignment);    // ضبط المحاذاة / Set alignment
            TextAlignment GetAlignment() const { return m_alignment; }  // الحصول على المحاذاة / Get alignment

            // Word Wrapping - لف النص
            void SetWordWrap(bool wrap) { m_wordWrap = wrap; }  // ضبط لف النص / Set word wrap
            bool GetWordWrap() const { return m_wordWrap; }     // الحصول على حالة اللف / Get word wrap state

            // حجم النص التلقائي / Auto sizing
            void SetAutoSize(bool autoSize) { m_autoSize = autoSize; }  // ضبط الحجم التلقائي / Set auto size
            bool GetAutoSize() const { return m_autoSize; }             // الحصول على حالة الحجم التلقائي / Get auto size state

            // قياس النص / Text measurement
            void MeasureText(float& width, float& height) const;  // قياس أبعاد النص / Measure text dimensions

            // الرسم / Drawing
            virtual void Draw(SadGraphics::Renderer2D* renderer) override;  // رسم التسمية / Draw label

        protected:
            std::string m_text;             // النص المعروض / Displayed text
            FontRef m_font;                 // الخط المستخدم / Font used
            Color m_textColor;              // لون النص / Text color
            TextAlignment m_alignment;      // محاذاة النص / Text alignment
            bool m_wordWrap;                // هل نلف النص؟ / Word wrap enabled?
            bool m_autoSize;                // هل نضبط الحجم تلقائياً؟ / Auto size enabled?

            // دوال مساعدة / Helper functions
            void CalculateTextPosition(float& x, float& y) const;  // حساب موقع النص / Calculate text position
            void UpdateSize();                                     // تحديث الحجم / Update size
        };

    } // namespace UI
} // namespace Graphics

#endif // GRAPHICS_UI_LABEL_H
