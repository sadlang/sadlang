// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: arabic_search.h
 * الوصف: محرك البحث العربي لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * محرك بحث متخصص يدعم:
 *
 *   ╔═══════════════════════════════════════════════════════════════════════╗
 *   ║  الميزة                     │  الوصف                              ║
 *   ╠═══════════════════════════════════════════════════════════════════════╣
 *   ║  إزالة التشكيل              │  فَتْحَة → فتحة                      ║
 *   ║  استخراج الجذر              │  "كاتب" → "ك ت ب"                   ║
 *   ║  البحث بالمرادفات           │  "حاسوب" = "كمبيوتر" = "حاسب"      ║
 *   ║  التطبيع                    │  إ/أ/آ → ا ، ة → ه                  ║
 *   ║  الترجمة الثنائية           │  "شبكة" ↔ "network"                 ║
 *   ║  البحث الضبابي              │  تصحيح الأخطاء الإملائية            ║
 *   ║  الترتيب الذكي              │  مطابقة + شعبية + جودة              ║
 *   ╚═══════════════════════════════════════════════════════════════════════╝
 *
 * خوارزمية البحث:
 *   ١. تطبيع نص البحث (إزالة تشكيل، توحيد أشكال الحروف)
 *   ٢. استخراج الجذر العربي (إن أمكن)
 *   ٣. البحث عن مرادفات ومقابلات إنجليزية
 *   ٤. البحث في الفهرس بجميع المصطلحات
 *   ٥. حساب درجة الصلة
 *   ٦. ترتيب النتائج وإرجاعها
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "../storage/database.h"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <algorithm>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// نتيجة البحث العربي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief نتيجة بحث واحدة مع درجة الصلة
 */
struct ArabicSearchResult {
    int64_t package_id = 0;         // معرف الحزمة
    std::string name;               // اسم الحزمة (لاتيني)
    std::string arabic_name;        // اسم الحزمة (عربي)
    std::string description;        // وصف إنجليزي
    std::string description_ar;     // وصف عربي
    std::string latest_version;     // آخر إصدار
    int64_t downloads = 0;          // عدد التنزيلات

    double relevance_score = 0.0;   // درجة الصلة (0.0 - 1.0)

    /// أسباب المطابقة (للتصحيح والعرض)
    std::vector<std::string> match_reasons;
};

// ═══════════════════════════════════════════════════════════════════════════════
// خريطة الجذور العربية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief جدول المرادفات والمقابلات
 *
 * يربط كل مصطلح عربي بمرادفاته ومقابلاته الإنجليزية.
 */
struct SynonymEntry {
    std::string arabic_term;                  // المصطلح العربي
    std::string arabic_root;                  // الجذر (ثلاثي أو رباعي)
    std::vector<std::string> arabic_synonyms; // مرادفات عربية
    std::vector<std::string> english_terms;   // مقابلات إنجليزية
};

// ═══════════════════════════════════════════════════════════════════════════════
// محرك البحث العربي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ArabicSearchEngine
 * @brief محرك بحث متقدم يدعم العربية والإنجليزية
 *
 * التصميم:
 *   يستخدم فهرساً في الذاكرة مبنياً من قاعدة البيانات.
 *   عند البحث، ينشئ مصطلحات بحث متعددة (أصلي + مطبَّع + جذر + مرادفات)
 *   ثم يبحث في الفهرس بكل منها ويجمع النتائج مع ترتيب ذكي.
 */
class ArabicSearchEngine {
public:
    /**
     * @brief المُنشئ
     * @param db مؤشر على قاعدة البيانات
     */
    explicit ArabicSearchEngine(std::shared_ptr<Database> db);

    /**
     * @brief تهيئة المحرك (بناء الفهرس والقواميس)
     */
    bool initialize();

    // ─────────────────────────────────────────────────────────────────────
    // البحث
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief البحث عن حزم
     * @param query نص البحث (عربي أو إنجليزي)
     * @param max_results أقصى عدد نتائج
     * @return قائمة نتائج مرتبة حسب الصلة
     */
    std::vector<ArabicSearchResult> search(const std::string& query,
                                            int max_results = 50);

    // ─────────────────────────────────────────────────────────────────────
    // الفهرسة
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief فهرسة حزمة جديدة
     * @param package_id معرف الحزمة
     * @param name الاسم اللاتيني
     * @param arabic_name الاسم العربي
     * @param description الوصف الإنجليزي
     * @param description_ar الوصف العربي
     */
    void index_package(int64_t package_id,
                       const std::string& name,
                       const std::string& arabic_name,
                       const std::string& description,
                       const std::string& description_ar);

