// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: diagnostics.cpp
 * الوصف: مزود التشخيصات للغة ص - Diagnostics Provider
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T094
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يوفر خدمة التشخيصات (Diagnostics) للغة ص.
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  ما هي التشخيصات؟                                                        ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  رسائل تُظهر للمطور أثناء الكتابة لتنبيهه على:                           ║
 * ║  - الأخطاء النحوية (Syntax Errors)                                       ║
 * ║  - الأخطاء الدلالية (Semantic Errors)                                    ║
 * ║  - التحذيرات (Warnings)                                                  ║
 * ║  - الاقتراحات (Hints)                                                    ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * أنواع التشخيصات:
 * 
 * ┌──────────────────┬───────────────────────────────────────────────────────────┐
 * │ النوع            │ الأمثلة                                                    │
 * ├──────────────────┼───────────────────────────────────────────────────────────┤
 * │ خطأ نحوي         │ قوس غير مغلق، كلمة مفتاحية ناقصة، تنسيق خاطئ             │
 * │ خطأ دلالي        │ متغير غير معرف، نوع غير متوافق، دالة غير موجودة          │
 * │ تحذير            │ متغير غير مستخدم، كود لن يُنفذ، استيراد غير مستخدم        │
 * │ اقتراح           │ تبسيط الكود، تحسين الأداء، أفضل الممارسات                │
 * └──────────────────┴───────────────────────────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * رموز الأخطاء:
 * 
 * ص-001: خطأ نحوي عام
 * ص-002: قوس غير مغلق
 * ص-003: كلمة مفتاحية ناقصة
 * ص-004: متغير غير معرف
 * ص-005: نوع غير متوافق
 * ص-006: دالة غير موجودة
 * ص-007: عدد معلمات خاطئ
 * ص-008: متغير غير مستخدم
 * ص-009: استيراد غير مستخدم
 * ص-010: كود لن يُنفذ
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include "diagnostics.hpp"
#include "../lsp_types.h"

#include <regex>
#include <stack>
#include <unordered_set>

namespace sad {
namespace lsp {

// ═══════════════════════════════════════════════════════════════════════════════
// رموز الأخطاء - Error Codes
// ═══════════════════════════════════════════════════════════════════════════════

namespace ErrorCodes {
    const std::string SYNTAX_ERROR = "ص-001";
    const std::string UNCLOSED_BRACKET = "ص-002";
    const std::string MISSING_KEYWORD = "ص-003";
    const std::string UNDEFINED_VARIABLE = "ص-004";
    const std::string TYPE_MISMATCH = "ص-005";
    const std::string UNDEFINED_FUNCTION = "ص-006";
    const std::string WRONG_ARGUMENT_COUNT = "ص-007";
    const std::string UNUSED_VARIABLE = "ص-008";
    const std::string UNUSED_IMPORT = "ص-009";
    const std::string UNREACHABLE_CODE = "ص-010";
}


// ═══════════════════════════════════════════════════════════════════════════════
// صنف مزود التشخيصات - Diagnostics Provider Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class DiagnosticsProvider
 * @brief يحلل الكود ويكتشف الأخطاء والتحذيرات
 */
class DiagnosticsProvider {
public:
    /**
     * @brief تحليل المستند
     * @param doc المستند للتحليل
     * @return قائمة التشخيصات
     */
    std::vector<Diagnostic> analyze(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        // ١. فحص الأقواس
        auto bracket_errors = check_brackets(doc);
        diagnostics.insert(diagnostics.end(), bracket_errors.begin(), bracket_errors.end());
        
        // ٢. فحص الكلمات المفتاحية
        auto keyword_errors = check_keywords(doc);
        diagnostics.insert(diagnostics.end(), keyword_errors.begin(), keyword_errors.end());
        
        // ٣. فحص المتغيرات
        auto variable_errors = check_variables(doc);
        diagnostics.insert(diagnostics.end(), variable_errors.begin(), variable_errors.end());
        
        // ٤. فحص الاستيرادات
        auto import_warnings = check_imports(doc);
        diagnostics.insert(diagnostics.end(), import_warnings.begin(), import_warnings.end());
        
        // ٥. فحص الكود غير المُنفذ
        auto unreachable_warnings = check_unreachable_code(doc);
        diagnostics.insert(diagnostics.end(), unreachable_warnings.begin(), unreachable_warnings.end());
        
        // ٦. فحص الأنماط الشائعة
        auto pattern_hints = check_common_patterns(doc);
        diagnostics.insert(diagnostics.end(), pattern_hints.begin(), pattern_hints.end());
        
        return diagnostics;
    }

private:
    /**
     * @brief إنشاء تشخيص
     */
    Diagnostic create_diagnostic(
        DiagnosticSeverity severity,
        const std::string& code,
        const std::string& message,
        int line,
        int start_char,
        int end_char) {
        
        Diagnostic diag;
        diag.severity = severity;
        diag.code = code;
        diag.message = message;
        diag.range.start = {line, start_char};
        diag.range.end = {line, end_char};
        diag.source = "خادم لغة ص";
        
        return diag;
    }
    
