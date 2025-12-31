// ============================================================================
// textinput.cpp - تطبيق عنصر حقل إدخال النص
// Implementation of text input field widget
// ============================================================================

#include "../include/ui/textinput.h"               // ملف الرأس / Header file
#include "../include/rendering/renderer2d.h"       // للرسم / For rendering
#include "../include/resources/resource_manager.h" // لإدارة الخطوط / For font management
#include "../include/input/input_manager.h"        // للحافظة / For clipboard
#include <SDL.h>                                   // لوظائف الحافظة / For clipboard functions
#include <algorithm>                               // للخوارزميات / For algorithms

namespace Graphics {
    namespace UI {

        // ============================================================================
        // البناء الافتراضي / Default constructor
        // ============================================================================
        TextInput::TextInput()
            : Widget()                              // استدعاء بناء الأساس / Call base constructor
            , m_text("")                            // نص فارغ / Empty text
            , m_placeholder("Enter text...")        // نص توضيحي افتراضي / Default placeholder
            , m_font(nullptr)                       // لا يوجد خط / No font
            , m_textColor(Color::Black)             // لون أسود / Black color
            , m_placeholderColor(Color(150, 150, 150)) // لون رمادي / Gray color
            , m_selectionColor(Color(100, 150, 255, 128)) // لون أزرق شفاف / Transparent blue
            , m_cursorColor(Color::Black)           // مؤشر أسود / Black cursor
            , m_cursorPos(0)                        // المؤشر في البداية / Cursor at start
            , m_selectionStart(-1)                  // لا يوجد تحديد / No selection
            , m_selectionEnd(-1)                    // لا يوجد تحديد / No selection
            , m_passwordMode(false)                 // ليس وضع كلمة مرور / Not password mode
            , m_readOnly(false)                     // ليس قراءة فقط / Not read-only
            , m_maxLength(-1)                       // لا يوجد حد / No limit
            , m_cursorBlinkTime(0.0f)               // وقت وميض صفر / Zero blink time
            , m_cursorVisible(true)                 // المؤشر ظاهر / Cursor visible
            , m_scrollOffset(0.0f)                  // لا يوجد إزاحة / No offset
            , m_onTextChange(nullptr)               // لا توجد دالة / No callback
            , m_onEnter(nullptr)                    // لا توجد دالة / No callback
            , m_onEscape(nullptr)                   // لا توجد دالة / No callback
        {
            // تحميل خط افتراضي / Load default font
            auto& rm = ResourceManager::GetInstance();
            m_font = rm.LoadFont("arial.ttf", 16.0f);  // خط Arial بحجم 16 / Arial font size 16

            // ضبط حجم افتراضي / Set default size
            SetSize(200, 30);

            // نمط افتراضي / Default style
            m_style.backgroundColor = Color::White;     // خلفية بيضاء / White background
            m_style.borderColor = Color(150, 150, 150); // حدود رمادية / Gray border
            m_style.borderWidth = 1.0f;                 // عرض حدود / Border width
            m_style.padding = 5.0f;                     // مسافة داخلية / Padding
        }

        // ============================================================================
        // البناء مع نص توضيحي / Constructor with placeholder
        // ============================================================================
        TextInput::TextInput(const std::string& placeholder)
            : TextInput()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetPlaceholder(placeholder);  // ضبط النص التوضيحي / Set placeholder
        }

        // ============================================================================
        // التدمير / Destructor
        // ============================================================================
        TextInput::~TextInput() {
            // لا حاجة لتنظيف إضافي / No additional cleanup needed
        }

        // ============================================================================
        // ضبط النص / Set text
        // ============================================================================
        void TextInput::SetText(const std::string& text) {
            // التحقق من الحد الأقصى / Check max length
            if (m_maxLength > 0 && (int)text.length() > m_maxLength) {
                m_text = text.substr(0, m_maxLength);  // قص النص / Truncate text
            } else {
                m_text = text;  // ضبط النص / Set text
            }

            // ضبط المؤشر في النهاية / Set cursor at end
            m_cursorPos = (int)m_text.length();

            // مسح التحديد / Clear selection
            ClearSelection();

            // استدعاء دالة التغيير / Call change callback
            if (m_onTextChange) {
                m_onTextChange(m_text);
            }
        }

