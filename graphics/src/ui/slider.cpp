// ============================================================================
// slider.cpp - تطبيق عنصر المنزلق
// Implementation of slider widget
// ============================================================================

#include "../include/ui/slider.h"              // ملف الرأس / Header file
#include "../include/rendering/renderer2d.h"   // للرسم / For rendering
#include <algorithm>                           // للخوارزميات / For algorithms
#include <cmath>                               // للدوال الرياضية / For math functions
#include <sstream>                             // لتنسيق النص / For string formatting
#include <iomanip>                             // لتنسيق الأرقام / For number formatting

namespace sad {
namespace graphics {
    namespace ui {

        // ============================================================================
        // البناء الافتراضي / Default constructor
        // ============================================================================
        Slider::Slider()
            : Widget()                                  // استدعاء بناء الأساس / Call base constructor
            , m_value(0.0f)                             // قيمة ابتدائية 0 / Initial value 0
            , m_minValue(0.0f)                          // الحد الأدنى 0 / Minimum 0
            , m_maxValue(100.0f)                        // الحد الأقصى 100 / Maximum 100
            , m_stepSize(0.0f)                          // بدون خطوات / No steps (continuous)
            , m_orientation(SliderOrientation::Horizontal)  // أفقي افتراضياً / Horizontal by default
            , m_thumbSize(20.0f)                        // حجم مقبض 20 بكسل / 20 pixel thumb
            , m_trackColor(Color::FromBytes(200, 200, 200))        // مسار رمادي فاتح / Light gray track
            , m_thumbColor(Color::FromBytes(0, 120, 215))          // مقبض أزرق / Blue thumb
            , m_fillColor(Color::FromBytes(0, 120, 215))           // تعبئة زرقاء / Blue fill
            , m_thumbHoverColor(Color::FromBytes(0, 160, 255))     // أزرق فاتح عند التحويم / Light blue on hover
            , m_showValue(true)                         // إظهار القيمة / Show value
            , m_valueFormat("%.1f")                     // تنسيق برقم عشري / Format with decimal
            , m_isDragging(false)                       // لا يتم السحب / Not dragging
            , m_isThumbHovered(false)                   // المقبض غير محوم / Thumb not hovered
        {
            // ضبط حجم افتراضي / Set default size
            SetSize(200, 30);

            // نمط افتراضي / Default style
            m_style.backgroundColor = Color::FromBytes(240, 240, 240);  // خلفية فاتحة / Light background
            m_style.borderColor = Color::FromBytes(180, 180, 180);      // حدود رمادية / Gray border
            m_style.borderWidth = 1.0f;                      // عرض حدود / Border width
            m_style.padding = 5.0f;                          // مسافة / Padding
        }

        // ============================================================================
        // البناء بموقع وحجم / Constructor with position and size
        // ============================================================================
        Slider::Slider(float x, float y, float width, float height)
            : Slider()  // استدعاء البناء الافتراضي / Call default constructor
        {
            SetPosition(x, y);      // ضبط الموقع / Set position
            SetSize(width, height);  // ضبط الحجم / Set size
        }

        // ============================================================================
        // البناء الكامل / Full constructor
        // ============================================================================
        Slider::Slider(float x, float y, float width, float height,
                       float minValue, float maxValue, float initialValue)
            : Slider(x, y, width, height)  // استدعاء البناء بموقع وحجم / Call position/size constructor
        {
            m_minValue = minValue;         // ضبط الحد الأدنى / Set minimum
            m_maxValue = maxValue;         // ضبط الحد الأقصى / Set maximum
            SetValue(initialValue);        // ضبط القيمة الأولية / Set initial value
        }

        // ============================================================================
        // التدمير / Destructor
        // ============================================================================
        Slider::~Slider() {
            // لا حاجة لتنظيف إضافي / No additional cleanup needed
        }

        // ============================================================================
        // ضبط القيمة / Set value
        // ============================================================================
        void Slider::SetValue(float value) {
            // تقييد القيمة ضمن النطاق / Clamp value to range
            float clampedValue = ClampValue(value);

            // محاذاة للخطوة إذا كان هناك حجم خطوة / Snap to step if step size is set
            if (m_stepSize > 0.0f) {
                clampedValue = SnapToStep(clampedValue);
            }

            // إذا تغيرت القيمة / If value changed
            if (clampedValue != m_value) {
                m_value = clampedValue;  // تحديث القيمة / Update value

                // استدعاء الدالة / Call callback
                if (m_onValueChange) {
                    m_onValueChange(m_value);
                }
            }
        }

