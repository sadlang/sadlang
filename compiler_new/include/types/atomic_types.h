// ======================================================================
// atomic_types.h - أنواع العمليات الذرية / Atomic Types
// ======================================================================
// الوصف بالعربية:
//   تعريف أنواع العمليات الذرية لنظام النحلة (BeeOS)
//   يدعم عمليات القراءة/الكتابة الذرية مع ترتيبات الذاكرة
//
// English Description:
//   Atomic type definitions for BeeOS
//   Supports atomic read/write with memory orderings
// ======================================================================

#ifndef SAD_ATOMIC_TYPES_H
#define SAD_ATOMIC_TYPES_H

#include <string>
#include <vector>

namespace Sad {
namespace Types {

/**
 * @brief Memory ordering / ترتيب الذاكرة
 * 
 * (AR) ترتيبات الذاكرة المدعومة — مطابقة لـ C++ std::memory_order
 * (EN) Supported memory orderings — matching C++ std::memory_order
 */
enum class MemoryOrdering {
    Relaxed,        ///< مرتخي / relaxed — no ordering guarantees
    Acquire,        ///< اكتساب / acquire — reads after this see writes before release
    Release,        ///< إطلاق / release — writes before this are visible after acquire
    AcqRel,         ///< اكتساب_إطلاق / acq_rel — both acquire and release
    SeqCst          ///< تسلسلي / seq_cst — sequential consistency (default)
};

/**
 * @brief Atomic operation kind / نوع العملية الذرية
 */
enum class AtomicOp {
    Load,           ///< حمّل / load — atomic read
    Store,          ///< خزّن / store — atomic write
    Exchange,       ///< بدّل / exchange — atomic swap
    CompareExchange,///< قارن_وبدّل / compare_exchange — CAS
    FetchAdd,       ///< جلب_وأضف / fetch_add — atomic add
    FetchSub,       ///< جلب_واطرح / fetch_sub — atomic subtract
    FetchAnd,       ///< جلب_و_و / fetch_and — atomic AND
    FetchOr,        ///< جلب_و_أو / fetch_or — atomic OR
    FetchXor,       ///< جلب_و_حصري / fetch_xor — atomic XOR
    FetchMin,       ///< جلب_والأصغر / fetch_min — atomic min
    FetchMax        ///< جلب_والأكبر / fetch_max — atomic max
};

/**
 * @brief Atomic type wrapper / غلاف النوع الذرّي
 * 
 * (AR) يمثل نوع ذرّي<ن> حيث ن هو النوع الأساسي
 * (EN) Represents atomic<T> where T is the base type
 * 
 * @example أمثلة:
 * - ذرّي<رقم> عداد = ذرّي::جديد(0)
 * - عداد.أضف(1، ترتيب::تسلسلي)
 * - دع قيمة = عداد.حمّل(ترتيب::اكتساب)
 */
class AtomicType {
public:
    std::string baseTypeName;   ///< Base type name / اسم النوع الأساسي (e.g., "رقم", "ط64")
    
    AtomicType() = default;
    AtomicType(const std::string& base) : baseTypeName(base) {}
    
    /**
     * @brief Get the Arabic name / الحصول على الاسم العربي
     */
    std::string getArabicName() const {
        return "ذرّي<" + baseTypeName + ">";
    }
    
    /**
     * @brief Check if base type is valid for atomic operations
     * (AR) التحقق من أن النوع الأساسي يصلح للعمليات الذرية
     * (EN) Valid types: integers (ط8-ط64, ص8-ص64), pointers, bool
     */
    bool isValidBaseType() const {
        // (AR) الأنواع الصالحة: أعداد صحيحة، مؤشرات، منطقي
        static const std::vector<std::string> validTypes = {
            "ط8", "ط16", "ط32", "ط64", "ص8", "ص16", "ص32", "ص64",
            "u8", "u16", "u32", "u64", "i8", "i16", "i32", "i64",
            "رقم", "منطقي", "مؤشر",
            "int", "bool", "ptr",
            "ط_حجم", "ص_حجم", "usize", "isize"
        };
        for (const auto& t : validTypes) {
            if (baseTypeName == t) return true;
        }
        return false;
    }
};

/**
 * @brief Memory fence / سياج الذاكرة
 * 
 * (AR) سياج ذاكرة يفرض ترتيب العمليات
 * (EN) Memory fence that enforces operation ordering
 * 
 * @example:
 * - سياج_ذاكرة(ترتيب::تسلسلي)
 * - memory_fence(ordering::seq_cst)
 */
struct MemoryFence {
    MemoryOrdering ordering;
    
    MemoryFence(MemoryOrdering ord = MemoryOrdering::SeqCst)
        : ordering(ord) {}
};

/**
 * @brief Convert Arabic ordering name to enum
 * (AR) تحويل اسم الترتيب العربي إلى enum
 */
inline MemoryOrdering orderingFromArabic(const std::string& name) {
    if (name == "مرتخي" || name == "relaxed") return MemoryOrdering::Relaxed;
    if (name == "اكتساب" || name == "acquire") return MemoryOrdering::Acquire;
    if (name == "إطلاق" || name == "release") return MemoryOrdering::Release;
    if (name == "اكتساب_إطلاق" || name == "acq_rel") return MemoryOrdering::AcqRel;
    if (name == "تسلسلي" || name == "seq_cst") return MemoryOrdering::SeqCst;
    return MemoryOrdering::SeqCst; // (AR) الافتراضي: التسلسلي (الأكثر أماناً)
}

/**
 * @brief Convert ordering enum to LLVM AtomicOrdering string
 */
inline std::string orderingToLLVM(MemoryOrdering ord) {
    switch (ord) {
        case MemoryOrdering::Relaxed: return "monotonic";
        case MemoryOrdering::Acquire: return "acquire";
        case MemoryOrdering::Release: return "release";
        case MemoryOrdering::AcqRel: return "acq_rel";
        case MemoryOrdering::SeqCst: return "seq_cst";
    }
    return "seq_cst";
}

} // namespace Types
} // namespace Sad

#endif // SAD_ATOMIC_TYPES_H
