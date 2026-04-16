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
#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <fstream>
#include <iostream>

namespace Sad
{
    namespace Interpreter
    {

        using namespace StdLib;

        void registerBuiltinsMath(Interpreter &interpreter)
        {

            // ═══════════════════════════════════════════════════════════════
            // Advanced Math Functions (log, exp, clamp, etc.)

            // ═══════════════════════════════════════════════════════════════
            // Natural logarithm / اللوغاريتم الطبيعي
            auto math_log_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("لوغ", math_log_func);
            interpreter.getFunctionManager().registerBuiltinFunction("log", math_log_func);
            interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم", math_log_func);

            // Base-10 logarithm / لوغاريتم عشري
            auto math_log10_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log10(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("لوغ10", math_log10_func);
            interpreter.getFunctionManager().registerBuiltinFunction("log10", math_log10_func);
            interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم_عشري", math_log10_func);

            // Base-2 logarithm / لوغاريتم ثنائي
            auto math_log2_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log2(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("لوغ2", math_log2_func);
            interpreter.getFunctionManager().registerBuiltinFunction("log2", math_log2_func);
            interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم_ثنائي", math_log2_func);

            // Exponential e^x / أُس
            auto math_exp_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::exp(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("أس", math_exp_func);
            interpreter.getFunctionManager().registerBuiltinFunction("exp", math_exp_func);
            interpreter.getFunctionManager().registerBuiltinFunction("أُس", math_exp_func);

            // Clamp / تقييد القيمة
            auto math_clamp_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::clamp(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("قيد", math_clamp_func);
            interpreter.getFunctionManager().registerBuiltinFunction("clamp", math_clamp_func);
            interpreter.getFunctionManager().registerBuiltinFunction("تقييد", math_clamp_func);

            // Truncate / اقتطاع
            auto math_trunc_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::trunc(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("اقتطاع", math_trunc_func);
            interpreter.getFunctionManager().registerBuiltinFunction("trunc", math_trunc_func);
            interpreter.getFunctionManager().registerBuiltinFunction("اقتطع", math_trunc_func);

            // Modulo / باقي القسمة
            auto math_mod_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::mod(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("باقي", math_mod_func);
            interpreter.getFunctionManager().registerBuiltinFunction("mod", math_mod_func);
            interpreter.getFunctionManager().registerBuiltinFunction("باقي_القسمة", math_mod_func);

            // Sign / إشارة
            auto math_sign_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::sign(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("إشارة", math_sign_func);
            interpreter.getFunctionManager().registerBuiltinFunction("sign", math_sign_func);

            // Pi constant / ثابت باي
            auto math_pi_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::pi());
            };

            interpreter.getFunctionManager().registerBuiltinFunction("باي", math_pi_func);
            interpreter.getFunctionManager().registerBuiltinFunction("pi", math_pi_func);
            interpreter.getFunctionManager().registerBuiltinFunction("ط", math_pi_func);

            // e constant / ثابت إ (أولر)
            auto math_e_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::e());
            };

            interpreter.getFunctionManager().registerBuiltinFunction("إ", math_e_func);
            interpreter.getFunctionManager().registerBuiltinFunction("e_const", math_e_func);
            interpreter.getFunctionManager().registerBuiltinFunction("أولر", math_e_func);

            // ═══════════════════════════════════════════════════════════════
            // System Functions (environment, directory, execution)

            // ═══════════════════════════════════════════════════════════════
            // Get environment variable / الحصول على متغير بيئة
            auto sys_getenv_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::getEnv(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("احصل_بيئة", sys_getenv_func);

            // Set environment variable / تعيين متغير بيئة
            auto sys_setenv_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::setEnv(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("عيّن_بيئة", sys_setenv_func);
            interpreter.getFunctionManager().registerBuiltinFunction("setEnv", sys_setenv_func);
            interpreter.getFunctionManager().registerBuiltinFunction("set_env", sys_setenv_func);

            // Get current directory / الحصول على المجلد الحالي
            auto sys_curdir_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::currentDir(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("مجلد_حالي", sys_curdir_func);
            interpreter.getFunctionManager().registerBuiltinFunction("currentDir", sys_curdir_func);
            interpreter.getFunctionManager().registerBuiltinFunction("المجلد_الحالي", sys_curdir_func);
            interpreter.getFunctionManager().registerBuiltinFunction("pwd", sys_curdir_func);

            // Change directory / تغيير المجلد
            auto sys_chdir_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::changeDir(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("غيّر_مجلد", sys_chdir_func);
            interpreter.getFunctionManager().registerBuiltinFunction("changeDir", sys_chdir_func);
            interpreter.getFunctionManager().registerBuiltinFunction("cd", sys_chdir_func);

            // Execute command / تنفيذ أمر
            auto sys_execute_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::execute(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("نفّذ", sys_execute_func);
            interpreter.getFunctionManager().registerBuiltinFunction("تنفذ", sys_execute_func);

            // Get platform / المنصة
            auto sys_platform_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::platform());
            };

            interpreter.getFunctionManager().registerBuiltinFunction("منصة", sys_platform_func);
            interpreter.getFunctionManager().registerBuiltinFunction("المنصة", sys_platform_func);

            // Get OS name / اسم نظام التشغيل
            auto sys_osname_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::osName());
            };

            interpreter.getFunctionManager().registerBuiltinFunction("نظام", sys_osname_func);
            interpreter.getFunctionManager().registerBuiltinFunction("osName", sys_osname_func);
            interpreter.getFunctionManager().registerBuiltinFunction("os", sys_osname_func);

            // Get timestamp / الطابع الزمني
            auto sys_timestamp_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::timestamp());
            };

            interpreter.getFunctionManager().registerBuiltinFunction("طابع_زمن", sys_timestamp_func);
            interpreter.getFunctionManager().registerBuiltinFunction("الطابع_الزمني", sys_timestamp_func);

            // Get clock time / وقت الساعة
            auto sys_clock_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::clock_time());
            };

            interpreter.getFunctionManager().registerBuiltinFunction("ساعة", sys_clock_func);
            interpreter.getFunctionManager().registerBuiltinFunction("clock", sys_clock_func);
            interpreter.getFunctionManager().registerBuiltinFunction("وقت_الساعة", sys_clock_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال برمجة أنظمة التشغيل — محاكاة في المفسر
            // (EN) OS Development Functions — Simulation in interpreter

            // ═══════════════════════════════════════════════════════════════
            // (AR) هذه الدوال تحاكي عمليات النظام المنخفض المستوى في المفسر.
            //       المترجم (sadc) يولّد هذه العمليات كود أصلي (inline asm).
            //       المفسر (sad) يحاكيها بطباعة رسائل تشخيصية.

            // ═══════════════════════════════════════════════════════════════
            // (AR) محاكاة كتابة منفذ — outb(port, value)
            auto port_write_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("منفذ_اكتب: يحتاج معاملين (منفذ، قيمة)");
                int port = args[0]->toInt();
                int val = args[1]->toInt();
                std::cout << "[OS-SIM] outb(0x" << std::hex << port << ", 0x" << val << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("منفذ_اكتب", port_write_func);

            // (AR) محاكاة قراءة منفذ — inb(port)
            auto port_read_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("منفذ_اقرأ: يحتاج معامل واحد (رقم المنفذ)");
                int port = args[0]->toInt();
                std::cout << "[OS-SIM] inb(0x" << std::hex << port << std::dec << ") -> 0x00" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("منفذ_اقرأ", port_read_func);

            // (AR) محاكاة كتابة ذاكرة — poke(addr, value)
            auto mem_write_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("ذاكرة_اكتب: يحتاج معاملين (عنوان، قيمة)");
                int addr = args[0]->toInt();
                int val = args[1]->toInt();
                std::cout << "[OS-SIM] poke(0x" << std::hex << addr << ", 0x" << val << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("ذاكرة_اكتب", mem_write_func);

            // (AR) محاكاة قراءة ذاكرة — peek(addr)
            auto mem_read_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("ذاكرة_اقرأ: يحتاج معامل واحد (عنوان)");
                int addr = args[0]->toInt();
                std::cout << "[OS-SIM] peek(0x" << std::hex << addr << std::dec << ") -> 0x00" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("ذاكرة_اقرأ", mem_read_func);

            // (AR) محاكاة مقاطعة — interrupt(n)
            auto interrupt_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("مقاطعة: يحتاج معامل واحد (رقم المقاطعة)");
                int n = args[0]->toInt();
                std::cout << "[OS-SIM] int 0x" << std::hex << n << std::dec << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("مقاطعة", interrupt_func);
            interpreter.getFunctionManager().registerBuiltinFunction("interrupt", interrupt_func);
            interpreter.getFunctionManager().registerBuiltinFunction("int_call", interrupt_func);

            // (AR) محاكاة إيقاف المعالج — hlt
            auto halt_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] hlt — CPU halted" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("توقف", halt_func);

            // (AR) محاكاة تعطيل المقاطعات — cli
            auto cli_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] cli — interrupts disabled" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("تعطيل_مقاطعات", cli_func);

