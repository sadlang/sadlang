// ============================================================================
// panel.cpp - تطبيق عنصر اللوحة الحاوية
// Implementation of panel container widget
// ============================================================================

#include "../include/ui/panel.h"               // ملف الرأس / Header file
#include "../include/rendering/renderer2d.h"   // للرسم / For rendering
#include <algorithm>                           // للخوارزميات / For algorithms

namespace Graphics {
    namespace UI {

        // ============================================================================
        // البناء الافتراضي / Default constructor
        // ============================================================================
        Panel::Panel()
            : Widget()                          // استدعاء بناء الأساس / Call base constructor
            , m_layout(PanelLayout::None)       // بدون تخطيط افتراضي / No default layout
            , m_spacing(5.0f)                   // مسافة 5 بكسل / 5 pixel spacing
            , m_gridColumns(2)                  // عمودين افتراضياً / 2 columns by default
            , m_contentPadding(10.0f)           // مسافة داخلية 10 / 10 pixel padding
            , m_scrollable(false)               // غير قابل للتمرير / Not scrollable
            , m_scrollX(0.0f)                   // لا يوجد تمرير / No scroll
            , m_scrollY(0.0f)                   // لا يوجد تمرير / No scroll
        {
            // ضبط حجم افتراضي / Set default size
            SetSize(300, 200);

            // نمط افتراضي / Default style
            m_style.backgroundColor = Color(245, 245, 245);  // خلفية فاتحة / Light background
            m_style.borderColor = Color(200, 200, 200);      // حدود رمادية / Gray border
            m_style.borderWidth = 1.0f;                      // عرض حدود / Border width
            m_style.padding = 0.0f;                          // بدون مسافة خارجية / No outer padding
        }

        // ============================================================================
        // البناء بموقع وحجم / Constructor with position and size
        // ============================================================================
        Panel::Panel(float x, float y, float width, float height)
            : Panel()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetPosition(x, y);     // ضبط الموقع / Set position
            SetSize(width, height);  // ضبط الحجم / Set size
        }

        // ============================================================================
        // التدمير / Destructor
        // ============================================================================
        Panel::~Panel() {
            // لا حاجة لتنظيف إضافي / No additional cleanup needed
        }

        // ============================================================================
        // ضبط نوع التخطيط / Set layout type
        // ============================================================================
        void Panel::SetLayout(PanelLayout layout) {
            m_layout = layout;  // تحديث نوع التخطيط / Update layout type
            
            // تطبيق التخطيط إذا كان هناك عناصر / Apply layout if children exist
            if (!m_children.empty()) {
                ApplyLayout();
            }
        }

        // ============================================================================
        // ضبط إزاحة التمرير / Set scroll offset
        // ============================================================================
        void Panel::SetScrollOffset(float x, float y) {
            if (m_scrollable) {  // إذا كان قابل للتمرير / If scrollable
                m_scrollX = x;   // ضبط الإزاحة الأفقية / Set horizontal offset
                m_scrollY = y;   // ضبط الإزاحة العمودية / Set vertical offset
            }
        }

        // ============================================================================
        // الحصول على إزاحة التمرير / Get scroll offset
        // ============================================================================
        void Panel::GetScrollOffset(float& x, float& y) const {
            x = m_scrollX;  // إرجاع الإزاحة الأفقية / Return horizontal offset
            y = m_scrollY;  // إرجاع الإزاحة العمودية / Return vertical offset
        }

        // ============================================================================
        // تطبيق التخطيط / Apply layout
        // ============================================================================
        void Panel::ApplyLayout() {
            // اختيار نوع التخطيط / Choose layout type
            switch (m_layout) {
                case PanelLayout::Vertical:
                    ApplyVerticalLayout();    // تخطيط عمودي / Vertical layout
                    break;

                case PanelLayout::Horizontal:
                    ApplyHorizontalLayout();  // تخطيط أفقي / Horizontal layout
                    break;

                case PanelLayout::Grid:
                    ApplyGridLayout();        // تخطيط شبكي / Grid layout
                    break;

                case PanelLayout::None:
                default:
                    // لا نفعل شيء / Do nothing
                    break;
            }
        }

        // ============================================================================
        // إضافة عنصر مع تخطيط تلقائي / Add child with automatic layout
        // ============================================================================
        void Panel::AddChildWithLayout(std::shared_ptr<Widget> child) {
            AddChild(child);   // إضافة العنصر / Add child
            ApplyLayout();     // تطبيق التخطيط / Apply layout
        }

        // ============================================================================
        // رسم اللوحة / Draw panel
        // ============================================================================
        void Panel::Draw(Renderer2D* renderer) {
            // إذا لم يكن ظاهراً / If not visible
            if (!m_visible) {
                return;
            }

            // رسم الخلفية والحدود / Draw background and borders
            Widget::Draw(renderer);

            // حفظ حالة التحويل / Save transform state (للتمرير في المستقبل)
            // Save transform state (for future scrolling)

            // رسم العناصر الفرعية / Draw children
            for (auto& child : m_children) {
                if (child && child->IsVisible()) {
                    // تطبيق إزاحة التمرير / Apply scroll offset
                    if (m_scrollable) {
                        // سنحتاج لنظام clipping في المستقبل
                        // Will need clipping system in future
                    }
                    
                    child->Draw(renderer);  // رسم العنصر الفرعي / Draw child
                }
            }

            // استعادة حالة التحويل / Restore transform state
        }

