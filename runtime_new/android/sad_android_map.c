/**
 * @file sad_android_map.c
 * @brief (AR) تطبيق نظام الخرائط (Robin Hood Hashing) — لغة ص لأندرويد
 * @brief (EN) Hash map implementation (Robin Hood Hashing) — Sad Language Android
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق الخرائط بتقنية Robin Hood — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * Robin Hood Hashing — الطريقة "العادلة":
 * ───────────────────────────────────────
 * تخيّل أن كل عنصر يحاول الجلوس في مقعده المثالي (حسب التجزئة).
 * إذا كان المقعد مشغولاً، يتقدّم للمقعد التالي.
 *
 * القاعدة: إذا العنصر الجديد "أبعد عن بيته" من العنصر الجالس،
 * يأخذ الجديد مكانه ويُزيح القديم. هذا يوازن المسافات ويمنع
 * تكتل العناصر في مكان واحد.
 *
 * مثال:
 *   المقعد 5 ← عنصر أ (مسافة 0 — مقعده المثالي)
 *   المقعد 6 ← عنصر ب (مسافة 2 — مقعده المثالي كان 4)
 *   يأتي عنصر ج (مقعده المثالي 5، المسافة الحالية 1):
 *   → يجلس في 5 لأنه لا "يستحق" سرقة مقعد أ (مسافة 0 < 1)... لا
 *   → ينتقل لـ 6... عنصر ب مسافته 2 > مسافة ج (1)...
 *   → عنصر ج يأخذ مقعد 6، وعنصر ب يبحث عن مقعد جديد (7)
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#include "sad_android_map.h"
#include <string.h>   /* memset, memcpy */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — أدوات داخلية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تقريب العدد لأقرب قوة 2 (أكبر أو تساوي)
 *
 * مثال: 10 → 16, 17 → 32, 8 → 8
 * مهم لأن mod بقوة 2 = AND بماسك بسيط (أسرع بكثير).
 */
static size_t next_power_of_2(size_t v) {
    if (v == 0) return 1;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;
    return v;
}

/**
 * @brief (AR) حساب فهرس الموقع المثالي من التجزئة
 *
 * بما أن السعة دائماً قوة 2: index = hash & (capacity - 1)
 * هذا أسرع من hash % capacity.
 */
static inline size_t ideal_index(uint64_t hash, size_t capacity) {
    return (size_t)(hash & (capacity - 1));
}

/**
 * @brief (AR) تحرير قيمة إدخال (مفتاح + قيمة)
 */
static void release_entry(SadMapEntry* entry) {
    if (!entry || !entry->occupied) return;

    sad_string_free(&entry->key);

    /* تحرير القيمة إذا كانت مرجعية */
    switch (entry->value.type) {
        case SAD_ELEM_STRING:
        case SAD_ELEM_ARRAY:
        case SAD_ELEM_MAP:
        case SAD_ELEM_OBJECT:
            if (entry->value.as_ptr) {
                sad_release(entry->value.as_ptr);
            }
            break;
        default:
            break;
    }

    entry->occupied = false;
}

/** @brief (AR) هل يجب إعادة التجزئة؟ (تجاوز عامل التحميل) */
static bool should_rehash(const SadMap* map) {
    /* count * 4 > capacity * 3 === count/capacity > 3/4 = 75% */
    return (map->count * SAD_MAP_LOAD_FACTOR_DEN) >
           (map->capacity * SAD_MAP_LOAD_FACTOR_NUM);
}

/* إعلان مسبق لإعادة التجزئة */
static bool rehash(SadMap* map, size_t new_capacity);

/**
 * @brief (AR) المُدمّر — يُحرّر كل الإدخالات
 */
