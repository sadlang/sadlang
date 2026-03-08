// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_core.h — النواة الأساسية لنظام Flutter في لغة ص
//  SadFlutter Core — Widget, Key, BuildContext, State
// ═══════════════════════════════════════════════════════════════════════════════
//
//  هذا الملف يحتوي على البنية الأساسية المستوحاة من Flutter:
//    - Widget (الأساس المجرد)
//    - Key (للتعريف الفريد)
//    - BuildContext (سياق البناء)
//    - State<T> (حالة المكون المتغير)
//    - StatelessWidget (مكون بدون حالة)
//    - StatefulWidget (مكون بحالة)
//
//  المؤلف: فريق لغة ص
//  التاريخ: 2026
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <typeinfo>
#include <any>

namespace sad {
namespace flutter {

// إعلانات مسبقة
class Widget;
class Element;
class BuildContext;
class RenderObject;
class State_Base;
template<typename T> class State;

// ═══════════════════════════════════════════════════════════════════════════════
//  Key — المفتاح للتعريف الفريد للمكونات
// ═══════════════════════════════════════════════════════════════════════════════

/// أنواع المفاتيح
enum class KeyType {
    None,           // بدون مفتاح
    Value,          // مفتاح بقيمة (ValueKey)
    Unique,         // مفتاح فريد (UniqueKey)
    Global,         // مفتاح يشير لعنصر معين في أي مكان (GlobalKey)
    Object          // مفتاح بكائن (ObjectKey)
};

/// المفتاح — يحدد هوية Widget عبر إعادة البناء
class Key {
public:
    Key() : type_(KeyType::None), value_("") {}
    
    /// مفتاح بقيمة نصية
    static Key value(const std::string& val) {
        Key k;
        k.type_ = KeyType::Value;
        k.value_ = val;
        return k;
    }
    
    /// مفتاح بقيمة رقمية
    static Key value(int val) {
        return value(std::to_string(val));
    }
    
    /// مفتاح فريد عشوائي
    static Key unique();
    
    /// مفتاح عالمي (يمكن الوصول للعنصر والحالة من أي مكان)
    static Key global(const std::string& id);
    
    bool operator==(const Key& other) const {
        return type_ == other.type_ && value_ == other.value_;
    }
    bool operator!=(const Key& other) const { return !(*this == other); }
    
    KeyType type() const { return type_; }
    const std::string& value() const { return value_; }
    bool isNone() const { return type_ == KeyType::None; }
    
    /// تحويل لنص للتصحيح
    std::string toString() const;
    
private:
    KeyType type_;
    std::string value_;
};

// GlobalKey registry
Key globalKey(const std::string& id);
Element* findElementByGlobalKey(const Key& key);
State_Base* findStateByGlobalKey(const Key& key);

// ═══════════════════════════════════════════════════════════════════════════════
//  Widget — الأساس المجرد لكل المكونات
// ═══════════════════════════════════════════════════════════════════════════════

/// Widget — المكون الأساسي (غير قابل للتغيير — immutable)
/// كل Widget يصف شكل جزء من الواجهة في لحظة معينة.
/// لتغيير الواجهة، ننشئ Widget جديد بدلاً من تعديل القديم.
class Widget {
public:
    Widget(Key key = Key()) : key_(std::move(key)) {}
    virtual ~Widget() = default;
    
    /// المفتاح الفريد (اختياري)
    const Key& key() const { return key_; }
    
    /// إنشاء العنصر (Element) المقابل لهذا المكون
    /// يُستدعى مرة واحدة عند إدراج Widget في الشجرة
    virtual std::unique_ptr<Element> createElement() = 0;
    
    /// اسم نوع المكون (للتصحيح)
    virtual std::string typeName() const { return "Widget"; }
    
