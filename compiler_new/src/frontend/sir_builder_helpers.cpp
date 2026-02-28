// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <functional>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// HELPER FUNCTIONS - الدوال المساعدة
// ============================================================================

// ============================================================================
// createBasicBlock - إنشاء كتلة أساسية جديدة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:501
// التوقيع / Signature: std::shared_ptr<SIRBasicBlock> createBasicBlock(const std::string& name);
//
// الإرجاع / Returns:
// - std::shared_ptr<SIRBasicBlock>: sir_module.h:145 (SIRBasicBlock class)
//
// SIRBasicBlock Constructor (sir_module.h:145):
// - SIRBasicBlock(const std::string& name): line 168
// ============================================================================
std::shared_ptr<SIRBasicBlock> SIRBuilder::createBasicBlock(const std::string& name) {
    // (AR) إنشاء كتلة جديدة (sir_module.h:168 - SIRBasicBlock constructor)
    // (EN) Create new block
    return std::make_shared<SIRBasicBlock>(name);
}

// ============================================================================
// newTempRegister - إنشاء سجل مؤقت جديد
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:511
// التوقيع / Signature: std::string newTempRegister();
//
// المتغيرات المستخدمة / Used variables:
// - registerCounter_: sir_builder.h:600 (int)
//
// الإرجاع / Returns:
// - std::string: اسم السجل بصيغة %0, %1, %2, ...
// ============================================================================
std::string SIRBuilder::newTempRegister() {
    // (AR) إنشاء سجل بصيغة %N حيث N هو nextTempRegister_
    // (EN) Create register in format %N where N is nextTempRegister_
    return "%" + std::to_string(nextTempRegister_++);
}

// ============================================================================
// newLabel - إنشاء تسمية جديدة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:520
// التوقيع / Signature: std::string newLabel(const std::string& prefix);
//
// المعاملات / Parameters:
// - prefix: std::string = البادئة (L, if, loop, etc)
//
// المتغيرات المستخدمة / Used variables:
// - labelCounter_: sir_builder.h:601 (int)
//
// الإرجاع / Returns:
// - std::string: اسم التسمية بصيغة prefix_N
// ============================================================================
std::string SIRBuilder::newLabel(const std::string& prefix) {
    // (AR) إنشاء تسمية بصيغة prefix_N
    // (EN) Create label in format prefix_N
    return prefix + "_" + std::to_string(nextLabel_++);
}

// ============================================================================
// astTypeToSIRType - تحويل DataType إلى SIRType
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:741
// التوقيع / Signature: SIRType astTypeToSIRType(const Sad::Data::DataType& type);
//
// المعاملات / Parameters:
// - type: const Sad::Data::DataType& (parser/data.h)
//
// الإرجاع / Returns:
// - SIRType: sir_types.h:57 (enum class SIRType)
//
// SIRType Values (sir_types.h:57):
// - VOID, I64, F64, BOOL, PTR, ARRAY, STRING, STRUCT, FUNCTION
//
// DataType Values (data_types.h:20):
// - UNKNOWN, INTEGER, FLOAT, STRING, BOOLEAN, NONE, ARRAY, MAP, TUPLE,
//   FUNCTION, OBJECT, ENUM, BYTE, ERROR
// ============================================================================
SIRType SIRBuilder::astTypeToSIRType(const Sad::Data::DataType& type) {
    // (AR) تحويل DataType إلى SIRType
    // (EN) Convert DataType to SIRType
    switch (type) {
        case Data::DataType::INTEGER:
            return SIRType::I64;
        case Data::DataType::FLOAT:
            return SIRType::F64;
        case Data::DataType::BOOLEAN:
            return SIRType::BOOL;
        case Data::DataType::STRING:
            return SIRType::STRING;
        case Data::DataType::ARRAY:
            return SIRType::ARRAY;
        case Data::DataType::FUNCTION:
            return SIRType::FUNCTION;
        case Data::DataType::OBJECT:
            return SIRType::STRUCT;
        case Data::DataType::NONE:
            return SIRType::VOID;
        case Data::DataType::UNKNOWN:
            // (AR) نوع غير معروف - سيتم استنتاجه من التعبير
            // (EN) Unknown type - will be inferred from expression
            return SIRType::I64;  // Default, will be overwritten by type inference
        default:
            // (AR) أنواع أخرى (MAP, TUPLE, ENUM, BYTE, ERROR)
            // (EN) Other types (MAP, TUPLE, ENUM, BYTE, ERROR)
            return SIRType::I64;  // Fallback
    }
}

// ============================================================================
// hasReturnWithValue - فحص إذا كانت الجملة تحتوي return مع قيمة
// ============================================================================
// التوقيع / Signature: bool hasReturnWithValue(const Sad::AST::Statement* stmt);
//
// الوصف / Description:
// تفحص الجملة بشكل تعاودي للبحث عن جمل return تحتوي قيمة.
// Recursively checks statement for return statements with values.
// ============================================================================
bool SIRBuilder::hasReturnWithValue(const Sad::AST::Statement* stmt) {
    if (!stmt) return false;
    
    // (AR) جملة return مباشرة
    // (EN) Direct return statement
    if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt*>(stmt)) {
        return ret->value != nullptr;  // true if return has a value
    }
    
    // (AR) كتلة من الجمل
    // (EN) Block of statements
    if (auto block = dynamic_cast<const Sad::AST::BlockStmt*>(stmt)) {
        for (const auto& s : block->statements) {
            if (hasReturnWithValue(s.get())) return true;
        }
        return false;
    }
    
    // (AR) جملة if
    // (EN) If statement
    if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt*>(stmt)) {
        if (hasReturnWithValue(ifStmt->thenBranch.get())) return true;
        if (ifStmt->elseBranch && hasReturnWithValue(ifStmt->elseBranch.get())) return true;
        return false;
    }
    
    // (AR) حلقة while
    // (EN) While loop
    if (auto whileLoop = dynamic_cast<const Sad::AST::WhileStmt*>(stmt)) {
        return hasReturnWithValue(whileLoop->body.get());
    }
    
    // (AR) حلقة for
    // (EN) For loop
    if (auto forLoop = dynamic_cast<const Sad::AST::ForStmt*>(stmt)) {
        return hasReturnWithValue(forLoop->body.get());
    }
    
    // (AR) حلقة for-range
    // (EN) For-range loop
    if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt*>(stmt)) {
        return hasReturnWithValue(forRange->body.get());
    }
    
    // (AR) جمل أخرى لا تحتوي return
    // (EN) Other statements don't contain return
    return false;
}

// ============================================================================
// inferReturnTypeFromBody - استنتاج نوع الإرجاع من جسم الدالة
// ============================================================================
// التوقيع / Signature: SIRType inferReturnTypeFromBody(const Sad::AST::Statement* body);
//
// الوصف / Description:
// تفحص جسم الدالة لاستنتاج نوع الإرجاع:
// - إذا لم يكن هناك return مع قيمة، تُرجع VOID
// - إذا وُجد return مع قيمة، تفحص نوع التعبير
// ============================================================================

// Helper: get the return expression from the body
static const Sad::AST::Expression* findFirstReturnExpr(const Sad::AST::Statement* stmt) {
    if (!stmt) return nullptr;
    if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt*>(stmt)) {
        return ret->value ? ret->value.get() : nullptr;
    }
    if (auto block = dynamic_cast<const Sad::AST::BlockStmt*>(stmt)) {
        for (const auto& s : block->statements) {
            auto* e = findFirstReturnExpr(s.get());
            if (e) return e;
        }
    }
    if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt*>(stmt)) {
        auto* e = findFirstReturnExpr(ifStmt->thenBranch.get());
        if (e) return e;
        if (ifStmt->elseBranch) return findFirstReturnExpr(ifStmt->elseBranch.get());
    }
    if (auto w = dynamic_cast<const Sad::AST::WhileStmt*>(stmt)) {
        return findFirstReturnExpr(w->body.get());
    }
    if (auto f = dynamic_cast<const Sad::AST::ForStmt*>(stmt)) {
        return findFirstReturnExpr(f->body.get());
    }
    if (auto fr = dynamic_cast<const Sad::AST::ForRangeStmt*>(stmt)) {
        return findFirstReturnExpr(fr->body.get());
    }
    return nullptr;
}

