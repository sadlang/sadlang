/**
 * ==========================================================
 * ملف: explainer.cpp
 * الوصف: شرح الكود للمبتدئين
 * المهمة: T303 - نظام ذكاء ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🎓 ما هو شارح الكود؟
 * --------------------
 * برنامج يأخذ كود ويشرحه بلغة بسيطة للمبتدئين.
 * مثل المعلم الذي يشرح لك خطوة بخطوة!
 * 
 * 📝 مثال:
 * --------
 * الكود:
 *   دالة مضروب(ن: عدد) ← عدد
 *       إذا ن <= 1
 *           أرجع 1
 *       نهاية
 *       أرجع ن * مضروب(ن - 1)
 *   نهاية
 * 
 * الشرح:
 *   📌 هذه دالة تحسب "مضروب" العدد
 *   📌 المضروب هو ضرب العدد في كل الأعداد الأصغر منه
 *   📌 مثال: 5! = 5 × 4 × 3 × 2 × 1 = 120
 *   📌 الدالة تستخدم "التكرار" - تنادي نفسها!
 * 
 * 🎯 أنماط الشرح:
 * --------------
 * 1. مبتدئ: شرح كل سطر بالتفصيل
 * 2. متوسط: شرح المفاهيم الأساسية
 * 3. متقدم: شرح الأداء والتحسينات
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace sad {
namespace ai {

// ==========================================================
// 📌 مستوى الشرح
// ==========================================================

/**
 * 📊 مستوى الشرح
 */
enum class ExplanationLevel {
    BEGINNER,       // مبتدئ - شرح كل شيء
    INTERMEDIATE,   // متوسط - المفاهيم الأساسية
    ADVANCED        // متقدم - الأداء والتحسين
};

// ==========================================================
// 📌 عنصر الشرح
// ==========================================================

/**
 * 📝 عنصر شرح
 */
struct ExplanationItem {
    int lineNumber;         // رقم السطر
    std::string code;       // الكود
    std::string explanation;// الشرح
    std::string emoji;      // رمز تعبيري
    std::string tip;        // نصيحة (اختياري)
    
    std::string toString() const {
        std::ostringstream oss;
        oss << emoji << " السطر " << lineNumber << ":\n";
        oss << "   الكود: " << code << "\n";
        oss << "   الشرح: " << explanation << "\n";
        if (!tip.empty()) {
            oss << "   💡 نصيحة: " << tip << "\n";
        }
        return oss.str();
    }
};

// ==========================================================
// 📌 نتيجة الشرح
// ==========================================================

/**
 * 📋 نتيجة الشرح الكاملة
 */
struct ExplanationResult {
    std::string title;                      // عنوان الشرح
    std::string summary;                    // ملخص
    std::vector<ExplanationItem> items;     // عناصر الشرح
    std::vector<std::string> concepts;      // المفاهيم المستخدمة
    std::vector<std::string> suggestions;   // اقتراحات للتعلم
    ExplanationLevel level;                 // مستوى الشرح
    
    /**
     * تحويل لـ string
     */
    std::string toString() const {
        std::ostringstream oss;
        
        oss << "═══════════════════════════════════════════════════\n";
        oss << "  📚 " << title << "\n";
        oss << "═══════════════════════════════════════════════════\n\n";
        
        oss << "📝 الملخص:\n";
        oss << summary << "\n\n";
        
        oss << "📖 شرح الكود سطراً بسطر:\n";
        oss << "───────────────────────────────────────────────────\n";
        
        for (const auto& item : items) {
            oss << item.toString() << "\n";
        }
        
        if (!concepts.empty()) {
            oss << "🎯 المفاهيم المستخدمة:\n";
            for (const auto& concept : concepts) {
                oss << "   • " << concept << "\n";
            }
            oss << "\n";
        }
        
        if (!suggestions.empty()) {
            oss << "💡 اقتراحات للتعلم:\n";
            for (const auto& sugg : suggestions) {
                oss << "   • " << sugg << "\n";
            }
        }
        
        return oss.str();
    }
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"title\": \"" << title << "\",\n";
        oss << "  \"summary\": \"" << escapeJson(summary) << "\",\n";
        oss << "  \"items\": [\n";
        
