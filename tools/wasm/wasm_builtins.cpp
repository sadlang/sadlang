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

namespace Sad
{
    namespace Interpreter
    {

        using Data::Value;
        using VV = std::vector<std::shared_ptr<Value>>;

        // ─── builtin_module_strings: نصوص + مصفوفات ──────────────────────────────────
        void registerBuiltinsStrings(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction("اطبع",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::print(a); });
            fm.registerBuiltinFunction("print",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::print(a); });

            fm.registerBuiltinFunction("اطبع_سطر",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::println(a); });
            fm.registerBuiltinFunction("println",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::println(a); });

            fm.registerBuiltinFunction("اقرأ",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::input(a); });
            fm.registerBuiltinFunction("input",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::input(a); });

            fm.registerBuiltinFunction("طول",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::length(a); });
            fm.registerBuiltinFunction("length",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::length(a); });

            fm.registerBuiltinFunction("أضف",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::append(a); });
            fm.registerBuiltinFunction("append",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::append(a); });

            fm.registerBuiltinFunction("أزل",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::remove(a); });
            fm.registerBuiltinFunction("remove",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::remove(a); });
        }

        // ─── builtin_module_basics: أنواع + خروج + مدى ──────────────────────────────
        void registerBuiltinsBasics(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction("رقم",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_int(a); });
            fm.registerBuiltinFunction("int",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_int(a); });

            fm.registerBuiltinFunction("عشري",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_float(a); });
            fm.registerBuiltinFunction("float",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_float(a); });

            fm.registerBuiltinFunction("نص",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_string(a); });
            fm.registerBuiltinFunction("str",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::to_string(a); });

            fm.registerBuiltinFunction("نوع",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::type_of(a); });
            fm.registerBuiltinFunction("type",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::type_of(a); });

            fm.registerBuiltinFunction("مدى",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::range(a); });
            fm.registerBuiltinFunction("range",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::range(a); });
        }

        // ─── builtin_module_math: رياضيات ────────────────────────────────────────────
        void registerBuiltinsMath(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction("جذر",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::sqrt(a); });
            fm.registerBuiltinFunction("sqrt",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::sqrt(a); });

            fm.registerBuiltinFunction("مطلق",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::abs(a); });
            fm.registerBuiltinFunction("abs",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::abs(a); });

            fm.registerBuiltinFunction("أكبر",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::max(a); });
            fm.registerBuiltinFunction("max",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::max(a); });

            fm.registerBuiltinFunction("أصغر",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::min(a); });
            fm.registerBuiltinFunction("min",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::min(a); });

            fm.registerBuiltinFunction("جمع",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::sum(a); });
            fm.registerBuiltinFunction("sum",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::sum(a); });
        }

        // ─── builtin_module_assertions: تأكيدات (غير متاح في WASM) ──────────────────
        void registerBuiltinsAssertions(Interpreter &) {}

        // ─── builtin_module_async: نصوص (WASM) ──────────────────────────────────────
        void registerBuiltinsAsync(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            fm.registerBuiltinFunction("قسم",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::split(a); });
            fm.registerBuiltinFunction("split",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::split(a); });

            fm.registerBuiltinFunction("صل",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::join(a); });
            fm.registerBuiltinFunction("join",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::join(a); });

            fm.registerBuiltinFunction("أحرف_كبيرة",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::upper(a); });
            fm.registerBuiltinFunction("upper",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::upper(a); });

            fm.registerBuiltinFunction("أحرف_صغيرة",
                                       [](const VV &a)
                                       { return StdLib::BuiltinFunctions::lower(a); });
            fm.registerBuiltinFunction("lower",
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
