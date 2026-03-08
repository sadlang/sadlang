/**
 * @file builtin_registry_part6.cpp
 * @brief (AR) جزء من تسجيل الدوال المضمنة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"
#include "graphics/graphics_module.h"
#include "graphics/sad_particles.h"
#include "graphics/sad_effects.h"
#include "graphics/sad_physics.h"
#include "graphics/sad_scene3d.h"
#include "graphics/sad_ai.h"
#include "graphics/sad_camera2d.h"
#include "graphics/sad_charts.h"
#include "graphics/sad_audio.h"
#include "graphics/sad_tilemap.h"
#include "graphics/sad_islamic_art.h"
#include "graphics/sad_dhikr.h"
#include "graphics/sad_qibla.h"
#include "graphics/sad_prayer.h"
#include "graphics/sad_hijri.h"
#include "graphics/sad_android.h"
#include "mobile/mobile_module.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <set>
#include <regex>
#include <numeric>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <atomic>
#include <queue>
#include <unordered_map>
#include <condition_variable>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsPart6(Interpreter& interpreter) {
    // ─── شاشة_إلى_عالم_س / cam2d_screen_to_world_x ───
    // تحويل إحداثي أفقي من نظام الشاشة إلى نظام العالم.
    // مفيد لتحديد النقطة التي نقر عليها المستخدم في عالم اللعبة.
    // المعاملات: معرّف الكاميرا، الإحداثي الأفقي على الشاشة.
    auto cam2d_screenToWorldX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float sx = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::screenToWorldX(camId, sx));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_إلى_عالم_س", cam2d_screenToWorldX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_screen_to_world_x", cam2d_screenToWorldX_func);

    // ─── شاشة_إلى_عالم_ص / cam2d_screen_to_world_y ───
    // تحويل إحداثي عمودي من نظام الشاشة إلى نظام العالم.
    // المعاملات: معرّف الكاميرا، الإحداثي العمودي على الشاشة.
    auto cam2d_screenToWorldY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float sy = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::screenToWorldY(camId, sy));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_إلى_عالم_ص", cam2d_screenToWorldY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_screen_to_world_y", cam2d_screenToWorldY_func);

    // ─── ابدأ_كاميرا / cam2d_begin ───
    // تفعيل الكاميرا — كل عمليات الرسم بعد هذا السطر ستُحوَّل حسب
    // موضع الكاميرا وتكبيرها ودورانها. يجب استدعاء أنهِ_كاميرا بعد الانتهاء.
    // المعامل: معرّف الكاميرا.
    auto cam2d_begin_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::beginCamera(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ابدأ_كاميرا", cam2d_begin_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_begin", cam2d_begin_func);

    // ─── أنهِ_كاميرا / cam2d_end ───
    // إنهاء تحويل الكاميرا — العودة لنظام الإحداثيات العادي (الشاشة).
    // كل الرسم بعد هذا السطر لن يتأثر بالكاميرا (مفيد لعناصر الواجهة HUD).
    auto cam2d_end_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::camera2d::endCamera();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنهِ_كاميرا", cam2d_end_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_end", cam2d_end_func);

    // ─── إزاحة_كاميرا_س / cam2d_get_offset_x ───
    // الحصول على الإزاحة الأفقية الحالية للكاميرا (تشمل التنعيم والاهتزاز).
    // مفيد للاستخدام اليدوي مع SDL بدلاً من beginCamera/endCamera.
    // المعامل: معرّف الكاميرا.
    auto cam2d_getOffsetX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getOffsetX(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("إزاحة_كاميرا_س", cam2d_getOffsetX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_offset_x", cam2d_getOffsetX_func);

    // ─── إزاحة_كاميرا_ص / cam2d_get_offset_y ───
    // الحصول على الإزاحة العمودية الحالية للكاميرا (تشمل التنعيم والاهتزاز).
    // المعامل: معرّف الكاميرا.
    auto cam2d_getOffsetY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getOffsetY(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("إزاحة_كاميرا_ص", cam2d_getOffsetY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_offset_y", cam2d_getOffsetY_func);

    // ===================================================================
    // Phase 23: Advanced Math Functions (log, exp, clamp, etc.)
    // ===================================================================
    
    // Natural logarithm / لوغاريتم طبيعي
    auto math_log_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لوغ", math_log_func);
    interpreter.getFunctionManager().registerBuiltinFunction("log", math_log_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم", math_log_func);
    
    // Base-10 logarithm / لوغاريتم عشري
    auto math_log10_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log10(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لوغ10", math_log10_func);
    interpreter.getFunctionManager().registerBuiltinFunction("log10", math_log10_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم_عشري", math_log10_func);
    
    // Base-2 logarithm / لوغاريتم ثنائي
    auto math_log2_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log2(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لوغ2", math_log2_func);
    interpreter.getFunctionManager().registerBuiltinFunction("log2", math_log2_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم_ثنائي", math_log2_func);
    
    // Exponential e^x / أسّي
    auto math_exp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::exp(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أسّي", math_exp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exp", math_exp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أُسّي", math_exp_func);
    
    // Clamp / تقييد
    auto math_clamp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::clamp(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قيّد", math_clamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("clamp", math_clamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تقييد", math_clamp_func);
    
    // Truncate / اقتطاع
    auto math_trunc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::trunc(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقتطاع", math_trunc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("trunc", math_trunc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("اقتطع", math_trunc_func);
    
    // Modulo / باقي القسمة
    auto math_mod_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::mod(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("باقي", math_mod_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mod", math_mod_func);
    interpreter.getFunctionManager().registerBuiltinFunction("باقي_القسمة", math_mod_func);
    
    // Sign / إشارة
    auto math_sign_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::sign(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إشارة", math_sign_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sign", math_sign_func);
    
    // Pi constant / ثابت باي
    auto math_pi_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::pi());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("باي", math_pi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pi", math_pi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ط", math_pi_func);
    
    // e constant / ثابت إي
    auto math_e_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::e());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إي", math_e_func);
    interpreter.getFunctionManager().registerBuiltinFunction("e_const", math_e_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أويلر", math_e_func);
    
    // ===================================================================
    // Phase 24: System Functions (environment, directory, execution)
    // ===================================================================
    
    // Get environment variable / الحصول على متغير بيئة
    auto sys_getenv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::getEnv(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احصل_بيئة", sys_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("getEnv", sys_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("get_env", sys_getenv_func);
    
    // Set environment variable / تعيين متغير بيئة
    auto sys_setenv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::setEnv(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_بيئة", sys_setenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("setEnv", sys_setenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("set_env", sys_setenv_func);
    
    // Get current directory / الحصول على المجلد الحالي
    auto sys_curdir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::currentDir(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مجلد_حالي", sys_curdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("currentDir", sys_curdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("المجلد_الحالي", sys_curdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pwd", sys_curdir_func);
    
    // Change directory / تغيير المجلد
    auto sys_chdir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::changeDir(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("غيّر_مجلد", sys_chdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("changeDir", sys_chdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cd", sys_chdir_func);
    
    // Execute command / تنفيذ أمر
    auto sys_execute_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::execute(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نفّذ", sys_execute_func);
    interpreter.getFunctionManager().registerBuiltinFunction("execute", sys_execute_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تنفيذ", sys_execute_func);
    interpreter.getFunctionManager().registerBuiltinFunction("system", sys_execute_func);
    
    // Get platform / المنصة
    auto sys_platform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::platform());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("منصة", sys_platform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("platform", sys_platform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("المنصة", sys_platform_func);
    
    // Get OS name / اسم نظام التشغيل
    auto sys_osname_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::osName());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نظام", sys_osname_func);
    interpreter.getFunctionManager().registerBuiltinFunction("osName", sys_osname_func);
    interpreter.getFunctionManager().registerBuiltinFunction("os", sys_osname_func);
    
    // Get timestamp / الطابع الزمني
    auto sys_timestamp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::timestamp());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("طابع_زمني", sys_timestamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timestamp", sys_timestamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("الطابع_الزمني", sys_timestamp_func);
    
    // Get clock time / وقت الساعة
    auto sys_clock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::clock_time());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("ساعة", sys_clock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("clock", sys_clock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("وقت_الساعة", sys_clock_func);

    // ===================================================================
    // (AR) دوال برمجة أنظمة التشغيل — محاكاة في المفسر
    // (EN) OS Development Functions — Simulation in interpreter
    // ===================================================================
    // (AR) هذه الدوال تحاكي عمليات النظام المنخفض المستوى في المفسر.
    //      في المترجم (sadc)، تُولّد هذه العمليات كود أصلي (inline asm).
    //      في المفسر (sad)، تُحاكى بطباعة رسائل تشخيصية.
    // ===================================================================

    // (AR) محاكاة كتابة منفذ — outb(port, value)
    auto port_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب: يحتاج معاملين (منفذ، قيمة)");
        int port = args[0]->toInt();
        int val = args[1]->toInt();
        std::cout << "[OS-SIM] outb(0x" << std::hex << port << ", 0x" << val << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_اكتب", port_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("port_write", port_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("outb", port_write_func);

    // (AR) محاكاة قراءة منفذ — inb(port)
    auto port_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ: يحتاج معامل واحد (رقم المنفذ)");
        int port = args[0]->toInt();
        std::cout << "[OS-SIM] inb(0x" << std::hex << port << std::dec << ") -> 0x00" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_اقرأ", port_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("port_read", port_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("inb", port_read_func);

    // (AR) محاكاة كتابة ذاكرة — poke(addr, value)
    auto mem_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("ذاكرة_اكتب: يحتاج معاملين (عنوان، قيمة)");
        int addr = args[0]->toInt();
        int val = args[1]->toInt();
        std::cout << "[OS-SIM] poke(0x" << std::hex << addr << ", 0x" << val << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ذاكرة_اكتب", mem_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_write", mem_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("poke", mem_write_func);

    // (AR) محاكاة قراءة ذاكرة — peek(addr)
    auto mem_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ذاكرة_اقرأ: يحتاج معامل واحد (عنوان)");
        int addr = args[0]->toInt();
        std::cout << "[OS-SIM] peek(0x" << std::hex << addr << std::dec << ") -> 0x00" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ذاكرة_اقرأ", mem_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_read", mem_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("peek", mem_read_func);

    // (AR) محاكاة مقاطعة — interrupt(n)
    auto interrupt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("مقاطعة: يحتاج معامل واحد (رقم المقاطعة)");
        int n = args[0]->toInt();
        std::cout << "[OS-SIM] int 0x" << std::hex << n << std::dec << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مقاطعة", interrupt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("interrupt", interrupt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("int_call", interrupt_func);

    // (AR) محاكاة إيقاف المعالج — hlt
    auto halt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] hlt — CPU halted" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("توقف", halt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("halt", halt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("hlt", halt_func);

    // (AR) محاكاة تعطيل المقاطعات — cli
    auto cli_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] cli — interrupts disabled" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تعطيل_مقاطعات", cli_func);
    interpreter.getFunctionManager().registerBuiltinFunction("disable_interrupts", cli_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cli", cli_func);

    // (AR) محاكاة تفعيل المقاطعات — sti
    auto sti_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] sti — interrupts enabled" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تفعيل_مقاطعات", sti_func);
    interpreter.getFunctionManager().registerBuiltinFunction("enable_interrupts", sti_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sti", sti_func);

    // (AR) محاكاة كتابة VGA — vga_write(row, col, char, color)
    auto vga_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("شاشة_اكتب: يحتاج 4 معاملات (صف، عمود، حرف، لون)");
        int row = args[0]->toInt();
        int col = args[1]->toInt();
        int ch  = args[2]->toInt();
        int color = args[3]->toInt();
        std::cout << "[OS-SIM] VGA[" << row << "," << col << "] = '" 
                  << (char)ch << "' (color=0x" << std::hex << color << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_اكتب", vga_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("vga_write", vga_write_func);

    // (AR) محاكاة مسح الشاشة — vga_clear(color?)
    auto vga_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int color = 0x0F;
        if (!args.empty()) color = args[0]->toInt();
        std::cout << "[OS-SIM] VGA clear (color=0x" << std::hex << color << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_امسح", vga_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("vga_clear", vga_clear_func);

    // (AR) محاكاة نسخ ذاكرة — memcpy(dest, src, size)
    auto memcpy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("انسخ_ذاكرة: يحتاج 3 معاملات");
        std::cout << "[OS-SIM] memcpy(0x" << std::hex << args[0]->toInt() 
                  << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ذاكرة", memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_copy", memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memcpy", memcpy_func);

    // (AR) محاكاة ملء ذاكرة — memset(dest, value, size)
    auto memset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("املأ_ذاكرة: يحتاج 3 معاملات");
        std::cout << "[OS-SIM] memset(0x" << std::hex << args[0]->toInt() 
                  << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("املأ_ذاكرة", memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_set", memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memset", memset_func);

    // ══════════════════════════════════════════════════════════════════════
    // ⚡ دوال Embedded المتقدمة / Advanced Embedded Functions (18)
    // ══════════════════════════════════════════════════════════════════════

    // ─── Serial I/O (4) ─────────────────────────────────────────────────

    // (AR) تهيئة منفذ تسلسلي — serial_init(port, baud_rate)
    auto serial_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("تسلسلي_هيئ: يحتاج 2 معاملات (منفذ، سرعة)");
        int port = args[0]->toInt();
        int baud = args[1]->toInt();
        std::cout << "[OS-SIM] Serial init: port=0x" << std::hex << port 
                  << " baud=" << std::dec << baud << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_هيئ", serial_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_init", serial_init_func);

    // (AR) إرسال بايت عبر التسلسلي — serial_send(port, byte)
    auto serial_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("تسلسلي_ارسل: يحتاج 2 معاملات (منفذ، بايت)");
        int port = args[0]->toInt();
        int byte = args[1]->toInt();
        std::cout << "[OS-SIM] Serial send: port=0x" << std::hex << port 
                  << " byte=0x" << byte << " ('" << (char)byte << "')" << std::dec << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_ارسل", serial_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_send", serial_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_write", serial_write_func);

    // (AR) استقبال بايت من التسلسلي — serial_receive(port) → byte
    auto serial_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("تسلسلي_استقبل: يحتاج معامل (منفذ)");
        int port = args[0]->toInt();
        std::cout << "[OS-SIM] Serial receive: port=0x" << std::hex << port << std::dec 
                  << " → simulated byte 0x41 ('A')" << std::endl;
        return std::make_shared<Data::Value>(0x41); // simulated received 'A'
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_استقبل", serial_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_receive", serial_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_read", serial_read_func);

    // (AR) فحص جاهزية البيانات — serial_ready(port) → bool
    auto serial_ready_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("تسلسلي_جاهز: يحتاج معامل (منفذ)");
        int port = args[0]->toInt();
        std::cout << "[OS-SIM] Serial ready: port=0x" << std::hex << port << std::dec 
                  << " → true (simulated)" << std::endl;
        return std::make_shared<Data::Value>(1); // simulated: data available
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_جاهز", serial_ready_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_ready", serial_ready_func);

    // ─── GPIO (3) ────────────────────────────────────────────────────────

    // (AR) كتابة GPIO — gpio_write(pin, value)
    auto gpio_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_رقمي_اكتب: يحتاج 2 معاملات (رقم_المنفذ، قيمة)");
        int pin = args[0]->toInt();
        int val = args[1]->toInt();
        std::cout << "[OS-SIM] GPIO write: pin=" << pin << " value=" << val << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_رقمي_اكتب", gpio_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gpio_write", gpio_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("digital_write", gpio_write_func);

    // (AR) قراءة GPIO — gpio_read(pin) → value
    auto gpio_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_رقمي_اقرأ: يحتاج معامل (رقم_المنفذ)");
        int pin = args[0]->toInt();
        std::cout << "[OS-SIM] GPIO read: pin=" << pin << " → 1 (simulated HIGH)" << std::endl;
        return std::make_shared<Data::Value>(1); // simulated HIGH
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_رقمي_اقرأ", gpio_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gpio_read", gpio_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("digital_read", gpio_read_func);

    // (AR) تحديد وضع GPIO — gpio_mode(pin, mode)
    auto gpio_mode_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("حدد_وضع_منفذ: يحتاج 2 معاملات (رقم_المنفذ، الوضع)");
        int pin = args[0]->toInt();
        int mode = args[1]->toInt();
        std::string modeName = (mode == 0) ? "INPUT" : "OUTPUT";
        std::cout << "[OS-SIM] GPIO mode: pin=" << pin << " mode=" << modeName << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدد_وضع_منفذ", gpio_mode_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gpio_mode", gpio_mode_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pin_mode", gpio_mode_func);

    // ─── Timer (3) ───────────────────────────────────────────────────────

    // (AR) تهيئة مؤقت — timer_init(frequency)
    auto timer_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("مؤقت_هيئ: يحتاج معامل (التردد)");
        int freq = args[0]->toInt();
        std::cout << "[OS-SIM] Timer init: frequency=" << freq << " Hz" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_هيئ", timer_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timer_init", timer_init_func);

    // (AR) قراءة المؤقت — timer_read() → value
    auto timer_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto now = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        std::cout << "[OS-SIM] Timer read: " << us << " us" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(us & 0x7FFFFFFF));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_قراءة", timer_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timer_read", timer_read_func);

    // (AR) انتظار ميكروثوان — timer_wait(us)
    auto timer_wait_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("مؤقت_انتظر: يحتاج معامل (ميكروثوان)");
        int us = args[0]->toInt();
        std::cout << "[OS-SIM] Timer wait: " << us << " microseconds" << std::endl;
        // Actually wait (in simulation)
        std::this_thread::sleep_for(std::chrono::microseconds(us));
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_انتظر", timer_wait_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timer_wait", timer_wait_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delay_us", timer_wait_func);

    // ─── System Control (3) ──────────────────────────────────────────────

    // (AR) إعادة تشغيل — reset()
    auto reset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] *** SYSTEM RESET ***" << std::endl;
        std::cout << "[OS-SIM] (in simulation: program continues)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اعد_تشغيل", reset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("reset", reset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("reboot", reset_func);

    // (AR) معرّف المعالج — cpu_id(leaf?) → value
    auto cpuid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int leaf = 0;
        if (!args.empty()) leaf = args[0]->toInt();
        std::cout << "[OS-SIM] CPUID leaf=" << leaf << " → 0x000306C3 (simulated i7)" << std::endl;
        return std::make_shared<Data::Value>(0x000306C3); // simulated Haswell i7
    };
    interpreter.getFunctionManager().registerBuiltinFunction("معرف_المعالج", cpuid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cpu_id", cpuid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cpuid", cpuid_func);

    // (AR) عداد الدورات — rdtsc() → cycle_count
    auto rdtsc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        int simCycles = static_cast<int>(ns & 0x7FFFFFFF);
        std::cout << "[OS-SIM] RDTSC → " << simCycles << " (simulated cycles)" << std::endl;
        return std::make_shared<Data::Value>(simCycles);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عداد_الدورات", rdtsc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("rdtsc", rdtsc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cycle_count", rdtsc_func);

    // ─── Memory Barriers (3) ────────────────────────────────────────────

    // (AR) حاجز ذاكرة كامل — mfence()
    auto mfence_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] MFENCE — full memory barrier" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حاجز_ذاكرة", mfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memory_barrier", mfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mfence", mfence_func);

    // (AR) حاجز قراءة — lfence()
    auto lfence_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] LFENCE — load fence" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حاجز_قراءة", lfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("read_barrier", lfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lfence", lfence_func);

    // (AR) حاجز كتابة — sfence()
    auto sfence_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] SFENCE — store fence" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حاجز_كتابة", sfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("write_barrier", sfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sfence", sfence_func);

    // ─── DMA (2) ─────────────────────────────────────────────────────────

    // (AR) تهيئة DMA — dma_init(channel, src, dest, size)
    auto dma_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("نقل_مباشر_هيئ: يحتاج 4 معاملات (قناة، مصدر، وجهة، حجم)");
        int ch   = args[0]->toInt();
        int src  = args[1]->toInt();
        int dest = args[2]->toInt();
        int size = args[3]->toInt();
        std::cout << "[OS-SIM] DMA init: ch=" << ch << " src=0x" << std::hex << src 
                  << " dest=0x" << dest << " size=" << std::dec << size << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نقل_مباشر_هيئ", dma_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("dma_init", dma_init_func);

    // (AR) بدء نقل DMA — dma_start(channel)
    auto dma_start_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("نقل_مباشر_ابدأ: يحتاج معامل (رقم_القناة)");
        int ch = args[0]->toInt();
        std::cout << "[OS-SIM] DMA start: channel=" << ch << " — transfer in progress" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نقل_مباشر_ابدأ", dma_start_func);
    interpreter.getFunctionManager().registerBuiltinFunction("dma_start", dma_start_func);

    // ══════════════════════════════════════════════════════════════════════
    // 📱 وحدة تطبيقات الهاتف / Mobile Applications Module
    // ══════════════════════════════════════════════════════════════════════

    // (AR) معلومات الجهاز — هاتف_معلومات()
    auto mobile_info_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto info = sad::stdlib::mobile::get_device_info();
        std::cout << "📱 معلومات الجهاز:" << std::endl;
        std::cout << "  المنصة: " << info.platform << std::endl;
        std::cout << "  إصدار النظام: " << info.os_version << std::endl;
        std::cout << "  اسم الجهاز: " << info.device_name << std::endl;
        std::cout << "  الشاشة: " << info.screen_width << "x" << info.screen_height << std::endl;
        std::cout << "  كثافة الشاشة: " << info.screen_density << std::endl;
        std::cout << "  كاميرا: " << (info.has_camera ? "✓ متوفرة" : "✗ غير متوفرة") << std::endl;
        std::cout << "  GPS: " << (info.has_gps ? "✓ متوفر" : "✗ غير متوفر") << std::endl;
        return std::make_shared<Data::Value>(info.platform);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_معلومات", mobile_info_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_info", mobile_info_func);

    // (AR) اسم المنصة — هاتف_منصة()
    auto mobile_platform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::get_platform_name());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_منصة", mobile_platform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_platform", mobile_platform_func);

    // (AR) عرض تنبيه — هاتف_رسالة(عنوان، نص)
    auto mobile_alert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = "تنبيه";
        std::string message = "";
        if (args.size() >= 2) {
            title = args[0]->toString();
            message = args[1]->toString();
        } else if (args.size() >= 1) {
            message = args[0]->toString();
        }
        bool result = sad::stdlib::mobile::show_alert(title, message);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_رسالة", mobile_alert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_alert", mobile_alert_func);

    // (AR) إشعار — هاتف_إشعار(عنوان، نص)
    auto mobile_notify_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = "إشعار";
        std::string message = "";
        if (args.size() >= 2) {
            title = args[0]->toString();
            message = args[1]->toString();
        } else if (args.size() >= 1) {
            message = args[0]->toString();
        }
        bool result = sad::stdlib::mobile::show_notification(title, message);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_إشعار", mobile_notify_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_notify", mobile_notify_func);

    // (AR) رسالة سريعة — هاتف_رسالة_سريعة(نص)
    auto mobile_toast_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("هاتف_رسالة_سريعة: يحتاج معامل واحد على الأقل");
        bool result = sad::stdlib::mobile::show_toast(args[0]->toString());
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_رسالة_سريعة", mobile_toast_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_toast", mobile_toast_func);

    // (AR) إنشاء نص واجهة — واجهة_نص(محتوى)
    auto ui_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_نص: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_نص", ui_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_text", ui_text_func);

    // (AR) إنشاء زر — واجهة_زر(نص)
    auto ui_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_زر: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_button(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_زر", ui_button_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_button", ui_button_func);

    // (AR) إنشاء صورة — واجهة_صورة(مورد)
    auto ui_image_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_صورة: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_image(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_صورة", ui_image_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_image", ui_image_func);

    // (AR) توليد كود Compose من نص — صدّر_كومبوز(نص)
    auto export_compose_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("صدّر_كومبوز: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_to_compose(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("صدّر_كومبوز", export_compose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("صدر_كومبوز", export_compose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("export_compose", export_compose_func);

    // (AR) توليد كود SwiftUI من نص — صدّر_سويفت(نص)
    auto export_swiftui_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("صدّر_سويفت: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_to_swiftui(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("صدّر_سويفت", export_swiftui_func);
    interpreter.getFunctionManager().registerBuiltinFunction("صدر_سويفت", export_swiftui_func);
    interpreter.getFunctionManager().registerBuiltinFunction("export_swiftui", export_swiftui_func);

    // =========================================================================
    // Camera Functions / دوال الكاميرا
    // =========================================================================

    // (AR) التقاط صورة — هاتف_كاميرا()
    auto camera_capture_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string result = sad::stdlib::mobile::capture_photo();
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_كاميرا", camera_capture_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_camera", camera_capture_func);

    // (AR) التحقق من توفر الكاميرا — كاميرا_متوفرة()
    auto camera_available_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::is_camera_available());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كاميرا_متوفرة", camera_available_func);
    interpreter.getFunctionManager().registerBuiltinFunction("camera_available", camera_available_func);

    // =========================================================================
    // Location Functions / دوال الموقع
    // =========================================================================

    // (AR) الموقع الحالي — هاتف_موقع()
    auto location_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto loc = sad::stdlib::mobile::get_current_location();
        std::string result = "خط_عرض:" + std::to_string(loc.latitude) + 
                            ",خط_طول:" + std::to_string(loc.longitude) +
                            ",ارتفاع:" + std::to_string(loc.altitude);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_موقع", location_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_location", location_func);

    // (AR) حساب المسافة بين نقطتين — احسب_المسافة(عرض1، طول1، عرض2، طول2)
    auto distance_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("احسب_المسافة: يحتاج 4 معاملات (عرض1، طول1، عرض2، طول2)");
        double lat1 = args[0]->toDouble();
        double lon1 = args[1]->toDouble();
        double lat2 = args[2]->toDouble();
        double lon2 = args[3]->toDouble();
        double km = sad::stdlib::mobile::calculate_distance(lat1, lon1, lat2, lon2);
        return std::make_shared<Data::Value>(km);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احسب_المسافة", distance_func);
    interpreter.getFunctionManager().registerBuiltinFunction("calculate_distance", distance_func);

    // =========================================================================
    // Sensor Functions / دوال المستشعرات
    // =========================================================================

    // (AR) مستشعر التسارع — هاتف_تسارع()
    auto accelerometer_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto data = sad::stdlib::mobile::get_accelerometer();
        std::string result = "x:" + std::to_string(data.x) + 
                            ",y:" + std::to_string(data.y) +
                            ",z:" + std::to_string(data.z);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_تسارع", accelerometer_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_accelerometer", accelerometer_func);

    // (AR) الجيروسكوب — هاتف_دوران()
    auto gyroscope_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto data = sad::stdlib::mobile::get_gyroscope();
        std::string result = "x:" + std::to_string(data.x) + 
                            ",y:" + std::to_string(data.y) +
                            ",z:" + std::to_string(data.z);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_دوران", gyroscope_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_gyroscope", gyroscope_func);

    // =========================================================================
    // Project Generation / إنشاء المشاريع
    // =========================================================================

    // (AR) إنشاء مشروع هاتف جديد — هاتف_مشروع_جديد(اسم، منصة)
    auto new_project_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_مشروع_جديد: يحتاج معاملين (اسم، منصة)");
        std::string name = args[0]->toString();
        std::string platform = args[1]->toString();
        bool result = sad::stdlib::mobile::create_mobile_project(name, platform);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_مشروع_جديد", new_project_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_new_project", new_project_func);

    // (AR) توليد AndroidManifest — انشئ_manifest(اسم_الحزمة، اسم_التطبيق)  
    auto manifest_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("انشئ_manifest: يحتاج معاملين (اسم_الحزمة، اسم_التطبيق)");
        std::string pkg = args[0]->toString();
        std::string app = args[1]->toString();
        std::string xml = sad::stdlib::mobile::generate_android_manifest(pkg, app);
        return std::make_shared<Data::Value>(xml);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انشئ_manifest", manifest_func);
    interpreter.getFunctionManager().registerBuiltinFunction("generate_manifest", manifest_func);

    // (AR) توليد Info.plist — انشئ_plist(bundle_id، اسم_التطبيق)
    auto plist_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("انشئ_plist: يحتاج معاملين (bundle_id، اسم_التطبيق)");
        std::string bundle = args[0]->toString();
        std::string app = args[1]->toString();
        std::string plist = sad::stdlib::mobile::generate_ios_plist(bundle, app);
        return std::make_shared<Data::Value>(plist);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انشئ_plist", plist_func);
    interpreter.getFunctionManager().registerBuiltinFunction("generate_plist", plist_func);


}

} // namespace Interpreter
} // namespace Sad
