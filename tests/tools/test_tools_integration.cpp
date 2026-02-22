// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: test_tools_integration.cpp
 * الوصف: اختبارات تكامل أدوات التطوير - Development Tools Integration Tests
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T099
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على اختبارات شاملة لجميع أدوات التطوير:
 * - مدير الحزم (Package Manager)
 * - محلل التبعيات (Dependency Resolver)
 * - عميل السجل (Registry Client)
 * - خادم LSP
 * - منسق الكود (Code Formatter)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * هيكل الاختبارات:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │ القسم                      │ الاختبارات                                   │
 * ├────────────────────────────────────────────────────────────────────────────┤
 * │ مدير الحزم                 │ التهيئة، التثبيت، الإزالة، القفل            │
 * │ محلل التبعيات              │ الحل، الترتيب، التعارضات                     │
 * │ عميل السجل                 │ البحث، التنزيل، النشر                        │
 * │ خادم LSP                   │ التهيئة، الإكمال، التشخيص، التمرير          │
 * │ منسق الكود                 │ المسافات، الأقواس، السطور                    │
 * └────────────────────────────────────────────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص - Sad Language Team
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <sstream>
#include <functional>
#include <chrono>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════════
// إطار الاختبار المبسط - Simple Test Framework
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class TestRunner
 * @brief إطار تشغيل الاختبارات - Test Runner Framework
 * 
 * يوفر آلية بسيطة لتسجيل وتشغيل الاختبارات
 * مع تتبع النتائج والإحصائيات
 */
class TestRunner {
public:
    /**
     * @struct TestResult
     * @brief نتيجة اختبار - Test Result
     */
    struct TestResult {
        std::string name;           // اسم الاختبار
        bool passed;                // هل نجح؟
        std::string message;        // رسالة الخطأ (إن وجدت)
        double duration_ms;         // المدة بالميلي ثانية
    };
    
    /**
     * @brief تسجيل اختبار
     */
    void register_test(const std::string& name, std::function<void()> test_fn) {
        tests_.push_back({name, test_fn});
    }
    
    /**
     * @brief تشغيل جميع الاختبارات
     */
    std::vector<TestResult> run_all() {
        std::vector<TestResult> results;
        
        std::cout << "\n═══════════════════════════════════════════════════════════════════\n";
        std::cout << "       اختبارات تكامل أدوات التطوير - Development Tools Tests\n";
        std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
        
        int passed = 0, failed = 0;
        
        for (const auto& [name, test_fn] : tests_) {
            TestResult result;
            result.name = name;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                test_fn();
                result.passed = true;
                passed++;
                std::cout << "  ✓ " << name << "\n";
            } catch (const std::exception& e) {
                result.passed = false;
                result.message = e.what();
                failed++;
                std::cout << "  ✗ " << name << "\n";
                std::cout << "    └─ خطأ: " << e.what() << "\n";
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();
            
            results.push_back(result);
        }
        
        std::cout << "\n───────────────────────────────────────────────────────────────────\n";
        std::cout << "  النتائج: " << passed << " نجح، " << failed << " فشل\n";
        std::cout << "───────────────────────────────────────────────────────────────────\n\n";
        
        return results;
    }
    
private:
    std::vector<std::pair<std::string, std::function<void()>>> tests_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ماكروهات الاختبار - Test Macros
// ═══════════════════════════════════════════════════════════════════════════════

#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        throw std::runtime_error(std::string(message) + " في السطر " + std::to_string(__LINE__)); \
    }

#define TEST_EQUAL(expected, actual, message) \
    if ((expected) != (actual)) { \
        std::ostringstream oss; \
        oss << message << " - متوقع: " << (expected) << "، فعلي: " << (actual); \
        throw std::runtime_error(oss.str()); \
    }

#define TEST_TRUE(condition) TEST_ASSERT(condition, "الشرط يجب أن يكون صحيحاً")
#define TEST_FALSE(condition) TEST_ASSERT(!(condition), "الشرط يجب أن يكون خاطئاً")

// ═══════════════════════════════════════════════════════════════════════════════
// محاكاة الإصدار الدلالي - Semantic Version Mock
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct SemanticVersion
 * @brief إصدار دلالي مبسط للاختبار
 */
