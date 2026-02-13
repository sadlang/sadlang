/**
 * ==========================================================
 * ملف: py_to_sad.cpp
 * الوصف: تحويل Python إلى كود ص
 * المهمة: T387 - ترجمة ص
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
 * قواعد التحويل Python → ص
 */
class محوّل_بايثون_إلى_ص {
private:
    std::map<std::string, std::string> كلمات_مفتاحية_;
    std::map<std::string, std::string> أنواع_;
    std::map<std::string, std::string> دوال_مدمجة_;
    
public:
    محوّل_بايثون_إلى_ص() {
        // تهيئة جداول التحويل
        هيّئ_الكلمات_المفتاحية();
        هيّئ_الأنواع();
        هيّئ_الدوال_المدمجة();
    }
    
    /**
     * تحويل كود Python إلى ص
     */
    std::string حوّل(const std::string& كود_بايثون) {
        std::string نتيجة;
        
        // تقسيم إلى أسطر
        std::vector<std::string> سطور;
        std::string سطر_حالي;
        for (char c : كود_بايثون) {
            if (c == '\n') {
                سطور.push_back(سطر_حالي);
                سطر_حالي.clear();
            } else {
                سطر_حالي += c;
            }
        }
        if (!سطر_حالي.empty()) {
            سطور.push_back(سطر_حالي);
        }
        
        // تتبع الإزاحة
        std::vector<int> مستويات_الإزاحة;
        مستويات_الإزاحة.push_back(0);
        
        for (size_t i = 0; i < سطور.size(); i++) {
            std::string& سطر = سطور[i];
            
            // حساب الإزاحة
            int إزاحة_حالية = 0;
            while (إزاحة_حالية < (int)سطر.length() && 
                   (سطر[إزاحة_حالية] == ' ' || سطر[إزاحة_حالية] == '\t')) {
                إزاحة_حالية += (سطر[إزاحة_حالية] == '\t') ? 4 : 1;
            }
            
            // إغلاق الكتل عند تقليل الإزاحة
            while (مستويات_الإزاحة.size() > 1 && إزاحة_حالية < مستويات_الإزاحة.back()) {
                مستويات_الإزاحة.pop_back();
                نتيجة += std::string(مستويات_الإزاحة.back(), ' ') + "نهاية\n";
            }
            
            // تحويل السطر
            std::string سطر_محول = حوّل_سطر(سطر);
            
            // تحقق من فتح كتلة جديدة
            if (سطر.find(':') != std::string::npos && 
                (سطر.find("def ") != std::string::npos ||
                 سطر.find("class ") != std::string::npos ||
                 سطر.find("if ") != std::string::npos ||
                 سطر.find("elif ") != std::string::npos ||
                 سطر.find("else:") != std::string::npos ||
                 سطر.find("for ") != std::string::npos ||
                 سطر.find("while ") != std::string::npos ||
                 سطر.find("try:") != std::string::npos ||
                 سطر.find("except") != std::string::npos)) {
                
                if (i + 1 < سطور.size()) {
                    int إزاحة_تالية = 0;
                    std::string& سطر_تالي = سطور[i + 1];
                    while (إزاحة_تالية < (int)سطر_تالي.length() && 
                           (سطر_تالي[إزاحة_تالية] == ' ' || سطر_تالي[إزاحة_تالية] == '\t')) {
                        إزاحة_تالية += (سطر_تالي[إزاحة_تالية] == '\t') ? 4 : 1;
                    }
                    if (إزاحة_تالية > إزاحة_حالية) {
                        مستويات_الإزاحة.push_back(إزاحة_تالية);
                    }
                }
            }
            
            نتيجة += سطر_محول + "\n";
        }
        
        // إغلاق أي كتل متبقية
        while (مستويات_الإزاحة.size() > 1) {
            مستويات_الإزاحة.pop_back();
            نتيجة += std::string(مستويات_الإزاحة.back(), ' ') + "نهاية\n";
        }
        
        return نتيجة;
    }
    
private:
    void هيّئ_الكلمات_المفتاحية() {
        كلمات_مفتاحية_ = {
            {"def ", "دالة "},
            {"class ", "صنف "},
            {"if ", "إذا "},
            {"elif ", "وإلا إذا "},
            {"else:", "وإلا"},
            {"for ", "لكل "},
            {"while ", "طالما "},
            {"return ", "أرجع "},
            {"import ", "استورد "},
            {"from ", "من "},
            {"try:", "حاول"},
            {"except ", "التقط "},
            {"finally:", "أخيراً"},
            {"raise ", "ارمِ "},
            {"pass", "مرر"},
            {"break", "اكسر"},
            {"continue", "واصل"},
            {"True", "صحيح"},
            {"False", "خطأ"},
            {"None", "فارغ"},
            {"and ", "و "},
            {"or ", "أو "},
            {"not ", "ليس "},
            {"in ", "في "},
            {"is ", "هو "},
            {"lambda ", "دالة_مختصرة "},
            {"with ", "مع "},
            {"as ", "كـ "},
            {"global ", "عام "},
            {"async ", "غير_متزامن "},
            {"await ", "انتظر "}
        };
    }
    
