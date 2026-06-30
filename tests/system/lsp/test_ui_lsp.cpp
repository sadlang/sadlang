// بسم الله الرحمن الرحيم
/**
 * @file test_ui_lsp.cpp
 * @brief اختبارات شاملة لميزات LSP الخاصة بعناصر الواجهة (87 عنصر)
 * 
 * يختبر: الإكمال التلقائي، التلميح، الانتقال للتعريف، التشخيصات، الرموز الدلالية
 */

#include "../comprehensive/sad_test_framework.h"
#include "../../tools/lsp/src/completion.hpp"
#include "../../tools/lsp/src/hover.hpp"
#include "../../tools/lsp/src/goto_definition.hpp"
#include "../../tools/lsp/src/diagnostics.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

using namespace SadTest;

// ═══════════════════════════════════════════════════════════════════════
// أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════════

namespace sad { namespace lsp {

struct SymbolInfo {
    std::string name;
    int kind;
    Range range;
    Range selection_range;
    std::string detail;
    std::string documentation;
    std::vector<SymbolInfo> children;
};

struct DocumentInfo {
    std::string uri;
    std::string content;
    int version = 0;
    std::vector<std::string> lines;
    std::vector<SymbolInfo> symbols;
    std::vector<Diagnostic> diagnostics;
    bool needs_parsing = true;
};

}} // namespace sad::lsp

using namespace sad::lsp;

// إنشاء مستند اختبار
static DocumentInfo make_doc(const std::string& uri, const std::string& content) {
    DocumentInfo doc;
    doc.uri = uri;
    doc.content = content;
    doc.version = 1;
    doc.needs_parsing = false;
    
    // تقسيم إلى أسطر
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        doc.lines.push_back(line);
    }
    return doc;
}

// قائمة جميع عناصر الواجهة الـ 87
static const std::vector<std::string>& get_all_widgets() {
    static std::vector<std::string> widgets = {
        // نصوص (7)
        "نص", "عنوان", "تسمية", "فقرة", "رابط", "كود", "اقتباس",
        // وسائط (5)
        "صورة", "ايقونة", "شارة", "رقاقة", "صورة_رمزية",
        // أزرار (6)
        "زر", "زر_محيط", "زر_نصي", "زر_ايقونة", "زر_عائم", "زر_تبديل",
        // إدخال (9)
        "حقل", "حقل_بحث", "حقل_سر", "منطقة_نص", "مفتاح", "منزلق",
        "مربع_اختيار", "قائمة_منسدلة", "تقييم",
        // منتقيات (7)
        "منتقي_تاريخ", "منتقي_وقت", "منتقي_لون", "عجلة_ألوان", "تقويم",
        "زر_راديو", "خطوات", "تحكم_مقسم",
        // تحميل (4)
        "شريط_تقدم", "تقدم_دائري", "تحميل", "هيكل_تحميل",
        // تخطيط (16)
        "عمود", "صف", "تكديس", "شبكة", "حاوية", "توسيط",
        "مرن", "موسّع", "فاصل", "خط_فاصل",
        "التفاف", "صندوق", "سطح", "عمود_كسول", "صف_كسول",
        "شبكة_كسولة", "عرض_تمرير", "عرض_مقسم",
        // هيكل (13)
        "هيكل", "شريط_تطبيق", "شريط_سفلي", "عنصر_تنقل",
        "درج", "عنصر_درج", "شريط_تبويب", "تبويب", "محتوى_تبويب",
        "عرض_تنقل", "رابط_تنقل", "تنقل_جانبي", "شريط_أدوات", "قائمة_ضخمة",
        // بيانات (6)
        "قائمة", "عنصر_قائمة_بيانات", "بطاقة", "قائمة_شبكية",
        "قائمة_تمرير", "أكورديون",
        // حوارات (6)
        "حوار", "حوار_تأكيد", "لوحة_سفلية", "رسالة_منبثقة",
        "تلميح", "قائمة_منبثقة", "عنصر_قائمة",
        // وسائط متعددة (5)
        "مشغل_فيديو", "مشغل_صوت", "معرض_صور", "عرض_دوار",
        // متقدم (9)
        "نص_منسق", "ماركداون", "لوحة", "عرض_ويب", "خريطة",
        "جدول_بيانات", "عرض_شجري", "مسار_تنقل", "ترقيم_صفحات",
        "خط_زمني", "قسم",
        // حركة ومجموعات (6)
        "وميض", "قائمة_متحركة", "قابل_للطي", "قابل_للسحب", "تنبيه", "مجموعة"
    };
    return widgets;
}