        for (size_t i = 0; i < items.size(); ++i) {
            if (i > 0) oss << ",\n";
            oss << "    {\n";
            oss << "      \"line\": " << items[i].lineNumber << ",\n";
            oss << "      \"code\": \"" << escapeJson(items[i].code) << "\",\n";
            oss << "      \"explanation\": \"" << escapeJson(items[i].explanation) << "\"\n";
            oss << "    }";
        }
        
        oss << "\n  ]\n}";
        return oss.str();
    }
    
private:
    static std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// ==========================================================
// 📌 شارح الكود
// ==========================================================

/**
 * 🎓 شارح الكود
 */
class CodeExplainer {
private:
    ExplanationLevel level_ = ExplanationLevel::BEGINNER;
    
    // قاموس المصطلحات
    std::map<std::string, std::string> terms_ = {
        {"دالة", "مجموعة من التعليمات المترابطة التي تقوم بمهمة محددة"},
        {"متغير", "صندوق يحفظ قيمة يمكن تغييرها"},
        {"ثابت", "قيمة لا تتغير أبداً"},
        {"إذا", "اختيار مسار بناءً على شرط (صح أو خطأ)"},
        {"لكل", "تكرار لمجموعة من العناصر"},
        {"بينما", "تكرار طالما الشرط صحيح"},
        {"أرجع", "إنهاء الدالة وإعادة قيمة"},
        {"هيكل", "نوع بيانات مخصص يجمع عدة قيم"},
        {"قائمة", "مجموعة مرتبة من العناصر"},
        {"قاموس", "مجموعة من الأزواج (مفتاح ← قيمة)"},
        {"استعارة", "استخدام قيمة مؤقتاً دون امتلاكها"},
        {"نقل", "تحويل الملكية من متغير لآخر"},
        {"مؤجل", "تنفيذ كود عند الخروج من النطاق"}
    };
    
    // شرح الأنماط
    std::map<std::string, std::string> patterns_ = {
        {"recursion", "التكرار: الدالة تنادي نفسها لحل مشكلة أصغر"},
        {"loop", "الحلقة: تكرار نفس التعليمات عدة مرات"},
        {"condition", "الشرط: اختيار مسار بناءً على قيمة منطقية"},
        {"ownership", "الملكية: نظام يضمن سلامة الذاكرة"}
    };
    
public:
    /**
     * تعيين مستوى الشرح
     */
    void setLevel(ExplanationLevel level) { level_ = level; }
    
    /**
     * شرح كود
     */
    ExplanationResult explain(const std::string& code) {
        ExplanationResult result;
        result.level = level_;
        
        // تحليل الكود
        auto lines = splitLines(code);
        
        // تحديد العنوان
        result.title = detectTitle(code);
        
        // توليد الملخص
        result.summary = generateSummary(code);
        
        // شرح كل سطر
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string line = trim(lines[i]);
            if (line.empty() || line.substr(0, 2) == "//") continue;
            
            ExplanationItem item;
            item.lineNumber = static_cast<int>(i + 1);
            item.code = line;
            item.explanation = explainLine(line, i, lines);
            item.emoji = getEmojiForLine(line);
            item.tip = getTipForLine(line);
            
            result.items.push_back(item);
        }
        
        // استخراج المفاهيم
        result.concepts = extractConcepts(code);
        
        // اقتراحات للتعلم
        result.suggestions = generateSuggestions(result.concepts);
        
