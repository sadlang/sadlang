/**
 * @file sad_android_io.h
 * @brief (AR) نظام الإدخال/الإخراج والملفات — لغة ص لأندرويد
 * @brief (EN) I/O and file system header for Sad Android runtime
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام الملفات والإدخال/الإخراج
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يوفر واجهة موحّدة للتعامل مع الملفات على أندرويد:
 *
 *  ┌──────────────────────────────────────────────┐
 *  │              واجهة لغة ص (API)                │
 *  ├──────────────┬──────────────┬────────────────┤
 *  │  ملفات عادية  │  أصول التطبيق │   التخزين     │
 *  │   (POSIX)    │   (Assets)   │  المشترك      │
 *  ├──────────────┼──────────────┼────────────────┤
 *  │     fopen    │ AAssetManager│  SharedPrefs   │
 *  │   read/write │   AAsset     │  (key-value)   │
 *  └──────────────┴──────────────┴────────────────┘
 *
 * أنواع التخزين على أندرويد:
 * ─────────────────────────
 *
 * ١. التخزين الداخلي (Internal Storage):
 *    • /data/data/com.app/files/
 *    • خاص بالتطبيق — لا يحتاج إذن
 *    • يُحذف مع إلغاء التطبيق
 *
 * ٢. التخزين الخارجي (External Storage):
 *    • /sdcard/Android/data/com.app/
 *    • يحتاج إذن STORAGE (أندرويد < 10)
 *    • قد يتوفر/لا يتوفر
 *
 * ٣. أصول التطبيق (Assets):
 *    • ملفات مُدمجة في APK (للقراءة فقط)
 *    • صور، أصوات، ملفات إعداد
 *    • عبر AAssetManager
 *
 * ٤. ذاكرة التخزين المؤقت (Cache):
 *    • /data/data/com.app/cache/
 *    • يُحذف تلقائياً عند نقص المساحة
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#ifndef SAD_ANDROID_IO_H
#define SAD_ANDROID_IO_H

#include "sad_android_memory.h"
#include "sad_android_string.h"
#include "sad_android_array.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — ثوابت ومُعرّفات
 * ═══════════════════════════════════════════════════════════════════════════ */

/** حجم المخزن المؤقت الافتراضي للقراءة/الكتابة */
#define SAD_IO_BUFFER_SIZE       8192

/** أقصى حجم لمسار الملف */
#define SAD_IO_MAX_PATH          4096

/** أقصى حجم ملف مسموح بقراءته دفعة واحدة (16 ميجابايت) */
#define SAD_IO_MAX_READ_SIZE     (16 * 1024 * 1024)

/**
 * @brief (AR) أوضاع فتح الملف
 *
 *  الوضع │ الوصف
 *  ──────┼──────────────────
 *  READ  │ قراءة فقط
 *  WRITE │ كتابة (مسح المحتوى)
 *  APPEND│ إضافة في النهاية
 *  READW │ قراءة وكتابة
 */
typedef enum {
    SAD_FILE_READ    = 0,  /* "r" أو "rb" */
    SAD_FILE_WRITE   = 1,  /* "w" أو "wb" */
    SAD_FILE_APPEND  = 2,  /* "a" أو "ab" */
    SAD_FILE_READWRITE = 3 /* "r+" أو "rb+" */
} SadFileMode;

/**
 * @brief (AR) نوع عنصر المجلد
 */
typedef enum {
    SAD_ENTRY_FILE = 0,       /* ملف عادي       */
    SAD_ENTRY_DIRECTORY = 1,  /* مجلد           */
    SAD_ENTRY_SYMLINK = 2,    /* رابط رمزي      */
    SAD_ENTRY_OTHER = 3       /* نوع آخر        */
} SadEntryType;

/**
 * @brief (AR) موضع البحث في الملف
 */
typedef enum {
    SAD_SEEK_BEGIN   = 0,  /* من بداية الملف */
    SAD_SEEK_CURRENT = 1,  /* من الموضع الحالي */
    SAD_SEEK_END     = 2   /* من نهاية الملف */
} SadSeekFrom;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — هياكل البيانات
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) ملف مفتوح
 *
 * يُغلّف مؤشر FILE* من مكتبة C القياسية.
 * يدعم القراءة والكتابة بالنص (UTF-8) أو البايتات.
 *
 *  ┌─────────────────────┐
 *  │     SadFile          │
 *  ├─────────────────────┤
 *  │ fp       : FILE*    │  مؤشر الملف
 *  │ path     : SadString│  مسار الملف
 *  │ mode     : enum     │  وضع الفتح
 *  │ is_binary: bool     │  وضع ثنائي؟
 *  │ is_open  : bool     │  مفتوح؟
 *  │ size     : int64    │  حجم الملف (إن عُرف)
 *  └─────────────────────┘
 */
