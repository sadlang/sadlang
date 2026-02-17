// ============================================================================
// checkbox.cpp - تطبيق عنصر مربع الاختيار
// Implementation of checkbox widget
// ============================================================================

#include "../include/ui/checkbox.h"                // ملف الرأس / Header file
#include "../include/rendering/renderer2d.h"       // للرسم / For rendering
#include "../include/resources/resource_manager.h" // لإدارة الخطوط / For font management
#include <SDL.h>                                   // للمفاتيح / For keys

using sad::graphics::ResourceManager;

namespace Graphics {
    namespace UI {

        // ============================================================================
        // البناء الافتراضي / Default constructor
        // ============================================================================
        Checkbox::Checkbox()
            : Widget()                           // استدعاء بناء الأساس / Call base constructor
            , m_checked(false)                   // غير محدد افتراضياً / Unchecked by default
            , m_label("")                        // بدون تسمية / No label
            , m_font(nullptr)                    // لا يوجد خط / No font
            , m_checkColor(Color::Black)         // علامة سوداء / Black check
            , m_labelColor(Color::Black)         // تسمية سوداء / Black label
            , m_boxColor(Color::White)           // مربع أبيض / White box
            , m_boxSize(20.0f)                   // حجم 20 بكسل / 20 pixels size
            , m_onCheckChange(nullptr)           // لا توجد دالة / No callback
        {
            // تحميل خط افتراضي / Load default font
            auto& rm = ResourceManager::GetInstance();
            m_font = rm.LoadFont("arial.ttf", 14.0f);  // خط Arial بحجم 14 / Arial font size 14

            // ضبط حجم افتراضي / Set default size
            SetSize(150, 24);

            // نمط افتراضي / Default style
            m_style.backgroundColor = Color::FromBytes(240, 240, 240);  // خلفية فاتحة / Light background
            m_style.borderColor = Color::Black;              // حدود سوداء / Black border
            m_style.borderWidth = 0.0f;                      // بدون حدود / No border
            m_style.padding = 2.0f;                          // مسافة صغيرة / Small padding
        }

        // ============================================================================
        // البناء مع تسمية / Constructor with label
        // ============================================================================
        Checkbox::Checkbox(const std::string& label)
            : Checkbox()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetLabel(label);  // ضبط التسمية / Set label
        }

        // ============================================================================
        // البناء مع تسمية وحالة / Constructor with label and state
        // ============================================================================
        Checkbox::Checkbox(const std::string& label, bool checked)
            : Checkbox(label)  // استدعاء البناء مع تسمية / Call constructor with label
        {
            SetChecked(checked);  // ضبط الحالة / Set checked state
        }

        // ============================================================================
        // التدمير / Destructor
        // ============================================================================
        Checkbox::~Checkbox() {
            // لا حاجة لتنظيف إضافي / No additional cleanup needed
        }

        // ============================================================================
        // ضبط الحالة / Set checked state
        // ============================================================================
        void Checkbox::SetChecked(bool checked) {
            if (m_checked != checked) {  // إذا تغيرت الحالة / If state changed
                m_checked = checked;     // تحديث الحالة / Update state
                
                // استدعاء دالة التغيير / Call change callback
                if (m_onCheckChange) {
                    m_onCheckChange(m_checked);
                }
            }
        }

        // ============================================================================
        // تبديل الحالة / Toggle state
        // ============================================================================
        void Checkbox::Toggle() {
            SetChecked(!m_checked);  // عكس الحالة / Invert state
        }

        // ============================================================================
        // ضبط التسمية / Set label
        // ============================================================================
        void Checkbox::SetLabel(const std::string& label) {
            m_label = label;  // تحديث التسمية / Update label
        }

        // ============================================================================
        // ضبط الخط / Set font
        // ============================================================================
        void Checkbox::SetFont(FontRef font) {
            if (font) {  // التحقق من صحة الخط / Check font validity
                m_font = font;  // تحديث الخط / Update font
            }
        }

