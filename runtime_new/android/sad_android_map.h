/**
 * @file sad_android_map.h
 * @brief (AR) نظام الخرائط (جداول التجزئة) لرانتايم أندرويد — لغة ص
 * @brief (EN) Hash map system for Android runtime — Sad Language
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام الخرائط (Hash Map) — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الخريطة (SadMap) هي جدول تجزئة يربط مفاتيح (سلاسل نصية) بقيم
 * ديناميكية (SadArrayValue). مُحسّن لأسماء المتغيرات العربية.
 *
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║                    تصميم الخريطة                               ║
 * ╠══════════════════════════════════════════════════════════════════╣
 * ║                                                                ║
 * ║  نمط: Open Addressing مع Robin Hood Hashing                   ║
 * ║                                                                ║
 * ║  لماذا Robin Hood Hashing؟                                    ║
 * ║  ─────────────────────────                                     ║
 * ║  • أداء مستقر: فرق المسافة بين العناصر يبقى صغيراً            ║
 * ║  • ودّي للـ cache: العناصر متجاورة في الذاكرة                  ║
 * ║  • أبسط من chaining: لا قوائم مرتبطة                          ║
 * ║  • ممتاز لعامل تحميل ≤ 0.75                                   ║
 * ║                                                                ║
 * ║  البنية في الذاكرة:                                           ║
 * ║  ┌─────────┬────────────────────────────────────────┐          ║
 * ║  │ SadMap  │ entries[] (مصفوفة متصلة)               │          ║
 * ║  │  ↓      │ ┌──────┬──────┬──────┬──────┬──────┐   │          ║
 * ║  │ count   │ │entry0│entry1│ فارغ │entry3│ فارغ │   │          ║
 * ║  │ cap     │ └──────┴──────┴──────┴──────┴──────┘   │          ║
 * ║  └─────────┴────────────────────────────────────────┘          ║
 * ║                                                                ║
 * ║  كل إدخال (SadMapEntry):                                      ║
 * ║  ┌──────────────────────────────────────────┐                  ║
 * ║  │ key_hash: uint64_t (تجزئة FNV-1a)       │                  ║
 * ║  │ key:      SadString (المفتاح)            │  ~80 بايت       ║
 * ║  │ value:    SadArrayValue (القيمة)         │                  ║
 * ║  │ distance: uint32_t (مسافة Robin Hood)    │                  ║
 * ║  │ occupied: bool (هل مشغول؟)               │                  ║
 * ║  └──────────────────────────────────────────┘                  ║
 * ║                                                                ║
 * ║  عامل التحميل الأقصى: 0.75                                    ║
 * ║  عند تجاوزه: إعادة تجزئة بضعف السعة                          ║
 * ║                                                                ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#ifndef SAD_ANDROID_MAP_H
#define SAD_ANDROID_MAP_H

#include "sad_android_memory.h"
#include "sad_android_string.h"
#include "sad_android_array.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — ثوابت الخريطة
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) السعة الابتدائية — يجب أن تكون قوة 2 */
#define SAD_MAP_INITIAL_CAPACITY 16

/**
 * @brief (AR) عامل التحميل الأقصى (75%)
 *
 * عند تجاوز هذا العامل، نُعيد التجزئة بضعف السعة.
 * 75% يُعطي توازناً بين:
 *  • استغلال الذاكرة (أفضل من 50%)
 *  • تقليل التصادمات (أفضل من 90%)
 */
#define SAD_MAP_LOAD_FACTOR_NUM   3
#define SAD_MAP_LOAD_FACTOR_DEN   4

/** @brief (AR) أقصى سعة (16M إدخال) */
#define SAD_MAP_MAX_CAPACITY (1024 * 1024 * 16)

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — بنية الإدخال
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إدخال واحد في الخريطة
 *
 * يحتوي على:
 *  • key_hash: تجزئة المفتاح (محسوبة مسبقاً لتسريع البحث)
 *  • key: نسخة من المفتاح (SadString)
 *  • value: القيمة المُخزّنة
 *  • distance: مسافة Robin Hood — كم خطوة ابتعد هذا الإدخال عن موقعه المثالي
 *  • occupied: هل هذا الموقع مشغول؟
 */
typedef struct SadMapEntry {
    uint64_t        key_hash;     /**< تجزئة FNV-1a للمفتاح */
    SadString       key;          /**< المفتاح */
    SadArrayValue   value;        /**< القيمة */
    uint32_t        distance;     /**< مسافة Robin Hood */
    bool            occupied;     /**< مشغول أم فارغ */
} SadMapEntry;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — بنية الخريطة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) الخريطة — جدول تجزئة بتقنية Robin Hood
 *
 * مُدارة بالعدّ المرجعي. عند تحرير آخر مرجع:
 *  ١. تُحرّر كل المفاتيح (SadString)
 *  ٢. تُحرّر كل القيم المرجعية
 *  ٣. تُحرّر مصفوفة الإدخالات
 *  ٤. تُحرّر البنية نفسها
 */