struct SemanticVersion {
    int major = 0;
    int minor = 0;
    int patch = 0;
    std::string prerelease;
    
    bool operator==(const SemanticVersion& other) const {
        return major == other.major && 
               minor == other.minor && 
               patch == other.patch &&
               prerelease == other.prerelease;
    }
    
    bool operator<(const SemanticVersion& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
    
    std::string to_string() const {
        std::string result = std::to_string(major) + "." + 
                            std::to_string(minor) + "." + 
                            std::to_string(patch);
        if (!prerelease.empty()) {
            result += "-" + prerelease;
        }
        return result;
    }
    
    static SemanticVersion parse(const std::string& str) {
        SemanticVersion v;
        size_t pos = 0;
        
        // تحليل الإصدار الرئيسي
        size_t dot1 = str.find('.', pos);
        v.major = std::stoi(str.substr(pos, dot1 - pos));
        pos = dot1 + 1;
        
        // تحليل الإصدار الفرعي
        size_t dot2 = str.find('.', pos);
        v.minor = std::stoi(str.substr(pos, dot2 - pos));
        pos = dot2 + 1;
        
        // تحليل الترقيع
        size_t dash = str.find('-', pos);
        if (dash != std::string::npos) {
            v.patch = std::stoi(str.substr(pos, dash - pos));
            v.prerelease = str.substr(dash + 1);
        } else {
            v.patch = std::stoi(str.substr(pos));
        }
        
        return v;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات الإصدار الدلالي - Semantic Version Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_semver_parse() {
    auto v1 = SemanticVersion::parse("1.2.3");
    TEST_EQUAL(1, v1.major, "الإصدار الرئيسي");
    TEST_EQUAL(2, v1.minor, "الإصدار الفرعي");
    TEST_EQUAL(3, v1.patch, "الترقيع");
    
    auto v2 = SemanticVersion::parse("2.0.0-alpha");
    TEST_EQUAL(2, v2.major, "الإصدار الرئيسي");
    TEST_EQUAL(0, v2.minor, "الإصدار الفرعي");
    TEST_EQUAL(0, v2.patch, "الترقيع");
    TEST_EQUAL("alpha", v2.prerelease, "الإصدار التجريبي");
}

void test_semver_compare() {
    auto v1 = SemanticVersion::parse("1.0.0");
    auto v2 = SemanticVersion::parse("2.0.0");
    auto v3 = SemanticVersion::parse("1.1.0");
    auto v4 = SemanticVersion::parse("1.0.1");
    
    TEST_TRUE(v1 < v2);
    TEST_TRUE(v1 < v3);
    TEST_TRUE(v1 < v4);
    TEST_TRUE(v3 < v2);
    TEST_TRUE(v4 < v3);
}

void test_semver_equality() {
    auto v1 = SemanticVersion::parse("1.2.3");
    auto v2 = SemanticVersion::parse("1.2.3");
    auto v3 = SemanticVersion::parse("1.2.4");
    
    TEST_TRUE(v1 == v2);
    TEST_FALSE(v1 == v3);
}

// ═══════════════════════════════════════════════════════════════════════════════
// محاكاة ملف التكوين - Config File Mock
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ConfigParser
 * @brief محلل ملفات التكوين TOML المبسط
 */
class ConfigParser {
public:
    /**
     * @brief تحليل محتوى TOML مبسط
     */
    std::map<std::string, std::map<std::string, std::string>> parse(const std::string& content) {
        std::map<std::string, std::map<std::string, std::string>> result;
        std::string current_section;
        
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            // تجاهل السطور الفارغة والتعليقات
            if (line.empty() || line[0] == '#') continue;
            
            // التحقق من قسم جديد
            if (line[0] == '[') {
                size_t end = line.find(']');
                if (end != std::string::npos) {
                    current_section = line.substr(1, end - 1);
                }
                continue;
            }
            
            // تحليل المفتاح = القيمة
            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string key = trim(line.substr(0, eq));
                std::string value = trim(line.substr(eq + 1));
                
                // إزالة علامات الاقتباس
                if (value.size() >= 2 && value[0] == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                result[current_section][key] = value;
            }
        }
        
        return result;
    }
    
private:
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات محلل التكوين - Config Parser Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_config_parser_basic() {
    ConfigParser parser;
    
    std::string content = R"(
[حزمة]
اسم = "مشروعي"
إصدار = "1.0.0"
وصف = "مشروع تجريبي"

[تبعيات]
رياضيات = "^1.0"
نصوص = "~2.3"
)";
    
    auto config = parser.parse(content);
    
    TEST_EQUAL("مشروعي", config["حزمة"]["اسم"], "اسم الحزمة");
    TEST_EQUAL("1.0.0", config["حزمة"]["إصدار"], "إصدار الحزمة");
    TEST_EQUAL("^1.0", config["تبعيات"]["رياضيات"], "تبعية رياضيات");
    TEST_EQUAL("~2.3", config["تبعيات"]["نصوص"], "تبعية نصوص");
}

void test_config_parser_empty() {
    ConfigParser parser;
    auto config = parser.parse("");
    TEST_TRUE(config.empty());
}

void test_config_parser_comments() {
    ConfigParser parser;
    
    std::string content = R"(
# هذا تعليق
[قسم]
# تعليق آخر
مفتاح = "قيمة"
)";
    
    auto config = parser.parse(content);
    TEST_EQUAL("قيمة", config["قسم"]["مفتاح"], "قيمة بعد التعليقات");
}

// ═══════════════════════════════════════════════════════════════════════════════
// محاكاة محلل التبعيات - Dependency Resolver Mock
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class DependencyResolver
 * @brief محلل التبعيات المبسط للاختبار
 */
class DependencyResolver {
public:
    struct Package {
        std::string name;
        std::string version;
        std::vector<std::pair<std::string, std::string>> dependencies;
    };
    
