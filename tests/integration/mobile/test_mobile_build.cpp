/**
 * @file test_mobile_build.cpp
 * @brief Integration tests for Sad Language mobile build system
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0.0
 *
 * هذا الملف يختبر نظام بناء تطبيقات الهاتف للغة ص
 * يشمل اختبارات Android و iOS
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

// Mobile build system headers
#include "tools/compiler/src/android_target.hpp"
#include "tools/compiler/src/ios_target.hpp"
#include "tools/compiler/src/mobile_project_gen.hpp"
#include "shared/parser/src/ui_parser.hpp"
#include "compiler_new/src/backend/android_compose.hpp"
#include "compiler_new/src/backend/ios_swiftui.hpp"

namespace fs = std::filesystem;

/**
 * @brief Test fixture for mobile build integration tests
 */
class MobileBuildTest : public ::testing::Test {
protected:
    fs::path testDir;
    fs::path outputDir;
    
    void SetUp() override {
        // Create temporary test directory
        testDir = fs::temp_directory_path() / "sad_mobile_tests";
        outputDir = testDir / "output";
        
        fs::create_directories(testDir);
        fs::create_directories(outputDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }
    
    // Helper to create a test Sad file with UI code
    void createTestSadFile(const std::string& filename, const std::string& content) {
        std::ofstream file(testDir / filename);
        file << content;
        file.close();
    }
    
    // Helper to read file content
    std::string readFile(const fs::path& path) {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};

// ===================================================
//                    Android Tests
// ===================================================

/**
 * @test Test Android NDK detection and configuration
 */
TEST_F(MobileBuildTest, AndroidNDKDetection) {
    sad::mobile::AndroidTarget target;
    
    // Should detect NDK if ANDROID_NDK_HOME is set
    bool hasNDK = target.detectNDK();
    
    // Test passes if NDK is found or gracefully reports not found
    if (hasNDK) {
        EXPECT_FALSE(target.getNDKPath().empty());
        EXPECT_GE(target.getNDKVersion(), 21); // Minimum NDK version
    }
}

/**
 * @test Test Android ABI configuration
 */
TEST_F(MobileBuildTest, AndroidABIConfiguration) {
    sad::mobile::AndroidTarget target;
    
    // Configure for multiple ABIs
    target.setTargetABIs({"arm64-v8a", "armeabi-v7a", "x86_64"});
    
    auto abis = target.getTargetABIs();
    EXPECT_EQ(abis.size(), 3);
    EXPECT_TRUE(std::find(abis.begin(), abis.end(), "arm64-v8a") != abis.end());
}

/**
 * @test Test Android API level configuration
 */
TEST_F(MobileBuildTest, AndroidAPILevel) {
    sad::mobile::AndroidTarget target;
    
    // Set minimum SDK
    target.setMinSDK(26);
    target.setTargetSDK(34);
    
    EXPECT_EQ(target.getMinSDK(), 26);
    EXPECT_EQ(target.getTargetSDK(), 34);
}

/**
 * @test Test Android Compose code generation
 */
TEST_F(MobileBuildTest, AndroidComposeCodegen) {
    // Create test UI code
    std::string sadUICode = R"(
        واجهة تطبيقي
            عمود(محاذاة: وسط)
                نص("مرحبا بالعالم!")
                    .لون(أزرق)
                    .حجم_خط(24)
                
                زر("اضغط هنا")
                    .عند_ضغط(أغلق)
            نهاية
        نهاية
    )";
    
    createTestSadFile("test_ui.s", sadUICode);
    
    // Parse UI
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "test_ui.s").string());
    ASSERT_NE(ast, nullptr);
    
    // Generate Compose code
    sad::backend::AndroidComposeGenerator generator;
    std::string composeCode = generator.generate(ast.get());
    
    // Verify generated code contains Compose elements
    EXPECT_TRUE(composeCode.find("@Composable") != std::string::npos);
    EXPECT_TRUE(composeCode.find("Column") != std::string::npos);
    EXPECT_TRUE(composeCode.find("Text") != std::string::npos);
    EXPECT_TRUE(composeCode.find("Button") != std::string::npos);
}

/**
 * @test Test Android project structure generation
 */
TEST_F(MobileBuildTest, AndroidProjectGeneration) {
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    config.name = "تطبيقي";
    config.nameEn = "MyApp";
    config.package = "com.example.myapp";
    config.platform = sad::mobile::Platform::Android;
    config.outputDir = outputDir.string();
    
    bool success = generator.generateProject(config);
    EXPECT_TRUE(success);
    
    // Verify project structure
    EXPECT_TRUE(fs::exists(outputDir / "app"));
    EXPECT_TRUE(fs::exists(outputDir / "app" / "src" / "main"));
    EXPECT_TRUE(fs::exists(outputDir / "build.gradle.kts"));
    EXPECT_TRUE(fs::exists(outputDir / "settings.gradle.kts"));
}

