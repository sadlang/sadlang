// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file trait_system.cpp
 * @brief تنفيذ نظام السمات / Trait System Implementation
 */

#include "types/trait_system.h"
#include <algorithm>
#include <sstream>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    TraitMethod Implementation
// =============================================================================

std::string TraitMethod::getSignature(bool arabic) const {
    std::stringstream ss;
    
    const std::string& name = arabic ? arabicName : englishName;
    ss << (arabic ? "دالة " : "fn ") << name << "(";
    
    bool first = true;
    if (requiresSelf) {
        ss << (mutatesSelf ? (arabic ? "ذات&" : "&mut self") 
                           : (arabic ? "ذات" : "&self"));
        first = false;
    }
    
    for (const auto& param : paramTypes) {
        if (!first) ss << ", ";
        // في المستقبل: استخدام param->toString()
        ss << (arabic ? "معامل" : "param");
        first = false;
    }
    
    ss << ")";
    
    if (returnType) {
        ss << (arabic ? " -> نوع" : " -> Type");
    }
    
    return ss.str();
}

// =============================================================================
//                    Trait Implementation
// =============================================================================

size_t Trait::nextId_ = 0;

Trait::Trait(const std::string& arabicName, const std::string& englishName)
    : arabicName_(arabicName)
    , englishName_(englishName)
    , id_(nextId_++)
{
}

std::string Trait::getName(bool arabic) const {
    return arabic ? arabicName_ : englishName_;
}

void Trait::addMethod(const TraitMethod& method) {
    methods_.push_back(method);
}

const TraitMethod* Trait::findMethod(const std::string& name) const {
    for (const auto& method : methods_) {
        if (method.arabicName == name || method.englishName == name) {
            return &method;
        }
    }
    return nullptr;
}

void Trait::addSuperTrait(TraitPtr superTrait) {
    if (superTrait && superTrait.get() != this) {
        superTraits_.push_back(superTrait);
    }
}

bool Trait::extendsTraitTransitive(const Trait* other) const {
    if (!other) return false;
    if (this == other) return true;
    
    for (const auto& superTrait : superTraits_) {
        if (superTrait.get() == other || 
            superTrait->extendsTraitTransitive(other)) {
            return true;
        }
    }
    
    return false;
}

void Trait::addAssociatedType(const std::string& name) {
    associatedTypes_.push_back(name);
}

bool Trait::isObjectSafe() const {
    // السمة آمنة للكائنات إذا:
    // 1. لا توجد دوال ثابتة بدون self
    // 2. لا توجد أنواع مرتبطة (في التنفيذ المبسط)
    // 3. كل الدوال تأخذ self
    
    if (!associatedTypes_.empty()) {
        return false;
    }
    
    for (const auto& method : methods_) {
        if (method.isStatic || !method.requiresSelf) {
            return false;
        }
    }
    
    return true;
}

// =============================================================================
//                    TraitImpl Implementation
// =============================================================================

TraitImpl::TraitImpl(TypePtr forType, TraitPtr trait)
    : forType_(forType)
    , trait_(trait)
{
}

void TraitImpl::addMethodImpl(const MethodImpl& impl) {
    methodImpls_.push_back(impl);
}

bool TraitImpl::isComplete() const {
    if (!trait_) return false;
    
    for (const auto& method : trait_->getMethods()) {
        if (method.hasDefaultImpl) continue;
        
        bool found = false;
        for (const auto& impl : methodImpls_) {
            if (impl.methodName == method.arabicName || 
                impl.methodName == method.englishName) {
                found = true;
                break;
            }
        }
        
        if (!found) return false;
    }
    
    return true;
}

std::vector<std::string> TraitImpl::getMissingMethods() const {
    std::vector<std::string> missing;
    
    if (!trait_) return missing;
    
    for (const auto& method : trait_->getMethods()) {
        if (method.hasDefaultImpl) continue;
        
        bool found = false;
        for (const auto& impl : methodImpls_) {
            if (impl.methodName == method.arabicName || 
                impl.methodName == method.englishName) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            missing.push_back(method.arabicName);
        }
    }
    
    return missing;
}

// =============================================================================
//                    TraitBound Implementation
// =============================================================================

TraitBound::TraitBound(TraitPtr trait) {
    if (trait) {
        traits_.push_back(trait);
    }
}

