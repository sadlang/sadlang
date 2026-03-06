/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ نظام الملفات — POSIX + Android
 *  File System Implementation — POSIX + Android
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_files.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SadFiles", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SadFiles", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGI(...) do { printf("[SadFiles] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGE(...) do { fprintf(stderr, "[SadFiles] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  هيكل داخلي
// ═══════════════════════════════════════════════════════════════════════════════

struct مقبض_ملفات {
    char مسار_التطبيق[512];
};

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/** بناء المسار الكامل (مطلق أو نسبي لمسار التطبيق) */
static void build_full_path(مقبض_ملفات* م, const char* المسار, char* out, size_t outSize) {
    if (المسار[0] == '/') {
        // مسار مطلق
        strncpy(out, المسار, outSize - 1);
        out[outSize - 1] = '\0';
    } else {
        // مسار نسبي لمجلد التطبيق
        snprintf(out, outSize, "%s/%s", م->مسار_التطبيق, المسار);
    }
}

/** إنشاء مجلدات وسيطة (mkdir -p) */
static bool mkdirs(const char* path) {
    char tmp[512];
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    
    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    return mkdir(tmp, 0755) == 0 || errno == EEXIST;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء وتدمير
// ═══════════════════════════════════════════════════════════════════════════════

مقبض_ملفات* أنشئ_نظام_ملفات(const char* مسار_التطبيق) {
    مقبض_ملفات* م = calloc(1, sizeof(مقبض_ملفات));
    if (!م) return NULL;
    
    if (مسار_التطبيق) {
        strncpy(م->مسار_التطبيق, مسار_التطبيق, sizeof(م->مسار_التطبيق) - 1);
    } else {
        strcpy(م->مسار_التطبيق, ".");
    }
    
    LOGI("تم إنشاء نظام الملفات — المسار: %s", م->مسار_التطبيق);
    return م;
}

void دمر_نظام_ملفات(مقبض_ملفات* الملفات) {
    if (!الملفات) return;
    LOGI("تم تدمير نظام الملفات");
    free(الملفات);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  قراءة وكتابة ملفات نصية
// ═══════════════════════════════════════════════════════════════════════════════

char* اقرأ_ملف_نصي(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return NULL;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    FILE* f = fopen(fullPath, "rb");
    if (!f) {
        LOGE("فشل فتح الملف للقراءة: %s (%s)", fullPath, strerror(errno));
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size < 0) {
        fclose(f);
        return NULL;
    }
    
    char* buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    
    size_t read = fread(buf, 1, (size_t)size, f);
    buf[read] = '\0';
    fclose(f);
    
    LOGI("قراءة %zu bytes من %s", read, fullPath);
    return buf;
}

حالة_ملف اكتب_ملف_نصي(مقبض_ملفات* م, const char* المسار, const char* المحتوى) {
    if (!م || !المسار || !المحتوى) return ملف_مسار_خاطئ;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    // إنشاء المجلدات الوسيطة
    char dirPath[512];
    strncpy(dirPath, fullPath, sizeof(dirPath) - 1);
    dirPath[sizeof(dirPath) - 1] = '\0';
    char* lastSlash = strrchr(dirPath, '/');
    if (lastSlash) {
        *lastSlash = '\0';
        mkdirs(dirPath);
    }
    
    FILE* f = fopen(fullPath, "wb");
    if (!f) {
        LOGE("فشل فتح الملف للكتابة: %s (%s)", fullPath, strerror(errno));
        return ملف_خطأ_كتابة;
    }
    
    size_t len = strlen(المحتوى);
    size_t written = fwrite(المحتوى, 1, len, f);
    fclose(f);
    
    if (written != len) return ملف_خطأ_كتابة;
    
    LOGI("كتابة %zu bytes إلى %s", written, fullPath);
    return ملف_نجاح;
}

حالة_ملف أضف_لملف(مقبض_ملفات* م, const char* المسار, const char* المحتوى) {
    if (!م || !المسار || !المحتوى) return ملف_مسار_خاطئ;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    FILE* f = fopen(fullPath, "ab");
    if (!f) {
        LOGE("فشل فتح الملف للإضافة: %s", fullPath);
        return ملف_خطأ_كتابة;
    }
    
    size_t len = strlen(المحتوى);
    size_t written = fwrite(المحتوى, 1, len, f);
    fclose(f);
    
    return written == len ? ملف_نجاح : ملف_خطأ_كتابة;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  قراءة وكتابة ملفات ثنائية
// ═══════════════════════════════════════════════════════════════════════════════

uint8_t* اقرأ_ملف_ثنائي(مقبض_ملفات* م, const char* المسار, size_t* الحجم) {
    if (!م || !المسار || !الحجم) return NULL;
    *الحجم = 0;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    FILE* f = fopen(fullPath, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size <= 0) {
        fclose(f);
        return NULL;
    }
    
    uint8_t* buf = malloc((size_t)size);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    
    *الحجم = fread(buf, 1, (size_t)size, f);
    fclose(f);
    return buf;
}

حالة_ملف اكتب_ملف_ثنائي(مقبض_ملفات* م, const char* المسار,
                          const uint8_t* البيانات, size_t الحجم) {
    if (!م || !المسار || !البيانات) return ملف_مسار_خاطئ;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    // إنشاء المجلدات الوسيطة
    char dirPath[512];
    strncpy(dirPath, fullPath, sizeof(dirPath) - 1);
    dirPath[sizeof(dirPath) - 1] = '\0';
    char* lastSlash = strrchr(dirPath, '/');
    if (lastSlash) {
        *lastSlash = '\0';
        mkdirs(dirPath);
    }
    
    FILE* f = fopen(fullPath, "wb");
    if (!f) return ملف_خطأ_كتابة;
    
    size_t written = fwrite(البيانات, 1, الحجم, f);
    fclose(f);
    
    return written == الحجم ? ملف_نجاح : ملف_خطأ_كتابة;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عمليات الملفات
// ═══════════════════════════════════════════════════════════════════════════════

bool ملف_موجود(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return false;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    struct stat st;
    return stat(fullPath, &st) == 0 && S_ISREG(st.st_mode);
}

حالة_ملف احذف_ملف(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return ملف_مسار_خاطئ;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    if (unlink(fullPath) != 0) {
        if (errno == ENOENT) return ملف_غير_موجود;
        if (errno == EACCES) return ملف_لا_صلاحية;
        return ملف_خطأ_عام;
    }
    
    LOGI("تم حذف: %s", fullPath);
    return ملف_نجاح;
}

حالة_ملف انقل_ملف(مقبض_ملفات* م, const char* المسار_القديم, const char* المسار_الجديد) {
    if (!م || !المسار_القديم || !المسار_الجديد) return ملف_مسار_خاطئ;
    
    char oldFull[512], newFull[512];
    build_full_path(م, المسار_القديم, oldFull, sizeof(oldFull));
    build_full_path(م, المسار_الجديد, newFull, sizeof(newFull));
    
    if (rename(oldFull, newFull) != 0) {
        LOGE("فشل نقل %s → %s: %s", oldFull, newFull, strerror(errno));
        return ملف_خطأ_عام;
    }
    
    LOGI("نقل: %s → %s", oldFull, newFull);
    return ملف_نجاح;
}

حالة_ملف انسخ_ملف(مقبض_ملفات* م, const char* المصدر, const char* الهدف) {
    if (!م || !المصدر || !الهدف) return ملف_مسار_خاطئ;
    
    size_t حجم = 0;
    uint8_t* بيانات = اقرأ_ملف_ثنائي(م, المصدر, &حجم);
    if (!بيانات) return ملف_خطأ_قراءة;
    
    حالة_ملف نتيجة = اكتب_ملف_ثنائي(م, الهدف, بيانات, حجم);
    free(بيانات);
    return نتيجة;
}

int64_t حجم_ملف(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return -1;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    struct stat st;
    if (stat(fullPath, &st) != 0) return -1;
    return (int64_t)st.st_size;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عمليات المجلدات
// ═══════════════════════════════════════════════════════════════════════════════

حالة_ملف أنشئ_مجلد(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return ملف_مسار_خاطئ;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    if (!mkdirs(fullPath)) {
        return ملف_خطأ_عام;
    }
    
    LOGI("تم إنشاء مجلد: %s", fullPath);
    return ملف_نجاح;
}

bool مجلد_موجود(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return false;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    struct stat st;
    return stat(fullPath, &st) == 0 && S_ISDIR(st.st_mode);
}

قائمة_عناصر استعرض_مجلد(مقبض_ملفات* م, const char* المسار) {
    قائمة_عناصر ق = {NULL, 0};
    if (!م || !المسار) return ق;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    DIR* dir = opendir(fullPath);
    if (!dir) return ق;
    
    // عد العناصر أولاً
    int count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        count++;
    }
    rewinddir(dir);
    
    if (count == 0) {
        closedir(dir);
        return ق;
    }
    
    ق.العناصر = calloc(count, sizeof(معلومات_عنصر));
    if (!ق.العناصر) {
        closedir(dir);
        return ق;
    }
    
    int idx = 0;
    while ((entry = readdir(dir)) != NULL && idx < count) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        ق.العناصر[idx].الاسم = strdup(entry->d_name);
        
        char itemPath[512];
        snprintf(itemPath, sizeof(itemPath), "%s/%s", fullPath, entry->d_name);
        ق.العناصر[idx].المسار_الكامل = strdup(itemPath);
        
        struct stat st;
        if (stat(itemPath, &st) == 0) {
            ق.العناصر[idx].الحجم = (size_t)st.st_size;
            ق.العناصر[idx].تاريخ_التعديل = (int64_t)st.st_mtime;
            if (S_ISDIR(st.st_mode))
                ق.العناصر[idx].النوع = نوع_مجلد;
            else if (S_ISLNK(st.st_mode))
                ق.العناصر[idx].النوع = نوع_رابط;
            else
                ق.العناصر[idx].النوع = نوع_ملف;
        } else {
            ق.العناصر[idx].النوع = نوع_مجهول;
        }
        
        idx++;
    }
    
    ق.العدد = idx;
    closedir(dir);
    
    LOGI("استعراض %s: %d عناصر", fullPath, idx);
    return ق;
}

حالة_ملف احذف_مجلد(مقبض_ملفات* م, const char* المسار) {
    if (!م || !المسار) return ملف_مسار_خاطئ;
    
    char fullPath[512];
    build_full_path(م, المسار, fullPath, sizeof(fullPath));
    
    if (rmdir(fullPath) != 0) {
        if (errno == ENOENT) return ملف_غير_موجود;
        if (errno == ENOTEMPTY) return ملف_خطأ_عام;
        return ملف_خطأ_عام;
    }
    
    LOGI("تم حذف مجلد: %s", fullPath);
    return ملف_نجاح;
}

void حرر_قائمة_عناصر(قائمة_عناصر* ق) {
    if (!ق || !ق->العناصر) return;
    for (int i = 0; i < ق->العدد; i++) {
        free(ق->العناصر[i].الاسم);
        free(ق->العناصر[i].المسار_الكامل);
    }
    free(ق->العناصر);
    ق->العناصر = NULL;
    ق->العدد = 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  مسارات مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

char* ادمج_مسار(const char* أساس, const char* فرعي) {
    if (!أساس && !فرعي) return NULL;
    if (!أساس) return strdup(فرعي);
    if (!فرعي) return strdup(أساس);
    
    size_t len1 = strlen(أساس);
    size_t len2 = strlen(فرعي);
    bool needSlash = (len1 > 0 && أساس[len1 - 1] != '/');
    
    char* result = malloc(len1 + len2 + 2);
    if (!result) return NULL;
    
    if (needSlash)
        snprintf(result, len1 + len2 + 2, "%s/%s", أساس, فرعي);
    else
        snprintf(result, len1 + len2 + 2, "%s%s", أساس, فرعي);
    
    return result;
}

char* اسم_الملف(const char* المسار) {
    if (!المسار) return NULL;
    
    const char* lastSlash = strrchr(المسار, '/');
    if (lastSlash)
        return strdup(lastSlash + 1);
    return strdup(المسار);
}

char* امتداد_ملف(const char* المسار) {
    if (!المسار) return NULL;
    
    const char* name = strrchr(المسار, '/');
    if (!name) name = المسار; else name++;
    
    const char* dot = strrchr(name, '.');
    if (!dot || dot == name)
        return strdup("");
    return strdup(dot + 1);
}

const char* مسار_التطبيق_الداخلي(مقبض_ملفات* م) {
    if (!م) return NULL;
    return م->مسار_التطبيق;
}
