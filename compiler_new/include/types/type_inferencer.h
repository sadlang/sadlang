// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_inferencer.h
// File: type_inferencer.h
//
// الوصف: محرك استنتاج الأنواع الرئيسي (Main Type Inference Engine)
// Description: Main type inference engine using Hindley-Milner algorithm
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// الخوارزمية: Hindley-Milner Type Inference (Algorithm W)
// Algorithm: Hindley-Milner Type Inference (Algorithm W)
//
// الغرض: استنتاج أنواع التعبيرات والدوال تلقائياً بدون تعليقات صريحة
// Purpose: Automatically infer types of expressions and functions without explicit annotations
//
// مثال / Example:
//   دالة هوية(س) { إرجاع س }       => forall T. (T) -> T
//   دالة مربع(س) { إرجاع س * س }   => (Integer | Float) -> (Integer | Float)
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_INFERENCER_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_INFERENCER_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type (type.h:1-218)
#include "type_variable.h"      // استيراد متغيرات الأنواع / Import TypeVariable
#include "constraint.h"         // استيراد القيود / Import Constraint
#include "constraint_solver.h"  // استيراد حلال القيود / Import ConstraintSolver
#include "substitution.h"       // استيراد الاستبدال / Import Substitution
#include <unordered_map>       // لـ جدول الأنواع / For type table
#include <string>              // لـ الأسماء / For names
#include <memory>              // لـ shared_ptr / For shared_ptr

// Forward declarations للـ AST / Forward declarations for AST
// سيتم استيرادها بالكامل في .cpp / Will be fully imported in .cpp
namespace Sad {
    class ASTNode;           // من include/parser/ast/ast_node.h / From ast_node.h
    class Expression;        // من include/parser/ast/expressions.h / From expressions.h
    class Statement;         // من include/parser/ast/statements.h / From statements.h
    class FunctionDeclaration; // من include/parser/ast/declarations.h / From declarations.h
    class VariableDeclaration; // من include/parser/ast/declarations.h / From declarations.h
}

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeEnvironment
// Class: TypeEnvironment
//
// الوصف: بيئة الأنواع - تخزين أنواع المتغيرات والدوال في نطاق معين
// Description: Type environment - stores types of variables and functions in a scope
//
// الاستخدام / Usage:
//   TypeEnvironment env;
//   env.bind("x", integerType);  // ربط x بنوع Integer
//   auto type = env.lookup("x"); // البحث عن نوع x
// ════════════════════════════════════════════════════════════════════════════════

class TypeEnvironment {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي - بيئة فارغة / Default constructor - empty environment
    TypeEnvironment();
    
    // منشئ مع بيئة أب / Constructor with parent environment
    // المعامل / Parameter: parent - البيئة الأب / Parent environment
    explicit TypeEnvironment(std::shared_ptr<TypeEnvironment> parent);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الربط والبحث / Binding and Lookup
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ربط اسم بنوع / Bind name to type
    // المعامل / Parameter: name - اسم المتغير / Variable name
    // المعامل / Parameter: type - النوع / Type
    void bind(const std::string& name, TypePtr type);
    
    // البحث عن نوع / Look up type
    // المعامل / Parameter: name - اسم المتغير / Variable name
    // الإرجاع / Return: النوع أو nullptr إذا لم يُوجد / Type or nullptr if not found
    TypePtr lookup(const std::string& name) const;
    
    // التحقق من وجود ربط / Check if binding exists
    // المعامل / Parameter: name - اسم المتغير / Variable name
    // الإرجاع / Return: true إذا وُجد / true if found
    bool contains(const std::string& name) const;
    
    // إزالة ربط / Remove binding
    // المعامل / Parameter: name - اسم المتغير / Variable name
    void unbind(const std::string& name);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إدارة النطاق / Scope Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    // إنشاء بيئة فرعية / Create child environment
    // الإرجاع / Return: بيئة جديدة تشير إلى هذه البيئة كأب / New environment with this as parent
    std::shared_ptr<TypeEnvironment> createChild();
    
    // الحصول على البيئة الأب / Get parent environment
    // الإرجاع / Return: البيئة الأب أو nullptr / Parent environment or nullptr
    std::shared_ptr<TypeEnvironment> getParent() const { return parent_; }
    
    // مسح البيئة / Clear environment
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الاستعلام / Query
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد الروابط / Get number of bindings
    size_t size() const { return bindings_.size(); }
    
