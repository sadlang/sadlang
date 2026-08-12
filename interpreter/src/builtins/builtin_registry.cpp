/**
 * @file builtin_registry.cpp
 * @brief (AR) نظام التسجيل الكسول — يسجل الوحدات الأساسية فقط عند البدء
 *        والوحدات الأخرى عند الاستيراد
 * @brief (EN) Lazy registration system — registers only core modules at startup,
 *        other modules on import
 *
 * @details
 * (AR) الوحدات المُضمّنة:
 *   - الأساسية (Core)       → Parts 1-2, 6, 8, 10 — تُسجَّل دائماً
 *   - نظام_منخفض (OS)       → Parts 19-22          — عند `استورد نظام_منخفض`
 *   - usb                   → Part 23               — عند `استورد usb`
 *   - محرك (Game Engine)    → Parts 29-34           — عند `استورد محرك`
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_module_registry.h"
#include "function_manager.h"
#include "ui/ui_eval_bridge.h" // (AR) م2-أ: مزوّدو وحدات خارجيّون؛ بادئة ui/ لتحلّ في sad_core وsad_interpreter

// (AR) السجل المركزي الموحّد — ADR-003
// (EN) Unified central registry — ADR-003
#include "builtin_registry.h"

#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) اختصارات لأسماء الدوال المركزية
        namespace Bs = Builtins::Names::Strings;
        namespace Ba = Builtins::Names::Arrays;
        namespace Bm = Builtins::Names::Math;
        namespace Bb = Builtins::Names::Basics;
        namespace Bss = Builtins::Names::Assertions;
        namespace Bmp = Builtins::Names::Maps;
        namespace Bsk = Builtins::Names::Sockets;
        namespace Bhc = Builtins::Names::HttpClient;
        namespace Bhs = Builtins::Names::HttpServer;
        namespace Bcr = Builtins::Names::Crypto;
        namespace Bnu = Builtins::Names::NetworkUtils;
        namespace Bcr = Builtins::Names::Crypto;
        // (AR) ثوابت أسماء الوحدات — مُشتقَّة من module_definitions.h
        // (EN) Module name constants — derived from module_definitions.h
        using Builtins::ModuleId;
        using Builtins::getCanonicalModuleName;
        static const std::string kModStrings    = std::string(Builtins::getCanonicalModuleName(ModuleId::STRINGS));
        static const std::string kModMath       = std::string(Builtins::getCanonicalModuleName(ModuleId::MATH));
        static const std::string kModBasics     = std::string(Builtins::getCanonicalModuleName(ModuleId::BASICS));
        static const std::string kModAssertions = std::string(Builtins::getCanonicalModuleName(ModuleId::ASSERTIONS));
        static const std::string kModCrypto     = std::string(Builtins::getCanonicalModuleName(ModuleId::CRYPTO));
        static const std::string kModMaps       = std::string(Builtins::getCanonicalModuleName(ModuleId::MAPS));
        static const std::string kModAsync      = std::string(Builtins::getCanonicalModuleName(ModuleId::ASYNC));
        static const std::string kModPlatform   = std::string(Builtins::getCanonicalModuleName(ModuleId::PLATFORM));
        static const std::string kModNetwork    = std::string(Builtins::getCanonicalModuleName(ModuleId::NETWORK));
        static const std::string kModSockets    = std::string(Builtins::getCanonicalModuleName(ModuleId::SOCKETS));
        static const std::string kModProcesses  = std::string(Builtins::getCanonicalModuleName(ModuleId::PROCESSES));

        // ═════════════════════════════════════════════════════════════════
        // (AR) إعلانات خارجية — ملفات التسجيل حسب المحتوى
        // (EN) Extern declarations — registration files by content
        // ═════════════════════════════════════════════════════════════════
        extern void registerBuiltinsStrings(Interpreter &interpreter);
        extern void registerBuiltinsBasics(Interpreter &interpreter);
        extern void registerBuiltinsMath(Interpreter &interpreter);
        extern void registerBuiltinsAssertions(Interpreter &interpreter);
        extern void registerBuiltinsCrypto(Interpreter &interpreter);
        extern void registerBuiltinsProcesses(Interpreter &interpreter);
        extern void registerBuiltinsAsync(Interpreter &interpreter);
        extern void registerBuiltinsMaps(Interpreter &interpreter);
        extern void registerBuiltinsKernelCPU(Interpreter &interpreter);
        extern void registerBuiltinsKernelUEFI(Interpreter &interpreter);
        extern void registerBuiltinsKernelACPI(Interpreter &interpreter);
        extern void registerBuiltinsKernelGPU(Interpreter &interpreter);
        extern void registerBuiltinsKernelUSB(Interpreter &interpreter);
        extern void registerBuiltinsKernelStorage(Interpreter &interpreter);
        extern void registerBuiltinsKernelNetwork(Interpreter &interpreter);
        extern void registerBuiltinsKernelAudio(Interpreter &interpreter);
        extern void registerBuiltinsKernelTimers(Interpreter &interpreter);
        extern void registerBuiltinsKernelSerial(Interpreter &interpreter);
        extern void registerBuiltinsKernelMemory(Interpreter &interpreter);
        extern void registerBuiltinsKernelThreads(Interpreter &interpreter);
        extern void registerBuiltinsFFI(Interpreter &interpreter);
        extern void registerBuiltinsExceptions(Interpreter &interpreter);
        extern void registerBuiltinsPlatform(Interpreter &interpreter);
        extern void registerBuiltinsSockets(Interpreter &interpreter);
        extern void registerBuiltinsHttp(Interpreter &interpreter);
        extern void registerBuiltinsSadNet(Interpreter &interpreter);
        extern void registerBuiltinsWebSocket(Interpreter &interpreter);

        // ═════════════════════════════════════════════════════════════════
        // BuiltinModuleRegistry::loadModule — implemented here because it
        // needs FunctionManager (which requires interpreter_core.h)
        // ═════════════════════════════════════════════════════════════════
        bool BuiltinModuleRegistry::loadModule(const std::string &name)
        {
            std::string canonical = resolveAlias(name);
            if (loadedModules_.count(canonical) > 0)
                return true;

            auto it = modules_.find(canonical);
            if (it == modules_.end() || !interpreter_)
                return false;

            // (AR) تفعيل تتبع التسجيل لالتقاط جميع الدوال المسجلة فعلاً
            //      بدلاً من الاعتماد على فرق أسماء الدوال (diff) الذي يفشل
            //      عندما تكون الدالة مسجلة مسبقاً بواسطة StandardLibraryManager
            // (EN) Enable registration tracking to capture ALL actually registered
            //      functions, instead of relying on name diff which fails when
            //      functions are pre-registered by StandardLibraryManager
            auto &fm = interpreter_->getFunctionManager();
            fm.startRegistrationTracking();

            // (AR) استدعاء دالة التسجيل
            // (EN) Call the registration function
            it->second.registrar(*interpreter_);

            // (AR) الحصول على جميع الدوال المسجلة أثناء التتبع (بدون تكرار)
            // (EN) Get all functions registered during tracking (deduplicated)
            it->second.exportedFunctions = fm.stopRegistrationTracking();

            loadedModules_.insert(canonical);
            return true;
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) الوحدةُ اللازمةُ لمدمَجةٍ ما — استعلامٌ عن الجدولِ المولَّدِ لا غير.
        //
        //      🔴 كانت هنا خريطةٌ مكتوبةٌ باليد (٤٠٠ سطر) تتجاوز مصدرَ الحقيقة،
        //         ونظيرتُها في المصرّف (sir_builder_module_check.cpp) تتجاوزه
        //         وتخالفها. فصار للغةِ ثلاثةُ مصادرَ متناقضةٍ لسؤالٍ واحد.
        //         حُذفت الخريطتان، والمصدرُ الآن Sad::Builtins::IMPORT_GATE
        //         المولَّدُ من require_import + module في مصدر الحقيقة.
        //      🔴 لا تُعِد كتابةَ خريطةٍ يدويّةٍ هنا: حارسُ
        //         scripts/codegen/check_no_handwritten_import_gate.py يرفضها.
        // (EN) The module a builtin requires — a lookup into the generated table.
        //      Two hand-written maps (one per engine) used to override the SoT and
        //      contradict each other; both are gone. Change behaviour in the YAML
        //      SoT and regenerate, never here.
        // ═════════════════════════════════════════════════════════════════
        std::string BuiltinModuleRegistry::suggestModuleForFunction(const std::string &funcName) const
        {
            return std::string(
                Sad::Builtins::importGateModuleNameFor(std::string_view(funcName)));
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) دالة حرة: اقتراح الوحدة لدالة — يستدعيها expression_evaluator_calls
        // (EN) Free function: suggest module for function — called by expression_evaluator_calls
        // ═════════════════════════════════════════════════════════════════
        std::string suggestModuleForUnimportedFunction(const std::string &funcName)
        {
            return BuiltinModuleRegistry::getInstance().suggestModuleForFunction(funcName);
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) تسجيل الوحدات المُضمّنة في السجل المركزي
        // (EN) Register builtin modules in the central registry
        // ═════════════════════════════════════════════════════════════════
        static void registerBuiltinModules()
        {
            auto &registry = BuiltinModuleRegistry::getInstance();

            // ═══════════════════════════════════════════════════════════════
            // (AR) الوحدات الأساسية — متاحة دائماً، تُحمَّل كسولاً في الوضع الصارم
            // (EN) Core modules — always available, lazy-loaded in strict mode
            // ═══════════════════════════════════════════════════════════════

            // ─── نصوص / Strings (Part 1) — string ops, basic math, type checks ───
            registry.registerModule({"\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5", // نصوص

                                     "\xd8\xaf\xd9\x88\xd8\xa7\xd9\x84 \xd8\xa7\xd9\x84\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5 \xd9\x88\xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd8\xb3\xd9\x84",
                                     // دوال النصوص والسلاسل
                                     [](Interpreter &interp)
                                     { registerBuiltinsStrings(interp); },
                                     {}});

            // ─── أساسيات / Basics (Part 2) — exit, assert, range, file I/O basics ───
            registry.registerModule({"\xd8\xa3\xd8\xb3\xd8\xa7\xd8\xb3\xd9\x8a\xd8\xa7\xd8\xaa", // أساسيات

                                     "\xd8\xae\xd8\xb1\xd9\x88\xd8\xac\xd8\x8c \xd9\x85\xd8\xaf\xd9\x89\xd8\x8c \xd8\xaa\xd8\xa3\xd9\x83\xd8\xaf\xd8\x8c \xd9\x85\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa",
                                     // خروج، مدى، تأكد، ملفات
                                     [](Interpreter &interp)
                                     { registerBuiltinsBasics(interp); },
                                     {}});

            // ─── رياضيات / Math (Part 6) — log, clamp, embedded IO ───
            registry.registerModule({"\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa7\xd8\xaa", // رياضيات
                                     "\xd9\x84\xd9\x88\xd8\xba\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xaa\xd9\x85\xd8\x8c \xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd8\xaf\xd8\x8c \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84 \xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa9",
                                     // لوغاريتم، تقييد، دوال رياضية
                                     [](Interpreter &interp)
                                     { registerBuiltinsMath(interp); },
                                     {}});

            // ─── تأكيدات / Assertions (Part 7) — assert, hash, crypto, FFI ───
            registry.registerModule({"\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf\xd8\xa7\xd8\xaa", // تأكيدات
                                     "\xd8\xaa\xd8\xa3\xd9\x83\xd8\xaf\xd8\x8c \xd9\x87\xd8\xa7\xd8\xb4\xd8\x8c \xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1\xd8\x8c FFI",
                                     // تأكد، هاش، تشفير، FFI
                                     [](Interpreter &interp)
                                     { registerBuiltinsAssertions(interp); },
                                     {}});

            // ─── تزامن / Async (Part 8) — spawn, await, channels, threads, atomics ───
            registry.registerModule({"\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // تزامن_متقدم

                                     "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd9\x82\xd9\x86\xd9\x88\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xb0\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xaa\xd8\x8c OOP",
                                     // خيوط، قنوات، ذريات، OOP
                                     [](Interpreter &interp)
                                     { registerBuiltinsAsync(interp); },
                                     {}});

            // ─── خرائط / Maps & Advanced (Part 10) — maps, functional, date, regex, JSON ───
            registry.registerModule({"\xd8\xae\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xb7", // خرائط

                                     "\xd8\xae\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xb7\xd8\x8c JSON\xd8\x8c \xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae\xd8\x8c \xd8\xaa\xd8\xb9\xd8\xa8\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa \xd9\x86\xd9\x85\xd8\xb7\xd9\x8a\xd8\xa9",
                                     // خرائط، JSON، تاريخ، تعبيرات نمطية
                                     [](Interpreter &interp)
                                     { registerBuiltinsMaps(interp); },
                                     {}});

            // ─── منصة / Platform (Part 39) — platform detection ───
            registry.registerModule({"\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // منصة

                                     "\xd9\x83\xd8\xb4\xd9\x81 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9 \xd9\x88\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84",
                                     // كشف المنصة ونظام التشغيل
                                     [](Interpreter &interp)
                                     { registerBuiltinsPlatform(interp); },
                                     {}});

            // ─── مقابس / Sockets (Part 40) — TCP, UDP ───
            registry.registerModule({kModSockets,
                                     "وصلات TCP، مراسيل UDP، إعدادات الشبكة",
                                     [](Interpreter &interp)
                                     { registerBuiltinsSockets(interp); },
                                     {}});

            // ─── عمليات / Processes — fork/execvp، waitpid، pipe، close، open ───
            //     (AR) البدائيّات التي تُغني برنامج ص عن system()، فتصير الصدَفة
            //          برنامجًا بص لا كودًا بـC++ داخل sad-repl.
            registry.registerModule({kModProcesses,
                                     "تشغيل برنامج، انتظار، أنبوب، توجيه، وصف ملفّ",
                                     [](Interpreter &interp)
                                     { registerBuiltinsProcesses(interp); },
                                     {}});

            // ─── شبكة / Network HTTP (Part 40b) — HTTP client, server, utilities ───
            registry.registerModule({kModNetwork,
                                     "عميل HTTP، خادم HTTP، عناوين، أدوات شبكة",
                                     [](Interpreter &interp)
                                     { registerBuiltinsHttp(interp); },
                                     {}});

            // ─── ويبسوكت / WebSocket (Part 40c) — bidirectional WS client + server ───
            registry.registerModule({"ويبسوكت",
                                     "عميل وخادم WebSocket، بث، غرف",
                                     [](Interpreter &interp)
                                     { registerBuiltinsWebSocket(interp); },
                                     {}});

            // ─── شبكة_لامركزية / SadNet Decentralized Network (Part 41) ───
            registry.registerModule({"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x84\xd8\xa7\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2\xd9\x8a\xd8\xa9", // شبكة_لامركزية
                                     "DHT\xd8\x8c E2EE\xd8\x8c \xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86 \xd9\x85\xd9\x88\xd8\xb2\xd8\xb9\xd8\x8c CDN\xd8\x8c DNS\xd8\x8c \xd8\xa8\xd8\xab",
                                     // DHT، E2EE، تخزين موزع، CDN، DNS، بث
                                     [](Interpreter &interp)
                                     { registerBuiltinsSadNet(interp); },
                                     {}});

            // ═══════════════════════════════════════════════════════════════
            // (AR) وحدات النظام المنخفض — تُحمَّل عند الاستيراد فقط
            // (EN) Low-level system modules — loaded on import only
            // ═══════════════════════════════════════════════════════════════

            // ─── نظام_منخفض / Low-Level OS (Parts 19, 20, 21, 22) ───
            registry.registerModule({"\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6", // نظام_منخفض
                                     "CPU\xd8\x8c APIC\xd8\x8c UEFI\xd8\x8c ACPI\xd8\x8c GPU \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd9\x86\xd8\xb8\xd8\xa7\xd9\x85",
                                     // CPU، APIC، UEFI، ACPI، GPU بدون نظام
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelCPU(interp);
                                         registerBuiltinsKernelUEFI(interp);
                                         registerBuiltinsKernelACPI(interp);
                                         registerBuiltinsKernelGPU(interp);
                                     },
                                     {}});

            // ─── يو_إس_بي ───
            registry.registerModule({"\xd9\x8a\xd9\x88_\xd8\xa5\xd8\xb3_\xd8\xa8\xd9\x8a", // يو_إس_بي
                                     "USB/HID\xd8\x8c \xd9\x84\xd9\x88\xd8\xad\xd8\xa9 \xd9\x85\xd9\x81\xd8\xa7\xd8\xaa\xd9\x8a\xd8\xad\xd8\x8c \xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9",
                                     // USB/HID، لوحة مفاتيح، فأرة
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelUSB(interp);
                                     },
                                     {}});

            // ─── تخزين / Storage (Part 24) — NVMe, AHCI, S.M.A.R.T ───
            registry.registerModule({"\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // تخزين
                                     "NVMe\xd8\x8c AHCI\xd8\x8c S.M.A.R.T\xd8\x8c \xd8\xaa\xd9\x82\xd8\xb3\xd9\x8a\xd9\x85\xd8\xa7\xd8\xaa",
                                     // NVMe، AHCI، S.M.A.R.T، تقسيمات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelStorage(interp);
                                     },
                                     {}});

            // ─── شبكة_منخفضة / Low-Level Network Stack (Part 25) — Ethernet, ARP, IP, UDP ───
            registry.registerModule({"شبكة_منخفضة", // شبكة_منخفضة
                                     "Ethernet\xd8\x8c ARP\xd8\x8c IPv4\xd8\x8c ICMP\xd8\x8c UDP",
                                     // Ethernet، ARP، IPv4، ICMP، UDP
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelNetwork(interp);
                                     },
                                     {}});

            // ─── صوت / Audio (Part 26) — AC'97, HDA, PC Speaker ───
            registry.registerModule({"\xd8\xb5\xd9\x88\xd8\xaa", // صوت
                                     "AC'97\xd8\x8c HDA\xd8\x8c PC Speaker\xd8\x8c \xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84",
                                     // AC'97، HDA، PC Speaker، تسجيل
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelAudio(interp);
                                     },
                                     {}});

            // ─── مؤقتات / Timers (Part 27) — PIT, RTC, Stopwatch ───
            registry.registerModule({"\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // مؤقتات
                                     "PIT 8254\xd8\x8c RTC\xd8\x8c \xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81",
                                     // PIT 8254، RTC، ساعة إيقاف
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelTimers(interp);
                                     },
                                     {}});

            // ─── تسلسلي / Serial UART (Part 28) — COM1-COM4 ───
            registry.registerModule({"\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a", // تسلسلي
                                     "UART 16550\xd8\x8c COM1-COM4\xd8\x8c loopback",
                                     // UART 16550، COM1-COM4، loopback
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelSerial(interp);
                                     },
                                     {}});

            // ─── ذاكرة / Memory (Part 35) — Allocator, GC ───
            registry.registerModule({"\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", // ذاكرة
                                     "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5 \xd8\xa7\xd9\x84\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9\xd8\x8c \xd8\xac\xd8\xa7\xd9\x85\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x82\xd9\x85\xd8\xa7\xd9\x85\xd8\xa9\xd8\x8c \xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa",
                                     // مخصص الذاكرة، جامع القمامة، إحصائيات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelMemory(interp);
                                     },
                                     {}});

            // ─── خيوط / Threads (Part 36) — Thread, Mutex, Semaphore ───
            registry.registerModule({"\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7", // خيوط
                                     "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd8\xa3\xd9\x82\xd9\x81\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa \xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",
                                     // خيوط، أقفال، إشارات تزامن
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelThreads(interp);
                                     },
                                     {}});

            // ─── واجهة_خارجية / FFI (Part 37) — Dynamic library loading ───
            registry.registerModule({"\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9_\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a\xd8\xa9", // واجهة_خارجية
                                     "FFI\xd8\x8c \xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84 \xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xaa C\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84",
                                     // FFI، تحميل مكتبات C، استدعاء دوال
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsFFI(interp);
                                     },
                                     {}});

            // ─── استثناءات / Exceptions & Sandbox (Part 38) ───
            registry.registerModule({"\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa", // استثناءات
                                     "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xaa\xd8\xaa\xd8\xa8\xd8\xb9 \xd9\x85\xd9\x83\xd8\xaf\xd8\xb3\xd8\x8c \xd8\xa3\xd8\xb0\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa",
                                     // استثناءات، تتبع مكدس، أذونات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsExceptions(interp);
                                     },
                                     {}});

            // ─── مقابس / Sockets (Part 40) — TCP & UDP عالي المستوى ───
            registry.registerModule({"\xd9\x85\xd9\x82\xd8\xa7\xd8\xa8\xd8\xb3", // مقابس
                                     "TCP\xd8\x8c UDP\xd8\x8c \xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84",
                                     // TCP، UDP، اتصال، إرسال، استقبال
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsSockets(interp); // TCP & UDP high-level sockets
                                     },
                                     {}});

            // ─── تشفير / Crypto (Part 7 crypto + Part 41 BLAKE3) — SHA-256, تشفير/فك_تشفير، بلايك3 ───
            registry.registerModule({"\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // تشفير
                                     "SHA-256\xd8\x8c \xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1 CTR\xd8\x8c \xd9\x87\xd8\xa7\xd8\xb4\xd8\x8c BLAKE3",
                                     // SHA-256، تشفير CTR، هاش، BLAKE3
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsAssertions(interp); // SHA-256, encrypt/decrypt, hash
                                         registerBuiltinsCrypto(interp);     // BLAKE3, keyed hash
                                     },
                                     {}});

            // ═══════════════════════════════════════════════════════════════
            // (AR) م2-أ (sadlang-rfcs#10): وحدات الواجهات (رسومات/_صوت/...) لم تَعُد
            //      مُسجَّلة هنا؛ يسجّلها sad_graphics_bridge عبر مزوّد خارجيّ. القلب لا يعرف
            //      sad_ui. يثبّت sad-run المزوّد قبل إنشاء المفسّر.
            // (EN) Phase 2-A: UI modules are no longer registered here; sad_graphics_bridge
            //      registers them via an external provider. The core does not know sad_ui.
            // ═══════════════════════════════════════════════════════════════
            for (auto &provider : externalModuleProviders())
                provider();
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) تعريف دالة التسجيل المصغّر
        // (EN) Forward declaration for core-only registration
        // ═════════════════════════════════════════════════════════════════
        extern void registerCoreIOBuiltins(Interpreter &interpreter);

        // ═════════════════════════════════════════════════════════════════
        // (AR) نقطة الدخول الرئيسية: تسجيل الدوال الأساسية + تهيئة سجل الوحدات
        // (EN) Main entry point: register core functions + initialize module registry
        // ═════════════════════════════════════════════════════════════════
        void registerBuiltinFunctions(Interpreter &interpreter)
        {
            // ═══ (AR) الأساسية — I/O وتحويل الأنواع فقط ═══
            // ═══ (EN) Core — I/O and type conversion only ═══
            registerCoreIOBuiltins(interpreter); // اطبع، اقرأ، طول، نوع، نص، رقم، عشري

            // ═══════════════════════════════════════════════════════════════
            // (AR) جميع الوحدات الأخرى تُحمَّل عند الاستيراد فقط
            // (EN) All other modules are loaded on import only
            //
            // الوحدات المتاحة للاستيراد:
            //   استورد نصوص         — دوال النصوص والسلاسل (Part 1)
            //   استورد أساسيات       — خروج، مدى، تأكد، ملفات (Part 2)
            //   استورد رياضيات       — دوال رياضية (Part 6)
            //   استورد تأكيدات       — تأكد، هاش، تشفير، FFI (Part 7)
            //   استورد تزامن_متقدم   — خيوط، قنوات، ذريات (Part 8)
            //   استورد خرائط         — خرائط، JSON، تاريخ، regex (Part 10)
            //   استورد منصة          — كشف المنصة (Part 39)
            //   استورد مقابس          — وصلات TCP، مراسيل UDP (Part 40)
            //   استورد شبكة           — عميل HTTP، خادم HTTP، أدوات شبكة (Part 40b)
            // ═══════════════════════════════════════════════════════════════
            auto &registry = BuiltinModuleRegistry::getInstance();
            registry.setInterpreter(&interpreter);
            registerBuiltinModules();
        }

    } // namespace Interpreter
} // namespace Sad
