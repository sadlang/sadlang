// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file generics.h
 * @brief نظام الأنواع المعممة / Generics System
 * 
 * نظام المعممات في لغة ص:
 * - معمم<ت> - تعريف نوع معمم
 * - حيث ت: سمة - قيد على النوع المعمم
 * - التخصيص (Monomorphization) - إنشاء نسخ متخصصة
 * 
 * بناء الجملة:
 *   دالة<ت> اسم(معامل: ت) -> ت { ... }
 *   دالة<ت: قابل_للنسخ> اسم(معامل: ت) -> ت { ... }
 *   بنية<ت> قائمة { عناصر: [ت] }
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_GENERICS_H
#define SAD_TYPES_GENERICS_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include "trait_system.h"

namespace Sad {
namespace TypeSystem {

// Forward declarations
class Type;
class GenericParameter;
class GenericType;
class GenericInstantiation;
class Monomorphizer;

using TypePtr = std::shared_ptr<Type>;
using GenericParameterPtr = std::shared_ptr<GenericParameter>;
using GenericTypePtr = std::shared_ptr<GenericType>;
using GenericInstantiationPtr = std::shared_ptr<GenericInstantiation>;

// =============================================================================
//                    معامل معمم / Generic Parameter
// =============================================================================

/**
 * @class GenericParameter
 * @brief معامل نوع معمم (مثل ت في دالة<ت>)
 */
class GenericParameter {
public:
    /**
     * @brief إنشاء معامل معمم
     * @param name اسم المعامل (مثل "ت")
     * @param index الترتيب في قائمة المعاملات
     */
    GenericParameter(const std::string& name, size_t index);
    
    /**
     * @brief الحصول على الاسم
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief الحصول على الترتيب
     */
    size_t getIndex() const { return index_; }
    
    // ==========================================================================
    //                    القيود / Bounds
    // ==========================================================================
    
    /**
     * @brief إضافة قيد سمة
     * @param trait السمة المطلوبة
     */
    void addTraitBound(TraitPtr trait);
    
    /**
     * @brief الحصول على القيود
     */
    const TraitBound& getBounds() const { return bounds_; }
    
    /**
     * @brief هل المعامل له قيود؟
     */
    bool hasBounds() const { return !bounds_.getTraits().empty(); }
    
    /**
     * @brief التحقق من أن نوع يحقق القيود
     */
    bool satisfiesBounds(TypePtr type, TraitRegistry& registry) const;
    
    // ==========================================================================
    //                    القيمة الافتراضية / Default Value
    // ==========================================================================
    
    /**
     * @brief تعيين نوع افتراضي
     */
    void setDefaultType(TypePtr defaultType);
    
    /**
     * @brief الحصول على النوع الافتراضي
     */
    TypePtr getDefaultType() const { return defaultType_; }
    
    /**
     * @brief هل هناك نوع افتراضي؟
     */
    bool hasDefault() const { return defaultType_ != nullptr; }
    
    // ==========================================================================
    //                    التحويل للنص / String Conversion
    // ==========================================================================
    
    /**
     * @brief تحويل للنص
     */
    std::string toString(bool arabic = true) const;

private:
    std::string name_;
    size_t index_;
    TraitBound bounds_;
    TypePtr defaultType_;
};

// =============================================================================
//                    نوع معمم / Generic Type
// =============================================================================

/**
 * @class GenericType
 * @brief نوع معمم (دالة أو بنية مع معاملات نوع)
 */
class GenericType : public std::enable_shared_from_this<GenericType> {
public:
    /**
     * @brief إنشاء نوع معمم
     * @param baseName اسم النوع الأساسي
     */
    explicit GenericType(const std::string& baseName);
    
    /**
     * @brief الحصول على الاسم الأساسي
     */
    const std::string& getBaseName() const { return baseName_; }
    
    // ==========================================================================
    //                    المعاملات / Parameters
    // ==========================================================================
    
    /**
     * @brief إضافة معامل معمم
     */
    void addParameter(GenericParameterPtr param);
    
    /**
     * @brief الحصول على المعاملات
     */
    const std::vector<GenericParameterPtr>& getParameters() const { 
        return parameters_; 
    }
    
    /**
     * @brief الحصول على عدد المعاملات
     */
    size_t getParameterCount() const { return parameters_.size(); }
    
    /**
     * @brief البحث عن معامل بالاسم
     */
    GenericParameterPtr findParameter(const std::string& name) const;
    
    /**
     * @brief الحصول على معامل بالترتيب
     */
    GenericParameterPtr getParameter(size_t index) const;
    
    // ==========================================================================
    //                    التخصيص / Instantiation
    // ==========================================================================
    
    /**
     * @brief إنشاء نسخة متخصصة
     * @param typeArgs الأنواع المحددة لكل معامل
     */
    GenericInstantiationPtr instantiate(
        const std::vector<TypePtr>& typeArgs,
        TraitRegistry& registry) const;
    
    /**
     * @brief التحقق من صحة الأنواع المحددة
     */
    bool validateTypeArgs(
        const std::vector<TypePtr>& typeArgs,
        TraitRegistry& registry,
        std::vector<std::string>& errors) const;
    