        // ============================================================================
        // ضبط النص التوضيحي / Set placeholder
        // ============================================================================
        void TextInput::SetPlaceholder(const std::string& placeholder) {
            m_placeholder = placeholder;  // تحديث النص التوضيحي / Update placeholder
        }

        // ============================================================================
        // ضبط الخط / Set font
        // ============================================================================
        void TextInput::SetFont(FontRef font) {
            if (font) {  // التحقق من صحة الخط / Check font validity
                m_font = font;  // تحديث الخط / Update font
            }
        }

        // ============================================================================
        // تحديد الكل / Select all
        // ============================================================================
        void TextInput::SelectAll() {
            if (!m_text.empty()) {  // إذا كان هناك نص / If text exists
                m_selectionStart = 0;                  // من البداية / From start
                m_selectionEnd = (int)m_text.length(); // إلى النهاية / To end
            }
        }

        // ============================================================================
        // مسح التحديد / Clear selection
        // ============================================================================
        void TextInput::ClearSelection() {
            m_selectionStart = -1;  // لا يوجد تحديد / No selection
            m_selectionEnd = -1;    // لا يوجد تحديد / No selection
        }

        // ============================================================================
        // هل يوجد تحديد؟ / Has selection?
        // ============================================================================
        bool TextInput::HasSelection() const {
            return m_selectionStart >= 0 && m_selectionEnd >= 0 && m_selectionStart != m_selectionEnd;
        }

        // ============================================================================
        // الحصول على النص المحدد / Get selected text
        // ============================================================================
        std::string TextInput::GetSelectedText() const {
            if (!HasSelection()) {  // إذا لم يكن هناك تحديد / If no selection
                return "";          // إرجاع نص فارغ / Return empty string
            }

            // ترتيب البداية والنهاية / Sort start and end
            int start = std::min(m_selectionStart, m_selectionEnd);
            int end = std::max(m_selectionStart, m_selectionEnd);

            // إرجاع النص المحدد / Return selected text
            return m_text.substr(start, end - start);
        }

        // ============================================================================
        // نسخ / Copy
        // ============================================================================
        void TextInput::Copy() {
            if (HasSelection()) {  // إذا كان هناك تحديد / If has selection
                std::string selected = GetSelectedText();  // الحصول على النص / Get text
                SDL_SetClipboardText(selected.c_str());    // نسخ للحافظة / Copy to clipboard
            }
        }

        // ============================================================================
        // قص / Cut
        // ============================================================================
        void TextInput::Cut() {
            if (HasSelection() && !m_readOnly) {  // إذا كان هناك تحديد وليس قراءة فقط / If has selection and not read-only
                Copy();            // نسخ / Copy
                DeleteSelection(); // حذف / Delete
            }
        }

        // ============================================================================
        // لصق / Paste
        // ============================================================================
        void TextInput::Paste() {
            if (m_readOnly) {  // إذا كان قراءة فقط / If read-only
                return;        // لا نفعل شيء / Do nothing
            }

            // الحصول على نص الحافظة / Get clipboard text
            if (SDL_HasClipboardText()) {
                char* clipText = SDL_GetClipboardText();  // قراءة النص / Read text
                if (clipText) {
                    InsertText(std::string(clipText));  // إدراج النص / Insert text
                    SDL_free(clipText);                 // تحرير الذاكرة / Free memory
                }
            }
        }

        // ============================================================================
        // ضبط موقع المؤشر / Set cursor position
        // ============================================================================
        void TextInput::SetCursorPosition(int pos) {
            // التأكد من الحدود / Ensure bounds
            m_cursorPos = std::max(0, std::min(pos, (int)m_text.length()));
            
            // إعادة تعيين وميض المؤشر / Reset cursor blink
            m_cursorBlinkTime = 0.0f;
            m_cursorVisible = true;
        }

