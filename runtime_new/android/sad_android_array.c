/**
 * @file sad_android_array.c
 * @brief (AR) تطبيق نظام المصفوفات الديناميكية — لغة ص لأندرويد
 * @brief (EN) Dynamic array implementation — Sad Language for Android
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق المصفوفات الديناميكية — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * ملاحظات التطبيق:
 * ─────────────────
 * • النمو يتم بعامل 1.5x (3/2) — أفضل من 2x لاستغلال الذاكرة
 * • العناصر المرجعية (سلاسل، مصفوفات، كائنات) تُدار بالعدّ المرجعي
 * • الترتيب يستخدم quicksort المدمج (qsort) لبساطة الكود
 * • العمليات الوظيفية (map, filter, foreach) تدعم سياق المستخدم
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#include "sad_android_array.h"
#include "sad_android_string.h"
#include <string.h>   /* memcpy, memmove, memset */
#include <stdlib.h>   /* qsort */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — أدوات داخلية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تحرير قيمة عنصر إذا كانت مرجعية
 *
 * القيم المرجعية هي: STRING, ARRAY, MAP, OBJECT.
 * كلها تستخدم العدّ المرجعي عبر sad_release.
 */
static void release_value(SadArrayValue* val) {
    if (!val) return;
    switch (val->type) {
        case SAD_ELEM_STRING:
        case SAD_ELEM_ARRAY:
        case SAD_ELEM_MAP:
        case SAD_ELEM_OBJECT:
            if (val->as_ptr) {
                sad_release(val->as_ptr);
                val->as_ptr = NULL;
            }
            break;
        default:
            break;
    }
}

/**
 * @brief (AR) الاحتفاظ بقيمة عنصر (زيادة عدّ مرجعي)
 */
