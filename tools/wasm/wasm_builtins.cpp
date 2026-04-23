/**
 * @file wasm_builtins.cpp
 * @brief (AR) إعادة تنفيذ آمنة لـ WASM لجميع ملفات تسجيل الدوال المضمنة
 * @brief (EN) WASM-safe reimplementation of all builtin registry files
 *
 * (AR) يستخدم فقط الدوال الموجودة في stdlib/core/builtins.h.
 *      الوحدات الخاصة بالمنصة (SDL2, OS منخفض المستوى, USB) هي stubs فارغة.
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"

namespace Sad
{
    namespace Interpreter
    {

        using Data::Value;
        using VV = std::vector<std::shared_ptr<Value>>;

        // (AR) اختصارات لفضاءات الأسماء المركزية
        namespace Bc = Sad::Builtins::Names::Core;
        namespace Bt = Sad::Builtins::Names::TypeCtor;
        namespace Bm = Sad::Builtins::Names::Math;
        namespace Bs = Sad::Builtins::Names::Strings;
        namespace Bb = Sad::Builtins::Names::Basics;

        // ─── builtin_module_strings: نصوص + مصفوفات ──────────────────────────────────
        void registerBuiltinsStrings(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction(std::string(Bc::PRINT),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::print(a); });

            fm.registerBuiltinFunction(std::string(Bc::PRINTLN),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::println(a); });

            fm.registerBuiltinFunction(std::string(Bc::READ),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::input(a); });

            fm.registerBuiltinFunction(std::string(Bc::LENGTH),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::length(a); });

            fm.registerBuiltinFunction(std::string(Bs::APPEND),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::append(a); });

            fm.registerBuiltinFunction(std::string(Bs::REMOVE_ELEM),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::remove(a); });
        }

        // ─── builtin_module_basics: أنواع + خروج + مدى ──────────────────────────────
        void registerBuiltinsBasics(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction(std::string(Bt::TO_INT),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_int(a); });

            fm.registerBuiltinFunction(std::string(Bt::TO_FLOAT),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_float(a); });

            fm.registerBuiltinFunction(std::string(Bt::TO_STRING),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_string(a); });

            fm.registerBuiltinFunction(std::string(Bc::TYPE),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::type_of(a); });

            fm.registerBuiltinFunction(std::string(Bb::RANGE),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::range(a); });
        }

        // ─── builtin_module_math: رياضيات ────────────────────────────────────────────
        void registerBuiltinsMath(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction(std::string(Bm::SQRT),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::sqrt(a); });

            fm.registerBuiltinFunction(std::string(Bm::ABS),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::abs(a); });

            fm.registerBuiltinFunction(std::string(Bm::MAX),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::max(a); });

            fm.registerBuiltinFunction(std::string(Bm::MIN),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::min(a); });

            fm.registerBuiltinFunction(std::string(Bm::SUM),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::sum(a); });
        }

        // ─── builtin_module_assertions: تأكيدات (غير متاح في WASM) ──────────────────
        void registerBuiltinsAssertions(Interpreter &) {}

        // ─── builtin_module_async: نصوص (WASM) ──────────────────────────────────────
        void registerBuiltinsAsync(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction(std::string(Bs::SPLIT),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::split(a); });

            fm.registerBuiltinFunction(std::string(Bs::JOIN),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::join(a); });

            fm.registerBuiltinFunction(std::string(Bs::TO_UPPER),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::upper(a); });

            fm.registerBuiltinFunction(std::string(Bs::TO_LOWER),
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::lower(a); });
        }

        // ─── builtin_module_maps: خرائط ──────────────────────────────────────────────
        void registerBuiltinsMaps(Interpreter &) {}

        // ─── وحدات النواة — غير متاحة في WASM ─────────────────────────────────────────
        void registerBuiltinsKernelCPU(Interpreter &) {}
        void registerBuiltinsKernelUEFI(Interpreter &) {}
        void registerBuiltinsKernelACPI(Interpreter &) {}
        void registerBuiltinsKernelGPU(Interpreter &) {}
        void registerBuiltinsKernelUSB(Interpreter &) {}
        void registerBuiltinsKernelStorage(Interpreter &) {}
        void registerBuiltinsKernelNetwork(Interpreter &) {}
        void registerBuiltinsKernelAudio(Interpreter &) {}
        void registerBuiltinsKernelTimers(Interpreter &) {}
        void registerBuiltinsKernelSerial(Interpreter &) {}
        void registerBuiltinsKernelMemory(Interpreter &) {}
        void registerBuiltinsKernelThreads(Interpreter &) {}
        void registerBuiltinsFFI(Interpreter &) {}
        void registerBuiltinsExceptions(Interpreter &) {}
        void registerBuiltinsPlatform(Interpreter &) {}
        void registerBuiltinsSockets(Interpreter &) {}

    } // namespace Interpreter
} // namespace Sad
