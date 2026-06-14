// ============================================================================
// sir_builder_stmt_extension.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
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
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool StatementBuilder::buildStatement_Extension(AST::Statement *stmt)
            {

                // ========================================================================
                // (AR) ExtensionDecl - كتلة الامتداد: امتداد نوع_هدف ... نهاية
                //      تُضيف دوالاً جديدة لصنف موجود بدون تعديل تعريفه الأصلي
                //      كل دالة في كتلة الامتداد تُسجَّل كطريقة في الصنف الهدف
                //      مثال:
                //        امتداد نقطة
                //            دالة وصف()
                //                ارجع "(" + هذا.س + "، " + هذا.ص + ")"
                //            نهاية
                //        نهاية
                // (EN) Extension block: extension target_type ... end
                //      Adds new methods to an existing class without modifying its definition
                //      Each function in extension block is registered as a method on the target class
                // ========================================================================
                if (auto extensionDecl = dynamic_cast<Sad::AST::ExtensionDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ExtensionDecl for type: '" << extensionDecl->targetType << "'" << std::endl;
#endif
                    // (AR) حفظ اسم الصنف الحالي واستعادته بعد الانتهاء
                    // (EN) Save current class name and restore after finishing
                    std::string savedClassName = b_.currentClassName_;
                    b_.currentClassName_ = extensionDecl->targetType;

                    // (AR) معالجة كل دالة في كتلة الامتداد كطريقة في الصنف الهدف
                    // (EN) Process each function in extension block as a method on the target class
                    for (auto &method : extensionDecl->methods)
                    {
                        if (!method)
                            continue;

                        auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(method.get());
                        if (!funcDecl)
                            continue;

#ifndef NDEBUG
                        std::cout << "[DEBUG] ExtensionDecl: adding method '" << funcDecl->name
                                  << "' to class '" << extensionDecl->targetType << "'" << std::endl;
#endif

                        // (AR) إنشاء اسم الطريقة الكامل: صنف.اسم_الطريقة
                        // (EN) Create full method name: class.method_name
                        std::string fullMethodName = extensionDecl->targetType + "." + funcDecl->name;

                        // (AR) استنتاج نوع الإرجاع
                        // (EN) Infer return type
                        SadTypeKind inferredReturnType = SadTypeKind::Void;
                        if (funcDecl->body)
                        {
                            inferredReturnType = b_.inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                        }

                        // (AR) إنشاء دالة SIR للطريقة
                        // (EN) Create SIR function for the method
                        auto sirFunc = std::make_shared<SIRFunction>(fullMethodName, inferredReturnType);

                        // (AR) إضافة معامل self كأول معامل
                        // (EN) Add self as first parameter
                        sirFunc->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        // (AR) إضافة بقية المعاملات
                        // (EN) Add remaining parameters
                        for (auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = SadTypeKind::Integer;
                            if (param.type != Sad::Types::SadTypeKind::Unknown)
                            {
                                paramType = b_.astTypeToSIRType(param.type);
                            }
                            sirFunc->addParameter(SIRParameter(param.name, paramType));
                        }

                        // (AR) تسجيل الدالة في الوحدة وجدول الدوال
                        // (EN) Register function in module and function table
                        b_.module_->addFunction(sirFunc);

                        FunctionInfo funcInfo;
                        funcInfo.name = fullMethodName;
                        funcInfo.returnType = inferredReturnType;
                        funcInfo.parameters = sirFunc->parameters;
                        funcInfo.sirFunction = sirFunc;
                        funcInfo.astDecl = funcDecl;
                        b_.functionTable_[fullMethodName] = funcInfo;

                        // (AR) بناء جسم الطريقة
                        // (EN) Build method body
                        auto savedCtxExt = b_.saveContext();

                        b_.currentFunction_ = sirFunc;
                        auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                        sirFunc->addBasicBlock(entryBlock);
                        b_.currentBlock_ = entryBlock;

                        b_.enterScope();

                        // (AR) تسجيل self
                        VariableInfo selfInfo;
                        selfInfo.name = kSelfParamName;
                        selfInfo.type = SadTypeKind::Integer;
                        selfInfo.registerName = kSelfRegisterName;
                        selfInfo.isMutable = false;
                        selfInfo.className = extensionDecl->targetType;
                        b_.addVariable(selfInfo);
                        b_.classInstanceTypes_[kSelfRegisterName] = extensionDecl->targetType;

                        // (AR) تسجيل المعاملات
                        // (EN) Register parameters
                        for (size_t i = 0; i < funcDecl->parameters.size(); ++i)
                        {
                            VariableInfo paramInfo;
                            paramInfo.name = funcDecl->parameters[i].name;
                            paramInfo.type = sirFunc->parameters[i + 1].type;
                            paramInfo.registerName = "%" + funcDecl->parameters[i].name;
                            paramInfo.isParameter = true;
                            b_.addVariable(paramInfo);
                        }

                        // (AR) بناء الجسم
                        if (funcDecl->body)
                        {
                            buildStatement(funcDecl->body.get());
                        }

                        // (AR) إضافة return void إذا لم يكن هناك terminator
                        if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                        {
                            SIRInstruction retInst(SIROpcode::RET);
                            retInst.comment = "implicit return from extension method";
                            b_.currentBlock_->addInstruction(retInst);
                        }

                        b_.exitScope();

                        b_.restoreContext(std::move(savedCtxExt));
                    }

                    b_.currentClassName_ = savedClassName;
                    return true;
                }

                // ========================================================================
                // (AR) TypeAliasDecl - تصريح اسم مستعار للنوع: نوع اسم = نوع_هدف
                //      يُسجّل الاسم المستعار كمرادف للنوع الهدف
                //      مثال: نوع عدد = رقم
                //      مثال: نوع قائمة = مصفوفة
                //      في سياق المترجم: نُسجّل الاسم كصنف بنفس بنية الهدف
                //      أو نُسجّل كمرادف في جدول الأنواع
                // (EN) Type alias declaration: type name = target_type
                //      Registers the alias as equivalent to the target type
                //      In compiler context: register as a synonym in type table
                // ========================================================================
                if (auto typeAliasDecl = dynamic_cast<Sad::AST::TypeAliasDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TypeAliasDecl: '" << typeAliasDecl->name << "'" << std::endl;
#endif

                    // (AR) إذا كان الهدف اسم صنف → نُسجّل الاسم المستعار ككائن بنفس بنية الصنف
                    //      إذا كان نوعاً بدائياً → نتجاهل (الأنواع البدائية لا تحتاج alias في SIR)
                    // (EN) If target is a class name → register alias as object with same class structure
                    //      If primitive type → ignore (primitives don't need alias in SIR)
                    if (typeAliasDecl->target)
                    {
                        auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(typeAliasDecl->target.get());
                        if (varExpr)
                        {
                            std::string targetName = varExpr->name;

                            // (AR) فحص هل الهدف صنف مسجل
                            // (EN) Check if target is a registered class
                            auto targetClass = b_.module_ ? b_.module_->getClass(targetName) : nullptr;
                            if (targetClass)
                            {
                                // (AR) إنشاء صنف SIR جديد بنفس البنية
                                // (EN) Create new SIR class with same structure
                                auto aliasClass = std::make_shared<SIRClass>(typeAliasDecl->name);
                                aliasClass->parentClass = targetClass->parentClass;
                                aliasClass->fields_ = targetClass->fields_;
                                b_.module_->addClass(aliasClass);
                                b_.classTable_[typeAliasDecl->name] = aliasClass;

                                // (AR) نسخ جميع دوال الصنف الهدف مع الاسم الجديد
                                //      نجمع الإدخالات أولاً ثم نضيفها (تجنب التعديل أثناء التكرار)
                                // (EN) Copy all target class functions with the new name
                                //      Collect entries first then insert (avoid modifying during iteration)
                                std::vector<std::pair<std::string, FunctionInfo>> newEntries;
                                for (auto &[funcName, funcInfo] : b_.functionTable_)
                                {
                                    if (funcName.find(targetName + ".") == 0)
                                    {
                                        std::string newFuncName = typeAliasDecl->name + funcName.substr(targetName.size());
                                        if (b_.functionTable_.find(newFuncName) == b_.functionTable_.end())
                                        {
                                            FunctionInfo aliasFunc = funcInfo;
                                            aliasFunc.name = newFuncName;
                                            newEntries.push_back({newFuncName, aliasFunc});
                                        }
                                    }
                                }
                                for (auto &[name, info] : newEntries)
                                {
                                    b_.functionTable_[name] = info;
                                }

                                // (AR) نسخ دوال SIR الفعلية في الوحدة (مطلوب للربط)
                                //      نستنسخ كل دالة من الصنف الأصلي باسم الصنف المستعار
                                //      هذا يضمن أن الرابط يجد حيوان_أليف.بناء / حيوان_أليف.وصف
                                // (EN) Clone actual SIR functions in module (required for linking)
                                //      Clone each function from original class with alias class name
                                if (b_.module_)
                                {
                                    std::vector<std::shared_ptr<SIRFunction>> newFuncs;
                                    for (auto &func : b_.module_->getFunctions())
                                    {
                                        if (func && func->name.find(targetName + ".") == 0)
                                        {
                                            std::string newFuncName = typeAliasDecl->name + func->name.substr(targetName.size());
                                            // (AR) تحقق أن الدالة غير موجودة بالفعل
                                            bool exists = false;
                                            for (auto &f : b_.module_->getFunctions())
                                            {
                                                if (f && f->name == newFuncName)
                                                {
                                                    exists = true;
                                                    break;
                                                }
                                            }
                                            if (!exists)
                                            {
                                                // (AR) استنساخ الدالة الأصلية بالكامل باسم جديد
                                                //      نسخ المعاملات والكتل والتعليمات كما هي
                                                // (EN) Clone original function completely with new name
                                                //      Copy parameters, blocks, and instructions as-is
                                                auto clonedFunc = std::make_shared<SIRFunction>(newFuncName, func->returnType);
                                                clonedFunc->parameters = func->parameters;
                                                clonedFunc->basicBlocks = func->basicBlocks;
                                                newFuncs.push_back(clonedFunc);
                                            }
                                        }
                                    }
                                    for (auto &f : newFuncs)
                                    {
                                        b_.module_->addFunction(f);
                                    }
                                }

#ifndef NDEBUG
                                std::cout << "[DEBUG] TypeAlias: registered '" << typeAliasDecl->name
                                          << "' as alias for class '" << targetName << "'" << std::endl;
#endif
                            }
                            else
                            {
#ifndef NDEBUG
                                std::cout << "[DEBUG] TypeAlias: '" << typeAliasDecl->name
                                          << "' = '" << targetName << "' (primitive alias, no SIR class)" << std::endl;
#endif
                            }
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) ReExportStmt - إعادة التصدير: صدّر * من وحدة / صدّر عنصر1 من وحدة
                //      يستورد الوحدة ثم يُعيد تصدير رموزها
                //      في سياق المترجم: نُعامل كاستيراد عادي ثم نُسجّل الرموز للتصدير
                // (EN) Re-export: export * from module / export item1 from module
                //      Imports the module then re-exports its symbols
                //      In compiler context: treat as normal import then register symbols for export
                // ========================================================================
                if (auto reExportStmt = dynamic_cast<Sad::AST::ReExportStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ReExportStmt from module: ";
                    for (auto &seg : reExportStmt->modulePath)
                        std::cout << seg << ".";
                    std::cout << " wildcard=" << reExportStmt->isWildcard << std::endl;
#endif

                    // (AR) بناء اسم الوحدة الكامل
                    // (EN) Build full module name
                    std::string fullModuleName;
                    for (size_t i = 0; i < reExportStmt->modulePath.size(); ++i)
                    {
                        if (i > 0)
                            fullModuleName += ".";
                        fullModuleName += reExportStmt->modulePath[i];
                    }

                    // (AR) تحميل الوحدة عبر محلل الوحدات (نفس منطق b_.buildImportStmt)
                    // (EN) Load module via module resolver (same logic as b_.buildImportStmt)
                    if (!b_.moduleResolver_)
                    {
                        b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                    }

                    // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
                    // (EN) Check if module was already processed
                    if (!b_.processedModules_.count(fullModuleName))
                    {
                        Modules::Module *resolvedModule = b_.moduleResolver_->resolveModule(
                            reExportStmt->modulePath, b_.currentFilePath_);

                        if (resolvedModule)
                        {
                            // (AR) تمييز الوحدة كمعالجة
                            // (EN) Mark module as processed
                            b_.processedModules_.insert(fullModuleName);

                            // (AR) معالجة تصريحات الوحدة — نفس منطق b_.buildImportStmt
                            // (EN) Process module declarations — same logic as b_.buildImportStmt
                            for (const auto &modStmt : resolvedModule->ast)
                            {
                                if (!modStmt)
                                    continue;

                                AST::FunctionDecl *funcDecl = nullptr;
                                AST::VarDeclStmt *varDecl = nullptr;
                                AST::ClassDecl *classDecl = nullptr;

                                // (AR) تصدير صريح
                                if (auto exportDecl = dynamic_cast<AST::ExportDecl *>(modStmt.get()))
                                {
                                    if (exportDecl->declaration)
                                    {
                                        funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                                        varDecl = dynamic_cast<AST::VarDeclStmt *>(exportDecl->declaration.get());
                                        classDecl = dynamic_cast<AST::ClassDecl *>(exportDecl->declaration.get());
                                    }
                                }
                                else if (auto exportStmt2 = dynamic_cast<AST::ExportStmt *>(modStmt.get()))
                                {
                                    if (exportStmt2->declaration)
                                    {
                                        funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt2->declaration.get());
                                        varDecl = dynamic_cast<AST::VarDeclStmt *>(exportStmt2->declaration.get());
                                        classDecl = dynamic_cast<AST::ClassDecl *>(exportStmt2->declaration.get());
                                    }
                                }
                                else
                                {
                                    funcDecl = dynamic_cast<AST::FunctionDecl *>(modStmt.get());
                                    varDecl = dynamic_cast<AST::VarDeclStmt *>(modStmt.get());
                                    classDecl = dynamic_cast<AST::ClassDecl *>(modStmt.get());
                                }

                                // (AR) تصفية إذا لم يكن wildcard
                                // (EN) Filter if not wildcard
                                if (!reExportStmt->isWildcard)
                                {
                                    std::string declName;
                                    if (funcDecl)
                                        declName = funcDecl->name;
                                    else if (varDecl)
                                        declName = varDecl->name;
                                    else if (classDecl)
                                        declName = classDecl->name;

                                    bool found = false;
                                    for (auto &item : reExportStmt->items)
                                    {
                                        if (item.name == declName)
                                        {
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found)
                                        continue;
                                }

                                // (AR) بناء SIR للتصريحات
                                // (EN) Build SIR for declarations
                                if (funcDecl)
                                    b_.buildFunction(funcDecl);
                                if (varDecl)
                                    b_.buildGlobalVariable(varDecl);
                                if (classDecl)
                                    b_.buildClass(classDecl);
                            }
                        }
                        else
                        {
                            b_.errors_.push_back(
                                "خطأ: فشل استيراد الوحدة للتصدير '" + fullModuleName + "' / "
                                                                                       "Error: Failed to import module for re-export '" +
                                fullModuleName + "'");
                        }
                    }
                    return true;
                }


                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
