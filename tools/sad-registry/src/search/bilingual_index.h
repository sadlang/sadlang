// بسم الله الرحمن الرحيم
#pragma once
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: bilingual_index.h
 * الوصف: الفهرس ثنائي اللغة لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يعرّف هيكل الفهرس ثنائي اللغة الذي يربط بين المصطلحات
 * العربية والإنجليزية في فضاء واحد، مما يسمح بـ:
 *
 *   ١. البحث بالعربية وإيجاد حزم بأوصاف إنجليزية
 *   ٢. البحث بالإنجليزية وإيجاد حزم بأسماء عربية
 *   ٣. البحث المختلط (مزيج من اللغتين)
 *   ٤. الاقتراحات ثنائية اللغة (Bilingual Suggestions)
 *
 * الهندسة:
 *
 *   ┌──────────────────────────────────────────────┐
 *   │           الفهرس ثنائي اللغة                 │
 *   │                                              │
 *   │   ┌──────────┐     ┌──────────┐              │
 *   │   │ فهرس     │────▶│ فهرس    │              │
 *   │   │ عربي     │◀────│ إنجليزي │              │
 *   │   └────┬─────┘     └────┬─────┘              │
 *   │        │                │                    │
 *   │   ┌────▼────────────────▼─────┐              │
 *   │   │    خريطة الربط             │              │
 *   │   │  (BilingualMapping)        │              │
 *   │   └────────┬──────────────────┘              │
 *   │            │                                 │
 *   │   ┌────────▼──────────────────┐              │
 *   │   │     الحزم المُفهرسة       │              │
 *   │   └───────────────────────────┘              │
 *   └──────────────────────────────────────────────┘
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تعيين ثنائي اللغة (Bilingual Mapping)
 *
 * يربط مصطلحاً عربياً بمقابلاته الإنجليزية (أو العكس)
 * مع وزن يحدد قوة العلاقة.
 *
 * أمثلة:
 *   مصطلح عربي: "شبكة"
 *   مقابلات: [("network", 1.0), ("web", 0.8), ("net", 0.7)]
 */
struct BilingualMapping {
    std::string arabic_term;          // المصطلح العربي (مُطبَّع)
    std::string arabic_root;          // جذر المصطلح العربي
    std::vector<std::pair<std::string, double>> english_equivalents; // المقابلات + الوزن
    std::string category;             // التصنيف (شبكات، أمان، بيانات...)
    bool is_technical;                // هل هو مصطلح تقني
};

/**
 * @brief نتيجة اقتراح ثنائي اللغة
 *
 * عندما يبحث المستخدم بكلمة، نقترح كلمات ذات صلة
 * من اللغة الأخرى.
 */
struct BilingualSuggestion {
    std::string original;             // الكلمة الأصلية
    std::string suggestion;           // الاقتراح
    std::string language;             // لغة الاقتراح (ar/en)
    double confidence;                // درجة الثقة (0-1)
};

/**
 * @brief إحصائيات الفهرس
 */
struct BilingualIndexStats {
    size_t arabic_terms;              // عدد المصطلحات العربية
    size_t english_terms;             // عدد المصطلحات الإنجليزية
    size_t mappings;                  // عدد الربط بين اللغتين
    size_t indexed_packages;          // عدد الحزم المُفهرسة
    size_t categories;                // عدد التصنيفات
};

// ═══════════════════════════════════════════════════════════════════════════════
// الفهرس ثنائي اللغة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief الفهرس ثنائي اللغة
 *
 * هذا الصنف يبني فهرساً متكاملاً يربط بين اللغتين العربية والإنجليزية:
 *
 * الميزات:
 *   ● ربط المصطلحات مع أوزان تحدد قوة العلاقة
 *   ● تصنيف المصطلحات (شبكات/أمان/بيانات/... إلخ)
 *   ● اقتراحات ذكية من اللغة المقابلة
 *   ● تتبع الشعبية لتحسين الترتيب
 *
 * الاستخدام:
 *   BilingualIndex index;
 *   index.initialize();
 *   index.add_mapping("شبكة", {{"network", 1.0}, {"web", 0.8}}, "شبكات");
 *   auto suggestions = index.suggest("network"); // → ["شبكة"]
 */
class BilingualIndex {
public:
    // ─────────────────────────────────────────────────────────────────
    // التهيئة
    // ─────────────────────────────────────────────────────────────────

    /**
     * تهيئة الفهرس بالقواميس المدمجة
     * يُبنى قاموس أساسي من المصطلحات البرمجية الشائعة.
     */
    void initialize() {
        build_base_mappings();
    }

    // ─────────────────────────────────────────────────────────────────
    // إضافة وحذف
    // ─────────────────────────────────────────────────────────────────

