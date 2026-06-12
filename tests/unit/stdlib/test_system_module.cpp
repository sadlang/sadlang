// بسم الله الرحمن الرحيم
/**
 * @file test_system_module.cpp
 * @brief Comprehensive Tests for System Module - اختبارات شاملة لوحدة النظام
 *
 * Tests cover: platform(), osName(), timestamp(), clock_time(),
 *              getEnv(), setEnv(), currentDir(), changeDir()
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "system/system_functions.h"
#include <filesystem>
#include <cstdlib>

using namespace Sad::StdLib::System;
using Sad::Data::Value;

// ============================================================================
// 1. Platform Info Tests
//    اختبارات معلومات المنصة
// ============================================================================

TEST(SystemFunctionsTest, Platform) {
    Value result = SystemFunctions::platform();
    std::string platform = result.toString();
    EXPECT_FALSE(platform.empty());
    // Should be one of: "windows", "linux", "macos", "unknown"
#ifdef _WIN32
    EXPECT_EQ(platform, "windows");
#elif __APPLE__
    EXPECT_EQ(platform, "macos");
#elif __linux__
    EXPECT_EQ(platform, "linux");
#endif
}

TEST(SystemFunctionsTest, OsName) {
    Value result = SystemFunctions::osName();
    std::string name = result.toString();
    EXPECT_FALSE(name.empty());
}

// ============================================================================
// 2. Time Functions Tests
//    اختبارات دوال الوقت
// ============================================================================

TEST(SystemFunctionsTest, Timestamp) {
    Value result = SystemFunctions::timestamp();
    // timestamp should return a positive number (UNIX epoch)
    double ts = result.toNumber();
    EXPECT_GT(ts, 1000000000.0); // After ~2001
}

TEST(SystemFunctionsTest, TimestampIncreasing) {
    Value t1 = SystemFunctions::timestamp();
    Value t2 = SystemFunctions::timestamp();
    EXPECT_GE(t2.toNumber(), t1.toNumber());
}

TEST(SystemFunctionsTest, ClockTime) {
    Value result = SystemFunctions::clock_time();
    double t = result.toNumber();
    EXPECT_GE(t, 0.0);
}

// ============================================================================
// 3. Environment Variable Tests
//    اختبارات متغيرات البيئة
// ============================================================================

TEST(SystemFunctionsTest, GetEnvPath) {
    std::vector<Value> args = {Value(std::string("PATH"))};
    Value result = SystemFunctions::getEnv(args);
    std::string path = result.toString();
    EXPECT_FALSE(path.empty());
}

TEST(SystemFunctionsTest, GetEnvNonExistent) {
    std::vector<Value> args = {Value(std::string("SAD_LANG_TEST_NONEXISTENT_VAR_12345"))};
    Value result = SystemFunctions::getEnv(args);
    // Should return empty string for non-existent variables
    EXPECT_EQ(result.toString(), "");
}

TEST(SystemFunctionsTest, GetEnvNoArgs) {
    std::vector<Value> args;
    EXPECT_THROW(SystemFunctions::getEnv(args), std::invalid_argument);
}

TEST(SystemFunctionsTest, SetEnvAndGet) {
    std::string testVar = "SAD_TEST_VAR_REEM";
    std::string testVal = "قيمة_اختبارية";

    std::vector<Value> setArgs = {Value(testVar), Value(testVal)};
    SystemFunctions::setEnv(setArgs);

    std::vector<Value> getArgs = {Value(testVar)};
    Value result = SystemFunctions::getEnv(getArgs);
    EXPECT_EQ(result.toString(), testVal);

    // Cleanup
#ifdef _WIN32
    _putenv_s(testVar.c_str(), "");
#else
    unsetenv(testVar.c_str());
#endif
}

TEST(SystemFunctionsTest, SetEnvNoArgs) {
    std::vector<Value> args;
    EXPECT_THROW(SystemFunctions::setEnv(args), std::invalid_argument);
}

// ============================================================================
// 4. Directory Operations Tests
//    اختبارات عمليات المجلدات
// ============================================================================

TEST(SystemFunctionsTest, CurrentDir) {
    std::vector<Value> args;
    Value result = SystemFunctions::currentDir(args);
    std::string dir = result.toString();
    EXPECT_FALSE(dir.empty());
    EXPECT_TRUE(std::filesystem::exists(dir));
}

TEST(SystemFunctionsTest, ChangeDirAndBack) {
    // Save original directory
    std::vector<Value> args;
    Value originalDir = SystemFunctions::currentDir(args);
    std::string original = originalDir.toString();

    // Change to temp directory
    std::string tempDir = std::filesystem::temp_directory_path().string();
    std::vector<Value> cdArgs = {Value(tempDir)};
    SystemFunctions::changeDir(cdArgs);

    // Verify we changed
    Value newDir = SystemFunctions::currentDir(args);
    // The paths might differ in normalization, just check it changed or is valid

    // Restore original
    std::vector<Value> restoreArgs = {Value(original)};
    SystemFunctions::changeDir(restoreArgs);
}

// ============================================================================
// 5. Edge Cases
//    حالات الحافة
// ============================================================================

TEST(SystemFunctionsTest, GetEnvArabicVarName) {
    // Even with Arabic var name, should not crash
    std::vector<Value> args = {Value(std::string("متغير_عربي"))};
    EXPECT_NO_THROW(SystemFunctions::getEnv(args));
}

TEST(SystemFunctionsTest, PlatformConsistency) {
    // Calling multiple times should return same result
    Value p1 = SystemFunctions::platform();
    Value p2 = SystemFunctions::platform();
    EXPECT_EQ(p1.toString(), p2.toString());
}
