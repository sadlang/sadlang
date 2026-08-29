// ============================================================================
// (AR) تخفيضُ SIR إلى RISC-V RV64 — م٦، الهدفُ الثالثُ في «الخلفيّة السياديّة بلا LLVM».
//
//      ⚠️ **نطاقٌ مُعلَنٌ لا مُدَّعى**: هذا المخفّضُ لا يُغطّي أوپكودات SIR الـ١٠٣
//      التي يغطّيها هدفا x86-64/arm64. يُغطّي المجموعةَ التي تخفّضها قاعدةُ
//      «تصريح المتغيّر» (gr.decl.variable) فعلًا — ALLOC وSTORE وBUILTIN_PRINT
//      وRET_VOID — ومعها RET وحدَه لأنّه يقع في المسارِ نفسِه، ثمّ MOVE وNEG.
//      ⚠️ **تصحيحُ حكمٍ سابقٍ في هذا الموضع:** كان هنا أنّ MOVE (وLOAD وCONST_I64)
//      أُسقِطت «عمدًا بعد قياس» لأنّ نتائجَها مؤقّتاتٌ سجليّةٌ لا خاناتُ إطار. وقد
//      كان ذلك **خطأً في التشخيص لا قرارَ نطاق**: نتيجةُ MOVE تقبل خانةَ إطارٍ
//      كأيِّ ALLOC، وإسقاطُها جعل الهدفَ **معتمدًا على المُحسِّن**: `متغير س = 2048`
//      يُبنى معه `%N = move` ميّتٌ يحذفه DCE، فكان RV64 يمرُّ في ‑O2 ويُخفِق في ‑O0.
//      وكذلك NEG: `متغير س = ‑7` قبل الطيّ `%N = neg 7`. فالنطاقُ الصادقُ لقاعدةِ
//      تصريحِ المتغيّرِ يشملهما، ويقيسهما البرهانُ الحيُّ عند المستويَين معًا.
//      ويبقى المبدأُ قائمًا: الفرعُ الفارغُ يُلوِّن اللوحةَ خضراءَ ولا يُشغِّل برنامجًا.
//      كلُّ ما عداها **يفشل صراحةً** برمزِ خطأٍ من كتالوج SoT، لا يُخفَّض خطأً ولا
//      يُتجاهَل صامتًا: مخفّضٌ يبتلع أوپكودًا لا يعرفه يُنتج ثنائيًّا يعمل خطأً،
//      وهو أسوأُ من واحدٍ يرفض بوضوح.
//
//      البنيةُ عمدًا مستقلّةٌ عن LoweringDriver: ذاك مبنيٌّ حول جداولِ العمليّاتِ
//      الثنائيّة وتخصيصِ السجلّات، ولا تلزمُ منه لبنةٌ في هذه المجموعة. وصلُه يأتي
//      حين تتّسع المجموعةُ إلى الحساب والتفرّع (م٦-ب).
//
//      نموذجُ الإطار: كلُّ ALLOC خانةُ ٨ بايت في إطارِ sp؛ لا تخصيصَ سجلّاتٍ عابرًا
//      للتعليمات (السجلّاتُ t0..t6 وa0..a2 مؤقّتاتٌ داخلَ التعليمةِ الواحدة فقط)،
//      فينتفي بابُ «الالتفافُ يدهس سجلًّا حيًّا».
//      نصوصُ الحرفيّات تُلحَق بعد الشيفرة في المقطع نفسِه (R+X)، وعنوانُها المطلقُ
//      (vbase + إزاحة) يُرقَّع في زوجِ LUI+ADDI بعد معرفةِ حجمِ الشيفرة.
// (EN) SIR → RISC-V RV64 lowering (m6). Deliberately covers only the opcode set
//      that gr.decl.variable actually lowers to; everything else fails loudly with
//      an SoT error code rather than being silently mis-lowered.
// ============================================================================
#ifndef SAD_NATIVE_RISCV64_SIR_LOWERING_H
#define SAD_NATIVE_RISCV64_SIR_LOWERING_H

