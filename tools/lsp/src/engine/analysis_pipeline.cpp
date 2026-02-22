// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════════════════════════
// ملف: analysis_pipeline.cpp
// الوصف: خط أنابيب التحليل الثوري المتقدم - المحرك الأساسي لخادم LSP
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
// ╔═══════════════════════════════════════════════════════════════════════════════════════════════╗
// ║                         خط أنابيب التحليل الثوري - الإصدار ٣.٠                              ║
// ╠═══════════════════════════════════════════════════════════════════════════════════════════════╣
// ║                                                                                             ║
// ║  ┌──────────────────────────────────────────────────────────────────────────────────────┐    ║
// ║  │                              مراحل خط الأنابيب                                     │    ║
// ║  │                                                                                      │    ║
// ║  │  النص ──→ [المرحلة ١: التحليل المعجمي LexerCore]                                     │    ║
// ║  │       ──→ سلسلة الرموز (Tokens)                                                     │    ║
// ║  │       ──→ [المرحلة ٢: التحليل النحوي ParserCore]                                     │    ║
// ║  │       ──→ شجرة AST (البرنامج كشجرة من العقد)                                        │    ║
// ║  │       ──→ [المرحلة ٣: عبور شجرة AST العميق]                                         │    ║
// ║  │       ──→ AnalyzedSymbol[] (دوال، متغيرات، أصناف، بنى، سمات، تعدادات، إلخ)          │    ║
// ║  │       ──→ [المرحلة ٤: تتبع المراجع]                                                 │    ║
// ║  │       ──→ SymbolReference[] (أين يُستخدم كل رمز في الكود)                            │    ║
// ║  │       ──→ [المرحلة ٥: استنتاج الأنواع]                                               │    ║
// ║  │       ──→ TypeInfo (استنتاج أنواع المتغيرات من القيم المسندة)                         │    ║
// ║  │       ──→ [المرحلة ٦: التشخيصات المتقدمة]                                            │    ║
// ║  │       ──→ Diagnostic[] (أخطاء، تحذيرات، اقتراحات، تلميحات)                           │    ║
// ║  │       ──→ [المرحلة ٧: استخراج التوثيق]                                               │    ║
// ║  │       ──→ documentation (تعليقات ## → وصف غني)                                       │    ║
// ║  └──────────────────────────────────────────────────────────────────────────────────────┘    ║
// ║                                                                                             ║
// ║  المميزات الثورية في هذا الإصدار:                                                           ║
// ║    ✦ تحليل حقيقي ١٠٠٪ باستخدام LexerCore + ParserCore (بدون regex للتحليل الأساسي)         ║
// ║    ✦ عبور عميق لشجرة AST يشمل كل أنواع العقد:                                              ║
// ║      • FunctionDecl (دالة) - مع معلماتها ونوع إرجاعها و async/generator                     ║
// ║      • ClassDecl (صنف) - مع أعضائه وأصنافه الأب والوراثة المتعددة                           ║
// ║      • VarDeclStmt (متغير/ثابت) - مع استنتاج النوع من القيمة المسندة                       ║
// ║      • EnumDecl (تعداد) - مع أعضاء التعداد وقيمهم                                          ║
// ║      • StructDecl (بنية) - مع حقولها ودوالها المنفذة                                       ║
// ║      • TraitDecl (سمة) - مع دوالها وسماتها المطلوبة                                        ║
// ║      • ImplDecl (تنفيذ) - تنفيذ سمة لنوع أو إضافة دوال لبنية                              ║
// ║      • NamespaceDecl (فضاء أسماء) - مع أعضائه المتداخلة                                    ║
// ║      • TemplateFunctionDecl (دالة قالب) - مع معاملات الأنواع                                ║
// ║      • TemplateClassDecl (صنف قالب) - مع معاملات الأنواع                                    ║
// ║      • MethodDecl (طريقة) - مع معدّل الوصول والخصائص                                       ║
// ║      • FieldDecl (حقل) - مع معدّل الوصول والقيمة الافتراضية                                ║
// ║      • ConstructorDecl (منشئ) - مع معاملاته                                                 ║
// ║      • DestructorDecl (هادم)                                                                ║
// ║      • OperatorDecl (تحميل عامل) - مع رمز العامل                                           ║
// ║      • TestDecl (اختبار) - مع اسم الاختبار                                                 ║
// ║      • ImportStmt/FromImportStmt (استيراد)                                                   ║
// ║      • ExportStmt (تصدير)                                                                   ║
// ║    ✦ استنتاج أنواع المتغيرات من القيم المسندة:                                              ║
// ║      • "نص" → نص، 42 → رقم، 3.14 → عشري، صحيح/خطأ → منطقي                                ║
// ║      • [...] → مصفوفة، {...} → قاموس                                                       ║
// ║    ✦ تتبع المراجع مع التمييز بين القراءة والكتابة والتعريف                                  ║
// ║    ✦ كشف متقدم: متغيرات غير مستخدمة، تعريفات مكررة، أنماط إنجليزية                         ║
// ║    ✦ كشف أقواس غير متوازنة، مسافات مختلطة، أسطر طويلة                                      ║
// ║    ✦ تحليل احتياطي بـ regex عندما يفشل المحلل النحوي                                        ║
// ║    ✦ استخراج التوثيق من تعليقات ## فوق التعريفات                                            ║
// ║                                                                                             ║
// ╚═══════════════════════════════════════════════════════════════════════════════════════════════╝
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════════════════════════

// ──────────────────────────────────────────────────────────────────────────────
//  المكتبات المطلوبة
// ──────────────────────────────────────────────────────────────────────────────

#include "lsp_engine.h"       // تعريفات المحرك الأساسي وكل الأنواع
#include "arabic_utils.h"     // أدوات النص العربي (تطبيع، بحث ضبابي، تقسيم أسطر)

// ── مكونات المحلل الحقيقية للغة ص ──
#include "lexer_core.h"       // المحلل المعجمي: يحول النص إلى رموز (tokens)
#include "parser_core.h"      // المحلل النحوي: يبني شجرة AST من الرموز
#include "lexer_keywords.h"   // جدول الكلمات المفتاحية العربية
#include "token.h"            // أنواع الرموز (Token) والموقع (Position)
#include "ast_node.h"         // العقدة الأساسية لشجرة AST
#include "declarations.h"     // تصريحات: دالة، صنف، تعداد، بنية، سمة، فضاء أسماء، إلخ
#include "statements.h"       // جمل: إذا، بينما، لكل، إرجاع، متغير، إلخ
#include "expressions.h"      // تعبيرات: حرفي، ثنائي، استدعاء، وصول عضو، إلخ
#include "module_nodes.h"     // عقد نظام الوحدات: استيراد، تصدير

#include <algorithm>          // std::sort, std::reverse
#include <regex>              // regex لاستخراج أرقام الأسطر من رسائل الأخطاء
#include <sstream>            // std::istringstream لتقسيم النص
#include <unordered_set>      // std::unordered_set لتتبع الأسماء الفريدة
#include <unordered_map>      // std::unordered_map لخرائط الرموز والمراجع

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ١: المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  AnalysisPipeline هو كائن خفيف الوزن (stateless تقريباً).
//  كل استدعاء لـ analyze() مستقل تماماً عن السابق.
//  لذلك المنشئ والمدمر بسيطان.
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

AnalysisPipeline::AnalysisPipeline() = default;
AnalysisPipeline::~AnalysisPipeline() = default;

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٢: دوال مساعدة لتحويل أنواع البيانات
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  هذه الدوال تحول بين أنواع البيانات في المحلل (Sad::Data::DataType)
//  وأنواع البيانات في LSP (TypeInfo). التحويل ضروري لأن:
//  - المحلل يستخدم enum DataType (رقم ثابت)
//  - LSP يحتاج TypeInfo (اسم نصي + خصائص إضافية)
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

/// ──────────────────────────────────────────────────────────────────────────────
/// تحويل DataType من المحلل إلى TypeInfo لنظام الفهرسة
///
/// كل نوع بيانات في لغة ص له اسم عربي يُعرض في LSP:
///   INTEGER → "رقم"    | FLOAT → "عشري"   | STRING → "نص"
///   BOOLEAN → "منطقي"  | NONE → "عدم"     | ARRAY → "مصفوفة"
///   MAP → "قاموس"      | TUPLE → "صف"     | FUNCTION → "دالة"
///   OBJECT → "كائن"    | ENUM → "تعداد"   | BYTE → "بايت"
///   ERROR → "خطأ"      | غير معروف → "غير_محدد" (مستنتج)
/// ──────────────────────────────────────────────────────────────────────────────
static TypeInfo data_type_to_type_info(Sad::Data::DataType dt) {
    TypeInfo ti;
    switch (dt) {
        case Sad::Data::DataType::INTEGER:  ti.name = "رقم";      break;
        case Sad::Data::DataType::FLOAT:    ti.name = "عشري";     break;
        case Sad::Data::DataType::STRING:   ti.name = "نص";       break;
        case Sad::Data::DataType::BOOLEAN:  ti.name = "منطقي";    break;
        case Sad::Data::DataType::NONE:     ti.name = "عدم";      break;
        case Sad::Data::DataType::ARRAY:    ti.name = "مصفوفة";   break;
        case Sad::Data::DataType::MAP:      ti.name = "قاموس";    break;
        case Sad::Data::DataType::TUPLE:    ti.name = "صف";       break;
        case Sad::Data::DataType::FUNCTION: ti.name = "دالة";     break;
        case Sad::Data::DataType::OBJECT:   ti.name = "كائن";     break;
        case Sad::Data::DataType::ENUM:     ti.name = "تعداد";    break;
        case Sad::Data::DataType::BYTE:     ti.name = "بايت";     break;
        case Sad::Data::DataType::ERROR:    ti.name = "خطأ";      break;
        default:
            // نوع غير معروف - سيُحاول استنتاجه لاحقاً من السياق
            ti.name = "غير_محدد";
            ti.is_inferred = true;
            break;
    }
    return ti;
}

