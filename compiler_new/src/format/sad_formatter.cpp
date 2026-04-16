// ״¨״³… ״§„„‡ ״§„״±״­…† ״§„״±״­…
// ============================================================================
// sad_formatter.cpp ג€” ״×†״° …†״³‘‚ ƒˆ״¯ „״÷״© ״µ ״§„…״×‚״¯…
// Advanced Code Formatter Implementation
// ============================================================================

#include "format/sad_formatter.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cmath>

namespace fs = std::filesystem;

namespace Sad {
namespace Format {

// ============================================================================
// ״§„ƒ„…״§״× ״§„…״×״§״­״© ˆ״§„״¹ˆ״§…„
// ============================================================================

static const std::vector<std::string> SAD_KEYWORDS = {
    // ״×״­ƒ…
    "״¥״°״§", "״§״°״§", "ˆ״¥„״§", "ˆ״§„״§", "ˆ״§„״§_״§״°״§", "ˆ״¥„״§_״¥״°״§",
    "״¨†…״§", "״·״§„…״§", "„ƒ„", "ƒ״±״±",
    // ״×״¹״±״§״×
    "״¯״§„״©", "״µ†", "‡ƒ„", "ˆ״§״¬‡״©", "״³…״©", "ˆ״­״¯״©", "״×״¹״¯״§״¯", "״¨†״©",
    "״«״§״¨״×", "…״×״÷״±",
    // ״×״­ƒ… ״×״¯‚
    "״§״±״¬״¹", "״¥״±״¬״§״¹", "״§״®״±״¬", "״×ˆ‚", "״×״§״¨״¹", "״§״³״×…״±",
    // ״¨†״©
    "†‡״§״©", "״¨״§†",
    // ״§״³״×״±״§״¯
    "״§״³״×ˆ״±״¯", "…†", "״µ״¯‘״±", "ƒ€",
    // ״§״³״×״«†״§״¡״§״×
    "״­״§ˆ„", "״§…״³ƒ", "״£״®״±״§‹", "״§״±…", "״§״±…",
    // ƒ״§״¦†״©
    "״¬״¯״¯", "‡״°״§", "״°״§״×", "״§„״£״³״§״³",
    // ‚… ״®״§״µ״©
    "״µ״­״­", "״®״·״£", "״¹״¯…", "״§״±״÷", "„״§״´״¡",
    // ״¹״¶ˆ״©
    "",
    // ״£†ˆ״§״¹
    "״±‚…", "״¹״´״±", "…†״·‚", "†״µ", "…״µˆ״©", "‚״§״¦…״©", "״®״±״·״©", "…״¬…ˆ״¹״©", "״£", "ƒ״§״¦†", "״±״§״÷",
    // …†״·‚״©
    "ˆ", "״£ˆ", "„״³",
    // …״·״§״¨‚״© ״£†…״§״·
    "״·״§״¨‚", "״­״§„״©", "״¹†״¯…״§", "״§״×״±״§״¶",
    // ƒ„…״§״× ״³״§‚״© ג€” „״§…״¯״§ ˆ״÷״± …״×״²״§…†
    "„״§…״¯״§", "״÷״±_…״×״²״§…†", "״§†״×״¸״±", "…ˆ„״¯", "״£†״×״¬",
    // ƒ„…״§״× ״³״§‚״© ג€” ‚ˆ״§„״¨ ˆ״³…״§״×
    "‚״§„״¨", "†‘״°", "†״°",
    // ƒ„…״§״× ״³״§‚״© ג€” ״®״µ״§״¦״µ ˆ״¯ˆ״±״© ״­״§״©
    "״®״§״µ״©", "״§״­״µ„", "״¹‘†", "‡״¯…", "״¹״§…„", "״±״¦״³״©",
    // …״¹״¯‘„״§״× ״§„ˆ״µˆ„
    "״¹״§…", "״®״§״µ", "…״­…", "…״¬״±״¯", "״³״§ƒ†", "״®״§״±״¬",
    // ״¥״¯״§״±״© ״§„…ˆ״§״±״¯
    "״¨״§״³״×״®״¯״§…", "†‡״§״©_״§״³״×״®״¯״§…",
    // ״¶״§״¡ ״§„״£״³…״§״¡
    "״¶״§״¡", "†‡״§״©_״¶״§״¡",
    // ״§״®״×״¨״§״±״§״×
    "״§״®״×״¨״±", "״±״«"
};

static const std::vector<std::string> SAD_OPERATORS_SORTED = {
    // 3 chars
    "**=",
    // 2 chars
    "==", "!=", ">=", "<=", "+=", "-=", "*=", "/=", "%=",
    "&&", "||", "**", "->", "..", "?.", "??",
    // 1 char
    "+", "-", "*", "/", "%", "^",
    "=", ">", "<", "!", "?",
    ":", ".", "&", "|", "~"
};

static bool isKeyword(const std::string& s) {
    for (const auto& kw : SAD_KEYWORDS)
        if (s == kw) return true;
    return false;
}

static bool isImportKeyword(const std::string& s) {
    return s == "״§״³״×ˆ״±״¯" || s == "…†" || s == "״µ״¯‘״±" || s == "ƒ€";
}

static bool isBlockOpener(const std::string& s) {
    return s == "״¯״§„״©" || s == "״µ†" || s == "‡ƒ„" || s == "ˆ״§״¬‡״©" ||
           s == "״³…״©" || s == "ˆ״­״¯״©" || s == "״×״¹״¯״§״¯" || s == "״¨†״©" ||
           s == "״¥״°״§" || s == "״§״°״§" ||
           s == "״¨†…״§" || s == "״·״§„…״§" || s == "„ƒ„" || s == "ƒ״±״±" ||
           s == "״­״§ˆ„" || s == "״·״§״¨‚" || s == "״­״§„״©" ||
           s == "״¨״§†" || s == "‡״¯…" || s == "״®״§״µ״©" ||
           s == "״÷״±_…״×״²״§…†" || s == "„״§…״¯״§" ||
           s == "‚״§„״¨" || s == "†‘״°" || s == "†״°" ||
           s == "״¨״§״³״×״®״¯״§…" || s == "״¶״§״¡" || s == "״§״®״×״¨״±" ||
           s == "״±״¦״³״©";
}

static bool isBlockCloser(const std::string& s) {
    return s == "†‡״§״©" || s == "†‡״§״©_״§״³״×״®״¯״§…" || s == "†‡״§״©_״¶״§״¡";
}

static bool isInterBlock(const std::string& s) {
    return s == "ˆ״¥„״§" || s == "ˆ״§„״§" || s == "ˆ״§„״§_״§״°״§" || s == "ˆ״¥„״§_״¥״°״§" ||
           s == "״§…״³ƒ" || s == "״£״®״±״§‹" ||
           s == "״¹†״¯…״§" || s == "״§״×״±״§״¶";
}

// ============================================================================
// FmtToken methods
// ============================================================================

bool FmtToken::isBlockOpener() const {
    return (type == FmtTokenType::KEYWORD) && Sad::Format::isBlockOpener(value);
}
bool FmtToken::isBlockCloser() const {
    return (type == FmtTokenType::KEYWORD) && Sad::Format::isBlockCloser(value);
}
bool FmtToken::isElseKeyword() const {
    return (type == FmtTokenType::KEYWORD) && Sad::Format::isInterBlock(value);
}
bool FmtToken::isImport() const {
    return (type == FmtTokenType::IMPORT_KEYWORD);
}
bool FmtToken::isTypeKeyword() const {
    return (type == FmtTokenType::KEYWORD) &&
           (value == "״±‚…" || value == "״¹״´״±" || value == "…†״·‚" ||
            value == "†״µ" || value == "…״µˆ״©" || value == "‚״§״¦…״©" ||
            value == "״®״±״·״©" || value == "…״¬…ˆ״¹״©" || value == "״£" || 
            value == "ƒ״§״¦†" || value == "״±״§״÷");
}

// ============================================================================
// FmtLexer
// ============================================================================

char FmtLexer::current() const {
    return pos_ < source_.size() ? source_[pos_] : '\0';
}

char FmtLexer::peek(int ahead) const {
    size_t idx = pos_ + ahead;
    return idx < source_.size() ? source_[idx] : '\0';
}

void FmtLexer::advance(int count) {
    for (int i = 0; i < count && pos_ < source_.size(); ++i) {
        if (source_[pos_] == '\n') { line_++; column_ = 1; }
        else { column_++; }
        pos_++;
    }
}

bool FmtLexer::match(const std::string& expected) const {
    if (pos_ + expected.size() > source_.size()) return false;
    return source_.compare(pos_, expected.size(), expected) == 0;
}

bool FmtLexer::isDigit(char c) const { return c >= '0' && c <= '9'; }

bool FmtLexer::isArabicByte(char c) const {
    return static_cast<unsigned char>(c) >= 0x80;
}

bool FmtLexer::isIdentStart(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || isArabicByte(c);
}

bool FmtLexer::isIdentChar(char c) const {
    return isIdentStart(c) || isDigit(c);
}

void FmtLexer::emit(FmtTokenType type, const std::string& value, int sl, int sc, int so) {
    tokens_.push_back({type, value, sl, sc, so});
}

std::vector<FmtToken> FmtLexer::tokenize(const std::string& source) {
    source_ = source;
    pos_ = 0; line_ = 1; column_ = 1;
    tokens_.clear();

    while (pos_ < source_.size()) {
        char c = current();

        // ״×״¹„‚ ״×ˆ״«‚ #** ״£ˆ ##
        if (c == '#' && peek(1) == '*' && peek(2) == '*') {
            scanDocComment();
        }
        // ״×״¹„‚ ƒ״×„״© #*
        else if (c == '#' && peek(1) == '*') {
            scanBlockComment();
        }
        // ״×״¹„‚ ״³״·״± #
        else if (c == '#') {
            int sl = line_, sc = column_; int so = (int)pos_;
            if (peek(1) == '#' && peek(2) != '*') {
                // ## doc comment line
                std::string val;
                while (pos_ < source_.size() && current() != '\n') { val += current(); advance(); }
                emit(FmtTokenType::COMMENT_DOC, val, sl, sc, so);
            } else {
                scanLineComment();
            }
        }
        // †״µ
        else if (c == '"' || c == '\'') {
            scanString(c);
        }
        // ״±‚…
        else if (isDigit(c) || (c == '.' && isDigit(peek(1)))) {
            scanNumber();
        }
        // ״³״·״± ״¬״¯״¯
        else if (c == '\n' || c == '\r') {
            scanNewline();
        }
        // …״³״§״©
        else if (c == ' ' || c == '\t') {
            scanWhitespace();
        }
        // ״§״µ„״© ״¹״±״¨״© ״
        else if (static_cast<unsigned char>(c) == 0xD8 &&
                 pos_+1 < source_.size() &&
                 static_cast<unsigned char>(source_[pos_+1]) == 0x8C) {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance(2);
            emit(FmtTokenType::COMMA, "\xD8\x8C", sl, sc, so);
        }
        // ״§״µ„״© …†‚ˆ״·״© ״¹״±״¨״© ״›
        else if (static_cast<unsigned char>(c) == 0xD8 &&
                 pos_+1 < source_.size() &&
                 static_cast<unsigned char>(source_[pos_+1]) == 0x9B) {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance(2);
            emit(FmtTokenType::SEMICOLON, "\xD8\x9B", sl, sc, so);
        }
        // ״§״µ„״© „״§״×†״©
        else if (c == ',') {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance();
            emit(FmtTokenType::COMMA, ",", sl, sc, so);
        }
        // ״§״µ„״© …†‚ˆ״·״© „״§״×†״©
        else if (c == ';') {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance();
            emit(FmtTokenType::SEMICOLON, ";", sl, sc, so);
        }
        // …״¹״±‘ (״¹״±״¨ ״£ˆ „״§״×†)
        else if (isIdentStart(c)) {
            scanIdentifier();
        }
        // ״×ˆ״¬‡ @
        else if (c == '@') {
            int sl = line_, sc = column_; int so = (int)pos_;
            std::string val = "@";
            advance();
            // ‚״±״§״¡״© ״§״³… ״§„״×ˆ״¬‡ ״¨״¹״¯ @
            while (pos_ < source_.size() && isIdentChar(current())) {
                val += current(); advance();
            }
            emit(FmtTokenType::DIRECTIVE, val, sl, sc, so);
        }
        // ״¹״§…„
        else {
            scanOperator();
        }
    }

    emit(FmtTokenType::END_OF_FILE, "", line_, column_, (int)pos_);
    return tokens_;
}

void FmtLexer::scanLineComment() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    while (pos_ < source_.size() && current() != '\n') {
        val += current(); advance();
    }
    emit(FmtTokenType::COMMENT_LINE, val, sl, sc, so);
}

void FmtLexer::scanBlockComment() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    val += current(); advance(); // #
    val += current(); advance(); // *
    while (pos_ < source_.size()) {
        if (current() == '*' && peek(1) == '#') {
            val += current(); advance();
            val += current(); advance();
            break;
        }
        val += current(); advance();
    }
    emit(FmtTokenType::COMMENT_BLOCK, val, sl, sc, so);
}

