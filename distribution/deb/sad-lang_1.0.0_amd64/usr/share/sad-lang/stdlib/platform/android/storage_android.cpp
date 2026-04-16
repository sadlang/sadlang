/**
 * =============================================================================
 * ملف: storage_android.cpp
 * الوصف: واجهة SharedPreferences لنظام Android
 * (AR) @brief تخزين محلي مفتاح/قيمة متوافق مع SharedPreferences
 * (EN) @brief Key-value local storage compatible with Android SharedPreferences
 * المهمة: T312 - سعيد: SharedPreferences API
 * =============================================================================
 */

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <mutex>
#include <vector>
#include <functional>

namespace sad {
namespace platform {
namespace android {

// ============================================================================
// واجهة التخزين / Storage Interface
// ============================================================================

/**
 * (AR) واجهة SharedPreferences — تخزين محلي بسيط بنمط مفتاح/قيمة
 * (EN) SharedPreferences interface — simple key-value local storage
 * 
 * يدعم: نص، رقم، عشري، منطقي، قائمة نصوص
 * Cross-platform: يستخدم ملف JSON على سطح المكتب، SharedPreferences على Android
 */
class SharedPreferences {
public:
    /**
     * (AR) الحصول على مثيل SharedPreferences باسم
     * (EN) Get SharedPreferences instance by name
     */
    static SharedPreferences& getInstance(const std::string& name = "default") {
        static std::unordered_map<std::string, SharedPreferences> instances;
        auto it = instances.find(name);
        if (it == instances.end()) {
            instances.emplace(name, SharedPreferences(name));
            return instances.at(name);
        }
        return it->second;
    }

    // ========================================================================
    // قراءة القيم / Reading Values
    // ========================================================================
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = stringValues_.find(key);
        return it != stringValues_.end() ? it->second : defaultValue;
    }
    
    int64_t getInt(const std::string& key, int64_t defaultValue = 0) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = intValues_.find(key);
        return it != intValues_.end() ? it->second : defaultValue;
    }
    
    double getFloat(const std::string& key, double defaultValue = 0.0) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = floatValues_.find(key);
        return it != floatValues_.end() ? it->second : defaultValue;
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = boolValues_.find(key);
        return it != boolValues_.end() ? it->second : defaultValue;
    }
    
    std::vector<std::string> getStringSet(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = setValues_.find(key);
        return it != setValues_.end() ? it->second : std::vector<std::string>{};
    }
    
    bool contains(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stringValues_.count(key) || intValues_.count(key) ||
               floatValues_.count(key) || boolValues_.count(key) ||
               setValues_.count(key);
    }

    // ========================================================================
    // المحرر / Editor (Android pattern)
    // ========================================================================
    
    class Editor {
    public:
        explicit Editor(SharedPreferences& prefs) : prefs_(prefs) {}
        
        Editor& putString(const std::string& key, const std::string& value) {
            ops_.push_back([key, value](SharedPreferences& p) {
                p.stringValues_[key] = value;
            });
            return *this;
        }
        
        Editor& putInt(const std::string& key, int64_t value) {
            ops_.push_back([key, value](SharedPreferences& p) {
                p.intValues_[key] = value;
            });
            return *this;
        }
        
        Editor& putFloat(const std::string& key, double value) {
            ops_.push_back([key, value](SharedPreferences& p) {
                p.floatValues_[key] = value;
            });
            return *this;
        }
        
        Editor& putBool(const std::string& key, bool value) {
            ops_.push_back([key, value](SharedPreferences& p) {
                p.boolValues_[key] = value;
            });
            return *this;
        }
        
        Editor& putStringSet(const std::string& key, const std::vector<std::string>& value) {
            ops_.push_back([key, value](SharedPreferences& p) {
                p.setValues_[key] = value;
            });
            return *this;
        }
        
        Editor& remove(const std::string& key) {
            ops_.push_back([key](SharedPreferences& p) {
                p.stringValues_.erase(key);
                p.intValues_.erase(key);
                p.floatValues_.erase(key);
                p.boolValues_.erase(key);
                p.setValues_.erase(key);
            });
            return *this;
        }
        
        Editor& clear() {
            ops_.push_back([](SharedPreferences& p) {
                p.stringValues_.clear();
                p.intValues_.clear();
                p.floatValues_.clear();
                p.boolValues_.clear();
                p.setValues_.clear();
            });
            return *this;
        }
        