        // ============================================================================
        // ضبط القيمة الدنيا / Set minimum value
        // ============================================================================
        void Slider::SetMinValue(float minValue) {
            m_minValue = minValue;  // ضبط الحد الأدنى / Set minimum
            
            // التأكد من أن القيمة الحالية ضمن النطاق / Ensure current value is in range
            if (m_value < m_minValue) {
                SetValue(m_minValue);
            }
        }

        // ============================================================================
        // ضبط القيمة القصوى / Set maximum value
        // ============================================================================
        void Slider::SetMaxValue(float maxValue) {
            m_maxValue = maxValue;  // ضبط الحد الأقصى / Set maximum
            
            // التأكد من أن القيمة الحالية ضمن النطاق / Ensure current value is in range
            if (m_value > m_maxValue) {
                SetValue(m_maxValue);
            }
        }

        // ============================================================================
        // ضبط النطاق / Set range
        // ============================================================================
        void Slider::SetRange(float minValue, float maxValue) {
            m_minValue = minValue;  // ضبط الحد الأدنى / Set minimum
            m_maxValue = maxValue;  // ضبط الحد الأقصى / Set maximum
            SetValue(m_value);      // إعادة ضبط القيمة / Re-clamp value
        }

        // ============================================================================
        // ضبط الاتجاه / Set orientation
        // ============================================================================
        void Slider::SetOrientation(SliderOrientation orientation) {
            m_orientation = orientation;  // تحديث الاتجاه / Update orientation
        }

        // ============================================================================
        // رسم المنزلق / Draw slider
        // ============================================================================
        void Slider::Draw(SadGraphics::Renderer2D* renderer) {
            // إذا لم يكن ظاهراً / If not visible
            if (!m_visible) {
                return;
            }

            // رسم الخلفية / Draw background
            Widget::Draw(renderer);

            // الحصول على مستطيلات المسار والمقبض / Get track and thumb rectangles
            Rect trackRect = GetTrackRect();
            Rect thumbRect = GetThumbRect();

            // رسم المسار (Track) / Draw track
            renderer->DrawFilledRect(trackRect.x, trackRect.y, 
                                    trackRect.width, trackRect.height, 
                                    m_trackColor);

            // رسم التعبئة (المسار المملوء حتى المقبض) / Draw fill (filled track up to thumb)
            if (m_orientation == SliderOrientation::Horizontal) {
                // تعبئة أفقية من اليسار / Horizontal fill from left
                float fillWidth = thumbRect.x + (m_thumbSize / 2.0f) - trackRect.x;
                renderer->DrawFilledRect(trackRect.x, trackRect.y, 
                                        fillWidth, trackRect.height, 
                                        m_fillColor);
            } else {
                // تعبئة عمودية من الأسفل / Vertical fill from bottom
                float fillHeight = (trackRect.y + trackRect.height) - (thumbRect.y + (m_thumbSize / 2.0f));
                float fillY = thumbRect.y + (m_thumbSize / 2.0f);
                renderer->DrawFilledRect(trackRect.x, fillY, 
                                        trackRect.width, fillHeight, 
                                        m_fillColor);
            }

            // رسم المقبض (Thumb) / Draw thumb
            Color thumbDrawColor = (m_isThumbHovered || m_isDragging) ? m_thumbHoverColor : m_thumbColor;
            renderer->DrawFilledCircle(thumbRect.GetCenter().x, thumbRect.GetCenter().y,
                                      m_thumbSize / 2.0f, thumbDrawColor);

            // رسم حدود المقبض / Draw thumb border
            renderer->DrawCircle(thumbRect.GetCenter().x, thumbRect.GetCenter().y,
                                m_thumbSize / 2.0f, Color::Black, false);

            // إظهار القيمة إذا كان مطلوباً / Show value if required
            if (m_showValue) {
                std::string valueText = FormatValue(m_value);  // تنسيق القيمة / Format value
                
                // حساب موقع النص / Calculate text position
                float textX, textY;
                if (m_orientation == SliderOrientation::Horizontal) {
                    // فوق المنزلق / Above slider
                    textX = thumbRect.GetCenter().x;
                    textY = m_bounds.y - 5.0f;
                } else {
                    // يمين المنزلق / Right of slider
                    textX = m_bounds.x + m_bounds.width + 10.0f;
                    textY = thumbRect.GetCenter().y;
                }

                // رسم النص / Draw text
                // سنحتاج لنظام رسم النص في المستقبل
                // Will need text rendering system in future
            }
        }

        // ============================================================================
        // تحديث المنزلق / Update slider
        // ============================================================================
        void Slider::Update(float deltaTime) {
            // استدعاء تحديث الأساس / Call base update
            Widget::Update(deltaTime);
        }

