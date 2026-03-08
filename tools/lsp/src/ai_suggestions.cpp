/**
 * ==========================================================
 * ملف: ai_suggestions.cpp
 * الوصف: تكامل AI مع LSP
 * المهمة: T302 - نظام ذكاء ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔌 ما هو LSP؟
 * -------------
 * LSP = Language Server Protocol
 * بروتوكول يربط المحرر (مثل VS Code) بخادم اللغة.
 * يوفر:
 * - إكمال تلقائي
 * - أخطاء فورية
 * - القفز للتعريف
 * - وأكثر!
 * 
 * 🤖 ما هو تكامل AI مع LSP؟
 * -------------------------
 * بدلاً من اقتراحات عادية، AI يقترح كود ذكي!
 * 
 * مثال:
 * -----
 * تكتب: "دالة تقرأ ملف..."
 * AI يقترح: الدالة الكاملة مع إدارة الأخطاء والملكية!
 * 
 * 🎯 الميزات:
 * ----------
 * 1. اقتراحات كود ذكية
 * 2. شرح فوري للأخطاء
 * 3. إصلاح تلقائي
 * 4. توليد كود من تعليقات
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <functional>
#include <memory>
#include <cstring>
#include <algorithm>

namespace sad {
namespace lsp {

// ==========================================================
// 📌 أنواع الاقتراحات
// ==========================================================

/**
 * 🏷️ نوع الاقتراح
 */
enum class SuggestionKind {
    COMPLETION,     // إكمال تلقائي
    SNIPPET,        // قالب كود
    FUNCTION,       // دالة كاملة
    FIX,            // إصلاح خطأ
    REFACTOR,       // إعادة هيكلة
    DOCUMENTATION,  // توثيق
    TEST            // اختبار
};

/**
 * 📝 أسماء أنواع الاقتراحات
 */
const char* suggestionKindName(SuggestionKind kind) {
    switch (kind) {
        case SuggestionKind::COMPLETION: return "إكمال";
        case SuggestionKind::SNIPPET: return "قالب";
        case SuggestionKind::FUNCTION: return "دالة";
        case SuggestionKind::FIX: return "إصلاح";
        case SuggestionKind::REFACTOR: return "إعادة هيكلة";
        case SuggestionKind::DOCUMENTATION: return "توثيق";
        case SuggestionKind::TEST: return "اختبار";
        default: return "غير معروف";
    }
}

// ==========================================================
// 📌 اقتراح AI
// ==========================================================

/**
 * 💡 اقتراح من AI
 */
struct AISuggestion {
    SuggestionKind kind;
    std::string label;          // التسمية المختصرة
    std::string detail;         // التفاصيل
    std::string insertText;     // النص للإدراج
    std::string documentation;  // الشرح
    int priority = 0;           // الأولوية
    bool isAIGenerated = true;  // من AI
    
    /**
     * تحويل لـ JSON (لـ LSP)
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"label\": \"" << escapeJson(label) << "\",\n";
        oss << "  \"kind\": " << static_cast<int>(kind) << ",\n";
        oss << "  \"detail\": \"" << escapeJson(detail) << "\",\n";
        oss << "  \"documentation\": {\n";
        oss << "    \"kind\": \"markdown\",\n";
        oss << "    \"value\": \"" << escapeJson(documentation) << "\"\n";
        oss << "  },\n";
        oss << "  \"insertText\": \"" << escapeJson(insertText) << "\",\n";
        oss << "  \"insertTextFormat\": 2,\n"; // Snippet
        oss << "  \"sortText\": \"" << (100 - priority) << "\"\n";
        oss << "}";
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
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// ==========================================================
// 📌 سياق الكود
// ==========================================================

/**
 * 📍 سياق الكود الحالي
 */
struct CodeContext {
    std::string currentLine;    // السطر الحالي
    std::string previousLines;  // السطور السابقة
    std::string fileName;       // اسم الملف
    int lineNumber = 0;         // رقم السطر
    int column = 0;             // العمود
    std::string currentWord;    // الكلمة الحالية
    bool inFunction = false;    // داخل دالة
    bool inStruct = false;      // داخل هيكل
    bool inLoop = false;        // داخل حلقة
    bool inCondition = false;   // داخل شرط
    std::string currentFunctionName; // اسم الدالة الحالية
    std::vector<std::string> availableVariables; // المتغيرات المتاحة
};

// ==========================================================
// 📌 مولّد الاقتراحات
// ==========================================================

/**
 * 🤖 مولّد اقتراحات AI
 */
class AISuggestionGenerator {
private:
    // قوالب الاقتراحات
    std::vector<std::pair<std::string, std::string>> snippetTemplates_ = {
        // كلمة مفتاحية → قالب
        {"دالة", "دالة ${1:اسم}(${2:معاملات}) ← ${3:نوع_الإرجاع}\n    ${0}\nنهاية"},
        {"إذا", "إذا ${1:شرط}\n    ${0}\nنهاية"},
        {"إذا_وإلا", "إذا ${1:شرط}\n    ${2}\nوإلا\n    ${0}\nنهاية"},
        {"لكل", "لكل ${1:عنصر} في ${2:قائمة}\n    ${0}\nنهاية"},
        {"بينما", "بينما ${1:شرط}\n    ${0}\nنهاية"},
        {"هيكل", "هيكل ${1:اسم}\n    ${0}\nنهاية"},
        {"اختبار", "اختبار \"${1:اسم_الاختبار}\"\n    ${0}\nنهاية"},
        {"مطابقة", "طابق ${1:قيمة}\n    حالة ${2:نمط} =>\n        ${0}\نهاية"},
        {"حاول", "حاول\n    ${1}\nامسك خطأ\n    ${0}\نهاية"},
        {"مؤجل", "مؤجل ${0}"}
    };
    
