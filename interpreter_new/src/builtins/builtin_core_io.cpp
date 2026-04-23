/**
 * @file builtin_core_io.cpp
 * @brief (AR) الدوال الأساسية — الإدخال/الإخراج وتحويل الأنواع والتزامن الأساسي
 * @brief (EN) Core builtins — I/O, type conversion, basic concurrency
 *
 * @details
 * (AR) الأقسام:
 *   1. الطباعة (اطبع، اطبع_سطر)
 *   2. الإدخال (اقرأ)
 *   3. تحويل الأنواع (نص، رقم، عشري، منطقي)
 *   4. الأطوال والأنواع (طول، النوع)
 *   5. التزامن الأساسي (قناة، قفل، مستقبل...)
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "channel.h"
#include "object_instance.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        using namespace StdLib;

        void registerCoreIOBuiltins(Interpreter &interpreter)
        {
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال الإخراج / (EN) Output Functions
            // ═════════════════════════════════════════════════════════════════

            auto print_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::print(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("اطبع", print_func);
            // (AR) اطبع_سطر — طباعة قيمة مع سطر جديد
            // (EN) println — print value with newline
            auto println_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::println(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("اطبع_سطر", println_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال الإدخال / (EN) Input Functions
            // ═════════════════════════════════════════════════════════════════

            auto input_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::input(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("اقرأ", input_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال الطول الأساسية / (EN) Basic Length Functions
            // ═════════════════════════════════════════════════════════════════

            auto len_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::length(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("طول", len_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال النوع / (EN) Type Functions
            // ═════════════════════════════════════════════════════════════════

            auto type_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::type_of(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("النوع", type_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) تحويل الأنواع الأساسية / (EN) Basic Type Conversion
            // ═════════════════════════════════════════════════════════════════

            // نص / str / to_string
            auto to_string_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::to_string(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("نص", to_string_func);

            // رقم / int / to_int
            auto to_int_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::to_int(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("رقم", to_int_func);
            // عشري / float / to_float
            auto to_float_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                return BuiltinFunctions::to_float(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("عشري", to_float_func);

            // منطقي / bool / to_bool
            auto to_bool_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
            {
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toBool(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction("منطقي", to_bool_func);

            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال القنوات والتزامن / (EN) Channel & Concurrency Functions
            // ═════════════════════════════════════════════════════════════════

            // (AR) قناة(حجم) — إنشاء قناة اتصال جديدة
            // (EN) channel(size) — create a new communication channel
            auto channel_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                // (AR) حجم المخزن المؤقت — 0 افتراضياً (unbuffered)
                size_t bufferSize = 0;
                if (!args.empty() && args[0])
                {
                    bufferSize = static_cast<size_t>(args[0]->toInt());
                }

                // (AR) إنشاء القناة
                auto channel = std::make_shared<SadChannel>(bufferSize);

                // (AR) تسجيل القناة في السجل العام
                auto &registry = SadChannelRegistry::getInstance();
                size_t channelId = registry.registerChannel(channel);

                // (AR) إنشاء كائن ObjectInstance يمثل القناة
                auto obj = std::make_shared<Data::ObjectInstance>(nullptr, channelId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x82\xD9\x86\xD8\xA7\xD8\xA9__")); // __قناة__
                obj->fields["__channel_id__"] = Data::Value(static_cast<int>(channelId));

                return std::make_shared<Data::Value>(obj);
            };

            // (AR) تسجيل بالعربية والإنجليزية
            interpreter.getFunctionManager().registerBuiltinFunction("\xD9\x82\xD9\x86\xD8\xA7\xD8\xA9", channel_func); // قناة

            // (AR) انتظر_الكل() — انتظار جميع المهام المتزامنة (goroutines)
            // (EN) waitAll() — wait for all concurrent goroutines to complete
            auto wait_all_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args; // (AR) لا تتطلب وسائط
                GoroutineManager::getInstance().waitAll();
                return std::make_shared<Data::Value>(); // void
            };

            interpreter.getFunctionManager().registerBuiltinFunction("\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xB1_\xD8\xA7\xD9\x84\xD9\x83\xD9\x84", wait_all_func); // انتظر_الكل

            // (AR) عدد_المهام() — عدد المهام المتزامنة النشطة
            // (EN) activeGoroutines() — number of active goroutines
            auto active_count_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                int count = static_cast<int>(GoroutineManager::getInstance().activeCount());
                return std::make_shared<Data::Value>(count);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("\xD8\xB9\xD8\xAF\xD8\xAF_\xD8\xA7\xD9\x84\xD9\x85\xD9\x87\xD8\xA7\xD9\x85", active_count_func); // عدد_المهام

            // ═══════════════════════════════════════════════════════════════
            // (AR) مجموعة_انتظار() — إنشاء WaitGroup جديدة
            // (EN) waitGroup() — create a new WaitGroup
            // ═══════════════════════════════════════════════════════════════
            auto waitgroup_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto wg = std::make_shared<SadWaitGroup>();
                auto &registry = SadWaitGroupRegistry::getInstance();
                size_t wgId = registry.registerWaitGroup(wg);

                // (AR) إنشاء كائن ObjectInstance يمثل مجموعة الانتظار
                auto obj = std::make_shared<Data::ObjectInstance>(nullptr, wgId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x85\xD8\xAC\xD9\x85\xD9\x88\xD8\xB9\xD8\xA9_\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xA7\xD8\xB1__")); // __مجموعة_انتظار__
                obj->fields["__waitgroup_id__"] = Data::Value(static_cast<int>(wgId));

                return std::make_shared<Data::Value>(obj);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("\xD9\x85\xD8\xAC\xD9\x85\xD9\x88\xD8\xB9\xD8\xA9_\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xA7\xD8\xB1", waitgroup_func); // مجموعة_انتظار

            // ═══════════════════════════════════════════════════════════════
            // (AR) قفل() — إنشاء Mutex جديد
            // (EN) mutex() — create a new Mutex
            // ═══════════════════════════════════════════════════════════════
            auto mutex_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto mtx = std::make_shared<SadMutex>();
                auto &registry = SadMutexRegistry::getInstance();
                size_t mtxId = registry.registerMutex(mtx);

                auto obj = std::make_shared<Data::ObjectInstance>(nullptr, mtxId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x82\xD9\x81\xD9\x84__")); // __قفل__
                obj->fields["__mutex_id__"] = Data::Value(static_cast<int>(mtxId));

                return std::make_shared<Data::Value>(obj);
            };

            interpreter.getFunctionManager().registerBuiltinFunction("\xD9\x82\xD9\x81\xD9\x84", mutex_func); // قفل

            // ═══════════════════════════════════════════════════════════════
            // (AR) مستقبل() — إنشاء Future جديد لإرجاع قيمة من goroutine
            // (EN) future() — create a new Future for returning a value from a goroutine
            // ═══════════════════════════════════════════════════════════════
            // (AR) النمط المُوصى به:
            //   متغير م = مستقبل()
            //   أطلق
            //       # ... حسابات ...
            //       م.عيّن(النتيجة)
            //   نهاية
            //   متغير ن = م.احصل()   # يحجب حتى تجهز النتيجة
            // (EN) Recommended pattern:
            //   var f = future()
            //   go { ... f.set(result) }
            //   var r = f.get()  # blocks until result is ready
            auto future_func = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;

                // (AR) إنشاء المستقبل / (EN) Create the future
                auto fut = std::make_shared<SadFuture>();

                // (AR) تسجيل المستقبل في السجل العام / (EN) Register future in global registry
                auto &registry = SadFutureRegistry::getInstance();
                size_t futureId = registry.registerFuture(fut);

                // (AR) إنشاء كائن ObjectInstance يمثل المستقبل / (EN) Create ObjectInstance representing the future
                auto obj = std::make_shared<Data::ObjectInstance>(nullptr, futureId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x85\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84__")); // __مستقبل__
                obj->fields["__future_id__"] = Data::Value(static_cast<int>(futureId));

                return std::make_shared<Data::Value>(obj);
            };

            // (AR) تسجيل بالعربية والإنجليزية / (EN) Register in Arabic and English
            interpreter.getFunctionManager().registerBuiltinFunction("\xD9\x85\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84", future_func); // مستقبل
        }

    } // namespace Interpreter
} // namespace Sad
