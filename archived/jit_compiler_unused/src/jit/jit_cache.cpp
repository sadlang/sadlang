// ============================================================================
// jit_cache.cpp - تنفيذ نظام الذاكرة المؤقتة لمحرك JIT
// JIT Cache System Implementation
// ============================================================================

#include "jit/jit_cache.h"
#include <algorithm>    // لدوال الخوارزميات / For algorithms
#include <sstream>      // لمعالجة النصوص / For string processing
#include <iomanip>      // لتنسيق الإخراج / For output formatting
#include <iostream>     // للإخراج القياسي / For standard output
#include <ctime>        // للوقت / For time operations

namespace Sad {
namespace JIT {

// ============================================================================
// المُنشئ / Constructor
// ============================================================================

JITCache::JITCache(size_t max_size_mb, CacheEvictionPolicy eviction_policy)
    : max_size_bytes_(max_size_mb * 1024 * 1024)  // تحويل MB إلى bytes / Convert MB to bytes
    , current_size_bytes_(0)                       // البداية بذاكرة فارغة / Start with empty cache
    , eviction_policy_(eviction_policy)            // حفظ سياسة الإخلاء / Store eviction policy
    , hit_count_(0)                                // صفر إصابات / Zero hits
    , miss_count_(0)                               // صفر إخفاقات / Zero misses
    , eviction_count_(0)                           // صفر إخلاءات / Zero evictions
{
    // لا حاجة لتهيئة إضافية / No additional initialization needed
}

// ============================================================================
// المُدمر / Destructor
// ============================================================================

JITCache::~JITCache() {
    // تنظيف الذاكرة المؤقتة / Clean up cache
    clear();
}

// ============================================================================
// إضافة عنصر إلى الذاكرة المؤقتة / Add Entry to Cache
// ============================================================================

bool JITCache::put(const CacheEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // التحقق من وجود العنصر مسبقاً / Check if entry already exists
    auto it = cache_.find(entry.function_name);
    if (it != cache_.end()) {
        // تحديث العنصر الموجود / Update existing entry
        current_size_bytes_ -= it->second.code_size_bytes; // طرح الحجم القديم / Subtract old size
        it->second = entry;                                 // استبدال بالجديد / Replace with new
        current_size_bytes_ += entry.code_size_bytes;       // إضافة الحجم الجديد / Add new size
        return true;
    }
    
    // التحقق من المساحة المتاحة / Check if space is available
    if (current_size_bytes_ + entry.code_size_bytes > max_size_bytes_) {
        // محاولة إخلاء مساحة / Try to evict space
        if (!evict(entry.code_size_bytes)) {
            return false; // فشل الإخلاء / Eviction failed
        }
    }
    
    // إضافة العنصر الجديد / Add new entry
    cache_[entry.function_name] = entry;
    current_size_bytes_ += entry.code_size_bytes; // تحديث الحجم الكلي / Update total size
    
    return true;
}

// ============================================================================
// البحث عن عنصر / Lookup Entry
// ============================================================================

CacheEntry* JITCache::get(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = cache_.find(function_name);
    if (it != cache_.end()) {
        // إصابة! / Hit!
        hit_count_++;                    // زيادة عداد الإصابات / Increment hit counter
        it->second.recordAccess();       // تسجيل الوصول / Record access
        return &(it->second);            // إرجاع مؤشر للعنصر / Return pointer to entry
    }
    
    // إخفاق / Miss
    miss_count_++; // زيادة عداد الإخفاقات / Increment miss counter
    return nullptr;
}

// ============================================================================
// البحث عن عنصر (ثابت) / Lookup Entry (Const)
// ============================================================================

const CacheEntry* JITCache::get(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = cache_.find(function_name);
    if (it != cache_.end()) {
        // إصابة! / Hit!
        const_cast<size_t&>(hit_count_)++; // زيادة عداد الإصابات (const_cast ضروري) / Increment hit counter
        return &(it->second);               // إرجاع مؤشر ثابت للعنصر / Return const pointer to entry
    }
    
    // إخفاق / Miss
    const_cast<size_t&>(miss_count_)++; // زيادة عداد الإخفاقات / Increment miss counter
    return nullptr;
}

// ============================================================================
// هل العنصر موجود؟ / Does Entry Exist?
// ============================================================================

bool JITCache::contains(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return cache_.find(function_name) != cache_.end();
}

// ============================================================================
// حذف عنصر / Remove Entry
// ============================================================================

bool JITCache::remove(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = cache_.find(function_name);
    if (it != cache_.end()) {
        current_size_bytes_ -= it->second.code_size_bytes; // طرح حجم العنصر / Subtract entry size
        cache_.erase(it);                                   // حذف من الخريطة / Remove from map
        return true;
    }
    
    return false; // العنصر غير موجود / Entry not found
}

// ============================================================================
// مسح الذاكرة المؤقتة بالكامل / Clear Entire Cache
// ============================================================================

void JITCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    cache_.clear();            // مسح الخريطة / Clear map
    current_size_bytes_ = 0;   // إعادة تعيين الحجم / Reset size
}

// ============================================================================
// الحصول على الحجم الحالي / Get Current Size
// ============================================================================

size_t JITCache::getCurrentSize() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return current_size_bytes_;
}

