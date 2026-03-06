/**
 * @file sad_android_value.h
 * @brief (AR) نظام القيم الموحّد لرانتايم أندرويد — لغة ص
 * @brief (EN) Unified value system for Android runtime — Sad Language
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام القيم الموحّد (SadValue) — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * SadValue هو النوع الأساسي الذي يُمثّل أيّ قيمة في لغة ص أثناء التشغيل.
 * كل متغيّر، كل نتيجة عملية، كل وسيط دالة — كلها SadValue.
 *
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║                    تصميم SadValue                              ║
 * ╠══════════════════════════════════════════════════════════════════╣
 * ║                                                                ║
 * ║  الحجم: 32 بايت (لتتماشى مع SadString)                       ║
 * ║                                                                ║
 * ║  ┌──────────────────────────────────────────────────────┐      ║
 * ║  │ type:  SadValueType (4 بايت)                        │      ║
 * ║  │ flags: uint32_t    (4 بايت — const, ref, etc.)      │      ║
 * ║  │ data:  union       (24 بايت)                        │      ║
 * ║  │   ├─ as_int:    int64_t                             │      ║
 * ║  │   ├─ as_double: double                              │      ║
 * ║  │   ├─ as_bool:   bool                                │      ║
 * ║  │   ├─ as_string: SadString  (32 بايت — مضمّنة!)     │      ║
 * ║  │   ├─ as_array:  SadArray*  (مؤشر)                   │      ║
 * ║  │   ├─ as_map:    SadMap*    (مؤشر)                   │      ║
 * ║  │   ├─ as_object: void*      (كائن)                   │      ║
 * ║  │   ├─ as_func:   SadFunc*   (دالة/إغلاق)            │      ║
 * ║  │   └─ as_ptr:    void*      (عام)                    │      ║
 * ║  └──────────────────────────────────────────────────────┘      ║
 * ║                                                                ║
 * ║  ملاحظة: بما أن SadString = 32 بايت، والقيمة تحتاج 8 بايت    ║
 * ║  إضافية (type+flags)، نستخدم 40 بايت إجمالي للـ SadValue      ║
 * ║  عند احتواء سلسلة. هذا مقبول لأن معظم القيم أصغر.            ║
 * ║                                                                ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#ifndef SAD_ANDROID_VALUE_H
#define SAD_ANDROID_VALUE_H

#include "sad_android_memory.h"
#include "sad_android_string.h"
#include "sad_android_array.h"
#include "sad_android_map.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — أنواع القيم
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) أنواع القيم المدعومة في لغة ص
 *
 * كل نوع يُطابق نوعاً في لغة ص:
 *  - رقم    → INT
 *  - عشري   → DOUBLE
 *  - منطقي  → BOOL
 *  - نص     → STRING
 *  - مصفوفة → ARRAY
 *  - خريطة  → MAP
 *  - دالة   → FUNCTION
 *  - عدم    → VOID (لا قيمة)
 *  - فارغ   → NULL_VAL
 */
typedef enum {
    SAD_TYPE_VOID      = 0,   /**< عدم — لا قيمة (ارجع بدون قيمة) */
    SAD_TYPE_NULL      = 1,   /**< فارغ — قيمة فارغة صريحة */
    SAD_TYPE_INT       = 2,   /**< رقم — عدد صحيح 64-bit */
    SAD_TYPE_DOUBLE    = 3,   /**< عشري — عدد عشري مزدوج الدقة */
    SAD_TYPE_BOOL      = 4,   /**< منطقي — صحيح أو خطأ */
    SAD_TYPE_STRING    = 5,   /**< نص — سلسلة UTF-8 */
    SAD_TYPE_ARRAY     = 6,   /**< مصفوفة — مصفوفة ديناميكية */
    SAD_TYPE_MAP       = 7,   /**< خريطة — جدول تجزئة */
    SAD_TYPE_OBJECT    = 8,   /**< كائن — نسخة من صنف */
    SAD_TYPE_FUNCTION  = 9,   /**< دالة — دالة أو إغلاق (closure) */
    SAD_TYPE_ERROR     = 10,  /**< خطأ — كائن خطأ */
} SadValueType;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — أعلام القيمة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) أعلام تصف خصائص القيمة
 */
#define SAD_VAL_FLAG_NONE      0x00   /**< بدون أعلام */
#define SAD_VAL_FLAG_CONST     0x01   /**< قيمة ثابتة (لا تقبل التعديل) */
#define SAD_VAL_FLAG_REF       0x02   /**< مرجع لقيمة أخرى */
#define SAD_VAL_FLAG_OWNED     0x04   /**< القيمة مملوكة (يجب تحريرها) */
#define SAD_VAL_FLAG_BORROWED  0x08   /**< قيمة مُستعارة (لا تُحررها) */
#define SAD_VAL_FLAG_OPTIONAL  0x10   /**< قيمة اختيارية (قد تكون فارغة) */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — بنية الدالة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) بنية الدالة/الإغلاق (Closure)
 *
 * تُمثّل دالة في لغة ص:
 *  • func_ptr: مؤشر للكود المُترجم
 *  • closure_data: البيئة المُلتقطة (للإغلاقات)
 *  • param_count: عدد الوسائط
 *  • name: اسم الدالة (للتتبع)
 */