/// ──────────────────────────────────────────────────────────────────────────────
/// تحويل Position من المحلل المعجمي إلى Position في بروتوكول LSP
///
/// فرق مهم:
///   • المحلل المعجمي (LexerCore): الأسطر والأعمدة تبدأ من ١
///   • بروتوكول LSP: الأسطر والأعمدة تبدأ من ٠
///
/// لذلك نطرح ١ من كل إحداثي عند التحويل.
/// ──────────────────────────────────────────────────────────────────────────────
static Position lexer_pos_to_lsp(const Sad::Lexer::Position& pos) {
    Position p;
    p.line = static_cast<int>(pos.line) - 1;       // LSP: الأسطر من ٠
    p.character = static_cast<int>(pos.column) - 1; // LSP: الأعمدة من ٠
    return p;
}

/// ──────────────────────────────────────────────────────────────────────────────
/// إنشاء نطاق (Range) من Position المحلل المعجمي
///
/// النطاق يبدأ من موقع الرمز وينتهي عند نهايته (موقع البداية + طول الرمز).
/// هذا مفيد لتحديد مكان الرمز بدقة في المحرر (لتلوينه أو إظهار خطأ).
/// ──────────────────────────────────────────────────────────────────────────────
static Range lexer_pos_to_range(const Sad::Lexer::Position& pos) {
    Range r;
    r.start = lexer_pos_to_lsp(pos);
    r.end = r.start;
    r.end.character += static_cast<int>(pos.length);
    return r;
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٣: استخراج التوثيق من تعليقات ## فوق التعريفات
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  في لغة ص، تعليقات التوثيق تبدأ بـ ## (مشابه لـ /// في Rust أو /** */ في Java).
//  هذه الدالة تصعد من السطر قبل التعريف وتجمع كل تعليقات ## المتتالية.
//
//  مثال:
//    ## هذه دالة تحسب المجموع
//    ## تأخذ عددين وترجع مجموعهما
//    دالة مجموع(أ، ب) {
//        إرجاع أ + ب
//    }
//
//  النتيجة: "هذه دالة تحسب المجموع\nتأخذ عددين وترجع مجموعهما"
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

/// استخراج تعليقات التوثيق (## أو #) الموجودة فوق سطر معين
///
/// @param lines  أسطر الملف مقسمة مسبقاً
/// @param target_line  رقم السطر المستهدف (٠-based) الذي يحتوي على التعريف
/// @return النص التوثيقي المُجمّع، أو سلسلة فارغة إذا لم يوجد توثيق
///
/// الخوارزمية:
///   ① نبدأ من السطر قبل التعريف مباشرة (target_line - 1)
///   ② نصعد لأعلى سطراً بسطر
///   ③ إذا وجدنا ## أو # في بداية السطر، نضيفه للقائمة
///   ④ إذا وجدنا سطراً فارغاً بعد أن بدأنا بجمع التعليقات، نتوقف
///   ⑤ إذا وجدنا سطر كود عادي، نتوقف
///   ⑥ نعكس الترتيب (لأننا جمعنا من الأسفل للأعلى)
static std::string extract_documentation(const std::vector<std::string>& lines, int target_line) {
    std::string doc;
    std::vector<std::string> doc_lines;

    // نرجع للأعلى من السطر قبل التعريف
    for (int i = target_line - 1; i >= 0; i--) {
        const auto& line = lines[i];

        // تنظيف المسافات البادئة (المسافات والجداول)
        size_t start = 0;
        while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) start++;
        std::string trimmed = line.substr(start);

        // ──── تعليق توثيق ## ────
        // هذا هو النوع المفضل للتوثيق في لغة ص
        if (trimmed.size() >= 2 && trimmed[0] == '#' && trimmed[1] == '#') {
            std::string comment = trimmed.substr(2);
            // حذف المسافة الاختيارية بعد ##
            if (!comment.empty() && comment[0] == ' ') comment = comment.substr(1);
            doc_lines.push_back(comment);
        }
        // ──── تعليق عادي # ────
        // نضيفه أيضاً لأن بعض المبرمجين يستخدمون # بدل ##
        else if (trimmed.size() >= 1 && trimmed[0] == '#' && (trimmed.size() < 2 || trimmed[1] != '#')) {
            std::string comment = trimmed.substr(1);
            if (!comment.empty() && comment[0] == ' ') comment = comment.substr(1);
            doc_lines.push_back(comment);
        }
        // ──── سطر فارغ ────
        // إذا كنا في منتصف جمع التعليقات، نتوقف
        // إذا لم نبدأ بعد، نتخطى الأسطر الفارغة
        else if (trimmed.empty()) {
            if (!doc_lines.empty()) break;
        }
        // ──── سطر كود عادي ────
        // نتوقف فوراً - انتهت التعليقات
        else {
            break;
        }
    }

    // عكس الترتيب (لأننا جمعنا من الأسفل للأعلى)
    std::reverse(doc_lines.begin(), doc_lines.end());
    for (const auto& dl : doc_lines) {
        if (!doc.empty()) doc += "\n";
        doc += dl;
    }
    return doc;
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٤: استنتاج الأنواع من التعبيرات (Type Inference)
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  عندما يكتب المبرمج: متغير س = 42
//  لا يحتاج لتحديد النوع - نستنتجه تلقائياً من القيمة المسندة.
//
//  هذه الميزة تعمل على مستوى التعبيرات:
//    • LiteralExpr("نص") → TypeInfo{name="نص"}
//    • LiteralExpr(42) → TypeInfo{name="رقم"}
//    • ArrayLiteralExpr → TypeInfo{name="مصفوفة"}
//    • MapLiteralExpr → TypeInfo{name="قاموس"}
//    • FunctionCallExpr(اسم_دالة) → نوع إرجاع الدالة (إذا معروف)
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

/// استنتاج نوع تعبير من شجرة AST
///
/// @param expr  مؤشر لعقدة التعبير في AST
/// @return TypeInfo مع النوع المستنتج، أو نوع "غير_محدد" إذا لم نستطع الاستنتاج
///
/// الأنواع المدعومة للاستنتاج:
///   • القيم الحرفية: "نص"→نص، 42→رقم، 3.14→عشري، صحيح/خطأ→منطقي، عدم→عدم
///   • المصفوفات: [1,2,3] → مصفوفة
///   • القواميس: {"مفتاح": "قيمة"} → قاموس
///   • الدوال المجهولة: دالة(أ) { ... } → دالة
///   • باقي التعبيرات: يُعاد نوع "غير_محدد" مع is_inferred = true
static TypeInfo infer_type_from_expression(const Sad::AST::Expression* expr) {
    TypeInfo ti;
    ti.is_inferred = true;  // نعلّم أن النوع مستنتج وليس مُصرّح

    if (!expr) {
        ti.name = "غير_محدد";
        return ti;
    }

    // ──── القيم الحرفية (Literal) ────
    // هذا هو أبسط وأدق نوع من الاستنتاج
    // ملاحظة: LiteralExpr لديه دالة getType() وليس حقل type
    auto* literal = dynamic_cast<const Sad::AST::LiteralExpr*>(expr);
    if (literal) {
        Sad::Data::DataType literalType = literal->getType();
        switch (literalType) {
            case Sad::Data::DataType::STRING:   ti.name = "نص";    break;
            case Sad::Data::DataType::INTEGER:  ti.name = "رقم";   break;
            case Sad::Data::DataType::FLOAT:    ti.name = "عشري";  break;
            case Sad::Data::DataType::BOOLEAN:  ti.name = "منطقي"; break;
            case Sad::Data::DataType::NONE:     ti.name = "عدم";   break;
            default:                            ti.name = "غير_محدد"; break;
        }
        return ti;
    }

    // ──── مصفوفة حرفية [1, 2, 3] ────
    // ملاحظة: ArrayExpr وليس ArrayLiteralExpr
    auto* arr = dynamic_cast<const Sad::AST::ArrayExpr*>(expr);
    if (arr) {
        ti.name = "مصفوفة";
        // محاولة استنتاج نوع العناصر من العنصر الأول
        if (!arr->elements.empty() && arr->elements[0]) {
            auto elem_type = infer_type_from_expression(
                dynamic_cast<const Sad::AST::Expression*>(arr->elements[0].get()));
            if (!elem_type.name.empty() && elem_type.name != "غير_محدد") {
                ti.type_args.push_back(elem_type);
                ti.name = "مصفوفة[" + elem_type.name + "]";
            }
        }
        return ti;
    }

    // ──── قاموس حرفي {"مفتاح": "قيمة"} ────
    // ملاحظة: MapExpr وليس MapLiteralExpr
    auto* map = dynamic_cast<const Sad::AST::MapExpr*>(expr);
    if (map) {
        ti.name = "قاموس";
        return ti;
    }

    // ──── تعبير لامبدا (دالة مجهولة) ────
    auto* lambda = dynamic_cast<const Sad::AST::LambdaExpr*>(expr);
    if (lambda) {
        ti.name = "دالة";
        return ti;
    }

    // ──── عملية ثنائية (أ + ب، أ * ب، إلخ) ────
    // نحاول استنتاج النوع من النوع الأيسر
    auto* binary = dynamic_cast<const Sad::AST::BinaryExpr*>(expr);
    if (binary && binary->left) {
        auto left_type = infer_type_from_expression(
            dynamic_cast<const Sad::AST::Expression*>(binary->left.get()));
        if (!left_type.name.empty() && left_type.name != "غير_محدد") {
            ti = left_type;
        }
        return ti;
    }

    // ──── عملية أحادية (-أ، !أ) ────
    auto* unary = dynamic_cast<const Sad::AST::UnaryExpr*>(expr);
    if (unary && unary->operand) {
        return infer_type_from_expression(
            dynamic_cast<const Sad::AST::Expression*>(unary->operand.get()));
    }

    // ──── لم نستطع الاستنتاج ────
    ti.name = "غير_محدد";
    return ti;
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٥: تتبع المراجع - البحث عن استخدامات الرموز في الكود
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  بعد جمع كل تعريفات الرموز (دوال، متغيرات، أصناف)، نبحث في الكود
//  عن كل مكان يُذكر فيه اسم كل رمز. هذا يتيح:
//    • "Go to References" (Shift+F12) - إيجاد كل استخدامات رمز
//    • "Rename Symbol" - إعادة تسمية رمز في كل الأماكن
//    • كشف المتغيرات غير المستخدمة (لا مراجع لها بعد التعريف)
//    • Code Lens: عرض عدد المراجع فوق كل دالة/صنف
//
//  نميّز بين ثلاثة أنواع من المراجع:
//    ① تعريف (declaration) - مكان تعريف الرمز الأصلي
//    ② كتابة (write) - مكان تعديل قيمة الرمز (مثل: س = 5)
//    ③ قراءة (read) - مكان استخدام الرمز بدون تعديل (مثل: اطبع(س))
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

/// البحث عن كل استخدامات الرموز المعروفة في الكود
///
/// @param content    محتوى الملف النصي الكامل
/// @param uri        عنوان المستند (URI)
/// @param symbols    الرموز المكتشفة من التحليل
/// @param references [out] قائمة المراجع المكتشفة (تُضاف إليها)
///
/// الخوارزمية:
///   ① نبني خريطة من أسماء الرموز المعروفة
///   ② لكل سطر في الملف:
///     أ. نتخطى التعليقات
///     ب. نبحث عن كل رمز معروف في السطر
///     ج. نتحقق من حدود الكلمة (أنه ليس جزءاً من كلمة أطول)
///     د. نحدد إذا كان تعريفاً أم كتابة أم قراءة
///     هـ. نضيف المرجع للقائمة
static void collect_references(
    const std::string& content,
    const DocumentUri& uri,
    const std::vector<AnalyzedSymbol>& symbols,
    std::vector<SymbolReference>& references)
{
    auto lines = arabic::split_lines(content);

    // ──── بناء خريطة الرموز المعروفة ────
    // نستثني رموز الاستيراد لأنها ليست تعريفات محلية
    std::unordered_map<std::string, const AnalyzedSymbol*> known_symbols;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.kind != AnalyzedSymbolKind::Import) {
            known_symbols[sym.name] = &sym;
        }
    }

    // ──── فحص كل سطر ────
    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];
        if (line.empty()) continue;

        // تخطي أسطر التعليقات (تبدأ بـ # بعد المسافات)
        size_t first_non_space = 0;
        while (first_non_space < line.size() &&
               (line[first_non_space] == ' ' || line[first_non_space] == '\t')) {
            first_non_space++;
        }
        if (first_non_space < line.size() && line[first_non_space] == '#') continue;

        // ──── البحث عن كل رمز معروف في السطر ────
        for (const auto& [name, sym] : known_symbols) {
            size_t search_pos = 0;
            while (search_pos < line.size()) {
                size_t found = line.find(name, search_pos);
                if (found == std::string::npos) break;

                // ──── التحقق من حدود الكلمة ────
                // نتأكد أن الاسم ليس جزءاً من كلمة أطول
                // مثال: اسم "س" يجب ألا يتطابق مع "سلام"
                bool valid_start = (found == 0) ||
                    (!arabic::is_identifier_char_byte(line[found - 1]) &&
                     line[found - 1] != '_');
                size_t end_pos = found + name.size();
                bool valid_end = (end_pos >= line.size()) ||
                    (!arabic::is_identifier_char_byte(line[end_pos]) &&
                     line[end_pos] != '_');

                if (valid_start && valid_end) {
                    // ──── هل هو تعريف الرمز نفسه؟ ────
                    bool is_definition = (i == sym->name_range.start.line &&
                                          static_cast<int>(found) == sym->name_range.start.character);

                    // ──── هل هو كتابة (تعديل)؟ ────
                    // نبحث عن = بعد الاسم (مع تخطي المسافات)
                    // لكن نتجنب == (مقارنة) و != و <= و >= إلخ
                    bool is_write = false;
                    size_t after = end_pos;
                    while (after < line.size() && (line[after] == ' ' || line[after] == '\t')) after++;
                    if (after < line.size() && line[after] == '=' &&
                        (after + 1 >= line.size() || line[after + 1] != '=')) {
                        is_write = true;
                    }

                    // ──── إنشاء المرجع وإضافته ────
                    SymbolReference ref;
                    ref.uri = uri;
                    ref.range.start = {i, static_cast<int>(found)};
                    ref.range.end = {i, static_cast<int>(end_pos)};
                    ref.is_declaration = is_definition;
                    ref.is_write = is_write || is_definition;
                    references.push_back(ref);
                }

                // الانتقال لما بعد هذا التطابق للبحث عن المزيد
                search_pos = found + name.size();
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٦: الدالة الرئيسية للتحليل - analyze()
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  هذه هي نقطة الدخول الرئيسية. تُستدعى عند كل تغيير في المستند.
//  تمر بكل مراحل خط الأنابيب وترجع النتيجة الكاملة.
//
//  مراحل التحليل:
//    المرحلة ١: التحليل المعجمي (Lexer) → رموز (Tokens)
//    المرحلة ٢: التحليل النحوي (Parser) → شجرة AST
//    المرحلة ٣: جمع الرموز من AST (عبور عميق لكل أنواع العقد)
//    المرحلة ٤: تتبع المراجع (أين يُستخدم كل رمز)
//    المرحلة ٥: التشخيصات المتقدمة (تحذيرات واقتراحات)
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

AnalysisPipeline::AnalysisResult AnalysisPipeline::analyze(
    const std::string& content, const DocumentUri& uri)
{
    AnalysisResult result;

    // تحضير الأسطر لاستخراج التوثيق والمراجع
    auto lines = arabic::split_lines(content);

    // ════════════════════════════════════════════════════════════════════════
    //  المرحلة ١: التحليل المعجمي (Lexer)
    //  ════════════════════════════════════════════════════════════════════════
    //  نحول النص الخام إلى سلسلة من الرموز (tokens).
    //  كل رمز يحمل: النوع، القيمة، الموقع (سطر + عمود + طول).
    //
    //  مثال:
    //    "دالة مجموع(أ، ب) { إرجاع أ + ب }"
    //    →  [KEYWORD:دالة] [IDENT:مجموع] [LPAREN] [IDENT:أ] [COMMA] [IDENT:ب]
    //       [RPAREN] [LBRACE] [KEYWORD:إرجاع] [IDENT:أ] [PLUS] [IDENT:ب] [RBRACE]
    // ════════════════════════════════════════════════════════════════════════
    try {
        // تهيئة جدول الكلمات المفتاحية (يتم مرة واحدة فقط عبر static)
        Sad::Lexer::KeywordTable::initialize();

        // إنشاء المحلل المعجمي من النص
        Sad::Lexer::LexerCore lexer(content);

        // ════════════════════════════════════════════════════════════════════
        //  المرحلة ٢: التحليل النحوي (Parser)
        //  ════════════════════════════════════════════════════════════════════
        //  نبني شجرة AST (Abstract Syntax Tree) من الرموز.
        //  الشجرة تمثل البنية الهرمية للبرنامج:
        //
        //  Program
        //  ├── FunctionDecl "مجموع"
        //  │   ├── Parameter "أ"
        //  │   ├── Parameter "ب"
        //  │   └── Body
        //  │       └── ReturnStmt
        //  │           └── BinaryExpr (+)
        //  │               ├── IdentExpr "أ"
        //  │               └── IdentExpr "ب"
        //  └── VarDeclStmt "نتيجة"
        //      └── FunctionCallExpr "مجموع"
        //          ├── LiteralExpr 3
        //          └── LiteralExpr 5
        // ════════════════════════════════════════════════════════════════════
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        // ──── جمع أخطاء التحليل النحوي ────
        // إذا كان هناك أخطاء في الكود (مثل: قوس مفقود)
        // المحلل النحوي يجمعها ونحولها لتشخيصات LSP
        if (parser.hasErrors()) {
            auto errors = parser.getErrors();
            for (const auto& err : errors) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Error;
                diag.message = err;
                diag.source = "ص-محلل";

                // محاولة استخراج رقم السطر من رسالة الخطأ
                // رسائل المحلل عادة بصيغة: "[سطر X:عمود Y] رسالة الخطأ"
                std::regex line_pattern(R"(\[.*?(\d+):(\d+)\])");
                std::smatch match;
                if (std::regex_search(err, match, line_pattern)) {
                    int line = std::stoi(match[1].str()) - 1;  // تحويل لـ 0-based
                    int col = std::stoi(match[2].str()) - 1;
                    diag.range.start = {std::max(0, line), std::max(0, col)};
                    diag.range.end = {std::max(0, line), std::max(0, col) + 1};
                }

                diag.code = "ص-٠٠١";  // كود خطأ نحوي
                result.diagnostics.push_back(diag);
            }
        }

        // ════════════════════════════════════════════════════════════════════
        //  المرحلة ٣: عبور شجرة AST العميق - جمع كل الرموز
        //  ════════════════════════════════════════════════════════════════════
        //  نمر على كل عقدة في الشجرة ونستخرج الرموز.
        //  هذه المرحلة تدعم كل أنواع العقد في لغة ص:
        //
        //  العقد المدعومة:
        //    • FunctionDecl - تصريح دالة عادية
        //    • ClassDecl - تصريح صنف مع وراثة متعددة
        //    • VarDeclStmt - تصريح متغير أو ثابت
        //    • EnumDecl - تصريح تعداد
        //    • StructDecl - تصريح بنية (Phase 9)
        //    • TraitDecl - تصريح سمة (Phase 9)
        //    • ImplDecl - كتلة تنفيذ (Phase 9)
        //    • NamespaceDecl - فضاء أسماء
        //    • TemplateFunctionDecl - دالة قالب
        //    • TemplateClassDecl - صنف قالب
        //    • ImportStmt/FromImportStmt - استيراد
        //    • ExportStmt - تصدير
        //    • MethodDecl - طريقة في صنف
        //    • FieldDecl - حقل في صنف
        //    • ConstructorDecl - منشئ
        //    • DestructorDecl - هادم
        //    • OperatorDecl - تحميل عامل
        //    • TestDecl - اختبار
        // ════════════════════════════════════════════════════════════════════

        // نبدأ أيضاً بتحليل regex كخطة احتياطية لالتقاط ما قد يفوت المحلل
        collect_symbols_from_ast(content, uri, result);

        // ──── عبور العقد على مستوى البرنامج ────
        if (!program.empty()) {
            for (const auto& stmt : program) {
                if (!stmt) continue;

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ① تصريح دالة (FunctionDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  يمثل تعريف دالة عادية في المستوى العلوي:
                //    دالة مجموع(أ: رقم، ب: رقم) رقم { إرجاع أ + ب }
                //
                //  نجمع: اسم الدالة، المعلمات، نوع الإرجاع، الخصائص
                //  (async, generator, exported)، والتوثيق من ## فوقها.
                //  كل معلمة تُضاف كرمز Parameter مستقل.
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* func = dynamic_cast<Sad::AST::FunctionDecl*>(stmt.get());
                if (func) {
                    AnalyzedSymbol sym;
                    sym.name = func->name;
                    sym.normalized_name = arabic::normalize_arabic(func->name);
                    sym.kind = AnalyzedSymbolKind::Function;
                    sym.type = data_type_to_type_info(func->returnType);
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(func->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = func->isExported;
                    sym.scope_depth = 0;

                    // استخراج التوثيق من ## فوق الدالة
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    // بناء معلومات الدالة (التوقيع الكامل)
                    AnalyzedSymbol::FunctionInfo fi;
                    fi.return_type = data_type_to_type_info(func->returnType);
                    fi.is_async = func->is_async;
                    fi.is_generator = func->isGenerator;

                    // ──── معلمات الدالة ────
                    for (const auto& param : func->parameters) {
                        fi.parameters.push_back({
                            param.name,
                            data_type_to_type_info(param.type)
                        });

                        // إضافة المعلمة كرمز مستقل (مرئي داخل جسم الدالة)
                        AnalyzedSymbol paramSym;
                        paramSym.name = param.name;
                        paramSym.normalized_name = arabic::normalize_arabic(param.name);
                        paramSym.kind = AnalyzedSymbolKind::Parameter;
                        paramSym.type = data_type_to_type_info(param.type);
                        paramSym.uri = uri;
                        paramSym.scope_owner = func->name;  // الدالة الحاوية
                        paramSym.scope_depth = 1;            // مستوى أعمق من العلوي
                        result.symbols.push_back(paramSym);
                    }

                    sym.func_info = fi;
                    result.symbols.push_back(sym);
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ② تصريح صنف (ClassDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  يمثل تعريف صنف مع دعم الوراثة المتعددة:
                //    صنف طالب يرث شخص، مواطن { ... }
                //
                //  نجمع: اسم الصنف، الأصناف الأب، التوثيق.
                //  ثم نعبر أعضاء الصنف (methods, fields, constructor, etc.)
                //  ونضيف كل عضو كرمز مستقل مع scope_owner = اسم الصنف.
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* cls = dynamic_cast<Sad::AST::ClassDecl*>(stmt.get());
                if (cls) {
                    AnalyzedSymbol sym;
                    sym.name = cls->name;
                    sym.normalized_name = arabic::normalize_arabic(cls->name);
                    sym.kind = AnalyzedSymbolKind::Class;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(cls->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = cls->isExported;
                    sym.scope_depth = 0;

                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    // معلومات الصنف (الأصناف الأب والأعضاء)
                    AnalyzedSymbol::ClassInfo ci;
                    for (const auto& base : cls->superclasses) {
                        ci.parent_class = base;  // آخر واحد يُعتبر الأساسي
                    }
                    sym.class_info = ci;
                    result.symbols.push_back(sym);

                    // ──── عبور أعضاء الصنف ────
                    for (const auto& member : cls->members) {
                        if (!member) continue;

                        // ── طريقة (MethodDecl) ──
                        // طريقة صريحة مع معدّل وصول (عام/خاص/محمي)
                        auto* method = dynamic_cast<Sad::AST::MethodDecl*>(member.get());
                        if (method) {
                            AnalyzedSymbol msym;
                            msym.name = method->name;
                            msym.normalized_name = arabic::normalize_arabic(method->name);
                            msym.kind = AnalyzedSymbolKind::Method;
                            msym.type = data_type_to_type_info(method->returnType);
                            msym.uri = uri;
                            msym.definition_range = lexer_pos_to_range(method->position);
                            msym.name_range = msym.definition_range;
                            msym.scope_owner = cls->name;
                            msym.scope_depth = 1;
                            msym.documentation = extract_documentation(lines, msym.definition_range.start.line);

                            AnalyzedSymbol::FunctionInfo mfi;
                            mfi.return_type = data_type_to_type_info(method->returnType);
                            for (const auto& p : method->parameters) {
                                mfi.parameters.push_back({
                                    p.name,
                                    data_type_to_type_info(p.type)
                                });
                            }
                            msym.func_info = mfi;
                            result.symbols.push_back(msym);
                            continue;
                        }

                        // ── دالة عادية داخل صنف (FunctionDecl كطريقة) ──
                        // بعض الأصناف تستخدم FunctionDecl بدل MethodDecl
                        auto* funcMember = dynamic_cast<Sad::AST::FunctionDecl*>(member.get());
                        if (funcMember) {
                            AnalyzedSymbol msym;
                            msym.name = funcMember->name;
                            msym.normalized_name = arabic::normalize_arabic(funcMember->name);
                            msym.kind = AnalyzedSymbolKind::Method;
                            msym.type = data_type_to_type_info(funcMember->returnType);
                            msym.uri = uri;
                            msym.definition_range = lexer_pos_to_range(funcMember->position);
                            msym.name_range = msym.definition_range;
                            msym.scope_owner = cls->name;
                            msym.scope_depth = 1;
                            msym.documentation = extract_documentation(lines, msym.definition_range.start.line);

                            AnalyzedSymbol::FunctionInfo mfi;
                            mfi.return_type = data_type_to_type_info(funcMember->returnType);
                            mfi.is_async = funcMember->is_async;
                            mfi.is_generator = funcMember->isGenerator;
                            for (const auto& p : funcMember->parameters) {
                                mfi.parameters.push_back({
                                    p.name,
                                    data_type_to_type_info(p.type)
                                });
                            }
                            msym.func_info = mfi;
                            result.symbols.push_back(msym);
                            continue;
                        }

                        // ── حقل (FieldDecl) ──
                        // حقل صريح مع نوع ومعدّل وصول
                        auto* field = dynamic_cast<Sad::AST::FieldDecl*>(member.get());
                        if (field) {
                            AnalyzedSymbol fsym;
                            fsym.name = field->name;
                            fsym.normalized_name = arabic::normalize_arabic(field->name);
                            fsym.kind = AnalyzedSymbolKind::Property;
                            fsym.type = data_type_to_type_info(field->type);
                            fsym.uri = uri;
                            fsym.definition_range = lexer_pos_to_range(field->position);
                            fsym.name_range = fsym.definition_range;
                            fsym.scope_owner = cls->name;
                            fsym.scope_depth = 1;
                            fsym.documentation = extract_documentation(lines, fsym.definition_range.start.line);
                            result.symbols.push_back(fsym);
                            continue;
                        }

                        // ── تصريح متغير كعضو (VarDeclStmt) ──
                        // بعض الأصناف تستخدم VarDeclStmt لتعريف الحقول
                        auto* varMember = dynamic_cast<Sad::AST::VarDeclStmt*>(member.get());
                        if (varMember) {
                            AnalyzedSymbol fsym;
                            fsym.name = varMember->name;
                            fsym.normalized_name = arabic::normalize_arabic(varMember->name);
                            fsym.kind = AnalyzedSymbolKind::Property;
                            fsym.type = data_type_to_type_info(varMember->type);
                            fsym.uri = uri;
                            fsym.definition_range = lexer_pos_to_range(varMember->position);
                            fsym.name_range = fsym.definition_range;
                            fsym.scope_owner = cls->name;
                            fsym.scope_depth = 1;
                            fsym.documentation = extract_documentation(lines, fsym.definition_range.start.line);
                            // محاولة استنتاج النوع من القيمة المسندة
                            // ملاحظة: VarDeclStmt لديه initializer وليس value
                            if (fsym.type.name == "غير_محدد" && varMember->initializer) {
                                auto* expr = dynamic_cast<Sad::AST::Expression*>(varMember->initializer.get());
                                if (expr) {
                                    fsym.type = infer_type_from_expression(expr);
                                }
                            }
                            result.symbols.push_back(fsym);
                            continue;
                        }

                        // ── منشئ (ConstructorDecl) ──
                        auto* ctor = dynamic_cast<Sad::AST::ConstructorDecl*>(member.get());
                        if (ctor) {
                            AnalyzedSymbol csym;
                            csym.name = "منشئ";
                            csym.normalized_name = arabic::normalize_arabic("منشئ");
                            csym.kind = AnalyzedSymbolKind::Constructor;
                            csym.uri = uri;
                            csym.definition_range = lexer_pos_to_range(ctor->position);
                            csym.name_range = csym.definition_range;
                            csym.scope_owner = cls->name;
                            csym.scope_depth = 1;
                            csym.documentation = extract_documentation(lines, csym.definition_range.start.line);

                            AnalyzedSymbol::FunctionInfo cfi;
                            for (const auto& p : ctor->parameters) {
                                cfi.parameters.push_back({
                                    p.name,
                                    data_type_to_type_info(p.type)
                                });
                            }
                            csym.func_info = cfi;
                            result.symbols.push_back(csym);
                            continue;
                        }

                        // ── هادم (DestructorDecl) ──
                        auto* dtor = dynamic_cast<Sad::AST::DestructorDecl*>(member.get());
                        if (dtor) {
                            AnalyzedSymbol dsym;
                            dsym.name = "هادم";
                            dsym.normalized_name = arabic::normalize_arabic("هادم");
                            dsym.kind = AnalyzedSymbolKind::Method;
                            dsym.uri = uri;
                            dsym.definition_range = lexer_pos_to_range(dtor->position);
                            dsym.name_range = dsym.definition_range;
                            dsym.scope_owner = cls->name;
                            dsym.scope_depth = 1;
                            result.symbols.push_back(dsym);
                            continue;
                        }

                        // ── تحميل عامل (OperatorDecl) ──
                        // مثل: عامل +(كسر آخر) كسر { ... }
                        auto* opDecl = dynamic_cast<Sad::AST::OperatorDecl*>(member.get());
                        if (opDecl) {
                            AnalyzedSymbol osym;
                            osym.name = "عامل" + opDecl->operatorSymbol;
                            osym.normalized_name = arabic::normalize_arabic(osym.name);
                            osym.kind = AnalyzedSymbolKind::Method;
                            osym.type = data_type_to_type_info(opDecl->returnType);
                            osym.uri = uri;
                            osym.definition_range = lexer_pos_to_range(opDecl->position);
                            osym.name_range = osym.definition_range;
                            osym.scope_owner = cls->name;
                            osym.scope_depth = 1;

                            AnalyzedSymbol::FunctionInfo ofi;
                            ofi.return_type = data_type_to_type_info(opDecl->returnType);
                            for (const auto& p : opDecl->parameters) {
                                ofi.parameters.push_back({
                                    p.name,
                                    data_type_to_type_info(p.type)
                                });
                            }
                            osym.func_info = ofi;
                            result.symbols.push_back(osym);
                            continue;
                        }
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ③ تصريح متغير/ثابت (VarDeclStmt)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  يمثل تعريف متغير أو ثابت:
                //    متغير العداد = 0
                //    ثابت الحد_الأقصى = 100
                //    متغير الاسم: نص = "أحمد"
                //
                //  إذا لم يُحدد النوع صراحة، نستنتجه من القيمة المسندة:
                //    متغير س = 42        → نوع "رقم" (من LiteralExpr)
                //    متغير ن = "مرحباً"   → نوع "نص" (من LiteralExpr)
                //    متغير ق = [1,2,3]   → نوع "مصفوفة" (من ArrayLiteralExpr)
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* var = dynamic_cast<Sad::AST::VarDeclStmt*>(stmt.get());
                if (var) {
                    AnalyzedSymbol sym;
                    sym.name = var->name;
                    sym.normalized_name = arabic::normalize_arabic(var->name);
                    sym.kind = var->isConst ? AnalyzedSymbolKind::Constant
                                            : AnalyzedSymbolKind::Variable;
                    sym.type = data_type_to_type_info(var->type);
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(var->position);
                    sym.name_range = sym.definition_range;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    // ──── استنتاج النوع إذا لم يُحدد ────
                    // ملاحظة: VarDeclStmt لديه initializer وليس value
                    if ((sym.type.name == "غير_محدد" || sym.type.name.empty()) && var->initializer) {
                        auto* expr = dynamic_cast<Sad::AST::Expression*>(var->initializer.get());
                        if (expr) {
                            sym.type = infer_type_from_expression(expr);
                        }
                    }

                    result.symbols.push_back(sym);
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ④ تصريح تعداد (EnumDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  يمثل تعريف تعداد مع أعضائه:
                //    تعداد اللون { أحمر، أخضر، أزرق }
                //    تعداد الحالة { نجاح = 0، خطأ = 1 }
                //
                //  نجمع: اسم التعداد + كل عضو فيه كرمز EnumMember.
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* enumDecl = dynamic_cast<Sad::AST::EnumDecl*>(stmt.get());
                if (enumDecl) {
                    AnalyzedSymbol sym;
                    sym.name = enumDecl->name;
                    sym.normalized_name = arabic::normalize_arabic(enumDecl->name);
                    sym.kind = AnalyzedSymbolKind::Enum;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(enumDecl->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = enumDecl->isExported;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);
                    result.symbols.push_back(sym);

                    // ──── أعضاء التعداد ────
                    for (const auto& member : enumDecl->members) {
                        AnalyzedSymbol msym;
                        msym.name = member.name;
                        msym.normalized_name = arabic::normalize_arabic(member.name);
                        msym.kind = AnalyzedSymbolKind::Constant;  // أعضاء التعداد ثوابت
                        msym.type.name = enumDecl->name;           // نوعها هو اسم التعداد
                        msym.uri = uri;
                        msym.scope_owner = enumDecl->name;
                        msym.scope_depth = 1;
                        result.symbols.push_back(msym);
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑤ تصريح بنية (StructDecl) - Phase 9 BeeOS
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  يمثل بنية بيانات خفيفة (بدون وراثة، بدون vtable):
                //    بنية نقطة { عشري س، عشري ي }
                //    بنية<ن> صندوق { ن قيمة }
                //
                //  نجمع: اسم البنية + حقولها + الدوال المنفذة.
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* structDecl = dynamic_cast<Sad::AST::StructDecl*>(stmt.get());
                if (structDecl) {
                    AnalyzedSymbol sym;
                    sym.name = structDecl->name;
                    sym.normalized_name = arabic::normalize_arabic(structDecl->name);
                    sym.kind = AnalyzedSymbolKind::Struct;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(structDecl->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = structDecl->isExported;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);
                    result.symbols.push_back(sym);

                    // ──── حقول البنية ────
                    for (const auto& field : structDecl->fields) {
                        AnalyzedSymbol fsym;
                        fsym.name = field.name;
                        fsym.normalized_name = arabic::normalize_arabic(field.name);
                        fsym.kind = AnalyzedSymbolKind::Property;
                        fsym.type = data_type_to_type_info(field.type);
                        fsym.uri = uri;
                        fsym.scope_owner = structDecl->name;
                        fsym.scope_depth = 1;
                        result.symbols.push_back(fsym);
                    }

                    // ──── دوال البنية المنفذة ────
                    for (const auto& m : structDecl->methods) {
                        if (!m) continue;
                        auto* mf = dynamic_cast<Sad::AST::FunctionDecl*>(m.get());
                        if (mf) {
                            AnalyzedSymbol msym;
                            msym.name = mf->name;
                            msym.normalized_name = arabic::normalize_arabic(mf->name);
                            msym.kind = AnalyzedSymbolKind::Method;
                            msym.type = data_type_to_type_info(mf->returnType);
                            msym.uri = uri;
                            msym.definition_range = lexer_pos_to_range(mf->position);
                            msym.name_range = msym.definition_range;
                            msym.scope_owner = structDecl->name;
                            msym.scope_depth = 1;

                            AnalyzedSymbol::FunctionInfo fi;
                            fi.return_type = data_type_to_type_info(mf->returnType);
                            for (const auto& p : mf->parameters) {
                                fi.parameters.push_back({p.name, data_type_to_type_info(p.type)});
                            }
                            msym.func_info = fi;
                            result.symbols.push_back(msym);
                        }
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑥ تصريح سمة (TraitDecl) - Phase 9 BeeOS
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  سمة = واجهة مع تنفيذات افتراضية اختيارية:
                //    سمة قابل_للعرض { دالة اعرض(هذا) نص }
                //    سمة<ن> مقارن { دالة قارن(هذا، آخر: ن) رقم }
                //
                //  نجمع: اسم السمة + دوالها.
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* traitDecl = dynamic_cast<Sad::AST::TraitDecl*>(stmt.get());
                if (traitDecl) {
                    AnalyzedSymbol sym;
                    sym.name = traitDecl->name;
                    sym.normalized_name = arabic::normalize_arabic(traitDecl->name);
                    sym.kind = AnalyzedSymbolKind::Interface;  // السمات تُعامل كواجهات
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(traitDecl->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = traitDecl->isExported;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);
                    result.symbols.push_back(sym);

                    // ──── دوال السمة ────
                    for (const auto& m : traitDecl->methods) {
                        AnalyzedSymbol msym;
                        msym.name = m.name;
                        msym.normalized_name = arabic::normalize_arabic(m.name);
                        msym.kind = AnalyzedSymbolKind::Method;
                        msym.type = data_type_to_type_info(m.returnType);
                        msym.uri = uri;
                        msym.scope_owner = traitDecl->name;
                        msym.scope_depth = 1;

                        AnalyzedSymbol::FunctionInfo fi;
                        fi.return_type = data_type_to_type_info(m.returnType);
                        for (const auto& p : m.params) {
                            fi.parameters.push_back({p.name, data_type_to_type_info(p.type)});
                        }
                        msym.func_info = fi;
                        result.symbols.push_back(msym);
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑦ كتلة تنفيذ (ImplDecl) - Phase 9 BeeOS
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  تنفيذ سمة لنوع أو إضافة دوال لبنية:
                //    نفّذ قابل_للعرض لـ نقطة { ... }
                //    نفّذ نقطة { دالة المسافة(هذا) عشري { ... } }
                //
                //  نجمع دوال التنفيذ كطرق منتمية للنوع المستهدف.
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* implDecl = dynamic_cast<Sad::AST::ImplDecl*>(stmt.get());
                if (implDecl) {
                    // الدوال في كتلة التنفيذ تنتمي للنوع المستهدف
                    for (const auto& m : implDecl->methods) {
                        if (!m) continue;
                        auto* mf = dynamic_cast<Sad::AST::FunctionDecl*>(m.get());
                        if (mf) {
                            AnalyzedSymbol msym;
                            msym.name = mf->name;
                            msym.normalized_name = arabic::normalize_arabic(mf->name);
                            msym.kind = AnalyzedSymbolKind::Method;
                            msym.type = data_type_to_type_info(mf->returnType);
                            msym.uri = uri;
                            msym.definition_range = lexer_pos_to_range(mf->position);
                            msym.name_range = msym.definition_range;
                            msym.scope_owner = implDecl->targetType;
                            msym.scope_depth = 1;
                            msym.documentation = extract_documentation(lines, msym.definition_range.start.line);

                            AnalyzedSymbol::FunctionInfo fi;
                            fi.return_type = data_type_to_type_info(mf->returnType);
                            for (const auto& p : mf->parameters) {
                                fi.parameters.push_back({p.name, data_type_to_type_info(p.type)});
                            }
                            msym.func_info = fi;
                            result.symbols.push_back(msym);
                        }
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑧ فضاء أسماء (NamespaceDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  يمثل فضاء أسماء لتنظيم الكود:
                //    فضاء رياضيات { دالة جيب(زاوية) { ... } }
                //
                //  نجمع: اسم الفضاء + نعبر أعضاءه بنفس المنطق
                //  (كل الأنواع: دوال، أصناف، متغيرات...).
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* nsDecl = dynamic_cast<Sad::AST::NamespaceDecl*>(stmt.get());
                if (nsDecl) {
                    AnalyzedSymbol sym;
                    sym.name = nsDecl->name;
                    sym.normalized_name = arabic::normalize_arabic(nsDecl->name);
                    sym.kind = AnalyzedSymbolKind::Module;  // الفضاءات تُعامل كوحدات
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(nsDecl->position);
                    sym.name_range = sym.definition_range;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);
                    result.symbols.push_back(sym);

                    // عبور أعضاء الفضاء
                    for (const auto& m : nsDecl->members) {
                        if (!m) continue;
                        // دوال في الفضاء
                        auto* nf = dynamic_cast<Sad::AST::FunctionDecl*>(m.get());
                        if (nf) {
                            AnalyzedSymbol fsym;
                            fsym.name = nf->name;
                            fsym.normalized_name = arabic::normalize_arabic(nf->name);
                            fsym.kind = AnalyzedSymbolKind::Function;
                            fsym.type = data_type_to_type_info(nf->returnType);
                            fsym.uri = uri;
                            fsym.definition_range = lexer_pos_to_range(nf->position);
                            fsym.name_range = fsym.definition_range;
                            fsym.scope_owner = nsDecl->name;
                            fsym.scope_depth = 1;
                            fsym.is_exported = nf->isExported;

                            AnalyzedSymbol::FunctionInfo fi;
                            fi.return_type = data_type_to_type_info(nf->returnType);
                            for (const auto& p : nf->parameters) {
                                fi.parameters.push_back({p.name, data_type_to_type_info(p.type)});
                            }
                            fsym.func_info = fi;
                            result.symbols.push_back(fsym);
                        }
                        // متغيرات في الفضاء
                        auto* nv = dynamic_cast<Sad::AST::VarDeclStmt*>(m.get());
                        if (nv) {
                            AnalyzedSymbol vsym;
                            vsym.name = nv->name;
                            vsym.normalized_name = arabic::normalize_arabic(nv->name);
                            vsym.kind = nv->isConst ? AnalyzedSymbolKind::Constant
                                                     : AnalyzedSymbolKind::Variable;
                            vsym.type = data_type_to_type_info(nv->type);
                            vsym.uri = uri;
                            vsym.definition_range = lexer_pos_to_range(nv->position);
                            vsym.name_range = vsym.definition_range;
                            vsym.scope_owner = nsDecl->name;
                            vsym.scope_depth = 1;
                            result.symbols.push_back(vsym);
                        }
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑨ دالة قالب (TemplateFunctionDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  دالة معممة مع معاملات أنواع:
                //    قالب<نوع ت> دالة أكبر(أ: ت، ب: ت) ت { ... }
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* tmplFunc = dynamic_cast<Sad::AST::TemplateFunctionDecl*>(stmt.get());
                if (tmplFunc) {
                    AnalyzedSymbol sym;
                    sym.name = tmplFunc->name;
                    sym.normalized_name = arabic::normalize_arabic(tmplFunc->name);
                    sym.kind = AnalyzedSymbolKind::Function;
                    sym.type = data_type_to_type_info(tmplFunc->returnType);
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(tmplFunc->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = tmplFunc->isExported;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    // بناء توقيع مع معاملات الأنواع
                    // مثل: أكبر<ت>(أ: ت، ب: ت) → ت
                    std::string type_params = "<";
                    for (size_t i = 0; i < tmplFunc->typeParameters.size(); i++) {
                        if (i > 0) type_params += "، ";
                        type_params += tmplFunc->typeParameters[i].name;
                        if (!tmplFunc->typeParameters[i].constraint.empty()) {
                            type_params += ": " + tmplFunc->typeParameters[i].constraint;
                        }
                    }
                    type_params += ">";

                    AnalyzedSymbol::FunctionInfo fi;
                    fi.return_type = data_type_to_type_info(tmplFunc->returnType);
                    for (const auto& p : tmplFunc->parameters) {
                        fi.parameters.push_back({p.name, data_type_to_type_info(p.type)});
                    }
                    sym.func_info = fi;

                    // إضافة معلومات القالب للتوثيق
                    if (!sym.documentation.empty()) sym.documentation += "\n";
                    sym.documentation += "قالب" + type_params;

                    result.symbols.push_back(sym);
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑩ صنف قالب (TemplateClassDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  صنف معمم مع معاملات أنواع:
                //    قالب<نوع ت> صنف صندوق { ... }
                //    قالب<نوع ك، نوع ق> صنف قاموس { ... }
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* tmplClass = dynamic_cast<Sad::AST::TemplateClassDecl*>(stmt.get());
                if (tmplClass) {
                    AnalyzedSymbol sym;
                    sym.name = tmplClass->name;
                    sym.normalized_name = arabic::normalize_arabic(tmplClass->name);
                    sym.kind = AnalyzedSymbolKind::Class;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(tmplClass->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = tmplClass->isExported;
                    sym.scope_depth = 0;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    AnalyzedSymbol::ClassInfo ci;
                    for (const auto& base : tmplClass->superclasses) {
                        ci.parent_class = base;
                    }
                    sym.class_info = ci;

                    // إضافة معلومات القالب
                    std::string type_params = "<";
                    for (size_t i = 0; i < tmplClass->typeParameters.size(); i++) {
                        if (i > 0) type_params += "، ";
                        type_params += tmplClass->typeParameters[i].name;
                    }
                    type_params += ">";
                    if (!sym.documentation.empty()) sym.documentation += "\n";
                    sym.documentation += "قالب" + type_params;

                    result.symbols.push_back(sym);

                    // عبور أعضاء الصنف القالب
                    for (const auto& member : tmplClass->members) {
                        if (!member) continue;
                        auto* mf = dynamic_cast<Sad::AST::FunctionDecl*>(member.get());
                        if (mf) {
                            AnalyzedSymbol msym;
                            msym.name = mf->name;
                            msym.normalized_name = arabic::normalize_arabic(mf->name);
                            msym.kind = AnalyzedSymbolKind::Method;
                            msym.type = data_type_to_type_info(mf->returnType);
                            msym.uri = uri;
                            msym.definition_range = lexer_pos_to_range(mf->position);
                            msym.name_range = msym.definition_range;
                            msym.scope_owner = tmplClass->name;
                            msym.scope_depth = 1;
                            result.symbols.push_back(msym);
                        }
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑪ استيراد كامل (ImportStmt)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  مثال: استورد رياضيات
                //  مثال: استورد رياضيات كـ ر
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* importStmt = dynamic_cast<Sad::AST::ImportStmt*>(stmt.get());
                if (importStmt) {
                    AnalyzedSymbol sym;
                    // ملاحظة: modulePath هو vector<string> فنستخدم العنصر الأخير أو ندمج المسار
                    std::string modulePathStr;
                    for (size_t i = 0; i < importStmt->modulePath.size(); ++i) {
                        if (i > 0) modulePathStr += ".";
                        modulePathStr += importStmt->modulePath[i];
                    }
                    sym.name = modulePathStr;
                    sym.normalized_name = arabic::normalize_arabic(modulePathStr);
                    sym.kind = AnalyzedSymbolKind::Import;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(importStmt->position);
                    sym.name_range = sym.definition_range;
                    sym.scope_depth = 0;
                    result.symbols.push_back(sym);
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑫ استيراد انتقائي (FromImportStmt)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  مثال: من رياضيات استورد جيب، جتا
                //  مثال: من رياضيات استورد جيب كـ س
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* fromImport = dynamic_cast<Sad::AST::FromImportStmt*>(stmt.get());
                if (fromImport) {
                    // كل عنصر مستورد يُضاف كرمز في النطاق الحالي
                    for (const auto& item : fromImport->items) {
                        AnalyzedSymbol sym;
                        // نستخدم الاسم المستعار إذا وُجد
                        // ملاحظة: alias هو optional<string> فنستخدم has_value() و value()
                        sym.name = item.alias.has_value() ? item.alias.value() : item.name;
                        sym.normalized_name = arabic::normalize_arabic(sym.name);
                        sym.kind = AnalyzedSymbolKind::Import;
                        sym.uri = uri;
                        sym.definition_range = lexer_pos_to_range(fromImport->position);
                        sym.name_range = sym.definition_range;
                        sym.scope_depth = 0;
                        result.symbols.push_back(sym);
                    }
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑬ تصدير (ExportStmt)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  مثال: صدّر دالة الرئيسية() { ... }
                //  التصدير يُعلّم التصريح الداخلي كمصدّر
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* exportStmt = dynamic_cast<Sad::AST::ExportStmt*>(stmt.get());
                if (exportStmt && exportStmt->declaration) {
                    // نعامل التصريح المصدّر ونعلمه كمصدّر
                    // (سيُعالج في الدورة التالية إذا أُضيف للبرنامج)
                    continue;
                }

                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  ⑭ تصريح اختبار (TestDecl)
                //  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                //  مثال: اختبر("الجمع يعمل") { تأكد(1 + 1 == 2) }
                // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                auto* testDecl = dynamic_cast<Sad::AST::TestDecl*>(stmt.get());
                if (testDecl) {
                    AnalyzedSymbol sym;
                    sym.name = "اختبار: " + testDecl->testName;
                    sym.normalized_name = arabic::normalize_arabic(testDecl->testName);
                    sym.kind = AnalyzedSymbolKind::Function;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(testDecl->position);
                    sym.name_range = sym.definition_range;
                    sym.scope_depth = 0;
                    sym.documentation = "اختبار: " + testDecl->testName;
                    result.symbols.push_back(sym);
                    continue;
                }

            } // نهاية حلقة عبور عقد البرنامج
        } // نهاية if (!program.empty())

        // نجاح التحليل!
        result.success = true;

    } catch (const std::exception& e) {
        // ════════════════════════════════════════════════════════════════════
        //  معالجة الأخطاء: إذا فشل التحليل كلياً
        //  ════════════════════════════════════════════════════════════════════
        //  هذا يحدث عندما يكون الكود مكسوراً جداً بحيث لا يستطيع
        //  المحلل المعجمي حتى أن يعمل. في هذه الحالة:
        //    ① نضيف تشخيصاً بالخطأ
        //    ② نستخدم التحليل الاحتياطي (regex)
        //  هذا يضمن أن المستخدم يحصل على بعض المعلومات حتى مع كود مكسور.
        // ════════════════════════════════════════════════════════════════════
        Diagnostic diag;
        diag.severity = DiagnosticSeverity::Error;
        diag.message = std::string("خطأ في التحليل: ") + e.what();
        diag.source = "ص-محلل";
        diag.code = "ص-٠٠٠";
        result.diagnostics.push_back(diag);

        // التحليل الاحتياطي بـ regex
        collect_symbols_from_ast(content, uri, result);
    }

    // ════════════════════════════════════════════════════════════════════════
    //  المرحلة ٤: تتبع المراجع
    //  ════════════════════════════════════════════════════════════════════════
    //  بعد جمع كل الرموز، نبحث عن كل استخداماتها في الكود.
    //  هذا يتيح: Go to References, Rename, Code Lens (عدد المراجع)
    // ════════════════════════════════════════════════════════════════════════
    collect_references(content, uri, result.symbols, result.references);

    // ════════════════════════════════════════════════════════════════════════
    //  المرحلة ٥: التشخيصات الإضافية المتقدمة
    //  ════════════════════════════════════════════════════════════════════════
    //  تحذيرات واقتراحات تتجاوز الأخطاء النحوية:
    //    • متغيرات غير مستخدمة
    //    • تعريفات مكررة
    //    • كلمات إنجليزية لها مقابل عربي
    //    • أقواس غير متوازنة
    //    • مسافات مختلطة (spaces + tabs)
    //    • أسطر طويلة جداً
    //    • مسافات زائدة في نهاية السطر
    //    • تعليقات TODO/FIXME
    // ════════════════════════════════════════════════════════════════════════
    collect_extra_diagnostics(content, uri, result);

    return result;
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٧: جمع الرموز باستخدام regex (خطة احتياطية)
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  هذه الدالة تعمل كخطة بديلة عندما:
//    • يفشل المحلل النحوي بسبب كود مكسور جداً
//    • لالتقاط رموز قد لا يلتقطها المحلل (حالات حافة)
//
//  لكل رمز تلتقطه regex، نتأكد أنه لم يُضف من قبل (من التحليل الحقيقي).
//  هذا يمنع التكرار ويضمن أن التحليل الحقيقي يأخذ الأولوية.
//
//  الأنماط المكتشفة:
//    • "دالة اسم_الدالة(...)"  → رمز Function
//    • "صنف اسم_الصنف ..."    → رمز Class
//    • "متغير اسم = ..."       → رمز Variable
//    • "ثابت اسم = ..."        → رمز Constant
//    • "تعداد اسم { ... }"     → رمز Enum
//    • "بنية اسم { ... }"      → رمز Struct
//    • "سمة اسم { ... }"       → رمز Interface (Trait)
//    • "فضاء اسم { ... }"      → رمز Module (Namespace)
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

void AnalysisPipeline::collect_symbols_from_ast(
    const std::string& content,
    const DocumentUri& uri,
    AnalysisResult& result)
{
    auto lines = arabic::split_lines(content);

    // ──── جدول الكلمات المفتاحية والأنماط ────
    // كل عنصر يحتوي: الكلمة المفتاحية (UTF-8)، نوع الرمز، حرف النهاية
    struct KeywordPattern {
        std::string keyword;              // الكلمة المفتاحية بـ UTF-8
        AnalyzedSymbolKind kind;          // نوع الرمز
        std::string end_chars;            // حروف تنهي اسم الرمز
    };

    // الكلمات المفتاحية التي تعرّف رموزاً (بـ UTF-8)
    static const std::vector<KeywordPattern> patterns = {
        {"\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9", AnalyzedSymbolKind::Function, "({ \t"},   // دالة
        {"\xd8\xb5\xd9\x86\xd9\x81",          AnalyzedSymbolKind::Class,    " \t{(:"},   // صنف
        {"\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1", AnalyzedSymbolKind::Variable, " \t=:"}, // متغير
        {"\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa",  AnalyzedSymbolKind::Constant, " \t=:"},   // ثابت
        {"\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf", AnalyzedSymbolKind::Enum, " \t{"},  // تعداد
        {"\xd8\xa8\xd9\x86\xd9\x8a\xd8\xa9",  AnalyzedSymbolKind::Struct,   " \t{<"},   // بنية
        {"\xd8\xb3\xd9\x85\xd8\xa9",          AnalyzedSymbolKind::Interface, " \t{<"},   // سمة
        {"\xd9\x81\xd8\xb6\xd8\xa7\xd8\xa1",  AnalyzedSymbolKind::Module,   " \t{"},    // فضاء
    };

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];

        // تخطي التعليقات
        size_t first_non_space = 0;
        while (first_non_space < line.size() &&
               (line[first_non_space] == ' ' || line[first_non_space] == '\t'))
            first_non_space++;
        if (first_non_space < line.size() && line[first_non_space] == '#') continue;

        // ──── البحث عن كل نمط ────
        for (const auto& pat : patterns) {
            size_t pos = line.find(pat.keyword);
            if (pos == std::string::npos) continue;

            // تخطي الكلمة المفتاحية والمسافة
            size_t name_start = pos + pat.keyword.size();
            while (name_start < line.size() &&
                   (line[name_start] == ' ' || line[name_start] == '\t')) {
                name_start++;
            }

            // قراءة الاسم حتى أحد حروف النهاية
            size_t name_end = name_start;
            while (name_end < line.size() &&
                   pat.end_chars.find(line[name_end]) == std::string::npos) {
                name_end++;
            }

            if (name_end <= name_start) continue;
            std::string name = line.substr(name_start, name_end - name_start);
            if (name.empty()) continue;

            // ──── التحقق من عدم التكرار ────
            bool already_exists = false;
            for (const auto& sym : result.symbols) {
                if (sym.name == name &&
                    (sym.kind == pat.kind ||
                     (pat.kind == AnalyzedSymbolKind::Variable && sym.kind == AnalyzedSymbolKind::Constant) ||
                     (pat.kind == AnalyzedSymbolKind::Constant && sym.kind == AnalyzedSymbolKind::Variable))) {
                    already_exists = true;
                    break;
                }
            }

            if (!already_exists) {
                AnalyzedSymbol sym;
                sym.name = name;
                sym.normalized_name = arabic::normalize_arabic(name);
                sym.kind = pat.kind;
                sym.uri = uri;
                sym.definition_range.start = {i, 0};
                sym.definition_range.end = {i, static_cast<int>(line.size())};
                sym.name_range.start = {i, static_cast<int>(name_start)};
                sym.name_range.end = {i, static_cast<int>(name_end)};
                sym.documentation = extract_documentation(lines, i);
                result.symbols.push_back(sym);
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════════════════════════
//  القسم ٨: التشخيصات الإضافية المتقدمة
// ══════════════════════════════════════════════════════════════════════════════════════════════════
//
//  هذه التشخيصات تتجاوز الأخطاء النحوية. تشمل:
//
//  ┌─────────────────────────────────────────────────────────────────────────┐
//  │ الرقم │ النوع    │ الوصف                                               │
//  ├───────┼──────────┼─────────────────────────────────────────────────────│
//  │  ١    │ تلميح   │ متغير معرّف لكنه غير مستخدم                         │
//  │  ٢    │ تحذير   │ تعريف مكرر لنفس الاسم                               │
//  │  ٣    │ تلميح   │ سطر طويل جداً (أكثر من 200 حرف)                     │
//  │  ٤    │ معلومة  │ استخدام print بدل اطبع                              │
//  │  ٥    │ معلومة  │ كلمة إنجليزية لها مقابل عربي                        │
//  │  ٦    │ معلومة  │ تعليق TODO/FIXME/HACK يحتاج مراجعة                  │
//  │  ٧    │ تحذير   │ أقواس غير متوازنة (أكثر من اثنتين)                  │
//  │  ٨    │ تلميح   │ مسافات مختلطة (spaces + tabs)                       │
//  │  ٩    │ تلميح   │ مسافات زائدة في نهاية السطر                         │
//  └─────────────────────────────────────────────────────────────────────────┘
//
// ══════════════════════════════════════════════════════════════════════════════════════════════════

void AnalysisPipeline::collect_extra_diagnostics(
    const std::string& content,
    const DocumentUri& uri,
    AnalysisResult& result)
{
    auto lines = arabic::split_lines(content);

    // ════════════════════════════════════════════════════════════════════════
    //  ① كشف المتغيرات غير المستخدمة
    // ════════════════════════════════════════════════════════════════════════
    //  نبني خريطة: اسم المتغير → عدد المراجع (بدون التعريف).
    //  إذا كان العدد = 0، فالمتغير غير مستخدم.
    //
    //  هذا مفيد جداً لتنظيف الكود من المتغيرات الميتة، خاصة أن
    //  المبرمجين المبتدئين كثيراً ما ينسون حذف المتغيرات غير المستخدمة.
    // ════════════════════════════════════════════════════════════════════════
    {
        std::unordered_map<std::string, int> ref_counts;
        for (const auto& sym : result.symbols) {
            if (sym.kind == AnalyzedSymbolKind::Variable ||
                sym.kind == AnalyzedSymbolKind::Constant ||
                sym.kind == AnalyzedSymbolKind::Parameter) {
                ref_counts[sym.name] = 0;
            }
        }
        // حساب المراجع (بدون التعريف نفسه)
        for (const auto& ref : result.references) {
            if (ref.is_declaration) continue;
            int ref_line = ref.range.start.line;
            int ref_col = ref.range.start.character;
            int ref_end = ref.range.end.character;
            if (ref_line < static_cast<int>(lines.size())) {
                std::string ref_text = lines[ref_line].substr(
                    std::max(0, ref_col),
                    std::max(0, ref_end - ref_col));
                if (ref_counts.count(ref_text)) {
                    ref_counts[ref_text]++;
                }
            }
        }

        for (const auto& sym : result.symbols) {
            if ((sym.kind == AnalyzedSymbolKind::Variable ||
                 sym.kind == AnalyzedSymbolKind::Constant) &&
                ref_counts.count(sym.name) && ref_counts[sym.name] == 0 &&
                sym.scope_depth == 0) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Hint;
                diag.message = "المتغير '" + sym.name + "' معرّف لكنه غير مستخدم";
                diag.source = "ص-تحليل";
                diag.code = "ص-ت١٠١";
                diag.range = sym.name_range;
                diag.tags.push_back(DiagnosticTag::Unnecessary);
                result.diagnostics.push_back(diag);
            }
        }
    }

    // ════════════════════════════════════════════════════════════════════════
    //  ② كشف التعريفات المكررة في نفس النطاق
    // ════════════════════════════════════════════════════════════════════════
    //  إذا عرّف المبرمج نفس الاسم مرتين في المستوى العلوي:
    //    دالة حساب() { ... }
    //    دالة حساب() { ... }  ← تحذير: تعريف مكرر
    // ════════════════════════════════════════════════════════════════════════
    {
        std::unordered_map<std::string, std::vector<const AnalyzedSymbol*>> name_groups;
        for (const auto& sym : result.symbols) {
            if (sym.scope_depth == 0 &&
                (sym.kind == AnalyzedSymbolKind::Function ||
                 sym.kind == AnalyzedSymbolKind::Class ||
                 sym.kind == AnalyzedSymbolKind::Variable ||
                 sym.kind == AnalyzedSymbolKind::Constant ||
                 sym.kind == AnalyzedSymbolKind::Enum ||
                 sym.kind == AnalyzedSymbolKind::Struct ||
                 sym.kind == AnalyzedSymbolKind::Interface)) {
                name_groups[sym.name].push_back(&sym);
            }
        }
        for (const auto& [name, syms] : name_groups) {
            if (syms.size() > 1) {
                for (size_t i = 1; i < syms.size(); i++) {
                    Diagnostic diag;
                    diag.severity = DiagnosticSeverity::Warning;
                    diag.message = "تعريف مكرر للاسم '" + name + "'";
                    diag.source = "ص-تحليل";
                    diag.code = "ص-ت١٠٢";
                    diag.range = syms[i]->name_range;

                    // معلومات مرتبطة: مكان التعريف الأول
                    DiagnosticRelatedInformation related;
                    related.location = {uri, syms[0]->name_range};
                    related.message = "التعريف الأول هنا";
                    diag.related.push_back(related);

                    result.diagnostics.push_back(diag);
                }
            }
        }
    }

    // ════════════════════════════════════════════════════════════════════════
    //  فحص كل سطر للتشخيصات ③-⑨
    // ════════════════════════════════════════════════════════════════════════
    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];

        // ──── ③ تحذير: سطر طويل جداً (أكثر من ~200 حرف) ────
        // النص العربي يأخذ ~2-4 بايت لكل حرف بـ UTF-8
        if (line.size() > 400) {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Hint;
            diag.message = "هذا السطر طويل جداً. يُفضل تقسيمه لسطور أقصر لتسهيل القراءة.";
            diag.source = "ص-نمط";
            diag.code = "ص-ت٠٠١";
            diag.range.start = {i, 0};
            diag.range.end = {i, static_cast<int>(line.size())};
            result.diagnostics.push_back(diag);
        }

        // ──── ④ تحذير: استخدام print بدل اطبع ────
        if (line.find("print(") != std::string::npos ||
            line.find("print (") != std::string::npos) {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Information;
            diag.message = "استخدم 'اطبع' بدلاً من 'print' — لغة ص تستخدم الكلمات العربية";
            diag.source = "ص-نمط";
            diag.code = "ص-ت٠٠٢";
            auto pos = line.find("print");
            diag.range.start = {i, static_cast<int>(pos)};
            diag.range.end = {i, static_cast<int>(pos + 5)};
            result.diagnostics.push_back(diag);
        }

        // ──── ⑤ كشف كلمات إنجليزية شائعة لها مقابل عربي ────
        // هذا يساعد المبرمجين الجدد على تعلم المفردات العربية للغة ص
        {
            struct EnglishArabic { const char* english; const char* arabic; int len; };
            static const EnglishArabic eng_ar_map[] = {
                {"function",  "دالة",     8},
                {"class",     "صنف",      5},
                {"if",        "إذا",      2},
                {"else",      "وإلا",     4},
                {"while",     "بينما",    5},
                {"for",       "لكل",      3},
                {"return",    "إرجاع",    6},
                {"var",       "متغير",    3},
                {"let",       "متغير",    3},
                {"const",     "ثابت",     5},
                {"true",      "صحيح",     4},
                {"false",     "خطأ",      5},
                {"null",      "عدم",      4},
                {"import",    "استورد",   6},
                {"try",       "حاول",     3},
                {"catch",     "امسك",     5},
                {"break",     "اخرج",     5},
                {"continue",  "استمر",    8},
                {"struct",    "بنية",     6},
                {"trait",     "سمة",      5},
                {"enum",      "تعداد",    4},
            };

            for (const auto& ea : eng_ar_map) {
                size_t search_pos = 0;
                while (search_pos < line.size()) {
                    size_t found = line.find(ea.english, search_pos);
                    if (found == std::string::npos) break;

                    // تحقق من حدود الكلمة
                    bool valid_start = (found == 0) ||
                        (!std::isalnum(static_cast<unsigned char>(line[found - 1])) && line[found - 1] != '_');
                    size_t end_pos = found + ea.len;
                    bool valid_end = (end_pos >= line.size()) ||
                        (!std::isalnum(static_cast<unsigned char>(line[end_pos])) && line[end_pos] != '_');

                    // تخطي إذا كان داخل نص مقتبس (تقريبي)
                    bool in_string = false;
                    int quote_count = 0;
                    for (size_t j = 0; j < found; j++) {
                        if (line[j] == '"' || line[j] == '\'') quote_count++;
                    }
                    if (quote_count % 2 != 0) in_string = true;

                    // تخطي إذا كان في تعليق
                    bool in_comment = false;
                    for (size_t j = 0; j < found; j++) {
                        if (line[j] == '#') { in_comment = true; break; }
                    }

                    if (valid_start && valid_end && !in_string && !in_comment &&
                        std::string(ea.english) != "print") {
                        Diagnostic diag;
                        diag.severity = DiagnosticSeverity::Information;
                        diag.message = std::string("لغة ص تستخدم '") + ea.arabic +
                                       "' بدلاً من '" + ea.english + "'";
                        diag.source = "ص-نمط";
                        diag.code = "ص-ت٠٠٣";
                        diag.range.start = {i, static_cast<int>(found)};
                        diag.range.end = {i, static_cast<int>(end_pos)};
                        result.diagnostics.push_back(diag);
                    }
                    search_pos = end_pos;
                }
            }
        }

        // ──── ⑥ تحذير: تعليق TODO/FIXME/HACK ────
        // مفيد لتذكير المبرمج بالمهام المعلقة
        if (line.find("TODO") != std::string::npos ||
            line.find("FIXME") != std::string::npos ||
            line.find("HACK") != std::string::npos) {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Information;
            diag.message = "يوجد تعليق يحتاج مراجعة";
            diag.source = "ص-نمط";
            diag.code = "ص-ت٠٠٤";
            diag.range.start = {i, 0};
            diag.range.end = {i, static_cast<int>(line.size())};
            result.diagnostics.push_back(diag);
        }

        // ──── ⑦ تحذير: أقواس غير متوازنة ────
        // نحسب عدد الأقواس المفتوحة والمغلقة في السطر
        // إذا كان الفرق كبيراً (>2)، غالباً هناك خطأ
        {
            int parens = 0, braces = 0, brackets = 0;
            bool in_str = false;
            char str_char = 0;
            for (size_t j = 0; j < line.size(); j++) {
                char c = line[j];
                if (in_str) {
                    if (c == str_char && (j == 0 || line[j-1] != '\\')) in_str = false;
                    continue;
                }
                if (c == '"' || c == '\'') { in_str = true; str_char = c; continue; }
                if (c == '#') break;  // تعليق - نتوقف
                if (c == '(') parens++;
                else if (c == ')') parens--;
                else if (c == '{') braces++;
                else if (c == '}') braces--;
                else if (c == '[') brackets++;
                else if (c == ']') brackets--;
            }
            if (parens > 2 || parens < -2) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Warning;
                diag.message = parens > 0 ? "يوجد أقواس مفتوحة بدون إغلاق"
                                           : "يوجد أقواس مغلقة بدون فتح";
                diag.source = "ص-نحو";
                diag.code = "ص-ن٠٠١";
                diag.range.start = {i, 0};
                diag.range.end = {i, static_cast<int>(line.size())};
                result.diagnostics.push_back(diag);
            }
        }

        // ──── ⑧ تحذير: مسافات مختلطة (spaces + tabs) ────
        // الخلط بينهما يسبب مشاكل في الرؤية والتنسيق
        {
            bool has_spaces = false, has_tabs = false;
            for (size_t j = 0; j < line.size(); j++) {
                if (line[j] == ' ') has_spaces = true;
                else if (line[j] == '\t') has_tabs = true;
                else break;
            }
            if (has_spaces && has_tabs) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Hint;
                diag.message = "يوجد خلط بين المسافات والجداول (tabs) في المسافة البادئة";
                diag.source = "ص-نمط";
                diag.code = "ص-ت٠٠٥";
                diag.range.start = {i, 0};
                diag.range.end = {i, static_cast<int>(line.size())};
                result.diagnostics.push_back(diag);
            }
        }

        // ──── ⑨ تحذير: مسافات زائدة في نهاية السطر ────
        // تلوث git diff وتسبب مراجعات كود مزعجة
        if (!line.empty()) {
            size_t end = line.size();
            while (end > 0 && (line[end-1] == ' ' || line[end-1] == '\t')) end--;
            if (end < line.size() && end > 0) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Hint;
                diag.message = "يوجد مسافات زائدة في نهاية السطر";
                diag.source = "ص-نمط";
                diag.code = "ص-ت٠٠٦";
                diag.range.start = {i, static_cast<int>(end)};
                diag.range.end = {i, static_cast<int>(line.size())};
                diag.tags.push_back(DiagnosticTag::Unnecessary);
                result.diagnostics.push_back(diag);
            }
        }
    }
}

} // namespace lsp
} // namespace sad
