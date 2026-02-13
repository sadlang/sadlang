// ===================================================================
// أنواع المؤشرات الخام - لغة ص
// pointer_types.cpp - تنفيذ أنواع المؤشرات (*ثابت T و *متغير T)
// ===================================================================
// يُنفذ نظام المؤشرات الخام (Raw Pointers) في لغة ص:
// - *ثابت T: مؤشر للقراءة فقط
// - *متغير T: مؤشر للقراءة والكتابة
// المؤشرات الخام تتطلب كتلة غير_آمن {} للاستخدام
// ===================================================================

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_set>
#include <cassert>

namespace sad {
namespace types {

/// نوع ثبات المؤشر
enum class PointerMutability {
    Const,    // *ثابت T
    Mutable,  // *متغير T  
};

/// نوع مؤشر خام في لغة ص
class RawPointerType {
public:
    /// إنشاء نوع مؤشر خام
    RawPointerType(std::shared_ptr<class Type> pointee_type,
                   PointerMutability mutability)
        : pointee_type_(std::move(pointee_type))
        , mutability_(mutability) {}
    
    /// النوع المُشار إليه
    const std::shared_ptr<class Type>& pointeeType() const { return pointee_type_; }
    
    /// هل المؤشر متغير؟
    bool isMutable() const { return mutability_ == PointerMutability::Mutable; }
    
    /// هل المؤشر ثابت؟
    bool isConst() const { return mutability_ == PointerMutability::Const; }
    
    /// مستوى الثبات
    PointerMutability mutability() const { return mutability_; }
    
    /// التمثيل النصي العربي
    std::string toArabicString() const {
        std::string result = "*";
        if (mutability_ == PointerMutability::Const) {
            result += "ثابت ";
        } else {
            result += "متغير ";
        }
        result += "<نوع>";
        return result;
    }
    
    /// هل يمكن تحويل هذا المؤشر إلى آخر؟
    bool canCastTo(const RawPointerType& target) const {
        // *متغير يمكن تحويله إلى *ثابت
        if (mutability_ == PointerMutability::Mutable &&
            target.mutability_ == PointerMutability::Const) {
            return true;
        }
        // نفس مستوى الثبات
        return mutability_ == target.mutability_;
    }
    
    /// هل يمكن عمل حساب مؤشرات؟
    bool supportsArithmetic() const {
        return true; // المؤشرات الخام تدعم الحساب في كتلة غير_آمن
    }

private:
    std::shared_ptr<class Type> pointee_type_;  // النوع المُشار إليه
    PointerMutability mutability_;               // ثابت أو متغير
};

/// عمليات المؤشرات الخام (تتطلب غير_آمن)
class RawPointerOps {
public:
    /// إلغاء مرجعية مؤشر (*مؤشر)
    struct DerefResult {
        bool requires_unsafe;     // يتطلب كتلة غير_آمن
        bool is_valid;            // هل العملية صالحة
        std::string error_ar;     // رسالة خطأ عربية
    };
    
    /// التحقق من صلاحية إلغاء المرجعية
    static DerefResult checkDeref(const RawPointerType& ptr, bool in_unsafe_block) {
        DerefResult result;
        result.requires_unsafe = true;
        
        if (!in_unsafe_block) {
            result.is_valid = false;
            result.error_ar = "خطأ[ص0040]: لا يمكن إلغاء مرجعية مؤشر خام خارج كتلة غير_آمن {}";
            return result;
        }
        
        result.is_valid = true;
        return result;
    }
    
    /// التحقق من حساب المؤشرات
    static DerefResult checkArithmetic(const RawPointerType& ptr, 
                                        const std::string& op,
                                        bool in_unsafe_block) {
        DerefResult result;
        result.requires_unsafe = true;
        
        if (!in_unsafe_block) {
            result.is_valid = false;
            result.error_ar = "خطأ[ص0041]: لا يمكن إجراء حساب مؤشرات (" + op + 
                             ") خارج كتلة غير_آمن {}";
            return result;
        }
        
        result.is_valid = true;
        return result;
    }
    
    /// تحويل مرجع إلى مؤشر خام
    static RawPointerType fromReference(const class ReferenceType& ref) {
        auto mutability = ref.isMutable() ? PointerMutability::Mutable 
                                           : PointerMutability::Const;
        return RawPointerType(ref.innerType(), mutability);
    }
    