typedef struct SadFunc {
    void*       func_ptr;         /**< مؤشر للكود المترجم (JIT أو AOT) */
    void*       closure_data;     /**< بيانات الإغلاق (المتغيرات المُلتقطة) */
    uint16_t    param_count;      /**< عدد الوسائط */
    uint16_t    flags;            /**< أعلام إضافية */
    SadString   name;             /**< اسم الدالة */
} SadFunc;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — بنية الكائن
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) بنية الكائن — نسخة من صنف
 *
 * تُمثّل نسخة من صنف (class instance) في لغة ص.
 * الحقول مُخزّنة في خريطة (اسم الحقل → قيمة).
 *
 * مثال:
 *   صنف شخص
 *     متغير الاسم = ""
 *     متغير العمر = 0
 *   نهاية
 *
 *   يُنشئ SadObject مع:
 *   - class_name = "شخص"
 *   - fields = {"الاسم": "", "العمر": 0}
 */
typedef struct SadObject {
    SadString   class_name;       /**< اسم الصنف */
    SadMap*     fields;           /**< حقول الكائن */
    SadMap*     methods;          /**< دوال الكائن (مؤشرات SadFunc) */
    void*       native_data;      /**< بيانات أصلية (لربط مع أندرويد) */
} SadObject;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — بنية القيمة الموحّدة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) القيمة الموحّدة — النوع الأساسي لوقت التشغيل
 *
 * هذه هي البنية التي يتعامل معها كل كود لغة ص.
 * كل متغيّر في لغة ص هو SadValue.
 */
typedef struct SadValue {
    SadValueType type;            /**< نوع القيمة */
    uint32_t     flags;           /**< أعلام (const, ref, etc.) */

    union {
        int64_t     as_int;       /**< عدد صحيح */
        double      as_double;    /**< عدد عشري */
        bool        as_bool;      /**< منطقي */
        SadString   as_string;    /**< سلسلة نصية (مُضمّنة — 32 بايت) */
        SadArray*   as_array;     /**< مصفوفة (مؤشر) */
        SadMap*     as_map;       /**< خريطة (مؤشر) */
        SadObject*  as_object;    /**< كائن (مؤشر) */
        SadFunc*    as_func;      /**< دالة (مؤشر) */
        void*       as_ptr;       /**< مؤشر عام */
    };
} SadValue;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — إنشاء القيم
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) إنشاء قيمة فارغة (عدم) */
SadValue sad_value_void(void);

/** @brief (AR) إنشاء قيمة null */
SadValue sad_value_null(void);

/** @brief (AR) إنشاء قيمة عدد صحيح */
SadValue sad_value_int(int64_t value);

/** @brief (AR) إنشاء قيمة عدد عشري */
SadValue sad_value_double(double value);

/** @brief (AR) إنشاء قيمة منطقية */
SadValue sad_value_bool(bool value);

/**
 * @brief (AR) إنشاء قيمة نصية من سلسلة C
 * @param text النص (UTF-8)
 * @return SadValue من النوع STRING
 */
SadValue sad_value_string(const char* text);

/**
 * @brief (AR) إنشاء قيمة نصية من SadString
 */
SadValue sad_value_from_string(SadString str);

/** @brief (AR) إنشاء قيمة مصفوفة */
SadValue sad_value_array(SadArray* arr);

/** @brief (AR) إنشاء قيمة خريطة */
SadValue sad_value_map(SadMap* map);

/** @brief (AR) إنشاء قيمة كائن */
SadValue sad_value_object(SadObject* obj);

/** @brief (AR) إنشاء قيمة دالة */
SadValue sad_value_function(SadFunc* func);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — فحص النوع
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) هل هي void؟ */
static inline bool sad_value_is_void(const SadValue* v) {
    return v && v->type == SAD_TYPE_VOID;
}

/** @brief (AR) هل هي null؟ */
static inline bool sad_value_is_null(const SadValue* v) {
    return v && v->type == SAD_TYPE_NULL;
}

/** @brief (AR) هل هي عدد صحيح؟ */
static inline bool sad_value_is_int(const SadValue* v) {
    return v && v->type == SAD_TYPE_INT;
}

/** @brief (AR) هل هي عدد عشري؟ */
static inline bool sad_value_is_double(const SadValue* v) {
    return v && v->type == SAD_TYPE_DOUBLE;
}

/** @brief (AR) هل هي رقم (صحيح أو عشري)؟ */
static inline bool sad_value_is_number(const SadValue* v) {
    return v && (v->type == SAD_TYPE_INT || v->type == SAD_TYPE_DOUBLE);
}

