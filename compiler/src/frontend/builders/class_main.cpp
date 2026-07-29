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
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildClass - بناء صنف
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:364
            // التوقيع / Signature: void buildClass(AST::ClassDeclNode* classDecl);
            //
            // المعاملات / Parameters:
            // - classDecl: AST::ClassDeclNode* = Sad::AST::ClassDecl* (sir_builder.h:60)
            //
            // ClassDecl Members (declarations.h:122-156):
            // - name: std::string (line 125)
            // - superclasses: std::vector<std::string> (line 126)
            // - members: StmtList (line 127)
            // - typeParameters: std::vector<TypeParameter> (line 128)
            //
            // الدوال المستدعاة / Called functions:
            // - std::make_shared<SIRClass>: sir_module.h:409
            // - b_.module_->addClass: sir_module.h:608
            // ============================================================================
            void ClassBuilder::buildClass(AST::ClassDeclNode *classDecl)
            {
                if (!classDecl)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildClass: processing class '" << classDecl->name << "'" << std::endl;
#endif

                // (AR) تحديد الصنف الأب (إن وجد)
                // (EN) Determine parent class (if any)
                std::string parentClass = "";
                if (!classDecl->superclasses.empty())
                {
                    parentClass = classDecl->superclasses[0]; // دعم وراثة واحدة حالياً
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildClass: parent class = '" << parentClass << "'" << std::endl;
#endif
                }

                // (AR) إنشاء صنف SIR (SIRClass constructor: sir_module.h:409)
                // (EN) Create SIR class
                auto sirClass = std::make_shared<SIRClass>(classDecl->name, parentClass);

                // (AR) تعيين علامة المجرد / (EN) Set abstract flag
                sirClass->isAbstract = classDecl->isAbstract;

                // (AR) تعيين علامة الصنف المحكم / (EN) Set sealed flag
                sirClass->isSealed = classDecl->isSealed;
                sirClass->sourceFile = classDecl->sourceFile;

                // (AR) فحص الوراثة من صنف محكم / (EN) Check inheritance from sealed class
                if (!parentClass.empty())
                {
                    auto parentSirClass = b_.module_->getClass(parentClass);
                    if (parentSirClass && parentSirClass->isSealed && !parentSirClass->sourceFile.empty())
                    {
                        if (classDecl->sourceFile != parentSirClass->sourceFile)
                        {
                            std::cerr << "(AR) خطأ: لا يمكن وراثة الصنف المحكم '" << parentClass
                                      << "' من خارج ملفه المصدري.\n"
                                      << "(EN) Error: Cannot inherit from sealed class '" << parentClass
                                      << "' outside its source file." << std::endl;
                            return;
                        }
                    }
                }

                // (AR) نسخ حقول الصنف الأب إلى الصنف الابن (دعم الوراثة)
                // (EN) Copy parent class fields into child class (inheritance support)
                if (!parentClass.empty())
                {
                    auto parentSirClass = b_.module_->getClass(parentClass);
                    if (parentSirClass)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildClass: inheriting " << parentSirClass->fields_.size()
                                  << " fields from parent '" << parentClass << "'" << std::endl;
#endif
                        // (AR) إضافة حقول الأب بالترتيب أولاً
                        // (EN) Add parent fields in order first
                        for (const auto &parentFieldName : parentSirClass->fieldOrder_)
                        {
                            auto fieldIt = parentSirClass->fields_.find(parentFieldName);
                            if (fieldIt != parentSirClass->fields_.end())
                            {
                                sirClass->addField(parentFieldName, fieldIt->second);
                            }
                        }
                    }
                    else
                    {
                        std::cerr << "[WARNING] buildClass: parent class '" << parentClass
                                  << "' not found in module (must be declared before child class)" << std::endl;
                    }
                }

                // ═══════════════════════════════════════════════════════════════════════
                // (AR) تسجيل الصنف مبكراً في الوحدة قبل بناء أجسام الميثودات والعوامل
                //      هذا ضروري لأن b_.buildNewObject داخل جسم عامل (مثل: ارجع جديد متجه(...))
                //      يبحث عن الصنف في b_.module_->getClass() — إذا لم يكن مسجلاً سيفشل.
                //      بما أن sirClass هو shared_ptr، أي تعديلات لاحقة (إضافة حقول/ميثودات)
                //      تنعكس تلقائياً على المرجع المخزن في b_.module_.
                // (EN) Register class early in module BEFORE building method/operator bodies.
                //      This is necessary because b_.buildNewObject inside an operator body
                //      (e.g., return new Vector(...)) searches b_.module_->getClass() — it fails
                //      if class isn't registered yet. Since sirClass is shared_ptr, later
                //      modifications (adding fields/methods) auto-reflect in b_.module_.
                // ═══════════════════════════════════════════════════════════════════════
                b_.module_->addClass(sirClass);

                // (AR) معالجة أعضاء الصنف (members)
                // (EN) Process class members
                for (const auto &member : classDecl->members)
                {
                    // (AR) التحقق من نوع العضو: حقل أو دالة
                    // (EN) Check member type: field or method

                    // (AR) الحقول (FieldDecl - declarations.h:180)
                    // (EN) Fields
                    if (auto fieldDecl = dynamic_cast<AST::FieldDecl *>(member.get()))
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildClass: found field '" << fieldDecl->name
                                  << "' isStatic=" << fieldDecl->isStatic << std::endl;
#endif

                        // (AR) تحويل النوع وإضافة الحقل
                        // (EN) Convert type and add field
                        SadTypeKind fieldType = b_.astTypeToSIRType(fieldDecl->type);

                        // (AR) الحقول الديناميكية (UNKNOWN/OBJECT): استنتاج النوع من المُهيئ أولاً
                        // (EN) Dynamic fields (UNKNOWN/OBJECT): infer type from initializer first
                        if (fieldDecl->type == Types::SadTypeKind::Unknown ||
                            fieldDecl->type == Types::SadTypeKind::Class)
                        {
                            fieldType = SadTypeKind::Pointer; // الافتراضي / default

                            // (AR) ISSUE-058: حقلٌ غير مُصرَّح بلا مُهيّئ — استنتِج نوعه من
                            //      إسناداته عبر البرنامج (Phase 0.5) بدل Pointer الذي ينهار
                            //      عند تخزين عدد. لا نطبّقه إلّا حين لا مُهيّئ (المُهيّئ أدقّ).
                            // (EN) ISSUE-058: untyped field with no initializer — infer its type
                            //      from its program-wide assignments (Phase 0.5) instead of the
                            //      Pointer default that crashes when an integer is stored. Only
                            //      when there is no initializer (an initializer is more precise).
                            if (!fieldDecl->initializer)
                            {
                                // (AR) المفتاح مؤهَّل بالصنف «صنف.حقل» لمنع التلوّث العابر للأصناف
                                // (EN) Class-qualified key "class.field" to prevent cross-class pollution
                                auto infIt = b_.inferredFieldTypes_.find(classDecl->name + "." + fieldDecl->name);
                                if (infIt != b_.inferredFieldTypes_.end() &&
                                    infIt->second != SadTypeKind::Unknown)
                                {
                                    fieldType = infIt->second;
                                }
                            }

                            // (AR) استنتاج النوع من القيمة الابتدائية
                            // (EN) Infer type from initializer value
                            if (fieldDecl->initializer)
                            {
                                if (auto *lit = dynamic_cast<AST::LiteralExpr *>(fieldDecl->initializer.get()))
                                {
                                    auto tokType = lit->token.getType();
                                    if (tokType == Sad::Lexer::TokenType::NUMBER_INTEGER)
                                    {
                                        fieldType = SadTypeKind::Integer;
                                    }
                                    else if (tokType == Sad::Lexer::TokenType::NUMBER_DOUBLE)
                                    {
                                        fieldType = SadTypeKind::Float;
                                    }
                                    else if (tokType == Sad::Lexer::TokenType::STRING_LITERAL)
                                    {
                                        fieldType = SadTypeKind::String;
                                    }
                                    else if (tokType == Sad::Lexer::TokenType::LITERAL_TRUE ||
                                             tokType == Sad::Lexer::TokenType::LITERAL_FALSE)
                                    {
                                        fieldType = SadTypeKind::Boolean;
                                    }
                                    // (AR) [] مصفوفة فارغة تبقى PTR
                                    // (EN) [] empty array stays PTR
                                }
                                // (AR) كشف تهيئة المصفوفة الفارغة [] — تسجيل الحقل كمصفوفة
                                //      لتخصيص SadArray في المنشئ بدلاً من ترك مؤشر null
                                // (EN) Detect empty array initializer [] — mark field as array
                                //      so emitConstructorCall allocates SadArray instead of leaving null ptr
                                if (dynamic_cast<Sad::AST::ArrayExpr *>(fieldDecl->initializer.get()))
                                {
                                    sirClass->markFieldAsArray(fieldDecl->name);
                                }
                            }
                        }

                        // ═══════════════════════════════════════════════════════════════
                        // (AR) تسجيل القيمة الابتدائية للحقل إن وُجدت (لتطبيقها في emitConstructorCall)
                        //      هذا ضروري للأصناف بدون باني حيث memset(0) يُصفّر كل الحقول
                        //      وأيضاً للحقول الموروثة التي يجب تهيئتها بقيمها الابتدائية
                        // (EN) Record field default value if present (applied in emitConstructorCall)
                        //      Necessary for classes without constructors where memset(0) zeroes all fields
                        //      Also for inherited fields that must be initialized with their default values
                        // ═══════════════════════════════════════════════════════════════
                        if (fieldDecl->initializer)
                        {
                            if (auto *lit = dynamic_cast<AST::LiteralExpr *>(fieldDecl->initializer.get()))
                            {
                                auto tokType = lit->token.getType();
                                std::string litVal = lit->token.getValue();
                                if (tokType == Sad::Lexer::TokenType::NUMBER_INTEGER ||
                                    tokType == Sad::Lexer::TokenType::NUMBER_DOUBLE ||
                                    tokType == Sad::Lexer::TokenType::STRING_LITERAL ||
                                    tokType == Sad::Lexer::TokenType::LITERAL_TRUE ||
                                    tokType == Sad::Lexer::TokenType::LITERAL_FALSE)
                                {
                                    sirClass->fieldDefaultValues_[fieldDecl->name] = {litVal, fieldType};
#ifndef NDEBUG
                                    std::cout << "[DEBUG] buildClass: field '" << fieldDecl->name
                                              << "' default value = '" << litVal
                                              << "' type=" << static_cast<int>(fieldType) << std::endl;
#endif
                                }
                            }
                        }

                        // ================================================================
                        // (AR) الحقول الساكنة: تُنشأ كمتغيرات عامة وليست حقول نسخة
                        // (EN) Static fields: created as global variables, not instance fields
                        // ================================================================
                        if (fieldDecl->isStatic)
                        {
                            std::string globalName = classDecl->name + "." + fieldDecl->name;
                            VariableInfo globalVar;
                            globalVar.name = globalName;
                            globalVar.type = fieldType;
                            globalVar.registerName = "@" + globalName;
                            globalVar.isGlobal = true;
                            b_.addVariable(globalVar);

                            if (b_.module_)
                            {
                                std::string initVal = "0";
                                if (fieldDecl->initializer)
                                {
                                    auto initResult = b_.buildExpression(fieldDecl->initializer.get());
                                    if (initResult.isConstant && !initResult.constantValue.empty())
                                    {
                                        initVal = initResult.constantValue;
                                    }
                                }
                                auto gv = std::make_shared<SIRGlobalVariable>(
                                    globalName, fieldType, initVal, false);
                                b_.module_->addGlobalVariable(gv);
                            }

                            b_.staticFields_[globalName] = fieldType;
                            continue; // (AR) لا نضيفه كحقل نسخة
                        }

                        sirClass->addField(fieldDecl->name, fieldType);
                    }

                    // (AR) الباني (ConstructorDecl) — CW-05: مُستخرج إلى buildClassConstructor
                    // (EN) Constructor — CW-05: extracted to buildClassConstructor
                    else if (auto ctorDecl = dynamic_cast<Sad::AST::ConstructorDecl *>(member.get()))
                    {
                        buildClassConstructor(classDecl, sirClass, ctorDecl);
                    }

                    // (AR) الخصائص (PropertyDecl): تخفيضها إلى __get_*/__set_* ضمن سياق الصنف الحالي
                    // (EN) Properties: lower to __get_*/__set_* within current class context
                    else if (auto propDecl = dynamic_cast<Sad::AST::PropertyDecl *>(member.get()))
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildClass: found property '" << propDecl->name << "'" << std::endl;
#endif
                        auto savedClassName = b_.currentClassName_;
                        b_.currentClassName_ = classDecl->name;
                        b_.buildStatement(propDecl);
                        b_.currentClassName_ = savedClassName;
                    }

                    // (AR) الدوال (MethodDecl - declarations.h:222)
                    // (EN) Methods
                    else if (auto methodDecl = dynamic_cast<AST::MethodDecl *>(member.get()))
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildClass: found method '" << methodDecl->name << "'" << std::endl;
#endif

                        // (AR) تخطي الدوال المجردة — لا تملك جسماً يُبنى
                        // (EN) Skip abstract methods — they have no body to build
                        if (methodDecl->isAbstract)
                        {
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildClass: skipping abstract method '" << methodDecl->name << "'" << std::endl;
#endif
                            continue;
                        }

                        // (AR) استنتاج نوع الإرجاع إذا لم يُحدد
                        // (EN) Infer return type if not specified
                        SadTypeKind returnType;
                        if (methodDecl->returnType == Types::SadTypeKind::Unknown ||
                            methodDecl->returnType == Types::SadTypeKind::Void)
                        {
                            // (AR) تعيين اسم الصنف الحالي مؤقتاً لتمكين b_.inferReturnTypeFromBody
                            //      من البحث عن أنواع الحقول عبر b_.module_->getClass(b_.currentClassName_)
                            //      عند مطابقة MemberExpr (هذا.حقل)
                            // (EN) Temporarily set b_.currentClassName_ so b_.inferReturnTypeFromBody
                            //      can look up field types via b_.module_->getClass(b_.currentClassName_)
                            //      when matching MemberExpr (this.field)
                            auto savedClassName = b_.currentClassName_;
                            b_.currentClassName_ = classDecl->name;
                            returnType = b_.inferReturnTypeFromBody(methodDecl->body.get());
                            b_.currentClassName_ = savedClassName;
                        }
                        else
                        {
                            returnType = b_.astTypeToSIRType(methodDecl->returnType);
                        }
                        std::string fullMethodName = classDecl->name + "." + methodDecl->name;
                        auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);

                        // (AR) الدوال الساكنة لا تأخذ self
                        // (EN) Static methods don't take self
                        bool isStaticMethod = methodDecl->isStatic;
                        if (!isStaticMethod)
                        {
                            sirMethod->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));
                        }

                        // ================================================================
                        // (AR) استخدام الأنواع المُستنتجة من b_.functionTable_ (المرحلة 1.7)
                        //      إذا كانت الدالة سُجلت مسبقاً في المرحلة 1.3 وتم تحديث أنواعها
                        //      نستخدم الأنواع المحدثة بدلاً من أنواع AST الافتراضية
                        // (EN) Use inferred types from b_.functionTable_ (Phase 1.7)
                        //      If the method was pre-registered in Phase 1.3 and types were updated
                        //      use the updated types instead of default AST types
                        // ================================================================
                        {
                            auto ftIt = b_.functionTable_.find(fullMethodName);
                            if (ftIt != b_.functionTable_.end())
                            {
                                auto &inferredParams = ftIt->second.parameters;
                                for (const auto &param : methodDecl->parameters)
                                {
                                    SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                                    // (AR) ابحث عن النوع المُستنتج للمعامل
                                    for (const auto &ip : inferredParams)
                                    {
                                        if (ip.name == param.name && ip.type != SadTypeKind::Integer)
                                        {
                                            paramType = ip.type;
                                            break;
                                        }
                                        // (AR) إذا كان النوع الأصلي UNKNOWN (= Integer) والمُستنتج أكثر تحديداً
                                        if (ip.name == param.name && param.type == Types::SadTypeKind::Unknown && ip.type != paramType)
                                        {
                                            paramType = ip.type;
                                            break;
                                        }
                                    }
                                    // (AR) بذر صنف المعامل المصرَّح («حدث ح» في طريقة) — نظير
                                    //      الدوالّ الحرّة في sir_builder_functions (جولة أميليا ٢)
                                    // (EN) Seed declared param class («حدث ح» in a method) —
                                    //      mirrors free functions in sir_builder_functions
                                    SIRParameter sp(param.name, paramType);
                                    if (!param.typeName.empty() && b_.module_ && b_.module_->getClass(param.typeName))
                                        sp.className = param.typeName;
                                    sirMethod->addParameter(sp);
                                }
                            }
                            else
                            {
                                for (const auto &param : methodDecl->parameters)
                                {
                                    SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                                    SIRParameter sp(param.name, paramType);
                                    if (!param.typeName.empty() && b_.module_ && b_.module_->getClass(param.typeName))
                                        sp.className = param.typeName;
                                    sirMethod->addParameter(sp);
                                }
                            }
                        }

                        sirClass->addMethod(sirMethod);

                        // (AR) بناء جسم الطريقة
                        // (EN) Build method body
                        if (methodDecl->body)
                        {
                            auto prevFunction = b_.currentFunction_;
                            auto prevBlock = b_.currentBlock_;
                            auto prevClassName = b_.currentClassName_;

                            b_.currentFunction_ = sirMethod;
                            b_.currentClassName_ = classDecl->name;

                            b_.enterScope();

                            // (AR) تسجيل self + معاملات + حقول الصنف
                            // (EN) Register self + params + class fields
                            if (!isStaticMethod)
                            {
                                {
                                    VariableInfo selfInfo;
                                    selfInfo.name = kSelfParamName;
                                    selfInfo.type = SadTypeKind::Integer;
                                    selfInfo.registerName = kSelfRegisterName;
                                    selfInfo.isGlobal = false;
                                    selfInfo.isMutable = false;
                                    selfInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                    b_.addVariable(selfInfo);
                                }

                                // (AR) تسجيل "هذا" كمرادف لـ self
                                // (EN) Register "هذا" (this) as alias for self
                                {
                                    VariableInfo thisInfo;
                                    thisInfo.name = kThisAliasName; // هذا
                                    thisInfo.type = SadTypeKind::Integer;
                                    thisInfo.registerName = kSelfRegisterName;
                                    thisInfo.isGlobal = false;
                                    thisInfo.isMutable = false;
                                    thisInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                    b_.addVariable(thisInfo);
                                }
                            }

                            // (AR) إصلاح: بناء مجموعة أسماء المعاملات لمنع تضارب الأسماء مع الحقول
                            // (EN) Fix: build param name set to prevent name collision with fields
                            std::unordered_set<std::string> methodParamNames;
                            for (const auto &param : methodDecl->parameters)
                            {
                                methodParamNames.insert(param.name);
                                VariableInfo paramInfo;
                                paramInfo.name = param.name;
                                paramInfo.type = b_.astTypeToSIRType(param.type);
                                paramInfo.registerName = "%" + param.name;
                                paramInfo.isGlobal = false;
                                paramInfo.isMutable = false;
                                paramInfo.isParameter = true;
                                paramInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                b_.addVariable(paramInfo);
                            }

                            // (AR) تسجيل الحقول — تخطي المتطابقة مع المعاملات
                            //      والدوال الساكنة لا تصل لحقول النسخة
                            // (EN) Register fields — skip those matching parameter names
                            //      Static methods don't access instance fields
                            if (!isStaticMethod)
                            {
                                for (const auto &field : sirClass->fields_)
                                {
                                    if (methodParamNames.count(field.first) > 0)
                                    {
                                        continue;
                                    }
                                    VariableInfo fieldInfo;
                                    fieldInfo.name = field.first;
                                    fieldInfo.type = field.second;
                                    fieldInfo.registerName = "%" + field.first;
                                    fieldInfo.isGlobal = false;
                                    fieldInfo.isMutable = true;
                                    fieldInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                    b_.addVariable(fieldInfo);
                                }
                            }

                            auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                            sirMethod->addBasicBlock(entryBlock);
                            b_.currentBlock_ = entryBlock;

                            // (AR) Alloca لكل حقل — تخطي الدوال الساكنة والمتطابقة مع المعاملات
                            // (EN) Alloca for each field — skip static methods and param-matching fields
                            if (!isStaticMethod)
                            {
                                for (const auto &field : sirClass->fields_)
                                {
                                    if (methodParamNames.count(field.first) > 0)
                                    {
                                        continue;
                                    }
                                    SIRInstruction allocInst;
                                    allocInst.opcode = SIROpcode::ALLOC;
                                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                                    b_.currentBlock_->addInstruction(allocInst);
                                }
                            }

                            // (AR) تسجيل/تحديث في b_.functionTable_ قبل بناء الجسم
                            //      نحافظ على الأنواع المُستنتجة من المرحلة 1.7
                            //      ونُحدّث فقط returnType و sirFunction
                            // (EN) Register/update in b_.functionTable_ before building body
                            //      Preserve inferred types from Phase 1.7
                            //      Only update returnType and sirFunction
                            {
                                auto existingIt = b_.functionTable_.find(fullMethodName);
                                if (existingIt != b_.functionTable_.end())
                                {
                                    // (AR) تحديث فقط — لا تمسح الأنواع المُستنتجة
                                    existingIt->second.returnType = returnType;
                                    existingIt->second.sirFunction = sirMethod;
                                }
                                else
                                {
                                    FunctionInfo preInfo;
                                    preInfo.name = fullMethodName;
                                    preInfo.returnType = returnType;
                                    preInfo.sirFunction = sirMethod;
                                    b_.functionTable_[fullMethodName] = preInfo;
                                }
                            }

                            // (AR) تسجيل الدالة الساكنة
                            // (EN) Register static method
                            if (isStaticMethod)
                            {
                                b_.staticMethods_.insert(fullMethodName);
                            }

                            b_.buildStatement(methodDecl->body.get());

                            // (AR) إضافة terminator
                            // (EN) Add terminator
                            if (b_.currentBlock_)
                            {
                                bool hasTerminator = false;
                                if (!b_.currentBlock_->instructions.empty())
                                {
                                    auto lastOp = b_.currentBlock_->instructions.back().opcode;
                                    hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                                }
                                if (!hasTerminator)
                                {
                                    SIRInstruction retInst;
                                    if (returnType == SadTypeKind::Void)
                                    {
                                        retInst.opcode = SIROpcode::RET_VOID;
                                    }
                                    else
                                    {
                                        retInst.opcode = SIROpcode::RET;
                                        if (returnType == SadTypeKind::String)
                                        {
                                            retInst.operands.push_back(SIROperand::ConstantString(""));
                                        }
                                        else
                                        {
                                            retInst.operands.push_back(SIROperand::ConstantI64(0));
                                        }
                                    }
                                    b_.currentBlock_->addInstruction(retInst);
                                }
                            }

                            b_.exitScope();

                            b_.module_->addFunction(sirMethod);

                            // (AR) حفظ returnClassName المُعيَّن أثناء بناء جسم الطريقة
                            // (EN) Preserve returnClassName set during method body build
                            std::string savedReturnClassName1;
                            auto prevFtIt1 = b_.functionTable_.find(fullMethodName);
                            if (prevFtIt1 != b_.functionTable_.end())
                            {
                                savedReturnClassName1 = prevFtIt1->second.returnClassName;
                            }

                            FunctionInfo methodInfo;
                            methodInfo.name = fullMethodName;
                            methodInfo.returnType = returnType;
                            methodInfo.parameters = sirMethod->getParameters();
                            methodInfo.sirFunction = sirMethod;
                            methodInfo.returnClassName = savedReturnClassName1;
                            // (AR) [وسم زمن-التشغيل] احفظ نوعَ عنصر المصفوفة المُرجعة (كما أعلاه).
                            // (EN) [runtime tags] preserve the returned array element type (as above).
                            {
                                auto prevMi = b_.functionTable_.find(fullMethodName);
                                if (prevMi != b_.functionTable_.end())
                                    methodInfo.returnElementType = prevMi->second.returnElementType;
                            }
                            b_.functionTable_[fullMethodName] = methodInfo;

                            b_.currentFunction_ = prevFunction;
                            b_.currentBlock_ = prevBlock;
                            b_.currentClassName_ = prevClassName;
                        }
                    }

                    // (AR) الدالة العادية داخل الصنف (FunctionDecl)
                    // (EN) Regular function inside class (FunctionDecl)
                    else if (auto funcDecl = dynamic_cast<AST::FunctionDecl *>(member.get()))
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildClass: found function as method '" << funcDecl->name << "'" << std::endl;
#endif

                        // (AR) استنتاج نوع الإرجاع إذا لم يُحدد
                        // (EN) Infer return type if not specified
                        SadTypeKind returnType;
                        if (funcDecl->returnType == Types::SadTypeKind::Unknown ||
                            funcDecl->returnType == Types::SadTypeKind::Void)
                        {
                            auto savedClassName = b_.currentClassName_;
                            b_.currentClassName_ = classDecl->name;
                            returnType = b_.inferReturnTypeFromBody(funcDecl->body.get());
                            b_.currentClassName_ = savedClassName;
                        }
                        else
                        {
                            returnType = b_.astTypeToSIRType(funcDecl->returnType);
                        }
                        std::string fullMethodName = classDecl->name + "." + funcDecl->name;
                        auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);

                        sirMethod->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        for (const auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                            // (AR) بذر صنف المعامل المصرَّح (جولة أميليا ٢)
                            // (EN) Seed declared param class (Amelia round 2)
                            SIRParameter sp(param.name, paramType);
                            if (!param.typeName.empty() && b_.module_ && b_.module_->getClass(param.typeName))
                                sp.className = param.typeName;
                            sirMethod->addParameter(sp);
                        }

                        sirClass->addMethod(sirMethod);

                        // (AR) بناء جسم الدالة (مثل الطريقة تماماً)
                        // (EN) Build function body (same as method)
                        if (funcDecl->body)
                        {
                            auto prevFunction = b_.currentFunction_;
                            auto prevBlock = b_.currentBlock_;
                            auto prevClassName = b_.currentClassName_;

                            b_.currentFunction_ = sirMethod;
                            b_.currentClassName_ = classDecl->name;

                            b_.enterScope();

                            {
                                VariableInfo selfInfo;
                                selfInfo.name = kSelfParamName;
                                selfInfo.type = SadTypeKind::Integer;
                                selfInfo.registerName = kSelfRegisterName;
                                selfInfo.isGlobal = false;
                                selfInfo.isMutable = false;
                                selfInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                b_.addVariable(selfInfo);
                            }

                            // (AR) تسجيل "هذا" كمرادف لـ self
                            // (EN) Register "هذا" (this) as alias for self
                            {
                                VariableInfo thisInfo;
                                thisInfo.name = kThisAliasName; // هذا
                                thisInfo.type = SadTypeKind::Integer;
                                thisInfo.registerName = kSelfRegisterName;
                                thisInfo.isGlobal = false;
                                thisInfo.isMutable = false;
                                thisInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                b_.addVariable(thisInfo);
                            }

                            for (const auto &param : funcDecl->parameters)
                            {
                                VariableInfo paramInfo;
                                paramInfo.name = param.name;
                                paramInfo.type = b_.astTypeToSIRType(param.type);
                                paramInfo.registerName = "%" + param.name;
                                paramInfo.isGlobal = false;
                                paramInfo.isMutable = false;
                                paramInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                b_.addVariable(paramInfo);
                            }

                            for (const auto &field : sirClass->fields_)
                            {
                                VariableInfo fieldInfo;
                                fieldInfo.name = field.first;
                                fieldInfo.type = field.second;
                                fieldInfo.registerName = "%" + field.first;
                                fieldInfo.isGlobal = false;
                                fieldInfo.isMutable = true;
                                fieldInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_main_size");
                                b_.addVariable(fieldInfo);
                            }

                            auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                            sirMethod->addBasicBlock(entryBlock);
                            b_.currentBlock_ = entryBlock;

                            for (const auto &field : sirClass->fields_)
                            {
                                SIRInstruction allocInst;
                                allocInst.opcode = SIROpcode::ALLOC;
                                allocInst.result = SIROperand::Register("%" + field.first, field.second);
                                b_.currentBlock_->addInstruction(allocInst);
                            }

                            // (AR) تسجيل مبدئي في b_.functionTable_ قبل بناء الجسم
                            //      حتى يتمكن buildReturnStatement من تعيين returnClassName
                            // (EN) Pre-register in b_.functionTable_ before building body
                            //      so buildReturnStatement can set returnClassName
                            {
                                FunctionInfo preInfo;
                                preInfo.name = fullMethodName;
                                preInfo.returnType = returnType;
                                preInfo.sirFunction = sirMethod;
                                b_.functionTable_[fullMethodName] = preInfo;
                            }

                            b_.buildStatement(funcDecl->body.get());

                            if (b_.currentBlock_)
                            {
                                bool hasTerminator = false;
                                if (!b_.currentBlock_->instructions.empty())
                                {
                                    auto lastOp = b_.currentBlock_->instructions.back().opcode;
                                    hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                                }
                                if (!hasTerminator)
                                {
                                    SIRInstruction retInst;
                                    if (returnType == SadTypeKind::Void)
                                    {
                                        retInst.opcode = SIROpcode::RET_VOID;
                                    }
                                    else
                                    {
                                        retInst.opcode = SIROpcode::RET;
                                        if (returnType == SadTypeKind::String)
                                        {
                                            retInst.operands.push_back(SIROperand::ConstantString(""));
                                        }
                                        else
                                        {
                                            retInst.operands.push_back(SIROperand::ConstantI64(0));
                                        }
                                    }
                                    b_.currentBlock_->addInstruction(retInst);
                                }
                            }

                            b_.exitScope();

                            b_.module_->addFunction(sirMethod);

                            // (AR) حفظ returnClassName المُعيَّن أثناء بناء جسم الطريقة
                            // (EN) Preserve returnClassName set during method body build
                            std::string savedReturnClassName2;
                            auto prevFtIt2 = b_.functionTable_.find(fullMethodName);
                            if (prevFtIt2 != b_.functionTable_.end())
                            {
                                savedReturnClassName2 = prevFtIt2->second.returnClassName;
                            }

                            FunctionInfo methodInfo;
                            methodInfo.name = fullMethodName;
                            methodInfo.returnType = returnType;
                            methodInfo.parameters = sirMethod->getParameters();
                            methodInfo.sirFunction = sirMethod;
                            methodInfo.returnClassName = savedReturnClassName2;
                            // (AR) [وسم زمن-التشغيل] احفظ نوعَ عنصر المصفوفة المُرجعة الذي
                            //      كتبه buildReturnStatement أثناء بناء الجسم، وإلّا طمسه هذا
                            //      الإسناد (نظير الحفظ في sir_builder_functions للدوالّ الحرّة).
                            // (EN) [runtime tags] preserve the returned array element type
                            //      written by buildReturnStatement during body build; else this
                            //      assignment clobbers it (sibling of the free-function preserve
                            //      in sir_builder_functions).
                            {
                                auto prevMi = b_.functionTable_.find(fullMethodName);
                                if (prevMi != b_.functionTable_.end())
                                    methodInfo.returnElementType = prevMi->second.returnElementType;
                            }
                            b_.functionTable_[fullMethodName] = methodInfo;

                            b_.currentFunction_ = prevFunction;
                            b_.currentBlock_ = prevBlock;
                            b_.currentClassName_ = prevClassName;
                        }
                    }

                    // (AR) تحميل المعاملات الزائد (OperatorDecl) — CW-05: مُستخرج إلى buildClassOperator
                    // (EN) Operator overloading — CW-05: extracted to buildClassOperator
                    else if (auto operatorDecl = dynamic_cast<Sad::AST::OperatorDecl *>(member.get()))
                    {
                        buildClassOperator(classDecl, sirClass, operatorDecl);
                    }
                } // end for (members)

                // (AR) التسجيل تم مبكراً (قبل معالجة الأعضاء) — لا حاجة لتكراره هنا
                // (EN) Registration was done early (before processing members) — no need to repeat here

