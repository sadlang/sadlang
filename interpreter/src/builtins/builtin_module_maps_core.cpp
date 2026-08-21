/**
 * @file builtin_module_maps_core.cpp
 * @brief (AR) وحدة الخرائط الأساسية — منشئات الأنواع وعمليات الخرائط والبرمجة الوظيفية والمجموعات
 * @brief (EN) Core maps module — type constructors, map operations, functional programming, set operations
 *
 * @details
 * (AR) الأقسام:
 *   0. منشئات الأنواع (خريطة، مصفوفة)
 *   1. عمليات الخرائط (احصل، عيّن، مفاتيح، قيم، دمج...)
 *   2. البرمجة الوظيفية (تخطيط، تصفية، اختزال، ضم...)
 *   11. عمليات المجموعات (فريد، اتحاد، تقاطع، فرق)
 *
 * @note يتطلب: Interpreter& للبرمجة الوظيفية (callUserFunction)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
// (AR) kindToArabic: اسم النوع من مصدره الواحد لرسالة RUN074 (لا سلاسل مباشرة)
#include "sad_type_utils.h"
#include <algorithm>
#include <unordered_set>

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#ifdef VOID
#undef VOID
#endif

// (AR) اختصار لفضاء أسماء ثوابت وحدة الخرائط
namespace Bmp = Sad::Builtins::Names::Maps;

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

        void registerBuiltinsMapsCore(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════════
            // 0. منشئات الأنواع / Type Constructors
            // (AR) دوال إنشاء أنواع البيانات الأساسية
            // ═══════════════════════════════════════════════════════════════════

            // خريطة() — إنشاء خريطة فارغة أو من أزواج
            auto map_constructor_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                Data::Value::MapType map;
                // إذا لم تُعطَ وسائط، أرجع خريطة فارغة
                if (args.empty())
                {
                    return makeMapVal(map);
                }
                // إذا أُعطيت وسائط زوجية (مفتاح، قيمة، مفتاح، قيمة...)
                if (args.size() % 2 == 0)
                {
                    for (size_t i = 0; i < args.size(); i += 2)
                    {
                        map[args[i]->toString()] = *args[i + 1];
                    }
                }
                return makeMapVal(map);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_CTOR), map_constructor_fn);

            // مصفوفة() — إنشاء مصفوفة فارغة أو من عناصر

            // ═══════════════════════════════════════════════════════════════════
            // 1. عمليات الخرائط / Map Operations
            // (AR) إصلاح نقطة ضعف رقم 1: عدم وجود دوال للتعامل مع الخرائط
            // ═══════════════════════════════════════════════════════════════════

            // map_get / خريطة_احصل — الحصول على قيمة من خريطة
            auto map_get_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (!args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &map = args[0]->toMapRef();
                std::string key = args[1]->toString();
                auto it = map.find(key);
                if (it != map.end())
                    return std::make_shared<Data::Value>(it->second);
                if (args.size() >= 3)
                    return args[2]; // قيمة افتراضية
                return makeVoidVal();
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_GET), map_get_fn);

            // ═══════════════════════════════════════════════════════════════════
            // (AR) الجلب المصنَّف (RFC عقد الغياب — المرحلة ب): خريطة_اجلب_نص/رقم/منطقي.
            //      الغيابُ «لاشيء» حصرًا (لا «فراغ» — عكس خريطة_احصل الديناميكيّة)؛
            //      والحضورُ بنوعٍ مغايرٍ **أو بعدمٍ مخزَّن** خطأُ تشغيلٍ صريح (RUN074)
            //      كي لا تكون للعدمِ قناتان مُنتِجتان. لا وسيطَ بديلًا ثالثًا —
            //      التحصيلُ بـ«؟؟» هو الطريقُ الواحد.
            // (EN) Typed fetch (absence-contract RFC, stage ب): absence returns Null
            //      exclusively (not Void); presence with a different type OR a stored
            //      null is an explicit runtime error (RUN074) so null never has two
            //      producing channels. No third default argument — «؟؟» is the way.
            // ═══════════════════════════════════════════════════════════════════
            auto make_typed_fetch = [](std::string funcName,
                                       bool (Data::Value::*matches)() const,
                                       ::Sad::Types::SadTypeKind expectedKind)
            {
                const std::string expectedName = ::Sad::Types::kindToArabic(expectedKind);
                return [funcName, matches, expectedName](Sad::Interpreter::BuiltinContext &ctx)
                           -> std::shared_ptr<Data::Value>
                {
                    const auto &args = ctx.args();
                    if (args.size() != 2)
                        ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG,
                                  {{"func", funcName}});
                    if (!args[0]->isMap())
                        ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG,
                                  {{"func", funcName}});
                    const auto &map = args[0]->toMapRef();
                    const std::string key = args[1]->toString();
                    auto it = map.find(key);
                    if (it == map.end())
                        return std::make_shared<Data::Value>(Data::Value::makeNull());
                    if (!((it->second).*matches)())
                        ctx.error(::Sad::Errors::ErrorCode::RUN_MAP_FETCH_TYPE_MISMATCH,
                                  {{"func", funcName},
                                   {"key", key},
                                   {"expected", expectedName},
                                   // (AR) الاسمُ من مصدره الواحد — getTypeName تُرجع
                                   //      «NULL» الإنجليزيّة للعدم (قِيس).
                                   // (EN) Single-source name — getTypeName returns the
                                   //      English "NULL" for null (measured).
                                   {"found", ::Sad::Types::kindToArabic(it->second.getType())}});
                    return std::make_shared<Data::Value>(it->second);
                };
            };
            fm.registerBuiltinFunction(
                std::string(Bmp::MAP_FETCH_STR),
                make_typed_fetch(std::string(Bmp::MAP_FETCH_STR), &Data::Value::isString,
                                 ::Sad::Types::SadTypeKind::String));
            fm.registerBuiltinFunction(
                std::string(Bmp::MAP_FETCH_NUM),
                make_typed_fetch(std::string(Bmp::MAP_FETCH_NUM), &Data::Value::isInteger,
                                 ::Sad::Types::SadTypeKind::Integer));
            fm.registerBuiltinFunction(
                std::string(Bmp::MAP_FETCH_BOOL),
                make_typed_fetch(std::string(Bmp::MAP_FETCH_BOOL), &Data::Value::isBoolean,
                                 ::Sad::Types::SadTypeKind::Boolean));

            // map_set / خريطة_عيّن — تعيين قيمة في خريطة (يرجع خريطة جديدة)
            auto map_set_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 3)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                if (!args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto map = args[0]->toMap(); // نسخة
                map[args[1]->toString()] = *args[2];
                return makeMapVal(map);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_SET), map_set_fn);

            // map_keys / خريطة_مفاتيح — الحصول على مفاتيح الخريطة
            auto map_keys_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                Data::Value::ArrayType keys;
                for (const auto &[k, v] : args[0]->toMapRef())
                {
                    keys.push_back(Data::Value(k));
                }
                return makeArrayVal(keys);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_KEYS), map_keys_fn);

            // map_values / خريطة_قيم — الحصول على قيم الخريطة
            auto map_values_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                Data::Value::ArrayType vals;
                for (const auto &[k, v] : args[0]->toMapRef())
                {
                    vals.push_back(v);
                }
                return makeArrayVal(vals);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_VALUES), map_values_fn);

            // map_has_key / خريطة_تحتوي — التحقق من وجود مفتاح
            auto map_has_key_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &map = args[0]->toMapRef();
                return makeVal(map.find(args[1]->toString()) != map.end());
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_HAS_KEY), map_has_key_fn);

            // map_delete / خريطة_احذف — حذف مفتاح (يرجع خريطة جديدة)
            auto map_delete_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto map = args[0]->toMap(); // نسخة
                map.erase(args[1]->toString());
                return makeMapVal(map);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_DELETE), map_delete_fn);

            // map_size / خريطة_حجم — حجم الخريطة
            auto map_size_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return makeVal(static_cast<int>(args[0]->toMapRef().size()));
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_SIZE), map_size_fn);

            // map_entries / خريطة_عناصر — كل عنصر كمصفوفة [مفتاح، قيمة]
            auto map_entries_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                Data::Value::ArrayType entries;
                for (const auto &[k, v] : args[0]->toMapRef())
                {
                    Data::Value::ArrayType pair;
                    pair.push_back(Data::Value(k));
                    pair.push_back(v);
                    entries.push_back(Data::Value(pair));
                }
                return makeArrayVal(entries);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_ENTRIES), map_entries_fn);

            // map_merge / خريطة_دمج — دمج خريطتين
            auto map_merge_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isMap() || !args[1]->isMap())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto result = args[0]->toMap(); // نسخة
                for (const auto &[k, v] : args[1]->toMapRef())
                {
                    result[k] = v;
                }
                return makeMapVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_MERGE), map_merge_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 2. البرمجة الوظيفية / Functional Programming
            // (AR) إصلاح نقطة ضعف رقم 2: عدم وجود map/filter/reduce
            // ═══════════════════════════════════════════════════════════════════

            // map / تخطيط — تطبيق دالة على كل عنصر
            auto map_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                Data::Value::ArrayType result;
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {item};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    result.push_back(res);
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAP_FN), map_fn);

            // filter / تصفية — تصفية عناصر المصفوفة
            auto filter_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                Data::Value::ArrayType result;
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {item};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    if (res.toBool())
                        result.push_back(item);
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::FILTER), filter_fn);

            // reduce / اختزال — تجميع عناصر المصفوفة
            auto reduce_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 3 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                Data::Value accumulator = *args[2];
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {accumulator, item};
                    accumulator = interpreter.callUserFunction(funcName, callArgs);
                }
                return std::make_shared<Data::Value>(accumulator);
            };
            fm.registerBuiltinFunction(std::string(Bmp::REDUCE), reduce_fn);

            // forEach / لكل_عنصر — تنفيذ دالة على كل عنصر
            auto forEach_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {item};
                    interpreter.callUserFunction(funcName, callArgs);
                }
                return makeVoidVal();
            };
            fm.registerBuiltinFunction(std::string(Bmp::FOR_EACH), forEach_fn);

            // flatMap / تخطيط_مسطح — تخطيط ثم تسطيح
            auto flatMap_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                Data::Value::ArrayType result;
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {item};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    if (res.isArray())
                    {
                        for (const auto &sub : res.toArrayRef())
                            result.push_back(sub);
                    }
                    else
                    {
                        result.push_back(res);
                    }
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::FLAT_MAP), flatMap_fn);

            // zip / ضم — ضم مصفوفتين في مصفوفة أزواج
            auto zip_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &a = args[0]->toArrayRef();
                const auto &b = args[1]->toArrayRef();
                size_t len = std::min(a.size(), b.size());
                Data::Value::ArrayType result;
                for (size_t i = 0; i < len; i++)
                {
                    Data::Value::ArrayType pair;
                    pair.push_back(a[i]);
                    pair.push_back(b[i]);
                    result.push_back(Data::Value(pair));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::ZIP), zip_fn);

            // any / أي_عنصر — هل هناك عنصر يحقق الشرط
            auto any_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {item};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    if (res.toBool())
                        return makeVal(true);
                }
                return makeVal(false);
            };
            fm.registerBuiltinFunction(std::string(Bmp::ANY_ELEMENT), any_fn);

            // all / كل_العناصر — هل كل العناصر تحقق الشرط
            auto all_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION);
                const auto &arr = args[0]->toArrayRef();
                std::string funcName = args[1]->toString();
                for (const auto &item : arr)
                {
                    std::vector<Data::Value> callArgs = {item};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    if (!res.toBool())
                        return makeVal(false);
                }
                return makeVal(true);
            };
            fm.registerBuiltinFunction(std::string(Bmp::ALL_ELEMENTS), all_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 11. عمليات المجموعات / Set Operations
            // (AR) إصلاح نقطة ضعف رقم 11: عدم وجود عمليات مجموعات
            // ═══════════════════════════════════════════════════════════════════

            // unique / فريد — إزالة العناصر المكررة من مصفوفة
            auto unique_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                Data::Value::ArrayType result;
                std::unordered_set<std::string> seen;
                for (const auto &item : arr)
                {
                    std::string key = item.toString();
                    if (seen.find(key) == seen.end())
                    {
                        seen.insert(key);
                        result.push_back(item);
                    }
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::UNIQUE), unique_fn);

            // union / اتحاد — اتحاد مصفوفتين
            auto union_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                Data::Value::ArrayType result = args[0]->toArray();
                std::unordered_set<std::string> seen;
                for (const auto &item : result)
                    seen.insert(item.toString());
                for (const auto &item : args[1]->toArrayRef())
                {
                    std::string key = item.toString();
                    if (seen.find(key) == seen.end())
                    {
                        seen.insert(key);
                        result.push_back(item);
                    }
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::UNION), union_fn);

            // intersect / تقاطع — تقاطع مصفوفتين
            auto intersect_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::unordered_set<std::string> setB;
                for (const auto &item : args[1]->toArrayRef())
                    setB.insert(item.toString());
                Data::Value::ArrayType result;
                for (const auto &item : args[0]->toArrayRef())
                {
                    if (setB.count(item.toString()))
                    {
                        result.push_back(item);
                    }
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::INTERSECT), intersect_fn);

            // difference / فرق — الفرق بين مصفوفتين
            auto difference_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray() || !args[1]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::unordered_set<std::string> setB;
                for (const auto &item : args[1]->toArrayRef())
                    setB.insert(item.toString());
                Data::Value::ArrayType result;
                for (const auto &item : args[0]->toArrayRef())
                {
                    if (!setB.count(item.toString()))
                    {
                        result.push_back(item);
                    }
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::DIFFERENCE), difference_fn);

        } // registerBuiltinsMapsCore

    } // namespace Interpreter
} // namespace Sad
