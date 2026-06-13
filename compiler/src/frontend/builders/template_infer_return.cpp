// ============================================================================
// sir_builder_helpers_infer.cpp - inferReturnType and collectFreeVars helpers
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/template_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <limits>
#include <functional>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            SadTypeKind TemplateBuilder::inferReturnTypeFromBody(const Sad::AST::Statement *body,
                                                            const Sad::AST::FunctionDecl *funcDecl)
            {
                if (!body)
                    return SadTypeKind::Void;

                if (!b_.hasReturnWithValue(body))
                {
                    return SadTypeKind::Void;
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״®״±״·״© ״£†ˆ״§״¹ ״§„…״×״÷״±״§״× ״§„…״­„״© ג€” ״×…„״£ ״×״³„״³„״§‹ ״£״«†״§״¡ ״§„…״³״­
                // (EN) Local variable type map ג€” populated sequentially during scan
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                std::unordered_map<std::string, SadTypeKind> localVarTypes;

                // (AR) ״§„״®״·ˆ״© 1: ״×״¹״¨״¦״© ״£†ˆ״§״¹ …״¹״§…„״§״× ״§„״¯״§„״© …† b_.functionTable_ (״§„…״±״­„״© 1.7)
                // (EN) Step 1: Populate function parameter types from b_.functionTable_ (Phase 1.7)
                if (funcDecl)
                {
                    auto ftIt = b_.functionTable_.find(funcDecl->name);
                    if (ftIt != b_.functionTable_.end())
                    {
                        for (size_t i = 0; i < funcDecl->parameters.size() &&
                                           i < ftIt->second.parameters.size();
                             i++)
                        {
                            SadTypeKind paramType = ftIt->second.parameters[i].type;
                            localVarTypes[funcDecl->parameters[i].name] = paramType;
                        }
                    }
                    else
                    {
                        // (AR) ״¥״°״§ „… ״×ƒ†  ״§„״¬״¯ˆ„״ †״³״×״®״¯… ״£†ˆ״§״¹ AST
                        // (EN) If not in table, use AST types
                        for (const auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                            localVarTypes[param.name] = paramType;
                        }
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„״®״·ˆ״© 2: ״¯״§„״© ״§״³״×†״×״§״¬ †ˆ״¹ ״×״¹״¨״± …״¹ ״¯״¹… ״§„…״×״÷״±״§״× ״§„…״­„״©
                // (EN) Step 2: Expression type inference with local variable support
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                std::function<SadTypeKind(const Sad::AST::Expression *)> inferExprType;
                inferExprType = [&](const Sad::AST::Expression *expr) -> SadTypeKind
                {
                    if (!expr)
                        return SadTypeKind::Integer;

                    // (AR) ״­״± ג€” †״³״×†״×״¬ …† †ˆ״¹ ״§„״±…״²
                    // (EN) Literal ג€” infer from token type
                    if (auto lit = dynamic_cast<const Sad::AST::LiteralExpr *>(expr))
                    {
                        auto tt = lit->token.getType();
                        if (tt == Sad::Lexer::TokenType::NUMBER_DOUBLE)
                            return SadTypeKind::Float;
                        if (tt == Sad::Lexer::TokenType::STRING_LITERAL)
                            return SadTypeKind::String;
                        if (tt == Sad::Lexer::TokenType::LITERAL_TRUE ||
                            tt == Sad::Lexer::TokenType::LITERAL_FALSE)
                            return SadTypeKind::Boolean;
                        if (tt == Sad::Lexer::TokenType::LITERAL_NULL)
                            return SadTypeKind::Integer;
                        return SadTypeKind::Integer;
                    }

                    // (AR) …״×״÷״± ג€” †״¨״­״«  ״§„…״×״÷״±״§״× ״§„…״­„״© ״§„…״×״×״¨‘״¹״©
                    // (EN) Variable ג€” look up in tracked local variable types
                    if (auto var = dynamic_cast<const Sad::AST::VariableExpr *>(expr))
                    {
                        auto it = localVarTypes.find(var->name);
                        if (it != localVarTypes.end())
                            return it->second;
                        return SadTypeKind::Integer;
                    }

                    // (AR) …״µˆ״© ״­״±״©
                    // (EN) Array literal
                    if (dynamic_cast<const Sad::AST::ArrayExpr *>(expr))
                        return SadTypeKind::Array;

                    // (AR) ״×״¹״¨״± ״«†״§״¦ ג€” †״´״± ״§„†ˆ״¹ ״§„†״µ/״§„״¹״´״±
                    // (EN) Binary expression ג€” propagate string/float types
                    if (auto bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                    {
                        SadTypeKind left = inferExprType(bin->left.get());
                        SadTypeKind right = inferExprType(bin->right.get());
                        // (AR) ״¹…„״§״× ״§„…‚״§״±†״© ״×״±״¬״¹ BOOL ״¯״§״¦…״§‹ ג€” ״¬״¨ ״­״µ‡״§ ‚״¨„ †״´״± ״§„†ˆ״¹ ״§„†״µ/״§„״¹״´״±
                        // (EN) Comparison operations always return BOOL ג€” must be checked BEFORE string/float propagation
                        switch (bin->op)
                        {
                        case Sad::Lexer::TokenType::OP_EQUAL:
                        case Sad::Lexer::TokenType::OP_NOT_EQUAL:
                        case Sad::Lexer::TokenType::OP_LESS:
                        case Sad::Lexer::TokenType::OP_GREATER:
                        case Sad::Lexer::TokenType::OP_LESS_EQUAL:
                        case Sad::Lexer::TokenType::OP_GREATER_EQUAL:
                        case Sad::Lexer::TokenType::OP_AND:
                        case Sad::Lexer::TokenType::OP_OR:
                            return SadTypeKind::Boolean;
                        default:
                            break;
                        }
                        // (AR) ״§„‚״³…״© `/` ״×†״×״¬ ״¹״´״± ״¯״§״¦…״§‹ (״­״³״¨ …ˆ״§״µ״§״× ״§„„״÷״©)
                        // (EN) Division `/` always produces float (per language spec)
                        if (bin->op == Sad::Lexer::TokenType::OP_DIVIDE)
                            return SadTypeKind::Float;
                        // (AR) ״¥״°״§ ƒ״§† ״§„״·״±״§† †״µ״§‹ ״£ˆ ״£״­״¯‡…״§״ ״§„†״×״¬״© †״µ (״¬…״¹ †״µˆ״µ)
                        if (left == SadTypeKind::String || right == SadTypeKind::String)
                            return SadTypeKind::String;
                        if (left == SadTypeKind::Float || right == SadTypeKind::Float)
                            return SadTypeKind::Float;
                        return left;
                    }

                    // (AR) ״×״¹״¨״± ״£״­״§״¯
                    // (EN) Unary expression
                    if (auto unary = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
                    {
                        if (unary->op == Sad::Lexer::TokenType::OP_NOT)
                            return SadTypeKind::Boolean;
                        return inferExprType(unary->operand.get());
                    }

                    // (AR) استدعاء دالة: نحاول استنتاج نوع الإرجاع من b_.functionTable_
                    //      مع دعم استدعاءات الأعضاء (obj.method()) حتى داخل اللامدا.
                    // (EN) Function call: infer return type from b_.functionTable_,
                    //      with support for member calls (obj.method()) even inside lambdas.
                    if (auto call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                    {
                        if (auto varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                        {
                            auto it = b_.functionTable_.find(varExpr->name);
                            if (it != b_.functionTable_.end())
                            {
                                return it->second.returnType;
                            }
                        }

                        if (auto memberCallee = dynamic_cast<const Sad::AST::MemberExpr *>(call->callee.get()))
                        {
                            std::string className;

                            if (dynamic_cast<const Sad::AST::ThisExpr *>(memberCallee->object.get()))
                            {
                                className = b_.currentClassName_;
                            }
                            else if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(memberCallee->object.get()))
                            {
                                auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                                if (ciIt != b_.classInstanceTypes_.end())
                                {
                                    className = ciIt->second;
                                }
                            }

                            if (!className.empty())
                            {
                                std::string searchClass = className;
                                while (!searchClass.empty())
                                {
                                    std::string fullMethodName = searchClass + "." + memberCallee->member;
                                    auto fit = b_.functionTable_.find(fullMethodName);
                                    if (fit != b_.functionTable_.end())
                                    {
                                        return fit->second.returnType;
                                    }

                                    if (!b_.module_)
                                    {
                                        break;
                                    }
                                    auto sirClass = b_.module_->getClass(searchClass);
                                    if (!sirClass || sirClass->parentClass.empty())
                                    {
                                        break;
                                    }
                                    searchClass = sirClass->parentClass;
                                }
                            }

                            // (AR) احتياط: إذا كان نوع الكائن غير معروف، نبحث عن أي دالة باسم "*.member"
                            //      وعند توحّد نوع الإرجاع نستخدمه بدلاً من الافتراضي Integer.
                            // (EN) Fallback: if object type is unknown, look for "*.member" methods and
                            //      use the return type when all matches agree.
                            bool foundAny = false;
                            bool conflict = false;
                            SadTypeKind unifiedType = SadTypeKind::Integer;
                            std::string suffix = "." + memberCallee->member;
                            for (const auto &entry : b_.functionTable_)
                            {
                                const auto &name = entry.first;
                                if (name.size() >= suffix.size() &&
                                    name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
                                {
                                    if (!foundAny)
                                    {
                                        unifiedType = entry.second.returnType;
                                        foundAny = true;
                                    }
                                    else if (unifiedType != entry.second.returnType)
                                    {
                                        conflict = true;
                                        break;
                                    }
                                }
                            }

                            if (foundAny && !conflict)
                            {
                                return unifiedType;
                            }
                        }

                        return SadTypeKind::Integer;
                    }

                    // (AR) ״×״¹״¨״± ״«„״§״«
                    // (EN) Ternary expression
                    if (auto ternary = dynamic_cast<const Sad::AST::TernaryExpr *>(expr))
                    {
                        SadTypeKind trueType = inferExprType(ternary->trueExpr.get());
                        SadTypeKind falseType = inferExprType(ternary->falseExpr.get());
                        if (trueType == SadTypeKind::String || falseType == SadTypeKind::String)
                            return SadTypeKind::String;
                        if (trueType == SadTypeKind::Float || falseType == SadTypeKind::Float)
                            return SadTypeKind::Float;
                        return trueType;
                    }

                    // (AR) ״¥״³†״§״¯ ג€” †ˆ״¹ ״§„‚…״© ״§„…״³†״¯״©
                    // (EN) Assignment ג€” type of assigned value
                    if (auto assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                    {
                        return inferExprType(assign->value.get());
                    }

                    // (AR) ˆ״µˆ„ „״­‚„ ״¹״¨״± ‡״°״§.״­‚„ ג€” †״¨״­״« ״¹† †ˆ״¹ ״§„״­‚„  b_.module_->getClass
                    // (EN) Member access via this.field ג€” look up field type in b_.module_->getClass
                    // (AR) …„״§״­״¸״©: AST ״­״×ˆ ״¹„‰ †ˆ״¹†: MemberExpr (expressions.h) ˆ MemberAccessExpr (class_nodes.h)
                    // (EN) Note: AST has two types: MemberExpr (expressions.h) and MemberAccessExpr (class_nodes.h)
                    if (auto memberExpr = dynamic_cast<const Sad::AST::MemberExpr *>(expr))
                    {
                        if (dynamic_cast<const Sad::AST::ThisExpr *>(memberExpr->object.get()))
                        {
                            if (!b_.currentClassName_.empty() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(b_.currentClassName_);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberExpr->member);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;

                                        // ═══════════════════════════════════════════════════
                                        // (AR) إصلاح: إذا كان الحقل Pointer (مجهول النوع)،
                                        //      نبحث عبر paramToFieldMap_ عن المعامل المرتبط
                                        //      ثم نتحقق من نوعه في b_.functionTable_ (باني الصنف)
                                        //      Phase 1.7 حدّث نوع المعامل، وPhase 2 حفظه
                                        //      بدون هذا: ارجع هذا.حقل يُستنتج كـ Integer
                                        // (EN) Fix: If field is Pointer (unknown), infer from
                                        //      constructor param via paramToFieldMap_ lookup
                                        //      Phase 1.7 updated param type, Phase 2 preserved it
                                        // ═══════════════════════════════════════════════════
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberExpr->member)
                                                {
                                                    std::string ctorName = b_.currentClassName_ + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(memberExpr->object.get()))
                        {
                            auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                            if (ciIt != b_.classInstanceTypes_.end() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(ciIt->second);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberExpr->member);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;

                                        // ═══════════════════════════════════════════════════
                                        // (AR) إصلاح: إذا كان الحقل Pointer (مجهول النوع)،
                                        //      نبحث عبر paramToFieldMap_ عن المعامل المرتبط
                                        //      ثم نتحقق من نوعه في b_.functionTable_ (باني الصنف)
                                        //      بدون هذا: ارجع هذا.حقل يُستنتج كـ Integer
                                        //      بدلاً من String عندما الحقل بلا مُهيئ
                                        // (EN) Fix: If field is Pointer (unknown type), try to
                                        //      infer from constructor param via paramToFieldMap_
                                        //      Without this: return this.field infers as Integer
                                        //      instead of String when field has no initializer
                                        // ═══════════════════════════════════════════════════
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberExpr->member)
                                                {
                                                    std::string ctorName = b_.currentClassName_ + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        // (EN) If field is Pointer (unknown), try to infer from constructor
                                        //      Find param linked to this field via paramToFieldMap_
                                        //      then check its type in b_.functionTable_ (updated in Phase 1.7)
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            // (AR) ״¨״­״« ״¹ƒ״³: ״£ …״¹״§…„ ‚״§״¨„ ‡״°״§ ״§„״­‚„״
                                            // (EN) Reverse lookup: which param maps to this field?
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberExpr->member)
                                                {
                                                    // (AR) ˆ״¬״¯†״§ ״§„…״¹״§…„ ג€” ״§„״¢† †״¨״­״« ״¹† †ˆ״¹‡  ״§„״¨״§†
                                                    // (EN) Found param ג€” now look up its type in constructor
                                                    std::string ctorName = ciIt->second + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .״¨״§†
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        // (AR) params[0] = self, ״§״¨״­״« ״¹† ״§„…״¹״§…„ ״¨״§„״§״³…
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        return SadTypeKind::Integer;
                    }
                    if (auto memberAccessExpr = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                    {
                        if (dynamic_cast<const Sad::AST::ThisExpr *>(memberAccessExpr->object.get()))
                        {
                            if (!b_.currentClassName_.empty() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(b_.currentClassName_);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberAccessExpr->memberName);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;

                                        // (EN) Same fix as MemberExpr: Pointer → paramToFieldMap_ lookup
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberAccessExpr->memberName)
                                                {
                                                    std::string ctorName = b_.currentClassName_ + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(memberAccessExpr->object.get()))
                        {
                            auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                            if (ciIt != b_.classInstanceTypes_.end() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(ciIt->second);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberAccessExpr->memberName);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;
                                    }
                                }
                            }
                        }
                        return SadTypeKind::Integer;
                    }

                    // (AR) ״¥†״´״§״¡ ƒ״§״¦† ״¬״¯״¯
                    // (EN) New object creation
                    if (dynamic_cast<const Sad::AST::NewExpr *>(expr))
                        return SadTypeKind::Struct;
                    // (AR) تعبير صف (Tuple)
                    // (EN) Tuple expression
                    if (dynamic_cast<const Sad::AST::TupleExpr *>(expr))
                        return SadTypeKind::Tuple;
                    // ================================================================
                    // (AR) [Fix #52] تعبير لامدا — يُرجع دائماً نوع Function
                    //      بدون هذا الفحص، inferReturnTypeFromBody يُعيد Integer
                    //      لدالة مثل: دالة صانع() ارجع لامدا()...نهاية نهاية
                    //      مما يمنع تتبع returnLambdaName في b_.functionTable_
                    // (EN) [Fix #52] Lambda expression — always returns Function type
                    //      Without this check, inferReturnTypeFromBody returns Integer
                    //      for functions like: function maker() return lambda()...end end
                    //      which prevents returnLambdaName tracking in b_.functionTable_
                    // ================================================================
                    if (dynamic_cast<const Sad::AST::LambdaExpr *>(expr))
                        return SadTypeKind::Function;
                    // (AR) ״­״µ DataType …† ״§„״×״¹״¨״± †״³‡ (״¥״°״§ ״×ˆ״±)
                    // (EN) Check DataType from expression itself (if available)
                    auto dtype = expr->getTypeKind();
                    if (dtype == Sad::Types::SadTypeKind::Float)
                        return SadTypeKind::Float;
                    if (dtype == Sad::Types::SadTypeKind::Boolean)
                        return SadTypeKind::Boolean;
                    if (dtype == Sad::Types::SadTypeKind::String)
                        return SadTypeKind::String;
                    if (dtype == Sad::Types::SadTypeKind::Array)
                        return SadTypeKind::Array;
                    if (dtype == Sad::Types::SadTypeKind::Class)
                        return SadTypeKind::Struct;
                    if (dtype == Sad::Types::SadTypeKind::Map)
                        return SadTypeKind::Map;

                    return SadTypeKind::Integer;
                };

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„״®״·ˆ״© 3: …״³״­ ״×״³„״³„ „״¬״³… ״§„״¯״§„״© „״×״¹״¨״¦״© ״£†ˆ״§״¹ ״§„…״×״÷״±״§״× ״§„…״­„״©
                // (EN) Step 3: Sequential scan of function body to populate local variable types
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                std::function<void(const Sad::AST::Statement *)> populateVarTypes;
                populateVarTypes = [&](const Sad::AST::Statement *stmt)
                {
                    if (!stmt)
                        return;

                    // (AR) ״¥״¹„״§† …״×״÷״± ג€” †״³״×†״×״¬ †ˆ״¹ ״§„…‡‘״¦ ˆ†״³״¬‘„‡
                    // (EN) Variable declaration ג€” infer initializer type and register
                    if (auto varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                    {
                        if (varDecl->initializer)
                        {
                            SadTypeKind varType = inferExprType(varDecl->initializer.get());
                            localVarTypes[varDecl->name] = varType;
                        }
                        else
                        {
                            // (AR) …״×״÷״± ״¨״¯ˆ† …‡‘״¦ ג€” ††״¸״± ״¥„‰ †ˆ״¹ AST
                            SadTypeKind astType = b_.astTypeToSIRType(varDecl->type);
                            localVarTypes[varDecl->name] = astType;
                        }
                        return;
                    }

                    // (AR) ƒ״×„״© ״¬…„ ג€” …״³״­ ״×״³„״³„ „„״­״§״¸ ״¹„‰ ״×״±״×״¨ ״§„״¥״¹„״§†״§״×
                    // (EN) Block statement ג€” sequential scan to preserve declaration order
                    if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                    {
                        for (const auto &s : block->statements)
                        {
                            populateVarTypes(s.get());
                        }
                        return;
                    }

                    // (AR) ״×״¹״¨״± ״¥״³†״§״¯ ג€” ״×״­״¯״« †ˆ״¹ ״§„…״×״÷״±
                    // (EN) Assignment expression ג€” update variable type
                    if (auto exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                    {
                        if (auto assign = dynamic_cast<const Sad::AST::AssignExpr *>(exprStmt->expression.get()))
                        {
                            SadTypeKind newType = inferExprType(assign->value.get());
                            localVarTypes[assign->name] = newType;
                        }
                        return;
                    }

                    // (AR) ״±ˆ״¹ ״´״±״·״© ג€” …״³״­ ƒ„״§ ״§„״±״¹†
                    // (EN) Conditional branches ג€” scan both branches
                    if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                    {
                        populateVarTypes(ifStmt->thenBranch.get());
                        if (ifStmt->elseBranch)
                            populateVarTypes(ifStmt->elseBranch.get());
                        return;
                    }

                    // (AR) ״­„‚״§״×
                    // (EN) Loops
                    if (auto whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                    {
                        populateVarTypes(whileStmt->body.get());
                        return;
                    }
                    if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                    {
                        populateVarTypes(forRange->body.get());
                        return;
                    }
                    // (AR) ״¬…„״© match ג€” …״³״­ ״£״¬״³״§… ״§„״­״§„״§״×
                    // (EN) Match statement ג€” scan case bodies
                    if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                    {
                        for (const auto &caseClause : matchStmt->cases)
                        {
                            for (const auto &bodyStmt : caseClause.body)
                            {
                                populateVarTypes(bodyStmt.get());
                            }
                        }
                        return;
                    }
                    // (AR) ״¬…„״© try-catch ג€” …״³״­ ƒ״×„ ״§„…״­״§ˆ„״© ˆ״§„״§„״×‚״§״·
                    // (EN) Try-catch statement ג€” scan try and catch blocks
                    if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                    {
                        populateVarTypes(tryStmt->tryBlock.get());
                        for (const auto &catchClause : tryStmt->catchClauses)
                        {
                            populateVarTypes(catchClause.body.get());
                        }
                        if (tryStmt->finallyBlock)
                            populateVarTypes(tryStmt->finallyBlock.get());
                        return;
                    }
                };

                populateVarTypes(body);

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„״®״·ˆ״© 4: ״¬…״¹ ״£†ˆ״§״¹ ״¬…״¹ ״¹״¨״§״±״§״× ״§„״¥״±״¬״§״¹ ˆ״×ˆ״­״¯‡״§
                // (EN) Step 4: Collect types from ALL return statements and unify them
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                std::function<void(const Sad::AST::Statement *, std::vector<SadTypeKind> &)> collectReturnTypes;
                collectReturnTypes = [&](const Sad::AST::Statement *stmt, std::vector<SadTypeKind> &types)
                {
                    if (!stmt)
                        return;
                    if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                    {
                        if (ret->value)
                        {
                            types.push_back(inferExprType(ret->value.get()));
                        }
                        return;
                    }
                    if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                    {
                        for (auto &s : block->statements)
                            collectReturnTypes(s.get(), types);
                    }
                    if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                    {
                        collectReturnTypes(ifStmt->thenBranch.get(), types);
                        if (ifStmt->elseBranch)
                            collectReturnTypes(ifStmt->elseBranch.get(), types);
                    }
                    if (auto whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                    {
                        collectReturnTypes(whileStmt->body.get(), types);
                    }
                    if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                    {
                        collectReturnTypes(forRange->body.get(), types);
                    }
                    // (AR) ״¬…„״© match ג€” †״¨״­״« ״¹† return  ״¬…״¹ ״£״¬״³״§… ״§„״­״§„״§״×
                    // (EN) Match statement ג€” search for return in all case bodies
                    if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                    {
                        for (const auto &caseClause : matchStmt->cases)
                        {
                            for (const auto &bodyStmt : caseClause.body)
                            {
                                collectReturnTypes(bodyStmt.get(), types);
                            }
                        }
                    }
                    // (AR) ״¬…„״© try-catch ג€” †״¨״­״«  ƒ״×„ ״§„…״­״§ˆ„״© ˆ״§„״§„״×‚״§״·
                    // (EN) Try-catch statement ג€” search in try and catch blocks
                    if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                    {
                        collectReturnTypes(tryStmt->tryBlock.get(), types);
                        for (const auto &catchClause : tryStmt->catchClauses)
                        {
                            collectReturnTypes(catchClause.body.get(), types);
                        }
                        if (tryStmt->finallyBlock)
                            collectReturnTypes(tryStmt->finallyBlock.get(), types);
                    }
                };

                std::vector<SadTypeKind> returnTypes;
                collectReturnTypes(body, returnTypes);

                if (returnTypes.empty())
                    return SadTypeKind::Integer;

                // (AR) ״×ˆ״­״¯ ״§„״£†ˆ״§״¹: STRING ״³״·״±״ I64+F64ג†’F64
                // (EN) Unify types: STRING dominates, I64+F64ג†’F64
                SadTypeKind unified = returnTypes[0];
                for (size_t i = 1; i < returnTypes.size(); ++i)
                {
                    if (unified == returnTypes[i])
                        continue;
                    if ((unified == SadTypeKind::Integer && returnTypes[i] == SadTypeKind::Float) ||
                        (unified == SadTypeKind::Float && returnTypes[i] == SadTypeKind::Integer))
                    {
                        unified = SadTypeKind::Float;
                    }
                    else if (unified == SadTypeKind::String || returnTypes[i] == SadTypeKind::String)
                    {
                        unified = SadTypeKind::String;
                    }
                }

                return unified;
            }

            // ============================================================================
            // b_.enterScope - ״¯״®ˆ„ †״·״§‚ ״¬״¯״¯
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:587
            // ״§„״×ˆ‚״¹ / Signature: void b_.enterScope();
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - b_.currentScopeLevel_: sir_builder.h:599 (int)
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
