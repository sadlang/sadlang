// ═══════════════════════════════════════════════════════════════════════════════
// sad_serialization_resources.cpp — تنفيذ التسلسل ومدير الموارد
// Serialization & Resource Manager Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_serialization_resources.h"
#include <map>
#include <unordered_map>
#include <sstream>
#include <fstream>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// JSON — هياكل داخلية / JSON Internals
// ═══════════════════════════════════════════════════════════════

struct JsonEntry {
    enum Type { STRING, NUMBER, BOOLEAN, NUL };
    Type type = NUL;
    std::string strVal;
    double numVal = 0.0;
    bool boolVal = false;
};

struct JsonData {
    int id = 0;
    std::unordered_map<std::string, JsonEntry> entries;
};

static std::map<int, JsonData> g_jsons;
static int g_nextJsonId = 1;

// ═══════════════════════════════════════════════════════════════
// مدير الموارد — هياكل داخلية / Resource Manager Internals
// ═══════════════════════════════════════════════════════════════

struct ResourceData {
    std::string path;
    std::string type;
    int size = 0;
    bool loaded = true;
};

static std::unordered_map<std::string, ResourceData> g_resources;
static int g_loadCount = 0;
static int g_cacheHits = 0;
static int g_cacheMisses = 0;
static int g_memoryLimit = 512 * 1024 * 1024;

// ═══════════════════════════════════════════════════════════════
// تنفيذ JSON / JSON Implementation
// ═══════════════════════════════════════════════════════════════

int json_create_impl() {
    int id = g_nextJsonId++;
    g_jsons[id].id = id;
    return id;
}

void json_destroy_impl(int jsonId) {
    g_jsons.erase(jsonId);
}

void json_write_string_impl(int jsonId, const std::string& key, const std::string& value) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return;
    it->second.entries[key] = {JsonEntry::STRING, value, 0.0, false};
}

void json_write_number_impl(int jsonId, const std::string& key, double value) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return;
    it->second.entries[key] = {JsonEntry::NUMBER, "", value, false};
}

void json_write_bool_impl(int jsonId, const std::string& key, bool value) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return;
    it->second.entries[key] = {JsonEntry::BOOLEAN, "", 0.0, value};
}

std::string json_read_string_impl(int jsonId, const std::string& key) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return "";
    auto eit = it->second.entries.find(key);
    if (eit == it->second.entries.end()) return "";
    return eit->second.strVal;
}

double json_read_number_impl(int jsonId, const std::string& key) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return 0.0;
    auto eit = it->second.entries.find(key);
    if (eit == it->second.entries.end()) return 0.0;
    return eit->second.numVal;
}

bool json_read_bool_impl(int jsonId, const std::string& key) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return false;
    auto eit = it->second.entries.find(key);
    if (eit == it->second.entries.end()) return false;
    return eit->second.boolVal;
}

static std::string escapeJsonString(const std::string& s) {
    std::string result;
    result.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string json_to_string_impl(int jsonId) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return "{}";
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& [key, entry] : it->second.entries) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << escapeJsonString(key) << "\":";
        switch (entry.type) {
            case JsonEntry::STRING:
                oss << "\"" << escapeJsonString(entry.strVal) << "\"";
                break;
            case JsonEntry::NUMBER:
                oss << entry.numVal;
                break;
            case JsonEntry::BOOLEAN:
                oss << (entry.boolVal ? "true" : "false");
                break;
            default:
                oss << "null";
                break;
        }
    }
    oss << "}";
    return oss.str();
}

int json_parse_impl(const std::string& /*jsonStr*/) {
    // تحليل بسيط — إنشاء كائن فارغ / Simple parse — create empty obj
    int id = g_nextJsonId++;
    g_jsons[id].id = id;
    return id;
}

bool json_has_key_impl(int jsonId, const std::string& key) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return false;
    return it->second.entries.count(key) > 0;
}

void json_remove_impl(int jsonId, const std::string& key) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return;
    it->second.entries.erase(key);
}

int json_size_impl(int jsonId) {
    auto it = g_jsons.find(jsonId);
    if (it == g_jsons.end()) return 0;
    return static_cast<int>(it->second.entries.size());
}

bool json_save_file_impl(int jsonId, const std::string& filepath) {
    auto str = json_to_string_impl(jsonId);
    std::ofstream f(filepath);
    if (!f.is_open()) return false;
    f << str;
    return true;
}

int json_load_file_impl(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f.is_open()) return -1;
    std::string content((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());
    return json_parse_impl(content);
}

// ═══════════════════════════════════════════════════════════════
// تنفيذ مدير الموارد / Resource Manager Implementation
// ═══════════════════════════════════════════════════════════════

int resource_load_impl(const std::string& path, const std::string& type) {
    g_loadCount++;
    auto it = g_resources.find(path);
    if (it != g_resources.end()) {
        g_cacheHits++;
        return 1; // already loaded
    }
    g_cacheMisses++;
    g_resources[path] = {path, type, 0, true};
    return 1;
}

void resource_unload_impl(const std::string& path) {
    g_resources.erase(path);
}

bool resource_loaded_impl(const std::string& path) {
    return g_resources.count(path) > 0;
}

int resource_count_impl() {
    return static_cast<int>(g_resources.size());
}

int resource_memory_impl() {
    int total = 0;
    for (const auto& [_, r] : g_resources) {
        total += r.size;
    }
    return total;
}

void resource_clear_all_impl() {
    g_resources.clear();
    g_loadCount = 0;
    g_cacheHits = 0;
    g_cacheMisses = 0;
}

double resource_cache_ratio_impl() {
    int total = g_cacheHits + g_cacheMisses;
    return total > 0 ? static_cast<double>(g_cacheHits) / total : 0.0;
}

int resource_load_count_impl() {
    return g_loadCount;
}

void resource_set_limit_impl(int bytes) {
    g_memoryLimit = bytes;
}

} // namespace graphics
} // namespace stdlib
} // namespace sad
