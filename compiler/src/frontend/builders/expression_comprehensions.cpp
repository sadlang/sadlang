// ============================================================================
// sir_builder_expr_comprehensions.cpp
// ============================================================================
// Comprehension expression builders (list, dict, set comprehensions + generator)
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "sir_constants.h"
#include <set>
#include <functional>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // lowerMapComprehensionIterable — تهيئة تكرار الخريطة (مشترك بين الاستيعابات)
            // ============================================================================
            void ExpressionBuilder::lowerMapComprehensionIterable(BuildResult &iterResult,
                                                                  const std::string &valueVar,
                                                                  std::string &outValuesReg,
                                                                  SadTypeKind &outKeyType,
                                                                  SadTypeKind &outValueType)
            {
                outValuesReg.clear();
                outValueType = SadTypeKind::Integer;

                if (iterResult.type != SadTypeKind::Map)
                {
                    // (AR) مصفوفة عاديّة: نوع العنصر من نتيجة المصدر (أو Integer إن مجهولًا). لا قيم.
                    // (EN) Plain array: element type from the source result (or Integer if unknown). No values.
                    outKeyType = (iterResult.elementType != SadTypeKind::Void)
                                     ? iterResult.elementType
                                     : SadTypeKind::Integer;
                    return;
                }

                // (AR) نوع قيمة الخريطة كما يعقّبه buildExprMap في elementType — يُلتقَط قبل الدهس.
                // (EN) Map value type as tracked by buildExprMap in elementType — captured before overwrite.
                SadTypeKind mapValueType = iterResult.elementType;

                // (AR) __sad_map_keys ⇒ مصفوفة المفاتيح (نصوص دائمًا: الخريطة تُخزّنها strdup).
                // (EN) __sad_map_keys ⇒ keys array (always strings: the map strdup's them).
                std::string keysReg = b_.newTempRegister();
                {
                    SIRInstruction c(SIROpcode::CALL);
                    c.result = SIROperand::Register(keysReg, SadTypeKind::Array);
                    c.operands.push_back(SIROperand::ConstantString(kRuntimeMapKeys));
                    c.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(c);
                }

                if (!valueVar.empty())
                {
                    outValuesReg = b_.newTempRegister();
                    SIRInstruction c(SIROpcode::CALL);
                    c.result = SIROperand::Register(outValuesReg, SadTypeKind::Array);
                    c.operands.push_back(SIROperand::ConstantString(kRuntimeMapValues));
                    c.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(c);

                    // (AR) نوع القيمة يُشتقّ من تمثيلها المخزَّن الفعليّ (كما في حلقة «لكل»):
                    //      صحيح/منطقيّ ⇒ i64؛ نصّ/عشريّ ⇒ مؤشّر نصّ ⇒ String؛ مختلط ⇒ Integer.
                    // (EN) Value type derived from its actual stored representation (as in the «for» loop):
                    //      int/bool ⇒ i64; string/float ⇒ string pointer ⇒ String; mixed ⇒ Integer.
                    outValueType = (mapValueType == SadTypeKind::Integer ||
                                    mapValueType == SadTypeKind::Boolean)
                                       ? mapValueType
                                   : (mapValueType == SadTypeKind::String ||
                                      mapValueType == SadTypeKind::Float)
                                       ? SadTypeKind::String
                                       : SadTypeKind::Integer;
                }

                // (AR) استبدل المصدر بمصفوفة المفاتيح (عناصرها نصوص). / (EN) Replace source with the keys array (string elements).
                iterResult.registerName = keysReg;
                iterResult.type = SadTypeKind::Array;
                iterResult.elementType = SadTypeKind::String;
                iterResult.className.clear();
                iterResult.elementClassName.clear();
                outKeyType = SadTypeKind::String;
            }

            BuildResult ExpressionBuilder::buildExprListComp(AST::ListComprehensionExpr *listCompExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found ListComprehensionExpr" << std::endl;
#endif

                // (AR) تخصيص مصفوفة النتيجة عبر ARRAY_NEW (لا ALLOC) لتتوافق مع ARRAY_APPEND
                //      وبقيّة عمليّات المصفوفة المُلوَّنة في الخلفية (ISSUE-016).
                // (EN) Allocate result array via ARRAY_NEW (not ALLOC) so it is compatible with
                //      ARRAY_APPEND and the rest of the backend-lowered array ops (ISSUE-016).
                std::string resultArrReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ARRAY_NEW;
                allocInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "list comprehension result";

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // (AR) بناء التعبير القابل للتكرار
                // (EN) Build iterable expression
                auto iterResult = buildExpression(listCompExpr->iterable.get());

                // (AR) تهيئة تكرار الخريطة (يستبدل المصدر بمفاتيحها ويُصدِر قيمها إن طُلب فكّ زوج)
                // (EN) Prepare map iteration (replaces source with keys, emits values if pair-unpacking)
                std::string mapValuesReg;
                SadTypeKind keyElemType = SadTypeKind::Integer;
                SadTypeKind valueVarType = SadTypeKind::Integer;
                lowerMapComprehensionIterable(iterResult, listCompExpr->valueVariable,
                                              mapValuesReg, keyElemType, valueVarType);

                // (AR) إنشاء حلقة للتكرار (تُترجم إلى حلقة عداد)
                // (EN) Create iteration loop (lowered to counter loop)
                std::string idxReg = b_.newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                allocIdx.operands.push_back(SIROperand::ConstantI64(1));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocIdx);

                SIRInstruction storeZero;
                storeZero.opcode = SIROpcode::STORE;
                storeZero.operands.push_back(SIROperand::ConstantI64(0));
                storeZero.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(storeZero);

                // (AR) كتل الحلقة
                // (EN) Loop blocks
                std::string condLabel = b_.newLabel("lc_cond");
                std::string bodyLabel = b_.newLabel("lc_body");
                std::string exitLabel = b_.newLabel("lc_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                // (AR) كتلة الشرط — مقارنة العداد مع طول القائمة
                // (EN) Condition block — compare counter to list length
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(condBlock);
                b_.currentBlock_ = condBlock;

                // (AR) تحميل العداد الحالي
                // (EN) Load current counter
                std::string curIdxReg = b_.newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadIdx);

                // (AR) طول القابل للتكرار عبر ARRAY_LEN المُلوَّن في الخلفية (بدل CALL لرمز runtime)
                // (EN) Iterable length via backend-lowered ARRAY_LEN (instead of a runtime-symbol CALL)
                std::string lenReg = b_.newTempRegister();
                SIRInstruction callLen(SIROpcode::ARRAY_LEN);
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(callLen);

                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::LT,
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(lenReg, SadTypeKind::Integer)));
                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel)));
                }

                // (AR) كتلة الجسم
                // (EN) Body block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                b_.currentBlock_ = bodyBlock;

                b_.enterScope();

                // (AR) تحميل العنصر الحالي وتسجيل متغير الحلقة
                // (EN) Load current element and register loop variable
                std::string elemReg = b_.newTempRegister();
                SIRInstruction loadElem(SIROpcode::ARRAY_GET);
                loadElem.result = SIROperand::Register(elemReg, keyElemType);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = listCompExpr->variable;
                loopVar.type = keyElemType;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(loopVar);

                // (AR) فكّ زوج الخريطة: حمّل القيمة المقابلة وسجّل متغيّر القيمة (SSA — دومينوسه الجسم).
                // (EN) Map pair-unpack: load the matching value and register the value variable (SSA — dominated by body).
                if (!mapValuesReg.empty())
                {
                    std::string valElemReg = b_.newTempRegister();
                    SIRInstruction loadVal(SIROpcode::ARRAY_GET);
                    loadVal.result = SIROperand::Register(valElemReg, valueVarType);
                    loadVal.operands.push_back(SIROperand::Register(mapValuesReg, SadTypeKind::Array));
                    loadVal.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(loadVal);

                    VariableInfo valueLoopVar;
                    valueLoopVar.name = listCompExpr->valueVariable;
                    valueLoopVar.type = valueVarType;
                    valueLoopVar.registerName = valElemReg;
                    valueLoopVar.isMutable = false;
                    valueLoopVar.scopeLevel = b_.currentScopeLevel_;
                    b_.addVariable(valueLoopVar);
                }

                // (AR) فحص الشرط (إن وجد)
                // (EN) Check condition (if present)
                bool hasCondition = (listCompExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = b_.newLabel("lc_store");
                    incLabel = b_.newLabel("lc_inc");

                    auto condResult = buildExpression(listCompExpr->condition.get());
                    auto storeBlock2 = b_.createBasicBlock(storeLabel);
                    auto incBlock = b_.createBasicBlock(incLabel);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(storeLabel),
                            SIROperand::Label(incLabel)));
                    }

                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(storeBlock2);
                    b_.currentBlock_ = storeBlock2;
                }

                // (AR) بناء تعبير العنصر وإضافته للمصفوفة
                // (EN) Build element expression and append to array
                auto elemExprResult = buildExpression(listCompExpr->element.get());

                // (AR) إضافة العنصر عبر ARRAY_APPEND المُلوَّن في الخلفية (لا `CALL __sad_array_push`
                //      غير المعرَّف الذي كان يُفشِل الربط — ISSUE-016). الترتيب: [المصفوفة، القيمة].
                // (EN) Append via backend-lowered ARRAY_APPEND (not the undefined `CALL __sad_array_push`
                //      that broke linking — ISSUE-016). Operand order: [array, value].
                SIRInstruction appendInst(SIROpcode::ARRAY_APPEND);
                appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                appendInst.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(appendInst);

                if (hasCondition)
                {
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
                    }
                    // (AR) كتلة زيادة العداد
                    // (EN) Increment block
                    auto incBlock2 = b_.createBasicBlock(incLabel);
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(incBlock2);
                    b_.currentBlock_ = incBlock2;
                }

                // (AR) زيادة العداد
                // (EN) Increment counter
                std::string nextIdxReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1)));

                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(storeIdx);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                b_.exitScope();

                // (AR) كتلة الخروج
                // (EN) Exit block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(exitBlock);
                b_.currentBlock_ = exitBlock;

                // (AR) نمرّر نوع عنصر الناتج إلى نتيجة المصفوفة حتى يعمل الوصول المفهرَس بنوعه
                //      الصحيح (نصّ يُطبع كنصّ لا كمؤشّر). يطابق سلوك المصفوفة الحرفيّة النصّيّة.
                //      (حدّ منفصل: طبع المصفوفة كاملةً يبقى عدديًّا عبر __sad_array_to_string.)
                // (EN) Propagate the output element type to the array result so indexed access
                //      is correctly typed (a string prints as a string, not a pointer) — matching
                //      a string array literal. (Separate limit: whole-array print stays integer via
                //      the untyped __sad_array_to_string helper.)
                BuildResult listResult(resultArrReg, SadTypeKind::Array);
                listResult.elementType = elemExprResult.type;
                return listResult;
            }

            // ============================================================================
            // buildExprDictComp
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprDictComp(AST::DictComprehensionExpr *dictCompExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found DictComprehensionExpr" << std::endl;
#endif

                // (AR) إنشاء خريطة النتيجة عبر `__sad_map_create` (نفس مسار الخريطة الحرفيّة العامل)
                //      بدل `ALLOC` + `CALL __sad_dict_set` غير المعرَّف الذي كان يُفشِل الربط (ISSUE-017).
                // (EN) Create the result map via `__sad_map_create` (same path as the working map literal)
                //      instead of `ALLOC` + the undefined `CALL __sad_dict_set` that broke linking (ISSUE-017).
                std::string resultMapReg = b_.newTempRegister();
                SIRInstruction allocInst(SIROpcode::CALL);
                allocInst.result = SIROperand::Register(resultMapReg, SadTypeKind::Map);
                allocInst.operands.push_back(SIROperand::ConstantString("__sad_map_create"));
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "dict comprehension result";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocInst);

                // (AR) بناء التعبير القابل للتكرار
                // (EN) Build iterable expression
                auto iterResult = buildExpression(dictCompExpr->iterable.get());

                // (AR) تهيئة تكرار الخريطة (مفاتيح + قيم إن طُلب فكّ زوج)
                // (EN) Prepare map iteration (keys + values if pair-unpacking)
                std::string mapValuesReg;
                SadTypeKind keyElemType = SadTypeKind::Integer;
                SadTypeKind valueVarType = SadTypeKind::Integer;
                lowerMapComprehensionIterable(iterResult, dictCompExpr->valueVariable,
                                              mapValuesReg, keyElemType, valueVarType);

                // (AR) تخصيص عداد الحلقة
                // (EN) Allocate loop counter
                std::string idxReg = b_.newTempRegister();
                SIRInstruction allocIdx;
                allocIdx.opcode = SIROpcode::ALLOC;
                allocIdx.result = SIROperand::Register(idxReg, SadTypeKind::Integer);
                allocIdx.operands.push_back(SIROperand::ConstantI64(1));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocIdx);

                SIRInstruction storeZero;
                storeZero.opcode = SIROpcode::STORE;
                storeZero.operands.push_back(SIROperand::ConstantI64(0));
                storeZero.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(storeZero);

                // (AR) كتل الحلقة
                // (EN) Loop blocks
                std::string condLabel = b_.newLabel("dc_cond");
                std::string bodyLabel = b_.newLabel("dc_body");
                std::string exitLabel = b_.newLabel("dc_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                // (AR) كتلة الشرط
                // (EN) Condition block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(condBlock);
                b_.currentBlock_ = condBlock;

                std::string curIdxReg = b_.newTempRegister();
                SIRInstruction loadIdx;
                loadIdx.opcode = SIROpcode::LOAD;
                loadIdx.result = SIROperand::Register(curIdxReg, SadTypeKind::Integer);
                loadIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadIdx);

                std::string lenReg = b_.newTempRegister();
                SIRInstruction callLen(SIROpcode::ARRAY_LEN);
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(callLen);

                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::LT,
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(lenReg, SadTypeKind::Integer)));
                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel)));
                }

                // (AR) كتلة الجسم
                // (EN) Body block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                b_.currentBlock_ = bodyBlock;

                b_.enterScope();

                // (AR) تحميل العنصر وتسجيل متغير الحلقة
                // (EN) Load element and register loop variable
                std::string elemReg = b_.newTempRegister();
                SIRInstruction loadElem(SIROpcode::ARRAY_GET);
                loadElem.result = SIROperand::Register(elemReg, keyElemType);
                loadElem.operands.push_back(SIROperand::Register(iterResult.registerName, iterResult.type));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);

                VariableInfo loopVar;
                loopVar.name = dictCompExpr->variable;
                loopVar.type = keyElemType;
                loopVar.registerName = elemReg;
                loopVar.isMutable = false;
                loopVar.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(loopVar);

                // (AR) فكّ زوج الخريطة: حمّل القيمة المقابلة وسجّل متغيّر القيمة. / (EN) Map pair-unpack: load & register the value variable.
                if (!mapValuesReg.empty())
                {
                    std::string valElemReg = b_.newTempRegister();
                    SIRInstruction loadVal(SIROpcode::ARRAY_GET);
                    loadVal.result = SIROperand::Register(valElemReg, valueVarType);
                    loadVal.operands.push_back(SIROperand::Register(mapValuesReg, SadTypeKind::Array));
                    loadVal.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(loadVal);

                    VariableInfo valueLoopVar;
                    valueLoopVar.name = dictCompExpr->valueVariable;
                    valueLoopVar.type = valueVarType;
                    valueLoopVar.registerName = valElemReg;
                    valueLoopVar.isMutable = false;
                    valueLoopVar.scopeLevel = b_.currentScopeLevel_;
                    b_.addVariable(valueLoopVar);
                }

                // (AR) فحص الشرط (إن وجد)
                // (EN) Check condition (if present)
                bool hasCondition = (dictCompExpr->condition != nullptr);
                std::string storeLabel, incLabel;

                if (hasCondition)
                {
                    storeLabel = b_.newLabel("dc_store");
                    incLabel = b_.newLabel("dc_inc");

                    auto condResult = buildExpression(dictCompExpr->condition.get());
                    auto storeBlock2 = b_.createBasicBlock(storeLabel);
                    auto incBlock = b_.createBasicBlock(incLabel);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(storeLabel),
                            SIROperand::Label(incLabel)));
                    }

                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(storeBlock2);
                    b_.currentBlock_ = storeBlock2;
                }

                // (AR) بناء تعبيرات المفتاح والقيمة وإضافتها للخريطة
                // (EN) Build key and value expressions and add to map
                auto keyResult = buildExpression(dictCompExpr->key.get());
                auto valResult = buildExpression(dictCompExpr->value.get());

                // (AR) تجسيد الثوابت إلى سجلات (نظير الخريطة الحرفيّة)
                // (EN) Materialize constants to registers (mirrors the map literal)
                auto materialize = [&](BuildResult &res) -> SIROperand
                {
                    if (res.isConstant && b_.currentBlock_)
                    {
                        std::string reg = b_.newTempRegister();
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(reg, res.type);
                        if (res.type == SadTypeKind::String)
                            moveInst.operands.push_back(SIROperand::ConstantString(res.constantValue));
                        else if (res.type == SadTypeKind::Float)
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(res.constantValue)));
                        else if (res.type == SadTypeKind::Boolean)
                            moveInst.operands.push_back(SIROperand::ConstantBool(res.constantValue == "true" || res.constantValue == "1"));
                        else
                        {
                            try { moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(res.constantValue))); }
                            catch (const std::exception &) { moveInst.operands.push_back(SIROperand::ConstantI64(0)); }
                        }
                        b_.currentBlock_->addInstruction(moveInst);
                        res.registerName = reg;
                        res.isConstant = false;
                    }
                    return SIROperand::Register(res.registerName, res.type);
                };
                SIROperand keyOp = materialize(keyResult);
                SIROperand valOp = materialize(valResult);

                // (AR) المفتاح يُخزَّن كنصّ — حوّل العدديّ/العشريّ/المنطقيّ إلى نص (ISSUE-044)
                // (EN) Keys are stored as strings — convert int/float/bool key to a string (ISSUE-044)
                if (keyResult.type == SadTypeKind::Integer ||
                    keyResult.type == SadTypeKind::Float ||
                    keyResult.type == SadTypeKind::Boolean)
                {
                    std::string keyStrReg = b_.newTempRegister();
                    SIROpcode kc = (keyResult.type == SadTypeKind::Float)     ? SIROpcode::F64_TO_STRING
                                   : (keyResult.type == SadTypeKind::Boolean) ? SIROpcode::BOOL_TO_STRING
                                                                              : SIROpcode::I64_TO_STRING;
                    SIRInstruction keyConv(kc);
                    keyConv.result = SIROperand::Register(keyStrReg, SadTypeKind::String);
                    keyConv.operands.push_back(keyOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(keyConv);
                    keyOp = SIROperand::Register(keyStrReg, SadTypeKind::String);
                }

                // (AR) إدراج الزوج عبر `__sad_map_set_typed` (نفس الخريطة الحرفيّة): القيمة كـi64 + وسم نوع.
                // (EN) Insert pair via `__sad_map_set_typed` (same as map literal): value as i64 + type tag.
                SIRInstruction setInst(SIROpcode::CALL);
                setInst.operands.push_back(SIROperand::ConstantString("__sad_map_set_typed"));
                setInst.operands.push_back(SIROperand::Register(resultMapReg, SadTypeKind::Map));
                setInst.operands.push_back(keyOp);
                int typeTag = 0; // SVAL_STRING=0, SVAL_INT=1, SVAL_FLOAT=2, SVAL_BOOL=3
                if (valResult.type == SadTypeKind::Integer)
                {
                    setInst.operands.push_back(valOp);
                    typeTag = 1;
                }
                else if (valResult.type == SadTypeKind::Float)
                {
                    std::string strReg = b_.newTempRegister();
                    SIRInstruction toStrInst(SIROpcode::F64_TO_STRING);
                    toStrInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                    toStrInst.operands.push_back(valOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(toStrInst);
                    setInst.operands.push_back(SIROperand::Register(strReg, SadTypeKind::String));
                    typeTag = 0;
                }
                else if (valResult.type == SadTypeKind::Boolean)
                {
                    setInst.operands.push_back(valOp);
                    typeTag = 3;
                }
                else
                {
                    setInst.operands.push_back(valOp);
                    typeTag = 0;
                }
                setInst.operands.push_back(SIROperand::ConstantI64(typeTag));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(setInst);

                if (hasCondition)
                {
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));
                    }
                    auto incBlock2 = b_.createBasicBlock(incLabel);
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(incBlock2);
                    b_.currentBlock_ = incBlock2;
                }

                // (AR) زيادة العداد
                // (EN) Increment counter
                std::string nextIdxReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1)));

                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(storeIdx);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));
                }

                b_.exitScope();

                // (AR) كتلة الخروج
                // (EN) Exit block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(exitBlock);
                b_.currentBlock_ = exitBlock;

                return BuildResult(resultMapReg, SadTypeKind::Map);
            }

            // ============================================================================
            // buildExprSetComp
            // ============================================================================
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
