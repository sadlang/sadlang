/**
 * @file builtin_module_maps_utils.cpp
 * @brief (AR) وحدة الأدوات — التاريخ/الوقت والرياضيات والملفات والتسلسل والبيئة ومعالجة الأخطاء
 * @brief (EN) Utilities module — date/time, math, files, serialization, environment, error handling
 *
 * @details
 * (AR) الأقسام:
 *   3. التاريخ والوقت (الآن، صيغة_تاريخ، مكونات...)
 *   5. ثوابت ودوال رياضية متقدمة (π، لوغاريتم، جذر...)
 *   7. دوال الملفات الإضافية (قراءة، كتابة، وجود...)
 *   8. التسلسل (base64, hex, url)
 *   9. بيئة النظام (متغيرات البيئة)
 *   10. معالجة الأخطاء المحسنة (محاولة_تنفيذ)
 *
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#ifdef VOID
#undef VOID
#endif

namespace Sad
{
    namespace Interpreter
    {

        // (AR) اختصارات لأسماء الدوال المركزية
        namespace Bmp = Builtins::Names::Maps;

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) دوال مساعدة / (EN) Helper Functions
        // ═══════════════════════════════════════════════════════════════════════

        static std::shared_ptr<Data::Value> makeVal(int v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(double v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(const std::string &v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(bool v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVoidVal() { return std::make_shared<Data::Value>(); }
        static std::shared_ptr<Data::Value> makeArrayVal(const Data::Value::ArrayType &a) { return std::make_shared<Data::Value>(a); }
        static std::shared_ptr<Data::Value> makeMapVal(const Data::Value::MapType &m) { return std::make_shared<Data::Value>(m); }

        void registerBuiltinsMapsUtils(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════════
            // 3. التاريخ والوقت / Date & Time
            // (AR) إصلاح نقطة ضعف رقم 3: عدم وجود دوال للتاريخ والوقت
            // ═══════════════════════════════════════════════════════════════════

            // now / الآن — الحصول على الطابع الزمني الحالي (بالثواني)
            auto now_fn = [](Sad::Interpreter::BuiltinContext &) -> std::shared_ptr<Data::Value>
            {
                auto t = std::chrono::system_clock::now();
                auto epoch = t.time_since_epoch();
                double secs = std::chrono::duration<double>(epoch).count();
                return makeVal(secs);
            };
            fm.registerBuiltinFunction(std::string(Bmp::NOW), now_fn);
            // now_ms / الآن_مللي — الطابع الزمني بالمللي ثانية
            auto now_ms_fn = [](Sad::Interpreter::BuiltinContext &) -> std::shared_ptr<Data::Value>
            {
                auto t = std::chrono::system_clock::now();
                auto epoch = t.time_since_epoch();
                double ms = std::chrono::duration<double, std::milli>(epoch).count();
                return makeVal(ms);
            };
            fm.registerBuiltinFunction(std::string(Bmp::NOW_MS), now_ms_fn);

            // date_format / صيغة_تاريخ — تنسيق التاريخ
            auto date_format_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime;
                if (!args.empty() && args[0]->isNumeric())
                {
                    rawtime = static_cast<time_t>(args[0]->toDouble());
                }
                else
                {
                    rawtime = std::time(nullptr);
                }
                std::string fmt = "%Y-%m-%d %H:%M:%S";
                if (args.size() >= 2 && args[1]->isString())
                    fmt = args[1]->toString();
                struct tm timeinfo;
#ifdef _WIN32
                localtime_s(&timeinfo, &rawtime);
#else
                localtime_r(&rawtime, &timeinfo);
#endif
                char buffer[256];
                std::strftime(buffer, sizeof(buffer), fmt.c_str(), &timeinfo);
                return makeVal(std::string(buffer));
            };
            fm.registerBuiltinFunction(std::string(Bmp::DATE_FORMAT), date_format_fn);

            // date components / مكونات التاريخ — استخراج مكونات التاريخ
            auto date_component_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime;
                if (!args.empty() && args[0]->isNumeric())
                {
                    rawtime = static_cast<time_t>(args[0]->toDouble());
                }
                else
                {
                    rawtime = std::time(nullptr);
                }
                struct tm timeinfo;
#ifdef _WIN32
                localtime_s(&timeinfo, &rawtime);
#else
                localtime_r(&rawtime, &timeinfo);
#endif
                Data::Value::MapType dateMap;
                dateMap["سنة"] = Data::Value(timeinfo.tm_year + 1900);
                dateMap["year"] = Data::Value(timeinfo.tm_year + 1900);
                dateMap["شهر"] = Data::Value(timeinfo.tm_mon + 1);
                dateMap["month"] = Data::Value(timeinfo.tm_mon + 1);
                dateMap["يوم"] = Data::Value(timeinfo.tm_mday);
                dateMap["day"] = Data::Value(timeinfo.tm_mday);
                dateMap["ساعة"] = Data::Value(timeinfo.tm_hour);
                dateMap["hour"] = Data::Value(timeinfo.tm_hour);
                dateMap["دقيقة"] = Data::Value(timeinfo.tm_min);
                dateMap["minute"] = Data::Value(timeinfo.tm_min);
                dateMap["ثانية"] = Data::Value(timeinfo.tm_sec);
                dateMap["second"] = Data::Value(timeinfo.tm_sec);
                dateMap["يوم_الأسبوع"] = Data::Value(timeinfo.tm_wday);
                dateMap["weekday"] = Data::Value(timeinfo.tm_wday);
                return makeMapVal(dateMap);
            };
            fm.registerBuiltinFunction(std::string(Bmp::DATE_COMPONENTS), date_component_fn);

            // year, month, day, hour, minute, second, weekday
            auto year_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_year + 1900);
            };
            fm.registerBuiltinFunction(std::string(Bmp::YEAR), year_fn);

            // (AR) شهر — استخراج الشهر من كائن تاريخ
            // (EN) month — extract month from date object
            auto month_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_mon + 1);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MONTH), month_fn);

            // (AR) يوم — استخراج اليوم من كائن تاريخ
            // (EN) day — extract day from date object
            auto day_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_mday);
            };
            fm.registerBuiltinFunction(std::string(Bmp::DAY), day_fn);

            // (AR) ساعة — استخراج الساعة من كائن تاريخ
            // (EN) hour — extract hour from date object
            auto hour_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_hour);
            };
            fm.registerBuiltinFunction(std::string(Bmp::HOUR), hour_fn);

            // (AR) دقيقة — استخراج الدقيقة من كائن تاريخ
            // (EN) minute — extract minute from date object
            auto minute_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_min);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MINUTE), minute_fn);

            // (AR) ثانية — استخراج الثانية من كائن تاريخ
            // (EN) second — extract second from date object
            auto second_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_sec);
            };
            fm.registerBuiltinFunction(std::string(Bmp::SECOND), second_fn);

            // (AR) يوم_الأسبوع — استخراج يوم الأسبوع من كائن تاريخ
            // (EN) weekday — extract weekday from date object
            auto weekday_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_wday);
            };
            fm.registerBuiltinFunction(std::string(Bmp::WEEKDAY), weekday_fn);

            // time_diff / فرق_وقت — الفرق بين طابعين زمنيين (بالثواني)
            auto time_diff_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return makeVal(args[0]->toDouble() - args[1]->toDouble());
            };
            fm.registerBuiltinFunction(std::string(Bmp::TIME_DIFF), time_diff_fn);

            // ═══════════════════════════════════════════════════════════════════
            // (AR) القسم 5 (رياضيات) — حُذف: جميع هذه الدوال متوفرة الآن عبر:
            //   - stdlib_manager (عند بدء التشغيل): جيب، جتا، ظل، معكوس_جيب...
            //   - وحدة رياضيات (builtin_module_math.cpp): لوغ، أسي، حصر...
            //   انظر: CLEANUP_LOG.md للتفاصيل
            // ═══════════════════════════════════════════════════════════════════

            // ═══════════════════════════════════════════════════════════════════
            // 7. دوال الملفات الإضافية / Extra File Functions
            // (AR) إصلاح نقطة ضعف رقم 7: نقص دوال المسارات والملفات
            // ═══════════════════════════════════════════════════════════════════

            // file_size / حجم_ملف — حجم ملف بالبايتات
            auto file_size_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string path = args[0]->toString();
                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (!file.is_open())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_FILE_ERROR,
                              {{"path", path}, {"reason", "(AR) تعذّر الفتح / (EN) cannot open"}});
                return makeVal(static_cast<int>(file.tellg()));
            };
            fm.registerBuiltinFunction(std::string(Bmp::FILE_SIZE), file_size_fn);

            // path_join / ضم_مسار — ضم أجزاء مسار
            auto path_join_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    return makeVal(std::string(""));
                std::string result = args[0]->toString();
                for (size_t i = 1; i < args.size(); i++)
                {
                    std::string part = args[i]->toString();
                    if (!result.empty() && result.back() != '/' && result.back() != '\\')
                    {
#ifdef _WIN32
                        result += '\\';
#else
                        result += '/';
#endif
                    }
                    result += part;
                }
                return makeVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::PATH_JOIN), path_join_fn);

            // path_dirname / مجلد_مسار — استخراج المجلد من مسار
            auto path_dirname_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string path = args[0]->toString();
                auto pos = path.find_last_of("/\\");
                if (pos == std::string::npos)
                    return makeVal(std::string("."));
                return makeVal(path.substr(0, pos));
            };
            fm.registerBuiltinFunction(std::string(Bmp::PATH_DIRNAME), path_dirname_fn);

            // path_basename / اسم_ملف — استخراج اسم الملف من مسار
            auto path_basename_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string path = args[0]->toString();
                auto pos = path.find_last_of("/\\");
                if (pos == std::string::npos)
                    return makeVal(path);
                return makeVal(path.substr(pos + 1));
            };
            fm.registerBuiltinFunction(std::string(Bmp::PATH_BASENAME), path_basename_fn);

            // path_extension / امتداد_ملف — استخراج الامتداد من مسار
            auto path_extension_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string path = args[0]->toString();
                auto pos = path.find_last_of('.');
                if (pos == std::string::npos)
                    return makeVal(std::string(""));
                return makeVal(path.substr(pos));
            };
            fm.registerBuiltinFunction(std::string(Bmp::PATH_EXT), path_extension_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 8. التسلسل / Serialization (base64, hex, url)
            // (AR) إصلاح نقطة ضعف رقم 8: عدم وجود دوال التسلسل
            // ═══════════════════════════════════════════════════════════════════

            // base64_encode / ترميز_قاعدة64 — ترميز Base64
            auto base64_encode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                std::string input = args[0]->toString();
                std::string output;
                int val = 0, bits = -6;
                const unsigned int mask = 0x3F;
                for (unsigned char c : input)
                {
                    val = (val << 8) + c;
                    bits += 8;
                    while (bits >= 0)
                    {
                        output.push_back(table[(val >> bits) & mask]);
                        bits -= 6;
                    }
                }
                if (bits > -6)
                    output.push_back(table[((val << 8) >> (bits + 8)) & mask]);
                while (output.size() % 4)
                    output.push_back('=');
                return makeVal(output);
            };
            fm.registerBuiltinFunction(std::string(Bmp::BASE64_ENCODE), base64_encode_fn);

            // base64_decode / فك_قاعدة64 — فك ترميز Base64
            auto base64_decode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                static const std::string table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                std::string input = args[0]->toString();
                std::string output;
                int val = 0, bits = -8;
                for (unsigned char c : input)
                {
                    if (c == '=')
                        break;
                    auto pos = table.find(c);
                    if (pos == std::string::npos)
                        continue;
                    val = (val << 6) + static_cast<int>(pos);
                    bits += 6;
                    if (bits >= 0)
                    {
                        output.push_back(static_cast<char>((val >> bits) & 0xFF));
                        bits -= 8;
                    }
                }
                return makeVal(output);
            };
            fm.registerBuiltinFunction(std::string(Bmp::BASE64_DECODE), base64_decode_fn);

            // hex_encode / ترميز_ست_عشري — ترميز إلى سداسي عشري
            auto hex_encode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                std::ostringstream oss;
                for (unsigned char c : input)
                {
                    oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c);
                }
                return makeVal(oss.str());
            };
            fm.registerBuiltinFunction(std::string(Bmp::HEX_ENCODE), hex_encode_fn);

            // url_encode / ترميز_عنوان — ترميز URL
            auto url_encode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                std::ostringstream oss;
                for (unsigned char c : input)
                {
                    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                    {
                        oss << c;
                    }
                    else
                    {
                        oss << '%' << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(c);
                    }
                }
                return makeVal(oss.str());
            };
            fm.registerBuiltinFunction(std::string(Bmp::URL_ENCODE), url_encode_fn);

            // url_decode / فك_عنوان — فك ترميز URL
            auto url_decode_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string input = args[0]->toString();
                std::string output;
                output.reserve(input.size());

                for (size_t i = 0; i < input.size(); i++)
                {
                    if (input[i] == '%' && i + 2 < input.size())
                    {
                        std::string hex = input.substr(i + 1, 2);

                        // التحقق من صحة أحرف hex
                        bool validHex = true;
                        for (char c : hex)
                        {
                            if (!std::isxdigit(static_cast<unsigned char>(c)))
                            {
                                validHex = false;
                                break;
                            }
                        }

                        if (validHex)
                        {
                            try
                            {
                                output += static_cast<char>(std::stoi(hex, nullptr, 16));
                                i += 2;
                            }
                            catch (const std::exception &)
                            {
                                // إذا فشل التحويل، نضيف الحرف كما هو
                                output += input[i];
                            }
                        }
                        else
                        {
                            // أحرف غير صالحة - نضيف الحرف كما هو
                            output += input[i];
                        }
                    }
                    else if (input[i] == '+')
                    {
                        output += ' ';
                    }
                    else
                    {
                        output += input[i];
                    }
                }
                return makeVal(output);
            };
            fm.registerBuiltinFunction(std::string(Bmp::URL_DECODE), url_decode_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 9. بيئة النظام / System Environment
            // (AR) إصلاح نقطة ضعف رقم 9: نقص دوال بيئة النظام
            // ═══════════════════════════════════════════════════════════════════

            // env_get / متغير_بيئة — الحصول على متغير بيئة
            auto env_get_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string name = args[0]->toString();
#ifdef _WIN32
                char *buf = nullptr;
                size_t len = 0;
                if (_dupenv_s(&buf, &len, name.c_str()) == 0 && buf != nullptr)
                {
                    std::string result(buf);
                    free(buf);
                    return makeVal(result);
                }
#else
                const char *val = std::getenv(name.c_str());
                if (val)
                    return makeVal(std::string(val));
#endif
                if (args.size() >= 2)
                    return args[1]; // قيمة افتراضية
                return makeVoidVal();
            };
            fm.registerBuiltinFunction(std::string(Bmp::ENV_GET), env_get_fn);

            // cwd / المجلد_الحالي — الحصول على المجلد الحالي
            auto cwd_fn = [](Sad::Interpreter::BuiltinContext &) -> std::shared_ptr<Data::Value>
            {
                char buffer[4096];
                if (getcwd(buffer, sizeof(buffer)))
                {
                    return makeVal(std::string(buffer));
                }
                return makeVal(std::string("."));
            };
            fm.registerBuiltinFunction(std::string(Bmp::CWD), cwd_fn);

            // platform / المنصة — اسم نظام التشغيل

            // ═══════════════════════════════════════════════════════════════════
            // 10. معالجة الأخطاء المحسنة / Enhanced Error Handling
            // (AR) إصلاح نقطة ضعف رقم 10: نقص بنية الأخطاء
            // ═══════════════════════════════════════════════════════════════════

            // error / خطأ — إنشاء خطأ ورميه
            auto error_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                std::string msg = args.empty() ? "(AR) خطأ غير محدد" : args[0]->toString();
                // (AR) EM-CPP: رمي خام مقصود — دالة `خطأ` ترفع رسالة المستخدم الحرّة (لا خطأ
                //      كتالوج). يُلتقَط بالموزّع. لا يُحوَّل لـ ctx.error (الذي يتطلب رمزاً).
                throw std::runtime_error(msg);
                return makeVoidVal(); // لن يصل هنا
            };
            fm.registerBuiltinFunction(std::string(Bmp::ERROR_FN), error_fn);

            // typeof / نوع_القيمة — الحصول على نوع القيمة كنص

        } // registerBuiltinsMapsUtils

    } // namespace Interpreter
} // namespace Sad
