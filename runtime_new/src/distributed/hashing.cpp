/**
 * ===================================================================================
 *  التجزئة المتسقة - Consistent Hashing (T443)
 * ===================================================================================
 * 
 *  الوصف: خوارزمية التجزئة المتسقة لتوزيع البيانات
 *  Description: Consistent hashing algorithm for data distribution
 *
 *  يدعم:
 *  - حلقة التجزئة
 *  - العقد الافتراضية
 *  - إعادة التوزيع الأدنى
 *  - التوزيع المتوازن
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <cstdint>
#include <functional>
#include <algorithm>

namespace ص::موزع {

// ============== دالة التجزئة ==============

/**
 * MurmurHash3 - تجزئة سريعة
 */
class مجزئ {
public:
    static uint64_t تجزئة(const std::string& مفتاح, uint32_t بذرة = 0) {
        const uint64_t c1 = 0x87c37b91114253d5ULL;
        const uint64_t c2 = 0x4cf5ad432745937fULL;
        
        const uint8_t* data = reinterpret_cast<const uint8_t*>(مفتاح.data());
        const size_t len = مفتاح.length();
        const size_t nblocks = len / 16;
        
        uint64_t h1 = بذرة;
        uint64_t h2 = بذرة;
        
        // جسم
        const uint64_t* blocks = reinterpret_cast<const uint64_t*>(data);
        for (size_t i = 0; i < nblocks; i++) {
            uint64_t k1 = blocks[i * 2];
            uint64_t k2 = blocks[i * 2 + 1];
            
            k1 *= c1;
            k1 = (k1 << 31) | (k1 >> 33);
            k1 *= c2;
            h1 ^= k1;
            
            h1 = (h1 << 27) | (h1 >> 37);
            h1 += h2;
            h1 = h1 * 5 + 0x52dce729;
            
            k2 *= c2;
            k2 = (k2 << 33) | (k2 >> 31);
            k2 *= c1;
            h2 ^= k2;
            
            h2 = (h2 << 31) | (h2 >> 33);
            h2 += h1;
            h2 = h2 * 5 + 0x38495ab5;
        }
        
        // ذيل
        const uint8_t* tail = data + nblocks * 16;
        uint64_t k1 = 0;
        uint64_t k2 = 0;
        
        switch (len & 15) {
            case 15: k2 ^= static_cast<uint64_t>(tail[14]) << 48; [[fallthrough]];
            case 14: k2 ^= static_cast<uint64_t>(tail[13]) << 40; [[fallthrough]];
            case 13: k2 ^= static_cast<uint64_t>(tail[12]) << 32; [[fallthrough]];
            case 12: k2 ^= static_cast<uint64_t>(tail[11]) << 24; [[fallthrough]];
            case 11: k2 ^= static_cast<uint64_t>(tail[10]) << 16; [[fallthrough]];
            case 10: k2 ^= static_cast<uint64_t>(tail[9]) << 8; [[fallthrough]];
            case 9:  k2 ^= static_cast<uint64_t>(tail[8]);
                     k2 *= c2;
                     k2 = (k2 << 33) | (k2 >> 31);
                     k2 *= c1;
                     h2 ^= k2;
                     [[fallthrough]];
            case 8:  k1 ^= static_cast<uint64_t>(tail[7]) << 56; [[fallthrough]];
            case 7:  k1 ^= static_cast<uint64_t>(tail[6]) << 48; [[fallthrough]];
            case 6:  k1 ^= static_cast<uint64_t>(tail[5]) << 40; [[fallthrough]];
            case 5:  k1 ^= static_cast<uint64_t>(tail[4]) << 32; [[fallthrough]];
            case 4:  k1 ^= static_cast<uint64_t>(tail[3]) << 24; [[fallthrough]];
            case 3:  k1 ^= static_cast<uint64_t>(tail[2]) << 16; [[fallthrough]];
            case 2:  k1 ^= static_cast<uint64_t>(tail[1]) << 8; [[fallthrough]];
            case 1:  k1 ^= static_cast<uint64_t>(tail[0]);
                     k1 *= c1;
                     k1 = (k1 << 31) | (k1 >> 33);
                     k1 *= c2;
                     h1 ^= k1;
        }
        
        // إنهاء
        h1 ^= len;
        h2 ^= len;
        
        h1 += h2;
        h2 += h1;
        
        // fmix64
        h1 ^= h1 >> 33;
        h1 *= 0xff51afd7ed558ccdULL;
        h1 ^= h1 >> 33;
        h1 *= 0xc4ceb9fe1a85ec53ULL;
        h1 ^= h1 >> 33;
        
        h2 ^= h2 >> 33;
        h2 *= 0xff51afd7ed558ccdULL;
        h2 ^= h2 >> 33;
        h2 *= 0xc4ceb9fe1a85ec53ULL;
        h2 ^= h2 >> 33;
        
        h1 += h2;
        
        return h1;
    }
    