void FmtLexer::scanDocComment() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    val += current(); advance(); // #
    val += current(); advance(); // *
    val += current(); advance(); // *
    while (pos_ < source_.size()) {
        if (current() == '*' && peek(1) == '*' && peek(2) == '#') {
            val += current(); advance();
            val += current(); advance();
            val += current(); advance();
            break;
        }
        val += current(); advance();
    }
    emit(FmtTokenType::COMMENT_DOC, val, sl, sc, so);
}

void FmtLexer::scanString(char quote) {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    val += current(); advance();
    while (pos_ < source_.size() && current() != quote) {
        if (current() == '\\') { val += current(); advance(); }
        if (pos_ < source_.size()) { val += current(); advance(); }
    }
    if (pos_ < source_.size()) { val += current(); advance(); }
    emit(FmtTokenType::STRING, val, sl, sc, so);
}

void FmtLexer::scanRawString() {
    // raw strings: r"..." ג€” handled via normal scan with prefix
    scanString('"');
}

void FmtLexer::scanNumber() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    if (current() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
        val += current(); advance(); val += current(); advance();
        while (pos_ < source_.size() && (isDigit(current()) ||
               (current() >= 'a' && current() <= 'f') ||
               (current() >= 'A' && current() <= 'F'))) {
            val += current(); advance();
        }
    } else if (current() == '0' && (peek(1) == 'b' || peek(1) == 'B')) {
        val += current(); advance(); val += current(); advance();
        while (pos_ < source_.size() && (current() == '0' || current() == '1')) {
            val += current(); advance();
        }
    } else {
        while (pos_ < source_.size() && (isDigit(current()) || current() == '.' || current() == '_')) {
            if (current() == '.' && !isDigit(peek(1))) break;
            val += current(); advance();
        }
        if (pos_ < source_.size() && (current() == 'e' || current() == 'E')) {
            val += current(); advance();
            if (pos_ < source_.size() && (current() == '+' || current() == '-')) {
                val += current(); advance();
            }
            while (pos_ < source_.size() && isDigit(current())) {
                val += current(); advance();
            }
        }
    }
    emit(FmtTokenType::NUMBER, val, sl, sc, so);
}

