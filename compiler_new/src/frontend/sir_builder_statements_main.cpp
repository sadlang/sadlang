// ============================================================================
// sir_builder_statements_main.cpp - buildStatement implementation
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

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void SIRBuilder::buildStatement(AST::Statement *stmt)
            {
                if (!stmt)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildStatement: processing statement" << std::endl;
#endif

                // (AR) BlockStmt - كتلة من الجمل (statements.h:423)
                // (EN) Block statement
                if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found BlockStmt with " << blockStmt->statements.size() << " statements" << std::endl;
#endif
                    // (AR) معالجة كل جملة في الكتلة
                    // (EN) Process each statement in the block
                    for (const auto &innerStmt : blockStmt->statements)
                    {
                        buildStatement(innerStmt.get());
                    }
                    return;
                }

                // (AR) FunctionDecl — تعريف دالة متداخلة (declarations.h:19)
                //      عندما يُعرَّف تعريف دالة داخل جسم دالة أخرى (مثل دالة رئيسية)،
                //      نبنيها كدالة مستقلة في الوحدة (module-level) لأن LLVM لا يدعم
                //      الدوال المتداخلة. نحفظ ونستعيد حالة الدالة الحالية والكتلة الحالية
                //      والنطاق حتى لا يتأثر سياق الدالة الأم.
                // (EN) Nested function declaration (declarations.h:19)
                //      When a function declaration appears inside another function body
                //      (e.g. inside main), we build it as a module-level function since
                //      LLVM doesn't support nested functions. We save/restore the current
                //      function, block, and scope so the parent function context is preserved.
                if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(stmt))
                {
                    // (AR) حفظ حالة الدالة الأم (الدالة الحالية، الكتلة الحالية)
                    // (EN) Save parent function state
                    auto savedCtxNested = saveContext();

                    // (AR) بناء الدالة المتداخلة كدالة مستقلة في الوحدة
                    // (EN) Build nested function as module-level function
                    buildFunction(funcDecl);

                    // (AR) استعادة حالة الدالة الأم
                    // (EN) Restore parent function state
                    restoreContext(std::move(savedCtxNested));
                    return;
                }

                // (AR) IfStmt - جملة if (statements.h:104)
                // (EN) If statement
                if (auto ifStmt = dynamic_cast<Sad::AST::IfStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found IfStmt" << std::endl;
#endif
                    buildIfStatement(ifStmt);
                    return;
                }

                // (AR) WhileStmt - حلقة while (statements.h:143)
                // (EN) While loop
                if (auto whileStmt = dynamic_cast<Sad::AST::WhileStmt *>(stmt))
                {
                    buildWhileLoop(whileStmt);
                    return;
                }

                // (AR) ForStmt - حلقة for (statements.h:187)
                // (EN) For loop
                if (auto forStmt = dynamic_cast<Sad::AST::ForStmt *>(stmt))
                {
                    buildForLoop(forStmt);
                    return;
                }

                // (AR) ForRangeStmt - حلقة لكل في (statements.h:228)
                // (EN) For-range loop (foreach)
                if (auto forRangeStmt = dynamic_cast<Sad::AST::ForRangeStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ForRangeStmt" << std::endl;
#endif
                    buildForRangeLoop(forRangeStmt);
                    return;
                }

                // (AR) ReturnStmt - جملة return (statements.h:266)
                // (EN) Return statement
                if (auto returnStmt = dynamic_cast<Sad::AST::ReturnStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ReturnStmt" << std::endl;
#endif
                    buildReturnStatement(returnStmt);
                    return;
                }

                // (AR) BreakStmt - جملة break (statements.h:360)
                // (EN) Break statement
                if (auto breakStmt = dynamic_cast<Sad::AST::BreakStmt *>(stmt))
                {
                    buildBreakStatement(breakStmt);
                    return;
                }

                // (AR) ContinueStmt - جملة continue (statements.h:392)
                // (EN) Continue statement
                if (auto continueStmt = dynamic_cast<Sad::AST::ContinueStmt *>(stmt))
                {
                    buildContinueStatement(continueStmt);
                    return;
                }

                // (AR) ExprStmt - جملة تعبير (statements.h:38)
                // (EN) Expression statement
                if (auto exprStmt = dynamic_cast<Sad::AST::ExprStmt *>(stmt))
                {
                    // (AR) تنفيذ التعبير فقط (نتجاهل النتيجة)
                    // (EN) Execute expression only (ignore result)
                    if (exprStmt->expression)
                    {
                        buildExpression(exprStmt->expression.get());
                    }
                    return;
                }

                // (AR) VarDeclStmt - تصريح متغير (statements.h:74)
                // (EN) Variable declaration
                if (auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found VarDeclStmt: " << varDecl->name << std::endl;
#endif
                    buildLocalVariable(varDecl);
                    return;
                }

                // ================================================================
                // (AR) MultiVarDeclStmt - تصريح متغيرات متعددة بفاصلة
                //      مثال: متغير اسم = "أحمد"، عمر = 20، وظيفة = "مهندس"
                //      نُفكّك إلى تصريحات VarDeclStmt فردية ونبنيها واحدة تلو الأخرى
                // (EN) Multiple variable declarations separated by comma
                //      Example: var name = "Ahmad", age = 20, job = "Engineer"
                //      We unpack to individual VarDeclStmt and build each one
                // ================================================================
                if (auto multiVarDecl = dynamic_cast<Sad::AST::MultiVarDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found MultiVarDeclStmt with " << multiVarDecl->declarations.size() << " declarations" << std::endl;
#endif
                    for (auto &decl : multiVarDecl->declarations)
                    {
                        if (decl)
                        {
                            buildStatement(decl.get());
                        }
                    }
                    return;
                }

                // (AR) TupleDestructureStmt - تفكيك صف إلى متغيرات: متغير (أ، ب) = صف
                // (EN) Tuple destructuring: var (a, b) = tuple
                if (auto tupleDestr = dynamic_cast<Sad::AST::TupleDestructureStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TupleDestructureStmt with " << tupleDestr->names.size() << " names" << std::endl;
#endif
                    // (AR) بناء تعبير الصف المصدر
                    // (EN) Build source tuple expression
                    auto tupleResult = buildExpression(tupleDestr->initializer.get());

                    // (AR) استخراج كل عنصر وتسجيله كمتغير محلي
                    // (EN) Extract each element and register as local variable
                    for (size_t i = 0; i < tupleDestr->names.size(); ++i)
                    {
                        const std::string &name = tupleDestr->names[i];
                        if (name == "_")
                            continue; // (AR) تجاهل / (EN) Skip placeholder

                        // (AR) استخراج العنصر من الصف باستخدام TUPLE_GET
                        // (EN) Extract element from tuple using TUPLE_GET
                        std::string elemReg = newTempRegister();
                        if (currentBlock_)
                        {
                            SIRInstruction getInst(SIROpcode::TUPLE_GET);
                            getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                            getInst.operands.push_back(SIROperand::Register(tupleResult.registerName, tupleResult.type));
                            getInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                            getInst.comment = "tuple destructure: " + name + " = tuple[" + std::to_string(i) + "]";
                            currentBlock_->addInstruction(getInst);
                        }

                        // (AR) تسجيل المتغير المحلي
                        // (EN) Register local variable
                        VariableInfo varInfo;
                        varInfo.name = name;
                        varInfo.type = SadTypeKind::Integer;
                        varInfo.registerName = "%" + name;
                        varInfo.isGlobal = false;
                        varInfo.isMutable = !tupleDestr->isConst;
                        varInfo.scopeLevel = currentScopeLevel_;
                        addVariable(varInfo);

                        // (AR) تخزين القيمة المستخرجة في السجل
                        // (EN) Store extracted value in register
                        if (currentBlock_)
                        {
                            SIRInstruction storeInst(SIROpcode::STORE);
                            storeInst.operands.push_back(SIROperand::Register(varInfo.registerName, SadTypeKind::Integer));
                            storeInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                            storeInst.comment = "store destructured: " + name;
                            currentBlock_->addInstruction(storeInst);
                        }
                    }
                    return;
                }

                // (AR) TemplateFunctionDecl - تصريح دالة قالب (declarations.h:470)
                // (EN) Template function declaration
                if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TemplateFunctionDecl: " << templateDecl->name << std::endl;
