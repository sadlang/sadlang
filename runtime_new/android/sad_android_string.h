/**
 * @file sad_android_string.h
 * @brief (AR) نظام السلاسل النصية لأندرويد — UTF-8 مع تحسين السلاسل القصيرة (SSO)
 * @brief (EN) Android string system — UTF-8 with Small String Optimization (SSO)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام السلاسل النصية — لغة ص على أندرويد
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * نظام السلاسل النصية مُصمم خصيصاً لدعم اللغة العربية بكفاءة عالية.
 * يستخدم ترميز UTF-8 داخلياً (وهو الترميز القياسي في أندرويد ولينكس).
 *
 * ───────────────────────────────────────────────────────────────────────────
 *                  تحسين السلاسل القصيرة (SSO)
 * ───────────────────────────────────────────────────────────────────────────
 *
 * معظم السلاسل في التطبيقات قصيرة (أسماء مستخدمين، أزرار، رسائل خطأ).
 * بدلاً من تخصيص ذاكرة heap لكل سلسلة، نُخزّن السلاسل القصيرة
 * داخل بنية SadString نفسها:
 *
 *  سلسلة قصيرة (≤ 22 بايت UTF-8):
 *  ┌────────────────────────────────────────────┐
 *  │ sso_data[23]              │ sso_len │flags │
 *  │ "مرحباً" (12 بايت UTF-8) │ 0x0C    │ 0x01 │
 *  └────────────────────────────────────────────┘
 *  ← لا تخصيص heap! كل شيء في المكدس (stack) →
 *
 *  سلسلة طويلة (> 22 بايت):
 *  ┌──────────────────────────────────────────────┐
 *  │ data (مؤشر) │ length  │ capacity │ flags     │
 *  │  → heap     │ الطول   │ السعة   │ 0x00       │
 *  └──────────────────────────────────────────────┘
 *  ← البيانات على الـ heap →
 *
 * ───────────────────────────────────────────────────────────────────────────
 *                  ملاحظات حول UTF-8 والعربية
 * ───────────────────────────────────────────────────────────────────────────
 *
 * - الحروف العربية تأخذ 2 بايت في UTF-8 (مدى U+0600-U+06FF)
 * - التشكيل (الفتحة، الضمة، الكسرة) يأخذ 2 بايت إضافية لكل حرف
 * - "مرحبا" = 10 بايت في UTF-8 (5 حروف × 2 بايت)
 * - "مَرْحَبًا" مع التشكيل = ~20 بايت
 * - الإيموجي تأخذ 4 بايت (مدى U+1F000+)
 *
 * لذلك:
 * - sad_string_length(): الطول بالبايت (سريع — O(1))
 * - sad_string_char_count(): عدد الأحرف (أبطأ — O(n) يجب تجاوز UTF-8)
 *
 * ───────────────────────────────────────────────────────────────────────────
 *                  العدّ المرجعي والنسخ عند الكتابة (COW)
 * ───────────────────────────────────────────────────────────────────────────
 *
 * السلاسل الطويلة تستخدم العدّ المرجعي:
 * - عند النسخ: لا تُنسخ البيانات — فقط يزيد عدد المراجع
 * - عند التعديل: إذا عدد المراجع > 1 → أنشئ نسخة خاصة (COW)
 *
 * هذا يعني أن تمرير سلسلة لدالة = عملية O(1) ولا نسخ!
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 * @date مارس 2026
 */

#ifndef SAD_ANDROID_STRING_H
#define SAD_ANDROID_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sad_android_memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *                         ثوابت السلاسل النصية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) الحد الأقصى لسلسلة SSO — 22 بايت (+ 1 بايت للطول + 1 للأعلام)
 * @brief (EN) Maximum SSO string — 22 bytes (+ 1 byte for length + 1 for flags)
 *
 * 22 بايت تكفي لـ:
 * - ~11 حرف عربي (2 بايت لكل حرف)
 * - ~22 حرف لاتيني (1 بايت لكل حرف)
 * - معظم أسماء المتغيرات والأزرار والعناوين القصيرة
 *
 * تم اختيار 22 ليصبح حجم SadString = 32 بايت بالضبط (محاذاة cache line).
 */
#define SAD_SSO_MAX_SIZE  22

/**
 * @brief (AR) أعلام السلاسل النصية
 * @brief (EN) String flags
 */
#define SAD_STR_FLAG_SSO       0x01  /* سلسلة قصيرة (في المكدس) */
#define SAD_STR_FLAG_OWNED     0x02  /* تملك بياناتها (يجب تحريرها) */
#define SAD_STR_FLAG_INTERNED  0x04  /* سلسلة مُضمّنة (لا تُحرّر) */
#define SAD_STR_FLAG_UTF8_VALID 0x08 /* تم التحقق من صحة UTF-8 */

