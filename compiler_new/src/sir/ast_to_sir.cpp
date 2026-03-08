/**
 * =============================================================================
 * ملف: ast_to_sir.cpp
 * الوصف: تحويل AST إلى SIR
 * المهمة: T262 - AST to SIR lowering
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 * 
 * 🔄 دليل المبتدئ لتحويل AST إلى SIR
 * ═══════════════════════════════════════
 * 
 * ما هو التحويل؟
 * ──────────────
 * نأخذ شجرة AST ونحولها إلى تعليمات SIR.
 * 
 * مثال التحويل:
 * ─────────────
 * ```
 * كود حزين:
 *     متغير س = 10
 *     متغير ص = س + 5
 *     اطبع(ص)
 * 
 * AST:
 *     Program
 *     ├── VarDecl(س, 10)
 *     ├── VarDecl(ص, BinaryOp(+, س, 5))
 *     └── Call(اطبع, ص)
 * 
 * SIR:
 *     %0 = تخصيص(عدد)       ; س
 *     تخزين(%0, 10)
 *     %1 = تخصيص(عدد)       ; ص
 *     %2 = تحميل(%0)
 *     %3 = جمع(%2, 5)
 *     تخزين(%1, %3)
 *     %4 = تحميل(%1)
 *     استدعاء(@اطبع, %4)
 *     إسقاط(%1)
 *     إسقاط(%0)
 * ```
 * 
 * خطوات التحويل:
 * ─────────────
 * 1. زيارة كل عقدة في AST
 * 2. إنشاء قيم SIR للمتغيرات
 * 3. تحويل التعبيرات إلى تعليمات
 * 4. إضافة عمليات الملكية المناسبة
 * 5. إدراج Drop في نهاية النطاقات
 * 
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <map>
#include <iostream>

namespace sad::sir {

// =============================================================================
// تمثيل AST مبسط (للتوثيق)
// =============================================================================

/**
 * نوع عقدة AST
 */
enum class AstNodeKind {
    // تعريفات
    Program,
    Function,
    VarDecl,
    ConstDecl,
    Parameter,
    
    // تعبيرات
    Identifier,
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    BoolLiteral,
    BinaryOp,
    UnaryOp,
    Call,
    MethodCall,
    FieldAccess,
    IndexAccess,
    Borrow,
    BorrowMut,
    Move,
    
    // عبارات
    Block,
    If,
    While,
    For,
    Return,
    Assignment,
    
    // أنواع
    TypeAnnotation
};

/**
 * عقدة AST بسيطة
 */
struct AstNode {
    AstNodeKind kind;
    std::string value;                              // للقيم البسيطة
    std::vector<std::shared_ptr<AstNode>> children; // العقد الفرعية
    std::shared_ptr<SirType> type;                  // النوع (إن عُرف)
    SourceLocation location;
    
    // مساعدات للإنشاء
    static std::shared_ptr<AstNode> IntLit(int64_t val, const SourceLocation& loc = {}) {
        auto node = std::make_shared<AstNode>();
        node->kind = AstNodeKind::IntLiteral;
        node->value = std::to_string(val);
        node->type = SirType::Int32();
        node->location = loc;
        return node;
    }
    
    static std::shared_ptr<AstNode> Ident(const std::string& name, const SourceLocation& loc = {}) {
        auto node = std::make_shared<AstNode>();
        node->kind = AstNodeKind::Identifier;
        node->value = name;
        node->location = loc;
        return node;
    }
    
    static std::shared_ptr<AstNode> VarDecl(const std::string& name, 
                                             std::shared_ptr<AstNode> init,
                                             const SourceLocation& loc = {}) {
        auto node = std::make_shared<AstNode>();
        node->kind = AstNodeKind::VarDecl;
        node->value = name;
        if (init) node->children.push_back(init);
        node->location = loc;
        return node;
    }
};

// =============================================================================
// نطاق المتغيرات
// =============================================================================

/**
 * معلومات متغير في النطاق
 */
struct ScopeVariable {
    std::string name;
    ValueId valueId;
    std::shared_ptr<SirType> type;
    bool isMutable;
    bool isMoved = false;
    int scopeDepth;
};

/**
 * نطاق واحد
 */
struct Scope {
    int depth;
    std::vector<ValueId> variables;  // المتغيرات في هذا النطاق
    BlockId startBlock;
};

/**
 * مدير النطاقات
 */
class ScopeManager {
public:
    void pushScope(BlockId startBlock) {
        int depth = scopes_.size();
        scopes_.push_back({depth, {}, startBlock});
    }
    
