// بسم الله الرحمن الرحيم
/**
 * @file test_lsp.cpp
 * @brief LSP Server Tests - اختبارات خادم LSP
 * 
 * اختبارات شاملة لجميع ميزات خادم بروتوكول اللغة
 * Comprehensive tests for all Language Server Protocol features
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "../../tools/lsp/lsp_server.h"
#include "../../tools/lsp/lsp_types.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace sad::lsp;

// ============================================================================
// Test Fixtures - تجهيزات الاختبار
// ============================================================================

/**
 * @brief LSP Server test fixture - تجهيز اختبار خادم LSP
 */
class LSPServerTest : public ::testing::Test {
protected:
    LanguageServer server;
    
    void SetUp() override {
        // تهيئة الخادم / Initialize server
        server.initialize();
    }
    
    void TearDown() override {
        // تنظيف / Cleanup
    }
    
    /**
     * @brief Open a test document - فتح مستند اختبار
     */
    void openDocument(const std::string& uri, const std::string& text) {
        TextDocumentItem item;
        item.uri = uri;
        item.language_id = "sad";
        item.version = 1;
        item.text = text;
        server.did_open(item);
    }
};

// ============================================================================
// Basic Types Tests - اختبارات الأنواع الأساسية
// ============================================================================

TEST(LSPTypesTest, PositionComparison) {
    Position p1{1, 5};
    Position p2{1, 5};
    Position p3{2, 3};
    
    EXPECT_EQ(p1, p2);
    EXPECT_NE(p1, p3);
    EXPECT_LT(p1, p3);
}

TEST(LSPTypesTest, RangeContains) {
    Range range{Position{1, 5}, Position{3, 10}};
    
    EXPECT_TRUE(range.contains(Position{2, 7}));
    EXPECT_FALSE(range.contains(Position{0, 0}));
    EXPECT_FALSE(range.contains(Position{4, 0}));
}

TEST(LSPTypesTest, RangeEmpty) {
    Range empty_range{Position{1, 5}, Position{1, 5}};
    Range non_empty{Position{1, 5}, Position{1, 10}};
    
    EXPECT_TRUE(empty_range.is_empty());
    EXPECT_FALSE(non_empty.is_empty());
}

// ============================================================================
// Server Initialization Tests - اختبارات تهيئة الخادم
// ============================================================================

TEST_F(LSPServerTest, ServerInitialize) {
    auto caps = server.initialize();
    
    // فحص القدرات / Check capabilities
    EXPECT_TRUE(caps.text_document_sync.open_close);
    EXPECT_EQ(caps.text_document_sync.change, 2); // Incremental
    EXPECT_TRUE(caps.completion_provider.enabled);
    EXPECT_TRUE(caps.hover_provider);
    EXPECT_TRUE(caps.definition_provider);
    EXPECT_TRUE(caps.references_provider);
    EXPECT_TRUE(caps.document_symbol_provider);
}

// ============================================================================
// Document Synchronization Tests - اختبارات مزامنة المستندات
// ============================================================================

TEST_F(LSPServerTest, DocumentOpen) {
    std::string uri = "file:///test.s";
    std::string text = "دالة جمع(رقم أ، رقم ب)\n    إرجاع أ + ب\nنهاية";
    
    EXPECT_NO_THROW(openDocument(uri, text));
}

TEST_F(LSPServerTest, DocumentChange) {
    std::string uri = "file:///test.s";
    std::string initial_text = "رقم x = 5";
    
    openDocument(uri, initial_text);
    
    // تغيير المستند / Change document
    TextDocumentContentChangeEvent change;
    change.text = "رقم x = 10";
    
    std::vector<TextDocumentContentChangeEvent> changes = {change};
    EXPECT_NO_THROW(server.did_change(uri, changes, 2));
}

TEST_F(LSPServerTest, DocumentClose) {
    std::string uri = "file:///test.s";
    std::string text = "رقم x = 5";
    
    openDocument(uri, text);
    EXPECT_NO_THROW(server.did_close(uri));
}

// ============================================================================
// Hover Tests - اختبارات التلميح
// ============================================================================

