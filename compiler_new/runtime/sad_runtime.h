// ========================================================================
// Sad Language Runtime Support Library
// مكتبة دعم وقت التشغيل للغة ص
// ========================================================================
// This file contains C++ runtime functions that support builtin operations
// in the Sad compiler. These functions are compiled and linked with generated
// LLVM IR to provide string, array, file, and utility operations.
// ========================================================================

#ifndef SAD_RUNTIME_H
#define SAD_RUNTIME_H

#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>

// Forward declarations for opaque types
struct SadString;
struct SadArray;

// ========================================================================
// String Functions (12 functions)
// ========================================================================

extern "C" {

// 1. طول_نص / string_length
int64_t sad_string_length(const char* str) {
    if (!str) return 0;
    return static_cast<int64_t>(strlen(str));
}

// 2. تحويل_كبير / toUpper
char* sad_string_to_upper(const char* str) {
    if (!str) return nullptr;
    size_t len = strlen(str);
    char* result = (char*)malloc(len + 1);
    for (size_t i = 0; i < len; ++i) {
        result[i] = toupper(str[i]);
    }
    result[len] = '\0';
    return result;
}

// 3. تحويل_صغير / toLower
char* sad_string_to_lower(const char* str) {
    if (!str) return nullptr;
    size_t len = strlen(str);
    char* result = (char*)malloc(len + 1);
    for (size_t i = 0; i < len; ++i) {
        result[i] = tolower(str[i]);
    }
    result[len] = '\0';
    return result;
}

// 4. بحث / find (returns index or -1)
int64_t sad_string_find(const char* str, const char* substr) {
    if (!str || !substr) return -1;
    const char* pos = strstr(str, substr);
    if (!pos) return -1;
    return static_cast<int64_t>(pos - str);
}

// 5. استبدل / replace
char* sad_string_replace(const char* str, const char* old_str, const char* new_str) {
    if (!str || !old_str || !new_str) return nullptr;
    
    std::string s(str);
    std::string old_s(old_str);
    std::string new_s(new_str);
    
    size_t pos = 0;
    while ((pos = s.find(old_s, pos)) != std::string::npos) {
        s.replace(pos, old_s.length(), new_s);
        pos += new_s.length();
    }
    
    char* result = (char*)malloc(s.length() + 1);
    strcpy(result, s.c_str());
    return result;
}

// 6. استخراج / substring
char* sad_string_substring(const char* str, int64_t start, int64_t end) {
    if (!str) return nullptr;
    int64_t len = strlen(str);
    if (start < 0) start = 0;
    if (end < 0 || end > len) end = len;
    if (start >= end) return strdup("");
    
    int64_t substr_len = end - start;
    char* result = (char*)malloc(substr_len + 1);
    strncpy(result, str + start, substr_len);
    result[substr_len] = '\0';
    return result;
}

// 7. قص_أطراف / trim
char* sad_string_trim(const char* str) {
    if (!str) return nullptr;
    
    const char* start = str;
    while (*start && isspace(*start)) start++;
    
    if (*start == '\0') return strdup("");
    
    const char* end = str + strlen(str) - 1;
    while (end > start && isspace(*end)) end--;
    
    size_t len = end - start + 1;
    char* result = (char*)malloc(len + 1);
    strncpy(result, start, len);
    result[len] = '\0';
    return result;
}

// 8. تقسيم / split (simplified - returns array size via pointer)
char** sad_string_split(const char* str, const char* delimiter, int64_t* out_size) {
    if (!str || !delimiter || !out_size) {
        if (out_size) *out_size = 0;
        return nullptr;
    }
    
    std::vector<std::string> parts;
    std::string s(str);
    std::string delim(delimiter);
    
    size_t pos = 0;
    size_t found;
    while ((found = s.find(delim, pos)) != std::string::npos) {
        parts.push_back(s.substr(pos, found - pos));
        pos = found + delim.length();
    }
    parts.push_back(s.substr(pos));
    
    *out_size = parts.size();
    char** result = (char**)malloc(parts.size() * sizeof(char*));
    for (size_t i = 0; i < parts.size(); ++i) {
        result[i] = strdup(parts[i].c_str());
    }
    return result;
}

// 9. دمج / join
char* sad_string_join(char** arr, int64_t size, const char* delimiter) {
    if (!arr || size <= 0 || !delimiter) return strdup("");
    
    std::string result;
    for (int64_t i = 0; i < size; ++i) {
        if (i > 0) result += delimiter;
        if (arr[i]) result += arr[i];
    }
    
    return strdup(result.c_str());
}

// 10. يبدأ_ب / startsWith
bool sad_string_starts_with(const char* str, const char* prefix) {
    if (!str || !prefix) return false;
    size_t str_len = strlen(str);
    size_t prefix_len = strlen(prefix);
    if (prefix_len > str_len) return false;
    return strncmp(str, prefix, prefix_len) == 0;
}

// 11. ينتهي_ب / endsWith
bool sad_string_ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) return false;
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) return false;
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