typedef struct SadMap {
    SadMapEntry*    entries;      /**< مصفوفة الإدخالات */
    size_t          count;        /**< عدد الإدخالات المشغولة */
    size_t          capacity;     /**< سعة المصفوفة (قوة 2 دائماً) */
} SadMap;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — إنشاء وتدمير الخريطة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إنشاء خريطة جديدة فارغة
 * @return مؤشر للخريطة، أو NULL عند فشل التخصيص
 */
SadMap* sad_map_new(void);

/**
 * @brief (AR) إنشاء خريطة بسعة محددة
 * @param initial_capacity السعة المبدئية (تُقرّب لأقرب قوة 2)
 * @return مؤشر للخريطة
 */
SadMap* sad_map_with_capacity(size_t initial_capacity);

/**
 * @brief (AR) تحرير الخريطة
 * @param map الخريطة
 */
void sad_map_free(SadMap* map);

/**
 * @brief (AR) الاحتفاظ بمرجع (زيادة عدّ مرجعي)
 */
SadMap* sad_map_retain(SadMap* map);

/**
 * @brief (AR) نسخ عميق للخريطة
 */
SadMap* sad_map_clone(const SadMap* map);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عمليات القراءة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) الحصول على قيمة بمفتاح
 * @param map الخريطة
 * @param key المفتاح
 * @return مؤشر للقيمة، أو NULL إذا لم يُوجد المفتاح
 *
 * تعقيد: O(1) في المتوسط.
 * لا تُغيّر ملكية القيمة — المؤشر صالح فقط
 * طالما الخريطة موجودة ولم تُعدّل.
 */
SadArrayValue* sad_map_get(const SadMap* map, const SadString* key);

/**
 * @brief (AR) البحث بمفتاح نصي C
 * @param map الخريطة
 * @param key المفتاح كسلسلة C
 * @return مؤشر للقيمة أو NULL
 */
SadArrayValue* sad_map_get_cstr(const SadMap* map, const char* key);

/**
 * @brief (AR) هل يحتوي المفتاح؟
 */
bool sad_map_contains(const SadMap* map, const SadString* key);

/**
 * @brief (AR) هل يحتوي مفتاح C؟
 */
bool sad_map_contains_cstr(const SadMap* map, const char* key);

/** @brief (AR) عدد الإدخالات */
size_t sad_map_count(const SadMap* map);

/** @brief (AR) هل الخريطة فارغة؟ */
bool sad_map_is_empty(const SadMap* map);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — عمليات الكتابة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إضافة أو تحديث إدخال
 * @param map الخريطة
 * @param key المفتاح
 * @param value القيمة
 * @return true إذا كان إدراجاً جديداً، false إذا كان تحديثاً
 *
 * Robin Hood: عند التصادم، العنصر الأبعد عن موقعه المثالي
 * "يسرق" الموقع من العنصر الأقرب — مما يوازن المسافات.
 */
bool sad_map_set(SadMap* map, const SadString* key, SadArrayValue value);

/**
 * @brief (AR) إضافة بمفتاح C
 */
bool sad_map_set_cstr(SadMap* map, const char* key, SadArrayValue value);

/**
 * @brief (AR) حذف إدخال
 * @param map الخريطة
 * @param key المفتاح
 * @param out_value (اختياري) لتخزين القيمة المحذوفة
 * @return true إذا وُجد وحُذف
 */
bool sad_map_remove(SadMap* map, const SadString* key, SadArrayValue* out_value);

/**
 * @brief (AR) حذف بمفتاح C
 */
bool sad_map_remove_cstr(SadMap* map, const char* key, SadArrayValue* out_value);

/**
 * @brief (AR) مسح جميع الإدخالات
 */
void sad_map_clear(SadMap* map);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — التكرار
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) نوع دالة التكرار على الخريطة
 *
 * تُستدعى لكل إدخال. ترجع:
 *  true = استمر في التكرار
 *  false = توقف
 */
typedef bool (*SadMapIterFn)(const SadString* key, SadArrayValue* value, void* context);

/**
 * @brief (AR) التكرار على كل إدخالات الخريطة
 * @param map الخريطة
 * @param fn دالة الاستدعاء
 * @param context بيانات سياق
 *
 * الترتيب غير مضمون.
 */
void sad_map_foreach(SadMap* map, SadMapIterFn fn, void* context);

/**
 * @brief (AR) الحصول على كل المفاتيح كمصفوفة
 * @param map الخريطة
 * @return مصفوفة سلاسل نصية (يجب تحريرها بعد الاستخدام)
 */
SadArray* sad_map_keys(const SadMap* map);

/**
 * @brief (AR) الحصول على كل القيم كمصفوفة
 * @param map الخريطة
 * @return مصفوفة قيم
 */
SadArray* sad_map_values(const SadMap* map);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_MAP_H */
