/**
 * @file sad_android_array.h
 * @brief (AR) نظام المصفوفات الديناميكية لرانتايم أندرويد — لغة ص
 * @brief (EN) Dynamic array system for Android runtime — Sad Language
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام المصفوفات الديناميكية — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * المصفوفة (SadArray) هي بنية بيانات ديناميكية تُخزّن عناصر من نوع
 * موحّد (SadValue) بحجم متغيّر. تدعم النمو التلقائي والعدّ المرجعي.
 *
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║                  تصميم المصفوفة                                ║
 * ╠══════════════════════════════════════════════════════════════════╣
 * ║                                                                ║
 * ║ SadArray (على الـ heap):                                       ║
 * ║ ┌──────────────────────────────────────────────────────────┐   ║
 * ║ │ header: SadObjectHeader (32 بايت — عدد مرجعي + نوع)    │   ║
 * ║ │ data:   SadValue* → مؤشر لمصفوفة العناصر               │   ║
 * ║ │ length: عدد العناصر الحالية                              │   ║
 * ║ │ capacity: السعة المُخصصة                                │   ║
 * ║ │ elem_size: حجم العنصر الواحد (بايت)                     │   ║
 * ║ │ elem_type: نوع العنصر (SadValueType)                    │   ║
 * ║ └──────────────────────────────────────────────────────────┘   ║
 * ║                         ↓                                      ║
 * ║ البيانات (مصفوفة مستمرة):                                     ║
 * ║ ┌────────┬────────┬────────┬────────┬─── ───┐                 ║
 * ║ │ val[0] │ val[1] │ val[2] │ val[3] │  ...  │                 ║
 * ║ └────────┴────────┴────────┴────────┴─── ───┘                 ║
 * ║                                                                ║
 * ║ استراتيجية النمو:                                             ║
 * ║  • السعة الأولية: 8 عناصر                                    ║
 * ║  • عامل النمو: 1.5x (توازن بين الذاكرة والأداء)             ║
 * ║  • يمكن إنشاء مصفوفة بسعة محددة مسبقاً                      ║
 * ║                                                                ║
 * ║ العدّ المرجعي:                                                ║
 * ║  • المصفوفة نفسها مُحصاة بالمرجع                             ║
 * ║  • العناصر (SadValue) لها عدّها المرجعي الخاص                ║
 * ║  • عند حذف المصفوفة، نُنقص مراجع كل عنصر                    ║
 * ║                                                                ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#ifndef SAD_ANDROID_ARRAY_H
#define SAD_ANDROID_ARRAY_H

#include "sad_android_memory.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — ثوابت المصفوفة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) السعة الابتدائية الافتراضية
 *
 * ٨ عناصر — موازنة بين:
 *  • تقليل عدد مرات إعادة التخصيص للمصفوفات الصغيرة
 *  • عدم هدر الذاكرة للمصفوفات التي لن تكبر
 */
#define SAD_ARRAY_INITIAL_CAPACITY 8

/**
 * @brief (AR) عامل النمو (بسط/مقام لحساب بالأعداد الصحيحة)
 *
 * ١.٥ = 3/2
 * أفضل من 2x لأنه يسمح بإعادة استخدام الكتل المُحرّرة سابقاً.
 * (مع 2x، الكتلة الجديدة دائماً أكبر من مجموع الكتل السابقة)
 */
#define SAD_ARRAY_GROWTH_NUMERATOR   3
#define SAD_ARRAY_GROWTH_DENOMINATOR 2

/** @brief (AR) أقصى عدد عناصر (حماية من الفيضان) */
#define SAD_ARRAY_MAX_LENGTH (1024 * 1024 * 64) /* 64M عنصر */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — أنواع العناصر
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) أنواع القيم المدعومة في المصفوفة
 *
 * تحدد نوع العنصر المخزّن. المصفوفة يمكن أن تكون:
 *  • مُحددة النوع (مثل: مصفوفة أعداد صحيحة فقط)
 *  • مصفوفة عامة (SAD_ELEM_ANY) — تقبل أيّ نوع
 */
typedef enum {
    SAD_ELEM_NONE   = 0,     /**< غير محدد */
    SAD_ELEM_INT    = 1,     /**< عدد صحيح (int64_t) */
    SAD_ELEM_DOUBLE = 2,     /**< عدد عشري (double) */
    SAD_ELEM_BOOL   = 3,     /**< منطقي (bool) */
    SAD_ELEM_STRING = 4,     /**< سلسلة نصية (SadString) */
    SAD_ELEM_ARRAY  = 5,     /**< مصفوفة متداخلة */
    SAD_ELEM_MAP    = 6,     /**< خريطة */
    SAD_ELEM_OBJECT = 7,     /**< كائن */
    SAD_ELEM_ANY    = 0xFF   /**< أيّ نوع — مصفوفة مرنة */
} SadElemType;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — بنية القيمة الخفيفة
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * SadArrayValue هي بنية خفيفة الوزن (16 بايت) تُستخدم كعنصر مصفوفة.
 * الفرق عن SadValue الكاملة (التي ستُعرّف في value.h) أنها لا تحمل
 * بيانات وصفية إضافية.
 *
 * ┌────────────────┐
 * │ type (4 بايت)  │
 * │ padding (4)    │
 * │ data (8 بايت)  │ ← int64_t / double / مؤشر
 * └────────────────┘
 */
