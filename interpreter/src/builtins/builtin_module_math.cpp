/**
 * @file builtin_module_math.cpp
 * @brief (AR) وحدة الرياضيات — الدوال الحسابية والمثلثية والإحصائية والنظام
 * @brief (EN) Math module — trigonometric, statistical, system, and I/O functions
 *
 * @details
 * (AR) الأقسام:
 *   1. الدوال المثلثية (جيب، جتا، ظا...)
 *   2. اللوغاريتمات والأسس (لوغاريتم، أس...)
 *   3. الإحصاء (متوسط، تباين...)
 *   4. دوال النظام (وقت، نم، أمر...)
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtin_common.h"
#include "builtin_registry.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>

namespace Sad
{
    namespace Interpreter
    {

        using namespace StdLib;

        // (AR) اختصار لأسماء الدوال المركزية
        namespace Bm = Builtins::Names::Math;
        namespace Bp = Builtins::Names::Platform;
        namespace Bk = Builtins::Names::Kernel;
        namespace Bkc = Builtins::Names::KernelCpu;

        void registerBuiltinsMath(Interpreter &interpreter)
        {

            // ═══════════════════════════════════════════════════════════════
            // Advanced Math Functions (log, exp, clamp, etc.)

            // ═══════════════════════════════════════════════════════════════
            // Natural logarithm / اللوغاريتم الطبيعي
            auto math_log_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::LOG), math_log_func);

            // Base-10 logarithm / لوغاريتم عشري
            auto math_log10_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log10(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::LOG10), math_log10_func);

            // Base-2 logarithm / لوغاريتم ثنائي
            auto math_log2_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log2(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::LOG2), math_log2_func);

            // (AR) الدالة الأسية e^x — اسمها "أسي" لتجنب تعارض مع "أس" (القوة x^y)
            // (EN) Exponential e^x — named "أسي" to avoid conflict with "أس" (power x^y)
            auto math_exp_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::exp(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::EXP), math_exp_func);

            // Clamp / تقييد القيمة
            auto math_clamp_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::clamp(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::CLAMP), math_clamp_func);

            // Truncate / اقتطاع
            auto math_trunc_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::trunc(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::TRUNCATE), math_trunc_func);

            // Modulo / باقي القسمة
            auto math_mod_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::mod(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::FMOD), math_mod_func);

            // Sign / إشارة
            auto math_sign_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::sign(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::SIGN), math_sign_func);

            // Pi constant / ثابت باي
            auto math_pi_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::pi());
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::PI), math_pi_func);

            // (AR) ثابت أويلر e — الاسم الموحد "هـ" (يطابق stdlib_manager)
            // (EN) Euler's e constant — unified name "هـ" (matches stdlib_manager)
            auto math_e_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::e());
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::E), math_e_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال رياضية إضافية — نُقلت من وحدة خرائط (كانت في القسم 5)
            // (EN) Additional math functions — moved from maps module (was Section 5)
            // ═══════════════════════════════════════════════════════════════

            // (AR) الاستيفاء الخطي — lerp(a, b, t) = a + t*(b-a)
            // (EN) Linear interpolation — lerp(a, b, t) = a + t*(b-a)
            auto math_lerp_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 3)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                double a = args[0]->toDouble();
                double b = args[1]->toDouble();
                double t = args[2]->toDouble();
                return std::make_shared<Data::Value>(a + t * (b - a));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::LERP), math_lerp_func);

            // (AR) عشوائي_نطاق — رقم صحيح عشوائي في نطاق [min, max]
            // (EN) Random integer in range [min, max]
            auto math_random_range_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int lo = args[0]->toInt();
                int hi = args[1]->toInt();
                static std::mt19937 gen(std::random_device{}());
                std::uniform_int_distribution<int> dist(lo, hi);
                return std::make_shared<Data::Value>(dist(gen));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::RANDOM_RANGE), math_random_range_func);

            // (AR) عشوائي_عشري — رقم عشري عشوائي بين 0.0 و 1.0
            // (EN) Random float between 0.0 and 1.0
            auto math_random_float_func = [](Sad::Interpreter::BuiltinContext &) -> std::shared_ptr<Data::Value>
            {
                static std::mt19937 gen(std::random_device{}());
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                return std::make_shared<Data::Value>(dist(gen));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::RANDOM_FLOAT), math_random_float_func);

            // ═══════════════════════════════════════════════════════════════
            // System Functions (environment, directory, execution)

            // ═══════════════════════════════════════════════════════════════
            // Get environment variable / الحصول على متغير بيئة
            auto sys_getenv_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::getEnv(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::GET_ENV), sys_getenv_func);

            // Set environment variable / تعيين متغير بيئة
            auto sys_setenv_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::setEnv(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::SET_ENV), sys_setenv_func);

            // Get current directory / الحصول على المجلد الحالي
            auto sys_curdir_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::currentDir(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::CUR_DIR), sys_curdir_func);

            // Change directory / تغيير المجلد
            auto sys_chdir_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::changeDir(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::CHANGE_DIR), sys_chdir_func);

            // Execute command / تنفيذ أمر
            auto sys_execute_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::execute(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::EXECUTE), sys_execute_func);

            // Get platform / المنصة
            auto sys_platform_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::platform());
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::PLATFORM_NAME), sys_platform_func);

            // Get OS name / اسم نظام التشغيل
            auto sys_osname_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::osName());
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::OS_NAME), sys_osname_func);

            // Get timestamp / الطابع الزمني
            auto sys_timestamp_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::timestamp());
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::TIMESTAMP), sys_timestamp_func);

            // Get clock time / وقت الساعة
            auto sys_clock_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::clock_time());
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bp::CLOCK), sys_clock_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال برمجة أنظمة التشغيل — محاكاة في المفسر
            // (EN) OS Development Functions — Simulation in interpreter

            // ═══════════════════════════════════════════════════════════════
            // (AR) هذه الدوال تحاكي عمليات النظام المنخفض المستوى في المفسر.
            //       المترجم (sadc) يولّد هذه العمليات كود أصلي (inline asm).
            //       المفسر (sad) يحاكيها بطباعة رسائل تشخيصية.

            // ═══════════════════════════════════════════════════════════════
            // (AR) محاكاة كتابة منفذ — outb(port, value)
            auto port_write_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int port = args[0]->toInt();
                int val = args[1]->toInt();
                std::cout << "[OS-SIM] outb(0x" << std::hex << port << ", 0x" << val << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bkc::CPU_9), port_write_func);

            // (AR) محاكاة قراءة منفذ — inb(port)
            auto port_read_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int port = args[0]->toInt();
                std::cout << "[OS-SIM] inb(0x" << std::hex << port << std::dec << ") -> 0x00" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bkc::CPU_8), port_read_func);

            // (AR) محاكاة كتابة ذاكرة — poke(addr, value)
            auto mem_write_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int addr = args[0]->toInt();
                int val = args[1]->toInt();
                std::cout << "[OS-SIM] poke(0x" << std::hex << addr << ", 0x" << val << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bkc::CPU_25), mem_write_func);

            // (AR) محاكاة قراءة ذاكرة — peek(addr)
            auto mem_read_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int addr = args[0]->toInt();
                std::cout << "[OS-SIM] peek(0x" << std::hex << addr << std::dec << ") -> 0x00" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bkc::CPU_24), mem_read_func);

            // (AR) محاكاة مقاطعة — interrupt(n)
            auto interrupt_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int n = args[0]->toInt();
                std::cout << "[OS-SIM] int 0x" << std::hex << n << std::dec << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::INTERRUPT), interrupt_func);

            // (AR) محاكاة إيقاف المعالج — hlt
            auto halt_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] hlt — CPU halted" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::HALT), halt_func);

            // (AR) محاكاة تعطيل المقاطعات — cli
            auto cli_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] cli — interrupts disabled" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bkc::CPU_3), cli_func);

            // (AR) محاكاة تفعيل المقاطعات — sti
            auto sti_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] sti — interrupts enabled" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bkc::CPU_4), sti_func);

            // (AR) محاكاة كتابة VGA — vga_write(row, col, char, color)
            auto vga_write_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 4)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int row = args[0]->toInt();
                int col = args[1]->toInt();
                int ch = args[2]->toInt();
                int color = args[3]->toInt();
                std::cout << "[OS-SIM] VGA[" << row << "," << col << "] = '"
                          << (char)ch << "' (color=0x" << std::hex << color << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::VGA_WRITE), vga_write_func);

            // (AR) محاكاة مسح الشاشة — vga_clear(color?)
            auto vga_clear_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                int color = 0x0F;
                if (!args.empty())
                    color = args[0]->toInt();
                std::cout << "[OS-SIM] VGA clear (color=0x" << std::hex << color << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::VGA_CLEAR), vga_clear_func);

            // (AR) محاكاة نسخ ذاكرة — memcpy(dest, src, size)
            auto memcpy_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 3)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                std::cout << "[OS-SIM] memcpy(0x" << std::hex << args[0]->toInt()
                          << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::MEMCPY), memcpy_func);

            // (AR) محاكاة ملء ذاكرة — memset(dest, value, size)
            auto memset_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 3)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                std::cout << "[OS-SIM] memset(0x" << std::hex << args[0]->toInt()
                          << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::MEMSET), memset_func);

            // ══════════════════════════════════════════════════════════════════════
            // ■ دوال Embedded المتقدمة / Advanced Embedded Functions (18)
            // ══════════════════════════════════════════════════════════════════════

            // ─── Serial I/O (4) ─────────────────────────────────────────

            // (AR) تهيئة منفذ تسلسل — serial_init(port, baud_rate)
            auto serial_init_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int port = args[0]->toInt();
                int baud = args[1]->toInt();
                std::cout << "[OS-SIM] Serial init: port=0x" << std::hex << port
                          << " baud=" << std::dec << baud << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::SERIAL_INIT), serial_init_func);

            // (AR) إرسال بايت عبر التسلسل — serial_send(port, byte)
            auto serial_write_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int port = args[0]->toInt();
                int byte = args[1]->toInt();
                std::cout << "[OS-SIM] Serial send: port=0x" << std::hex << port
                          << " byte=0x" << byte << " ('" << (char)byte << "')" << std::dec << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::SERIAL_SEND), serial_write_func);

            // (AR) استقبال بايت من التسلسل — serial_receive(port)
            auto serial_read_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int port = args[0]->toInt();
                std::cout << "[OS-SIM] Serial receive: port=0x" << std::hex << port << std::dec
                          << " → simulated byte 0x41 ('A')" << std::endl;
                return std::make_shared<Data::Value>(0x41); // simulated received 'A'
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::SERIAL_RECV), serial_read_func);

            // (AR) فحص جاهزية البيانات — serial_ready(port)
            auto serial_ready_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int port = args[0]->toInt();
                std::cout << "[OS-SIM] Serial ready: port=0x" << std::hex << port << std::dec
                          << " → true (simulated)" << std::endl;
                return std::make_shared<Data::Value>(1); // simulated: data available
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::SERIAL_READY), serial_ready_func);

            // ─── GPIO (3) ──────────────────────────────────────────────

            // (AR) كتابة GPIO — gpio_write(pin, value)
            auto gpio_write_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int pin = args[0]->toInt();
                int val = args[1]->toInt();
                std::cout << "[OS-SIM] GPIO write: pin=" << pin << " value=" << val << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::GPIO_WRITE), gpio_write_func);

            // (AR) قراءة GPIO — gpio_read(pin)
            auto gpio_read_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int pin = args[0]->toInt();
                std::cout << "[OS-SIM] GPIO read: pin=" << pin << " → 1 (simulated HIGH)" << std::endl;
                return std::make_shared<Data::Value>(1); // simulated HIGH
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::GPIO_READ), gpio_read_func);

            // (AR) تحديد وضع GPIO — gpio_mode(pin, mode)
            auto gpio_mode_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 2)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int pin = args[0]->toInt();
                int mode = args[1]->toInt();
                std::string modeName = (mode == 0) ? "INPUT" : "OUTPUT";
                std::cout << "[OS-SIM] GPIO mode: pin=" << pin << " mode=" << modeName << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::GPIO_MODE), gpio_mode_func);

            // ─── Timer (3) ─────────────────────────────────────────────

            // (AR) تهيئة مؤقت — timer_init(frequency)
            auto timer_init_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int freq = args[0]->toInt();
                std::cout << "[OS-SIM] Timer init: frequency=" << freq << " Hz" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::EMBED_TIMER_INIT), timer_init_func);

            // (AR) قراءة المؤقت — timer_read()
            auto timer_read_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto now = std::chrono::high_resolution_clock::now();
                auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
                std::cout << "[OS-SIM] Timer read: " << us << " us" << std::endl;
                return std::make_shared<Data::Value>(static_cast<int>(us & 0x7FFFFFFF));
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::EMBED_TIMER_READ), timer_read_func);

            // (AR) انتظار ميكروثانية — timer_wait(us)
            auto timer_wait_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int us = args[0]->toInt();
                std::cout << "[OS-SIM] Timer wait: " << us << " microseconds" << std::endl;
                // Actually wait (in simulation)
                std::this_thread::sleep_for(std::chrono::microseconds(us));
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::EMBED_TIMER_WAIT), timer_wait_func);

            // ─── System Control (3) ────────────────────────────────────

            // (AR) إعادة تشغيل — reset()
            auto reset_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] *** SYSTEM RESET ***" << std::endl;
                std::cout << "[OS-SIM] (in simulation: program continues)" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::RESET), reset_func);

            // (AR) معرّف المعالج — cpu_id(leaf?)
            auto cpuid_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                int leaf = 0;
                if (!args.empty())
                    leaf = args[0]->toInt();
                std::cout << "[OS-SIM] CPUID leaf=" << leaf << " → 0x000306C3 (simulated i7)" << std::endl;
                return std::make_shared<Data::Value>(0x000306C3); // simulated Haswell i7
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::CPUID), cpuid_func);

            // (AR) عداد الدورات — rdtsc()
            auto rdtsc_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto now = std::chrono::high_resolution_clock::now();
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
                int simCycles = static_cast<int>(ns & 0x7FFFFFFF);
                std::cout << "[OS-SIM] RDTSC → " << simCycles << " (simulated cycles)" << std::endl;
                return std::make_shared<Data::Value>(simCycles);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::RDTSC), rdtsc_func);

            // ─── Memory Barriers (3) ───────────────────────────────────

            // (AR) حاجز ذاكرة كامل — mfence()
            auto mfence_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] MFENCE — full memory barrier" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::MFENCE), mfence_func);

            // (AR) حاجز قراءة — lfence()
            auto lfence_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] LFENCE — load fence" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::LFENCE), lfence_func);

            // (AR) حاجز كتابة — sfence()
            auto sfence_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::cout << "[OS-SIM] SFENCE — store fence" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::SFENCE), sfence_func);

            // ─── DMA (2) ───────────────────────────────────────────────

            // (AR) تهيئة DMA — dma_init(channel, src, dest, size)
            auto dma_init_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() < 4)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int ch = args[0]->toInt();
                int src = args[1]->toInt();
                int dest = args[2]->toInt();
                int size = args[3]->toInt();
                std::cout << "[OS-SIM] DMA init: ch=" << ch << " src=0x" << std::hex << src
                          << " dest=0x" << dest << " size=" << std::dec << size << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::DMA_INIT), dma_init_func);

            // (AR) بدء نقل DMA — dma_start(channel)
            auto dma_start_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                if (ctx.argCount() == 0)
                    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &args = ctx.args();
                int ch = args[0]->toInt();
                std::cout << "[OS-SIM] DMA start: channel=" << ch << " — transfer in progress" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bk::DMA_START), dma_start_func);
        }

    } // namespace Interpreter
} // namespace Sad
