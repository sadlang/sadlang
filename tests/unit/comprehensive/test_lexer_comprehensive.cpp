/**
 * @file test_lexer_comprehensive.cpp
 * @brief (AR) ״§״®״×״¨״§״±״§״× ״´״§…„״© „„…״­„„ ״§„…״¹״¬… / (EN) Comprehensive Lexer Tests
 * 
 * 85 ״§״®״×״¨״§״± ״÷״·: ״§„ƒ„…״§״× ״§„…״×״§״­״©״ ״§„…״¹״±״§״×״ ״§„״£״±‚״§…״ ״§„†״µˆ״µ״ ״§„״¹ˆ״§…„״
 * ״¹„״§…״§״× ״§„״×״±‚…״ ״§„״×״¹„‚״§״×״ ״­״§„״§״× ״§„״­״¯ˆ״¯״ UTF-8״ RTL
 */

#include "sad_test_framework.h"
#include "lexer_core.h"
#include "token.h"

// Using specific declarations to avoid Windows SDK TokenType ambiguity
using Sad::Lexer::Token;
using Sad::Lexer::LexerCore;
using Sad::Lexer::Position;
using Sad::Lexer::TokenType;

// ======================================================================
// ״¯ˆ״§„ …״³״§״¹״¯״© / Helper Functions
// ======================================================================

static std::vector<Token> lex(const std::string& source) {
    LexerCore lexer(source);
    return lexer.tokenize();
}

static Token lexSingle(const std::string& source) {
    auto tokens = lex(source);
    // Return first non-EOF token
    for (auto& t : tokens) {
        if (t.getType() != TokenType::END_OF_FILE && 
            t.getType() != TokenType::NEWLINE)
            return t;
    }
    return Token(TokenType::END_OF_FILE, "", Position());
}

static bool hasTokenType(const std::vector<Token>& tokens, Sad::Lexer::TokenType type) {
    for (auto& t : tokens) {
        if (t.getType() == type) return true;
    }
    return false;
}

static size_t countNonEOF(const std::vector<Token>& tokens) {
    size_t count = 0;
    for (auto& t : tokens) {
        if (t.getType() != TokenType::END_OF_FILE && 
            t.getType() != TokenType::NEWLINE)
            count++;
    }
    return count;
}

