/**
 * @file layout_engine.h
 * @brief محرك التخطيط Flexbox بالعربية
 * @brief (EN) Flexbox-style layout engine with Arabic support
 * 
 * @author كامل
 * @date أكتوبر 2026
 */

#ifndef SAD_LAYOUT_ENGINE_H
#define SAD_LAYOUT_ENGINE_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;memory&gt;
#include &lt;functional&gt;
#include &lt;cmath&gt;

namespace Sad {
namespace UI {
namespace Layout {

//==============================================================================
// أنواع التخطيط
//==============================================================================

/**
 * @brief اتجاه المحور الرئيسي
 */
enum class FlexDirection {
    ROW,            // صف (من اليمين لليسار للعربية)
    ROW_REVERSE,    // صف معكوس
    COLUMN,         // عمود
    COLUMN_REVERSE  // عمود معكوس
};

/**
 * @brief محاذاة المحور الرئيسي
 */
enum class JustifyContent {
    FLEX_START,     // بداية
    FLEX_END,       // نهاية
    CENTER,         // وسط
    SPACE_BETWEEN,  // مسافة بين
    SPACE_AROUND,   // مسافة حول
    SPACE_EVENLY    // مسافة متساوية
};

/**
 * @brief محاذاة المحور الثانوي
 */
enum class AlignItems {
    FLEX_START,     // بداية
    FLEX_END,       // نهاية
    CENTER,         // وسط
    STRETCH,        // تمدد
    BASELINE        // خط الأساس
};

/**
 * @brief محاذاة الذات
 */
enum class AlignSelf {
    AUTO,           // تلقائي
    FLEX_START,
    FLEX_END,
    CENTER,
    STRETCH,
    BASELINE
};

/**
 * @brief الالتفاف
 */
enum class FlexWrap {
    NOWRAP,         // بدون التفاف
    WRAP,           // التفاف
    WRAP_REVERSE    // التفاف معكوس
};

/**
 * @brief نوع الموضع
 */
enum class Position {
    RELATIVE,       // نسبي
    ABSOLUTE        // مطلق
};

/**
 * @brief وحدة القياس
 */
enum class Unit {
    PX,             // بكسل
    PERCENT,        // نسبة مئوية
    AUTO            // تلقائي
};

//==============================================================================
// قيم الأبعاد
//==============================================================================

/**
 * @brief قيمة مع وحدة
 */
struct DimensionValue {
    float value;
    Unit unit;
    
    DimensionValue() : value(0), unit(Unit::AUTO) {}
    DimensionValue(float v) : value(v), unit(Unit::PX) {}
    DimensionValue(float v, Unit u) : value(v), unit(u) {}
    
    bool isAuto() const { return unit == Unit::AUTO; }
    bool isPercent() const { return unit == Unit::PERCENT; }
    
    float resolve(float parentSize) const {
        switch (unit) {
            case Unit::PX: return value;
            case Unit::PERCENT: return parentSize * (value / 100.0f);
            case Unit::AUTO: return NAN;
        }
        return 0;
    }
    
    // مساعدات ثابتة
    static DimensionValue تلقائي() { return DimensionValue(0, Unit::AUTO); }
    static DimensionValue بكسل(float v) { return DimensionValue(v, Unit::PX); }
    static DimensionValue نسبة(float v) { return DimensionValue(v, Unit::PERCENT); }
};

/**
 * @brief الهوامش (TRBL)
 */
struct Edges {
    DimensionValue top;
    DimensionValue right;
    DimensionValue bottom;
    DimensionValue left;
    