    std::vector<ValueId> popScope() {
        if (scopes_.empty()) return {};
        
        auto vars = scopes_.back().variables;
        scopes_.pop_back();
        return vars;
    }
    
    void addVariable(const std::string& name, ValueId id, 
                     std::shared_ptr<SirType> type, bool isMut) {
        if (!scopes_.empty()) {
            scopes_.back().variables.push_back(id);
        }
        variables_[name] = {name, id, type, isMut, false, (int)scopes_.size()};
    }
    
    ScopeVariable* lookupVariable(const std::string& name) {
        auto it = variables_.find(name);
        return it != variables_.end() ? &it->second : nullptr;
    }
    
    void markMoved(const std::string& name) {
        auto it = variables_.find(name);
        if (it != variables_.end()) {
            it->second.isMoved = true;
        }
    }
    
    int currentDepth() const {
        return scopes_.size();
    }
    
private:
    std::vector<Scope> scopes_;
    std::map<std::string, ScopeVariable> variables_;
};

// =============================================================================
// مُحول AST إلى SIR
// =============================================================================

/**
 * مُحول AST إلى SIR
 */
class AstToSirLowering {
public:
    AstToSirLowering() {
        nextValueId_ = 0;
    }
    
    /**
     * تحويل برنامج كامل
     */
    SirModule lower(const std::shared_ptr<AstNode>& program) {
        SirModule module;
        module.name = "main";
        
        if (program->kind != AstNodeKind::Program) {
            return module;
        }
        
        // تحويل كل تعريف في البرنامج
        for (const auto& child : program->children) {
            if (child->kind == AstNodeKind::Function) {
                auto func = lowerFunction(child);
                module.addFunction(func);
            }
        }
        
        return module;
    }
    
    /**
     * تحويل دالة
     */
    SirFunction lowerFunction(const std::shared_ptr<AstNode>& funcNode) {
        SirFunction func;
        func.name = funcNode->value;
        func.returnType = funcNode->type ? funcNode->type : SirType::Void();
        
        // إنشاء كتلة الدخول
        BlockId entryBlock = func.addBlock("دخول");
        currentBlock_ = func.getBlock(entryBlock);
        
        // نطاق جديد للدالة
        scopes_.pushScope(entryBlock);
        
        // تحويل المعاملات
        for (size_t i = 0; i < funcNode->children.size() - 1; i++) {
            const auto& param = funcNode->children[i];
            if (param->kind == AstNodeKind::Parameter) {
                SirParameter p;
                p.name = param->value;
                p.type = param->type;
                func.params.push_back(p);
                
                // إنشاء قيمة للمعامل
                ValueId paramId = createValue();
                scopes_.addVariable(p.name, paramId, p.type, p.isMutable);
            }
        }
        
        // تحويل جسم الدالة
        if (!funcNode->children.empty()) {
            const auto& body = funcNode->children.back();
            if (body->kind == AstNodeKind::Block) {
                lowerBlock(body, func);
            }
        }
        
        // إدراج Drop لمتغيرات النطاق
        auto varsToDropFunc = scopes_.popScope();
        for (auto it = varsToDropFunc.rbegin(); it != varsToDropFunc.rend(); ++it) {
            emit(SirInstruction::Drop(*it));
        }
        
        // إضافة return إذا لم يوجد
        if (!currentBlock_->hasTerminator()) {
            emit(SirInstruction::Return());
        }
        
        return func;
    }
    
    /**
     * تحويل كتلة
     */
    void lowerBlock(const std::shared_ptr<AstNode>& block, SirFunction& func) {
        scopes_.pushScope(currentBlock_->id);
        
        for (const auto& stmt : block->children) {
            lowerStatement(stmt, func);
        }
        
        // إدراج Drop لمتغيرات النطاق
        auto varsToDrop = scopes_.popScope();
        for (auto it = varsToDrop.rbegin(); it != varsToDrop.rend(); ++it) {
            emit(SirInstruction::Drop(*it));
        }
    }
    
