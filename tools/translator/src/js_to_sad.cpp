/**
 * ==========================================================
 * ملف: js_to_sad.cpp
 * الوصف: تحويل JavaScript إلى كود ص
 * المهمة: T388 - ترجمة ص
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
 * قواعد التحويل JavaScript → ص
 */
class محوّل_جافاسكربت_إلى_ص {
private:
    std::map<std::string, std::string> كلمات_مفتاحية_;
    std::map<std::string, std::string> دوال_مدمجة_;
    
public:
    محوّل_جافاسكربت_إلى_ص() {
        هيّئ_الكلمات_المفتاحية();
        هيّئ_الدوال_المدمجة();
    }
    
    /**
     * تحويل كود JavaScript إلى ص
     */
    std::string حوّل(const std::string& كود_جافاسكربت) {
        std::string نتيجة;
        
        // تقسيم إلى أسطر
        std::vector<std::string> سطور;
        std::string سطر_حالي;
        for (char c : كود_جافاسكربت) {
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
        
        // معالجة كل سطر
        for (const auto& سطر : سطور) {
            std::string سطر_محول = حوّل_سطر(سطر);
            نتيجة += سطر_محول + "\n";
        }
        
        // تحويل الأقواس {} إلى نهاية
        نتيجة = حوّل_الأقواس(نتيجة);
        
        return نتيجة;
    }
    
private:
    void هيّئ_الكلمات_المفتاحية() {
        كلمات_مفتاحية_ = {
            {"function ", "دالة "},
            {"async function ", "دالة غير_متزامنة "},
            {"class ", "صنف "},
            {"const ", "ثابت "},
            {"let ", "متغير "},
            {"var ", "متغير "},
            {"if (", "إذا "},
            {"else if (", "وإلا إذا "},
            {"else {", "وإلا"},
            {"for (", "لكل "},
            {"while (", "طالما "},
            {"return ", "أرجع "},
            {"import ", "استورد "},
            {"export ", "صدّر "},
            {"from ", "من "},
            {"try {", "حاول"},
            {"catch (", "التقط "},
            {"finally {", "أخيراً"},
            {"throw ", "ارمِ "},
            {"new ", "جديد "},
            {"true", "صحيح"},
            {"false", "خطأ"},
            {"null", "فارغ"},
            {"undefined", "غير_معرف"},
            {"await ", "انتظر "},
            {"async ", "غير_متزامن "},
            {"this.", "ذات."},
            {"typeof ", "نوع "},
            {"instanceof ", "مثيل_من "},
            {"&&", "و"},
            {"||", "أو"},
            {"===", "=="},
            {"!==", "!="}
        };
    }
    
    void هيّئ_الدوال_المدمجة() {
        دوال_مدمجة_ = {
            {"console.log(", "اطبع("},
            {"console.error(", "اطبع_خطأ("},
            {"console.warn(", "اطبع_تحذير("},
            {"prompt(", "أدخل("},
            {"alert(", "نبّه("},
            {".length", ".طول"},
            {".push(", ".أضف("},
            {".pop(", ".احذف_آخر("},
            {".shift(", ".احذف_أول("},
            {".unshift(", ".أضف_أول("},
            {".splice(", ".قصّ("},
            {".slice(", ".شريحة("},
            {".concat(", ".اجمع("},
            {".join(", ".صل("},
            {".split(", ".قسّم("},
            {".trim(", ".نظّف("},
            {".toLowerCase(", ".صغّر("},
            {".toUpperCase(", ".كبّر("},
            {".indexOf(", ".فهرس("},
            {".includes(", ".يحتوي("},
            {".startsWith(", ".يبدأ_بـ("},
            {".endsWith(", ".ينتهي_بـ("},
            {".replace(", ".استبدل("},
            {".match(", ".طابق("},
            {".map(", ".طبّق("},
            {".filter(", ".صفّي("},
            {".reduce(", ".اختزل("},
            {".forEach(", ".لكل("},
            {".find(", ".جد("},
            {".findIndex(", ".جد_فهرس("},
            {".sort(", ".رتّب("},
            {".reverse(", ".اعكس("},
            {"Math.abs(", "قيمة_مطلقة("},
            {"Math.floor(", "أرضية("},
            {"Math.ceil(", "سقف("},
            {"Math.round(", "قرّب("},
            {"Math.random(", "عشوائي("},
            {"Math.max(", "أكبر("},
            {"Math.min(", "أصغر("},
            {"Math.sqrt(", "جذر("},
            {"Math.pow(", "أس("},
            {"JSON.parse(", "حلل_json("},
            {"JSON.stringify(", "نص_json("},
            {"parseInt(", "صحيح("},
            {"parseFloat(", "عشري("},
            {"String(", "نص("},
            {"Number(", "رقم("},
            {"Boolean(", "منطقي("},
            {"Array.isArray(", "هل_قائمة("},
            {"Object.keys(", "مفاتيح("},
            {"Object.values(", "قيم("},
            {"Object.entries(", "مدخلات("},
            {"setTimeout(", "بعد_مهلة("},
            {"setInterval(", "كل_فترة("},
            {"fetch(", "جلب("},
            {".then(", ".ثم("},
            {".catch(", ".التقط("},
            {".finally(", ".أخيراً("}
        };
    }
    
    std::string حوّل_سطر(const std::string& سطر) {
        std::string نتيجة = سطر;
        
        // تحويل الكلمات المفتاحية
        for (const auto& [جافا, ص] : كلمات_مفتاحية_) {
            size_t موقع = 0;
            while ((موقع = نتيجة.find(جافا, موقع)) != std::string::npos) {
                نتيجة.replace(موقع, جافا.length(), ص);
                موقع += ص.length();
            }
        }
        
        // تحويل الدوال المدمجة
        for (const auto& [جافا, ص] : دوال_مدمجة_) {
            size_t موقع = 0;
            while ((موقع = نتيجة.find(جافا, موقع)) != std::string::npos) {
                نتيجة.replace(موقع, جافا.length(), ص);
                موقع += ص.length();
            }
        }
        
        // إزالة الفاصلة المنقوطة
        if (!نتيجة.empty() && نتيجة.back() == ';') {
            نتيجة.pop_back();
        }
        
        // تحويل arrow functions => إلى ->
        size_t موقع_سهم = نتيجة.find("=>");
        if (موقع_سهم != std::string::npos) {
            نتيجة.replace(موقع_سهم, 2, "->");
        }
        
        // تحويل template literals
        نتيجة = حوّل_نصوص_قالبية(نتيجة);
        
        return نتيجة;
    }
    
    std::string حوّل_نصوص_قالبية(const std::string& سطر) {
        std::string نتيجة = سطر;
        
        // تحويل `text ${expr}` إلى "text " + expr
        bool في_قالب = false;
        bool في_تعبير = false;
        std::string نص_جديد;
        
        for (size_t i = 0; i < نتيجة.length(); i++) {
            char c = نتيجة[i];
            
            if (c == '`') {
                في_قالب = !في_قالب;
                نص_جديد += '"';
            } else if (في_قالب && c == '$' && i + 1 < نتيجة.length() && نتيجة[i + 1] == '{') {
                في_تعبير = true;
                نص_جديد += "\" + ";
                i++;  // تخطي {
            } else if (في_تعبير && c == '}') {
                في_تعبير = false;
                نص_جديد += " + \"";
            } else {
                نص_جديد += c;
            }
        }
        
        return نص_جديد;
    }
    
    std::string حوّل_الأقواس(const std::string& كود) {
        std::string نتيجة;
        int مستوى = 0;
        
        for (size_t i = 0; i < كود.length(); i++) {
            char c = كود[i];
            
            if (c == '{') {
                // تحويل { إلى "ثم" إذا كانت بعد شرط
                مستوى++;
                // تجاهل القوس
            } else if (c == '}') {
                مستوى--;
                // إضافة "نهاية" بدل }
                نتيجة += std::string(مستوى * 4, ' ') + "نهاية";
            } else {
                نتيجة += c;
            }
        }
        
        return نتيجة;
    }
};

} // namespace translator
} // namespace sad

extern "C" {
using namespace sad::translator;

void* sad_js_to_sad_create() {
    return new محوّل_جافاسكربت_إلى_ص();
}

void sad_js_to_sad_destroy(void* converter) {
    delete static_cast<محوّل_جافاسكربت_إلى_ص*>(converter);
}

const char* sad_js_to_sad_convert(void* converter, const char* js_code) {
    static std::string نتيجة;
    نتيجة = static_cast<محوّل_جافاسكربت_إلى_ص*>(converter)->حوّل(js_code);
    return نتيجة.c_str();
}
}
