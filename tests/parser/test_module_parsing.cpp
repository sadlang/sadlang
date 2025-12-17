/**
 * @file test_module_parsing.cpp
 * @brief (AR) اختبار تحليل نظام الوحدات
 *        (EN) Test module system parsing
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "../../include/parser/parser_core.h"
#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/ast/module_nodes.h"
#include "../../include/parser/ast/ast_printer.h"

#include <iostream>
#include <cassert>

using namespace Sad;
using namespace Sad::Parser;
using namespace Sad::AST;

// =========================================================================
// (AR) دوال مساعدة / (EN) Helper Functions
// =========================================================================

void testImportSimple() {
    std::cout << "\n=== Test 1: Simple Import (استورد رياضيات) ===" << std::endl;
    
    std::string code = "استورد رياضيات";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    assert(dynamic_cast<ImportStmt*>(program[0].get()) != nullptr);
    
    auto* importStmt = dynamic_cast<ImportStmt*>(program[0].get());
    assert(importStmt->modulePath.size() == 1);
    assert(importStmt->modulePath[0] == "رياضيات");
    assert(!importStmt->alias.has_value());
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testImportWithAlias() {
    std::cout << "\n=== Test 2: Import with Alias (استورد رياضيات كـ م) ===" << std::endl;
    
    std::string code = "استورد رياضيات كـ م";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* importStmt = dynamic_cast<ImportStmt*>(program[0].get());
    assert(importStmt != nullptr);
    assert(importStmt->modulePath[0] == "رياضيات");
    assert(importStmt->alias.has_value());
    assert(importStmt->alias.value() == "م");
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testImportNested() {
    std::cout << "\n=== Test 3: Nested Import (استورد مكتبتي.أدوات.رياضية) ===" << std::endl;
    
    std::string code = "استورد مكتبتي.أدوات.رياضية";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* importStmt = dynamic_cast<ImportStmt*>(program[0].get());
    assert(importStmt != nullptr);
    assert(importStmt->modulePath.size() == 3);
    assert(importStmt->modulePath[0] == "مكتبتي");
    assert(importStmt->modulePath[1] == "أدوات");
    assert(importStmt->modulePath[2] == "رياضية");
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testFromImportSimple() {
    std::cout << "\n=== Test 4: From-Import (من رياضيات استورد جذر) ===" << std::endl;
    
    std::string code = "من رياضيات استورد جذر";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* fromImportStmt = dynamic_cast<FromImportStmt*>(program[0].get());
    assert(fromImportStmt != nullptr);
    assert(fromImportStmt->modulePath[0] == "رياضيات");
    assert(fromImportStmt->items.size() == 1);
    assert(fromImportStmt->items[0].name == "جذر");
    assert(!fromImportStmt->isWildcard);
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testFromImportMultiple() {
    std::cout << "\n=== Test 5: From-Import Multiple (من رياضيات استورد جذر، قوة) ===" << std::endl;
    
    std::string code = "من رياضيات استورد جذر، قوة";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* fromImportStmt = dynamic_cast<FromImportStmt*>(program[0].get());
    assert(fromImportStmt != nullptr);
    assert(fromImportStmt->items.size() == 2);
    assert(fromImportStmt->items[0].name == "جذر");
    assert(fromImportStmt->items[1].name == "قوة");
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testFromImportWithAlias() {
    std::cout << "\n=== Test 6: From-Import with Alias (من رياضيات استورد جذر كـ ج) ===" << std::endl;
    
    std::string code = "من رياضيات استورد جذر كـ ج";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* fromImportStmt = dynamic_cast<FromImportStmt*>(program[0].get());
    assert(fromImportStmt != nullptr);
    assert(fromImportStmt->items.size() == 1);
    assert(fromImportStmt->items[0].name == "جذر");
    assert(fromImportStmt->items[0].alias.has_value());
    assert(fromImportStmt->items[0].alias.value() == "ج");
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testFromImportWildcard() {
    std::cout << "\n=== Test 7: From-Import Wildcard (من رياضيات استورد *) ===" << std::endl;
    
    std::string code = "من رياضيات استورد *";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* fromImportStmt = dynamic_cast<FromImportStmt*>(program[0].get());
    assert(fromImportStmt != nullptr);
    assert(fromImportStmt->isWildcard);
    assert(fromImportStmt->items.empty());
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testExportFunction() {
    std::cout << "\n=== Test 8: Export Function (صدّر دالة...) ===" << std::endl;
    
    std::string code = "صدّر دالة رقم مضاعفة(رقم س)  إرجاع س * 2 نهاية";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* exportDecl = dynamic_cast<ExportDecl*>(program[0].get());
    assert(exportDecl != nullptr);
    assert(exportDecl->declaration != nullptr);
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

void testExportConst() {
    std::cout << "\n=== Test 9: Export Const (صدّر ثابت PI = 3.14) ===" << std::endl;
    
    std::string code = "صدّر ثابت PI = 3.14";
    Lexer::LexerCore lexer(code);
    ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cout << "❌ FAILED: Parser errors detected" << std::endl;
        parser.printErrors();
        return;
    }
    
    assert(program.size() == 1);
    auto* exportDecl = dynamic_cast<ExportDecl*>(program[0].get());
    assert(exportDecl != nullptr);
    
    std::cout << "✅ PASSED" << std::endl;
    
    ASTPrinter printer;
    std::cout << "AST Output: " << printer.print(program[0].get()) << std::endl;
}

// =========================================================================
// (AR) الدالة الرئيسية / (EN) Main Function
// =========================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  (AR) اختبار نظام الوحدات - الاستيراد والتصدير          ║" << std::endl;
    std::cout << "║  (EN) Module System Test - Import and Export             ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    
    try {
        testImportSimple();
        testImportWithAlias();
        testImportNested();
        testFromImportSimple();
        testFromImportMultiple();
        testFromImportWithAlias();
        testFromImportWildcard();
        testExportFunction();
        testExportConst();
        
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║  ✅ جميع الاختبارات نجحت! / All Tests Passed!          ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Exception: " << e.what() << std::endl;
        return 1;
    }
}