typedef struct SadFile {
    void*       fp;          /* FILE* — نستخدم void* لتجنب #include <stdio.h> */
    SadString   path;        /* مسار الملف الكامل                              */
    SadFileMode mode;        /* وضع الفتح                                     */
    bool        is_binary;   /* true = وضع ثنائي (b)                         */
    bool        is_open;     /* true = الملف مفتوح حالياً                     */
    int64_t     size;        /* حجم الملف بالبايت (-1 إذا غير معروف)         */
} SadFile;

/**
 * @brief (AR) معلومات ملف/مجلد
 *
 * تُعاد من دالة sad_file_info() — معلومات الحالة.
 */
typedef struct SadFileInfo {
    SadString    name;          /* اسم الملف (بدون المسار)     */
    SadString    full_path;     /* المسار الكامل              */
    SadEntryType type;          /* نوع العنصر                 */
    int64_t      size;          /* الحجم بالبايت              */
    int64_t      modified_time; /* آخر تعديل (Unix timestamp) */
    int64_t      created_time;  /* وقت الإنشاء               */
    bool         is_readable;   /* قابل للقراءة؟             */
    bool         is_writable;   /* قابل للكتابة؟             */
} SadFileInfo;

/**
 * @brief (AR) عنصر في المجلد (لسرد المحتويات)
 */
typedef struct SadDirEntry {
    SadString    name;  /* اسم العنصر */
    SadEntryType type;  /* نوع العنصر */
} SadDirEntry;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — عمليات الملفات
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) فتح ملف
 * @param path مسار الملف
 * @param mode وضع الفتح (قراءة/كتابة/إضافة)
 * @param binary true لفتح بوضع ثنائي
 * @return مؤشر SadFile أو NULL عند الفشل
 */
SadFile* sad_file_open(const char* path, SadFileMode mode, bool binary);

/**
 * @brief (AR) إغلاق الملف
 */
void sad_file_close(SadFile* file);

/**
 * @brief (AR) تحرير الملف وموارده
 */
void sad_file_free(SadFile* file);

/**
 * @brief (AR) قراءة محتوى الملف بالكامل كنص
 * @param path مسار الملف
 * @return SadString تحتوي محتوى الملف (فارغة عند الفشل)
 *
 * مثال بلغة ص:
 *   متغير محتوى = اقرأ_ملف("بيانات.txt")
 */
SadString sad_file_read_all(const char* path);

/**
 * @brief (AR) كتابة نص كامل إلى ملف
 * @param path مسار الملف (يُنشأ إذا لم يوجد)
 * @param content المحتوى المراد كتابته
 * @return true عند النجاح
 *
 * مثال بلغة ص:
 *   اكتب_ملف("رسالة.txt"، "مرحباً بالعالم!")
 */
bool sad_file_write_all(const char* path, const SadString* content);

/**
 * @brief (AR) قراءة عدد محدد من البايتات
 * @param file الملف المفتوح
 * @param buffer المخزن المؤقت
 * @param size عدد البايتات المطلوبة
 * @return عدد البايتات المقروءة فعلاً (-1 عند الفشل)
 */
int64_t sad_file_read(SadFile* file, void* buffer, size_t size);

/**
 * @brief (AR) كتابة بايتات إلى الملف
 * @param file الملف المفتوح
 * @param data البيانات
 * @param size عدد البايتات
 * @return عدد البايتات المكتوبة فعلاً (-1 عند الفشل)
 */
int64_t sad_file_write(SadFile* file, const void* data, size_t size);

/**
 * @brief (AR) قراءة سطر واحد من الملف
 * @param file الملف المفتوح
 * @return SadString تحتوي السطر (بدون \n)
 */
SadString sad_file_read_line(SadFile* file);

/**
 * @brief (AR) كتابة سطر (مع \n)
 */
bool sad_file_write_line(SadFile* file, const SadString* line);

/**
 * @brief (AR) تحريك مؤشر القراءة/الكتابة
 * @param file الملف المفتوح
 * @param offset الإزاحة بالبايت
 * @param from نقطة البداية
 * @return true عند النجاح
 */
bool sad_file_seek(SadFile* file, int64_t offset, SadSeekFrom from);

/**
 * @brief (AR) الحصول على موضع المؤشر الحالي
 */
int64_t sad_file_tell(SadFile* file);

/**
 * @brief (AR) مسح المخزن المؤقت (flush)
 */
bool sad_file_flush(SadFile* file);

/**
 * @brief (AR) هل وصلنا لنهاية الملف؟
 */
bool sad_file_eof(SadFile* file);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — عمليات نظام الملفات
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) هل الملف/المجلد موجود؟
 *
 * مثال بلغة ص:
 *   إذا (ملف_موجود("إعدادات.json"))
 *      اطبع("الإعدادات موجودة")
 *   نهاية
 */
bool sad_file_exists(const char* path);

/**
 * @brief (AR) هل المسار مجلد؟
 */
bool sad_is_directory(const char* path);

