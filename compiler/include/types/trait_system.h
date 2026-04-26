// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file trait_system.h
 * @brief نظام السمات / Trait System
 * 
 * نظام السمات في لغة ص مستوحى من Rust مع واجهة عربية كاملة:
 * - سمة (Trait) - تعريف سلوك مشترك
 * - تنفيذ (Impl) - تطبيق السمة على نوع
 * - قيد (Bound) - شرط على المعممات
 * 
 * السمات الأساسية:
 * - قابل_للنسخ (Copy)
 * - قابل_للاستنساخ (Clone)
 * - قابل_للمقارنة (PartialEq, Eq)
 * - قابل_للترتيب (PartialOrd, Ord)
 * - قابل_للتجزئة (Hash)
 * - قابل_للعرض (Display)
 * - قابل_للتنقيح (Debug)
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_TRAIT_SYSTEM_H
#define SAD_TYPES_TRAIT_SYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace Sad {
namespace TypeSystem {

// Forward declarations
class Type;
class Trait;
class TraitImpl;
class TraitBound;

using TypePtr = std::shared_ptr<Type>;
using TraitPtr = std::shared_ptr<Trait>;
using TraitImplPtr = std::shared_ptr<TraitImpl>;
using TraitBoundPtr = std::shared_ptr<TraitBound>;

// =============================================================================
//                    تعريف دالة السمة / Trait Method Definition
// =============================================================================

/**
 * @struct TraitMethod
 * @brief تعريف دالة في سمة
 */
struct TraitMethod {
    std::string arabicName;         // الاسم العربي
    std::string englishName;        // الاسم الإنجليزي
    std::vector<TypePtr> paramTypes; // أنواع المعاملات
    TypePtr returnType;             // نوع الإرجاع
    bool hasDefaultImpl;            // هل لها تنفيذ افتراضي؟
    bool isStatic;                  // هل دالة ثابتة؟
    bool requiresSelf;              // هل تحتاج self؟
    bool mutatesSelf;               // هل تعدل self؟
    
    /**
     * @brief الحصول على التوقيع النصي
     */
    std::string getSignature(bool arabic = true) const;
};

// =============================================================================
//                    السمة / Trait
// =============================================================================

/**
 * @class Trait
 * @brief تمثيل سمة (Trait/Interface)
 */
class Trait : public std::enable_shared_from_this<Trait> {
public:
    /**
     * @brief إنشاء سمة جديدة
     */
    Trait(const std::string& arabicName, const std::string& englishName);
    
    virtual ~Trait() = default;
    
    // ==========================================================================
    //                    الاسم والهوية / Name and Identity
    // ==========================================================================
    
    const std::string& getArabicName() const { return arabicName_; }
    const std::string& getEnglishName() const { return englishName_; }
    std::string getName(bool arabic = true) const;
    
    /**
     * @brief الحصول على المعرف الفريد
     */
    size_t getId() const { return id_; }
    
    // ==========================================================================
    //                    الدوال / Methods
    // ==========================================================================
    
    /**
     * @brief إضافة دالة للسمة
     */
    void addMethod(const TraitMethod& method);
    
    /**
     * @brief الحصول على كل الدوال
     */
    const std::vector<TraitMethod>& getMethods() const { return methods_; }
    
    /**
     * @brief البحث عن دالة بالاسم
     */
    const TraitMethod* findMethod(const std::string& name) const;
    
    /**
     * @brief عدد الدوال
     */
    size_t getMethodCount() const { return methods_.size(); }
    
    // ==========================================================================
    //                    السمات الأم / Super Traits
    // ==========================================================================
    
    /**
     * @brief إضافة سمة أم (التي يجب تنفيذها أيضاً)
     */
    void addSuperTrait(TraitPtr superTrait);
    
    /**
     * @brief الحصول على السمات الأم
     */
    const std::vector<TraitPtr>& getSuperTraits() const { return superTraits_; }
    
    /**
     * @brief هل السمة مشتقة من سمة أخرى؟
     */
    bool extendsTraitTransitive(const Trait* other) const;
    
    // ==========================================================================
    //                    الأنواع المرتبطة / Associated Types
    // ==========================================================================
    
    /**
     * @brief إضافة نوع مرتبط
     */
    void addAssociatedType(const std::string& name);
    
    /**
     * @brief الحصول على الأنواع المرتبطة
     */
    const std::vector<std::string>& getAssociatedTypes() const { return associatedTypes_; }
    
    // ==========================================================================
    //                    الخصائص / Properties
    // ==========================================================================
    
    /**
     * @brief هل السمة علامة فقط (marker trait)؟
     */
    bool isMarker() const { return methods_.empty() && associatedTypes_.empty(); }
    
    /**
     * @brief هل السمة آمنة للكائنات (object-safe)؟
     */
    bool isObjectSafe() const;
    
    /**
     * @brief هل السمة تلقائية (auto trait)؟
     */
    bool isAuto() const { return isAuto_; }
    void setAuto(bool value) { isAuto_ = value; }

private:
    std::string arabicName_;
    std::string englishName_;
    size_t id_;
    std::vector<TraitMethod> methods_;
    std::vector<TraitPtr> superTraits_;
    std::vector<std::string> associatedTypes_;
    bool isAuto_ = false;
    
    static size_t nextId_;
};

// =============================================================================
//                    تنفيذ السمة / Trait Implementation
// =============================================================================

/**
 * @struct MethodImpl
 * @brief تنفيذ دالة من سمة
 */
struct MethodImpl {
    std::string methodName;         // اسم الدالة في السمة
    // هنا سيكون مؤشر للكود الفعلي
    // في المستقبل: AST node أو function pointer
};

/**
 * @class TraitImpl
 * @brief تنفيذ سمة لنوع معين
 */
class TraitImpl {
public:
    /**
     * @brief إنشاء تنفيذ جديد
     */
    TraitImpl(TypePtr forType, TraitPtr trait);
    