    static uint64_t تجزئة_مركبة(const std::string& مفتاح, int رقم_افتراضي) {
        return تجزئة(مفتاح + "#" + std::to_string(رقم_افتراضي));
    }
};

// ============== العقدة الافتراضية ==============

/**
 * عقدة افتراضية على الحلقة
 */
struct عقدة_افتراضية {
    std::string معرف_العقدة;    // العقدة الحقيقية
    int الرقم = 0;               // رقم العقدة الافتراضية
    uint64_t موقع_الحلقة = 0;   // الموقع على الحلقة
};

// ============== حلقة التجزئة ==============

/**
 * حلقة التجزئة المتسقة
 */
class حلقة_تجزئة {
private:
    // الحلقة: موقع -> عقدة افتراضية
    std::map<uint64_t, عقدة_افتراضية> الحلقة_;
    
    // العقد الحقيقية: معرف -> عدد الافتراضية
    std::map<std::string, int> العقد_;
    
    // عدد العقد الافتراضية الافتراضي
    int عقد_افتراضية_افتراضي_ = 150;
    
public:
    // ============== إدارة العقد ==============
    
    void إضافة_عقدة(const std::string& معرف, int عدد_افتراضية = -1) {
        if (عدد_افتراضية < 0) {
            عدد_افتراضية = عقد_افتراضية_افتراضي_;
        }
        
        if (العقد_.find(معرف) != العقد_.end()) {
            return;  // موجودة
        }
        
        العقد_[معرف] = عدد_افتراضية;
        
        // إضافة العقد الافتراضية
        for (int i = 0; i < عدد_افتراضية; i++) {
            uint64_t موقع = مجزئ::تجزئة_مركبة(معرف, i);
            
            عقدة_افتراضية ع;
            ع.معرف_العقدة = معرف;
            ع.الرقم = i;
            ع.موقع_الحلقة = موقع;
            
            الحلقة_[موقع] = ع;
        }
    }
    
    void إزالة_عقدة(const std::string& معرف) {
        auto it = العقد_.find(معرف);
        if (it == العقد_.end()) {
            return;
        }
        
        int عدد_افتراضية = it->second;
        العقد_.erase(it);
        
        // إزالة العقد الافتراضية
        for (int i = 0; i < عدد_افتراضية; i++) {
            uint64_t موقع = مجزئ::تجزئة_مركبة(معرف, i);
            الحلقة_.erase(موقع);
        }
    }
    
    // ============== البحث ==============
    
    std::string الحصول_على_عقدة(const std::string& مفتاح) const {
        if (الحلقة_.empty()) {
            return "";
        }
        
        uint64_t تجزئة = مجزئ::تجزئة(مفتاح);
        
        // البحث عن أول عقدة بعد أو تساوي التجزئة
        auto it = الحلقة_.lower_bound(تجزئة);
        
        // إذا وصلنا للنهاية، نعود للبداية (حلقة)
        if (it == الحلقة_.end()) {
            it = الحلقة_.begin();
        }
        
        return it->second.معرف_العقدة;
    }
    
    std::vector<std::string> الحصول_على_نسخ(const std::string& مفتاح,
                                              int عدد_نسخ) const {
        std::vector<std::string> نتيجة;
        std::set<std::string> مستخدمة;
        
        if (الحلقة_.empty() || عدد_نسخ <= 0) {
            return نتيجة;
        }
        
        uint64_t تجزئة = مجزئ::تجزئة(مفتاح);
        auto it = الحلقة_.lower_bound(تجزئة);
        
        // المرور على الحلقة
        while (نتيجة.size() < static_cast<size_t>(عدد_نسخ) &&
               مستخدمة.size() < العقد_.size()) {
            
            if (it == الحلقة_.end()) {
                it = الحلقة_.begin();
            }
            
            const std::string& عقدة = it->second.معرف_العقدة;
            
            if (مستخدمة.find(عقدة) == مستخدمة.end()) {
                نتيجة.push_back(عقدة);
                مستخدمة.insert(عقدة);
            }
            
            ++it;
        }
        
        return نتيجة;
    }
    
