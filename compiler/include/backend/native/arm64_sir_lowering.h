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
                for (const auto &blockPtr : entry->getBasicBlocks())
                    if (!lowerBlock(*blockPtr))
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

            bool lowerBlock(const sir::SIRBasicBlock &block)
            {
                for (const sir::SIRInstruction &inst : block.instructions)
                    if (!lowerInstruction(inst))
                        return false;
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
