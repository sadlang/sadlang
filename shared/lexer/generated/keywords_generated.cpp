// ============================================================================
// AUTO-GENERATED FROM language-truth/keywords.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد البناء.
// (EN) Auto-generated file. To modify, edit YAML and rebuild.
//
// (AR) نسخة المعجم: 5.0
// (EN) Lexicon version: 5.0
// ============================================================================

#include "keywords_generated.h"

namespace Sad {
namespace Lexer {
namespace Generated {

const std::vector<KeywordEntry>& allEntries() {
    static const std::vector<KeywordEntry> entries = {
        // ─── reserved (40) ───
        { "دالة", TokenType::KEYWORD_FUNCTION, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "function" },
        { "ارجع", TokenType::KEYWORD_RETURN, KeywordCategory::RESERVED, true, {}, {}, "return" },
        { "صنف", TokenType::KEYWORD_CLASS, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "class" },
        { "بنية", TokenType::KEYWORD_STRUCT, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "struct" },
        { "تعداد", TokenType::KEYWORD_ENUM, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "enum" },
        { "يرث", TokenType::KEYWORD_INHERITS, KeywordCategory::RESERVED, true, {}, {}, "inherits" },
        { "نهاية", TokenType::KEYWORD_END, KeywordCategory::RESERVED, true, {}, {"block_closer"}, "end" },
        { "جديد", TokenType::KEYWORD_NEW, KeywordCategory::RESERVED, true, {}, {}, "new" },
        { "هذا", TokenType::KEYWORD_THIS, KeywordCategory::RESERVED, true, {}, {}, "this" },
        { "باني", TokenType::KEYWORD_CONSTRUCTOR, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "constructor" },
        { "الأساس", TokenType::KEYWORD_SUPER, KeywordCategory::RESERVED, true, {}, {}, "super" },
        { "إذا", TokenType::KEYWORD_IF, KeywordCategory::RESERVED, true, {"اذا"}, {"block_opener"}, "if" },
        { "وإلا", TokenType::KEYWORD_ELSE, KeywordCategory::RESERVED, true, {"والا"}, {"inter_block"}, "else" },
        { "بينما", TokenType::KEYWORD_WHILE, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "while" },
        { "لكل", TokenType::KEYWORD_FOR, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "for" },
        { "في", TokenType::KEYWORD_IN, KeywordCategory::RESERVED, true, {}, {}, "in" },
        { "توقف", TokenType::KEYWORD_BREAK, KeywordCategory::RESERVED, true, {}, {}, "break" },
        { "استمر", TokenType::KEYWORD_CONTINUE, KeywordCategory::RESERVED, true, {}, {}, "continue" },
        { "طابق", TokenType::KEYWORD_MATCH, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "match" },
        { "عندما", TokenType::KEYWORD_WHEN, KeywordCategory::RESERVED, true, {}, {"inter_block"}, "when" },
        { "افتراضي", TokenType::KEYWORD_DEFAULT, KeywordCategory::RESERVED, true, {}, {"inter_block"}, "default" },
        { "حاول", TokenType::KEYWORD_TRY, KeywordCategory::RESERVED, true, {}, {"block_opener"}, "try" },
        { "امسك", TokenType::KEYWORD_CATCH, KeywordCategory::RESERVED, true, {}, {"inter_block"}, "catch" },
        { "ارمي", TokenType::KEYWORD_THROW, KeywordCategory::RESERVED, true, {}, {}, "throw" },
        { "أخيراً", TokenType::KEYWORD_FINALLY, KeywordCategory::RESERVED, true, {"أخيرا"}, {"inter_block"}, "finally" },
        { "عام", TokenType::KEYWORD_PUBLIC, KeywordCategory::RESERVED, true, {}, {}, "public" },
        { "خاص", TokenType::KEYWORD_PRIVATE, KeywordCategory::RESERVED, true, {}, {}, "private" },
        { "محمي", TokenType::KEYWORD_PROTECTED, KeywordCategory::RESERVED, true, {}, {}, "protected" },
        { "مجرد", TokenType::KEYWORD_ABSTRACT, KeywordCategory::RESERVED, true, {}, {}, "abstract" },
        { "استورد", TokenType::KEYWORD_IMPORT, KeywordCategory::RESERVED, true, {}, {"import_keyword"}, "import" },
        { "من", TokenType::KEYWORD_FROM, KeywordCategory::RESERVED, true, {}, {"import_keyword"}, "from" },
        { "كـ", TokenType::KEYWORD_AS, KeywordCategory::RESERVED, true, {}, {"import_keyword"}, "as" },
        { "صدّر", TokenType::KEYWORD_EXPORT, KeywordCategory::RESERVED, true, {"صدر"}, {"import_keyword"}, "export" },
        { "متغير", TokenType::KEYWORD_VAR, KeywordCategory::RESERVED, true, {}, {}, "var" },
        { "ثابت", TokenType::KEYWORD_CONST, KeywordCategory::RESERVED, true, {}, {}, "const" },
        { "ساكن", TokenType::KEYWORD_STATIC, KeywordCategory::RESERVED, true, {}, {}, "static" },
        { "خارجي", TokenType::KEYWORD_EXTERN, KeywordCategory::RESERVED, true, {}, {}, "extern" },
        { "صحيح", TokenType::LITERAL_TRUE, KeywordCategory::RESERVED, true, {}, {}, "true" },
        { "خطأ", TokenType::LITERAL_FALSE, KeywordCategory::RESERVED, true, {}, {}, "false" },
        { "لاشيء", TokenType::LITERAL_NULL, KeywordCategory::RESERVED, true, {}, {}, "null" },
        // ─── operators (3) ───
        { "و", TokenType::OP_AND, KeywordCategory::OPERATOR, true, {}, {}, "and" },
        { "أو", TokenType::OP_OR, KeywordCategory::OPERATOR, true, {}, {}, "or" },
        { "ليس", TokenType::OP_NOT, KeywordCategory::OPERATOR, true, {}, {}, "not" },
        // ─── contextual (40) ───
        { "مؤكد", TokenType::OP_NULL_ASSERT, KeywordCategory::CONTEXTUAL, false, {"مؤكدة"}, {}, "asserted" },
        { "خاصية", TokenType::KEYWORD_PROPERTY, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "property" },
        { "احصل", TokenType::KEYWORD_GET, KeywordCategory::CONTEXTUAL, false, {}, {}, "get" },
        { "عيّن", TokenType::KEYWORD_SET, KeywordCategory::CONTEXTUAL, false, {"عين"}, {}, "set" },
        { "هدم", TokenType::KEYWORD_DESTRUCTOR, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "destructor" },
        { "عامل", TokenType::KEYWORD_OPERATOR, KeywordCategory::CONTEXTUAL, false, {}, {}, "operator" },
        { "رئيسية", TokenType::KEYWORD_MAIN, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "main" },
        { "محكم", TokenType::KEYWORD_SEALED, KeywordCategory::CONTEXTUAL, false, {}, {}, "sealed" },
        { "ترجع", TokenType::KEYWORD_RETURNS, KeywordCategory::CONTEXTUAL, false, {}, {}, "returns" },
        { "حالة", TokenType::KEYWORD_CASE, KeywordCategory::CONTEXTUAL, false, {}, {"inter_block"}, "case" },
        { "غير_متزامن", TokenType::KEYWORD_ASYNC, KeywordCategory::CONTEXTUAL, false, {"غير_متزامنة"}, {}, "async" },
        { "انتظر", TokenType::KEYWORD_AWAIT, KeywordCategory::CONTEXTUAL, false, {}, {}, "await" },
        { "لامدا", TokenType::KEYWORD_LAMBDA, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "lambda" },
        { "أنتج", TokenType::KEYWORD_YIELD, KeywordCategory::CONTEXTUAL, false, {}, {}, "yield" },
        { "مولد", TokenType::KEYWORD_GENERATOR, KeywordCategory::CONTEXTUAL, false, {}, {}, "generator" },
        { "باستخدام", TokenType::KEYWORD_WITH, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "with" },
        { "نهاية_استخدام", TokenType::KEYWORD_END_WITH, KeywordCategory::CONTEXTUAL, false, {}, {"block_closer"}, "end_with" },
        { "قالب", TokenType::KEYWORD_TEMPLATE, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "template" },
        { "نوع", TokenType::KEYWORD_TYPENAME, KeywordCategory::CONTEXTUAL, false, {}, {}, "typename" },
        { "حيث", TokenType::KEYWORD_WHERE, KeywordCategory::CONTEXTUAL, false, {}, {}, "where" },
        { "فضاء", TokenType::KEYWORD_NAMESPACE, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "namespace" },
        { "نهاية_فضاء", TokenType::KEYWORD_END_NAMESPACE, KeywordCategory::CONTEXTUAL, false, {}, {"block_closer"}, "end_namespace" },
        { "سمة", TokenType::KEYWORD_TRAIT, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "trait" },
        { "نفّذ", TokenType::KEYWORD_IMPL, KeywordCategory::CONTEXTUAL, false, {"نفذ"}, {"block_opener"}, "impl" },
        { "امتداد", TokenType::KEYWORD_EXTENSION, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "extension" },
        { "ماكرو", TokenType::KEYWORD_MACRO, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "macro" },
        { "أجّل", TokenType::KEYWORD_DEFER, KeywordCategory::CONTEXTUAL, false, {"اجل", "أجل", "اجّل"}, {}, "defer" },
        { "أطلق", TokenType::KEYWORD_GO, KeywordCategory::CONTEXTUAL, false, {"اطلق"}, {}, "go" },
        { "اختر", TokenType::KEYWORD_SELECT, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "select" },
        { "واجهة", TokenType::KEYWORD_UI_DECL, KeywordCategory::CONTEXTUAL, false, {}, {}, "ui_decl" },
        { "اعرض", TokenType::KEYWORD_SHOW, KeywordCategory::CONTEXTUAL, false, {}, {}, "show" },
        { "متطاير", TokenType::KEYWORD_VOLATILE, KeywordCategory::CONTEXTUAL, false, {}, {}, "volatile" },
        { "اعطِ", TokenType::KEYWORD_GIVE_DEPRECATED, KeywordCategory::CONTEXTUAL, false, {}, {}, "give_deprecated" },
        { "ثم", TokenType::KEYWORD_THEN, KeywordCategory::CONTEXTUAL, false, {}, {}, "then" },
        { "انشر", TokenType::KEYWORD_PROPAGATE, KeywordCategory::CONTEXTUAL, false, {}, {}, "propagate" },
        { "الى", TokenType::KEYWORD_TO, KeywordCategory::CONTEXTUAL, false, {"إلى"}, {}, "to" },
        { "يتطلب", TokenType::KEYWORD_REQUIRES, KeywordCategory::CONTEXTUAL, false, {}, {}, "requires" },
        { "يضمن", TokenType::KEYWORD_ENSURES, KeywordCategory::CONTEXTUAL, false, {}, {}, "ensures" },
        { "عقد", TokenType::KEYWORD_CONTRACT, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "contract" },
        { "اختبر", TokenType::KEYWORD_TEST, KeywordCategory::CONTEXTUAL, false, {}, {"block_opener"}, "test" },
        // ─── builtin_types (9) ───
        { "رقم", TokenType::TYPE_INTEGER, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "integer" },
        { "عشري", TokenType::TYPE_DOUBLE, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "double" },
        { "نص", TokenType::TYPE_STRING, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "string" },
        { "منطقي", TokenType::TYPE_BOOLEAN, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "boolean" },
        { "فراغ", TokenType::TYPE_VOID, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "void" },
        { "عدم", TokenType::TYPE_NULL, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "null" },
        { "مصفوفة", TokenType::TYPE_ARRAY, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "array" },
        { "خريطة", TokenType::TYPE_MAP, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "map" },
        { "أي", TokenType::TYPE_ANY, KeywordCategory::BUILTIN_TYPE, false, {}, {}, "any" },
    };
    return entries;
}

} // namespace Generated
} // namespace Lexer
} // namespace Sad