    /**
     * @brief الحصول على النوع
     */
    TypePtr getType() const { return forType_; }
    
    /**
     * @brief الحصول على السمة
     */
    TraitPtr getTrait() const { return trait_; }
    
    /**
     * @brief إضافة تنفيذ دالة
     */
    void addMethodImpl(const MethodImpl& impl);
    
    /**
     * @brief الحصول على تنفيذات الدوال
     */
    const std::vector<MethodImpl>& getMethodImpls() const { return methodImpls_; }
    
    /**
     * @brief هل التنفيذ كامل؟
     */
    bool isComplete() const;
    
    /**
     * @brief الحصول على الدوال غير المنفذة
     */
    std::vector<std::string> getMissingMethods() const;

private:
    TypePtr forType_;
    TraitPtr trait_;
    std::vector<MethodImpl> methodImpls_;
};

// =============================================================================
//                    قيود السمات / Trait Bounds
// =============================================================================

/**
 * @class TraitBound
 * @brief قيد على نوع معمم
 * 
 * مثال: حيث ت: قابل_للنسخ + قابل_للمقارنة
 */
class TraitBound {
public:
    /**
     * @brief إنشاء قيد فارغ
     */
    TraitBound() = default;
    
    /**
     * @brief إنشاء قيد بسمة واحدة
     */
    explicit TraitBound(TraitPtr trait);
    
    /**
     * @brief إضافة سمة للقيد
     */
    void addTrait(TraitPtr trait);
    
    /**
     * @brief الحصول على السمات
     */
    const std::vector<TraitPtr>& getTraits() const { return traits_; }
    
    /**
     * @brief هل النوع يحقق القيد؟
     */
    bool isSatisfiedBy(TypePtr type, class TraitRegistry& registry) const;
    
    /**
     * @brief تحويل للنص
     */
    std::string toString(bool arabic = true) const;

private:
    std::vector<TraitPtr> traits_;
};

// =============================================================================
//                    سجل السمات / Trait Registry
// =============================================================================

/**
 * @class TraitRegistry
 * @brief سجل السمات وتنفيذاتها
 */
class TraitRegistry {
public:
    /**
     * @brief الحصول على النسخة الوحيدة
     */
    static TraitRegistry& instance();
    
    // ==========================================================================
    //                    تسجيل السمات / Trait Registration
    // ==========================================================================
    
    /**
     * @brief تسجيل سمة جديدة
     */
    void registerTrait(TraitPtr trait);
    
    /**
     * @brief البحث عن سمة بالاسم
     */
    TraitPtr findTrait(const std::string& name) const;
    
    /**
     * @brief الحصول على كل السمات
     */
    std::vector<TraitPtr> getAllTraits() const;
    
    // ==========================================================================
    //                    تسجيل التنفيذات / Implementation Registration
    // ==========================================================================
    
    /**
     * @brief تسجيل تنفيذ جديد
     */
    void registerImpl(TraitImplPtr impl);
    
    /**
     * @brief البحث عن تنفيذ
     */
    TraitImplPtr findImpl(TypePtr type, TraitPtr trait) const;
    
    /**
     * @brief هل النوع ينفذ السمة؟
     */
    bool doesImplement(TypePtr type, TraitPtr trait) const;
    
    /**
     * @brief الحصول على كل السمات التي ينفذها نوع
     */
    std::vector<TraitPtr> getImplementedTraits(TypePtr type) const;
    
    /**
     * @brief الحصول على كل الأنواع التي تنفذ سمة
     */
    std::vector<TypePtr> getImplementingTypes(TraitPtr trait) const;
    
    // ==========================================================================
    //                    السمات الأساسية / Core Traits
    // ==========================================================================
    
    TraitPtr getCopyTrait() const { return copyTrait_; }
    TraitPtr getCloneTrait() const { return cloneTrait_; }
    TraitPtr getEqTrait() const { return eqTrait_; }
    TraitPtr getOrdTrait() const { return ordTrait_; }
    TraitPtr getHashTrait() const { return hashTrait_; }
    TraitPtr getDisplayTrait() const { return displayTrait_; }
    TraitPtr getDebugTrait() const { return debugTrait_; }

private:
    TraitRegistry();
    void initializeCoreTraits();
    
    std::unordered_map<std::string, TraitPtr> traitsByName_;
    std::vector<TraitImplPtr> implementations_;
    
    // السمات الأساسية
    TraitPtr copyTrait_;
    TraitPtr cloneTrait_;
    TraitPtr eqTrait_;
    TraitPtr ordTrait_;
    TraitPtr hashTrait_;
    TraitPtr displayTrait_;
    TraitPtr debugTrait_;
};

// =============================================================================
//                    السمات الأساسية المُعرَّفة مسبقاً
// =============================================================================

/**
 * @brief إنشاء سمة قابل_للنسخ
 */
TraitPtr createCopyTrait();

/**
 * @brief إنشاء سمة قابل_للاستنساخ
 */
TraitPtr createCloneTrait();

/**
 * @brief إنشاء سمة قابل_للمقارنة
 */
TraitPtr createEqTrait();

/**
 * @brief إنشاء سمة قابل_للترتيب
 */
TraitPtr createOrdTrait();

/**
 * @brief إنشاء سمة قابل_للتجزئة
 */
TraitPtr createHashTrait();

/**
 * @brief إنشاء سمة قابل_للعرض
 */
TraitPtr createDisplayTrait();

/**
 * @brief إنشاء سمة قابل_للتنقيح
 */
TraitPtr createDebugTrait();

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_TRAIT_SYSTEM_H
