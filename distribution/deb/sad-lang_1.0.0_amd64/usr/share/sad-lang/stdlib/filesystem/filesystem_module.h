// بسم الله الرحمن الرحيم
/**
 * @file filesystem_module.h
 * @brief Filesystem Module for Sad Language - وحدة نظام الملفات للغة ص
 * 
 * توفر دوال شاملة للتعامل مع نظام الملفات
 * Provides comprehensive functions for filesystem operations
 * 
 * Categories / الفئات:
 * - File I/O (read, write, append)
 * - Directory Operations (create, list, remove)
 * - Path Manipulation (join, split, normalize)
 * - File Information (size, permissions, timestamps)
 * - File System Queries (exists, is_file, is_directory)
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include "value.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <optional>
#include <memory>

namespace sad {
namespace stdlib {
namespace filesystem {

// ============================================================================
// File I/O Operations - عمليات قراءة/كتابة الملفات
// ============================================================================

/**
 * @brief Read entire file as string - قراءة الملف كاملاً كنص
 * @param path File path / مسار الملف
 * @return File contents or error / محتوى الملف أو خطأ
 * 
 * Usage: محتوى = اقرأ_ملف("data.txt")
 */
std::string read_file(const std::string& path);

/**
 * @brief Read file as lines - قراءة الملف كأسطر
 * @param path File path / مسار الملف
 * @return Vector of lines / مصفوفة من الأسطر
 * 
 * Usage: أسطر = اقرأ_أسطر("data.txt")
 */
std::vector<std::string> read_lines(const std::string& path);

/**
 * @brief Read file as bytes - قراءة الملف كبايتات
 * @param path File path / مسار الملف
 * @return Vector of bytes / مصفوفة من البايتات
 * 
 * Usage: بيانات = اقرأ_بايتات("image.png")
 */
std::vector<uint8_t> read_bytes(const std::string& path);

/**
 * @brief Write string to file - كتابة نص إلى ملف
 * @param path File path / مسار الملف
 * @param content Content to write / المحتوى للكتابة
 * @param truncate Truncate existing file / مسح الملف الموجود
 * 
 * Usage: اكتب_ملف("output.txt", "مرحباً بالعالم", صحيح)
 */
void write_file(const std::string& path, const std::string& content, bool truncate = true);

/**
 * @brief Write lines to file - كتابة أسطر إلى ملف
 * @param path File path / مسار الملف
 * @param lines Lines to write / الأسطر للكتابة
 * 
 * Usage: اكتب_أسطر("output.txt", ["سطر 1", "سطر 2"])
 */
void write_lines(const std::string& path, const std::vector<std::string>& lines);

/**
 * @brief Write bytes to file - كتابة بايتات إلى ملف
 * @param path File path / مسار الملف
 * @param bytes Bytes to write / البايتات للكتابة
 * 
 * Usage: اكتب_بايتات("image.png", بيانات)
 */
void write_bytes(const std::string& path, const std::vector<uint8_t>& bytes);

/**
 * @brief Append string to file - إضافة نص إلى ملف
 * @param path File path / مسار الملف
 * @param content Content to append / المحتوى للإضافة
 * 
 * Usage: أضف_إلى_ملف("log.txt", "رسالة جديدة\n")
 */
void append_to_file(const std::string& path, const std::string& content);

/**
 * @brief Copy file - نسخ ملف
 * @param source Source path / المسار المصدر
 * @param destination Destination path / المسار الوجهة
 * @param overwrite Overwrite if exists / الكتابة فوق إذا موجود
 * 
 * Usage: انسخ_ملف("source.txt", "dest.txt", صحيح)
 */
void copy_file(const std::string& source, const std::string& destination, bool overwrite = false);

/**
 * @brief Move/rename file - نقل/إعادة تسمية ملف
 * @param source Source path / المسار المصدر
 * @param destination Destination path / المسار الوجهة
 * 
 * Usage: انقل_ملف("old.txt", "new.txt")
 */
void move_file(const std::string& source, const std::string& destination);

/**
 * @brief Delete file - حذف ملف
 * @param path File path / مسار الملف
 * @return true if deleted / صحيح إذا تم الحذف
 * 
 * Usage: احذف_ملف("temp.txt")
 */
bool delete_file(const std::string& path);

// ============================================================================
// Directory Operations - عمليات المجلدات
// ============================================================================

