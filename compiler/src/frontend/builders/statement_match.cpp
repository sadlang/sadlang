// ============================================================================
// sir_builder_control_match.cpp
// (AR) Pattern matching: buildMatchStatement
// (EN) Pattern matching: match/when statement compilation
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "sir_constants.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <set>
#include <vector>

namespace
{
    // ========================================================================
    // (AR) يجمع (تعاوديًّا) كلّ أسماء المتغيّرات التي يربطها نمطٌ ما، عبر كلّ
    //      أنواع الأنماط المركّبة (قائمة/بنية/ربط/بدائل/تعداد). يُستعمَل لضمان
    //      أنّ جسم الذراع يُترجَم حتى حين يفشل نمطٌ مركّب ساكنًا فلا تُستخرَج
    //      متغيّراته (مثل `[[أ،ب]]` على قيمةٍ قياديّة — ISSUE-067).
    // (EN) Recursively collect every variable name a pattern binds, across all
    //      composite pattern kinds (list/struct/binding/or/enum). Used to keep the
    //      arm body compilable even when a composite pattern fails statically and
    //      its vars are never extracted (e.g. `[[a,b]]` over a scalar — ISSUE-067).
    // ========================================================================
    void collectPatternVarNames(const Sad::AST::Pattern *p,
                                std::vector<std::string> &out)
    {
        if (!p)
            return;
        if (auto *v = dynamic_cast<const Sad::AST::VariablePattern *>(p))
        {
            out.push_back(v->name);
        }
        else if (auto *l = dynamic_cast<const Sad::AST::ListPattern *>(p))
        {
            for (const auto &e : l->elements)
                collectPatternVarNames(e.get(), out);
            if (l->has_rest && !l->rest_name.empty())
                out.push_back(l->rest_name);
        }
        else if (auto *s = dynamic_cast<const Sad::AST::StructPattern *>(p))
        {
            for (const auto &f : s->fields)
                collectPatternVarNames(f.second.get(), out);
        }
        else if (auto *b = dynamic_cast<const Sad::AST::BindingPattern *>(p))
        {
            out.push_back(b->name);
            collectPatternVarNames(b->pattern.get(), out);
        }
        else if (auto *o = dynamic_cast<const Sad::AST::OrPattern *>(p))
        {
            for (const auto &a : o->alternatives)
                collectPatternVarNames(a.get(), out);
        }
        else if (auto *en = dynamic_cast<const Sad::AST::EnumVariantPattern *>(p))
        {
            for (const auto &fp : en->fieldPatterns)
                collectPatternVarNames(fp.get(), out);
        }
    }
} // anonymous namespace

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            void StatementBuilder::buildMatchStatement(Sad::AST::MatchStmt *matchStmt)
            {
                if (!matchStmt)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: starting with "
                          << matchStmt->cases.size() << " cases" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: تقييم القيمة المُطابقة
                // (EN) Step 1: Evaluate the match value
                // ========================================================================
                auto matchResult = b_.buildExpression(matchStmt->value.get());

                if (matchResult.registerName.empty() && !matchResult.isConstant)
                {
                    b_.errors_.push_back("Error: Failed to build match expression");
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: match value reg="
                          << matchResult.registerName << ", isConst=" << matchResult.isConstant << std::endl;
#endif

                // (AR) إذا كانت القيمة ثابتة، نحتاج تحميلها في سجل
                // (EN) If value is constant, need to load it into a register
                std::string matchValueReg = matchResult.registerName;
                SadTypeKind matchValueType = matchResult.type;
                // (AR) ISSUE-067: نوع عنصر المصفوفة المُطابَقة (لبوّابة النوع الساكنة
                //      في قصر الدائرة المتداخل — يمنع Segfault على عنصرٍ قياديّ).
                // (EN) ISSUE-067: matched array's element type (for the static gate
                //      in nested short-circuit — prevents Segfault on a scalar element).
                SadTypeKind matchValueElementType = matchResult.elementType;

                if (matchResult.isConstant)
                {
                    matchValueReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(matchValueReg, matchResult.type);
                    SIROperand constOp;
                    if (matchResult.type == SadTypeKind::String)
                    {
                        constOp = SIROperand::ConstantString(matchResult.constantValue);
                    }
                    else if (matchResult.type == SadTypeKind::Float)
                    {
                        constOp = SIROperand::ConstantF64(std::stod(matchResult.constantValue));
                    }
                    else if (matchResult.type == SadTypeKind::Boolean)
                    {
                        constOp = SIROperand::ConstantBool(matchResult.constantValue == "true");
                    }
                    else
                    {
                        constOp = SIROperand::ConstantI64(std::stoll(matchResult.constantValue));
                    }
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = resultOp;
                    moveInst.operands = {constOp};
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(moveInst);
                    }
                }
                else if (!matchResult.registerName.empty() && matchResult.registerName[0] == '%')
                {
                    // (AR) نتحقق هل القيمة في alloca (متغير مُسجل) أم سجل مؤقت (نتيجة تعبير)
                    //      السجلات المؤقتة (مثل نتيجة member access) تحتوي القيمة مباشرة
                    //      المتغيرات المُسجلة (alloca) تحتاج LOAD لتحميل القيمة
                    // (EN) Check if value is in alloca (registered variable) or temp register
                    //      Temp registers (e.g. member access result) contain value directly
                    //      Registered variables (alloca) need LOAD to get the value
                    std::string regNameWithoutPercent = matchResult.registerName.substr(1);
                    auto *varInfo = b_.lookupVariable(regNameWithoutPercent);
                    bool isAllocaVar = (varInfo != nullptr);

                    if (isAllocaVar)
                    {
                        std::string loadedReg = b_.newTempRegister();
                        SIRInstruction loadInst;
                        loadInst.opcode = SIROpcode::LOAD;
                        loadInst.result = SIROperand::Register(loadedReg, matchValueType);
                        loadInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(loadInst);
                        }
                        matchValueReg = loadedReg;
                    }
                    // (AR) إذا سجل مؤقت — القيمة جاهزة مباشرة، لا نحتاج LOAD
                    // (EN) If temp register — value is ready, no LOAD needed
                }

                // ========================================================================
                // (AR) الخطوة 2: إنشاء كتلة النهاية
                // (EN) Step 2: Create merge block
                // ========================================================================
                std::string mergeLabel = b_.newLabel("match.end");
                auto mergeBlock = b_.createBasicBlock(mergeLabel);
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                }

                // ========================================================================
                // (AR) الخطوة 3: إنشاء كتل لكل case
                // (EN) Step 3: Create blocks for each case
                // ========================================================================
                struct CaseBlockInfo
                {
                    std::string testLabel;
                    std::string bodyLabel;
                    std::string guardLabel;
                    std::shared_ptr<SIRBasicBlock> testBlock;
                    std::shared_ptr<SIRBasicBlock> bodyBlock;
                    std::shared_ptr<SIRBasicBlock> guardBlock;
                };

                // (AR) لكل case: قائمة بالحقول المؤجل استخراجها (تستخدم MatchDeferredField من الهيدر)
                // (EN) For each case: list of fields to extract later (uses MatchDeferredField from header)
                std::vector<std::vector<MatchDeferredField>> deferredADTExtractions(matchStmt->cases.size());

                std::vector<CaseBlockInfo> caseBlocks;

                for (size_t i = 0; i < matchStmt->cases.size(); ++i)
                {
                    CaseBlockInfo info;
                    info.testLabel = b_.newLabel("match.case" + std::to_string(i) + ".test");
                    info.bodyLabel = b_.newLabel("match.case" + std::to_string(i) + ".body");
                    info.testBlock = b_.createBasicBlock(info.testLabel);
                    info.bodyBlock = b_.createBasicBlock(info.bodyLabel);

                    if (b_.currentFunction_)
                    {
                        // (AR) أضف فقط testBlock هنا — bodyBlock/guardBlock تُضاف لاحقاً
                        //      عند بناء كل arm مباشرة قبل بدء بنائه (الخطوة 5).
                        //      السبب: while sub-blocks الناتجة من بناء arm0 body
                        //      تُضاف في نهاية basicBlocks فور إنشائها.
                        //      إذا أضفنا arm1 bodyBlock مسبقاً، سيكون ترتيب blocks:
                        //        arm0_body, arm1_body, arm0_while_cond, arm0_while_body
                        //      مما يجعل arm1_body يُعالج قبل arm0_while_cond في LLVM codegen
                        //      → namedValues["%ع"] يُحدَّث بـ arm1's alloca → يُفسد arm0's while
                        //      الترتيب الصحيح:
                        //        arm0_body, arm0_while_cond, arm0_while_body, arm1_body, ...
                        // (EN) Only add testBlock here — bodyBlock/guardBlock added later
                        //      just before building each arm (Step 5).
                        //      Reason: while sub-blocks from arm0 body get appended at end
                        //      of basicBlocks when created. If arm1 bodyBlock is pre-added,
                        //      order becomes: arm0_body, arm1_body, arm0_while_cond, arm0_while_body
                        //      → arm1_body processed before arm0_while_cond in LLVM codegen
                        //      → namedValues["%var"] updated to arm1's alloca → corrupts arm0's while
                        //      Correct order:
                        //        arm0_body, arm0_while_cond, arm0_while_body, arm1_body, ...
                        b_.currentFunction_->addBasicBlock(info.testBlock);
                        // bodyBlock intentionally deferred — see Step 5 below
                    }

                    // (AR) كتلة guard إن وُجد — أيضاً مؤجلة الإضافة
                    // (EN) Guard block if exists — also deferred
                    if (matchStmt->cases[i].guard)
                    {
                        info.guardLabel = b_.newLabel("match.case" + std::to_string(i) + ".guard");
                        info.guardBlock = b_.createBasicBlock(info.guardLabel);
                        // guardBlock تُضاف لاحقاً في الخطوة 5 (مؤجلة مثل bodyBlock)
                        // guardBlock deferred — added in Step 5 below
                    }

                    caseBlocks.push_back(std::move(info));
                }

                // ========================================================================
                // (AR) الخطوة 4: القفز للـ case الأول
                // (EN) Step 4: Jump to first case
                // ========================================================================
                if (!caseBlocks.empty())
                {
                    SIROperand firstLabel = SIROperand::Label(caseBlocks[0].testLabel);
                    SIRInstruction brFirst = SIRInstruction::Branch(firstLabel);
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(brFirst);
                    }
                }
                else
                {
                    // (AR) لا يوجد cases، القفز مباشرة للنهاية
                    // (EN) No cases, jump directly to merge
                    SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
                    SIRInstruction brMerge = SIRInstruction::Branch(mergeLabelOp);
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(brMerge);
                    }
                    b_.currentBlock_ = mergeBlock;
                    return;
                }

                // ========================================================================
                // (AR) ISSUE-069/081: استخراج الحقول المؤجّلة (عناصر القائمة/حمولة التعداد)
                //      وربطها في **النطاق الحاليّ**. يُستدعى في كتلة الحارس (قبل تقييمه) وفي
                //      كتلة الجسم كليهما: الحارس المسطّح كان يُقيَّم قبل استخراج الجسم فلا يرى
                //      «أ»/«ق» ⇒ تباعد (069 عناصر القائمة، 081 حمولة التعداد). كلتا الكتلتين
                //      لا تُبلَغان إلّا بعد نجاح فحص المميّز/الطول (condReg=true؛ والجسم المحروس
                //      بعد نجاح الحارس أيضًا) فالاستخراج آمن.
                // (EN) ISSUE-069/081: emit deferred field extractions (list elements / ADT
                //      payload) binding them in the CURRENT scope. Called in BOTH the guard
                //      block (before evaluating the guard) and the body block: the flat-path
                //      guard used to be evaluated before the body's extraction, so it never
                //      saw «أ»/«ق» ⇒ divergence (069 list elements, 081 enum payload). Both
                //      blocks are reached only after the tag/length check passes (condReg=true),
                //      so the extraction is safe in either.
                // ========================================================================
                auto emitDeferredExtractions =
                    [&](const std::vector<MatchDeferredField> &deferredFields)
                {
                    for (const auto &deferred : deferredFields)
                    {
                        if (deferred.enumName == Sad::Compiler::kListPatternSentinel)
                        {
                            std::string elemReg = b_.newTempRegister();
                            SIRInstruction getInst;
                            getInst.opcode = SIROpcode::ARRAY_GET;
                            // (AR) [ISSUE-082، مسار عنصر القائمة] نمرّر نوع عنصر المصفوفة
                            //      المُتتبَّع سكونيًّا (`matchValueElementType`) بدل تصليب
                            //      Integer، فالمقارنة النازلة `ن == "س"` ترى النوع الصحيح
                            //      (نصّ ⇒ STRING_CMP محتوًى لا مقارنة بتّات مؤشّر). نظير المسار
                            //      أ′ للحمولة وعامل `في` القائم. مصفوفة مختلطة/مجهولة
                            //      (elementType=Void) ⇒ تراجُع Integer آمن (لا انحدار؛
                            //      المختلطة عضو عائلة «لا وسوم نوع تشغيليّة» ISSUE-070/080).
                            // (EN) [ISSUE-082, list-element path] Pass the statically-tracked
                            //      array element type instead of hardcoding Integer, so the
                            //      downstream comparison sees the real type (string ⇒ content
                            //      STRING_CMP, not pointer-bit compare). Mirrors the payload a′
                            //      path and the existing `في` operator. Mixed/unknown array
                            //      (elementType=Void) ⇒ safe Integer fallback (no regression).
                            SadTypeKind elemType =
                                (matchValueElementType != SadTypeKind::Void)
                                    ? matchValueElementType
                                    : SadTypeKind::Integer;
                            getInst.result = SIROperand::Register(elemReg, elemType);
                            getInst.operands.push_back(
                                SIROperand::Register(matchValueReg, matchValueType));
                            getInst.operands.push_back(
                                SIROperand::ConstantI64(static_cast<int64_t>(deferred.fieldIndex)));
                            getInst.comment = "Deferred list pattern: get element [" +
                                              std::to_string(deferred.fieldIndex) + "] → " + deferred.varName;
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(getInst);

                            VariableInfo elemVarInfo;
                            elemVarInfo.name = deferred.varName;
                            elemVarInfo.type = elemType;
                            elemVarInfo.registerName = elemReg;
                            elemVarInfo.isGlobal = false;
                            elemVarInfo.isMutable = false;
                            elemVarInfo.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(elemVarInfo);
                        }
                        else if (deferred.enumName == Sad::Compiler::kListPatternLiteralSentinel)
                        {
                            // (AR) مقارنة حرفية مؤجّلة — لا-عمل حاليًّا (condReg = فحص الطول فقط).
                            //      ⚠️ دَينٌ معروف = ISSUE-083: العنصر الحرفيّ في المسار المسطّح
                            //      غير مفحوص فيطابق النمط بالطول وحده (`[1، أ]` على `[9، 5]`
                            //      يطابق زورًا). التنفيذ الكامل يلزمه مقارنة العنصر المؤجّلة.
                            // (EN) Deferred list literal comparison — currently a no-op
                            //      (condReg = length check only). ⚠️ Known debt = ISSUE-083:
                            //      the flat-path literal element is unchecked, so the pattern
                            //      matches by length alone (`[1, أ]` over `[9, 5]` matches
                            //      falsely). Full impl needs a deferred element comparison.
                            // TODO(FUTURE): إضافة مقارنة حرفية مؤجلة للمصفوفات (ISSUE-083)
                        }
                        else
                        {
                            std::string fieldReg = b_.newTempRegister();
                            SIRInstruction getPayload(SIROpcode::ENUM_GET_PAYLOAD);
                            // (AR) ISSUE-076/082/084 (ب″): استخرج بالنوع المُستنتَج للحمولة
                            //      (عشريّ/نصّ/منطقيّ/صحيح) إن سُجِّل من موقع الإنشاء (نفس النطاق)؛
                            //      وإلّا **Any** (لا Integer): حمولةٌ مجهولة النوع سكونيًّا (إحالة
                            //      أماميّة/تعارُض) يجب أن تسري قيمةً ديناميّة موسومة فيكشف المستهلك
                            //      وسمها زمنَ التشغيل، بدل معاملتها صحيحًا صامتًا (قمامة/انهيار).
                            // (EN) ISSUE-076/082/084 (ب″): extract with the inferred payload type
                            //      (float/string/bool/int) if registered from a construction site
                            //      (same scope); else **Any** (not Integer): a statically-unknown
                            //      payload (forward-ref/conflict) must flow as a tagged dynamic
                            //      value so the consumer detects its tag at runtime, instead of
                            //      being silently treated as an integer (garbage/segfault).
                            SadTypeKind payloadType = (deferred.fieldType != SadTypeKind::Unknown)
                                                          ? deferred.fieldType
                                                          : SadTypeKind::Any;
                            getPayload.result = SIROperand::Register(fieldReg, payloadType);
                            getPayload.operands.push_back(
                                SIROperand::Register(matchValueReg, matchValueType));
                            getPayload.operands.push_back(
                                SIROperand::ConstantI64(static_cast<int64_t>(deferred.fieldIndex)));
                            // (AR) المعامل [2]: اسم التعداد للبحث عن البنية عبر حدود الدوال
                            // (EN) Operand [2]: enum name for struct lookup across function boundaries
                            getPayload.operands.push_back(
                                SIROperand::ConstantString(deferred.enumName));
                            getPayload.comment = "Deferred extract: field " +
                                                 std::to_string(deferred.fieldIndex) + " (" + deferred.fieldName +
                                                 ") → " + deferred.varName;
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(getPayload);

                            VariableInfo fieldVarInfo;
                            fieldVarInfo.name = deferred.varName;
                            fieldVarInfo.type = payloadType;
                            fieldVarInfo.registerName = fieldReg;
                            fieldVarInfo.isGlobal = false;
                            fieldVarInfo.isMutable = false;
                            fieldVarInfo.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(fieldVarInfo);
                        }
                    }
                };

                // ========================================================================
                // (AR) الخطوة 5: توليد كود كل case
                // (EN) Step 5: Generate code for each case
                // ========================================================================
                for (size_t i = 0; i < matchStmt->cases.size(); ++i)
                {
                    const auto &caseClause = matchStmt->cases[i];
                    auto &info = caseBlocks[i];

                    // (AR) تحديد الـ case التالي للقفز عند الفشل
                    // (EN) Determine next case to jump to on failure
                    std::string nextLabel = (i + 1 < caseBlocks.size())
                                                ? caseBlocks[i + 1].testLabel
                                                : mergeLabel;

                    // === كتلة الاختبار / Test block ===
                    b_.currentBlock_ = info.testBlock;

                    std::string condReg;

                    if (caseClause.pattern)
                    {
                        // (AR) توليد شرط النمط عبر دالة مساعدة — CW-05
                        // (EN) Generate pattern condition via helper — CW-05
                        // (AR) ISSUE-067/075: تمرير nextLabel كهدف فشلٍ **دائمًا** يُمكّن قصر
                        //      الدائرة للأنماط المركّبة المتداخلة حتّى مع وجود guard. حين يفشل
                        //      النمط بنيويًّا يتفرّع قصر الدائرة إلى nextLabel متجاوزًا كتلة
                        //      الحارس — وهذا **صحيحٌ دلاليًّا** (لا يُقيَّم حارسٌ لنمطٍ لم يطابق؛
                        //      المفسّر يوافق). قصر الدائرة يربط متغيّرات النمط المتداخلة في
                        //      كتلة النجاح المُهيمِنة على كتلتَي الحارس والجسم، فيراها الحارس.
                        //      (كان سابقًا "" مع guard ⇒ مسارٌ مسطّح يُهمل الابن المركّب صامتًا
                        //      فيبقى «أ» غير مربوطٍ فيفشل الحارس/الجسم = ISSUE-075.) للأنماط
                        //      غير المركّبة، بوّابة التوجيه لا تُفعَّل ⇒ صفر تغيّر (المسطّح كما هو).
                        // (EN) ISSUE-067/075: ALWAYS pass nextLabel as the fail target so the
                        //      short-circuit enables for nested composite patterns even with a
                        //      guard. A structural pattern failure branches to nextLabel, bypassing
                        //      the guard block — which is semantically CORRECT (a guard is not
                        //      evaluated for a pattern that didn't match; the interpreter agrees).
                        //      Short-circuit binds nested pattern vars in the success block that
                        //      dominates both the guard and body blocks, so the guard sees them.
                        //      (Previously "" with a guard ⇒ a flat path that silently ignored the
                        //      composite child, leaving «أ» unbound ⇒ guard/body failed = ISSUE-075.)
                        //      For non-composite patterns the routing gate is inert ⇒ zero change.
                        const std::string patternFailLabel = nextLabel;
                        // (AR) صفِّر علَم «الذراع ميت ساكنًا» قبل توليد الشرط؛ يضبطه
                        //      failAlways داخل الدائرة القصيرة إن كان الفشل بنيويًّا
                        //      غير مشروط. يقرأه ربطُ المتغيّرات الصوريّ أدناه.
                        // (EN) Reset the "statically-dead arm" flag before generating the
                        //      condition; failAlways sets it inside the short-circuit on an
                        //      unconditional structural fail. Read by the dummy-binding pass below.
                        b_.matchArmStaticallyDead_ = false;
                        condReg = b_.buildMatchPatternCondition(
                            caseClause.pattern.get(),
                            matchValueReg,
                            matchValueType,
                            i,
                            deferredADTExtractions[i],
                            patternFailLabel,
                            matchValueElementType);
                    } // end if (caseClause.pattern)

                    // === التفريع / Branching ===
                    if (caseClause.guard && !info.guardLabel.empty())
                    {
                        // (AR) القفز إلى كتلة guard عند نجاح الاختبار
                        // (EN) Jump to guard block on test success
                        SIROperand condOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        SIROperand guardLabelOp = SIROperand::Label(info.guardLabel);
                        SIROperand nextLabelOp = SIROperand::Label(nextLabel);
                        SIRInstruction brCond = SIRInstruction::BranchCond(condOp, guardLabelOp, nextLabelOp);
                        b_.currentBlock_->instructions.push_back(brCond);

                        // === كتلة guard / Guard block ===
                        // (AR) إضافة guardBlock هنا (مؤجلة من الخطوة 3) قبل بنائه
                        // (EN) Add guardBlock here (deferred from Step 3) before building it
                        if (b_.currentFunction_ && info.guardBlock)
                        {
                            b_.currentFunction_->addBasicBlock(info.guardBlock);
                        }
                        b_.currentBlock_ = info.guardBlock;
                        b_.enterScope();

                        // (AR) ربط المتغيرات من النمط قبل تقييم الشرط
                        // (EN) Bind pattern variables before evaluating guard
                        if (auto *varPat = dynamic_cast<const Sad::AST::VariablePattern *>(caseClause.pattern.get()))
                        {
                            VariableInfo guardVar;
                            guardVar.name = varPat->name;
                            guardVar.type = matchValueType;
                            guardVar.registerName = matchValueReg;
                            guardVar.isGlobal = false;
                            guardVar.isMutable = false;
                            guardVar.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(guardVar);
                        }

                        // (AR) ISSUE-075: ربط صوريّ لمتغيّرات النمط للذراع الميت ساكنًا قبل
                        //      تقييم الحارس. حين يفشل النمط بنيويًّا (مثل `[[أ،ب]]` على قياديّ)
                        //      يضبط failAlways العلَم دون ربط «أ»، لكنّ كتلة الحارس تُبنى
                        //      (وإن كانت ميتةً وقت التشغيل — قصر الدائرة تفرّع لـnextLabel) فيقرأ
                        //      الحارس «أ» ⇒ خطأ ترجمة. نربط صوريًّا (يُقصَر على الميت ساكنًا عبر
                        //      matchArmStaticallyDead_ كما في الجسم) كي يبقى الحارس قابلًا
                        //      للترجمة؛ الصوريّة لا تُقرأ وقت التشغيل (الذراع ميت). نطاق الحارس
                        //      يُغلَق قبل الجسم فلا يتسرّب هذا الربط إليه.
                        // (EN) ISSUE-075: dummy-bind pattern variables for a statically-dead arm
                        //      before evaluating the guard. On a structural pattern fail (e.g.
                        //      `[[أ،ب]]` over a scalar) failAlways sets the flag without binding
                        //      «أ», yet the guard block is still built (dead at runtime — the
                        //      short-circuit branched to nextLabel) and reads «أ» ⇒ compile error.
                        //      Dummy-bind (gated on matchArmStaticallyDead_ as in the body) so the
                        //      guard stays compilable; the dummy is never read at runtime (arm is
                        //      dead). The guard scope is closed before the body, so it doesn't leak.
                        if (caseClause.pattern && b_.matchArmStaticallyDead_)
                        {
                            std::vector<std::string> guardVarNames;
                            collectPatternVarNames(caseClause.pattern.get(), guardVarNames);
                            for (const auto &vn : guardVarNames)
                            {
                                if (vn.empty() || b_.lookupVariable(vn) != nullptr)
                                    continue;
                                std::string dummyReg = b_.newTempRegister();
                                SIRInstruction dummyMove(SIROpcode::MOVE);
                                dummyMove.result = SIROperand::Register(dummyReg, SadTypeKind::Integer);
                                dummyMove.operands = {SIROperand::ConstantI64(0)};
                                dummyMove.comment = "ISSUE-075 dead-arm guard dummy bind: " + vn;
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(dummyMove);

                                VariableInfo dummyVar;
                                dummyVar.name = vn;
                                dummyVar.type = SadTypeKind::Integer;
                                dummyVar.registerName = dummyReg;
                                dummyVar.isGlobal = false;
                                dummyVar.isMutable = false;
                                dummyVar.scopeLevel = b_.currentScopeLevel_;
                                b_.addVariable(dummyVar);
                            }
                        }

                        // (AR) ISSUE-069/081: استخرج الحقول المؤجّلة (عناصر القائمة/حمولة
                        //      التعداد) في نطاق الحارس قبل تقييمه كي يراها؛ نظير ربط الجسم.
                        //      كتلة الحارس تُبلَغ فقط بعد نجاح فحص المميّز/الطول ⇒ الاستخراج آمن.
                        //      **الازدواج متعمَّد:** يُبثّ هنا (نطاق الحارس) ومجدَّدًا في كتلة
                        //      الجسم بسجلّين مختلفين؛ نطاق الحارس يُغلَق (`exitScope`) قبل الجسم
                        //      فلا يعبر سجلّ الحارس حدّ الكتلة، وإعادة الاستخراج في الجسم ضروريّة.
                        // (EN) ISSUE-069/081: extract deferred fields (list elements / enum
                        //      payload) into the guard scope before evaluating it so the guard
                        //      sees them; mirrors the body binding. The guard block is reached
                        //      only after the tag/length check passes ⇒ extraction is safe.
                        //      **The double-emit is intentional:** emitted here (guard scope)
                        //      and again in the body block with distinct registers; the guard
                        //      scope is closed (`exitScope`) before the body, so the guard
                        //      register never crosses the block boundary and re-extraction in
                        //      the body is required.
                        emitDeferredExtractions(deferredADTExtractions[i]);

                        auto guardResult = b_.buildExpression(caseClause.guard.get());

                        std::string guardCondReg = guardResult.registerName;
                        if (guardResult.isConstant)
                        {
                            guardCondReg = b_.newTempRegister();
                            SIROperand gResultOp = SIROperand::Register(guardCondReg, guardResult.type);
                            SIROperand gConstOp;
                            if (guardResult.type == SadTypeKind::Boolean)
                            {
                                gConstOp = SIROperand::ConstantBool(guardResult.constantValue == "true");
                            }
                            else if (guardResult.type == SadTypeKind::Integer)
                            {
                                gConstOp = SIROperand::ConstantI64(std::stoll(guardResult.constantValue));
                            }
                            else
                            {
                                gConstOp = SIROperand::ConstantI64(guardResult.constantValue == "true" ? 1 : 0);
                            }
                            SIRInstruction moveG(SIROpcode::MOVE);
                            moveG.result = gResultOp;
                            moveG.operands = {gConstOp};
                            b_.currentBlock_->instructions.push_back(moveG);
                        }

                        SIROperand guardCondOp = SIROperand::Register(guardCondReg, SadTypeKind::Boolean);
                        SIROperand bodyLabelOp = SIROperand::Label(info.bodyLabel);
                        SIROperand nextLabelOp2 = SIROperand::Label(nextLabel);
                        SIRInstruction brGuard = SIRInstruction::BranchCond(guardCondOp, bodyLabelOp, nextLabelOp2);
                        b_.currentBlock_->instructions.push_back(brGuard);

                        b_.exitScope();
                    }
                    else
                    {
                        // (AR) بدون guard: القفز مباشرة إلى الجسم أو التالي
                        // (EN) No guard: jump directly to body or next
                        SIROperand condOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        SIROperand bodyLabelOp = SIROperand::Label(info.bodyLabel);
                        SIROperand nextLabelOp = SIROperand::Label(nextLabel);
                        SIRInstruction brCond = SIRInstruction::BranchCond(condOp, bodyLabelOp, nextLabelOp);
                        b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // === كتلة الجسم / Body block ===
                    // (AR) إضافة bodyBlock هنا (مؤجلة من الخطوة 3) قبل بنائه.
                    //      هذا يضمن أن while sub-blocks الناتجة من بناء هذا arm
                    //      تأتي مباشرةً بعد bodyBlock وقبل bodyBlock التالي
                    // (EN) Add bodyBlock here (deferred from Step 3) before building it.
                    //      Ensures while sub-blocks generated during this arm's body build
                    //      appear immediately after this bodyBlock, before next arm's bodyBlock.
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(info.bodyBlock);
                    }
                    b_.currentBlock_ = info.bodyBlock;
                    b_.enterScope();

                    // (AR) ربط المتغيرات من النمط في الجسم
                    // (EN) Bind pattern variables in body scope
                    if (auto *varPat = dynamic_cast<const Sad::AST::VariablePattern *>(caseClause.pattern.get()))
                    {
                        VariableInfo bodyVar;
                        bodyVar.name = varPat->name;
                        bodyVar.type = matchValueType;
                        bodyVar.registerName = matchValueReg;
                        bodyVar.isGlobal = false;
                        bodyVar.isMutable = false;
                        bodyVar.scopeLevel = b_.currentScopeLevel_;
                        b_.addVariable(bodyVar);
                    }

                    // (AR) === استخراج حقول ADT/القائمة المؤجلة (كتلة الجسم) ===
                    //      هذه الحقول تم تأجيل استخراجها من كتلة الاختبار إلى هنا لأن كتلة
                    //      الجسم تنفَّذ فقط عندما يتطابق المميّز (tag)/الطول، مما يضمن صلاحية
                    //      الحقول. تُستخرَج أيضًا في كتلة الحارس (إن وُجد) عبر نفس المُساعِد
                    //      (ISSUE-069/081) بنطاقٍ منفصل.
                    // (EN) === Deferred ADT/list field extraction (body block) ===
                    //      Deferred from testBlock to here because bodyBlock executes only
                    //      when the tag/length matches, ensuring fields are valid. Also
                    //      extracted in the guard block (if any) via the same helper
                    //      (ISSUE-069/081) in its own scope.
                    emitDeferredExtractions(deferredADTExtractions[i]);

                    // ========================================================
                    // (AR) حارس صلابة (ISSUE-067): اربط أيّ متغيّر نمطٍ لم يُربَط
                    //      بعدُ بقيمةٍ صوريّة. يحدث حين يفشل نمطٌ مركّب ساكنًا (مثل
                    //      `[[أ،ب]]` على قيمةٍ قياديّة) فلا تُستخرَج متغيّراته المتداخلة؛
                    //      الذراع حينئذٍ ميتٌ (قصر الدائرة تفرّع لـfailLabel قبل الوصول
                    //      هنا) فالقيمة الصوريّة لا تُقرأ وقت التشغيل، لكنّها تُبقي جسم
                    //      الذراع قابلًا للترجمة بدل خطأ «Undefined variable».
                    //      لا نلمس المربوطة فعلًا (المسار الصالح يربطها قبل هذه النقطة).
                    //
                    //      ⚠️ **قصرٌ على الذراع الميت ساكنًا (نقدا Amelia):** لا يكفي
                    //      «بلا guard». الربط الصوريّ آمنٌ فقط إن كان النمط قد فشل بنيويًّا
                    //      وقت الترجمة (failAlways تفرّع إلى failLabel) فلا يُقرأ الصوريّ
                    //      وقت التشغيل. لكنْ ذراعٌ حيٌّ فقد ربط حمولته (مثل تعدادٍ داخل
                    //      قائمة `[نتيجة.نجاح(ق)]` بلا guard: المسطّح يطابق الطول ويترك
                    //      «ق» غير مربوط دون قتل الذراع) ⇒ الصوريّ يُقرأ ⇒ «ق=0» صامتة
                    //      وتباعد عن المفسّر. لذا نربط صوريًّا **فقط** إن ضبط failAlways
                    //      علَم matchArmStaticallyDead_؛ وإلّا نتركها خطأ ترجمة صاخبًا
                    //      («Undefined variable») بدل إخفاء التباعد. **ملاحظة (بعد ISSUE-075):**
                    //      العلَم يُضبَط الآن لأذرع guard المركّبة الميتة أيضًا (guard+مركّب صار
                    //      يُفعّل قصر الدائرة، فـfailAlways قد يضبط العلَم لذراعٍ ميت)؛ الربط
                    //      الصوريّ هنا (للجسم) وفي كتلة الحارس صحيحٌ لأنّ الذراع ميتٌ وقت
                    //      التشغيل (SC تفرّع لـnextLabel) فلا تُقرأ الصوريّة. العلَم لا يُضبَط
                    //      لذراعٍ حيّ (SC لا يستدعي failAlways) فلا يُخفى تباعدُ ذراعٍ حيّ.
                    // (EN) Robustness guard (ISSUE-067): bind any pattern variable still
                    //      unbound to a dummy. Happens when a composite pattern fails
                    //      statically (e.g. `[[a,b]]` over a scalar) so its nested vars are
                    //      never extracted; the arm is then dead (short-circuit branched to
                    //      failLabel before reaching here) so the dummy is never read at
                    //      runtime, yet it keeps the arm body compilable instead of an
                    //      "Undefined variable" error. Already-bound vars (viable path) are
                    //      left untouched.
                    //
                    //      ⚠️ **Restricted to statically-dead arms (Amelia's 2 reviews):**
                    //      "guard-less" is NOT enough. The dummy is only safe when the arm
                    //      failed structurally at compile time (failAlways branched to
                    //      failLabel), so it is never read at runtime. But a LIVE arm that
                    //      lost its payload binding (e.g. enum-in-list `[نتيجة.نجاح(ق)]` with
                    //      no guard: the flat path matches the length and leaves `ق` unbound
                    //      WITHOUT killing the arm) would read the dummy ⇒ silent "ق=0" and
                    //      divergence from the interpreter. So we dummy-bind ONLY when
                    //      failAlways set matchArmStaticallyDead_; otherwise we leave a loud
                    //      compile error ("Undefined variable"). NOTE (post-ISSUE-075): the flag
                    //      is now ALSO set for dead composite guard arms (guard+composite now
                    //      routes to the short-circuit, so failAlways may set it for a dead arm);
                    //      the dummy here (body) and in the guard block is correct because the arm
                    //      is dead at runtime (SC branched to nextLabel), so it is never read. The
                    //      flag is never set for a LIVE arm (SC doesn't call failAlways), so no
                    //      live-arm divergence is masked.
                    // ========================================================
                    if (caseClause.pattern && b_.matchArmStaticallyDead_)
                    {
                        std::vector<std::string> patVarNames;
                        collectPatternVarNames(caseClause.pattern.get(), patVarNames);
                        for (const auto &vn : patVarNames)
                        {
                            if (vn.empty() || b_.lookupVariable(vn) != nullptr)
                                continue;
                            std::string dummyReg = b_.newTempRegister();
                            SIRInstruction dummyMove(SIROpcode::MOVE);
                            dummyMove.result = SIROperand::Register(dummyReg, SadTypeKind::Integer);
                            dummyMove.operands = {SIROperand::ConstantI64(0)};
                            dummyMove.comment = "ISSUE-067 dead-arm dummy bind: " + vn;
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(dummyMove);

                            VariableInfo dummyVar;
                            dummyVar.name = vn;
                            dummyVar.type = SadTypeKind::Integer;
                            dummyVar.registerName = dummyReg;
                            dummyVar.isGlobal = false;
                            dummyVar.isMutable = false;
                            dummyVar.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(dummyVar);
                        }
                    }

                    // (AR) تنفيذ جسم الحالة
                    // (EN) Execute case body
                    for (auto &bodyStmt : caseClause.body)
                    {
                        buildStatement(bodyStmt.get());
                    }

                    b_.exitScope();

                    // (AR) قفز غير مشروط إلى كتلة النهاية
                    // (EN) Unconditional jump to merge block
                    SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
                    SIRInstruction brMerge = SIRInstruction::Branch(mergeLabelOp);
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(brMerge);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: generated case " << i << std::endl;
#endif
                } // end for each case

                // ========================================================================
                // (AR) الخطوة 5.5: فحص شمولية المطابقة (Exhaustiveness Check)
                //      إذا كانت القيمة المُطابقة من نوع تعداد جبري (ADT)
                //      نتحقق أن جميع حالات التعداد مغطاة أو يوجد نمط شامل (_/افتراضي)
                //      هذا يمنع السلوك غير المعرف عندما لا يتطابق أي نمط
                // (EN) Step 5.5: Exhaustiveness Check
                //      If match value is an ADT enum type, verify all variants are covered
                //      or a wildcard/default pattern exists. Prevents undefined behavior.
                // ========================================================================
                {
                    bool hasWildcard = false;
                    bool hasVariablePattern = false;
                    std::set<std::string> coveredVariants;
                    std::string adtEnumName;

                    for (size_t i = 0; i < matchStmt->cases.size(); ++i)
                    {
                        const auto &caseClause = matchStmt->cases[i];
                        if (!caseClause.pattern)
                            continue;

                        if (dynamic_cast<const Sad::AST::WildcardPattern *>(caseClause.pattern.get()))
                        {
                            hasWildcard = true;
                            break;
                        }
                        if (dynamic_cast<const Sad::AST::VariablePattern *>(caseClause.pattern.get()))
                        {
                            hasVariablePattern = true;
                            break;
                        }
                        if (auto *enumPat = dynamic_cast<const Sad::AST::EnumVariantPattern *>(caseClause.pattern.get()))
                        {
                            adtEnumName = enumPat->enumName;
                            coveredVariants.insert(enumPat->variantName);
                        }
                    }

                    // (AR) إذا كان تعداد ADT ولا يوجد نمط شامل — نتحقق من التغطية
                    // (EN) If ADT enum and no wildcard — check coverage
                    if (!hasWildcard && !hasVariablePattern && !adtEnumName.empty())
                    {
                        auto adtIt = b_.adtEnumTable_.find(adtEnumName);
                        if (adtIt != b_.adtEnumTable_.end())
                        {
                            const ADTEnumInfo &adtInfo = adtIt->second;
                            std::vector<std::string> missingVariants;

                            for (const auto &variant : adtInfo.variants)
                            {
                                if (coveredVariants.find(variant.name) == coveredVariants.end())
                                {
                                    missingVariants.push_back(variant.name);
                                }
                            }

                            if (!missingVariants.empty())
                            {
                                // (AR) تحذير: حالات غير مغطاة — قد يحدث سلوك غير متوقع.
                                //      يُدفَع في قناة warnings_ لا errors_: تحذيرٌ لا خطأٌ، فلا
                                //      يُفشِل البناء (RFC: فصل التحذيرات عن الأخطاء في بانِي SIR).
                                // (EN) Warning: uncovered variants — may cause unexpected behavior.
                                //      Pushed to warnings_ (not errors_): a warning must not fail
                                //      the build (RFC: separate warnings from errors in SIRBuilder).
                                std::string warning = "Warning: Non-exhaustive match on enum '" +
                                                      adtEnumName + "'. Missing variants: ";
                                for (size_t m = 0; m < missingVariants.size(); ++m)
                                {
                                    if (m > 0)
                                        warning += ", ";
                                    warning += adtEnumName + "." + missingVariants[m];
                                }
                                // (AR) يُطبَع رسميًّا من السائق عبر report_warning بعد بناء الوحدة؛
                                //   لا نطبعه هنا ثانيةً (كان يُكرَّر في بناء Debug). [L3]
                                b_.warnings_.push_back(warning);
                            }
                        }
                    }
                }

                // ========================================================================
                // (AR) الخطوة 6: الاستمرار بعد match
                // (EN) Step 6: Continue after match statement
                // ========================================================================
                b_.currentBlock_ = mergeBlock;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: completed" << std::endl;
#endif
            }

            // ============================================================================
            // buildWhileLoop - بناء حلقة while
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:393
            // التوقيع / Signature: void buildWhileLoop(AST::WhileStmt* whileLoop);
            //
            // المعاملات / Parameters:
            // - whileLoop: AST::WhileStmt* = Sad::AST::WhileStmt* (sir_builder.h:393)
            //
            // WhileStmt Members (statements.h:143-175):
            // - condition: ExprPtr (line 149)
            // - body: StmtPtr (line 150)
            //
            // المتغيرات المستخدمة / Used variables:
            // - b_.currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
            //
            // الدوال المستدعاة / Called functions:
            // - b_.buildExpression: sir_builder.h:432
            // - buildStatement: sir_builder.h:372
            // - b_.createBasicBlock: sir_builder.h:501
            // - b_.newLabel: sir_builder.h:520
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad