// ============================================================================
// (AR) جسر SIR→x86-64 أصليّ — أوّل ترجمة «لغة ص → شيفرة آلة» حقيقيّة بلا LLVM.
//      يستهلك SIR الذي يبنيه الأمامُ الفعليّ من مصدر ص (في الوضع الحرّ، حيث تُسقَط
//      سِقالةُ defer/الاستثناء فتبقى شيفرةٌ خطّيّة نظيفة)، ويخفّض مجموعةً دنيا من
//      الأوپكودات (MOVE / ADD_I64 / SUB_I64 / المقارنات / BR / BR_COND / RET) إلى
//      بايتات x86-64 عبر المحرّك الجدوليّ (lookupEncSpec + encodeVariable)، ثمّ
//      يلفّها كاتبُ ELF في تنفيذيٍّ ساكن يخرج بقيمة إرجاع `رئيسية`. لا clang/lld/as.
//
//      تدفّقُ التحكّم بمرورين: (١) يُصدر بايتاتِ كلّ كتلةٍ بالترتيب ويسجّل إزاحةَ
//      لصيقتها؛ يُصدر القفزاتِ بإزاحةٍ صفريّةٍ نائبة ويسجّل ترقيعًا. (٢) يرقّع كلَّ
//      rel32 بالفرق (هدف − نهايةُ القفز). المقارنةُ المُغذِّيةُ لـBR_COND تُدمَج
//      (cmp؛ jCC ثمّ؛ jmp وإلّا) فلا حاجةَ لـsetcc/movzx. تخصيصُ السجلّات دنيا عمدًا
//      («كلٌّ في سجلّ» بلا انسكاب، ≤٧ سجلّات حيّة، بلا PHI/ذاكرة). كلُّ بنيةٍ غير
//      مدعومةٍ (PHI/ذاكرة/مقارنةٌ غيرُ مدموجة) تُفشِل التخفيضَ صراحةً لا تُترجَم خطأً.
// (EN) SIR→native x86-64 bridge — the first real "S-lang → machine code" lowering
//      without LLVM. Consumes SIR built by the actual frontend from ص source (in
//      freestanding mode), lowers a minimal opcode set (MOVE/ADD_I64/SUB_I64/
//      comparisons/BR/BR_COND/RET) to x86-64 bytes via the table-driven encoder,
//      then wraps them in a static ELF that exits with `رئيسية`'s return value.
//
//      Control flow via two passes: (1) emit each block's bytes in order, record
//      its label offset, emit jumps with a placeholder rel32 and record a fixup;
//      (2) patch each rel32 with (target − end-of-jump). The comparison feeding a
//      BR_COND is fused (cmp; jCC then; jmp else) — no setcc/movzx needed. Register
//      allocation is minimal on purpose (everything-in-registers, no spilling, ≤7
//      live regs, no PHI/memory). Unsupported constructs (PHI/memory/an unfused
//      comparison) fail loudly rather than miscompile silently.
// ============================================================================
#ifndef SAD_NATIVE_SIR_LOWERING_H
#define SAD_NATIVE_SIR_LOWERING_H

#include "backend/native/x86_variable_encoder.h"
#include "backend/native/generated/x86_64_encoding_generated.h"
#include "backend/native/elf64_writer.h"

#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

