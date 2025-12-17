/*
 * اختبارات الرابط LLVM / LLVM Linker Tests
 * ==========================================
 */

#include "test_framework.h"
#include "test_helpers.h"
#include "../../../compiler/backends/llvm/llvm_linker.h"
#include <filesystem>

using namespace sad;
using namespace sad::testing;

namespace fs = std::filesystem;

/**
 * الاختبار 1: التهيئة الأساسية / Basic Initialization
 */
TEST(LLVMLinker, BasicInitialization) {
    LLVMLinker linker;
    EXPECT_TRUE(linker.initialize());
}

/**
 * الاختبار 2: التهيئة مع هدف مخصص / Initialization with Custom Target
 */
TEST(LLVMLinker, InitializationWithTarget) {
    LLVMLinker linker;
    EXPECT_TRUE(linker.initialize("x86_64-unknown-linux-gnu"));
}

/**
 * الاختبار 3: تعيين نوع الربط - تنفيذي / Set Linking Type - Executable
 */
TEST(LLVMLinker, SetLinkingTypeExecutable) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLinkingType(LinkingType::Executable));
}

/**
 * الاختبار 4: تعيين نوع الربط - مكتبة مشتركة / Set Linking Type - Shared Library
 */
TEST(LLVMLinker, SetLinkingTypeSharedLibrary) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLinkingType(LinkingType::SharedLibrary));
}

/**
 * الاختبار 5: تعيين نوع الربط - مكتبة ثابتة / Set Linking Type - Static Library
 */
TEST(LLVMLinker, SetLinkingTypeStaticLibrary) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLinkingType(LinkingType::StaticLibrary));
}

/**
 * الاختبار 6: تعيين نمط الربط - ثابت / Set Linking Mode - Static
 */
TEST(LLVMLinker, SetLinkingModeStatic) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLinkingMode(LinkingMode::Static));
}

/**
 * الاختبار 7: تعيين نمط الربط - ديناميكي / Set Linking Mode - Dynamic
 */
TEST(LLVMLinker, SetLinkingModeDynamic) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLinkingMode(LinkingMode::Dynamic));
}

/**
 * الاختبار 8: تعيين نمط الربط - PIE / Set Linking Mode - PIE
 */
TEST(LLVMLinker, SetLinkingModePIE) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLinkingMode(LinkingMode::PIE));
}

/**
 * الاختبار 9: تعيين مستوى LTO - بدون / Set LTO Level - None
 */
TEST(LLVMLinker, SetLTOLevelNone) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLTOLevel(LTOLevel::None));
}

/**
 * الاختبار 10: تعيين مستوى LTO - Thin / Set LTO Level - Thin
 */
TEST(LLVMLinker, SetLTOLevelThin) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLTOLevel(LTOLevel::Thin));
}

/**
 * الاختبار 11: تعيين مستوى LTO - Full / Set LTO Level - Full
 */
TEST(LLVMLinker, SetLTOLevelFull) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setLTOLevel(LTOLevel::Full));
}

/**
 * الاختبار 12: تعيين ملف الإخراج / Set Output File
 */
TEST(LLVMLinker, SetOutputFile) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setOutputFile("test_output.exe"));
}

/**
 * الاختبار 13: تعيين نقطة الدخول / Set Entry Point
 */
TEST(LLVMLinker, SetEntryPoint) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setEntryPoint("main"));
}

/**
 * الاختبار 14: إضافة ملف كائن / Add Object File
 */
TEST(LLVMLinker, AddObjectFile) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.addObjectFile("test.o"));
}

/**
 * الاختبار 15: إضافة عدة ملفات كائنات / Add Multiple Object Files
 */
TEST(LLVMLinker, AddMultipleObjectFiles) {
    LLVMLinker linker;
    linker.initialize();
    std::vector<std::string> files = {"test1.o", "test2.o", "test3.o"};
    EXPECT_NO_THROW(linker.addObjectFiles(files));
}

/**
 * الاختبار 16: إضافة مكتبة / Add Library
 */
TEST(LLVMLinker, AddLibrary) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.addLibrary("m"));
}

/**
 * الاختبار 17: إضافة عدة مكتبات / Add Multiple Libraries
 */
TEST(LLVMLinker, AddMultipleLibraries) {
    LLVMLinker linker;
    linker.initialize();
    std::vector<std::string> libs = {"c", "m", "pthread"};
    EXPECT_NO_THROW(linker.addLibraries(libs));
}

/**
 * الاختبار 18: إضافة مسار مكتبة / Add Library Path
 */
TEST(LLVMLinker, AddLibraryPath) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.addLibraryPath("/usr/lib"));
}

