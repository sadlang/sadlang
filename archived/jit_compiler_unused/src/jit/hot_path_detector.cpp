// ============================================================================
// hot_path_detector.cpp - تنفيذ كاشف المسارات الساخنة
// Hot Path Detector Implementation
// ============================================================================

#include "jit/hot_path_detector.h"
#include <algorithm>    // لدوال الخوارزميات / For algorithms
#include <sstream>      // لمعالجة النصوص / For string processing
#include <iomanip>      // لتنسيق الإخراج / For output formatting
#include <iostream>     // للإخراج القياسي / For standard output
#include <fstream>      // للملفات / For file I/O

namespace Sad {
namespace JIT {

// ============================================================================
// المُنشئ / Constructor
// ============================================================================

HotPathDetector::HotPathDetector(const TieringConfig& config)
    : config_(config)              // حفظ الإعدادات / Store configuration
    , tiering_enabled_(true)       // التجميع المُدرّج مفعّل افتراضياً / Tiering enabled by default
    , total_executions_(0)         // صفر تنفيذات / Zero executions
    , hot_detections_(0)           // صفر اكتشافات / Zero detections
{
    // لا حاجة لتهيئة إضافية / No additional initialization needed
}

// ============================================================================
// المُدمر / Destructor
// ============================================================================

HotPathDetector::~HotPathDetector() {
    // تنظيف الموارد / Clean up resources
    callbacks_.clear();
    paths_.clear();
}

// ============================================================================
// تسجيل تنفيذ دالة / Record Function Execution
// ============================================================================

void HotPathDetector::recordExecution(const std::string& function_name, size_t execution_time_us) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // زيادة عداد التنفيذات الكلي / Increment total executions
    total_executions_++;
    
    // البحث عن معلومات المسار أو إنشاؤها / Find or create path info
    auto& info = paths_[function_name];
    
    // إذا كانت هذه أول مرة، تهيئة البيانات / If first time, initialize data
    if (info.function_name.empty()) {
        info.function_name = function_name;
        info.path_id = function_name; // افتراضياً، path_id = function_name / By default, path_id = function_name
    }
    
    // تسجيل التنفيذ / Record execution
    info.recordExecution(execution_time_us);
    
    // تحديث درجة الحرارة / Update temperature
    updateTemperature(info);
    
    // التحقق من العتبات / Check thresholds
    if (checkThreshold(info)) {
        // المسار أصبح ساخناً! / Path became hot!
        if (!info.is_hot) {
            info.is_hot = true;
            hot_detections_++;           // زيادة عداد الاكتشافات / Increment detection counter
            notifyHotPath(info);         // إشعار المستمعين / Notify listeners
        }
    }
}

// ============================================================================
// تسجيل تنفيذ مسار محدد / Record Specific Path Execution
// ============================================================================

void HotPathDetector::recordPathExecution(
    const std::string& function_name,
    const std::string& path_id,
    size_t execution_time_us
) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // إنشاء مفتاح فريد للمسار / Create unique key for path
    std::string key = function_name + "::" + path_id;
    
    // زيادة عداد التنفيذات الكلي / Increment total executions
    total_executions_++;
    
    // البحث عن معلومات المسار أو إنشاؤها / Find or create path info
    auto& info = paths_[key];
    
    // تهيئة البيانات إذا لزم الأمر / Initialize if needed
    if (info.function_name.empty()) {
        info.function_name = function_name;
        info.path_id = path_id;
    }
    
    // تسجيل التنفيذ / Record execution
    info.recordExecution(execution_time_us);
    
    // تحديث درجة الحرارة / Update temperature
    updateTemperature(info);
    
    // التحقق من العتبات / Check thresholds
    if (checkThreshold(info)) {
        if (!info.is_hot) {
            info.is_hot = true;
            hot_detections_++;
            notifyHotPath(info);
        }
    }
}

// ============================================================================
// هل الدالة ساخنة؟ / Is Function Hot?
// ============================================================================

bool HotPathDetector::isHot(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        return it->second.is_hot;
    }
    
    return false; // غير موجود = غير ساخن / Not found = not hot
}

// ============================================================================
// الحصول على معلومات المسار / Get Path Information
// ============================================================================

HotPathInfo* HotPathDetector::getPathInfo(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        return &(it->second);
    }
    
    return nullptr;
}

const HotPathInfo* HotPathDetector::getPathInfo(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        return &(it->second);
    }
    
    return nullptr;
}

