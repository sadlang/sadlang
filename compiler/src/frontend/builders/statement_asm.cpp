// ============================================================================
// statement_asm.cpp
// ============================================================================
// (AR) خفض كتلة لهجة التجميع العربيّ «تجميع … نهاية» إلى llvm::InlineAsm
//      (م١ RFC اللهجات الأصيلة). يبني قالب الأسمبلي الأصليّ (صيغة AT&T بترتيب
//      مصدر ثمّ وجهة) والقيود آليًّا من المعجم المولَّد (asm_dialect_generated.h)،
//      ويربط {متغيّر ص} بقيود r/‏=r تلقائيًّا، ويلوّث(…) ⇒ clobbers، واللصائق قفزًا
//      محلّيًّا رقميًّا. الناتج تعليمة SIR INLINE_ASM يستهلكها المولّد الخلفيّ.
// (EN) Lowers the Arabic assembly dialect block to llvm::InlineAsm (native-
//      dialects RFC M1). Builds the native AT&T template + constraints from the
//      generated lexicon, auto-binds {sad var} to r/=r, يلوّث ⇒ clobbers, labels
//      to numeric local jumps. Emits an INLINE_ASM SIR instruction.
// ============================================================================
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "directive_nodes.h"
#include "asm_dialect_generated.h"
#include "sir_constants.h" // (AR) kRawLlvmAsmMarker — عقد SOH المشترك مع الخلفيّة
#include "error_manager.h" // (AR) buildBilingualMessage — بلاغات كتالوج الأخطاء (مصدر الحقيقة)

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            namespace
            {
                // (AR) نمرّر النصّ النهائيّ بصيغة LLVM جاهزة (‎$$‎ للثابت، ‎$N‎ للمعامل،
                //      ‎%reg‎ للسجلّ) مع بادئة SOH كي يتخطّى الخلفيُّ تحويل صيغة GCC —
                //      الثابت المشترك kRawLlvmAsmMarker في sir_constants.h (عقد الطبقتين).
                // (EN) SOH prefix tells the backend to skip its GCC->LLVM conversion —
                //      shared constant kRawLlvmAsmMarker in sir_constants.h.

                // (AR) يصيّر معامل سجلّ/عدد إلى نصّ AT&T. العنونة تُعالَج في المستدعي
                //      (renderMemory) لأنّها تحتاج ربط {متغيّر} ⇒ $N والإبلاغ عن الأخطاء.
                // (EN) Renders a register/immediate operand to AT&T text. Memory is handled
                //      by the caller (needs {var}->$N binding and error reporting).
                std::string renderStaticOperand(const ::Sad::AST::AsmOperand &op)
                {
                    using Kind = ::Sad::AST::AsmOperand::Kind;
                    switch (op.kind)
                    {
                    case Kind::Register:
                        return "%" + op.loweredText;
                    case Kind::Immediate:
                        // (AR) ثابت AT&T ‎$imm‎ ⇒ يُهرَّب ‎$$‎ في نصّ LLVM.
                        return "$$" + op.loweredText;
                    default:
                        return std::string();
                    }
                }
            } // namespace

            void StatementBuilder::buildAsmBlock(::Sad::AST::AsmBlockStmt *asmBlock)
            {
                if (!asmBlock)
                    return;

                using namespace ::Sad::Dialects::Asm;
                using Kind = ::Sad::AST::AsmOperand::Kind;

                // (AR) بلاغ كتالوج ثنائيّ اللغة يُدفَع إلى errors_ ليُفشِل البناء عبر
                //      hasErrors() (نمط reportDuplicateExportSymbol/SEM024).
                // (EN) Bilingual catalog diagnostic pushed to errors_ (fails the build).
                auto emitAsmError = [&](::Sad::Errors::ErrorCode code,
                                        std::map<std::string, std::string> placeholders) {
                    ::Sad::Errors::RenderContext ctx;
                    ctx.placeholders = std::move(placeholders);
                    b_.errors_.push_back(
                        ::Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(code, ctx));
                };

                // ── ① تصنيف متغيّرات ص: مكتوب (يظهر في موضع 'w') ومقروء (خلا ذلك) ──
                //    متغيّر مكتوب+مقروء ⇒ inout (خرج «=r» مقيَّد بمدخل مربوط).
                std::vector<std::string> writtenOrder; // (AR) بترتيب أوّل ظهور ككتابة
                std::vector<std::string> readOrder;    // (AR) بترتيب أوّل ظهور كقراءة صِرفة/inout
                std::map<std::string, bool> isWritten;
                std::map<std::string, bool> isRead;
                auto pushUnique = [](std::vector<std::string> &v, const std::string &n) {
                    for (const auto &x : v)
                        if (x == n)
                            return;
                    v.push_back(n);
                };

                for (const auto &item : asmBlock->items)
                {
                    if (item.isLabel)
                        continue;
                    for (std::size_t i = 0; i < item.operands.size(); ++i)
                    {
                        const ::Sad::AST::AsmOperand &op = item.operands[i];
                        const char cls = (i < item.operandClasses.size())
                                             ? item.operandClasses[i]
                                             : 'r';
                        if (op.kind == Kind::SadVariable)
                        {
                            if (cls == 'w')
                            {
                                pushUnique(writtenOrder, op.text);
                                isWritten[op.text] = true;
                                // (AR) وجهة تُقرأ قبل الكتابة (اجمع/اطرح/وافق…) ⇒ inout.
                                //      التصنيف من علم reads_dest في معجم مصدر الحقيقة
                                //      (انقل/حمّل_عنوان/اسحب… تكتب دون قراءة).
                                // (EN) Dest-reading mnemonics (per the SoT reads_dest
                                //      flag) bind as inout; write-only dests do not.
                                if (item.readsDest)
                                    isRead[op.text] = true;
                            }
                            else
                                isRead[op.text] = true;
                        }
                        else if (op.kind == Kind::Memory)
                        {
                            for (const auto &p : op.pieces)
                                if (p.kind == Kind::SadVariable)
                                    isRead[p.text] = true;
                        }
                    }
                }
                for (const auto &item : asmBlock->items)
                {
                    if (item.isLabel)
                        continue;
                    for (std::size_t i = 0; i < item.operands.size(); ++i)
                    {
                        const ::Sad::AST::AsmOperand &op = item.operands[i];
                        const char cls = (i < item.operandClasses.size()) ? item.operandClasses[i] : 'r';
                        if (op.kind == Kind::SadVariable && cls != 'w')
                            pushUnique(readOrder, op.text);
                        else if (op.kind == Kind::Memory)
                            for (const auto &p : op.pieces)
                                if (p.kind == Kind::SadVariable)
                                    pushUnique(readOrder, p.text);
                    }
                }

                // (AR) [تصحيح ٢] كلّ متغيّر ص مُشار إليه بـ{…} يجب أن يكون معرَّفًا —
                //      قراءةً أو خرجًا مكتوبًا — وإلا خطأ كتالوج SEM031 (لا تجاهل صامت).
                // (EN) [Fix 2] every {…}-referenced sad var (read or written) must be
                //      defined, else SEM031 catalog error (no silent drop).
                {
                    std::set<std::string> allVars;
                    for (const auto &w : writtenOrder)
                        allVars.insert(w);
                    for (const auto &r : readOrder)
                        allVars.insert(r);
                    bool undefined = false;
                    for (const auto &v : allVars)
                    {
                        if (!b_.lookupVariable(v))
                        {
                            emitAsmError(::Sad::Errors::ErrorCode::SEM_ASM_UNDEFINED_VAR, {{"name", v}});
                            undefined = true;
                        }
                    }
                    if (undefined)
                        return;
                }

                // (AR) [تصحيح ٥] م١: خرجٌ واحد كحدّ أقصى — خطأ كتالوج SEM028 (لا سلسلة خام).
                // (EN) [Fix 5] M1: at most one written output — SEM028 catalog error.
                if (writtenOrder.size() > 1)
                {
                    emitAsmError(::Sad::Errors::ErrorCode::SEM_ASM_MULTIPLE_OUTPUTS,
                                 {{"count", std::to_string(writtenOrder.size())}});
                    return;
                }

                // ── ② ترقيم المعاملات: الخرج أوّلًا ($0) ثمّ المدخلات ($1..) ──
                //    كلّ متغيّر مكتوب ⇒ خانة خرج $idx (تستعملها القراءة والكتابة).
                //    متغيّر مقروء صِرف ⇒ خانة مدخل «r». متغيّر inout ⇒ مدخل مربوط
                //    «idx» (بقيمته الابتدائيّة) لا يُشار إليه في القالب.
                std::map<std::string, int> outSlot; // written var -> $N
                std::map<std::string, int> inSlot;  // pure-read var -> $N
                int slot = 0;
                for (const auto &w : writtenOrder)
                    outSlot[w] = slot++;
                // (AR) المدخلات المربوطة (inout) تأخذ خانات $N لكنّها لا تُشار إليها.
                for (const auto &w : writtenOrder)
                    if (isRead.count(w))
                        slot++; // (AR) خانة المدخل المربوط
                for (const auto &r : readOrder)
                    if (!isWritten.count(r))
                        inSlot[r] = slot++;

                // ── ③ بناء نصّ القالب (AT&T: مصدر ثمّ وجهة) ──
                // (AR) اللصائق ⇒ لصائق محلّيّة رقميّة: تعريف «idx:» وقفز «idxb/idxf».
                //      [تصحيح ٤] لصيقة مكرَّرة ⇒ خطأ كتالوج SEM030 (لا خفض صامت لرقم واحد).
                // (EN) [Fix 4] duplicate label ⇒ SEM030 (no silent same-number lowering).
                std::map<std::string, int> labelIndex;
                int nextLabel = 0;
                bool asmFailed = false;
                for (const auto &item : asmBlock->items)
                {
                    if (!item.isLabel)
                        continue;
                    if (labelIndex.find(item.labelName) != labelIndex.end())
                    {
                        emitAsmError(::Sad::Errors::ErrorCode::SEM_ASM_DUPLICATE_LABEL,
                                     {{"label", item.labelName}});
                        asmFailed = true;
                        continue;
                    }
                    labelIndex[item.labelName] = nextLabel++;
                }
                if (asmFailed)
                    return;

                std::vector<std::string> definedLabels; // (AR) اللصائق المعرَّفة حتى الآن (لتمييز b/f)

                std::ostringstream tmpl;
                bool firstLine = true;

                auto slotOfVar = [&](const std::string &name) -> int {
                    // (AR) متغيّر مكتوب (خرج/inout) ⇒ خانة الخرج؛ وإلا خانة المدخل.
                    if (outSlot.count(name))
                        return outSlot[name];
                    return inSlot[name];
                };

                // (AR) [تصحيح ١] عنونة ⇒ نصّ AT&T «disp(base)»: قاعدة واحدة (سجلّ أو
                //      متغيّر ص ⇒ $N)، وإزاحة عدديّة اختياريّة (مع إشارتها). قاعدتان أو
                //      فهرس×حجم ⇒ SEM032 (لا إسقاط صامت للإزاحة).
                // (EN) [Fix 1] memory ⇒ "disp(base)": one base + optional signed disp;
                //      two bases or index*scale ⇒ SEM032 (no silent drop).
                auto renderMemory = [&](const ::Sad::AST::AsmOperand &mem, std::string &out) -> bool {
                    std::string base;
                    int baseCount = 0;
                    std::string disp;
                    bool haveDisp = false;
                    bool indexScale = false;
                    int sign = 1;
                    for (const auto &p : mem.pieces)
                    {
                        if (p.kind == Kind::Register)
                        {
                            base = "%" + p.loweredText;
                            ++baseCount;
                        }
                        else if (p.kind == Kind::SadVariable)
                        {
                            base = "$" + std::to_string(slotOfVar(p.text));
                            ++baseCount;
                        }
                        else if (p.kind == Kind::Immediate)
                        {
                            if (haveDisp)
                            {
                                indexScale = true; // (AR) أكثر من إزاحة ⇒ غير مدعوم
                                break;
                            }
                            disp = (sign < 0 ? std::string("-") : std::string()) + p.loweredText;
                            haveDisp = true;
                        }
                        else if (p.kind == Kind::Punct)
                        {
                            if (p.loweredText == "-")
                                sign = -1;
                            else if (p.loweredText == "*")
                                indexScale = true; // (AR) فهرس×حجم ⇒ مؤجَّل م١
                            // '+' يُتجاهَل (الافتراضيّ)
                        }
                    }
                    if (baseCount != 1 || indexScale)
                    {
                        emitAsmError(::Sad::Errors::ErrorCode::SEM_ASM_MEMORY_FORM, {});
                        return false;
                    }
                    out = (haveDisp ? disp : std::string()) + "(" + base + ")";
                    return true;
                };

                // (AR) width: عرض معامل السجلّ من المعجم (operandWidth). 16 ⇒ نُصدر
                //      ‎${N:w}‎ فيختار العتاد السجلَّ الفرعيّ 16-بت (‎%ax‎ لا ‎%eax‎) —
                //      إلزاميّ لمعامل r/m16 (ltr/str) الذي يرفض المُجمِّع صيغته 32-بت.
                //      يسري على معامل السجلّ (‎$N‎) فقط؛ الذاكرة/العدد لا مُعدِّل لهما.
                // (EN) width: register-operand bit width; 16 ⇒ emit ${N:w} (16-bit
                //      sub-register) for r/m16 mnemonics whose 32-bit form is rejected.
                auto operandRef = [&](const ::Sad::AST::AsmOperand &op, char /*cls*/, int width, bool &ok) -> std::string {
                    if (op.kind == Kind::SadVariable)
                    {
                        const std::string slot = std::to_string(slotOfVar(op.text));
                        return width == 16 ? ("${" + slot + ":w}") : ("$" + slot);
                    }
                    if (op.kind == Kind::Memory)
                    {
                        std::string out;
                        if (!renderMemory(op, out))
                        {
                            ok = false;
                            return std::string();
                        }
                        return out;
                    }
                    return renderStaticOperand(op);
                };

                for (const auto &item : asmBlock->items)
                {
                    if (item.isLabel)
                    {
                        if (!firstLine)
                            tmpl << "\n\t";
                        firstLine = false;
                        tmpl << labelIndex[item.labelName] << ":";
                        definedLabels.push_back(item.labelName);
                        continue;
                    }

                    if (!firstLine)
                        tmpl << "\n\t";
                    firstLine = false;

                    tmpl << item.mnemonicEn;

                    // (AR) بادئة كرّر: تليها منمنمة تابعة ⇒ «rep\n\tstosd».
                    if (item.operandClasses == std::string("a") && !item.operands.empty() &&
                        item.operands[0].kind == Kind::SubMnemonic)
                    {
                        tmpl << "\n\t" << item.operands[0].loweredText;
                        continue;
                    }

                    // (AR) اللصيقة (قفز/نداء): b إن كانت معرَّفة سابقًا وإلا f.
                    //      [تصحيح ٣] هدف غير معرَّف كلصيقة داخل الكتلة ⇒ SEM029 (لا «0f»
                    //      المعتم). النداء إلى رمز خارجيّ حقيقيّ خارج نطاق م١ فيُخطأ هنا أيضًا.
                    // (EN) [Fix 3] undefined jump/call target ⇒ SEM029 (no opaque "0f").
                    if (item.operandClasses == std::string("l") && !item.operands.empty() &&
                        item.operands[0].kind == Kind::LabelRef)
                    {
                        const std::string &lbl = item.operands[0].text;
                        auto it = labelIndex.find(lbl);
                        if (it == labelIndex.end())
                        {
                            emitAsmError(::Sad::Errors::ErrorCode::SEM_ASM_UNDEFINED_LABEL,
                                         {{"label", lbl}});
                            asmFailed = true;
                            continue;
                        }
                        bool backward = false;
                        for (const auto &d : definedLabels)
                            if (d == lbl)
                                backward = true;
                        tmpl << " " << it->second << (backward ? "b" : "f");
                        continue;
                    }

                    // (AR) معاملات بيانات: تُعكس إلى ترتيب AT&T (مصدر ثمّ وجهة).
                    if (!item.operands.empty())
                    {
                        std::vector<std::string> rendered;
                        for (std::size_t i = 0; i < item.operands.size(); ++i)
                        {
                            const char cls = (i < item.operandClasses.size())
                                                 ? item.operandClasses[i]
                                                 : 'r';
                            bool ok = true;
                            std::string r = operandRef(item.operands[i], cls, item.operandWidth, ok);
                            if (!ok)
                                asmFailed = true;
                            rendered.push_back(r);
                        }
                        // (AR) وجهة أوّلًا في المصدر العربيّ ⇒ نعكس لصيغة AT&T.
                        tmpl << " ";
                        for (std::size_t i = 0; i < rendered.size(); ++i)
                        {
                            if (i)
                                tmpl << ", ";
                            tmpl << rendered[rendered.size() - 1 - i];
                        }
                    }
                }

                // (AR) أيّ خطأ عنونة/لصيقة أثناء بناء القالب ⇒ لا نُصدر تعليمة فاسدة.
                // (EN) Any addressing/label error during template build ⇒ do not emit.
                if (asmFailed)
                    return;

                // ── ④ بناء سلسلة القيود: outputs، ثمّ المدخلات المربوطة، ثمّ القراءة الصِرفة، ثمّ clobbers ──
                std::vector<std::string> constraintParts;
                for (const auto &w : writtenOrder)
                    constraintParts.push_back("=r");
                // (AR) مدخل مربوط لكلّ متغيّر inout: القيد = رقم خانة خرجه.
                for (const auto &w : writtenOrder)
                    if (isRead.count(w))
                        constraintParts.push_back(std::to_string(outSlot[w]));
                for (const auto &r : readOrder)
                    if (!isWritten.count(r))
                        constraintParts.push_back("r");
                for (const auto &c : asmBlock->clobbers)
                    constraintParts.push_back(c.llvm);

                // (AR) التلويث الضمنيّ من المعجم (implicit_writes): سجلّات يكتبها العتاد
                //      دون أن تظهر معاملًا (اضرب/اقسم ⇒ ‎~{eax},~{edx}‎…) تُصرَّح لـllvm
                //      تلقائيًّا كي لا يضع مدخلًا/خرجًا حيًّا فيها — مع إسقاط المكرَّر
                //      ممّا صرّح به «يلوّث(…)» أو منمنمة سابقة.
                // (EN) Lexicon implicit clobbers (implicit_writes): registers the hardware
                //      writes without an operand are auto-declared to llvm, deduplicated
                //      against explicit يلوّث clobbers and earlier mnemonics.
                {
                    std::set<std::string> declaredClobbers;
                    for (const auto &c : asmBlock->clobbers)
                        declaredClobbers.insert(c.llvm);
                    for (const auto &item : asmBlock->items)
                    {
                        if (item.isLabel || item.implicitClobbers.empty())
                            continue;
                        std::istringstream parts(item.implicitClobbers);
                        std::string one;
                        // (AR) الفاصل من الثابت المولَّد — مصدر واحد مع المولّد.
                        while (std::getline(parts, one, kImplicitClobberSep))
                            if (!one.empty() && declaredClobbers.insert(one).second)
                                constraintParts.push_back(one);
                    }
                }

                std::string constraints;
                for (std::size_t i = 0; i < constraintParts.size(); ++i)
                {
                    if (i)
                        constraints += ",";
                    constraints += constraintParts[i];
                }

                // ── ⑤ إصدار تعليمة INLINE_ASM ──
                SIRInstruction asmInst(SIROpcode::INLINE_ASM);
                asmInst.operands.push_back(
                    SIROperand::ConstantString(std::string(1, kRawLlvmAsmMarker) + tmpl.str()));
                asmInst.operands.push_back(SIROperand::ConstantString(constraints));

                // (AR) المدخلات بترتيب القيود: (أ) قيمة ابتدائيّة لكلّ inout، (ب) قراءة صِرفة.
                auto emitLoadInput = [&](const std::string &varName) {
                    ::Sad::AST::VariableExpr ve(varName);
                    BuildResult v = b_.buildExpression(&ve);
                    SIROperand inOp;
                    inOp.type = SIROperandType::REGISTER;
                    inOp.name = v.registerName;
                    inOp.dataType = v.type;
                    asmInst.operands.push_back(inOp);
                };
                for (const auto &w : writtenOrder)
                    if (isRead.count(w))
                        emitLoadInput(w);
                for (const auto &r : readOrder)
                    if (!isWritten.count(r))
                        emitLoadInput(r);

                // (AR) الخرج: سجلّ نتيجة مؤقّت (يُخزَّن لاحقًا إلى alloca المتغيّر).
                std::string resultReg;
                if (!writtenOrder.empty())
                {
                    resultReg = b_.newTempRegister();
                    SIROperand resOp;
                    resOp.type = SIROperandType::REGISTER;
                    resOp.name = resultReg;
                    resOp.dataType = SadTypeKind::Integer;
                    asmInst.result = resOp;
                }

                asmInst.comment = "asm dialect block (M1)";
                if (b_.currentBlock_)
                    b_.currentBlock_->instructions.push_back(asmInst);

                // ── ⑥ تخزين الخرج إلى متغيّر ص المكتوب ──
                // (AR) [تصحيح ٢] المتغيّر معرَّف قطعًا (تحقّقنا SEM031 أعلاه)؛ فإن غاب
                //      هنا فهو تناقض داخليّ لا تجاهل صامت — نُبلغ SEM031 احتياطًا.
                // (EN) [Fix 2] the var is guaranteed defined (SEM031 checked above);
                //      if somehow absent it's an internal inconsistency — report, don't drop.
                if (!writtenOrder.empty())
                {
                    VariableInfo *varInfo = b_.lookupVariable(writtenOrder[0]);
                    if (!varInfo)
                    {
                        emitAsmError(::Sad::Errors::ErrorCode::SEM_ASM_UNDEFINED_VAR,
                                     {{"name", writtenOrder[0]}});
                        return;
                    }
                    if (b_.currentBlock_)
                    {
                        SIRInstruction storeInst(SIROpcode::STORE);
                        SIROperand valueOp;
                        valueOp.type = SIROperandType::REGISTER;
                        valueOp.name = resultReg;
                        valueOp.dataType = SadTypeKind::Integer;
                        storeInst.operands.push_back(valueOp);

                        SIROperand ptrOp;
                        ptrOp.type = SIROperandType::REGISTER;
                        ptrOp.name = varInfo->registerName;
                        ptrOp.dataType = varInfo->type;
                        storeInst.operands.push_back(ptrOp);

                        storeInst.comment = "asm dialect: store output to " + writtenOrder[0];
                        b_.currentBlock_->instructions.push_back(storeInst);
                    }
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