    Edges() = default;
    Edges(DimensionValue all) : top(all), right(all), bottom(all), left(all) {}
    Edges(DimensionValue v, DimensionValue h) : top(v), right(h), bottom(v), left(h) {}
    Edges(DimensionValue t, DimensionValue r, DimensionValue b, DimensionValue l)
        : top(t), right(r), bottom(b), left(l) {}
};

//==============================================================================
// نتيجة التخطيط
//==============================================================================

/**
 * @brief مستطيل التخطيط المحسوب
 */
struct LayoutRect {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    
    float right() const { return x + width; }
    float bottom() const { return y + height; }
};

//==============================================================================
// خصائص التخطيط
//==============================================================================

/**
 * @brief خصائص تخطيط العنصر
 */
struct FlexStyle {
    // الاتجاه
    FlexDirection flexDirection = FlexDirection::ROW;
    FlexWrap flexWrap = FlexWrap::NOWRAP;
    
    // المحاذاة
    JustifyContent justifyContent = JustifyContent::FLEX_START;
    AlignItems alignItems = AlignItems::STRETCH;
    AlignSelf alignSelf = AlignSelf::AUTO;
    
    // Flex
    float flexGrow = 0;         // نسبة النمو
    float flexShrink = 1;       // نسبة الانكماش
    DimensionValue flexBasis;   // الحجم الأساسي
    
    // الأبعاد
    DimensionValue width;
    DimensionValue height;
    DimensionValue minWidth;
    DimensionValue minHeight;
    DimensionValue maxWidth;
    DimensionValue maxHeight;
    
    // الموضع
    Position position = Position::RELATIVE;
    DimensionValue top;
    DimensionValue right;
    DimensionValue bottom;
    DimensionValue left;
    
    // الهوامش
    Edges margin;
    Edges padding;
    
    // خصائص إضافية
    float aspectRatio = 0;      // نسبة العرض للارتفاع (0 = لا تطبق)
    bool display = true;        // مرئي؟
    
    // دعم RTL (العربية)
    bool isRTL = true;         // من اليمين لليسار
};

//==============================================================================
// عقدة التخطيط
//==============================================================================

class LayoutNode;
using LayoutNodePtr = std::shared_ptr&lt;LayoutNode&gt;;

/**
 * @brief عقدة في شجرة التخطيط
 */
class LayoutNode : public std::enable_shared_from_this&lt;LayoutNode&gt; {
public:
    LayoutNode() = default;
    ~LayoutNode() = default;
    
    // التسلسل الهرمي
    LayoutNodePtr parent;
    std::vector&lt;LayoutNodePtr&gt; children;
    
    // الخصائص
    FlexStyle style;
    
    // النتيجة المحسوبة
    LayoutRect layout;
    
    // معرّف للربط بالعناصر
    void* userData = nullptr;
    std::string nodeId;
    
    //==========================================================================
    // إدارة الأطفال
    //==========================================================================
    
    void addChild(LayoutNodePtr child) {
        child-&gt;parent = shared_from_this();
        children.push_back(child);
    }
    
    void removeChild(LayoutNodePtr child) {
        children.erase(
            std::remove(children.begin(), children.end(), child),
            children.end()
        );
        child-&gt;parent.reset();
    }
    
    void clearChildren() {
        for (auto&amp; child : children) {
            child-&gt;parent.reset();
        }
        children.clear();
    }
    
    //==========================================================================
    // مساعدات التخطيط
    //==========================================================================
    
    /**
     * @brief الحصول على العرض المتاح
     */
    float getAvailableWidth() const;
    
    /**
     * @brief الحصول على الارتفاع المتاح
     */
    float getAvailableHeight() const;
    
    /**
     * @brief هل العنصر مرن؟
     */
    bool isFlex() const {
        return style.flexGrow &gt; 0 || style.flexShrink &gt; 0;
    }
};

//==============================================================================
// محرك التخطيط
//==============================================================================

/**
 * @brief محرك تخطيط Flexbox
 */
class LayoutEngine {
public:
    LayoutEngine();
    ~LayoutEngine();
    
    /**
     * @brief حساب تخطيط الشجرة
     * 
     * @param root جذر الشجرة
     * @param availableWidth العرض المتاح
     * @param availableHeight الارتفاع المتاح
     */
    void calculate(LayoutNodePtr root, float availableWidth, float availableHeight);
    
    /**
     * @brief تعيين اتجاه النص
     */
    void setRTL(bool rtl) { isRTL_ = rtl; }
    