    // ============== إحصائيات ==============
    
    int عدد_العقد() const { return العقد_.size(); }
    int حجم_الحلقة() const { return الحلقة_.size(); }
    
    std::map<std::string, int> توزيع_المفاتيح(
            const std::vector<std::string>& المفاتيح) const {
        std::map<std::string, int> توزيع;
        
        for (const auto& مفتاح : المفاتيح) {
            std::string عقدة = الحصول_على_عقدة(مفتاح);
            if (!عقدة.empty()) {
                توزيع[عقدة]++;
            }
        }
        
        return توزيع;
    }
    
    double انحراف_التوزيع(const std::vector<std::string>& المفاتيح) const {
        auto توزيع = توزيع_المفاتيح(المفاتيح);
        
        if (توزيع.empty()) return 0.0;
        
        // حساب المتوسط
        double مجموع = 0.0;
        for (const auto& [_, عدد] : توزيع) {
            مجموع += عدد;
        }
        double متوسط = مجموع / توزيع.size();
        
        // حساب الانحراف المعياري
        double تباين = 0.0;
        for (const auto& [_, عدد] : توزيع) {
            double فرق = عدد - متوسط;
            تباين += فرق * فرق;
        }
        
        return sqrt(تباين / توزيع.size()) / متوسط;
    }
    
    // ============== إعادة التوزيع ==============
    
    std::vector<std::pair<std::string, std::string>> 
    حساب_إعادة_التوزيع(const std::string& عقدة_جديدة) const {
        // المفاتيح التي ستنتقل من عقدة قديمة إلى الجديدة
        std::vector<std::pair<std::string, std::string>> تحويلات;
        
        // هذه محاكاة - في الواقع نحتاج قائمة المفاتيح الفعلية
        
        return تحويلات;
    }
};

// ============== موزع البيانات ==============

/**
 * موزع البيانات باستخدام التجزئة المتسقة
 */
class موزع_بيانات {
private:
    حلقة_تجزئة الحلقة_;
    int عامل_النسخ_ = 3;
    
public:
    void تعيين_عامل_النسخ(int عامل) {
        عامل_النسخ_ = عامل;
    }
    
    void إضافة_خادم(const std::string& معرف) {
        الحلقة_.إضافة_عقدة(معرف);
    }
    
    void إزالة_خادم(const std::string& معرف) {
        الحلقة_.إزالة_عقدة(معرف);
    }
    
    std::string الخادم_الرئيسي(const std::string& مفتاح) const {
        return الحلقة_.الحصول_على_عقدة(مفتاح);
    }
    
    std::vector<std::string> خوادم_النسخ(const std::string& مفتاح) const {
        return الحلقة_.الحصول_على_نسخ(مفتاح, عامل_النسخ_);
    }
    
    int عدد_الخوادم() const {
        return الحلقة_.عدد_العقد();
    }
};

}  // namespace ص::موزع

// ============== C API ==============

extern "C" {

using namespace ص::موزع;

static موزع_بيانات* الموزع_العام = nullptr;

void sad_hashing_init(int عامل_النسخ) {
    if (!الموزع_العام) {
        الموزع_العام = new موزع_بيانات();
        الموزع_العام->تعيين_عامل_النسخ(عامل_النسخ);
    }
}

void sad_hashing_cleanup() {
    delete الموزع_العام;
    الموزع_العام = nullptr;
}

void sad_hashing_add_node(const char* معرف) {
    if (الموزع_العام) {
        الموزع_العام->إضافة_خادم(معرف);
    }
}

void sad_hashing_remove_node(const char* معرف) {
    if (الموزع_العام) {
        الموزع_العام->إزالة_خادم(معرف);
    }
}

const char* sad_hashing_get_node(const char* مفتاح) {
    if (!الموزع_العام) return "";
    
    static std::string نتيجة;
    نتيجة = الموزع_العام->الخادم_الرئيسي(مفتاح);
    
    return نتيجة.c_str();
}

int sad_hashing_node_count() {
    if (!الموزع_العام) return 0;
    return الموزع_العام->عدد_الخوادم();
}

}  // extern "C"