// ============================================================================
// الحصول على عدد العناصر / Get Number of Entries
// ============================================================================

size_t JITCache::getCount() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return cache_.size();
}

// ============================================================================
// هل الذاكرة المؤقتة ممتلئة؟ / Is Cache Full?
// ============================================================================

bool JITCache::isFull() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return current_size_bytes_ >= max_size_bytes_;
}

// ============================================================================
// الحصول على نسبة الاستخدام / Get Usage Percentage
// ============================================================================

double JITCache::getUsagePercentage() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (max_size_bytes_ == 0) return 0.0;
    return (static_cast<double>(current_size_bytes_) / max_size_bytes_) * 100.0;
}

// ============================================================================
// تطبيق سياسة الإخلاء / Apply Eviction Policy
// ============================================================================

bool JITCache::evict(size_t required_space) {
    // ملاحظة: يُفترض أن mutex مقفول من الدالة المُستدعية / Note: Assumes mutex is locked by caller
    
    // إخلاء حتى توفر المساحة المطلوبة / Evict until required space is available
    while (current_size_bytes_ + required_space > max_size_bytes_) {
        if (!evictOne()) {
            return false; // فشل الإخلاء / Eviction failed
        }
    }
    
    return true;
}

// ============================================================================
// إخلاء عنصر واحد / Evict Single Entry
// ============================================================================

bool JITCache::evictOne() {
    // ملاحظة: يُفترض أن mutex مقفول من الدالة المُستدعية / Note: Assumes mutex is locked by caller
    
    if (cache_.empty()) {
        return false; // لا يوجد ما يُخلى / Nothing to evict
    }
    
    // اختيار الضحية بناءً على السياسة / Select victim based on policy
    std::string victim = selectVictim();
    
    if (victim.empty()) {
        return false; // فشل الاختيار / Selection failed
    }
    
    // حذف الضحية / Remove victim
    auto it = cache_.find(victim);
    if (it != cache_.end()) {
        current_size_bytes_ -= it->second.code_size_bytes; // طرح الحجم / Subtract size
        cache_.erase(it);                                   // حذف / Remove
        eviction_count_++;                                  // زيادة عداد الإخلاءات / Increment eviction counter
        return true;
    }
    
    return false;
}

// ============================================================================
// تغيير سياسة الإخلاء / Change Eviction Policy
// ============================================================================

void JITCache::setEvictionPolicy(CacheEvictionPolicy policy) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    eviction_policy_ = policy;
}

// ============================================================================
// الحصول على سياسة الإخلاء الحالية / Get Current Eviction Policy
// ============================================================================

CacheEvictionPolicy JITCache::getEvictionPolicy() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return eviction_policy_;
}

// ============================================================================
// نسبة الإصابة / Hit Rate
// ============================================================================

double JITCache::getHitRate() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    size_t total_accesses = hit_count_ + miss_count_;
    if (total_accesses == 0) return 0.0;
    
    return (static_cast<double>(hit_count_) / total_accesses) * 100.0;
}

// ============================================================================
// إعادة تعيين الإحصائيات / Reset Statistics
// ============================================================================

void JITCache::resetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    hit_count_ = 0;       // إعادة تعيين الإصابات / Reset hits
    miss_count_ = 0;      // إعادة تعيين الإخفاقات / Reset misses
    eviction_count_ = 0;  // إعادة تعيين الإخلاءات / Reset evictions
}

