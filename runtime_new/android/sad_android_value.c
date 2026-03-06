/**
 * @file sad_android_value.c
 * @brief (AR) تطبيق نظام القيم الموحّد — لغة ص لأندرويد
 * @brief (EN) Unified value system implementation — Sad Language Android
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق القيم الموحّدة — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يحتوي التطبيق الكامل لنظام القيم الذي يُشكّل
 * قلب بيئة التشغيل. كل عملية في لغة ص تمر عبر هذا النظام.
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#include "sad_android_value.h"
#include <string.h>  /* memset, memcpy */
#include <stdio.h>   /* snprintf */
#include <math.h>    /* pow, fmod */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — إنشاء القيم
 * ═══════════════════════════════════════════════════════════════════════════ */

SadValue sad_value_void(void) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_VOID;
    return v;
}

SadValue sad_value_null(void) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_NULL;
    return v;
}

SadValue sad_value_int(int64_t value) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_INT;
    v.as_int = value;
    return v;
}

SadValue sad_value_double(double value) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_DOUBLE;
    v.as_double = value;
    return v;
}

SadValue sad_value_bool(bool value) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_BOOL;
    v.as_bool = value;
    return v;
}

SadValue sad_value_string(const char* text) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_STRING;
    v.as_string = sad_string_from_cstr(text);
    v.flags = SAD_VAL_FLAG_OWNED;
    return v;
}

SadValue sad_value_from_string(SadString str) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_STRING;
    v.as_string = str;
    v.flags = SAD_VAL_FLAG_OWNED;
    return v;
}

SadValue sad_value_array(SadArray* arr) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_ARRAY;
    v.as_array = arr;
    if (arr) {
        sad_retain(arr);
        v.flags = SAD_VAL_FLAG_OWNED;
    }
    return v;
}

SadValue sad_value_map(SadMap* map) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_MAP;
    v.as_map = map;
    if (map) {
        sad_retain(map);
        v.flags = SAD_VAL_FLAG_OWNED;
    }
    return v;
}

SadValue sad_value_object(SadObject* obj) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_OBJECT;
    v.as_object = obj;
    if (obj) {
        sad_retain(obj);
        v.flags = SAD_VAL_FLAG_OWNED;
    }
    return v;
}