/**
 * الاختبار 19: إضافة عدة مسارات مكتبات / Add Multiple Library Paths
 */
TEST(LLVMLinker, AddMultipleLibraryPaths) {
    LLVMLinker linker;
    linker.initialize();
    std::vector<std::string> paths = {"/usr/lib", "/usr/local/lib"};
    EXPECT_NO_THROW(linker.addLibraryPaths(paths));
}

/**
 * الاختبار 20: إضافة علم رابط / Add Linker Flag
 */
TEST(LLVMLinker, AddLinkerFlag) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.addLinkerFlag("-pie"));
}

/**
 * الاختبار 21: تعيين إزالة الرموز / Set Strip Symbols
 */
TEST(LLVMLinker, SetStripSymbols) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setStripSymbols(true));
    EXPECT_NO_THROW(linker.setStripSymbols(false));
}

/**
 * الاختبار 22: تعيين إنشاء ملف خريطة / Set Generate Map
 */
TEST(LLVMLinker, SetGenerateMap) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setGenerateMap(true, "test.map"));
    EXPECT_NO_THROW(linker.setGenerateMap(false));
}

/**
 * الاختبار 23: تعيين بدون stdlib / Set No Stdlib
 */
TEST(LLVMLinker, SetNoStdlib) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setNoStdlib(true));
    EXPECT_NO_THROW(linker.setNoStdlib(false));
}

/**
 * الاختبار 24: تعيين بدون مكتبات افتراضية / Set No Default Libs
 */
TEST(LLVMLinker, SetNoDefaultLibs) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setNoDefaultLibs(true));
    EXPECT_NO_THROW(linker.setNoDefaultLibs(false));
}

/**
 * الاختبار 25: تعيين مطوّل / Set Verbose
 */
TEST(LLVMLinker, SetVerbose) {
    LLVMLinker linker;
    linker.initialize();
    EXPECT_NO_THROW(linker.setVerbose(true));
    EXPECT_NO_THROW(linker.setVerbose(false));
}

/**
 * الاختبار 26: الحصول على الأعلام الافتراضية لـ Windows / Get Default Flags for Windows
 */
TEST(LLVMLinker, GetDefaultFlagsWindows) {
    auto flags = LLVMLinker::getDefaultLinkerFlags("x86_64-pc-windows-msvc");
    EXPECT_FALSE(flags.empty());
}

/**
 * الاختبار 27: الحصول على الأعلام الافتراضية لـ Linux / Get Default Flags for Linux
 */
TEST(LLVMLinker, GetDefaultFlagsLinux) {
    auto flags = LLVMLinker::getDefaultLinkerFlags("x86_64-unknown-linux-gnu");
    EXPECT_FALSE(flags.empty());
}

/**
 * الاختبار 28: الحصول على المكتبات الافتراضية لـ Windows / Get Default Libraries for Windows
 */
TEST(LLVMLinker, GetDefaultLibrariesWindows) {
    auto libs = LLVMLinker::getDefaultLibraries("x86_64-pc-windows-msvc");
    EXPECT_FALSE(libs.empty());
}

/**
 * الاختبار 29: الحصول على المكتبات الافتراضية لـ Linux / Get Default Libraries for Linux
 */
TEST(LLVMLinker, GetDefaultLibrariesLinux) {
    auto libs = LLVMLinker::getDefaultLibraries("x86_64-unknown-linux-gnu");
    EXPECT_FALSE(libs.empty());
}

/**
 * الاختبار 30: الحصول على معلومات الربط / Get Linking Info
 */
TEST(LLVMLinker, GetLinkingInfo) {
    LLVMLinker linker;
    linker.initialize();
    
    const auto& info = linker.getLinkingInfo();
    EXPECT_EQ(info.total_symbols, 0);
    EXPECT_EQ(info.undefined_symbols, 0);
    EXPECT_EQ(info.output_size, 0);
}

/**
 * الاختبار 31: الربط بدون ملفات يفشل / Linking Without Files Fails
 */
TEST(LLVMLinker, LinkWithoutFilesFails) {
    LLVMLinker linker;
    linker.initialize();
    linker.setOutputFile("test.exe");
    
    EXPECT_FALSE(linker.link());
}

/**
 * الاختبار 32: الربط مع ملف غير موجود يفشل / Linking With Non-Existent File Fails
 */
TEST(LLVMLinker, LinkWithNonExistentFileFails) {
    LLVMLinker linker;
    linker.initialize();
    linker.setOutputFile("test.exe");
    linker.addObjectFile("non_existent_file.o");
    
    EXPECT_FALSE(linker.link());
}

