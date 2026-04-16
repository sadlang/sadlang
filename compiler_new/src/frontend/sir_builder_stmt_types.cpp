// ============================================================================
// sir_builder_stmt_types.cpp
// (AR) Type declarations: enum, struct, namespace, class, destructor
// (EN) Type declaration statements
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
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool SIRBuilder::buildStatement_Types(AST::Statement *stmt)
            {
                // ========================================================================
                // (AR) EnumDecl - تصريح تعداد بسيط أو جبري (ADT)
                //      تعداد بسيط: تعداد اللون { أحمر، أخضر، أزرق }
                //      تعداد جبري: تعداد شكل { دائرة(نصف_القطر)، مستطيل(عرض، ارتفاع)، نقطة }
                // (EN) Enum declaration - simple or ADT (Algebraic Data Type)
                //      Simple: enum Color { Red, Green, Blue }
                //      ADT:    enum Shape { Circle(radius), Rectangle(w, h), Point }
                // ========================================================================
                if (auto enumDecl = dynamic_cast<Sad::AST::EnumDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found EnumDecl: " << enumDecl->name
                              << " (isADT=" << enumDecl->isADT() << ")" << std::endl;
#endif

                    if (enumDecl->isADT())
                    {
                        // ==============================================================
                        // (AR) === مسار التعداد الجبري (ADT) ===
                        //      يُمثَّل كـ tagged union: بنية { __tag: I64, __f0: PTR, __f1: PTR, ... }
                        //      لكل حالة: دالة باني تُنشئ البنية مع المميّز الصحيح
                        //      الحالات بدون بيانات (Unit): ثابت عام يحمل البنية مع المميّز فقط
                        // (EN) === ADT Enum Path ===
                        //      Represented as tagged union: struct { __tag: I64, __f0: PTR, __f1: PTR, ... }
                        //      For each variant: constructor function that creates struct with correct tag
                        //      Unit variants: global constant with tag only
                        // ==============================================================

                        // (AR) الخطوة 1: جمع معلومات الحالات وحساب أكبر عدد حقول
                        // (EN) Step 1: Collect variant info and calculate max field count
                        ADTEnumInfo adtInfo;
                        adtInfo.name = enumDecl->name;
                        adtInfo.structName = enumDecl->name;

                        for (size_t i = 0; i < enumDecl->members.size(); ++i)
                        {
                            const auto &member = enumDecl->members[i];
                            ADTVariantInfo variant;
                            variant.name = member.name;
                            variant.tag = static_cast<int64_t>(i);
                            variant.fields = member.fields;

                            if (variant.fields.size() > adtInfo.maxFieldCount)
                            {
                                adtInfo.maxFieldCount = variant.fields.size();
                            }

                            adtInfo.variants.push_back(std::move(variant));
                        }

#ifndef NDEBUG
                        std::cout << "[DEBUG] ADT enum '" << enumDecl->name
                                  << "': " << adtInfo.variants.size() << " variants, maxFields="
                                  << adtInfo.maxFieldCount << std::endl;
#endif

                        // (AR) الخطوة 2: إنشاء بنية SIR للـ tagged union
                        //      البنية: { __tag: I64, __f0: PTR, __f1: PTR, ..., __fN: PTR }
                        //      حيث N = maxFieldCount - 1
                        //      نستخدم PTR لكل حقل لأن الحقول قد تحمل أي نوع
                        // (EN) Step 2: Create SIR struct for tagged union
                        //      Struct: { __tag: I64, __f0: PTR, __f1: PTR, ..., __fN: PTR }
                        //      where N = maxFieldCount - 1
                        //      We use PTR for all fields since they can hold any type
                        auto sirClass = std::make_shared<SIRClass>(adtInfo.structName, "");
                        sirClass->addField("__tag", SadTypeKind::Integer);
                        for (size_t f = 0; f < adtInfo.maxFieldCount; ++f)
                        {
                            sirClass->addField("__f" + std::to_string(f), SadTypeKind::Pointer);
                        }

                        if (module_)
                        {
                            module_->addClass(sirClass);
                        }
                        classTable_[adtInfo.structName] = sirClass;

                        // (AR) الخطوة 3: لكل حالة، إنشاء دالة باني أو ثابت عام
                        // (EN) Step 3: For each variant, create constructor function or global constant
                        for (const auto &variant : adtInfo.variants)
                        {
                            std::string fullName = enumDecl->name + "." + variant.name;

                            if (variant.isUnit())
                            {
                                // (AR) حالة بدون بيانات (Unit variant): ثابت i64 يحمل المميّز
                                //      هذا يحافظ على التوافق مع التعدادات البسيطة
                                //      ويُسجَّل أيضاً كباني بدون معاملات لدعم match
                                // (EN) Unit variant: i64 constant holding the discriminant
                                //      This maintains compatibility with simple enums
                                //      Also registered as zero-arg constructor for match support

                                if (module_)
                                {
                                    auto global = std::make_shared<SIRGlobalVariable>(
                                        fullName, SadTypeKind::Integer, std::to_string(variant.tag), true);
                                    module_->addGlobalVariable(global);
                                }

                                VariableInfo enumVar;
                                enumVar.name = fullName;
                                enumVar.type = SadTypeKind::Integer;
                                enumVar.registerName = "%" + fullName;
                                enumVar.isGlobal = true;
                                enumVar.isMutable = false;
                                enumVar.scopeLevel = 0;
                                addVariable(enumVar);

                                // (AR) أيضاً: تسجيل دالة باني بدون معاملات
                                //      حتى يمكن استدعاء شكل.نقطة() كدالة أو استخدامها في match
                                // (EN) Also: register zero-arg constructor function
                                //      So Shape.Point() works as function and in match patterns
                                std::string ctorName = "__adt_ctor_" + enumDecl->name + "_" + variant.name;

                                auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SadTypeKind::Struct);
                                auto entryBlock = createBasicBlock(ctorName + ".entry");
                                ctorFunc->addBasicBlock(entryBlock);

                                // (AR) جسم الباني: ENUM_CONSTRUCT بدون حقول
                                // (EN) Constructor body: ENUM_CONSTRUCT with no fields
                                std::string resultReg = "%" + ctorName + ".result";
                                SIRInstruction constructInst(SIROpcode::ENUM_CONSTRUCT);
                                constructInst.result = SIROperand::Register(resultReg, SadTypeKind::Struct);
                                constructInst.operands.push_back(SIROperand::ConstantString(enumDecl->name));
                                constructInst.operands.push_back(SIROperand::ConstantI64(variant.tag));
                                constructInst.comment = "Unit variant: " + fullName;
                                entryBlock->addInstruction(constructInst);

                                // (AR) إرجاع البنية
                                // (EN) Return the struct
                                SIRInstruction retInst(SIROpcode::RET);
                                retInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Struct));
                                entryBlock->addInstruction(retInst);

                                if (module_)
                                {
                                    module_->addFunction(ctorFunc);
                                }

                                // (AR) تسجيل الباني في جدول الدوال
                                // (EN) Register constructor in function table
                                FunctionInfo ctorInfo;
                                ctorInfo.name = ctorName;
                                ctorInfo.returnType = SadTypeKind::Struct;
                                ctorInfo.returnClassName = adtInfo.structName;
                                ctorInfo.sirFunction = ctorFunc;
                                functionTable_[fullName] = ctorInfo;
                            }
                            else
                            {
                                // (AR) حالة مع بيانات (Tuple variant): دالة باني تأخذ الحقول كمعاملات
                                //      مثال: شكل.دائرة(نصف_القطر) → __adt_ctor_شكل_دائرة(نصف_القطر)
                                //      الدالة تنشئ بنية { __tag=0, __f0=نصف_القطر }
                                // (EN) Data variant: constructor function that takes fields as parameters
                                //      Example: Shape.Circle(radius) → __adt_ctor_Shape_Circle(radius)
                                //      Function creates struct { __tag=0, __f0=radius }

                                std::string ctorName = "__adt_ctor_" + enumDecl->name + "_" + variant.name;

                                // (AR) إنشاء دالة SIR مع المعاملات
                                // (EN) Create SIR function with parameters
                                auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SadTypeKind::Struct);
                                for (size_t fi = 0; fi < variant.fields.size(); ++fi)
                                {
                                    SIRParameter param(variant.fields[fi], SadTypeKind::Pointer);
                                    ctorFunc->addParameter(param);
                                }

                                auto entryBlock = createBasicBlock(ctorName + ".entry");
                                ctorFunc->addBasicBlock(entryBlock);

                                // (AR) جسم الباني: ENUM_CONSTRUCT مع الحقول
                                // (EN) Constructor body: ENUM_CONSTRUCT with fields
                                std::string resultReg = "%" + ctorName + ".result";
                                SIRInstruction constructInst(SIROpcode::ENUM_CONSTRUCT);
                                constructInst.result = SIROperand::Register(resultReg, SadTypeKind::Struct);
                                constructInst.operands.push_back(SIROperand::ConstantString(enumDecl->name));
                                constructInst.operands.push_back(SIROperand::ConstantI64(variant.tag));

                                // (AR) إضافة معاملات الباني كمعاملات للتعليمة
                                // (EN) Add constructor parameters as instruction operands
                                for (size_t fi = 0; fi < variant.fields.size(); ++fi)
                                {
                                    std::string paramReg = "%" + variant.fields[fi];
                                    constructInst.operands.push_back(
                                        SIROperand::Register(paramReg, SadTypeKind::Pointer));
                                }
                                constructInst.comment = "Tuple variant: " + fullName + "(" +
                                                        std::to_string(variant.fields.size()) + " fields)";
                                entryBlock->addInstruction(constructInst);

                                // (AR) إرجاع البنية
                                // (EN) Return the struct
                                SIRInstruction retInst(SIROpcode::RET);
                                retInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Struct));
                                entryBlock->addInstruction(retInst);

                                if (module_)
                                {
                                    module_->addFunction(ctorFunc);
                                }

                                // (AR) تسجيل الباني في جدول الدوال
                                // (EN) Register constructor in function table
                                FunctionInfo ctorInfo;
                                ctorInfo.name = ctorName;
                                ctorInfo.returnType = SadTypeKind::Struct;
                                ctorInfo.returnClassName = adtInfo.structName;
                                ctorInfo.sirFunction = ctorFunc;
                                for (size_t fi = 0; fi < variant.fields.size(); ++fi)
                                {
                                    ctorInfo.parameters.push_back(SIRParameter(variant.fields[fi], SadTypeKind::Pointer));
                                }
                                functionTable_[fullName] = ctorInfo;
                            }
                        }

                        // (AR) الخطوة 4: تسجيل معلومات ADT في الجدول العام
                        // (EN) Step 4: Register ADT info in global table
                        adtEnumTable_[enumDecl->name] = std::move(adtInfo);

