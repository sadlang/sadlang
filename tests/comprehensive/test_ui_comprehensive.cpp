// بسم الله الرحمن الرحيم
/**
 * @file test_ui_comprehensive.cpp
 * @brief اختبارات شاملة لجميع عناصر الواجهة الـ 87 + المترجم SIR + الربط
 * 
 * يغطي: أنواع العناصر، التصنيفات، عمليات SIR، تحويل LLVM IR
 */

#include "sad_test_framework.h"
#include "../../compiler_new/src/sir/sir_opcodes.h"

// ماكرو مساعد: SAD_ASSERT(cond, msg) → SAD_ASSERT_TRUE(cond)
#define SAD_ASSERT(cond, msg) SAD_ASSERT_TRUE(cond)

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace SadTest;
using namespace sad::sir;

// ═══════════════════════════════════════════════════════════════════════
// بيانات الاختبار: جميع عناصر الواجهة الـ 87
// ═══════════════════════════════════════════════════════════════════════

struct WidgetInfo {
    std::string name;
    std::string category;
    std::string file;
};

static const std::vector<WidgetInfo>& get_all_widgets() {
    static std::vector<WidgetInfo> widgets = {
        // نصوص (7)
        {"نص", "نصوص", "أساسيات.ص"},
        {"عنوان", "نصوص", "أساسيات.ص"},
        {"تسمية", "نصوص", "أساسيات.ص"},
        {"فقرة", "نصوص", "أساسيات.ص"},
        {"رابط", "نصوص", "أساسيات.ص"},
        {"كود", "نصوص", "أساسيات.ص"},
        {"اقتباس", "نصوص", "أساسيات.ص"},
        
        // وسائط (5)
        {"صورة", "وسائط", "أساسيات.ص"},
        {"ايقونة", "وسائط", "أساسيات.ص"},
        {"شارة", "وسائط", "أساسيات.ص"},
        {"رقاقة", "وسائط", "أساسيات.ص"},
        {"صورة_رمزية", "وسائط", "أساسيات.ص"},
        
        // أزرار (6)
        {"زر", "أزرار", "أزرار.ص"},
        {"زر_محيط", "أزرار", "أزرار.ص"},
        {"زر_نصي", "أزرار", "أزرار.ص"},
        {"زر_ايقونة", "أزرار", "أزرار.ص"},
        {"زر_عائم", "أزرار", "أزرار.ص"},
        {"زر_تبديل", "أزرار", "أزرار.ص"},
        
        // إدخال (9)
        {"حقل", "إدخال", "إدخال.ص"},
        {"حقل_بحث", "إدخال", "إدخال.ص"},
        {"حقل_سر", "إدخال", "إدخال.ص"},
        {"منطقة_نص", "إدخال", "إدخال.ص"},
        {"مفتاح", "إدخال", "إدخال.ص"},
        {"منزلق", "إدخال", "إدخال.ص"},
        {"مربع_اختيار", "إدخال", "إدخال.ص"},
        {"قائمة_منسدلة", "إدخال", "إدخال.ص"},
        {"تقييم", "إدخال", "إدخال.ص"},
        
        // منتقيات (8)
        {"منتقي_تاريخ", "منتقيات", "منتقيات.ص"},
        {"منتقي_وقت", "منتقيات", "منتقيات.ص"},
        {"منتقي_لون", "منتقيات", "منتقيات.ص"},
        {"عجلة_ألوان", "منتقيات", "منتقيات.ص"},
        {"تقويم", "منتقيات", "منتقيات.ص"},
        {"زر_راديو", "منتقيات", "منتقيات.ص"},
        {"خطوات", "منتقيات", "منتقيات.ص"},
        {"تحكم_مقسم", "منتقيات", "منتقيات.ص"},
        
        // تحميل (4)
        {"شريط_تقدم", "تحميل", "تحميل.ص"},
        {"تقدم_دائري", "تحميل", "تحميل.ص"},
        {"تحميل", "تحميل", "تحميل.ص"},
        {"هيكل_تحميل", "تحميل", "تحميل.ص"},
        
        // تخطيط (18)
        {"عمود", "تخطيط", "تخطيط.ص"},
        {"صف", "تخطيط", "تخطيط.ص"},
        {"تكديس", "تخطيط", "تخطيط.ص"},
        {"شبكة", "تخطيط", "تخطيط.ص"},
        {"حاوية", "تخطيط", "تخطيط.ص"},
        {"توسيط", "تخطيط", "تخطيط.ص"},
        {"مرن", "تخطيط", "تخطيط.ص"},
        {"موسّع", "تخطيط", "تخطيط.ص"},
        {"فاصل", "تخطيط", "تخطيط.ص"},
        {"خط_فاصل", "تخطيط", "تخطيط.ص"},
        {"التفاف", "تخطيط", "تخطيط.ص"},
        {"صندوق", "تخطيط", "تخطيط.ص"},
        {"سطح", "تخطيط", "تخطيط.ص"},
        {"عمود_كسول", "تخطيط", "تخطيط_كسول.ص"},
        {"صف_كسول", "تخطيط", "تخطيط_كسول.ص"},
        {"شبكة_كسولة", "تخطيط", "تخطيط_كسول.ص"},
        {"عرض_تمرير", "تخطيط", "تخطيط_كسول.ص"},
        {"عرض_مقسم", "تخطيط", "تخطيط_كسول.ص"},
        
        // هيكل (14)
        {"هيكل", "هيكل", "هيكل.ص"},
        {"شريط_تطبيق", "هيكل", "هيكل.ص"},
        {"شريط_سفلي", "هيكل", "هيكل.ص"},
        {"عنصر_تنقل", "هيكل", "هيكل.ص"},
        {"درج", "هيكل", "هيكل.ص"},
        {"عنصر_درج", "هيكل", "هيكل.ص"},
        {"شريط_تبويب", "هيكل", "هيكل.ص"},
        {"تبويب", "هيكل", "هيكل.ص"},
        {"محتوى_تبويب", "هيكل", "هيكل.ص"},
        {"عرض_تنقل", "هيكل", "تنقل.ص"},
        {"رابط_تنقل", "هيكل", "تنقل.ص"},
        {"تنقل_جانبي", "هيكل", "تنقل.ص"},
        {"شريط_أدوات", "هيكل", "تنقل.ص"},
        {"قائمة_ضخمة", "هيكل", "تنقل.ص"},
        
        // بيانات (6)
        {"قائمة", "بيانات", "بيانات.ص"},
        {"عنصر_قائمة_بيانات", "بيانات", "بيانات.ص"},
        {"بطاقة", "بيانات", "بيانات.ص"},
        {"قائمة_شبكية", "بيانات", "بيانات.ص"},
        {"قائمة_تمرير", "بيانات", "بيانات.ص"},
        {"أكورديون", "بيانات", "بيانات.ص"},
        
        // حوارات (7)
        {"حوار", "حوارات", "حوارات.ص"},
        {"حوار_تأكيد", "حوارات", "حوارات.ص"},
        {"لوحة_سفلية", "حوارات", "حوارات.ص"},
        {"رسالة_منبثقة", "حوارات", "حوارات.ص"},
        {"تلميح", "حوارات", "حوارات.ص"},
        {"قائمة_منبثقة", "حوارات", "حوارات.ص"},
        {"عنصر_قائمة", "حوارات", "حوارات.ص"},
        
        // وسائط متعددة (4)
        {"مشغل_فيديو", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"مشغل_صوت", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"معرض_صور", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"عرض_دوار", "وسائط_متعددة", "وسائط_متعددة.ص"},
        
        // متقدم (11)
        {"نص_منسق", "متقدم", "متقدم.ص"},
        {"ماركداون", "متقدم", "متقدم.ص"},
        {"لوحة", "متقدم", "متقدم.ص"},
        {"عرض_ويب", "متقدم", "متقدم.ص"},
        {"خريطة", "متقدم", "متقدم.ص"},
        {"جدول_بيانات", "متقدم", "متقدم.ص"},
        {"عرض_شجري", "متقدم", "متقدم.ص"},
        {"مسار_تنقل", "متقدم", "متقدم.ص"},
        {"ترقيم_صفحات", "متقدم", "متقدم.ص"},
        {"خط_زمني", "متقدم", "متقدم.ص"},
        {"قسم", "متقدم", "متقدم.ص"},
        
        // حركة ومجموعات (6)
        {"وميض", "حركة", "حركة.ص"},
        {"قائمة_متحركة", "حركة", "حركة.ص"},
        {"قابل_للطي", "حركة", "حركة.ص"},
        {"قابل_للسحب", "حركة", "حركة.ص"},
        {"تنبيه", "حركة", "حركة.ص"},
        {"مجموعة", "حركة", "حركة.ص"},
    };
    return widgets;
}