void FmtLexer::scanIdentifier() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    while (pos_ < source_.size() && isIdentChar(current())) {
        // „״§ ״×״³״×‡„ƒ ״§„״§״µ„״© ״§„״¹״±״¨״© ״ ״£ˆ ״§„״§״µ„״© ״§„…†‚ˆ״·״© ״§„״¹״±״¨״© ״›
        unsigned char uc = static_cast<unsigned char>(current());
        if (uc == 0xD8 && pos_ + 1 < source_.size()) {
            unsigned char nc = static_cast<unsigned char>(source_[pos_ + 1]);
            if (nc == 0x8C || nc == 0x9B) break;  // ״ or ״›
        }
        val += current(); advance();
    }

    if (isImportKeyword(val)) {
        emit(FmtTokenType::IMPORT_KEYWORD, val, sl, sc, so);
    } else if (isKeyword(val)) {
        emit(FmtTokenType::KEYWORD, val, sl, sc, so);
    } else {
        emit(FmtTokenType::IDENTIFIER, val, sl, sc, so);
    }
}

void FmtLexer::scanWhitespace() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    while (pos_ < source_.size() && (current() == ' ' || current() == '\t')) {
        val += current(); advance();
    }
    emit(FmtTokenType::WHITESPACE, val, sl, sc, so);
}

