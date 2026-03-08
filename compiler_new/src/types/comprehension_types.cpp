/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                    نظام التعبيرات التوليدية - لغة ص                        ║
 * ║                  Comprehension System - Sad Language                      ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * الملف: comprehension_types.cpp
 * File: comprehension_types.cpp
 * 
 * الوصف: تنفيذ نظام التعبيرات التوليدية (List/Set/Dict Comprehensions)
 * Description: Implementation of comprehension expressions for collections
 * 
 * المهمة: T149 - دعم المولدات والتعبيرات التوليدية
 * Task: T149 - Generator and Comprehension Support
 * 
 * التاريخ: 26 يناير 2026
 * Date: January 26, 2026
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              المفاهيم الأساسية
 *                              Core Concepts
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * 🌟 ما هي التعبيرات التوليدية (Comprehensions)؟
 * 
 *    التعبيرات التوليدية هي طريقة موجزة وقوية لإنشاء مجموعات (قوائم، مجموعات،
 *    قواميس) بناءً على مجموعة موجودة، مع إمكانية تطبيق تحويلات وفلاتر.
 * 
 *    Comprehensions are a concise and powerful way to create collections
 *    (lists, sets, dicts) based on existing ones with transformations and filters.
 * 
 * 📋 الأنواع المدعومة:
 * 
 *    1️⃣ تعبيرات القوائم (List Comprehensions):
 *       [x * 2 لكل x في قائمة]
 *       [x * 2 for x in list]
 * 
 *    2️⃣ تعبيرات المجموعات (Set Comprehensions):
 *       {x * 2 لكل x في قائمة}
 *       {x * 2 for x in list}
 * 
 *    3️⃣ تعبيرات القواميس (Dict Comprehensions):
 *       {x: x * 2 لكل x في قائمة}
 *       {x: x * 2 for x in list}
 * 
 *    4️⃣ تعبيرات المولدات (Generator Comprehensions):
 *       (x * 2 لكل x في قائمة)
 *       (x * 2 for x in list)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              أمثلة الاستخدام
 *                              Usage Examples
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * // مثال 1: تعبير قائمة بسيط - مضاعفة الأرقام
 * // Example 1: Simple list comprehension - doubling numbers
 * دع أرقام = [1، 2، 3، 4، 5]؛
 * دع مضاعف = [ع * 2 لكل ع في أرقام]؛
 * // النتيجة: [2, 4, 6, 8, 10]
 * 
 * // مثال 2: تعبير قائمة مع شرط - الأرقام الزوجية فقط
 * // Example 2: List comprehension with condition - even numbers only
 * دع زوجي = [ع لكل ع في أرقام إذا ع % 2 == 0]؛
 * // النتيجة: [2, 4]
 * 
 * // مثال 3: تعبير قاموس - مربع الأرقام
 * // Example 3: Dict comprehension - squares of numbers
 * دع مربعات = {ع: ع * ع لكل ع في أرقام}؛
 * // النتيجة: {1: 1, 2: 4, 3: 9, 4: 16, 5: 25}
 * 
 * // مثال 4: تعبيرات متداخلة - جدول الضرب
 * // Example 4: Nested comprehensions - multiplication table
 * دع جدول_ضرب = [
 *     [ع * س لكل س في مدى(1، 11)]
 *     لكل ع في مدى(1، 11)
 * ]؛
 * 
 * // مثال 5: تعبير مولد - معالجة كسولة
 * // Example 5: Generator comprehension - lazy processing
 * دع مولد = (ع * 2 لكل ع في أرقام)؛
 * // لا يحسب القيم حتى يُطلب منه ذلك
 * 
 * // مثال 6: تطبيق سلسلة من العمليات
 * // Example 6: Chain of operations
 * دع أسماء = ["أحمد"، "محمد"، "علي"]؛
 * دع كبير_وطويل = [
 *     اسم.كبّر()
 *     لكل اسم في أسماء
 *     إذا اسم.طول() > 3
 * ]؛
 * 
 * // مثال 7: استخدام مع Map و Filter
 * // Example 7: Using with Map and Filter
 * دع نتيجة = أرقام
 *     .حوّل(ع => ع * 2)       // Map
 *     .رشّح(ع => ع > 5)       // Filter
 *     .خذ(3)                  // Take first 3
 *     .اجمع()؛                // Collect to list
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                           العمليات المدعومة
 *                           Supported Operations
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * 🔄 Map (حوّل): تطبيق دالة على كل عنصر
 *    [f(x) for x in list] -> [f(x₁), f(x₂), ..., f(xₙ)]
 * 
 * 🔍 Filter (رشّح): اختيار عناصر تحقق شرط
 *    [x for x in list if condition] -> عناصر تحقق الشرط فقط
 * 
 * 🎯 FlatMap (سطّح): Map ثم تسطيح النتيجة
 *    [y for x in list for y in f(x)] -> قائمة مسطحة
 * 
 * 🔗 Zip (ادمج): دمج قوائم متعددة
 *    [(x, y) for x, y in zip(list1, list2)]
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "../../include/types/comprehension_types.hpp"
#include "../../include/types/generator_type.hpp"
#include <memory>
#include <string>
#include <stdexcept>

