// ============================================================================
// (AR) جسر SIR→AArch64 أصليّ — أوّل ترجمةِ «لغة ص → شيفرة ARM64» بلا LLVM. يُبرهِن أنّ
//      الخطّ (SIR الأماميّ → تخفيضٌ خلفيّ → محرّكٌ جدوليّ → كاتب ELF) عامٌّ عبر صنفَي
//      ISA: يستهلك نفسَ SIR الذي يبنيه الأمامُ من مصدر ص (وضعٌ حرّ)، ويخفّض مجموعةَ
//      الحساب الصحيح (MOVE ثابت/سجلّ، ADD/SUB/MUL/FLOOR_DIV/MOD) إلى كلماتِ ARM64
//      ثابتةِ العرض عبر المحرّك الجدوليّ (lookupEncSpec + encodeFixed32)، ثمّ يلفّها
//      كاتبُ ELF (e_machine=EM_AARCH64) في تنفيذيٍّ ساكن يخرج عبر svc بقيمةِ `رئيسية`.
//      نطاقٌ مُعلَن (م٤-أ): دالّةٌ واحدةٌ (الداخلة)، كتلةٌ خطّيّة، حسابٌ صحيحٌ وثوابتُ
//      ≤١٦-بت؛ كلُّ ما عداه (فروع/نداءات/ذاكرة/طباعة/سالب/ثابتٌ كبير) يفشل صراحةً لا
//      يُترجَم خطأً — نظيرُ انضباطِ جسر x86. لا clang/lld/as (متحقَّقٌ حيًّا على qemu).
// (EN) SIR→native AArch64 bridge — first "S-lang → ARM64" lowering with no LLVM.
//      Proves the pipeline (frontend SIR → backend lowering → table-driven encoder →
//      ELF writer) generalizes across two ISA classes. Lowers integer arithmetic to
//      fixed-width ARM64 words and exits via svc. Declared scope (m4-a): single entry
//      function, linear block, integer arithmetic with ≤16-bit constants; everything
//      else fails loudly. Byte-verified vs llvm-mc; live-proven on qemu-aarch64.
// ============================================================================
#ifndef SAD_NATIVE_ARM64_SIR_LOWERING_H
#define SAD_NATIVE_ARM64_SIR_LOWERING_H

#include "backend/native/arm64_fixed32_encoder.h"
#include "backend/native/generated/arm64_encoding_generated.h"
#include "backend/native/elf64_writer.h"

#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

#include "error_codes.h"
#include "error_messages_generated.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// (AR) يعيد استعمالَ LoweringResult من جسر x86 (نفسُ العقد: بايتاتٌ أو ErrorCode+detail).
#include "backend/native/sir_native_lowering.h"

namespace sad
{
    namespace native
    {
        namespace a64 = ::sad::native::arm64;

        // (AR) أرقامُ نداءات AArch64/Linux وثوابتُ السجلّات (من abi/aarch64-linux.yaml).
        //      ثوابتُ مسمّاةٌ لا أرقامٌ سحريّة.
        inline constexpr long long kSysExitArm64 = 93; // (AR) exit
        namespace a64reg
        {
            inline constexpr int kX0 = 0;    // (AR) قيمةُ الإرجاع/الوسيط الأوّل (رمزُ الخروج)
            inline constexpr int kX8 = 8;    // (AR) رقمُ النداء (svc)
            inline constexpr int kScratch0 = 16; // (AR) x16 (IP0) مُبدَّدٌ لتجهيز المعامل الأوّل
            inline constexpr int kScratch1 = 17; // (AR) x17 (IP1) مُبدَّدٌ لتجهيز المعامل الثاني
            inline constexpr long long kImm16Max = 0xFFFF; // (AR) أقصى فوريّ لـMOVZ (بلا MOVK بعد)
        } // namespace a64reg