    void هيّئ_الأنواع() {
        أنواع_ = {
            {"int", "صحيح"},
            {"float", "عشري"},
            {"str", "نص"},
            {"bool", "منطقي"},
            {"list", "قائمة"},
            {"dict", "قاموس"},
            {"tuple", "صف"},
            {"set", "مجموعة"},
            {"bytes", "بايتات"},
            {"object", "كائن"}
        };
    }
    
    void هيّئ_الدوال_المدمجة() {
        دوال_مدمجة_ = {
            {"print(", "اطبع("},
            {"input(", "أدخل("},
            {"len(", "طول("},
            {"range(", "نطاق("},
            {"str(", "نص("},
            {"int(", "صحيح("},
            {"float(", "عشري("},
            {"list(", "قائمة("},
            {"dict(", "قاموس("},
            {"open(", "افتح("},
            {"read(", "اقرأ("},
            {"write(", "اكتب("},
            {"append(", "أضف("},
            {"remove(", "احذف("},
            {"sort(", "رتّب("},
            {"reverse(", "اعكس("},
            {"split(", "قسّم("},
            {"join(", "اجمع("},
            {"strip(", "نظّف("},
            {"lower(", "صغّر("},
            {"upper(", "كبّر("},
            {"replace(", "استبدل("},
            {"find(", "جد("},
            {"format(", "نسّق("},
            {"type(", "نوع("},
            {"isinstance(", "هل_نوع("},
            {"enumerate(", "عدّد("},
            {"zip(", "ازوج("},
            {"map(", "طبّق("},
            {"filter(", "صفّي("},
            {"sum(", "مجموع("},
            {"min(", "أصغر("},
            {"max(", "أكبر("},
            {"abs(", "قيمة_مطلقة("},
            {"round(", "قرّب("}
        };
    }
    
    std::string حوّل_سطر(const std::string& سطر) {
        std::string نتيجة = سطر;
        
        // تحويل الكلمات المفتاحية
        for (const auto& [بايثون, ص] : كلمات_مفتاحية_) {
            size_t موقع = 0;
            while ((موقع = نتيجة.find(بايثون, موقع)) != std::string::npos) {
                نتيجة.replace(موقع, بايثون.length(), ص);
                موقع += ص.length();
            }
        }
        
        // تحويل الدوال المدمجة
        for (const auto& [بايثون, ص] : دوال_مدمجة_) {
            size_t موقع = 0;
            while ((موقع = نتيجة.find(بايثون, موقع)) != std::string::npos) {
                نتيجة.replace(موقع, بايثون.length(), ص);
                موقع += ص.length();
            }
        }
        
        // تحويل الأنواع
        for (const auto& [بايثون, ص] : أنواع_) {
            // في تعليقات الأنواع
            std::string نمط_نوع = ": " + بايثون;
            std::string بديل_نوع = ": " + ص;
            size_t موقع = 0;
            while ((موقع = نتيجة.find(نمط_نوع, موقع)) != std::string::npos) {
                نتيجة.replace(موقع, نمط_نوع.length(), بديل_نوع);
                موقع += بديل_نوع.length();
            }
        }
        
        // تحويل := (walrus operator)
        size_t موقع_والروس = نتيجة.find(":=");
        if (موقع_والروس != std::string::npos) {
            نتيجة.replace(موقع_والروس, 2, "=");
        }
        
        // إزالة النقطتين من نهاية الكتل
        if (!نتيجة.empty() && نتيجة.back() == ':') {
            نتيجة.pop_back();
            نتيجة += " ثم";  // إضافة "ثم" بدلاً من :
        }
        
        // تحويل self إلى ذات
        size_t موقع_ذات = 0;
        while ((موقع_ذات = نتيجة.find("self.", موقع_ذات)) != std::string::npos) {
            نتيجة.replace(موقع_ذات, 5, "ذات.");
            موقع_ذات += 4;
        }
        
        // تحويل __init__ إلى منشئ
        size_t موقع_منشئ = نتيجة.find("__init__");
        if (موقع_منشئ != std::string::npos) {
            نتيجة.replace(موقع_منشئ, 8, "منشئ");
        }
        
        return نتيجة;
    }
};

} // namespace translator
} // namespace sad

extern "C" {
using namespace sad::translator;

void* sad_py_to_sad_create() {
    return new محوّل_بايثون_إلى_ص();
}

void sad_py_to_sad_destroy(void* converter) {
    delete static_cast<محوّل_بايثون_إلى_ص*>(converter);
}

const char* sad_py_to_sad_convert(void* converter, const char* python_code) {
    static std::string نتيجة;
    نتيجة = static_cast<محوّل_بايثون_إلى_ص*>(converter)->حوّل(python_code);
    return نتيجة.c_str();
}
}