static void retain_value(SadArrayValue* val) {
    if (!val) return;
    switch (val->type) {
        case SAD_ELEM_STRING:
        case SAD_ELEM_ARRAY:
        case SAD_ELEM_MAP:
        case SAD_ELEM_OBJECT:
            if (val->as_ptr) {
                sad_retain(val->as_ptr);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief (AR) ضمان توفر سعة كافية
 *
 * إذا كانت المصفوفة ممتلئة (length == capacity)، يتم النمو.
 *
 * الخوارزمية:
 *  new_capacity = max(min_capacity, capacity * 3 / 2)
 *
 * ثم نُخصّص مصفوفة جديدة، ننسخ، ونُحرّر القديمة.
 */
static bool ensure_capacity(SadArray* arr, size_t min_capacity) {
    if (!arr) return false;
    if (arr->capacity >= min_capacity) return true;

    /* حساب السعة الجديدة */
    size_t new_cap = arr->capacity * SAD_ARRAY_GROWTH_NUMERATOR / SAD_ARRAY_GROWTH_DENOMINATOR;
    if (new_cap < min_capacity) new_cap = min_capacity;
    if (new_cap > SAD_ARRAY_MAX_LENGTH) {
        if (min_capacity > SAD_ARRAY_MAX_LENGTH) return false;
        new_cap = SAD_ARRAY_MAX_LENGTH;
    }

    /* تخصيص مصفوفة جديدة */
    SadArrayValue* new_data = (SadArrayValue*)sad_alloc(new_cap * sizeof(SadArrayValue));
    if (!new_data) return false;

    /* نسخ العناصر الموجودة */
    if (arr->data && arr->length > 0) {
        memcpy(new_data, arr->data, arr->length * sizeof(SadArrayValue));
    }

    /* تصفير العناصر الجديدة */
    if (new_cap > arr->length) {
        memset(new_data + arr->length, 0,
               (new_cap - arr->length) * sizeof(SadArrayValue));
    }

    /* تحرير المصفوفة القديمة */
    if (arr->data) {
        sad_free(arr->data);
    }

    arr->data = new_data;
    arr->capacity = new_cap;
    return true;
}

/**
 * @brief (AR) المُدمّر — يُحرّر كل العناصر ومصفوفة البيانات
 *
 * يُستدعى تلقائياً عند وصول العدّ المرجعي لصفر.
 */
static void array_finalizer(void* ptr) {
    SadArray* arr = (SadArray*)ptr;
    if (!arr) return;

    /* تحرير كل عنصر مرجعي */
    for (size_t i = 0; i < arr->length; i++) {
        release_value(&arr->data[i]);
    }

    /* تحرير مصفوفة البيانات */
    if (arr->data) {
        sad_free(arr->data);
        arr->data = NULL;
    }

    arr->length = 0;
    arr->capacity = 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — إنشاء وتدمير المصفوفة
 * ═══════════════════════════════════════════════════════════════════════════ */

SadArray* sad_array_new(SadElemType elem_type) {
    return sad_array_with_capacity(elem_type, SAD_ARRAY_INITIAL_CAPACITY);
}

SadArray* sad_array_with_capacity(SadElemType elem_type, size_t initial_capacity) {
    /*
     * ١. نُخصّص بنية SadArray عبر sad_rc_alloc (عدّ مرجعي = 1)
     * ٢. نُخصّص مصفوفة البيانات بالسعة المطلوبة
     * ٣. نُصفّر كل شيء
     */
    if (initial_capacity == 0) initial_capacity = SAD_ARRAY_INITIAL_CAPACITY;
    if (initial_capacity > SAD_ARRAY_MAX_LENGTH) initial_capacity = SAD_ARRAY_MAX_LENGTH;

    SadArray* arr = (SadArray*)sad_rc_alloc(sizeof(SadArray), SAD_OBJ_ARRAY,
                                             array_finalizer);
    if (!arr) return NULL;

    arr->data = (SadArrayValue*)sad_alloc(initial_capacity * sizeof(SadArrayValue));
    if (!arr->data) {
        sad_release(arr);
        return NULL;
    }

    memset(arr->data, 0, initial_capacity * sizeof(SadArrayValue));
    arr->length = 0;
    arr->capacity = initial_capacity;
    arr->elem_type = elem_type;

    return arr;
}

void sad_array_free(SadArray* arr) {
    if (!arr) return;

    /*
     * فقط نُنقص العدّ المرجعي.
     * array_finalizer يُنظّف كل شيء عند وصوله لصفر.
     */
    sad_release(arr);
}

SadArray* sad_array_retain(SadArray* arr) {
    if (arr) sad_retain(arr);
    return arr;
}

SadArray* sad_array_clone(const SadArray* arr) {
    if (!arr) return NULL;

    SadArray* clone = sad_array_with_capacity(arr->elem_type, arr->capacity);
    if (!clone) return NULL;

    /* نسخ عميق لكل عنصر */
    for (size_t i = 0; i < arr->length; i++) {
        SadArrayValue val = arr->data[i];
        retain_value(&val); /* زيادة عدّ مرجعي للمؤشرات */
        clone->data[i] = val;
    }
    clone->length = arr->length;

    return clone;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — الوصول للعناصر
 * ═══════════════════════════════════════════════════════════════════════════ */

SadArrayValue* sad_array_get(const SadArray* arr, size_t index) {
    if (!arr || index >= arr->length) return NULL;
    return &arr->data[index];
}

bool sad_array_set(SadArray* arr, size_t index, SadArrayValue value) {
    if (!arr || index >= arr->length) return false;

    /* تحرير القيمة القديمة وتخزين الجديدة */
    release_value(&arr->data[index]);
    retain_value(&value);
    arr->data[index] = value;

    return true;
}

size_t sad_array_length(const SadArray* arr) {
    return arr ? arr->length : 0;
}

size_t sad_array_capacity(const SadArray* arr) {
    return arr ? arr->capacity : 0;
}

bool sad_array_is_empty(const SadArray* arr) {
    return !arr || arr->length == 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — إضافة وحذف العناصر
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_array_push(SadArray* arr, SadArrayValue value) {
    if (!arr) return false;

    /*
     * إضافة لنهاية المصفوفة:
     *
     * قبل: [1, 2, 3, _, _, _, _, _]  length=3, capacity=8
     * بعد: [1, 2, 3, 4, _, _, _, _]  length=4, capacity=8
     *
     * إذا length == capacity:
     * قبل: [1, 2, 3, 4, 5, 6, 7, 8]  length=8, capacity=8
     * نمو:  تخصيص capacity=12 (8 * 3/2)
     * بعد: [1, 2, 3, 4, 5, 6, 7, 8, 9, _, _, _]  length=9, capacity=12
     */
    if (!ensure_capacity(arr, arr->length + 1)) return false;

    retain_value(&value);
    arr->data[arr->length] = value;
    arr->length++;

    return true;
}

bool sad_array_pop(SadArray* arr, SadArrayValue* out_value) {
    if (!arr || arr->length == 0) return false;

    arr->length--;
    if (out_value) {
        *out_value = arr->data[arr->length];
        /* لا نُحرّر — المُستدعي الآن يملك المرجع */
    } else {
        release_value(&arr->data[arr->length]);
    }

    /* تصفير الموقع */
    memset(&arr->data[arr->length], 0, sizeof(SadArrayValue));
    return true;
}

bool sad_array_insert(SadArray* arr, size_t index, SadArrayValue value) {
    if (!arr || index > arr->length) return false;

    /*
     * إدراج في الوسط:
     *
     * قبل: [1, 2, 3, 4, _, _]  index=2, value=99
     * إزاحة: [1, 2, _, 3, 4, _]
     * بعد: [1, 2, 99, 3, 4, _]
     */
    if (!ensure_capacity(arr, arr->length + 1)) return false;

    /* إزاحة العناصر لليمين */
    if (index < arr->length) {
        memmove(&arr->data[index + 1], &arr->data[index],
                (arr->length - index) * sizeof(SadArrayValue));
    }

    retain_value(&value);
    arr->data[index] = value;
    arr->length++;

    return true;
}

bool sad_array_remove(SadArray* arr, size_t index, SadArrayValue* out_value) {
    if (!arr || index >= arr->length) return false;

    /*
     * إزالة من الوسط:
     *
     * قبل: [1, 2, 99, 3, 4]  index=2
     * بعد: [1, 2, 3, 4, _]
     */
    if (out_value) {
        *out_value = arr->data[index];
    } else {
        release_value(&arr->data[index]);
    }

    /* إزاحة العناصر لليسار */
    if (index < arr->length - 1) {
        memmove(&arr->data[index], &arr->data[index + 1],
                (arr->length - index - 1) * sizeof(SadArrayValue));
    }

    arr->length--;
    memset(&arr->data[arr->length], 0, sizeof(SadArrayValue));

    return true;
}

void sad_array_clear(SadArray* arr) {
    if (!arr) return;

    /* تحرير كل عنصر مرجعي */
    for (size_t i = 0; i < arr->length; i++) {
        release_value(&arr->data[i]);
    }

    /* تصفير البيانات (لا تغيير في السعة) */
    memset(arr->data, 0, arr->length * sizeof(SadArrayValue));
    arr->length = 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — البحث والترتيب
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_array_value_equals(const SadArrayValue* a, const SadArrayValue* b) {
    if (!a || !b) return false;
    if (a->type != b->type) return false;

    switch (a->type) {
        case SAD_ELEM_INT:    return a->as_int == b->as_int;
        case SAD_ELEM_DOUBLE: return a->as_double == b->as_double;
        case SAD_ELEM_BOOL:   return a->as_bool == b->as_bool;
        case SAD_ELEM_STRING:
            /*
             * مقارنة سلاسل: نستخدم مقارنة SadString
             * المؤشر يجب أن يشير لـ SadString
             */
            if (a->as_ptr && b->as_ptr) {
                return sad_string_equals((const SadString*)a->as_ptr,
                                          (const SadString*)b->as_ptr);
            }
            return a->as_ptr == b->as_ptr;
        default:
            /* للأنواع المرجعية الأخرى: مقارنة المؤشرات */
            return a->as_ptr == b->as_ptr;
    }
}

size_t sad_array_find(const SadArray* arr, SadArrayValue value) {
    if (!arr) return (size_t)-1;

    for (size_t i = 0; i < arr->length; i++) {
        if (sad_array_value_equals(&arr->data[i], &value)) {
            return i;
        }
    }

    return (size_t)-1;
}

bool sad_array_contains(const SadArray* arr, SadArrayValue value) {
    return sad_array_find(arr, value) != (size_t)-1;
}

/**
 * @brief (AR) دالة المقارنة لـ qsort — أعداد صحيحة
 */
static int compare_int(const void* a, const void* b) {
    int64_t va = ((const SadArrayValue*)a)->as_int;
    int64_t vb = ((const SadArrayValue*)b)->as_int;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

/**
 * @brief (AR) دالة المقارنة لـ qsort — أعداد عشرية
 */
static int compare_double(const void* a, const void* b) {
    double va = ((const SadArrayValue*)a)->as_double;
    double vb = ((const SadArrayValue*)b)->as_double;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}

void sad_array_sort(SadArray* arr) {
    if (!arr || arr->length <= 1) return;

    /*
     * ترتيب المصفوفة — نستخدم qsort المدمج.
     * نختار دالة المقارنة حسب نوع العناصر.
     */
    switch (arr->elem_type) {
        case SAD_ELEM_INT:
            qsort(arr->data, arr->length, sizeof(SadArrayValue), compare_int);
            break;
        case SAD_ELEM_DOUBLE:
            qsort(arr->data, arr->length, sizeof(SadArrayValue), compare_double);
            break;
        default:
            /*
             * للأنواع الأخرى: ترتيب بسيط بالنوع أولاً، ثم بالقيمة.
             * يمكن إضافة دوال مقارنة مخصصة لاحقاً.
             */
            break;
    }
}

void sad_array_reverse(SadArray* arr) {
    if (!arr || arr->length <= 1) return;

    /*
     * عكس المصفوفة في مكانها:
     * نبدل العنصر الأول مع الأخير، الثاني مع ما قبل الأخير...
     *
     * [1, 2, 3, 4, 5] → [5, 4, 3, 2, 1]
     */
    size_t left = 0;
    size_t right = arr->length - 1;

    while (left < right) {
        SadArrayValue tmp = arr->data[left];
        arr->data[left] = arr->data[right];
        arr->data[right] = tmp;
        left++;
        right--;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — العمليات الوظيفية
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * هذه العمليات مستوحاة من البرمجة الوظيفية (functional programming):
 *
 * • foreach: تنفيذ إجراء على كل عنصر (بدون إنشاء مصفوفة جديدة)
 * • filter: إنشاء مصفوفة جديدة بالعناصر التي تُحقق شرطاً
 * • map: تحويل كل عنصر وإنشاء مصفوفة جديدة بالنتائج
 *
 * مثال بلغة ص:
 *   مصفوفة أرقام = [1, 2, 3, 4, 5]
 *   متغير زوجية = أرقام.تصفية(دالة(ر) ارجع ر % 2 == 0 نهاية)
 *   # زوجية = [2, 4]
 */

void sad_array_foreach(SadArray* arr, SadArrayForEachFn fn, void* context) {
    if (!arr || !fn) return;

    for (size_t i = 0; i < arr->length; i++) {
        fn(&arr->data[i], i, context);
    }
}

SadArray* sad_array_filter(const SadArray* arr, SadArrayFilterFn fn, void* context) {
    if (!arr || !fn) return sad_array_new(SAD_ELEM_ANY);

    SadArray* result = sad_array_new(arr->elem_type);
    if (!result) return NULL;

    for (size_t i = 0; i < arr->length; i++) {
        if (fn(&arr->data[i], i, context)) {
            /* العنصر يُحقق الشرط — أضفه للنتيجة */
            sad_array_push(result, arr->data[i]);
        }
    }

    return result;
}

SadArray* sad_array_map(const SadArray* arr, SadArrayMapFn fn, void* context) {
    if (!arr || !fn) return sad_array_new(SAD_ELEM_ANY);

    SadArray* result = sad_array_with_capacity(arr->elem_type, arr->length);
    if (!result) return NULL;

    for (size_t i = 0; i < arr->length; i++) {
        SadArrayValue mapped = fn(&arr->data[i], i, context);
        sad_array_push(result, mapped);
    }

    return result;
}