        // ============================================================================
        // حركة الفأرة / Mouse move
        // ============================================================================
        bool Slider::OnMouseMove(int x, int y) {
            // التحقق من التحويم على المقبض / Check thumb hover
            Rect thumbRect = GetThumbRect();
            m_isThumbHovered = thumbRect.Contains(static_cast<float>(x), static_cast<float>(y));

            // إذا كان يتم السحب / If dragging
            if (m_isDragging) {
                UpdateValueFromMouse(x, y);  // تحديث القيمة / Update value
                return true;
            }

            // استدعاء الأساس / Call base
            return Widget::OnMouseMove(x, y);
        }

        // ============================================================================
        // ضغط زر الفأرة / Mouse button down
        // ============================================================================
        bool Slider::OnMouseDown(int x, int y, int button) {
            // إذا كان الزر الأيسر / If left button
            if (button == 0) {  // Left mouse button
                Rect thumbRect = GetThumbRect();
                
                // إذا تم الضغط على المقبض / If clicked on thumb
                if (thumbRect.Contains(static_cast<float>(x), static_cast<float>(y))) {
                    m_isDragging = true;  // بدء السحب / Start dragging
                    SetState(WidgetState::Pressed);  // تغيير الحالة / Change state
                    return true;
                }
                
                // إذا تم الضغط على المسار / If clicked on track
                Rect trackRect = GetTrackRect();
                if (trackRect.Contains(static_cast<float>(x), static_cast<float>(y))) {
                    UpdateValueFromMouse(x, y);  // تحديث القيمة مباشرة / Update value directly
                    m_isDragging = true;  // بدء السحب / Start dragging
                    SetState(WidgetState::Pressed);  // تغيير الحالة / Change state
                    return true;
                }
            }

            // استدعاء الأساس / Call base
            return Widget::OnMouseDown(x, y, button);
        }

        // ============================================================================
        // رفع زر الفأرة / Mouse button up
        // ============================================================================
        bool Slider::OnMouseUp(int x, int y, int button) {
            // إذا كان الزر الأيسر وكان يتم السحب / If left button and was dragging
            if (button == 0 && m_isDragging) {
                m_isDragging = false;  // إيقاف السحب / Stop dragging
                SetState(WidgetState::Normal);  // إعادة الحالة / Reset state
                return true;
            }

            // استدعاء الأساس / Call base
            return Widget::OnMouseUp(x, y, button);
        }

        // ============================================================================
        // ضغط مفتاح / Key down
        // ============================================================================
        bool Slider::OnKeyDown(int keyCode) {
            // إذا كان عنصر مركز عليه / If focused
            if (!m_focused) {
                return false;
            }

            bool handled = false;
            float step = (m_stepSize > 0.0f) ? m_stepSize : (m_maxValue - m_minValue) / 10.0f;  // حجم الخطوة / Step size

            // معالجة المفاتيح حسب الاتجاه / Handle keys based on orientation
            if (m_orientation == SliderOrientation::Horizontal) {
                if (keyCode == 37) {  // Left Arrow - سهم اليسار
                    SetValue(m_value - step);  // تقليل القيمة / Decrease value
                    handled = true;
                } else if (keyCode == 39) {  // Right Arrow - سهم اليمين
                    SetValue(m_value + step);  // زيادة القيمة / Increase value
                    handled = true;
                }
            } else {  // Vertical
                if (keyCode == 38) {  // Up Arrow - سهم الأعلى
                    SetValue(m_value + step);  // زيادة القيمة / Increase value
                    handled = true;
                } else if (keyCode == 40) {  // Down Arrow - سهم الأسفل
                    SetValue(m_value - step);  // تقليل القيمة / Decrease value
                    handled = true;
                }
            }

            // Home و End / Home and End
            if (keyCode == 36) {  // Home
                SetValue(m_minValue);  // إلى الحد الأدنى / To minimum
                handled = true;
            } else if (keyCode == 35) {  // End
                SetValue(m_maxValue);  // إلى الحد الأقصى / To maximum
                handled = true;
            }

            // Page Up و Page Down / Page Up and Page Down
            if (keyCode == 33) {  // Page Up
                SetValue(m_value + (step * 5.0f));  // زيادة كبيرة / Large increase
                handled = true;
            } else if (keyCode == 34) {  // Page Down
                SetValue(m_value - (step * 5.0f));  // تقليل كبير / Large decrease
                handled = true;
            }

            if (handled) {
                return true;
            }

            // استدعاء الأساس / Call base
            return Widget::OnKeyDown(keyCode);
        }