/**
 * @brief Create directory - إنشاء مجلد
 * @param path Directory path / مسار المجلد
 * @param recursive Create parent directories / إنشاء المجلدات الأم
 * @return true if created / صحيح إذا تم الإنشاء
 * 
 * Usage: أنشئ_مجلد("data/output", صحيح)
 */
bool create_directory(const std::string& path, bool recursive = true);

/**
 * @brief List directory contents - سرد محتويات مجلد
 * @param path Directory path / مسار المجلد
 * @return Vector of file/directory names / مصفوفة من أسماء الملفات/المجلدات
 * 
 * Usage: محتويات = اسرد_مجلد("data")
 */
std::vector<std::string> list_directory(const std::string& path);

/**
 * @brief List directory with full paths - سرد مجلد بمسارات كاملة
 * @param path Directory path / مسار المجلد
 * @return Vector of full paths / مصفوفة من المسارات الكاملة
 * 
 * Usage: مسارات = اسرد_مجلد_كامل("data")
 */
std::vector<std::string> list_directory_full(const std::string& path);

/**
 * @brief List directory recursively - سرد مجلد تكرارياً
 * @param path Directory path / مسار المجلد
 * @return Vector of all paths recursively / مصفوفة من جميع المسارات تكرارياً
 * 
 * Usage: كل_الملفات = اسرد_مجلد_تكراري("data")
 */
std::vector<std::string> list_directory_recursive(const std::string& path);

/**
 * @brief Copy directory - نسخ مجلد
 * @param source Source path / المسار المصدر
 * @param destination Destination path / المسار الوجهة
 * @param recursive Copy recursively / النسخ تكرارياً
 * 
 * Usage: انسخ_مجلد("source_dir", "dest_dir", صحيح)
 */
void copy_directory(const std::string& source, const std::string& destination, bool recursive = true);

/**
 * @brief Remove directory - حذف مجلد
 * @param path Directory path / مسار المجلد
 * @param recursive Remove recursively / الحذف تكرارياً
 * @return true if removed / صحيح إذا تم الحذف
 * 
 * Usage: احذف_مجلد("temp_dir", صحيح)
 */
bool remove_directory(const std::string& path, bool recursive = false);

/**
 * @brief Get current working directory - الحصول على مجلد العمل الحالي
 * @return Current directory path / مسار المجلد الحالي
 * 
 * Usage: مجلد_حالي = احصل_على_مجلد_العمل()
 */
std::string get_current_directory();

/**
 * @brief Change current working directory - تغيير مجلد العمل الحالي
 * @param path New directory path / مسار المجلد الجديد
 * @return true if changed / صحيح إذا تم التغيير
 * 
 * Usage: غير_مجلد_العمل("data")
 */
bool change_directory(const std::string& path);

// ============================================================================
// Path Manipulation - معالجة المسارات
// ============================================================================

/**
 * @brief Join path components - دمج مكونات المسار
 * @param components Path components / مكونات المسار
 * @return Joined path / المسار المدمج
 * 
 * Usage: مسار = اجمع_مسار(["data", "files", "output.txt"])
 */
std::string join_path(const std::vector<std::string>& components);

/**
 * @brief Get absolute path - الحصول على المسار المطلق
 * @param path Relative or absolute path / مسار نسبي أو مطلق
 * @return Absolute path / المسار المطلق
 * 
 * Usage: مسار_مطلق = احصل_على_مسار_مطلق("data/file.txt")
 */
std::string get_absolute_path(const std::string& path);

/**
 * @brief Get relative path - الحصول على المسار النسبي
 * @param path Path to convert / المسار للتحويل
 * @param base Base path / المسار الأساسي
 * @return Relative path / المسار النسبي
 * 
 * Usage: مسار_نسبي = احصل_على_مسار_نسبي("/home/user/file.txt", "/home")
 */
std::string get_relative_path(const std::string& path, const std::string& base);

/**
 * @brief Normalize path - تطبيع المسار
 * @param path Path to normalize / المسار للتطبيع
 * @return Normalized path / المسار المطبّع
 * 
 * Usage: مسار = طبّع_مسار("data//files/../output.txt")
 */
std::string normalize_path(const std::string& path);

/**
 * @brief Get file name from path - الحصول على اسم الملف من المسار
 * @param path File path / مسار الملف
 * @return File name / اسم الملف
 * 
 * Usage: اسم = احصل_على_اسم_ملف("data/files/output.txt")  // "output.txt"
 */
