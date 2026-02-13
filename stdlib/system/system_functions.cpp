/**
 * @file system_functions.cpp
 * @brief Implementation of System Functions
 * @date February 4, 2026
 */

#include "system_functions.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <filesystem>
#include <stdexcept>

using namespace Sad::Data;

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Sad {
namespace StdLib {
namespace System {

// Get environment variable / الحصول على متغير بيئة
Value SystemFunctions::getEnv(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("getEnv requires variable name");
    }
    
    std::string varName = args[0].toString();
    const char* value = std::getenv(varName.c_str());
    
    if (value == nullptr) {
        return Value("");  // Return empty string if not found
    }
    
    return Value(std::string(value));
}

// Set environment variable / تعيين متغير بيئة
Value SystemFunctions::setEnv(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::invalid_argument("setEnv requires name and value");
    }
    
    std::string varName = args[0].toString();
    std::string varValue = args[1].toString();
    
#ifdef _WIN32
    _putenv_s(varName.c_str(), varValue.c_str());
#else
    setenv(varName.c_str(), varValue.c_str(), 1);
#endif
    
    return Value();  // void return
}

// Get current directory / الحصول على المجلد الحالي
Value SystemFunctions::currentDir(const std::vector<Value>& args) {
    try {
        std::filesystem::path current = std::filesystem::current_path();
        return Value(current.string());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("currentDir failed: ") + e.what());
    }
}

// Change directory / تغيير المجلد
Value SystemFunctions::changeDir(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("changeDir requires directory path");
    }
    
    std::string path = args[0].toString();
    
    try {
        std::filesystem::current_path(path);
        return Value(true);
    } catch (const std::exception& e) {
        return Value(false);
    }
}

// Execute command / تنفيذ أمر
Value SystemFunctions::execute(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("execute requires command");
    }
    
    std::string command = args[0].toString();
    int exitCode = std::system(command.c_str());
    
    return Value(exitCode);
}

// System call (alias for execute)
Value SystemFunctions::system_call(const std::vector<Value>& args) {
    return execute(args);
}

// Get platform name / اسم المنصة
Value SystemFunctions::platform() {
#ifdef _WIN32
    return Value("Windows");
#elif __linux__
    return Value("Linux");
#elif __APPLE__
    return Value("macOS");
#else
    return Value("Unknown");
#endif
}

// Get OS name (same as platform)
Value SystemFunctions::osName() {
    return platform();
}

// Get current timestamp / الطابع الزمني
Value SystemFunctions::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    return Value(static_cast<int>(seconds));
}

// Get clock time / وقت الساعة
Value SystemFunctions::clock_time() {
    clock_t t = clock();
    double seconds = static_cast<double>(t) / CLOCKS_PER_SEC;
    
    return Value(seconds);
}

} // namespace System
} // namespace StdLib
} // namespace Sad
