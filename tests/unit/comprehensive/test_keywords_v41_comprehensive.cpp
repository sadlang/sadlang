// =============================================================================
// (AR) …„: tests/comprehensive/test_keywords_v41_comprehensive.cpp
// (AR) ״§„ˆ״µ: ״§״®״×״¨״§״±״§״× ״´״§…„״© „״¨†״© v4.1 ג€” YAML SoT „„ƒ„…״§״× ״§„…״×״§״­״©
// (EN) Comprehensive tests for v4.1 ג€” Keywords YAML SoT architecture
// =============================================================================
//
// (AR) ״£״±״¨״¹ …״¬…ˆ״¹״§״× ״§״®״×״¨״§״± ״±״¦״³״©:
//   G1 (Snapshot):   ״×״·״§״¨‚ Generated::allEntries() ג†” KeywordTable
//                    ג€” ״§„״¹״¯״¯״ ״§„״¦״§״×״ ״§„״£״¯ˆ״§״±״ ״§„״¬ˆ„״© ״§„ƒ״§…„״© „€ TokenType
//   G_REAL (Real):   „ƒ״³״© ״´״±״© ״­‚‚״© ״×״³״×״®״¯… ״§„ƒ„…״§״× ״§„״³״§‚״© ƒ…״¹״±‘״§״×
//                    ˆƒ„…״§״×  ״³״§‚‡״§ ״§„״µ״­״­
//   G_UNICODE:       ƒ„ primaryWord + alias ״×״¹״±‘ ״¹„‡ ˆ״±״¬״¹ ״§„״¦״© †״³‡״§
//   G_CONFLICT:      „״§ ״×״µ״§״¯… ג€” „״§ alias ״·״§״¨‚ primaryWord …״®״×„״§‹״
//                    ˆ״§„ƒ„…״§״× ״§„״³״§‚״© ״×״µ״¯״± IDENTIFIER („״§ KEYWORD_*)
//
// (AR) ״§„״¥״·״§״±: sad_test_framework.h (†״³ ״§„†…״· ״§„…״×״¨״¹)
// (EN) Framework: sad_test_framework.h (consistent with existing tests)
// =============================================================================

#include "sad_test_framework.h"
#include "lexer_core.h"
#include "lexer_keywords.h"
#include "keywords_generated.h"
#include "token.h"

#include <set>
#include <string>
#include <vector>

using Sad::Lexer::LexerCore;
using Sad::Lexer::Token;
using Sad::Lexer::TokenType;
using Sad::Lexer::Position;
using Sad::Lexer::KeywordTable;
using Sad::Lexer::Generated::allEntries;
using Sad::Lexer::Generated::KeywordCategory;
using Sad::Lexer::Generated::KeywordEntry;
using Sad::Lexer::Generated::kEntryCount;

// ============================================================================
// (AR) ״¯ˆ״§„ …״³״§״¹״¯״© ג€” „ƒ״³״© ƒ„…״© ˆ״§״­״¯״© ˆ״¥״±״¬״§״¹ †ˆ״¹‡״§
// (EN) Helpers ג€” lex a single word and return its TokenType
// ============================================================================

/**
 * @brief (AR) „ƒ‘״³ ״§„†״µ ˆ״±״¬״¹ ״£ˆ„ ״±…״² ״¹„ (״×״¬״§ˆ״² NEWLINE/EOF).
 * @brief (EN) Tokenize and return the first non-trivial token.
 */
static Token lexFirstToken(const std::string& source) {
    LexerCore lexer(source);
    auto tokens = lexer.tokenize();
    for (auto& t : tokens) {
        if (t.getType() != TokenType::END_OF_FILE &&
            t.getType() != TokenType::NEWLINE) {
            return t;
        }
    }
    return Token(TokenType::END_OF_FILE, "", Position());
}

/**
 * @brief (AR) „ƒ‘״³ ˆ״±״¬״¹ ״¬…״¹ ״§„״±…ˆ״² ״§„״¹„״© (״¨״¯ˆ† NEWLINE/EOF).
 * @brief (EN) Tokenize and return all non-trivial tokens.
 */
