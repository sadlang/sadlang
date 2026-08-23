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

#include "frontend/sir_constants.h"
#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

#include "error_codes.h"
#include "error_messages_generated.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <utility>
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
        inline constexpr long long kSysMunmapArm64 = 215; // (AR) munmap — تحريرُ كتلةِ الكومة (FREE)
        // (AR) نداءاتُ نظامِ الملفّات (Linux/AArch64) — مدمَجَتا اكتب_بايتات/اقرأ_بايتات.
        //      AArch64 لا نداءَ open فيه أصلًا (أُسقِط من الـABI الحديث) ⇒ openat بواصفِ
        //      المجلّد AT_FDCWD. هذا فارقٌ حقيقيٌّ عن x86-64 لا اختلافُ رقمٍ فحسب.
        // (EN) Filesystem syscalls (Linux/AArch64). AArch64 has no `open` at all — the
        //      modern ABI dropped it — so openat with AT_FDCWD is used. A real difference
        //      from x86-64, not merely a renumbering.
        inline constexpr long long kSysOpenatArm64 = 56;  // (AR) openat
        inline constexpr long long kSysCloseArm64 = 57;   // (AR) close
        inline constexpr long long kSysLseekArm64 = 62;   // (AR) lseek
        inline constexpr long long kSysReadArm64 = 63;    // (AR) read
        inline constexpr long long kAtFdCwd = -100;       // (AR) AT_FDCWD — المسارُ نسبيٌّ لمجلّد العمل
        inline constexpr long long kFdStdoutArm64 = 1;  // (AR) واصفُ الخرج القياسيّ (stdout)
        inline constexpr long long kItoaRadixArm64 = 10; // (AR) أساسُ التحويل العشريّ
        inline constexpr long long kAsciiZeroArm64 = 0x30; // (AR) رمزُ الصفر ASCII ('0') — أساسُ itoa
        inline constexpr long long kAsciiMinusArm64 = 0x2D; // (AR) رمزُ السالب ASCII ('-') — بادئةُ العدد السالب
        inline constexpr long long kAsciiNineArm64 = 0x39; // (AR) رمزُ التسعة ASCII ('9') — حدُّ التحليل العشريّ الأعلى
        inline constexpr long long kAsciiDotArm64 = 0x2E; // (AR) رمزُ النقطة ASCII ('.') — فاصلةُ العشريّ في عشريّ→نصّ
        inline constexpr long long kUtf8ContMaskArm64 = 0xC0; // (AR) قناعُ بتّي البدايةِ العليَين في UTF-8
        inline constexpr long long kUtf8ContTagArm64 = 0x80;  // (AR) وسمُ البايتِ التابع (10xxxxxx) في UTF-8
        namespace a64reg
        {
            inline constexpr int kX0 = 0;    // (AR) قيمةُ الإرجاع/الوسيط الأوّل (رمزُ الخروج)
            inline constexpr int kX1 = 1;    // (AR) الوسيط الثاني لـsvc (مؤشّر write)
            inline constexpr int kX2 = 2;    // (AR) الوسيط الثالث لـsvc (طولُ write)
            inline constexpr int kX8 = 8;    // (AR) رقمُ النداء (svc)
            inline constexpr int kScratch0 = 16; // (AR) x16 (IP0) مُبدَّدٌ لتجهيز المعامل الأوّل
            inline constexpr int kScratch1 = 17; // (AR) x17 (IP1) مُبدَّدٌ لتجهيز المعامل الثاني
            inline constexpr int kXzr = 31;      // (AR) السجلّ الصفريّ XZR في موضعِ Rm/Rn الحسابيّ (لا SP)
            inline constexpr int kSp = 31;       // (AR) مؤشّرُ المكدّس SP — الرقمُ ٣١ في عنونةِ add/sub الفوريّة (لا XZR)
            inline constexpr int kLr = 30;       // (AR) سجلّ الرابط x30 (عنوانُ العودة) — يُحفَظ عبر النداء (الدفعة ٦)
            inline constexpr long long kImm16Max = 0xFFFF; // (AR) أقصى فوريّ لـMOVZ (بلا MOVK)
            inline constexpr long long kImm12Max = 0xFFF;  // (AR) أقصى فوريّ لـADD/SUB (حقلُ imm12)
        } // namespace a64reg

        // (AR) مخفّضُ SIR→AArch64 (نطاقُ تكافؤ x86 الكامل). صنفٌ مستقلٌّ عن X86SirLowering
        //      عمدًا: يبقى مسارُ x86 المُبرهَنُ سليمًا بلا مساس، والمشتركُ لاحقًا يُستخرَج
        //      خلف واجهةٍ حين ينضج المساران (تجريدٌ بعد برهانٍ لا قبله).
        // (AR) يرث القاعدةَ التشخيصيّةَ المشتركة (م٤): حالةُ الفشل وعقدُ الشكل
        //      عقدٌ واحدٌ عبر الأهداف لا نسخةٌ لكلّ معماريّة.
        class Arm64SirLowering : public common::LoweringDriver<Arm64SirLowering>
        {
            friend class common::LoweringDriver<Arm64SirLowering>;

        public:
            LoweringResult lowerModule(const sir::SIRModule &module)
            {
                LoweringResult r;
                const sir::SIRFunction *entry = findEntry(module);
                if (!entry)
                    return finishError(r, EC::INT_NATIVE_NO_ENTRY);
                entryName_ = entry->getName();

                // (AR) ابنِ سجلَّ تخطيطِ الأصناف مرّةً (الكائنيّة، مرآةُ x86).
                classLayout_.clear();
                for (const auto &c : module.getClasses())
                {
                    if (!c)
                        continue;
                    ClassLayout cl;
                    cl.isCRepr = c->isCRepr;
                    int idx = 0;
                    for (const auto &fname : c->fieldOrder_)
                    {
                        cl.fieldIndex[fname] = idx++;
                        auto ft = c->fields_.find(fname);
                        const types::SadTypeKind t =
                            ft != c->fields_.end() ? ft->second : types::SadTypeKind::Integer;
                        // (AR) الدفعة ٨ (مرآةُ x86): حقلُ bool في صنفٍ غير-CRepr = ٠/١ في خانةِ ٨-بت
                        //      ⇒ مسموح؛ يُرفَض في CRepr (ABI بايت) أو حقلُ Any (SadDyn ١٦-بت).
                        if (t == types::SadTypeKind::Any ||
                            (t == types::SadTypeKind::Boolean && c->isCRepr))
                            cl.allEightByte = false;
                        // (AR) حقلُ مصفوفةٍ (مُهيّأ بـ[]): سجّلْ إزاحتَه ليُخصَّص له SadArray فارغٌ عند ALLOC (مرآةُ x86).
                        if (c->isArrayField(fname))
                            cl.arrayFieldOffsets.push_back(8LL * ((idx - 1) + (c->isCRepr ? 0 : 1)));
                    }
                    cl.numFields = idx;
                    classLayout_[c->name] = cl;
                }

                // (AR) ابنِ جداولَ الدوالّ (vtables) + احجزْ خاناتِها في مقدّمةِ rodata_. (الدفعة ٦)
                buildVtableLayouts(module);

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
                if (!strFixups_.empty() || !fnPtrFixups_.empty() ||
                    !vtableAddrFixups_.empty() || !funcAddrFixups_.empty() || !rodata_.empty())
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
                    // (AR) خاناتُ جدولِ الدوالّ (بياناتٌ في rodata): عنوانُ الدالّةِ المطلق ٨-بت خامًّا.
                    for (const FnPtrFixup &ff : fnPtrFixups_)
                    {
                        auto it = funcOffset_.find(ff.fnName);
                        if (it == funcOffset_.end())
                            return finishError(r, EC::INT_NATIVE_UNSUPPORTED,
                                               diag::kVtableUnresolvedFn + ff.fnName);
                        const uint64_t fnAddr = elf::kDefaultVBase + elf::kCodeOffset +
                                                static_cast<uint64_t>(it->second);
                        const size_t at = rodataBase + ff.rodataOff;
                        for (int i = 0; i < 8; ++i)
                            code_[at + i] = static_cast<uint8_t>((fnAddr >> (8 * i)) & 0xFF);
                    }
                    // (AR) عناوينُ الجداولِ في تسلسلِ movz+movk×3 داخلَ ALLOC-صنف (كـStrFixup).
                    for (const VtableAddrFixup &vf : vtableAddrFixups_)
                    {
                        const uint64_t vtAddr = elf::kDefaultVBase + elf::kCodeOffset +
                                                static_cast<uint64_t>(rodataBase + vtableBaseOff_[vf.className]);
                        for (int chunk = 0; chunk < 4; ++chunk)
                        {
                            const uint32_t imm16 = static_cast<uint32_t>((vtAddr >> (16 * chunk)) & 0xFFFF);
                            patchImm16At(vf.movStart + static_cast<size_t>(chunk) * 4, imm16);
                        }
                    }
                    // (AR) عناوينُ الدوالّ في تسلسلِ movz+movk×3 داخلَ CLOSURE_CREATE (الدفعة ٧).
                    for (const FuncAddrFixup &af : funcAddrFixups_)
                    {
                        auto it = funcOffset_.find(af.fnName);
                        if (it == funcOffset_.end())
                            return finishError(r, EC::INT_NATIVE_UNSUPPORTED,
                                               diag::kFuncAddrUnresolved + af.fnName);
                        const uint64_t fnAddr = elf::kDefaultVBase + elf::kCodeOffset +
                                                static_cast<uint64_t>(it->second);
                        for (int chunk = 0; chunk < 4; ++chunk)
                        {
                            const uint32_t imm16 = static_cast<uint32_t>((fnAddr >> (16 * chunk)) & 0xFFFF);
                            patchImm16At(af.movStart + static_cast<size_t>(chunk) * 4, imm16);
                        }
                    }
                }

                r.ok = true;
                r.code = std::move(code_);
                return r;
            }

        private:
            std::map<std::string, int> regOf_;                       // (AR) سجلّ افتراضيّ ⇒ فيزيائيّ
            const std::vector<int> pool_{9, 10, 11, 12, 13, 14, 15}; // (AR) x9..x15 (caller-saved)
            size_t next_ = 0;
            // (AR) سياقُ التخفيض الحاليّ (يُضبَط في lowerBlock): يمكّن allocReg من استرجاعِ سجلِّ
            //      مؤقّتٍ ميّتٍ عند نفاد الحوض (لازمٌ للمصفوفات كثيرةِ المؤقّتات). نظيرُ x86.
            const sir::SIRBasicBlock *curBlock_ = nullptr;
            // (AR) عددُ الإسناداتِ المفهرَسةِ للخرائطِ في الدالّةِ الجارية (سعةٌ ساكنة).
            long long mapSetCount_ = 0;
            size_t curInstIdx_ = 0;
            std::vector<uint8_t> code_;

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
            // (AR) تخطيطُ الأصناف + استنتاجُ صنفِ الكائن (الدفعة ٥، مرآةُ x86): إزاحةُ الحقل = 8×(الفهرس +
            //      (isCRepr؟0:1))؛ حقولٌ ٨-بت فقط. objClassOf_ يُصفَّر لكلّ دالّة. classLayout_ يُبنى مرّةً.
            struct ClassLayout { bool isCRepr = false; std::map<std::string, int> fieldIndex; int numFields = 0; bool allEightByte = true; std::vector<long long> arrayFieldOffsets; };
            std::map<std::string, ClassLayout> classLayout_;
            std::map<std::string, std::string> objClassOf_;
            bool objFieldByteOffset(const std::string &className, const std::string &field, long long &off) const
            {
                auto ci = classLayout_.find(className);
                if (ci == classLayout_.end() || !ci->second.allEightByte)
                    return false;
                auto fi = ci->second.fieldIndex.find(field);
                if (fi == ci->second.fieldIndex.end())
                    return false;
                off = 8LL * (fi->second + (ci->second.isCRepr ? 0 : 1));
                return true;
            }
            const std::string *objClassForOperand(const sir::SIROperand &op) const
            {
                auto it = objClassOf_.find(op.name);
                return it != objClassOf_.end() ? &it->second : nullptr;
            }
            // (AR) حوافُ PHI: اسمُ الكتلةِ السَّلَف ⇒ (معاملُ القيمةِ الوارد، اسمُ ناتجِ PHI). الحوضُ
            //      النظيفُ لكلّ كتلةٍ لا يحمل قيمةً عابرة ⇒ نحمِلها بخانةِ إطارٍ (السَّلَفُ يخزّن، الدامجُ يقرأ).
            std::map<std::string, std::vector<std::pair<sir::SIROperand, std::string>>> phiEdges_;
            // (AR) قيمٌ تُعرَّف في كتلةٍ وتُستعمَل في أخرى (عدا نتائجَ PHI): تُنسَك عند تعريفها في خانةِ إطارٍ
            //      (strSlot) فتُقرأ memSlot_-أوّلًا عبر الحدّ ⇒ يُمكّن لولبَ المدى الديناميّ.
            std::set<std::string> crossBlockSpill_;
            long long frameSize_ = 0; // (AR) بايتات (مُحاذاةُ ١٦)
            // (AR) انسكابُ وسائطِ النداء (سدُّ اختلال x86≠ARM64): AAPCS64 يمرّر أوّلَ ٨ وسائطَ في
            //      x0..x7، والزائدُ على المكدّس. نحجز منطقةَ «وسائطٍ صادرةٍ» في **أسفلِ** الإطار
            //      (الخاناتُ 0..n-1 = [sp+i×8])، ونُزيح بقيّةَ الخانات فوقها؛ فلا يتحرّك sp وسطَ الجسم.
            //      المُستدعى يقرأُ معاملاتِه ٨+ من [sp + frameSize_ + …] (BL لا يدفع للمكدّس). مرآةٌ لـx86.
            int maxOutgoingStackArgs_ = 0;

            // (AR) الطباعة: فهرسُ قمّةِ مخزنِ itoa (العنوانُ الأعلى، حصريّ) في الإطار؛ الأرقامُ
            //      تُبنى تنازليًّا منه. الانسكابُ عبر النداء/الطبع: فهرسُ أوّلِ خانةٍ لمنطقةِ انسكابِ
            //      سجلّات الحوض (خانةٌ لكلّ سجلّ). النداء/الطبعُ يدهسان الحوضَ (caller-saved).
            int printBufTopSlot_ = -1;
            int spillBaseSlot_ = -1;
            // (AR) خانةُ حفظِ سجلّ الرابط x30 (الدفعة ٦): الدالّةُ غيرُ الداخلةِ التي تنادي (bl/blr
            //      يدهسان x30) تحفظه في المقدّمة وتستعيده قبل RET. صفرٌ سالبٌ ⇒ لا حفظ (ورقة/داخلة).
            int lrSlot_ = -1;
            // (AR) خانةُ حفظِ المقسوم a لتصحيحِ **أرضيّةِ** القسمةِ الموقَّعةِ السالبة (sdiv يقتطع؛
            //      المرجعُ يُصحّح ‑7//2=‑4). لا يلزمُ حارسُ b==-1 هنا: sdiv لا يفخّ على INT64_MIN/‑1.
            int floorDivDividendSlot_ = -1;
            // (AR) طباعةُ العشريّ: خانةُ خدشٍ لنمطِ بتّاتِ الـdouble (القيمةِ المطلقة) عبر مراحل المُنسِّق.
            int floatValSlot_ = -1;
            // (AR) مصفوفةُ العشريّ→نصّ: قمّةُ مخزنِ خدشٍ (kFtoaBufPayload/8 خانات) لبناءِ نصِّ كلِّ عنصرٍ
            //      عشريّ تنازليًّا قبل byteCopy — أوسعُ من مخزنِ itoa؛ لا mmap بين التفكيك والبناء.
            int atsFtoaTopSlot_ = -1;
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

            // (AR) عمليّاتُ كومةِ النصّ (الدفعة ٣): فهرسُ أوّلِ خانةٍ من kStrHeapSlots تبقى حيّةً عبر
            //      mmap الداخليّ (طول/مخزن/عدّاد). الخانةُ i عند sp + (strHeapBaseSlot_+i)×8.
            int strHeapBaseSlot_ = -1;

            // (AR) امتدادُ المصفوفة (CONCAT/ZIP، الدفعة ٤): ٦ خاناتِ خدشٍ تبقى حيّةً عبر mmap الداخليّ
            //      (أطوالٌ/مؤشّراتُ بياناتٍ/عدّادُ لولبٍ/مؤشّرُ الناتج). الخانةُ i عند sp + (arrExtBaseSlot_+i)×8.
            static constexpr int kArrExtSlots = 6;
            int arrExtBaseSlot_ = -1;
            long long arrExtSlot(int i) const { return static_cast<long long>(arrExtBaseSlot_) + i; }

            // (AR) خاناتُ خدشِ بايتاتِ الملفّات (مرآةُ x86): fd/الطول/المخزن/البيانات/العدّاد/البنية.
            //      قيمٌ يجب أن تبقى حيّةً عبر نداءاتِ النظام وmmap، وكلاهما يدهس الحوضَ بأكمله.
            static constexpr int kFileBytesSlots = 6;
            int fileBytesBaseSlot_ = -1;
            long long fileBytesSlot(int i) const { return static_cast<long long>(fileBytesBaseSlot_) + i; }

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

            // ── الإرسالُ الافتراضيّ (الدفعة ٦، مرآةُ x86): جداولُ الدوالّ (vtables) ──────────
            std::map<std::string, std::vector<std::pair<std::string, std::string>>> classVtableLayout_;
            std::map<std::string, size_t> vtableBaseOff_;
            struct FnPtrFixup { size_t rodataOff; std::string fnName; };
            std::vector<FnPtrFixup> fnPtrFixups_;
            // (AR) عنوانُ الجدول يُرقَّع في تسلسلِ movz+movk×3 (نظيرُ StrFixup): موضعُ الكلمةِ الأولى + الصنف.
            struct VtableAddrFixup { size_t movStart; std::string className; };
            std::vector<VtableAddrFixup> vtableAddrFixups_;
            // (AR) الإغلاقات (الدفعة ٧، مرآةُ x86): عنوانُ دالّةٍ يُرقَّع في تسلسلِ movz+movk×3 داخلَ
            //      CLOSURE_CREATE ⇒ يُملأ بعنوانِ الدالّةِ المطلق (يُحلُّ من funcOffset_).
            struct FuncAddrFixup { size_t movStart; std::string fnName; };
            std::vector<FuncAddrFixup> funcAddrFixups_;

            // (AR) يبني تخطيطاتِ جداولِ الدوالّ (بترتيبٍ أبٌ-قبل-ابن: نسخُ الأب + تجاوزٌ في المكان
            //      أو إلحاق) ثمّ يحجزُ خاناتِها في مقدّمةِ rodata_. مرآةٌ حرفيّةٌ لـx86. (الدفعة ٦)
            void buildVtableLayouts(const sir::SIRModule &module)
            {
                classVtableLayout_.clear();
                vtableBaseOff_.clear();
                fnPtrFixups_.clear();
                vtableAddrFixups_.clear();
                funcAddrFixups_.clear();
                std::map<std::string, const sir::SIRClass *> byName;
                for (const auto &c : module.getClasses())
                    if (c && !c->isCRepr)
                        byName[c->name] = c.get();
                std::set<std::string> done;
                std::vector<const sir::SIRClass *> order;
                bool progress = true;
                while (progress)
                {
                    progress = false;
                    for (const auto &kv : byName)
                    {
                        if (done.count(kv.first))
                            continue;
                        const sir::SIRClass *c = kv.second;
                        const bool parentReady =
                            c->parentClass.empty() || !byName.count(c->parentClass) ||
                            done.count(c->parentClass);
                        if (!parentReady)
                            continue;
                        order.push_back(c);
                        done.insert(kv.first);
                        progress = true;
                    }
                }
                for (const sir::SIRClass *c : order)
                {
                    std::vector<std::pair<std::string, std::string>> layout;
                    if (byName.count(c->parentClass) && classVtableLayout_.count(c->parentClass))
                        layout = classVtableLayout_[c->parentClass];
                    std::vector<std::string> qualNames;
                    for (const auto &m : c->methods_)
                        if (m.second)
                            qualNames.push_back(m.first);
                    std::sort(qualNames.begin(), qualNames.end());
                    for (const std::string &qual : qualNames)
                    {
                        const std::string shortName = shortMethodName(qual);
                        if (isCtorOrDtorName(shortName))
                            continue;
                        bool overridden = false;
                        for (auto &slot : layout)
                            if (slot.first == shortName) { slot.second = qual; overridden = true; break; }
                        if (!overridden)
                            layout.emplace_back(shortName, qual);
                    }
                    classVtableLayout_[c->name] = layout;
                }
                for (const sir::SIRClass *c : order)
                {
                    vtableBaseOff_[c->name] = rodata_.size();
                    for (const auto &slot : classVtableLayout_[c->name])
                    {
                        fnPtrFixups_.push_back({rodata_.size(), slot.second});
                        rodata_.insert(rodata_.end(), 8, 0);
                    }
                }
            }
            static std::string shortMethodName(const std::string &qualified)
            {
                const auto dot = qualified.rfind('.');
                return dot == std::string::npos ? qualified : qualified.substr(dot + 1);
            }
            // (AR) البانِي والهادمُ يُكشفانِ بنيويًّا: خانتاهما المفكوكتانِ في
            //      الفضاءِ الداخليِّ `#` (kConstructorSlotName/kDestructorSlotName) —
            //      لا بقائمةِ تهجئاتٍ (بناء/باني/منشئ/__init__/هدم/__del__) كانت
            //      تُقصي طرائقَ مستخدمٍ عاديّةً بهذه الأسماءِ من التخطيط.
            // (EN) Constructor and destructor are both detected structurally: their
            //      mangled slots live in the internal `#` namespace — not via a
            //      spelling list (بناء/باني/منشئ/__init__/هدم/__del__) that also
            //      evicted ordinary user methods from the layout.
            static bool isCtorOrDtorName(const std::string &n)
            {
                return ::Sad::Compiler::startsWithPrefix(
                    n.c_str(), ::Sad::Compiler::kSlotNamespaceSeparator);
            }
            // (AR) يبني عنوانَ جدولِ دوالِّ صنفٍ ٦٤-بت في سجلّ (movz+movk×3 نائبٌ) ويسجّل ترقيعًا.
            bool emitVtableAddr(int reg, const std::string &className)
            {
                const size_t start = code_.size();
                if (!emit(a64::mnem::kMovz, "x, imm16", {a64::Operand::R(reg), a64::Operand::I(0)}) ||
                    !movk(reg, 0, 1) || !movk(reg, 0, 2) || !movk(reg, 0, 3))
                    return false;
                vtableAddrFixups_.push_back({start, className});
                return true;
            }
            // (AR) نداءٌ غيرُ مباشرٍ عبر سجلّ (blr Xn) — للإرسالِ الافتراضيّ (مؤشّرُ الدالّةِ من vtable).
            bool emitBlr(int reg)
            {
                return emit(a64::mnem::kBlr, "Xn", {a64::Operand::R(reg)});
            }
            // (AR) الإغلاقات (الدفعة ٧): يبني عنوانَ دالّةٍ ٦٤-بت في سجلّ (movz+movk×3 نائبٌ) ويسجّل
            //      ترقيعًا يُحلُّ من funcOffset_ زمنَ الإنهاء. مرآةُ emitVtableAddr.
            bool emitFuncAddr(int reg, const std::string &fnName)
            {
                const size_t start = code_.size();
                if (!emit(a64::mnem::kMovz, "x, imm16", {a64::Operand::R(reg), a64::Operand::I(0)}) ||
                    !movk(reg, 0, 1) || !movk(reg, 0, 2) || !movk(reg, 0, 3))
                    return false;
                funcAddrFixups_.push_back({start, fnName});
                return true;
            }

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
            bool ldrb(int wt, int xn) // (AR) ldrb Wt, [Xn] — تحميلُ بايتٍ واحد (لحلقةِ نسخ الذاكرة)
            {
                return emit(a64::mnem::kLdrb, "w, x", {a64::Operand::R(wt), a64::Operand::R(xn)});
            }
            // (AR) طولُ سلسلةٍ بالبايت (لا نقاطِ رمز — لنسخِ CONCAT/SUBSTR الخام): يمشي ptrReg حتّى NUL
            //      عادًّا في lenReg (يدهس ptrReg/lenReg/byteReg). ldrb يمدّد بالصفر ذاتيًّا (لا movzx).
            bool byteStrlen(int ptrReg, int lenReg, int byteReg)
            {
                if (!movz(lenReg, 0))
                    return false;
                const size_t head = code_.size();
                if (!ldrb(byteReg, ptrReg) || !cmp(byteReg, a64reg::kXzr))
                    return false;
                size_t done;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", done))
                    return false;
                if (!addImm(lenReg, lenReg, 1) || !addImm(ptrReg, ptrReg, 1) || !emitBBack(head))
                    return false;
                return patchBranchFwd(done, 23, 5);
            }
            // (AR) نسخُ cntReg بايتًا من srcReg إلى dstReg (كلاهما يتقدّم) بفحصِ العدّ في الرأس. dstReg
            //      يبقى عند نهايةِ النسخ (لسلسلةِ نسخات CONCAT). البايتُ عبر byteReg.
            bool byteCopy(int dstReg, int srcReg, int cntReg, int byteReg)
            {
                const size_t head = code_.size();
                if (!cmp(cntReg, a64reg::kXzr))
                    return false;
                size_t done;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", done))
                    return false;
                if (!ldrb(byteReg, srcReg) || !strb(byteReg, dstReg) || !addImm(dstReg, dstReg, 1) ||
                    !addImm(srcReg, srcReg, 1) || !subImm(cntReg, cntReg, 1) || !emitBBack(head))
                    return false;
                return patchBranchFwd(done, 23, 5);
            }
            // (AR) يقدّم ptrReg متجاوزًا nReg نقطةَ رمزِ UTF-8 (أو حتّى NUL — قصٌّ ضمنيّ). نقطةُ الرمز =
            //      بايتٌ بادئٌ ثمّ تابعوه [0x80,0xC0). byteReg خدشُ البايت، x17 خدشُ ثابتِ الحدّ. ldrb
            //      يمدّد بالصفر ذاتيًّا. يدهس ptr/n/byteReg/x17.
            bool skipCodepoints(int ptrReg, int nReg, int byteReg)
            {
                const size_t headN = code_.size();
                if (!cmp(nReg, a64reg::kXzr))
                    return false;
                size_t doneN;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", doneN))
                    return false;
                if (!ldrb(byteReg, ptrReg) || !cmp(byteReg, a64reg::kXzr))
                    return false;
                size_t doneNul;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", doneNul))
                    return false;
                if (!addImm(ptrReg, ptrReg, 1)) // (AR) تجاوزِ البايتَ البادئ
                    return false;
                const size_t headC = code_.size(); // (AR) تجاوزِ التابعين [0x80, 0xC0)
                if (!ldrb(byteReg, ptrReg) || !movz(a64reg::kScratch1, kUtf8ContTagArm64) ||
                    !cmp(byteReg, a64reg::kScratch1))
                    return false;
                size_t notContLo;
                if (!emitBranchFwd(a64::mnem::kBlt, "rel19", notContLo)) // < 0x80 ⇒ بادئ
                    return false;
                if (!movz(a64reg::kScratch1, kUtf8ContMaskArm64) || !cmp(byteReg, a64reg::kScratch1))
                    return false;
                size_t notContHi;
                if (!emitBranchFwd(a64::mnem::kBge, "rel19", notContHi)) // ≥ 0xC0 ⇒ بادئ
                    return false;
                if (!addImm(ptrReg, ptrReg, 1) || !emitBBack(headC))
                    return false;
                if (!patchBranchFwd(notContLo, 23, 5) || !patchBranchFwd(notContHi, 23, 5))
                    return false;
                if (!subImm(nReg, nReg, 1) || !emitBBack(headN))
                    return false;
                return patchBranchFwd(doneN, 23, 5) && patchBranchFwd(doneNul, 23, 5);
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
            static constexpr int kD2 = 2; // (AR) خدشٌ ٢: متراكمُ نصّ→عشريّ **ومُبدَّدُ**
                                          //      loadFloatOperandInto (مفصولٌ عن d0/d1 عمدًا)
            static constexpr int kD3 = 3; // (AR) سجلّ عشريّ خدشٌ ٣ (ثابتُ العشرة / مقسومُ الكسر)
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
            bool fsqrt(int d, int n) { return emit(a64::mnem::kFsqrt, "d, d", {a64::Operand::R(d), a64::Operand::R(n)}); } // (AR) d = √n (عشريّ مزدوج)
            bool cselLt(int d, int n, int m) { return emit(a64::mnem::kCselLt, "x, x, x", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); } // (AR) d = (n<m موقَّعًا بعدَ cmp)؟ n : m
            bool cselGt(int d, int n, int m) { return emit(a64::mnem::kCselGt, "x, x, x", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); } // (AR) d = (n>m)؟ n : m
            bool cselHi(int d, int n, int m) { return emit(a64::mnem::kCselHi, "x, x, x", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); } // (AR) d = (n>m لا-موقَّعًا HI بعدَ cmp)؟ n : m
            bool cselLo(int d, int n, int m) { return emit(a64::mnem::kCselLo, "x, x, x", {a64::Operand::R(d), a64::Operand::R(n), a64::Operand::R(m)}); } // (AR) d = (n<m لا-موقَّعًا LO بعدَ cmp)؟ n : m
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
            // (AR) يحمّل ثابتًا صحيحًا في سجلّ: [0, 2¹⁶) بـmovz واحدةٍ (مُوجَز)؛ السالبُ أو الأكبرُ
            //      (كخطوةِ لولبٍ ديناميٍّ −1 = 0xFFFF…FFFF) عبر نمطِ البتّات الكامل movz+movk×3 ⇒ لا بترَ إشارة.
            bool movConst(int reg, long long imm)
            {
                if (imm >= 0 && imm <= a64reg::kImm16Max)
                    return movz(reg, imm);
                return movImm64Bits(reg, static_cast<unsigned long long>(imm));
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
            // (AR) حارسُ القسمةِ على صفر: المقسومُ عليه في kScratch1 (x17). إن كان صفرًا ⇒ إجهاضٌ
            //      exit(136). sdiv/udiv لا يفخّان (يُرجعان صفرًا) ⇒ بلا الحارسِ تكونُ النتيجةُ صامتةً
            //      خاطئة تخالفُ المرجعَ (يرمي خطأً) وتخالفُ x86 (يُجهِض) ⇒ الحارسُ يُحقّقُ التماثلَ.
            bool emitDivZeroGuardArm64()
            {
                if (!cmp(a64reg::kScratch1, a64reg::kXzr)) // divisor vs 0
                    return false;
                size_t skip;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", skip)) // ≠0 ⇒ تخطّي كتلةِ الهلع
                    return false;
                if (!movz(a64reg::kX0, kDivZeroPanicCode) || !movz(a64reg::kX8, kSysExitArm64) ||
                    !emit(a64::mnem::kSvc, "", {})) // (AR) exit(136) — لا عودة
                    return false;
                return patchBranchFwd(skip, 23, 5);
            }
            // (AR) حارسُ المُستقبِلِ العدميّ (مرآةُ `emitNullReceiverGuard` في x86): المُستقبِلُ في
            //      x0. صفرٌ ⇒ exit(132)، وإلّا نتخطّى كتلةَ الهلعِ ويمضي النداءُ الذي يليه.
            //      الرمزُ نفسُه على المعماريّتَين عمدًا: سببُ الإجهاضِ يُقرأ من الخروجِ وحدَه،
            //      فلو افترق الرمزان لصار الهدفُ جزءًا من الجواب.
            bool emitNullReceiverGuardArm64()
            {
                if (!cmp(a64reg::kX0, a64reg::kXzr)) // (AR) المُستقبِل مقابلَ صفر
                    return false;
                size_t skip;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", skip)) // (AR) ≠0 ⇒ تخطّي كتلةِ الهلع
                    return false;
                if (!movz(a64reg::kX0, kNullReceiverPanicCode) || !movz(a64reg::kX8, kSysExitArm64) ||
                    !emit(a64::mnem::kSvc, "", {})) // (AR) exit(132) — لا عودة
                    return false;
                return patchBranchFwd(skip, 23, 5);
            }
            // (AR) [عقدُ Any] يُعلِّب قيمةً قياسيّة في خانتَي dyn ({tag، payload}) ويُعيد مؤشّرَها
            //      (sp + الفهرس×٨) في dst — مرآةُ تعليبِ ARRAY_GET(Any)/x86 boxScalarInto. الخانةُ
            //      محجوزةٌ سلفًا في assignFrameSlots (dynGetCount_). payloadReg يُخزَّن قبل addImm.
            // (AR) 🛡️ المُخصِّصُ الوحيدُ لخانةِ dyn — مرآةُ `takeDynSlot` في x86 (الحجّةُ والقياسُ
            //      والعطبُ الذي استدعاه مفصَّلةٌ هناك): الحجزُ والاستهلاكُ جدولان منفصلان، وتجاوزُ
            //      الحجزِ فشلُ ترجمةٍ صريحٌ لا دهسُ خانةٍ جارة.
            bool takeDynSlot(int &outSlot)
            {
                if (dynSlotNext_ >= dynGetCount_)
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                diag::kDynSlotOverrun + std::to_string(dynSlotNext_ + 1) +
                                    diag::kSlotOfReserved + std::to_string(dynGetCount_));
                outSlot = dynBaseSlot_ + dynSlotNext_ * 2;
                ++dynSlotNext_;
                return true;
            }
            bool boxScalarIntoArm64(int dst, int tagReg, int payloadReg)
            {
                int ts = 0;
                if (!takeDynSlot(ts))
                    return false;
                return strSlot(tagReg, ts) && strSlot(payloadReg, ts + 1) &&
                       addImm(dst, 31, static_cast<long long>(ts) * 8); // dst = sp + ts×8
            }
            // (AR) يُعيد تعليبَ نتيجةٍ حسابيّةٍ خامٍّ في dst إن كان نوعُها Any (سلسلةُ Any): وسمُ Int،
            //      الحمولةُ = dst. يستعمل kScratch0 للوسم. غيرُ Any ⇒ لا شيء (يبقى dst خامًا).
            bool boxIfAnyArm64(const sir::SIRInstruction &inst, int dst)
            {
                if (!inst.result || inst.result->dataType != types::SadTypeKind::Any)
                    return true;
                return movz(a64reg::kScratch0, kDynKindInt) &&
                       boxScalarIntoArm64(dst, a64reg::kScratch0, dst);
            }
            // (AR) يحمّل معاملًا **قياسيًّا** للحساب: Any (مؤشّرُ خانةِ dyn معلَّب) ⇒ فُكَّ الحمولةَ
            //      ([ptr+payloadOff]) — الحسابُ يريد القيمةَ الخام. غيرُه ⇒ materialize المعتاد.
            //      حصرًا في مسارِ العمليّة الثنائيّة (لا MOVE/STORE/الطباعة التي تُبقي المؤشّر).
            bool loadScalarInto(int dst, const sir::SIROperand &op)
            {
                if (common::isBoxedAny(op))
                    return materialize(dst, op) && ldrBase(dst, dst, kSadDynPayloadOff / kArrSlotBytes);
                return materialize(dst, op);
            }

            // (AR) تحميلُ معامِلٍ **لعمليّةٍ عشريّة**: الخارجُ في dst بتّاتُ double لكلّ
            //      معامِلٍ معلَّب (لا «دائمًا» — انظر قيدَ المعامِلِ غيرِ المعلَّبِ في x86).
            //      مرآةُ `loadFloatOperandInto` في x86 (التفصيلُ والشاهدُ الحيُّ هناك):
            //      الحمولةُ لا تكون بتّاتِ double إلّا إذا كان الوسمُ Float، والوسمُ لا
            //      يُعرَف إلّا زمنَ التشغيل ⇒ الفرعُ في الشيفرةِ المولَّدة.
            //      x8 وسمًا: خارجَ الحوض (x9..x15) وخارجَ سجلّاتِ الوسائط (x0..x7)، ولا
            //      نداءَ نظامٍ في هذا التتابع ⇒ لا قيمةَ حيّةٌ فيه.
            //      ومُبدَّدُه العشريُّ **kD2 لا kD0**: المُنادي قد يكون ركن معامِلًا سابقًا في
            //      d0/d1، والفرعُ يُصدَر بحسبِ وسمٍ لا يُعرَف إلّا زمنَ التشغيل ⇒ دهسٌ
            //      متقطّعٌ لولا الفصل (نظيرُ عقدِ kXmm2 في x86).
            //      ⚠️ ولا `cmp Xn, #imm` في جداولِ AArch64 (المقارنةُ سجلٌّ بسجلّ)، وسجلّا
            //         الخدشِ محجوزان للمعامِلَين ⇒ `sub x8, x8, #Float` ثمّ `cmp x8, xzr`:
            //         مقارنةٌ بالصفرِ بلا سجلٍّ ثالث ولا صفٍّ جديدٍ في مصدرِ الحقيقة.
            bool loadFloatOperandInto(int dst, const sir::SIROperand &op)
            {
                if (!common::isBoxedAny(op))
                    return materialize(dst, op);
                if (!materialize(dst, op) ||
                    !ldrBase(a64reg::kX8, dst, kSadDynKindOff / kArrSlotBytes) ||
                    !ldrBase(dst, dst, kSadDynPayloadOff / kArrSlotBytes) ||
                    !subImm(a64reg::kX8, a64reg::kX8, kDynKindFloat) ||
                    !cmp(a64reg::kX8, a64reg::kXzr))
                    return false;
                size_t isFloat;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", isFloat))
                    return false;
                if (!scvtf(kD2, dst) || !fmovFromFp(dst, kD2))
                    return false;
                return patchBranchFwd(isFloat, 23, 5);
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
            // ── تمثيلُ قيمةِ النصّ زمنَ التشغيل (الدفعة ٣): مؤشّرٌ إلى بايتات UTF-8 منتهيةٍ بـNUL ──
            //    (يطابق تمثيلَ خلفيّة LLVM ومخفّضَ x86). الحرفيّةُ تُفرَّد في rodata منتهيةً بـNUL؛
            //    النصُّ المحسوب مؤشّرُ كومةٍ من ALLOC_HEAP. مميَّزٌ عن الواصفِ الطوليّ (التعليب فقط).
            size_t internCStr(const std::string &s)
            {
                std::string z = s;
                z.push_back('\0'); // (AR) خاتمةُ NUL (بايتاتٌ مغايرةٌ لسلاسلِ الطباعة الخام ⇒ لا تصادم)
                return internString(z);
            }
            // (AR) يحمّل العنوانَ المطلق لسلسلةٍ منتهيةٍ بـNUL في سجلّ (movz+movk×3 يُرقَّع كأيّ سلسلة).
            bool emitLoadCStrAddr(int reg, const std::string &content)
            {
                return emitStringAddr(reg, internCStr(content));
            }
            // (AR) يُجسّد قيمةَ نصٍّ في السجلّ reg مؤشّرًا زمنَ تشغيلٍ منتهيًا بـNUL: حرفيّةٌ (ثابتٌ نصّيّ
            //      أو سجلٌّ شبحيٌّ في strReg_) ⇒ عنوانُ rodata؛ سجلٌّ حيٌّ (مؤشّرُ كومةٍ سابق) ⇒ تحميلٌ
            //      مباشر. fromSpill: داخلَ منطقةِ انسكابٍ يُقرأ السجلُّ الحيُّ من خانتِه (loadArgInto).
            bool materializeString(const sir::SIROperand &op, int reg, bool fromSpill)
            {
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::String)
                    return emitLoadCStrAddr(reg, op.name);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    auto it = strReg_.find(op.name);
                    if (it != strReg_.end())
                        return emitLoadCStrAddr(reg, it->second); // (AR) حرفيّةٌ شبحيّة
                    return fromSpill ? loadArgInto(reg, op) : materialize(reg, op);
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            diag::kOperandKind + std::to_string(static_cast<int>(op.type)));
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

            // (AR) يطبع نصًّا محسوبًا زمنَ التشغيل (مؤشّرٌ منتهٍ بـNUL في `ptrReg`) — مرآةُ
            //      x86 (الحجّةُ هناك): حلقةُ بايتاتٍ تحسب الطولَ ثمّ write.
            //      ⚠️ قيدُ `ptrReg` كما في x86: يُقرأ بعدَ الحلقةِ التي تدهس x1/x2/x17،
            //         فلا يكون منها. المُنادي الوحيدُ اليومَ يمرّر x9.
            // (EN) Print a runtime-computed NUL-terminated string — mirror of x86.
            bool emitPrintStrPtrArm64(int ptrReg)
            {
                if (!movReg(a64reg::kX1, ptrReg) || !movz(a64reg::kX2, 0))
                    return false;
                const size_t head = code_.size();
                if (!ldrb(a64reg::kScratch1, a64reg::kX1) || !cmp(a64reg::kScratch1, a64reg::kXzr))
                    return false;
                size_t done;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", done))
                    return false;
                if (!addImm(a64reg::kX1, a64reg::kX1, 1) || !addImm(a64reg::kX2, a64reg::kX2, 1) ||
                    !emitBBack(head))
                    return false;
                if (!patchBranchFwd(done, 23, 5))
                    return false;
                return movReg(a64reg::kX1, ptrReg) && // (AR) أعِد المؤشّرَ لبدايتِه (x2=الطول)
                       movz(a64reg::kX0, kFdStdoutArm64) && movz(a64reg::kX8, kSysWriteArm64) &&
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

            // (AR) يطبع عددًا **لا-موقَّعًا** (طبيعي64، في x9): itoa عبر udiv/msub بلا فحصِ إشارةٍ
            //      ولا بادئةِ '-' فيُعطي «18446744073709551615» لا «-1». مرآةُ x86 emitPrintUInt
            //      ويطابقُ نصَّ المفسّرِ لـطبيعي64. x9=القيمة، x10=١٠، x13=المؤشّر، x14=القمّة،
            //      x11=الحاصل، x12=الباقي. لولبٌ واحدٌ (cbnz) يُنفَّذُ مرّةً على الأقلّ فيطبع '0' للصفر.
            bool emitPrintUInt()
            {
                if (!addImm(13, 31, static_cast<long long>(printBufTopSlot_) * 8) || // ptr = sp + top*8
                    !movReg(14, 13) ||                                             // top = ptr (للطول)
                    !movz(10, kItoaRadixArm64))
                    return false;
                const size_t loop = code_.size();
                if (!rrr(a64::mnem::kUdiv, 11, 9, 10) || !msub(12, 11, 10, 9) || // x11=x9/10، x12=x9−x11×10
                    !addImm(12, 12, kAsciiZeroArm64) ||
                    !subImm(13, 13, 1) || !strb(12, 13) ||
                    !movReg(9, 11) || !emitCbnzBack(9, loop))
                    return false;
                return movReg(a64reg::kX1, 13) &&
                       rrr(a64::mnem::kSub, a64reg::kX2, 14, 13) &&
                       movz(a64reg::kX0, kFdStdoutArm64) &&
                       movz(a64reg::kX8, kSysWriteArm64) &&
                       emit(a64::mnem::kSvc, "", {});
            }

            // (AR) ثوابتُ نمطِ بتّاتٍ + رموزٌ للمُنسِّق العشريّ (لا حرفيّاتٍ خام).
            static constexpr unsigned long long kF64SignMask = 0x8000000000000000ULL;
            bool unaryAnyTagged_ = false; // (AR) هل كان معامِلُ الأحاديّ علبةَ Any؟
            bool unaryStaticFloat_ = false; // (AR) معامِلٌ عشريٌّ ساكنٌ (غيرُ معلَّب)؟
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
                // (١·٥) [عائمٌ ≥2^63] fcvtzs يُشبِعُ عند |x|≥2^63 (⇒INT64_MAX) ⇒ يكسرُ رسمَ 2^63 (نتيجةُ
                //       INT64_MIN//‑1) والتماثلَ مع x86 (cvttsd2si يفيض⇒INT64_MIN). القيمُ ≥2^63 صحيحةٌ
                //       حتمًا (مانتيسا ٥٢ بتًّا) ⇒ الجزءُ الصحيحُ لا-موقَّعًا: 2^63 + (i64)(|x|−2^63) ثمّ OR
                //       2^63 (نفسُ خوارزميّةِ x86، بلا fcvtzu) + ".0"؛ الكسرُ صفر. '‑' طُبِع سلفًا.
                if (!loadFloatConst(kScratch0, kTwoPow63) || !fmovToFp(kD1, kScratch0)) // d1 = 2^63.0
                    return false;
                if (!ldrSlot(9, floatValSlot_) || !fmovToFp(kD0, 9) || !fcmp(kD0, kD1)) // |x| vs 2^63
                    return false;
                size_t below2p63;
                if (!emitBranchFwd(a64::mnem::kBlt, "rel19", below2p63)) // |x| < 2^63 ⇒ المعتاد
                    return false;
                if (!fsub(kD0, kD0, kD1) || !fcvtzs(9, kD0) ||                       // x9 = (i64)(|x|−2^63)
                    !movImm64Bits(kScratch0, kF64SignMask) || !rrr(a64::mnem::kOrr, 9, 9, kScratch0)) // x9 |= 2^63
                    return false;
                size_t bigDone;
                if (!emitPrintUInt() || !emitPrintString(kFloatDotZero) ||
                    !emitBranchFwd(a64::mnem::kB, "rel26", bigDone))
                    return false;
                if (!patchBranchFwd(below2p63, 23, 5))
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
                if (!(movReg(kX2, 12) && movz(kX0, kFdStdoutArm64) &&
                      movz(kX8, kSysWriteArm64) && emit(a64::mnem::kSvc, "", {})))
                    return false;
                if (!patchBranchFwd(bigDone, 25, 0)) // (AR) مقصدُ قفزةِ فرعِ ≥2^63 (بعد الكتابة)
                    return false;
                return true;
            }

            // (AR) طباعةُ المنطقيِّ الساكن «صحيح»/«خطأ» — مرآةُ `emitPrintBool` في x86
            //      (الحجّةُ والقياسُ هناك). النصّان من مصدرِ الحقيقةِ لا حرفيّتَين.
            bool emitPrintBool(int valReg)
            {
                size_t isFalse, done;
                if (!cmp(valReg, 31) || !emitBranchFwd(a64::mnem::kBeq, "rel19", isFalse))
                    return false;
                if (!emitPrintString(kDynBoolTrueText) ||
                    !emitBranchFwd(a64::mnem::kB, "rel26", done))
                    return false;
                if (!patchBranchFwd(isFalse, 23, 5))
                    return false;
                if (!emitPrintString(kDynBoolFalseText))
                    return false;
                return patchBranchFwd(done, 25, 0);
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
            // (AR) يقصّ x0 إلى صفرٍ إن كان سالبًا — نظيرُ clampNonNegative في x86. نداءاتُ
            //      النظام تُعيد ‎-errno‎، وطولٌ سالبٌ يُنتج حجمَ تخصيصٍ سالبًا ثمّ انهيارًا.
            // (EN) Clamp x0 to zero when negative — the AArch64 twin of clampNonNegative.
            bool clampNonNegativeArm64(int reg)
            {
                if (!cmp(reg, a64reg::kXzr))
                    return false;
                size_t nonNeg;
                if (!emitBranchFwd(a64::mnem::kBge, "rel19", nonNeg))
                    return false;
                if (!movz(reg, 0))
                    return false;
                return patchBranchFwd(nonNeg, 23, 5);
            }
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
                // (AR) توصيةُ أميليا (الدفعة ٨، تماثلٌ مع x86): اسمٌ يطابق خانةَ إطارٍ لا يجوز أن
                //      يُخصَّص سجلَّ حوض — يقرؤه isMemVar/materialize من الذاكرة بينما يخصّصه هذا
                //      سجلًّا ⇒ افتراقٌ صامتٌ لنصفَي الاسم (x86 يفشل صراحةً، وكان ARM64 يمرّره صامتًا).
                //      استثناءُ القيمة العابرة للكتل (PHI/مقارنةٌ حيّة): تُعرَّف سجلًّا ثمّ تُنسَك في خانتها.
                if (memSlot_.find(vreg) != memSlot_.end() && !crossBlockSpill_.count(vreg))
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kVregAliasesSlot + diag::kVregSigil + vreg);
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
                    return movConst(dst, c);
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

            // (AR) مصفوفة→نصّ (Any): يقرأ وسمَ العنصرِ i (tagReg) من tags[i]، أو Int إن كان مخزنُ
            //      الوسوم null. يستعمل x16 خدشًا؛ يُبقي iReg (يجب أن يخالف tagReg وx16).
            bool emitAtsDynLoadTagArm64(int tagReg, int iReg)
            {
                if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 5) || !cmp(a64reg::kScratch0, a64reg::kXzr))
                    return false;
                size_t haveTags;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", haveTags))
                    return false;
                if (!movz(tagReg, kDynKindInt)) // tags=null ⇒ Int
                    return false;
                size_t done;
                if (!emitBranchFwd(a64::mnem::kB, "rel26", done))
                    return false;
                if (!patchBranchFwd(haveTags, 23, 5))
                    return false;
                if (!addLsl3(a64reg::kScratch0, a64reg::kScratch0, iReg) || !ldrBase(tagReg, a64reg::kScratch0, 0))
                    return false;
                if (!patchBranchFwd(done, 25, 0))
                    return false;
                return true;
            }

            // (AR) يبني نصَّ عددٍ صحيحٍ (x9) تنازليًّا في مخزنِ itoa (printBufTopSlot_)؛ x13 = المؤشّر،
            //      x14 = القمّة (الطولُ = x14 − x13). itoa باقٍ-سالبٌ ⇒ آمنٌ لـINT64_MIN. x10=10.
            bool emitAtsIntToScratchArm64()
            {
                if (!addImm(13, 31, static_cast<long long>(printBufTopSlot_) * 8) || !movReg(14, 13) ||
                    !movz(10, kItoaRadixArm64) || !cmp(9, a64reg::kXzr))
                    return false;
                size_t pos;
                if (!emitBranchFwd(a64::mnem::kBge, "rel19", pos))
                    return false;
                const size_t neg = code_.size();
                if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                    !rrr(a64::mnem::kSub, 12, 31, 12) || !addImm(12, 12, kAsciiZeroArm64) ||
                    !subImm(13, 13, 1) || !strb(12, 13) || !movReg(9, 11) || !emitCbnzBack(9, neg))
                    return false;
                if (!movz(12, kAsciiMinusArm64) || !subImm(13, 13, 1) || !strb(12, 13))
                    return false;
                size_t done;
                if (!emitBranchFwd(a64::mnem::kB, "rel26", done))
                    return false;
                if (!patchBranchFwd(pos, 23, 5))
                    return false;
                const size_t p = code_.size();
                if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                    !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                    !movReg(9, 11) || !emitCbnzBack(9, p))
                    return false;
                if (!patchBranchFwd(done, 25, 0))
                    return false;
                return true;
            }

            // (AR) يبني نصَّ عشريٍّ (نمطُ بتّاته في x9) fixed6 مع حذفِ الأصفار تنازليًّا في مخزنِ خدشٍ
            //      (atsFtoaTopSlot_)؛ x13 = المؤشّر، x14 = القمّة. لا mmap ⇒ سجلّاتٌ خدشٌ (x17=sign, x15=ip،
            //      x12=scaled, x11=nd, x10=10). مطابقٌ لـF64_TO_STRING.
            bool emitAtsFloatToScratchArm64()
            {
                // sign ⇒ x17.
                if (!movImm64Bits(a64reg::kScratch0, kF64SignMask) ||
                    !rrr(a64::mnem::kAnd, 10, 9, a64reg::kScratch0) || !cmp(10, 31))
                    return false;
                size_t signZero;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", signZero))
                    return false;
                if (!movz(a64reg::kScratch1, 1))
                    return false;
                size_t signSet;
                if (!emitBranchFwd(a64::mnem::kB, "rel26", signSet))
                    return false;
                if (!patchBranchFwd(signZero, 23, 5))
                    return false;
                if (!movz(a64reg::kScratch1, 0))
                    return false;
                if (!patchBranchFwd(signSet, 25, 0))
                    return false;
                // |x| ⇒ ip=x15.
                if (!movImm64Bits(a64reg::kScratch0, kF64AbsMask) || !rrr(a64::mnem::kAnd, 9, 9, a64reg::kScratch0))
                    return false;
                if (!fmovToFp(kD0, 9) || !fcvtzs(15, kD0))
                    return false;
                if (!fmovToFp(kD0, 9) || !scvtf(kD1, 15) || !fsub(kD0, kD0, kD1))
                    return false;
                if (!loadFloatConst(a64reg::kScratch0, static_cast<double>(kFloatPrecisionScale)) ||
                    !fmovToFp(kD1, a64reg::kScratch0) || !fmul(kD0, kD0, kD1) || !fcvtns(12, kD0))
                    return false;
                if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kFloatPrecisionScale)) ||
                    !cmp(12, a64reg::kScratch0))
                    return false;
                size_t noCarry;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", noCarry))
                    return false;
                if (!addImm(15, 15, 1) || !movz(12, 0))
                    return false;
                if (!patchBranchFwd(noCarry, 23, 5))
                    return false;
                if (!movz(11, kFloatDecimals) || !movz(10, kItoaRadixArm64) || !movz(a64reg::kScratch0, 1))
                    return false;
                const size_t stripTop = code_.size();
                if (!cmp(11, a64reg::kScratch0))
                    return false;
                size_t stripDone;
                if (!emitBranchFwd(a64::mnem::kBle, "rel19", stripDone))
                    return false;
                if (!rrr(a64::mnem::kSdiv, 14, 12, 10) || !msub(9, 14, 10, 12) || !cmp(9, 31))
                    return false;
                size_t stripStop;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", stripStop))
                    return false;
                if (!movReg(12, 14) || !subImm(11, 11, 1) || !emitBBack(stripTop))
                    return false;
                if (!patchBranchFwd(stripDone, 23, 5) || !patchBranchFwd(stripStop, 23, 5))
                    return false;
                // ابنِ تنازليًّا في المخزن؛ x13 = المؤشّر.
                if (!addImm(13, 31, static_cast<long long>(atsFtoaTopSlot_) * 8) || !movReg(9, 12))
                    return false;
                const size_t fracTop = code_.size();
                if (!cmp(11, 31))
                    return false;
                size_t fracDone;
                if (!emitBranchFwd(a64::mnem::kBle, "rel19", fracDone))
                    return false;
                if (!rrr(a64::mnem::kSdiv, 14, 9, 10) || !msub(12, 14, 10, 9) ||
                    !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                    !movReg(9, 14) || !subImm(11, 11, 1) || !emitBBack(fracTop))
                    return false;
                if (!patchBranchFwd(fracDone, 23, 5))
                    return false;
                if (!subImm(13, 13, 1) || !movz(12, kAsciiDotArm64) || !strb(12, 13))
                    return false;
                if (!movReg(9, 15))
                    return false;
                const size_t ipTop = code_.size();
                if (!rrr(a64::mnem::kSdiv, 14, 9, 10) || !msub(12, 14, 10, 9) ||
                    !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                    !movReg(9, 14) || !emitCbnzBack(9, ipTop))
                    return false;
                if (!cmp(a64reg::kScratch1, 31))
                    return false;
                size_t noSign;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", noSign))
                    return false;
                if (!subImm(13, 13, 1) || !movz(12, kAsciiMinusArm64) || !strb(12, 13))
                    return false;
                if (!patchBranchFwd(noSign, 23, 5))
                    return false;
                // x14 = القمّة (للطول).
                if (!addImm(14, 31, static_cast<long long>(atsFtoaTopSlot_) * 8))
                    return false;
                return true;
            }

            // (AR) يحمّل وسيطَ نداءٍ/معاملَ طباعةٍ في سجلٍّ وجهة. يُستدعى بعد انسكابِ المؤقّتات،
            //      فالمؤقّتُ يُحمَّل من **خانة انسكابه** لا من سجلّه ⇒ صفر تصادمِ نقلٍ متوازٍ.
            bool loadArgInto(int dst, const sir::SIROperand &op)
            {
                long long c;
                if (common::isConstInt(op, c))
                    return movConst(dst, c);
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

            // (AR) يمرّر وسيطًا منطقيًّا إلى الفتحة `slot`: <٨ ⇒ سجلُّ ABI (x0..x7)؛ ≥٨ ⇒ منطقةُ
            //      الوسائطِ الصادرةِ في أسفلِ الإطار ([sp + (slot-8)×8]) عبر سجلِّ النقلِ `scratch`.
            //      النصُّ يُجسَّد (حرفيّةُ rodata أو مؤشّرُ كومةٍ من الانسكاب). سدُّ اختلالِ x86≠ARM64.
            bool passAbiArg(size_t slot, const sir::SIROperand &op, int scratch)
            {
                if (slot < 8)
                {
                    if (op.dataType == types::SadTypeKind::String)
                        return materializeString(op, abiArg_[slot], /*fromSpill=*/true);
                    return loadArgInto(abiArg_[slot], op);
                }
                if (op.dataType == types::SadTypeKind::String)
                {
                    if (!materializeString(op, scratch, /*fromSpill=*/true))
                        return false;
                }
                else if (!loadArgInto(scratch, op))
                    return false;
                return strSlot(scratch, static_cast<int>(slot - 8));
            }

            // (AR) المسحُ المسبق: يخصّص فهرسَ خانةٍ للمعاملات (بترتيب ABI) ثمّ لكلّ ALLOC، ثمّ (إن
            //      لزم) لمخزنِ itoa ولمنطقة الانسكاب؛ ويحسب حجمَ الإطارِ المُحاذى ١٦. المعاملُ يُعامَل
            //      كمتغيّرِ ذاكرةٍ (قراءتُه = تحميلٌ من خانته)، وتُسكَنُ خانتُه من سجلّ الوسيط في المقدّمة.
            bool assignFrameSlots(const sir::SIRFunction &fn)
            {
                const auto &params = fn.getParameters();
                lrSlot_ = -1; // (AR) يُصفَّر لكلّ دالّة؛ يُحجَز حين تنادي دالّةٌ غيرُ داخلة.
                // (AR) امسحْ مسبقًا أقصى عددِ وسائطٍ صادرةٍ زائدةٍ (>٨) عبر كلِّ نداءٍ، لحجزِ منطقتها
                //      في أسفلِ الإطار **قبلَ** ترقيمِ الخانات (بخلافِ x86، هنا الخاناتُ نسبيّةٌ لـsp
                //      فالمنطقةُ يجب أن تبدأ من الخانةِ صفر ⇒ تُزاحُ بقيّةُ الخانات فوقها).
                maxOutgoingStackArgs_ = 0;
                for (const auto &blockPtr : fn.getBasicBlocks())
                    for (const auto &inst : blockPtr->instructions)
                        if (inst.opcode == sir::SIROpcode::CALL ||
                            inst.opcode == sir::SIROpcode::CALL_INDIRECT ||
                            inst.opcode == sir::SIROpcode::OBJECT_CALL ||
                            inst.opcode == sir::SIROpcode::CLOSURE_CALL)
                        {
                            size_t regArgs = 0;
                            if (inst.opcode == sir::SIROpcode::CALL ||
                                inst.opcode == sir::SIROpcode::CALL_INDIRECT)
                                regArgs = inst.operands.size() - 1;
                            else if (inst.opcode == sir::SIROpcode::OBJECT_CALL)
                                regArgs = inst.operands.size() >= 2 ? inst.operands.size() - 1 : 0;
                            else // CLOSURE_CALL: الصريحةُ (size-1) + البيئة
                                regArgs = inst.operands.size();
                            if (regArgs > 8 &&
                                static_cast<int>(regArgs - 8) > maxOutgoingStackArgs_)
                                maxOutgoingStackArgs_ = static_cast<int>(regArgs - 8);
                        }
                // (AR) الخاناتُ 0..maxOutgoingStackArgs_-1 محجوزةٌ لمنطقةِ الوسائطِ الصادرة ⇒ ابدأْ فوقها.
                int slot = maxOutgoingStackArgs_;
                // (AR) المعاملاتُ ٨+ تصلُ على مكدّسِ المُنادي؛ فهرسُها = frameSize_/8 + (i-8) يُحسَب
                //      بعدَ معرفةِ حجمِ الإطار (تُجمَع هنا وتُرقَّم في نهايةِ الدالّة).
                std::vector<std::pair<std::string, size_t>> stackParams;
                for (size_t i = 0; i < params.size(); ++i)
                    // (AR) المعاملُ يُشار إليه بـ«%»+الاسم (sir_builder_functions) بينما اسمُ
                    //      SIRParameter بلا «%» ⇒ نُفهرِس بالمرجع كي يطابقَه isMemVar.
                    if (i < 8)
                        memSlot_[diag::kVregSigil + params[i].name] = slot++;
                    else
                        stackParams.push_back({diag::kVregSigil + params[i].name, i});

                bool hasCall = false;
                bool hasPrint = false;
                bool hasNumberPrint = false;
                bool hasFloatPrint = false; // (AR) طباعةُ عشريّ ⇒ خانةُ خدشِ البتّات + مُنسِّق fixed6
                bool hasArrayNew = false; // (AR) ARRAY_NEW ⇒ mmap (نسكٌ تحفّظيّ حولَ svc)
                bool hasAppend = false;   // (AR) BUILTIN_ARRAY_APPEND ⇒ mmap (عند النموّ) + خانات خدش
                bool hasBoxing = false;   // (AR) SET/GET معلَّبٌ ⇒ يستعمل الحوضَ خدشًا + خانات dyn
                bool hasMemBlock = false; // (AR) حجز/حرر/عبّئ/انسخ (نواةُ الكومة) ⇒ svc/حلقةٌ تدهس الحوض
                bool hasFileBytes = false; // (AR) اكتب_بايتات/اقرأ_بايتات ⇒ نداءاتُ نظامٍ + mmap تدهس الحوض
                bool hasStrHeap = false;  // (AR) أوپكودُ نصٍّ يخصّص كومةً داخليًّا (I64_TO_STRING/CONCAT/…)
                bool hasArrayExt = false; // (AR) CONCAT/ZIP ⇒ mmap (+حلقةٌ لـZIP) + خاناتُ خدشٍ عابرةٌ لـmmap
                bool hasArrayToStr = false; // (AR) ARRAY_TO_STRING ⇒ مخزنُ خدشِ عشريّ للمسار العشريّ
                bool hasFloorDiv = false;   // (AR) FLOOR_DIV_I64 ⇒ خانةُ حفظِ a لتصحيحِ أرضيّةِ السالب
                dynGetCount_ = 0;
                for (const auto &blockPtr : fn.getBasicBlocks())
                    for (const auto &inst : blockPtr->instructions)
                    {
                        if (inst.opcode == sir::SIROpcode::FLOOR_DIV_I64)
                            hasFloorDiv = true; // (AR) يلزمه خانةُ حفظِ المقسوم لتصحيحِ الأرضيّة
                        // (AR) PHI: احجز خانةً لناتجِه (memSlot_ ⇒ قراءةٌ لاحقةٌ تُحلُّ تحميلًا)، وسجّل
                        //      كلَّ حافّةٍ بأزواجِ [قيمة، لصيقةُ سَلَف].
                        if (inst.opcode == sir::SIROpcode::PHI && inst.result)
                        {
                            memSlot_[inst.result->name] = slot++;
                            for (size_t k = 0; k + 1 < inst.operands.size(); k += 2)
                                phiEdges_[inst.operands[k + 1].name].push_back(
                                    {inst.operands[k], inst.result->name});
                        }
                        if (inst.opcode == sir::SIROpcode::ALLOC && inst.result)
                            memSlot_[inst.result->name] = slot++;
                        else if (inst.opcode == sir::SIROpcode::CALL ||
                                 inst.opcode == sir::SIROpcode::OBJECT_CALL || // (AR) نداءٌ افتراضيّ (blr يدهس x30)
                                 inst.opcode == sir::SIROpcode::CLOSURE_CALL || // (AR) نداءُ إغلاقٍ (الدفعة ٧)
                                 inst.opcode == sir::SIROpcode::CALL_INDIRECT)  // (AR) نداءٌ عبر مؤشّرِ دالّة
                            hasCall = true;
                        else if (inst.opcode == sir::SIROpcode::ARRAY_NEW ||
                                 inst.opcode == sir::SIROpcode::TUPLE_NEW || // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً
                                 inst.opcode == sir::SIROpcode::ENUM_CONSTRUCT) // (AR) بناءُ تعدادٍ = mmap للكومة
                            hasArrayNew = true;
                        // (AR) ALLOC بمعامل نصّيّ (اسم صنف) = كائنُ كومة ⇒ mmap يدهس الحوض.
                        if (inst.opcode == sir::SIROpcode::ALLOC && !inst.operands.empty() &&
                            inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                            inst.operands[0].dataType == types::SadTypeKind::String)
                            hasArrayNew = true;
                        // (AR) CLOSURE_CREATE (الدفعة ٧) = mmap لبنية {fn,env}+الملتقَطات ⇒ يدهس الحوض.
                        if (inst.opcode == sir::SIROpcode::CLOSURE_CREATE)
                            hasArrayNew = true;
                        // (AR) محلّيٌّ ضمنيّ (الدفعة ٧، مرآةُ x86): STORE ثنائيٌّ إلى سجلٍّ بلا ALLOC
                        //      (مثل %__cap_X في جسم اللامدا) ⇒ يلزمه فهرسُ خانةٍ كي يطابقَه isMemVar.
                        if (inst.opcode == sir::SIROpcode::STORE && inst.operands.size() == 2 &&
                            inst.operands[1].type == sir::SIROperandType::REGISTER &&
                            memSlot_.find(inst.operands[1].name) == memSlot_.end())
                            memSlot_[inst.operands[1].name] = slot++;
                        else if (inst.opcode == sir::SIROpcode::BUILTIN_ARRAY_APPEND ||
                                 inst.opcode == sir::SIROpcode::ARRAY_APPEND)
                            hasAppend = true;
                        else if (inst.opcode == sir::SIROpcode::ARRAY_CONCAT ||
                                 inst.opcode == sir::SIROpcode::ARRAY_ZIP)
                            hasArrayExt = true; // (AR) mmap (+حلقةٌ) + خاناتُ خدشٍ عابرةٌ لـmmap
                        else if (inst.opcode == sir::SIROpcode::ARRAY_REMOVE ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_ARRAY_REMOVE)
                            hasMemBlock = true; // (AR) حلقةُ إزاحةٍ في المكان تدهس الحوضَ ⇒ انسكابٌ حولَها
                        else if (inst.opcode == sir::SIROpcode::ALLOC_HEAP ||
                                 inst.opcode == sir::SIROpcode::FFI_MALLOC ||
                                 inst.opcode == sir::SIROpcode::FREE ||
                                 inst.opcode == sir::SIROpcode::FFI_FREE ||
                                 inst.opcode == sir::SIROpcode::MEMSET ||
                                 inst.opcode == sir::SIROpcode::FFI_MEMSET ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_MEM_SET ||
                                 inst.opcode == sir::SIROpcode::MEMCPY ||
                                 inst.opcode == sir::SIROpcode::FFI_MEMCPY ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_MEM_COPY ||
                                 // (AR) النصوصُ الديناميّةُ ذاتُ الحلقة (الدفعة ٣): مسحٌ بايتيٌّ يدهس الحوضَ
                                 //      (STRING_NEW/BOOL_TO_STRING مستثناةٌ: خدشٌ فقط، لا حلقة).
                                 inst.opcode == sir::SIROpcode::STRING_LEN ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_STRING_LENGTH ||
                                 inst.opcode == sir::SIROpcode::STRING_CMP ||
                                 inst.opcode == sir::SIROpcode::STRING_ORD_CMP ||
                                 inst.opcode == sir::SIROpcode::STRING_TO_I64 ||
                                 inst.opcode == sir::SIROpcode::STRING_TO_F64)
                            hasMemBlock = true; // (AR) svc/حلقةُ بايتاتٍ تدهس الحوض ⇒ انسكابٌ حولَها
                        else if (inst.opcode == sir::SIROpcode::I64_TO_STRING ||
                                 inst.opcode == sir::SIROpcode::F64_TO_STRING ||
                                 inst.opcode == sir::SIROpcode::STRING_CONCAT ||
                                 inst.opcode == sir::SIROpcode::STRING_SUBSTR ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_STRING_SUBSTRING ||
                                 inst.opcode == sir::SIROpcode::STRING_FIND ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_STRING_FIND ||
                                 inst.opcode == sir::SIROpcode::STRING_REPLACE ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_STRING_REPLACE)
                            hasStrHeap = true; // (AR) يخصّص مخزنَ نصٍّ/يستعمل خانةَ hayBase (mmap/الحوض)
                        else if (inst.opcode == sir::SIROpcode::BUILTIN_FILE_WRITE_BYTES ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_FILE_READ_BYTES)
                            hasFileBytes = true; // (AR) openat/read/write/lseek + mmap ⇒ انسكابٌ + خانات خدش
                        else if (inst.opcode == sir::SIROpcode::ARRAY_TO_STRING ||
                                 inst.opcode == sir::SIROpcode::TUPLE_TO_STRING)
                        {
                            // (AR) مصفوفة→نصّ سياديّة (مرآةُ x86): mmap ⇒ خانات strHeap + انسكاب،
                            //      وitoa تنازليًّا في مخزنِ الإطار (printBufTopSlot_).
                            hasStrHeap = true;
                            hasNumberPrint = true;
                            hasArrayToStr = true; // (AR) + مخزنُ خدشٍ عشريّ للمسار العشريّ
                        }
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
                        if ((inst.opcode == sir::SIROpcode::ARRAY_GET ||
                             inst.opcode == sir::SIROpcode::TUPLE_GET) && inst.result &&
                            inst.result->dataType == types::SadTypeKind::Any)
                        {
                            hasBoxing = true;
                            ++dynGetCount_;
                        }
                        // (AR) [عقدُ Any] عمليّةٌ حسابيّةٌ نتيجتُها Any (سلسلةُ Any) ⇒ خانةُ dyn لتعليبها
                        //      (مرآةُ x86): FLOOR_DIV/DIV (وحافّةُ INT64_MIN//‑1 بوسمِ Float) وADD/SUB/MUL.
                        if ((inst.opcode == sir::SIROpcode::FLOOR_DIV_I64 ||
                             inst.opcode == sir::SIROpcode::DIV_I64 ||
                             inst.opcode == sir::SIROpcode::MOD_I64 ||
                             inst.opcode == sir::SIROpcode::ADD_I64 ||
                             inst.opcode == sir::SIROpcode::SUB_I64 ||
                             inst.opcode == sir::SIROpcode::MUL_I64) && inst.result &&
                            inst.result->dataType == types::SadTypeKind::Any)
                        {
                            hasBoxing = true;
                            ++dynGetCount_;
                        }
                        // (AR) [عقدُ Any] **والأحاديُّ** (NEG/NOT) — مرآةُ x86، والعلّةُ
                        //      والقياسُ مفصَّلان هناك: `boxUnaryResult` يُعلِّب ولم يكن
                        //      معدودًا ⇒ خانةٌ غيرُ محجوزةٍ تدهس جارتَها («لاشيء» هنا
                        //      و«0.0» في x86 مكانَ «‑2.5»).
                        if ((inst.opcode == sir::SIROpcode::NEG ||
                             inst.opcode == sir::SIROpcode::NOT) && inst.result &&
                            inst.result->dataType == types::SadTypeKind::Any)
                        {
                            hasBoxing = true;
                            ++dynGetCount_;
                        }
                    }
                // (AR) القيمُ العابرةُ للكتل (تُعرَّف في كتلةٍ وتُقرأ في أخرى، عدا PHI/ALLOC/السلاسل):
                //      تُنسَك عند تعريفها في خانةٍ. مُمرَّرٌ أوّلٌ يسجّل كتلةَ التعريف، وثانٍ يكتشف الاستعمالَ
                //      المغاير (متجاهلًا معامِلاتِ PHI). كلٌّ يُخصَّص فهرسَ خانةٍ هنا (slot++).
                {
                    std::map<std::string, const sir::SIRBasicBlock *> defBlk;
                    for (const auto &blockPtr : fn.getBasicBlocks())
                        for (const auto &inst : blockPtr->instructions)
                        {
                            if (!inst.result || inst.opcode == sir::SIROpcode::ALLOC ||
                                inst.opcode == sir::SIROpcode::PHI)
                                continue;
                            if (inst.opcode == sir::SIROpcode::MOVE && !inst.operands.empty() &&
                                inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                                inst.operands[0].dataType == types::SadTypeKind::String)
                                continue;
                            defBlk[inst.result->name] = blockPtr.get();
                        }
                    for (const auto &blockPtr : fn.getBasicBlocks())
                        for (const auto &inst : blockPtr->instructions)
                            // (AR) نشمل معامِلاتِ PHI عمدًا (نسكُها عند التعريف يحصُنُ خزنَ الحافّة ضدّ الدهس).
                            for (const auto &op : inst.operands)
                                if (op.type == sir::SIROperandType::REGISTER)
                                {
                                    auto it = defBlk.find(op.name);
                                    if (it != defBlk.end() && it->second != blockPtr.get())
                                        crossBlockSpill_.insert(op.name);
                                }
                    for (const auto &name : crossBlockSpill_)
                        if (memSlot_.find(name) == memSlot_.end())
                            memSlot_[name] = slot++;
                }
                // (AR) 🔑 حفظُ سجلّ الرابط x30 (الدفعة ٦): bl/blr يدهسان x30. الدالّةُ الداخلة تنادي
                //      وتخرج svc (لا تعود ⇒ لا حفظ). الدالّةُ غيرُ الداخلةِ التي تنادي تحفظ x30 في خانةِ
                //      إطارٍ في المقدّمة وتستعيده قبل RET ⇒ يُمكِّن الطرائقَ من نداءِ الطرائق (حرِجٌ
                //      للاستضافة الذاتيّة) والإرسالَ الافتراضيّ (blr) في دالّةٍ غيرِ داخلة.
                if (hasCall && !curIsEntry_)
                    lrSlot_ = slot++;

                // (AR) طباعةُ عددٍ: احجز مخزنَ itoa (٣ خاناتٍ = ٢٤ بايتًا تكفي ٢٠ رقمًا لـi64 + هامش).
                //      القمّةُ (العنوانُ الأعلى، حصريّ) = فهرسُ ما بعد المخزن.
                if (hasNumberPrint)
                {
                    slot += 3;
                    printBufTopSlot_ = slot; // (AR) العنوانُ الأعلى الحصريّ = sp + slot*8
                }
                // (AR) مصفوفةُ العشريّ→نصّ: مخزنُ خدشِ عنصرٍ عشريّ (kFtoaBufPayload/8 خانات، القمّةُ حصريّة).
                if (hasArrayToStr)
                {
                    slot += static_cast<int>(kFtoaBufPayload / kArrSlotBytes);
                    atsFtoaTopSlot_ = slot;
                }
                // (AR) طباعةُ عشريّ: خانةٌ لنمطِ بتّاتِ الـdouble (القيمةِ المطلقة) عبر مراحل المُنسِّق.
                if (hasFloatPrint)
                    floatValSlot_ = slot++;
                // (AR) قسمةٌ أرضيّةٌ موقَّعة: خانةٌ لحفظِ المقسوم a (يُعادُ تحميلُه بعد msub لحسابِ إشارةِ a^b).
                if (hasFloorDiv)
                    floorDivDividendSlot_ = slot++;
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
                // (AR) عمليّاتُ كومةِ النصّ: kStrHeapSlots خاناتٍ تبقى حيّةً عبر mmap الداخليّ.
                if (hasStrHeap)
                {
                    strHeapBaseSlot_ = slot;
                    slot += static_cast<int>(kStrHeapSlots);
                }
                // (AR) امتدادُ المصفوفة (CONCAT/ZIP): ٦ خاناتِ خدشٍ تبقى حيّةً عبر mmap الداخليّ.
                if (hasArrayExt)
                {
                    arrExtBaseSlot_ = slot;
                    slot += kArrExtSlots;
                }
                // (AR) بايتاتُ الملفّات: ٦ خاناتِ خدشٍ تبقى حيّةً عبر نداءاتِ النظام وmmap.
                if (hasFileBytes)
                {
                    fileBytesBaseSlot_ = slot;
                    slot += kFileBytesSlots;
                }
                // (AR) إن نادت الدالّةُ أو طبعت أو خصّصت مصفوفة أو ألحقت أو علّبت، احجز منطقةَ انسكابٍ.
                if (hasCall || hasPrint || hasArrayNew || hasAppend || hasBoxing || hasMemBlock || hasStrHeap || hasArrayExt || hasFileBytes)
                {
                    spillBaseSlot_ = slot;
                    slot += static_cast<int>(pool_.size());
                }
                const long long bytes = static_cast<long long>(slot) * 8;
                frameSize_ = (bytes + 15) / 16 * 16; // (AR) مُحاذاةُ ١٦ (عقدُ AAPCS64 لـSP)
                // (AR) المعاملاتُ ٨+: بعدَ sub sp,#frameSize تُصبح وسائطُ المُنادي الصادرةُ ([sp+j×8]
                //      عنده) عندنا في [sp + frameSize_ + j×8] (BL لا يدفع عنوانَ عودةٍ للمكدّس) ⇒
                //      فهرسُ الخانة = frameSize_/8 + (i-8). تُقرأُ في مكانها ⇒ لا تخزينَ في المقدّمة.
                for (const auto &sp : stackParams)
                    memSlot_[sp.first] =
                        static_cast<int>(frameSize_ / 8) + static_cast<int>(sp.second - 8);
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
                // (AR) احفظْ سجلّ الرابط x30 (إن كانت الدالّةُ تنادي وليست داخلة) قبلَ أوّلِ نداءٍ
                //      يدهسه ⇒ يُستعاد في الخاتمة. (الدفعة ٦)
                if (lrSlot_ >= 0 && !strSlot(a64reg::kLr, lrSlot_))
                    return false;
                const auto &params = fn.getParameters();
                // (AR) خزّن سجلّاتِ الوسائطِ الواردةَ (٨ كحدٍّ أقصى) في خاناتها؛ المعاملاتُ ٨+
                //      واردةٌ على مكدّسِ المُنادي وتُقرأُ في مكانها ⇒ لا تخزينَ لها.
                for (size_t i = 0; i < params.size() && i < 8; ++i)
                    if (!strSlot(abiArg_[i], memSlot_[diag::kVregSigil + params[i].name]))
                        return false;
                return true;
            }

            // (AR) خاتمةُ الدالّة غير الداخلة: استعِد سجلَّ الرابط x30 (إن حُفِظ) ثمّ المكدّسَ (add sp)
            //      ثمّ ارجع (RET إلى x30).
            bool emitEpilogue()
            {
                if (lrSlot_ >= 0 && !ldrSlot(a64reg::kLr, lrSlot_)) // (AR) استعِد x30 المحفوظ
                    return false;
                if (frameSize_ > 0 && !addImm(31, 31, frameSize_)) // add sp, sp, #frameSize
                    return false;
                return emit(a64::mnem::kRet, "", {});
            }

            // ══════════════════════════════════════════════════════════════
            // (AR) **جدولُ العمليّاتِ الثنائيّةِ وخطّافاتُها** (م٤، الطورُ الرابع).
            //   التتابعُ (تحقّقٌ ⇒ جدولٌ ⇒ وجهةٌ ⇒ معامِلان ⇒ إصدارٌ ⇒ تعليب) في
            //   `common::LoweringDriver`؛ وهنا مضمونُه في AArch64 وحدَه.
            //   العشرُ في الجدول هي عينُها في جدول x86-64 — وهذه المطابقةُ مقصودةٌ:
            //   المُصدِرُ المجرَّد لا يُبنى على تناظرٍ جزئيّ، فمَن يقود هدفَين بعضويّتين
            //   مختلفتين قد وحَّد اسمًا لا شكلًا.
            // ══════════════════════════════════════════════════════════════
            const common::BinaryOpShape *binaryShape(sir::SIROpcode op) const
            {
                using OP = sir::SIROpcode;
                // (AR) عقدُ Any: الحسابُ الصحيحُ يُعلِّب، والبتّيّاتُ والعشريّاتُ لا.
                static constexpr common::BinaryOpShape kBoxed{true};
                static constexpr common::BinaryOpShape kRaw{false};
                switch (op)
                {
                case OP::ADD_I64:
                case OP::SUB_I64:
                case OP::MUL_I64:
                    return &kBoxed;
                case OP::AND:
                case OP::OR:
                case OP::XOR:
                case OP::ADD_F64:
                case OP::SUB_F64:
                case OP::MUL_F64:
                case OP::DIV_F64:
                    return &kRaw;
                default:
                    return nullptr; // (AR) القسمةُ والباقي والإزاحاتُ في lowerBinary أدناه
                }
            }

            // (AR) تهيئةُ المعامِلَين: AArch64 ثلاثيُّ العنوان ⇒ كلاهما في مُبدَّدٍ خارجَ
            //      الحوض (x16/x17) والوجهةُ منفصلة. loadScalarInto يفكّ Any المعلَّب.
            //      ولا طيَّ فوريًّا هنا (بخلاف x86): المعامِلان يُحمَّلان دائمًا.
            bool prepareBinaryOperands(const sir::SIRInstruction &inst, int)
            {
                using OP = sir::SIROpcode;
                if (inst.opcode == OP::ADD_F64 || inst.opcode == OP::SUB_F64 ||
                    inst.opcode == OP::MUL_F64 || inst.opcode == OP::DIV_F64)
                    return loadFloatOperandInto(a64reg::kScratch0, inst.operands[0]) &&
                           loadFloatOperandInto(a64reg::kScratch1, inst.operands[1]);
                return loadScalarInto(a64reg::kScratch0, inst.operands[0]) &&
                       loadScalarInto(a64reg::kScratch1, inst.operands[1]);
            }

            // (AR) الإصدار: ثلاثيُّ العنوان مباشرةً، والعشريّاتُ عبر d0/d1.
            bool emitBinaryOp(const sir::SIRInstruction &inst, int dst)
            {
                using OP = sir::SIROpcode;
                switch (inst.opcode)
                {
                case OP::ADD_I64: return rrr(a64::mnem::kAdd, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SUB_I64: return rrr(a64::mnem::kSub, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::MUL_I64: return rrr(a64::mnem::kMul, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::AND: return rrr(a64::mnem::kAnd, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::OR: return rrr(a64::mnem::kOrr, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::XOR: return rrr(a64::mnem::kEor, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::ADD_F64:
                case OP::SUB_F64:
                case OP::MUL_F64:
                case OP::DIV_F64:
                {
                    // (AR) العشريّ بتّاتُ i64 في x16/x17: عبّئهما في d0/d1، نفّذ عمليّةَ FP
                    //      المزدوجة، ثمّ استخرج البتّاتِ إلى dst (نظيرُ مسارِ xmm في x86).
                    if (!fmovToFp(kD0, a64reg::kScratch0) || !fmovToFp(kD1, a64reg::kScratch1))
                        return false;
                    const bool ok = inst.opcode == OP::ADD_F64   ? fadd(kD0, kD0, kD1)
                                    : inst.opcode == OP::SUB_F64 ? fsub(kD0, kD0, kD1)
                                    : inst.opcode == OP::MUL_F64 ? fmul(kD0, kD0, kD1)
                                                                 : fdiv(kD0, kD0, kD1);
                    return ok && fmovFromFp(dst, kD0);
                }
                // (AR) لا يُبلَغ: `binaryShape` ردّ nullptr لكلِّ ما عداه فما وصل هنا شيء.
                default: return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // (AR) عقدُ Any: الحاصلُ الخامُّ يُعاد تعليبًا (وسمُ Int) — RET/الطباعةُ تفكّان.
            bool boxBinaryResult(const sir::SIRInstruction &inst, int dst) { return boxIfAnyArm64(inst, dst); }

            // (AR) **الإرسالُ بالوسمِ زمنَ التشغيل** — مرآةُ نظيرِه في x86-64 (العلّةُ
            //   والعلاجُ والقيدُ مفصَّلةٌ هناك في `lowerTaggedBinary`). ما يخصُّ AArch64:
            //   • لا `cmp Xn, #imm` ⇒ `sub` ثمّ `cmp Xn, xzr` (كما في `loadFloatOperandInto`).
            //   • حاملُ العلَمِ **x16**: يعبر فرعًا واحدًا فقط (من حسابِه إلى `b.ne`)،
            //     ثمّ يعود مُبدَّدَ معامِلٍ في كلا المسارَين. وx8 مُبدَّدُ الوسم كعادتِه،
            //     ثمّ يحمل وسمَ المُخرَجِ إلى التعليبِ بعد الالتقاء.
            //   • حقولُ الرقعة: `b.<cond>` رَبعُها imm19@23-5 و`b` غيرُ المشروط imm26@25-0.
            bool computeAnyFloatFlagArm64(const sir::SIRInstruction &inst)
            {
                if (!movz(a64reg::kScratch0, 0))
                    return false;
                for (size_t i = 0; i < 2; ++i)
                {
                    const sir::SIROperand &op = inst.operands[i];
                    if (!common::isBoxedAny(op))
                        continue; // (AR) غيرُ المعلَّبِ نوعُه ساكنٌ ⇒ لا وسمَ يُقاس
                    if (!materialize(a64reg::kScratch1, op) ||
                        !ldrBase(a64reg::kX8, a64reg::kScratch1, kSadDynKindOff / kArrSlotBytes) ||
                        !subImm(a64reg::kX8, a64reg::kX8, kDynKindFloat) ||
                        !cmp(a64reg::kX8, a64reg::kXzr))
                        return false;
                    size_t notFloat;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", notFloat))
                        return false;
                    if (!movz(a64reg::kScratch0, 1) || !patchBranchFwd(notFloat, 23, 5))
                        return false;
                }
                return cmp(a64reg::kScratch0, a64reg::kXzr);
            }

            // (AR) **معامِلٌ عشريٌّ مع ترقيةِ الصحيحِ الساكن** — مرآةُ نظيرِه في x86
            //      (العلّةُ والقياسُ مفصَّلان هناك): في المسارَين المُرسَلَين بالوسم لا
            //      صيغةَ عشريّةً سبقتْ، فثابتٌ صحيحٌ يمرّ نمطَ بتّاتِه ويُقرأ double.
            bool loadFloatOperandPromoting(int dst, const sir::SIROperand &op)
            {
                if (common::isBoxedAny(op) || !common::isSignedIntKind(op.dataType))
                    return loadFloatOperandInto(dst, op);
                return materialize(dst, op) && scvtf(kD2, dst) && fmovFromFp(dst, kD2);
            }

            bool emitFloatArithFromIntArm64(sir::SIROpcode op, const sir::SIRInstruction &inst,
                                            int dst)
            {
                using OP = sir::SIROpcode;
                if (!loadFloatOperandPromoting(a64reg::kScratch0, inst.operands[0]) ||
                    !loadFloatOperandPromoting(a64reg::kScratch1, inst.operands[1]) ||
                    !fmovToFp(kD0, a64reg::kScratch0) || !fmovToFp(kD1, a64reg::kScratch1))
                    return false;
                const bool ok = op == OP::ADD_I64   ? fadd(kD0, kD0, kD1)
                                : op == OP::SUB_I64 ? fsub(kD0, kD0, kD1)
                                                    : fmul(kD0, kD0, kD1);
                return ok && fmovFromFp(dst, kD0);
            }

            // (AR) **حارسُ الوسمِ العدديِّ على المسارِ العشريّ** — مرآةُ نظيرِه في x86
            //      (العلّةُ والقياسُ مفصَّلان هناك): وسمٌ غيرُ عدديٍّ مقرونٌ بـFloat كان
            //      يُجَرُّ إلى `scvtf` فيُنتج «٣٫٥» بدل قمامةٍ مرئيّة. العقدُ عينُ عقدِ
            //      `negTagged`: إجهاضٌ برمزِ ١٣٣. ولا `cmp Xn, #imm` ⇒ `sub` ثمّ
            //      `cmp x8, xzr` مرّتين (Float ثمّ Int).
            bool guardNumericTagsOnFloatPathArm64(const sir::SIRInstruction &inst)
            {
                for (size_t i = 0; i < 2; ++i)
                {
                    const sir::SIROperand &op = inst.operands[i];
                    if (!common::isBoxedAny(op))
                        continue;
                    if (!materialize(a64reg::kScratch1, op) ||
                        !ldrBase(a64reg::kX8, a64reg::kScratch1, kSadDynKindOff / kArrSlotBytes))
                        return false;
                    size_t okFloat, okInt;
                    if (!subImm(a64reg::kScratch0, a64reg::kX8, kDynKindFloat) ||
                        !cmp(a64reg::kScratch0, a64reg::kXzr) ||
                        !emitBranchFwd(a64::mnem::kBeq, "rel19", okFloat) ||
                        !subImm(a64reg::kScratch0, a64reg::kX8, kDynKindInt) ||
                        !cmp(a64reg::kScratch0, a64reg::kXzr) ||
                        !emitBranchFwd(a64::mnem::kBeq, "rel19", okInt))
                        return false;
                    if (!movz(a64reg::kX0, kNonNumericOperandPanicCode) ||
                        !movz(a64reg::kX8, kSysExitArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!patchBranchFwd(okFloat, 23, 5) || !patchBranchFwd(okInt, 23, 5))
                        return false;
                }
                return true;
            }

            bool lowerTaggedBinary(const sir::SIRInstruction &inst, int dst)
            {
                size_t isFloat, done;
                if (!computeAnyFloatFlagArm64(inst) ||
                    !emitBranchFwd(a64::mnem::kBne, "rel19", isFloat))
                    return false;
                if (!loadScalarInto(a64reg::kScratch0, inst.operands[0]) ||
                    !loadScalarInto(a64reg::kScratch1, inst.operands[1]) ||
                    !emitBinaryOp(inst, dst) || !movz(a64reg::kX8, kDynKindInt) ||
                    !emitBranchFwd(a64::mnem::kB, "rel26", done) ||
                    !patchBranchFwd(isFloat, 23, 5))
                    return false;
                if (!guardNumericTagsOnFloatPathArm64(inst) ||
                    !emitFloatArithFromIntArm64(inst.opcode, inst, dst) ||
                    !movz(a64reg::kX8, kDynKindFloat) || !patchBranchFwd(done, 25, 0))
                    return false;
                return boxScalarIntoArm64(dst, a64reg::kX8, dst);
            }

            // (AR) **`==`/`!=` لا تُجهَض** — مرآةُ نظيرِه في x86 (الحجّةُ والقياسُ هناك):
            //      وسمان مختلفان جوابُهما «غيرُ متساويين» لا خطأُ نوع.
            bool taggedEqNeShortcutArm64(const sir::SIRInstruction &inst, int dst,
                                         std::vector<size_t> &outJumps)
            {
                const long long answer = inst.opcode == sir::SIROpcode::NE ? 1 : 0;
                for (size_t i = 0; i < 2; ++i)
                {
                    const sir::SIROperand &op = inst.operands[i];
                    if (!common::isBoxedAny(op))
                        continue;
                    if (!materialize(a64reg::kScratch1, op) ||
                        !ldrBase(a64reg::kX8, a64reg::kScratch1, kSadDynKindOff / kArrSlotBytes))
                        return false;
                    size_t okFloat, okInt;
                    if (!subImm(a64reg::kScratch0, a64reg::kX8, kDynKindFloat) ||
                        !cmp(a64reg::kScratch0, a64reg::kXzr) ||
                        !emitBranchFwd(a64::mnem::kBeq, "rel19", okFloat) ||
                        !subImm(a64reg::kScratch0, a64reg::kX8, kDynKindInt) ||
                        !cmp(a64reg::kScratch0, a64reg::kXzr) ||
                        !emitBranchFwd(a64::mnem::kBeq, "rel19", okInt))
                        return false;
                    size_t toJoin;
                    if (!movz(dst, answer) || !emitBranchFwd(a64::mnem::kB, "rel26", toJoin))
                        return false;
                    outJumps.push_back(toJoin);
                    if (!patchBranchFwd(okFloat, 23, 5) || !patchBranchFwd(okInt, 23, 5))
                        return false;
                }
                return true;
            }

            static bool isEqNe(sir::SIROpcode op)
            {
                return op == sir::SIROpcode::EQ || op == sir::SIROpcode::NE;
            }

            bool lowerTaggedComparison(const sir::SIRInstruction &inst, int dst)
            {
                // (AR) الحقلُ العشريُّ يُحلُّ **قبل** الإصدار: فشلُه بعده يترك مسارًا
                //      صحيحًا مُصدَرًا وفرعًا بلا مقصد (والشرطُ الصحيحُ حُلَّ في
                //      `resolveCompareCondition` سلفًا — عقدُ التتابع نفسُه).
                long long floatField;
                if (!csetFloatInvertedField(inst.opcode, floatField))
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                size_t isFloat, done;
                if (!computeAnyFloatFlagArm64(inst) ||
                    !emitBranchFwd(a64::mnem::kBne, "rel19", isFloat))
                    return false;
                if (!prepareCompareOperands(inst) || !emitCompareResult(inst, dst) ||
                    !emitBranchFwd(a64::mnem::kB, "rel26", done) ||
                    !patchBranchFwd(isFloat, 23, 5))
                    return false;
                std::vector<size_t> eqNeJoins;
                if (isEqNe(inst.opcode) ? !taggedEqNeShortcutArm64(inst, dst, eqNeJoins)
                                        : !guardNumericTagsOnFloatPathArm64(inst))
                    return false;
                if (!loadFloatOperandPromoting(a64reg::kScratch0, inst.operands[0]) ||
                    !loadFloatOperandPromoting(a64reg::kScratch1, inst.operands[1]) ||
                    !fmovToFp(kD0, a64reg::kScratch0) || !fmovToFp(kD1, a64reg::kScratch1) ||
                    !fcmp(kD0, kD1) ||
                    !emit(a64::mnem::kCset, "x, cond",
                          {a64::Operand::R(dst), a64::Operand::I(floatField)}))
                    return false;
                if (!patchBranchFwd(done, 25, 0))
                    return false;
                for (size_t j : eqNeJoins)
                    if (!patchBranchFwd(j, 25, 0))
                        return false;
                return true;
            }

            // (AR) **الباقياتُ الستّ**، وهي صنفان لا صنفٌ واحد:
            //      (أ) القسمةُ والأرضيّةُ والباقي **تكسر التتابعَ فعلًا**: حارسُ القسمةِ على
            //          صفرٍ يسبق الإصدار، وتصحيحُ الأرضيّةِ الموقَّعةِ يُفرِّع ويُصحِّح بعده.
            //      (ب) الإزاحاتُ الثلاثُ **لا تكسره هنا**: `rrr` خالصٌ بمقدارِ سجلّ، أي
            //          الشكلُ عينُه الذي يقوده المُصدِر. تبقى خارجَ الجدول لسببٍ واحدٍ
            //          مُعلَن: نظيرُها في x86 يكسر الترتيبَ (تخصيصُ الوجهةِ **بعد** الإصدار
            //          لأنّ CL يجب أن يُحرَّر أوّلًا)، وعضويّةُ الجدولين عقدٌ واحد. فمَن
            //          يُضيف معماريّةً ثالثةً لا يستنتج من وجودِها هنا مانعًا عتاديًّا في
            //          معماريّتِه: المانعُ في x86 وحدَه، والعضويّةُ تُغيَّر في الجداول معًا.
            bool lowerBinary(const sir::SIRInstruction &inst)
            {
                using OP = sir::SIROpcode;
                if (!requireArity(inst, 2))
                    return false;
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                // (AR) جهّز المعامِلَين في x16/x17 (مُبدَّدان خارجَ الحوض) ثمّ أصدِر العمليّةَ الثلاثيّة.
                //      loadScalarInto يفكّ معاملَ Any المعلَّب (عقدُ Any) — الحسابُ يريد القيمةَ الخام.
                if (!loadScalarInto(a64reg::kScratch0, inst.operands[0]) ||
                    !loadScalarInto(a64reg::kScratch1, inst.operands[1]))
                    return false;
                switch (inst.opcode)
                {
                case OP::DIV_I64:       // (AR) قسمةٌ صحيحةٌ مقتطَعة (`/`⇒FLOOR_DIV_I64 سطحيًّا)؛ udiv للا-موقَّع.
                    // (AR) لا-موقَّعة (هيمنةً) ⇒ udiv؛ وإلّا sdiv (اقتطاعٌ نحوَ الصفر، بلا تصحيحِ أرضيّة).
                    if (!emitDivZeroGuardArm64()) // (AR) المقسومُ عليه=0 ⇒ إجهاضٌ exit(136) (تماثلٌ مع x86)
                        return false;
                    return rrr(eitherUInt64(inst) ? a64::mnem::kUdiv : a64::mnem::kSdiv,
                               dst, a64reg::kScratch0, a64reg::kScratch1) &&
                           boxIfAnyArm64(inst, dst); // (AR) نتيجةٌ Any ⇒ تعليب (وسمُ Int)؛ لا-موقَّع⇒UInt64 لا Any ⇒ no-op
                case OP::FLOOR_DIV_I64:
                {
                    // (AR) لا-موقَّعة (هيمنةً كـLLVM/x86) ⇒ udiv (الأرضيّة=الاقتطاع، بلا سالب): MAX//2=INT64_MAX.
                    if (!emitDivZeroGuardArm64()) // (AR) المقسومُ عليه=0 ⇒ إجهاضٌ exit(136)
                        return false;
                    const bool uns = eitherUInt64(inst);
                    if (!rrr(uns ? a64::mnem::kUdiv : a64::mnem::kSdiv, dst,
                             a64reg::kScratch0, a64reg::kScratch1)) // dst = حاصلٌ مقتطَع
                        return false;
                    if (uns)
                        return true;
                    // (AR) تصحيحُ أرضيّةِ الموقَّع السالب: sdiv يقتطعُ نحوَ الصفر (‑7//2=‑3)، والمرجعُ
                    //      (المفسّر/LLVM) يُنزِلُ نحوَ ‑∞ (‑4). إن (باقٍ≠0) و(إشارتا a,b مختلفتان) ⇒ dst‑=1.
                    //      a=x16, b=x17. sdiv لا يفخّ على INT64_MIN/‑1 (بخلاف idiv x86) ⇒ لا حارسَ b==-1.
                    if (!strSlot(a64reg::kScratch0, floorDivDividendSlot_)) // احفظ a (x16 سيُعادُ استعمالُه)
                        return false;
                    // باقٍ = a − q·b: msub x16 = x16(a) − dst(q)·x17(b).
                    if (!msub(a64reg::kScratch0, dst, a64reg::kScratch1, a64reg::kScratch0))
                        return false;
                    if (!cmp(a64reg::kScratch0, a64reg::kXzr)) // باقٍ vs 0
                        return false;
                    size_t skipRem;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", skipRem)) // باقٍ=0 ⇒ مضبوطٌ ⇒ تخطٍّ
                        return false;
                    if (!ldrSlot(a64reg::kScratch0, floorDivDividendSlot_)) // x16 = a
                        return false;
                    if (!rrr(a64::mnem::kEor, a64reg::kScratch0, a64reg::kScratch0, a64reg::kScratch1)) // x16 = a^b
                        return false;
                    if (!cmp(a64reg::kScratch0, a64reg::kXzr)) // (a^b) vs 0
                        return false;
                    size_t skipSign;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", skipSign)) // (a^b)≥0 ⇒ إشارتان متساويتان ⇒ تخطٍّ
                        return false;
                    if (!subImm(dst, dst, 1)) // dst ‑= 1 (أرضيّة)
                        return false;
                    if (!patchBranchFwd(skipSign, 23, 5) || !patchBranchFwd(skipRem, 23, 5))
                        return false;
                    // (AR) [عقدُ Any] dst = حاصلُ الأرضيّةِ الموقَّع؛ نوعُ FLOOR الموقَّع دومًا Any ⇒ تعليب.
                    //      حافّةُ INT64_MIN//‑1 (dst==INT64_MIN وb==‑1) تفيضُ الصحيحَ ⇒ وسمُ Float بحمولةِ
                    //      2^63 (كالمفسّر؛ المنسِّقُ ≥2^63 يرسمُها الآن)؛ وإلّا وسمُ Int بحمولةِ الحاصل.
                    //      x17=b سليمٌ، x16 خدش. نفسُ خوارزميّةِ x86 (تماثلٌ صارم).
                    if (!inst.result || inst.result->dataType != types::SadTypeKind::Any)
                        return true;
                    if (!movImm64Bits(a64reg::kScratch0, kF64SignMask) || // x16 = INT64_MIN نمطَ بتّات
                        !cmp(dst, a64reg::kScratch0))                     // dst == INT64_MIN؟
                        return false;
                    size_t skipFloat1;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", skipFloat1))
                        return false;
                    if (!movImm64Bits(a64reg::kScratch0, ~0ULL) ||        // x16 = ‑1
                        !cmp(a64reg::kScratch1, a64reg::kScratch0))       // b == ‑1؟
                        return false;
                    size_t skipFloat2;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", skipFloat2))
                        return false;
                    if (!loadFloatConst(a64reg::kScratch0, kTwoPow63) ||  // x16 = بتّاتُ 2^63
                        !movz(a64reg::kScratch1, kDynKindFloat))          // x17 = وسمُ Float
                        return false;
                    size_t skipToBox;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", skipToBox))
                        return false;
                    if (!patchBranchFwd(skipFloat1, 23, 5) || !patchBranchFwd(skipFloat2, 23, 5))
                        return false;
                    if (!movReg(a64reg::kScratch0, dst) ||                // x16 = الحاصل (حمولة)
                        !movz(a64reg::kScratch1, kDynKindInt))            // x17 = وسمُ Int
                        return false;
                    if (!patchBranchFwd(skipToBox, 25, 0))
                        return false;
                    return boxScalarIntoArm64(dst, a64reg::kScratch1, a64reg::kScratch0); // tag=x17، payload=x16
                }
                case OP::MOD_I64:
                    // (AR) الباقي = a − (a÷b)×b: s/udiv dst=الحاصل، ثمّ msub dst = x16 − dst×x17.
                    //      الهيمنةُ تختارُ udiv للا-موقَّع (MAX%2=1) مطابقةً للمفسّر ومسارِ LLVM.
                    //      نتيجةٌ Any (معاملٌ Any في السلسلة، كـ`(//)%ك`) ⇒ تعليب (وسمُ Int) — مرآةُ x86.
                    if (!emitDivZeroGuardArm64()) // (AR) المقسومُ عليه=0 ⇒ إجهاضٌ exit(136)
                        return false;
                    return rrr(eitherUInt64(inst) ? a64::mnem::kUdiv : a64::mnem::kSdiv,
                               dst, a64reg::kScratch0, a64reg::kScratch1) &&
                           msub(dst, dst, a64reg::kScratch1, a64reg::kScratch0) &&
                           boxIfAnyArm64(inst, dst);
                // (AR) الإزاحاتُ عبر lslv/lsrv/asrv بمقدارِ سجلّ (x17) ⇒ تدعم الثابتَ
                //      **والمتغيّر** (بخلاف x86 المحدودِ بالثابت وإلّا لزمه CL). SHR منطقيّة.
                case OP::SHL: return rrr(a64::mnem::kLslv, dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SHR:
                    // (AR) `>>` إشارتُه من نوعِ المعامل الأيسر (طبيعي64/بايت⇒منطقيّة lsrv، غيرُها⇒حسابيّة
                    //      asrv) مطابقةً للمفسّر ومسارِ LLVM: `-16>>2=-4` لا قيمةً ضخمة.
                    return rrr(isUnsignedType(inst.operands[0].dataType) ? a64::mnem::kLsrv : a64::mnem::kAsrv,
                               dst, a64reg::kScratch0, a64reg::kScratch1);
                case OP::SAR: return rrr(a64::mnem::kAsrv, dst, a64reg::kScratch0, a64reg::kScratch1); // (AR) حسابيّة (تمدّ الإشارة)
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // (AR) تحويلُ صحيح↔عشريّ (أحاديّ): جهّز المعاملَ في x16، حوّل عبر d0، استخرج إلى dst.
            bool lowerFloatConv(const sir::SIRInstruction &inst)
            {
                if (!requireArity(inst, 1))
                    return false;
                int dst;
                if (!allocReg(inst.result->name, dst))
                    return false;
                // (AR) معامِلُ Any يُفَكُّ بوسمِه: الخارجُ بتّاتُ double دائمًا (مرآةُ x86) ⇒
                //      I64_TO_F64 يكتفي به، وF64_TO_I64 يقتطعه. وإلّا عُومِل مؤشّرُ الخانةِ
                //      عددًا صحيحًا فحُوِّل عنوانًا («عشري(خليط[٠])» عنوانٌ لا «7.0»).
                if (common::isBoxedAny(inst.operands[0]))
                {
                    if (!loadFloatOperandInto(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    if (inst.opcode == sir::SIROpcode::I64_TO_F64)
                        return movReg(dst, a64reg::kScratch0);
                    return fmovToFp(kD0, a64reg::kScratch0) && fcvtzs(dst, kD0);
                }
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
            // (AR) حقلُ الشرطِ المقلوب لمقارنةٍ **لا-موقَّعة** (كلا المعامِلَين طبيعي64): كـcset
            //      الموقَّع لكنِ الترتيبُ لا-موقَّعٌ (LO/LS/HI/HS بدل LT/LE/GT/GE). الحقلُ=الشرط XOR 1:
            //      LT⇒LO(3)⇒2(HS)، LE⇒LS(9)⇒8(HI)، GT⇒HI(8)⇒9(LS)، GE⇒HS(2)⇒3(LO). ==,!= كالموقَّع.
            static bool csetInvertedFieldUnsigned(sir::SIROpcode op, long long &field)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: field = 1; return true;  // invert(EQ)=NE
                case OP::NE: field = 0; return true;  // invert(NE)=EQ
                case OP::LT: field = 2; return true;  // invert(LO)=HS
                case OP::LE: field = 8; return true;  // invert(LS)=HI
                case OP::GT: field = 9; return true;  // invert(HI)=LS
                case OP::GE: field = 3; return true;  // invert(HS)=LO
                default: return false;
                }
            }
            // (AR) هل كلا معامِلَي المقارنة طبيعي64 صريحٌ؟ ⇒ ترتيبٌ لا-موقَّع (مرآةُ x86 bothUInt64
            //      والمفسّر: كلاهما UInt64). الخلطُ/بايت يبقى مرفوضًا كما كان (لا توسيعَ صامتًا).
            static bool bothUInt64(const sir::SIRInstruction &cmp)
            {
                return cmp.operands.size() == 2 &&
                       cmp.operands[0].dataType == types::SadTypeKind::UInt64 &&
                       cmp.operands[1].dataType == types::SadTypeKind::UInt64;
            }
            // (AR) هل **أيُّ** معامِلٍ طبيعي64؟ ⇒ قسمة/باقٍ لا-موقَّعان (udiv لا sdiv). بوّابةُ
            //      الهيمنة (either) لا التطابق (both) — مرآةٌ حرفيّةٌ لبوّابةِ LLVM وx86 eitherUInt64.
            //      تختلفُ عن bothUInt64 قصدًا: القسمةُ دلالتُها هيمنةٌ بينما المقارنةُ تطابُق.
            static bool eitherUInt64(const sir::SIRInstruction &inst)
            {
                return inst.operands.size() == 2 &&
                       (inst.operands[0].dataType == types::SadTypeKind::UInt64 ||
                        inst.operands[1].dataType == types::SadTypeKind::UInt64);
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
                // (AR) المعامِلان عبر `loadFloatOperandInto` كنظيرَيهما في الحساب: معامِلُ
                //      Any يُفكُّ بوسمِه، وإلّا قُورِن مؤشّرُ خانةٍ ببتّاتِ double فصدر جوابٌ
                //      خاطئٌ صامت. ولا تصادمَ: الحاملان x16/x17 ومُبدَّدُ الوسمِ x8.
                return loadFloatOperandPromoting(a64reg::kScratch0, inst.operands[0]) &&
                       loadFloatOperandPromoting(a64reg::kScratch1, inst.operands[1]) &&
                       fmovToFp(kD0, a64reg::kScratch0) && fmovToFp(kD1, a64reg::kScratch1) &&
                       fcmp(kD0, kD1) &&
                       emit(a64::mnem::kCset, "x, cond", {a64::Operand::R(dst), a64::Operand::I(field)});
            }
            // (AR) **خطّافاتُ المقارنة** (م٤، الطورُ الرابع). التتابعُ في
            //   `common::LoweringDriver::driveComparison`؛ وهنا مضمونُه في AArch64.

            // (AR) حلُّ الشرط: كلا المعامِلَين طبيعي64 ⇒ cset بشرطٍ لا-موقَّع؛ وإلّا الموقَّع
            //      مع رفضِ المتبقّي. يفشل قبل تخصيصِ الوجهة (عقدُ التتابع).
            bool resolveCompareCondition(const sir::SIRInstruction &inst)
            {
                const bool unsignedCmp = bothUInt64(inst);
                if (!unsignedCmp)
                    for (const auto &op : inst.operands)
                        if (isUnsignedType(op.dataType))
                            return fail(EC::INT_NATIVE_UNSUPPORTED,
                                        diag::kCmpValueUnsigned + std::to_string(static_cast<int>(op.dataType)));
                return (unsignedCmp ? csetInvertedFieldUnsigned(inst.opcode, compareField_)
                                    : csetInvertedField(inst.opcode, compareField_)) ||
                       fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
            }

            // (AR) ثلاثيُّ العنوان: المعامِلان في مُبدَّدَين (x16/x17) ثمّ cmp.
            //      🐞 كانا يُحمَّلان بـ`materialize` فيُقارَن **مؤشّرا العلبة** لا القيمتان:
            //         `خليط[٠] == ٧` كان كاذبًا هنا وصادقًا في x86 (انجرافُ هدفَين لا عيبُ
            //         هدفٍ واحد)، بل `خليط[٠] == خليط[٠]` نفسُه كان كاذبًا لأنّ كلَّ قراءةٍ
            //         تُخصّص علبةً جديدة. مرآةُ x86 الذي يفكّها منذ عقدِ Any.
            //      ✅ والفكُّ هنا **بلا وسم** — وهو صوابٌ في موضعِه بعد الإرسالِ بالوسم: هذا
            //         الخطّافُ لا يُبلَغ اليومَ إلّا حين **لا** يكون في التعليمةِ معامِلٌ
            //         معلَّب، أو كمسارٍ صحيحٍ داخلَ `lowerTaggedComparison` بعد أن قاس
            //         الوسمَ وقرّر. وقبلَ ذلك الإرسالِ كان يُبلَغ عاريًا فتُقارَن حمولةُ
            //         وسمِ Float بتّاتِ double كعددٍ صحيحٍ ضخم: `خليط[١] > ٣` (حيث
            //         خليط[١] = ٢٫٥) «صادقة» والمفسّرُ «كاذبة».
            //      ⚠️ وما يبقى خارجَ حراسةِ الإرسال: معاملاتُ «أي» المكتوبةُ في تصريحِ
            //         دالّة. `دالة ط(أي س، أي ع)` ثمّ `س > ع` ⇒ جوابٌ **ثابتٌ لا يتبع
            //         الوسيطَين**، ويختلف بالمعماريّة (x86 «١» دائمًا وAArch64 «٠»
            //         دائمًا، والمفسّرُ «صحيح» ثمّ «خطأ»). عيبٌ **سابقٌ لهذه الدفعة**
            //         (قِيس على HEAD بالنتيجةِ عينِها) مُسجَّلٌ حالةً في
            //         prove_any_float.sh (`any_param_cmp`).
            bool prepareCompareOperands(const sir::SIRInstruction &inst)
            {
                return loadScalarInto(a64reg::kScratch0, inst.operands[0]) &&
                       loadScalarInto(a64reg::kScratch1, inst.operands[1]) &&
                       cmp(a64reg::kScratch0, a64reg::kScratch1);
            }

            // (AR) dst = ٠/١ بـcset على الشرطِ المحلول (الحقلُ مقلوبٌ كما يوجبه الترميز).
            bool emitCompareResult(const sir::SIRInstruction &, int dst)
            {
                return emit(a64::mnem::kCset, "x, cond",
                            {a64::Operand::R(dst), a64::Operand::I(compareField_)});
            }

            long long compareField_ = 0; // (AR) الشرطُ المحلولُ يعبر إلى الإصدار

            // (AR) **خطّافا الأحاديّ** (م٤، الطورُ الرابع): AArch64 ثلاثيُّ العنوان ⇒
            //      المعامِلُ في مُبدَّدٍ والوجهةُ منفصلة (وx86 يعمل في المكان).
            // (AR) مرآةُ `prepareUnaryOperand` في x86 (التفصيلُ والشاهدُ الحيُّ هناك):
            //      المعامِلُ المعلَّبُ يُفَكُّ بوسمِه، والوسمُ يبقى في x17 حتّى التعليب.
            bool prepareUnaryOperand(const sir::SIRInstruction &inst, int)
            {
                const sir::SIROperand &op = inst.operands[0];
                unaryAnyTagged_ = false;
                unaryStaticFloat_ = (op.dataType == types::SadTypeKind::Float);
                if (!common::isBoxedAny(op))
                    return materialize(a64reg::kScratch0, op);
                if (!materialize(a64reg::kScratch0, op) ||
                    !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kSadDynKindOff / kArrSlotBytes) ||
                    !ldrBase(a64reg::kScratch0, a64reg::kScratch0, kSadDynPayloadOff / kArrSlotBytes))
                    return false;
                unaryAnyTagged_ = true;
                return true;
            }

            // (AR) النفيُ بحسبِ الوسم — مرآةُ x86. و`cmp Xn,#imm` غيرُ موجودٍ في الجداول
            //      فالمقارنةُ طرحٌ ثمّ `cmp x8, xzr` (كما في `loadFloatOperandInto`).
            bool flipSignBitArm64(int dst)
            {
                return movImm64Bits(a64reg::kX8, kF64SignMask) &&
                       rrr(a64::mnem::kEor, dst, a64reg::kScratch0, a64reg::kX8);
            }

            // (AR) محوُ بتِّ الإشارةِ في **المُبدَّدِ نفسِه** (لا في الوجهة): مُستهلِكُه
            //      `logicalNotArm64` يختبر kScratch0. مرآةُ `clearSignBit` في x86.
            bool clearSignBitArm64()
            {
                return movImm64Bits(a64reg::kX8, ~kF64SignMask) &&
                       rrr(a64::mnem::kAnd, a64reg::kScratch0, a64reg::kScratch0, a64reg::kX8);
            }

            bool negTagged(int dst)
            {
                if (!unaryAnyTagged_)
                    return unaryStaticFloat_ ? flipSignBitArm64(dst)
                                             : rrr(a64::mnem::kSub, dst, 31, a64reg::kScratch0);
                if (!subImm(a64reg::kX8, a64reg::kScratch1, kDynKindFloat) ||
                    !cmp(a64reg::kX8, a64reg::kXzr))
                    return false;
                size_t isFloat, done;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", isFloat))
                    return false;
                // (AR) ليس عشريًّا ⇒ يجبُ أن يكون صحيحًا؛ وما عداهما يُجهِض (مرآةُ x86).
                size_t isInt;
                if (!subImm(a64reg::kX8, a64reg::kScratch1, kDynKindInt) ||
                    !cmp(a64reg::kX8, a64reg::kXzr) ||
                    !emitBranchFwd(a64::mnem::kBeq, "rel19", isInt))
                    return false;
                if (!movz(a64reg::kX0, kNonNumericOperandPanicCode) ||
                    !movz(a64reg::kX8, kSysExitArm64) || !emit(a64::mnem::kSvc, "", {}))
                    return false;
                if (!patchBranchFwd(isInt, 23, 5))
                    return false;
                if (!rrr(a64::mnem::kSub, dst, 31, a64reg::kScratch0) ||
                    !emitBranchFwd(a64::mnem::kB, "rel26", done))
                    return false;
                if (!patchBranchFwd(isFloat, 23, 5))
                    return false;
                if (!flipSignBitArm64(dst))
                    return false;
                return patchBranchFwd(done, 25, 0); // (AR) b غيرُ مشروط: imm26@25-0
            }

            // (AR) النفيُ المنطقيُّ (`ليس`) — مرآةُ `logicalNot` في x86. اختبارٌ صفريٌّ مجرّدٌ،
            //      ومن يَفرِزُ ما يصلحُ له هي **قائمةُ السماحِ** عند `case OP::NOT` وحدَها
            //      (الحجّةُ والقياسُ في نظيرتِها في x86).
            bool logicalNotArm64(int dst)
            {
                size_t isZero, done;
                if (!cmp(a64reg::kScratch0, a64reg::kXzr) ||
                    !emitBranchFwd(a64::mnem::kBeq, "rel19", isZero))
                    return false;
                if (!movz(dst, 0) || !emitBranchFwd(a64::mnem::kB, "rel26", done))
                    return false;
                if (!patchBranchFwd(isZero, 23, 5))
                    return false;
                if (!movz(dst, 1))
                    return false;
                return patchBranchFwd(done, 25, 0); // (AR) b غيرُ مشروط: imm26@25-0
            }

            // (AR) تعليبُ ناتجِ الأحاديّ بوسمِ **مدخلِه** حين كان معلَّبًا — لا بوسمِ Int
            //      المفترَضِ في boxIfAnyArm64. مرآةُ x86.
            bool boxUnaryResult(const sir::SIRInstruction &inst, int dst)
            {
                if (!inst.result || inst.result->dataType != types::SadTypeKind::Any)
                    return true;
                if (unaryAnyTagged_)
                    return boxScalarIntoArm64(dst, a64reg::kScratch1, dst);
                // (AR) ولا فرعَ للعشريِّ الساكن — مرآةُ x86 (تفصيلُ سببِ عدمِ بلوغِه هناك).
                return boxIfAnyArm64(inst, dst);
            }

            bool emitUnaryOp(const sir::SIRInstruction &inst, int dst)
            {
                using OP = sir::SIROpcode;
                switch (inst.opcode)
                {
                case OP::NEG: // (AR) −a = xzr − a (متمّمٌ ثنائيّ)، أو قلبُ بتِّ الإشارةِ للعشريّ
                    return negTagged(dst);
                // (AR) القلبُ البتّيُّ على معامِلٍ معلَّبٍ يُرفَض ترجمةً لا يُخمَّن (انظر x86)،
                //      وتمييزُ المنطقيِّ من البتّيِّ بنوعِ النتيجةِ — مرآةُ x86.
                case OP::NOT:
                {
                    const bool isLogical =
                        inst.result && inst.result->dataType == types::SadTypeKind::Boolean;
                    if (unaryAnyTagged_)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    if (!isLogical)
                        return emit(a64::mnem::kMvn, "x, x",
                                    {a64::Operand::R(dst), a64::Operand::R(a64reg::kScratch0)});
                    // (AR) قائمةُ سماحٍ لا قائمةَ منع — الحجّةُ والقياسُ (‑0.0 والعدمُ
                    //      والمصفوفةُ الفارغة) مفصَّلةٌ في نظيرتِها في x86.
                    switch (inst.operands[0].dataType)
                    {
                    case types::SadTypeKind::Integer:
                    case types::SadTypeKind::Boolean:
                    case types::SadTypeKind::Byte:
                    case types::SadTypeKind::UInt64:
                    case types::SadTypeKind::Int8:
                    case types::SadTypeKind::Int16:
                    case types::SadTypeKind::Int32:
                    case types::SadTypeKind::Int64:
                    case types::SadTypeKind::UInt8:
                    case types::SadTypeKind::UInt16:
                    case types::SadTypeKind::UInt32:
                        return logicalNotArm64(dst);
                    // (AR) والعروضُ المسمّاةُ معه (قيمٌ مستقلّةٌ في التعداد): أوّلُ تمريرةٍ
                    //      تُطبّع `Float`⇒`Float64` تجعل `ليس 2.5` فشلَ ترجمةٍ لو غابت.
                    case types::SadTypeKind::Float:
                    case types::SadTypeKind::Float32:
                    case types::SadTypeKind::Float64:
                        return clearSignBitArm64() && logicalNotArm64(dst);
                    default:
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    }
                }
                // (AR) لا يُبلَغ: الموزِّعُ لا يوجّه إلى هنا إلّا الاثنين.
                default: return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // (AR) مُساعِداتُ زمنِ تشغيلِ الخريطةِ أصليًّا — مرآةُ x86 (الحجّةُ والتخطيطُ
            //      وحدودُ الدعمِ موثَّقةٌ هناك). التخطيطُ من مصدرِ الحقيقة عينِه، فلا
            //      يتباعد الهدفان. المُنفَّذُ: الإنشاءُ والحجمُ فقط.
            // (EN) Native map runtime helpers — mirror of x86 (rationale, layout and the
            //      limits of support are documented there). Same SoT layout, so the two
            //      targets cannot drift. Implemented: create and size only.
            // (AR) **بحثٌ خطّيٌّ عن خانةِ مفتاحٍ في الخريطة** — مرآةُ نظيرِه في x86 حرفيًّا
            //      (العقدُ والعلّةُ مفصَّلان هناك): مسحٌ على السعة، ومفتاحٌ عدمٌ ⇒ خانةٌ فارغة.
            //      الداخل: x0=القاعدة · x1=مؤشّرُ المفتاح. الخارج: x4=فهرسُ الخانة ·
            //      x5=١ إن كان موجودًا و٠ إن كانت فارغة · x3=مصفوفةُ المفاتيح · x2=السعة.
            //      يدهس x2..x8 وx16 ⇒ داخلَ سياجِ انسكابٍ فقط. و`panicWhenFull` يفصل
            //      عقدَ الإدراجِ (لا بديلَ عن الإجهاض) عن عقدِ البحثِ (جوابُه «غيرُ موجود»).
            bool emitMapFindSlotArm64(bool panicWhenFull)
            {
                namespace rep = types::repr;
                if (!ldrBase(2, 0, rep::kMapFieldCapacity) || !ldrBase(3, 0, rep::kMapFieldKeys) ||
                    !movz(4, 0))
                    return false;
                const size_t scan = code_.size();
                if (!cmp(4, 2))
                    return false;
                size_t full;
                if (!emitBranchFwd(a64::mnem::kBge, "rel19", full))
                    return false;
                // (AR) x5 = المفاتيح[x4] — العنوانُ بـadd Xd,Xn,Xm,LSL#3 ثمّ تحميلٌ بإزاحةِ صفر.
                if (!addLsl3(a64reg::kScratch0, 3, 4) || !ldrBase(5, a64reg::kScratch0, 0) ||
                    !cmp(5, a64reg::kXzr))
                    return false;
                size_t empty;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", empty))
                    return false;
                // (AR) مقارنةُ نصَّين بايتًا بايتًا حتّى NUL (لا strcmp — لا libc).
                if (!movReg(6, 1))
                    return false;
                const size_t cmpLoop = code_.size();
                if (!ldrb(7, 5) || !ldrb(a64reg::kX8, 6) || !cmp(7, a64reg::kX8))
                    return false;
                size_t differ;
                if (!emitBranchFwd(a64::mnem::kBne, "rel19", differ) || !cmp(7, a64reg::kXzr))
                    return false;
                size_t hit;
                if (!emitBranchFwd(a64::mnem::kBeq, "rel19", hit))
                    return false;
                if (!addImm(5, 5, 1) || !addImm(6, 6, 1) || !emitBBack(cmpLoop))
                    return false;
                if (!patchBranchFwd(differ, 23, 5))
                    return false;
                if (!addImm(4, 4, 1) || !emitBBack(scan))
                    return false;
                if (!patchBranchFwd(full, 23, 5))
                    return false;
                if (panicWhenFull &&
                    (!movz(a64reg::kX0, kMapOverflowPanicCode) ||
                     !movz(a64reg::kX8, kSysExitArm64) || !emit(a64::mnem::kSvc, "", {})))
                    return false;
                // (AR) بحثٌ على خريطةٍ ممتلئةٍ ⇒ يسقط إلى «غيرُ موجود» (x5=٠).
                if (!patchBranchFwd(empty, 23, 5) || !movz(5, 0))
                    return false;
                size_t done;
                if (!emitBranchFwd(a64::mnem::kB, "rel26", done))
                    return false;
                if (!patchBranchFwd(hit, 23, 5) || !movz(5, 1))
                    return false;
                return patchBranchFwd(done, 25, 0);
            }

            bool emitMapHelper(const sir::SIRInstruction &inst, const sir::SIRBasicBlock &block,
                               size_t instIdx, bool &handled)
            {
                namespace rep = types::repr;
                handled = false;
                const std::string &fname = inst.operands[0].name;
                // (AR) [RFC عقد الغياب — درسُ «ثلاث نسخ»] مرآةُ نظيرِه في x86:
                //      قناةُ `_method` مرادفٌ تنفيذيٌّ تامٌّ، وحارسُ «لكل»
                //      لا-عمليّةَ معلَنةً (المسارُ غيرُ موسومٍ فلا غيابَ يُحرَس).
                // (EN) Mirror of the x86 twin: the `_method` channel is an exact
                //      execution synonym; the foreach guard is a declared no-op.
                if (fname == Sad::Compiler::kRuntimeForeachAbsenceGuard)
                {
                    handled = true;
                    return true;
                }
                const bool isCreate = (fname == Sad::Compiler::kRuntimeMapCreate);
                const bool isSize = (fname == Sad::Compiler::kRuntimeMapSize);
                const bool isSet = (fname == Sad::Compiler::kRuntimeMapSetTyped ||
                                    fname == Sad::Compiler::kRuntimeMapSetTypedMethod);
                const bool isGet = (fname == Sad::Compiler::kRuntimeMapGetI64);
                const bool isHas = (fname == Sad::Compiler::kRuntimeMapHas);
                if (!isCreate && !isSize && !isSet && !isGet && !isHas)
                    return true;
                handled = true;
                // (AR) الإسنادُ وحدَه بلا نتيجة؛ وما عداه يجب أن يُنتج قيمة.
                if (!inst.result && !isSet)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));

                auto deliver = [&]() -> bool {
                    int dst;
                    return allocReg(inst.result->name, dst) && movReg(dst, 0);
                };
                // (AR) سياجُ الانسكاب — الشرطُ نفسُه المستعمَلُ في النداءِ العامّ (حيٌّ **أو**
                //      وسيطٌ سجليٌّ لهذا النداء): الوسيطُ يُقرأ من خانتِه بعدَ دهسِ الحوض.
                auto spillLive = [&]() -> bool {
                    for (const auto &kv : regOf_)
                        if ((common::usedAfterInBlock(block, instIdx, kv.first) ||
                             common::isPoolArgOfCall(inst, kv.first,
                                                     [this](const std::string &n) { return isMemName(n); })) &&
                            !spillReg(kv.second))
                            return false;
                    return true;
                };
                auto reloadLive = [&]() -> bool {
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) && !reloadReg(kv.second))
                            return false;
                    return true;
                };

                // (AR) خ[م] = ق — مرآةُ x86 (العقدُ ودَينُ المفتاحِ غيرِ المنسوخِ منصوصان هناك).
                if (isSet)
                {
                    if (inst.operands.size() != 5)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) مرآةُ x86: النصُّ قيمةً غيرُ مدعومٍ بعد، ويُسمّى باسمه.
                    if (inst.operands[3].dataType == types::SadTypeKind::String ||
                        strReg_.count(inst.operands[3].name))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    diag::kMapValueUnsupported + inst.operands[3].name);
                    if (!spillLive() || !materialize(0, inst.operands[1]) ||
                        !materializeString(inst.operands[2], 1, true) ||
                        !emitMapFindSlotArm64(/*panicWhenFull=*/true) || !cmp(5, a64reg::kXzr))
                        return false;
                    size_t keyStored;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", keyStored))
                        return false;
                    if (!addLsl3(a64reg::kScratch0, 3, 4) || !strBase(1, a64reg::kScratch0, 0) ||
                        !ldrBase(a64reg::kScratch1, 0, rep::kMapFieldCount) ||
                        !addImm(a64reg::kScratch1, a64reg::kScratch1, 1) ||
                        !strBase(a64reg::kScratch1, 0, rep::kMapFieldCount))
                        return false;
                    if (!patchBranchFwd(keyStored, 23, 5))
                        return false;
                    const long long fields[2] = {rep::kMapFieldValues, rep::kMapFieldTypes};
                    for (int i = 0; i < 2; ++i)
                    {
                        if (!loadArgInto(7, inst.operands[3 + i]) || !ldrBase(3, 0, fields[i]) ||
                            !addLsl3(a64reg::kScratch0, 3, 4) || !strBase(7, a64reg::kScratch0, 0))
                            return false;
                    }
                    return reloadLive();
                }

                // (AR) خ[م] قراءةً صحيحةً · و«يحوي» — الخانةُ الفارغةُ مصفَّرةٌ بـmmap ⇒ صفرٌ بلا فرع.
                if (isGet || isHas)
                {
                    if (!requireArity(inst, 3))
                        return false;
                    if (!spillLive() || !materialize(0, inst.operands[1]) ||
                        !materializeString(inst.operands[2], 1, true) ||
                        !emitMapFindSlotArm64(/*panicWhenFull=*/false))
                        return false;
                    if (isHas)
                    {
                        if (!movReg(0, 5))
                            return false;
                    }
                    else
                    {
                        // (AR) مرآةُ x86: الفرعُ على وجودِ المفتاح، لا على تصفيرِ mmap —
                        //      فهرسُ «لم يُوجَد» على خريطةٍ ممتلئةٍ يقرأ مصفوفةَ الوسوم.
                        if (!cmp(5, a64reg::kXzr))
                            return false;
                        size_t hit;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", hit) || !movz(0, 0))
                            return false;
                        size_t end;
                        if (!emitBranchFwd(a64::mnem::kB, "rel26", end))
                            return false;
                        if (!patchBranchFwd(hit, 23, 5) || !ldrBase(3, 0, rep::kMapFieldValues) ||
                            !addLsl3(a64reg::kScratch0, 3, 4) || !ldrBase(0, a64reg::kScratch0, 0))
                            return false;
                        if (!patchBranchFwd(end, 25, 0))
                            return false;
                    }
                    return reloadLive() && deliver();
                }

                if (isSize)
                {
                    if (!requireArity(inst, 2))
                        return false;
                    return materialize(0, inst.operands[1]) &&
                           ldrBase(0, 0, rep::kMapFieldCount) &&
                           deliver();
                }

                // (AR) مرآةُ x86 حرفيًّا (العلّةُ والتقديرُ الزائدُ منصوصان هناك).
                long long hint = 0;
                if (inst.operands.size() >= 2 && !common::isConstInt(inst.operands[1], hint))
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                diag::kMapDynamicCapacity + inst.operands[1].name);
                long long cap = (hint + mapSetCount_) * rep::kMapGrowthFactor;
                if (cap < rep::kMapMinCapacity)
                    cap = rep::kMapMinCapacity;
                const long long total = rep::kMapHeaderBytes + 3 * cap * rep::kMapSlotBytes;
                for (const auto &kv : regOf_)
                    if (common::usedAfterInBlock(block, instIdx, kv.first))
                        if (!spillReg(kv.second))
                            return false;
                if (!emitMmapArm64(total)) // (AR) x0 = القاعدة (مصفَّرة)
                    return false;
                if (!movz(a64reg::kScratch0, cap) ||
                    !strBase(a64reg::kScratch0, 0, rep::kMapFieldCapacity))
                    return false;
                const long long fields[3] = {rep::kMapFieldKeys, rep::kMapFieldValues, rep::kMapFieldTypes};
                for (int i = 0; i < 3; ++i)
                {
                    const long long off = rep::kMapHeaderBytes + i * cap * rep::kMapSlotBytes;
                    // (AR) `addImm` لا يفحص المدى، و`encodeFixed32` **يرمي** عند تجاوزِ
                    //      imm12 (٤٠٩٥) — استثناءٌ غيرُ ملتقَطٍ يُنهي المترجمَ بلا تشخيص،
                    //      بينما x86 (imm32) يترجم الحرفيّةَ نفسَها ⇒ انجرافُ هدفَين.
                    //      فالفحصُ هنا يحوّله إلى إخفاقِ ترجمةٍ مُسمّى.
                    // (EN) addImm does not range-check and the encoder THROWS past imm12,
                    //      killing the compiler with no diagnostic while x86 compiles the
                    //      same literal. Check explicitly and fail with a named diagnostic.
                    if (off > a64reg::kImm12Max)
                        return fail(EC::INT_NATIVE_IMM_RANGE,
                                    diag::kMapCapacityImm12 + std::to_string(cap));
                    if (!addImm(a64reg::kScratch0, 0, off) ||
                        !strBase(a64reg::kScratch0, 0, fields[i]))
                        return false;
                }
                for (const auto &kv : regOf_)
                    if (common::usedAfterInBlock(block, instIdx, kv.first))
                        if (!reloadReg(kv.second))
                            return false;
                return deliver();
            }

            bool lowerInstruction(const sir::SIRInstruction &inst,
                                  const sir::SIRBasicBlock &block, size_t instIdx)
            {
                using OP = sir::SIROpcode;
                switch (inst.opcode)
                {
                case OP::MOVE:
                {
                    if (!requireArity(inst, 1))
                        return false;
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
                        return movConst(dst, c);
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
                case OP::AND:
                case OP::OR:
                case OP::XOR:
                case OP::ADD_F64:
                case OP::SUB_F64:
                case OP::MUL_F64:
                case OP::DIV_F64:
                    return driveBinary(inst); // (AR) العشرُ ذاتُ التتابعِ المشترك
                case OP::MOD_I64:
                case OP::DIV_I64:
                case OP::FLOOR_DIV_I64:
                case OP::SHL:
                case OP::SHR:
                case OP::SAR:
                    return lowerBinary(inst); // (AR) الستُّ ذاتُ التتابعِ الخاصّ (حارسُ القسمة…)
                case OP::I64_TO_F64:
                case OP::F64_TO_I64:
                    return lowerFloatConv(inst);
                case OP::F64_TO_I64_SAT:
                {
                    // (AR) عشريّ ⇒ صحيح **مُشبَّع** (llvm.fptosi.sat): على ARMv8 تعليمةُ fcvtzs نفسُها
                    //      مُشبِّعةٌ بالتعريف (NaN→٠، +طفح→INT64_MAX، −طفح→INT64_MIN) ⇒ لا حاجةَ لمعالجةٍ
                    //      يدويّةٍ كـx86. المعاملُ الصحيحُ/المنطقيّ (تمريرٌ) = هويّة.
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (inst.operands[0].dataType == types::SadTypeKind::Integer ||
                        inst.operands[0].dataType == types::SadTypeKind::Boolean)
                        return materialize(dst, inst.operands[0]);
                    return materialize(a64reg::kScratch0, inst.operands[0]) &&
                           fmovToFp(kD0, a64reg::kScratch0) && fcvtzs(dst, kD0);
                }
                case OP::BUILTIN_SQRT:
                {
                    // (AR) جذر(س) ⇒ عشريّ. المعاملُ عشريٌّ ⇒ بتّاته double (fmov إلى FP)؛ صحيحٌ ⇒ حوّله
                    //      double (scvtf). ثمّ fsqrt (أساسيٌّ ARMv8) ⇒ أعِد البتّاتِ إلى GPR.
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    //      ومعامِلُ Any لا يُحسَم نوعُه ساكنًا ⇒ يُفَكُّ بوسمِه إلى بتّاتِ double.
                    if (common::isBoxedAny(inst.operands[0]))
                    {
                        if (!loadFloatOperandInto(dst, inst.operands[0]) || !fmovToFp(kD0, dst))
                            return false;
                        return fsqrt(kD0, kD0) && fmovFromFp(dst, kD0);
                    }
                    if (!materialize(dst, inst.operands[0]))
                        return false;
                    if (inst.operands[0].dataType == types::SadTypeKind::Float)
                    {
                        if (!fmovToFp(kD0, dst))
                            return false;
                    }
                    else if (!scvtf(kD0, dst)) // (AR) صحيح ⇒ double قبلَ الجذر
                        return false;
                    return fsqrt(kD0, kD0) && fmovFromFp(dst, kD0);
                }
                case OP::BUILTIN_ABS:
                {
                    // (AR) مطلق(س): نوعُ النتيجة = نوعُ المعامل. العشريُّ = مسحُ بتِّ الإشارة (AND بقناع
                    //      0x7FFF…FFFF؛ العشريُّ بتّاتٌ في GPR). الصحيحُ = نفيٌ شرطيّ لا فرعيّ: t=−x؛
                    //      cmp x,0؛ csel x = (x<0)؟ t : x = |x| (آمنٌ لـINT64_MIN لأنّ csel لا يطفح).
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (!materialize(dst, inst.operands[0]))
                        return false;
                    if (inst.result->dataType == types::SadTypeKind::Float)
                        return movImm64Bits(a64reg::kScratch0, kF64AbsMask) &&
                               rrr(a64::mnem::kAnd, dst, dst, a64reg::kScratch0);
                    return rrr(a64::mnem::kSub, a64reg::kScratch0, a64reg::kXzr, dst) && // scratch = −dst
                           cmp(dst, a64reg::kXzr) && cselLt(dst, a64reg::kScratch0, dst);
                }
                case OP::BUILTIN_MIN:
                case OP::BUILTIN_MAX:
                {
                    // (AR) أصغر/أكبر (صحيحان موقَّعان، نتيجةٌ Integer): حمّل a في dst وb في x16، قارِنْ،
                    //      ثمّ اختيارٌ شرطيّ — أكبر: csel dst=(dst>x16)؟dst:x16؛ أصغر: (dst<x16)؟dst:x16.
                    if (!requireArity(inst, 2))
                        return false;
                    const bool aFloat = inst.operands[0].dataType == types::SadTypeKind::Float;
                    const bool bFloat = inst.operands[1].dataType == types::SadTypeKind::Float;
                    // (AR) المختلطُ (صحيحٌ×عشريّ) يُرفَضُ صراحةً (مرآةُ x86؛ دلالةُ نوعِ الفائزِ تتباعد).
                    if (aFloat != bFloat)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kBuiltinFloatMinMax + detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (!materialize(dst, inst.operands[0]) ||               // dst = a
                        !materialize(a64reg::kScratch0, inst.operands[1]))   // x16 = b
                        return false;
                    if (aFloat)
                    {
                        // (AR) عشريّان: نطابقُ المفسّرَ عبرَ fcmp المرتَّب + cselGt (false على NaN/unordered ⇒
                        //      يُبقي a المعامَلَ الأوّلَ = دلالةُ المفسّرِ للـNaN/التعادل). لا fmin/fmaxnm (دلالةُ
                        //      NaN مختلفة). أكبر: fcmp(b,a) ⇒ (b>a)?b:a؛ أصغر: fcmp(a,b) ⇒ (a>b)?b:a=(b<a)?b:a.
                        if (!fmovToFp(kD0, dst) || !fmovToFp(kD1, a64reg::kScratch0))
                            return false;
                        bool ok = inst.opcode == OP::BUILTIN_MAX ? fcmp(kD1, kD0)  // (b, a)
                                                                : fcmp(kD0, kD1); // (a, b)
                        return ok && cselGt(dst, a64reg::kScratch0, dst); // dst = GT ? b : a
                    }
                    // (AR) صحيحان: cmp + اختيارٌ شرطيّ. كلاهما طبيعي64 (لا-موقَّع) ⇒ csel hi/lo؛ وإلّا موقَّع
                    //      ⇒ csel gt/lt. اللا-موقَّعُ يطابقُ المفسّرَ (ctx.argType==UInt64 للمعامِلَين) ومرآةُ x86.
                    //      أكبر: (dst>x16)؟dst:x16؛ أصغر: (dst<x16)؟dst:x16. النوعُ من الأمامِ عبرَ resolveSurfaceType.
                    const bool bothU64 =
                        inst.operands[0].dataType == types::SadTypeKind::UInt64 &&
                        inst.operands[1].dataType == types::SadTypeKind::UInt64;
                    if (!cmp(dst, a64reg::kScratch0))
                        return false;
                    if (bothU64)
                        return inst.opcode == OP::BUILTIN_MAX ? cselHi(dst, dst, a64reg::kScratch0)
                                                             : cselLo(dst, dst, a64reg::kScratch0);
                    return inst.opcode == OP::BUILTIN_MAX ? cselGt(dst, dst, a64reg::kScratch0)
                                                         : cselLt(dst, dst, a64reg::kScratch0);
                }
                case OP::PHI:
                {
                    // (AR) PHI: لا شيفرة — القيمةُ تعيش في خانةِ الإطار (السَّلَفُ يخزّن، الدامجُ يقرأ).
                    if (!inst.result || memSlot_.find(inst.result->name) == memSlot_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kPhiUnslotted + detailOpcode(inst));
                    return true;
                }
                case OP::NULL_ASSERT: // (AR) تأكيدُ عدمٍ مؤكَّد T؟→T: هويّة (حارسُ العدمِ زمنَ التشغيل مستقلّ).
                case OP::BOOL_TO_I64: // (AR) منطقيّ (٠/١) ⇒ صحيح: هويّة.
                case OP::CAST:        // (AR) تحويلٌ بلا تغييرِ تمثيلٍ (i64→i64): هويّة.
                {
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return materialize(dst, inst.operands[0]);
                }
                case OP::I64_TO_BOOL:
                {
                    // (AR) صحيح ⇒ منطقيّ (٠/١): cmp scratch,xzr؛ cset dst,ne (غيرُ صفرٍ ⇒ ١). الحقلُ ٠=ne.
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return materialize(a64reg::kScratch0, inst.operands[0]) &&
                           cmp(a64reg::kScratch0, a64reg::kXzr) &&
                           emit(a64::mnem::kCset, "x, cond", {a64::Operand::R(dst), a64::Operand::I(0)});
                }
                case OP::ADDR:
                {
                    // (AR) عنوانُ متغيّرِ إطارٍ (ALLOC/معامل): add dst, sp, #(فهرس×٨). عنوانُ مؤشّرِ حوضٍ غيرُ مدعوم.
                    if (!requireArity(inst, 1))
                        return false;
                    int slot;
                    if (!isMemVar(inst.operands[0], slot))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kAddrNonslot + detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return addImm(dst, a64reg::kSp, static_cast<long long>(slot) * kArrSlotBytes);
                }
                case OP::ROL:
                {
                    // (AR) دورانٌ يسارًا (يتيمٌ سطحيًّا؛ صحيحٌ للاكتمال): ROL n = ROR (64−n). مقدارٌ ثابتٌ فقط.
                    //      movz tmp,#(64−n)؛ rorv dst, src, tmp. n∈[0,63] (n=0 ⇒ 64 mod 64=0 = هويّة).
                    if (!requireArity(inst, 2))
                        return false;
                    long long n;
                    if (!common::isConstInt(inst.operands[1], n))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kRolVar + detailOpcode(inst));
                    if (n < 0 || n > 63)
                        return fail(EC::INT_NATIVE_IMM_RANGE, diag::kShift + std::to_string(n));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return materialize(a64reg::kScratch0, inst.operands[0]) &&
                           movz(a64reg::kScratch1, 64 - n) &&
                           rrr(a64::mnem::kRorv, dst, a64reg::kScratch0, a64reg::kScratch1);
                }
                // ── نواة الكومة الأصليّة (الدفعة ٢): مؤشّرات + تخصيص/تحرير + نسخ/ملء الذاكرة ──
                case OP::PTR_CAST:
                {
                    // (AR) تحويلُ مؤشّر (i64↔ptr): هويّةٌ في نموذجِ القيمة-في-سجلّ (لا تغييرَ تمثيل).
                    if (!requireArity(inst, 1))
                        return false;
                    if (!materialize(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::PTR_ADD:
                {
                    // (AR) حسابُ مؤشّرٍ ببايتات: result = ptr + offset (عبر add Xd,Xn,Xm ⇒ أيُّ إزاحةٍ ٦٤-بت).
                    if (!requireArity(inst, 2))
                        return false;
                    if (!materialize(a64reg::kScratch0, inst.operands[0]) ||
                        !materialize(a64reg::kScratch1, inst.operands[1]) ||
                        !rrr(a64::mnem::kAdd, a64reg::kScratch0, a64reg::kScratch0, a64reg::kScratch1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::ALLOC_HEAP:
                case OP::FFI_MALLOC:
                {
                    // (AR) حجز(size): mmap(size+16)؛ اكتب [base]=توقيع، [base+8]=الحجمُ الكلّيّ؛ أرجِع
                    //      base+16. نَنسِك المؤقّتاتِ الحيّةَ/المعاملاتِ حولَ svc ثمّ نعيدها.
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) x1 = الحجمُ الكلّيّ = size + 16 (من خانةِ الانسكاب/ثابت ⇒ لا تصادمَ نقلٍ متوازٍ).
                    if (!loadArgInto(a64reg::kX1, inst.operands[0]) ||
                        !addImm(a64reg::kX1, a64reg::kX1, kHeapHdrBytes))
                        return false;
                    if (!emitMmapArm64PresetSize()) // (AR) x0=base؛ x1 يبقى (لا svc ولا النواةُ يدهسانه)
                        return false;
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes))
                        return false;
                    if (!strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes)) // (AR) الحجمُ الكلّيّ
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes)) // (AR) المؤشّرُ المُعاد = base + 16
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
                case OP::FREE:
                case OP::FFI_FREE:
                {
                    // (AR) حرر(ptr): base=ptr-16؛ إن طابق [base] التوقيعَ فـmunmap(base, [base+8]) وإلّا
                    //      يتخطّى دون فكِّ منطقةٍ عشوائيّة (قيدُ أميليا ٢: مصفوفةُ ARRAY_NEW الأجنبيّةُ ⇒
                    //      base داخل منطقتها المخطَّطة ⇒ التوقيعُ لا يطابق ⇒ تخطٍّ). ⚠️ الحارسُ يقرأ
                    //      [ptr−16]، فمؤشّرٌ حيث ptr−16 غيرُ مخطَّط (مكدّسٌ محاذٍ لحدّ صفحة/حرٌّ مزدوجٌ فُكّت
                    //      منطقتُه) ⇒ SIGSEGV صريحٌ (فشلٌ حادّ لا إفسادٌ صامت) — متأصِّلٌ في كشفٍ بلا سجلٍّ حرّ.
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) x9 = base = ptr − 16.
                    if (!loadArgInto(9, inst.operands[0]) || !subImm(9, 9, kHeapHdrBytes))
                        return false;
                    if (!ldrBase(10, 9, kHeapMagicOff / kArrSlotBytes) ||
                        !movImm64Bits(11, static_cast<unsigned long long>(kHeapMagic)) || !cmp(10, 11))
                        return false;
                    size_t skip;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", skip)) // (AR) لا يطابق ⇒ تخطَّ munmap
                        return false;
                    // (AR) munmap(x0=base, x1=len=[base+8])، x8=215.
                    if (!ldrBase(a64reg::kX1, 9, kHeapSizeOff / kArrSlotBytes) || !movReg(a64reg::kX0, 9) ||
                        !movz(a64reg::kX8, kSysMunmapArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!patchBranchFwd(skip, 23, 5)) // (AR) b.ne imm19@23-5
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    if (inst.result) // (AR) FFI_FREE عديمةُ النتيجة عمومًا؛ حارسٌ دفاعيّ إن وُجِدت.
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movz(dst, 0);
                    }
                    return true;
                }
                case OP::MEMSET:
                case OP::FFI_MEMSET:
                case OP::BUILTIN_MEM_SET:
                {
                    // (AR) عبّئ(dest, val, size): حلقةُ بايتاتٍ بفحصِ size=0 **في الرأس** (لا do-while ⇒
                    //      لا التفافَ 2^64). dest→x9، val→x10 (البايتُ الأدنى)، size→x11 (عدّاد).
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!loadArgInto(9, inst.operands[0]) || !loadArgInto(10, inst.operands[1]) ||
                        !loadArgInto(11, inst.operands[2]))
                        return false;
                    const size_t head = code_.size();
                    if (!cmp(11, a64reg::kXzr))
                        return false;
                    size_t done;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", done)) // (AR) size==0 ⇒ اخرج قبل الجسد
                        return false;
                    if (!strb(10, 9) || !addImm(9, 9, 1) || !subImm(11, 11, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(done, 23, 5))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    if (inst.result) // (AR) FFI_MEMSET تُرجع dest؛ BUILTIN/بارز بلا نتيجة.
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return loadArgInto(dst, inst.operands[0]);
                    }
                    return true;
                }
                case OP::MEMCPY:
                case OP::FFI_MEMCPY:
                case OP::BUILTIN_MEM_COPY:
                {
                    // (AR) انسخ(dest, src, size): حلقةُ بايتاتٍ أماميّةٌ بفحصِ size=0 في الرأس. dest→x9،
                    //      src→x10، size→x11، البايتُ عبر x12 (ldrb الجديدة). دلالةُ memcpy (تراكبٌ غيرُ
                    //      معرَّف، ليست memmove).
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!loadArgInto(9, inst.operands[0]) || !loadArgInto(10, inst.operands[1]) ||
                        !loadArgInto(11, inst.operands[2]))
                        return false;
                    const size_t head = code_.size();
                    if (!cmp(11, a64reg::kXzr))
                        return false;
                    size_t done;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", done))
                        return false;
                    if (!ldrb(12, 10) || !strb(12, 9) || !addImm(9, 9, 1) || !addImm(10, 10, 1) ||
                        !subImm(11, 11, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(done, 23, 5))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    if (inst.result) // (AR) FFI_MEMCPY تُرجع dest.
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return loadArgInto(dst, inst.operands[0]);
                    }
                    return true;
                }
                // ── النصوصُ الديناميّة (الدفعة ٣): تمثيلُ القيمة = مؤشّرُ i8* منتهٍ بـNUL (مرآةُ x86) ──
                case OP::STRING_NEW:
                {
                    // (AR) إنشاءُ نصّ: يُجسّد المعاملَ قيمةً زمنَ تشغيلٍ (مؤشّرَ i8*). لا تخصيصَ كومة،
                    //      ولا دهسَ حوضٍ (dst/الخدشُ فقط).
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return materializeString(inst.operands[0], dst, false);
                }
                case OP::BOOL_TO_STRING:
                {
                    // (AR) منطقيّ → نصّ: ≠٠ ⇒ عنوانُ «صحيح»، وإلّا «خطأ». مؤشّرُ i8*. الخدشُ x16/x17 فقط.
                    if (!requireArity(inst, 1))
                        return false;
                    if (!materialize(a64reg::kScratch0, inst.operands[0]) ||
                        !cmp(a64reg::kScratch0, a64reg::kXzr))
                        return false;
                    size_t isFalse;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", isFalse))
                        return false;
                    if (!emitLoadCStrAddr(a64reg::kScratch1, kDynBoolTrueText))
                        return false;
                    size_t doneW;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", doneW))
                        return false;
                    if (!patchBranchFwd(isFalse, 23, 5))
                        return false;
                    if (!emitLoadCStrAddr(a64reg::kScratch1, kDynBoolFalseText))
                        return false;
                    if (!patchBranchFwd(doneW, 25, 0))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch1);
                }
                case OP::STRING_LEN:
                case OP::BUILTIN_STRING_LENGTH:
                {
                    // (AR) طولُ النصّ بنقاطِ رمزِ UTF-8 (يطابق x86 والمفسّر): عُدَّ البايتَ غيرَ التابع
                    //      (b & 0xC0) != 0x80 حتّى NUL. الحلقةُ تدهس الحوضَ (x9..x13) ⇒ نسكٌ حولَها.
                    //      النتيجةُ تُنقَل إلى الخدشِ x16 لتبقى عبر إعادةِ التحميل (سجلّاتُ الحلقة من الحوض).
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) || !movz(10, 0)) // x9=المؤشّر، x10=العدّاد
                        return false;
                    const size_t head = code_.size();
                    if (!ldrb(11, 9) || !cmp(11, a64reg::kXzr)) // x11 = *x9 (بايتٌ ممدَّدٌ بالصفر)
                        return false;
                    size_t done;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", done)) // NUL ⇒ انتهى
                        return false;
                    // (AR) بايتٌ تابع؟ (x11 & 0xC0) == 0x80 ⇒ لا تَعُدّه. x12 = x11 & القناع، x13 = الوسم.
                    if (!movz(13, kUtf8ContMaskArm64) || !rrr(a64::mnem::kAnd, 12, 11, 13) ||
                        !movz(13, kUtf8ContTagArm64) || !cmp(12, 13))
                        return false;
                    size_t cont;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", cont)) // تابع ⇒ تخطَّ الزيادة
                        return false;
                    if (!addImm(10, 10, 1))
                        return false;
                    if (!patchBranchFwd(cont, 23, 5))
                        return false;
                    if (!addImm(9, 9, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(done, 23, 5))
                        return false;
                    if (!movReg(a64reg::kScratch0, 10)) // (AR) احفظِ النتيجةَ خارجَ الحوض قبل إعادةِ التحميل
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_CMP:
                {
                    // (AR) مقارنةُ نصّين: تُعيد منطقيًّا (١ متساويان، ٠ مختلفان) — يطابق x86 وemitStringCmp
                    //      (strcmp==0). مقارنةٌ بايتيّةٌ حتّى NUL. تدهس الحوضَ (x9..x12) ⇒ نسكٌ حولَها.
                    if (!requireArity(inst, 2))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) ||
                        !materializeString(inst.operands[1], 10, true)) // x9، x10 مؤشّران
                        return false;
                    const size_t head = code_.size();
                    if (!ldrb(11, 9) || !ldrb(12, 10) || !cmp(11, 12))
                        return false;
                    size_t neq;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", neq)) // بايتان مختلفان ⇒ غيرُ متساويين
                        return false;
                    if (!cmp(11, a64reg::kXzr)) // متساويان وكلاهما NUL ⇒ انتهيا معًا = متساويان
                        return false;
                    size_t eq;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", eq))
                        return false;
                    if (!addImm(9, 9, 1) || !addImm(10, 10, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(eq, 23, 5) || !movz(11, 1))
                        return false;
                    size_t doneW;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", doneW))
                        return false;
                    if (!patchBranchFwd(neq, 23, 5) || !movz(11, 0))
                        return false;
                    if (!patchBranchFwd(doneW, 25, 0))
                        return false;
                    if (!movReg(a64reg::kScratch0, 11)) // (AR) احفظِ النتيجةَ خارجَ الحوض قبل إعادةِ التحميل
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_ORD_CMP:
                {
                    // (AR) [ز.١٣] الترتيبُ المعجميُّ لنصّين ⇒ ‎-1/0/+1‎ — مرآةُ x86 حرفًا بحرف.
                    //      المقارنةُ ببايتاتٍ **غيرِ موقَّعة** (ldrb يوسّع بالأصفار، وblo «أدنى»
                    //      لا-موقَّعة): لو قُورنت موقَّعةً لصار كلُّ حرفٍ عربيّ أصغرَ من ASCII.
                    //      وUTF-8 حافظٌ للترتيب ⇒ ترتيبُ البايتاتِ هو ترتيبُ نقاطِ الترميز.
                    //      تدهس الحوضَ (x9..x12) ⇒ نسكٌ حولَها.
                    if (!requireArity(inst, 2))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) ||
                        !materializeString(inst.operands[1], 10, true)) // x9، x10 مؤشّران
                        return false;
                    const size_t head = code_.size();
                    if (!ldrb(11, 9) || !ldrb(12, 10) || !cmp(11, 12))
                        return false;
                    size_t neq;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", neq)) // بايتان مختلفان ⇒ الإشارةُ من فرقِهما
                        return false;
                    if (!cmp(11, a64reg::kXzr)) // متساويان وكلاهما NUL ⇒ انتهيا معًا ⇒ ٠
                        return false;
                    size_t eq;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", eq))
                        return false;
                    if (!addImm(9, 9, 1) || !addImm(10, 10, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(eq, 23, 5) || !movz(11, 0))
                        return false;
                    size_t doneW;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", doneW))
                        return false;
                    if (!patchBranchFwd(neq, 23, 5) || !cmp(11, 12))
                        return false;
                    size_t less;
                    if (!emitBranchFwd(a64::mnem::kBlo, "rel19", less))
                        return false;
                    if (!movz(11, 1))
                        return false;
                    size_t doneGt;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", doneGt))
                        return false;
                    // (AR) ‎-1‎: movz لا يقبل سالبًا (١٦ بتًّا لا-موقَّعة) ⇒ صفرٌ ثمّ نقصُ واحد.
                    if (!patchBranchFwd(less, 23, 5) || !movz(11, 0) || !subImm(11, 11, 1))
                        return false;
                    if (!patchBranchFwd(doneGt, 25, 0) || !patchBranchFwd(doneW, 25, 0))
                        return false;
                    if (!movReg(a64reg::kScratch0, 11)) // (AR) احفظِ النتيجةَ خارجَ الحوض قبل إعادةِ التحميل
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_TO_I64:
                {
                    // (AR) نصّ → رقم (atoll): إشارةٌ اختياريّةٌ ثمّ أرقامٌ عشريّةٌ حتّى أوّلِ غيرِ رقم. تدهس
                    //      الحوضَ (x9..x14) ⇒ نسكٌ حولَها. x10=المتراكم، x11=الإشارة (١/‎-1‎).
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) || !movz(10, 0) || !movz(11, 1))
                        return false;
                    // (AR) الإشارة: أوّلُ بايتٍ '-' ⇒ الإشارة ‎-1‎ وتقدّم (لا نعالج '+': غيرُ رقمٍ يُنهي).
                    if (!ldrb(12, 9) || !movz(13, kAsciiMinusArm64) || !cmp(12, 13))
                        return false;
                    size_t notMinus;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", notMinus))
                        return false;
                    if (!movz(11, 0) || !subImm(11, 11, 1) || !addImm(9, 9, 1)) // x11 = ‎-1‎
                        return false;
                    if (!patchBranchFwd(notMinus, 23, 5))
                        return false;
                    if (!movz(14, kItoaRadixArm64)) // x14 = ١٠
                        return false;
                    const size_t head = code_.size();
                    if (!ldrb(12, 9) || !movz(13, kAsciiZeroArm64) || !cmp(12, 13))
                        return false;
                    size_t doneLo;
                    if (!emitBranchFwd(a64::mnem::kBlt, "rel19", doneLo)) // بايت < '0' ⇒ انتهى
                        return false;
                    if (!movz(13, kAsciiNineArm64) || !cmp(12, 13))
                        return false;
                    size_t doneHi;
                    if (!emitBranchFwd(a64::mnem::kBgt, "rel19", doneHi)) // بايت > '9' ⇒ انتهى
                        return false;
                    // (AR) x10 = x10*10 + (بايت - '0')؛ ثمّ x9++.
                    if (!rrr(a64::mnem::kMul, 10, 10, 14) || !movz(13, kAsciiZeroArm64) ||
                        !rrr(a64::mnem::kSub, 12, 12, 13) || !rrr(a64::mnem::kAdd, 10, 10, 12) ||
                        !addImm(9, 9, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(doneLo, 23, 5) || !patchBranchFwd(doneHi, 23, 5))
                        return false;
                    if (!rrr(a64::mnem::kMul, 10, 10, 11)) // (AR) طبّقِ الإشارةَ (×١ أو ×‎-1‎) بلا فرع
                        return false;
                    if (!movReg(a64reg::kScratch0, 10)) // (AR) احفظِ النتيجةَ خارجَ الحوض قبل إعادةِ التحميل
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_REPLACE:
                case OP::BUILTIN_STRING_REPLACE:
                {
                    // (AR) استبدالُ أوّلِ ورودٍ (مرآةُ x86؛ يطابق LLVM؛ 🔴 المفسّرُ يستبدل الكلَّ ⇒ انجراف).
                    //      نسخٌ ٣-مقاطع؛ الغياب: matchPtr=نهايةُ s، lenN=lenR=٠ ⇒ نسخةُ s. الخريطة:
                    //      x9=RSI x10=RDI x11=R10 x12=R8 x13=R9 x14=R11/RCX x15=RAX. خانات strHeap 0..6.
                    if (!requireArity(inst, 3))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[1], 9, true) ||
                        !byteStrlen(9, 11, 10) || !strSlot(11, strHeapBaseSlot_ + 0)) // lenN
                        return false;
                    if (!materializeString(inst.operands[0], 9, true))
                        return false;
                    const size_t outer = code_.size();
                    if (!movReg(10, 9) || !materializeString(inst.operands[1], 11, true))
                        return false;
                    const size_t inner = code_.size();
                    if (!ldrb(12, 11) || !cmp(12, a64reg::kXzr))
                        return false;
                    size_t matched;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", matched))
                        return false;
                    if (!ldrb(13, 10) || !cmp(12, 13))
                        return false;
                    size_t mism;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", mism))
                        return false;
                    if (!addImm(10, 10, 1) || !addImm(11, 11, 1) || !emitBBack(inner))
                        return false;
                    if (!patchBranchFwd(mism, 23, 5))
                        return false;
                    if (!ldrb(12, 9) || !cmp(12, a64reg::kXzr))
                        return false;
                    size_t notfound;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", notfound))
                        return false;
                    if (!addImm(9, 9, 1) || !emitBBack(outer))
                        return false;
                    // (AR) الغياب: matchPtr=نهايةُ s، lenN=٠، lenR=٠.
                    if (!patchBranchFwd(notfound, 23, 5))
                        return false;
                    if (!strSlot(9, strHeapBaseSlot_ + 1) || !movz(12, 0) ||
                        !strSlot(12, strHeapBaseSlot_ + 0) || !strSlot(12, strHeapBaseSlot_ + 5))
                        return false;
                    size_t toUnified;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", toUnified))
                        return false;
                    // (AR) التطابق: matchPtr=x9؛ lenR = طولُ repl.
                    if (!patchBranchFwd(matched, 23, 5))
                        return false;
                    if (!strSlot(9, strHeapBaseSlot_ + 1) ||
                        !materializeString(inst.operands[2], 9, true) ||
                        !byteStrlen(9, 11, 10) || !strSlot(11, strHeapBaseSlot_ + 5))
                        return false;
                    // (AR) الموحَّد.
                    if (!patchBranchFwd(toUnified, 25, 0))
                        return false;
                    if (!materializeString(inst.operands[0], 14, true)) // x14 = sBase
                        return false;
                    if (!ldrSlot(15, strHeapBaseSlot_ + 1) || !rrr(a64::mnem::kSub, 11, 15, 14) ||
                        !strSlot(11, strHeapBaseSlot_ + 4)) // seg1Len
                        return false;
                    if (!ldrSlot(15, strHeapBaseSlot_ + 1) || !ldrSlot(11, strHeapBaseSlot_ + 0) ||
                        !rrr(a64::mnem::kAdd, 15, 15, 11) || !movReg(9, 15) ||
                        !byteStrlen(9, 11, 10) || !strSlot(11, strHeapBaseSlot_ + 6)) // lenTail
                        return false;
                    if (!ldrSlot(15, strHeapBaseSlot_ + 4) || !ldrSlot(11, strHeapBaseSlot_ + 5) ||
                        !rrr(a64::mnem::kAdd, 15, 15, 11) || !ldrSlot(11, strHeapBaseSlot_ + 6) ||
                        !rrr(a64::mnem::kAdd, 15, 15, 11) || !strSlot(15, strHeapBaseSlot_ + 2)) // totalLen
                        return false;
                    if (!movReg(a64reg::kX1, 15) || !addImm(a64reg::kX1, a64reg::kX1, 1 + kHeapHdrBytes) ||
                        !emitMmapArm64PresetSize())
                        return false; // x0 = base
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                        !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                        !strSlot(a64reg::kX0, strHeapBaseSlot_ + 3)) // buf
                        return false;
                    // (AR) المقطعُ ١: s[0..seg1Len) ← x10 الجاري.
                    if (!movReg(10, a64reg::kX0) || !materializeString(inst.operands[0], 9, true) ||
                        !ldrSlot(14, strHeapBaseSlot_ + 4) || !byteCopy(10, 9, 14, 12))
                        return false;
                    // (AR) المقطعُ ٢: repl[0..lenR).
                    if (!materializeString(inst.operands[2], 9, true) ||
                        !ldrSlot(14, strHeapBaseSlot_ + 5) || !byteCopy(10, 9, 14, 12))
                        return false;
                    // (AR) المقطعُ ٣: s[matchPtr+lenN ..) بطولِ lenTail.
                    if (!ldrSlot(9, strHeapBaseSlot_ + 1) || !ldrSlot(11, strHeapBaseSlot_ + 0) ||
                        !rrr(a64::mnem::kAdd, 9, 9, 11) || !ldrSlot(14, strHeapBaseSlot_ + 6) ||
                        !byteCopy(10, 9, 14, 12))
                        return false;
                    if (!movz(12, 0) || !strb(12, 10)) // NUL
                        return false;
                    if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 3)) // (AR) النتيجة = buf
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_FIND:
                case OP::BUILTIN_STRING_FIND:
                {
                    // (AR) البحث (مرآةُ x86): يُعيد فهرسَ نقطةِ رمزٍ لأوّلِ ورودٍ لـneedle في haystack (كـLLVM
                    //      strstr + تحويلٌ لفهرسِ نقطةِ رمز)، أو ‎-1‎. 🔴 المفسّرُ يُعيد إزاحةَ بايتٍ (انجرافٌ
                    //      موثَّق). needle فارغٌ ⇒ ٠. x9=haystack الجاري، hayBase في strHeap[0]، x15=الفهرس.
                    if (!requireArity(inst, 2))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) || !strSlot(9, strHeapBaseSlot_ + 0))
                        return false;
                    const size_t outer = code_.size();
                    if (!movReg(10, 9) || !materializeString(inst.operands[1], 11, true)) // hp=x10، np=x11
                        return false;
                    const size_t inner = code_.size();
                    if (!ldrb(12, 11) || !cmp(12, a64reg::kXzr))
                        return false;
                    size_t matched;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", matched)) // نهايةُ needle ⇒ تطابق
                        return false;
                    if (!ldrb(13, 10) || !cmp(12, 13))
                        return false;
                    size_t mism;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", mism))
                        return false;
                    if (!addImm(10, 10, 1) || !addImm(11, 11, 1) || !emitBBack(inner))
                        return false;
                    if (!patchBranchFwd(mism, 23, 5))
                        return false;
                    if (!ldrb(12, 9) || !cmp(12, a64reg::kXzr))
                        return false;
                    size_t notfound;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", notfound)) // نهايةُ haystack
                        return false;
                    if (!addImm(9, 9, 1) || !emitBBack(outer))
                        return false;
                    // (AR) تطابق: الفهرس = عددُ نقاطِ الرمزِ في [hayBase, x9).
                    if (!patchBranchFwd(matched, 23, 5))
                        return false;
                    if (!ldrSlot(14, strHeapBaseSlot_ + 0) || !movz(15, 0))
                        return false;
                    const size_t countHead = code_.size();
                    if (!cmp(14, 9))
                        return false;
                    size_t countDone;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", countDone))
                        return false;
                    if (!ldrb(12, 14) || !movz(a64reg::kScratch1, kUtf8ContMaskArm64) ||
                        !rrr(a64::mnem::kAnd, 13, 12, a64reg::kScratch1) ||
                        !movz(a64reg::kScratch1, kUtf8ContTagArm64) || !cmp(13, a64reg::kScratch1))
                        return false;
                    size_t skipInc;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", skipInc)) // تابع ⇒ لا يُحسَب
                        return false;
                    if (!addImm(15, 15, 1))
                        return false;
                    if (!patchBranchFwd(skipInc, 23, 5))
                        return false;
                    if (!addImm(14, 14, 1) || !emitBBack(countHead))
                        return false;
                    if (!patchBranchFwd(countDone, 23, 5))
                        return false;
                    size_t toEnd;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", toEnd))
                        return false;
                    if (!patchBranchFwd(notfound, 23, 5))
                        return false;
                    if (!movConst(15, -1)) // (AR) غيرُ موجود
                        return false;
                    if (!patchBranchFwd(toEnd, 25, 0))
                        return false;
                    if (!movReg(a64reg::kScratch0, 15)) // (AR) النتيجة ⇒ خارجَ الحوض
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_SUBSTR:
                case OP::BUILTIN_STRING_SUBSTRING:
                {
                    // (AR) جزءُ نصّ (مرآةُ x86): s، start، count بنقاطِ رمز؛ p0=s+start، p1=p0+count (قصٌّ
                    //      عند NUL)؛ byteLen=p1−p0؛ buf=mmap(byteLen+1+16)+memcpy+NUL. خانات strHeap 0/1/2.
                    if (!requireArity(inst, 3))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) ||
                        !loadArgInto(10, inst.operands[1]) || !skipCodepoints(9, 10, 11) ||
                        !strSlot(9, strHeapBaseSlot_ + 0)) // p0
                        return false;
                    if (!loadArgInto(10, inst.operands[2]) || !skipCodepoints(9, 10, 11))
                        return false;
                    if (!ldrSlot(11, strHeapBaseSlot_ + 0) || !rrr(a64::mnem::kSub, 10, 9, 11) ||
                        !strSlot(10, strHeapBaseSlot_ + 1)) // byteLen = p1 − p0
                        return false;
                    if (!movReg(a64reg::kX1, 10) || !addImm(a64reg::kX1, a64reg::kX1, 1 + kHeapHdrBytes) ||
                        !emitMmapArm64PresetSize())
                        return false; // (AR) x0 = base
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                        !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                        !strSlot(a64reg::kX0, strHeapBaseSlot_ + 2)) // buf
                        return false;
                    if (!movReg(12, a64reg::kX0) || !ldrSlot(9, strHeapBaseSlot_ + 0) ||
                        !ldrSlot(10, strHeapBaseSlot_ + 1) || !byteCopy(12, 9, 10, 11)) // memcpy
                        return false;
                    if (!movz(11, 0) || !strb(11, 12)) // NUL
                        return false;
                    if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 2)) // (AR) النتيجة = buf
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_CONCAT:
                {
                    // (AR) دمجُ نصّين (مرآةُ x86): buf = mmap(len1+len2+1+16)؛ انسخ s1 ثمّ s2 ثمّ NUL؛
                    //      أرجِع buf. len1/len2 بالبايت. الأطوالُ/المخزنُ في خانات strHeap (تبقى عبر mmap).
                    //      x12 مؤشّرُ الوجهةِ الجاري، x16 خدشُ النتيجة.
                    if (!requireArity(inst, 2))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true) ||
                        !byteStrlen(9, 10, 11) || !strSlot(10, strHeapBaseSlot_ + 0)) // len1
                        return false;
                    if (!materializeString(inst.operands[1], 9, true) ||
                        !byteStrlen(9, 10, 11) || !strSlot(10, strHeapBaseSlot_ + 1)) // len2
                        return false;
                    // (AR) mmap(len1+len2+1+16)؛ x1 = الحجمُ الكلّيّ.
                    if (!ldrSlot(10, strHeapBaseSlot_ + 0) || !ldrSlot(11, strHeapBaseSlot_ + 1) ||
                        !rrr(a64::mnem::kAdd, a64reg::kX1, 10, 11) ||
                        !addImm(a64reg::kX1, a64reg::kX1, 1 + kHeapHdrBytes) || !emitMmapArm64PresetSize())
                        return false; // (AR) x0 = base
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                        !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                        !strSlot(a64reg::kX0, strHeapBaseSlot_ + 2)) // buf (النتيجة)
                        return false;
                    // (AR) انسخ s1 → buf (x12 الجاري)، ثمّ s2 → buf+len1.
                    if (!movReg(12, a64reg::kX0) || !materializeString(inst.operands[0], 9, true) ||
                        !ldrSlot(10, strHeapBaseSlot_ + 0) || !byteCopy(12, 9, 10, 11))
                        return false;
                    if (!materializeString(inst.operands[1], 9, true) ||
                        !ldrSlot(10, strHeapBaseSlot_ + 1) || !byteCopy(12, 9, 10, 11))
                        return false;
                    if (!movz(11, 0) || !strb(11, 12)) // (AR) خاتمةُ NUL
                        return false;
                    if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 2)) // (AR) النتيجة = buf (خارجَ الحوض)
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::I64_TO_STRING:
                {
                    // (AR) رقم → نصّ (مرآةُ x86): يخصّص مخزنَ كومةٍ ويبني الأرقامَ تنازليًّا من نهايته
                    //      منتهيةً بـNUL (itoa عبر sdiv/msub، باقٍ-سالبٌ ⇒ آمنٌ لـINT64_MIN)، ثمّ يُعيد
                    //      مؤشّرًا لأوّلِ رقم/إشارة. 🔴 المؤشّرُ وسطُ المخزن ⇒ غيرُ قابلٍ لـFREE (تسريبٌ
                    //      مقبولٌ مطابقٌ لـLLVM). النتيجةُ تُحمَل خارجَ الحوض (x16) قبل إعادةِ التحميل.
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!loadArgInto(9, inst.operands[0]) || !strSlot(9, strHeapBaseSlot_)) // احفظِ العددَ
                        return false;
                    if (!movz(a64reg::kX1, kItoaBufPayload + kHeapHdrBytes) || !emitMmapArm64PresetSize())
                        return false; // (AR) x0 = base
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                        !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes)) // (AR) x0 = buf
                        return false;
                    // (AR) x13 = buf + payload (قمّةٌ حصريّة)؛ NUL في *(--x13).
                    if (!addImm(13, a64reg::kX0, kItoaBufPayload) ||
                        !subImm(13, 13, 1) || !movz(12, 0) || !strb(12, 13))
                        return false;
                    if (!ldrSlot(9, strHeapBaseSlot_) || !movz(10, kItoaRadixArm64)) // استرجعِ العددَ، x10=١٠
                        return false;
                    if (inst.operands[0].dataType == types::SadTypeKind::UInt64)
                    {
                        // (AR) طبيعي64: itoa **لا-موقَّع** (udiv/msub بلا فحصِ إشارةٍ ولا '-') ⇒
                        //   «18446744073709551615» لا «-1». مرآةُ x86 ويطابقُ نصَّ المفسّر.
                        const size_t uLoop = code_.size();
                        if (!rrr(a64::mnem::kUdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                            !addImm(12, 12, kAsciiZeroArm64) ||
                            !subImm(13, 13, 1) || !strb(12, 13) ||
                            !movReg(9, 11) || !emitCbnzBack(9, uLoop))
                            return false;
                    }
                    else
                    {
                        if (!cmp(9, 31)) // (AR) الإشارة: x9 ≥ ٠ ⇒ اقفز للموجب
                            return false;
                        size_t toPositive;
                        if (!emitBranchFwd(a64::mnem::kBge, "rel19", toPositive))
                            return false;
                        // (AR) لولبُ السالب: x11=حاصل، x12=باقٍ؛ الرقم = |باقٍ| + '0'.
                        const size_t negLoop = code_.size();
                        if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                            !rrr(a64::mnem::kSub, 12, 31, 12) || !addImm(12, 12, kAsciiZeroArm64) ||
                            !subImm(13, 13, 1) || !strb(12, 13) ||
                            !movReg(9, 11) || !emitCbnzBack(9, negLoop))
                            return false;
                        if (!movz(12, kAsciiMinusArm64) || !subImm(13, 13, 1) || !strb(12, 13)) // '-'
                            return false;
                        size_t toDone;
                        if (!emitBranchFwd(a64::mnem::kB, "rel26", toDone))
                            return false;
                        if (!patchBranchFwd(toPositive, 23, 5))
                            return false;
                        // (AR) لولبُ الموجب: الرقم = باقٍ + '0'.
                        const size_t posLoop = code_.size();
                        if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                            !addImm(12, 12, kAsciiZeroArm64) ||
                            !subImm(13, 13, 1) || !strb(12, 13) ||
                            !movReg(9, 11) || !emitCbnzBack(9, posLoop))
                            return false;
                        if (!patchBranchFwd(toDone, 25, 0))
                            return false;
                    }
                    if (!movReg(a64reg::kScratch0, 13)) // (AR) النتيجة = x13 ⇒ خارجَ الحوض
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::TUPLE_TO_STRING:
                case OP::ARRAY_TO_STRING:
                {
                    // (AR) مصفوفة → نصّ سياديًّا (مرآةُ x86، بلا زمنِ تشغيل C): يبني «[ع0، ع1، ...]». المسارُ
                    //      العدديّ (elementType عدديٌّ/مختلطٌ غيرُ-Any): buf = mmap(len*34 + 4 + 16)؛ اكتب '['؛
                    //      لكلِّ عنصرٍ فاصلَ «، » (عدا الأوّل) ثمّ itoa (باقٍ-سالبٌ آمنٌ لـINT64_MIN) تنازليًّا
                    //      في مخزنِ الإطار (printBufTopSlot_) ثمّ byteCopy إلى buf؛ اختم بـ']'+NUL. النصّيّ/
                    //      العشريّ/Any تلي في هذه الدفعة. خانات strHeap: +0=len +1=data +2=buf +3=dst +4=i.
                    if (!requireArity(inst, 1))
                        return false;
                    const types::SadTypeKind atsElem = inst.operands[0].elementType;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (atsElem == types::SadTypeKind::Any)
                    {
                        // (AR) المسارُ المختلط (Any، مرآةُ x86): حمولةٌ خام في data[i] ووسمٌ في tags[i]
                        //      (null⇒Int). تمريرتان: (١) الحجم = 3 + Σ(طولُ العنصر + 2) [النصّ=طولُ واصفه،
                        //      غيرُه ≤340]. (٢) mmap ثمّ '[' ثمّ فاصلَ «، » ثمّ توزيعٌ حسب الوسم وbyteCopy؛
                        //      اختم بـ']'+NUL. خانات strHeap: +0=len +1=data +2=acc/buf +3=dst +4=i +5=tags +6=payload +7=tag.
                        if (!loadArgInto(9, inst.operands[0]))
                            return false;
                        if (!ldrBase(10, 9, kArrOffLen / kArrSlotBytes) || !strSlot(10, strHeapBaseSlot_ + 0))
                            return false;
                        if (!ldrBase(11, 9, kArrOffData / kArrSlotBytes) || !strSlot(11, strHeapBaseSlot_ + 1))
                            return false;
                        if (!ldrBase(11, 9, kArrOffTags / kArrSlotBytes) || !strSlot(11, strHeapBaseSlot_ + 5))
                            return false;
                        // (AR) تمريرة ١: acc = 3؛ i = 0.
                        if (!movz(9, kAtsStrBaseBytes) || !strSlot(9, strHeapBaseSlot_ + 2) ||
                            !movz(9, 0) || !strSlot(9, strHeapBaseSlot_ + 4))
                            return false;
                        const size_t atsDSizeHead = code_.size();
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !ldrSlot(10, strHeapBaseSlot_ + 0) || !cmp(9, 10))
                            return false;
                        size_t atsDSizeDone;
                        if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsDSizeDone))
                            return false;
                        if (!emitAtsDynLoadTagArm64(11, 9)) // x11 = tag (x9 = i)
                            return false;
                        // (AR) النصّ: طولُ العنصرِ = طولُ واصفه.
                        if (!movz(a64reg::kScratch0, kDynKindStr) || !cmp(11, a64reg::kScratch0))
                            return false;
                        size_t atsDSizeNotStr;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsDSizeNotStr))
                            return false;
                        if (!ldrSlot(10, strHeapBaseSlot_ + 1) || !ldrSlot(9, strHeapBaseSlot_ + 4) ||
                            !addLsl3(10, 10, 9) || !ldrBase(12, 10, 0) || !ldrBase(10, 12, 0))
                            return false; // x10 = طولُ الواصف
                        if (!ldrSlot(9, strHeapBaseSlot_ + 2) || !rrr(a64::mnem::kAdd, 9, 9, 10) ||
                            !addImm(9, 9, kAtsStrPerElemExtra) || !strSlot(9, strHeapBaseSlot_ + 2))
                            return false;
                        size_t atsDSizeAdv;
                        if (!emitBranchFwd(a64::mnem::kB, "rel26", atsDSizeAdv))
                            return false;
                        if (!patchBranchFwd(atsDSizeNotStr, 23, 5))
                            return false;
                        // (AR) غيرُ النصّ: تقديرٌ آمنٌ ثابت + فاصل.
                        if (!ldrSlot(9, strHeapBaseSlot_ + 2) ||
                            !addImm(9, 9, kAtsFloatBytesPerElem + kAtsStrPerElemExtra) ||
                            !strSlot(9, strHeapBaseSlot_ + 2))
                            return false;
                        if (!patchBranchFwd(atsDSizeAdv, 25, 0))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !addImm(9, 9, 1) ||
                            !strSlot(9, strHeapBaseSlot_ + 4) || !emitBBack(atsDSizeHead))
                            return false;
                        if (!patchBranchFwd(atsDSizeDone, 23, 5))
                            return false;
                        // (AR) mmap(acc + 16)؛ x1 = الحجم.
                        if (!ldrSlot(a64reg::kX1, strHeapBaseSlot_ + 2) ||
                            !addImm(a64reg::kX1, a64reg::kX1, kHeapHdrBytes) || !emitMmapArm64PresetSize())
                            return false; // x0 = base
                        if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                            !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                            !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                            return false;
                        if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                            !strSlot(a64reg::kX0, strHeapBaseSlot_ + 2)) // buf
                            return false;
                        if (!movz(9, kAsciiLBracket) || !strb(9, a64reg::kX0))
                            return false;
                        if (!addImm(12, a64reg::kX0, 1) || !strSlot(12, strHeapBaseSlot_ + 3)) // dst
                            return false;
                        if (!movz(9, 0) || !strSlot(9, strHeapBaseSlot_ + 4)) // i = 0
                            return false;
                        // (AR) تمريرة ٢: الملء.
                        const size_t atsDFillHead = code_.size();
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !ldrSlot(10, strHeapBaseSlot_ + 0) || !cmp(9, 10))
                            return false;
                        size_t atsDFillDone;
                        if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsDFillDone))
                            return false;
                        // (AR) الفاصلُ «، » إن i>0.
                        if (!cmp(9, a64reg::kXzr))
                            return false;
                        size_t atsDNoSep;
                        if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsDNoSep))
                            return false;
                        if (!ldrSlot(12, strHeapBaseSlot_ + 3) ||
                            !movz(9, kAsciiComma) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !movz(9, kAsciiSpace) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !strSlot(12, strHeapBaseSlot_ + 3))
                            return false;
                        if (!patchBranchFwd(atsDNoSep, 23, 5))
                            return false;
                        // (AR) payload = data[i] ⇒ +6؛ tag ⇒ +7.
                        if (!ldrSlot(10, strHeapBaseSlot_ + 1) || !ldrSlot(9, strHeapBaseSlot_ + 4) ||
                            !addLsl3(10, 10, 9) || !ldrBase(11, 10, 0) || !strSlot(11, strHeapBaseSlot_ + 6))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !emitAtsDynLoadTagArm64(11, 9) ||
                            !strSlot(11, strHeapBaseSlot_ + 7))
                            return false;
                        // (AR) التوزيع ⇒ (x10=المصدر، x11=الطول)، ثمّ ذيلٌ موحَّدٌ يَنسخ.
                        std::vector<size_t> atsDArmEnds;
                        // Float.
                        if (!ldrSlot(11, strHeapBaseSlot_ + 7) || !movz(a64reg::kScratch0, kDynKindFloat) ||
                            !cmp(11, a64reg::kScratch0))
                            return false;
                        size_t atsDNotF;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsDNotF))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 6) || !emitAtsFloatToScratchArm64())
                            return false; // x13=ptr, x14=top
                        if (!movReg(10, 13) || !rrr(a64::mnem::kSub, 11, 14, 13))
                            return false; // x10=src, x11=len
                        {
                            size_t j;
                            if (!emitBranchFwd(a64::mnem::kB, "rel26", j))
                                return false;
                            atsDArmEnds.push_back(j);
                        }
                        if (!patchBranchFwd(atsDNotF, 23, 5))
                            return false;
                        // Int.
                        if (!ldrSlot(11, strHeapBaseSlot_ + 7) || !movz(a64reg::kScratch0, kDynKindInt) ||
                            !cmp(11, a64reg::kScratch0))
                            return false;
                        size_t atsDNotI;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsDNotI))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 6) || !emitAtsIntToScratchArm64())
                            return false; // x13=ptr, x14=top
                        if (!movReg(10, 13) || !rrr(a64::mnem::kSub, 11, 14, 13))
                            return false;
                        {
                            size_t j;
                            if (!emitBranchFwd(a64::mnem::kB, "rel26", j))
                                return false;
                            atsDArmEnds.push_back(j);
                        }
                        if (!patchBranchFwd(atsDNotI, 23, 5))
                            return false;
                        // Str: الواصف: x10 = payload+8، x11 = [payload].
                        if (!ldrSlot(11, strHeapBaseSlot_ + 7) || !movz(a64reg::kScratch0, kDynKindStr) ||
                            !cmp(11, a64reg::kScratch0))
                            return false;
                        size_t atsDNotS;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsDNotS))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 6) || !ldrBase(11, 9, 0) || !movReg(10, 9) ||
                            !addImm(10, 10, 8))
                            return false;
                        {
                            size_t j;
                            if (!emitBranchFwd(a64::mnem::kB, "rel26", j))
                                return false;
                            atsDArmEnds.push_back(j);
                        }
                        if (!patchBranchFwd(atsDNotS, 23, 5))
                            return false;
                        // Bool ⇒ «صحيح»/«خطأ».
                        if (!ldrSlot(11, strHeapBaseSlot_ + 7) || !movz(a64reg::kScratch0, kDynKindBool) ||
                            !cmp(11, a64reg::kScratch0))
                            return false;
                        size_t atsDNotB;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsDNotB))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 6) || !cmp(9, a64reg::kXzr))
                            return false;
                        size_t atsDBoolFalse;
                        if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsDBoolFalse))
                            return false;
                        if (!emitLoadCStrAddr(10, kDynBoolTrueText))
                            return false;
                        size_t atsDBoolDone;
                        if (!emitBranchFwd(a64::mnem::kB, "rel26", atsDBoolDone))
                            return false;
                        if (!patchBranchFwd(atsDBoolFalse, 23, 5))
                            return false;
                        if (!emitLoadCStrAddr(10, kDynBoolFalseText))
                            return false;
                        if (!patchBranchFwd(atsDBoolDone, 25, 0))
                            return false;
                        if (!movReg(9, 10) || !byteStrlen(9, 11, 12))
                            return false; // x10=start, x11=len
                        {
                            size_t j;
                            if (!emitBranchFwd(a64::mnem::kB, "rel26", j))
                                return false;
                            atsDArmEnds.push_back(j);
                        }
                        if (!patchBranchFwd(atsDNotB, 23, 5))
                            return false;
                        // Null (وأيُّ وسمٍ آخر) ⇒ «عدم».
                        if (!emitLoadCStrAddr(10, kDynNullText) || !movReg(9, 10) || !byteStrlen(9, 11, 12))
                            return false;
                        for (size_t j : atsDArmEnds)
                            if (!patchBranchFwd(j, 25, 0))
                                return false;
                        // (AR) الذيلُ الموحَّد: byteCopy(dst, x10, x11)؛ dst يتقدّم ⇒ خزّنه.
                        if (!ldrSlot(9, strHeapBaseSlot_ + 3) || !byteCopy(9, 10, 11, 12) ||
                            !strSlot(9, strHeapBaseSlot_ + 3))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !addImm(9, 9, 1) ||
                            !strSlot(9, strHeapBaseSlot_ + 4) || !emitBBack(atsDFillHead))
                            return false;
                        if (!patchBranchFwd(atsDFillDone, 23, 5))
                            return false;
                        if (!ldrSlot(12, strHeapBaseSlot_ + 3) ||
                            !movz(9, kAsciiRBracket) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !movz(9, 0) || !strb(9, 12))
                            return false;
                        if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 2)) // النتيجة = buf
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!reloadReg(kv.second))
                                    return false;
                        int dstD;
                        if (!allocReg(inst.result->name, dstD))
                            return false;
                        return movReg(dstD, a64reg::kScratch0);
                    }
                    if (atsElem == types::SadTypeKind::String)
                    {
                        // (AR) المسارُ النصّيّ (مرآةُ x86): كلُّ عنصرٍ مؤشّرُ char*. تمريرتان: (١) acc =
                        //      3 + Σ(بايتات strlen + 2)؛ (٢) mmap ثمّ '[' ثمّ لكلِّ عنصرٍ فاصلَ «، »
                        //      (عدا الأوّل) ثمّ byteCopy لسلسلته؛ اختم بـ']'+NUL. خانات strHeap:
                        //      +0=len +1=data +2=acc +3=buf +4=dst +5=i.
                        if (!loadArgInto(9, inst.operands[0]))
                            return false;
                        if (!ldrBase(10, 9, kArrOffLen / kArrSlotBytes) || !strSlot(10, strHeapBaseSlot_ + 0))
                            return false;
                        if (!ldrBase(11, 9, kArrOffData / kArrSlotBytes) || !strSlot(11, strHeapBaseSlot_ + 1))
                            return false;
                        // (AR) تمريرة ١: acc = 3؛ i = 0.
                        if (!movz(9, kAtsStrBaseBytes) || !strSlot(9, strHeapBaseSlot_ + 2) ||
                            !movz(9, 0) || !strSlot(9, strHeapBaseSlot_ + 5))
                            return false;
                        const size_t atsStrSizeHead = code_.size();
                        if (!ldrSlot(9, strHeapBaseSlot_ + 5) || !ldrSlot(10, strHeapBaseSlot_ + 0) || !cmp(9, 10))
                            return false;
                        size_t atsStrSizeDone;
                        if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsStrSizeDone))
                            return false;
                        // (AR) elem = data[i] (char*)؛ L = byteStrlen(elem)؛ acc += L + 2.
                        if (!ldrSlot(11, strHeapBaseSlot_ + 1) || !addLsl3(11, 11, 9) || !ldrBase(12, 11, 0))
                            return false;
                        if (!byteStrlen(12, 10, 13)) // x10 = L
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 2) || !rrr(a64::mnem::kAdd, 9, 9, 10) ||
                            !addImm(9, 9, kAtsStrPerElemExtra) || !strSlot(9, strHeapBaseSlot_ + 2))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 5) || !addImm(9, 9, 1) ||
                            !strSlot(9, strHeapBaseSlot_ + 5) || !emitBBack(atsStrSizeHead))
                            return false;
                        if (!patchBranchFwd(atsStrSizeDone, 23, 5))
                            return false;
                        // (AR) mmap(acc + 16)؛ x1 = الحجم.
                        if (!ldrSlot(a64reg::kX1, strHeapBaseSlot_ + 2) ||
                            !addImm(a64reg::kX1, a64reg::kX1, kHeapHdrBytes) || !emitMmapArm64PresetSize())
                            return false; // x0 = base
                        if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                            !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                            !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                            return false;
                        if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                            !strSlot(a64reg::kX0, strHeapBaseSlot_ + 3)) // buf
                            return false;
                        // (AR) '[' في buf[0]؛ dst = buf+1؛ i = 0.
                        if (!movz(9, kAsciiLBracket) || !strb(9, a64reg::kX0))
                            return false;
                        if (!addImm(12, a64reg::kX0, 1) || !strSlot(12, strHeapBaseSlot_ + 4))
                            return false;
                        if (!movz(9, 0) || !strSlot(9, strHeapBaseSlot_ + 5))
                            return false;
                        // (AR) تمريرة ٢: الملء.
                        const size_t atsStrFillHead = code_.size();
                        if (!ldrSlot(9, strHeapBaseSlot_ + 5) || !ldrSlot(10, strHeapBaseSlot_ + 0) || !cmp(9, 10))
                            return false;
                        size_t atsStrFillDone;
                        if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsStrFillDone))
                            return false;
                        if (!ldrSlot(12, strHeapBaseSlot_ + 4)) // (AR) dst الجاري (x12)
                            return false;
                        if (!cmp(9, a64reg::kXzr)) // i>0 ⇒ الفاصل «، »
                            return false;
                        size_t atsStrNoSep;
                        if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsStrNoSep))
                            return false;
                        if (!movz(13, kAsciiComma) || !strb(13, 12) || !addImm(12, 12, 1) ||
                            !movz(13, kAsciiSpace) || !strb(13, 12) || !addImm(12, 12, 1))
                            return false;
                        if (!patchBranchFwd(atsStrNoSep, 23, 5))
                            return false;
                        // (AR) elem = data[i] ⇒ x10 (يُحفَظ)؛ L = byteStrlen(نسخة x9)؛ byteCopy(dst, elem, L).
                        if (!ldrSlot(11, strHeapBaseSlot_ + 1) || !ldrSlot(9, strHeapBaseSlot_ + 5) ||
                            !addLsl3(11, 11, 9) || !ldrBase(10, 11, 0))
                            return false;
                        if (!movReg(9, 10) || !byteStrlen(9, 13, 14)) // x13 = L
                            return false;
                        if (!byteCopy(12, 10, 13, 14) || !strSlot(12, strHeapBaseSlot_ + 4))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 5) || !addImm(9, 9, 1) ||
                            !strSlot(9, strHeapBaseSlot_ + 5) || !emitBBack(atsStrFillHead))
                            return false;
                        if (!patchBranchFwd(atsStrFillDone, 23, 5))
                            return false;
                        // (AR) الختام: ']' ثمّ NUL؛ النتيجة = buf.
                        if (!ldrSlot(12, strHeapBaseSlot_ + 4) ||
                            !movz(9, kAsciiRBracket) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !movz(9, 0) || !strb(9, 12))
                            return false;
                        if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 3)) // النتيجة = buf
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!reloadReg(kv.second))
                                    return false;
                        int dstS;
                        if (!allocReg(inst.result->name, dstS))
                            return false;
                        return movReg(dstS, a64reg::kScratch0);
                    }
                    if (atsElem == types::SadTypeKind::Float)
                    {
                        // (AR) المسارُ العشريّ (مرآةُ x86): كلُّ عنصرٍ نمطُ بتّاتِ double. لكلِّ عنصرٍ نفكّكه
                        //      (إشارة/ip/scaled/nd كـF64_TO_STRING) ونبنيه تنازليًّا في مخزنِ خدشٍ على الإطار
                        //      (atsFtoaTopSlot_؛ لا mmap لكلِّ عنصرٍ ⇒ sign=x17, ip=x15 تبقيان) ثمّ byteCopy.
                        //      خانات strHeap: +0=len +1=data +2=buf +3=dst +4=i.
                        if (!loadArgInto(9, inst.operands[0]))
                            return false;
                        if (!ldrBase(10, 9, kArrOffLen / kArrSlotBytes) || !strSlot(10, strHeapBaseSlot_ + 0))
                            return false;
                        if (!ldrBase(11, 9, kArrOffData / kArrSlotBytes) || !strSlot(11, strHeapBaseSlot_ + 1))
                            return false;
                        // (AR) mmap(len*340 + 4 + 16)؛ x1 = الحجم.
                        if (!movz(12, kAtsFloatBytesPerElem) || !rrr(a64::mnem::kMul, a64reg::kX1, 10, 12) ||
                            !addImm(a64reg::kX1, a64reg::kX1, kAtsFrameBytes + kHeapHdrBytes) ||
                            !emitMmapArm64PresetSize())
                            return false; // x0 = base
                        if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                            !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                            !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                            return false;
                        if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                            !strSlot(a64reg::kX0, strHeapBaseSlot_ + 2)) // buf
                            return false;
                        if (!movz(9, kAsciiLBracket) || !strb(9, a64reg::kX0))
                            return false;
                        if (!addImm(12, a64reg::kX0, 1) || !strSlot(12, strHeapBaseSlot_ + 3)) // dst
                            return false;
                        if (!movz(9, 0) || !strSlot(9, strHeapBaseSlot_ + 4)) // i = 0
                            return false;
                        const size_t atsFLoopHead = code_.size();
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !ldrSlot(10, strHeapBaseSlot_ + 0) || !cmp(9, 10))
                            return false;
                        size_t atsFLoopDone;
                        if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsFLoopDone))
                            return false;
                        // (AR) الفاصلُ «، » إن i>0.
                        if (!cmp(9, a64reg::kXzr))
                            return false;
                        size_t atsFNoSep;
                        if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsFNoSep))
                            return false;
                        if (!ldrSlot(12, strHeapBaseSlot_ + 3) ||
                            !movz(9, kAsciiComma) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !movz(9, kAsciiSpace) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !strSlot(12, strHeapBaseSlot_ + 3))
                            return false;
                        if (!patchBranchFwd(atsFNoSep, 23, 5))
                            return false;
                        // (AR) العنصرُ = data[i] (بتّاتُ double) ⇒ x9.
                        if (!ldrSlot(11, strHeapBaseSlot_ + 1) || !ldrSlot(12, strHeapBaseSlot_ + 4) ||
                            !addLsl3(11, 11, 12) || !ldrBase(9, 11, 0))
                            return false;
                        // (AR) sign ⇒ x17 (يبقى عبر البناء).
                        if (!movImm64Bits(a64reg::kScratch0, kF64SignMask) ||
                            !rrr(a64::mnem::kAnd, 10, 9, a64reg::kScratch0) || !cmp(10, 31))
                            return false;
                        size_t atsFSignZero;
                        if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsFSignZero))
                            return false;
                        if (!movz(a64reg::kScratch1, 1))
                            return false;
                        size_t atsFSignSet;
                        if (!emitBranchFwd(a64::mnem::kB, "rel26", atsFSignSet))
                            return false;
                        if (!patchBranchFwd(atsFSignZero, 23, 5))
                            return false;
                        if (!movz(a64reg::kScratch1, 0))
                            return false;
                        if (!patchBranchFwd(atsFSignSet, 25, 0))
                            return false;
                        // (AR) |x| = bits & absmask ⇒ ip = trunc(|x|) في x15.
                        if (!movImm64Bits(a64reg::kScratch0, kF64AbsMask) ||
                            !rrr(a64::mnem::kAnd, 9, 9, a64reg::kScratch0))
                            return false;
                        if (!fmovToFp(kD0, 9) || !fcvtzs(15, kD0))
                            return false;
                        // (AR) scaled = round((|x| − ip)×١٠^٦) في x12.
                        if (!fmovToFp(kD0, 9) || !scvtf(kD1, 15) || !fsub(kD0, kD0, kD1))
                            return false;
                        if (!loadFloatConst(a64reg::kScratch0, static_cast<double>(kFloatPrecisionScale)) ||
                            !fmovToFp(kD1, a64reg::kScratch0) || !fmul(kD0, kD0, kD1) || !fcvtns(12, kD0))
                            return false;
                        // (AR) ترحيلُ الحمل: scaled == ١٠^٦ ⇒ ip++ وscaled=0.
                        if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kFloatPrecisionScale)) ||
                            !cmp(12, a64reg::kScratch0))
                            return false;
                        size_t atsFNoCarry;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsFNoCarry))
                            return false;
                        if (!addImm(15, 15, 1) || !movz(12, 0))
                            return false;
                        if (!patchBranchFwd(atsFNoCarry, 23, 5))
                            return false;
                        // (AR) حذفُ الأصفار الزائدة: nd(x11)=٦؛ x10=١٠؛ x16=١ (ثابتُ المقارنة).
                        if (!movz(11, kFloatDecimals) || !movz(10, kItoaRadixArm64) || !movz(a64reg::kScratch0, 1))
                            return false;
                        const size_t atsFStripTop = code_.size();
                        if (!cmp(11, a64reg::kScratch0))
                            return false;
                        size_t atsFStripDone;
                        if (!emitBranchFwd(a64::mnem::kBle, "rel19", atsFStripDone))
                            return false;
                        if (!rrr(a64::mnem::kSdiv, 14, 12, 10) || !msub(9, 14, 10, 12) || !cmp(9, 31))
                            return false;
                        size_t atsFStripStop;
                        if (!emitBranchFwd(a64::mnem::kBne, "rel19", atsFStripStop))
                            return false;
                        if (!movReg(12, 14) || !subImm(11, 11, 1) || !emitBBack(atsFStripTop))
                            return false;
                        if (!patchBranchFwd(atsFStripDone, 23, 5) || !patchBranchFwd(atsFStripStop, 23, 5))
                            return false;
                        // (AR) ابنِ تنازليًّا: x13 = قمّةُ المخزن (حصريّ). x17=sign, x15=ip, x12=scaled, x11=nd, x10=١٠.
                        if (!addImm(13, 31, static_cast<long long>(atsFtoaTopSlot_) * 8))
                            return false;
                        // (١) خاناتُ الكسر (nd خانةً).
                        if (!movReg(9, 12)) // x9 = scaled
                            return false;
                        const size_t atsFFracTop = code_.size();
                        if (!cmp(11, 31))
                            return false;
                        size_t atsFFracDone;
                        if (!emitBranchFwd(a64::mnem::kBle, "rel19", atsFFracDone))
                            return false;
                        if (!rrr(a64::mnem::kSdiv, 14, 9, 10) || !msub(12, 14, 10, 9) ||
                            !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                            !movReg(9, 14) || !subImm(11, 11, 1) || !emitBBack(atsFFracTop))
                            return false;
                        if (!patchBranchFwd(atsFFracDone, 23, 5))
                            return false;
                        // (٢) النقطة.
                        if (!subImm(13, 13, 1) || !movz(12, kAsciiDotArm64) || !strb(12, 13))
                            return false;
                        // (٣) الجزءُ الصحيح ip تنازليًّا (رقمٌ واحدٌ على الأقلّ).
                        if (!movReg(9, 15))
                            return false;
                        const size_t atsFIpTop = code_.size();
                        if (!rrr(a64::mnem::kSdiv, 14, 9, 10) || !msub(12, 14, 10, 9) ||
                            !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                            !movReg(9, 14) || !emitCbnzBack(9, atsFIpTop))
                            return false;
                        // (٤) بادئةُ السالب إن لزم (x17 = sign).
                        if (!cmp(a64reg::kScratch1, 31))
                            return false;
                        size_t atsFNoSign;
                        if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsFNoSign))
                            return false;
                        if (!subImm(13, 13, 1) || !movz(12, kAsciiMinusArm64) || !strb(12, 13))
                            return false;
                        if (!patchBranchFwd(atsFNoSign, 23, 5))
                            return false;
                        // (AR) L = القمّة − x13؛ byteCopy(dst, x13, L).
                        if (!addImm(14, 31, static_cast<long long>(atsFtoaTopSlot_) * 8) ||
                            !rrr(a64::mnem::kSub, 11, 14, 13))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 3) || !byteCopy(9, 13, 11, 12) ||
                            !strSlot(9, strHeapBaseSlot_ + 3))
                            return false;
                        if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !addImm(9, 9, 1) ||
                            !strSlot(9, strHeapBaseSlot_ + 4) || !emitBBack(atsFLoopHead))
                            return false;
                        if (!patchBranchFwd(atsFLoopDone, 23, 5))
                            return false;
                        if (!ldrSlot(12, strHeapBaseSlot_ + 3) ||
                            !movz(9, kAsciiRBracket) || !strb(9, 12) || !addImm(12, 12, 1) ||
                            !movz(9, 0) || !strb(9, 12))
                            return false;
                        if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 2)) // النتيجة = buf
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!reloadReg(kv.second))
                                    return false;
                        int dstF;
                        if (!allocReg(inst.result->name, dstF))
                            return false;
                        return movReg(dstF, a64reg::kScratch0);
                    }
                    // (AR) x9 = مؤشّرُ المصفوفة؛ len=[arr+0]، data=[arr+16] ⇒ خانات.
                    if (!loadArgInto(9, inst.operands[0]))
                        return false;
                    if (!ldrBase(10, 9, kArrOffLen / kArrSlotBytes) || !strSlot(10, strHeapBaseSlot_ + 0))
                        return false;
                    if (!ldrBase(11, 9, kArrOffData / kArrSlotBytes) || !strSlot(11, strHeapBaseSlot_ + 1))
                        return false;
                    // (AR) mmap(len*34 + 4 + 16)؛ x1 = الحجمُ الكلّيّ.
                    if (!movz(12, kAtsBytesPerElem) || !rrr(a64::mnem::kMul, a64reg::kX1, 10, 12) ||
                        !addImm(a64reg::kX1, a64reg::kX1, kAtsFrameBytes + kHeapHdrBytes) ||
                        !emitMmapArm64PresetSize())
                        return false; // (AR) x0 = base
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                        !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes) ||
                        !strSlot(a64reg::kX0, strHeapBaseSlot_ + 2)) // buf
                        return false;
                    // (AR) '[' في buf[0]؛ dst = buf+1؛ i = 0.
                    if (!movz(9, kAsciiLBracket) || !strb(9, a64reg::kX0))
                        return false;
                    if (!addImm(12, a64reg::kX0, 1) || !strSlot(12, strHeapBaseSlot_ + 3)) // dst
                        return false;
                    if (!movz(9, 0) || !strSlot(9, strHeapBaseSlot_ + 4)) // i = 0
                        return false;
                    // (AR) رأسُ الحلقة: if i >= len ⇒ الختام (x9 = i، x10 = len).
                    const size_t atsLoopHead = code_.size();
                    if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !ldrSlot(10, strHeapBaseSlot_ + 0) || !cmp(9, 10))
                        return false;
                    size_t atsLoopDone;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsLoopDone))
                        return false;
                    // (AR) الفاصلُ «، »: إن i>0 اكتب ',' ثمّ ' ' وقدّمِ dst بـ٢ (x9 لا يزال = i).
                    if (!cmp(9, a64reg::kXzr))
                        return false;
                    size_t atsNoSep;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", atsNoSep))
                        return false;
                    if (!ldrSlot(12, strHeapBaseSlot_ + 3) ||
                        !movz(9, kAsciiComma) || !strb(9, 12) || !addImm(12, 12, 1) ||
                        !movz(9, kAsciiSpace) || !strb(9, 12) || !addImm(12, 12, 1) ||
                        !strSlot(12, strHeapBaseSlot_ + 3))
                        return false;
                    if (!patchBranchFwd(atsNoSep, 23, 5))
                        return false;
                    // (AR) العنصرُ = data[i] (i64): elemPtr = data + i*8 ⇒ x9.
                    if (!ldrSlot(11, strHeapBaseSlot_ + 1) || !ldrSlot(12, strHeapBaseSlot_ + 4) ||
                        !addLsl3(11, 11, 12) || !ldrBase(9, 11, 0))
                        return false;
                    // (AR) itoa(x9) تنازليًّا في مخزنِ الإطار؛ x13 = المؤشّر، x14 = القمّة (للطول).
                    if (!addImm(13, 31, static_cast<long long>(printBufTopSlot_) * 8) || !movReg(14, 13) ||
                        !movz(10, kItoaRadixArm64))
                        return false;
                    if (!cmp(9, a64reg::kXzr)) // (AR) الإشارة: x9 ≥ ٠ ⇒ الموجب
                        return false;
                    size_t atsPositive;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", atsPositive))
                        return false;
                    const size_t atsNegLoop = code_.size();
                    if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                        !rrr(a64::mnem::kSub, 12, 31, 12) || !addImm(12, 12, kAsciiZeroArm64) ||
                        !subImm(13, 13, 1) || !strb(12, 13) ||
                        !movReg(9, 11) || !emitCbnzBack(9, atsNegLoop))
                        return false;
                    if (!movz(12, kAsciiMinusArm64) || !subImm(13, 13, 1) || !strb(12, 13)) // '-'
                        return false;
                    size_t atsItoaDone;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", atsItoaDone))
                        return false;
                    if (!patchBranchFwd(atsPositive, 23, 5))
                        return false;
                    const size_t atsPosLoop = code_.size();
                    if (!rrr(a64::mnem::kSdiv, 11, 9, 10) || !msub(12, 11, 10, 9) ||
                        !addImm(12, 12, kAsciiZeroArm64) ||
                        !subImm(13, 13, 1) || !strb(12, 13) ||
                        !movReg(9, 11) || !emitCbnzBack(9, atsPosLoop))
                        return false;
                    if (!patchBranchFwd(atsItoaDone, 25, 0))
                        return false;
                    // (AR) L = x14 − x13؛ byteCopy(dst, x13, L). dst يتقدّم ⇒ خزّنه ثانيةً.
                    if (!rrr(a64::mnem::kSub, 11, 14, 13) || !movReg(10, 13) ||
                        !ldrSlot(9, strHeapBaseSlot_ + 3) || !byteCopy(9, 10, 11, 12) ||
                        !strSlot(9, strHeapBaseSlot_ + 3))
                        return false;
                    // (AR) i++؛ عُدْ للرأس.
                    if (!ldrSlot(9, strHeapBaseSlot_ + 4) || !addImm(9, 9, 1) ||
                        !strSlot(9, strHeapBaseSlot_ + 4) || !emitBBack(atsLoopHead))
                        return false;
                    if (!patchBranchFwd(atsLoopDone, 23, 5))
                        return false;
                    // (AR) الختام: ']' ثمّ NUL في dst؛ النتيجة = buf.
                    if (!ldrSlot(12, strHeapBaseSlot_ + 3) ||
                        !movz(9, kAsciiRBracket) || !strb(9, 12) || !addImm(12, 12, 1) ||
                        !movz(9, 0) || !strb(9, 12))
                        return false;
                    if (!ldrSlot(a64reg::kScratch0, strHeapBaseSlot_ + 2)) // (AR) النتيجة = buf (خارجَ الحوض)
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::F64_TO_STRING:
                {
                    // (AR) عشريّ → نصّ (مرآةُ x86): يفكّك البتّاتِ (إشارة/جزءٌ صحيح/كسرٌ مقرَّبٌ ٦ خاناتٍ مع
                    //      حذفِ الأصفار الزائدة كـemitPrintFloat) ثمّ يبني النصَّ تنازليًّا في مخزنِ كومةٍ منتهيًا
                    //      بـNUL ويُعيد مؤشّرًا لأوّلِ رمز. 🔴 المؤشّرُ وسطُ المخزن ⇒ غيرُ قابلٍ لـFREE (تسريبٌ
                    //      مطابقٌ لـLLVM). خانات strHeap: 0=ip 1=scaled 2=nd 3=sign (تبقى عبر mmap).
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) فُكّ العشريَّ (نمطُ بتّاته في x9) إلى ip/scaled/nd/sign **قبل** mmap.
                    if (!loadArgInto(9, inst.operands[0]))
                        return false;
                    // sign = (bits & signmask) ? 1 : 0.
                    if (!movImm64Bits(a64reg::kScratch0, kF64SignMask) ||
                        !rrr(a64::mnem::kAnd, 11, 9, a64reg::kScratch0) || !cmp(11, 31))
                        return false;
                    size_t signZero;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", signZero))
                        return false;
                    if (!movz(11, 1))
                        return false;
                    size_t afterSign;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", afterSign))
                        return false;
                    if (!patchBranchFwd(signZero, 23, 5))
                        return false;
                    if (!movz(11, 0))
                        return false;
                    if (!patchBranchFwd(afterSign, 25, 0))
                        return false;
                    if (!strSlot(11, strHeapBaseSlot_ + 3)) // sign
                        return false;
                    // |x| = bits & absmask ⇒ ip = trunc(|x|) في x15.
                    if (!movImm64Bits(a64reg::kScratch0, kF64AbsMask) || !rrr(a64::mnem::kAnd, 9, 9, a64reg::kScratch0))
                        return false;
                    if (!fmovToFp(kD0, 9) || !fcvtzs(15, kD0))
                        return false;
                    // scaled = round_nearest(( |x| − ip ) × ١٠^٦) في x12.
                    if (!fmovToFp(kD0, 9) || !scvtf(kD1, 15) || !fsub(kD0, kD0, kD1))
                        return false;
                    if (!loadFloatConst(a64reg::kScratch0, static_cast<double>(kFloatPrecisionScale)) ||
                        !fmovToFp(kD1, a64reg::kScratch0) || !fmul(kD0, kD0, kD1) || !fcvtns(12, kD0))
                        return false;
                    // ترحيلُ الحمل: scaled == ١٠^٦ ⇒ ip++ وscaled=0.
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kFloatPrecisionScale)) ||
                        !cmp(12, a64reg::kScratch0))
                        return false;
                    size_t noCarry;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", noCarry))
                        return false;
                    if (!addImm(15, 15, 1) || !movz(12, 0))
                        return false;
                    if (!patchBranchFwd(noCarry, 23, 5))
                        return false;
                    if (!strSlot(15, strHeapBaseSlot_ + 0)) // ip
                        return false;
                    // حذفُ الأصفار الزائدة: nd(x13)=٦؛ x10=١٠؛ بينما (nd>1 && scaled%10==0) scaled/=10، nd--.
                    if (!movz(13, kFloatDecimals) || !movz(10, kItoaRadixArm64) || !movz(a64reg::kScratch1, 1))
                        return false;
                    size_t stripTop = code_.size();
                    if (!cmp(13, a64reg::kScratch1))
                        return false;
                    size_t stripDone;
                    if (!emitBranchFwd(a64::mnem::kBle, "rel19", stripDone))
                        return false;
                    if (!rrr(a64::mnem::kSdiv, 14, 12, 10) || !msub(11, 14, 10, 12) || !cmp(11, 31))
                        return false;
                    size_t stripStop;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", stripStop))
                        return false;
                    if (!movReg(12, 14) || !subImm(13, 13, 1) || !emitBBack(stripTop))
                        return false;
                    if (!patchBranchFwd(stripDone, 23, 5) || !patchBranchFwd(stripStop, 23, 5))
                        return false;
                    if (!strSlot(12, strHeapBaseSlot_ + 1) || !strSlot(13, strHeapBaseSlot_ + 2)) // scaled, nd
                        return false;
                    // (AR) خصّصْ مخزنَ الكومة (kFtoaBufPayload + رأسٌ خفيّ).
                    if (!movz(a64reg::kX1, kFtoaBufPayload + kHeapHdrBytes) || !emitMmapArm64PresetSize())
                        return false; // (AR) x0 = base
                    if (!movImm64Bits(a64reg::kScratch0, static_cast<unsigned long long>(kHeapMagic)) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, kHeapMagicOff / kArrSlotBytes) ||
                        !strBase(a64reg::kX1, a64reg::kX0, kHeapSizeOff / kArrSlotBytes))
                        return false;
                    if (!addImm(a64reg::kX0, a64reg::kX0, kHeapHdrBytes)) // (AR) x0 = buf
                        return false;
                    // (AR) x13 = buf + payload (قمّةٌ حصريّة)؛ NUL في *(--x13). x10=١٠.
                    if (!addImm(13, a64reg::kX0, kFtoaBufPayload) || !subImm(13, 13, 1) ||
                        !movz(12, 0) || !strb(12, 13) || !movz(10, kItoaRadixArm64))
                        return false;
                    // (١) خاناتُ الكسر تنازليًّا (nd خانةً؛ أصفارٌ بادئةٌ تُحشى تلقائيًّا).
                    if (!ldrSlot(9, strHeapBaseSlot_ + 1) || !ldrSlot(11, strHeapBaseSlot_ + 2)) // x9=scaled, x11=nd
                        return false;
                    size_t fracTop = code_.size();
                    if (!cmp(11, 31))
                        return false;
                    size_t fracDone;
                    if (!emitBranchFwd(a64::mnem::kBle, "rel19", fracDone))
                        return false;
                    if (!rrr(a64::mnem::kSdiv, 14, 9, 10) || !msub(12, 14, 10, 9) ||
                        !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                        !movReg(9, 14) || !subImm(11, 11, 1) || !emitBBack(fracTop))
                        return false;
                    if (!patchBranchFwd(fracDone, 23, 5))
                        return false;
                    // (٢) النقطة.
                    if (!subImm(13, 13, 1) || !movz(12, kAsciiDotArm64) || !strb(12, 13))
                        return false;
                    // (٣) الجزءُ الصحيح ip تنازليًّا (رقمٌ واحدٌ على الأقلّ؛ ip ≥ ٠).
                    if (!ldrSlot(9, strHeapBaseSlot_ + 0))
                        return false;
                    size_t ipTop = code_.size();
                    if (!rrr(a64::mnem::kSdiv, 14, 9, 10) || !msub(12, 14, 10, 9) ||
                        !addImm(12, 12, kAsciiZeroArm64) || !subImm(13, 13, 1) || !strb(12, 13) ||
                        !movReg(9, 14) || !emitCbnzBack(9, ipTop))
                        return false;
                    // (٤) بادئةُ السالب إن لزم.
                    if (!ldrSlot(12, strHeapBaseSlot_ + 3) || !cmp(12, 31))
                        return false;
                    size_t noSign;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", noSign))
                        return false;
                    if (!subImm(13, 13, 1) || !movz(12, kAsciiMinusArm64) || !strb(12, 13))
                        return false;
                    if (!patchBranchFwd(noSign, 23, 5))
                        return false;
                    if (!movReg(a64reg::kScratch0, 13)) // (AR) النتيجة = x13 ⇒ خارجَ الحوض
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::STRING_TO_F64:
                {
                    // (AR) نصّ → عشريّ (atof مبسّط، مرآةُ x86): [إشارة][أرقام][.][أرقام]. نتراكمُ كلَّ الأرقام
                    //      (متجاهلين النقطة) في متراكمٍ double: acc = acc×١٠ + رقم؛ ونعدّ خاناتِ الكسر fd، ثمّ
                    //      result = acc ÷ ١٠^fd، ثمّ الإشارة (٠−acc). النتيجةُ نمطُ بتّاتٍ في dst. d2=المتراكم،
                    //      d3=١٠٫٠. يدهس الحوضَ ⇒ نسكٌ حولَه. x9=المؤشّر x11=الإشارة x14=fd x10=علمُ النقطة.
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], 9, true))
                        return false;
                    // d2 = acc = 0.0 ؛ d3 = 10.0 (ثابتُ التراكم والقسمة).
                    if (!movz(15, 0) || !scvtf(kD2, 15))
                        return false;
                    if (!loadFloatConst(a64reg::kScratch0, static_cast<double>(kItoaRadixArm64)) || !fmovToFp(kD3, a64reg::kScratch0))
                        return false;
                    // الإشارة: أوّلُ بايتٍ '-' ⇒ x11=1 وتقدّم (لا نعالج '+': غيرُ رقمٍ يُنهي).
                    if (!movz(11, 0))
                        return false;
                    if (!ldrb(12, 9) || !movz(13, kAsciiMinusArm64) || !cmp(12, 13))
                        return false;
                    size_t notMinus;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", notMinus))
                        return false;
                    if (!movz(11, 1) || !addImm(9, 9, 1))
                        return false;
                    if (!patchBranchFwd(notMinus, 23, 5))
                        return false;
                    // x14 = fd (خاناتُ الكسر)، x10 = علمُ «مررنا بالنقطة».
                    if (!movz(14, 0) || !movz(10, 0))
                        return false;
                    const size_t head = code_.size();
                    if (!ldrb(12, 9))
                        return false;
                    // النقطة ⇒ فعّلْ علمَ الكسر وتقدّم (الثانيةُ تُعامَل كغيرِ رقمٍ فتُنهي).
                    if (!movz(13, kAsciiDotArm64) || !cmp(12, 13))
                        return false;
                    size_t notDot;
                    if (!emitBranchFwd(a64::mnem::kBne, "rel19", notDot))
                        return false;
                    if (!movz(10, 1) || !addImm(9, 9, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(notDot, 23, 5))
                        return false;
                    // غيرُ رقمٍ (< '0' أو > '9') ⇒ انتهى.
                    if (!movz(13, kAsciiZeroArm64) || !cmp(12, 13))
                        return false;
                    size_t doneLo;
                    if (!emitBranchFwd(a64::mnem::kBlt, "rel19", doneLo))
                        return false;
                    if (!movz(13, kAsciiNineArm64) || !cmp(12, 13))
                        return false;
                    size_t doneHi;
                    if (!emitBranchFwd(a64::mnem::kBgt, "rel19", doneHi))
                        return false;
                    // رقم: acc = acc×١٠ + (بايت−'0')؛ fd += x10 (١ إن بعد النقطة، ٠ قبلها) ⇒ بلا فرع.
                    if (!fmul(kD2, kD2, kD3) || !movz(13, kAsciiZeroArm64) || !rrr(a64::mnem::kSub, 12, 12, 13) ||
                        !scvtf(kD0, 12) || !fadd(kD2, kD2, kD0))
                        return false;
                    if (!rrr(a64::mnem::kAdd, 14, 14, 10) || !addImm(9, 9, 1) || !emitBBack(head))
                        return false;
                    if (!patchBranchFwd(doneLo, 23, 5) || !patchBranchFwd(doneHi, 23, 5))
                        return false;
                    // القسمةُ على ١٠^fd: بينما fd>0: acc /= 10، fd--.
                    size_t divTop = code_.size();
                    if (!cmp(14, 31))
                        return false;
                    size_t divDone;
                    if (!emitBranchFwd(a64::mnem::kBle, "rel19", divDone))
                        return false;
                    if (!fdiv(kD2, kD2, kD3) || !subImm(14, 14, 1) || !emitBBack(divTop))
                        return false;
                    if (!patchBranchFwd(divDone, 23, 5))
                        return false;
                    // الإشارة: إن x11==1 فالنتيجة = ٠−acc، وإلّا acc مباشرةً.
                    if (!cmp(11, 31))
                        return false;
                    size_t noNeg;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", noNeg))
                        return false;
                    if (!movz(15, 0) || !scvtf(kD0, 15) || !fsub(kD0, kD0, kD2) || !fmovFromFp(a64reg::kScratch0, kD0))
                        return false;
                    size_t signApplied;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", signApplied))
                        return false;
                    if (!patchBranchFwd(noNeg, 23, 5))
                        return false;
                    if (!fmovFromFp(a64reg::kScratch0, kD2))
                        return false;
                    if (!patchBranchFwd(signApplied, 25, 0))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kScratch0);
                }
                case OP::NOT:
                case OP::NEG:
                    return driveUnary(inst);
                case OP::EQ:
                case OP::NE:
                case OP::LT:
                case OP::LE:
                case OP::GT:
                case OP::GE:
                    return driveComparison(inst);
                case OP::ALLOC:
                {
                    // (AR) الخانةُ خُصِّصت في المسح المسبق؛ لا شيفرةَ (العنوان ضمنيٌّ [sp، #فهرس×٨]).
                    if (!inst.result || memSlot_.find(inst.result->name) == memSlot_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kAllocUnslotted + detailOpcode(inst));
                    // (AR) الكائنيّة: ALLOC بمعامل نصّيّ = اسمُ صنفٍ ⇒ كائنُ كومة (نظيرُ x86).
                    if (!inst.operands.empty() &&
                        inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                        inst.operands[0].dataType == types::SadTypeKind::String)
                    {
                        const std::string &className = inst.operands[0].name;
                        auto ci = classLayout_.find(className);
                        if (ci == classLayout_.end())
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectUnknownClass + className);
                        if (!ci->second.allEightByte)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectFieldLayout + className);
                        long long size = 8LL * (ci->second.numFields + (ci->second.isCRepr ? 0 : 1));
                        if (size < 8)
                            size = 8;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!spillReg(kv.second))
                                    return false;
                        if (!emitMmapArm64(size)) // x0 = المؤشّر (mmap يصفّر الكتلة)
                            return false;
                        objClassOf_[inst.result->name] = className;
                        // (AR) الإرسالُ الافتراضيّ (الدفعة ٦): خزّنْ عنوانَ جدولِ الدوالّ في obj[0]
                        //      (ترويسةُ vtable). العنوانُ يُرقَّع زمنَ الإنهاء. x0=المؤشّر، x16 خدشٌ.
                        if (!ci->second.isCRepr && vtableBaseOff_.count(className))
                        {
                            if (!emitVtableAddr(a64reg::kScratch0, className) ||
                                !strBase(a64reg::kScratch0, a64reg::kX0, 0)) // obj[0] = عنوانُ الجدول
                                return false;
                        }
                        // (AR) خزّنِ المؤشّرَ في خانةِ النتيجة الآن (يبقى الحوضُ منسكبًا) ⇒ يصمد عبرَ
                        //      mmapاتِ حقولِ المصفوفةِ التالية، ونعيد تحميلَه منها. (مرآةُ x86).
                        if (!strSlot(a64reg::kX0, memSlot_[inst.result->name]))
                            return false;
                        // (AR) تهيئةُ حقولِ المصفوفةِ (المُهيّأة بـ[]): مرآةُ mem_alloca في LLVM وx86 — لكلِّ
                        //      حقلٍ خصّصْ SadArray فارغًا (نمطُ ARRAY_NEW: كتلةٌ واحدةٌ، cap=kArrFieldInitCap،
                        //      len=0، homogKind=0 من mmap) وخزّنْ مؤشّرَه في obj[الإزاحة]. بدونها ⇒ مؤشّرٌ
                        //      قمامةٌ (صفرٌ من mmap) ⇒ SIGSEGV عند «طول»/الإلحاق.
                        for (const long long fieldOff : ci->second.arrayFieldOffsets)
                        {
                            const long long total = kArrHeaderBytes +
                                                    kArrFieldInitCap * kArrSlotBytes +
                                                    kArrFieldInitCap * kTagSlotBytes;
                            if (!emitMmapArm64(total)) // x0 = قاعدةُ SadArray (mmap يصفّر)
                                return false;
                            if (!movz(a64reg::kScratch0, 0) ||
                                !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffLen / kArrSlotBytes))
                                return false; // len = 0
                            if (!movz(a64reg::kScratch0, kArrFieldInitCap) ||
                                !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffCap / kArrSlotBytes))
                                return false; // cap = 8
                            if (!addImm(a64reg::kScratch0, a64reg::kX0, kArrHeaderBytes) ||
                                !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffData / kArrSlotBytes))
                                return false; // data = base + 40
                            if (!addImm(a64reg::kScratch0, a64reg::kX0, kArrHeaderBytes + kArrFieldInitCap * kArrSlotBytes) ||
                                !strBase(a64reg::kScratch0, a64reg::kX0, kArrOffTags / kArrSlotBytes))
                                return false; // tags = نهايةُ البيانات (منطقةٌ مصفّرةٌ في الكتلة نفسها)
                            // (AR) خزّنْ مؤشّرَ المصفوفة (x0) في obj[fieldOff]: أعِد تحميلَ obj في x17.
                            if (!ldrSlot(a64reg::kScratch1, memSlot_[inst.result->name]))
                                return false;
                            if (!strBase(a64reg::kX0, a64reg::kScratch1, fieldOff / kArrSlotBytes))
                                return false;
                        }
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!reloadReg(kv.second))
                                    return false;
                        return true;
                    }
                    return true;
                }
                case OP::LOAD:
                {
                    if (!inst.result)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) OBJECT_GET: LOAD بمعاملين [obj, اسمُ الحقل نصًّا] ⇒ ldr dst,[obj+إزاحة].
                    //      المؤشّرُ في x16 (خارج الحوض) كي لا يدهس نتيجةً حوضيّةً سابقة (فخّ GET_PAYLOAD).
                    if (inst.operands.size() == 2 &&
                        inst.operands[1].type == sir::SIROperandType::CONSTANT &&
                        inst.operands[1].dataType == types::SadTypeKind::String)
                    {
                        const std::string *cn = objClassForOperand(inst.operands[0]);
                        if (!cn)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectUnknownClass + inst.operands[0].name);
                        long long off;
                        if (!objFieldByteOffset(*cn, inst.operands[1].name, off))
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectFieldLayout + inst.operands[1].name);
                        if (!materialize(a64reg::kScratch0, inst.operands[0]))
                            return false;
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return ldrBase(dst, a64reg::kScratch0, off / kArrSlotBytes);
                    }
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int slot;
                    if (!isMemVar(inst.operands[0], slot))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kLoadNonslot + detailOpcode(inst));
                    if (const std::string *cn = objClassForOperand(inst.operands[0]))
                        objClassOf_[inst.result->name] = *cn;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrSlot(dst, slot);
                }
                case OP::STORE:
                {
                    // (AR) OBJECT_SET: STORE بثلاثة معاملات [value, obj, اسمُ الحقل نصًّا] ⇒ str value,[obj+إزاحة].
                    if (inst.operands.size() == 3 &&
                        inst.operands[2].type == sir::SIROperandType::CONSTANT &&
                        inst.operands[2].dataType == types::SadTypeKind::String)
                    {
                        const std::string *cn = objClassForOperand(inst.operands[1]);
                        if (!cn)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectUnknownClass + inst.operands[1].name);
                        long long off;
                        if (!objFieldByteOffset(*cn, inst.operands[2].name, off))
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectFieldLayout + inst.operands[2].name);
                        return materialize(a64reg::kScratch0, inst.operands[1]) && // x16 = obj
                               materialize(a64reg::kScratch1, inst.operands[0]) && // x17 = value
                               strBase(a64reg::kScratch1, a64reg::kScratch0, off / kArrSlotBytes);
                    }
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int slot;
                    if (!isMemVar(inst.operands[1], slot))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kStoreNonslot + detailOpcode(inst));
                    if (const std::string *cn = objClassForOperand(inst.operands[0]))
                        objClassOf_[inst.operands[1].name] = *cn;
                    // (AR) قيمةٌ نصّيّة ⇒ جسّدْ عنوانَها — مرآةُ x86 (الحجّةُ والقياسُ هناك).
                    // (EN) A string value ⇒ materialize its address — mirror of x86.
                    if (inst.operands[0].dataType == types::SadTypeKind::String)
                        return materializeString(inst.operands[0], a64reg::kScratch0, /*fromSpill=*/false) &&
                               strSlot(a64reg::kScratch0, slot);
                    return materialize(a64reg::kScratch0, inst.operands[0]) &&
                           strSlot(a64reg::kScratch0, slot);
                }
                case OP::CALL:
                {
                    // (AR) call @دالّة, وسائط… ⇒ ضع الوسائطَ في x0..x7 ثمّ bl (imm26 يُرقَّع لإزاحة
                    //      الدالّة)؛ النتيجةُ في x0 ⇒ سجلُّ النتيجة. النداءُ من الداخلة فقط (فُحِص).
                    // (AR) النداءُ بالاسمِ لمُساعِدِ زمنِ تشغيلٍ يُصنَّف «غيرَ مدعومٍ بعد»
                    //      لا «خطأً مترجمًا داخليًّا» — مرآةُ x86 (الحجّةُ والقياسُ هناك).
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (inst.operands[0].type != sir::SIROperandType::FUNCTION)
                    {
                        if (common::isRuntimeHelperCallee(inst.operands[0]))
                        {
                            bool handled = false;
                            if (!emitMapHelper(inst, block, instIdx, handled))
                                return false;
                            if (handled)
                                return true;
                            return fail(EC::INT_NATIVE_UNSUPPORTED,
                                        diag::kRuntimeHelper + inst.operands[0].name);
                        }
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    }
                    const size_t argc = inst.operands.size() - 1;
                    // (AR) bl يدهس x9..x15/x0..x7 (caller-saved). انسكِبْ المؤقّتاتِ الحيّةَ بعد
                    //      النداء (أو وسائطَ سجليّةً له) إلى خانات الانسكاب، حمّل الوسائطَ منها (لا
                    //      من سجلّاتها ⇒ صفر تصادمِ نقلٍ متوازٍ)، bl، أعِد الحيّةَ، النتيجةُ من x0.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) || common::isPoolArgOfCall(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) الوسائطُ ٠..٧ ⇒ x0..x7؛ ٨+ ⇒ منطقةُ الوسائطِ الصادرةِ (x16 سجلُّ النقل؛ النداءُ
                    //      المباشرُ bl لا يستعملُه). النصُّ يُجسَّد (حرفيّةُ rodata/مؤشّرُ كومة).
                    for (size_t i = 0; i < argc; ++i)
                        if (!passAbiArg(i, inst.operands[i + 1], a64reg::kScratch0))
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
                case OP::OBJECT_CALL:
                {
                    // (AR) الإرسالُ الافتراضيّ (الدفعة ٦، مرآةُ x86): operands=[obj, اسمُ الطريقة(نصّ), وسائط…].
                    //      vtable=[obj+0]، مؤشّرُ الدالّة=[vtable+خانة×٨]، ثمّ blr (self=x0). الفهرسُ من
                    //      الصنفِ الساكن؛ جدولُ الكائنِ زمنَ التشغيلِ يحملُ التجاوزَ في نفسِ الخانة ⇒ إرسالٌ صحيح.
                    if (inst.operands.size() < 2 ||
                        inst.operands[1].type != sir::SIROperandType::CONSTANT ||
                        inst.operands[1].dataType != types::SadTypeKind::String)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const std::string *cn = objClassForOperand(inst.operands[0]);
                    if (!cn)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectUnknownClass + inst.operands[0].name);
                    auto lit = classVtableLayout_.find(*cn);
                    if (lit == classVtableLayout_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectNoVtable + *cn);
                    int slot = -1;
                    for (size_t i = 0; i < lit->second.size(); ++i)
                        if (lit->second[i].first == inst.operands[1].name) { slot = static_cast<int>(i); break; }
                    if (slot < 0)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kObjectMethodNoSlot + inst.operands[1].name);
                    const size_t argc = inst.operands.size() - 2; // (AR) الوسائطُ الإضافيّة (عدا self)
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) || common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) الوسائطُ الإضافيّةُ عند الفتحاتِ المنطقيّة ١..argc (self=٠): x1..x7 للفتحاتِ
                    //      <٨، والمكدّسُ للفتحاتِ ٨+ (x16 سجلُّ النقل، قبلَ ضبطِ self/الهدف). ثمّ self → x0.
                    for (size_t i = 0; i < argc; ++i)
                        if (!passAbiArg(i + 1, inst.operands[i + 2], a64reg::kScratch0))
                            return false;
                    if (!loadArgInto(a64reg::kX0, inst.operands[0]))
                        return false;
                    // (AR) x16 (خارجَ الحوض): vtable=[x0+0]، مؤشّرُ الدالّة=[x16+خانة] (فهرسٌ مقيسٌ ٨).
                    if (!ldrBase(a64reg::kScratch0, a64reg::kX0, 0) ||
                        !ldrBase(a64reg::kScratch0, a64reg::kScratch0, static_cast<long long>(slot)))
                        return false;
                    if (!emitBlr(a64reg::kScratch0)) // (AR) blr x16 (نداءٌ غيرُ مباشر)
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
                case OP::OBJECT_NULL_CHECK:
                {
                    // (AR) حارسُ المُستقبِلِ العدميّ (مرآةُ x86): operands=[المُستقبِل، سياقُ النداء(نصّ)]،
                    //      ولا نتيجة. يُحمَّل المُستقبِلُ في x0 ثمّ يُقارَن بصفر: صفرٌ ⇒ exit(132).
                    if (inst.operands.size() < 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) مُستقبِلٌ لا خانةَ له في هذا الإطار: لا حارسَ ولا تشخيص — عقدُ خلفيّةِ
                    //      LLVM نفسُه (الحارسُ إضافةٌ إلى مسارٍ قائمٍ لا شرطٌ لصحّتِه).
                    const sir::SIROperand &recv = inst.operands[0];
                    if (recv.type != sir::SIROperandType::REGISTER)
                        return true; // (AR) ثابتٌ مُستقبِلًا: ليس عدمًا بالبناء
                    int recvSlot;
                    if (!isMemVar(recv, recvSlot) && regOf_.find(recv.name) == regOf_.end())
                        return true;
                    if (!loadArgInto(a64reg::kX0, recv))
                        return false;
                    return emitNullReceiverGuardArm64();
                }
                case OP::CLOSURE_CREATE:
                {
                    // (AR) الإغلاقات (الدفعة ٧، مرآةُ x86): operands=[@دالّة, ملتقَط٠، …]. تخصيصٌ واحدٌ
                    //      مدموج mmap(16 + N×8): [base]=عنوانُ الدالّة، [base+8]=البيئة=base+16 (أو صفر)،
                    //      [base+16+i×8]=الملتقَطُ i. النتيجةُ=base. عنوانُ الدالّةِ يُرقَّع (funcAddrFixups_).
                    if (!inst.result || inst.operands.empty() ||
                        inst.operands[0].type != sir::SIROperandType::FUNCTION)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const long long numCaptures = static_cast<long long>(inst.operands.size()) - 1;
                    const long long allocSize = 16 + numCaptures * 8;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    if (!emitMmapArm64(allocSize)) // x0 = base (mmap يصفّر الكتلة)
                        return false;
                    // (AR) [base+0] = عنوانُ الدالّة (نائبٌ يُرقَّع). x16 خدشٌ خارجَ الحوض.
                    if (!emitFuncAddr(a64reg::kScratch0, inst.operands[0].name) ||
                        !strBase(a64reg::kScratch0, a64reg::kX0, 0))
                        return false;
                    if (numCaptures > 0)
                    {
                        // (AR) [base+8] = البيئة = base + 16 (الفهرسُ المقيسُ ١).
                        if (!addImm(a64reg::kScratch0, a64reg::kX0, 16) ||
                            !strBase(a64reg::kScratch0, a64reg::kX0, 1))
                            return false;
                        // (AR) [base+16+i×8] = الملتقَطُ i (الفهرسُ المقيسُ ٢+i؛ يُقرأ من خانةِ انسكابه).
                        for (long long i = 0; i < numCaptures; ++i)
                            if (!loadArgInto(a64reg::kScratch0, inst.operands[static_cast<size_t>(i + 1)]) ||
                                !strBase(a64reg::kScratch0, a64reg::kX0, 2 + i))
                                return false;
                    }
                    else if (!strBase(a64reg::kXzr, a64reg::kX0, 1))
                        return false; // (AR) بلا التقاطٍ ⇒ البيئة = صفر
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kX0);
                }
                case OP::CLOSURE_CALL:
                {
                    // (AR) نداءُ إغلاق (الدفعة ٧): operands=[مؤشّرُ الإغلاق, وسيط٠، …]. البنيةُ {fn@0, env@8}.
                    //      الوسائطُ الصريحةُ في x0..؛ البيئةُ وسيطًا أخيرًا (عقدُ اللامدا: __env أخيرًا)؛
                    //      ثمّ blr [الإغلاق+0]. x16 خارجَ الحوضِ (لا يدهس وسيطًا).
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const size_t argc = inst.operands.size() - 1; // (AR) الوسائطُ الصريحة (عدا البيئة)
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) البيئةُ هي الوسيطُ المنطقيُّ الأخيرُ (الفتحةُ argc). الترتيبُ الآمن (مرآةُ x86):
                    //      (١) الوسائطُ الصريحةُ المكدّسةُ عبر x16 (نقلٌ حرٌّ الآن)؛ (٢) مؤشّرُ الإغلاق → x16؛
                    //      (٣) البيئةُ من [الإغلاق+8] إلى سجلٍّ (argc<8) أو المكدّس عبر x17؛ (٤) الوسائطُ
                    //      السجليّةُ (٠..٧)؛ (٥) أعِد مؤشّرَ الإغلاق (قد دهسته الوسائطُ السجليّة) ثمّ الدالّةَ.
                    for (size_t i = 8; i < argc; ++i)
                        if (!passAbiArg(i, inst.operands[i + 1], a64reg::kScratch0))
                            return false;
                    if (!loadArgInto(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    if (argc < 8)
                    {
                        if (!ldrBase(abiArg_[argc], a64reg::kScratch0, 1)) // (AR) البيئةُ في سجلّ
                            return false;
                    }
                    else if (!ldrBase(a64reg::kScratch1, a64reg::kScratch0, 1) || // (AR) البيئةُ على المكدّس
                             !strSlot(a64reg::kScratch1, static_cast<int>(argc - 8)))
                        return false;
                    for (size_t i = 0; i < argc && i < 8; ++i)
                        if (!loadArgInto(abiArg_[i], inst.operands[i + 1]))
                            return false;
                    if (!loadArgInto(a64reg::kScratch0, inst.operands[0]) || // (AR) أعِد الإغلاق (قد دُهس)
                        !ldrBase(a64reg::kScratch0, a64reg::kScratch0, 0))    // (AR) مؤشّرُ الدالّة = [الإغلاق+0]
                        return false;
                    if (!emitBlr(a64reg::kScratch0))
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
                        return movReg(dst, a64reg::kX0);
                    }
                    return true;
                }
                case OP::CALL_INDIRECT:
                {
                    // (AR) نداءٌ عبر مؤشّرِ دالّة (الدفعة ٧): operands=[مؤشّرُ الدالّة, وسيط٠، …]. لا بيئةَ.
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const size_t argc = inst.operands.size() - 1;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) الوسائطُ ٠..٧ ⇒ x0..x7؛ ٨+ ⇒ المكدّس (x16 سجلُّ النقل؛ الهدفُ يُحمَّل بعده).
                    for (size_t i = 0; i < argc; ++i)
                        if (!passAbiArg(i, inst.operands[i + 1], a64reg::kScratch0))
                            return false;
                    // (AR) مؤشّرُ الدالّة → x16: معاملُ FUNCTION ⇒ عنوانٌ مُرقَّع؛ وإلّا سجلّ/خانة.
                    if (inst.operands[0].type == sir::SIROperandType::FUNCTION)
                    {
                        if (!emitFuncAddr(a64reg::kScratch0, inst.operands[0].name))
                            return false;
                    }
                    else if (!loadArgInto(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    if (!emitBlr(a64reg::kScratch0))
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
                        return movReg(dst, a64reg::kX0);
                    }
                    return true;
                }
                case OP::ENV_LOAD:
                {
                    // (AR) تحميلُ ملتقَطٍ من البيئة (الدفعة ٧): operands=[%__env, فهرس]. dst = [env + فهرس×8].
                    //      الفهرسُ في ldrBase مقيسٌ (فهرسُ الملتقَط = الفهرسُ المقيسُ عينُه).
                    if (!requireArity(inst, 2))
                        return false;
                    long long idx;
                    if (!common::isConstInt(inst.operands[1], idx) || idx < 0)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!materialize(a64reg::kScratch0, inst.operands[0])) // x16 = مؤشّرُ البيئة
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrBase(dst, a64reg::kScratch0, idx);
                }
                case OP::ENV_STORE:
                {
                    // (AR) تخزينُ ملتقَطٍ في البيئة (الدفعة ٧): operands=[قيمة, %__env, فهرس]. [env+فهرس×8]=قيمة.
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long idx;
                    if (!common::isConstInt(inst.operands[2], idx) || idx < 0)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    return materialize(a64reg::kScratch0, inst.operands[1]) && // x16 = مؤشّرُ البيئة
                           materialize(a64reg::kScratch1, inst.operands[0]) && // x17 = القيمة
                           strBase(a64reg::kScratch1, a64reg::kScratch0, idx);
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
                        {
                            if (!materializeString(op, 9, /*fromSpill=*/true) ||
                                !emitPrintStrPtrArm64(9))
                                return fail(EC::INT_NATIVE_UNSUPPORTED,
                                            diag::kPrintStrComputed + diag::kVregSigil + op.name);
                        }
                        else if (op.dataType == types::SadTypeKind::Any)
                        {
                            // (AR) معلَّب: المعاملُ مؤشّرٌ إلى خانةِ dyn ⇒ طباعةٌ مبوَّبةٌ زمنَ التشغيل.
                            if (!loadArgInto(9, op) || !emitPrintBoxed(9))
                                return false;
                        }
                        else if (op.dataType == types::SadTypeKind::Boolean)
                        {
                            // (AR) منطقيّ: «صحيح»/«خطأ» كالمعلَّب والمفسّر، لا «1»/«0».
                            if (!loadArgInto(9, op) || !emitPrintBool(9))
                                return false;
                        }
                        else if (op.dataType == types::SadTypeKind::Float)
                        {
                            // (AR) عشريّ: حمّل نمطَ بتّاته في x9 ثمّ المُنسِّق (fixed6 + حذفُ الأصفار).
                            if (!loadArgInto(9, op) || !emitPrintFloat())
                                return false;
                        }
                        else if (op.dataType == types::SadTypeKind::UInt64)
                        {
                            // (AR) طبيعي64: حمّله في x9 ثمّ itoa **لا-موقَّع** (يطابقُ المفسّرَ ومرآةَ x86).
                            if (!loadArgInto(9, op) || !emitPrintUInt())
                                return false;
                        }
                        else
                        {
                            // (AR) عددٌ صحيح موقَّع: حمّله في x9 (ثابت/ذاكرة/خانة انسكاب) ثمّ itoa+write.
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
                    //      loadScalarInto يفكّ Any المعلَّب ⇒ رمزُ الخروج/العائدُ القيمةَ الخام لا المؤشّر.
                    if (curIsEntry_)
                        return loadScalarInto(a64reg::kX0, inst.operands[0]) &&
                               movz(a64reg::kX8, kSysExitArm64) &&
                               emit(a64::mnem::kSvc, "", {});
                    return loadScalarInto(a64reg::kX0, inst.operands[0]) && emitEpilogue();
                }
                case OP::RET_VOID:
                {
                    // (AR) إرجاعٌ فارغ: الداخلةُ تخرج بـexit(0)؛ غيرُها خاتمةٌ + ret بلا ضبطِ x0.
                    if (curIsEntry_)
                        return movz(a64reg::kX0, 0) && movz(a64reg::kX8, kSysExitArm64) &&
                               emit(a64::mnem::kSvc, "", {});
                    return emitEpilogue();
                }
                case OP::TUPLE_NEW: // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً وتخطيطًا ⇒ نفسُ الخفض (مطابقٌ لمسار LLVM)
                case OP::ARRAY_NEW:
                {
                    // (AR) result = مصفوفةٌ جديدة؛ operands=[len(const), cap(const)]. mmap كتلةً
                    //      واحدة (رأسٌ ٤٠ + بياناتٌ cap×٨)، تهيئةُ len/cap/data؛ tags/homogKind
                    //      يُصفّرهما mmap. السعةُ ثابتةٌ؛ الحجمُ الديناميّ مؤجَّلٌ ⇒ فشلٌ صريح.
                    if (!requireArity(inst, 2))
                        return false;
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
                    //      القيمةُ النصّيّةُ الحرفيّة (مصفوفةُ نصوصٍ متجانسة) تُجسَّد مؤشّرًا؛ materialize
                    //      يرفض حرفيّةَ النصّ (شبحيّةٌ بلا سجلّ) ⇒ يفتح مصفوفةَ نصوصٍ لـARRAY_TO_STRING.
                    if (inst.operands[2].dataType == types::SadTypeKind::String)
                    {
                        // (AR) fromSpill=false: ARRAY_SET لا يَنسِك الحوضَ ⇒ القيمةُ المحسوبةُ تُقرأ من
                        //      سجلّها الحيّ (movReg) لا من خانةِ انسكابٍ باتّةٍ؛ الحرفيّةُ تمرّ بـemitLoadCStrAddr.
                        if (!materializeString(inst.operands[2], a64reg::kScratch1, false))
                            return false;
                    }
                    else if (!materialize(a64reg::kScratch1, inst.operands[2]))
                        return false;
                    return strBase(a64reg::kScratch1, a64reg::kScratch0, off);
                }
                case OP::TUPLE_GET: // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً ⇒ نفسُ خفضِ القراءة (بما فيه المسارُ المعلَّب)
                case OP::ARRAY_GET:
                {
                    // (AR) result = arr[index]. نحسب عنوانَ العنصر ثمّ نحمّله في الوجهة.
                    if (!requireArity(inst, 2))
                        return false;
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
                        int ts = 0; // (AR) عبر المُخصِّصِ المحروس (لا عدَّ يدويّ)
                        if (!takeDynSlot(ts))
                            return false;
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
                case OP::TUPLE_LEN: // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً ⇒ الطولُ في [arr+0] كالمصفوفة
                case OP::ARRAY_LEN:
                {
                    // (AR) result = طول(arr) ⇒ [arr+0].
                    if (!requireArity(inst, 1))
                        return false;
                    if (!materialize(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrBase(dst, a64reg::kScratch0, kArrOffLen / kArrSlotBytes);
                }
                case OP::BUILTIN_FILE_WRITE_BYTES:
                {
                    // (AR) اكتب_بايتات(مسار، مصفوفة) ⇒ منطقيّ — مرآةُ x86 بنداءاتِ AArch64.
                    //      الفارقُ الحقيقيّ: لا نداءَ open في هذا الـABI ⇒ openat(AT_FDCWD, …).
                    //      البايتُ الصفريُّ لا يقطع شيئًا (الطولُ من رأس المصفوفة) — سببُ وجودِ
                    //      المدمجة: رأسُ ELF لا يُكتَب بمسارٍ نصّيٍّ يقف عند أوّل 0x00.
                    //      خانات: 0=fd 1=len 2=buf 3=data 4=i 5=النتيجة.
                    // (EN) Mirror of the x86 case with AArch64 syscalls; the real difference
                    //      is that this ABI has no `open`, so openat(AT_FDCWD, …) is used.
                    if (!requireArity(inst, 2))
                        return false;
                    if (inst.operands[1].elementType == types::SadTypeKind::Any ||
                        inst.operands[1].elementType == types::SadTypeKind::Float)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;

                    // (AR) 🔑 اقرأ المعاملَ عبر x16/x17 (خارج الحوض) قبل لمسِ أيّ سجلّ حوض.
                    if (!materialize(a64reg::kScratch0, inst.operands[1]) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffLen / kArrSlotBytes) ||
                        !strSlot(a64reg::kScratch1, fileBytesSlot(1)) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffData / kArrSlotBytes) ||
                        !strSlot(a64reg::kScratch1, fileBytesSlot(3)))
                        return false;

                    // (AR) مخزنُ البايتات: الطول+١ ⇒ حجمٌ غيرُ صفريّ حتّى لمصفوفةٍ فارغة.
                    if (!ldrSlot(a64reg::kX1, fileBytesSlot(1)) || !addImm(a64reg::kX1, a64reg::kX1, 1) ||
                        !emitMmapArm64PresetSize() || !strSlot(a64reg::kX0, fileBytesSlot(2)))
                        return false;

                    // (AR) لولبُ التعبئة: buf[i] = data[i] & 0xFF بفحصِ i<len في الرأس.
                    if (!movz(9, 0) || !strSlot(9, fileBytesSlot(4)))
                        return false;
                    const size_t wFillHead = code_.size();
                    if (!ldrSlot(9, fileBytesSlot(4)) || !ldrSlot(10, fileBytesSlot(1)) || !cmp(9, 10))
                        return false;
                    size_t wFillDone;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", wFillDone))
                        return false;
                    if (!ldrSlot(10, fileBytesSlot(3)) || !addLsl3(10, 10, 9) || // x10 = data + i×8
                        !ldrBase(11, 10, 0) ||                                   // x11 = data[i]
                        !ldrSlot(12, fileBytesSlot(2)) || !rrr(a64::mnem::kAdd, 12, 12, 9) ||
                        !strb(11, 12))                                           // buf[i] = البايتُ الأدنى
                        return false;
                    if (!addImm(9, 9, 1) || !strSlot(9, fileBytesSlot(4)) || !emitBBack(wFillHead))
                        return false;
                    if (!patchBranchFwd(wFillDone, 23, 5))
                        return false;

                    // (AR) openat(AT_FDCWD, path, O_WRONLY|O_CREAT|O_TRUNC, 0644) ⇒ x0 = fd أو -errno.
                    if (!movConst(a64reg::kX0, kAtFdCwd) ||
                        !materializeString(inst.operands[0], a64reg::kX1, true) ||
                        !movz(a64reg::kX2, kOpenWriteCreateTrunc) || !movz(3, kFileCreateMode) ||
                        !movz(a64reg::kX8, kSysOpenatArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!strSlot(a64reg::kX0, fileBytesSlot(0)))
                        return false;

                    // (AR) fd سالبٌ ⇒ فشلُ فتح: «خطأ» بلا كتابةٍ ولا إغلاق.
                    if (!cmp(a64reg::kX0, a64reg::kXzr))
                        return false;
                    size_t wOpenFailed;
                    if (!emitBranchFwd(a64::mnem::kBlt, "rel19", wOpenFailed))
                        return false;

                    // (AR) write(fd, buf, len) ثمّ close(fd)؛ الكتابةُ الجزئيّةُ ليست نجاحًا.
                    if (!ldrSlot(a64reg::kX0, fileBytesSlot(0)) || !ldrSlot(a64reg::kX1, fileBytesSlot(2)) ||
                        !ldrSlot(a64reg::kX2, fileBytesSlot(1)) ||
                        !movz(a64reg::kX8, kSysWriteArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!strSlot(a64reg::kX0, fileBytesSlot(5)))
                        return false;
                    if (!ldrSlot(a64reg::kX0, fileBytesSlot(0)) ||
                        !movz(a64reg::kX8, kSysCloseArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!ldrSlot(9, fileBytesSlot(5)) || !ldrSlot(10, fileBytesSlot(1)) || !cmp(9, 10))
                        return false;
                    size_t wWrote;
                    if (!emitBranchFwd(a64::mnem::kBeq, "rel19", wWrote))
                        return false;

                    if (!patchBranchFwd(wOpenFailed, 23, 5))
                        return false;
                    if (!movz(9, 0)) // (AR) خطأ
                        return false;
                    size_t wEnd;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", wEnd))
                        return false;
                    if (!patchBranchFwd(wWrote, 23, 5))
                        return false;
                    if (!movz(9, 1)) // (AR) صحيح
                        return false;
                    if (!patchBranchFwd(wEnd, 25, 0))
                        return false;
                    if (!strSlot(9, fileBytesSlot(5)))
                        return false;

                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int wdst;
                    if (!allocReg(inst.result->name, wdst))
                        return false;
                    return ldrSlot(wdst, fileBytesSlot(5));
                }
                case OP::BUILTIN_FILE_READ_BYTES:
                {
                    // (AR) اقرأ_بايتات(مسار) ⇒ مصفوفةُ أعدادٍ ٠..٢٥٥ (مرآةُ x86 بنداءاتِ AArch64).
                    //      البنيةُ خماسيّةٌ: الطول=السعة، tags=عدم، homogKind=Int — كلُّ مُنتِجٍ
                    //      يُهيّئ الحقلين ٣+٤ وإلّا قرأ المستهلِكُ مؤشّرَ قمامةٍ أو وسمًا خاطئًا.
                    //      خانات: 0=fd 1=len 2=buf 3=(غير مستعملة) 4=i 5=البنية.
                    if (!requireArity(inst, 1))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;

                    // (AR) openat(AT_FDCWD, path, O_RDONLY) ⇒ fd. الفشلُ ⇒ مصفوفةٌ فارغة.
                    //      ⚠️ دَينٌ مُعلَنٌ (انحرافٌ عن المفسّر، مطابقٌ لمخفّض x86): المفسّر يرفع
                    //      RUN_FILE_ERROR، وهذا المسارُ يُعيد مصفوفةً فارغةً صامتًا.
                    if (!movConst(a64reg::kX0, kAtFdCwd) ||
                        !materializeString(inst.operands[0], a64reg::kX1, true) ||
                        !movz(a64reg::kX2, kOpenReadOnly) || !movz(3, 0) ||
                        !movz(a64reg::kX8, kSysOpenatArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!strSlot(a64reg::kX0, fileBytesSlot(0)) || !movz(9, 0) ||
                        !strSlot(9, fileBytesSlot(1))) // (AR) الطولُ الافتراضيّ = ٠
                        return false;
                    if (!cmp(a64reg::kX0, a64reg::kXzr))
                        return false;
                    size_t rOpenFailed;
                    if (!emitBranchFwd(a64::mnem::kBlt, "rel19", rOpenFailed))
                        return false;

                    // (AR) الحجمُ = lseek(fd, 0, SEEK_END) ثمّ العودةُ إلى البداية.
                    if (!ldrSlot(a64reg::kX0, fileBytesSlot(0)) || !movz(a64reg::kX1, 0) ||
                        !movz(a64reg::kX2, kSeekEnd) ||
                        !movz(a64reg::kX8, kSysLseekArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    if (!clampNonNegativeArm64(a64reg::kX0) || !strSlot(a64reg::kX0, fileBytesSlot(1)))
                        return false;
                    if (!ldrSlot(a64reg::kX0, fileBytesSlot(0)) || !movz(a64reg::kX1, 0) ||
                        !movz(a64reg::kX2, kSeekSet) ||
                        !movz(a64reg::kX8, kSysLseekArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;

                    // (AR) مخزنُ القراءة (الحجم+١)، ثمّ read(fd, buf, size) وclose.
                    if (!ldrSlot(a64reg::kX1, fileBytesSlot(1)) || !addImm(a64reg::kX1, a64reg::kX1, 1) ||
                        !emitMmapArm64PresetSize() || !strSlot(a64reg::kX0, fileBytesSlot(2)))
                        return false;
                    if (!ldrSlot(a64reg::kX0, fileBytesSlot(0)) || !ldrSlot(a64reg::kX1, fileBytesSlot(2)) ||
                        !ldrSlot(a64reg::kX2, fileBytesSlot(1)) ||
                        !movz(a64reg::kX8, kSysReadArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    // (AR) الطولُ الفعليُّ = ما قرأه read (قد يقلّ عن حجمِ lseek عند سباقٍ على الملفّ)،
                    //      وقد يكون سالبًا (‎-errno‎؛ قراءةُ مجلّدٍ ⇒ ‎-EISDIR‎). القصُّ يمنع حجمَ
                    //      تخصيصٍ سالبٍ ثمّ كتابةً على مؤشّرِ خطأ.
                    if (!clampNonNegativeArm64(a64reg::kX0) || !strSlot(a64reg::kX0, fileBytesSlot(1)))
                        return false;
                    if (!ldrSlot(a64reg::kX0, fileBytesSlot(0)) ||
                        !movz(a64reg::kX8, kSysCloseArm64) || !emit(a64::mnem::kSvc, "", {}))
                        return false;
                    size_t rHaveBuf;
                    if (!emitBranchFwd(a64::mnem::kB, "rel26", rHaveBuf))
                        return false;

                    // (AR) مسارُ فشلِ الفتح: مخزنُ بايتٍ واحدٍ وطولٌ صفريّ ⇒ مصفوفةٌ فارغةٌ سليمةُ البنية.
                    if (!patchBranchFwd(rOpenFailed, 23, 5))
                        return false;
                    if (!movz(a64reg::kX1, 1) || !emitMmapArm64PresetSize() ||
                        !strSlot(a64reg::kX0, fileBytesSlot(2)))
                        return false;
                    if (!patchBranchFwd(rHaveBuf, 25, 0))
                        return false;

                    // (AR) بنيةُ SadArray: الرأسُ + الطولُ×٨ (+٨ كي لا يكون الحجمُ صفريًّا).
                    if (!ldrSlot(9, fileBytesSlot(1)) || !movz(10, 3) ||
                        !rrr(a64::mnem::kLslv, a64reg::kX1, 9, 10) ||
                        !addImm(a64reg::kX1, a64reg::kX1, kArrHeaderBytes + kArrSlotBytes) ||
                        !emitMmapArm64PresetSize())
                        return false;
                    if (!strSlot(a64reg::kX0, fileBytesSlot(5)))
                        return false;
                    if (!ldrSlot(9, fileBytesSlot(1)) ||
                        !strBase(9, a64reg::kX0, kArrOffLen / kArrSlotBytes) ||
                        !strBase(9, a64reg::kX0, kArrOffCap / kArrSlotBytes))
                        return false;
                    if (!addImm(9, a64reg::kX0, kArrHeaderBytes) ||
                        !strBase(9, a64reg::kX0, kArrOffData / kArrSlotBytes))
                        return false;
                    if (!movz(10, 0) || !strBase(10, a64reg::kX0, kArrOffTags / kArrSlotBytes) ||
                        !movz(10, kDynKindInt) || !strBase(10, a64reg::kX0, kArrOffHomog / kArrSlotBytes))
                        return false;

                    // (AR) لولبُ التعبئة: data[i] = (i64)buf[i] ممدَّدًا بالصفر ⇒ ٠..٢٥٥ لا سالبًا.
                    if (!movz(9, 0) || !strSlot(9, fileBytesSlot(4)))
                        return false;
                    const size_t rHead = code_.size();
                    if (!ldrSlot(9, fileBytesSlot(4)) || !ldrSlot(10, fileBytesSlot(1)) || !cmp(9, 10))
                        return false;
                    size_t rDone;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", rDone))
                        return false;
                    if (!ldrSlot(10, fileBytesSlot(2)) || !rrr(a64::mnem::kAdd, 10, 10, 9) ||
                        !ldrb(11, 10) ||                                          // x11 = buf[i] (٠..٢٥٥)
                        !ldrSlot(12, fileBytesSlot(5)) ||
                        !ldrBase(12, 12, kArrOffData / kArrSlotBytes) ||
                        !addLsl3(12, 12, 9) || !strBase(11, 12, 0))               // data[i] = البايت
                        return false;
                    if (!addImm(9, 9, 1) || !strSlot(9, fileBytesSlot(4)) || !emitBBack(rHead))
                        return false;
                    if (!patchBranchFwd(rDone, 23, 5))
                        return false;

                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int rdst;
                    if (!allocReg(inst.result->name, rdst))
                        return false;
                    return ldrSlot(rdst, fileBytesSlot(5));
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
                case OP::BUILTIN_ARRAY_REMOVE:
                case OP::ARRAY_REMOVE:
                {
                    // (AR) حذفٌ في المكان (نظيرُ x86): operands=[arr, index]. نطبّع السالب (idx+=len)،
                    //      نفحص الحدَّ، ثمّ نُزيح data[i]=data[i+1] لـi من removeIdx حتّى len-2، ونُنقِص الطول.
                    //      لا mmap؛ لكن الحلقةُ تستعمل الحوضَ خدشًا ⇒ انسكابٌ حولَها. النتيجة (إن وُجدت) = المصفوفة.
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) 🔑 اقرأ الفهرسَ أوّلًا في x17 (خارج الحوض) ثمّ المصفوفةَ في x9: materialize يقرأ
                    //      سجلَّ المعامل الفيزيائيّ، فلو قرأنا arr→x9 أوّلًا لدهسنا x9 قبل قراءة فهرسٍ قد يشغلُه
                    //      (فخّ ترتيبٍ كامن — نظيرُ فخّ CONCAT/ZIP). x16=len=[arr+0]. تطبيعُ السالب: إن idx<0 ⇒ idx+=len.
                    if (!materialize(a64reg::kScratch1, inst.operands[1]) ||
                        !materialize(9, inst.operands[0]) ||
                        !ldrBase(a64reg::kScratch0, 9, kArrOffLen / kArrSlotBytes))
                        return false;
                    if (!cmp(a64reg::kScratch1, a64reg::kXzr))
                        return false;
                    size_t bgeSkipNeg;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", bgeSkipNeg))
                        return false;
                    if (!rrr(a64::mnem::kAdd, a64reg::kScratch1, a64reg::kScratch1, a64reg::kScratch0))
                        return false;
                    if (!patchBranchFwd(bgeSkipNeg, 23, 5))
                        return false;
                    if (!emitBoundsCheckArm64()) // (AR) x17=idx مقابل x16=len لا-موقَّعًا
                        return false;
                    // x10 = len-1 (= الطولُ الجديد وحدُّ الحلقة)؛ x11 = data=[arr+2]. ثمّ احفظ arr في x16
                    //      (خارج الحوض ⇒ ينجو من الحلقة والاستعادة) للنتيجة — نظيرُ RAX في x86.
                    if (!subImm(10, a64reg::kScratch0, 1) || !movReg(a64reg::kScratch0, 9) ||
                        !ldrBase(11, 9, kArrOffData / kArrSlotBytes))
                        return false;
                    // x12 = i = removeIdx (x17).
                    if (!movReg(12, a64reg::kScratch1))
                        return false;
                    const size_t remHead = code_.size();
                    if (!cmp(12, 10))
                        return false;
                    size_t bgeRemDone;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", bgeRemDone))
                        return false;
                    // x15 = data[i+1] ⇒ data[i] = x15.
                    if (!addImm(13, 12, 1) || !addLsl3(14, 11, 13) || !ldrBase(15, 14, 0))
                        return false;
                    if (!addLsl3(14, 11, 12) || !strBase(15, 14, 0))
                        return false;
                    if (!addImm(12, 12, 1) || !emitBBack(remHead))
                        return false;
                    if (!patchBranchFwd(bgeRemDone, 23, 5))
                        return false;
                    // (AR) الطولُ الجديد = x10 ⇒ [arr+len] = x10.
                    if (!strBase(10, 9, kArrOffLen / kArrSlotBytes))
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
                        return movReg(dst, a64reg::kScratch0); // (AR) النتيجة = المصفوفة (x16 نجا من الحلقة والاستعادة)
                    }
                    return true;
                }
                case OP::ARRAY_CONCAT:
                {
                    // (AR) دمجُ مصفوفتين (نظيرُ x86): operands=[arr1, arr2]. كتلةٌ واحدة [رأسٌ ٤٠ | بياناتٌ
                    //      (len1+len2)×8]، نسخُ المنطقتين خامًا، len/cap/data/tags=null/homog. المسارُ الساكن
                    //      فقط (نرفض Any والمختلطَ النوعين). data=base+40 (كـARRAY_NEW).
                    if (!requireArity(inst, 2))
                        return false;
                    const types::SadTypeKind e0 = inst.operands[0].elementType;
                    const types::SadTypeKind e1 = inst.operands[1].elementType;
                    const bool bothKnown = e0 != types::SadTypeKind::Void && e1 != types::SadTypeKind::Void;
                    if (e0 == types::SadTypeKind::Any || e1 == types::SadTypeKind::Any ||
                        (bothKnown && e0 != e1))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayConcatBoxed);
                    long long homogKind = kDynKindInt;
                    if (bothKnown && e0 == e1)
                        (void)dynTagForType(e0, homogKind);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) 🔑 اقرأ كِلا المعاملين إلى الخانات مستعملًا x16/x17 (خارج الحوض) **فقط** قبل
                    //      لمسِ أيّ سجلّ حوض: materialize يقرأ سجلَّ المعامل الفيزيائيّ، فلو دهسنا سجلَّ
                    //      حوضٍ يحمله لقرأنا قيمةً خاطئة (فخّ ترتيبٍ كامن). خانات: 0=len1 1=data1 2=len2 3=data2.
                    if (!materialize(a64reg::kScratch0, inst.operands[0]) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffLen / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(0)) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffData / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(1)))
                        return false;
                    if (!materialize(a64reg::kScratch0, inst.operands[1]) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffLen / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(2)) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffData / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(3)))
                        return false;
                    // totalLen = len1+len2 ⇒ خانة 4؛ size = 40 + totalLen×8 في x1 ⇒ mmap.
                    if (!ldrSlot(9, arrExtSlot(0)) || !ldrSlot(10, arrExtSlot(2)) ||
                        !rrr(a64::mnem::kAdd, 9, 9, 10) || !strSlot(9, arrExtSlot(4)))
                        return false;
                    if (!movz(11, 3) || !rrr(a64::mnem::kLslv, a64reg::kX1, 9, 11) ||
                        !addImm(a64reg::kX1, a64reg::kX1, kArrHeaderBytes) || !emitMmapArm64PresetSize())
                        return false;
                    // (AR) x0=base. الحقول: len=cap=totalLen، data=base+40، tags=null، homog=النوع.
                    if (!ldrSlot(9, arrExtSlot(4)) || !strBase(9, a64reg::kX0, kArrOffLen / kArrSlotBytes) ||
                        !strBase(9, a64reg::kX0, kArrOffCap / kArrSlotBytes))
                        return false;
                    if (!addImm(9, a64reg::kX0, kArrHeaderBytes) || !strBase(9, a64reg::kX0, kArrOffData / kArrSlotBytes))
                        return false;
                    if (!movz(10, 0) || !strBase(10, a64reg::kX0, kArrOffTags / kArrSlotBytes))
                        return false;
                    if (!movz(10, homogKind) || !strBase(10, a64reg::kX0, kArrOffHomog / kArrSlotBytes))
                        return false;
                    // ── نسخُ المنطقة ١: dst=base+40 (x9)، src=data1، cnt=len1×8؛ x9 يتقدّم ──
                    if (!addImm(9, a64reg::kX0, kArrHeaderBytes) || !ldrSlot(10, arrExtSlot(1)) ||
                        !ldrSlot(11, arrExtSlot(0)) || !movz(12, 3) || !rrr(a64::mnem::kLslv, 11, 11, 12) ||
                        !byteCopy(9, 10, 11, 13))
                        return false;
                    // ── نسخُ المنطقة ٢: dst=x9 (يتابع)، src=data2، cnt=len2×8 ──
                    if (!ldrSlot(10, arrExtSlot(3)) || !ldrSlot(11, arrExtSlot(2)) || !movz(12, 3) ||
                        !rrr(a64::mnem::kLslv, 11, 11, 12) || !byteCopy(9, 10, 11, 13))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kX0); // (AR) x0 (base) ليس من الحوض ⇒ نجا من الاستعادة
                }
                case OP::ARRAY_ZIP:
                {
                    // (AR) زاوج (نظيرُ x86): operands=[arr1, arr2]. طولُ الناتج min(len1,len2)، كلُّ خانةٍ
                    //      مؤشّرُ مصفوفةِ زوجٍ {أ[i]، ب[i]} (خامًا). حلقةٌ ذاتُ mmap داخليٍّ لكلّ زوج ⇒ القيمُ
                    //      العابرةُ في خانات الخدش. homog=Array. خانات: 0=outLen 1=i 2=data1 3=data2 4=outArr 5=outData.
                    if (!requireArity(inst, 2))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!spillReg(kv.second))
                                return false;
                    // (AR) 🔑 اقرأ كِلا المعاملين إلى الخانات مستعملًا x16/x17 (خارج الحوض) **فقط** قبل
                    //      لمسِ أيّ سجلّ حوض (فخّ ترتيبٍ كامن أطاح ZIP على ARM64). خانات: 0=len1(مؤقّت)
                    //      2=data1؛ 1=len2(مؤقّت) 3=data2؛ ثمّ min ⇒ 0=outLen، وتُعادُ 1 لـi.
                    if (!materialize(a64reg::kScratch0, inst.operands[0]) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffLen / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(0)) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffData / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(2)))
                        return false;
                    if (!materialize(a64reg::kScratch0, inst.operands[1]) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffLen / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(1)) ||
                        !ldrBase(a64reg::kScratch1, a64reg::kScratch0, kArrOffData / kArrSlotBytes) || !strSlot(a64reg::kScratch1, arrExtSlot(3)))
                        return false;
                    // outLen = min(len1,len2) (الحوضُ حرٌّ الآن): x9=len1، x10=len2؛ إن x9<=x10 أبقِ x9 وإلّا x9=x10.
                    if (!ldrSlot(9, arrExtSlot(0)) || !ldrSlot(10, arrExtSlot(1)) || !cmp(9, 10))
                        return false;
                    size_t bleKeep;
                    if (!emitBranchFwd(a64::mnem::kBle, "rel19", bleKeep))
                        return false;
                    if (!movReg(9, 10))
                        return false;
                    if (!patchBranchFwd(bleKeep, 23, 5))
                        return false;
                    if (!strSlot(9, arrExtSlot(0))) // outLen (x9)
                        return false;
                    // خصّص كتلةَ الناتج: size = 40 + outLen×8 ⇒ x0=outArr (x9=outLen).
                    if (!movz(11, 3) || !rrr(a64::mnem::kLslv, a64reg::kX1, 9, 11) ||
                        !addImm(a64reg::kX1, a64reg::kX1, kArrHeaderBytes) || !emitMmapArm64PresetSize())
                        return false;
                    if (!strSlot(a64reg::kX0, arrExtSlot(4))) // outArr
                        return false;
                    if (!ldrSlot(9, arrExtSlot(0)) || !strBase(9, a64reg::kX0, kArrOffLen / kArrSlotBytes) ||
                        !strBase(9, a64reg::kX0, kArrOffCap / kArrSlotBytes))
                        return false;
                    if (!addImm(9, a64reg::kX0, kArrHeaderBytes) ||
                        !strBase(9, a64reg::kX0, kArrOffData / kArrSlotBytes) || !strSlot(9, arrExtSlot(5)))
                        return false;
                    if (!movz(9, 0) || !strBase(9, a64reg::kX0, kArrOffTags / kArrSlotBytes))
                        return false;
                    if (!movz(9, kDynKindArray) || !strBase(9, a64reg::kX0, kArrOffHomog / kArrSlotBytes))
                        return false;
                    if (!movz(9, 0) || !strSlot(9, arrExtSlot(1))) // i=0
                        return false;
                    // ── الحلقة: بينما i<outLen ──
                    const size_t zipHead = code_.size();
                    if (!ldrSlot(9, arrExtSlot(1)) || !ldrSlot(10, arrExtSlot(0)) || !cmp(9, 10))
                        return false;
                    size_t bgeZipDone;
                    if (!emitBranchFwd(a64::mnem::kBge, "rel19", bgeZipDone))
                        return false;
                    // خصّص زوجًا: size = 40 + 16 ⇒ x0=pairBase؛ len=cap=2، data=pairBase+40، tags=null (homog=0 من mmap).
                    if (!movz(a64reg::kX1, kArrHeaderBytes + 2 * kArrSlotBytes) || !emitMmapArm64PresetSize())
                        return false;
                    if (!movz(9, 2) || !strBase(9, a64reg::kX0, kArrOffLen / kArrSlotBytes) ||
                        !strBase(9, a64reg::kX0, kArrOffCap / kArrSlotBytes))
                        return false;
                    if (!addImm(9, a64reg::kX0, kArrHeaderBytes) || !strBase(9, a64reg::kX0, kArrOffData / kArrSlotBytes))
                        return false;
                    if (!movz(9, 0) || !strBase(9, a64reg::kX0, kArrOffTags / kArrSlotBytes))
                        return false;
                    // e1=data1[i]→x13، e2=data2[i]→x14 (x12=i).
                    if (!ldrSlot(12, arrExtSlot(1)))
                        return false;
                    if (!ldrSlot(10, arrExtSlot(2)) || !addLsl3(10, 10, 12) || !ldrBase(13, 10, 0))
                        return false;
                    if (!ldrSlot(10, arrExtSlot(3)) || !addLsl3(10, 10, 12) || !ldrBase(14, 10, 0))
                        return false;
                    // pairData = pairBase+40 ⇒ [pd+0]=e1، [pd+1]=e2.
                    if (!addImm(9, a64reg::kX0, kArrHeaderBytes) || !strBase(13, 9, 0) || !strBase(14, 9, 1))
                        return false;
                    // outData[i] = pairBase (x0).
                    if (!ldrSlot(10, arrExtSlot(5)) || !ldrSlot(12, arrExtSlot(1)) || !addLsl3(10, 10, 12) ||
                        !strBase(a64reg::kX0, 10, 0))
                        return false;
                    // i++ ثمّ عُد للرأس.
                    if (!ldrSlot(9, arrExtSlot(1)) || !addImm(9, 9, 1) || !strSlot(9, arrExtSlot(1)) ||
                        !emitBBack(zipHead))
                        return false;
                    if (!patchBranchFwd(bgeZipDone, 23, 5))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrSlot(dst, arrExtSlot(4)); // (AR) النتيجة = outArr
                }
                case OP::ENUM_CONSTRUCT:
                {
                    // (AR) بناءُ تعدادٍ جبريّ (نظيرُ x86): operands=[String(اسم), ConstI64(tag), payload…].
                    //      مؤشّرُ كومة [tag@0 | SadDyn slots]. mmap+tag+لكلّ حمولة(kind من نوعها Int + i64 قيمتها).
                    if (!requireMinArity(inst, 2))
                        return false;
                    long long tagv;
                    if (!common::isConstInt(inst.operands[1], tagv))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    const long long nPayload = static_cast<long long>(inst.operands.size()) - 2;
                    std::vector<long long> kinds(static_cast<size_t>(nPayload), kDynKindInt);
                    for (long long i = 0; i < nPayload; ++i)
                    {
                        const auto &po = inst.operands[static_cast<size_t>(2 + i)];
                        long long k;
                        // (AR) الدفعة ٨ (نظيرُ x86): نصّ ⇒ kind=Str (مؤشّرُ char* i64)؛ عشريّ ⇒ kind=Float
                        //      (بتّاتٌ i64). كلاهما i64 في خانةِ الحمولة؛ GET_PAYLOAD يعيدها والنوعُ في SIR
                        //      يقودُ الاستهلاك (fmov لـFP للعشريّ، مؤشّرٌ للنصّ).
                        if (po.dataType == types::SadTypeKind::String)
                            kinds[static_cast<size_t>(i)] = kDynKindStr;
                        else if (po.dataType != types::SadTypeKind::Any && dynTagForType(po.dataType, k))
                            kinds[static_cast<size_t>(i)] = k;
                        else if (po.dataType != types::SadTypeKind::Any &&
                                 po.dataType != types::SadTypeKind::Integer)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kEnumPayloadKind);
                    }
                    // (AR) انسكِبْ المؤقّتاتِ الحيّةَ ومعامِلاتِ الحمولة (mmap يدهس الحوض؛ نقرؤها بعده).
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!spillReg(kv.second))
                                return false;
                    const long long total = kAdtPayloadBase + nPayload * kSadDynBytes;
                    if (!emitMmapArm64(total)) // x0 = المؤشّر (خارجَ الحوض ⇒ ينجو)
                        return false;
                    if (!movz(9, tagv) || !strBase(9, a64reg::kX0, kAdtTagOff / kArrSlotBytes))
                        return false;
                    for (long long i = 0; i < nPayload; ++i)
                    {
                        const long long slotOff = kAdtPayloadBase + i * kSadDynBytes;
                        if (!movz(9, kinds[static_cast<size_t>(i)]) ||
                            !strBase(9, a64reg::kX0, (slotOff + kSadDynKindOff) / kArrSlotBytes))
                            return false;
                        const auto &po = inst.operands[static_cast<size_t>(2 + i)];
                        // (AR) نصّ ⇒ جسِّد المؤشّرَ في x9 (حرفيّةُ rodata أو مؤشّرُ كومةٍ من الانسكاب)؛
                        //      غيرُه (عشريّ/صحيح/منطقيّ) ⇒ i64 مباشرةً (loadArgInto يقرأ من الانسكاب).
                        if (po.dataType == types::SadTypeKind::String)
                        {
                            if (!materializeString(po, 9, /*fromSpill=*/true))
                                return false;
                        }
                        else if (!loadArgInto(9, po))
                            return false;
                        if (!strBase(9, a64reg::kX0, (slotOff + kSadDynPayloadOff) / kArrSlotBytes))
                            return false;
                    }
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!reloadReg(kv.second))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, a64reg::kX0);
                }
                case OP::ENUM_IS_VARIANT:
                {
                    // (AR) فحصُ حالة (نظيرُ x86): operands=[obj, ConstI64(tag), String, ConstI64(isUnit)].
                    //      isUnit=1⇒القيمةُ الوسمُ؛ 0⇒[obj+0]. النتيجةُ منطقيّة عبر cset EQ.
                    if (!requireMinArity(inst, 4))
                        return false;
                    long long tagv, isUnit;
                    if (!common::isConstInt(inst.operands[1], tagv) ||
                        !common::isConstInt(inst.operands[3], isUnit))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    // (AR) المؤشّر/الوسمُ في x17 والمتوقَّعُ في x16 (خارج الحوض) ⇒ لا يدهسان نتيجةً حوضيّةً حيّة.
                    if (!materialize(a64reg::kScratch1, inst.operands[0]))
                        return false;
                    if (isUnit == 0 && !ldrBase(a64reg::kScratch1, a64reg::kScratch1, kAdtTagOff / kArrSlotBytes))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    long long field;
                    (void)csetInvertedField(sir::SIROpcode::EQ, field); // (AR) EQ ⇒ field=1
                    return movz(a64reg::kScratch0, tagv) && cmp(a64reg::kScratch1, a64reg::kScratch0) &&
                           emit(a64::mnem::kCset, "x, cond", {a64::Operand::R(dst), a64::Operand::I(field)});
                }
                case OP::ENUM_GET_PAYLOAD:
                {
                    // (AR) استخراجُ حمولة (نظيرُ x86): operands=[obj, ConstI64(fieldIdx), String]. نحمّل
                    //      جزءَ payload الـi64 من خانةِ SadDyn (الوسمُ يُتجاهَل؛ للحمولة العدديّة/سياق i64).
                    if (!requireMinArity(inst, 2))
                        return false;
                    long long fieldIdx;
                    if (!common::isConstInt(inst.operands[1], fieldIdx) || fieldIdx < 0)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    // (AR) 🔑 المؤشّرُ في x16 (خارج الحوض): استخراجٌ متتالٍ (حقل٠ ثمّ حقل١) — لو حمّلنا
                    //      المؤشّرَ في x9 (حوض) لدهسنا نتيجةَ GET_PAYLOAD سابقةً محفوظةً في سجلّ حوضٍ
                    //      (بُرهِن بفكّ الترميز: mul(ptr,field1)). نظيرُ RAX خارج الحوض في x86.
                    if (!materialize(a64reg::kScratch0, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return ldrBase(dst, a64reg::kScratch0, (kAdtPayloadBase + fieldIdx * kSadDynBytes + kSadDynPayloadOff) / kArrSlotBytes);
                }
                default:
                    // (AR) أوپكوداتُ الواجهةِ لها تشخيصٌ خاصٌّ — قرارٌ معماريٌّ مُعلَن (ث١).
                    if (common::isUiOpcode(inst.opcode))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailUiOpcode(inst));
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
            // (AR) نظائرُ B.cond **لا-موقَّعة** (b.lo/ls/hi/hs) لفرعِ مقارنةِ طبيعي64. المساواة/عدمها
            //      لا-حسّاستان للإشارة. مرآةُ x86 jccForCmpUnsigned ويطابقُ csel-hi/lo ومقارنةَ المفسّر.
            static const std::string *bccForCmpUnsigned(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: return &a64::mnem::kBeq;
                case OP::NE: return &a64::mnem::kBne;
                case OP::LT: return &a64::mnem::kBlo;
                case OP::LE: return &a64::mnem::kBls;
                case OP::GT: return &a64::mnem::kBhi;
                case OP::GE: return &a64::mnem::kBhs;
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
                // (AR) نتيجةٌ عابرةٌ للكتل ⇒ لا تُدمَج (تُقرأ من خانتها احتياطيًّا). يطابق lowerBlock.
                if (cmpInst && cmpInst->result && crossBlockSpill_.count(cmpInst->result->name))
                    cmpInst = nullptr;
                if (!cmpInst || cmpInst->operands.size() != 2)
                {
                    // (AR) قيمةٌ منطقيّةٌ حيّةٌ (٠/١) عُرِّفت في هذه الكتلة (نتيجةُ مقارنةِ عوائم أو
                    //      متغيّرٌ منطقيّ): cmp Xcond, xzr؛ b.ne then؛ b else. (العوائم لا تُدمَج عمدًا.)
                    return materialize(a64reg::kScratch0, cond) &&
                           cmp(a64reg::kScratch0, a64reg::kXzr) &&
                           emitBranch(a64::mnem::kBne, "rel19", thenLbl) &&
                           emitBranch(a64::mnem::kB, "rel26", elseLbl);
                }
                // (AR) كلا المعامِلَين طبيعي64 ⇒ فرعٌ لا-موقَّع (b.lo/ls/hi/hs)؛ وإلّا الموقَّع مع
                //      رفضِ أيِّ لا-موقَّعٍ متبقٍّ (خلط/بايت) صراحةً كما كان (مرآةُ x86).
                const bool unsignedCmp = bothUInt64(*cmpInst);
                if (!unsignedCmp)
                    for (const auto &cop : cmpInst->operands)
                        if (isUnsignedType(cop.dataType))
                            return fail(EC::INT_NATIVE_UNSUPPORTED,
                                        diag::kCmpUnsignedType + std::to_string(static_cast<int>(cop.dataType)));
                const std::string *bcc = unsignedCmp ? bccForCmpUnsigned(cmpInst->opcode)
                                                     : bccForCmp(cmpInst->opcode);
                if (!bcc)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(*cmpInst));
                // (AR) 🐞 كان `materialize` للمعامِلَين معًا ⇒ الفرعُ يقارن **مؤشّرَي
                //      العلبة** لا القيمتين: `إذا (خليط[٠] == ٧)` كاذبًا، بل
                //      `إذا (خليط[٠] == خليط[٠])` كاذبًا (كلُّ قراءةٍ تُخصّص علبةً جديدة).
                //      مرآةُ الإصلاحِ في `prepareCompareOperands` — الشكلان (قيمةً وفرعًا)
                //      عمليّةٌ واحدةٌ فلا يجوز افتراقُهما.
                return loadScalarInto(a64reg::kScratch0, cmpInst->operands[0]) &&
                       loadScalarInto(a64reg::kScratch1, cmpInst->operands[1]) &&
                       cmp(a64reg::kScratch0, a64reg::kScratch1) &&
                       emitBranch(*bcc, "rel19", thenLbl) &&
                       emitBranch(a64::mnem::kB, "rel26", elseLbl);
            }

            // (AR) عند نهايةِ كتلةٍ سَلَفٍ (قبل قفزِها): خزّن كلَّ قيمةٍ واردةٍ لـPHI في خانةِ ناتجِه.
            //      x16 مُبدَّد. الخانةُ تحمل القيمةَ عبر الحافّة إلى الكتلةِ الدامجة (تقرؤها memSlot_-أوّلًا).
            bool emitPhiEdgeStores(const std::string &predName)
            {
                auto it = phiEdges_.find(predName);
                if (it == phiEdges_.end())
                    return true;
                for (const auto &edge : it->second)
                    if (!materialize(a64reg::kScratch0, edge.first) ||
                        !strSlot(a64reg::kScratch0, memSlot_[edge.second]))
                        return false;
                return true;
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
                // (AR) لا تُدمِج مقارنةً نتيجتُها عابرةٌ للكتل (تُنسَك؛ الفرعُ يقرؤها احتياطيًّا). سدُّ عائق أميليا ١.
                if (fused && fused->result && crossBlockSpill_.count(fused->result->name))
                    fused = nullptr;

                for (size_t idx = 0; idx < is.size(); ++idx)
                {
                    const sir::SIRInstruction &inst = is[idx];
                    // (AR) سياقُ التخصيص: يمكّن allocReg من استرجاعِ سجلٍّ ميّتٍ عند نفاد الحوض.
                    curBlock_ = &block;
                    curInstIdx_ = idx;
                    if (&inst == fused)
                        continue;
                    // (AR) قبل مُنهي هذه الكتلة: أفرِغ قيمَ PHI الواردةَ منها إلى خاناتها (إن كانت سَلَفًا).
                    if (inst.isTerminatorInst() && !emitPhiEdgeStores(block.name))
                        return false;
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
                    // (AR) نسكُ قيمةٍ عابرةٍ للكتل عند تعريفها: خزّن ناتجَها في خانتِه (strSlot).
                    if (inst.result && crossBlockSpill_.count(inst.result->name))
                    {
                        auto rit = regOf_.find(inst.result->name);
                        if (rit != regOf_.end() && !strSlot(rit->second, memSlot_[inst.result->name]))
                            return false;
                    }
                }
                return true;
            }

            // (AR) يخفّض دالّةً واحدة: إطارٌ خاصّ، مقدّمةٌ تُسكِن سجلّاتِ ABI في خانات المعاملات، ثمّ كتلُها.
            // (AR) عددُ نداءاتِ `__sad_map_set_typed` في دالّةٍ — مرآةُ نظيرِه في x86.
            static long long countMapSets(const sir::SIRFunction &fn)
            {
                long long n = 0;
                for (const auto &block : fn.getBasicBlocks())
                    for (const auto &inst : block->instructions)
                        if (inst.opcode == sir::SIROpcode::CALL && !inst.operands.empty() &&
                            (inst.operands[0].name == Sad::Compiler::kRuntimeMapSetTyped ||
                             inst.operands[0].name == Sad::Compiler::kRuntimeMapSetTypedMethod))
                            ++n;
                return n;
            }

            bool lowerFunction(const sir::SIRFunction &fn)
            {
                mapSetCount_ = countMapSets(fn);
                currentFn_ = fn.getName();
                curIsEntry_ = (currentFn_ == entryName_);
                memSlot_.clear();
                // (AR) استنتاجُ صنفِ الكائن يُصفَّر لكلّ دالّة؛ %self داخل «صنف.طريقة» صنفُه الجزءُ قبل النقطة.
                objClassOf_.clear();
                {
                    const auto dot = currentFn_.find('.');
                    if (dot != std::string::npos)
                        objClassOf_[std::string(::Sad::Compiler::kSelfRegisterName)] = currentFn_.substr(0, dot);
                }
                // (AR) ابذرْ صنفَ كلِّ معاملٍ مصرَّحٍ بصنفٍ مسجَّل (للإرسالِ الافتراضيّ على معامل). (الدفعة ٦)
                for (const auto &p : fn.getParameters())
                    if (!p.className.empty())
                        objClassOf_[diag::kVregSigil + p.name] = p.className;
                phiEdges_.clear();
                crossBlockSpill_.clear();
                regOf_.clear();
                strReg_.clear();
                next_ = 0;
                frameSize_ = 0;
                printBufTopSlot_ = -1;
                atsFtoaTopSlot_ = -1;
                spillBaseSlot_ = -1;
                floatValSlot_ = -1;
                floorDivDividendSlot_ = -1;
                dynBaseSlot_ = -1;
                dynGetCount_ = 0;
                dynSlotNext_ = 0;
                appendPSlot_ = appendValSlot_ = appendLenSlot_ = appendNewSlot_ = appendCapSlot_ = -1;
                strHeapBaseSlot_ = -1;

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
