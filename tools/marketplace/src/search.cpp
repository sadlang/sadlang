/**
 * ===================================================================================
 *  البحث واكتشاف الحزم - Package Discovery & Search (T411)
 * ===================================================================================
 * 
 *  الوصف: نظام بحث واكتشاف متقدم للحزم في السوق
 *  Description: Advanced search and discovery system for marketplace packages
 *
 *  المميزات:
 *  - بحث نصي كامل
 *  - بحث دلالي (Semantic)
 *  - اقتراحات ذكية
 *  - الحزم الموصى بها
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <cmath>

namespace ص::سوق {

// ============== الأنواع ==============

/**
 * معايير الترتيب
 */
enum class معيار_ترتيب {
    صلة,               // Relevance (default)
    تحميلات,           // Downloads
    تقييم,             // Rating
    جديد,              // Newest
    تحديث,             // Recently updated
    شعبية,             // Trending
    اسم                // Alphabetical
};

/**
 * نتيجة بحث مع درجة الصلة
 */
struct نتيجة_بحث {
    std::string معرف;
    std::string اسم;
    std::string وصف;
    std::string إصدار;
    std::string ناشر;
    
    double تقييم = 0.0;
    uint64_t تحميلات = 0;
    bool موثق = false;
    bool مجاني = true;
    double سعر = 0.0;
    
    double درجة_صلة = 0.0;
    std::vector<std::string> كلمات_متطابقة;
};

/**
 * اقتراح بحث
 */
struct اقتراح_بحث {
    std::string نص;
    std::string نوع;  // "package", "category", "keyword", "correction"
    double درجة = 0.0;
};

/**
 * توصية حزمة
 */
struct توصية_حزمة {
    std::string معرف;
    std::string اسم;
    std::string سبب;  // "popular", "similar", "trending", "new"
    double درجة = 0.0;
};

/**
 * فلتر البحث المتقدم
 */
struct فلتر_بحث_متقدم {
    std::string استعلام;
    
    // فلاتر
    std::vector<std::string> فئات;
    std::vector<std::string> كلمات_مفتاحية;
    std::optional<double> حد_تقييم_أدنى;
    std::optional<uint64_t> حد_تحميلات_أدنى;
    bool مجاني_فقط = false;
    bool موثق_فقط = false;
    
    // ترتيب
    معيار_ترتيب الترتيب = معيار_ترتيب::صلة;
    bool تنازلي = true;
    
    // تقسيم الصفحات
    int صفحة = 1;
    int حجم_صفحة = 20;
};

// ============== محرك البحث ==============

/**
 * محرك البحث
 */
class محرك_بحث {
private:
    // الفهرس المقلوب (Inverted Index)
    std::map<std::string, std::set<std::string>> فهرس_كلمات_;
    
    // بيانات الحزم
    std::map<std::string, نتيجة_بحث> الحزم_;
    
    // كلمات التوقف (Stop Words)
    std::set<std::string> كلمات_توقف_ = {
        "في", "من", "إلى", "على", "عن", "مع", "هذا", "هذه",
        "التي", "الذي", "وهو", "وهي", "أن", "كان", "كانت",
        "the", "a", "an", "in", "on", "at", "for", "to", "of", "is", "are"
    };
    
    // المرادفات
    std::map<std::string, std::vector<std::string>> مرادفات_ = {
        {"رسم", {"graphics", "drawing", "render", "رسومات", "جرافيكس"}},
        {"شبكة", {"network", "http", "api", "web", "ويب"}},
        {"قاعدة", {"database", "sql", "orm", "بيانات", "db"}},
        {"أمان", {"security", "crypto", "encryption", "تشفير"}},
        {"ذكاء", {"ai", "ml", "machine-learning", "اصطناعي"}}
    };
    
public:
    محرك_بحث() = default;
    
    // ============== الفهرسة ==============
    
    /**
     * فهرسة حزمة
     */
    void فهرسة(const نتيجة_بحث& حزمة) {
        الحزم_[حزمة.معرف] = حزمة;
        
        // فهرسة الاسم
        for (const auto& كلمة : تقسيم_نص(حزمة.اسم)) {
            if (!كلمة_توقف(كلمة)) {
                فهرس_كلمات_[تطبيع(كلمة)].insert(حزمة.معرف);
            }
        }
        
        // فهرسة الوصف
        for (const auto& كلمة : تقسيم_نص(حزمة.وصف)) {
            if (!كلمة_توقف(كلمة)) {
                فهرس_كلمات_[تطبيع(كلمة)].insert(حزمة.معرف);
            }
        }
    }
    