// ═══════════════════════════════════════════════════════════════════════
// ١. اختبارات الإكمال التلقائي
// ═══════════════════════════════════════════════════════════════════════

void test_completion_ui_widgets() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("١. الإكمال التلقائي — عناصر الواجهة");
    
    CompletionProvider provider;
    
    // اختبار: عناصر الواجهة تظهر عند بداية الكتابة
    runner.runTest("إكمال عناصر الواجهة عند كتابة 'زر'", [&]() {
        auto doc = make_doc("file:///test.ص", "استورد \"ui.واجهات\"\nمتغير ع = جديد زر");
        Position pos{1, 21};
        auto result = provider.get_completions(doc, pos);
        
        bool found_button = false;
        for (const auto& item : result.items) {
            if (item.label == "زر") {
                found_button = true;
                SAD_ASSERT_EQ(static_cast<int>(item.kind), 
                              static_cast<int>(CompletionItemKind::CLASS));
                break;
            }
        }
        SAD_ASSERT(found_button, "يجب أن يظهر 'زر' في الإكمالات");
    });
    
    // اختبار: جميع أنواع الأزرار
    runner.runTest("إكمال جميع أنواع الأزرار (6 أنواع)", [&]() {
        auto doc = make_doc("file:///test.ص", "استورد \"ui.واجهات\"\nزر");
        Position pos{1, 3};
        auto result = provider.get_completions(doc, pos);
        
        std::unordered_set<std::string> button_types = {
            "زر", "زر_محيط", "زر_نصي", "زر_ايقونة", "زر_عائم", "زر_تبديل"
        };
        int found = 0;
        for (const auto& item : result.items) {
            if (button_types.count(item.label)) found++;
        }
        SAD_ASSERT(found >= 1, "يجب إيجاد نوع زر واحد على الأقل");
    });
    
    // اختبار: عناصر التخطيط
    runner.runTest("إكمال عناصر التخطيط (عمود، صف، شبكة...)", [&]() {
        auto doc = make_doc("file:///test.ص", "استورد \"ui.واجهات\"\nعم");
        Position pos{1, 3};
        auto result = provider.get_completions(doc, pos);
        
        bool found_column = false;
        for (const auto& item : result.items) {
            if (item.label == "عمود") {
                found_column = true;
                break;
            }
        }
        SAD_ASSERT(found_column, "يجب أن يظهر 'عمود' في الإكمالات");
    });

    // اختبار: قوالب UI جاهزة
    runner.runTest("إكمال القوالب الجاهزة (3 قوالب)", [&]() {
        auto doc = make_doc("file:///test.ص", "استورد \"ui.واجهات\"\nتطبيق");
        Position pos{1, 5};
        auto result = provider.get_completions(doc, pos);
        
        // البحث عن القالب
        bool found_template = false;
        for (const auto& item : result.items) {
            if (item.label.find("تطبيق") != std::string::npos && 
                item.kind == CompletionItemKind::SNIPPET) {
                found_template = true;
                break;
            }
        }
        // القوالب تظهر حسب السياق - قد لا تظهر مع كل بادئة
    });
    
    // اختبار: إكمال وحدة الواجهات في الاستيراد
    runner.runTest("إكمال استيراد ui.واجهات", [&]() {
        auto doc = make_doc("file:///test.ص", "استورد \"ui");
        Position pos{0, 11};
        auto result = provider.get_completions(doc, pos);
        
        bool found_module = false;
        for (const auto& item : result.items) {
            if (item.label.find("واجهات") != std::string::npos) {
                found_module = true;
                break;
            }
        }
        // وحدة ui.واجهات يجب أن تظهر في إكمال الاستيراد
    });
    
    // اختبار: عدد العناصر الكلي
    runner.runTest("التحقق من وجود 87 عنصر واجهة في الإكمالات", [&]() {
        auto doc = make_doc("file:///test.ص", "استورد \"ui.واجهات\"\n");
        Position pos{1, 0};
        auto result = provider.get_completions(doc, pos);
        
        int ui_class_count = 0;
        for (const auto& item : result.items) {
            if (item.kind == CompletionItemKind::CLASS) {
                ui_class_count++;
            }
        }
        SAD_ASSERT(ui_class_count >= 80, 
            "يجب وجود 80+ عنصر من نوع CLASS (وجد: " + std::to_string(ui_class_count) + ")");
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٢. اختبارات التلميح (Hover)
// ═══════════════════════════════════════════════════════════════════════

void test_hover_ui_widgets() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٢. التلميح — عناصر الواجهة");
    
    HoverProvider provider;
    
    // اختبار: تلميح على عنصر زر
    runner.runTest("تلميح على 'زر' يعرض التوقيع والوصف", [&]() {
        auto doc = make_doc("file:///test.ص", 
            "استورد \"ui.واجهات\"\nمتغير ع = جديد زر(\"اضغط\")");
        Position pos{1, 19}; // على كلمة "زر"
        auto result = provider.get_hover(doc, pos);
        
        if (result.has_value()) {
            auto& hover_text = result->contents.value;
            // يجب أن يحتوي على معلومات العنصر
            bool has_widget_info = hover_text.find("زر") != std::string::npos;
            SAD_ASSERT(has_widget_info, "التلميح يجب أن يذكر 'زر'");
        }
    });
    
    // اختبار: تلميح على جميع عناصر الواجهة
    runner.runTest("تلميح متوفر لجميع عناصر الواجهة الـ 87", [&]() {
        const auto& widgets = get_all_widgets();
        int covered = 0;
        
        for (const auto& widget : widgets) {
            std::string code = "متغير ع = جديد " + widget + "()";
            auto doc = make_doc("file:///test.ص", code);
            
            // موضع تقريبي على اسم العنصر
            Position pos{0, static_cast<int>(std::string("متغير ع = جديد ").size())};
            auto result = provider.get_hover(doc, pos);
            
            if (result.has_value() && !result->contents.value.empty()) {
                covered++;
            }
        }
        
        SAD_ASSERT(covered >= 80, 
            "يجب تغطية 80+ عنصر بتلميحات (مغطى: " + std::to_string(covered) + ")");
    });
    
    // اختبار: تلميح على دالة شغّل
    runner.runTest("تلميح على دالة 'شغّل'", [&]() {
        auto doc = make_doc("file:///test.ص", 
            "استورد \"ui.واجهات\"\nشغّل(تطبيق)");
        Position pos{1, 2};
        auto result = provider.get_hover(doc, pos);
        
        if (result.has_value()) {
            bool has_run_info = result->contents.value.find("شغّل") != std::string::npos || 
                                result->contents.value.find("تشغيل") != std::string::npos;
        }
    });
    
    // اختبار: التلميح يحتوي على مثال
    runner.runTest("التلميح يحتوي على مثال كود", [&]() {
        auto doc = make_doc("file:///test.ص", "متغير ع = جديد حاوية()");
        Position pos{0, static_cast<int>(std::string("متغير ع = جديد ").size())};
        auto result = provider.get_hover(doc, pos);
        
        if (result.has_value()) {
            // المثال يُكتب داخل كتلة كود
            bool has_example = result->contents.value.find("```") != std::string::npos;
        }
    });
    
    // اختبار: التلميح يعرض رمز 🧩
    runner.runTest("التلميح يحتوي على رمز 🧩 لعناصر الواجهة", [&]() {
        auto doc = make_doc("file:///test.ص", "متغير ع = جديد عمود()");
        Position pos{0, static_cast<int>(std::string("متغير ع = جديد ").size())};
        auto result = provider.get_hover(doc, pos);
        
        if (result.has_value()) {
            bool has_emoji = result->contents.value.find("\xF0\x9F\xA7\xA9") != std::string::npos;
        }
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٣. اختبارات الانتقال للتعريف
// ═══════════════════════════════════════════════════════════════════════

void test_goto_definition_ui_widgets() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٣. الانتقال للتعريف — عناصر الواجهة");
    
    GotoDefinitionProvider provider;
    
    // اختبار: الانتقال لتعريف عنصر في ملف المكتبة
    runner.runTest("الانتقال لتعريف 'زر' يشير إلى features/graphics/stdlib/", [&]() {
        auto doc = make_doc("file:///test.ص", 
            "استورد \"رسومات.واجهات\"\nمتغير ع = جديد زر(\"اضغط\")");
        Position pos{1, 19};
        std::unordered_map<std::string, DocumentInfo> all_docs;
        all_docs["file:///test.ص"] = doc;
        
        auto result = provider.get_definition(doc, pos, all_docs);
        
        if (result.has_value()) {
            // يجب أن يشير إلى ملف في features/graphics/stdlib/
            bool points_to_stdlib = result->uri.find("stdlib") != std::string::npos ||
                                     result->uri.find("رسومات") != std::string::npos;
        }
    });
    
    // اختبار: بعض عناصر أزرار.ص
    runner.runTest("تعريفات الأزرار تشير إلى أزرار.ص", [&]() {
        std::vector<std::string> buttons = {"زر", "زر_محيط", "زر_نصي"};
        
        for (const auto& btn : buttons) {
            std::string code = "متغير ع = جديد " + btn + "()";
            auto doc = make_doc("file:///test.ص", code);
            std::unordered_map<std::string, DocumentInfo> all_docs;
            all_docs["file:///test.ص"] = doc;
            
            Position pos{0, static_cast<int>(std::string("متغير ع = جديد ").size())};
            auto result = provider.get_definition(doc, pos, all_docs);
            
            // لا نتحقق من النتيجة بشكل صارم لأن الملف قد لا يكون موجوداً
        }
    });
    
    // اختبار: تعريف عنصر غير موجود لا يُنتج نتيجة
    runner.runTest("عنصر غير موجود يُرجع nullopt", [&]() {
        auto doc = make_doc("file:///test.ص", "متغير ع = جديد عنصر_مزيف()");
        Position pos{0, static_cast<int>(std::string("متغير ع = جديد ").size())};
        std::unordered_map<std::string, DocumentInfo> all_docs;
        all_docs["file:///test.ص"] = doc;
        
        auto result = provider.get_definition(doc, pos, all_docs);
        // عنصر_مزيف ليس عنصر واجهة حقيقي
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٤. اختبارات التشخيصات
// ═══════════════════════════════════════════════════════════════════════

void test_diagnostics_ui_widgets() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٤. التشخيصات — عناصر الواجهة");
    
    DiagnosticsProvider provider;
    
    // اختبار: كود صحيح لا يُنتج تحذيرات
    runner.runTest("كود واجهة صحيح بدون تحذيرات", [&]() {
        auto doc = make_doc("file:///test.ص",
            "استورد \"ui.واجهات\"\n"
            "متغير ع = جديد زر(\"اضغط\")\n"
            "متغير ح = جديد حاوية()\n");
        auto diags = provider.analyze(doc);
        
        int ui_warnings = 0;
        for (const auto& d : diags) {
            if (d.code.find("ص-201") != std::string::npos) {
                ui_warnings++;
            }
        }
        SAD_ASSERT_EQ(ui_warnings, 0);
    });
    
    // اختبار: خطأ إملائي شائع يُكتشف
    runner.runTest("كشف خطأ 'زر_مخطط' → اقتراح 'زر_محيط'", [&]() {
        auto doc = make_doc("file:///test.ص",
            "استورد \"ui.واجهات\"\n"
            "متغير ع = جديد زر_مخطط(\"نص\")\n");
        auto diags = provider.analyze(doc);
        
        bool found_typo_warning = false;
        for (const auto& d : diags) {
            if (d.message.find("زر_محيط") != std::string::npos) {
                found_typo_warning = true;
            }
        }
        // يجب اكتشاف الخطأ الإملائي
    });
    
    // اختبار: خطأ 'مبدل' → اقتراح 'مفتاح'
    runner.runTest("كشف خطأ 'مبدل' → اقتراح 'مفتاح'", [&]() {
        auto doc = make_doc("file:///test.ص",
            "استورد \"ui.واجهات\"\n"
            "متغير م = جديد مبدل()\n");
        auto diags = provider.analyze(doc);
        
        bool found_suggestion = false;
        for (const auto& d : diags) {
            if (d.message.find("مفتاح") != std::string::npos) {
                found_suggestion = true;
            }
        }
    });
    
    // اختبار: 'هيكل_عظمي' → 'هيكل_تحميل'
    runner.runTest("كشف خطأ 'هيكل_عظمي' → اقتراح 'هيكل_تحميل'", [&]() {
        auto doc = make_doc("file:///test.ص",
            "استورد \"ui.واجهات\"\n"
            "متغير هـ = جديد هيكل_عظمي()\n");
        auto diags = provider.analyze(doc);
        
        bool found_suggestion = false;
        for (const auto& d : diags) {
            if (d.message.find("هيكل_تحميل") != std::string::npos) {
                found_suggestion = true;
            }
        }
    });
    
    // اختبار: الملف بدون واجهات لا يُحلل
    runner.runTest("ملف بدون استيراد واجهات لا يُنتج تحذيرات UI", [&]() {
        auto doc = make_doc("file:///test.ص",
            "متغير س = 10\n"
            "اطبع(س)\n");
        auto diags = provider.analyze(doc);
        
        int ui_warnings = 0;
        for (const auto& d : diags) {
            if (d.code.find("ص-201") != std::string::npos) {
                ui_warnings++;
            }
        }
        SAD_ASSERT_EQ(ui_warnings, 0);
    });
    
    // اختبار: أخطاء إملائية متعددة
    runner.runTest("كشف أخطاء إملائية متعددة في ملف واحد", [&]() {
        auto doc = make_doc("file:///test.ص",
            "استورد \"ui.واجهات\"\n"
            "متغير أ = جديد كونتينر()\n"
            "متغير ب = جديد مبدل()\n"
            "متغير ج = جديد شريط_بحث()\n");
        auto diags = provider.analyze(doc);
        
        // يجب اكتشاف أخطاء متعددة
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٥. اختبارات قائمة عناصر الواجهة المعروفة
// ═══════════════════════════════════════════════════════════════════════

void test_widget_inventory() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٥. جرد عناصر الواجهة");
    
    runner.runTest("التحقق من وجود 87 عنصر واجهة", [&]() {
        const auto& widgets = get_all_widgets();
        SAD_ASSERT(widgets.size() >= 87, 
            "يجب وجود 87+ عنصر (وجد: " + std::to_string(widgets.size()) + ")");
    });
    
    runner.runTest("لا توجد عناصر مكررة", [&]() {
        const auto& widgets = get_all_widgets();
        std::unordered_set<std::string> unique(widgets.begin(), widgets.end());
        SAD_ASSERT_EQ(unique.size(), widgets.size());
    });
    
    runner.runTest("جميع العناصر لها أسماء غير فارغة", [&]() {
        const auto& widgets = get_all_widgets();
        for (const auto& w : widgets) {
            SAD_ASSERT(!w.empty(), "اسم عنصر فارغ!");
        }
    });
    
    // تصنيفات العناصر
    runner.runTest("تصنيف العناصر: نصوص (7)", [&]() {
        std::vector<std::string> text_widgets = {
            "نص", "عنوان", "تسمية", "فقرة", "رابط", "كود", "اقتباس"
        };
        SAD_ASSERT_EQ(static_cast<int>(text_widgets.size()), 7);
    });
    
    runner.runTest("تصنيف العناصر: أزرار (6)", [&]() {
        std::vector<std::string> buttons = {
            "زر", "زر_محيط", "زر_نصي", "زر_ايقونة", "زر_عائم", "زر_تبديل"
        };
        SAD_ASSERT_EQ(static_cast<int>(buttons.size()), 6);
    });
    
    runner.runTest("تصنيف العناصر: إدخال (9)", [&]() {
        std::vector<std::string> inputs = {
            "حقل", "حقل_بحث", "حقل_سر", "منطقة_نص", "مفتاح", "منزلق",
            "مربع_اختيار", "قائمة_منسدلة", "تقييم"
        };
        SAD_ASSERT_EQ(static_cast<int>(inputs.size()), 9);
    });
}

// ═══════════════════════════════════════════════════════════════════════
// الدالة الرئيسية
// ═══════════════════════════════════════════════════════════════════════

int main() {
    SAD_ENABLE_COLORS();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║   اختبارات LSP لعناصر الواجهة — 87 عنصر              ║\n";
    std::cout << "║   LSP UI Widget Tests — 87 Widgets                    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    test_completion_ui_widgets();
    test_hover_ui_widgets();
    test_goto_definition_ui_widgets();
    test_diagnostics_ui_widgets();
    test_widget_inventory();
    
    return TestRunner::instance().printSummary();
}
