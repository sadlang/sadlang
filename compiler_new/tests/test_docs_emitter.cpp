// ============================================================================
// test_docs_emitter.cpp - Documentation Emitter Tests
// ============================================================================

#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "meta/docs_emitter.h"

using namespace Sad::Meta;
namespace fs = std::filesystem;

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
// 1. DocEntry Tests
// ============================================================================

void testDocEntry() {
    std::cout << "\n=== DocEntry Tests ===\n";

    TEST("Kind names in Arabic") {
        DocEntry e;
        e.kind = DocItemKind::Function;
        EXPECT(e.kindNameAr().size() > 0);
        e.kind = DocItemKind::Class;
        EXPECT(e.kindNameAr().size() > 0);
        e.kind = DocItemKind::Variable;
        EXPECT(e.kindNameAr().size() > 0);
    } END_TEST;

    TEST("Kind names in English") {
        DocEntry e;
        e.kind = DocItemKind::Function;
        EXPECT(e.kindNameEn() == "function");
        e.kind = DocItemKind::Class;
        EXPECT(e.kindNameEn() == "class");
    } END_TEST;
}

// ============================================================================
// 2. DocCommentParser Tests
// ============================================================================

void testDocCommentParser() {
    std::cout << "\n=== DocCommentParser Tests ===\n";

    DocCommentParser parser;

    TEST("Parse documented function") {
        std::string code =
            "## function to add two numbers\n"
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 add(a, b)\n"
            "    \xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 a + b\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto entries = parser.parseSource(code, "test.s");
        EXPECT(!entries.empty());
        if (!entries.empty()) {
            EXPECT(entries[0].kind == DocItemKind::Function);
            EXPECT(!entries[0].description.empty());
        }
    } END_TEST;

    TEST("Parse documented class") {
        std::string code =
            "## A class representing an animal\n"
            "\xd8\xb5\xd9\x86\xd9\x81 Animal\n"
            "    \xd8\xa8\xd8\xa7\xd9\x86\xd9\x8a(name)\n"
            "        \xd9\x87\xd8\xb0\xd8\xa7.name = name\n"
            "    \xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto entries = parser.parseSource(code, "test.s");
        bool foundClass = false;
        for (const auto& e : entries)
            if (e.kind == DocItemKind::Class) foundClass = true;
        EXPECT(foundClass);
    } END_TEST;

    TEST("Code without doc comments - empty") {
        std::string code =
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 secret()\n"
            "    \xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 42\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        auto entries = parser.parseSource(code, "test.s");
        EXPECT(entries.empty());
    } END_TEST;
}

// ============================================================================
// 3. DocsEmitter Tests
// ============================================================================

void testDocsEmitter() {
    std::cout << "\n=== DocsEmitter Tests ===\n";

    TEST("Empty emitter findByName") {
        DocsEmitter emitter;
        auto found = emitter.findByName("nonexistent");
        EXPECT(found == nullptr);
    } END_TEST;

    TEST("Add entry manually") {
        DocsEmitter emitter;
        DocEntry e;
        e.name = "test_func";
        e.kind = DocItemKind::Function;
        e.description = "A test function";
        emitter.addEntry(e);
        auto found = emitter.findByName("test_func");
        EXPECT(found != nullptr);
    } END_TEST;

    TEST("Add from source") {
        DocsConfig cfg;
        DocsEmitter emitter(cfg);
        std::string code =
            "## A hello function\n"
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 hello()\n"
            "    print(\"hello\")\n"
            "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9\n";
        emitter.addFromSource(code, "test.s");
        EXPECT(emitter.entryCount() > 0);
    } END_TEST;

    TEST("Find by kind") {
        DocsConfig cfg;
        DocsEmitter emitter(cfg);
        DocEntry f1; f1.name = "func1"; f1.kind = DocItemKind::Function;
        DocEntry f2; f2.name = "func2"; f2.kind = DocItemKind::Function;
        DocEntry c1; c1.name = "class1"; c1.kind = DocItemKind::Class;
        emitter.addEntry(f1);
        emitter.addEntry(f2);
        emitter.addEntry(c1);
        auto funcs = emitter.findByKind(DocItemKind::Function);
        EXPECT(funcs.size() >= 2);
        auto classes = emitter.findByKind(DocItemKind::Class);
        EXPECT(classes.size() >= 1);
    } END_TEST;

    TEST("Text search") {
        DocsConfig cfg;
        DocsEmitter emitter(cfg);
        DocEntry e;
        e.name = "calc_area";
        e.kind = DocItemKind::Function;
        e.description = "Calculate circle area from radius";
        emitter.addEntry(e);
        auto results = emitter.search("area");
        EXPECT(!results.empty());
    } END_TEST;
}

// ============================================================================
// 4. JSON Generator Tests
// ============================================================================

void testJsonGenerator() {
    std::cout << "\n=== JSON Generator Tests ===\n";

    TEST("Generate JSON for entry") {
        DocsConfig cfg;
        JsonDocGenerator gen(cfg);
        DocEntry e;
        e.name = "test";
        e.kind = DocItemKind::Function;
        e.description = "test description";
        std::string json = gen.entryToJson(e);
        EXPECT(!json.empty());
        EXPECT(json.find("test") != std::string::npos);
    } END_TEST;

    TEST("JSON escaping") {
        std::string escaped = JsonDocGenerator::escapeJson("text \"with\" quotes\nand newline");
        EXPECT(escaped.find("\\\"") != std::string::npos);
        EXPECT(escaped.find("\\n") != std::string::npos);
    } END_TEST;
}

// ============================================================================
// 5. HTML Generator Tests
// ============================================================================

void testHtmlGenerator() {
    std::cout << "\n=== HTML Generator Tests ===\n";

    TEST("Basic HTML generation") {
        DocsConfig cfg;
        cfg.htmlOutput = true;
        cfg.outputDir = "test_docs_output_tmp";
        HtmlDocGenerator gen(cfg);
        std::vector<DocEntry> entries;
        DocEntry e;
        e.name = "test_func";
        e.kind = DocItemKind::Function;
        e.description = "description";
        entries.push_back(e);
        DocsResult result;
        int count = gen.generate(entries, result);
        EXPECT(count >= 0);
        fs::remove_all("test_docs_output_tmp");
    } END_TEST;
}

// ============================================================================
// 6. Markdown Generation via DocsEmitter
// ============================================================================

void testMarkdownGeneration() {
    std::cout << "\n=== Markdown Generation Tests ===\n";

    TEST("Emit markdown via DocsEmitter") {
        DocsConfig cfg;
        cfg.markdownOutput = true;
        DocsEmitter emitter(cfg);
        DocEntry e;
        e.name = "add";
        e.kind = DocItemKind::Function;
        e.description = "add two numbers";
        e.returnType = "number";
        emitter.addEntry(e);
        std::string md = emitter.emitMarkdown();
        EXPECT(!md.empty());
        EXPECT(md.find("add") != std::string::npos);
    } END_TEST;
}

// ============================================================================
// Entry Point
// ============================================================================

int main() {
    std::cout << "========================================\n"
              << "  Documentation Emitter Tests\n"
              << "========================================\n";

    testDocEntry();
    testDocCommentParser();
    testDocsEmitter();
    testJsonGenerator();
    testHtmlGenerator();
    testMarkdownGeneration();

    stats.print();
    return stats.failed > 0 ? 1 : 0;
}
