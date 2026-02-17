// ============================================================================
// panel.h - عنصر اللوحة الحاوية (Panel Widget)
// Panel container widget for grouping other widgets
// ============================================================================
// الوصف: لوحة حاوية لتجميع العناصر الأخرى مع خيارات تخطيط
// Description: Container panel for grouping widgets with layout options
// ============================================================================

#ifndef GRAPHICS_UI_PANEL_H
#define GRAPHICS_UI_PANEL_H

#include "widget.h"  // الفئة الأساسية / Base class

namespace Graphics {
    namespace UI {

        // ============================================================================
        // PanelLayout - نوع التخطيط
        // Panel layout type
        // ============================================================================
        enum class PanelLayout {
            None,           // بدون تخطيط تلقائي / No automatic layout
            Vertical,       // تخطيط عمودي / Vertical layout
            Horizontal,     // تخطيط أفقي / Horizontal layout
            Grid            // تخطيط شبكي / Grid layout
        };

        // ============================================================================
        // Panel - عنصر اللوحة
        // Panel widget class
        // ============================================================================
        class Panel : public Widget {
        public:
            // البناء والتدمير / Construction and destruction
            Panel();                                            // البناء الافتراضي / Default constructor
            Panel(float x, float y, float width, float height); // البناء بموقع وحجم / Constructor with position and size
            virtual ~Panel();                                   // المدمر / Destructor

            // إدارة التخطيط / Layout management
            void SetLayout(PanelLayout layout);                 // ضبط نوع التخطيط / Set layout type
            PanelLayout GetLayout() const { return m_layout; }  // الحصول على نوع التخطيط / Get layout type

            // المسافة بين العناصر / Spacing between widgets
            void SetSpacing(float spacing) { m_spacing = spacing; }  // ضبط المسافة / Set spacing
            float GetSpacing() const { return m_spacing; }            // الحصول على المسافة / Get spacing

            // تخطيط شبكي / Grid layout
            void SetGridColumns(int columns) { m_gridColumns = columns; }  // ضبط عدد الأعمدة / Set column count
            int GetGridColumns() const { return m_gridColumns; }            // الحصول على عدد الأعمدة / Get column count

            // المسافة الداخلية للوحة / Panel padding
            void SetContentPadding(float padding) { m_contentPadding = padding; }  // ضبط المسافة الداخلية / Set content padding
            float GetContentPadding() const { return m_contentPadding; }            // الحصول على المسافة الداخلية / Get content padding

            // تطبيق التخطيط / Apply layout
            void ApplyLayout();                                 // تطبيق التخطيط على العناصر / Apply layout to children

            // إضافة عنصر مع تطبيق التخطيط / Add child with automatic layout
            void AddChildWithLayout(std::shared_ptr<Widget> child);  // إضافة عنصر مع تخطيط تلقائي / Add child with auto layout

            // التمرير / Scrolling
            void SetScrollable(bool scrollable) { m_scrollable = scrollable; }  // ضبط إمكانية التمرير / Set scrollable
            bool IsScrollable() const { return m_scrollable; }                  // هل قابل للتمرير؟ / Is scrollable?

            void SetScrollOffset(float x, float y);             // ضبط إزاحة التمرير / Set scroll offset
            void GetScrollOffset(float& x, float& y) const;     // الحصول على إزاحة التمرير / Get scroll offset

            // الرسم والتحديث / Drawing and updating
            virtual void Draw(SadGraphics::Renderer2D* renderer) override;   // رسم اللوحة / Draw panel
            virtual void Update(float deltaTime) override;      // تحديث اللوحة / Update panel

            // معالجة الأحداث / Event handling
            virtual bool OnMouseMove(int x, int y) override;    // حركة الفأرة / Mouse move
            virtual bool OnMouseDown(int x, int y, int button) override;   // ضغط زر الفأرة / Mouse button down
            virtual bool OnMouseUp(int x, int y, int button) override;     // رفع زر الفأرة / Mouse button up

        protected:
            PanelLayout m_layout;            // نوع التخطيط / Layout type
            float m_spacing;                 // المسافة بين العناصر / Spacing between widgets
            int m_gridColumns;               // عدد الأعمدة للشبكة / Grid column count
            float m_contentPadding;          // المسافة الداخلية / Content padding

            bool m_scrollable;               // هل قابل للتمرير؟ / Is scrollable?
            float m_scrollX;                 // إزاحة التمرير الأفقي / Horizontal scroll offset
            float m_scrollY;                 // إزاحة التمرير العمودي / Vertical scroll offset

            // دوال مساعدة / Helper functions
            void ApplyVerticalLayout();      // تطبيق التخطيط العمودي / Apply vertical layout
            void ApplyHorizontalLayout();    // تطبيق التخطيط الأفقي / Apply horizontal layout
            void ApplyGridLayout();          // تطبيق التخطيط الشبكي / Apply grid layout
        };

    } // namespace UI
} // namespace Graphics

#endif // GRAPHICS_UI_PANEL_H