#include "backend/native/riscv64_fixed32_encoder.h"
#include "backend/native/elf64_writer.h"
#include "backend/native/sir_native_lowering.h"

#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

#include "error_codes.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace sad
{
    namespace native
    {
        namespace rv = ::sad::native::riscv64;

        class Riscv64SirLowering
        {
        public:
            using OP = sir::SIROpcode;
            using EC = ::Sad::Errors::ErrorCode;

            LoweringResult lowerModule(const sir::SIRModule &module)
            {
                LoweringResult r;
                // (AR) تصفيرٌ في المطلع: الصنفُ عامٌّ، ونداءٌ ثانٍ على الكائنِ نفسِه كان
                //      سيُلحِق شيفرةً بشيفرةٍ سابقةٍ ويُبقي خاناتٍ من وحدةٍ أخرى — عطبٌ
                //      لا يقع اليومَ (نقطةُ الدخولِ تُنشئ كائنًا جديدًا) لكنّه بابٌ مفتوح.
                code_.clear();
                rodata_.clear();
                slotOf_.clear();
                strFixups_.clear();
                frameBytes_ = 0;
                bufOff_ = 0;
                const sir::SIRFunction *entry = findEntry(module);
                if (!entry)
                {
                    fail(r, EC::INT_NATIVE_NO_ENTRY, std::string());
                    return r;
                }

                // (AR) هدفُ م٦: وحدةٌ بدالّةِ دخولٍ واحدة. وجودُ غيرِها ليس «تجاهلًا
                //      آمنًا»: نداءٌ إليها سيُخفَّض إلى لا شيء ⇒ نرفض بوضوح.
                if (module.getFunctions().size() != 1)
                {
                    fail(r, EC::INT_NATIVE_UNSUPPORTED, kMultiFunction);
                    return r;
                }

                if (!countSlots(*entry, r))
                    return r;

                emitPrologue();
                for (const auto &block : entry->basicBlocks)
                {
                    if (!block)
                        continue;
                    for (const auto &inst : block->instructions)
                        if (!lowerInst(inst, r))
                            return r;
                }
                // (AR) خاتمةٌ احتياطيّة: دالّةٌ بلا RET صريحٍ تخرج بصفرٍ لا تنساب إلى
                //      بايتاتِ النصوص فتُنفَّذ بياناتٍ.
                emitExit(0);

                appendRodataAndPatch();

                r.ok = true;
                r.code = code_;
                return r;
            }

        private:
            // ── ثوابتُ الإطار ───────────────────────────────────────────────
            static constexpr long long kSlotBytes = 8;
            static constexpr long long kBufBytes = 32;  // (AR) عازلُ تحويلِ العدد إلى عشريّ
            static constexpr long long kAlign = 16;

            // (AR) تفاصيلُ تشخيصٍ ({detail} في الكتالوج) — لا نثرَ رسائلَ هنا.
            static constexpr const char *kMultiFunction = "riscv64: multi-function module";
            static constexpr const char *kOpcode = "riscv64 opcode ";
            static constexpr const char *kPrintKind = "riscv64 print operand kind";
            // (AR) ع-٤/ع-٥ (مراجعةُ أميليا): كان فشلُ **قيمةِ** STORE يُبلَّغ بـkPrintKind
            //      فيُقرأ عيبَ طباعة، وكانت الطباعةُ متعدّدةُ المعاملات تُبلَّغ
            //      «معاملاتٌ غيرُ صالحة» — أي عيبٌ داخليٌّ في المُصدِر — والحقيقةُ أنّ
            //      SIR سليمٌ والهدفَ قاصر. لكلِّ حدٍّ اسمُه ورمزُه.
            static constexpr const char *kStoreValue = "riscv64 store value kind";
            static constexpr const char *kPrintArity = "riscv64: multi-operand print";
            static constexpr const char *kStoreDest = "riscv64 store destination is not a frame slot";
            static constexpr const char *kMoveDest = "riscv64 move result has no frame slot";
            static constexpr const char *kNegDest = "riscv64 neg result has no frame slot";
            static constexpr const char *kNegValue = "riscv64 neg value kind";
            static constexpr const char *kMoveValue = "riscv64 move value kind";
            static constexpr const char *kFrameTooLarge = "riscv64 frame exceeds 12-bit displacement";

            std::vector<uint8_t> code_;
            std::vector<uint8_t> rodata_;
            std::map<std::string, long long> slotOf_;
            long long frameBytes_ = 0;
            long long bufOff_ = 0;

            // (AR) موضعُ زوجِ LUI+ADDI الذي يحمل عنوانَ سلسلةٍ، وإزاحتُها في rodata_.
            struct StrFixup
            {
                size_t codeOff;
                size_t rodataOff;
            };
            std::vector<StrFixup> strFixups_;

            // (AR) يَسِمُ النتيجةَ بالفشلِ ويُرجع false ليكون `return fail(...)` صحيحًا
            //      في الموزِّع. (كانت صيغةُ `!fail(...).ok` تُرجع **true** عند الفشل
            //      فيمضي التخفيضُ كأنّ شيئًا لم يكن — عيبٌ يُنتج ثنائيًّا مبتورًا صامتًا.)
            static bool fail(LoweringResult &r, EC code, const std::string &detail)
            {
                r.ok = false;
                r.errorCode = code;
                r.detail = detail;
                return false;
            }

            const sir::SIRFunction *findEntry(const sir::SIRModule &m) const
            {
                static const std::string kMain = "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9"; // رئيسية
                for (const auto &name : {kMain, std::string("main"), std::string("__sad_main")})
                {
                    auto f = m.getFunction(name);
                    if (f)
                        return f.get();
                }
                const auto &fns = m.getFunctions();
                return fns.empty() ? nullptr : fns[0].get();
            }

            void put(const std::vector<uint8_t> &w) { code_.insert(code_.end(), w.begin(), w.end()); }

            // (AR) تحميلُ ثابتٍ ٦٤-بت: شرائحُ ١١ بتًّا **موجبةً** من الأعلى إلى الأدنى.
            //      اختيرت ١١ لا ١٢ عمدًا: ADDI يمدُّ الإشارة، فشريحةٌ ١٢-بتّيّةٌ قيمتُها
            //      ≥٢٠٤٨ تُطرَح بدل أن تُضاف — وهو بالضبط بابُ «الترميزُ صحيحٌ والقيمةُ
            //      خاطئة». بـ١١ بتًّا كلُّ شريحةٍ في [0, 2047] فلا إشارةَ أصلًا.
            void emitLi(int rd, long long value)
            {
                const uint64_t u = static_cast<uint64_t>(value);
                if (u == 0)
                {
                    put(rv::ADDI(rd, rv::ZERO, 0));
                    return;
                }
                int high = 63;
                while (high > 0 && ((u >> high) & 1ULL) == 0)
                    --high;
                // (AR) عدُّ الشرائح بحيث تبدأ الأولى من أعلى بتٍّ مرفوع.
                int chunks = (high / 11) + 1;
                bool first = true;
                for (int i = chunks - 1; i >= 0; --i)
                {
                    const int shift = i * 11;
                    const long long part = static_cast<long long>((u >> shift) & 0x7FFULL);
                    if (first)
                    {
                        put(rv::ADDI(rd, rv::ZERO, part));
                        first = false;
                    }
                    else
                    {
                        put(rv::SLLI(rd, rd, 11));
                        if (part != 0)
                            put(rv::ADDI(rd, rd, part));
                    }
                }
            }

            // ── الإطار ─────────────────────────────────────────────────────
            bool countSlots(const sir::SIRFunction &fn, LoweringResult &r)
            {
                long long n = 0;
                for (const auto &block : fn.basicBlocks)
                {
                    if (!block)
                        continue;
                    // (AR) خانةٌ لكلِّ ALLOC — ولكلِّ MOVE أيضًا: نتيجتُه سجلٌّ افتراضيٌّ
                    //      يُقرأ لاحقًا، وبلا خانةٍ له لا موضعَ لقيمته.
                    for (const auto &inst : block->instructions)
                        if ((inst.opcode == OP::ALLOC || inst.opcode == OP::MOVE ||
                             inst.opcode == OP::NEG) && inst.result)
                            slotOf_[inst.result->name] = kSlotBytes * n++;
                }
                bufOff_ = kSlotBytes * n;
                frameBytes_ = bufOff_ + kBufBytes;
                frameBytes_ = (frameBytes_ + kAlign - 1) / kAlign * kAlign;
                // (AR) إزاحاتُ الإطارِ كلُّها فوريٌّ ١٢-بت موقَّع ⇒ حدُّ ٢٠٤٧. تجاوزُه
                //      لا يُبتَر صامتًا (المُحرِّكُ يرمي) بل يُبلَّغ خطأً مفهومًا.
                if (frameBytes_ > 2047)
                    return fail(r, EC::INT_NATIVE_UNSUPPORTED, kFrameTooLarge);
                return true;
            }

            void emitPrologue() { put(rv::ADDI(rv::SP, rv::SP, -frameBytes_)); }

            void emitExit(long long status)
            {
                emitLi(rv::A0, status);
                emitLi(rv::A7, 93); // (AR) exit — يُرآي syscall.nr.exit في abi/riscv64-linux.yaml
                put(rv::ECALL());
            }

            // ── الوصولُ إلى المعامِلات ───────────────────────────────────────
            bool slotFor(const sir::SIROperand &op, long long &off) const
            {
                if (op.type != sir::SIROperandType::REGISTER)
                    return false;
                auto it = slotOf_.find(op.name);
                if (it == slotOf_.end())
                    return false;
                off = it->second;
                return true;
            }

            // (AR) يُحضِرُ قيمةَ المعامل في rd: ثابتٌ صحيحٌ أو خانةُ إطار. غيرُهما ⇒ لا.
            bool materialize(int rd, const sir::SIROperand &op)
            {
                if (op.type == sir::SIROperandType::CONSTANT &&
                    (op.dataType == types::SadTypeKind::Integer ||
                     op.dataType == types::SadTypeKind::Boolean))
                {
                    emitLi(rd, op.dataType == types::SadTypeKind::Boolean
                                   ? (op.boolValue ? 1 : 0)
                                   : static_cast<long long>(op.intValue));
                    return true;
                }
                long long off;
                if (slotFor(op, off))
                {
                    put(rv::LD(rd, rv::SP, off));
                    return true;
                }
                return false;
            }

            // ── الطباعة ────────────────────────────────────────────────────
            // (AR) write(1, buf, len) — الأرقامُ من ABI SoT (asm-generic: write=64).
            void emitWriteSyscall()
            {
                emitLi(rv::A7, 64);
                put(rv::ECALL());
            }

            // (AR) يطبع سلسلةً حرفيّة: تُلحَق في rodata_ ويُرقَّع عنوانُها لاحقًا.
            void emitPrintLiteral(const std::string &s)
            {
                const size_t off = rodata_.size();
                rodata_.insert(rodata_.end(), s.begin(), s.end());
                emitLi(rv::A0, 1);
                strFixups_.push_back({code_.size(), off});
                rv::appendAbs32(code_, rv::A1, 0); // (AR) عنوانٌ مؤقّتٌ يُرقَّع
                emitLi(rv::A2, static_cast<long long>(s.size()));
                emitWriteSyscall();
            }

            // (AR) يُرقِّع فرعًا مُصدَرًا سلفًا في `pos` ليقفز إلى `target`.
            //      الفرعُ يُصدَر بإزاحةٍ صفريّةٍ مؤقّتة ثمّ يُعاد ترميزُه هنا: طولُ
            //      كتلتَي الطباعةِ غيرُ معروفٍ قبل إصدارِها (emitLi متغيّرُ الطول).
            void patchBranch(size_t pos, int rs1, int rs2, size_t target, bool equal)
            {
                const long long disp = static_cast<long long>(target) - static_cast<long long>(pos);
                const std::vector<uint8_t> enc = equal ? rv::BEQ(rs1, rs2, disp)
                                                       : rv::BNE(rs1, rs2, disp);
                for (size_t i = 0; i < enc.size(); ++i)
                    code_[pos + i] = enc[i];
            }

            // (AR) يطبع منطقيًّا في السجلّ: صفرٌ ⇒ «خطأ»، وإلّا «صحيح».
            //      البنية: فرعٌ إلى كتلةِ الكذب · كتلةُ الصدق · قفزةٌ غيرُ مشروطة
            //      (beq zero,zero) إلى النهاية · كتلةُ الكذب. الفرعان يُرقَّعان بعد
            //      إصدارِ الكتلتين لأنّ طولَهما لا يُعرَف قبلَه.
            void emitPrintBool(int valReg)
            {
                const size_t posToFalse = code_.size();
                put(rv::BEQ(valReg, rv::ZERO, 0)); // (AR) يُرقَّع
                emitPrintLiteral(kDynBoolTrueText);
                const size_t posToEnd = code_.size();
                put(rv::BEQ(rv::ZERO, rv::ZERO, 0)); // (AR) قفزةٌ دائمة، تُرقَّع
                const size_t falseBlock = code_.size();
                emitPrintLiteral(kDynBoolFalseText);
                patchBranch(posToFalse, valReg, rv::ZERO, falseBlock, true);
                patchBranch(posToEnd, rv::ZERO, rv::ZERO, code_.size(), true);
            }

            // (AR) يطبع عددًا صحيحًا موقَّعًا في t0. الخوارزميّة: بناءُ الأرقام من
            //      نهايةِ العازلِ إلى بدايته بـremu/divu، ثمّ إلحاقُ الإشارة.
            //      السالبُ يُحوَّل بـ`sub t0, zero, t0` ثمّ يُعامَل **لا-موقَّعًا** —
            //      وهو ما يجعل أصغرَ عددٍ (−2^63) صحيحًا بلا حالةٍ خاصّة: نمطُ بتّاته
            //      بعد النفي هو 2^63 نفسُه، وهو مقدارُه المطلوب لا-موقَّعًا.
            //      ⚠️ و`unsignedValue` ليس ترفًا: بدونه كان `طبيعي` بقيمة 2^64−1
            //      يُطبَع **`-1`** على RV64 بينما المفسّرُ وx86_64 يطبعان القيمةَ
            //      الصحيحة — تباعُدٌ صامتٌ يخرج بصفرٍ بلا تشخيص. وحارسُ هذا الملفِّ
            //      مبنيٌّ على **الأوپكود**، وهذا فرقٌ في **النوع**، فلم يره.
            //      (كشفته مراجعةُ أميليا الخصميّة — ع-١، مقيسٌ بالتشغيل.)
            void emitPrintInt(int srcReg, bool unsignedValue = false)
            {
                const int V = rv::T0;   // (AR) القيمة (تُستهلَك بالقسمة)
                const int P = rv::T1;   // (AR) مؤشّرُ الكتابة (يتناقص)
                const int TEN = rv::T2; // (AR) الثابت ١٠
                const int D = rv::T3;   // (AR) الرقم الحاليّ
                const int NEG = rv::T4; // (AR) علَمُ السالب
                const int END = rv::T5; // (AR) واحدٌ بعد نهايةِ العازل

                if (srcReg != V)
                    put(rv::ADDI(V, srcReg, 0));

                put(rv::ADDI(END, rv::SP, bufOff_ + kBufBytes));
                put(rv::ADDI(P, END, -1));
                put(rv::ADDI(NEG, rv::ZERO, 0));

                // (AR) اللا-موقَّعُ لا إشارةَ له: كتلةُ النفيِ كلُّها تُسقَط، وNEG يبقى صفرًا
                //      فتُتخطّى كتابةُ «−» زمنَ التشغيلِ بالفرعِ القائمِ أصلًا.
                if (!unsignedValue)
                {
                    // (AR) إن كان موجبًا (أو صفرًا) تخطَّ كتلتَي النفي. الإزاحةُ تُقاس من
                    //      عنوانِ الفرعِ نفسِه: ٤ (الفرع) + ٨ (تعليمتان) = ١٢.
                    put(rv::BGE(V, rv::ZERO, 12));
                    put(rv::SUB(V, rv::ZERO, V));
                    put(rv::ADDI(NEG, rv::ZERO, 1));
                }

                put(rv::ADDI(TEN, rv::ZERO, 10));

                // (AR) حلقةٌ «افعل ثمّ افحص»: الصفرُ يمرّ مرّةً فيُطبَع «0» بلا حالةٍ خاصّة.
                const size_t loopStart = code_.size();
                put(rv::REMU(D, V, TEN));
                put(rv::ADDI(D, D, 48)); // '0'
                put(rv::SB(D, P, 0));
                put(rv::ADDI(P, P, -1));
                put(rv::DIVU(V, V, TEN));
                const long long back = static_cast<long long>(loopStart) -
                                       static_cast<long long>(code_.size());
                put(rv::BNE(V, rv::ZERO, back));

                // (AR) الإشارة: إن كان العلَمُ صفرًا تخطَّ تعليمتَي كتابةِ «−».
                put(rv::BEQ(NEG, rv::ZERO, 16)); // (AR) ثلاثُ تعليماتٍ تُتخطّى
                put(rv::ADDI(D, rv::ZERO, 45)); // '-'
                put(rv::SB(D, P, 0));
                put(rv::ADDI(P, P, -1));

                // (AR) العنوان = P+1، والطولُ = END − (P+1).
                put(rv::ADDI(rv::A1, P, 1));
                put(rv::SUB(rv::A2, END, rv::A1));
                emitLi(rv::A0, 1);
                emitWriteSyscall();
            }

            // ── الموزِّع ────────────────────────────────────────────────────
            bool lowerInst(const sir::SIRInstruction &inst, LoweringResult &r)
            {
                switch (inst.opcode)
                {
                case OP::ALLOC:
                    // (AR) الخانةُ حُجزت في countSlots؛ لا شيفرةَ زمنَ تشغيلٍ لازمة.
                    return true;

                case OP::STORE:
                {
                    // (AR) [القيمة، الوجهة] — الترتيبُ نفسُه في هدفَي x86/arm64.
                    if (inst.operands.size() != 2)
                        return fail(r, EC::INT_COMPILER_INVALID_OPERANDS, kOpcode);
                    long long dst;
                    if (!slotFor(inst.operands[1], dst))
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kStoreDest);
                    if (!materialize(rv::T0, inst.operands[0]))
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kStoreValue);
                    put(rv::SD(rv::T0, rv::SP, dst));
                    return true;
                }

                case OP::NEG:
                {
                    // (AR) الحرفُ السالبُ **ليس ثابتًا** قبلَ الطيّ: `متغير س = -7` يُبنى
                    //      في -O0 `%1 = neg 7`. فبلا هذا الفرعِ يعمل الموجبُ ويُخفِقُ
                    //      السالبُ عند -O0 وحدَه — تباعُدٌ مشروطٌ بمستوى التحسين.
                    if (inst.operands.size() != 1 || !inst.result)
                        return fail(r, EC::INT_COMPILER_INVALID_OPERANDS, kOpcode);
                    auto it = slotOf_.find(inst.result->name);
                    if (it == slotOf_.end())
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kNegDest);
                    if (!materialize(rv::T0, inst.operands[0]))
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kNegValue);
                    put(rv::SUB(rv::T0, rv::ZERO, rv::T0)); // (AR) sub t0, zero, t0
                    put(rv::SD(rv::T0, rv::SP, it->second));
                    return true;
                }

                case OP::MOVE:
                {
                    // (AR) نسخُ قيمةٍ إلى سجلٍّ افتراضيّ. أُضيف لأنّ غيابَه كان يجعل هذا
                    //      الهدفَ **يعتمد على المُحسِّن**: `متغير س = 2048` يُبنى معه
                    //      `%0 = move 2048` ميتًّا يحذفه DCE — فيمرُّ التخفيضُ في -O2
                    //      ويُخفِق في **-O0** بـ«أوپكود ٣٩ غير مدعوم»، بينما x86_64
                    //      وarm64 يمرّان في المستويَين. قِيس بالتشغيل على الثلاثة.
                    if (inst.operands.size() != 1 || !inst.result)
                        return fail(r, EC::INT_COMPILER_INVALID_OPERANDS, kOpcode);
                    auto it = slotOf_.find(inst.result->name);
                    if (it == slotOf_.end())
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kMoveDest);
                    if (!materialize(rv::T0, inst.operands[0]))
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kMoveValue);
                    put(rv::SD(rv::T0, rv::SP, it->second));
                    return true;
                }

                case OP::BUILTIN_PRINT:
                {
                    // (AR) `اطبع_سطر("أ"، "ب")` يُبنى BUILTIN_PRINT واحدًا بمعاملَين
                    //      (بُناةُ المدمجات، وarm64 يدور عليها). فتعدُّدُها SIR سليمٌ
                    //      وقصورٌ في هذا الهدف — لا «معاملاتٌ غيرُ صالحة».
                    if (inst.operands.size() != 1)
                        return fail(r, EC::INT_NATIVE_UNSUPPORTED, kPrintArity);
                    const sir::SIROperand &op = inst.operands[0];
                    if (op.type == sir::SIROperandType::CONSTANT &&
                        op.dataType == types::SadTypeKind::String)
                    {
                        emitPrintLiteral(op.name);
                        return true;
                    }
                    if (materialize(rv::T0, op))
                    {
                        // (AR) ع-٦: المنطقيُّ يُطبَع «صحيح»/«خطأ» كالمفسّرِ وarm64، لا «1»/«0».
                        //      الكلمتان من `types::repr` المولَّدِ من مصدر الحقيقة لا مكتوبتَين هنا.
                        if (op.dataType == types::SadTypeKind::Boolean)
                            emitPrintBool(rv::T0);
                        else if (op.dataType == types::SadTypeKind::UInt64)
                            emitPrintInt(rv::T0, true); // (AR) ع-١: طبيعي لا-موقَّع
                        else
                            emitPrintInt(rv::T0);
                        return true;
                    }
                    return fail(r, EC::INT_NATIVE_UNSUPPORTED, kPrintKind);
                }

                case OP::RET:
                case OP::RET_VOID:
                    // (AR) دالّةُ الدخولِ وحدَها في هذا النطاق ⇒ الإرجاعُ خروجٌ بصفر.
                    //      قيمةُ RET لا تصير رمزَ خروجٍ عمدًا: المفسّرُ لا يفعل ذلك،
                    //      وفعلُه هنا تباعُدٌ بين المحرّكين لا ميزة.
                    emitExit(0);
                    return true;

                default:
                    return fail(r, EC::INT_NATIVE_UNSUPPORTED,
                                kOpcode + std::to_string(static_cast<int>(inst.opcode)));
                }
            }

            // ── إلحاقُ النصوص وترقيعُ عناوينها ───────────────────────────────
            void appendRodataAndPatch()
            {
                const size_t rodataBase = code_.size();
                code_.insert(code_.end(), rodata_.begin(), rodata_.end());
                for (const StrFixup &f : strFixups_)
                {
                    const uint64_t addr = elf::kDefaultVBase + elf::kCodeOffset +
                                          static_cast<uint64_t>(rodataBase + f.rodataOff);
                    std::vector<uint8_t> pair;
                    rv::appendAbs32(pair, rv::A1, addr);
                    for (size_t i = 0; i < pair.size(); ++i)
                        code_[f.codeOff + i] = pair[i];
                }
            }
        };

        // (AR) نقطةُ الدخول: تخفيضٌ ثمّ تغليفٌ في ELF64 بـe_machine=243 (EM_RISCV)
        //      المقروءِ من abi/riscv64-linux.yaml — نفسُ الكاتبِ المحايدِ للمعماريّة.
        inline LoweringResult lowerModuleToElfRiscv64(const sir::SIRModule &module)
        {
            Riscv64SirLowering low;
            LoweringResult r = low.lowerModule(module);
            if (!r.ok)
                return r;
            r.code = elf::writeStaticExec(r.code, elf::kEmRiscv64);
            return r;
        }

    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_RISCV64_SIR_LOWERING_H