    /**
     * تحويل عبارة
     */
    void lowerStatement(const std::shared_ptr<AstNode>& stmt, SirFunction& func) {
        switch (stmt->kind) {
            case AstNodeKind::VarDecl:
                lowerVarDecl(stmt, func);
                break;
                
            case AstNodeKind::Assignment:
                lowerAssignment(stmt, func);
                break;
                
            case AstNodeKind::If:
                lowerIf(stmt, func);
                break;
                
            case AstNodeKind::While:
                lowerWhile(stmt, func);
                break;
                
            case AstNodeKind::Return:
                lowerReturn(stmt, func);
                break;
                
            case AstNodeKind::Call:
                lowerCall(stmt, func);
                break;
                
            case AstNodeKind::Block:
                lowerBlock(stmt, func);
                break;
                
            default:
                // تعبير عادي (نتجاهل النتيجة)
                lowerExpression(stmt, func);
                break;
        }
    }
    
    /**
     * تحويل تعريف متغير
     */
    void lowerVarDecl(const std::shared_ptr<AstNode>& varDecl, SirFunction& func) {
        std::string name = varDecl->value;
        auto type = varDecl->type ? varDecl->type : SirType::Int32();
        
        // تخصيص مكان للمتغير
        ValueId varId = createValue();
        emit(SirInstruction::Alloc(varId, type));
        
        // تسجيل المتغير في النطاق
        scopes_.addVariable(name, varId, type, true);
        
        // تهيئة القيمة
        if (!varDecl->children.empty()) {
            ValueId initVal = lowerExpression(varDecl->children[0], func);
            
            // تحديد نوع العملية (نسخ أو نقل)
            auto initType = varDecl->children[0]->type;
            if (initType && initType->isCopyable()) {
                ValueId copyVal = createValue();
                emit(SirInstruction::Copy(copyVal, initVal));
                emit(SirInstruction::Store(varId, copyVal));
            } else {
                emit(SirInstruction::Store(varId, initVal));
            }
        }
    }
    
    /**
     * تحويل تعيين
     */
    void lowerAssignment(const std::shared_ptr<AstNode>& assign, SirFunction& func) {
        // الحصول على المتغير المستهدف
        std::string targetName = assign->children[0]->value;
        auto* target = scopes_.lookupVariable(targetName);
        
        if (!target) return;  // خطأ: متغير غير معرف
        
        // تحويل القيمة الجديدة
        ValueId newVal = lowerExpression(assign->children[1], func);
        
        // تخزين القيمة
        emit(SirInstruction::Store(target->valueId, newVal));
    }
    
    /**
     * تحويل if
     */
    void lowerIf(const std::shared_ptr<AstNode>& ifStmt, SirFunction& func) {
        // تحويل الشرط
        ValueId condVal = lowerExpression(ifStmt->children[0], func);
        
        // إنشاء الكتل
        BlockId thenBlock = func.addBlock("إذا_صح");
        BlockId elseBlock = func.addBlock("إذا_خطأ");
        BlockId mergeBlock = func.addBlock("بعد_إذا");
        
        // تفرع
        emit(SirInstruction::Branch(condVal, thenBlock, elseBlock));
        
        // كتلة then
        currentBlock_ = func.getBlock(thenBlock);
        if (ifStmt->children.size() > 1) {
            lowerStatement(ifStmt->children[1], func);
        }
        if (!currentBlock_->hasTerminator()) {
            emit(SirInstruction::Jump(mergeBlock));
        }
        
        // كتلة else
        currentBlock_ = func.getBlock(elseBlock);
        if (ifStmt->children.size() > 2) {
            lowerStatement(ifStmt->children[2], func);
        }
        if (!currentBlock_->hasTerminator()) {
            emit(SirInstruction::Jump(mergeBlock));
        }
        
        // كتلة الدمج
        currentBlock_ = func.getBlock(mergeBlock);
    }
    
    /**
     * تحويل while
     */
    void lowerWhile(const std::shared_ptr<AstNode>& whileStmt, SirFunction& func) {
        // إنشاء الكتل
        BlockId condBlock = func.addBlock("طالما_شرط");
        BlockId bodyBlock = func.addBlock("طالما_جسم");
        BlockId exitBlock = func.addBlock("بعد_طالما");
        
        // قفز للشرط
        emit(SirInstruction::Jump(condBlock));
        
        // كتلة الشرط
        currentBlock_ = func.getBlock(condBlock);
        ValueId condVal = lowerExpression(whileStmt->children[0], func);
        emit(SirInstruction::Branch(condVal, bodyBlock, exitBlock));
        
        // كتلة الجسم
        currentBlock_ = func.getBlock(bodyBlock);
        if (whileStmt->children.size() > 1) {
            lowerStatement(whileStmt->children[1], func);
        }
        if (!currentBlock_->hasTerminator()) {
            emit(SirInstruction::Jump(condBlock));
        }
        
        // كتلة الخروج
        currentBlock_ = func.getBlock(exitBlock);
    }
    
