// ===================================================================
// أنواع المراجع - لغة ص
// reference_types.cpp - تنفيذ أنواع المراجع (&ثابت و &متغير)
// ===================================================================
// يُنفذ نظام المراجع الآمنة في لغة ص:
// - &T (مرجع ثابت): قراءة فقط، عدة مراجع مسموحة
// - &متغير T (مرجع متغير): قراءة وكتابة، مرجع واحد فقط
// ===================================================================

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>

namespace sad {
namespace types {

/// مستوى الثبات للمرجع
enum class Mutability {
    Immutable,  // &T - ثابت
    Mutable,    // &متغير T - متغير
};

/// دورة حياة المرجع
struct Lifetime {
    std::string name;        // اسم دورة الحياة (مثل 'أ)
    int scope_depth;         // عمق النطاق
    bool is_static;          // هل هو ساكن ('ساكن)
    bool is_anonymous;       // هل هو مجهول الاسم
    
    /// إنشاء دورة حياة ساكنة
    static Lifetime static_lifetime() {
        return Lifetime{"ساكن", 0, true, false};
    }
    
    /// إنشاء دورة حياة مجهولة
    static Lifetime anonymous(int depth) {
        return Lifetime{"_", depth, false, true};
    }
    
    /// هل هذه الدورة تعيش أطول من الأخرى؟
    bool outlives(const Lifetime& other) const {
        if (is_static) return true;
        if (other.is_static) return false;
        return scope_depth <= other.scope_depth;
    }
};

/// نوع مرجع في لغة ص
class ReferenceType {
public:
    /// إنشاء نوع مرجع
    ReferenceType(std::shared_ptr<class Type> inner_type,
                  Mutability mutability,
                  Lifetime lifetime)
        : inner_type_(std::move(inner_type))
        , mutability_(mutability)
        , lifetime_(std::move(lifetime)) {}
    
    /// النوع الداخلي المُشار إليه
    const std::shared_ptr<class Type>& innerType() const { return inner_type_; }
    
    /// هل المرجع متغير؟
    bool isMutable() const { return mutability_ == Mutability::Mutable; }
    
    /// هل المرجع ثابت؟
    bool isImmutable() const { return mutability_ == Mutability::Immutable; }
    
    /// مستوى الثبات
    Mutability mutability() const { return mutability_; }
    
    /// دورة حياة المرجع
    const Lifetime& lifetime() const { return lifetime_; }
    
    /// التمثيل النصي العربي
    std::string toArabicString() const {
        std::string result = "&";
        if (mutability_ == Mutability::Mutable) {
            result += "متغير ";
        }
        if (!lifetime_.is_anonymous) {
            result += "'" + lifetime_.name + " ";
        }
        // سيتم إضافة اسم النوع الداخلي
        result += "<نوع_داخلي>";
        return result;
    }
    
    /// هل يمكن تحويل هذا المرجع إلى مرجع آخر؟
    bool canCoerceTo(const ReferenceType& target) const {
        // مرجع متغير يمكن تحويله إلى ثابت
        if (mutability_ == Mutability::Mutable && 
            target.mutability_ == Mutability::Immutable) {
            return lifetime_.outlives(target.lifetime_);
        }
        // نفس مستوى الثبات
        if (mutability_ == target.mutability_) {
            return lifetime_.outlives(target.lifetime_);
        }
        // لا يمكن تحويل ثابت إلى متغير
        return false;
    }

private:
    std::shared_ptr<class Type> inner_type_;  // النوع المُشار إليه
    Mutability mutability_;                    // ثابت أو متغير
    Lifetime lifetime_;                        // دورة الحياة
};

/// فاحص صلاحية المراجع
class ReferenceValidator {
public:
    /// قاعدة المرجع الواحد المتغير
    /// في أي لحظة، يمكن وجود إما:
    /// - مرجع واحد متغير (&متغير)
    /// - أو عدة مراجع ثابتة (&ثابت)
    struct BorrowState {
        int immutable_count = 0;    // عدد المراجع الثابتة
        bool has_mutable = false;    // هل يوجد مرجع متغير
        std::string variable_name;   // اسم المتغير
    };
    