    /**
     * إزالة من الفهرس
     */
    void إزالة_من_الفهرس(const std::string& معرف) {
        الحزم_.erase(معرف);
        
        // إزالة من الفهرس المقلوب
        for (auto& [_, مجموعة] : فهرس_كلمات_) {
            مجموعة.erase(معرف);
        }
    }
    
    // ============== البحث ==============
    
    /**
     * بحث أساسي
     */
    std::vector<نتيجة_بحث> بحث(const std::string& استعلام) {
        فلتر_بحث_متقدم فلتر;
        فلتر.استعلام = استعلام;
        return بحث_متقدم(فلتر);
    }
    
    /**
     * بحث متقدم
     */
    std::vector<نتيجة_بحث> بحث_متقدم(const فلتر_بحث_متقدم& فلتر) {
        std::map<std::string, double> درجات;
        std::map<std::string, std::vector<std::string>> كلمات_متطابقة;
        
        // تقسيم الاستعلام
        auto كلمات = تقسيم_نص(فلتر.استعلام);
        
        for (const auto& كلمة : كلمات) {
            if (كلمة_توقف(كلمة)) continue;
            
            std::string مطبعة = تطبيع(كلمة);
            
            // البحث المباشر
            if (فهرس_كلمات_.count(مطبعة)) {
                for (const auto& معرف : فهرس_كلمات_[مطبعة]) {
                    درجات[معرف] += 1.0;
                    كلمات_متطابقة[معرف].push_back(كلمة);
                }
            }
            
            // البحث بالمرادفات
            for (const auto& [أصل, قائمة] : مرادفات_) {
                if (أصل == مطبعة) {
                    for (const auto& مرادف : قائمة) {
                        if (فهرس_كلمات_.count(مرادف)) {
                            for (const auto& معرف : فهرس_كلمات_[مرادف]) {
                                درجات[معرف] += 0.5;
                            }
                        }
                    }
                }
            }
            
            // البحث الجزئي (Prefix matching)
            for (const auto& [كلمة_فهرس, مجموعة] : فهرس_كلمات_) {
                if (كلمة_فهرس.find(مطبعة) == 0 && كلمة_فهرس != مطبعة) {
                    for (const auto& معرف : مجموعة) {
                        درجات[معرف] += 0.3;
                    }
                }
            }
        }
        
        // بناء النتائج
        std::vector<نتيجة_بحث> نتائج;
        
        for (const auto& [معرف, درجة] : درجات) {
            if (الحزم_.count(معرف)) {
                auto نتيجة = الحزم_[معرف];
                
                // تطبيق الفلاتر
                if (فلتر.مجاني_فقط && !نتيجة.مجاني) continue;
                if (فلتر.موثق_فقط && !نتيجة.موثق) continue;
                if (فلتر.حد_تقييم_أدنى && نتيجة.تقييم < *فلتر.حد_تقييم_أدنى) continue;
                if (فلتر.حد_تحميلات_أدنى && نتيجة.تحميلات < *فلتر.حد_تحميلات_أدنى) continue;
                
                // حساب الدرجة النهائية
                نتيجة.درجة_صلة = درجة;
                
                // تعزيز بناءً على التقييم والتحميلات
                نتيجة.درجة_صلة += نتيجة.تقييم * 0.1;
                نتيجة.درجة_صلة += std::log10(نتيجة.تحميلات + 1) * 0.05;
                
                // تعزيز للموثقين
                if (نتيجة.موثق) {
                    نتيجة.درجة_صلة *= 1.1;
                }
                
                نتيجة.كلمات_متطابقة = كلمات_متطابقة[معرف];
                نتائج.push_back(نتيجة);
            }
        }
        
        // الترتيب
        ترتيب_نتائج(نتائج, فلتر.الترتيب, فلتر.تنازلي);
        
        // التقسيم للصفحات
        int بداية = (فلتر.صفحة - 1) * فلتر.حجم_صفحة;
        int نهاية = std::min(بداية + فلتر.حجم_صفحة, (int)نتائج.size());
        
        if (بداية < نتائج.size()) {
            return std::vector<نتيجة_بحث>(نتائج.begin() + بداية, نتائج.begin() + نهاية);
        }
        
        return {};
    }
    
    // ============== الاقتراحات ==============
    
