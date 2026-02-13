/**
 * ==========================================================
 * ملف: suggestions.cpp
 * الوصف: اقتراحات وتحذيرات الترجمة
 * المهمة: T393 - ترجمة ص
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
 * شدة الاقتراح
 */
enum class شدة_اقتراح {
    معلومة,
    تحذير,
    خطأ
};

/**
 * اقتراح ترجمة
 */
struct اقتراح_ترجمة {
    شدة_اقتراح شدة;
    int سطر;
    std::string رسالة;
    std::string اقتراح;
    std::string كود_أصلي;
    std::string كود_مقترح;
};

/**
 * مولد الاقتراحات
 */
class مولّد_اقتراحات_الترجمة {
public:
    /**
     * تحليل كود Python وتوليد اقتراحات
     */
    std::vector<اقتراح_ترجمة> حلل_بايثون(const std::string& كود) {
        std::vector<اقتراح_ترجمة> اقتراحات;
        
        // البحث عن أنماط تحتاج اهتمام
        
        // 1. استخدام eval
        if (كود.find("eval(") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::خطأ,
                .سطر = جد_سطر(كود, "eval("),
                .رسالة = "استخدام eval() خطير ولا يُدعم في ص",
                .اقتراح = "أعد كتابة المنطق بدون eval",
                .كود_أصلي = "eval(code)",
                .كود_مقترح = "// استخدم منطق مباشر بدلاً من eval"
            });
        }
        
        // 2. استخدام exec
        if (كود.find("exec(") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::خطأ,
                .سطر = جد_سطر(كود, "exec("),
                .رسالة = "استخدام exec() خطير ولا يُدعم في ص",
                .اقتراح = "أعد كتابة المنطق بدون exec",
                .كود_أصلي = "exec(code)",
                .كود_مقترح = "// استخدم دوال محددة بدلاً من exec"
            });
        }
        
        // 3. global variables
        if (كود.find("global ") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::تحذير,
                .سطر = جد_سطر(كود, "global "),
                .رسالة = "استخدام المتغيرات العامة غير مستحسن",
                .اقتراح = "استخدم معاملات الدالة بدلاً من global",
                .كود_أصلي = "global x",
                .كود_مقترح = "// مرر x كمعامل للدالة"
            });
        }
        
        // 4. mutable default arguments
        std::regex نمط_معامل_متحول(R"(def\s+\w+\(.+?=\s*\[\])");
        if (std::regex_search(كود, نمط_معامل_متحول)) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::تحذير,
                .سطر = 1,
                .رسالة = "استخدام قائمة فارغة كقيمة افتراضية خطير",
                .اقتراح = "استخدم None وتحقق داخل الدالة",
                .كود_أصلي = "def f(x=[]):",
                .كود_مقترح = "دالة f(x: قائمة? = فارغ) ثم\n    إذا x == فارغ ثم\n        x = []\n    نهاية\nنهاية"
            });
        }
        
        // 5. bare except
        if (كود.find("except:") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::تحذير,
                .سطر = جد_سطر(كود, "except:"),
                .رسالة = "التقاط كل الأخطاء بدون تحديد غير مستحسن",
                .اقتراح = "حدد نوع الخطأ المتوقع",
                .كود_أصلي = "except:",
                .كود_مقترح = "التقط خطأ: استثناء ثم"
            });
        }
        
        // 6. type annotations
        std::regex نمط_دالة_بدون_نوع(R"(def\s+(\w+)\s*\(([^)]*)\)\s*:)");
        std::smatch تطابق;
        if (std::regex_search(كود, تطابق, نمط_دالة_بدون_نوع)) {
            std::string معاملات = تطابق[2];
            if (معاملات.find(':') == std::string::npos && !معاملات.empty()) {
                اقتراحات.push_back({
                    .شدة = شدة_اقتراح::معلومة,
                    .سطر = 1,
                    .رسالة = "إضافة تعليقات الأنواع يحسن سلامة الكود",
                    .اقتراح = "أضف أنواع للمعاملات",
                    .كود_أصلي = "def f(x):",
                    .كود_مقترح = "دالة f(x: صحيح) -> صحيح"
                });
            }
        }
        
        return اقتراحات;
    }
    
    /**
     * تحليل كود JavaScript وتوليد اقتراحات
     */
    std::vector<اقتراح_ترجمة> حلل_جافاسكربت(const std::string& كود) {
        std::vector<اقتراح_ترجمة> اقتراحات;
        
        // 1. var بدلاً من const/let
        if (كود.find("var ") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::تحذير,
                .سطر = جد_سطر(كود, "var "),
                .رسالة = "استخدام var قديم ويسبب مشاكل النطاق",
                .اقتراح = "سيتم تحويله لـ 'متغير' لكن تأكد من النطاق",
                .كود_أصلي = "var x = 5",
                .كود_مقترح = "متغير x = 5"
            });
        }
        
        // 2. == بدلاً من ===
        std::regex نمط_مساواة_ضعيفة(R"([^=!]=[^=])");
        // (skip - too many false positives)
        
        // 3. eval
        if (كود.find("eval(") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::خطأ,
                .سطر = جد_سطر(كود, "eval("),
                .رسالة = "استخدام eval() خطير ولا يُدعم في ص",
                .اقتراح = "أعد كتابة المنطق بدون eval",
                .كود_أصلي = "eval(code)",
                .كود_مقترح = "// استخدم JSON.parse أو منطق مباشر"
            });
        }
        
        // 4. new Function
        if (كود.find("new Function(") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::خطأ,
                .سطر = جد_سطر(كود, "new Function("),
                .رسالة = "إنشاء دوال ديناميكياً غير مدعوم",
                .اقتراح = "استخدم دوال محددة مسبقاً",
                .كود_أصلي = "new Function('x', 'return x')",
                .كود_مقترح = "// استخدم دالة عادية"
            });
        }
        
        // 5. callback hell
        int عمق_callbacks = 0;
        size_t موقع = 0;
        while ((موقع = كود.find("function(", موقع)) != std::string::npos) {
            عمق_callbacks++;
            موقع++;
        }
        if (عمق_callbacks > 3) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::معلومة,
                .سطر = 1,
                .رسالة = "يبدو أن هناك callbacks متداخلة كثيراً",
                .اقتراح = "استخدم async/await بدلاً من callbacks",
                .كود_أصلي = "f(function() { g(function() { ... }) })",
                .كود_مقترح = "دالة غير_متزامنة المهمة() ثم\n    انتظر f()\n    انتظر g()\nنهاية"
            });
        }
        
        // 6. typeof == "undefined"
        if (كود.find("typeof") != std::string::npos && كود.find("undefined") != std::string::npos) {
            اقتراحات.push_back({
                .شدة = شدة_اقتراح::معلومة,
                .سطر = جد_سطر(كود, "typeof"),
                .رسالة = "فحص typeof للـ undefined يُترجم للأنواع الاختيارية",
                .اقتراح = "استخدم النوع الاختياري (?) في ص",
                .كود_أصلي = "if (typeof x === 'undefined')",
                .كود_مقترح = "إذا x == فارغ ثم"
            });
        }
        
        return اقتراحات;
    }
    
    /**
     * تنسيق الاقتراحات للعرض
     */
    std::string نسّق(const std::vector<اقتراح_ترجمة>& اقتراحات) {
        if (اقتراحات.empty()) {
            return "✅ لا توجد اقتراحات - الكود جاهز للترجمة!\n";
        }
        
        std::string نتيجة;
        نتيجة += "═══════════════════════════════════════════════════\n";
        نتيجة += "💡 اقتراحات الترجمة\n";
        نتيجة += "═══════════════════════════════════════════════════\n\n";
        
        int أخطاء = 0, تحذيرات = 0, معلومات = 0;
        
        for (const auto& اق : اقتراحات) {
            std::string رمز;
            switch (اق.شدة) {
                case شدة_اقتراح::خطأ:
                    رمز = "❌";
                    أخطاء++;
                    break;
                case شدة_اقتراح::تحذير:
                    رمز = "⚠️";
                    تحذيرات++;
                    break;
                case شدة_اقتراح::معلومة:
                    رمز = "💡";
                    معلومات++;
                    break;
            }
            
            نتيجة += رمز + " سطر " + std::to_string(اق.سطر) + ": " + اق.رسالة + "\n";
            نتيجة += "   📝 " + اق.اقتراح + "\n";
            نتيجة += "   قبل: " + اق.كود_أصلي + "\n";
            نتيجة += "   بعد: " + اق.كود_مقترح + "\n\n";
        }
        
        نتيجة += "───────────────────────────────────────────────────\n";
        نتيجة += "📊 ملخص: ";
        نتيجة += std::to_string(أخطاء) + " أخطاء، ";
        نتيجة += std::to_string(تحذيرات) + " تحذيرات، ";
        نتيجة += std::to_string(معلومات) + " معلومات\n";
        
        return نتيجة;
    }
    
