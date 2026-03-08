// ============================================================================
// widget.cpp - تطبيق الفئة الأساسية للعناصر
// Implementation of base widget class
// ============================================================================

#include "../include/ui/widget.h"                  // ملف الرأس الخاص بـ Widget / Widget header
#include "../include/rendering/renderer2d.h"      // لاستخدام Renderer2D / For Renderer2D
#include <algorithm>                              // لاستخدام الخوارزميات / For algorithms

namespace sad {
namespace graphics {
    namespace ui {

        // تهيئة المعرف الثابت / Initialize static ID counter
        int Widget::s_nextID = 1;  // البدء من 1 / Start from 1

        // ============================================================================
        // البناء / Construction
        // ============================================================================
        Widget::Widget()
            : m_id(s_nextID++)              // تعيين معرف فريد وزيادة العداد / Assign unique ID and increment
            , m_name("")                    // اسم فارغ افتراضياً / Empty name by default
            , m_bounds(0, 0, 100, 100)      // حجم افتراضي 100×100 / Default size 100×100
            , m_visible(true)               // ظاهر افتراضياً / Visible by default
            , m_enabled(true)               // مفعّل افتراضياً / Enabled by default
            , m_focused(false)              // ليس له التركيز افتراضياً / Not focused by default
            , m_state(WidgetState::Normal)  // الحالة العادية / Normal state
            , m_style()                     // النمط الافتراضي / Default style
            , m_parent(nullptr)             // لا يوجد أب / No parent
        {
            // لا حاجة لمزيد من التهيئة / No further initialization needed
        }

        // ============================================================================
        // التدمير / Destruction
        // ============================================================================
        Widget::~Widget() {
            // مسح جميع العناصر الفرعية / Clear all children
            ClearChildren();
        }

        // ============================================================================
        // ضبط الموقع / Set position
        // ============================================================================
        void Widget::SetPosition(float x, float y) {
            m_bounds.x = x;  // ضبط الموقع الأفقي / Set horizontal position
            m_bounds.y = y;  // ضبط الموقع العمودي / Set vertical position
        }

        // ============================================================================
        // ضبط الحجم / Set size
        // ============================================================================
        void Widget::SetSize(float width, float height) {
            m_bounds.width = width;    // ضبط العرض / Set width
            m_bounds.height = height;  // ضبط الارتفاع / Set height
        }

        // ============================================================================
        // ضبط المنطقة / Set bounds
        // ============================================================================
        void Widget::SetBounds(const Rect& bounds) {
            m_bounds = bounds;  // نسخ المنطقة / Copy bounds
        }

        // ============================================================================
        // ضبط التفعيل / Set enabled
        // ============================================================================
        void Widget::SetEnabled(bool enabled) {
            if (m_enabled != enabled) {  // إذا تغيرت الحالة / If state changed
                m_enabled = enabled;     // تحديث الحالة / Update state
                
                // تغيير حالة العنصر / Change widget state
                if (!enabled) {
                    SetState(WidgetState::Disabled);  // معطل / Disabled
                } else {
                    SetState(WidgetState::Normal);    // عادي / Normal
                }
            }
        }

        // ============================================================================
        // ضبط الحالة / Set state
        // ============================================================================
        void Widget::SetState(WidgetState state) {
            if (m_state != state) {  // إذا تغيرت الحالة / If state changed
                m_state = state;     // تحديث الحالة / Update state
                OnStateChanged();    // استدعاء دالة التغيير / Call change handler
            }
        }

        // ============================================================================
        // ضبط التركيز / Set focus
        // ============================================================================
        void Widget::SetFocused(bool focused) {
            if (m_focused != focused) {  // إذا تغيرت حالة التركيز / If focus state changed
                m_focused = focused;     // تحديث التركيز / Update focus
                
                // تحديث الحالة / Update state
                if (focused && m_enabled) {
                    SetState(WidgetState::Focused);  // له التركيز / Focused
                } else if (m_state == WidgetState::Focused) {
                    SetState(WidgetState::Normal);   // عادي / Normal
                }
            }
        }

