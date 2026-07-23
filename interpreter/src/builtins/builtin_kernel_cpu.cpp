/**
 * @file builtin_kernel_cpu.cpp
 * @brief (AR) وحدة المعالج — إدارة سجلات ومقاطعات وجداول وصف المعالج
 * @brief (EN) CPU module — registers, interrupts, descriptor tables
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtins.h"
#include "builtin_registry.h"
#include "builtin_categories.h" // (AR) kBitwiseShiftCountMask المشترك بين المحرّكين (ت-1) / (EN) cross-engine shift-count mask (T-1)
#include <cmath>  // (AR) isnan لدلالة الإشباع في البتّيّات / (EN) isnan for bitwise saturation semantics
#include <limits> // (AR) numeric_limits لدلالة الإشباع / (EN) numeric_limits for saturation semantics
namespace Kcpu = Sad::Builtins::Names::KernelCpu;
#include "interpreter_core.h"

// Low-level headers
#include "cpu.h"
#include "io_ports.h"
#include "bitwise_ops.h"
#include "memory_map.h"
#include "interrupts.h"
#include "pointer_type.h"
#include "gdt.h"
#include "paging.h"
#include "pci.h"
#include "dma.h"
#include "framebuffer.h"
#include "acpi.h"
#include "sync.h"
#include "scheduler.h"
#include "boot.h"
#include "vfs.h"
#include "apic.h"
#include "hpet.h"
#include "syscall.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelCPU(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. وحدة المعالج / CPU Module
    // ═══════════════════════════════════════════════════════════════
    auto cpu_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& cpu = LowLevel::CPUManager::getInstance();
        return std::make_shared<Data::Value>(cpu.generateReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_0), cpu_info);

    // إيقاف المعالج (تعليمة HLT)
    auto cpu_halt = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        LowLevel::CPUManager::halt();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_1), cpu_halt);

    // قراءة عداد الطوابع الزمنية (TSC)
    auto cpu_rdtsc = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        uint64_t tsc = LowLevel::CPUManager::readTSC();
        return std::make_shared<Data::Value>(static_cast<double>(tsc));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_2), cpu_rdtsc);

    // تعطيل المقاطعات (CLI)
    auto cpu_cli = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        LowLevel::CPUManager::disableInterrupts();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_3), cpu_cli);

    // تفعيل المقاطعات (STI)
    auto cpu_sti = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        LowLevel::CPUManager::enableInterrupts();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_4), cpu_sti);

    // قراءة سجل MSR بالرقم المحدد
    auto cpu_read_msr = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint32_t msr = static_cast<uint32_t>(args[0]->toInt());
        uint64_t val = LowLevel::CPUManager::readMSR(msr);
        return std::make_shared<Data::Value>(static_cast<double>(val));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_5), cpu_read_msr);

    // كتابة قيمة في سجل MSR
    auto cpu_write_msr = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint32_t msr = static_cast<uint32_t>(args[0]->toInt());
        uint64_t val = static_cast<uint64_t>(args[1]->toDouble());
        LowLevel::CPUManager::writeMSR(msr, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_6), cpu_write_msr);

    // إبطال صفحة ذاكرة محددة (INVLPG)
    auto cpu_invlpg = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        LowLevel::CPUManager::invlpg(addr);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_7), cpu_invlpg);

    // ═══════════════════════════════════════════════════════════════
    // 2. منافذ الإدخال/الإخراج / IO Ports
    // ═══════════════════════════════════════════════════════════════
    auto io_inb = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint8_t val = LowLevel::IOPorts::inb(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_8), io_inb);

    // كتابة بايت واحد إلى منفذ الإخراج (outb)
    auto io_outb = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint8_t val = static_cast<uint8_t>(args[1]->toInt());
        LowLevel::IOPorts::outb(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_9), io_outb);

    // قراءة كلمة 16-بت من منفذ الإدخال (inw)
    auto io_inw = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint16_t val = LowLevel::IOPorts::inw(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_10), io_inw);

    // كتابة كلمة 16-بت إلى منفذ الإخراج (outw)
    auto io_outw = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint16_t val = static_cast<uint16_t>(args[1]->toInt());
        LowLevel::IOPorts::outw(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_11), io_outw);

    // قراءة كلمة مزدوجة 32-بت من منفذ الإدخال (inl)
    auto io_inl = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint32_t val = LowLevel::IOPorts::inl(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_12), io_inl);

    // كتابة كلمة مزدوجة 32-بت إلى منفذ الإخراج (outl)
    auto io_outl = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint32_t val = static_cast<uint32_t>(args[1]->toInt());
        LowLevel::IOPorts::outl(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_13), io_outl);

    // ═══════════════════════════════════════════════════════════════
    // 3. العمليات البتية / Bitwise Operations
    // (AR) القرار المعماريّ (توافق المحرّكين): البتّيّات تعمل على i64 (أعداد ص
    //      الصحيحة 64-بت)، وعدّاد الإزاحة يُقنَّع بـ&63 (دلالة عتاد x86-64) —
    //      سلوك حتميّ بلا UB للعدّاد السالب/الكبير، مطابق لتوليد المترجم.
    //      الإزاحة اليمنى حسابيّة (تحفظ الإشارة) — int64_t >>.
    // (EN) Architectural decision (engine parity): bitwise ops act on i64;
    //      shift count is masked with &63 (x86-64 hardware semantics) —
    //      deterministic, UB-free for negative/oversized counts, matching
    //      compiler codegen. Right shift is arithmetic (sign-preserving).
    // ═══════════════════════════════════════════════════════════════
    // (AR) قناع عدّاد الإزاحة المشترك بين المحرّكين (ت-1) — مصدره الوحيد
    //      shared/builtins/include/builtin_categories.h كي لا تتباعد النسختان.
    // (EN) Cross-engine shared shift-count mask (T-1) — single source in
    //      shared/builtins/include/builtin_categories.h so the engines cannot drift.
    constexpr int64_t kShiftCountMask = Sad::Builtins::kBitwiseShiftCountMask;

    // (AR) تحويل معامل بتّيّ إلى i64 بدلالة الإشباع المنصّيّة الموحَّدة (إصلاح CI‏
    //      ARM): عشريّ NaN ⇒ 0، فوق INT64_MAX ⇒ INT64_MAX، تحت INT64_MIN ⇒
    //      INT64_MIN، وإلّا اقتطاع نحو الصفر — نفس llvm.fptosi.sat.i64.f64 الذي
    //      يُصدره المترجم (emitF64ToI64Sat) ونفس طيّ الثوابت
    //      (foldDoubleToI64Saturating). cast خام على عشريّ خارج المدى UB منصّيّ
    //      (x86 cvttsd2si⇒INT64_MIN، ‏ARM fcvtzs⇒إشباع) فكان يكسر التكافؤ على
    //      macOS ‏ARM64. مقصور على مسار البتّيّات — toInt64 العامّ لم يُمَسّ.
    // (EN) Saturating i64 conversion for bitwise operands — Sad's single
    //      platform-independent semantics (ARM CI fix): float NaN ⇒ 0, above
    //      INT64_MAX ⇒ INT64_MAX, below INT64_MIN ⇒ INT64_MIN, else truncation
    //      toward zero — exactly the llvm.fptosi.sat.i64.f64 the compiler emits
    //      and the constant fold applies. A raw out-of-range cast is UB and
    //      hardware-dependent (x86 cvttsd2si vs ARM fcvtzs), breaking parity on
    //      macOS ARM64. Scoped to the bitwise path — the general toInt64 is
    //      untouched.
    auto bitwiseOperandToI64 = [](const Data::Value &v) -> int64_t {
        if (!v.isDouble())
            return v.toInt64();
        const double d = v.toDouble();
        if (std::isnan(d))
            return 0;
        constexpr double kTwoPow63 = 9223372036854775808.0; // 2^63 (بالضبط كـdouble)
        if (d >= kTwoPow63)
            return std::numeric_limits<int64_t>::max();
        if (d < -kTwoPow63)
            return std::numeric_limits<int64_t>::min();
        return static_cast<int64_t>(d); // اقتطاع نحو الصفر داخل المدى / in-range truncation
    };

    // (AR) حارس النوع العدديّ للبتّيّات (د-2، توافق المحرّكين): المعاملات عدديّة
    //      حصرًا (صحيح/عشريّ). كان toInt64 يقبل النصّ عبر stoll («"7"» تصير 7)
    //      ويُصفّر المصفوفة بصمت، بينما المترجم كان يحوّل النصّ ptrtoint (بتّات
    //      مؤشّر!) — كود خاطئ صامت في المحرّكين. الرفض الصريح: RUN053 (تتطلّب
    //      قيمة رقميّة) — الأدقّ من RUN037 العامّة. المترجم يرفض النظير سكونيًّا
    //      بـSEM002 حيث النوع معلوم (builtins_system.cpp).
    // (EN) Numeric-type guard for the bitwise builtins (D-2, engine parity):
    //      operands are strictly numeric (int/float). toInt64 accepted strings
    //      via stoll ("7" became 7) and silently zeroed arrays, while the
    //      compiler ptrtoint'ed strings (pointer bits!) — silent wrong results
    //      in both engines. Explicit rejection: RUN053 (numeric required) — more
    //      precise than the generic RUN037. The compiler rejects the mirror case
    //      statically with SEM002 where the type is known (builtins_system.cpp).
    auto requireNumericBitArgs = [](Sad::Interpreter::BuiltinContext &ctx, std::size_t n) {
        const auto &args = ctx.args();
        for (std::size_t i = 0; i < n; ++i)
            if (!args[i]->isNumeric())
                ctx.error(::Sad::Errors::ErrorCode::RUN_NUMERIC_REQUIRED,
                          {{"op", std::string(ctx.functionName())}});
    };

    auto bit_and = [requireNumericBitArgs, bitwiseOperandToI64](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        requireNumericBitArgs(ctx, 2);
        return std::make_shared<Data::Value>(bitwiseOperandToI64(*args[0]) & bitwiseOperandToI64(*args[1]));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_14), bit_and);

    // عملية OR بتية بين قيمتين
    auto bit_or = [requireNumericBitArgs, bitwiseOperandToI64](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        requireNumericBitArgs(ctx, 2);
        return std::make_shared<Data::Value>(bitwiseOperandToI64(*args[0]) | bitwiseOperandToI64(*args[1]));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_15), bit_or);

    // عملية XOR بتية حصرية بين قيمتين
    auto bit_xor = [requireNumericBitArgs, bitwiseOperandToI64](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        requireNumericBitArgs(ctx, 2);
        return std::make_shared<Data::Value>(bitwiseOperandToI64(*args[0]) ^ bitwiseOperandToI64(*args[1]));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_16), bit_xor);

    // عملية NOT بتية (نفي بتي)
    auto bit_not = [requireNumericBitArgs, bitwiseOperandToI64](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        requireNumericBitArgs(ctx, 1);
        return std::make_shared<Data::Value>(~bitwiseOperandToI64(*args[0]));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_17), bit_not);

    // إزاحة بتية لليسار (SHL) — العدّاد مُقنَّع بـ&63
    auto bit_shl = [kShiftCountMask, requireNumericBitArgs, bitwiseOperandToI64](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        requireNumericBitArgs(ctx, 2);
        const int64_t count = bitwiseOperandToI64(*args[1]) & kShiftCountMask;
        // (AR) الإزاحة على غير المُوقَّع ثم الإرجاع لـi64 — سلوك التفاف حتميّ بلا UB
        // (EN) Shift as unsigned then cast back — deterministic wrap, no UB
        const uint64_t shifted = static_cast<uint64_t>(bitwiseOperandToI64(*args[0])) << count;
        return std::make_shared<Data::Value>(static_cast<int64_t>(shifted));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_18), bit_shl);

    // إزاحة بتية لليمين (حسابيّة تحفظ الإشارة) — العدّاد مُقنَّع بـ&63
    auto bit_shr = [kShiftCountMask, requireNumericBitArgs, bitwiseOperandToI64](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        requireNumericBitArgs(ctx, 2);
        const int64_t count = bitwiseOperandToI64(*args[1]) & kShiftCountMask;
        return std::make_shared<Data::Value>(bitwiseOperandToI64(*args[0]) >> count);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_19), bit_shr);

    // اختبار بت محدد في قيمة
    auto bit_test = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>((val >> bit) & 1);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_20), bit_test);

    // تعيين بت محدد في قيمة
    auto bit_set = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>(val | (1 << bit));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_21), bit_set);

    // مسح بت محدد في قيمة
    auto bit_clear = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>(val & ~(1 << bit));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_22), bit_clear);

    // ═══════════════════════════════════════════════════════════════
    // 4. إدارة الذاكرة / Memory Management
    // ═══════════════════════════════════════════════════════════════
    auto mem_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& mm = LowLevel::MemoryManager::getInstance();
        return std::make_shared<Data::Value>(mm.getMemoryReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_23), mem_info);

    // قراءة بايت من عنوان ذاكرة مباشر (peek)
    auto mem_peek = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        uint8_t val = *reinterpret_cast<volatile uint8_t*>(addr);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_24), mem_peek);

    // كتابة بايت إلى عنوان ذاكرة مباشر (poke)
    auto mem_poke = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        uint8_t val = static_cast<uint8_t>(args[1]->toInt());
        *reinterpret_cast<volatile uint8_t*>(addr) = val;
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_25), mem_poke);

    // ═══════════════════════════════════════════════════════════════
    // 5. المقاطعات / Interrupts
    // ═══════════════════════════════════════════════════════════════
    auto int_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& im = LowLevel::InterruptManager::getInstance();
        return std::make_shared<Data::Value>(im.getInterruptReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_26), int_info);

    // تهيئة جدول المقاطعات IDT وتحميله
    auto int_init = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& im = LowLevel::InterruptManager::getInstance();
        im.initialize();
        im.loadIDT();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_27), int_init);

    // ═══════════════════════════════════════════════════════════════
    // 6. GDT / TSS
    // ═══════════════════════════════════════════════════════════════
    auto gdt_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& gdt = LowLevel::GDTManager::getInstance();
        return std::make_shared<Data::Value>(gdt.generateReport());
    };

    // تهيئة جدول الواصفات العام GDT
    auto gdt_init = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& gdt = LowLevel::GDTManager::getInstance();
        gdt.initialize();
        return std::make_shared<Data::Value>(0);
    };

    // ═══════════════════════════════════════════════════════════════
    // 7. الترحيل (Paging)
    // ═══════════════════════════════════════════════════════════════
    auto paging_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& pg = LowLevel::PagingManager::getInstance();
        return std::make_shared<Data::Value>(pg.generateReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_28), paging_info);

    // ربط صفحة افتراضية بعنوان فيزيائي
    auto paging_map = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t virt = static_cast<uint64_t>(args[0]->toDouble());
        uint64_t phys = static_cast<uint64_t>(args[1]->toDouble());
        uint64_t flags = args.size() > 2 ? static_cast<uint64_t>(args[2]->toInt()) : 0x3;
        auto& pg = LowLevel::PagingManager::getInstance();
        bool ok = pg.mapPage(virt, phys, flags);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_29), paging_map);

    // إلغاء ربط صفحة افتراضية
    auto paging_unmap = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t virt = static_cast<uint64_t>(args[0]->toDouble());
        auto& pg = LowLevel::PagingManager::getInstance();
        pg.unmapPage(virt);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_30), paging_unmap);

    // ═══════════════════════════════════════════════════════════════
    // 8. PCI / PCIe
    // ═══════════════════════════════════════════════════════════════
    auto pci_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        return std::make_shared<Data::Value>(pci.generateReport());
    };

    // مسح ناقل PCI واكتشاف الأجهزة
    auto pci_scan = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        pci.enumerate();
        return std::make_shared<Data::Value>(static_cast<int>(pci.getDevices().size()));
    };

    // قراءة سجل تهيئة PCI (32-بت)
    auto pci_read = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 4) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        LowLevel::PCIAddress addr;
        addr.bus = static_cast<uint8_t>(args[0]->toInt());
        addr.device = static_cast<uint8_t>(args[1]->toInt());
        addr.function = static_cast<uint8_t>(args[2]->toInt());
        uint8_t offset = static_cast<uint8_t>(args[3]->toInt());
        uint32_t val = LowLevel::PCIManager::readConfig32(addr, offset);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };

    // كتابة سجل تهيئة PCI (32-بت)
    auto pci_write = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 5) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        LowLevel::PCIAddress addr;
        addr.bus = static_cast<uint8_t>(args[0]->toInt());
        addr.device = static_cast<uint8_t>(args[1]->toInt());
        addr.function = static_cast<uint8_t>(args[2]->toInt());
        uint8_t offset = static_cast<uint8_t>(args[3]->toInt());
        uint32_t val = static_cast<uint32_t>(args[4]->toInt());
        LowLevel::PCIManager::writeConfig32(addr, offset, val);
        return std::make_shared<Data::Value>(0);
    };

    // البحث عن بطاقات GPU في ناقل PCI
    auto pci_find_gpu = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        auto gpus = pci.findGPUs();
        std::ostringstream oss;
        oss << "عدد GPUs: " << gpus.size() << "\n";
        for (size_t i = 0; i < gpus.size(); i++) {
            oss << "  [" << i << "] Bus=" << static_cast<int>(gpus[i].address.bus)
                << " Dev=" << static_cast<int>(gpus[i].address.device)
                << " Vendor=0x" << std::hex << gpus[i].vendorId
                << " Device=0x" << gpus[i].deviceId << std::dec << "\n";
        }
        return std::make_shared<Data::Value>(oss.str());
    };

    // ═══════════════════════════════════════════════════════════════
    // 9. DMA
    // ═══════════════════════════════════════════════════════════════
    auto dma_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& dma = LowLevel::DMAManager::getInstance();
        return std::make_shared<Data::Value>(dma.generateReport());
    };

    // ═══════════════════════════════════════════════════════════════
    // 10. إطار الشاشة / Framebuffer & GPU
    // ═══════════════════════════════════════════════════════════════
    auto fb_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        return std::make_shared<Data::Value>(fb.generateReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_31), fb_info);

    // رسم نقطة (بكسل) على الشاشة بإحداثيات ولون RGB
    auto fb_pixel = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 5) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int32_t x = args[0]->toInt(), y = args[1]->toInt();
        uint8_t r = static_cast<uint8_t>(args[2]->toInt());
        uint8_t g = static_cast<uint8_t>(args[3]->toInt());
        uint8_t b = static_cast<uint8_t>(args[4]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.putPixel(x, y, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_32), fb_pixel);

    // رسم خط مستقيم بين نقطتين بلون RGB
    auto fb_line = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 7) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int32_t x1 = args[0]->toInt(), y1 = args[1]->toInt();
        int32_t x2 = args[2]->toInt(), y2 = args[3]->toInt();
        uint8_t r = static_cast<uint8_t>(args[4]->toInt());
        uint8_t g = static_cast<uint8_t>(args[5]->toInt());
        uint8_t b = static_cast<uint8_t>(args[6]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawLine(x1, y1, x2, y2, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_33), fb_line);

    // رسم إطار مستطيل (بدون تعبئة) بلون RGB
    auto fb_rect = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 7) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(), static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
        uint8_t r = static_cast<uint8_t>(args[4]->toInt());
        uint8_t g = static_cast<uint8_t>(args[5]->toInt());
        uint8_t b = static_cast<uint8_t>(args[6]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawRect(rect, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_34), fb_rect);

    // رسم مستطيل ممتلئ بلون RGB
    auto fb_fill_rect = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 7) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(), static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
        uint8_t r = static_cast<uint8_t>(args[4]->toInt());
        uint8_t g = static_cast<uint8_t>(args[5]->toInt());
        uint8_t b = static_cast<uint8_t>(args[6]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.fillRect(rect, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_35), fb_fill_rect);

    // رسم دائرة بمركز ونصف قطر ولون RGB
    auto fb_circle = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 6) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int32_t cx = args[0]->toInt(), cy = args[1]->toInt();
        int32_t radius = args[2]->toInt();
        uint8_t r = static_cast<uint8_t>(args[3]->toInt());
        uint8_t g = static_cast<uint8_t>(args[4]->toInt());
        uint8_t b = static_cast<uint8_t>(args[5]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawCircle(cx, cy, radius, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_36), fb_circle);

    // رسم نص على الشاشة بإحداثيات ولون RGB
    auto fb_text = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 6) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        int32_t x = args[0]->toInt(), y = args[1]->toInt();
        std::string text = args[2]->toString();
        uint8_t r = static_cast<uint8_t>(args[3]->toInt());
        uint8_t g = static_cast<uint8_t>(args[4]->toInt());
        uint8_t b = static_cast<uint8_t>(args[5]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawString(x, y, text.c_str(), LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_37), fb_text);

    // مسح الشاشة بلون محدد (أسود افتراضياً)
    auto fb_clear = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        uint8_t r = 0, g = 0, b = 0;
        if (args.size() >= 3) {
            r = static_cast<uint8_t>(args[0]->toInt());
            g = static_cast<uint8_t>(args[1]->toInt());
            b = static_cast<uint8_t>(args[2]->toInt());
        }
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.clear(LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_38), fb_clear);

    // تبديل المخزن المؤقت (double buffering)
    auto fb_swap = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.swapBuffers();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_39), fb_swap);

    // رسم تدرج لوني (أفقي أو عمودي) في مستطيل
    auto fb_gradient = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 11) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(), static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
        LowLevel::Color c1(static_cast<uint8_t>(args[4]->toInt()),
                           static_cast<uint8_t>(args[5]->toInt()),
                           static_cast<uint8_t>(args[6]->toInt()));
        LowLevel::Color c2(static_cast<uint8_t>(args[7]->toInt()),
                           static_cast<uint8_t>(args[8]->toInt()),
                           static_cast<uint8_t>(args[9]->toInt()));
        bool horizontal = args[10]->toInt() != 0;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        if (horizontal) {
            fb.fillGradientH(rect, c1, c2);
        } else {
            fb.fillGradientV(rect, c1, c2);
        }
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_40), fb_gradient);

    // ═══════════════════════════════════════════════════════════════
    // 11. ACPI — إدارة الطاقة
    // ═══════════════════════════════════════════════════════════════
    auto acpi_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.generateReport());
    };

    // إيقاف تشغيل الجهاز عبر ACPI
    auto acpi_shutdown = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.shutdown();
        return std::make_shared<Data::Value>(0);
    };

    // إعادة تشغيل الجهاز عبر ACPI
    auto acpi_reboot = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.reboot();
        return std::make_shared<Data::Value>(0);
    };

    // 11b. ACPI — الدوال الموسّعة / Extended ACPI functions (v2)

    // ═══════════════════════════════════════════════════════════════
    // 12. التزامن / Synchronization
    // ═══════════════════════════════════════════════════════════════
    auto sync_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        std::ostringstream oss;
        oss << "وحدة التزامن جاهزة / Sync module ready\n";
        oss << "المكونات: Spinlock, RWLock, Mutex, Semaphore, Barrier, Once\n";
        return std::make_shared<Data::Value>(oss.str());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_41), sync_info);

    // توقف مؤقت للمعالج (تعليمة PAUSE للحلقات المزدحمة)
    auto sync_pause = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        LowLevel::Atomic::pause();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_42), sync_pause);

    // حاجز ذاكرة لضمان ترتيب العمليات
    auto sync_barrier = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        LowLevel::Atomic::memoryBarrier();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_43), sync_barrier);

    // ═══════════════════════════════════════════════════════════════
    // 13. المجدول / Scheduler
    // ═══════════════════════════════════════════════════════════════
    auto sched_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& sched = LowLevel::SchedulerManager::getInstance();
        return std::make_shared<Data::Value>(sched.generateReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_44), sched_info);

    // إنشاء عملية جديدة في المجدول
    auto sched_create_process = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        std::string name = args.empty() ? "عملية" : args[0]->toString();
        auto& sched = LowLevel::SchedulerManager::getInstance();
        uint64_t pid = sched.createProcess(name.c_str());
        return std::make_shared<Data::Value>(static_cast<double>(pid));
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_45), sched_create_process);

    // تنازل العملية الحالية عن المعالج
    auto sched_yield = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& sched = LowLevel::SchedulerManager::getInstance();
        sched.yield();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_46), sched_yield);

    // ═══════════════════════════════════════════════════════════════
    // 14. الإقلاع / Boot
    // ═══════════════════════════════════════════════════════════════
    auto boot_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& boot = LowLevel::BootManager::getInstance();
        return std::make_shared<Data::Value>(boot.generateReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_47), boot_info);

    // ═══════════════════════════════════════════════════════════════
    // 15. VFS — نظام الملفات الافتراضي
    // ═══════════════════════════════════════════════════════════════
    auto vfs_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& vfs = LowLevel::VFSManager::getInstance();
        return std::make_shared<Data::Value>(vfs.generateReport());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_48), vfs_info);

    // تركيب نظام ملفات على مسار محدد
    auto vfs_mount = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 3) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        std::string dev = args[0]->toString();
        std::string path = args[1]->toString();
        std::string fsType = args[2]->toString();
        bool ro = args.size() > 3 ? args[3]->toInt() != 0 : false;
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.mount(dev, path, fsType, ro);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_49), vfs_mount);

    // فصل نظام ملفات مركّب
    auto vfs_unmount = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.unmount(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_50), vfs_unmount);

    // التحقق من وجود ملف أو مجلد
    auto vfs_exists = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.exists(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_51), vfs_exists);

    // إنشاء مجلد جديد في نظام الملفات
    auto vfs_mkdir = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.mkdir(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_52), vfs_mkdir);

    // ═══════════════════════════════════════════════════════════════
    // 16. APIC — المقاطعات المتقدمة
    // ═══════════════════════════════════════════════════════════════
    auto apic_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.generateReport());
    };

    // إرسال نهاية المقاطعة (End of Interrupt)
    auto apic_eoi = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendEOI();
        return std::make_shared<Data::Value>(0);
    };

    // إرسال مقاطعة بين المعالجات (IPI)
    auto apic_ipi = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint8_t dest = static_cast<uint8_t>(args[0]->toInt());
        uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendIPI(dest, vec, LowLevel::DeliveryMode::FIXED);
        return std::make_shared<Data::Value>(0);
    };

    // توجيه مقاطعة IRQ إلى معالج محدد عبر IO APIC
    auto apic_route_irq = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 3) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint8_t irq = static_cast<uint8_t>(args[0]->toInt());
        uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
        uint8_t dest = static_cast<uint8_t>(args[2]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.routeIRQ(irq, vec, dest);
        return std::make_shared<Data::Value>(0);
    };

    // 16b. APIC — الدوال الموسّعة / Extended APIC functions

    // ═══════════════════════════════════════════════════════════════
    // 17. HPET — المؤقت عالي الدقة
    // ═══════════════════════════════════════════════════════════════
    auto hpet_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        return std::make_shared<Data::Value>(hpet.generateReport());
    };

    // تهيئة مؤقت HPET بعنوان MMIO
    auto hpet_init = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t base = static_cast<uint64_t>(args[0]->toDouble());
        auto& hpet = LowLevel::HPETManager::getInstance();
        bool ok = hpet.init(base);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };

    // تفعيل مؤقت HPET
    auto hpet_enable = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.enable();
        return std::make_shared<Data::Value>(0);
    };

    // قراءة عداد HPET الحالي
    auto hpet_read = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        uint64_t count = hpet.readCounter();
        return std::make_shared<Data::Value>(static_cast<double>(count));
    };

    // تأخير بالمايكروثانية عبر HPET
    auto hpet_delay_us = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t us = static_cast<uint64_t>(args[0]->toInt());
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.delayMicroseconds(us);
        return std::make_shared<Data::Value>(0);
    };

    // تأخير بالمللي ثانية عبر HPET
    auto hpet_delay_ms = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t ms = static_cast<uint64_t>(args[0]->toInt());
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.delayMilliseconds(ms);
        return std::make_shared<Data::Value>(0);
    };

    // حساب الوقت المنقضي بالنانوثانية منذ عداد بداية
    auto hpet_elapsed_ns = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        if (args.empty()) ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
        uint64_t start = static_cast<uint64_t>(args[0]->toDouble());
        auto& hpet = LowLevel::HPETManager::getInstance();
        uint64_t ns = hpet.elapsedNanoseconds(start);
        return std::make_shared<Data::Value>(static_cast<double>(ns));
    };

    // الحصول على تردد مؤقت HPET بالهرتز
    auto hpet_frequency = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(hpet.getFrequency()));
    };

    // ═══════════════════════════════════════════════════════════════
    // 18. استدعاءات النظام / System Calls
    // ═══════════════════════════════════════════════════════════════
    auto syscall_info = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        return std::make_shared<Data::Value>(sc.generateReport());
    };

    // تهيئة آلية SYSCALL/SYSRET بشرائح النواة والمستخدم
    auto syscall_init = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        uint16_t kCS = args.size() > 0 ? static_cast<uint16_t>(args[0]->toInt()) : 0x08;
        uint16_t kSS = args.size() > 1 ? static_cast<uint16_t>(args[1]->toInt()) : 0x10;
        uint16_t uCS = args.size() > 2 ? static_cast<uint16_t>(args[2]->toInt()) : 0x18;
        uint16_t uSS = args.size() > 3 ? static_cast<uint16_t>(args[3]->toInt()) : 0x20;
        auto& sc = LowLevel::SyscallManager::getInstance();
        bool ok = sc.init(kCS, kSS, uCS, uSS);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };

    // تسجيل استدعاءات النظام الافتراضية
    auto syscall_register_defaults = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        sc.registerDefaultSyscalls();
        return std::make_shared<Data::Value>(0);
    };

    // الحصول على إحصائيات استدعاءات النظام
    auto syscall_stats = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        auto stats = sc.getStats();
        std::ostringstream oss;
        oss << "إجمالي: " << stats.totalCalls << " | فاشلة: " << stats.failedCalls << " | مجهولة: " << stats.unknownCalls;
        return std::make_shared<Data::Value>(oss.str());
    };

    // ═══════════════════════════════════════════════════════════════
    // 19. دالة تقرير شامل / Comprehensive Report
    // ═══════════════════════════════════════════════════════════════
    auto lowlevel_report = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
        (void)args;
        std::ostringstream oss;
        oss << "\n" << std::string(70, '#') << "\n";
        oss << "# تقرير المكتبة منخفضة المستوى الشامل\n";
        oss << "# Low-Level Library Comprehensive Report\n";
        oss << std::string(70, '#') << "\n\n";

        oss << "الوحدات المتاحة / Available Modules:\n";
        oss << "  1.  معالج (CPU)         — CPUID, MSR, TSC, CR0-CR4\n";
        oss << "  2.  منافذ (IO)          — inb/outb/inw/outw/inl/outl\n";
        oss << "  3.  بتات (Bitwise)      — AND/OR/XOR/NOT/SHL/SHR\n";
        oss << "  4.  ذاكرة (Memory)      — malloc/free/mmap/munmap\n";
        oss << "  5.  مقاطعات (Interrupts) — IDT/ISR/IRQ/PIC\n";
        oss << "  6.  GDT/TSS            — جداول الواصفات\n";
        oss << "  7.  ترحيل (Paging)      — صفحات 4K/2M/1G\n";
        oss << "  8.  PCI/PCIe           — مسح/قراءة/كتابة/GPU\n";
        oss << "  9.  DMA                — نقل ذاكرة مباشر\n";
        oss << "  10. شاشة (Framebuffer)  — رسومات HD/GPU\n";
        oss << "  11. ACPI               — طاقة/إيقاف/إعادة تشغيل\n";
        oss << "  12. تزامن (Sync)        — أقفال/حواجز/سيمافور\n";
        oss << "  13. مجدول (Scheduler)   — عمليات/خيوط\n";
        oss << "  14. إقلاع (Boot)        — Multiboot2\n";
        oss << "  15. ملفات (VFS)         — mount/open/read/write\n";
        oss << "  16. APIC               — مقاطعات متقدمة/IPI\n";
        oss << "  17. HPET               — مؤقت عالي الدقة\n";
        oss << "  18. Syscall            — SYSCALL/SYSRET\n";
        oss << "\nإجمالي الدوال المسجلة / Total registered functions: 100+\n";
        oss << std::string(70, '#') << "\n";
        return std::make_shared<Data::Value>(oss.str());
    };
    fm.registerBuiltinFunction(std::string(Kcpu::CPU_53), lowlevel_report);

} // registerBuiltinsKernelCPU

} // namespace Interpreter
} // namespace Sad