/** @brief (AR) هل هي منطقية؟ */
static inline bool sad_value_is_bool(const SadValue* v) {
    return v && v->type == SAD_TYPE_BOOL;
}

/** @brief (AR) هل هي نص؟ */
static inline bool sad_value_is_string(const SadValue* v) {
    return v && v->type == SAD_TYPE_STRING;
}

/** @brief (AR) هل هي مصفوفة؟ */
static inline bool sad_value_is_array(const SadValue* v) {
    return v && v->type == SAD_TYPE_ARRAY;
}

/** @brief (AR) هل هي خريطة؟ */
static inline bool sad_value_is_map(const SadValue* v) {
    return v && v->type == SAD_TYPE_MAP;
}

/** @brief (AR) هل هي كائن؟ */
static inline bool sad_value_is_object(const SadValue* v) {
    return v && v->type == SAD_TYPE_OBJECT;
}

/** @brief (AR) هل هي دالة؟ */
static inline bool sad_value_is_function(const SadValue* v) {
    return v && v->type == SAD_TYPE_FUNCTION;
}

/** @brief (AR) هل هي قيمة "حقيقية" (truthy)? */
bool sad_value_is_truthy(const SadValue* v);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٨ — عمليات على القيم
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تحويل القيمة لنص
 * @param v القيمة
 * @return SadString تمثّل القيمة كنص
 *
 * أمثلة:
 *  - int(42) → "42"
 *  - bool(true) → "صحيح"
 *  - null → "فارغ"
 *  - array → "[1, 2, 3]"
 */
SadString sad_value_to_string(const SadValue* v);

/**
 * @brief (AR) تحويل القيمة لعدد عشري
 * @return العدد العشري، أو 0.0 إذا فشل التحويل
 */
double sad_value_to_double(const SadValue* v);

/**
 * @brief (AR) تحويل القيمة لعدد صحيح
 */
int64_t sad_value_to_int(const SadValue* v);

/** @brief (AR) مقارنة قيمتين للتساوي */
bool sad_value_equals(const SadValue* a, const SadValue* b);

/** @brief (AR) مقارنة ترتيبية */
int sad_value_compare(const SadValue* a, const SadValue* b);

/** @brief (AR) حساب تجزئة القيمة */
uint64_t sad_value_hash(const SadValue* v);

/** @brief (AR) نسخ القيمة (ضحل — يزيد عدّ مرجعي) */
SadValue sad_value_copy(const SadValue* v);

/** @brief (AR) نسخ عميق للقيمة */
SadValue sad_value_clone(const SadValue* v);

/** @brief (AR) تحرير القيمة */
void sad_value_free(SadValue* v);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٩ — العمليات الحسابية
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) جمع: أ + ب */
SadValue sad_value_add(const SadValue* a, const SadValue* b);

/** @brief (AR) طرح: أ - ب */
SadValue sad_value_sub(const SadValue* a, const SadValue* b);

/** @brief (AR) ضرب: أ * ب */
SadValue sad_value_mul(const SadValue* a, const SadValue* b);

/** @brief (AR) قسمة: أ / ب */
SadValue sad_value_div(const SadValue* a, const SadValue* b);

/** @brief (AR) باقي القسمة: أ % ب */
SadValue sad_value_mod(const SadValue* a, const SadValue* b);

/** @brief (AR) الأس: أ ** ب */
SadValue sad_value_pow(const SadValue* a, const SadValue* b);

/** @brief (AR) السالب: -أ */
SadValue sad_value_neg(const SadValue* a);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١٠ — اسم النوع بالعربية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) الحصول على اسم النوع بالعربية
 * @param type نوع القيمة
 * @return اسم النوع بالعربية (مثال: "رقم"، "نص"، "مصفوفة")
 */
const char* sad_value_type_name(SadValueType type);

/**
 * @brief (AR) الحصول على اسم النوع بالإنجليزية
 */
const char* sad_value_type_name_en(SadValueType type);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١١ — إنشاء الكائنات
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إنشاء كائن جديد (نسخة من صنف)
 * @param class_name اسم الصنف
 * @return مؤشر للكائن، أو NULL عند الفشل
 */
SadObject* sad_object_new(const char* class_name);

/**
 * @brief (AR) تحرير الكائن
 */
void sad_object_free(SadObject* obj);

/**
 * @brief (AR) تعيين حقل في الكائن
 */
bool sad_object_set_field(SadObject* obj, const char* name, SadValue value);

/**
 * @brief (AR) الحصول على حقل من الكائن
 */
SadValue* sad_object_get_field(SadObject* obj, const char* name);

/**
 * @brief (AR) إنشاء دالة جديدة
 */
SadFunc* sad_func_new(const char* name, void* func_ptr, uint16_t param_count);

/**
 * @brief (AR) تحرير الدالة
 */
void sad_func_free(SadFunc* func);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_VALUE_H */
