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
#include "../../../shared/utils/include/utf8_utils.h"
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
            // - module_->addClass: sir_module.h:608
            // ============================================================================
            void SIRBuilder::buildClass(AST::ClassDeclNode *classDecl)
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
                    auto parentSirClass = module_->getClass(parentClass);
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
                    auto parentSirClass = module_->getClass(parentClass);
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
                //      هذا ضروري لأن buildNewObject داخل جسم عامل (مثل: ارجع جديد متجه(...))
                //      يبحث عن الصنف في module_->getClass() — إذا لم يكن مسجلاً سيفشل.
                //      بما أن sirClass هو shared_ptr، أي تعديلات لاحقة (إضافة حقول/ميثودات)
                //      تنعكس تلقائياً على المرجع المخزن في module_.
                // (EN) Register class early in module BEFORE building method/operator bodies.
                //      This is necessary because buildNewObject inside an operator body
                //      (e.g., return new Vector(...)) searches module_->getClass() — it fails
                //      if class isn't registered yet. Since sirClass is shared_ptr, later
                //      modifications (adding fields/methods) auto-reflect in module_.
                // ═══════════════════════════════════════════════════════════════════════
                module_->addClass(sirClass);

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
                        SadTypeKind fieldType = astTypeToSIRType(fieldDecl->type);

                        // (AR) الحقول الديناميكية (UNKNOWN/OBJECT): استنتاج النوع من المُهيئ أولاً
                        // (EN) Dynamic fields (UNKNOWN/OBJECT): infer type from initializer first
                        if (fieldDecl->type == Data::DataType::UNKNOWN ||
                            fieldDecl->type == Data::DataType::OBJECT)
                        {
                            fieldType = SadTypeKind::Pointer; // الافتراضي / default

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
                            addVariable(globalVar);

                            if (module_)
                            {
                                std::string initVal = "0";
                                if (fieldDecl->initializer)
                                {
                                    auto initResult = buildExpression(fieldDecl->initializer.get());
                                    if (initResult.isConstant && !initResult.constantValue.empty())
                                    {
                                        initVal = initResult.constantValue;
                                    }
                                }
                                auto gv = std::make_shared<SIRGlobalVariable>(
                                    globalName, fieldType, initVal, false);
                                module_->addGlobalVariable(gv);
                            }

                            staticFields_[globalName] = fieldType;
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
                        if (methodDecl->returnType == Data::DataType::UNKNOWN ||
                            methodDecl->returnType == Data::DataType::NONE)
                        {
                            // (AR) تعيين اسم الصنف الحالي مؤقتاً لتمكين inferReturnTypeFromBody
                            //      من البحث عن أنواع الحقول عبر module_->getClass(currentClassName_)
                            //      عند مطابقة MemberExpr (هذا.حقل)
                            // (EN) Temporarily set currentClassName_ so inferReturnTypeFromBody
                            //      can look up field types via module_->getClass(currentClassName_)
                            //      when matching MemberExpr (this.field)
                            auto savedClassName = currentClassName_;
                            currentClassName_ = classDecl->name;
                            returnType = inferReturnTypeFromBody(methodDecl->body.get());
                            currentClassName_ = savedClassName;
                        }
                        else
                        {
                            returnType = astTypeToSIRType(methodDecl->returnType);
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
                        // (AR) استخدام الأنواع المُستنتجة من functionTable_ (المرحلة 1.7)
                        //      إذا كانت الدالة سُجلت مسبقاً في المرحلة 1.3 وتم تحديث أنواعها
                        //      نستخدم الأنواع المحدثة بدلاً من أنواع AST الافتراضية
                        // (EN) Use inferred types from functionTable_ (Phase 1.7)
                        //      If the method was pre-registered in Phase 1.3 and types were updated
                        //      use the updated types instead of default AST types
                        // ================================================================
                        {
                            auto ftIt = functionTable_.find(fullMethodName);
                            if (ftIt != functionTable_.end())
                            {
                                auto &inferredParams = ftIt->second.parameters;
                                for (const auto &param : methodDecl->parameters)
                                {
                                    SadTypeKind paramType = astTypeToSIRType(param.type);
                                    // (AR) ابحث عن النوع المُستنتج للمعامل
                                    for (const auto &ip : inferredParams)
                                    {
                                        if (ip.name == param.name && ip.type != SadTypeKind::Integer)
                                        {
                                            paramType = ip.type;
                                            break;
                                        }
                                        // (AR) إذا كان النوع الأصلي UNKNOWN (= Integer) والمُستنتج أكثر تحديداً
                                        if (ip.name == param.name && param.type == Data::DataType::UNKNOWN && ip.type != paramType)
                                        {
                                            paramType = ip.type;
                                            break;
                                        }
                                    }
                                    sirMethod->addParameter(SIRParameter(param.name, paramType));
                                }
                            }
                            else
                            {
                                for (const auto &param : methodDecl->parameters)
                                {
                                    SadTypeKind paramType = astTypeToSIRType(param.type);
                                    sirMethod->addParameter(SIRParameter(param.name, paramType));
                                }
                            }
                        }

                        sirClass->addMethod(sirMethod);

                        // (AR) بناء جسم الطريقة
                        // (EN) Build method body
                        if (methodDecl->body)
                        {
                            auto prevFunction = currentFunction_;
                            auto prevBlock = currentBlock_;
                            auto prevClassName = currentClassName_;

                            currentFunction_ = sirMethod;
                            currentClassName_ = classDecl->name;

                            enterScope();

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
                                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                    addVariable(selfInfo);
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
                                    thisInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                    addVariable(thisInfo);
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
                                paramInfo.type = astTypeToSIRType(param.type);
                                paramInfo.registerName = "%" + param.name;
                                paramInfo.isGlobal = false;
                                paramInfo.isMutable = false;
                                paramInfo.isParameter = true;
                                paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                addVariable(paramInfo);
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
                                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                    addVariable(fieldInfo);
                                }
                            }

                            auto entryBlock = createBasicBlock(kEntryBlockName);
                            sirMethod->addBasicBlock(entryBlock);
                            currentBlock_ = entryBlock;

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
                                    currentBlock_->addInstruction(allocInst);
                                }
                            }

                            // (AR) تسجيل/تحديث في functionTable_ قبل بناء الجسم
                            //      نحافظ على الأنواع المُستنتجة من المرحلة 1.7
                            //      ونُحدّث فقط returnType و sirFunction
                            // (EN) Register/update in functionTable_ before building body
                            //      Preserve inferred types from Phase 1.7
                            //      Only update returnType and sirFunction
                            {
                                auto existingIt = functionTable_.find(fullMethodName);
                                if (existingIt != functionTable_.end())
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
                                    functionTable_[fullMethodName] = preInfo;
                                }
                            }

                            // (AR) تسجيل الدالة الساكنة
                            // (EN) Register static method
                            if (isStaticMethod)
                            {
                                staticMethods_.insert(fullMethodName);
                            }

                            buildStatement(methodDecl->body.get());

                            // (AR) إضافة terminator
                            // (EN) Add terminator
                            if (currentBlock_)
                            {
                                bool hasTerminator = false;
                                if (!currentBlock_->instructions.empty())
                                {
                                    auto lastOp = currentBlock_->instructions.back().opcode;
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
                                    currentBlock_->addInstruction(retInst);
                                }
                            }

                            exitScope();

                            module_->addFunction(sirMethod);

                            // (AR) حفظ returnClassName المُعيَّن أثناء بناء جسم الطريقة
                            // (EN) Preserve returnClassName set during method body build
                            std::string savedReturnClassName1;
                            auto prevFtIt1 = functionTable_.find(fullMethodName);
                            if (prevFtIt1 != functionTable_.end())
                            {
                                savedReturnClassName1 = prevFtIt1->second.returnClassName;
                            }

                            FunctionInfo methodInfo;
                            methodInfo.name = fullMethodName;
                            methodInfo.returnType = returnType;
                            methodInfo.parameters = sirMethod->getParameters();
                            methodInfo.sirFunction = sirMethod;
                            methodInfo.returnClassName = savedReturnClassName1;
                            functionTable_[fullMethodName] = methodInfo;

                            currentFunction_ = prevFunction;
                            currentBlock_ = prevBlock;
                            currentClassName_ = prevClassName;
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
                        if (funcDecl->returnType == Data::DataType::UNKNOWN ||
                            funcDecl->returnType == Data::DataType::NONE)
                        {
                            auto savedClassName = currentClassName_;
                            currentClassName_ = classDecl->name;
                            returnType = inferReturnTypeFromBody(funcDecl->body.get());
                            currentClassName_ = savedClassName;
                        }
                        else
                        {
                            returnType = astTypeToSIRType(funcDecl->returnType);
                        }
                        std::string fullMethodName = classDecl->name + "." + funcDecl->name;
                        auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);

                        sirMethod->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        for (const auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            sirMethod->addParameter(SIRParameter(param.name, paramType));
                        }

                        sirClass->addMethod(sirMethod);

                        // (AR) بناء جسم الدالة (مثل الطريقة تماماً)
                        // (EN) Build function body (same as method)
                        if (funcDecl->body)
                        {
                            auto prevFunction = currentFunction_;
                            auto prevBlock = currentBlock_;
                            auto prevClassName = currentClassName_;

                            currentFunction_ = sirMethod;
                            currentClassName_ = classDecl->name;

                            enterScope();

                            {
                                VariableInfo selfInfo;
                                selfInfo.name = kSelfParamName;
                                selfInfo.type = SadTypeKind::Integer;
                                selfInfo.registerName = kSelfRegisterName;
                                selfInfo.isGlobal = false;
                                selfInfo.isMutable = false;
                                selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                addVariable(selfInfo);
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
                                thisInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                addVariable(thisInfo);
                            }

                            for (const auto &param : funcDecl->parameters)
                            {
                                VariableInfo paramInfo;
                                paramInfo.name = param.name;
                                paramInfo.type = astTypeToSIRType(param.type);
                                paramInfo.registerName = "%" + param.name;
                                paramInfo.isGlobal = false;
                                paramInfo.isMutable = false;
                                paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                addVariable(paramInfo);
                            }

                            for (const auto &field : sirClass->fields_)
                            {
                                VariableInfo fieldInfo;
                                fieldInfo.name = field.first;
                                fieldInfo.type = field.second;
                                fieldInfo.registerName = "%" + field.first;
                                fieldInfo.isGlobal = false;
                                fieldInfo.isMutable = true;
                                fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                                addVariable(fieldInfo);
                            }

                            auto entryBlock = createBasicBlock(kEntryBlockName);
                            sirMethod->addBasicBlock(entryBlock);
                            currentBlock_ = entryBlock;

                            for (const auto &field : sirClass->fields_)
                            {
                                SIRInstruction allocInst;
                                allocInst.opcode = SIROpcode::ALLOC;
                                allocInst.result = SIROperand::Register("%" + field.first, field.second);
                                currentBlock_->addInstruction(allocInst);
                            }

                            // (AR) تسجيل مبدئي في functionTable_ قبل بناء الجسم
                            //      حتى يتمكن buildReturnStatement من تعيين returnClassName
                            // (EN) Pre-register in functionTable_ before building body
                            //      so buildReturnStatement can set returnClassName
                            {
                                FunctionInfo preInfo;
                                preInfo.name = fullMethodName;
                                preInfo.returnType = returnType;
                                preInfo.sirFunction = sirMethod;
                                functionTable_[fullMethodName] = preInfo;
                            }

                            buildStatement(funcDecl->body.get());

                            if (currentBlock_)
                            {
                                bool hasTerminator = false;
                                if (!currentBlock_->instructions.empty())
                                {
                                    auto lastOp = currentBlock_->instructions.back().opcode;
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
                                    currentBlock_->addInstruction(retInst);
                                }
                            }

                            exitScope();

                            module_->addFunction(sirMethod);

                            // (AR) حفظ returnClassName المُعيَّن أثناء بناء جسم الطريقة
                            // (EN) Preserve returnClassName set during method body build
                            std::string savedReturnClassName2;
                            auto prevFtIt2 = functionTable_.find(fullMethodName);
                            if (prevFtIt2 != functionTable_.end())
                            {
                                savedReturnClassName2 = prevFtIt2->second.returnClassName;
                            }

                            FunctionInfo methodInfo;
                            methodInfo.name = fullMethodName;
                            methodInfo.returnType = returnType;
                            methodInfo.parameters = sirMethod->getParameters();
                            methodInfo.sirFunction = sirMethod;
                            methodInfo.returnClassName = savedReturnClassName2;
                            functionTable_[fullMethodName] = methodInfo;

                            currentFunction_ = prevFunction;
                            currentBlock_ = prevBlock;
                            currentClassName_ = prevClassName;
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
            // (AR) تم نقل buildTrait و buildImpl إلى sir_builder_traits_impl.cpp (CW-05)
            // (EN) buildTrait and buildImpl moved to sir_builder_traits_impl.cpp (CW-05)
            // ============================================================================
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad