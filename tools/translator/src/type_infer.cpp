/**
 * ==========================================================
 * ملف: type_infer.cpp
 * الوصف: استنتاج الأنواع من الكود غير المُنمط
 * المهمة: T390 - ترجمة ص
 * ==========================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>

namespace sad {
namespace translator {

/**
 * نوع ص
 */
enum class نوع_ص {
    غير_معروف,
    صحيح,
    عشري,
    نص,
    منطقي,
    قائمة,
    قاموس,
    دالة,
    كائن,
    فارغ
};

/**
 * معلومات نوع
 */
struct معلومات_نوع {
    نوع_ص نوع;
    std::string نوع_عنصر;  // للقوائم
    std::string نوع_مفتاح;  // للقواميس
    std::string نوع_قيمة;   // للقواميس
    float ثقة;  // مستوى الثقة 0.0 - 1.0
};

/**
 * مستنتج الأنواع
 */
class مستنتج_الأنواع {
private:
    std::map<std::string, معلومات_نوع> متغيرات_;
    
public:
    /**
     * تحليل الكود واستنتاج الأنواع
     */
    std::map<std::string, معلومات_نوع> حلل(const std::string& كود) {
        متغيرات_.clear();
        
        // 1. استنتاج من القيم الابتدائية
        استنتج_من_القيم(كود);
        
        // 2. استنتاج من الاستخدام
        استنتج_من_الاستخدام(كود);
        
        // 3. استنتاج من العمليات
        استنتج_من_العمليات(كود);
        
        return متغيرات_;
    }
    
    /**
     * تحويل إلى تعليقات أنواع ص
     */
    std::string أنتج_تعليقات_أنواع() {
        std::string نتيجة;
        
        for (const auto& [اسم, معلومات] : متغيرات_) {
            نتيجة += اسم + ": " + نوع_إلى_نص(معلومات) + "\n";
        }
        
        return نتيجة;
    }
    
    /**
     * إضافة أنواع للكود
     */
    std::string أضف_أنواع(const std::string& كود) {
        std::string نتيجة = كود;
        
        for (const auto& [اسم, معلومات] : متغيرات_) {
            // البحث عن تعريف المتغير وإضافة النوع
            std::regex نمط("(متغير|ثابت)\\s+" + اسم + "\\s*=");
            std::string بديل = "$1 " + اسم + ": " + نوع_إلى_نص(معلومات) + " =";
            نتيجة = std::regex_replace(نتيجة, نمط, بديل);
        }
        
        return نتيجة;
    }
    
private:
    void استنتج_من_القيم(const std::string& كود) {
        // البحث عن تعيينات
        std::regex نمط_تعيين(R"((متغير|ثابت|let|const|var)\s+(\w+)\s*=\s*(.+))");
        std::smatch تطابق;
        std::string نسخة = كود;
        
        while (std::regex_search(نسخة, تطابق, نمط_تعيين)) {
            std::string اسم = تطابق[2];
            std::string قيمة = تطابق[3];
            
            معلومات_نوع معلومات = استنتج_نوع_قيمة(قيمة);
            متغيرات_[اسم] = معلومات;
            
            نسخة = تطابق.suffix();
        }
    }
    
    معلومات_نوع استنتج_نوع_قيمة(const std::string& قيمة) {
        معلومات_نوع معلومات;
        معلومات.نوع = نوع_ص::غير_معروف;
        معلومات.ثقة = 0.5;
        
        std::string ق = قيمة;
        // إزالة المسافات
        while (!ق.empty() && ق.front() == ' ') ق.erase(0, 1);
        while (!ق.empty() && (ق.back() == ' ' || ق.back() == ';' || ق.back() == '\n')) ق.pop_back();
        
        // رقم صحيح
        if (std::regex_match(ق, std::regex(R"(-?\d+)"))) {
            معلومات.نوع = نوع_ص::صحيح;
            معلومات.ثقة = 1.0;
            return معلومات;
        }
        
        // رقم عشري
        if (std::regex_match(ق, std::regex(R"(-?\d+\.\d+)"))) {
            معلومات.نوع = نوع_ص::عشري;
            معلومات.ثقة = 1.0;
            return معلومات;
        }
        
        // نص
        if ((ق.front() == '"' && ق.back() == '"') ||
            (ق.front() == '\'' && ق.back() == '\'') ||
            (ق.front() == '`' && ق.back() == '`')) {
            معلومات.نوع = نوع_ص::نص;
            معلومات.ثقة = 1.0;
            return معلومات;
        }
        
        // منطقي
        if (ق == "true" || ق == "false" || ق == "صحيح" || ق == "خطأ" ||
            ق == "True" || ق == "False") {
            معلومات.نوع = نوع_ص::منطقي;
            معلومات.ثقة = 1.0;
            return معلومات;
        }
        
        // فارغ
        if (ق == "null" || ق == "None" || ق == "undefined" || ق == "فارغ") {
            معلومات.نوع = نوع_ص::فارغ;
            معلومات.ثقة = 1.0;
            return معلومات;
        }
        
        // قائمة
        if (ق.front() == '[' && ق.back() == ']') {
            معلومات.نوع = نوع_ص::قائمة;
            معلومات.ثقة = 0.9;
            // استنتاج نوع العناصر
            معلومات.نوع_عنصر = استنتج_نوع_عناصر_قائمة(ق);
            return معلومات;
        }
        
        // قاموس/كائن
        if (ق.front() == '{' && ق.back() == '}') {
            معلومات.نوع = نوع_ص::قاموس;
            معلومات.ثقة = 0.9;
            return معلومات;
        }
        
        // دالة
        if (ق.find("function") != std::string::npos ||
            ق.find("=>") != std::string::npos ||
            ق.find("lambda") != std::string::npos ||
            ق.find("دالة") != std::string::npos) {
            معلومات.نوع = نوع_ص::دالة;
            معلومات.ثقة = 0.95;
            return معلومات;
        }
        
        return معلومات;
    }
    