// 12. يحتوي_على / contains
bool sad_string_contains(const char* str, const char* substr) {
    if (!str || !substr) return false;
    return strstr(str, substr) != nullptr;
}

// ========================================================================
// Array Functions (10 functions) — Fully Implemented
// دوال المصفوفات (10 دوال) — مطبقة بالكامل
// ========================================================================

// النوع الداخلي للمصفوفة / Internal array type
typedef struct {
    void** data;
    int64_t size;
    int64_t capacity;
} SadArray;

// دالة مساعدة: إنشاء مصفوفة جديدة / Helper: create new array
static SadArray* sad_array_create_internal(int64_t initialCapacity) {
    if (initialCapacity <= 0) initialCapacity = 8;
    SadArray* arr = (SadArray*)malloc(sizeof(SadArray));
    if (!arr) return nullptr;
    arr->data = (void**)calloc(initialCapacity, sizeof(void*));
    arr->size = 0;
    arr->capacity = initialCapacity;
    return arr;
}

// دالة مساعدة: توسيع سعة المصفوفة / Helper: grow array capacity
static void sad_array_grow(SadArray* arr) {
    if (!arr) return;
    int64_t newCapacity = arr->capacity * 2;
    if (newCapacity < 8) newCapacity = 8;
    void** newData = (void**)realloc(arr->data, newCapacity * sizeof(void*));
    if (newData) {
        arr->data = newData;
        arr->capacity = newCapacity;
    }
}

// 1. إضافة_عنصر / append - إضافة عنصر في نهاية المصفوفة
void* sad_array_append(void* arrPtr, void* element) {
    if (!arrPtr) {
        // (AR) إنشاء مصفوفة جديدة إذا لم تكن موجودة
        SadArray* arr = sad_array_create_internal(8);
        if (!arr) return nullptr;
        arr->data[0] = element;
        arr->size = 1;
        return arr;
    }
    SadArray* arr = (SadArray*)arrPtr;
    if (arr->size >= arr->capacity) {
        sad_array_grow(arr);
    }
    if (arr->size < arr->capacity) {
        arr->data[arr->size] = element;
        arr->size++;
    }
    return arr;
}

