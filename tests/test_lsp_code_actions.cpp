/**
 * اختبار شامل لـ Code Actions في LSP — يحاكي سيناريو كامل من الأخطاء حتى الإصلاح
 * Comprehensive test for LSP Code Actions — simulates full error-to-fix pipeline
 *
 * يختبر:
 *   1. المحلل يكتشف أخطاء ويتعافى منها
 *   2. سجل التعافي يُترجم لتشخيصات LSP
 *   3. Code Actions تُولد Quick Fix صحيح لكل تشخيص
 *
 * الترجمة:
 *   cl /EHsc /utf-8 /std:c++17 ^
 *     /I shared/lexer/include /I shared/parser/include /I shared/ast/include ^
 *     /I shared/types/include /I shared/errors/include ^
 *     /I tools/lsp/include ^
 *     tests\test_lsp_code_actions.cpp ^
 *     /Fe:build\test_code_actions.exe ^
 *     /link build\lib\Debug\sad_core.lib build\lib\Debug\sad_lsp_engine.lib
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <regex>

// === من المحلل ===
#include "lexer_core.h"
#include "parser_core.h"
#include "error_recovery.h"

// === أنواع LSP (نعيد تعريفها لتجنب الاعتماد على المحرك الكامل) ===
namespace lsp_test {

struct Position { int line; int character; };
struct Range { Position start; Position end; };

enum class DiagnosticSeverity { Error = 1, Warning = 2, Information = 3, Hint = 4 };

struct Diagnostic {
    Range range;
    DiagnosticSeverity severity;
    std::string code;
    std::string source;
    std::string message;
    std::string message_ar;
};

enum class CodeActionKind { QuickFix, Refactor };

struct TextEdit {
    Range range;
    std::string new_text;
};

struct WorkspaceEdit {
    std::vector<TextEdit> edits;
};

struct CodeAction {
    std::string title;
    CodeActionKind kind;
    bool is_preferred;
    Diagnostic diagnostic;
    bool has_edit;
    WorkspaceEdit edit;
};

} // namespace lsp_test

// ═══════════════════════════════════════════════════════════════════════
//  مساعدات الاختبار
// ═══════════════════════════════════════════════════════════════════════

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    std::cout << "  اختبار: " << name << "... "; \

#define ASSERT_TRUE(cond, msg) \
    if (!(cond)) { \
        std::cerr << "❌ فشل: " << msg << "\n"; \
        tests_failed++; \
        return; \
    }

#define ASSERT_EQ(a, b, msg) \
    if ((a) != (b)) { \
        std::cerr << "❌ فشل: " << msg << " (توقعنا: " << (b) << ", حصلنا: " << (a) << ")\n"; \
        tests_failed++; \
        return; \
    }

#define PASS() \
    std::cout << "✅\n"; \
    tests_passed++;

// ═══════════════════════════════════════════════════════════════════════
//  محاكاة Pipeline: مصدر → محلل → تشخيصات → Code Actions
// ═══════════════════════════════════════════════════════════════════════

/// تحليل مصدر وجمع تشخيصات التعافي (يحاكي analysis_pipeline.cpp)
static std::vector<lsp_test::Diagnostic> parse_and_collect_diagnostics(const std::string& source) {
    std::vector<lsp_test::Diagnostic> diagnostics;

    Sad::Lexer::LexerCore lexer(source);
    Sad::Parser::ParserCore parser(lexer);
    auto program = parser.parseProgram();

    // أخطاء المحلل → Error
    if (parser.hasErrors()) {
        for (const auto& err : parser.getErrors()) {
            lsp_test::Diagnostic diag;
            diag.severity = lsp_test::DiagnosticSeverity::Error;
            diag.message = err;
            diag.source = "ص-محلل";
            diag.code = "ص-٠٠١";

            std::regex line_pattern(R"(\[.*?(\d+):(\d+)\])");
            std::smatch match;
            if (std::regex_search(err, match, line_pattern)) {
                int line = std::stoi(match[1].str()) - 1;
                int col = std::stoi(match[2].str()) - 1;
                diag.range.start = {std::max(0, line), std::max(0, col)};
                diag.range.end = {std::max(0, line), std::max(0, col) + 1};
            }
            diagnostics.push_back(diag);
        }
    }

    // نتائج التعافي → Warning
    const auto& recoveryLog = parser.getRecoveryLog();
    for (const auto& recovery : recoveryLog) {
        if (recovery.recovered) {
            lsp_test::Diagnostic diag;
            diag.severity = lsp_test::DiagnosticSeverity::Warning;
            diag.message = recovery.action.description;
            if (!recovery.action.descriptionEn.empty()) {
                diag.message += " | " + recovery.action.descriptionEn;
            }
            diag.source = "ص-تعافي";

            if (recovery.action.strategy == Sad::Errors::RecoveryStrategyType::INSERT_TOKEN) {
                diag.code = "ص-تعافي-INSERT:" + recovery.action.insertedText;
            } else {
                diag.code = "ص-تعافي-٠٠١";
            }

            diag.message_ar = recovery.action.description;
            int line = std::max(0, (int)recovery.line - 1);
            int col = std::max(0, (int)recovery.column - 1);
            diag.range.start = {line, col};
            diag.range.end = {line, col + 1};
            diagnostics.push_back(diag);
        }
    }

    return diagnostics;
}

/// توليد Code Actions لتشخيص (يحاكي code_actions_provider.cpp)
static std::vector<lsp_test::CodeAction> generate_code_actions(
    const std::vector<lsp_test::Diagnostic>& diagnostics)
{
    std::vector<lsp_test::CodeAction> actions;
    std::string prefix = "ص-تعافي-INSERT:";

    for (const auto& diag : diagnostics) {
        // تشخيص INSERT → Quick Fix مع edit
        if (diag.code.rfind(prefix, 0) == 0) {
            std::string inserted_text = diag.code.substr(prefix.size());
            if (!inserted_text.empty()) {
                lsp_test::CodeAction action;
                action.title = "🔧 إدراج '" + inserted_text + "' المفقودة";
                action.kind = lsp_test::CodeActionKind::QuickFix;
                action.is_preferred = true;
                action.diagnostic = diag;
                action.has_edit = true;

                lsp_test::TextEdit te;
                te.range.start = diag.range.start;
                te.range.end = diag.range.start; // insertion point
                te.new_text = inserted_text;
                action.edit.edits.push_back(te);
                actions.push_back(action);
            }
        }

        // تشخيص عام → إشعار فقط
        if (diag.code == "ص-تعافي-٠٠١") {
            lsp_test::CodeAction action;
            action.title = "🔍 " + diag.message;
            action.kind = lsp_test::CodeActionKind::QuickFix;
            action.is_preferred = false;
            action.diagnostic = diag;
            action.has_edit = false;
            actions.push_back(action);
        }
    }

    return actions;
}

/// تطبيق TextEdit على مصدر (يحاكي ما يفعله VS Code)
static std::string apply_edit(const std::string& source, const lsp_test::TextEdit& edit) {
    // تحويل المصدر لأسطر
    std::vector<std::string> lines;
    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    // تطبيق الإدراج في الموضع المحدد
    int ln = edit.range.start.line;
    int col = edit.range.start.character;
    if (ln >= 0 && ln < (int)lines.size()) {
        auto& target = lines[ln];
        // نحسب byte offset من character (بسيط — نفترض ASCII/UTF-8 متوافق)
        int byte_offset = 0;
        int char_count = 0;
        while (byte_offset < (int)target.size() && char_count < col) {
            unsigned char c = target[byte_offset];
            if (c < 0x80) byte_offset++;
            else if (c < 0xE0) byte_offset += 2;
            else if (c < 0xF0) byte_offset += 3;
            else byte_offset += 4;
            char_count++;
        }
        target.insert(byte_offset, edit.new_text);
    }

    // إعادة تجميع الأسطر
    std::string result;
    for (size_t i = 0; i < lines.size(); i++) {
        if (i > 0) result += "\n";
        result += lines[i];
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════
//  الاختبارات
// ═══════════════════════════════════════════════════════════════════════

/// اختبار 1: دالة بقوس مفقود → تشخيص Warning + Quick Fix
void test_missing_paren() {
    TEST("قوس ')' مفقود في استدعاء دالة")

    // اطبع("مرحبا"  — بدون )
    std::string source = "اطبع(\"مرحبا\"\n";

    auto diagnostics = parse_and_collect_diagnostics(source);

    // يجب أن يكون هناك تشخيص تعافي واحد على الأقل
    bool has_recovery = false;
    lsp_test::Diagnostic recovery_diag;
    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي" && d.code.find("INSERT:") != std::string::npos) {
            has_recovery = true;
            recovery_diag = d;
            break;
        }
    }
    ASSERT_TRUE(has_recovery, "لم يُكتشف تشخيص تعافي INSERT")
    ASSERT_TRUE(recovery_diag.severity == lsp_test::DiagnosticSeverity::Warning,
                "التشخيص يجب أن يكون Warning")

    // توليد Code Actions
    auto actions = generate_code_actions({recovery_diag});
    ASSERT_TRUE(!actions.empty(), "يجب أن يُولد Code Action واحد على الأقل")
    ASSERT_TRUE(actions[0].is_preferred, "يجب أن يكون preferred Quick Fix")
    ASSERT_TRUE(actions[0].has_edit, "يجب أن يكون له edit")
    ASSERT_TRUE(actions[0].title.find("المفقودة") != std::string::npos,
                "العنوان يجب أن يحتوي 'المفقودة'")

    // التحقق من أن الـ edit يحتوي النص الصحيح
    ASSERT_TRUE(!actions[0].edit.edits.empty(), "يجب أن يكون هناك edit واحد على الأقل")
    ASSERT_EQ(actions[0].edit.edits[0].new_text, ")", "النص المُدرج يجب أن يكون ')'")

    PASS()
}

/// اختبار 2: مصفوفة بقوس ] مفقود → تشخيص + Quick Fix
void test_missing_bracket() {
    TEST("قوس ']' مفقود في مصفوفة")

    // متغير م = [1، 2، 3  — بدون ]
    std::string source = "متغير م = [1، 2، 3\n";

    auto diagnostics = parse_and_collect_diagnostics(source);

    bool has_bracket_recovery = false;
    for (const auto& d : diagnostics) {
        if (d.code.find("INSERT:]") != std::string::npos ||
            d.code.find("INSERT:)") != std::string::npos) {
            has_bracket_recovery = true;
            break;
        }
    }
    ASSERT_TRUE(has_bracket_recovery, "لم يُكتشف تعافي لـ ] أو )")

    auto warnings_only = std::vector<lsp_test::Diagnostic>();
    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي") warnings_only.push_back(d);
    }

    auto actions = generate_code_actions(warnings_only);
    ASSERT_TRUE(!actions.empty(), "يجب أن يُولد Code Action")

    // كل action يجب أن يكون QuickFix
    for (const auto& a : actions) {
        ASSERT_TRUE(a.kind == lsp_test::CodeActionKind::QuickFix,
                    "يجب أن يكون نوعه QuickFix")
    }

    PASS()
}

/// اختبار 3: عدة أخطاء في ملف واحد → عدة Code Actions
void test_multiple_recoveries() {
    TEST("عدة أخطاء وتعافيات في ملف واحد")

    // دالة بقوس مفقود + مصفوفة بقوس مفقود
    std::string source =
        "دالة سليمة()\n"
        "   اطبع(\"مرحبا\"\n"  // ) مفقود
        "نهاية\n"
        "\n"
        "متغير م = [1، 2، 3\n";  // ] مفقود

    auto diagnostics = parse_and_collect_diagnostics(source);

    // عد تشخيصات التعافي
    int recovery_count = 0;
    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي") recovery_count++;
    }
    ASSERT_TRUE(recovery_count >= 2, "يجب أن يكون هناك تعافيان على الأقل")

    // توليد Code Actions لكل التشخيصات
    std::vector<lsp_test::Diagnostic> recovery_diags;
    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي") recovery_diags.push_back(d);
    }
    auto actions = generate_code_actions(recovery_diags);
    ASSERT_TRUE(actions.size() >= 2, "يجب أن يكون هناك إجراءان على الأقل")

    PASS()
}

/// اختبار 4: التحقق من أن تنويع أخطاء المحلل يُولد Error وليس Warning
void test_errors_vs_warnings() {
    TEST("أخطاء المحلل = Error، تعافيات = Warning")

    std::string source = "اطبع(\"مرحبا\"\n";

    auto diagnostics = parse_and_collect_diagnostics(source);

    for (const auto& d : diagnostics) {
        if (d.source == "ص-محلل") {
            ASSERT_TRUE(d.severity == lsp_test::DiagnosticSeverity::Error,
                        "أخطاء المحلل يجب أن تكون Error")
        }
        if (d.source == "ص-تعافي") {
            ASSERT_TRUE(d.severity == lsp_test::DiagnosticSeverity::Warning,
                        "تشخيصات التعافي يجب أن تكون Warning")
        }
    }

    PASS()
}

/// اختبار 5: كود صحيح → لا تشخيصات تعافي ولا Code Actions
void test_clean_code() {
    TEST("كود صحيح بدون أخطاء → لا تشخيصات تعافي")

    std::string source =
        "دالة جمع(أ، ب)\n"
        "   ارجع أ + ب\n"
        "نهاية\n"
        "متغير ن = جمع(3، 5)\n"
        "اطبع(ن)\n";

    auto diagnostics = parse_and_collect_diagnostics(source);

    int recovery_count = 0;
    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي") recovery_count++;
    }
    ASSERT_EQ(recovery_count, 0, "لا يجب أن يكون هناك تعافي لكود صحيح")

    PASS()
}

/// اختبار 6: Code Action يحتوي الكود الصحيح في diag.code
void test_diag_code_format() {
    TEST("صيغة كود التشخيص: ص-تعافي-INSERT:نص")

    std::string source = "اطبع(\"مرحبا\"\n";

    auto diagnostics = parse_and_collect_diagnostics(source);

    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي") {
            std::string prefix = "ص-تعافي-INSERT:";
            if (d.code.rfind(prefix, 0) == 0) {
                std::string token = d.code.substr(prefix.size());
                ASSERT_TRUE(!token.empty(), "النص المُدرج يجب ألا يكون فارغاً")
                std::cout << "(رمز: '" << token << "') ";
            }
        }
    }

    PASS()
}

/// اختبار 7: التحقق من أن Quick Fix يحتوي بيانات Edit صحيحة
void test_fix_edit_data() {
    TEST("Quick Fix يحتوي بيانات Edit صحيحة")

    std::string source = "اطبع(\"مرحبا\"\n";

    auto diagnostics = parse_and_collect_diagnostics(source);

    bool found_fix = false;
    for (const auto& d : diagnostics) {
        if (d.source == "ص-تعافي" && d.code.find("INSERT:") != std::string::npos) {
            auto actions = generate_code_actions({d});
            if (!actions.empty() && actions[0].has_edit && !actions[0].edit.edits.empty()) {
                const auto& te = actions[0].edit.edits[0];

                // التحقق: الإدراج في نقطة (range.start == range.end)
                ASSERT_TRUE(te.range.start.line == te.range.end.line,
                            "Edit يجب أن يكون إدراجاً في نفس السطر")
                ASSERT_TRUE(te.range.start.character == te.range.end.character,
                            "Edit يجب أن يكون إدراجاً (start == end)")

                // التحقق: النص المُدرج صحيح
                ASSERT_EQ(te.new_text, ")", "النص المُدرج يجب أن يكون ')'")

                // التحقق: الموضع في السطر الأول (0-indexed)
                ASSERT_TRUE(te.range.start.line >= 0, "السطر يجب أن يكون >= 0")

                found_fix = true;
                std::cout << "(سطر: " << te.range.start.line
                          << ", عمود: " << te.range.start.character << ") ";
            }
        }
    }
    ASSERT_TRUE(found_fix, "يجب أن نجد Quick Fix واحداً على الأقل")

    PASS()
}

/// اختبار 8: getRecoveryLog() يعمل عبر ParserCore العام
void test_recovery_log_accessor() {
    TEST("getRecoveryLog() يكشف سجل التعافي عبر API عام")

    std::string source = "اطبع(\"مرحبا\"\n";

    Sad::Lexer::LexerCore lexer(source);
    Sad::Parser::ParserCore parser(lexer);
    parser.parseProgram();

    const auto& log = parser.getRecoveryLog();
    // يجب أن يكون هناك سجل واحد على الأقل
    bool has_insert = false;
    for (const auto& entry : log) {
        if (entry.recovered &&
            entry.action.strategy == Sad::Errors::RecoveryStrategyType::INSERT_TOKEN) {
            has_insert = true;
            ASSERT_TRUE(!entry.action.insertedText.empty(),
                        "insertedText يجب ألا يكون فارغاً")
            ASSERT_TRUE(entry.line > 0, "line يجب أن يكون > 0")
        }
    }
    ASSERT_TRUE(has_insert, "يجب أن يكون هناك INSERT_TOKEN في السجل")

    // getRecoveryStats() يجب أن يعمل
    std::string stats = parser.getRecoveryStats(true);
    ASSERT_TRUE(stats.find("ناجح") != std::string::npos,
                "الإحصائيات يجب أن تحتوي 'ناجح'")

    PASS()
}

// ═══════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ═══════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║   اختبار شامل: LSP Code Actions + نظام التعافي          ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    test_missing_paren();
    test_missing_bracket();
    test_multiple_recoveries();
    test_errors_vs_warnings();
    test_clean_code();
    test_diag_code_format();
    test_fix_edit_data();
    test_recovery_log_accessor();

    std::cout << "\n═══════════════════════════════════════════════════════════\n";
    std::cout << "  النتيجة: " << tests_passed << " ✅ نجح، " << tests_failed << " ❌ فشل\n";
    std::cout << "═══════════════════════════════════════════════════════════\n";

    return tests_failed > 0 ? 1 : 0;
}