    /// هل يمكن تحديث عنصر قائم بهذا Widget؟
    /// يُرجع true إذا كان نفس النوع ونفس المفتاح (أو كلاهما بدون مفتاح)
    virtual bool canUpdate(const Widget* oldWidget) const {
        if (!oldWidget) return false;
        if (typeid(*this) != typeid(*oldWidget)) return false;
        if (key_.isNone() && oldWidget->key_.isNone()) return true;
        return key_ == oldWidget->key_;
    }
    
protected:
    Key key_;
};

// مؤشر ذكي للمكونات
using WidgetPtr = std::shared_ptr<Widget>;
using WidgetList = std::vector<WidgetPtr>;

// ═══════════════════════════════════════════════════════════════════════════════
//  BuildContext — سياق البناء
// ═══════════════════════════════════════════════════════════════════════════════

/// BuildContext — واجهة للوصول إلى سياق الشجرة أثناء البناء
/// تُمرر إلى build() وتسمح بالوصول إلى:
///   - Theme: سمات التصميم
///   - Navigator: التنقل بين الصفحات
///   - الأسلاف في الشجرة
///   - InheritedWidgets (بيانات موروثة)
class BuildContext {
public:
    BuildContext(Element* element) : element_(element) {}
    
    /// الحصول على العنصر المالك
    Element* element() const { return element_; }
    
    /// البحث عن أقرب سلف من نوع محدد
    template<typename T>
    T* findAncestorWidgetOfExactType();
    
    /// البحث عن حالة أقرب سلف StatefulWidget من نوع محدد
    template<typename T>
    State<T>* findAncestorStateOfType();
    
    /// الحصول على بيانات موروثة (InheritedWidget)
    template<typename T>
    T* dependOnInheritedWidgetOfExactType();
    
    /// حجم الشاشة
    struct Size { float width, height; };
    Size mediaQuerySize() const;
    
    /// هل الجهاز بالوضع العمودي؟
    bool isPortrait() const;
    
    /// اتجاه النص (RTL للعربية)
    bool isRTL() const;
    
private:
    Element* element_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Element — العقدة الحية في شجرة الواجهة
// ═══════════════════════════════════════════════════════════════════════════════

/// Element — تمثيل حي للمكون في موقعه بالشجرة
/// بينما Widget وصفي (immutable)، الـ Element هو التمثيل المتغير
/// الذي يحافظ على الحالة ويدير دورة الحياة.
class Element {
public:
    Element(Widget* widget);
    virtual ~Element();
    
    /// المكون الذي يصف هذا العنصر
    Widget* widget() const { return widget_.get(); }
    
    /// العنصر الأب
    Element* parent() const { return parent_; }
    
    /// الأبناء
    const std::vector<std::unique_ptr<Element>>& children() const { return children_; }
    
    /// سياق البناء
    BuildContext* context() { return &context_; }
    
    /// هل العنصر مركب في الشجرة؟
    bool mounted() const { return mounted_; }
    
    /// عمق العنصر في الشجرة
    int depth() const { return depth_; }
    
    // ─── دورة الحياة ───
    
    /// تركيب العنصر في الشجرة تحت أب معين
    virtual void mount(Element* parent);
    
    /// فك تركيب العنصر من الشجرة
    virtual void unmount();
    
    /// تحديث العنصر بمكون جديد (نفس النوع)
    virtual void update(std::shared_ptr<Widget> newWidget);
    
    /// إعادة بناء العنصر (استدعاء build)
    virtual void rebuild();
    
    /// وضع علامة "يحتاج إعادة بناء"
    void markNeedsBuild();
    
    /// هل العنصر يحتاج إعادة بناء؟
    bool dirty() const { return dirty_; }
    
    // ─── RenderObject ───
    
    /// كائن الرسم المقابل (إن وجد)
    RenderObject* renderObject() const { return renderObject_.get(); }
    
protected:
    /// بناء الأبناء — يُنفذ من قبل الأصناف الفرعية
    virtual void performRebuild() = 0;
    
    /// إضافة ابن
    void attachChild(std::unique_ptr<Element> child);
    
    /// تحديث أو إنشاء ابن بناءً على Widget جديد
    std::unique_ptr<Element> updateChild(
        std::unique_ptr<Element> child,
        std::shared_ptr<Widget> newWidget
    );
    
    std::shared_ptr<Widget> widget_;
    Element* parent_ = nullptr;
    std::vector<std::unique_ptr<Element>> children_;
    BuildContext context_;
    std::unique_ptr<RenderObject> renderObject_;
    int depth_ = 0;
    bool mounted_ = false;
    bool dirty_ = true;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  StatelessWidget — مكون بدون حالة
// ═══════════════════════════════════════════════════════════════════════════════

/// StatelessWidget — مكون ثابت لا يتغير بعد إنشائه
/// يُعرّف شكل الواجهة بناءً على configuration فقط.
class StatelessWidget : public Widget {
public:
    using Widget::Widget;
    