        // ============================================================================
        // ضبط حجم المربع / Set box size
        // ============================================================================
        void Checkbox::SetBoxSize(float size) {
            m_boxSize = std::max(10.0f, size);  // الحد الأدنى 10 / Minimum 10
        }

        // ============================================================================
        // رسم مربع الاختيار / Draw checkbox
        // ============================================================================
        void Checkbox::Draw(SadGraphics::Renderer2D* renderer) {
            // إذا لم يكن ظاهراً / If not visible
            if (!m_visible) {
                return;
            }

            // رسم الخلفية / Draw background
            Widget::Draw(renderer);

            // حساب موقع المربع / Calculate box position
            float boxX = m_bounds.x + m_style.padding;
            float boxY = m_bounds.y + (m_bounds.height - m_boxSize) / 2.0f;

            // رسم المربع / Draw box
            renderer->DrawRect(boxX, boxY, m_boxSize, m_boxSize, m_boxColor, true);  // تعبئة / Fill
            renderer->DrawRect(boxX, boxY, m_boxSize, m_boxSize, Color::Black, false);  // حدود / Border

            // رسم العلامة إذا كان محدداً / Draw check mark if checked
            if (m_checked) {
                // رسم علامة صح (✓) / Draw check mark (✓)
                float checkPadding = m_boxSize * 0.2f;  // مسافة داخلية / Inner padding
                float checkX1 = boxX + checkPadding;
                float checkY1 = boxY + m_boxSize / 2.0f;
                float checkX2 = boxX + m_boxSize / 2.0f;
                float checkY2 = boxY + m_boxSize - checkPadding;
                float checkX3 = boxX + m_boxSize - checkPadding;
                float checkY3 = boxY + checkPadding;

                // رسم خطوط العلامة / Draw check mark lines
                renderer->DrawLine(checkX1, checkY1, checkX2, checkY2, m_checkColor, 2.0f);
                renderer->DrawLine(checkX2, checkY2, checkX3, checkY3, m_checkColor, 2.0f);
            }

            // رسم التسمية / Draw label
            if (!m_label.empty() && m_font) {
                float labelX = boxX + m_boxSize + 8.0f;  // مسافة من المربع / Space from box
                float labelY = boxY;
                
                // تطبيق الشفافية / Apply opacity
                Color finalColor = m_labelColor;
                finalColor.a *= m_style.opacity;

                renderer->DrawText(m_label, m_font, labelX, labelY, finalColor);
            }
        }

        // ============================================================================
        // ضغط زر الفأرة / Mouse button down
        // ============================================================================
        bool Checkbox::OnMouseDown(int x, int y, int button) {
            // إذا لم يكن مفعّلاً / If not enabled
            if (!m_enabled || !m_visible) {
                return false;
            }

            // التحقق من النقر داخل العنصر / Check if click is inside widget
            if (ContainsPoint((float)x, (float)y)) {
                Toggle();  // تبديل الحالة / Toggle state
                SetFocused(true);  // إعطاء التركيز / Give focus
                return true;
            }

            return false;
        }

        // ============================================================================
        // ضغط مفتاح / Key down
        // ============================================================================
        bool Checkbox::OnKeyDown(int keyCode) {
            // إذا لم يكن له التركيز / If not focused
            if (!m_focused || !m_enabled) {
                return false;
            }

            // مفتاح Space أو Enter للتبديل / Space or Enter key to toggle
            if (keyCode == SDLK_SPACE || keyCode == SDLK_RETURN) {
                Toggle();  // تبديل الحالة / Toggle state
                return true;
            }

            return false;
        }

        // ============================================================================
        // هل النقطة داخل المربع؟ / Is point in box?
        // ============================================================================
        bool Checkbox::IsPointInBox(float x, float y) const {
            float boxX = m_bounds.x + m_style.padding;
            float boxY = m_bounds.y + (m_bounds.height - m_boxSize) / 2.0f;
            
            return x >= boxX && x <= (boxX + m_boxSize) &&
                   y >= boxY && y <= (boxY + m_boxSize);
        }

    } // namespace UI
} // namespace Graphics