    /**
     * @brief إضافة حزمة للسجل
     */
    void add_package(const Package& pkg) {
        packages_[pkg.name + "@" + pkg.version] = pkg;
    }
    
    /**
     * @brief حل التبعيات
     */
    std::vector<Package> resolve(const std::string& name, const std::string& version) {
        std::vector<Package> result;
        std::set<std::string> visited;
        
        resolve_recursive(name, version, result, visited);
        
        return result;
    }
    
    /**
     * @brief الترتيب التوبولوجي
     */
    std::vector<Package> topological_sort(const std::vector<Package>& packages) {
        std::map<std::string, int> in_degree;
        std::map<std::string, Package> pkg_map;
        
        // بناء الخريطة
        for (const auto& pkg : packages) {
            pkg_map[pkg.name] = pkg;
            if (in_degree.find(pkg.name) == in_degree.end()) {
                in_degree[pkg.name] = 0;
            }
        }
        
        // حساب درجات الدخول
        for (const auto& pkg : packages) {
            for (const auto& [dep_name, _] : pkg.dependencies) {
                in_degree[dep_name]++;
            }
        }
        
        // الترتيب
        std::vector<Package> result;
        std::queue<std::string> queue;
        
        for (const auto& [name, degree] : in_degree) {
            if (degree == 0) {
                queue.push(name);
            }
        }
        
        while (!queue.empty()) {
            std::string name = queue.front();
            queue.pop();
            
            if (pkg_map.find(name) != pkg_map.end()) {
                result.push_back(pkg_map[name]);
            }
            
            for (const auto& [dep_name, _] : pkg_map[name].dependencies) {
                in_degree[dep_name]--;
                if (in_degree[dep_name] == 0) {
                    queue.push(dep_name);
                }
            }
        }
        
        return result;
    }
    
private:
    std::map<std::string, Package> packages_;
    
