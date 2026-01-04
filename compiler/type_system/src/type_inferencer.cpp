// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_inferencer.cpp
// File: type_inferencer.cpp
//
// الوصف: تنفيذ محرك استنتاج الأنواع - Hindley-Milner Algorithm W
// Description: Implementation of type inference engine - Hindley-Milner Algorithm W
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المرجع: Hindley-Milner Type Inference
// Reference: Damas & Milner, "Principal type-schemes for functional programs" (1982)
//            Algorithm W - Wand's type inference algorithm
// ════════════════════════════════════════════════════════════════════════════════

#include "type_inferencer.h"    // المصدر: type_inferencer.h:1-350 / Source: type_inferencer.h:1-350
#include "primitive_type.h"      // المصدر: primitive_type.h:1-100 / Source: primitive_type.h:1-100
#include "type_registry.h"       // المصدر: type_registry.h:1-end / Source: type_registry.h:1-end
#include "type_variable.h"       // المصدر: type_variable.h:1-186 / Source: type_variable.h:1-186
#include <sstream>               // لـ ostringstream / For ostringstream
#include <algorithm>             // لـ std::find / For std::find
#include <set>                   // لـ std::set / For std::set
#include <iostream>              // لـ std::cout / For std::cout

namespace Sad {

// Forward declaration لـ AST nodes / Forward declaration for AST nodes
// سنستخدم Expression و Statement من namespace Sad::AST
// We'll use Expression and Statement from namespace Sad::AST
// لكن حالياً سنعمل بالمؤشرات الخام / But currently we'll work with raw pointers

namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ TypeEnvironment - بيئة الأنواع
// TypeEnvironment Implementation - Type Environment
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ الافتراضي / Default constructor
// المصدر: type_inferencer.h:44 / Source: type_inferencer.h:44
TypeEnvironment::TypeEnvironment() 
    : parent_(nullptr) {}  // لا يوجد أب / No parent

// المنشئ مع أب / Constructor with parent
// المصدر: type_inferencer.h:48 / Source: type_inferencer.h:48
TypeEnvironment::TypeEnvironment(std::shared_ptr<TypeEnvironment> parent)
    : parent_(parent) {}  // تخزين المؤشر للأب / Store pointer to parent

// ربط اسم بنوع / Bind name to type
// المصدر: type_inferencer.h:53 / Source: type_inferencer.h:53
void TypeEnvironment::bind(const std::string& name, TypePtr type) {
    if (name.empty()) {
        // اسم فارغ غير مسموح / Empty name not allowed
        return;
    }
    
    if (!type) {
        // نوع null غير مسموح / null type not allowed
        return;
    }
    
    bindings_[name] = type;  // إضافة/تحديث الربط / Add/update binding
}

// البحث عن نوع باسمه / Lookup type by name
// المصدر: type_inferencer.h:58 / Source: type_inferencer.h:58
TypePtr TypeEnvironment::lookup(const std::string& name) const {
    // البحث في البيئة الحالية / Search in current environment
    auto it = bindings_.find(name);
    if (it != bindings_.end()) {
        return it->second;  // تم العثور / Found
    }
    
    // البحث في البيئة الأب / Search in parent environment
    if (parent_) {
        return parent_->lookup(name);  // بحث تكراري / Recursive search
    }
    
    return nullptr;  // لم يتم العثور / Not found
}

// التحقق من وجود اسم / Check if name exists
// المصدر: type_inferencer.h:63 / Source: type_inferencer.h:63
bool TypeEnvironment::contains(const std::string& name) const {
    return lookup(name) != nullptr;  // يوجد إذا لم يكن null / Exists if not null
}

// إزالة ربط / Remove binding
// المصدر: type_inferencer.h:68 / Source: type_inferencer.h:68
void TypeEnvironment::unbind(const std::string& name) {
    bindings_.erase(name);  // حذف من الخريطة / Erase from map
}

// إنشاء بيئة فرعية / Create child environment
// المصدر: type_inferencer.h:73 / Source: type_inferencer.h:73
std::shared_ptr<TypeEnvironment> TypeEnvironment::createChild() {
    // ملاحظة: لا يمكن استخدام shared_from_this لأن TypeEnvironment لا ترث من enable_shared_from_this
    // Note: Cannot use shared_from_this as TypeEnvironment doesn't inherit from enable_shared_from_this
    // TODO: في التنفيذ الكامل، يجب أن ترث TypeEnvironment من std::enable_shared_from_this<TypeEnvironment>
    // TODO: In full implementation, TypeEnvironment should inherit from std::enable_shared_from_this<TypeEnvironment>
    return std::make_shared<TypeEnvironment>();  // إنشاء بيئة جديدة / Create new environment
}

// تنظيف جميع الروابط / Clear all bindings
// المصدر: type_inferencer.h:78 / Source: type_inferencer.h:78
void TypeEnvironment::clear() {
    bindings_.clear();  // مسح الخريطة / Clear map
}

// الحصول على جميع الأسماء / Get all names
// المصدر: type_inferencer.h:83 / Source: type_inferencer.h:83
std::vector<std::string> TypeEnvironment::getAllNames() const {
    std::vector<std::string> names;  // قائمة الأسماء / List of names
    
    // إضافة الأسماء من البيئة الحالية / Add names from current environment
    for (const auto& [name, type] : bindings_) {
        (void)type;  // تجنب تحذير / Avoid warning
        names.push_back(name);  // إضافة الاسم / Add name
    }
    
    return names;  // إرجاع القائمة / Return list
}



// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ InferenceContext - سياق الاستنتاج
// InferenceContext Implementation - Inference Context
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ / Constructor
// المصدر: type_inferencer.h:111 / Source: type_inferencer.h:111
InferenceContext::InferenceContext(std::shared_ptr<TypeEnvironment> env)
    : environment_(env),      // تخزين البيئة / Store environment
      typeVarCounter_(0) {}   // عداد المتغيرات = 0 / Variable counter = 0







// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ InferenceResult - نتيجة الاستنتاج
// InferenceResult Implementation - Inference Result
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء نتيجة ناجحة / Create success result
// المصدر: type_inferencer.h:166 / Source: type_inferencer.h:166
InferenceResult InferenceResult::makeSuccess(TypePtr type, const Substitution& subst) {
    InferenceResult result;      // إنشاء نتيجة جديدة / Create new result
    result.success = true;       // تعيين النجاح / Set success
    result.type = type;          // تخزين النوع / Store type
    result.substitution = subst; // نسخ الاستبدال / Copy substitution
    return result;               // إرجاع النتيجة / Return result
}

// إنشاء نتيجة فاشلة / Create failure result
// المصدر: type_inferencer.h:169 / Source: type_inferencer.h:169
InferenceResult InferenceResult::makeFailure(const std::string& errorMsg) {
    InferenceResult result;       // إنشاء نتيجة جديدة / Create new result
    result.success = false;       // تعيين الفشل / Set failure
    result.errorMessage = errorMsg; // نسخ رسالة الخطأ / Copy error message
    return result;                // إرجاع النتيجة / Return result
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ TypeInferencer - المحرك الرئيسي
// TypeInferencer Implementation - Main Engine
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ / Constructor
// المصدر: type_inferencer.h:184 / Source: type_inferencer.h:184
TypeInferencer::TypeInferencer()
    : globalEnv_(createInitialEnvironment()),  // إنشاء البيئة العامة / Create global environment
      verbose_(false),                         // عدم طباعة التفاصيل / Don't print details
      inferenceCount_(0) {}                    // عداد الاستنتاجات = 0 / Inference counter = 0

// استنتاج نوع تعبير بسيط / Simple infer expression type
// المصدر: type_inferencer.h:197 / Source: type_inferencer.h:197
InferenceResult TypeInferencer::inferExpression(const Sad::Expression* expr) {
    return inferExpression(expr, globalEnv_);  // استدعاء مع البيئة العامة / Call with global environment
}

// استنتاج نوع تعبير مع بيئة - المحرك الرئيسي
// Infer expression type with environment - Main engine
// المصدر: type_inferencer.h:202 / Source: type_inferencer.h:202
InferenceResult TypeInferencer::inferExpression(const Sad::Expression* expr,
                                               std::shared_ptr<TypeEnvironment> env) {
    // === الخطوة 1: التحقق من صحة المدخلات / Step 1: Validate inputs ===
    if (!expr) {
        return InferenceResult::makeFailure(
            "تعبير null / null expression"
        );
    }
    
    if (!env) {
        return InferenceResult::makeFailure(
            "بيئة null / null environment"
        );
    }
    
    inferenceCount_++;  // زيادة عداد الاستنتاجات / Increment inference counter
    
    if (verbose_) {
        std::cout << "\n=== استنتاج نوع التعبير / Inferring expression type ===" << std::endl;
    }
    
    // === الخطوة 2: إنشاء سياق الاستنتاج / Step 2: Create inference context ===
    // المصدر: type_inferencer.h:111 / Source: type_inferencer.h:111
    InferenceContext ctx(env);
    
    // === الخطوة 3: استنتاج النوع / Step 3: Infer type ===
    TypePtr inferredType = inferExpressionInternal(expr, ctx);
    
    if (!inferredType) {
        return InferenceResult::makeFailure(
            "فشل استنتاج النوع / Failed to infer type"
        );
    }
    
    // === الخطوة 4: حل القيود / Step 4: Solve constraints ===
    // المصدر: constraint_solver.h:149 / Source: constraint_solver.h:149
    SolverResult solverResult = solver_.solve(ctx.getConstraints());
    
    if (!solverResult.success) {
        // فشل حل القيود / Constraint solving failed
        std::ostringstream oss;
        oss << "فشل حل القيود / Failed to solve constraints: ";
        // المصدر: constraint_solver.h:75 / Source: constraint_solver.h:75
        oss << solverResult.error.getMessage();
        return InferenceResult::makeFailure(oss.str());
    }
    
    // === الخطوة 5: تطبيق الاستبدال على النوع المُستنتَج / Step 5: Apply substitution to inferred type ===
    // المصدر: substitution.h:81 / Source: substitution.h:81
    TypePtr finalType = solverResult.substitution.apply(inferredType);
    
    if (verbose_) {
        std::cout << "✓ استنتاج ناجح / Successful inference" << std::endl;
        std::cout << "  النوع / Type: " << finalType->toString() << std::endl;  // المصدر: type.h:104 / Source: type.h:104
    }
    
    // === النجاح / Success ===
    return InferenceResult::makeSuccess(finalType, solverResult.substitution);
}

// استنتاج نوع دالة / Infer function type
// المصدر: type_inferencer.h:207 / Source: type_inferencer.h:207
InferenceResult TypeInferencer::inferFunction(const Sad::FunctionDeclaration* func) {
    (void)func;  // تجنب تحذير / Avoid warning
    
    // TODO: في التنفيذ الكامل، سنحلل الدوال بالكامل
    // TODO: In full implementation, we'll fully analyze functions
    // - استنتاج أنواع المعاملات / Infer parameter types
    // - استنتاج نوع الإرجاع / Infer return type
    // - استنتاج أنواع المتغيرات المحلية / Infer local variable types
    // - التحقق من توافق الإرجاع / Check return compatibility
    
    // حالياً، نرجع نوع دالة عام / Currently, return generic function type
    TypeRegistry& registry = TypeRegistry::getInstance();
    return InferenceResult::makeSuccess(registry.getUnknownType(), Substitution());
}

// استنتاج نوع متغير / Infer variable type
// المصدر: type_inferencer.h:212 / Source: type_inferencer.h:212
InferenceResult TypeInferencer::inferVariable(const Sad::VariableDeclaration* var) {
    (void)var;  // تجنب تحذير / Avoid warning
    
    // TODO: في التنفيذ الكامل، سنحلل المتغيرات بالكامل
    // TODO: In full implementation, we'll fully analyze variables
    // - استنتاج النوع من القيمة المبدئية / Infer type from initial value
    // - التحقق من تطابق النوع المُصرَّح / Check declared type compatibility
    
    // حالياً، نرجع نوع عام / Currently, return generic type
    TypeRegistry& registry = TypeRegistry::getInstance();
    return InferenceResult::makeSuccess(registry.getUnknownType(), Substitution());
}

// استنتاج النوع الداخلي - Algorithm W
// Internal type inference - Algorithm W
// المصدر: type_inferencer.h:230 / Source: type_inferencer.h:230
TypePtr TypeInferencer::inferExpressionInternal(const Sad::Expression* expr,
                                                InferenceContext& ctx) {
    if (!expr) {
        return nullptr;  // تعبير null / null expression
    }
    
    // === استنتاج بناءً على نوع التعبير / Infer based on expression type ===
    // TODO: في التنفيذ الكامل، سنضيف جميع أنواع التعابير
    // TODO: In full implementation, we'll add all expression types
    
    // حالياً، نرجع متغير نوع جديد لأي تعبير
    // Currently, return fresh type variable for any expression
    return ctx.freshTypeVar();  // المصدر: type_inferencer.h:136 / Source: type_inferencer.h:136
}



// تعميم النوع (إضافة forall) / Generalize type (add forall)
// المصدر: type_inferencer.h:253 / Source: type_inferencer.h:253
TypePtr TypeInferencer::generalize(TypePtr type, const TypeEnvironment& env) {
    (void)env;  // تجنب تحذير / Avoid warning
    
    if (!type) {
        return nullptr;  // نوع null / null type
    }
    
    // TODO: في التنفيذ الكامل / In full implementation
    // حالياً، نرجع النوع كما هو / Currently, return type as is
    return type;
}

// تخصيص النوع (إزالة forall واستبدال بمتغيرات جديدة) / Instantiate type (remove forall and substitute with fresh variables)
// المصدر: type_inferencer.h:258 / Source: type_inferencer.h:258
TypePtr TypeInferencer::instantiate(TypePtr type) {
    if (!type) {
        return nullptr;  // نوع null / null type
    }
    
    // === في التنفيذ الكامل، سنفكك نوع Forall
    // In full implementation, we'll unwrap Forall type ===
    // TODO: إذا كان type هو ForallType(vars, innerType):
    // TODO: If type is ForallType(vars, innerType):
    //   1. إنشاء متغيرات جديدة لكل var / Create fresh variables for each var
    //   2. إنشاء استبدال من vars إلى المتغيرات الجديدة / Create substitution from vars to fresh variables
    //   3. تطبيق الاستبدال على innerType / Apply substitution to innerType
    //   4. إرجاع النوع الناتج / Return resulting type
    
    // حالياً، نرجع النوع كما هو / Currently, return type as is
    return type;
}



// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// استنتاج نوع تعبير بسيط / Simple infer expression type
// المصدر: type_inferencer.h:318 / Source: type_inferencer.h:318
InferenceResult inferType(const Sad::Expression* expr) {
    TypeInferencer inferencer;  // إنشاء محرك استنتاج / Create inference engine
    return inferencer.inferExpression(expr);  // استنتاج النوع / Infer type
}

// إنشاء البيئة المبدئية / Create initial environment
// المصدر: type_inferencer.h:323 / Source: type_inferencer.h:323
std::shared_ptr<TypeEnvironment> createInitialEnvironment() {
    // المصدر: type_inferencer.h:44 / Source: type_inferencer.h:44
    auto env = std::make_shared<TypeEnvironment>();
    
    // === إضافة الأنواع المدمجة / Add built-in types ===
    TypeRegistry& registry = TypeRegistry::getInstance();
    
    // المصدر: type_inferencer.h:53 / Source: type_inferencer.h:53
    env->bind("صحيح", registry.getIntegerType());     // عربي / Arabic
    env->bind("integer", registry.getIntegerType());  // إنجليزي / English
    env->bind("int", registry.getIntegerType());      // اختصار / Abbreviation
    
    env->bind("عشري", registry.getFloatType());       // عربي / Arabic
    env->bind("float", registry.getFloatType());      // إنجليزي / English
    
    env->bind("منطقي", registry.getBooleanType());    // عربي / Arabic
    env->bind("boolean", registry.getBooleanType());  // إنجليزي / English
    env->bind("bool", registry.getBooleanType());     // اختصار / Abbreviation
    
    env->bind("نص", registry.getStringType());        // عربي / Arabic
    env->bind("string", registry.getStringType());    // إنجليزي / English
    env->bind("str", registry.getStringType());       // اختصار / Abbreviation
    
    env->bind("فراغ", registry.getVoidType());        // عربي / Arabic
    env->bind("void", registry.getVoidType());        // إنجليزي / English
    
    // === إضافة الثوابت المدمجة / Add built-in constants ===
    env->bind("صحيح", registry.getBooleanType());     // true بالعربي
    env->bind("خطأ", registry.getBooleanType());       // false بالعربي
    env->bind("true", registry.getBooleanType());     // true بالإنجليزي
    env->bind("false", registry.getBooleanType());    // false بالإنجليزي
    
    return env;  // إرجاع البيئة / Return environment
}

} // namespace TypeSystem
} // namespace Sad