void FmtLexer::scanNewline() {
    int sl = line_, sc = column_; int so = (int)pos_;
    std::string val;
    if (current() == '\r' && peek(1) == '\n') {
        val = "\r\n"; advance(2);
    } else {
        val = std::string(1, current()); advance();
    }
    emit(FmtTokenType::NEWLINE, val, sl, sc, so);
}

void FmtLexer::scanOperator() {
    int sl = line_, sc = column_; int so = (int)pos_;
    // …״­״§ˆ„״© …״·״§״¨‚״© ״£״·ˆ„ ״¹״§…„
    std::string best;
    for (const auto& op : SAD_OPERATORS_SORTED) {
        if (match(op) && op.size() > best.size()) {
            best = op;
        }
    }

    if (best == "->") {
        advance((int)best.size());
        emit(FmtTokenType::ARROW, best, sl, sc, so);
    } else if (best == "?.") {
        // (AR) ˆ״µˆ„ ״§״®״×״§״± ?. ג€” ״¹״§…„ ƒ†‚״·״© (״¨״¯ˆ† …״³״§״§״×)
        // (EN) Optional chain ?. ג€” treated like dot (no spaces)
        advance(2);
        emit(FmtTokenType::DOT, "?.", sl, sc, so);
    } else if (best == "??") {
        // (AR) ״§†״¯…״§״¬ ״µ״± ?? ג€” ״¹״§…„ ״«†״§״¦ (…״¹ …״³״§״§״×)
        // (EN) Null coalesce ?? ג€” binary operator (with spaces)
        advance(2);
        emit(FmtTokenType::OPERATOR, "??", sl, sc, so);
    } else if (best == ":") {
        advance(1);
        emit(FmtTokenType::COLON, ":", sl, sc, so);
    } else if (best == ".") {
        advance(1);
        emit(FmtTokenType::DOT, ".", sl, sc, so);
    } else if (!best.empty()) {
        advance((int)best.size());
        emit(FmtTokenType::OPERATOR, best, sl, sc, so);
    } else {
        // ״¹„״§…״© ״×״±‚…
        std::string val(1, current());
        advance();
        emit(FmtTokenType::PUNCTUATION, val, sl, sc, so);
    }
}

