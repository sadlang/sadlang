// ============================================================================
// label.cpp - تطبيق عنصر التسمية
// Implementation of label widget
// ============================================================================

#include "../include/ui/label.h"                   // ملف الرأس / Header file
#include "../include/rendering/renderer2d.h"       // للرسم / For rendering
#include "../include/resources/resource_manager.h" // لإدارة الخطوط / For font management

namespace Graphics {
    namespace UI {

        // ============================================================================
        // البناء الافتراضي / Default constructor
        // ============================================================================
        Label::Label()
            : Widget()                             // استدعاء بناء الأساس / Call base constructor
            , m_text("")                           // نص فارغ / Empty text
            , m_font(nullptr)                      // لا يوجد خط / No font
            , m_textColor(Color::Black)            // لون أسود افتراضياً / Black by default
            , m_alignment(TextAlignment::MiddleCenter)  // محاذاة وسطية / Center alignment
            , m_wordWrap(false)                    // لا لف للنص / No word wrap
            , m_autoSize(true)                     // حجم تلقائي / Auto size enabled
        {
            // تحميل خط افتراضي / Load default font
            auto& rm = ResourceManager::GetInstance();
            m_font = rm.LoadFont("arial.ttf", 16.0f);  // خط Arial بحجم 16 / Arial font size 16
        }

        // ============================================================================
        // البناء بنص / Constructor with text
        // ============================================================================
        Label::Label(const std::string& text)
            : Label()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetText(text);  // ضبط النص / Set text
        }

        // ============================================================================
        // البناء بنص وخط / Constructor with text and font
        // ============================================================================
        Label::Label(const std::string& text, FontRef font)
            : Label()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetText(text);  // ضبط النص / Set text
            SetFont(font);  // ضبط الخط / Set font
        }

        // ============================================================================
        // التدمير / Destructor
        // ============================================================================
        Label::~Label() {
            // لا حاجة لتنظيف إضافي / No additional cleanup needed
            // المؤشرات الذكية تهتم بنفسها / Smart pointers handle themselves
        }

        // ============================================================================
        // ضبط النص / Set text
        // ============================================================================
        void Label::SetText(const std::string& text) {
            if (m_text != text) {  // إذا تغير النص / If text changed
                m_text = text;     // تحديث النص / Update text
                
                // تحديث الحجم إذا كان تلقائياً / Update size if auto
                if (m_autoSize) {
                    UpdateSize();
                }
            }
        }

        // ============================================================================
        // ضبط الخط / Set font
        // ============================================================================
        void Label::SetFont(FontRef font) {
            if (font && m_font != font) {  // إذا تغير الخط / If font changed
                m_font = font;             // تحديث الخط / Update font
                
                // تحديث الحجم إذا كان تلقائياً / Update size if auto
                if (m_autoSize) {
                    UpdateSize();
                }
            }
        }

        // ============================================================================
        // ضبط المحاذاة / Set alignment
        // ============================================================================
        void Label::SetAlignment(TextAlignment alignment) {
            m_alignment = alignment;  // تحديث المحاذاة / Update alignment
        }

        // ============================================================================
        // قياس النص / Measure text
        // ============================================================================
        void Label::MeasureText(float& width, float& height) const {
            // إذا لم يكن هناك خط / If no font
            if (!m_font) {
                width = 0;   // عرض صفر / Zero width
                height = 0;  // ارتفاع صفر / Zero height
                return;
            }

            // قياس النص باستخدام الخط / Measure text using font
            m_font->MeasureText(m_text, width, height);
        }

        // ============================================================================
        // حساب موقع النص / Calculate text position
        // ============================================================================
        void Label::CalculateTextPosition(float& x, float& y) const {
            // قياس أبعاد النص / Measure text dimensions
            float textWidth, textHeight;
            MeasureText(textWidth, textHeight);

            // البداية من موقع العنصر / Start from widget position
            x = m_bounds.x;
            y = m_bounds.y;

            // تطبيق المحاذاة / Apply alignment
            switch (m_alignment) {
                case TextAlignment::Left:
                case TextAlignment::TopLeft:
                case TextAlignment::MiddleLeft:
                case TextAlignment::BottomLeft:
                    // محاذاة لليسار / Left aligned
                    x += m_style.padding;
                    break;

                case TextAlignment::Center:
                case TextAlignment::TopCenter:
                case TextAlignment::MiddleCenter:
                case TextAlignment::BottomCenter:
                    // محاذاة للمركز / Center aligned
                    x += (m_bounds.width - textWidth) / 2.0f;
                    break;

                case TextAlignment::Right:
                case TextAlignment::TopRight:
                case TextAlignment::MiddleRight:
                case TextAlignment::BottomRight:
                    // محاذاة لليمين / Right aligned
                    x += m_bounds.width - textWidth - m_style.padding;
                    break;
            }

            // المحاذاة العمودية / Vertical alignment
            switch (m_alignment) {
                case TextAlignment::TopLeft:
                case TextAlignment::TopCenter:
                case TextAlignment::TopRight:
                    // أعلى / Top
                    y += m_style.padding;
                    break;

                case TextAlignment::Left:
                case TextAlignment::Center:
                case TextAlignment::Right:
                case TextAlignment::MiddleLeft:
                case TextAlignment::MiddleCenter:
                case TextAlignment::MiddleRight:
                    // وسط / Middle
                    y += (m_bounds.height - textHeight) / 2.0f;
                    break;

                case TextAlignment::BottomLeft:
                case TextAlignment::BottomCenter:
                case TextAlignment::BottomRight:
                    // أسفل / Bottom
                    y += m_bounds.height - textHeight - m_style.padding;
                    break;
            }
        }

        // ============================================================================
        // تحديث الحجم / Update size
        // ============================================================================
        void Label::UpdateSize() {
            // إذا لم يكن الحجم التلقائي مفعّلاً / If auto size not enabled
            if (!m_autoSize) {
                return;
            }

            // قياس النص / Measure text
            float textWidth, textHeight;
            MeasureText(textWidth, textHeight);

            // إضافة المسافة الداخلية / Add padding
            float totalWidth = textWidth + (m_style.padding * 2.0f);
            float totalHeight = textHeight + (m_style.padding * 2.0f);

            // تحديث الحجم / Update size
            SetSize(totalWidth, totalHeight);
        }

        // ============================================================================
        // رسم التسمية / Draw label
        // ============================================================================
        void Label::Draw(Renderer2D* renderer) {
            // إذا لم يكن ظاهراً / If not visible
            if (!m_visible) {
                return;
            }

            // رسم الخلفية والحدود / Draw background and borders
            Widget::Draw(renderer);  // استدعاء رسم الأساس / Call base draw

            // إذا لم يكن هناك نص أو خط / If no text or font
            if (m_text.empty() || !m_font) {
                return;  // لا نرسم نص / Don't draw text
            }

            // حساب موقع النص / Calculate text position
            float textX, textY;
            CalculateTextPosition(textX, textY);

            // تطبيق الشفافية على اللون / Apply opacity to color
            Color finalColor = m_textColor;
            finalColor.a = (unsigned char)(m_style.opacity * 255.0f * (m_textColor.a / 255.0f));

            // رسم النص / Draw text
            renderer->DrawText(m_text, m_font, textX, textY, finalColor);

            // رسم العناصر الفرعية / Draw children
            // (بالفعل تم رسمهم في Widget::Draw، لكن نذكرهم للوضوح)
            // (Already drawn in Widget::Draw, but mentioned for clarity)
        }

    } // namespace UI
} // namespace Graphics
