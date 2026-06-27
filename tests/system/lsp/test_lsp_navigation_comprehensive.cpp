// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: test_lsp_navigation_comprehensive.cpp
// الوصف: اختبارات C++ شاملة لميزات التنقّل والعرض والتنسيق في خادم LSP
// ══════════════════════════════════════════════════════════════════════════════
//
// يكمّل `test_lsp_engine_comprehensive.cpp` بتغطية بقيّة الميزات الـ22 الموصولة:
//   • التنقّل المتقدّم: شجرة الاستدعاءات · شجرة الأنواع · روابط المستند
//   • العرض: عدسات الكود · نطاق التحديد الذكيّ · الرموز الدلالية (نطاق) · الإبراز
//   • المساعدة: مساعدة التوقيع
//   • التنسيق: تنسيق المستند · تنسيق النطاق · التنسيق أثناء الكتابة
//
// المبدأ: التوكيد على ثوابت سليمة (لا انهيار + بنية صحيحة + حتميّة حيث تُمكن).
//
// يربط: sad_lsp_engine + sad_core    يُبنى تحت BUILD_TESTS=ON
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "sad_test.h"
#include "lsp_engine.h"

#include <string>
#include <vector>

using namespace sad::lsp;

namespace {

struct EngineFixture {
    LspEngine engine;
    DocumentUri uri = "file:///تنقّل.ص";

    EngineFixture() { engine.initialize("file:///"); }

    void open(const std::string& text) {
        TextDocumentItem item;
        item.uri = uri;
        item.language_id = "sad";
        item.version = 1;
        item.text = text;
        engine.did_open(item);
    }
};

/// ملفّ يحوي دالتين إحداهما تستدعي الأخرى — مناسب لشجرة الاستدعاءات.
const char* TWO_FUNCS = u8"دالة مساعدة()\n"
                        u8"    أرجع 1\n"
                        u8"نهاية\n"
                        u8"دالة رئيسية()\n"
                        u8"    متغير ق = مساعدة()\n"
                        u8"    أرجع ق\n"
                        u8"نهاية\n";

} // namespace

// ══════════════════════════════════════════════════════════════════════════════
//  التنقّل المتقدّم
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspCallHierarchy, تحضير_ثم_استدعاءات_بلا_انهيار) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    ASSERT_NO_THROW({
        auto items = fx.engine.call_hierarchy_prepare(fx.uri, Position{3, 6});
        for (const auto& it : items) {
            (void)it.name;
            auto incoming = fx.engine.call_hierarchy_incoming(it);
            auto outgoing = fx.engine.call_hierarchy_outgoing(it);
            (void)incoming; (void)outgoing;
        }
    });
}

TEST(LspTypeHierarchy, تحضير_ثم_أنواع_فوقية_وفرعية) {
    EngineFixture fx;
    fx.open(u8"صنف حيوان\nنهاية\n"
            u8"صنف قطة يرث حيوان\nنهاية\n");
    ASSERT_NO_THROW({
        auto items = fx.engine.type_hierarchy_prepare(fx.uri, Position{2, 5});
        for (const auto& it : items) {
            auto supers = fx.engine.type_hierarchy_supertypes(it);
            auto subs = fx.engine.type_hierarchy_subtypes(it);
            (void)supers; (void)subs;
        }
    });
}

TEST(LspDocumentLinks, روابط_الاستيراد_بلا_انهيار) {
    EngineFixture fx;
    fx.open(u8"استورد \"رياضيات\"\n"
            u8"دالة س()\n    أرجع 0\nنهاية\n");
    ASSERT_NO_THROW({
        auto links = fx.engine.document_links(fx.uri);
        for (const auto& l : links) { (void)l.target; (void)l.range; }
    });
}

// ══════════════════════════════════════════════════════════════════════════════
//  العرض
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspCodeLens, عدسات_فوق_الدوال_بلا_انهيار) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    ASSERT_NO_THROW({
        auto lenses = fx.engine.code_lens(fx.uri);
        for (const auto& cl : lenses) { (void)cl.range; }
    });
}

TEST(LspSelectionRange, توسيع_التحديد_يعيد_نطاقًا) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    std::vector<Position> positions = {Position{4, 14}};
    ASSERT_NO_THROW({
        auto ranges = fx.engine.selection_range(fx.uri, positions);
        // عند الإرجاع، كلّ نطاق يجب أن يكون متّسقًا (البداية ≤ النهاية)
        for (const auto& sr : ranges)
            ASSERT_TRUE(sr.range.start <= sr.range.end);
    });
}

TEST(LspSemanticTokensRange, البيانات_من_مضاعفات_الخمسة) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    Range r{Position{0, 0}, Position{2, 0}};
    SemanticTokensData tok = fx.engine.semantic_tokens_range(fx.uri, r);
    ASSERT_EQ(tok.data.size() % 5u, 0u);
}

TEST(LspDocumentHighlights, إبراز_الرمز_بلا_انهيار) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    ASSERT_NO_THROW({
        auto hl = fx.engine.document_highlights(fx.uri, Position{0, 6});
        for (const auto& h : hl) ASSERT_TRUE(h.range.start <= h.range.end);
    });
}

// ══════════════════════════════════════════════════════════════════════════════
//  المساعدة والتنسيق
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspSignatureHelp, توقيع_عند_استدعاء_دالة) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    ASSERT_NO_THROW({
        // داخل قوسي استدعاء "مساعدة()" في السطر الخامس
        auto sh = fx.engine.signature_help(fx.uri, Position{4, 21});
        (void)sh.active_parameter;
        for (const auto& s : sh.signatures) (void)s;
    });
}

// ملاحظة (مُصلَحة): كان format_document/format_range يرميان "No mapping for the
// Unicode character" — خطأ std::filesystem يبني path من مسار عربيّ عبر صفحة ترميز
// ANSI. أُصلح جذريًّا في formatting_provider.cpp (u8path + تأمين المعالِج ضد الرمي).
// هذان الاختباران يحرسان عدم تكرار الانحدار.
TEST(LspFormatting, تنسيق_المستند_لا_يرمي_على_مسار_عربيّ) {
    EngineFixture fx;
    fx.open(u8"دالة   س(  )\n  أرجع    0\nنهاية\n"); // مسافات زائدة
    ASSERT_NO_THROW({
        auto edits = fx.engine.format_document(fx.uri, FormattingOptions{});
        for (const auto& e : edits) ASSERT_TRUE(e.range.start <= e.range.end);
    });
}

TEST(LspRangeFormatting, تنسيق_نطاق_لا_يرمي_على_مسار_عربيّ) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    Range r{Position{0, 0}, Position{2, 0}};
    ASSERT_NO_THROW({
        auto edits = fx.engine.format_range(fx.uri, r, FormattingOptions{});
        for (const auto& e : edits) ASSERT_TRUE(e.range.start <= e.range.end);
    });
}

TEST(LspOnTypeFormatting, تنسيق_عند_كتابة_حرف) {
    EngineFixture fx;
    fx.open(TWO_FUNCS);
    FormattingOptions opts;
    ASSERT_NO_THROW({
        // محاكاة كتابة سطر جديد في نهاية كتلة
        auto edits = fx.engine.on_type_formatting(fx.uri, Position{2, 0}, "\n", opts);
        for (const auto& e : edits) ASSERT_TRUE(e.range.start <= e.range.end);
    });
}

// ══════════════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ══════════════════════════════════════════════════════════════════════════════

int main(int argc, char** argv) {
    return SAD_RUN_TESTS();
}
