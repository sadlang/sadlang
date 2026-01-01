/*
 * ============================================================================
 * Type Inference - استنتاج الأنواع
 * Type Inference - Type Inference
 * ============================================================================
 * 
 * الوصف (Description):
 *   هذا الملف يحتوي على محرك Type Inference المبني على Algorithm W
 *   (Hindley-Milner type inference). يستنتج أنواع التعابير والمتغيرات
 *   تلقائياً بدون تصريح صريح من المبرمج.
 * 
 *   This file contains the Type Inference engine based on Algorithm W
 *   (Hindley-Milner type inference). Infers types of expressions and
 *   variables automatically without explicit declaration.
 * 
 * المكونات الرئيسية (Main Components):
 *   - TypeVariable: متغير نوع (لم يُحدد بعد) / Type variable (not yet determined)
 *   - Constraint: قيد على الأنواع / Type constraint
 *   - TypeSubstitution: استبدال الأنواع / Type substitution
 *   - TypeInference: محرك الاستنتاج الرئيسي / Main inference engine
 * 
 * الخوارزمية (Algorithm):
 *   Algorithm W (Damas-Milner):
 *   1. Generate constraints من الكود / Generate constraints from code
 *   2. Solve constraints عبر unification / Solve constraints via unification
 *   3. Substitute type variables / استبدال متغيرات الأنواع
 *   4. Return inferred type / إرجاع النوع المُستنتج
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_TYPE_INFERENCE_H
#define SAD_TYPE_INFERENCE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "typed_ast.h"
#include "type_context.h"
#include "../../../../include/parser/ast/ast_node.h"
#include "../../../../include/parser/ast/expressions.h"
#include "../../../../include/parser/ast/statements.h"

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

class TypeVariable;
class Constraint;
class TypeSubstitution;
class TypeInference;

// ============================================================================
// TypeVariable - متغير نوع
// TypeVariable - Type Variable
// ============================================================================

/**
 * متغير نوع - نوع لم يُحدد بعد
 * Type variable - type not yet determined
 * 
 * يُستخدم في Algorithm W:
 * - let x = 5     // x : α (type variable)
 * - x + 3         // α must be int (constraint)
 * - α = int       // substitution
 */
class TypeVariable : public Type {
private:
    static int nextId_;           // معرف تلقائي متزايد / Auto-incrementing ID
    int id_;                      // معرف المتغير / Variable ID
    std::string varName_;         // اسم المتغير (α, β, γ...) / Variable name
    bool isBound_;                // هل تم ربطه بنوع / Is bound to type
    std::shared_ptr<Type> boundType_; // النوع المربوط / Bound type
    
public:
    /**
     * المنشئ / Constructor
     */
    TypeVariable()
        : Type(Kind::GENERIC, generateVarName())
        , id_(nextId_++)
        , varName_(getName())
        , isBound_(false)
        , boundType_(nullptr)
    {}
    
    /**
     * المنشئ باسم محدد / Constructor with specific name
     * 
     * @param name اسم المتغير / Variable name
     */
    explicit TypeVariable(const std::string& name)
        : Type(Kind::GENERIC, name)
        , id_(nextId_++)
        , varName_(name)
        , isBound_(false)
        , boundType_(nullptr)
    {}
    
    /**
     * الحصول على معرف المتغير / Get variable ID
     */
    int getId() const { return id_; }
    
    /**
     * الحصول على اسم المتغير / Get variable name
     */
    std::string getVarName() const { return varName_; }
    
    /**
     * هل تم ربط المتغير / Is variable bound
     */
    bool isBound() const { return isBound_; }
    
    /**
     * الحصول على النوع المربوط / Get bound type
     */
    std::shared_ptr<Type> getBoundType() const { return boundType_; }
    
    /**
     * ربط المتغير بنوع / Bind variable to type
     * 
     * @param type النوع المراد الربط به / Type to bind to
     */
    void bind(std::shared_ptr<Type> type) {
        isBound_ = true;
        boundType_ = type;
    }
    
    /**
     * فك الربط / Unbind
     */
    void unbind() {
        isBound_ = false;
        boundType_ = nullptr;
    }
    
