// ═══════════════════════════════════════════════════════════════════════════════
// sad_serialization_resources.h — ربط التسلسل ومدير الموارد بلغة ص
// Serialization & Resource Manager Binding for SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// التسلسل / Serialization (JSON)
// ═══════════════════════════════════════════════════════════════

/// إنشاء كائن JSON / Create JSON object
int json_create_impl();

/// تدمير كائن JSON / Destroy JSON object
void json_destroy_impl(int jsonId);

/// كتابة نص / Write string
void json_write_string_impl(int jsonId, const std::string& key, const std::string& value);

/// كتابة رقم / Write number
void json_write_number_impl(int jsonId, const std::string& key, double value);

/// كتابة منطقي / Write boolean
void json_write_bool_impl(int jsonId, const std::string& key, bool value);

/// قراءة نص / Read string
std::string json_read_string_impl(int jsonId, const std::string& key);

/// قراءة رقم / Read number
double json_read_number_impl(int jsonId, const std::string& key);

/// قراءة منطقي / Read boolean
bool json_read_bool_impl(int jsonId, const std::string& key);

/// تحويل إلى نص JSON / Convert to JSON string
std::string json_to_string_impl(int jsonId);

/// تحميل من نص JSON / Parse from JSON string
int json_parse_impl(const std::string& jsonStr);

/// هل المفتاح موجود؟ / Does key exist?
bool json_has_key_impl(int jsonId, const std::string& key);

/// حذف مفتاح / Remove key
void json_remove_impl(int jsonId, const std::string& key);

/// عدد المفاتيح / Key count
int json_size_impl(int jsonId);

/// حفظ في ملف / Save to file
bool json_save_file_impl(int jsonId, const std::string& filepath);

/// تحميل من ملف / Load from file
int json_load_file_impl(const std::string& filepath);

// ═══════════════════════════════════════════════════════════════
// مدير الموارد / Resource Manager
// ═══════════════════════════════════════════════════════════════

/// تحميل مورد / Load resource
int resource_load_impl(const std::string& path, const std::string& type);

/// تفريغ مورد / Unload resource
void resource_unload_impl(const std::string& path);

/// التحقق من وجود مورد / Check if resource is loaded
bool resource_loaded_impl(const std::string& path);

/// عدد الموارد / Resource count
int resource_count_impl();

/// الذاكرة المستخدمة / Memory used by resources
int resource_memory_impl();

/// تفريغ كل الموارد / Clear all resources
void resource_clear_all_impl();

/// نسبة نجاح الـ Cache / Cache hit ratio
double resource_cache_ratio_impl();

/// عدد التحميلات / Load count
int resource_load_count_impl();

/// تعيين حد الذاكرة / Set memory limit
void resource_set_limit_impl(int bytes);

} // namespace graphics
} // namespace stdlib
} // namespace sad
