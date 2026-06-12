#include "catch.hpp"
#include "lexer_keywords.h"
#include <string>

using namespace Sad::Lexer;

TEST_CASE("Keyword table initialization and lookup", "[lexer][keywords]") {
    KeywordTable::initialize();

    auto all = KeywordTable::getAllKeywords();
    REQUIRE(all.size() > 0);

    // For each keyword, ensure table recognizes it and that mapping round-trips
    for (const auto& kw : all) {
        INFO("Checking keyword: " << kw);
        REQUIRE(KeywordTable::isKeyword(kw));
        auto t = KeywordTable::getKeywordType(kw);
        auto back = KeywordTable::getKeyword(t);
        REQUIRE(back == kw);
    }
}

TEST_CASE("Arabic diacritics do not match plain keywords", "[lexer][keywords][arabic]") {
    KeywordTable::initialize();
    auto all = KeywordTable::getAllKeywords();
    REQUIRE(all.size() > 0);

    // pick a known keyword (if exists) and add a diacritic variant
    std::string sample = KeywordTable::getAllKeywords().front();
    std::string diacritic = sample + "\u0651"; // add shadda as example

    REQUIRE_FALSE(KeywordTable::isKeyword(diacritic));
}
