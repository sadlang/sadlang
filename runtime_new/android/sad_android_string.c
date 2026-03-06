/**
 * @file sad_android_string.c
 * @brief (AR) تطبيق نظام السلاسل النصية لأندرويد — UTF-8 مع SSO والعدّ المرجعي
 * @brief (EN) Android string system implementation — UTF-8 with SSO and ref counting
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق السلاسل النصية — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يحتوي التطبيق الكامل لنظام السلاسل النصية المُصمم
 * لدعم اللغة العربية بكفاءة عالية مع تحسين السلاسل القصيرة (SSO).
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 * @date مارس 2026
 */

#include "sad_android_string.h"
#include <string.h>   /* memcpy, memset, memmem, strcmp */
#include <stdio.h>    /* snprintf */
#include <stdarg.h>   /* va_list */
#include <stdlib.h>   /* strtoll, strtod */

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — أدوات UTF-8
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * UTF-8 هو الترميز القياسي في أندرويد ولينكس.
 * يُمثّل كل حرف يونيكود بـ 1-4 بايت:
 *
 *  البايت الأول  │ عدد البايتات │ مدى يونيكود
 * ──────────────┼─────────────┼───────────────
 *  0xxxxxxx     │     1       │ U+0000 – U+007F (ASCII)
 *  110xxxxx     │     2       │ U+0080 – U+07FF (عربي، لاتيني موسّع)
 *  1110xxxx     │     3       │ U+0800 – U+FFFF (صيني، ياباني)
 *  11110xxx     │     4       │ U+10000 – U+10FFFF (إيموجي)
 *
 * الحروف العربية (U+0600-U+06FF) تقع في المدى الذي يأخذ 2 بايت.
 * مثلاً: حرف 'م' (U+0645) = [0xD9, 0x85] في UTF-8.
 */

int sad_utf8_char_size(uint8_t first_byte) {
    /*
     * تحديد عدد بايتات الحرف من أول بايت:
     *
     * بت 7=0 → ASCII (1 بايت): أرقام، حروف لاتينية، علامات
     * بت 7,6=11, بت 5=0 → 2 بايت: عربي، عبري، يوناني
     * بت 7,6,5=111, بت 4=0 → 3 بايت: صيني، ياباني، كوري
     * بت 7,6,5,4=1111, بت 3=0 → 4 بايت: إيموجي، رموز نادرة
     */
    if ((first_byte & 0x80) == 0x00) return 1;       /* 0xxxxxxx */
    if ((first_byte & 0xE0) == 0xC0) return 2;       /* 110xxxxx */
    if ((first_byte & 0xF0) == 0xE0) return 3;       /* 1110xxxx */
    if ((first_byte & 0xF8) == 0xF0) return 4;       /* 11110xxx */
    return 0; /* بايت غير صالح — لا يجب أن يكون أول بايت */
}

int sad_utf8_encode(uint32_t codepoint, char* out) {
    /*
     * تحويل نقطة كود يونيكود إلى تسلسل بايتات UTF-8.
     *
     * مثال: حرف 'ص' (U+0635):
     *   ١. المدى: U+0080 – U+07FF → 2 بايت
     *   ٢. القالب: 110xxxxx 10xxxxxx
     *   ٣. 0x0635 = 0b 011000 110101
     *   ٤. النتيجة: [0xD8, 0xB5]
     */
    if (codepoint <= 0x7F) {
        out[0] = (char)codepoint;
        return 1;
    }
    if (codepoint <= 0x7FF) {
        out[0] = (char)(0xC0 | (codepoint >> 6));
        out[1] = (char)(0x80 | (codepoint & 0x3F));
        return 2;
    }
    if (codepoint <= 0xFFFF) {
        out[0] = (char)(0xE0 | (codepoint >> 12));
        out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | (codepoint & 0x3F));
        return 3;
    }
    if (codepoint <= 0x10FFFF) {
        out[0] = (char)(0xF0 | (codepoint >> 18));
        out[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[3] = (char)(0x80 | (codepoint & 0x3F));
        return 4;
    }

    /* نقطة كود غير صالحة */
    return 0;
}