typedef struct SadArrayValue {
    SadElemType type;             /**< نوع القيمة */
    union {
        int64_t   as_int;         /**< عدد صحيح */
        double    as_double;      /**< عدد عشري */
        bool      as_bool;        /**< قيمة منطقية */
        void*     as_ptr;         /**< مؤشر لسلسلة/مصفوفة/خريطة/كائن */
    };
} SadArrayValue;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — بنية المصفوفة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) المصفوفة الديناميكية — بنية البيانات الأساسية
 *
 * مُدارة بالعدّ المرجعي عبر SadObjectHeader.
 * عند آخر sad_release، يتم:
 *  ١. تحرير كل العناصر التي هي مؤشرات (sad_release لكل منها)
 *  ٢. تحرير مصفوفة البيانات
 *  ٣. تحرير بنية SadArray نفسها
 */
typedef struct SadArray {
    SadArrayValue*  data;         /**< مؤشر لمصفوفة العناصر */
    size_t          length;       /**< عدد العناصر الحالي */
    size_t          capacity;     /**< السعة المُخصصة */
    SadElemType     elem_type;    /**< نوع العناصر (أو ANY) */
} SadArray;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — إنشاء وتدمير المصفوفة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إنشاء مصفوفة جديدة فارغة
 * @param elem_type نوع العناصر (SAD_ELEM_ANY للمصفوفة المرنة)
 * @return مؤشر للمصفوفة الجديدة، أو NULL عند فشل التخصيص
 *
 * تُنشئ مصفوفة بسعة ابتدائية SAD_ARRAY_INITIAL_CAPACITY.
 * مُدارة بالعدّ المرجعي (العدد يبدأ من 1).
 */
SadArray* sad_array_new(SadElemType elem_type);

/**
 * @brief (AR) إنشاء مصفوفة بسعة محددة مسبقاً
 * @param elem_type نوع العناصر
 * @param initial_capacity السعة الابتدائية
 * @return مؤشر للمصفوفة
 *
 * مفيد عندما نعرف العدد التقريبي للعناصر مسبقاً.
 * مثال: sad_array_with_capacity(SAD_ELEM_INT, 100) لمصفوفة 100 عدد.
 */
SadArray* sad_array_with_capacity(SadElemType elem_type, size_t initial_capacity);

/**
 * @brief (AR) تحرير المصفوفة وعناصرها
 * @param arr المصفوفة المراد تحريرها
 *
 * يُنقص العدّ المرجعي. إذا وصل لصفر:
 *  • يُحرّر كل العناصر المرجعية (سلاسل، مصفوفات، كائنات)
 *  • يُحرّر مصفوفة البيانات
 *  • يُحرّر البنية نفسها
 */
void sad_array_free(SadArray* arr);

/**
 * @brief (AR) نسخ المصفوفة (زيادة عدّ مرجعي فقط — ضحل)
 * @param arr المصفوفة المصدر
 * @return نفس المؤشر مع عدّ مرجعي أعلى
 */
SadArray* sad_array_retain(SadArray* arr);

/**
 * @brief (AR) نسخ عميق للمصفوفة
 * @param arr المصفوفة المصدر
 * @return مصفوفة جديدة مستقلة تماماً
 */
SadArray* sad_array_clone(const SadArray* arr);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — الوصول للعناصر
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) الحصول على عنصر بالفهرس
 * @param arr المصفوفة
 * @param index الفهرس (يبدأ من 0)
 * @return مؤشر للقيمة، أو NULL إذا الفهرس خارج النطاق
 */
SadArrayValue* sad_array_get(const SadArray* arr, size_t index);

/**
 * @brief (AR) تعيين قيمة عنصر موجود
 * @param arr المصفوفة
 * @param index الفهرس
 * @param value القيمة الجديدة
 * @return true إذا نجح التعيين
 */
bool sad_array_set(SadArray* arr, size_t index, SadArrayValue value);

/** @brief (AR) عدد العناصر الحالي */
size_t sad_array_length(const SadArray* arr);

/** @brief (AR) السعة المُخصصة */
size_t sad_array_capacity(const SadArray* arr);

/** @brief (AR) هل المصفوفة فارغة؟ */
bool sad_array_is_empty(const SadArray* arr);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — إضافة وحذف العناصر
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إضافة عنصر لنهاية المصفوفة
 * @param arr المصفوفة
 * @param value القيمة المراد إضافتها
 * @return true إذا نجحت الإضافة
 *
 * عملية O(1) مُستهلَكة — أحياناً O(n) عند النمو.
 * إذا كانت المصفوفة ممتلئة، تنمو بعامل 1.5x.
 */
bool sad_array_push(SadArray* arr, SadArrayValue value);

