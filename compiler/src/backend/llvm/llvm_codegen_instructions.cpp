/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 *
 * هذا الملف يحتوي على تنفيذ مولد كود LLVM IR الذي يترجم SIR إلى LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 *
 * التزام ب STRICT_CODING_RULES.md:
 * - تم قراءة جميع الملفات الرأسية بالكامل (1746 سطر)
 * - تم توثيق كل استخدام API بموقع المصدر
 * - لا توجد دوال أو متغيرات مخترعة
 *
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2024
 * الإصدار (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "backend/generated/arch_specific_opcodes_generated.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h بدلاً من llvm/Support/TargetRegistry.h
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>
#include "bounds_checker.h" // (AR) فحص حدود موحَّد / (EN) unified bounds checking
#include "builtin_registry.h" // (AR) أسماء المدمجات القانونيّة لبوّابة الوضع الحرّ

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        // ====================================================================
        // (AR) بوّابة سلامة الوضع الحرّ — تصنيف المدمجات غير الآمنة:
        //      مدمجات نظام الملفّات ودخل الطرفيّة والتحكّم الطرفيّ/الإخراج تُصدر رموز
        //      libc (fopen/scanf/system...) غائبة في --freestanding، فكانت تُترجَم
        //      بنجاح ثمّ تفشل زمن الربط برسالة غامضة. هذا التصنيف يسمّي الدالّة
        //      القانونيّة (من SoT: Names) ليُصدر التشخيص المبكّر SEM019 اسمًا واضحًا.
        //      ملاحظة: «اطبع» آمنة حرًّا (تُوجَّه لمنافذ) فلا تُبوَّب. سلسلة فارغة = آمنة.
        //      عمدًا لم يُبوَّب أيضًا (بالدليل):
        //      - BUILTIN_DEBUG (تنقيح): مُصدره (emitBuiltinDebug) يستدعي printf فقط،
        //        وحرًّا تُحقَن نسخة printf تسلسليّة داخل الوحدة (emitFreestandingPrintf
        //        تدعم ‎%lld/%s/%g‎ المستعملة فيه) ⇒ مساره الحرّ سليم بلا رمز libc
        //        خارجيّ. فوق ذلك الواجهة الأماميّة لا تُنتج هذا الـopcode اليوم
        //        (لا اسم SoT له أصلًا).
        //      - BUILTIN_ASSERT (تأكيد؛ opcode بلا مُنتِج — «تأكد» تُخفَض إلى
        //        BUILTIN_SECURITY_ASSERT) وعائلة التأكيدات الحيّة SECURITY_ASSERT/
        //        PANIC/ASSERT_EQUAL/ASSERT_GREATER: التأكيد مفهوم أصيل في النوى،
        //        ومساره الحرّ سليم بعد توجيه abort (رمز libc غائب حرًّا) إلى
        //        __sad_panic (weak؛ النواة تتجاوزه) — انظر builtin_funcs_ops.cpp
        //        وsecurity_builtins_ops.cpp. (مسار ASSERT_EQUAL النصّيّ يضمَّن حرًّا
        //        بـstrcmp داخل الوحدة بدل رمز runtime المستضاف.)
        //      استثناء من العائلة: BUILTIN_SECURITY_ASSERT_TYPE (تأكد_نوع) **يُبوَّب**
        //      أدناه — مُصدره يستدعي sad_security_assert_type من runtime المستضاف
        //      (sad_embedded_runtime.c) الذي لا يُربط مع ‎-nostdlib‎ ⇒ فشل ربط غامض.
        //      ⚠️ قائمة منع محافِظة (لا سماح): ما ليس هنا يُترك كما هو (لا انحدار).
        // (EN) Freestanding-safety classifier: filesystem/stdin and terminal-control
        //      /output builtins emit libc symbols (fopen/scanf/system...) absent in
        //      --freestanding. Returns the canonical (SoT) name for unsafe opcodes so
        //      SEM019 can name it. Note: «print» is freestanding-safe (routed to ports)
        //      and is not gated. Empty = safe; conservative deny-list.
        // ====================================================================
        static std::string freestandingUnsafeBuiltinName(SIROpcode op)
        {
            namespace Nb = Sad::Builtins::Names::Basics;
            namespace Nc = Sad::Builtins::Names::Core;
            namespace Nio = Sad::Builtins::Names::CompilerIo;
            namespace Nm = Sad::Builtins::Names::Math;
            namespace Na = Sad::Builtins::Names::Assertions;
            namespace Ncr = Sad::Builtins::Names::Crypto;
            namespace Nar = Sad::Builtins::Names::Arrays;
            namespace Nasync = Sad::Builtins::Names::AsyncAdvanced;
            switch (op)
            {
            case SIROpcode::BUILTIN_READ:            return std::string(Nc::READ);        // اقرأ (دخل قياسيّ)
            // (AR) قراءة_سطر: دخل قياسيّ كـ«اقرأ» تمامًا، يُصدر scanf/getchar/strdup
            //      (libc غائبة حرًّا) ⇒ يُبوَّب كي يعطي SEM019 نظيفًا لا فشل ربط غامضًا.
            // (EN) read-line: stdin like «اقرأ»; emits scanf/getchar/strdup (libc,
            //      absent freestanding) ⇒ gated for a clean SEM019, not an opaque link error.
            case SIROpcode::BUILTIN_READ_LINE:       return std::string(Nio::IO_1);       // قراءة_سطر (دخل قياسيّ)
            // (AR) مسح_الشاشة: تحكّم طرفيّة يُصدر نداء system("cls"/"clear") — رمز
            //      libc غائب حرًّا فيكسر الربط تمامًا كمدمجات الملفّات/الدخل. توسعةً
            //      لميثاق البوّابة نحو الإخراج/الطرفيّة، يُبوَّب كي يعطي SEM019 نظيفًا.
            // (EN) clear-screen: terminal control emitting system("cls"/"clear") — a
            //      libc symbol absent freestanding, breaking the link exactly like the
            //      file/stdin builtins. Extends the gate's charter to output/terminal.
            case SIROpcode::BUILTIN_CLEAR_SCREEN:    return std::string(Nio::IO_2);       // مسح_الشاشة (تحكّم طرفيّة)
            case SIROpcode::BUILTIN_FILE_READ:       return std::string(Nb::READ_FILE);
            case SIROpcode::BUILTIN_FILE_WRITE:      return std::string(Nb::WRITE_FILE);
            case SIROpcode::BUILTIN_FILE_APPEND:     return std::string(Nb::APPEND_FILE);
            case SIROpcode::BUILTIN_FILE_DELETE:     return std::string(Nb::DELETE_FILE);
            case SIROpcode::BUILTIN_FILE_COPY:       return std::string(Nb::COPY_FILE);
            case SIROpcode::BUILTIN_FILE_MOVE:       return std::string(Nb::MOVE_FILE);
            case SIROpcode::BUILTIN_FILE_CREATE_DIR: return std::string(Nb::MKDIR);
            case SIROpcode::BUILTIN_FILE_LIST_DIR:   return std::string(Nb::LIST_DIR);
            // (AR) عشوائي: مُصدره (emitBuiltinRandom) يستدعي rand() — رمز libc غائب
            //      حرًّا فيكسر الربط برسالة غامضة، ولا مصدر عشوائيّة طبيعيّ على المعدن
            //      العاري (مولّد حرّ ببذرة/حالة/عتاد = قرار تصميم بمستوى RFC لا رقعة
            //      توليد). يُبوَّب كي يعطي SEM019 نظيفًا يسمّيها زمن الترجمة.
            // (EN) random: its emitter (emitBuiltinRandom) calls rand() — a libc
            //      symbol absent freestanding, breaking the link opaquely; bare metal
            //      has no natural entropy source (a freestanding PRNG with a seeding
            //      policy is RFC-level design, not a codegen patch) ⇒ gated.
            case SIROpcode::BUILTIN_RANDOM:          return std::string(Nm::RANDOM);      // عشوائي (عشوائيّة مستضافة)
            // (AR) تأكد_نوع: مُصدره يستدعي sad_security_assert_type — دالّة من runtime
            //      المستضاف المضمَّن (sad_embedded_runtime.c) لا رمز libc، والوضع الحرّ
            //      يربط بـ‎-nostdlib‎ بلا هذا الـruntime ⇒ فشل ربط غامض (بخلاف بقيّة
            //      عائلة التأكيد المضمّنة داخل الوحدة). يُبوَّب حتّى يملك مسارًا حرًّا
            //      مضمَّنًا (يتطلّب أوّلًا إصلاح دلالته المستضافة — انظر تقرير المراجعة).
            // (EN) assert_type: its emitter calls sad_security_assert_type — a hosted
            //      embedded-runtime helper (not libc) that -nostdlib never links ⇒
            //      opaque link failure (unlike the rest of the assert family, which
            //      is inlined in-module). Gated until it gets an inlined
            //      freestanding path (its hosted semantics need fixing first).
            case SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE: return std::string(Na::ASSERT_TYPE); // تأكد_نوع (runtime مستضاف)
            // (AR) نم/نوم_غير_متزامن: مُصدراهما يستدعيان sad_rt_sleep_ms من runtime
            //      الخيوط المستضاف (Win32/pthread) الذي لا يُربط مع ‎-nostdlib‎ ⇒ فشل
            //      ربط غامض. النوم على المعدن العاري يحتاج مؤقّتًا عتاديًّا (PIT/HPET)
            //      — قرار نواة لا رقعة توليد. يُبوَّبان.
            // (EN) sleep/async-sleep: both emit sad_rt_sleep_ms from the hosted
            //      threading runtime (never linked under -nostdlib) ⇒ opaque link
            //      failure. Bare-metal sleep needs a hardware timer (kernel design,
            //      not a codegen patch) ⇒ gated.
            case SIROpcode::BUILTIN_SLEEP:           return std::string(Nio::IO_0);       // نم (runtime مستضاف)
            case SIROpcode::ASYNC_SLEEP:             return std::string(Nasync::ASYNC_SLEEP); // نوم_غير_متزامن (runtime مستضاف)
            // (AR) اخرج: يستدعي exit — دلالة «إنهاء عمليّة» مستضافة (نظام تشغيل يستلم
            //      الرمز)؛ على المعدن العاري لا عمليّة أصلًا وexit رمز libc غائب.
            //      الإيقاف الحرّ الصحيح (hlt/إيقاف طاقة) قرار نواة. يُبوَّب.
            // (EN) exit: hosted process-termination semantics (an OS receives the
            //      code); bare metal has no process and exit is an absent libc symbol.
            //      The right freestanding halt is a kernel decision ⇒ gated.
            case SIROpcode::BUILTIN_EXIT:            return std::string(Nb::EXIT_ALT);    // اخرج (إنهاء عمليّة مستضاف)
            // (AR) عائلة libm: هذه المُصدرات تُصدر نداء مكتبة الرياضيّات المستضافة
            //      (sin/cos/log/floor... — نداء رمز لا intrinsic؛ مؤكَّد بفحص المُصدرات)
            //      الغائبة مع ‎-nostdlib‎ ⇒ فشل ربط غامض. libm حرّة (دقّة/مدى/جداول)
            //      قرار بمستوى RFC لا رقعة توليد. ملاحظة: «جذر» و«مطلق» ليسا هنا —
            //      مُصدراهما صارا intrinsics (llvm.sqrt/llvm.fabs تُخفَض لتعليمة عتاد
            //      x87/SSE على x86 بلا رمز libm)، و«أس» له نسخة حرّة داخل الوحدة
            //      (emitFreestandingPow) و«باقي» يُخفَض إلى MOD_I64 — لا تُبوَّب.
            // (EN) libm family: these emitters issue hosted libm symbol calls (not
            //      intrinsics; verified in the emitters), absent under -nostdlib. A
            //      freestanding libm is RFC-level design. Note: sqrt/abs are now
            //      llvm.sqrt/llvm.fabs intrinsics (hardware-lowered on x86), pow has
            //      an in-module freestanding version (emitFreestandingPow), and fmod
            //      lowers to MOD_I64 — none of those gated.
            case SIROpcode::BUILTIN_LOG:             return std::string(Nm::LOG);         // لوغ (libm)
            case SIROpcode::BUILTIN_LOG2:            return std::string(Nm::LOG2);        // لوغ2 (libm)
            case SIROpcode::BUILTIN_LOG10:           return std::string(Nm::LOG10);       // لوغ10 (libm)
            case SIROpcode::BUILTIN_SIN:             return std::string(Nm::SIN);         // جيب (libm)
            case SIROpcode::BUILTIN_COS:             return std::string(Nm::COS);         // جتا (libm)
            case SIROpcode::BUILTIN_TAN:             return std::string(Nm::TAN);         // ظل (libm)
            case SIROpcode::BUILTIN_ASIN:            return std::string(Nm::ASIN);        // معكوس_جيب (libm)
            case SIROpcode::BUILTIN_ACOS:            return std::string(Nm::ACOS);        // معكوس_جتا (libm)
            case SIROpcode::BUILTIN_ATAN:            return std::string(Nm::ATAN);        // معكوس_ظل (libm)
            case SIROpcode::BUILTIN_FLOOR:           return std::string(Nm::FLOOR);       // أرضية (libm)
            case SIROpcode::BUILTIN_CEIL:            return std::string(Nm::CEIL);        // سقف (libm)
            case SIROpcode::BUILTIN_ROUND:           return std::string(Nm::ROUND);       // تقريب (libm)
            case SIROpcode::BUILTIN_TRUNC:           return std::string(Nm::TRUNCATE);    // اقتطاع (libm)
            // (AR) رتب (المدمجة وطريقة .رتب): المُصدر يستدعي qsort — رمز libc غائب
            //      حرًّا. فرز حرّ داخل الوحدة رقعة توليد غير رخيصة (حلقات مقارنة/تبديل
            //      لكلّ نوع عنصر) ⇒ يُبوَّب حتّى تُنفَّذ نسخة حرّة مدروسة.
            // (EN) sort (builtin + .sort method): the emitter calls qsort — an absent
            //      libc symbol freestanding. An in-module freestanding sort is a
            //      non-trivial codegen patch ⇒ gated until designed.
            case SIROpcode::BUILTIN_ARRAY_SORT:      return std::string(Nar::SORT);       // رتب (qsort)
            // (AR) الآن: لم يعُد مُبوَّبًا حرًّا. مُصدره يستدعي `time`، ويُحلّ حسب بيئة
            //      الجسر (classifyHwBridgeProfile/emitFreestandingTime في
            //      freestanding_ops.cpp) بأربع حالات لا حالتين:
            //        • معدن x86 ⇒ ساعة CMOS الحقيقيّة (منافذ 0x70/0x71 بلا libc).
            //        • لينكس (x86_64/i386) ⇒ نداء نظام `time` مبثوث داخل الوحدة —
            //          يعمل مع libc وبدونها؛ لا CMOS (منافذ في الحلقة 3 = #GP⇒SIGSEGV).
            //        • معدن بمعمارية أخرى ⇒ كعب يعيد صفرًا يتجاوزه دعم اللوحة.
            //        • نظام آخر (ويندوز/ماك) ⇒ يُترَك خارجيًّا يحلّه CRT.
            //      التعليل القديم «لا ساعة جدار على المعدن» بائت: الجسور كلّها موجودة.
            // (EN) now/timestamp: no longer gated freestanding. Its emitter calls
            //      `time`, resolved per bridge profile (classifyHwBridgeProfile /
            //      emitFreestandingTime in freestanding_ops.cpp) across four cases,
            //      not two: x86 bare metal ⇒ the CMOS RTC (ports 0x70/0x71, no libc);
            //      Linux (x86_64/i386) ⇒ an in-module `time` syscall (works with and
            //      without libc; no CMOS — ports in ring 3 = #GP→SIGSEGV); other bare
            //      metal ⇒ a zero stub for the BSP; another OS (Windows/macOS) ⇒ left
            //      external for the CRT. The old «no bare-metal wall clock» is stale.
            // (AR) عشوائي_آمن: يستدعي BCryptGenRandom (خدمة نظام تشغيل) — غائبة حرًّا
            //      بداهةً، ولا مصدر عشوائيّة معياريّ على المعدن (نفس ميثاق «عشوائي»).
            // (EN) secure-random: calls BCryptGenRandom (an OS service) — absent
            //      freestanding; no standard bare-metal entropy (same charter as rand).
            case SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM: return std::string(Na::SECURE_RANDOM); // عشوائي_آمن (خدمة OS)
            // (AR) عائلة الأمن المستضافة: مُصدراتها تستدعي sad_security_* من runtime
            //      المستضاف المضمَّن (sad_embedded_runtime.c) الذي لا يُربط مع
            //      ‎-nostdlib‎ — علّة «تأكد_نوع» نفسها أعلاه. تُبوَّب حتّى تُضمَّن حرًّا.
            // (EN) Hosted security family: emitters call sad_security_* helpers from
            //      the hosted embedded runtime (never linked under -nostdlib) — same
            //      failure mode as assert_type above. Gated until inlined freestanding.
            case SIROpcode::BUILTIN_SECURITY_HASH:          return std::string(Na::HASH);          // هاش (runtime مستضاف)
            case SIROpcode::BUILTIN_SECURITY_ENCRYPT:       return std::string(Na::ENCRYPT);       // شفر (runtime مستضاف)
            case SIROpcode::BUILTIN_SECURITY_DECRYPT:       return std::string(Na::DECRYPT);       // فك_تشفير (runtime مستضاف)
            case SIROpcode::BUILTIN_SECURITY_SANITIZE:      return std::string(Na::SANITIZE);      // نظف (runtime مستضاف)
            case SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE: return std::string(Na::BASE64_ENCODE); // ترميز_64 (runtime مستضاف)
            // (AR) نفس بوّابة عائلة الأمن المستضافة أعلاه — بلايك3/هاش_مفتاح
            //      يستدعيان sad_blake3_* من نفس runtime المستضاف المضمَّن.
            // (EN) Same hosted-runtime gate as the security family above —
            //      BLAKE3 functions call into the same embedded runtime.
            case SIROpcode::BUILTIN_CRYPTO_BLAKE3_HASH:       return std::string(Ncr::BLAKE3_HASH);       // بلايك3 (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_BLAKE3_KEYED_HASH: return std::string(Ncr::BLAKE3_KEYED_HASH); // هاش_مفتاح (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_KDF_PBKDF2:        return std::string(Ncr::KDF_PBKDF2);        // اشتق_مفتاح_مرور (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_KDF_HKDF:          return std::string(Ncr::KDF_HKDF);          // اشتق_مفتاح (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_AEAD_ENCRYPT:      return std::string(Ncr::AEAD_ENCRYPT);      // شفّر_موثّق (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_AEAD_DECRYPT:      return std::string(Ncr::AEAD_DECRYPT);      // فك_تشفير_موثّق (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_KDF_ARGON2ID:      return std::string(Ncr::KDF_ARGON2ID);      // أرجون2 (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_X25519_KEYGEN_PRIV:  return std::string(Ncr::X25519_KEYGEN_PRIV);  // ولّد_مفتاح_خاص_x25519 (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_X25519_DERIVE_PUB:   return std::string(Ncr::X25519_DERIVE_PUB);   // اشتق_مفتاح_عام_x25519 (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_X25519_EXCHANGE:     return std::string(Ncr::X25519_EXCHANGE);     // تبادل_مفتاح (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_ED25519_KEYGEN_PRIV: return std::string(Ncr::ED25519_KEYGEN_PRIV); // ولّد_مفتاح_خاص_توقيع (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_ED25519_DERIVE_PUB:  return std::string(Ncr::ED25519_DERIVE_PUB);  // اشتق_مفتاح_عام_توقيع (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_ED25519_SIGN:        return std::string(Ncr::ED25519_SIGN);        // وقّع (runtime مستضاف)
            case SIROpcode::BUILTIN_CRYPTO_ED25519_VERIFY:      return std::string(Ncr::ED25519_VERIFY);      // تحقق_توقيع (runtime مستضاف)
            default:                                 return std::string();
            }
        }

        void LLVMCodeGen::emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc)
        {
            if (!sirFunc || !llvmFunc)
            {
                return;
            }

            // Source: SIRFunction::getParameters() is at sir_module.h:293
            const auto &params = sirFunc->getParameters();

            unsigned idx = 0;
            for (auto &arg : llvmFunc->args())
            {
                if (Sad::Security::BoundsChecker::checkArrayIndex(idx, params.size()))
                {
                    // Source: SIRParameter::name is PUBLIC member at sir_module.h:229
                    std::string paramName = params[idx].name;

                    // إنشاء alloca للمعامل
                    // Create alloca for parameter
                    llvm::IRBuilder<> tmpBuilder(&llvmFunc->getEntryBlock(),
                                                 llvmFunc->getEntryBlock().begin());
                    llvm::AllocaInst *alloca = tmpBuilder.CreateAlloca(
                        arg.getType(),
                        nullptr,
                        paramName);

                    // تخزين قيمة المعامل
                    // Store parameter value
                    tmpBuilder.CreateStore(&arg, alloca);

                    // حفظ في القيم المسماة
                    // Save to named values
                    // Source: context_info_.namedValues is at llvm_codegen.h:617
                    // (AR) نحفظ بكلا الاسمين: بدون % وبادئة % لأن SIR builder يسجل
                    //      المعاملات كسجلات بادئة % (sir_builder.cpp:271)
                    // (EN) Store under both names: without % and with % prefix because
                    //      the SIR builder registers parameters as %param_name registers
                    //      (sir_builder.cpp:271)
                    context_info_.namedValues[paramName] = alloca;
                    context_info_.namedValues["%" + paramName] = alloca;
                }
                idx++;
            }
        }

        /**
         * إنشاء كتلة أساسية فارغة
         * Create empty basic block
         *
         * Source: llvm_codegen.h:400
         * @param name اسم الكتلة / Block name
         * @param function الدالة المحتوية / Containing function
         * @return كتلة LLVM الأساسية / LLVM basic block
         */
        llvm::BasicBlock *LLVMCodeGen::createBasicBlock(const std::string &name, llvm::Function *function)
        {
            if (!function)
            {
                reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR, {{"detail", "Null"}});
                return nullptr;
            }

            // Source: context_ is defined at llvm_codegen.h:631
            return llvm::BasicBlock::Create(*context_, name, function);
        }

        // ============================================================================
        // ============================================================================
        // Dispatcher — يوزع على ثلاث طبقات فرعية (Strangler Fig v3.1)
        // ============================================================================

        /**
         * (AR) تنفيذ تعليمة SIR — dispatcher رئيسي يوزع على ثلاث طبقات:
         *      1. emitInstructionCore      — الجوهر (حساب، async، كائنات، إلخ)
         *      2. emitInstructionLowlevel  — مستوى منخفض (CPU، UEFI، إلخ)
         *      3. emitInstructionPlatform  — منصات (Android، UI، توجيهات)
         * (EN) Emit SIR instruction — main dispatcher that delegates to three tiers:
         *      1. emitInstructionCore      — core (arithmetic, async, objects, etc.)
         *      2. emitInstructionLowlevel  — low-level (CPU, UEFI, etc.)
         *      3. emitInstructionPlatform  — platform (Android, UI, directives)
         */
        llvm::Value *LLVMCodeGen::emitInstruction(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
            {
                reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR, {{"detail", "Null"}});
                return nullptr;
            }

            // (AR) بوّابة الوضع الحرّ: أوقِف مدمجة غير آمنة (ملفّات/دخل قياسيّ) زمن
            //      الترجمة بتشخيص واضح يسمّيها (SEM019) بدل تركها تُصدر رمز libc
            //      غائبًا فيفشل الربط لاحقًا برسالة غامضة. توضع هنا (قبل الطبقات) كي
            //      لا يُفسَّر إيقافها «opcode غير مدعوم» (سقوط عبر كلّ الطبقات).
            //      نربط السجلّ الناتج بمؤشّر باطل فلا تتتالى «سجلّ غير معرَّف» على
            //      مستهلكيه؛ والبناء يُحبَط عبر بوّابة hasErrors الحرّة في السائق.
            // (EN) Freestanding gate: reject an unsafe (filesystem/stdin) builtin
            //      at compile time with a named diagnostic (SEM019) instead of
            //      emitting an absent libc symbol. Placed before the tiers so the
            //      early-out isn't mistaken for "unsupported opcode". The result
            //      register is bound to null to avoid cascading "undefined
            //      register" errors; the build aborts via the driver's gate.
            if (freestanding_)
            {
                std::string unsafeName = freestandingUnsafeBuiltinName(inst->opcode);
                if (!unsafeName.empty())
                {
                    reportError(::Sad::Errors::ErrorCode::SEM_FREESTANDING_BUILTIN,
                                {{"name", unsafeName}});
                    if (inst->result.has_value())
                    {
                        auto *ptrTy = llvm::PointerType::getUnqual(*context_);
                        context_info_.namedValues[inst->result->name] =
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(ptrTy));
                    }
                    return nullptr;
                }
            }

            // (AR) بوّابةُ معماريّة الهدف: أوپكوداتٌ يبثّ خفضُها أسمبليًّا مضمَّنًا
            //      لا يفهمه إلّا معالجٌ من عائلةٍ بعينها (`cli`، `outb`، `mov %crN`،
            //      `rdtsc`…). كان المترجم يبثّها **لأيّ هدفٍ يُطلَب** بخروجٍ صفريّ:
            //      `عداد_الدورات()` بـ`--هدف=aarch64-unknown-elf` يخرج بصفر ويبثّ
            //      `rdtsc` — فيقع الإخفاقُ عند المُجمِّع برسالةٍ لا تدلّ على السبب،
            //      أو لا يقع فيخرج ثنائيٌّ لا يعمل. القائمةُ من مصدر الحقيقة
            //      (language-truth/backend/arch_specific_opcodes.yaml) لا مكتوبةً هنا.
            //      توضع قبل الطبقات للسبب نفسِه الذي وُضِعت لأجله بوّابةُ الوضع
            //      الحرّ: كي لا يُقرأ الإيقافُ «opcode غير مدعوم».
            // (EN) Target-architecture gate: opcodes whose lowering emits inline asm
            //      only one CPU family understands. The list comes from the SoT, not
            //      from this file. Placed before the tiers so the early-out is not
            //      misread as "unsupported opcode".
            if (const auto *constraint = ::Sad::Backend::findArchConstraint(inst->opcode))
            {
                // (AR) الاسمُ **القانونيّ** للمعماريّة لا مكوّنُ الثالوث الخام:
                //      `getArchName()` تعيد ما كُتب («i686»)، فقائمةُ العائلة
                //      لا تطابقه ⇒ يُرفَض i686 وهو من x86. و`getArchTypeName`
                //      تعيد اسمًا واحدًا لكلّ صيغِ العائلة («i386»).
                // (EN) Canonical arch name, not the raw triple component:
                //      getArchName() returns "i686" as written, which no family
                //      list matches — rejecting i686 though it *is* x86.
                const std::string targetArch =
                    llvm::Triple::getArchTypeName(
                        llvm::Triple(llvm::Triple::normalize(module_->getTargetTriple()))
                            .getArch())
                        .str();
                if (!::Sad::Backend::archSatisfiesFamily(constraint->familyId, targetArch))
                {
                    // (AR) الصياغةُ في الكتالوج لا هنا — كبوّابةِ الوضعِ الحرِّ
                    //      أعلاه: نصٌّ مبنيٌّ في C++ يجعل الرسالةَ أحاديّةَ اللغة
                    //      حتمًا مهما كان `brief.en`.
                    //      🔴 دَينان مُعلَنان في هذا التشخيص، كلاهما أوسعُ من هذه
                    //      البوّابة فلا يُسَدّان هنا:
                    //      (١) `{opcode}` اسمُ أوپكودِ SIR (`BUILTIN_RDTSC`) لا الاسمُ
                    //          الذي كتبه المستعمل (`عداد_الدورات`). والوصلُ بينهما
                    //          يحتاج قرارًا لا نسخًا: `cpp_id` في `builtins/*.yaml`
                    //          لا يطابق اسمَ الأوپكود ١:١ (`CPU_11` مقابل
                    //          `BUILTIN_PORT_WRITE_16`)، ونسخُ الاسم العربيّ إلى جدولنا
                    //          يجعل الحقيقةَ في موضعين.
                    //      (٢) لا موقعَ مصدرٍ (سطرًا وعمودًا) — و`reportError` في
                    //          `llvm_codegen_context.h` لا تقبل موقعًا أصلًا، فالنقصُ
                    //          في مسار أخطاء الخلفيّة كلِّه لا في هذه البوّابة.
                    reportError(
                        ::Sad::Errors::ErrorCode::SEM_TARGET_ARCH_UNSUPPORTED_BUILTIN,
                        {{"opcode", std::string(constraint->opcode)},
                         {"witness", std::string(constraint->witness)},
                         {"family", std::string(constraint->familyAr)},
                         {"target", targetArch}});
                    // (AR) اربط الناتجَ بصفرٍ ثابت كما تفعل بوّابةُ الوضع الحرّ فوقها:
                    //      الرجوعُ عاريًا يترك سجلَّ النتيجة غيرَ معرَّفٍ فيتلوه بلاغُ
                    //      «خطأ مترجم داخليّ: Undefined register» يطلب من المستعمل
                    //      الإبلاغَ عن علّةٍ لا وجودَ لها. البناءُ يُحبَط عبر بوّابة
                    //      الرمز في السائق لا عبر هذا الإرجاع.
                    // (EN) Bind a zero result as the freestanding gate above does:
                    //      returning bare leaves the result register undefined, so a
                    //      spurious "internal compiler error" follows the real
                    //      diagnostic. The build aborts via the driver's code gate.
                    // (AR) i64: أكثرُ المقيَّدين بلا نتيجةٍ أصلًا (`cli`/`hlt`/`outb`…)،
                    //      وكلُّ **ذي نتيجةٍ** منهم اليومَ يربطها i64 (منافذُ الدخل،
                    //      معرّفُ المعالج، التسلسليّ، المؤقّت، عدّادُ الدورات، سجلّاتُ
                    //      التحكّم والنموذج). مقيَّدٌ جديدٌ نتيجتُه أضيقُ يحتاج نوعَه.
                    if (inst->result.has_value())
                    {
                        context_info_.namedValues[inst->result->name] =
                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0);
                    }
                    return nullptr;
                }
            }

            // (AR) حارس تمييز الإشارة (sentinel): بعض المعالجات تبلّغ خطأً حقيقيًّا ثمّ
            //      تعيد nullptr (نفس قيمة «opcode غير معالَج ⇒ جرّب الطبقة التالية»)،
            //      فكان الموزّع يسقط عبر الطبقات ويطبع «Unsupported opcode» مضلِّلًا
            //      فوق الخطأ الأصليّ (النمط المثبَت في emitBuiltinSum/emitBuiltinClearScreen
            //      قبل #185/#188). نلتقط عدد الأخطاء قبل التوزيع؛ إن عاد nullptr وقد
            //      زاد العدد فالخطأ أُبلغ فعلًا — نتوقّف بلا بلاغ opcode زائف.
            // (EN) Sentinel-disambiguation guard: some handlers report a real error
            //      then return nullptr (the same value as "unhandled ⇒ try next
            //      tier"), so the dispatcher fell through and printed a misleading
            //      "Unsupported opcode" on top of the original error. Capture the
            //      error count before dispatch; if a tier returns nullptr while the
            //      count grew, a real error was reported — stop without the spurious
            //      opcode report.
            const size_t errorsBeforeDispatch = getErrors().size();
            auto reportedRealError = [&]() -> bool
            { return getErrors().size() > errorsBeforeDispatch; };

            // (AR) الطبقة الأولى: التعليمات الجوهرية
            // (EN) Tier 1: Core instructions
            if (llvm::Value *result = emitInstructionCore(inst))
                return result;
            if (reportedRealError())
                return nullptr;

            // (AR) الطبقة الثانية: تعليمات المستوى المنخفض
            // (EN) Tier 2: Low-level instructions
            if (llvm::Value *result = emitInstructionLowlevel(inst))
                return result;
            if (reportedRealError())
                return nullptr;

            // (AR) الطبقة الثالثة: تعليمات المنصات والتوجيهات
            // (EN) Tier 3: Platform and directive instructions
            if (llvm::Value *result = emitInstructionPlatform(inst))
                return result;
            if (reportedRealError())
                return nullptr;

            // (AR) الطبقة الرابعة: تعليمات الشبكة (TCP/UDP، HTTP، عناوين)
            // (EN) Tier 4: Network instructions (TCP/UDP, HTTP, addresses)
            if (llvm::Value *result = emitNetworkBuiltin(inst))
                return result;
            if (reportedRealError())
                return nullptr;

            // (AR) الطبقة 4.5: بدائيّاتُ وحدةِ «عمليات» (fork/execvp، waitpid، pipe)
            // (EN) Tier 4.5: «عمليات» process primitives
            if (llvm::Value *result = emitProcessesBuiltin(inst))
                return result;
            if (reportedRealError())
                return nullptr;

            // (AR) الطبقة الخامسة: تعليمات SIMD متجهات (Phase 3)
            //      تنتج <N x T> أصلية + intrinsics (fma, sqrt, vector.reduce)
            // (EN) Tier 5: SIMD vector instructions (Phase 3)
            //      Emits native <N x T> + intrinsics (fma, sqrt, vector.reduce)
            if (llvm::Value *result = emitInstructionSimd(inst))
                return result;
            if (reportedRealError())
                return nullptr;

            // (AR) لا توجد طبقة تعرف هذا الـ opcode
            // (EN) No tier recognized this opcode
            reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", std::string("Unsupported opcode:") + std::to_string(static_cast<int>(inst->opcode))}});
            return nullptr;
        }

    } // namespace LLVM
} // namespace Sad
