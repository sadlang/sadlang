// ============================================================================
// sir_builder_traits_impl.cpp - بناء السمات وكتل التنفيذ / Build Traits & Impl
// ============================================================================
// (AR) هذا الملف يحتوي على بناء السمات (Traits) وكتل التنفيذ (Impl Blocks)
//      تم فصله من sir_builder_classes.cpp وفق CW-05 (حد 800 سطر)
// (EN) This file contains Trait and Impl block building.
//      Split from sir_builder_classes.cpp per CW-05 (800 line limit)
// ============================================================================

#include "sir_builder.h"
#include "builders/class_builder.h"
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
#include <functional>
#include <unordered_set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // (AR) بناء سمة/واجهة / (EN) Build Trait/Interface
            // ============================================================================
            // السمة هي عقد موصفة — لا تولّد كوداً مباشراً، بل تُسجَّل للتحقق والتوثيق
            // Traits are descriptive nodes — no direct code generated, registered for validation
            // ============================================================================
            void ClassBuilder::buildTrait(AST::TraitDecl *traitDecl)
            {
                if (!traitDecl)
                    return;

#ifndef NDEBUG
                std::cout << "[DEBUG] buildTrait: registering trait '" << traitDecl->name
                          << "' with " << traitDecl->methods.size() << " methods" << std::endl;
#endif

                // (AR) بناء بيانات السمة وتسجيلها في الوحدة
                // (EN) Build trait data and register in module
                SIR::SIRTrait sirTrait;
                sirTrait.name = traitDecl->name;

                // (AR) نسخ السمات الأب
                // (EN) Copy super traits
                sirTrait.superTraits = traitDecl->superTraits;

                // (AR) تحويل الدوال المطلوبة
                // (EN) Convert required methods
                for (const auto &method : traitDecl->methods)
                {
                    SIR::SIRTraitMethod tm;
                    tm.name = method.name;
                    tm.returnType = b_.astTypeToSIRType(method.returnType);
                    tm.hasDefaultImpl = (method.defaultImpl != nullptr);
                    for (const auto &param : method.params)
                    {
                        tm.paramTypes.push_back(b_.astTypeToSIRType(param.type));
                    }
                    sirTrait.methods.push_back(tm);
                }

                b_.module_->addTrait(sirTrait);

                // (AR) حفظ AST السمة لتوليد الدوال الافتراضية لاحقاً
                // (EN) Store trait AST for generating default methods later
                b_.traitDefaultImpls_[traitDecl->name] = traitDecl;

#ifndef NDEBUG
                std::cout << "[DEBUG] buildTrait: registered trait '" << traitDecl->name
                          << "' with " << sirTrait.methods.size() << " methods, "
                          << sirTrait.superTraits.size() << " super traits" << std::endl;
#endif
            }

            // ============================================================================
            // (AR) بناء كتلة تنفيذ سمة / (EN) Build Impl Block
            // ============================================================================
            // كتلة التنفيذ تضيف دوال إلى صنف موجود
            // Impl block adds methods to an existing class
            // ============================================================================
            void ClassBuilder::buildImpl(AST::ImplDecl *implDecl)
            {
                if (!implDecl)
                    return;

                std::string className = implDecl->targetType;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildImpl: implementing";
                if (!implDecl->traitName.empty())
                {
                    std::cout << " trait '" << implDecl->traitName << "'";
                }
                std::cout << " for class '" << className << "'" << std::endl;
#endif

                // (AR) البحث عن الصنف المستهدف في وحدة SIR
                // (EN) Find target class in SIR module
                auto sirClass = b_.module_->getClass(className);
                if (!sirClass)
                {
                    std::cerr << "[WARNING] buildImpl: class '" << className
                              << "' not found in module" << std::endl;
                    return;
                }

                // (AR) معالجة الدوال في كتلة التنفيذ
                // (EN) Process methods in impl block
                for (const auto &method : implDecl->methods)
                {
                    if (!method)
                        continue;

                    auto funcDecl = dynamic_cast<AST::FunctionDecl *>(method.get());
                    if (!funcDecl)
                        continue;

                    std::string fullMethodName = className + "." + funcDecl->name;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildImpl: adding method '" << fullMethodName << "'" << std::endl;
#endif

                    // (AR) تحويل نوع الإرجاع — مع استنتاج تلقائي إذا لم يُحدد
                    //      [إصلاح BF-04] buildImpl كان يستخدم b_.astTypeToSIRType مباشرة
                    //      بدون b_.inferReturnTypeFromBody، مما يجعل نوع الإرجاع Integer
                    //      للدوال التي ترجع نصاً (مثل وصف() في trait impl)
                    //      الحل: استخدام b_.inferReturnTypeFromBody كما في buildClass
                    // (EN) Convert return type — with automatic inference if not specified
                    //      [Fix BF-04] buildImpl was using b_.astTypeToSIRType directly
                    //      without b_.inferReturnTypeFromBody, causing return type to be Integer
                    //      for string-returning functions (e.g. وصف() in trait impl)
                    //      Fix: use b_.inferReturnTypeFromBody as in buildClass
                    SIR::SadTypeKind retType;
                    if (funcDecl->returnType == Data::DataType::UNKNOWN ||
                        funcDecl->returnType == Data::DataType::NONE)
                    {
                        // (AR) تعيين الصنف الحالي مؤقتاً لتمكين b_.inferReturnTypeFromBody
                        //      من الوصول لحقول الصنف عبر b_.module_->getClass(b_.currentClassName_)
                        // (EN) Temporarily set b_.currentClassName_ so b_.inferReturnTypeFromBody
                        //      can access class fields via b_.module_->getClass(b_.currentClassName_)
                        auto savedClassName = b_.currentClassName_;
                        b_.currentClassName_ = className;
                        retType = b_.inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                        b_.currentClassName_ = savedClassName;
                    }
                    else
                    {
                        retType = b_.astTypeToSIRType(funcDecl->returnType);
                    }
                    auto sirMethod = std::make_shared<SIR::SIRFunction>(fullMethodName, retType);

                    // (AR) إضافة معامل self ضمنياً
                    // (EN) Add implicit self parameter
                    sirMethod->addParameter(SIR::SIRParameter(kSelfParamName, SIR::SadTypeKind::Integer));

                    // (AR) إضافة المعاملات
                    // (EN) Add parameters
                    for (const auto &param : funcDecl->parameters)
                    {
                        SIR::SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                        sirMethod->addParameter(SIR::SIRParameter(param.name, paramType));
                    }

                    // (AR) إضافة الدالة للصنف
                    // (EN) Add method to class
                    sirClass->addMethod(sirMethod);

                    // (AR) بناء جسم الدالة
                    // (EN) Build method body
                    if (funcDecl->body)
                    {
                        auto prevFunction = b_.currentFunction_;
                        auto prevBlock = b_.currentBlock_;
                        auto prevClassName = b_.currentClassName_;

                        b_.currentFunction_ = sirMethod;
                        b_.currentClassName_ = className;

                        // (AR) إنشاء كتلة الدخول — ضروري لتوليد التعليمات
                        // (EN) Create entry basic block — required for instruction generation
                        auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                        sirMethod->addBasicBlock(entryBlock);
                        b_.currentBlock_ = entryBlock;

                        b_.enterScope();

                        // (AR) Alloca لحقول الصنف — لتمكين الوصول لها عبر self
                        // (EN) Alloca for class fields — enables access through self
                        for (const auto &field : sirClass->fields_)
                        {
                            SIR::SIRInstruction allocInst;
                            allocInst.opcode = SIR::SIROpcode::ALLOC;
                            allocInst.result = SIR::SIROperand::Register("%" + field.first, field.second);
                            b_.currentBlock_->addInstruction(allocInst);
                        }

                        // (AR) تسجيل معامل self
                        // (EN) Register self parameter
                        {
                            VariableInfo selfInfo;
                            selfInfo.name = kSelfParamName;
                            selfInfo.type = SIR::SadTypeKind::Integer;
                            selfInfo.registerName = kSelfRegisterName;
                            selfInfo.isGlobal = false;
                            selfInfo.isMutable = false;
                            selfInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                            b_.addVariable(selfInfo);
                        }

                        // (AR) تسجيل "هذا" كمرادف لـ self — ضروري لدوال trait impl
                        //      [إصلاح BF-04] buildImpl كان ينقص تسجيل "هذا"
                        //      بدون هذا: هذا.وصف() تفشل لأن "هذا" غير معرّف
                        // (EN) Register "هذا" (this) as alias for self — required for trait impl methods
                        //      [Fix BF-04] buildImpl was missing "هذا" registration
                        {
                            VariableInfo thisInfo;
                            thisInfo.name = kThisAliasName; // هذا
                            thisInfo.type = SIR::SadTypeKind::Integer;
                            thisInfo.registerName = kSelfRegisterName;
                            thisInfo.isGlobal = false;
                            thisInfo.isMutable = false;
                            thisInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                            b_.addVariable(thisInfo);
                        }

                        // (AR) تسجيل المعاملات
                        // (EN) Register parameters
                        for (const auto &param : funcDecl->parameters)
                        {
                            VariableInfo paramInfo;
                            paramInfo.name = param.name;
                            paramInfo.type = b_.astTypeToSIRType(param.type);
                            paramInfo.registerName = "%" + param.name;
                            paramInfo.isGlobal = false;
                            paramInfo.isMutable = true;
                            paramInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                            b_.addVariable(paramInfo);
                        }

                        // (AR) بناء جسم الدالة
                        // (EN) Build function body
                        if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt *>(funcDecl->body.get()))
                        {
                            for (const auto &bodyStmt : blockStmt->statements)
                            {
                                if (bodyStmt)
                                {
                                    b_.buildStatement(bodyStmt.get());
                                }
                            }
                        }
                        else
                        {
                            b_.buildStatement(funcDecl->body.get());
                        }

                        b_.exitScope();

                        // (AR) إضافة terminator إن لم يوجد
                        // (EN) Add terminator if none present
                        if (b_.currentBlock_)
                        {
                            bool hasTerminator = false;
                            if (!b_.currentBlock_->instructions.empty())
                            {
                                auto lastOp = b_.currentBlock_->instructions.back().opcode;
                                hasTerminator = (lastOp == SIR::SIROpcode::RET || lastOp == SIR::SIROpcode::RET_VOID);
                            }
                            if (!hasTerminator)
                            {
                                SIR::SIRInstruction retInst;
                                if (retType == SIR::SadTypeKind::Void)
                                {
                                    retInst.opcode = SIR::SIROpcode::RET_VOID;
                                }
                                else
                                {
                                    retInst.opcode = SIR::SIROpcode::RET;
                                    if (retType == SIR::SadTypeKind::String)
                                    {
                                        retInst.operands.push_back(SIR::SIROperand::ConstantString(""));
                                    }
                                    else
                                    {
                                        retInst.operands.push_back(SIR::SIROperand::ConstantI64(0));
                                    }
                                }
                                b_.currentBlock_->addInstruction(retInst);
                            }
                        }

                        b_.currentFunction_ = prevFunction;
                        b_.currentBlock_ = prevBlock;
                        b_.currentClassName_ = prevClassName;
                    }

                    // (AR) إضافة الدالة للوحدة
                    // (EN) Add function to module
                    b_.module_->addFunction(sirMethod);

                    // (AR) تسجيل الدالة في جدول الدوال — مطلوب لبحث الطرق عند الاستدعاء
                    // (EN) Register function in function table — required for method lookup at call site
                    {
                        FunctionInfo methodInfo;
                        methodInfo.name = fullMethodName;
                        methodInfo.returnType = retType;
                        methodInfo.sirFunction = sirMethod;
                        b_.functionTable_[fullMethodName] = methodInfo;
                    }
                }

                // (AR) التحقق من تنفيذ جميع دوال السمة المطلوبة + توليد الدوال الافتراضية
                // (EN) Validate all required trait methods + generate default methods
                if (!implDecl->traitName.empty())
                {
                    auto *trait = b_.module_->getTrait(implDecl->traitName);
                    if (trait)
                    {
                        // (AR) جمع كل السمات المطلوبة (بما فيها سمات الأب بشكل متعدٍّ)
                        // (EN) Collect all required traits (including transitive super traits)
                        std::vector<const SIR::SIRTrait *> allTraits;
                        std::unordered_set<std::string> visited;
                        std::function<void(const SIR::SIRTrait *)> collectTraits;
                        collectTraits = [&](const SIR::SIRTrait *t)
                        {
                            if (!t || visited.count(t->name))
                                return;
                            visited.insert(t->name);
                            // (AR) جمع سمات الأب أولاً
                            // (EN) Collect super traits first
                            for (const auto &superName : t->superTraits)
                            {
                                auto *superTrait = b_.module_->getTrait(superName);
                                if (superTrait)
                                {
                                    collectTraits(superTrait);
                                }
                                else
                                {
                                    b_.errors_.push_back(
                                        "(AR) ❌ السمة '" + t->name + "' ترث من سمة غير معرّفة '" + superName + "'.\n"
                                                                                                               "(EN) Trait '" +
                                        t->name + "' extends undefined trait '" + superName + "'.");
                                }
                            }
                            allTraits.push_back(t);
                        };
                        collectTraits(trait);

                        // (AR) لكل سمة مطلوبة، تحقق من الدوال وولّد الافتراضية
                        // (EN) For each required trait, check methods and generate defaults
                        for (const auto *requiredTrait : allTraits)
                        {
                            // (AR) ابحث عن AST السمة لاستخراج الدوال الافتراضية
                            // (EN) Find trait AST for extracting default implementations
                            auto traitAstIt = b_.traitDefaultImpls_.find(requiredTrait->name);
                            AST::TraitDecl *traitAst = (traitAstIt != b_.traitDefaultImpls_.end()) ? traitAstIt->second : nullptr;

                            for (size_t mi = 0; mi < requiredTrait->methods.size(); mi++)
                            {
                                const auto &reqMethod = requiredTrait->methods[mi];
                                std::string fullName = className + "." + reqMethod.name;
                                auto existingMethod = sirClass->getMethod(fullName);

                                if (!existingMethod)
                                {
                                    if (reqMethod.hasDefaultImpl && traitAst && mi < traitAst->methods.size())
                                    {
                                        // (AR) الدالة غير منفذة لكن لها تنفيذ افتراضي — ولّدها
                                        // (EN) Method not implemented but has default impl — generate it
                                        const auto &traitMethod = traitAst->methods[mi];
                                        if (traitMethod.defaultImpl)
                                        {
#ifndef NDEBUG
                                            std::cout << "[DEBUG] buildImpl: generating default method '"
                                                      << fullName << "' from trait '" << requiredTrait->name << "'" << std::endl;
#endif

                                            // (AR) تحويل نوع الإرجاع — مع استنتاج تلقائي
                                            //      [إصلاح BF-04] نفس إصلاح buildImpl الأساسي
                                            // (EN) Convert return type — with automatic inference
                                            SIR::SadTypeKind retType;
                                            if (traitMethod.returnType == Data::DataType::UNKNOWN ||
                                                traitMethod.returnType == Data::DataType::NONE)
                                            {
                                                auto savedClassName = b_.currentClassName_;
                                                b_.currentClassName_ = className;
                                                retType = b_.inferReturnTypeFromBody(traitMethod.defaultImpl.get());
                                                b_.currentClassName_ = savedClassName;
                                            }
                                            else
                                            {
                                                retType = b_.astTypeToSIRType(traitMethod.returnType);
                                            }
                                            auto sirMethod = std::make_shared<SIR::SIRFunction>(fullName, retType);

                                            // (AR) إضافة معامل self ضمنياً
                                            // (EN) Add implicit self parameter
                                            sirMethod->addParameter(SIR::SIRParameter(kSelfParamName, SIR::SadTypeKind::Integer));

                                            for (const auto &param : traitMethod.params)
                                            {
                                                SIR::SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                                                sirMethod->addParameter(SIR::SIRParameter(param.name, paramType));
                                            }

                                            sirClass->addMethod(sirMethod);

                                            // (AR) بناء جسم الدالة الافتراضية
                                            // (EN) Build default method body
                                            auto prevFunction = b_.currentFunction_;
                                            auto prevBlock = b_.currentBlock_;
                                            auto prevClassName = b_.currentClassName_;

                                            b_.currentFunction_ = sirMethod;
                                            b_.currentClassName_ = className;

                                            // (AR) إنشاء كتلة الدخول — مطلوب لتوليد التعليمات
                                            // (EN) Create entry basic block — required for instruction generation
                                            auto defaultEntryBlock = b_.createBasicBlock(kEntryBlockName);
                                            sirMethod->addBasicBlock(defaultEntryBlock);
                                            b_.currentBlock_ = defaultEntryBlock;

                                            // (AR) تخصيص حقول الصنف — لإتاحة الوصول عبر self
                                            // (EN) Alloca for class fields — enables access through self
                                            for (const auto &field : sirClass->fields_)
                                            {
                                                SIR::SIRInstruction allocInst;
                                                allocInst.opcode = SIR::SIROpcode::ALLOC;
                                                allocInst.result = SIR::SIROperand::Register("%" + field.first, field.second);
                                                b_.currentBlock_->addInstruction(allocInst);
                                            }

                                            b_.enterScope();

                                            {
                                                VariableInfo selfInfo;
                                                selfInfo.name = kSelfParamName;
                                                selfInfo.type = SIR::SadTypeKind::Integer;
                                                selfInfo.registerName = kSelfRegisterName;
                                                selfInfo.isGlobal = false;
                                                selfInfo.isMutable = false;
                                                selfInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                                                b_.addVariable(selfInfo);
                                            }

                                            for (const auto &param : traitMethod.params)
                                            {
                                                VariableInfo paramInfo;
                                                paramInfo.name = param.name;
                                                paramInfo.type = b_.astTypeToSIRType(param.type);
                                                paramInfo.registerName = "%" + param.name;
                                                paramInfo.isGlobal = false;
                                                paramInfo.isMutable = true;
                                                paramInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                                                b_.addVariable(paramInfo);
                                            }

                                            if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt *>(traitMethod.defaultImpl.get()))
                                            {
                                                for (const auto &bodyStmt : blockStmt->statements)
                                                {
                                                    if (bodyStmt)
                                                    {
                                                        b_.buildStatement(bodyStmt.get());
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                b_.buildStatement(traitMethod.defaultImpl.get());
                                            }

                                            b_.exitScope();

                                            // (AR) إضافة terminator إن لم يوجد
                                            // (EN) Add terminator if none present
                                            if (b_.currentBlock_)
                                            {
                                                bool hasTerminator = false;
                                                if (!b_.currentBlock_->instructions.empty())
                                                {
                                                    auto lastOp = b_.currentBlock_->instructions.back().opcode;
                                                    hasTerminator = (lastOp == SIR::SIROpcode::RET || lastOp == SIR::SIROpcode::RET_VOID);
                                                }
                                                if (!hasTerminator)
                                                {
                                                    SIR::SIRInstruction retInst;
                                                    if (retType == SIR::SadTypeKind::Void)
                                                    {
                                                        retInst.opcode = SIR::SIROpcode::RET_VOID;
                                                    }
                                                    else
                                                    {
                                                        retInst.opcode = SIR::SIROpcode::RET;
                                                        if (retType == SIR::SadTypeKind::String)
                                                        {
                                                            retInst.operands.push_back(SIR::SIROperand::ConstantString(""));
                                                        }
                                                        else
                                                        {
                                                            retInst.operands.push_back(SIR::SIROperand::ConstantI64(0));
                                                        }
                                                    }
                                                    b_.currentBlock_->addInstruction(retInst);
                                                }
                                            }

                                            b_.currentFunction_ = prevFunction;
                                            b_.currentBlock_ = prevBlock;
                                            b_.currentClassName_ = prevClassName;

                                            b_.module_->addFunction(sirMethod);

                                            // (AR) تسجيل الدالة في جدول الدوال
                                            // (EN) Register function in function table
                                            {
                                                FunctionInfo defaultMethodInfo;
                                                defaultMethodInfo.name = fullName;
                                                defaultMethodInfo.returnType = retType;
                                                defaultMethodInfo.sirFunction = sirMethod;
                                                b_.functionTable_[fullName] = defaultMethodInfo;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        // (AR) الدالة مطلوبة ولا تنفيذ افتراضي — خطأ
                                        // (EN) Required method with no default — error
                                        std::string errorMsg =
                                            "(AR) ❌ الصنف '" + className + "' لا ينفّذ الدالة المطلوبة '" + reqMethod.name + "' من السمة '" + requiredTrait->name + "'.\n"
                                                                                                                                                                    "(EN) Class '" +
                                            className + "' does not implement required method '" + reqMethod.name + "' from trait '" + requiredTrait->name + "'.";
                                        b_.errors_.push_back(errorMsg);
                                    }
                                }
                            }
                        }

                        // (AR) تسجيل جميع السمات المنفذة (بما فيها الأب)
                        // (EN) Record all implemented traits (including parents)
                        for (const auto *t : allTraits)
                        {
                            bool alreadyRegistered = false;
                            for (const auto &existing : sirClass->implementedTraits)
                            {
                                if (existing == t->name)
                                {
                                    alreadyRegistered = true;
                                    break;
                                }
                            }
                            if (!alreadyRegistered)
                            {
                                sirClass->implementedTraits.push_back(t->name);
                            }
                        }
                    }
                    else
                    {
                        b_.errors_.push_back(
                            "(AR) ❌ السمة '" + implDecl->traitName + "' غير معرّفة.\n"
                                                                      "(EN) Trait '" +
                            implDecl->traitName + "' is not defined.");
                    }
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildImpl: finished impl for '" << className << "'" << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad