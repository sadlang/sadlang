/**
 * @file sad_android_io.c
 * @brief (AR) تطبيق نظام الإدخال/الإخراج والملفات
 * @brief (EN) I/O and file system implementation
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق نظام الملفات — لغة ص لأندرويد
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يستخدم مكتبة C القياسية لعمليات الملفات (POSIX) المدعومة في bionic.
 * على أندرويد يُوجّه الإخراج إلى logcat عبر __android_log_print.
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#include "sad_android_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

/* اكتشاف أندرويد لتوجيه الإخراج */
#ifdef __ANDROID__
  #include <android/log.h>
  #include <android/asset_manager.h>
  #include <android/asset_manager_jni.h>
  #define SAD_LOG_TAG "SadLang"
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — مُتغيّرات عامة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) مؤشر مدير الأصول (AAssetManager)
 *
 * يُهيّأ مرة واحدة عند بدء التطبيق عبر sad_assets_init().
 * على الحاسوب يكون NULL ونستخدم ملفات عادية بدلاً منه.
 */
static void* g_asset_manager = NULL;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — أدوات داخلية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تحويل وضع الفتح إلى نص mode string لـ fopen
 *
 *  الوضع      │ نصي │ ثنائي
 *  ───────────┼─────┼──────
 *  READ       │ "r" │ "rb"
 *  WRITE      │ "w" │ "wb"
 *  APPEND     │ "a" │ "ab"
 *  READWRITE  │ "r+"│ "rb+"
 */
static const char* mode_to_string(SadFileMode mode, bool binary) {
    switch (mode) {
        case SAD_FILE_READ:
            return binary ? "rb" : "r";
        case SAD_FILE_WRITE:
            return binary ? "wb" : "w";
        case SAD_FILE_APPEND:
            return binary ? "ab" : "a";
        case SAD_FILE_READWRITE:
            return binary ? "rb+" : "r+";
        default:
            return "r";
    }
}

/**
 * @brief (AR) الحصول على حجم الملف من المؤشر
 */
static int64_t get_file_size(FILE* fp) {
    if (!fp) return -1;

    long current = ftell(fp);
    if (fseek(fp, 0, SEEK_END) != 0) return -1;
    long size = ftell(fp);
    fseek(fp, current, SEEK_SET);

    return (int64_t)size;
}

/**
 * @brief (AR) إنشاء جميع المجلدات في المسار
 *
 * مثال: "/data/app/a/b/c" يُنشئ a, a/b, a/b/c
 * مشابه لـ mkdir -p
 */
static bool mkdirs(const char* path) {
    if (!path || strlen(path) == 0) return false;

    char tmp[SAD_IO_MAX_PATH];
    size_t len = strlen(path);
    if (len >= SAD_IO_MAX_PATH) return false;

    memcpy(tmp, path, len + 1);

    for (size_t i = 1; i < len; i++) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                return false;
            }
            tmp[i] = '/';
        }
    }

    return mkdir(tmp, 0755) == 0 || errno == EEXIST;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — عمليات الملفات
 * ═══════════════════════════════════════════════════════════════════════════ */

SadFile* sad_file_open(const char* path, SadFileMode mode, bool binary) {
    if (!path) return NULL;

    const char* mode_str = mode_to_string(mode, binary);
    FILE* fp = fopen(path, mode_str);
    if (!fp) return NULL;

    SadFile* file = (SadFile*)sad_alloc(sizeof(SadFile));
    if (!file) {
        fclose(fp);
        return NULL;
    }

    file->fp = fp;
    file->path = sad_string_from_cstr(path);
    file->mode = mode;
    file->is_binary = binary;
    file->is_open = true;
    file->size = get_file_size(fp);

    return file;
}

void sad_file_close(SadFile* file) {
    if (!file || !file->is_open) return;

    if (file->fp) {
        fclose((FILE*)file->fp);
        file->fp = NULL;
    }
    file->is_open = false;
}

void sad_file_free(SadFile* file) {
    if (!file) return;

    sad_file_close(file);
    sad_string_free(&file->path);
    sad_free(file);
}