// ============================================================================
// الحصول على جميع المسارات الساخنة / Get All Hot Paths
// ============================================================================

std::vector<HotPathInfo> HotPathDetector::getHotPaths() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::vector<HotPathInfo> hot_paths;
    
    for (const auto& pair : paths_) {
        if (pair.second.is_hot) {
            hot_paths.push_back(pair.second);
        }
    }
    
    // ترتيب حسب درجة الحرارة / Sort by temperature
    std::sort(hot_paths.begin(), hot_paths.end(),
        [](const HotPathInfo& a, const HotPathInfo& b) {
            return a.temperature_level > b.temperature_level;
        });
    
    return hot_paths;
}

// ============================================================================
// الحصول على عدد التنفيذات / Get Execution Count
// ============================================================================

size_t HotPathDetector::getExecutionCount(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        return it->second.execution_count;
    }
    
    return 0;
}

// ============================================================================
// الحصول على مستوى الحرارة / Get Temperature Level
// ============================================================================

int HotPathDetector::getTemperature(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        return it->second.temperature_level;
    }
    
    return 0;
}

// ============================================================================
// الحصول على المستوى الموصى به / Get Recommended Tier Level
// ============================================================================

int HotPathDetector::getRecommendedTier(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        return calculateRecommendedTier(it->second);
    }
    
    return 0; // المفسر افتراضياً / Interpreter by default
}

// ============================================================================
// هل يجب ترقية المستوى؟ / Should Upgrade Tier?
// ============================================================================

bool HotPathDetector::shouldUpgradeTier(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!tiering_enabled_) return false; // التجميع المُدرّج معطل / Tiering disabled
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        const HotPathInfo& info = it->second;
        int recommended_tier = calculateRecommendedTier(info);
        
        // ترقية إذا كان المستوى الموصى به أعلى من الحالي / Upgrade if recommended tier is higher than current
        return recommended_tier > info.tier_level;
    }
    
    return false;
}

// ============================================================================
// تحديث مستوى التجميع / Update Compilation Tier
// ============================================================================

void HotPathDetector::updateCompilationTier(const std::string& function_name, int tier_level) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = paths_.find(function_name);
    if (it != paths_.end()) {
        it->second.tier_level = tier_level;             // تحديث المستوى / Update tier
        it->second.is_compiled = true;                  // وضع علامة التجميع / Mark as compiled
        it->second.compilation_level = tier_level;      // حفظ مستوى التحسين / Save optimization level
        it->second.compiled_at = std::chrono::steady_clock::now(); // وقت التجميع / Compilation time
    }
}

// ============================================================================
// تسجيل callback للمسارات الساخنة / Register Callback for Hot Paths
// ============================================================================

void HotPathDetector::onHotPath(HotPathCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    callbacks_.push_back(callback);
}

// ============================================================================
// إلغاء تسجيل callbacks / Unregister Callbacks
// ============================================================================

void HotPathDetector::clearCallbacks() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    callbacks_.clear();
}

// ============================================================================
// الحصول على الإعدادات الحالية / Get Current Configuration
// ============================================================================

const TieringConfig& HotPathDetector::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return config_;
}

// ============================================================================
// تحديث الإعدادات / Update Configuration
// ============================================================================

void HotPathDetector::updateConfig(const TieringConfig& new_config) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    config_ = new_config;
}

// ============================================================================
// تفعيل/تعطيل التجميع المُدرّج / Enable/Disable Tiered Compilation
// ============================================================================

void HotPathDetector::setTieringEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    tiering_enabled_ = enabled;
}

// ============================================================================
// هل التجميع المُدرّج مفعّل؟ / Is Tiered Compilation Enabled?
// ============================================================================

bool HotPathDetector::isTieringEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return tiering_enabled_;
}

// ============================================================================
// عدد المسارات المُراقبة / Number of Tracked Paths
// ============================================================================

size_t HotPathDetector::getTrackedPathsCount() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return paths_.size();
}

// ============================================================================
// عدد المسارات الساخنة / Number of Hot Paths
// ============================================================================

size_t HotPathDetector::getHotPathsCount() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    size_t count = 0;
    for (const auto& pair : paths_) {
        if (pair.second.is_hot) {
            count++;
        }
    }
    
    return count;
}

// ============================================================================
// معدل اكتشاف المسارات الساخنة / Hot Path Detection Rate
// ============================================================================

double HotPathDetector::getHotPathRate() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (paths_.empty()) return 0.0;
    
    return (static_cast<double>(getHotPathsCount()) / paths_.size()) * 100.0;
}