// (AR) كتالوج الأخطاء المولَّد من مصدر الحقيقة (language-truth/errors/*.yaml):
//      المخفّضُ يحمل ErrorCode وبياناتِ {detail} فقط، والرسالةُ تُشتقّ من الكتالوج —
//      لا نصَّ رسالةٍ مباشرًا في هذا الكود (قاعدة SoT: كلّها من YAML).
#include "error_codes.h"
#include "error_messages_generated.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace sad
{
    namespace native
    {
        // (AR) أسماءٌ مختصرة لأنواع SIR (فضاء الاسم Sad::Compiler::SIR) وأنواع البيانات.
        namespace sir = ::Sad::Compiler::SIR;
        namespace types = ::Sad::Types;

        // (AR) رقمُ نداء exit في SysV/x86-64 (من abi/x86_64-linux.yaml). ثابتٌ مسمّى لا رقمٌ سحريّ.
        inline constexpr long long kSysExitX86 = 60;

        // (AR) مخرَجُ التخفيض: بايتاتُ الشيفرة عند النجاح، أو رمزُ خطأٍ (ErrorCode من
        //      كتالوج SoT) + بياناتُ {detail} عند الفشل. الرسالةُ النصّيّة تُشتقّ من
        //      الكتالوج عبر message() — لا تُخزَّن كنصٍّ مباشر.
        struct LoweringResult
        {
            bool ok = false;
            ::Sad::Errors::ErrorCode errorCode = ::Sad::Errors::ErrorCode::INT_NATIVE_NO_ENTRY;
            std::string detail;        // (AR) بياناتُ {detail} فقط (لا نثر)
            std::vector<uint8_t> code; // (AR) شيفرة .text ثمّ ELF (عند النجاح)

            // (AR) الرسالةُ المختصرة من كتالوج SoT (briefAr) مع إحلال {detail} — لا نصَّ مباشرًا.
            // (EN) The brief message from the SoT catalog with {detail} substituted.
            std::string message() const
            {
                const auto *e = ::Sad::Errors::Generated::findByCode(errorCode);
                if (!e || !e->briefAr)
                    return {};
                std::string m = e->briefAr;
                static const std::string kPh = "{detail}";
                auto pos = m.find(kPh);
                if (pos != std::string::npos)
                    m.replace(pos, kPh.size(), detail);
                return m;
            }
        };

        // (AR) مخفّضٌ لوحدةٍ متعدّدة الدوالّ إلى x86-64: كلُّ دالّةٍ بإطارها الخاصّ (الداخلةُ
        //      أوّلًا = نقطةُ دخول ELF). تفرّعٌ (مرور طبقتين + ترقيع rel32، لصائقُ مؤهَّلةٌ
        //      بالدالّة) + متغيّراتٌ محلّيّةٌ ومعاملاتٌ في الذاكرة (إطارُ rbp + خانات
        //      [rbp−إزاحة]؛ المعاملُ يُسكَن من سجلّ وسيطِ SysV الوارد؛ قراءةُ متغيّرِ الذاكرة
        //      كقيمة = تحميلٌ ضمنيّ) + استدعاءٌ (call/ret، وسائطُ rdi/rsi/… وإرجاعُ rax).
        //      نموذجُ التخصيص: كلُّ سجلٍّ افتراضيّ (بالاسم) ⇒ سجلٌّ فيزيائيّ من حوضٍ لا يشمل
        //      rax (مُبدَّد + رقم النداء) وla rdi (وسيط أوّل) ولا rsp/rbp؛ الحوضُ يُنظَّف لكلّ
        //      كتلة (ما يعبر الكتلَ فعبر الذاكرة). النداءُ يدهس كلَّ الحوض (caller-saved) ⇒
        //      تُنسَك المؤقّتاتُ الحيّة إلى خانات إطارٍ قبله وتُعاد بعده، وتُحمَّل وسائطُ
        //      المؤقّتات من خانات الانسكاب (صفر تصادمٍ = نقلٌ متوازٍ). PHI ⇒ فشلٌ صريح.
        class X86SirLowering
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

                // (AR) المرور ٢: رقّع كلَّ rel32 (فرعٌ داخل الدالّة، أو نداءٌ بين الدوالّ).
                if (!applyFixups())
                    return finishError(r, errorCode_, detail_);

                r.ok = true;
                r.code = std::move(code_);
                return r;
            }

            // (AR) يخفّض دالّةً واحدة: إطارٌ خاصّ (يُخصَّص لمعاملاتها ومحلّيّاتها)، مقدّمةٌ
            //      تُسكِن سجلّاتِ ABI الواردة في خانات المعاملات، ثمّ كتلُها بالترتيب.
            bool lowerFunction(const sir::SIRFunction &fn)
            {
                currentFn_ = fn.getName();
                curIsEntry_ = (currentFn_ == entryName_);
                memSlot_.clear();
                regOf_.clear();
                next_ = 0;
                frameSize_ = 0;
                spillBase_ = 0; // (AR) تأمينٌ دفاعيّ: لا يُستعمَل إلّا حين hasCall (يُضبَط في assignFrameSlots)

                const auto &blocks = fn.getBasicBlocks();
                if (blocks.empty())
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailBlocks());

                // (AR) خانات: المعاملاتُ أوّلًا (بترتيب ABI)، ثمّ ALLOCات المحلّيّات.
                if (!assignFrameSlots(fn, blocks))
                    return false;

                // (AR) المقدّمة (إن وُجدت خانات): push rbp؛ mov rbp,rsp؛ sub rsp,N؛ ثمّ
                //      خزّن سجلّاتِ الوسائط الواردة (rdi/rsi/…) في خانات المعاملات.
                if (frameSize_ > 0 && !emitPrologue(fn))
                    return false;

                for (const auto &blockPtr : blocks)
                {
                    const sir::SIRBasicBlock &block = *blockPtr;
                    labelOffset_[qualify(block.name)] = code_.size();
                    if (!lowerBlock(block))
                        return false;
                }
                return true;
            }

        private:
            using EC = ::Sad::Errors::ErrorCode;

            std::map<std::string, int> regOf_;                                    // (AR) اسم السجلّ الافتراضيّ ⇒ فيزيائيّ
            const std::vector<int> pool_{x86::RDX, x86::RCX, x86::RSI, x86::R8,    // (AR) حوضُ التخصيص
                                         x86::R9, x86::R10, x86::R11};
            size_t next_ = 0;
            std::vector<uint8_t> code_;
            EC errorCode_ = EC::INT_NATIVE_NO_ENTRY;
            std::string detail_;

            // (AR) تدفّق التحكّم: خريطةُ لصيقة الكتلة (مؤهَّلةٌ بالدالّة) ⇒ إزاحتها، وطابورُ
            //      ترقيع rel32. الفرعُ يستهدف لصيقةً داخل الدالّة؛ النداءُ يستهدف دالّةً.
            std::map<std::string, size_t> labelOffset_;
            std::map<std::string, size_t> funcOffset_; // (AR) اسمُ الدالّة ⇒ إزاحةُ شيفرتها (للنداءات)
            struct Fixup
            {
                size_t rel32Pos;    // (AR) موضعُ بايتات الإزاحة النسبيّة في code_
                std::string target; // (AR) هدفٌ: لصيقةٌ مؤهَّلة (فرع) أو اسمُ دالّة (نداء)
                int width;          // (AR) عرضُ حقل الإزاحة بالبايت (من مواصفة الترميز، لا ثابتًا)
                bool isCall;        // (AR) نداءٌ (يُحلُّ من funcOffset_) أم فرعٌ (من labelOffset_)
            };
            std::vector<Fixup> fixups_;

            // (AR) الدوالّ: اسمُ الدالّة الداخلة، الدالّةُ الجاري تخفيضُها (لتأهيل اللصائق)،
            //      وهل هي الداخلة (RET ⇒ exit) أم لا (RET ⇒ خاتمة + ret).
            std::string entryName_;
            std::string currentFn_;
            bool curIsEntry_ = false;

            // (AR) سجلّاتُ وسائط SysV/x86-64 بالترتيب (الوسائطُ الستّة الأولى الصحيحة).
            const int abiArg_[6]{x86::RDI, x86::RSI, x86::RDX, x86::RCX, x86::R8, x86::R9};

            // (AR) الذاكرة: اسمُ الخانة (معامل أو ALLOC) ⇒ إزاحتُه عن rbp (سالبةٌ)، وحجمُ
            //      الإطار المُحاذى ١٦. تُملأ في المسح المسبق لكلّ دالّة.
            std::map<std::string, long long> memSlot_;
            long long frameSize_ = 0;

            // (AR) الانسكابُ عبر النداء: إزاحةُ الخانة الأولى لمنطقة انسكابِ سجلّات الحوض
            //      (خانةٌ لكلّ سجلٍّ من pool_). النداءُ يدهس كلَّ الحوض (caller-saved)، فتُنسَك
            //      المؤقّتاتُ الحيّةُ إليها قبله وتُعاد بعده؛ وتُحمَّل وسائطُ المؤقّتات منها
            //      (لا من السجلّات) ⇒ صفر تصادمٍ في نقل الوسائط (حلٌّ موحَّد للنقل المتوازي).
            long long spillBase_ = 0;

            // (AR) إزاحةُ خانة انسكابِ سجلّ الحوض بموضعه i (0..pool_.size()-1).
            long long spillDisp(size_t poolIdx) const
            {
                return spillBase_ - static_cast<long long>(poolIdx) * 8;
            }

            // (AR) موضعُ سجلٍّ فيزيائيّ في حوض التخصيص (لفهرسة خانة انسكابه). ‎-1‎ إن ليس منه.
            int poolIndexOf(int physReg) const
            {
                for (size_t i = 0; i < pool_.size(); ++i)
                    if (pool_[i] == physReg)
                        return static_cast<int>(i);
                return -1;
            }

            // (AR) يؤهّل لصيقةَ كتلةٍ باسم الدالّة الحاليّة (يمنع تصادمَ «entry» بين الدوالّ).
            std::string qualify(const std::string &blockName) const
            {
                return currentFn_ + "\x1f" + blockName; // (AR) فاصلُ وحدةٍ لا يظهر في الأسماء
            }

            // (AR) رموزُ {detail} كبياناتٍ محضة (لا نثر): وسمُ الحالة قصيرٌ يُميّز فرعَ الفشل.
            //      النثرُ كلُّه في كتالوج SoT؛ هذه القيمُ تملأ {detail} حصرًا.
            static std::string detailBlocks() { return "blocks=0"; }
            static std::string detailNoRet() { return "no-ret-terminator"; }
            static std::string detailOpcode(const sir::SIRInstruction &i)
            {
                return "opcode=" + std::to_string(static_cast<int>(i.opcode));
            }

            LoweringResult &finishError(LoweringResult &r, EC code, const std::string &detail = "")
            {
                r.ok = false;
                r.errorCode = code;
                r.detail = detail;
                return r;
            }

            const sir::SIRFunction *findEntry(const sir::SIRModule &m) const
            {
                // (AR) رئيسية (بايتات UTF-8) ثمّ بدائل، ثمّ أوّل دالّة.
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

            // (AR) يسجّل رمزَ الخطأ (من كتالوج SoT) وبياناتِ {detail}؛ لا نصَّ رسالةٍ هنا.
            bool fail(EC code, const std::string &detail = "")
            {
                errorCode_ = code;
                detail_ = detail;
                return false;
            }

            // (AR) يخصّص سجلًّا فيزيائيًّا لسجلٍّ افتراضيّ. عند نفاد الحوض **يفشل بوضوح**
            //      لا يلتفّ (التفافٌ صامتٌ يدهس سجلًّا حيًّا ⇒ إفساد — عيب أميليا BLOCKER-1).
            bool allocReg(const std::string &vreg, int &out)
            {
                // (AR) اسمٌ يطابق خانةَ ALLOC لا يجوز أن يُخصَّص سجلَّ حوض: يقرؤه isMemVar من
                //      الذاكرة بينما يخصّصه هذا سجلًّا ⇒ افتراقٌ صامتٌ لنصفَي الاسم. فشلٌ صريح.
                if (memSlot_.find(vreg) != memSlot_.end())
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "vreg-aliases-slot:%" + vreg);
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

            // ── مُصدِرات التعليمات (كلٌّ يقرأ مواصفتَه من الجدول المولَّد من SoT) ──
            //      كلٌّ يُرجع bool: الفشلُ (مواصفةٌ مفقودة/ثابتٌ خارج المدى) يصعد فيُوقف
            //      التخفيضَ بدل إنتاج ELF مبتورٍ بعلامة نجاح (عيب أميليا رقم ٤).
            bool emit(const std::string &mnemonic, const std::string &form,
                      const std::vector<x86::Operand> &ops)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, form);
                if (!spec)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + " " + form);
                auto bytes = x86::encodeVariable(*spec, ops);
                code_.insert(code_.end(), bytes.begin(), bytes.end());
                return true;
            }

            static const char *kMov;  // انقل
            static const char *kAdd;  // اجمع
            static const char *kSub;  // اطرح
            static const char *kSys;  // نداء_نظام
            static const char *kPush; // ادفع
            static const char *kPop;  // اسحب
            static const char *kRet;  // ارجع
            static const char *kCall; // نادِ
            static const char *kCmp;  // قارن
            static const char *kJmp;  // اقفز
            static const char *kJe;   // اقفز_إذا_ساوى
            static const char *kJne;  // اقفز_إذا_لم_يساوِ
            static const char *kJl;   // اقفز_إذا_أصغر
            static const char *kJle;  // اقفز_إذا_أصغر_أو_ساوى
            static const char *kJg;   // اقفز_إذا_أكبر
            static const char *kJge;  // اقفز_إذا_أكبر_أو_ساوى

            // (AR) mov r32, imm32 يمتدّ صفريًّا إلى ٦٤؛ لا يمثّل إلّا [0, 2³²). خارجَه
            //      (سالبٌ أو ≥2³²) يُبتَر ⇒ نرفضه بوضوح (عيب أميليا رقم ٣).
            bool movImm(int reg, long long imm)
            {
                if (imm < 0 || imm > 0xFFFFFFFFLL)
                    return fail(EC::INT_NATIVE_IMM_RANGE, "u32:" + std::to_string(imm));
                return emit(kMov, "r32, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }
            bool movReg(int dst, int src)
            {
                if (dst == src)
                    return true;
                return emit(kMov, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            // (AR) add/sub r64, imm32 يمتدّ إشاريًّا؛ نقصره على مدى imm32 الموقَّع.
            bool checkImm32(long long imm)
            {
                if (imm < -2147483648LL || imm > 2147483647LL)
                    return fail(EC::INT_NATIVE_IMM_RANGE, "s32:" + std::to_string(imm));
                return true;
            }
            bool addImm(int reg, long long imm)
            {
                return checkImm32(imm) && emit(kAdd, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }
            bool addReg(int dst, int src)
            {
                return emit(kAdd, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            bool subImm(int reg, long long imm)
            {
                return checkImm32(imm) && emit(kSub, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }

            // ── الذاكرة: خانات الإطار [rbp+إزاحة] ──
            // (AR) mov r64, [rbp+disp] — تحميلٌ من خانة إطار.
            bool loadMem(int dst, long long disp)
            {
                return emit(kMov, "r64, m64", {x86::Operand::R(dst), x86::Operand::M(x86::RBP, disp)});
            }
            // (AR) mov [rbp+disp], r64 — تخزينٌ في خانة إطار.
            bool storeMem(long long disp, int src)
            {
                return emit(kMov, "m64, r64", {x86::Operand::M(x86::RBP, disp), x86::Operand::R(src)});
            }

            // (AR) هل المعاملُ سجلٌّ يُشير إلى خانةِ متغيّرٍ مخصَّص (ALLOC)؟ يُعيد إزاحتَه.
            bool isMemVar(const sir::SIROperand &op, long long &disp) const
            {
                if (op.type != sir::SIROperandType::REGISTER)
                    return false;
                auto it = memSlot_.find(op.name);
                if (it == memSlot_.end())
                    return false;
                disp = it->second;
                return true;
            }

            // (AR) يحمّل وسيطَ نداءٍ في سجلّ SysV. يُستدعى بعد انسكابِ كلّ المؤقّتات، فالمؤقّتُ
            //      يُحمَّل من **خانة انسكابه** لا من سجلّه (قد يدهسه تحميلُ وسيطٍ سابقٍ لسجلٍّ
            //      مشترك) ⇒ لا تصادمَ نقلٍ متوازٍ. الثابتُ فوريٌّ، ومتغيّرُ الذاكرة من خانته.
            bool loadArgInto(int dst, const sir::SIROperand &op)
            {
                long long c;
                if (isConstInt(op, c))
                    return movImm(dst, c);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    long long disp;
                    if (isMemVar(op, disp))
                        return loadMem(dst, disp);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + op.name);
                    return loadMem(dst, spillDisp(static_cast<size_t>(poolIndexOf(it->second))));
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "arg-kind=" + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) يحمّل معاملًا (ثابتًا/سجلًّا فيزيائيًّا/متغيّرَ ذاكرة) في سجلٍّ وجهة. قراءةُ
            //      متغيّرِ الذاكرة = تحميلٌ من خانته (يُطابق auto-load في خلفيّة LLVM).
            bool loadInto(int dst, const sir::SIROperand &op)
            {
                if (op.type == sir::SIROperandType::CONSTANT)
                {
                    if (op.dataType != types::SadTypeKind::Integer)
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "const-type=" + std::to_string(static_cast<int>(op.dataType)));
                    return movImm(dst, op.intValue);
                }
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    long long disp;
                    if (isMemVar(op, disp))
                        return loadMem(dst, disp);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + op.name);
                    return movReg(dst, it->second);
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "operand-kind=" + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) المسحُ المسبق: يخصّص خانةَ إطارٍ للمعاملات (بترتيب ABI) ثمّ لكلّ ALLOC،
            //      ويحسب حجمَ الإطار المُحاذى ١٦. المعاملُ يُعامَل كمتغيّرِ ذاكرةٍ (قراءتُه =
            //      تحميلٌ من خانته)، وتُسكَنُ خانتُه من سجلّ الوسيط الوارد في المقدّمة.
            bool assignFrameSlots(const sir::SIRFunction &fn,
                                  const std::vector<std::shared_ptr<sir::SIRBasicBlock>> &blocks)
            {
                const auto &params = fn.getParameters();
                if (params.size() > 6)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "params>6:" + std::to_string(params.size()));
                long long used = 0;
                for (const auto &p : params)
                {
                    used += 8;
                    // (AR) المعاملُ يُشار إليه في التعابير بـ«%»+الاسم (sir_builder_functions.cpp:495)
                    //      بينما اسمُ SIRParameter بلا «%» ⇒ نُفهرِس بالمرجع كي يطابقَه isMemVar.
                    memSlot_["%" + p.name] = -used;
                }
                bool hasCall = false;
                for (const auto &blockPtr : blocks)
                    for (const auto &inst : blockPtr->instructions)
                    {
                        if (inst.opcode == sir::SIROpcode::ALLOC && inst.result)
                        {
                            used += 8;
                            memSlot_[inst.result->name] = -used; // [rbp−8]، [rbp−16]، …
                        }
                        else if (inst.opcode == sir::SIROpcode::CALL)
                            hasCall = true;
                    }
                // (AR) إن كانت الدالّةُ تُنادي، احجز منطقةَ انسكابٍ: خانةٌ لكلّ سجلّ حوض تُحفَظ
                //      فيها المؤقّتاتُ الحيّةُ عبر النداء (وتُحمَّل منها وسائطُ المؤقّتات).
                if (hasCall)
                {
                    spillBase_ = -(used + 8);
                    used += static_cast<long long>(pool_.size()) * 8;
                }
                long long aligned = (used + 15) / 16 * 16; // (AR) محاذاةٌ ١٦
                // (AR) عقدُ SysV: rsp مُحاذًى ١٦ قبل call. الدالّةُ غيرُ الداخلة تدخل عند
                //      rsp%16==8 (النداءُ دفع عنوانَ العودة) فـpush rbp يُعيد المحاذاةَ لصفر.
                //      أمّا الداخلةُ فتدخل عند rsp%16==0 (النواة)، فـpush rbp يتركها ٨؛ إن كانت
                //      تُنادي وذاتَ إطارٍ فتلزمها ٨ إضافيّةٌ لتعيد المحاذاةَ قبل نداءاتها.
                frameSize_ = (curIsEntry_ && hasCall && used > 0) ? aligned + 8 : aligned;
                return true;
            }

            // (AR) المقدّمة: push rbp؛ mov rbp,rsp؛ sub rsp,N؛ ثمّ خزّن سجلّاتِ الوسائط
            //      الواردة (rdi/rsi/…) في خانات المعاملات (كلٌّ مفحوص).
            bool emitPrologue(const sir::SIRFunction &fn)
            {
                if (!emit(kPush, "r64", {x86::Operand::R(x86::RBP)}) ||
                    !movReg(x86::RBP, x86::RSP) ||
                    !subImm(x86::RSP, frameSize_))
                    return false;
                const auto &params = fn.getParameters();
                for (size_t i = 0; i < params.size(); ++i)
                    if (!storeMem(memSlot_["%" + params[i].name], abiArg_[i]))
                        return false;
                return true;
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
                    return loadInto(dst, inst.operands[0]);
                }
                case OP::ADD_I64:
                case OP::SUB_I64:
                {
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (!loadInto(dst, inst.operands[0])) // (AR) المعامل الأوّل في الوجهة (يحمّل من الخانة إن متغيّرَ ذاكرة)
                        return false;
                    const sir::SIROperand &b = inst.operands[1];
                    long long bc;
                    if (isConstInt(b, bc)) // (AR) فوريّ ⇒ add/sub r64,imm32
                        return inst.opcode == OP::ADD_I64 ? addImm(dst, bc) : subImm(dst, bc);
                    // (AR) معاملٌ ثانٍ سجليّ/ذاكرة: حمّله في المُبدَّد RAX ثمّ اجمع بالسجلّ.
                    //      SUB سجلّ-سجلّ غير مدعومٍ بعد (لا صيغةَ 29 /r) ⇒ فشلٌ صريح.
                    if (inst.opcode == OP::SUB_I64)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "sub-reg-reg:" + detailOpcode(inst));
                    return loadInto(x86::RAX, b) && addReg(dst, x86::RAX);
                }
                case OP::ALLOC:
                {
                    // (AR) الخانةُ خُصِّصت في المسح المسبق؛ لا شيفرةَ تُصدَر (العنوان ضمنيٌّ [rbp−إزاحة]).
                    if (!inst.result || memSlot_.find(inst.result->name) == memSlot_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "alloc-unslotted:" + detailOpcode(inst));
                    return true;
                }
                case OP::LOAD:
                {
                    // (AR) %dst = load %slot ⇒ mov dst, [rbp+إزاحة].
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long disp;
                    if (!isMemVar(inst.operands[0], disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "load-nonslot:" + detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMem(dst, disp);
                }
                case OP::STORE:
                {
                    // (AR) store value, %slot ⇒ حمّل القيمةَ في RAX ثمّ mov [rbp+إزاحة], RAX.
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long disp;
                    if (!isMemVar(inst.operands[1], disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "store-nonslot:" + detailOpcode(inst));
                    return loadInto(x86::RAX, inst.operands[0]) && storeMem(disp, x86::RAX);
                }
                case OP::RET:
                {
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) الدالّةُ الداخلة تُنهي البرنامجَ بـexit(rdi)؛ غيرُها تُعيد القيمةَ في
                    //      rax (عقدُ SysV) ثمّ خاتمةٌ (استعادةُ الإطار) و ret. تُحمَّل من الخانة
                    //      إن متغيّرَ ذاكرة. كلٌّ مفحوص.
                    if (curIsEntry_)
                    {
                        if (!loadInto(x86::RDI, inst.operands[0]))
                            return false;
                        if (!movImm(x86::RAX, kSysExitX86))
                            return false;
                        return emit(kSys, "", {});
                    }
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    return emitEpilogue();
                }
                case OP::CALL:
                {
                    // (AR) call @دالّة, وسائط… ⇒ ضع الوسائطَ في سجلّات SysV (rdi/rsi/…) ثمّ
                    //      نادِ (rel32 يُرقَّع لإزاحة الدالّة)؛ النتيجةُ في rax ⇒ سجلُّ النتيجة.
                    if (inst.operands.empty() ||
                        inst.operands[0].type != sir::SIROperandType::FUNCTION)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const size_t argc = inst.operands.size() - 1;
                    if (argc > 6)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "args>6:" + std::to_string(argc));
                    // (AR) النداءُ يدهس كلَّ سجلّات الحوض (caller-saved في SysV). لحفظِ المؤقّتات
                    //      الحيّة عبره: (١) انسكِبْ كلَّ مؤقّتٍ مخصَّصٍ إلى خانة انسكابه؛ (٢) حمّل
                    //      الوسائطَ في سجلّات SysV — المؤقّتُ من خانة انسكابه لا من سجلّه ⇒ صفر
                    //      تصادمٍ (حلُّ النقل المتوازي)؛ (٣) نادِ؛ (٤) أعِد تحميلَ المؤقّتات؛
                    //      (٥) النتيجةُ في rax ⇒ سجلّ النتيجة. الذاكرة (memSlot_) تنجو أصلًا.
                    for (const auto &kv : regOf_)
                        if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                            return false;
                    for (size_t i = 0; i < argc; ++i)
                        if (!loadArgInto(abiArg_[i], inst.operands[i + 1]))
                            return false;
                    if (!emitCall(inst.operands[0].name))
                        return false;
                    for (const auto &kv : regOf_)
                        if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                            return false;
                    if (inst.result)
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movReg(dst, x86::RAX); // (AR) قيمةُ الإرجاع من rax
                    }
                    return true;
                }
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // (AR) خاتمةُ الدالّة غير الداخلة: إن كان لها إطارٌ استعِده (mov rsp,rbp؛ pop rbp)،
            //      ثمّ ret. بلا إطارٍ ⇒ ret مباشرةً (لم تُلمَس rbp/rsp).
            bool emitEpilogue()
            {
                if (frameSize_ > 0)
                {
                    if (!movReg(x86::RSP, x86::RBP) ||
                        !emit(kPop, "r64", {x86::Operand::R(x86::RBP)}))
                        return false;
                }
                return emit(kRet, "", {});
            }

            // (AR) يُصدر نداءً (call rel32) بإزاحةٍ نائبة، ويسجّل ترقيعًا لإزاحة الدالّة.
            bool emitCall(const std::string &funcName)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(kCall, "rel32");
                if (!spec || spec->imm_op < 0 || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, std::string(kCall) + " rel32");
                const int width = spec->imm_bits / 8;
                if (!emit(kCall, "rel32", {x86::Operand::I(0, spec->imm_bits)}))
                    return false;
                fixups_.push_back({code_.size() - static_cast<size_t>(width), funcName, width, true});
                return true;
            }

            // (AR) هل المعاملُ ثابتٌ صحيح؟ يُعيد قيمتَه (يُطابق شرطَ resolve للثوابت).
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

            // ── تدفّق التحكّم: تخفيضُ كتلةٍ كاملة + الفروع + الترقيع ──

            // (AR) هل الأوپكود مقارنةٌ عدديّةٌ موقَّعة؟ (تُدمَج في BR_COND التالي.)
            static bool isComparison(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                return op == OP::EQ || op == OP::NE || op == OP::LT ||
                       op == OP::LE || op == OP::GT || op == OP::GE;
            }

            // (AR) منمنمةُ القفز الشرطيّ المطابقة للمقارنة (موقَّعة): «إن صحّ الشرط اقفز لـthen».
            static const char *jccForCmp(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: return kJe;
                case OP::NE: return kJne;
                case OP::LT: return kJl;
                case OP::LE: return kJle;
                case OP::GT: return kJg;
                case OP::GE: return kJge;
                default: return nullptr;
                }
            }

            // (AR) يُصدر قفزًا (مشروطًا أو غير مشروط) بإزاحةٍ نسبيّةٍ صفريّةٍ نائبة، ويسجّل
            //      ترقيعًا عند حقل الإزاحة. عرضُ الحقل وموضعُه يُشتقّان من مواصفة الترميز
            //      (imm_bits) لا من ثابتٍ مُرمَّز ⇒ يبقى سليمًا لو أُضيفت صيغةُ قفزٍ بعرضٍ آخر.
            bool emitJump(const char *mnemonic, const std::string &targetLabel)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, "rel32");
                if (!spec || spec->imm_op < 0 || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, std::string(mnemonic) + " rel32");
                const int width = spec->imm_bits / 8;
                if (!emit(mnemonic, "rel32", {x86::Operand::I(0, spec->imm_bits)}))
                    return false;
                // (AR) الهدفُ لصيقةٌ مؤهَّلةٌ بالدالّة الحاليّة (فرعٌ داخليّ، لا نداء).
                fixups_.push_back({code_.size() - static_cast<size_t>(width), qualify(targetLabel), width, false});
                return true;
            }

            // (AR) يقارن المُبدَّد RAX بمعاملٍ ثانٍ: ثابتٌ (يختار imm8/imm32) أو سجلٌّ مؤقّت.
            //      متغيّرُ ذاكرةٍ كطرفٍ ثانٍ غيرُ مدعومٍ (كلا الطرفَين في الذاكرة) ⇒ فشلٌ صريح.
            bool cmpAgainst(const sir::SIROperand &b)
            {
                long long c;
                if (isConstInt(b, c))
                {
                    if (c >= -128 && c <= 127)
                        return emit(kCmp, "r64, imm8", {x86::Operand::R(x86::RAX), x86::Operand::I(c, 8)});
                    if (!checkImm32(c))
                        return false;
                    return emit(kCmp, "r64, imm32", {x86::Operand::R(x86::RAX), x86::Operand::I(c, 32)});
                }
                if (b.type == sir::SIROperandType::REGISTER)
                {
                    long long disp;
                    if (isMemVar(b, disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "cmp-b-memvar:%" + b.name);
                    auto it = regOf_.find(b.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + b.name);
                    return emit(kCmp, "r64, r64", {x86::Operand::R(x86::RAX), x86::Operand::R(it->second)});
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "cmp-b-kind=" + std::to_string(static_cast<int>(b.type)));
            }

            // (AR) القفزُ غير المشروط BR: operands[0] لصيقةُ الهدف.
            bool lowerBranch(const sir::SIRInstruction &inst)
            {
                if (inst.operands.size() != 1 || inst.operands[0].type != sir::SIROperandType::LABEL)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                return emitJump(kJmp, inst.operands[0].name);
            }

            // (AR) القفزُ المشروط BR_COND: {condition, thenLabel, elseLabel}.
            //      الشرطُ إمّا ثابتٌ منطقيّ (⇒ قفزٌ غير مشروطٍ للفرع المختار) أو سجلٌّ
            //      نتيجةُ مقارنةٍ مدموجةٍ في نفس الكتلة (⇒ cmp؛ jCC then؛ jmp else).
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

                // (AR) شرطٌ ثابتٌ منطقيّ: قفزٌ غير مشروطٍ للفرع المحدَّد ثابتًا.
                if (cond.type == sir::SIROperandType::CONSTANT)
                {
                    if (cond.dataType != types::SadTypeKind::Boolean)
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "cond-const-type=" + std::to_string(static_cast<int>(cond.dataType)));
                    return emitJump(kJmp, cond.boolValue ? thenLbl : elseLbl);
                }
                if (cond.type != sir::SIROperandType::REGISTER)
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                "cond-kind=" + std::to_string(static_cast<int>(cond.type)));

                // (AR) شرطٌ سجليّ: يجب أن يكون نتيجةَ مقارنةٍ في هذه الكتلة (إدماج).
                const sir::SIRInstruction *cmp = findFusedComparison(block, cond.name);
                if (!cmp || cmp->operands.size() != 2)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "cond-not-fused-cmp:%" + cond.name);
                // (AR) المعامل الأوّل في المُبدَّد RAX (يُحمَّل من الخانة إن متغيّرَ ذاكرة)، ثمّ
                //      قارنه بالثاني (ثابتٌ imm أو سجلّ). كلا المعامِلَين في الذاكرة غيرُ مدعوم.
                if (!loadInto(x86::RAX, cmp->operands[0]))
                    return false;
                if (!cmpAgainst(cmp->operands[1]))
                    return false;
                const char *jcc = jccForCmp(cmp->opcode);
                if (!jcc)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(*cmp));
                return emitJump(jcc, thenLbl) && emitJump(kJmp, elseLbl);
            }

            // (AR) يجد مقارنةً في الكتلة نتيجتُها cond تُغذّي BR_COND المُنهيَ لها. تُدمَج
            //      فقط إن كانت المقارنةُ آخرَ تعليمةٍ قبل المُنهي (نمطُ البانِي المعتاد) —
            //      وإلّا لا إدماجَ (المقارنةُ غيرُ المدموجة تفشل صراحةً في lowerInstruction).
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

            // (AR) يخفّض كتلةً كاملة: يتخطّى المقارنةَ المدموجة (يعالجها BR_COND)، ويوجّه
            //      المُنهياتِ لمعالِجاتها. الكتلةُ يجب أن تنتهيَ بمُنهٍ.
            bool lowerBlock(const sir::SIRBasicBlock &block)
            {
                const auto &is = block.instructions;
                if (is.empty() || !is.back().isTerminatorInst())
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailNoRet());

                // (AR) نموذج «كلٌّ في سجلّ» لا يحمل حياةً عابرةً للكتل: كلُّ كتلةٍ تبدأ بحوضٍ
                //      نظيف. فقراءةُ سجلٍّ عُرِّف في كتلةٍ أخرى (PHI ضمنيّ، يحتاج انسكابًا/دمجًا)
                //      تغيبُ عن الخريطة ⇒ تفشل صراحةً UNDEF_VREG بدل قراءةِ قمامةٍ صامتة.
                regOf_.clear();
                next_ = 0;

                // (AR) المقارنةُ المدموجة (إن وُجدت) لا تُخفَّض مستقلّةً.
                const sir::SIRInstruction *fused = nullptr;
                if (is.back().opcode == sir::SIROpcode::BR_COND &&
                    !is.back().operands.empty() &&
                    is.back().operands[0].type == sir::SIROperandType::REGISTER)
                    fused = findFusedComparison(block, is.back().operands[0].name);

                for (const sir::SIRInstruction &inst : is)
                {
                    if (&inst == fused)
                        continue; // (AR) مدموجة ⇒ يعالجها BR_COND
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

            // (AR) المرور ٢: يرقّع كلَّ قفزٍ بالفرق الموقَّع (إزاحةُ الهدف − نهايةُ القفز).
            //      نهايةُ القفز = موضعُ الحقل + عرضُه (الحقلُ آخرُ ما يُصدَر). المدى والكتابة
            //      يحترمان عرضَ الحقل ⇒ يصحّان لأيّ عرض (rel32 اليوم، وrel8 مستقبلًا).
            bool applyFixups()
            {
                for (const Fixup &fx : fixups_)
                {
                    // (AR) النداءُ يُحلُّ من funcOffset_، والفرعُ من labelOffset_ (لصيقةٌ مؤهَّلة).
                    const auto &table = fx.isCall ? funcOffset_ : labelOffset_;
                    auto it = table.find(fx.target);
                    if (it == table.end())
                        return fail(EC::INT_NATIVE_LABEL_UNDEFINED, fx.target);
                    long long disp = static_cast<long long>(it->second) -
                                     static_cast<long long>(fx.rel32Pos + fx.width);
                    const long long lim = 1LL << (fx.width * 8 - 1); // (AR) نصفُ مدى الحقل الموقَّع
                    if (disp < -lim || disp > lim - 1)
                        return fail(EC::INT_NATIVE_IMM_RANGE, "rel:" + std::to_string(disp));
                    uint64_t u = static_cast<uint64_t>(disp); // (AR) متمّمُ الاثنين، LE بعرض الحقل
                    for (int i = 0; i < fx.width; ++i)
                        code_[fx.rel32Pos + i] = static_cast<uint8_t>((u >> (8 * i)) & 0xFF);
                }
                return true;
            }
        };

        // (AR) منمنمات SoT العربيّة (تُرآي backend/x86_64/instructions.yaml).
        inline const char *X86SirLowering::kMov = "\xD8\xA7\xD9\x86\xD9\x82\xD9\x84";                 // انقل
        inline const char *X86SirLowering::kAdd = "\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9";                 // اجمع
        inline const char *X86SirLowering::kSub = "\xD8\xA7\xD8\xB7\xD8\xB1\xD8\xAD";                 // اطرح
        inline const char *X86SirLowering::kSys = "\xD9\x86\xD8\xAF\xD8\xA7\xD8\xA1_\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85"; // نداء_نظام
        inline const char *X86SirLowering::kPush = "\xD8\xA7\xD8\xAF\xD9\x81\xD8\xB9";                               // ادفع
        inline const char *X86SirLowering::kPop = "\xD8\xA7\xD8\xB3\xD8\xAD\xD8\xA8";                                // اسحب
        inline const char *X86SirLowering::kRet = "\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9";                                // ارجع
        inline const char *X86SirLowering::kCall = "\xD9\x86\xD8\xA7\xD8\xAF\xD9\x90";                               // نادِ
        inline const char *X86SirLowering::kCmp = "\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86";                                                                                                 // قارن
        inline const char *X86SirLowering::kJmp = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2";                                                                                                 // اقفز
        inline const char *X86SirLowering::kJe = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xB3\xD8\xA7\xD9\x88\xD9\x89";                                     // اقفز_إذا_ساوى
        inline const char *X86SirLowering::kJne = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD9\x84\xD9\x85\x5F\xD9\x8A\xD8\xB3\xD8\xA7\xD9\x88\xD9\x90";        // اقفز_إذا_لم_يساوِ
        inline const char *X86SirLowering::kJl = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD8\xB5\xD8\xBA\xD8\xB1";                                     // اقفز_إذا_أصغر
        inline const char *X86SirLowering::kJle = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD8\xB5\xD8\xBA\xD8\xB1\x5F\xD8\xA3\xD9\x88\x5F\xD8\xB3\xD8\xA7\xD9\x88\xD9\x89"; // اقفز_إذا_أصغر_أو_ساوى
        inline const char *X86SirLowering::kJg = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD9\x83\xD8\xA8\xD8\xB1";                                     // اقفز_إذا_أكبر
        inline const char *X86SirLowering::kJge = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD9\x83\xD8\xA8\xD8\xB1\x5F\xD8\xA3\xD9\x88\x5F\xD8\xB3\xD8\xA7\xD9\x88\xD9\x89"; // اقفز_إذا_أكبر_أو_ساوى

        // (AR) مُيسِّرٌ عالي المستوى: SIRModule ⇒ ثنائيُّ ELF64 ساكن (x86-64) قابل للتنفيذ.
        inline LoweringResult lowerModuleToElf(const sir::SIRModule &module)
        {
            X86SirLowering low;
            LoweringResult r = low.lowerModule(module);
            if (!r.ok)
                return r;
            r.code = elf::writeStaticExec(r.code); // (AR) يلفّ الشيفرة في ELF ويعيدها مكانها
            return r;
        }

    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_SIR_LOWERING_H
