/**
 * ===================================================================================
 *  التعلم والإرشاد - Peer Learning & Mentoring (T428)
 * ===================================================================================
 * 
 *  الوصف: نظام التعلم من الأقران والإرشاد
 *  Description: Peer learning and mentoring system
 *
 *  يدعم:
 *  - جلسات إرشاد
 *  - مجموعات دراسة
 *  - أسئلة وأجوبة
 *  - تقييم المرشدين
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ctime>

namespace ص::جامعة {

// ============== أنواع الإرشاد ==============

/**
 * نوع الإرشاد
 */
enum class نوع_إرشاد {
    فردي,           // جلسة 1:1
    جماعي,          // مجموعة صغيرة
    ورشة,           // ورشة عمل
    محاضرة,         // محاضرة مفتوحة
    مراجعة_كود,     // مراجعة كود
    تصحيح_أخطاء     // مساعدة في تصحيح الأخطاء
};

/**
 * حالة الجلسة
 */
enum class حالة_جلسة {
    قادمة,
    نشطة,
    مكتملة,
    ملغاة
};

/**
 * مستوى المرشد
 */
enum class مستوى_مرشد {
    متطوع,         // مبتدئ يريد المساعدة
    مساعد,         // خبرة متوسطة
    مرشد,          // خبير معتمد
    معلم           // مدرس رسمي
};

// ============== المرشد ==============

/**
 * ملف المرشد
 */
struct مرشد {
    std::string معرف;
    std::string الاسم;
    std::string البريد;
    
    مستوى_مرشد المستوى = مستوى_مرشد::متطوع;
    
    // التخصصات
    std::vector<std::string> التخصصات;
    
    // الخبرة
    std::string نبذة;
    int سنوات_الخبرة = 0;
    
    // الإحصائيات
    int جلسات_مكتملة = 0;
    double تقييم_متوسط = 0.0;
    int عدد_التقييمات = 0;
    
    // التوفر
    std::vector<std::string> أوقات_متاحة;
    bool متاح = true;
    
    // السعر (0 = مجاني)
    double السعر_بالساعة = 0.0;
};

// ============== الجلسة ==============

/**
 * جلسة إرشاد
 */
struct جلسة_إرشاد {
    std::string معرف;
    std::string عنوان;
    std::string وصف;
    
    نوع_إرشاد النوع;
    حالة_جلسة الحالة = حالة_جلسة::قادمة;
    
    // المشاركون
    std::string معرف_مرشد;
    std::vector<std::string> المتعلمون;
    int الحد_الأقصى = 1;  // للجلسات الجماعية
    
    // الوقت
    std::string تاريخ_البدء;
    std::string تاريخ_النهاية;
    int المدة = 60;  // بالدقائق
    
    // المكان
    std::string الرابط;  // رابط الاجتماع الافتراضي
    
    // الموضوع
    std::vector<std::string> المواضيع;
    
    // الملاحظات
    std::string ملاحظات_المرشد;
    std::string ملاحظات_الجلسة;
};

// ============== السؤال ==============

/**
 * سؤال في منتدى الأسئلة
 */
struct سؤال {
    std::string معرف;
    std::string معرف_سائل;
    
    std::string العنوان;
    std::string النص;
    std::string الكود;  // كود مرفق
    
    std::vector<std::string> الوسوم;
    
    // الإحصائيات
    int المشاهدات = 0;
    int الأصوات = 0;
    
    // الإجابات
    int عدد_الإجابات = 0;
    bool تم_الحل = false;
    std::string معرف_أفضل_إجابة;
    
    std::string تاريخ_الإنشاء;
    std::string تاريخ_التحديث;
};

// ============== الإجابة ==============

/**
 * إجابة على سؤال
 */
struct إجابة {
    std::string معرف;
    std::string معرف_سؤال;
    std::string معرف_مجيب;
    
    std::string النص;
    std::string الكود;  // كود مرفق
    
    int الأصوات = 0;
    bool أفضل_إجابة = false;
    
    std::string التاريخ;
};

// ============== مجموعة الدراسة ==============

/**
 * مجموعة دراسة
 */
struct مجموعة_دراسة {
    std::string معرف;
    std::string الاسم;
    std::string الوصف;
    
    std::string معرف_المنشئ;
    std::vector<std::string> الأعضاء;
    int الحد_الأقصى = 10;
    
    // الموضوع
    std::string معرف_دورة;  // دورة مرتبطة
    std::vector<std::string> المواضيع;
    
    // الجلسات
    std::vector<std::string> الجلسات;  // معرفات جلسات
    
    // الحالة
    bool نشطة = true;
    bool خاصة = false;
    
    std::string تاريخ_الإنشاء;
};

// ============== التقييم ==============

/**
 * تقييم جلسة/مرشد
 */
struct تقييم {
    std::string معرف;
    std::string معرف_جلسة;
    std::string معرف_مرشد;
    std::string معرف_متعلم;
    
    int النجوم = 5;  // 1-5
    std::string التعليق;
    