/**
 * @test Test Android Gradle configuration
 */
TEST_F(MobileBuildTest, AndroidGradleConfiguration) {
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    config.name = "اختبار";
    config.nameEn = "Test";
    config.package = "com.test.app";
    config.platform = sad::mobile::Platform::Android;
    config.minSDK = 26;
    config.targetSDK = 34;
    config.outputDir = outputDir.string();
    
    generator.generateProject(config);
    
    std::string buildGradle = readFile(outputDir / "app" / "build.gradle.kts");
    
    // Verify Gradle configuration
    EXPECT_TRUE(buildGradle.find("minSdk = 26") != std::string::npos);
    EXPECT_TRUE(buildGradle.find("targetSdk = 34") != std::string::npos);
    EXPECT_TRUE(buildGradle.find("compose") != std::string::npos);
}

// ===================================================
//                    iOS Tests
// ===================================================

/**
 * @test Test iOS SDK detection (macOS only)
 */
TEST_F(MobileBuildTest, iOSSDKDetection) {
    sad::mobile::iOSTarget target;
    
    bool hasSDK = target.detectSDK();
    
    #ifdef __APPLE__
    if (hasSDK) {
        EXPECT_FALSE(target.getSDKPath().empty());
        EXPECT_GE(target.getSDKVersion(), 15); // Minimum iOS version
    }
    #else
    // On non-macOS, should gracefully report not found
    EXPECT_FALSE(hasSDK);
    #endif
}

/**
 * @test Test iOS deployment target configuration
 */
TEST_F(MobileBuildTest, iOSDeploymentTarget) {
    sad::mobile::iOSTarget target;
    
    target.setDeploymentTarget("15.0");
    EXPECT_EQ(target.getDeploymentTarget(), "15.0");
    
    target.setArchitectures({"arm64"});
    auto archs = target.getArchitectures();
    EXPECT_EQ(archs.size(), 1);
    EXPECT_EQ(archs[0], "arm64");
}

/**
 * @test Test iOS SwiftUI code generation
 */
TEST_F(MobileBuildTest, iOSSwiftUICodegen) {
    // Create test UI code
    std::string sadUICode = R"(
        واجهة تطبيقي
            عمود(محاذاة: وسط)
                نص("مرحبا!")
                    .لون(أحمر)
                
                صورة("logo")
                    .عرض(100)
                    .ارتفاع(100)
                
                قائمة(عناصر: البيانات)
                    عنصر -> 
                        صف
                            نص(عنصر.اسم)
                        نهاية
                نهاية
            نهاية
        نهاية
    )";
    
    createTestSadFile("test_ui.s", sadUICode);
    
    // Parse UI
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "test_ui.s").string());
    ASSERT_NE(ast, nullptr);
    
    // Generate SwiftUI code
    sad::backend::iOSSwiftUIGenerator generator;
    std::string swiftCode = generator.generate(ast.get());
    
    // Verify generated code contains SwiftUI elements
    EXPECT_TRUE(swiftCode.find("import SwiftUI") != std::string::npos);
    EXPECT_TRUE(swiftCode.find("struct") != std::string::npos);
    EXPECT_TRUE(swiftCode.find("VStack") != std::string::npos);
    EXPECT_TRUE(swiftCode.find("Text") != std::string::npos);
    EXPECT_TRUE(swiftCode.find("Image") != std::string::npos);
    EXPECT_TRUE(swiftCode.find("ForEach") != std::string::npos);
}

/**
 * @test Test iOS project structure generation
 */
TEST_F(MobileBuildTest, iOSProjectGeneration) {
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    config.name = "تطبيقي";
    config.nameEn = "MyApp";
    config.bundleId = "com.example.myapp";
    config.platform = sad::mobile::Platform::iOS;
    config.outputDir = outputDir.string();
    
    bool success = generator.generateProject(config);
    EXPECT_TRUE(success);
    
    // Verify project structure
    EXPECT_TRUE(fs::exists(outputDir / "MyApp.xcodeproj"));
    EXPECT_TRUE(fs::exists(outputDir / "MyApp"));
    EXPECT_TRUE(fs::exists(outputDir / "MyApp" / "Info.plist"));
}

/**
 * @test Test iOS Info.plist generation
 */