// ============================================================================
// FormatterOptions ג€” ״×״­…„/״­״¸
// ============================================================================

FormatterOptions FormatterOptions::fromFile(const std::string& path) {
    FormatterOptions opts;
    std::ifstream file(path);
    if (!file.is_open()) return opts;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        auto trim = [](std::string& s) {
            size_t a = s.find_first_not_of(" \t");
            size_t b = s.find_last_not_of(" \t\r\n");
            s = (a != std::string::npos) ? s.substr(a, b - a + 1) : "";
        };
        trim(key); trim(val);
        bool bval = (val == "true" || val == "†״¹…" || val == "1");

        if (key == "indent_size" || key == "״­״¬…_״§„…״³״§״©") opts.indentSize = std::stoi(val);
        else if (key == "use_spaces" || key == "״§״³״×״®״¯…_…״³״§״§״×") opts.useSpaces = bval;
        else if (key == "max_line_length" || key == "״­״¯_״·ˆ„_״§„״³״·״±") opts.maxLineLength = std::stoi(val);
        else if (key == "wrap_long_lines" || key == "„_״§„״£״³״·״±") opts.wrapLongLines = bval;
        else if (key == "space_after_comma" || key == "…״³״§״©_״¨״¹״¯_״§„״§״µ„״©") opts.spaceAfterComma = bval;
        else if (key == "space_around_operators" || key == "…״³״§״©_״­ˆ„_״§„״¹ˆ״§…„") opts.spaceAroundOperators = bval;
        else if (key == "space_after_keywords" || key == "…״³״§״©_״¨״¹״¯_״§„…״×״§״­״©") opts.spaceAfterKeywords = bval;
        else if (key == "space_after_colon" || key == "…״³״§״©_״¨״¹״¯_״§„†‚״·״×†") opts.spaceAfterColon = bval;
        else if (key == "space_before_colon" || key == "…״³״§״©_‚״¨„_״§„†‚״·״×†") opts.spaceBeforeColon = bval;
        else if (key == "space_inside_parens" || key == "…״³״§״©_״¯״§״®„_״§„״£‚ˆ״§״³") opts.spaceInsideParens = bval;
        else if (key == "space_around_arrow" || key == "…״³״§״©_״­ˆ„_״§„״³‡…") opts.spaceAroundArrow = bval;
        else if (key == "blank_lines_after_imports") opts.blankLinesAfterImports = std::stoi(val);
        else if (key == "blank_lines_between_functions") opts.blankLinesBetweenFunctions = std::stoi(val);
        else if (key == "blank_lines_between_classes") opts.blankLinesBetweenClasses = std::stoi(val);
        else if (key == "max_consecutive_blank_lines") opts.maxConsecutiveBlankLines = std::stoi(val);
        else if (key == "trim_trailing_whitespace" || key == "״­״°_״§„״²״§״¦״¯") opts.trimTrailingWhitespace = bval;
        else if (key == "ensure_final_newline") opts.ensureFinalNewline = bval;
        else if (key == "remove_trailing_semicolons") opts.removeTrailingSemicolons = bval;
        else if (key == "sort_imports" || key == "״±״×״¨_״§„״§״³״×״±״§״¯״§״×") opts.sortImports = bval;
        else if (key == "group_imports") opts.groupImports = bval;
        else if (key == "normalize_arabic_comma") opts.normalizeArabicComma = bval;
        else if (key == "quote_style") {
            if (val == "double") opts.quoteStyle = QuoteStyle::Double;
            else if (val == "single") opts.quoteStyle = QuoteStyle::Single;
            else opts.quoteStyle = QuoteStyle::Preserve;
        }
        else if (key == "line_ending") {
            if (val == "lf") opts.lineEnding = LineEnding::LF;
            else if (val == "crlf") opts.lineEnding = LineEnding::CRLF;
            else opts.lineEnding = LineEnding::Auto;
        }
        else if (key == "profile" || key == "ˆ״¶״¹") {
            if (val == "compact" || val == "…״¶״÷ˆ״·") opts.applyProfile(FormatProfile::Compact);
            else if (val == "verbose" || val == "…״·ˆ‘„") opts.applyProfile(FormatProfile::Verbose);
            else opts.applyProfile(FormatProfile::Standard);
        }
    }
    return opts;
}

