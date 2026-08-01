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
#include <cstring>
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
        inline constexpr long long kSysMmapArm64 = 222; // (AR) mmap (تخصيصُ كومةٍ للمصفوفات)
        inline constexpr long long kFdStdoutArm64 = 1;  // (AR) واصفُ الخرج القياسيّ (stdout)
        inline constexpr long long kItoaRadixArm64 = 10; // (AR) أساسُ التحويل العشريّ
        inline constexpr long long kAsciiZeroArm64 = 0x30; // (AR) رمزُ الصفر ASCII ('0') — أساسُ itoa
        inline constexpr long long kAsciiMinusArm64 = 0x2D; // (AR) رمزُ السالب ASCII ('-') — بادئةُ العدد السالب
        namespace a64reg
        {
            inline constexpr int kX0 = 0;    // (AR) قيمةُ الإرجاع/الوسيط الأوّل (رمزُ الخروج)
            inline constexpr int kX1 = 1;    // (AR) الوسيط الثاني لـsvc (مؤشّر write)
            inline constexpr int kX2 = 2;    // (AR) الوسيط الثالث لـsvc (طولُ write)
            inline constexpr int kX8 = 8;    // (AR) رقمُ النداء (svc)
            inline constexpr int kScratch0 = 16; // (AR) x16 (IP0) مُبدَّدٌ لتجهيز المعامل الأوّل
            inline constexpr int kScratch1 = 17; // (AR) x17 (IP1) مُبدَّدٌ لتجهيز المعامل الثاني
            inline constexpr int kXzr = 31;      // (AR) السجلّ الصفريّ XZR في موضعِ Rm/Rn الحسابيّ (لا SP)
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
            // (AR) سياقُ التخفيض الحاليّ (يُضبَط في lowerBlock): يمكّن allocReg من استرجاعِ سجلِّ
            //      مؤقّتٍ ميّتٍ عند نفاد الحوض (لازمٌ للمصفوفات كثيرةِ المؤقّتات). نظيرُ x86.
            const sir::SIRBasicBlock *curBlock_ = nullptr;
            size_t curInstIdx_ = 0;
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
            // (AR) طباعةُ العشريّ: خانةُ خدشٍ لنمطِ بتّاتِ الـdouble (القيمةِ المطلقة) عبر مراحل المُنسِّق.
            int floatValSlot_ = -1;
            // (AR) التعليب: خانتان (١٦ بايت: tag، payload) لكلّ ARRAY_GET معلَّب. المؤشّرُ = sp + الفهرس×٨.
            int dynBaseSlot_ = -1; // (AR) فهرسُ أوّلِ خانةِ dyn
            int dynGetCount_ = 0;  // (AR) عددُ القراءات المعلَّبة
            int dynSlotNext_ = 0;  // (AR) الفهرسُ الجاري (يُصفَّر لكلّ دالّة)

            // (AR) الإلحاق (append): خمسُ خاناتِ إطارٍ (فهارس) تبقى حيّةً عبر mmap الذي يدهس كلَّ
            //      الحوض: مؤشّرُ البنية P، القيمةُ المُلحَقة، الطولُ L، المؤشّرُ الجديد، والسعةُ الجديدة.
            int appendPSlot_ = -1;
            int appendValSlot_ = -1;
            int appendLenSlot_ = -1;
            int appendNewSlot_ = -1;
            int appendCapSlot_ = -1;

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
                return diag::kOpcode + std::to_string(static_cast<int>(i.opcode));
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
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + diag::kFormSep + form);
                auto bytes = a64::encodeFixed32(*spec, ops);
                code_.insert(code_.end(), bytes.begin(), bytes.end());
                return true;
            }
            bool movz(int reg, long long imm)
            {
                if (imm < 0 || imm > a64reg::kImm16Max) // (AR) MOVZ يحمّل ١٦ بتًّا فقط؛ الأكبر/السالب غيرُ مدعوم
                    return fail(EC::INT_NATIVE_IMM_RANGE, diag::kU16 + std::to_string(imm));
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
            // (AR) str/ldr بقاعدةِ سجلٍّ عامّة: [Xn, #idx×8] (idx = الإزاحةُ المقيسة). لحقولِ
            //      SadArray وعناصرها. add Xd,Xn,Xm,LSL#3 لعنونةِ عنصرٍ بفهرسٍ متغيّر (data+idx×8).
            bool strBase(int rt, int rn, long long idx)
            {
                return emit(a64::mnem::kStr, "x, x, imm12",
                            {a64::Operand::R(rt), a64::Operand::R(rn), a64::Operand::I(idx)});
            }
            bool ldrBase(int rt, int rn, long long idx)
            {
                return emit(a64::mnem::kLdr, "x, x, imm12",
                            {a64::Operand::R(rt), a64::Operand::R(rn), a64::Operand::I(idx)});
            }
            bool addLsl3(int d, int n, int m) // (AR) add Xd, Xn, Xm, LSL #3
            {
                return emit(a64::mnem::kAdd, "x, x, x, lsl3",
                            {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)});
            }

            // ── FP عدديّ مزدوج: العشريّ بتّاتٌ i64 في حوض GPR؛ d0/d1 خدشٌ عابر (كـx16/x17) ──
            static constexpr int kD0 = 0; // (AR) سجلّ عشريّ خدشٌ ٠
            static constexpr int kD1 = 1; // (AR) سجلّ عشريّ خدشٌ ١
            bool fmovToFp(int d, int x) { return emit(a64::mnem::kFmovDX, "d, x", {a64::Operand::R(d), a64::Operand::R(x)}); }
            bool fmovFromFp(int x, int d) { return emit(a64::mnem::kFmovXD, "x, d", {a64::Operand::R(x), a64::Operand::R(d)}); }
            bool scvtf(int d, int x) { return emit(a64::mnem::kScvtf, "d, x", {a64::Operand::R(d), a64::Operand::R(x)}); }
            bool fcvtzs(int x, int d) { return emit(a64::mnem::kFcvtzs, "x, d", {a64::Operand::R(x), a64::Operand::R(d)}); }
            bool fadd(int d, int n, int m) { return emit(a64::mnem::kFadd, "d, d, d", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); }
            // (AR) fcmp Dn, Dm — يضبط NZCV لمقارنةِ عشريَّين (لا وجهةَ لها)؛ NaN ⇒ غيرُ مرتَّبٍ (C=1,V=1).
            bool fcmp(int n, int m) { return emit(a64::mnem::kFcmp, "d, d", {a64::Operand::R(n), a64::Operand::R(m)}); }
            bool fsub(int d, int n, int m) { return emit(a64::mnem::kFsub, "d, d, d", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); }
            bool fmul(int d, int n, int m) { return emit(a64::mnem::kFmul, "d, d, d", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); }
            bool fdiv(int d, int n, int m) { return emit(a64::mnem::kFdiv, "d, d, d", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); }
            // (AR) fcvtns x, d: عشريّ ⇒ صحيح بتقريبٍ لأقربِ زوجٍ (nearest-even) = تقريبُ IEEE
            //      المطابقُ للمفسّر (نظيرُ cvtsd2si x86). أساسيٌّ في ARMv8؛ نظيرُ fcvtzs لكن round-nearest.
            bool fcvtns(int x, int d) { return emit(a64::mnem::kFcvtns, "x, d", {a64::Operand::R(x), a64::Operand::R(d)}); }
            // (AR) يحمّل فوريًّا ٦٤-بت (نمطَ بتّات) في سجلّ GPR عبر movz + movk×٣ (كلُّ ١٦ بتًّا).
            bool movImm64Bits(int reg, unsigned long long bits)
            {
                return movz(reg, static_cast<long long>(bits & 0xFFFF)) &&
                       movk(reg, static_cast<long long>((bits >> 16) & 0xFFFF), 1) &&
                       movk(reg, static_cast<long long>((bits >> 32) & 0xFFFF), 2) &&
                       movk(reg, static_cast<long long>((bits >> 48) & 0xFFFF), 3);
            }
            // (AR) يحمّل ثابتَ عشريّ (نمطُ بتّاته i64، إعادةُ تفسيرٍ زمنَ التصريف) في سجلّ GPR.
            bool loadFloatConst(int reg, double v)
            {
                std::uint64_t bits;
                std::memcpy(&bits, &v, sizeof bits);
                return movImm64Bits(reg, bits);
            }
            // (AR) mmap(NULL, size, R|W, PRIVATE|ANON, -1, 0) عبر svc ⇒ المؤشّرُ في x0. الحجمُ ثابتٌ
            //      (≤٦٥٥٣٥ ⇒ movz؛ الأكبرُ مؤجَّل). fd=-1 عبر movz صفر ثمّ sub #1. x8=222.
            bool emitMmapArm64(long long sizeBytes)
            {
                return movz(a64reg::kX0, 0) &&
                       movz(a64reg::kX1, sizeBytes) &&
                       movz(a64reg::kX2, kProtReadWrite) &&
                       movz(3, kMapPrivAnon) &&
                       movz(4, 0) && subImm(4, 4, 1) && // (AR) x4 = -1 (fd)
                       movz(5, 0) &&
                       movz(a64reg::kX8, kSysMmapArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }
            // (AR) b.lo #(عددُ تعليماتٍ×٤): قفزةٌ لا-موقَّعةٌ أماميّةٌ قصيرة (تخطّي كتلةِ الهلع).
            //      imm19 = عددُ التعليمات (نسبيّ للفرع نفسِه). لا طابورَ ترقيعٍ (الهدفُ معلومٌ محلّيًّا).
            bool bloSkip(long long instrCount)
            {
                return emit(a64::mnem::kBlo, "rel19", {a64::Operand::I(instrCount)});
            }
            // (AR) فحصُ حدّ المصفوفة (AArch64): x17=idx، x16=len؛ cmp ثمّ b.lo يتخطّى كتلةَ الهلع إن
            //      idx < len لا-موقَّعًا (يشملُ الفهرسَ السالبَ). الهلع: movz x0,#134؛ movz x8,#93؛ svc.
            //      كتلةُ الهلع ٣ تعليمات ⇒ b.lo يتخطّى ٤ (الفرعُ + الثلاث). لا عودةَ بعد svc.
            bool emitBoundsCheckArm64()
            {
                constexpr long long kPanicInstrs = 3; // (AR) movz x0 + movz x8 + svc
                if (!cmp(a64reg::kScratch1, a64reg::kScratch0) || !bloSkip(kPanicInstrs + 1))
                    return false;
                return movz(a64reg::kX0, kArrayBoundsPanicCode) &&
                       movz(a64reg::kX8, kSysExitArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }
            // (AR) يضع عنوانَ عنصرِ المصفوفة في x16 مع فحصِ حدٍّ زمنَ التشغيل، ويُعيد عبر outIdx الإزاحةَ
            //      المقيسةَ لـldr/str. أوّلًا الفحص: x17=idx، x16=len=[arr+0]، cmp+b.lo. ثمّ يُعادُ
            //      تحميلُ مؤشّرِ البنية (arch حِمل/خزن ⇒ لا cmp-بذاكرةٍ كـx86): x16=data=[arr+16]؛
            //      الفهرسُ ثابتٌ ⇒ يبقى إزاحةً (outIdx=idx)، أو سجلٌّ ⇒ x16 += idx×8 (add lsl#3).
            //      x17 يحملُ idx طوالَ المسار (لا يُدهَس بين الفحص وحسابِ العنوان).
            // (AR) الفهرسُ السالب: المتغيّرُ (سجلّ) يلتفّ ضخمًا ⇒ يفشلُ الفحصَ ⇒ هلعُ تشغيلٍ (١٣٤).
            //      أمّا الثابتُ السالب فيرفضه movz ⇒ فشلُ ترجمةٍ صريح — كلاهما آمن.
            bool emitElemAddrArm64(const sir::SIROperand &arrOp, const sir::SIROperand &idxOp,
                                   long long &outIdx)
            {
                long long idx;
                const bool isConst = common::isConstInt(idxOp, idx);
                if (isConst && idx > 4095)
                    // (AR) الفهرسُ الثابتُ يمرّ كـimm12 لـldr/str (مقياسُه ٨) ⇒ حدُّه ٤٠٩٥؛ الأكبرُ
                    //      يفشل صراحةً لا انهيارًا. الفهرسُ المتغيّرُ بلا حدٍّ (يُقاسُ زمنَ التشغيل).
                    return fail(EC::INT_NATIVE_IMM_RANGE, diag::kArrayIndexImm12 + std::to_string(idx));
                // (AR) فحصُ الحدّ: x17 = idx (ثابتٌ ⇒ movz، متغيّرٌ ⇒ materialize)، x16 = len.
                if (!materialize(a64reg::kScratch1, idxOp) || !materialize(a64reg::kScratch0, arrOp) ||
                    !ldrBase(a64reg::kScratch0, a64reg::kScratch0, kArrOffLen / kArrSlotBytes) ||
                    !emitBoundsCheckArm64())
                    return false;
                // (AR) حسابُ العنوان: أعِد تحميلَ المؤشّرِ (x16 صار len)، ثمّ data.
                if (!materialize(a64reg::kScratch0, arrOp) ||
                    !ldrBase(a64reg::kScratch0, a64reg::kScratch0, kArrOffData / kArrSlotBytes))
                    return false;
                if (isConst)
                {
                    outIdx = idx;
                    return true;
                }
                if (!addLsl3(a64reg::kScratch0, a64reg::kScratch0, a64reg::kScratch1))
                    return false;
                outIdx = 0;
                return true;
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

            // (AR) يحمّل العنوانَ المطلق لواصفِ النصّ المعلَّب في سجلّ (movz+movk×3 نائبٌ يُرقَّع كأيّ
            //      سلسلةِ rodata). الواصفُ {len@0، bytes@8}. يُستعمَل حمولةً في ARRAY_SET(Any) النصّيّ.
            bool emitLoadStrDescAddr(int reg, const std::string &content)
            {
                const size_t off = internString(makeStrDescriptor(content));
                return emitStringAddr(reg, off);
            }
            // (AR) يستخرجُ محتوى نصٍّ حرفيّ من معاملٍ (نظير توزيعِ الطباعة): سجلٌّ مسجَّلٌ في strReg_،
            //      أو ثابتُ سلسلةٍ مباشر. النصُّ المحسوب (لا حرفيّ) غيرُ مدعومٍ ⇒ يُعيد false.
            bool resolveBoxedStringLiteral(const sir::SIROperand &op, std::string &out) const
            {
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    auto it = strReg_.find(op.name);
                    if (it == strReg_.end())
                        return false;
                    out = it->second;
                    return true;
                }
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::String)
                {
                    out = op.name;
                    return true;
                }
                return false;
            }

            // (AR) يطبع سلسلةً حرفيّة: يُفرِّدها، يبني عنوانَها في x1، ثمّ write(stdout, x1, الطول).
            bool emitPrintString(const std::string &s)
            {
                if (s.size() > static_cast<size_t>(a64reg::kImm16Max)) // (AR) الطولُ يُحمَّل movz (≤٦٥٥٣٥)
                    return fail(EC::INT_NATIVE_IMM_RANGE, diag::kStrlen + std::to_string(s.size()));
                const size_t off = internString(s);
                return emitStringAddr(a64reg::kX1, off) &&
                       movz(a64reg::kX2, static_cast<long long>(s.size())) &&
                       movz(a64reg::kX0, kFdStdoutArm64) &&
                       movz(a64reg::kX8, kSysWriteArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }

            // (AR) يطبع عددًا صحيحًا موقَّعًا (في x9): itoa عبر sdiv/msub يبني الأرقامَ تنازليًّا في
            //      مخزنِ الإطار، ثمّ write. السالبُ يُعالَج بحيلةِ الباقي-السالب (النفيُ عبر xzr للرقم
            //      المطلق) بلا نفيِ x9 ⇒ **آمنٌ لـINT64_MIN**؛ ثمّ '-' في المخزن نفسِه. يستعمل x9..x14
            //      مُبدَّداتٍ (لا x15 الذي يحمل ip في نداءِ emitPrintFloat؛ ip موجبٌ فلا يسلك فرعَ السالب).
            bool emitPrintInt()
            {
                //   x9=القيمة، x10=١٠، x13=المؤشّر (قمّةُ المخزن، حصريّ)، x14=القمّة (للطول).
                if (!addImm(13, 31, static_cast<long long>(printBufTopSlot_) * 8) || // ptr = sp + top*8
                    !movReg(14, 13) ||                                             // top = ptr (نسخةٌ للطول)
                    !movz(10, kItoaRadixArm64))
                    return false;
                // (AR) الإشارة: إن x9 ≥ ٠ اقفز للولبِ الموجب (لا نمسّ x9). السالبُ يلي.
                if (!cmp(9, 31)) // cmp x9, xzr
                    return false;
                size_t toPositive;
                if (!emitBranchFwd(a64::mnem::kBge, "rel19", toPositive))
                    return false;
                // (AR) لولبُ السالب: x11=حاصل، x12=باقٍ(≤٠)؛ الرقم = ‎-باقٍ‎ (‎sub x12,xzr,x12‎) + '0'.
                const size_t negLoop = code_.size();
                if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                    !rrr(a64::mnem::kSub, 12, 31, 12) ||                 // x12 = xzr − x12 = |باقٍ|
                    !addImm(12, 12, kAsciiZeroArm64) ||
                    !subImm(13, 13, 1) || !strb(12, 13) ||
                    !movReg(9, 11) || !emitCbnzBack(9, negLoop))
                    return false;
                // (AR) بادئةُ '-' في المخزن نفسِه.
                if (!movz(12, kAsciiMinusArm64) || !subImm(13, 13, 1) || !strb(12, 13))
                    return false;
                size_t toWrite;
                if (!emitBranchFwd(a64::mnem::kB, "rel26", toWrite))
                    return false;
                // (AR) لولبُ الموجب: x11=حاصل، x12=باقٍ(٠..٩)؛ الرقم = باقٍ + '0'.
                if (!patchBranchFwd(toPositive, 23, 5))
                    return false;
                const size_t posLoop = code_.size();
                if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                    !addImm(12, 12, kAsciiZeroArm64) ||
                    !subImm(13, 13, 1) || !strb(12, 13) ||
                    !movReg(9, 11) || !emitCbnzBack(9, posLoop))
                    return false;
                if (!patchBranchFwd(toWrite, 25, 0)) // (AR) b غيرُ مشروط: imm26@25-0
                    return false;
                //   x1=المؤشّر؛ x2=الطول=(القمّة−المؤشّر)؛ write(stdout).
                return movReg(a64reg::kX1, 13) &&
                       rrr(a64::mnem::kSub, a64reg::kX2, 14, 13) &&
                       movz(a64reg::kX0, kFdStdoutArm64) &&
                       movz(a64reg::kX8, kSysWriteArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }

            // (AR) ثوابتُ نمطِ بتّاتٍ + رموزٌ للمُنسِّق العشريّ (لا حرفيّاتٍ خام).
            static constexpr unsigned long long kF64SignMask = 0x8000000000000000ULL;
            static constexpr unsigned long long kF64AbsMask = 0x7FFFFFFFFFFFFFFFULL;
            static constexpr long long kFloatPrecisionScale = 1000000; // ١٠^٦ (setprecision(6))
            static constexpr int kFloatDecimals = 6;

            // (AR) يطبع عددًا عشريًّا (نمطُ بتّاته i64 في x9) بصيغةِ المفسّر: fixed(6) ثمّ حذفُ
            //      الأصفار الزائدة مع إبقاءِ رقمٍ ≥١ بعد النقطة (نظير x86 emitPrintFloat). العشريّ
            //      يُحسَب عبر d0/d1؛ السجلّاتُ خدشٌ (الحوضُ مُنسكَبٌ حولَ الطباعة). +0.5 يقرّب.
            bool emitPrintFloat()
            {
                using namespace a64reg;
                // (١) خزّن البتّاتِ الأصليّة؛ إن كانت الإشارةُ سالبةً اطبع '-' واحسب |القيمة|.
                if (!strSlot(9, floatValSlot_))
                    return false;
                if (!movImm64Bits(kScratch0, kF64SignMask) || !rrr(a64::mnem::kAnd, kScratch1, 9, kScratch0) ||
                    !cmp(kScratch1, 31)) // (AR) x17 & vs xzr
                    return false;
                size_t skipNeg;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", skipNeg))
                    return false;
                if (!emitPrintString("-"))
                    return false;
                if (!patchBranchFwd(skipNeg, 23, 5))
                    return false;
                // |القيمة| = البتّات & قناعُ مسحِ الإشارة ⇒ خزّنها.
                if (!ldrSlot(9, floatValSlot_) || !movImm64Bits(kScratch0, kF64AbsMask) ||
                    !rrr(a64::mnem::kAnd, 9, 9, kScratch0) || !strSlot(9, floatValSlot_))
                    return false;
                // (٢) الجزءُ الصحيح ip = trunc(|x|) في x15 (يبقى عبر emitPrintInt الذي يمسّ x9-x14 فقط).
                if (!ldrSlot(9, floatValSlot_) || !fmovToFp(kD0, 9) || !fcvtzs(15, kD0))
                    return false;
                // (٣) الكسر: frac = |x| − ip ؛ scaled = round_nearest_even(frac×١٠^٦) في x11.
                //     fcvtns (nearest-even لا half-up ‎+0.5‎) يطابق تقريبَ IEEE الافتراضيّ للمفسّر.
                if (!fmovToFp(kD0, 9) || !scvtf(kD1, 15) || !fsub(kD0, kD0, kD1)) // d0=frac
                    return false;
                if (!loadFloatConst(kScratch0, static_cast<double>(kFloatPrecisionScale)) ||
                    !fmovToFp(kD1, kScratch0) || !fmul(kD0, kD0, kD1))
                    return false;
                if (!fcvtns(11, kD0)) // x11 = scaled (تحويلٌ بتقريبِ أقربِ زوج)
                    return false;
                // (٤) ترحيلُ الحمل: إن قرّب الكسرُ إلى ١٫٠ (scaled == ١٠^٦) زِد ip وصفّر scaled — قبل
                //      طباعةِ ip (عائقُ أميليا: «1.0 − 4e−7» كان يطبع «0.0»). ثمّ خزّن scaled (x11) في
                //      الخانة (البتّاتُ المطلقةُ انتهت) ليبقى عبر emitPrintInt.
                if (!movImm64Bits(kScratch0, static_cast<unsigned long long>(kFloatPrecisionScale)) ||
                    !cmp(11, kScratch0))
                    return false;
                size_t bneNoCarry;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", bneNoCarry))
                    return false;
                if (!addImm(15, 15, 1) || !movz(11, 0)) // ip++، scaled=0
                    return false;
                if (!patchBranchFwd(bneNoCarry, 23, 5))
                    return false;
                if (!strSlot(11, floatValSlot_))
                    return false;
                // (٥) اطبع ip (x9 = x15) ثمّ النقطة، ثمّ أعِد تحميلَ scaled إلى x11.
                if (!movReg(9, 15) || !emitPrintInt() || !emitPrintString(".") || !ldrSlot(11, floatValSlot_))
                    return false;
                // (٦) حذفُ الأصفار الزائدة: nd(x12)=٦؛ x10=١٠؛ x17=١ (للمقارنة). بينما nd>1 && scaled%10==0.
                if (!movz(12, kFloatDecimals) || !movz(10, kItoaRadixArm64) || !movz(kScratch1, 1))
                    return false;
                size_t stripTop = code_.size();
                if (!cmp(12, kScratch1))
                    return false;
                size_t jleStripDone;
                if (!emitBranchFwd(a64::mnem::kBle, "rel19", jleStripDone))
                    return false;
                if (!rrr(a64::mnem::kSdiv, 13, 11, 10) || !msub(14, 13, 10, 11) || !cmp(14, 31))
                    return false;
                size_t jneStripDone;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", jneStripDone))
                    return false;
                if (!movReg(11, 13) || !subImm(12, 12, 1) || !emitBBack(stripTop))
                    return false;
                if (!patchBranchFwd(jleStripDone, 23, 5) || !patchBranchFwd(jneStripDone, 23, 5))
                    return false;
                // (٦) ابنِ nd رقمًا تنازليًّا (أصفارٌ بادئةٌ تُحشى) ثمّ write(stdout).
                //     x13 مؤشّرُ الكتابة (يبدأ من القمّة)؛ x1 = القمّة − nd (بدايةُ الحقل).
                if (!addImm(13, 31, static_cast<long long>(printBufTopSlot_) * 8) ||
                    !rrr(a64::mnem::kSub, kX1, 13, 12))
                    return false;
                if (!movReg(9, 11)) // x9 = scaled (العملُ عليه)
                    return false;
                size_t digitTop = code_.size();
                if (!rrr(a64::mnem::kSdiv, 15, 9, 10) || !msub(kScratch0, 15, 10, 9) ||
                    !addImm(kScratch0, kScratch0, kAsciiZeroArm64) || !subImm(13, 13, 1) ||
                    !strb(kScratch0, 13) || !movReg(9, 15) || !emitCbnzBack(9, digitTop))
                    return false;
                // حشوُ الأصفار البادئة: بينما x13 > x1 أدخِل '0'.
                size_t padTop = code_.size();
                if (!cmp(13, kX1))
                    return false;
                size_t jlePadDone;
                if (!emitBranchFwd(a64::mnem::kBle, "rel19", jlePadDone))
                    return false;
                if (!subImm(13, 13, 1) || !movz(kScratch0, kAsciiZeroArm64) || !strb(kScratch0, 13) ||
                    !emitBBack(padTop))
                    return false;
                if (!patchBranchFwd(jlePadDone, 23, 5))
                    return false;
                // write(stdout, x1=البداية، x2=nd)
                return movReg(kX2, 12) && movz(kX0, kFdStdoutArm64) &&
                       movz(kX8, kSysWriteArm64) && emit(a64::mnem::kSvc, "", {});
            }

            // (AR) طباعةُ قيمةٍ معلَّبة (Any): ptrReg يشير إلى خانةِ dyn {tag@0، payload@8}. توزيعٌ
            //      زمنَ التشغيل على الوسم: Float⇒المُنسِّق، Bool⇒«صحيح»/«خطأ»، Int⇒itoa، غيرها⇒«عدم».
            //      x16=الوسم، x17=الحمولة (يبقيان حتّى دخولِ الفرع). كلُّ فرعٍ يقفز للنهاية (b).
            bool emitPrintBoxed(int ptrReg)
            {
                using namespace a64reg;
                if (!ldrBase(kScratch0, ptrReg, 0) || !ldrBase(kScratch1, ptrReg, 1)) // tag، payload
                    return false;
                std::vector<size_t> endJmps;
                auto branchEnd = [&]() -> bool {
                    size_t w;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", w))
                        return false;
                    endJmps.push_back(w);
                    return true;
                };
                // Float (tag==2) ⇒ المُنسِّق
                size_t notF;
                if (!movz(10, kDynKindFloat) || !cmp(kScratch0, 10) ||
                    !emitBranchFwd(a64::mnem::kBne, "rel19", notF))
                    return false;
                if (!movReg(9, kScratch1) || !emitPrintFloat() || !branchEnd())
                    return false;
                if (!patchBranchFwd(notF, 23, 5))
                    return false;
                // Bool (tag==4) ⇒ «صحيح»/«خطأ» حسب الحمولة (٠/١)
                size_t notB;
                if (!movz(10, kDynKindBool) || !cmp(kScratch0, 10) ||
                    !emitBranchFwd(a64::mnem::kBne, "rel19", notB))
                    return false;
                {
                    size_t isFalse;
                    if (!cmp(kScratch1, 31) || !emitBranchFwd(a64::mnem::kBeq, "rel19", isFalse)) // payload==0?
                        return false;
                    if (!emitPrintString(kDynBoolTrueText) || !branchEnd())
                        return false;
                    if (!patchBranchFwd(isFalse, 23, 5))
                        return false;
                    if (!emitPrintString(kDynBoolFalseText) || !branchEnd())
                        return false;
                }
                if (!patchBranchFwd(notB, 23, 5))
                    return false;
                // Int (tag==1) ⇒ itoa
                size_t notI;
                if (!movz(10, kDynKindInt) || !cmp(kScratch0, 10) ||
                    !emitBranchFwd(a64::mnem::kBne, "rel19", notI))
                    return false;
                if (!movReg(9, kScratch1) || !emitPrintInt() || !branchEnd())
                    return false;
                if (!patchBranchFwd(notI, 23, 5))
                    return false;
                // Str (tag==3) ⇒ نصٌّ: x17 = عنوانُ الواصف {len@0، bytes@8}. write(stdout, bytes, len).
                size_t notS;
                if (!movz(10, kDynKindStr) || !cmp(kScratch0, 10) ||
                    !emitBranchFwd(a64::mnem::kBne, "rel19", notS))
                    return false;
                if (!ldrBase(kX2, kScratch1, 0) ||          // x2 = الطول
                    !addImm(kX1, kScratch1, 8) ||           // x1 = البايتات
                    !movz(kX0, kFdStdoutArm64) || !movz(kX8, kSysWriteArm64) ||
                    !emit(a64::mnem::kSvc, "", {}) || !branchEnd())
                    return false;
                if (!patchBranchFwd(notS, 23, 5))
                    return false;
                // غيرها (Null) ⇒ «عدم»
                if (!emitPrintString(kDynNullText))
                    return false;
                for (size_t w : endJmps)
                    if (!patchBranchFwd(w, 25, 0)) // (AR) b غيرُ مشروط: imm26@25-0
                        return false;
                return true;
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

            // (AR) فرعٌ أماميّ بإزاحةٍ نائبةٍ (٠) يُرقَّع لاحقًا (للولباتِ الإلحاق المحلّيّة، لا لصائقَ
            //      كتلٍ ⇒ لا طابورَ Fixup). يُعيد موضعَ الكلمة؛ patchBranchFwd يملأ حقلَ الإزاحة.
            bool emitBranchFwd(const std::string &mnem, const std::string &form, size_t &outWord)
            {
                outWord = code_.size();
                return emit(mnem, form, {a64::Operand::I(0)});
            }
            // (AR) يُرقّع فرعًا أماميًّا (كلمتُه في wordPos، حقلُ الإزاحة [immHi..immLo]) إلى الموضعِ
            //      الحاليّ. الإزاحةُ = (الهدف − موضعُ الفرع) ÷ ٤ تعليمات؛ تُقنَّع وتُدخَل OR.
            bool patchBranchFwd(size_t wordPos, int immHi, int immLo)
            {
                const long long imm = (static_cast<long long>(code_.size()) - static_cast<long long>(wordPos)) / 4;
                const int width = immHi - immLo + 1;
                const long long lim = 1LL << (width - 1);
                if (imm < -lim || imm > lim - 1)
                    return fail(EC::INT_NATIVE_IMM_RANGE, diag::kRel + std::to_string(imm));
                const uint32_t mask = (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);
                const uint32_t field = (static_cast<uint32_t>(imm) & mask) << immLo;
                uint32_t w = 0;
                for (int i = 0; i < 4; ++i)
                    w |= static_cast<uint32_t>(code_[wordPos + i]) << (8 * i);
                w |= field;
                for (int i = 0; i < 4; ++i)
                    code_[wordPos + i] = static_cast<uint8_t>((w >> (8 * i)) & 0xFF);
                return true;
            }
            // (AR) فرعٌ غيرُ مشروطٍ خلفيٌّ إلى هدفٍ معلوم (لبدايةِ لولبِ النسخ): imm26 موقَّع.
            bool emitBBack(size_t target)
            {
                const long long imm = (static_cast<long long>(target) - static_cast<long long>(code_.size())) / 4;
                return emit(a64::mnem::kB, "rel26", {a64::Operand::I(imm)});
            }
            // (AR) mmap بحجمٍ مُهيّأٍ سلفًا في x1 (لنموّ الإلحاق: الحجمُ newcap×٨ يُحسَب زمنَ التشغيل).
            bool emitMmapArm64PresetSize()
            {
                return movz(a64reg::kX0, 0) &&
                       movz(a64reg::kX2, kProtReadWrite) &&
                       movz(3, kMapPrivAnon) &&
                       movz(4, 0) && subImm(4, 4, 1) &&
                       movz(5, 0) &&
                       movz(a64reg::kX8, kSysMmapArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }

            bool allocReg(const std::string &vreg, int &out)
            {
                auto it = regOf_.find(vreg);
                if (it != regOf_.end())
                {
                    out = it->second;
                    return true;
                }
                // (AR) المسارُ الأحاديّ (يحفظ بايتات البرامج القائمة تمامًا): خصّص التاليَ ما دام
                //      في الحوض متّسع ⇒ صفر انحدارٍ بايتيّ لما كان يعمل.
                if (next_ < pool_.size())
                {
                    out = pool_[next_++];
                    regOf_[vreg] = out;
                    return true;
                }
                // (AR) نفد الحوضُ الأحاديّ: استرجِعْ سجلَّ مؤقّتٍ ميّتٍ (ليس معاملَ التعليمة الحاليّة
                //      وليس حيًّا بعدها) — لازمٌ للمصفوفات (مؤقّتاتٌ كثيرةٌ قصيرةُ العمر). نظيرُ x86.
                if (curBlock_ && curInstIdx_ < curBlock_->instructions.size())
                {
                    const auto &curOps = curBlock_->instructions[curInstIdx_].operands;
                    auto usedNow = [&curOps](const std::string &n) {
                        for (const auto &op : curOps)
                            if (op.type == sir::SIROperandType::REGISTER && op.name == n)
                                return true;
                        return false;
                    };
                    for (auto rit = regOf_.begin(); rit != regOf_.end(); ++rit)
                        if (!usedNow(rit->first) &&
                            !common::usedAfterInBlock(*curBlock_, curInstIdx_, rit->first))
                        {
                            out = rit->second;
                            regOf_.erase(rit);
                            regOf_[vreg] = out;
                            return true;
                        }
                }
                return fail(EC::INT_NATIVE_REGALLOC_EXHAUSTED, diag::kPool + std::to_string(pool_.size()));
            }

            // (AR) يُجهّز معاملًا في سجلٍّ وجهة: ثابتٌ ⇒ movz؛ متغيّرُ ذاكرةٍ ⇒ ldr من خانته (قراءةُ
            //      اسمِ ALLOC/معاملٍ كقيمة = تحميلٌ ضمنيّ)؛ سجلٌّ افتراضيٌّ ⇒ نسخٌ من موضعه.
            bool materialize(int dst, const sir::SIROperand &op)
            {
                long long c;
                if (common::isConstInt(op, c))
                    return movz(dst, c);
                // (AR) ثابتٌ عشريّ ⇒ حمّل نمطَ بتّاته i64 (العشريّ يعيش كبتّاتٍ في GPR).
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Float)
                    return loadFloatConst(dst, op.floatValue);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    int slot;
                    if (isMemVar(op, slot))
                        return ldrSlot(dst, slot);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, diag::kVregSigil + op.name);
                    return movReg(dst, it->second);
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            diag::kOperandKind + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) يحمّل وسيطَ نداءٍ/معاملَ طباعةٍ في سجلٍّ وجهة. يُستدعى بعد انسكابِ المؤقّتات،
            //      فالمؤقّتُ يُحمَّل من **خانة انسكابه** لا من سجلّه ⇒ صفر تصادمِ نقلٍ متوازٍ.
            bool loadArgInto(int dst, const sir::SIROperand &op)
            {
                long long c;
                if (common::isConstInt(op, c))
                    return movz(dst, c);
                // (AR) ثابتٌ عشريّ ⇒ نمطُ بتّاته i64 (العشريّ يعيش كبتّاتٍ في GPR).
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Float)
                    return loadFloatConst(dst, op.floatValue);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    int slot;
                    if (isMemVar(op, slot))
                        return ldrSlot(dst, slot);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, diag::kVregSigil + op.name);
                    return ldrSlot(dst, spillBaseSlot_ + poolIndexOf(it->second));
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            diag::kArgKind + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) المسحُ المسبق: يخصّص فهرسَ خانةٍ للمعاملات (بترتيب ABI) ثمّ لكلّ ALLOC، ثمّ (إن
            //      لزم) لمخزنِ itoa ولمنطقة الانسكاب؛ ويحسب حجمَ الإطارِ المُحاذى ١٦. المعاملُ يُعامَل
            //      كمتغيّرِ ذاكرةٍ (قراءتُه = تحميلٌ من خانته)، وتُسكَنُ خانتُه من سجلّ الوسيط في المقدّمة.
            bool assignFrameSlots(const sir::SIRFunction &fn)
            {
                const auto &params = fn.getParameters();
                if (params.size() > 8)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kParamsGt8 + std::to_string(params.size()));
                int slot = 0;
                for (const auto &p : params)
                    // (AR) المعاملُ يُشار إليه بـ«%»+الاسم (sir_builder_functions) بينما اسمُ
                    //      SIRParameter بلا «%» ⇒ نُفهرِس بالمرجع كي يطابقَه isMemVar.
                    memSlot_[diag::kVregSigil + p.name] = slot++;

                bool hasCall = false;
                bool hasPrint = false;
                bool hasNumberPrint = false;
                bool hasFloatPrint = false; // (AR) طباعةُ عشريّ ⇒ خانةُ خدشِ البتّات + مُنسِّق fixed6
                bool hasArrayNew = false; // (AR) ARRAY_NEW ⇒ mmap (نسكٌ تحفّظيّ حولَ svc)
                bool hasAppend = false;   // (AR) BUILTIN_ARRAY_APPEND ⇒ mmap (عند النموّ) + خانات خدش
                bool hasBoxing = false;   // (AR) SET/GET معلَّبٌ ⇒ يستعمل الحوضَ خدشًا + خانات dyn
                dynGetCount_ = 0;
                for (const auto &blockPtr : fn.getBasicBlocks())
                    for (const auto &inst : blockPtr->instructions)
                    {
                        if (inst.opcode == sir::SIROpcode::ALLOC && inst.result)
                            memSlot_[inst.result->name] = slot++;
                        else if (inst.opcode == sir::SIROpcode::CALL)
                            hasCall = true;
                        else if (inst.opcode == sir::SIROpcode::ARRAY_NEW)
                            hasArrayNew = true;
                        else if (inst.opcode == sir::SIROpcode::BUILTIN_ARRAY_APPEND ||
                                 inst.opcode == sir::SIROpcode::ARRAY_APPEND)
                            hasAppend = true;
                        else if (inst.opcode == sir::SIROpcode::BUILTIN_PRINT)
                        {
                            hasPrint = true;
                            for (const auto &op : inst.operands)
                            {
                                if (op.dataType != types::SadTypeKind::String)
                                    hasNumberPrint = true; // (AR) معاملٌ غيرُ نصّيٍّ ⇒ يُطبَع عددًا (itoa)
                                // (AR) العشريّ أو المعلَّب (Any، قد يُبوَّب عشريًّا) ⇒ يلزمه المُنسِّق.
                                if (op.dataType == types::SadTypeKind::Float ||
                                    op.dataType == types::SadTypeKind::Any)
                                    hasFloatPrint = true;
                            }
                        }
                        // (AR) SET معلَّبٌ: Any على المعامل. GET معلَّبٌ: Any على النتيجة (كمسار LLVM).
                        if (inst.opcode == sir::SIROpcode::ARRAY_SET && !inst.operands.empty() &&
                            inst.operands[0].elementType == types::SadTypeKind::Any)
                            hasBoxing = true;
                        if (inst.opcode == sir::SIROpcode::ARRAY_GET && inst.result &&
                            inst.result->dataType == types::SadTypeKind::Any)
                        {
                            hasBoxing = true;
                            ++dynGetCount_;
                        }
                    }
                // (AR) 🔑 النطاق (نداءٌ ورقيّ بلا حفظ x30): الدالّةُ الداخلة تنادي وتخرج svc (لا
                //      تعود، فلا يهمّها دهسُ x30). الدالّةُ غيرُ الداخلة إن نادت تدهسُ x30 قبل RET
                //      (لا نحفظه بعد) ⇒ رفضٌ صريح. فالنداءُ مسموحٌ من الداخلة فقط (المنادَاةُ ورقة).
                if (hasCall && !curIsEntry_)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kNestedCallInNonentry + currentFn_);

                // (AR) طباعةُ عددٍ: احجز مخزنَ itoa (٣ خاناتٍ = ٢٤ بايتًا تكفي ٢٠ رقمًا لـi64 + هامش).
                //      القمّةُ (العنوانُ الأعلى، حصريّ) = فهرسُ ما بعد المخزن.
                if (hasNumberPrint)
                {
                    slot += 3;
                    printBufTopSlot_ = slot; // (AR) العنوانُ الأعلى الحصريّ = sp + slot*8
                }
                // (AR) طباعةُ عشريّ: خانةٌ لنمطِ بتّاتِ الـdouble (القيمةِ المطلقة) عبر مراحل المُنسِّق.
                if (hasFloatPrint)
                    floatValSlot_ = slot++;
                // (AR) الإلحاق: خمسُ خاناتٍ تبقى حيّةً عبر mmap (P/القيمة/الطول/newdata/newcap).
                if (hasAppend)
                {
                    appendPSlot_ = slot++;
                    appendValSlot_ = slot++;
                    appendLenSlot_ = slot++;
                    appendNewSlot_ = slot++;
                    appendCapSlot_ = slot++;
                }
                // (AR) التعليب: خانتان (tag، payload) لكلّ قراءةٍ معلَّبة. المؤشّرُ = sp + الفهرس×٨.
                if (dynGetCount_ > 0)
                {
                    dynBaseSlot_ = slot;
                    slot += dynGetCount_ * 2;
                }
                // (AR) إن نادت الدالّةُ أو طبعت أو خصّصت مصفوفة أو ألحقت أو علّبت، احجز منطقةَ انسكابٍ.
                if (hasCall || hasPrint || hasArrayNew || hasAppend || hasBoxing)
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
                        return fail(EC::INT_NATIVE_IMM_RANGE, diag::kFrame + std::to_string(frameSize_));
                    if (!subSp(frameSize_))
                        return false;
                }
                const auto &params = fn.getParameters();
                for (size_t i = 0; i < params.size(); ++i)
                    if (!strSlot(abiArg_[i], memSlot_[diag::kVregSigil + params[i].name]))
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
                case OP::SHR:
                    // (AR) `>>` إشارتُه من نوعِ المعامل الأيسر (طبيعي64/بايت⇒منطقيّة lsrv، غيرُها⇒حسابيّة
                    //      asrv) مطابقةً للمفسّر ومسارِ LLVM: `-16>>2=-4` لا قيمةً ضخمة.
                    return rrr(isUnsignedType(inst.operands[0].dataType) ? a64::mnem::kLsrv : a64::mnem::kAsrv,
                               dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SAR: return rrr(a64::mnem::kAsrv, dst, a64reg::kScratch0, a64reg::kScratch1); // (AR) حسابيّة (تمدّ الإشارة)
                // (AR) حسابٌ عشريّ: العشريّ بتّاتُ i64 في x16/x17 ⇒ عبّئهما في d0/d1، نفّذ عمليّةَ
                //      FP المزدوجة، ثمّ استخرج البتّات إلى dst (نظير x86 عبر xmm).
                case OP::ADD_F64:
                    return fmovToFp(kD0, a64reg::kScratch0) && fmovToFp(kD1, a64reg::kScratch1) &&
                           fadd(kD0, kD0, kD1) && fmovFromFp(dst, kD0);
                case OP::SUB_F64:
                    return fmovToFp(kD0, a64reg::kScratch0) && fmovToFp(kD1, a64reg::kScratch1) &&
                           fsub(kD0, kD0, kD1) && fmovFromFp(dst, kD0);
                case OP::MUL_F64:
                    return fmovToFp(kD0, a64reg::kScratch0) && fmovToFp(kD1, a64reg::kScratch1) &&
                           fmul(kD0, kD0, kD1) && fmovFromFp(dst, kD0);
                case OP::DIV_F64:
                    return fmovToFp(kD0, a64reg::kScratch0) && fmovToFp(kD1, a64reg::kScratch1) &&
                           fdiv(kD0, kD0, kD1) && fmovFromFp(dst, kD0);
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // (AR) تحويلُ صحيح↔عشريّ (أحاديّ): جهّز المعاملَ في x16، حوّل عبر d0، استخرج إلى dst.
            bool lowerFloatConv(const sir::SIRInstruction &inst)
            {
                if (!inst.result || inst.operands.size() != 1)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                if (!materialize(a64reg::kScratch0, inst.operands[0]))
                    return false;
                if (inst.opcode == sir::SIROpcode::I64_TO_F64)
                    return scvtf(kD0, a64reg::kScratch0) && fmovFromFp(dst, kD0); // صحيح ⇒ عشريّ
                return fmovToFp(kD0, a64reg::kScratch0) && fcvtzs(dst, kD0);      // عشريّ ⇒ صحيح باقتطاع
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
            // (AR) حقلُ الشرطِ المقلوب لمقارنةِ العوائم (IEEE عبر fcmp): يختلف عن الصحيح في <,<=
            //      فقط. الأصغرُ يستعمل MI (N=1) لا LT (N≠V) — لأنّ NaN يضبط V=1 فتَصدُق LT خطأً؛
            //      والأصغر-أو-يساوي يستعمل LS (C=0∨Z=1) لا LE. البقيّةُ (==,!=,>,>=) كالصحيح لأنّ
            //      شروطَها (EQ/NE/GT/GE) تُخيَّب أصلًا عند غيرِ المرتَّب. الحقلُ = رمزُ الشرط XOR 1.
            static bool csetFloatInvertedField(sir::SIROpcode op, long long &field)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: field = 1;  return true;  // invert(EQ)=NE
                case OP::NE: field = 0;  return true;  // invert(NE)=EQ
                case OP::LT: field = 5;  return true;  // invert(MI)=PL  (الأصغر IEEE عبر MI)
                case OP::LE: field = 8;  return true;  // invert(LS)=HI  (الأصغر-أو-يساوي عبر LS)
                case OP::GT: field = 13; return true;  // invert(GT)=LE
                case OP::GE: field = 11; return true;  // invert(GE)=LT
                default: return false;
                }
            }
            // (AR) مقارنةُ عوائم ⇒ %dst = ٠/١ بدلالة IEEE: انقل نمطَي البتّات لسجلَّي d الخدش، fcmp،
            //      ثمّ cset بحقلِ العائم المقلوب. النتيجةُ خطأٌ لأيّ NaN عدا !=.
            bool lowerFloatComparison(const sir::SIRInstruction &inst)
            {
                long long field;
                if (!csetFloatInvertedField(inst.opcode, field))
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                return materialize(a64reg::kScratch0, inst.operands[0]) &&
                       materialize(a64reg::kScratch1, inst.operands[1]) &&
                       fmovToFp(kD0, a64reg::kScratch0) && fmovToFp(kD1, a64reg::kScratch1) &&
                       fcmp(kD0, kD1) &&
                       emit(a64::mnem::kCset, "x, cond", {a64::Operand::R(dst), a64::Operand::I(field)});
            }
            bool lowerComparison(const sir::SIRInstruction &inst)
            {
                if (!inst.result || inst.operands.size() != 2)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                // (AR) مقارنةُ عوائم (أحدُ المعامِلَين Float): fcmp + cset بدلالة IEEE.
                if (common::isFloatCompare(inst))
                    return lowerFloatComparison(inst);
                // (AR) رفضُ المقارنةِ اللا-موقَّعة صراحةً (cset يحتاج شرطًا لا-موقَّعًا؛ توصية أميليا).
                for (const auto &op : inst.operands)
                    if (isUnsignedType(op.dataType))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    diag::kCmpValueUnsigned + std::to_string(static_cast<int>(op.dataType)));
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

            // (AR) أحاديّ: %dst = ~a (NOT ⇒ mvn) أو −a (NEG ⇒ sub dst,xzr,a). جهّز a في x16.
            bool lowerNot(const sir::SIRInstruction &inst)
            {
                if (!inst.result || inst.operands.size() != 1)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                if (!materialize(a64reg::kScratch0, inst.operands[0]))
                    return false;
                if (inst.opcode == sir::SIROpcode::NEG) // (AR) −a = xzr − a (متمّمٌ ثنائيّ)
                    return rrr(a64::mnem::kSub, dst, 31, a64reg::kScratch0);
                return emit(a64::mnem::kMvn, "x, x",
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
                    // (AR) ثابتٌ عشريّ ⇒ حمّل نمطَ بتّاته i64 (العشريّ يعيش كبتّاتٍ في GPR).
                    if (inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                        inst.operands[0].dataType == types::SadTypeKind::Float)
                        return loadFloatConst(dst, inst.operands[0].floatValue);
                    if (inst.operands[0].type == sir::SIROperandType::REGISTER)
                    {
                        int slot;
                        if (isMemVar(inst.operands[0], slot))
                            return ldrSlot(dst, slot);
                        auto it = regOf_.find(inst.operands[0].name);
                        if (it == regOf_.end())
                            return fail(EC::INT_NATIVE_UNDEF_VREG, diag::kVregSigil + inst.operands[0].name);
                        return movReg(dst, it->second);
                    }
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kMoveKind);
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
                case OP::SAR:
                case OP::ADD_F64:
                case OP::SUB_F64:
                case OP::MUL_F64:
                case OP::DIV_F64:
                    return lowerBinary(inst);
                case OP::I64_TO_F64:
                case OP::F64_TO_I64:
                    return lowerFloatConv(inst);
                case OP::NOT:
                case OP::NEG:
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
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kAllocUnslotted + detailOpcode(inst));
                    return true;
                }
                case OP::LOAD:
                {
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int slot;
                    if (!isMemVar(inst.operands[0], slot))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kLoadNonslot + detailOpcode(inst));
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
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kStoreNonslot + detailOpcode(inst));
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
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArgsGt8 + std::to_string(argc));
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
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kPrintStrComputed + diag::kVregSigil + op.name);
                        else if (op.dataType == types::SadTypeKind::Any)
                        {
                            // (AR) معلَّب: المعاملُ مؤشّرٌ إلى خانةِ dyn ⇒ طباعةٌ مبوَّبةٌ زمنَ التشغيل.
                            if (!loadArgInto(9, op) || !emitPrintBoxed(9))
                                return false;
                        }
                        else if (op.dataType == types::SadTypeKind::Float)
                        {
                            // (AR) عشريّ: حمّل نمطَ بتّاته في x9 ثمّ المُنسِّق (fixed6 + حذفُ الأصفار).
                            if (!loadArgInto(9, op) || !emitPrintFloat())
                                return false;
                        }
                        else
                        {
                            // (AR) عددٌ صحيح: حمّله في x9 (ثابت/ذاكرة/خانة انسكاب) ثمّ itoa+write.
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
                case OP::RET_VOID:
                {
                    // (AR) إرجاعٌ فارغ: الداخلةُ تخرج بـexit(0)؛ غيرُها خاتمةٌ + ret بلا ضبطِ x0.
                    if (curIsEntry_)
                        return movz(a64reg::kX0, 0) && movz(a64reg::kX8, kSysExitArm64) &&
                               emit(a64::mnem::kSvc, "", {});
                    return emitEpilogue();
                }
                case OP::ARRAY_NEW:
                {
                    // (AR) result = مصفوفةٌ جديدة؛ operands=[len(const), cap(const)]. mmap كتلةً
                    //      واحدة (رأسٌ ٤٠ + بياناتٌ cap×٨)، تهيئةُ len/cap/data؛ tags/homogKind
                    //      يُصفّرهما mmap. السعةُ ثابتةٌ؛ الحجمُ الديناميّ مؤجَّلٌ ⇒ فشلٌ صريح.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long lenv, capv;
                    if (!common::isConstInt(inst.operands[0], lenv) ||
                        !common::isConstInt(inst.operands[1], capv))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayNewDynamicSize);
                    if (lenv < 0 || capv < 0)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayNewNegative);
                    // (AR) نُخصّص مخزنَ الوسوم (cap×٨) بعد البيانات + نضبط arr[tags]=data_end (كـx86).
                    const long long total = kArrHeaderBytes + capv * kArrSlotBytes + capv * kTagSlotBytes;
                    // (AR) انسكِبْ المؤقّتاتِ الحيّةَ (تحفّظًا حولَ svc)، ثمّ mmap، ثمّ أعِدها.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;
                    if (!emitMmapArm64(total))
                        return false;
                    // (AR) x0 = المؤشّر. x16 خدشٌ: len ثمّ cap ثمّ data=x0+40.
                    if (!movz(a64reg::kScratch0, lenv) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffLen / kArrSlotBytes))
                        return false;
                    if (!movz(a64reg::kScratch0, capv) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffCap / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kScratch0, a64reg::kX0, kArrHeaderBytes) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffData / kArrSlotBytes))
                        return false;
                    // (AR) tags = data_end = x0 + 40 + cap×8 ⇒ arr[tags] = هذا العنوان.
                    if (!addImm(a64reg::kScratch0, a64reg::kX0, kArrHeaderBytes + capv * kArrSlotBytes) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffTags / kArrSlotBytes))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kX0);
                }
                case OP::ARRAY_SET:
                {
                    // (AR) operands=[arr(reg,Array), index(const/reg), value(reg)]. نحسب عنوانَ
                    //      العنصر ثمّ نخزّن القيمة. المعلَّب (elementType=Any) مؤجَّلٌ ⇒ فشلٌ صريح.
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (inst.operands[0].elementType == types::SadTypeKind::Any)
                    {
                        // (AR) تخزينٌ معلَّب: الحمولةُ الخام في data[idx]، والوسمُ في tags[idx] (خانةٌ ٨-بت).
                        //      الوسمُ ثابتٌ من نوعِ القيمة. نستعمل الحوضَ خدشًا (x9-x12 مُنسكَبة).
                        // (AR) النصُّ المعلَّب: الوسمُ Str، والحمولةُ عنوانُ واصفٍ في rodata (لا قيمةٌ خام).
                        const bool isStr = inst.operands[2].dataType == types::SadTypeKind::String;
                        std::string strContent;
                        long long tag;
                        if (isStr)
                        {
                            if (!resolveBoxedStringLiteral(inst.operands[2], strContent))
                                return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArraySetBoxed);
                            tag = kDynKindStr;
                        }
                        else if (!dynTagForType(inst.operands[2].dataType, tag))
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArraySetBoxed);
                        // (AR) انسِك المؤقّتاتِ الحيّةَ **ومعاملاتِ هذه التعليمة** ⇒ نقرأ المعاملاتِ من
                        //      خانات الانسكاب (loadArgInto) لا سجلّاتها الفيزيائيّة ⇒ لا تصادمَ نقلٍ
                        //      حين يشترك معاملٌ وشقيقُه في سجلِّ حوضٍ (تحصينُ أميليا؛ نظيرُ x86 خارج الحوض).
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                                common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                                if (!spillReg(kv.second))
                                    return false;
                        // x9=arr، x17=idx، x16=len، فحصُ الحدّ.
                        if (!loadArgInto(9, inst.operands[0]) || !loadArgInto(a64reg::kScratch1, inst.operands[1]) ||
                            !ldrBase(a64reg::kScratch0, 9, kArrOffLen / kArrSlotBytes) || !emitBoundsCheckArm64())
                            return false;
                        // data[idx] = x10 (الحمولة: النصّ⇒عنوانُ الواصف؛ غيرُه⇒قيمةٌ خام): x11 = [arr+data] + idx×8.
                        if (isStr ? !emitLoadStrDescAddr(10, strContent) : !loadArgInto(10, inst.operands[2]))
                            return false;
                        if (!ldrBase(11, 9, kArrOffData / kArrSlotBytes) ||
                            !addLsl3(11, 11, a64reg::kScratch1) || !strBase(10, 11, 0))
                            return false;
                        // tags[idx] = tag: x11 = [arr+tags] + idx×8.
                        if (!ldrBase(11, 9, kArrOffTags / kArrSlotBytes) || !addLsl3(11, 11, a64reg::kScratch1) ||
                            !movz(10, tag) || !strBase(10, 11, 0))
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!reloadReg(kv.second))
                                    return false;
                        return true;
                    }
                    long long off;
                    if (!emitElemAddrArm64(inst.operands[0], inst.operands[1], off))
                        return false;
                    // (AR) x17 = القيمة (بعد حساب العنوان ⇒ فهرسُه في x17 لم يعد مطلوبًا)؛ str.
                    return materialize(a64reg::kScratch1, inst.operands[2]) &&
                           strBase(a64reg::kScratch1, a64reg::kScratch0, off);
                }
                case OP::ARRAY_GET:
                {
                    // (AR) result = arr[index]. نحسب عنوانَ العنصر ثمّ نحمّله في الوجهة.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) القراءةُ المعلَّبة تُكشَف بنوعِ **النتيجة** Any (كمسار LLVM) لا المعامل.
                    if (inst.result->dataType == types::SadTypeKind::Any)
                    {
                        // (AR) نقرأ الحمولةَ (data[idx]) والوسمَ (tags[idx])، نبني قيمةً ديناميّةً في
                        //      خانتَي dyn ({tag، payload})، ونُعيد مؤشّرَها (sp + الفهرس×٨).
                        // (AR) انسِك المؤقّتاتِ الحيّةَ ومعاملاتِ التعليمة ⇒ اقرأها من خانات الانسكاب (تحصينُ أميليا).
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                                common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                                if (!spillReg(kv.second))
                                    return false;
                        if (!loadArgInto(9, inst.operands[0]) || !loadArgInto(a64reg::kScratch1, inst.operands[1]) ||
                            !ldrBase(a64reg::kScratch0, 9, kArrOffLen / kArrSlotBytes) || !emitBoundsCheckArm64())
                            return false;
                        // x10 = data[idx] (الحمولة)
                        if (!ldrBase(11, 9, kArrOffData / kArrSlotBytes) || !addLsl3(11, 11, a64reg::kScratch1) ||
                            !ldrBase(10, 11, 0))
                            return false;
                        // x12 = tags[idx] (الوسم)
                        if (!ldrBase(11, 9, kArrOffTags / kArrSlotBytes) || !addLsl3(11, 11, a64reg::kScratch1) ||
                            !ldrBase(12, 11, 0))
                            return false;
                        const int ts = dynBaseSlot_ + dynSlotNext_ * 2;
                        ++dynSlotNext_;
                        if (!strSlot(12, ts) || !strSlot(10, ts + 1)) // tag@ts، payload@ts+1
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!reloadReg(kv.second))
                                    return false;
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return addImm(dst, 31, static_cast<long long>(ts) * 8); // (AR) dst = sp + ts×8 (مؤشّرُ dyn)
                    }
                    long long off;
                    if (!emitElemAddrArm64(inst.operands[0], inst.operands[1], off))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrBase(dst, a64reg::kScratch0, off);
                }
                case OP::ARRAY_LEN:
                {
                    // (AR) result = طول(arr) ⇒ [arr+0].
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!materialize(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrBase(dst, a64reg::kScratch0, kArrOffLen / kArrSlotBytes);
                }
                case OP::BUILTIN_ARRAY_APPEND:
                case OP::ARRAY_APPEND:
                {
                    // (AR) الإلحاق (نظيرُ x86): operands=[arr, value]، النتيجةُ Void. إن L<C خزّن؛
                    //      وإلّا نمِّ (mmap سعةٍ مضاعفةٍ + نسخُ L خانة + تحديثُ data/cap) ثمّ خزّن.
                    //      المعلَّب (Any) مؤجَّل. صفرُ ترميزٍ جديد: cmp cnt,xzr للولب؛ b.lt/b.eq/b/cbnz.
                    //      سجلّاتُ العمل x16/x17 + الحوض المنسكِب x9-x12؛ ما يعبر mmap ⇒ خاناتُ إطار.
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (inst.operands[0].elementType == types::SadTypeKind::Any)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayAppendBoxed);
                    constexpr int kX9 = 9, kX10 = 10, kX11 = 11, kX12 = 12;
                    const int s0 = a64reg::kScratch0, s1 = a64reg::kScratch1; // x16, x17
                    const long long capIdx = kArrOffCap / kArrSlotBytes, dataIdx = kArrOffData / kArrSlotBytes;
                    // (AR) انسكِبْ كلَّ مؤقّتٍ حيّ (الإلحاق يستعمل الحوضَ خدشًا، وmmap يدهسه).
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) خزّن القيمةَ والمؤشّرَ P في الإطار (يبقيان عبر mmap)؛ s0 = P.
                    if (!materialize(s0, inst.operands[1]) || !strSlot(s0, appendValSlot_))
                        return false;
                    if (!materialize(s0, inst.operands[0]) || !strSlot(s0, appendPSlot_))
                        return false;
                    // (AR) L = [P+len]؛ احفظه؛ C = [P+cap]؛ cmp L,C؛ b.lt إلى التخزين.
                    if (!ldrBase(s1, s0, kArrOffLen / kArrSlotBytes) || !strSlot(s1, appendLenSlot_))
                        return false;
                    if (!ldrBase(kX9, s0, capIdx) || !cmp(s1, kX9))
                        return false;
                    size_t bltStore;
                    if (!emitBranchFwd(a64::mnem::kBlt, "rel19", bltStore))
                        return false;
                    // ── النموّ: newcap = (C==0)?1:C×2 (x9=C) ──
                    if (!rrr(a64::mnem::kAdd, kX10, kX9, kX9)) // x10 = 2C
                        return false;
                    // (AR) cbnz x10, +2 تعليمة (تخطّي movz 1) — عددٌ ثابتٌ (لا ترقيع).
                    if (!emit(a64::mnem::kCbnz, "x, rel19", {a64::Operand::R(kX10), a64::Operand::I(2)}))
                        return false;
                    if (!movz(kX10, 1)) // newcap = 1 (حين C==0)
                        return false;
                    if (!strSlot(kX10, appendCapSlot_))
                        return false;
                    // (AR) newsize = newcap×8 في x1؛ mmap ⇒ x0=newdata؛ احفظه.
                    if (!movz(kX11, 3) || !rrr(a64::mnem::kLslv, a64reg::kX1, kX10, kX11))
                        return false;
                    if (!emitMmapArm64PresetSize() || !strSlot(a64reg::kX0, appendNewSlot_))
                        return false;
                    // (AR) لولبُ النسخ: x9=olddata=[P+data]، x10=newdata، x11=L (عدّاد)، x12=خدش.
                    if (!ldrSlot(s0, appendPSlot_) || !ldrBase(kX9, s0, dataIdx))
                        return false;
                    if (!ldrSlot(kX10, appendNewSlot_) || !ldrSlot(kX11, appendLenSlot_))
                        return false;
                    const size_t copyTop = code_.size();
                    if (!cmp(kX11, 31)) // (AR) cmp cnt, xzr (سجلّ ٣١ = xzr في subs)
                        return false;
                    size_t beqDone;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", beqDone))
                        return false;
                    if (!ldrBase(kX12, kX9, 0) || !strBase(kX12, kX10, 0)) // *new = *old
                        return false;
                    if (!addImm(kX9, kX9, kArrSlotBytes) || !addImm(kX10, kX10, kArrSlotBytes) ||
                        !subImm(kX11, kX11, 1))
                        return false;
                    if (!emitBBack(copyTop))
                        return false;
                    if (!patchBranchFwd(beqDone, 23, 5)) // (AR) b.eq imm19@23-5
                        return false;
                    // (AR) حدّث [P+data]=newdata؛ [P+cap]=newcap.
                    if (!ldrSlot(s0, appendPSlot_))
                        return false;
                    if (!ldrSlot(kX9, appendNewSlot_) || !strBase(kX9, s0, dataIdx))
                        return false;
                    if (!ldrSlot(kX9, appendCapSlot_) || !strBase(kX9, s0, capIdx))
                        return false;
                    // ── التخزينُ المشترك: data[L] = value؛ [P+len] = L+1 ──
                    if (!patchBranchFwd(bltStore, 23, 5)) // (AR) b.lt imm19@23-5
                        return false;
                    if (!ldrSlot(s0, appendPSlot_) || !ldrBase(kX9, s0, dataIdx))
                        return false;
                    if (!ldrSlot(kX10, appendLenSlot_) || !addLsl3(kX9, kX9, kX10)) // x9 = data + L×8
                        return false;
                    if (!ldrSlot(kX11, appendValSlot_) || !strBase(kX11, kX9, 0)) // data[L] = value
                        return false;
                    if (!ldrSlot(kX10, appendLenSlot_) || !addImm(kX10, kX10, 1) ||
                        !strBase(kX10, s0, kArrOffLen / kArrSlotBytes)) // [P+len] = L+1
                        return false;
                    // (AR) أعِد المؤقّتاتِ الحيّة.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    return true;
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
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + diag::kFormSep + form);
                int immHi = -1, immLo = -1;
                for (const auto &f : spec->fields)
                    if (!f.is_const && f.from_op == 0)
                    {
                        immHi = f.hi;
                        immLo = f.lo;
                    }
                if (immHi < 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + diag::kNoImmField);
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
                                    diag::kCondConstType + std::to_string(static_cast<int>(cond.dataType)));
                    return emitBranch(a64::mnem::kB, "rel26", cond.boolValue ? thenLbl : elseLbl);
                }
                if (cond.type != sir::SIROperandType::REGISTER)
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                diag::kCondKind + std::to_string(static_cast<int>(cond.type)));

                const sir::SIRInstruction *cmpInst = common::findFusedComparison(block, cond.name);
                if (!cmpInst || cmpInst->operands.size() != 2)
                {
                    // (AR) قيمةٌ منطقيّةٌ حيّةٌ (٠/١) عُرِّفت في هذه الكتلة (نتيجةُ مقارنةِ عوائم أو
                    //      متغيّرٌ منطقيّ): cmp Xcond, xzr؛ b.ne then؛ b else. (العوائم لا تُدمَج عمدًا.)
                    return materialize(a64reg::kScratch0, cond) &&
                           cmp(a64reg::kScratch0, a64reg::kXzr) &&
                           emitBranch(a64::mnem::kBne, "rel19", thenLbl) &&
                           emitBranch(a64::mnem::kB, "rel26", elseLbl);
                }
                // (AR) الفروعُ المُصدَرة موقَّعة؛ معاملٌ لا-موقَّعٌ يلزمه b.lo/ls/hi/hs (لا نظائرَ بعد)
                //      ⇒ رفضٌ صريح بدل ترميزٍ موقَّعٍ خاطئٍ صامت (توصية أميليا).
                for (const auto &cop : cmpInst->operands)
                    if (isUnsignedType(cop.dataType))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    diag::kCmpUnsignedType + std::to_string(static_cast<int>(cop.dataType)));
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
                    // (AR) سياقُ التخصيص: يمكّن allocReg من استرجاعِ سجلٍّ ميّتٍ عند نفاد الحوض.
                    curBlock_ = &block;
                    curInstIdx_ = idx;
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
                floatValSlot_ = -1;
                dynBaseSlot_ = -1;
                dynGetCount_ = 0;
                dynSlotNext_ = 0;
                appendPSlot_ = appendValSlot_ = appendLenSlot_ = appendNewSlot_ = appendCapSlot_ = -1;

                const auto &blocks = fn.getBasicBlocks();
                if (blocks.empty())
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kNoBlocks + currentFn_);
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
                        return fail(EC::INT_NATIVE_IMM_RANGE, diag::kUnaligned + std::to_string(dispBytes));
                    const long long imm = dispBytes / 4; // (AR) الإزاحةُ عددُ تعليماتٍ موقَّع
                    const int width = fx.immHi - fx.immLo + 1;
                    const long long lim = 1LL << (width - 1);
                    if (imm < -lim || imm > lim - 1)
                        return fail(EC::INT_NATIVE_IMM_RANGE, diag::kRel + std::to_string(imm));
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
