// ============================================================================
// sir_builder_calls_objects.cpp - بناء تعابير الكائنات (إنشاء الكائنات والوصول للأعضاء)
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// الوصف / Description:
//   هذا الملف يحتوي دوال بناء تعليمات SIR الخاصة بالكائنات والأصناف:
//   - buildNewObject: إنشاء كائن جديد (جديد اسم_الصنف(...))
//   - buildMemberAccess: الوصول لأعضاء الكائن (كائن.عضو)
//   - b_.buildMethodCall: استدعاء طريقة على كائن (كائن.طريقة(...)) — نُقلت لاحقاً إلى call_method_dispatch.cpp (CW-05)
//
//   تم فصل هذه الدوال عن sir_builder_calls.cpp الذي يحتوي على:
//   - b_.buildFunctionCall: استدعاء الدوال العادية والدوال المدمجة
// ============================================================================

#include <string>
#include <cstdio>
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // buildNewObject - بناء تعليمات إنشاء كائن جديد
            // ============================================================================
            // مصدر التعريف / Source: class_nodes.h:164
            // التوقيع / Signature: BuildResult buildNewObject(AST::NewExpr* newExpr);
            //
            // المعاملات / Parameters:
            // - newExpr: AST::NewExpr* (class_nodes.h:164)
            //
            // NewExpr Members:
            // - className: std::string (line 167)
            // - arguments: std::vector<std::unique_ptr<Expr>> (line 168)
            // ============================================================================
            BuildResult ExpressionBuilder::buildNewObject(AST::NewExpr *newExpr)
            {
                if (!newExpr)
                {
                    return BuildResult();
                }

                // (AR) الخطوة 1: البحث عن الصنف في الوحدة
                // (EN) Step 1: Find class in module
                auto sirClass = b_.module_->getClass(newExpr->className);
                if (!sirClass)
                {
                    b_.errors_.push_back("Class not found: " + newExpr->className);
                    return BuildResult();
                }

                // (AR) الخطوة 2: حجز ذاكرة للكائن
                // (EN) Step 2: Allocate memory for object
                std::string objReg = b_.newTempRegister();

                if (b_.currentBlock_)
                {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register(objReg, SadTypeKind::Integer);
                    // (AR) إضافة اسم الصنف في metadata
                    // (EN) Add class name as metadata
                    allocInst.operands.push_back(SIROperand::ConstantString(newExpr->className));
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // ───────────────────────────────────────────────────────────────
                // (AR) تهيئة حقول البنية بقيمها الافتراضية قبل أيّ باني (ISSUE-036)
                //      المفسّر يطبّق الافتراضيّات دائماً؛ نطابقه هنا في codegen.
                //      أيّ باني لاحق يكتب فوقها (الافتراضيّ ثم التعيين الصريح).
                // (EN) Initialize struct fields with their defaults before any ctor (ISSUE-036)
                //      The interpreter always applies defaults; we match that in codegen.
                //      Any later constructor overwrites them (default first, then assignment).
                // ───────────────────────────────────────────────────────────────
                auto defaultsIt = b_.structFieldDefaults_.find(newExpr->className);
                if (defaultsIt != b_.structFieldDefaults_.end() && b_.currentBlock_)
                {
                    for (const auto &fieldDefault : defaultsIt->second)
                    {
                        const std::string &fieldName = fieldDefault.first;
                        Sad::AST::Expression *defaultExpr = fieldDefault.second;
                        if (!defaultExpr)
                            continue;

                        auto valResult = buildExpression(defaultExpr);
                        std::string valReg = valResult.registerName;
                        SadTypeKind valType = valResult.type;

                        // (AR) تجسيد القيمة الثابتة في سجلّ عبر MOVE (STORE يتطلّب سجلّاً)
                        // (EN) Materialize constant into a register via MOVE (STORE needs a register)
                        if (valResult.isConstant)
                        {
                            valReg = b_.newTempRegister();
                            SIRInstruction moveInst(SIROpcode::MOVE);
                            moveInst.result = SIROperand::Register(valReg, valType);
                            if (valType == SadTypeKind::String)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                            }
                            else if (valType == SadTypeKind::Float)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                            }
                            else if (valType == SadTypeKind::Boolean)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantBool(
                                    valResult.constantValue == "true" || valResult.constantValue == "1"));
                            }
                            else
                            {
                                try
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                                }
                                catch (const std::exception &)
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                            }
                            b_.currentBlock_->addInstruction(moveInst);
                        }

                        // (AR) STORE(القيمة، الكائن، اسم الحقل) — نفس مسار إسناد العضو
                        // (EN) STORE(value, object, fieldName) — same path as member assignment
                        SIRInstruction storeInst;
                        storeInst.opcode = SIROpcode::STORE;
                        storeInst.operands.push_back(SIROperand::Register(valReg, valType));
                        storeInst.operands.push_back(SIROperand::Register(objReg, SadTypeKind::Integer));
                        storeInst.operands.push_back(SIROperand::ConstantString(fieldName));
                        storeInst.comment = "struct field default: " + fieldName;
                        b_.currentBlock_->addInstruction(storeInst);
                    }
                }

                // (AR) الخطوة 3: استدعاء دالة البناء (constructor) إن وجدت
                // (EN) Step 3: Call constructor if exists
                std::string constructorName = constructorNameFor(newExpr->className);
                auto constructor = sirClass->getMethod(constructorName);

                // (AR) لا نُصدر CALL إلا حين يوجد باني فعليّ. تمرير وسائط موضعيّة لبنية بلا باني
                //      يتجاهلها المفسّر (يستعمل الافتراضيّات)؛ فلا نُصدر CALL لرمز غير معرَّف.
                // (EN) Only emit CALL when a real constructor exists. Passing positional args to a
                //      struct with no constructor is ignored by the interpreter (uses defaults);
                //      so we avoid emitting a CALL to an undefined symbol.
                if (constructor)
                {
                    // (AR) بناء وسائط الباني
                    // (EN) Build constructor arguments
                    std::vector<SIROperand> args;

                    // (AR) الوسيط الأول هو الكائن نفسه (self)
                    // (EN) First argument is the object itself (self)
                    args.push_back(SIROperand::Register(objReg, SadTypeKind::Integer));

                    // (AR) أسماء أصناف الوسائط (لتتبّع أنواع الحقول الكائنيّة)
                    // (EN) Argument class names (to track object-typed field classes)
                    std::vector<std::string> argClassNames;
                    argClassNames.push_back(""); // self

                    // (AR) بقية الوسائط
                    // (EN) Rest of arguments
                    for (const auto &arg : newExpr->arguments)
                    {
                        auto argResult = buildExpression(arg.get());
                        argClassNames.push_back(argResult.className);
                        if (argResult.isConstant && !argResult.constantValue.empty())
                        {
                            // تمرير الثوابت بحسب نوعها
                            switch (argResult.type)
                            {
                            case SadTypeKind::Integer:
                                args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                                break;
                            case SadTypeKind::Float:
                                args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                                break;
                            case SadTypeKind::String:
                                args.push_back(SIROperand::ConstantString(argResult.constantValue));
                                break;
                            default:
                                args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                break;
                            }
                        }
                        else
                        {
                            args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        }
                    }

                    // ───────────────────────────────────────────────────────────────
                    // (AR) تبطينُ الوسائطِ الناقصةِ بـ**عدمٍ** (لاشيء) لا بتركِ الخانةِ فارغة.
                    //      بدونه تحملُ تعليمةُ الاستدعاءِ معاملاتٍ أقلَّ ممّا يُصرِّحُه الباني،
                    //      فتُبطّنُها الخلفيّةُ بـ`Constant::getNullValue` أي **صفرًا** —
                    //      والصفرُ رقمٌ مشروعٌ لا عدم، فتعطي `هذا._مهلة == لاشيء` **خطأً**
                    //      في الشيفرةِ المترجَمةِ و**صحيحًا** في المفسّرِ بعدَ إصلاحِ
                    //      expression_evaluator_oop_new.cpp ⇒ تباعُدُ محرّكَين.
                    //      التفصيلُ والثغرةُ المعلَنةُ في `padOmittedArgsWithNull` (sir_module.h).
                    //      و`params[0]` هو self (يُضيفُه buildClassConstructor أوّلَ معامل)
                    //      و`args[0]` هو self كذلك، فالفهرسانِ متقابلانِ تمامًا.
                    // (EN) Pad missing arguments with **null**. Without this the CALL carries
                    //      fewer operands than the constructor declares, so the backend pads with
                    //      `Constant::getNullValue`, i.e. **zero** — a legitimate number, not
                    //      null. `this._timeout == null` then yields false in compiled code and
                    //      true in the interpreter: an engine divergence. Details and the declared
                    //      gap live in `padOmittedArgsWithNull` (sir_module.h). `params[0]` is
                    //      self (added first by buildClassConstructor) and so is `args[0]`, so the
                    //      two index spaces line up exactly.
                    // ───────────────────────────────────────────────────────────────
                    // (AR) عددُ الوسائطِ المُمرَّرةِ **فعلًا** — يُلتقَطُ قبلَ التبطين. استنتاجُ
                    //      أنواعِ الحقولِ أدناه يجبُ أن يقتصرَ عليه: خانةٌ مُبطَّنةٌ لا تحملُ
                    //      نوعًا كتبَه المستخدم، فلو دخلت الاستنتاجَ لثبّتت نوعَ الحقلِ على
                    //      وسمِ التبطينِ فلا يُصحّحُه استدعاءٌ لاحقٌ يُمرّرُ الوسيطَ حقًّا.
                    // (EN) The count of arguments **actually** passed — captured before padding.
                    //      The field-type inference below must be bounded by it: a padded slot
                    //      carries no user-written type, and letting it into the inference would
                    //      pin the field to the pad's tag, which a later call that does pass the
                    //      argument can no longer correct.
                    const size_t passedArgCount = args.size();
                    padOmittedArgsWithNull(constructor->getParameters(), args);
                    // (AR) إبقاءُ أسماءِ الأصنافِ متوازيةً مع الوسائط — المُبطَّنُ بلا صنف.
                    // (EN) Keep class names parallel to args — a padded slot has no class.
                    argClassNames.resize(args.size());

                    // (AR) إنشاء تعليمة استدعاء الباني
                    // (EN) Create constructor call instruction
                    if (b_.currentBlock_)
                    {
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(b_.newTempRegister(), SadTypeKind::Void);
                        callInst.operands.push_back(SIROperand::Function(constructorName));
                        for (const auto &arg : args)
                        {
                            callInst.operands.push_back(arg);
                        }
                        b_.currentBlock_->addInstruction(callInst);
                    }

                    // ───────────────────────────────────────────────────────────────
                    // (AR) استنتاج أنواع الحقول من أنواع الوسائط الفعلية في موضع الاستدعاء
                    // (EN) Infer field types from actual argument types at call site
                    // ───────────────────────────────────────────────────────────────
                    if (constructor)
                    {
                        const auto &params = constructor->getParameters();
                        // params[0] = self, params[1..N] = user params
                        // args[0] = self, args[1..N] = user args

                        // (AR) بناء خريطة: اسم_المعامل ← نوع_الوسيط
                        // (EN) Build map: paramName → argType
                        std::unordered_map<std::string, SadTypeKind> paramTypes;
                        std::unordered_map<std::string, std::string> paramClassNames;
                        // (AR) الحدُّ `passedArgCount` لا `args.size()`: الخاناتُ المُبطَّنةُ
                        //      بعدَه ليست أنواعًا كتبها المستخدم (انظر التعليقَ أعلاه).
                        // (EN) Bound by `passedArgCount`, not `args.size()`: the slots padded
                        //      past it are not user-written types (see the note above).
                        for (size_t i = 1; i < params.size() && i < passedArgCount; i++)
                        {
                            paramTypes[params[i].name] = args[i].dataType;
                            if (i < argClassNames.size() && !argClassNames[i].empty())
                                paramClassNames[params[i].name] = argClassNames[i];
                        }

                        // (AR) تحديث حقول الصنف الحالي
                        // (EN) Update current class fields
                        if (!sirClass->paramToFieldMap_.empty())
                        {
                            for (auto &[paramName, argType] : paramTypes)
                            {
                                auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                                if (fieldIt != sirClass->paramToFieldMap_.end())
                                {
                                    const std::string &fieldName = fieldIt->second;
                                    // (AR) سجّل اسم صنف الحقل الكائنيّ إن وُجد (لتمكين الوصول المتسلسل)
                                    // (EN) Record object field's class name if any (enables chained access)
                                    auto pcnIt = paramClassNames.find(paramName);
                                    if (pcnIt != paramClassNames.end())
                                    {
                                        sirClass->fieldClassNames_[fieldName] = pcnIt->second;
                                    }
                                    auto currentType = sirClass->fields_.find(fieldName);
                                    if (currentType != sirClass->fields_.end() &&
                                        currentType->second == SadTypeKind::Pointer &&
                                        argType != SadTypeKind::Pointer && argType != SadTypeKind::Void)
                                    {
                                        sirClass->fields_[fieldName] = argType;
#ifndef NDEBUG
                                        std::cout << "[DEBUG] buildNewObject: inferred field '" << fieldName
                                                  << "' type=" << static_cast<int>(argType)
                                                  << " from arg '" << paramName << "'" << std::endl;
#endif
                                    }
                                }
                            }
                        }

                        // ───────────────────────────────────────────────────────────────
                        // (AR) نشر أنواع الوسائط عبر سلسلة الوراثة (super constructor chain)
                        // (EN) Propagate arg types through inheritance chain (super ctor chain)
                        // ───────────────────────────────────────────────────────────────
                        auto currentClass = sirClass;
                        auto currentParamTypes = paramTypes;

                        while (currentClass && !currentClass->parentClass.empty() &&
                               !currentClass->superParamMapping_.empty())
                        {
                            auto parentSirClass = b_.module_->getClass(currentClass->parentClass);
                            if (!parentSirClass)
                                break;

                            // (AR) الحصول على معاملات باني الأب
                            // (EN) Get parent constructor params
                            std::string parentCtorName = constructorNameFor(currentClass->parentClass);
                            auto parentCtor = parentSirClass->getMethod(parentCtorName);
                            if (!parentCtor)
                                break;

                            const auto &parentParams = parentCtor->getParameters();

                            // (AR) بناء أنواع معاملات باني الأب من superParamMapping
                            // (EN) Build parent param types from superParamMapping
                            std::unordered_map<std::string, SadTypeKind> parentParamTypes;
                            for (auto &[superIdx, childParamName] : currentClass->superParamMapping_)
                            {
                                int parentIdx = superIdx + 1; // +1 (skip self)
                                if (parentIdx < Sad::Security::SafeArithmetic::assertSafeCast<int>(parentParams.size(), "expression_objects_size"))
                                {
                                    auto it = currentParamTypes.find(childParamName);
                                    if (it != currentParamTypes.end())
                                    {
                                        parentParamTypes[parentParams[parentIdx].name] = it->second;
                                    }
                                }
                            }

                            // (AR) تحديث حقول الأب باستخدام paramToFieldMap
                            // (EN) Update parent fields using paramToFieldMap
                            for (auto &[parentParamName, inferredType] : parentParamTypes)
                            {
                                auto fieldIt = parentSirClass->paramToFieldMap_.find(parentParamName);
                                if (fieldIt != parentSirClass->paramToFieldMap_.end())
                                {
                                    const std::string &fieldName = fieldIt->second;
                                    auto currentFldType = parentSirClass->fields_.find(fieldName);
                                    if (currentFldType != parentSirClass->fields_.end() &&
                                        currentFldType->second == SadTypeKind::Pointer &&
                                        inferredType != SadTypeKind::Pointer && inferredType != SadTypeKind::Void)
                                    {
                                        parentSirClass->fields_[fieldName] = inferredType;
                                        // (AR) التحديث أيضاً في الأبناء (الحقول الموروثة)
                                        // (EN) Also update in children (inherited fields)
                                        if (sirClass->fields_.count(fieldName))
                                        {
                                            sirClass->fields_[fieldName] = inferredType;
                                        }
                                        if (currentClass->fields_.count(fieldName))
                                        {
                                            currentClass->fields_[fieldName] = inferredType;
                                        }
#ifndef NDEBUG
                                        std::cout << "[DEBUG] buildNewObject: propagated field '" << fieldName
                                                  << "' type=" << static_cast<int>(inferredType)
                                                  << " to parent '" << parentSirClass->name << "'" << std::endl;
#endif
                                    }
                                }
                            }

                            // (AR) انتقل للأب التالي
                            // (EN) Move to next parent
                            currentClass = parentSirClass;
                            currentParamTypes = parentParamTypes;
                        }
                    }
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildNewObject: object created in register '" << objReg << "'" << std::endl;
#endif

                // (AR) تتبّع نوع الكائن لدعم إعادة تعريف العوامل
                // (EN) Track object type for operator overloading support
                b_.classInstanceTypes_[objReg] = newExpr->className;

                // (AR) إرجاع مؤشر للكائن مع اسم الصنف — النوع STRUCT وليس I64
                // (EN) Return pointer to object with class name — STRUCT type not I64
                BuildResult result(objReg, SadTypeKind::Struct);
                result.className = newExpr->className;
                return result;
            }

            // ============================================================================
            // buildMemberAccess - بناء الوصول لعضو داخل كائن
            // ============================================================================
            // مصدر التعريف / Source: class_nodes.h:206
            // التوقيع / Signature: BuildResult buildMemberAccess(AST::MemberAccessExpr* memberExpr);
            //
            // المعاملات / Parameters:
            // - memberExpr: AST::MemberAccessExpr* (class_nodes.h:206)
            //
            // MemberAccessExpr Members:
            // - object: std::unique_ptr<Expr> (line 209)
            // - memberName: std::string (line 210)
            // ============================================================================
            BuildResult ExpressionBuilder::buildMemberAccess(AST::MemberAccessExpr *memberExpr)
            {
                if (!memberExpr)
                {
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMemberAccess: accessing member '"
                          << memberExpr->memberName << "'" << std::endl;
#endif

                // ================================================================
                // (AR) فحص مبكر: الوصول لحقل ساكن عبر اسم الصنف
                //      مثال: عداد.القيمة — "عداد" اسم صنف وليس متغيراً
                //      نبحث في b_.staticFields_ عن "عداد.القيمة" كمتغير عام
                // (EN) Early check: static field access via class name
                //      Example: Counter.value — "Counter" is class name, not variable
                //      Look up "Counter.value" in b_.staticFields_ as global variable
                // ================================================================
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                {
                    // ================================================================
                    // (AR) فحص مبكر: وصول عضو فضاء أسماء (namespace.member)
                    //      مثال: رياضيات.PI → تحميل المتغير العام رياضيات::PI
                    // (EN) Early check: namespace member access (namespace.member)
                    //      Example: math.PI → load global math::PI
                    // ================================================================
                    auto nsIt = b_.namespaceMembers_.find(varExpr->name);
                    if (nsIt != b_.namespaceMembers_.end())
                    {
                        auto memIt = nsIt->second.find(memberExpr->memberName);
                        if (memIt != nsIt->second.end())
                        {
                            const auto &nsInfo = memIt->second;
                            if (nsInfo.kind == "var")
                            {
                                // (AR) متغير فضاء → تحميل من المتغير العام
                                // (EN) Namespace variable → load from global
                                std::string loadReg = b_.newTempRegister();
                                if (b_.currentBlock_)
                                {
                                    SIRInstruction loadInst(SIROpcode::LOAD);
                                    loadInst.result = SIROperand::Register(loadReg, nsInfo.type);
                                    loadInst.operands.push_back(
                                        SIROperand::Global(nsInfo.sirName, nsInfo.type));
                                    loadInst.comment = "Load namespace member: " + nsInfo.sirName;
                                    b_.currentBlock_->addInstruction(loadInst);
                                }
                                return BuildResult(loadReg, nsInfo.type);
                            }
                            else if (nsInfo.kind == "func")
                            {
                                // (AR) دالة فضاء → نُرجع مرجع الدالة بالاسم الكامل
                                //      الاستدعاء الفعلي يتم في b_.buildMethodCall أو buildCallExpr
                                // (EN) Namespace function → return function reference with full name
                                //      Actual call happens in b_.buildMethodCall or buildCallExpr
                                return BuildResult(nsInfo.sirName, SadTypeKind::Function);
                            }
                        }
                    }

                    std::string staticFieldName = varExpr->name + "." + memberExpr->memberName;
                    auto sfIt = b_.staticFields_.find(staticFieldName);
                    if (sfIt != b_.staticFields_.end())
                    {
                        // (AR) حقل ساكن — التحميل من متغير عام
                        // (EN) Static field — load from global variable
                        std::string loadReg = b_.newTempRegister();
                        if (b_.currentBlock_)
                        {
                            SIRInstruction loadInst(SIROpcode::LOAD);
                            loadInst.result = SIROperand::Register(loadReg, sfIt->second);
                            loadInst.operands.push_back(
                                SIROperand::Global(staticFieldName, sfIt->second));
                            loadInst.comment = "Load static field: " + staticFieldName;
                            b_.currentBlock_->addInstruction(loadInst);
                        }
                        return BuildResult(loadReg, sfIt->second);
                    }
                }

                // (AR) الخطوة 1: بناء تعبير الكائن. مثل buildExprMember: لا تبنِ قاعدةً اسمًا
                //      مجرّدًا ليس متغيّرًا معرّفًا (تدفع «Undefined variable» زائفًا)؛ قد تكون اسم
                //      نوعٍ قاعدةَ وصولِ عضوٍ تحلّه الخطوة 1.25. نؤجّل ونُبلّغ الخطأ الحقيقيّ لاحقًا
                //      فقط إن لم يُحَلّ (H2 — مرآة إصلاح buildExprMember؛ هذا التوأم غير مُخلَّق
                //      اليوم من المحلّل لكن نُبقيه متّسقًا تفاديًا لقنبلةٍ كامنة عند إحيائه).
                // (EN) Step 1: Build object expression. Like buildExprMember: don't build a bare
                //      undefined-variable base (it pushes a spurious "Undefined variable"); it may
                //      be a type name serving as a member-access base resolved by Step 1.25. Defer
                //      and report the genuine error later only if unresolved (H2 — mirrors the
                //      buildExprMember fix; this twin is not produced by the parser today but is
                //      kept consistent to defuse a latent bomb on revival).
                Sad::AST::VariableExpr *baseVarExpr =
                    dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get());
                bool deferBase = (baseVarExpr != nullptr) && (b_.lookupVariable(baseVarExpr->name) == nullptr);

                BuildResult objResult;
                if (!deferBase)
                {
                    objResult = buildExpression(memberExpr->object.get());
                }

                // ================================================================
                // (AR) الخطوة 1.25: فحص إن كان الوصول لمتغاير وحدة (Unit variant)
                //      من تعداد ADT. عند استخدام شكل.نقطة (بدون أقواس):
                //      - "شكل" ليس متغيراً حقيقياً — بل اسم نوع التعداد
                //      - buildExpression("شكل") تُرجع نتيجة فارغة/غير صالحة
                //      - لذا نبحث عن الاسم الكامل "شكل.نقطة" كمتغير عام مباشر
                //      بدون هذا: متغير س = شكل.نقطة يخزن 0 بدلاً من الوسم الصحيح
                // (EN) Step 1.25: Check if access is to a Unit variant of an ADT enum.
                //      When شكل.نقطة (without parens) is used:
                //      - "شكل" isn't a real variable — it's the enum type name
                //      - buildExpression("شكل") returns empty/invalid result
                //      - So look up the full name "شكل.نقطة" as a direct global variable
                //      Without this: var s = Shape.Point stores 0 instead of correct tag
                // ================================================================
                {
                    // (AR) استخراج اسم الكائن من VariableExpr
                    // (EN) Extract object name from VariableExpr
                    std::string objName;
                    if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                    {
                        objName = varExpr->name;
                    }

                    if (!objName.empty())
                    {
                        std::string fullName = objName + "." + memberExpr->memberName;

                        // (AR) أولاً: فحص إن كان الاسم الكامل متغيراً مسجلاً (Unit variant عام)
                        // (EN) First: check if full name is a registered variable (global unit variant)
                        auto *varInfo = b_.lookupVariable(fullName);
                        if (varInfo)
                        {
                            // (AR) وُجد المتغير مباشرة — نحمّل قيمته
                            //      مثال: شكل.نقطة مسجل كـ global constant i64 2
                            // (EN) Found variable directly — load its value
                            //      Example: Shape.Point registered as global constant i64 2
                            if (varInfo->isGlobal && b_.currentBlock_)
                            {
                                std::string loadReg = b_.newTempRegister();
                                SIRInstruction loadInst(SIROpcode::LOAD);
                                loadInst.result = SIROperand::Register(loadReg, varInfo->type);
                                loadInst.operands.push_back(
                                    SIROperand::Global(fullName, varInfo->type));
                                loadInst.comment = "Load unit variant: " + fullName;
                                b_.currentBlock_->addInstruction(loadInst);
                                return BuildResult(loadReg, varInfo->type);
                            }
                            return BuildResult(varInfo->registerName, varInfo->type);
                        }
                    }
                }

                // (AR) الخطوة 1.5: فحص إن كان الكائن قيمة تعداد جبري (ADT)
                //      إذا كان الكائن مسجلاً في objectClassMap_ كنوع ADT
                //      نستخدم ENUM_GET_PAYLOAD بدلاً من LOAD العادي
                //      مثال: س.نصف_القطر حيث س = شكل.دائرة(5)
                // (EN) Step 1.5: Check if object is an ADT enum value
                //      If object is registered in objectClassMap_ as ADT type
                //      use ENUM_GET_PAYLOAD instead of regular LOAD
                //      Example: s.radius where s = Shape.Circle(5)
                if (!objResult.className.empty())
                {
                    auto adtIt = b_.adtEnumTable_.find(objResult.className);
                    if (adtIt != b_.adtEnumTable_.end())
                    {
                        // (AR) هذا كائن ADT — نبحث عن الحقل بالاسم
                        // (EN) This is an ADT object — look up field by name
                        const ADTEnumInfo &adtInfo = adtIt->second;
                        // (AR) [ISSUE-080] توزيعٌ حسب وسم الحالة زمن التشغيل (المسار المشترك مع
                        //      التوأم buildExprMember عبر buildAdtFieldDispatch) — يُغلق تصادم
                        //      أسماء الحقول عبر الحالات والوصول لحالةٍ خاطئة (بدل الفهرس الأحاديّ).
                        // (EN) [ISSUE-080] runtime variant-tag dispatch (shared with the twin
                        //      buildExprMember via buildAdtFieldDispatch) — closes cross-variant
                        //      field-name collision and wrong-variant access (vs the single index).
                        if (adtInfo.findFieldIndex(memberExpr->memberName) >= 0 && b_.currentBlock_)
                        {
                            return buildAdtFieldDispatch(objResult, memberExpr->memberName, adtInfo);
                        }
                        // (AR) الحقل غير موجود في ADT — نسقط إلى المسار العادي
                        // (EN) Field not found in ADT — fall through to regular path
                    }
                }

                // (AR) قاعدة مؤجّلة لم يحلّها أيّ مسار عضو ⇒ اسمٌ غير معرّف فعلًا (H2). نُبلّغ
                //      الخطأ الحقيقيّ (لا نجاح صامت — objResult فارغ فالمتابعة تُنتِج SIR معطوبًا).
                // (EN) Deferred base unresolved by any member path ⇒ genuinely undefined (H2).
                //      Report the real error (no silent success — objResult is empty).
                if (deferBase)
                {
                    b_.errors_.push_back("Error: Undefined variable '" + baseVarExpr->name + "'");
                    return BuildResult();
                }

                // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو (المسار العادي للأصناف)
                // (EN) Step 2: Create member access instruction (regular path for classes)
                std::string resultReg = b_.newTempRegister();

                // (AR) محاولة استنتاج نوع العضو من module
                // (EN) Try to infer member type from module
                SadTypeKind memberType = SadTypeKind::Integer; // (AR) افتراضي
                // ═══════════════════════════════════════════════════════════════
                // (AR) 🔑 صنفُ الحقلِ المُعشَّشِ — بلا هذا السطرِ لا يُقرأ التعشيشُ أصلًا.
                //
                //      كان `result.className` يُورَّث من **الكائنِ الحاوي** دائمًا، فَـ
                //      `ك.د` تخرج بصنفِ `ك` لا بصنفِ `د`. فيبحث الوصولُ التالي
                //      (`.ق`) عن الحقلِ في الصنفِ الخطأِ فلا يجده، فيُحمَّل بإزاحةٍ
                //      لا تخصُّه ⇒ `rc=139` (مقيس 2026-08-16).
                //
                //      🔑 والدرسُ أنّ إنشاءَ الكائنِ المُعشَّشِ في الباني **وحدَه لا
                //      يكفي**: الكتابةُ والقراءةُ بابان، وسدُّ أحدِهما يترك العطبَ
                //      حيًّا كما هو. وقد قِيس ذلك: بعدَ عملِ الباني تعاوديًّا بقي
                //      الانهيارُ كما كان حتّى وُصِل هذا الطرف.
                // (EN) The nested field's own class. className was always inherited
                //      from the CONTAINING object, so `k.d` carried k's class and the
                //      next access looked `.q` up in the wrong class, loading a foreign
                //      offset ⇒ measured rc=139. Constructing the nested object in the
                //      constructor alone is not enough: write and read are two doors.
                // ═══════════════════════════════════════════════════════════════
                std::string nestedFieldClass;
                if (!objResult.className.empty() && b_.module_)
                {
                    auto sirClass = b_.module_->getClass(objResult.className);
                    if (sirClass)
                    {
                        // (AR) الجدولُ نفسُه الذي يقرؤه التوأمُ `buildExprMember` —
                        //      فلا يفترق البابان في جوابِ السؤالِ الواحد.
                        // (EN) The same table the twin buildExprMember reads.
                        auto nestedIt = sirClass->fieldClassNames_.find(memberExpr->memberName);
                        if (nestedIt != sirClass->fieldClassNames_.end())
                        {
                            nestedFieldClass = nestedIt->second;
                        }
                        auto fieldIt = sirClass->fields_.find(memberExpr->memberName);
                        if (fieldIt != sirClass->fields_.end())
                        {
                            // (AR) حمّل الحقل بنوعه الفعليّ للأنواع القيميّة وإلا بُتر/أُسيء تفسيره
                            //      (Float كان يُبتر إلى صحيح — ISSUE-037). الأنواع الكائنيّة/المؤشّرة تبقى I64.
                            // (EN) Load field with its actual type for value types, else it gets
                            //      truncated/misread (Float was truncated to int — ISSUE-037).
                            //      Object/pointer types stay I64.
                            if (fieldIt->second == SadTypeKind::Array ||
                                fieldIt->second == SadTypeKind::String ||
                                fieldIt->second == SadTypeKind::Float ||
                                fieldIt->second == SadTypeKind::Boolean)
                            {
                                memberType = fieldIt->second;
                            }
                        }
                    }
                }

                if (b_.currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(resultReg, memberType);

                    // (AR) المعامل الأول: الكائن
                    // (EN) First operand: object
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));

                    // (AR) المعامل الثاني: اسم العضو (كـ offset أو اسم)
                    // (EN) Second operand: member name (as offset or name)
                    loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->memberName));

                    b_.currentBlock_->addInstruction(loadInst);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMemberAccess: result in register '" << resultReg
                          << "' with type " << sirTypeToString(memberType) << std::endl;
#endif

                BuildResult result(resultReg, memberType);
                result.className = nestedFieldClass.empty() ? objResult.className : nestedFieldClass;
                result.isFieldAccess = true;
                return result;
            }
            // (AR) تم نقل b_.buildMethodCall إلى sir_builder_method_call.cpp (CW-05)
            // (EN) b_.buildMethodCall moved to sir_builder_method_call.cpp (CW-05)
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