    /**
     * @brief الحصول على اتجاه النص
     */
    bool isRTL() const { return isRTL_; }
    
    /**
     * @brief مسح الكاش
     */
    void invalidate();

private:
    bool isRTL_ = true;
    
    void calculateNode(LayoutNodePtr node, float availableWidth, float availableHeight);
    void calculateFlexContainer(LayoutNodePtr node, float availableWidth, float availableHeight);
    void calculateFlexLine(std::vector&lt;LayoutNodePtr&gt;&amp; items, 
                           float containerWidth, float containerHeight,
                           bool isRow, float&amp; crossOffset);
    float resolveSize(DimensionValue size, float available, float fallback);
    float getMainSize(LayoutNodePtr node, bool isRow);
    float getCrossSize(LayoutNodePtr node, bool isRow);
    void setMainSize(LayoutNodePtr node, bool isRow, float size);
    void setCrossSize(LayoutNodePtr node, bool isRow, float size);
    void positionAbsoluteChildren(LayoutNodePtr node);
};

//==============================================================================
// منشئ التخطيط (DSL)
//==============================================================================

/**
 * @brief منشئ تخطيط بأسلوب fluent
 */
class LayoutBuilder {
public:
    LayoutBuilder();
    
    // بدء عقدة جديدة
    LayoutBuilder&amp; عقدة(const std::string&amp; id = "");
    
    // Flex
    LayoutBuilder&amp; اتجاه(FlexDirection dir);
    LayoutBuilder&amp; صف() { return اتجاه(FlexDirection::ROW); }
    LayoutBuilder&amp; عمود() { return اتجاه(FlexDirection::COLUMN); }
    LayoutBuilder&amp; التفاف(FlexWrap wrap);
    LayoutBuilder&amp; نمو(float grow);
    LayoutBuilder&amp; انكماش(float shrink);
    
    // المحاذاة
    LayoutBuilder&amp; توزيع(JustifyContent justify);
    LayoutBuilder&amp; محاذاة(AlignItems align);
    
    // الأبعاد
    LayoutBuilder&amp; عرض(DimensionValue w);
    LayoutBuilder&amp; ارتفاع(DimensionValue h);
    LayoutBuilder&amp; عرض_أدنى(DimensionValue w);
    LayoutBuilder&amp; ارتفاع_أدنى(DimensionValue h);
    LayoutBuilder&amp; عرض_أقصى(DimensionValue w);
    LayoutBuilder&amp; ارتفاع_أقصى(DimensionValue h);
    
    // الهوامش
    LayoutBuilder&amp; هامش(Edges m);
    LayoutBuilder&amp; حشو(Edges p);
    
    // الموضع
    LayoutBuilder&amp; موضع(Position pos);
    LayoutBuilder&amp; مطلق() { return موضع(Position::ABSOLUTE); }
    LayoutBuilder&amp; نسبي() { return موضع(Position::RELATIVE); }
    
    // الأطفال
    LayoutBuilder&amp; طفل(std::function&lt;void(LayoutBuilder&amp;)&gt; builder);
    LayoutBuilder&amp; نهاية();
    
    // البناء
    LayoutNodePtr بناء();

private:
    std::vector&lt;LayoutNodePtr&gt; stack_;
    LayoutNodePtr root_;
};

//==============================================================================
// أدوات مساعدة
//==============================================================================

namespace Utils {

/**
 * @brief طباعة شجرة التخطيط (للتصحيح)
 */
void printLayoutTree(LayoutNodePtr node, int indent = 0);

/**
 * @brief العثور على عقدة بالمعرّف
 */
LayoutNodePtr findById(LayoutNodePtr root, const std::string&amp; id);

/**
 * @brief نسخ شجرة التخطيط
 */
LayoutNodePtr cloneTree(LayoutNodePtr root);

} // namespace Utils

} // namespace Layout
} // namespace UI
} // namespace Sad

#endif // SAD_LAYOUT_ENGINE_H