#ifndef NDEBUG
                        std::cout << "[DEBUG] ADT enum '" << enumDecl->name
                                  << "' fully registered with " << enumDecl->members.size()
                                  << " variant constructors" << std::endl;
#endif
                    }
                    else
                    {
                        // ==============================================================
                        // (AR) === مسار التعداد البسيط (C-style) ===
                        //      كل عضو → ثابت i64 عام
                        //      تعداد اللون { أحمر، أخضر، أزرق }
                        //      → اللون.أحمر = 0, اللون.أخضر = 1, اللون.أزرق = 2
                        // (EN) === Simple Enum Path (C-style) ===
                        //      Each member → i64 global constant
                        //      enum Color { Red, Green, Blue }
                        //      → Color.Red = 0, Color.Green = 1, Color.Blue = 2
                        // ==============================================================

                        for (size_t i = 0; i < enumDecl->members.size(); ++i)
                        {
                            const auto &member = enumDecl->members[i];
                            std::string fullName = enumDecl->name + "." + member.name;

                            // (AR) تحديد القيمة: إما صريحة أو تلقائية
                            // (EN) Determine value: explicit or automatic
                            int64_t enumValue = static_cast<int64_t>(i);
                            if (member.value)
                            {
                                auto valResult = buildExpression(member.value.get());
                                if (valResult.isConstant)
                                {
                                    try
                                    {
                                        enumValue = std::stoll(valResult.constantValue);
                                    }
                                    catch (const std::exception &)
                                    {
                                        // (AR) فشل التحويل: استخدام القيمة الافتراضية
                                        // (EN) Conversion failed: use default value
                                    }
                                }
                            }

                            // (AR) إنشاء متغير عام للثابت
                            // (EN) Create global variable for constant
                            std::string regName = "%" + fullName;

                            if (module_)
                            {
                                auto global = std::make_shared<SIRGlobalVariable>(
                                    fullName, SadTypeKind::Integer, std::to_string(enumValue), true);
                                module_->addGlobalVariable(global);
                            }

                            // (AR) تسجيل كثابت في النطاق
                            // (EN) Register as constant in scope
                            VariableInfo enumVar;
                            enumVar.name = fullName;
                            enumVar.type = SadTypeKind::Integer;
                            enumVar.registerName = regName;
                            enumVar.isGlobal = true;
                            enumVar.isMutable = false;
                            enumVar.scopeLevel = 0;
                            addVariable(enumVar);
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) StructDecl - تصريح بنية: بنية نقطة { عشري س، عشري ي }
                // (EN) Struct declaration
                // ========================================================================
                if (auto structDecl = dynamic_cast<Sad::AST::StructDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found StructDecl: " << structDecl->name << std::endl;
#endif

                    // (AR) إنشاء "صنف" SIR بدون وراثة لتمثيل البنية
                    // (EN) Create SIR "class" without inheritance to represent struct
                    auto sirClass = std::make_shared<SIRClass>(structDecl->name, "");

                    // (AR) إضافة الحقول
                    // (EN) Add fields
                    for (const auto &field : structDecl->fields)
                    {
                        SadTypeKind fieldType = SadTypeKind::Integer;
                        switch (field.type)
                        {
                        case Sad::Data::DataType::INTEGER:
                            fieldType = SadTypeKind::Integer;
                            break;
                        case Sad::Data::DataType::FLOAT:
                            fieldType = SadTypeKind::Float;
                            break;
                        case Sad::Data::DataType::BOOLEAN:
                            fieldType = SadTypeKind::Boolean;
                            break;
                        case Sad::Data::DataType::STRING:
                            fieldType = SadTypeKind::String;
                            break;
                        case Sad::Data::DataType::ARRAY:
                            fieldType = SadTypeKind::Array;
                            break;
                        default:
                            fieldType = SadTypeKind::Integer;
                            break;
                        }
                        sirClass->addField(field.name, fieldType);
                    }

                    // (AR) إضافة الدوال المنفذة (إن وجدت)
                    // (EN) Add implemented methods (if any)
                    for (auto &methodStmt : structDecl->methods)
                    {
                        buildStatement(methodStmt.get());
                    }

                    if (module_)
                    {
                        module_->addClass(sirClass);
                    }

                    // (AR) تسجيل الصنف في الجدول
                    // (EN) Register class in table
                    classTable_[structDecl->name] = sirClass;
                    return true;
                }

                // ========================================================================
                // (AR) NamespaceDecl - فضاء أسماء: فضاء رياضيات ... نهاية_فضاء
                // (EN) Namespace declaration
                // ========================================================================
                if (auto nsDecl = dynamic_cast<Sad::AST::NamespaceDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found NamespaceDecl: " << nsDecl->name << std::endl;
#endif

                    // (AR) بناء أعضاء الفضاء (الدوال، الأصناف، المتغيرات)
                    // (EN) Build namespace members (functions, classes, variables)
                    enterScope();

                    for (const auto &member : nsDecl->members)
                    {
                        if (!member)
                            continue;

                        // (AR) محاولة بناء كتعريف عام أو جملة
                        // (EN) Try to build as top-level declaration or statement
                        auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(member.get());
                        auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(member.get());
                        auto classDecl = dynamic_cast<Sad::AST::ClassDecl *>(member.get());

                        if (funcDecl)
                        {
                            // (AR) بناء الدالة مع بادئة الفضاء
                            // (EN) Build function with namespace prefix
                            std::string originalName = funcDecl->name;
                            funcDecl->name = nsDecl->name + "::" + originalName;
                            buildFunction(funcDecl);
                            funcDecl->name = originalName;
                        }
                        else if (varDecl)
                        {
                            buildGlobalVariable(varDecl);
                        }
                        else if (classDecl)
                        {
                            std::string originalName = classDecl->name;
                            classDecl->name = nsDecl->name + "::" + originalName;
                            buildClass(classDecl);
                            classDecl->name = originalName;
                        }
                        else
                        {
                            buildStatement(member.get());
                        }
                    }

                    exitScope();
                    return true;
                }

                // ========================================================================
                // (AR) ClassDeclStmt - تصريح صنف (الصيغة البديلة)
                // (EN) ClassDeclStmt - alternate class declaration syntax
                // ========================================================================
                if (auto classDeclStmt = dynamic_cast<Sad::AST::ClassDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ClassDeclStmt: " << classDeclStmt->name << std::endl;
#endif

                    // (AR) تحويل ClassDeclStmt إلى SIRClass
                    // (EN) Convert ClassDeclStmt to SIRClass
                    std::string parentClass;
                    if (!classDeclStmt->baseClasses.empty())
                    {
                        parentClass = classDeclStmt->baseClasses[0];
                    }

                    auto sirClass = std::make_shared<SIRClass>(classDeclStmt->name, parentClass);

                    // (AR) معالجة الحقول
                    // (EN) Process fields
                    for (const auto &field : classDeclStmt->fields)
                    {
                        SadTypeKind fieldType = SadTypeKind::Integer;

                        // (AR) الحقول الساكنة: تُنشأ كمتغيرات عامة بدلاً من حقول نسخة
                        // (EN) Static fields: created as global variables instead of instance fields
                        if (field->isStatic)
                        {
                            std::string globalName = classDeclStmt->name + "." + field->name;
                            VariableInfo globalVar;
                            globalVar.name = globalName;
                            globalVar.type = fieldType;
                            globalVar.registerName = "@" + globalName;
                            globalVar.isGlobal = true;
                            addVariable(globalVar);

                            // (AR) تعيين القيمة الابتدائية
                            // (EN) Set initial value
                            if (module_)
                            {
                                std::string initVal = "0";
                                if (field->initializer)
                                {
                                    auto initResult = buildExpression(field->initializer.get());
                                    if (initResult.isConstant && !initResult.constantValue.empty())
                                    {
                                        initVal = initResult.constantValue;
                                    }
                                }
                                auto globalVar = std::make_shared<SIRGlobalVariable>(
                                    globalName, fieldType, initVal, false);
                                module_->addGlobalVariable(globalVar);
                            }

                            // (AR) تسجيل الحقل الساكن لاكتشافه لاحقاً
                            // (EN) Register static field for later detection
                            staticFields_[globalName] = fieldType;
                            continue;
                        }

                        sirClass->addField(field->name, fieldType);
                    }

                    // (AR) حفظ السياق
                    // (EN) Save context
                    std::string savedClassName = currentClassName_;
                    currentClassName_ = classDeclStmt->name;

                    // (AR) معالجة الباني
                    // (EN) Process constructor
                    if (classDeclStmt->constructor)
                    {
                        auto ctorDecl = classDeclStmt->constructor.get();
                        std::string ctorName = classDeclStmt->name + "::باني";

                        std::vector<SIRParameter> ctorParams;
                        ctorParams.push_back(SIRParameter(kSelfParamName, SadTypeKind::Pointer));
                        if (auto ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(ctorDecl))
                        {
                            for (const auto &p : ctor->parameters)
                            {
                                ctorParams.push_back(SIRParameter(p.name, SadTypeKind::Integer));
                            }
                        }

                        auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SadTypeKind::Void);
                        for (const auto &cp : ctorParams)
                            ctorFunc->addParameter(cp);
                        auto savedCtxCtor = saveContext();

                        currentFunction_ = ctorFunc;
                        auto entryBlock = createBasicBlock("ctor_entry");
                        ctorFunc->addBasicBlock(entryBlock);
                        currentBlock_ = entryBlock;

                        enterScope();
                        VariableInfo selfVar;
                        selfVar.name = kSelfParamName;
                        selfVar.type = SadTypeKind::Pointer;
                        selfVar.registerName = kSelfRegisterName;
                        addVariable(selfVar);

                        if (auto ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(ctorDecl))
                        {
                            if (ctor->body)
                                buildStatement(ctor->body.get());
                        }

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                        }
                        exitScope();

                        if (module_)
                            module_->addFunction(ctorFunc);
                        sirClass->addMethod(ctorFunc);

                        restoreContext(std::move(savedCtxCtor));
                    }

                    // (AR) معالجة الدوال
                    // (EN) Process methods
                    for (const auto &method : classDeclStmt->methods)
                    {
                        if (auto methodDecl = dynamic_cast<Sad::AST::MethodDecl *>(method.get()))
                        {
                            bool isStaticMethod = methodDecl->isStatic;
                            std::string methodName = classDeclStmt->name + "::" + methodDecl->name;

                            std::vector<SIRParameter> methodParams;
                            // (AR) الدوال الساكنة لا تأخذ self
                            // (EN) Static methods don't take self
                            if (!isStaticMethod)
                            {
                                methodParams.push_back(SIRParameter(kSelfParamName, SadTypeKind::Pointer));
                            }
                            for (const auto &p : methodDecl->parameters)
                            {
                                methodParams.push_back(SIRParameter(p.name, SadTypeKind::Integer));
                            }

                            auto methodFunc = std::make_shared<SIRFunction>(methodName, SadTypeKind::Integer);
                            for (const auto &mp : methodParams)
                                methodFunc->addParameter(mp);
                            auto savedCtxMethod = saveContext();

                            currentFunction_ = methodFunc;
                            auto entryBlock = createBasicBlock("method_entry");
                            methodFunc->addBasicBlock(entryBlock);
                            currentBlock_ = entryBlock;

                            enterScope();

                            // (AR) الدوال الساكنة لا تحتاج self في النطاق
                            // (EN) Static methods don't need self in scope
                            if (!isStaticMethod)
                            {
                                VariableInfo selfVar;
                                selfVar.name = kSelfParamName;
                                selfVar.type = SadTypeKind::Pointer;
                                selfVar.registerName = kSelfRegisterName;
                                addVariable(selfVar);
                            }

                            for (const auto &p : methodDecl->parameters)
                            {
                                VariableInfo pVar;
                                pVar.name = p.name;
                                pVar.type = SadTypeKind::Integer;
                                pVar.registerName = "%" + p.name;
                                addVariable(pVar);
                            }

                            if (methodDecl->body)
                                buildStatement(methodDecl->body.get());
                            exitScope();

                            if (module_)
                                module_->addFunction(methodFunc);
                            sirClass->addMethod(methodFunc);

                            // (AR) تسجيل الدالة في functionTable_ بصيغة النقطة (للبحث الذكي)
                            // (EN) Register method in functionTable_ with dot notation (for smart lookup)
                            std::string dotName = classDeclStmt->name + "." + methodDecl->name;
                            FunctionInfo fInfo;
                            fInfo.name = methodName;
                            fInfo.returnType = SadTypeKind::Integer;
                            functionTable_[dotName] = fInfo;

                            // (AR) تسجيل الدالة الساكنة لتمييزها عند الاستدعاء
                            // (EN) Register static method for identification during calls
                            if (isStaticMethod)
                            {
                                staticMethods_.insert(dotName);
                            }

                            restoreContext(std::move(savedCtxMethod));
                        }
                    }

                    currentClassName_ = savedClassName;

                    if (module_)
                        module_->addClass(sirClass);
                    classTable_[classDeclStmt->name] = sirClass;
                    return true;
                }

                // ========================================================================
                // (AR) FunctionDecl داخل جسم دالة (دوال متداخلة)
                // (EN) FunctionDecl inside function body (nested function)
                // ========================================================================
                if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found nested FunctionDecl: " << funcDecl->name << std::endl;
