// ============================================================================
// sir_builder_expr_comprehensions.cpp
// ============================================================================
// Comprehension expression builders (list, dict, set comprehensions + generator)
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "sir_constants.h"
#include "sad_debug_log.h"
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
                    //      صحيح/منطقيّ/عشريّ ⇒ بتّاتُه i64 ⇒ النوعُ نفسُه؛ نصّ ⇒ مؤشّر ⇒ String؛
                    //      مختلط ⇒ Integer. ⚠️ مرآةٌ لتمثيلِ التخزين — يجبُ أن يبقى موافقًا
                    //      لنظيرِه في `statement_for_range.cpp` (انظرِ التحذيرَ هناك).
                    // (EN) Value type derived from its actual stored representation (as in the «for»
                    //      loop): int/bool/float ⇒ raw i64 bits ⇒ same type; string ⇒ pointer ⇒
                    //      String; mixed ⇒ Integer. ⚠️ Mirrors the storage representation — must stay
                    //      in step with its twin in `statement_for_range.cpp` (see the warning there).
                    outValueType = (mapValueType == SadTypeKind::Integer ||
                                    mapValueType == SadTypeKind::Boolean ||
                                    mapValueType == SadTypeKind::Float ||
                                    mapValueType == SadTypeKind::Any)
                                       ? mapValueType
                                   : (mapValueType == SadTypeKind::String)
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

            ExpressionBuilder::ComprehensionSource
            ExpressionBuilder::prepareComprehensionSource(Sad::AST::Expression *iterable,
                                                          const std::string &valueVar)
            {
                ComprehensionSource src;

                // (AR) كشف مُكرِّر المدى (مثل `لكل س في 1..5`): نخفضه حسابيًّا بلا تجسيد
                //      مصفوفة، تمامًا كحلقة for-range التي لا تبني قيمة مدى. بدونه كان
                //      يُبنى المدى صفًّا [بداية، نهاية] فيُكرَّر كعنصرين بدل التوسيع.
                // (EN) Detect a range iterable (e.g. `for x in 1..5`): lower it
                //      arithmetically with no materialized array, exactly like the
                //      for-range loop which never builds a range value. Without this the
                //      range was built as a 2-tuple and iterated as two elements.
                auto *rangeIter = dynamic_cast<Sad::AST::RangeExpr *>(iterable);
                if (rangeIter)
                {
                    src.isRange = true;

                    // (AR) تجسيد حدّي المدى في سجلّين (قد يكونان ثابتين كـ 1..5)
                    // (EN) Materialize both range bounds into registers (may be constants)
                    auto materializeInt = [&](Sad::AST::Expression *e) -> std::string
                    {
                        auto res = buildExpression(e);
                        if (res.isConstant)
                        {
                            std::string reg = b_.newTempRegister();
                            SIRInstruction mv(SIROpcode::MOVE);
                            mv.result = SIROperand::Register(reg, SadTypeKind::Integer);
                            try
                            {
                                mv.operands.push_back(SIROperand::ConstantI64(std::stoll(res.constantValue)));
                            }
                            catch (const std::exception &)
                            {
                                mv.operands.push_back(SIROperand::ConstantI64(0));
                            }
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(mv);
                            return reg;
                        }
                        return res.registerName;
                    };

                    src.startReg = materializeInt(rangeIter->start.get());
                    std::string endReg = materializeInt(rangeIter->end.get());

                    // (AR) اتّجاه المدى شامل الطرفين بلا تفريعات (يطابق for-range تصاعدًا
                    //      وتنازلًا، لثوابت أو متغيّرات): فرق = نهاية − بداية؛ قناع الإشارة =
                    //      فرق >> 63 (‏0 موجبًا، ‏−1 سالبًا)؛ |فرق| = (فرق ⊕ قناع) − قناع؛
                    //      الطول = |فرق| + 1؛ الخطوة = 1 + (قناع << 1) ⇒ +1 تصاعدًا و−1 تنازلًا.
                    // (EN) Branchless inclusive range direction (matches for-range, ascending
                    //      or descending, constant or variable): diff = end − start; sign mask =
                    //      diff >> 63 (0 if ≥0, −1 if <0); |diff| = (diff ⊕ mask) − mask;
                    //      length = |diff| + 1; step = 1 + (mask << 1) ⇒ +1 up, −1 down.
                    std::string diffReg = b_.newTempRegister();
                    std::string maskReg = b_.newTempRegister();
                    std::string xorReg = b_.newTempRegister();
                    std::string absReg = b_.newTempRegister();
                    std::string shiftReg = b_.newTempRegister();
                    src.lenReg = b_.newTempRegister();
                    src.stepReg = b_.newTempRegister();
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::SUB_I64,
                            SIROperand::Register(diffReg, SadTypeKind::Integer),
                            SIROperand::Register(endReg, SadTypeKind::Integer),
                            SIROperand::Register(src.startReg, SadTypeKind::Integer)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::SAR,
                            SIROperand::Register(maskReg, SadTypeKind::Integer),
                            SIROperand::Register(diffReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(63)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::XOR,
                            SIROperand::Register(xorReg, SadTypeKind::Integer),
                            SIROperand::Register(diffReg, SadTypeKind::Integer),
                            SIROperand::Register(maskReg, SadTypeKind::Integer)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::SUB_I64,
                            SIROperand::Register(absReg, SadTypeKind::Integer),
                            SIROperand::Register(xorReg, SadTypeKind::Integer),
                            SIROperand::Register(maskReg, SadTypeKind::Integer)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::ADD_I64,
                            SIROperand::Register(src.lenReg, SadTypeKind::Integer),
                            SIROperand::Register(absReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(1)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::SHL,
                            SIROperand::Register(shiftReg, SadTypeKind::Integer),
                            SIROperand::Register(maskReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(1)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::ADD_I64,
                            SIROperand::Register(src.stepReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(1),
                            SIROperand::Register(shiftReg, SadTypeKind::Integer)));
                    }
                    return src;
                }

                // (AR) المسار العاديّ: مصفوفة/خريطة
                // (EN) Normal path: array/map
                auto iterResult = buildExpression(iterable);
                lowerMapComprehensionIterable(iterResult, valueVar,
                                              src.mapValuesReg, src.keyElemType, src.valueVarType);
                src.iterRegName = iterResult.registerName;
                src.iterType = iterResult.type;
                return src;
            }

            std::string ExpressionBuilder::comprehensionSourceLength(const ComprehensionSource &src)
            {
                if (src.isRange)
                    return src.lenReg;

                std::string lenReg = b_.newTempRegister();
                SIRInstruction callLen(SIROpcode::ARRAY_LEN);
                callLen.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                callLen.operands.push_back(SIROperand::Register(src.iterRegName, src.iterType));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(callLen);
                return lenReg;
            }

            std::string ExpressionBuilder::comprehensionSourceElement(const ComprehensionSource &src,
                                                                      const std::string &curIdxReg)
            {
                std::string elemReg = b_.newTempRegister();
                if (src.isRange)
                {
                    // (AR) القيمة = بداية + العدّاد × الخطوة (±1) ⇒ تصاعد أو تنازل
                    // (EN) value = start + counter × step (±1) ⇒ ascending or descending
                    std::string scaledReg = b_.newTempRegister();
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::MUL_I64,
                            SIROperand::Register(scaledReg, SadTypeKind::Integer),
                            SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                            SIROperand::Register(src.stepReg, SadTypeKind::Integer)));
                        b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                            SIROpcode::ADD_I64,
                            SIROperand::Register(elemReg, SadTypeKind::Integer),
                            SIROperand::Register(src.startReg, SadTypeKind::Integer),
                            SIROperand::Register(scaledReg, SadTypeKind::Integer)));
                    }
                    return elemReg;
                }

                SIRInstruction loadElem(SIROpcode::ARRAY_GET);
                loadElem.result = SIROperand::Register(elemReg, src.keyElemType);
                loadElem.operands.push_back(SIROperand::Register(src.iterRegName, src.iterType));
                loadElem.operands.push_back(SIROperand::Register(curIdxReg, SadTypeKind::Integer));
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(loadElem);
                return elemReg;
            }

            BuildResult ExpressionBuilder::buildExprListComp(AST::ListComprehensionExpr *listCompExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found ListComprehensionExpr");
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

                // (AR) تهيئة مصدر التكرار (مدى ⇒ حسابيّ، أو مصفوفة/خريطة) عبر المساعِد المشترك.
                // (EN) Prepare the iteration source (range ⇒ arithmetic, or array/map) via the shared helper.
                ComprehensionSource src = prepareComprehensionSource(
                    listCompExpr->iterable.get(), listCompExpr->valueVariable);
                std::string mapValuesReg = src.mapValuesReg;
                SadTypeKind keyElemType = src.keyElemType;
                SadTypeKind valueVarType = src.valueVarType;

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

                // (AR) طول القابل للتكرار (المدى: محسوب مسبقًا؛ وإلّا ARRAY_LEN) عبر المساعِد.
                // (EN) Iterable length (range: precomputed; else ARRAY_LEN) via the helper.
                std::string lenReg = comprehensionSourceLength(src);

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

                // (AR) العنصر الحالي (المدى: بداية+عدّاد×خطوة؛ وإلّا ARRAY_GET) عبر المساعِد.
                // (EN) Current element (range: start+counter×step; else ARRAY_GET) via the helper.
                std::string elemReg = comprehensionSourceElement(src, curIdxReg);

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
                // (AR) ننشر نوعَ العنصر المُنتَج إلى معامل المصفوفة: عنصرٌ ديناميّ (Any، من
                //      مصدرٍ مختلط) ⇒ تُعلّب الخلفيّةُ العنصرَ في %SadDyn، متّسقةً مع وسم
                //      النتيجة Any (سطر 477). لولاه: النتيجةُ موسومةٌ Any لكنّ خاناتها خامٌ
                //      ⇒ فكُّ القراءة/الطباعة يعبث بمؤشّرٍ ⇒ انهيار.
                // (EN) Propagate the produced element type to the array operand: a dynamic
                //      element (Any, from a heterogeneous source) ⇒ the backend boxes it into
                //      a %SadDyn, consistent with the result's Any tag (line 477). Without this
                //      the result is tagged Any but its slots are raw ⇒ the read/print unbox
                //      dereferences a non-pointer ⇒ crash.
                SIROperand compArrOp = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                compArrOp.elementType = elemExprResult.type;
                appendInst.operands.push_back(compArrOp);
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
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found DictComprehensionExpr");
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

                // (AR) تهيئة مصدر التكرار (مدى ⇒ حسابيّ، أو مصفوفة/خريطة) عبر المساعِد المشترك.
                // (EN) Prepare the iteration source (range ⇒ arithmetic, or array/map) via the shared helper.
                ComprehensionSource src = prepareComprehensionSource(
                    dictCompExpr->iterable.get(), dictCompExpr->valueVariable);
                std::string mapValuesReg = src.mapValuesReg;
                SadTypeKind keyElemType = src.keyElemType;
                SadTypeKind valueVarType = src.valueVarType;

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

                std::string lenReg = comprehensionSourceLength(src);

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

                // (AR) تحميل العنصر وتسجيل متغير الحلقة (المدى: حسابيّ؛ وإلّا ARRAY_GET) عبر المساعِد.
                // (EN) Load element and register loop variable (range: arithmetic; else ARRAY_GET) via helper.
                std::string elemReg = comprehensionSourceElement(src, curIdxReg);

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
