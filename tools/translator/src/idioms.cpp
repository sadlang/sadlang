/**
 * ==========================================================
 * ملف: idioms.cpp
 * الوصف: تحويل الأنماط الاصطلاحية
 * المهمة: T391 - ترجمة ص
 * ==========================================================
 * 
 * يحول الأنماط الشائعة في Python/JS إلى نظيراتها في ص
 * مثل: list comprehension → فهم القائمة
 * ==========================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

namespace sad {
namespace translator {

/**
 * نمط اصطلاحي
 */
struct نمط_اصطلاحي {
    std::string اسم;
    std::regex نمط_مصدر;
    std::string قالب_هدف;
    std::string وصف;
};

/**
 * محوّل الأنماط الاصطلاحية
 */
class محوّل_الأنماط {
private:
    std::vector<نمط_اصطلاحي> أنماط_بايثون_;
    std::vector<نمط_اصطلاحي> أنماط_جافاسكربت_;
    
public:
    محوّل_الأنماط() {
        هيّئ_أنماط_بايثون();
        هيّئ_أنماط_جافاسكربت();
    }
    
    /**
     * تحويل أنماط Python
     */
    std::string حوّل_بايثون(const std::string& كود) {
        std::string نتيجة = كود;
        
        for (const auto& نمط : أنماط_بايثون_) {
            نتيجة = std::regex_replace(نتيجة, نمط.نمط_مصدر, نمط.قالب_هدف);
        }
        
        return نتيجة;
    }
    
    /**
     * تحويل أنماط JavaScript
     */
    std::string حوّل_جافاسكربت(const std::string& كود) {
        std::string نتيجة = كود;
        
        for (const auto& نمط : أنماط_جافاسكربت_) {
            نتيجة = std::regex_replace(نتيجة, نمط.نمط_مصدر, نمط.قالب_هدف);
        }
        
        return نتيجة;
    }
    