int sad_utf8_decode(const char* data, uint32_t* out_codepoint) {
    if (!data || !out_codepoint) return 0;

    uint8_t b0 = (uint8_t)data[0];
    int size = sad_utf8_char_size(b0);

    switch (size) {
        case 1:
            *out_codepoint = b0;
            return 1;
        case 2:
            *out_codepoint = ((b0 & 0x1F) << 6) |
                             ((uint8_t)data[1] & 0x3F);
            return 2;
        case 3:
            *out_codepoint = ((b0 & 0x0F) << 12) |
                             (((uint8_t)data[1] & 0x3F) << 6) |
                             ((uint8_t)data[2] & 0x3F);
            return 3;
        case 4:
            *out_codepoint = ((b0 & 0x07) << 18) |
                             (((uint8_t)data[1] & 0x3F) << 12) |
                             (((uint8_t)data[2] & 0x3F) << 6) |
                             ((uint8_t)data[3] & 0x3F);
            return 4;
        default:
            *out_codepoint = 0xFFFD; /* حرف الاستبدال (�) */
            return 1;
    }
}

bool sad_utf8_validate(const char* data, size_t length) {
    if (!data) return false;

    size_t i = 0;
    while (i < length) {
        int char_size = sad_utf8_char_size((uint8_t)data[i]);
        if (char_size == 0 || i + char_size > length) {
            return false;
        }

        /* التحقق من بايتات الاستمرار (continuation bytes) */
        for (int j = 1; j < char_size; j++) {
            if (((uint8_t)data[i + j] & 0xC0) != 0x80) {
                return false; /* بايت استمرار غير صالح */
            }
        }

        i += char_size;
    }

    return true;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — إنشاء وتدمير السلاسل
 * ═══════════════════════════════════════════════════════════════════════════ */

SadString sad_string_new(const char* data, size_t length) {
    SadString str;
    memset(&str, 0, sizeof(SadString));

    if (!data || length == 0) {
        /* سلسلة فارغة — SSO */
        str.sso.data[0] = '\0';
        str.sso.length = 0;
        str.flags = SAD_STR_FLAG_SSO;
        return str;
    }

    if (length <= SAD_SSO_MAX_SIZE) {
        /*
         * سلسلة قصيرة — تُخزّن داخل البنية (لا heap):
         *
         * ┌──────────────────────┬─────┬──────┐
         * │ data (22 بايت حد)   │ len │flags │
         * │ "مرحباً" مباشرة هنا │ 12  │ SSO  │
         * └──────────────────────┴─────┴──────┘
         */
        memcpy(str.sso.data, data, length);
        str.sso.data[length] = '\0';
        str.sso.length = (uint8_t)length;
        str.flags = SAD_STR_FLAG_SSO;
    } else {
        /*
         * سلسلة طويلة — تُخصص على الـ heap:
         * نُخصص بسعة أكبر قليلاً (1.5x) للسماح بالإلحاق المستقبلي
         * بدون إعادة تخصيص.
         */
        size_t capacity = length + (length / 2) + 1; /* 1.5x + 1 للـ '\0' */
        str.heap.data = (char*)sad_rc_alloc(capacity, SAD_OBJ_STRING, NULL);
        if (!str.heap.data) {
            /* فشل التخصيص — أعد سلسلة فارغة */
            str.sso.data[0] = '\0';
            str.sso.length = 0;
            str.flags = SAD_STR_FLAG_SSO;
            return str;
        }

        memcpy(str.heap.data, data, length);
        str.heap.data[length] = '\0';
        str.heap.length = length;
        str.heap.capacity = capacity;
        str.flags = SAD_STR_FLAG_OWNED;
    }

    return str;
}

SadString sad_string_from_cstr(const char* cstr) {
    if (!cstr) return sad_string_empty();
    return sad_string_new(cstr, strlen(cstr));
}

SadString sad_string_empty(void) {
    SadString str;
    memset(&str, 0, sizeof(SadString));
    str.sso.data[0] = '\0';
    str.sso.length = 0;
    str.flags = SAD_STR_FLAG_SSO;
    return str;
}

void sad_string_free(SadString* str) {
    if (!str) return;

    if (!(str->flags & SAD_STR_FLAG_SSO) && (str->flags & SAD_STR_FLAG_OWNED)) {
        /*
         * سلسلة heap — نُنقص عدد المراجع.
         * إذا وصل لصفر، sad_release تُحرّر الذاكرة تلقائياً.
         */
        if (str->heap.data) {
            sad_release(str->heap.data);
        }
    }

    /* تصفير البنية */
    memset(str, 0, sizeof(SadString));
    str->flags = SAD_STR_FLAG_SSO;
}

SadString sad_string_copy(const SadString* str) {
    if (!str) return sad_string_empty();

    SadString copy;
    memcpy(&copy, str, sizeof(SadString));

    if (!(str->flags & SAD_STR_FLAG_SSO) && (str->flags & SAD_STR_FLAG_OWNED)) {
        /*
         * سلسلة heap — فقط نزيد عدد المراجع (لا نسخ بيانات!).
         * هذا يجعل نسخ السلاسل الطويلة عملية O(1).
         *
         * تصبح كلتا السلسلتين تتشاركان نفس البيانات.
         * عند التعديل، COW (النسخ عند الكتابة) سيُنشئ نسخة مستقلة.
         */
        if (str->heap.data) {
            sad_retain(copy.heap.data);
        }
    }
    /* سلسلة SSO: memcpy أعلاه كافٍ — البيانات داخل البنية */

    return copy;
}

SadString sad_string_clone(const SadString* str) {
    if (!str) return sad_string_empty();

    /* نسخ عميق دائماً — لا مشاركة مراجع */
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);
    return sad_string_new(data, len);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — الوصول للبيانات
 * ═══════════════════════════════════════════════════════════════════════════ */

const char* sad_string_data(const SadString* str) {
    if (!str) return "";
    if (str->flags & SAD_STR_FLAG_SSO) {
        return str->sso.data;
    }
    return str->heap.data ? str->heap.data : "";
}

size_t sad_string_length(const SadString* str) {
    if (!str) return 0;
    if (str->flags & SAD_STR_FLAG_SSO) {
        return str->sso.length;
    }
    return str->heap.length;
}

size_t sad_string_char_count(const SadString* str) {
    /*
     * حساب عدد أحرف يونيكود — يتطلب تجاوز كل البايتات.
     * هذا أبطأ من sad_string_length (O(n) بدلاً من O(1)).
     *
     * "مرحبا" (10 بايت) → 5 أحرف
     * "Hello" (5 بايت) → 5 أحرف
     * "مرحبا Hello" (16 بايت) → 11 حرف
     */
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);
    size_t count = 0;
    size_t i = 0;

    while (i < len) {
        int char_size = sad_utf8_char_size((uint8_t)data[i]);
        if (char_size == 0) char_size = 1; /* بايت غير صالح — تخطّيه */
        i += char_size;
        count++;
    }

    return count;
}