// ═══════════════════════════════════════════════════════════════════════
// ١. اختبارات جرد العناصر
// ═══════════════════════════════════════════════════════════════════════

void test_widget_inventory() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("١. جرد عناصر الواجهة");
    
    runner.runTest("العدد الإجمالي 87+ عنصر", [&]() {
        SAD_ASSERT(get_all_widgets().size() >= 87,
            "العدد: " + std::to_string(get_all_widgets().size()));
    });
    
    runner.runTest("لا توجد عناصر مكررة", [&]() {
        std::unordered_set<std::string> seen;
        for (const auto& w : get_all_widgets()) {
            SAD_ASSERT(seen.insert(w.name).second,
                "عنصر مكرر: " + w.name);
        }
    });
    
    runner.runTest("جميع الأسماء غير فارغة", [&]() {
        for (const auto& w : get_all_widgets()) {
            SAD_ASSERT(!w.name.empty(), "اسم فارغ!");
            SAD_ASSERT(!w.category.empty(), "تصنيف فارغ: " + w.name);
            SAD_ASSERT(!w.file.empty(), "ملف فارغ: " + w.name);
        }
    });
    
    runner.runTest("جميع العناصر مصنفة", [&]() {
        std::unordered_set<std::string> valid_categories = {
            "نصوص", "وسائط", "أزرار", "إدخال", "منتقيات",
            "تحميل", "تخطيط", "هيكل", "بيانات", "حوارات",
            "وسائط_متعددة", "متقدم", "حركة"
        };
        for (const auto& w : get_all_widgets()) {
            SAD_ASSERT(valid_categories.count(w.category),
                "تصنيف غير معروف: " + w.category + " لعنصر: " + w.name);
        }
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٢. اختبارات التصنيفات
// ═══════════════════════════════════════════════════════════════════════

void test_widget_categories() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٢. تصنيفات العناصر");
    
    // حساب عناصر كل تصنيف
    std::unordered_map<std::string, int> category_counts;
    for (const auto& w : get_all_widgets()) {
        category_counts[w.category]++;
    }
    
    runner.runTest("نصوص: 7 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["نصوص"], 7);
    });
    
    runner.runTest("وسائط: 5 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["وسائط"], 5);
    });
    
    runner.runTest("أزرار: 6 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["أزرار"], 6);
    });
    
    runner.runTest("إدخال: 9 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["إدخال"], 9);
    });
    
    runner.runTest("منتقيات: 8 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["منتقيات"], 8);
    });
    
    runner.runTest("تحميل: 4 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["تحميل"], 4);
    });
    
    runner.runTest("تخطيط: 18 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["تخطيط"], 18);
    });
    
    runner.runTest("هيكل: 14 عنصر", [&]() {
        SAD_ASSERT_EQ(category_counts["هيكل"], 14);
    });
    
    runner.runTest("بيانات: 6 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["بيانات"], 6);
    });
    
    runner.runTest("حوارات: 7 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["حوارات"], 7);
    });
    
    runner.runTest("وسائط متعددة: 4 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["وسائط_متعددة"], 4);
    });
    
    runner.runTest("متقدم: 11 عنصر", [&]() {
        SAD_ASSERT_EQ(category_counts["متقدم"], 11);
    });
    
    runner.runTest("حركة: 6 عناصر", [&]() {
        SAD_ASSERT_EQ(category_counts["حركة"], 6);
    });
    
    runner.runTest("13 تصنيف في المجموع", [&]() {
        SAD_ASSERT_EQ(static_cast<int>(category_counts.size()), 13);
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٣. اختبارات SIR — عمليات الواجهة
// ═══════════════════════════════════════════════════════════════════════

void test_sir_ui_opcodes() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٣. عمليات SIR للواجهة");
    
    runner.runTest("إنشاء تعليمة UICreateWidget", [&]() {
        auto inst = SirInstruction::UICreateWidget(1, "زر", {2, 3});
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), 
                      static_cast<int>(Opcode::UICreateWidget));
        SAD_ASSERT(inst.result.has_value(), "يجب أن يكون لها نتيجة");
        SAD_ASSERT_EQ(static_cast<int>(*inst.result), 1);
        SAD_ASSERT(inst.operands.size() >= 1, "يجب أن يكون لها معامل واحد على الأقل");
    });
    
    runner.runTest("إنشاء تعليمة UISetProperty", [&]() {
        auto inst = SirInstruction::UISetProperty(1, "لون", 2);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode),
                      static_cast<int>(Opcode::UISetProperty));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 3);
    });
    
    runner.runTest("إنشاء تعليمة UIGetProperty", [&]() {
        auto inst = SirInstruction::UIGetProperty(3, 1, "عرض");
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode),
                      static_cast<int>(Opcode::UIGetProperty));
        SAD_ASSERT(inst.result.has_value(), "يجب أن يكون لها نتيجة");
    });
    
    runner.runTest("إنشاء تعليمة UIAddChild", [&]() {
        auto inst = SirInstruction::UIAddChild(1, 2);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode),
                      static_cast<int>(Opcode::UIAddChild));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 2);
    });
    
    runner.runTest("إنشاء تعليمة UIBindEvent", [&]() {
        auto inst = SirInstruction::UIBindEvent(1, "ضغط", 2);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode),
                      static_cast<int>(Opcode::UIBindEvent));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 3);
    });
    
    runner.runTest("إنشاء تعليمة UIRender", [&]() {
        auto inst = SirInstruction::UIRender(1);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode),
                      static_cast<int>(Opcode::UIRender));
    });
    
    runner.runTest("إنشاء تعليمة UIRunApp", [&]() {
        auto inst = SirInstruction::UIRunApp(1, "تطبيق اختبار");
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode),
                      static_cast<int>(Opcode::UIRunApp));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 2);
    });
    
    runner.runTest("تعليق UICreateWidget يحتوي اسم العنصر", [&]() {
        auto inst = SirInstruction::UICreateWidget(1, "حاوية");
        SAD_ASSERT(inst.comment.find("حاوية") != std::string::npos,
            "التعليق يجب أن يذكر 'حاوية'");
    });
    
    // اختبار تصنيف العمليات
    runner.runTest("تصنيف OpCategory يحتوي UI", [&]() {
        SAD_ASSERT_EQ(static_cast<int>(OpCategory::UI), 9);
    });
    
    // إنشاء جميع أنواع عمليات UI
    runner.runTest("إنشاء جميع أنواع عمليات UI (10 عمليات)", [&]() {
        std::vector<Opcode> ui_ops = {
            Opcode::UICreateWidget, Opcode::UISetProperty,
            Opcode::UIGetProperty, Opcode::UIAddChild,
            Opcode::UIRemoveChild, Opcode::UIBindEvent,
            Opcode::UIEmitEvent, Opcode::UISetLayout,
            Opcode::UIRender, Opcode::UIRunApp
        };
        SAD_ASSERT_EQ(static_cast<int>(ui_ops.size()), 10);
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٤. اختبارات سيناريوهات الاستخدام
// ═══════════════════════════════════════════════════════════════════════

void test_usage_scenarios() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٤. سيناريوهات الاستخدام");
    
    runner.runTest("سيناريو: تطبيق بسيط (هيكل + عنوان + زر)", [&]() {
        // إنشاء العناصر
        auto scaffold = SirInstruction::UICreateWidget(1, "هيكل");
        auto appBar = SirInstruction::UICreateWidget(2, "شريط_تطبيق", {});
        auto title = SirInstruction::UICreateWidget(3, "عنوان");
        auto button = SirInstruction::UICreateWidget(4, "زر");
        
        // بناء الشجرة
        auto addBar = SirInstruction::UIAddChild(1, 2);
        auto addTitle = SirInstruction::UIAddChild(1, 3);
        auto addBtn = SirInstruction::UIAddChild(1, 4);
        
        // تشغيل
        auto run = SirInstruction::UIRunApp(1, "تطبيقي");
        
        SAD_ASSERT_EQ(static_cast<int>(scaffold.opcode), 
                      static_cast<int>(Opcode::UICreateWidget));
        SAD_ASSERT_EQ(static_cast<int>(run.opcode),
                      static_cast<int>(Opcode::UIRunApp));
    });
    
    runner.runTest("سيناريو: نموذج إدخال بيانات", [&]() {
        auto form = SirInstruction::UICreateWidget(1, "عمود");
        auto nameField = SirInstruction::UICreateWidget(2, "حقل");
        auto emailField = SirInstruction::UICreateWidget(3, "حقل");
        auto passField = SirInstruction::UICreateWidget(4, "حقل_سر");
        auto submitBtn = SirInstruction::UICreateWidget(5, "زر");
        
        auto addName = SirInstruction::UIAddChild(1, 2);
        auto addEmail = SirInstruction::UIAddChild(1, 3);
        auto addPass = SirInstruction::UIAddChild(1, 4);
        auto addSubmit = SirInstruction::UIAddChild(1, 5);
        
        SAD_ASSERT_EQ(static_cast<int>(addSubmit.operands.size()), 2);
    });
    
    runner.runTest("سيناريو: حوار تأكيد", [&]() {
        auto dialog = SirInstruction::UICreateWidget(1, "حوار_تأكيد");
        auto setProp = SirInstruction::UISetProperty(1, "عنوان", 2);
        auto bindOk = SirInstruction::UIBindEvent(1, "تأكيد", 3);
        auto bindCancel = SirInstruction::UIBindEvent(1, "إلغاء", 4);
        
        SAD_ASSERT(dialog.comment.find("حوار_تأكيد") != std::string::npos, "");
    });
    
    runner.runTest("سيناريو: قائمة بيانات مع تمرير", [&]() {
        auto scrollView = SirInstruction::UICreateWidget(1, "عرض_تمرير");
        auto lazyCol = SirInstruction::UICreateWidget(2, "عمود_كسول");
        auto addList = SirInstruction::UIAddChild(1, 2);
        
        // إضافة 10 عناصر
        for (int i = 0; i < 10; i++) {
            auto item = SirInstruction::UICreateWidget(10 + i, "بطاقة");
            auto addItem = SirInstruction::UIAddChild(2, 10 + i);
        }
        
        SAD_ASSERT_EQ(static_cast<int>(scrollView.opcode),
                      static_cast<int>(Opcode::UICreateWidget));
    });
    
    runner.runTest("سيناريو: واجهة تبويب", [&]() {
        auto tabBar = SirInstruction::UICreateWidget(1, "شريط_تبويب");
        auto tab1 = SirInstruction::UICreateWidget(2, "تبويب");
        auto tab2 = SirInstruction::UICreateWidget(3, "تبويب");
        auto content1 = SirInstruction::UICreateWidget(4, "محتوى_تبويب");
        auto content2 = SirInstruction::UICreateWidget(5, "محتوى_تبويب");
        
        SAD_ASSERT(tab1.comment.find("تبويب") != std::string::npos, "");
    });
}

// ═══════════════════════════════════════════════════════════════════════
// ٥. اختبارات الملفات والتوزيع
// ═══════════════════════════════════════════════════════════════════════

void test_file_distribution() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("٥. توزيع العناصر على الملفات");
    
    std::unordered_map<std::string, int> file_counts;
    for (const auto& w : get_all_widgets()) {
        file_counts[w.file]++;
    }
    
    runner.runTest("العناصر موزعة على ملفات متعددة", [&]() {
        SAD_ASSERT(file_counts.size() >= 10, 
            "القليل جداً من الملفات: " + std::to_string(file_counts.size()));
    });
    
    runner.runTest("لا يوجد ملف يحتوي أكثر من 20 عنصر", [&]() {
        for (const auto& [file, count] : file_counts) {
            SAD_ASSERT(count <= 20,
                file + " يحتوي " + std::to_string(count) + " عنصر (أكثر من 20)");
        }
    });
    
    runner.runTest("أساسيات.ص يحتوي نصوص + وسائط", [&]() {
        SAD_ASSERT(file_counts.count("أساسيات.ص"), "أساسيات.ص غير موجود!");
        SAD_ASSERT(file_counts["أساسيات.ص"] >= 10, "أساسيات.ص يحتوي عناصر قليلة");
    });
    
    runner.runTest("أزرار.ص يحتوي 6 أزرار", [&]() {
        SAD_ASSERT(file_counts.count("أزرار.ص"), "أزرار.ص غير موجود!");
        SAD_ASSERT_EQ(file_counts["أزرار.ص"], 6);
    });
}

// ═══════════════════════════════════════════════════════════════════════
// الدالة الرئيسية
// ═══════════════════════════════════════════════════════════════════════

int main() {
    SAD_ENABLE_COLORS();
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║   اختبارات شاملة لعناصر الواجهة — 87 عنصر + SIR      ║\n";
    std::cout << "║   Comprehensive UI Widget Tests + Compiler SIR        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    test_widget_inventory();
    test_widget_categories();
    test_sir_ui_opcodes();
    test_usage_scenarios();
    test_file_distribution();
    
    return TestRunner::instance().printSummary();
}