// 2. إزالة_عنصر / remove - حذف عنصر بالفهرس
void* sad_array_remove(void* arrPtr, int64_t index) {
    if (!arrPtr) return nullptr;
    SadArray* arr = (SadArray*)arrPtr;
    if (index < 0 || index >= arr->size) return arr;
    
    // (AR) انقل العناصر لملء الفجوة
    for (int64_t i = index; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->data[arr->size - 1] = nullptr;
    arr->size--;
    return arr;
}

// 3. حجم_مصفوفة / size - إرجاع عدد العناصر
int64_t sad_array_size(void* arrPtr) {
    if (!arrPtr) return 0;
    SadArray* arr = (SadArray*)arrPtr;
    return arr->size;
}

// 4. فهرس / index_of - البحث عن عنصر وإرجاع فهرسه
int64_t sad_array_index_of(void* arrPtr, void* element) {
    if (!arrPtr) return -1;
    SadArray* arr = (SadArray*)arrPtr;
    for (int64_t i = 0; i < arr->size; i++) {
        if (arr->data[i] == element) return i;
    }
    return -1;
}

// 5. يحتوي_عنصر / contains - هل المصفوفة تحتوي على العنصر؟
bool sad_array_contains(void* arrPtr, void* element) {
    return sad_array_index_of(arrPtr, element) >= 0;
}

// 6. قلب / reverse - عكس ترتيب العناصر
void* sad_array_reverse(void* arrPtr) {
    if (!arrPtr) return nullptr;
    SadArray* arr = (SadArray*)arrPtr;
    for (int64_t i = 0; i < arr->size / 2; i++) {
        int64_t j = arr->size - 1 - i;
        void* tmp = arr->data[i];
        arr->data[i] = arr->data[j];
        arr->data[j] = tmp;
    }
    return arr;
}

// 7. ترتيب / sort - ترتيب المصفوفة (يعامل المؤشرات كأعداد صحيحة)
void* sad_array_sort(void* arrPtr) {
    if (!arrPtr) return nullptr;
    SadArray* arr = (SadArray*)arrPtr;
    // (AR) ترتيب فقاعي بسيط بمقارنة قيم المؤشرات (أعداد صحيحة)
    for (int64_t i = 0; i < arr->size - 1; i++) {
        for (int64_t j = 0; j < arr->size - 1 - i; j++) {
            if ((intptr_t)arr->data[j] > (intptr_t)arr->data[j + 1]) {
                void* tmp = arr->data[j];
                arr->data[j] = arr->data[j + 1];
                arr->data[j + 1] = tmp;
            }
        }
    }
    return arr;
}

// 8. أول / first - إرجاع أول عنصر
void* sad_array_first(void* arrPtr) {
    if (!arrPtr) return nullptr;
    SadArray* arr = (SadArray*)arrPtr;
    if (arr->size == 0) return nullptr;
    return arr->data[0];
}

// 9. آخر / last - إرجاع آخر عنصر
void* sad_array_last(void* arrPtr) {
    if (!arrPtr) return nullptr;
    SadArray* arr = (SadArray*)arrPtr;
    if (arr->size == 0) return nullptr;
    return arr->data[arr->size - 1];
}

// 10. شريحة / slice - استخراج جزء من المصفوفة
void* sad_array_slice(void* arrPtr, int64_t start, int64_t end) {
    if (!arrPtr) return nullptr;
    SadArray* arr = (SadArray*)arrPtr;
    
    // (AR) تصحيح الحدود / Bounds correction
    if (start < 0) start = 0;
    if (end > arr->size) end = arr->size;
    if (start >= end) {
        return sad_array_create_internal(4);
    }
    
    int64_t sliceLen = end - start;
    SadArray* result = sad_array_create_internal(sliceLen > 0 ? sliceLen : 4);
    if (!result) return nullptr;
    
    for (int64_t i = start; i < end; i++) {
        result->data[result->size] = arr->data[i];
        result->size++;
    }
    
    return result;
}

// ========================================================================
// Dictionary Functions (10 functions) — NEW
// دوال الخرائط/القواميس (10 دوال) — جديدة
// ========================================================================

// النوع الداخلي لمدخل القاموس / Internal dict entry
typedef struct SadDictEntry {
    char* key;
    void* value;
    struct SadDictEntry* next;  // (AR) للتعامل مع التصادمات / for collision handling
} SadDictEntry;

// النوع الداخلي للقاموس / Internal dict type
typedef struct {
    SadDictEntry** buckets;
    int64_t bucketCount;
    int64_t size;
} SadDict;

// دالة هاش بسيطة / Simple hash function
static uint64_t sad_dict_hash(const char* key, int64_t bucketCount) {
    if (!key || bucketCount <= 0) return 0;
    uint64_t hash = 14695981039346656037ULL;
    while (*key) {
        hash ^= (uint64_t)(unsigned char)(*key);
        hash *= 1099511628211ULL;
        key++;
    }
    return hash % (uint64_t)bucketCount;
}

// 1. إنشاء_خريطة / dict_new - إنشاء قاموس جديد
void* sad_dict_new() {
    SadDict* dict = (SadDict*)malloc(sizeof(SadDict));
    if (!dict) return nullptr;
    dict->bucketCount = 64;
    dict->buckets = (SadDictEntry**)calloc(dict->bucketCount, sizeof(SadDictEntry*));
    dict->size = 0;
    return dict;
}

// 2. أضف / dict_set - إضافة أو تحديث مدخل
void sad_dict_set(void* dictPtr, const char* key, void* value) {
    if (!dictPtr || !key) return;
    SadDict* dict = (SadDict*)dictPtr;
    
    uint64_t idx = sad_dict_hash(key, dict->bucketCount);
    
    // (AR) البحث عن مفتاح موجود
    SadDictEntry* entry = dict->buckets[idx];
    while (entry) {
        if (entry->key && strcmp(entry->key, key) == 0) {
            entry->value = value;  // (AR) تحديث القيمة
            return;
        }
        entry = entry->next;
    }
    
    // (AR) إنشاء مدخل جديد
    SadDictEntry* newEntry = (SadDictEntry*)malloc(sizeof(SadDictEntry));
    if (!newEntry) return;
    newEntry->key = strdup(key);
    newEntry->value = value;
    newEntry->next = dict->buckets[idx];
    dict->buckets[idx] = newEntry;
    dict->size++;
}

// 3. احصل / dict_get - الحصول على قيمة بالمفتاح
void* sad_dict_get(void* dictPtr, const char* key) {
    if (!dictPtr || !key) return nullptr;
    SadDict* dict = (SadDict*)dictPtr;
    
    uint64_t idx = sad_dict_hash(key, dict->bucketCount);
    SadDictEntry* entry = dict->buckets[idx];
    while (entry) {
        if (entry->key && strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return nullptr;
}

// 4. احذف / dict_remove - حذف مدخل بالمفتاح
bool sad_dict_remove(void* dictPtr, const char* key) {
    if (!dictPtr || !key) return false;
    SadDict* dict = (SadDict*)dictPtr;
    
    uint64_t idx = sad_dict_hash(key, dict->bucketCount);
    SadDictEntry** prevPtr = &dict->buckets[idx];
    SadDictEntry* entry = dict->buckets[idx];
    
    while (entry) {
        if (entry->key && strcmp(entry->key, key) == 0) {
            *prevPtr = entry->next;
            free(entry->key);
            free(entry);
            dict->size--;
            return true;
        }
        prevPtr = &entry->next;
        entry = entry->next;
    }
    return false;
}

// 5. يحتوي_مفتاح / dict_has - هل يحتوي على المفتاح؟
bool sad_dict_has(void* dictPtr, const char* key) {
    if (!dictPtr || !key) return false;
    SadDict* dict = (SadDict*)dictPtr;
    
    uint64_t idx = sad_dict_hash(key, dict->bucketCount);
    SadDictEntry* entry = dict->buckets[idx];
    while (entry) {
        if (entry->key && strcmp(entry->key, key) == 0) return true;
        entry = entry->next;
    }
    return false;
}

// 6. حجم / dict_size - عدد المداخل
int64_t sad_dict_size(void* dictPtr) {
    if (!dictPtr) return 0;
    SadDict* dict = (SadDict*)dictPtr;
    return dict->size;
}

// 7. مفاتيح / dict_keys - إرجاع مصفوفة المفاتيح
char** sad_dict_keys(void* dictPtr, int64_t* outSize) {
    if (!dictPtr || !outSize) {
        if (outSize) *outSize = 0;
        return nullptr;
    }
    SadDict* dict = (SadDict*)dictPtr;
    *outSize = dict->size;
    if (dict->size == 0) return nullptr;
    
    char** keys = (char**)malloc(dict->size * sizeof(char*));
    int64_t idx = 0;
    for (int64_t b = 0; b < dict->bucketCount; b++) {
        SadDictEntry* entry = dict->buckets[b];
        while (entry) {
            keys[idx++] = strdup(entry->key);
            entry = entry->next;
            if (idx >= dict->size) break;
        }
        if (idx >= dict->size) break;
    }
    return keys;
}

// 8. امسح / dict_clear - مسح جميع المداخل
void sad_dict_clear(void* dictPtr) {
    if (!dictPtr) return;
    SadDict* dict = (SadDict*)dictPtr;
    
    for (int64_t b = 0; b < dict->bucketCount; b++) {
        SadDictEntry* entry = dict->buckets[b];
        while (entry) {
            SadDictEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
        dict->buckets[b] = nullptr;
    }
    dict->size = 0;
}

// 9. حرر / dict_free - تحرير القاموس
void sad_dict_free(void* dictPtr) {
    if (!dictPtr) return;
    sad_dict_clear(dictPtr);
    SadDict* dict = (SadDict*)dictPtr;
    free(dict->buckets);
    free(dict);
}

// 10. القيم / dict_values - إرجاع مصفوفة القيم
void** sad_dict_values(void* dictPtr, int64_t* outSize) {
    if (!dictPtr || !outSize) {
        if (outSize) *outSize = 0;
        return nullptr;
    }
    SadDict* dict = (SadDict*)dictPtr;
    *outSize = dict->size;
    if (dict->size == 0) return nullptr;
    
    void** values = (void**)malloc(dict->size * sizeof(void*));
    int64_t idx = 0;
    for (int64_t b = 0; b < dict->bucketCount; b++) {
        SadDictEntry* entry = dict->buckets[b];
        while (entry) {
            values[idx++] = entry->value;
            entry = entry->next;
            if (idx >= dict->size) break;
        }
        if (idx >= dict->size) break;
    }
    return values;
}

// ========================================================================
// File I/O Functions (8 functions)
// ========================================================================

// 1. اقرأ_ملف / read_file
char* sad_file_read(const char* path) {
    if (!path) return nullptr;
    
    FILE* file = fopen(path, "rb");
    if (!file) return nullptr;
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(size + 1);
    size_t read = fread(content, 1, size, file);
    content[read] = '\0';
    fclose(file);
    
    return content;
}

// 2. اكتب_ملف / write_file
bool sad_file_write(const char* path, const char* content) {
    if (!path || !content) return false;
    
    FILE* file = fopen(path, "wb");
    if (!file) return false;
    
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);
    fclose(file);
    
    return written == len;
}

// 3. أضف_إلى_ملف / append_to_file
bool sad_file_append(const char* path, const char* content) {
    if (!path || !content) return false;
    
    FILE* file = fopen(path, "ab");
    if (!file) return false;
    
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);
    fclose(file);
    
    return written == len;
}

// 4. احذف_ملف / delete_file
bool sad_file_delete(const char* path) {
    if (!path) return false;
    return remove(path) == 0;
}

// 5. انسخ_ملف / copy_file
bool sad_file_copy(const char* src, const char* dst) {
    if (!src || !dst) return false;
    
    try {
        std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

// 6. انقل_ملف / move_file
bool sad_file_move(const char* src, const char* dst) {
    if (!src || !dst) return false;
    
    try {
        std::filesystem::rename(src, dst);
        return true;
    } catch (...) {
        return false;
    }
}

// 7. أنشئ_مجلد / create_dir
bool sad_file_create_dir(const char* path) {
    if (!path) return false;
    
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

// 8. اسرد_مجلد / list_dir
char** sad_file_list_dir(const char* path, int64_t* out_size) {
    if (!path || !out_size) {
        if (out_size) *out_size = 0;
        return nullptr;
    }
    
    try {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
        
        *out_size = files.size();
        char** result = (char**)malloc(files.size() * sizeof(char*));
        for (size_t i = 0; i < files.size(); ++i) {
            result[i] = strdup(files[i].c_str());
        }
        return result;
    } catch (...) {
        *out_size = 0;
        return nullptr;
    }
}

// ========================================================================
// Utility Functions (4 functions)
// ========================================================================

// 1. عشوائي / random (0.0 to 1.0)
double sad_random() {
    return (double)rand() / (double)RAND_MAX;
}

// 2. نم / sleep (seconds)
void sad_sleep(double seconds) {
    #ifdef _WIN32
        #include <windows.h>
        Sleep((DWORD)(seconds * 1000));
    #else
        #include <unistd.h>
        usleep((useconds_t)(seconds * 1000000));
    #endif
}

// 3. اخرج / exit
void sad_exit(int64_t code) {
    exit((int)code);
}

// 4. النوع / type_of - إرجاع اسم النوع
const char* sad_type_of(void* value) {
    if (!value) return "فارغ";  // null → فارغ
    // (AR) فحص أنماط معروفة بناءً على بنية الذاكرة
    // (EN) Check known patterns based on memory structure
    // بدون RTTI كامل، نرجع "كائن" كنوع افتراضي / Without full RTTI, return "object"
    return "كائن";
}

// ========================================================================
// Security Functions / دوال الأمان (14 functions)
// ========================================================================
// (AR) هذا القسم يحتوي على دوال وقت التشغيل لنظام الأمان
//      تُستدعى من كود LLVM IR المولّد بواسطة المترجم sadc
// ========================================================================

// 1. تأكد / assert - يوقف البرنامج إذا كان الشرط خاطئاً
void sad_security_assert(bool condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "[خطأ أمان / Security Error] تأكيد فاشل: %s\n", 
                message ? message : "Assertion failed");
        exit(1);
    }
}

// 2. تحقق / verify - يعيد القيمة المنطقية مباشرة (مُعالج في LLVM IR)
// Note: verify is handled directly in LLVM IR codegen (no runtime function needed)

// 3. آمن / is_safe - مُعالج في LLVM IR مباشرة
// Note: is_safe is handled directly in LLVM IR codegen

// 4. ذعر / panic - إيقاف طارئ مع رسالة
void sad_security_panic(const char* message) {
    fprintf(stderr, "\n[ذعر! / PANIC!] %s\n", message ? message : "Unknown error");
    fprintf(stderr, "البرنامج توقف بشكل طارئ / Program halted\n");
    exit(2);
}

// 5. هاش / hash - حساب هاش FNV-1a
int64_t sad_security_hash(const char* text) {
    if (!text) return 0;
    
    // FNV-1a hash algorithm
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis
    const uint64_t prime = 1099511628211ULL;  // FNV prime
    
    while (*text) {
        hash ^= (uint64_t)(unsigned char)(*text);
        hash *= prime;
        text++;
    }
    
    return (int64_t)hash;
}

// 6. شفّر / encrypt - تشفير XOR
char* sad_security_encrypt(const char* text, const char* key) {
    if (!text || !key || !*key) return nullptr;
    
    size_t textLen = strlen(text);
    size_t keyLen = strlen(key);
    char* result = (char*)malloc(textLen + 1);
    
    for (size_t i = 0; i < textLen; i++) {
        result[i] = text[i] ^ key[i % keyLen];
    }
    result[textLen] = '\0';
    
    return result;
}

// 7. فك_تشفير / decrypt - فك تشفير XOR (نفس عملية التشفير)
char* sad_security_decrypt(const char* text, const char* key) {
    return sad_security_encrypt(text, key); // XOR is symmetric
}

// 8. تأكد_نوع / assert_type - التحقق من النوع
void sad_security_assert_type(const char* value, const char* expectedType) {
    // Runtime type checking - simplified for compiled code
    if (!value || !expectedType) {
        fprintf(stderr, "[خطأ أمان] تأكد_نوع: معاملات فارغة\n");
        exit(1);
    }
    // In compiled code, type checking is primarily done at compile time
    // This is a runtime safeguard
}

// 9. تأكد_مساواة (نصوص) / assert_equal for strings
void sad_security_assert_equal_str(const char* a, const char* b) {
    if (!a || !b || strcmp(a, b) != 0) {
        fprintf(stderr, "[خطأ أمان] تأكيد المساواة فشل: \"%s\" != \"%s\"\n",
                a ? a : "null", b ? b : "null");
        exit(1);
    }
}

// 10. (تأكد_أكبر مُعالج في LLVM IR مباشرة باستخدام sad_security_assert)

// 11. نظّف / sanitize - تنظيف HTML
char* sad_security_sanitize(const char* text) {
    if (!text) return nullptr;
    
    // Count required size
    size_t len = strlen(text);
    size_t newLen = 0;
    for (size_t i = 0; i < len; i++) {
        switch (text[i]) {
            case '<':  newLen += 4; break; // &lt;
            case '>':  newLen += 4; break; // &gt;
            case '&':  newLen += 5; break; // &amp;
            case '"':  newLen += 6; break; // &quot;
            case '\'': newLen += 5; break; // &#39;
            default:   newLen += 1; break;
        }
    }
    
    char* result = (char*)malloc(newLen + 1);
    size_t pos = 0;
    for (size_t i = 0; i < len; i++) {
        switch (text[i]) {
            case '<':
                memcpy(result + pos, "&lt;", 4); pos += 4; break;
            case '>':
                memcpy(result + pos, "&gt;", 4); pos += 4; break;
            case '&':
                memcpy(result + pos, "&amp;", 5); pos += 5; break;
            case '"':
                memcpy(result + pos, "&quot;", 6); pos += 6; break;
            case '\'':
                memcpy(result + pos, "&#39;", 5); pos += 5; break;
            default:
                result[pos++] = text[i]; break;
        }
    }
    result[pos] = '\0';
    
    return result;
}

// 12. وقت_الآن / timestamp - الوقت الحالي بالثواني
int64_t sad_security_timestamp() {
    return (int64_t)time(NULL);
}

// 13. عشوائي_آمن / secure_random - رقم عشوائي آمن
int64_t sad_security_secure_random(int64_t minVal, int64_t maxVal) {
    if (minVal >= maxVal) return minVal;
    
    // Use rand() seeded with time for simplicity
    // In production, use OS-specific secure random
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
    
    int64_t range = maxVal - minVal + 1;
    return minVal + (rand() % range);
}

// 14. ترميز_64 / base64_encode - ترميز Base64
char* sad_security_base64_encode(const char* text) {
    if (!text) return nullptr;
    
    static const char base64_chars[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    size_t len = strlen(text);
    size_t outLen = ((len + 2) / 3) * 4;
    char* result = (char*)malloc(outLen + 1);
    
    size_t pos = 0;
    for (size_t i = 0; i < len; i += 3) {
        unsigned char b0 = (unsigned char)text[i];
        unsigned char b1 = (i + 1 < len) ? (unsigned char)text[i + 1] : 0;
        unsigned char b2 = (i + 2 < len) ? (unsigned char)text[i + 2] : 0;
        
        result[pos++] = base64_chars[b0 >> 2];
        result[pos++] = base64_chars[((b0 & 0x03) << 4) | (b1 >> 4)];
        result[pos++] = (i + 1 < len) ? base64_chars[((b1 & 0x0F) << 2) | (b2 >> 6)] : '=';
        result[pos++] = (i + 2 < len) ? base64_chars[b2 & 0x3F] : '=';
    }
    result[pos] = '\0';
    
    return result;
}

} // extern "C"

#endif // SAD_RUNTIME_H
