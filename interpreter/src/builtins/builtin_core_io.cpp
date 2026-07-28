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
// (AR) ثوابت أسماء الدوال المُولَّدة من language-truth/builtins/*.yaml
// (EN) Generated builtin name constants from language-truth/builtins/*.yaml
#include "builtin_registry.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace Bn = Sad::Builtins::Names;

namespace Sad
{
    namespace Interpreter
    {

        using namespace StdLib;

        // (AR) [طبقة طبيعي64 — الخطوة ٤] يُنتج نسخة من وسائط الطباعة/التحويل مع تصيير قيم
        //      طبيعي64 (UInt64) الصحيحة كنصّ عشريّ لا-موقَّع (نمط بتّاتها int64 مُعاد تفسيره
        //      uint64). Byte دائمًا موجب [0،255] فطباعته الموقَّعة صحيحة أصلًا. سائر الوسائط
        //      تُمرَّر كما هي (نفس المؤشّر). النوع الساكن من ctx.argType (resolveStaticType
        //      عند موقع النداء). يُطابق نظيرَه في المترجم (%llu في I64_TO_STRING).
        // (EN) [طبيعي64 layer — Step 4] Produce a copy of print/convert args with طبيعي64
        //      (UInt64) integer values rendered as unsigned decimal (int64 bits reinterpreted as
        //      uint64). Byte is always in [0,255] so its signed print is already correct. Other
        //      args pass through (same pointer). Static type from ctx.argType (resolveStaticType
        //      at the call site). Mirrors the compiler (%llu in I64_TO_STRING).
        static std::vector<std::shared_ptr<Data::Value>>
        renderUnsignedArgs(const Sad::Interpreter::BuiltinContext &ctx)
        {
            const auto &args = ctx.args();
            std::vector<std::shared_ptr<Data::Value>> out;
            out.reserve(args.size());
            for (std::size_t i = 0; i < args.size(); ++i)
            {
                const auto &a = args[i];
                if (a && ctx.argType(i) == Sad::Types::SadTypeKind::UInt64 && a->isInteger())
                {
                    const uint64_t u = static_cast<uint64_t>(a->toInt64());
                    out.push_back(std::make_shared<Data::Value>(std::to_string(u)));
                }
                else
                {
                    out.push_back(a);
                }
            }
            return out;
        }