bool sad_string_is_empty(const SadString* str) {
    return sad_string_length(str) == 0;
}

bool sad_string_is_sso(const SadString* str) {
    return str && (str->flags & SAD_STR_FLAG_SSO);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — عمليات التعديل
 * ═══════════════════════════════════════════════════════════════════════════ */

SadString sad_string_concat(const SadString* a, const SadString* b) {
    const char* a_data = sad_string_data(a);
    size_t a_len = sad_string_length(a);
    const char* b_data = sad_string_data(b);
    size_t b_len = sad_string_length(b);

    size_t total = a_len + b_len;

    if (total <= SAD_SSO_MAX_SIZE) {
        /* النتيجة قصيرة بما يكفي لـ SSO */
        SadString result;
        memset(&result, 0, sizeof(SadString));
        memcpy(result.sso.data, a_data, a_len);
        memcpy(result.sso.data + a_len, b_data, b_len);
        result.sso.data[total] = '\0';
        result.sso.length = (uint8_t)total;
        result.flags = SAD_STR_FLAG_SSO;
        return result;
    }

    /* النتيجة طويلة — تخصيص heap */
    SadString result;
    memset(&result, 0, sizeof(SadString));

    size_t capacity = total + (total / 2) + 1;
    result.heap.data = (char*)sad_rc_alloc(capacity, SAD_OBJ_STRING, NULL);
    if (!result.heap.data) {
        return sad_string_empty();
    }

    memcpy(result.heap.data, a_data, a_len);
    memcpy(result.heap.data + a_len, b_data, b_len);
    result.heap.data[total] = '\0';
    result.heap.length = total;
    result.heap.capacity = capacity;
    result.flags = SAD_STR_FLAG_OWNED;

    return result;
}

void sad_string_append(SadString* str, const char* suffix, size_t suffix_len) {
    if (!str || !suffix || suffix_len == 0) return;

    /*
     * الإلحاق في المكان — ثلاث حالات:
     *
     * ١. SSO + النتيجة تبقى SSO: مجرد نسخ إضافي
     * ٢. SSO + النتيجة تتجاوز SSO: تحويل إلى heap
     * ٣. Heap: إعادة تخصيص إذا لا يوجد مساحة
     */
    size_t old_len = sad_string_length(str);
    size_t new_len = old_len + suffix_len;

    if (str->flags & SAD_STR_FLAG_SSO) {
        if (new_len <= SAD_SSO_MAX_SIZE) {
            /* الحالة ١: يبقى SSO */
            memcpy(str->sso.data + old_len, suffix, suffix_len);
            str->sso.data[new_len] = '\0';
            str->sso.length = (uint8_t)new_len;
        } else {
            /* الحالة ٢: تحويل من SSO إلى heap */
            size_t capacity = new_len + (new_len / 2) + 1;
            char* new_data = (char*)sad_rc_alloc(capacity, SAD_OBJ_STRING, NULL);
            if (!new_data) return;

            memcpy(new_data, str->sso.data, old_len);
            memcpy(new_data + old_len, suffix, suffix_len);
            new_data[new_len] = '\0';

            str->heap.data = new_data;
            str->heap.length = new_len;
            str->heap.capacity = capacity;
            str->flags = SAD_STR_FLAG_OWNED;
        }
    } else {
        /* الحالة ٣: heap — تحقق من السعة */
        if (new_len + 1 > str->heap.capacity) {
            /* سعة غير كافية — إعادة تخصيص */
            size_t new_capacity = new_len + (new_len / 2) + 1;
            char* new_data = (char*)sad_rc_alloc(new_capacity, SAD_OBJ_STRING, NULL);
            if (!new_data) return;

            memcpy(new_data, str->heap.data, old_len);
            sad_release(str->heap.data);

            str->heap.data = new_data;
            str->heap.capacity = new_capacity;
        }

        memcpy(str->heap.data + old_len, suffix, suffix_len);
        str->heap.data[new_len] = '\0';
        str->heap.length = new_len;
    }
}

SadString sad_string_substr(const SadString* str, size_t start, size_t length) {
    const char* data = sad_string_data(str);
    size_t total_len = sad_string_length(str);

    if (start >= total_len) return sad_string_empty();
    if (start + length > total_len) length = total_len - start;

    return sad_string_new(data + start, length);
}

SadString sad_string_substr_chars(const SadString* str, size_t char_start, size_t char_count) {
    /*
     * استخراج جزء بأحرف يونيكود:
     * نحتاج تجاوز UTF-8 لإيجاد مواقع البايت الصحيحة.
     *
     * مثال: "مرحباً بالعالم" (28 بايت)
     *  char_start=7 ("ب"), char_count=6 ("بالعالم")
     *  → نتجاوز 7 أحرف = 14 بايت → byte_start = 14
     *  → نتجاوز 6 أحرف = 12 بايت → byte_length = 12
     */
    const char* data = sad_string_data(str);
    size_t total_len = sad_string_length(str);

    /* إيجاد byte_start */
    size_t byte_start = 0;
    size_t chars_skipped = 0;
    while (byte_start < total_len && chars_skipped < char_start) {
        int cs = sad_utf8_char_size((uint8_t)data[byte_start]);
        if (cs == 0) cs = 1;
        byte_start += cs;
        chars_skipped++;
    }

    /* إيجاد byte_length */
    size_t byte_end = byte_start;
    size_t chars_included = 0;
    while (byte_end < total_len && chars_included < char_count) {
        int cs = sad_utf8_char_size((uint8_t)data[byte_end]);
        if (cs == 0) cs = 1;
        byte_end += cs;
        chars_included++;
    }

    return sad_string_new(data + byte_start, byte_end - byte_start);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عمليات البحث والمقارنة
 * ═══════════════════════════════════════════════════════════════════════════ */

size_t sad_string_find(const SadString* str, const char* needle, size_t needle_len) {
    const char* data = sad_string_data(str);
    size_t data_len = sad_string_length(str);

    if (needle_len == 0 || needle_len > data_len) return (size_t)-1;

    /*
     * بحث بسيط — يمكن تحسينه لاحقاً بخوارزمية KMP أو Boyer-Moore.
     * لسلاسل المحادثة القصيرة، البحث الخطي كافٍ.
     */
    for (size_t i = 0; i <= data_len - needle_len; i++) {
        if (memcmp(data + i, needle, needle_len) == 0) {
            return i;
        }
    }

    return (size_t)-1;
}

bool sad_string_contains(const SadString* str, const char* needle, size_t needle_len) {
    return sad_string_find(str, needle, needle_len) != (size_t)-1;
}

bool sad_string_starts_with(const SadString* str, const char* prefix, size_t prefix_len) {
    size_t len = sad_string_length(str);
    if (prefix_len > len) return false;
    return memcmp(sad_string_data(str), prefix, prefix_len) == 0;
}

bool sad_string_ends_with(const SadString* str, const char* suffix, size_t suffix_len) {
    size_t len = sad_string_length(str);
    if (suffix_len > len) return false;
    return memcmp(sad_string_data(str) + len - suffix_len, suffix, suffix_len) == 0;
}

bool sad_string_equals(const SadString* a, const SadString* b) {
    size_t a_len = sad_string_length(a);
    size_t b_len = sad_string_length(b);

    if (a_len != b_len) return false;
    if (a_len == 0) return true; /* كلتاهما فارغتان */

    return memcmp(sad_string_data(a), sad_string_data(b), a_len) == 0;
}

int sad_string_compare(const SadString* a, const SadString* b) {
    const char* a_data = sad_string_data(a);
    size_t a_len = sad_string_length(a);
    const char* b_data = sad_string_data(b);
    size_t b_len = sad_string_length(b);

    size_t min_len = a_len < b_len ? a_len : b_len;
    int cmp = memcmp(a_data, b_data, min_len);

    if (cmp != 0) return cmp;
    if (a_len < b_len) return -1;
    if (a_len > b_len) return 1;
    return 0;
}

uint64_t sad_string_hash(const SadString* str) {
    /*
     * خوارزمية FNV-1a — سريعة وتوزيع ممتاز:
     *
     * hash = FNV_OFFSET_BASIS
     * لكل بايت:
     *   hash ^= بايت
     *   hash *= FNV_PRIME
     */
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);

    uint64_t hash = 14695981039346656037ULL;  /* FNV offset basis (64-bit) */
    for (size_t i = 0; i < len; i++) {
        hash ^= (uint8_t)data[i];
        hash *= 1099511628211ULL;             /* FNV prime (64-bit) */
    }

    return hash;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — عمليات التحويل
 * ═══════════════════════════════════════════════════════════════════════════ */

SadString sad_string_from_int(int64_t value) {
    char buf[32]; /* كافٍ لأي int64_t */
    int len = snprintf(buf, sizeof(buf), "%lld", (long long)value);
    return sad_string_new(buf, (size_t)len);
}

SadString sad_string_from_double(double value) {
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%g", value);
    return sad_string_new(buf, (size_t)len);
}

SadString sad_string_from_bool(bool value) {
    /*
     * نُرجع الكلمات العربية — لأن لغة ص عربية!
     * "صحيح" = 8 بايت (4 حروف × 2)
     * "خطأ" = 6 بايت (3 حروف × 2)
     * كلاهما يقع في نطاق SSO (≤ 22 بايت).
     */
    if (value) {
        return sad_string_from_cstr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad"); /* صحيح */
    } else {
        return sad_string_from_cstr("\xd8\xae\xd8\xb7\xd8\xa3");         /* خطأ */
    }
}

bool sad_string_to_int(const SadString* str, int64_t* out_value) {
    if (!str || !out_value) return false;

    const char* data = sad_string_data(str);
    char* end;
    *out_value = strtoll(data, &end, 10);

    /* نجح التحويل إذا end يشير لنهاية السلسلة */
    return (end != data && *end == '\0');
}

bool sad_string_to_double(const SadString* str, double* out_value) {
    if (!str || !out_value) return false;

    const char* data = sad_string_data(str);
    char* end;
    *out_value = strtod(data, &end);

    return (end != data && *end == '\0');
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — عمليات عربية خاصة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) هل نقطة الكود هذه علامة تشكيل عربية؟
 * @brief (EN) Is this code point an Arabic diacritical mark?
 *
 * علامات التشكيل العربية (U+064B – U+065F):
 *   U+064B = فتحتان (ً)
 *   U+064C = ضمتان (ٌ)
 *   U+064D = كسرتان (ٍ)
 *   U+064E = فتحة (َ)
 *   U+064F = ضمة (ُ)
 *   U+0650 = كسرة (ِ)
 *   U+0651 = شدة (ّ)
 *   U+0652 = سكون (ْ)
 *   U+0653 = مدة (ٓ)
 *   U+0654 = همزة فوقية (ٔ)
 *   U+0655 = همزة تحتية (ٕ)
 *   U+0656 – U+065F = علامات إضافية
 */
static bool is_arabic_diacritical(uint32_t codepoint) {
    return (codepoint >= 0x064B && codepoint <= 0x065F) ||
           (codepoint >= 0x0610 && codepoint <= 0x061A) || /* علامات إضافية */
           (codepoint == 0x0670);                          /* ألف خنجرية */
}

SadString sad_string_strip_tashkeel(const SadString* str) {
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);

    /*
     * ننسخ كل الأحرف عدا علامات التشكيل.
     * الحجم الأقصى للنتيجة = الحجم الأصلي (إذا لا يوجد تشكيل).
     */
    char* buf = (char*)sad_alloc(len + 1);
    if (!buf) return sad_string_clone(str);

    size_t out_pos = 0;
    size_t i = 0;

    while (i < len) {
        uint32_t cp;
        int char_size = sad_utf8_decode(data + i, &cp);
        if (char_size == 0) char_size = 1;

        if (!is_arabic_diacritical(cp)) {
            /* ليس تشكيلاً — انسخه */
            memcpy(buf + out_pos, data + i, char_size);
            out_pos += char_size;
        }

        i += char_size;
    }

    buf[out_pos] = '\0';
    SadString result = sad_string_new(buf, out_pos);
    sad_free(buf);
    return result;
}

bool sad_string_is_arabic(const SadString* str) {
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);
    size_t i = 0;

    while (i < len) {
        uint32_t cp;
        int char_size = sad_utf8_decode(data + i, &cp);
        if (char_size == 0) char_size = 1;

        /* تحقق: هل هذا حرف عربي؟ */
        if ((cp >= 0x0600 && cp <= 0x06FF) || /* العربية الأساسية */
            (cp >= 0x0750 && cp <= 0x077F) || /* العربية الموسّعة أ */
            (cp >= 0x08A0 && cp <= 0x08FF) || /* العربية الموسّعة ب */
            (cp >= 0xFB50 && cp <= 0xFDFF) || /* أشكال العربية أ */
            (cp >= 0xFE70 && cp <= 0xFEFF)) { /* أشكال العربية ب */
            return true; /* يحتوي حرفاً عربياً واحداً على الأقل */
        }

        i += char_size;
    }

    return false;
}

