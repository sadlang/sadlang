// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_core.cpp — تنفيذ النواة الأساسية لنظام Flutter
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_core.h"
#include <atomic>
#include <chrono>
#include <algorithm>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Key — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

static std::atomic<int> s_uniqueKeyCounter{0};
static std::unordered_map<std::string, Element*> s_globalKeyRegistry;

Key Key::unique() {
    Key k;
    k.type_ = KeyType::Unique;
    k.value_ = "__unique_" + std::to_string(++s_uniqueKeyCounter);
    return k;
}

Key Key::global(const std::string& id) {
    Key k;
    k.type_ = KeyType::Global;
    k.value_ = id;
    return k;
}

std::string Key::toString() const {
    switch (type_) {
        case KeyType::None: return "<no-key>";
        case KeyType::Value: return "ValueKey(" + value_ + ")";
        case KeyType::Unique: return "UniqueKey(" + value_ + ")";
        case KeyType::Global: return "GlobalKey(" + value_ + ")";
        case KeyType::Object: return "ObjectKey(" + value_ + ")";
    }
    return "<unknown>";
}

Key globalKey(const std::string& id) {
    return Key::global(id);
}

Element* findElementByGlobalKey(const Key& key) {
    if (key.type() != KeyType::Global) return nullptr;
    auto it = s_globalKeyRegistry.find(key.value());
    return (it != s_globalKeyRegistry.end()) ? it->second : nullptr;
}

