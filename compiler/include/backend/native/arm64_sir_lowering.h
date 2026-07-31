// ============================================================================
// (AR) جسر SIR→AArch64 أصليّ — أوّل ترجمةِ «لغة ص → شيفرة ARM64» بلا LLVM. يُبرهِن أنّ
//      الخطّ (SIR الأماميّ → تخفيضٌ خلفيّ → محرّكٌ جدوليّ → كاتب ELF) عامٌّ عبر صنفَي
//      ISA: يستهلك نفسَ SIR الذي يبنيه الأمامُ من مصدر ص (وضعٌ حرّ)، ويخفّضه إلى كلماتِ
//      ARM64 ثابتةِ العرض عبر المحرّك الجدوليّ (lookupEncSpec + encodeFixed32)، ثمّ يلفّها
//      كاتبُ ELF (e_machine=EM_AARCH64) في تنفيذيٍّ ساكن يخرج عبر svc بقيمةِ `رئيسية`.
//      النطاق (تكافؤُ x86 الكامل، ٩/٩): حسابٌ صحيح، تدفّقُ تحكّمٍ (if/else + حلقات)،
//      ذاكرةٌ ومتغيّرات، نداءُ دوالّ (BL/RET + AAPCS64، الداخلةُ تخرج svc والمنادَاةُ ورقةٌ
//      بلا حفظِ x30)، وطباعةٌ أصليّة (svc-write x8=64: سلاسلُ rodata عبر عنوانٍ ٦٤-بت مبنيٍّ
//      movz+movk×3، وأعدادٌ itoa عبر sdiv/msub). كلُّ ما عداه يفشل صراحةً لا يُترجَم خطأً —
//      نظيرُ انضباطِ جسر x86. لا clang/lld/as (متحقَّقٌ حيًّا على qemu-aarch64، بايتٌ ببايت llvm-mc).
// (EN) SIR→native AArch64 bridge — first "S-lang → ARM64" lowering with no LLVM.
//      Proves the pipeline generalizes across two ISA classes. Full x86 parity (9/9):
//      arithmetic, control flow (if/else + loops), memory/vars, function calls (BL/RET +
//      AAPCS64; entry exits via svc, callees are leaf without x30 save), and native
//      printing (svc-write x8=64: rodata strings via a 64-bit address built movz+movk×3,
//      integers via itoa with sdiv/msub). Everything else fails loudly. Byte-verified vs
//      llvm-mc; live-proven on qemu-aarch64. No clang/lld/as.
// ============================================================================
#ifndef SAD_NATIVE_ARM64_SIR_LOWERING_H
#define SAD_NATIVE_ARM64_SIR_LOWERING_H

#include "backend/native/arm64_fixed32_encoder.h"
#include "backend/native/generated/arm64_encoding_generated.h"
#include "backend/native/elf64_writer.h"
#include "backend/native/sir_lowering_common.h"

#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

#include "error_codes.h"
#include "error_messages_generated.h"