SadString sad_file_read_all(const char* path) {
    if (!path) return sad_string_empty();

    FILE* fp = fopen(path, "rb");
    if (!fp) return sad_string_empty();

    /* حساب الحجم */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0 || (size_t)size > SAD_IO_MAX_READ_SIZE) {
        fclose(fp);
        return sad_string_empty();
    }

    /* قراءة المحتوى */
    char* buffer = (char*)sad_alloc((size_t)size + 1);
    if (!buffer) {
        fclose(fp);
        return sad_string_empty();
    }

    size_t read_count = fread(buffer, 1, (size_t)size, fp);
    fclose(fp);

    buffer[read_count] = '\0';

    SadString result = sad_string_new(buffer, read_count);
    sad_free(buffer);

    return result;
}

bool sad_file_write_all(const char* path, const SadString* content) {
    if (!path || !content) return false;

    FILE* fp = fopen(path, "wb");
    if (!fp) return false;

    const char* data = sad_string_data(content);
    size_t len = sad_string_length(content);

    size_t written = fwrite(data, 1, len, fp);
    fclose(fp);

    return written == len;
}

int64_t sad_file_read(SadFile* file, void* buffer, size_t size) {
    if (!file || !file->fp || !file->is_open || !buffer || size == 0) {
        return -1;
    }

    size_t read_count = fread(buffer, 1, size, (FILE*)file->fp);
    if (read_count == 0 && ferror((FILE*)file->fp)) {
        return -1;
    }

    return (int64_t)read_count;
}

int64_t sad_file_write(SadFile* file, const void* data, size_t size) {
    if (!file || !file->fp || !file->is_open || !data || size == 0) {
        return -1;
    }

    size_t written = fwrite(data, 1, size, (FILE*)file->fp);
    if (written == 0 && ferror((FILE*)file->fp)) {
        return -1;
    }

    return (int64_t)written;
}

SadString sad_file_read_line(SadFile* file) {
    if (!file || !file->fp || !file->is_open) {
        return sad_string_empty();
    }

    /*
     * قراءة سطر بدون حدود حجم مسبقة:
     * ١. نبدأ بمخزن صغير
     * ٢. نُوسّعه إذا السطر أطول
     * ٣. نُزيل \n و \r\n من النهاية
     */
    size_t capacity = 256;
    size_t length = 0;
    char* buffer = (char*)sad_alloc(capacity);
    if (!buffer) return sad_string_empty();

    while (1) {
        int ch = fgetc((FILE*)file->fp);
        if (ch == EOF || ch == '\n') break;

        if (length + 1 >= capacity) {
            capacity *= 2;
            char* new_buf = (char*)sad_alloc(capacity);
            if (!new_buf) {
                sad_free(buffer);
                return sad_string_empty();
            }
            memcpy(new_buf, buffer, length);
            sad_free(buffer);
            buffer = new_buf;
        }

        buffer[length++] = (char)ch;
    }

    /* إزالة \r في نهاية السطر (Windows \r\n) */
    if (length > 0 && buffer[length - 1] == '\r') {
        length--;
    }

    buffer[length] = '\0';
    SadString result = sad_string_new(buffer, length);
    sad_free(buffer);

    return result;
}

bool sad_file_write_line(SadFile* file, const SadString* line) {
    if (!file || !file->fp || !file->is_open || !line) return false;

    const char* data = sad_string_data(line);
    size_t len = sad_string_length(line);

    if (fwrite(data, 1, len, (FILE*)file->fp) != len) return false;
    if (fwrite("\n", 1, 1, (FILE*)file->fp) != 1) return false;

    return true;
}

bool sad_file_seek(SadFile* file, int64_t offset, SadSeekFrom from) {
    if (!file || !file->fp || !file->is_open) return false;

    int whence;
    switch (from) {
        case SAD_SEEK_BEGIN:   whence = SEEK_SET; break;
        case SAD_SEEK_CURRENT: whence = SEEK_CUR; break;
        case SAD_SEEK_END:     whence = SEEK_END; break;
        default:               whence = SEEK_SET; break;
    }

    return fseek((FILE*)file->fp, (long)offset, whence) == 0;
}

int64_t sad_file_tell(SadFile* file) {
    if (!file || !file->fp || !file->is_open) return -1;
    return (int64_t)ftell((FILE*)file->fp);
}

bool sad_file_flush(SadFile* file) {
    if (!file || !file->fp || !file->is_open) return false;
    return fflush((FILE*)file->fp) == 0;
}

