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
            auto now_fn = [](const std::vector<std::shared_ptr<Data::Value>> &) -> std::shared_ptr<Data::Value>
            {
                auto t = std::chrono::system_clock::now();
                auto epoch = t.time_since_epoch();
                double secs = std::chrono::duration<double>(epoch).count();
                return makeVal(secs);
            };
            fm.registerBuiltinFunction("الآن", now_fn);
            fm.registerBuiltinFunction("now", now_fn);
            // now_ms / الآن_مللي — الطابع الزمني بالمللي ثانية
            auto now_ms_fn = [](const std::vector<std::shared_ptr<Data::Value>> &) -> std::shared_ptr<Data::Value>
            {
                auto t = std::chrono::system_clock::now();
                auto epoch = t.time_since_epoch();
                double ms = std::chrono::duration<double, std::milli>(epoch).count();
                return makeVal(ms);
            };
            fm.registerBuiltinFunction("الآن_مللي", now_ms_fn);
            fm.registerBuiltinFunction("now_ms", now_ms_fn);

            // date_format / صيغة_تاريخ — تنسيق التاريخ
            auto date_format_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
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
            fm.registerBuiltinFunction("صيغة_تاريخ", date_format_fn);
            fm.registerBuiltinFunction("date_format", date_format_fn);

            // date components / مكونات التاريخ — استخراج مكونات التاريخ
            auto date_component_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
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
            fm.registerBuiltinFunction("مكونات_تاريخ", date_component_fn);
            fm.registerBuiltinFunction("date_components", date_component_fn);

            // year, month, day, hour, minute, second, weekday
            auto year_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_year + 1900);
            };
            fm.registerBuiltinFunction("سنة", year_fn);
            fm.registerBuiltinFunction("year", year_fn);

            // (AR) شهر — استخراج الشهر من كائن تاريخ
            // (EN) month — extract month from date object
            auto month_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_mon + 1);
            };
            fm.registerBuiltinFunction("شهر", month_fn);
            fm.registerBuiltinFunction("month", month_fn);

            // (AR) يوم — استخراج اليوم من كائن تاريخ
            // (EN) day — extract day from date object
            auto day_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_mday);
            };
            fm.registerBuiltinFunction("يوم", day_fn);
            fm.registerBuiltinFunction("day", day_fn);

            // (AR) ساعة — استخراج الساعة من كائن تاريخ
            // (EN) hour — extract hour from date object
            auto hour_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_hour);
            };
            fm.registerBuiltinFunction("ساعة", hour_fn);
            fm.registerBuiltinFunction("hour", hour_fn);

            // (AR) دقيقة — استخراج الدقيقة من كائن تاريخ
            // (EN) minute — extract minute from date object
            auto minute_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_min);
            };
            fm.registerBuiltinFunction("دقيقة", minute_fn);
            fm.registerBuiltinFunction("minute", minute_fn);

            // (AR) ثانية — استخراج الثانية من كائن تاريخ
            // (EN) second — extract second from date object
            auto second_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_sec);
            };
            fm.registerBuiltinFunction("ثانية", second_fn);
            fm.registerBuiltinFunction("second", second_fn);

            // (AR) يوم_الأسبوع — استخراج يوم الأسبوع من كائن تاريخ
            // (EN) weekday — extract weekday from date object
            auto weekday_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                time_t rawtime = !args.empty() && args[0]->isNumeric() ? static_cast<time_t>(args[0]->toDouble()) : std::time(nullptr);
                struct tm ti;
#ifdef _WIN32
                localtime_s(&ti, &rawtime);
#else
                localtime_r(&rawtime, &ti);