    /**
     * @brief حذف حزمة من الفهرس
     */
    void remove_package(int64_t package_id);

    /**
     * @brief إعادة بناء الفهرس بالكامل
     */
    void rebuild_index();

    // ─────────────────────────────────────────────────────────────────────
    // المعالجة اللغوية
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief إزالة التشكيل من نص عربي
     *
     * يزيل: الفتحة، الكسرة، الضمة، السكون، الشدة، التنوين
     * مدى يونيكود: U+0610 - U+065F
     */
    static std::string remove_diacritics(const std::string& text);

    /**
     * @brief تطبيع النص العربي
     *
     * يوحد أشكال الحروف:
     *   - أ/إ/آ → ا
     *   - ة → ه
     *   - ى → ي
     *   - إزالة التشكيل
     *   - تحويل اللاتيني إلى حروف صغيرة
     */
    static std::string normalize(const std::string& text);

    /**
     * @brief استخراج الجذر العربي (الثلاثي/الرباعي)
     *
     * يستخدم خوارزمية تجريد بسيطة:
     *   ١. إزالة السوابق: ال، و، ب، ك، ل، ف، س
     *   ٢. إزالة اللواحق: ون، ين، ات، ة، ي، ها
     *   ٣. المتبقي يُعتبر جذراً تقريبياً
     */
    static std::string extract_root(const std::string& word);

    /**
     * @brief التحقق مما إذا كان النص يحتوي على أحرف عربية
     */
    static bool is_arabic(const std::string& text);

    /**
     * @brief الحصول على مرادفات مصطلح
     */
    std::vector<std::string> get_synonyms(const std::string& term);

    /**
     * @brief الحصول على المقابل الإنجليزي لمصطلح عربي
     */
    std::vector<std::string> get_english_equivalents(const std::string& arabic_term);

    /**
     * @brief الحصول على المقابل العربي لمصطلح إنجليزي
     */
    std::vector<std::string> get_arabic_equivalents(const std::string& english_term);

private:
    std::shared_ptr<Database> db_;

    // ─────────────────────────────────────────────────────────────────────
    // الفهرس في الذاكرة
    // ─────────────────────────────────────────────────────────────────────

    /// معلومات حزمة مفهرسة
    struct IndexedPackage {
        int64_t id = 0;
        std::string name;
        std::string arabic_name;
        std::string name_normalized;
        std::string arabic_name_normalized;
        std::string description;
        std::string description_ar;
        std::string description_normalized;
        std::string description_ar_normalized;
        int64_t downloads = 0;
        std::string latest_version;

        /// الكلمات المفتاحية المستخرجة (مطبَّعة)
        std::set<std::string> keywords;

        /// الجذور العربية المستخرجة
        std::set<std::string> roots;
    };

    /// الفهرس الرئيسي: معرف_الحزمة → بيانات مفهرسة
    std::map<int64_t, IndexedPackage> index_;

    /// فهرس عكسي: كلمة_مطبَّعة → مجموعة معرفات الحزم
    std::map<std::string, std::set<int64_t>> inverted_index_;

    /// فهرس الجذور: جذر → مجموعة معرفات الحزم
    std::map<std::string, std::set<int64_t>> root_index_;

    // ─────────────────────────────────────────────────────────────────────
    // القواميس
    // ─────────────────────────────────────────────────────────────────────

    /// قاموس المرادفات: مصطلح → مرادفات
    std::map<std::string, std::vector<std::string>> synonyms_;

    /// قاموس عربي-إنجليزي: مصطلح_عربي → مقابلات_إنجليزية
    std::map<std::string, std::vector<std::string>> ar_en_dict_;

    /// قاموس إنجليزي-عربي: مصطلح_إنجليزي → مقابلات_عربية
    std::map<std::string, std::vector<std::string>> en_ar_dict_;

    // ─────────────────────────────────────────────────────────────────────
    // دوال داخلية
    // ─────────────────────────────────────────────────────────────────────

    /// تقسيم نص إلى كلمات
    static std::vector<std::string> tokenize(const std::string& text);

    /// بناء القواميس المدمجة
    void build_dictionaries();

    /// حساب درجة الصلة بين استعلام وحزمة مفهرسة
    double compute_relevance(const std::vector<std::string>& query_terms,
                              const IndexedPackage& pkg);

    /// فهرسة حزمة واحدة في الفهرس العكسي
    void add_to_inverted_index(int64_t package_id, const std::set<std::string>& keywords);
};

} // namespace registry
} // namespace sad
