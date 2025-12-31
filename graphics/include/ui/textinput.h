// ============================================================================
// textinput.h - عنصر حقل إدخال النص (TextInput Widget)
// Text input field widget for user text entry
// ============================================================================
// الوصف: عنصر تفاعلي لإدخال النص مع دعم التحديد والنسخ واللصق
// Description: Interactive text input widget with selection, copy, and paste
// ============================================================================

#ifndef GRAPHICS_UI_TEXTINPUT_H
#define GRAPHICS_UI_TEXTINPUT_H

#include "widget.h"                                // الفئة الأساسية / Base class
#include "../text/font.h"                          // لاستخدام الخطوط / For fonts
#include <memory>                                  // لاستخدام المؤشرات الذكية / For smart pointers
#include <functional>                              // لاستخدام الدوال / For functions

namespace Graphics {
    namespace UI {

        // تعريف دالة رد عند تغيير النص / Text change callback type
        using TextChangeCallback = std::function<void(const std::string&)>;

        // ============================================================================
        // TextInput - عنصر حقل إدخال النص
        // Text input field widget class
        // ============================================================================
        class TextInput : public Widget {
        public:
            // البناء والتدمير / Construction and destruction
            TextInput();                                        // البناء الافتراضي / Default constructor
            TextInput(const std::string& placeholder);          // البناء مع نص توضيحي / Constructor with placeholder
            virtual ~TextInput();                               // المدمر / Destructor

            // إدارة النص / Text management
            void SetText(const std::string& text);              // ضبط النص / Set text
            const std::string& GetText() const { return m_text; } // الحصول على النص / Get text

            // نص توضيحي / Placeholder
            void SetPlaceholder(const std::string& placeholder); // ضبط النص التوضيحي / Set placeholder
            const std::string& GetPlaceholder() const { return m_placeholder; } // الحصول على النص التوضيحي / Get placeholder

            // إدارة الخط / Font management
            void SetFont(FontRef font);                         // ضبط الخط / Set font
            FontRef GetFont() const { return m_font; }          // الحصول على الخط / Get font

            // إدارة الألوان / Color management
            void SetTextColor(const Color& color) { m_textColor = color; }          // ضبط لون النص / Set text color
            void SetPlaceholderColor(const Color& color) { m_placeholderColor = color; } // ضبط لون النص التوضيحي / Set placeholder color
            void SetSelectionColor(const Color& color) { m_selectionColor = color; } // ضبط لون التحديد / Set selection color
            void SetCursorColor(const Color& color) { m_cursorColor = color; }      // ضبط لون المؤشر / Set cursor color

            const Color& GetTextColor() const { return m_textColor; }               // الحصول على لون النص / Get text color

            // وضع كلمة المرور / Password mode
            void SetPasswordMode(bool enabled) { m_passwordMode = enabled; }        // ضبط وضع كلمة المرور / Set password mode
            bool IsPasswordMode() const { return m_passwordMode; }                  // هل في وضع كلمة المرور؟ / Is password mode?

            // حد النص / Text limit
            void SetMaxLength(int maxLength) { m_maxLength = maxLength; }           // ضبط الحد الأقصى للأحرف / Set max length
            int GetMaxLength() const { return m_maxLength; }                        // الحصول على الحد الأقصى / Get max length

            // قراءة فقط / Read-only
            void SetReadOnly(bool readOnly) { m_readOnly = readOnly; }              // ضبط القراءة فقط / Set read-only
            bool IsReadOnly() const { return m_readOnly; }                          // هل قراءة فقط؟ / Is read-only?

            // دوال الأحداث / Event callbacks
            void SetOnTextChange(TextChangeCallback callback) { m_onTextChange = callback; } // عند تغيير النص / On text change
            void SetOnEnter(EventCallback callback) { m_onEnter = callback; }       // عند الضغط على Enter / On enter key
            void SetOnEscape(EventCallback callback) { m_onEscape = callback; }     // عند الضغط على Escape / On escape key

            // التحديد / Selection
            void SelectAll();                                   // تحديد الكل / Select all
            void ClearSelection();                              // مسح التحديد / Clear selection
            bool HasSelection() const;                          // هل يوجد تحديد؟ / Has selection?
            std::string GetSelectedText() const;                // الحصول على النص المحدد / Get selected text

            // الحافظة / Clipboard
            void Copy();                                        // نسخ / Copy
            void Cut();                                         // قص / Cut
            void Paste();                                       // لصق / Paste

            // موقع المؤشر / Cursor position
            int GetCursorPosition() const { return m_cursorPos; } // الحصول على موقع المؤشر / Get cursor position
            void SetCursorPosition(int pos);                    // ضبط موقع المؤشر / Set cursor position

            // الرسم والتحديث / Drawing and updating
            virtual void Draw(Renderer2D* renderer) override;   // رسم حقل الإدخال / Draw input field
            virtual void Update(float deltaTime) override;      // تحديث / Update

            // معالجة الأحداث / Event handling
            virtual bool OnMouseDown(int x, int y, int button) override;   // ضغط زر الفأرة / Mouse button down
            virtual bool OnMouseMove(int x, int y) override;               // حركة الفأرة / Mouse move
            virtual bool OnKeyDown(int keyCode) override;                  // ضغط مفتاح / Key down
            virtual bool OnTextInput(const std::string& text) override;    // إدخال نص / Text input

        protected:
            std::string m_text;                  // النص الحالي / Current text
            std::string m_placeholder;           // النص التوضيحي / Placeholder text
            FontRef m_font;                      // الخط المستخدم / Font used
            
            Color m_textColor;                   // لون النص / Text color
            Color m_placeholderColor;            // لون النص التوضيحي / Placeholder color
            Color m_selectionColor;              // لون التحديد / Selection color
            Color m_cursorColor;                 // لون المؤشر / Cursor color

            int m_cursorPos;                     // موقع المؤشر / Cursor position
            int m_selectionStart;                // بداية التحديد / Selection start
            int m_selectionEnd;                  // نهاية التحديد / Selection end

            bool m_passwordMode;                 // وضع كلمة المرور / Password mode
            bool m_readOnly;                     // قراءة فقط / Read-only
            int m_maxLength;                     // الحد الأقصى للأحرف / Max character length

            float m_cursorBlinkTime;             // وقت وميض المؤشر / Cursor blink time
            bool m_cursorVisible;                // هل المؤشر ظاهر / Is cursor visible
            float m_scrollOffset;                // إزاحة التمرير / Scroll offset

            TextChangeCallback m_onTextChange;   // دالة عند تغيير النص / Text change callback
            EventCallback m_onEnter;             // دالة عند Enter / Enter callback
            EventCallback m_onEscape;            // دالة عند Escape / Escape callback

            // دوال مساعدة / Helper functions
            void InsertText(const std::string& text);           // إدراج نص / Insert text
            void DeleteSelection();                             // حذف النص المحدد / Delete selected text
            void DeleteChar(bool forward);                      // حذف حرف / Delete character
            int GetCharIndexAtX(float x);                       // الحصول على موقع الحرف عند X / Get char index at X
            float GetCursorX() const;                           // الحصول على موقع المؤشر X / Get cursor X position
            std::string GetDisplayText() const;                 // الحصول على النص المعروض / Get display text
            void EnsureCursorVisible();                         // التأكد من ظهور المؤشر / Ensure cursor visible
        };

    } // namespace UI
} // namespace Graphics

#endif // GRAPHICS_UI_TEXTINPUT_H