    std::string استنتج_نوع_عناصر_قائمة(const std::string& قائمة) {
        // استخراج العناصر
        if (قائمة.length() <= 2) return "غير_معروف";
        
        std::string محتوى = قائمة.substr(1, قائمة.length() - 2);
        if (محتوى.empty()) return "غير_معروف";
        
        // أخذ أول عنصر
        size_t فاصلة = محتوى.find(',');
        std::string أول = (فاصلة != std::string::npos) ? محتوى.substr(0, فاصلة) : محتوى;
        
        auto نوع_أول = استنتج_نوع_قيمة(أول);
        return نوع_إلى_نص(نوع_أول);
    }
    
    void استنتج_من_الاستخدام(const std::string& كود) {
        // استنتاج من استخدام الدوال
        for (auto& [اسم, معلومات] : متغيرات_) {
            if (معلومات.نوع != نوع_ص::غير_معروف) continue;
            
            // البحث عن استخدامات
            if (كود.find(اسم + ".length") != std::string::npos ||
                كود.find(اسم + ".طول") != std::string::npos) {
                // قد يكون نص أو قائمة
                if (كود.find(اسم + ".push") != std::string::npos ||
                    كود.find(اسم + ".أضف") != std::string::npos ||
                    كود.find(اسم + "[") != std::string::npos) {
                    معلومات.نوع = نوع_ص::قائمة;
                    معلومات.ثقة = 0.7;
                } else {
                    معلومات.نوع = نوع_ص::نص;
                    معلومات.ثقة = 0.6;
                }
            }
            
            // استخدام كرقم
            if (كود.find(اسم + " +") != std::string::npos ||
                كود.find(اسم + " -") != std::string::npos ||
                كود.find(اسم + " *") != std::string::npos ||
                كود.find(اسم + " /") != std::string::npos) {
                معلومات.نوع = نوع_ص::صحيح;  // افتراض صحيح
                معلومات.ثقة = 0.5;
            }
        }
    }
    
    void استنتج_من_العمليات(const std::string& كود) {
        // استنتاج من مقارنات
        for (auto& [اسم, معلومات] : متغيرات_) {
            if (معلومات.نوع != نوع_ص::غير_معروف) continue;
            
            // مقارنة منطقية
            std::regex نمط_منطقي(اسم + R"(\s*(==|!=|===|!==)\s*(true|false|صحيح|خطأ))");
            if (std::regex_search(كود, نمط_منطقي)) {
                معلومات.نوع = نوع_ص::منطقي;
                معلومات.ثقة = 0.8;
            }
        }
    }
    
    std::string نوع_إلى_نص(const معلومات_نوع& معلومات) {
        switch (معلومات.نوع) {
            case نوع_ص::صحيح: return "صحيح";
            case نوع_ص::عشري: return "عشري";
            case نوع_ص::نص: return "نص";
            case نوع_ص::منطقي: return "منطقي";
            case نوع_ص::قائمة: 
                return "قائمة<" + (معلومات.نوع_عنصر.empty() ? "أي" : معلومات.نوع_عنصر) + ">";
            case نوع_ص::قاموس:
                return "قاموس<نص، أي>";
            case نوع_ص::دالة: return "دالة";
            case نوع_ص::كائن: return "كائن";
            case نوع_ص::فارغ: return "فارغ?";
            default: return "أي";
        }
    }
};

} // namespace translator
} // namespace sad

extern "C" {
using namespace sad::translator;

void* sad_type_infer_create() {
    return new مستنتج_الأنواع();
}

void sad_type_infer_destroy(void* infer) {
    delete static_cast<مستنتج_الأنواع*>(infer);
}

const char* sad_type_infer_analyze(void* infer, const char* code) {
    static std::string نتيجة;
    auto مستنتج = static_cast<مستنتج_الأنواع*>(infer);
    مستنتج->حلل(code);
    نتيجة = مستنتج->أنتج_تعليقات_أنواع();
    return نتيجة.c_str();
}

const char* sad_type_infer_add_types(void* infer, const char* code) {
    static std::string نتيجة;
    auto مستنتج = static_cast<مستنتج_الأنواع*>(infer);
    مستنتج->حلل(code);
    نتيجة = مستنتج->أضف_أنواع(code);
    return نتيجة.c_str();
}
}
