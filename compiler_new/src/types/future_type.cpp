/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                      نظام المستقبليات - لغة ص                             ║
 * ║                      Future System - Sad Language                         ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * الملف: future_type.cpp
 * File: future_type.cpp
 * 
 * الوصف: تنفيذ نظام المستقبليات (Futures) والبرمجة غير المتزامنة في لغة ص
 * Description: Implementation of Futures and asynchronous programming system
 * 
 * المهمة: T128 - دعم البرمجة غير المتزامنة و async/await
 * Task: T128 - Async/Await and Asynchronous Programming Support
 * 
 * التاريخ: 26 يناير 2026
 * Date: January 26, 2026
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              المفاهيم الأساسية
 *                              Core Concepts
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * 🌟 ما هو المستقبل (Future)؟
 * 
 *    المستقبل هو قيمة ستكون متاحة في المستقبل، بعد اكتمال عملية غير متزامنة.
 *    إنه يمثل "وعد" بقيمة ستأتي لاحقاً، وليس القيمة نفسها الآن.
 * 
 *    A Future is a value that will be available in the future, after an
 *    asynchronous operation completes. It represents a "promise" of a value
 *    to come, not the value itself right now.
 * 
 * 📋 الحالات الثلاث للمستقبل:
 * 
 *    1️⃣ معلق (Pending): العملية مازالت قيد التنفيذ
 *       Pending: Operation is still in progress
 * 
 *    2️⃣ جاهز (Ready): العملية اكتملت بنجاح وال Valueجاهز
 *       Ready: Operation completed successfully, value is available
 * 
 *    3️⃣ خطأ (Error): العملية فشلت مع خطأ
 *       Error: Operation failed with an error
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              أمثلة الاستخدام
 *                              Usage Examples
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * // مثال 1: دالة غير متزامنة بسيطة
 * // Example 1: Simple async function
 * async دالة احصل_على_بيانات(رابط: نص) -> نص {
 *     دع استجابة = انتظر http.احصل(رابط)؛
 *     دع بيانات = انتظر استجابة.نص()؛
 *     ارجع بيانات؛
 * }
 * 
 * // الاستخدام:
 * دع مستقبل = احصل_على_بيانات("https://api.example.com/data")؛
 * دع بيانات = انتظر مستقبل؛  // يوقف التنفيذ حتى تصل البيانات
 * 
 * // مثال 2: تنفيذ عدة عمليات بالتوازي
 * // Example 2: Running multiple operations in parallel
 * async دالة احصل_على_بيانات_متعددة() {
 *     دع [نتيجة1، نتيجة2، نتيجة3] = انتظر* [
 *         احصل_على_بيانات("رابط1")،
 *         احصل_على_بيانات("رابط2")،
 *         احصل_على_بيانات("رابط3")
 *     ]؛
 *     
 *     ارجع (نتيجة1، نتيجة2، نتيجة3)؛
 * }
 * 
 * // مثال 3: معالجة الأخطاء
 * // Example 3: Error handling
 * async دالة تنزيل_آمن(رابط: نص) -> نتيجة<نص، خطأ> {
 *     حاول {
 *         دع بيانات = انتظر احصل_على_بيانات(رابط)؛
 *         ارجع نجاح(بيانات)؛
 *     } اصطد خطأ {
 *         ارجع فشل(خطأ)؛
 *     }
 * }
 * 
 * // مثال 4: سلسلة من العمليات غير المتزامنة
 * // Example 4: Chain of async operations
 * async دالة معالج_البيانات() {
 *     دع مستخدمون = انتظر احصل_على_مستخدمين()؛
 *     دع تفاصيل = انتظر احصل_على_تفاصيل(مستخدمون)؛
 *     دع معالج = انتظر عالج_التفاصيل(تفاصيل)؛
 *     ارجع معالج؛
 * }
 * 
 * // مثال 5: Future.then() للتسلسل
 * // Example 5: Future.then() for chaining
 * دع نتيجة = احصل_على_بيانات("رابط")
 *     .ثم(بيانات => معالج(بيانات))
 *     .ثم(معالج => احفظ(معالج))
 *     .خطأ(خطأ => اطبع("فشل: " + خطأ))؛
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                           الفوائد الأساسية
 *                           Core Benefits
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * ✅ عدم الحظر (Non-blocking):
 *    لا يتم حظر خيط التنفيذ أثناء انتظار العمليات البطيئة
 *    The execution thread is not blocked while waiting for slow operations
 * 
 * ✅ التوازي (Concurrency):
 *    يمكن تنفيذ عدة عمليات في نفس الوقت
 *    Multiple operations can execute at the same time
 * 
 * ✅ كفاءة الموارد (Resource Efficiency):
 *    استخدام أفضل للموارد المتاحة
 *    Better utilization of available resources
 * 
 * ✅ تجربة مستخدم أفضل (Better UX):
 *    واجهات المستخدم تبقى مستجيبة أثناء العمليات الطويلة
 *    User interfaces remain responsive during long operations
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "../../include/types/future_type.hpp"
#include <memory>
#include <string>
#include <stdexcept>