    /// بناء شجرة المكونات الفرعية
    /// يجب أن يُرجع مكوناً واحداً (قد يحتوي أبناء)
    virtual WidgetPtr build(BuildContext* context) = 0;
    
    std::unique_ptr<Element> createElement() override;
    std::string typeName() const override { return "StatelessWidget"; }
};

/// StatelessElement — العنصر المقابل لـ StatelessWidget
class StatelessElement : public Element {
public:
    StatelessElement(StatelessWidget* widget);
    void performRebuild() override;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  State<T> — حالة المكون المتغير
// ═══════════════════════════════════════════════════════════════════════════════

/// State_Base — الأساس غير العام للحالة
class State_Base {
public:
    virtual ~State_Base() = default;
    
    /// سياق البناء
    BuildContext* context() const { return context_; }
    
    /// هل الحالة مركبة؟
    bool mounted() const { return mounted_; }
    
    // ─── دورة الحياة ─── (يمكن للفئات الفرعية override)
    
    /// يُستدعى بعد إنشاء الحالة مباشرة (قبل أول build)
    virtual void initState() {}
    
    /// يُستدعى عند تغير dependencies (InheritedWidget)
    virtual void didChangeDependencies() {}
    
    /// يُستدعى عند تحديث المكون الأب بمكون جديد
    virtual void didUpdateWidget(Widget* oldWidget) {}
    
    /// يُستدعى قبل إزالة العنصر من الشجرة
    virtual void deactivate() {}
    
    /// يُستدعى عند إعادة تفعيل العنصر
    virtual void activate() {}
    
    /// يُستدعى عند حذف العنصر نهائياً (تنظيف الموارد)
    virtual void dispose() {}
    
protected:
    friend class StatefulElement;
    
    /// طلب إعادة بناء المكون — يُستدعى من setState()
    void markNeedsBuild();
    
    BuildContext* context_ = nullptr;
    Element* element_ = nullptr;
    bool mounted_ = false;
};

/// State<T> — حالة مكون StatefulWidget من نوع T
template<typename T>
class State : public State_Base {
public:
    /// الحصول على المكون المالك
    T* widget() const { return static_cast<T*>(widget_); }
    
    /// بناء شجرة المكونات الفرعية
    /// يُستدعى عند كل إعادة بناء
    virtual WidgetPtr build(BuildContext* context) = 0;
    
    /// تحديث الحالة وطلب إعادة البناء
    /// استخدم: setState([&]{ counter++; });
    void setState(std::function<void()> fn) {
        fn();
        if (mounted_) {
            markNeedsBuild();
        }
    }
    
protected:
    friend class StatefulElement;
    Widget* widget_ = nullptr;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  StatefulWidget — مكون بحالة متغيرة
// ═══════════════════════════════════════════════════════════════════════════════

/// StatefulWidget — مكون له حالة داخلية قابلة للتغيير
/// عند تغير الحالة (عبر setState)، يُعاد بناء المكون.
class StatefulWidget : public Widget {
public:
    using Widget::Widget;
    
    /// إنشاء كائن الحالة — يُستدعى مرة واحدة عند إدراج المكون
    virtual std::unique_ptr<State_Base> createState() = 0;
    
    std::unique_ptr<Element> createElement() override;
    std::string typeName() const override { return "StatefulWidget"; }
};

/// StatefulElement — العنصر المقابل لـ StatefulWidget
class StatefulElement : public Element {
public:
    StatefulElement(StatefulWidget* widget);
    ~StatefulElement();
    
    /// الحالة المرتبطة
    State_Base* state() const { return state_.get(); }
    
    void mount(Element* parent) override;
    void unmount() override;
    void update(std::shared_ptr<Widget> newWidget) override;
    void performRebuild() override;
    
private:
    std::unique_ptr<State_Base> state_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  RenderObject — كائن الرسم
// ═══════════════════════════════════════════════════════════════════════════════

/// RenderObject — يمثل العنصر المرئي الفعلي (موقع، حجم، رسم)
/// يفصل بين وصف الواجهة (Widget) والرسم الفعلي (RenderObject)
class RenderObject {
public:
    virtual ~RenderObject() = default;
    
    /// حساب التخطيط (layout)
    virtual void layout(float maxWidth, float maxHeight) = 0;
    
