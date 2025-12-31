// ============================================================================
// button.cpp - تطبيق عنصر الزر
// Implementation of button widget
// ============================================================================

#include "../include/ui/button.h"          // ملف الرأس / Header file
#include "../include/rendering/renderer2d.h"  // للرسم / For rendering

namespace Graphics {
    namespace UI {

        // ============================================================================
        // البناء الافتراضي / Default constructor
        // ============================================================================
        Button::Button()
            : Label()                     // استدعاء بناء Label / Call Label constructor
            , m_onClick(nullptr)          // لا توجد دالة نقر / No click callback
            , m_onHover(nullptr)          // لا توجد دالة تحويم / No hover callback
            , m_onPress(nullptr)          // لا توجد دالة ضغط / No press callback
            , m_onRelease(nullptr)        // لا توجد دالة رفع / No release callback
            , m_isPressed(false)          // غير مضغوط / Not pressed
        {
            // إعداد الأنماط الافتراضية / Setup default styles
            
            // نمط عادي / Normal style
            m_normalStyle = m_style;
            m_normalStyle.backgroundColor = Color(200, 200, 200);  // رمادي فاتح / Light gray
            m_normalStyle.foregroundColor = Color::Black;           // نص أسود / Black text
            m_normalStyle.borderColor = Color(150, 150, 150);       // حدود رمادية / Gray border
            m_normalStyle.borderWidth = 2.0f;                       // عرض حدود / Border width

            // نمط التحويم / Hover style
            m_hoverStyle = m_normalStyle;
            m_hoverStyle.backgroundColor = Color(220, 220, 220);    // رمادي أفتح / Lighter gray
            m_hoverStyle.borderColor = Color(100, 100, 255);        // حدود زرقاء / Blue border

            // نمط الضغط / Pressed style
            m_pressedStyle = m_normalStyle;
            m_pressedStyle.backgroundColor = Color(150, 150, 150);  // رمادي داكن / Darker gray
            m_pressedStyle.borderColor = Color(50, 50, 200);        // حدود زرقاء داكنة / Dark blue border

            // نمط معطل / Disabled style
            m_disabledStyle = m_normalStyle;
            m_disabledStyle.backgroundColor = Color(180, 180, 180); // رمادي باهت / Pale gray
            m_disabledStyle.foregroundColor = Color(120, 120, 120); // نص رمادي / Gray text
            m_disabledStyle.opacity = 0.6f;                         // شفافية / Opacity

            // تطبيق النمط العادي / Apply normal style
            m_style = m_normalStyle;
            
            // تعطيل الحجم التلقائي / Disable auto size
            m_autoSize = false;
            
            // ضبط حجم افتراضي / Set default size
            SetSize(120, 40);
        }

        // ============================================================================
        // البناء بنص / Constructor with text
        // ============================================================================
        Button::Button(const std::string& text)
            : Button()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetText(text);  // ضبط النص / Set text
        }

        // ============================================================================
        // البناء بنص ودالة / Constructor with text and callback
        // ============================================================================
        Button::Button(const std::string& text, EventCallback onClick)
            : Button(text)  // استدعاء البناء بنص / Call constructor with text
        {
            m_onClick = onClick;  // ضبط دالة النقر / Set click callback
        }

        // ============================================================================
        // التدمير / Destructor
        // ============================================================================
        Button::~Button() {
            // لا حاجة لتنظيف إضافي / No additional cleanup needed
        }

        // ============================================================================
        // حركة الفأرة / Mouse move
        // ============================================================================
        bool Button::OnMouseMove(int x, int y) {
            // إذا لم يكن مفعّلاً / If not enabled
            if (!m_enabled || !m_visible) {
                return false;
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it) && (*it)->OnMouseMove(x, y)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            // التحقق من وجود الفأرة فوق الزر / Check if mouse is over button
            bool inside = ContainsPoint((float)x, (float)y);

            // إذا دخلت الفأرة للزر / If mouse entered button
            if (inside && m_state == WidgetState::Normal) {
                SetState(WidgetState::Hovered);  // تغيير للحالة Hovered / Change to Hovered
                
                // استدعاء دالة التحويم / Call hover callback
                if (m_onHover) {
                    m_onHover();
                }
                
                return true;
            }
            // إذا خرجت الفأرة من الزر / If mouse left button
            else if (!inside && m_state == WidgetState::Hovered) {
                SetState(WidgetState::Normal);  // العودة للحالة العادية / Return to Normal
                return true;
            }

            return inside;
        }