SadValue sad_value_function(SadFunc* func) {
    SadValue v;
    memset(&v, 0, sizeof(SadValue));
    v.type = SAD_TYPE_FUNCTION;
    v.as_func = func;
    if (func) {
        sad_retain(func);
        v.flags = SAD_VAL_FLAG_OWNED;
    }
    return v;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — فحص الحقيقة (Truthiness)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * قواعد "الحقيقة" في لغة ص:
 * ─────────────────────────
 * | النوع     | حقيقي (صحيح)                | زائف (خطأ)           |
 * |───────────|─────────────────────────────|──────────────────────|
 * | عدم/فارغ  | أبداً                        | دائماً               |
 * | منطقي    | صحيح                         | خطأ                  |
 * | صحيح     | ≠ 0                          | == 0                 |
 * | عشري     | ≠ 0.0                        | == 0.0               |
 * | نص       | غير فارغ                     | فارغ ("")             |
 * | مصفوفة   | غير فارغة                    | فارغة ([])            |
 * | خريطة    | غير فارغة                    | فارغة ({})            |
 * | كائن     | دائماً                       | أبداً                |
 * | دالة     | دائماً                       | أبداً                |
 */
bool sad_value_is_truthy(const SadValue* v) {
    if (!v) return false;

    switch (v->type) {
        case SAD_TYPE_VOID:
        case SAD_TYPE_NULL:
            return false;
        case SAD_TYPE_BOOL:
            return v->as_bool;
        case SAD_TYPE_INT:
            return v->as_int != 0;
        case SAD_TYPE_DOUBLE:
            return v->as_double != 0.0;
        case SAD_TYPE_STRING:
            return sad_string_length(&v->as_string) > 0;
        case SAD_TYPE_ARRAY:
            return v->as_array && sad_array_length(v->as_array) > 0;
        case SAD_TYPE_MAP:
            return v->as_map && sad_map_count(v->as_map) > 0;
        case SAD_TYPE_OBJECT:
        case SAD_TYPE_FUNCTION:
            return true; /* كائن أو دالة = دائماً صحيح */
        default:
            return false;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — تحويل القيم
 * ═══════════════════════════════════════════════════════════════════════════ */

SadString sad_value_to_string(const SadValue* v) {
    if (!v) return sad_string_from_cstr("فارغ");

    switch (v->type) {
        case SAD_TYPE_VOID:
            return sad_string_from_cstr("عدم");
        case SAD_TYPE_NULL:
            return sad_string_from_cstr("فارغ");
        case SAD_TYPE_INT:
            return sad_string_from_int(v->as_int);
        case SAD_TYPE_DOUBLE:
            return sad_string_from_double(v->as_double);
        case SAD_TYPE_BOOL:
            return sad_string_from_bool(v->as_bool);
        case SAD_TYPE_STRING:
            return sad_string_copy(&v->as_string);
        case SAD_TYPE_ARRAY: {
            /*
             * مصفوفة → "[عنصر1, عنصر2, ...]"
             * نبني النص تدريجياً
             */
            SadString result = sad_string_from_cstr("[");
            if (v->as_array) {
                for (size_t i = 0; i < sad_array_length(v->as_array); i++) {
                    if (i > 0) {
                        sad_string_append(&result, ", ", 2);
                    }
                    SadArrayValue* elem = sad_array_get(v->as_array, i);
                    if (elem) {
                        /* تحويل العنصر لـ SadValue ثم لنص */
                        char buf[64];
                        switch (elem->type) {
                            case SAD_ELEM_INT:
                                snprintf(buf, sizeof(buf), "%lld", (long long)elem->as_int);
                                sad_string_append(&result, buf, strlen(buf));
                                break;
                            case SAD_ELEM_DOUBLE:
                                snprintf(buf, sizeof(buf), "%g", elem->as_double);
                                sad_string_append(&result, buf, strlen(buf));
                                break;
                            case SAD_ELEM_BOOL:
                                if (elem->as_bool) {
                                    sad_string_append(&result, "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", 8);
                                } else {
                                    sad_string_append(&result, "\xd8\xae\xd8\xb7\xd8\xa3", 6);
                                }
                                break;
                            default:
                                sad_string_append(&result, "...", 3);
                                break;
                        }
                    }
                }
            }
            sad_string_append(&result, "]", 1);
            return result;
        }
        case SAD_TYPE_MAP:
            return sad_string_format("{خريطة: %zu}", v->as_map ? sad_map_count(v->as_map) : 0);
        case SAD_TYPE_OBJECT: {
            if (v->as_object) {
                return sad_string_format("<كائن %s>", sad_string_data(&v->as_object->class_name));
            }
            return sad_string_from_cstr("<كائن فارغ>");
        }
        case SAD_TYPE_FUNCTION: {
            if (v->as_func) {
                return sad_string_format("<دالة %s>", sad_string_data(&v->as_func->name));
            }
            return sad_string_from_cstr("<دالة>");
        }
        default:
            return sad_string_from_cstr("غير معروف");
    }
}

double sad_value_to_double(const SadValue* v) {
    if (!v) return 0.0;
    switch (v->type) {
        case SAD_TYPE_INT:    return (double)v->as_int;
        case SAD_TYPE_DOUBLE: return v->as_double;
        case SAD_TYPE_BOOL:   return v->as_bool ? 1.0 : 0.0;
        case SAD_TYPE_STRING: {
            double d = 0.0;
            sad_string_to_double(&v->as_string, &d);
            return d;
        }
        default: return 0.0;
    }
}

int64_t sad_value_to_int(const SadValue* v) {
    if (!v) return 0;
    switch (v->type) {
        case SAD_TYPE_INT:    return v->as_int;
        case SAD_TYPE_DOUBLE: return (int64_t)v->as_double;
        case SAD_TYPE_BOOL:   return v->as_bool ? 1 : 0;
        case SAD_TYPE_STRING: {
            int64_t i = 0;
            sad_string_to_int(&v->as_string, &i);
            return i;
        }
        default: return 0;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — المقارنة والتجزئة
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_value_equals(const SadValue* a, const SadValue* b) {
    if (!a || !b) return (a == b);
    if (a->type != b->type) {
        /* مقارنة أعداد مختلطة: int == double */
        if (sad_value_is_number(a) && sad_value_is_number(b)) {
            return sad_value_to_double(a) == sad_value_to_double(b);
        }
        return false;
    }

    switch (a->type) {
        case SAD_TYPE_VOID:
        case SAD_TYPE_NULL:
            return true; /* null == null, void == void */
        case SAD_TYPE_INT:
            return a->as_int == b->as_int;
        case SAD_TYPE_DOUBLE:
            return a->as_double == b->as_double;
        case SAD_TYPE_BOOL:
            return a->as_bool == b->as_bool;
        case SAD_TYPE_STRING:
            return sad_string_equals(&a->as_string, &b->as_string);
        case SAD_TYPE_ARRAY:
            return a->as_array == b->as_array; /* مقارنة هوية */
        case SAD_TYPE_MAP:
            return a->as_map == b->as_map;
        case SAD_TYPE_OBJECT:
            return a->as_object == b->as_object;
        case SAD_TYPE_FUNCTION:
            return a->as_func == b->as_func;
        default:
            return false;
    }
}

int sad_value_compare(const SadValue* a, const SadValue* b) {
    if (!a || !b) return 0;

    /* مقارنة أرقام */
    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        double da = sad_value_to_double(a);
        double db = sad_value_to_double(b);
        if (da < db) return -1;
        if (da > db) return 1;
        return 0;
    }

    /* مقارنة نصوص */
    if (a->type == SAD_TYPE_STRING && b->type == SAD_TYPE_STRING) {
        return sad_string_compare(&a->as_string, &b->as_string);
    }

    /* مقارنة منطقية */
    if (a->type == SAD_TYPE_BOOL && b->type == SAD_TYPE_BOOL) {
        return (int)a->as_bool - (int)b->as_bool;
    }

    /* مقارنة أنواع مختلفة: بترتيب النوع */
    return (int)a->type - (int)b->type;
}

uint64_t sad_value_hash(const SadValue* v) {
    if (!v) return 0;

    switch (v->type) {
        case SAD_TYPE_VOID:
        case SAD_TYPE_NULL:
            return 0;
        case SAD_TYPE_INT: {
            /* تجزئة عدد صحيح — mixing hash */
            uint64_t h = (uint64_t)v->as_int;
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccdULL;
            h ^= h >> 33;
            h *= 0xc4ceb9fe1a85ec53ULL;
            h ^= h >> 33;
            return h;
        }
        case SAD_TYPE_DOUBLE: {
            /* تجزئة عدد عشري — نستخدم بتات التمثيل */
            uint64_t bits;
            memcpy(&bits, &v->as_double, sizeof(bits));
            return bits * 2654435761ULL;
        }
        case SAD_TYPE_BOOL:
            return v->as_bool ? 1 : 0;
        case SAD_TYPE_STRING:
            return sad_string_hash(&v->as_string);
        default:
            /* للمؤشرات: تجزئة العنوان */
            return (uint64_t)(uintptr_t)v->as_ptr * 2654435761ULL;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — نسخ وتحرير القيم
 * ═══════════════════════════════════════════════════════════════════════════ */

SadValue sad_value_copy(const SadValue* v) {
    if (!v) return sad_value_null();

    SadValue copy;
    memcpy(&copy, v, sizeof(SadValue));

    /* زيادة عدّ مرجعي للأنواع المرجعية */
    switch (v->type) {
        case SAD_TYPE_STRING:
            copy.as_string = sad_string_copy(&v->as_string);
            break;
        case SAD_TYPE_ARRAY:
            if (v->as_array) sad_retain(v->as_array);
            break;
        case SAD_TYPE_MAP:
            if (v->as_map) sad_retain(v->as_map);
            break;
        case SAD_TYPE_OBJECT:
            if (v->as_object) sad_retain(v->as_object);
            break;
        case SAD_TYPE_FUNCTION:
            if (v->as_func) sad_retain(v->as_func);
            break;
        default:
            break;
    }

    return copy;
}

SadValue sad_value_clone(const SadValue* v) {
    if (!v) return sad_value_null();

    SadValue clone;
    memset(&clone, 0, sizeof(SadValue));
    clone.type = v->type;
    clone.flags = v->flags;

    switch (v->type) {
        case SAD_TYPE_INT:
            clone.as_int = v->as_int;
            break;
        case SAD_TYPE_DOUBLE:
            clone.as_double = v->as_double;
            break;
        case SAD_TYPE_BOOL:
            clone.as_bool = v->as_bool;
            break;
        case SAD_TYPE_STRING:
            clone.as_string = sad_string_clone(&v->as_string);
            break;
        case SAD_TYPE_ARRAY:
            clone.as_array = sad_array_clone(v->as_array);
            break;
        case SAD_TYPE_MAP:
            clone.as_map = sad_map_clone(v->as_map);
            break;
        default:
            memcpy(&clone, v, sizeof(SadValue));
            break;
    }

    return clone;
}

void sad_value_free(SadValue* v) {
    if (!v) return;

    switch (v->type) {
        case SAD_TYPE_STRING:
            sad_string_free(&v->as_string);
            break;
        case SAD_TYPE_ARRAY:
            if (v->as_array) {
                sad_release(v->as_array);
                v->as_array = NULL;
            }
            break;
        case SAD_TYPE_MAP:
            if (v->as_map) {
                sad_release(v->as_map);
                v->as_map = NULL;
            }
            break;
        case SAD_TYPE_OBJECT:
            if (v->as_object) {
                sad_release(v->as_object);
                v->as_object = NULL;
            }
            break;
        case SAD_TYPE_FUNCTION:
            if (v->as_func) {
                sad_release(v->as_func);
                v->as_func = NULL;
            }
            break;
        default:
            break;
    }

    v->type = SAD_TYPE_VOID;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — العمليات الحسابية
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * قواعد الترقية (Type Promotion):
 *   رقم + رقم    = رقم
 *   رقم + عشري   = عشري (يُرقّى الرقم)
 *   عشري + عشري  = عشري
 *   نص + أيّ شيء = نص (concatenation — مثل: "العمر: " + 25)
 */

SadValue sad_value_add(const SadValue* a, const SadValue* b) {
    if (!a || !b) return sad_value_null();

    /* نص + شيء = دمج نصوص */
    if (a->type == SAD_TYPE_STRING || b->type == SAD_TYPE_STRING) {
        SadString sa = sad_value_to_string(a);
        SadString sb = sad_value_to_string(b);
        SadString result = sad_string_concat(&sa, &sb);
        sad_string_free(&sa);
        sad_string_free(&sb);
        return sad_value_from_string(result);
    }

    /* أعداد */
    if (a->type == SAD_TYPE_INT && b->type == SAD_TYPE_INT) {
        return sad_value_int(a->as_int + b->as_int);
    }

    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        return sad_value_double(sad_value_to_double(a) + sad_value_to_double(b));
    }

    return sad_value_null(); /* عملية غير مدعومة */
}

SadValue sad_value_sub(const SadValue* a, const SadValue* b) {
    if (!a || !b) return sad_value_null();

    if (a->type == SAD_TYPE_INT && b->type == SAD_TYPE_INT) {
        return sad_value_int(a->as_int - b->as_int);
    }

    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        return sad_value_double(sad_value_to_double(a) - sad_value_to_double(b));
    }

    return sad_value_null();
}

SadValue sad_value_mul(const SadValue* a, const SadValue* b) {
    if (!a || !b) return sad_value_null();

    if (a->type == SAD_TYPE_INT && b->type == SAD_TYPE_INT) {
        return sad_value_int(a->as_int * b->as_int);
    }

    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        return sad_value_double(sad_value_to_double(a) * sad_value_to_double(b));
    }

    return sad_value_null();
}

SadValue sad_value_div(const SadValue* a, const SadValue* b) {
    if (!a || !b) return sad_value_null();

    /* فحص القسمة على صفر */
    if (sad_value_is_number(b)) {
        double divisor = sad_value_to_double(b);
        if (divisor == 0.0) {
            /* خطأ: قسمة على صفر — نُرجع null */
            return sad_value_null();
        }
    }

    if (a->type == SAD_TYPE_INT && b->type == SAD_TYPE_INT && b->as_int != 0) {
        /* قسمة صحيحة */
        if (a->as_int % b->as_int == 0) {
            return sad_value_int(a->as_int / b->as_int);
        }
        /* النتيجة ليست صحيحة — نُرقّي لعشري */
        return sad_value_double((double)a->as_int / (double)b->as_int);
    }

    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        return sad_value_double(sad_value_to_double(a) / sad_value_to_double(b));
    }

    return sad_value_null();
}

SadValue sad_value_mod(const SadValue* a, const SadValue* b) {
    if (!a || !b) return sad_value_null();

    if (a->type == SAD_TYPE_INT && b->type == SAD_TYPE_INT && b->as_int != 0) {
        return sad_value_int(a->as_int % b->as_int);
    }

    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        return sad_value_double(fmod(sad_value_to_double(a), sad_value_to_double(b)));
    }

    return sad_value_null();
}

SadValue sad_value_pow(const SadValue* a, const SadValue* b) {
    if (!a || !b) return sad_value_null();

    if (sad_value_is_number(a) && sad_value_is_number(b)) {
        double result = pow(sad_value_to_double(a), sad_value_to_double(b));

        /* إذا كلاهما صحيح والنتيجة صحيحة — أبقِ صحيح */
        if (a->type == SAD_TYPE_INT && b->type == SAD_TYPE_INT &&
            b->as_int >= 0 && result == (double)(int64_t)result) {
            return sad_value_int((int64_t)result);
        }

        return sad_value_double(result);
    }

    return sad_value_null();
}

SadValue sad_value_neg(const SadValue* a) {
    if (!a) return sad_value_null();

    if (a->type == SAD_TYPE_INT) {
        return sad_value_int(-a->as_int);
    }
    if (a->type == SAD_TYPE_DOUBLE) {
        return sad_value_double(-a->as_double);
    }

    return sad_value_null();
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — أسماء الأنواع
 * ═══════════════════════════════════════════════════════════════════════════ */

const char* sad_value_type_name(SadValueType type) {
    switch (type) {
        case SAD_TYPE_VOID:     return "\xd8\xb9\xd8\xaf\xd9\x85";             /* عدم */
        case SAD_TYPE_NULL:     return "\xd9\x81\xd8\xa7\xd8\xb1\xd8\xba";     /* فارغ */
        case SAD_TYPE_INT:      return "\xd8\xb1\xd9\x82\xd9\x85";             /* رقم */
        case SAD_TYPE_DOUBLE:   return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a";    /* عشري */
        case SAD_TYPE_BOOL:     return "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a"; /* منطقي */
        case SAD_TYPE_STRING:   return "\xd9\x86\xd8\xb5";                     /* نص */
        case SAD_TYPE_ARRAY:    return "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9"; /* مصفوفة */
        case SAD_TYPE_MAP:      return "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9"; /* خريطة */
        case SAD_TYPE_OBJECT:   return "\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86";     /* كائن */
        case SAD_TYPE_FUNCTION: return "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9";     /* دالة */
        case SAD_TYPE_ERROR:    return "\xd8\xae\xd8\xb7\xd8\xa3";             /* خطأ */
        default:                return "\xd9\x85\xd8\xac\xd9\x87\xd9\x88\xd9\x84"; /* مجهول */
    }
}

const char* sad_value_type_name_en(SadValueType type) {
    switch (type) {
        case SAD_TYPE_VOID:     return "void";
        case SAD_TYPE_NULL:     return "null";
        case SAD_TYPE_INT:      return "int";
        case SAD_TYPE_DOUBLE:   return "double";
        case SAD_TYPE_BOOL:     return "bool";
        case SAD_TYPE_STRING:   return "string";
        case SAD_TYPE_ARRAY:    return "array";
        case SAD_TYPE_MAP:      return "map";
        case SAD_TYPE_OBJECT:   return "object";
        case SAD_TYPE_FUNCTION: return "function";
        case SAD_TYPE_ERROR:    return "error";
        default:                return "unknown";
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٨ — إنشاء الكائنات والدوال
 * ═══════════════════════════════════════════════════════════════════════════ */

SadObject* sad_object_new(const char* class_name) {
    SadObject* obj = (SadObject*)sad_rc_alloc(sizeof(SadObject), SAD_OBJ_INSTANCE, NULL);
    if (!obj) return NULL;

    obj->class_name = sad_string_from_cstr(class_name);
    obj->fields = sad_map_new();
    obj->methods = sad_map_new();
    obj->native_data = NULL;

    return obj;
}

void sad_object_free(SadObject* obj) {
    if (!obj) return;

    sad_string_free(&obj->class_name);
    if (obj->fields) sad_map_free(obj->fields);
    if (obj->methods) sad_map_free(obj->methods);
    obj->native_data = NULL;

    sad_release(obj);
}

bool sad_object_set_field(SadObject* obj, const char* name, SadValue value) {
    if (!obj || !name || !obj->fields) return false;

    /* نُحوّل SadValue لـ SadArrayValue لتخزينها في الخريطة */
    SadArrayValue av;
    memset(&av, 0, sizeof(av));

    switch (value.type) {
        case SAD_TYPE_INT:
            av.type = SAD_ELEM_INT;
            av.as_int = value.as_int;
            break;
        case SAD_TYPE_DOUBLE:
            av.type = SAD_ELEM_DOUBLE;
            av.as_double = value.as_double;
            break;
        case SAD_TYPE_BOOL:
            av.type = SAD_ELEM_BOOL;
            av.as_bool = value.as_bool;
            break;
        default:
            av.type = SAD_ELEM_ANY;
            av.as_ptr = value.as_ptr;
            break;
    }

    return sad_map_set_cstr(obj->fields, name, av);
}

SadValue* sad_object_get_field(SadObject* obj, const char* name) {
    if (!obj || !name || !obj->fields) return NULL;
    /* ملاحظة: هذا يُرجع مؤشراً لـ SadArrayValue في الخريطة.
     * سنحتاج لتحويله — في المستقبل سنستخدم خريطة SadValue مباشرة. */
    (void)obj; (void)name;
    return NULL; /* TODO: تحويل النوع بشكل صحيح */
}

SadFunc* sad_func_new(const char* name, void* func_ptr, uint16_t param_count) {
    SadFunc* func = (SadFunc*)sad_rc_alloc(sizeof(SadFunc), SAD_OBJ_CLOSURE, NULL);
    if (!func) return NULL;

    func->func_ptr = func_ptr;
    func->closure_data = NULL;
    func->param_count = param_count;
    func->flags = 0;
    func->name = sad_string_from_cstr(name ? name : "");

    return func;
}

void sad_func_free(SadFunc* func) {
    if (!func) return;
    sad_string_free(&func->name);
    sad_release(func);
}