    /**
     * @brief فحص الأقواس
     * يتحقق من أن جميع الأقواس مغلقة بشكل صحيح
     */
    std::vector<Diagnostic> check_brackets(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        struct BracketInfo {
            char bracket;
            int line;
            int column;
        };
        
        std::stack<BracketInfo> stack;
        
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            bool in_string = false;
            bool in_comment = false;
            
            for (int col = 0; col < static_cast<int>(text.length()); ++col) {
                char c = text[col];
                
                // تخطي التعليقات
                if (!in_string && col + 1 < static_cast<int>(text.length()) && 
                    text[col] == '/' && text[col + 1] == '/') {
                    in_comment = true;
                    break;
                }
                
                // تتبع النصوص
                if (!in_comment && (c == '"' || c == '\'')) {
                    if (col == 0 || text[col - 1] != '\\') {
                        in_string = !in_string;
                    }
                }
                
                if (in_string || in_comment) continue;
                
                // فحص الأقواس
                if (c == '(' || c == '[' || c == '{') {
                    stack.push({c, line, col});
                }
                else if (c == ')' || c == ']' || c == '}') {
                    char expected = (c == ')') ? '(' : (c == ']') ? '[' : '{';
                    
                    if (stack.empty()) {
                        diagnostics.push_back(create_diagnostic(
                            DiagnosticSeverity::ERROR,
                            ErrorCodes::UNCLOSED_BRACKET,
                            "قوس إغلاق '" + std::string(1, c) + "' بدون قوس فتح مقابل",
                            line, col, col + 1
                        ));
                    }
                    else if (stack.top().bracket != expected) {
                        diagnostics.push_back(create_diagnostic(
                            DiagnosticSeverity::ERROR,
                            ErrorCodes::UNCLOSED_BRACKET,
                            "قوس '" + std::string(1, c) + "' لا يتطابق مع قوس الفتح '" + 
                            std::string(1, stack.top().bracket) + "'",
                            line, col, col + 1
                        ));
                    }
                    else {
                        stack.pop();
                    }
                }
            }
        }
        
        // الأقواس غير المغلقة
        while (!stack.empty()) {
            auto& info = stack.top();
            diagnostics.push_back(create_diagnostic(
                DiagnosticSeverity::ERROR,
                ErrorCodes::UNCLOSED_BRACKET,
                "قوس '" + std::string(1, info.bracket) + "' غير مغلق",
                info.line, info.column, info.column + 1
            ));
            stack.pop();
        }
        
        return diagnostics;
    }
    
    /**
     * @brief فحص الكلمات المفتاحية
     * يتحقق من استخدام الكلمات المفتاحية بشكل صحيح
     */
    std::vector<Diagnostic> check_keywords(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        // عداد الكتل المفتوحة
        int block_count = 0;
        std::vector<std::pair<std::string, int>> open_blocks; // (نوع الكتلة، رقم السطر)
        
        // الكلمات التي تفتح كتلة
        std::regex block_start(R"(\b(دالة|صنف|إذا|طالما|لكل|كرر|حاول)\b)");
        
        // نهاية الكتلة
        std::regex block_end(R"(\bنهاية\b)");
        
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            // تخطي التعليقات
            if (text.find("//") == 0) continue;
            
            // البحث عن بدايات الكتل
            std::smatch match;
            std::string::const_iterator search_start = text.cbegin();
            while (std::regex_search(search_start, text.cend(), match, block_start)) {
                open_blocks.push_back({match[1].str(), line});
                block_count++;
                search_start = match.suffix().first;
            }
            
            // البحث عن نهايات الكتل
            search_start = text.cbegin();
            while (std::regex_search(search_start, text.cend(), match, block_end)) {
                if (open_blocks.empty()) {
                    diagnostics.push_back(create_diagnostic(
                        DiagnosticSeverity::ERROR,
                        ErrorCodes::MISSING_KEYWORD,
                        "'نهاية' بدون كتلة مفتوحة",
                        line, 
                        static_cast<int>(match.position()),
                        static_cast<int>(match.position() + match.length())
                    ));
                } else {
                    open_blocks.pop_back();
                    block_count--;
                }
                search_start = match.suffix().first;
            }
        }
        
        // الكتل غير المغلقة
        for (const auto& [keyword, line] : open_blocks) {
            diagnostics.push_back(create_diagnostic(
                DiagnosticSeverity::ERROR,
                ErrorCodes::MISSING_KEYWORD,
                "كتلة '" + keyword + "' غير مغلقة - يجب إضافة 'نهاية'",
                line, 0, static_cast<int>(doc.lines[line].length())
            ));
        }
        
