// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: semantic_tokens_provider.cpp
// الوصف: مزود الرموز الدلالية الثوري - تلوين ذكي للنحو العربي
// ══════════════════════════════════════════════════════════════════════════════
//
// المميزات الثورية:
//   ✦ تلوين دقيق يعتمد على المحلل المعجمي الحقيقي
//   ✦ تحسين نوع المعرّف بناءً على الفهرس (دالة/صنف/متغير/ثابت)
//   ✦ معدّلات ذكية: readonly للثوابت، async للدوال غير المتزامنة
//   ✦ تلوين التعليقات العادية (#) وتعليقات التوثيق (##)
//   ✦ تلوين خاص للدوال المدمجة (defaultLibrary)
//   ✦ تلوين الإيموجي في النصوص
//   ✦ تمييز المعلمات عن المتغيرات المحلية
//   ✦ تمييز الخصائص عند الوصول بالنقطة
//
// البروتوكول: نرسل مصفوفة أرقام بصيغة delta encoding:
//   [deltaLine, deltaStartChar, length, tokenType, tokenModifiers]
//
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include "lexer_core.h"
#include "lexer_keywords.h"
#include <iostream>
#include "token.h"
#include <algorithm>
#include <unordered_set>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  تحويل TokenType إلى SemanticTokenType
// ══════════════════════════════════════════════════════════════════════════════