#include <cstdint>
#include <map>
#include <memory>
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
        inline constexpr long long kSysExitArm64 = 93;  // (AR) exit
        inline constexpr long long kSysWriteArm64 = 64; // (AR) write
        inline constexpr long long kFdStdoutArm64 = 1;  // (AR) واصفُ الخرج القياسيّ (stdout)
        inline constexpr long long kItoaRadixArm64 = 10; // (AR) أساسُ التحويل العشريّ
        inline constexpr long long kAsciiZeroArm64 = 0x30; // (AR) رمزُ الصفر ASCII ('0') — أساسُ itoa
        namespace a64reg
        {
            inline constexpr int kX0 = 0;    // (AR) قيمةُ الإرجاع/الوسيط الأوّل (رمزُ الخروج)
            inline constexpr int kX1 = 1;    // (AR) الوسيط الثاني لـsvc (مؤشّر write)
            inline constexpr int kX2 = 2;    // (AR) الوسيط الثالث لـsvc (طولُ write)
            inline constexpr int kX8 = 8;    // (AR) رقمُ النداء (svc)
            inline constexpr int kScratch0 = 16; // (AR) x16 (IP0) مُبدَّدٌ لتجهيز المعامل الأوّل
            inline constexpr int kScratch1 = 17; // (AR) x17 (IP1) مُبدَّدٌ لتجهيز المعامل الثاني
            inline constexpr long long kImm16Max = 0xFFFF; // (AR) أقصى فوريّ لـMOVZ (بلا MOVK)
        } // namespace a64reg

        // (AR) مخفّضُ SIR→AArch64 (نطاقُ تكافؤ x86 الكامل). صنفٌ مستقلٌّ عن X86SirLowering
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
                entryName_ = entry->getName();

                // (AR) ترتيبُ الإصدار: الدالّةُ الداخلة أوّلًا (نقطةُ دخول ELF = code_[0])،
                //      ثمّ البقيّة. النداءاتُ للدوالّ اللاحقة مراجعُ أماميّةٌ تُرقَّع لاحقًا.
                std::vector<const sir::SIRFunction *> ordered{entry};
                for (const auto &f : module.getFunctions())
                    if (f.get() != entry)
                        ordered.push_back(f.get());

                // (AR) المرور ١: أصدِر كلَّ دالّةٍ (إطارُها الخاصّ + كتلُها)، سجّل إزاحةَ
                //      لصيقتها (funcOffset_ للنداءات، labelOffset_ للفروع، كلاهما مؤهَّلٌ بالدالّة).
                for (const sir::SIRFunction *fn : ordered)
                {
                    funcOffset_[fn->getName()] = code_.size();
                    if (!lowerFunction(*fn))
                        return finishError(r, errorCode_, detail_);
                }

                // (AR) المرور ٢: رقّع كلَّ فرعٍ (imm19/imm26 داخل الدالّة) أو نداءٍ (imm26 بين الدوالّ).
                if (!applyFixups())
                    return finishError(r, errorCode_, detail_);

                // (AR) ألحِق كتلةَ البيانات (سلاسلُ الطباعة) بعد كلّ الشيفرة في مقطع R+X، ثمّ رقّع
                //      عنوانَ كلّ سلسلةٍ (٦٤-بت) في تسلسلِ movz+movk×3. العنوانُ المطلقُ = vbase +
                //      إزاحةُ الشيفرة + قاعدةُ rodata داخل code_ + إزاحةُ السلسلة (ثباتُ vbase في ET_EXEC).
                if (!strFixups_.empty() || !rodata_.empty())
                {
                    const size_t rodataBase = code_.size();
                    code_.insert(code_.end(), rodata_.begin(), rodata_.end());
                    for (const StrFixup &sf : strFixups_)
                    {
                        const uint64_t vaddr = elf::kDefaultVBase + elf::kCodeOffset +
                                               static_cast<uint64_t>(rodataBase + sf.rodataOff);
                        // (AR) رقّع حقلَ imm16 (بتّات ٢٠-٥) في كلٍّ من movz(القطعة٠)/movk(١٦/٣٢/٤٨).
                        for (int chunk = 0; chunk < 4; ++chunk)
                        {
                            const uint32_t imm16 = static_cast<uint32_t>((vaddr >> (16 * chunk)) & 0xFFFF);
                            patchImm16At(sf.movStart + static_cast<size_t>(chunk) * 4, imm16);
                        }
                    }
                }

                r.ok = true;
                r.code = std::move(code_);
                return r;
            }

        private:
            using EC = ::Sad::Errors::ErrorCode;

            std::map<std::string, int> regOf_;                       // (AR) سجلّ افتراضيّ ⇒ فيزيائيّ
            const std::vector<int> pool_{9, 10, 11, 12, 13, 14, 15}; // (AR) x9..x15 (caller-saved)
            size_t next_ = 0;
            std::vector<uint8_t> code_;
            EC errorCode_ = EC::INT_NATIVE_NO_ENTRY;
            std::string detail_;

            // (AR) الدوالّ: الداخلةُ (RET ⇒ exit svc)، والجاري تخفيضُها (لتأهيل اللصائق وحالة RET).
            std::string entryName_;
            std::string currentFn_;
            bool curIsEntry_ = false;

            // (AR) سجلّاتُ وسائط AAPCS64 بالترتيب (الوسائطُ الثمانية الأولى الصحيحة).
            const int abiArg_[8]{0, 1, 2, 3, 4, 5, 6, 7};

            // (AR) تدفّقُ التحكّم/النداء: خريطةُ لصيقةِ الكتلة (مؤهَّلةٌ) ⇒ إزاحتُها، وإزاحةُ كلّ
            //      دالّة (للنداءات)، وطابورُ ترقيعِ الفروع/النداءات. كلٌّ يحمل موضعَ كلمتِه وحقلَ
            //      إزاحته (imm19 لـb.cond، imm26 لـb/bl) والهدفَ، وهل هو نداءٌ (funcOffset_) أم فرعٌ.
            std::map<std::string, size_t> labelOffset_;
            std::map<std::string, size_t> funcOffset_;
            struct Arm64Fixup
            {
                size_t wordPos;     // (AR) موضعُ كلمةِ الفرع/النداء (٤ بايت) في code_
                int immHi;          // (AR) أعلى بتّةٍ لحقل الإزاحة (٢٣ لـimm19، ٢٥ لـimm26)
                int immLo;          // (AR) أدنى بتّة (٥ لـimm19، ٠ لـimm26)
                std::string target; // (AR) لصيقةُ الكتلة الهدف (فرع) أو اسمُ الدالّة (نداء)
                bool isCall;        // (AR) نداءٌ (يُحلُّ من funcOffset_) أم فرعٌ (من labelOffset_)
            };
            std::vector<Arm64Fixup> fixups_;

            // (AR) الذاكرة: اسمُ خانةٍ (معامل أو ALLOC) ⇒ فهرسُها (٠،١،…)؛ العنوانُ [sp, #فهرس×٨].
            //      STR/LDR تستعملان imm12 مقيسًا بـ٨ فالفهرسُ = imm12 مباشرةً. حجمُ الإطارِ مُحاذًى ١٦.
            std::map<std::string, int> memSlot_;
            long long frameSize_ = 0; // (AR) بايتات (مُحاذاةُ ١٦)

            // (AR) الطباعة: فهرسُ قمّةِ مخزنِ itoa (العنوانُ الأعلى، حصريّ) في الإطار؛ الأرقامُ
            //      تُبنى تنازليًّا منه. الانسكابُ عبر النداء/الطبع: فهرسُ أوّلِ خانةٍ لمنطقةِ انسكابِ
            //      سجلّات الحوض (خانةٌ لكلّ سجلّ). النداء/الطبعُ يدهسان الحوضَ (caller-saved).
            int printBufTopSlot_ = -1;
            int spillBaseSlot_ = -1;

            // (AR) كتلةُ البيانات (rodata): سلاسلُ الطباعة الحرفيّةُ تُلحَق بعد كلّ الشيفرة في نفس
            //      مقطع R+X؛ عنوانُها المطلق (vbase+إزاحة) يُرقَّع في تسلسلِ movz+movk×3.
            std::vector<uint8_t> rodata_;
            std::map<std::string, size_t> internedStr_; // (AR) سلسلة ⇒ إزاحتُها في rodata (تفريدٌ)
            struct StrFixup
            {
                size_t movStart;  // (AR) موضعُ أوّلِ كلمةٍ (movz) في تسلسلِ الأربع كلمات
                size_t rodataOff; // (AR) إزاحةُ السلسلة داخل rodata_
            };
            std::vector<StrFixup> strFixups_;

            // (AR) سجلٌّ افتراضيٌّ عُرِّف بـ«MOVE %r = سلسلةٌ حرفيّة» ⇒ محتواها (لا شيفرةَ للـMOVE؛
            //      السلاسلُ بياناتٌ لا قيمُ سجلّات)؛ تُرآيه الطباعةُ لتُصدِر السلسلةَ حرفيًّا.
            std::map<std::string, std::string> strReg_;

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

            // (AR) يؤهّل لصيقةَ كتلةٍ باسم الدالّة الحاليّة (يمنع تصادمَ «entry» بين الدوالّ).
            std::string qualify(const std::string &blockName) const
            {
                return currentFn_ + "\x1f" + blockName; // (AR) فاصلُ وحدةٍ لا يظهر في الأسماء
            }

            int poolIndexOf(int physReg) const
            {
                for (size_t i = 0; i < pool_.size(); ++i)
                    if (pool_[i] == physReg)
                        return static_cast<int>(i);
                return -1;
            }

            // (AR) تحليلُ الحياة/الانسكاب (usedAfterInBlock/isPoolArgOfCall/isPoolOperandOf)
            //      نُقِل إلى common — محايدٌ للمعماريّة، مشتركٌ مع مخفّض x86 لمنع الانجراف.
            //      يُمرَّر مُسنِدُ «هل متغيّرُ ذاكرة» (memSlot_) لأنّ تمييزَه خاصٌّ بكلّ مخفّض.
            bool isMemName(const std::string &name) const { return memSlot_.find(name) != memSlot_.end(); }

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
                if (imm < 0 || imm > a64reg::kImm16Max) // (AR) MOVZ يحمّل ١٦ بتًّا فقط؛ الأكبر/السالب غيرُ مدعوم
                    return fail(EC::INT_NATIVE_IMM_RANGE, "u16:" + std::to_string(imm));
                return emit(a64::mnem::kMovz, "x, imm16", {a64::Operand::R(reg), a64::Operand::I(imm)});
            }
            bool movk(int reg, long long imm16, int hw) // (AR) movk reg, #imm16, lsl #(hw*16)
            {
                return emit(a64::mnem::kMovk, "x, imm16, lsl",
                            {a64::Operand::R(reg), a64::Operand::I(imm16), a64::Operand::I(hw)});
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
            bool subSp(long long imm12) // (AR) sub sp, sp, #imm12 — تخصيصُ إطار
            {
                return emit(a64::mnem::kSub, "sp, imm12", {a64::Operand::I(imm12)});
            }
            bool addImm(int d, int n, long long imm12) // (AR) add Xd, Xn, #imm12 (Xn قد يكون sp=31)
            {
                return emit(a64::mnem::kAdd, "x, x, imm12",
                            {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::I(imm12)});
            }
            bool subImm(int d, int n, long long imm12) // (AR) sub Xd, Xn, #imm12 (فوريّ عامّ)
            {
                return emit(a64::mnem::kSub, "x, x, imm12",
                            {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::I(imm12)});
            }
            bool strb(int wt, int xn) // (AR) strb Wt, [Xn]
            {
                return emit(a64::mnem::kStrb, "w, x", {a64::Operand::R(wt), a64::Operand::R(xn)});
            }
            // (AR) الفهرسُ (=imm12) مضمونٌ في [0, 4095]: حارسُ emitPrologue يرفض الإطارَ > 4095.
            bool strSlot(int reg, int slot) // (AR) str Xreg, [sp, #slot*8]
            {
                return emit(a64::mnem::kStr, "x, sp, imm12", {a64::Operand::R(reg), a64::Operand::I(slot)});
            }
            bool ldrSlot(int reg, int slot) // (AR) ldr Xreg, [sp, #slot*8]
            {
                return emit(a64::mnem::kLdr, "x, sp, imm12", {a64::Operand::R(reg), a64::Operand::I(slot)});
            }
            // (AR) انسكابُ/استعادةُ سجلٍّ فيزيائيٍّ من الحوض إلى/من خانته في منطقة الانسكاب.
            bool spillReg(int physReg)
            {
                return strSlot(physReg, spillBaseSlot_ + poolIndexOf(physReg));
            }
            bool reloadReg(int physReg)
            {
                return ldrSlot(physReg, spillBaseSlot_ + poolIndexOf(physReg));
            }

            // (AR) هل المعاملُ سجلٌّ يشير إلى خانةِ متغيّرٍ مخصَّص (ALLOC/معامل)؟ يُعيد فهرسَ خانته.
            bool isMemVar(const sir::SIROperand &op, int &slot) const
            {
                if (op.type != sir::SIROperandType::REGISTER)
                    return false;
                auto it = memSlot_.find(op.name);
                if (it == memSlot_.end())
                    return false;
                slot = it->second;
                return true;
            }

            // (AR) يرقّع حقلَ imm16 (بتّات ٢٠-٥) في كلمةٍ ٣٢-بت بموضعِ wordPos (نائبُه صفر ⇒ OR).
            void patchImm16At(size_t wordPos, uint32_t imm16)
            {
                uint32_t w = 0;
                for (int i = 0; i < 4; ++i)
                    w |= static_cast<uint32_t>(code_[wordPos + i]) << (8 * i);
                w |= (imm16 & 0xFFFFu) << 5;
                for (int i = 0; i < 4; ++i)
                    code_[wordPos + i] = static_cast<uint8_t>((w >> (8 * i)) & 0xFF);
            }

            // (AR) يبني عنوانَ سلسلةٍ ٦٤-بت في سجلّ: movz(القطعة٠) + movk×3 (١٦/٣٢/٤٨) بقيمٍ نائبةٍ
            //      صفريّة، ويسجّل ترقيعًا يملأ القطعَ الأربعَ حين يُعرَف موضعُ rodata.
            bool emitStringAddr(int reg, size_t rodataOff)
            {
                const size_t start = code_.size();
                if (!emit(a64::mnem::kMovz, "x, imm16", {a64::Operand::R(reg), a64::Operand::I(0)}) ||
                    !movk(reg, 0, 1) || !movk(reg, 0, 2) || !movk(reg, 0, 3))
                    return false;
                strFixups_.push_back({start, rodataOff});
                return true;
            }

            // (AR) يُفرِّد سلسلةً في rodata (يُعيد إزاحتَها)؛ التكرارُ يُشارك الإزاحةَ نفسَها.
            size_t internString(const std::string &s)
            {
                auto it = internedStr_.find(s);
                if (it != internedStr_.end())
                    return it->second;
                size_t off = rodata_.size();
                rodata_.insert(rodata_.end(), s.begin(), s.end());
                internedStr_[s] = off;
                return off;
            }

            // (AR) يطبع سلسلةً حرفيّة: يُفرِّدها، يبني عنوانَها في x1، ثمّ write(stdout, x1, الطول).
            bool emitPrintString(const std::string &s)
            {
                if (s.size() > static_cast<size_t>(a64reg::kImm16Max)) // (AR) الطولُ يُحمَّل movz (≤٦٥٥٣٥)
                    return fail(EC::INT_NATIVE_IMM_RANGE, "strlen:" + std::to_string(s.size()));
                const size_t off = internString(s);
                return emitStringAddr(a64reg::kX1, off) &&
                       movz(a64reg::kX2, static_cast<long long>(s.size())) &&
                       movz(a64reg::kX0, kFdStdoutArm64) &&
                       movz(a64reg::kX8, kSysWriteArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }

            // (AR) يطبع عددًا صحيحًا غيرَ سالبٍ (في x9): itoa عبر sdiv/msub يبني الأرقامَ العشريّةَ
            //      تنازليًّا في مخزنِ الإطار، ثمّ write. القيمةُ السالبةُ غيرُ مدعومةٍ بعد (دَينٌ موثَّق).
            //      يستعمل سجلّاتِ الحوض x9..x14 مُبدَّداتٍ (مُنسَكةٌ حولَ الطبع فتُستعاد الحيّةُ).
            bool emitPrintInt()
            {
                //   x9=القيمة (مُحمَّلةٌ مسبقًا)، x10=١٠، x13=المؤشّر (قمّةُ المخزن، حصريّ)، x14=القمّة (للطول).
                if (!addImm(13, 31, static_cast<long long>(printBufTopSlot_) * 8) || // ptr = sp + top*8
                    !movReg(14, 13) ||                                             // top = ptr (نسخةٌ للطول)
                    !movz(10, kItoaRadixArm64))
                    return false;
                const size_t loopStart = code_.size();
                //   x11=الحاصل=x9÷x10؛ x12=الباقي=x9−x11×x10؛ x12+='0'؛ ptr−−؛ strb w12,[ptr]؛ x9=الحاصل؛ cbnz x9
                if (!rrr(a64::mnem::kSdiv, 11, 9, 10) ||
                    !msub(12, 11, 10, 9) ||
                    !addImm(12, 12, kAsciiZeroArm64) ||
                    !subImm(13, 13, 1) ||
                    !strb(12, 13) ||
                    !movReg(9, 11) ||
                    !emitCbnzBack(9, loopStart))
                    return false;
                //   x1=المؤشّر؛ x2=الطول=(القمّة−المؤشّر)؛ write(stdout).
                return movReg(a64reg::kX1, 13) &&
                       rrr(a64::mnem::kSub, a64reg::kX2, 14, 13) &&
                       movz(a64reg::kX0, kFdStdoutArm64) &&
                       movz(a64reg::kX8, kSysWriteArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }

            // (AR) cbnz إلى إزاحةٍ خلفيّةٍ معلومة (لولبٌ محلّيّ داخل itoa، لا لصيقةَ كتلة): نحسب
            //      imm19 مباشرةً (الهدفُ معلومٌ زمنَ الإصدار) فلا حاجةَ لطابور الترقيع.
            bool emitCbnzBack(int reg, size_t target)
            {
                const long long dispBytes = static_cast<long long>(target) -
                                            static_cast<long long>(code_.size());
                const long long imm = dispBytes / 4; // (AR) عددُ تعليماتٍ موقَّع (سالب)
                return emit(a64::mnem::kCbnz, "x, rel19",
                            {a64::Operand::R(reg), a64::Operand::I(imm)});
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

            // (AR) يُجهّز معاملًا في سجلٍّ وجهة: ثابتٌ ⇒ movz؛ متغيّرُ ذاكرةٍ ⇒ ldr من خانته (قراءةُ
            //      اسمِ ALLOC/معاملٍ كقيمة = تحميلٌ ضمنيّ)؛ سجلٌّ افتراضيٌّ ⇒ نسخٌ من موضعه.
            bool materialize(int dst, const sir::SIROperand &op)
            {
                long long c;
                if (common::isConstInt(op, c))
                    return movz(dst, c);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    int slot;
                    if (isMemVar(op, slot))
                        return ldrSlot(dst, slot);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + op.name);
                    return movReg(dst, it->second);
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "operand-kind=" + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) يحمّل وسيطَ نداءٍ/معاملَ طباعةٍ في سجلٍّ وجهة. يُستدعى بعد انسكابِ المؤقّتات،
            //      فالمؤقّتُ يُحمَّل من **خانة انسكابه** لا من سجلّه ⇒ صفر تصادمِ نقلٍ متوازٍ.
            bool loadArgInto(int dst, const sir::SIROperand &op)
            {
                long long c;
                if (common::isConstInt(op, c))
                    return movz(dst, c);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    int slot;
                    if (isMemVar(op, slot))
                        return ldrSlot(dst, slot);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + op.name);
                    return ldrSlot(dst, spillBaseSlot_ + poolIndexOf(it->second));
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "arg-kind=" + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) المسحُ المسبق: يخصّص فهرسَ خانةٍ للمعاملات (بترتيب ABI) ثمّ لكلّ ALLOC، ثمّ (إن
            //      لزم) لمخزنِ itoa ولمنطقة الانسكاب؛ ويحسب حجمَ الإطارِ المُحاذى ١٦. المعاملُ يُعامَل
            //      كمتغيّرِ ذاكرةٍ (قراءتُه = تحميلٌ من خانته)، وتُسكَنُ خانتُه من سجلّ الوسيط في المقدّمة.
            bool assignFrameSlots(const sir::SIRFunction &fn)
            {
                const auto &params = fn.getParameters();
                if (params.size() > 8)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "params>8:" + std::to_string(params.size()));
                int slot = 0;
                for (const auto &p : params)
                    // (AR) المعاملُ يُشار إليه بـ«%»+الاسم (sir_builder_functions) بينما اسمُ
                    //      SIRParameter بلا «%» ⇒ نُفهرِس بالمرجع كي يطابقَه isMemVar.
                    memSlot_["%" + p.name] = slot++;

                bool hasCall = false;
                bool hasPrint = false;
                bool hasNumberPrint = false;
                for (const auto &blockPtr : fn.getBasicBlocks())
                    for (const auto &inst : blockPtr->instructions)
                    {
                        if (inst.opcode == sir::SIROpcode::ALLOC && inst.result)
                            memSlot_[inst.result->name] = slot++;
                        else if (inst.opcode == sir::SIROpcode::CALL)
                            hasCall = true;
                        else if (inst.opcode == sir::SIROpcode::BUILTIN_PRINT)
                        {
                            hasPrint = true;
                            for (const auto &op : inst.operands)
                                if (op.dataType != types::SadTypeKind::String)
                                    hasNumberPrint = true; // (AR) معاملٌ غيرُ نصّيٍّ ⇒ يُطبَع عددًا (itoa)
                        }
                    }
                // (AR) 🔑 النطاق (نداءٌ ورقيّ بلا حفظ x30): الدالّةُ الداخلة تنادي وتخرج svc (لا
                //      تعود، فلا يهمّها دهسُ x30). الدالّةُ غيرُ الداخلة إن نادت تدهسُ x30 قبل RET
                //      (لا نحفظه بعد) ⇒ رفضٌ صريح. فالنداءُ مسموحٌ من الداخلة فقط (المنادَاةُ ورقة).
                if (hasCall && !curIsEntry_)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "nested-call-in-nonentry:" + currentFn_);

                // (AR) طباعةُ عددٍ: احجز مخزنَ itoa (٣ خاناتٍ = ٢٤ بايتًا تكفي ٢٠ رقمًا لـi64 + هامش).
                //      القمّةُ (العنوانُ الأعلى، حصريّ) = فهرسُ ما بعد المخزن.
                if (hasNumberPrint)
                {
                    slot += 3;
                    printBufTopSlot_ = slot; // (AR) العنوانُ الأعلى الحصريّ = sp + slot*8
                }
                // (AR) إن نادت الدالّةُ أو طبعت، احجز منطقةَ انسكابٍ: خانةٌ لكلّ سجلّ حوض.
                if (hasCall || hasPrint)
                {
                    spillBaseSlot_ = slot;
                    slot += static_cast<int>(pool_.size());
                }
                const long long bytes = static_cast<long long>(slot) * 8;
                frameSize_ = (bytes + 15) / 16 * 16; // (AR) مُحاذاةُ ١٦ (عقدُ AAPCS64 لـSP)
                return true;
            }

            // (AR) المقدّمة: sub sp, sp, #frameSize (إن وُجدت خانات)، ثمّ خزّن سجلّاتِ الوسائط
            //      الواردة (x0..x7) في خانات المعاملات. imm12 يسع حتّى ٤٠٩٥.
            bool emitPrologue(const sir::SIRFunction &fn)
            {
                if (frameSize_ > 0)
                {
                    if (frameSize_ > 4095)
                        return fail(EC::INT_NATIVE_IMM_RANGE, "frame:" + std::to_string(frameSize_));
                    if (!subSp(frameSize_))
                        return false;
                }
                const auto &params = fn.getParameters();
                for (size_t i = 0; i < params.size(); ++i)
                    if (!strSlot(abiArg_[i], memSlot_["%" + params[i].name]))
                        return false;
                return true;
            }

            // (AR) خاتمةُ الدالّة غير الداخلة: استعِد المكدّسَ (add sp) ثمّ ارجع (RET إلى x30).
            bool emitEpilogue()
            {
                if (frameSize_ > 0 && !addImm(31, 31, frameSize_)) // add sp, sp, #frameSize
                    return false;
                return emit(a64::mnem::kRet, "", {});
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
                    return rrr(a64::mnem::kSdiv, dst, a64reg::kScratch0, a64reg::kScratch1) &&
                           msub(dst, dst, a64reg::kScratch1, a64reg::kScratch0);
                // (AR) البتّيّات (نظير x86 reg-reg). SHL/SHR عبر lslv/lsrv بمقدارِ سجلّ (x17) ⇒
                //      يدعمان الثابتَ **والمتغيّر** (بخلاف x86 المحدودِ بالثابت). SHR منطقيّة.
                case OP::AND: return rrr(a64::mnem::kAnd, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::OR:  return rrr(a64::mnem::kOrr, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::XOR: return rrr(a64::mnem::kEor, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SHL: return rrr(a64::mnem::kLslv, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SHR: return rrr(a64::mnem::kLsrv, dst, a64reg::kScratch0, a64reg::kScratch1);
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // (AR) المقارنةُ كقيمة (غيرُ مدموجةٍ في فرع): cmp ثمّ cset dst بالشرط. حقلُ الشرط
            //      المُرمَّز في cset هو **المقلوب** (cond XOR 1) لأنّها CSINC بشرطٍ معكوس.
            static bool csetInvertedField(sir::SIROpcode op, long long &field)
            {
                using OP = sir::SIROpcode;
                switch (op) // (AR) القيمةُ = رمزُ الشرط XOR 1 (EQ0/NE1/LT11/LE13/GT12/GE10)
                {
                case OP::EQ: field = 1;  return true;  // invert(EQ)=NE
                case OP::NE: field = 0;  return true;  // invert(NE)=EQ
                case OP::LT: field = 10; return true;  // invert(LT)=GE
                case OP::LE: field = 12; return true;  // invert(LE)=GT
                case OP::GT: field = 13; return true;  // invert(GT)=LE
                case OP::GE: field = 11; return true;  // invert(GE)=LT
                default: return false;
                }
            }
            bool lowerComparison(const sir::SIRInstruction &inst)
            {
                if (!inst.result || inst.operands.size() != 2)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                // (AR) رفضُ المقارنةِ اللا-موقَّعة صراحةً (cset يحتاج شرطًا لا-موقَّعًا؛ توصية أميليا).
                for (const auto &op : inst.operands)
                    if (isUnsignedType(op.dataType))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "cmp-value-unsigned=" + std::to_string(static_cast<int>(op.dataType)));
                long long field;
                if (!csetInvertedField(inst.opcode, field))
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                return materialize(a64reg::kScratch0, inst.operands[0]) &&
                       materialize(a64reg::kScratch1, inst.operands[1]) &&
                       cmp(a64reg::kScratch0, a64reg::kScratch1) &&
                       emit(a64::mnem::kCset, "x, cond",
                            {a64::Operand::R(dst), a64::Operand::I(field)});
            }

            // (AR) %dst = ~a (نفيٌ بتّيّ أحاديّ): جهّز a في x16 ثمّ mvn dst,x16.
            bool lowerNot(const sir::SIRInstruction &inst)
            {
                if (!inst.result || inst.operands.size() != 1)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                return materialize(a64reg::kScratch0, inst.operands[0]) &&
                       emit(a64::mnem::kMvn, "x, x",
                            {a64::Operand::R(dst), a64::Operand::R(a64reg::kScratch0)});
            }

            bool lowerInstruction(const sir::SIRInstruction &inst,
                                  const sir::SIRBasicBlock &block, size_t instIdx)
            {
                using OP = sir::SIROpcode;
                switch (inst.opcode)
                {
                case OP::MOVE:
                {
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) MOVE %r = سلسلةٌ حرفيّة: لا سجلَّ فيزيائيًّا (بياناتٌ لا قيمُ سجلّات)؛
                    //      نسجّل المحتوى ليُطبَعَ حرفيًّا، ولا نُصدِر شيفرة.
                    if (inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                        inst.operands[0].dataType == types::SadTypeKind::String)
                    {
                        strReg_[inst.result->name] = inst.operands[0].name;
                        return true;
                    }
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    long long c;
                    if (common::isConstInt(inst.operands[0], c))
                        return movz(dst, c);
                    if (inst.operands[0].type == sir::SIROperandType::REGISTER)
                    {
                        int slot;
                        if (isMemVar(inst.operands[0], slot))
                            return ldrSlot(dst, slot);
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
                case OP::AND:
                case OP::OR:
                case OP::XOR:
                case OP::SHL:
                case OP::SHR:
                    return lowerBinary(inst);
                case OP::NOT:
                    return lowerNot(inst);
                case OP::EQ:
                case OP::NE:
                case OP::LT:
                case OP::LE:
                case OP::GT:
                case OP::GE:
                    return lowerComparison(inst);
                case OP::ALLOC:
                {
                    // (AR) الخانةُ خُصِّصت في المسح المسبق؛ لا شيفرةَ (العنوان ضمنيٌّ [sp، #فهرس×٨]).
                    if (!inst.result || memSlot_.find(inst.result->name) == memSlot_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "alloc-unslotted:" + detailOpcode(inst));
                    return true;
                }
                case OP::LOAD:
                {
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int slot;
                    if (!isMemVar(inst.operands[0], slot))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "load-nonslot:" + detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrSlot(dst, slot);
                }
                case OP::STORE:
                {
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int slot;
                    if (!isMemVar(inst.operands[1], slot))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "store-nonslot:" + detailOpcode(inst));
                    return materialize(a64reg::kScratch0, inst.operands[0]) &&
                           strSlot(a64reg::kScratch0, slot);
                }
                case OP::CALL:
                {
                    // (AR) call @دالّة, وسائط… ⇒ ضع الوسائطَ في x0..x7 ثمّ bl (imm26 يُرقَّع لإزاحة
                    //      الدالّة)؛ النتيجةُ في x0 ⇒ سجلُّ النتيجة. النداءُ من الداخلة فقط (فُحِص).
                    if (inst.operands.empty() ||
                        inst.operands[0].type != sir::SIROperandType::FUNCTION)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const size_t argc = inst.operands.size() - 1;
                    if (argc > 8)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "args>8:" + std::to_string(argc));
                    // (AR) bl يدهس x9..x15/x0..x7 (caller-saved). انسكِبْ المؤقّتاتِ الحيّةَ بعد
                    //      النداء (أو وسائطَ سجليّةً له) إلى خانات الانسكاب، حمّل الوسائطَ منها (لا
                    //      من سجلّاتها ⇒ صفر تصادمِ نقلٍ متوازٍ)، bl، أعِد الحيّةَ، النتيجةُ من x0.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) || common::isPoolArgOfCall(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    for (size_t i = 0; i < argc; ++i)
                        if (!loadArgInto(abiArg_[i], inst.operands[i + 1]))
                            return false;
                    if (!emitBranchTo(a64::mnem::kBl, "rel26", inst.operands[0].name, /*isCall=*/true))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    if (inst.result)
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movReg(dst, a64reg::kX0); // (AR) قيمةُ الإرجاع من x0
                    }
                    return true;
                }
                case OP::BUILTIN_PRINT:
                {
                    // (AR) اطبع(معاملات…): سلسلةٌ حرفيّة ⇒ write مباشر؛ عددٌ ⇒ itoa. الطبعُ يُبدِّد
                    //      سجلّاتِ الحوض، فنَنسِك الحيّةَ (+ المعاملاتِ المؤقّتة) ونعيدها بعده.
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) || common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    for (const auto &op : inst.operands)
                    {
                        if (op.type == sir::SIROperandType::CONSTANT &&
                            op.dataType == types::SadTypeKind::String)
                        {
                            if (!emitPrintString(op.name))
                                return false;
                        }
                        else if (op.type == sir::SIROperandType::REGISTER &&
                                 strReg_.find(op.name) != strReg_.end())
                        {
                            if (!emitPrintString(strReg_[op.name]))
                                return false;
                        }
                        else if (op.dataType == types::SadTypeKind::String)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, "print-str-computed:%" + op.name);
                        else
                        {
                            // (AR) عددٌ: حمّله في x9 (ثابت/ذاكرة/خانة انسكاب) ثمّ itoa+write.
                            if (!loadArgInto(9, op) || !emitPrintInt())
                                return false;
                        }
                    }
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    return true;
                }
                case OP::RET:
                {
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) الداخلةُ تُنهي البرنامج: x0=القيمة، x8=exit، svc. غيرُها ترجع: x0=القيمة، خاتمة+ret.
                    if (curIsEntry_)
                        return materialize(a64reg::kX0, inst.operands[0]) &&
                               movz(a64reg::kX8, kSysExitArm64) &&
                               emit(a64::mnem::kSvc, "", {});
                    return materialize(a64reg::kX0, inst.operands[0]) && emitEpilogue();
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

            // (AR) يُصدر فرعًا/نداءً بإزاحةٍ صفريّةٍ نائبة، ويسجّل ترقيعًا بحقلِ إزاحته (imm19/imm26).
            //      عرضُ الحقل وموضعُه من مواصفة الترميز (from_op==0) لا من ثابتٍ مُرمَّز.
            bool emitBranchTo(const std::string &mnemonic, const std::string &form,
                              const std::string &target, bool isCall)
            {
                const a64::EncSpec *spec = a64::lookupEncSpec(mnemonic, form);
                if (!spec)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + " " + form);
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
                fixups_.push_back({wordPos, immHi, immLo, target, isCall});
                return true;
            }
            bool emitBranch(const std::string &mnemonic, const std::string &form,
                            const std::string &targetLabel)
            {
                return emitBranchTo(mnemonic, form, qualify(targetLabel), /*isCall=*/false);
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

                const sir::SIRInstruction *cmpInst = common::findFusedComparison(block, cond.name);
                if (!cmpInst || cmpInst->operands.size() != 2)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "cond-not-fused-cmp:%" + cond.name);
                // (AR) الفروعُ المُصدَرة موقَّعة؛ معاملٌ لا-موقَّعٌ يلزمه b.lo/ls/hi/hs (لا نظائرَ بعد)
                //      ⇒ رفضٌ صريح بدل ترميزٍ موقَّعٍ خاطئٍ صامت (توصية أميليا).
                for (const auto &cop : cmpInst->operands)
                    if (isUnsignedType(cop.dataType))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "cmp-unsigned-type=" + std::to_string(static_cast<int>(cop.dataType)));
                const std::string *bcc = bccForCmp(cmpInst->opcode);
                if (!bcc)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(*cmpInst));
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

                const sir::SIRInstruction *fused = nullptr;
                if (!is.empty() && is.back().opcode == sir::SIROpcode::BR_COND &&
                    !is.back().operands.empty() &&
                    is.back().operands[0].type == sir::SIROperandType::REGISTER)
                    fused = common::findFusedComparison(block, is.back().operands[0].name);

                for (size_t idx = 0; idx < is.size(); ++idx)
                {
                    const sir::SIRInstruction &inst = is[idx];
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
                    else if (!lowerInstruction(inst, block, idx))
                        return false;
                }
                return true;
            }

            // (AR) يخفّض دالّةً واحدة: إطارٌ خاصّ، مقدّمةٌ تُسكِن سجلّاتِ ABI في خانات المعاملات، ثمّ كتلُها.
            bool lowerFunction(const sir::SIRFunction &fn)
            {
                currentFn_ = fn.getName();
                curIsEntry_ = (currentFn_ == entryName_);
                memSlot_.clear();
                regOf_.clear();
                strReg_.clear();
                next_ = 0;
                frameSize_ = 0;
                printBufTopSlot_ = -1;
                spillBaseSlot_ = -1;

                const auto &blocks = fn.getBasicBlocks();
                if (blocks.empty())
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "no-blocks:" + currentFn_);
                if (!assignFrameSlots(fn) || !emitPrologue(fn))
                    return false;

                for (const auto &blockPtr : blocks)
                {
                    labelOffset_[qualify(blockPtr->name)] = code_.size();
                    if (!lowerBlock(*blockPtr))
                        return false;
                }
                return true;
            }

            // (AR) المرور ٢: يرقّع كلَّ فرعٍ/نداءٍ بالإزاحة النسبيّة ÷٤ (عددُ تعليمات) في حقلِ imm
            //      داخلَ كلمتِه ٣٢-بت (تعبئةٌ بتّيّة: قناعٌ ثمّ OR؛ الحقلُ نائبُه صفر أصلًا). الفرعُ
            //      يُحلُّ من labelOffset_ (لصيقةٌ مؤهَّلة)، والنداءُ من funcOffset_ (اسمُ دالّة).
            bool applyFixups()
            {
                for (const Arm64Fixup &fx : fixups_)
                {
                    const auto &table = fx.isCall ? funcOffset_ : labelOffset_;
                    auto it = table.find(fx.target);
                    if (it == table.end())
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