SadString sad_string_reverse(const SadString* str) {
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);

    if (len == 0) return sad_string_empty();

    /*
     * عكس يونيكود — نعكس الأحرف (code points) وليس البايتات.
     * "مرحبا" → "ابحرم" (وليس بايتات مقلوبة!)
     *
     * الخطوات:
     * ١. اجمع كل الأحرف ومواقعها
     * ٢. انسخها بترتيب معكوس
     */
    char* buf = (char*)sad_alloc(len + 1);
    if (!buf) return sad_string_clone(str);

    /* أولاً: اجمع مواقع بداية كل حرف */
    size_t char_starts[4096]; /* حد أقصى 4096 حرف — كافٍ لسلاسل المحادثة */
    size_t char_sizes[4096];
    size_t char_count = 0;
    size_t i = 0;

    while (i < len && char_count < 4096) {
        char_starts[char_count] = i;
        int cs = sad_utf8_char_size((uint8_t)data[i]);
        if (cs == 0) cs = 1;
        char_sizes[char_count] = cs;
        i += cs;
        char_count++;
    }

    /* ثانياً: انسخها بترتيب معكوس */
    size_t out_pos = 0;
    for (size_t j = char_count; j > 0; j--) {
        size_t idx = j - 1;
        memcpy(buf + out_pos, data + char_starts[idx], char_sizes[idx]);
        out_pos += char_sizes[idx];
    }
    buf[out_pos] = '\0';

    SadString result = sad_string_new(buf, out_pos);
    sad_free(buf);
    return result;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٨ — عمليات شائعة أخرى
 * ═══════════════════════════════════════════════════════════════════════════ */