    // التحقق من كون البيئة فارغة / Check if environment is empty
    bool isEmpty() const { return bindings_.empty(); }
    
    // الحصول على جميع الأسماء / Get all names
    std::vector<std::string> getAllNames() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // خريطة الروابط: اسم -> نوع / Bindings map: name -> type
    std::unordered_map<std::string, TypePtr> bindings_;
    
    // البيئة الأب (للنطاقات المتداخلة) / Parent environment (for nested scopes)
    std::shared_ptr<TypeEnvironment> parent_;
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: InferenceContext
// Class: InferenceContext
//
// الوصف: سياق الاستنتاج - يحمل جميع البيانات اللازمة أثناء الاستنتاج
// Description: Inference context - holds all data needed during inference
// ════════════════════════════════════════════════════════════════════════════════

class InferenceContext {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ مع بيئة / Constructor with environment
    explicit InferenceContext(std::shared_ptr<TypeEnvironment> env);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // البيئة / Environment
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على البيئة / Get environment
    std::shared_ptr<TypeEnvironment> getEnvironment() const { return environment_; }
    
    // تعيين بيئة جديدة / Set new environment
    void setEnvironment(std::shared_ptr<TypeEnvironment> env) { environment_ = env; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // القيود / Constraints
    // ═══════════════════════════════════════════════════════════════════════════
    
    // إضافة قيد / Add constraint
    void addConstraint(const Constraint& constraint) { constraints_.add(constraint); }
    
    // الحصول على جميع القيود / Get all constraints
    const ConstraintSet& getConstraints() const { return constraints_; }
    
    // مسح القيود / Clear constraints
    void clearConstraints() { constraints_.clear(); }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // متغيرات الأنواع / Type Variables
    // ═══════════════════════════════════════════════════════════════════════════
    
    // توليد متغير نوع جديد / Generate fresh type variable
    // الإرجاع / Return: متغير نوع جديد / New type variable
    TypePtr freshTypeVar() { return createFreshTypeVariable(); }
    
    // توليد متغير نوع بتلميح / Generate type variable with hint
    // المعامل / Parameter: hint - تلميح الاسم / Name hint
    // الإرجاع / Return: متغير نوع جديد / New type variable
    TypePtr freshTypeVar(const std::string& hint) { (void)hint; return createFreshTypeVariable(); }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // البيئة الحالية / Current environment
    std::shared_ptr<TypeEnvironment> environment_;
    
    // مجموعة القيود المُجمّعة / Collected constraints
    ConstraintSet constraints_;
    
    // عداد متغيرات الأنواع / Type variable counter
    int typeVarCounter_;
};

// ════════════════════════════════════════════════════════════════════════════════
// نوع النتيجة / Result Type
// ════════════════════════════════════════════════════════════════════════════════

// نتيجة الاستنتاج: نوع + استبدال
// Inference result: type + substitution
struct InferenceResult {
    // هل نجح الاستنتاج؟ / Did inference succeed?
    bool success;
    
    // النوع المُستنتج / Inferred type
    TypePtr type;
    
    // الاستبدال الناتج / Resulting substitution
    Substitution substitution;
    
    // رسالة خطأ (إذا فشل) / Error message (if failed)
    std::string errorMessage;
    
    // منشئ لنتيجة ناجحة / Constructor for successful result
    static InferenceResult makeSuccess(TypePtr type, const Substitution& subst);
    
    // منشئ لنتيجة فاشلة / Constructor for failed result
    static InferenceResult makeFailure(const std::string& errorMsg);
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeInferencer
// Class: TypeInferencer
//
// الوصف: محرك استنتاج الأنواع الرئيسي / Main type inference engine
// يُنفذ خوارزمية Hindley-Milner (Algorithm W)
// Implements Hindley-Milner algorithm (Algorithm W)
//
// الاستخدام / Usage:
//   TypeInferencer inferencer;
//   auto result = inferencer.inferExpression(expr);
//   if (result.success) {
//       std::cout << "النوع المُستنتج: " << result.type->toString();
//   }
// ════════════════════════════════════════════════════════════════════════════════

class TypeInferencer {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي / Default constructor
    TypeInferencer();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الاستنتاج الرئيسي / Main Inference
    // ═══════════════════════════════════════════════════════════════════════════
    
    // استنتاج نوع تعبير / Infer type of expression
    // المعامل / Parameter: expr - التعبير / Expression
    // الإرجاع / Return: نتيجة الاستنتاج / Inference result
    InferenceResult inferExpression(const Sad::Expression* expr);
    