std::string get_filename(const std::string& path);

/**
 * @brief Get file extension - الحصول على امتداد الملف
 * @param path File path / مسار الملف
 * @return File extension with dot / امتداد الملف مع النقطة
 * 
 * Usage: امتداد = احصل_على_امتداد("output.txt")  // ".txt"
 */
std::string get_extension(const std::string& path);

/**
 * @brief Get file stem (name without extension) - الحصول على اسم الملف بدون امتداد
 * @param path File path / مسار الملف
 * @return File name without extension / اسم الملف بدون امتداد
 * 
 * Usage: جذر = احصل_على_جذر_اسم("output.txt")  // "output"
 */
std::string get_stem(const std::string& path);

/**
 * @brief Get parent directory - الحصول على المجلد الأب
 * @param path File/directory path / مسار الملف/المجلد
 * @return Parent directory path / مسار المجلد الأب
 * 
 * Usage: مجلد_أب = احصل_على_مجلد_أب("data/files/output.txt")  // "data/files"
 */
std::string get_parent_path(const std::string& path);

/**
 * @brief Replace file extension - استبدال امتداد الملف
 * @param path File path / مسار الملف
 * @param new_extension New extension / الامتداد الجديد
 * @return Path with new extension / المسار بالامتداد الجديد
 * 
 * Usage: مسار_جديد = استبدل_امتداد("output.txt", ".json")  // "output.json"
 */
std::string replace_extension(const std::string& path, const std::string& new_extension);

// ============================================================================
// File Information - معلومات الملف
// ============================================================================

/**
 * @brief Check if path exists - التحقق من وجود المسار
 * @param path File/directory path / مسار الملف/المجلد
 * @return true if exists / صحيح إذا موجود
 * 
 * Usage: موجود = هل_موجود("data/file.txt")
 */
bool exists(const std::string& path);

/**
 * @brief Check if path is a file - التحقق من كون المسار ملفاً
 * @param path Path to check / المسار للتحقق
 * @return true if file / صحيح إذا ملف
 * 
 * Usage: هو_ملف = هل_ملف("data/file.txt")
 */
bool is_file(const std::string& path);

/**
 * @brief Check if path is a directory - التحقق من كون المسار مجلداً
 * @param path Path to check / المسار للتحقق
 * @return true if directory / صحيح إذا مجلد
 * 
 * Usage: هو_مجلد = هل_مجلد("data")
 */
bool is_directory(const std::string& path);

/**
 * @brief Check if path is a symbolic link - التحقق من كون المسار رابطاً رمزياً
 * @param path Path to check / المسار للتحقق
 * @return true if symlink / صحيح إذا رابط رمزي
 * 
 * Usage: هو_رابط = هل_رابط_رمزي("link")
 */
bool is_symlink(const std::string& path);

/**
 * @brief Get file size in bytes - الحصول على حجم الملف بالبايت
 * @param path File path / مسار الملف
 * @return File size / حجم الملف
 * 
 * Usage: حجم = احصل_على_حجم_ملف("data.txt")
 */
uint64_t get_file_size(const std::string& path);

/**
 * @brief Get last modification time - الحصول على وقت آخر تعديل
 * @param path File/directory path / مسار الملف/المجلد
 * @return Timestamp in seconds since epoch / الطابع الزمني بالثواني
 * 
 * Usage: وقت = احصل_على_وقت_تعديل("file.txt")
 */
int64_t get_last_write_time(const std::string& path);

/**
 * @brief Get available disk space - الحصول على المساحة المتاحة على القرص
 * @param path Path on disk / مسار على القرص
 * @return Available space in bytes / المساحة المتاحة بالبايت
 * 
 * Usage: مساحة = احصل_على_مساحة_متاحة("C:")
 */
uint64_t get_available_space(const std::string& path);

/**
 * @brief Get total disk space - الحصول على المساحة الكلية للقرص
 * @param path Path on disk / مسار على القرص
 * @return Total space in bytes / المساحة الكلية بالبايت
 * 
 * Usage: مساحة_كلية = احصل_على_مساحة_كلية("C:")
 */
uint64_t get_total_space(const std::string& path);

// ============================================================================
// File Permissions - صلاحيات الملفات
// ============================================================================