    /**
     * الحصول على النوع الفعلي (مع اتباع الروابط) / Get actual type (following bindings)
     */
    std::shared_ptr<Type> resolve() const {
        if (isBound_ && boundType_) {
            // إذا كان النوع المربوط هو متغير آخر، اتبعه / If bound type is another variable, follow it
            auto varType = std::dynamic_pointer_cast<TypeVariable>(boundType_);
            if (varType) {
                return varType->resolve();
            }
            return boundType_;
        }
        return nullptr; // غير محدد بعد / Not yet determined
    }
    
    /**
     * تحويل لنص / Convert to string
     */
    std::string toString() const override {
        if (isBound_ && boundType_) {
            return boundType_->toString();
        }
        return varName_;
    }
    
    /**
     * مقارنة / Comparison
     */
    bool equals(const Type* other) const override {
        // إذا كان مربوطاً، قارن بالنوع المربوط / If bound, compare with bound type
        if (isBound_ && boundType_) {
            return boundType_->equals(other);
        }
        
        // إذا كان الآخر متغير نوع، قارن بالمعرف / If other is type variable, compare by ID
        auto otherVar = dynamic_cast<const TypeVariable*>(other);
        if (otherVar) {
            return id_ == otherVar->id_;
        }
        
        return false;
    }
    
private:
    /**
     * توليد اسم متغير تلقائياً / Generate variable name automatically
     */
    static std::string generateVarName() {
        // استخدام حروف يونانية: α, β, γ, δ... / Use Greek letters
        static const char* greekLetters[] = {
            "α", "β", "γ", "δ", "ε", "ζ", "η", "θ",
            "ι", "κ", "λ", "μ", "ν", "ξ", "ο", "π",
            "ρ", "σ", "τ", "υ", "φ", "χ", "ψ", "ω"
        };
        
        int index = nextId_ % 24; // 24 حرف يوناني / 24 Greek letters
        int count = nextId_ / 24;
        
        std::string name = greekLetters[index];
        if (count > 0) {
            name += std::to_string(count);
        }
        
        return name;
    }
};

// ============================================================================
// Constraint - قيد على الأنواع
// Constraint - Type Constraint
// ============================================================================

/**
 * قيد على الأنواع - يجب أن يكون نوعان متساويين أو متوافقين
 * Type constraint - two types must be equal or compatible
 * 
 * مثال:
 * x + y  =>  constraint: type(x) = type(y) = int
 */
class Constraint {
public:
    /**
     * نوع القيد / Constraint kind
     */
    enum class Kind {
        EQUALITY,       // t1 = t2 (يجب أن يكونا متساويين / must be equal)
        SUBTYPE,        // t1 <: t2 (t1 نوع فرعي من t2 / t1 is subtype of t2)
        MEMBER_ACCESS,  // t1.member : t2 (عضو في نوع / member in type)
        FUNCTION_CALL   // t1(t2...) : t3 (استدعاء دالة / function call)
    };

private:
    Kind kind_;                      // نوع القيد / Constraint kind
    std::shared_ptr<Type> type1_;    // النوع الأول / First type
    std::shared_ptr<Type> type2_;    // النوع الثاني / Second type
    std::string memberName_;         // اسم العضو (للـ MEMBER_ACCESS) / Member name
    AST::ASTNode* sourceNode_;       // العقدة المصدرية / Source node
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param kind نوع القيد / Constraint kind
     * @param type1 النوع الأول / First type
     * @param type2 النوع الثاني / Second type
     * @param sourceNode العقدة المصدرية / Source node
     */
    Constraint(
        Kind kind,
        std::shared_ptr<Type> type1,
        std::shared_ptr<Type> type2,
        AST::ASTNode* sourceNode = nullptr
    )
        : kind_(kind)
        , type1_(type1)
        , type2_(type2)
        , sourceNode_(sourceNode)
    {}
    
    // Getters
    Kind getKind() const { return kind_; }
    std::shared_ptr<Type> getType1() const { return type1_; }
    std::shared_ptr<Type> getType2() const { return type2_; }
    std::string getMemberName() const { return memberName_; }
    AST::ASTNode* getSourceNode() const { return sourceNode_; }
    