    /**
     * إضافة ربط ثنائي اللغة
     * @param arabic المصطلح العربي
     * @param english_pairs أزواج (مقابل إنجليزي، وزن)
     * @param category التصنيف
     * @param technical هل مصطلح تقني
     */
    void add_mapping(
            const std::string& arabic,
            const std::vector<std::pair<std::string, double>>& english_pairs,
            const std::string& category = "",
            bool technical = true) {

        BilingualMapping mapping;
        mapping.arabic_term = arabic;
        mapping.english_equivalents = english_pairs;
        mapping.category = category;
        mapping.is_technical = technical;

        // إضافة للفهرس العربي
        ar_to_en_[arabic] = mapping;

        // إضافة للفهرس الإنجليزي العكسي
        for (const auto& [en, weight] : english_pairs) {
            en_to_ar_[en].push_back({arabic, weight});
        }

        // إضافة للتصنيف
        if (!category.empty()) {
            categories_[category].insert(arabic);
        }
    }

    /**
     * حذف ربط ثنائي اللغة
     */
    void remove_mapping(const std::string& arabic) {
        auto it = ar_to_en_.find(arabic);
        if (it == ar_to_en_.end()) return;

        // حذف من الفهرس العكسي
        for (const auto& [en, _] : it->second.english_equivalents) {
            auto en_it = en_to_ar_.find(en);
            if (en_it != en_to_ar_.end()) {
                auto& vec = en_it->second;
                vec.erase(
                    std::remove_if(vec.begin(), vec.end(),
                        [&](const std::pair<std::string, double>& p) {
                            return p.first == arabic;
                        }),
                    vec.end());
                if (vec.empty()) en_to_ar_.erase(en_it);
            }
        }

        // حذف من التصنيف
        if (!it->second.category.empty()) {
            auto cat_it = categories_.find(it->second.category);
            if (cat_it != categories_.end()) {
                cat_it->second.erase(arabic);
            }
        }

        ar_to_en_.erase(it);
    }

    // ─────────────────────────────────────────────────────────────────
    // الاستعلام
    // ─────────────────────────────────────────────────────────────────

    /**
     * الحصول على المقابلات الإنجليزية لمصطلح عربي
     */
    std::vector<std::pair<std::string, double>>
    get_english(const std::string& arabic) const {
        auto it = ar_to_en_.find(arabic);
        if (it != ar_to_en_.end()) {
            return it->second.english_equivalents;
        }
        return {};
    }

    /**
     * الحصول على المقابلات العربية لمصطلح إنجليزي
     */
    std::vector<std::pair<std::string, double>>
    get_arabic(const std::string& english) const {
        auto it = en_to_ar_.find(english);
        if (it != en_to_ar_.end()) {
            return it->second;
        }
        return {};
    }

    /**
     * اقتراح مصطلحات من اللغة الأخرى
     *
     * إذا كان الإدخال عربياً → يقترح إنجليزية
     * إذا كان لاتينياً → يقترح عربية
     */
    std::vector<BilingualSuggestion>
    suggest(const std::string& term, int max_suggestions = 5) const {
        std::vector<BilingualSuggestion> suggestions;

        // محاولة البحث العربي أولاً
        auto ar_it = ar_to_en_.find(term);
        if (ar_it != ar_to_en_.end()) {
            for (const auto& [en, weight] : ar_it->second.english_equivalents) {
                suggestions.push_back({
                    term, en, "en", weight
                });
                if ((int)suggestions.size() >= max_suggestions) break;
            }
            return suggestions;
        }

        // محاولة البحث الإنجليزي
        auto en_it = en_to_ar_.find(term);
        if (en_it != en_to_ar_.end()) {
            for (const auto& [ar, weight] : en_it->second) {
                suggestions.push_back({
                    term, ar, "ar", weight
                });
                if ((int)suggestions.size() >= max_suggestions) break;
            }
            return suggestions;
        }

        // بحث جزئي - كلمات تحتوي على المصطلح
        for (const auto& [ar, mapping] : ar_to_en_) {
            if (ar.find(term) != std::string::npos) {
                for (const auto& [en, weight] : mapping.english_equivalents) {
                    suggestions.push_back({
                        term, en, "en", weight * 0.5
                    });
                    if ((int)suggestions.size() >= max_suggestions) return suggestions;
                }
            }
        }

        return suggestions;
    }

    /**
     * الحصول على مصطلحات في تصنيف معين
     */
    std::set<std::string> get_category_terms(const std::string& category) const {
        auto it = categories_.find(category);
        if (it != categories_.end()) return it->second;
        return {};
    }