    /// الرسم على الشاشة
    virtual void paint() = 0;
    
    /// اختبار اللمس
    virtual bool hitTest(float x, float y) { return false; }
    
    // ─── الموقع والحجم ───
    float x() const { return x_; }
    float y() const { return y_; }
    float width() const { return width_; }
    float height() const { return height_; }
    
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    void setSize(float w, float h) { width_ = w; height_ = h; }
    
protected:
    float x_ = 0, y_ = 0;
    float width_ = 0, height_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  RenderObjectWidget — مكون ينشئ RenderObject
// ═══════════════════════════════════════════════════════════════════════════════

/// RenderObjectWidget — مكون له رسم فعلي (على عكس Stateless/Stateful)
class RenderObjectWidget : public Widget {
public:
    using Widget::Widget;
    
    /// إنشاء RenderObject المقابل
    virtual std::unique_ptr<RenderObject> createRenderObject(BuildContext* context) = 0;
    
    /// تحديث RenderObject موجود بخصائص جديدة
    virtual void updateRenderObject(BuildContext* context, RenderObject* renderObject) {}
    
    std::unique_ptr<Element> createElement() override;
    std::string typeName() const override { return "RenderObjectWidget"; }
};

/// RenderObjectElement — العنصر المقابل لـ RenderObjectWidget
class RenderObjectElement : public Element {
public:
    RenderObjectElement(RenderObjectWidget* widget);
    
    void mount(Element* parent) override;
    void update(std::shared_ptr<Widget> newWidget) override;
    void performRebuild() override {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//  InheritedWidget — مكون لتمرير بيانات للأحفاد
// ═══════════════════════════════════════════════════════════════════════════════

/// InheritedWidget — مكون يشارك بيانات مع كل الأحفاد
/// مثل: Theme, Locale, MediaQuery
class InheritedWidget : public Widget {
public:
    InheritedWidget(WidgetPtr child, Key key = Key())
        : Widget(std::move(key)), child_(std::move(child)) {}
    
    /// الابن الوحيد
    WidgetPtr child() const { return child_; }
    
    /// هل يجب إعادة بناء المكونات المعتمدة عند التحديث؟
    virtual bool updateShouldNotify(const InheritedWidget* oldWidget) const = 0;
    
    std::unique_ptr<Element> createElement() override;
    std::string typeName() const override { return "InheritedWidget"; }
    
protected:
    WidgetPtr child_;
};

/// InheritedElement — العنصر المقابل لـ InheritedWidget
class InheritedElement : public Element {
public:
    InheritedElement(InheritedWidget* widget);
    void performRebuild() override;
    void notifyDependents();
    void addDependent(Element* dependent);
    void removeDependent(Element* dependent);
    
private:
    std::vector<Element*> dependents_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  MultiChildRenderObjectWidget — مكون رسم بعدة أبناء
// ═══════════════════════════════════════════════════════════════════════════════

/// MultiChildRenderObjectWidget — مكون رسم بعدة أبناء (مثل Column, Row)
class MultiChildRenderObjectWidget : public RenderObjectWidget {
public:
    MultiChildRenderObjectWidget(WidgetList children, Key key = Key())
        : RenderObjectWidget(std::move(key)), children_(std::move(children)) {}
    
    const WidgetList& children() const { return children_; }
    
    std::string typeName() const override { return "MultiChildRenderObjectWidget"; }
    
protected:
    WidgetList children_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  SingleChildRenderObjectWidget — مكون رسم بابن واحد
// ═══════════════════════════════════════════════════════════════════════════════

/// SingleChildRenderObjectWidget — مكون رسم بابن واحد (مثل Container, Center)
class SingleChildRenderObjectWidget : public RenderObjectWidget {
public:
    SingleChildRenderObjectWidget(WidgetPtr child = nullptr, Key key = Key())
        : RenderObjectWidget(std::move(key)), child_(std::move(child)) {}
    
    WidgetPtr child() const { return child_; }
    
    std::string typeName() const override { return "SingleChildRenderObjectWidget"; }
    
protected:
    WidgetPtr child_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  مساعدات للإنشاء
// ═══════════════════════════════════════════════════════════════════════════════

/// إنشاء مكون بذكاء
template<typename T, typename... Args>
std::shared_ptr<T> widget(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace flutter
} // namespace sad