// ============================================================================
// طباعة الإحصائيات / Print Statistics
// ============================================================================

void HotPathDetector::printStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::cout << "\n========================================\n";
    std::cout << " Hot Path Detector Statistics\n";
    std::cout << " إحصائيات كاشف المسارات الساخنة\n";
    std::cout << "========================================\n";
    std::cout << "Total Executions:  " << total_executions_ << "\n";
    std::cout << "Tracked Paths:     " << paths_.size() << "\n";
    std::cout << "Hot Paths:         " << getHotPathsCount() << "\n";
    std::cout << "Hot Path Rate:     " << std::fixed << std::setprecision(2) 
              << getHotPathRate() << "%\n";
    std::cout << "Hot Detections:    " << hot_detections_ << "\n";
    std::cout << "Tiering:           " << (tiering_enabled_ ? "Enabled" : "Disabled") << "\n";
    std::cout << "========================================\n\n";
}

// ============================================================================
// إعادة تعيين الإحصائيات / Reset Statistics
// ============================================================================

void HotPathDetector::reset() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    paths_.clear();          // مسح جميع المسارات / Clear all paths
    total_executions_ = 0;   // إعادة تعيين التنفيذات / Reset executions
    hot_detections_ = 0;     // إعادة تعيين الاكتشافات / Reset detections
}

// ============================================================================
// تصدير إلى JSON / Export to JSON
// ============================================================================

std::string HotPathDetector::toJSON() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"total_executions\": " << total_executions_ << ",\n";
    json << "  \"tracked_paths\": " << paths_.size() << ",\n";
    json << "  \"hot_paths\": " << getHotPathsCount() << ",\n";
    json << "  \"hot_path_rate_percent\": " << getHotPathRate() << ",\n";
    json << "  \"hot_detections\": " << hot_detections_ << ",\n";
    json << "  \"tiering_enabled\": " << (tiering_enabled_ ? "true" : "false") << "\n";
    json << "}";
    
    return json.str();
}

// ============================================================================
// حفظ إلى ملف / Save to File
// ============================================================================

bool HotPathDetector::saveToFile(const std::string& file_path) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJSON();
    file.close();
    
    return true;
}

// ============================================================================
// تحميل من ملف / Load from File
// ============================================================================

bool HotPathDetector::loadFromFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    // ملاحظة: تحميل JSON يحتاج parser - للتبسيط، نتركه للمستقبل / Note: JSON parsing needs parser - simplified for now
    file.close();
    
    return true;
}

// ============================================================================
// الدوال المساعدة الداخلية / Internal Helper Functions
// ============================================================================

// تحديث درجة الحرارة / Update Temperature
void HotPathDetector::updateTemperature(HotPathInfo& info) {
    // حساب درجة الحرارة بناءً على التنفيذات والوقت / Calculate temperature based on executions and time
    info.temperature_level = info.calculateTemperature();
}

// التحقق من العتبات / Check Thresholds
bool HotPathDetector::checkThreshold(const HotPathInfo& info) const {
    // التحقق من تجاوز عتبة السخونة / Check if exceeds hot threshold
    return info.execution_count >= config_.tier1_threshold && 
           info.temperature_level >= 50; // درجة حرارة 50+ تُعتبر ساخنة / Temperature 50+ is hot
}

// إشعار المستمعين / Notify Listeners
void HotPathDetector::notifyHotPath(const HotPathInfo& info) {
    // استدعاء جميع callbacks المسجلة / Call all registered callbacks
    for (const auto& callback : callbacks_) {
        try {
            callback(info);
        } catch (...) {
            // تجاهل الأخطاء في callbacks / Ignore callback errors
        }
    }
}

// حساب المستوى الموصى به / Calculate Recommended Tier
int HotPathDetector::calculateRecommendedTier(const HotPathInfo& info) const {
    // حساب المستوى بناءً على عدد التنفيذات / Calculate tier based on execution count
    size_t exec_count = info.execution_count;
    
    if (exec_count >= config_.tier4_threshold) {
        return 4; // O3 + PGO
    } else if (exec_count >= config_.tier3_threshold) {
        return 3; // O2
    } else if (exec_count >= config_.tier2_threshold) {
        return 2; // O1
    } else if (exec_count >= config_.tier1_threshold) {
        return 1; // O0
    } else {
        return 0; // المفسر / Interpreter
    }
}

} // namespace JIT
} // namespace Sad

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
