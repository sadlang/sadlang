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

#include <string>
#include "sir_builder.h"
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
            std::shared_ptr<SIRBasicBlock> SIRBuilder::createBasicBlock(const std::string &name)
            {
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
            std::string SIRBuilder::newTempRegister()
            {
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
            std::string SIRBuilder::newLabel(const std::string &prefix)
            {
                // (AR) إنشاء تسمية بصيغة prefix_N
                // (EN) Create label in format prefix_N
                return prefix + "_" + std::to_string(nextLabel_++);
            }

            // ============================================================================
            // astTypeToSIRType - تحويل DataType إلى SadTypeKind
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:741
            // التوقيع / Signature: SadTypeKind astTypeToSIRType(const Sad::Types::SadTypeKind& type);
            //
            // المعاملات / Parameters:
            // - type: const Sad::Types::SadTypeKind& (parser/data.h)
            //
            // الإرجاع / Returns:
            // - SadTypeKind: sir_types.h:57 (enum class SadTypeKind)
            //
            // SadTypeKind Values (sir_types.h:57):
            // - VOID, I64, F64, BOOL, PTR, ARRAY, STRING, STRUCT, FUNCTION
            // ============================================================================
            SadTypeKind SIRBuilder::astTypeToSIRType(const Sad::Types::SadTypeKind &type)
            {
                // (AR) تحويل DataType إلى SadTypeKind
                // (EN) Convert DataType to SadTypeKind
                switch (type)
                {
                case Types::SadTypeKind::Integer:
                    return SadTypeKind::Integer;
                case Types::SadTypeKind::Float:
                    return SadTypeKind::Float;
                case Types::SadTypeKind::Boolean:
                    return SadTypeKind::Boolean;
                case Types::SadTypeKind::String:
                    return SadTypeKind::String;
                case Types::SadTypeKind::Array:
                    return SadTypeKind::Array;
                case Types::SadTypeKind::Function:
                    return SadTypeKind::Function;
                case Types::SadTypeKind::Class:
                    // (AR) كائن - في الغالب معامل بدون نوع صريح
                    // (EN) Object - usually a parameter without explicit type
                    // في LLVM، نستخدم i64 لتمرير المؤشرات/القيم
                    return SadTypeKind::Integer;
                case Types::SadTypeKind::Void:
                    return SadTypeKind::Void;
                case Types::SadTypeKind::Unknown:
                    // (AR) نوع غير معروف - سيتم استنتاجه من التعبير
                    // (EN) Unknown type - will be inferred from expression
                    return SadTypeKind::Integer; // Default, will be overwritten by type inference
                default:
                    // (AR) أنواع أخرى (MAP, TUPLE, ENUM, BYTE, ERROR) — تحذير + fallback
                    // (EN) Other types (MAP, TUPLE, ENUM, BYTE, ERROR) — warn + fallback
                    std::cerr << "[sadc تحذير] DataType غير معالج في astTypeToSIRType: "
                              << static_cast<int>(type) << " — استخدام I64" << std::endl;
                    return SadTypeKind::Integer; // Fallback
                }
            }

            // ============================================================================
            // astTypeToSadType — تحويل DataType إلى SadTypePtr (النظام الموحد)
            // ============================================================================
            // (AR) يُرجع SadTypePtr مباشرة بدلاً من SadTypeKind
            //      يستخدم SadType::fromDataType للتحويل المباشر
            //      هذا هو الطريق المثالي — يتجنب فقدان معلومات النوع
            // (EN) Returns SadTypePtr directly instead of SadTypeKind
            //      Uses SadType::fromDataType for direct conversion
            //      This is the ideal path — avoids type information loss
            // ============================================================================
            Sad::Types::SadTypePtr SIRBuilder::astTypeToSadType(const Sad::Types::SadTypeKind &type)
            {
                return Sad::Types::SadType::fromValueType(type);
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
            bool SIRBuilder::hasReturnWithValue(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return false;

                // (AR) جملة return مباشرة
                // (EN) Direct return statement
                if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    return ret->value != nullptr; // true if return has a value
                }

                // (AR) كتلة من الجمل
                // (EN) Block of statements
                if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                    {
                        if (hasReturnWithValue(s.get()))
                            return true;
                    }
                    return false;
                }

                // (AR) جملة if
                // (EN) If statement
                if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    if (hasReturnWithValue(ifStmt->thenBranch.get()))
                        return true;
                    if (ifStmt->elseBranch && hasReturnWithValue(ifStmt->elseBranch.get()))
                        return true;
                    return false;
                }

                // (AR) حلقة while
                // (EN) While loop
                if (auto whileLoop = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    return hasReturnWithValue(whileLoop->body.get());
                }

                // (AR) حلقة for
                // (EN) For loop
                if (auto forLoop = dynamic_cast<const Sad::AST::ForStmt *>(stmt))
                {
                    return hasReturnWithValue(forLoop->body.get());
                }

                // (AR) حلقة for-range
                // (EN) For-range loop
                if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    return hasReturnWithValue(forRange->body.get());
                }

                // (AR) جملة match — نبحث في أجسام جميع الحالات
                // (EN) Match statement — search in all case bodies
                if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                {
                    for (const auto &caseClause : matchStmt->cases)
                    {
                        for (const auto &bodyStmt : caseClause.body)
                        {
                            if (hasReturnWithValue(bodyStmt.get()))
                                return true;
                        }
                    }
                    return false;
                }

                // (AR) جملة try-catch — نبحث في كتل المحاولة والالتقاط
                // (EN) Try-catch statement — search in try and catch blocks
                if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                {
                    if (hasReturnWithValue(tryStmt->tryBlock.get()))
                        return true;
                    for (const auto &catchClause : tryStmt->catchClauses)
                    {
                        if (hasReturnWithValue(catchClause.body.get()))
                            return true;
                    }
                    if (tryStmt->finallyBlock && hasReturnWithValue(tryStmt->finallyBlock.get()))
                        return true;
                    return false;
                }

                // (AR) جمل أخرى لا تحتوي return
                // (EN) Other statements don't contain return
                return false;
            }

            // ============================================================================
            // inferReturnTypeFromBody - استنتاج نوع الإرجاع من جسم الدالة
            // ============================================================================
            // التوقيع / Signature: SadTypeKind inferReturnTypeFromBody(const Sad::AST::Statement* body);
            //
            // الوصف / Description:
            // تفحص جسم الدالة لاستنتاج نوع الإرجاع:
            // - إذا لم يكن هناك return مع قيمة، تُرجع VOID
            // - إذا وُجد return مع قيمة، تفحص نوع التعبير
            // ============================================================================

            // Helper: get the return expression from the body
            static const Sad::AST::Expression *findFirstReturnExpr(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return nullptr;
                if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    return ret->value ? ret->value.get() : nullptr;
                }
                if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                    {
                        auto *e = findFirstReturnExpr(s.get());
                        if (e)
                            return e;
                    }
                }
                if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    auto *e = findFirstReturnExpr(ifStmt->thenBranch.get());
                    if (e)
                        return e;
                    if (ifStmt->elseBranch)
                        return findFirstReturnExpr(ifStmt->elseBranch.get());
                }
                if (auto w = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    return findFirstReturnExpr(w->body.get());
                }
                if (auto f = dynamic_cast<const Sad::AST::ForStmt *>(stmt))
                {
                    return findFirstReturnExpr(f->body.get());
                }
                if (auto fr = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    return findFirstReturnExpr(fr->body.get());
                }
                // (AR) جملة match — نبحث عن أول return في أي حالة
                // (EN) Match statement — find first return in any case
                if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                {
                    for (const auto &caseClause : matchStmt->cases)
                    {
                        for (const auto &bodyStmt : caseClause.body)
                        {
                            auto *e = findFirstReturnExpr(bodyStmt.get());
                            if (e)
                                return e;
                        }
                    }
                }
                // (AR) جملة try-catch — نبحث في كتل المحاولة والالتقاط
                // (EN) Try-catch statement — search in try and catch blocks
                if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                {
                    auto *e = findFirstReturnExpr(tryStmt->tryBlock.get());
                    if (e)
                        return e;
                    for (const auto &catchClause : tryStmt->catchClauses)
                    {
                        e = findFirstReturnExpr(catchClause.body.get());
                        if (e)
                            return e;
                    }
                    if (tryStmt->finallyBlock)
                    {
                        e = findFirstReturnExpr(tryStmt->finallyBlock.get());
                        if (e)
                            return e;
                    }
                }
                return nullptr;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