        // ============================================================================
        // رسم حقل الإدخال / Draw input field
        // ============================================================================
        void TextInput::Draw(Renderer2D* renderer) {
            // إذا لم يكن ظاهراً / If not visible
            if (!m_visible) {
                return;
            }

            // رسم الخلفية والحدود / Draw background and borders
            Widget::Draw(renderer);

            // إذا لم يكن هناك خط / If no font
            if (!m_font) {
                return;
            }

            // حساب منطقة النص / Calculate text area
            float textX = m_bounds.x + m_style.padding;
            float textY = m_bounds.y + m_style.padding;
            float textWidth = m_bounds.width - (m_style.padding * 2.0f);
            float textHeight = m_bounds.height - (m_style.padding * 2.0f);

            // الحصول على النص المعروض / Get display text
            std::string displayText = GetDisplayText();

            // رسم التحديد / Draw selection
            if (HasSelection() && m_focused) {
                int start = std::min(m_selectionStart, m_selectionEnd);
                int end = std::max(m_selectionStart, m_selectionEnd);

                // حساب موقع التحديد / Calculate selection position
                std::string beforeSelection = displayText.substr(0, start);
                std::string selection = displayText.substr(start, end - start);

                float beforeWidth, beforeHeight;
                m_font->MeasureText(beforeSelection, beforeWidth, beforeHeight);

                float selWidth, selHeight;
                m_font->MeasureText(selection, selWidth, selHeight);

                // رسم مستطيل التحديد / Draw selection rectangle
                float selX = textX + beforeWidth - m_scrollOffset;
                renderer->DrawRect(selX, textY, selWidth, textHeight, m_selectionColor, true);
            }

            // رسم النص أو النص التوضيحي / Draw text or placeholder
            if (!displayText.empty()) {
                // رسم النص الفعلي / Draw actual text
                renderer->DrawText(displayText, m_font, textX - m_scrollOffset, textY, m_textColor);
            } else if (!m_placeholder.empty() && !m_focused) {
                // رسم النص التوضيحي / Draw placeholder
                renderer->DrawText(m_placeholder, m_font, textX, textY, m_placeholderColor);
            }

            // رسم المؤشر / Draw cursor
            if (m_focused && m_cursorVisible && !m_readOnly) {
                float cursorX = GetCursorX();
                float cursorHeight = textHeight;

                // رسم خط المؤشر / Draw cursor line
                renderer->DrawRect(cursorX, textY, 2.0f, cursorHeight, m_cursorColor, true);
            }
        }

        // ============================================================================
        // تحديث / Update
        // ============================================================================
        void TextInput::Update(float deltaTime) {
            // استدعاء تحديث الأساس / Call base update
            Widget::Update(deltaTime);

            // تحديث وميض المؤشر / Update cursor blink
            if (m_focused) {
                m_cursorBlinkTime += deltaTime;  // زيادة الوقت / Increment time
                
                // تبديل الظهور كل 0.5 ثانية / Toggle visibility every 0.5 seconds
                if (m_cursorBlinkTime >= 0.5f) {
                    m_cursorVisible = !m_cursorVisible;  // تبديل / Toggle
                    m_cursorBlinkTime = 0.0f;            // إعادة الضبط / Reset
                }
            }
        }

        // ============================================================================
        // ضغط زر الفأرة / Mouse button down
        // ============================================================================
        bool TextInput::OnMouseDown(int x, int y, int button) {
            // استدعاء الأساس / Call base
            if (Widget::OnMouseDown(x, y, button)) {
                // حساب موقع النقر / Calculate click position
                float clickX = (float)x - m_bounds.x - m_style.padding + m_scrollOffset;
                int charIndex = GetCharIndexAtX(clickX);

                // ضبط المؤشر / Set cursor
                SetCursorPosition(charIndex);
                
                // مسح التحديد / Clear selection
                ClearSelection();

                return true;
            }
            return false;
        }

        // ============================================================================
        // حركة الفأرة / Mouse move
        // ============================================================================
        bool TextInput::OnMouseMove(int x, int y) {
            // استدعاء الأساس / Call base
            return Widget::OnMouseMove(x, y);
        }