        /**
         * (AR) تطبيق التغييرات بشكل متزامن ثم حفظ
         * (EN) Apply changes synchronously then save
         */
        bool commit() {
            std::lock_guard<std::mutex> lock(prefs_.mutex_);
            for (auto& op : ops_) op(prefs_);
            ops_.clear();
            return prefs_.save();
        }
        
        /**
         * (AR) تطبيق التغييرات بسرعة (غير متزامن لاحقاً)
         * (EN) Apply changes quickly (async saving later)
         */
        void apply() {
            std::lock_guard<std::mutex> lock(prefs_.mutex_);
            for (auto& op : ops_) op(prefs_);
            ops_.clear();
            prefs_.save(); // Simplified: sync save
        }
        
    private:
        SharedPreferences& prefs_;
        std::vector<std::function<void(SharedPreferences&)>> ops_;
    };
    
    /**
     * (AR) إنشاء محرر
     * (EN) Create editor
     */
    Editor edit() { return Editor(*this); }

    // ========================================================================
    // مستمع التغييرات / Change Listener
    // ========================================================================
    
    using ChangeListener = std::function<void(const std::string& key)>;
    
    void registerOnSharedPreferenceChangeListener(ChangeListener listener) {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.push_back(listener);
    }

private:
    explicit SharedPreferences(const std::string& name) : name_(name) {
        load();
    }
    
    // Prevent copying
    SharedPreferences(const SharedPreferences&) = delete;
    SharedPreferences& operator=(const SharedPreferences&) = delete;
    // Allow move
    SharedPreferences(SharedPreferences&&) = default;
    SharedPreferences& operator=(SharedPreferences&&) = default;
    
    std::string name_;
    mutable std::mutex mutex_;
    
    std::unordered_map<std::string, std::string> stringValues_;
    std::unordered_map<std::string, int64_t> intValues_;
    std::unordered_map<std::string, double> floatValues_;
    std::unordered_map<std::string, bool> boolValues_;
    std::unordered_map<std::string, std::vector<std::string>> setValues_;
    
    std::vector<ChangeListener> listeners_;
    
    /**
     * (AR) حفظ للملف (JSON مبسط)
     * (EN) Save to file (simplified JSON)
     */
    bool save() {
        std::string path = getFilePath();
        std::ofstream ofs(path);
        if (!ofs.is_open()) return false;
        
        ofs << "{\n";
        bool first = true;
        
        for (const auto& [k, v] : stringValues_) {
            if (!first) ofs << ",\n";
            ofs << "  \"s:" << escapeJson(k) << "\": \"" << escapeJson(v) << "\"";
            first = false;
        }
        for (const auto& [k, v] : intValues_) {
            if (!first) ofs << ",\n";
            ofs << "  \"i:" << escapeJson(k) << "\": " << v;
            first = false;
        }
        for (const auto& [k, v] : floatValues_) {
            if (!first) ofs << ",\n";
            ofs << "  \"f:" << escapeJson(k) << "\": " << v;
            first = false;
        }
        for (const auto& [k, v] : boolValues_) {
            if (!first) ofs << ",\n";
            ofs << "  \"b:" << escapeJson(k) << "\": " << (v ? "true" : "false");
            first = false;
        }
        
        ofs << "\n}\n";
        return ofs.good();
    }
    
    /**
     * (AR) تحميل من الملف
     * (EN) Load from file
     */
    void load() {
        std::string path = getFilePath();
        std::ifstream ifs(path);
        if (!ifs.is_open()) return;
        
        std::string content((std::istreambuf_iterator<char>(ifs)),
                           std::istreambuf_iterator<char>());
        // Simple JSON parser for our format
        parseSimpleJson(content);
    }
    
    std::string getFilePath() const {
#ifdef __ANDROID__
        return "/data/data/com.sadlang.ide/shared_prefs/" + name_ + ".json";
#else
        return name_ + "_prefs.json";
#endif
    }
    
    static std::string escapeJson(const std::string& s) {
        std::string r;
        r.reserve(s.size());
        for (char c : s) {
            if (c == '"') r += "\\\"";
            else if (c == '\\') r += "\\\\";
            else if (c == '\n') r += "\\n";
            else r += c;
        }
        return r;
    }
    
