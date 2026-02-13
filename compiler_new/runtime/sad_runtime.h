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
// Array Functions (10 functions)
// ========================================================================

// Note: Arrays are simplified as dynamic arrays of doubles for now
// In a complete implementation, these would handle SadValue* arrays

typedef struct {
    void** data;
    int64_t size;
    int64_t capacity;
} SadArray;

void* sad_array_append(void* arr, void* element) {
    // Placeholder - requires full type system
    return arr;
}

void* sad_array_remove(void* arr, int64_t index) {
    // Placeholder - requires full type system
    return arr;
}

int64_t sad_array_size(void* arr) {
    if (!arr) return 0;
    SadArray* array = (SadArray*)arr;
    return array->size;
}

int64_t sad_array_index_of(void* arr, void* element) {
    // Placeholder - requires full type system
    return -1;
}

bool sad_array_contains(void* arr, void* element) {
    return sad_array_index_of(arr, element) >= 0;
}

void* sad_array_reverse(void* arr) {
    // Placeholder - requires full type system
    return arr;
}

void* sad_array_sort(void* arr) {
    // Placeholder - requires full type system
    return arr;
}

void* sad_array_first(void* arr) {
    // Placeholder - requires full type system
    return nullptr;
}

void* sad_array_last(void* arr) {
    // Placeholder - requires full type system
    return nullptr;
}

void* sad_array_slice(void* arr, int64_t start, int64_t end) {
    // Placeholder - requires full type system
    return nullptr;
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

// 4. النوع / type_of (simplified - returns type name)
const char* sad_type_of(void* value) {
    // Placeholder - requires full type system with RTTI
    return "unknown";
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
