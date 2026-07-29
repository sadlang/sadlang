// ============================================================================
// sir_builder_expr_comp2.cpp - SetComp and Generator expression builders
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
            BuildResult ExpressionBuilder::buildExprSetComp(AST::SetComprehensionExpr *setCompExpr)
            {
                // (AR) المجموعة تُمثَّل كمصفوفة بعناصر فريدة (مثل المفسر)
                // (EN) Set represented as array with unique elements (like interpreter)
                std::string resultSetReg = b_.newTempRegister();
                SIRInstruction allocInst(SIROpcode::ARRAY_NEW);
                allocInst.result = SIROperand::Register(resultSetReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.operands.push_back(SIROperand::ConstantI64(0));
                allocInst.comment = "set comprehension result (dedup array)";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(allocInst);

                // (AR) تهيئة مصدر التكرار (مدى ⇒ حسابيّ، أو مصفوفة/خريطة) عبر المساعِد المشترك.
                // (EN) Prepare the iteration source (range ⇒ arithmetic, or array/map) via the shared helper.
                ComprehensionSource src = prepareComprehensionSource(
                    setCompExpr->iterable.get(), setCompExpr->valueVariable);
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

                // (AR) علَم "موجود" وعدّاد المسح الداخليّ — يُخصَّصان مرّة واحدة في كتلة الدخول
                //      (قبل الحلقة) تفاديًا لتسريب مكدس من تخصيص متكرّر داخل الجسم (مراجعة IR).
                //      التصفير يبقى داخل الحلقة عبر STORE في كتلة الناتج.
                // (EN) "found" flag and inner scan counter — allocated ONCE in the entry block
                //      (before the loop) to avoid a stack leak from per-iteration allocas; the
                //      reset stays inside the loop via STORE in the value block.
                std::string foundReg = b_.newTempRegister();
                {
                    SIRInstruction a(SIROpcode::ALLOC);
                    a.result = SIROperand::Register(foundReg, SadTypeKind::Integer);
                    a.operands.push_back(SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(a);
                }
                std::string jdxReg = b_.newTempRegister();
                {
                    SIRInstruction a(SIROpcode::ALLOC);
                    a.result = SIROperand::Register(jdxReg, SadTypeKind::Integer);
                    a.operands.push_back(SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(a);
                }

                // (AR) كتل الحلقة
                // (EN) Loop blocks
                std::string condLabel = b_.newLabel("sc_cond");
                std::string bodyLabel = b_.newLabel("sc_body");
                std::string exitLabel = b_.newLabel("sc_exit");

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

                // (AR) العنصر الحالي (المدى: حسابيّ؛ وإلّا ARRAY_GET) عبر المساعِد المشترك.
                // (EN) Current element (range: arithmetic; else ARRAY_GET) via the shared helper.
                std::string elemReg = comprehensionSourceElement(src, curIdxReg);

                VariableInfo loopVar;
                loopVar.name = setCompExpr->variable;
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
                    valueLoopVar.name = setCompExpr->valueVariable;
                    valueLoopVar.type = valueVarType;
                    valueLoopVar.registerName = valElemReg;
                    valueLoopVar.isMutable = false;
                    valueLoopVar.scopeLevel = b_.currentScopeLevel_;
                    b_.addVariable(valueLoopVar);
                }

                // (AR) إزالة التكرار: نبني الناتج ثمّ نمسح مصفوفة النتيجة، ونضيف فقط إن لم يكن موجودًا
                //      (المجموعة = مصفوفة فريدة، مطابقة للمفسّر). كلّ العمليّات أوكواد مصفوفة مُلوَّنة
                //      في الخلفيّة (ARRAY_NEW/LEN/GET/APPEND) بدل رموز وقت تشغيل غير معرَّفة — RFC 25 م1ب.
                //      حدّ معروف: المقارنة `EQ` عدديّة (كبقيّة بنية الاستيعابات عدديّة النوع)، فإزالة
                //      التكرار صحيحة للأعداد؛ مجموعات النصوص/العشريّ ستتباعد صامتًا عن المفسّر حتى
                //      يُعمَّم النوع في الاستيعابات كلّها (يُتابَع مع التعميم النوعيّ للقائمة/القاموس).
                // (EN) Dedup: build the output, scan the result array, append only if absent
                //      (a set is a unique array, like the interpreter). All backend-lowered array ops.
                //      Known limit: the `EQ` compare is integer (like the whole comprehension infra),
                //      so dedup is correct for integers; string/double sets diverge silently until the
                //      comprehension pipeline becomes type-generic (tracked with list/dict generalization).

                bool hasCondition = (setCompExpr->condition != nullptr);
                std::string incLabel = b_.newLabel("sc_inc");
                std::string valLabel = b_.newLabel("sc_val");
                std::string scanCondLabel = b_.newLabel("sc_scan_cond");
                std::string scanBodyLabel = b_.newLabel("sc_scan_body");
                std::string scanFoundLabel = b_.newLabel("sc_scan_found");
                std::string scanNextLabel = b_.newLabel("sc_scan_next");
                std::string scanDoneLabel = b_.newLabel("sc_scan_done");
                std::string appendLabel = b_.newLabel("sc_append");

                // (AR) الشرط الاختياريّ: يتفرّع إلى بناء الناتج أو إلى الزيادة مباشرة.
                // (EN) Optional condition: branch to output-build or straight to increment.
                if (hasCondition)
                {
                    auto condResult = buildExpression(setCompExpr->condition.get());
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                            SIROperand::Label(valLabel),
                            SIROperand::Label(incLabel)));
                }
                else if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(valLabel)));
                }

                // (AR) كتلة الناتج: ابنِ التعبير، صفّر العلَم والعدّاد، ثمّ ابدأ المسح.
                // (EN) Value block: build the output, reset flag+counter, then start the scan.
                auto valBlock = b_.createBasicBlock(valLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(valBlock);
                b_.currentBlock_ = valBlock;
                auto elemExprResult = buildExpression(setCompExpr->expression.get());
                {
                    SIRInstruction s;
                    s.opcode = SIROpcode::STORE;
                    s.operands.push_back(SIROperand::ConstantI64(0));
                    s.operands.push_back(SIROperand::Register(foundReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(s);
                }
                {
                    SIRInstruction s;
                    s.opcode = SIROpcode::STORE;
                    s.operands.push_back(SIROperand::ConstantI64(0));
                    s.operands.push_back(SIROperand::Register(jdxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(s);
                }
                b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(scanCondLabel)));

                // (AR) شرط المسح: العدّاد الداخليّ < طول النتيجة؟
                // (EN) Scan condition: inner counter < result length?
                auto scanCondBlock = b_.createBasicBlock(scanCondLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(scanCondBlock);
                b_.currentBlock_ = scanCondBlock;
                std::string jcurReg = b_.newTempRegister();
                {
                    SIRInstruction l;
                    l.opcode = SIROpcode::LOAD;
                    l.result = SIROperand::Register(jcurReg, SadTypeKind::Integer);
                    l.operands.push_back(SIROperand::Register(jdxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(l);
                }
                std::string rlenReg = b_.newTempRegister();
                {
                    SIRInstruction cl(SIROpcode::ARRAY_LEN);
                    cl.result = SIROperand::Register(rlenReg, SadTypeKind::Integer);
                    cl.operands.push_back(SIROperand::Register(resultSetReg, SadTypeKind::Array));
                    b_.currentBlock_->addInstruction(cl);
                }
                std::string scmpReg = b_.newTempRegister();
                b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                    SIROpcode::LT,
                    SIROperand::Register(scmpReg, SadTypeKind::Boolean),
                    SIROperand::Register(jcurReg, SadTypeKind::Integer),
                    SIROperand::Register(rlenReg, SadTypeKind::Integer)));
                b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(scmpReg, SadTypeKind::Boolean),
                    SIROperand::Label(scanBodyLabel),
                    SIROperand::Label(scanDoneLabel)));

                // (AR) جسم المسح: قارن عنصر النتيجة بالناتج؛ إن تساويا فالعنصر موجود.
                // (EN) Scan body: compare a result element to the output; equal ⇒ present.
                auto scanBodyBlock = b_.createBasicBlock(scanBodyLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(scanBodyBlock);
                b_.currentBlock_ = scanBodyBlock;
                std::string relReg = b_.newTempRegister();
                {
                    SIRInstruction le(SIROpcode::ARRAY_GET);
                    le.result = SIROperand::Register(relReg, SadTypeKind::Integer);
                    le.operands.push_back(SIROperand::Register(resultSetReg, SadTypeKind::Array));
                    le.operands.push_back(SIROperand::Register(jcurReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(le);
                }
                std::string eqReg = b_.newTempRegister();
                b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                    SIROpcode::EQ,
                    SIROperand::Register(eqReg, SadTypeKind::Boolean),
                    SIROperand::Register(relReg, SadTypeKind::Integer),
                    SIROperand::Register(elemExprResult.registerName, elemExprResult.type)));
                b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(eqReg, SadTypeKind::Boolean),
                    SIROperand::Label(scanFoundLabel),
                    SIROperand::Label(scanNextLabel)));

                // (AR) وُجد: ارفع العلَم وأنهِ المسح.
                // (EN) Found: raise the flag and end the scan.
                auto scanFoundBlock = b_.createBasicBlock(scanFoundLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(scanFoundBlock);
                b_.currentBlock_ = scanFoundBlock;
                {
                    SIRInstruction s;
                    s.opcode = SIROpcode::STORE;
                    s.operands.push_back(SIROperand::ConstantI64(1));
                    s.operands.push_back(SIROperand::Register(foundReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(s);
                }
                b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(scanDoneLabel)));

                // (AR) التالي: زِد عدّاد المسح وعُد لشرط المسح.
                // (EN) Next: increment scan counter and loop back.
                auto scanNextBlock = b_.createBasicBlock(scanNextLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(scanNextBlock);
                b_.currentBlock_ = scanNextBlock;
                std::string jnextReg = b_.newTempRegister();
                b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                    SIROpcode::ADD_I64,
                    SIROperand::Register(jnextReg, SadTypeKind::Integer),
                    SIROperand::Register(jcurReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(1)));
                {
                    SIRInstruction s;
                    s.opcode = SIROpcode::STORE;
                    s.operands.push_back(SIROperand::Register(jnextReg, SadTypeKind::Integer));
                    s.operands.push_back(SIROperand::Register(jdxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(s);
                }
                b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(scanCondLabel)));

                // (AR) انتهى المسح: إن لم يوجد العنصر فأضِفه.
                // (EN) Scan done: if the element was absent, append it.
                auto scanDoneBlock = b_.createBasicBlock(scanDoneLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(scanDoneBlock);
                b_.currentBlock_ = scanDoneBlock;
                std::string fReg = b_.newTempRegister();
                {
                    SIRInstruction l;
                    l.opcode = SIROpcode::LOAD;
                    l.result = SIROperand::Register(fReg, SadTypeKind::Integer);
                    l.operands.push_back(SIROperand::Register(foundReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(l);
                }
                std::string isZeroReg = b_.newTempRegister();
                b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                    SIROpcode::EQ,
                    SIROperand::Register(isZeroReg, SadTypeKind::Boolean),
                    SIROperand::Register(fReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(0)));
                b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(isZeroReg, SadTypeKind::Boolean),
                    SIROperand::Label(appendLabel),
                    SIROperand::Label(incLabel)));

                // (AR) الإضافة عبر ARRAY_APPEND المُلوَّن (لا CALL __sad_set_add غير المعرَّف).
                // (EN) Append via backend-lowered ARRAY_APPEND (not the undefined CALL __sad_set_add).
                auto appendBlock = b_.createBasicBlock(appendLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(appendBlock);
                b_.currentBlock_ = appendBlock;
                {
                    SIRInstruction ap(SIROpcode::ARRAY_APPEND);
                    // (AR) ننشر نوعَ العنصر: Any (مصدرٌ مختلط) ⇒ تُعلّب الخلفيّةُ العنصرَ،
                    //      متّسقةً مع وسم النتيجة Any (سطر 390) — وإلّا انهيارٌ عند القراءة.
                    // (EN) Propagate element type: Any (heterogeneous source) ⇒ backend boxes,
                    //      consistent with the result's Any tag (line 390) — else read crash.
                    SIROperand setArrOp = SIROperand::Register(resultSetReg, SadTypeKind::Array);
                    setArrOp.elementType = elemExprResult.type;
                    ap.operands.push_back(setArrOp);
                    ap.operands.push_back(SIROperand::Register(elemExprResult.registerName, elemExprResult.type));
                    b_.currentBlock_->addInstruction(ap);
                }
                b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(incLabel)));

                // (AR) كتلة الزيادة: زِد العدّاد الخارجيّ وعُد لشرط الحلقة.
                // (EN) Increment block: advance the outer counter and loop.
                auto incBlock = b_.createBasicBlock(incLabel);
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(incBlock);
                b_.currentBlock_ = incBlock;
                std::string nextIdxReg = b_.newTempRegister();
                b_.currentBlock_->addInstruction(SIRInstruction::Binary(
                    SIROpcode::ADD_I64,
                    SIROperand::Register(nextIdxReg, SadTypeKind::Integer),
                    SIROperand::Register(curIdxReg, SadTypeKind::Integer),
                    SIROperand::ConstantI64(1)));
                {
                    SIRInstruction storeIdx;
                    storeIdx.opcode = SIROpcode::STORE;
                    storeIdx.operands.push_back(SIROperand::Register(nextIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(idxReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(storeIdx);
                }
                b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                b_.exitScope();

                // (AR) كتلة الخروج
                // (EN) Exit block
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(exitBlock);
                b_.currentBlock_ = exitBlock;

                // (AR) نمرّر نوع عنصر الناتج إلى نتيجة المجموعة (كالقائمة) — يُصلح الوصول المفهرَس
                //      النصّيّ. طبع المجموعة كاملةً يبقى محدودًا (نفس قيد __sad_array_to_string).
                // (EN) Propagate the output element type to the set result (like the list) — fixes
                //      indexed string access. Whole-set print stays limited (same __sad_array_to_string).
                BuildResult setResult(resultSetReg, SadTypeKind::Array);
                setResult.elementType = elemExprResult.type;
                return setResult;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