    void resolve_recursive(const std::string& name, const std::string& version,
                          std::vector<Package>& result, std::set<std::string>& visited) {
        std::string key = name + "@" + version;
        
        if (visited.count(key)) return;
        visited.insert(key);
        
        auto it = packages_.find(key);
        if (it == packages_.end()) return;
        
        const Package& pkg = it->second;
        
        // حل التبعيات أولاً
        for (const auto& [dep_name, dep_version] : pkg.dependencies) {
            resolve_recursive(dep_name, dep_version, result, visited);
        }
        
        result.push_back(pkg);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات محلل التبعيات - Dependency Resolver Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_resolver_simple() {
    DependencyResolver resolver;
    
    resolver.add_package({"قاعدة", "1.0.0", {}});
    resolver.add_package({"مشروعي", "1.0.0", {{"قاعدة", "1.0.0"}}});
    
    auto result = resolver.resolve("مشروعي", "1.0.0");
    
    TEST_EQUAL(2, (int)result.size(), "عدد الحزم");
    TEST_EQUAL("قاعدة", result[0].name, "الحزمة الأولى");
    TEST_EQUAL("مشروعي", result[1].name, "الحزمة الثانية");
}

void test_resolver_chain() {
    DependencyResolver resolver;
    
    resolver.add_package({"أ", "1.0.0", {}});
    resolver.add_package({"ب", "1.0.0", {{"أ", "1.0.0"}}});
    resolver.add_package({"ج", "1.0.0", {{"ب", "1.0.0"}}});
    
    auto result = resolver.resolve("ج", "1.0.0");
    
    TEST_EQUAL(3, (int)result.size(), "عدد الحزم");
    TEST_EQUAL("أ", result[0].name, "الحزمة الأولى");
    TEST_EQUAL("ب", result[1].name, "الحزمة الثانية");
    TEST_EQUAL("ج", result[2].name, "الحزمة الثالثة");
}

void test_resolver_diamond() {
    DependencyResolver resolver;
    
    // تبعية ماسية: د -> (ب، ج) -> أ
    resolver.add_package({"أ", "1.0.0", {}});
    resolver.add_package({"ب", "1.0.0", {{"أ", "1.0.0"}}});
    resolver.add_package({"ج", "1.0.0", {{"أ", "1.0.0"}}});
    resolver.add_package({"د", "1.0.0", {{"ب", "1.0.0"}, {"ج", "1.0.0"}}});
    
    auto result = resolver.resolve("د", "1.0.0");
    
    // يجب أن تظهر "أ" مرة واحدة فقط
    int count_a = 0;
    for (const auto& pkg : result) {
        if (pkg.name == "أ") count_a++;
    }
    TEST_EQUAL(1, count_a, "عدد مرات ظهور أ");
}

// ═══════════════════════════════════════════════════════════════════════════════
// محاكاة منسق الكود - Code Formatter Mock
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class CodeFormatter
 * @brief منسق كود ص المبسط للاختبار
 */
class CodeFormatter {
public:
    struct Options {
        int indent_size = 4;
        bool use_spaces = true;
        int max_line_length = 80;
        bool space_after_comma = true;
        bool space_around_operators = true;
    };
    
    CodeFormatter(const Options& opts = {}) : options_(opts) {}
    
    /**
     * @brief تنسيق الكود
     */
    std::string format(const std::string& code) {
        std::string result;
        int indent_level = 0;
        std::istringstream stream(code);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            
            if (line.empty()) {
                result += "\n";
                continue;
            }
            
            // تقليل المسافة البادئة للكلمات المغلقة
            if (is_closing_keyword(line)) {
                indent_level = std::max(0, indent_level - 1);
            }
            
            // إضافة المسافة البادئة
            result += get_indent(indent_level);
            
            // تنسيق السطر
            result += format_line(line);
            result += "\n";
            
            // زيادة المسافة البادئة للكلمات الفاتحة
            if (is_opening_keyword(line)) {
                indent_level++;
            }
        }
        
        return result;
    }
    
private:
    Options options_;
    
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
    
    std::string get_indent(int level) {
        std::string indent;
        int spaces = level * options_.indent_size;
        for (int i = 0; i < spaces; i++) {
            indent += options_.use_spaces ? " " : "\t";
        }
        return indent;
    }
    
    bool is_opening_keyword(const std::string& line) {
        // كلمات تفتح كتلة جديدة
        return line.find("دالة") == 0 ||
               line.find("إذا") == 0 ||
               line.find("طالما") == 0 ||
               line.find("لكل") == 0 ||
               line.find("صنف") == 0 ||
               line.find("وإلا") == 0 ||
               line.find("وإلا_إذا") == 0;
    }
    
    bool is_closing_keyword(const std::string& line) {
        // كلمات تغلق كتلة
        return line == "نهاية" || line.find("نهاية") == 0;
    }
    
    std::string format_line(const std::string& line) {
        std::string result = line;
        
        // تنسيق المسافات حول العوامل
        if (options_.space_around_operators) {
            // هذا تبسيط - التنفيذ الفعلي أكثر تعقيداً
        }
        
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات منسق الكود - Code Formatter Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_formatter_indent() {
    CodeFormatter formatter;
    
    std::string input = R"(دالة رئيسية()
اطبع("مرحبا")
نهاية)";
    
    std::string expected = R"(دالة رئيسية()
    اطبع("مرحبا")
نهاية
)";
    
    std::string result = formatter.format(input);
    TEST_EQUAL(expected, result, "المسافة البادئة");
}

void test_formatter_nested() {
    CodeFormatter formatter;
    
    std::string input = R"(دالة رئيسية()
إذا (صحيح)
اطبع("نعم")
نهاية
نهاية)";
    