void TraitBound::addTrait(TraitPtr trait) {
    if (trait) {
        traits_.push_back(trait);
    }
}

bool TraitBound::isSatisfiedBy(TypePtr type, TraitRegistry& registry) const {
    for (const auto& trait : traits_) {
        if (!registry.doesImplement(type, trait)) {
            return false;
        }
    }
    return true;
}

std::string TraitBound::toString(bool arabic) const {
    if (traits_.empty()) return "";
    
    std::stringstream ss;
    bool first = true;
    
    for (const auto& trait : traits_) {
        if (!first) ss << " + ";
        ss << trait->getName(arabic);
        first = false;
    }
    
    return ss.str();
}

// =============================================================================
//                    TraitRegistry Implementation
// =============================================================================

TraitRegistry& TraitRegistry::instance() {
    static TraitRegistry instance;
    return instance;
}

TraitRegistry::TraitRegistry() {
    initializeCoreTraits();
}

void TraitRegistry::initializeCoreTraits() {
    // إنشاء السمات الأساسية
    copyTrait_ = createCopyTrait();
    cloneTrait_ = createCloneTrait();
    eqTrait_ = createEqTrait();
    ordTrait_ = createOrdTrait();
    hashTrait_ = createHashTrait();
    displayTrait_ = createDisplayTrait();
    debugTrait_ = createDebugTrait();
    
    // تسجيل السمات
    registerTrait(copyTrait_);
    registerTrait(cloneTrait_);
    registerTrait(eqTrait_);
    registerTrait(ordTrait_);
    registerTrait(hashTrait_);
    registerTrait(displayTrait_);
    registerTrait(debugTrait_);
    
    // ربط العلاقات
    // Clone يتطلب Copy
    cloneTrait_->addSuperTrait(copyTrait_);
    // Ord يتطلب Eq
    ordTrait_->addSuperTrait(eqTrait_);
}

void TraitRegistry::registerTrait(TraitPtr trait) {
    if (!trait) return;
    
    traitsByName_[trait->getArabicName()] = trait;
    traitsByName_[trait->getEnglishName()] = trait;
}

TraitPtr TraitRegistry::findTrait(const std::string& name) const {
    auto it = traitsByName_.find(name);
    return (it != traitsByName_.end()) ? it->second : nullptr;
}