        return result;
    }
    
    /**
     * شرح مصطلح
     */
    std::string explainTerm(const std::string& term) {
        auto it = terms_.find(term);
        if (it != terms_.end()) {
            return it->second;
        }
        return "مصطلح غير معروف";
    }
    
    /**
     * شرح خطأ
     */
    std::string explainError(const std::string& errorMessage) {
        std::ostringstream oss;
        
        oss << "❌ الخطأ: " << errorMessage << "\n\n";
        
        // تحليل نوع الخطأ
        if (errorMessage.find("غير معرّف") != std::string::npos) {
            oss << "📚 الشرح:\n";
            oss << "   أنت تحاول استخدام متغير أو دالة لم تُعرَّف بعد.\n";
            oss << "   تأكد من كتابة 'متغير' أو 'دالة' قبل الاستخدام.\n\n";
            oss << "✅ مثال صحيح:\n";
            oss << "   متغير س = 5    // تعريف المتغير أولاً\n";
            oss << "   اطبع(س)        // ثم استخدامه\n";
        } else if (errorMessage.find("نهاية") != std::string::npos) {
            oss << "📚 الشرح:\n";
            oss << "   كل كتلة (دالة، إذا، لكل...) يجب أن تنتهي بكلمة 'نهاية'.\n\n";
            oss << "✅ مثال صحيح:\n";
            oss << "   دالة مرحبا()\n";
            oss << "       اطبع(\"مرحبا\")\n";
            oss << "   نهاية    // ← لا تنسَ هذه!\n";
        } else if (errorMessage.find("نوع") != std::string::npos) {
            oss << "📚 الشرح:\n";
            oss << "   أنت تحاول استخدام قيمة بنوع خاطئ.\n";
            oss << "   مثلاً: جمع نص مع عدد.\n\n";
            oss << "✅ مثال صحيح:\n";
            oss << "   متغير س = 5         // عدد\n";
            oss << "   متغير ص = 10        // عدد\n";
            oss << "   اطبع(س + ص)         // ✓ جمع عددين\n";
        } else if (errorMessage.find("ملكية") != std::string::npos ||
                  errorMessage.find("استعارة") != std::string::npos) {
            oss << "📚 الشرح:\n";
            oss << "   لغة ص تستخدم نظام الملكية لحماية الذاكرة.\n";
            oss << "   كل قيمة لها مالك واحد فقط.\n\n";
            oss << "💡 الحل:\n";
            oss << "   1. استخدم .استنسخ() لنسخ القيمة\n";
            oss << "   2. أو استخدم &استعارة للإشارة دون نقل\n";
        }
        
        return oss.str();
    }
    