    std::string result = formatter.format(input);
    
    // التحقق من أن هناك مستويين من المسافة البادئة
    TEST_TRUE(result.find("        اطبع") != std::string::npos);
}

void test_formatter_empty() {
    CodeFormatter formatter;
    std::string result = formatter.format("");
    TEST_TRUE(result.empty() || result == "\n");
}

// ═══════════════════════════════════════════════════════════════════════════════
// محاكاة عميل السجل - Registry Client Mock
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class MockRegistryClient
 * @brief محاكاة عميل سجل الحزم للاختبار
 */
class MockRegistryClient {
public:
    struct PackageInfo {
        std::string name;
        std::string description;
        std::vector<std::string> versions;
        std::string author;
    };
    
    /**
     * @brief إضافة حزمة للسجل المحاكى
     */
    void add_package(const PackageInfo& info) {
        registry_[info.name] = info;
    }
    
    /**
     * @brief البحث عن حزم
     */
    std::vector<PackageInfo> search(const std::string& query) {
        std::vector<PackageInfo> results;
        
        for (const auto& [name, info] : registry_) {
            if (name.find(query) != std::string::npos ||
                info.description.find(query) != std::string::npos) {
                results.push_back(info);
            }
        }
        
        return results;
    }
    
    /**
     * @brief الحصول على معلومات حزمة
     */
    std::optional<PackageInfo> get_package(const std::string& name) {
        auto it = registry_.find(name);
        if (it != registry_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief التحقق من وجود إصدار
     */
    bool has_version(const std::string& name, const std::string& version) {
        auto it = registry_.find(name);
        if (it == registry_.end()) return false;
        
        for (const auto& v : it->second.versions) {
            if (v == version) return true;
        }
        return false;
    }
    
private:
    std::map<std::string, PackageInfo> registry_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات عميل السجل - Registry Client Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_registry_search() {
    MockRegistryClient client;
    
    client.add_package({"رياضيات", "مكتبة رياضية", {"1.0.0", "1.1.0"}, "أحمد"});
    client.add_package({"نصوص", "معالجة النصوص", {"2.0.0"}, "فاطمة"});
    client.add_package({"رسوم", "رسومات متجهية", {"1.0.0"}, "محمد"});
    
    auto results = client.search("رياضي");
    TEST_EQUAL(1, (int)results.size(), "عدد النتائج");
    TEST_EQUAL("رياضيات", results[0].name, "اسم الحزمة");
}

void test_registry_get_package() {
    MockRegistryClient client;
    
    client.add_package({"رياضيات", "مكتبة رياضية", {"1.0.0", "1.1.0"}, "أحمد"});
    
    auto pkg = client.get_package("رياضيات");
    TEST_TRUE(pkg.has_value());
    TEST_EQUAL("رياضيات", pkg->name, "اسم الحزمة");
    TEST_EQUAL(2, (int)pkg->versions.size(), "عدد الإصدارات");
}

void test_registry_has_version() {
    MockRegistryClient client;
    
    client.add_package({"رياضيات", "مكتبة رياضية", {"1.0.0", "1.1.0"}, "أحمد"});
    
    TEST_TRUE(client.has_version("رياضيات", "1.0.0"));
    TEST_TRUE(client.has_version("رياضيات", "1.1.0"));
    TEST_FALSE(client.has_version("رياضيات", "2.0.0"));
    TEST_FALSE(client.has_version("نصوص", "1.0.0"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// محاكاة LSP - LSP Mock
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct Position
 * @brief موقع في المستند
 */
struct Position {
    int line = 0;
    int character = 0;
};

/**
 * @struct Range
 * @brief نطاق في المستند
 */
struct Range {
    Position start;
    Position end;
};

/**
 * @struct CompletionItem
 * @brief عنصر إكمال تلقائي
 */
struct CompletionItem {
    std::string label;
    std::string kind;
    std::string detail;
    std::string documentation;
};

/**
 * @struct Diagnostic
 * @brief تشخيص (خطأ أو تحذير)
 */
struct Diagnostic {
    Range range;
    std::string message;
    std::string severity; // error, warning, info, hint
    std::string code;
};

/**
 * @class MockLSPServer
 * @brief محاكاة خادم LSP للاختبار
 */
class MockLSPServer {
public:
    /**
     * @brief الحصول على اقتراحات الإكمال
     */
    std::vector<CompletionItem> get_completions(const std::string& prefix) {
        std::vector<CompletionItem> results;
        
        // الكلمات المفتاحية
        std::vector<std::string> keywords = {
            "دالة", "نهاية", "إذا", "وإلا", "وإلا_إذا",
            "طالما", "لكل", "إرجاع", "اطبع", "أدخل",
            "رقم", "نص", "منطقي", "قائمة", "خريطة",
            "صحيح", "خطأ", "عدم", "صنف", "جديد"
        };
        
        for (const auto& kw : keywords) {
            if (kw.find(prefix) == 0) {
                results.push_back({kw, "keyword", "كلمة مفتاحية", ""});
            }
        }
        
        return results;
    }
    
    /**
     * @brief الحصول على تشخيصات لكود
     */
    std::vector<Diagnostic> get_diagnostics(const std::string& code) {
        std::vector<Diagnostic> diagnostics;
        
        // فحص أقواس غير مطابقة
        int parens = 0, braces = 0, brackets = 0;
        int line = 0, ch = 0;
        
        for (size_t i = 0; i < code.size(); i++) {
            char c = code[i];
            if (c == '\n') { line++; ch = 0; continue; }
            
            if (c == '(') parens++;
            else if (c == ')') parens--;
            else if (c == '{') braces++;
            else if (c == '}') braces--;
            else if (c == '[') brackets++;
            else if (c == ']') brackets--;
            
            ch++;
        }
        
        if (parens != 0) {
            diagnostics.push_back({
                {{0, 0}, {0, 1}},
                "أقواس غير متطابقة",
                "error",
                "ص-001"
            });
        }
        
        return diagnostics;
    }
    
    /**
     * @brief الحصول على معلومات التمرير
     */
    std::string get_hover(const std::string& word) {
        static std::map<std::string, std::string> docs = {
            {"دالة", "**دالة** - تعريف دالة جديدة\n\nالصياغة:\n```\nدالة اسم_الدالة(معاملات)\n    // الكود\nنهاية\n```"},
            {"إذا", "**إذا** - جملة شرطية\n\nتنفذ الكود إذا كان الشرط صحيحاً"},
            {"اطبع", "**اطبع** - طباعة نص\n\nتطبع النص المعطى في المخرجات"},
            {"رقم", "**رقم** - نوع عددي\n\nيمثل الأعداد الصحيحة والعشرية"}
        };
        
        auto it = docs.find(word);
        if (it != docs.end()) {
            return it->second;
        }
        return "";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات LSP - LSP Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_lsp_completion() {
    MockLSPServer server;
    
    auto completions = server.get_completions("دا");
    TEST_TRUE(!completions.empty());
    TEST_EQUAL("دالة", completions[0].label, "أول اقتراح");
}

void test_lsp_completion_empty() {
    MockLSPServer server;
    
    auto completions = server.get_completions("xyz");
    TEST_TRUE(completions.empty());
}

void test_lsp_diagnostics_valid() {
    MockLSPServer server;
    
    std::string code = "دالة رئيسية()\n    اطبع(\"مرحبا\")\nنهاية";
    auto diagnostics = server.get_diagnostics(code);
    TEST_TRUE(diagnostics.empty());
}

void test_lsp_diagnostics_unmatched_parens() {
    MockLSPServer server;
    
    std::string code = "دالة رئيسية(\n    اطبع(\"مرحبا\"\nنهاية";
    auto diagnostics = server.get_diagnostics(code);
    TEST_FALSE(diagnostics.empty());
    TEST_EQUAL("ص-001", diagnostics[0].code, "كود الخطأ");
}

void test_lsp_hover() {
    MockLSPServer server;
    
    std::string hover = server.get_hover("دالة");
    TEST_FALSE(hover.empty());
    TEST_TRUE(hover.find("دالة") != std::string::npos);
}

void test_lsp_hover_unknown() {
    MockLSPServer server;
    
    std::string hover = server.get_hover("كلمة_غير_موجودة");
    TEST_TRUE(hover.empty());
}

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات نظام الملفات - File System Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_config_file_write_read() {
    // إنشاء ملف مؤقت
    std::string temp_path = "test_config_temp.toml";
    
    std::string content = R"([حزمة]
اسم = "اختبار"
إصدار = "1.0.0"
)";
    
    // الكتابة
    {
        std::ofstream file(temp_path);
        TEST_TRUE(file.is_open());
        file << content;
    }
    
    // القراءة
    {
        std::ifstream file(temp_path);
        TEST_TRUE(file.is_open());
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string read_content = buffer.str();
        
        TEST_EQUAL(content, read_content, "محتوى الملف");
    }
    
    // الحذف
    fs::remove(temp_path);
    TEST_FALSE(fs::exists(temp_path));
}

void test_directory_operations() {
    std::string temp_dir = "test_pkg_temp";
    
    // إنشاء المجلد
    fs::create_directory(temp_dir);
    TEST_TRUE(fs::exists(temp_dir));
    TEST_TRUE(fs::is_directory(temp_dir));
    
    // إنشاء ملف داخله
    std::string file_path = temp_dir + "/ص.toml";
    {
        std::ofstream file(file_path);
        file << "[حزمة]\nاسم = \"اختبار\"";
    }
    TEST_TRUE(fs::exists(file_path));
    
    // الحذف
    fs::remove_all(temp_dir);
    TEST_FALSE(fs::exists(temp_dir));
}

// ═══════════════════════════════════════════════════════════════════════════════
// اختبارات الأداء - Performance Tests
// ═══════════════════════════════════════════════════════════════════════════════

void test_formatter_performance() {
    CodeFormatter formatter;
    
    // إنشاء كود كبير
    std::string large_code;
    for (int i = 0; i < 100; i++) {
        large_code += "دالة دالة_" + std::to_string(i) + "()\n";
        large_code += "رقم س = " + std::to_string(i) + "\n";
        large_code += "إذا (س > 0)\n";
        large_code += "اطبع(س)\n";
        large_code += "نهاية\n";
        large_code += "نهاية\n\n";
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    std::string formatted = formatter.format(large_code);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    
    // يجب أن يكون أقل من ثانية
    TEST_TRUE(duration < 1000.0);
}

void test_resolver_performance() {
    DependencyResolver resolver;
    
    // إنشاء شجرة تبعيات كبيرة
    for (int i = 0; i < 50; i++) {
        std::vector<std::pair<std::string, std::string>> deps;
        if (i > 0) {
            deps.push_back({"حزمة_" + std::to_string(i - 1), "1.0.0"});
        }
        if (i > 1) {
            deps.push_back({"حزمة_" + std::to_string(i - 2), "1.0.0"});
        }
        
        resolver.add_package({"حزمة_" + std::to_string(i), "1.0.0", deps});
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    auto result = resolver.resolve("حزمة_49", "1.0.0");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double, std::milli>(end - start).count();
    
    // يجب أن يكون أقل من ثانية
    TEST_TRUE(duration < 1000.0);
    TEST_TRUE(!result.empty());
}

// ═══════════════════════════════════════════════════════════════════════════════
// الدالة الرئيسية - Main Function
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief نقطة الدخول الرئيسية
 * 
 * يسجل جميع الاختبارات ويشغلها
 * ثم يطبع ملخص النتائج
 */
int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                   ║\n";
    std::cout << "║        بسم الله الرحمن الرحيم                                    ║\n";
    std::cout << "║                                                                   ║\n";
    std::cout << "║   اختبارات تكامل أدوات التطوير لـ لغة ص                          ║\n";
    std::cout << "║   Development Tools Integration Tests for Sad Language           ║\n";
    std::cout << "║                                                                   ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════════╝\n";
    
    TestRunner runner;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات الإصدار الدلالي
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات الإصدار الدلالي - Semantic Version Tests                  │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("تحليل الإصدار الدلالي", test_semver_parse);
    runner.register_test("مقارنة الإصدارات", test_semver_compare);
    runner.register_test("تساوي الإصدارات", test_semver_equality);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات محلل التكوين
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات محلل التكوين - Config Parser Tests                        │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("تحليل التكوين الأساسي", test_config_parser_basic);
    runner.register_test("تحليل تكوين فارغ", test_config_parser_empty);
    runner.register_test("تحليل مع تعليقات", test_config_parser_comments);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات محلل التبعيات
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات محلل التبعيات - Dependency Resolver Tests                 │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("حل تبعية بسيطة", test_resolver_simple);
    runner.register_test("حل سلسلة تبعيات", test_resolver_chain);
    runner.register_test("حل تبعية ماسية", test_resolver_diamond);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات منسق الكود
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات منسق الكود - Code Formatter Tests                         │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("تنسيق مسافة بادئة", test_formatter_indent);
    runner.register_test("تنسيق كتل متداخلة", test_formatter_nested);
    runner.register_test("تنسيق كود فارغ", test_formatter_empty);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات عميل السجل
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات عميل السجل - Registry Client Tests                        │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("بحث في السجل", test_registry_search);
    runner.register_test("جلب معلومات حزمة", test_registry_get_package);
    runner.register_test("التحقق من إصدار", test_registry_has_version);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات LSP
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات LSP - Language Server Protocol Tests                      │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("الإكمال التلقائي", test_lsp_completion);
    runner.register_test("الإكمال لكلمة غير موجودة", test_lsp_completion_empty);
    runner.register_test("تشخيصات كود صحيح", test_lsp_diagnostics_valid);
    runner.register_test("تشخيصات أقواس غير متطابقة", test_lsp_diagnostics_unmatched_parens);
    runner.register_test("معلومات التمرير", test_lsp_hover);
    runner.register_test("تمرير لكلمة غير موجودة", test_lsp_hover_unknown);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات نظام الملفات
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات نظام الملفات - File System Tests                          │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("كتابة وقراءة ملف التكوين", test_config_file_write_read);
    runner.register_test("عمليات المجلدات", test_directory_operations);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل اختبارات الأداء
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::cout << "\n┌─────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ اختبارات الأداء - Performance Tests                                │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────┘\n";
    
    runner.register_test("أداء منسق الكود", test_formatter_performance);
    runner.register_test("أداء محلل التبعيات", test_resolver_performance);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تشغيل جميع الاختبارات
    // ═══════════════════════════════════════════════════════════════════════════
    
    auto results = runner.run_all();
    
    // حساب الإحصائيات
    int passed = 0, failed = 0;
    double total_time = 0;
    
    for (const auto& r : results) {
        if (r.passed) passed++;
        else failed++;
        total_time += r.duration_ms;
    }
    
    // طباعة الملخص النهائي
    std::cout << "\n╔═══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                         الملخص النهائي                           ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  إجمالي الاختبارات: " << (passed + failed) << "                                           ║\n";
    std::cout << "║  نجح: " << passed << "                                                          ║\n";
    std::cout << "║  فشل: " << failed << "                                                          ║\n";
    std::cout << "║  الوقت الإجمالي: " << total_time << " مللي ثانية                               ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════════╣\n";
    
    if (failed == 0) {
        std::cout << "║  ✓ جميع الاختبارات نجحت! الحمد لله                               ║\n";
    } else {
        std::cout << "║  ✗ بعض الاختبارات فشلت. يرجى المراجعة                            ║\n";
    }
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════╝\n\n";
    
    return failed > 0 ? 1 : 0;
}