TEST_F(MobileBuildTest, iOSInfoPlistGeneration) {
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    config.name = "تطبيق الاختبار";
    config.nameEn = "TestApp";
    config.bundleId = "com.test.app";
    config.platform = sad::mobile::Platform::iOS;
    config.iosDeploymentTarget = "15.0";
    config.outputDir = outputDir.string();
    
    generator.generateProject(config);
    
    std::string infoPlist = readFile(outputDir / "TestApp" / "Info.plist");
    
    // Verify Info.plist content
    EXPECT_TRUE(infoPlist.find("com.test.app") != std::string::npos);
    EXPECT_TRUE(infoPlist.find("MinimumOSVersion") != std::string::npos);
    EXPECT_TRUE(infoPlist.find("15.0") != std::string::npos);
}

// ===================================================
//                    UI Parser Tests
// ===================================================

/**
 * @test Test UI parser with basic elements
 */
TEST_F(MobileBuildTest, UIParserBasicElements) {
    std::string sadUICode = R"(
        واجهة شاشة_رئيسية
            نص("عنوان")
            زر("موافق")
            صورة("icon.png")
        نهاية
    )";
    
    createTestSadFile("basic_ui.s", sadUICode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "basic_ui.s").string());
    
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->getName(), "شاشة_رئيسية");
    EXPECT_EQ(ast->getChildren().size(), 3);
}

/**
 * @test Test UI parser with nested layouts
 */
TEST_F(MobileBuildTest, UIParserNestedLayouts) {
    std::string sadUICode = R"(
        واجهة شاشة_مركبة
            عمود
                صف
                    نص("يسار")
                    نص("يمين")
                نهاية
                
                شبكة(أعمدة: 2)
                    نص("1")
                    نص("2")
                    نص("3")
                    نص("4")
                نهاية
            نهاية
        نهاية
    )";
    
    createTestSadFile("nested_ui.s", sadUICode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "nested_ui.s").string());
    
    ASSERT_NE(ast, nullptr);
    
    // Verify nested structure
    auto column = ast->getChildren()[0];
    EXPECT_EQ(column->getType(), sad::parser::UINodeType::Column);
    EXPECT_EQ(column->getChildren().size(), 2);
}

/**
 * @test Test UI parser with modifiers
 */
TEST_F(MobileBuildTest, UIParserModifiers) {
    std::string sadUICode = R"(
        واجهة مع_تعديلات
            نص("نص معدل")
                .لون(أحمر)
                .حجم_خط(24)
                .خلفية(رمادي)
                .حشوة(16)
                .هامش(8)
                .دائري(12)
        نهاية
    )";
    
    createTestSadFile("modified_ui.s", sadUICode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "modified_ui.s").string());
    
    ASSERT_NE(ast, nullptr);
    
    auto textNode = ast->getChildren()[0];
    auto& modifiers = textNode->getModifiers();
    
    EXPECT_TRUE(modifiers.find("لون") != modifiers.end());
    EXPECT_TRUE(modifiers.find("حجم_خط") != modifiers.end());
    EXPECT_TRUE(modifiers.find("خلفية") != modifiers.end());
}

/**
 * @test Test UI parser with event handlers
 */
TEST_F(MobileBuildTest, UIParserEventHandlers) {
    std::string sadUICode = R"(
        واجهة مع_أحداث
            زر("اضغط")
                .عند_ضغط(معالج_ضغط)
            
            حقل_نص(قيمة: النص)
                .عند_تغيير(معالج_تغيير)
                .عند_إرسال(معالج_إرسال)
        نهاية
    )";
    
    createTestSadFile("events_ui.s", sadUICode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "events_ui.s").string());
    
    ASSERT_NE(ast, nullptr);
    
    auto buttonNode = ast->getChildren()[0];
    EXPECT_TRUE(buttonNode->hasEventHandler("عند_ضغط"));
    
    auto textFieldNode = ast->getChildren()[1];
    EXPECT_TRUE(textFieldNode->hasEventHandler("عند_تغيير"));
    EXPECT_TRUE(textFieldNode->hasEventHandler("عند_إرسال"));
}

// ===================================================
//                    API Bindings Tests
// ===================================================

/**
 * @test Test camera API type definitions
 */
TEST_F(MobileBuildTest, CameraAPITypes) {
    // Verify camera types are properly defined
    EXPECT_TRUE(sad::stdlib::camera::CameraErrorType::NotAvailable >= 0);
    EXPECT_TRUE(sad::stdlib::camera::CameraPosition::Back >= 0);
    EXPECT_TRUE(sad::stdlib::camera::FlashMode::Auto >= 0);
}