        // (AR) مخفّضُ SIR→AArch64 للحساب الصحيح (نطاق م٤-أ). صنفٌ مستقلٌّ عن X86SirLowering
        //      عمدًا: يبقى مسارُ x86 المُبرهَنُ سليمًا بلا مساس، والمشتركُ لاحقًا يُستخرَج
        //      خلف واجهةٍ حين ينضج المساران (تجريدٌ بعد برهانٍ لا قبله).
        class Arm64SirLowering
        {
        public:
            LoweringResult lowerModule(const sir::SIRModule &module)
            {
                LoweringResult r;
                const sir::SIRFunction *entry = findEntry(module);
                if (!entry)
                    return finishError(r, EC::INT_NATIVE_NO_ENTRY);

                // (AR) النطاقُ الحاليّ: الدالّةُ الداخلة وحدها (لا نداءات) — نقطةُ الدخول = code_[0].
                //      مرورُ طبقتين: (١) أصدِر كتلَ الدالّة بالترتيب مسجّلًا إزاحةَ لصيقةِ كلٍّ،
                //      وأصدِر الفروعَ بإزاحةٍ صفريّةٍ نائبة مسجّلًا ترقيعًا؛ (٢) رقّع كلَّ فرعٍ
                //      بالفرق النسبيّ ÷٤ (عددُ تعليمات) في حقلِ imm19/imm26 (تعبئةٌ بتّيّة).
                for (const auto &blockPtr : entry->getBasicBlocks())
                {
                    labelOffset_[blockPtr->name] = code_.size();
                    if (!lowerBlock(*blockPtr))
                        return finishError(r, errorCode_, detail_);
                }
                if (!applyFixups())
                    return finishError(r, errorCode_, detail_);

                r.ok = true;
                r.code = std::move(code_);
                return r;
            }

        private:
            using EC = ::Sad::Errors::ErrorCode;

            std::map<std::string, int> regOf_;                                     // (AR) سجلّ افتراضيّ ⇒ فيزيائيّ
            const std::vector<int> pool_{9, 10, 11, 12, 13, 14, 15};               // (AR) x9..x15 (caller-saved)
            size_t next_ = 0;
            std::vector<uint8_t> code_;
            EC errorCode_ = EC::INT_NATIVE_NO_ENTRY;
            std::string detail_;

            // (AR) تدفّق التحكّم: خريطةُ لصيقةِ الكتلة ⇒ إزاحتُها في code_، وطابورُ ترقيعِ الفروع.
            //      كلُّ فرعٍ يحمل موضعَ كلمتِه وحقلَ إزاحته (imm19 لـb.cond، imm26 لـb) واللصيقةَ.
            std::map<std::string, size_t> labelOffset_;
            struct Arm64Fixup
            {
                size_t wordPos;     // (AR) موضعُ كلمةِ الفرع (٤ بايت) في code_
                int immHi;          // (AR) أعلى بتّةٍ لحقل الإزاحة (٢٣ لـimm19، ٢٥ لـimm26)
                int immLo;          // (AR) أدنى بتّة (٥ لـimm19، ٠ لـimm26)
                std::string target; // (AR) لصيقةُ الكتلة الهدف
            };
            std::vector<Arm64Fixup> fixups_;