FormatterOptions FormatterOptions::fromProfile(FormatProfile profile) {
    FormatterOptions opts;
    opts.applyProfile(profile);
    return opts;
}

void FormatterOptions::applyProfile(FormatProfile p) {
    profile = p;
    switch (p) {
        case FormatProfile::Compact:
            indentSize = 2;
            maxLineLength = 120;
            blankLinesBetweenFunctions = 0;
            blankLinesBetweenClasses = 1;
            maxConsecutiveBlankLines = 1;
            spaceInsideParens = false;
            blankLineBeforeReturn = false;
            break;
        case FormatProfile::Verbose:
            indentSize = 4;
            maxLineLength = 80;
            blankLinesBetweenFunctions = 2;
            blankLinesBetweenClasses = 3;
            maxConsecutiveBlankLines = 3;
            blankLineBeforeReturn = true;
            blankLineAfterBlockOpen = true;
            break;
        case FormatProfile::Standard:
        case FormatProfile::Custom:
            break;
    }
}

void FormatterOptions::saveToFile(const std::string& path) const {
    std::ofstream f(path);
    if (!f.is_open()) return;
    f << "# ״¥״¹״¯״§״¯״§״× …†״³‘‚ ƒˆ״¯ „״÷״© ״µ\n"
      << "# Sad Language Formatter Configuration\n\n"
      << "indent_size = " << indentSize << "\n"
      << "use_spaces = " << (useSpaces ? "true" : "false") << "\n"
      << "max_line_length = " << maxLineLength << "\n"
      << "wrap_long_lines = " << (wrapLongLines ? "true" : "false") << "\n\n"
      << "# ״§„…״³״§״§״×\n"
      << "space_after_comma = " << (spaceAfterComma ? "true" : "false") << "\n"
      << "space_around_operators = " << (spaceAroundOperators ? "true" : "false") << "\n"
      << "space_after_keywords = " << (spaceAfterKeywords ? "true" : "false") << "\n"
      << "space_after_colon = " << (spaceAfterColon ? "true" : "false") << "\n"
      << "space_around_arrow = " << (spaceAroundArrow ? "true" : "false") << "\n\n"
      << "# ״§„״£״³״·״± ״§„״§״±״÷״©\n"
      << "blank_lines_between_functions = " << blankLinesBetweenFunctions << "\n"
      << "blank_lines_between_classes = " << blankLinesBetweenClasses << "\n"
      << "max_consecutive_blank_lines = " << maxConsecutiveBlankLines << "\n\n"
      << "# ״§„״×†״¸\n"
      << "trim_trailing_whitespace = " << (trimTrailingWhitespace ? "true" : "false") << "\n"
      << "ensure_final_newline = " << (ensureFinalNewline ? "true" : "false") << "\n"
      << "sort_imports = " << (sortImports ? "true" : "false") << "\n";
}