    /// التحقق من تحويل مؤشر خام إلى مرجع آمن
    static DerefResult checkPointerToRef(const RawPointerType& ptr, 
                                          bool in_unsafe_block) {
        DerefResult result;
        result.requires_unsafe = true;
        
        if (!in_unsafe_block) {
            result.is_valid = false;
            result.error_ar = "خطأ[ص0042]: لا يمكن تحويل مؤشر خام إلى مرجع آمن "
                             "خارج كتلة غير_آمن {}";
            return result;
        }
        
        result.is_valid = true;
        return result;
    }
};

/// فاحص كتل غير_آمن
class UnsafeBlockChecker {
public:
    /// تتبع كتل غير_آمن المفتوحة
    void enterUnsafeBlock() { unsafe_depth_++; }
    void exitUnsafeBlock() { 
        assert(unsafe_depth_ > 0);
        unsafe_depth_--; 
    }
    
    /// هل نحن داخل كتلة غير_آمن؟
    bool isInUnsafeBlock() const { return unsafe_depth_ > 0; }
    
    /// التحقق من عملية تتطلب غير_آمن
    struct UnsafeCheckResult {
        bool allowed;
        std::string error_ar;
    };
    
    /// التحقق من عملية مؤشر خام
    UnsafeCheckResult checkRawPointerOp(const std::string& operation) const {
        if (isInUnsafeBlock()) {
            return {true, ""};
        }
        return {false, "خطأ[ص0043]: العملية '" + operation + 
                       "' على مؤشر خام تتطلب كتلة غير_آمن {}"};
    }
    
    /// التحقق من استدعاء دالة خارجية (FFI)
    UnsafeCheckResult checkFfiCall(const std::string& function_name) const {
        if (isInUnsafeBlock()) {
            return {true, ""};
        }
        return {false, "خطأ[ص0044]: استدعاء الدالة الخارجية '" + function_name + 
                       "' يتطلب كتلة غير_آمن {}"};
    }
    
    /// التحقق من الوصول المباشر للذاكرة
    UnsafeCheckResult checkDirectMemoryAccess(const std::string& address) const {
        if (isInUnsafeBlock()) {
            return {true, ""};
        }
        return {false, "خطأ[ص0045]: الوصول المباشر للذاكرة عند " + address + 
                       " يتطلب كتلة غير_آمن {}"};
    }

    /// تحذير عند استخدام غير_آمن بدون سبب
    struct UnsafeWarning {
        std::string message_ar;
        int line;
    };
    
    /// فحص كتلة غير_آمن فارغة (لا تحتوي عمليات خطرة)
    std::optional<UnsafeWarning> checkUnnecessaryUnsafe(
        bool has_unsafe_ops, int block_line) const 
    {
        if (!has_unsafe_ops) {
            return UnsafeWarning{
                "تحذير[ص0046]: كتلة غير_آمن {} لا تحتوي عمليات خطرة - "
                "يمكن إزالتها",
                block_line
            };
        }
        return std::nullopt;
    }

private:
    int unsafe_depth_ = 0;
};

/// حساب المؤشرات
class PointerArithmetic {
public:
    /// إزاحة مؤشر (ptr + n)
    struct OffsetResult {
        bool valid;
        std::string error_ar;
    };
    
    /// التحقق من صلاحية الإزاحة
    static OffsetResult checkOffset(int64_t offset, size_t array_size) {
        if (offset < 0 || static_cast<size_t>(offset) >= array_size) {
            return {false, "خطأ[ص0047]: إزاحة المؤشر " + 
                          std::to_string(offset) + " خارج النطاق [0, " + 
                          std::to_string(array_size) + ")"};
        }
        return {true, ""};
    }
    
    /// التحقق من محاذاة المؤشر
    static bool checkAlignment(uintptr_t address, size_t alignment) {
        return (address % alignment) == 0;
    }
    
    /// رسالة خطأ للمحاذاة
    static std::string alignmentError(uintptr_t address, size_t required) {
        return "خطأ[ص0048]: العنوان " + std::to_string(address) + 
               " غير محاذى للمحاذاة المطلوبة " + std::to_string(required);
    }
};

} // namespace types
} // namespace sad