SadString sad_string_trim(const SadString* str) {
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);

    /* إيجاد بداية النص (بعد المسافات) */
    size_t start = 0;
    while (start < len && (data[start] == ' '  || data[start] == '\t' ||
                            data[start] == '\n' || data[start] == '\r')) {
        start++;
    }

    /* إيجاد نهاية النص (قبل المسافات) */
    size_t end = len;
    while (end > start && (data[end - 1] == ' '  || data[end - 1] == '\t' ||
                            data[end - 1] == '\n' || data[end - 1] == '\r')) {
        end--;
    }

    if (start == 0 && end == len) {
        /* لا مسافات — أعد نسخة */
        return sad_string_copy(str);
    }

    return sad_string_new(data + start, end - start);
}

bool sad_string_split(const SadString* str, const char* delimiter,
                      size_t delim_len, SadString** out_parts, size_t* out_count) {
    if (!str || !delimiter || !out_parts || !out_count) return false;

    const char* data = sad_string_data(str);
    size_t data_len = sad_string_length(str);

    /* أولاً: عدّ الأجزاء */
    size_t count = 1;
    for (size_t i = 0; i <= data_len - delim_len; i++) {
        if (memcmp(data + i, delimiter, delim_len) == 0) {
            count++;
            i += delim_len - 1;
        }
    }

    /* تخصيص مصفوفة النتيجة */
    SadString* parts = (SadString*)sad_alloc(count * sizeof(SadString));
    if (!parts) return false;

    /* ثانياً: تقسيم النص */
    size_t part_idx = 0;
    size_t seg_start = 0;

    for (size_t i = 0; i <= data_len; i++) {
        bool is_delim = false;
        if (i <= data_len - delim_len) {
            is_delim = (memcmp(data + i, delimiter, delim_len) == 0);
        }

        if (is_delim || i == data_len) {
            parts[part_idx] = sad_string_new(data + seg_start, i - seg_start);
            part_idx++;
            if (is_delim) {
                i += delim_len - 1;
                seg_start = i + 1;
            }
        }
    }

    *out_parts = parts;
    *out_count = count;
    return true;
}