bool sad_file_eof(SadFile* file) {
    if (!file || !file->fp || !file->is_open) return true;
    return feof((FILE*)file->fp) != 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — عمليات نظام الملفات
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_file_exists(const char* path) {
    if (!path) return false;
    return access(path, F_OK) == 0;
}

bool sad_is_directory(const char* path) {
    if (!path) return false;

    struct stat st;
    if (stat(path, &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

bool sad_file_info(const char* path, SadFileInfo* info) {
    if (!path || !info) return false;

    struct stat st;
    if (stat(path, &st) != 0) return false;

    /* اسم الملف (الجزء بعد آخر /) */
    const char* name = strrchr(path, '/');
    info->name = sad_string_from_cstr(name ? name + 1 : path);
    info->full_path = sad_string_from_cstr(path);
    info->size = (int64_t)st.st_size;
    info->modified_time = (int64_t)st.st_mtime;
    info->created_time = (int64_t)st.st_ctime;
    info->is_readable = (access(path, R_OK) == 0);
    info->is_writable = (access(path, W_OK) == 0);

    if (S_ISDIR(st.st_mode)) {
        info->type = SAD_ENTRY_DIRECTORY;
    } else if (S_ISLNK(st.st_mode)) {
        info->type = SAD_ENTRY_SYMLINK;
    } else if (S_ISREG(st.st_mode)) {
        info->type = SAD_ENTRY_FILE;
    } else {
        info->type = SAD_ENTRY_OTHER;
    }

    return true;
}

int64_t sad_file_size(const char* path) {
    if (!path) return -1;

    struct stat st;
    if (stat(path, &st) != 0) return -1;
    return (int64_t)st.st_size;
}

bool sad_file_delete(const char* path) {
    if (!path) return false;
    return unlink(path) == 0;
}

bool sad_file_copy(const char* src, const char* dst) {
    if (!src || !dst) return false;

    FILE* in = fopen(src, "rb");
    if (!in) return false;

    FILE* out = fopen(dst, "wb");
    if (!out) {
        fclose(in);
        return false;
    }

    char buffer[SAD_IO_BUFFER_SIZE];
    size_t read_count;
    bool success = true;

    while ((read_count = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        if (fwrite(buffer, 1, read_count, out) != read_count) {
            success = false;
            break;
        }
    }

    fclose(out);
    fclose(in);
    return success;
}

bool sad_file_move(const char* src, const char* dst) {
    if (!src || !dst) return false;

    /* محاولة rename أولاً (فورية إذا على نفس نظام الملفات) */
    if (rename(src, dst) == 0) return true;

    /* إذا فشل rename (أنظمة ملفات مختلفة)، ننسخ ثم نحذف */
    if (sad_file_copy(src, dst)) {
        unlink(src);
        return true;
    }

    return false;
}

bool sad_mkdir(const char* path) {
    if (!path) return false;
    return mkdirs(path);
}

bool sad_rmdir(const char* path) {
    if (!path) return false;
    return rmdir(path) == 0;
}

bool sad_dir_list(const char* path, SadDirEntry** out_entries, size_t* out_count) {
    if (!path || !out_entries || !out_count) return false;

    DIR* dir = opendir(path);
    if (!dir) return false;

    /* مرحلة ١: عدّ العناصر (باستثناء . و ..) */
    size_t count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        count++;
    }

    if (count == 0) {
        closedir(dir);
        *out_entries = NULL;
        *out_count = 0;
        return true;
    }

    /* مرحلة ٢: إعادة القراءة وتخزين النتائج */
    rewinddir(dir);

    SadDirEntry* entries = (SadDirEntry*)sad_alloc(sizeof(SadDirEntry) * count);
    if (!entries) {
        closedir(dir);
        return false;
    }

    size_t idx = 0;
    while ((entry = readdir(dir)) != NULL && idx < count) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        entries[idx].name = sad_string_from_cstr(entry->d_name);

        /* تحديد النوع */
        #ifdef _DIRENT_HAVE_D_TYPE
        switch (entry->d_type) {
            case DT_DIR: entries[idx].type = SAD_ENTRY_DIRECTORY; break;
            case DT_LNK: entries[idx].type = SAD_ENTRY_SYMLINK; break;
            case DT_REG: entries[idx].type = SAD_ENTRY_FILE; break;
            default:     entries[idx].type = SAD_ENTRY_OTHER; break;
        }
        #else
        /* fallback: استخدام stat */
        char full[SAD_IO_MAX_PATH];
        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(full, &st) == 0) {
            if (S_ISDIR(st.st_mode))      entries[idx].type = SAD_ENTRY_DIRECTORY;
            else if (S_ISREG(st.st_mode)) entries[idx].type = SAD_ENTRY_FILE;
            else                           entries[idx].type = SAD_ENTRY_OTHER;
        } else {
            entries[idx].type = SAD_ENTRY_OTHER;
        }
        #endif

        idx++;
    }

    closedir(dir);

    *out_entries = entries;
    *out_count = idx;
    return true;
}

void sad_dir_list_free(SadDirEntry* entries, size_t count) {
    if (!entries) return;

    for (size_t i = 0; i < count; i++) {
        sad_string_free(&entries[i].name);
    }
    sad_free(entries);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عمليات المسار
 * ═══════════════════════════════════════════════════════════════════════════ */

SadString sad_path_join(const char* base, const char* child) {
    if (!base && !child) return sad_string_empty();
    if (!base) return sad_string_from_cstr(child);
    if (!child) return sad_string_from_cstr(base);

    size_t base_len = strlen(base);
    size_t child_len = strlen(child);

    /* إزالة / من نهاية base */
    while (base_len > 0 && base[base_len - 1] == '/') base_len--;

    /* إزالة / من بداية child */
    while (child_len > 0 && child[0] == '/') {
        child++;
        child_len--;
    }

    /* base + "/" + child */
    size_t total = base_len + 1 + child_len;
    char* buf = (char*)sad_alloc(total + 1);
    if (!buf) return sad_string_empty();

    memcpy(buf, base, base_len);
    buf[base_len] = '/';
    memcpy(buf + base_len + 1, child, child_len);
    buf[total] = '\0';

    SadString result = sad_string_new(buf, total);
    sad_free(buf);
    return result;
}

SadString sad_path_filename(const char* path) {
    if (!path) return sad_string_empty();

    const char* last_sep = strrchr(path, '/');
    const char* name = last_sep ? last_sep + 1 : path;
    return sad_string_from_cstr(name);
}

SadString sad_path_extension(const char* path) {
    if (!path) return sad_string_empty();

    const char* last_dot = strrchr(path, '.');
    const char* last_sep = strrchr(path, '/');

    /* تأكد أن النقطة بعد آخر فاصل مسار */
    if (!last_dot || (last_sep && last_dot < last_sep)) {
        return sad_string_empty();
    }

    return sad_string_from_cstr(last_dot);
}

SadString sad_path_parent(const char* path) {
    if (!path) return sad_string_empty();

    const char* last_sep = strrchr(path, '/');
    if (!last_sep || last_sep == path) {
        return last_sep == path ? sad_string_from_cstr("/") : sad_string_empty();
    }

    return sad_string_new(path, (size_t)(last_sep - path));
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — أصول التطبيق (Android Assets)
 * ═══════════════════════════════════════════════════════════════════════════ */

void sad_assets_init(void* asset_manager) {
    g_asset_manager = asset_manager;
}

SadString sad_asset_read(const char* asset_path) {
    if (!asset_path) return sad_string_empty();

#ifdef __ANDROID__
    /*
     * على أندرويد: نستخدم AAssetManager لقراءة الأصول
     * المُدمجة في APK.
     */
    if (!g_asset_manager) return sad_string_empty();

    AAssetManager* mgr = (AAssetManager*)g_asset_manager;
    AAsset* asset = AAssetManager_open(mgr, asset_path, AASSET_MODE_BUFFER);
    if (!asset) return sad_string_empty();

    off_t size = AAsset_getLength(asset);
    if (size <= 0) {
        AAsset_close(asset);
        return sad_string_empty();
    }

    const void* data = AAsset_getBuffer(asset);
    SadString result = sad_string_new((const char*)data, (size_t)size);
    AAsset_close(asset);
    return result;

#else
    /*
     * على الحاسوب (للاختبار):
     * نقرأ من مجلد "assets/" نسبياً
     */
    char full_path[SAD_IO_MAX_PATH];
    snprintf(full_path, sizeof(full_path), "assets/%s", asset_path);
    return sad_file_read_all(full_path);
#endif
}

bool sad_asset_read_binary(const char* asset_path, void** out_data, size_t* out_size) {
    if (!asset_path || !out_data || !out_size) return false;

#ifdef __ANDROID__
    if (!g_asset_manager) return false;

    AAssetManager* mgr = (AAssetManager*)g_asset_manager;
    AAsset* asset = AAssetManager_open(mgr, asset_path, AASSET_MODE_BUFFER);
    
    // محاولة البحث بالامتداد .sad إذا فشلت القراءة (لتوافق الحروف العربية مع أداة aapt)
    if (!asset && strstr(asset_path, ".ص")) {
        char fallback_path[SAD_IO_MAX_PATH];
        strncpy(fallback_path, asset_path, sizeof(fallback_path) - 1);
        char* pos = strstr(fallback_path, ".ص");
        if (pos) {
            strcpy(pos, ".sad");
            asset = AAssetManager_open(mgr, fallback_path, AASSET_MODE_BUFFER);
        }
    }
    
    if (!asset) return false;

    off_t size = AAsset_getLength(asset);
    if (size <= 0) {
        AAsset_close(asset);
        return false;
    }

    void* buffer = sad_alloc((size_t)size);
    if (!buffer) {
        AAsset_close(asset);
        return false;
    }

    int read_count = AAsset_read(asset, buffer, (size_t)size);
    AAsset_close(asset);

    if (read_count != size) {
        sad_free(buffer);
        return false;
    }

    *out_data = buffer;
    *out_size = (size_t)size;
    return true;

#else
    /* على الحاسوب */
    char full_path[SAD_IO_MAX_PATH];
    snprintf(full_path, sizeof(full_path), "assets/%s", asset_path);

    FILE* fp = fopen(full_path, "rb");
    if (!fp) return false;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0) {
        fclose(fp);
        return false;
    }

    void* buffer = sad_alloc((size_t)size);
    if (!buffer) {
        fclose(fp);
        return false;
    }

    size_t read_count = fread(buffer, 1, (size_t)size, fp);
    fclose(fp);

    *out_data = buffer;
    *out_size = read_count;
    return true;
#endif
}

bool sad_asset_exists(const char* asset_path) {
    if (!asset_path) return false;

#ifdef __ANDROID__
    if (!g_asset_manager) return false;

    AAssetManager* mgr = (AAssetManager*)g_asset_manager;
    AAsset* asset = AAssetManager_open(mgr, asset_path, AASSET_MODE_UNKNOWN);
    if (asset) {
        AAsset_close(asset);
        return true;
    }
    return false;

#else
    char full_path[SAD_IO_MAX_PATH];
    snprintf(full_path, sizeof(full_path), "assets/%s", asset_path);
    return access(full_path, F_OK) == 0;
#endif
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — الإخراج والتسجيل
 * ═══════════════════════════════════════════════════════════════════════════ */

void sad_print(const char* text) {
    if (!text) return;

#ifdef __ANDROID__
    /* على أندرويد: توجيه إلى logcat */
    __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, "%s", text);
#else
    fputs(text, stdout);
    fflush(stdout);
#endif
}

void sad_println(const char* text) {
    if (!text) return;

#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, "%s", text);
#else
    puts(text);
    fflush(stdout);
#endif
}

void sad_printf(const char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

#ifdef __ANDROID__
    __android_log_vprint(ANDROID_LOG_INFO, SAD_LOG_TAG, format, args);
#else
    vprintf(format, args);
    fflush(stdout);
#endif

    va_end(args);
}

void sad_log_error(const char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

#ifdef __ANDROID__
    __android_log_vprint(ANDROID_LOG_ERROR, SAD_LOG_TAG, format, args);
#else
    fprintf(stderr, "[خطأ] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
#endif

    va_end(args);
}

void sad_log_warn(const char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

#ifdef __ANDROID__
    __android_log_vprint(ANDROID_LOG_WARN, SAD_LOG_TAG, format, args);
#else
    fprintf(stderr, "[تحذير] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);
#endif

    va_end(args);
}

void sad_log_info(const char* format, ...) {
    if (!format) return;

    va_list args;
    va_start(args, format);

#ifdef __ANDROID__
    __android_log_vprint(ANDROID_LOG_INFO, SAD_LOG_TAG, format, args);
#else
    printf("[معلومة] ");
    vprintf(format, args);
    printf("\n");
    fflush(stdout);
#endif

    va_end(args);
}