namespace Sad {
namespace TypeSystem {

/**
 * المُنشئ: إنشاء نوع مستقبل جديد
 * Constructor: Create a new Future type
 * 
 * @param valueType نوع القيمة التي سينتجها المستقبل عند الاكتمال
 *                  The type of value the future will produce when completed
 * 
 * أمثلة:
 * - Future<عدد> -> مستقبل سينتج رقماً
 * - Future<نص> -> مستقبل سينتج نصاً
 * - Future<نتيجة<T، خطأ>> -> مستقبل سينتج نتيجة قد تكون نجاح أو فشل
 */
FutureType::FutureType(TypePtr valueType) 
    : Type(TypeKind::Future)
    , valueType_(valueType)
    , state_(FutureState::Pending) 
{
    if (!valueType_) {
        throw std::invalid_argument("FutureType: Value type cannot be null");
    }
}

/**
 * الحصول على نوع القيمة المنتظرة
 * Get the expected value type
 * 
 * @return نوع القيمة التي سينتجها المستقبل
 *         The type of value the future will produce
 */
TypePtr FutureType::getValueType() const {
    return valueType_;
}

/**
 * الحصول على الحالة الحالية للمستقبل
 * Get the current state of the future
 * 
 * @return الحالة (معلق، جاهز، أو خطأ)
 *         The state (Pending, Ready, or Error)
 */
FutureState FutureType::getState() const {
    return state_;
}

/**
 * تعيين حالة جديدة للمستقبل
 * Set a new state for the future
 * 
 * @param newState الحالة الجديدة
 *                 The new state
 */
void FutureType::setState(FutureState newState) {
    state_ = newState;
}

/**
 * تحويل النوع إلى نص للعرض
 * Convert type to string for display
 * 
 * @return تمثيل نصي للنوع
 *         String representation of the type
 */
std::string FutureType::toString() const {
    std::string stateStr;
    switch (state_) {
        case FutureState::Pending:
            stateStr = "معلق"; // Pending
            break;
        case FutureState::Ready:
            stateStr = "جاهز"; // Ready
            break;
        case FutureState::Error:
            stateStr = "خطأ"; // Error
            break;
        default:
            stateStr = "غير معروف"; // Unknown
    }
    
    return "مستقبل<" + valueType_->toString() + "> [" + stateStr + "]";
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
bool FutureType::equals(const Type* other) const {
    if (!other || other->getKind() != TypeKind::Future) {
        return false;
    }
    
    auto otherFuture = static_cast<const FutureType*>(other);
    return valueType_->equals(otherFuture->valueType_.get());
}

/**
 * استنساخ النوع
 * Clone the type
 * 
 * @return نسخة جديدة من النوع
 *         New copy of the type
 */
std::shared_ptr<Type> FutureType::clone() const {
    return std::make_shared<FutureType>(valueType_);
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
 * القواعد:
 * - Future<T> يمكن تحويله إلى Future<U> إذا كان T يمكن تحويله إلى U
 * - Future<T> يمكن "انتظاره" للحصول على T
 */
bool FutureType::isConvertibleTo(const Type& target) const {
    // يمكن تحويل Future إلى:
    // 1. نفس النوع من Future
    if (equals(&target)) {
        return true;
    }
    
    // 2. Future آخر بنوع قيمة متوافق
    if (target.getKind() == TypeKind::Future) {
        auto targetFuture = static_cast<const FutureType*>(&target);
        return valueType_->equals(targetFuture->valueType_.get());
    }
    
    // 3. في حالة await، يمكن "فك" المستقبل للحصول على القيمة
    // يتم التعامل مع هذا في محلل الأنواع عند رؤية await
    
    return false;
}

/**
 * التحقق من أن المستقبل جاهز
 * Check if future is ready
 * 
 * @return true إذا كان المستقبل في حالة جاهز
 *         true if future is in Ready state
 */
bool FutureType::isReady() const {
    return state_ == FutureState::Ready;
}

/**
 * التحقق من أن المستقبل في حالة خطأ
 * Check if future is in error state
 * 
 * @return true إذا كان المستقبل في حالة خطأ
 *         true if future is in Error state
 */
bool FutureType::isError() const {
    return state_ == FutureState::Error;
}

/**
 * التحقق من أن المستقبل مازال معلقاً
 * Check if future is still pending
 * 
 * @return true إذا كان المستقبل مازال معلقاً
 *         true if future is still pending
 */
bool FutureType::isPending() const {
    return state_ == FutureState::Pending;
}

} // namespace TypeSystem
} // namespace Sad
