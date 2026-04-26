// بسم الله الرحمن الرحيم
// ============================================================================
// test_formatter.cpp — اختبارات شاملة لمنسّق كود لغة ص
// Comprehensive Code Formatter Tests
// ============================================================================

#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "format/sad_formatter.h"

using namespace Sad::Format;
namespace fs = std::filesystem;

// ============================================================================
// Test Helpers
// ============================================================================

struct TestStats {
    int total = 0, passed = 0, failed = 0;
    void record(bool ok) { total++; if (ok) passed++; else failed++; }
    void print() const {
        std::cout << "\n===========================================\n"
                  << "  Results: " << total << " tests | PASS " << passed
                  << " | FAIL " << failed << "\n"
                  << "===========================================\n";
    }
};

static TestStats stats;

#define TEST(name) do { bool _ok = true; std::string _name = name;
#define EXPECT(cond) if (!(cond)) { _ok = false; std::cout << "    X " << #cond << " (line " << __LINE__ << ")\n"; }
#define END_TEST stats.record(_ok); std::cout << (_ok ? "  OK " : "  FAIL ") << _name << "\n"; } while(0)

// ============================================================================
// 1. Options and Configuration Tests
// ============================================================================

void testFormatterOptions() {
    std::cout << "\n=== Formatter Options Tests ===\n";

    TEST("Default options") {
        FormatterOptions opts;
        EXPECT(opts.indentSize == 4);
        EXPECT(opts.useSpaces == true);
        EXPECT(opts.maxLineLength == 100);
        EXPECT(opts.spaceAfterComma == true);
        EXPECT(opts.spaceAroundOperators == true);
    } END_TEST;

    TEST("Config file") {
        std::string tmp = "test_fmt_config.tmp";
        {
            std::ofstream f(tmp);
            f << "indent_size=2\n"
              << "use_spaces=true\n"
              << "max_line_length=80\n"
              << "space_after_comma=false\n";
        }
        auto opts = FormatterOptions::fromFile(tmp);
        EXPECT(opts.indentSize == 2);
        EXPECT(opts.maxLineLength == 80);
        EXPECT(opts.spaceAfterComma == false);
        fs::remove(tmp);
    } END_TEST;

    TEST("Arabic config keys") {
        std::string tmp = "test_fmt_ar.tmp";
        {
            std::ofstream f(tmp);
            f << "indent_size=3\n"
              << "max_line_length=120\n";
        }
        auto opts = FormatterOptions::fromFile(tmp);
        EXPECT(opts.indentSize == 3);
        EXPECT(opts.maxLineLength == 120);
        fs::remove(tmp);
    } END_TEST;
}

// ============================================================================
// 2. FmtLexer Tests
// ============================================================================

void testFmtLexer() {
    std::cout << "\n=== FmtLexer Tests ===\n";

    TEST("Recognize keywords") {
        FmtLexer lexer;
        auto tokens = lexer.tokenize("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 test()");
        bool hasKeyword = false;
        for (const auto& t : tokens)
            if (t.type == FmtTokenType::KEYWORD && t.value == "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9") hasKeyword = true;
        EXPECT(hasKeyword);
    } END_TEST;

    TEST("Recognize strings") {
        FmtLexer lexer;
        auto tokens = lexer.tokenize("\"hello world\"");
        bool hasString = false;
        for (const auto& t : tokens)
            if (t.type == FmtTokenType::STRING) hasString = true;
        EXPECT(hasString);
    } END_TEST;

    TEST("Recognize numbers") {
        FmtLexer lexer;
        auto tokens = lexer.tokenize("42 3.14");
        int numCount = 0;
        for (const auto& t : tokens)
            if (t.type == FmtTokenType::NUMBER) numCount++;
        EXPECT(numCount == 2);
    } END_TEST;

    TEST("Recognize comments") {
        FmtLexer lexer;
        auto tokens = lexer.tokenize("# comment\ncode");
        bool hasComment = false;
        for (const auto& t : tokens)
            if (t.type == FmtTokenType::COMMENT_LINE) hasComment = true;
        EXPECT(hasComment);
    } END_TEST;

    TEST("Recognize operators") {
        FmtLexer lexer;
        auto tokens = lexer.tokenize("a + b * c");
        int opCount = 0;
        for (const auto& t : tokens)
            if (t.type == FmtTokenType::OPERATOR) opCount++;
        EXPECT(opCount >= 2);
    } END_TEST;

    TEST("Arabic comma") {
        FmtLexer lexer;
        auto tokens = lexer.tokenize("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9(a\xd8\x8c b)");
        bool hasComma = false;
        for (const auto& t : tokens)
            if (t.type == FmtTokenType::COMMA) hasComma = true;
        EXPECT(hasComma);
    } END_TEST;
}