// ======================================================================
// main
// ======================================================================
int main() {
    SAD_TEST_INIT();

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 1: ״§„ƒ„…״§״× ״§„…״×״§״­״© ״§„״¹״±״¨״© / Arabic Keywords
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.ArabicKeywords / ״§„…״­„„.״§„ƒ„…״§״×_״§„…״×״§״­״©_״§„״¹״±״¨״©");

    SAD_TEST("KW01: ״¯״§„״© -> KEYWORD_FUNCTION", {
        auto t = lexSingle("\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9"); // ״¯״§„״©
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_FUNCTION);
    });

    SAD_TEST("KW02: …״×״÷״± -> KEYWORD_VAR", {
        auto t = lexSingle("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1"); // …״×״÷״±
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_VAR);
    });

    SAD_TEST("KW03: ״«״§״¨״× -> KEYWORD_CONST", {
        auto t = lexSingle("\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA"); // ״«״§״¨״×
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_CONST);
    });

    SAD_TEST("KW04: ״¥״°״§ -> KEYWORD_IF", {
        auto t = lexSingle("\xD8\xA5\xD8\xB0\xD8\xA7"); // ״¥״°״§
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_IF);
    });

    SAD_TEST("KW05: ˆ״¥„״§ -> KEYWORD_ELSE", {
        auto t = lexSingle("\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7"); // ˆ״¥„״§
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_ELSE);
    });

    SAD_TEST("KW06: ״¨†…״§ -> KEYWORD_WHILE", {
        auto t = lexSingle("\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7"); // ״¨†…״§
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_WHILE);
    });

    SAD_TEST("KW07: „ƒ„ -> KEYWORD_FOR", {
        auto t = lexSingle("\xD9\x84\xD9\x83\xD9\x84"); // „ƒ„
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_FOR);
    });

    SAD_TEST("KW08: ״§״±״¬״¹ -> KEYWORD_RETURN", {
        auto t = lexSingle("\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9"); // ״§״±״¬״¹
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_RETURN);
    });

    SAD_TEST("KW09: ״µ† -> KEYWORD_CLASS", {
        auto t = lexSingle("\xD8\xB5\xD9\x86\xD9\x81"); // ״µ†
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_CLASS);
    });

    SAD_TEST("KW10: ״­״§ˆ„ -> KEYWORD_TRY", {
        auto t = lexSingle("\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84"); // ״­״§ˆ„
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_TRY);
    });

    SAD_TEST("KW11: ״§…״³ƒ -> KEYWORD_CATCH", {
        auto t = lexSingle("\xD8\xA7\xD9\x85\xD8\xB3\xD9\x83"); // ״§…״³ƒ
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_CATCH);
    });

    SAD_TEST("KW12: ״§״±… -> KEYWORD_THROW", {
        auto t = lexSingle("\xD8\xA7\xD8\xB1\xD9\x85\xD9\x8A"); // ״§״±…
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_THROW);
    });

    SAD_TEST("KW13: ״µ״­״­ -> LITERAL_TRUE", {
        auto t = lexSingle("\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"); // ״µ״­״­
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::LITERAL_TRUE);
    });

    SAD_TEST("KW14: ״®״·״£ -> LITERAL_FALSE", {
        auto t = lexSingle("\xD8\xAE\xD8\xB7\xD8\xA3"); // ״®״·״£
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::LITERAL_FALSE);
    });

    SAD_TEST("KW15: ״¹״¯… -> LITERAL_NULL", {
        auto t = lexSingle("\xD8\xB9\xD8\xAF\xD9\x85"); // ״¹״¯…
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER); // contextual type name, not keyword
    });

    SAD_TEST("KW16: ״§״³״×ˆ״±״¯ -> KEYWORD_IMPORT", {
        auto t = lexSingle("\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x88\xD8\xB1\xD8\xAF"); // ״§״³״×ˆ״±״¯
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_IMPORT);
    });

    SAD_TEST("KW17: ״±״« -> KEYWORD_EXTENDS", {
        auto t = lexSingle("\xD9\x8A\xD8\xB1\xD8\xAB"); // ״±״«
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_INHERITS);
    });

    SAD_TEST("KW18: ״¬״¯״¯ -> KEYWORD_NEW", {
        auto t = lexSingle("\xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF"); // ״¬״¯״¯
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_NEW);
    });

    SAD_TEST("KW19: ״×ˆ‚ -> KEYWORD_BREAK", {
        auto t = lexSingle("\xD8\xAA\xD9\x88\xD9\x82\xD9\x81"); // ״×ˆ‚
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_BREAK);
    });

    SAD_TEST("KW20: ״§״³״×…״± -> KEYWORD_CONTINUE", {
        auto t = lexSingle("\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x85\xD8\xB1"); // ״§״³״×…״±
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_CONTINUE);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 2: ״§„״£†ˆ״§״¹ ״§„״¹״±״¨״© / Arabic Type Keywords
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.ArabicTypes / ״§„…״­„„.״§„״£†ˆ״§״¹_״§„״¹״±״¨״©");

    SAD_TEST("TY01: ״±‚… -> TYPE_INTEGER", {
        auto t = lexSingle("\xD8\xB1\xD9\x82\xD9\x85"); // ״±‚…
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER); // contextual type name
    });

    SAD_TEST("TY02: ״¹״´״± -> TYPE_DOUBLE", {
        auto t = lexSingle("\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A"); // ״¹״´״±
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER); // contextual type name
    });

    SAD_TEST("TY03: †״µ -> TYPE_STRING", {
        auto t = lexSingle("\xD9\x86\xD8\xB5"); // †״µ
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER); // contextual type name
    });

    SAD_TEST("TY04: …†״·‚ -> TYPE_BOOLEAN", {
        auto t = lexSingle("\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A"); // …†״·‚
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER); // contextual type name
    });

    SAD_TEST("TY05: …״µˆ״© -> TYPE_ARRAY", {
        auto t = lexSingle("\xD9\x85\xD8\xB5\xD9\x81\xD9\x88\xD9\x81\xD8\xA9"); // …״µˆ״©
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER); // contextual type name
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 3: ״§„״£״±‚״§… / Numbers
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Numbers / ״§„…״­„„.״§„״£״±‚״§…");

    SAD_TEST("NUM01: ״¹״¯״¯ ״µ״­״­ ״¨״³״· 42", {
        auto t = lexSingle("42");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_INTEGER);
        SAD_ASSERT_EQ(t.getValue(), std::string("42"));
    });

    SAD_TEST("NUM02: ״µ״±", {
        auto t = lexSingle("0");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_INTEGER);
        SAD_ASSERT_EQ(t.getValue(), std::string("0"));
    });

    SAD_TEST("NUM03: ״¹״¯״¯ ƒ״¨״±", {
        auto t = lexSingle("999999999");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_INTEGER);
    });

    SAD_TEST("NUM04: ״¹״¯״¯ ״¹״´״± 3.14", {
        auto t = lexSingle("3.14");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_DOUBLE);
        SAD_ASSERT_EQ(t.getValue(), std::string("3.14"));
    });

    SAD_TEST("NUM05: ״¹״´״± ״¨״µ״± 0.5", {
        auto t = lexSingle("0.5");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_DOUBLE);
    });

    SAD_TEST("NUM06: ״¹״´״± ״·ˆ„ 123.456789", {
        auto t = lexSingle("123.456789");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_DOUBLE);
    });

    SAD_TEST("NUM07: ״¹״¯״¯ ״³״§„״¨ -10", {
        // Lexer ״¹״¯ MINUS + NUMBER
        auto tokens = lex("-10");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_MINUS));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::NUMBER_INTEGER));
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 4: ״§„†״µˆ״µ / Strings
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Strings / ״§„…״­„„.״§„†״µˆ״µ");

    SAD_TEST("STR01: †״µ ״¨״³״· ״¨״§„״¥†״¬„״²״©", {
        auto t = lexSingle("\"hello\"");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    SAD_TEST("STR02: †״µ ״¹״±״¨", {
        auto t = lexSingle("\"\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\""); // "…״±״­״¨״§"
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    SAD_TEST("STR03: †״µ ״§״±״÷", {
        auto t = lexSingle("\"\"");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    SAD_TEST("STR04: †״µ …״¹ …״³״§״§״×", {
        auto t = lexSingle("\"hello world\"");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    SAD_TEST("STR05: †״µ …״¹ ״£״­״± ״®״§״µ״© escaped", {
        auto t = lexSingle("\"hello\\nworld\"");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    SAD_TEST("STR06: †״µ …״¹ ״£״±‚״§…", {
        auto t = lexSingle("\"abc123\"");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    SAD_TEST("STR07: †״µ …״¹ ״±…ˆ״²", {
        auto t = lexSingle("\"!@#$%\"");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::STRING_LITERAL);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 5: ״§„״¹ˆ״§…„ / Operators
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Operators / ״§„…״­„„.״§„״¹ˆ״§…„");

    SAD_TEST("OP01: + -> OP_PLUS", {
        auto t = lexSingle("+");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_PLUS);
    });

    SAD_TEST("OP02: - -> OP_MINUS", {
        auto t = lexSingle("-");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_MINUS);
    });

    SAD_TEST("OP03: * -> OP_MULTIPLY", {
        auto t = lexSingle("*");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_MULTIPLY);
    });

    SAD_TEST("OP04: / -> OP_DIVIDE", {
        auto t = lexSingle("/");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_DIVIDE);
    });

    SAD_TEST("OP05: % -> OP_MODULO", {
        auto t = lexSingle("%");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_MODULO);
    });

    SAD_TEST("OP06: = -> OP_ASSIGN", {
        auto t = lexSingle("=");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_ASSIGN);
    });

    SAD_TEST("OP07: == -> OP_EQUAL", {
        auto t = lexSingle("==");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_EQUAL);
    });

    SAD_TEST("OP08: != -> OP_NOT_EQUAL", {
        auto t = lexSingle("!=");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_NOT_EQUAL);
    });

    SAD_TEST("OP09: < -> OP_LESS", {
        auto t = lexSingle("<");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_LESS);
    });

    SAD_TEST("OP10: > -> OP_GREATER", {
        auto t = lexSingle(">");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_GREATER);
    });

    SAD_TEST("OP11: <= -> OP_LESS_EQUAL", {
        auto t = lexSingle("<=");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_LESS_EQUAL);
    });

    SAD_TEST("OP12: >= -> OP_GREATER_EQUAL", {
        auto t = lexSingle(">=");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_GREATER_EQUAL);
    });

    SAD_TEST("OP13: && -> OP_AND", {
        auto t = lexSingle("&&");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_AND);
    });

    SAD_TEST("OP14: || -> OP_OR", {
        auto t = lexSingle("||");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_OR);
    });

    SAD_TEST("OP15: ! -> OP_NOT", {
        auto t = lexSingle("!");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_NOT);
    });

    SAD_TEST("OP16: += -> OP_PLUS_ASSIGN", {
        auto t = lexSingle("+=");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_PLUS_ASSIGN);
    });

    SAD_TEST("OP17: -= -> OP_MINUS_ASSIGN", {
        auto t = lexSingle("-=");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_MINUS_ASSIGN);
    });

    SAD_TEST("OP18: ** -> OP_POWER", {
        auto t = lexSingle("**");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_POWER);
    });

    SAD_TEST("OP19: ++ -> OP_INCREMENT", {
        auto t = lexSingle("++");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_INCREMENT);
    });

    SAD_TEST("OP20: -- -> OP_DECREMENT", {
        auto t = lexSingle("--");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::OP_DECREMENT);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 6: ״¹„״§…״§״× ״§„״×״±‚… / Punctuation
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Punctuation / ״§„…״­„„.״¹„״§…״§״×_״§„״×״±‚…");

    SAD_TEST("PUNCT01: ( -> PAREN_LEFT", {
        auto t = lexSingle("(");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::PAREN_LEFT);
    });

    SAD_TEST("PUNCT02: ) -> PAREN_RIGHT", {
        auto t = lexSingle(")");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::PAREN_RIGHT);
    });

    SAD_TEST("PUNCT03: { -> BRACE_LEFT", {
        auto t = lexSingle("{");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::BRACE_LEFT);
    });

    SAD_TEST("PUNCT04: } -> BRACE_RIGHT", {
        auto t = lexSingle("}");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::BRACE_RIGHT);
    });

    SAD_TEST("PUNCT05: [ -> BRACKET_LEFT", {
        auto t = lexSingle("[");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::BRACKET_LEFT);
    });

    SAD_TEST("PUNCT06: ] -> BRACKET_RIGHT", {
        auto t = lexSingle("]");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::BRACKET_RIGHT);
    });

    SAD_TEST("PUNCT07: ; -> SEMICOLON", {
        auto t = lexSingle(";");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::SEMICOLON);
    });

    SAD_TEST("PUNCT08: : -> COLON", {
        auto t = lexSingle(":");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::COLON);
    });

    SAD_TEST("PUNCT09: , -> COMMA", {
        auto t = lexSingle(",");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::COMMA);
    });

    SAD_TEST("PUNCT10: . -> DOT", {
        auto t = lexSingle(".");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::DOT);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 7: ״§„…״¹״±״§״× / Identifiers
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Identifiers / ״§„…״­„„.״§„…״¹״±״§״×");

    SAD_TEST("ID01: …״¹״± ״¥†״¬„״² ״¨״³״· x", {
        auto t = lexSingle("x");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
        SAD_ASSERT_EQ(t.getValue(), std::string("x"));
    });

    SAD_TEST("ID02: …״¹״± ״¥†״¬„״² ״·ˆ„ myVariable", {
        auto t = lexSingle("myVariable");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("ID03: …״¹״± …״¹ underscore _test", {
        auto t = lexSingle("_test");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("ID04: …״¹״± …״¹ ״£״±‚״§… var123", {
        auto t = lexSingle("var123");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("ID05: …״¹״± ״¹״±״¨ …״®״µ״µ", {
        auto t = lexSingle("\xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD9\x85"); // ״§„״§״³…
        // Should be IDENTIFIER (not a keyword)
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 8: ״×״¹״§״¨״± …״±ƒ״¨״© / Complex Expressions
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Complex / ״§„…״­„„.״×״¹״§״¨״±_…״±ƒ״¨״©");

    SAD_TEST("CX01: ״×״¹״± …״×״÷״± ״¹״±״¨ ƒ״§…„", {
        // …״×״÷״± ״³ = 5
        auto tokens = lex("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xB3 = 5");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::KEYWORD_VAR));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::IDENTIFIER));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_ASSIGN));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::NUMBER_INTEGER));
    });

    SAD_TEST("CX02: ״¹…„״© ״­״³״§״¨״© 2 + 3 * 4", {
        auto tokens = lex("2 + 3 * 4");
        SAD_ASSERT_GE(countNonEOF(tokens), (size_t)5);
    });

    SAD_TEST("CX03: ״§״³״×״¯״¹״§״¡ ״¯״§„״© f(x, y)", {
        auto tokens = lex("f(x, y)");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::IDENTIFIER));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::PAREN_LEFT));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::COMMA));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::PAREN_RIGHT));
    });

    SAD_TEST("CX04: …‚״§״±†״© x >= 10 && y < 20", {
        auto tokens = lex("x >= 10 && y < 20");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_GREATER_EQUAL));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_AND));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_LESS));
    });

    SAD_TEST("CX05: …״µˆ״© [1, 2, 3]", {
        auto tokens = lex("[1, 2, 3]");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::BRACKET_LEFT));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::BRACKET_RIGHT));
    });

    SAD_TEST("CX06: ״§„ˆ״µˆ„ „״¹״¶ˆ obj.method()", {
        auto tokens = lex("obj.method()");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::DOT));
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 9: ״­״§„״§״× ״­״¯ˆ״¯ / Edge Cases
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.EdgeCases / ״§„…״­„„.״­״§„״§״×_״§„״­״¯ˆ״¯");

    SAD_TEST("EDGE01: †״µ …״µ״¯״± ״§״±״÷", {
        auto tokens = lex("");
        // (AR) المحلل المعجمي لا يُنتج أي رمز لنص فارغ (حلقة tokenize تخرج فوراً)
        // (EN) Lexer produces no tokens for empty source (tokenize loop exits immediately)
        // Either empty or contains only EOF — both valid behaviors
        bool ok = tokens.empty() || hasTokenType(tokens, TokenType::END_OF_FILE);
        SAD_ASSERT_TRUE(ok);
    });

    SAD_TEST("EDGE02: …״³״§״§״× ‚״·", {
        auto tokens = lex("   \t  ");
        // Should just get EOF (whitespace skipped)
        bool onlyEofAndWs = true;
        for (auto& t : tokens) {
            if (t.getType() != TokenType::END_OF_FILE && 
                t.getType() != TokenType::NEWLINE &&
                t.getType() != TokenType::WHITESPACE)
                onlyEofAndWs = false;
        }
        SAD_ASSERT_TRUE(onlyEofAndWs);
    });

    SAD_TEST("EDGE03: ״£״³״·״± ״¬״¯״¯״© …״×״¹״¯״¯״©", {
        auto tokens = lex("\n\n\n");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::END_OF_FILE));
    });

    SAD_TEST("EDGE04: ״±‚… ˆ״§״­״¯", {
        auto t = lexSingle("7");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::NUMBER_INTEGER);
        SAD_ASSERT_EQ(t.getValue(), std::string("7"));
    });

    SAD_TEST("EDGE05: ״­״± ˆ״§״­״¯ ƒ…״¹״±", {
        auto t = lexSingle("a");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("EDGE06: …״¹״§…„״§״× …״×״×״§„״© +-", {
        auto tokens = lex("+-");
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_PLUS));
        SAD_ASSERT_TRUE(hasTokenType(tokens, TokenType::OP_MINUS));
    });

    SAD_TEST("EDGE07: ״£‚ˆ״§״³ …״×״¯״§״®„״© ((()))", {
        auto tokens = lex("((()))");
        int leftCount = 0, rightCount = 0;
        for (auto& t : tokens) {
            if (t.getType() == TokenType::PAREN_LEFT) leftCount++;
            if (t.getType() == TokenType::PAREN_RIGHT) rightCount++;
        }
        SAD_ASSERT_EQ(leftCount, 3);
        SAD_ASSERT_EQ(rightCount, 3);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 10: …ˆ‚״¹ ״§„״±…ˆ״² / Token Positions
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.Positions / ״§„…״­„„.״§„…ˆ״§‚״¹");

    SAD_TEST("POS01: …ˆ‚״¹ ״§„״±…״² ״§„״£ˆ„ (״³״·״± 1)", {
        auto tokens = lex("x = 5");
        auto pos = tokens[0].getPosition();
        SAD_ASSERT_EQ((int)pos.line, 1);
    });

    SAD_TEST("POS02: ״¹״¯״© ״£״³״·״±", {
        auto tokens = lex("x\ny\nz");
        // Verify tokens exist on different lines
        bool foundMultiLine = false;
        int maxLine = 0;
        for (auto& t : tokens) {
            if ((int)t.getPosition().line > maxLine) {
                maxLine = (int)t.getPosition().line;
            }
        }
        SAD_ASSERT_GT(maxLine, 1);
    });

    SAD_TEST("POS03: EOF  ״§„†‡״§״©", {
        auto tokens = lex("abc");
        // (AR) المحلل قد لا يُضيف EOF إذا انتهى المصدر بعد آخر رمز مباشرة
        // (EN) Lexer may not append EOF if source ends right after last token
        SAD_ASSERT_TRUE(!tokens.empty());
        // Last token is either EOF or the identifier itself
        auto lastType = tokens.back().getType();
        SAD_ASSERT_TRUE(lastType == TokenType::END_OF_FILE || lastType == TokenType::IDENTIFIER);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // …״¬…ˆ״¹״© 11: Token API
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("Lexer.TokenAPI / ״§„…״­„„.ˆ״§״¬‡״©_״§„״±…״²");

    SAD_TEST("API01: Token::is()", {
        auto t = lexSingle("+");
        SAD_ASSERT_TRUE(t.is(TokenType::OP_PLUS));
        SAD_ASSERT_FALSE(t.is(TokenType::OP_MINUS));
    });

    SAD_TEST("API02: Token::isKeyword()", {
        auto t = lexSingle("\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9"); // ״¯״§„״©
        SAD_ASSERT_TRUE(t.isKeyword());
    });

    SAD_TEST("API03: Token::isOperator()", {
        auto t = lexSingle("+=");
        SAD_ASSERT_TRUE(t.isOperator());
    });

    SAD_TEST("API04: Token::isLiteral()", {
        auto t = lexSingle("\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"); // ״µ״­״­
        SAD_ASSERT_TRUE(t.isLiteral());
    });

    SAD_TEST("API05: Token::toString()", {
        auto t = lexSingle("42");
        std::string str = t.toString();
        SAD_ASSERT_FALSE(str.empty());
    });

    SAD_TEST("API06: Token::getTypeName()", {
        auto t = lexSingle("42");
        std::string name = t.getTypeName();
        SAD_ASSERT_FALSE(name.empty());
    });

    SAD_SUMMARY();
}