        // ============================================================================
        // ضغط مفتاح / Key down
        // ============================================================================
        bool TextInput::OnKeyDown(int keyCode) {
            // إذا لم يكن له التركيز / If not focused
            if (!m_focused) {
                return false;
            }

            // معالجة المفاتيح الخاصة / Handle special keys
            switch (keyCode) {
                case SDLK_RETURN:      // مفتاح Enter / Enter key
                case SDLK_KP_ENTER:    // مفتاح Enter الرقمي / Numeric Enter
                    if (m_onEnter) {
                        m_onEnter();   // استدعاء الدالة / Call callback
                    }
                    return true;

                case SDLK_ESCAPE:      // مفتاح Escape / Escape key
                    if (m_onEscape) {
                        m_onEscape();  // استدعاء الدالة / Call callback
                    }
                    SetFocused(false); // إلغاء التركيز / Remove focus
                    return true;

                case SDLK_BACKSPACE:   // مفتاح Backspace / Backspace key
                    if (!m_readOnly) {
                        if (HasSelection()) {
                            DeleteSelection();  // حذف التحديد / Delete selection
                        } else {
                            DeleteChar(false);  // حذف الحرف السابق / Delete previous char
                        }
                    }
                    return true;

                case SDLK_DELETE:      // مفتاح Delete / Delete key
                    if (!m_readOnly) {
                        if (HasSelection()) {
                            DeleteSelection();  // حذف التحديد / Delete selection
                        } else {
                            DeleteChar(true);   // حذف الحرف التالي / Delete next char
                        }
                    }
                    return true;

                case SDLK_LEFT:        // سهم يسار / Left arrow
                    if (m_cursorPos > 0) {
                        SetCursorPosition(m_cursorPos - 1);  // تحريك المؤشر / Move cursor
                    }
                    ClearSelection();  // مسح التحديد / Clear selection
                    return true;

                case SDLK_RIGHT:       // سهم يمين / Right arrow
                    if (m_cursorPos < (int)m_text.length()) {
                        SetCursorPosition(m_cursorPos + 1);  // تحريك المؤشر / Move cursor
                    }
                    ClearSelection();  // مسح التحديد / Clear selection
                    return true;

                case SDLK_HOME:        // مفتاح Home / Home key
                    SetCursorPosition(0);          // البداية / Start
                    ClearSelection();              // مسح التحديد / Clear selection
                    return true;

                case SDLK_END:         // مفتاح End / End key
                    SetCursorPosition((int)m_text.length());  // النهاية / End
                    ClearSelection();              // مسح التحديد / Clear selection
                    return true;

                case SDLK_a:           // Ctrl+A (Select All)
                    if (SDL_GetModState() & KMOD_CTRL) {
                        SelectAll();   // تحديد الكل / Select all
                        return true;
                    }
                    break;

                case SDLK_c:           // Ctrl+C (Copy)
                    if (SDL_GetModState() & KMOD_CTRL) {
                        Copy();        // نسخ / Copy
                        return true;
                    }
                    break;

                case SDLK_x:           // Ctrl+X (Cut)
                    if (SDL_GetModState() & KMOD_CTRL) {
                        Cut();         // قص / Cut
                        return true;
                    }
                    break;

                case SDLK_v:           // Ctrl+V (Paste)
                    if (SDL_GetModState() & KMOD_CTRL) {
                        Paste();       // لصق / Paste
                        return true;
                    }
                    break;
            }

            return false;
        }

        // ============================================================================
        // إدخال نص / Text input
        // ============================================================================
        bool TextInput::OnTextInput(const std::string& text) {
            // إذا لم يكن له التركيز أو قراءة فقط / If not focused or read-only
            if (!m_focused || m_readOnly) {
                return false;
            }

            // إدراج النص / Insert text
            InsertText(text);
            return true;
        }

        // ============================================================================
        // إدراج نص / Insert text
        // ============================================================================
        void TextInput::InsertText(const std::string& text) {
            // حذف التحديد إذا وجد / Delete selection if exists
            if (HasSelection()) {
                DeleteSelection();
            }

            // التحقق من الحد الأقصى / Check max length
            if (m_maxLength > 0 && (int)(m_text.length() + text.length()) > m_maxLength) {
                return;  // تجاوز الحد / Exceeded limit
            }

            // إدراج النص / Insert text
            m_text.insert(m_cursorPos, text);
            m_cursorPos += (int)text.length();  // تحريك المؤشر / Move cursor

            // إعادة ضبط وميض المؤشر / Reset cursor blink
            m_cursorBlinkTime = 0.0f;
            m_cursorVisible = true;

            // التأكد من ظهور المؤشر / Ensure cursor visible
            EnsureCursorVisible();

            // استدعاء دالة التغيير / Call change callback
            if (m_onTextChange) {
                m_onTextChange(m_text);
            }
        }