// ============================================================================
// حفظ إلى القرص / Save to Disk
// ============================================================================

bool JITCache::saveToDisk(const std::string& file_path) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return false; // فشل فتح الملف / Failed to open file
    }
    
    // كتابة عدد العناصر / Write number of entries
    size_t count = cache_.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // كتابة كل عنصر / Write each entry
    for (const auto& pair : cache_) {
        const CacheEntry& entry = pair.second;
        
        // كتابة اسم الدالة / Write function name
        size_t name_len = entry.function_name.size();
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(entry.function_name.data(), name_len);
        
        // كتابة الحجم / Write size
        file.write(reinterpret_cast<const char*>(&entry.code_size_bytes), sizeof(entry.code_size_bytes));
        
        // كتابة الإحصائيات / Write statistics
        file.write(reinterpret_cast<const char*>(&entry.hit_count), sizeof(entry.hit_count));
        file.write(reinterpret_cast<const char*>(&entry.execution_count), sizeof(entry.execution_count));
        
        // ملاحظة: لا نحفظ المؤشرات للكود المُجمّع / Note: Don't save compiled code pointers
    }
    
    file.close();
    return true;
}

// ============================================================================
// تحميل من القرص / Load from Disk
// ============================================================================

bool JITCache::loadFromDisk(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return false; // فشل فتح الملف / Failed to open file
    }
    
    // قراءة عدد العناصر / Read number of entries
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    // قراءة كل عنصر / Read each entry
    for (size_t i = 0; i < count; i++) {
        CacheEntry entry;
        
        // قراءة اسم الدالة / Read function name
        size_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        entry.function_name.resize(name_len);
        file.read(&entry.function_name[0], name_len);
        
        // قراءة الحجم / Read size
        file.read(reinterpret_cast<char*>(&entry.code_size_bytes), sizeof(entry.code_size_bytes));
        
        // قراءة الإحصائيات / Read statistics
        file.read(reinterpret_cast<char*>(&entry.hit_count), sizeof(entry.hit_count));
        file.read(reinterpret_cast<char*>(&entry.execution_count), sizeof(entry.execution_count));
        
        // ملاحظة: الكود المُجمّع سيحتاج إعادة تجميع / Note: Compiled code needs recompilation
        entry.compiled_code = nullptr;
        
        // لا نضيف للذاكرة المؤقتة الآن - فقط نحمل الإحصائيات / Don't add to cache now - just load statistics
        // المُستدعي مسؤول عن إعادة التجميع / Caller responsible for recompilation
    }
    
    file.close();
    return true;
}

// ============================================================================
// الحصول على جميع العناصر / Get All Entries
// ============================================================================

std::vector<CacheEntry> JITCache::getAllEntries() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::vector<CacheEntry> entries;
    entries.reserve(cache_.size()); // حجز المساحة مسبقاً / Reserve space
    
    for (const auto& pair : cache_) {
        entries.push_back(pair.second);
    }
    
    return entries;
}

// ============================================================================
// الحصول على العناصر الساخنة / Get Hot Entries
// ============================================================================

std::vector<CacheEntry> JITCache::getHotEntries() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::vector<CacheEntry> hot_entries;
    
    for (const auto& pair : cache_) {
        if (pair.second.is_hot) {
            hot_entries.push_back(pair.second);
        }
    }
    
    // ترتيب حسب الأولوية / Sort by priority
    std::sort(hot_entries.begin(), hot_entries.end(),
        [](const CacheEntry& a, const CacheEntry& b) {
            return a.getPriority() > b.getPriority();
        });
    
    return hot_entries;
}

// ============================================================================
// طباعة معلومات الذاكرة المؤقتة / Print Cache Information
// ============================================================================

void JITCache::printInfo() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::cout << "\n========================================\n";
    std::cout << " JIT Cache Information\n";
    std::cout << " معلومات الذاكرة المؤقتة\n";
    std::cout << "========================================\n";
    std::cout << "Entries:           " << cache_.size() << "\n";
    std::cout << "Size:              " << (current_size_bytes_ / 1024.0 / 1024.0) << " MB\n";
    std::cout << "Max Size:          " << (max_size_bytes_ / 1024.0 / 1024.0) << " MB\n";
    std::cout << "Usage:             " << std::fixed << std::setprecision(2) 
              << getUsagePercentage() << "%\n";
    std::cout << "Hit Rate:          " << std::fixed << std::setprecision(2) 
              << getHitRate() << "%\n";
    std::cout << "Hits:              " << hit_count_ << "\n";
    std::cout << "Misses:            " << miss_count_ << "\n";
    std::cout << "Evictions:         " << eviction_count_ << "\n";
    std::cout << "========================================\n\n";
}