#endif
                    buildTemplateFunction(templateDecl);
                    return;
                }

                // (AR) TemplateClassDecl - تصريح صنف قالب (declarations.h:520)
                // (EN) Template class declaration — store for later instantiation
                if (auto templateClassDecl = dynamic_cast<Sad::AST::TemplateClassDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TemplateClassDecl: " << templateClassDecl->name
                              << " with " << templateClassDecl->typeParameters.size() << " type params" << std::endl;
#endif

                    // (AR) تسجيل صنف القالب — يُنشأ عند TemplateInstantiation
                    // (EN) Register template class — instantiated at TemplateInstantiation
                    templateClasses_[templateClassDecl->name] = templateClassDecl;

                    // (AR) إنشاء SIRClass فارغ بالاسم الأساسي ليُعرف في الجدول
                    // (EN) Create empty SIRClass with base name so it's known
                    std::string parentClass;
                    if (!templateClassDecl->superclasses.empty())
                    {
                        parentClass = templateClassDecl->superclasses[0];
                    }
                    auto sirClass = std::make_shared<SIRClass>(templateClassDecl->name, parentClass);

                    for (const auto &tp : templateClassDecl->typeParameters)
                    {
                        sirClass->addField(tp.name, SadTypeKind::Pointer);
                    }

                    module_->addClass(sirClass);
                    return;
                }

                // (AR) MatchStmt - جملة match (pattern_nodes.h:MatchStmt)
                // (EN) Match statement (pattern matching)
                if (auto matchStmt = dynamic_cast<Sad::AST::MatchStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found MatchStmt" << std::endl;
#endif
                    buildMatchStatement(matchStmt);
                    return;
                }

                // ========================================================================
                // (AR) DeferStmt - جملة التأجيل (أجّل) — تنظيف مضمون عند خروج الدالة
                // (EN) Defer statement — guaranteed cleanup on function exit
                //
                // (AR) في المترجم: نُسجّل الجملة المؤجلة في قائمة الدالة الحالية.
                //      عند بناء buildReturnStatement، نُصدر كود الـ defer قبل RET.
                //      هذا مشابه لكيفية تنفيذ defer في llvm.org/docs/LangImpl10.html
                // (EN) In the compiler: register deferred statement in current function.
                //      When buildReturnStatement is called, emit defer code before RET.
                //      Similar to how defer is implemented via cleanup blocks.
                // ========================================================================
                if (auto deferStmt = dynamic_cast<Sad::AST::DeferStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found DeferStmt — registering deferred statement" << std::endl;
#endif
                    if (!currentDeferStackReg_.empty())
                    {
                        BuildResult closureResult = buildDeferredClosure(deferStmt->body.get());
                        if (!closureResult.registerName.empty() && currentBlock_)
                        {
                            SIRInstruction appendInst(SIROpcode::BUILTIN_ARRAY_APPEND);
                            appendInst.result = SIROperand::Register(newTempRegister(), SadTypeKind::Void);
                            appendInst.operands.push_back(SIROperand::Register(currentDeferStackReg_, SadTypeKind::Array));
                            appendInst.operands.push_back(SIROperand::Register(closureResult.registerName, SadTypeKind::Function));
                            appendInst.comment = "register deferred closure at runtime";
                            currentBlock_->addInstruction(appendInst);
                        }
                        return;
                    }

                    // (AR) تسجيل الجملة المؤجلة في مكدس الدالة الحالية
                    // (EN) Register deferred statement in current function's stack
                    if (currentFunction_)
                    {
                        deferredStatements_.push_back(deferStmt->body.get());
                    }
                    return;
                }

                // ========================================================================
                // (AR) SwitchStmt - جملة حالة/switch (statements.h:SwitchStmt)
                // (EN) Switch statement
                // ========================================================================
                if (auto switchStmt = dynamic_cast<Sad::AST::SwitchStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found SwitchStmt with " << switchStmt->cases.size() << " cases" << std::endl;
#endif

                    // (AR) بناء تعبير الحالة
                    // (EN) Build switch expression
                    auto exprResult = buildExpression(switchStmt->expression.get());

                    // (AR) إنشاء كتل لكل حالة وكتلة الخروج
                    // (EN) Create blocks for each case and exit block
                    std::string exitLabel = newLabel("switch_exit");
                    auto exitBlock = createBasicBlock(exitLabel);

                    std::string defaultLabel = exitLabel;
                    if (switchStmt->defaultCase)
                    {
                        defaultLabel = newLabel("switch_default");
                    }

                    // (AR) بناء سلسلة من فروع شرطية لكل حالة
                    // (EN) Build chain of conditional branches for each case
                    std::vector<std::pair<std::string, std::shared_ptr<SIRBasicBlock>>> caseBlocks;

                    for (size_t i = 0; i < switchStmt->cases.size(); ++i)
                    {
                        std::string caseLabel = newLabel("switch_case_" + std::to_string(i));
                        auto caseBlock = createBasicBlock(caseLabel);
                        caseBlocks.push_back({caseLabel, caseBlock});
                    }

                    // (AR) بناء شروط المقارنة
                    // (EN) Build comparison conditions
                    for (size_t i = 0; i < switchStmt->cases.size(); ++i)
                    {
                        auto &caseBranch = switchStmt->cases[i];

                        // (AR) بناء تعبير قيمة الحالة
                        // (EN) Build case value expression
                        auto caseVal = buildExpression(caseBranch.value.get());

                        // (AR) مقارنة المساواة
                        // (EN) Equality comparison
                        std::string cmpReg = newTempRegister();
                        SIRInstruction cmpInst = SIRInstruction::Binary(
                            SIROpcode::EQ,
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Register(exprResult.registerName, exprResult.type),
                            SIROperand::Register(caseVal.registerName, caseVal.type));
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(cmpInst);
                        }

                        // (AR) التحديد: إذا تطابقت القيمة، اذهب لكتلة الحالة
                        // (EN) Branch: if matched, go to case block
                        std::string nextCheckLabel;
                        if (i + 1 < switchStmt->cases.size())
                        {
                            nextCheckLabel = newLabel("switch_check_" + std::to_string(i + 1));
                        }
                        else
                        {
                            nextCheckLabel = defaultLabel;
                        }

                        auto nextCheckBlock = createBasicBlock(nextCheckLabel);

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::BranchCond(
                                SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Label(caseBlocks[i].first),
                                SIROperand::Label(nextCheckLabel)));
                        }

                        if (currentFunction_)
                        {
                            currentFunction_->addBasicBlock(nextCheckBlock);
                        }
                        currentBlock_ = nextCheckBlock;
                    }

                    // (AR) بناء أجسام الحالات
                    // (EN) Build case bodies
                    for (size_t i = 0; i < switchStmt->cases.size(); ++i)
                    {
                        if (currentFunction_)
                        {
                            currentFunction_->addBasicBlock(caseBlocks[i].second);
                        }
                        currentBlock_ = caseBlocks[i].second;

                        enterScope();
                        if (switchStmt->cases[i].body)
                        {
                            buildStatement(switchStmt->cases[i].body.get());
                        }
                        exitScope();

                        // (AR) لا fall-through — القفز مباشرة للخروج
                        // (EN) No fall-through — jump directly to exit
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                    }

                    // (AR) بناء الحالة الافتراضية
                    // (EN) Build default case
                    if (switchStmt->defaultCase)
                    {
                        auto defaultBlock = createBasicBlock(defaultLabel);
                        if (currentFunction_)
                        {
                            currentFunction_->addBasicBlock(defaultBlock);
                        }
                        currentBlock_ = defaultBlock;

                        enterScope();
                        buildStatement(switchStmt->defaultCase.get());
                        exitScope();

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                    }

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(exitBlock);
                    }
                    currentBlock_ = exitBlock;
                    return;
                }

                // === Delegation to sub-functions ===

                // Exception handling (try/catch/finally, raise)
                if (buildStatement_Exceptions(stmt))
                    return;

                // Generator statements (yield, with)
                if (buildStatement_Generators(stmt))
                    return;

                // Type declarations (enum, struct, namespace, class)
                if (buildStatement_Types(stmt))
                    return;

                // Advanced statements (test, atomic, macro, go, select, etc.)
                if (buildStatement_Advanced(stmt))
                    return;

#ifndef NDEBUG
                std::cout << "[DEBUG] Unknown statement type: " << typeid(*stmt).name() << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