SadString sad_string_replace(const SadString* str,
                              const char* old_text, size_t old_len,
                              const char* new_text, size_t new_len) {
    const char* data = sad_string_data(str);
    size_t data_len = sad_string_length(str);

    if (old_len == 0 || old_len > data_len) {
        return sad_string_copy(str);
    }

    /* حساب الحجم الجديد */
    size_t count = 0;
    for (size_t i = 0; i <= data_len - old_len; i++) {
        if (memcmp(data + i, old_text, old_len) == 0) {
            count++;
            i += old_len - 1;
        }
    }

    if (count == 0) return sad_string_copy(str);

    size_t new_size = data_len + count * ((int64_t)new_len - (int64_t)old_len);
    char* buf = (char*)sad_alloc(new_size + 1);
    if (!buf) return sad_string_copy(str);

    size_t out_pos = 0;
    size_t i = 0;

    while (i < data_len) {
        if (i <= data_len - old_len && memcmp(data + i, old_text, old_len) == 0) {
            memcpy(buf + out_pos, new_text, new_len);
            out_pos += new_len;
            i += old_len;
        } else {
            buf[out_pos++] = data[i++];
        }
    }
    buf[out_pos] = '\0';

    SadString result = sad_string_new(buf, out_pos);
    sad_free(buf);
    return result;
}

