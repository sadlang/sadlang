/**
 * =============================================================================
 * @file test_discovery.h
 * @brief (AR) اكتشاف الاختبارات تلقائياً
 * @brief (EN) Automatic Test Discovery
 * 
 * المهمة: T255 - Test Discovery
 * المرحلة: سبتمبر 2026 — Test Framework
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ اكتشاف ملفات الاختبار (.اختبار.ص)
 * ✓ اكتشاف دوال الاختبار (اختبر_*)
 * ✓ تصفية الاختبارات بالاسم
 * ✓ تشغيل اختبارات محددة
 * ✓ تحميل الاختبارات من مجلد
 * 
 * =============================================================================
 */

#ifndef SAD_STDLIB_TEST_DISCOVERY_H
#define SAD_STDLIB_TEST_DISCOVERY_H

#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>

namespace sad {
namespace test {

namespace fs = std::filesystem;

// =============================================================================
// أنماط الاكتشاف
// =============================================================================

/// قواعد اكتشاف الاختبارات
struct DiscoveryRules {
    /// أنماط أسماء الملفات
    std::vector<std::string> filePatterns = {
        R"(.*\.اختبار\.ص$)",
        R"(.*_test\.ص$)",
        R"(test_.*\.ص$)",
        R"(.*\.test\.sad$)"
    };
    
    /// أنماط أسماء الدوال
    std::vector<std::string> functionPatterns = {
        R"(اختبر_.*)",
        R"(test_.*)",
        R"(اختبار_.*)"
    };
    
    /// مجلدات يتم تجاهلها
    std::vector<std::string> ignoreDirs = {
        "node_modules",
        ".git",
        "build",
        "dist",
        "__pycache__"
    };
    
    /// هل نبحث بشكل متكرر؟
    bool recursive = true;
};

// =============================================================================
// معلومات الاختبار المكتشف
// =============================================================================

/// ملف اختبار مكتشف
struct DiscoveredFile {
    std::string path;           // المسار الكامل
    std::string relativePath;   // المسار النسبي
    std::string name;           // اسم الملف
    std::vector<std::string> testFunctions;  // الدوال المكتشفة
    int lineCount = 0;          // عدد الأسطر
};

/// اختبار مكتشف
struct DiscoveredTest {
    std::string file;           // الملف
    std::string name;           // اسم الاختبار
    int line = 0;               // السطر
    std::string suite;          // المجموعة (من اسم الملف)
    std::vector<std::string> tags;  // التصنيفات
};

// =============================================================================
// مكتشف الاختبارات
// =============================================================================

class TestDiscovery {
public:
    // ─────────────────────────────────────────────
    // الخصائص
    // ─────────────────────────────────────────────
    
    DiscoveryRules rules;
    
    // ─────────────────────────────────────────────
    // اكتشاف الملفات
    // ─────────────────────────────────────────────
    
    /**
     * اكتشاف ملفات الاختبار في مجلد
     */
    std::vector<DiscoveredFile> discoverFiles(const std::string& directory) {
        std::vector<DiscoveredFile> files;
        
        if (!fs::exists(directory)) {
            return files;
        }
        
        auto iterator = rules.recursive 
            ? fs::recursive_directory_iterator(directory)
            : fs::recursive_directory_iterator(directory, fs::directory_options::none);
        
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;
            
            std::string path = entry.path().string();
            std::string parentDir = entry.path().parent_path().filename().string();
            
            // تجاهل المجلدات المستثناة
            if (shouldIgnore(parentDir)) continue;
            
            // التحقق من نمط الملف
            if (matchesFilePattern(path)) {
                DiscoveredFile file;
                file.path = path;
                file.relativePath = fs::relative(entry.path(), directory).string();
                file.name = entry.path().filename().string();
                file.lineCount = countLines(path);
                file.testFunctions = discoverFunctionsInFile(path);
                
                files.push_back(file);
            }
        }
        
        return files;
    }
    
    /**
     * اكتشاف الاختبارات في ملف واحد
     */
    std::vector<DiscoveredTest> discoverTests(const std::string& filePath) {
        std::vector<DiscoveredTest> tests;
        
        std::ifstream file(filePath);
        if (!file) return tests;
        
        std::string line;
        int lineNumber = 0;
        std::string currentSuite = fs::path(filePath).stem().string();
        
        // أنماط البحث
        std::regex testPattern(R"(اختبر\s+[\"']([^\"']+)[\"'])");
        std::regex funcPattern(R"(دالة\s+(اختبر_\w+|test_\w+))");
        std::regex suitePattern(R"(مجموعة\s+[\"']([^\"']+)[\"'])");
        std::regex tagPattern(R"(@(\w+))");
        
        std::vector<std::string> pendingTags;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // البحث عن tags
            std::smatch tagMatch;
            std::string::const_iterator searchStart(line.cbegin());
            while (std::regex_search(searchStart, line.cend(), tagMatch, tagPattern)) {
                pendingTags.push_back(tagMatch[1].str());
                searchStart = tagMatch.suffix().first;
            }
            
            // البحث عن اختبر "..."
            std::smatch testMatch;
            if (std::regex_search(line, testMatch, testPattern)) {
                DiscoveredTest test;
                test.file = filePath;
                test.name = testMatch[1].str();
                test.line = lineNumber;
                test.suite = currentSuite;
                test.tags = pendingTags;
                tests.push_back(test);
                pendingTags.clear();
            }
            
            // البحث عن دالة اختبر_...
            std::smatch funcMatch;
            if (std::regex_search(line, funcMatch, funcPattern)) {
                DiscoveredTest test;
                test.file = filePath;
                test.name = funcMatch[1].str();
                test.line = lineNumber;
                test.suite = currentSuite;
                test.tags = pendingTags;
                tests.push_back(test);
                pendingTags.clear();
            }
            
            // البحث عن تغيير المجموعة
            std::smatch suiteMatch;
            if (std::regex_search(line, suiteMatch, suitePattern)) {
                currentSuite = suiteMatch[1].str();
            }
        }
        
