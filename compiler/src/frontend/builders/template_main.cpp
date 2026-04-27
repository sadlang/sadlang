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
#include "builders/template_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <unordered_set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            namespace
            {
                // ====================================================================
                // (AR) قائمة موحّدة لوحدات المكتبة القياسية التي يعترضها المترجم
                //      مباشرةً ويعامل دوالها كـ builtins بدون تحليل ملف .ص الخارجي.
                //      السبب الجذري لهذا الاستخراج هو منع انجراف قائمتين منفصلتين بين
                //      buildImportStmt و buildFromImportStmt، لأن أي اختلاف بينهما يولّد
                //      سلوكاً متناقضاً: استيراد كامل ينجح بينما الاستيراد الانتقائي من
                //      نفس الوحدة قد يحاول تحميل ملف wrapper خارجي ويؤدي إلى أخطاء ربط.
                //
                // (EN) Unified set of stdlib modules intercepted directly by the
                //      compiler and treated as builtins without parsing external .ص
                //      wrapper files. This shared helper prevents drift between the
                //      regular import and selective from-import paths.
                // ====================================================================
                const std::unordered_set<std::string> &getCompilerBuiltinStdlibModules()
                {
                    static const std::unordered_set<std::string> modules = {
                        "رياضيات", "math", "حساب",
                        "نصوص", "strings", "str", "سلاسل",
                        "أساسيات", "basics", "base", "ملفات",
                        "خرائط", "maps", "json", "xml",
                        "وقت", "time", "تاريخ",
                        "تأكيدات", "assertions", "اختبار",
                        "تزامن_متقدم", "async", "توازي",
                        "منصة", "platform", "نظام_تشغيل",
                        "شبكة", "شبكة_عالية", "http", "اتصالات",
                        "تشفير", "crypto", "أمان_بيانات",
                        "مقابس", "sockets", "tcp", "udp",
                        "ويبسوكت", "websocket", "ws"};
                    return modules;
                }

                bool isCompilerBuiltinStdlibModule(const std::string &moduleName)
                {
                    return getCompilerBuiltinStdlibModules().count(moduleName) > 0;
                }
            }

            // ============================================================================
            // بناء دالة قالب / Build Template Function
            // ============================================================================
            // Purpose (AR): حفظ دالة قالب للإنشاء لاحقاً عند الاستدعاء مع أنواع محددة
            // Purpose (EN): Store template function for later instantiation with specific types
            //
            // التعريف / Source: declarations.h:470 (TemplateFunctionDecl)
            // التوقيع / Signature: void buildTemplateFunction(AST::TemplateFunctionDecl*)
            //
            // TemplateFunctionDecl Members:
            // - typeParameters: std::vector<TypeParameter> (line 472)
            // - name: std::string (line 473)
            // - parameters: std::vector<Parameter> (line 474)
            // - returnType: Data::DataType (line 475)
            // - body: StmtPtr (line 476)
            // ============================================================================
            void TemplateBuilder::buildTemplateFunction(AST::TemplateFunctionDecl *templateDecl)
            {
                if (!templateDecl)
                {
                    return;
                }

                std::cout << "[Template] Registering template function: " << templateDecl->name << std::endl;
                std::cout << "[Template] Type parameters: ";
                for (const auto &typeParam : templateDecl->typeParameters)
                {
                    std::cout << typeParam.name << " ";
                }
                std::cout << std::endl;

                // (AR) حفظ دالة القالب في الجدول
                // (EN) Store template function in table
                b_.templateFunctions_[templateDecl->name] = templateDecl;

                std::cout << "[Template] Template function '" << templateDecl->name
                          << "' stored for later instantiation" << std::endl;
            }

            // ============================================================================
            // إنشاء نسخة محددة من دالة قالب / Instantiate Template Function
            // ============================================================================
            // Purpose (AR): إنشاء نسخة محددة من دالة القالب باستبدال معاملات الأنواع
            // Purpose (EN): Create concrete instance of template function with type substitution
            //
            // Example:
            // - Template: قالب<نوع ت> دالة أكبر(ت أ، ت ب) ت
            // - Call: أكبر<رقم>(5, 10)
            // - Instantiation: أكبر_i64(رقم أ، رقم ب) رقم
            // ============================================================================
            std::string TemplateBuilder::instantiateTemplate(const std::string &templateName,
                                                        const std::vector<SadTypeKind> &typeArguments)
            {
                // (AR) [Phase 4] واجهة قديمة — تفويض للنسخة الجديدة بدون const args
                // (EN) [Phase 4] Legacy overload — delegate to new version with no const args
                return instantiateTemplate(templateName, typeArguments, std::vector<SIROperand>{});
            }

            // ============================================================================
            // (AR) [Phase 4 — Monomorphization] إنشاء نسخة قالب مع const-generics
            // (EN) [Phase 4 — Monomorphization] Template instantiation with const-generics
            // ============================================================================
            std::string TemplateBuilder::instantiateTemplate(const std::string &templateName,
                                                        const std::vector<SadTypeKind> &typeArguments,
                                                        const std::vector<SIROperand> &constArguments)
            {
                std::cout << "[Template] Instantiating template: " << templateName << std::endl;

                // (AR) البحث عن دالة القالب
                // (EN) Find template function
                auto it = b_.templateFunctions_.find(templateName);
                if (it == b_.templateFunctions_.end())
                {
                    // (AR) تسجيل الخطأ في قائمة الأخطاء بدلاً من stderr فقط
                    // (EN) Record error in error list instead of only stderr
                    std::string errMsg = "دالة القالب غير موجودة / Template function not found: '" + templateName + "'";
                    std::cerr << "[Template Error] " << errMsg << std::endl;
                    b_.errors_.push_back(errMsg);
                    return "";
                }

                Sad::AST::TemplateFunctionDecl *templateDecl = it->second;

                // ================================================================
                // (AR) [Phase 4] فصل معاملات typename عن معاملات const
                //      الترتيب الأصلي محفوظ في templateDecl->typeParameters
                //      مع isConst لكل معامل. نُنشئ خرائط منفصلة:
                //        - typeParamNames: أسماء معاملات النوع فقط
                //        - constParamNames: أسماء معاملات الثابت فقط
                //      ثم نتحقق أن عدد typeArguments == |typeParams|
                //      وعدد constArguments == |constParams|.
                // ================================================================
                std::vector<std::string> typeParamNames;
                std::vector<std::string> constParamNames;
                for (const auto &p : templateDecl->typeParameters)
                {
                    if (p.isConst)
                        constParamNames.push_back(p.name);
                    else
                        typeParamNames.push_back(p.name);
                }

                // (AR) التحقق من عدد معاملات الأنواع
                // (EN) Check type argument count
                if (typeArguments.size() != typeParamNames.size())
                {
                    std::string errMsg = "عدد معاملات الأنواع غير متطابق / Type argument count mismatch for '" + templateName + "': expected " + std::to_string(typeParamNames.size()) + ", got " + std::to_string(typeArguments.size());
                    std::cerr << "[Template Error] " << errMsg << std::endl;
                    b_.errors_.push_back(errMsg);
                    return "";
                }

                // (AR) [Phase 4] التحقق من عدد المعاملات الثابتة
                // (EN) [Phase 4] Check const-arg count
                if (constArguments.size() != constParamNames.size())
                {
                    std::string errMsg = "عدد معاملات الثوابت غير متطابق / Const-arg count mismatch for '" + templateName + "': expected " + std::to_string(constParamNames.size()) + ", got " + std::to_string(constArguments.size());
                    std::cerr << "[Template Error] " << errMsg << std::endl;
                    b_.errors_.push_back(errMsg);
                    return "";
                }

                // (AR) إنشاء اسم الدالة المُنشأة: اسم_القالب_نوع1_نوع2
                // (EN) Create instantiated function name: template_name_type1_type2
                std::string instantiatedName = templateName;
                for (const auto &type : typeArguments)
                {
                    instantiatedName += "_";
                    switch (type)
                    {
                    case SadTypeKind::Integer:
                        instantiatedName += "i64";
                        break;
                    case SadTypeKind::Float:
                        instantiatedName += "f64";
                        break;
                    case SadTypeKind::String:
                        instantiatedName += "str";
                        break;
                    case SadTypeKind::Boolean:
                        instantiatedName += "bool";
                        break;
                    default:
                        instantiatedName += "obj";
                        break;
                    }
                }

                // ================================================================
                // (AR) [Phase 4] دمج قيم الثوابت في اسم النسخة لضمان نسخة فريدة
                //      لكل تركيبة (T، N). مثال: f<عشري،4> → f_f64_c4
                //      نستخدم بادئة "_c" قبل القيمة لتفادي الالتباس مع أسماء الأنواع.
                // (EN) [Phase 4] Embed const-arg values in instantiation name to
                //      keep specializations unique per (T,N). Prefix "_c" prevents
                //      collision with type-name suffixes.
                // ================================================================
                for (const auto &cArg : constArguments)
                {
                    instantiatedName += "_c";
                    switch (cArg.dataType)
                    {
                    case SadTypeKind::Integer:
                        instantiatedName += std::to_string(cArg.intValue);
                        break;
                    case SadTypeKind::Float:
                    {
                        // (AR) نُحوّل العشري لتمثيل آمن للأسماء (نقطة → s)
                        std::string s = std::to_string(cArg.floatValue);
                        for (auto &ch : s)
                            if (ch == '.' || ch == '-')
                                ch = '_';
                        instantiatedName += s;
                        break;
                    }
                    case SadTypeKind::Boolean:
                        instantiatedName += cArg.boolValue ? "T" : "F";
                        break;
                    case SadTypeKind::String:
                        // (AR) للنصوص نستخدم بصمة بسيطة من الطول والـhash لتفادي رموز خاصة
                        instantiatedName += "s" + std::to_string(cArg.name.size()) +
                                            "_" + std::to_string(std::hash<std::string>{}(cArg.name) & 0xFFFF);
                        break;
                    default:
                        instantiatedName += "x";
                        break;
                    }
                }

                std::cout << "[Template] Instantiated name: " << instantiatedName << std::endl;

                // (AR) التحقق من وجود النسخة بالفعل
                // (EN) Check if already instantiated
                if (b_.instantiatedTemplates_.find(instantiatedName) != b_.instantiatedTemplates_.end())
                {
                    std::cout << "[Template] Already instantiated, reusing" << std::endl;
                    return instantiatedName;
                }

                // (AR) إنشاء جدول استبدال الأنواع
                // (EN) Create type substitution map
                std::unordered_map<std::string, SadTypeKind> typeSubstitutions;
                for (size_t i = 0; i < typeParamNames.size(); ++i)
                {
                    typeSubstitutions[typeParamNames[i]] = typeArguments[i];
                    std::cout << "[Template] Substitution: " << typeParamNames[i]
                              << " -> " << static_cast<int>(typeArguments[i]) << std::endl;
                }

                // ================================================================
                // (AR) [Phase 4] جدول استبدال الثوابت — يُستخدم في b_.buildVariableAccess
                //      لاعتراض أي مرجع لمعامل ثابت وإصدار قيمة ثابتة فوراً.
                // (EN) [Phase 4] Const substitution table — consumed by
                //      b_.buildVariableAccess to short-circuit any reference to a
                //      const-generic param into an immediate Constant operand.
                // ================================================================
                std::unordered_map<std::string, SIROperand> constSubstitutions;
                for (size_t i = 0; i < constParamNames.size(); ++i)
                {
                    constSubstitutions[constParamNames[i]] = constArguments[i];
                    std::cout << "[Template] Const substitution: " << constParamNames[i] << std::endl;
                }

                // (AR) استبدال نوع الإرجاع
                // (EN) Substitute return type
                SadTypeKind returnType = b_.astTypeToSIRType(templateDecl->returnType);

                // (AR) إذا كان نوع الإرجاع OBJECT نستخدم أول استبدال نوع متاح كحل عملي
                // (EN) If return type is OBJECT, use the first available type substitution pragmatically
                if (templateDecl->returnType == Data::DataType::OBJECT &&
                    !typeParamNames.empty())
                {
                    auto it = typeSubstitutions.find(typeParamNames.front());
                    if (it != typeSubstitutions.end())
                    {
                        returnType = it->second;
                    }
                }

                // ================================================================
                // (AR) [Phase 4] إذا لم يُعلَن نوع إرجاع (NONE → Void) لكن للقالب
                //      معاملات أنواع أو ثوابت، نستنتج نوع الإرجاع من أول بديل متاح:
                //      1) أول const param (إذا وُجد) — ارجع N → نوع N
                //      2) أول type param — ارجع قيمة من نوع T → نوع T
                //      هذا حل عملي شامل لدوال القوالب غير المُعلَنة العودة.
                // (EN) [Phase 4] If no return type declared (NONE → Void) but the
                //      template has type/const params, infer return type from the
                //      first available substitution:
                //      1) first const param (if any) — `return N` → type of N
                //      2) first type param — `return val:T` → T
                //      Pragmatic fallback for templates without explicit return type.
                // ================================================================
                if (returnType == SadTypeKind::Void)
                {
                    if (!constParamNames.empty())
                    {
                        auto cIt = constSubstitutions.find(constParamNames.front());
                        if (cIt != constSubstitutions.end())
                            returnType = cIt->second.dataType;
                    }
                    else if (!typeParamNames.empty())
                    {
                        auto tIt = typeSubstitutions.find(typeParamNames.front());
                        if (tIt != typeSubstitutions.end())
                            returnType = tIt->second;
                    }
                }

                // (AR) إنشاء دالة SIR جديدة
                // (EN) Create new SIR function
                auto sirFunction = std::make_shared<SIRFunction>(instantiatedName, returnType);

                // (AR) استبدال أنواع المعاملات
                // (EN) Substitute parameter types
                for (const auto &param : templateDecl->parameters)
                {
                    SadTypeKind paramType = b_.astTypeToSIRType(param.type);

                    // (AR) إذا كان نوع المعامل هو OBJECT، قد يكون معامل قالب
                    // (EN) If parameter type is OBJECT, might be template parameter
                    if (param.type == Data::DataType::OBJECT)
                    {
                        // (AR) البحث في جدول الاستبدال باسم النوع المعلن (param.typeName)
                        // (EN) Look up type name in substitution table (param.typeName)
                        if (!param.typeName.empty())
                        {
                            auto substIt = typeSubstitutions.find(param.typeName);
                            if (substIt != typeSubstitutions.end())
                            {
                                paramType = substIt->second;
                            }
                        }
                        else
                        {
                            // (AR) افتراضي: البحث بترتيب معاملات القالب حسب ترتيب المعاملات
                            // (EN) Fallback: try matching by index using type param names
                            for (size_t ti = 0; ti < templateDecl->typeParameters.size(); ++ti)
                            {
                                auto substIt = typeSubstitutions.find(templateDecl->typeParameters[ti].name);
                                if (substIt != typeSubstitutions.end())
                                {
                                    paramType = substIt->second;
                                    break;
                                }
                            }
                        }
                    }

                    sirFunction->addParameter(Sad::Compiler::SIR::SIRParameter(param.name, paramType));
                }

                // (AR) بناء جسم الدالة مع استبدال الأنواع
                // (EN) Build function body with type substitution
                //
                // (AR) [إصلاح علة codegen — defer state leak]
                //      نستخدم b_.saveContext()/b_.restoreContext() بدلاً من حفظ
                //      b_.currentFunction_/b_.currentBlock_ يدوياً. السبب الجذري: defer
                //      infrastructure (b_.currentDeferStackReg_, b_.currentDeferExecutedFlagReg_,
                //      b_.deferredStatements_) كانت تتسرّب من الدالة الخارجية إلى دالة
                //      القالب instantiated، فعند ترجمة `ارجع` كان codegen يولّد
                //      تعليمات تشير إلى سجلات defer من نطاق آخر → "Undefined register"
                //      وتعطل وقت تشغيل. b_.saveContext يصفّر الحالة بالكامل.
                // (EN) [Codegen bug fix — defer state leak]
                //      Use b_.saveContext()/b_.restoreContext() instead of manually saving
                //      b_.currentFunction_/b_.currentBlock_. Root cause: defer infrastructure
                //      (defer stack/done regs, deferred stmts) leaked from outer
                //      function into instantiated template, causing return codegen
                //      to emit refs to defer registers from a different scope →
                //      "Undefined register" + runtime crash. b_.saveContext fully resets.
                auto savedCtx = b_.saveContext();

                b_.currentFunction_ = sirFunction;

                // (AR) إنشاء نطاق جديد
                // (EN) Create new scope
                b_.enterScope();

                // (AR) إضافة استبدالات الأنواع للنطاق الحالي (للاستخدام عند بناء الجسم)
                // (EN) Add type substitutions to current scope
                GenericScope genericScope;

                // (AR) تحويل typeParameters من AST إلى SIR
                // (EN) Convert typeParameters from AST to SIR
                for (const auto &astParam : templateDecl->typeParameters)
                {
                    TypeParameter sirParam(astParam.name);
                    genericScope.typeParameters.push_back(sirParam);
                }

                genericScope.typeSubstitutions = typeSubstitutions;
                // (AR) [Phase 4] دفع جدول الثوابت لاستخدامه أثناء بناء الجسم
                // (EN) [Phase 4] Push const substitutions for use during body building
                genericScope.constSubstitutions = constSubstitutions;
                b_.genericScopeStack_.push_back(genericScope);

                // (AR) إنشاء basic block للدخول
                // (EN) Create entry basic block
                auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                sirFunction->addBasicBlock(entryBlock);
                b_.currentBlock_ = entryBlock;

                // ================================================================
                // (AR) [إصلاح علة Phase 5] تسجيل معاملات الدالة كمتغيرات محلية
                //      كي يتمكن جسم القالب من الوصول إليها بالاسم.
                //      السبب الجذري للعلة: دوال القوالب instantiated كانت تنشأ
                //      بدون تسجيل معاملاتها في VariableManager، فعند ترجمة
                //      `ارجع أ` لا يُعثر على المتغير `أ` فيُولَّد كود قمامة
                //      (defer iterating over null array، ret 0 ثابت).
                //      الحل: نسخ النمط من sir_builder_statements_main.cpp:184
                // (EN) [Phase 5 Bug Fix] Register template function parameters
                //      as local variables so the template body can resolve them
                //      by name. Root cause: instantiated template functions were
                //      created without registering parameters in VariableManager,
                //      so `return أ` couldn't find `أ` and produced garbage IR
                //      (defer iterating null array, constant ret 0).
                //      Fix: mirror pattern from sir_builder_statements_main.cpp
                // ================================================================
                {
                    const auto &sirParams = sirFunction->getParameters();
                    for (size_t i = 0; i < sirParams.size(); ++i)
                    {
                        VariableInfo paramVar;
                        paramVar.name = sirParams[i].name;
                        paramVar.type = sirParams[i].type;
                        paramVar.registerName = "%" + sirParams[i].name;
                        paramVar.isMutable = true;
                        paramVar.isParameter = true;
                        paramVar.scopeLevel = b_.currentScopeLevel_;
                        b_.addVariable(paramVar);
                    }
                }

                // (AR) بناء جسم الدالة
                // (EN) Build function body
                if (templateDecl->body)
                {
                    b_.buildStatement(templateDecl->body.get());
                }

                // (AR) إزالة نطاق الأنواع العامة
                // (EN) Remove generic scope
                b_.genericScopeStack_.pop_back();

                // (AR) الخروج من النطاق
                // (EN) Exit scope
                b_.exitScope();

                // (AR) التأكد من وجود terminator
                // (EN) Ensure terminator exists
                if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                {
                    const auto &lastInst = b_.currentBlock_->instructions.back();
                    if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID)
                    {
                        if (returnType == SadTypeKind::Void)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            b_.currentBlock_->addInstruction(retInst);
                        }
                        else
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET;
                            retInst.operands.push_back(SIROperand::ConstantI64(0));
                            b_.currentBlock_->addInstruction(retInst);
                        }
                    }
                }

                // (AR) استعادة السياق
                // (EN) Restore context
                b_.restoreContext(std::move(savedCtx));

                // (AR) حفظ الدالة المُنشأة
                // (EN) Store instantiated function
                b_.instantiatedTemplates_[instantiatedName] = sirFunction;
                b_.module_->addFunction(sirFunction);

                // (AR) إضافة الدالة المُنشأة إلى جدول الدوال
                // (EN) Add instantiated function to function table
                FunctionInfo funcInfo;
                funcInfo.name = instantiatedName;
                funcInfo.returnType = returnType;
                funcInfo.parameters = sirFunction->getParameters();
                funcInfo.sirFunction = sirFunction;
                b_.functionTable_[instantiatedName] = funcInfo;

                std::cout << "[Template] Instantiation complete: " << instantiatedName << std::endl;

                return instantiatedName;
            }

            // ============================================================================
            // (AR) نظام الاستيراد والتصدير في المترجم
            // (EN) Import/Export System in Compiler
            // ============================================================================

            /**
             * @brief (AR) تعيين مسار الملف الحالي - يُستخدم لحل مسارات الاستيراد النسبية
             * @brief (EN) Set current file path - used for resolving relative import paths
             */
            void TemplateBuilder::setCurrentFilePath(const std::string &filePath)
            {
                b_.currentFilePath_ = filePath;

                // (AR) إنشاء محلل الوحدات إذا لم يكن موجوداً
                // (EN) Create module resolver if it doesn't exist
                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();

                    // (AR) إضافة مجلد الملف الحالي كمسار بحث
                    // (EN) Add current file's directory as search path
                    if (!filePath.empty())
                    {
                        try
                        {
                            auto fsPath = sad::utf8::make_path(filePath);
                            auto parentDir = fsPath.parent_path();
                            if (!parentDir.empty() && std::filesystem::exists(parentDir))
                            {
                                // (AR) نستخدم filesystem::path مباشرة لتجنب مشاكل ترميز UTF-8 مع ANSI
                                // (EN) Push filesystem::path directly to avoid UTF-8/ANSI encoding issues
                                b_.moduleResolver_->addSearchPathDirect(parentDir);
                            }
                        }
                        catch (const std::exception &e)
                        {
                            // (AR) تجاهل الأخطاء - مسارات البحث الافتراضية كافية
                            // (EN) Ignore errors - default search paths are sufficient
                            (void)e;
                        }
                    }
                }
            }

            /**
             * @brief (AR) معالجة وحدة مستوردة - تحليل ملف الوحدة وبناء دوالها
             * @brief (EN) Process imported module - parse module file and build its functions
             *
             * @details
             * (AR) آلية العمل:
             *      1. استخدام ModuleResolver للعثور على ملف الوحدة
             *      2. تحليل AST الوحدة
             *      3. استخراج الدوال المُصدَّرة
             *      4. بناء SIR لكل دالة مُصدَّرة وإضافتها للوحدة الحالية
             *
             * (EN) How it works:
             *      1. Use ModuleResolver to find module file
             *      2. Parse module AST
             *      3. Extract exported functions
             *      4. Build SIR for each exported function and add to current module
             */
            void TemplateBuilder::buildImportStmt(AST::ImportStmt *importStmt)
            {
                if (!importStmt)
                    return;

                // (AR) التحقق من وجود محلل الوحدات
                // (EN) Check module resolver exists
                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                }

                std::string fullModuleName = importStmt->getFullModuleName();

                // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
                // (EN) Check if module was already processed
                if (b_.processedModules_.count(fullModuleName))
                {
                    return;
                }

                // ====================================================================
                // (AR) اعتراض وحدات المكتبة القياسية — الدوال المضمنة مدعومة مباشرة
                //      في المترجم بدون الحاجة لتحليل ملف .ص الخارجي.
                //      عند استيراد وحدة قياسية، نُعلّمها كمعالجة ونعود فوراً —
                //      لأن جميع دوالها معروفة للمترجم كدوال مضمنة (builtins).
                // (EN) Stdlib module intercept — builtin functions are supported directly
                //      in the compiler without needing to parse the external .ص file.
                //      When importing a stdlib module, mark it as processed and return —
                //      all its functions are known to the compiler as builtins.
                // ====================================================================
                if (isCompilerBuiltinStdlibModule(fullModuleName))
                {
                    // (AR) تسجيل الوحدة كمعالجة — دوالها مضمنة في المترجم
                    // (EN) Mark module as processed — its functions are compiler builtins
                    b_.processedModules_.insert(fullModuleName);
                    return;
                }

                // (AR) إصدار تعليمة تحميل الوحدة في SIR
                // (EN) Emit MODULE_LOAD SIR instruction for linker tracking
                if (b_.currentBlock_)
                {
                    std::string moduleReg = b_.newTempRegister();
                    SIRInstruction loadInst(SIROpcode::MODULE_LOAD);
                    loadInst.result = SIROperand::Register(moduleReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::ConstantString(fullModuleName));
                    loadInst.comment = "استيراد وحدة / import module: " + fullModuleName;
                    b_.currentBlock_->addInstruction(loadInst);

                    // (AR) إصدار تعليمة تهيئة الوحدة
                    // (EN) Emit MODULE_INIT instruction
                    SIRInstruction initInst(SIROpcode::MODULE_INIT);
                    initInst.operands.push_back(SIROperand::Register(moduleReg, SadTypeKind::Integer));
                    initInst.comment = "تهيئة وحدة / init module: " + fullModuleName;
                    b_.currentBlock_->addInstruction(initInst);
                }

                // (AR) تحميل الوحدة
                // (EN) Load module
                Modules::Module *module = b_.moduleResolver_->resolveModule(
                    importStmt->modulePath,
                    b_.currentFilePath_);

                if (!module)
                {
                    b_.errors_.push_back(
                        "خطأ: لم يُعثر على الوحدة '" + fullModuleName + "' / "
                                                                       "Error: Module '" +
                        fullModuleName + "' not found");
                    return;
                }

                // (AR) تمييز الوحدة كمعالجة
                // (EN) Mark module as processed
                b_.processedModules_.insert(fullModuleName);

                // (AR) معالجة كل تصريح في الوحدة
                // (EN) Process each declaration in module
                for (const auto &stmt : module->ast)
                {
                    if (!stmt)
                        continue;

                    // (AR) معالجة عبارات الاستيراد الداخلية (استيراد متداخل/عابر)
                    // (EN) Process internal import statements (transitive/nested imports)
                    if (auto innerFromImport = dynamic_cast<AST::FromImportStmt *>(stmt.get()))
                    {
                        buildFromImportStmt(innerFromImport);
                        continue;
                    }
                    if (auto innerImport = dynamic_cast<AST::ImportStmt *>(stmt.get()))
                    {
                        buildImportStmt(innerImport);
                        continue;
                    }

                    // (AR) استخراج الدوال (المُصدَّرة أو غير المُصدَّرة)
                    // (EN) Extract functions (exported or not)
                    AST::FunctionDecl *funcDecl = nullptr;
                    AST::VarDeclStmt *varDecl = nullptr;
                    AST::ClassDecl *classDecl = nullptr;

                    // (AR) تحقق من تصدير صريح (ExportDecl)
                    // (EN) Check for explicit export (ExportDecl)
                    if (auto exportDecl = dynamic_cast<AST::ExportDecl *>(stmt.get()))
                    {
                        if (exportDecl->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportDecl->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportDecl->declaration.get());
                        }
                    }
                    // (AR) تحقق من تصدير قديم (ExportStmt)
                    // (EN) Check for legacy export (ExportStmt)
                    else if (auto exportStmt = dynamic_cast<AST::ExportStmt *>(stmt.get()))
                    {
                        if (exportStmt->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportStmt->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportStmt->declaration.get());
                        }
                    }
                    // (AR) دالة عادية (بدون تصدير صريح)
                    // (EN) Regular function (no explicit export)
                    else
                    {
                        funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get());
                        varDecl = dynamic_cast<AST::VarDeclStmt *>(stmt.get());
                        classDecl = dynamic_cast<AST::ClassDecl *>(stmt.get());
                    }

                    // (AR) بناء SIR للتصريحات المُكتشفة
                    // (EN) Build SIR for discovered declarations
                    if (funcDecl)
                    {
                        b_.buildFunction(funcDecl);
                    }
                    if (varDecl)
                    {
                        b_.buildGlobalVariable(varDecl);
                    }
                    if (classDecl)
                    {
                        b_.buildClass(classDecl);
                    }
                }
            }

            /**
             * @brief (AR) معالجة استيراد انتقائي: من وحدة استورد ...
             * @brief (EN) Process selective import: from module import ...
             */
            void TemplateBuilder::buildFromImportStmt(AST::FromImportStmt *fromImportStmt)
            {
                if (!fromImportStmt)
                    return;

                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                }

                std::string fullModuleName = fromImportStmt->getFullModuleName();

                // (AR) تحسين جذري: إذا كانت الوحدة من المكتبة القياسية المعروفة للمترجم،
                //      فلا حاجة لتحليل ملف الوحدة أو تحميله. جميع الدوال مضمنة في المترجم.
                //      هذا يمنع أخطاء الرابط عند تحميل ملفات .ص القياسية التي تستدعي
                //      دوال داخلية غير موجودة في وقت التشغيل المضمن.
                // (EN) Root fix: if the module is a known stdlib module, skip parsing/loading
                //      the module file entirely. All functions are compiler builtins.
                //      This prevents linker errors from .ص wrappers calling undefined internals.
                if (isCompilerBuiltinStdlibModule(fullModuleName))
                {
                    // (AR) وحدة قياسية — جميع دوالها مضمنة، لا حاجة لتحميل الملف
                    // (EN) Stdlib module — all functions are builtins, skip file loading
                    b_.processedModules_.insert(fullModuleName);
                    return;
                }

                // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
                //      للاستيراد الانتقائي: لا نتوقف — نكمل لاستيراد رموز جديدة
                // (EN) Check if module was already processed
                //      For selective import: don't stop — continue to import new symbols
                bool moduleAlreadyLoaded = b_.processedModules_.count(fullModuleName) > 0;

                // (AR) إصدار تعليمات SIR (فقط أول مرة)
                // (EN) Emit SIR instructions (first time only)
                if (!moduleAlreadyLoaded && b_.currentBlock_)
                {
                    std::string moduleReg = b_.newTempRegister();
                    SIRInstruction loadInst(SIROpcode::MODULE_LOAD);
                    loadInst.result = SIROperand::Register(moduleReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::ConstantString(fullModuleName));
                    loadInst.comment = "تحميل وحدة انتقائي / selective import from: " + fullModuleName;
                    b_.currentBlock_->addInstruction(loadInst);

                    SIRInstruction initInst(SIROpcode::MODULE_INIT);
                    initInst.operands.push_back(SIROperand::Register(moduleReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(initInst);

                    // (AR) إصدار MODULE_SYMBOL لكل رمز مطلوب
                    // (EN) Emit MODULE_SYMBOL for each requested symbol
                    if (!fromImportStmt->isWildcard)
                    {
                        for (const auto &item : fromImportStmt->items)
                        {
                            std::string symReg = b_.newTempRegister();
                            SIRInstruction symInst(SIROpcode::MODULE_SYMBOL);
                            symInst.result = SIROperand::Register(symReg, SadTypeKind::Integer);
                            symInst.operands.push_back(SIROperand::Register(moduleReg, SadTypeKind::Integer));
                            symInst.operands.push_back(SIROperand::ConstantString(item.name));
                            symInst.comment = "استيراد رمز / import symbol: " + item.name;
                            b_.currentBlock_->addInstruction(symInst);
                        }
                    }
                }

                // (AR) تحميل الوحدة (ModuleResolver يخبئ النتائج)
                // (EN) Load module (ModuleResolver caches results)
                Modules::Module *module = b_.moduleResolver_->resolveModule(
                    fromImportStmt->modulePath,
                    b_.currentFilePath_);

                if (!module)
                {
                    if (!moduleAlreadyLoaded)
                    {
                        b_.errors_.push_back(
                            "خطأ: لم يُعثر على الوحدة '" + fullModuleName + "' / "
                                                                           "Error: Module '" +
                            fullModuleName + "' not found");
                    }
                    return;
                }

                b_.processedModules_.insert(fullModuleName);

                // (AR) جمع أسماء الرموز المطلوبة (للاستيراد الانتقائي)
                // (EN) Collect requested symbol names (for selective import)
                std::unordered_set<std::string> requestedSymbols;
                bool isWildcard = fromImportStmt->isWildcard;

                if (!isWildcard)
                {
                    for (const auto &item : fromImportStmt->items)
                    {
                        requestedSymbols.insert(item.name);
                    }
                }

                // (AR) معالجة تصريحات الوحدة
                // (EN) Process module declarations
                for (const auto &stmt : module->ast)
                {
                    if (!stmt)
                        continue;

                    // (AR) معالجة عبارات الاستيراد الداخلية (استيراد متداخل/عابر)
                    // (EN) Process internal import statements (transitive/nested imports)
                    if (auto innerFromImport = dynamic_cast<AST::FromImportStmt *>(stmt.get()))
                    {
                        buildFromImportStmt(innerFromImport);
                        continue;
                    }
                    if (auto innerImport = dynamic_cast<AST::ImportStmt *>(stmt.get()))
                    {
                        buildImportStmt(innerImport);
                        continue;
                    }

                    AST::FunctionDecl *funcDecl = nullptr;
                    AST::VarDeclStmt *varDecl = nullptr;
                    AST::ClassDecl *classDecl = nullptr;
                    bool isExported = false;

                    if (auto exportDecl = dynamic_cast<AST::ExportDecl *>(stmt.get()))
                    {
                        isExported = true;
                        if (exportDecl->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportDecl->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportDecl->declaration.get());
                        }
                    }
                    else if (auto exportStmt = dynamic_cast<AST::ExportStmt *>(stmt.get()))
                    {
                        isExported = true;
                        if (exportStmt->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportStmt->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportStmt->declaration.get());
                        }
                    }
                    else
                    {
                        funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get());
                        varDecl = dynamic_cast<AST::VarDeclStmt *>(stmt.get());
                        classDecl = dynamic_cast<AST::ClassDecl *>(stmt.get());
                    }

                    // (AR) تحديد ما إذا كان الرمز مطلوباً
                    // (EN) Determine if symbol is requested
                    std::string symbolName;
                    if (funcDecl)
                        symbolName = funcDecl->name;
                    else if (varDecl)
                        symbolName = varDecl->name;
                    else if (classDecl)
                        symbolName = classDecl->name;
                    else
                        continue;

                    // (AR) في حالة الاستيراد الانتقائي، نتحقق من أن الرمز مطلوب
                    // (EN) For selective import, check if symbol is requested
                    if (!isWildcard && requestedSymbols.find(symbolName) == requestedSymbols.end())
                    {
                        continue;
                    }

                    // (AR) تخطي الرموز المبنية مسبقاً (لمنع التكرار عند إعادة معالجة الوحدة)
                    // (EN) Skip already-built symbols (prevent duplication when re-processing module)
                    if (funcDecl && b_.functionTable_.find(symbolName) != b_.functionTable_.end())
                    {
                        continue;
                    }
                    if (classDecl && b_.module_->getClass(symbolName))
                    {
                        continue;
                    }

                    // (AR) بناء SIR
                    // (EN) Build SIR
                    if (funcDecl)
                    {
                        b_.buildFunction(funcDecl);
                    }
                    if (varDecl)
                    {
                        b_.buildGlobalVariable(varDecl);
                    }
                    if (classDecl)
                    {
                        b_.buildClass(classDecl);
                    }
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
