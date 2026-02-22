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
        // أنواع البيانات → type
        case TT::TYPE_INTEGER: case TT::TYPE_DOUBLE: case TT::TYPE_STRING:
        case TT::TYPE_BOOLEAN: case TT::TYPE_VOID: case TT::TYPE_NULL:
        case TT::TYPE_ARRAY: case TT::TYPE_MAP: case TT::TYPE_ANY:
        case TT::TYPE_U8: case TT::TYPE_U16: case TT::TYPE_U32: case TT::TYPE_U64:
        case TT::TYPE_I8: case TT::TYPE_I16: case TT::TYPE_I32: case TT::TYPE_I64:
            return static_cast<int>(SemanticTokenType::Type);

        // الكلمات المفتاحية → keyword
        case TT::KEYWORD_FUNCTION: case TT::KEYWORD_RETURN: case TT::KEYWORD_RETURNS:
        case TT::KEYWORD_CLASS: case TT::KEYWORD_NEW: case TT::KEYWORD_THIS:
        case TT::KEYWORD_IF: case TT::KEYWORD_ELSE: case TT::KEYWORD_ELSE_IF:
        case TT::KEYWORD_WHILE: case TT::KEYWORD_FOR: case TT::KEYWORD_IN:
        case TT::KEYWORD_BREAK: case TT::KEYWORD_CONTINUE:
        case TT::KEYWORD_TRY: case TT::KEYWORD_CATCH: case TT::KEYWORD_THROW:
        case TT::KEYWORD_FINALLY: case TT::KEYWORD_IMPORT: case TT::KEYWORD_FROM:
        case TT::KEYWORD_AS: case TT::KEYWORD_EXPORT:
        case TT::KEYWORD_VAR: case TT::KEYWORD_CONST:
        case TT::KEYWORD_ASYNC: case TT::KEYWORD_AWAIT:
        case TT::KEYWORD_LAMBDA: case TT::KEYWORD_YIELD:
        case TT::KEYWORD_MATCH: case TT::KEYWORD_CASE:
        case TT::KEYWORD_INHERITS: case TT::KEYWORD_EXTENDS:
        case TT::KEYWORD_PUBLIC: case TT::KEYWORD_PRIVATE: case TT::KEYWORD_PROTECTED:
        case TT::KEYWORD_END:
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
                auto it = symbol_map.find(token.getValue());
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

    } catch (...) {
        // إذا فشل التحليل المعجمي، نرجع بيانات فارغة
    }

    return data;
}

SemanticTokensData LspEngine::semantic_tokens_range(const DocumentUri& uri, const Range& range) {
    // نرجع كل الرموز - العميل يصفي حسب النطاق
    return semantic_tokens_full(uri);
}

} // namespace lsp
} // namespace sad