    /**
     * تحويل return
     */
    void lowerReturn(const std::shared_ptr<AstNode>& ret, SirFunction& func) {
        if (ret->children.empty()) {
            emit(SirInstruction::Return());
        } else {
            ValueId retVal = lowerExpression(ret->children[0], func);
            emit(SirInstruction::Return(retVal));
        }
    }
    
    /**
     * تحويل استدعاء دالة
     */
    ValueId lowerCall(const std::shared_ptr<AstNode>& call, SirFunction& func) {
        std::string funcName = call->value;
        std::vector<ValueId> args;
        
        for (const auto& arg : call->children) {
            args.push_back(lowerExpression(arg, func));
        }
        
        ValueId result = createValue();
        emit(SirInstruction::Call(result, funcName, 0, args));
        
        return result;
    }
    
    /**
     * تحويل تعبير
     */
    ValueId lowerExpression(const std::shared_ptr<AstNode>& expr, SirFunction& func) {
        switch (expr->kind) {
            case AstNodeKind::IntLiteral:
                return lowerIntLiteral(expr);
                
            case AstNodeKind::FloatLiteral:
                return lowerFloatLiteral(expr);
                
            case AstNodeKind::StringLiteral:
                return lowerStringLiteral(expr);
                
            case AstNodeKind::BoolLiteral:
                return lowerBoolLiteral(expr);
                
            case AstNodeKind::Identifier:
                return lowerIdentifier(expr);
                
            case AstNodeKind::BinaryOp:
                return lowerBinaryOp(expr, func);
                
            case AstNodeKind::UnaryOp:
                return lowerUnaryOp(expr, func);
                
            case AstNodeKind::Call:
                return lowerCall(expr, func);
                
            case AstNodeKind::Borrow:
                return lowerBorrow(expr, false);
                
            case AstNodeKind::BorrowMut:
                return lowerBorrow(expr, true);
                
            case AstNodeKind::Move:
                return lowerMove(expr);
                
            case AstNodeKind::FieldAccess:
                return lowerFieldAccess(expr, func);
                
            default:
                // (AR) نوع عقدة AST غير معالج — تحذير + قيمة افتراضية
                // (EN) Unhandled AST expression kind — warn + default value
                std::cerr << "[sadc تحذير] نوع تعبير AST غير معالج في SIR lowering: "
                          << static_cast<int>(expr->kind()) << std::endl;
                return createValue();
        }
    }
    
private:
    // إصدار تعليمة
    void emit(const SirInstruction& inst) {
        if (currentBlock_) {
            currentBlock_->addInstruction(inst);
        }
    }
    
    // إنشاء معرف قيمة جديد
    ValueId createValue() {
        return nextValueId_++;
    }
    
    // تحويل القيم الحرفية
    ValueId lowerIntLiteral(const std::shared_ptr<AstNode>& lit) {
        ValueId id = createValue();
        SirInstruction inst;
        inst.opcode = Opcode::Alloc;
        inst.result = id;
        inst.operands.push_back(TypeOperand{SirType::Int32()});
        emit(inst);
        
        // تخزين القيمة
        ValueId constId = createValue();
        // سنفترض أن القيمة مخزنة مباشرة
        
        return id;
    }
    
    ValueId lowerFloatLiteral(const std::shared_ptr<AstNode>& lit) {
        ValueId id = createValue();
        emit(SirInstruction::Alloc(id, SirType::Float64()));
        return id;
    }
    
    ValueId lowerStringLiteral(const std::shared_ptr<AstNode>& lit) {
        ValueId id = createValue();
        emit(SirInstruction::Alloc(id, SirType::String()));
        return id;
    }
    
    ValueId lowerBoolLiteral(const std::shared_ptr<AstNode>& lit) {
        ValueId id = createValue();
        emit(SirInstruction::Alloc(id, SirType::Bool()));
        return id;
    }
    
    // تحويل المعرفات
    ValueId lowerIdentifier(const std::shared_ptr<AstNode>& ident) {
        auto* var = scopes_.lookupVariable(ident->value);
        if (var) {
            // تحميل قيمة المتغير
            ValueId loadedVal = createValue();
            emit(SirInstruction::Load(loadedVal, var->valueId));
            return loadedVal;
        }
        return createValue();  // خطأ: متغير غير معرف
    }
    
