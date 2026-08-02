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

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
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
        inline constexpr long long kAsciiNine = 0x39;  // (AR) رمزُ التسعة ASCII ('9') — حدُّ التحليل العشريّ الأعلى
        inline constexpr long long kAsciiDot = 0x2E;   // (AR) رمزُ النقطة ASCII ('.') — فاصلةُ العشريّ في عشريّ→نصّ
        inline constexpr long long kUtf8ContMask = 0xC0; // (AR) قناعُ بتّي البدايةِ العليَين في UTF-8
        inline constexpr long long kUtf8ContTag = 0x80;  // (AR) وسمُ البايتِ التابع (10xxxxxx) في UTF-8
        inline constexpr long long kItoaRadix = 10;    // (AR) أساسُ التحويل العشريّ

        // (AR) تخصيصُ الكومة الحرّة: mmap مباشرةً عبر syscall (لا libc في الخلفيّة الساكنة).
        //      addr=0 (النواةُ تختار)، prot=قراءة|كتابة، flags=خاصّ|مجهول (الذاكرةُ مُصفّرةٌ
        //      سلفًا ⇒ tags/homogKind للمصفوفة يُصفّران مجّانًا)، fd=-1، offset=0.
        inline constexpr long long kSysMmapX86 = 9;      // (AR) mmap
        inline constexpr long long kSysMunmapX86 = 11;   // (AR) munmap — لتحرير كتلةِ الكومة (FREE)
        inline constexpr long long kProtReadWrite = 0x3; // (AR) PROT_READ|PROT_WRITE
        inline constexpr long long kMapPrivAnon = 0x22;  // (AR) MAP_PRIVATE|MAP_ANONYMOUS
        inline constexpr long long kMmapNoFd = -1;       // (AR) fd للتخصيص المجهول = ‎-1‎

        // (AR) نواةُ الكومة الأصليّة (حجز/حرر): mmap لكلّ تخصيصٍ برأسِ ١٦ بايتًا خفيّ قبل ما يراه
        //      المستخدم [توقيعٌ سحريّ@0 | حجمُ mmap الكلّيّ@8]، والمؤشّرُ المُعاد = base+16 (محاذاةُ
        //      ١٦ تكفي SadArray/SSE). حرر يقرأ الرأسَ: إن طابق التوقيعَ فـmunmap(base, الحجمُ الكلّيّ)
        //      وإلّا **يتخطّى بأمان** (مؤشّرٌ أجنبيّ/مصفوفةٌ بلا رأس ⇒ لا يُفكّ منطقةً عشوائيّة).
        //      🔑 نُخزّن الحجمَ الكلّيَّ (size+16) لا حجمَ المستخدم: munmap لحجمٍ ناقصٍ يُسرّب صفحةً
        //      حين يعبر التخصيصُ حدَّ صفحة (قيدُ أميليا الإلزاميّ ١).
        inline constexpr long long kHeapHdrBytes = 16;               // (AR) رأسٌ خفيّ (توقيع + حجم)
        inline constexpr long long kHeapMagicOff = 0;                // (AR) إزاحةُ التوقيع في الرأس
        inline constexpr long long kHeapSizeOff = 8;                 // (AR) إزاحةُ الحجم الكلّيّ في الرأس
        inline constexpr long long kHeapMagic = 0x5341444845415001LL; // (AR) توقيعٌ مميّز "SADHEAP\x01"
        inline constexpr long long kStrHeapSlots = 8;   // (AR) خانات خدشِ عمليّاتِ كومةِ النصّ (تبقى عبر mmap؛ REPLACE يلزمه ٦)
        inline constexpr long long kItoaBufPayload = 24; // (AR) مخزنُ رقم→نصّ على الكومة (٢٠ رقمًا+إشارة+NUL+هامش)
        inline constexpr long long kFtoaBufPayload = 40; // (AR) مخزنُ عشريّ→نصّ (إشارة+٢٠ صحيحًا+نقطة+٦ عشريّة+NUL+هامش)

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
        inline constexpr long long kDynKindArray = 5; // (AR) مصفوفة (مؤشّرٌ مُدار) — homogKind لناتجِ ZIP (مصفوفةٌ من أزواج)

        // (AR) تخطيطُ التعداد الجبريّ (ADT، الدفعة ٥؛ يُطابق مسارَ LLVM enum_ops.cpp): قيمةُ التعداد =
        //      مؤشّرُ كومة [i64 tag@0 | SadDyn f0@8 | SadDyn f1@24 | …]. SadDyn = {i8 kind@0، i64 payload@8}
        //      = ١٦ بايت (يُمرَّر في سجلّين في LLVM؛ هنا خانةُ كومةٍ). حقلُ i: SadDyn slot @8+i×16، الحمولةُ
        //      i64 @16+i×16 (جزءُ payload). الحمولاتُ العدديّةُ kind=Int؛ float/string مؤجَّلة.
        inline constexpr long long kAdtTagOff = 0;         // (AR) إزاحةُ الوسم (i64)
        inline constexpr long long kAdtPayloadBase = 8;    // (AR) إزاحةُ أوّلِ خانةِ SadDyn
        inline constexpr long long kSadDynBytes = 16;      // (AR) حجمُ خانةِ SadDyn الواحدة
        inline constexpr long long kSadDynKindOff = 0;     // (AR) إزاحةُ الوسم داخلَ SadDyn
        inline constexpr long long kSadDynPayloadOff = 8;  // (AR) إزاحةُ الحمولة داخلَ SadDyn

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

                // (AR) ابنِ سجلَّ تخطيطِ الأصناف مرّةً (الكائنيّة): فهرسُ كلّ حقلٍ بترتيب fieldOrder_،
                //      وعلمُ allEightByte (يُطفأ عند bool/Any ⇒ تخطيطٌ غيرُ منتظم يُرفَض عند الوصول).
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
                        if (t == types::SadTypeKind::Boolean || t == types::SadTypeKind::Any)
                            cl.allEightByte = false; // (AR) i1/SadDyn ≠ ٨ بايت ⇒ تخطيطٌ غيرُ منتظم
                    }
                    cl.numFields = idx;
                    classLayout_[c->name] = cl;
                }

                // (AR) ابنِ جداولَ الدوالّ (vtables) للإرسالِ الافتراضيّ + احجزْ خاناتِها في
                //      مقدّمةِ rodata_ (قبلَ تفريدِ سلاسلِ الدوالّ) فتثبتُ إزاحاتُها. (الدفعة ٦)
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

                // (AR) المرور ٢: رقّع كلَّ rel32 (فرعٌ داخل الدالّة، أو نداءٌ بين الدوالّ).
                if (!applyFixups())
                    return finishError(r, errorCode_, detail_);

                // (AR) ألحِق كتلةَ البيانات (سلاسلُ الطباعة) بعد كلّ الشيفرة في نفس مقطع R+X،
                //      ثمّ رقّع كلَّ mov r64,imm64 بالعنوان المطلق للسلسلة. العنوانُ المطلقُ =
                //      vbase + إزاحةُ الشيفرة (الرأس) + قاعدةُ rodata داخل code_ + إزاحةُ السلسلة.
                //      يعتمد ثباتَ vbase في ET_EXEC (يُمرَّر نفسُه إلى writeStaticExec).
                if (!strFixups_.empty() || !fnPtrFixups_.empty() ||
                    !vtableAddrFixups_.empty() || !funcAddrFixups_.empty() || !rodata_.empty())
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
                    // (AR) خاناتُ جدولِ الدوالّ (في rodata، الآن جزءٌ من code_ عند rodataBase):
                    //      تُملأ بعنوانِ الدالّةِ المطلق.
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
                    // (AR) عناوينُ الجداولِ في تعليماتِ mov r64,imm64 داخلَ ALLOC-صنف.
                    for (const VtableAddrFixup &vf : vtableAddrFixups_)
                    {
                        const uint64_t vtAddr = elf::kDefaultVBase + elf::kCodeOffset +
                                                static_cast<uint64_t>(rodataBase + vtableBaseOff_[vf.className]);
                        for (int i = 0; i < 8; ++i)
                            code_[vf.imm64Pos + i] = static_cast<uint8_t>((vtAddr >> (8 * i)) & 0xFF);
                    }
                    // (AR) عناوينُ الدوالّ في تعليماتِ mov r64,imm64 داخلَ CLOSURE_CREATE (الدفعة ٧).
                    for (const FuncAddrFixup &af : funcAddrFixups_)
                    {
                        auto it = funcOffset_.find(af.fnName);
                        if (it == funcOffset_.end())
                            return finishError(r, EC::INT_NATIVE_UNSUPPORTED,
                                               diag::kFuncAddrUnresolved + af.fnName);
                        const uint64_t fnAddr = elf::kDefaultVBase + elf::kCodeOffset +
                                                static_cast<uint64_t>(it->second);
                        for (int i = 0; i < 8; ++i)
                            code_[af.imm64Pos + i] = static_cast<uint8_t>((fnAddr >> (8 * i)) & 0xFF);
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
                // (AR) استنتاجُ صنفِ الكائن يُصفَّر لكلّ دالّة؛ %self داخل «صنف.طريقة» صنفُه الجزءُ قبل النقطة.
                objClassOf_.clear();
                {
                    const auto dot = currentFn_.find('.');
                    if (dot != std::string::npos)
                        objClassOf_[diag::kVregSigil + std::string("self")] = currentFn_.substr(0, dot);
                }
                // (AR) ابذرْ صنفَ كلِّ معاملٍ مصرَّحٍ بصنفٍ مسجَّل (نحو `دالة نطق(حيوان ح)`) —
                //      لازمٌ للإرسالِ الافتراضيّ على معاملٍ (لا ALLOC/self يشير إليه). (الدفعة ٦)
                for (const auto &p : fn.getParameters())
                    if (!p.className.empty())
                        objClassOf_[diag::kVregSigil + p.name] = p.className;
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
            // (AR) تخطيطُ الأصناف (الدفعة ٥، الكائنيّة): اسمُ الصنف ⇒ {isCRepr، اسمُ الحقل⇒فهرسُه، عددُ الحقول،
            //      allEightByte}. إزاحةُ الحقل = 8×(الفهرس + (isCRepr؟0:1)) [ترويسةُ vtable@0 لغير-CRepr].
            //      يُدعَم الحقلُ ٨-بت فقط (i64/f64/ptr/نصّ)؛ bool/Any مؤجَّلان (تخطيطٌ غيرُ منتظم). يُبنى مرّةً في lowerModule.
            struct ClassLayout { bool isCRepr = false; std::map<std::string, int> fieldIndex; int numFields = 0; bool allEightByte = true; };
            std::map<std::string, ClassLayout> classLayout_;
            // (AR) استنتاجُ صنفِ الكائن (سجلّ/خانة ⇒ اسمُ الصنف): يُعيد بناءَ objectClassMap الأماميّ.
            //      %self داخل «صنف.طريقة» صنفُه الصنف؛ ALLOC"C" نتيجتُه C؛ STORE/LOAD ينشران. يُصفَّر لكلّ دالّة.
            std::map<std::string, std::string> objClassOf_;
            // (AR) إزاحةُ حقلِ كائنٍ (بايت) من صنفه: 8×(الفهرس + (isCRepr؟0:1)). تفشل إن الصنفُ مجهولٌ
            //      أو غيرُ منتظمِ التخطيط (bool/Any) أو الحقلُ غيرُ موجود.
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
            // (AR) صنفُ الكائنِ الذي يشير إليه معاملٌ (سجلّ/خانة)، أو nullptr إن مجهولًا.
            const std::string *objClassForOperand(const sir::SIROperand &op) const
            {
                auto it = objClassOf_.find(op.name);
                return it != objClassOf_.end() ? &it->second : nullptr;
            }
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

            // (AR) عمليّاتُ كومةِ النصّ (الدفعة ٣، الطبقات ٢–٥: CONCAT/I64_TO_STRING/…): منطقةُ خدشٍ من
            //      kStrHeapSlots خاناتٍ تبقى حيّةً عبر mmap الداخليّ (الذي يدهس الحوضَ): طولٌ/مخزنٌ/عدّاد.
            //      تُحجَز حين تحوي الدالّةُ أوپكودَ نصٍّ يخصّص. الخانةُ i عند strHeapBaseDisp_ + i×8.
            long long strHeapBaseDisp_ = 0;
            long long strHeapSlot(int i) const { return strHeapBaseDisp_ + static_cast<long long>(i) * 8; }

            // (AR) خاناتُ خدشِ امتدادِ المصفوفة (CONCAT/ZIP): قيمٌ (أطوالٌ/مؤشّراتُ بياناتٍ/عدّادُ
            //      لولبٍ/مؤشّرُ الناتج) تبقى حيّةً عبر mmap الداخليّ. ٦ خاناتٍ تكفي أعقدَها (ZIP).
            //      تُحجَز حين تحوي الدالّةُ CONCAT/ZIP. الخانةُ i عند arrExtBaseDisp_ + i×8.
            inline constexpr static int kArrExtSlots = 6;
            long long arrExtBaseDisp_ = 0;
            long long arrExtSlot(int i) const { return arrExtBaseDisp_ + static_cast<long long>(i) * 8; }

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

            // ── الإرسالُ الافتراضيّ (الدفعة ٦): جداولُ الدوالّ (vtables) ──────────────────
            // (AR) تخطيطُ جدولِ دوالِّ كلِّ صنفٍ غيرِ-CRepr: قائمةُ خاناتٍ {الاسمُ القصير للطريقة،
            //      اسمُ الدالّةِ المؤهَّل المنفِّذ}. يُبنى بترتيبٍ أبٌ-قبل-ابن: يُنسَخ جدولُ الأب،
            //      ثمّ تُدهَس الخانةُ عند التجاوز (نفسُ الاسمِ القصير) أو تُلحَق خانةٌ جديدة —
            //      مطابقةً لـclasses_vtables_ops.cpp. فهرسُ الخانةِ ثابتٌ عبر الأب والأبناء
            //      (التجاوزُ في المكان) ⇒ حلُّ الفهرس من الصنفِ الساكن صحيحٌ للكائنِ زمنَ التشغيل.
            std::map<std::string, std::vector<std::pair<std::string, std::string>>> classVtableLayout_;
            // (AR) اسمُ الصنف ⇒ إزاحةُ جدولِه في rodata_ (لحساب عنوانِه المطلق لتخزينِه في obj[0]).
            std::map<std::string, size_t> vtableBaseOff_;
            // (AR) ترقيعُ خانةِ جدول: إزاحةٌ في rodata_ ⇒ اسمُ الدالّة؛ تُملأ زمنَ الإنهاء بعنوانِ
            //      الدالّةِ المطلق (vbase+kCodeOffset+funcOffset_[fn]).
            struct FnPtrFixup { size_t rodataOff; std::string fnName; };
            std::vector<FnPtrFixup> fnPtrFixups_;
            // (AR) ترقيعُ عنوانِ جدول: موضعُ mov r64,imm64 في code_ (داخل ALLOC-صنف) ⇒ اسمُ الصنف؛
            //      يُملأ بعنوانِ جدولِه المطلق (vbase+kCodeOffset+rodataBase+vtableBaseOff_[class]).
            struct VtableAddrFixup { size_t imm64Pos; std::string className; };
            std::vector<VtableAddrFixup> vtableAddrFixups_;
            // (AR) الإغلاقات (الدفعة ٧): ترقيعُ عنوانِ دالّةٍ في تعليمة mov r64,imm64 داخلَ
            //      CLOSURE_CREATE ⇒ يُملأ بعنوانِ الدالّةِ المطلق (vbase+kCodeOffset+funcOffset_[fn]).
            //      مثلُ VtableAddrFixup لكن يحلُّ من funcOffset_ لا vtableBaseOff_.
            struct FuncAddrFixup { size_t imm64Pos; std::string fnName; };
            std::vector<FuncAddrFixup> funcAddrFixups_;

            // (AR) يبني تخطيطاتِ جداولِ الدوالّ لكلّ الأصنافِ غيرِ-CRepr (بترتيبٍ أبٌ-قبل-ابن)،
            //      ثمّ يحجزُ لكلّ جدولٍ خاناتِه (٨ بايت/خانة، أصفارٌ نائبة) في مقدّمةِ rodata_
            //      (قبلَ تفريدِ أيّ سلسلةٍ زمنَ تخفيضِ الدوالّ) فتثبتُ الإزاحات، ويسجّل ترقيعاتِ
            //      مؤشّراتِ الدوالّ. يُستدعى مرّةً في lowerModule بعد بناءِ classLayout_.
            void buildVtableLayouts(const sir::SIRModule &module)
            {
                classVtableLayout_.clear();
                vtableBaseOff_.clear();
                fnPtrFixups_.clear();
                vtableAddrFixups_.clear();
                funcAddrFixups_.clear();
                // (AR) فهرسةُ الأصناف بالاسم + مرورٌ طوبولوجيّ (أبٌ قبل ابن).
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
                        // (AR) جاهزٌ إن لا أبَ له أو أبوه ليس صنفَ vtable (CRepr/مجهول) أو أُنجِز أبوه.
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
                // (AR) ابنِ التخطيطات (نسخُ الأب + تجاوزٌ في المكان أو إلحاق). الأسماءُ القصيرةُ
                //      من مفتاحِ methods_ المؤهَّل (بعدَ آخرِ نقطة). البانونَ/الهادمونَ مستثنَونَ.
                for (const sir::SIRClass *c : order)
                {
                    std::vector<std::pair<std::string, std::string>> layout;
                    if (byName.count(c->parentClass) && classVtableLayout_.count(c->parentClass))
                        layout = classVtableLayout_[c->parentClass]; // (AR) نسخُ جدولِ الأب
                    // (AR) ترتيبٌ حتميّ: افرزِ الأسماءَ المؤهَّلةَ للطرائق (unordered_map).
                    std::vector<std::string> qualNames;
                    for (const auto &m : c->methods_)
                        if (m.second)
                            qualNames.push_back(m.first);
                    std::sort(qualNames.begin(), qualNames.end());
                    for (const std::string &qual : qualNames)
                    {
                        const std::string shortName = shortMethodName(qual);
                        if (isCtorOrDtorName(shortName))
                            continue; // (AR) البانونَ/الهادمونَ خارجَ الجدول (نداءٌ مباشر)
                        bool overridden = false;
                        for (auto &slot : layout)
                            if (slot.first == shortName) { slot.second = qual; overridden = true; break; }
                        if (!overridden)
                            layout.emplace_back(shortName, qual);
                    }
                    classVtableLayout_[c->name] = layout;
                }
                // (AR) احجزْ خاناتِ الجداولِ في مقدّمةِ rodata_ (أصفارٌ نائبة) وسجّلِ الترقيعات.
                for (const sir::SIRClass *c : order)
                {
                    vtableBaseOff_[c->name] = rodata_.size();
                    for (const auto &slot : classVtableLayout_[c->name])
                    {
                        fnPtrFixups_.push_back({rodata_.size(), slot.second});
                        rodata_.insert(rodata_.end(), 8, 0); // (AR) خانةٌ ٨-بت نائبةٌ لعنوانِ الدالّة
                    }
                }
            }
            // (AR) الاسمُ القصيرُ للطريقة من اسمٍ مؤهَّل «صنف.طريقة» (ما بعدَ آخرِ نقطة).
            static std::string shortMethodName(const std::string &qualified)
            {
                const auto dot = qualified.rfind('.');
                return dot == std::string::npos ? qualified : qualified.substr(dot + 1);
            }
            // (AR) هل الاسمُ القصيرُ بانٍ/هادم؟ (مستثنًى من جدولِ الدوالّ — نداءٌ مباشر).
            static bool isCtorOrDtorName(const std::string &n)
            {
                return n == "\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A" ||        // باني
                       n == "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1" ||        // بناء
                       n == "\xD9\x85\xD9\x86\xD8\xB4\xD8\xA6" ||        // منشئ
                       n == "__init__" ||
                       n == "\xD9\x87\xD8\xAF\xD9\x85" ||                // هدم
                       n == "__del__";
            }

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
            // ── تمثيلُ قيمةِ النصّ زمنَ التشغيل (الدفعة ٣): مؤشّرٌ إلى بايتات UTF-8 منتهيةٍ بـNUL ──
            //    (يطابق تمثيلَ خلفيّة LLVM: i8* منتهٍ بـNUL، الطولُ ضمنيّ). النصُّ الحرفيّ يُفرَّد في
            //    rodata منتهيًا بـNUL؛ النصُّ المحسوب (CONCAT/SUBSTR/...) مؤشّرُ كومةٍ من ALLOC_HEAP.
            //    مميَّزٌ عن الواصفِ الطوليّ للتعليب (makeStrDescriptor) الذي يُبقى مقصورًا على SadDyn.
            size_t internCStr(const std::string &s)
            {
                std::string z = s;
                z.push_back('\0'); // (AR) خاتمةُ NUL (لا تتصادم مع سلاسلِ الطباعة الخام: بايتاتٌ مغايرة)
                return internString(z);
            }
            // (AR) يحمّل العنوانَ المطلق لسلسلةٍ منتهيةٍ بـNUL في سجلّ (mov r64,imm64 نائبٌ يُرقَّع كأيّ
            //      سلسلةِ rodata). يُستعمَل لتجسيدِ حرفيّةِ النصّ قيمةً زمنَ تشغيلٍ (STRING_NEW/BOOL_TO_STRING).
            bool emitLoadCStrAddr(int reg, const std::string &content)
            {
                const size_t off = internCStr(content);
                if (!emit(x86::mnem::kMov, "r64, imm64", {x86::Operand::R(reg), x86::Operand::I(0, 64)}))
                    return false;
                strFixups_.push_back({code_.size() - 8, off});
                return true;
            }
            // (AR) يُجسّد قيمةَ نصٍّ في السجلّ reg مؤشّرًا زمنَ تشغيلٍ منتهيًا بـNUL: حرفيّةٌ (ثابتٌ نصّيّ
            //      أو سجلٌّ شبحيٌّ في strReg_) ⇒ عنوانُ rodata؛ سجلٌّ حيٌّ (مؤشّرُ كومةٍ من عمليّةٍ سابقة)
            //      ⇒ تحميلٌ مباشر. fromSpill: داخلَ منطقةِ انسكابٍ يُقرأ السجلُّ الحيُّ من خانتِه لا من ذاته.
            bool materializeString(const sir::SIROperand &op, int reg, bool fromSpill)
            {
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::String)
                    return emitLoadCStrAddr(reg, op.name);
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    auto it = strReg_.find(op.name);
                    if (it != strReg_.end())
                        return emitLoadCStrAddr(reg, it->second); // (AR) حرفيّةٌ شبحيّةٌ (لا سجلَّ لها)
                    return fromSpill ? loadArgInto(reg, op) : loadInto(reg, op);
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
            static constexpr int kXmm2 = 2; // (AR) سجلّ عشريّ خدشٌ ٢ (متراكمُ نصّ→عشريّ)
            static constexpr int kXmm3 = 3; // (AR) سجلّ عشريّ خدشٌ ٣ (ثابتُ العشرة / مقسومُ الكسر)
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
            // (AR) mov dstLow8, [ptrReg] — تحميلُ بايتٍ واحد (8A /r) إلى أدنى ٨ بتّاتٍ من dstReg
            //      (لا يُصفّر الأعلى؛ لكنّ storeByte يكتب الأدنى فقط ⇒ لا يضرّ). لحلقةِ نسخِ الذاكرة.
            bool loadByte(int dstReg, int ptrReg)
            {
                return emit(x86::mnem::kMov, "r8, m8", {x86::Operand::R(dstReg), x86::Operand::M(ptrReg, 0)});
            }
            // (AR) تحميلُ بايتٍ من الذاكرة مع تمديدِ الصفر إلى ٦٤-بت في تعليمةٍ واحدة (movzx r64,m8،
            //      REX.W + 0F B6 /r). لازمٌ حين تُقارَن/تُحسَب القيمةُ الكاملةُ (مسحُ النصّ): (١) `mov r8`
            //      لا يمسح أعلى ٥٦ بتًّا (بخلاف ldrb في ARM64 الذي يمدّد بالصفر ذاتيًّا)؛ (٢) الأهمّ:
            //      `mov r8` بلا REX يُفسّر الوجهةَ ٤–٧ سجلًّا عاليًا (dil⇒bh) ⇒ البايتُ يُكتَب في سجلٍّ
            //      خاطئ. movzx r64,m8 يعالج الأمرين (REX.W يعنون dil/sil صحيحًا + يمدّد بالصفر).
            bool loadByteZX(int dstReg, int ptrReg)
            {
                return emit(x86::mnem::kMovzx, "r64, m8", {x86::Operand::R(dstReg), x86::Operand::M(ptrReg, 0)});
            }
            // (AR) طولُ سلسلةٍ بالبايت (لا نقاطِ رمز — لنسخِ CONCAT/SUBSTR الخام): يمشي ptrReg حتّى NUL
            //      عادًّا في lenReg (يدهس ptrReg وlenReg وscratchByte). للاستعمال داخلَ منطقةِ انسكاب.
            bool byteStrlen(int ptrReg, int lenReg, int scratchByte)
            {
                if (!movImm(lenReg, 0))
                    return false;
                const size_t head = code_.size();
                if (!loadByteZX(scratchByte, ptrReg) || !cmpZero(scratchByte))
                    return false;
                size_t done;
                if (!emitJccFwd(x86::mnem::kJe, done))
                    return false;
                if (!addImm(lenReg, 1) || !addImm(ptrReg, 1) || !emitJmpBack(head))
                    return false;
                patchFwd(done);
                return true;
            }
            // (AR) نسخُ cntReg بايتًا من srcReg إلى dstReg (كلاهما يتقدّم) بفحصِ العدّ في الرأس (لا التفافَ
            //      عند صفر). البايتُ عبر scratchByte. dstReg يبقى عند نهايةِ النسخ (لسلسلةِ نسخات CONCAT).
            bool byteCopy(int dstReg, int srcReg, int cntReg, int scratchByte)
            {
                const size_t head = code_.size();
                if (!cmpZero(cntReg))
                    return false;
                size_t done;
                if (!emitJccFwd(x86::mnem::kJe, done))
                    return false;
                if (!loadByteZX(scratchByte, srcReg) || !storeByte(dstReg, scratchByte) ||
                    !addImm(dstReg, 1) || !addImm(srcReg, 1) || !subImm(cntReg, 1) || !emitJmpBack(head))
                    return false;
                patchFwd(done);
                return true;
            }
            // (AR) cmp r64, imm32 (لحدود UTF-8 ٠x80/٠xC0 التي تتجاوز مدى imm8 الموقَّع).
            bool cmpImm32(int reg, long long imm) { return emit(x86::mnem::kCmp, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)}); }
            // (AR) يقدّم ptrReg متجاوزًا nReg نقطةَ رمزِ UTF-8 (أو حتّى NUL — قصٌّ ضمنيّ). نقطةُ الرمز =
            //      بايتٌ بادئٌ ثمّ بايتاتُه التابعة (0x80..0xBF). scratchByte خدشُ البايت. يدهس ptr/n/scratch.
            bool skipCodepoints(int ptrReg, int nReg, int scratchByte)
            {
                const size_t headN = code_.size();
                if (!cmpZero(nReg))
                    return false;
                size_t doneN;
                if (!emitJccFwd(x86::mnem::kJe, doneN)) // استُهلِكت nReg نقطة
                    return false;
                if (!loadByteZX(scratchByte, ptrReg) || !cmpZero(scratchByte))
                    return false;
                size_t doneNul;
                if (!emitJccFwd(x86::mnem::kJe, doneNul)) // نهايةُ النصّ ⇒ قصّ
                    return false;
                if (!addImm(ptrReg, 1)) // (AR) تجاوزِ البايتَ البادئ
                    return false;
                const size_t headC = code_.size(); // (AR) تجاوزِ البايتاتِ التابعة [0x80, 0xC0)
                if (!loadByteZX(scratchByte, ptrReg) || !cmpImm32(scratchByte, kUtf8ContTag))
                    return false;
                size_t notContLo;
                if (!emitJccFwd(x86::mnem::kJl, notContLo)) // < 0x80 ⇒ بادئ
                    return false;
                if (!cmpImm32(scratchByte, kUtf8ContMask))
                    return false;
                size_t notContHi;
                if (!emitJccFwd(x86::mnem::kJge, notContHi)) // ≥ 0xC0 ⇒ بادئ
                    return false;
                if (!addImm(ptrReg, 1) || !emitJmpBack(headC))
                    return false;
                patchFwd(notContLo);
                patchFwd(notContHi);
                if (!subImm(nReg, 1) || !emitJmpBack(headN))
                    return false;
                patchFwd(doneN);
                patchFwd(doneNul);
                return true;
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
                bool hasMemBlock = false;    // (AR) حجز/حرر/عبّئ/انسخ (نواةُ الكومة) ⇒ syscall/حلقةٌ تدهس الحوض
                bool hasStrHeap = false;     // (AR) أوپكودُ نصٍّ يخصّص كومةً داخليًّا (I64_TO_STRING/CONCAT/…)
                bool hasArrayExt = false;    // (AR) CONCAT/ZIP ⇒ mmap (+حلقةٌ لـZIP) يدهس الحوضَ + خاناتُ خدشٍ عابرةٌ لـmmap
                dynGetCount_ = 0;
                for (const auto &blockPtr : blocks)
                    for (const auto &inst : blockPtr->instructions)
                    {
                        // (AR) SET معلَّبٌ: الأمامُ يضع Any على **معامِل** المصفوفة. GET معلَّبٌ:
                        //      الأمامُ يضع Any على **نتيجة** القراءة (لا المعامل) — كما يكشفه مسارُ LLVM.
                        if (inst.opcode == sir::SIROpcode::ARRAY_SET && !inst.operands.empty() &&
                            inst.operands[0].elementType == types::SadTypeKind::Any)
                            hasBoxing = true;
                        // (AR) TUPLE_GET يشاركُ ARRAY_GET بنيةً ⇒ قراءتُه المعلَّبة (نتيجةٌ Any) تلزمها خانةُ dyn أيضًا.
                        if ((inst.opcode == sir::SIROpcode::ARRAY_GET ||
                             inst.opcode == sir::SIROpcode::TUPLE_GET) && inst.result &&
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
                        else if (inst.opcode == sir::SIROpcode::CALL ||
                                 inst.opcode == sir::SIROpcode::OBJECT_CALL || // (AR) نداءٌ افتراضيّ ⇒ منطقةُ انسكابٍ (مرآةُ ARM64)
                                 inst.opcode == sir::SIROpcode::CLOSURE_CALL || // (AR) نداءُ إغلاقٍ (الدفعة ٧) ⇒ نداءٌ غيرُ مباشر
                                 inst.opcode == sir::SIROpcode::CALL_INDIRECT)  // (AR) نداءٌ عبر مؤشّرِ دالّة ⇒ نداءٌ غيرُ مباشر
                            hasCall = true;
                        else if (inst.opcode == sir::SIROpcode::MOD_I64 ||
                                 inst.opcode == sir::SIROpcode::FLOOR_DIV_I64)
                            hasIdiv = true;
                        else if ((inst.opcode == sir::SIROpcode::SHL ||
                                  inst.opcode == sir::SIROpcode::SHR) &&
                                 inst.operands.size() == 2 &&
                                 inst.operands[1].type != sir::SIROperandType::CONSTANT)
                            hasVarShift = true; // (AR) مقدارُ الإزاحة غيرُ ثابتٍ ⇒ يلزمه CL/حفظُ RCX
                        else if (inst.opcode == sir::SIROpcode::ARRAY_NEW ||
                                 inst.opcode == sir::SIROpcode::TUPLE_NEW || // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً
                                 inst.opcode == sir::SIROpcode::ENUM_CONSTRUCT) // (AR) بناءُ تعدادٍ = mmap للكومة
                            hasArrayNew = true; // (AR) mmap يدهس الحوض ⇒ انسكابٌ حولَه
                        // (AR) ALLOC بمعامل نصّيّ (اسم صنف) = كائنُ كومة (OBJECT_NEW) ⇒ mmap يدهس الحوض.
                        if (inst.opcode == sir::SIROpcode::ALLOC && !inst.operands.empty() &&
                            inst.operands[0].type == sir::SIROperandType::CONSTANT &&
                            inst.operands[0].dataType == types::SadTypeKind::String)
                            hasArrayNew = true;
                        // (AR) CLOSURE_CREATE (الدفعة ٧) = mmap لبنية {fn,env}+الملتقَطات ⇒ يدهس الحوضَ حولَه.
                        if (inst.opcode == sir::SIROpcode::CLOSURE_CREATE)
                            hasArrayNew = true;
                        // (AR) محلّيٌّ ضمنيّ (الدفعة ٧): STORE ثنائيُّ المعامل إلى سجلٍّ لم يُخصَّص له ALLOC
                        //      (مثل %__cap_X في جسم اللامدا: env.load ثمّ store إلى محلّيٍّ لم يُصرَّح صراحةً).
                        //      نخصّص له خانةَ إطارٍ كي يطابقَه isMemVar في STORE ثمّ في القراءةِ اللاحقة.
                        if (inst.opcode == sir::SIROpcode::STORE && inst.operands.size() == 2 &&
                            inst.operands[1].type == sir::SIROperandType::REGISTER &&
                            memSlot_.find(inst.operands[1].name) == memSlot_.end())
                        {
                            used += 8;
                            memSlot_[inst.operands[1].name] = -used;
                        }
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
                                 //      (STRING_NEW/BOOL_TO_STRING مستثناةٌ: RAX/RDI خدشٌ فقط، لا حلقة).
                                 inst.opcode == sir::SIROpcode::STRING_LEN ||
                                 inst.opcode == sir::SIROpcode::BUILTIN_STRING_LENGTH ||
                                 inst.opcode == sir::SIROpcode::STRING_CMP ||
                                 inst.opcode == sir::SIROpcode::STRING_TO_I64 ||
                                 inst.opcode == sir::SIROpcode::STRING_TO_F64)
                            hasMemBlock = true; // (AR) syscall/حلقةُ بايتاتٍ تدهس الحوض ⇒ انسكابٌ حولَها
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
                // (AR) عمليّاتُ كومةِ النصّ: منطقةُ خدشٍ (٤ خانات) تبقى حيّةً عبر mmap الداخليّ. الخانةُ
                //      الدنيا (العنوانُ الأدنى) = ‎-used‎ بعد الحجز؛ الخانةُ i عند strHeapBaseDisp_ + i×8.
                if (hasStrHeap)
                {
                    used += kStrHeapSlots * 8;
                    strHeapBaseDisp_ = -used;
                }
                // (AR) امتدادُ المصفوفة (CONCAT/ZIP): ٦ خاناتِ خدشٍ تبقى حيّةً عبر mmap الداخليّ.
                if (hasArrayExt)
                {
                    used += kArrExtSlots * 8;
                    arrExtBaseDisp_ = -used;
                }
                // (AR) إن نادت الدالّةُ أو طبعت، احجز منطقةَ انسكابٍ: خانةٌ لكلّ سجلّ حوض. النداءُ
                //      يدهس كلَّ الحوض (caller-saved)، والطباعةُ تستعمل سجلّاتِ الحوض مُبدَّداتٍ ⇒
                //      تُنسَك المؤقّتاتُ الحيّةُ حولَهما وتُعاد.
                if (hasCall || hasPrint || hasArrayNew || hasAppend || hasBoxing || hasMemBlock || hasStrHeap || hasArrayExt)
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
                // ── نواة الكومة الأصليّة (الدفعة ٢): مؤشّرات + تخصيص/تحرير + نسخ/ملء الذاكرة ──
                case OP::PTR_CAST:
                {
                    // (AR) تحويلُ مؤشّر (i64↔ptr): هويّةٌ في نموذجِ القيمة-في-سجلّ (لا تغييرَ تمثيل).
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::PTR_ADD:
                {
                    // (AR) حسابُ مؤشّرٍ ببايتات: result = ptr + offset (المُبدَّدان RAX/RDI خارجَ الحوض).
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    long long off;
                    if (common::isConstInt(inst.operands[1], off))
                    {
                        if (!addImm(x86::RAX, off))
                            return false;
                    }
                    else if (!loadInto(x86::RDI, inst.operands[1]) || !addReg(x86::RAX, x86::RDI))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::ALLOC_HEAP:
                case OP::FFI_MALLOC:
                {
                    // (AR) حجز(size): mmap(size+16)؛ اكتب [base]=توقيع، [base+8]=الحجمُ الكلّيّ؛ أرجِع
                    //      base+16. نَنسِك المؤقّتاتِ الحيّةَ/المعاملاتِ (mmap يدهس caller-saved) ثمّ نعيدها.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) RSI = الحجمُ الكلّيّ = size + 16 (من خانةِ الانسكاب/ثابت ⇒ لا تصادمَ نقلٍ متوازٍ).
                    if (!loadArgInto(x86::RSI, inst.operands[0]) || !addImm(x86::RSI, kHeapHdrBytes))
                        return false;
                    if (!emitMmapPresetSize()) // (AR) RAX=base؛ RSI يبقى (لا mmap ولا النواةُ يدهسانه)
                        return false;
                    if (!movImm64(x86::RDI, kHeapMagic) || !storeMemBase(x86::RAX, kHeapMagicOff, x86::RDI))
                        return false;
                    if (!storeMemBase(x86::RAX, kHeapSizeOff, x86::RSI)) // (AR) خزّنِ الحجمَ الكلّيَّ
                        return false;
                    if (!addImm(x86::RAX, kHeapHdrBytes)) // (AR) المؤشّرُ المُعاد = base + 16
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
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
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!loadArgInto(x86::RDI, inst.operands[0]) || !subImm(x86::RDI, kHeapHdrBytes))
                        return false; // (AR) RDI = base
                    if (!loadMemBase(x86::RAX, x86::RDI, kHeapMagicOff) ||
                        !movImm64(x86::RSI, kHeapMagic) || !cmpRegReg(x86::RAX, x86::RSI))
                        return false;
                    size_t skip;
                    if (!emitJccFwd(x86::mnem::kJne, skip)) // (AR) لا يطابق ⇒ تخطَّ munmap
                        return false;
                    if (!loadMemBase(x86::RSI, x86::RDI, kHeapSizeOff) || // (AR) len = الحجمُ الكلّيّ
                        !movImm(x86::RAX, kSysMunmapX86) || !emit(x86::mnem::kSyscall, "", {}))
                        return false;
                    patchFwd(skip);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    if (inst.result) // (AR) FFI_FREE عديمةُ النتيجة عمومًا؛ حارسٌ دفاعيّ إن وُجِدت.
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movImm(dst, 0);
                    }
                    return true;
                }
                case OP::MEMSET:
                case OP::FFI_MEMSET:
                case OP::BUILTIN_MEM_SET:
                {
                    // (AR) عبّئ(dest, val, size): حلقةُ بايتاتٍ بفحصِ size=0 **في الرأس** (لا do-while ⇒
                    //      لا التفافَ 2^64 عند size=0). dest→RDI، val→R8 (البايتُ الأدنى)، size→RCX.
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!loadArgInto(x86::RDI, inst.operands[0]) ||
                        !loadArgInto(x86::R8, inst.operands[1]) ||
                        !loadArgInto(x86::RCX, inst.operands[2]))
                        return false;
                    const size_t head = code_.size();
                    if (!cmpZero(x86::RCX))
                        return false;
                    size_t done;
                    if (!emitJccFwd(x86::mnem::kJe, done)) // (AR) size==0 ⇒ اخرج قبل الجسد
                        return false;
                    if (!storeByte(x86::RDI, x86::R8) || !addImm(x86::RDI, 1) ||
                        !subImm(x86::RCX, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(done);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
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
                    // (AR) انسخ(dest, src, size): حلقةُ بايتاتٍ أماميّةٌ بفحصِ size=0 في الرأس. dest→RDI،
                    //      src→RSI، size→RCX، البايتُ عبر R8 (loadByte الجديدة 8A). دلالةُ memcpy (تراكبٌ
                    //      غيرُ معرَّف، ليست memmove).
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!loadArgInto(x86::RDI, inst.operands[0]) ||
                        !loadArgInto(x86::RSI, inst.operands[1]) ||
                        !loadArgInto(x86::RCX, inst.operands[2]))
                        return false;
                    const size_t head = code_.size();
                    if (!cmpZero(x86::RCX))
                        return false;
                    size_t done;
                    if (!emitJccFwd(x86::mnem::kJe, done))
                        return false;
                    if (!loadByte(x86::R8, x86::RSI) || !storeByte(x86::RDI, x86::R8) ||
                        !addImm(x86::RDI, 1) || !addImm(x86::RSI, 1) ||
                        !subImm(x86::RCX, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(done);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
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
                // ── النصوصُ الديناميّة (الدفعة ٣): تمثيلُ القيمة = مؤشّرُ i8* منتهٍ بـNUL ──
                case OP::STRING_NEW:
                {
                    // (AR) إنشاءُ نصّ: يُجسّد المعاملَ قيمةً زمنَ تشغيلٍ (مؤشّرَ i8*). لا تخصيصَ كومة
                    //      (حرفيّةٌ ⇒ عنوانُ rodata؛ مؤشّرٌ حيّ ⇒ هويّة). لا يدهس الحوضَ (RAX/dst فقط).
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return materializeString(inst.operands[0], dst, false);
                }
                case OP::BOOL_TO_STRING:
                {
                    // (AR) منطقيّ → نصّ: القيمةُ ≠٠ ⇒ عنوانُ «صحيح»، وإلّا «خطأ» (ثابتان في rodata).
                    //      مؤشّرُ i8*. لا تخصيصَ ولا دهسَ حوضٍ (RAX/RDI خدشٌ خارجَ الحوض).
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!loadInto(x86::RAX, inst.operands[0]) || !cmpZero(x86::RAX))
                        return false;
                    size_t isFalse;
                    if (!emitJccFwd(x86::mnem::kJe, isFalse))
                        return false;
                    if (!emitLoadCStrAddr(x86::RDI, kDynBoolTrueText))
                        return false;
                    size_t done;
                    if (!emitJccFwd(x86::mnem::kJmp, done))
                        return false;
                    patchFwd(isFalse);
                    if (!emitLoadCStrAddr(x86::RDI, kDynBoolFalseText))
                        return false;
                    patchFwd(done);
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RDI);
                }
                case OP::STRING_LEN:
                case OP::BUILTIN_STRING_LENGTH:
                {
                    // (AR) طولُ النصّ بنقاطِ رمزِ UTF-8 (يطابق __sad_utf8_strlen والمفسّر): عُدَّ البايتَ
                    //      الذي ليس تابعًا (b & 0xC0) != 0x80 حتّى NUL. تدهس الحلقةُ الحوضَ (RSI/R8/R9)
                    //      ⇒ نسكٌ حولَها؛ المعاملُ يُقرأ من خانتِه (fromSpill). RSI=المؤشّر، RAX=العدّاد.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], x86::RSI, true) || !movImm(x86::RAX, 0))
                        return false;
                    const size_t head = code_.size();
                    if (!loadByteZX(x86::RDI, x86::RSI) || !cmpZero(x86::RDI)) // RDI = *RSI (بايتٌ ممدَّدٌ بالصفر)
                        return false;
                    size_t done;
                    if (!emitJccFwd(x86::mnem::kJe, done)) // NUL ⇒ انتهى النصّ
                        return false;
                    // (AR) بايتٌ تابع؟ (RDI & 0xC0) == 0x80 ⇒ لا تَعُدّه. R8 = RDI & القناع، R9 = الوسم.
                    if (!movReg(x86::R8, x86::RDI) || !movImm(x86::R9, kUtf8ContMask) ||
                        !andReg(x86::R8, x86::R9) || !movImm(x86::R9, kUtf8ContTag) ||
                        !cmpRegReg(x86::R8, x86::R9))
                        return false;
                    size_t cont;
                    if (!emitJccFwd(x86::mnem::kJe, cont)) // بايتٌ تابع ⇒ تخطَّ الزيادة
                        return false;
                    if (!addImm(x86::RAX, 1))
                        return false;
                    patchFwd(cont);
                    if (!addImm(x86::RSI, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(done);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_CMP:
                {
                    // (AR) مقارنةُ نصّين: تُعيد منطقيًّا (١ متساويان، ٠ مختلفان) — يطابق emitStringCmp في
                    //      LLVM (strcmp==0، ليست -1/0/1). مقارنةٌ بايتيّةٌ حتّى NUL. تدهس الحوضَ (RSI/R8).
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], x86::RSI, true) ||
                        !materializeString(inst.operands[1], x86::R8, true))
                        return false;
                    const size_t head = code_.size();
                    if (!loadByteZX(x86::RAX, x86::RSI) || !loadByteZX(x86::RDI, x86::R8) ||
                        !cmpRegReg(x86::RAX, x86::RDI))
                        return false;
                    size_t neq;
                    if (!emitJccFwd(x86::mnem::kJne, neq)) // بايتان مختلفان ⇒ غيرُ متساويين
                        return false;
                    if (!cmpZero(x86::RAX)) // متساويان وكلاهما NUL ⇒ انتهى النصّان معًا = متساويان
                        return false;
                    size_t eq;
                    if (!emitJccFwd(x86::mnem::kJe, eq))
                        return false;
                    if (!addImm(x86::RSI, 1) || !addImm(x86::R8, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(eq);
                    if (!movImm(x86::RAX, 1))
                        return false;
                    size_t done;
                    if (!emitJccFwd(x86::mnem::kJmp, done))
                        return false;
                    patchFwd(neq);
                    if (!movImm(x86::RAX, 0))
                        return false;
                    patchFwd(done);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_TO_I64:
                {
                    // (AR) نصّ → رقم (atoll): إشارةٌ اختياريّةٌ ثمّ أرقامٌ عشريّةٌ حتّى أوّلِ غيرِ رقم. تدهس
                    //      الحوضَ (RSI/RCX/R8/R9) ⇒ نسكٌ حولَها. RAX=المتراكم، R9=الإشارة (١/‎-1‎).
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], x86::RSI, true) ||
                        !movImm(x86::RAX, 0) || !movImm(x86::R9, 1))
                        return false;
                    // (AR) الإشارة: أوّلُ بايتٍ '-' ⇒ الإشارة ‎-1‎ وتقدّمٌ (لا نعالج '+' صراحةً: غيرُ رقمٍ يُنهي).
                    if (!loadByteZX(x86::RDI, x86::RSI) || !cmpImm8(x86::RDI, kAsciiMinus))
                        return false;
                    size_t notMinus;
                    if (!emitJccFwd(x86::mnem::kJne, notMinus))
                        return false;
                    if (!movImm(x86::R9, -1) || !addImm(x86::RSI, 1))
                        return false;
                    patchFwd(notMinus);
                    if (!movImm(x86::RCX, kItoaRadix))
                        return false;
                    const size_t head = code_.size();
                    if (!loadByteZX(x86::RDI, x86::RSI) || !cmpImm8(x86::RDI, kAsciiZero))
                        return false;
                    size_t doneLo;
                    if (!emitJccFwd(x86::mnem::kJl, doneLo)) // بايت < '0' ⇒ انتهى
                        return false;
                    if (!cmpImm8(x86::RDI, kAsciiNine))
                        return false;
                    size_t doneHi;
                    if (!emitJccFwd(x86::mnem::kJg, doneHi)) // بايت > '9' ⇒ انتهى
                        return false;
                    // (AR) RAX = RAX*10 + (بايت - '0')؛ ثمّ RSI++.
                    if (!imulReg(x86::RAX, x86::RCX) || !subImm(x86::RDI, kAsciiZero) ||
                        !addReg(x86::RAX, x86::RDI) || !addImm(x86::RSI, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(doneLo);
                    patchFwd(doneHi);
                    if (!imulReg(x86::RAX, x86::R9)) // (AR) طبّقِ الإشارةَ (×١ أو ×‎-1‎) بلا فرع
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_REPLACE:
                case OP::BUILTIN_STRING_REPLACE:
                {
                    // (AR) استبدالُ **أوّلِ ورودٍ** لـneedle بـrepl في s (يطابق LLVM string_ops.cpp؛ 🔴 المفسّرُ
                    //      يستبدل الكلَّ ⇒ انجرافٌ موثَّق). نسخٌ ٣-مقاطع: [s قبلَ المطابقة]+[repl]+[s بعدها]+NUL.
                    //      عند الغياب: matchPtr=نهايةُ s، lenN=lenR=٠ ⇒ المقطعُ ١=كلُّ s، البديلُ/الذيلُ فارغان
                    //      = نسخةٌ من s (توحيدٌ بلا فرعِ نسخٍ منفصل). خانات strHeap: 0=lenN 1=matchPtr 2=totalLen
                    //      3=buf 4=seg1Len 5=lenR 6=lenTail.
                    if (!inst.result || inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) lenN = طولُ needle بالبايت.
                    if (!materializeString(inst.operands[1], x86::RSI, true) ||
                        !byteStrlen(x86::RSI, x86::R10, x86::RDI) || !storeMem(strHeapSlot(0), x86::R10))
                        return false;
                    // (AR) البحثُ عن needle في s (RSI يمشي s).
                    if (!materializeString(inst.operands[0], x86::RSI, true))
                        return false;
                    const size_t outer = code_.size();
                    if (!movReg(x86::RDI, x86::RSI) || !materializeString(inst.operands[1], x86::R10, true))
                        return false;
                    const size_t inner = code_.size();
                    if (!loadByteZX(x86::R8, x86::R10) || !cmpZero(x86::R8))
                        return false;
                    size_t matched;
                    if (!emitJccFwd(x86::mnem::kJe, matched))
                        return false;
                    if (!loadByteZX(x86::R9, x86::RDI) || !cmpRegReg(x86::R8, x86::R9))
                        return false;
                    size_t mism;
                    if (!emitJccFwd(x86::mnem::kJne, mism))
                        return false;
                    if (!addImm(x86::RDI, 1) || !addImm(x86::R10, 1) || !emitJmpBack(inner))
                        return false;
                    patchFwd(mism);
                    if (!loadByteZX(x86::R8, x86::RSI) || !cmpZero(x86::R8))
                        return false;
                    size_t notfound;
                    if (!emitJccFwd(x86::mnem::kJe, notfound))
                        return false;
                    if (!addImm(x86::RSI, 1) || !emitJmpBack(outer))
                        return false;
                    // (AR) الغياب: matchPtr=نهايةُ s، lenN=٠، lenR=٠.
                    patchFwd(notfound);
                    if (!storeMem(strHeapSlot(1), x86::RSI) || !movImm(x86::R8, 0) ||
                        !storeMem(strHeapSlot(0), x86::R8) || !storeMem(strHeapSlot(5), x86::R8))
                        return false;
                    size_t toUnified;
                    if (!emitJccFwd(x86::mnem::kJmp, toUnified))
                        return false;
                    // (AR) التطابق: matchPtr=RSI؛ lenR = طولُ repl بالبايت.
                    patchFwd(matched);
                    if (!storeMem(strHeapSlot(1), x86::RSI) ||
                        !materializeString(inst.operands[2], x86::RSI, true) ||
                        !byteStrlen(x86::RSI, x86::R10, x86::RDI) || !storeMem(strHeapSlot(5), x86::R10))
                        return false;
                    // (AR) الموحَّد: احسبِ الأطوالَ ثمّ mmap ثمّ انسخِ المقاطعَ الثلاثة.
                    patchFwd(toUnified);
                    if (!materializeString(inst.operands[0], x86::R11, true)) // R11 = sBase
                        return false;
                    if (!loadMem(x86::RAX, strHeapSlot(1)) || !movReg(x86::R10, x86::RAX) ||
                        !subReg(x86::R10, x86::R11) || !storeMem(strHeapSlot(4), x86::R10)) // seg1Len
                        return false;
                    // (AR) lenTail = strlen(matchPtr + lenN).
                    if (!loadMem(x86::RAX, strHeapSlot(1)) || !loadMem(x86::R10, strHeapSlot(0)) ||
                        !addReg(x86::RAX, x86::R10) || !movReg(x86::RSI, x86::RAX) ||
                        !byteStrlen(x86::RSI, x86::R10, x86::RDI) || !storeMem(strHeapSlot(6), x86::R10))
                        return false;
                    // (AR) totalLen = seg1Len + lenR + lenTail.
                    if (!loadMem(x86::RAX, strHeapSlot(4)) || !loadMem(x86::R10, strHeapSlot(5)) ||
                        !addReg(x86::RAX, x86::R10) || !loadMem(x86::R10, strHeapSlot(6)) ||
                        !addReg(x86::RAX, x86::R10) || !storeMem(strHeapSlot(2), x86::RAX))
                        return false;
                    if (!movReg(x86::RSI, x86::RAX) || !addImm(x86::RSI, 1 + kHeapHdrBytes) || !emitMmapPresetSize())
                        return false; // RAX = base
                    if (!movImm64(x86::RDI, kHeapMagic) || !storeMemBase(x86::RAX, kHeapMagicOff, x86::RDI) ||
                        !storeMemBase(x86::RAX, kHeapSizeOff, x86::RSI))
                        return false;
                    if (!addImm(x86::RAX, kHeapHdrBytes) || !storeMem(strHeapSlot(3), x86::RAX)) // buf
                        return false;
                    // (AR) المقطعُ ١: s[0..seg1Len)  ← RDI الجاري.
                    if (!movReg(x86::RDI, x86::RAX) || !materializeString(inst.operands[0], x86::RSI, true) ||
                        !loadMem(x86::RCX, strHeapSlot(4)) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    // (AR) المقطعُ ٢: repl[0..lenR).
                    if (!materializeString(inst.operands[2], x86::RSI, true) ||
                        !loadMem(x86::RCX, strHeapSlot(5)) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    // (AR) المقطعُ ٣: s[matchPtr+lenN ..) بطولِ lenTail.
                    if (!loadMem(x86::RSI, strHeapSlot(1)) || !loadMem(x86::R10, strHeapSlot(0)) ||
                        !addReg(x86::RSI, x86::R10) || !loadMem(x86::RCX, strHeapSlot(6)) ||
                        !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    if (!movImm(x86::R8, 0) || !storeByte(x86::RDI, x86::R8)) // NUL
                        return false;
                    if (!loadMem(x86::RAX, strHeapSlot(3))) // (AR) النتيجة = buf
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_FIND:
                case OP::BUILTIN_STRING_FIND:
                {
                    // (AR) البحثُ عن needle في haystack: يُعيد **فهرسَ نقطةِ رمزٍ** لأوّلِ ورود (يطابق LLVM
                    //      string_ops.cpp: strstr ثمّ تحويلُ إزاحةِ البايت لفهرسِ نقطةِ رمز)، أو ‎-1‎ عند
                    //      الغياب. 🔴 المفسّرُ يُعيد إزاحةَ بايتٍ (انجرافٌ موثَّق؛ الخلفيّةُ تطابق LLVM).
                    //      needle فارغٌ ⇒ ٠ (كـstrstr). RSI=haystack الجاري، hayBase في strHeap[0].
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], x86::RSI, true) || !storeMem(strHeapSlot(0), x86::RSI))
                        return false;
                    // (AR) الحلقةُ الخارجيّة: مطابقةُ needle عند RSI.
                    const size_t outer = code_.size();
                    if (!movReg(x86::RDI, x86::RSI) || !materializeString(inst.operands[1], x86::R10, true))
                        return false; // RDI=hp، R10=np (قاعدةُ needle)
                    const size_t inner = code_.size();
                    if (!loadByteZX(x86::R8, x86::R10) || !cmpZero(x86::R8))
                        return false;
                    size_t matched;
                    if (!emitJccFwd(x86::mnem::kJe, matched)) // نهايةُ needle ⇒ تطابقٌ عند RSI
                        return false;
                    if (!loadByteZX(x86::R9, x86::RDI) || !cmpRegReg(x86::R8, x86::R9))
                        return false;
                    size_t mism;
                    if (!emitJccFwd(x86::mnem::kJne, mism)) // بايتان مختلفان ⇒ لا تطابق
                        return false;
                    if (!addImm(x86::RDI, 1) || !addImm(x86::R10, 1) || !emitJmpBack(inner))
                        return false;
                    patchFwd(mism);
                    if (!loadByteZX(x86::R8, x86::RSI) || !cmpZero(x86::R8))
                        return false;
                    size_t notfound;
                    if (!emitJccFwd(x86::mnem::kJe, notfound)) // نهايةُ haystack ⇒ غيرُ موجود
                        return false;
                    if (!addImm(x86::RSI, 1) || !emitJmpBack(outer))
                        return false;
                    // (AR) تطابق: الفهرس = عددُ نقاطِ الرمزِ في [hayBase, RSI).
                    patchFwd(matched);
                    if (!loadMem(x86::R11, strHeapSlot(0)) || !movImm(x86::RAX, 0))
                        return false;
                    const size_t countHead = code_.size();
                    if (!cmpRegReg(x86::R11, x86::RSI))
                        return false;
                    size_t countDone;
                    if (!emitJccFwd(x86::mnem::kJge, countDone)) // R11 بلغ موضعَ التطابق
                        return false;
                    if (!loadByteZX(x86::R8, x86::R11) || !movReg(x86::R9, x86::R8) ||
                        !movImm(x86::RCX, kUtf8ContMask) || !andReg(x86::R9, x86::RCX) ||
                        !movImm(x86::RCX, kUtf8ContTag) || !cmpRegReg(x86::R9, x86::RCX))
                        return false;
                    size_t skipInc;
                    if (!emitJccFwd(x86::mnem::kJe, skipInc)) // بايتٌ تابع ⇒ لا يُحسَب
                        return false;
                    if (!addImm(x86::RAX, 1))
                        return false;
                    patchFwd(skipInc);
                    if (!addImm(x86::R11, 1) || !emitJmpBack(countHead))
                        return false;
                    patchFwd(countDone);
                    size_t toEnd;
                    if (!emitJccFwd(x86::mnem::kJmp, toEnd))
                        return false;
                    patchFwd(notfound);
                    if (!movImm(x86::RAX, -1)) // (AR) غيرُ موجود
                        return false;
                    patchFwd(toEnd);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_SUBSTR:
                case OP::BUILTIN_STRING_SUBSTRING:
                {
                    // (AR) جزءُ نصّ: المعاملات s، start، count (بنقاطِ رمزِ UTF-8 كـLLVM). p0=s متقدّمًا
                    //      start نقطة؛ p1=p0 متقدّمًا count نقطة (قصٌّ ضمنيٌّ عند NUL)؛ byteLen=p1−p0؛
                    //      buf=mmap(byteLen+1+16)+memcpy(p0,byteLen)+NUL. p0/byteLen/buf في خانات strHeap.
                    if (!inst.result || inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) p0 = s متقدّمًا start نقطةَ رمز.
                    if (!materializeString(inst.operands[0], x86::RSI, true) ||
                        !loadArgInto(x86::R10, inst.operands[1]) || !skipCodepoints(x86::RSI, x86::R10, x86::RDI) ||
                        !storeMem(strHeapSlot(0), x86::RSI))
                        return false;
                    // (AR) p1 = p0 متقدّمًا count نقطة؛ byteLen = p1 − p0.
                    if (!loadArgInto(x86::R10, inst.operands[2]) || !skipCodepoints(x86::RSI, x86::R10, x86::RDI))
                        return false;
                    if (!loadMem(x86::R11, strHeapSlot(0)) || !movReg(x86::R10, x86::RSI) ||
                        !subReg(x86::R10, x86::R11) || !storeMem(strHeapSlot(1), x86::R10)) // byteLen
                        return false;
                    // (AR) mmap(byteLen+1+16).
                    if (!movReg(x86::RSI, x86::R10) || !addImm(x86::RSI, 1 + kHeapHdrBytes) || !emitMmapPresetSize())
                        return false; // RAX = base
                    if (!movImm64(x86::RDI, kHeapMagic) || !storeMemBase(x86::RAX, kHeapMagicOff, x86::RDI) ||
                        !storeMemBase(x86::RAX, kHeapSizeOff, x86::RSI))
                        return false;
                    if (!addImm(x86::RAX, kHeapHdrBytes) || !storeMem(strHeapSlot(2), x86::RAX)) // buf
                        return false;
                    // (AR) memcpy(buf, p0, byteLen) + NUL.
                    if (!movReg(x86::RDI, x86::RAX) || !loadMem(x86::RSI, strHeapSlot(0)) ||
                        !loadMem(x86::RCX, strHeapSlot(1)) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    if (!movImm(x86::R8, 0) || !storeByte(x86::RDI, x86::R8))
                        return false;
                    if (!loadMem(x86::RAX, strHeapSlot(2))) // (AR) النتيجة = buf
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_CONCAT:
                {
                    // (AR) دمجُ نصّين: buf = mmap(len1+len2+1+16)؛ انسخ s1 ثمّ s2 ثمّ NUL؛ أرجِع buf.
                    //      (يطابق string_ops.cpp: malloc(l1+l2+1)+نسخ+NUL). len1/len2 بالبايت (لا نقاطِ
                    //      رمز). الأطوالُ والمخزنُ في خانات strHeap لتبقى عبر mmap (يدهس الحوضَ). RDI مؤشّرُ
                    //      الوجهةِ الجاري (خدشٌ خارجَ الحوض)، RAX خدشُ mmap/النتيجة.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) len1 → خانة٠، len2 → خانة١.
                    if (!materializeString(inst.operands[0], x86::RSI, true) ||
                        !byteStrlen(x86::RSI, x86::R10, x86::RDI) || !storeMem(strHeapSlot(0), x86::R10))
                        return false;
                    if (!materializeString(inst.operands[1], x86::RSI, true) ||
                        !byteStrlen(x86::RSI, x86::R11, x86::RDI) || !storeMem(strHeapSlot(1), x86::R11))
                        return false;
                    // (AR) mmap(len1+len2+1+16)؛ RSI = الحجمُ الكلّيّ.
                    if (!loadMem(x86::R10, strHeapSlot(0)) || !loadMem(x86::R11, strHeapSlot(1)) ||
                        !movReg(x86::RSI, x86::R10) || !addReg(x86::RSI, x86::R11) ||
                        !addImm(x86::RSI, 1 + kHeapHdrBytes) || !emitMmapPresetSize())
                        return false; // (AR) RAX = base
                    if (!movImm64(x86::RDI, kHeapMagic) || !storeMemBase(x86::RAX, kHeapMagicOff, x86::RDI) ||
                        !storeMemBase(x86::RAX, kHeapSizeOff, x86::RSI))
                        return false;
                    if (!addImm(x86::RAX, kHeapHdrBytes) || !storeMem(strHeapSlot(2), x86::RAX)) // buf (النتيجة)
                        return false;
                    // (AR) انسخ s1 → buf (RDI الجاري)، ثمّ s2 → buf+len1.
                    if (!movReg(x86::RDI, x86::RAX) || !materializeString(inst.operands[0], x86::RSI, true) ||
                        !loadMem(x86::RCX, strHeapSlot(0)) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    if (!materializeString(inst.operands[1], x86::RSI, true) ||
                        !loadMem(x86::RCX, strHeapSlot(1)) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    if (!movImm(x86::R8, 0) || !storeByte(x86::RDI, x86::R8)) // (AR) خاتمةُ NUL
                        return false;
                    if (!loadMem(x86::RAX, strHeapSlot(2))) // (AR) النتيجة = buf (خارجَ الحوض)
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::I64_TO_STRING:
                {
                    // (AR) رقم → نصّ: يخصّص مخزنَ كومةٍ (kItoaBufPayload) ويبني الأرقامَ تنازليًّا من
                    //      نهايته منتهيةً بـNUL، ثمّ يُعيد مؤشّرًا لأوّلِ رقم/إشارة. منطقُ itoa كـemitPrintInt
                    //      (باقٍ-سالبٌ ⇒ آمنٌ لـINT64_MIN) لكن الكتابةُ في الكومة والإرجاعُ مؤشّرًا لا write.
                    //      🔴 المؤشّرُ المُعاد وسطُ المخزن (الأرقامُ من النهاية) لا قاعدتُه ⇒ غيرُ قابلٍ
                    //      لـFREE (يتخطّاه الحارسُ بأمان)؛ تسريبٌ مقبولٌ مطابقٌ لـLLVM (نصوصُ TO_STRING لا تُحرَّر).
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) احفظِ العددَ في خانةِ خدشٍ (mmap يدهسه)، ثمّ خصّصْ المخزنَ.
                    if (!loadArgInto(x86::RAX, inst.operands[0]) || !storeMem(strHeapSlot(0), x86::RAX))
                        return false;
                    if (!movImm(x86::RSI, kItoaBufPayload + kHeapHdrBytes) || !emitMmapPresetSize())
                        return false; // (AR) RAX = base
                    if (!movImm64(x86::RDI, kHeapMagic) || !storeMemBase(x86::RAX, kHeapMagicOff, x86::RDI) ||
                        !storeMemBase(x86::RAX, kHeapSizeOff, x86::RSI))
                        return false;
                    if (!addImm(x86::RAX, kHeapHdrBytes)) // (AR) RAX = buf (المؤشّرُ المستخدَم)
                        return false;
                    // (AR) R10 = buf + payload (قمّةٌ حصريّة)؛ اكتب NUL في *(--R10).
                    if (!movReg(x86::R10, x86::RAX) || !addImm(x86::R10, kItoaBufPayload) ||
                        !subImm(x86::R10, 1) || !movImm(x86::R9, 0) || !storeByte(x86::R10, x86::R9))
                        return false;
                    if (!loadMem(x86::RAX, strHeapSlot(0)) || !movImm(x86::RCX, kItoaRadix))
                        return false;
                    if (!cmpZero(x86::RAX)) // (AR) الإشارة: RAX ≥ 0 ⇒ اقفز للموجب
                        return false;
                    size_t toPositive;
                    if (!emitJccFwd(x86::mnem::kJge, toPositive))
                        return false;
                    // (AR) لولبُ السالب: cqo؛ idiv ⇒ RDX=باقٍ(≤٠)؛ الرقم = '0' − RDX = |باقٍ|.
                    const size_t negLoop = code_.size();
                    if (!cqo() || !idivReg(x86::RCX) ||
                        !movImm(x86::R9, kAsciiZero) || !subReg(x86::R9, x86::RDX) ||
                        !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::R9) ||
                        !cmpZero(x86::RAX) || !emitLocalJneBack(negLoop))
                        return false;
                    if (!movImm(x86::R9, kAsciiMinus) || !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::R9))
                        return false;
                    size_t toDone;
                    if (!emitJccFwd(x86::mnem::kJmp, toDone))
                        return false;
                    // (AR) لولبُ الموجب: cqo؛ idiv ⇒ RDX=باقٍ(٠..٩)؛ الرقم = '0' + RDX.
                    patchFwd(toPositive);
                    const size_t posLoop = code_.size();
                    if (!cqo() || !idivReg(x86::RCX) ||
                        !addImm(x86::RDX, kAsciiZero) ||
                        !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::RDX) ||
                        !cmpZero(x86::RAX) || !emitLocalJneBack(posLoop))
                        return false;
                    patchFwd(toDone);
                    if (!movReg(x86::RAX, x86::R10)) // (AR) النتيجة = R10 (أوّل رقم/إشارة) ⇒ خارجَ الحوض
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::F64_TO_STRING:
                {
                    // (AR) عشريّ → نصّ: يفكّك البتّاتِ (إشارة/جزءٌ صحيح/كسرٌ مقرَّبٌ ٦ خاناتٍ مع حذفِ
                    //      الأصفار الزائدة كـemitPrintFloat) ثمّ يبني النصَّ تنازليًّا في مخزنِ كومةٍ منتهيًا
                    //      بـNUL ويُعيد مؤشّرًا لأوّلِ رمز — كتابةٌ في الكومة لا write. 🔴 المؤشّرُ وسطُ
                    //      المخزن ⇒ غيرُ قابلٍ لـFREE (تسريبٌ مطابقٌ لـLLVM؛ نصوصُ TO_STRING لا تُحرَّر).
                    //      خانات strHeap: 0=ip 1=scaled 2=nd 3=sign.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) فُكّ العشريَّ (نمطُ بتّاته في RAX) إلى ip/scaled/nd/sign **قبل** mmap (يدهس السجلّات).
                    if (!loadArgInto(x86::RAX, inst.operands[0]))
                        return false;
                    // sign = (bits & signmask) ? 1 : 0 (يُخزَّن لتقرير بادئةِ '-' بعد mmap).
                    if (!movImm64(x86::R8, static_cast<long long>(kF64SignMask)) || !andReg(x86::R8, x86::RAX))
                        return false;
                    size_t signZero;
                    if (!cmpZero(x86::R8) || !emitJccFwd(x86::mnem::kJe, signZero))
                        return false;
                    if (!movImm(x86::R8, 1))
                        return false;
                    size_t afterSign;
                    if (!emitJccFwd(x86::mnem::kJmp, afterSign))
                        return false;
                    patchFwd(signZero);
                    if (!movImm(x86::R8, 0))
                        return false;
                    patchFwd(afterSign);
                    if (!storeMem(strHeapSlot(3), x86::R8)) // sign
                        return false;
                    // |x| = bits & absmask ⇒ ip = trunc(|x|) في R11.
                    if (!movImm64(x86::R8, static_cast<long long>(kF64AbsMask)) || !andReg(x86::RAX, x86::R8))
                        return false;
                    if (!movqToXmm(kXmm0, x86::RAX) || !cvttsd2si(x86::R11, kXmm0))
                        return false;
                    // scaled = round_nearest(( |x| − ip ) × ١٠^٦) في R8.
                    if (!movqToXmm(kXmm0, x86::RAX) || !cvtsi2sd(kXmm1, x86::R11) || !subsd(kXmm0, kXmm1))
                        return false;
                    if (!loadFloatConst(x86::RAX, static_cast<double>(kFloatPrecisionScale)) ||
                        !movqToXmm(kXmm1, x86::RAX) || !mulsd(kXmm0, kXmm1) || !cvtsd2si(x86::R8, kXmm0))
                        return false;
                    // ترحيلُ الحمل: scaled == ١٠^٦ ⇒ ip++ وscaled=0 (يطابق عائقَ «1.0 − 4e−7» في emitPrintFloat).
                    if (!movImm(x86::RCX, kFloatPrecisionScale) || !cmpRegReg(x86::R8, x86::RCX))
                        return false;
                    size_t noCarry;
                    if (!emitJccFwd(x86::mnem::kJne, noCarry))
                        return false;
                    if (!addImm(x86::R11, 1) || !movImm(x86::R8, 0))
                        return false;
                    patchFwd(noCarry);
                    if (!storeMem(strHeapSlot(0), x86::R11)) // ip
                        return false;
                    // حذفُ الأصفار الزائدة: nd=٦؛ بينما (nd>1 && scaled%10==0) scaled/=10 وnd--.
                    if (!movImm(x86::R9, kFloatDecimals) || !movImm(x86::RCX, kItoaRadix))
                        return false;
                    size_t stripTop = code_.size();
                    if (!cmpImm8(x86::R9, 1))
                        return false;
                    size_t stripDone;
                    if (!emitJccFwd(x86::mnem::kJle, stripDone))
                        return false;
                    if (!movReg(x86::RAX, x86::R8) || !cqo() || !idivReg(x86::RCX) || !cmpZero(x86::RDX))
                        return false;
                    size_t stripStop;
                    if (!emitJccFwd(x86::mnem::kJne, stripStop))
                        return false;
                    if (!movReg(x86::R8, x86::RAX) || !subImm(x86::R9, 1) || !emitJmpBack(stripTop))
                        return false;
                    patchFwd(stripDone);
                    patchFwd(stripStop);
                    if (!storeMem(strHeapSlot(1), x86::R8) || !storeMem(strHeapSlot(2), x86::R9)) // scaled, nd
                        return false;
                    // (AR) خصّصْ مخزنَ الكومة (kFtoaBufPayload + رأسٌ خفيّ).
                    if (!movImm(x86::RSI, kFtoaBufPayload + kHeapHdrBytes) || !emitMmapPresetSize())
                        return false; // (AR) RAX = base
                    if (!movImm64(x86::RDI, kHeapMagic) || !storeMemBase(x86::RAX, kHeapMagicOff, x86::RDI) ||
                        !storeMemBase(x86::RAX, kHeapSizeOff, x86::RSI))
                        return false;
                    if (!addImm(x86::RAX, kHeapHdrBytes)) // (AR) RAX = buf
                        return false;
                    // (AR) R10 = buf + payload (قمّةٌ حصريّة)؛ اكتب NUL في *(--R10).
                    if (!movReg(x86::R10, x86::RAX) || !addImm(x86::R10, kFtoaBufPayload) ||
                        !subImm(x86::R10, 1) || !movImm(x86::R9, 0) || !storeByte(x86::R10, x86::R9))
                        return false;
                    if (!movImm(x86::RCX, kItoaRadix))
                        return false;
                    // (١) خاناتُ الكسر تنازليًّا (nd خانةً؛ الأصفارُ البادئةُ تُحشى تلقائيًّا حين يبلغ scaled صفرًا).
                    if (!loadMem(x86::RAX, strHeapSlot(1)) || !loadMem(x86::R9, strHeapSlot(2))) // RAX=scaled, R9=nd
                        return false;
                    size_t fracTop = code_.size();
                    if (!cmpImm8(x86::R9, 0))
                        return false;
                    size_t fracDone;
                    if (!emitJccFwd(x86::mnem::kJle, fracDone))
                        return false;
                    if (!cqo() || !idivReg(x86::RCX) || !addImm(x86::RDX, kAsciiZero) ||
                        !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::RDX) ||
                        !subImm(x86::R9, 1) || !emitJmpBack(fracTop))
                        return false;
                    patchFwd(fracDone);
                    // (٢) النقطة.
                    if (!subImm(x86::R10, 1) || !movImm(x86::R9, kAsciiDot) || !storeByte(x86::R10, x86::R9))
                        return false;
                    // (٣) الجزءُ الصحيح ip تنازليًّا (رقمٌ واحدٌ على الأقلّ حتّى لو ٠؛ ip ≥ ٠).
                    if (!loadMem(x86::RAX, strHeapSlot(0)))
                        return false;
                    size_t ipTop = code_.size();
                    if (!cqo() || !idivReg(x86::RCX) || !addImm(x86::RDX, kAsciiZero) ||
                        !subImm(x86::R10, 1) || !storeByte(x86::R10, x86::RDX) ||
                        !cmpZero(x86::RAX) || !emitJccBack(x86::mnem::kJne, ipTop))
                        return false;
                    // (٤) بادئةُ السالب إن لزم.
                    if (!loadMem(x86::R8, strHeapSlot(3)) || !cmpZero(x86::R8))
                        return false;
                    size_t noSign;
                    if (!emitJccFwd(x86::mnem::kJe, noSign))
                        return false;
                    if (!subImm(x86::R10, 1) || !movImm(x86::R9, kAsciiMinus) || !storeByte(x86::R10, x86::R9))
                        return false;
                    patchFwd(noSign);
                    if (!movReg(x86::RAX, x86::R10)) // (AR) النتيجة = R10 (أوّلُ رمز) ⇒ خارجَ الحوض
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::STRING_TO_F64:
                {
                    // (AR) نصّ → عشريّ (atof مبسّط): [إشارة][أرقام][.][أرقام]. نتراكمُ كلَّ الأرقام (متجاهلين
                    //      النقطة) في متراكمٍ double: acc = acc×١٠ + رقم؛ ونعدّ خاناتِ الكسر fd، ثمّ
                    //      result = acc ÷ ١٠^fd، ثمّ نطبّقُ الإشارة (٠−acc). النتيجةُ نمطُ بتّاتٍ i64 في dst
                    //      (كسائرِ العوائم في الحوض). يدهس الحوضَ ⇒ نسكٌ حولَه. xmm2=المتراكم، xmm3=١٠٫٠.
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!materializeString(inst.operands[0], x86::RSI, true))
                        return false;
                    // xmm2 = acc = 0.0 ؛ xmm3 = 10.0 (ثابتُ التراكم والقسمة).
                    if (!movImm(x86::RAX, 0) || !cvtsi2sd(kXmm2, x86::RAX))
                        return false;
                    if (!loadFloatConst(x86::RAX, static_cast<double>(kItoaRadix)) || !movqToXmm(kXmm3, x86::RAX))
                        return false;
                    // الإشارة: أوّلُ بايتٍ '-' ⇒ R9=1 وتقدّمٌ (لا نعالج '+' صراحةً: غيرُ رقمٍ يُنهي).
                    if (!movImm(x86::R9, 0))
                        return false;
                    if (!loadByteZX(x86::RDI, x86::RSI) || !cmpImm8(x86::RDI, kAsciiMinus))
                        return false;
                    size_t notMinus;
                    if (!emitJccFwd(x86::mnem::kJne, notMinus))
                        return false;
                    if (!movImm(x86::R9, 1) || !addImm(x86::RSI, 1))
                        return false;
                    patchFwd(notMinus);
                    // R8 = fd (خاناتُ الكسر)، R10 = علمُ «مررنا بالنقطة» (٠/١).
                    if (!movImm(x86::R8, 0) || !movImm(x86::R10, 0))
                        return false;
                    const size_t head = code_.size();
                    if (!loadByteZX(x86::RDI, x86::RSI))
                        return false;
                    // النقطة ⇒ فعّلْ علمَ الكسر وتقدّمْ (الثانيةُ ستُعامَل كغيرِ رقمٍ فتُنهي).
                    if (!cmpImm8(x86::RDI, kAsciiDot))
                        return false;
                    size_t notDot;
                    if (!emitJccFwd(x86::mnem::kJne, notDot))
                        return false;
                    if (!movImm(x86::R10, 1) || !addImm(x86::RSI, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(notDot);
                    // غيرُ رقمٍ (< '0' أو > '9') ⇒ انتهى.
                    if (!cmpImm8(x86::RDI, kAsciiZero))
                        return false;
                    size_t doneLo;
                    if (!emitJccFwd(x86::mnem::kJl, doneLo))
                        return false;
                    if (!cmpImm8(x86::RDI, kAsciiNine))
                        return false;
                    size_t doneHi;
                    if (!emitJccFwd(x86::mnem::kJg, doneHi))
                        return false;
                    // رقم: acc = acc×١٠ + (بايت−'0')؛ fd += R10 (١ إن بعد النقطة، ٠ قبلها) ⇒ بلا فرع.
                    if (!mulsd(kXmm2, kXmm3) || !subImm(x86::RDI, kAsciiZero) ||
                        !cvtsi2sd(kXmm0, x86::RDI) || !addsd(kXmm2, kXmm0))
                        return false;
                    if (!addReg(x86::R8, x86::R10) || !addImm(x86::RSI, 1) || !emitJmpBack(head))
                        return false;
                    patchFwd(doneLo);
                    patchFwd(doneHi);
                    // القسمةُ على ١٠^fd: بينما fd>0: acc /= 10، fd--.
                    size_t divTop = code_.size();
                    if (!cmpImm8(x86::R8, 0))
                        return false;
                    size_t divDone;
                    if (!emitJccFwd(x86::mnem::kJle, divDone))
                        return false;
                    if (!divsd(kXmm2, kXmm3) || !subImm(x86::R8, 1) || !emitJmpBack(divTop))
                        return false;
                    patchFwd(divDone);
                    // الإشارة: إن R9==1 فالنتيجة = ٠−acc (نفيٌ عشريّ)، وإلّا acc مباشرةً.
                    size_t noNeg;
                    if (!cmpZero(x86::R9) || !emitJccFwd(x86::mnem::kJe, noNeg))
                        return false;
                    if (!movImm(x86::RAX, 0) || !cvtsi2sd(kXmm0, x86::RAX) ||
                        !subsd(kXmm0, kXmm2) || !movqFromXmm(x86::RAX, kXmm0))
                        return false;
                    size_t signApplied;
                    if (!emitJccFwd(x86::mnem::kJmp, signApplied))
                        return false;
                    patchFwd(noNeg);
                    if (!movqFromXmm(x86::RAX, kXmm2))
                        return false;
                    patchFwd(signApplied);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
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
                    // (AR) الكائنيّة: ALLOC بمعامل نصّيّ = اسمُ صنفٍ ⇒ كائنُ كومة. نخصّص عبر mmap
                    //      (رأسُ vtable@0 لغير-CRepr + حقولٌ ٨-بت)، ونخزّن المؤشّرَ في خانة النتيجة،
                    //      ونسجّل صنفَ الكائن (لحساب إزاحات OBJECT_GET/SET). حقولُ الصنفِ ٨-بت فقط.
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
                        const long long size =
                            8LL * (ci->second.numFields + (ci->second.isCRepr ? 0 : 1));
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                    return false;
                        if (!emitMmap(size < 8 ? 8 : size)) // (AR) RAX=المؤشّر (mmap يصفّر الكتلة)
                            return false;
                        for (const auto &kv : regOf_)
                            if (common::usedAfterInBlock(block, instIdx, kv.first))
                                if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                    return false;
                        objClassOf_[inst.result->name] = className;
                        // (AR) الإرسالُ الافتراضيّ (الدفعة ٦): خزّنْ عنوانَ جدولِ دوالِّ الصنفِ في
                        //      obj[0] (ترويسةُ vtable). عنوانُ الجدولِ يُرقَّع زمنَ الإنهاء. RAX=المؤشّر.
                        if (!ci->second.isCRepr && vtableBaseOff_.count(className))
                        {
                            if (!movImm64(x86::RDI, 0)) // (AR) نائبٌ يُرقَّع بعنوانِ الجدولِ المطلق
                                return false;
                            vtableAddrFixups_.push_back({code_.size() - 8, className});
                            if (!storeMemBase(x86::RAX, 0, x86::RDI)) // obj[0] = عنوانُ الجدول
                                return false;
                        }
                        return storeMem(memSlot_[inst.result->name], x86::RAX); // المؤشّرُ ⇒ خانةُ النتيجة
                    }
                    return true;
                }
                case OP::LOAD:
                {
                    if (!inst.result)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    // (AR) OBJECT_GET: LOAD بمعاملين [obj, اسمُ الحقل نصًّا] ⇒ mov dst, [obj+إزاحة].
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
                        if (!loadInto(x86::RAX, inst.operands[0]))
                            return false;
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return loadMemBase(dst, x86::RAX, off);
                    }
                    // (AR) %dst = load %slot ⇒ mov dst, [rbp+إزاحة]. ينشرُ صنفَ الكائن إن حمله المصدر.
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long disp;
                    if (!isMemVar(inst.operands[0], disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kLoadNonslot + detailOpcode(inst));
                    if (const std::string *cn = objClassForOperand(inst.operands[0]))
                        objClassOf_[inst.result->name] = *cn;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMem(dst, disp);
                }
                case OP::STORE:
                {
                    // (AR) OBJECT_SET: STORE بثلاثة معاملات [value, obj, اسمُ الحقل نصًّا] ⇒ mov [obj+إزاحة], value.
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
                        return loadInto(x86::RAX, inst.operands[1]) && // RAX=obj
                               loadInto(x86::RDI, inst.operands[0]) && // RDI=value
                               storeMemBase(x86::RAX, off, x86::RDI);
                    }
                    // (AR) store value, %slot ⇒ حمّل القيمةَ في RAX ثمّ mov [rbp+إزاحة], RAX. ينشرُ صنفَ الكائن.
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long disp;
                    if (!isMemVar(inst.operands[1], disp))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kStoreNonslot + detailOpcode(inst));
                    if (const std::string *cn = objClassForOperand(inst.operands[0]))
                        objClassOf_[inst.operands[1].name] = *cn;
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
                case OP::OBJECT_CALL:
                {
                    // (AR) الإرسالُ الافتراضيّ (الدفعة ٦): operands=[obj, اسمُ الطريقة(نصّ), وسائط…].
                    //      نُحمّل مؤشّرَ vtable من obj[0]، ثمّ مؤشّرَ الدالّةِ من [vtable+خانة×٨]،
                    //      ونُنادي غيرَ مباشرٍ (self=arg0). الفهرسُ من الصنفِ الساكن؛ الكائنُ زمنَ
                    //      التشغيل قد يكون ابنًا (جدولُه يحملُ التجاوزَ في نفسِ الخانة) ⇒ إرسالٌ صحيح.
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
                    if (argc + 1 > 6)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArgsGt6 + std::to_string(argc + 1));
                    // (AR) انسكِبْ الحيَّ + المعاملاتِ السجليّة (تُقرأ من خاناتها بعدَ الانسكاب).
                    for (const auto &kv : regOf_)
                    {
                        const bool live = common::usedAfterInBlock(block, instIdx, kv.first);
                        if (live || common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    }
                    // (AR) الوسائطُ الإضافيّة → rsi/rdx/… (abiArg_[1..])؛ لا تمسّ RDI/RAX.
                    for (size_t i = 0; i < argc; ++i)
                        if (!loadArgInto(abiArg_[i + 1], inst.operands[i + 2]))
                            return false;
                    // (AR) self → RDI (arg0). ثمّ مؤشّرُ vtable = [RDI+0]، ثمّ مؤشّرُ الدالّة =
                    //      [RAX+خانة×٨]. RAX/RDI خارجَ الحوض ⇒ لا يدهسان معاملًا حيًّا (درسُ الدفعة ٥).
                    if (!loadArgInto(x86::RDI, inst.operands[0]))
                        return false;
                    if (!loadMemBase(x86::RAX, x86::RDI, 0))
                        return false;
                    if (!loadMemBase(x86::RAX, x86::RAX, static_cast<long long>(slot) * 8))
                        return false;
                    if (!emitCallReg(x86::RAX)) // (AR) call rax (نداءٌ غيرُ مباشر)
                        return false;
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
                case OP::CLOSURE_CREATE:
                {
                    // (AR) الإغلاقات (الدفعة ٧): operands=[@دالّة, ملتقَط٠, ملتقَط١، …].
                    //      تخصيصٌ واحدٌ مدموج mmap(16 + N×8): [base]=عنوانُ الدالّة، [base+8]=مؤشّرُ
                    //      البيئة=base+16 (أو صفرٌ إن بلا التقاط)، [base+16+i×8]=الملتقَطُ i. النتيجةُ=base.
                    //      عنوانُ الدالّةِ يُرقَّع زمنَ الإنهاء (funcAddrFixups_). mmap يدهس الحوضَ ⇒ ننسِك.
                    if (!inst.result || inst.operands.empty() ||
                        inst.operands[0].type != sir::SIROperandType::FUNCTION)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const long long numCaptures = static_cast<long long>(inst.operands.size()) - 1;
                    const long long allocSize = 16 + numCaptures * 8;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    if (!emitMmap(allocSize)) // (AR) RAX = base (mmap يصفّر الكتلة)
                        return false;
                    // (AR) [base+0] = عنوانُ الدالّةِ المطلق (نائبٌ يُرقَّع).
                    if (!emitFuncAddr(x86::RDI, inst.operands[0].name) ||
                        !storeMemBase(x86::RAX, 0, x86::RDI))
                        return false;
                    if (numCaptures > 0)
                    {
                        // (AR) [base+8] = مؤشّرُ البيئة = base + 16.
                        if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, 16) ||
                            !storeMemBase(x86::RAX, 8, x86::RDI))
                            return false;
                        // (AR) [base+16+i×8] = الملتقَطُ i (يُقرأ من خانةِ انسكابه ⇒ صفرُ تصادم).
                        for (long long i = 0; i < numCaptures; ++i)
                            if (!loadArgInto(x86::RDI, inst.operands[static_cast<size_t>(i + 1)]) ||
                                !storeMemBase(x86::RAX, 16 + i * 8, x86::RDI))
                                return false;
                    }
                    else if (!movImm(x86::RDI, 0) || !storeMemBase(x86::RAX, 8, x86::RDI))
                        return false; // (AR) بلا التقاطٍ ⇒ مؤشّرُ البيئة = صفر
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::CLOSURE_CALL:
                {
                    // (AR) نداءُ إغلاق (الدفعة ٧): operands=[مؤشّرُ الإغلاق, وسيط٠, …]. البنيةُ {fn@0, env@8}.
                    //      نُحمّل الوسائطَ الصريحةَ في rdi/rsi/… ثمّ مؤشّرَ البيئةِ وسيطًا أخيرًا (عقدُ اللامدا:
                    //      __env هو المعاملُ الأخير)، ثمّ ننادي [الإغلاق+0] غيرَ مباشر.
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const size_t argc = inst.operands.size() - 1; // (AR) الوسائطُ الصريحة (عدا البيئة)
                    if (argc + 1 > 6)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArgsGt6 + std::to_string(argc + 1));
                    for (const auto &kv : regOf_)
                    {
                        const bool live = common::usedAfterInBlock(block, instIdx, kv.first);
                        if (live || common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    }
                    // (AR) الوسائطُ الصريحة → abiArg_[0..k-1] (تُقرأ من خاناتها ⇒ صفرُ تصادم).
                    for (size_t i = 0; i < argc; ++i)
                        if (!loadArgInto(abiArg_[i], inst.operands[i + 1]))
                            return false;
                    // (AR) مؤشّرُ الإغلاق → RAX (خارجَ الحوض ⇒ لا يدهس وسيطًا). ثمّ البيئةُ = [RAX+8]
                    //      → abiArg_[argc] (الوسيطُ الأخير)، ومؤشّرُ الدالّة = [RAX+0] → RAX ⇒ نداء.
                    if (!loadArgInto(x86::RAX, inst.operands[0]))
                        return false;
                    if (!loadMemBase(abiArg_[argc], x86::RAX, 8)) // (AR) البيئةُ وسيطًا أخيرًا
                        return false;
                    if (!loadMemBase(x86::RAX, x86::RAX, 0)) // (AR) مؤشّرُ الدالّة
                        return false;
                    if (!emitCallReg(x86::RAX))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    if (inst.result)
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movReg(dst, x86::RAX);
                    }
                    return true;
                }
                case OP::CALL_INDIRECT:
                {
                    // (AR) نداءٌ عبر مؤشّرِ دالّة (الدفعة ٧): operands=[مؤشّرُ الدالّة, وسيط٠, …].
                    //      لا بيئةَ (بخلاف CLOSURE_CALL). نُحمّل الوسائطَ ثمّ ننادي المؤشّرَ غيرَ مباشر.
                    if (inst.operands.empty())
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const size_t argc = inst.operands.size() - 1;
                    if (argc > 6)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArgsGt6 + std::to_string(argc));
                    for (const auto &kv : regOf_)
                    {
                        const bool live = common::usedAfterInBlock(block, instIdx, kv.first);
                        if (live || common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    }
                    for (size_t i = 0; i < argc; ++i)
                        if (!loadArgInto(abiArg_[i], inst.operands[i + 1]))
                            return false;
                    // (AR) مؤشّرُ الدالّة → RAX (خارجَ الحوض): معاملُ FUNCTION ⇒ عنوانٌ مُرقَّع؛ وإلّا سجلّ/خانة.
                    if (inst.operands[0].type == sir::SIROperandType::FUNCTION)
                    {
                        if (!emitFuncAddr(x86::RAX, inst.operands[0].name))
                            return false;
                    }
                    else if (!loadArgInto(x86::RAX, inst.operands[0]))
                        return false;
                    if (!emitCallReg(x86::RAX))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    if (inst.result)
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movReg(dst, x86::RAX);
                    }
                    return true;
                }
                case OP::ENV_LOAD:
                {
                    // (AR) تحميلُ ملتقَطٍ من البيئة (الدفعة ٧): operands=[%__env, فهرس]. dst = [env + فهرس×8].
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long idx;
                    if (!common::isConstInt(inst.operands[1], idx) || idx < 0)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    if (!loadInto(x86::RAX, inst.operands[0])) // RAX = مؤشّرُ البيئة (خارجَ الحوض)
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMemBase(dst, x86::RAX, idx * 8);
                }
                case OP::ENV_STORE:
                {
                    // (AR) تخزينُ ملتقَطٍ في البيئة (الدفعة ٧): operands=[قيمة, %__env, فهرس]. [env+فهرس×8]=قيمة.
                    if (inst.operands.size() != 3)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long idx;
                    if (!common::isConstInt(inst.operands[2], idx) || idx < 0)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    return loadInto(x86::RAX, inst.operands[1]) && // RAX = مؤشّرُ البيئة
                           loadInto(x86::RDI, inst.operands[0]) && // RDI = القيمة
                           storeMemBase(x86::RAX, idx * 8, x86::RDI);
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
                case OP::TUPLE_NEW: // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً وتخطيطًا ⇒ نفسُ الخفض (مطابقٌ لمسار LLVM instr_core_ops)
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
                case OP::TUPLE_GET: // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً ⇒ نفسُ خفضِ القراءة (بما فيه المسارُ المعلَّب)
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
                case OP::TUPLE_LEN: // (AR) الصفُّ يشاركُ المصفوفةَ بنيةً ⇒ الطولُ في [arr+0] كالمصفوفة
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
                case OP::BUILTIN_ARRAY_REMOVE:
                case OP::ARRAY_REMOVE:
                {
                    // (AR) حذفٌ في المكان: operands=[arr, index]. نطبّع الفهرسَ السالب (idx+=len)، نفحص
                    //      الحدَّ، ثمّ نُزيح data[i]=data[i+1] لـi من removeIdx حتّى len-2، ونُنقِص الطولَ.
                    //      لا mmap ⇒ لا خاناتِ خدشٍ عابرة؛ لكن نستعمل الحوضَ خدشًا في الحلقة ⇒ انسكابٌ حولَها.
                    //      النتيجة (إن وُجدت) = مؤشّرُ المصفوفة نفسه (مطابقٌ لمسار LLVM).
                    if (inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // RAX=arr، RDI=idx، RSI=len=[arr+0]. تطبيعُ السالب: إن idx<0 (موقَّع) ⇒ idx+=len.
                    if (!loadInto(x86::RAX, inst.operands[0]) || !loadInto(x86::RDI, inst.operands[1]) ||
                        !loadMemBase(x86::RSI, x86::RAX, kArrOffLen))
                        return false;
                    if (!cmpZero(x86::RDI))
                        return false;
                    size_t jgeSkipNeg;
                    if (!emitJccFwd(x86::mnem::kJge, jgeSkipNeg))
                        return false;
                    if (!addReg(x86::RDI, x86::RSI))
                        return false;
                    patchFwd(jgeSkipNeg);
                    if (!emitBoundsCheck()) // (AR) idx مقابل [arr+len] لا-موقَّعًا (السالبُ الباقي يلتفّ ⇒ هلع)
                        return false;
                    // R8 = len-1 (= الطولُ الجديد وحدُّ الحلقة العلويّ)؛ RDX = data=[arr+16].
                    if (!movReg(x86::R8, x86::RSI) || !subImm(x86::R8, 1) ||
                        !loadMemBase(x86::RDX, x86::RAX, kArrOffData))
                        return false;
                    // ── حلقةُ الإزاحة: لكلّ i (RDI) من removeIdx حتّى R8-1: data[i]=data[i+1] ──
                    const size_t remHead = code_.size();
                    if (!movReg(x86::RSI, x86::RDI)) // (AR) RSI=i؛ cmp i, len-1
                        return false;
                    if (!cmpRegReg(x86::RSI, x86::R8))
                        return false;
                    size_t jgeRemDone;
                    if (!emitJccFwd(x86::mnem::kJge, jgeRemDone))
                        return false;
                    // RCX = &data[i+1] = data + (i+1)×8 ⇒ R9 = [RCX]؛ ثمّ data[i] = R9.
                    if (!movReg(x86::R9, x86::RDI) || !addImm(x86::R9, 1) || !shlImm(x86::R9, 3) ||
                        !movReg(x86::RCX, x86::RDX) || !addReg(x86::RCX, x86::R9) ||
                        !loadMemBase(x86::R9, x86::RCX, 0))
                        return false;
                    if (!movReg(x86::RCX, x86::RDI) || !shlImm(x86::RCX, 3) || !movReg(x86::RSI, x86::RDX) ||
                        !addReg(x86::RSI, x86::RCX) || !storeMemBase(x86::RSI, 0, x86::R9))
                        return false;
                    if (!addImm(x86::RDI, 1) || !emitJccBack(x86::mnem::kJmp, remHead))
                        return false;
                    patchFwd(jgeRemDone);
                    // (AR) الطولُ الجديد = R8 (len-1) ⇒ [arr+len] = R8.
                    if (!storeMemBase(x86::RAX, kArrOffLen, x86::R8))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    if (inst.result)
                    {
                        int dst;
                        if (!allocReg(inst.result->name, dst))
                            return false;
                        return movReg(dst, x86::RAX);
                    }
                    return true;
                }
                case OP::ARRAY_CONCAT:
                {
                    // (AR) دمجُ مصفوفتين: operands=[arr1, arr2]. نُخصّص كتلةً واحدةً [رأسٌ ٤٠ | بياناتٌ
                    //      (len1+len2)×8]، ننسخ منطقتَي البيانات خامًا، ونضبط الطول/السعة/data/tags=null/homog.
                    //      المسارُ الساكن (tags=null) فقط: نرفض المعلَّبَ (Any) والمختلطَ النوعين صراحةً
                    //      (يلزمهما دمجُ مخزنِ الوسوم — مؤجَّلٌ). data=base+40 (مطابقٌ لـARRAY_NEW).
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    const types::SadTypeKind e0 = inst.operands[0].elementType;
                    const types::SadTypeKind e1 = inst.operands[1].elementType;
                    const bool bothKnown = e0 != types::SadTypeKind::Void && e1 != types::SadTypeKind::Void;
                    if (e0 == types::SadTypeKind::Any || e1 == types::SadTypeKind::Any ||
                        (bothKnown && e0 != e1))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kArrayConcatBoxed);
                    long long homogKind = kDynKindInt;
                    if (bothKnown && e0 == e1)
                        (void)dynTagForType(e0, homogKind); // (AR) نوعُ العنصر المتجانس (يُقرأ حين tags=null)
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) اقرأ len/data للمصفوفتين إلى خانات الخدش (تبقى عبر mmap): 0=len1 1=data1 2=len2 3=data2.
                    if (!loadInto(x86::RAX, inst.operands[0]) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffLen) || !storeMem(arrExtSlot(0), x86::RDI) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffData) || !storeMem(arrExtSlot(1), x86::RDI))
                        return false;
                    if (!loadInto(x86::RAX, inst.operands[1]) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffLen) || !storeMem(arrExtSlot(2), x86::RDI) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffData) || !storeMem(arrExtSlot(3), x86::RDI))
                        return false;
                    // totalLen = len1+len2 ⇒ خانة 4؛ size = 40 + totalLen×8 في RSI ⇒ mmap.
                    if (!loadMem(x86::RDI, arrExtSlot(0)) || !loadMem(x86::RCX, arrExtSlot(2)) ||
                        !addReg(x86::RDI, x86::RCX) || !storeMem(arrExtSlot(4), x86::RDI))
                        return false;
                    if (!movReg(x86::RSI, x86::RDI) || !shlImm(x86::RSI, 3) ||
                        !addImm(x86::RSI, kArrHeaderBytes) || !emitMmapPresetSize())
                        return false;
                    // (AR) RAX=base. اضبط الحقول: len=cap=totalLen، data=base+40، tags=null، homog=النوع.
                    if (!loadMem(x86::RDI, arrExtSlot(4)) || !storeMemBase(x86::RAX, kArrOffLen, x86::RDI) ||
                        !storeMemBase(x86::RAX, kArrOffCap, x86::RDI))
                        return false;
                    if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, kArrHeaderBytes) ||
                        !storeMemBase(x86::RAX, kArrOffData, x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, 0) || !storeMemBase(x86::RAX, kArrOffTags, x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, homogKind) || !storeMemBase(x86::RAX, kArrOffHomog, x86::RDI))
                        return false;
                    // ── نسخُ المنطقة ١: dst=base+40 (RDI)، src=data1، cnt=len1×8؛ RDI يتقدّم لنهايتها ──
                    if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, kArrHeaderBytes) ||
                        !loadMem(x86::RSI, arrExtSlot(1)) || !loadMem(x86::RCX, arrExtSlot(0)) ||
                        !shlImm(x86::RCX, 3) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    // ── نسخُ المنطقة ٢: dst=RDI (يتابع)، src=data2، cnt=len2×8 ──
                    if (!loadMem(x86::RSI, arrExtSlot(3)) || !loadMem(x86::RCX, arrExtSlot(2)) ||
                        !shlImm(x86::RCX, 3) || !byteCopy(x86::RDI, x86::RSI, x86::RCX, x86::R8))
                        return false;
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::ARRAY_ZIP:
                {
                    // (AR) زاوج: operands=[arr1, arr2]. النتيجةُ مصفوفةٌ بطول min(len1,len2)، كلُّ خانةٍ
                    //      مؤشّرٌ لمصفوفةِ زوجٍ {أ[i]، ب[i]} (الخاناتُ تُنسخ خامًا). حلقةٌ ذاتُ mmap داخليٍّ
                    //      لكلّ زوج ⇒ القيمُ العابرةُ (outLen/i/data1/data2/outArr/outData) في خانات الخدش.
                    //      homog للناتج = Array (مصفوفةٌ من مصفوفات، tags=null). خانات: 0=outLen 1=i 2=data1 3=data2 4=outArr 5=outData.
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    // (AR) 🔑 اقرأ كِلا المعاملين إلى الخانات مستعملًا RAX/RDI (خارج الحوض) **فقط** قبل
                    //      لمسِ أيّ سجلّ حوض: قراءةُ المعامل الثاني (loadInto) تقرأ سجلَّه الفيزيائيّ، فلو
                    //      دهسنا سجلَّ حوضٍ يحمله لقرأنا قيمةً خاطئة (فخّ ترتيبٍ كامن أطاح ZIP على ARM64).
                    //      خانات: 0=len1(مؤقّت) 2=data1؛ 1=len2(مؤقّت) 3=data2؛ ثمّ min ⇒ 0=outLen، وتُعادُ 1 لـi.
                    if (!loadInto(x86::RAX, inst.operands[0]) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffLen) || !storeMem(arrExtSlot(0), x86::RDI) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffData) || !storeMem(arrExtSlot(2), x86::RDI))
                        return false;
                    if (!loadInto(x86::RAX, inst.operands[1]) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffLen) || !storeMem(arrExtSlot(1), x86::RDI) ||
                        !loadMemBase(x86::RDI, x86::RAX, kArrOffData) || !storeMem(arrExtSlot(3), x86::RDI))
                        return false;
                    // outLen = min(len1,len2) (الحوضُ حرٌّ الآن): إن len1<=len2 أبقِ RDI، وإلّا RDI=RCX.
                    if (!loadMem(x86::RDI, arrExtSlot(0)) || !loadMem(x86::RCX, arrExtSlot(1)) ||
                        !cmpRegReg(x86::RDI, x86::RCX))
                        return false;
                    size_t jleKeep;
                    if (!emitJccFwd(x86::mnem::kJle, jleKeep))
                        return false;
                    if (!movReg(x86::RDI, x86::RCX))
                        return false;
                    patchFwd(jleKeep);
                    if (!storeMem(arrExtSlot(0), x86::RDI)) // outLen
                        return false;
                    // خصّص كتلةَ الناتج: size = 40 + outLen×8 ⇒ RAX=outArr.
                    if (!movReg(x86::RSI, x86::RDI) || !shlImm(x86::RSI, 3) ||
                        !addImm(x86::RSI, kArrHeaderBytes) || !emitMmapPresetSize())
                        return false;
                    if (!storeMem(arrExtSlot(4), x86::RAX)) // outArr
                        return false;
                    if (!loadMem(x86::RDI, arrExtSlot(0)) || !storeMemBase(x86::RAX, kArrOffLen, x86::RDI) ||
                        !storeMemBase(x86::RAX, kArrOffCap, x86::RDI))
                        return false;
                    if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, kArrHeaderBytes) ||
                        !storeMemBase(x86::RAX, kArrOffData, x86::RDI) || !storeMem(arrExtSlot(5), x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, 0) || !storeMemBase(x86::RAX, kArrOffTags, x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, kDynKindArray) || !storeMemBase(x86::RAX, kArrOffHomog, x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, 0) || !storeMem(arrExtSlot(1), x86::RDI)) // i=0
                        return false;
                    // ── الحلقة: بينما i<outLen ── / (AR) لكلّ i: أنشئ مصفوفةَ زوجٍ وخزّن مؤشّرها.
                    const size_t zipHead = code_.size();
                    if (!loadMem(x86::RDI, arrExtSlot(1)) || !cmpMemBase(x86::RDI, x86::RBP, arrExtSlot(0)))
                        return false;
                    size_t jgeZipDone;
                    if (!emitJccFwd(x86::mnem::kJge, jgeZipDone))
                        return false;
                    // خصّص زوجًا: size = 40 + 16 ⇒ RAX=pairBase؛ len=cap=2، data=pairBase+40، tags=null (homog=0 من mmap).
                    if (!movImm(x86::RSI, kArrHeaderBytes + 2 * kArrSlotBytes) || !emitMmapPresetSize())
                        return false;
                    if (!movImm(x86::RDI, 2) || !storeMemBase(x86::RAX, kArrOffLen, x86::RDI) ||
                        !storeMemBase(x86::RAX, kArrOffCap, x86::RDI))
                        return false;
                    if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, kArrHeaderBytes) ||
                        !storeMemBase(x86::RAX, kArrOffData, x86::RDI))
                        return false;
                    if (!movImm(x86::RDI, 0) || !storeMemBase(x86::RAX, kArrOffTags, x86::RDI))
                        return false;
                    // e1=data1[i]→R8، e2=data2[i]→R9 (i×8 في RCX).
                    if (!loadMem(x86::RCX, arrExtSlot(1)) || !shlImm(x86::RCX, 3))
                        return false;
                    if (!loadMem(x86::RSI, arrExtSlot(2)) || !addReg(x86::RSI, x86::RCX) ||
                        !loadMemBase(x86::R8, x86::RSI, 0))
                        return false;
                    if (!loadMem(x86::RSI, arrExtSlot(3)) || !addReg(x86::RSI, x86::RCX) ||
                        !loadMemBase(x86::R9, x86::RSI, 0))
                        return false;
                    // pairData = pairBase+40 ⇒ [pairData+0]=e1، [pairData+8]=e2.
                    if (!movReg(x86::RDI, x86::RAX) || !addImm(x86::RDI, kArrHeaderBytes) ||
                        !storeMemBase(x86::RDI, 0, x86::R8) || !storeMemBase(x86::RDI, kArrSlotBytes, x86::R9))
                        return false;
                    // outData[i] = pairBase (RAX).
                    if (!loadMem(x86::RSI, arrExtSlot(5)) || !loadMem(x86::RCX, arrExtSlot(1)) ||
                        !shlImm(x86::RCX, 3) || !addReg(x86::RSI, x86::RCX) ||
                        !storeMemBase(x86::RSI, 0, x86::RAX))
                        return false;
                    // i++ ثمّ عُد للرأس.
                    if (!loadMem(x86::RCX, arrExtSlot(1)) || !addImm(x86::RCX, 1) ||
                        !storeMem(arrExtSlot(1), x86::RCX) || !emitJccBack(x86::mnem::kJmp, zipHead))
                        return false;
                    patchFwd(jgeZipDone);
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMem(dst, arrExtSlot(4)); // (AR) النتيجة = outArr
                }
                case OP::ENUM_CONSTRUCT:
                {
                    // (AR) بناءُ تعدادٍ جبريّ (الدفعة ٥): operands=[String(اسم التعداد), ConstI64(tag), payload…].
                    //      قيمةُ التعداد = مؤشّرُ كومة [i64 tag@0 | SadDyn f0@8 | …]، SadDyn={kind@0،payload@8}.
                    //      نخصّص عبر mmap، نخزّن الوسمَ، ثمّ لكلّ حمولة (kind من نوعها + i64 قيمتها).
                    //      الحمولاتُ العدديّةُ kind=Int؛ float/string مؤجَّلةٌ (فشلٌ صريح). النتيجةُ = المؤشّر.
                    if (!inst.result || inst.operands.size() < 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long tagv;
                    if (!common::isConstInt(inst.operands[1], tagv))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    const long long nPayload = static_cast<long long>(inst.operands.size()) - 2;
                    // (AR) وسمُ كلّ حمولة (من نوعها المحدَّد؛ Any⇒Int افتراضًا للحمولة العدديّة).
                    std::vector<long long> kinds(static_cast<size_t>(nPayload), kDynKindInt);
                    for (long long i = 0; i < nPayload; ++i)
                    {
                        const auto &po = inst.operands[static_cast<size_t>(2 + i)];
                        long long k;
                        // (AR) حمولةٌ عشريّةٌ/نصّيّةٌ محدَّدة مؤجَّلة: GET_PAYLOAD يُرجِع i64 في GPR بلا سياق
                        //      xmm/طول ⇒ استهلاكُها عشريًّا/نصًّا يتباين. الحمولةُ العدديّة (Any/Integer/Bool/عدم) آمنة.
                        if (po.dataType == types::SadTypeKind::Float ||
                            po.dataType == types::SadTypeKind::String)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kEnumPayloadKind);
                        if (po.dataType != types::SadTypeKind::Any && dynTagForType(po.dataType, k))
                            kinds[static_cast<size_t>(i)] = k;
                        else if (po.dataType != types::SadTypeKind::Any &&
                                 po.dataType != types::SadTypeKind::Integer)
                            return fail(EC::INT_NATIVE_UNSUPPORTED, diag::kEnumPayloadKind);
                    }
                    // (AR) انسكِبْ المؤقّتاتِ الحيّةَ ومعامِلاتِ الحمولة (mmap يدهس الحوض؛ نقرؤها بعده من الانسكاب).
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first) ||
                            common::isPoolOperandOf(inst, kv.first, [this](const std::string &n){ return isMemName(n); }))
                            if (!storeMem(spillDisp(static_cast<size_t>(poolIndexOf(kv.second))), kv.second))
                                return false;
                    const long long total = kAdtPayloadBase + nPayload * kSadDynBytes;
                    if (!emitMmap(total)) // RAX = المؤشّر (خارجَ الحوض ⇒ ينجو)
                        return false;
                    if (!movImm(x86::RDI, tagv) || !storeMemBase(x86::RAX, kAdtTagOff, x86::RDI))
                        return false;
                    for (long long i = 0; i < nPayload; ++i)
                    {
                        const long long slotOff = kAdtPayloadBase + i * kSadDynBytes;
                        if (!movImm(x86::RDI, kinds[static_cast<size_t>(i)]) ||
                            !storeMemBase(x86::RAX, slotOff + kSadDynKindOff, x86::RDI))
                            return false;
                        if (!loadArgInto(x86::RDI, inst.operands[static_cast<size_t>(2 + i)]) ||
                            !storeMemBase(x86::RAX, slotOff + kSadDynPayloadOff, x86::RDI))
                            return false;
                    }
                    for (const auto &kv : regOf_)
                        if (common::usedAfterInBlock(block, instIdx, kv.first))
                            if (!loadMem(kv.second, spillDisp(static_cast<size_t>(poolIndexOf(kv.second)))))
                                return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return movReg(dst, x86::RAX);
                }
                case OP::ENUM_IS_VARIANT:
                {
                    // (AR) فحصُ حالةِ التعداد: operands=[obj, ConstI64(tag), String(اسم), ConstI64(isUnit)].
                    //      isUnit=1 ⇒ القيمةُ الوسمُ مباشرةً؛ 0 ⇒ حمّل [obj+0]. النتيجةُ منطقيّة (tag==المتوقَّع).
                    if (!inst.result || inst.operands.size() < 4)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long tagv, isUnit;
                    if (!common::isConstInt(inst.operands[1], tagv) ||
                        !common::isConstInt(inst.operands[3], isUnit))
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    if (isUnit == 0 && !loadMemBase(x86::RAX, x86::RAX, kAdtTagOff)) // RAX = الوسمُ من الكومة
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    // (AR) RDI = المتوقَّع؛ cmp RAX,RDI؛ sete al؛ movzx dst,al ⇒ منطقيّة ٠/١.
                    return movImm(x86::RDI, tagv) && cmpRegReg(x86::RAX, x86::RDI) &&
                           setccReg(x86::mnem::kSete, x86::RAX) && movzxReg(dst, x86::RAX);
                }
                case OP::ENUM_GET_PAYLOAD:
                {
                    // (AR) استخراجُ حمولةٍ: operands=[obj, ConstI64(fieldIdx), String(اسم)]. نحمّل الحمولةَ
                    //      i64 من خانةِ SadDyn (جزءُ payload). للنتيجة العدديّة؛ Any (قيمةٌ ديناميّة) مؤجَّل.
                    if (!inst.result || inst.operands.size() < 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    long long fieldIdx;
                    if (!common::isConstInt(inst.operands[1], fieldIdx) || fieldIdx < 0)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    // (AR) نحمّل جزءَ payload الـi64 من خانةِ SadDyn مباشرةً (الوسمُ يُتجاهَل): صحيحٌ
                    //      للحمولة العدديّة وللسياق المستهلِك لها i64 (mul.i64…). النتيجةُ Any تعني
                    //      «ديناميّة» لكنّ الجزءَ الرقميَّ يحمل القيمةَ؛ float/string الحقيقيّان (بناءُ
                    //      قيمةٍ موسومةٍ كاملة) مؤجَّلان — لا يُنتَجان في مسار الحمولة العدديّة الحاليّ.
                    if (!loadInto(x86::RAX, inst.operands[0]))
                        return false;
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    return loadMemBase(dst, x86::RAX, kAdtPayloadBase + fieldIdx * kSadDynBytes + kSadDynPayloadOff);
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

            // (AR) نداءٌ غيرُ مباشرٍ عبر سجلّ (call r64 = FF /2) — للإرسالِ الافتراضيّ. مؤشّرُ
            //      الدالّةِ في reg (مُحمَّلٌ من vtable). لا ترقيع (العنوانُ زمنَ التشغيلِ في السجلّ).
            bool emitCallReg(int reg)
            {
                return emit(x86::mnem::kCallIndirect, "r64", {x86::Operand::R(reg)});
            }
            // (AR) الإغلاقات (الدفعة ٧): يحمّل عنوانَ دالّةٍ ٦٤-بت في سجلّ (mov r64,imm64 نائبٌ)
            //      ويسجّل ترقيعًا يُحلُّ من funcOffset_ زمنَ الإنهاء.
            bool emitFuncAddr(int reg, const std::string &fnName)
            {
                if (!movImm64(reg, 0))
                    return false;
                funcAddrFixups_.push_back({code_.size() - 8, fnName});
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