// ============================================================================
// SadFormatter
// ============================================================================

SadFormatter::SadFormatter(const FormatterOptions& options) : options_(options) {}

FormatResult SadFormatter::format(const std::string& source) {
    FormatResult result;
    auto start = std::chrono::high_resolution_clock::now();

    try {
        std::string normalized = normalizeLineEndings(source);

        // …״±״­„״© 1: ״×״±״×״¨ ״§„״§״³״×״±״§״¯״§״×
        std::string processed = normalized;
        if (options_.sortImports) {
            processed = sortAndGroupImports(processed);
        }

        // …״±״­„״© 2: ״§„״×‚״³… ״¥„‰ ״±…ˆ״²
        FmtLexer lexer;
        auto tokens = lexer.tokenize(processed);

        // …״±״­„״© 3: ״¥״¹״§״¯״© ״§„״¨†״§״¡ …״¹ ״§„״×†״³‚
        result.output = rebuild(tokens);

        // …״±״­„״© 4: „‘ ״§„״£״³״·״± ״§„״·ˆ„״©
        if (options_.wrapLongLines) {
            auto lines = splitLines(result.output);
            // †״­״³״¨ …״³״×ˆ‰ ״§„…״³״§״© „ƒ„ ״³״·״±
            for (auto& line : lines) {
                if (lineLength(line) > options_.maxLineLength) {
                    int indent = 0;
                    for (char ch : line) {
                        if (ch == ' ') indent++;
                        else if (ch == '\t') indent += options_.indentSize;
                        else break;
                    }
                    line = wrapLine(line, indent);
                }
            }
            result.output = joinLines(lines);
        }

        // …״±״­„״© 5: ״×†״³‚ †‡״§״© ״§„…„
        if (options_.ensureFinalNewline && !result.output.empty() && result.output.back() != '\n') {
            result.output += "\n";
        }

        // …״±״­„״© 6: ״×״·״¨‚ †…״· †‡״§״© ״§„״³״·״±
        if (options_.lineEnding == LineEnding::CRLF) {
            std::string crlf;
            for (size_t i = 0; i < result.output.size(); i++) {
                if (result.output[i] == '\n' && (i == 0 || result.output[i-1] != '\r')) {
                    crlf += "\r\n";
                } else {
                    crlf += result.output[i];
                }
            }
            result.output = crlf;
        }

        // ״­״³״§״¨ ״§„״×״÷״±״§״×
        result.changed = (result.output != source);
        if (result.changed) {
            auto origLines = splitLines(source);
            auto fmtLines = splitLines(result.output);
            result.totalLines = (int)fmtLines.size();
            int changed = 0;
            size_t maxLen = std::max(origLines.size(), fmtLines.size());
            for (size_t i = 0; i < maxLen; i++) {
                std::string o = (i < origLines.size()) ? origLines[i] : "";
                std::string f = (i < fmtLines.size()) ? fmtLines[i] : "";
                if (o != f) changed++;
            }
            result.linesChanged = changed;
        } else {
            auto fmtLines = splitLines(result.output);
            result.totalLines = (int)fmtLines.size();
        }
    } catch (const std::exception& e) {
        result.error = std::string("״®״·״£  ״§„״×†״³‚: ") + e.what();
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
    return result;
}

FormatResult SadFormatter::formatFile(const std::string& path, bool dryRun) {
    FormatResult result;

    std::ifstream input(path);
    if (!input.is_open()) {
        result.error = "„״§ …ƒ† ״×״­ ״§„…„: " + path;
        return result;
    }
    std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    result = format(source);

    if (result.success() && result.changed && !dryRun) {
        std::ofstream output(path);
        if (!output.is_open()) {
            result.error = "„״§ …ƒ† ƒ״×״§״¨״© ״§„…„: " + path;
            return result;
        }
        output << result.output;
    }

    return result;
}

FormatStats SadFormatter::formatDirectory(const std::string& dir, bool recursive, bool dryRun) {
    FormatStats stats;

    try {
        auto process = [&](const fs::path& p) {
            if (p.extension().string() == ".״µ" || p.extension().string() == ".sad") {
                auto res = formatFile(p.string(), dryRun);
                stats.filesProcessed++;
                stats.totalDurationMs += res.durationMs;
                if (!res.success()) {
                    stats.errors.push_back(p.string() + ": " + res.error);
                } else if (res.changed) {
                    stats.filesChanged++;
                    stats.totalLinesChanged += res.linesChanged;
                    stats.changedFiles.push_back(p.string());
                }
            }
        };

        if (recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file()) process(entry.path());
            }
        } else {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file()) process(entry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        stats.errors.push_back(std::string("״®״·״£  ‚״±״§״¡״© ״§„…״¬„״¯: ") + e.what());
    }

    return stats;
}