        // ============================================================================
        // رسم العنصر / Draw widget
        // ============================================================================
        void Widget::Draw(SadGraphics::Renderer2D* renderer) {
            // إذا لم يكن العنصر ظاهراً، لا نرسمه / If not visible, don't draw
            if (!m_visible) {
                return;
            }

            // رسم الخلفية / Draw background
            Color bgColor = m_style.backgroundColor;           // لون الخلفية / Background color
            bgColor.a *= m_style.opacity;  // تطبيق الشفافية / Apply opacity

            // رسم مستطيل الخلفية / Draw background rectangle
            if (m_style.borderRadius > 0.0f) {
                // مستطيل بزوايا دائرية / Rectangle with rounded corners
                // ملاحظة: سنحتاج لدالة DrawRoundedRect في المستقبل
                // Note: We'll need DrawRoundedRect function in future
                renderer->DrawRect(m_bounds.x, m_bounds.y, 
                                 m_bounds.width, m_bounds.height, 
                                 bgColor, true);
            } else {
                // مستطيل عادي / Regular rectangle
                renderer->DrawRect(m_bounds.x, m_bounds.y, 
                                 m_bounds.width, m_bounds.height, 
                                 bgColor, true);
            }

            // رسم الحدود إذا كانت موجودة / Draw border if present
            if (m_style.borderWidth > 0.0f) {
                Color borderColor = m_style.borderColor;  // لون الحدود / Border color
                borderColor.a *= m_style.opacity;  // تطبيق الشفافية / Apply opacity

                // رسم مستطيل الحدود / Draw border rectangle
                renderer->DrawRect(m_bounds.x, m_bounds.y, 
                                 m_bounds.width, m_bounds.height, 
                                 borderColor, false);
            }

            // رسم العناصر الفرعية / Draw children
            for (auto& child : m_children) {
                if (child) {  // التحقق من صحة المؤشر / Check pointer validity
                    child->Draw(renderer);  // رسم العنصر الفرعي / Draw child
                }
            }
        }

        // ============================================================================
        // تحديث العنصر / Update widget
        // ============================================================================
        void Widget::Update(float deltaTime) {
            // إذا لم يكن العنصر مفعّلاً، لا نحدثه / If not enabled, don't update
            if (!m_enabled) {
                return;
            }

            // تحديث العناصر الفرعية / Update children
            for (auto& child : m_children) {
                if (child) {  // التحقق من صحة المؤشر / Check pointer validity
                    child->Update(deltaTime);  // تحديث العنصر الفرعي / Update child
                }
            }
        }

        // ============================================================================
        // حركة الفأرة / Mouse move
        // ============================================================================
        bool Widget::OnMouseMove(int x, int y) {
            // إذا لم يكن العنصر مفعّلاً أو ظاهراً / If not enabled or visible
            if (!m_enabled || !m_visible) {
                return false;  // لم نعالج الحدث / Event not handled
            }

            // معالجة العناصر الفرعية أولاً (من الأعلى للأسفل) / Handle children first (top to bottom)
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it) && (*it)->OnMouseMove(x, y)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            // التحقق من وجود الفأرة فوق العنصر / Check if mouse is over widget
            bool inside = ContainsPoint((float)x, (float)y);  // تحويل للأعداد العشرية / Convert to floats

            // تحديث الحالة بناءً على الموقع / Update state based on position
            if (inside && m_state == WidgetState::Normal) {
                SetState(WidgetState::Hovered);  // تغيير للحالة Hovered / Change to Hovered
                return true;  // معالج / Handled
            } else if (!inside && m_state == WidgetState::Hovered) {
                SetState(WidgetState::Normal);  // العودة للحالة العادية / Return to Normal
            }

            return inside;  // إرجاع نتيجة الاحتواء / Return containment result
        }

        // ============================================================================
        // ضغط زر الفأرة / Mouse button down
        // ============================================================================
        bool Widget::OnMouseDown(int x, int y, int button) {
            // إذا لم يكن العنصر مفعّلاً أو ظاهراً / If not enabled or visible
            if (!m_enabled || !m_visible) {
                return false;  // لم نعالج الحدث / Event not handled
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it) && (*it)->OnMouseDown(x, y, button)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            // التحقق من وجود النقر داخل العنصر / Check if click is inside widget
            if (ContainsPoint((float)x, (float)y)) {
                SetState(WidgetState::Pressed);  // تغيير للحالة Pressed / Change to Pressed
                SetFocused(true);                // إعطاء التركيز / Give focus
                return true;                     // معالج / Handled
            }

            return false;  // لم نعالج / Not handled
        }