/**
 * @brief (AR) إزالة وإرجاع آخر عنصر
 * @param arr المصفوفة
 * @param out_value مؤشر لتخزين القيمة المُزالة
 * @return true إذا كانت المصفوفة غير فارغة
 */
bool sad_array_pop(SadArray* arr, SadArrayValue* out_value);

/**
 * @brief (AR) إدراج عنصر في موقع محدد
 * @param arr المصفوفة
 * @param index الفهرس (0 = البداية)
 * @param value القيمة
 * @return true إذا نجح الإدراج
 *
 * O(n) — العناصر بعد index تُزاح لليمين.
 */
bool sad_array_insert(SadArray* arr, size_t index, SadArrayValue value);

/**
 * @brief (AR) إزالة عنصر من موقع محدد
 * @param arr المصفوفة
 * @param index الفهرس
 * @param out_value (اختياري) مؤشر لتخزين القيمة المُزالة
 * @return true إذا نجحت الإزالة
 *
 * O(n) — العناصر بعد index تُزاح لليسار.
 */
bool sad_array_remove(SadArray* arr, size_t index, SadArrayValue* out_value);

/**
 * @brief (AR) مسح جميع العناصر (بدون تغيير السعة)
 * @param arr المصفوفة
 */
void sad_array_clear(SadArray* arr);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٨ — عمليات البحث والترتيب
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) البحث عن عنصر
 * @param arr المصفوفة
 * @param value القيمة المُبحث عنها
 * @return فهرس العنصر أو (size_t)-1 إذا لم يُوجد
 */
size_t sad_array_find(const SadArray* arr, SadArrayValue value);

/**
 * @brief (AR) هل المصفوفة تحتوي على عنصر؟
 * @param arr المصفوفة
 * @param value القيمة
 * @return true إذا وُجدت
 */
bool sad_array_contains(const SadArray* arr, SadArrayValue value);

/**
 * @brief (AR) ترتيب المصفوفة تصاعدياً
 * @param arr المصفوفة
 *
 * يستخدم خوارزمية ترتيب سريع (quicksort).
 * يعمل فقط مع الأنواع القابلة للمقارنة (INT, DOUBLE, STRING).
 */
void sad_array_sort(SadArray* arr);

/**
 * @brief (AR) عكس ترتيب المصفوفة
 * @param arr المصفوفة
 */
void sad_array_reverse(SadArray* arr);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٩ — عمليات وظيفية
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) نوع دالة الاستدعاء لكل عنصر */
typedef void (*SadArrayForEachFn)(SadArrayValue* value, size_t index, void* context);

/** @brief (AR) نوع دالة التصفية (ترجع true للعناصر المطلوبة) */
typedef bool (*SadArrayFilterFn)(const SadArrayValue* value, size_t index, void* context);

/** @brief (AR) نوع دالة التحويل */
typedef SadArrayValue (*SadArrayMapFn)(const SadArrayValue* value, size_t index, void* context);

/**
 * @brief (AR) تنفيذ دالة على كل عنصر
 * @param arr المصفوفة
 * @param fn الدالة
 * @param context بيانات سياق (يُمرّر للدالة)
 */
void sad_array_foreach(SadArray* arr, SadArrayForEachFn fn, void* context);

/**
 * @brief (AR) إنشاء مصفوفة جديدة بالعناصر المُرشّحة
 * @param arr المصفوفة المصدر
 * @param fn دالة التصفية
 * @param context بيانات سياق
 * @return مصفوفة جديدة
 */
SadArray* sad_array_filter(const SadArray* arr, SadArrayFilterFn fn, void* context);

/**
 * @brief (AR) تحويل كل عنصر وإنشاء مصفوفة جديدة
 * @param arr المصفوفة المصدر
 * @param fn دالة التحويل
 * @param context بيانات سياق
 * @return مصفوفة جديدة
 */
SadArray* sad_array_map(const SadArray* arr, SadArrayMapFn fn, void* context);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١٠ — أدوات مساعدة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إنشاء قيمة عدد صحيح
 */
static inline SadArrayValue sad_val_int(int64_t v) {
    SadArrayValue val;
    val.type = SAD_ELEM_INT;
    val.as_int = v;
    return val;
}

/** @brief (AR) إنشاء قيمة عشرية */
static inline SadArrayValue sad_val_double(double v) {
    SadArrayValue val;
    val.type = SAD_ELEM_DOUBLE;
    val.as_double = v;
    return val;
}

/** @brief (AR) إنشاء قيمة منطقية */
static inline SadArrayValue sad_val_bool(bool v) {
    SadArrayValue val;
    val.type = SAD_ELEM_BOOL;
    val.as_bool = v;
    return val;
}

/** @brief (AR) إنشاء قيمة مؤشر (سلسلة/مصفوفة/كائن) */
static inline SadArrayValue sad_val_ptr(void* p, SadElemType type) {
    SadArrayValue val;
    val.type = type;
    val.as_ptr = p;
    return val;
}

/** @brief (AR) مقارنة قيمتين */
bool sad_array_value_equals(const SadArrayValue* a, const SadArrayValue* b);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_ARRAY_H */
