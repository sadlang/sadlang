/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ نظام قواعد البيانات — SQLite3
 *  Database System Implementation — SQLite3
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يستخدم SQLite3 amalgamation مُضمّن مباشرة في التطبيق
 */

#include "sad_database.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* استخدام SQLite3 المُضمّن مباشرة */
#include "sqlite3.h"

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SadDB", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SadDB", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGI(...) do { printf("[SadDB] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGE(...) do { fprintf(stderr, "[SadDB] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل داخلية
// ═══════════════════════════════════════════════════════════════════════════════

struct مقبض_قاعدة {
    sqlite3* db;
    char مسار[512];
};

struct أمر_محضّر {
    sqlite3_stmt* stmt;
    مقبض_قاعدة* القاعدة;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

static حالة_قاعدة map_sqlite_error(int rc) {
    switch (rc) {
        case SQLITE_OK:
        case SQLITE_DONE: return قاعدة_نجاح;
        case SQLITE_BUSY: return قاعدة_خطأ_قفل;
        case SQLITE_ERROR: return قاعدة_خطأ_تنفيذ;
        default: return قاعدة_خطأ_عام;
    }
}

static نوع_عمود map_column_type(int sqliteType) {
    switch (sqliteType) {
        case SQLITE_INTEGER: return عمود_عدد;
        case SQLITE_FLOAT:   return عمود_عشري;
        case SQLITE_TEXT:    return عمود_نص;
        case SQLITE_BLOB:    return عمود_بيانات;
        case SQLITE_NULL:    return عمود_فارغ;
        default:             return عمود_نص;
    }
}

/** تنفيذ SELECT وتجميع النتائج */
static نتيجة_استعلام collect_results(sqlite3_stmt* stmt, مقبض_قاعدة* ق) {
    نتيجة_استعلام ن = {0};
    
    int colCount = sqlite3_column_count(stmt);
    ن.عدد_الأعمدة = colCount;
    
    // أسماء الأعمدة
    ن.أسماء_الأعمدة = calloc(colCount, sizeof(char*));
    if (!ن.أسماء_الأعمدة) {
        ن.الحالة = قاعدة_خطأ_ذاكرة;
        return ن;
    }
    for (int i = 0; i < colCount; i++) {
        const char* name = sqlite3_column_name(stmt, i);
        ن.أسماء_الأعمدة[i] = name ? strdup(name) : strdup("?");
    }
    
    // تجميع الصفوف
    int capacity = 32;
    int count = 0;
    ن.الصفوف = calloc(capacity, sizeof(صف_نتيجة));
    if (!ن.الصفوف) {
        ن.الحالة = قاعدة_خطأ_ذاكرة;
        return ن;
    }
    
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (count >= capacity) {
            capacity *= 2;
            صف_نتيجة* newRows = realloc(ن.الصفوف, capacity * sizeof(صف_نتيجة));
            if (!newRows) break;
            ن.الصفوف = newRows;
        }
        
        صف_نتيجة* صف = &ن.الصفوف[count];
        صف->عدد_الأعمدة = colCount;
        صف->القيم = calloc(colCount, sizeof(char*));
        صف->الأنواع = calloc(colCount, sizeof(نوع_عمود));
        
        if (!صف->القيم || !صف->الأنواع) break;
        
        for (int i = 0; i < colCount; i++) {
            صف->الأنواع[i] = map_column_type(sqlite3_column_type(stmt, i));
            const char* val = (const char*)sqlite3_column_text(stmt, i);
            صف->القيم[i] = val ? strdup(val) : NULL;
        }
        
        count++;
    }
    
    ن.عدد_الصفوف = count;
    ن.الحالة = (rc == SQLITE_DONE || rc == SQLITE_ROW) ? قاعدة_نجاح : map_sqlite_error(rc);
    
    if (ن.الحالة != قاعدة_نجاح) {
        ن.رسالة_الخطأ = strdup(sqlite3_errmsg(ق->db));
    }
    
    return ن;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  فتح وإغلاق
// ═══════════════════════════════════════════════════════════════════════════════

مقبض_قاعدة* افتح_قاعدة_بيانات(const char* المسار) {
    if (!المسار) return NULL;
    
    مقبض_قاعدة* ق = calloc(1, sizeof(مقبض_قاعدة));
    if (!ق) return NULL;
    
    strncpy(ق->مسار, المسار, sizeof(ق->مسار) - 1);
    
    int rc = sqlite3_open_v2(المسار, &ق->db,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) {
        LOGE("فشل فتح قاعدة البيانات: %s — %s", المسار,
             ق->db ? sqlite3_errmsg(ق->db) : "unknown");
        if (ق->db) sqlite3_close(ق->db);
        free(ق);
        return NULL;
    }
    
    // تفعيل WAL mode للأداء
    sqlite3_exec(ق->db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    // تفعيل UTF-8
    sqlite3_exec(ق->db, "PRAGMA encoding='UTF-8';", NULL, NULL, NULL);
    
    LOGI("تم فتح قاعدة البيانات: %s", المسار);
    return ق;
}

void أغلق_قاعدة_بيانات(مقبض_قاعدة* ق) {
    if (!ق) return;
    if (ق->db) {
        sqlite3_close(ق->db);
    }
    LOGI("تم إغلاق قاعدة البيانات: %s", ق->مسار);
    free(ق);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تنفيذ مباشر
// ═══════════════════════════════════════════════════════════════════════════════

حالة_قاعدة نفذ_أمر(مقبض_قاعدة* ق, const char* sql) {
    if (!ق || !ق->db || !sql) return قاعدة_خطأ_عام;
    
    char* err = NULL;
    int rc = sqlite3_exec(ق->db, sql, NULL, NULL, &err);
    
    if (rc != SQLITE_OK) {
        LOGE("خطأ SQL: %s — %s", sql, err ? err : "?");
        if (err) sqlite3_free(err);
        return map_sqlite_error(rc);
    }
    
    if (err) sqlite3_free(err);
    return قاعدة_نجاح;
}

نتيجة_استعلام نفذ_استعلام(مقبض_قاعدة* ق, const char* sql) {
    نتيجة_استعلام ن = {0};
    
    if (!ق || !ق->db || !sql) {
        ن.الحالة = قاعدة_خطأ_عام;
        ن.رسالة_الخطأ = strdup("معاملات غير صالحة");
        return ن;
    }
    
    sqlite3_stmt* stmt = NULL;
    int rc = sqlite3_prepare_v2(ق->db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK || !stmt) {
        ن.الحالة = قاعدة_خطأ_صيغة;
        ن.رسالة_الخطأ = strdup(sqlite3_errmsg(ق->db));
        LOGE("خطأ تحضير: %s", ن.رسالة_الخطأ);
        return ن;
    }
    
    ن = collect_results(stmt, ق);
    sqlite3_finalize(stmt);
    
    return ن;
}

void حرر_نتيجة(نتيجة_استعلام* ن) {
    if (!ن) return;
    
    // أسماء الأعمدة
    if (ن->أسماء_الأعمدة) {
        for (int i = 0; i < ن->عدد_الأعمدة; i++) {
            free(ن->أسماء_الأعمدة[i]);
        }
        free(ن->أسماء_الأعمدة);
    }
    
    // الصفوف
    if (ن->الصفوف) {
        for (int i = 0; i < ن->عدد_الصفوف; i++) {
            صف_نتيجة* s = &ن->الصفوف[i];
            if (s->القيم) {
                for (int j = 0; j < s->عدد_الأعمدة; j++) {
                    free(s->القيم[j]);
                }
                free(s->القيم);
            }
            free(s->الأنواع);
        }
        free(ن->الصفوف);
    }
    
    free(ن->رسالة_الخطأ);
    memset(ن, 0, sizeof(نتيجة_استعلام));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  أوامر محضّرة
// ═══════════════════════════════════════════════════════════════════════════════

أمر_محضّر* حضّر_أمر(مقبض_قاعدة* ق, const char* sql) {
    if (!ق || !ق->db || !sql) return NULL;
    
    أمر_محضّر* أ = calloc(1, sizeof(أمر_محضّر));
    if (!أ) return NULL;
    
    أ->القاعدة = ق;
    
    int rc = sqlite3_prepare_v2(ق->db, sql, -1, &أ->stmt, NULL);
    if (rc != SQLITE_OK || !أ->stmt) {
        LOGE("فشل تحضير: %s — %s", sql, sqlite3_errmsg(ق->db));
        free(أ);
        return NULL;
    }
    
    return أ;
}

حالة_قاعدة اربط_عدد(أمر_محضّر* أ, int الترتيب, int64_t القيمة) {
    if (!أ || !أ->stmt) return قاعدة_خطأ_عام;
    int rc = sqlite3_bind_int64(أ->stmt, الترتيب, القيمة);
    return map_sqlite_error(rc);
}

حالة_قاعدة اربط_عشري(أمر_محضّر* أ, int الترتيب, double القيمة) {
    if (!أ || !أ->stmt) return قاعدة_خطأ_عام;
    int rc = sqlite3_bind_double(أ->stmt, الترتيب, القيمة);
    return map_sqlite_error(rc);
}

حالة_قاعدة اربط_نص(أمر_محضّر* أ, int الترتيب, const char* القيمة) {
    if (!أ || !أ->stmt) return قاعدة_خطأ_عام;
    int rc = sqlite3_bind_text(أ->stmt, الترتيب, القيمة, -1, SQLITE_TRANSIENT);
    return map_sqlite_error(rc);
}

حالة_قاعدة اربط_فارغ(أمر_محضّر* أ, int الترتيب) {
    if (!أ || !أ->stmt) return قاعدة_خطأ_عام;
    int rc = sqlite3_bind_null(أ->stmt, الترتيب);
    return map_sqlite_error(rc);
}

حالة_قاعدة نفذ_محضّر(أمر_محضّر* أ) {
    if (!أ || !أ->stmt) return قاعدة_خطأ_عام;
    int rc = sqlite3_step(أ->stmt);
    return (rc == SQLITE_DONE) ? قاعدة_نجاح : map_sqlite_error(rc);
}

نتيجة_استعلام استعلم_محضّر(أمر_محضّر* أ) {
    نتيجة_استعلام ن = {0};
    if (!أ || !أ->stmt || !أ->القاعدة) {
        ن.الحالة = قاعدة_خطأ_عام;
        return ن;
    }
    return collect_results(أ->stmt, أ->القاعدة);
}

حالة_قاعدة أعد_تعيين_محضّر(أمر_محضّر* أ) {
    if (!أ || !أ->stmt) return قاعدة_خطأ_عام;
    sqlite3_reset(أ->stmt);
    sqlite3_clear_bindings(أ->stmt);
    return قاعدة_نجاح;
}

void دمر_محضّر(أمر_محضّر* أ) {
    if (!أ) return;
    if (أ->stmt) sqlite3_finalize(أ->stmt);
    free(أ);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  المعاملات
// ═══════════════════════════════════════════════════════════════════════════════

حالة_قاعدة ابدأ_معاملة(مقبض_قاعدة* ق) {
    return نفذ_أمر(ق, "BEGIN TRANSACTION;");
}

حالة_قاعدة أكد_معاملة(مقبض_قاعدة* ق) {
    return نفذ_أمر(ق, "COMMIT;");
}

حالة_قاعدة تراجع_عن_معاملة(مقبض_قاعدة* ق) {
    return نفذ_أمر(ق, "ROLLBACK;");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/** إيجاد فهرس عمود بالاسم */
static int find_column_index(نتيجة_استعلام* ن, const char* اسم_العمود) {
    if (!ن || !اسم_العمود || !ن->أسماء_الأعمدة) return -1;
    for (int i = 0; i < ن->عدد_الأعمدة; i++) {
        if (ن->أسماء_الأعمدة[i] && strcmp(ن->أسماء_الأعمدة[i], اسم_العمود) == 0)
            return i;
    }
    return -1;
}

const char* قيمة_عمود_نص(نتيجة_استعلام* ن, int صف, const char* اسم_العمود) {
    if (!ن || صف < 0 || صف >= ن->عدد_الصفوف) return NULL;
    int col = find_column_index(ن, اسم_العمود);
    if (col < 0) return NULL;
    return ن->الصفوف[صف].القيم[col];
}

int64_t قيمة_عمود_عدد(نتيجة_استعلام* ن, int صف, const char* اسم_العمود) {
    const char* val = قيمة_عمود_نص(ن, صف, اسم_العمود);
    return val ? strtoll(val, NULL, 10) : 0;
}

double قيمة_عمود_عشري(نتيجة_استعلام* ن, int صف, const char* اسم_العمود) {
    const char* val = قيمة_عمود_نص(ن, صف, اسم_العمود);
    return val ? strtod(val, NULL) : 0.0;
}

int الصفوف_المتأثرة(مقبض_قاعدة* ق) {
    if (!ق || !ق->db) return 0;
    return sqlite3_changes(ق->db);
}

int64_t آخر_معرف_إدراج(مقبض_قاعدة* ق) {
    if (!ق || !ق->db) return 0;
    return sqlite3_last_insert_rowid(ق->db);
}

const char* خطأ_القاعدة(مقبض_قاعدة* ق) {
    if (!ق || !ق->db) return "لا يوجد اتصال";
    return sqlite3_errmsg(ق->db);
}