        // ============================================================================
        // رفع زر الفأرة / Mouse button up
        // ============================================================================
        bool Widget::OnMouseUp(int x, int y, int button) {
            // إذا لم يكن العنصر مفعّلاً أو ظاهراً / If not enabled or visible
            if (!m_enabled || !m_visible) {
                return false;  // لم نعالج الحدث / Event not handled
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
                if ((*it) && (*it)->OnMouseUp(x, y, button)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            // إذا كان العنصر في حالة Pressed / If widget is in Pressed state
            if (m_state == WidgetState::Pressed) {
                // التحقق من موقع الرفع / Check release position
                if (ContainsPoint((float)x, (float)y)) {
                    SetState(WidgetState::Hovered);  // العودة لـ Hovered / Return to Hovered
                } else {
                    SetState(WidgetState::Normal);   // العودة لـ Normal / Return to Normal
                }
                return true;  // معالج / Handled
            }

            return false;  // لم نعالج / Not handled
        }

        // ============================================================================
        // ضغط مفتاح / Key down
        // ============================================================================
        bool Widget::OnKeyDown(int keyCode) {
            // إذا لم يكن له التركيز / If not focused
            if (!m_focused || !m_enabled || !m_visible) {
                return false;  // لم نعالج / Not handled
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto& child : m_children) {
                if (child && child->OnKeyDown(keyCode)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            return false;  // لم نعالج / Not handled
        }

        // ============================================================================
        // رفع مفتاح / Key up
        // ============================================================================
        bool Widget::OnKeyUp(int keyCode) {
            // إذا لم يكن له التركيز / If not focused
            if (!m_focused || !m_enabled || !m_visible) {
                return false;  // لم نعالج / Not handled
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto& child : m_children) {
                if (child && child->OnKeyUp(keyCode)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            return false;  // لم نعالج / Not handled
        }

        // ============================================================================
        // إدخال نص / Text input
        // ============================================================================
        bool Widget::OnTextInput(const std::string& text) {
            // إذا لم يكن له التركيز / If not focused
            if (!m_focused || !m_enabled || !m_visible) {
                return false;  // لم نعالج / Not handled
            }

            // معالجة العناصر الفرعية أولاً / Handle children first
            for (auto& child : m_children) {
                if (child && child->OnTextInput(text)) {
                    return true;  // معالج بواسطة عنصر فرعي / Handled by child
                }
            }

            return false;  // لم نعالج / Not handled
        }

        // ============================================================================
        // إضافة عنصر فرعي / Add child
        // ============================================================================
        void Widget::AddChild(std::shared_ptr<Widget> child) {
            if (!child) {  // التحقق من صحة المؤشر / Check pointer validity
                return;    // لا نفعل شيء / Do nothing
            }

            // إضافة للقائمة / Add to list
            m_children.push_back(child);
            
            // ضبط الأب / Set parent
            child->m_parent = this;
        }

        // ============================================================================
        // إزالة عنصر فرعي / Remove child
        // ============================================================================
        void Widget::RemoveChild(std::shared_ptr<Widget> child) {
            if (!child) {  // التحقق من صحة المؤشر / Check pointer validity
                return;    // لا نفعل شيء / Do nothing
            }

            // البحث عن العنصر / Find child
            auto it = std::find(m_children.begin(), m_children.end(), child);
            
            // إذا وجدنا العنصر / If found
            if (it != m_children.end()) {
                (*it)->m_parent = nullptr;  // إزالة الأب / Remove parent
                m_children.erase(it);       // حذف من القائمة / Remove from list
            }
        }

        // ============================================================================
        // مسح جميع العناصر الفرعية / Clear all children
        // ============================================================================
        void Widget::ClearChildren() {
            // إزالة الأب من كل عنصر / Remove parent from each child
            for (auto& child : m_children) {
                if (child) {
                    child->m_parent = nullptr;
                }
            }
            
            // مسح القائمة / Clear list
            m_children.clear();
        }

        // ============================================================================
        // التحقق من احتواء نقطة / Check if point is inside
        // ============================================================================
        bool Widget::ContainsPoint(float x, float y) const {
            return m_bounds.Contains(x, y);  // استخدام دالة المستطيل / Use rectangle function
        }

        // ============================================================================
        // عند تغيير الحالة / When state changes
        // ============================================================================
        void Widget::OnStateChanged() {
            // يمكن للفئات المشتقة تنفيذ هذه الدالة / Derived classes can override this
            // لإضافة سلوك خاص عند تغيير الحالة / To add custom behavior on state change
        }

        // ============================================================================
        // عند تغيير النمط / When style changes
        // ============================================================================
        void Widget::OnStyleChanged() {
            // يمكن للفئات المشتقة تنفيذ هذه الدالة / Derived classes can override this
            // لإضافة سلوك خاص عند تغيير النمط / To add custom behavior on style change
        }

    } // namespace ui
} // namespace graphics
} // namespace sad
