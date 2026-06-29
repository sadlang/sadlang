// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: known_issues_detector.cpp
// الوصف: كاشف المشاكل المعروفة — يحذر المبرمج من الأنماط الإشكالية الموثقة
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يكشف الأنماط الإشكالية الموثقة في مشاكل.md ويحذر المبرمج منها
// حتى يتم إصلاحها في النواة.
//
// المشاكل المكتشفة:
//   [P2]  متغير x = x + 1 داخل بينما → حلقة لانهائية
//   [P1]  array[i]["key"] داخل دالة → crash محتمل
//   [P3]  .الطول() في عمق استدعاء ≥ 2 → crash محتمل
//   [P18] هذا.method() داخل method آخر → تغييرات مفقودة
//   [P9]  كلمات محجوزة غير موثقة
//   [P19] hex literals غير مدعومة
//   [P12] القسمة كفهرس مصفوفة
//
// @author عمر — مهندس البنية التحتية
// @date 2026-03-06
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <regex>
#include <unordered_set>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  الكلمات المحجوزة غير الموثقة — P9/P17/P20/P21
// ══════════════════════════════════════════════════════════════════════════════

/// الكلمات المحجوزة التي ليست موثقة بوضوح ولكنها تسبب أخطاء
static const std::unordered_set<std::string>& get_undocumented_reserved_words() {
    static std::unordered_set<std::string> words = {
        // P9: كلمات محجوزة غير موثقة
        "\xd9\x86\xd9\x88\xd8\xb9",      // نوع
        "\xd8\xad\xd8\xac\xd9\x85",      // حجم
        
        // P17: كلمات محجوزة أخرى
        "\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf",  // جديد (مستخدمة لإنشاء كائنات)
        "\xd8\xb3\xd8\xac\xd9\x84",          // سجل
        
        // P20: KEYWORD_SUPER
        "\xd8\xa3\xd8\xb3\xd8\xa7\xd8\xb3",  // أساس
        
        // P21: KEYWORD_TEMPLATE — FIXED: قالب is now contextual (not reserved)
        // "\xd9\x82\xd8\xa7\xd9\x84\xd8\xa8",  // قالب — removed after Bug #21 fix
    };
    return words;
}

// ══════════════════════════════════════════════════════════════════════════════
//  كاشف المشاكل المعروفة
// ══════════════════════════════════════════════════════════════════════════════

class KnownIssuesDetector {
public:
    /// تحليل الكود وإنتاج تحذيرات للمشاكل المعروفة
    static std::vector<Diagnostic> detect(const std::string& content, const DocumentUri& uri) {
        std::vector<Diagnostic> diagnostics;
        
        auto lines = arabic::split_lines(content);
        
        for (size_t line_num = 0; line_num < lines.size(); ++line_num) {
            const auto& line = lines[line_num];
            
            // ── P2: متغير x = x + 1 داخل بينما ──
            detect_p2_while_variable_shadow(line, line_num, lines, diagnostics);
            
            // ── P1: array[i]["key"] داخل دالة ──
            detect_p1_chain_index_crash(line, line_num, lines, diagnostics);
            
            // ── P18: هذا.method() داخل method ──
            detect_p18_this_method_call(line, line_num, lines, diagnostics);
            
            // ── P9/P17/P20/P21: كلمات محجوزة غير موثقة في معاملات ──
            detect_undocumented_reserved_as_param(line, line_num, diagnostics);
            
            // ── P19: hex literals ──
            detect_p19_hex_literals(line, line_num, diagnostics);
            
            // ── P12: استخدام القسمة كفهرس ──
            detect_p12_division_as_index(line, line_num, diagnostics);
            
            // ── P3: .الطول() في سياق عميق ──
            detect_p3_length_deep_context(line, line_num, lines, diagnostics);
        }
        
        return diagnostics;
    }

private:
    // ──────────────────────────────────────────────────────────────────────────
    // P2: متغير x = x + 1 داخل بينما → إنشاء متغير محلي جديد بدل التحديث
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_p2_while_variable_shadow(
        const std::string& line, 
        size_t line_num,
        const std::vector<std::string>& lines,
        std::vector<Diagnostic>& diagnostics
    ) {
        // نبحث عن: متغير اسم = اسم + ...
        // يجب التحقق أننا داخل حلقة بينما أو لكل
        
        static std::regex pattern_ar(
            R"(\s*(?:\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1)\s+(\S+)\s*=\s*\1\s*[\+\-])"
        );  // متغير اسم = اسم +/-
        
        std::smatch match;
        if (std::regex_search(line, match, pattern_ar)) {
            // التحقق أننا داخل حلقة
            if (is_inside_loop(lines, line_num)) {
                Diagnostic diag;
                diag.range.start = {static_cast<int>(line_num), 0};
                diag.range.end = {static_cast<int>(line_num), static_cast<int>(line.size())};
                diag.severity = DiagnosticSeverity::Warning;
                diag.code = "SAD-P2";
                diag.source = "sad-lsp-issues";
                diag.message = 
                    u8"⚠️ مشكلة معروفة P2: استخدام 'متغير' داخل حلقة يُنشئ متغيراً محلياً جديداً.\n"
                    u8"💡 الحل: أزل كلمة 'متغير' لتحديث المتغير الموجود، أو استخدم مصفوفة كعداد.\n"
                    u8"📖 راجع: مشاكل.md#المشكلة-2";
                diagnostics.push_back(diag);
            }
        }
    }
    
