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
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // ״§„…†״´״¦ / Constructor
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:287
            // ״§„״×ˆ‚״¹ / Signature: SIRBuilder();
            //
            // ״§„…״×״÷״±״§״× ״§„…‡‘״¦״© / Initialized variables:
            // - module_: sir_builder.h:582 (std::shared_ptr<SIRModule>)
            // - currentFunction_: sir_builder.h:583 (std::shared_ptr<SIRFunction>)
            // - currentBlock_: sir_builder.h:584 (SIRBasicBlock*)
            // - currentScopeLevel_: sir_builder.h:599 (int)
            // - registerCounter_: sir_builder.h:600 (int)
            // - labelCounter_: sir_builder.h:601 (int)
            // ============================================================================
            SIRBuilder::SIRBuilder()
            {
                // (AR) جميع الحقول مُهيّأة في SIRBuilderContext (base class) بقيم افتراضية
                // (EN) All fields are default-initialized in SIRBuilderContext (base class)
                // (AR) تهيئة بنّاء استدعاءات الطرق (Phase 6 Step 2)
                // (EN) Initialize method-call builder (Phase 6 Step 2)
                methodCalls_ = std::make_unique<MethodCallBuilder>(*this);
                // (AR) تهيئة بنّاء الدوال المدمجة (Phase 6 Step 3)
                // (EN) Initialize builtin function builder (Phase 6 Step 3)
                builtins_ = std::make_unique<BuiltinBuilder>(*this);
                // (AR) تهيئة بنّاء استدعاءات الدوال (Phase 6 Step 4)
                // (EN) Initialize function call builder (Phase 6 Step 4)
                calls_ = std::make_unique<CallBuilder>(*this);
                // (AR) تهيئة بنّاء الأصناف والسمات (Phase 6 Step 5)
                // (EN) Initialize class/trait builder (Phase 6 Step 5)
                classes_ = std::make_unique<ClassBuilder>(*this);
                // (AR) تهيئة بنّاء العبارات (Phase 6 Step 6)
                // (EN) Initialize statement builder (Phase 6 Step 6)
                statements_ = std::make_unique<StatementBuilder>(*this);
                // (AR) تهيئة بنّاء التعابير (Phase 6 Step 7)
                // (EN) Initialize expression builder (Phase 6 Step 7)
                expressions_ = std::make_unique<ExpressionBuilder>(*this);
                // (AR) تهيئة بنّاء القوالب (Phase 6 Step 8 - الأخيرة)
                // (EN) Initialize template builder (Phase 6 Step 8 - final)
                templates_ = std::make_unique<TemplateBuilder>(*this);
                // (AR) „״§ ״×ˆ״¬״¯ ״×‡״¦״© ״¥״¶״§״© …״·„ˆ״¨״©
                // (EN) No additional initialization required
            }

            // ============================================================================
            // buildModule - ״¨†״§״¡ ˆ״­״¯״© SIR ƒ״§…„״© …† AST
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:329
            // ״§„״×ˆ‚״¹ / Signature:
            //   std::shared_ptr<SIRModule> buildModule(AST::ProgramNode* program);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - program: AST::ProgramNode* = Sad::AST::StmtList* (sir_builder.h:51)
            //            ˆ‡ˆ alias „€ std::vector<StmtPtr> (ast_node.h:170)
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - std::shared_ptr<SIRModule>: ˆ״­״¯״© SIR ƒ״§…„״© ״£ˆ nullptr ״¹†״¯ ״§„״®״·״£
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - std::make_shared<SIRModule>: standard library
            // - buildFunction: sir_builder.h:348
            // - buildGlobalVariable: sir_builder.h:356
            // - buildClass: sir_builder.h:364
            //
            // AST Node Types Used:
            // - Sad::AST::FunctionDecl: declarations.h:19
            // - Sad::AST::VarDeclStmt: statements.h:74
            // - Sad::AST::ClassDecl: declarations.h:122
            // ============================================================================
            std::shared_ptr<SIRModule> SIRBuilder::buildModule(AST::ProgramNode *program)
            {
                if (!program)
                {
                    errors_.push_back("Error: Null program AST");
                    return nullptr;
                }

                // (AR) ״¥†״´״§״¡ ˆ״­״¯״© SIR ״¬״¯״¯״© (sir_module.h:501 - SIRModule constructor)
                // (EN) Create new SIR module
                module_ = std::make_shared<SIRModule>("main");

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© ״§„״£ˆ„‰: ״×״³״¬„ ״×ˆ‚״¹״§״× ״¬…״¹ ״§„״¯ˆ״§„ …״³״¨‚״§‹
                // (EN) Phase 1: Pre-register all function signatures for forward references
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    AST::FunctionDecl *funcDecl = nullptr;

                    // (AR) ״¯״§„״© ״¹״§״¯״©
                    if (auto fd = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                    {
                        funcDecl = fd;
                    }
                    // (AR) ״¯״§„״© …״µ״¯‘״±״© (״§„״¥״µ״¯״§״± ״§„״¬״¯״¯)
                    else if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                    {
                        if (exportDecl->declaration)
                        {
                            funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportDecl->declaration.get());
                        }
                    }
                    // (AR) ״¯״§„״© …״µ״¯‘״±״© (״§„״¥״µ״¯״§״± ״§„‚״¯…)
                    else if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                    {
                        if (exportStmt->declaration)
                        {
                            funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportStmt->declaration.get());
                        }
                    }

                    if (funcDecl)
                    {
                        // (AR) ״×״³״¬„ ״×ˆ‚״¹ ״§„״¯״§„״©  ״§„״¬״¯ˆ„
                        // (EN) Register function signature in function table
                        FunctionInfo funcInfo;
                        funcInfo.name = funcDecl->name;
                        funcInfo.returnType = astTypeToSIRType(funcDecl->returnType);
                        for (const auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            funcInfo.parameters.push_back(SIRParameter(param.name, paramType));
                        }
                        // (AR) …״₪״´״± ״§„״¯״§„״© ״³״­״¯‘״« „״§״­‚״§‹  buildFunction
                        // (EN) sirFunction pointer will be updated later in buildFunction
                        funcInfo.sirFunction = nullptr;
                        // (AR) ״­״¸ …״±״¬״¹ AST „…״¹״§„״¬״© ״§„‚… ״§„״§״×״±״§״¶״© ״¹†״¯ ״§„״§״³״×״¯״¹״§״¡
                        // (EN) Save AST reference for default parameter values at call sites
                        funcInfo.astDecl = funcDecl;
                        functionTable_[funcDecl->name] = funcInfo;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase1: registered func '" << funcDecl->name
                                  << "' retType=" << static_cast<int>(funcInfo.returnType)
                                  << " AST-retType=" << static_cast<int>(funcDecl->returnType) << std::endl;
#endif
                    }

                    // ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                    // (AR) ״×״³״¬„ ״¯ˆ״§„ ״§„״§…״×״¯״§״¯ …״³״¨‚״§‹: ״§…״×״¯״§״¯ †ˆ״¹_‡״¯ ... †‡״§״©
                    //      ״×״³״¬‘„ ƒ€ "†ˆ״¹_‡״¯.״§״³…_״§„״¯״§„״©"  ״¬״¯ˆ„ ״§„״¯ˆ״§„
                    //      ‡״°״§ ״¶…† ״£† ״§„״¯ˆ״§„ ״§„…״¹״±‘״© ‚״¨„ ƒ״×„״© ״§„״§…״×״¯״§״¯ …ƒ†‡״§ ״§„ˆ״µˆ„ ״¥„‡״§
                    // (EN) Pre-register extension methods: extension target_type ... end
                    //      Registered as "target_type.method_name" in function table
                    // ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                    if (auto extensionDecl = dynamic_cast<Sad::AST::ExtensionDecl *>(stmt.get()))
                    {
                        for (auto &method : extensionDecl->methods)
                        {
                            if (!method)
                                continue;
                            auto extFunc = dynamic_cast<Sad::AST::FunctionDecl *>(method.get());
                            if (!extFunc)
                                continue;

                            std::string fullName = extensionDecl->targetType + "." + extFunc->name;
                            FunctionInfo extInfo;
                            extInfo.name = fullName;
                            extInfo.returnType = astTypeToSIRType(extFunc->returnType);
                            // (AR) ״§„…״¹״§…„ ״§„״£ˆ„ ‡ˆ self
                            extInfo.parameters.push_back(SIRParameter(kSelfParamName, SadTypeKind::Integer));
                            for (const auto &param : extFunc->parameters)
                            {
                                SadTypeKind paramType = astTypeToSIRType(param.type);
                                extInfo.parameters.push_back(SIRParameter(param.name, paramType));
                            }
                            extInfo.sirFunction = nullptr;
                            extInfo.astDecl = extFunc;
                            functionTable_[fullName] = extInfo;
#ifdef SIR_BUILDER_DEBUG
                            std::cerr << "[SIR-DBG] Phase1: registered extension method '" << fullName << "'" << std::endl;
#endif
                        }
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 1.3: ״×״³״¬„ ״×ˆ‚״¹״§״× ״¯ˆ״§„ ״§„״£״µ†״§ …״³״¨‚״§‹ (״®״§״µ״© ״§„״³״§ƒ†״©)
                //      ״­״×‰ ״×״×ˆ״±  functionTable_ ‚״¨„ ״§„…״±״­„״© 1.7 (״§״³״×†״×״§״¬ ״§„״£†ˆ״§״¹)
                //      ״¨״¯ˆ† ‡״°״§: ״§״³״×״¯״¹״§״¡״§״× …״«„ ״³״¬„.״³״¬‘„("״£ˆ„") „״§ ״×״­״¯‘״« ״£†ˆ״§״¹ ״§„…״¹״§…„״§״×
                // (EN) Phase 1.3: Pre-register class method signatures (especially static)
                //      So they're available in functionTable_ before Phase 1.7 (type inference)
                //      Without this: calls like Class.method("arg") don't update param types
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    auto *classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt.get());
                    if (!classDecl)
                        continue;

                    for (const auto &member : classDecl->members)
                    {
                        if (!member)
                            continue;

                        auto *methodDecl = dynamic_cast<Sad::AST::MethodDecl *>(member.get());
                        if (!methodDecl)
                            continue;

                        if (methodDecl->isAbstract)
                            continue;

                        std::string fullMethodName = classDecl->name + "." + methodDecl->name;

                        // (AR) ״×״®״· ״¥״°״§ ״³״¨‚ ״§„״×״³״¬„
                        // (EN) Skip if already registered
                        if (functionTable_.count(fullMethodName) > 0)
                            continue;

                        FunctionInfo methodInfo;
                        methodInfo.name = fullMethodName;
                        methodInfo.returnType = astTypeToSIRType(methodDecl->returnType);

                        // (AR) ״§„״¯ˆ״§„ ״÷״± ״§„״³״§ƒ†״© ״×״£״®״° self ƒ…״¹״§…„ ״£ˆ„
                        // (EN) Non-static methods take self as first parameter
                        if (!methodDecl->isStatic)
                        {
                            methodInfo.parameters.push_back(SIRParameter(kSelfParamName, SadTypeKind::Integer));
                        }

                        for (const auto &param : methodDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            methodInfo.parameters.push_back(SIRParameter(param.name, paramType));
                        }

                        methodInfo.sirFunction = nullptr;
                        functionTable_[fullMethodName] = methodInfo;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase1.3: registered method '" << fullMethodName
                                  << "' static=" << methodDecl->isStatic
                                  << " params=" << methodInfo.parameters.size() << std::endl;
#endif
                    }
                }

                // ═══════════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.35: تسجيل توقيعات البُناة مبكراً (قبل Phase 1.7/1.8)
                //      بدون هذا: Phase 1.7 لا يستطيع تحديث أنواع معاملات الباني
                //      و Phase 1.8 لا يستطيع استنتاج أنواع الحقول من وسائط الباني
                //      مما يُبقي الحقول كـ Pointer والمعاملات كـ Integer
                // (EN) Phase 1.35: Pre-register constructor signatures (before Phase 1.7/1.8)
                //      Without this: Phase 1.7 can't update constructor param types
                //      and Phase 1.8 can't infer field types from constructor arguments
                //      leaving fields as Pointer and params as Integer
                // ═══════════════════════════════════════════════════════════════════════
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    auto *classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt.get());
                    if (!classDecl)
                        continue;

                    for (const auto &member : classDecl->members)
                    {
                        if (!member)
                            continue;

                        auto *ctorDecl = dynamic_cast<Sad::AST::ConstructorDecl *>(member.get());
                        if (!ctorDecl)
                            continue;

                        std::string fullCtorName = classDecl->name + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني

                        // (AR) تخطي إذا سبق التسجيل
                        // (EN) Skip if already registered
                        if (functionTable_.count(fullCtorName) > 0)
                            continue;

                        FunctionInfo ctorInfo;
                        ctorInfo.name = fullCtorName;
                        ctorInfo.returnType = SadTypeKind::Void;

                        // (AR) المعامل الأول دائماً self
                        // (EN) First parameter is always self
                        ctorInfo.parameters.push_back(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        for (const auto &param : ctorDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            ctorInfo.parameters.push_back(SIRParameter(param.name, paramType));
                        }

                        ctorInfo.sirFunction = nullptr;
                        functionTable_[fullCtorName] = ctorInfo;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase1.35: registered ctor '" << fullCtorName
                                  << "' params=" << ctorInfo.parameters.size() << std::endl;
#endif
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 1.5: ״×״³״¬„ ״§„…״×״÷״±״§״× ״§„״¹״§…״© …״³״¨‚״§‹  †״·״§‚ ״¹״§… ״«״§״¨״×
                // (EN) Phase 1.5: Pre-register global variables in a persistent global scope
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ‡״°״§ ״§„†״·״§‚ ״¨‚‰ ״·ˆ״§„ ״¨†״§״¡ ״§„ˆ״­״¯״© ״­״×‰ ״×״³״×״·״¹ ״§„״¯ˆ״§„ ״§„ˆ״µˆ„ „„…״×״÷״±״§״× ״§„״¹״§…״©
                // (EN) This scope persists throughout module building so functions can access globals
                enterScope(); // (AR) ״§„†״·״§‚ ״§„״¹״§… - ״³״¨‚‰ ״­״×‰ †‡״§״© buildModule

                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    // (AR) ״§״³״×״®״±״§״¬ VarDeclStmt …״¨״§״´״±״© ״£ˆ …† ״¯״§״®„ ExportDecl/ExportStmt
                    // (EN) Extract VarDeclStmt directly or from inside ExportDecl/ExportStmt
                    Sad::AST::VarDeclStmt *varDecl = nullptr;

                    varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt.get());

                    if (!varDecl)
                    {
                        if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                        {
                            if (exportDecl->declaration)
                            {
                                varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(exportDecl->declaration.get());
                            }
                        }
                    }
                    if (!varDecl)
                    {
                        if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                        {
                            if (exportStmt->declaration)
                            {
                                varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(exportStmt->declaration.get());
                            }
                        }
                    }

                    if (varDecl)
                    {
                        // (AR) ״×״³״¬„ ״§„…״×״÷״± ״§„״¹״§…  ״§„†״·״§‚ ״§„״¹״§…
                        // (EN) Register global variable in global scope
                        SadTypeKind varType = astTypeToSIRType(varDecl->type);

                        // ================================================================
                        // (AR) ״§״³״×†״×״§״¬ ״§„†ˆ״¹ …† ״§„‚…״© ״§„״­״±״© ״¹†״¯…״§ ƒˆ† ״§„†ˆ״¹ ״÷״± …״¹״±ˆ:
                        //      …״×״÷״± ״³ = 3.14 ג†’ ״§„†ˆ״¹ UNKNOWN ג†’ ״¬״¨ ״£† ƒˆ† F64 „״§ I64
                        //      ״¨״¯ˆ† ‡״°״§: ״§„״¹״´״± ״®״²† ƒ€ i64 (״¨״×״§״× ״®״§…) ˆ״·״¨״¹ ״¨״´ƒ„ ״®״§״·״¦
                        // (EN) Infer type from literal initializer when type is UNKNOWN:
                        //      var x = 3.14 ג†’ type UNKNOWN ג†’ should be F64 not I64
                        //      Without this: double stored as i64 (raw bits) and printed incorrectly
                        // ================================================================
                        if (varDecl->type == Data::DataType::UNKNOWN && varDecl->initializer)
                        {
                            // (AR) ״§״³״×†״×״§״¬ ״§„†ˆ״¹ …† ״£ ״×״¹״¨״± ג€” „״³ ‚״· ״§„״­״±״§״×
                            //      †״³״×״®״¯… ״¯״§„״© …״³״§״¹״¯״© ״×…״´  ״´״¬״±״© ״§„״×״¹״¨״± ״¨״´ƒ„ ״¹…‚
                            //      „״§ƒ״×״´״§ ״£ ˆ״¬ˆ״¯ „€ NUMBER_DOUBLE ״£ˆ …״×״÷״± Float
                            // (EN) Infer type from any expression ג€” not just literals
                            //      Use helper that walks expression tree to detect float presence
                            varType = inferExprType(varDecl->initializer.get());
                        }

                        VariableInfo globalVarInfo;
                        globalVarInfo.name = varDecl->name;
                        globalVarInfo.type = varType;
                        globalVarInfo.registerName = "%" + varDecl->name;
                        globalVarInfo.isGlobal = true;
                        globalVarInfo.isMutable = !varDecl->isConst;
                        globalVarInfo.scopeLevel = 0;

                        // (AR) ״§״³״×†״×״§״¬ †ˆ״¹ ״¹†״µ״± ״§„…״µˆ״© …† ArrayExpr „״¯״¹… foreach
                        //      …״«״§„: …״×״÷״± ״§„ˆ״§ƒ‡ = ["״×״§״­"״ "…ˆ״²"] ג†’ elementType = STRING
                        //      ‡״°״§ …ƒ‘† ARRAY_GET …† ״§״³״×״®״¯״§… ״§„†ˆ״¹ ״§„״µ״­״­ ״¹†״¯ ״§„״×ƒ״±״§״±
                        // (EN) Infer array element type from ArrayExpr for foreach support
                        if (varDecl->initializer)
                        {
                            if (auto *arrayExpr = dynamic_cast<Sad::AST::ArrayExpr *>(varDecl->initializer.get()))
                            {
                                if (!arrayExpr->elements.empty())
                                {
                                    if (auto *litElem = dynamic_cast<Sad::AST::LiteralExpr *>(arrayExpr->elements[0].get()))
                                    {
                                        auto elemTokenType = litElem->token.getType();
                                        if (elemTokenType == Lexer::TokenType::STRING_LITERAL)
                                        {
                                            globalVarInfo.elementType = SadTypeKind::String;
                                        }
                                        else if (elemTokenType == Lexer::TokenType::NUMBER_DOUBLE)
                                        {
                                            globalVarInfo.elementType = SadTypeKind::Float;
                                        }
                                        else if (elemTokenType == Lexer::TokenType::LITERAL_TRUE ||
                                                 elemTokenType == Lexer::TokenType::LITERAL_FALSE)
                                        {
                                            globalVarInfo.elementType = SadTypeKind::Boolean;
                                        }
                                    }
                                }
                            }
                        }

                        addVariable(globalVarInfo);

                        // (AR) ״×״×״¨״¹ †ˆ״¹ ״§„״µ† …״³״¨‚״§‹ „„…״×״÷״±״§״× ״§„״¹״§…״© ״§„…‡״£״© ״¨€ ״¬״¯״¯ ״µ†()
                        // (EN) Pre-track class type for global variables initialized with new ClassName()
                        if (varDecl->initializer)
                        {
                            if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(varDecl->initializer.get()))
                            {
                                classInstanceTypes_[varDecl->name] = newExpr->className;
                            }
                        }

                        // (AR) ״¥״¶״§״© ״§„…״×״÷״± ״§„״¹״§… ״£״¶״§‹ „ˆ״­״¯״© SIR ״­״×‰ †״´״¦ LLVM CodeGen …״×״÷״±״§״× ״¹״§…״© ״­‚‚״©
                        // (EN) Also add global variable to SIR module so LLVM CodeGen creates real LLVM globals
                        auto sirGlobal = std::make_shared<SIRGlobalVariable>(varDecl->name, varType);
                        sirGlobal->isConstant = varDecl->isConst;

                        // (AR) ״§״³״×״®״±״§״¬ ״§„‚…״© ״§„״£ˆ„״© ״¥״°״§ ƒ״§†״× ״«״§״¨״×״§‹ ״­״±״§‹
                        // (EN) Extract initial value if it's a literal constant
                        if (varDecl->initializer)
                        {
                            if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
                            {
                                const auto &token = litExpr->token;
                                std::string value = token.getValue();
                                Lexer::TokenType tokenType = token.getType();

                                if (tokenType == Lexer::TokenType::NUMBER_INTEGER)
                                {
                                    // (AR) ״×״­ˆ„ ״§„״£״¹״¯״§״¯ ״§„״³״× ״¹״´״±״©/״§„״«…״§†״©/״§„״«†״§״¦״© ״¥„‰ ״¹״´״±״©
                                    // (EN) Normalize hex/octal/binary literals to decimal
                                    if (value.size() > 2 && value[0] == '0')
                                    {
                                        char prefix = value[1];
                                        if (prefix == 'x' || prefix == 'X')
                                        {
                                            value = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
                                        }
                                        else if (prefix == 'o' || prefix == 'O')
                                        {
                                            value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
                                        }
                                        else if (prefix == 'b' || prefix == 'B')
                                        {
                                            value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
                                        }
                                    }
                                    sirGlobal->initialValue = value;
                                }
                                else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE)
                                {
                                    sirGlobal->initialValue = value;
                                }
                                else if (tokenType == Lexer::TokenType::LITERAL_TRUE)
                                {
                                    sirGlobal->initialValue = "1";
                                }
                                else if (tokenType == Lexer::TokenType::LITERAL_FALSE)
                                {
                                    sirGlobal->initialValue = "0";
                                }
                            }
                        }

                        module_->addGlobalVariable(sirGlobal);
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 1.7: ״§״³״×†״×״§״¬ ״£†ˆ״§״¹ ״§„…״¹״§…„״§״× …† …ˆ״§‚״¹ ״§„״§״³״×״¯״¹״§״¡
                // (EN) Phase 1.7: Infer parameter types from call sites
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ‚״¨„ ״¨†״§״¡ ״£״¬״³״§… ״§„״¯ˆ״§„״ †…״³״­ ƒ„ ״§״³״×״¯״¹״§״¡״§״× ״§„״¯ˆ״§„  ״§„״¨״±†״§…״¬
                //      „״×״­״¯״« ״£†ˆ״§״¹ ״§„…״¹״§…„״§״× ״÷״± ״§„…״­״¯״¯״© (UNKNOWN ג†’ I64) ״¥„‰ ״£†ˆ״§״¹‡״§ ״§„״­‚‚״©
                //      ״¨†״§״¡‹ ״¹„‰ ״§„ˆ״³״§״¦״· ״§„״¹„״© ״§„……״±״±״©  …ˆ״§‚״¹ ״§„״§״³״×״¯״¹״§״¡
                // (EN) Before building function bodies, scan all function calls in the program
                //      to update untyped parameter types (UNKNOWN ג†’ I64) to their actual types
                //      based on the real arguments passed at call sites
                inferParamTypesFromCallSites(program);

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 1.8: ״§״³״×†״×״§״¬ ״£†ˆ״§״¹ ״­‚ˆ„ ״§„״£״µ†״§ …† ˆ״³״§״¦״· ״¬״¯״¯() …״¨ƒ״±״§‹
                //      †…״³״­ ƒ„ ״×״¹״¨״±״§״× NewExpr  ״§„״¨״±†״§…״¬ ˆ†״·״§״¨‚ ˆ״³״§״¦״·‡״§ …״¹ ״­‚ˆ„ ״§„״µ†
                //      ״¹״¨״± paramToFieldMap_ ג€” ‡״°״§ ״³…״­ „€ inferReturnTypeFromBody ״¨…״¹״±״©
                //      ״£† ״­‚„ "״§״³…" ‡ˆ STRING ‚״¨„ ״¨†״§״¡ ״£״¬״³״§… ״§„״¯ˆ״§„
                // (EN) Phase 1.8: Early field type inference from NewExpr arguments
                //      Scan all NewExpr in program and match arguments with class fields
                //      via paramToFieldMap_ ג€” this allows inferReturnTypeFromBody to know
                //      that field "״§״³…" is STRING before building function bodies
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                {
                    // (AR) ״¯״§„״© ״×״¹״§ˆ״¯״© „…״³״­ ״×״¹״¨״±״§״× NewExpr  ״§„״¬…„
                    // (EN) Recursive function to scan NewExpr in statements
                    std::function<void(const Sad::AST::Expression *)> scanExprForNewExpr;
                    std::function<void(const Sad::AST::Statement *)> scanStmtForNewExpr;

                    scanExprForNewExpr = [&](const Sad::AST::Expression *expr)
                    {
                        if (!expr)
                            return;
                        if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                        {
                            auto sirClass = module_->getClass(newExpr->className);
                            if (sirClass && !sirClass->paramToFieldMap_.empty())
                            {
                                // (AR) ״§„״¨״­״« ״¹† ״§„״¨״§† „„״­״µˆ„ ״¹„‰ ״£״³…״§״¡ ״§„…״¹״§…„״§״×
                                // (EN) Find constructor to get parameter names
                                std::string ctorName = newExpr->className + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .״¨״§†
                                auto ctorFunc = module_->getFunction(ctorName);
                                if (ctorFunc)
                                {
                                    const auto &params = ctorFunc->getParameters();
                                    // params[0] = self, params[1..N] = user params
                                    for (size_t i = 1; i < params.size() && (i - 1) < newExpr->arguments.size(); i++)
                                    {
                                        const std::string &paramName = params[i].name;
                                        auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                                        if (fieldIt != sirClass->paramToFieldMap_.end())
                                        {
                                            const std::string &fieldName = fieldIt->second;
                                            auto currentType = sirClass->fields_.find(fieldName);
                                            if (currentType != sirClass->fields_.end() &&
                                                currentType->second == SadTypeKind::Pointer)
                                            {
                                                // (AR) ״§״³״×†״×״§״¬ †ˆ״¹ ״§„ˆ״³״· …† AST
                                                // (EN) Infer argument type from AST
                                                const auto &arg = newExpr->arguments[i - 1];
                                                SadTypeKind argType = SadTypeKind::Pointer;
                                                if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(arg.get()))
                                                {
                                                    auto tt = lit->token.getType();
                                                    if (tt == Lexer::TokenType::STRING_LITERAL)
                                                        argType = SadTypeKind::String;
                                                    else if (tt == Lexer::TokenType::NUMBER_INTEGER)
                                                        argType = SadTypeKind::Integer;
                                                    else if (tt == Lexer::TokenType::NUMBER_DOUBLE)
                                                        argType = SadTypeKind::Float;
                                                    else if (tt == Lexer::TokenType::LITERAL_TRUE || tt == Lexer::TokenType::LITERAL_FALSE)
                                                        argType = SadTypeKind::Boolean;
                                                }
                                                else if (dynamic_cast<const Sad::AST::ArrayExpr *>(arg.get()))
                                                {
                                                    argType = SadTypeKind::Array;
                                                }
                                                else if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(arg.get()))
                                                {
                                                    // (AR) …״×״÷״± ג€” †״¨״­״« ״¹† †ˆ״¹‡  ״§„״¬״¯ˆ„
                                                    // (EN) Variable ג€” look up its type
                                                    auto *varInfo = lookupVariable(varExpr->name);
                                                    if (varInfo && varInfo->type != SadTypeKind::Void)
                                                        argType = varInfo->type;
                                                }

                                                if (argType != SadTypeKind::Pointer && argType != SadTypeKind::Void)
                                                {
                                                    sirClass->fields_[fieldName] = argType;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // (AR) …״³״­ ״§„״×״¹״¨״±״§״× ״§„״±״¹״©
                        if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                        {
                            scanExprForNewExpr(call->callee.get());
                            for (const auto &arg : call->arguments)
                                scanExprForNewExpr(arg.get());
                        }
                        else if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                        {
                            scanExprForNewExpr(bin->left.get());
                            scanExprForNewExpr(bin->right.get());
                        }
                        else if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                        {
                            scanExprForNewExpr(assign->value.get());
                        }
                    };

                    scanStmtForNewExpr = [&](const Sad::AST::Statement *stmt)
                    {
                        if (!stmt)
                            return;
                        if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                        {
                            if (varDecl->initializer)
                                scanExprForNewExpr(varDecl->initializer.get());
                        }
                        else if (auto *exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                        {
                            if (exprStmt->expression)
                                scanExprForNewExpr(exprStmt->expression.get());
                        }
                        else if (auto *block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                        {
                            for (const auto &s : block->statements)
                                scanStmtForNewExpr(s.get());
                        }
                        else if (auto *ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                        {
                            scanStmtForNewExpr(ifStmt->thenBranch.get());
                            if (ifStmt->elseBranch)
                                scanStmtForNewExpr(ifStmt->elseBranch.get());
                        }
                        else if (auto *whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                        {
                            scanStmtForNewExpr(whileStmt->body.get());
                        }
                        else if (auto *forStmt = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                        {
                            scanStmtForNewExpr(forStmt->body.get());
                        }
                    };

                    for (const auto &stmt : *program)
                    {
                        if (stmt)
                            scanStmtForNewExpr(stmt.get());
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© ״§„״«״§†״©: ״¨†״§״¡ ״§„״×״µ״±״­״§״× ˆ״¬…״¹ ״§„״¬…„ ״§„״×†״°״©
                // (EN) Phase 2: Build declarations and collect executable statements
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״¬…״¹ ״§„״¬…„ ״§„״×†״°״©  ״§„…״³״×ˆ‰ ״§„״£״¹„‰ „״×״¶…†‡״§  __sad_main
                // (EN) Collect top-level executable statements for __sad_main wrapper
                std::vector<Sad::AST::Statement *> topLevelStatements;
                bool hasMainFunction = false; // (AR) ‡„ ״×ˆ״¬״¯ ״¯״§„״© "״±״¦״³״©" ״£ˆ "main"״

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 2״£: ״¨†״§״¡ ״§„״£״µ†״§ ˆ״§„״×״¹״¯״§״¯״§״× ˆ״§„״³…״§״× ״£ˆ„״§‹
                //      ‡״°״§ ״¶״±ˆ״± ״­״×‰ ״×״×ˆ״± …״¹„ˆ…״§״× ״§„״­‚ˆ„ ˆ״§„״¨״§†״§״×
                //      ‚״¨„ ״¨†״§״¡ ״§„״¯ˆ״§„ ״§„״× ‚״¯ ״×״­״×״§״¬ inferReturnTypeFromBody
                // (EN) Phase 2A: Build classes, enums, traits FIRST
                //      This ensures field info and constructors are available
                //      before building functions that may need inferReturnTypeFromBody
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt.get()))
                    {
                        buildClass(classDecl);
                        continue;
                    }

                    if (auto enumDecl = dynamic_cast<Sad::AST::EnumDecl *>(stmt.get()))
                    {
                        buildStatement(enumDecl);
                        continue;
                    }

                    if (auto traitDecl = dynamic_cast<Sad::AST::TraitDecl *>(stmt.get()))
                    {
                        buildTrait(traitDecl);
                        continue;
                    }

                    if (auto implDecl = dynamic_cast<Sad::AST::ImplDecl *>(stmt.get()))
                    {
                        buildImpl(implDecl);
                        continue;
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 2״¨: …״³״­ NewExpr „״×״­״¯״« ״£†ˆ״§״¹ ״­‚ˆ„ ״§„״£״µ†״§
                //      ״§„״¢† ״¨״¹״¯ ״¨†״§״¡ ״§„״£״µ†״§״ †…״³״­ ״¬…״¹ ״×״¹״¨״±״§״× ״¬״¯״¯()  ״§„״¨״±†״§…״¬
                //      ˆ†״³״×†״×״¬ ״£†ˆ״§״¹ ״§„״­‚ˆ„ …† ״§„ˆ״³״§״¦״· ״§„״¹„״©
                //      ƒ״°„ƒ †״­״¯‘״« ״£†ˆ״§״¹ …״¹״§…„״§״× ״§„״¨״§†״§״×  functionTable_
                // (EN) Phase 2B: Scan NewExpr to update class field types
                //      Now that classes are built, scan all NewExpr in program
                //      and infer field types from actual arguments
                //      Also update constructor param types in functionTable_
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                {
                    std::function<void(const Sad::AST::Expression *)> scanExprForNewExpr;
                    std::function<void(const Sad::AST::Statement *)> scanStmtForNewExpr;

                    scanExprForNewExpr = [&](const Sad::AST::Expression *expr)
                    {
                        if (!expr)
                            return;
                        if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                        {
                            auto sirClass = module_->getClass(newExpr->className);
                            if (sirClass && !sirClass->paramToFieldMap_.empty())
                            {
                                std::string ctorName = newExpr->className + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .״¨״§†
                                auto ctorFunc = module_->getFunction(ctorName);
                                auto ctorTableIt = functionTable_.find(ctorName);
                                if (ctorFunc)
                                {
                                    const auto &params = ctorFunc->getParameters();
                                    for (size_t i = 1; i < params.size() && (i - 1) < newExpr->arguments.size(); i++)
                                    {
                                        const std::string &paramName = params[i].name;
                                        auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                                        if (fieldIt != sirClass->paramToFieldMap_.end())
                                        {
                                            const std::string &fieldName = fieldIt->second;
                                            auto currentType = sirClass->fields_.find(fieldName);
                                            if (currentType != sirClass->fields_.end() &&
                                                currentType->second == SadTypeKind::Pointer)
                                            {
                                                const auto &arg = newExpr->arguments[i - 1];
                                                SadTypeKind argType = SadTypeKind::Pointer;
                                                if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(arg.get()))
                                                {
                                                    auto tt = lit->token.getType();
                                                    if (tt == Lexer::TokenType::STRING_LITERAL)
                                                        argType = SadTypeKind::String;
                                                    else if (tt == Lexer::TokenType::NUMBER_INTEGER)
                                                        argType = SadTypeKind::Integer;
                                                    else if (tt == Lexer::TokenType::NUMBER_DOUBLE)
                                                        argType = SadTypeKind::Float;
                                                    else if (tt == Lexer::TokenType::LITERAL_TRUE || tt == Lexer::TokenType::LITERAL_FALSE)
                                                        argType = SadTypeKind::Boolean;
                                                }
                                                else if (dynamic_cast<const Sad::AST::ArrayExpr *>(arg.get()))
                                                {
                                                    argType = SadTypeKind::Array;
                                                }

                                                if (argType != SadTypeKind::Pointer && argType != SadTypeKind::Void)
                                                {
                                                    sirClass->fields_[fieldName] = argType;
                                                    // (AR) ״×״­״¯״« ״£״¶״§‹ functionTable_ „„״¨״§†
                                                    if (ctorTableIt != functionTable_.end() && i < ctorTableIt->second.parameters.size())
                                                    {
                                                        if (ctorTableIt->second.parameters[i].type == SadTypeKind::Integer)
                                                            ctorTableIt->second.parameters[i].type = argType;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                        {
                            // (EN) Fix: Handle constructor calls via CallExpr (without "new" keyword)
                            //      In Sad, ClassName("arg") is parsed as CallExpr not NewExpr
                            //      Without this: field types don't get updated from implicit ctor args
                            if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                            {
                                auto callSirClass = module_->getClass(varExpr->name);
                                if (callSirClass && !callSirClass->paramToFieldMap_.empty())
                                {
                                    std::string ctorCallName = varExpr->name + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                    // (AR) نبحث في functionTable_ (مسجّل في Phase 1.35) بدلاً من module_
                                    //      لأن الباني لم يُضف إلى module_ بعد (يحدث في Phase 2)
                                    // (EN) Look up in functionTable_ (registered in Phase 1.35) instead of module_
                                    //      because constructor isn't added to module_ until Phase 2
                                    auto ctorIt = functionTable_.find(ctorCallName);
                                    if (ctorIt != functionTable_.end())
                                    {
                                        const auto &ctorParams = ctorIt->second.parameters;
                                        for (size_t ci = 1; ci < ctorParams.size() && (ci - 1) < call->arguments.size(); ci++)
                                        {
                                            const std::string &cpName = ctorParams[ci].name;
                                            auto cfIt = callSirClass->paramToFieldMap_.find(cpName);
                                            if (cfIt != callSirClass->paramToFieldMap_.end())
                                            {
                                                const std::string &cfName = cfIt->second;
                                                auto cfType = callSirClass->fields_.find(cfName);
                                                if (cfType != callSirClass->fields_.end() &&
                                                    cfType->second == SadTypeKind::Pointer)
                                                {
                                                    const auto &cArg = call->arguments[ci - 1];
                                                    SadTypeKind cArgType = SadTypeKind::Pointer;
                                                    if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(cArg.get()))
                                                    {
                                                        auto tt = lit->token.getType();
                                                        if (tt == Lexer::TokenType::STRING_LITERAL)
                                                            cArgType = SadTypeKind::String;
                                                        else if (tt == Lexer::TokenType::NUMBER_INTEGER)
                                                            cArgType = SadTypeKind::Integer;
                                                        else if (tt == Lexer::TokenType::NUMBER_DOUBLE)
                                                            cArgType = SadTypeKind::Float;
                                                        else if (tt == Lexer::TokenType::LITERAL_TRUE || tt == Lexer::TokenType::LITERAL_FALSE)
                                                            cArgType = SadTypeKind::Boolean;
                                                    }
                                                    else if (dynamic_cast<const Sad::AST::ArrayExpr *>(cArg.get()))
                                                    {
                                                        cArgType = SadTypeKind::Array;
                                                    }
                                                    else if (auto *varArg = dynamic_cast<const Sad::AST::VariableExpr *>(cArg.get()))
                                                    {
                                                        auto *varInfo = lookupVariable(varArg->name);
                                                        if (varInfo && varInfo->type != SadTypeKind::Void)
                                                            cArgType = varInfo->type;
                                                    }
                                                    if (cArgType != SadTypeKind::Pointer && cArgType != SadTypeKind::Void)
                                                    {
                                                        callSirClass->fields_[cfName] = cArgType;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            scanExprForNewExpr(call->callee.get());
                            for (const auto &arg : call->arguments)
                                scanExprForNewExpr(arg.get());
                        }
                        else if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                        {
                            scanExprForNewExpr(bin->left.get());
                            scanExprForNewExpr(bin->right.get());
                        }
                        else if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                        {
                            scanExprForNewExpr(assign->value.get());
                        }
                    };

                    scanStmtForNewExpr = [&](const Sad::AST::Statement *stmt)
                    {
                        if (!stmt)
                            return;
                        if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                        {
                            if (varDecl->initializer)
                                scanExprForNewExpr(varDecl->initializer.get());
                        }
                        else if (auto *exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                        {
                            if (exprStmt->expression)
                                scanExprForNewExpr(exprStmt->expression.get());
                        }
                        else if (auto *block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                        {
                            for (const auto &s : block->statements)
                                scanStmtForNewExpr(s.get());
                        }
                        else if (auto *ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                        {
                            scanStmtForNewExpr(ifStmt->thenBranch.get());
                            if (ifStmt->elseBranch)
                                scanStmtForNewExpr(ifStmt->elseBranch.get());
                        }
                        else if (auto *whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                        {
                            scanStmtForNewExpr(whileStmt->body.get());
                        }
                        else if (auto *forStmt = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                        {
                            scanStmtForNewExpr(forStmt->body.get());
                        }
                    };

                    for (const auto &stmt : *program)
                    {
                        if (stmt)
                            scanStmtForNewExpr(stmt.get());
                    }
                }

                // ═══════════════════════════════════════════════════════════════════════
                // (AR) المرحلة 2B2: نشر أنواع الحقول الموروثة من الأب إلى الابن
                //      المشكلة: عند نسخ حقول الأب في Phase 2A، قد تكون أنواعها لم تُحدَّث بعد
                //      (ستُحدَّث لاحقاً في Phase 2B). الحل: بعد Phase 2B، ننشر أنواع الحقول
                //      المُحدَّثة من الأب إلى جميع الأبناء عبر سلسلة الوراثة.
                // (EN) Phase 2B2: Propagate inherited field types from parent to child classes
                //      Problem: when copying parent fields in Phase 2A, their types may not be
                //      updated yet (Phase 2B updates them later). Fix: after Phase 2B, propagate
                //      updated field types from parent to all children down the inheritance chain.
                // ═══════════════════════════════════════════════════════════════════════
                {
                    const auto &allClasses = module_->getClasses();
                    // (AR) نمرر عدة مرات لضمان النشر في سلاسل الوراثة العميقة
                    //      (قاعدة → وسط → فرع): المرة الأولى تنشر من قاعدة لوسط،
                    //      المرة الثانية تنشر من وسط لفرع
                    // (EN) Multiple passes to handle deep chains (base->middle->child):
                    //      Pass 1 propagates base->middle, pass 2 propagates middle->child
                    for (size_t pass = 0; pass < allClasses.size(); pass++)
                    {
                        bool anyUpdated = false;
                        for (const auto &sirClass : allClasses)
                        {
                            if (sirClass->parentClass.empty())
                                continue;

                            auto parentSirClass = module_->getClass(sirClass->parentClass);
                            if (!parentSirClass)
                                continue;

                            // (AR) لكل حقل موروث: إذا كان الأب يملك نوعاً أدق، نحدّث الابن
                            // (EN) For each inherited field: if parent has more specific type, update child
                            for (const auto &fieldName : sirClass->fieldOrder_)
                            {
                                auto parentFieldIt = parentSirClass->fields_.find(fieldName);
                                if (parentFieldIt == parentSirClass->fields_.end())
                                    continue; // (AR) ليس حقلاً موروثاً / (EN) not an inherited field

                                auto childFieldIt = sirClass->fields_.find(fieldName);
                                if (childFieldIt == sirClass->fields_.end())
                                    continue;

                                // (AR) إذا الأب أدق (ليس Pointer) والابن لا يزال Pointer → حدّث
                                // (EN) If parent is more specific (not Pointer) and child is still Pointer -> update
                                if (parentFieldIt->second != SadTypeKind::Pointer &&
                                    childFieldIt->second == SadTypeKind::Pointer)
                                {
                                    sirClass->fields_[fieldName] = parentFieldIt->second;
                                    anyUpdated = true;
#ifndef NDEBUG
                                    std::cout << "[DEBUG] Phase2B2: propagated field '"
                                              << fieldName << "' type from parent '"
                                              << sirClass->parentClass << "' to child '"
                                              << sirClass->name << "'" << std::endl;
#endif
                                }
                            }
                        }
                        if (!anyUpdated)
                            break; // (AR) لا تحديثات — توقف مبكراً / (EN) No updates — early exit
                    }
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© 2״¬: ״¨†״§״¡ ״§„״¯ˆ״§„ ˆ״§„‚ˆ״§„״¨ ˆ״§„״µ״§״¯״±״§״× + ״¬…״¹ ״§„״¬…„ ״§„״×†״°״©
                // (EN) Phase 2C: Build functions, templates, exports + collect executable stmts
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

                // program ‡ˆ StmtList = std::vector<StmtPtr> (ast_node.h:170)
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    // (AR) ״§„״£״µ†״§ ˆ״§„״×״¹״¯״§״¯״§״× ˆ״§„״³…״§״× ״×… ״¨†״§״₪‡״§  ״§„…״±״­„״© 2״£ ג€” †״×״®״·״§‡״§
                    // (EN) Classes, enums, traits were built in Phase 2A ג€” skip them
                    if (dynamic_cast<Sad::AST::ClassDecl *>(stmt.get()))
                        continue;
                    if (dynamic_cast<Sad::AST::EnumDecl *>(stmt.get()))
                        continue;
                    if (dynamic_cast<Sad::AST::TraitDecl *>(stmt.get()))
                        continue;
                    if (dynamic_cast<Sad::AST::ImplDecl *>(stmt.get()))
                        continue;

                    // (AR) فضاء الأسماء: يُبنى في المرحلة 2 لأن أعضاءه (دوال/متغيرات/أصناف)
                    //      تعريفات على مستوى الوحدة وليست جملاً تنفيذية
                    // (EN) Namespace: build in Phase 2 because its members (functions/vars/classes)
                    //      are module-level declarations, not executable statements
                    if (auto nsDecl = dynamic_cast<Sad::AST::NamespaceDecl *>(stmt.get()))
                    {
                        buildStatement(nsDecl);
                        continue;
                    }

                    // (AR) ״¯״§„״©״ (declarations.h:19 - class FunctionDecl)
                    // (EN) Function declaration?
                    if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                    {
                        buildFunction(funcDecl);
                        // (AR) ״§„״×״­‚‚ ‡„ ‡ ״¯״§„״© ״±״¦״³״©
                        if (funcDecl->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" || funcDecl->name == "main")
                        {
                            hasMainFunction = true;
                        }
                        continue;
                    }

                    // (AR) ״¯״§„״© ‚״§„״¨״ (declarations.h:470 - class TemplateFunctionDecl)
                    // (EN) Template function declaration?
                    if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl *>(stmt.get()))
                    {
                        buildTemplateFunction(templateDecl);
                        continue;
                    }

                    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                    // (AR) †״¸״§… ״§„״§״³״×״±״§״¯ ˆ״§„״×״µ״¯״±  ״§„…״×״±״¬…
                    // (EN) Import/Export system in compiler
                    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

                    // (AR) ״×״µ״¯״± (״§„״¥״µ״¯״§״± ״§„״¬״¯״¯): ״µ״¯‘״± ״¯״§„״©|״µ†|…״×״÷״± ...
                    // (EN) Export declaration (new version): export function|class|var ...
                    if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                    {
                        if (exportDecl->declaration)
                        {
                            auto *innerStmt = exportDecl->declaration.get();
                            if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl *>(innerStmt))
                            {
                                buildFunction(innerFunc);
                                // (AR) ״×״¹„… ״§„״¯״§„״© ƒ…״µ״¯‘״±״© ג†’ ExternalLinkage  ˆ״¶״¹ ״§„ˆ״­״¯״©
                                // (EN) Mark function as exported ג†’ ExternalLinkage in module mode
                                if (currentFunction_)
                                {
                                    currentFunction_->isExported = true;
                                }
                                // (AR) ״§„״¨״­״« ״¹† ״§„״¯״§„״© ״¨״§„״§״³…  ״­״§„ currentFunction_ ״×״÷״±״×
                                auto exportedFn = module_->getFunction(innerFunc->name);
                                if (exportedFn)
                                {
                                    exportedFn->isExported = true;
                                }
                                if (innerFunc->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" || innerFunc->name == "main")
                                {
                                    hasMainFunction = true;
                                }
                            }
                            else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl *>(innerStmt))
                            {
                                buildClass(innerClass);
                            }
                            else
                            {
                                // (AR) ״£ ״×״µ״¯״± ״¢״®״± (…״×״÷״±״ ״¥„״®) ג†’ ״¬…„״© ״×†״°״©
                                topLevelStatements.push_back(stmt.get());
                            }
                        }
                        continue;
                    }

                    // (AR) ״×״µ״¯״± (״§„״¥״µ״¯״§״± ״§„‚״¯…): ״µ״¯‘״± ...
                    // (EN) Export statement (legacy): export ...
                    if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                    {
                        if (exportStmt->declaration)
                        {
                            auto *innerStmt = exportStmt->declaration.get();

                            if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl *>(innerStmt))
                            {
                                buildFunction(innerFunc);
                                // (AR) ״×״¹„… ״§„״¯״§„״© ƒ…״µ״¯‘״±״©
                                // (EN) Mark function as exported
                                auto exportedFn = module_->getFunction(innerFunc->name);
                                if (exportedFn)
                                {
                                    exportedFn->isExported = true;
                                }
                            }
                            else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl *>(innerStmt))
                            {
                                buildClass(innerClass);
                            }
                            else
                            {
                                topLevelStatements.push_back(stmt.get());
                            }
                        }
                        continue;
                    }

                    // (AR) ״§״³״×״±״§״¯ ״§†״×‚״§״¦: …† ˆ״­״¯״© ״§״³״×ˆ״±״¯ ...
                    // (EN) Selective import: from module import ...
                    if (auto fromImport = dynamic_cast<Sad::AST::FromImportStmt *>(stmt.get()))
                    {
                        buildFromImportStmt(fromImport);
                        continue;
                    }

                    // (AR) ״§״³״×״±״§״¯ ƒ״§…„: ״§״³״×ˆ״±״¯ ˆ״­״¯״©
                    // (EN) Full import: import module
                    if (auto importStmt = dynamic_cast<Sad::AST::ImportStmt *>(stmt.get()))
                    {
                        buildImportStmt(importStmt);
                        continue;
                    }

                    // (AR) ״¥״¹״§״¯״© ״§„״×״µ״¯״±: ״µ״¯‘״± * …† ˆ״­״¯״© / ״µ״¯‘״± ״¹†״µ״±1 …† ˆ״­״¯״©
                    // (EN) Re-export: export * from module / export item1 from module
                    if (auto reExportStmt = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get()))
                    {
                        buildStatement(reExportStmt);
                        continue;
                    }

                    // (AR) ƒ״×„״© ״§„״§…״×״¯״§״¯: ״§…״×״¯״§״¯ ״µ† ... †‡״§״©
                    //      ״×״¹״§„״¬  ״§„…״±״­„״© 2 „״£† ״§„״¯ˆ״§„ ״§„…״¶״§״© ״¬״¨ ״£† ״×ƒˆ† …״×״§״­״© „״¬…״¹ ״§„״¯ˆ״§„
                    // (EN) Extension block: extension class ... end
                    //      Processed in Phase 2 so added methods are available to all functions
                    if (auto extensionDecl = dynamic_cast<Sad::AST::ExtensionDecl *>(stmt.get()))
                    {
                        buildStatement(extensionDecl);
                        continue;
                    }

                    // (AR) ״§״³… …״³״×״¹״§״± „„†ˆ״¹: †ˆ״¹ ״§״³… = †ˆ״¹_‡״¯
                    //      ״¹״§„״¬  ״§„…״±״­„״© 2 „״£† ״§„״£״³…״§״¡ ״¬״¨ ״£† ״×ƒˆ† …״×״§״­״© …״¨ƒ״±״§‹
                    // (EN) Type alias: type name = target_type
                    //      Processed in Phase 2 so aliases are available early
                    if (auto typeAliasDecl = dynamic_cast<Sad::AST::TypeAliasDecl *>(stmt.get()))
                    {
                        buildStatement(typeAliasDecl);
                        continue;
                    }

                    // (AR) ״¨†״©: ״¨†״© ״§״³… ... †‡״§״©
                    //      ״¹״§„״¬  ״§„…״±״­„״© 2 …״«„ ״§„״£״µ†״§
                    // (EN) Struct: struct name ... end
                    //      Processed in Phase 2 like classes
                    if (auto structDecl = dynamic_cast<Sad::AST::StructDecl *>(stmt.get()))
                    {
                        buildStatement(structDecl);
                        continue;
                    }

                    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                    // (AR) ״¬…„״© ״×†״°״©  ״§„…״³״×ˆ‰ ״§„״£״¹„‰ (״·״¨״§״¹״©״ …״×״÷״±״ ״´״±״·״ ״­„‚״©...)
                    // (EN) Top-level executable statement (print, var, if, while...)
                    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                    topLevelStatements.push_back(stmt.get());
                }

                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״§„…״±״­„״© ״§„״«״§„״«״©: ״¥†״´״§״¡ ״¯״§„״© __sad_main „״×״÷„ ״§„ƒˆ״¯ ״§„״×†״°
                // (EN) Phase 3: Create __sad_main to wrap top-level executable code
                // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
                // (AR) ״¥״°״§ ״×ˆ״¬״¯ ״¬…„ ״×†״°״© ˆ„״§ ״×ˆ״¬״¯ ״¯״§„״© ״±״¦״³״© ״µ״±״­״©
                //      ˆ„״³  ˆ״¶״¹ ״§„ˆ״­״¯״© (--module)
                // (EN) If there are executable statements and no explicit main function
                //      and not in module mode (--module)
                if (!topLevelStatements.empty() && !hasMainFunction && !moduleMode_)
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildModule: creating __sad_main with "
                              << topLevelStatements.size() << " top-level statements" << std::endl;
#endif

                    // (AR) ״¥†״´״§״¡ ״¯״§„״© __sad_main …† †ˆ״¹ void ״¨״¯ˆ† …״¹״§…„״§״×
                    // (EN) Create __sad_main function: void __sad_main()
                    auto sadMainFunc = std::make_shared<SIRFunction>("__sad_main", SadTypeKind::Void);

                    // (AR) ״­״¸ ״§„״­״§„״© ״§„״­״§„״©
                    // (EN) Save current state
                    auto prevFunction = currentFunction_;
                    auto prevBlock = currentBlock_;

                    currentFunction_ = sadMainFunc;

                    // (AR) ״¥†״´״§״¡ †״·״§‚ ״¬״¯״¯ „„״¯״§„״© ״§„״±״¦״³״©
                    // (EN) Create new scope for main function
                    enterScope();

                    // (AR) ״¥†״´״§״¡ basic block „„״¯״®ˆ„
                    // (EN) Create entry basic block
                    auto entryBlock = createBasicBlock(kEntryBlockName);
                    sadMainFunc->addBasicBlock(entryBlock);
                    currentBlock_ = entryBlock;

                    // (AR) ״¨†״§״¡ ״¬…״¹ ״§„״¬…„ ״§„״×†״°״©
                    // (EN) Build all executable statements
                    for (auto *stmtNode : topLevelStatements)
                    {
                        if (stmtNode)
                        {
                            buildStatement(stmtNode);
                        }
                    }

                    // (AR) ״§„״®״±ˆ״¬ …† ״§„†״·״§‚
                    // (EN) Exit scope
                    exitScope();

                    // (AR) ״§„״×״£ƒ״¯ …† ˆ״¬ˆ״¯ terminator
                    // (EN) Ensure terminator exists
                    if (currentBlock_)
                    {
                        bool hasTerminator = false;
                        if (!currentBlock_->instructions.empty())
                        {
                            const auto &lastInst = currentBlock_->instructions.back();
                            hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                             lastInst.opcode == SIROpcode::RET_VOID);
                        }
                        if (!hasTerminator)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            currentBlock_->addInstruction(retInst);
                        }
                    }

                    // (AR) ״¥״¶״§״© ״§„״¯״§„״© „„ˆ״­״¯״©
                    // (EN) Add function to module
                    module_->addFunction(sadMainFunc);

                    // (AR) ״×״³״¬„  ״¬״¯ˆ„ ״§„״¯ˆ״§„
                    // (EN) Register in function table
                    {
                        FunctionInfo funcInfo;
                        funcInfo.name = "__sad_main";
                        funcInfo.returnType = SadTypeKind::Void;
                        funcInfo.sirFunction = sadMainFunc;
                        functionTable_["__sad_main"] = funcInfo;
                    }

                    // (AR) ״§״³״×״¹״§״¯״© ״§„״­״§„״© ״§„״³״§״¨‚״©
                    // (EN) Restore previous state
                    currentFunction_ = prevFunction;
                    currentBlock_ = prevBlock;
                }

                // (AR) ״§„״®״±ˆ״¬ …† ״§„†״·״§‚ ״§„״¹״§… ״§„״° ״£†״´״£†״§‡  ״§„…״±״­„״© 1.5
                // (EN) Exit the global scope we created in Phase 1.5
                exitScope();

                return module_;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