        // ============================================================================
        // الحصول على موقع المقبض / Get thumb position
        // ============================================================================
        float Slider::GetThumbPosition() const {
            // حساب النسبة / Calculate ratio
            float ratio = (m_value - m_minValue) / (m_maxValue - m_minValue);

            Rect trackRect = GetTrackRect();
            
            if (m_orientation == SliderOrientation::Horizontal) {
                // موقع أفقي / Horizontal position
                return trackRect.x + (trackRect.width * ratio);
            } else {
                // موقع عمودي (من الأسفل للأعلى) / Vertical position (bottom to top)
                return trackRect.y + trackRect.height - (trackRect.height * ratio);
            }
        }

        // ============================================================================
        // الحصول على مستطيل المقبض / Get thumb rectangle
        // ============================================================================
        Rect Slider::GetThumbRect() const {
            float thumbPos = GetThumbPosition();  // موقع المقبض / Thumb position
            
            if (m_orientation == SliderOrientation::Horizontal) {
                // مستطيل أفقي / Horizontal rectangle
                return Rect(thumbPos - (m_thumbSize / 2.0f), 
                           m_bounds.y + (m_bounds.height / 2.0f) - (m_thumbSize / 2.0f),
                           m_thumbSize, m_thumbSize);
            } else {
                // مستطيل عمودي / Vertical rectangle
                return Rect(m_bounds.x + (m_bounds.width / 2.0f) - (m_thumbSize / 2.0f),
                           thumbPos - (m_thumbSize / 2.0f),
                           m_thumbSize, m_thumbSize);
            }
        }

        // ============================================================================
        // الحصول على مستطيل المسار / Get track rectangle
        // ============================================================================
        Rect Slider::GetTrackRect() const {
            float trackThickness = 4.0f;  // سمك المسار / Track thickness
            
            if (m_orientation == SliderOrientation::Horizontal) {
                // مسار أفقي / Horizontal track
                float trackY = m_bounds.y + (m_bounds.height / 2.0f) - (trackThickness / 2.0f);
                return Rect(m_bounds.x + (m_thumbSize / 2.0f), trackY,
                           m_bounds.width - m_thumbSize, trackThickness);
            } else {
                // مسار عمودي / Vertical track
                float trackX = m_bounds.x + (m_bounds.width / 2.0f) - (trackThickness / 2.0f);
                return Rect(trackX, m_bounds.y + (m_thumbSize / 2.0f),
                           trackThickness, m_bounds.height - m_thumbSize);
            }
        }

        // ============================================================================
        // القيمة من الموقع / Value from position
        // ============================================================================
        float Slider::ValueFromPosition(float position) const {
            Rect trackRect = GetTrackRect();
            float ratio;
            
            if (m_orientation == SliderOrientation::Horizontal) {
                // نسبة أفقية / Horizontal ratio
                ratio = (position - trackRect.x) / trackRect.width;
            } else {
                // نسبة عمودية (مقلوبة) / Vertical ratio (inverted)
                ratio = 1.0f - ((position - trackRect.y) / trackRect.height);
            }

            // تقييد النسبة / Clamp ratio
            ratio = std::max(0.0f, std::min(1.0f, ratio));

            // حساب القيمة / Calculate value
            return m_minValue + (ratio * (m_maxValue - m_minValue));
        }

        // ============================================================================
        // تحديث القيمة من الفأرة / Update value from mouse
        // ============================================================================
        void Slider::UpdateValueFromMouse(int x, int y) {
            float position = (m_orientation == SliderOrientation::Horizontal) ? 
                            static_cast<float>(x) : static_cast<float>(y);
            
            float newValue = ValueFromPosition(position);  // حساب القيمة الجديدة / Calculate new value
            SetValue(newValue);  // ضبط القيمة / Set value
        }

        // ============================================================================
        // تقييد القيمة / Clamp value
        // ============================================================================
        float Slider::ClampValue(float value) const {
            return std::max(m_minValue, std::min(m_maxValue, value));  // تقييد ضمن النطاق / Clamp to range
        }

        // ============================================================================
        // محاذاة للخطوة / Snap to step
        // ============================================================================
        float Slider::SnapToStep(float value) const {
            if (m_stepSize <= 0.0f) {  // إذا لم يكن هناك خطوة / If no step
                return value;
            }

            // حساب عدد الخطوات / Calculate number of steps
            float steps = std::round((value - m_minValue) / m_stepSize);
            
            // إرجاع القيمة الموافقة / Return snapped value
            return m_minValue + (steps * m_stepSize);
        }

        // ============================================================================
        // تنسيق القيمة / Format value
        // ============================================================================
        std::string Slider::FormatValue(float value) const {
            char buffer[64];  // حاوية مؤقتة / Temporary buffer
            std::snprintf(buffer, sizeof(buffer), m_valueFormat.c_str(), value);  // تنسيق / Format
            return std::string(buffer);  // إرجاع النص / Return string
        }

    } // namespace ui
} // namespace graphics
} // namespace sad
