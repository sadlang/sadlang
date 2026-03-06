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
 *   - رسوميات (Graphics)    → Parts 3-5, 9         — عند `استورد رسوميات`
 *   - واجهات (UI/Widgets)   → Parts 7, 11-18       — عند `استورد واجهات`
 *   - نظام_منخفض (OS)       → Parts 19-22          — عند `استورد نظام_منخفض`
 *   - usb                   → Part 23               — عند `استورد usb`
 *   - محرك (Game Engine)    → Parts 29-34           — عند `استورد محرك`
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_module_registry.h"
#include "function_manager.h"

#include <algorithm>

namespace Sad {
namespace Interpreter {

// ═════════════════════════════════════════════════════════════════
// Extern declarations — all 34 part files
// ═════════════════════════════════════════════════════════════════
extern void registerBuiltinsPart1(Interpreter& interpreter);
extern void registerBuiltinsPart2(Interpreter& interpreter);
extern void registerBuiltinsPart3(Interpreter& interpreter);
extern void registerBuiltinsPart4(Interpreter& interpreter);
extern void registerBuiltinsPart5(Interpreter& interpreter);
extern void registerBuiltinsPart6(Interpreter& interpreter);
extern void registerBuiltinsPart7(Interpreter& interpreter);
extern void registerBuiltinsPart8(Interpreter& interpreter);
extern void registerBuiltinsPart9(Interpreter& interpreter);
extern void registerBuiltinsPart10(Interpreter& interpreter);
extern void registerBuiltinsPart11(Interpreter& interpreter);
extern void registerBuiltinsPart12(Interpreter& interpreter);
extern void registerBuiltinsPart13(Interpreter& interpreter);
extern void registerBuiltinsPart14(Interpreter& interpreter);
extern void registerBuiltinsPart15(Interpreter& interpreter);
extern void registerBuiltinsPart16(Interpreter& interpreter);
extern void registerBuiltinsPart17(Interpreter& interpreter);
extern void registerBuiltinsPart18(Interpreter& interpreter);
/* omitted */
/* omitted */
/* omitted */
/* omitted */
/* omitted */
/* omitted */
/* omitted */
/* omitted */
/* omitted */
/* omitted */
extern void registerBuiltinsPart29(Interpreter& interpreter);
extern void registerBuiltinsPart30(Interpreter& interpreter);
extern void registerBuiltinsPart31(Interpreter& interpreter);
extern void registerBuiltinsPart32(Interpreter& interpreter);
extern void registerBuiltinsPart33(Interpreter& interpreter);
extern void registerBuiltinsPart34(Interpreter& interpreter);
/* omitted */
/* omitted */
/* omitted */
/* omitted */
extern void registerBuiltinsPart39(Interpreter& interpreter);

// ═════════════════════════════════════════════════════════════════
// BuiltinModuleRegistry::loadModule — implemented here because it
// needs FunctionManager (which requires interpreter_core.h)
// ═════════════════════════════════════════════════════════════════
bool BuiltinModuleRegistry::loadModule(const std::string& name) {
    std::string canonical = resolveAlias(name);
    if (loadedModules_.count(canonical) > 0) return true;

    auto it = modules_.find(canonical);
    if (it == modules_.end() || !interpreter_) return false;

    // (AR) لقطة أسماء الدوال قبل التسجيل
    // (EN) Snapshot function names before registration
    auto& fm = interpreter_->getFunctionManager();
    auto beforeNames = fm.getFunctionNames();
    std::unordered_set<std::string> beforeSet(beforeNames.begin(), beforeNames.end());

    // (AR) استدعاء دالة التسجيل
    // (EN) Call the registration function
    it->second.registrar(*interpreter_);

    // (AR) حساب الدوال الجديدة المُسجَّلة
    // (EN) Compute newly registered function names
    auto afterNames = fm.getFunctionNames();
    it->second.exportedFunctions.clear();
    for (const auto& fname : afterNames) {
        if (beforeSet.find(fname) == beforeSet.end()) {
            it->second.exportedFunctions.push_back(fname);
        }
    }

    loadedModules_.insert(canonical);
    return true;
}

// ═════════════════════════════════════════════════════════════════
// (AR) تسجيل الوحدات المُضمّنة في السجل المركزي
// (EN) Register builtin modules in the central registry
// ═════════════════════════════════════════════════════════════════
static void registerBuiltinModules() {
    auto& registry = BuiltinModuleRegistry::getInstance();

    // ─── رسوميات / Graphics (Parts 3, 4, 5, 9) ───
    registry.registerModule({
        "\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa7\xd8\xaa", // رسوميات
        {"graphics",
         "\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa7\xd8\xaa_sdl"}, // رسوميات_sdl
        "\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa7\xd8\xaa SDL\xd8\x8c \xd8\xac\xd8\xb3\xd9\x8a\xd9\x85\xd8\xa7\xd8\xaa\xd8\x8c \xd9\x81\xd9\x8a\xd8\xb2\xd9\x8a\xd8\xa7\xd8\xa1\xd8\x8c AI",
        // رسوميات SDL، جسيمات، فيزياء، AI
        [](Interpreter& interp) {
            registerBuiltinsPart3(interp);   // Checkbox, particles, effects
            registerBuiltinsPart4(interp);   // Physics, 3D, AI pathfinding
            registerBuiltinsPart5(interp);   // AI agents, Camera2D
            registerBuiltinsPart9(interp);   // Window/SDL renderer/audio
        },
        {} // exportedFunctions — populated dynamically
    });

    // ─── واجهات / UI Widgets (Parts 7, 11-18) ───
    registry.registerModule({
        "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa", // واجهات
        {"ui", "widgets",
         "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85"}, // واجهات_المستخدم
        "\xd8\xb9\xd9\x86\xd8\xa7\xd8\xb5\xd8\xb1 UI\xd8\x8c Flutter\xd8\x8c Material\xd8\x8c Cupertino",
        // عناصر UI، Flutter، Material، Cupertino
        [](Interpreter& interp) {
            registerBuiltinsPart7(interp);   // Old UI composites, mobile, FFI
            registerBuiltinsPart11(interp);  // 109 widget factories
            registerBuiltinsPart12(interp);  // Widget properties, tree ops
            registerBuiltinsPart13(interp);  // App lifecycle, navigator, state
            registerBuiltinsPart14(interp);  // Opacity, shadow, tooltip
            registerBuiltinsPart15(interp);  // Queries, scroll, alignment
            registerBuiltinsPart16(interp);  // 50 composite Flutter widgets
            registerBuiltinsPart17(interp);  // Property setters/getters
            registerBuiltinsPart18(interp);  // Material3, Cupertino, Data, Forms
            registerBuiltinsPart39(interp);  // Unified UI platform (هيئ_واجهة_موحدة, المنصة...)
        },
        {}
    });

    // ─── نظام_منخفض / Low-Level OS (Parts 19, 20, 21, 22) ───
    registry.registerModule({
        "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6", // نظام_منخفض
        {"lowlevel", "os", "kernel",
         "\xd9\x86\xd9\x88\xd8\xa7\xd8\xa9"}, // نواة
        "CPU\xd8\x8c APIC\xd8\x8c UEFI\xd8\x8c ACPI\xd8\x8c GPU \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd9\x86\xd8\xb8\xd8\xa7\xd9\x85",
        // CPU، APIC، UEFI، ACPI، GPU بدون نظام
        [](Interpreter& interp) {
/* omitted */
/* omitted */
/* omitted */
/* omitted */
        },
        {}
    });

    // ─── usb ───
    registry.registerModule({
        "usb",
        {"\xd9\x8a\xd9\x88_\xd8\xa5\xd8\xb3_\xd8\xa8\xd9\x8a",   // يو_إس_بي
         "hid",
         "\xd8\xa3\xd8\xac\xd9\x87\xd8\xb2\xd8\xa9_\xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84"}, // أجهزة_إدخال
        "USB/HID\xd8\x8c \xd9\x84\xd9\x88\xd8\xad\xd8\xa9 \xd9\x85\xd9\x81\xd8\xa7\xd8\xaa\xd9\x8a\xd8\xad\xd8\x8c \xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9",
        // USB/HID، لوحة مفاتيح، فأرة
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── تخزين / Storage (Part 24) — NVMe, AHCI, S.M.A.R.T ───
    registry.registerModule({
        "\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // تخزين
        {"storage", "nvme", "ahci",
         "\xd8\xa3\xd9\x82\xd8\xb1\xd8\xa7\xd8\xb5"}, // أقراص
        "NVMe\xd8\x8c AHCI\xd8\x8c S.M.A.R.T\xd8\x8c \xd8\xaa\xd9\x82\xd8\xb3\xd9\x8a\xd9\x85\xd8\xa7\xd8\xaa",
        // NVMe، AHCI، S.M.A.R.T، تقسيمات
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── شبكة / Network Stack (Part 25) — Ethernet, ARP, IP, UDP ───
    registry.registerModule({
        "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", // شبكة
        {"network", "net",
         "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6\xd8\xa9"}, // شبكة_منخفضة
        "Ethernet\xd8\x8c ARP\xd8\x8c IPv4\xd8\x8c ICMP\xd8\x8c UDP",
        // Ethernet، ARP، IPv4، ICMP، UDP
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── صوت / Audio (Part 26) — AC'97, HDA, PC Speaker ───
    registry.registerModule({
        "\xd8\xb5\xd9\x88\xd8\xaa", // صوت
        {"audio",
         "\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6"}, // صوت_منخفض
        "AC'97\xd8\x8c HDA\xd8\x8c PC Speaker\xd8\x8c \xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84",
        // AC'97، HDA، PC Speaker، تسجيل
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── مؤقتات / Timers (Part 27) — PIT, RTC, Stopwatch ───
    registry.registerModule({
        "\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // مؤقتات
        {"timers", "timer", "rtc",
         "\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9"}, // ساعة
        "PIT 8254\xd8\x8c RTC\xd8\x8c \xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81",
        // PIT 8254، RTC، ساعة إيقاف
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── تسلسلي / Serial UART (Part 28) — COM1-COM4 ───
    registry.registerModule({
        "\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a", // تسلسلي
        {"serial", "uart", "com",
         "\xd9\x85\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0_\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a\xd8\xa9"}, // منافذ_تسلسلية
        "UART 16550\xd8\x8c COM1-COM4\xd8\x8c loopback",
        // UART 16550، COM1-COM4، loopback
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── محرك / Game Engine (Parts 29-34) — ECS, Events, State, Profiler, Serialization, FB, 3D, Shapes, PostFX ───
    registry.registerModule({
        "\xd9\x85\xd8\xad\xd8\xb1\xd9\x83", // محرك
        {"engine", "game_engine",
         "\xd9\x85\xd8\xad\xd8\xb1\xd9\x83_\xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd8\xa8",  // محرك_العاب
         "\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa7\xd8\xaa_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85\xd8\xa9"}, // رسوميات_متقدمة
        "ECS\xd8\x8c EventBus\xd8\x8c StateMachine\xd8\x8c Profiler\xd8\x8c JSON\xd8\x8c Resources\xd8\x8c Framebuffer\xd8\x8c 3D\xd8\x8c Shapes\xd8\x8c PostFX",
        // ECS، EventBus، StateMachine، Profiler، JSON، Resources، Framebuffer، 3D، Shapes، PostFX
        [](Interpreter& interp) {
            registerBuiltinsPart29(interp);  // ECS & EventBus
            registerBuiltinsPart30(interp);  // StateMachine & Profiler
            registerBuiltinsPart31(interp);  // Serialization (JSON) & Resource Manager
            registerBuiltinsPart32(interp);  // OS Framebuffer (GPU-free)
            registerBuiltinsPart33(interp);  // 3D Renderer & Texture Atlas
            registerBuiltinsPart34(interp);  // Advanced Shapes & Post-Processing
        },
        {}
    });

    // ─── ذاكرة / Memory (Part 35) — Allocator, GC ───
    registry.registerModule({
        "\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", // ذاكرة
        {"memory", "mem",
         "\xd8\xaa\xd8\xae\xd8\xb5\xd9\x8a\xd8\xb5",  // تخصيص
         "\xd9\x82\xd9\x85\xd8\xa7\xd9\x85\xd8\xa9"}, // قمامة
        "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5 \xd8\xa7\xd9\x84\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9\xd8\x8c \xd8\xac\xd8\xa7\xd9\x85\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x82\xd9\x85\xd8\xa7\xd9\x85\xd8\xa9\xd8\x8c \xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa",
        // مخصص الذاكرة، جامع القمامة، إحصائيات
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── خيوط / Threads (Part 36) — Thread, Mutex, Semaphore ───
    registry.registerModule({
        "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7", // خيوط
        {"threads", "thread",
         "\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",  // تزامن
         "\xd8\xa3\xd9\x82\xd9\x81\xd8\xa7\xd9\x84"}, // أقفال
        "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd8\xa3\xd9\x82\xd9\x81\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa \xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",
        // خيوط، أقفال، إشارات تزامن
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── واجهة_خارجية / FFI (Part 37) — Dynamic library loading ───
    registry.registerModule({
        "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9_\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a\xd8\xa9", // واجهة_خارجية
        {"ffi", "foreign",
         "\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xaa_\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a\xd8\xa9"}, // مكتبات_خارجية
        "FFI\xd8\x8c \xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84 \xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xaa C\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84",
        // FFI، تحميل مكتبات C، استدعاء دوال
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });

    // ─── استثناءات / Exceptions & Sandbox (Part 38) ───
    registry.registerModule({
        "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa", // استثناءات
        {"exceptions", "errors",
         "\xd8\xa3\xd9\x85\xd8\xa7\xd9\x86",  // أمان
         "sandbox"},
        "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xaa\xd8\xaa\xd8\xa8\xd8\xb9 \xd9\x85\xd9\x83\xd8\xaf\xd8\xb3\xd8\x8c \xd8\xa3\xd8\xb0\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa",
        // استثناءات، تتبع مكدس، أذونات
        [](Interpreter& interp) {
/* omitted */
        },
        {}
    });
}

// ═════════════════════════════════════════════════════════════════
// (AR) نقطة الدخول الرئيسية: تسجيل الدوال الأساسية + تهيئة سجل الوحدات
// (EN) Main entry point: register core functions + initialize module registry
// ═════════════════════════════════════════════════════════════════
void registerBuiltinFunctions(Interpreter& interpreter) {
    // ═══ (AR) الأساسية — تُسجَّل دائماً ═══
    // ═══ (EN) Core — always registered ═══
    registerBuiltinsPart1(interpreter);   // print, input, len, append, remove, string ops, basic math, type checks
    registerBuiltinsPart2(interpreter);   // exit, assert, range, file I/O basics, window/renderer basics
    registerBuiltinsPart6(interpreter);   // log/clamp math, embedded IO (outb/inb/poke), mobile info
    registerBuiltinsPart8(interpreter);   // async/concurrency (spawn, await, channels, threads, atomics), OOP
    registerBuiltinsPart10(interpreter);  // maps, functional programming, date/time, regex, JSON, sets, arrays

    // ═══ (AR) سجل الوحدات المُضمّنة — تُحمَّل عند الاستيراد ═══
    // ═══ (EN) Builtin module registry — loaded on import ═══
    auto& registry = BuiltinModuleRegistry::getInstance();
    registry.setInterpreter(&interpreter);
    registerBuiltinModules();
}

} // namespace Interpreter
} // namespace Sad
