/**
 * @file builtin_module_ffi.cpp
 * @brief (AR) وحدة FFI — واجهة الدوال الخارجية C/C++
 * @brief (EN) FFI module — Foreign Function Interface for C/C++
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "value.h"
#include "builtin_registry.h"
namespace Bffi = Sad::Builtins::Names::FFI;

#include <sstream>
#include <string>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace Sad {
namespace Interpreter {

// Self-contained FFI stub using platform-native library loading
namespace {
    struct StubLibrary {
        std::string path;
#ifdef _WIN32
        HMODULE handle = nullptr;
#else
        void* handle = nullptr;
#endif
        bool loaded = false;

        StubLibrary(const std::string& p) : path(p) {
#ifdef _WIN32
            handle = LoadLibraryA(p.c_str());
            loaded = (handle != nullptr);
#else
            handle = dlopen(p.c_str(), RTLD_LAZY);
            loaded = (handle != nullptr);
#endif
        }

        ~StubLibrary() {
#ifdef _WIN32
            if (handle) FreeLibrary(handle);
#else
            if (handle) dlclose(handle);
#endif
        }

        bool isLoaded() const { return loaded; }
        const std::string& getPath() const { return path; }

        bool hasSymbol(const std::string& name) const {
            if (!handle) return false;
#ifdef _WIN32
            return GetProcAddress(handle, name.c_str()) != nullptr;
#else
            return dlsym(handle, name.c_str()) != nullptr;
#endif
        }

        void* getSymbol(const std::string& name) const {
            if (!handle) return nullptr;
#ifdef _WIN32
            return reinterpret_cast<void*>(GetProcAddress(handle, name.c_str()));
#else
            return dlsym(handle, name.c_str());
#endif
        }
    };

    static std::unordered_map<uint64_t, std::shared_ptr<StubLibrary>> g_libraries;
    static uint64_t g_nextLibId = 1;
}

void registerBuiltinsFFI(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // (1) مكتبة_حمل / ffi_load
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(-1.0);
            std::string path = args[0]->toString();
            try {
                auto lib = std::make_shared<StubLibrary>(path);
                if (!lib->isLoaded()) return std::make_shared<Data::Value>(-1.0);
                uint64_t id = g_nextLibId++;
                g_libraries[id] = lib;
                return std::make_shared<Data::Value>(static_cast<double>(id));
            } catch (...) {
                return std::make_shared<Data::Value>(-1.0);
            }
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_LOAD), f); // مكتبة_حمل
    }

    // (2) مكتبة_أفرغ / ffi_unload
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            uint64_t id = static_cast<uint64_t>(args[0]->toDouble());
            return std::make_shared<Data::Value>(g_libraries.erase(id) > 0);
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_FREE), f); // مكتبة_أفرغ
    }

    // (3) مكتبة_دالة / ffi_get_func
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(false);
            uint64_t libId = static_cast<uint64_t>(args[0]->toDouble());
            std::string funcName = args[1]->toString();
            auto it = g_libraries.find(libId);
            if (it == g_libraries.end()) return std::make_shared<Data::Value>(false);
            bool hasFunc = it->second->hasSymbol(funcName);
            return std::make_shared<Data::Value>(hasFunc);
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_FUNC), f); // مكتبة_دالة
    }

    // (4) مكتبة_استدع / ffi_call
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(false);
            uint64_t libId = static_cast<uint64_t>(args[0]->toDouble());
            std::string funcName = args[1]->toString();
            auto it = g_libraries.find(libId);
            if (it == g_libraries.end())
                return std::make_shared<Data::Value>(std::string("خطأ: مكتبة غير موجودة"));
            if (!it->second->hasSymbol(funcName))
                return std::make_shared<Data::Value>(std::string("خطأ: دالة غير موجودة: " + funcName));
            auto* rawFunc = reinterpret_cast<void(*)()>(it->second->getSymbol(funcName));
            if (rawFunc) {
                rawFunc();
                return std::make_shared<Data::Value>(true);
            }
            return std::make_shared<Data::Value>(false);
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_CALL), f); // مكتبة_استدع
    }

    // (5) مكتبة_استدع_رقم / ffi_call_int
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(-1.0);
            uint64_t libId = static_cast<uint64_t>(args[0]->toDouble());
            std::string funcName = args[1]->toString();
            auto it = g_libraries.find(libId);
            if (it == g_libraries.end()) return std::make_shared<Data::Value>(-1.0);
            auto* rawFunc = reinterpret_cast<int(*)()>(it->second->getSymbol(funcName));
            if (rawFunc) {
                int result = rawFunc();
                return std::make_shared<Data::Value>(static_cast<double>(result));
            }
            return std::make_shared<Data::Value>(-1.0);
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_CALL_INT), f); // مكتبة_استدع_رقم
    }

    // (6) مكتبة_استدع_نص / ffi_call_string
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(std::string(""));
            uint64_t libId = static_cast<uint64_t>(args[0]->toDouble());
            std::string funcName = args[1]->toString();
            auto it = g_libraries.find(libId);
            if (it == g_libraries.end()) return std::make_shared<Data::Value>(std::string(""));
            auto* rawFunc = reinterpret_cast<const char*(*)()>(it->second->getSymbol(funcName));
            if (rawFunc) {
                const char* result = rawFunc();
                return std::make_shared<Data::Value>(std::string(result ? result : ""));
            }
            return std::make_shared<Data::Value>(std::string(""));
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_CALL_STR), f); // مكتبة_استدع_نص
    }

    // (7) مكتبة_رمز / ffi_has_symbol
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(false);
            uint64_t libId = static_cast<uint64_t>(args[0]->toDouble());
            std::string symbol = args[1]->toString();
            auto it = g_libraries.find(libId);
            if (it == g_libraries.end()) return std::make_shared<Data::Value>(false);
            return std::make_shared<Data::Value>(it->second->hasSymbol(symbol));
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_SYMBOL), f); // مكتبة_رمز
    }

    // (8) مكتبة_مسار / ffi_path
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
            uint64_t libId = static_cast<uint64_t>(args[0]->toDouble());
            auto it = g_libraries.find(libId);
            if (it == g_libraries.end()) return std::make_shared<Data::Value>(std::string(""));
            return std::make_shared<Data::Value>(it->second->getPath());
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_PATH), f); // مكتبة_مسار
    }

    // (9) مكتبة_قائمة / ffi_list
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            std::vector<Data::Value> result;
            for (auto& [id, lib] : g_libraries) {
                result.push_back(Data::Value(static_cast<double>(id)));
            }
            return std::make_shared<Data::Value>(result);
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_LIST), f); // مكتبة_قائمة
    }

    // (10) مكتبة_عدد / ffi_count
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(static_cast<double>(g_libraries.size()));
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_COUNT), f); // مكتبة_عدد
    }

    // (11) نوع_حجم / type_size
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(0.0);
            std::string typeName = args[0]->toString();
            size_t size = 0;
            if (typeName == "int" || typeName == "عدد_صحيح") size = sizeof(int);
            else if (typeName == "long" || typeName == "عدد_طويل") size = sizeof(long long);
            else if (typeName == "float" || typeName == "عشري") size = sizeof(float);
            else if (typeName == "double" || typeName == "مضاعف" || typeName == "عشري_مزدوج") size = sizeof(double);
            else if (typeName == "char" || typeName == "حرف") size = sizeof(char);
            else if (typeName == "pointer" || typeName == "مؤشر") size = sizeof(void*);
            else if (typeName == "bool" || typeName == "منطقي") size = sizeof(bool);
            return std::make_shared<Data::Value>(static_cast<double>(size));
        };
        fm.registerBuiltinFunction(std::string(Bffi::TYPE_SIZE), f); // نوع_حجم
    }

    // (12) نوع_محاذاة / type_alignment
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(0.0);
            std::string typeName = args[0]->toString();
            size_t align = 0;
            if (typeName == "int") align = alignof(int);
            else if (typeName == "long") align = alignof(long long);
            else if (typeName == "double") align = alignof(double);
            else if (typeName == "pointer") align = alignof(void*);
            else align = 1;
            return std::make_shared<Data::Value>(static_cast<double>(align));
        };
        fm.registerBuiltinFunction(std::string(Bffi::TYPE_ALIGN), f); // نوع_محاذاة
    }

    // (13) نوع_مؤشر / pointer_size
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(static_cast<double>(sizeof(void*) * 8));
        };
        fm.registerBuiltinFunction(std::string(Bffi::TYPE_PTR), f); // نوع_مؤشر
    }

    // (14) منصة / platform
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
#ifdef _WIN32
            return std::make_shared<Data::Value>(std::string("ويندوز"));
#elif __APPLE__
            return std::make_shared<Data::Value>(std::string("ماك"));
#elif __linux__
            return std::make_shared<Data::Value>(std::string("لينكس"));
#else
            return std::make_shared<Data::Value>(std::string("غير معروف"));
#endif
        };
        fm.registerBuiltinFunction(std::string(Bffi::FFI_PLATFORM), f); // منصة
    }

    // (15) امتداد_مكتبة / lib_extension
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
#ifdef _WIN32
            return std::make_shared<Data::Value>(std::string(".dll"));
#elif __APPLE__
            return std::make_shared<Data::Value>(std::string(".dylib"));
#else
            return std::make_shared<Data::Value>(std::string(".so"));
#endif
        };
        fm.registerBuiltinFunction(std::string(Bffi::LIB_EXTENSION), f); // امتداد_مكتبة
    }
}

} // namespace Interpreter
} // namespace Sad