private:
    /**
     * تقسيم السطور
     */
    std::vector<std::string> splitLines(const std::string& code) {
        std::vector<std::string> lines;
        std::istringstream iss(code);
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        return lines;
    }
    
    /**
     * تنظيف النص
     */
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    /**
     * تحديد العنوان
     */
    std::string detectTitle(const std::string& code) {
        if (code.find("دالة مضروب") != std::string::npos) {
            return "دالة حساب المضروب (Factorial)";
        }
        if (code.find("دالة فيبوناتشي") != std::string::npos) {
            return "دالة متتالية فيبوناتشي";
        }
        if (code.find("هيكل") != std::string::npos) {
            return "تعريف هيكل بيانات";
        }
        if (code.find("لكل") != std::string::npos) {
            return "حلقة تكرار";
        }
        if (code.find("دالة رئيسية") != std::string::npos) {
            return "البرنامج الرئيسي";
        }
        return "شرح الكود";
    }
    
    /**
     * توليد الملخص
     */
    std::string generateSummary(const std::string& code) {
        std::ostringstream oss;
        
        // عد العناصر
        int funcCount = countOccurrences(code, "دالة");
        int varCount = countOccurrences(code, "متغير");
        int loopCount = countOccurrences(code, "لكل") + countOccurrences(code, "بينما");
        int condCount = countOccurrences(code, "إذا");
        
        if (funcCount > 0) {
            oss << "يحتوي الكود على " << funcCount << " دالة. ";
        }
        if (varCount > 0) {
            oss << "يستخدم " << varCount << " متغير. ";
        }
        if (loopCount > 0) {
            oss << "يحتوي على " << loopCount << " حلقة تكرار. ";
        }
        if (condCount > 0) {
            oss << "يتضمن " << condCount << " شرط. ";
        }
        
        // اكتشاف الأنماط
        if (code.find("مضروب(ن - 1)") != std::string::npos ||
            code.find("فيبوناتشي(ن -") != std::string::npos) {
            oss << "يستخدم أسلوب التكرار (الدالة تنادي نفسها). ";
        }
        
        return oss.str();
    }
    
    /**
     * شرح سطر
     */
    std::string explainLine(const std::string& line, size_t index,
                            const std::vector<std::string>& allLines) {
        // دالة
        if (line.find("دالة ") == 0) {
            std::string name = extractFunctionName(line);
            return "تعريف دالة اسمها '" + name + "' - الدالة مجموعة تعليمات مترابطة";
        }
        
        // متغير
        if (line.find("متغير ") == 0) {
            return "إنشاء متغير جديد - المتغير يحفظ قيمة للاستخدام لاحقاً";
        }
        
        // ثابت
        if (line.find("ثابت ") == 0) {
            return "تعريف ثابت - قيمة لا تتغير أبداً في البرنامج";
        }
        
        // إذا
        if (line.find("إذا ") == 0) {
            return "شرط - إذا كان الشرط صحيحاً، تُنفَّذ التعليمات التالية";
        }
        
        // وإلا
        if (line == "وإلا") {
            return "الفرع البديل - يُنفَّذ إذا كان الشرط خاطئاً";
        }
        
        // لكل
        if (line.find("لكل ") == 0) {
            return "حلقة تكرار - تمر على كل عنصر في المجموعة";
        }
        
        // بينما
        if (line.find("بينما ") == 0) {
            return "حلقة شرطية - تتكرر طالما الشرط صحيح";
        }
        
        // أرجع
        if (line.find("أرجع ") == 0) {
            return "إنهاء الدالة وإرجاع القيمة للمستدعي";
        }
        
        // نهاية
        if (line == "نهاية") {
            return "نهاية الكتلة الحالية (دالة، شرط، حلقة...)";
        }
        
        // هيكل
        if (line.find("هيكل ") == 0) {
            return "تعريف نوع بيانات جديد يجمع عدة قيم معاً";
        }
        
        // اطبع
        if (line.find("اطبع(") == 0 || line.find("اطبع ") == 0) {
            return "طباعة نص أو قيمة على الشاشة";
        }
        
        // مؤجل
        if (line.find("مؤجل ") == 0) {
            return "تأجيل تنفيذ الكود حتى نهاية النطاق الحالي";
        }
        
        // افتراضي
        return "تعليمة برمجية";
    }
    
    /**
     * الحصول على emoji للسطر
     */
    std::string getEmojiForLine(const std::string& line) {
        if (line.find("دالة ") == 0) return "📦";
        if (line.find("متغير ") == 0) return "📝";
        if (line.find("ثابت ") == 0) return "🔒";
        if (line.find("إذا ") == 0) return "❓";
        if (line == "وإلا") return "↪️";
        if (line.find("لكل ") == 0) return "🔄";
        if (line.find("بينما ") == 0) return "🔁";
        if (line.find("أرجع ") == 0) return "↩️";
        if (line == "نهاية") return "🏁";
        if (line.find("هيكل ") == 0) return "🏗️";
        if (line.find("اطبع") == 0) return "📢";
        if (line.find("مؤجل ") == 0) return "⏰";
        return "▫️";
    }
    
    /**
     * الحصول على نصيحة للسطر
     */
    std::string getTipForLine(const std::string& line) {
        if (line.find("دالة ") == 0 && line.find("←") == std::string::npos) {
            return "الدوال التي لا ترجع قيمة تنتهي بدون ←";
        }
        if (line.find("متغير ") == 0 && line.find(":") == std::string::npos) {
            return "يمكن تحديد النوع صراحةً: متغير س: عدد = 5";
        }
        return "";
    }
    
    /**
     * استخراج المفاهيم
     */
    std::vector<std::string> extractConcepts(const std::string& code) {
        std::vector<std::string> concepts;
        
        if (code.find("دالة") != std::string::npos) {
            concepts.push_back("الدوال (Functions)");
        }
        if (code.find("متغير") != std::string::npos) {
            concepts.push_back("المتغيرات (Variables)");
        }
        if (code.find("إذا") != std::string::npos) {
            concepts.push_back("الشروط (Conditionals)");
        }
        if (code.find("لكل") != std::string::npos || 
            code.find("بينما") != std::string::npos) {
            concepts.push_back("الحلقات (Loops)");
        }
        if (code.find("(ن - 1)") != std::string::npos) {
            concepts.push_back("التكرار (Recursion)");
        }
        if (code.find("استعارة") != std::string::npos ||
            code.find("&") != std::string::npos) {
            concepts.push_back("الملكية والاستعارة (Ownership)");
        }
        
        return concepts;
    }
    
    /**
     * توليد اقتراحات
     */
    std::vector<std::string> generateSuggestions(
        const std::vector<std::string>& concepts) {
        std::vector<std::string> suggestions;
        
        for (const auto& concept : concepts) {
            if (concept.find("التكرار") != std::string::npos) {
                suggestions.push_back("جرّب كتابة دالة فيبوناتشي باستخدام التكرار");
                suggestions.push_back("تعلم الفرق بين التكرار والحلقات");
            }
            if (concept.find("الدوال") != std::string::npos) {
                suggestions.push_back("اكتب دوالاً صغيرة تقوم بمهمة واحدة");
            }
            if (concept.find("الملكية") != std::string::npos) {
                suggestions.push_back("راجع درس الملكية في 'ص تعلّم ملكية'");
            }
        }
        
        return suggestions;
    }
    
    /**
     * استخراج اسم الدالة
     */
    std::string extractFunctionName(const std::string& line) {
        size_t start = line.find(' ') + 1;
        size_t end = line.find('(');
        if (end == std::string::npos) end = line.length();
        return line.substr(start, end - start);
    }
    
    /**
     * عد التكرارات
     */
    int countOccurrences(const std::string& str, const std::string& sub) {
        int count = 0;
        size_t pos = 0;
        while ((pos = str.find(sub, pos)) != std::string::npos) {
            count++;
            pos += sub.length();
        }
        return count;
    }
};

} // namespace ai
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء شارح كود
 */