#endif
                    buildFunction(funcDecl);
                    return true;
                }

                // ========================================================================
                // (AR) ClassDecl داخل جسم (صنف متداخل)
                // (EN) ClassDecl inside body (nested class)
                // ========================================================================
                if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found nested ClassDecl: " << classDecl->name << std::endl;
#endif
                    buildClass(classDecl);
                    return true;
                }

                // ========================================================================
                // (AR) DestructorDecl - هادم الصنف (declarations.h:DestructorDecl)
                // (EN) Destructor declaration — lowered to a special __destructor function
                // ========================================================================
                if (auto destructorDecl = dynamic_cast<Sad::AST::DestructorDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found DestructorDecl" << std::endl;
#endif

                    // (AR) بناء دالة هادم خاصة: __destructor_<className>
                    // (EN) Build special destructor function: __destructor_<className>
                    std::string dtorName = "__destructor";
                    auto dtorFunc = std::make_shared<SIRFunction>(dtorName, SadTypeKind::Void);

                    // (AR) إضافة معامل this
                    // (EN) Add 'this' parameter
                    dtorFunc->addParameter(SIRParameter("this", SadTypeKind::Pointer));

                    auto entryBlock = createBasicBlock("destructor_entry");
                    dtorFunc->addBasicBlock(entryBlock);

                    auto savedCtxDtor = saveContext();
                    currentFunction_ = dtorFunc;
                    currentBlock_ = entryBlock;

                    enterScope();
                    if (destructorDecl->body)
                    {
                        buildStatement(destructorDecl->body.get());
                    }
                    exitScope();

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                    }

                    restoreContext(std::move(savedCtxDtor));
                    module_->addFunction(dtorFunc);
                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad