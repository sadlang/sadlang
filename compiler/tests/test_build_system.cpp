// بسم الله الرحمن الرحيم
// ============================================================================
// test_build_system.cpp — اختبارات شاملة لنظام البناء
// Comprehensive Build System Tests
// ============================================================================

#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "build/build_system.h"

using namespace Sad::Build;
namespace fs = std::filesystem;

// ============================================================================
struct TestStats {
    int total = 0, passed = 0, failed = 0;
    void record(bool ok) { total++; if (ok) passed++; else failed++; }
    void print() const {
        std::cout << "\n═══════════════════════════════════════════\n"
                  << "  📊 النتائج: " << total << " اختبار | ✅ " << passed
                  << " نجح | ❌ " << failed << " فشل\n"
                  << "═══════════════════════════════════════════\n";
    }
};
static TestStats stats;

#define TEST(name) do { bool _ok = true; std::string _name = name;
#define EXPECT(cond) if (!(cond)) { _ok = false; std::cout << "    ✗ " << #cond << " (سطر " << __LINE__ << ")\n"; }
#define END_TEST stats.record(_ok); std::cout << (_ok ? "  ✓ " : "  ✗ ") << _name << "\n"; } while(0)

// ============================================================================
// مساعدات الاختبار
// ============================================================================

static const std::string TEST_DIR = "test_build_tmp";

static void setupTestDir() {
    fs::create_directories(TEST_DIR);
    fs::create_directories(TEST_DIR + "/src");
}

static void cleanupTestDir() {
    fs::remove_all(TEST_DIR);
}

// ============================================================================
// 1. اختبارات التحويلات الأساسية
// ============================================================================

void testEnumConversions() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات التحويلات الأساسية             ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("تحويل نوع الهدف من العربية") {
        EXPECT(targetTypeFromArabic("تنفيذي") == TargetType::Executable);
        EXPECT(targetTypeFromArabic("exe") == TargetType::Executable);
        EXPECT(targetTypeFromArabic("مكتبة_ساكنة") == TargetType::StaticLib);
        EXPECT(targetTypeFromArabic("static") == TargetType::StaticLib);
        EXPECT(targetTypeFromArabic("مكتبة_مشتركة") == TargetType::SharedLib);
        EXPECT(targetTypeFromArabic("shared") == TargetType::SharedLib);
        EXPECT(targetTypeFromArabic("صورة_نواة") == TargetType::KernelImage);
        EXPECT(targetTypeFromArabic("kernel") == TargetType::KernelImage);
        EXPECT(targetTypeFromArabic("مشغّل") == TargetType::Driver);
        EXPECT(targetTypeFromArabic("driver") == TargetType::Driver);
        EXPECT(targetTypeFromArabic("اختبار") == TargetType::Test);
        EXPECT(targetTypeFromArabic("test") == TargetType::Test);
    } END_TEST;

    TEST("تحويل ملف البناء من العربية") {
        EXPECT(profileFromArabic("تطوير") == BuildProfile::Debug);
        EXPECT(profileFromArabic("debug") == BuildProfile::Debug);
        EXPECT(profileFromArabic("إصدار") == BuildProfile::Release);
        EXPECT(profileFromArabic("release") == BuildProfile::Release);
        EXPECT(profileFromArabic("أصغر_حجم") == BuildProfile::MinSize);
        EXPECT(profileFromArabic("min-size") == BuildProfile::MinSize);
        EXPECT(profileFromArabic("نواة") == BuildProfile::Kernel);
        EXPECT(profileFromArabic("kernel") == BuildProfile::Kernel);
    } END_TEST;

    TEST("قيمة افتراضية لنوع غير معروف") {
        EXPECT(targetTypeFromArabic("unknown_xyz") == TargetType::Executable);
        EXPECT(profileFromArabic("unknown_xyz") == BuildProfile::Debug);
    } END_TEST;
}

// ============================================================================
// 2. اختبارات هياكل البيانات
// ============================================================================

void testDataStructures() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات هياكل البيانات                 ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("Dependency — إنشاء افتراضي") {
        Dependency dep("مكتبة_رياضيات");
        EXPECT(dep.name == "مكتبة_رياضيات");
        EXPECT(dep.version == "*");
        EXPECT(dep.isOptional == false);
    } END_TEST;

    TEST("Dependency — مع إصدار") {
        Dependency dep("مكتبة_شبكة", ">=2.0");
        EXPECT(dep.name == "مكتبة_شبكة");
        EXPECT(dep.version == ">=2.0");
    } END_TEST;

    TEST("BuildTarget — إنشاء") {
        BuildTarget target("تطبيقي", TargetType::Executable);
        EXPECT(target.name == "تطبيقي");
        EXPECT(target.type == TargetType::Executable);
        EXPECT(target.noStd == false);
        EXPECT(target.sources.empty());
    } END_TEST;

    TEST("ProjectConfig — افتراضي") {
        ProjectConfig cfg;
        EXPECT(cfg.profile == BuildProfile::Debug);
        EXPECT(cfg.targets.empty());
        EXPECT(cfg.dependencies.empty());
    } END_TEST;
}