    /**
     * اقتراحات البحث
     */
    std::vector<اقتراح_بحث> اقتراحات(const std::string& بادئة, int حد = 10) {
        std::vector<اقتراح_بحث> نتائج;
        std::string مطبعة = تطبيع(بادئة);
        
        // اقتراح أسماء الحزم
        for (const auto& [_, حزمة] : الحزم_) {
            if (تطبيع(حزمة.اسم).find(مطبعة) == 0) {
                اقتراح_بحث اق;
                اق.نص = حزمة.اسم;
                اق.نوع = "package";
                اق.درجة = حزمة.تحميلات;
                نتائج.push_back(اق);
            }
        }
        
        // اقتراح الكلمات من الفهرس
        for (const auto& [كلمة, _] : فهرس_كلمات_) {
            if (كلمة.find(مطبعة) == 0) {
                اقتراح_بحث اق;
                اق.نص = كلمة;
                اق.نوع = "keyword";
                اق.درجة = 1.0;
                نتائج.push_back(اق);
            }
        }
        
        // ترتيب وتحديد
        std::sort(نتائج.begin(), نتائج.end(),
            [](const اقتراح_بحث& أ, const اقتراح_بحث& ب) {
                return أ.درجة > ب.درجة;
            });
        
        if (نتائج.size() > حد) {
            نتائج.resize(حد);
        }
        
        return نتائج;
    }
    
    // ============== التوصيات ==============
    
    /**
     * الحزم الموصى بها
     */
    std::vector<توصية_حزمة> توصيات(int حد = 10) {
        std::vector<توصية_حزمة> نتائج;
        
        // الأكثر شعبية
        std::vector<std::pair<std::string, uint64_t>> حسب_التحميلات;
        for (const auto& [معرف, حزمة] : الحزم_) {
            حسب_التحميلات.push_back({معرف, حزمة.تحميلات});
        }
        
        std::sort(حسب_التحميلات.begin(), حسب_التحميلات.end(),
            [](const auto& أ, const auto& ب) { return أ.second > ب.second; });
        
        for (int i = 0; i < std::min(حد / 2, (int)حسب_التحميلات.size()); ++i) {
            const auto& حزمة = الحزم_[حسب_التحميلات[i].first];
            توصية_حزمة توصية;
            توصية.معرف = حزمة.معرف;
            توصية.اسم = حزمة.اسم;
            توصية.سبب = "شائع";
            توصية.درجة = حزمة.تحميلات;
            نتائج.push_back(توصية);
        }
        
        // الأعلى تقييماً
        std::vector<std::pair<std::string, double>> حسب_التقييم;
        for (const auto& [معرف, حزمة] : الحزم_) {
            if (حزمة.تحميلات > 100) {  // فقط الحزم المستخدمة
                حسب_التقييم.push_back({معرف, حزمة.تقييم});
            }
        }
        
        std::sort(حسب_التقييم.begin(), حسب_التقييم.end(),
            [](const auto& أ, const auto& ب) { return أ.second > ب.second; });
        
        for (int i = 0; i < std::min(حد / 2, (int)حسب_التقييم.size()); ++i) {
            const auto& حزمة = الحزم_[حسب_التقييم[i].first];
            
            // تجنب التكرار
            bool موجود = false;
            for (const auto& ت : نتائج) {
                if (ت.معرف == حزمة.معرف) {
                    موجود = true;
                    break;
                }
            }
            
            if (!موجود) {
                توصية_حزمة توصية;
                توصية.معرف = حزمة.معرف;
                توصية.اسم = حزمة.اسم;
                توصية.سبب = "عالي التقييم";
                توصية.درجة = حزمة.تقييم;
                نتائج.push_back(توصية);
            }
        }
        
        return نتائج;
    }
    