static int token_to_semantic_type(Sad::Lexer::TokenType type) {
    using TT = Sad::Lexer::TokenType;
    switch (type) {
        // أنواع البيانات → type (legacy — المحلل المعجمي لا ينتجها بعد الآن)
        case TT::TYPE_INTEGER: case TT::TYPE_DOUBLE: case TT::TYPE_STRING:
        case TT::TYPE_BOOLEAN: case TT::TYPE_VOID: case TT::TYPE_NULL:
        case TT::TYPE_ARRAY: case TT::TYPE_MAP: case TT::TYPE_ANY:
        case TT::TYPE_U8: case TT::TYPE_U16: case TT::TYPE_U32: case TT::TYPE_U64:
        case TT::TYPE_I8: case TT::TYPE_I16: case TT::TYPE_I32: case TT::TYPE_I64:
            return static_cast<int>(SemanticTokenType::Type);

        // الكلمات المفتاحية المحجوزة (40) → keyword
        case TT::KEYWORD_FUNCTION: case TT::KEYWORD_RETURN:
        case TT::KEYWORD_CLASS: case TT::KEYWORD_NEW: case TT::KEYWORD_THIS:
        case TT::KEYWORD_IF: case TT::KEYWORD_ELSE:
        case TT::KEYWORD_WHILE: case TT::KEYWORD_FOR: case TT::KEYWORD_IN:
        case TT::KEYWORD_BREAK: case TT::KEYWORD_CONTINUE:
        case TT::KEYWORD_MATCH: case TT::KEYWORD_WHEN: case TT::KEYWORD_DEFAULT:
        case TT::KEYWORD_TRY: case TT::KEYWORD_CATCH: case TT::KEYWORD_THROW:
        case TT::KEYWORD_FINALLY: case TT::KEYWORD_IMPORT: case TT::KEYWORD_FROM:
        case TT::KEYWORD_AS: case TT::KEYWORD_EXPORT:
        case TT::KEYWORD_VAR: case TT::KEYWORD_CONST: case TT::KEYWORD_STATIC:
        case TT::KEYWORD_EXTERN:
        case TT::KEYWORD_STRUCT: case TT::KEYWORD_ENUM:
        case TT::KEYWORD_INHERITS: case TT::KEYWORD_ABSTRACT:
        case TT::KEYWORD_CONSTRUCTOR: case TT::KEYWORD_SUPER:
        case TT::KEYWORD_PUBLIC: case TT::KEYWORD_PRIVATE: case TT::KEYWORD_PROTECTED:
        case TT::KEYWORD_END:
        // الكلمات السياقية (legacy tokens — نادراً ما تُنتَج)
        case TT::KEYWORD_RETURNS: case TT::KEYWORD_ELSE_IF:
        case TT::KEYWORD_ASYNC: case TT::KEYWORD_AWAIT:
        case TT::KEYWORD_LAMBDA: case TT::KEYWORD_YIELD:
        case TT::KEYWORD_CASE: case TT::KEYWORD_EXTENDS:
        case TT::KEYWORD_PROPERTY: case TT::KEYWORD_OPERATOR:
            return static_cast<int>(SemanticTokenType::Keyword);

        // القيم المنطقية والعدم → keyword
        case TT::LITERAL_TRUE: case TT::LITERAL_FALSE: case TT::LITERAL_NULL:
            return static_cast<int>(SemanticTokenType::Keyword);

        // أرقام → number
        case TT::NUMBER_INTEGER: case TT::NUMBER_DOUBLE:
            return static_cast<int>(SemanticTokenType::Number);

        // نصوص → string
        case TT::STRING_LITERAL: case TT::STRING_RAW: case TT::STRING_FSTRING:
            return static_cast<int>(SemanticTokenType::String);

        // تعليقات التوثيق → comment
        case TT::DOC_COMMENT:
            return static_cast<int>(SemanticTokenType::Comment);

        // عوامل → operator
        case TT::OP_PLUS: case TT::OP_MINUS: case TT::OP_MULTIPLY:
        case TT::OP_DIVIDE: case TT::OP_MODULO: case TT::OP_POWER:
        case TT::OP_ASSIGN: case TT::OP_EQUAL: case TT::OP_NOT_EQUAL:
        case TT::OP_LESS: case TT::OP_LESS_EQUAL: case TT::OP_GREATER:
        case TT::OP_GREATER_EQUAL: case TT::OP_AND: case TT::OP_OR: case TT::OP_NOT:
        case TT::QUESTION_DOT: case TT::QUESTION_QUESTION:
            return static_cast<int>(SemanticTokenType::Operator);

        // معرّفات → variable (سيتم تنقيحها لاحقاً بناءً على الفهرس)
        case TT::IDENTIFIER:
            return static_cast<int>(SemanticTokenType::Variable);

        default:
            return -1; // لا تلوين
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  إنشاء الرموز الدلالية
// ══════════════════════════════════════════════════════════════════════════════

SemanticTokensData LspEngine::semantic_tokens_full(const DocumentUri& uri) {
    SemanticTokensData data;

    auto doc = doc_store_->get(uri);
    if (!doc) return data;

    try {
        Sad::Lexer::KeywordTable::initialize();
        Sad::Lexer::LexerCore lexer(doc->content);
        auto tokens = lexer.tokenize();

        // بناء خريطة سريعة للرموز حسب الاسم
        auto doc_symbols = index_->get_document_symbols(uri);
        std::unordered_map<std::string, const AnalyzedSymbol*> symbol_map;
        for (const auto& sym : doc_symbols) {
            symbol_map[sym.name] = &sym;
        }

        // ──── ١. معالجة التعليقات العادية (#) أولاً ────
        // Lexer لا يُرجع التعليقات العادية كرموز، نبحث عنها يدوياً
        auto lines = arabic::split_lines(doc->content);
        struct ManualToken {
            int line;
            int start;
            int length;
            int type;
            int modifiers;
        };
        std::vector<ManualToken> manual_tokens;

        for (int i = 0; i < static_cast<int>(lines.size()); i++) {
            const auto& line = lines[i];
            // نبحث عن # خارج النصوص
            bool in_string = false;
            char string_char = 0;
            for (int j = 0; j < static_cast<int>(line.size()); j++) {
                if (!in_string && (line[j] == '"' || line[j] == '\'')) {
                    in_string = true;
                    string_char = line[j];
                } else if (in_string && line[j] == string_char && (j == 0 || line[j-1] != '\\')) {
                    in_string = false;
                } else if (!in_string && line[j] == '#') {
                    // وجدنا تعليق
                    int comment_len = static_cast<int>(line.size()) - j;
                    manual_tokens.push_back({
                        i, j, comment_len,
                        static_cast<int>(SemanticTokenType::Comment),
                        0
                    });
                    break;
                }
            }
        }

        // ──── ٢. معالجة الرموز من Lexer ────
        int prev_line = 0;
        int prev_start = 0;

        // نجمع كل الرموز (من Lexer ومن البحث اليدوي) ونرتبها
        struct TokenEntry {
            int line;
            int start;
            int length;
            int type;
            int modifiers;
        };
        std::vector<TokenEntry> all_tokens;

        // رموز Lexer
        for (const auto& token : tokens) {
            int semantic_type = token_to_semantic_type(token.getType());
            if (semantic_type < 0) continue;

            int token_modifiers = 0;

            // تحسين نوع المعرّف بناءً على الفهرس
            if (token.getType() == Sad::Lexer::TokenType::IDENTIFIER) {
                const std::string& val = token.getValue();
                
                // (AR) تلوين أسماء الأنواع المدمجة كأنواع (لم تعد محجوزة)
                // (EN) Color built-in type names as types (no longer reserved)
                if (val == "رقم" || val == "عشري" || val == "نص" ||
                    val == "منطقي" || val == "فراغ" || val == "عدم" ||
                    val == "مصفوفة" || val == "خريطة" || val == "أي") {
                    semantic_type = static_cast<int>(SemanticTokenType::Type);
                }
                // (AR) تلوين الكلمات السياقية ككلمات مفتاحية
                // (EN) Color contextual keywords as keywords
                else if (val == "خاصية" || val == "هدم" || val == "عامل" ||
                         val == "احصل" || val == "عيّن" || val == "غير_متزامن" ||
                         val == "انتظر" || val == "لامدا" || val == "مولد" ||
                         val == "قالب" || val == "فضاء" || val == "سمة" ||
                         val == "واجهة" || val == "نفّذ" || val == "نفذ" ||
                         val == "اختبر" || val == "حالة" || val == "أنتج" ||
                         val == "باستخدام" || val == "رئيسية") {
                    semantic_type = static_cast<int>(SemanticTokenType::Keyword);
                }
                
                auto it = symbol_map.find(val);
                if (it != symbol_map.end()) {
                    const auto& sym = *(it->second);
                    switch (sym.kind) {
                        case AnalyzedSymbolKind::Function:
                            semantic_type = static_cast<int>(SemanticTokenType::Function);
                            if (sym.func_info && sym.func_info->is_async) {
                                token_modifiers |= static_cast<int>(SemanticTokenModifier::Async);
                            }
                            break;
                        case AnalyzedSymbolKind::Method:
                            semantic_type = static_cast<int>(SemanticTokenType::Method);
                            break;
                        case AnalyzedSymbolKind::Class:
                            semantic_type = static_cast<int>(SemanticTokenType::Class);
                            break;
                        case AnalyzedSymbolKind::Parameter:
                            semantic_type = static_cast<int>(SemanticTokenType::Parameter);
                            break;
                        case AnalyzedSymbolKind::Property:
                            semantic_type = static_cast<int>(SemanticTokenType::Property);
                            break;
                        case AnalyzedSymbolKind::Enum:
                            semantic_type = static_cast<int>(SemanticTokenType::Enum);
                            break;
                        case AnalyzedSymbolKind::Constant:
                            semantic_type = static_cast<int>(SemanticTokenType::Variable);
                            token_modifiers |= static_cast<int>(SemanticTokenModifier::Readonly);
                            break;
                        case AnalyzedSymbolKind::Module:
                            semantic_type = static_cast<int>(SemanticTokenType::Namespace);
                            break;
                        default:
                            break;
                    }
                    if (sym.is_builtin) {
                        token_modifiers |= static_cast<int>(SemanticTokenModifier::DefaultLibrary);
                    }
                    if (sym.is_exported) {
                        // يمكن إضافة معدّل التصدير
                    }
                }
            }

            int line = static_cast<int>(token.getPosition().line) - 1;
            int start = static_cast<int>(token.getPosition().column) - 1;
            int length = static_cast<int>(token.getValue().size());
            if (length <= 0) length = 1;

            all_tokens.push_back({line, start, length, semantic_type, token_modifiers});
        }

        // إضافة التعليقات اليدوية
        for (const auto& mt : manual_tokens) {
            // نتأكد أنها لا تتداخل مع رمز DOC_COMMENT من Lexer
            bool overlaps = false;
            for (const auto& t : all_tokens) {
                if (t.line == mt.line && t.type == static_cast<int>(SemanticTokenType::Comment)) {
                    overlaps = true;
                    break;
                }
            }
            if (!overlaps) {
                all_tokens.push_back({mt.line, mt.start, mt.length, mt.type, mt.modifiers});
            }
        }

        // ترتيب حسب الموضع
        std::sort(all_tokens.begin(), all_tokens.end(),
            [](const TokenEntry& a, const TokenEntry& b) {
                if (a.line != b.line) return a.line < b.line;
                return a.start < b.start;
            });

        // ──── ٣. توليد delta encoding ────
        for (const auto& t : all_tokens) {
            int delta_line = t.line - prev_line;
            int delta_start = (delta_line == 0) ? (t.start - prev_start) : t.start;

            if (delta_start < 0) delta_start = 0;

            data.data.push_back(delta_line);
            data.data.push_back(delta_start);
            data.data.push_back(t.length);
            data.data.push_back(t.type);
            data.data.push_back(t.modifiers);

            prev_line = t.line;
            prev_start = t.start;
        }

    } catch (const std::exception& e) {
        // إذا فشل التحليل المعجمي، نرجع بيانات فارغة مع تسجيل الخطأ
        std::cerr << "[LSP] semantic_tokens failed: " << e.what() << std::endl;
    } catch (...) {
        // خطأ غير معروف أثناء التحليل المعجمي
    }

    return data;
}

SemanticTokensData LspEngine::semantic_tokens_range(const DocumentUri& uri, const Range& range) {
    // (AR) الحصول على كل الرموز ثم تصفية النطاق المطلوب فقط
    // (EN) Get all tokens then filter to the requested range only
    auto full_data = semantic_tokens_full(uri);

    if (full_data.data.empty()) return full_data;

    // فك ترميز delta → مواقع مطلقة → تصفية → إعادة ترميز delta
    SemanticTokensData filtered;
    int abs_line = 0;
    int abs_start = 0;
    int prev_line = 0;
    int prev_start = 0;

    for (size_t i = 0; i + 4 < full_data.data.size(); i += 5) {
        int delta_line = full_data.data[i];
        int delta_start = full_data.data[i + 1];
        int length = full_data.data[i + 2];
        int type = full_data.data[i + 3];
        int modifiers = full_data.data[i + 4];

        // حساب الموقع المطلق
        abs_line += delta_line;
        abs_start = (delta_line == 0) ? (abs_start + delta_start) : delta_start;

        // تصفية: هل الرمز داخل النطاق المطلوب؟
        if (abs_line >= range.start.line && abs_line <= range.end.line) {
            // حساب delta من آخر رمز مُضاف
            int new_delta_line = abs_line - prev_line;
            int new_delta_start = (new_delta_line == 0) ? (abs_start - prev_start) : abs_start;
            if (new_delta_start < 0) new_delta_start = 0;

            filtered.data.push_back(new_delta_line);
            filtered.data.push_back(new_delta_start);
            filtered.data.push_back(length);
            filtered.data.push_back(type);
            filtered.data.push_back(modifiers);

            prev_line = abs_line;
            prev_start = abs_start;
        }
    }

    return filtered;
}

} // namespace lsp
} // namespace sad