    // Setters
    void setMemberName(const std::string& name) { memberName_ = name; }
    
    /**
     * تحويل لنص / Convert to string
     */
    std::string toString() const {
        std::string result;
        switch (kind_) {
            case Kind::EQUALITY:
                result = type1_->toString() + " = " + type2_->toString();
                break;
            case Kind::SUBTYPE:
                result = type1_->toString() + " <: " + type2_->toString();
                break;
            case Kind::MEMBER_ACCESS:
                result = type1_->toString() + "." + memberName_ + " : " + type2_->toString();
                break;
            case Kind::FUNCTION_CALL:
                result = type1_->toString() + "(...) : " + type2_->toString();
                break;
        }
        return result;
    }
};

// ============================================================================
// TypeSubstitution - استبدال الأنواع
// TypeSubstitution - Type Substitution
// ============================================================================

/**
 * استبدال متغيرات الأنواع بأنواع محددة
 * Substitute type variables with concrete types
 * 
 * مثال:
 * [α → int, β → string]
 * α + β  =>  int + string
 */
class TypeSubstitution {
private:
    // خريطة من متغير النوع إلى النوع المستبدل / Map from type variable to substituted type
    std::unordered_map<int, std::shared_ptr<Type>> substitutions_;
    
public:
    /**
     * المنشئ / Constructor
     */
    TypeSubstitution() = default;
    
    /**
     * إضافة استبدال / Add substitution
     * 
     * @param varId معرف متغير النوع / Type variable ID
     * @param type النوع المستبدل / Substituted type
     */
    void add(int varId, std::shared_ptr<Type> type) {
        substitutions_[varId] = type;
    }
    
    /**
     * الحصول على الاستبدال / Get substitution
     * 
     * @param varId معرف متغير النوع / Type variable ID
     * @return النوع المستبدل أو nullptr / Substituted type or nullptr
     */
    std::shared_ptr<Type> get(int varId) const {
        auto it = substitutions_.find(varId);
        return (it != substitutions_.end()) ? it->second : nullptr;
    }
    
    /**
     * هل يوجد استبدال / Has substitution
     * 
     * @param varId معرف متغير النوع / Type variable ID
     * @return true إذا كان موجوداً / true if exists
     */
    bool has(int varId) const {
        return substitutions_.find(varId) != substitutions_.end();
    }
    
    /**
     * تطبيق الاستبدال على نوع / Apply substitution to type
     * 
     * @param type النوع المراد تطبيق الاستبدال عليه / Type to apply substitution to
     * @return النوع بعد الاستبدال / Type after substitution
     */
    std::shared_ptr<Type> apply(std::shared_ptr<Type> type) const;
    
    /**
     * دمج استبدالين / Compose two substitutions
     * 
     * @param other الاستبدال الآخر / Other substitution
     * @return استبدال مدمج / Composed substitution
     */
    TypeSubstitution compose(const TypeSubstitution& other) const;
    
    /**
     * عدد الاستبدالات / Number of substitutions
     */
    size_t size() const {
        return substitutions_.size();
    }
    
    /**
     * مسح جميع الاستبدالات / Clear all substitutions
     */
    void clear() {
        substitutions_.clear();
    }
    
    /**
     * تحويل لنص / Convert to string
     */
    std::string toString() const;
};

// ============================================================================
// TypeInference - محرك استنتاج الأنواع
// TypeInference - Type Inference Engine
// ============================================================================

/**
 * محرك استنتاج الأنواع - Algorithm W
 * Type inference engine - Algorithm W
 * 
 * يستنتج أنواع التعابير تلقائياً:
 * ```
 * let x = 5           // infer: x : int
 * let y = x + 3       // infer: y : int
 * let f = λx -> x + 1 // infer: f : int -> int
 * ```
 */