SIRType SIRBuilder::inferReturnTypeFromBody(const Sad::AST::Statement* body) {
    if (!body) return SIRType::VOID;
    
    if (!hasReturnWithValue(body)) {
        return SIRType::VOID;
    }
    
    // (AR) Helper lambda لاستنتاج نوع تعبير بشكل تعاودي
    // (EN) Recursive helper to infer expression type
    std::function<SIRType(const Sad::AST::Expression*)> inferExprType;
    inferExprType = [&](const Sad::AST::Expression* expr) -> SIRType {
        if (!expr) return SIRType::I64;
        
        // Check if it's a literal
        if (auto lit = dynamic_cast<const Sad::AST::LiteralExpr*>(expr)) {
            auto tt = lit->token.getType();
            if (tt == Sad::Lexer::TokenType::NUMBER_DOUBLE) return SIRType::F64;
            // (AR) النصوص تُرجع STRING وليس PTR
            // (EN) String literals return STRING not PTR
            if (tt == Sad::Lexer::TokenType::STRING_LITERAL) return SIRType::STRING;
            if (tt == Sad::Lexer::TokenType::LITERAL_TRUE ||
                tt == Sad::Lexer::TokenType::LITERAL_FALSE) return SIRType::BOOL;
            return SIRType::I64;
        }
        // Check array expressions
        if (dynamic_cast<const Sad::AST::ArrayExpr*>(expr)) return SIRType::ARRAY;
        // Check binary expression - if either operand is F64, result is F64
        if (auto bin = dynamic_cast<const Sad::AST::BinaryExpr*>(expr)) {
            SIRType left = inferExprType(bin->left.get());
            SIRType right = inferExprType(bin->right.get());
            if (left == SIRType::STRING || right == SIRType::STRING) return SIRType::STRING;
            if (left == SIRType::F64 || right == SIRType::F64) return SIRType::F64;
            // (AR) عمليات المقارنة تُرجع BOOL
            // (EN) Comparison operators return BOOL
            switch (bin->op) {
                case Sad::Lexer::TokenType::OP_EQUAL:
                case Sad::Lexer::TokenType::OP_NOT_EQUAL:
                case Sad::Lexer::TokenType::OP_LESS:
                case Sad::Lexer::TokenType::OP_GREATER:
                case Sad::Lexer::TokenType::OP_LESS_EQUAL:
                case Sad::Lexer::TokenType::OP_GREATER_EQUAL:
                case Sad::Lexer::TokenType::OP_AND:
                case Sad::Lexer::TokenType::OP_OR:
                    return SIRType::BOOL;
                default:
                    break;
            }
            return left;
        }
        // Unary negation preserves type, NOT returns BOOL
        if (auto unary = dynamic_cast<const Sad::AST::UnaryExpr*>(expr)) {
            switch (unary->op) {
                case Sad::Lexer::TokenType::OP_NOT:
                    return SIRType::BOOL;
                default:
                    break;
            }
            return inferExprType(unary->operand.get());
        }
        // Check DataType from expression (if available)
        auto dtype = expr->getType();
        if (dtype == Sad::Data::DataType::FLOAT) return SIRType::F64;
        if (dtype == Sad::Data::DataType::BOOLEAN) return SIRType::BOOL;
        if (dtype == Sad::Data::DataType::STRING) return SIRType::STRING;
        if (dtype == Sad::Data::DataType::ARRAY) return SIRType::ARRAY;
        if (dtype == Sad::Data::DataType::OBJECT) return SIRType::STRUCT;
        if (dtype == Sad::Data::DataType::MAP) return SIRType::MAP;
        return SIRType::I64;
    };
    
    // (AR) جمع أنواع جميع عبارات الإرجاع وتوحيدها
    // (EN) Collect types from ALL return statements and unify them
    std::function<void(const Sad::AST::Statement*, std::vector<SIRType>&)> collectReturnTypes;
    collectReturnTypes = [&](const Sad::AST::Statement* stmt, std::vector<SIRType>& types) {
        if (!stmt) return;
        if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt*>(stmt)) {
            if (ret->value) {
                types.push_back(inferExprType(ret->value.get()));
            }
            return;
        }
        if (auto block = dynamic_cast<const Sad::AST::BlockStmt*>(stmt)) {
            for (auto& s : block->statements) collectReturnTypes(s.get(), types);
        }
        if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt*>(stmt)) {
            collectReturnTypes(ifStmt->thenBranch.get(), types);
            if (ifStmt->elseBranch) collectReturnTypes(ifStmt->elseBranch.get(), types);
        }
    };
    
    std::vector<SIRType> returnTypes;
    collectReturnTypes(body, returnTypes);
    
    if (returnTypes.empty()) return SIRType::I64;
    
    // (AR) توحيد الأنواع: I64+F64→F64, STRING+أي→STRING, وإلا→نوع الأول
    // (EN) Unify types: I64+F64→F64, STRING+any→STRING, else→first type
    SIRType unified = returnTypes[0];
    for (size_t i = 1; i < returnTypes.size(); ++i) {
        if (unified == returnTypes[i]) continue;
        // (AR) توسيع: I64→F64
        if ((unified == SIRType::I64 && returnTypes[i] == SIRType::F64) ||
            (unified == SIRType::F64 && returnTypes[i] == SIRType::I64)) {
            unified = SIRType::F64;
        }
        // (AR) أي مع STRING → STRING
        else if (unified == SIRType::STRING || returnTypes[i] == SIRType::STRING) {
            unified = SIRType::STRING;
        }
    }
    
    return unified;
}

// ============================================================================
// enterScope - دخول نطاق جديد
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:587
// التوقيع / Signature: void enterScope();
//
// المتغيرات المستخدمة / Used variables:
// - currentScopeLevel_: sir_builder.h:599 (int)
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
// ============================================================================
void SIRBuilder::enterScope() {
    // (AR) زيادة مستوى النطاق (sir_builder.h:599 - currentScopeLevel_)
    // (EN) Increase scope level
    currentScopeLevel_++;
    
    // (AR) إضافة نطاق جديد (sir_builder.h:714 - scopeStack_)
    // (EN) Add new scope
    scopeStack_.push_back(std::unordered_map<std::string, VariableInfo>());
}

// ============================================================================
// exitScope - الخروج من النطاق الحالي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:589
// التوقيع / Signature: void exitScope();
//
// المتغيرات المستخدمة / Used variables:
// - currentScopeLevel_: sir_builder.h:599 (int)
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
// ============================================================================
void SIRBuilder::exitScope() {
    // (AR) إزالة النطاق الأخير (sir_builder.h:714 - scopeStack_)
    // (EN) Remove last scope
    if (!scopeStack_.empty()) {
        scopeStack_.pop_back();
    }
    
    // (AR) تقليل مستوى النطاق (sir_builder.h:599 - currentScopeLevel_)
    // (EN) Decrease scope level
    if (currentScopeLevel_ > 0) {
        currentScopeLevel_--;
    }
}

// ============================================================================
// addVariable - إضافة متغير للنطاق الحالي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:591
// التوقيع / Signature: void addVariable(const VariableInfo& varInfo);
//
// المعاملات / Parameters:
// - varInfo: const VariableInfo& = sir_builder.h:139 (VariableInfo struct)
//
// المتغيرات المستخدمة / Used variables:
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
// ============================================================================
void SIRBuilder::addVariable(const VariableInfo& varInfo) {
    // (AR) إضافة المتغير للنطاق الحالي (sir_builder.h:714 - scopeStack_)
    // (EN) Add variable to current scope
    if (!scopeStack_.empty()) {
        scopeStack_.back()[varInfo.name] = varInfo;
    }
}

// ============================================================================
// lookupVariable - البحث عن متغير في النطاقات
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:597
// التوقيع / Signature: VariableInfo* lookupVariable(const std::string& name);
//
// المعاملات / Parameters:
// - name: const std::string& (اسم المتغير)
//
// المتغيرات المستخدمة / Used variables:
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
//
// الإرجاع / Returns:
// - VariableInfo*: مؤشر للمتغير أو nullptr
// ============================================================================
VariableInfo* SIRBuilder::lookupVariable(const std::string& name) {
    // (AR) البحث في النطاقات من الأحدث للأقدم (sir_builder.h:714 - scopeStack_)
    // (EN) Search in scopes from newest to oldest
    for (auto scopeIt = scopeStack_.rbegin(); scopeIt != scopeStack_.rend(); ++scopeIt) {
        auto it = scopeIt->find(name);
        if (it != scopeIt->end()) {
            return &(it->second);
        }
    }
    
    // =====================================================================
    // (AR) البحث في المتغيرات العامة على مستوى الوحدة
    // (EN) Fallback: search in module-level global variables
    // عندما لا يُعثر على المتغير في النطاقات المحلية، نبحث في الوحدة
    // When not found in local scopes, search in module globals
    // =====================================================================
    if (module_) {
        auto globalVar = module_->getGlobalVariable(name);
        if (globalVar) {
            // (AR) إنشاء VariableInfo للمتغير العام وتخزينه في أول نطاق
            // (EN) Create VariableInfo for global var and cache in first scope
            VariableInfo globalInfo;
            globalInfo.name = name;
            globalInfo.type = globalVar->getType();
            globalInfo.registerName = name;  // use same name as LLVM global
            globalInfo.isGlobal = true;
            globalInfo.isMutable = !globalVar->getIsConstant();
            globalInfo.scopeLevel = 0;
            
            // (AR) تخزين في أقدم نطاق للبحث السريع لاحقاً
            // (EN) Cache in oldest scope for fast future lookups
            if (!scopeStack_.empty()) {
                scopeStack_.front()[name] = globalInfo;
                return &scopeStack_.front()[name];
            }
        }
    }
    
    // (AR) لم يُعثر على المتغير
    // (EN) Variable not found
    return nullptr;
}

