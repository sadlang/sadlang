// ============================================================================
// widget.h - الفئة الأساسية للعناصر الرسومية
// Base class for UI widgets
// ============================================================================
// الوصف: فئة أساسية لجميع عناصر واجهة المستخدم (Widgets)
// Description: Base class for all UI widgets
// ============================================================================

#ifndef GRAPHICS_UI_WIDGET_H
#define GRAPHICS_UI_WIDGET_H

#include <string>          // لاستخدام النصوص / For strings
#include <memory>          // لاستخدام المؤشرات الذكية / For smart pointers
#include <vector>          // لاستخدام المصفوفات / For vectors
#include <functional>      // لاستخدام الدوال / For functions
#include "../core/types.h" // لاستخدام الأنواع الأساسية / For basic types
#include "../text/font.h"  // لاستخدام الخطوط / For fonts (FontRef)

// استخدام أنواع من مكتبة الرسومات / Use types from graphics library
using sad::graphics::Color;
using sad::graphics::Float32;
using sad::graphics::Int32;
using sad::graphics::UInt32;
using sad::graphics::FontRef;
using sad::graphics::Point2D;

// التصريح المسبق لمحرك الرسم / Forward declaration for renderer
namespace SadGraphics { class Renderer2D; }

namespace Graphics {
    // المساحة الاسمية للواجهة / Namespace for UI
    namespace UI {

        // تعريف أنواع معرفات الأحداث / Event callback types
        using EventCallback = std::function<void()>;                          // دالة رد بدون معاملات / Callback without parameters
        using MouseEventCallback = std::function<void(int x, int y)>;        // دالة رد لأحداث الفأرة / Mouse event callback
        using KeyEventCallback = std::function<void(int keyCode)>;           // دالة رد لأحداث المفاتيح / Key event callback
        using FocusEventCallback = std::function<void(bool focused)>;        // دالة رد لأحداث التركيز / Focus event callback

        // ============================================================================
        // Rect - مستطيل لتحديد المنطقة
        // Rectangle for defining widget bounds
        // ============================================================================
        struct Rect {
            float x;        // الموقع الأفقي / Horizontal position
            float y;        // الموقع العمودي / Vertical position
            float width;    // العرض / Width
            float height;   // الارتفاع / Height

            // البناء الافتراضي / Default constructor
            Rect() : x(0), y(0), width(0), height(0) {}

            // البناء بمعاملات / Constructor with parameters
            Rect(float x, float y, float w, float h) 
                : x(x), y(y), width(w), height(h) {}

            // التحقق من احتواء نقطة / Check if point is inside
            bool Contains(float px, float py) const {
                return px >= x && px <= (x + width) && 
                       py >= y && py <= (y + height);
            }

            // الحصول على المركز / Get center point
            void GetCenter(float& cx, float& cy) const {
                cx = x + width / 2.0f;   // حساب المركز الأفقي / Calculate center X
                cy = y + height / 2.0f;  // حساب المركز العمودي / Calculate center Y
            }

            // الحصول على المركز كنقطة / Get center as point
            Point2D GetCenter() const {
                return Point2D(x + width / 2.0f, y + height / 2.0f);
            }
        };

        // ============================================================================
        // WidgetState - حالة العنصر
        // Widget state enumeration
        // ============================================================================
        enum class WidgetState {
            Normal,      // حالة عادية / Normal state
            Hovered,     // الفأرة فوق العنصر / Mouse hovering
            Pressed,     // العنصر مضغوط / Widget pressed
            Focused,     // العنصر له التركيز / Widget focused
            Disabled     // العنصر معطل / Widget disabled
        };

        // ============================================================================
        // WidgetStyle - نمط العنصر
        // Widget styling properties
        // ============================================================================
        struct WidgetStyle {
            Color backgroundColor;    // لون الخلفية / Background color
            Color foregroundColor;    // لون المقدمة (النص) / Foreground (text) color
            Color borderColor;        // لون الحدود / Border color
            
            float borderWidth;        // عرض الحدود / Border width
            float borderRadius;       // نصف قطر زوايا الحدود / Border corner radius
            float padding;            // المسافة الداخلية / Internal padding
            float opacity;            // الشفافية (0-1) / Opacity (0-1)

            // البناء الافتراضي / Default constructor
            WidgetStyle()
                : backgroundColor(Color::White)
                , foregroundColor(Color::Black)
                , borderColor(Color::Gray)
                , borderWidth(1.0f)
                , borderRadius(0.0f)
                , padding(5.0f)
                , opacity(1.0f)
            {}
        };

        // ============================================================================
        // Widget - الفئة الأساسية للعناصر
        // Base widget class
        // ============================================================================
        class Widget {
        public:
            // البناء والتدمير / Construction and destruction
            Widget();                                      // البناء الافتراضي / Default constructor
            virtual ~Widget();                             // المدمر الافتراضي / Virtual destructor