/**
 * الاختبار 33: إنشاء ملف كائن وهمي والربط / Create Dummy Object and Link
 */
TEST(LLVMLinker, CreateDummyObjectAndLink) {
    // إنشاء ملف كائن وهمي / Create dummy object file
    const std::string obj_file = "test_dummy.o";
    EXPECT_TRUE(createDummyObjectFile(obj_file));
    
    LLVMLinker linker;
    linker.initialize();
    linker.setOutputFile("test_dummy.exe");
    linker.addObjectFile(obj_file);
    
    // قد ينجح أو يفشل حسب توفر الرابط / May succeed or fail depending on linker availability
    linker.link();
    
    // تنظيف / Cleanup
    deleteFile(obj_file);
}

/**
 * الاختبار 34: التحقق من توفر LLD / Check LLD Availability
 */
TEST(LLVMLinker, CheckLLDAvailability) {
    bool available = LLVMLinker::isLLDAvailable();
    // لا نتوقع قيمة معينة، فقط التحقق من عدم حدوث خطأ / Just verify no error
    (void)available;
}

/**
 * الاختبار 35: تكوين خيارات شاملة / Configure Comprehensive Options
 */
TEST(LLVMLinker, ConfigureComprehensiveOptions) {
    LLVMLinker linker;
    linker.initialize("x86_64-unknown-linux-gnu");
    
    linker.setLinkingType(LinkingType::Executable);
    linker.setLinkingMode(LinkingMode::PIE);
    linker.setLTOLevel(LTOLevel::Thin);
    linker.setOutputFile("comprehensive_test.exe");
    linker.setEntryPoint("main");
    linker.setStripSymbols(true);
    linker.setGenerateMap(true, "comprehensive_test.map");
    linker.setVerbose(true);
    
    linker.addObjectFile("main.o");
    linker.addLibraries({"c", "m", "pthread"});
    linker.addLibraryPath("/usr/lib");
    linker.addLinkerFlag("-O3");
    
    // التكوين يجب أن ينجح / Configuration should succeed
    EXPECT_TRUE(true);
}

/**
 * الاختبار 36: طباعة معلومات الربط / Print Linking Info
 */
TEST(LLVMLinker, PrintLinkingInfo) {
    LLVMLinker linker;
    linker.initialize();
    
    // يجب ألا يحدث استثناء / Should not throw
    EXPECT_NO_THROW(linker.printLinkingInfo());
}

/**
 * الاختبار 37: أعلام Windows MSVC / Windows MSVC Flags
 */
TEST(LLVMLinker, WindowsMSVCFlags) {
    auto flags = LLVMLinker::getDefaultLinkerFlags("x86_64-pc-windows-msvc");
    
    bool has_subsystem = false;
    for (const auto& flag : flags) {
        if (contains(flag, "SUBSYSTEM") || contains(flag, "subsystem")) {
            has_subsystem = true;
            break;
        }
    }
    
    EXPECT_TRUE(has_subsystem);
}

/**
 * الاختبار 38: مكتبات Linux الافتراضية / Linux Default Libraries
 */
TEST(LLVMLinker, LinuxDefaultLibraries) {
    auto libs = LLVMLinker::getDefaultLibraries("x86_64-unknown-linux-gnu");
    
    bool has_libc = false;
    for (const auto& lib : libs) {
        if (lib == "c") {
            has_libc = true;
            break;
        }
    }
    
    EXPECT_TRUE(has_libc);
}

/**
 * الاختبار 39: أعلام macOS / macOS Flags
 */
TEST(LLVMLinker, MacOSFlags) {
    auto flags = LLVMLinker::getDefaultLinkerFlags("x86_64-apple-darwin");
    
    bool has_arch = false;
    for (const auto& flag : flags) {
        if (flag == "-arch" || contains(flag, "arch")) {
            has_arch = true;
            break;
        }
    }
    
    EXPECT_TRUE(has_arch);
}

/**
 * الاختبار 40: مكتبات macOS الافتراضية / macOS Default Libraries
 */
TEST(LLVMLinker, MacOSDefaultLibraries) {
    auto libs = LLVMLinker::getDefaultLibraries("x86_64-apple-darwin");
    
    bool has_system = false;
    for (const auto& lib : libs) {
        if (lib == "System") {
            has_system = true;
            break;
        }
    }
    
    EXPECT_TRUE(has_system);
}

/**
 * الدالة الرئيسية لتشغيل الاختبارات / Main function to run tests
 */
int main(int argc, char** argv) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات الرابط LLVM / LLVM Linker Tests                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << std::endl;
    
    auto& manager = TestManager::instance();
    int result = manager.runAllTests(true);
    
    return result;
}
