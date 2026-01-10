/**
 * @file type_narrowing.cpp
 * @brief Type Narrowing System Implementation / تنفيذ نظام تضييق الأنواع
 * 
 * Phase 1.3.5.2: Type Narrowing Integration
 * 
 * @author Sad Language Team
 * @date 2026-01-03
 */

#include "types/type_narrowing.h"
#include "types/union_type.h"
#include "types/type_registry.h"
#include "types/optional_type.h"
#include "expressions.h"
#include "statements.h"
#include "token.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// TypeNarrowingContext Implementation
// ════════════════════════════════════════════════════════════════════════════════

void TypeNarrowingContext::pushScope() {
    scopes_.push_back(Scope());
}

void TypeNarrowingContext::popScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

void TypeNarrowingContext::registerVariable(const std::string& varName, TypePtr type) {
    originalTypes_[varName] = type;
}

void TypeNarrowingContext::narrowVariable(const std::string& varName, TypePtr narrowedType) {
    if (scopes_.empty()) {
        pushScope(); // تأكد من وجود نطاق / Ensure scope exists
    }
    
    scopes_.back().narrowedTypes[varName] = narrowedType;
}

TypePtr TypeNarrowingContext::getVariableType(const std::string& varName) const {
    // ابحث في النطاقات من الأحدث للأقدم / Search scopes from newest to oldest
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->narrowedTypes.find(varName);
        if (found != it->narrowedTypes.end()) {
            return found->second; // وُجد نوع مضيّق / Found narrowed type
        }
    }
    
    // إذا لم يُضيَّق، ارجع النوع الأصلي / If not narrowed, return original
    auto origIt = originalTypes_.find(varName);
    if (origIt != originalTypes_.end()) {
        return origIt->second;
    }
    
    return nullptr; // متغير غير معروف / Unknown variable
}

bool TypeNarrowingContext::hasVariable(const std::string& varName) const {
    return originalTypes_.find(varName) != originalTypes_.end();
}

TypePtr TypeNarrowingContext::getOriginalType(const std::string& varName) const {
    auto it = originalTypes_.find(varName);
    if (it != originalTypes_.end()) {
        return it->second;
    }
    return nullptr;
}

bool TypeNarrowingContext::applyGuard(const std::string& varName, TypeGuardPtr guard) {
    if (!guard) return false;
    
    // الحصول على النوع الحالي / Get current type
    TypePtr currentType = getVariableType(varName);
    if (!currentType) {
        return false; // متغير غير معروف / Unknown variable
    }
    
    // تطبيق الحارس / Apply guard
    auto result = guard->apply(currentType);
    if (!result || !result->isSuccess()) {
        return false; // فشل الحارس / Guard failed
    }
    
    // تضييق النوع / Narrow type
    TypePtr narrowedType = result->getNarrowedType();
    if (narrowedType) {
        narrowVariable(varName, narrowedType);
        return true;
    }
    
    return false;
}

bool TypeNarrowingContext::applyElseGuard(const std::string& varName, TypeGuardPtr guard) {
    if (!guard) return false;
    
    // الحصول على النوع الحالي / Get current type
    TypePtr currentType = getVariableType(varName);
    if (!currentType) {
        return false;
    }
    
    // تطبيق narrowElse / Apply narrowElse
    TypePtr elseType = guard->narrowElse(currentType);
    if (elseType) {
        narrowVariable(varName, elseType);
        return true;
    }
    
    return false;
}

void TypeNarrowingContext::mergeBranches(const TypeNarrowingContext& thenContext,
                                        const TypeNarrowingContext& elseContext) {
    // جمع جميع المتغيرات المُضيّقة من كلا الفرعين
    // Collect all narrowed variables from both branches
    std::unordered_set<std::string> allNarrowedVars;
    
    // من then branch
    if (!thenContext.scopes_.empty()) {
        const auto& thenScope = thenContext.scopes_.back();
        for (const auto& [varName, _] : thenScope.narrowedTypes) {
            allNarrowedVars.insert(varName);
        }
    }
    
    // من else branch
    if (!elseContext.scopes_.empty()) {
        const auto& elseScope = elseContext.scopes_.back();
        for (const auto& [varName, _] : elseScope.narrowedTypes) {
            allNarrowedVars.insert(varName);
        }
    }
    
    // دمج كل متغير / Merge each variable
    for (const auto& varName : allNarrowedVars) {
        TypePtr thenType = thenContext.getVariableType(varName);
        TypePtr elseType = elseContext.getVariableType(varName);
        
        if (thenType && elseType) {
            // كلا الفرعين ضيّقا المتغير / Both branches narrowed the variable
            if (thenType->equals(elseType.get())) {
                // نفس النوع في كلا الفرعين / Same type in both branches
                narrowVariable(varName, thenType);
            } else {
                // أنواع مختلفة → إنشاء Union / Different types → create Union
                TypePtr unionType = TypeNarrowingAnalyzer::createUnionType(thenType, elseType);
                if (unionType) {
                    narrowVariable(varName, unionType);
                } else {
                    // فشل الدمج → استخدم النوع الأصلي / Merge failed → use original
                    TypePtr original = getOriginalType(varName);
                    if (original) {
                        narrowVariable(varName, original);
                    }
                }
            }
        } else if (thenType) {
            // then فقط ضيّق / Only then narrowed
            // استخدم النوع الأصلي لأن else قد لا يكون مُضيَّقاً
            // Use original type since else may not be narrowed
            TypePtr original = getOriginalType(varName);
            if (original) {
                narrowVariable(varName, original);
            }
        } else if (elseType) {
            // else فقط ضيّق / Only else narrowed
            TypePtr original = getOriginalType(varName);
            if (original) {
                narrowVariable(varName, original);
            }
        }
    }
}

