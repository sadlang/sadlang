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
#include "backend/native/generated/native_diagnostics_generated.h"
#include "backend/native/elf64_writer.h"
#include "backend/native/sir_lowering_common.h"

#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

// (AR) كتالوج الأخطاء المولَّد من مصدر الحقيقة (language-truth/errors/*.yaml):
//      المخفّضُ يحمل ErrorCode وبياناتِ {detail} فقط، والرسالةُ تُشتقّ من الكتالوج —
//      لا نصَّ رسالةٍ مباشرًا في هذا الكود (قاعدة SoT: كلّها من YAML).
#include "error_codes.h"
#include "error_messages_generated.h"

#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace sad
{
    namespace native
    {
        // (AR) أسماءٌ مختصرة لأنواع SIR (فضاء الاسم Sad::Compiler::SIR) وأنواع البيانات.
        namespace sir = ::Sad::Compiler::SIR;
        namespace types = ::Sad::Types;

        // (AR) أرقامُ نداءات SysV/x86-64 وثوابتُ الطباعة (من abi/x86_64-linux.yaml). ثوابتُ
        //      مسمّاةٌ لا أرقامٌ سحريّة (قاعدة: لا سلاسلَ/أرقامًا حرفيّةً ذاتَ معنًى منطقيّ خام).
        inline constexpr long long kSysExitX86 = 60;   // (AR) exit
        inline constexpr long long kSysWriteX86 = 1;   // (AR) write
        inline constexpr long long kFdStdout = 1;      // (AR) واصفُ الخرج القياسيّ (stdout)
        inline constexpr long long kAsciiZero = 0x30;  // (AR) رمزُ الصفر ASCII ('0') — أساسُ itoa
        inline constexpr long long kAsciiMinus = 0x2D; // (AR) رمزُ السالب ASCII ('-') — بادئةُ العدد السالب
        inline constexpr long long kItoaRadix = 10;    // (AR) أساسُ التحويل العشريّ

        // (AR) تخصيصُ الكومة الحرّة: mmap مباشرةً عبر syscall (لا libc في الخلفيّة الساكنة).
        //      addr=0 (النواةُ تختار)، prot=قراءة|كتابة، flags=خاصّ|مجهول (الذاكرةُ مُصفّرةٌ
        //      سلفًا ⇒ tags/homogKind للمصفوفة يُصفّران مجّانًا)، fd=-1، offset=0.
        inline constexpr long long kSysMmapX86 = 9;      // (AR) mmap
        inline constexpr long long kProtReadWrite = 0x3; // (AR) PROT_READ|PROT_WRITE
        inline constexpr long long kMapPrivAnon = 0x22;  // (AR) MAP_PRIVATE|MAP_ANONYMOUS
        inline constexpr long long kMmapNoFd = -1;       // (AR) fd للتخصيص المجهول = ‎-1‎

        // (AR) تخطيطُ SadArray الخماسيّ (يُطابق مسارَ LLVM array_ops.cpp:49): إزاحاتٌ ثابتة.
        //      المتجانسةُ العدديّة: tags=null (المسارُ الساكن)، homogKind=Integer=0 (كلاهما صفرٌ
        //      من mmap). خانةُ العنصر ٨ بايت (SAD_ARRAY_SLOT_BYTES) موحَّدةٌ لكلّ الأنواع.
        inline constexpr long long kArrOffLen = 0;       // (AR) الطول (i64)
        inline constexpr long long kArrOffCap = 8;       // (AR) السعة (i64)
        inline constexpr long long kArrOffData = 16;     // (AR) مؤشّرُ البيانات (ptr)
        inline constexpr long long kArrOffTags = 24;     // (AR) مؤشّرُ مخزنِ الوسوم (ptr) — للعناصر المعلَّبة
        inline constexpr long long kArrOffHomog = 32;    // (AR) نوعُ المصفوفة المتجانسة (i8) — يُقرأ حين tags=null
        inline constexpr long long kArrHeaderBytes = 40; // (AR) حجمُ رأس البنية (٥ حقول مع الحشو)
        inline constexpr long long kArrSlotBytes = 8;    // (AR) بايتات خانةِ العنصر الواحد
        inline constexpr long long kTagSlotBytes = 8;    // (AR) خانةُ وسمٍ ٨-بت لكلّ عنصر (i64؛ يتجنّب movzx m8 — الخلفيّةُ الأصليّة مستقلّةٌ عن تخطيط LLVM ذي البايت)

        // (AR) وسومُ النوع زمنَ التشغيل (تعكس DynKind في sad_dyn_repr.h ⇒ تطابق مسارَ LLVM):
        //      Null=0، Int=1، Float=2، Str=3، Bool=4. تُخزَّن بايتًا في مخزن الوسوم، وتُقرأ عند
        //      القراءة المعلَّبة (ARRAY_GET Any) لتقرير نوعِ العنصر زمنَ التشغيل ثمّ توزيعِ الطباعة.
        inline constexpr long long kDynKindNull = 0;
        inline constexpr long long kDynKindInt = 1;
        inline constexpr long long kDynKindFloat = 2;
        inline constexpr long long kDynKindStr = 3;
        inline constexpr long long kDynKindBool = 4;

        // (AR) نصوصُ طباعةِ القيمة المعلَّبة (تطابق المفسّر value.cpp:476/عدم) — ثوابتُ مسمّاةٌ لا حرفيّاتٌ خام.
        inline const std::string kDynBoolTrueText = "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD";  // صحيح
        inline const std::string kDynBoolFalseText = "\xD8\xAE\xD8\xB7\xD8\xA3";          // خطأ
        inline const std::string kDynNullText = "\xD8\xB9\xD8\xAF\xD9\x85";               // عدم

        // (AR) واصفُ النصّ المعلَّب (ذاتيُّ الوصف): طولٌ ٦٤-بت (LE) يليه البايتات. النصُّ المعلَّب
        //      يُقرأ نوعُه زمنَ التشغيل من الوسم، فطولُه غيرُ معلومٍ عند الطباعة إلّا من الواصف. دالّةٌ
        //      حرّةٌ يشترك فيها مخفّضا x86 وARM64 (كلاهما يُفرِّد الواصفَ في rodata كأيّ ثابت).
        inline std::string makeStrDescriptor(const std::string &s)
        {
            std::string desc;
            const unsigned long long len = static_cast<unsigned long long>(s.size());
            for (int i = 0; i < 8; ++i)
                desc.push_back(static_cast<char>((len >> (8 * i)) & 0xFF));
            desc += s;
            return desc;
        }

        // (AR) الوسمُ (DynKind) من نوعِ العنصر المحدَّد زمنَ الترجمة (كلُّ ARRAY_SET له نوعُ قيمةٍ محدَّد).
        //      صحيح/بايت/طبيعي64 ⇒ Int، عشريّ ⇒ Float، منطقيّ ⇒ Bool، عدم ⇒ Null؛ غيرها ⇒ فشلٌ صريح.
        //      دالّةٌ حرّةٌ (يشترك فيها مخفّضا x86 وARM64). النصُّ يُعالَج خصّيصًا (وسمُ Str + عنوانُ واصف).
        inline bool dynTagForType(types::SadTypeKind t, long long &tag)
        {
            switch (t)
            {
            case types::SadTypeKind::Integer:
            case types::SadTypeKind::Byte:
            case types::SadTypeKind::UInt64:
                tag = kDynKindInt; return true;
            case types::SadTypeKind::Float: tag = kDynKindFloat; return true;
            case types::SadTypeKind::Boolean: tag = kDynKindBool; return true;
            case types::SadTypeKind::Null: tag = kDynKindNull; return true;
            default: return false;
            }
        }
        // (AR) رمزُ خروجِ الهلع عند تجاوز حدّ المصفوفة (فحصُ مدًى لا-موقَّع idx ≥ len ⇒ يشمل الفهرسَ
        //      السالبَ الذي يلتفّ إلى قيمةٍ ضخمة). القيمةُ ١٣٤ = ‎128+SIGABRT‎ (عرفُ الإجهاض).
        inline constexpr long long kArrayBoundsPanicCode = 134;

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

                // (AR) ألحِق كتلةَ البيانات (سلاسلُ الطباعة) بعد كلّ الشيفرة في نفس مقطع R+X،
                //      ثمّ رقّع كلَّ mov r64,imm64 بالعنوان المطلق للسلسلة. العنوانُ المطلقُ =
                //      vbase + إزاحةُ الشيفرة (الرأس) + قاعدةُ rodata داخل code_ + إزاحةُ السلسلة.
                //      يعتمد ثباتَ vbase في ET_EXEC (يُمرَّر نفسُه إلى writeStaticExec).
                if (!strFixups_.empty() || !rodata_.empty())
                {
                    const size_t rodataBase = code_.size();
                    code_.insert(code_.end(), rodata_.begin(), rodata_.end());
                    for (const StrFixup &sf : strFixups_)
                    {
                        const uint64_t vaddr = elf::kDefaultVBase + elf::kCodeOffset +
                                               static_cast<uint64_t>(rodataBase + sf.rodataOff);
                        for (int i = 0; i < 8; ++i)
                            code_[sf.imm64Pos + i] = static_cast<uint8_t>((vaddr >> (8 * i)) & 0xFF);
                    }
                }

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
                phiEdges_.clear();
                crossBlockSpill_.clear();
                regOf_.clear();
                strReg_.clear();
                next_ = 0;
                frameSize_ = 0;
                spillBase_ = 0; // (AR) تأمينٌ دفاعيّ: لا يُستعمَل إلّا حين hasCall (يُضبَط في assignFrameSlots)
                idivScratchDisp_ = 0; // (AR) يُضبَط في assignFrameSlots حين تحوي الدالّةُ قسمةً/باقيًا
                shiftScratchDisp_ = 0; // (AR) يُضبَط حين تحوي الدالّةُ إزاحةً بمقدارٍ متغيّر
                printBufTopDisp_ = 0; // (AR) يُضبَط في assignFrameSlots حين تطبع الدالّةُ عددًا
                floatValDisp_ = 0;    // (AR) يُضبَط في assignFrameSlots حين تطبع الدالّةُ عشريًّا
                dynSlotBaseDisp_ = 0;
                dynGetCount_ = 0;
                dynSlotNext_ = 0;     // (AR) فهرسُ خانةِ dyn الجاري (يُصفَّر لكلّ دالّة)
                appendPDisp_ = appendValDisp_ = appendLenDisp_ = appendNewDisp_ = appendCapDisp_ = 0;

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
            // (AR) سياقُ التخفيض الحاليّ (يُضبَط في lowerBlock): يمكّن allocReg من استرجاعِ
            //      سجلِّ مؤقّتٍ ميّتٍ عند نفاد الحوض (لازمٌ للمصفوفات كثيرةِ المؤقّتات).
            const sir::SIRBasicBlock *curBlock_ = nullptr;
            size_t curInstIdx_ = 0;
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
            // (AR) حوافُ PHI: اسمُ الكتلةِ السَّلَف ⇒ قائمةُ (معاملِ القيمةِ الوارد، اسمُ ناتجِ PHI).
            //      نموذجُ «الحوض النظيف لكلّ كتلة» لا يحمل قيمةً عابرةً بسجلّ ⇒ نحمِلها بخانةِ إطارٍ:
            //      السَّلَفُ يخزّن قيمتَه الواردةَ في الخانة قبل قفزِه، والكتلةُ الدامجةُ تقرؤها لاحقًا.
            std::map<std::string, std::vector<std::pair<sir::SIROperand, std::string>>> phiEdges_;
            // (AR) قيمٌ تُعرَّف في كتلةٍ وتُستعمَل في أخرى (عدا نتائجَ PHI المُدارةَ بحوافّها): تُنسَك عند
            //      تعريفها في خانةِ إطارٍ فتُقرأ memSlot_-أوّلًا عبر الحدّ. النموذجُ «حوضٌ نظيفٌ لكلّ كتلة»
            //      لا يحمل قيمةً بسجلّ عبر الكتل ⇒ هذا الجسرُ يُمكّن لولبَ المدى الديناميّ (والتعابيرَ الشرطيّة).
            std::set<std::string> crossBlockSpill_;
            long long frameSize_ = 0;

            // (AR) الانسكابُ عبر النداء: إزاحةُ الخانة الأولى لمنطقة انسكابِ سجلّات الحوض
            //      (خانةٌ لكلّ سجلٍّ من pool_). النداءُ يدهس كلَّ الحوض (caller-saved)، فتُنسَك
            //      المؤقّتاتُ الحيّةُ إليها قبله وتُعاد بعده؛ وتُحمَّل وسائطُ المؤقّتات منها
            //      (لا من السجلّات) ⇒ صفر تصادمٍ في نقل الوسائط (حلٌّ موحَّد للنقل المتوازي).
            long long spillBase_ = 0;

            // (AR) إزاحةُ خانةِ خدشِ القسمة: نحفظ فيها rdx حولَ تسلسلِ cqo/idiv (idiv يدهس
            //      rdx بالباقي)، فلا يُفقَد مؤقّتٌ حيٌّ كان في rdx. صفرٌ إن لم تُنادِ الدالّةُ قسمةً.
            long long idivScratchDisp_ = 0;

            // (AR) خانةُ خدشِ الإزاحة المتغيّرة: نحفظ فيها RCX حولَ إزاحةٍ بمقدارٍ متغيّر (تلزمها CL،
            //      وRCX=pool[1] قد يحمل مؤقّتًا حيًّا). صفرٌ إن لم تحوِ الدالّةُ إزاحةً متغيّرة.
            long long shiftScratchDisp_ = 0;

            // (AR) الطباعة الأصليّة: إزاحةُ قمّةِ مخزنِ itoa (العنوانُ الأعلى، حصريّ) في الإطار؛
            //      تُبنى الأرقامُ العشريّةُ تنازليًّا منها. صفرٌ إن لم تطبع الدالّةُ عددًا.
            long long printBufTopDisp_ = 0;

            // (AR) طباعةُ العشريّ: خانةُ خدشٍ تحفظ نمطَ بتّاتِ الـdouble (القيمةَ المطلقة) عبر
            //      مراحل المُنسِّق (إشارة/جزء صحيح/كسر) — تُعاد قراءتُها بعد كلّ طباعةٍ جزئيّةٍ
            //      تدهس السجلّات. صفرٌ إن لم تطبع الدالّةُ عشريًّا.
            long long floatValDisp_ = 0;

            // (AR) التعليب (المصفوفات المختلطة): كلُّ ARRAY_GET معلَّبٍ (Any) يُنتِج قيمةً ديناميّةً
            //      نمثّلها **مؤشّرًا إلى خانةِ dyn** في الإطار: {tag@0 (i64)، payload@8 (i64)} = ١٦ بايت.
            //      نحجز خانةً لكلّ ARRAY_GET(Any) (بلا تعارضِ أسماء) ونُسنِدها بفهرسٍ جارٍ زمنَ التخفيض.
            long long dynSlotBaseDisp_ = 0; // (AR) قمّةُ منطقةِ خانات dyn (العنوانُ الأعلى، حصريّ)
            int dynGetCount_ = 0;           // (AR) عددُ ARRAY_GET(Any) في الدالّة (لحجزِ الخانات)
            int dynSlotNext_ = 0;           // (AR) الفهرسُ الجاري (يُصفَّر لكلّ دالّة)

            // (AR) الإلحاق (append): خمسُ خاناتِ خدشٍ تبقى حيّةً عبر mmap (الذي يدهس كلَّ الحوض):
            //      مؤشّرُ البنية P، القيمةُ المُلحَقة، الطولُ L المحفوظ، المؤشّرُ الجديد newdata،
            //      والسعةُ الجديدة newcap. تُحجَز حين تحوي الدالّةُ BUILTIN_ARRAY_APPEND.
            long long appendPDisp_ = 0;
            long long appendValDisp_ = 0;
            long long appendLenDisp_ = 0;
            long long appendNewDisp_ = 0;
            long long appendCapDisp_ = 0;

            // (AR) كتلةُ البيانات (rodata): سلاسلُ الطباعة الحرفيّةُ تُلحَق بعد كلّ الشيفرة في
            //      نفس مقطع R+X؛ عنوانُها المطلق (vbase+إزاحة) يُرقَّع في mov r64,imm64.
            std::vector<uint8_t> rodata_;
            std::map<std::string, size_t> internedStr_; // (AR) سلسلة ⇒ إزاحتُها في rodata (تفريدٌ)
            struct StrFixup
            {
                size_t imm64Pos; // (AR) موضعُ الفوريّ ٦٤-بت في code_ (عنوانُ السلسلة المطلق)
                size_t rodataOff; // (AR) إزاحةُ السلسلة داخل rodata_
            };
            std::vector<StrFixup> strFixups_;

            // (AR) سجلٌّ افتراضيٌّ عُرِّف بـ«MOVE %r = سلسلةٌ حرفيّة» ⇒ محتواها. الأمامُ يُخرِج
            //      حرفيَّ السلسلة كـMOVE إلى سجلّ ثمّ يطبعه؛ نموذجُنا لا يحمل السلاسلَ في سجلّات،
            //      فنُسجّل المحتوى هنا (بلا شيفرةٍ للـMOVE) وتُرآيه الطباعةُ لتُصدِر السلسلةَ حرفيًّا.
            //      استعمالُ السجلّ في غير الطباعة (لا سلاسلَ محسوبةً بعد) يفشل صراحةً لاحقًا.
            std::map<std::string, std::string> strReg_;

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

            // (AR) يحمّل العنوانَ المطلق لواصفِ النصّ المعلَّب في سجلّ GPR (mov r64,imm64 نائبٌ يُرقَّع
            //      كأيّ سلسلةِ rodata). يُستعمَل حمولةً في ARRAY_SET(Any) لعنصرٍ نصّيّ.
            bool emitLoadStrDescAddr(int reg, const std::string &content)
            {
                const size_t off = internString(makeStrDescriptor(content));
                if (!emit(x86::mnem::kMov, "r64, imm64", {x86::Operand::R(reg), x86::Operand::I(0, 64)}))
                    return false;
                strFixups_.push_back({code_.size() - 8, off});
                return true;
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

            // (AR) تحليلُ الحياة/الانسكاب (usedAfterInBlock/isPoolArgOfCall/isPoolOperandOf)
            //      نُقِل إلى common — محايدٌ للمعماريّة، مشتركٌ مع مخفّض ARM64 لمنع الانجراف.
            //      يُمرَّر مُسنِدُ «هل متغيّرُ ذاكرة» (memSlot_) لأنّ تمييزَه خاصٌّ بكلّ مخفّض.
            bool isMemName(const std::string &name) const { return memSlot_.find(name) != memSlot_.end(); }

            // (AR) يؤهّل لصيقةَ كتلةٍ باسم الدالّة الحاليّة (يمنع تصادمَ «entry» بين الدوالّ).
            std::string qualify(const std::string &blockName) const
            {
                return currentFn_ + "\x1f" + blockName; // (AR) فاصلُ وحدةٍ لا يظهر في الأسماء
            }

            // (AR) رموزُ {detail} كبياناتٍ محضة (لا نثر): وسمُ الحالة قصيرٌ يُميّز فرعَ الفشل.
            //      النثرُ كلُّه في كتالوج SoT؛ هذه القيمُ تملأ {detail} حصرًا.
            static std::string detailBlocks() { return diag::kBlocksZero; }
            static std::string detailNoRet() { return diag::kNoRetTerminator; }
            static std::string detailOpcode(const sir::SIRInstruction &i)
            {
                return diag::kOpcode + std::to_string(static_cast<int>(i.opcode));
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
                //      **استثناءُ القيمة العابرة للكتل**: تُخصَّص سجلًّا (تُعرَّف فيه) ثمّ تُنسَك في خانتها
                //      مباشرةً؛ القراءةُ اللاحقةُ من الخانة مقصودةٌ لا مُفترِقة (الخانةُ تُكتَب قبل أيّ قراءة).
                if (memSlot_.find(vreg) != memSlot_.end() && !crossBlockSpill_.count(vreg))
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kVregAliasesSlot + diag::kVregSigil + vreg);
                auto it = regOf_.find(vreg);
                if (it != regOf_.end())
                {
                    out = it->second;
                    return true;
                }
                // (AR) المسارُ الأحاديّ (يحفظ بايتات البرامج القائمة تمامًا): خصّص التاليَ ما دام
                //      في الحوض متّسع. لا استرجاعَ ما لم ينفد ⇒ صفر انحدارٍ بايتيّ لما كان يعمل.
                if (next_ < pool_.size())
                {
                    out = pool_[next_++];
                    regOf_[vreg] = out;
                    return true;
                }
                // (AR) نفد الحوضُ الأحاديّ: استرجِعْ سجلَّ مؤقّتٍ ميّتٍ (لا يُقرأ بعد التعليمة
                //      الحاليّة) — لازمٌ للمصفوفات (تُولّد مؤقّتاتٍ كثيرةً قصيرةَ العمر). الاسترجاعُ
                //      بعد أن يُصبح المؤقّتُ ميّتًا ⇒ لا يدهس حيًّا (يحفظُ حَدسَ «الوجهةُ لا تحمل حيًّا»).
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
                        // (AR) مرشّحٌ صالحٌ للاسترجاع: ليس معاملَ التعليمة الحاليّة (قد لا يكون
                        //      استُهلِك بعد) وليس حيًّا بعدها ⇒ ميّتٌ يقينًا، سجلُّه قابلٌ للإعادة.
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

            // ── مُصدِرات التعليمات (كلٌّ يقرأ مواصفتَه من الجدول المولَّد من SoT) ──
            //      كلٌّ يُرجع bool: الفشلُ (مواصفةٌ مفقودة/ثابتٌ خارج المدى) يصعد فيُوقف
            //      التخفيضَ بدل إنتاج ELF مبتورٍ بعلامة نجاح (عيب أميليا رقم ٤).
            bool emit(const std::string &mnemonic, const std::string &form,
                      const std::vector<x86::Operand> &ops)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, form);
                if (!spec)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + diag::kFormSep + form);
                auto bytes = x86::encodeVariable(*spec, ops);
                code_.insert(code_.end(), bytes.begin(), bytes.end());
                return true;
            }

            // (AR) أسماءُ التعليمات (منمنمات) لا تُؤلَّف هنا: مصدرُها الوحيد فضاءُ
            //      x86::mnem في الهيدر المولَّد من SoT (instructions.yaml) ⇒ إعادةُ
            //      تسميةٍ في الـYAML تنتشر آليًّا بلا انجرافٍ صامتٍ في المُخفِّض.

            // (AR) mov r32, imm32 يمتدّ صفريًّا إلى ٦٤؛ يمثّل [0, 2³²) بثلاثةِ بايتاتٍ زائدة. القيمةُ
            //      السالبةُ أو ≥2³² (كخطوةِ لولبٍ ديناميٍّ −1) تُحمَّل بفوريّ ٦٤-بت كامل (mov r64,imm64)
            //      ⇒ لا بترَ إشارة. (كان يُرفَض؛ الآن يُدعَم عبر المسار الكامل.)
            bool movImm(int reg, long long imm)
            {
                if (imm >= 0 && imm <= 0xFFFFFFFFLL)
                    return emit(x86::mnem::kMov, "r32, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
                return movImm64(reg, imm);
            }
            bool movReg(int dst, int src)
            {
                if (dst == src)
                    return true;
                return emit(x86::mnem::kMov, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            // (AR) add/sub r64, imm32 يمتدّ إشاريًّا؛ نقصره على مدى imm32 الموقَّع.
            bool checkImm32(long long imm)
            {
                if (imm < -2147483648LL || imm > 2147483647LL)
                    return fail(EC::INT_NATIVE_IMM_RANGE, diag::kS32 + std::to_string(imm));
                return true;
            }
            bool addImm(int reg, long long imm)
            {
                return checkImm32(imm) && emit(x86::mnem::kAdd, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }
            bool addReg(int dst, int src)
            {
                return emit(x86::mnem::kAdd, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            bool subImm(int reg, long long imm)
            {
                return checkImm32(imm) && emit(x86::mnem::kSub, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }
            bool subReg(int dst, int src) // (AR) dst -= src (29 /r)
            {
                return emit(x86::mnem::kSub, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            bool imulReg(int dst, int src) // (AR) dst *= src (0F AF /r؛ reg=الوجهة)
            {
                return emit(x86::mnem::kImul, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            bool cqo() { return emit(x86::mnem::kCqo, "", {}); }        // (AR) يمدّ إشارةَ rax إلى rdx:rax
            bool idivReg(int divisor)                        // (AR) rdx:rax ÷ divisor ⇒ rax=حاصل، rdx=باقٍ
            {
                return emit(x86::mnem::kIdiv, "r64", {x86::Operand::R(divisor)});
            }
            // ── العمليّات البتّيّة ──
            bool andReg(int dst, int src) { return emit(x86::mnem::kAnd, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)}); }
            bool orReg(int dst, int src) { return emit(x86::mnem::kOr, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)}); }
            bool xorReg(int dst, int src) { return emit(x86::mnem::kXor, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)}); }
            bool notReg(int dst) { return emit(x86::mnem::kNot, "r64", {x86::Operand::R(dst)}); }
            bool shlImm(int dst, long long n) { return emit(x86::mnem::kShl, "r64, imm8", {x86::Operand::R(dst), x86::Operand::I(n, 8)}); }
            bool shrImm(int dst, long long n) { return emit(x86::mnem::kShr, "r64, imm8", {x86::Operand::R(dst), x86::Operand::I(n, 8)}); }
            bool shlCl(int dst) { return emit(x86::mnem::kShl, "r64, cl", {x86::Operand::R(dst)}); } // (AR) dst <<= CL
            bool shrCl(int dst) { return emit(x86::mnem::kShr, "r64, cl", {x86::Operand::R(dst)}); } // (AR) dst >>= CL (منطقيّ)
            bool sarImm(int dst, long long n) { return emit(x86::mnem::kSar, "r64, imm8", {x86::Operand::R(dst), x86::Operand::I(n, 8)}); } // (AR) dst >>= n (حسابيّ)
            bool sarCl(int dst) { return emit(x86::mnem::kSar, "r64, cl", {x86::Operand::R(dst)}); }    // (AR) dst >>= CL (حسابيّ، يمدّ الإشارة)
            bool negReg(int dst) { return emit(x86::mnem::kNeg, "r64", {x86::Operand::R(dst)}); }        // (AR) dst = −dst (متمّمٌ ثنائيّ)
            bool rolImm(int dst, long long n) { return emit(x86::mnem::kRol, "r64, imm8", {x86::Operand::R(dst), x86::Operand::I(n, 8)}); } // (AR) dst = دورانٌ يسارًا n
            bool leaMem(int dst, long long disp) { return emit(x86::mnem::kLea, "r64, m64", {x86::Operand::R(dst), x86::Operand::M(x86::RBP, disp)}); } // (AR) dst = &[rbp+disp]
            // ── المقارنة كقيمة: setcc r8 (٠/١ حسب الأعلام) ثمّ movzx r64,r8 (تمديدُ بالصفر) ──
            bool setccReg(const std::string &mnem, int r8) { return emit(mnem, "r8", {x86::Operand::R(r8)}); }
            // (AR) test r64,r64 — يضبط ZF إن كان السجلّ صفرًا (لاختبارِ منطقيٍّ حيٍّ في فرعٍ غيرِ مدموج).
            bool testReg(int a, int b) { return emit(x86::mnem::kTest, "r64, r64", {x86::Operand::R(a), x86::Operand::R(b)}); }
            bool movzxReg(int dst, int src8) { return emit(x86::mnem::kMovzx, "r64, r8", {x86::Operand::R(dst), x86::Operand::R(src8)}); }

            // ── SSE عدديّ مزدوج: العشريّ يعيش كنمطِ بتّاتٍ i64 في حوض GPR؛ xmm0/xmm1 خدشٌ
            //    عابرٌ للحساب فقط (كـRAX/RDI للصحيح) لا يُخصَّص لسجلٍّ افتراضيّ ⇒ لا حوضَ جديد. ──
            static constexpr int kXmm0 = 0; // (AR) سجلّ عشريّ خدشٌ ٠
            static constexpr int kXmm1 = 1; // (AR) سجلّ عشريّ خدشٌ ١
            bool movqToXmm(int xmm, int gpr) { return emit(x86::mnem::kMovqXmmR64, "xmm, r64", {x86::Operand::R(xmm), x86::Operand::R(gpr)}); }
            bool movqFromXmm(int gpr, int xmm) { return emit(x86::mnem::kMovqR64Xmm, "r64, xmm", {x86::Operand::R(gpr), x86::Operand::R(xmm)}); }
            bool addsd(int d, int s) { return emit(x86::mnem::kAddsd, "xmm, xmm", {x86::Operand::R(d), x86::Operand::R(s)}); }
            bool subsd(int d, int s) { return emit(x86::mnem::kSubsd, "xmm, xmm", {x86::Operand::R(d), x86::Operand::R(s)}); }
            bool mulsd(int d, int s) { return emit(x86::mnem::kMulsd, "xmm, xmm", {x86::Operand::R(d), x86::Operand::R(s)}); }
            bool divsd(int d, int s) { return emit(x86::mnem::kDivsd, "xmm, xmm", {x86::Operand::R(d), x86::Operand::R(s)}); }
            bool cvtsi2sd(int xmm, int gpr) { return emit(x86::mnem::kCvtsi2sd, "xmm, r64", {x86::Operand::R(xmm), x86::Operand::R(gpr)}); }
            bool cvttsd2si(int gpr, int xmm) { return emit(x86::mnem::kCvttsd2si, "r64, xmm", {x86::Operand::R(gpr), x86::Operand::R(xmm)}); }
            bool ucomisd(int a, int b) { return emit(x86::mnem::kUcomisd, "xmm, xmm", {x86::Operand::R(a), x86::Operand::R(b)}); }
            // (AR) cvtsd2si r64, xmm (SSE2): عشريّ ⇒ صحيح بتقريبِ MXCSR (nearest-even افتراضيًّا،
            //      لا نمسّ MXCSR فيبقى الافتراض) = تقريبُ IEEE المطابقُ للمفسّر. نظيرُ cvttsd2si لكن
            //      round-nearest لا truncate. يبقى ضمن SSE2 بلا رفعِ خطّ أساسٍ (بخلاف roundsd).
            //      ⚠️ حارسٌ دفاعيّ (أميليا): تقريبُ الطباعة يعتمد MXCSR.RC=nearest-even الافتراضيّ
            //      (تضبطه النواةُ عند exec؛ لا CRT يبدّله، ولا الخلفيّةُ تبعث ldmxcsr). أيُّ إدخالٍ
            //      مستقبليٍّ لـldmxcsr (مثلًا floor/ceil عبر تبديل وضعِ التقريب) يجب أن يحفظ/يستعيد
            //      MXCSR وإلّا انكسر تقريبُ الطباعة صامتًا. (ARM64 fcvtns يُرمِّز الوضعَ في التعليمة ⇒ مصونٌ ذاتيًّا).
            bool cvtsd2si(int gpr, int xmm) { return emit(x86::mnem::kCvtsd2si, "r64, xmm", {x86::Operand::R(gpr), x86::Operand::R(xmm)}); }
            // (AR) حمّل ثابتَ عشريّ (نمطُ بتّاته i64، إعادةُ تفسيرٍ زمنَ التصريف) في سجلّ GPR.
            bool loadFloatConst(int gpr, double v)
            {
                int64_t bits;
                std::memcpy(&bits, &v, sizeof bits);
                return movImm64(gpr, static_cast<long long>(bits));
            }

            // (AR) مقارنةُ عوائم ⇒ %dst = ٠/١ بدلالة IEEE (النتيجةُ خطأٌ لأيّ NaN عدا !=). العشريّ
            //      يعيش كبتّاتٍ في GPR فنحمّله لـxmm ثمّ ucomisd (يضبط CF/ZF/PF لا-موقَّعةً):
            //      unordered(NaN) ⇒ CF=ZF=PF=1. للترتيب نستعمل seta/setae (تخيَّبان عند CF=1
            //      ⇒ خطأٌ عند NaN تلقائيًّا) مع تبديلِ المعامِلَين للأصغر/الأصغر-أو-يساوي. وللمساواة
            //      نضمّ الشرطَ المرتَّب: == ⇒ (ZF=1) و (PF=0)؛ != ⇒ (ZF=0) أو (PF=1). المُبدَّدات
            //      RAX/RDI خارجَ الحوض (لا تدهسان مؤقّتًا حيًّا).
            bool floatCompareToReg(int dst, const sir::SIROperand &lhs,
                                   const sir::SIROperand &rhs, sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                // (AR) حمّل نمطَي البتّات ثمّ انقلهما لسجلَّي xmm الخدش.
                if (!loadInto(x86::RAX, lhs) || !loadInto(x86::RDI, rhs))
                    return false;
                if (!movqToXmm(kXmm0, x86::RAX) || !movqToXmm(kXmm1, x86::RDI))
                    return false;
                switch (op)
                {
                case OP::GT: // a>b: ucomisd a,b ثمّ seta (CF=0&ZF=0)
                    return ucomisd(kXmm0, kXmm1) && setccReg(x86::mnem::kSeta, x86::RAX) &&
                           movzxReg(dst, x86::RAX);
                case OP::GE: // a>=b: ucomisd a,b ثمّ setae (CF=0)
                    return ucomisd(kXmm0, kXmm1) && setccReg(x86::mnem::kSetae, x86::RAX) &&
                           movzxReg(dst, x86::RAX);
                case OP::LT: // a<b ⟺ b>a: ucomisd b,a ثمّ seta
                    return ucomisd(kXmm1, kXmm0) && setccReg(x86::mnem::kSeta, x86::RAX) &&
                           movzxReg(dst, x86::RAX);
                case OP::LE: // a<=b ⟺ b>=a: ucomisd b,a ثمّ setae
                    return ucomisd(kXmm1, kXmm0) && setccReg(x86::mnem::kSetae, x86::RAX) &&
                           movzxReg(dst, x86::RAX);
                // (AR) EQ/NE: setcc يستهدفُ AL حصرًا (سجلٌّ منخفضٌ لا يلزمه REX)، ثمّ movzx ينقل
                //      كلَّ نتيجةٍ إلى سجلٍّ ٦٤ (RDI ثمّ RAX، مُبدَّدان خارجَ الحوض) قبل إعادةِ استعمالِ AL.
                case OP::EQ: // a==b: (ZF=1) و (PF=0) ⇒ sete∧setnp (NaN ⇒ PF=1 فيُخيَّب)
                    return ucomisd(kXmm0, kXmm1) &&
                           setccReg(x86::mnem::kSete, x86::RAX) && movzxReg(x86::RDI, x86::RAX) &&
                           setccReg(x86::mnem::kSetnp, x86::RAX) && movzxReg(x86::RAX, x86::RAX) &&
                           andReg(x86::RAX, x86::RDI) && movReg(dst, x86::RAX);
                case OP::NE: // a!=b: (ZF=0) أو (PF=1) ⇒ setne∨setp (NaN ⇒ PF=1 فيَصدُق)
                    return ucomisd(kXmm0, kXmm1) &&
                           setccReg(x86::mnem::kSetne, x86::RAX) && movzxReg(x86::RDI, x86::RAX) &&
                           setccReg(x86::mnem::kSetp, x86::RAX) && movzxReg(x86::RAX, x86::RAX) &&
                           orReg(x86::RAX, x86::RDI) && movReg(dst, x86::RAX);
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kVregSigil);
                }
            }

            // ── الذاكرة: خانات الإطار [rbp+إزاحة] ──
            // (AR) mov r64, [rbp+disp] — تحميلٌ من خانة إطار.
            bool loadMem(int dst, long long disp)
            {
                return emit(x86::mnem::kMov, "r64, m64", {x86::Operand::R(dst), x86::Operand::M(x86::RBP, disp)});
            }
            // (AR) mov [rbp+disp], r64 — تخزينٌ في خانة إطار.
            bool storeMem(long long disp, int src)
            {
                return emit(x86::mnem::kMov, "m64, r64", {x86::Operand::M(x86::RBP, disp), x86::Operand::R(src)});
            }
            // (AR) mov [ptrReg], srcLow8 — تخزينُ بايتٍ واحد (88 /r) في العنوان الذي يشير إليه ptrReg.
            bool storeByte(int ptrReg, int srcReg)
            {
                return emit(x86::mnem::kMov, "m8, r8", {x86::Operand::M(ptrReg, 0), x86::Operand::R(srcReg)});
            }
            // (AR) mov r64, imm64 (movabs) — لثابتٍ ٦٤-بت لا يُمثَّل بـimm32 (مثل fd=-1).
            bool movImm64(int reg, long long imm)
            {
                return emit(x86::mnem::kMov, "r64, imm64", {x86::Operand::R(reg), x86::Operand::I(imm, 64)});
            }
            // (AR) mov r64, [base+disp] — تحميلٌ من عنوانٍ بقاعدةٍ عامّة (لا rbp حصرًا).
            bool loadMemBase(int dst, int base, long long disp)
            {
                return emit(x86::mnem::kMov, "r64, m64", {x86::Operand::R(dst), x86::Operand::M(base, disp)});
            }
            // (AR) mov [base+disp], r64 — تخزينٌ في عنوانٍ بقاعدةٍ عامّة.
            bool storeMemBase(int base, long long disp, int src)
            {
                return emit(x86::mnem::kMov, "m64, r64", {x86::Operand::M(base, disp), x86::Operand::R(src)});
            }
            // (AR) cmp r64, [base+disp] — يقارنُ سجلًّا بقيمةِ ذاكرةٍ (3B /r) لضبطِ الأعلام؛ لفحصِ
            //      حدّ المصفوفة: cmp idx, [arr+len]. لا يكتبُ وجهةً ولا يمسّ القاعدة.
            bool cmpMemBase(int reg, int base, long long disp)
            {
                return emit(x86::mnem::kCmp, "r64, m64", {x86::Operand::R(reg), x86::Operand::M(base, disp)});
            }
            // (AR) فحصُ حدّ المصفوفة: يفترضُ idx في RDI ومؤشّرَ البنية في RAX. يقارنُ idx بالطول
            //      لا-موقَّعًا (cmp + jb): إن idx < len (لا-موقَّع) يتخطّى كتلةَ الهلع، وإلّا يخرجُ
            //      بالرمز ١٣٤. الفهرسُ السالبُ يلتفّ إلى قيمةٍ ضخمةٍ لا-موقَّعة ⇒ يفشلُ الفحصَ أيضًا.
            //      كتلةُ الهلع ثابتةُ الطول (١٢ بايتًا) ⇒ إزاحةُ jb القصيرة تُحسَب محلّيًّا وتُرقَّع.
            bool emitBoundsCheck()
            {
                if (!cmpMemBase(x86::RDI, x86::RAX, kArrOffLen))
                    return false;
                const x86::EncSpec *spec = x86::lookupEncSpec(x86::mnem::kJb, "rel8");
                if (!spec || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, std::string(x86::mnem::kJb) + diag::kFormSep + "rel8");
                const size_t jbPos = code_.size();
                if (!emit(x86::mnem::kJb, "rel8", {x86::Operand::I(0, 8)})) // (AR) نائبٌ يُرقَّع بعد كتلةِ الهلع
                    return false;
                // (AR) كتلةُ الهلع: mov edi,134؛ mov eax,60؛ syscall (خروجٌ فوريّ، لا عودة).
                if (!movImm(x86::RDI, kArrayBoundsPanicCode) || !movImm(x86::RAX, kSysExitX86) ||
                    !emit(x86::mnem::kSyscall, "", {}))
                    return false;
                const long long rel = static_cast<long long>(code_.size()) - static_cast<long long>(jbPos + 2);
                if (rel < 0 || rel > 127) // (AR) كتلةُ الهلع قصيرةٌ دومًا ⇒ يجبُ أن تسعَ rel8
                    return fail(EC::INT_NATIVE_IMM_RANGE, diag::kBoundsPanicRel8 + std::to_string(rel));
                code_[jbPos + 1] = static_cast<uint8_t>(rel);
                return true;
            }
            // (AR) mmap(NULL, size, R|W, PRIVATE|ANON, -1, 0) عبر syscall ⇒ المؤشّرُ في RAX.
            //      يدهس RAX/RCX/R11 وسجلّاتِ الوسائط (كلُّها في الحوض) ⇒ يُنسَك حولَه في المستدعي.
            //      وسيطُ syscall الرابعُ في R10 لا RCX. الحجمُ ثابتٌ (سعةُ المصفوفة من الأمام).
            bool emitMmap(long long sizeBytes)
            {
                return movImm(x86::RDI, 0) &&
                       movImm(x86::RSI, sizeBytes) &&
                       movImm(x86::RDX, kProtReadWrite) &&
                       movImm(x86::R10, kMapPrivAnon) &&
                       movImm64(x86::R8, kMmapNoFd) &&
                       movImm(x86::R9, 0) &&
                       movImm(x86::RAX, kSysMmapX86) &&
                       emit(x86::mnem::kSyscall, "", {});
            }
            // (AR) يضع عنوانَ العنصر (data + index×8) في RAX مع فحصِ حدٍّ زمنَ التشغيل. مسارٌ موحَّدٌ
            //      للفهرسِ الثابتِ والمتغيّر: RDI = idx (الثابتُ عبر movImm، المتغيّرُ عبر loadInto)،
            //      RAX = مؤشّرُ البنية؛ ثمّ emitBoundsCheck (idx مقابل [arr+len] لا-موقَّعًا). بعد الفحص:
            //      RAX = [arr+data]، ثمّ RAX += RDI×٨. المشترَكُ بين ARRAY_GET/SET. RAX يبقى مؤشّرَ
            //      البنية عبر cmp (لا يُكتَب) حتّى تحميلِ data.
            // (AR) الفهرسُ السالب: المتغيّرُ (سجلّ) يلتفّ إلى قيمةٍ ضخمةٍ لا-موقَّعة ⇒ يفشلُ الفحصَ
            //      ⇒ هلعُ تشغيلٍ (١٣٤). أمّا الثابتُ السالب (`arr[-1]` حرفيًّا) فيرفضه movImm ⇒ فشلُ
            //      ترجمةٍ صريح (INT_NATIVE_IMM_RANGE) — كلاهما آمنٌ (لا وصولَ خارج الحدّ).
            bool emitElemAddr(const sir::SIROperand &arrOp, const sir::SIROperand &idxOp)
            {
                if (!loadInto(x86::RDI, idxOp) || !loadInto(x86::RAX, arrOp))
                    return false;
                if (!emitBoundsCheck())
                    return false;
                if (!loadMemBase(x86::RAX, x86::RAX, kArrOffData)) // (AR) RAX = مؤشّرُ البيانات
                    return false;
                return shlImm(x86::RDI, 3) && addReg(x86::RAX, x86::RDI); // (AR) RAX += idx×٨
            }
            // (AR) dst = rbp + disp (عنوانُ خانةٍ في الإطار): mov dst,rbp ثمّ add dst,disp.
            bool leaFrame(int dst, long long disp)
            {
                return movReg(dst, x86::RBP) && addImm(dst, disp);
            }
            // (AR) cmp reg, 0 (83 /7 ib) — لفحص «هل الحاصلُ صفر؟» في لولب itoa.
            bool cmpZero(int reg)
            {
                return emit(x86::mnem::kCmp, "r64, imm8", {x86::Operand::R(reg), x86::Operand::I(0, 8)});
            }
            // (AR) jne إلى إزاحةٍ خلفيّةٍ معلومةٍ (لولبٌ محلّيّ داخل تسلسلِ الطباعة، لا لصيقةَ كتلة):
            //      نحسب rel32 مباشرةً (الهدفُ معلومٌ زمنَ الإصدار) فلا حاجةَ لطابور الترقيع.
            bool emitLocalJneBack(size_t target)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(x86::mnem::kJne, "rel32");
                if (!spec || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, std::string(x86::mnem::kJne) + diag::kFormSep + "rel32");
                const long long instrLen = static_cast<long long>(spec->opcode.size()) + spec->imm_bits / 8;
                const long long disp = static_cast<long long>(target) -
                                       (static_cast<long long>(code_.size()) + instrLen);
                if (!checkImm32(disp))
                    return false;
                return emit(x86::mnem::kJne, "rel32", {x86::Operand::I(disp, 32)});
            }

            // (AR) يكتب قيمةً ٣٢-بت موقَّعةً (LE) في موضعٍ سابقٍ من code_ (لترقيعِ قفزةٍ أماميّة).
            void patchLE32(size_t pos, long long val)
            {
                for (int i = 0; i < 4; ++i)
                    code_[pos + i] = static_cast<uint8_t>((val >> (8 * i)) & 0xFF);
            }
            // (AR) قفزةٌ شرطيّةٌ/غيرُ مشروطةٍ أماميّةٌ بإزاحةٍ نائبةٍ (٠) تُرقَّع لاحقًا بـpatchFwd.
            //      تُعيد عبر outRelPos موضعَ حقل rel32 (لترقيعه إلى الهدف حين يُعرَف). للولباتِ
            //      الإلحاق المحلّيّة (لا لصائقَ كتلٍ ⇒ لا طابورَ Fixup العامّ).
            bool emitJccFwd(const std::string &mnem, size_t &outRelPos)
            {
                if (!emit(mnem, "rel32", {x86::Operand::I(0, 32)}))
                    return false;
                outRelPos = code_.size() - 4; // (AR) حقلُ rel32 آخرُ ٤ بايتات
                return true;
            }
            // (AR) يُرقّع قفزةً أماميّةً (حقلُها في relPos) إلى الموضع الحاليّ (نهايةِ الشيفرة).
            void patchFwd(size_t relPos)
            {
                patchLE32(relPos, static_cast<long long>(code_.size()) - static_cast<long long>(relPos + 4));
            }
            // (AR) قفزةٌ شرطيّةٌ خلفيّةٌ إلى هدفٍ معلوم (لبدايةِ لولبِ النسخ).
            bool emitJccBack(const std::string &mnem, size_t target)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnem, "rel32");
                if (!spec || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnem + diag::kFormSep + "rel32");
                const long long instrLen = static_cast<long long>(spec->opcode.size()) + spec->imm_bits / 8;
                const long long disp = static_cast<long long>(target) - (static_cast<long long>(code_.size()) + instrLen);
                if (!checkImm32(disp))
                    return false;
                return emit(mnem, "rel32", {x86::Operand::I(disp, 32)});
            }
            // (AR) mmap بحجمٍ مُهيّأٍ سلفًا في RSI (بخلاف emitMmap ذي الحجم الثابت) — لنموّ الإلحاق
            //      حيث الحجمُ (newcap×٨) يُحسَب زمنَ التشغيل. يضبط بقيّةَ الوسائط ثمّ syscall.
            bool emitMmapPresetSize()
            {
                return movImm(x86::RDI, 0) &&
                       movImm(x86::RDX, kProtReadWrite) &&
                       movImm(x86::R10, kMapPrivAnon) &&
                       movImm64(x86::R8, kMmapNoFd) &&
                       movImm(x86::R9, 0) &&
                       movImm(x86::RAX, kSysMmapX86) &&
                       emit(x86::mnem::kSyscall, "", {});
            }

            // (AR) يطبع سلسلةً حرفيّة: يُفرِّدها في rodata، يحمّل عنوانَها المطلق في rsi (mov r64,imm64
            //      نائبٌ يُرقَّع لاحقًا حين يُعرَف موضعُ rodata)، ثمّ write(stdout, rsi, الطول).
            bool emitPrintString(const std::string &s)
            {
                const size_t off = internString(s);
                // (AR) mov rsi, <عنوانٌ نائب٦٤>؛ سجّل ترقيعَ العنوان المطلق.
                if (!emit(x86::mnem::kMov, "r64, imm64", {x86::Operand::R(x86::RSI), x86::Operand::I(0, 64)}))
                    return false;
                strFixups_.push_back({code_.size() - 8, off});
                // (AR) rdx=الطول، rax=write، rdi=stdout، ثمّ syscall.
                return movImm(x86::RDX, static_cast<long long>(s.size())) &&
                       movImm(x86::RAX, kSysWriteX86) &&
                       movImm(x86::RDI, kFdStdout) &&
                       emit(x86::mnem::kSyscall, "", {});
            }

            // (AR) يطبع عددًا صحيحًا موقَّعًا في RAX: itoa عبر idiv/10 يبني الأرقامَ تنازليًّا في مخزنِ
            //      الإطار، ثمّ write(stdout, المؤشّر، الطول). R10 مؤشّرٌ، RCX=10. السالبُ يُعالَج بحيلةِ
            //      الباقي-السالب (‎'0' − rem‎ للرقم المطلق) بلا `neg RAX` ⇒ **آمنٌ لـINT64_MIN** (نفيُه
            //      يطفح)؛ ثمّ نكتب '-' في المخزن نفسِه (لا syscall منفصل ⇒ لا دهسَ RAX). لا يمسّ R8/R11
            //      (يستعملهما emitPrintFloat عبر ندائه؛ ip موجبٌ دومًا فلا يسلك فرعَ السالب).
            bool emitPrintInt()
            {
                // (AR) R10 = قمّةُ المخزن (rbp+إزاحة، حصريّ)؛ RCX = الأساس ١٠.
                if (!leaFrame(x86::R10, printBufTopDisp_) || !movImm(x86::RCX, kItoaRadix))
                    return false;
                // (AR) الإشارة: إن RAX ≥ ٠ اقفز للولبِ الموجب (لا نمسّ RAX). السالبُ يلي.
                if (!cmpZero(x86::RAX))
                    return false;
                size_t toPositive;
                if (!emitJccFwd(x86::mnem::kJge, toPositive))
                    return false;
                // (AR) لولبُ السالب: cqo؛ idiv ⇒ RAX=حاصل(≤٠)، RDX=باقٍ(≤٠)؛ الرقم = '0' − RDX (=|باقٍ|).
                const size_t negLoop = code_.size();
                if (!cqo() || !idivReg(x86::RCX) ||
                    !movImm(x86::R9, kAsciiZero) || !subReg(x86::R9, x86::RDX) ||
                    !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::R9) ||
                    !cmpZero(x86::RAX) || !emitLocalJneBack(negLoop))
                    return false;
                // (AR) بادئةُ '-' في المخزن نفسِه (تسبق الأرقام).
                if (!movImm(x86::R9, kAsciiMinus) || !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::R9))
                    return false;
                size_t toWrite;
                if (!emitJccFwd(x86::mnem::kJmp, toWrite))
                    return false;
                // (AR) لولبُ الموجب: cqo؛ idiv ⇒ RDX=باقٍ(٠..٩)؛ الرقم = '0' + RDX.
                patchFwd(toPositive);
                const size_t posLoop = code_.size();
                if (!cqo() || !idivReg(x86::RCX) ||
                    !addImm(x86::RDX, kAsciiZero) ||
                    !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::RDX) ||
                    !cmpZero(x86::RAX) || !emitLocalJneBack(posLoop))
                    return false;
                patchFwd(toWrite);
                // (AR) rsi=المؤشّرُ إلى أوّل رمز؛ rdx=الطول=(القمّة − r10)؛ write(stdout).
                return movReg(x86::RSI, x86::R10) &&
                       leaFrame(x86::RDX, printBufTopDisp_) &&
                       subReg(x86::RDX, x86::R10) &&
                       movImm(x86::RAX, kSysWriteX86) &&
                       movImm(x86::RDI, kFdStdout) &&
                       emit(x86::mnem::kSyscall, "", {});
            }

            // (AR) cmp reg, imm8 عامّ (لا صفرًا حصرًا) — لفحص «nd>1» في حذفِ الأصفار.
            bool cmpImm8(int reg, long long imm) { return emit(x86::mnem::kCmp, "r64, imm8", {x86::Operand::R(reg), x86::Operand::I(imm, 8)}); }
            // (AR) cmp reg, reg — لفحص «المؤشّر بلغ بدايةَ الحقل» في حشوِ الأصفار البادئة.
            bool cmpRegReg(int a, int b) { return emit(x86::mnem::kCmp, "r64, r64", {x86::Operand::R(a), x86::Operand::R(b)}); }
            // (AR) قفزةٌ غيرُ مشروطةٌ خلفيّةٌ إلى هدفٍ معلوم (لولباتُ المُنسِّق المحلّيّة).
            bool emitJmpBack(size_t target) { return emitJccBack(x86::mnem::kJmp, target); }

            // (AR) ثوابتُ نمطِ بتّاتٍ + رموزٌ للمُنسِّق العشريّ (لا حرفيّاتٍ خام).
            static constexpr unsigned long long kF64SignMask = 0x8000000000000000ULL; // بتّ الإشارة
            static constexpr unsigned long long kF64AbsMask = 0x7FFFFFFFFFFFFFFFULL;  // مسحُ الإشارة ⇒ |x|
            static constexpr long long kFloatPrecisionScale = 1000000; // ١٠^٦ (setprecision(6))
            static constexpr int kFloatDecimals = 6;                   // خاناتٌ عشريّة (يطابق المفسّر)

            // (AR) يطبع عددًا عشريًّا (نمطُ بتّاته i64 في RAX) بصيغةِ المفسّر: fixed(6) ثمّ حذفُ
            //      الأصفار الزائدة مع إبقاءِ رقمٍ ≥١ بعد النقطة (‎3.5‎ لا ‎3.500000‎، ‎42.0‎ لا ‎42‎).
            //      العشريّ يُحسَب عبر xmm0/xmm1؛ السجلّاتُ (R8/R9/R10/RAX/RDX/RCX/RSI/RDI) خدشٌ
            //      (الحوضُ مُنسكَبٌ حولَ الطباعة). النصفُ (‎+0.5‎) يقرّب الخانةَ السادسة.
            bool emitPrintFloat()
            {
                // (١) خزّن البتّاتِ الأصليّة؛ إن كانت الإشارةُ سالبةً اطبع '-' واحسب |القيمة|.
                if (!storeMem(floatValDisp_, x86::RAX))
                    return false;
                if (!movImm64(x86::R8, static_cast<long long>(kF64SignMask)) ||
                    !andReg(x86::R8, x86::RAX) || !cmpZero(x86::R8))
                    return false;
                size_t skipNeg;
                if (!emitJccFwd(x86::mnem::kJe, skipNeg))
                    return false;
                if (!emitPrintString("-")) // (AR) يدهس RAX/RSI/RDX/RDI — البتّاتُ محفوظةٌ في الخانة
                    return false;
                patchFwd(skipNeg);
                // |القيمة| = البتّات & قناعُ مسحِ الإشارة ⇒ خزّنها (نُعيد قراءتَها في كلّ مرحلة).
                if (!loadMem(x86::RAX, floatValDisp_) ||
                    !movImm64(x86::R8, static_cast<long long>(kF64AbsMask)) ||
                    !andReg(x86::RAX, x86::R8) || !storeMem(floatValDisp_, x86::RAX))
                    return false;
                // (٢) الجزءُ الصحيح ip = trunc(|x|) في R11 (يبقى عبر الطباعة؛ emitPrintInt لا يمسّه).
                if (!movqToXmm(kXmm0, x86::RAX) || !cvttsd2si(x86::R11, kXmm0))
                    return false;
                // (٣) الكسر: frac = |x| − ip ؛ scaled = round_nearest_even(frac×١٠^٦) في R8.
                //     cvtsd2si (تقريبُ MXCSR = nearest-even، لا half-up ‎+0.5‎) يطابق تقريبَ IEEE
                //     الافتراضيّ في المفسّر (std::fixed) ⇒ يسدّ انحرافَ ULP في تعادلِ الخانة السابعة.
                if (!movqToXmm(kXmm0, x86::RAX) || !cvtsi2sd(kXmm1, x86::R11) || !subsd(kXmm0, kXmm1))
                    return false;
                if (!loadFloatConst(x86::RAX, static_cast<double>(kFloatPrecisionScale)) ||
                    !movqToXmm(kXmm1, x86::RAX) || !mulsd(kXmm0, kXmm1))
                    return false;
                if (!cvtsd2si(x86::R8, kXmm0)) // R8 = scaled ∈ [0, ١٠^٦] (تقريبُ أقربِ زوج)
                    return false;
                // (٤) ترحيلُ الحمل: إن قرّب الكسرُ إلى ١٫٠ (scaled == ١٠^٦) زِد ip وصفّر scaled — قبل
                //      طباعةِ ip (عائقُ أميليا: «1.0 − 4e−7» كان يطبع «0.0» بدل «1.0»).
                if (!movImm(x86::RCX, kFloatPrecisionScale) || !cmpRegReg(x86::R8, x86::RCX))
                    return false;
                size_t jneNoCarry;
                if (!emitJccFwd(x86::mnem::kJne, jneNoCarry))
                    return false;
                if (!addImm(x86::R11, 1) || !movImm(x86::R8, 0))
                    return false;
                patchFwd(jneNoCarry);
                // (٥) اطبع ip (RAX = R11) ثمّ النقطة. R8 (scaled) يبقى (لا يمسّه itoa/الطباعة النصّيّة).
                if (!movReg(x86::RAX, x86::R11) || !emitPrintInt() || !emitPrintString("."))
                    return false;
                // (٦) حذفُ الأصفار الزائدة: nd=٦؛ بينما (nd>1 && scaled%10==0) اقسم على ١٠ وأنقص nd.
                if (!movImm(x86::R9, kFloatDecimals) || !movImm(x86::RCX, kItoaRadix))
                    return false;
                size_t stripTop = code_.size();
                if (!cmpImm8(x86::R9, 1))
                    return false;
                size_t jleStripDone;
                if (!emitJccFwd(x86::mnem::kJle, jleStripDone))
                    return false;
                if (!movReg(x86::RAX, x86::R8) || !cqo() || !idivReg(x86::RCX) || !cmpZero(x86::RDX))
                    return false;
                size_t jneStripDone;
                if (!emitJccFwd(x86::mnem::kJne, jneStripDone))
                    return false;
                if (!movReg(x86::R8, x86::RAX) || !subImm(x86::R9, 1) || !emitJmpBack(stripTop))
                    return false;
                patchFwd(jleStripDone);
                patchFwd(jneStripDone);
                // (٦) ابنِ nd رقمًا تنازليًّا (الأصفارُ البادئةُ تُحشى) ثمّ write(stdout).
                //     R10 مؤشّرُ الكتابة (يبدأ من القمّة)؛ RDI = القمّة − nd (بدايةُ الحقل).
                if (!leaFrame(x86::RSI, printBufTopDisp_) || !movReg(x86::RDI, x86::RSI) ||
                    !subReg(x86::RDI, x86::R9) || !movReg(x86::R10, x86::RSI))
                    return false;
                if (!movReg(x86::RAX, x86::R8)) // RAX = scaled
                    return false;
                size_t digitTop = code_.size();
                if (!cqo() || !idivReg(x86::RCX) || !addImm(x86::RDX, kAsciiZero) ||
                    !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::RDX) ||
                    !cmpZero(x86::RAX) || !emitJccBack(x86::mnem::kJne, digitTop))
                    return false;
                // حشوُ الأصفار البادئة: بينما R10 > RDI أدخِل '0'.
                size_t padTop = code_.size();
                if (!cmpRegReg(x86::R10, x86::RDI))
                    return false;
                size_t jlePadDone;
                if (!emitJccFwd(x86::mnem::kJle, jlePadDone))
                    return false;
                if (!subImm(x86::R10, 1) || !movImm(x86::RAX, kAsciiZero) ||
                    !storeByte(x86::R10, x86::RAX) || !emitJmpBack(padTop))
                    return false;
                patchFwd(jlePadDone);
                return movReg(x86::RSI, x86::RDI) && movReg(x86::RDX, x86::R9) &&
                       movImm(x86::RAX, kSysWriteX86) && movImm(x86::RDI, kFdStdout) &&
                       emit(x86::mnem::kSyscall, "", {});
            }

            // (AR) طباعةُ قيمةٍ معلَّبة (Any): ptrReg يشير إلى خانةِ dyn {tag@0، payload@8}. نوزّع
            //      على الوسم زمنَ التشغيل: Float⇒المُنسِّق، Bool⇒«صحيح»/«خطأ»، Int⇒itoa، غيرها⇒«عدم».
            //      R8=الوسم، R9=الحمولة (يبقيان حتّى دخولِ الفرع؛ الفرعُ يقفز للنهاية بعد الطباعة).
            bool emitPrintBoxed(int ptrReg)
            {
                if (!loadMemBase(x86::R8, ptrReg, 0) || !loadMemBase(x86::R9, ptrReg, 8))
                    return false;
                std::vector<size_t> endJmps;
                auto branchEnd = [&](size_t &pos) { return emitJccFwd(x86::mnem::kJmp, pos); };
                // Float ⇒ المُنسِّق
                size_t notF;
                if (!cmpImm8(x86::R8, kDynKindFloat) || !emitJccFwd(x86::mnem::kJne, notF))
                    return false;
                {
                    size_t j;
                    if (!movReg(x86::RAX, x86::R9) || !emitPrintFloat() || !branchEnd(j))
                        return false;
                    endJmps.push_back(j);
                }
                patchFwd(notF);
                // Bool ⇒ «صحيح»/«خطأ» حسب الحمولة (٠/١)
                size_t notB;
                if (!cmpImm8(x86::R8, kDynKindBool) || !emitJccFwd(x86::mnem::kJne, notB))
                    return false;
                {
                    size_t isFalse;
                    if (!cmpZero(x86::R9) || !emitJccFwd(x86::mnem::kJe, isFalse))
                        return false;
                    size_t j1;
                    if (!emitPrintString(kDynBoolTrueText) || !branchEnd(j1))
                        return false;
                    endJmps.push_back(j1);
                    patchFwd(isFalse);
                    size_t j2;
                    if (!emitPrintString(kDynBoolFalseText) || !branchEnd(j2))
                        return false;
                    endJmps.push_back(j2);
                }
                patchFwd(notB);
                // Int ⇒ itoa
                size_t notI;
                if (!cmpImm8(x86::R8, kDynKindInt) || !emitJccFwd(x86::mnem::kJne, notI))
                    return false;
                {
                    size_t j;
                    if (!movReg(x86::RAX, x86::R9) || !emitPrintInt() || !branchEnd(j))
                        return false;
                    endJmps.push_back(j);
                }
                patchFwd(notI);
                // Str ⇒ نصٌّ: R9 = عنوانُ الواصف {len@0، bytes@8}. اكتب bytes بطولِ len (write).
                size_t notS;
                if (!cmpImm8(x86::R8, kDynKindStr) || !emitJccFwd(x86::mnem::kJne, notS))
                    return false;
                {
                    if (!loadMemBase(x86::RDX, x86::R9, 0) ||          // RDX = الطول
                        !movReg(x86::RSI, x86::R9) || !addImm(x86::RSI, 8) || // RSI = البايتات
                        !movImm(x86::RAX, kSysWriteX86) || !movImm(x86::RDI, kFdStdout) ||
                        !emit(x86::mnem::kSyscall, "", {}))
                        return false;
                    size_t j;
                    if (!branchEnd(j))
                        return false;
                    endJmps.push_back(j);
                }
                patchFwd(notS);
                // غيرها (Null) ⇒ «عدم»
                if (!emitPrintString(kDynNullText))
                    return false;
                for (size_t j : endJmps)
                    patchFwd(j);
                return true;
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
                if (common::isConstInt(op, c))
                    return movImm(dst, c);
                // (AR) ثابتٌ عشريّ ⇒ نمطُ بتّاته i64 (العشريّ يعيش كبتّاتٍ في GPR).
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Float)
                    return loadFloatConst(dst, op.floatValue);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    long long disp;
                    if (isMemVar(op, disp))
                        return loadMem(dst, disp);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, diag::kVregSigil + op.name);
                    return loadMem(dst, spillDisp(static_cast<size_t>(poolIndexOf(it->second))));
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            diag::kArgKind + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) يحمّل معاملًا (ثابتًا/سجلًّا فيزيائيًّا/متغيّرَ ذاكرة) في سجلٍّ وجهة. قراءةُ
            //      متغيّرِ الذاكرة = تحميلٌ من خانته (يُطابق auto-load في خلفيّة LLVM).
            bool loadInto(int dst, const sir::SIROperand &op)
            {
                if (op.type == sir::SIROperandType::CONSTANT)
                {
                    // (AR) ثابتٌ عشريّ ⇒ حمّل نمطَ بتّاته i64 (العشريّ يعيش كبتّاتٍ في GPR).
                    if (op.dataType == types::SadTypeKind::Float)
                        return loadFloatConst(dst, op.floatValue);
                    if (op.dataType != types::SadTypeKind::Integer)
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    diag::kConstType + std::to_string(static_cast<int>(op.dataType)));
                    return movImm(dst, op.intValue);
                }
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    long long disp;
                    if (isMemVar(op, disp))
                        return loadMem(dst, disp);
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, diag::kVregSigil + op.name);
                    return movReg(dst, it->second);
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            diag::kOperandKind + std::to_string(static_cast<int>(op.type)));
            }

            // (AR) المسحُ المسبق: يخصّص خانةَ إطارٍ للمعاملات (بترتيب ABI) ثمّ لكلّ ALLOC،
            //      ويحسب حجمَ الإطار المُحاذى ١٦. المعاملُ يُعامَل كمتغيّرِ ذاكرةٍ (قراءتُه =
            //      تحميلٌ من خانته)، وتُسكَنُ خانتُه من سجلّ الوسيط الوارد في المقدّمة.
            bool assignFrameSlots(const sir::SIRFunction &fn,
                                  const std::vector<std::shared_ptr<sir::SIRBasicBlock>> &blocks)
            {
                const auto &params = fn.getParameters();
                if (params.size() > 6)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kParamsGt6 + std::to_string(params.size()));
                long long used = 0;
                for (const auto &p : params)
                {
                    used += 8;
                    // (AR) المعاملُ يُشار إليه في التعابير بـ«%»+الاسم (sir_builder_functions.cpp:495)
                    //      بينما اسمُ SIRParameter بلا «%» ⇒ نُفهرِس بالمرجع كي يطابقَه isMemVar.
                    memSlot_[diag::kVregSigil + p.name] = -used;
                }
                bool hasCall = false;
                bool hasIdiv = false;
                bool hasVarShift = false;    // (AR) إزاحةٌ بمقدارٍ متغيّر (تلزمها خانةُ حفظِ RCX)
                bool hasArrayNew = false;    // (AR) ARRAY_NEW ⇒ mmap يدهس الحوض (يلزمه انسكابٌ حولَه)
                bool hasAppend = false;      // (AR) BUILTIN_ARRAY_APPEND ⇒ mmap (عند النموّ) + خانات خدش
                bool hasPrint = false;       // (AR) أيُّ BUILTIN_PRINT (يلزمه انسكابُ الحوض حولَه)
                bool hasNumberPrint = false; // (AR) طباعةُ عددٍ (تلزمها خانةُ مخزنِ itoa)
                bool hasFloatPrint = false;  // (AR) طباعةُ عشريّ (تلزمها خانةُ خدشِ البتّات + مُنسِّق fixed6)
                bool hasBoxing = false;      // (AR) SET/GET معلَّبٌ (Any) ⇒ يستعمل الحوضَ خدشًا + خانات dyn
                dynGetCount_ = 0;
                for (const auto &blockPtr : blocks)
                    for (const auto &inst : blockPtr->instructions)
                    {
                        // (AR) SET معلَّبٌ: الأمامُ يضع Any على **معامِل** المصفوفة. GET معلَّبٌ:
                        //      الأمامُ يضع Any على **نتيجة** القراءة (لا المعامل) — كما يكشفه مسارُ LLVM.
                        if (inst.opcode == sir::SIROpcode::ARRAY_SET && !inst.operands.empty() &&
                            inst.operands[0].elementType == types::SadTypeKind::Any)
                            hasBoxing = true;
                        if (inst.opcode == sir::SIROpcode::ARRAY_GET && inst.result &&
                            inst.result->dataType == types::SadTypeKind::Any)
                        {
                            hasBoxing = true;
                            ++dynGetCount_; // (AR) خانةُ dyn لكلّ قراءةٍ معلَّبة
                        }
                        // (AR) PHI: احجز خانةَ إطارٍ لناتجِه (كـALLOC) — تسجيلُه في memSlot_ يجعل
                        //      كلَّ قراءةٍ لاحقةٍ له تُحلُّ تحميلًا من الخانة تلقائيًّا (memSlot_ أوّلًا).
                        //      وسجّل كلَّ حافّةٍ (القيمةُ الواردة، لصيقةُ السَّلَف) بأزواجِ [قيمة، لصيقة].
                        if (inst.opcode == sir::SIROpcode::PHI && inst.result)
                        {
                            used += 8;
                            memSlot_[inst.result->name] = -used;
                            for (size_t k = 0; k + 1 < inst.operands.size(); k += 2)
                                phiEdges_[inst.operands[k + 1].name].push_back(
                                    {inst.operands[k], inst.result->name});
                        }
                        if (inst.opcode == sir::SIROpcode::ALLOC && inst.result)
                        {
                            used += 8;
                            memSlot_[inst.result->name] = -used; // [rbp−8]، [rbp−16]، …
                        }
                        else if (inst.opcode == sir::SIROpcode::CALL)
                            hasCall = true;
                        else if (inst.opcode == sir::SIROpcode::MOD_I64 ||
                                 inst.opcode == sir::SIROpcode::FLOOR_DIV_I64)
                            hasIdiv = true;
                        else if ((inst.opcode == sir::SIROpcode::SHL ||
                                  inst.opcode == sir::SIROpcode::SHR) &&
                                 inst.operands.size() == 2 &&
                                 inst.operands[1].type != sir::SIROperandType::CONSTANT)
                            hasVarShift = true; // (AR) مقدارُ الإزاحة غيرُ ثابتٍ ⇒ يلزمه CL/حفظُ RCX
                        else if (inst.opcode == sir::SIROpcode::ARRAY_NEW)
                            hasArrayNew = true; // (AR) mmap يدهس الحوض ⇒ انسكابٌ حولَه
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
                                // (AR) العشريّ أو المعلَّب (Any، قد يُبوَّب عشريًّا) ⇒ يلزمه المُنسِّق + خانتُه.
                                if (op.dataType == types::SadTypeKind::Float ||
                                    op.dataType == types::SadTypeKind::Any)
                                    hasFloatPrint = true;
                            }
                        }
                    }
                // (AR) القيمُ العابرةُ للكتل (تُعرَّف في كتلةٍ وتُقرأ في أخرى، عدا نتائجَ PHI ومتغيّراتِ
                //      ALLOC والسلاسلِ الحرفيّة): تُنسَك عند تعريفها في خانةِ إطارٍ. مُمرَّرٌ أوّلٌ يسجّل
                //      كتلةَ التعريفِ لكلّ ناتجٍ ذي قيمةٍ سجليّة، وثانٍ يكتشف الاستعمالَ في كتلةٍ مغايرة
                //      (متجاهلًا معامِلاتِ PHI التي تديرها حوافُّها). كلٌّ يُخصَّص خانةً هنا (نفسُ مساحةِ used).
                {
                    std::map<std::string, const sir::SIRBasicBlock *> defBlk;
                    for (const auto &blockPtr : blocks)
                        for (const auto &inst : blockPtr->instructions)
                        {
                            if (!inst.result || inst.opcode == sir::SIROpcode::ALLOC ||
                                inst.opcode == sir::SIROpcode::PHI)
                                continue; // (AR) ALLOC/PHI مُدارتان بخاناتهما؛ لا نسكَ قيمةٍ لهما
                            if (inst.opcode == sir::SIROpcode::MOVE && !inst.operands.empty() &&
                                inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                                inst.operands[0].dataType == types::SadTypeKind::String)
                                continue; // (AR) سلسلةٌ حرفيّةٌ (strReg_، لا قيمةَ سجلّ)
                            defBlk[inst.result->name] = blockPtr.get();
                        }
                    for (const auto &blockPtr : blocks)
                        for (const auto &inst : blockPtr->instructions)
                            // (AR) نشمل معامِلاتِ PHI عمدًا: قيمتُها الواردةُ عابرةٌ (تُعرَّف في السَّلَف،
                            //      تُقرأ في الدمج) ⇒ نسكُها عند التعريف يجعل خزنَ الحافّة يقرؤها memSlot_-أوّلًا،
                            //      فيَحصُنُ ضدّ دهسِ نداءٍ/طباعةٍ لاحقٍ في السَّلَف (سدُّ عائق أميليا اللاتِن).
                            for (const auto &op : inst.operands)
                                if (op.type == sir::SIROperandType::REGISTER)
                                {
                                    auto it = defBlk.find(op.name);
                                    if (it != defBlk.end() && it->second != blockPtr.get())
                                        crossBlockSpill_.insert(op.name);
                                }
                    for (const auto &name : crossBlockSpill_)
                        if (memSlot_.find(name) == memSlot_.end())
                        {
                            used += 8;
                            memSlot_[name] = -used;
                        }
                }
                // (AR) القسمةُ الصحيحةُ (idiv) تدهس rdx (=أوّلُ سجلّ حوض، قد يحمل مؤقّتًا حيًّا)؛
                //      احجز خانةَ خدشٍ نحفظ فيها rdx حولَ التسلسل cqo/idiv ونعيده بعده.
                if (hasIdiv)
                {
                    used += 8;
                    idivScratchDisp_ = -used;
                }
                // (AR) الإزاحةُ المتغيّرة تلزمها CL ⇒ احجز خانةَ حفظِ RCX (قد يحمل مؤقّتًا حيًّا).
                if (hasVarShift)
                {
                    used += 8;
                    shiftScratchDisp_ = -used;
                }
                // (AR) طباعةُ عددٍ: احجز مخزنَ itoa (٢٤ بايتًا تكفي ٢٠ رقمًا لـi64 + هامش).
                //      القمّةُ (العنوانُ الأعلى، حصريّ) = ‎-used‎ قبل الحجز؛ الأرقامُ تُبنى تنازليًّا.
                if (hasNumberPrint)
                {
                    printBufTopDisp_ = -used;
                    used += 24;
                }
                // (AR) طباعةُ عشريّ: خانةُ خدشٍ (٨ بايت) لنمطِ بتّاتِ الـdouble عبر مراحل المُنسِّق.
                if (hasFloatPrint)
                {
                    used += 8;
                    floatValDisp_ = -used;
                }
                // (AR) الإلحاق: خمسُ خاناتٍ تبقى حيّةً عبر mmap (P/القيمة/الطول/newdata/newcap).
                if (hasAppend)
                {
                    used += 8; appendPDisp_ = -used;
                    used += 8; appendValDisp_ = -used;
                    used += 8; appendLenDisp_ = -used;
                    used += 8; appendNewDisp_ = -used;
                    used += 8; appendCapDisp_ = -used;
                }
                // (AR) التعليب: خانةُ dyn (١٦ بايت: tag@0، payload@8) لكلّ قراءةٍ معلَّبة. القمّةُ
                //      (العنوانُ الأعلى، حصريّ) = ‎-used‎ قبل الحجز؛ الخانةُ i عند ‎base - i×16‎.
                if (dynGetCount_ > 0)
                {
                    dynSlotBaseDisp_ = -used;
                    used += static_cast<long long>(dynGetCount_) * 16;
                }
                // (AR) إن نادت الدالّةُ أو طبعت، احجز منطقةَ انسكابٍ: خانةٌ لكلّ سجلّ حوض. النداءُ
                //      يدهس كلَّ الحوض (caller-saved)، والطباعةُ تستعمل سجلّاتِ الحوض مُبدَّداتٍ ⇒
                //      تُنسَك المؤقّتاتُ الحيّةُ حولَهما وتُعاد.
                if (hasCall || hasPrint || hasArrayNew || hasAppend || hasBoxing)
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
                if (!emit(x86::mnem::kPush, "r64", {x86::Operand::R(x86::RBP)}) ||
                    !movReg(x86::RBP, x86::RSP) ||
                    !subImm(x86::RSP, frameSize_))
                    return false;
                const auto &params = fn.getParameters();
                for (size_t i = 0; i < params.size(); ++i)
                    if (!storeMem(memSlot_[diag::kVregSigil + params[i].name], abiArg_[i]))
                        return false;
                return true;
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
                    // (AR) MOVE %r = سلسلةٌ حرفيّة: لا سجلَّ فيزيائيًّا (السلاسلُ بياناتٌ لا قيمُ
                    //      سجلّات)؛ نسجّل المحتوى ليُطبَعَ حرفيًّا، ولا نُصدِر شيفرة.
                    if (inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                        inst.operands[0].dataType == types::SadTypeKind::String)
                    {
                        strReg_[inst.result->name] = inst.operands[0].name;
                        return true;
                    }
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
                    if (common::isConstInt(b, bc)) // (AR) فوريّ ⇒ add/sub r64,imm32
                        return inst.opcode == OP::ADD_I64 ? addImm(dst, bc) : subImm(dst, bc);
                    // (AR) معاملٌ ثانٍ سجليّ/ذاكرة: حمّله في المُبدَّد RAX ثمّ اجمع/اطرح بالسجلّ.
                    if (!loadInto(x86::RAX, b))
                        return false;
                    return inst.opcode == OP::ADD_I64 ? addReg(dst, x86::RAX) : subReg(dst, x86::RAX);
                }
                case OP::MUL_I64:
                {
                    // (AR) %dst = a × b ⇒ حمّل a في dst وb في المُبدَّد RAX ثمّ imul dst,RAX.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(dst, inst.operands[0]) &&
                           loadInto(x86::RAX, inst.operands[1]) &&
                           imulReg(dst, x86::RAX);
                }
                case OP::ADD_F64:
                case OP::SUB_F64:
                case OP::MUL_F64:
                case OP::DIV_F64:
                {
                    // (AR) حسابٌ عشريّ: العشريّ بتّاتُ i64 في GPR. حمّل a في dst وb في RAX، عبّئهما
                    //      في xmm0/xmm1، نفّذ عمليّةَ SSE المزدوجة، ثمّ استخرج البتّات إلى dst.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (!loadInto(dst, inst.operands[0]) || !loadInto(x86::RAX, inst.operands[1]))
                        return false;
                    if (!movqToXmm(kXmm0, dst) || !movqToXmm(kXmm1, x86::RAX))
                        return false;
                    bool ok = inst.opcode == OP::ADD_F64   ? addsd(kXmm0, kXmm1)
                              : inst.opcode == OP::SUB_F64 ? subsd(kXmm0, kXmm1)
                              : inst.opcode == OP::MUL_F64 ? mulsd(kXmm0, kXmm1)
                                                          : divsd(kXmm0, kXmm1);
                    return ok && movqFromXmm(dst, kXmm0);
                }
                case OP::I64_TO_F64:
                {
                    // (AR) صحيح ⇒ عشريّ: cvtsi2sd xmm0, dst(=الصحيح) ثمّ استخرج بتّاتِ العشريّ إلى dst.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(dst, inst.operands[0]) && cvtsi2sd(kXmm0, dst) &&
                           movqFromXmm(dst, kXmm0);
                }
                case OP::F64_TO_I64:
                {
                    // (AR) عشريّ ⇒ صحيح باقتطاعٍ نحو الصفر: عبّئ بتّاتِ dst في xmm0 ثمّ cvttsd2si dst, xmm0.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(dst, inst.operands[0]) && movqToXmm(kXmm0, dst) &&
                           cvttsd2si(dst, kXmm0);
                }
                case OP::PHI:
                {
                    // (AR) PHI: لا شيفرةَ تُصدَر — القيمةُ تعيش في خانةِ الإطار (حُجِزت في المسح المسبق).
                    //      السَّلَفُ خزّنها عند حافّته (emitPhiEdgeStores)، والقارئُ يحمّلها memSlot_-أوّلًا.
                    //      يفتح لولبَ المدى ذا الاتّجاه الديناميّ (أوّلُ باعثٍ أماميٍّ لـPHI).
                    if (!inst.result || memSlot_.find(inst.result->name) == memSlot_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kPhiUnslotted + detailOpcode(inst));
                    return true;
                }
                case OP::NULL_ASSERT: // (AR) تأكيدُ عدمٍ مؤكَّد T؟→T: هويّةٌ (حارسُ العدمِ زمنَ التشغيل مستقلّ).
                case OP::BOOL_TO_I64: // (AR) منطقيّ (٠/١ في GPR أصلًا) ⇒ صحيح: هويّة.
                case OP::CAST:        // (AR) تحويلٌ عامّ بلا تغييرِ تمثيلٍ (i64→i64): هويّة.
                {
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(dst, inst.operands[0]);
                }
                case OP::I64_TO_BOOL:
                {
                    // (AR) صحيح ⇒ منطقيّ (٠/١): test rax,rax؛ setne AL (غيرُ صفرٍ ⇒ ١)؛ movzx dst,AL.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(x86::RAX, inst.operands[0]) && testReg(x86::RAX, x86::RAX) &&
                           setccReg(x86::mnem::kSetne, x86::RAX) && movzxReg(dst, x86::RAX);
                }
                case OP::ADDR:
                {
                    // (AR) عنوانُ متغيّرِ إطارٍ (ALLOC/معامل): lea dst,[rbp+إزاحة]. عنوانُ مؤشّرِ حوضٍ غيرُ مدعوم.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long disp;
                    if (!isMemVar(inst.operands[0], disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kAddrNonslot + detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return leaMem(dst, disp);
                }
                case OP::ROL:
                {
                    // (AR) دورانٌ يسارًا (يتيمٌ سطحيًّا؛ صحيحٌ للاكتمال): مقدارٌ ثابت ⇒ rol dst,imm8.
                    //      المتغيّرُ غيرُ مدعوم (نادرٌ وميّت). n∈[0,63].
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long n;
                    if (!common::isConstInt(inst.operands[1], n))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kRolVar + detailOpcode(inst));
                    if (n < 0 || n > 63)
                        return fail(EC::INT_NATIVE_IMM_RANGE, diag::kShift + std::to_string(n));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(dst, inst.operands[0]) && rolImm(dst, n);
                }
                case OP::AND:
                case OP::OR:
                case OP::XOR:
                {
                    // (AR) %dst = a <بتّيّ> b ⇒ حمّل a في dst وb في المُبدَّد RAX ثمّ العمليّة بالسجلّ.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (!loadInto(dst, inst.operands[0]) || !loadInto(x86::RAX, inst.operands[1]))
                        return false;
                    return inst.opcode == OP::AND ? andReg(dst, x86::RAX)
                         : inst.opcode == OP::OR  ? orReg(dst, x86::RAX)
                                                  : xorReg(dst, x86::RAX);
                }
                case OP::NOT:
                case OP::NEG:
                {
                    // (AR) أحاديّ: %dst = ~a (NOT) أو −a (NEG) ⇒ حمّل a في dst ثمّ التعليمةُ الأحاديّة.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(dst, inst.operands[0]) &&
                           (inst.opcode == OP::NOT ? notReg(dst) : negReg(dst));
                }
                case OP::SHL:
                case OP::SHR:
                case OP::SAR:
                {
                    // (AR) %dst = a <<|>>|>>حسابيّ n. المقدارُ الثابتُ ⇒ shl/shr/sar dst,imm8 مباشرة.
                    //      المتغيّرُ (سجلّ/ذاكرة) يلزمه CL: نمرّر القيمةَ عبر RAX ونحفظ/نعيد RCX حولَ
                    //      الإزاحة (خانةُ خدشٍ shiftScratchDisp_) ⇒ لا يُدهَس مؤقّتٌ حيٌّ في RCX. SHR
                    //      منطقيّة (تحشو أصفارًا)، SAR حسابيّة (تمدّ الإشارة).
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) الإزاحةُ اليمنى الحسابيّة إن SAR صراحةً، أو SHR على معاملٍ **موقَّع** (Integer):
                    //      `>>` عامٌّ إشارتُه من نوعِ المعامل الأيسر (طبيعي64/بايت⇒منطقيّة، غيرُها⇒حسابيّة)
                    //      مطابقةً للمفسّر ومسارِ LLVM (arith_cmp.cpp) — `-16>>2=-4` لا قيمةً ضخمة.
                    //      (LLVM يقصر المنطقيّةَ على طبيعي64 وحده، لكنّ الأنواعَ الفرعيّةَ اللا-موقَّعة
                    //      <٦٤ ممدَّدةٌ بالصفر ⇒ بتُّ الإشارة ٠ ⇒ sar==shr لها؛ فالمحمولان متكافئان رصدًا.)
                    const bool arithRight =
                        inst.opcode == OP::SAR ||
                        (inst.opcode == OP::SHR && !isUnsignedType(inst.operands[0].dataType));
                    long long n;
                    if (common::isConstInt(inst.operands[1], n)) // (AR) مقدارٌ ثابت
                    {
                        if (n < 0 || n > 63)
                            return fail(EC::INT_NATIVE_IMM_RANGE, diag::kShift + std::to_string(n));
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return loadInto(dst, inst.operands[0]) &&
                               (inst.opcode == OP::SHL ? shlImm(dst, n)
                                : arithRight           ? sarImm(dst, n)
                                                       : shrImm(dst, n));
                    }
                    // (AR) مقدارٌ متغيّر: الإزاحةُ تلزمها CL. نحمّل المعامِلَين في مُبدَّدَين (RAX=القيمة،
                    //      RDI=العدّاد) **قبل** لمسِ RCX (فلو كان أحدُهما في RCX قرأناه صحيحًا أوّلًا)،
                    //      نحفظ RCX (قد يحمل مؤقّتًا حيًّا) في خانةِ خدشٍ، CL=RDI، نُزيح RAX، نعيد RCX،
                    //      ثمّ نخصّص dst وننقل النتيجةَ (يعمل حتّى لو dst==RCX إذ يُخصَّص بعد الاستعادة).
                    if (!loadInto(x86::RAX, inst.operands[0]) || !loadInto(x86::RDI, inst.operands[1]))
                        return false;
                    if (!storeMem(shiftScratchDisp_, x86::RCX) || !movReg(x86::RCX, x86::RDI))
                        return false;
                    if (!(inst.opcode == OP::SHL ? shlCl(x86::RAX)
                          : arithRight           ? sarCl(x86::RAX)
                                                 : shrCl(x86::RAX)))
                        return false;
                    if (!loadMem(x86::RCX, shiftScratchDisp_))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::EQ:
                case OP::NE:
                case OP::LT:
                case OP::LE:
                case OP::GT:
                case OP::GE:
                {
                    // (AR) المقارنةُ كقيمة (غيرُ مدموجةٍ في فرع): cmp ثمّ setcc AL (٠/١) ثمّ movzx dst,AL.
                    //      المدموجةُ في BR_COND تُتخطّى في lowerBlock فلا تصل هنا. RAX مُبدَّد.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) مقارنةُ عوائم (أحدُ المعامِلَين Float): ucomisd + setcc لا-موقَّع بدلالة IEEE.
                    if (common::isFloatCompare(inst))
                    {
                        int fdst;
                        if (!allocReg(inst.result->name, fdst))
                            return false;
                        return floatCompareToReg(fdst, inst.operands[0], inst.operands[1], inst.opcode);
                    }
                    if (!rejectUnsignedCmp(inst, diag::kCmpValue)) // (AR) لا-موقَّع ⇒ فشلٌ صريح (اتّساقًا مع ARM64)
                        return false;
                    const std::string *setcc = setccForCmp(inst.opcode);
                    if (!setcc)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadInto(x86::RAX, inst.operands[0]) && // (AR) الطرفُ الأيسر في RAX
                           cmpAgainst(inst.operands[1]) &&         // (AR) قارنه بالأيمن (imm/سجلّ)
                           setccReg(*setcc, x86::RAX) &&           // (AR) AL = ٠/١
                           movzxReg(dst, x86::RAX);                // (AR) dst = تمديدُ AL بالصفر
                }
                case OP::MOD_I64:
                case OP::DIV_I64:       // (AR) قسمةٌ صحيحةٌ (حاصلُ idiv نحوَ الصفر)؛ يتيمةٌ سطحيًّا (`/`⇒FLOOR_DIV_I64).
                case OP::FLOOR_DIV_I64:
                {
                    // (AR) قسمةٌ صحيحةٌ موقَّعة: المقسومُ في rax، cqo يمدّ الإشارةَ إلى rdx:rax،
                    //      idiv بمقسومٍ عليه في مُبدَّدٍ خارجَ الحوض (RDI) ⇒ rax=حاصل، rdx=باقٍ.
                    //      rdx (=pool[0]) قد يحمل مؤقّتًا حيًّا لتعليماتٍ لاحقة، فنحفظه في خانةِ
                    //      خدشِ القسمة ونعيده بعد نقلِ النتيجة ⇒ لا مؤقّتٌ يُدهَس. الحاصلُ للقسمة،
                    //      الباقي للباقي.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    if (!loadInto(x86::RAX, inst.operands[0]) || !loadInto(x86::RDI, inst.operands[1]))
                        return false;
                    if (!storeMem(idivScratchDisp_, x86::RDX)) // (AR) احفظ rdx (مؤقّتٌ حيٌّ محتمَل)
                        return false;
                    if (!cqo() || !idivReg(x86::RDI))
                        return false;
                    const int resultReg = (inst.opcode == OP::MOD_I64) ? x86::RDX : x86::RAX;
                    if (!movReg(dst, resultReg)) // (AR) الباقي/الحاصل إلى وجهته
                        return false;
                    // (AR) أعِد rdx فقط إن لم تكن هي وجهةَ النتيجة: حين dst==RDX تسكن النتيجةُ
                    //      rdx (سجلٌّ خُصِّص للتوّ، لا مؤقّتَ حيًّا سابقًا فيه) فاستعادةُ rdx تدهسها.
                    if (dst == x86::RDX)
                        return true;
                    return loadMem(x86::RDX, idivScratchDisp_); // (AR) أعِد rdx (مؤقّتٌ حيٌّ محتمَل)
                }
                case OP::ALLOC:
                {
                    // (AR) الخانةُ خُصِّصت في المسح المسبق؛ لا شيفرةَ تُصدَر (العنوان ضمنيٌّ [rbp−إزاحة]).
                    if (!inst.result || memSlot_.find(inst.result->name) == memSlot_.end())
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kAllocUnslotted + detailOpcode(inst));
                    return true;
                }
                case OP::LOAD:
                {
                    // (AR) %dst = load %slot ⇒ mov dst, [rbp+إزاحة].
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long disp;
                    if (!isMemVar(inst.operands[0], disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kLoadNonslot + detailOpcode(inst));
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
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kStoreNonslot + detailOpcode(inst));
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
                        return emit(x86::mnem::kSyscall, "", {});
                    }
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    return emitEpilogue();
                }
                case OP::RET_VOID:
                {
                    // (AR) إرجاعٌ فارغ (دالّةٌ/إجراءٌ بلا قيمة): الداخلة تخرج بـexit(0)؛ غيرُها خاتمةٌ + ret
                    //      بلا ضبطِ RAX (لا قيمةَ إرجاع). يفتح كلَّ الدوالّ العديمة الإرجاع (٢٢ باعثًا).
                    if (curIsEntry_)
                        return movImm(x86::RDI, 0) && movImm(x86::RAX, kSysExitX86) &&
                               emit(x86::mnem::kSyscall, "", {});
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
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArgsGt6 + std::to_string(argc));
                    // (AR) النداءُ يدهس كلَّ سجلّات الحوض (caller-saved في SysV). لحفظِ المؤقّتات
                    //      الحيّة عبره: (١) انسكِبْ ما يلزم إلى خانات الانسكاب؛ (٢) حمّل الوسائطَ
                    //      في سجلّات SysV — المؤقّتُ من خانة انسكابه لا من سجلّه ⇒ صفر تصادمٍ (نقلٌ
                    //      متوازٍ)؛ (٣) نادِ؛ (٤) أعِد تحميلَ المؤقّتات الحيّة؛ (٥) النتيجةُ في rax.
                    //      🔑 تقليمُ الانسكاب (تحليلُ حياةٍ): نَنسِك مؤقّتًا فقط إن كان حيًّا بعد
                    //      النداء (يُقرأ لاحقًا في الكتلة) أو وسيطًا سجليًّا لهذا النداء (يُقرأ من
                    //      خانته). الميتُ غيرُ الوسيط لا يُنسَك — سدُّ دَينِ الانسكاب التحفّظيّ.
                    for (const auto &kv : regOf_)
                    {
                        const bool live = common::usedAfterInBlock(block, instIdx, kv.first);
                        if (live || common::isPoolArgOfCall(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    }
                    for (size_t i = 0; i < argc; ++i)
                        if (!loadArgInto(abiArg_[i], inst.operands[i + 1]))
                            return false;
                    if (!emitCall(inst.operands[0].name))
                        return false;
                    // (AR) أعِد تحميلَ المؤقّتات الحيّة بعد النداء فقط (الميتُ/الوسيطُ الفاني لا يُعاد).
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
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
                case OP::BUILTIN_PRINT:
                {
                    // (AR) اطبع(معاملات…): لكلّ معاملٍ سلسلةً حرفيّةً ⇒ write مباشر؛ وعددًا ⇒ itoa.
                    //      الطباعةُ تُبدِّد سجلّاتِ الحوض (rsi/rdx/rcx/r10…)، فنَنسِك أوّلًا المؤقّتاتِ
                    //      الحيّةَ بعدها + المؤقّتاتِ المعاملاتِ لهذه الطباعة (تُقرأ من خاناتها)،
                    //      ونعيد الحيّةَ بعدها. تقليمُ الحياة يمنع نسكَ الميت.
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) || common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    for (const auto &op : inst.operands)
                    {
                        // (AR) سلسلةٌ حرفيّةٌ مباشرة (مثل «\n») ⇒ اطبعها.
                        if (op.type == sir::SIROperandType::CONSTANT &&
                            op.dataType == types::SadTypeKind::String)
                        {
                            if (!emitPrintString(op.name))
                                return false;
                        }
                        // (AR) سجلٌّ عُرِّف بسلسلةٍ حرفيّة (النمطُ المعتاد: MOVE %r=نص ثمّ اطبع %r).
                        else if (op.type == sir::SIROperandType::REGISTER &&
                                 strReg_.find(op.name) != strReg_.end())
                        {
                            if (!emitPrintString(strReg_[op.name]))
                                return false;
                        }
                        // (AR) سلسلةٌ محسوبةٌ في سجلٍّ (لا حرفيّة) غيرُ مدعومةٍ بعد ⇒ فشلٌ صريح.
                        else if (op.dataType == types::SadTypeKind::String)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kPrintStrComputed + diag::kVregSigil + op.name);
                        else if (op.dataType == types::SadTypeKind::Any)
                        {
                            // (AR) معلَّب: المعاملُ مؤشّرٌ إلى خانةِ dyn ⇒ طباعةٌ مبوَّبةٌ زمنَ التشغيل.
                            if (!loadArgInto(x86::RDI, op) || !emitPrintBoxed(x86::RDI))
                                return false;
                        }
                        else if (op.dataType == types::SadTypeKind::Float)
                        {
                            // (AR) عشريّ: حمّل نمطَ بتّاته في RAX ثمّ المُنسِّق (fixed6 + حذفُ الأصفار).
                            if (!loadArgInto(x86::RAX, op) || !emitPrintFloat())
                                return false;
                        }
                        else
                        {
                            // (AR) عددٌ صحيح: حمّله في RAX (ثابت/ذاكرة/خانة انسكاب) ثمّ itoa+write.
                            if (!loadArgInto(x86::RAX, op) || !emitPrintInt())
                                return false;
                        }
                    }
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    return true;
                }
                case OP::ARRAY_NEW:
                {
                    // (AR) result = مصفوفةٌ جديدة؛ operands=[len(const), cap(const)]. نخصّص عبر
                    //      mmap كتلةً واحدة (رأسٌ ٤٠ + بياناتٌ cap×٨)، ونهيّئ len/cap/data؛
                    //      tags/homogKind يُصفّرهما mmap (المتجانسةُ العدديّة). السعةُ ثابتةٌ
                    //      من الأمام؛ الحجمُ الديناميّ مؤجَّلٌ ⇒ فشلٌ صريح.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long lenv, capv;
                    if (!common::isConstInt(inst.operands[0], lenv) ||
                        !common::isConstInt(inst.operands[1], capv))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayNewDynamicSize);
                    if (lenv < 0 || capv < 0)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayNewNegative);
                    // (AR) نُخصّص أيضًا مخزنَ الوسوم (cap بايت) بعد البيانات في نفس الكتلة، ونضبط
                    //      arr[tags]=data_end. هذا يُغني عن التخصيصِ الكسول وسطَ ARRAY_SET (الذي يلزمه
                    //      mmap يدهس الحوض). المسارُ غيرُ المعلَّب لا يقرأ arr[tags] فلا يتأثّر. MAP_ANONYMOUS
                    //      يُصفّر مخزنَ الوسوم ⇒ الوسمُ الابتدائيّ Null=0 (يُكتَب فعليًّا عند SET المعلَّب).
                    const long long total = kArrHeaderBytes + capv * kArrSlotBytes + capv * kTagSlotBytes;
                    // (AR) انسكِبْ المؤقّتاتِ الحيّةَ (mmap يدهس الحوض)، ثمّ mmap، ثمّ أعِدها.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!emitMmap(total))
                        return false;
                    // (AR) RAX = المؤشّر. RDI خدشٌ: len ثمّ cap ثمّ data=RAX+40.
                    if (!movImm(x86::RDI, lenv) || !storeMemBase(x86::RAX, kArrOffLen, x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, capv) || !storeMemBase(x86::RAX, kArrOffCap, x86::RDI))
                        return false;
                    if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, kArrHeaderBytes) ||
                        !storeMemBase(x86::RAX, kArrOffData, x86::RDI))
                        return false;
                    // (AR) tags = نهايةُ البيانات = base + 40 + cap×8 ⇒ arr[tags] = هذا العنوان.
                    if (!movReg(x86::RDI, x86::RAX) ||
                        !addImm(x86::RDI, kArrHeaderBytes + capv * kArrSlotBytes) ||
                        !storeMemBase(x86::RAX, kArrOffTags, x86::RDI))
                        return false;
                    // (AR) أعِد تحميلَ المؤقّتاتِ الحيّة (RAX لا يُعاد كتابتُه — يحمل المؤشّرَ).
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    // (AR) خصّص سجلَّ النتيجة الآن (بعد mmap ⇒ خارجَ ما يدهسه) وانقل إليه المؤشّر.
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::ARRAY_SET:
                {
                    // (AR) operands=[arr(reg,Array), index(const/reg), value(reg)]. نحسب عنوانَ
                    //      العنصر في RAX ثمّ نخزّن القيمةَ فيه. المعلَّب (elementType=Any) مؤجَّل ⇒ فشلٌ صريح.
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (inst.operands[0].elementType == types::SadTypeKind::Any)
                    {
                        // (AR) تخزينٌ معلَّب: نخزّن الحمولةَ الخام i64 في data[idx] (العشريّ بتّاتُه،
                        //      الصحيح/المنطقيّ قيمتُه)، والوسمَ (DynKind) في tags[idx] (خانةٌ ٨-بت).
                        //      القراءةُ (ARRAY_GET Any) تُبوِّب زمنَ التشغيل من الوسم. الوسمُ ثابتٌ من
                        //      نوعِ القيمة. نستعمل الحوضَ خدشًا (RCX/RDX/RSI مُنسكَبةٌ حولَه).
                        // (AR) النصُّ المعلَّب: الوسمُ Str، والحمولةُ عنوانُ واصفٍ في rodata (لا قيمةٌ
                        //      خام) ⇒ يُعالَج خصّيصًا. غيرُه (صحيح/عشريّ/منطقيّ/عدم) حمولتُه i64 خام.
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
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                    return false;
                        // RAX=arr، RDI=idx، فحصُ الحدّ، ثمّ RCX=الحمولة (النصّ: عنوانُ الواصف؛ غيرُه: قيمةٌ خام).
                        if (!loadInto(x86::RAX, inst.operands[0]) || !loadInto(x86::RDI, inst.operands[1]) ||
                            !emitBoundsCheck())
                            return false;
                        if (isStr ? !emitLoadStrDescAddr(x86::RCX, strContent)
                                  : !loadInto(x86::RCX, inst.operands[2]))
                            return false;
                        // data[idx] = RCX: RDX=[arr+data]، RSI=idx×8، RDX+=RSI، [RDX]=RCX.
                        if (!loadMemBase(x86::RDX, x86::RAX, kArrOffData) || !movReg(x86::RSI, x86::RDI) ||
                            !shlImm(x86::RSI, 3) || !addReg(x86::RDX, x86::RSI) ||
                            !storeMemBase(x86::RDX, 0, x86::RCX))
                            return false;
                        // tags[idx] = tag: RDX=[arr+tags]، RSI=idx×8، RDX+=RSI، RCX=tag، [RDX]=RCX.
                        if (!loadMemBase(x86::RDX, x86::RAX, kArrOffTags) || !movReg(x86::RSI, x86::RDI) ||
                            !shlImm(x86::RSI, 3) || !addReg(x86::RDX, x86::RSI) ||
                            !movImm(x86::RCX, tag) || !storeMemBase(x86::RDX, 0, x86::RCX))
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                    return false;
                        return true;
                    }
                    if (!emitElemAddr(inst.operands[0], inst.operands[1]))
                        return false;
                    // (AR) RDI = القيمة (بعد حساب العنوان ⇒ الفهرسُ في RDI لم يعد مطلوبًا)؛ [RAX]=RDI.
                    if (!loadInto(x86::RDI, inst.operands[2]))
                        return false;
                    return storeMemBase(x86::RAX, 0, x86::RDI);
                }
                case OP::ARRAY_GET:
                {
                    // (AR) result = arr[index]. نحسب عنوانَ العنصر في RAX ثمّ نحمّله في الوجهة.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) القراءةُ المعلَّبة تُكشَف بنوعِ **النتيجة** Any (الأمامُ يضعها هناك لا على
                    //      المعامل — بخلاف SET) اتّساقًا مع مسارِ LLVM (array_ops.cpp).
                    if (inst.result->dataType == types::SadTypeKind::Any)
                    {
                        // (AR) قراءةٌ معلَّبة: نقرأ الحمولةَ الخام (data[idx]) والوسمَ (tags[idx])، ثمّ
                        //      نبني قيمةً ديناميّةً في خانةِ dyn ({tag@0، payload@8}) ونُعيد مؤشّرَها في
                        //      سجلّ النتيجة (نوعُه Any ⇒ الطباعةُ تُبوِّب زمنَ التشغيل). خانةٌ لكلّ قراءة.
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                    return false;
                        if (!loadInto(x86::RAX, inst.operands[0]) || !loadInto(x86::RDI, inst.operands[1]) ||
                            !emitBoundsCheck())
                            return false;
                        // RCX = data[idx] (الحمولة)
                        if (!loadMemBase(x86::RDX, x86::RAX, kArrOffData) || !movReg(x86::RSI, x86::RDI) ||
                            !shlImm(x86::RSI, 3) || !addReg(x86::RDX, x86::RSI) ||
                            !loadMemBase(x86::RCX, x86::RDX, 0))
                            return false;
                        // R8 = tags[idx] (الوسم)
                        if (!loadMemBase(x86::RDX, x86::RAX, kArrOffTags) || !movReg(x86::RSI, x86::RDI) ||
                            !shlImm(x86::RSI, 3) || !addReg(x86::RDX, x86::RSI) ||
                            !loadMemBase(x86::R8, x86::RDX, 0))
                            return false;
                        // خانةُ dyn i: tag@0، payload@8.
                        const long long sd = dynSlotBaseDisp_ - static_cast<long long>(dynSlotNext_ + 1) * 16;
                        ++dynSlotNext_;
                        if (!storeMem(sd, x86::R8) || !storeMem(sd + 8, x86::RCX))
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                    return false;
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return leaFrame(dst, sd); // (AR) النتيجة = مؤشّرُ خانةِ dyn
                    }
                    if (!emitElemAddr(inst.operands[0], inst.operands[1]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMemBase(dst, x86::RAX, 0);
                }
                case OP::ARRAY_LEN:
                {
                    // (AR) result = طول(arr) ⇒ [arr+0].
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMemBase(dst, x86::RAX, kArrOffLen);
                }
                case OP::BUILTIN_ARRAY_APPEND:
                case OP::ARRAY_APPEND:
                {
                    // (AR) الإلحاق: operands=[arr, value]، النتيجةُ Void. إن L<C خزّن مباشرةً؛ وإلّا
                    //      نمِّ السعةَ (mmap سعةٍ مضاعفةٍ + نسخُ L خانة + تحديثُ data/cap) ثمّ خزّن.
                    //      المعلَّب (elementType=Any) مؤجَّلٌ ⇒ فشلٌ صريح (يلزمه تعليبُ SadDyn).
                    //      L/C عدّادان غيرُ سالبين ⇒ jl (موقَّع) ≡ jb (لا-موقَّع) ⇒ صفر ترميزٍ جديد.
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (inst.operands[0].elementType == types::SadTypeKind::Any)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayAppendBoxed);
                    // (AR) انسكِبْ كلَّ مؤقّتٍ حيّ: الإلحاق يستعمل سجلّاتِ الحوض خدشًا، وmmap يدهسها.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) خزّن القيمةَ والمؤشّرَ P في الإطار (يبقيان عبر mmap)؛ RAX = P.
                    if (!loadInto(x86::RAX, inst.operands[1]) || !storeMem(appendValDisp_, x86::RAX))
                        return false;
                    if (!loadInto(x86::RAX, inst.operands[0]) || !storeMem(appendPDisp_, x86::RAX))
                        return false;
                    // (AR) L = [P+len]؛ احفظه؛ ثمّ cmp L,[P+cap] وjl إلى التخزين (إن L<C).
                    if (!loadMemBase(x86::RDI, x86::RAX, kArrOffLen) || !storeMem(appendLenDisp_, x86::RDI))
                        return false;
                    if (!cmpMemBase(x86::RDI, x86::RAX, kArrOffCap))
                        return false;
                    size_t jlToStore;
                    if (!emitJccFwd(x86::mnem::kJl, jlToStore))
                        return false;
                    // ── مسارُ النموّ: newcap = (C==0) ? 1 : C×2 ──
                    if (!loadMemBase(x86::RDI, x86::RAX, kArrOffCap) || !cmpZero(x86::RDI))
                        return false;
                    size_t jneDouble;
                    if (!emitJccFwd(x86::mnem::kJne, jneDouble))
                        return false;
                    size_t jmpHaveCap;
                    if (!movImm(x86::RDI, 1) || !emitJccFwd(x86::mnem::kJmp, jmpHaveCap)) // newcap = 1
                        return false;
                    patchFwd(jneDouble);
                    if (!shlImm(x86::RDI, 1)) // newcap = C×2
                        return false;
                    patchFwd(jmpHaveCap);
                    // (AR) احفظ newcap، احسب newsize=newcap×8 في RSI، mmap ⇒ RAX=newdata، احفظه.
                    if (!storeMem(appendCapDisp_, x86::RDI))
                        return false;
                    if (!movReg(x86::RSI, x86::RDI) || !shlImm(x86::RSI, 3) || !emitMmapPresetSize())
                        return false;
                    if (!storeMem(appendNewDisp_, x86::RAX))
                        return false;
                    // (AR) نسخُ L خانةً: RSI=olddata=[P+data]، RDX=newdata، RCX=L (عدّاد)، R8=خدش.
                    if (!loadMem(x86::RDI, appendPDisp_) || !loadMemBase(x86::RSI, x86::RDI, kArrOffData))
                        return false;
                    if (!movReg(x86::RDX, x86::RAX) || !loadMem(x86::RCX, appendLenDisp_))
                        return false;
                    const size_t copyTop = code_.size();
                    if (!cmpZero(x86::RCX))
                        return false;
                    size_t jeCopyDone;
                    if (!emitJccFwd(x86::mnem::kJe, jeCopyDone))
                        return false;
                    if (!loadMemBase(x86::R8, x86::RSI, 0) || !storeMemBase(x86::RDX, 0, x86::R8))
                        return false;
                    if (!addImm(x86::RSI, kArrSlotBytes) || !addImm(x86::RDX, kArrSlotBytes) ||
                        !subImm(x86::RCX, 1))
                        return false;
                    if (!emitJccBack(x86::mnem::kJmp, copyTop))
                        return false;
                    patchFwd(jeCopyDone);
                    // (AR) حدّث [P+data]=newdata؛ [P+cap]=newcap (P من الإطار، تُغيَّر حقولُه في مكانها).
                    if (!loadMem(x86::RDI, appendPDisp_))
                        return false;
                    if (!loadMem(x86::RAX, appendNewDisp_) || !storeMemBase(x86::RDI, kArrOffData, x86::RAX))
                        return false;
                    if (!loadMem(x86::RAX, appendCapDisp_) || !storeMemBase(x86::RDI, kArrOffCap, x86::RAX))
                        return false;
                    // ── التخزينُ المشترك: data[L] = value؛ [P+len] = L+1 ──
                    patchFwd(jlToStore);
                    if (!loadMem(x86::RAX, appendPDisp_) || !loadMemBase(x86::RDX, x86::RAX, kArrOffData))
                        return false;
                    if (!loadMem(x86::RCX, appendLenDisp_) || !shlImm(x86::RCX, 3) || !addReg(x86::RDX, x86::RCX))
                        return false;
                    if (!loadMem(x86::RSI, appendValDisp_) || !storeMemBase(x86::RDX, 0, x86::RSI))
                        return false;
                    if (!loadMem(x86::RCX, appendLenDisp_) || !addImm(x86::RCX, 1) ||
                        !storeMemBase(x86::RAX, kArrOffLen, x86::RCX))
                        return false;
                    // (AR) أعِد المؤقّتاتِ الحيّة.
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
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
                        !emit(x86::mnem::kPop, "r64", {x86::Operand::R(x86::RBP)}))
                        return false;
                }
                return emit(x86::mnem::kRet, "", {});
            }

            // (AR) يُصدر نداءً (call rel32) بإزاحةٍ نائبة، ويسجّل ترقيعًا لإزاحة الدالّة.
            bool emitCall(const std::string &funcName)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(x86::mnem::kCall, "rel32");
                if (!spec || spec->imm_op < 0 || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, std::string(x86::mnem::kCall) + diag::kFormSep + "rel32");
                const int width = spec->imm_bits / 8;
                if (!emit(x86::mnem::kCall, "rel32", {x86::Operand::I(0, spec->imm_bits)}))
                    return false;
                fixups_.push_back({code_.size() - static_cast<size_t>(width), funcName, width, true});
                return true;
            }

            // (AR) isConstInt نُقِلت إلى common (backend/native/sir_lowering_common.h) — مشتركةٌ
            //      مع مخفّض ARM64 لمنع الانجراف؛ تُستدعى بـcommon::isConstInt.

            // ── تدفّق التحكّم: تخفيضُ كتلةٍ كاملة + الفروع + الترقيع ──
            // (AR) isComparison نُقِلت إلى common — تُستدعى بـcommon::isComparison.

            // (AR) منمنمةُ القفز الشرطيّ المطابقة للمقارنة (موقَّعة): «إن صحّ الشرط اقفز لـthen».
            static const std::string *jccForCmp(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: return &x86::mnem::kJe;
                case OP::NE: return &x86::mnem::kJne;
                case OP::LT: return &x86::mnem::kJl;
                case OP::LE: return &x86::mnem::kJle;
                case OP::GT: return &x86::mnem::kJg;
                case OP::GE: return &x86::mnem::kJge;
                default: return nullptr;
                }
            }

            // (AR) هل النوعُ صحيحٌ لا-موقَّع؟ (طبيعي8/16/32/64 أو بايت). المقارناتُ المرتَّبةُ
            //      (setl/g/le/ge و jl/g/…) موقَّعةٌ، فمعاملٌ لا-موقَّعٌ يلزمه setb/seta/jb/ja؛
            //      لا نظائرَ لا-موقَّعةٍ في opcodes المقارنة بعد ⇒ نرفضه صراحةً بدل ترميزٍ خاطئٍ
            //      صامت (نظيرُ حارس ARM64، لحفظ التكافؤ عبر المسارين).
            static bool isUnsignedType(types::SadTypeKind t)
            {
                using K = types::SadTypeKind;
                return t == K::UInt8 || t == K::UInt16 || t == K::UInt32 ||
                       t == K::UInt64 || t == K::Byte;
            }
            bool rejectUnsignedCmp(const sir::SIRInstruction &cmp, const std::string &where)
            {
                for (const auto &op : cmp.operands)
                    if (isUnsignedType(op.dataType))
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    where + diag::kUnsignedSuffix +
                                        std::to_string(static_cast<int>(op.dataType)));
                return true;
            }

            // (AR) منمنمةُ setcc المطابقة للمقارنة (موقَّعة): تضبط بايتًا ٠/١ للمقارنةِ كقيمة.
            static const std::string *setccForCmp(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: return &x86::mnem::kSete;
                case OP::NE: return &x86::mnem::kSetne;
                case OP::LT: return &x86::mnem::kSetl;
                case OP::LE: return &x86::mnem::kSetle;
                case OP::GT: return &x86::mnem::kSetg;
                case OP::GE: return &x86::mnem::kSetge;
                default: return nullptr;
                }
            }

            // (AR) يُصدر قفزًا (مشروطًا أو غير مشروط) بإزاحةٍ نسبيّةٍ صفريّةٍ نائبة، ويسجّل
            //      ترقيعًا عند حقل الإزاحة. عرضُ الحقل وموضعُه يُشتقّان من مواصفة الترميز
            //      (imm_bits) لا من ثابتٍ مُرمَّز ⇒ يبقى سليمًا لو أُضيفت صيغةُ قفزٍ بعرضٍ آخر.
            bool emitJump(const std::string &mnemonic, const std::string &targetLabel)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, "rel32");
                if (!spec || spec->imm_op < 0 || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + diag::kFormSep + "rel32");
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
                if (common::isConstInt(b, c))
                {
                    if (c >= -128 && c <= 127)
                        return emit(x86::mnem::kCmp, "r64, imm8", {x86::Operand::R(x86::RAX), x86::Operand::I(c, 8)});
                    if (!checkImm32(c))
                        return false;
                    return emit(x86::mnem::kCmp, "r64, imm32", {x86::Operand::R(x86::RAX), x86::Operand::I(c, 32)});
                }
                if (b.type == sir::SIROperandType::REGISTER)
                {
                    long long disp;
                    // (AR) معامِلٌ ثانٍ في خانةِ إطارٍ (متغيّرُ ALLOC أو قيمةٌ عابرةٌ للكتل مُنسَكة):
                    //      حمّله في مُبدَّدٍ (RDI) ثمّ cmp RAX,RDI. (كان مرفوضًا؛ لزمَ لقيمِ الكتل العابرة.)
                    if (isMemVar(b, disp))
                        return loadMem(x86::RDI, disp) &&
                               emit(x86::mnem::kCmp, "r64, r64", {x86::Operand::R(x86::RAX), x86::Operand::R(x86::RDI)});
                    auto it = regOf_.find(b.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, diag::kVregSigil + b.name);
                    return emit(x86::mnem::kCmp, "r64, r64", {x86::Operand::R(x86::RAX), x86::Operand::R(it->second)});
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            diag::kCmpBKind + std::to_string(static_cast<int>(b.type)));
            }

            // (AR) القفزُ غير المشروط BR: operands[0] لصيقةُ الهدف.
            bool lowerBranch(const sir::SIRInstruction &inst)
            {
                if (inst.operands.size() != 1 || inst.operands[0].type != sir::SIROperandType::LABEL)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                return emitJump(x86::mnem::kJmp, inst.operands[0].name);
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
                                    diag::kCondConstType + std::to_string(static_cast<int>(cond.dataType)));
                    return emitJump(x86::mnem::kJmp, cond.boolValue ? thenLbl : elseLbl);
                }
                if (cond.type != sir::SIROperandType::REGISTER)
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                diag::kCondKind + std::to_string(static_cast<int>(cond.type)));

                // (AR) شرطٌ سجليّ: مقارنةٌ صحيحةٌ مدموجةٌ في هذه الكتلة (cmp؛ jCC) — الأسرع. وإلّا
                //      قيمةٌ منطقيّةٌ حيّةٌ (٠/١) عُرِّفت في هذه الكتلة (نتيجةُ مقارنةِ عوائم أو متغيّرٌ
                //      منطقيّ) ⇒ اختبرها test؛ jnz then؛ jmp else. (مقارنةُ العوائم لا تُدمَج عمدًا.)
                const sir::SIRInstruction *cmp = common::findFusedComparison(block, cond.name);
                // (AR) نتيجةٌ عابرةٌ للكتل ⇒ لا تُدمَج (تُنسَك وتُقرأ من خانتها كمنطقيٍّ حيّ). يطابق lowerBlock.
                if (cmp && cmp->result && crossBlockSpill_.count(cmp->result->name))
                    cmp = nullptr;
                if (!cmp || cmp->operands.size() != 2)
                {
                    if (!loadInto(x86::RAX, cond)) // (AR) منطقيٌّ حيٌّ من الحوض/خانةِ الإطار
                        return false;
                    return testReg(x86::RAX, x86::RAX) &&
                           emitJump(x86::mnem::kJne, thenLbl) &&
                           emitJump(x86::mnem::kJmp, elseLbl);
                }
                if (!rejectUnsignedCmp(*cmp, diag::kCmpBranch)) // (AR) لا-موقَّع ⇒ فشلٌ صريح (jl/g موقَّعة)
                    return false;
                // (AR) المعامل الأوّل في المُبدَّد RAX (يُحمَّل من الخانة إن متغيّرَ ذاكرة)، ثمّ
                //      قارنه بالثاني (ثابتٌ imm أو سجلّ). كلا المعامِلَين في الذاكرة غيرُ مدعوم.
                if (!loadInto(x86::RAX, cmp->operands[0]))
                    return false;
                if (!cmpAgainst(cmp->operands[1]))
                    return false;
                const std::string *jcc = jccForCmp(cmp->opcode);
                if (!jcc)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(*cmp));
                return emitJump(*jcc, thenLbl) && emitJump(x86::mnem::kJmp, elseLbl);
            }

            // (AR) findFusedComparison نُقِلت إلى common — تُستدعى بـcommon::findFusedComparison.

            // (AR) عند نهايةِ كتلةٍ سَلَفٍ (قبل قفزِها): خزّن كلَّ قيمةٍ واردةٍ لـPHI في خانةِ ناتجِه.
            //      القيمةُ ما تزال حيّةً في هذه الكتلة (عُرِّفت فيها)، وRAX مُبدَّد. الخانةُ تحمل
            //      القيمةَ عبر الحافّة إلى الكتلةِ الدامجة (تقرؤها memSlot_-أوّلًا بعد تنظيفِ الحوض).
            bool emitPhiEdgeStores(const std::string &predName)
            {
                auto it = phiEdges_.find(predName);
                if (it == phiEdges_.end())
                    return true;
                for (const auto &edge : it->second)
                    if (!loadInto(x86::RAX, edge.first) || !storeMem(memSlot_[edge.second], x86::RAX))
                        return false;
                return true;
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
                    fused = common::findFusedComparison(block, is.back().operands[0].name);
                // (AR) لا تُدمِج مقارنةً نتيجتُها عابرةٌ للكتل (تُقرأ في كتلةٍ أخرى): الدمجُ يتخطّى
                //      تخفيضَها المستقلَّ فيُفوَّت نسكُها ⇒ خانةٌ غيرُ مكتوبة. أبقِها مستقلّةً (تُنسَك،
                //      والفرعُ يقرؤها عبر المسار الاحتياطيّ). سدُّ عائق أميليا اللاتِن رقم ١.
                if (fused && fused->result && crossBlockSpill_.count(fused->result->name))
                    fused = nullptr;

                for (size_t idx = 0; idx < is.size(); ++idx)
                {
                    const sir::SIRInstruction &inst = is[idx];
                    // (AR) سياقُ التخصيص: يمكّن allocReg من استرجاعِ سجلٍّ ميّتٍ عند نفاد الحوض.
                    curBlock_ = &block;
                    curInstIdx_ = idx;
                    if (&inst == fused)
                        continue; // (AR) مدموجة ⇒ يعالجها BR_COND
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
                    // (AR) نسكُ قيمةٍ عابرةٍ للكتل عند تعريفها: خزّن ناتجَها في خانتِه فتُقرأ لاحقًا
                    //      (في كتلةٍ أخرى) memSlot_-أوّلًا. (نتائجُ PHI مُستثناةٌ — تديرها الحوافّ.)
                    if (inst.result && crossBlockSpill_.count(inst.result->name))
                    {
                        auto rit = regOf_.find(inst.result->name);
                        if (rit != regOf_.end() && !storeMem(memSlot_[inst.result->name], rit->second))
                            return false;
                    }
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
                        return fail(EC::INT_NATIVE_IMM_RANGE, diag::kRel + std::to_string(disp));
                    uint64_t u = static_cast<uint64_t>(disp); // (AR) متمّمُ الاثنين، LE بعرض الحقل
                    for (int i = 0; i < fx.width; ++i)
                        code_[fx.rel32Pos + i] = static_cast<uint8_t>((u >> (8 * i)) & 0xFF);
                }
                return true;
            }
        };

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