        // ============================================================================
        // ضغط زر الفأرة / Mouse button down
        // ============================================================================
        bool Button::OnMouseDown(int x, int y, int button) {
            // إذا لم يكن مفعّلاً / If not enabled
            if (!m_enabled || !m_visible) {
                return false;
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it) && (*it)->OnMouseDown(x, y, button)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            // التحقق من النقر داخل الزر / Check if click is inside button
            if (ContainsPoint((float)x, (float)y)) {
                m_isPressed = true;                   // الزر مضغوط / Button pressed
                SetState(WidgetState::Pressed);       // تغيير الحالة / Change state
                
                // استدعاء دالة الضغط / Call press callback
                if (m_onPress) {
                    m_onPress();
                }
                
                return true;
            }

            return false;
        }

        // ============================================================================
        // رفع زر الفأرة / Mouse button up
        // ============================================================================
        bool Button::OnMouseUp(int x, int y, int button) {
            // إذا لم يكن مفعّلاً / If not enabled
            if (!m_enabled || !m_visible) {
                return false;
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it) && (*it)->OnMouseUp(x, y, button)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            // إذا كان الزر مضغوطاً / If button was pressed
            if (m_isPressed) {
                m_isPressed = false;  // إلغاء الضغط / Release press
                
                // استدعاء دالة الرفع / Call release callback
                if (m_onRelease) {
                    m_onRelease();
                }

                // التحقق من موقع الرفع / Check release position
                bool inside = ContainsPoint((float)x, (float)y);
                
                if (inside) {
                    // النقر كامل داخل الزر / Complete click inside button
                    SetState(WidgetState::Hovered);  // العودة لـ Hovered / Return to Hovered
                    
                    // استدعاء دالة النقر / Call click callback
                    if (m_onClick) {
                        m_onClick();
                    }
                } else {
                    // الرفع خارج الزر / Release outside button
                    SetState(WidgetState::Normal);  // العودة لـ Normal / Return to Normal
                }
                
                return true;
            }

            return false;
        }

        // ============================================================================
        // رسم الزر / Draw button
        // ============================================================================
        void Button::Draw(Renderer2D* renderer) {
            // إذا لم يكن ظاهراً / If not visible
            if (!m_visible) {
                return;
            }

            // تحديث النمط حسب الحالة / Update style for state
            UpdateStyleForState();

            // رسم باستخدام Label / Draw using Label
            Label::Draw(renderer);

            // يمكن إضافة تأثيرات إضافية هنا / Can add additional effects here
            // مثل ظل أو توهج / Such as shadow or glow
        }

        // ============================================================================
        // عند تغيير الحالة / When state changes
        // ============================================================================
        void Button::OnStateChanged() {
            // تحديث النمط / Update style
            UpdateStyleForState();
            
            // استدعاء الأساس / Call base
            Label::OnStateChanged();
        }

        // ============================================================================
        // تحديث النمط حسب الحالة / Update style for state
        // ============================================================================
        void Button::UpdateStyleForState() {
            // اختيار النمط حسب الحالة / Choose style based on state
            switch (m_state) {
                case WidgetState::Normal:
                    m_style = m_normalStyle;    // نمط عادي / Normal style
                    break;

                case WidgetState::Hovered:
                    m_style = m_hoverStyle;     // نمط التحويم / Hover style
                    break;

                case WidgetState::Pressed:
                    m_style = m_pressedStyle;   // نمط الضغط / Pressed style
                    break;

                case WidgetState::Focused:
                    m_style = m_hoverStyle;     // نفس نمط التحويم / Same as hover
                    break;

                case WidgetState::Disabled:
                    m_style = m_disabledStyle;  // نمط معطل / Disabled style
                    break;
            }
        }

    } // namespace UI
} // namespace Graphics