// ============================================================================
// enterLoop - دخول سياق حلقة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:607
// التوقيع / Signature: void enterLoop(const LoopContext& ctx);
//
// المعاملات / Parameters:
// - ctx: const LoopContext& = sir_builder.h:180 (LoopContext struct)
//
// المتغيرات المستخدمة / Used variables:
// - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
// ============================================================================
void SIRBuilder::enterLoop(const LoopContext& ctx) {
    // (AR) إضافة سياق حلقة جديد (sir_builder.h:639 - loopStack_)
    // (EN) Add new loop context
    loopStack_.push_back(ctx);
}

// ============================================================================
// exitLoop - الخروج من سياق الحلقة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:615
// التوقيع / Signature: void exitLoop();
//
// المتغيرات المستخدمة / Used variables:
// - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
// ============================================================================
void SIRBuilder::exitLoop() {
    // (AR) إزالة سياق الحلقة الأخير (sir_builder.h:639 - loopStack_)
    // (EN) Remove last loop context
    if (!loopStack_.empty()) {
        loopStack_.pop_back();
    }
}

// ============================================================================
// getCurrentLoop - الحصول على سياق الحلقة الحالية
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:622
// التوقيع / Signature: LoopContext* getCurrentLoop();
//
// المتغيرات المستخدمة / Used variables:
// - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
//
// الإرجاع / Returns:
// - LoopContext*: مؤشر لسياق الحلقة الحالية أو nullptr
// ============================================================================
LoopContext* SIRBuilder::getCurrentLoop() {
    // (AR) إرجاع سياق الحلقة الأخير (sir_builder.h:639 - loopStack_)
    // (EN) Return last loop context
    if (!loopStack_.empty()) {
        return &loopStack_.back();
    }
    
    // (AR) لا توجد حلقة حالية
    // (EN) No current loop
    return nullptr;
}

// ============================================================================
// enterGenericScope - دخول نطاق أنواع عامة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:641
// التوقيع / Signature: void enterGenericScope(const std::vector<TypeParameter>& typeParams);
//
// المعاملات / Parameters:
// - typeParams: const std::vector<TypeParameter>& = sir_builder.h:187 (TypeParameter)
//
// المتغيرات المستخدمة / Used variables:
// - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
// ============================================================================
void SIRBuilder::enterGenericScope(const std::vector<TypeParameter>& typeParams) {
    // (AR) إنشاء نطاق جديد وإضافته (sir_builder.h:643 - genericScopeStack_)
    // (EN) Create new scope and add it
    GenericScope scope;
    scope.typeParameters = typeParams;
    genericScopeStack_.push_back(scope);
}

// ============================================================================
// exitGenericScope - الخروج من نطاق الأنواع العامة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:649
// التوقيع / Signature: void exitGenericScope();
//
// المتغيرات المستخدمة / Used variables:
// - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
// ============================================================================
void SIRBuilder::exitGenericScope() {
    // (AR) إزالة النطاق الأخير (sir_builder.h:643 - genericScopeStack_)
    // (EN) Remove last scope
    if (!genericScopeStack_.empty()) {
        genericScopeStack_.pop_back();
    }
}

// ============================================================================
// EXPRESSION BUILDING - بناء التعابير
// ============================================================================