SadString sad_string_to_upper(const SadString* str) {
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);

    SadString result = sad_string_clone(str);
    /* نحصل على مؤشر قابل للتعديل */
    char* result_data;
    if (result.flags & SAD_STR_FLAG_SSO) {
        result_data = result.sso.data;
    } else {
        result_data = result.heap.data;
    }

    /* تحويل ASCII فقط (a-z → A-Z) — العربية لا تتأثر */
    for (size_t i = 0; i < len; i++) {
        if (result_data[i] >= 'a' && result_data[i] <= 'z') {
            result_data[i] -= 32;
        }
    }

    return result;
}

SadString sad_string_to_lower(const SadString* str) {
    const char* data = sad_string_data(str);
    size_t len = sad_string_length(str);

    SadString result = sad_string_clone(str);
    char* result_data;
    if (result.flags & SAD_STR_FLAG_SSO) {
        result_data = result.sso.data;
    } else {
        result_data = result.heap.data;
    }

    for (size_t i = 0; i < len; i++) {
        if (result_data[i] >= 'A' && result_data[i] <= 'Z') {
            result_data[i] += 32;
        }
    }

    return result;
}

SadString sad_string_format(const char* format, ...) {
    if (!format) return sad_string_empty();

    va_list args1, args2;
    va_start(args1, format);
    va_copy(args2, args1);

    /* أولاً: حساب الطول المطلوب */
    int len = vsnprintf(NULL, 0, format, args1);
    va_end(args1);

    if (len < 0) {
        va_end(args2);
        return sad_string_empty();
    }

    /* ثانياً: تنسيق النص */
    char* buf = (char*)sad_alloc((size_t)len + 1);
    if (!buf) {
        va_end(args2);
        return sad_string_empty();
    }

    vsnprintf(buf, (size_t)len + 1, format, args2);
    va_end(args2);

    SadString result = sad_string_new(buf, (size_t)len);
    sad_free(buf);
    return result;
}
