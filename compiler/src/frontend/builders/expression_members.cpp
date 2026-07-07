// ============================================================================
// sir_builder_expr_members.cpp
// ============================================================================
// (AR) دوال بناء تعابير الأعضاء والإسناد للأعضاء في الأصناف
//      مستخرجة من sir_builder_expressions_dispatch.cpp
// (EN) Member access and member assignment expression builders
//      Extracted from sir_builder_expressions_dispatch.cpp
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "class_nodes.h"

#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprMember — بناء تعبير MemberExpr (وصول للعضو في كائن)
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprMember(AST::MemberExpr *memberExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found MemberExpr for member '"
                          << memberExpr->member << "'" << std::endl;
#endif

                // ================================================================
                // (AR) فحص مبكر: وصول عضو فضاء أسماء (namespace.member)
                //      مثال: رياضيات.PI → تحميل المتغير العام رياضيات::PI
                //      هذا يجب أن يأتي قبل buildExpression(object) لأن اسم الفضاء
                //      ليس متغيراً حقيقياً ولن يُجد في النطاق
                // (EN) Early check: namespace member access (namespace.member)
                //      Example: math.PI → load global math::PI
                //      Must come before buildExpression(object) because namespace name
                //      is not a real variable and won't be found in scope
                // ================================================================
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                {
                    auto nsIt = b_.namespaceMembers_.find(varExpr->name);
                    if (nsIt != b_.namespaceMembers_.end())
                    {
                        auto memIt = nsIt->second.find(memberExpr->member);
                        if (memIt != nsIt->second.end())
                        {
                            const auto &nsInfo = memIt->second;
                            if (nsInfo.kind == "var")
                            {
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
                                return BuildResult(nsInfo.sirName, SadTypeKind::Function);
                            }
                        }
                    }

                    // (AR) عضو تعداد بقيمة نصّيّة صريحة (مثل «ألوان.أحمر») — فحصٌ
                    //   مبكّر **قبل** بناء الكائن، لأنّ اسم التعداد ليس متغيّرًا وبناؤه
                    //   يدفع «Undefined variable» زائفًا إلى errors_ (نظير فحص فضاء
                    //   الأسماء أعلاه). يُعاد ثابتًا نصّيًّا مطابقًا لليترال (تكافؤ).
                    // (EN) String-valued enum member (e.g. ألوان.أحمر) — checked BEFORE
                    //   building the object (the enum name is not a variable; building it
                    //   would push a spurious "Undefined variable" into errors_).
                    {
                        auto sit = b_.enumStringConstants_.find(varExpr->name + "." + memberExpr->member);
                        if (sit != b_.enumStringConstants_.end())
                        {
                            return BuildResult(sit->second, SadTypeKind::String, true);
                        }
                    }
                }

                // (AR) الخطوة 1: بناء تعبير الكائن.
                //   لكن إن كانت القاعدة اسمًا مجرّدًا (VariableExpr) ليس متغيّرًا معرّفًا، فلا
                //   نبنِها كمتغيّر: بناؤها يدفع «Undefined variable» زائفًا إلى errors_ بينما قد
                //   تكون اسم نوعٍ (تعداد/صنف) قاعدةَ وصولٍ لعضوٍ تحلّه الخطوة 1.25 أدناه عبر الاسم
                //   لا عبر السجلّ. نؤجّل البناء (deferBase) ونُبلّغ الخطأ الحقيقيّ لاحقًا فقط إن لم
                //   يُحَلّ كعضو (حارسٌ ضدّ إعادة إدخال «النجاح الصامت» لقاعدةٍ غير معرّفةٍ فعلًا).
                //   (RFC: فصل التحذيرات عن الأخطاء — تنظيف الأخطاء الكاذبة كي تصحّ بوّابة الأخطاء.)
                // (EN) Step 1: Build object expression. But if the base is a bare VariableExpr that
                //   is not a defined variable, don't build it as a variable: doing so pushes a
                //   spurious "Undefined variable" while it may be a type name (enum/class) serving
                //   as a member-access base resolved by Step 1.25 below via the name (not a
                //   register). We defer the build and report the genuine error later only if it is
                //   not resolved as a member (a guard against re-introducing "silent success" for a
                //   genuinely undefined base).
                Sad::AST::VariableExpr *baseVarExpr =
                    dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get());
                bool deferBase = (baseVarExpr != nullptr) && (b_.lookupVariable(baseVarExpr->name) == nullptr);

                BuildResult objResult;
                if (!deferBase)
                {
                    objResult = buildExpression(memberExpr->object.get());
                }

                // ================================================================
                // (AR) [ISSUE-062] الوصول النقطيّ الرقميّ للصفّ: «ص.0»/«ص.1». المحلّل
                //      يحوّله إلى MemberExpr باسم عضوٍ رقميّ؛ لكنّ الصفّ يُخزَّن ببنية
                //      المصفوفة، فتحميل حقلٍ باسم نصّيّ «0» كان يُرجع 0. الصحيح: تحويله
                //      إلى ARRAY_GET بفهرسٍ عدديّ — نظير الفهرسة بالأقواس «ص[0]».
                // (EN) [ISSUE-062] Numeric tuple dot access «ص.0»/«ص.1». The parser turns
                //      it into a MemberExpr with a numeric member name; but a tuple is
                //      stored with array layout, so loading a field named «0» returned 0.
                //      Correct: lower it to ARRAY_GET with an integer index — mirroring
                //      bracket indexing «ص[0]».
                if ((objResult.type == SadTypeKind::Tuple || objResult.type == SadTypeKind::Array) &&
                    !memberExpr->member.empty() &&
                    memberExpr->member.find_first_not_of("0123456789") == std::string::npos &&
                    b_.currentBlock_)
                {
                    int64_t tupleIdx = 0;
                    try
                    {
                        tupleIdx = std::stoll(memberExpr->member);
                    }
                    catch (...)
                    {
                        tupleIdx = 0;
                    }

                    // (AR) استنتاج نوع الناتج من نوع عنصر الكائن، مطابقةً لمسار الفهرسة
                    //      بالأقواس «ص[0]» (buildExprIndex): عنصرٌ معروف ⇒ نوعه، وإلّا Integer.
                    // (EN) Infer result type from the object's element type, mirroring the
                    //      bracket-index path «ص[0]» (buildExprIndex): known element ⇒ its
                    //      type, else Integer.
                    SadTypeKind elemType = SadTypeKind::Integer;
                    if (objResult.elementType == SadTypeKind::Array)
                        elemType = SadTypeKind::Array;
                    else if (objResult.elementType != SadTypeKind::Void)
                        elemType = objResult.elementType;

                    // (AR) تجسيد الفهرس في سجلّ (ARRAY_GET يتوقّع معاملًا سجلّيًّا)
                    // (EN) Materialize index into a register (ARRAY_GET expects a register operand)
                    std::string idxReg = b_.newTempRegister();
                    SIRInstruction idxMove(SIROpcode::MOVE);
                    idxMove.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                    idxMove.operands.push_back(SIROperand::ConstantI64(tupleIdx));
                    b_.currentBlock_->addInstruction(idxMove);

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(resultReg, elemType);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    getInst.comment = "tuple numeric member access: ." + memberExpr->member;
                    b_.currentBlock_->addInstruction(getInst);

                    return BuildResult(resultReg, elemType);
                }

                // ================================================================
                // (AR) الخطوة 1.4 [ISSUE-077]: وصول حقلٍ مباشر على قيمة تعداد جبريّ (ADT)
                //      مبنيّة، مثل «ش.نق» حيث «متغير ش = شكل.دائرة(7)». صيغة «.اسم» تُنتج
                //      MemberExpr فتصل إلى هذا الباني (لا buildMemberAccess)، الّذي كان يفتقر
                //      لفرع ADT فيبعث LOAD صنفٍ عامًّا ⇒ «No class mapping» + خروج 0 صامت.
                //      نشرُ className يعمل بعد #161 (classInstanceTypes_["ش"]="شكل" ⇒
                //      objResult.className)، فنستعمله للبحث في adtEnumTable_ ونبعث
                //      ENUM_GET_PAYLOAD **مع operand[2]=اسم التعداد** (نظير المطابقة). محروسٌ
                //      بوجود التعداد ووجود الحقل ⇒ لا يمسّ الأصناف العاديّة ولا الصفوف.
                // (EN) Step 1.4 [ISSUE-077]: direct field access on a constructed ADT enum
                //      value, e.g. «ش.نق» where «ش = شكل.دائرة(7)». The «.name» syntax yields a
                //      MemberExpr reaching THIS builder (not buildMemberAccess), which lacked an
                //      ADT branch and emitted a generic class LOAD ⇒ «No class mapping» + silent
                //      exit 0. className propagation works post-#161, so we use it to look up
                //      adtEnumTable_ and emit ENUM_GET_PAYLOAD WITH operand[2]=enum name (mirrors
                //      match). Guarded by enum-exists + field-exists ⇒ never touches plain
                //      classes or tuples (they fall through unchanged).
                // ================================================================
                if (!objResult.className.empty() && !memberExpr->member.empty())
                {
                    auto adtIt = b_.adtEnumTable_.find(objResult.className);
                    if (adtIt != b_.adtEnumTable_.end())
                    {
                        const ADTEnumInfo &adtInfo = adtIt->second;
                        int fieldIdx = adtInfo.findFieldIndex(memberExpr->member);
                        if (fieldIdx >= 0 && b_.currentBlock_)
                        {
                            std::string resultReg = b_.newTempRegister();
                            SIRInstruction getPayload(SIROpcode::ENUM_GET_PAYLOAD);
                            getPayload.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                            getPayload.operands.push_back(
                                SIROperand::Register(objResult.registerName, objResult.type));
                            getPayload.operands.push_back(
                                SIROperand::ConstantI64(static_cast<int64_t>(fieldIdx)));
                            // (AR) المعامل [2]: اسم التعداد للبحث الصحيح عبر الحدود/التعدّد
                            // (EN) Operand [2]: enum name for correct lookup across boundaries/multiplicity
                            getPayload.operands.push_back(
                                SIROperand::ConstantString(objResult.className));
                            getPayload.comment = "ADT field access (MemberExpr): " + objResult.className +
                                                 "." + memberExpr->member + " (index=" + std::to_string(fieldIdx) + ")";
                            b_.currentBlock_->addInstruction(getPayload);

                            BuildResult result(resultReg, SadTypeKind::Integer);
                            result.className = objResult.className;
                            result.isFieldAccess = true;
                            return result;
                        }
                        // (AR) الحقل غير موجود في ADT ⇒ نسقط للمسار العاديّ
                        // (EN) Field not found in ADT ⇒ fall through to regular path
                    }
                }

                // ================================================================
                // (AR) الخطوة 1.25: فحص وصول لحالة واحدية (Unit variant) في تعداد جبري
                // (EN) Step 1.25: Check if accessing Unit variant of ADT enum
                // ================================================================
                {
                    std::string objName;
                    if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                    {
                        objName = varExpr->name;
                    }
                    if (!objName.empty())
                    {
                        std::string fullName = objName + "." + memberExpr->member;

                        // (AR) ملاحظة: ثوابت التعداد النصّيّة (ألوان.أحمر) تُفحَص مبكّرًا
                        //   أعلاه (قبل بناء الكائن) لتفادي تلويث errors_.

                        // (AR) الخطوة 1.25أ: فحص ما إذا كان unit variant في تعداد جبري (ADT)
                        // (EN) Step 1.25a: Check if this is a unit variant in an ADT enum
                        auto funcIt = b_.functionTable_.find(fullName);
                        if (funcIt != b_.functionTable_.end() && b_.currentBlock_)
                        {
                            std::string callReg = b_.newTempRegister();
                            SIRInstruction callInst(SIROpcode::CALL);
                            callInst.result = SIROperand::Register(callReg, SadTypeKind::Integer);
                            callInst.operands.push_back(
                                SIROperand::Function(funcIt->second.name));
                            callInst.comment = "Call unit variant constructor: " + fullName;
                            b_.currentBlock_->addInstruction(callInst);

                            b_.classInstanceTypes_[callReg] = objName;

                            BuildResult result(callReg, SadTypeKind::Integer);
                            result.className = objName;
                            return result;
                        }

                        // (AR) الخطوة 1.25ب: تحميل ثابت عام (تعداد بسيط أو C-style enum)
                        // (EN) Step 1.25b: Load global constant (simple enum or C-style enum)
                        auto *varInfo = b_.lookupVariable(fullName);
                        if (varInfo)
                        {
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

                // (AR) وصلنا هنا دون أن يحلّ أيّ مسار عضوٍ القاعدةَ المؤجّلة ⇒ اسمٌ غير معرّف فعلًا
                //   (تعداد/صنف مجهول أو خطأ مطبعيّ). نُبلّغ الخطأ الحقيقيّ الآن (لا نجاح صامت).
                // (EN) Reached here without any member path resolving the deferred base ⇒ a
                //   genuinely undefined name (unknown enum/class or a typo). Report the real error
                //   now (no silent success). objResult is empty, so proceeding would emit broken SIR.
                if (deferBase)
                {
                    b_.errors_.push_back("Error: Undefined variable '" + baseVarExpr->name + "'");
                    return BuildResult();
                }

                // (AR) استنتاج نوع العضو من module (الأصناف المسجلة)
                // (EN) Infer member type from module (registered classes)
                SadTypeKind memberType = SadTypeKind::Integer;
                std::string className = objResult.className;
                std::string memberClassName; // (AR) صنف الحقل إن كان كائناً (للوصول المتسلسل)
                if (className.empty() && dynamic_cast<Sad::AST::ThisExpr *>(memberExpr->object.get()))
                {
                    className = b_.currentClassName_;
                }
                if (!className.empty() && b_.module_)
                {
                    auto sirClass = b_.module_->getClass(className);
                    if (sirClass)
                    {
                        auto fieldIt = sirClass->fields_.find(memberExpr->member);
                        if (fieldIt != sirClass->fields_.end())
                        {
                            if (fieldIt->second == SadTypeKind::Array)
                            {
                                memberType = SadTypeKind::Array;
                            }
                            else if (fieldIt->second == SadTypeKind::String)
                            {
                                memberType = SadTypeKind::String;
                            }
                            // (AR) حقل عشريّ: يجب تحميله بنوع Float وإلا بُتر إلى صحيح (ISSUE-037)
                            // (EN) Float field: must load as Float or it gets truncated to int (ISSUE-037)
                            else if (fieldIt->second == SadTypeKind::Float)
                            {
                                memberType = SadTypeKind::Float;
                            }
                            // (AR) حقل منطقيّ: حمّله بنوعه الصحيح للطباعة الصحيحة
                            // (EN) Boolean field: load with correct type for correct printing
                            else if (fieldIt->second == SadTypeKind::Boolean)
                            {
                                memberType = SadTypeKind::Boolean;
                            }
                        }
                        // (AR) حقل كائنيّ: انقل اسم صنفه لتمكين الوصول المتسلسل اللاحق
                        // (EN) Object-typed field: carry its class name to enable chained access
                        auto fcnIt = sirClass->fieldClassNames_.find(memberExpr->member);
                        if (fcnIt != sirClass->fieldClassNames_.end())
                        {
                            memberClassName = fcnIt->second;
                        }
                    }
                }

                // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو
                // (EN) Step 2: Create member access instruction
                std::string resultReg = b_.newTempRegister();

                if (b_.currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(resultReg, memberType);
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->member));
                    b_.currentBlock_->addInstruction(loadInst);
                }

                BuildResult memberResult(resultReg, memberType);
                memberResult.isFieldAccess = true;
                memberResult.className = memberClassName;
                if (!memberClassName.empty())
                {
                    b_.classInstanceTypes_[resultReg] = memberClassName;
                }
                return memberResult;
            }

            // ============================================================================
            // buildExprMemberAssign — بناء تعبير MemberAssignExpr (إسناد لعضو في كائن)
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprMemberAssign(AST::MemberAssignExpr *memberAssignExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found MemberAssignExpr" << std::endl;
#endif

                // (AR) فحص مبكر: تعيين حقل ساكن عبر اسم الصنف
                // (EN) Early check: static field assignment via class name
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberAssignExpr->object.get()))
                {
                    std::string staticFieldName = varExpr->name + "." + memberAssignExpr->member;
                    auto sfIt = b_.staticFields_.find(staticFieldName);
                    if (sfIt != b_.staticFields_.end())
                    {
                        auto valResult = buildExpression(memberAssignExpr->value.get());

                        if (valResult.isConstant && b_.currentBlock_)
                        {
                            std::string reg = b_.newTempRegister();
                            SIRInstruction moveInst(SIROpcode::MOVE);
                            moveInst.result = SIROperand::Register(reg, valResult.type);
                            if (valResult.type == SadTypeKind::Float)
                                moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                            else if (valResult.type == SadTypeKind::Boolean)
                            {
                                // (AR) القيمة المنطقية: صحيح=true أو خطأ=false
                                // (EN) Boolean value: صحيح=true or خطأ=false
                                moveInst.operands.push_back(SIROperand::ConstantBool(
                                    valResult.constantValue == "true" || valResult.constantValue == "1"));
                            }
                            else if (valResult.type == SadTypeKind::String)
                            {
                                // (AR) القيمة النصية: ثابت نصي
                                // (EN) String value: string constant
                                moveInst.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                            }
                            else
                            {
                                try
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                                }
                                catch (...)
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                            }
                            b_.currentBlock_->addInstruction(moveInst);
                            valResult.registerName = reg;
                            valResult.isConstant = false;
                        }

                        if (b_.currentBlock_)
                        {
                            SIRInstruction storeInst(SIROpcode::STORE);
                            storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                            storeInst.operands.push_back(SIROperand::Global(staticFieldName, sfIt->second));
                            storeInst.comment = "Store static field: " + staticFieldName;
                            b_.currentBlock_->addInstruction(storeInst);
                        }
                        return BuildResult(valResult.registerName, valResult.type);
                    }
                }

                // (AR) بناء تعبير الكائن والقيمة
                // (EN) Build object expression and value
                auto objResult = buildExpression(memberAssignExpr->object.get());
                auto valResult = buildExpression(memberAssignExpr->value.get());

                // (AR) تجسيد القيمة إذا كانت ثابتة
                // (EN) Materialize value if constant
                if (valResult.isConstant && b_.currentBlock_)
                {
                    std::string reg = b_.newTempRegister();
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(reg, valResult.type);
                    if (valResult.type == SadTypeKind::String)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                    }
                    else if (valResult.type == SadTypeKind::Float)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                    }
                    else if (valResult.type == SadTypeKind::Boolean)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantBool(valResult.constantValue == "true" || valResult.constantValue == "1"));
                    }
                    else
                    {
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                        }
                        catch (const std::exception &)
                        {
#ifndef NDEBUG
                            std::cerr << "[SIR] تحذير: فشل تحويل '" << valResult.constantValue << "' إلى I64، القيمة الافتراضية 0\n";
#endif
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                    valResult.registerName = reg;
                    valResult.isConstant = false;
                }

                // (AR) تعليمة STORE لتخزين القيمة في العضو
                // (EN) STORE instruction to store value in member
                SIRInstruction storeInst;
                storeInst.opcode = SIROpcode::STORE;
                storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                storeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                storeInst.operands.push_back(SIROperand::ConstantString(memberAssignExpr->member));
                storeInst.comment = "member assign: " + memberAssignExpr->member;

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(storeInst);
                }

                return BuildResult(valResult.registerName, valResult.type);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
