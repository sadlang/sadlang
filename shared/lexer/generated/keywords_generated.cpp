// ============================================================================
// AUTO-GENERATED FROM language-truth/keywords.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد البناء.
// (EN) Auto-generated file. To modify, edit YAML and rebuild.
//
// (AR) نسخة المعجم: 5.0
// (EN) Lexicon version: 5.0
// ============================================================================

#include "keywords_generated.h"

#include <unordered_map>

namespace Sad {
namespace Lexer {
namespace Generated {

const std::vector<KeywordEntry>& allEntries() {
    static const std::vector<KeywordEntry> entries = {
        // ─── reserved (40) ───
        { "دالة", TokenType::KEYWORD_FUNCTION, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "function", "تعريف دالة جديدة — كتلة كود قابلة للاستدعاء بالاسم." },
        { "ارجع", TokenType::KEYWORD_RETURN, KeywordCategory::RESERVED, true, {}, {}, "return", "إرجاع قيمة من دالة وإنهاء تنفيذها." },
        { "صنف", TokenType::KEYWORD_CLASS, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "class", "تعريف صنف — قالب لإنشاء كائنات تحمل بيانات وسلوكاً." },
        { "بنية", TokenType::KEYWORD_STRUCT, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "struct", "تعريف بنية — نوع مركّب يجمع حقولاً مترابطة." },
        { "تعداد", TokenType::KEYWORD_ENUM, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "enum", "تعريف تعداد — مجموعة قيم رمزية مُسمّاة." },
        { "يرث", TokenType::KEYWORD_INHERITS, KeywordCategory::RESERVED, true, {}, {}, "inherits", "وراثة صنف من صنف أب لاكتساب أعضائه." },
        { "نهاية", TokenType::KEYWORD_END, KeywordCategory::RESERVED, true, {}, {"block_closer"}, "end", "إغلاق كتلة (دالة/صنف/شرط…) بنمط الكلمات." },
        { "جديد", TokenType::KEYWORD_NEW, KeywordCategory::RESERVED, true, {}, {}, "new", "إنشاء نسخة (كائن) جديدة من صنف." },
        { "هذا", TokenType::KEYWORD_THIS, KeywordCategory::RESERVED, true, {}, {}, "this", "مرجع للكائن الحالي داخل أعضاء الصنف." },
        { "باني", TokenType::KEYWORD_CONSTRUCTOR, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "constructor", "دالة بناء الكائن — تُستدعى عند الإنشاء." },
        { "الأساس", TokenType::KEYWORD_SUPER, KeywordCategory::RESERVED, true, {}, {}, "super", "الوصول إلى أعضاء الصنف الأب (super)." },
        { "إذا", TokenType::KEYWORD_IF, KeywordCategory::RESERVED, true, {"اذا"}, {"block_opener"}, "if", "جملة شرطية — تُنفَّذ كتلتها إن صَحّ الشرط." },
        { "وإلا", TokenType::KEYWORD_ELSE, KeywordCategory::RESERVED, true, {"والا"}, {"inter_block"}, "else", "الفرع البديل لجملة شرطية عند فشل الشرط." },
        { "بينما", TokenType::KEYWORD_WHILE, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "while", "حلقة تتكرّر طالما الشرط صحيح." },
        { "لكل", TokenType::KEYWORD_FOR, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "for", "حلقة تمرّ على كل عنصر في مجموعة." },
        { "في", TokenType::KEYWORD_IN, KeywordCategory::RESERVED, true, {}, {}, "in", "تُحدِّد المجموعة المُكرَّر عليها في «لكل»." },
        { "توقف", TokenType::KEYWORD_BREAK, KeywordCategory::RESERVED, true, {}, {}, "break", "الخروج الفوري من الحلقة." },
        { "استمر", TokenType::KEYWORD_CONTINUE, KeywordCategory::RESERVED, true, {}, {}, "continue", "الانتقال للدورة التالية من الحلقة فوراً." },
        { "طابق", TokenType::KEYWORD_MATCH, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "match", "مطابقة قيمة على أنماط متعددة (match)." },
        { "عندما", TokenType::KEYWORD_WHEN, KeywordCategory::RESERVED, true, {}, {"inter_block"}, "when", "فرع نمط داخل «طابق»." },
        { "افتراضي", TokenType::KEYWORD_DEFAULT, KeywordCategory::RESERVED, true, {}, {"inter_block"}, "default", "الفرع الافتراضي في «طابق» عند عدم تطابق أيّ نمط." },
        { "حاول", TokenType::KEYWORD_TRY, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "try", "كتلة قد يحدث فيها خطأ يُلتقط في «امسك»." },
        { "امسك", TokenType::KEYWORD_CATCH, KeywordCategory::RESERVED, true, {}, {"inter_block"}, "catch", "التقاط الخطأ الحادث في «حاول»." },
        { "ارمي", TokenType::KEYWORD_THROW, KeywordCategory::RESERVED, true, {}, {}, "throw", "إطلاق خطأ يدوياً." },
        { "أخيراً", TokenType::KEYWORD_FINALLY, KeywordCategory::RESERVED, true, {"أخيرا"}, {"inter_block"}, "finally", "كتلة تُنفَّذ دائماً بعد «حاول/امسك»." },
        { "عام", TokenType::KEYWORD_PUBLIC, KeywordCategory::RESERVED, true, {}, {}, "public", "مُعدِّل وصول — عضو متاح للجميع." },
        { "خاص", TokenType::KEYWORD_PRIVATE, KeywordCategory::RESERVED, true, {}, {}, "private", "مُعدِّل وصول — عضو متاح داخل الصنف فقط." },
        { "محمي", TokenType::KEYWORD_PROTECTED, KeywordCategory::RESERVED, true, {}, {}, "protected", "مُعدِّل وصول — عضو متاح للصنف وورثته." },
        { "مجرد", TokenType::KEYWORD_ABSTRACT, KeywordCategory::RESERVED, true, {}, {}, "abstract", "صنف/عضو مجرَّد لا يُنشأ مباشرة ويجب تنفيذه." },
        { "استورد", TokenType::KEYWORD_IMPORT, KeywordCategory::RESERVED, true, {}, {"import_keyword"}, "import", "استيراد وحدة أو مكتبة خارجية." },
        { "من", TokenType::KEYWORD_FROM, KeywordCategory::RESERVED, true, {}, {"import_keyword"}, "from", "تحديد مصدر الاستيراد (من وحدة)." },
        { "كـ", TokenType::KEYWORD_AS, KeywordCategory::RESERVED, true, {}, {"import_keyword"}, "as", "إسناد اسم بديل للمستورد." },
        { "صدّر", TokenType::KEYWORD_EXPORT, KeywordCategory::RESERVED, true, {"صدر"}, {"import_keyword"}, "export", "تصدير رمز ليكون متاحاً للوحدات الأخرى." },
        { "متغير", TokenType::KEYWORD_VAR, KeywordCategory::RESERVED, true, {}, {}, "var", "تعريف متغير قابل للتعديل." },
        { "ثابت", TokenType::KEYWORD_CONST, KeywordCategory::RESERVED, true, {}, {}, "const", "تعريف ثابت لا تتغيّر قيمته بعد التعيين." },
        { "ساكن", TokenType::KEYWORD_STATIC, KeywordCategory::RESERVED, true, {}, {}, "static", "عضو/متغير ساكن مشترك على مستوى الصنف." },
        { "خارجي", TokenType::KEYWORD_EXTERN, KeywordCategory::RESERVED, true, {}, {}, "extern", "ربط رمز معرَّف خارج الوحدة (extern)." },
        { "صحيح", TokenType::LITERAL_TRUE, KeywordCategory::RESERVED, true, {}, {}, "true", "القيمة المنطقية الحقيقية (true)." },
        { "خطأ", TokenType::LITERAL_FALSE, KeywordCategory::RESERVED, true, {}, {}, "false", "القيمة المنطقية الزائفة (false)." },
        { "لاشيء", TokenType::LITERAL_NULL, KeywordCategory::RESERVED, true, {}, {}, "null", "القيمة العدمية — غياب قيمة (null)." },
        // ─── operators (3) ───
        { "و", TokenType::OP_AND, KeywordCategory::OPERATOR, true, {}, {}, "and", "العامل المنطقي «و» — يصحّ إن صحّ الطرفان." },
        { "أو", TokenType::OP_OR, KeywordCategory::OPERATOR, true, {}, {}, "or", "العامل المنطقي «أو» — يصحّ إن صحّ أحد الطرفين." },
        { "ليس", TokenType::OP_NOT, KeywordCategory::OPERATOR, true, {}, {}, "not", "العامل المنطقي «ليس» — ينفي القيمة المنطقية." },
        // ─── contextual (40) ───
        { "مؤكد", TokenType::OP_NULL_ASSERT, KeywordCategory::CONTEXTUAL, false, {"مؤكدة"}, {}, "asserted", "عامل تأكيد عدم الفراغ — يحوّل اختيارياً «؟» إلى قيمة مؤكَّدة." },
        { "خاصية", TokenType::KEYWORD_PROPERTY, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "property", "تعريف خاصية بمُحصِّل/مُعيِّن داخل صنف." },
        { "احصل", TokenType::KEYWORD_GET, KeywordCategory::CONTEXTUAL, false, {}, {}, "get", "مُحصِّل الخاصية (getter)." },
        { "عيّن", TokenType::KEYWORD_SET, KeywordCategory::CONTEXTUAL, false, {"عين"}, {}, "set", "مُعيِّن الخاصية (setter)." },
        { "هدم", TokenType::KEYWORD_DESTRUCTOR, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "destructor", "دالة هدم الكائن — تُستدعى عند الإتلاف." },
        { "عامل", TokenType::KEYWORD_OPERATOR, KeywordCategory::CONTEXTUAL, false, {}, {}, "operator", "تحميل عامل (operator overloading)." },
        { "رئيسية", TokenType::KEYWORD_MAIN, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "main", "نقطة دخول البرنامج الرئيسية." },
        { "محكم", TokenType::KEYWORD_SEALED, KeywordCategory::CONTEXTUAL, false, {}, {}, "sealed", "صنف محكم لا يقبل الوراثة (sealed)." },
        { "ترجع", TokenType::KEYWORD_RETURNS, KeywordCategory::CONTEXTUAL, false, {}, {}, "returns", "تحديد نوع إرجاع الدالة (returns)." },
        { "حالة", TokenType::KEYWORD_CASE, KeywordCategory::CONTEXTUAL, false, {}, {"inter_block"}, "case", "فرع حالة داخل «طابق» أو «اختر»." },
        { "غير_متزامن", TokenType::KEYWORD_ASYNC, KeywordCategory::CONTEXTUAL, false, {"غير_متزامنة"}, {}, "async", "دالة غير متزامنة تُرجع نتيجة مؤجَّلة." },
        { "انتظر", TokenType::KEYWORD_AWAIT, KeywordCategory::CONTEXTUAL, false, {}, {}, "await", "انتظار اكتمال عملية غير متزامنة." },
        { "لامدا", TokenType::KEYWORD_LAMBDA, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "lambda", "دالة مجهولة قصيرة (lambda)." },
        { "أنتج", TokenType::KEYWORD_YIELD, KeywordCategory::CONTEXTUAL, false, {}, {}, "yield", "إنتاج قيمة من مولِّد (yield)." },
        { "مولد", TokenType::KEYWORD_GENERATOR, KeywordCategory::CONTEXTUAL, false, {}, {}, "generator", "دالة مولِّدة تُنتج سلسلة قيم." },
        { "باستخدام", TokenType::KEYWORD_WITH, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "with", "كتلة إدارة مورد تُغلق تلقائياً (with)." },
        { "نهاية_استخدام", TokenType::KEYWORD_END_WITH, KeywordCategory::CONTEXTUAL, false, {}, {"block_closer"}, "end_with", "إغلاق كتلة «باستخدام»." },
        { "قالب", TokenType::KEYWORD_TEMPLATE, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "template", "تعريف قالب عام بمعاملات نوعية (template)." },
        { "نوع", TokenType::KEYWORD_TYPENAME, KeywordCategory::CONTEXTUAL, false, {}, {}, "typename", "معامل نوع في القوالب (typename)." },
        { "حيث", TokenType::KEYWORD_WHERE, KeywordCategory::CONTEXTUAL, false, {}, {}, "where", "قيد على معاملات القالب (where)." },
        { "فضاء", TokenType::KEYWORD_NAMESPACE, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "namespace", "تعريف فضاء أسماء لتجميع الرموز." },
        { "نهاية_فضاء", TokenType::KEYWORD_END_NAMESPACE, KeywordCategory::CONTEXTUAL, false, {}, {"block_closer"}, "end_namespace", "إغلاق فضاء الأسماء." },
        { "سمة", TokenType::KEYWORD_TRAIT, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "trait", "تعريف سمة (trait) تصف سلوكاً قابلاً للتنفيذ." },
        { "نفّذ", TokenType::KEYWORD_IMPL, KeywordCategory::CONTEXTUAL, false, {"نفذ"}, {"block_opener"}, "impl", "تنفيذ سمة لصنف (impl)." },
        { "امتداد", TokenType::KEYWORD_EXTENSION, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "extension", "إضافة طرق لنوع قائم (extension)." },
        { "ماكرو", TokenType::KEYWORD_MACRO, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "macro", "تعريف ماكرو يُوسَّع وقت الترجمة." },
        { "أجّل", TokenType::KEYWORD_DEFER, KeywordCategory::CONTEXTUAL, false, {"اجل", "أجل", "اجّل"}, {}, "defer", "تأجيل تنفيذ عبارة حتى نهاية النطاق (defer)." },
        { "أطلق", TokenType::KEYWORD_GO, KeywordCategory::CONTEXTUAL, false, {"اطلق"}, {}, "go", "إطلاق مهمة متزامنة خفيفة (goroutine)." },
        { "اختر", TokenType::KEYWORD_SELECT, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "select", "انتظار عدة قنوات تزامن واختيار الجاهز (select)." },
        { "واجهة", TokenType::KEYWORD_UI_DECL, KeywordCategory::CONTEXTUAL, false, {}, {}, "ui_decl", "تعريف واجهة مستخدم تصريحية." },
        { "اعرض", TokenType::KEYWORD_SHOW, KeywordCategory::CONTEXTUAL, false, {}, {}, "show", "عرض عنصر واجهة المستخدم." },
        { "متطاير", TokenType::KEYWORD_VOLATILE, KeywordCategory::CONTEXTUAL, false, {}, {}, "volatile", "متغيّر متطاير لا يُحسَّن وصوله (volatile)." },
        { "اعطِ", TokenType::KEYWORD_GIVE_DEPRECATED, KeywordCategory::CONTEXTUAL, false, {}, {}, "give_deprecated", "كلمة مهجورة — استُبدلت بـ«ارجع»." },
        { "ثم", TokenType::KEYWORD_THEN, KeywordCategory::CONTEXTUAL, false, {}, {}, "then", "ربط تسلسلي بين عبارتين (then)." },
        { "انشر", TokenType::KEYWORD_PROPAGATE, KeywordCategory::CONTEXTUAL, false, {}, {}, "propagate", "نشر الخطأ إلى المُستدعي (propagate)." },
        { "الى", TokenType::KEYWORD_TO, KeywordCategory::CONTEXTUAL, false, {"إلى"}, {}, "to", "تحديد نهاية نطاق عددي (to)." },
        { "يتطلب", TokenType::KEYWORD_REQUIRES, KeywordCategory::CONTEXTUAL, false, {}, {}, "requires", "شرط مسبق في عقد برمجي (requires)." },
        { "يضمن", TokenType::KEYWORD_ENSURES, KeywordCategory::CONTEXTUAL, false, {}, {}, "ensures", "شرط لاحق في عقد برمجي (ensures)." },
        { "عقد", TokenType::KEYWORD_CONTRACT, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "contract", "تعريف عقد برمجي بشروط مسبقة ولاحقة." },
        { "اختبر", TokenType::KEYWORD_TEST, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "test", "تعريف حالة اختبار." },
        // ─── builtin_types (9) ───
        { "رقم", TokenType::TYPE_INTEGER, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "integer", "" },
        { "عشري", TokenType::TYPE_DOUBLE, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "double", "" },
        { "نص", TokenType::TYPE_STRING, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "string", "" },
        { "منطقي", TokenType::TYPE_BOOLEAN, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "boolean", "" },
        { "فراغ", TokenType::TYPE_VOID, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "void", "" },
        { "عدم", TokenType::TYPE_NULL, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "null", "" },
        { "مصفوفة", TokenType::TYPE_ARRAY, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "array", "" },
        { "خريطة", TokenType::TYPE_MAP, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "map", "" },
        { "أي", TokenType::TYPE_ANY, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "any", "" },
    };
    return entries;
}

const std::string& keywordDescriptionAr(const std::string& word) {
    static const std::string kEmpty;
    // (AR) فهرس كلمة→وصف يُبنى مرّة واحدة من allEntries()، يشمل الأسماء البديلة.
    // (EN) Word→description map built once from allEntries(), aliases included.
    static const std::unordered_map<std::string, const std::string*> index = []() {
        std::unordered_map<std::string, const std::string*> m;
        for (const auto& e : allEntries()) {
            if (e.descriptionAr.empty()) continue;
            m.emplace(e.primaryWord, &e.descriptionAr);
            for (const auto& a : e.aliases) m.emplace(a, &e.descriptionAr);
        }
        return m;
    }();
    auto it = index.find(word);
    return it != index.end() ? *(it->second) : kEmpty;
}

} // namespace Generated
} // namespace Lexer
} // namespace Sad