    // أنماط لاكتشاف السياق
    std::map<std::string, std::string> contextPatterns_ = {
        {"ملف", "قراءة/كتابة ملفات"},
        {"اتصال", "عمليات الشبكة"},
        {"قائمة", "معالجة القوائم"},
        {"قاموس", "معالجة القواميس"},
        {"نص", "معالجة النصوص"}
    };
    
public:
    /**
     * توليد اقتراحات
     */
    std::vector<AISuggestion> generateSuggestions(const CodeContext& context) {
        std::vector<AISuggestion> suggestions;
        
        // 1. اقتراحات القوالب
        auto snippets = generateSnippetSuggestions(context);
        suggestions.insert(suggestions.end(), snippets.begin(), snippets.end());
        
        // 2. اقتراحات ذكية بناءً على السياق
        auto smart = generateSmartSuggestions(context);
        suggestions.insert(suggestions.end(), smart.begin(), smart.end());
        
        // 3. اقتراحات إكمال
        auto completions = generateCompletions(context);
        suggestions.insert(suggestions.end(), completions.begin(), completions.end());
        
        // ترتيب حسب الأولوية
        std::sort(suggestions.begin(), suggestions.end(),
                 [](const auto& a, const auto& b) {
                     return a.priority > b.priority;
                 });
        
        return suggestions;
    }
    
    /**
     * توليد إصلاح لخطأ
     */
    AISuggestion generateFix(const std::string& errorMessage,
                              const CodeContext& context) {
        AISuggestion fix;
        fix.kind = SuggestionKind::FIX;
        fix.priority = 100; // أعلى أولوية
        
        // تحليل الخطأ واقتراح الإصلاح
        if (errorMessage.find("غير معرّف") != std::string::npos) {
            fix.label = "🔧 إصلاح: تعريف مفقود";
            fix.detail = "إضافة تعريف للمتغير/الدالة";
            fix.insertText = "متغير ${1:اسم} = ${0}";
            fix.documentation = "هذا المتغير غير معرّف. أضف تعريفاً له.";
        } else if (errorMessage.find("نهاية") != std::string::npos) {
            fix.label = "🔧 إصلاح: نهاية مفقودة";
            fix.detail = "إضافة كلمة نهاية";
            fix.insertText = "نهاية";
            fix.documentation = "الكتلة تحتاج كلمة 'نهاية' لإغلاقها.";
        } else if (errorMessage.find("ملكية") != std::string::npos ||
                  errorMessage.find("استعارة") != std::string::npos) {
            fix.label = "🔧 إصلاح: مشكلة ملكية";
            fix.detail = "إضافة استنساخ أو استعارة";
            fix.insertText = "${1:المتغير}.استنسخ()";
            fix.documentation = "استخدم .استنسخ() لنسخ القيمة بدلاً من نقلها.";
        } else {
            fix.label = "🔧 إصلاح عام";
            fix.detail = errorMessage;
            fix.insertText = "";
            fix.documentation = "راجع الخطأ وأصلحه يدوياً.";
        }
        
        return fix;
    }
    