void* sad_explainer_new(void) {
    return new sad::ai::CodeExplainer();
}

/**
 * تحرير الشارح
 */
void sad_explainer_free(void* explainer) {
    delete static_cast<sad::ai::CodeExplainer*>(explainer);
}

/**
 * تعيين المستوى
 */
void sad_explainer_set_level(void* explainer, int level) {
    auto* e = static_cast<sad::ai::CodeExplainer*>(explainer);
    e->setLevel(static_cast<sad::ai::ExplanationLevel>(level));
}

/**
 * شرح كود
 */
char* sad_explainer_explain(void* explainer, const char* code) {
    auto* e = static_cast<sad::ai::CodeExplainer*>(explainer);
    auto result = e->explain(code);
    
    std::string output = result.toString();
    char* str = new char[output.size() + 1];
    std::strcpy(str, output.c_str());
    return str;
}

/**
 * شرح خطأ
 */
char* sad_explainer_error(void* explainer, const char* error) {
    auto* e = static_cast<sad::ai::CodeExplainer*>(explainer);
    std::string result = e->explainError(error);
    
    char* str = new char[result.size() + 1];
    std::strcpy(str, result.c_str());
    return str;
}

/**
 * شرح مصطلح
 */
char* sad_explainer_term(void* explainer, const char* term) {
    auto* e = static_cast<sad::ai::CodeExplainer*>(explainer);
    std::string result = e->explainTerm(term);
    
    char* str = new char[result.size() + 1];
    std::strcpy(str, result.c_str());
    return str;
}

/**
 * تحرير نص
 */
void sad_explainer_free_string(char* str) {
    delete[] str;
}

} // extern "C"
