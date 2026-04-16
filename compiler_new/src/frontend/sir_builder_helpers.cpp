// ============================================================================
// sir_builder.cpp - ״¨†״§״¡ SIR …† AST / SIR Builder from AST
// ============================================================================
// ״§„…״₪„ / Author: Sad Compiler Team
// ״§„״×״§״±״® / Date: January 5, 2026
// ״§„״¥״µ״¯״§״± / Version: 2.0 (״¥״¹״§״¯״© ƒ״×״§״¨״© ƒ״§…„״© …† ״§„״µ״±)
//
// …„״§״­״¸״© ‡״§…״© / Important Note:
// ‡״°״§ ״§„…„ …״¹״§״¯ ƒ״×״§״¨״×‡ ״¨״§„ƒ״§…„ …† ״§„״µ״± ״¨…״§ ״×ˆ״§‚ 100% …״¹:
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
#include "../../../shared/utils/include/utf8_utils.h"
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
            // HELPER FUNCTIONS - ״§„״¯ˆ״§„ ״§„…״³״§״¹״¯״©
            // ============================================================================

            // ============================================================================
            // createBasicBlock - ״¥†״´״§״¡ ƒ״×„״© ״£״³״§״³״© ״¬״¯״¯״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:501
            // ״§„״×ˆ‚״¹ / Signature: std::shared_ptr<SIRBasicBlock> createBasicBlock(const std::string& name);
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - std::shared_ptr<SIRBasicBlock>: sir_module.h:145 (SIRBasicBlock class)
            //
            // SIRBasicBlock Constructor (sir_module.h:145):
            // - SIRBasicBlock(const std::string& name): line 168
            // ============================================================================
            std::shared_ptr<SIRBasicBlock> SIRBuilder::createBasicBlock(const std::string &name)
            {
                // (AR) ״¥†״´״§״¡ ƒ״×„״© ״¬״¯״¯״© (sir_module.h:168 - SIRBasicBlock constructor)
                // (EN) Create new block
                return std::make_shared<SIRBasicBlock>(name);
            }

            // ============================================================================
            // newTempRegister - ״¥†״´״§״¡ ״³״¬„ …״₪‚״× ״¬״¯״¯
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:511
            // ״§„״×ˆ‚״¹ / Signature: std::string newTempRegister();
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - registerCounter_: sir_builder.h:600 (int)
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - std::string: ״§״³… ״§„״³״¬„ ״¨״µ״÷״© %0, %1, %2, ...
            // ============================================================================
            std::string SIRBuilder::newTempRegister()
            {
                // (AR) ״¥†״´״§״¡ ״³״¬„ ״¨״µ״÷״© %N ״­״« N ‡ˆ nextTempRegister_
                // (EN) Create register in format %N where N is nextTempRegister_
                return "%" + std::to_string(nextTempRegister_++);
            }

            // ============================================================================
            // newLabel - ״¥†״´״§״¡ ״×״³…״© ״¬״¯״¯״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:520
            // ״§„״×ˆ‚״¹ / Signature: std::string newLabel(const std::string& prefix);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - prefix: std::string = ״§„״¨״§״¯״¦״© (L, if, loop, etc)
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - labelCounter_: sir_builder.h:601 (int)
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - std::string: ״§״³… ״§„״×״³…״© ״¨״µ״÷״© prefix_N
            // ============================================================================
            std::string SIRBuilder::newLabel(const std::string &prefix)
            {
                // (AR) ״¥†״´״§״¡ ״×״³…״© ״¨״µ״÷״© prefix_N
                // (EN) Create label in format prefix_N
                return prefix + "_" + std::to_string(nextLabel_++);
            }

            // ============================================================================
            // astTypeToSIRType - ״×״­ˆ„ DataType ״¥„‰ SadTypeKind
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:741
            // ״§„״×ˆ‚״¹ / Signature: SadTypeKind astTypeToSIRType(const Sad::Data::DataType& type);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - type: const Sad::Data::DataType& (parser/data.h)
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - SadTypeKind: sir_types.h:57 (enum class SadTypeKind)
            //
            // SadTypeKind Values (sir_types.h:57):
            // - VOID, I64, F64, BOOL, PTR, ARRAY, STRING, STRUCT, FUNCTION
            //
            // DataType Values (data_types.h:20):
            // - UNKNOWN, INTEGER, FLOAT, STRING, BOOLEAN, NONE, ARRAY, MAP, TUPLE,
            //   FUNCTION, OBJECT, ENUM, BYTE, ERROR
            // ============================================================================
            SadTypeKind SIRBuilder::astTypeToSIRType(const Sad::Data::DataType &type)
            {
                // (AR) ״×״­ˆ„ DataType ״¥„‰ SadTypeKind
                // (EN) Convert DataType to SadTypeKind
                switch (type)
                {
                case Data::DataType::INTEGER:
                    return SadTypeKind::Integer;
                case Data::DataType::FLOAT:
                    return SadTypeKind::Float;
                case Data::DataType::BOOLEAN:
                    return SadTypeKind::Boolean;
                case Data::DataType::STRING:
                    return SadTypeKind::String;
                case Data::DataType::ARRAY:
                    return SadTypeKind::Array;
                case Data::DataType::FUNCTION:
                    return SadTypeKind::Function;
                case Data::DataType::OBJECT:
                    // (AR) ƒ״§״¦† -  ״§„״÷״§„״¨ …״¹״§…„ ״¨״¯ˆ† †ˆ״¹ ״µ״±״­
                    // (EN) Object - usually a parameter without explicit type
                    //  LLVM״ †״³״×״®״¯… i64 „״×…״±״± ״§„…״₪״´״±״§״×/״§„‚…
                    return SadTypeKind::Integer;
                case Data::DataType::NONE:
                    return SadTypeKind::Void;
                case Data::DataType::UNKNOWN:
                    // (AR) †ˆ״¹ ״÷״± …״¹״±ˆ - ״³״×… ״§״³״×†״×״§״¬‡ …† ״§„״×״¹״¨״±
                    // (EN) Unknown type - will be inferred from expression
                    return SadTypeKind::Integer; // Default, will be overwritten by type inference
                default:
                    // (AR) ״£†ˆ״§״¹ ״£״®״±‰ (MAP, TUPLE, ENUM, BYTE, ERROR) ג€” ״×״­״°״± + fallback
                    // (EN) Other types (MAP, TUPLE, ENUM, BYTE, ERROR) ג€” warn + fallback
                    std::cerr << "[sadc ״×״­״°״±] DataType ״÷״± …״¹״§„״¬  astTypeToSIRType: "
                              << static_cast<int>(type) << " ג€” ״§״³״×״®״¯״§… I64" << std::endl;
                    return SadTypeKind::Integer; // Fallback
                }
            }

            // ============================================================================
            // astTypeToSadType ג€” ״×״­ˆ„ DataType ״¥„‰ SadTypePtr (״§„†״¸״§… ״§„…ˆ״­״¯)
            // ============================================================================
            // (AR) ״±״¬״¹ SadTypePtr …״¨״§״´״±״© ״¨״¯„״§‹ …† SadTypeKind
            //      ״³״×״®״¯… SadType::fromDataType „„״×״­ˆ„ ״§„…״¨״§״´״±
            //      ‡״°״§ ‡ˆ ״§„״·״±‚ ״§„…״«״§„ ג€” ״×״¬†״¨ ‚״¯״§† …״¹„ˆ…״§״× ״§„†ˆ״¹
            // (EN) Returns SadTypePtr directly instead of SadTypeKind
            //      Uses SadType::fromDataType for direct conversion
            //      This is the ideal path ג€” avoids type information loss
            // ============================================================================
            Sad::Types::SadTypePtr SIRBuilder::astTypeToSadType(const Sad::Data::DataType &type)
            {
                return Sad::Types::SadType::fromDataType(type);
            }

            // ============================================================================
            // hasReturnWithValue - ״­״µ ״¥״°״§ ƒ״§†״× ״§„״¬…„״© ״×״­״×ˆ return …״¹ ‚…״©
            // ============================================================================
            // ״§„״×ˆ‚״¹ / Signature: bool hasReturnWithValue(const Sad::AST::Statement* stmt);
            //
            // ״§„ˆ״µ / Description:
            // ״×״­״µ ״§„״¬…„״© ״¨״´ƒ„ ״×״¹״§ˆ״¯ „„״¨״­״« ״¹† ״¬…„ return ״×״­״×ˆ ‚…״©.
            // Recursively checks statement for return statements with values.
            // ============================================================================
            bool SIRBuilder::hasReturnWithValue(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return false;

                // (AR) ״¬…„״© return …״¨״§״´״±״©
                // (EN) Direct return statement
                if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    return ret->value != nullptr; // true if return has a value
                }

                // (AR) ƒ״×„״© …† ״§„״¬…„
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

                // (AR) ״¬…„״© if
                // (EN) If statement
                if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    if (hasReturnWithValue(ifStmt->thenBranch.get()))
                        return true;
                    if (ifStmt->elseBranch && hasReturnWithValue(ifStmt->elseBranch.get()))
                        return true;
                    return false;
                }

                // (AR) ״­„‚״© while
                // (EN) While loop
                if (auto whileLoop = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    return hasReturnWithValue(whileLoop->body.get());
                }

                // (AR) ״­„‚״© for
                // (EN) For loop
                if (auto forLoop = dynamic_cast<const Sad::AST::ForStmt *>(stmt))
                {
                    return hasReturnWithValue(forLoop->body.get());
                }

                // (AR) ״­„‚״© for-range
                // (EN) For-range loop
                if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    return hasReturnWithValue(forRange->body.get());
                }

                // (AR) ״¬…„״© match ג€” †״¨״­״«  ״£״¬״³״§… ״¬…״¹ ״§„״­״§„״§״×
                // (EN) Match statement ג€” search in all case bodies
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

                // (AR) ״¬…„״© try-catch ג€” †״¨״­״«  ƒ״×„ ״§„…״­״§ˆ„״© ˆ״§„״§„״×‚״§״·
                // (EN) Try-catch statement ג€” search in try and catch blocks
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

                // (AR) ״¬…„ ״£״®״±‰ „״§ ״×״­״×ˆ return
                // (EN) Other statements don't contain return
                return false;
            }

            // ============================================================================
            // inferReturnTypeFromBody - ״§״³״×†״×״§״¬ †ˆ״¹ ״§„״¥״±״¬״§״¹ …† ״¬״³… ״§„״¯״§„״©
            // ============================================================================
            // ״§„״×ˆ‚״¹ / Signature: SadTypeKind inferReturnTypeFromBody(const Sad::AST::Statement* body);
            //
            // ״§„ˆ״µ / Description:
            // ״×״­״µ ״¬״³… ״§„״¯״§„״© „״§״³״×†״×״§״¬ †ˆ״¹ ״§„״¥״±״¬״§״¹:
            // - ״¥״°״§ „… ƒ† ‡†״§ƒ return …״¹ ‚…״©״ ״×״±״¬״¹ VOID
            // - ״¥״°״§ ˆ״¬״¯ return …״¹ ‚…״©״ ״×״­״µ †ˆ״¹ ״§„״×״¹״¨״±
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
                // (AR) ״¬…„״© match ג€” †״¨״­״« ״¹† ״£ˆ„ return  ״£ ״­״§„״©
                // (EN) Match statement ג€” find first return in any case
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
                // (AR) ״¬…„״© try-catch ג€” †״¨״­״«  ƒ״×„ ״§„…״­״§ˆ„״© ˆ״§„״§„״×‚״§״·
                // (EN) Try-catch statement ג€” search in try and catch blocks
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