        return tests;
    }
    
    /**
     * اكتشاف جميع الاختبارات في مجلد
     */
    std::vector<DiscoveredTest> discoverAllTests(const std::string& directory) {
        std::vector<DiscoveredTest> allTests;
        
        auto files = discoverFiles(directory);
        for (const auto& file : files) {
            auto tests = discoverTests(file.path);
            allTests.insert(allTests.end(), tests.begin(), tests.end());
        }
        
        return allTests;
    }
    
    // ─────────────────────────────────────────────
    // التصفية
    // ─────────────────────────────────────────────
    
    /**
     * تصفية الاختبارات بنمط الاسم
     */
    std::vector<DiscoveredTest> filterByName(
        const std::vector<DiscoveredTest>& tests,
        const std::string& pattern
    ) {
        std::vector<DiscoveredTest> filtered;
        std::regex regex(pattern, std::regex::icase);
        
        for (const auto& test : tests) {
            if (std::regex_search(test.name, regex)) {
                filtered.push_back(test);
            }
        }
        
        return filtered;
    }
    
    /**
     * تصفية الاختبارات بالتصنيف
     */
    std::vector<DiscoveredTest> filterByTag(
        const std::vector<DiscoveredTest>& tests,
        const std::string& tag
    ) {
        std::vector<DiscoveredTest> filtered;
        
        for (const auto& test : tests) {
            for (const auto& t : test.tags) {
                if (t == tag) {
                    filtered.push_back(test);
                    break;
                }
            }
        }
        
        return filtered;
    }
    
    /**
     * تصفية الاختبارات بالمجموعة
     */
    std::vector<DiscoveredTest> filterBySuite(
        const std::vector<DiscoveredTest>& tests,
        const std::string& suite
    ) {
        std::vector<DiscoveredTest> filtered;
        
        for (const auto& test : tests) {
            if (test.suite == suite) {
                filtered.push_back(test);
            }
        }
        
        return filtered;
    }
    
    // ─────────────────────────────────────────────
    // الإحصائيات
    // ─────────────────────────────────────────────
    
    /**
     * إحصائيات الاكتشاف
     */
    struct DiscoveryStats {
        int totalFiles = 0;
        int totalTests = 0;
        int totalLines = 0;
        std::map<std::string, int> testsBySuite;
        std::map<std::string, int> testsByTag;
    };
    
    DiscoveryStats getStats(const std::vector<DiscoveredFile>& files,
                           const std::vector<DiscoveredTest>& tests) {
        DiscoveryStats stats;
        
        stats.totalFiles = files.size();
        stats.totalTests = tests.size();
        
        for (const auto& file : files) {
            stats.totalLines += file.lineCount;
        }
        
        for (const auto& test : tests) {
            stats.testsBySuite[test.suite]++;
            for (const auto& tag : test.tags) {
                stats.testsByTag[tag]++;
            }
        }
        
        return stats;
    }

private:
    bool shouldIgnore(const std::string& dirName) {
        for (const auto& ignore : rules.ignoreDirs) {
            if (dirName == ignore) return true;
        }
        return false;
    }
    
    bool matchesFilePattern(const std::string& path) {
        for (const auto& pattern : rules.filePatterns) {
            std::regex regex(pattern, std::regex::icase);
            if (std::regex_search(path, regex)) {
                return true;
            }
        }
        return false;
    }
    
    int countLines(const std::string& path) {
        std::ifstream file(path);
        return std::count(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>(),
            '\n'
        ) + 1;
    }
    
    std::vector<std::string> discoverFunctionsInFile(const std::string& path) {
        std::vector<std::string> functions;
        std::ifstream file(path);
        if (!file) return functions;
        
        std::string line;
        for (const auto& pattern : rules.functionPatterns) {
            std::regex regex(pattern);
            while (std::getline(file, line)) {
                std::smatch match;
                if (std::regex_search(line, match, regex)) {
                    functions.push_back(match[0].str());
                }
            }
            file.clear();
            file.seekg(0);
        }
        
        return functions;
    }
};

// =============================================================================
// دوال مساعدة
// =============================================================================

/**
 * اكتشاف الاختبارات في مجلد
 */
inline std::vector<DiscoveredTest> اكتشف_الاختبارات(const std::string& مجلد) {
    TestDiscovery discovery;
    return discovery.discoverAllTests(مجلد);
}

/**
 * عدد الاختبارات
 */
inline int عدد_الاختبارات(const std::string& مجلد) {
    return اكتشف_الاختبارات(مجلد).size();
}

/**
 * طباعة الاختبارات المكتشفة
 */
inline void اطبع_الاختبارات(const std::vector<DiscoveredTest>& tests) {
    std::cout << "═══════════════════════════════════════════════════\n";
    std::cout << "  📋 الاختبارات المكتشفة: " << tests.size() << "\n";
    std::cout << "═══════════════════════════════════════════════════\n";
    
    std::string currentSuite;
    for (const auto& test : tests) {
        if (test.suite != currentSuite) {
            currentSuite = test.suite;
            std::cout << "\n📁 " << currentSuite << "\n";
        }
        std::cout << "   • " << test.name << " (سطر " << test.line << ")\n";
    }
    std::cout << "\n";
}

} // namespace test
} // namespace sad

#endif // SAD_STDLIB_TEST_DISCOVERY_H