// ============================================================================
// 3. اختبارات تحليل ملف البناء
// ============================================================================

void testConfigParsing() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات تحليل ملف البناء               ║\n"
              << "╚══════════════════════════════════════════╝\n";

    setupTestDir();

    TEST("تحليل ملف بناء بسيط") {
        std::string cfgFile = TEST_DIR + "/build.sad";
        {
            std::ofstream f(cfgFile);
            f << "[project]\n"
              << "name = \"MyApp\"\n"
              << "version = \"1.0.0\"\n"
              << "author = \"Ahmed\"\n"
              << "\n"
              << "[target]\n"
              << "type = \"exe\"\n"
              << "\n"
              << "[build]\n"
              << "profile = \"release\"\n";
        }
        BuildSystem bs;
        bool ok = bs.loadConfig(cfgFile);
        EXPECT(ok);
        EXPECT(bs.getConfig().name == "MyApp");
        EXPECT(bs.getConfig().version == "1.0.0");
        EXPECT(bs.getConfig().author == "Ahmed");
        EXPECT(bs.getConfig().profile == BuildProfile::Release);
    } END_TEST;

    TEST("تحليل ملف بتبعيات") {
        std::string cfgFile = TEST_DIR + "/build2.sad";
        {
            std::ofstream f(cfgFile);
            f << "[project]\n"
              << "name = \"TestDeps\"\n"
              << "\n"
              << "[dependencies]\n"
              << "math_lib = \"1.0\"\n"
              << "network = \"https://github.com/example/net.git\"\n";
        }
        BuildSystem bs;
        bool ok = bs.loadConfig(cfgFile);
        EXPECT(ok);
        EXPECT(bs.getConfig().dependencies.size() == 2);
    } END_TEST;

    TEST("تحليل ملف غير موجود") {
        BuildSystem bs;
        bool ok = bs.loadConfig("nonexistent_build_file_12345.sad");
        EXPECT(!ok);
    } END_TEST;

    TEST("تحليل ملف فارغ") {
        std::string cfgFile = TEST_DIR + "/empty.sad";
        { std::ofstream f(cfgFile); }
        BuildSystem bs;
        bool ok = bs.loadConfig(cfgFile);
        EXPECT(!ok);
    } END_TEST;

    cleanupTestDir();
}

// ============================================================================
// 4. اختبارات أمر البناء
// ============================================================================

void testBuildCommand() {
    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║  اختبارات أمر البناء                     ║\n"
              << "╚══════════════════════════════════════════╝\n";

    TEST("getBuildCommand — أساسي") {
        BuildSystem bs;
        BuildTarget target("test", TargetType::Executable);
        target.sources.push_back("main.ص");
        std::string cmd = bs.getBuildCommand(target);
        EXPECT(!cmd.empty());
        EXPECT(cmd.find("sadc") != std::string::npos);
        EXPECT(cmd.find("main.ص") != std::string::npos);
    } END_TEST;

    TEST("getBuildCommand — نواة بدون مكتبة قياسية") {
        BuildSystem bs;
        BuildTarget target("kernel", TargetType::KernelImage);
        target.noStd = true;
        target.targetTriple = "x86_64-bare-none";
        target.linkerScript = "kernel.ld";
        target.sources.push_back("boot.ص");
        std::string cmd = bs.getBuildCommand(target);
        // يجب أن يحتوي على خيارات النواة
        EXPECT(cmd.find("لا-مكتبة-قياسية") != std::string::npos ||
               cmd.find("no-std") != std::string::npos);
    } END_TEST;

    TEST("getBuildCommand — وضع إصدار") {
        BuildSystem bs;
        bs.getConfig().profile = BuildProfile::Release;
        BuildTarget target("app", TargetType::Executable);
        target.sources.push_back("app.ص");
        std::string cmd = bs.getBuildCommand(target);
        EXPECT(cmd.find("إصدار") != std::string::npos);
    } END_TEST;
}

// ============================================================================
// نقطة الدخول
// ============================================================================

int main() {
    std::cout << "╔══════════════════════════════════════════════════════╗\n"
              << "║  🔨 اختبارات نظام البناء — لغة ص                    ║\n"
              << "╚══════════════════════════════════════════════════════╝\n";

    testEnumConversions();
    testDataStructures();
    testConfigParsing();
    testBuildCommand();

    stats.print();
    return stats.failed > 0 ? 1 : 0;
}