// ============================================================================
// تصدير إلى JSON / Export to JSON
// ============================================================================

std::string JITCache::toJSON() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // حساب النسب محلياً بدون استدعاء الدوال (تجنب deadlock) / Calculate percentages locally without calling functions (avoid deadlock)
    double usage_percent = max_size_bytes_ > 0 ? (static_cast<double>(current_size_bytes_) / max_size_bytes_) * 100.0 : 0.0;
    size_t total_accesses = hit_count_ + miss_count_;
    double hit_rate_percent = total_accesses > 0 ? (static_cast<double>(hit_count_) / total_accesses) * 100.0 : 0.0;
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"entries\": " << cache_.size() << ",\n";
    json << "  \"size_mb\": " << (current_size_bytes_ / 1024.0 / 1024.0) << ",\n";
    json << "  \"max_size_mb\": " << (max_size_bytes_ / 1024.0 / 1024.0) << ",\n";
    json << "  \"usage_percent\": " << usage_percent << ",\n";
    json << "  \"hit_rate_percent\": " << hit_rate_percent << ",\n";
    json << "  \"hits\": " << hit_count_ << ",\n";
    json << "  \"misses\": " << miss_count_ << ",\n";
    json << "  \"evictions\": " << eviction_count_ << "\n";
    json << "}";
    
    return json.str();
}

// ============================================================================
// الدوال المساعدة الداخلية / Internal Helper Functions
// ============================================================================

// اختيار عنصر للإخلاء بناءً على السياسة / Select entry to evict based on policy
std::string JITCache::selectVictim() {
    // ملاحظة: يُفترض أن mutex مقفول من الدالة المُستدعية / Note: Assumes mutex is locked by caller
    
    switch (eviction_policy_) {
        case CacheEvictionPolicy::LRU:
            return selectLRU();
        case CacheEvictionPolicy::LFU:
            return selectLFU();
        case CacheEvictionPolicy::PRIORITY:
            return selectLowestPriority();
        case CacheEvictionPolicy::SIZE:
            return selectLargest();
        case CacheEvictionPolicy::FIFO:
            return selectFIFO();
        default:
            return selectLRU(); // الافتراضي / Default
    }
}

// اختيار عنصر LRU / Select LRU entry
std::string JITCache::selectLRU() {
    if (cache_.empty()) return "";
    
    auto oldest = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        if (it->second.last_accessed < oldest->second.last_accessed) {
            oldest = it;
        }
    }
    
    return oldest->first;
}

// اختيار عنصر LFU / Select LFU entry
std::string JITCache::selectLFU() {
    if (cache_.empty()) return "";
    
    auto least_freq = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        if (it->second.hit_count < least_freq->second.hit_count) {
            least_freq = it;
        }
    }
    
    return least_freq->first;
}

// اختيار عنصر بأقل أولوية / Select lowest priority entry
std::string JITCache::selectLowestPriority() {
    if (cache_.empty()) return "";
    
    auto lowest = cache_.begin();
    double min_priority = lowest->second.getPriority();
    
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        double priority = it->second.getPriority();
        if (priority < min_priority) {
            min_priority = priority;
            lowest = it;
        }
    }
    
    return lowest->first;
}

// اختيار أكبر عنصر / Select largest entry
std::string JITCache::selectLargest() {
    if (cache_.empty()) return "";
    
    auto largest = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        if (it->second.code_size_bytes > largest->second.code_size_bytes) {
            largest = it;
        }
    }
    
    return largest->first;
}

// اختيار عنصر FIFO / Select FIFO entry
std::string JITCache::selectFIFO() {
    if (cache_.empty()) return "";
    
    auto oldest = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
        if (it->second.created_at < oldest->second.created_at) {
            oldest = it;
        }
    }
    
    return oldest->first;
}

} // namespace JIT
} // namespace Sad

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
