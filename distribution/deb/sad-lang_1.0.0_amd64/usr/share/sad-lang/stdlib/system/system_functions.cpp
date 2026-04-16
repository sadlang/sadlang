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
#include <algorithm>
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

// ════════════════════════════════════════════════════════════════════════════
// Execute command with security validation / تنفيذ أمر مع التحقق الأمني
// ════════════════════════════════════════════════════════════════════════════

namespace {
    // قائمة الأحرف الخطرة / Dangerous characters list
    bool containsDangerousChars(const std::string& cmd) {
        // منع أحرف الحقن / Block injection characters
        static const std::string dangerous = ";|&$`\\\"'<>(){}[]!#*?~\n\r";
        for (char c : cmd) {
            if (dangerous.find(c) != std::string::npos) {
                return true;
            }
        }
        // منع أنماط خطرة / Block dangerous patterns
        if (cmd.find("..") != std::string::npos) return true;
        if (cmd.find("//") != std::string::npos) return true;
        return false;
    }
    
    // قائمة الأوامر المسموحة / Allowed commands whitelist
    std::vector<std::string> getAllowedCommands() {
        return {
            "ls", "dir", "echo", "pwd", "cd", "cat", "type",
            "mkdir", "rmdir", "copy", "cp", "mv", "move",
            "rm", "del", "touch", "date", "time", "whoami",
            "hostname", "ping", "curl", "wget", "git", "cmake",
            "make", "ninja", "python", "python3", "node", "npm"
        };
    }
    
    // التحقق من أن الأمر مسموح / Check if command is allowed
    bool isCommandAllowed(const std::string& cmd) {
        auto allowed = getAllowedCommands();
        
        // استخراج اسم الأمر الأول / Extract first command name
        std::string firstWord;
        size_t space = cmd.find(' ');
        if (space != std::string::npos) {
            firstWord = cmd.substr(0, space);
        } else {
            firstWord = cmd;
        }
        
        // تحويل للحروف الصغيرة للمقارنة / Convert to lowercase for comparison
        std::string lowerCmd = firstWord;
        std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
        
        // إزالة مسار الملف إن وجد / Remove path if present
        size_t lastSlash = lowerCmd.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            lowerCmd = lowerCmd.substr(lastSlash + 1);
        }
        
        // إزالة .exe إن وجد / Remove .exe if present
        if (lowerCmd.size() > 4 && lowerCmd.substr(lowerCmd.size() - 4) == ".exe") {
            lowerCmd = lowerCmd.substr(0, lowerCmd.size() - 4);
        }
        
        for (const auto& allowed_cmd : allowed) {
            if (lowerCmd == allowed_cmd) {
                return true;
            }
        }
        return false;
    }
}

Value SystemFunctions::execute(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::invalid_argument("execute requires command / يتطلب أمر");
    }
    
    std::string command = args[0].toString();
    
    // ════════════════════════════════════════════════════════
    // التحقق الأمني / Security validation
    // ════════════════════════════════════════════════════════
    
    // 1. رفض الأوامر الفارغة / Reject empty commands
    if (command.empty() || command.find_first_not_of(" \t\n\r") == std::string::npos) {
        throw std::invalid_argument("Empty command / أمر فارغ");
    }
    
    // 2. فحص الأحرف الخطرة / Check dangerous characters
    if (containsDangerousChars(command)) {
        throw std::invalid_argument("Command contains dangerous characters / الأمر يحتوي على أحرف خطرة");
    }
    
    // 3. التحقق من القائمة البيضاء / Check whitelist
    if (!isCommandAllowed(command)) {
        throw std::invalid_argument("Command not in allowed list / الأمر ليس في القائمة المسموحة");
    }
    
    // 4. تحديد طول الأمر / Limit command length
    if (command.length() > 4096) {
        throw std::invalid_argument("Command too long / الأمر طويل جداً");
    }
    
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
    
    // (AR) إرجاع كعشري لتجنب طفح العدد الصحيح 32-بت (Y2038)
    // (EN) Return as double to avoid 32-bit integer overflow (Y2038)
    return Value(static_cast<double>(seconds));
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