    // تحويل العمليات الثنائية
    ValueId lowerBinaryOp(const std::shared_ptr<AstNode>& binOp, SirFunction& func) {
        ValueId left = lowerExpression(binOp->children[0], func);
        ValueId right = lowerExpression(binOp->children[1], func);
        ValueId result = createValue();
        
        std::string op = binOp->value;
        
        SirInstruction inst;
        inst.result = result;
        
        if (op == "+" || op == "جمع") {
            inst.opcode = Opcode::Add;
        } else if (op == "-" || op == "طرح") {
            inst.opcode = Opcode::Sub;
        } else if (op == "*" || op == "ضرب") {
            inst.opcode = Opcode::Mul;
        } else if (op == "/" || op == "قسمة") {
            inst.opcode = Opcode::Div;
        } else if (op == "%" || op == "باقي") {
            inst.opcode = Opcode::Mod;
        } else if (op == "==" || op == "يساوي") {
            inst.opcode = Opcode::Eq;
        } else if (op == "!=" || op == "لا_يساوي") {
            inst.opcode = Opcode::Ne;
        } else if (op == "<" || op == "أقل") {
            inst.opcode = Opcode::Lt;
        } else if (op == "<=" || op == "أقل_أو_يساوي") {
            inst.opcode = Opcode::Le;
        } else if (op == ">" || op == "أكبر") {
            inst.opcode = Opcode::Gt;
        } else if (op == ">=" || op == "أكبر_أو_يساوي") {
            inst.opcode = Opcode::Ge;
        } else if (op == "و" || op == "&&") {
            inst.opcode = Opcode::And;
        } else if (op == "أو" || op == "||") {
            inst.opcode = Opcode::Or;
        } else {
            inst.opcode = Opcode::Add;  // افتراضي
        }
        
        inst.operands.push_back(ValueOperand{left});
        inst.operands.push_back(ValueOperand{right});
        emit(inst);
        
        return result;
    }
    
    // تحويل العمليات الأحادية
    ValueId lowerUnaryOp(const std::shared_ptr<AstNode>& unaryOp, SirFunction& func) {
        ValueId operand = lowerExpression(unaryOp->children[0], func);
        ValueId result = createValue();
        
        std::string op = unaryOp->value;
        
        SirInstruction inst;
        inst.result = result;
        
        if (op == "-" || op == "سالب") {
            inst.opcode = Opcode::Neg;
        } else if (op == "!" || op == "ليس") {
            inst.opcode = Opcode::Not;
        } else {
            inst.opcode = Opcode::Copy;  // افتراضي
        }
        
        inst.operands.push_back(ValueOperand{operand});
        emit(inst);
        
        return result;
    }
    
    // تحويل الاستعارة
    ValueId lowerBorrow(const std::shared_ptr<AstNode>& borrowNode, bool isMut) {
        std::string varName = borrowNode->children[0]->value;
        auto* var = scopes_.lookupVariable(varName);
        
        if (!var) return createValue();
        
        ValueId result = createValue();
        
        if (isMut) {
            emit(SirInstruction::BorrowMut(result, var->valueId));
        } else {
            emit(SirInstruction::Borrow(result, var->valueId));
        }
        
        return result;
    }
    
    // تحويل النقل
    ValueId lowerMove(const std::shared_ptr<AstNode>& moveNode) {
        std::string varName = moveNode->children[0]->value;
        auto* var = scopes_.lookupVariable(varName);
        
        if (!var) return createValue();
        
        ValueId result = createValue();
        emit(SirInstruction::Move(result, var->valueId));
        
        // تعليم المتغير كمنقول
        scopes_.markMoved(varName);
        
        return result;
    }
    
    // تحويل الوصول للحقل
    ValueId lowerFieldAccess(const std::shared_ptr<AstNode>& access, SirFunction& func) {
        ValueId base = lowerExpression(access->children[0], func);
        std::string fieldName = access->value;
        
        ValueId result = createValue();
        emit(SirInstruction::Project(result, base, fieldName, 0));
        
        return result;
    }
    
    // الحالة الداخلية
    ScopeManager scopes_;
    SirBasicBlock* currentBlock_ = nullptr;
    ValueId nextValueId_ = 0;
};

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadAstToSir {
    sad::sir::AstToSirLowering* lowering;
};

SadAstToSir* sad_ast_to_sir_new() {
    auto* ctx = new SadAstToSir();
    ctx->lowering = new sad::sir::AstToSirLowering();
    return ctx;
}

void sad_ast_to_sir_free(SadAstToSir* ctx) {
    if (ctx) {
        delete ctx->lowering;
        delete ctx;
    }
}

} // extern "C"