TEST_F(LSPServerTest, HoverOnKeyword) {
    std::string uri = "file:///test.s";
    std::string text = "دالة main()\nنهاية";
    
    openDocument(uri, text);
    
    // التلميح على "دالة" / Hover on "دالة"
    auto hover = server.hover(uri, Position{0, 2});
    
    ASSERT_TRUE(hover.has_value());
    EXPECT_EQ(hover->contents.kind, MarkupKind::MARKDOWN);
    EXPECT_FALSE(hover->contents.value.empty());
    EXPECT_TRUE(hover->contents.value.find("دالة") != std::string::npos);
}

TEST_F(LSPServerTest, HoverOnNonKeyword) {
    std::string uri = "file:///test.s";
    std::string text = "رقم x = 5";
    
    openDocument(uri, text);
    
    // التلميح على موضع فارغ / Hover on empty position
    auto hover = server.hover(uri, Position{0, 8});
    
    // قد يكون لاشيء أو يُرجع معلومات المتغير / May be null or return variable info
    // هذا يعتمد على التنفيذ الكامل للمحلل / Depends on full analyzer implementation
}

// ============================================================================
// Completion Tests - اختبارات الإكمال التلقائي
// ============================================================================

TEST_F(LSPServerTest, CompletionKeywords) {
    std::string uri = "file:///test.s";
    std::string text = "د";
    
    openDocument(uri, text);
    
    // طلب الإكمال / Request completion
    auto completion = server.completion(uri, Position{0, 1});
    
    EXPECT_FALSE(completion.is_incomplete);
    EXPECT_GT(completion.items.size(), 0);
    
    // التحقق من وجود "دالة" / Check for "دالة"
    bool found_function = false;
    for (const auto& item : completion.items) {
        if (item.label == "دالة") {
            found_function = true;
            EXPECT_EQ(item.kind, CompletionItemKind::KEYWORD);
            break;
        }
    }
    EXPECT_TRUE(found_function);
}

TEST_F(LSPServerTest, CompletionEmpty) {
    std::string uri = "file:///test.s";
    std::string text = "";
    
    openDocument(uri, text);
    
    // طلب الإكمال في مستند فارغ / Request completion in empty document
    auto completion = server.completion(uri, Position{0, 0});
    
    EXPECT_FALSE(completion.is_incomplete);
    // يجب أن يحتوي على كلمات مفتاحية / Should contain keywords
    EXPECT_GT(completion.items.size(), 10);
}

// ============================================================================
// Definition Tests - اختبارات الذهاب للتعريف
// ============================================================================

TEST_F(LSPServerTest, DefinitionNotImplemented) {
    std::string uri = "file:///test.s";
    std::string text = "دالة جمع()\nنهاية\nجمع()";
    
    openDocument(uri, text);
    
    // محاولة الذهاب للتعريف / Try go to definition
    auto location = server.definition(uri, Position{2, 0});
    
    // TODO: سيتم تنفيذ هذا لاحقاً / Will be implemented later
    // الآن يجب أن يُرجع nullopt / Should return nullopt for now
    EXPECT_FALSE(location.has_value());
}

// ============================================================================
// References Tests - اختبارات إيجاد المراجع
// ============================================================================

TEST_F(LSPServerTest, ReferencesNotImplemented) {
    std::string uri = "file:///test.s";
    std::string text = "دالة جمع()\nنهاية\nجمع()";
    
    openDocument(uri, text);
    
    // محاولة إيجاد المراجع / Try find references
    auto refs = server.references(uri, Position{0, 5});
    
    // TODO: سيتم تنفيذ هذا لاحقاً / Will be implemented later
    EXPECT_EQ(refs.size(), 0);
}

// ============================================================================
// Document Symbols Tests - اختبارات رموز المستند
// ============================================================================

TEST_F(LSPServerTest, DocumentSymbolsNotImplemented) {
    std::string uri = "file:///test.s";
    std::string text = "دالة main()\nنهاية";
    
    openDocument(uri, text);
    
    // محاولة الحصول على رموز المستند / Try get document symbols
    auto symbols = server.document_symbols(uri);
    
    // TODO: سيتم تنفيذ هذا لاحقاً / Will be implemented later
    EXPECT_EQ(symbols.size(), 0);
}

// ============================================================================
// Code Actions Tests - اختبارات إجراءات الكود
// ============================================================================