/* ═══════════════════════════════════════════════════════════════════════════
 *                         بنية SadString
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) السلسلة النصية — النوع الأساسي للنصوص في لغة ص
 * @brief (EN) String — the core text type in Sad language
 *
 * الحجم: 32 بايت بالضبط (مُحاذاة لخط الـ cache).
 *
 * يحتوي على union بين وضعين:
 * - وضع SSO: البيانات مُخزّنة داخل البنية نفسها (22 بايت + طول + أعلام)
 * - وضع Heap: مؤشر لبيانات على الـ heap + طول + سعة + أعلام
 *
 * التمييز بين الوضعين عبر flags: إذا (flags & SAD_STR_FLAG_SSO) → SSO
 */
typedef struct SadString {
    union {
        /* ═══ وضع السلاسل الطويلة (Heap) ═══ */
        struct {
            char*    data;      /* مؤشر لبيانات UTF-8 على الـ heap (8 بايت) */
            size_t   length;    /* الطول بالبايت — لا يشمل '\0' (8 بايت) */
            size_t   capacity;  /* السعة المُخصصة بالبايت (8 بايت) */
        } heap;

        /* ═══ وضع السلاسل القصيرة (SSO) ═══ */
        struct {
            char     data[SAD_SSO_MAX_SIZE + 1]; /* البيانات + '\0' (23 بايت) */
            uint8_t  length;                       /* الطول بالبايت (1 بايت) */
            /* 8 بايت متبقية — مستخدمة من padding */
        } sso;
    };
    /*
     * الأعلام في آخر 8 بايت (مشتركة بين الوضعين):
     * يجب أن تكون في نفس الموقع لكلا الوضعين.
     */
    uint8_t flags;             /* أعلام السلسلة (1 بايت) */
    uint8_t _padding[7];      /* حشو للوصول إلى 32 بايت */
} SadString;

/*
 * التحقق من حجم البنية في وقت الترجمة:
 * يجب أن يكون 32 بايت بالضبط.
 */
_Static_assert(sizeof(SadString) == 32 || sizeof(SadString) <= 40,
               "SadString must fit in 32-40 bytes for cache efficiency");

/* ═══════════════════════════════════════════════════════════════════════════
 *                    إنشاء وتدمير السلاسل النصية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إنشاء سلسلة نصية من مصفوفة char مع طول محدد
 * @brief (EN) Create string from char array with specified length
 *
 * يختار تلقائياً بين SSO و heap حسب الطول:
 * - ≤ 22 بايت → SSO (لا تخصيص heap)
 * - > 22 بايت → heap (تخصيص عبر sad_rc_alloc)
 *
 * @param data مؤشر لبيانات UTF-8 (لا يجب أن تنتهي بـ '\0')
 * @param length الطول بالبايت
 * @return سلسلة جديدة
 *
 * مثال:
 * @code
 * SadString اسم = sad_string_new("أحمد", 8);    // 4 حروف × 2 بايت = 8 → SSO
 * SadString رسالة = sad_string_new(نص_طويل, 500); // → heap
 * @endcode
 */
SadString sad_string_new(const char* data, size_t length);

/**
 * @brief (AR) إنشاء سلسلة من نص C منتهي بصفر (null-terminated)
 * @brief (EN) Create string from null-terminated C string
 *
 * @param cstr نص C منتهي بـ '\0'
 * @return سلسلة جديدة
 *
 * مثال:
 * @code
 * SadString مرحبا = sad_string_from_cstr("مرحباً بالعالم!");
 * @endcode
 */
SadString sad_string_from_cstr(const char* cstr);

/**
 * @brief (AR) إنشاء سلسلة فارغة
 * @brief (EN) Create an empty string
 */
SadString sad_string_empty(void);

/**
 * @brief (AR) تدمير سلسلة (تحرير ذاكرة heap إن وُجدت)
 * @brief (EN) Destroy string (free heap memory if any)
 *
 * - سلسلة SSO: لا شيء يُحرّر (البيانات في المكدس)
 * - سلسلة heap: يُنقص عدد المراجع ← يُحرّر إذا وصل لصفر
 *
 * @param str مؤشر للسلسلة
 */
void sad_string_free(SadString* str);

/**
 * @brief (AR) نسخ سلسلة (عدد مراجع أو نسخ فعلي حسب نوعها)
 * @brief (EN) Copy string (ref-counted or actual copy depending on type)
 *
 * - SSO: نسخ سطحي (memcpy — البيانات داخل البنية)
 * - Heap: زيادة عدد المراجع فقط (لا نسخ بيانات!)
 */