    /**
     * قائمة التحويلات
     */
    std::string اعرض_التحويلات() {
        std::string نتيجة;
        نتيجة += "═══════════════════════════════════════════════════\n";
        نتيجة += "📋 تحويلات الأنماط الاصطلاحية\n";
        نتيجة += "═══════════════════════════════════════════════════\n\n";
        
        نتيجة += "🐍 Python → ص:\n";
        for (const auto& نمط : أنماط_بايثون_) {
            نتيجة += "  • " + نمط.اسم + ": " + نمط.وصف + "\n";
        }
        
        نتيجة += "\n📜 JavaScript → ص:\n";
        for (const auto& نمط : أنماط_جافاسكربت_) {
            نتيجة += "  • " + نمط.اسم + ": " + نمط.وصف + "\n";
        }
        
        return نتيجة;
    }
    
private:
    void هيّئ_أنماط_بايثون() {
        أنماط_بايثون_ = {
            // List Comprehension
            {
                .اسم = "فهم القائمة",
                .نمط_مصدر = std::regex(R"(\[(\w+)\s+for\s+(\w+)\s+in\s+(\w+)\])"),
                .قالب_هدف = "[$3.طبّق(دالة($2) -> $1 نهاية)]",
                .وصف = "[x for x in list] → list.طبّق()"
            },
            // List Comprehension with condition
            {
                .اسم = "فهم القائمة مع شرط",
                .نمط_مصدر = std::regex(R"(\[(\w+)\s+for\s+(\w+)\s+in\s+(\w+)\s+if\s+(.+?)\])"),
                .قالب_هدف = "[$3.صفّي(دالة($2) -> $4 نهاية).طبّق(دالة($2) -> $1 نهاية)]",
                .وصف = "[x for x in list if cond] → filter + map"
            },
            // Dictionary Comprehension
            {
                .اسم = "فهم القاموس",
                .نمط_مصدر = std::regex(R"(\{(\w+):\s*(\w+)\s+for\s+(\w+)\s+in\s+(\w+)\})"),
                .قالب_هدف = "قاموس.من_قائمة($4.طبّق(دالة($3) -> ($1, $2) نهاية))",
                .وصف = "{k: v for x in list} → قاموس.من_قائمة()"
            },
            // f-string
            {
                .اسم = "نص منسق",
                .نمط_مصدر = std::regex(R"(f\"(.+?)\{(\w+)\}(.+?)\")"),
                .قالب_هدف = "\"$1\" + نص($2) + \"$3\"",
                .وصف = "f\"text {var}\" → \"text\" + var"
            },
            // with statement
            {
                .اسم = "with للموارد",
                .نمط_مصدر = std::regex(R"(with\s+open\((.+?)\)\s+as\s+(\w+):)"),
                .قالب_هدف = "استخدم $2 = افتح($1)",
                .وصف = "with open() as f → استخدم"
            },
            // enumerate
            {
                .اسم = "enumerate",
                .نمط_مصدر = std::regex(R"(for\s+(\w+),\s*(\w+)\s+in\s+enumerate\((\w+)\))"),
                .قالب_هدف = "لكل $1, $2 من $3.عدّد()",
                .وصف = "for i, x in enumerate() → لكل مع فهرس"
            },
            // zip
            {
                .اسم = "zip",
                .نمط_مصدر = std::regex(R"(for\s+(\w+),\s*(\w+)\s+in\s+zip\((\w+),\s*(\w+)\))"),
                .قالب_هدف = "لكل $1, $2 من ازوج($3, $4)",
                .وصف = "for a, b in zip() → لكل من ازوج()"
            },
            // lambda
            {
                .اسم = "lambda",
                .نمط_مصدر = std::regex(R"(lambda\s+(\w+):\s*(.+))"),
                .قالب_هدف = "دالة($1) -> $2 نهاية",
                .وصف = "lambda x: expr → دالة مختصرة"
            },
            // ternary operator
            {
                .اسم = "العامل الثلاثي",
                .نمط_مصدر = std::regex(R"((\w+)\s+if\s+(.+?)\s+else\s+(.+))"),
                .قالب_هدف = "إذا $2 ثم $1 وإلا $3",
                .وصف = "a if cond else b → إذا ثم وإلا"
            }
        };
    }
    