        // ============================================================================
        // تحديث اللوحة / Update panel
        // ============================================================================
        void Panel::Update(float deltaTime) {
            // استدعاء تحديث الأساس / Call base update
            Widget::Update(deltaTime);
        }

        // ============================================================================
        // حركة الفأرة / Mouse move
        // ============================================================================
        bool Panel::OnMouseMove(int x, int y) {
            // استدعاء الأساس / Call base
            return Widget::OnMouseMove(x, y);
        }

        // ============================================================================
        // ضغط زر الفأرة / Mouse button down
        // ============================================================================
        bool Panel::OnMouseDown(int x, int y, int button) {
            // استدعاء الأساس / Call base
            return Widget::OnMouseDown(x, y, button);
        }

        // ============================================================================
        // رفع زر الفأرة / Mouse button up
        // ============================================================================
        bool Panel::OnMouseUp(int x, int y, int button) {
            // استدعاء الأساس / Call base
            return Widget::OnMouseUp(x, y, button);
        }

        // ============================================================================
        // تطبيق التخطيط العمودي / Apply vertical layout
        // ============================================================================
        void Panel::ApplyVerticalLayout() {
            if (m_children.empty()) {  // إذا لم يكن هناك عناصر / If no children
                return;
            }

            // حساب الموقع الأول / Calculate first position
            float currentY = m_bounds.y + m_contentPadding;
            float contentX = m_bounds.x + m_contentPadding;
            float contentWidth = m_bounds.width - (m_contentPadding * 2.0f);

            // ترتيب العناصر عمودياً / Arrange children vertically
            for (auto& child : m_children) {
                if (!child || !child->IsVisible()) {  // تخطي العناصر غير الظاهرة / Skip invisible children
                    continue;
                }

                // ضبط موقع العنصر / Set child position
                child->SetPosition(contentX, currentY);
                
                // ضبط عرض العنصر ليطابق عرض اللوحة / Set child width to match panel
                child->SetSize(contentWidth, child->GetHeight());

                // الانتقال للموقع التالي / Move to next position
                currentY += child->GetHeight() + m_spacing;
            }
        }

        // ============================================================================
        // تطبيق التخطيط الأفقي / Apply horizontal layout
        // ============================================================================
        void Panel::ApplyHorizontalLayout() {
            if (m_children.empty()) {  // إذا لم يكن هناك عناصر / If no children
                return;
            }

            // حساب الموقع الأول / Calculate first position
            float currentX = m_bounds.x + m_contentPadding;
            float contentY = m_bounds.y + m_contentPadding;
            float contentHeight = m_bounds.height - (m_contentPadding * 2.0f);

            // ترتيب العناصر أفقياً / Arrange children horizontally
            for (auto& child : m_children) {
                if (!child || !child->IsVisible()) {  // تخطي العناصر غير الظاهرة / Skip invisible children
                    continue;
                }

                // ضبط موقع العنصر / Set child position
                child->SetPosition(currentX, contentY);
                
                // ضبط ارتفاع العنصر ليطابق ارتفاع اللوحة / Set child height to match panel
                child->SetSize(child->GetWidth(), contentHeight);

                // الانتقال للموقع التالي / Move to next position
                currentX += child->GetWidth() + m_spacing;
            }
        }

        // ============================================================================
        // تطبيق التخطيط الشبكي / Apply grid layout
        // ============================================================================
        void Panel::ApplyGridLayout() {
            if (m_children.empty() || m_gridColumns <= 0) {  // إذا لم يكن هناك عناصر أو أعمدة / If no children or columns
                return;
            }

            // حساب المساحة المتاحة / Calculate available space
            float contentWidth = m_bounds.width - (m_contentPadding * 2.0f);
            float contentHeight = m_bounds.height - (m_contentPadding * 2.0f);

            // حساب حجم الخلية / Calculate cell size
            float cellWidth = (contentWidth - (m_spacing * (m_gridColumns - 1))) / m_gridColumns;
            
            // حساب عدد الصفوف / Calculate row count
            int visibleCount = 0;
            for (auto& child : m_children) {
                if (child && child->IsVisible()) {
                    visibleCount++;
                }
            }
            int rowCount = (visibleCount + m_gridColumns - 1) / m_gridColumns;  // تقريب لأعلى / Round up
            float cellHeight = (contentHeight - (m_spacing * (rowCount - 1))) / rowCount;

            // ترتيب العناصر في شبكة / Arrange children in grid
            int currentIndex = 0;
            for (auto& child : m_children) {
                if (!child || !child->IsVisible()) {  // تخطي العناصر غير الظاهرة / Skip invisible children
                    continue;
                }

                // حساب الصف والعمود / Calculate row and column
                int row = currentIndex / m_gridColumns;
                int col = currentIndex % m_gridColumns;

                // حساب الموقع / Calculate position
                float x = m_bounds.x + m_contentPadding + (col * (cellWidth + m_spacing));
                float y = m_bounds.y + m_contentPadding + (row * (cellHeight + m_spacing));

                // ضبط موقع وحجم العنصر / Set child position and size
                child->SetPosition(x, y);
                child->SetSize(cellWidth, cellHeight);

                currentIndex++;  // الانتقال للعنصر التالي / Move to next child
            }
        }

    } // namespace UI
} // namespace Graphics