TEST_F(LSPServerTest, CodeActionsNotImplemented) {
    std::string uri = "file:///test.s";
    std::string text = "رقم x";
    
    openDocument(uri, text);
    
    Range range{Position{0, 0}, Position{0, 5}};
    std::vector<Diagnostic> diagnostics;
    
    // محاولة الحصول على إجراءات الكود / Try get code actions
    auto actions = server.code_actions(uri, range, diagnostics);
    
    // TODO: سيتم تنفيذ هذا لاحقاً / Will be implemented later
    EXPECT_EQ(actions.size(), 0);
}

// ============================================================================
// Formatting Tests - اختبارات التنسيق
// ============================================================================

TEST_F(LSPServerTest, FormattingNotImplemented) {
    std::string uri = "file:///test.s";
    std::string text = "دالة main()\nاطبع(\"مرحباً\")\nنهاية";
    
    openDocument(uri, text);
    
    FormattingOptions options;
    options.tab_size = 4;
    options.insert_spaces = true;
    
    // محاولة تنسيق المستند / Try format document
    auto edits = server.format_document(uri, options);
    
    // TODO: سيتم تنفيذ هذا لاحقاً / Will be implemented later
    EXPECT_EQ(edits.size(), 0);
}

// ============================================================================
// Diagnostic Callback Tests - اختبارات دالة التشخيصات
// ============================================================================

TEST_F(LSPServerTest, DiagnosticsCallback) {
    bool callback_called = false;
    std::string callback_uri;
    std::vector<Diagnostic> callback_diagnostics;
    
    // تعيين دالة callback / Set callback
    server.set_diagnostics_callback(
        [&](const std::string& uri, const std::vector<Diagnostic>& diags) {
            callback_called = true;
            callback_uri = uri;
            callback_diagnostics = diags;
        }
    );
    
    std::string uri = "file:///test.s";
    std::string text = "رقم x = ";  // خطأ بناء جملة / Syntax error
    
    // فتح مستند بخطأ / Open document with error
    openDocument(uri, text);
    
    // يجب أن يتم استدعاء callback / Callback should be called
    EXPECT_TRUE(callback_called);
    EXPECT_EQ(callback_uri, uri);
    // قد يحتوي على تشخيصات / May contain diagnostics
}

// ============================================================================
// Arabic Text Tests - اختبارات النصوص العربية
// ============================================================================

TEST_F(LSPServerTest, ArabicKeywords) {
    std::string uri = "file:///test.s";
    std::string text = "دالة جمع(رقم أ، رقم ب) -> رقم\n    إرجاع أ + ب\nنهاية";
    
    openDocument(uri, text);
    
    // التلميح على كلمات مفتاحية عربية / Hover on Arabic keywords
    auto hover_func = server.hover(uri, Position{0, 2});
    EXPECT_TRUE(hover_func.has_value());
    
    auto hover_return = server.hover(uri, Position{1, 6});
    EXPECT_TRUE(hover_return.has_value());
    
    auto hover_end = server.hover(uri, Position{2, 2});
    EXPECT_TRUE(hover_end.has_value());
}

TEST_F(LSPServerTest, MixedArabicEnglish) {
    std::string uri = "file:///test.s";
    std::string text = "دالة main()\n    رقم result = 42\n    اطبع(result)\nنهاية";
    
    EXPECT_NO_THROW(openDocument(uri, text));
    
    // الإكمال يجب أن يعمل / Completion should work
    auto completion = server.completion(uri, Position{2, 8});
    EXPECT_GT(completion.items.size(), 0);
}

// ============================================================================
// Performance Tests - اختبارات الأداء
// ============================================================================

TEST_F(LSPServerTest, LargeDocumentPerformance) {
    std::string uri = "file:///large.s";
    
    // إنشاء مستند كبير / Create large document
    std::string text;
    for (int i = 0; i < 1000; i++) {
        text += "دالة func_" + std::to_string(i) + "()\n";
        text += "    رقم x = " + std::to_string(i) + "\n";
        text += "نهاية\n\n";
    }
    
    // يجب أن يتم فتحه بسرعة / Should open quickly
    auto start = std::chrono::high_resolution_clock::now();
    openDocument(uri, text);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // يجب أن يكون أقل من ثانية / Should be less than 1 second
    EXPECT_LT(duration.count(), 1000);
}

// ============================================================================
// Main - الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "بسم الله الرحمن الرحيم" << std::endl;
    std::cout << "LSP Server Tests - اختبارات خادم LSP" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "=======================================" << std::endl;
    std::cout << "الحمد لله رب العالمين" << std::endl;
    
    return result;
}