        // ============================================================================
        // حذف النص المحدد / Delete selected text
        // ============================================================================
        void TextInput::DeleteSelection() {
            if (!HasSelection()) {  // إذا لم يكن هناك تحديد / If no selection
                return;
            }

            // ترتيب البداية والنهاية / Sort start and end
            int start = std::min(m_selectionStart, m_selectionEnd);
            int end = std::max(m_selectionStart, m_selectionEnd);

            // حذف النص / Delete text
            m_text.erase(start, end - start);
            
            // ضبط المؤشر / Set cursor
            m_cursorPos = start;
            
            // مسح التحديد / Clear selection
            ClearSelection();

            // استدعاء دالة التغيير / Call change callback
            if (m_onTextChange) {
                m_onTextChange(m_text);
            }
        }

        // ============================================================================
        // حذف حرف / Delete character
        // ============================================================================
        void TextInput::DeleteChar(bool forward) {
            if (m_text.empty()) {  // إذا كان النص فارغاً / If text is empty
                return;
            }

            if (forward) {  // حذف للأمام / Delete forward
                if (m_cursorPos < (int)m_text.length()) {
                    m_text.erase(m_cursorPos, 1);  // حذف حرف / Delete char
                }
            } else {  // حذف للخلف / Delete backward
                if (m_cursorPos > 0) {
                    m_text.erase(m_cursorPos - 1, 1);  // حذف حرف / Delete char
                    m_cursorPos--;                     // تحريك المؤشر / Move cursor
                }
            }

            // استدعاء دالة التغيير / Call change callback
            if (m_onTextChange) {
                m_onTextChange(m_text);
            }
        }

        // ============================================================================
        // الحصول على موقع الحرف عند X / Get char index at X
        // ============================================================================
        int TextInput::GetCharIndexAtX(float x) {
            if (!m_font || m_text.empty()) {
                return 0;  // البداية / Start
            }

            // البحث عن أقرب موقع / Find closest position
            float currentX = 0.0f;
            std::string displayText = GetDisplayText();

            for (size_t i = 0; i <= displayText.length(); ++i) {
                if (currentX >= x) {
                    return (int)i;  // إرجاع الموقع / Return position
                }

                if (i < displayText.length()) {
                    std::string ch(1, displayText[i]);
                    float charWidth, charHeight;
                    m_font->MeasureText(ch, charWidth, charHeight);
                    currentX += charWidth;
                }
            }

            return (int)displayText.length();  // النهاية / End
        }

        // ============================================================================
        // الحصول على موقع المؤشر X / Get cursor X position
        // ============================================================================
        float TextInput::GetCursorX() const {
            if (!m_font || m_cursorPos == 0) {
                return m_bounds.x + m_style.padding - m_scrollOffset;  // البداية / Start
            }

            // حساب عرض النص قبل المؤشر / Calculate text width before cursor
            std::string displayText = GetDisplayText();
            std::string beforeCursor = displayText.substr(0, m_cursorPos);

            float width, height;
            m_font->MeasureText(beforeCursor, width, height);

            return m_bounds.x + m_style.padding + width - m_scrollOffset;
        }

        // ============================================================================
        // الحصول على النص المعروض / Get display text
        // ============================================================================
        std::string TextInput::GetDisplayText() const {
            if (m_passwordMode && !m_text.empty()) {
                // عرض نجوم بدلاً من النص / Display asterisks instead of text
                return std::string(m_text.length(), '*');
            }
            return m_text;  // عرض النص العادي / Display normal text
        }

        // ============================================================================
        // التأكد من ظهور المؤشر / Ensure cursor visible
        // ============================================================================
        void TextInput::EnsureCursorVisible() {
            float cursorX = GetCursorX();
            float textAreaWidth = m_bounds.width - (m_style.padding * 2.0f);

            // إذا كان المؤشر خارج النطاق / If cursor is out of bounds
            if (cursorX < m_bounds.x + m_style.padding) {
                // تمرير لليسار / Scroll left
                m_scrollOffset = std::max(0.0f, m_scrollOffset - 20.0f);
            } else if (cursorX > m_bounds.x + m_style.padding + textAreaWidth) {
                // تمرير لليمين / Scroll right
                m_scrollOffset += 20.0f;
            }
        }

    } // namespace UI
} // namespace Graphics