void TypeNarrowingContext::mergeSingleBranch(const TypeNarrowingContext& branchContext) {
    // عندما لا يوجد else، لا يمكن تطبيق التضييق بعد if
    // When no else exists, cannot apply narrowing after if
    // لأن المتغير قد لا يدخل if أصلاً
    // Because variable may not enter if at all
    
    // لا نفعل شيئاً - نبقي الأنواع الأصلية
    // Do nothing - keep original types
    // (في المستقبل: يمكن تطبيق definite assignment analysis)
}

void TypeNarrowingContext::dump() const {
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          TypeNarrowingContext State                       ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════╣\n";
    
    std::cout << "║ Original Types (" << originalTypes_.size() << "):\n";
    for (const auto& [varName, type] : originalTypes_) {
        std::cout << "║   " << varName << ": " << (type ? type->toString() : "null") << "\n";
    }
    
    std::cout << "║ Scopes (" << scopes_.size() << "):\n";
    for (size_t i = 0; i < scopes_.size(); ++i) {
        std::cout << "║   Scope " << i << " (" << scopes_[i].narrowedTypes.size() << " narrowed):\n";
        for (const auto& [varName, type] : scopes_[i].narrowedTypes) {
            std::cout << "║     " << varName << ": " << (type ? type->toString() : "null") << "\n";
        }
    }
    
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// TypeNarrowingAnalyzer Implementation
// ════════════════════════════════════════════════════════════════════════════════

void TypeNarrowingAnalyzer::resetContext() {
    currentContext_ = TypeNarrowingContext();
}

void TypeNarrowingAnalyzer::analyzeIfStatement(void* condition, 
                                               void* thenBlock, 
                                               void* elseBlock) {
    // استخراج Type Guard من الشرط / Extract Type Guard from condition
    std::string varName;
    TypeGuardPtr guard = extractGuardFromCondition(condition, varName);
    
    if (!guard || varName.empty()) {
        // لا يوجد guard قابل للاستخراج / No extractable guard
        return;
    }
    
    // إنشاء سياق then / Create then context
    TypeNarrowingContext thenContext = currentContext_;
    thenContext.pushScope();
    thenContext.applyGuard(varName, guard);
    
    // TODO: تحليل thenBlock مع thenContext
    // TODO: Analyze thenBlock with thenContext
    
    if (elseBlock) {
        // إنشاء سياق else / Create else context
        TypeNarrowingContext elseContext = currentContext_;
        elseContext.pushScope();
        elseContext.applyElseGuard(varName, guard);
        
        // TODO: تحليل elseBlock مع elseContext
        // TODO: Analyze elseBlock with elseContext
        
        // دمج الفرعين / Merge branches
        currentContext_.mergeBranches(thenContext, elseContext);
    } else {
        // دمج فرع واحد / Merge single branch
        currentContext_.mergeSingleBranch(thenContext);
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// Helper Functions for Guard Extraction
// دوال مساعدة لاستخراج الحراس
// ════════════════════════════════════════════════════════════════════════════════

namespace {
    /**
     * استخراج guard من تعبير ثنائي (!=, ==, etc.)
     * Extract guard from binary expression
     */
    TypeGuardPtr extractFromBinaryExpr(Sad::AST::BinaryExpr* expr, std::string& varName) {
        if (!expr) {
            varName = "";
            return nullptr;
        }
        
        using Sad::Lexer::TokenType;
        
        // Pattern 1: var != null → NotNull guard
        if (expr->op == TokenType::OP_NOT_EQUAL) {
            // Check right side for null
            if (auto* rightLit = dynamic_cast<Sad::AST::LiteralExpr*>(expr->right.get())) {
                if (rightLit->token.getType() == TokenType::LITERAL_NULL) {
                    // Left should be variable
                    if (auto* leftVar = dynamic_cast<Sad::AST::VariableExpr*>(expr->left.get())) {
                        varName = leftVar->name;
                        return TypeGuard::makeNotNullGuard(varName);
                    }
                }
            }
            // Check left side for null (null != var)
            if (auto* leftLit = dynamic_cast<Sad::AST::LiteralExpr*>(expr->left.get())) {
                if (leftLit->token.getType() == TokenType::LITERAL_NULL) {
                    if (auto* rightVar = dynamic_cast<Sad::AST::VariableExpr*>(expr->right.get())) {
                        varName = rightVar->name;
                        return TypeGuard::makeNotNullGuard(varName);
                    }
                }
            }
        }
        
        // Pattern 2: var == null → Null guard
        else if (expr->op == TokenType::OP_EQUAL) {
            // Check right side for null
            if (auto* rightLit = dynamic_cast<Sad::AST::LiteralExpr*>(expr->right.get())) {
                if (rightLit->token.getType() == TokenType::LITERAL_NULL) {
                    if (auto* leftVar = dynamic_cast<Sad::AST::VariableExpr*>(expr->left.get())) {
                        varName = leftVar->name;
                        return TypeGuard::makeNullGuard(varName);
                    }
                }
            }
            // Check left side for null (null == var)
            if (auto* leftLit = dynamic_cast<Sad::AST::LiteralExpr*>(expr->left.get())) {
                if (leftLit->token.getType() == TokenType::LITERAL_NULL) {
                    if (auto* rightVar = dynamic_cast<Sad::AST::VariableExpr*>(expr->right.get())) {
                        varName = rightVar->name;
                        return TypeGuard::makeNullGuard(varName);
                    }
                }
            }
        }
        
        // Pattern 3: typeof checks (future)
        // Pattern 4: instanceof checks (future)
        // Pattern 5: 'is' operator (future - needs language support)
        
        varName = "";
        return nullptr;
    }
    
    /**
     * استخراج guard من تعبير أحادي (!, etc.)
     * Extract guard from unary expression
     */
    TypeGuardPtr extractFromUnaryExpr(Sad::AST::UnaryExpr* expr, std::string& varName) {
        if (!expr) {
            varName = "";
            return nullptr;
        }
        
        using Sad::Lexer::TokenType;
        
        // Pattern: !var → treat as null check for Optional types
        if (expr->op == TokenType::OP_NOT) {
            if (auto* varExpr = dynamic_cast<Sad::AST::VariableExpr*>(expr->operand.get())) {
                varName = varExpr->name;
                // For !var, we create a Null guard (checks if it's falsy/null)
                return TypeGuard::makeNullGuard(varName);
            }
        }
        
        varName = "";
        return nullptr;
    }
}

TypeGuardPtr TypeNarrowingAnalyzer::extractGuardFromCondition(void* condition, 
                                                              std::string& varName) {
    // ============================================================================
    // استخراج Type Guard من تعبير شرط
    // Extract Type Guard from condition expression
    // ============================================================================
    
    if (!condition) {
        varName = "";
        return nullptr;
    }
    
    // Cast to AST expression
    auto* expr = reinterpret_cast<Sad::AST::Expression*>(condition);
    
    // Try different expression types
    
    // Pattern 1: Binary expressions (!=, ==, etc.)
    if (auto* binaryExpr = dynamic_cast<Sad::AST::BinaryExpr*>(expr)) {
        return extractFromBinaryExpr(binaryExpr, varName);
    }
    
    // Pattern 2: Unary expressions (!)
    if (auto* unaryExpr = dynamic_cast<Sad::AST::UnaryExpr*>(expr)) {
        return extractFromUnaryExpr(unaryExpr, varName);
    }
    
    // Pattern 3: Call expressions (typeof, instanceof) - future
    // if (auto* callExpr = dynamic_cast<Sad::AST::CallExpr*>(expr)) {
    //     return extractFromCallExpr(callExpr, varName);
    // }
    
    // No guard found
    varName = "";
    return nullptr;
}

TypePtr TypeNarrowingAnalyzer::createUnionType(TypePtr type1, TypePtr type2) {
    if (!type1 || !type2) {
        return nullptr;
    }
    
    // إذا كان النوعان نفسهما / If types are same
    if (type1->equals(type2.get())) {
        return type1;
    }
    
    // إنشاء Union / Create Union
    std::vector<TypePtr> types;
    types.push_back(type1);
    types.push_back(type2);
    
    return std::make_shared<UnionType>(types);
}

// ════════════════════════════════════════════════════════════════════════════════
// Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

bool canNarrowVariable(const std::string& varName, TypePtr type, TypeGuardPtr guard) {
    if (!type || !guard || varName.empty()) {
        return false;
    }
    
    // تحقق من إمكانية التضييق عن طريق تطبيق الحارس
    // Check if narrowing is possible by applying the guard
    TypePtr narrowed = guard->narrow(type);
    if (!narrowed) {
        return false;
    }
    
    // إذا كانت النتيجة Never، لا يمكن التضييق
    // If result is Never, narrowing is not possible
    auto& registry = TypeRegistry::getInstance();
    return !narrowed->equals(registry.getNeverType().get());
}

TypePtr applyNarrowingToVariable(TypeNarrowingContext& context,
                                 const std::string& varName,
                                 TypeGuardPtr guard) {
    if (!guard || varName.empty()) {
        return nullptr;
    }
    
    // تطبيق التضييق / Apply narrowing
    if (context.applyGuard(varName, guard)) {
        return context.getVariableType(varName);
    }
    
    return nullptr;
}

} // namespace TypeSystem
} // namespace Sad