    /**
     * حزم مشابهة
     */
    std::vector<توصية_حزمة> حزم_مشابهة(const std::string& معرف_حزمة, int حد = 5) {
        std::vector<توصية_حزمة> نتائج;
        
        if (!الحزم_.count(معرف_حزمة)) return نتائج;
        
        const auto& حزمة = الحزم_[معرف_حزمة];
        
        // البحث عن حزم تتشارك كلمات مفتاحية
        auto كلمات = تقسيم_نص(حزمة.اسم + " " + حزمة.وصف);
        std::map<std::string, int> تشابه;
        
        for (const auto& كلمة : كلمات) {
            if (كلمة_توقف(كلمة)) continue;
            
            std::string مطبعة = تطبيع(كلمة);
            if (فهرس_كلمات_.count(مطبعة)) {
                for (const auto& معرف : فهرس_كلمات_[مطبعة]) {
                    if (معرف != معرف_حزمة) {
                        تشابه[معرف]++;
                    }
                }
            }
        }
        
        // ترتيب حسب التشابه
        std::vector<std::pair<std::string, int>> مرتبة(تشابه.begin(), تشابه.end());
        std::sort(مرتبة.begin(), مرتبة.end(),
            [](const auto& أ, const auto& ب) { return أ.second > ب.second; });
        
        for (int i = 0; i < std::min(حد, (int)مرتبة.size()); ++i) {
            const auto& ح = الحزم_[مرتبة[i].first];
            توصية_حزمة توصية;
            توصية.معرف = ح.معرف;
            توصية.اسم = ح.اسم;
            توصية.سبب = "مشابه";
            توصية.درجة = مرتبة[i].second;
            نتائج.push_back(توصية);
        }
        
        return نتائج;
    }
    
private:
    /**
     * تقسيم النص إلى كلمات
     */
    std::vector<std::string> تقسيم_نص(const std::string& نص) {
        std::vector<std::string> كلمات;
        std::string كلمة;
        
        for (char c : نص) {
            if (std::isalnum(c) || (unsigned char)c > 127) {
                كلمة += c;
            } else {
                if (!كلمة.empty()) {
                    كلمات.push_back(كلمة);
                    كلمة.clear();
                }
            }
        }
        
        if (!كلمة.empty()) {
            كلمات.push_back(كلمة);
        }
        
        return كلمات;
    }
    
    /**
     * تطبيع الكلمة
     */
    std::string تطبيع(const std::string& كلمة) {
        std::string نتيجة;
        for (char c : كلمة) {
            if (c >= 'A' && c <= 'Z') {
                نتيجة += (c - 'A' + 'a');
            } else {
                نتيجة += c;
            }
        }
        return نتيجة;
    }
    
    /**
     * هل كلمة توقف؟
     */
    bool كلمة_توقف(const std::string& كلمة) {
        return كلمات_توقف_.count(تطبيع(كلمة)) > 0;
    }
    
    /**
     * ترتيب النتائج
     */
    void ترتيب_نتائج(std::vector<نتيجة_بحث>& نتائج,
                    معيار_ترتيب معيار,
                    bool تنازلي) {
        auto مقارن = [&](const نتيجة_بحث& أ, const نتيجة_بحث& ب) {
            double قيمة_أ, قيمة_ب;
            
            switch (معيار) {
                case معيار_ترتيب::صلة:
                    قيمة_أ = أ.درجة_صلة;
                    قيمة_ب = ب.درجة_صلة;
                    break;
                case معيار_ترتيب::تحميلات:
                    قيمة_أ = أ.تحميلات;
                    قيمة_ب = ب.تحميلات;
                    break;
                case معيار_ترتيب::تقييم:
                    قيمة_أ = أ.تقييم;
                    قيمة_ب = ب.تقييم;
                    break;
                case معيار_ترتيب::اسم:
                    return تنازلي ? أ.اسم > ب.اسم : أ.اسم < ب.اسم;
                default:
                    قيمة_أ = أ.درجة_صلة;
                    قيمة_ب = ب.درجة_صلة;
            }
            
            return تنازلي ? قيمة_أ > قيمة_ب : قيمة_أ < قيمة_ب;
        };
        
        std::sort(نتائج.begin(), نتائج.end(), مقارن);
    }
};

}  // namespace ص::سوق

// ============== C API ==============

extern "C" {

using namespace ص::سوق;

typedef struct محرك_بحث_مقبض {
    std::unique_ptr<محرك_بحث> محرك;
} محرك_بحث_مقبض;

محرك_بحث_مقبض* sad_search_create() {
    auto مقبض = new محرك_بحث_مقبض();
    مقبض->محرك = std::make_unique<محرك_بحث>();
    return مقبض;
}

void sad_search_destroy(محرك_بحث_مقبض* مقبض) {
    delete مقبض;
}

void sad_search_index(محرك_بحث_مقبض* مقبض,
                      const char* id,
                      const char* name,
                      const char* description,
                      double rating,
                      uint64_t downloads) {
    if (مقبض && مقبض->محرك) {
        نتيجة_بحث حزمة;
        حزمة.معرف = id ? id : "";
        حزمة.اسم = name ? name : "";
        حزمة.وصف = description ? description : "";
        حزمة.تقييم = rating;
        حزمة.تحميلات = downloads;
        مقبض->محرك->فهرسة(حزمة);
    }
}

int sad_search_query(محرك_بحث_مقبض* مقبض,
                     const char* query,
                     char*** results,
                     int max_results) {
    // إرجاع عدد النتائج
    if (!مقبض || !مقبض->محرك) return 0;
    
    auto نتائج = مقبض->محرك->بحث(query ? query : "");
    return std::min((int)نتائج.size(), max_results);
}

}  // extern "C"