    void parseSimpleJson(const std::string& json) {
        // Simple key-value parser for our prefixed format
        size_t pos = 0;
        while (pos < json.size()) {
            // Find key start
            size_t keyStart = json.find('"', pos);
            if (keyStart == std::string::npos) break;
            keyStart++;
            
            size_t keyEnd = json.find('"', keyStart);
            if (keyEnd == std::string::npos) break;
            
            std::string fullKey = json.substr(keyStart, keyEnd - keyStart);
            
            // Find value
            size_t colonPos = json.find(':', keyEnd + 1);
            if (colonPos == std::string::npos) break;
            
            // Determine type from prefix
            if (fullKey.size() > 2 && fullKey[1] == ':') {
                char type = fullKey[0];
                std::string key = fullKey.substr(2);
                
                // Skip whitespace
                size_t valStart = colonPos + 1;
                while (valStart < json.size() && json[valStart] == ' ') valStart++;
                
                if (type == 's') {
                    // String value
                    if (valStart < json.size() && json[valStart] == '"') {
                        valStart++;
                        size_t valEnd = json.find('"', valStart);
                        if (valEnd != std::string::npos) {
                            stringValues_[key] = json.substr(valStart, valEnd - valStart);
                            pos = valEnd + 1;
                            continue;
                        }
                    }
                } else if (type == 'i') {
                    size_t valEnd = json.find_first_of(",}\n", valStart);
                    std::string val = json.substr(valStart, valEnd - valStart);
                    intValues_[key] = std::stoll(val);
                    pos = valEnd;
                    continue;
                } else if (type == 'f') {
                    size_t valEnd = json.find_first_of(",}\n", valStart);
                    std::string val = json.substr(valStart, valEnd - valStart);
                    floatValues_[key] = std::stod(val);
                    pos = valEnd;
                    continue;
                } else if (type == 'b') {
                    size_t valEnd = json.find_first_of(",}\n", valStart);
                    std::string val = json.substr(valStart, valEnd - valStart);
                    // Trim whitespace
                    while (!val.empty() && (val.back() == ' ' || val.back() == '\r'))
                        val.pop_back();
                    boolValues_[key] = (val == "true");
                    pos = valEnd;
                    continue;
                }
            }
            
            pos = keyEnd + 1;
        }
    }
};

// ============================================================================
// واجهة عربية مبسطة / Simplified Arabic API
// ============================================================================

/**
 * (AR) واجهة تخزين عربية
 * (EN) Arabic storage interface wrapper
 */
class تخزين {
public:
    static void احفظ_نص(const std::string& مفتاح, const std::string& قيمة,
                        const std::string& اسم = "default") {
        SharedPreferences::getInstance(اسم).edit()
            .putString(مفتاح, قيمة).commit();
    }
    
    static std::string اقرأ_نص(const std::string& مفتاح,
                                const std::string& افتراضي = "",
                                const std::string& اسم = "default") {
        return SharedPreferences::getInstance(اسم).getString(مفتاح, افتراضي);
    }
    
    static void احفظ_رقم(const std::string& مفتاح, int64_t قيمة,
                        const std::string& اسم = "default") {
        SharedPreferences::getInstance(اسم).edit()
            .putInt(مفتاح, قيمة).commit();
    }
    
    static int64_t اقرأ_رقم(const std::string& مفتاح, int64_t افتراضي = 0,
                            const std::string& اسم = "default") {
        return SharedPreferences::getInstance(اسم).getInt(مفتاح, افتراضي);
    }
    
    static void احفظ_منطقي(const std::string& مفتاح, bool قيمة,
                            const std::string& اسم = "default") {
        SharedPreferences::getInstance(اسم).edit()
            .putBool(مفتاح, قيمة).commit();
    }
    
    static bool اقرأ_منطقي(const std::string& مفتاح, bool افتراضي = false,
                            const std::string& اسم = "default") {
        return SharedPreferences::getInstance(اسم).getBool(مفتاح, افتراضي);
    }
    
    static void مسح(const std::string& اسم = "default") {
        SharedPreferences::getInstance(اسم).edit().clear().commit();
    }
};

} // namespace android
} // namespace platform
} // namespace sad