/**
 * @brief (AR) الحصول على معلومات الملف
 * @param path مسار الملف
 * @param info مؤشر لهيكل المعلومات (يُملأ عند النجاح)
 * @return true عند النجاح
 */
bool sad_file_info(const char* path, SadFileInfo* info);

/**
 * @brief (AR) حجم الملف بالبايت
 * @return الحجم أو -1 عند الفشل
 */
int64_t sad_file_size(const char* path);

/**
 * @brief (AR) حذف ملف
 */
bool sad_file_delete(const char* path);

/**
 * @brief (AR) نسخ ملف
 * @param src المسار المصدر
 * @param dst المسار الهدف
 */
bool sad_file_copy(const char* src, const char* dst);

/**
 * @brief (AR) نقل/إعادة تسمية ملف
 */
bool sad_file_move(const char* src, const char* dst);

/**
 * @brief (AR) إنشاء مجلد (مع المجلدات الأبوية إن لزم)
 *
 * مثال بلغة ص:
 *   أنشئ_مجلد("بيانات/صور/مصغرات")
 */
bool sad_mkdir(const char* path);

/**
 * @brief (AR) حذف مجلد (فارغ فقط)
 */
bool sad_rmdir(const char* path);

/**
 * @brief (AR) سرد محتويات مجلد
 * @param path مسار المجلد
 * @param out_entries مصفوفة لتخزين النتائج (يجب تحريرها لاحقاً)
 * @param out_count عدد العناصر
 * @return true عند النجاح
 */
bool sad_dir_list(const char* path, SadDirEntry** out_entries, size_t* out_count);

/**
 * @brief (AR) تحرير نتائج سرد المجلد
 */
void sad_dir_list_free(SadDirEntry* entries, size_t count);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عمليات المسار
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) ضم مكونات المسار
 *
 * مثال: sad_path_join("مجلد", "ملف.txt") → "مجلد/ملف.txt"
 */
SadString sad_path_join(const char* base, const char* child);

/**
 * @brief (AR) استخراج اسم الملف من المسار
 *
 * مثال: sad_path_filename("/data/app/ملف.txt") → "ملف.txt"
 */
SadString sad_path_filename(const char* path);

/**
 * @brief (AR) استخراج امتداد الملف
 *
 * مثال: sad_path_extension("صورة.png") → ".png"
 */
SadString sad_path_extension(const char* path);

/**
 * @brief (AR) استخراج المجلد الأب
 *
 * مثال: sad_path_parent("/data/app/ملف.txt") → "/data/app"
 */
SadString sad_path_parent(const char* path);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — أصول التطبيق (Android Assets)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تهيئة نظام الأصول
 *
 * يجب استدعاؤها مرة واحدة عند بدء التطبيق.
 * على أندرويد: تأخذ مؤشر AAssetManager من NativeActivity.
 * على الحاسوب (للاختبار): تستخدم مجلد assets/ عادي.
 *
 * @param asset_manager مؤشر AAssetManager (أو NULL للحاسوب)
 */
void sad_assets_init(void* asset_manager);

/**
 * @brief (AR) قراءة أصل (asset) بالكامل
 * @param asset_path المسار داخل مجلد assets/
 * @return SadString تحتوي المحتوى (فارغة عند الفشل)
 *
 * مثال بلغة ص:
 *   متغير إعدادات = اقرأ_أصل("config/settings.json")
 */
SadString sad_asset_read(const char* asset_path);

/**
 * @brief (AR) قراءة أصل كبايتات خام
 * @param asset_path المسار
 * @param out_data مؤشر لتخزين البيانات (يجب تحريره لاحقاً)
 * @param out_size حجم البيانات
 * @return true عند النجاح
 */
bool sad_asset_read_binary(const char* asset_path, void** out_data, size_t* out_size);

/**
 * @brief (AR) هل الأصل موجود؟
 */
bool sad_asset_exists(const char* asset_path);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — الإخراج القياسي (طباعة)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) طباعة نص (بدون سطر جديد)
 *
 * على أندرويد: تُوجّه إلى __android_log_print (logcat)
 * على الحاسوب: تطبع على stdout
 *
 * مُرتبطة بدالة اطبع() في لغة ص.
 */
void sad_print(const char* text);

/**
 * @brief (AR) طباعة نص مع سطر جديد
 *
 * مُرتبطة بدالة اطبع_سطر() في لغة ص.
 */
void sad_println(const char* text);

/**
 * @brief (AR) طباعة منسّقة (printf-style)
 */
void sad_printf(const char* format, ...);

/**
 * @brief (AR) تسجيل رسالة خطأ
 *
 * على أندرويد: تُوجّه إلى ANDROID_LOG_ERROR
 * على الحاسوب: تطبع على stderr
 */
void sad_log_error(const char* format, ...);

/**
 * @brief (AR) تسجيل رسالة تحذيرية
 */
void sad_log_warn(const char* format, ...);

/**
 * @brief (AR) تسجيل رسالة معلوماتية
 */
void sad_log_info(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_IO_H */