State_Base* findStateByGlobalKey(const Key& key) {
    Element* elem = findElementByGlobalKey(key);
    if (!elem) return nullptr;
    // Try to cast to StatefulElement
    auto* stateful = dynamic_cast<StatefulElement*>(elem);
    return stateful ? stateful->state() : nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Element — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

Element::Element(Widget* widget)
    : widget_(std::shared_ptr<Widget>(widget, [](Widget*){}))
    , context_(this)
{
}

Element::~Element() {
    // إزالة من GlobalKey registry
    if (widget_ && widget_->key().type() == KeyType::Global) {
        s_globalKeyRegistry.erase(widget_->key().value());
    }
}

void Element::mount(Element* parent) {
    parent_ = parent;
    depth_ = parent ? parent->depth() + 1 : 0;
    mounted_ = true;
    
    // تسجيل GlobalKey
    if (widget_->key().type() == KeyType::Global) {
        s_globalKeyRegistry[widget_->key().value()] = this;
    }
    
    // إعادة البناء الأولي
    rebuild();
}

void Element::unmount() {
    // إلغاء تركيب الأبناء أولاً
    for (auto& child : children_) {
        child->unmount();
    }
    children_.clear();
    
    // إزالة من GlobalKey registry
    if (widget_->key().type() == KeyType::Global) {
        s_globalKeyRegistry.erase(widget_->key().value());
    }
    
    mounted_ = false;
    parent_ = nullptr;
}

void Element::update(std::shared_ptr<Widget> newWidget) {
    widget_ = newWidget;
    
    // تحديث GlobalKey registry
    if (newWidget->key().type() == KeyType::Global) {
        s_globalKeyRegistry[newWidget->key().value()] = this;
    }
}

void Element::rebuild() {
    if (!mounted_) return;
    dirty_ = false;
    performRebuild();
}

void Element::markNeedsBuild() {
    if (dirty_) return;
    dirty_ = true;
    // TODO: إضافة إلى قائمة العناصر القذرة للبناء في الإطار التالي
}

void Element::attachChild(std::unique_ptr<Element> child) {
    children_.push_back(std::move(child));
    children_.back()->mount(this);
}

std::unique_ptr<Element> Element::updateChild(
    std::unique_ptr<Element> child,
    std::shared_ptr<Widget> newWidget
) {
    // حالة 1: لا يوجد ابن قديم ولا جديد
    if (!child && !newWidget) {
        return nullptr;
    }
    
    // حالة 2: لا يوجد ابن قديم، ننشئ جديد
    if (!child && newWidget) {
        auto newChild = newWidget->createElement();
        newChild->mount(this);
        return newChild;
    }
    
    // حالة 3: يوجد ابن قديم، لا يوجد جديد
    if (child && !newWidget) {
        child->unmount();
        return nullptr;
    }
    
    // حالة 4: كلاهما موجود
    if (newWidget->canUpdate(child->widget())) {
        // نفس النوع — نحدّث
        child->update(newWidget);
        child->rebuild();
        return child;
    } else {
        // نوع مختلف — نستبدل
        child->unmount();
        auto newChild = newWidget->createElement();
        newChild->mount(this);
        return newChild;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  BuildContext — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

BuildContext::Size BuildContext::mediaQuerySize() const {
    // TODO: الحصول على حجم الشاشة من النظام
    return {800.0f, 600.0f};
}

bool BuildContext::isPortrait() const {
    auto size = mediaQuerySize();
    return size.height > size.width;
}

bool BuildContext::isRTL() const {
    // افتراضي: RTL للعربية
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  StatelessWidget — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<Element> StatelessWidget::createElement() {
    return std::make_unique<StatelessElement>(this);
}

StatelessElement::StatelessElement(StatelessWidget* widget)
    : Element(widget)
{
}

void StatelessElement::performRebuild() {
    auto* sw = static_cast<StatelessWidget*>(widget_.get());
    auto built = sw->build(&context_);
    
    // تحديث الابن الوحيد
    std::unique_ptr<Element> existingChild;
    if (!children_.empty()) {
        existingChild = std::move(children_[0]);
        children_.clear();
    }
    
    auto newChild = updateChild(std::move(existingChild), built);
    if (newChild) {
        children_.push_back(std::move(newChild));
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  State_Base — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

void State_Base::markNeedsBuild() {
    if (element_) {
        element_->markNeedsBuild();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  StatefulWidget — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<Element> StatefulWidget::createElement() {
    return std::make_unique<StatefulElement>(this);
}

StatefulElement::StatefulElement(StatefulWidget* widget)
    : Element(widget)
    , state_(widget->createState())
{
    state_->element_ = this;
    state_->context_ = &context_;
}

StatefulElement::~StatefulElement() {
    if (state_ && state_->mounted_) {
        state_->dispose();
    }
}

void StatefulElement::mount(Element* parent) {
    Element::mount(parent);
    
    // دورة حياة الحالة
    state_->widget_ = widget_.get();
    state_->mounted_ = true;
    state_->initState();
    state_->didChangeDependencies();
}

void StatefulElement::unmount() {
    state_->deactivate();
    state_->dispose();
    state_->mounted_ = false;
    
    Element::unmount();
}

void StatefulElement::update(std::shared_ptr<Widget> newWidget) {
    auto* oldWidget = widget_.get();
    Element::update(newWidget);
    
    state_->widget_ = newWidget.get();
    state_->didUpdateWidget(oldWidget);
}

void StatefulElement::performRebuild() {
    auto built = static_cast<State_Base*>(state_.get())->context_ = &context_;
    
    // استدعاء build على الحالة
    // نحتاج لاستدعاء build بشكل ديناميكي
    // هذا يتطلب virtual في State_Base
    
    // الحصول على نتيجة البناء عبر visitor pattern
    WidgetPtr builtWidget = nullptr;
    
    // TODO: استدعاء build الصحيح
    // حالياً نفترض أن لدينا طريقة للوصول إليه
    
    // تحديث الابن
    std::unique_ptr<Element> existingChild;
    if (!children_.empty()) {
        existingChild = std::move(children_[0]);
        children_.clear();
    }
    
    if (builtWidget) {
        auto newChild = updateChild(std::move(existingChild), builtWidget);
        if (newChild) {
            children_.push_back(std::move(newChild));
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  RenderObjectWidget — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<Element> RenderObjectWidget::createElement() {
    return std::make_unique<RenderObjectElement>(this);
}

RenderObjectElement::RenderObjectElement(RenderObjectWidget* widget)
    : Element(widget)
{
}

void RenderObjectElement::mount(Element* parent) {
    Element::mount(parent);
    
    auto* row = static_cast<RenderObjectWidget*>(widget_.get());
    renderObject_ = row->createRenderObject(&context_);
}

void RenderObjectElement::update(std::shared_ptr<Widget> newWidget) {
    Element::update(newWidget);
    
    auto* row = static_cast<RenderObjectWidget*>(newWidget.get());
    row->updateRenderObject(&context_, renderObject_.get());
}

// ═══════════════════════════════════════════════════════════════════════════════
//  InheritedWidget — التنفيذ
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<Element> InheritedWidget::createElement() {
    return std::make_unique<InheritedElement>(this);
}

InheritedElement::InheritedElement(InheritedWidget* widget)
    : Element(widget)
{
}

void InheritedElement::performRebuild() {
    auto* iw = static_cast<InheritedWidget*>(widget_.get());
    auto childWidget = iw->child();
    
    std::unique_ptr<Element> existingChild;
    if (!children_.empty()) {
        existingChild = std::move(children_[0]);
        children_.clear();
    }
    
    auto newChild = updateChild(std::move(existingChild), childWidget);
    if (newChild) {
        children_.push_back(std::move(newChild));
    }
}

void InheritedElement::notifyDependents() {
    for (auto* dep : dependents_) {
        dep->markNeedsBuild();
    }
}

void InheritedElement::addDependent(Element* dependent) {
    dependents_.push_back(dependent);
}

void InheritedElement::removeDependent(Element* dependent) {
    dependents_.erase(
        std::remove(dependents_.begin(), dependents_.end(), dependent),
        dependents_.end()
    );
}

} // namespace flutter
} // namespace sad