SadString sad_string_copy(const SadString* str);

/**
 * @brief (AR) إنشاء نسخة عميقة (دائماً ينسخ البيانات)
 * @brief (EN) Create a deep copy (always copies data)
 */
SadString sad_string_clone(const SadString* str);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    الوصول للبيانات
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) الحصول على مؤشر للبيانات الخام (UTF-8)
 * @brief (EN) Get raw data pointer (UTF-8)
 *
 * @param str السلسلة
 * @return مؤشر ثابت لبيانات UTF-8 (منتهية بـ '\0')
 *
 * @warning المؤشر المُرجَع صالح فقط ما دامت السلسلة حية!
 */
const char* sad_string_data(const SadString* str);

/**
 * @brief (AR) الطول بالبايت — O(1) فوري
 * @brief (EN) Length in bytes — O(1) instant
 *
 * هذا هو حجم البيانات الخام، وليس عدد الأحرف.
 * "مرحبا" = 10 بايت (5 حروف عربية × 2 بايت).
 */
size_t sad_string_length(const SadString* str);

/**
 * @brief (AR) عدد أحرف يونيكود — O(n) يتطلب تجاوز UTF-8
 * @brief (EN) Unicode character count — O(n) requires UTF-8 traversal
 *
 * هذا يحسب عدد الأحرف الفعلية (code points)، وليس البايتات.
 * "مرحبا" = 5 أحرف.
 *
 * @note أبطأ من sad_string_length — استخدمه فقط عند الحاجة!
 */
size_t sad_string_char_count(const SadString* str);

/**
 * @brief (AR) هل السلسلة فارغة؟
 * @brief (EN) Is the string empty?
 */
bool sad_string_is_empty(const SadString* str);

/**
 * @brief (AR) هل السلسلة تستخدم SSO؟ (للتشخيص)
 * @brief (EN) Is the string using SSO? (for diagnostics)
 */
bool sad_string_is_sso(const SadString* str);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    عمليات التعديل
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) دمج (ربط) سلسلتين — إنشاء سلسلة جديدة
 * @brief (EN) Concatenate two strings — creates a new string
 *
 * @param a السلسلة الأولى
 * @param b السلسلة الثانية
 * @return سلسلة جديدة = a + b
 *
 * مثال:
 * @code
 * SadString اسم = sad_string_from_cstr("أحمد");
 * SadString تحية = sad_string_from_cstr("مرحباً ");
 * SadString نتيجة = sad_string_concat(&تحية, &اسم);
 * // نتيجة = "مرحباً أحمد"
 * @endcode
 */
SadString sad_string_concat(const SadString* a, const SadString* b);

/**
 * @brief (AR) إلحاق نص C بنهاية السلسلة (تعديل في المكان)
 * @brief (EN) Append C string to end (modify in-place)
 *
 * @param str السلسلة المراد الإلحاق بها
 * @param suffix النص المراد إلحاقه
 * @param suffix_len طول النص المُلحق بالبايت
 */
void sad_string_append(SadString* str, const char* suffix, size_t suffix_len);

/**
 * @brief (AR) استخراج جزء من السلسلة (بالبايت)
 * @brief (EN) Extract substring (by bytes)
 *
 * @param str السلسلة الأصلية
 * @param start موقع البداية (بالبايت، يبدأ من 0)
 * @param length الطول المطلوب (بالبايت)
 * @return سلسلة جديدة تحتوي الجزء المطلوب
 *
 * @warning المواقع بالبايت وليس بالأحرف! استخدم sad_string_substr_chars
 *          للعمل بالأحرف العربية.
 */
SadString sad_string_substr(const SadString* str, size_t start, size_t length);

/**
 * @brief (AR) استخراج جزء من السلسلة (بأحرف يونيكود)
 * @brief (EN) Extract substring (by Unicode characters)
 *
 * @param str السلسلة الأصلية
 * @param char_start موقع البداية (بالأحرف، يبدأ من 0)
 * @param char_count عدد الأحرف المطلوبة
 * @return سلسلة جديدة
 *
 * مثال:
 * @code
 * SadString نص = sad_string_from_cstr("مرحباً بالعالم");
 * SadString جزء = sad_string_substr_chars(&نص, 0, 6);
 * // جزء = "مرحباً"
 * @endcode
 */