    // استنتاج نوع تعبير مع بيئة / Infer type with environment
    // المعامل / Parameter: expr - التعبير / Expression
    // المعامل / Parameter: env - البيئة / Environment
    // الإرجاع / Return: نتيجة الاستنتاج / Inference result
    InferenceResult inferExpression(const Sad::Expression* expr,
                                   std::shared_ptr<TypeEnvironment> env);
    
    // استنتاج أنواع دالة / Infer function types
    // المعامل / Parameter: func - تصريح الدالة / Function declaration
    // الإرجاع / Return: نتيجة الاستنتاج / Inference result
    InferenceResult inferFunction(const Sad::FunctionDeclaration* func);
    
    // استنتاج أنواع متغير / Infer variable types
    // المعامل / Parameter: var - تصريح المتغير / Variable declaration
    // الإرجاع / Return: نتيجة الاستنتاج / Inference result
    InferenceResult inferVariable(const Sad::VariableDeclaration* var);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإعدادات / Settings
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تفعيل/تعطيل Verbose mode / Enable/disable verbose mode
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // هل Verbose mode مُفعّل؟ / Is verbose mode enabled?
    bool isVerbose() const { return verbose_; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإحصائيات / Statistics
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد الاستنتاجات / Get inference count
    int getInferenceCount() const { return inferenceCount_; }
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void resetStatistics();
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال الاستنتاج الداخلية / Internal Inference Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // استنتاج داخلي لتعبير / Internal infer expression
    // المعامل / Parameter: expr - التعبير / Expression
    // المعامل / Parameter: ctx - السياق / Context
    // الإرجاع / Return: النوع المُستنتج / Inferred type
    TypePtr inferExpressionInternal(const Sad::Expression* expr, 
                                   InferenceContext& ctx);
    
    // استنتاج عملية ثنائية / Infer binary operation
    // المعامل / Parameter: left - العامل الأيسر / Left operand
    // المعامل / Parameter: right - العامل الأيمن / Right operand
    // المعامل / Parameter: op - العملية / Operator
    // المعامل / Parameter: ctx - السياق / Context
    // الإرجاع / Return: النوع المُستنتج / Inferred type
    TypePtr inferBinaryOp(const Sad::Expression* left,
                         const Sad::Expression* right,
                         const std::string& op,
                         InferenceContext& ctx);
    
    // استنتاج استدعاء دالة / Infer function call
    // المعامل / Parameter: func - الدالة / Function
    // المعامل / Parameter: args - الوسائط / Arguments
    // المعامل / Parameter: ctx - السياق / Context
    // الإرجاع / Return: النوع المُستنتج / Inferred type
    TypePtr inferFunctionCall(const Sad::Expression* func,
                             const std::vector<const Sad::Expression*>& args,
                             InferenceContext& ctx);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة / Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تعميم نوع (Generalization) / Generalize a type
    // يُحوّل متغيرات الأنواع الحرة إلى forall
    // Converts free type variables to forall
    // المعامل / Parameter: type - النوع / Type
    // المعامل / Parameter: env - البيئة / Environment
    // الإرجاع / Return: نوع معمم / Generalized type
    TypePtr generalize(TypePtr type, const TypeEnvironment& env);
    
    // تخصيص نوع (Instantiation) / Instantiate a type
    // يستبدل متغيرات forall بمتغيرات جديدة
    // Replaces forall variables with fresh variables
    // المعامل / Parameter: type - النوع / Type
    // الإرجاع / Return: نوع مخصص / Instantiated type
    TypePtr instantiate(TypePtr type);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // حلال القيود / Constraint solver
    ConstraintSolver solver_;
    
    // البيئة العامة (built-in types and functions)
    // Global environment (built-in types and functions)
    std::shared_ptr<TypeEnvironment> globalEnv_;
    
    // وضع Verbose / Verbose mode
    bool verbose_;
    
    // إحصائيات / Statistics
    int inferenceCount_;
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// استنتاج سريع لتعبير / Quick infer expression
// المعامل / Parameter: expr - التعبير / Expression
// الإرجاع / Return: نتيجة الاستنتاج / Inference result
InferenceResult inferType(const Sad::Expression* expr);

// إنشاء بيئة أولية بالأنواع المُدمجة / Create initial environment with built-in types
// الإرجاع / Return: بيئة بها الأنواع المُدمجة / Environment with built-in types
std::shared_ptr<TypeEnvironment> createInitialEnvironment();

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_INFERENCER_H