#endif
                return makeVal(ti.tm_wday);
            };
            fm.registerBuiltinFunction("يوم_الأسبوع", weekday_fn);
            fm.registerBuiltinFunction("weekday", weekday_fn);

            // time_diff / فرق_وقت — الفرق بين طابعين زمنيين (بالثواني)
            auto time_diff_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("(AR) فرق_وقت تتطلب طابعين زمنيين");
                return makeVal(args[0]->toDouble() - args[1]->toDouble());
            };
            fm.registerBuiltinFunction("فرق_وقت", time_diff_fn);
            fm.registerBuiltinFunction("time_diff", time_diff_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 5. ثوابت ودوال رياضية متقدمة / Advanced Math
            // (AR) إصلاح نقطة ضعف رقم 5: نقص الثوابت والدوال الرياضية
            // ═══════════════════════════════════════════════════════════════════

            // PI / ط — ثابت باي
            auto pi_fn = [](const std::vector<std::shared_ptr<Data::Value>> &) -> std::shared_ptr<Data::Value>
            {
                return makeVal(3.14159265358979323846);
            };
            fm.registerBuiltinFunction("ط", pi_fn);
            fm.registerBuiltinFunction("PI", pi_fn);

            // E / هـ — ثابت أويلر
            auto e_fn = [](const std::vector<std::shared_ptr<Data::Value>> &) -> std::shared_ptr<Data::Value>
            {
                return makeVal(2.71828182845904523536);
            };
            fm.registerBuiltinFunction("هـ", e_fn);
            fm.registerBuiltinFunction("E", e_fn);

            // log / لوغاريتم — اللوغاريتم الطبيعي
            auto log_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) لوغاريتم تتطلب رقماً");
                return makeVal(std::log(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("لوغاريتم", log_fn);

            // log2 / لوغاريتم2
            auto log2_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) لوغاريتم2 تتطلب رقماً");
                return makeVal(std::log2(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("لوغاريتم2", log2_fn);

            // log10 / لوغاريتم10
            auto log10_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) لوغاريتم10 تتطلب رقماً");
                return makeVal(std::log10(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("لوغاريتم10", log10_fn);

            // exp / أسي — الدالة الأسية
            auto exp_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) أسي تتطلب رقماً");
                return makeVal(std::exp(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("أسي", exp_fn);

            // sin / جيب
            auto sin_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) جيب تتطلب رقماً");
                return makeVal(std::sin(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("جيب", sin_fn);
            fm.registerBuiltinFunction("sin", sin_fn);

            // cos / جيب_تمام
            auto cos_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) جيب_تمام تتطلب رقماً");
                return makeVal(std::cos(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("جيب_تمام", cos_fn);
            fm.registerBuiltinFunction("cos", cos_fn);

            // tan / ظل
            auto tan_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) ظل تتطلب رقماً");
                return makeVal(std::tan(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("ظل", tan_fn);
            fm.registerBuiltinFunction("tan", tan_fn);

            // asin / جيب_عكسي
            auto asin_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) جيب_عكسي تتطلب رقماً");
                return makeVal(std::asin(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("جيب_عكسي", asin_fn);
            fm.registerBuiltinFunction("asin", asin_fn);

            // acos / جيب_تمام_عكسي
            auto acos_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) جيب_تمام_عكسي تتطلب رقماً");
                return makeVal(std::acos(args[0]->toDouble()));
            };
            fm.registerBuiltinFunction("جيب_تمام_عكسي", acos_fn);
            fm.registerBuiltinFunction("acos", acos_fn);

            // atan2 / ظل_عكسي2
            auto atan2_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("(AR) ظل_عكسي2 تتطلب رقمين");
                return makeVal(std::atan2(args[0]->toDouble(), args[1]->toDouble()));
            };
            fm.registerBuiltinFunction("ظل_عكسي2", atan2_fn);
            fm.registerBuiltinFunction("atan2", atan2_fn);

            // clamp / حصر — حصر قيمة ضمن نطاق
            auto clamp_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 3)
                    throw std::runtime_error("(AR) حصر تتطلب قيمة وحد أدنى وحد أعلى");
                double val = args[0]->toDouble();
                double lo = args[1]->toDouble();
                double hi = args[2]->toDouble();
                return makeVal(std::max(lo, std::min(val, hi)));
            };
            fm.registerBuiltinFunction("حصر", clamp_fn);

            // lerp / استيفاء_خطي — الاستيفاء الخطي
            auto lerp_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 3)
                    throw std::runtime_error("(AR) استيفاء_خطي تتطلب ثلاث قيم");
                double a = args[0]->toDouble();
                double b = args[1]->toDouble();
                double t = args[2]->toDouble();
                return makeVal(a + t * (b - a));
            };
            fm.registerBuiltinFunction("استيفاء_خطي", lerp_fn);
            fm.registerBuiltinFunction("lerp", lerp_fn);

            // random_range / عشوائي_نطاق — رقم عشوائي في نطاق
            auto random_range_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.size() < 2)
                    throw std::runtime_error("(AR) عشوائي_نطاق تتطلب حد أدنى وحد أعلى");
                int lo = args[0]->toInt();
                int hi = args[1]->toInt();
                static std::mt19937 gen(std::random_device{}());
                std::uniform_int_distribution<int> dist(lo, hi);
                return makeVal(dist(gen));
            };
            fm.registerBuiltinFunction("عشوائي_نطاق", random_range_fn);
            fm.registerBuiltinFunction("random_range", random_range_fn);

            // random_float / عشوائي_عشري — رقم عشوائي عشري بين 0 و 1
            auto random_float_fn = [](const std::vector<std::shared_ptr<Data::Value>> &) -> std::shared_ptr<Data::Value>
            {
                static std::mt19937 gen(std::random_device{}());
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                return makeVal(dist(gen));
            };
            fm.registerBuiltinFunction("عشوائي_عشري", random_float_fn);
            fm.registerBuiltinFunction("random_float", random_float_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 7. دوال الملفات الإضافية / Extra File Functions
            // (AR) إصلاح نقطة ضعف رقم 7: نقص دوال المسارات والملفات
            // ═══════════════════════════════════════════════════════════════════

            // file_size / حجم_ملف — حجم ملف بالبايتات
            auto file_size_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) حجم_ملف تتطلب مسار ملف");
                std::string path = args[0]->toString();
                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (!file.is_open())
                    throw std::runtime_error("(AR) لا يمكن فتح الملف: " + path);
                return makeVal(static_cast<int>(file.tellg()));
            };
            fm.registerBuiltinFunction("حجم_ملف", file_size_fn);
            fm.registerBuiltinFunction("file_size", file_size_fn);

            // path_join / ضم_مسار — ضم أجزاء مسار
            auto path_join_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
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
            fm.registerBuiltinFunction("ضم_مسار", path_join_fn);
            fm.registerBuiltinFunction("path_join", path_join_fn);

            // path_dirname / مجلد_مسار — استخراج المجلد من مسار
            auto path_dirname_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) مجلد_مسار تتطلب مسار");
                std::string path = args[0]->toString();
                auto pos = path.find_last_of("/\\");
                if (pos == std::string::npos)
                    return makeVal(std::string("."));
                return makeVal(path.substr(0, pos));
            };
            fm.registerBuiltinFunction("مجلد_مسار", path_dirname_fn);
            fm.registerBuiltinFunction("path_dirname", path_dirname_fn);

            // path_basename / اسم_ملف — استخراج اسم الملف من مسار
            auto path_basename_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) اسم_ملف تتطلب مسار");
                std::string path = args[0]->toString();
                auto pos = path.find_last_of("/\\");
                if (pos == std::string::npos)
                    return makeVal(path);
                return makeVal(path.substr(pos + 1));
            };
            fm.registerBuiltinFunction("اسم_ملف", path_basename_fn);
            fm.registerBuiltinFunction("path_basename", path_basename_fn);

            // path_extension / امتداد_ملف — استخراج الامتداد من مسار
            auto path_extension_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) امتداد_ملف تتطلب مسار");
                std::string path = args[0]->toString();
                auto pos = path.find_last_of('.');
                if (pos == std::string::npos)
                    return makeVal(std::string(""));
                return makeVal(path.substr(pos));
            };
            fm.registerBuiltinFunction("امتداد_ملف", path_extension_fn);
            fm.registerBuiltinFunction("path_extension", path_extension_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 8. التسلسل / Serialization (base64, hex, url)
            // (AR) إصلاح نقطة ضعف رقم 8: عدم وجود دوال التسلسل
            // ═══════════════════════════════════════════════════════════════════

            // base64_encode / ترميز_قاعدة64 — ترميز Base64
            auto base64_encode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) ترميز_قاعدة64 تتطلب نصاً");
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
            fm.registerBuiltinFunction("ترميز_قاعدة64", base64_encode_fn);

            // base64_decode / فك_قاعدة64 — فك ترميز Base64
            auto base64_decode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) فك_قاعدة64 تتطلب نصاً");
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
            fm.registerBuiltinFunction("فك_قاعدة64", base64_decode_fn);
            fm.registerBuiltinFunction("base64_decode", base64_decode_fn);

            // hex_encode / ترميز_ست_عشري — ترميز إلى سداسي عشري
            auto hex_encode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) ترميز_ست_عشري تتطلب نصاً");
                std::string input = args[0]->toString();
                std::ostringstream oss;
                for (unsigned char c : input)
                {
                    oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c);
                }
                return makeVal(oss.str());
            };
            fm.registerBuiltinFunction("ترميز_ست_عشري", hex_encode_fn);
            fm.registerBuiltinFunction("hex_encode", hex_encode_fn);

            // url_encode / ترميز_عنوان — ترميز URL
            auto url_encode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) ترميز_عنوان تتطلب نصاً");
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
            fm.registerBuiltinFunction("ترميز_عنوان", url_encode_fn);
            fm.registerBuiltinFunction("url_encode", url_encode_fn);

            // url_decode / فك_عنوان — فك ترميز URL
            auto url_decode_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) فك_عنوان تتطلب نصاً");
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
            fm.registerBuiltinFunction("فك_عنوان", url_decode_fn);
            fm.registerBuiltinFunction("url_decode", url_decode_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 9. بيئة النظام / System Environment
            // (AR) إصلاح نقطة ضعف رقم 9: نقص دوال بيئة النظام
            // ═══════════════════════════════════════════════════════════════════

            // env_get / متغير_بيئة — الحصول على متغير بيئة
            auto env_get_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw std::runtime_error("(AR) متغير_بيئة تتطلب اسم المتغير");
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
            fm.registerBuiltinFunction("متغير_بيئة", env_get_fn);
            fm.registerBuiltinFunction("env_get", env_get_fn);

            // cwd / المجلد_الحالي — الحصول على المجلد الحالي
            auto cwd_fn = [](const std::vector<std::shared_ptr<Data::Value>> &) -> std::shared_ptr<Data::Value>
            {
                char buffer[4096];
                if (getcwd(buffer, sizeof(buffer)))
                {
                    return makeVal(std::string(buffer));
                }
                return makeVal(std::string("."));
            };
            fm.registerBuiltinFunction("المجلد_الحالي", cwd_fn);
            fm.registerBuiltinFunction("cwd", cwd_fn);

            // platform / المنصة — اسم نظام التشغيل

            // ═══════════════════════════════════════════════════════════════════
            // 10. معالجة الأخطاء المحسنة / Enhanced Error Handling
            // (AR) إصلاح نقطة ضعف رقم 10: نقص بنية الأخطاء
            // ═══════════════════════════════════════════════════════════════════

            // error / خطأ — إنشاء خطأ ورميه
            auto error_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                std::string msg = args.empty() ? "(AR) خطأ غير محدد" : args[0]->toString();
                throw std::runtime_error(msg);
                return makeVoidVal(); // لن يصل هنا
            };
            fm.registerBuiltinFunction("خطأ", error_fn);
            fm.registerBuiltinFunction("error", error_fn);

            // typeof / نوع_القيمة — الحصول على نوع القيمة كنص


        } // registerBuiltinsMapsUtils

    } // namespace Interpreter
} // namespace Sad