    /// التحقق من إمكانية إنشاء مرجع جديد
    bool canBorrow(const std::string& variable, Mutability mutability) const {
        auto it = borrow_states_.find(variable);
        if (it == borrow_states_.end()) {
            return true; // لا يوجد أي مرجع حالياً
        }
        
        const auto& state = it->second;
        
        if (mutability == Mutability::Mutable) {
            // مرجع متغير: يجب ألا يوجد أي مرجع آخر
            return state.immutable_count == 0 && !state.has_mutable;
        } else {
            // مرجع ثابت: يجب ألا يوجد مرجع متغير
            return !state.has_mutable;
        }
    }
    
    /// تسجيل استعارة جديدة
    bool addBorrow(const std::string& variable, Mutability mutability) {
        if (!canBorrow(variable, mutability)) {
            return false;
        }
        
        auto& state = borrow_states_[variable];
        state.variable_name = variable;
        
        if (mutability == Mutability::Mutable) {
            state.has_mutable = true;
        } else {
            state.immutable_count++;
        }
        
        return true;
    }
    
    /// إنهاء استعارة
    void releaseBorrow(const std::string& variable, Mutability mutability) {
        auto it = borrow_states_.find(variable);
        if (it == borrow_states_.end()) return;
        
        auto& state = it->second;
        if (mutability == Mutability::Mutable) {
            state.has_mutable = false;
        } else {
            state.immutable_count = std::max(0, state.immutable_count - 1);
        }
        
        // تنظيف إذا لم يعد هناك أي مراجع
        if (state.immutable_count == 0 && !state.has_mutable) {
            borrow_states_.erase(it);
        }
    }
    
    /// رسالة خطأ عربية للاستعارة المرفوضة
    std::string getArabicErrorMessage(const std::string& variable, 
                                       Mutability requested) const {
        auto it = borrow_states_.find(variable);
        if (it == borrow_states_.end()) return "";
        
        const auto& state = it->second;
        
        if (requested == Mutability::Mutable) {
            if (state.has_mutable) {
                return "خطأ[ص0020]: لا يمكن استعارة '" + variable + 
                       "' كمتغير لأنه مُستعار بالفعل كمتغير";
            }
            if (state.immutable_count > 0) {
                return "خطأ[ص0021]: لا يمكن استعارة '" + variable + 
                       "' كمتغير لأنه مُستعار كثابت (" + 
                       std::to_string(state.immutable_count) + " مرجع)";
            }
        } else {
            if (state.has_mutable) {
                return "خطأ[ص0022]: لا يمكن استعارة '" + variable + 
                       "' كثابت لأنه مُستعار بالفعل كمتغير";
            }
        }
        
        return "";
    }

private:
    std::unordered_map<std::string, BorrowState> borrow_states_;
};

/// فاحص دورات الحياة
class LifetimeChecker {
public:
    /// التحقق من أن المرجع لا يعيش أطول من المُشار إليه
    struct LifetimeError {
        std::string message_ar;  // رسالة عربية
        std::string variable;
        Lifetime reference_lifetime;
        Lifetime value_lifetime;
    };
    
    /// التحقق من قيد دورة الحياة
    std::optional<LifetimeError> checkLifetimeConstraint(
        const std::string& ref_name,
        const Lifetime& ref_lifetime,
        const std::string& val_name,
        const Lifetime& val_lifetime) 
    {
        if (!val_lifetime.outlives(ref_lifetime)) {
            LifetimeError error;
            error.variable = val_name;
            error.reference_lifetime = ref_lifetime;
            error.value_lifetime = val_lifetime;
            error.message_ar = "خطأ[ص0030]: المرجع '" + ref_name + 
                              "' يعيش أطول من القيمة '" + val_name + "'";
            return error;
        }
        return std::nullopt;
    }
    
    /// التحقق من إرجاع مرجع لمتغير محلي
    std::optional<LifetimeError> checkDanglingReference(
        const std::string& ref_name,
        const Lifetime& ref_lifetime,
        int current_scope_depth)
    {
        if (ref_lifetime.scope_depth >= current_scope_depth && !ref_lifetime.is_static) {
            LifetimeError error;
            error.variable = ref_name;
            error.reference_lifetime = ref_lifetime;
            error.message_ar = "خطأ[ص0031]: لا يمكن إرجاع مرجع لمتغير محلي '" + 
                              ref_name + "'";
            return error;
        }
        return std::nullopt;
    }
};

} // namespace types
} // namespace sad