            LoweringResult &finishError(LoweringResult &r, EC code, const std::string &detail = "")
            {
                r.ok = false;
                r.errorCode = code;
                r.detail = detail;
                return r;
            }
            bool fail(EC code, const std::string &detail = "")
            {
                errorCode_ = code;
                detail_ = detail;
                return false;
            }
            static std::string detailOpcode(const sir::SIRInstruction &i)
            {
                return "opcode=" + std::to_string(static_cast<int>(i.opcode));
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

            // ── مُصدِرات التعليمات (كلٌّ يقرأ مواصفتَه من الجدول المولَّد من SoT) ──
            bool emit(const std::string &mnemonic, const std::string &form,
                      const std::vector<a64::Operand> &ops)
            {
                const a64::EncSpec *spec = a64::lookupEncSpec(mnemonic, form);
                if (!spec)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + " " + form);
                auto bytes = a64::encodeFixed32(*spec, ops);
                code_.insert(code_.end(), bytes.begin(), bytes.end());
                return true;
            }
            bool movz(int reg, long long imm)
            {
                if (imm < 0 || imm > a64reg::kImm16Max) // (AR) MOVZ يحمّل ١٦ بتًّا فقط؛ الأكبر/السالب غيرُ مدعومٍ بعد
                    return fail(EC::INT_NATIVE_IMM_RANGE, "u16:" + std::to_string(imm));
                return emit(a64::mnem::kMovz, "x, imm16", {a64::Operand::R(reg), a64::Operand::I(imm)});
            }
            bool movReg(int dst, int src)
            {
                if (dst == src)
                    return true;
                return emit(a64::mnem::kMov, "x, x", {a64::Operand::R(dst), a64::Operand::R(src)});
            }
            bool rrr(const std::string &mnemonic, int d, int n, int m)
            {
                return emit(mnemonic, "x, x, x",
                            {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)});
            }
            bool msub(int d, int n, int m, int a)
            {
                return emit(a64::mnem::kMsub, "x, x, x, x",
                            {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m), a64::Operand::R(a)});
            }
            bool cmp(int n, int m) // (AR) cmp Xn, Xm = subs xzr,Xn,Xm ⇒ يضبط الأعلام
            {
                return emit(a64::mnem::kCmp, "x, x", {a64::Operand::R(n), a64::Operand::R(m)});
            }
            // (AR) يُصدر فرعًا (b/b.cond) بإزاحةٍ صفريّةٍ نائبة، ويسجّل ترقيعًا بحقلِ إزاحته.
            //      عرضُ الحقل وموضعُه من مواصفة الترميز (immHi/immLo) لا من ثابتٍ مُرمَّز.
            bool emitBranch(const std::string &mnemonic, const std::string &form,
                            const std::string &targetLabel)
            {
                const a64::EncSpec *spec = a64::lookupEncSpec(mnemonic, form);
                if (!spec)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + " " + form);
                // (AR) اعثر على الحقل المأخوذ من المعامل (op0) = حقلُ الإزاحة (imm19/imm26).
                int immHi = -1, immLo = -1;
                for (const auto &f : spec->fields)
                    if (!f.is_const && f.from_op == 0)
                    {
                        immHi = f.hi;
                        immLo = f.lo;
                    }
                if (immHi < 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + ":no-imm-field");
                const size_t wordPos = code_.size();
                if (!emit(mnemonic, form, {a64::Operand::I(0)})) // (AR) إزاحةٌ نائبةٌ صفريّة
                    return false;
                fixups_.push_back({wordPos, immHi, immLo, targetLabel});
                return true;
            }

            bool allocReg(const std::string &vreg, int &out)
            {
                auto it = regOf_.find(vreg);
                if (it != regOf_.end())
                {
                    out = it->second;
                    return true;
                }
                if (next_ >= pool_.size())
                    return fail(EC::INT_NATIVE_REGALLOC_EXHAUSTED, "pool=" + std::to_string(pool_.size()));
                out = pool_[next_++];
                regOf_[vreg] = out;
                return true;
            }

            static bool isConstInt(const sir::SIROperand &op, long long &out)
            {
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Integer)
                {
                    out = op.intValue;
                    return true;
                }
                return false;
            }

            // (AR) يُجهّز معاملًا في سجلٍّ مُبدَّد: ثابتٌ ⇒ movz؛ سجلٌّ افتراضيٌّ ⇒ نسخٌ من موضعه.
            bool materialize(int scratch, const sir::SIROperand &op)
            {
                long long c;
                if (isConstInt(op, c))
                    return movz(scratch, c);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + op.name);
                    return movReg(scratch, it->second);
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "operand-kind=" + std::to_string(static_cast<int>(op.type)));
            }

            bool lowerBinary(const sir::SIRInstruction &inst)
            {
                using OP = sir::SIROpcode;
                if (!inst.result || inst.operands.size() != 2)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                // (AR) جهّز المعامِلَين في x16/x17 (مُبدَّدان خارجَ الحوض) ثمّ أصدِر العمليّةَ الثلاثيّة.
                if (!materialize(a64reg::kScratch0, inst.operands[0]) ||
                    !materialize(a64reg::kScratch1, inst.operands[1]))
                    return false;
                switch (inst.opcode)
                {
                case OP::ADD_I64: return rrr(a64::mnem::kAdd, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SUB_I64: return rrr(a64::mnem::kSub, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::MUL_I64: return rrr(a64::mnem::kMul, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::FLOOR_DIV_I64:
                    return rrr(a64::mnem::kSdiv, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::MOD_I64:
                    // (AR) الباقي = a − (a÷b)×b: sdiv dst=الحاصل، ثمّ msub dst = x16 − dst×x17.
                    //      MSUB يقرأ Xn(=dst الحاصل) قبل أن يكتب Rd(=dst) ⇒ إعادةُ استعمالِ dst آمنة.
                    return rrr(a64::mnem::kSdiv, dst, a64reg::kScratch0, a64reg::kScratch1) &&
                           msub(dst, dst, a64reg::kScratch1, a64reg::kScratch0);
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            bool lowerInstruction(const sir::SIRInstruction &inst)
            {
                using OP = sir::SIROpcode;
                switch (inst.opcode)
                {
                case OP::MOVE:
                {
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    long long c;
                    if (isConstInt(inst.operands[0], c))
                        return movz(dst, c);
                    if (inst.operands[0].type == sir::SIROperandType::REGISTER)
                    {
                        auto it = regOf_.find(inst.operands[0].name);
                        if (it == regOf_.end())
                            return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + inst.operands[0].name);
                        return movReg(dst, it->second);
                    }
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "move-kind");
                }
                case OP::ADD_I64:
                case OP::SUB_I64:
                case OP::MUL_I64:
                case OP::MOD_I64:
                case OP::FLOOR_DIV_I64:
                    return lowerBinary(inst);
                case OP::RET:
                {
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) الدالّةُ الداخلة تُنهي البرنامجَ: x0=قيمةُ الإرجاع، x8=exit، svc #0.
                    return materialize(a64reg::kX0, inst.operands[0]) &&
                           movz(a64reg::kX8, kSysExitArm64) &&
                           emit(a64::mnem::kSvc, "", {});
                }
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // ── تدفّق التحكّم: المقارنة المدموجة + الفروع + الترقيع ──

            // (AR) هل النوعُ صحيحٌ لا-موقَّع؟ (طبيعي8/16/32/64 أو بايت) — يلزمه فرعٌ لا-موقَّع.
            static bool isUnsignedType(types::SadTypeKind t)
            {
                using K = types::SadTypeKind;
                return t == K::UInt8 || t == K::UInt16 || t == K::UInt32 ||
                       t == K::UInt64 || t == K::Byte;
            }

            static bool isComparison(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                return op == OP::EQ || op == OP::NE || op == OP::LT ||
                       op == OP::LE || op == OP::GT || op == OP::GE;
            }

            // (AR) منمنمةُ الفرع الشرطيّ المطابقة للمقارنة (موقَّعة): «إن صحّ الشرط اقفز لـthen».
            static const std::string *bccForCmp(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: return &a64::mnem::kBeq;
                case OP::NE: return &a64::mnem::kBne;
                case OP::LT: return &a64::mnem::kBlt;
                case OP::LE: return &a64::mnem::kBle;
                case OP::GT: return &a64::mnem::kBgt;
                case OP::GE: return &a64::mnem::kBge;
                default: return nullptr;
                }
            }

            // (AR) يجد مقارنةً في الكتلة نتيجتُها cond تُغذّي BR_COND المُنهيَ لها (آخرَ تعليمةٍ قبله).
            const sir::SIRInstruction *findFusedComparison(const sir::SIRBasicBlock &block,
                                                           const std::string &condName) const
            {
                const auto &is = block.instructions;
                if (is.size() < 2)
                    return nullptr;
                const sir::SIRInstruction &prev = is[is.size() - 2];
                if (isComparison(prev.opcode) && prev.result && prev.result->name == condName)
                    return &prev;
                return nullptr;
            }

            // (AR) القفزُ غير المشروط BR: operands[0] لصيقةُ الهدف ⇒ b (rel26).
            bool lowerBranch(const sir::SIRInstruction &inst)
            {
                if (inst.operands.size() != 1 || inst.operands[0].type != sir::SIROperandType::LABEL)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                return emitBranch(a64::mnem::kB, "rel26", inst.operands[0].name);
            }

            // (AR) القفزُ المشروط BR_COND: {condition, thenLabel, elseLabel}. شرطٌ ثابتٌ منطقيّ
            //      ⇒ b مباشر؛ أو نتيجةُ مقارنةٍ مدموجةٍ ⇒ cmp؛ b.cond then؛ b else.
            bool lowerBranchCond(const sir::SIRInstruction &inst,
                                 const sir::SIRBasicBlock &block)
            {
                if (inst.operands.size() != 3 ||
                    inst.operands[1].type != sir::SIROperandType::LABEL ||
                    inst.operands[2].type != sir::SIROperandType::LABEL)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                const sir::SIROperand &cond = inst.operands[0];
                const std::string &thenLbl = inst.operands[1].name;
                const std::string &elseLbl = inst.operands[2].name;

                if (cond.type == sir::SIROperandType::CONSTANT)
                {
                    if (cond.dataType != types::SadTypeKind::Boolean)
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "cond-const-type=" + std::to_string(static_cast<int>(cond.dataType)));
                    return emitBranch(a64::mnem::kB, "rel26", cond.boolValue ? thenLbl : elseLbl);
                }
                if (cond.type != sir::SIROperandType::REGISTER)
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                "cond-kind=" + std::to_string(static_cast<int>(cond.type)));

                const sir::SIRInstruction *cmpInst = findFusedComparison(block, cond.name);
                if (!cmpInst || cmpInst->operands.size() != 2)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "cond-not-fused-cmp:%" + cond.name);
                // (AR) الفروعُ الشرطيّةُ المُصدَرة موقَّعةٌ (b.lt/le/gt/ge)؛ معاملٌ لا-موقَّعٌ (طبيعي64/
                //      بايت) يلزمه b.lo/ls/hi/hs. لا نظائرَ لا-موقَّعةٍ في opcodes المقارنة بعد،
                //      فنرفض المعاملَ اللا-موقَّعَ صراحةً بدل ترميزٍ موقَّعٍ خاطئٍ صامت (توصية أميليا).
                for (const auto &cop : cmpInst->operands)
                    if (isUnsignedType(cop.dataType))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "cmp-unsigned-type=" + std::to_string(static_cast<int>(cop.dataType)));
                const std::string *bcc = bccForCmp(cmpInst->opcode);
                if (!bcc)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(*cmpInst));
                // (AR) جهّز طرفَي المقارنة في x16/x17، cmp، ثمّ b.cond then؛ b else.
                return materialize(a64reg::kScratch0, cmpInst->operands[0]) &&
                       materialize(a64reg::kScratch1, cmpInst->operands[1]) &&
                       cmp(a64reg::kScratch0, a64reg::kScratch1) &&
                       emitBranch(*bcc, "rel19", thenLbl) &&
                       emitBranch(a64::mnem::kB, "rel26", elseLbl);
            }

            bool lowerBlock(const sir::SIRBasicBlock &block)
            {
                const auto &is = block.instructions;
                // (AR) نموذجُ «كلٌّ في سجلّ» لا يحمل حياةً عابرةً للكتل: كلُّ كتلةٍ تبدأ بحوضٍ نظيف؛
                //      فقراءةُ سجلٍّ عُرِّف في كتلةٍ أخرى (PHI ضمنيّ) تفشل صراحةً UNDEF_VREG.
                regOf_.clear();
                next_ = 0;

                // (AR) المقارنةُ المدموجة (تغذّي BR_COND المُنهي) لا تُخفَّض مستقلّةً.
                const sir::SIRInstruction *fused = nullptr;
                if (!is.empty() && is.back().opcode == sir::SIROpcode::BR_COND &&
                    !is.back().operands.empty() &&
                    is.back().operands[0].type == sir::SIROperandType::REGISTER)
                    fused = findFusedComparison(block, is.back().operands[0].name);

                for (const sir::SIRInstruction &inst : is)
                {
                    if (&inst == fused)
                        continue;
                    if (inst.opcode == sir::SIROpcode::BR)
                    {
                        if (!lowerBranch(inst))
                            return false;
                    }
                    else if (inst.opcode == sir::SIROpcode::BR_COND)
                    {
                        if (!lowerBranchCond(inst, block))
                            return false;
                    }
                    else if (!lowerInstruction(inst))
                        return false;
                }
                return true;
            }

            // (AR) المرور ٢: يرقّع كلَّ فرعٍ بالإزاحة النسبيّة ÷٤ (عددُ تعليمات) في حقلِ imm
            //      داخلَ كلمتِه ٣٢-بت (تعبئةٌ بتّيّة: قناعٌ ثمّ OR؛ الحقلُ نائبُه صفر أصلًا).
            bool applyFixups()
            {
                for (const Arm64Fixup &fx : fixups_)
                {
                    auto it = labelOffset_.find(fx.target);
                    if (it == labelOffset_.end())
                        return fail(EC::INT_NATIVE_LABEL_UNDEFINED, fx.target);
                    const long long dispBytes = static_cast<long long>(it->second) -
                                                static_cast<long long>(fx.wordPos);
                    if (dispBytes % 4 != 0)
                        return fail(EC::INT_NATIVE_IMM_RANGE, "unaligned:" + std::to_string(dispBytes));
                    const long long imm = dispBytes / 4; // (AR) الإزاحةُ عددُ تعليماتٍ موقَّع
                    const int width = fx.immHi - fx.immLo + 1;
                    const long long lim = 1LL << (width - 1);
                    if (imm < -lim || imm > lim - 1)
                        return fail(EC::INT_NATIVE_IMM_RANGE, "rel:" + std::to_string(imm));
                    const uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
                    const uint32_t field = (static_cast<uint32_t>(imm) & mask) << fx.immLo;
                    // (AR) اقرأ الكلمةَ LE، أدخِل حقلَ الإزاحة (نائبُه صفر) بـOR، ثمّ أعِدها LE.
                    uint32_t w = 0;
                    for (int i = 0; i < 4; ++i)
                        w |= static_cast<uint32_t>(code_[fx.wordPos + i]) << (8 * i);
                    w |= field;
                    for (int i = 0; i < 4; ++i)
                        code_[fx.wordPos + i] = static_cast<uint8_t>((w >> (8 * i)) & 0xFF);
                }
                return true;
            }
        };

        // (AR) مُيسِّرٌ عالي المستوى: SIRModule ⇒ ثنائيُّ ELF64 ساكن (AArch64) قابل للتنفيذ.
        //      e_machine=EM_AARCH64 من جدول الـABI ⇒ الكاتبُ نفسُه يخدم x86 وARM64.
        inline LoweringResult lowerModuleToElfArm64(const sir::SIRModule &module)
        {
            Arm64SirLowering low;
            LoweringResult r = low.lowerModule(module);
            if (!r.ok)
                return r;
            r.code = elf::writeStaticExec(r.code, elf::kEmAArch64);
            return r;
        }

    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_ARM64_SIR_LOWERING_H