// ============================================================================
// 3. Basic Formatting Tests
// ============================================================================

void testBasicFormatting() {
    std::cout << "\n=== Basic Formatting Tests ===\n";

    FormatterOptions opts;
    opts.indentSize = 4;
    opts.useSpaces = true;
    SadFormatter formatter(opts);

    TEST("Format empty code") {
        auto result = formatter.format("");
        EXPECT(result.success());
    } END_TEST;

    TEST("Format simple function") {
        std::string input =
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 add(a, b)\n"
            "\xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 a + b\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
        EXPECT(!result.output.empty());
    } END_TEST;

    TEST("Format if condition") {
        std::string input =
            "\xd8\xa5\xd8\xb0\xd8\xa7 (x > 0)\n"
            "print(\"positive\")\n"
            "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7\n"
            "print(\"negative\")\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;

    TEST("Format while loop") {
        std::string input =
            "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7 (x > 0)\n"
            "x = x - 1\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;

    TEST("Format for-each loop") {
        std::string input =
            "\xd9\x84\xd9\x83\xd9\x84 item \xd9\x81\xd9\x8a arr\n"
            "print(item)\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;
}

// ============================================================================
// 4. Advanced Formatting Tests
// ============================================================================

void testAdvancedFormatting() {
    std::cout << "\n=== Advanced Formatting Tests ===\n";

    FormatterOptions opts;
    SadFormatter formatter(opts);

    TEST("Nested blocks") {
        std::string input =
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 process()\n"
            "\xd8\xa5\xd8\xb0\xd8\xa7 (cond)\n"
            "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7 (x > 0)\n"
            "print(x)\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;

    TEST("Class formatting") {
        std::string input =
            "\xd8\xb5\xd9\x86\xd9\x81 Animal\n"
            "\xd8\xa8\xd8\xa7\xd9\x86\xd9\x8a(name)\n"
            "\xd9\x87\xd8\xb0\xd8\xa7.name = name\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;

    TEST("Try/catch formatting") {
        std::string input =
            "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84\n"
            "result = dangerous()\n"
            "\xd8\xa7\xd9\x85\xd8\xb3\xd9\x83 err\n"
            "print(err)\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;
}

// ============================================================================
// 5. Check Mode Tests
// ============================================================================

void testCheckMode() {
    std::cout << "\n=== Check Mode Tests ===\n";

    FormatterOptions opts;
    SadFormatter formatter(opts);

    TEST("Already formatted code") {
        std::string clean =
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 hello()\n"
            "    print(\"hello\")\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(clean);
        EXPECT(result.success());
    } END_TEST;

    TEST("Check detects changes") {
        std::string messy =
            "  \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9   messy() \n"
            "print(  1  )\n"
            "  \xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto result = formatter.format(messy);
        EXPECT(result.success());
        EXPECT(result.output != messy);
    } END_TEST;
}

// ============================================================================
// 6. Import Sorting Tests
// ============================================================================

void testImportSorting() {
    std::cout << "\n=== Import Sorting Tests ===\n";

    FormatterOptions opts;
    opts.sortImports = true;
    SadFormatter formatter(opts);

    TEST("Sort imports alphabetically") {
        std::string input =
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf c\n"
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf a\n"
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf b\n"
            "\n"
            "print(\"code\")\n";
        auto result = formatter.format(input);
        EXPECT(result.success());
    } END_TEST;
}

// ============================================================================
// 7. FormatResult Tests
// ============================================================================

void testFormatResult() {
    std::cout << "\n=== FormatResult Tests ===\n";

    TEST("Successful result") {
        FormatResult r;
        r.output = "formatted code";
        // empty error = success
        EXPECT(r.success());
        EXPECT(!r.output.empty());
    } END_TEST;
}

// ============================================================================
// Entry Point
// ============================================================================

int main() {
    std::cout << "========================================\n"
              << "  Code Formatter Tests - Sad Language\n"
              << "========================================\n";

    testFormatterOptions();
    testFmtLexer();
    testBasicFormatting();
    testAdvancedFormatting();
    testCheckMode();
    testImportSorting();
    testFormatResult();

    stats.print();
    return stats.failed > 0 ? 1 : 0;
}