static void map_finalizer(void* ptr) {
    SadMap* map = (SadMap*)ptr;
    if (!map) return;

    if (map->entries) {
        for (size_t i = 0; i < map->capacity; i++) {
            release_entry(&map->entries[i]);
        }
        sad_free(map->entries);
        map->entries = NULL;
    }

    map->count = 0;
    map->capacity = 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — إنشاء وتدمير الخريطة
 * ═══════════════════════════════════════════════════════════════════════════ */

SadMap* sad_map_new(void) {
    return sad_map_with_capacity(SAD_MAP_INITIAL_CAPACITY);
}

SadMap* sad_map_with_capacity(size_t initial_capacity) {
    if (initial_capacity < SAD_MAP_INITIAL_CAPACITY) {
        initial_capacity = SAD_MAP_INITIAL_CAPACITY;
    }

    /* تقريب لقوة 2 */
    initial_capacity = next_power_of_2(initial_capacity);

    if (initial_capacity > SAD_MAP_MAX_CAPACITY) {
        initial_capacity = SAD_MAP_MAX_CAPACITY;
    }

    SadMap* map = (SadMap*)sad_rc_alloc(sizeof(SadMap), SAD_OBJ_MAP, map_finalizer);
    if (!map) return NULL;

    map->entries = (SadMapEntry*)sad_alloc(initial_capacity * sizeof(SadMapEntry));
    if (!map->entries) {
        sad_release(map);
        return NULL;
    }

    memset(map->entries, 0, initial_capacity * sizeof(SadMapEntry));
    map->count = 0;
    map->capacity = initial_capacity;

    return map;
}

void sad_map_free(SadMap* map) {
    if (!map) return;
    sad_release(map);
}

SadMap* sad_map_retain(SadMap* map) {
    if (map) sad_retain(map);
    return map;
}

SadMap* sad_map_clone(const SadMap* map) {
    if (!map) return NULL;

    SadMap* clone = sad_map_with_capacity(map->capacity);
    if (!clone) return NULL;

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].occupied) {
            const SadMapEntry* src = &map->entries[i];
            SadMapEntry* dst = &clone->entries[i];

            dst->key_hash = src->key_hash;
            dst->key = sad_string_copy(&src->key);
            dst->value = src->value;
            dst->distance = src->distance;
            dst->occupied = true;

            /* زيادة عدّ مرجعي للقيم المرجعية */
            switch (dst->value.type) {
                case SAD_ELEM_STRING:
                case SAD_ELEM_ARRAY:
                case SAD_ELEM_MAP:
                case SAD_ELEM_OBJECT:
                    if (dst->value.as_ptr) {
                        sad_retain(dst->value.as_ptr);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    clone->count = map->count;
    return clone;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — عمليات القراءة
 * ═══════════════════════════════════════════════════════════════════════════ */

SadArrayValue* sad_map_get(const SadMap* map, const SadString* key) {
    if (!map || !key || map->count == 0) return NULL;

    uint64_t hash = sad_string_hash(key);
    size_t idx = ideal_index(hash, map->capacity);
    uint32_t dist = 0;

    /*
     * بحث Robin Hood:
     * نبدأ من الموقع المثالي وننتقل للأمام.
     * نتوقف عندما:
     *  ١. نجد المفتاح (نجاح!)
     *  ٢. نجد موقعاً فارغاً (المفتاح غير موجود)
     *  ٣. المسافة الحالية أكبر من مسافة العنصر الموجود
     *     (لأن Robin Hood يضمن أن العناصر البعيدة تأتي أولاً)
     */
    while (true) {
        const SadMapEntry* entry = &map->entries[idx];

        if (!entry->occupied) {
            return NULL; /* مكان فارغ — المفتاح غير موجود */
        }

        if (dist > entry->distance) {
            return NULL; /* Robin Hood: لو كان المفتاح موجوداً، لكان هنا أو قبل */
        }

        if (entry->key_hash == hash && sad_string_equals(&entry->key, key)) {
            return (SadArrayValue*)&entry->value; /* وُجد! */
        }

        idx = (idx + 1) & (map->capacity - 1);
        dist++;
    }
}

SadArrayValue* sad_map_get_cstr(const SadMap* map, const char* key) {
    SadString skey = sad_string_from_cstr(key);
    SadArrayValue* result = sad_map_get(map, &skey);
    sad_string_free(&skey);
    return result;
}

bool sad_map_contains(const SadMap* map, const SadString* key) {
    return sad_map_get(map, key) != NULL;
}

bool sad_map_contains_cstr(const SadMap* map, const char* key) {
    return sad_map_get_cstr(map, key) != NULL;
}

size_t sad_map_count(const SadMap* map) {
    return map ? map->count : 0;
}

bool sad_map_is_empty(const SadMap* map) {
    return !map || map->count == 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — إعادة التجزئة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إدراج إدخال عند إعادة التجزئة (بدون فحص التكرار)
 */
static void insert_for_rehash(SadMapEntry* entries, size_t capacity,
                               SadMapEntry entry) {
    size_t idx = ideal_index(entry.key_hash, capacity);
    uint32_t dist = 0;

    while (true) {
        SadMapEntry* slot = &entries[idx];

        if (!slot->occupied) {
            entry.distance = dist;
            *slot = entry;
            return;
        }

        /* Robin Hood: إذا الإدخال الحالي أبعد، بدّل */
        if (dist > slot->distance) {
            SadMapEntry tmp = *slot;
            entry.distance = dist;
            *slot = entry;
            entry = tmp;
            dist = entry.distance;
        }

        idx = (idx + 1) & (capacity - 1);
        dist++;
    }
}

static bool rehash(SadMap* map, size_t new_capacity) {
    /*
     * إعادة التجزئة:
     * ١. نُخصّص مصفوفة جديدة بالسعة الجديدة
     * ٢. ننقل كل الإدخالات (نُعيد حساب المواقع)
     * ٣. نُحرّر المصفوفة القديمة
     *
     * هذه عملية O(n) لكنها نادرة (تتضاعف السعة).
     */
    new_capacity = next_power_of_2(new_capacity);
    if (new_capacity > SAD_MAP_MAX_CAPACITY) return false;

    SadMapEntry* new_entries = (SadMapEntry*)sad_alloc(
        new_capacity * sizeof(SadMapEntry));
    if (!new_entries) return false;

    memset(new_entries, 0, new_capacity * sizeof(SadMapEntry));

    /* نقل الإدخالات */
    SadMapEntry* old_entries = map->entries;
    size_t old_capacity = map->capacity;

    for (size_t i = 0; i < old_capacity; i++) {
        if (old_entries[i].occupied) {
            insert_for_rehash(new_entries, new_capacity, old_entries[i]);
        }
    }

    /* استبدال */
    sad_free(old_entries);
    map->entries = new_entries;
    map->capacity = new_capacity;

    return true;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عمليات الكتابة
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_map_set(SadMap* map, const SadString* key, SadArrayValue value) {
    if (!map || !key) return false;

    /* فحص عامل التحميل */
    if (should_rehash(map)) {
        if (!rehash(map, map->capacity * 2)) {
            return false;
        }
    }

    uint64_t hash = sad_string_hash(key);
    size_t idx = ideal_index(hash, map->capacity);
    uint32_t dist = 0;

    /* تحضير الإدخال الجديد */
    SadMapEntry new_entry;
    new_entry.key_hash = hash;
    new_entry.key = sad_string_copy(key);
    new_entry.value = value;
    new_entry.distance = 0;
    new_entry.occupied = true;

    /* زيادة عدّ مرجعي للقيمة */
    switch (value.type) {
        case SAD_ELEM_STRING:
        case SAD_ELEM_ARRAY:
        case SAD_ELEM_MAP:
        case SAD_ELEM_OBJECT:
            if (value.as_ptr) sad_retain(value.as_ptr);
            break;
        default:
            break;
    }

    bool is_new = true;

    while (true) {
        SadMapEntry* slot = &map->entries[idx];

        if (!slot->occupied) {
            /* مكان فارغ — إدراج مباشر */
            new_entry.distance = dist;
            *slot = new_entry;
            map->count++;
            return is_new; /* true = إدراج جديد */
        }

        /* هل هذا نفس المفتاح؟ → تحديث */
        if (slot->key_hash == hash && sad_string_equals(&slot->key, key)) {
            /* تحرير القيمة القديمة */
            switch (slot->value.type) {
                case SAD_ELEM_STRING:
                case SAD_ELEM_ARRAY:
                case SAD_ELEM_MAP:
                case SAD_ELEM_OBJECT:
                    if (slot->value.as_ptr) sad_release(slot->value.as_ptr);
                    break;
                default:
                    break;
            }

            slot->value = new_entry.value;
            sad_string_free(&new_entry.key); /* نحرّر النسخة — المفتاح القديم يبقى */
            return false; /* تحديث (ليس إدراجاً جديداً) */
        }

        /* Robin Hood: الإدخال الجديد أبعد عن بيته — يسرق الموقع */
        if (dist > slot->distance) {
            SadMapEntry tmp = *slot;
            new_entry.distance = dist;
            *slot = new_entry;
            new_entry = tmp;
            dist = new_entry.distance;
        }

        idx = (idx + 1) & (map->capacity - 1);
        dist++;
    }
}

bool sad_map_set_cstr(SadMap* map, const char* key, SadArrayValue value) {
    SadString skey = sad_string_from_cstr(key);
    bool result = sad_map_set(map, &skey, value);
    sad_string_free(&skey);
    return result;
}

bool sad_map_remove(SadMap* map, const SadString* key, SadArrayValue* out_value) {
    if (!map || !key || map->count == 0) return false;

    uint64_t hash = sad_string_hash(key);
    size_t idx = ideal_index(hash, map->capacity);
    uint32_t dist = 0;

    /* بحث عن المفتاح */
    while (true) {
        SadMapEntry* slot = &map->entries[idx];

        if (!slot->occupied || dist > slot->distance) {
            return false; /* غير موجود */
        }

        if (slot->key_hash == hash && sad_string_equals(&slot->key, key)) {
            /*
             * وُجد! نحذفه ونُرتّب العناصر التالية:
             *
             * "Backward Shift Delete" — بعد الحذف، ننقل العناصر
             * التي مسافتها > 0 خطوة للخلف، مما يُقلّل مسافاتها.
             */
            if (out_value) {
                *out_value = slot->value;
                /* لا نُحرّر القيمة — المستدعي يملكها الآن */
            } else {
                /* تحرير القيمة */
                switch (slot->value.type) {
                    case SAD_ELEM_STRING:
                    case SAD_ELEM_ARRAY:
                    case SAD_ELEM_MAP:
                    case SAD_ELEM_OBJECT:
                        if (slot->value.as_ptr) sad_release(slot->value.as_ptr);
                        break;
                    default:
                        break;
                }
            }

            /* تحرير المفتاح */
            sad_string_free(&slot->key);
            slot->occupied = false;
            map->count--;

            /* Backward shift: نقل العناصر التالية خطوة للخلف */
            size_t prev = idx;
            idx = (idx + 1) & (map->capacity - 1);

            while (map->entries[idx].occupied && map->entries[idx].distance > 0) {
                map->entries[prev] = map->entries[idx];
                map->entries[prev].distance--;
                map->entries[idx].occupied = false;

                prev = idx;
                idx = (idx + 1) & (map->capacity - 1);
            }

            return true;
        }

        idx = (idx + 1) & (map->capacity - 1);
        dist++;
    }
}

bool sad_map_remove_cstr(SadMap* map, const char* key, SadArrayValue* out_value) {
    SadString skey = sad_string_from_cstr(key);
    bool result = sad_map_remove(map, &skey, out_value);
    sad_string_free(&skey);
    return result;
}

void sad_map_clear(SadMap* map) {
    if (!map) return;

    for (size_t i = 0; i < map->capacity; i++) {
        release_entry(&map->entries[i]);
    }

    memset(map->entries, 0, map->capacity * sizeof(SadMapEntry));
    map->count = 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — التكرار
 * ═══════════════════════════════════════════════════════════════════════════ */

void sad_map_foreach(SadMap* map, SadMapIterFn fn, void* context) {
    if (!map || !fn) return;

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].occupied) {
            bool cont = fn(&map->entries[i].key,
                           &map->entries[i].value, context);
            if (!cont) return; /* دالة الاستدعاء طلبت التوقف */
        }
    }
}

SadArray* sad_map_keys(const SadMap* map) {
    SadArray* arr = sad_array_new(SAD_ELEM_STRING);
    if (!map || !arr) return arr;

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].occupied) {
            SadString key_copy = sad_string_copy(&map->entries[i].key);
            sad_array_push(arr, sad_val_ptr(&key_copy, SAD_ELEM_STRING));
        }
    }

    return arr;
}

SadArray* sad_map_values(const SadMap* map) {
    SadArray* arr = sad_array_new(SAD_ELEM_ANY);
    if (!map || !arr) return arr;

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].occupied) {
            sad_array_push(arr, map->entries[i].value);
        }
    }

    return arr;
}
