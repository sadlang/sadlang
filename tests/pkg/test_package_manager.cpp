// بسم الله الرحمن الرحيم
/**
 * @file test_package_manager.cpp
 * @brief Tests for Package Management System - اختبارات لنظام إدارة الحزم
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "../tools/pkg/toml_parser.h"
#include "../tools/pkg/package.h"
#include "../tools/pkg/dependency_resolver.h"
#include <filesystem>
#include <fstream>

using namespace sad::pkg;

// ============================================================================
// TOML Parser Tests - اختبارات محلل TOML
// ============================================================================

TEST(TomlParserTest, ParseSimpleKeyValue) {
    std::string toml = R"(
        name = "test_package"
        version = "1.0.0"
        number = 42
        float_num = 3.14
        flag = true
    )";

    auto result = TomlParser::parse(toml);

    EXPECT_TRUE(result.contains("name"));
    EXPECT_EQ(result["name"].as_string(), "test_package");
    EXPECT_EQ(result["version"].as_string(), "1.0.0");
    EXPECT_EQ(result["number"].as_integer(), 42);
    EXPECT_DOUBLE_EQ(result["float_num"].as_float(), 3.14);
    EXPECT_TRUE(result["flag"].as_bool());
}

TEST(TomlParserTest, ParseArray) {
    std::string toml = R"(
        numbers = [1, 2, 3, 4, 5]
        strings = ["hello", "world"]
    )";

    auto result = TomlParser::parse(toml);

    EXPECT_TRUE(result["numbers"].is_array());
    EXPECT_EQ(result["numbers"].size(), 5);
    EXPECT_EQ(result["numbers"][0].as_integer(), 1);
    EXPECT_EQ(result["numbers"][4].as_integer(), 5);

    EXPECT_EQ(result["strings"].size(), 2);
    EXPECT_EQ(result["strings"][0].as_string(), "hello");
}

TEST(TomlParserTest, ParseTable) {
    std::string toml = R"(
        [package]
        name = "my_package"
        version = "0.1.0"

        [dependencies]
        http = "^1.0.0"
        json = "~2.3.0"
    )";

    auto result = TomlParser::parse(toml);

    EXPECT_TRUE(result.contains("package"));
    EXPECT_TRUE(result["package"].is_table());
    EXPECT_EQ(result["package"]["name"].as_string(), "my_package");

    EXPECT_TRUE(result.contains("dependencies"));
    EXPECT_EQ(result["dependencies"]["http"].as_string(), "^1.0.0");
}

TEST(TomlParserTest, ParseInlineTable) {
    std::string toml = R"(
        point = { x = 10, y = 20 }
    )";

    auto result = TomlParser::parse(toml);

    EXPECT_TRUE(result["point"].is_table());
    EXPECT_EQ(result["point"]["x"].as_integer(), 10);
    EXPECT_EQ(result["point"]["y"].as_integer(), 20);
}

TEST(TomlParserTest, SerializeToString) {
    TomlTable table;
    table["name"] = TomlValue("test");
    table["version"] = TomlValue("1.0.0");
    table["count"] = TomlValue(42);

    std::string serialized = TomlParser::serialize(table);

    EXPECT_TRUE(serialized.find("name = \"test\"") != std::string::npos);
    EXPECT_TRUE(serialized.find("version = \"1.0.0\"") != std::string::npos);
    EXPECT_TRUE(serialized.find("count = 42") != std::string::npos);
}

// ============================================================================
// Version Tests - اختبارات الإصدار
// ============================================================================

TEST(VersionTest, ParseVersion) {
    auto v1 = Version::parse("1.2.3");
    EXPECT_EQ(v1.major, 1);
    EXPECT_EQ(v1.minor, 2);
    EXPECT_EQ(v1.patch, 3);

    auto v2 = Version::parse("2.0.0-alpha.1");
    EXPECT_EQ(v2.major, 2);
    EXPECT_EQ(v2.minor, 0);
    EXPECT_EQ(v2.patch, 0);
    EXPECT_EQ(v2.prerelease, "alpha.1");

    auto v3 = Version::parse("1.0.0+build.123");
    EXPECT_EQ(v3.build, "build.123");
}

TEST(VersionTest, VersionComparison) {
    auto v1 = Version::parse("1.0.0");
    auto v2 = Version::parse("1.0.1");
    auto v3 = Version::parse("1.1.0");
    auto v4 = Version::parse("2.0.0");

    EXPECT_TRUE(v1 < v2);
    EXPECT_TRUE(v2 < v3);
    EXPECT_TRUE(v3 < v4);
    EXPECT_TRUE(v1 <= v1);
    EXPECT_TRUE(v4 > v1);
}

TEST(VersionTest, PrereleaseVersion) {
    auto v1 = Version::parse("1.0.0-alpha");
    auto v2 = Version::parse("1.0.0");

    EXPECT_TRUE(v1 < v2);  // Prerelease has lower precedence
}

TEST(VersionTest, VersionToString) {
    auto v = Version::parse("1.2.3-beta.1+build.456");
    EXPECT_EQ(v.to_string(), "1.2.3-beta.1+build.456");
}

// ============================================================================
// Version Requirement Tests - اختبارات متطلب الإصدار
// ============================================================================

TEST(VersionRequirementTest, ExactMatch) {
    auto req = VersionRequirement::parse("1.2.3");
    
    EXPECT_TRUE(req.matches(Version::parse("1.2.3")));
    EXPECT_FALSE(req.matches(Version::parse("1.2.4")));
    EXPECT_FALSE(req.matches(Version::parse("1.3.0")));
}

TEST(VersionRequirementTest, CaretRequirement) {
    auto req = VersionRequirement::parse("^1.2.3");
    
    EXPECT_TRUE(req.matches(Version::parse("1.2.3")));
    EXPECT_TRUE(req.matches(Version::parse("1.2.4")));
    EXPECT_TRUE(req.matches(Version::parse("1.9.0")));
    EXPECT_FALSE(req.matches(Version::parse("2.0.0")));
    EXPECT_FALSE(req.matches(Version::parse("1.2.2")));
}

TEST(VersionRequirementTest, TildeRequirement) {
    auto req = VersionRequirement::parse("~1.2.3");
    
    EXPECT_TRUE(req.matches(Version::parse("1.2.3")));
    EXPECT_TRUE(req.matches(Version::parse("1.2.4")));
    EXPECT_FALSE(req.matches(Version::parse("1.3.0")));
    EXPECT_FALSE(req.matches(Version::parse("2.0.0")));
}

TEST(VersionRequirementTest, GreaterThan) {
    auto req = VersionRequirement::parse(">1.2.3");
    
    EXPECT_FALSE(req.matches(Version::parse("1.2.3")));
    EXPECT_TRUE(req.matches(Version::parse("1.2.4")));
    EXPECT_TRUE(req.matches(Version::parse("2.0.0")));
}

TEST(VersionRequirementTest, GreaterThanOrEqual) {
    auto req = VersionRequirement::parse(">=1.2.3");
    
    EXPECT_TRUE(req.matches(Version::parse("1.2.3")));
    EXPECT_TRUE(req.matches(Version::parse("1.2.4")));
    EXPECT_TRUE(req.matches(Version::parse("2.0.0")));
    EXPECT_FALSE(req.matches(Version::parse("1.2.2")));
}

TEST(VersionRequirementTest, Wildcard) {
    auto req = VersionRequirement::parse("*");
    
    EXPECT_TRUE(req.matches(Version::parse("1.0.0")));
    EXPECT_TRUE(req.matches(Version::parse("2.5.7")));
    EXPECT_TRUE(req.matches(Version::parse("100.0.0")));
}

// ============================================================================
// Package Tests - اختبارات الحزمة
// ============================================================================

TEST(PackageTest, CreateAndSavePackage) {
    // Create temporary directory - إنشاء مجلد مؤقت
    auto temp_dir = std::filesystem::temp_directory_path() / "sad_test_pkg";
    std::filesystem::create_directories(temp_dir);

    // Create package - إنشاء حزمة
    Package pkg;
    pkg.name = "test_package";
    pkg.version = Version(1, 0, 0);
    pkg.description = "A test package";
    pkg.authors.push_back("Test Author");
    pkg.license = "MIT";
    
    auto req = VersionRequirement::parse("^1.0.0");
    pkg.dependencies.emplace_back("http", req);

    // Save to file - الحفظ في ملف
    auto toml_path = temp_dir / "sad.toml";
    pkg.save_to_file(toml_path);

    EXPECT_TRUE(std::filesystem::exists(toml_path));

    // Load and verify - التحميل والتحقق
    Package loaded_pkg = Package::from_file(toml_path);
    EXPECT_EQ(loaded_pkg.name, "test_package");
    EXPECT_EQ(loaded_pkg.version.to_string(), "1.0.0");
    EXPECT_EQ(loaded_pkg.description, "A test package");
    EXPECT_EQ(loaded_pkg.dependencies.size(), 1);
    EXPECT_EQ(loaded_pkg.dependencies[0].name, "http");

    // Cleanup - التنظيف
    std::filesystem::remove_all(temp_dir);
}

TEST(PackageTest, ValidatePackage) {
    Package pkg;
    pkg.name = "test-package";
    pkg.version = Version(1, 0, 0);

    auto errors = pkg.validate();
    EXPECT_EQ(errors.size(), 0);

    // Test invalid name - اختبار اسم غير صالح
    Package invalid_pkg;
    invalid_pkg.name = "Invalid Name";  // Spaces not allowed
    invalid_pkg.version = Version(1, 0, 0);

    auto invalid_errors = invalid_pkg.validate();
    EXPECT_GT(invalid_errors.size(), 0);
}

TEST(PackageTest, GetSourceFiles) {
    // Create test directory structure - إنشاء هيكل مجلدات الاختبار
    auto temp_dir = std::filesystem::temp_directory_path() / "sad_test_src";
    std::filesystem::create_directories(temp_dir / "src");
    
    // Create source files - إنشاء ملفات المصدر
    std::ofstream(temp_dir / "src" / "main.s") << "// Test file";
    std::ofstream(temp_dir / "src" / "utils.s") << "// Utils";
    std::ofstream(temp_dir / "src" / "readme.txt") << "Not a source file";

    // Create package - إنشاء حزمة
    Package pkg;
    pkg.name = "test";
    pkg.version = Version(1, 0, 0);
    pkg.package_dir = temp_dir;
    pkg.source_dirs.push_back("src");

    auto source_files = pkg.get_source_files();
    EXPECT_EQ(source_files.size(), 2);  // Only .s files

    // Cleanup - التنظيف
    std::filesystem::remove_all(temp_dir);
}

// ============================================================================
// Dependency Resolver Tests - اختبارات محلل التبعيات
// ============================================================================

TEST(DependencyResolverTest, SimpleResolution) {
    MockPackageRegistry registry;

    // Create packages - إنشاء الحزم
    Package pkg_a;
    pkg_a.name = "pkg_a";
    pkg_a.version = Version(1, 0, 0);

    Package pkg_b;
    pkg_b.name = "pkg_b";
    pkg_b.version = Version(1, 0, 0);
    pkg_b.dependencies.emplace_back("pkg_a", VersionRequirement::parse("^1.0.0"));

    registry.add_package(pkg_a);
    registry.add_package(pkg_b);

    // Resolve - الحل
    DependencyResolver resolver(registry);
    auto resolved = resolver.resolve(pkg_b, false);

    EXPECT_EQ(resolved.size(), 1);  // pkg_a
    EXPECT_EQ(resolved[0].name, "pkg_a");
    EXPECT_FALSE(resolver.has_conflicts());
}

TEST(DependencyResolverTest, TransitiveDependencies) {
    MockPackageRegistry registry;

    // Create packages: C -> B -> A
    Package pkg_a;
    pkg_a.name = "pkg_a";
    pkg_a.version = Version(1, 0, 0);

    Package pkg_b;
    pkg_b.name = "pkg_b";
    pkg_b.version = Version(1, 0, 0);
    pkg_b.dependencies.emplace_back("pkg_a", VersionRequirement::parse("^1.0.0"));

    Package pkg_c;
    pkg_c.name = "pkg_c";
    pkg_c.version = Version(1, 0, 0);
    pkg_c.dependencies.emplace_back("pkg_b", VersionRequirement::parse("^1.0.0"));

    registry.add_package(pkg_a);
    registry.add_package(pkg_b);
    registry.add_package(pkg_c);

    // Resolve - الحل
    DependencyResolver resolver(registry);
    auto resolved = resolver.resolve(pkg_c, false);

    EXPECT_EQ(resolved.size(), 2);  // pkg_a and pkg_b
    
    // Verify installation order (A before B) - التحقق من ترتيب التثبيت
    bool a_before_b = false;
    for (size_t i = 0; i < resolved.size(); i++) {
        if (resolved[i].name == "pkg_a") {
            for (size_t j = i + 1; j < resolved.size(); j++) {
                if (resolved[j].name == "pkg_b") {
                    a_before_b = true;
                    break;
                }
            }
        }
    }
    EXPECT_TRUE(a_before_b);
}

TEST(DependencyResolverTest, VersionConflict) {
    MockPackageRegistry registry;

    // Create packages with conflicting versions
    Package pkg_a_v1;
    pkg_a_v1.name = "pkg_a";
    pkg_a_v1.version = Version(1, 0, 0);

    Package pkg_a_v2;
    pkg_a_v2.name = "pkg_a";
    pkg_a_v2.version = Version(2, 0, 0);

    Package pkg_b;
    pkg_b.name = "pkg_b";
    pkg_b.version = Version(1, 0, 0);
    pkg_b.dependencies.emplace_back("pkg_a", VersionRequirement::parse("^1.0.0"));

    Package pkg_c;
    pkg_c.name = "pkg_c";
    pkg_c.version = Version(1, 0, 0);
    pkg_c.dependencies.emplace_back("pkg_a", VersionRequirement::parse("^2.0.0"));

    Package pkg_root;
    pkg_root.name = "root";
    pkg_root.version = Version(1, 0, 0);
    pkg_root.dependencies.emplace_back("pkg_b", VersionRequirement::parse("^1.0.0"));
    pkg_root.dependencies.emplace_back("pkg_c", VersionRequirement::parse("^1.0.0"));

    registry.add_package(pkg_a_v1);
    registry.add_package(pkg_a_v2);
    registry.add_package(pkg_b);
    registry.add_package(pkg_c);

    // Resolve should fail due to conflict - يجب أن يفشل الحل بسبب التعارض
    DependencyResolver resolver(registry);
    
    EXPECT_THROW({
        resolver.resolve(pkg_root, false);
    }, std::exception);

    EXPECT_TRUE(resolver.has_conflicts());
}

TEST(DependencyResolverTest, MultipleVersionsCompatible) {
    MockPackageRegistry registry;

    // Create multiple versions of pkg_a
    Package pkg_a_v1;
    pkg_a_v1.name = "pkg_a";
    pkg_a_v1.version = Version(1, 0, 0);

    Package pkg_a_v2;
    pkg_a_v2.name = "pkg_a";
    pkg_a_v2.version = Version(1, 5, 0);

    Package pkg_b;
    pkg_b.name = "pkg_b";
    pkg_b.version = Version(1, 0, 0);
    pkg_b.dependencies.emplace_back("pkg_a", VersionRequirement::parse("^1.0.0"));

    registry.add_package(pkg_a_v1);
    registry.add_package(pkg_a_v2);
    registry.add_package(pkg_b);

    // Resolve should pick latest compatible version (1.5.0)
    // يجب أن يختار أحدث إصدار متوافق
    DependencyResolver resolver(registry);
    auto resolved = resolver.resolve(pkg_b, false);

    EXPECT_EQ(resolved.size(), 1);
    EXPECT_EQ(resolved[0].name, "pkg_a");
    EXPECT_EQ(resolved[0].version.to_string(), "1.5.0");
}

// ============================================================================
// Main - الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