    // ──────────────────────────────────────────────────────────────────────────
    // P1: array[i]["key"] داخل دالة → crash محتمل
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_p1_chain_index_crash(
        const std::string& line, 
        size_t line_num,
        const std::vector<std::string>& lines,
        std::vector<Diagnostic>& diagnostics
    ) {
        // نبحث عن نمط: اسم[...]["..."] أو اسم[...][...]
        static std::regex pattern(R"((\w+)\s*\[[^\]]+\]\s*\[[^\]]+\])");
        
        std::smatch match;
        if (std::regex_search(line, match, pattern)) {
            // تحقق أننا داخل دالة
            if (is_inside_function(lines, line_num)) {
                Diagnostic diag;
                diag.range.start = {static_cast<int>(line_num), static_cast<int>(match.position())};
                diag.range.end = {static_cast<int>(line_num), static_cast<int>(match.position() + match.length())};
                diag.severity = DiagnosticSeverity::Warning;
                diag.code = "SAD-P1";
                diag.source = "sad-lsp-issues";
                diag.message = 
                    u8"⚠️ مشكلة معروفة P1: الوصول المزدوج array[i][\"key\"] داخل دالة قد يسبب انهياراً.\n"
                    u8"💡 الحل: استخدم متغير وسيط:\n"
                    u8"   متغير عنصر = المصفوفة[فهرس]\n"
                    u8"   متغير قيمة = عنصر[\"مفتاح\"]\n"
                    u8"📖 راجع: مشاكل.md#المشكلة-1";
                diagnostics.push_back(diag);
            }
        }
    }
    
    // ──────────────────────────────────────────────────────────────────────────
    // P18: هذا.method() داخل method آخر → تغييرات مفقودة
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_p18_this_method_call(
        const std::string& line, 
        size_t line_num,
        const std::vector<std::string>& lines,
        std::vector<Diagnostic>& diagnostics
    ) {
        static std::string kw_this = "\xd9\x87\xd8\xb0\xd8\xa7"; // هذا
        
        // نبحث عن: هذا.اسم(
        static std::regex pattern(R"(\xd9\x87\xd8\xb0\xd8\xa7\.(\w+)\s*\()");
        
        std::smatch match;
        if (std::regex_search(line, match, pattern)) {
            // تحقق أننا داخل دالة في صنف
            if (is_inside_class_method(lines, line_num)) {
                Diagnostic diag;
                diag.range.start = {static_cast<int>(line_num), static_cast<int>(match.position())};
                diag.range.end = {static_cast<int>(line_num), static_cast<int>(match.position() + match.length())};
                diag.severity = DiagnosticSeverity::Hint;
                diag.code = "SAD-P18";
                diag.source = "sad-lsp-issues";
                diag.message = 
                    u8"💡 تنبيه P18: استدعاء هذا.method() داخل method آخر قد لا يحفظ التغييرات.\n"
                    u8"📖 راجع: مشاكل.md#المشكلة-18";
                diagnostics.push_back(diag);
            }
        }
    }
    