            // (AR) محاكاة تفعيل المقاطعات — sti
            auto sti_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] sti — interrupts enabled" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("تفعيل_مقاطعات", sti_func);

            // (AR) محاكاة كتابة VGA — vga_write(row, col, char, color)
            auto vga_write_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 4)
                    throw std::runtime_error("شاشة_اكتب: يحتاج 4 معاملات (صف، عمود، حرف، لون)");
                int row = args[0]->toInt();
                int col = args[1]->toInt();
                int ch = args[2]->toInt();
                int color = args[3]->toInt();
                std::cout << "[OS-SIM] VGA[" << row << "," << col << "] = '"
                          << (char)ch << "' (color=0x" << std::hex << color << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("شاشة_اكتب", vga_write_func);
            interpreter.getFunctionManager().registerBuiltinFunction("vga_write", vga_write_func);

            // (AR) محاكاة مسح الشاشة — vga_clear(color?)
            auto vga_clear_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                int color = 0x0F;
                if (!args.empty())
                    color = args[0]->toInt();
                std::cout << "[OS-SIM] VGA clear (color=0x" << std::hex << color << std::dec << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("شاشة_امسح", vga_clear_func);
            interpreter.getFunctionManager().registerBuiltinFunction("vga_clear", vga_clear_func);

            // (AR) محاكاة نسخ ذاكرة — memcpy(dest, src, size)
            auto memcpy_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 3)
                    throw std::runtime_error("انسخ_ذاكرة: يحتاج 3 معاملات");
                std::cout << "[OS-SIM] memcpy(0x" << std::hex << args[0]->toInt()
                          << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ذاكرة", memcpy_func);

            // (AR) محاكاة ملء ذاكرة — memset(dest, value, size)
            auto memset_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 3)
                    throw std::runtime_error("املأ_ذاكرة: يحتاج 3 معاملات");
                std::cout << "[OS-SIM] memset(0x" << std::hex << args[0]->toInt()
                          << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("املأ_ذاكرة", memset_func);

            // ══════════════════════════════════════════════════════════════════════
            // ■ دوال Embedded المتقدمة / Advanced Embedded Functions (18)
            // ══════════════════════════════════════════════════════════════════════

            // ─── Serial I/O (4) ─────────────────────────────────────────

            // (AR) تهيئة منفذ تسلسل — serial_init(port, baud_rate)
            auto serial_init_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("تسلسل_هئ: يحتاج 2 معاملات (منفذ، سرعة)");
                int port = args[0]->toInt();
                int baud = args[1]->toInt();
                std::cout << "[OS-SIM] Serial init: port=0x" << std::hex << port
                          << " baud=" << std::dec << baud << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("تسلسل_هئ", serial_init_func);

            // (AR) إرسال بايت عبر التسلسل — serial_send(port, byte)
            auto serial_write_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("تسلسل_ارسل: يحتاج 2 معاملات (منفذ، بايت)");
                int port = args[0]->toInt();
                int byte = args[1]->toInt();
                std::cout << "[OS-SIM] Serial send: port=0x" << std::hex << port
                          << " byte=0x" << byte << " ('" << (char)byte << "')" << std::dec << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("تسلسل_ارسل", serial_write_func);

            // (AR) استقبال بايت من التسلسل — serial_receive(port)
            auto serial_read_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("تسلسل_استقبل: يحتاج معامل (منفذ)");
                int port = args[0]->toInt();
                std::cout << "[OS-SIM] Serial receive: port=0x" << std::hex << port << std::dec
                          << " → simulated byte 0x41 ('A')" << std::endl;
                return std::make_shared<Data::Value>(0x41); // simulated received 'A'
            };
            interpreter.getFunctionManager().registerBuiltinFunction("تسلسل_استقبل", serial_read_func);

            // (AR) فحص جاهزية البيانات — serial_ready(port)
            auto serial_ready_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("تسلسل_جاهز: يحتاج معامل (منفذ)");
                int port = args[0]->toInt();
                std::cout << "[OS-SIM] Serial ready: port=0x" << std::hex << port << std::dec
                          << " → true (simulated)" << std::endl;
                return std::make_shared<Data::Value>(1); // simulated: data available
            };
            interpreter.getFunctionManager().registerBuiltinFunction("تسلسل_جاهز", serial_ready_func);

            // ─── GPIO (3) ──────────────────────────────────────────────

            // (AR) كتابة GPIO — gpio_write(pin, value)
            auto gpio_write_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("منفذ_رقمي_اكتب: يحتاج 2 معاملات (رقم_المنفذ، قيمة)");
                int pin = args[0]->toInt();
                int val = args[1]->toInt();
                std::cout << "[OS-SIM] GPIO write: pin=" << pin << " value=" << val << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("منفذ_رقمي_اكتب", gpio_write_func);
            interpreter.getFunctionManager().registerBuiltinFunction("gpio_write", gpio_write_func);
            interpreter.getFunctionManager().registerBuiltinFunction("digital_write", gpio_write_func);

            // (AR) قراءة GPIO — gpio_read(pin)
            auto gpio_read_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("منفذ_رقمي_اقرأ: يحتاج معامل (رقم_المنفذ)");
                int pin = args[0]->toInt();
                std::cout << "[OS-SIM] GPIO read: pin=" << pin << " → 1 (simulated HIGH)" << std::endl;
                return std::make_shared<Data::Value>(1); // simulated HIGH
            };
            interpreter.getFunctionManager().registerBuiltinFunction("منفذ_رقمي_اقرأ", gpio_read_func);
            interpreter.getFunctionManager().registerBuiltinFunction("gpio_read", gpio_read_func);
            interpreter.getFunctionManager().registerBuiltinFunction("digital_read", gpio_read_func);

            // (AR) تحديد وضع GPIO — gpio_mode(pin, mode)
            auto gpio_mode_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("حدد_وضع_منفذ: يحتاج 2 معاملات (رقم_المنفذ، الوضع)");
                int pin = args[0]->toInt();
                int mode = args[1]->toInt();
                std::string modeName = (mode == 0) ? "INPUT" : "OUTPUT";
                std::cout << "[OS-SIM] GPIO mode: pin=" << pin << " mode=" << modeName << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("حدد_وضع_منفذ", gpio_mode_func);
            interpreter.getFunctionManager().registerBuiltinFunction("gpio_mode", gpio_mode_func);
            interpreter.getFunctionManager().registerBuiltinFunction("pin_mode", gpio_mode_func);

            // ─── Timer (3) ─────────────────────────────────────────────

            // (AR) تهيئة مؤقت — timer_init(frequency)
            auto timer_init_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("مؤقت_هئ: يحتاج معامل (التردد)");
                int freq = args[0]->toInt();
                std::cout << "[OS-SIM] Timer init: frequency=" << freq << " Hz" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_هئ", timer_init_func);
            interpreter.getFunctionManager().registerBuiltinFunction("timer_init", timer_init_func);

            // (AR) قراءة المؤقت — timer_read()
            auto timer_read_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto now = std::chrono::high_resolution_clock::now();
                auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
                std::cout << "[OS-SIM] Timer read: " << us << " us" << std::endl;
                return std::make_shared<Data::Value>(static_cast<int>(us & 0x7FFFFFFF));
            };
            interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_قراءة", timer_read_func);
            interpreter.getFunctionManager().registerBuiltinFunction("timer_read", timer_read_func);

            // (AR) انتظار ميكروثانية — timer_wait(us)
            auto timer_wait_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("مؤقت_انتظر: يحتاج معامل (ميكروثانية)");
                int us = args[0]->toInt();
                std::cout << "[OS-SIM] Timer wait: " << us << " microseconds" << std::endl;
                // Actually wait (in simulation)
                std::this_thread::sleep_for(std::chrono::microseconds(us));
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_انتظر", timer_wait_func);
            interpreter.getFunctionManager().registerBuiltinFunction("timer_wait", timer_wait_func);
            interpreter.getFunctionManager().registerBuiltinFunction("delay_us", timer_wait_func);

            // ─── System Control (3) ────────────────────────────────────

            // (AR) إعادة تشغيل — reset()
            auto reset_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] *** SYSTEM RESET ***" << std::endl;
                std::cout << "[OS-SIM] (in simulation: program continues)" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("اعد_تشغيل", reset_func);
            interpreter.getFunctionManager().registerBuiltinFunction("reset", reset_func);
            interpreter.getFunctionManager().registerBuiltinFunction("reboot", reset_func);

            // (AR) معرّف المعالج — cpu_id(leaf?)
            auto cpuid_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                int leaf = 0;
                if (!args.empty())
                    leaf = args[0]->toInt();
                std::cout << "[OS-SIM] CPUID leaf=" << leaf << " → 0x000306C3 (simulated i7)" << std::endl;
                return std::make_shared<Data::Value>(0x000306C3); // simulated Haswell i7
            };
            interpreter.getFunctionManager().registerBuiltinFunction("معرف_المعالج", cpuid_func);
            interpreter.getFunctionManager().registerBuiltinFunction("cpu_id", cpuid_func);
            interpreter.getFunctionManager().registerBuiltinFunction("cpuid", cpuid_func);

            // (AR) عداد الدورات — rdtsc()
            auto rdtsc_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto now = std::chrono::high_resolution_clock::now();
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
                int simCycles = static_cast<int>(ns & 0x7FFFFFFF);
                std::cout << "[OS-SIM] RDTSC → " << simCycles << " (simulated cycles)" << std::endl;
                return std::make_shared<Data::Value>(simCycles);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("عداد_الدورات", rdtsc_func);

            // ─── Memory Barriers (3) ───────────────────────────────────

            // (AR) حاجز ذاكرة كامل — mfence()
            auto mfence_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] MFENCE — full memory barrier" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("حاجز_ذاكرة", mfence_func);
            interpreter.getFunctionManager().registerBuiltinFunction("memory_barrier", mfence_func);
            interpreter.getFunctionManager().registerBuiltinFunction("mfence", mfence_func);

            // (AR) حاجز قراءة — lfence()
            auto lfence_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] LFENCE — load fence" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("حاجز_قراءة", lfence_func);
            interpreter.getFunctionManager().registerBuiltinFunction("read_barrier", lfence_func);
            interpreter.getFunctionManager().registerBuiltinFunction("lfence", lfence_func);

            // (AR) حاجز كتابة — sfence()
            auto sfence_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::cout << "[OS-SIM] SFENCE — store fence" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("حاجز_كتابة", sfence_func);
            interpreter.getFunctionManager().registerBuiltinFunction("write_barrier", sfence_func);
            interpreter.getFunctionManager().registerBuiltinFunction("sfence", sfence_func);

            // ─── DMA (2) ───────────────────────────────────────────────

            // (AR) تهيئة DMA — dma_init(channel, src, dest, size)
            auto dma_init_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 4)
                    throw std::runtime_error("نقل_مباشر_هئ: يحتاج 4 معاملات (قناة، مصدر، وجهة، حجم)");
                int ch = args[0]->toInt();
                int src = args[1]->toInt();
                int dest = args[2]->toInt();
                int size = args[3]->toInt();
                std::cout << "[OS-SIM] DMA init: ch=" << ch << " src=0x" << std::hex << src
                          << " dest=0x" << dest << " size=" << std::dec << size << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("نقل_مباشر_هئ", dma_init_func);
            interpreter.getFunctionManager().registerBuiltinFunction("dma_init", dma_init_func);

            // (AR) بدء نقل DMA — dma_start(channel)
            auto dma_start_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("نقل_مباشر_ابدأ: يحتاج معامل (رقم_القناة)");
                int ch = args[0]->toInt();
                std::cout << "[OS-SIM] DMA start: channel=" << ch << " — transfer in progress" << std::endl;
                return std::make_shared<Data::Value>(0);
            };
            interpreter.getFunctionManager().registerBuiltinFunction("نقل_مباشر_ابدأ", dma_start_func);
            interpreter.getFunctionManager().registerBuiltinFunction("dma_start", dma_start_func);
        }

    } // namespace Interpreter
} // namespace Sad
