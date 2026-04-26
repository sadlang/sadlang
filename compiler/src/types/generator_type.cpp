/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                          نظام المولدات - لغة ص                             ║
 * ║                      Generator System - Sad Language                      ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * الملف: generator_type.cpp
 * File: generator_type.cpp
 * 
 * الوصف: تنفيذ نظام المولدات (Generators) للمتسلسلات الكسولة في لغة ص
 * Description: Implementation of Generator system for lazy sequences
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
 * 🌟 ما هو المولد (Generator)؟
 * 
 *    المولد هو دالة خاصة تُنتج سلسلة من القيم بشكل كسول (Lazy)، أي أنها
 *    تحسب القيمة التالية فقط عند الحاجة إليها، بدلاً من حساب جميع القيم
 *    مرة واحدة وتخزينها في الذاكرة.
 * 
 *    A Generator is a special function that produces a sequence of values lazily,
 *    computing the next value only when needed instead of computing all at once.
 * 
 * 📋 الفوائد الأساسية:
 * 
 *    1️⃣ كفاءة الذاكرة: لا يحتاج لتخزين جميع القيم
 *       Memory efficient: No need to store all values
 * 
 *    2️⃣ متسلسلات لانهائية: يمكن تمثيل متسلسلات لا نهائية
 *       Infinite sequences: Can represent infinite sequences
 * 
 *    3️⃣ تقييم كسول: الحساب عند الحاجة فقط
 *       Lazy evaluation: Computation only when needed
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              أمثلة الاستخدام
 *                              Usage Examples
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * // مثال 1: مولد بسيط للأرقام
 * // Example 1: Simple number generator
 * دالة* عداد(من: عدد، إلى: عدد) {
 *     لكل ع في مدى(من، إلى) {
 *         أنتج ع؛  // yield v
 *     }
 * }
 * 
 * // الاستخدام:
 * لكل رقم في عداد(1، 5) {
 *     اطبع(رقم)؛  // 1, 2, 3, 4, 5
 * }
 * 
 * // مثال 2: مولد لمتسلسلة فيبوناتشي اللانهائية
 * // Example 2: Infinite Fibonacci generator
 * دالة* فيبوناتشي() {
 *     دع (أ، ب) = (0، 1)؛
 *     بينما صحيح {
 *         أنتج أ؛
 *         (أ، ب) = (ب، أ + ب)؛
 *     }
 * }
 * 
 * // الاستخدام: خذ أول 10 أرقام فقط
 * دع أول_عشرة = فيبوناتشي().خذ(10).اجمع()؛
 * 
 * // مثال 3: معالجة ملف كبير بشكل كسول
 * // Example 3: Lazy processing of large file
 * دالة* اقرأ_أسطر(المسار: نص) {
 *     دع ملف = افتح(المسار)؛
 *     بينما دع سطر = ملف.اقرأ_سطر() {
 *         أنتج سطر؛
 *     }
 * }
 * 
 * // معالجة ملف كبير بدون تحميله كله في الذاكرة
 * لكل سطر في اقرأ_أسطر("ملف_كبير.txt") {
 *     إذا سطر.يبدأ_ب("ERROR") {
 *         اطبع("خطأ: " + سطر)؛
 *     }
 * }
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "../../include/types/generator_type.hpp"
#include <memory>
#include <string>
#include <stdexcept>

namespace Sad {
namespace TypeSystem {

/**
 * المُنشئ: إنشاء نوع مولد جديد
 * Constructor: Create a new Generator type
 * 
 * @param yieldType نوع القيمة التي سينتجها المولد
 *                  The type of value the generator will yield
 * 
 * مثال:
 * - مولد<عدد> -> Generator yielding numbers
 * - مولد<نص> -> Generator yielding strings
 * - مولد<(عدد، نص)> -> Generator yielding tuples
 */
GeneratorType::GeneratorType(TypePtr yieldType) 
    : Type(SadTypeKind::Generator)
    , yieldType_(yieldType)
    , state_(GeneratorState::Created) 
{
    if (!yieldType_) {
        throw std::invalid_argument("GeneratorType: Yield type cannot be null");
    }
}

/**
 * الحصول على نوع القيمة المنتجة
 * Get the yield type
 * 
 * @return نوع القيمة التي سينتجها المولد
 *         The type of value the generator will yield
 */
TypePtr GeneratorType::getYieldType() const {
    return yieldType_;
}

/**
 * الحصول على الحالة الحالية للمولد
 * Get the current state of the generator
 * 
 * @return الحالة (تم إنشاؤه، قيد التشغيل، معلق، أو مكتمل)
 *         The state (Created, Running, Suspended, or Completed)
 */
GeneratorState GeneratorType::getState() const {
    return state_;
}

/**
 * تعيين حالة جديدة للمولد
 * Set a new state for the generator
 * 
 * @param newState الحالة الجديدة
 *                 The new state
 */
void GeneratorType::setState(GeneratorState newState) {
    state_ = newState;
}

/**
 * تحويل النوع إلى نص للعرض
 * Convert type to string for display
 * 
 * @return تمثيل نصي للنوع
 *         String representation of the type
 */
std::string GeneratorType::toString() const {
    return "مولد<" + yieldType_->toString() + ">";
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
bool GeneratorType::equals(const Type* other) const {
    if (!other || other->getKind() != SadTypeKind::Generator) {
        return false;
    }
    
    auto otherGen = static_cast<const GeneratorType*>(other);
    return yieldType_->equals(otherGen->yieldType_.get());
}

/**
 * استنساخ النوع
 * Clone the type
 * 
 * @return نسخة جديدة من النوع
 *         New copy of the type
 */
std::shared_ptr<Type> GeneratorType::clone() const {
    return std::make_shared<GeneratorType>(yieldType_);
}

} // namespace TypeSystem
} // namespace Sad