    /**
     * الحصول على جميع التصنيفات
     */
    std::vector<std::string> get_categories() const {
        std::vector<std::string> cats;
        for (const auto& [cat, _] : categories_) {
            cats.push_back(cat);
        }
        return cats;
    }

    /**
     * إحصائيات الفهرس
     */
    BilingualIndexStats get_stats() const {
        BilingualIndexStats stats;
        stats.arabic_terms = ar_to_en_.size();
        stats.english_terms = en_to_ar_.size();
        stats.mappings = 0;
        for (const auto& [_, mapping] : ar_to_en_) {
            stats.mappings += mapping.english_equivalents.size();
        }
        stats.categories = categories_.size();
        stats.indexed_packages = 0;
        return stats;
    }

private:
    // ─────────────────────────────────────────────────────────────────
    // البيانات الداخلية
    // ─────────────────────────────────────────────────────────────────

    /// فهرس عربي → إنجليزي
    std::map<std::string, BilingualMapping> ar_to_en_;

    /// فهرس إنجليزي → عربي (عكسي)
    /// القيمة: أزواج (مصطلح عربي، وزن)
    std::map<std::string, std::vector<std::pair<std::string, double>>> en_to_ar_;

    /// فهرس التصنيفات
    std::map<std::string, std::set<std::string>> categories_;

    // ─────────────────────────────────────────────────────────────────
    // بناء القواميس المدمجة
    // ─────────────────────────────────────────────────────────────────