        return diagnostics;
    }
    
    /**
     * @brief فحص المتغيرات
     * يتحقق من تعريف واستخدام المتغيرات
     */
    std::vector<Diagnostic> check_variables(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        std::unordered_set<std::string> defined_variables;
        std::unordered_set<std::string> used_variables;
        std::unordered_map<std::string, int> definition_lines;
        
        // التعبير النمطي لتعريف المتغير
        // نوع اسم = قيمة
        std::regex var_def(R"(\b(رقم|نص|منطقي|قائمة|خريطة)\s+(\w+)\s*=)");
        
        // التعبير النمطي لاستخدام المتغير
        // أي معرف
        std::regex var_use(R"(\b([a-zA-Z_\u0600-\u06FF][\w\u0600-\u06FF]*)\b)");
        
        // المرحلة ١: جمع التعريفات
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            std::smatch match;
            if (std::regex_search(text, match, var_def)) {
                std::string var_name = match[2].str();
                defined_variables.insert(var_name);
                definition_lines[var_name] = line;
            }
        }
        
        // المرحلة ٢: جمع الاستخدامات وفحص التعريف
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            // تخطي التعليقات
            size_t comment_pos = text.find("//");
            std::string code = (comment_pos != std::string::npos) 
                             ? text.substr(0, comment_pos) 
                             : text;
            