SadString sad_string_substr_chars(const SadString* str, size_t char_start, size_t char_count);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    عمليات البحث
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) البحث عن نص فرعي — يُرجع موقع أول تطابق (بالبايت)
 * @brief (EN) Find substring — returns position of first match (in bytes)
 *
 * @param str السلسلة الأصلية
 * @param needle النص المبحوث عنه
 * @param needle_len طول النص المبحوث عنه
 * @return موقع أول تطابق (بالبايت)، أو (size_t)-1 إذا لم يُوجد
 */
size_t sad_string_find(const SadString* str, const char* needle, size_t needle_len);

/**
 * @brief (AR) هل تحتوي السلسلة على النص المحدد؟
 * @brief (EN) Does the string contain the specified text?
 */
bool sad_string_contains(const SadString* str, const char* needle, size_t needle_len);

/**
 * @brief (AR) هل تبدأ السلسلة بالنص المحدد؟
 * @brief (EN) Does the string start with the specified text?
 */
bool sad_string_starts_with(const SadString* str, const char* prefix, size_t prefix_len);

/**
 * @brief (AR) هل تنتهي السلسلة بالنص المحدد؟
 * @brief (EN) Does the string end with the specified text?
 */
bool sad_string_ends_with(const SadString* str, const char* suffix, size_t suffix_len);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    المقارنة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) مقارنة سلسلتين — هل متساويتان؟
 * @brief (EN) Compare two strings — are they equal?
 *
 * يقارن البايتات مباشرة (لا يأخذ بعين الاعتبار تطبيع يونيكود).
 * "محمد" == "محمد" → صحيح
 * "محمد" == "مُحَمّد" → خطأ (تشكيل مختلف!)
 */
bool sad_string_equals(const SadString* a, const SadString* b);

/**
 * @brief (AR) مقارنة أبجدية (lexicographic) — للترتيب
 * @brief (EN) Lexicographic comparison — for sorting
 *
 * @return < 0 إذا a < b، 0 إذا a == b، > 0 إذا a > b
 */
int sad_string_compare(const SadString* a, const SadString* b);

/**
 * @brief (AR) حساب قيمة التجزئة (hash) — لاستخدامها كمفتاح خريطة
 * @brief (EN) Compute hash value — for use as map key
 *
 * يستخدم خوارزمية FNV-1a (سريعة وتوزيع جيد).
 */
uint64_t sad_string_hash(const SadString* str);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    عمليات التحويل
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تحويل رقم إلى سلسلة
 * @brief (EN) Convert integer to string
 */
SadString sad_string_from_int(int64_t value);

/**
 * @brief (AR) تحويل عدد عشري إلى سلسلة
 * @brief (EN) Convert double to string
 */
SadString sad_string_from_double(double value);

/**
 * @brief (AR) تحويل قيمة منطقية إلى سلسلة ("صحيح" أو "خطأ")
 * @brief (EN) Convert boolean to string ("صحيح" or "خطأ")
 */
SadString sad_string_from_bool(bool value);

/**
 * @brief (AR) تحويل سلسلة إلى رقم
 * @brief (EN) Convert string to integer
 *
 * @param str السلسلة
 * @param out_value مخرج القيمة الرقمية
 * @return true إذا نجح التحويل، false إذا السلسلة ليست رقماً
 */
bool sad_string_to_int(const SadString* str, int64_t* out_value);

/**
 * @brief (AR) تحويل سلسلة إلى عدد عشري
 * @brief (EN) Convert string to double
 */
bool sad_string_to_double(const SadString* str, double* out_value);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    عمليات عربية خاصة
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إزالة التشكيل من النص العربي
 * @brief (EN) Remove Arabic diacritics (tashkeel) from text
 *
 * يزيل: الفتحة (َ)، الضمة (ُ)، الكسرة (ِ)، السكون (ْ)،
 *       الشدة (ّ)، التنوين (ً ٌ ٍ)
 *
 * "مُحَمَّد" → "محمد"
 */
SadString sad_string_strip_tashkeel(const SadString* str);

/**
 * @brief (AR) هل الحرف عربي؟
 * @brief (EN) Is the character Arabic?
 *
 * يتحقق من المدى U+0600 إلى U+06FF (العربية الأساسية)
 * + U+0750 إلى U+077F (العربية الموسّعة-أ)
 * + U+08A0 إلى U+08FF (العربية الموسّعة-ب)
 */
bool sad_string_is_arabic(const SadString* str);

/**
 * @brief (AR) عكس سلسلة مع مراعاة يونيكود
 * @brief (EN) Reverse string respecting Unicode
 *
 * يعكس الأحرف (code points) وليس البايتات.
 * "مرحبا" → "ابحرم"
 */
SadString sad_string_reverse(const SadString* str);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    عمليات شائعة أخرى
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) إزالة المسافات من البداية والنهاية (trim)
 * @brief (EN) Remove whitespace from beginning and end (trim)
 */