/**
 * @test Test location API type definitions
 */
TEST_F(MobileBuildTest, LocationAPITypes) {
    // Verify location types are properly defined
    EXPECT_TRUE(sad::stdlib::location::LocationError::NotAvailable >= 0);
    EXPECT_TRUE(sad::stdlib::location::LocationAccuracy::Best >= 0);
}

/**
 * @test Test sensors API type definitions
 */
TEST_F(MobileBuildTest, SensorsAPITypes) {
    // Verify sensor types are properly defined
    EXPECT_TRUE(sad::stdlib::sensors::SensorType::Accelerometer >= 0);
    EXPECT_TRUE(sad::stdlib::sensors::SensorError::NotAvailable >= 0);
    EXPECT_TRUE(sad::stdlib::sensors::UpdateRate::Normal >= 0);
}

// ===================================================
//                    Cross-Platform Tests
// ===================================================

/**
 * @test Test cross-platform UI abstraction
 */
TEST_F(MobileBuildTest, CrossPlatformUIAbstraction) {
    std::string sadUICode = R"(
        واجهة متعددة_المنصات
            عمود
                نص("يعمل على كل المنصات")
                زر("موافق")
            نهاية
        نهاية
    )";
    
    createTestSadFile("cross_platform.s", sadUICode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "cross_platform.s").string());
    ASSERT_NE(ast, nullptr);
    
    // Generate for Android
    sad::backend::AndroidComposeGenerator androidGen;
    std::string androidCode = androidGen.generate(ast.get());
    EXPECT_FALSE(androidCode.empty());
    
    // Generate for iOS
    sad::backend::iOSSwiftUIGenerator iosGen;
    std::string iosCode = iosGen.generate(ast.get());
    EXPECT_FALSE(iosCode.empty());
    
    // Both should produce valid code
    EXPECT_TRUE(androidCode.find("@Composable") != std::string::npos);
    EXPECT_TRUE(iosCode.find("SwiftUI") != std::string::npos);
}

/**
 * @test Test RTL layout support
 */
TEST_F(MobileBuildTest, RTLLayoutSupport) {
    std::string sadUICode = R"(
        واجهة عربية
            صف(اتجاه: يمين_لليسار)
                نص("نص عربي")
                أيقونة("سهم")
            نهاية
        نهاية
    )";
    
    createTestSadFile("rtl_ui.s", sadUICode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "rtl_ui.s").string());
    ASSERT_NE(ast, nullptr);
    
    // Android should use RTL layout
    sad::backend::AndroidComposeGenerator androidGen;
    std::string androidCode = androidGen.generate(ast.get());
    EXPECT_TRUE(androidCode.find("layoutDirection") != std::string::npos ||
                androidCode.find("RTL") != std::string::npos);
    
    // iOS should use RTL layout
    sad::backend::iOSSwiftUIGenerator iosGen;
    std::string iosCode = iosGen.generate(ast.get());
    EXPECT_TRUE(iosCode.find("layoutDirection") != std::string::npos ||
                iosCode.find("rightToLeft") != std::string::npos);
}

// ===================================================
//                    Build Integration Tests
// ===================================================

/**
 * @test Test full Android build process (requires Android SDK)
 */
TEST_F(MobileBuildTest, DISABLED_FullAndroidBuild) {
    // This test is disabled by default as it requires Android SDK
    // Enable with: --gtest_also_run_disabled_tests
    
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    config.name = "تطبيق_اختبار";
    config.nameEn = "TestApp";
    config.package = "com.sad.test";
    config.platform = sad::mobile::Platform::Android;
    config.outputDir = outputDir.string();
    
    generator.generateProject(config);
    
    // Create main activity
    std::string mainCode = R"(
        واجهة رئيسية
            عمود
                نص("تطبيق اختبار")
            نهاية
        نهاية
    )";
    createTestSadFile("main.ص", mainCode);
    
    // Build APK
    sad::mobile::AndroidBuilder builder;
    auto result = builder.buildDebugAPK(outputDir.string());
    
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(fs::exists(outputDir / "app" / "build" / "outputs" / "apk" / "debug" / "app-debug.apk"));
}

/**
 * @test Test full iOS build process (requires Xcode, macOS only)
 */