            // إدارة الموقع والحجم / Position and size management
            virtual void SetPosition(float x, float y);    // ضبط الموقع / Set position
            virtual void SetSize(float width, float height); // ضبط الحجم / Set size
            virtual void SetBounds(const Rect& bounds);    // ضبط المنطقة / Set bounds
            
            const Rect& GetBounds() const { return m_bounds; } // الحصول على المنطقة / Get bounds
            float GetX() const { return m_bounds.x; }      // الحصول على X / Get X
            float GetY() const { return m_bounds.y; }      // الحصول على Y / Get Y
            float GetWidth() const { return m_bounds.width; }   // الحصول على العرض / Get width
            float GetHeight() const { return m_bounds.height; } // الحصول على الارتفاع / Get height

            // الظهور والتفعيل / Visibility and enabled state
            void SetVisible(bool visible) { m_visible = visible; }  // ضبط الظهور / Set visibility
            bool IsVisible() const { return m_visible; }            // هل ظاهر؟ / Is visible?
            
            void SetEnabled(bool enabled);                          // ضبط التفعيل / Set enabled
            bool IsEnabled() const { return m_enabled; }            // هل مفعّل؟ / Is enabled?

            // الحالة / State
            WidgetState GetState() const { return m_state; }        // الحصول على الحالة / Get state
            void SetState(WidgetState state);                       // ضبط الحالة / Set state

            // النمط / Styling
            void SetStyle(const WidgetStyle& style) { m_style = style; }  // ضبط النمط / Set style
            const WidgetStyle& GetStyle() const { return m_style; }       // الحصول على النمط / Get style
            WidgetStyle& GetStyle() { return m_style; }                   // الحصول على النمط (قابل للتعديل) / Get style (mutable)

            // التركيز / Focus
            void SetFocused(bool focused);                          // ضبط التركيز / Set focus
            bool IsFocused() const { return m_focused; }            // هل له التركيز؟ / Is focused?

            // الرسم والتحديث / Drawing and updating
            virtual void Draw(SadGraphics::Renderer2D* renderer);   // رسم العنصر / Draw widget
            virtual void Update(float deltaTime);                   // تحديث العنصر / Update widget

            // معالجة الأحداث / Event handling
            virtual bool OnMouseMove(int x, int y);                 // حركة الفأرة / Mouse move
            virtual bool OnMouseDown(int x, int y, int button);     // ضغط زر الفأرة / Mouse button down
            virtual bool OnMouseUp(int x, int y, int button);       // رفع زر الفأرة / Mouse button up
            virtual bool OnKeyDown(int keyCode);                    // ضغط مفتاح / Key down
            virtual bool OnKeyUp(int keyCode);                      // رفع مفتاح / Key up
            virtual bool OnTextInput(const std::string& text);      // إدخال نص / Text input

            // التسلسل الهرمي / Hierarchy
            void AddChild(std::shared_ptr<Widget> child);           // إضافة عنصر فرعي / Add child
            void RemoveChild(std::shared_ptr<Widget> child);        // إزالة عنصر فرعي / Remove child
            void ClearChildren();                                   // مسح جميع العناصر الفرعية / Clear all children
            
            Widget* GetParent() const { return m_parent; }          // الحصول على الأب / Get parent
            const std::vector<std::shared_ptr<Widget>>& GetChildren() const { return m_children; } // الحصول على الأبناء / Get children

            // التحقق من احتواء نقطة / Check if point is inside
            virtual bool ContainsPoint(float x, float y) const;

            // الاسم والمعرف / Name and ID
            void SetName(const std::string& name) { m_name = name; }  // ضبط الاسم / Set name
            const std::string& GetName() const { return m_name; }     // الحصول على الاسم / Get name
            
            int GetID() const { return m_id; }                        // الحصول على المعرف / Get ID

        protected:
            // البيانات المحمية / Protected data
            int m_id;                                          // معرف فريد / Unique ID
            std::string m_name;                                // اسم العنصر / Widget name
            Rect m_bounds;                                     // موقع وحجم العنصر / Widget bounds
            
            bool m_visible;                                    // هل العنصر ظاهر؟ / Is widget visible?
            bool m_enabled;                                    // هل العنصر مفعّل؟ / Is widget enabled?
            bool m_focused;                                    // هل له التركيز؟ / Does widget have focus?
            
            WidgetState m_state;                               // حالة العنصر / Widget state
            WidgetStyle m_style;                               // نمط العنصر / Widget style
            
            Widget* m_parent;                                  // العنصر الأب / Parent widget
            std::vector<std::shared_ptr<Widget>> m_children;   // العناصر الفرعية / Child widgets

            // معرف ثابت للعناصر / Static ID counter
            static int s_nextID;                               // المعرف التالي / Next ID

            // دوال مساعدة / Helper functions
            virtual void OnStateChanged();                     // عند تغيير الحالة / When state changes
            virtual void OnStyleChanged();                     // عند تغيير النمط / When style changes
        };

    } // namespace UI
} // namespace Graphics

#endif // GRAPHICS_UI_WIDGET_H