/**
 * @brief Check if file is readable - التحقق من إمكانية القراءة
 * @param path File path / مسار الملف
 * @return true if readable / صحيح إذا قابل للقراءة
 * 
 * Usage: قابل_قراءة = هل_قابل_للقراءة("file.txt")
 */
bool is_readable(const std::string& path);

/**
 * @brief Check if file is writable - التحقق من إمكانية الكتابة
 * @param path File path / مسار الملف
 * @return true if writable / صحيح إذا قابل للكتابة
 * 
 * Usage: قابل_كتابة = هل_قابل_للكتابة("file.txt")
 */
bool is_writable(const std::string& path);

/**
 * @brief Check if file is executable - التحقق من إمكانية التنفيذ
 * @param path File path / مسار الملف
 * @return true if executable / صحيح إذا قابل للتنفيذ
 * 
 * Usage: قابل_تنفيذ = هل_قابل_للتنفيذ("program.exe")
 */
bool is_executable(const std::string& path);

/**
 * @brief Set file permissions - تعيين صلاحيات الملف
 * @param path File path / مسار الملف
 * @param readable Readable permission / صلاحية القراءة
 * @param writable Writable permission / صلاحية الكتابة
 * @param executable Executable permission / صلاحية التنفيذ
 * 
 * Usage: عيّن_صلاحيات("file.txt", صحيح, صحيح, خطأ)
 */
void set_permissions(const std::string& path, bool readable, bool writable, bool executable);

// ============================================================================
// Temporary Files - الملفات المؤقتة
// ============================================================================

/**
 * @brief Get temporary directory path - الحصول على مسار مجلد الملفات المؤقتة
 * @return Temporary directory / مجلد الملفات المؤقتة
 * 
 * Usage: مجلد_مؤقت = احصل_على_مجلد_مؤقت()
 */
std::string get_temp_directory();

/**
 * @brief Create temporary file - إنشاء ملف مؤقت
 * @param prefix File name prefix / بادئة اسم الملف
 * @return Path to temporary file / مسار الملف المؤقت
 * 
 * Usage: ملف_مؤقت = أنشئ_ملف_مؤقت("temp_")
 */
std::string create_temp_file(const std::string& prefix = "sad_");

/**
 * @brief Create temporary directory - إنشاء مجلد مؤقت
 * @param prefix Directory name prefix / بادئة اسم المجلد
 * @return Path to temporary directory / مسار المجلد المؤقت
 * 
 * Usage: مجلد_مؤقت = أنشئ_مجلد_مؤقت("temp_")
 */
std::string create_temp_directory(const std::string& prefix = "sad_");

// ============================================================================
// File Watching - مراقبة الملفات
// ============================================================================

/**
 * @brief File watcher class - فئة مراقبة الملفات
 */
class FileWatcher {
public:
    enum class Event {
        Created,    // File created / ملف منشأ
        Modified,   // File modified / ملف معدّل
        Deleted,    // File deleted / ملف محذوف
        Renamed     // File renamed / ملف معاد تسمية
    };

    FileWatcher(const std::string& path, bool recursive = false);
    ~FileWatcher();

    /**
     * @brief Start watching - بدء المراقبة
     */
    void start();

    /**
     * @brief Stop watching - إيقاف المراقبة
     */
    void stop();

    /**
     * @brief Poll for events - استطلاع الأحداث
     * @return Vector of events / مصفوفة من الأحداث
     */
    std::vector<std::pair<Event, std::string>> poll_events();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// Glob Pattern Matching - مطابقة الأنماط
// ============================================================================

/**
 * @brief Match files using glob pattern - مطابقة الملفات باستخدام نمط glob
 * @param pattern Glob pattern (e.g., "*.txt") / نمط glob
 * @param base_path Base directory / المجلد الأساسي
 * @return Vector of matching paths / مصفوفة من المسارات المطابقة
 * 
 * Usage: ملفات = طابق_نمط("*.txt", "data")
 */
std::vector<std::string> glob(const std::string& pattern, const std::string& base_path = ".");

/**
 * @brief Match path against pattern - مطابقة مسار مع نمط
 * @param path Path to match / المسار للمطابقة
 * @param pattern Glob pattern / نمط glob
 * @return true if matches / صحيح إذا يطابق
 * 
 * Usage: يطابق = هل_يطابق_نمط("file.txt", "*.txt")
 */
bool matches_pattern(const std::string& path, const std::string& pattern);

} // namespace filesystem
} // namespace stdlib
} // namespace sad