    void هيّئ_أنماط_جافاسكربت() {
        أنماط_جافاسكربت_ = {
            // Arrow function
            {
                .اسم = "دالة السهم",
                .نمط_مصدر = std::regex(R"(\((\w*)\)\s*=>\s*(.+))"),
                .قالب_هدف = "دالة($1) -> $2 نهاية",
                .وصف = "(x) => expr → دالة(x) -> expr"
            },
            // Template literals
            {
                .اسم = "نص قالبي",
                .نمط_مصدر = std::regex(R"(`(.+?)\$\{(\w+)\}(.+?)`)"),
                .قالب_هدف = "\"$1\" + نص($2) + \"$3\"",
                .وصف = "`text ${var}` → \"text\" + var"
            },
            // Destructuring array
            {
                .اسم = "تفكيك المصفوفة",
                .نمط_مصدر = std::regex(R"((const|let)\s+\[(\w+),\s*(\w+)\]\s*=\s*(\w+))"),
                .قالب_هدف = "متغير $2, $3 = $4.فكّك()",
                .وصف = "const [a, b] = arr → تفكيك"
            },
            // Destructuring object
            {
                .اسم = "تفكيك الكائن",
                .نمط_مصدر = std::regex(R"((const|let)\s+\{(\w+),\s*(\w+)\}\s*=\s*(\w+))"),
                .قالب_هدف = "متغير $2 = $4.$2\nمتغير $3 = $4.$3",
                .وصف = "const {a, b} = obj → تفكيك"
            },
            // Spread operator (array)
            {
                .اسم = "عامل النشر",
                .نمط_مصدر = std::regex(R"(\[\.\.\.(\w+),\s*(.+?)\])"),
                .قالب_هدف = "$1.انشر() + [$2]",
                .وصف = "[...arr, x] → arr.انشر() + [x]"
            },
            // Optional chaining
            {
                .اسم = "السلسلة الاختيارية",
                .نمط_مصدر = std::regex(R"((\w+)\?\.\s*(\w+))"),
                .قالب_هدف = "$1؟.$2",
                .وصف = "obj?.prop → obj؟.prop"
            },
            // Nullish coalescing
            {
                .اسم = "الاندماج الفارغ",
                .نمط_مصدر = std::regex(R"((\w+)\s*\?\?\s*(.+))"),
                .قالب_هدف = "$1 ?? $2",
                .وصف = "a ?? b → a إذا_غير_فارغ b"
            },
            // Promise.all
            {
                .اسم = "Promise.all",
                .نمط_مصدر = std::regex(R"(Promise\.all\((.+?)\))"),
                .قالب_هدف = "انتظر_الكل($1)",
                .وصف = "Promise.all() → انتظر_الكل()"
            },
            // async/await
            {
                .اسم = "async/await",
                .نمط_مصدر = std::regex(R"(async\s+function\s+(\w+)\((.+?)\))"),
                .قالب_هدف = "دالة غير_متزامنة $1($2)",
                .وصف = "async function → دالة غير_متزامنة"
            },
            // for...of
            {
                .اسم = "for...of",
                .نمط_مصدر = std::regex(R"(for\s*\(\s*(const|let)\s+(\w+)\s+of\s+(\w+)\s*\))"),
                .قالب_هدف = "لكل $2 من $3 افعل",
                .وصف = "for (x of arr) → لكل من افعل"
            },
            // map
            {
                .اسم = "map",
                .نمط_مصدر = std::regex(R"((\w+)\.map\(\s*(\w+)\s*=>\s*(.+?)\))"),
                .قالب_هدف = "$1.طبّق(دالة($2) -> $3 نهاية)",
                .وصف = "arr.map(x => f) → arr.طبّق()"
            },
            // filter
            {
                .اسم = "filter",
                .نمط_مصدر = std::regex(R"((\w+)\.filter\(\s*(\w+)\s*=>\s*(.+?)\))"),
                .قالب_هدف = "$1.صفّي(دالة($2) -> $3 نهاية)",
                .وصف = "arr.filter(x => f) → arr.صفّي()"
            },
            // reduce
            {
                .اسم = "reduce",
                .نمط_مصدر = std::regex(R"((\w+)\.reduce\(\s*\((\w+),\s*(\w+)\)\s*=>\s*(.+?),\s*(.+?)\))"),
                .قالب_هدف = "$1.اختزل(دالة($2, $3) -> $4 نهاية, $5)",
                .وصف = "arr.reduce() → arr.اختزل()"
            }
        };
    }
};

} // namespace translator
} // namespace sad

extern "C" {
using namespace sad::translator;

void* sad_idioms_create() {
    return new محوّل_الأنماط();
}

void sad_idioms_destroy(void* idioms) {
    delete static_cast<محوّل_الأنماط*>(idioms);
}

const char* sad_idioms_convert_python(void* idioms, const char* code) {
    static std::string نتيجة;
    نتيجة = static_cast<محوّل_الأنماط*>(idioms)->حوّل_بايثون(code);
    return نتيجة.c_str();
}

const char* sad_idioms_convert_js(void* idioms, const char* code) {
    static std::string نتيجة;
    نتيجة = static_cast<محوّل_الأنماط*>(idioms)->حوّل_جافاسكربت(code);
    return نتيجة.c_str();
}

const char* sad_idioms_list(void* idioms) {
    static std::string نتيجة;
    نتيجة = static_cast<محوّل_الأنماط*>(idioms)->اعرض_التحويلات();
    return نتيجة.c_str();
}
}