    // ──────────────────────────────────────────────────────────────────────────
    // P9/P17/P20/P21: كلمات محجوزة غير موثقة كمعاملات
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_undocumented_reserved_as_param(
        const std::string& line, 
        size_t line_num,
        std::vector<Diagnostic>& diagnostics
    ) {
        static std::string kw_func = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
        
        // نبحث عن تعريف دالة
        if (line.find(kw_func) == std::string::npos) return;
        
        // استخراج المعاملات بين القوسين
        size_t paren_start = line.find('(');
        size_t paren_end = line.find(')');
        if (paren_start == std::string::npos || paren_end == std::string::npos) return;
        
        std::string params = line.substr(paren_start + 1, paren_end - paren_start - 1);
        
        // فحص كل كلمة محجوزة غير موثقة
        for (const auto& reserved : get_undocumented_reserved_words()) {
            if (params.find(reserved) != std::string::npos) {
                Diagnostic diag;
                diag.range.start = {static_cast<int>(line_num), static_cast<int>(paren_start)};
                diag.range.end = {static_cast<int>(line_num), static_cast<int>(paren_end)};
                diag.severity = DiagnosticSeverity::Error;
                diag.code = "SAD-RESERVED";
                diag.source = "sad-lsp-issues";
                diag.message = 
                    u8"❌ كلمة '" + reserved + u8"' محجوزة ولا يمكن استخدامها كاسم معامل.\n"
                    u8"💡 استخدم اسماً بديلاً.";
                diagnostics.push_back(diag);
            }
        }
    }
    
    // ──────────────────────────────────────────────────────────────────────────
    // P19: hex literals غير مدعومة
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_p19_hex_literals(
        const std::string& line, 
        size_t line_num,
        std::vector<Diagnostic>& diagnostics
    ) {
        static std::regex hex_pattern(R"(0x[0-9A-Fa-f]+)");
        
        std::smatch match;
        std::string::const_iterator search_start(line.cbegin());
        
        while (std::regex_search(search_start, line.cend(), match, hex_pattern)) {
            Diagnostic diag;
            size_t pos = match.position() + (search_start - line.cbegin());
            diag.range.start = {static_cast<int>(line_num), static_cast<int>(pos)};
            diag.range.end = {static_cast<int>(line_num), static_cast<int>(pos + match.length())};
            diag.severity = DiagnosticSeverity::Error;
            diag.code = "SAD-P19";
            diag.source = "sad-lsp-issues";
            
            // حساب القيمة العشرية
            std::string hex_str = match.str();
            unsigned long decimal = std::stoul(hex_str, nullptr, 16);
            
            diag.message = 
                u8"❌ مشكلة P19: الأرقام الست عشرية (" + hex_str + u8") غير مدعومة حالياً.\n"
                u8"💡 الحل: استخدم القيمة العشرية: " + std::to_string(decimal);
            diagnostics.push_back(diag);
            
            search_start = match.suffix().first;
        }
    }
    
    // ──────────────────────────────────────────────────────────────────────────
    // P12: استخدام نتيجة القسمة كفهرس مصفوفة
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_p12_division_as_index(
        const std::string& line, 
        size_t line_num,
        std::vector<Diagnostic>& diagnostics
    ) {
        // نبحث عن: اسم[... / ...]
        static std::regex pattern(R"((\w+)\s*\[\s*[^/\]]+\s*/\s*[^\]]+\])");
        
        std::smatch match;
        if (std::regex_search(line, match, pattern)) {
            Diagnostic diag;
            diag.range.start = {static_cast<int>(line_num), static_cast<int>(match.position())};
            diag.range.end = {static_cast<int>(line_num), static_cast<int>(match.position() + match.length())};
            diag.severity = DiagnosticSeverity::Warning;
            diag.code = "SAD-P12";
            diag.source = "sad-lsp-issues";
            diag.message = 
                u8"⚠️ مشكلة P12: نتيجة القسمة قد تكون عشرية وتسبب خطأ عند استخدامها كفهرس.\n"
                u8"💡 الحل: استخدم دالة صحيح() للتحويل:\n"
                u8"   مصفوفة[صحيح(8 / 4)]";
            diagnostics.push_back(diag);
        }
    }
    