private:
    int جد_سطر(const std::string& كود, const std::string& نص) {
        size_t موقع = كود.find(نص);
        if (موقع == std::string::npos) return 1;
        
        int سطر = 1;
        for (size_t i = 0; i < موقع; i++) {
            if (كود[i] == '\n') سطر++;
        }
        return سطر;
    }
};

} // namespace translator
} // namespace sad

extern "C" {
using namespace sad::translator;

void* sad_translate_suggestions_create() {
    return new مولّد_اقتراحات_الترجمة();
}

void sad_translate_suggestions_destroy(void* suggestions) {
    delete static_cast<مولّد_اقتراحات_الترجمة*>(suggestions);
}

const char* sad_translate_suggestions_python(void* suggestions, const char* code) {
    static std::string نتيجة;
    auto مولّد = static_cast<مولّد_اقتراحات_الترجمة*>(suggestions);
    auto اقتراحات = مولّد->حلل_بايثون(code);
    نتيجة = مولّد->نسّق(اقتراحات);
    return نتيجة.c_str();
}

const char* sad_translate_suggestions_js(void* suggestions, const char* code) {
    static std::string نتيجة;
    auto مولّد = static_cast<مولّد_اقتراحات_الترجمة*>(suggestions);
    auto اقتراحات = مولّد->حلل_جافاسكربت(code);
    نتيجة = مولّد->نسّق(اقتراحات);
    return نتيجة.c_str();
}
}