            std::smatch match;
            std::string::const_iterator search_start = code.cbegin();
            while (std::regex_search(search_start, code.cend(), match, var_use)) {
                std::string var_name = match[1].str();
                
                // تجاهل الكلمات المحجوزة
                if (!is_reserved_word(var_name)) {
                    used_variables.insert(var_name);
                    
                    // التحقق من التعريف
                    if (defined_variables.find(var_name) == defined_variables.end() &&
                        !is_builtin_function(var_name)) {
                        
                        // التحقق من أنه ليس تعريفاً في نفس السطر
                        if (text.find("رقم " + var_name) == std::string::npos &&
                            text.find("نص " + var_name) == std::string::npos &&
                            text.find("منطقي " + var_name) == std::string::npos &&
                            text.find("قائمة " + var_name) == std::string::npos &&
                            text.find("خريطة " + var_name) == std::string::npos &&
                            text.find("دالة " + var_name) == std::string::npos &&
                            text.find("صنف " + var_name) == std::string::npos) {
                            
                            int col = static_cast<int>(match.position() + 
                                     (search_start - code.cbegin()));
                            
                            diagnostics.push_back(create_diagnostic(
                                DiagnosticSeverity::ERROR,
                                ErrorCodes::UNDEFINED_VARIABLE,
                                "المتغير '" + var_name + "' غير معرف",
                                line, col, col + static_cast<int>(var_name.length())
                            ));
                        }
                    }
                }
                
                search_start = match.suffix().first;
            }
        }
        
        // المرحلة ٣: فحص المتغيرات غير المستخدمة
        for (const auto& var : defined_variables) {
            if (used_variables.find(var) == used_variables.end()) {
                int line = definition_lines[var];
                
                diagnostics.push_back(create_diagnostic(
                    DiagnosticSeverity::WARNING,
                    ErrorCodes::UNUSED_VARIABLE,
                    "المتغير '" + var + "' معرف لكنه غير مستخدم",
                    line, 0, static_cast<int>(doc.lines[line].length())
                ));
            }
        }
        
        return diagnostics;
    }
    
    /**
     * @brief فحص الاستيرادات
     */
    std::vector<Diagnostic> check_imports(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        std::unordered_map<std::string, int> imports; // اسم -> سطر
        std::unordered_set<std::string> used_modules;
        
        // التعبير النمطي للاستيراد
        std::regex import_regex(R"(استورد\s+(\w+))");
        
        // المرحلة ١: جمع الاستيرادات
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            std::smatch match;
            if (std::regex_search(text, match, import_regex)) {
                imports[match[1].str()] = line;
            }
        }
        
        // المرحلة ٢: فحص الاستخدام
        for (const auto& [module, line] : imports) {
            bool found = false;
            
            // البحث عن استخدام الوحدة
            for (const auto& text : doc.lines) {
                if (text.find(module + ".") != std::string::npos) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                diagnostics.push_back(create_diagnostic(
                    DiagnosticSeverity::WARNING,
                    ErrorCodes::UNUSED_IMPORT,
                    "الوحدة '" + module + "' مستوردة لكنها غير مستخدمة",
                    line, 0, static_cast<int>(doc.lines[line].length())
                ));
            }
        }
        
        return diagnostics;
    }
    
    /**
     * @brief فحص الكود غير المُنفذ
     */
    std::vector<Diagnostic> check_unreachable_code(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        bool after_return = false;
        int return_line = -1;
        
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            // البحث عن إرجاع
            if (text.find("إرجاع") != std::string::npos) {
                after_return = true;
                return_line = line;
                continue;
            }
            
            // البحث عن نهاية الدالة
            if (text.find("نهاية") != std::string::npos) {
                after_return = false;
                continue;
            }
            
            // فحص الكود بعد الإرجاع
            if (after_return && !text.empty() && text.find_first_not_of(" \t") != std::string::npos) {
                diagnostics.push_back(create_diagnostic(
                    DiagnosticSeverity::WARNING,
                    ErrorCodes::UNREACHABLE_CODE,
                    "هذا الكود لن يُنفذ أبداً (بعد 'إرجاع' في السطر " + std::to_string(return_line + 1) + ")",
                    line, 0, static_cast<int>(text.length())
                ));
            }
        }
        
        return diagnostics;
    }
    
    /**
     * @brief فحص الأنماط الشائعة
     * يقترح تحسينات على الكود
     */
    std::vector<Diagnostic> check_common_patterns(const DocumentInfo& doc) {
        std::vector<Diagnostic> diagnostics;
        
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            // اقتراح: إذا مع مقارنة منطقية
            // إذا (x == صحيح) -> إذا (x)
            std::regex bool_compare(R"(إذا\s*\(\s*(\w+)\s*==\s*صحيح\s*\))");
            std::smatch match;
            if (std::regex_search(text, match, bool_compare)) {
                diagnostics.push_back(create_diagnostic(
                    DiagnosticSeverity::HINT,
                    "ص-101",
                    "يمكن تبسيط الشرط إلى: إذا (" + match[1].str() + ")",
                    line,
                    static_cast<int>(match.position()),
                    static_cast<int>(match.position() + match.length())
                ));
            }
            
            // اقتراح: إذا مع مقارنة خطأ
            // إذا (x == خطأ) -> إذا (!x)
            std::regex bool_false(R"(إذا\s*\(\s*(\w+)\s*==\s*خطأ\s*\))");
            if (std::regex_search(text, match, bool_false)) {
                diagnostics.push_back(create_diagnostic(
                    DiagnosticSeverity::HINT,
                    "ص-102",
                    "يمكن تبسيط الشرط إلى: إذا (!" + match[1].str() + ")",
                    line,
                    static_cast<int>(match.position()),
                    static_cast<int>(match.position() + match.length())
                ));
            }
            
            // اقتراح: طول قائمة == 0
            // طول(قائمة) == 0 -> فارغة(قائمة)
            std::regex empty_check(R"(طول\s*\(\s*(\w+)\s*\)\s*==\s*0)");
            if (std::regex_search(text, match, empty_check)) {
                diagnostics.push_back(create_diagnostic(
                    DiagnosticSeverity::HINT,
                    "ص-103",
                    "يمكن استخدام: فارغة(" + match[1].str() + ") بدلاً من ذلك",
                    line,
                    static_cast<int>(match.position()),
                    static_cast<int>(match.position() + match.length())
                ));
            }
        }
        
        return diagnostics;
    }
    
    /**
     * @brief التحقق من أن الكلمة محجوزة
     */
    bool is_reserved_word(const std::string& word) {
        static const std::unordered_set<std::string> reserved = {
            "إذا", "وإلا", "وإلا_إذا", "طالما", "لكل", "كرر",
            "دالة", "صنف", "واجهة", "وحدة", "ثابت",
            "إرجاع", "توقف", "استمر", "نهاية",
            "استورد", "من", "حاول", "امسك", "ارمِ",
            "جديد", "ذاتي", "فارغ", "صحيح", "خطأ",
            "رقم", "نص", "منطقي", "قائمة", "خريطة", "مجموعة", "كائن",
            "في", "و", "أو", "ليس"
        };
        return reserved.count(word) > 0;
    }
    
    /**
     * @brief التحقق من أن الاسم دالة مدمجة
     */
    bool is_builtin_function(const std::string& name) {
        static const std::unordered_set<std::string> builtins = {
            "اطبع", "اطبع_سطر", "ادخل",
            "إلى_رقم", "إلى_نص", "إلى_منطقي",
            "طول", "فارغة", "عكس", "رتب", "أضف", "احذف",
            "جذر", "قوة", "مطلق", "أقصى", "أدنى", "عشوائي",
            "قسّم", "اربط", "استبدل", "يحتوي", "يبدأ_بـ", "ينتهي_بـ",
            "أزل_الفراغات", "إلى_كبير", "إلى_صغير",
            "نوع", "هو_رقم", "هو_نص",
            "اقرأ_ملف", "اكتب_ملف", "ملف_موجود",
            "رئيسية"
        };
        return builtins.count(name) > 0;
    }
};

} // namespace lsp
} // namespace sad