    /**
     * توليد توثيق لدالة
     */
    AISuggestion generateDocumentation(const std::string& functionSignature) {
        AISuggestion doc;
        doc.kind = SuggestionKind::DOCUMENTATION;
        doc.label = "📝 إضافة توثيق";
        doc.priority = 50;
        
        // تحليل توقيع الدالة
        std::ostringstream oss;
        oss << "/**\n";
        oss << " * ${1:وصف الدالة}\n";
        
        // استخراج المعاملات (مبسط)
        if (functionSignature.find("(") != std::string::npos) {
            oss << " *\n";
            oss << " * @معامل ${2:اسم} - ${3:وصف}\n";
        }
        
        if (functionSignature.find("←") != std::string::npos) {
            oss << " * @إرجاع ${4:وصف القيمة المرجعة}\n";
        }
        
        oss << " */\n";
        
        doc.insertText = oss.str();
        doc.documentation = "إضافة توثيق للدالة بتنسيق JSDoc عربي";
        
        return doc;
    }
    
    /**
     * توليد اختبار لدالة
     */
    AISuggestion generateTest(const std::string& functionName,
                               const std::string& functionSignature) {
        AISuggestion test;
        test.kind = SuggestionKind::TEST;
        test.label = "🧪 توليد اختبار لـ " + functionName;
        test.priority = 40;
        
        std::ostringstream oss;
        oss << "اختبار \"" << functionName << " - الحالة العادية\"\n";
        oss << "    // الترتيب\n";
        oss << "    ${1:// إعداد البيانات}\n";
        oss << "    \n";
        oss << "    // التنفيذ\n";
        oss << "    متغير نتيجة = " << functionName << "(${2:معاملات})\n";
        oss << "    \n";
        oss << "    // التأكيد\n";
        oss << "    تأكد_يساوي(${3:متوقع}, نتيجة)\n";
        oss << "نهاية\n";
        
        test.insertText = oss.str();
        test.documentation = "اختبار وحدة للدالة " + functionName;
        
        return test;
    }
    
private:
    /**
     * توليد اقتراحات القوالب
     */
    std::vector<AISuggestion> generateSnippetSuggestions(const CodeContext& context) {
        std::vector<AISuggestion> suggestions;
        
        for (const auto& [keyword, snippet] : snippetTemplates_) {
            // تحقق من تطابق الكلمة الحالية
            if (context.currentWord.empty() || 
                keyword.find(context.currentWord) == 0) {
                AISuggestion sugg;
                sugg.kind = SuggestionKind::SNIPPET;
                sugg.label = "📝 " + keyword;
                sugg.detail = "قالب " + keyword;
                sugg.insertText = snippet;
                sugg.documentation = "قالب لإنشاء " + keyword;
                sugg.priority = 80;
                suggestions.push_back(sugg);
            }
        }
        
        return suggestions;
    }
    
    /**
     * توليد اقتراحات ذكية
     */
    std::vector<AISuggestion> generateSmartSuggestions(const CodeContext& context) {
        std::vector<AISuggestion> suggestions;
        
        // اقتراح بناءً على السياق
        std::string combined = context.currentLine + " " + context.previousLines;
        
        // اقتراح قراءة ملف
        if (combined.find("ملف") != std::string::npos || 
            combined.find("قراءة") != std::string::npos) {
            AISuggestion sugg;
            sugg.kind = SuggestionKind::FUNCTION;
            sugg.label = "🤖 دالة قراءة ملف آمنة";
            sugg.detail = "AI: دالة كاملة مع إدارة الأخطاء";
            sugg.insertText = R"(دالة اقرأ_ملف(مسار: نص) ← نتيجة<نص، خطأ>
    متغير ملف = حاول افتح(مسار، "قراءة")
    إذا ملف.فشل()
        أرجع خطأ("فشل فتح الملف: " + مسار)
    نهاية
    
    مؤجل أغلق(ملف.قيمة())
    
    أرجع نجاح(ملف.قيمة().اقرأ_الكل())
نهاية)";
            sugg.documentation = "دالة آمنة لقراءة ملف مع إدارة الأخطاء وضمان الإغلاق";
            sugg.priority = 95;
            suggestions.push_back(sugg);
        }
        