SadString sad_string_trim(const SadString* str);

/**
 * @brief (AR) تقسيم السلسلة إلى مصفوفة بفاصل محدد
 * @brief (EN) Split string into array by delimiter
 *
 * @param str السلسلة
 * @param delimiter الفاصل
 * @param delim_len طول الفاصل
 * @param out_parts مخرج — مصفوفة سلاسل (يجب تحريرها بعد الاستخدام)
 * @param out_count مخرج — عدد الأجزاء
 * @return true إذا نجحت العملية
 *
 * مثال:
 * @code
 * SadString نص = sad_string_from_cstr("أحمد,سارة,خالد");
 * SadString* أجزاء;
 * size_t عدد;
 * sad_string_split(&نص, ",", 1, &أجزاء, &عدد);
 * // أجزاء[0] = "أحمد", أجزاء[1] = "سارة", أجزاء[2] = "خالد"
 * @endcode
 */
bool sad_string_split(const SadString* str, const char* delimiter,
                      size_t delim_len, SadString** out_parts, size_t* out_count);

/**
 * @brief (AR) استبدال كل تكرارات نص فرعي بنص آخر
 * @brief (EN) Replace all occurrences of a substring with another
 *
 * @param str السلسلة الأصلية
 * @param old_text النص المراد استبداله
 * @param old_len طول النص القديم
 * @param new_text النص البديل
 * @param new_len طول النص البديل
 * @return سلسلة جديدة بالاستبدالات
 */
SadString sad_string_replace(const SadString* str,
                              const char* old_text, size_t old_len,
                              const char* new_text, size_t new_len);

/**
 * @brief (AR) تحويل إلى أحرف كبيرة (ASCII فقط — لا يؤثر على العربية)
 * @brief (EN) Convert to uppercase (ASCII only — doesn't affect Arabic)
 */
SadString sad_string_to_upper(const SadString* str);

/**
 * @brief (AR) تحويل إلى أحرف صغيرة (ASCII فقط — لا يؤثر على العربية)
 * @brief (EN) Convert to lowercase (ASCII only — doesn't affect Arabic)
 */
SadString sad_string_to_lower(const SadString* str);

/**
 * @brief (AR) تنسيق سلسلة (مثل sprintf لكن يُرجع SadString)
 * @brief (EN) Format string (like sprintf but returns SadString)
 *
 * @param format قالب التنسيق (مثل printf)
 * @param ... الوسائط
 * @return سلسلة مُنسّقة جديدة
 *
 * مثال:
 * @code
 * SadString رسالة = sad_string_format("مرحباً %s، عمرك %d سنة", "أحمد", 25);
 * @endcode
 */
SadString sad_string_format(const char* format, ...);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    أدوات UTF-8
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) حساب عدد بايتات حرف UTF-8 من أول بايت
 * @brief (EN) Calculate UTF-8 character byte count from first byte
 *
 * @param first_byte أول بايت في الحرف
 * @return عدد البايتات (1-4)، أو 0 إذا بايت غير صالح
 *
 * القاعدة:
 * - 0xxxxxxx → 1 بايت (ASCII)
 * - 110xxxxx → 2 بايت (عربي، لاتيني موسّع)
 * - 1110xxxx → 3 بايت (صيني، ياباني)
 * - 11110xxx → 4 بايت (إيموجي)
 */
int sad_utf8_char_size(uint8_t first_byte);

/**
 * @brief (AR) تحويل نقطة كود يونيكود إلى UTF-8
 * @brief (EN) Encode Unicode code point to UTF-8
 *
 * @param codepoint نقطة الكود (مثال: 0x0645 لحرف 'م')
 * @param out مُخزن مؤقت (4 بايت على الأقل)
 * @return عدد البايتات المكتوبة (1-4)
 */
int sad_utf8_encode(uint32_t codepoint, char* out);

/**
 * @brief (AR) فك ترميز حرف UTF-8 إلى نقطة كود
 * @brief (EN) Decode UTF-8 character to code point
 *
 * @param data مؤشر لبداية الحرف UTF-8
 * @param out_codepoint مخرج — نقطة الكود
 * @return عدد البايتات المقروءة (1-4)
 */
int sad_utf8_decode(const char* data, uint32_t* out_codepoint);

/**
 * @brief (AR) التحقق من صحة سلسلة UTF-8
 * @brief (EN) Validate UTF-8 string
 *
 * @param data البيانات
 * @param length الطول بالبايت
 * @return true إذا UTF-8 صالح
 */
bool sad_utf8_validate(const char* data, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_STRING_H */
