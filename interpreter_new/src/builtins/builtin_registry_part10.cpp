/**
 * @file builtin_registry_part10.cpp
 * @brief (AR) الجزء 10: دوال أساسية مفقودة — خرائط، برمجة وظيفية، تاريخ/وقت، تعبيرات نمطية، رياضيات متقدمة
 * @brief (EN) Part 10: Missing fundamentals — Maps, Functional programming, Date/Time, Regex, Advanced Math
 * 
 * هذا الملف يغطي الفجوات الأساسية التالية:
 * 1. عمليات الخرائط/القواميس (Map/Dictionary operations)
 * 2. البرمجة الوظيفية (map, filter, reduce, forEach, flatMap, zip)
 * 3. التاريخ والوقت (Date/Time)
 * 4. التعبيرات النمطية (Regex)
 * 5. ثوابت ودوال رياضية متقدمة (PI, E, log, clamp, lerp)
 * 6. دوال نصوص إضافية (format, repeat, padStart, padEnd, charCodeAt, reverse)
 * 7. دوال ملفات إضافية (file_size, path_join, path_dirname, path_basename)
 * 8. التسلسل (base64, url_encode, hex)
 * 9. بيئة النظام (env_get, cwd, platform)
 * 10. معالجة الأخطاء المحسنة (error, typeof)
 * 11. مجموعات البيانات (Set operations: unique, union, intersect, difference)
 * 12. JSON (json_parse, json_stringify)
 * 13. دوال المصفوفات المتقدمة (flatten, chunk, take, drop, enumerate, sum, average, min, max)
 * 
 * @author فريق تطوير لغة ص
 * @date 2026-02-19
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <functional>
#include <regex>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#ifdef VOID
#undef VOID
#endif

namespace Sad {
namespace Interpreter {

// ═══════════════════════════════════════════════════════════════════════
// (AR) دوال مساعدة / (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════

static std::shared_ptr<Data::Value> makeVal(int v) { return std::make_shared<Data::Value>(v); }
static std::shared_ptr<Data::Value> makeVal(double v) { return std::make_shared<Data::Value>(v); }
static std::shared_ptr<Data::Value> makeVal(const std::string& v) { return std::make_shared<Data::Value>(v); }
static std::shared_ptr<Data::Value> makeVal(bool v) { return std::make_shared<Data::Value>(v); }
static std::shared_ptr<Data::Value> makeVoidVal() { return std::make_shared<Data::Value>(); }
static std::shared_ptr<Data::Value> makeArrayVal(const Data::Value::ArrayType& a) { return std::make_shared<Data::Value>(a); }
static std::shared_ptr<Data::Value> makeMapVal(const Data::Value::MapType& m) { return std::make_shared<Data::Value>(m); }

void registerBuiltinsPart10(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════════
    // 1. عمليات الخرائط / Map Operations
    // (AR) إصلاح نقطة ضعف رقم 1: عدم وجود دوال للتعامل مع الخرائط
    // ═══════════════════════════════════════════════════════════════════

    // map_get / خريطة_احصل — الحصول على قيمة من خريطة
    auto map_get_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) خريطة_احصل تتطلب وسيطين: الخريطة والمفتاح");
        if (!args[0]->isMap()) throw std::runtime_error("(AR) الوسيط الأول يجب أن يكون خريطة");
        const auto& map = args[0]->toMapRef();
        std::string key = args[1]->toString();
        auto it = map.find(key);
        if (it != map.end()) return std::make_shared<Data::Value>(it->second);
        if (args.size() >= 3) return args[2]; // قيمة افتراضية
        return makeVoidVal();
    };
    fm.registerBuiltinFunction("خريطة_احصل", map_get_fn);
    fm.registerBuiltinFunction("map_get", map_get_fn);

    // map_set / خريطة_عيّن — تعيين قيمة في خريطة (يرجع خريطة جديدة)
    auto map_set_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("(AR) خريطة_عيّن تتطلب 3 وسائط: الخريطة، المفتاح، القيمة");
        if (!args[0]->isMap()) throw std::runtime_error("(AR) الوسيط الأول يجب أن يكون خريطة");
        auto map = args[0]->toMap(); // نسخة
        map[args[1]->toString()] = *args[2];
        return makeMapVal(map);
    };
    fm.registerBuiltinFunction("خريطة_عيّن", map_set_fn);
    fm.registerBuiltinFunction("map_set", map_set_fn);

    // map_keys / خريطة_مفاتيح — الحصول على مفاتيح الخريطة
    auto map_keys_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isMap()) throw std::runtime_error("(AR) خريطة_مفاتيح تتطلب خريطة");
        Data::Value::ArrayType keys;
        for (const auto& [k, v] : args[0]->toMapRef()) {
            keys.push_back(Data::Value(k));
        }
        return makeArrayVal(keys);
    };
    fm.registerBuiltinFunction("خريطة_مفاتيح", map_keys_fn);
    fm.registerBuiltinFunction("map_keys", map_keys_fn);

    // map_values / خريطة_قيم — الحصول على قيم الخريطة
    auto map_values_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isMap()) throw std::runtime_error("(AR) خريطة_قيم تتطلب خريطة");
        Data::Value::ArrayType vals;
        for (const auto& [k, v] : args[0]->toMapRef()) {
            vals.push_back(v);
        }
        return makeArrayVal(vals);
    };
    fm.registerBuiltinFunction("خريطة_قيم", map_values_fn);
    fm.registerBuiltinFunction("map_values", map_values_fn);

    // map_has_key / خريطة_تحتوي — التحقق من وجود مفتاح
    auto map_has_key_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isMap()) throw std::runtime_error("(AR) خريطة_تحتوي تتطلب خريطة ومفتاح");
        const auto& map = args[0]->toMapRef();
        return makeVal(map.find(args[1]->toString()) != map.end());
    };
    fm.registerBuiltinFunction("خريطة_تحتوي", map_has_key_fn);
    fm.registerBuiltinFunction("map_has_key", map_has_key_fn);

    // map_delete / خريطة_احذف — حذف مفتاح (يرجع خريطة جديدة)
    auto map_delete_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isMap()) throw std::runtime_error("(AR) خريطة_احذف تتطلب خريطة ومفتاح");
        auto map = args[0]->toMap(); // نسخة
        map.erase(args[1]->toString());
        return makeMapVal(map);
    };
    fm.registerBuiltinFunction("خريطة_احذف", map_delete_fn);
    fm.registerBuiltinFunction("map_delete", map_delete_fn);

    // map_size / خريطة_حجم — حجم الخريطة
    auto map_size_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isMap()) throw std::runtime_error("(AR) خريطة_حجم تتطلب خريطة");
        return makeVal(static_cast<int>(args[0]->toMapRef().size()));
    };
    fm.registerBuiltinFunction("خريطة_حجم", map_size_fn);
    fm.registerBuiltinFunction("map_size", map_size_fn);

    // map_entries / خريطة_عناصر — كل عنصر كمصفوفة [مفتاح، قيمة]
    auto map_entries_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isMap()) throw std::runtime_error("(AR) خريطة_عناصر تتطلب خريطة");
        Data::Value::ArrayType entries;
        for (const auto& [k, v] : args[0]->toMapRef()) {
            Data::Value::ArrayType pair;
            pair.push_back(Data::Value(k));
            pair.push_back(v);
            entries.push_back(Data::Value(pair));
        }
        return makeArrayVal(entries);
    };
    fm.registerBuiltinFunction("خريطة_عناصر", map_entries_fn);
    fm.registerBuiltinFunction("map_entries", map_entries_fn);

    // map_merge / خريطة_دمج — دمج خريطتين
    auto map_merge_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isMap() || !args[1]->isMap())
            throw std::runtime_error("(AR) خريطة_دمج تتطلب خريطتين");
        auto result = args[0]->toMap(); // نسخة
        for (const auto& [k, v] : args[1]->toMapRef()) {
            result[k] = v;
        }
        return makeMapVal(result);
    };
    fm.registerBuiltinFunction("خريطة_دمج", map_merge_fn);
    fm.registerBuiltinFunction("map_merge", map_merge_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 2. البرمجة الوظيفية / Functional Programming
    // (AR) إصلاح نقطة ضعف رقم 2: عدم وجود map/filter/reduce
    // ═══════════════════════════════════════════════════════════════════

    // map / تخطيط — تطبيق دالة على كل عنصر
    auto map_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) تخطيط تتطلب مصفوفة واسم دالة");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        Data::Value::ArrayType result;
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {item};
            auto res = interpreter.callUserFunction(funcName, callArgs);
            result.push_back(res);
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("تخطيط", map_fn);
    fm.registerBuiltinFunction("map", map_fn);

    // filter / تصفية — تصفية عناصر المصفوفة
    auto filter_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) تصفية تتطلب مصفوفة واسم دالة");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        Data::Value::ArrayType result;
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {item};
            auto res = interpreter.callUserFunction(funcName, callArgs);
            if (res.toBool()) result.push_back(item);
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("تصفية", filter_fn);
    fm.registerBuiltinFunction("filter", filter_fn);

    // reduce / اختزال — تجميع عناصر المصفوفة
    auto reduce_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3 || !args[0]->isArray())
            throw std::runtime_error("(AR) اختزال تتطلب مصفوفة واسم دالة وقيمة أولية");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        Data::Value accumulator = *args[2];
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {accumulator, item};
            accumulator = interpreter.callUserFunction(funcName, callArgs);
        }
        return std::make_shared<Data::Value>(accumulator);
    };
    fm.registerBuiltinFunction("اختزال", reduce_fn);
    fm.registerBuiltinFunction("reduce", reduce_fn);

    // forEach / لكل_عنصر — تنفيذ دالة على كل عنصر
    auto forEach_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) لكل_عنصر تتطلب مصفوفة واسم دالة");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {item};
            interpreter.callUserFunction(funcName, callArgs);
        }
        return makeVoidVal();
    };
    fm.registerBuiltinFunction("لكل_عنصر", forEach_fn);
    fm.registerBuiltinFunction("forEach", forEach_fn);
    fm.registerBuiltinFunction("for_each", forEach_fn);

    // flatMap / تخطيط_مسطح — تخطيط ثم تسطيح
    auto flatMap_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) تخطيط_مسطح تتطلب مصفوفة واسم دالة");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        Data::Value::ArrayType result;
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {item};
            auto res = interpreter.callUserFunction(funcName, callArgs);
            if (res.isArray()) {
                for (const auto& sub : res.toArrayRef()) result.push_back(sub);
            } else {
                result.push_back(res);
            }
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("تخطيط_مسطح", flatMap_fn);
    fm.registerBuiltinFunction("flatMap", flatMap_fn);
    fm.registerBuiltinFunction("flat_map", flatMap_fn);

    // zip / ضم — ضم مصفوفتين في مصفوفة أزواج
    auto zip_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
            throw std::runtime_error("(AR) ضم تتطلب مصفوفتين");
        const auto& a = args[0]->toArrayRef();
        const auto& b = args[1]->toArrayRef();
        size_t len = std::min(a.size(), b.size());
        Data::Value::ArrayType result;
        for (size_t i = 0; i < len; i++) {
            Data::Value::ArrayType pair;
            pair.push_back(a[i]);
            pair.push_back(b[i]);
            result.push_back(Data::Value(pair));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("ضم", zip_fn);
    fm.registerBuiltinFunction("zip", zip_fn);

    // any / أي_عنصر — هل هناك عنصر يحقق الشرط
    auto any_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) أي_عنصر تتطلب مصفوفة واسم دالة");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {item};
            auto res = interpreter.callUserFunction(funcName, callArgs);
            if (res.toBool()) return makeVal(true);
        }
        return makeVal(false);
    };
    fm.registerBuiltinFunction("أي_عنصر", any_fn);
    fm.registerBuiltinFunction("any", any_fn);

    // all / كل_العناصر — هل كل العناصر تحقق الشرط
    auto all_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) كل_العناصر تتطلب مصفوفة واسم دالة");
        const auto& arr = args[0]->toArrayRef();
        std::string funcName = args[1]->toString();
        for (const auto& item : arr) {
            std::vector<Data::Value> callArgs = {item};
            auto res = interpreter.callUserFunction(funcName, callArgs);
            if (!res.toBool()) return makeVal(false);
        }
        return makeVal(true);
    };
    fm.registerBuiltinFunction("كل_العناصر", all_fn);
    fm.registerBuiltinFunction("all", all_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 3. التاريخ والوقت / Date & Time
    // (AR) إصلاح نقطة ضعف رقم 3: عدم وجود دوال للتاريخ والوقت
    // ═══════════════════════════════════════════════════════════════════

    // now / الآن — الحصول على الطابع الزمني الحالي (بالثواني)
    auto now_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
        auto t = std::chrono::system_clock::now();
        auto epoch = t.time_since_epoch();
        double secs = std::chrono::duration<double>(epoch).count();
        return makeVal(secs);
    };
    fm.registerBuiltinFunction("الآن", now_fn);
    fm.registerBuiltinFunction("now", now_fn);
    fm.registerBuiltinFunction("الوقت_الحالي", now_fn);

    // now_ms / الآن_مللي — الطابع الزمني بالمللي ثانية
    auto now_ms_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
        auto t = std::chrono::system_clock::now();
        auto epoch = t.time_since_epoch();
        double ms = std::chrono::duration<double, std::milli>(epoch).count();
        return makeVal(ms);
    };
    fm.registerBuiltinFunction("الآن_مللي", now_ms_fn);
    fm.registerBuiltinFunction("now_ms", now_ms_fn);

    // date_format / صيغة_تاريخ — تنسيق التاريخ
    auto date_format_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        time_t rawtime;
        if (!args.empty() && args[0]->isNumeric()) {
            rawtime = static_cast<time_t>(args[0]->toDouble());
        } else {
            rawtime = std::time(nullptr);
        }
        std::string fmt = "%Y-%m-%d %H:%M:%S";
        if (args.size() >= 2 && args[1]->isString()) fmt = args[1]->toString();
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
    auto date_component_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        time_t rawtime;
        if (!args.empty() && args[0]->isNumeric()) {
            rawtime = static_cast<time_t>(args[0]->toDouble());
        } else {
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
    auto year_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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

    auto month_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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

    auto day_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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

    auto hour_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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

    auto minute_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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

    auto second_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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

    auto weekday_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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
    auto time_diff_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) فرق_وقت تتطلب طابعين زمنيين");
        return makeVal(args[0]->toDouble() - args[1]->toDouble());
    };
    fm.registerBuiltinFunction("فرق_وقت", time_diff_fn);
    fm.registerBuiltinFunction("time_diff", time_diff_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 4. التعبيرات النمطية / Regex
    // (AR) إصلاح نقطة ضعف رقم 4: عدم وجود دعم للتعبيرات النمطية
    // ═══════════════════════════════════════════════════════════════════

    // regex_match / تعبير_مطابقة — مطابقة كاملة
    auto regex_match_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تعبير_مطابقة تتطلب نص ونمط");
        try {
            std::string text = args[0]->toString();
            std::regex pattern(args[1]->toString());
            return makeVal(std::regex_match(text, pattern));
        } catch (const std::regex_error& e) {
            throw std::runtime_error(std::string("(AR) خطأ في التعبير النمطي: ") + e.what());
        }
    };
    fm.registerBuiltinFunction("تعبير_مطابقة", regex_match_fn);
    fm.registerBuiltinFunction("regex_match", regex_match_fn);

    // regex_search / تعبير_بحث — بحث جزئي
    auto regex_search_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تعبير_بحث تتطلب نص ونمط");
        try {
            std::string text = args[0]->toString();
            std::regex pattern(args[1]->toString());
            std::smatch match;
            if (std::regex_search(text, match, pattern)) {
                return makeVal(match[0].str());
            }
            return makeVoidVal();
        } catch (const std::regex_error& e) {
            throw std::runtime_error(std::string("(AR) خطأ في التعبير النمطي: ") + e.what());
        }
    };
    fm.registerBuiltinFunction("تعبير_بحث", regex_search_fn);
    fm.registerBuiltinFunction("regex_search", regex_search_fn);

    // regex_replace / تعبير_استبدال — استبدال بنمط
    auto regex_replace_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("(AR) تعبير_استبدال تتطلب نص ونمط وبديل");
        try {
            std::string text = args[0]->toString();
            std::regex pattern(args[1]->toString());
            std::string replacement = args[2]->toString();
            return makeVal(std::regex_replace(text, pattern, replacement));
        } catch (const std::regex_error& e) {
            throw std::runtime_error(std::string("(AR) خطأ في التعبير النمطي: ") + e.what());
        }
    };
    fm.registerBuiltinFunction("تعبير_استبدال", regex_replace_fn);
    fm.registerBuiltinFunction("regex_replace", regex_replace_fn);

    // regex_find_all / تعبير_جد_الكل — إيجاد جميع المطابقات
    auto regex_find_all_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تعبير_جد_الكل تتطلب نص ونمط");
        try {
            std::string text = args[0]->toString();
            std::regex pattern(args[1]->toString());
            Data::Value::ArrayType results;
            auto begin = std::sregex_iterator(text.begin(), text.end(), pattern);
            auto end = std::sregex_iterator();
            for (auto it = begin; it != end; ++it) {
                results.push_back(Data::Value((*it)[0].str()));
            }
            return makeArrayVal(results);
        } catch (const std::regex_error& e) {
            throw std::runtime_error(std::string("(AR) خطأ في التعبير النمطي: ") + e.what());
        }
    };
    fm.registerBuiltinFunction("تعبير_جد_الكل", regex_find_all_fn);
    fm.registerBuiltinFunction("regex_find_all", regex_find_all_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 5. ثوابت ودوال رياضية متقدمة / Advanced Math
    // (AR) إصلاح نقطة ضعف رقم 5: نقص الثوابت والدوال الرياضية
    // ═══════════════════════════════════════════════════════════════════

    // PI / ط — ثابت باي
    auto pi_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
        return makeVal(3.14159265358979323846);
    };
    fm.registerBuiltinFunction("ط", pi_fn);
    fm.registerBuiltinFunction("PI", pi_fn);

    // E / هـ — ثابت أويلر
    auto e_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
        return makeVal(2.71828182845904523536);
    };
    fm.registerBuiltinFunction("هـ", e_fn);
    fm.registerBuiltinFunction("E", e_fn);

    // log / لوغاريتم — اللوغاريتم الطبيعي
    auto log_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) لوغاريتم تتطلب رقماً");
        return makeVal(std::log(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("لوغاريتم", log_fn);
    fm.registerBuiltinFunction("log", log_fn);

    // log2 / لوغاريتم2
    auto log2_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) لوغاريتم2 تتطلب رقماً");
        return makeVal(std::log2(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("لوغاريتم2", log2_fn);
    fm.registerBuiltinFunction("log2", log2_fn);

    // log10 / لوغاريتم10
    auto log10_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) لوغاريتم10 تتطلب رقماً");
        return makeVal(std::log10(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("لوغاريتم10", log10_fn);
    fm.registerBuiltinFunction("log10", log10_fn);

    // exp / أسي — الدالة الأسية
    auto exp_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) أسي تتطلب رقماً");
        return makeVal(std::exp(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("أسي", exp_fn);
    fm.registerBuiltinFunction("exp", exp_fn);

    // sin / جيب
    auto sin_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) جيب تتطلب رقماً");
        return makeVal(std::sin(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("جيب", sin_fn);
    fm.registerBuiltinFunction("sin", sin_fn);

    // cos / جيب_تمام
    auto cos_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) جيب_تمام تتطلب رقماً");
        return makeVal(std::cos(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("جيب_تمام", cos_fn);
    fm.registerBuiltinFunction("cos", cos_fn);

    // tan / ظل
    auto tan_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) ظل تتطلب رقماً");
        return makeVal(std::tan(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("ظل", tan_fn);
    fm.registerBuiltinFunction("tan", tan_fn);

    // asin / جيب_عكسي
    auto asin_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) جيب_عكسي تتطلب رقماً");
        return makeVal(std::asin(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("جيب_عكسي", asin_fn);
    fm.registerBuiltinFunction("asin", asin_fn);

    // acos / جيب_تمام_عكسي
    auto acos_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) جيب_تمام_عكسي تتطلب رقماً");
        return makeVal(std::acos(args[0]->toDouble()));
    };
    fm.registerBuiltinFunction("جيب_تمام_عكسي", acos_fn);
    fm.registerBuiltinFunction("acos", acos_fn);

    // atan2 / ظل_عكسي2
    auto atan2_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) ظل_عكسي2 تتطلب رقمين");
        return makeVal(std::atan2(args[0]->toDouble(), args[1]->toDouble()));
    };
    fm.registerBuiltinFunction("ظل_عكسي2", atan2_fn);
    fm.registerBuiltinFunction("atan2", atan2_fn);

    // clamp / حصر — حصر قيمة ضمن نطاق
    auto clamp_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("(AR) حصر تتطلب قيمة وحد أدنى وحد أعلى");
        double val = args[0]->toDouble();
        double lo = args[1]->toDouble();
        double hi = args[2]->toDouble();
        return makeVal(std::max(lo, std::min(val, hi)));
    };
    fm.registerBuiltinFunction("حصر", clamp_fn);
    fm.registerBuiltinFunction("clamp", clamp_fn);

    // lerp / استيفاء_خطي — الاستيفاء الخطي
    auto lerp_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("(AR) استيفاء_خطي تتطلب ثلاث قيم");
        double a = args[0]->toDouble();
        double b = args[1]->toDouble();
        double t = args[2]->toDouble();
        return makeVal(a + t * (b - a));
    };
    fm.registerBuiltinFunction("استيفاء_خطي", lerp_fn);
    fm.registerBuiltinFunction("lerp", lerp_fn);

    // random_range / عشوائي_نطاق — رقم عشوائي في نطاق
    auto random_range_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) عشوائي_نطاق تتطلب حد أدنى وحد أعلى");
        int lo = args[0]->toInt();
        int hi = args[1]->toInt();
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dist(lo, hi);
        return makeVal(dist(gen));
    };
    fm.registerBuiltinFunction("عشوائي_نطاق", random_range_fn);
    fm.registerBuiltinFunction("random_range", random_range_fn);

    // random_float / عشوائي_عشري — رقم عشوائي عشري بين 0 و 1
    auto random_float_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return makeVal(dist(gen));
    };
    fm.registerBuiltinFunction("عشوائي_عشري", random_float_fn);
    fm.registerBuiltinFunction("random_float", random_float_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 6. دوال نصوص إضافية / Extra String Functions
    // (AR) إصلاح نقطة ضعف رقم 6: نقص دوال النصوص
    // ═══════════════════════════════════════════════════════════════════

    // repeat / تكرار_نص — تكرار نص عدة مرات
    auto repeat_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تكرار_نص تتطلب نص وعدد");
        std::string text = args[0]->toString();
        int count = args[1]->toInt();
        std::string result;
        for (int i = 0; i < count; i++) result += text;
        return makeVal(result);
    };
    fm.registerBuiltinFunction("تكرار_نص", repeat_fn);
    fm.registerBuiltinFunction("repeat", repeat_fn);

    // padStart / حشو_بداية — حشو نص من البداية
    auto padStart_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) حشو_بداية تتطلب نص وطول");
        std::string text = args[0]->toString();
        int targetLen = args[1]->toInt();
        std::string pad = " ";
        if (args.size() >= 3) pad = args[2]->toString();
        while (static_cast<int>(text.size()) < targetLen) {
            text = pad + text;
        }
        if (static_cast<int>(text.size()) > targetLen) text = text.substr(text.size() - targetLen);
        return makeVal(text);
    };
    fm.registerBuiltinFunction("حشو_بداية", padStart_fn);
    fm.registerBuiltinFunction("padStart", padStart_fn);
    fm.registerBuiltinFunction("pad_start", padStart_fn);

    // padEnd / حشو_نهاية — حشو نص من النهاية
    auto padEnd_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) حشو_نهاية تتطلب نص وطول");
        std::string text = args[0]->toString();
        int targetLen = args[1]->toInt();
        std::string pad = " ";
        if (args.size() >= 3) pad = args[2]->toString();
        while (static_cast<int>(text.size()) < targetLen) {
            text += pad;
        }
        if (static_cast<int>(text.size()) > targetLen) text = text.substr(0, targetLen);
        return makeVal(text);
    };
    fm.registerBuiltinFunction("حشو_نهاية", padEnd_fn);
    fm.registerBuiltinFunction("padEnd", padEnd_fn);
    fm.registerBuiltinFunction("pad_end", padEnd_fn);

    // reverse_string / عكس_نص — عكس نص
    auto reverse_string_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) عكس_نص تتطلب نصاً");
        std::string text = args[0]->toString();
        std::reverse(text.begin(), text.end());
        return makeVal(text);
    };
    fm.registerBuiltinFunction("عكس_نص", reverse_string_fn);
    fm.registerBuiltinFunction("reverse_string", reverse_string_fn);

    // charCodeAt / رمز_حرف — الحصول على رمز UTF-8 لحرف
    auto charCodeAt_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) رمز_حرف تتطلب نص وموضع");
        std::string text = args[0]->toString();
        int idx = args[1]->toInt();
        if (idx < 0 || idx >= static_cast<int>(text.size()))
            throw std::runtime_error("(AR) الموضع خارج النطاق");
        return makeVal(static_cast<int>(static_cast<unsigned char>(text[idx])));
    };
    fm.registerBuiltinFunction("رمز_حرف", charCodeAt_fn);
    fm.registerBuiltinFunction("charCodeAt", charCodeAt_fn);
    fm.registerBuiltinFunction("char_code_at", charCodeAt_fn);

    // fromCharCode / حرف_من_رمز — إنشاء حرف من رمز
    auto fromCharCode_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) حرف_من_رمز تتطلب رقماً");
        return makeVal(std::string(1, static_cast<char>(args[0]->toInt())));
    };
    fm.registerBuiltinFunction("حرف_من_رمز", fromCharCode_fn);
    fm.registerBuiltinFunction("fromCharCode", fromCharCode_fn);
    fm.registerBuiltinFunction("from_char_code", fromCharCode_fn);

    // count / عدّ — عدد ظهور نص فرعي في نص
    auto count_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) عدّ تتطلب نص ونص فرعي");
        std::string text = args[0]->toString();
        std::string sub = args[1]->toString();
        if (sub.empty()) return makeVal(0);
        int count = 0;
        size_t pos = 0;
        while ((pos = text.find(sub, pos)) != std::string::npos) {
            count++;
            pos += sub.length();
        }
        return makeVal(count);
    };
    fm.registerBuiltinFunction("عدّ", count_fn);
    fm.registerBuiltinFunction("عد", count_fn);
    fm.registerBuiltinFunction("count", count_fn);

    // format / تنسيق — تنسيق نص بسيط (استبدال {} بالقيم)
    auto format_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) تنسيق تتطلب نص قالب على الأقل");
        std::string tmpl = args[0]->toString();
        std::string result;
        size_t argIdx = 1;
        size_t i = 0;
        while (i < tmpl.size()) {
            if (i + 1 < tmpl.size() && tmpl[i] == '{' && tmpl[i + 1] == '}') {
                if (argIdx < args.size()) {
                    result += args[argIdx]->toString();
                    argIdx++;
                } else {
                    result += "{}";
                }
                i += 2;
            } else {
                result += tmpl[i];
                i++;
            }
        }
        return makeVal(result);
    };
    fm.registerBuiltinFunction("تنسيق", format_fn);
    fm.registerBuiltinFunction("format", format_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 7. دوال الملفات الإضافية / Extra File Functions
    // (AR) إصلاح نقطة ضعف رقم 7: نقص دوال المسارات والملفات
    // ═══════════════════════════════════════════════════════════════════

    // file_size / حجم_ملف — حجم ملف بالبايتات
    auto file_size_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) حجم_ملف تتطلب مسار ملف");
        std::string path = args[0]->toString();
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) throw std::runtime_error("(AR) لا يمكن فتح الملف: " + path);
        return makeVal(static_cast<int>(file.tellg()));
    };
    fm.registerBuiltinFunction("حجم_ملف", file_size_fn);
    fm.registerBuiltinFunction("file_size", file_size_fn);

    // path_join / ضم_مسار — ضم أجزاء مسار
    auto path_join_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return makeVal(std::string(""));
        std::string result = args[0]->toString();
        for (size_t i = 1; i < args.size(); i++) {
            std::string part = args[i]->toString();
            if (!result.empty() && result.back() != '/' && result.back() != '\\') {
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
    auto path_dirname_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) مجلد_مسار تتطلب مسار");
        std::string path = args[0]->toString();
        auto pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return makeVal(std::string("."));
        return makeVal(path.substr(0, pos));
    };
    fm.registerBuiltinFunction("مجلد_مسار", path_dirname_fn);
    fm.registerBuiltinFunction("path_dirname", path_dirname_fn);

    // path_basename / اسم_ملف — استخراج اسم الملف من مسار
    auto path_basename_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) اسم_ملف تتطلب مسار");
        std::string path = args[0]->toString();
        auto pos = path.find_last_of("/\\");
        if (pos == std::string::npos) return makeVal(path);
        return makeVal(path.substr(pos + 1));
    };
    fm.registerBuiltinFunction("اسم_ملف", path_basename_fn);
    fm.registerBuiltinFunction("path_basename", path_basename_fn);

    // path_extension / امتداد_ملف — استخراج الامتداد من مسار
    auto path_extension_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) امتداد_ملف تتطلب مسار");
        std::string path = args[0]->toString();
        auto pos = path.find_last_of('.');
        if (pos == std::string::npos) return makeVal(std::string(""));
        return makeVal(path.substr(pos));
    };
    fm.registerBuiltinFunction("امتداد_ملف", path_extension_fn);
    fm.registerBuiltinFunction("path_extension", path_extension_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 8. التسلسل / Serialization (base64, hex, url)
    // (AR) إصلاح نقطة ضعف رقم 8: عدم وجود دوال التسلسل
    // ═══════════════════════════════════════════════════════════════════

    // base64_encode / ترميز_قاعدة64 — ترميز Base64
    auto base64_encode_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) ترميز_قاعدة64 تتطلب نصاً");
        static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string input = args[0]->toString();
        std::string output;
        int val = 0, bits = -6;
        const unsigned int mask = 0x3F;
        for (unsigned char c : input) {
            val = (val << 8) + c;
            bits += 8;
            while (bits >= 0) {
                output.push_back(table[(val >> bits) & mask]);
                bits -= 6;
            }
        }
        if (bits > -6) output.push_back(table[((val << 8) >> (bits + 8)) & mask]);
        while (output.size() % 4) output.push_back('=');
        return makeVal(output);
    };
    fm.registerBuiltinFunction("ترميز_قاعدة64", base64_encode_fn);
    fm.registerBuiltinFunction("base64_encode", base64_encode_fn);

    // base64_decode / فك_قاعدة64 — فك ترميز Base64
    auto base64_decode_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) فك_قاعدة64 تتطلب نصاً");
        static const std::string table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string input = args[0]->toString();
        std::string output;
        int val = 0, bits = -8;
        for (unsigned char c : input) {
            if (c == '=') break;
            auto pos = table.find(c);
            if (pos == std::string::npos) continue;
            val = (val << 6) + static_cast<int>(pos);
            bits += 6;
            if (bits >= 0) {
                output.push_back(static_cast<char>((val >> bits) & 0xFF));
                bits -= 8;
            }
        }
        return makeVal(output);
    };
    fm.registerBuiltinFunction("فك_قاعدة64", base64_decode_fn);
    fm.registerBuiltinFunction("base64_decode", base64_decode_fn);

    // hex_encode / ترميز_ست_عشري — ترميز إلى سداسي عشري
    auto hex_encode_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) ترميز_ست_عشري تتطلب نصاً");
        std::string input = args[0]->toString();
        std::ostringstream oss;
        for (unsigned char c : input) {
            oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c);
        }
        return makeVal(oss.str());
    };
    fm.registerBuiltinFunction("ترميز_ست_عشري", hex_encode_fn);
    fm.registerBuiltinFunction("hex_encode", hex_encode_fn);

    // url_encode / ترميز_عنوان — ترميز URL
    auto url_encode_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) ترميز_عنوان تتطلب نصاً");
        std::string input = args[0]->toString();
        std::ostringstream oss;
        for (unsigned char c : input) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                oss << c;
            } else {
                oss << '%' << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(c);
            }
        }
        return makeVal(oss.str());
    };
    fm.registerBuiltinFunction("ترميز_عنوان", url_encode_fn);
    fm.registerBuiltinFunction("url_encode", url_encode_fn);

    // url_decode / فك_عنوان — فك ترميز URL
    auto url_decode_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) فك_عنوان تتطلب نصاً");
        std::string input = args[0]->toString();
        std::string output;
        output.reserve(input.size());
        
        for (size_t i = 0; i < input.size(); i++) {
            if (input[i] == '%' && i + 2 < input.size()) {
                std::string hex = input.substr(i + 1, 2);
                
                // التحقق من صحة أحرف hex
                bool validHex = true;
                for (char c : hex) {
                    if (!std::isxdigit(static_cast<unsigned char>(c))) {
                        validHex = false;
                        break;
                    }
                }
                
                if (validHex) {
                    try {
                        output += static_cast<char>(std::stoi(hex, nullptr, 16));
                        i += 2;
                    } catch (const std::exception&) {
                        // إذا فشل التحويل، نضيف الحرف كما هو
                        output += input[i];
                    }
                } else {
                    // أحرف غير صالحة - نضيف الحرف كما هو
                    output += input[i];
                }
            } else if (input[i] == '+') {
                output += ' ';
            } else {
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
    auto env_get_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) متغير_بيئة تتطلب اسم المتغير");
        std::string name = args[0]->toString();
#ifdef _WIN32
        char* buf = nullptr;
        size_t len = 0;
        if (_dupenv_s(&buf, &len, name.c_str()) == 0 && buf != nullptr) {
            std::string result(buf);
            free(buf);
            return makeVal(result);
        }
#else
        const char* val = std::getenv(name.c_str());
        if (val) return makeVal(std::string(val));
#endif
        if (args.size() >= 2) return args[1]; // قيمة افتراضية
        return makeVoidVal();
    };
    fm.registerBuiltinFunction("متغير_بيئة", env_get_fn);
    fm.registerBuiltinFunction("env_get", env_get_fn);

    // cwd / المجلد_الحالي — الحصول على المجلد الحالي
    auto cwd_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
        char buffer[4096];
        if (getcwd(buffer, sizeof(buffer))) {
            return makeVal(std::string(buffer));
        }
        return makeVal(std::string("."));
    };
    fm.registerBuiltinFunction("المجلد_الحالي", cwd_fn);
    fm.registerBuiltinFunction("cwd", cwd_fn);

    // platform / المنصة — اسم نظام التشغيل
    auto platform_fn = [](const std::vector<std::shared_ptr<Data::Value>>&) -> std::shared_ptr<Data::Value> {
#ifdef _WIN32
        return makeVal(std::string("windows"));
#elif __APPLE__
        return makeVal(std::string("macos"));
#elif __linux__
        return makeVal(std::string("linux"));
#else
        return makeVal(std::string("unknown"));
#endif
    };
    fm.registerBuiltinFunction("المنصة", platform_fn);
    fm.registerBuiltinFunction("platform", platform_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 10. معالجة الأخطاء المحسنة / Enhanced Error Handling
    // (AR) إصلاح نقطة ضعف رقم 10: نقص بنية الأخطاء
    // ═══════════════════════════════════════════════════════════════════

    // error / خطأ — إنشاء خطأ ورميه
    auto error_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string msg = args.empty() ? "(AR) خطأ غير محدد" : args[0]->toString();
        throw std::runtime_error(msg);
        return makeVoidVal(); // لن يصل هنا
    };
    fm.registerBuiltinFunction("خطأ", error_fn);
    fm.registerBuiltinFunction("error", error_fn);

    // typeof / نوع_القيمة — الحصول على نوع القيمة كنص
    auto typeof_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return makeVal(std::string("فراغ"));
        const auto& val = *args[0];
        if (val.isVoid()) return makeVal(std::string("فراغ"));
        if (val.isInteger()) return makeVal(std::string("رقم"));
        if (val.isDouble()) return makeVal(std::string("عشري"));
        if (val.isString()) return makeVal(std::string("نص"));
        if (val.isBoolean()) return makeVal(std::string("منطقي"));
        if (val.isArray()) return makeVal(std::string("مصفوفة"));
        if (val.isMap()) return makeVal(std::string("خريطة"));
        if (val.isObject()) return makeVal(std::string("كائن"));
        return makeVal(std::string("مجهول"));
    };
    fm.registerBuiltinFunction("نوع_القيمة", typeof_fn);
    fm.registerBuiltinFunction("typeof", typeof_fn);
    fm.registerBuiltinFunction("نوع", typeof_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 11. عمليات المجموعات / Set Operations
    // (AR) إصلاح نقطة ضعف رقم 11: عدم وجود عمليات مجموعات
    // ═══════════════════════════════════════════════════════════════════

    // unique / فريد — إزالة العناصر المكررة من مصفوفة
    auto unique_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) فريد تتطلب مصفوفة");
        const auto& arr = args[0]->toArrayRef();
        Data::Value::ArrayType result;
        std::unordered_set<std::string> seen;
        for (const auto& item : arr) {
            std::string key = item.toString();
            if (seen.find(key) == seen.end()) {
                seen.insert(key);
                result.push_back(item);
            }
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("فريد", unique_fn);
    fm.registerBuiltinFunction("unique", unique_fn);

    // union / اتحاد — اتحاد مصفوفتين
    auto union_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
            throw std::runtime_error("(AR) اتحاد تتطلب مصفوفتين");
        Data::Value::ArrayType result = args[0]->toArray();
        std::unordered_set<std::string> seen;
        for (const auto& item : result) seen.insert(item.toString());
        for (const auto& item : args[1]->toArrayRef()) {
            std::string key = item.toString();
            if (seen.find(key) == seen.end()) {
                seen.insert(key);
                result.push_back(item);
            }
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("اتحاد", union_fn);
    fm.registerBuiltinFunction("union_arr", union_fn);

    // intersect / تقاطع — تقاطع مصفوفتين
    auto intersect_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
            throw std::runtime_error("(AR) تقاطع تتطلب مصفوفتين");
        std::unordered_set<std::string> setB;
        for (const auto& item : args[1]->toArrayRef()) setB.insert(item.toString());
        Data::Value::ArrayType result;
        for (const auto& item : args[0]->toArrayRef()) {
            if (setB.count(item.toString())) {
                result.push_back(item);
            }
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("تقاطع", intersect_fn);
    fm.registerBuiltinFunction("intersect", intersect_fn);

    // difference / فرق — الفرق بين مصفوفتين
    auto difference_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
            throw std::runtime_error("(AR) فرق تتطلب مصفوفتين");
        std::unordered_set<std::string> setB;
        for (const auto& item : args[1]->toArrayRef()) setB.insert(item.toString());
        Data::Value::ArrayType result;
        for (const auto& item : args[0]->toArrayRef()) {
            if (!setB.count(item.toString())) {
                result.push_back(item);
            }
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("فرق", difference_fn);
    fm.registerBuiltinFunction("difference", difference_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 12. JSON (parse / stringify)
    // (AR) إصلاح نقطة ضعف رقم 12: عدم وجود تسلسل JSON
    // ═══════════════════════════════════════════════════════════════════

    // json_parse / تحليل_جيسون — تحليل نص JSON إلى قيمة
    auto json_parse_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) تحليل_جيسون تتطلب نصاً");
        std::string json = args[0]->toString();
        // (AR) تحليل بسيط — الكائنات المعقدة تحتاج مكتبة JSON كاملة
        // (EN) Simple parsing — complex objects need a full JSON library
        // نصوص مبسطة: "text", numbers, true/false/null, arrays
        // (AR) نعيد النص كما هو إذا لم نستطع تحليله
        if (json.empty()) return makeVoidVal();
        
        // (AR) محاولة تحليل رقم
        if (json[0] == '-' || std::isdigit(json[0])) {
            try {
                if (json.find('.') != std::string::npos) {
                    return makeVal(std::stod(json));
                } else {
                    return makeVal(std::stoi(json));
                }
            } catch (const std::invalid_argument&) {
                // ليس رقماً صالحاً - متابعة المحاولات الأخرى
            } catch (const std::out_of_range&) {
                // رقم خارج النطاق - إرجاع كنص
                return makeVal(json);
            }
        }
        // (AR) محاولة تحليل منطقي أو فراغ
        if (json == "true" || json == "صحيح") return makeVal(true);
        if (json == "false" || json == "خطأ") return makeVal(false);
        if (json == "null" || json == "فارغ") return makeVoidVal();
        // (AR) نص بين علامتي تنصيص
        if (json.size() >= 2 && json.front() == '"' && json.back() == '"') {
            return makeVal(json.substr(1, json.size() - 2));
        }
        // (AR) إرجاع كنص عادي
        return makeVal(json);
    };
    fm.registerBuiltinFunction("تحليل_جيسون", json_parse_fn);
    fm.registerBuiltinFunction("json_parse", json_parse_fn);

    // json_stringify / نص_جيسون — تحويل قيمة إلى نص JSON
    // (AR) دالة مساعدة داخلية للتحويل التعاقبي
    std::function<std::string(const Data::Value&)> jsonStringify;
    jsonStringify = [&jsonStringify](const Data::Value& val) -> std::string {
        if (val.isVoid()) return "null";
        if (val.isInteger()) return std::to_string(val.toInt());
        if (val.isDouble()) {
            std::ostringstream oss;
            oss << val.toDouble();
            return oss.str();
        }
        if (val.isBoolean()) return val.toBool() ? "true" : "false";
        if (val.isString()) {
            std::string s = val.toString();
            // (AR) تهريب الأحرف الخاصة
            std::string escaped = "\"";
            for (char c : s) {
                switch (c) {
                    case '"': escaped += "\\\""; break;
                    case '\\': escaped += "\\\\"; break;
                    case '\n': escaped += "\\n"; break;
                    case '\t': escaped += "\\t"; break;
                    case '\r': escaped += "\\r"; break;
                    default: escaped += c;
                }
            }
            escaped += "\"";
            return escaped;
        }
        if (val.isArray()) {
            const auto& arr = val.toArrayRef();
            std::string result = "[";
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) result += ",";
                result += jsonStringify(arr[i]);
            }
            result += "]";
            return result;
        }
        if (val.isMap()) {
            const auto& mp = val.toMapRef();
            std::string result = "{";
            bool first = true;
            for (const auto& [k, v] : mp) {
                if (!first) result += ",";
                result += "\"" + k + "\":" + jsonStringify(v);
                first = false;
            }
            result += "}";
            return result;
        }
        return "null";
    };

    auto json_stringify_fn = [jsonStringify](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return makeVal(std::string("null"));
        return makeVal(jsonStringify(*args[0]));
    };
    fm.registerBuiltinFunction("نص_جيسون", json_stringify_fn);
    fm.registerBuiltinFunction("json_stringify", json_stringify_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 13. دوال المصفوفات المتقدمة / Advanced Array Functions
    // (AR) إصلاح نقطة ضعف رقم 13: نقص دوال المصفوفات المتقدمة
    // ═══════════════════════════════════════════════════════════════════

    // flatten / تسطيح — تسطيح مصفوفة متداخلة
    auto flatten_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) تسطيح تتطلب مصفوفة");
        Data::Value::ArrayType result;
        std::function<void(const Data::Value::ArrayType&)> flattenHelper;
        flattenHelper = [&result, &flattenHelper](const Data::Value::ArrayType& arr) {
            for (const auto& item : arr) {
                if (item.isArray()) flattenHelper(item.toArrayRef());
                else result.push_back(item);
            }
        };
        flattenHelper(args[0]->toArrayRef());
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("تسطيح", flatten_fn);
    fm.registerBuiltinFunction("flatten", flatten_fn);

    // chunk / تقسيم — تقسيم مصفوفة إلى أجزاء
    auto chunk_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) تقسيم تتطلب مصفوفة وحجم");
        const auto& arr = args[0]->toArrayRef();
        int chunkSize = args[1]->toInt();
        if (chunkSize <= 0) throw std::runtime_error("(AR) حجم التقسيم يجب أن يكون موجباً");
        Data::Value::ArrayType result;
        for (size_t i = 0; i < arr.size(); i += chunkSize) {
            Data::Value::ArrayType chunk;
            for (size_t j = i; j < std::min(i + static_cast<size_t>(chunkSize), arr.size()); j++) {
                chunk.push_back(arr[j]);
            }
            result.push_back(Data::Value(chunk));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("تقسيم", chunk_fn);
    fm.registerBuiltinFunction("chunk", chunk_fn);

    // take / خذ — أخذ أول n عنصر
    auto take_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) خذ تتطلب مصفوفة وعدد");
        const auto& arr = args[0]->toArrayRef();
        int n = args[1]->toInt();
        Data::Value::ArrayType result;
        for (int i = 0; i < n && i < static_cast<int>(arr.size()); i++) {
            result.push_back(arr[i]);
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("خذ", take_fn);
    fm.registerBuiltinFunction("take", take_fn);

    // drop / اترك — حذف أول n عنصر
    auto drop_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray())
            throw std::runtime_error("(AR) اترك تتطلب مصفوفة وعدد");
        const auto& arr = args[0]->toArrayRef();
        int n = args[1]->toInt();
        Data::Value::ArrayType result;
        for (int i = n; i < static_cast<int>(arr.size()); i++) {
            result.push_back(arr[i]);
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("اترك", drop_fn);
    fm.registerBuiltinFunction("drop", drop_fn);

    // enumerate / رقّم — ترقيم عناصر المصفوفة [فهرس، قيمة]
    auto enumerate_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) رقّم تتطلب مصفوفة");
        const auto& arr = args[0]->toArrayRef();
        Data::Value::ArrayType result;
        for (size_t i = 0; i < arr.size(); i++) {
            Data::Value::ArrayType pair;
            pair.push_back(Data::Value(static_cast<int>(i)));
            pair.push_back(arr[i]);
            result.push_back(Data::Value(pair));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("رقّم", enumerate_fn);
    fm.registerBuiltinFunction("enumerate", enumerate_fn);

    // sum_array / مجموع — مجموع عناصر مصفوفة عددية
    auto sum_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) مجموع تتطلب مصفوفة");
        double sum = 0;
        for (const auto& item : args[0]->toArrayRef()) {
            sum += item.toDouble();
        }
        return makeVal(sum);
    };
    fm.registerBuiltinFunction("مجموع", sum_fn);
    fm.registerBuiltinFunction("sum", sum_fn);
    fm.registerBuiltinFunction("sum_array", sum_fn);

    // average / متوسط — متوسط عناصر مصفوفة عددية
    auto average_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) متوسط تتطلب مصفوفة");
        const auto& arr = args[0]->toArrayRef();
        if (arr.empty()) return makeVal(0.0);
        double sum = 0;
        for (const auto& item : arr) sum += item.toDouble();
        return makeVal(sum / static_cast<double>(arr.size()));
    };
    fm.registerBuiltinFunction("متوسط", average_fn);
    fm.registerBuiltinFunction("average", average_fn);

    // min_array / أصغر_المصفوفة — أصغر قيمة في المصفوفة
    auto min_array_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) أصغر_المصفوفة تتطلب مصفوفة");
        const auto& arr = args[0]->toArrayRef();
        if (arr.empty()) throw std::runtime_error("(AR) المصفوفة فارغة");
        double minVal = arr[0].toDouble();
        for (size_t i = 1; i < arr.size(); i++) {
            double v = arr[i].toDouble();
            if (v < minVal) minVal = v;
        }
        return makeVal(minVal);
    };
    fm.registerBuiltinFunction("أصغر_المصفوفة", min_array_fn);
    fm.registerBuiltinFunction("min_array", min_array_fn);

    // max_array / أكبر_المصفوفة — أكبر قيمة في المصفوفة
    auto max_array_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) أكبر_المصفوفة تتطلب مصفوفة");
        const auto& arr = args[0]->toArrayRef();
        if (arr.empty()) throw std::runtime_error("(AR) المصفوفة فارغة");
        double maxVal = arr[0].toDouble();
        for (size_t i = 1; i < arr.size(); i++) {
            double v = arr[i].toDouble();
            if (v > maxVal) maxVal = v;
        }
        return makeVal(maxVal);
    };
    fm.registerBuiltinFunction("أكبر_المصفوفة", max_array_fn);
    fm.registerBuiltinFunction("max_array", max_array_fn);

    // sort_array / ترتيب — ترتيب مصفوفة ​​عددية (مع دعم دالة مقارنة اختيارية)
    auto sort_fn = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) ترتيب تتطلب مصفوفة");
        auto arr = args[0]->toArray(); // نسخة

        // (AR) إذا تم تمرير دالة مقارنة كوسيط ثانٍ
        // (EN) If a comparator function name is passed as second argument
        if (args.size() >= 2 && args[1]->isString()) {
            std::string funcName = args[1]->toString();
            // Check if it's actually a function name (not "تصاعدي"/"تنازلي")
            if (funcName != "تصاعدي" && funcName != "تنازلي" &&
                funcName != "ascending" && funcName != "descending" &&
                funcName != "asc" && funcName != "desc") {
                std::sort(arr.begin(), arr.end(), [&](const Data::Value& a, const Data::Value& b) {
                    std::vector<Data::Value> callArgs = {a, b};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    if (res.isInteger()) return res.toInt() < 0;
                    return res.toBool();
                });
                return makeArrayVal(arr);
            }
            // (AR) ترتيب تنازلي
            if (funcName == "تنازلي" || funcName == "descending" || funcName == "desc") {
                std::sort(arr.begin(), arr.end(), [](const Data::Value& a, const Data::Value& b) {
                    if (a.isNumeric() && b.isNumeric()) return a.toDouble() > b.toDouble();
                    return a.toString() > b.toString();
                });
                return makeArrayVal(arr);
            }
        }

        // (AR) ترتيب تصاعدي عادي (الافتراضي)
        std::sort(arr.begin(), arr.end(), [](const Data::Value& a, const Data::Value& b) {
            if (a.isNumeric() && b.isNumeric()) return a.toDouble() < b.toDouble();
            return a.toString() < b.toString();
        });
        return makeArrayVal(arr);
    };
    fm.registerBuiltinFunction("ترتيب", sort_fn);
    fm.registerBuiltinFunction("sort", sort_fn);
    fm.registerBuiltinFunction("sort_array", sort_fn);

    // reverse_array / عكس_مصفوفة — عكس ترتيب مصفوفة
    auto reverse_arr_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) عكس_مصفوفة تتطلب مصفوفة");
        auto arr = args[0]->toArray(); // نسخة
        std::reverse(arr.begin(), arr.end());
        return makeArrayVal(arr);
    };
    fm.registerBuiltinFunction("عكس_مصفوفة", reverse_arr_fn);
    fm.registerBuiltinFunction("reverse_array", reverse_arr_fn);

    // range / نطاق — إنشاء مصفوفة أرقام من start إلى end
    auto range_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) نطاق تتطلب على الأقل رقماً واحداً");
        int start = 0, end_val = 0, step = 1;
        if (args.size() == 1) {
            end_val = args[0]->toInt();
        } else if (args.size() >= 2) {
            start = args[0]->toInt();
            end_val = args[1]->toInt();
            if (args.size() >= 3) step = args[2]->toInt();
        }
        if (step == 0) throw std::runtime_error("(AR) الخطوة لا يمكن أن تكون صفراً");
        Data::Value::ArrayType result;
        if (step > 0) {
            for (int i = start; i < end_val; i += step) result.push_back(Data::Value(i));
        } else {
            for (int i = start; i > end_val; i += step) result.push_back(Data::Value(i));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("نطاق", range_fn);
    fm.registerBuiltinFunction("range", range_fn);

    // includes / يتضمن — هل تتضمن المصفوفة عنصراً
    auto includes_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray()) throw std::runtime_error("(AR) يتضمن تتطلب مصفوفة وقيمة");
        const auto& arr = args[0]->toArrayRef();
        std::string target = args[1]->toString();
        for (const auto& item : arr) {
            if (item.toString() == target) return makeVal(true);
        }
        return makeVal(false);
    };
    fm.registerBuiltinFunction("يتضمن", includes_fn);
    fm.registerBuiltinFunction("includes", includes_fn);

    // index_of / فهرس — الحصول على فهرس عنصر في مصفوفة
    auto indexOf_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isArray()) throw std::runtime_error("(AR) فهرس تتطلب مصفوفة وقيمة");
        const auto& arr = args[0]->toArrayRef();
        std::string target = args[1]->toString();
        for (size_t i = 0; i < arr.size(); i++) {
            if (arr[i].toString() == target) return makeVal(static_cast<int>(i));
        }
        return makeVal(-1);
    };
    fm.registerBuiltinFunction("فهرس", indexOf_fn);
    fm.registerBuiltinFunction("index_of", indexOf_fn);

    // join_array / ربط — ربط عناصر مصفوفة بفاصل
    auto join_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) ربط تتطلب مصفوفة");
        const auto& arr = args[0]->toArrayRef();
        std::string sep = args.size() >= 2 ? args[1]->toString() : ",";
        std::string result;
        for (size_t i = 0; i < arr.size(); i++) {
            if (i > 0) result += sep;
            result += arr[i].toString();
        }
        return makeVal(result);
    };
    fm.registerBuiltinFunction("ربط", join_fn);
    fm.registerBuiltinFunction("join_array", join_fn);

    // sleep / انتظر — إيقاف التنفيذ لعدد من المللي ثواني
    auto sleep_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) انتظر تتطلب مدة بالمللي ثانية");
        int ms = args[0]->toInt();
        if (ms > 0) {
#ifdef _WIN32
            Sleep(static_cast<DWORD>(ms));
#else
            usleep(ms * 1000);
#endif
        }
        return makeVoidVal();
    };
    fm.registerBuiltinFunction("انتظر", sleep_fn);
    fm.registerBuiltinFunction("sleep", sleep_fn);

    // to_string / إلى_نص — تحويل أي قيمة لنص
    auto to_string_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return makeVal(std::string(""));
        return makeVal(args[0]->toString());
    };
    fm.registerBuiltinFunction("إلى_نص", to_string_fn);
    fm.registerBuiltinFunction("to_string", to_string_fn);

    // to_int / إلى_رقم — تحويل قيمة لرقم صحيح
    auto to_int_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) إلى_رقم تتطلب قيمة");
        return makeVal(args[0]->toInt());
    };
    fm.registerBuiltinFunction("إلى_رقم", to_int_fn);
    fm.registerBuiltinFunction("to_int", to_int_fn);

    // to_float / إلى_عشري — تحويل قيمة لرقم عشري
    auto to_float_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) إلى_عشري تتطلب قيمة");
        return makeVal(args[0]->toDouble());
    };
    fm.registerBuiltinFunction("إلى_عشري", to_float_fn);
    fm.registerBuiltinFunction("to_float", to_float_fn);

    // ═══════════════════════════════════════════════════════════════════
    // 14. دوال يونيكود / Unicode Functions
    // ═══════════════════════════════════════════════════════════════════

    // ازل_تشكيل / strip_diacritics — إزالة التشكيل العربي من النص
    // Arabic diacritics are U+064B to U+065F (encoded as 2-byte UTF-8: 0xD9 0x8B-0x9F, 0xDA 0x80-0x9F)
    auto strip_diacritics_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isString()) throw std::runtime_error("(AR) ازل_تشكيل تتطلب نصاً");
        std::string input = args[0]->toString();
        std::string result;
        result.reserve(input.size());
        size_t i = 0;
        while (i < input.size()) {
            unsigned char c = static_cast<unsigned char>(input[i]);
            if (c < 0x80) {
                // ASCII byte
                result += input[i];
                ++i;
            } else if ((c & 0xE0) == 0xC0 && i + 1 < input.size()) {
                // 2-byte UTF-8 sequence
                unsigned char c2 = static_cast<unsigned char>(input[i + 1]);
                // Decode codepoint
                uint32_t cp = ((c & 0x1F) << 6) | (c2 & 0x3F);
                // Arabic diacritics: U+064B to U+065F (Fathatan to Hamza below)
                // Also U+0610-U+061A (Quranic signs) and U+06D6-U+06ED
                if (cp >= 0x064B && cp <= 0x065F) {
                    i += 2; // skip diacritic
                } else if (cp >= 0x0610 && cp <= 0x061A) {
                    i += 2; // skip Quranic annotation signs
                } else {
                    result += input[i];
                    result += input[i + 1];
                    i += 2;
                }
            } else if ((c & 0xF0) == 0xE0 && i + 2 < input.size()) {
                // 3-byte UTF-8 sequence
                unsigned char c2 = static_cast<unsigned char>(input[i + 1]);
                unsigned char c3 = static_cast<unsigned char>(input[i + 2]);
                uint32_t cp = ((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
                // U+06D6-U+06ED (Quranic marks in 3-byte range if any, but these are 2-byte)
                // U+FE70-U+FEFF (Arabic Presentation Forms-B) — keep these
                result += input[i];
                result += input[i + 1];
                result += input[i + 2];
                i += 3;
            } else if ((c & 0xF8) == 0xF0 && i + 3 < input.size()) {
                // 4-byte UTF-8 — keep as-is
                result += input[i];
                result += input[i + 1];
                result += input[i + 2];
                result += input[i + 3];
                i += 4;
            } else {
                result += input[i];
                ++i;
            }
        }
        return makeVal(result);
    };
    fm.registerBuiltinFunction("ازل_تشكيل", strip_diacritics_fn);
    fm.registerBuiltinFunction("strip_diacritics", strip_diacritics_fn);

    // مقارنة_نص / compare_text — مقارنة نصوص مع خيار تجاهل التشكيل
    // Compares two strings optionally ignoring Arabic diacritics
    auto compare_text_fn = [&strip_diacritics_fn](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2 || !args[0]->isString() || !args[1]->isString())
            throw std::runtime_error("(AR) مقارنة_نص تتطلب نصين");
        
        bool ignoreDiacritics = false;
        if (args.size() >= 3 && args[2]->isBoolean()) {
            ignoreDiacritics = args[2]->toBool();
        }
        
        std::string a = args[0]->toString();
        std::string b = args[1]->toString();
        
        if (ignoreDiacritics) {
            // Strip diacritics from both before comparison
            std::vector<std::shared_ptr<Data::Value>> wrapA = {std::make_shared<Data::Value>(a)};
            std::vector<std::shared_ptr<Data::Value>> wrapB = {std::make_shared<Data::Value>(b)};
            a = strip_diacritics_fn(wrapA)->toString();
            b = strip_diacritics_fn(wrapB)->toString();
        }
        
        return makeVal(a == b);
    };
    fm.registerBuiltinFunction("مقارنة_نص", compare_text_fn);
    fm.registerBuiltinFunction("compare_text", compare_text_fn);

    // نص_يونيكود / unicode_codepoints — تحويل نص إلى مصفوفة نقاط يونيكود
    auto unicode_codepoints_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isString()) throw std::runtime_error("(AR) نص_يونيكود تتطلب نصاً");
        std::string input = args[0]->toString();
        Data::Value::ArrayType codepoints;
        size_t i = 0;
        while (i < input.size()) {
            unsigned char c = static_cast<unsigned char>(input[i]);
            uint32_t cp = 0;
            int bytes = 1;
            if (c < 0x80) {
                cp = c; bytes = 1;
            } else if ((c & 0xE0) == 0xC0 && i + 1 < input.size()) {
                cp = ((c & 0x1F) << 6) | (static_cast<unsigned char>(input[i+1]) & 0x3F);
                bytes = 2;
            } else if ((c & 0xF0) == 0xE0 && i + 2 < input.size()) {
                cp = ((c & 0x0F) << 12) | ((static_cast<unsigned char>(input[i+1]) & 0x3F) << 6) | (static_cast<unsigned char>(input[i+2]) & 0x3F);
                bytes = 3;
            } else if ((c & 0xF8) == 0xF0 && i + 3 < input.size()) {
                cp = ((c & 0x07) << 18) | ((static_cast<unsigned char>(input[i+1]) & 0x3F) << 12) | ((static_cast<unsigned char>(input[i+2]) & 0x3F) << 6) | (static_cast<unsigned char>(input[i+3]) & 0x3F);
                bytes = 4;
            }
            codepoints.push_back(Data::Value(static_cast<int>(cp)));
            i += bytes;
        }
        return makeArrayVal(codepoints);
    };
    fm.registerBuiltinFunction("نص_يونيكود", unicode_codepoints_fn);
    fm.registerBuiltinFunction("unicode_codepoints", unicode_codepoints_fn);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) 16. دوال المصفوفات متعددة الأبعاد (matrix / multidimensional)
    // (EN) 16. Multidimensional array (matrix) functions
    // ═══════════════════════════════════════════════════════════════════

    // مصفوفة_جديدة / matrix_new — إنشاء مصفوفة بأبعاد محددة مملوءة بقيمة افتراضية
    // Usage: مصفوفة_جديدة(صفوف، أعمدة) → zeros matrix
    //        مصفوفة_جديدة(صفوف، أعمدة، قيمة) → filled matrix
    //        مصفوفة_جديدة(صفوف، أعمدة، أعماق) → 3D zeros
    //        مصفوفة_جديدة(صفوف، أعمدة، أعماق، قيمة) → 3D filled
    auto matrix_new_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) مصفوفة_جديدة تتطلب بُعدين على الأقل (صفوف، أعمدة). (EN) matrix_new requires at least 2 dimensions.");
        int64_t rows = args[0]->toInt();
        int64_t cols = args[1]->toInt();
        if (rows <= 0 || cols <= 0) throw std::runtime_error("(AR) الأبعاد يجب أن تكون موجبة. (EN) Dimensions must be positive.");

        // (AR) تحقق من وجود بُعد ثالث (3D)
        if (args.size() >= 3 && args[2]->isNumeric() && args[2]->toInt() > 0 && (args.size() < 4 || args[3]->isNumeric())) {
            // (AR) 3D mode — if args[2] is a positive int and args[3] isn't a non-numeric fill value
            // Check: is args[2] an actual dimension or a fill value?
            // If args.size() == 3, it could be either 3D-zeros or 2D-filled
            // Convention: if args[2] > 0 and args.size() == 4, treat as 3D(rows, cols, depth, fill)
            if (args.size() >= 4) {
                int64_t depth = args[2]->toInt();
                Data::Value fillVal = *args[3];
                Data::Value::ArrayType result;
                for (int64_t i = 0; i < rows; ++i) {
                    Data::Value::ArrayType plane;
                    for (int64_t j = 0; j < cols; ++j) {
                        Data::Value::ArrayType row;
                        for (int64_t k = 0; k < depth; ++k) {
                            row.push_back(fillVal);
                        }
                        plane.push_back(Data::Value(row));
                    }
                    result.push_back(Data::Value(plane));
                }
                return makeArrayVal(result);
            }
            // args.size() == 3: treat it as 2D with fill value
        }
        
        // (AR) 2D mode
        Data::Value fillVal(static_cast<int64_t>(0));
        if (args.size() >= 3) {
            fillVal = *args[2];
        }
        Data::Value::ArrayType result;
        for (int64_t i = 0; i < rows; ++i) {
            Data::Value::ArrayType row;
            for (int64_t j = 0; j < cols; ++j) {
                row.push_back(fillVal);
            }
            result.push_back(Data::Value(row));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("مصفوفة_جديدة", matrix_new_fn);
    fm.registerBuiltinFunction("matrix_new", matrix_new_fn);
    fm.registerBuiltinFunction("matrix", matrix_new_fn);

    // مصفوفة_وحدة / identity_matrix — إنشاء مصفوفة وحدة n×n
    auto identity_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isNumeric()) throw std::runtime_error("(AR) مصفوفة_وحدة تتطلب حجم المصفوفة. (EN) identity_matrix requires size.");
        int64_t n = args[0]->toInt();
        if (n <= 0) throw std::runtime_error("(AR) الحجم يجب أن يكون موجباً. (EN) Size must be positive.");
        Data::Value::ArrayType result;
        for (int64_t i = 0; i < n; ++i) {
            Data::Value::ArrayType row;
            for (int64_t j = 0; j < n; ++j) {
                row.push_back(Data::Value(static_cast<int64_t>(i == j ? 1 : 0)));
            }
            result.push_back(Data::Value(row));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("مصفوفة_وحدة", identity_fn);
    fm.registerBuiltinFunction("identity_matrix", identity_fn);

    // نطاق_مصفوفة / arange — إنشاء مصفوفة أرقام متتالية [start, start+1, ..., end-1]
    auto arange_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) نطاق_مصفوفة تتطلب معاملاً واحداً على الأقل. (EN) arange requires at least one argument.");
        int64_t start = 0, end = 0, step = 1;
        if (args.size() == 1) {
            end = args[0]->toInt();
        } else if (args.size() >= 2) {
            start = args[0]->toInt();
            end = args[1]->toInt();
            if (args.size() >= 3) step = args[2]->toInt();
        }
        if (step == 0) throw std::runtime_error("(AR) الخطوة لا يمكن أن تكون صفراً. (EN) Step cannot be zero.");
        Data::Value::ArrayType result;
        if (step > 0) {
            for (int64_t i = start; i < end; i += step) result.push_back(Data::Value(i));
        } else {
            for (int64_t i = start; i > end; i += step) result.push_back(Data::Value(i));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("نطاق_مصفوفة", arange_fn);
    fm.registerBuiltinFunction("نطاق", arange_fn);
    fm.registerBuiltinFunction("arange", arange_fn);
    fm.registerBuiltinFunction("range", arange_fn);

    // أبعاد / shape — إرجاع أبعاد مصفوفة كدالة مستقلة
    auto shape_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) أبعاد تتطلب مصفوفة. (EN) shape requires an array.");
        Data::Value::ArrayType dims;
        const Data::Value::ArrayType* current = &args[0]->toArrayRef();
        while (true) {
            dims.push_back(Data::Value(static_cast<int64_t>(current->size())));
            if (!current->empty() && (*current)[0].isArray()) {
                current = &((*current)[0].toArrayRef());
            } else {
                break;
            }
        }
        return makeArrayVal(dims);
    };
    fm.registerBuiltinFunction("أبعاد_مصفوفة", shape_fn);
    fm.registerBuiltinFunction("shape", shape_fn);

    // قلب_محوري / transpose — تبديل صفوف وأعمدة مصفوفة ثنائية الأبعاد
    auto transpose_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty() || !args[0]->isArray()) throw std::runtime_error("(AR) قلب_محوري تتطلب مصفوفة ثنائية الأبعاد. (EN) transpose requires a 2D array.");
        const auto& arr = args[0]->toArrayRef();
        if (arr.empty()) return makeArrayVal(Data::Value::ArrayType{});
        if (!arr[0].isArray()) throw std::runtime_error("(AR) قلب_محوري تعمل على مصفوفات ثنائية الأبعاد فقط. (EN) transpose only works on 2D arrays.");
        size_t rows = arr.size();
        size_t cols = arr[0].toArrayRef().size();
        Data::Value::ArrayType result;
        for (size_t c = 0; c < cols; ++c) {
            Data::Value::ArrayType newRow;
            for (size_t r = 0; r < rows; ++r) {
                const auto& row = arr[r].toArrayRef();
                newRow.push_back(c < row.size() ? row[c] : Data::Value());
            }
            result.push_back(Data::Value(newRow));
        }
        return makeArrayVal(result);
    };
    fm.registerBuiltinFunction("قلب_محوري", transpose_fn);
    fm.registerBuiltinFunction("transpose", transpose_fn);

} // registerBuiltinsPart10

} // namespace Interpreter
} // namespace Sad