        // اقتراح HTTP client
        if (combined.find("http") != std::string::npos ||
            combined.find("طلب") != std::string::npos ||
            combined.find("API") != std::string::npos) {
            AISuggestion sugg;
            sugg.kind = SuggestionKind::FUNCTION;
            sugg.label = "🤖 دالة طلب HTTP";
            sugg.detail = "AI: طلب HTTP مع إدارة الأخطاء";
            sugg.insertText = R"(دالة طلب_http(رابط: نص) ← نتيجة<نص، خطأ>
    متغير عميل = حاول أنشئ_عميل_http()
    إذا عميل.فشل()
        أرجع خطأ("فشل إنشاء العميل")
    نهاية
    
    متغير استجابة = حاول عميل.قيمة().أرسل(طلب_جلب(رابط))
    إذا استجابة.فشل()
        أرجع خطأ("فشل الطلب: " + استجابة.خطأ())
    نهاية
    
    أرجع نجاح(استجابة.قيمة().النص())
نهاية)";
            sugg.documentation = "دالة آمنة لإرسال طلبات HTTP";
            sugg.priority = 90;
            suggestions.push_back(sugg);
        }
        
        return suggestions;
    }
    
    /**
     * توليد إكمالات
     */
    std::vector<AISuggestion> generateCompletions(const CodeContext& context) {
        std::vector<AISuggestion> suggestions;
        
        // إكمال من المتغيرات المتاحة
        for (const auto& var : context.availableVariables) {
            if (var.find(context.currentWord) == 0) {
                AISuggestion sugg;
                sugg.kind = SuggestionKind::COMPLETION;
                sugg.label = var;
                sugg.insertText = var;
                sugg.priority = 70;
                suggestions.push_back(sugg);
            }
        }
        
        return suggestions;
    }
};

// ==========================================================
// 📌 خادم LSP AI
// ==========================================================

/**
 * 🖥️ خادم اقتراحات AI
 */
class AILSPServer {
private:
    AISuggestionGenerator generator_;
    bool enabled_ = true;
    
public:
    /**
     * تمكين/تعطيل
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    /**
     * معالجة طلب إكمال
     */
    std::string handleCompletionRequest(const std::string& jsonRequest) {
        if (!enabled_) return "[]";
        
        // تحليل السياق من الطلب
        CodeContext context = parseContext(jsonRequest);
        
        // توليد الاقتراحات
        auto suggestions = generator_.generateSuggestions(context);
        
        // تحويل لـ JSON
        return suggestionsToJson(suggestions);
    }
    
    /**
     * معالجة طلب إصلاح
     */
    std::string handleCodeAction(const std::string& jsonRequest,
                                  const std::string& errorMessage) {
        if (!enabled_) return "[]";
        
        CodeContext context = parseContext(jsonRequest);
        auto fix = generator_.generateFix(errorMessage, context);
        
        return "[" + fix.toJson() + "]";
    }
    
    /**
     * معالجة طلب توثيق
     */
    std::string handleDocRequest(const std::string& functionSignature) {
        auto doc = generator_.generateDocumentation(functionSignature);
        return doc.toJson();
    }
    
private:
    /**
     * تحليل السياق من JSON
     */
    CodeContext parseContext(const std::string& json) {
        CodeContext context;
        
        // تحليل مبسط - في التنفيذ الحقيقي نستخدم محلل JSON كامل
        // استخراج السطر الحالي
        size_t pos = json.find("\"currentLine\":");
        if (pos != std::string::npos) {
            // ... تحليل
        }
        
        return context;
    }
    
    /**
     * تحويل الاقتراحات لـ JSON
     */
    std::string suggestionsToJson(const std::vector<AISuggestion>& suggestions) {
        std::ostringstream oss;
        oss << "[";
        
        for (size_t i = 0; i < suggestions.size(); ++i) {
            if (i > 0) oss << ",";
            oss << suggestions[i].toJson();
        }
        
        oss << "]";
        return oss.str();
    }
};

} // namespace lsp
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء خادم LSP AI
 */
void* sad_ai_lsp_new(void) {
    return new sad::lsp::AILSPServer();
}

/**
 * تحرير الخادم
 */
void sad_ai_lsp_free(void* server) {
    delete static_cast<sad::lsp::AILSPServer*>(server);
}

/**
 * تمكين/تعطيل
 */
void sad_ai_lsp_set_enabled(void* server, int enabled) {
    static_cast<sad::lsp::AILSPServer*>(server)->setEnabled(enabled != 0);
}

/**
 * معالجة طلب إكمال
 */
char* sad_ai_lsp_complete(void* server, const char* request) {
    auto* s = static_cast<sad::lsp::AILSPServer*>(server);
    std::string result = s->handleCompletionRequest(request);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * معالجة طلب إصلاح
 */
char* sad_ai_lsp_fix(void* server, const char* request, const char* error) {
    auto* s = static_cast<sad::lsp::AILSPServer*>(server);
    std::string result = s->handleCodeAction(request, error);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * تحرير نص
 */
void sad_ai_lsp_free_string(char* str) {
    delete[] str;
}

} // extern "C"