static std::vector<Token> lexAll(const std::string& source) {
    LexerCore lexer(source);
    auto raw = lexer.tokenize();
    std::vector<Token> out;
    out.reserve(raw.size());
    for (auto& t : raw) {
        if (t.getType() != TokenType::END_OF_FILE &&
            t.getType() != TokenType::NEWLINE) {
            out.push_back(t);
        }
    }
    return out;
}

// ============================================================================
// main
// ============================================================================
int main() {
    SAD_TEST_INIT();
    KeywordTable::initialize();

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) G1 ג€” Snapshot: ״×״·״§״¨‚ ״§„…״¹״¬… ״§„…ˆ„‘״¯ …״¹ KeywordTable
    // (EN) G1 ג€” Snapshot: Generated lexicon ג†” KeywordTable consistency
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("KeywordsV41.G1_Snapshot");

    SAD_TEST("G1.01: kEntryCount ״·״§״¨‚ ״­״¬… allEntries()", {
        SAD_ASSERT_EQ(allEntries().size(), kEntryCount);
    });

    SAD_TEST("G1.02: ״§„״¹״¯״¯ ״§„״¥״¬…״§„ 93 ״¥״¯״®״§„״§‹ (40+3+41+9)", {
        SAD_ASSERT_EQ(kEntryCount, static_cast<std::size_t>(93));
    });

    SAD_TEST("G1.03: ״¹״¯״¯ ״§„״¦״© RESERVED = 40", {
        size_t n = 0;
        for (const auto& e : allEntries())
            if (e.category == KeywordCategory::RESERVED) ++n;
        SAD_ASSERT_EQ(n, static_cast<std::size_t>(40));
    });

    SAD_TEST("G1.04: ״¹״¯״¯ ״§„״¦״© OPERATOR = 3 (ˆ/״£ˆ/„״³)", {
        size_t n = 0;
        for (const auto& e : allEntries())
            if (e.category == KeywordCategory::OPERATOR) ++n;
        SAD_ASSERT_EQ(n, static_cast<std::size_t>(3));
    });

    SAD_TEST("G1.05: ״¹״¯״¯ ״§„״¦״© CONTEXTUAL >= 35", {
        size_t n = 0;
        for (const auto& e : allEntries())
            if (e.category == KeywordCategory::CONTEXTUAL) ++n;
        SAD_ASSERT_GE(n, static_cast<std::size_t>(35));
    });

    SAD_TEST("G1.06: ״¹״¯״¯ ״§„״¦״© BUILTIN_TYPE >= 8", {
        size_t n = 0;
        for (const auto& e : allEntries())
            if (e.category == KeywordCategory::BUILTIN_TYPE) ++n;
        SAD_ASSERT_GE(n, static_cast<std::size_t>(8));
    });

    SAD_TEST("G1.07: ƒ„ RESERVED/OPERATOR „‡״§ emittedByLexer=true", {
        for (const auto& e : allEntries()) {
            if (e.category == KeywordCategory::RESERVED ||
                e.category == KeywordCategory::OPERATOR) {
                SAD_ASSERT_TRUE(e.emittedByLexer);
            }
        }
    });

    SAD_TEST("G1.08: ƒ„ CONTEXTUAL/BUILTIN_TYPE „‡״§ emittedByLexer=false", {
        for (const auto& e : allEntries()) {
            if (e.category == KeywordCategory::CONTEXTUAL ||
                e.category == KeywordCategory::BUILTIN_TYPE) {
                SAD_ASSERT_FALSE(e.emittedByLexer);
            }
        }
    });

    SAD_TEST("G1.09: ƒ„ ״§„…״µ״¯״±״© round-trip typeג†’wordג†’type", {
        for (const auto& e : allEntries()) {
            if (!e.emittedByLexer) continue;
            const std::string w = KeywordTable::getKeyword(e.type);
            SAD_ASSERT_EQ(w, e.primaryWord);
            SAD_ASSERT_TRUE(KeywordTable::isKeyword(w));
            SAD_ASSERT_EQ((int)KeywordTable::getKeywordType(w), (int)e.type);
        }
    });

    SAD_TEST("G1.10: KeywordTable::getEntry(type) ״±״¬״¹ †״³ ״§„״¥״¯״®״§„", {
        for (const auto& e : allEntries()) {
            const KeywordEntry* found = KeywordTable::getEntry(e.type);
            SAD_ASSERT_NOT_NULL(found);
            SAD_ASSERT_EQ(found->primaryWord, e.primaryWord);
        }
    });

    SAD_TEST("G1.11: getKeywordsByCategory ״·״§״¨‚ ״§„״¹״¯ ״§„…״¨״§״´״±", {
        for (auto cat : { KeywordCategory::RESERVED,
                          KeywordCategory::OPERATOR,
                          KeywordCategory::CONTEXTUAL,
                          KeywordCategory::BUILTIN_TYPE }) {
            size_t direct = 0;
            for (const auto& e : allEntries())
                if (e.category == cat) ++direct;
            const auto& list = KeywordTable::getKeywordsByCategory(cat);
            SAD_ASSERT_EQ(list.size(), direct);
        }
    });

    SAD_TEST("G1.12: ״£״¯ˆ״§״± block_opener/block_closer ״÷״± ״§״±״÷״©", {
        SAD_ASSERT_GT(KeywordTable::getKeywordsByRole("block_opener").size(),
                      static_cast<std::size_t>(0));
        SAD_ASSERT_GT(KeywordTable::getKeywordsByRole("block_closer").size(),
                      static_cast<std::size_t>(0));
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) G_REAL ג€” „ƒ״³״© ״´״±״© ״­‚‚״© ״×״³״×״®״¯… ״§„ƒ„…״§״× ״§„״³״§‚״© ƒ…״¹״±‘״§״×
    // (EN) G_REAL ג€” Lex real code that uses contextual words as identifiers
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("KeywordsV41.G_REAL");

    SAD_TEST("REAL.01: primaryWord(KEYWORD_FUNCTION) -> KEYWORD_FUNCTION", {
        // (AR) نحصل على النص من KeywordTable بدلاً من hardcode
        const KeywordEntry* e = KeywordTable::getEntry(TokenType::KEYWORD_FUNCTION);
        SAD_ASSERT_NOT_NULL(e);
        auto t = lexFirstToken(e->primaryWord);
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_FUNCTION);
    });

    SAD_TEST("REAL.02: primaryWord(KEYWORD_VAR) -> KEYWORD_VAR", {
        const KeywordEntry* e = KeywordTable::getEntry(TokenType::KEYWORD_VAR);
        SAD_ASSERT_NOT_NULL(e);
        auto t = lexFirstToken(e->primaryWord);
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::KEYWORD_VAR);
    });

    SAD_TEST("REAL.03: '„״§…״¯״§' (״³״§‚״©) ״×״µ״¯״± IDENTIFIER", {
        auto t = lexFirstToken("„״§…״¯״§");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
        SAD_ASSERT_EQ(t.getValue(), std::string("„״§…״¯״§"));
    });

    SAD_TEST("REAL.04: '״®״§״µ״©' (״³״§‚״©) ״×״µ״¯״± IDENTIFIER", {
        auto t = lexFirstToken("״®״§״µ״©");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("REAL.05: '‡״¯…' (״³״§‚״©) ״×״µ״¯״± IDENTIFIER", {
        auto t = lexFirstToken("‡״¯…");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("REAL.06: '״±‚…' (†ˆ״¹ …״¯…״¬) ״×״µ״¯״± IDENTIFIER", {
        auto t = lexFirstToken("״±‚…");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("REAL.07: '†״µ' (†ˆ״¹ …״¯…״¬) ״×״µ״¯״± IDENTIFIER", {
        auto t = lexFirstToken("†״µ");
        SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
    });

    SAD_TEST("REAL.08: VAR + CONTEXTUAL_AS_IDENT = 5 (data-driven)", {
        // (AR) نبني السلسلة من KeywordTable دون hardcode عربي
        const KeywordEntry* varE = KeywordTable::getEntry(TokenType::KEYWORD_VAR);
        const KeywordEntry* lamE = KeywordTable::getEntry(TokenType::KEYWORD_LAMBDA);
        SAD_ASSERT_NOT_NULL(varE);
        SAD_ASSERT_NOT_NULL(lamE);
        std::string src = varE->primaryWord + " " + lamE->primaryWord + " = 5";
        auto toks = lexAll(src);
        SAD_ASSERT_EQ(toks.size(), static_cast<std::size_t>(4));
        SAD_ASSERT_EQ((int)toks[0].getType(), (int)TokenType::KEYWORD_VAR);
        SAD_ASSERT_EQ((int)toks[1].getType(), (int)TokenType::IDENTIFIER);
        SAD_ASSERT_EQ(toks[1].getValue(), lamE->primaryWord);
    });

    SAD_TEST("REAL.09: VAR + BUILTIN_TYPE_AS_IDENT = 42 (data-driven)", {
        // (AR) نختبر أول نوع مدمج كمتغير عادي
        const KeywordEntry* varE = KeywordTable::getEntry(TokenType::KEYWORD_VAR);
        SAD_ASSERT_NOT_NULL(varE);
        const KeywordEntry* btE = nullptr;
        for (const auto& e : allEntries()) {
            if (e.category == KeywordCategory::BUILTIN_TYPE) { btE = &e; break; }
        }
        SAD_ASSERT_NOT_NULL(btE);
        std::string src = varE->primaryWord + " " + btE->primaryWord + " = 42";
        auto toks = lexAll(src);
        SAD_ASSERT_EQ(toks.size(), static_cast<std::size_t>(4));
        SAD_ASSERT_EQ((int)toks[1].getType(), (int)TokenType::IDENTIFIER);
        SAD_ASSERT_EQ(toks[1].getValue(), btE->primaryWord);
    });

    SAD_TEST("REAL.10: first OPERATOR entry is emittedByLexer (data-driven)", {
        const KeywordEntry* opE = nullptr;
        for (const auto& e : allEntries()) {
            if (e.category == KeywordCategory::OPERATOR) { opE = &e; break; }
        }
        SAD_ASSERT_NOT_NULL(opE);
        SAD_ASSERT_TRUE(opE->emittedByLexer);
        auto t = lexFirstToken(opE->primaryWord);
        SAD_ASSERT_EQ((int)t.getType(), (int)opE->type);
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) G_UNICODE ג€” primaryWord + ƒ„ alias ״×״¹״±‘ ״¹„‡ ״¨†״³ ״§„†ˆ״¹/״§„״¦״©
    // (EN) G_UNICODE ג€” primaryWord + every alias resolves to same entry
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("KeywordsV41.G_UNICODE");

    SAD_TEST("U.01: ƒ„ primaryWord …ˆ״¬ˆ״¯  wordIndex", {
        for (const auto& e : allEntries()) {
            const KeywordEntry* found = KeywordTable::getEntry(e.primaryWord);
            SAD_ASSERT_NOT_NULL(found);
            SAD_ASSERT_EQ((int)found->type, (int)e.type);
        }
    });

    SAD_TEST("U.02: ƒ„ alias ״±״¬״¹ †״³ ״§„״¥״¯״®״§„ ƒ€ primaryWord", {
        for (const auto& e : allEntries()) {
            for (const auto& a : e.aliases) {
                const KeywordEntry* found = KeywordTable::getEntry(a);
                SAD_ASSERT_NOT_NULL(found);
                SAD_ASSERT_EQ((int)found->type, (int)e.type);
                SAD_ASSERT_EQ(found->primaryWord, e.primaryWord);
            }
        }
    });

    SAD_TEST("U.03: alias „ƒ„…״© …״µ״¯״±״© ״×״¹…„  Lexer (†״³ ״§„†ˆ״¹)", {
        // (AR) †״®״×״¨״± ״¹†״§״× …״¹״±ˆ״©
        for (const auto& e : allEntries()) {
            if (!e.emittedByLexer) continue;
            for (const auto& a : e.aliases) {
                auto t = lexFirstToken(a);
                SAD_ASSERT_EQ((int)t.getType(), (int)e.type);
            }
        }
    });

    SAD_TEST("U.04: alias „ƒ„…״© ״³״§‚״© ״×״¸„ IDENTIFIER  Lexer", {
        for (const auto& e : allEntries()) {
            if (e.category != KeywordCategory::CONTEXTUAL) continue;
            for (const auto& a : e.aliases) {
                auto t = lexFirstToken(a);
                SAD_ASSERT_EQ((int)t.getType(), (int)TokenType::IDENTIFIER);
            }
        }
    });

    SAD_TEST("U.05: getCategory „„ƒ„…״© ״§„״±״¦״³״© ״·״§״¨‚ ״¦״×‡״§", {
        for (const auto& e : allEntries()) {
            auto cat = KeywordTable::getCategory(e.primaryWord);
            SAD_ASSERT_TRUE(cat.has_value());
            SAD_ASSERT_EQ((int)*cat, (int)e.category);
        }
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) G_CONFLICT ג€” „״§ ״×״µ״§״¯… ״¨† ״§„ƒ„…״§״× ״§„״±״¦״³״© ˆ״§„״£״³…״§״¡ ״§„״¨״¯„״©
    // (EN) G_CONFLICT ג€” No collisions between primaries and aliases
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_GROUP("KeywordsV41.G_CONFLICT");

    SAD_TEST("C.01: ƒ„ primaryWord ״±״¯ ״¹״¨״± ״§„…״¹״¬… ƒ״§…„״§‹", {
        std::set<std::string> seen;
        for (const auto& e : allEntries()) {
            SAD_ASSERT_TRUE(seen.insert(e.primaryWord).second);
        }
    });

    SAD_TEST("C.02: ƒ„ alias ״±״¯ ˆ„״§ ״·״§״¨‚ primaryWord …״®״×„״§‹", {
        // (AR) ״§״¨† ״®״±״·״© word ג†’ entry „ƒ„ ״§„ƒ״×״§״¨״§״×
        for (const auto& e : allEntries()) {
            for (const auto& a : e.aliases) {
                // (AR) „״§ ˆ״¬״¯ ״¥״¯״®״§„ ״¢״®״± primaryWord = a
                for (const auto& other : allEntries()) {
                    if (&other == &e) continue;
                    SAD_ASSERT_NE(other.primaryWord, a);
                }
            }
        }
    });

    SAD_TEST("C.03: TokenType ״±״¯ „ƒ„ ״¥״¯״®״§„ …״µ״¯״±", {
        std::set<int> seenTypes;
        for (const auto& e : allEntries()) {
            if (!e.emittedByLexer) continue;
            SAD_ASSERT_TRUE(seenTypes.insert(static_cast<int>(e.type)).second);
        }
    });

    SAD_TEST("C.04: „״§ CONTEXTUAL ״µ״¯״± ƒ״±…״² ״®״§״µ  Lexer", {
        for (const auto& e : allEntries()) {
            if (e.category != KeywordCategory::CONTEXTUAL) continue;
            // (AR) Lexer ״¬״¨ ״£„״§ ״µ״¯״± ״³״§‚״© ƒ״±…״² ״®״§״µ
            SAD_ASSERT_FALSE(KeywordTable::isKeyword(e.primaryWord));
        }
    });

    SAD_TEST("C.05: „״§ BUILTIN_TYPE ״µ״¯״± ƒ״±…״² ״®״§״µ  Lexer", {
        for (const auto& e : allEntries()) {
            if (e.category != KeywordCategory::BUILTIN_TYPE) continue;
            SAD_ASSERT_FALSE(KeywordTable::isKeyword(e.primaryWord));
        }
    });

    SAD_TEST("C.06: ƒ„ RESERVED …״µ״¯״± ƒ״±…״² ״®״§״µ  Lexer", {
        for (const auto& e : allEntries()) {
            if (e.category != KeywordCategory::RESERVED) continue;
            SAD_ASSERT_TRUE(KeywordTable::isKeyword(e.primaryWord));
        }
    });

    SAD_TEST("C.07: shouldEmitAsKeyword ״·״§״¨‚ emittedByLexer", {
        for (const auto& e : allEntries()) {
            SAD_ASSERT_EQ(KeywordTable::shouldEmitAsKeyword(e.type),
                          e.emittedByLexer);
        }
    });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    SAD_SUMMARY();
}
