/**
 * @file system_functions.h
 * @brief System and Environment Functions for Sad Language
 * @date February 4, 2026
 */

#ifndef SAD_SYSTEM_FUNCTIONS_H
#define SAD_SYSTEM_FUNCTIONS_H

#include "../../shared/types/include/value.h"
#include <vector>
#include <string>

namespace Sad {
namespace StdLib {
namespace System {

class SystemFunctions {
public:
    // Environment variables / متغيرات البيئة
    static Data::Value getEnv(const std::vector<Data::Value>& args);
    static Data::Value setEnv(const std::vector<Data::Value>& args);
    
    // Directory operations / عمليات المجلدات
    static Data::Value currentDir(const std::vector<Data::Value>& args);
    static Data::Value changeDir(const std::vector<Data::Value>& args);
    
    // Command execution / تنفيذ الأوامر
    static Data::Value execute(const std::vector<Data::Value>& args);
    static Data::Value system_call(const std::vector<Data::Value>& args);
    
    // Platform info / معلومات النظام
    static Data::Value platform();
    static Data::Value osName();
    
    // Time functions / دوال الوقت
    static Data::Value timestamp();
    static Data::Value clock_time();
};

} // namespace System
} // namespace StdLib
} // namespace Sad

#endif // SAD_SYSTEM_FUNCTIONS_H