class TypeInference {
private:
    std::shared_ptr<TypeContext> context_;          // سياق Type checking / Type checking context
    std::vector<Constraint> constraints_;           // قائمة القيود / Constraint list
    TypeSubstitution substitution_;                 // الاستبدالات الحالية / Current substitutions
    int freshVarCounter_;                           // عداد لمتغيرات الأنواع الجديدة / Counter for fresh type variables
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param context سياق Type checking / Type checking context
     */
    explicit TypeInference(std::shared_ptr<TypeContext> context)
        : context_(context)
        , freshVarCounter_(0)
    {}
    
    /**
     * استنتاج نوع تعبير / Infer expression type
     * 
     * @param expr التعبير / Expression
     * @return النوع المُستنتج / Inferred type
     */
    std::shared_ptr<Type> inferExpr(AST::Expression* expr);
    
    /**
     * استنتاج نوع جملة / Infer statement type
     * 
     * @param stmt الجملة / Statement
     * @return النوع المُستنتج / Inferred type
     */
    std::shared_ptr<Type> inferStmt(AST::Statement* stmt);
    
    /**
     * استنتاج نوع تصريح / Infer declaration type
     * 
     * @param stmt التصريح / Declaration statement
     * @return النوع المُستنتج / Inferred type
     */
    std::shared_ptr<Type> inferDecl(AST::Statement* stmt);
    
    /**
     * إنشاء متغير نوع جديد / Create fresh type variable
     * 
     * @return متغير نوع جديد / Fresh type variable
     */
    std::shared_ptr<TypeVariable> freshTypeVariable();
    
    /**
     * توليد قيود من تعبير / Generate constraints from expression
     * 
     * @param expr التعبير / Expression
     * @param expectedType النوع المتوقع / Expected type
     */
    void generateConstraints(AST::Expression* expr, std::shared_ptr<Type> expectedType);
    
    /**
     * حل القيود (Unification) / Solve constraints (Unification)
     * 
     * @return true إذا نجح الحل / true if successful
     */
    bool solveConstraints();
    
    /**
     * توحيد نوعين (Unification) / Unify two types
     * 
     * @param type1 النوع الأول / First type
     * @param type2 النوع الثاني / Second type
     * @return true إذا نجح التوحيد / true if successful
     */
    bool unify(std::shared_ptr<Type> type1, std::shared_ptr<Type> type2);
    
    /**
     * تطبيق الاستبدالات / Apply substitutions
     * 
     * @param type النوع / Type
     * @return النوع بعد الاستبدال / Type after substitution
     */
    std::shared_ptr<Type> substitute(std::shared_ptr<Type> type);
    
    /**
     * الحصول على القيود / Get constraints
     */
    const std::vector<Constraint>& getConstraints() const {
        return constraints_;
    }
    
    /**
     * الحصول على الاستبدالات / Get substitutions
     */
    const TypeSubstitution& getSubstitution() const {
        return substitution_;
    }
    
    /**
     * مسح القيود والاستبدالات / Clear constraints and substitutions
     */
    void reset() {
        constraints_.clear();
        substitution_.clear();
        freshVarCounter_ = 0;
    }

private:
    // دوال مساعدة للأنواع المختلفة من التعابير / Helper functions for different expression types
    std::shared_ptr<Type> inferLiteral(AST::Expression* expr);
    std::shared_ptr<Type> inferBinaryOp(AST::BinaryExpr* expr);
    std::shared_ptr<Type> inferUnaryOp(AST::UnaryExpr* expr);
    std::shared_ptr<Type> inferVariable(AST::VariableExpr* expr);
    std::shared_ptr<Type> inferFunctionCall(AST::CallExpr* expr);
    std::shared_ptr<Type> inferMemberAccess(AST::MemberExpr* expr);
    std::shared_ptr<Type> inferArrayAccess(AST::IndexExpr* expr);
    std::shared_ptr<Type> inferLambda(AST::LambdaExpr* expr);
    
    // دالة للتحقق من occurs check (منع التعريفات الدائرية) / Check for occurs check
    bool occursIn(std::shared_ptr<TypeVariable> var, std::shared_ptr<Type> type);
};

} // namespace TypeChecker
} // namespace Sad

#endif // SAD_TYPE_INFERENCE_H