    // تقييمات فرعية
    int المعرفة = 5;
    int التواصل = 5;
    int الالتزام = 5;
    
    std::string التاريخ;
};

// ============== نظام الإرشاد ==============

/**
 * نظام إدارة الإرشاد والتعلم
 */
class نظام_إرشاد {
private:
    std::map<std::string, مرشد> المرشدون_;
    std::map<std::string, جلسة_إرشاد> الجلسات_;
    std::map<std::string, سؤال> الأسئلة_;
    std::map<std::string, std::vector<إجابة>> الإجابات_;
    std::map<std::string, مجموعة_دراسة> المجموعات_;
    std::map<std::string, std::vector<تقييم>> التقييمات_;
    
    int عداد_ = 0;
    
public:
    // ============== إدارة المرشدين ==============
    
    std::string تسجيل_مرشد(مرشد& م) {
        م.معرف = "mentor_" + std::to_string(++عداد_);
        المرشدون_[م.معرف] = م;
        return م.معرف;
    }
    
    مرشد* الحصول_على_مرشد(const std::string& معرف) {
        auto it = المرشدون_.find(معرف);
        return it != المرشدون_.end() ? &it->second : nullptr;
    }
    
    std::vector<مرشد> البحث_عن_مرشدين(const std::string& تخصص = "") const {
        std::vector<مرشد> نتيجة;
        
        for (const auto& [_, م] : المرشدون_) {
            if (!م.متاح) continue;
            
            if (تخصص.empty()) {
                نتيجة.push_back(م);
            } else {
                for (const auto& ت : م.التخصصات) {
                    if (ت.find(تخصص) != std::string::npos) {
                        نتيجة.push_back(م);
                        break;
                    }
                }
            }
        }
        
        // ترتيب بالتقييم
        for (size_t i = 0; i < نتيجة.size(); i++) {
            for (size_t j = i + 1; j < نتيجة.size(); j++) {
                if (نتيجة[j].تقييم_متوسط > نتيجة[i].تقييم_متوسط) {
                    std::swap(نتيجة[i], نتيجة[j]);
                }
            }
        }
        
        return نتيجة;
    }
    
    // ============== الجلسات ==============
    
    std::string إنشاء_جلسة(جلسة_إرشاد& ج) {
        ج.معرف = "session_" + std::to_string(++عداد_);
        الجلسات_[ج.معرف] = ج;
        return ج.معرف;
    }
    
    bool حجز_جلسة(const std::string& معرف_جلسة, 
                  const std::string& معرف_متعلم) {
        auto it = الجلسات_.find(معرف_جلسة);
        if (it == الجلسات_.end()) return false;
        
        auto& ج = it->second;
        
        if (ج.الحالة != حالة_جلسة::قادمة) return false;
        if (static_cast<int>(ج.المتعلمون.size()) >= ج.الحد_الأقصى) return false;
        
        // التحقق من عدم التسجيل المسبق
        for (const auto& م : ج.المتعلمون) {
            if (م == معرف_متعلم) return false;
        }
        
        ج.المتعلمون.push_back(معرف_متعلم);
        return true;
    }
    
    bool إكمال_جلسة(const std::string& معرف_جلسة,
                     const std::string& ملاحظات = "") {
        auto it = الجلسات_.find(معرف_جلسة);
        if (it == الجلسات_.end()) return false;
        
        it->second.الحالة = حالة_جلسة::مكتملة;
        it->second.ملاحظات_الجلسة = ملاحظات;
        
        // تحديث إحصائيات المرشد
        auto* م = الحصول_على_مرشد(it->second.معرف_مرشد);
        if (م) {
            م->جلسات_مكتملة++;
        }
        
        return true;
    }
    
    std::vector<جلسة_إرشاد> جلسات_المرشد(const std::string& معرف_مرشد) const {
        std::vector<جلسة_إرشاد> نتيجة;
        
        for (const auto& [_, ج] : الجلسات_) {
            if (ج.معرف_مرشد == معرف_مرشد) {
                نتيجة.push_back(ج);
            }
        }
        
        return نتيجة;
    }
    
    // ============== الأسئلة والأجوبة ==============
    
    std::string طرح_سؤال(سؤال& س) {
        س.معرف = "question_" + std::to_string(++عداد_);
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d %H:%M", tm_info);
        س.تاريخ_الإنشاء = buffer;
        س.تاريخ_التحديث = buffer;
        
        الأسئلة_[س.معرف] = س;
        return س.معرف;
    }
    
    std::string إضافة_إجابة(إجابة& ج) {
        ج.معرف = "answer_" + std::to_string(++عداد_);
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d %H:%M", tm_info);
        ج.التاريخ = buffer;
        
        الإجابات_[ج.معرف_سؤال].push_back(ج);
        
        // تحديث السؤال
        auto it = الأسئلة_.find(ج.معرف_سؤال);
        if (it != الأسئلة_.end()) {
            it->second.عدد_الإجابات++;
            it->second.تاريخ_التحديث = buffer;
        }
        
        return ج.معرف;
    }
    