    // ==========================================================================
    //                    التحويل للنص / String Conversion
    // ==========================================================================
    
    /**
     * @brief تحويل للنص
     */
    std::string toString(bool arabic = true) const;

private:
    std::string baseName_;
    std::vector<GenericParameterPtr> parameters_;
};

// =============================================================================
//                    تخصيص معمم / Generic Instantiation
// =============================================================================

/**
 * @class GenericInstantiation
 * @brief نسخة متخصصة من نوع معمم
 * 
 * مثال: قائمة<عدد> هو تخصيص لـ قائمة<ت>
 */
class GenericInstantiation {
public:
    /**
     * @brief إنشاء تخصيص
     */
    GenericInstantiation(
        GenericTypePtr genericType,
        const std::vector<TypePtr>& typeArgs);
    
    /**
     * @brief الحصول على النوع المعمم الأصلي
     */
    GenericTypePtr getGenericType() const { return genericType_; }
    
    /**
     * @brief الحصول على الأنواع المحددة
     */
    const std::vector<TypePtr>& getTypeArgs() const { return typeArgs_; }
    
    /**
     * @brief الحصول على نوع محدد بالترتيب
     */
    TypePtr getTypeArg(size_t index) const;
    
    /**
     * @brief الحصول على اسم التخصيص
     */
    std::string getName(bool arabic = true) const;
    
    /**
     * @brief الحصول على مفتاح التخصيص (للتخزين)
     */
    std::string getKey() const;
    
    /**
     * @brief هل تخصيصان متطابقان؟
     */
    bool equals(const GenericInstantiation& other) const;

private:
    GenericTypePtr genericType_;
    std::vector<TypePtr> typeArgs_;
};

// =============================================================================
//                    محلل المعممات / Generics Parser
// =============================================================================

/**
 * @struct ParsedGenericDecl
 * @brief نتيجة تحليل تعريف معمم
 */
struct ParsedGenericDecl {
    std::string baseName;
    std::vector<GenericParameterPtr> parameters;
    bool isValid = false;
    std::string errorMessage;
};

/**
 * @class GenericsParser
 * @brief تحليل تعريفات المعممات من الكود
 */
class GenericsParser {
public:
    /**
     * @brief تحليل تعريف معمم
     * @param declaration التعريف (مثل "قائمة<ت: قابل_للنسخ>")
     */
    ParsedGenericDecl parse(const std::string& declaration);
    
    /**
     * @brief تحليل معاملات النوع
     * @param paramsStr سلسلة المعاملات (مثل "ت, ك: سمة")
     */
    std::vector<GenericParameterPtr> parseParameters(const std::string& paramsStr);
    
    /**
     * @brief تحليل قيود السمات
     * @param boundsStr سلسلة القيود (مثل "قابل_للنسخ + قابل_للمقارنة")
     */
    TraitBound parseBounds(const std::string& boundsStr);

private:
    std::vector<std::string> splitByComma(const std::string& str);
    std::vector<std::string> splitByPlus(const std::string& str);
    std::string trim(const std::string& str);
};

// =============================================================================
//                    مُخصِّص الأنواع / Monomorphizer
// =============================================================================

/**
 * @class Monomorphizer
 * @brief تخصيص الأنواع المعممة (Monomorphization)
 */
class Monomorphizer {
public:
    /**
     * @brief الحصول على النسخة الوحيدة
     */
    static Monomorphizer& instance();
    
    // ==========================================================================
    //                    التخصيص / Specialization
    // ==========================================================================
    
    /**
     * @brief الحصول على تخصيص أو إنشاؤه
     */
    GenericInstantiationPtr getOrCreate(
        GenericTypePtr genericType,
        const std::vector<TypePtr>& typeArgs,
        TraitRegistry& registry);
    
    /**
     * @brief البحث عن تخصيص موجود
     */
    GenericInstantiationPtr find(const std::string& key) const;
    
    /**
     * @brief الحصول على كل التخصيصات
     */
    const std::unordered_map<std::string, GenericInstantiationPtr>& 
    getAllInstantiations() const { return instantiations_; }
    
    // ==========================================================================
    //                    الإحصائيات / Statistics
    // ==========================================================================
    
    /**
     * @brief عدد التخصيصات
     */
    size_t getInstantiationCount() const { return instantiations_.size(); }
    
    /**
     * @brief مسح كل التخصيصات
     */
    void clear();

private:
    Monomorphizer() = default;
    
    std::unordered_map<std::string, GenericInstantiationPtr> instantiations_;
};

// =============================================================================
//                    دوال مساعدة / Helper Functions
// =============================================================================

/**
 * @brief استخراج معاملات النوع من اسم نوع
 * @param typeName مثل "قائمة<عدد, نص>"
 * @return اسم القاعدة والمعاملات
 */
std::pair<std::string, std::vector<std::string>> 
extractTypeParameters(const std::string& typeName);

/**
 * @brief بناء اسم نوع معمم متخصص
 */
std::string buildInstantiatedTypeName(
    const std::string& baseName,
    const std::vector<std::string>& typeArgNames);

/**
 * @brief هل اسم النوع معمم؟
 */
bool isGenericTypeName(const std::string& typeName);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_GENERICS_H
