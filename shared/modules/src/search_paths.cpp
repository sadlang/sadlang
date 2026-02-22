/**
 * @file search_paths.cpp
 * @brief (AR) تنفيذ مدير مسارات البحث
 *        (EN) Implementation of search path manager
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "search_paths.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace Sad {
namespace Modules {

// =========================================================================
// SearchPathManager Implementation
// =========================================================================

SearchPathManager& SearchPathManager::getInstance() {
    static SearchPathManager instance;
    return instance;
}

SearchPathManager::SearchPathManager() : paths_{} {
    initialize();
}

void SearchPathManager::initialize() {
    // إضافة المسارات الافتراضية
    // Add default paths
    addDefaultPaths();
    
    // تحميل من متغير البيئة
    // Load from environment variable
    loadFromEnvironment();
    
    // محاولة تحميل من ملف التكوين في الدليل الحالي
    // Try to load from config file in current directory
    if (std::filesystem::exists("sad.config.json")) {
        loadFromConfig("sad.config.json");
    }
}

void SearchPathManager::addPath(const std::string& path, int priority) {
    // التحقق من وجود المسار
    // Check if path exists
    if (!std::filesystem::exists(path)) {
        return;  // تجاهل المسارات غير الموجودة / Ignore non-existent paths
    }
    
    // التحقق من عدم التكرار
    // Check for duplicates
    for (const auto& entry : paths_) {
        if (entry.path == path) {
            return;  // المسار موجود بالفعل / Path already exists
        }
    }
    
    // إضافة المسار
    PathEntry entry;
    entry.path = std::filesystem::absolute(path).string();
    entry.priority = priority;
    paths_.push_back(entry);
    
    // إعادة الترتيب حسب الأولوية
    // Re-sort by priority
    std::sort(paths_.begin(), paths_.end());
}

void SearchPathManager::removePath(const std::string& path) {
    paths_.erase(
        std::remove_if(paths_.begin(), paths_.end(),
            [&path](const PathEntry& entry) { return entry.path == path; }),
        paths_.end()
    );
}

void SearchPathManager::clear() {
    paths_.clear();
}

std::vector<std::string> SearchPathManager::getPaths() const {
    std::vector<std::string> result;
    result.reserve(paths_.size());
    
    for (const auto& entry : paths_) {
        result.push_back(entry.path);
    }
    
    return result;
}

std::string SearchPathManager::findModule(const std::string& moduleName) const {
    // البحث في جميع المسارات حسب الأولوية
    // Search in all paths by priority
    for (const auto& entry : paths_) {
        std::string result = tryFindInPath(entry.path, moduleName);
        if (!result.empty()) {
            return result;
        }
    }
    
    return "";  // لم يُعثر على الوحدة / Module not found
}

void SearchPathManager::loadFromConfig(const std::string& configFile) {
    // TODO: تنفيذ تحليل JSON
    // For now, this is a placeholder
    // سيتم تنفيذه لاحقاً عند إضافة مكتبة JSON
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return;
    }
    
    // مؤقتاً: لا نفعل شيئاً
    // Temporarily: do nothing
    // نحتاج لمكتبة JSON parsing
}

void SearchPathManager::loadFromEnvironment() {
    const char* sadPath = std::getenv("SAD_PATH");
    if (sadPath != nullptr) {
        std::string pathsStr(sadPath);
        
        // تقسيم المسارات (مفصولة بـ : في Linux أو ; في Windows)
        // Split paths (separated by : on Linux or ; on Windows)
        #ifdef _WIN32
        const char delimiter = ';';
        #else
        const char delimiter = ':';
        #endif
        
        size_t start = 0;
        size_t end = pathsStr.find(delimiter);
        
        while (end != std::string::npos) {
            std::string path = pathsStr.substr(start, end - start);
            if (!path.empty()) {
                addPath(path, 40);  // أولوية متوسطة / Medium priority
            }
            start = end + 1;
            end = pathsStr.find(delimiter, start);
        }
        
        // المسار الأخير / Last path
        std::string path = pathsStr.substr(start);
        if (!path.empty()) {
            addPath(path, 40);
        }
    }
}

void SearchPathManager::addDefaultPaths() {
    // 1. الدليل الحالي (أولوية عالية جداً)
    // Current directory (very high priority)
    addPath(".", 200);
    
    // 2. دليل lib في المشروع
    // Project lib directory
    if (std::filesystem::exists("./lib")) {
        addPath("./lib", 150);
    }
    
    // 3. دليل modules في المشروع
    // Project modules directory
    if (std::filesystem::exists("./modules")) {
        addPath("./modules", 140);
    }
    
    // 4. المكتبة القياسية (من متغير البيئة SAD_HOME)
    // Standard library (from SAD_HOME environment variable)
    const char* sadHome = std::getenv("SAD_HOME");
    if (sadHome != nullptr) {
        std::string stdlibPath = std::string(sadHome) + "/stdlib";
        if (std::filesystem::exists(stdlibPath)) {
            addPath(stdlibPath, 100);
        }
    }
    
    // 5. مسارات النظام (أولوية منخفضة)
    // System paths (low priority)
    #ifdef _WIN32
    // Windows paths
    if (std::filesystem::exists("C:/Program Files/Sad/stdlib")) {
        addPath("C:/Program Files/Sad/stdlib", 20);
    }
    #else
    // Linux/Unix paths
    if (std::filesystem::exists("/usr/local/lib/sad/stdlib")) {
        addPath("/usr/local/lib/sad/stdlib", 20);
    }
    if (std::filesystem::exists("/usr/lib/sad/stdlib")) {
        addPath("/usr/lib/sad/stdlib", 10);
    }
    #endif
}

void SearchPathManager::printPaths() const {
    std::cout << "\n========================================\n";
    std::cout << "  Search Paths (مسارات البحث)\n";
    std::cout << "========================================\n";
    
    for (size_t i = 0; i < paths_.size(); ++i) {
        std::cout << "[" << i + 1 << "] "
                  << "Priority " << paths_[i].priority << ": "
                  << paths_[i].path << "\n";
    }
    
    std::cout << "========================================\n\n";
}

std::string SearchPathManager::tryFindInPath(const std::string& basePath, const std::string& moduleName) const {
    // تحويل اسم الوحدة إلى مسار
    // Convert module name to path
    std::string relativePath = moduleNameToPath(moduleName);
    
    // محاولات البحث:
    // Search attempts:
    
    // 1. ملف مباشر: basePath/module.ص
    std::string attempt1 = basePath + "/" + relativePath + ".\xd8\xb5";
    if (std::filesystem::exists(attempt1)) {
        return std::filesystem::absolute(attempt1).string();
    }
    
    // 2. دليل مع init.ص: basePath/module/init.ص
    std::string attempt2 = basePath + "/" + relativePath + "/init.\xd8\xb5";
    if (std::filesystem::exists(attempt2)) {
        return std::filesystem::absolute(attempt2).string();
    }
    
    // 3. دليل مع نفس الاسم: basePath/module/module.s
    size_t lastSlash = relativePath.find_last_of("/\\");
    std::string modulePart = (lastSlash != std::string::npos) 
        ? relativePath.substr(lastSlash + 1) 
        : relativePath;
    
    std::string attempt3 = basePath + "/" + relativePath + "/" + modulePart + ".\xd8\xb5";
    if (std::filesystem::exists(attempt3)) {
        return std::filesystem::absolute(attempt3).string();
    }
    
    return "";  // لم يُعثر / Not found
}

std::string SearchPathManager::moduleNameToPath(const std::string& moduleName) const {
    // تحويل "math.advanced.trig" إلى "math/advanced/trig"
    // Convert "math.advanced.trig" to "math/advanced/trig"
    std::string result = moduleName;
    std::replace(result.begin(), result.end(), '.', '/');
    return result;
}

} // namespace Modules
} // namespace Sad