#ifndef NDEBUG
                std::cout << "[DEBUG] buildClass: class '" << classDecl->name << "' completed with "
                          << sirClass->fields_.size() << " fields and "
                          << sirClass->methods_.size() << " methods" << std::endl;
#endif
            }

            // ============================================================================
            // (AR) buildClassMethod — بناء طريقة صنف/بنية بسياق الصنف الكامل (ISSUE-060)
            //      منطقٌ مطابق لكتلة MethodDecl في buildClass، لكن قابل لإعادة الاستخدام
            //      من مسار البنية (statement_types.cpp) حتى تحصل طرق البنية على ربط self/هذا
            //      وتخطيط الحقول (كانت تُبنى بلا سياق صنف فتقرأ 0).
            // (EN) buildClassMethod — build a class/struct method with full class context.
            //      Mirrors the MethodDecl block in buildClass but reusable from the struct path
            //      so struct methods get self/this binding + field layout (they used to build
            //      with no class context and read fields as 0).
            // ============================================================================
            void ClassBuilder::buildClassMethod(AST::ClassDeclNode *classDecl,
                                                std::shared_ptr<SIRClass> sirClass,
                                                AST::MethodDecl *methodDecl)
            {
                if (!methodDecl || !classDecl || !sirClass)
                    return;
                if (methodDecl->isAbstract)
                    return; // (AR) الدوال المجردة بلا جسم / (EN) abstract methods have no body

                SadTypeKind returnType;
                if (methodDecl->returnType == Types::SadTypeKind::Unknown ||
                    methodDecl->returnType == Types::SadTypeKind::Void)
                {
                    auto savedClassName = b_.currentClassName_;
                    b_.currentClassName_ = classDecl->name;
                    returnType = b_.inferReturnTypeFromBody(methodDecl->body.get());
                    b_.currentClassName_ = savedClassName;
                }
                else
                {
                    returnType = b_.astTypeToSIRType(methodDecl->returnType);
                }
                std::string fullMethodName = classDecl->name + "." + methodDecl->name;
                auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);

                bool isStaticMethod = methodDecl->isStatic;
                if (!isStaticMethod)
                    sirMethod->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                {
                    auto ftIt = b_.functionTable_.find(fullMethodName);
                    if (ftIt != b_.functionTable_.end())
                    {
                        auto &inferredParams = ftIt->second.parameters;
                        for (const auto &param : methodDecl->parameters)
                        {
                            SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                            for (const auto &ip : inferredParams)
                            {
                                if (ip.name == param.name && ip.type != SadTypeKind::Integer)
                                { paramType = ip.type; break; }
                                if (ip.name == param.name && param.type == Types::SadTypeKind::Unknown && ip.type != paramType)
                                { paramType = ip.type; break; }
                            }
                            // (AR) بذر صنف المعامل المصرَّح (جولة أميليا ٢)
                            // (EN) Seed declared param class (Amelia round 2)
                            SIRParameter sp(param.name, paramType);
                            if (!param.typeName.empty() && b_.module_ && b_.module_->getClass(param.typeName))
                                sp.className = param.typeName;
                            sirMethod->addParameter(sp);
                        }
                    }
                    else
                    {
                        for (const auto &param : methodDecl->parameters)
                        {
                            SIRParameter sp(param.name, b_.astTypeToSIRType(param.type));
                            if (!param.typeName.empty() && b_.module_ && b_.module_->getClass(param.typeName))
                                sp.className = param.typeName;
                            sirMethod->addParameter(sp);
                        }
                    }
                }

                sirClass->addMethod(sirMethod);

                if (!methodDecl->body)
                    return;

                auto prevFunction = b_.currentFunction_;
                auto prevBlock = b_.currentBlock_;
                auto prevClassName = b_.currentClassName_;
                b_.currentFunction_ = sirMethod;
                b_.currentClassName_ = classDecl->name;
                b_.enterScope();

                if (!isStaticMethod)
                {
                    VariableInfo selfInfo;
                    selfInfo.name = kSelfParamName;
                    selfInfo.type = SadTypeKind::Integer;
                    selfInfo.registerName = kSelfRegisterName;
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "buildClassMethod_size");
                    b_.addVariable(selfInfo);

                    VariableInfo thisInfo;
                    thisInfo.name = kThisAliasName;
                    thisInfo.type = SadTypeKind::Integer;
                    thisInfo.registerName = kSelfRegisterName;
                    thisInfo.isGlobal = false;
                    thisInfo.isMutable = false;
                    thisInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "buildClassMethod_size");
                    b_.addVariable(thisInfo);
                }

                std::unordered_set<std::string> methodParamNames;
                for (const auto &param : methodDecl->parameters)
                {
                    methodParamNames.insert(param.name);
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = b_.astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.isParameter = true;
                    paramInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "buildClassMethod_size");
                    b_.addVariable(paramInfo);
                }

                if (!isStaticMethod)
                {
                    for (const auto &field : sirClass->fields_)
                    {
                        if (methodParamNames.count(field.first) > 0)
                            continue;
                        VariableInfo fieldInfo;
                        fieldInfo.name = field.first;
                        fieldInfo.type = field.second;
                        fieldInfo.registerName = "%" + field.first;
                        fieldInfo.isGlobal = false;
                        fieldInfo.isMutable = true;
                        fieldInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "buildClassMethod_size");
                        b_.addVariable(fieldInfo);
                    }
                }

                auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                sirMethod->addBasicBlock(entryBlock);
                b_.currentBlock_ = entryBlock;

                if (!isStaticMethod)
                {
                    for (const auto &field : sirClass->fields_)
                    {
                        if (methodParamNames.count(field.first) > 0)
                            continue;
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register("%" + field.first, field.second);
                        b_.currentBlock_->addInstruction(allocInst);
                    }
                }

                {
                    auto existingIt = b_.functionTable_.find(fullMethodName);
                    if (existingIt != b_.functionTable_.end())
                    {
                        existingIt->second.returnType = returnType;
                        existingIt->second.sirFunction = sirMethod;
                    }
                    else
                    {
                        FunctionInfo preInfo;
                        preInfo.name = fullMethodName;
                        preInfo.returnType = returnType;
                        preInfo.sirFunction = sirMethod;
                        b_.functionTable_[fullMethodName] = preInfo;
                    }
                }

                if (isStaticMethod)
                    b_.staticMethods_.insert(fullMethodName);

                b_.buildStatement(methodDecl->body.get());

                if (b_.currentBlock_)
                {
                    bool hasTerminator = false;
                    if (!b_.currentBlock_->instructions.empty())
                    {
                        auto lastOp = b_.currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator)
                    {
                        SIRInstruction retInst;
                        if (returnType == SadTypeKind::Void)
                        {
                            retInst.opcode = SIROpcode::RET_VOID;
                        }
                        else
                        {
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SadTypeKind::String)
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            else
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                        b_.currentBlock_->addInstruction(retInst);
                    }
                }

                b_.exitScope();
                b_.module_->addFunction(sirMethod);

                std::string savedReturnClassName1;
                auto prevFtIt1 = b_.functionTable_.find(fullMethodName);
                if (prevFtIt1 != b_.functionTable_.end())
                    savedReturnClassName1 = prevFtIt1->second.returnClassName;

                FunctionInfo methodInfo;
                methodInfo.name = fullMethodName;
                methodInfo.returnType = returnType;
                methodInfo.parameters = sirMethod->getParameters();
                methodInfo.sirFunction = sirMethod;
                methodInfo.returnClassName = savedReturnClassName1;
                // (AR) [وسم زمن-التشغيل] احفظ نوعَ عنصر المصفوفة المُرجعة (كما أعلاه).
                // (EN) [runtime tags] preserve the returned array element type (as above).
                {
                    auto prevMi = b_.functionTable_.find(fullMethodName);
                    if (prevMi != b_.functionTable_.end())
                        methodInfo.returnElementType = prevMi->second.returnElementType;
                }
                b_.functionTable_[fullMethodName] = methodInfo;

                b_.currentFunction_ = prevFunction;
                b_.currentBlock_ = prevBlock;
                b_.currentClassName_ = prevClassName;
            }

            // ============================================================================
            // (AR) تم نقل buildTrait و buildImpl إلى sir_builder_traits_impl.cpp (CW-05)
            // (EN) buildTrait and buildImpl moved to sir_builder_traits_impl.cpp (CW-05)
            // ============================================================================
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad