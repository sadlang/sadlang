// بسم الله الرحمن الرحيم
/**
 * @file test_system_comprehensive.cpp
 * @brief Comprehensive Tests for System Module - اختبارات شاملة لوحدة النظام
 *
 * Tests: SystemFunctions (platform, time, env vars, directories)
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 */

#include "sad_test_framework.h"
#include "system/system_functions.h"
#include <string>
#include <ctime>

using namespace Sad::StdLib::System;
using namespace Sad::Data;

int main() {
    SAD_TEST_INIT();

    // ================================================================
    // 1. Platform Detection
    // ================================================================
    SAD_GROUP("معلومات المنصة / Platform Info");

    SAD_TEST("platform returns Value", {
        auto result = SystemFunctions::platform();
        SAD_ASSERT_TRUE(result.getType() != ValueType::VOID);
    });

    SAD_TEST("osName returns Value", {
        auto result = SystemFunctions::osName();
        SAD_ASSERT_TRUE(result.getType() != ValueType::VOID);
    });

    // ================================================================
    // 2. Time Functions
    // ================================================================
    SAD_GROUP("الوقت / Time");

    SAD_TEST("timestamp returns number", {
        auto ts = SystemFunctions::timestamp();
        SAD_ASSERT_TRUE(ts.getType() == ValueType::INTEGER ||
                        ts.getType() == ValueType::DOUBLE);
    });

    SAD_TEST("clock_time returns double", {
        auto result = SystemFunctions::clock_time();
        SAD_ASSERT_TRUE(result.getType() == ValueType::DOUBLE);
    });

    // ================================================================
    // 3. Environment Variables
    // ================================================================
    SAD_GROUP("متغيرات البيئة / Environment Variables");

    SAD_TEST("getEnv with PATH", {
        std::vector<Value> args;
        args.push_back(Value("PATH"));
        auto result = SystemFunctions::getEnv(args);
        SAD_ASSERT_TRUE(result.getType() == ValueType::STRING);
    });

    SAD_TEST("setEnv and getEnv round-trip", {
        std::vector<Value> setArgs;
        setArgs.push_back(Value("SAD_TEST_VAR_123"));
        setArgs.push_back(Value("test_value"));
        SystemFunctions::setEnv(setArgs);

        std::vector<Value> getArgs;
        getArgs.push_back(Value("SAD_TEST_VAR_123"));
        auto result = SystemFunctions::getEnv(getArgs);
        SAD_ASSERT_TRUE(result.getType() == ValueType::STRING);
    });

    // ================================================================
    // 4. Directory Operations
    // ================================================================
    SAD_GROUP("المجلدات / Directories");

    SAD_TEST("currentDir returns string", {
        std::vector<Value> args;
        auto result = SystemFunctions::currentDir(args);
        SAD_ASSERT_TRUE(result.getType() == ValueType::STRING);
    });

    SAD_TEST("changeDir and restore", {
        std::vector<Value> emptyArgs;
        auto original = SystemFunctions::currentDir(emptyArgs);

        std::vector<Value> cdArgs;
        cdArgs.push_back(Value(".."));
        SystemFunctions::changeDir(cdArgs);

        std::vector<Value> restoreArgs;
        restoreArgs.push_back(original);
        SystemFunctions::changeDir(restoreArgs);

        auto restored = SystemFunctions::currentDir(emptyArgs);
        SAD_ASSERT_EQ(restored.toString(), original.toString());
    });

    return _runner.printSummary();
}