    /**
     * بناء القواميس الأساسية للمصطلحات البرمجية
     *
     * التصنيفات:
     *   ● شبكات  = مصطلحات الشبكات والويب
     *   ● بيانات = مصطلحات قواعد البيانات والتخزين
     *   ● أمان   = مصطلحات الأمان والتشفير
     *   ● واجهات = مصطلحات واجهات المستخدم
     *   ● نظام   = مصطلحات النظام والملفات
     *   ● أدوات  = مصطلحات الأدوات والمكتبات
     *   ● نصوص   = مصطلحات معالجة النصوص
     *   ● حساب   = مصطلحات الرياضيات والحساب
     */
    void build_base_mappings() {
        // ── الشبكات ──
        add_mapping("\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA9", // شبكة
                    {{"network", 1.0}, {"web", 0.8}, {"net", 0.7}, {"internet", 0.6}},
                    "\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA7\xD8\xAA"); // شبكات

        add_mapping("\xD8\xAE\xD8\xA7\xD8\xAF\xD9\x85", // خادم
                    {{"server", 1.0}, {"backend", 0.8}, {"service", 0.6}},
                    "\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA7\xD8\xAA");

        add_mapping("\xD8\xB9\xD9\x85\xD9\x8A\xD9\x84", // عميل
                    {{"client", 1.0}, {"frontend", 0.7}},
                    "\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA7\xD8\xAA");

        add_mapping("\xD8\xB7\xD9\x84\xD8\xA8", // طلب
                    {{"request", 1.0}, {"http", 0.6}},
                    "\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA7\xD8\xAA");

        add_mapping("\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAC\xD8\xA7\xD8\xA8\xD8\xA9", // استجابة
                    {{"response", 1.0}},
                    "\xD8\xB4\xD8\xA8\xD9\x83\xD8\xA7\xD8\xAA");

        // ── البيانات ──
        add_mapping("\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA", // بيانات
                    {{"data", 1.0}, {"database", 0.8}, {"db", 0.7}, {"storage", 0.5}},
                    "\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA"); // بيانات

        add_mapping("\xD9\x82\xD8\xA7\xD8\xB9\xD8\xAF\xD8\xA9", // قاعدة
                    {{"database", 1.0}, {"db", 0.8}, {"sql", 0.6}},
                    "\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA");

        add_mapping("\xD8\xAA\xD8\xAE\xD8\xB2\xD9\x8A\xD9\x86", // تخزين
                    {{"storage", 1.0}, {"cache", 0.7}, {"store", 0.6}},
                    "\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA");

        add_mapping("\xD9\x85\xD9\x84\xD9\x81", // ملف
                    {{"file", 1.0}, {"io", 0.6}, {"filesystem", 0.7}},
                    "\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA");

        // ── الأمان ──
        add_mapping("\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86", // أمان
                    {{"security", 1.0}, {"crypto", 0.7}, {"auth", 0.6}},
                    "\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86"); // أمان

        add_mapping("\xD8\xAA\xD8\xB4\xD9\x81\xD9\x8A\xD8\xB1", // تشفير
                    {{"encryption", 1.0}, {"crypto", 0.9}, {"cipher", 0.7}},
                    "\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86");

        add_mapping("\xD9\x85\xD8\xB5\xD8\xA7\xD8\xAF\xD9\x82\xD8\xA9", // مصادقة
                    {{"authentication", 1.0}, {"auth", 0.9}, {"login", 0.6}},
                    "\xD8\xA3\xD9\x85\xD8\xA7\xD9\x86");

        // ── واجهات المستخدم ──
        add_mapping("\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA9", // واجهة
                    {{"ui", 1.0}, {"gui", 0.9}, {"interface", 0.8}, {"frontend", 0.6}},
                    "\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA7\xD8\xAA"); // واجهات

        add_mapping("\xD9\x86\xD8\xA7\xD9\x81\xD8\xB0\xD8\xA9", // نافذة
                    {{"window", 1.0}, {"dialog", 0.7}, {"gui", 0.5}},
                    "\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA7\xD8\xAA");

        // ── النصوص ──
        add_mapping("\xD9\x86\xD8\xB5", // نص
                    {{"text", 1.0}, {"string", 0.9}, {"str", 0.7}},
                    "\xD9\x86\xD8\xB5\xD9\x88\xD8\xB5"); // نصوص

        add_mapping("\xD8\xAC\xD9\x8A\xD8\xB3\xD9\x88\xD9\x86", // جيسون
                    {{"json", 1.0}, {"serialization", 0.5}},
                    "\xD9\x86\xD8\xB5\xD9\x88\xD8\xB5");

        add_mapping("\xD9\x85\xD8\xAD\xD9\x84\xD9\x84", // محلل
                    {{"parser", 1.0}, {"analyzer", 0.7}},
                    "\xD9\x86\xD8\xB5\xD9\x88\xD8\xB5");

        // ── الرياضيات ──
        add_mapping("\xD8\xB1\xD9\x8A\xD8\xA7\xD8\xB6\xD9\x8A\xD8\xA7\xD8\xAA", // رياضيات
                    {{"math", 1.0}, {"mathematics", 1.0}, {"numeric", 0.6}},
                    "\xD8\xAD\xD8\xB3\xD8\xA7\xD8\xA8"); // حساب

        add_mapping("\xD8\xAD\xD8\xB3\xD8\xA7\xD8\xA8", // حساب
                    {{"calculation", 1.0}, {"math", 0.8}, {"compute", 0.7}},
                    "\xD8\xAD\xD8\xB3\xD8\xA7\xD8\xA8");

        // ── أدوات ──
        add_mapping("\xD8\xA3\xD8\xAF\xD8\xA7\xD8\xA9", // أداة
                    {{"tool", 1.0}, {"utility", 0.8}, {"utils", 0.7}},
                    "\xD8\xA3\xD8\xAF\xD9\x88\xD8\xA7\xD8\xAA"); // أدوات

        add_mapping("\xD9\x85\xD9\x83\xD8\xAA\xD8\xA8\xD8\xA9", // مكتبة
                    {{"library", 1.0}, {"lib", 0.8}},
                    "\xD8\xA3\xD8\xAF\xD9\x88\xD8\xA7\xD8\xAA");

        add_mapping("\xD8\xB3\xD8\xAC\xD9\x84", // سجل
                    {{"log", 1.0}, {"logging", 0.9}, {"logger", 0.8}},
                    "\xD8\xA3\xD8\xAF\xD9\x88\xD8\xA7\xD8\xAA");

        add_mapping("\xD8\xA5\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF", // إعداد
                    {{"config", 1.0}, {"configuration", 0.9}, {"settings", 0.8}},
                    "\xD8\xA3\xD8\xAF\xD9\x88\xD8\xA7\xD8\xAA");

        // ── النظام ──
        add_mapping("\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85", // نظام
                    {{"system", 1.0}, {"os", 0.7}},
                    "\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85"); // نظام

        add_mapping("\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9", // ذاكرة
                    {{"memory", 1.0}, {"ram", 0.6}, {"buffer", 0.5}},
                    "\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85");

        add_mapping("\xD9\x85\xD8\xB3\xD8\xA7\xD8\xB1", // مسار
                    {{"path", 1.0}, {"filepath", 0.8}, {"directory", 0.6}},
                    "\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85");

        // ── الاختبار ──
        add_mapping("\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xA7\xD8\xB1", // اختبار
                    {{"test", 1.0}, {"testing", 0.9}, {"unittest", 0.7}},
                    "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xA7\xD8\xB1"); // اختبار

        add_mapping("\xD8\xAA\xD8\xAD\xD9\x82\xD9\x82", // تحقق
                    {{"validation", 1.0}, {"verify", 0.8}, {"check", 0.7}},
                    "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xA7\xD8\xB1");
    }
};

} // namespace registry
} // namespace sad