namespace Sad {
namespace TypeSystem {

/**
 * المُنشئ: إنشاء نوع تعبير توليدي جديد
 * Constructor: Create a new Comprehension type
 * 
 * @param elementType نوع العناصر الناتجة
 *                    The type of resulting elements
 * @param operation نوع العملية (Map/Filter/FlatMap/Zip)
 *                  The operation type
 * 
 * مثال:
 * - Comprehension<عدد, Map> -> تحويل إلى أرقام
 * - Comprehension<نص, Filter> -> فلترة النصوص
 * - Comprehension<[عدد], FlatMap> -> تسطيح القوائم
 */
ComprehensionType::ComprehensionType(TypePtr elementType, ComprehensionOp operation)
    : Type(TypeKind::Comprehension)
    , elementType_(elementType)
    , operation_(operation)
{
    if (!elementType_) {
        throw std::invalid_argument("ComprehensionType: Element type cannot be null");
    }
}

/**
 * الحصول على نوع العنصر
 * Get the element type
 * 
 * @return نوع العنصر في التعبير التوليدي
 *         The element type in the comprehension
 */
TypePtr ComprehensionType::getElementType() const {
    return elementType_;
}

/**
 * الحصول على نوع العملية
 * Get the operation type
 * 
 * @return نوع العملية المطبقة
 *         The applied operation type
 */
ComprehensionOp ComprehensionType::getOperation() const {
    return operation_;
}

/**
 * تحويل النوع إلى نص للعرض
 * Convert type to string for display
 * 
 * @return تمثيل نصي للنوع
 *         String representation of the type
 */
std::string ComprehensionType::toString() const {
    std::string opName;
    switch (operation_) {
        case ComprehensionOp::Map:
            opName = "حوّل"; // Map
            break;
        case ComprehensionOp::Filter:
            opName = "رشّح"; // Filter
            break;
        case ComprehensionOp::FlatMap:
            opName = "سطّح"; // FlatMap
            break;
        case ComprehensionOp::Zip:
            opName = "ادمج"; // Zip
            break;
        default:
            opName = "غير معروف"; // Unknown
    }
    
    return "[" + opName + "<" + elementType_->toString() + ">]";
}

/**
 * التحقق من تطابق نوعين
 * Check if two types are equal
 * 
 * @param other النوع الآخر للمقارنة
 *              Other type to compare with
 * @return true إذا كان النوعان متطابقان
 *         true if types are equal
 */
bool ComprehensionType::equals(const Type* other) const {
    if (!other || other->getKind() != TypeKind::Comprehension) {
        return false;
    }
    
    auto otherComp = static_cast<const ComprehensionType*>(other);
    return operation_ == otherComp->operation_ &&
           elementType_->equals(otherComp->elementType_.get());
}

/**
 * استنساخ النوع
 * Clone the type
 * 
 * @return نسخة جديدة من النوع
 *         New copy of the type
 */
std::shared_ptr<Type> ComprehensionType::clone() const {
    return std::make_shared<ComprehensionType>(elementType_, operation_);
}

/**
 * التحقق من إمكانية تحويل النوع إلى نوع آخر
 * Check if type can be converted to another type
 * 
 * @param target النوع المستهدف
 *               Target type
 * @return true إذا كان التحويل ممكناً
 *         true if conversion is possible
 * 
 * مثال:
 * - Comprehension<عدد> يمكن تحويله إلى قائمة<عدد>
 * - Comprehension<T> يمكن تحويله إلى مولد<T>
 */
bool ComprehensionType::isConvertibleTo(const Type& target) const {
    // يمكن تحويل Comprehension إلى:
    // 1. نفس النوع
    if (equals(&target)) {
        return true;
    }
    
    // 2. مولد من نفس نوع العنصر
    // (EN) Generator with same yield type
    const GeneratorType* genType = dynamic_cast<const GeneratorType*>(&target);
    if (genType != nullptr) {
        TypePtr yieldType = genType->getYieldType();
        if (elementType_ && yieldType && elementType_->equals(yieldType.get())) {
            return true;
        }
    }
    
    // 3. مصفوفة (قائمة) من نفس نوع العنصر — معلّق لحين توفر ListType
    // (EN) List with same element type — pending ListType availability
    
    return false;
}

} // namespace TypeSystem
} // namespace Sad