    bool قبول_إجابة(const std::string& معرف_سؤال, 
                    const std::string& معرف_إجابة) {
        auto it = الأسئلة_.find(معرف_سؤال);
        if (it == الأسئلة_.end()) return false;
        
        it->second.تم_الحل = true;
        it->second.معرف_أفضل_إجابة = معرف_إجابة;
        
        // تحديث الإجابة
        auto& إجابات = الإجابات_[معرف_سؤال];
        for (auto& ج : إجابات) {
            ج.أفضل_إجابة = (ج.معرف == معرف_إجابة);
        }
        
        return true;
    }
    
    std::vector<سؤال> أحدث_الأسئلة(int عدد = 10) const {
        std::vector<سؤال> قائمة;
        
        for (const auto& [_, س] : الأسئلة_) {
            قائمة.push_back(س);
        }
        
        // ترتيب بالتاريخ
        for (size_t i = 0; i < قائمة.size(); i++) {
            for (size_t j = i + 1; j < قائمة.size(); j++) {
                if (قائمة[j].تاريخ_الإنشاء > قائمة[i].تاريخ_الإنشاء) {
                    std::swap(قائمة[i], قائمة[j]);
                }
            }
        }
        
        if (static_cast<int>(قائمة.size()) > عدد) {
            قائمة.resize(عدد);
        }
        
        return قائمة;
    }
    
    // ============== مجموعات الدراسة ==============
    
    std::string إنشاء_مجموعة(مجموعة_دراسة& م) {
        م.معرف = "group_" + std::to_string(++عداد_);
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[11];
        strftime(buffer, 11, "%Y-%m-%d", tm_info);
        م.تاريخ_الإنشاء = buffer;
        
        م.الأعضاء.push_back(م.معرف_المنشئ);
        
        المجموعات_[م.معرف] = م;
        return م.معرف;
    }
    
    bool الانضمام_لمجموعة(const std::string& معرف_مجموعة,
                          const std::string& معرف_عضو) {
        auto it = المجموعات_.find(معرف_مجموعة);
        if (it == المجموعات_.end()) return false;
        
        auto& م = it->second;
        
        if (!م.نشطة) return false;
        if (static_cast<int>(م.الأعضاء.size()) >= م.الحد_الأقصى) return false;
        
        for (const auto& عضو : م.الأعضاء) {
            if (عضو == معرف_عضو) return false;
        }
        
        م.الأعضاء.push_back(معرف_عضو);
        return true;
    }
    
    // ============== التقييم ==============
    
    bool تقييم_جلسة(تقييم& ت) {
        ت.معرف = "rating_" + std::to_string(++عداد_);
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d %H:%M", tm_info);
        ت.التاريخ = buffer;
        
        التقييمات_[ت.معرف_مرشد].push_back(ت);
        
        // تحديث متوسط تقييم المرشد
        auto* م = الحصول_على_مرشد(ت.معرف_مرشد);
        if (م) {
            م->عدد_التقييمات++;
            م->تقييم_متوسط = (م->تقييم_متوسط * (م->عدد_التقييمات - 1) + ت.النجوم) 
                             / م->عدد_التقييمات;
        }
        
        return true;
    }
};

}  // namespace ص::جامعة

// ============== C API ==============

extern "C" {

using namespace ص::جامعة;

static نظام_إرشاد* النظام_العام = nullptr;

void sad_mentoring_init() {
    if (!النظام_العام) {
        النظام_العام = new نظام_إرشاد();
    }
}

void sad_mentoring_cleanup() {
    delete النظام_العام;
    النظام_العام = nullptr;
}

const char* sad_mentoring_register_mentor(const char* اسم, const char* بريد) {
    if (!النظام_العام) return "";
    
    مرشد م;
    م.الاسم = اسم;
    م.البريد = بريد;
    
    static std::string نتيجة;
    نتيجة = النظام_العام->تسجيل_مرشد(م);
    
    return نتيجة.c_str();
}

int sad_mentoring_book_session(const char* معرف_جلسة, const char* معرف_متعلم) {
    if (!النظام_العام) return 0;
    return النظام_العام->حجز_جلسة(معرف_جلسة, معرف_متعلم) ? 1 : 0;
}

const char* sad_mentoring_ask_question(const char* عنوان, 
                                        const char* نص,
                                        const char* معرف_سائل) {
    if (!النظام_العام) return "";
    
    سؤال س;
    س.العنوان = عنوان;
    س.النص = نص;
    س.معرف_سائل = معرف_سائل;
    
    static std::string نتيجة;
    نتيجة = النظام_العام->طرح_سؤال(س);
    
    return نتيجة.c_str();
}

int sad_mentoring_join_group(const char* معرف_مجموعة, const char* معرف_عضو) {
    if (!النظام_العام) return 0;
    return النظام_العام->الانضمام_لمجموعة(معرف_مجموعة, معرف_عضو) ? 1 : 0;
}

}  // extern "C"
