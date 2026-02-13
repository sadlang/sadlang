/**
 * ==========================================================
 * ملف: companion_lsp.cpp
 * الوصف: تكامل الرفيق مع LSP
 * المهمة: T374 - رفيق ص
 * ==========================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace sad {
namespace lsp {

/**
 * نوع إجراء الرفيق
 */
enum class نوع_إجراء_رفيق {
    اقتراح_كود,
    تحسين_كود,
    شرح_كود,
    توثيق_آلي,
    تصحيح_خطأ,
    نصيحة
};

/**
 * إجراء الرفيق
 */
struct إجراء_رفيق {
    نوع_إجراء_رفيق نوع;
    std::string عنوان;
    std::string وصف;
    std::string تعديل;
    int سطر_البداية;
    int سطر_النهاية;
};

/**
 * طلب الرفيق
 */
struct طلب_رفيق {
    std::string محتوى_الملف;
    std::string مسار_الملف;
    int سطر_المؤشر;
    int عمود_المؤشر;
    std::string نص_محدد;
};

/**
 * خادم LSP للرفيق
 */
class خادم_رفيق_LSP {
private:
    std::map<std::string, std::string> أنماط_المستخدم_;
    bool مفعّل_ = true;
    
public:
    /**
     * معالجة طلب الإكمال
     */
    std::vector<std::string> أكمل(const طلب_رفيق& طلب) {
        if (!مفعّل_) return {};
        
        std::vector<std::string> اقتراحات;
        
        // تحليل السياق
        std::string سطر = استخرج_السطر(طلب.محتوى_الملف, طلب.سطر_المؤشر);
        
        // اقتراحات حسب السياق
        if (سطر.find("دالة ") != std::string::npos) {
            اقتراحات.push_back("() -> نوع نهاية");
            اقتراحات.push_back("(معامل: نوع) -> نوع نهاية");
        }
        
        if (سطر.find("إذا ") != std::string::npos) {
            اقتراحات.push_back(" ثم\n    \nنهاية");
            اقتراحات.push_back(" ثم\n    \nوإلا\n    \nنهاية");
        }
        
        if (سطر.find("لكل ") != std::string::npos) {
            اقتراحات.push_back(" من قائمة افعل\n    \nنهاية");
        }
        
        if (سطر.find("صنف ") != std::string::npos) {
            اقتراحات.push_back("\n    // الخصائص\n    \n    // المنشئ\n    منشئ() نهاية\nنهاية");
        }
        
        return اقتراحات;
    }
    
    /**
     * اقتراح إجراءات الكود
     */
    std::vector<إجراء_رفيق> اقترح_إجراءات(const طلب_رفيق& طلب) {
        std::vector<إجراء_رفيق> إجراءات;
        
        // تحليل الكود المحدد
        if (!طلب.نص_محدد.empty()) {
            // اقتراح تحويل لدالة
            if (طلب.نص_محدد.length() > 50) {
                إجراءات.push_back({
                    .نوع = نوع_إجراء_رفيق::تحسين_كود,
                    .عنوان = "استخراج كدالة",
                    .وصف = "استخراج الكود المحدد كدالة منفصلة",
                    .تعديل = "",
                    .سطر_البداية = طلب.سطر_المؤشر,
                    .سطر_النهاية = طلب.سطر_المؤشر
                });
            }
            
            // اقتراح شرح
            إجراءات.push_back({
                .نوع = نوع_إجراء_رفيق::شرح_كود,
                .عنوان = "اشرح هذا الكود",
                .وصف = "الحصول على شرح مفصل للكود المحدد",
                .تعديل = "",
                .سطر_البداية = طلب.سطر_المؤشر,
                .سطر_النهاية = طلب.سطر_المؤشر
            });
        }
        
        // اقتراح توثيق للدوال
        std::string سطر = استخرج_السطر(طلب.محتوى_الملف, طلب.سطر_المؤشر);
        if (سطر.find("دالة ") != std::string::npos) {
            إجراءات.push_back({
                .نوع = نوع_إجراء_رفيق::توثيق_آلي,
                .عنوان = "أضف توثيق الدالة",
                .وصف = "إنشاء تعليق توثيقي للدالة",
                .تعديل = "/**\n * وصف الدالة\n */\n",
                .سطر_البداية = طلب.سطر_المؤشر,
                .سطر_النهاية = طلب.سطر_المؤشر
            });
        }
        
        return إجراءات;
    }
    
    /**
     * نصيحة سياقية
     */
    std::string نصيحة(const طلب_رفيق& طلب) {
        std::string سطر = استخرج_السطر(طلب.محتوى_الملف, طلب.سطر_المؤشر);
        
        // نصائح حسب السياق
        if (سطر.find("// TODO") != std::string::npos) {
            return "💡 لديك مهمة معلقة هنا. هل تريد إنجازها الآن؟";
        }
        
        if (سطر.find("متغير ") != std::string::npos && سطر.find("=") == std::string::npos) {
            return "💡 المتغير غير مهيأ. تأكد من إعطائه قيمة.";
        }
        
        return "";
    }
    
    /**
     * تفعيل/تعطيل
     */
    void فعّل(bool حالة) {
        مفعّل_ = حالة;
    }
    
private:
    std::string استخرج_السطر(const std::string& محتوى, int سطر) {
        std::vector<std::string> سطور;
        std::string سطر_حالي;
        
        for (char c : محتوى) {
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
        
        if (سطر >= 0 && سطر < (int)سطور.size()) {
            return سطور[سطر];
        }
        return "";
    }
};

} // namespace lsp
} // namespace sad

extern "C" {
using namespace sad::lsp;

void* sad_companion_lsp_create() {
    return new خادم_رفيق_LSP();
}

void sad_companion_lsp_destroy(void* lsp) {
    delete static_cast<خادم_رفيق_LSP*>(lsp);
}

void sad_companion_lsp_enable(void* lsp, int enabled) {
    static_cast<خادم_رفيق_LSP*>(lsp)->فعّل(enabled != 0);
}

const char* sad_companion_lsp_tip(void* lsp, const char* content, int line) {
    static std::string نتيجة;
    طلب_رفيق طلب;
    طلب.محتوى_الملف = content;
    طلب.سطر_المؤشر = line;
    نتيجة = static_cast<خادم_رفيق_LSP*>(lsp)->نصيحة(طلب);
    return نتيجة.c_str();
}
}