    // ──────────────────────────────────────────────────────────────────────────
    // P3: .الطول() في سياق عميق
    // ──────────────────────────────────────────────────────────────────────────
    static void detect_p3_length_deep_context(
        const std::string& line, 
        size_t line_num,
        const std::vector<std::string>& lines,
        std::vector<Diagnostic>& diagnostics
    ) {
        static std::string kw_length = ".\xd8\xa7\xd9\x84\xd8\xb7\xd9\x88\xd9\x84()"; // .الطول()
        
        if (line.find(kw_length) != std::string::npos) {
            // تحقق من عمق الاستدعاء
            int depth = calculate_call_depth(lines, line_num);
            if (depth >= 2) {
                size_t pos = line.find(kw_length);
                Diagnostic diag;
                diag.range.start = {static_cast<int>(line_num), static_cast<int>(pos)};
                diag.range.end = {static_cast<int>(line_num), static_cast<int>(pos + 9)}; // طول النص
                diag.severity = DiagnosticSeverity::Warning;
                diag.code = "SAD-P3";
                diag.source = "sad-lsp-issues";
                diag.message = 
                    u8"⚠️ مشكلة P3: الطول() في عمق استدعاء ≥ 2 قد يسبب انهياراً.\n"
                    u8"💡 الحل: احسب الطول في الدالة الأعلى ومرره كمعامل.";
                diagnostics.push_back(diag);
            }
        }
    }
    
    // ══════════════════════════════════════════════════════════════════════════
    //  دوال مساعدة
    // ══════════════════════════════════════════════════════════════════════════
    
    static bool is_inside_loop(const std::vector<std::string>& lines, size_t line_num) {
        static std::string kw_while = "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7"; // بينما
        static std::string kw_for = "\xd9\x84\xd9\x83\xd9\x84"; // لكل
        
        // بحث عكسي عن بينما أو لكل
        for (int i = static_cast<int>(line_num) - 1; i >= 0; --i) {
            if (lines[i].find(kw_while) != std::string::npos ||
                lines[i].find(kw_for) != std::string::npos) {
                return true;
            }
            // توقف عند بداية دالة
            if (lines[i].find("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9") != std::string::npos) {
                break;
            }
        }
        return false;
    }
    
    static bool is_inside_function(const std::vector<std::string>& lines, size_t line_num) {
        static std::string kw_func = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
        
        for (int i = static_cast<int>(line_num) - 1; i >= 0; --i) {
            if (lines[i].find(kw_func) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    static bool is_inside_class_method(const std::vector<std::string>& lines, size_t line_num) {
        static std::string kw_class = "\xd8\xb5\xd9\x86\xd9\x81"; // صنف
        static std::string kw_func = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
        
        bool found_func = false;
        bool found_class = false;
        
        for (int i = static_cast<int>(line_num) - 1; i >= 0; --i) {
            if (!found_func && lines[i].find(kw_func) != std::string::npos) {
                found_func = true;
            }
            if (lines[i].find(kw_class) != std::string::npos) {
                found_class = true;
                break;
            }
        }
        
        return found_func && found_class;
    }
    
    static int calculate_call_depth(const std::vector<std::string>& lines, size_t line_num) {
        static std::string kw_func = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
        
        int depth = 0;
        for (int i = static_cast<int>(line_num); i >= 0; --i) {
            if (lines[i].find(kw_func) != std::string::npos) {
                depth++;
            }
        }
        return depth;
    }
};

// ══════════════════════════════════════════════════════════════════════════════
//  دالة التكامل مع خط أنابيب التحليل
// ══════════════════════════════════════════════════════════════════════════════

/// تُستدعى من AnalysisPipeline::analyze() لإضافة تشخيصات المشاكل المعروفة
std::vector<Diagnostic> detect_known_issues(const std::string& content, const DocumentUri& uri) {
    return KnownIssuesDetector::detect(content, uri);
}

} // namespace lsp
} // namespace sad