        void registerCoreIOBuiltins(Interpreter &interpreter)
        {
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال الإخراج / (EN) Output Functions
            // ═════════════════════════════════════════════════════════════════

            auto print_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                return BuiltinFunctions::print(renderUnsignedArgs(ctx));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::PRINT), print_func);
            // (AR) اطبع_سطر — طباعة قيمة مع سطر جديد
            // (EN) println — print value with newline
            auto println_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                return BuiltinFunctions::println(renderUnsignedArgs(ctx));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::PRINTLN), println_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال الإدخال / (EN) Input Functions
            // ═════════════════════════════════════════════════════════════════

            auto input_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::input(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::READ), input_func);

            // ═════════════════════════════════════════════════════════════════
            // (AR) Branch Hints — تلميحات لتوقع الفروع (تحسين أداء)
            //      في المفسر: لا تأثير — تُرجع الشرط كما هو
            //      في المترجم: تُترجم إلى llvm.expect.i1 لتوجيه المعالج
            // (EN) Branch hints — predict branches (perf optimization)
            //      In interpreter: no-op, returns condition unchanged
            //      In compiler: lowered to llvm.expect.i1 for CPU branch prediction
            // ═════════════════════════════════════════════════════════════════
            auto expect_true_func = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) متوقع(شرط) → يُرجع الشرط (no-op في المفسر)
                if (args.empty())
                    return std::make_shared<Data::Value>(false);
                return args[0];
            };
            // (AR) متوقع/غير_متوقع — تلميحات فروع (Branch hints) — غير مُدرجَتَين في YAML بعد
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::EXPECT), expect_true_func);

            auto expect_false_func = [](Sad::Interpreter::BuiltinContext &ctx)
                -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                // (AR) غير_متوقع(شرط) → يُرجع الشرط (no-op في المفسر)
                if (args.empty())
                    return std::make_shared<Data::Value>(false);
                return args[0];
            };
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::EXPECT_FALSE), expect_false_func);

            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال الطول الأساسية / (EN) Basic Length Functions
            // ═════════════════════════════════════════════════════════════════

            auto len_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::length(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::LENGTH), len_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال النوع / (EN) Type Functions
            // ═════════════════════════════════════════════════════════════════

            auto type_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::type_of(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::TYPE), type_func);
            // ═════════════════════════════════════════════════════════════════
            // (AR) تحويل الأنواع الأساسية / (EN) Basic Type Conversion
            // ═════════════════════════════════════════════════════════════════

            // نص / str / to_string
            auto to_string_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                return BuiltinFunctions::to_string(renderUnsignedArgs(ctx));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::TypeCtor::TO_STRING), to_string_func);

            // رقم / int / to_int
            auto to_int_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::to_int(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::TypeCtor::TO_INT), to_int_func);
            // عشري / float / to_float
            auto to_float_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::to_float(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::TypeCtor::TO_FLOAT), to_float_func);

            // منطقي / bool / to_bool
            auto to_bool_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toBool(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::TypeCtor::TO_BOOL), to_bool_func);

            // ═════════════════════════════════════════════════════════════════
            // (AR) دوال القنوات والتزامن / (EN) Channel & Concurrency Functions
            // ═════════════════════════════════════════════════════════════════

            // (AR) قناة(حجم) — إنشاء قناة اتصال جديدة
            // (EN) channel(size) — create a new communication channel
            auto channel_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
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
                auto obj = new Data::ObjectInstance(nullptr, channelId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x82\xD9\x86\xD8\xA7\xD8\xA9__")); // __قناة__
                obj->fields["__channel_id__"] = Data::Value(static_cast<int>(channelId));

                return std::make_shared<Data::Value>(obj);
            };

            // (AR) تسجيل بالعربية والإنجليزية
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Concurrency::CHANNEL), channel_func);

            // (AR) انتظر_الكل() — انتظار جميع المهام المتزامنة (goroutines)
            // (EN) waitAll() — wait for all concurrent goroutines to complete
            auto wait_all_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args; // (AR) لا تتطلب وسائط
                GoroutineManager::getInstance().waitAll();
                return std::make_shared<Data::Value>(); // void
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Concurrency::WAIT_ALL), wait_all_func);

            // (AR) عدد_المهام() — عدد المهام المتزامنة النشطة
            // (EN) activeGoroutines() — number of active goroutines
            auto active_count_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                int count = static_cast<int>(GoroutineManager::getInstance().activeCount());
                return std::make_shared<Data::Value>(count);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Concurrency::TASK_COUNT), active_count_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) مجموعة_انتظار() — إنشاء WaitGroup جديدة
            // (EN) waitGroup() — create a new WaitGroup
            // ═══════════════════════════════════════════════════════════════
            auto waitgroup_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto wg = std::make_shared<SadWaitGroup>();
                auto &registry = SadWaitGroupRegistry::getInstance();
                size_t wgId = registry.registerWaitGroup(wg);

                // (AR) إنشاء كائن ObjectInstance يمثل مجموعة الانتظار
                auto obj = new Data::ObjectInstance(nullptr, wgId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x85\xD8\xAC\xD9\x85\xD9\x88\xD8\xB9\xD8\xA9_\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xA7\xD8\xB1__")); // __مجموعة_انتظار__
                obj->fields["__waitgroup_id__"] = Data::Value(static_cast<int>(wgId));

                return std::make_shared<Data::Value>(obj);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Concurrency::WAIT_GROUP), waitgroup_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) قفل() — إنشاء Mutex جديد
            // (EN) mutex() — create a new Mutex
            // ═══════════════════════════════════════════════════════════════
            auto mutex_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto mtx = std::make_shared<SadMutex>();
                auto &registry = SadMutexRegistry::getInstance();
                size_t mtxId = registry.registerMutex(mtx);

                auto obj = new Data::ObjectInstance(nullptr, mtxId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x82\xD9\x81\xD9\x84__")); // __قفل__
                obj->fields["__mutex_id__"] = Data::Value(static_cast<int>(mtxId));

                return std::make_shared<Data::Value>(obj);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Concurrency::MUTEX), mutex_func);

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
            auto future_func = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;

                // (AR) إنشاء المستقبل / (EN) Create the future
                auto fut = std::make_shared<SadFuture>();

                // (AR) تسجيل المستقبل في السجل العام / (EN) Register future in global registry
                auto &registry = SadFutureRegistry::getInstance();
                size_t futureId = registry.registerFuture(fut);

                // (AR) إنشاء كائن ObjectInstance يمثل المستقبل / (EN) Create ObjectInstance representing the future
                auto obj = new Data::ObjectInstance(nullptr, futureId);
                obj->fields["__class__"] = Data::Value(std::string("__\xD9\x85\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84__")); // __مستقبل__
                obj->fields["__future_id__"] = Data::Value(static_cast<int>(futureId));

                // (AR) [S-TS-P4] وسم القيمة بنوع Future الموحَّد: getKind()=Future فيُرجع
                //      نوع()=«مستقبل»، مع إبقاء type_=OBJECT (setSadType) ليعمل فحص الطرق.
                // (EN) [S-TS-P4] Tag with the unified Future type so نوع()=«مستقبل» while
                //      keeping type_=OBJECT (method dispatch intact).
                auto futVal = std::make_shared<Data::Value>(obj);
                futVal->setSadType(Types::SadTypeRegistry::instance().makeFuture());
                return futVal;
            };

            // (AR) تسجيل بالعربية والإنجليزية / (EN) Register in Arabic and English
            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Concurrency::FUTURE), future_func);
        }

    } // namespace Interpreter
} // namespace Sad