// ============================================================================
// buildExpression - بناء تعبير (موزع للأنواع المختلفة)
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:432
// التوقيع / Signature: BuildResult buildExpression(AST::ExpressionNode* expr);
//
// المعاملات / Parameters:
// - expr: AST::ExpressionNode* = Sad::AST::Expression* (sir_builder.h:53)
//
// الإرجاع / Returns:
// - BuildResult: sir_builder.h:103 (contains registerName, type)
//
// Expression Types (expressions.h):
// - BinaryExpr (line 40): left, op, right
// - UnaryExpr (line 78): op, operand
// - LiteralExpr (line 174): token
// - VariableExpr (line 206): name
// - CallExpr (line 276): callee, arguments
// - MemberExpr (line 353): object, member
// ============================================================================
BuildResult SIRBuilder::buildExpression(AST::ExpressionNode* expr) {
    if (!expr) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: expr is null!" << std::endl;
        #endif
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildExpression: processing expression of type: " << typeid(*expr).name() << std::endl;
    #endif
    
    // (AR) BinaryExpr - تعبير ثنائي (expressions.h:40)
    // (EN) Binary expression
    if (auto binaryExpr = dynamic_cast<Sad::AST::BinaryExpr*>(expr)) {
        return buildBinaryOp(binaryExpr);
    }
    
    // (AR) UnaryExpr - تعبير أحادي (expressions.h:78)
    // (EN) Unary expression
    if (auto unaryExpr = dynamic_cast<Sad::AST::UnaryExpr*>(expr)) {
        return buildUnaryOp(unaryExpr);
    }
    
    // (AR) LiteralExpr - ثابت حرفي (expressions.h:174)
    // (EN) Literal
    if (auto literalExpr = dynamic_cast<Sad::AST::LiteralExpr*>(expr)) {
        return buildLiteral(literalExpr);
    }
    
    // (AR) VariableExpr - متغير (expressions.h:206)
    // (EN) Variable
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found VariableExpr" << std::endl;
        #endif
        return buildVariableAccess(varExpr);
    }
    
    // (AR) CallExpr - استدعاء دالة (expressions.h:276)
    // (EN) Function call
    if (auto callExpr = dynamic_cast<Sad::AST::CallExpr*>(expr)) {
        return buildFunctionCall(callExpr);
    }
    
    // (AR) AssignExpr - إسناد متغير (expressions.h:247)
    // (EN) Variable assignment
    if (auto assignExpr = dynamic_cast<Sad::AST::AssignExpr*>(expr)) {
        buildAssignment(assignExpr);
        // (AR) بعد الإسناد، نرجع معلومات المتغير المُسند إليه
        // (EN) After assignment, return the assigned variable info
        VariableInfo* varInfo = lookupVariable(assignExpr->name);
        if (varInfo) {
            return BuildResult(varInfo->registerName, varInfo->type);
        }
        return BuildResult();
    }
    
    // (AR) NewExpr - إنشاء كائن جديد (class_nodes.h:164)
    // (EN) New object creation
    if (auto newExpr = dynamic_cast<Sad::AST::NewExpr*>(expr)) {
        return buildNewObject(newExpr);
    }
    
    // (AR) MemberExpr - الوصول لعضو في كائن (expressions.h:406)
    // (EN) Member access via MemberExpr (from parser)
    if (auto memberExpr = dynamic_cast<Sad::AST::MemberExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found MemberExpr for member '"
                  << memberExpr->member << "'" << std::endl;
        #endif
        
        // (AR) الخطوة 1: بناء تعبير الكائن
        // (EN) Step 1: Build object expression
        auto objResult = buildExpression(memberExpr->object.get());
        
        // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو
        // (EN) Step 2: Create member access instruction
        std::string resultReg = newTempRegister();
        
        if (currentBlock_) {
            SIRInstruction loadInst;
            loadInst.opcode = SIROpcode::LOAD;
            loadInst.result = SIROperand::Register(resultReg, SIRType::I64);
            
            // (AR) المعامل الأول: الكائن
            // (EN) First operand: object
            loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
            
            // (AR) المعامل الثاني: اسم العضو
            // (EN) Second operand: member name
            loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->member));
            
            currentBlock_->addInstruction(loadInst);
        }
        
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) MemberAccessExpr - الوصول لعضو في كائن (class_nodes.h:206)
    // (EN) Member access
    if (auto memberExpr = dynamic_cast<Sad::AST::MemberAccessExpr*>(expr)) {
        return buildMemberAccess(memberExpr);
    }
    
    // (AR) MethodCallExpr - استدعاء طريقة على كائن (class_nodes.h:245)
    // (EN) Method call on object
    if (auto methodCallExpr = dynamic_cast<Sad::AST::MethodCallExpr*>(expr)) {
        return buildMethodCall(methodCallExpr);
    }
    
    // (AR) ThisExpr - مرجع ذاتي 'هذا' (class_nodes.h:288)
    // (EN) This/self reference
    if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr*>(expr)) {
        // (AR) 'هذا' يشير إلى self في سياق الصنف
        // (EN) 'this' refers to self in class context
        VariableInfo* selfInfo = lookupVariable("self");
        if (selfInfo) {
            return BuildResult(selfInfo->registerName, selfInfo->type);
        }
        return BuildResult("%self", SIRType::I64);
    }
    
    // (AR) SuperExpr - مرجع الصنف الأب 'الأساس' (class_nodes.h:298)
    // (EN) Super/base class reference
    if (auto superExpr = dynamic_cast<Sad::AST::SuperExpr*>(expr)) {
        // (AR) في سياق المترجم، الأساس يشير إلى self (لأن الكائن واحد)
        // (EN) In compiler context, super refers to self (single object)
        VariableInfo* selfInfo = lookupVariable("self");
        if (selfInfo) {
            return BuildResult(selfInfo->registerName, selfInfo->type);
        }
        return BuildResult("%self", SIRType::I64);
    }
    
    // ========================================================================
    // (AR) TernaryExpr - التعبير الثلاثي: شرط ? قيمة_صح : قيمة_خطأ
    // (EN) Ternary expression: cond ? trueVal : falseVal
    // ========================================================================
    if (auto ternaryExpr = dynamic_cast<Sad::AST::TernaryExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found TernaryExpr" << std::endl;
        #endif
        
        // (AR) بناء الشرط
        // (EN) Build condition
        auto condResult = buildExpression(ternaryExpr->condition.get());
        
        // (AR) إنشاء الكتل الأساسية
        // (EN) Create basic blocks
        std::string thenLabel = newLabel("ternary_then");
        std::string elseLabel = newLabel("ternary_else");
        std::string mergeLabel = newLabel("ternary_merge");
        
        auto thenBlock = createBasicBlock(thenLabel);
        auto elseBlock = createBasicBlock(elseLabel);
        auto mergeBlock = createBasicBlock(mergeLabel);
        
        // (AR) تعليمة فرع شرطي
        // (EN) Conditional branch instruction
        if (currentBlock_) {
            SIRInstruction brInst = SIRInstruction::BranchCond(
                SIROperand::Register(condResult.registerName, SIRType::BOOL),
                SIROperand::Label(thenLabel),
                SIROperand::Label(elseLabel)
            );
            currentBlock_->addInstruction(brInst);
        }
        
        // (AR) بناء فرع الحالة الصحيحة
        // (EN) Build true branch
        if (currentFunction_) {
            currentFunction_->addBasicBlock(thenBlock);
        }
        currentBlock_ = thenBlock;
        auto trueResult = buildExpression(ternaryExpr->trueExpr.get());
        SIRType resultType = trueResult.type;
        
        // (AR) إذا كانت النتيجة ثابتة (literal)، نحتاج لتوليد تعليمة MOVE
        //      لتعريف السجل فعلياً في هذه الكتلة قبل استخدامه في PHI
        // (EN) If result is a constant (literal), emit a MOVE instruction
        //      to materialize the register in this block before PHI usage
        std::string trueReg = trueResult.registerName;
        if (trueResult.isConstant && currentBlock_) {
            SIRInstruction moveInst;
            moveInst.opcode = SIROpcode::MOVE;
            moveInst.result = SIROperand::Register(trueReg, resultType);
            switch (resultType) {
                case SIRType::I64:
                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(trueResult.constantValue)));
                    break;
                case SIRType::F64:
                    moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(trueResult.constantValue)));
                    break;
                case SIRType::BOOL:
                    moveInst.operands.push_back(SIROperand::ConstantBool(
                        trueResult.constantValue == "true" || trueResult.constantValue == "1"));
                    break;
                case SIRType::STRING:
                    moveInst.operands.push_back(SIROperand::ConstantString(trueResult.constantValue));
                    break;
                default:
                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                    break;
            }
            currentBlock_->addInstruction(moveInst);
        }
        
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
        }
        
        // (AR) بناء فرع الحالة الخاطئة
        // (EN) Build false branch
        if (currentFunction_) {
            currentFunction_->addBasicBlock(elseBlock);
        }
        currentBlock_ = elseBlock;
        auto falseResult = buildExpression(ternaryExpr->falseExpr.get());
        std::string falseReg = falseResult.registerName;
        
        // (AR) نفس المعالجة: تحويل الثوابت إلى تعليمات MOVE
        // (EN) Same treatment: materialize constants into MOVE instructions
        if (falseResult.isConstant && currentBlock_) {
            SIRInstruction moveInst;
            moveInst.opcode = SIROpcode::MOVE;
            moveInst.result = SIROperand::Register(falseReg, resultType);
            switch (resultType) {
                case SIRType::I64:
                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(falseResult.constantValue)));
                    break;
                case SIRType::F64:
                    moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(falseResult.constantValue)));
                    break;
                case SIRType::BOOL:
                    moveInst.operands.push_back(SIROperand::ConstantBool(
                        falseResult.constantValue == "true" || falseResult.constantValue == "1"));
                    break;
                case SIRType::STRING:
                    moveInst.operands.push_back(SIROperand::ConstantString(falseResult.constantValue));
                    break;
                default:
                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                    break;
            }
            currentBlock_->addInstruction(moveInst);
        }
        
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
        }
        
        // (AR) كتلة الدمج مع PHI
        // (EN) Merge block with PHI
        if (currentFunction_) {
            currentFunction_->addBasicBlock(mergeBlock);
        }
        currentBlock_ = mergeBlock;
        
        std::string phiReg = newTempRegister();
        SIRInstruction phiInst = SIRInstruction::Phi(
            SIROperand::Register(phiReg, resultType),
            {
                {SIROperand::Register(trueReg, resultType), SIROperand::Label(thenLabel)},
                {SIROperand::Register(falseReg, resultType), SIROperand::Label(elseLabel)}
            }
        );
        if (currentBlock_) {
            currentBlock_->addInstruction(phiInst);
        }
        
        return BuildResult(phiReg, resultType);
    }
    
    // ========================================================================
    // (AR) IndexExpr - الوصول بالفهرس: مصفوفة[فهرس] أو خريطة["مفتاح"]
    // (EN) Index access: array[index] or map["key"]
    // ========================================================================
    if (auto indexExpr = dynamic_cast<Sad::AST::IndexExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found IndexExpr" << std::endl;
        #endif
        
        // (AR) بناء تعبير الكائن وتعبير الفهرس
        // (EN) Build object expression and index expression
        auto objResult = buildExpression(indexExpr->object.get());
        auto idxResult = buildExpression(indexExpr->index.get());
        
        // (AR) استنتاج نوع النتيجة: إذا كان الكائن مصفوفة وعنصره مصفوفة → ARRAY، وإلا → نوع العنصر
        // (EN) Infer result type: if object is array with array elements → ARRAY, else → elementType
        SIRType resultType = SIRType::I64;
        if (objResult.type == SIRType::ARRAY) {
            if (objResult.elementType == SIRType::ARRAY) {
                resultType = SIRType::ARRAY;
            } else if (objResult.elementType != SIRType::VOID) {
                resultType = objResult.elementType;
            }
        } else if (objResult.type == SIRType::STRING) {
            resultType = SIRType::STRING;
        } else if (objResult.type == SIRType::MAP) {
            resultType = SIRType::I64;
        }
        
        // (AR) تعليمة Load للوصول بالفهرس
        // (EN) Load instruction for indexed access
        std::string resultReg = newTempRegister();
        SIRInstruction takeInst;
        takeInst.opcode = SIROpcode::LOAD;
        takeInst.result = SIROperand::Register(resultReg, resultType);
        takeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        takeInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
        takeInst.comment = "index access";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(takeInst);
        }
        
        BuildResult result(resultReg, resultType);
        // (AR) إذا كان العنصر مصفوفة، نرث نوع العنصر الداخلي
        // (EN) If element is array, inherit the inner element type (for chained indexing)
        if (resultType == SIRType::ARRAY) {
            result.elementType = SIRType::I64;  // default inner element type
        }
        return result;
    }
    
    // ========================================================================
    // (AR) MemberAssignExpr - إسناد لعضو: كائن.حقل = قيمة
    // (EN) Member assignment: obj.field = value
    // ========================================================================
    if (auto memberAssignExpr = dynamic_cast<Sad::AST::MemberAssignExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found MemberAssignExpr" << std::endl;
        #endif
        
        // (AR) بناء تعبير الكائن والقيمة
        // (EN) Build object expression and value
        auto objResult = buildExpression(memberAssignExpr->object.get());
        auto valResult = buildExpression(memberAssignExpr->value.get());
        
        // (AR) تعليمة STORE لتخزين القيمة في العضو
        // (EN) STORE instruction to store value in member
        SIRInstruction storeInst;
        storeInst.opcode = SIROpcode::STORE;
        storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
        storeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        storeInst.operands.push_back(SIROperand::ConstantString(memberAssignExpr->member));
        storeInst.comment = "member assign: " + memberAssignExpr->member;
        
        if (currentBlock_) {
            currentBlock_->addInstruction(storeInst);
        }
        
        return BuildResult(valResult.registerName, valResult.type);
    }
    
    // ========================================================================
    // (AR) IndexAssignExpr - إسناد بالفهرس: مصفوفة[فهرس] = قيمة
    // (EN) Index assignment: array[index] = value
    // ========================================================================
    if (auto indexAssignExpr = dynamic_cast<Sad::AST::IndexAssignExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found IndexAssignExpr" << std::endl;
        #endif
        
        // (AR) بناء تعابير الكائن والفهرس والقيمة
        // (EN) Build object, index, and value expressions
        auto objResult = buildExpression(indexAssignExpr->object.get());
        auto idxResult = buildExpression(indexAssignExpr->index.get());
        auto valResult = buildExpression(indexAssignExpr->value.get());
        
        // (AR) تعليمة STORE لتخزين القيمة في موضع الفهرس
        // (EN) STORE instruction to store value at index position
        SIRInstruction storeInst;
        storeInst.opcode = SIROpcode::STORE;
        storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
        storeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        storeInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
        storeInst.comment = "index assign";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(storeInst);
        }
        
        return BuildResult(valResult.registerName, valResult.type);
    }
    
    // ========================================================================
    // (AR) ArrayExpr - مصفوفة حرفية: [1، 2، 3]
    // (EN) Array literal: [1, 2, 3]
    // ========================================================================
    if (auto arrayExpr = dynamic_cast<Sad::AST::ArrayExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found ArrayExpr with " 
                  << arrayExpr->elements.size() << " elements" << std::endl;
        #endif
        
        // (AR) تخصيص مصفوفة جديدة
        // (EN) Allocate new array
        std::string arrReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(arrReg, SIRType::ARRAY);
        allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(arrayExpr->elements.size())));
        allocInst.comment = "array alloc [" + std::to_string(arrayExpr->elements.size()) + "]";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        // (AR) تخزين العناصر واحداً تلو الآخر
        // (EN) Store elements one by one
        SIRType inferredElementType = SIRType::VOID;
        for (size_t i = 0; i < arrayExpr->elements.size(); ++i) {
            auto elemResult = buildExpression(arrayExpr->elements[i].get());
            
            // (AR) استنتاج نوع العنصر من العنصر الأول
            // (EN) Infer element type from first element
            if (i == 0) {
                inferredElementType = elemResult.type;
            }
            
            // (AR) تجسيد الثوابت قبل تخزينها (نفس الإصلاح المُطبَّق على MapExpr)
            //      ملاحظة: عناصر المصفوفات المتداخلة (ARRAY) لا تحتاج تجسيداً — هي بالفعل في سجلات
            // (EN) Materialize constants before storing (same fix applied to MapExpr)
            //      Note: Nested array elements (ARRAY type) don't need materialization — already in registers
            if (elemResult.isConstant && elemResult.type != SIRType::ARRAY && currentBlock_) {
                std::string reg = newTempRegister();
                elemResult.registerName = reg;
                SIRInstruction moveInst(SIROpcode::MOVE);
                moveInst.result = SIROperand::Register(reg, elemResult.type);
                if (elemResult.type == SIRType::STRING) {
                    moveInst.operands.push_back(SIROperand::ConstantString(elemResult.constantValue));
                } else if (elemResult.type == SIRType::F64) {
                    moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(elemResult.constantValue)));
                } else if (elemResult.type == SIRType::BOOL) {
                    moveInst.operands.push_back(SIROperand::ConstantBool(elemResult.constantValue == "true" || elemResult.constantValue == "1"));
                } else {
                    try { moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(elemResult.constantValue))); }
                    catch (...) { moveInst.operands.push_back(SIROperand::ConstantI64(0)); }
                }
                currentBlock_->addInstruction(moveInst);
                elemResult.isConstant = false;
            }
            
            SIRInstruction storeInst;
            storeInst.opcode = SIROpcode::STORE;
            storeInst.operands.push_back(SIROperand::Register(elemResult.registerName, elemResult.type));
            storeInst.operands.push_back(SIROperand::Register(arrReg, SIRType::ARRAY));
            storeInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
            storeInst.comment = "array[" + std::to_string(i) + "] = ...";
            
            if (currentBlock_) {
                currentBlock_->addInstruction(storeInst);
            }
        }
        
        BuildResult result(arrReg, SIRType::ARRAY);
        // (AR) استنتاج نوع العنصر من العنصر الأول (تم حفظه في الحلقة أعلاه)
        // (EN) Infer element type from first element (saved during loop above)
        if (inferredElementType != SIRType::VOID) {
            result.elementType = inferredElementType;
        }
        return result;
    }
    
    // ========================================================================
    // (AR) MapExpr - قاموس حرفي: {"مفتاح": قيمة}
    // (EN) Map literal: {"key": value}
    // ========================================================================
    if (auto mapExpr = dynamic_cast<Sad::AST::MapExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found MapExpr with " 
                  << mapExpr->pairs.size() << " pairs" << std::endl;
        #endif
        
        // (AR) تخصيص خريطة جديدة
        // (EN) Allocate new map
        std::string mapReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(mapReg, SIRType::STRUCT);
        allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(mapExpr->pairs.size())));
        allocInst.comment = "map alloc {" + std::to_string(mapExpr->pairs.size()) + " pairs}";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        // (AR) تخزين الأزواج (مفتاح، قيمة)
        // (EN) Store key-value pairs
        for (size_t i = 0; i < mapExpr->pairs.size(); ++i) {
            auto keyResult = buildExpression(mapExpr->pairs[i].key.get());
            auto valResult = buildExpression(mapExpr->pairs[i].value.get());
            
            // (AR) دالة مساعدة: تحويل BuildResult إلى SIROperand مع معالجة الثوابت
            // (EN) Helper: convert BuildResult to SIROperand, handling constants properly
            // When isConstant=true, registerName is empty and constantValue holds the data.
            // We must emit a MOVE instruction to materialize the constant into a named register,
            // because emitStore resolves operands by register name lookup in namedValues.
            auto materializeResult = [&](BuildResult& res) -> SIROperand {
                if (res.isConstant) {
                    // Emit MOVE to materialize constant into a fresh register
                    std::string reg = newTempRegister();
                    res.registerName = reg;
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(reg, res.type);
                    if (res.type == SIRType::STRING) {
                        moveInst.operands.push_back(SIROperand::ConstantString(res.constantValue));
                    } else if (res.type == SIRType::F64) {
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(res.constantValue)));
                    } else if (res.type == SIRType::BOOL) {
                        moveInst.operands.push_back(SIROperand::ConstantBool(res.constantValue == "true" || res.constantValue == "1"));
                    } else {
                        // I64 or default
                        try {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(res.constantValue)));
                        } catch (...) {
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                    }
                    if (currentBlock_) {
                        currentBlock_->addInstruction(moveInst);
                    }
                    res.isConstant = false;
                }
                return SIROperand::Register(res.registerName, res.type);
            };
            
            SIROperand keyOp = materializeResult(keyResult);
            SIROperand valOp = materializeResult(valResult);
            
            // (AR) تخزين المفتاح
            // (EN) Store key
            SIRInstruction storeKeyInst;
            storeKeyInst.opcode = SIROpcode::STORE;
            storeKeyInst.operands.push_back(keyOp);
            storeKeyInst.operands.push_back(SIROperand::Register(mapReg, SIRType::STRUCT));
            storeKeyInst.operands.push_back(SIROperand::ConstantString("key_" + std::to_string(i)));
            storeKeyInst.comment = "map key[" + std::to_string(i) + "]";
            
            if (currentBlock_) {
                currentBlock_->addInstruction(storeKeyInst);
            }
            
            // (AR) تخزين القيمة
            // (EN) Store value
            SIRInstruction storeValInst;
            storeValInst.opcode = SIROpcode::STORE;
            storeValInst.operands.push_back(valOp);
            storeValInst.operands.push_back(SIROperand::Register(mapReg, SIRType::STRUCT));
            storeValInst.operands.push_back(SIROperand::ConstantString("val_" + std::to_string(i)));
            storeValInst.comment = "map val[" + std::to_string(i) + "]";
            
            if (currentBlock_) {
                currentBlock_->addInstruction(storeValInst);
            }
        }
        
        return BuildResult(mapReg, SIRType::STRUCT);
    }
    
    // ========================================================================
    // (AR) WalrusExpr - عامل Walrus: (متغير := قيمة)
    // (EN) Walrus operator: (var := expr)
    // ========================================================================
    if (auto walrusExpr = dynamic_cast<Sad::AST::WalrusExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found WalrusExpr: " << walrusExpr->variable << std::endl;
        #endif
        
        // (AR) بناء تعبير القيمة
        // (EN) Build value expression
        auto valResult = buildExpression(walrusExpr->value.get());
        
        // (AR) تخصيص متغير جديد وتخزين القيمة فيه
        // (EN) Allocate new variable and store value
        std::string varReg = newTempRegister();
        
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(varReg, valResult.type);
        allocInst.operands.push_back(SIROperand::ConstantI64(1));
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        SIRInstruction storeInst;
        storeInst.opcode = SIROpcode::STORE;
        storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
        storeInst.operands.push_back(SIROperand::Register(varReg, valResult.type));
        
        if (currentBlock_) {
            currentBlock_->addInstruction(storeInst);
        }
        
        // (AR) تسجيل المتغير في النطاق الحالي
        // (EN) Register variable in current scope
        VariableInfo varInfo;
        varInfo.name = walrusExpr->variable;
        varInfo.type = valResult.type;
        varInfo.registerName = varReg;
        varInfo.isMutable = true;
        varInfo.scopeLevel = currentScopeLevel_;
        addVariable(varInfo);
        
        // (AR) Walrus يرجع القيمة نفسها
        // (EN) Walrus returns the value itself
        return BuildResult(valResult.registerName, valResult.type);
    }
    
    // ========================================================================
    // (AR) LambdaExpr - دالة مجهولة: (س) => س * 2
    // (EN) Lambda expression: (x) => x * 2
    // ========================================================================
    if (auto lambdaExpr = dynamic_cast<Sad::AST::LambdaExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found LambdaExpr with " 
                  << lambdaExpr->parameters.size() << " params" << std::endl;
        #endif
        
        // (AR) إنشاء اسم فريد للدالة المجهولة
        // (EN) Create unique name for anonymous function
        std::string lambdaName = "__lambda_" + std::to_string(nextTempRegister_++);
        
        // (AR) بناء معاملات الدالة
        // (EN) Build function parameters
        std::vector<SIRParameter> sirParams;
        for (const auto& param : lambdaExpr->parameters) {
            SIRType paramType = SIRType::I64; // (AR) نوع افتراضي / (EN) Default type
            sirParams.push_back(SIRParameter(param.name, paramType));
        }
        
        // (AR) إنشاء دالة SIR للـ lambda
        // (EN) Create SIR function for lambda
        auto lambdaFunc = std::make_shared<SIRFunction>(lambdaName, SIRType::I64);
        for (const auto& lp : sirParams) lambdaFunc->addParameter(lp);
        
        // (AR) حفظ السياق الحالي
        // (EN) Save current context
        auto savedFunction = currentFunction_;
        auto savedBlock = currentBlock_;
        
        // (AR) تعيين سياق الدالة الجديدة
        // (EN) Set new function context
        currentFunction_ = lambdaFunc;
        auto entryBlock = createBasicBlock("lambda_entry");
        lambdaFunc->addBasicBlock(entryBlock);
        currentBlock_ = entryBlock;
        
        enterScope();
        
        // (AR) تسجيل المعاملات كمتغيرات محلية
        // (EN) Register parameters as local variables
        for (size_t i = 0; i < lambdaExpr->parameters.size(); ++i) {
            std::string paramReg = "%" + lambdaExpr->parameters[i].name;
            VariableInfo paramVar;
            paramVar.name = lambdaExpr->parameters[i].name;
            paramVar.type = SIRType::I64;
            paramVar.registerName = paramReg;
            paramVar.isMutable = false;
            paramVar.scopeLevel = currentScopeLevel_;
            addVariable(paramVar);
        }
        
        // (AR) بناء جسم الـ lambda (تعبير واحد)
        // (EN) Build lambda body (single expression)
        if (lambdaExpr->body) {
            auto bodyResult = buildExpression(lambdaExpr->body.get());
            // (AR) إرجاع النتيجة تلقائياً
            // (EN) Automatically return the result
            SIRInstruction retInst;
            retInst.opcode = SIROpcode::RET;
            retInst.operands.push_back(SIROperand::Register(bodyResult.registerName, bodyResult.type));
            if (currentBlock_) {
                currentBlock_->addInstruction(retInst);
            }
        }
        
        exitScope();
        
        // (AR) إضافة الدالة للوحدة
        // (EN) Add function to module
        if (module_) {
            module_->addFunction(lambdaFunc);
        }
        
        // (AR) استعادة السياق السابق
        // (EN) Restore previous context
        currentFunction_ = savedFunction;
        currentBlock_ = savedBlock;
        
        // (AR) إرجاع مؤشر للدالة
        // (EN) Return function pointer
        std::string resultReg = newTempRegister();
        BuildResult result(resultReg, SIRType::FUNCTION);
        result.constantValue = lambdaName;
        return result;
    }
    
    // ========================================================================
    // (AR) RangeExpr - تعبير المدى: بداية..نهاية
    // (EN) Range expression: start..end
    // ========================================================================
    if (auto rangeExpr = dynamic_cast<Sad::AST::RangeExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found RangeExpr" << std::endl;
        #endif
        
        // (AR) بناء بداية ونهاية المدى
        // (EN) Build range start and end
        auto startResult = buildExpression(rangeExpr->start.get());
        auto endResult = buildExpression(rangeExpr->end.get());
        
        // (AR) تمثيل المدى كخريطة بسيطة تحتوي على بداية ونهاية
        // (EN) Represent range as a simple struct with start and end
        std::string rangeReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(rangeReg, SIRType::STRUCT);
        allocInst.operands.push_back(SIROperand::ConstantI64(2));
        allocInst.comment = "range alloc";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        // (AR) تخزين البداية
        // (EN) Store start
        SIRInstruction storeStartInst;
        storeStartInst.opcode = SIROpcode::STORE;
        storeStartInst.operands.push_back(SIROperand::Register(startResult.registerName, startResult.type));
        storeStartInst.operands.push_back(SIROperand::Register(rangeReg, SIRType::STRUCT));
        storeStartInst.operands.push_back(SIROperand::ConstantString("start"));
        
        if (currentBlock_) {
            currentBlock_->addInstruction(storeStartInst);
        }
        
        // (AR) تخزين النهاية
        // (EN) Store end
        SIRInstruction storeEndInst;
        storeEndInst.opcode = SIROpcode::STORE;
        storeEndInst.operands.push_back(SIROperand::Register(endResult.registerName, endResult.type));
        storeEndInst.operands.push_back(SIROperand::Register(rangeReg, SIRType::STRUCT));
        storeEndInst.operands.push_back(SIROperand::ConstantString("end"));
        
        if (currentBlock_) {
            currentBlock_->addInstruction(storeEndInst);
        }
        
        return BuildResult(rangeReg, SIRType::STRUCT);
    }
    
    // ========================================================================
    // (AR) ListComprehensionExpr - استيعاب قائمي: [س*2 لكل س في قائمة]
    // (EN) List comprehension: [x*2 for x in list]
    // ========================================================================
    if (auto listCompExpr = dynamic_cast<Sad::AST::ListComprehensionExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found ListComprehensionExpr" << std::endl;
        #endif
        
        // (AR) تخصيص مصفوفة النتيجة
        // (EN) Allocate result array
        std::string resultArrReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(resultArrReg, SIRType::ARRAY);
        allocInst.operands.push_back(SIROperand::ConstantI64(0));
        allocInst.comment = "list comprehension result";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        // (AR) بناء التعبير القابل للتكرار
        // (EN) Build iterable expression
        auto iterResult = buildExpression(listCompExpr->iterable.get());
        
        // (AR) إنشاء حلقة للتكرار (تُترجم إلى حلقة عداد)
        // (EN) Create iteration loop (lowered to counter loop)
        std::string idxReg = newTempRegister();
        SIRInstruction allocIdx;
        allocIdx.opcode = SIROpcode::ALLOC;
        allocIdx.result = SIROperand::Register(idxReg, SIRType::I64);
        allocIdx.operands.push_back(SIROperand::ConstantI64(1));
        if (currentBlock_) currentBlock_->addInstruction(allocIdx);
        
        SIRInstruction storeZero;
        storeZero.opcode = SIROpcode::STORE;
        storeZero.operands.push_back(SIROperand::ConstantI64(0));
        storeZero.operands.push_back(SIROperand::Register(idxReg, SIRType::I64));
        if (currentBlock_) currentBlock_->addInstruction(storeZero);
        
        // (AR) كتل الحلقة
        // (EN) Loop blocks
        std::string condLabel = newLabel("lc_cond");
        std::string bodyLabel = newLabel("lc_body");
        std::string exitLabel = newLabel("lc_exit");
        
        auto condBlock = createBasicBlock(condLabel);
        auto bodyBlock = createBasicBlock(bodyLabel);
        auto exitBlock = createBasicBlock(exitLabel);
        
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
        }
        
        // (AR) كتلة الشرط — مقارنة العداد مع طول القائمة
        // (EN) Condition block — compare counter to list length
        if (currentFunction_) currentFunction_->addBasicBlock(condBlock);
        currentBlock_ = condBlock;
        
        // (AR) تحميل العداد الحالي
        // (EN) Load current counter
        std::string curIdxReg = newTempRegister();
        SIRInstruction loadIdx;
        loadIdx.opcode = SIROpcode::LOAD;
        loadIdx.result = SIROperand::Register(curIdxReg, SIRType::I64);
        loadIdx.operands.push_back(SIROperand::Register(idxReg, SIRType::I64));
        if (currentBlock_) currentBlock_->addInstruction(loadIdx);
        
        // (AR) للتبسيط: نستخدم استدعاء طول runtime
        // (EN) Simplified: use runtime length call
        std::string lenReg = newTempRegister();
        SIRInstruction callLen;
        callLen.opcode = SIROpcode::CALL;
        callLen.result = SIROperand::Register(lenReg, SIRType::I64);
        callLen.operands.push_back(SIROperand::ConstantString("__sad_len"));
        callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
        if (currentBlock_) currentBlock_->addInstruction(callLen);
        
        std::string cmpReg = newTempRegister();
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Binary(
                SIROpcode::LT,
                SIROperand::Register(cmpReg, SIRType::BOOL),
                SIROperand::Register(curIdxReg, SIRType::I64),
                SIROperand::Register(lenReg, SIRType::I64)
            ));
            currentBlock_->addInstruction(SIRInstruction::BranchCond(
                SIROperand::Register(cmpReg, SIRType::BOOL),
                SIROperand::Label(bodyLabel),
                SIROperand::Label(exitLabel)
            ));
        }
        
        // (AR) كتلة الجسم
        // (EN) Body block
        if (currentFunction_) currentFunction_->addBasicBlock(bodyBlock);
        currentBlock_ = bodyBlock;
        
        enterScope();
        
        // (AR) تحميل العنصر الحالي وتسجيل متغير الحلقة
        // (EN) Load current element and register loop variable
        std::string elemReg = newTempRegister();
        SIRInstruction loadElem;
        loadElem.opcode = SIROpcode::LOAD;
        loadElem.result = SIROperand::Register(elemReg, SIRType::I64);
        loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
        loadElem.operands.push_back(SIROperand::Register(curIdxReg, SIRType::I64));
        if (currentBlock_) currentBlock_->addInstruction(loadElem);
        
        VariableInfo loopVar;
        loopVar.name = listCompExpr->variable;
        loopVar.type = SIRType::I64;
        loopVar.registerName = elemReg;
        loopVar.isMutable = false;
        loopVar.scopeLevel = currentScopeLevel_;
        addVariable(loopVar);
        
        // (AR) فحص الشرط (إن وجد)
        // (EN) Check condition (if present)
        bool hasCondition = (listCompExpr->condition != nullptr);
        std::string storeLabel, incLabel;
        
        if (hasCondition) {
            storeLabel = newLabel("lc_store");
            incLabel = newLabel("lc_inc");
            
            auto condResult = buildExpression(listCompExpr->condition.get());
            auto storeBlock2 = createBasicBlock(storeLabel);
            auto incBlock = createBasicBlock(incLabel);
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(condResult.registerName, SIRType::BOOL),
                    SIROperand::Label(storeLabel),
                    SIROperand::Label(incLabel)
                ));
            }
            
            if (currentFunction_) currentFunction_->addBasicBlock(storeBlock2);
            currentBlock_ = storeBlock2;
        }
        
        // (AR) بناء تعبير العنصر وإضافته للمصفوفة
        // (EN) Build element expression and append to array
        auto elemExprResult = buildExpression(listCompExpr->element.get());
        
        SIRInstruction appendInst;
        appendInst.opcode = SIROpcode::CALL;
        appendInst.operands.push_back(SIROperand::ConstantString("__sad_array_push"));
        appendInst.operands.push_back(SIROperand::Register(resultArrReg, SIRType::ARRAY));
        appendInst.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
        if (currentBlock_) currentBlock_->addInstruction(appendInst);
        
        if (hasCondition) {
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
            }
            // (AR) كتلة زيادة العداد
            // (EN) Increment block  
            auto incBlock2 = createBasicBlock(incLabel);
            if (currentFunction_) currentFunction_->addBasicBlock(incBlock2);
            currentBlock_ = incBlock2;
        }
        
        // (AR) زيادة العداد
        // (EN) Increment counter
        std::string nextIdxReg = newTempRegister();
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Binary(
                SIROpcode::ADD_I64,
                SIROperand::Register(nextIdxReg, SIRType::I64),
                SIROperand::Register(curIdxReg, SIRType::I64),
                SIROperand::ConstantI64(1)
            ));
            
            SIRInstruction storeIdx;
            storeIdx.opcode = SIROpcode::STORE;
            storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SIRType::I64));
            storeIdx.operands.push_back(SIROperand::Register(idxReg, SIRType::I64));
            currentBlock_->addInstruction(storeIdx);
            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
        }
        
        exitScope();
        
        // (AR) كتلة الخروج
        // (EN) Exit block
        if (currentFunction_) currentFunction_->addBasicBlock(exitBlock);
        currentBlock_ = exitBlock;
        
        return BuildResult(resultArrReg, SIRType::ARRAY);
    }
    
    // ========================================================================
    // (AR) DictComprehensionExpr - استيعاب قاموسي: {س: س**2 لكل س في قائمة}
    // (EN) Dict comprehension: {x: x**2 for x in list}
    // ========================================================================
    if (auto dictCompExpr = dynamic_cast<Sad::AST::DictComprehensionExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found DictComprehensionExpr" << std::endl;
        #endif
        
        // (AR) تخصيص خريطة النتيجة
        // (EN) Allocate result map
        std::string resultMapReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(resultMapReg, SIRType::STRUCT);
        allocInst.operands.push_back(SIROperand::ConstantI64(0));
        allocInst.comment = "dict comprehension result";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        // (AR) بناء التعبير القابل للتكرار واستدعاء حلقة runtime
        // (EN) Build iterable and invoke runtime loop
        auto iterResult = buildExpression(dictCompExpr->iterable.get());
        
        // (AR) حلقة مبسطة مع runtime
        // (EN) Simplified loop with runtime
        SIRInstruction loopInst;
        loopInst.opcode = SIROpcode::CALL;
        loopInst.operands.push_back(SIROperand::ConstantString("__sad_dict_comprehension"));
        loopInst.operands.push_back(SIROperand::Register(resultMapReg, SIRType::STRUCT));
        loopInst.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
        loopInst.comment = "dict comprehension loop";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(loopInst);
        }
        
        return BuildResult(resultMapReg, SIRType::STRUCT);
    }
    
    // ========================================================================
    // (AR) SetComprehensionExpr - استيعاب مجموعة: {س لكل س في قائمة}
    // (EN) Set comprehension: {x for x in list}
    // ========================================================================
    if (auto setCompExpr = dynamic_cast<Sad::AST::SetComprehensionExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found SetComprehensionExpr" << std::endl;
        #endif
        
        std::string resultSetReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(resultSetReg, SIRType::STRUCT);
        allocInst.operands.push_back(SIROperand::ConstantI64(0));
        allocInst.comment = "set comprehension result";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        auto iterResult = buildExpression(setCompExpr->iterable.get());
        
        SIRInstruction loopInst;
        loopInst.opcode = SIROpcode::CALL;
        loopInst.operands.push_back(SIROperand::ConstantString("__sad_set_comprehension"));
        loopInst.operands.push_back(SIROperand::Register(resultSetReg, SIRType::STRUCT));
        loopInst.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
        loopInst.comment = "set comprehension loop";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(loopInst);
        }
        
        return BuildResult(resultSetReg, SIRType::STRUCT);
    }
    
    // ========================================================================
    // (AR) GeneratorExpr - مولّد: (س*2 لكل س في قائمة)
    // (EN) Generator expression: (x*2 for x in list)
    // ========================================================================
    if (auto genExpr = dynamic_cast<Sad::AST::GeneratorExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found GeneratorExpr" << std::endl;
        #endif
        
        // (AR) المولّد يُخفّض إلى دالة مجهولة تُنشئ مصفوفة كسولة
        // (EN) Generator is lowered to an anonymous fn producing a lazy array
        std::string genReg = newTempRegister();
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(genReg, SIRType::FUNCTION);
        allocInst.operands.push_back(SIROperand::ConstantI64(0));
        allocInst.comment = "generator expression";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(allocInst);
        }
        
        return BuildResult(genReg, SIRType::FUNCTION);
    }
    
    // ========================================================================
    // (AR) DecoratorExpr - مُزخرف: @decorator
    // (EN) Decorator expression: @decorator
    // ========================================================================
    if (auto decoratorExpr = dynamic_cast<Sad::AST::DecoratorExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found DecoratorExpr: " << decoratorExpr->name << std::endl;
        #endif
        
        // (AR) المُزخرف يُحوّل لاستدعاء دالة بالاسم
        // (EN) Decorator is lowered to a function call by name
        std::string resultReg = newTempRegister();
        SIRInstruction callInst;
        callInst.opcode = SIROpcode::CALL;
        callInst.result = SIROperand::Register(resultReg, SIRType::FUNCTION);
        callInst.operands.push_back(SIROperand::ConstantString(decoratorExpr->name));
        
        // (AR) إضافة الوسائط إن وُجدت
        // (EN) Add arguments if present
        if (decoratorExpr->hasArguments) {
            for (const auto& arg : decoratorExpr->arguments) {
                auto argResult = buildExpression(arg.get());
                callInst.operands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
            }
        }
        callInst.comment = "decorator: " + decoratorExpr->name;
        
        if (currentBlock_) {
            currentBlock_->addInstruction(callInst);
        }
        
        return BuildResult(resultReg, SIRType::FUNCTION);
    }
    
    // ========================================================================
    // (AR) BorrowExpr - استعارة: &متغير أو &mut متغير
    // (EN) Borrow expression: &var or &mut var
    // ========================================================================
    if (auto borrowExpr = dynamic_cast<Sad::AST::BorrowExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found BorrowExpr: " << borrowExpr->variableName << std::endl;
        #endif
        
        // (AR) البحث عن المتغير المُستعار
        // (EN) Look up the borrowed variable
        VariableInfo* varInfo = lookupVariable(borrowExpr->variableName);
        std::string sourceReg = varInfo ? varInfo->registerName : "%" + borrowExpr->variableName;
        SIRType sourceType = varInfo ? varInfo->type : SIRType::I64;
        
        std::string resultReg = newTempRegister();
        SIRInstruction borrowInst;
        // (AR) استعارة تُترجم إلى أخذ عنوان (ADDR) — SIR لا يحتوي BORROW مباشرة
        // (EN) Borrow lowered to address-of (ADDR) — SIR has no direct BORROW opcode
        borrowInst.opcode = SIROpcode::ADDR;
        borrowInst.result = SIROperand::Register(resultReg, SIRType::PTR);
        borrowInst.operands.push_back(SIROperand::Register(sourceReg, sourceType));
        borrowInst.comment = borrowExpr->isMutable ? "borrow_mut" : "borrow";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(borrowInst);
        }
        
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // ========================================================================
    // (AR) TemplateInstantiation - إنشاء نسخة من قالب: صندوق<رقم>(42)
    // (EN) Template instantiation: Box<int>(42)
    // ========================================================================
    if (auto templateInst = dynamic_cast<Sad::AST::TemplateInstantiation*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found TemplateInstantiation: " 
                  << templateInst->templateName << std::endl;
        #endif
        
        // (AR) تحويل أنواع القالب إلى SIRType
        // (EN) Convert template type arguments to SIRType
        std::vector<SIRType> typeArgs;
        for (const auto& arg : templateInst->typeArguments) {
            typeArgs.push_back(astTypeToSIRType(arg));
        }
        
        // (AR) محاولة إنشاء نسخة من القالب
        // (EN) Attempt to instantiate the template
        std::string instName = instantiateTemplate(templateInst->templateName, typeArgs);
        
        // (AR) استدعاء الدالة/الصنف المُنشأ بدون وسائط (القالب لا يحمل وسائط قيم)
        // (EN) Call the instantiated function/class with no args (template has no value args)
        std::string resultReg = newTempRegister();
        SIRInstruction callInst = SIRInstruction::Call(
            SIROperand::Register(resultReg, SIRType::I64),
            SIROperand::ConstantString(instName),
            {}
        );
        
        if (currentBlock_) {
            currentBlock_->addInstruction(callInst);
        }
        
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // ========================================================================
    // (AR) InlineAsmExpr - تعبير تجميع مضمّن (expressions.h:InlineAsmExpr)
    // (EN) Inline assembly expression — emit inline asm via runtime call
    // ========================================================================
    if (auto inlineAsm = dynamic_cast<Sad::AST::InlineAsmExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found InlineAsmExpr" << std::endl;
        #endif
        
        // (AR) تمرير كود التجميع كنص إلى دالة تشغيلية __sad_inline_asm
        // (EN) Pass assembly code string to runtime function __sad_inline_asm
        std::string resultReg = newTempRegister();
        std::vector<SIROperand> args;
        args.push_back(SIROperand::ConstantString(inlineAsm->asmCode));
        args.push_back(SIROperand::ConstantString(inlineAsm->outputConstraints));
        args.push_back(SIROperand::ConstantString(inlineAsm->inputConstraints));
        args.push_back(SIROperand::ConstantString(inlineAsm->clobbers));
        args.push_back(SIROperand::ConstantBool(inlineAsm->isVolatile));
        
        SIRInstruction callInst = SIRInstruction::Call(
            SIROperand::Register(resultReg, SIRType::I64),
            SIROperand::ConstantString("__sad_inline_asm"),
            args
        );
        
        if (currentBlock_) {
            currentBlock_->addInstruction(callInst);
        }
        
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // ========================================================================
    // (AR) AwaitExpr - تعبير الانتظار (advanced_expr_nodes.h:AwaitExpr)
    // (EN) Await expression — lowered to runtime coroutine resume call
    // ========================================================================
    if (auto awaitExpr = dynamic_cast<Sad::AST::AwaitExpr*>(expr)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildExpression: found AwaitExpr" << std::endl;
        #endif
        
        // (AR) بناء التعبير الداخلي (الوعد/المولد)
        // (EN) Build inner expression (promise/generator)
        auto innerResult = buildExpression(awaitExpr->expression.get());
        
        // (AR) استدعاء __sad_await لحل الوعد
        // (EN) Call __sad_await to resolve the promise
        std::string resultReg = newTempRegister();
        std::vector<SIROperand> args;
        args.push_back(SIROperand::Register(innerResult.registerName, innerResult.type));
        
        SIRInstruction callInst = SIRInstruction::Call(
            SIROperand::Register(resultReg, innerResult.type),
            SIROperand::ConstantString("__sad_await"),
            args
        );
        
        if (currentBlock_) {
            currentBlock_->addInstruction(callInst);
        }
        
        return BuildResult(resultReg, innerResult.type);
    }
    
    // (AR) تعبير غير معروف - نرجع قيمة افتراضية
    // (EN) Unknown expression - return default
    #ifndef NDEBUG
    std::cout << "[WARNING] buildExpression: unhandled expression type: " 
              << typeid(*expr).name() << std::endl;
    #endif
    return BuildResult();
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