FormatResult SadFormatter::check(const std::string& source) {
    auto result = format(source);
    //  ˆ״¶״¹ ״§„״­״µ „״§ †״¹״¯ ״§„ƒˆ״¯ ״§„…״¹״¯‘„
    if (result.changed) {
        result.diffs.push_back(diff(source, result.output));
    }
    return result;
}

std::string SadFormatter::diff(const std::string& original, const std::string& formatted,
                                const std::string& filename) const {
    auto origLines = splitLines(original);
    auto fmtLines = splitLines(formatted);

    std::ostringstream out;
    out << "--- " << filename << " (״§„״£״µ„)\n"
        << "+++ " << filename << " (״§„…†״³‘‚)\n";

    size_t maxLen = std::max(origLines.size(), fmtLines.size());
    int contextSize = 3;

    for (size_t i = 0; i < maxLen; i++) {
        std::string o = (i < origLines.size()) ? origLines[i] : "";
        std::string f = (i < fmtLines.size()) ? fmtLines[i] : "";
        if (o != f) {
            size_t start = (i >= (size_t)contextSize) ? i - contextSize : 0;
            size_t end = std::min(i + contextSize + 1, maxLen);
            out << "@@ -" << (start+1) << "," << (end-start) << " +" << (start+1) << "," << (end-start) << " @@\n";
            for (size_t j = start; j < end; j++) {
                std::string ol = (j < origLines.size()) ? origLines[j] : "";
                std::string fl = (j < fmtLines.size()) ? fmtLines[j] : "";
                if (ol == fl) {
                    out << " " << ol << "\n";
                } else {
                    if (j < origLines.size()) out << "-" << ol << "\n";
                    if (j < fmtLines.size()) out << "+" << fl << "\n";
                }
            }
            i = end;
        }
    }

    return out.str();
}

bool SadFormatter::loadConfigFromDirectory(const std::string& dir) {
    std::vector<std::string> configNames = {".״×†״³‚", ".sad-fmt", ".sadformat", "״×†״³‚.cfg"};
    for (const auto& name : configNames) {
        std::string path = dir + "/" + name;
        if (fs::exists(path)) {
            options_ = FormatterOptions::fromFile(path);
            return true;
        }
    }
    return false;
}

// ============================================================================
// rebuild ג€” ״¥״¹״§״¯״© ״¨†״§״¡ ״§„ƒˆ״¯ …† ״§„״±…ˆ״²
// ============================================================================


} // namespace Format
} // namespace Sad