TEST_F(MobileBuildTest, DISABLED_FulliOSBuild) {
    // This test is disabled by default as it requires Xcode on macOS
    
    #ifndef __APPLE__
    GTEST_SKIP() << "iOS build requires macOS";
    #endif
    
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    config.name = "تطبيق_اختبار";
    config.nameEn = "TestApp";
    config.bundleId = "com.sad.test";
    config.platform = sad::mobile::Platform::iOS;
    config.outputDir = outputDir.string();
    
    generator.generateProject(config);
    
    // Build IPA
    sad::mobile::iOSBuilder builder;
    auto result = builder.buildSimulator(outputDir.string());
    
    EXPECT_TRUE(result.success);
}

// ===================================================
//                    Performance Tests
// ===================================================

/**
 * @test Test UI parsing performance
 */
TEST_F(MobileBuildTest, UIParsingPerformance) {
    // Generate a complex UI with many elements
    std::stringstream ss;
    ss << "واجهة اختبار_أداء\n";
    ss << "    عمود\n";
    
    for (int i = 0; i < 100; i++) {
        ss << "        صف\n";
        ss << "            نص(\"عنصر " << i << "\")\n";
        ss << "            زر(\"زر " << i << "\")\n";
        ss << "        نهاية\n";
    }
    
    ss << "    نهاية\n";
    ss << "نهاية\n";
    
    createTestSadFile("perf_ui.s", ss.str());
    
    sad::parser::UIParser parser;
    
    auto start = std::chrono::high_resolution_clock::now();
    auto ast = parser.parseFile((testDir / "perf_ui.s").string());
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    ASSERT_NE(ast, nullptr);
    EXPECT_LT(duration.count(), 1000); // Should parse in under 1 second
    
    std::cout << "UI parsing time for 100 rows: " << duration.count() << "ms" << std::endl;
}

/**
 * @test Test code generation performance
 */
TEST_F(MobileBuildTest, CodegenPerformance) {
    // Generate a complex UI
    std::stringstream ss;
    ss << "واجهة اختبار_توليد\n";
    ss << "    عمود\n";
    
    for (int i = 0; i < 50; i++) {
        ss << "        نص(\"نص " << i << "\").لون(أزرق).حجم_خط(14)\n";
    }
    
    ss << "    نهاية\n";
    ss << "نهاية\n";
    
    createTestSadFile("codegen_ui.s", ss.str());
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "codegen_ui.s").string());
    ASSERT_NE(ast, nullptr);
    
    // Measure Android codegen
    sad::backend::AndroidComposeGenerator androidGen;
    auto start1 = std::chrono::high_resolution_clock::now();
    std::string androidCode = androidGen.generate(ast.get());
    auto end1 = std::chrono::high_resolution_clock::now();
    
    // Measure iOS codegen
    sad::backend::iOSSwiftUIGenerator iosGen;
    auto start2 = std::chrono::high_resolution_clock::now();
    std::string iosCode = iosGen.generate(ast.get());
    auto end2 = std::chrono::high_resolution_clock::now();
    
    auto androidDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    auto iosDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
    
    EXPECT_LT(androidDuration.count(), 500);
    EXPECT_LT(iosDuration.count(), 500);
    
    std::cout << "Android codegen time: " << androidDuration.count() << "ms" << std::endl;
    std::cout << "iOS codegen time: " << iosDuration.count() << "ms" << std::endl;
}

// ===================================================
//                    Error Handling Tests
// ===================================================

/**
 * @test Test UI parser error handling
 */
TEST_F(MobileBuildTest, UIParserErrorHandling) {
    // Invalid UI code
    std::string invalidCode = R"(
        واجهة غير_مكتملة
            عمود
                نص("نص"
            # missing closing
    )";
    
    createTestSadFile("invalid_ui.s", invalidCode);
    
    sad::parser::UIParser parser;
    auto ast = parser.parseFile((testDir / "invalid_ui.s").string());
    
    EXPECT_EQ(ast, nullptr);
    EXPECT_FALSE(parser.getErrors().empty());
}

/**
 * @test Test invalid project configuration
 */
TEST_F(MobileBuildTest, InvalidProjectConfiguration) {
    sad::mobile::MobileProjectGenerator generator;
    
    sad::mobile::ProjectConfig config;
    // Missing required fields
    config.name = "";
    config.platform = sad::mobile::Platform::Android;
    
    bool success = generator.generateProject(config);
    EXPECT_FALSE(success);
}

/**
 * @test Test missing SDK handling
 */
TEST_F(MobileBuildTest, MissingSDKHandling) {
    // Set invalid SDK path
    sad::mobile::AndroidTarget target;
    target.setNDKPath("/invalid/path/to/ndk");
    
    auto result = target.validate();
    EXPECT_FALSE(result.valid);
    EXPECT_FALSE(result.errors.empty());
}

// ===================================================
//                    Main
// ===================================================

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