std::vector<TraitPtr> TraitRegistry::getAllTraits() const {
    std::vector<TraitPtr> result;
    std::unordered_set<size_t> seen;
    
    for (const auto& pair : traitsByName_) {
        if (seen.insert(pair.second->getId()).second) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

void TraitRegistry::registerImpl(TraitImplPtr impl) {
    if (impl) {
        implementations_.push_back(impl);
    }
}

TraitImplPtr TraitRegistry::findImpl(TypePtr type, TraitPtr trait) const {
    // في المستقبل: استخدام مقارنة أنواع صحيحة
    for (const auto& impl : implementations_) {
        if (impl->getType() == type && impl->getTrait() == trait) {
            return impl;
        }
    }
    return nullptr;
}

bool TraitRegistry::doesImplement(TypePtr type, TraitPtr trait) const {
    return findImpl(type, trait) != nullptr;
}

std::vector<TraitPtr> TraitRegistry::getImplementedTraits(TypePtr type) const {
    std::vector<TraitPtr> result;
    
    for (const auto& impl : implementations_) {
        if (impl->getType() == type) {
            result.push_back(impl->getTrait());
        }
    }
    
    return result;
}

std::vector<TypePtr> TraitRegistry::getImplementingTypes(TraitPtr trait) const {
    std::vector<TypePtr> result;
    
    for (const auto& impl : implementations_) {
        if (impl->getTrait() == trait) {
            result.push_back(impl->getType());
        }
    }
    
    return result;
}

// =============================================================================
//                    Core Trait Creators
// =============================================================================

TraitPtr createCopyTrait() {
    auto trait = std::make_shared<Trait>("قابل_للنسخ", "Copy");
    trait->setAuto(true);
    // سمة علامة - بدون دوال
    return trait;
}

TraitPtr createCloneTrait() {
    auto trait = std::make_shared<Trait>("قابل_للاستنساخ", "Clone");
    
    TraitMethod cloneMethod;
    cloneMethod.arabicName = "استنسخ";
    cloneMethod.englishName = "clone";
    cloneMethod.requiresSelf = true;
    cloneMethod.mutatesSelf = false;
    cloneMethod.isStatic = false;
    cloneMethod.hasDefaultImpl = false;
    // returnType سيكون Self
    
    trait->addMethod(cloneMethod);
    
    return trait;
}

TraitPtr createEqTrait() {
    auto trait = std::make_shared<Trait>("قابل_للمقارنة", "Eq");
    
    TraitMethod eqMethod;
    eqMethod.arabicName = "يساوي";
    eqMethod.englishName = "eq";
    eqMethod.requiresSelf = true;
    eqMethod.mutatesSelf = false;
    eqMethod.isStatic = false;
    eqMethod.hasDefaultImpl = false;
    // paramTypes: [&Self]
    // returnType: bool
    
    trait->addMethod(eqMethod);
    
    // ne لها تنفيذ افتراضي (عكس eq)
    TraitMethod neMethod;
    neMethod.arabicName = "لا_يساوي";
    neMethod.englishName = "ne";
    neMethod.requiresSelf = true;
    neMethod.mutatesSelf = false;
    neMethod.isStatic = false;
    neMethod.hasDefaultImpl = true;
    
    trait->addMethod(neMethod);
    
    return trait;
}

TraitPtr createOrdTrait() {
    auto trait = std::make_shared<Trait>("قابل_للترتيب", "Ord");
    
    TraitMethod cmpMethod;
    cmpMethod.arabicName = "قارن";
    cmpMethod.englishName = "cmp";
    cmpMethod.requiresSelf = true;
    cmpMethod.mutatesSelf = false;
    cmpMethod.isStatic = false;
    cmpMethod.hasDefaultImpl = false;
    // paramTypes: [&Self]
    // returnType: Ordering
    
    trait->addMethod(cmpMethod);
    
    // lt, le, gt, ge لها تنفيذات افتراضية
    const std::vector<std::pair<std::string, std::string>> compOps = {
        {"أصغر_من", "lt"},
        {"أصغر_أو_يساوي", "le"},
        {"أكبر_من", "gt"},
        {"أكبر_أو_يساوي", "ge"}
    };
    
    for (const auto& [ar, en] : compOps) {
        TraitMethod opMethod;
        opMethod.arabicName = ar;
        opMethod.englishName = en;
        opMethod.requiresSelf = true;
        opMethod.mutatesSelf = false;
        opMethod.isStatic = false;
        opMethod.hasDefaultImpl = true;
        trait->addMethod(opMethod);
    }
    
    return trait;
}

TraitPtr createHashTrait() {
    auto trait = std::make_shared<Trait>("قابل_للتجزئة", "Hash");
    
    TraitMethod hashMethod;
    hashMethod.arabicName = "جزئ";
    hashMethod.englishName = "hash";
    hashMethod.requiresSelf = true;
    hashMethod.mutatesSelf = false;
    hashMethod.isStatic = false;
    hashMethod.hasDefaultImpl = false;
    // paramTypes: [&mut Hasher]
    
    trait->addMethod(hashMethod);
    
    return trait;
}

TraitPtr createDisplayTrait() {
    auto trait = std::make_shared<Trait>("قابل_للعرض", "Display");
    
    TraitMethod fmtMethod;
    fmtMethod.arabicName = "صيغ";
    fmtMethod.englishName = "fmt";
    fmtMethod.requiresSelf = true;
    fmtMethod.mutatesSelf = false;
    fmtMethod.isStatic = false;
    fmtMethod.hasDefaultImpl = false;
    // paramTypes: [&mut Formatter]
    // returnType: Result<(), Error>
    
    trait->addMethod(fmtMethod);
    
    return trait;
}

TraitPtr createDebugTrait() {
    auto trait = std::make_shared<Trait>("قابل_للتنقيح", "Debug");
    
    TraitMethod fmtMethod;
    fmtMethod.arabicName = "صيغ_تنقيح";
    fmtMethod.englishName = "fmt";
    fmtMethod.requiresSelf = true;
    fmtMethod.mutatesSelf = false;
    fmtMethod.isStatic = false;
    fmtMethod.hasDefaultImpl = false;
    
    trait->addMethod(fmtMethod);
    
    return trait;
}

} // namespace TypeSystem
} // namespace Sad
