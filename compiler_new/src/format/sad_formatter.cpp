// بسم الله الرحمن الرحيم
// ============================================================================
// sad_formatter.cpp — تنفيذ منسّق كود لغة ص المتقدم
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
// الكلمات المفتاحية والعوامل
// ============================================================================

static const std::vector<std::string> SAD_KEYWORDS = {
    // تحكم
    "إذا", "اذا", "وإلا", "والا", "والا_اذا", "وإلا_إذا",
    "بينما", "طالما", "لكل", "كرر",
    // تعريفات
    "دالة", "صنف", "هيكل", "واجهة", "سمة", "وحدة", "تعداد",
    "ثابت", "متغير",
    // تحكم تدفق
    "ارجع", "إرجاع", "اخرج", "توقف", "تابع", "استمر",
    // بنية
    "نهاية", "باني",
    // استيراد
    "استورد", "من", "صدّر", "كـ",
    // استثناءات
    "حاول", "امسك", "أخيراً", "ارمِ",
    // كائنية
    "جديد", "هذا", "ذاتي",
    // قيم خاصة
    "صحيح", "خطأ", "عدم", "فارغ",
    // عضوية
    "في",
    // أنواع
    "رقم", "عشري", "منطقي", "نص", "مصفوفة", "قائمة", "خريطة", "مجموعة", "أي", "كائن",
    // منطقية
    "و", "أو", "ليس",
    // switch
    "حالة", "عندما", "افتراضي"
};

static const std::vector<std::string> SAD_OPERATORS_SORTED = {
    // 3 chars
    "**=",
    // 2 chars
    "==", "!=", ">=", "<=", "+=", "-=", "*=", "/=", "%=",
    "&&", "||", "**", "->", "..",
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
    return s == "استورد" || s == "من" || s == "صدّر" || s == "كـ";
}

static bool isBlockOpener(const std::string& s) {
    return s == "دالة" || s == "صنف" || s == "هيكل" || s == "واجهة" ||
           s == "سمة" || s == "وحدة" || s == "تعداد" ||
           s == "إذا" || s == "اذا" ||
           s == "بينما" || s == "طالما" || s == "لكل" || s == "كرر" ||
           s == "حاول" || s == "حالة" ||
           s == "باني";
}

static bool isBlockCloser(const std::string& s) {
    return s == "نهاية";
}

static bool isInterBlock(const std::string& s) {
    return s == "وإلا" || s == "والا" || s == "والا_اذا" || s == "وإلا_إذا" ||
           s == "امسك" || s == "أخيراً" ||
           s == "عندما" || s == "افتراضي";
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
           (value == "رقم" || value == "عشري" || value == "منطقي" ||
            value == "نص" || value == "مصفوفة" || value == "قائمة" ||
            value == "خريطة" || value == "مجموعة" || value == "أي" || value == "كائن");
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

        // تعليق توثيق #** أو ##
        if (c == '#' && peek(1) == '*' && peek(2) == '*') {
            scanDocComment();
        }
        // تعليق كتلة #*
        else if (c == '#' && peek(1) == '*') {
            scanBlockComment();
        }
        // تعليق سطري #
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
        // نص
        else if (c == '"' || c == '\'') {
            scanString(c);
        }
        // رقم
        else if (isDigit(c) || (c == '.' && isDigit(peek(1)))) {
            scanNumber();
        }
        // سطر جديد
        else if (c == '\n' || c == '\r') {
            scanNewline();
        }
        // مسافة
        else if (c == ' ' || c == '\t') {
            scanWhitespace();
        }
        // فاصلة عربية ،
        else if (static_cast<unsigned char>(c) == 0xD8 &&
                 pos_+1 < source_.size() &&
                 static_cast<unsigned char>(source_[pos_+1]) == 0x8C) {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance(2);
            emit(FmtTokenType::COMMA, "\xD8\x8C", sl, sc, so);
        }
        // فاصلة منقوطة عربية ؛
        else if (static_cast<unsigned char>(c) == 0xD8 &&
                 pos_+1 < source_.size() &&
                 static_cast<unsigned char>(source_[pos_+1]) == 0x9B) {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance(2);
            emit(FmtTokenType::SEMICOLON, "\xD8\x9B", sl, sc, so);
        }
        // فاصلة لاتينية
        else if (c == ',') {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance();
            emit(FmtTokenType::COMMA, ",", sl, sc, so);
        }
        // فاصلة منقوطة لاتينية
        else if (c == ';') {
            int sl = line_, sc = column_; int so = (int)pos_;
            advance();
            emit(FmtTokenType::SEMICOLON, ";", sl, sc, so);
        }
        // معرّف (عربي أو لاتيني)
        else if (isIdentStart(c)) {
            scanIdentifier();
        }
        // عامل
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
    // raw strings: r"..." — handled via normal scan with prefix
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
        // لا تستهلك الفاصلة العربية ، أو الفاصلة المنقوطة العربية ؛
        unsigned char uc = static_cast<unsigned char>(current());
        if (uc == 0xD8 && pos_ + 1 < source_.size()) {
            unsigned char nc = static_cast<unsigned char>(source_[pos_ + 1]);
            if (nc == 0x8C || nc == 0x9B) break;  // ، or ؛
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
    // محاولة مطابقة أطول عامل
    std::string best;
    for (const auto& op : SAD_OPERATORS_SORTED) {
        if (match(op) && op.size() > best.size()) {
            best = op;
        }
    }

    if (best == "->") {
        advance((int)best.size());
        emit(FmtTokenType::ARROW, best, sl, sc, so);
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
        // علامة ترقيم
        std::string val(1, current());
        advance();
        emit(FmtTokenType::PUNCTUATION, val, sl, sc, so);
    }
}

// ============================================================================
// FormatterOptions — تحميل/حفظ
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
        bool bval = (val == "true" || val == "نعم" || val == "1");

        if (key == "indent_size" || key == "حجم_المسافة") opts.indentSize = std::stoi(val);
        else if (key == "use_spaces" || key == "استخدم_مسافات") opts.useSpaces = bval;
        else if (key == "max_line_length" || key == "حد_طول_السطر") opts.maxLineLength = std::stoi(val);
        else if (key == "wrap_long_lines" || key == "لف_الأسطر") opts.wrapLongLines = bval;
        else if (key == "space_after_comma" || key == "مسافة_بعد_الفاصلة") opts.spaceAfterComma = bval;
        else if (key == "space_around_operators" || key == "مسافة_حول_العوامل") opts.spaceAroundOperators = bval;
        else if (key == "space_after_keywords" || key == "مسافة_بعد_المفتاحية") opts.spaceAfterKeywords = bval;
        else if (key == "space_after_colon" || key == "مسافة_بعد_النقطتين") opts.spaceAfterColon = bval;
        else if (key == "space_before_colon" || key == "مسافة_قبل_النقطتين") opts.spaceBeforeColon = bval;
        else if (key == "space_inside_parens" || key == "مسافة_داخل_الأقواس") opts.spaceInsideParens = bval;
        else if (key == "space_around_arrow" || key == "مسافة_حول_السهم") opts.spaceAroundArrow = bval;
        else if (key == "blank_lines_after_imports") opts.blankLinesAfterImports = std::stoi(val);
        else if (key == "blank_lines_between_functions") opts.blankLinesBetweenFunctions = std::stoi(val);
        else if (key == "blank_lines_between_classes") opts.blankLinesBetweenClasses = std::stoi(val);
        else if (key == "max_consecutive_blank_lines") opts.maxConsecutiveBlankLines = std::stoi(val);
        else if (key == "trim_trailing_whitespace" || key == "حذف_الزائد") opts.trimTrailingWhitespace = bval;
        else if (key == "ensure_final_newline") opts.ensureFinalNewline = bval;
        else if (key == "remove_trailing_semicolons") opts.removeTrailingSemicolons = bval;
        else if (key == "sort_imports" || key == "رتب_الاستيرادات") opts.sortImports = bval;
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
        else if (key == "profile" || key == "وضع") {
            if (val == "compact" || val == "مضغوط") opts.applyProfile(FormatProfile::Compact);
            else if (val == "verbose" || val == "مطوّل") opts.applyProfile(FormatProfile::Verbose);
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
    f << "# إعدادات منسّق كود لغة ص\n"
      << "# Sad Language Formatter Configuration\n\n"
      << "indent_size = " << indentSize << "\n"
      << "use_spaces = " << (useSpaces ? "true" : "false") << "\n"
      << "max_line_length = " << maxLineLength << "\n"
      << "wrap_long_lines = " << (wrapLongLines ? "true" : "false") << "\n\n"
      << "# المسافات\n"
      << "space_after_comma = " << (spaceAfterComma ? "true" : "false") << "\n"
      << "space_around_operators = " << (spaceAroundOperators ? "true" : "false") << "\n"
      << "space_after_keywords = " << (spaceAfterKeywords ? "true" : "false") << "\n"
      << "space_after_colon = " << (spaceAfterColon ? "true" : "false") << "\n"
      << "space_around_arrow = " << (spaceAroundArrow ? "true" : "false") << "\n\n"
      << "# الأسطر الفارغة\n"
      << "blank_lines_between_functions = " << blankLinesBetweenFunctions << "\n"
      << "blank_lines_between_classes = " << blankLinesBetweenClasses << "\n"
      << "max_consecutive_blank_lines = " << maxConsecutiveBlankLines << "\n\n"
      << "# التنظيف\n"
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

        // مرحلة 1: ترتيب الاستيرادات
        std::string processed = normalized;
        if (options_.sortImports) {
            processed = sortAndGroupImports(processed);
        }

        // مرحلة 2: التقسيم إلى رموز
        FmtLexer lexer;
        auto tokens = lexer.tokenize(processed);

        // مرحلة 3: إعادة البناء مع التنسيق
        result.output = rebuild(tokens);

        // مرحلة 4: لفّ الأسطر الطويلة
        if (options_.wrapLongLines) {
            auto lines = splitLines(result.output);
            // نحسب مستوى المسافة لكل سطر
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

        // مرحلة 5: تنسيق نهاية الملف
        if (options_.ensureFinalNewline && !result.output.empty() && result.output.back() != '\n') {
            result.output += "\n";
        }

        // مرحلة 6: تطبيق نمط نهاية السطر
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

        // حساب التغييرات
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
        result.error = std::string("خطأ في التنسيق: ") + e.what();
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
    return result;
}

FormatResult SadFormatter::formatFile(const std::string& path, bool dryRun) {
    FormatResult result;

    std::ifstream input(path);
    if (!input.is_open()) {
        result.error = "لا يمكن فتح الملف: " + path;
        return result;
    }
    std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    result = format(source);

    if (result.success() && result.changed && !dryRun) {
        std::ofstream output(path);
        if (!output.is_open()) {
            result.error = "لا يمكن كتابة الملف: " + path;
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
            if (p.extension().string() == ".ص" || p.extension().string() == ".sad") {
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
        stats.errors.push_back(std::string("خطأ في قراءة المجلد: ") + e.what());
    }

    return stats;
}

FormatResult SadFormatter::check(const std::string& source) {
    auto result = format(source);
    // في وضع الفحص لا نعيد الكود المعدّل
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
    out << "--- " << filename << " (الأصل)\n"
        << "+++ " << filename << " (المنسّق)\n";

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
    std::vector<std::string> configNames = {".تنسيق", ".sad-fmt", ".sadformat", "تنسيق.cfg"};
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
// rebuild — إعادة بناء الكود من الرموز
// ============================================================================

std::string SadFormatter::rebuild(const std::vector<FmtToken>& tokens) {
    std::ostringstream out;
    int indentLevel = 0;
    bool startOfLine = true;
    int consecutiveBlankLines = 0;
    bool lastWasNewline = false;
    bool inImportBlock = false;

    const FmtToken* prev = nullptr;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& tok = tokens[i];
        const FmtToken* next = (i + 1 < tokens.size()) ? &tokens[i + 1] : nullptr;

        switch (tok.type) {
        case FmtTokenType::NEWLINE: {
            if (lastWasNewline) {
                consecutiveBlankLines++;
                if (consecutiveBlankLines > options_.maxConsecutiveBlankLines) {
                    prev = &tok;
                    continue;
                }
            } else {
                consecutiveBlankLines = 0;
            }

            if (options_.trimTrailingWhitespace) {
                std::string s = out.str();
                while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.pop_back();
                out.str(s); out.seekp(0, std::ios_base::end);
            }

            out << "\n";
            startOfLine = true;
            lastWasNewline = true;
            break;
        }

        case FmtTokenType::WHITESPACE:
            // تجاهل المسافات في بداية السطر — سنضيف مسافة بادئة
            if (!startOfLine) {
                // مسافة واحدة فقط
                out << " ";
            }
            break;

        case FmtTokenType::KEYWORD:
        case FmtTokenType::IMPORT_KEYWORD: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;

            if (tok.type == FmtTokenType::IMPORT_KEYWORD) inImportBlock = true;

            // المسافة البادئة والمستوى
            if (isInterBlock(tok.value)) {
                // وإلا / امسك / عندما — مستوى أقل مؤقتاً
                if (startOfLine) {
                    out << getIndent(std::max(0, indentLevel - 1));
                    startOfLine = false;
                }
                out << tok.value;
            }
            else if (isBlockCloser(tok.value)) {
                // نهاية
                indentLevel = std::max(0, indentLevel - 1);
                if (startOfLine) {
                    out << getIndent(indentLevel);
                    startOfLine = false;
                }
                out << tok.value;
            }
            else {
                if (startOfLine) {
                    out << getIndent(indentLevel);
                    startOfLine = false;
                }
                out << tok.value;
                if (isBlockOpener(tok.value)) {
                    indentLevel++;
                }
            }

            // مسافة بعد الكلمة المفتاحية
            if (options_.spaceAfterKeywords && next &&
                next->type != FmtTokenType::NEWLINE &&
                next->type != FmtTokenType::END_OF_FILE &&
                next->type != FmtTokenType::SEMICOLON) {
                // لا تضف مسافة إذا كان التالي قوس فتح ولغة ص لا تحتاجه
                if (next->value != "(" || options_.spaceBeforeParenInDef ||
                    !isBlockOpener(tok.value)) {
                    out << " ";
                }
            }
            break;
        }

        case FmtTokenType::IDENTIFIER:
        case FmtTokenType::NUMBER:
        case FmtTokenType::STRING: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) {
                out << getIndent(indentLevel);
                startOfLine = false;
            }
            out << tok.value;
            break;
        }

        case FmtTokenType::OPERATOR: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) {
                out << getIndent(indentLevel);
                startOfLine = false;
            }

            bool spaceBefore = options_.spaceAroundOperators;
            bool spaceAfter = options_.spaceAroundOperators;

            // سالب أحادي
            if ((tok.value == "-" || tok.value == "+") && prev &&
                (prev->type == FmtTokenType::OPERATOR ||
                 prev->value == "(" || prev->value == "," ||
                 prev->value == "\xD8\x8C" ||
                 prev->type == FmtTokenType::KEYWORD ||
                 prev->type == FmtTokenType::COMMA)) {
                spaceBefore = true;
                spaceAfter = false;
            }
            // ! أحادي
            if (tok.value == "!" || tok.value == "~") {
                spaceBefore = true;
                spaceAfter = false;
            }

            if (spaceBefore && prev &&
                prev->type != FmtTokenType::WHITESPACE &&
                prev->type != FmtTokenType::NEWLINE) {
                out << " ";
            }
            out << tok.value;
            if (spaceAfter && next &&
                next->type != FmtTokenType::WHITESPACE &&
                next->type != FmtTokenType::NEWLINE &&
                next->type != FmtTokenType::END_OF_FILE) {
                out << " ";
            }
            break;
        }

        case FmtTokenType::ARROW: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }

            if (options_.spaceAroundArrow && prev &&
                prev->type != FmtTokenType::WHITESPACE) out << " ";
            out << "->";
            if (options_.spaceAroundArrow && next &&
                next->type != FmtTokenType::WHITESPACE &&
                next->type != FmtTokenType::NEWLINE) out << " ";
            break;
        }

        case FmtTokenType::COLON: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }

            if (options_.spaceBeforeColon && prev &&
                prev->type != FmtTokenType::WHITESPACE) out << " ";
            out << ":";
            if (options_.spaceAfterColon && next &&
                next->type != FmtTokenType::WHITESPACE &&
                next->type != FmtTokenType::NEWLINE) out << " ";
            break;
        }

        case FmtTokenType::DOT: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }
            out << ".";
            break;
        }

        case FmtTokenType::COMMA: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }

            std::string commaChar = tok.value;
            if (options_.normalizeArabicComma && commaChar == "\xD8\x8C") commaChar = ",";
            out << commaChar;
            if (options_.spaceAfterComma && next &&
                next->type != FmtTokenType::NEWLINE &&
                next->type != FmtTokenType::END_OF_FILE) {
                out << " ";
            }
            break;
        }

        case FmtTokenType::SEMICOLON: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }
            if (!options_.removeTrailingSemicolons || (next && next->type != FmtTokenType::NEWLINE)) {
                std::string sc = tok.value;
                if (options_.normalizeSemicolon && sc == "\xD8\x9B") sc = ";";
                out << sc;
            }
            break;
        }

        case FmtTokenType::PUNCTUATION: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }

            if (tok.value == "(") {
                out << "(";
                if (options_.spaceInsideParens && next && next->value != ")") out << " ";
            }
            else if (tok.value == ")") {
                if (options_.spaceInsideParens && prev && prev->value != "(") out << " ";
                out << ")";
            }
            else if (tok.value == "[") {
                out << "[";
                if (options_.spaceInsideBrackets && next && next->value != "]") out << " ";
            }
            else if (tok.value == "]") {
                if (options_.spaceInsideBrackets && prev && prev->value != "[") out << " ";
                out << "]";
            }
            else {
                out << tok.value;
            }
            break;
        }

        case FmtTokenType::COMMENT_LINE:
        case FmtTokenType::COMMENT_BLOCK:
        case FmtTokenType::COMMENT_DOC: {
            lastWasNewline = false;
            consecutiveBlankLines = 0;
            if (startOfLine) { out << getIndent(indentLevel); startOfLine = false; }
            out << tok.value;
            break;
        }

        case FmtTokenType::END_OF_FILE:
            break;
        }

        prev = &tok;
    }

    return out.str();
}

// ============================================================================
// ترتيب الاستيرادات
// ============================================================================

bool SadFormatter::isImportLine(const std::string& line) const {
    std::string trimmed = line;
    size_t start = trimmed.find_first_not_of(" \t");
    if (start == std::string::npos) return false;
    trimmed = trimmed.substr(start);
    return trimmed.find("استورد") == 0;
}

std::string SadFormatter::sortAndGroupImports(const std::string& source) const {
    auto lines = splitLines(source);
    std::vector<ImportLine> imports;
    size_t importStart = std::string::npos;
    size_t importEnd = 0;

    for (size_t i = 0; i < lines.size(); i++) {
        if (isImportLine(lines[i])) {
            if (importStart == std::string::npos) importStart = i;
            importEnd = i;
            ImportLine il;
            il.text = lines[i];
            il.originalOrder = (int)i;
            // مفتاح الترتيب: اسم الوحدة بعد "استورد"
            size_t pos = lines[i].find("استورد");
            if (pos != std::string::npos) {
                std::string rest = lines[i].substr(pos);
                // تخطي كلمة "استورد"
                size_t ws = rest.find(' ');
                il.sortKey = (ws != std::string::npos) ? rest.substr(ws) : rest;
            } else {
                il.sortKey = lines[i];
            }
            imports.push_back(il);
        }
    }

    if (imports.size() < 2) return source;

    std::stable_sort(imports.begin(), imports.end(),
        [](const ImportLine& a, const ImportLine& b) {
            return a.sortKey < b.sortKey;
        });

    // إعادة بناء
    std::vector<std::string> result;
    for (size_t i = 0; i < importStart; i++) result.push_back(lines[i]);
    for (const auto& imp : imports) result.push_back(imp.text);
    // أسطر فارغة بعد الاستيرادات
    bool hasFollowing = importEnd + 1 < lines.size();
    if (hasFollowing) {
        size_t nextNonEmpty = importEnd + 1;
        while (nextNonEmpty < lines.size() && lines[nextNonEmpty].find_first_not_of(" \t\r\n") == std::string::npos) {
            nextNonEmpty++;
        }
        for (int b = 0; b < options_.blankLinesAfterImports; b++) result.emplace_back("");
        for (size_t i = nextNonEmpty; i < lines.size(); i++) result.push_back(lines[i]);
    }

    return joinLines(result);
}

// ============================================================================
// لفّ الأسطر
// ============================================================================

std::string SadFormatter::wrapLine(const std::string& line, int currentIndent) const {
    if (lineLength(line) <= options_.maxLineLength) return line;

    int breakAt = findBreakPoint(line, options_.maxLineLength);
    if (breakAt <= 0 || breakAt >= (int)line.size() - 1) return line;

    std::string firstPart = line.substr(0, breakAt);
    std::string rest = line.substr(breakAt);

    // حذف مسافة بداية الجزء الثاني
    size_t rStart = rest.find_first_not_of(" \t");
    if (rStart != std::string::npos) rest = rest.substr(rStart);

    std::string wrapIndent(currentIndent + options_.wrapIndent, ' ');
    std::string secondLine = wrapIndent + rest;

    // لفّ الجزء الثاني إذا طويل أيضاً
    if (lineLength(secondLine) > options_.maxLineLength) {
        secondLine = wrapLine(secondLine, currentIndent + options_.wrapIndent);
    }

    return firstPart + "\n" + secondLine;
}

int SadFormatter::findBreakPoint(const std::string& line, int maxCol) const {
    int bestBreak = -1;

    // أولوية الكسر: بعد الفاصلة، ثم قبل العامل، ثم بعد قوس
    for (int i = maxCol; i > maxCol / 3; i--) {
        if (i < 0 || i >= (int)line.size()) continue;
        char c = line[i];
        if (c == ',' || c == ';') return i + 1;
    }
    for (int i = maxCol; i > maxCol / 3; i--) {
        if (i < 0 || i >= (int)line.size()) continue;
        char c = line[i];
        if (c == '+' || c == '-' || c == '|' || c == '&') {
            if (options_.breakAfterOperator) return i + 1;
            else return i;
        }
    }
    for (int i = maxCol; i > maxCol / 3; i--) {
        if (i < 0 || i >= (int)line.size()) continue;
        if (line[i] == ' ' || line[i] == '\t') bestBreak = i;
    }

    return bestBreak;
}

// ============================================================================
// المساعدات
// ============================================================================

std::string SadFormatter::getIndent(int level) const {
    if (level <= 0) return "";
    if (options_.useSpaces) return std::string(level * options_.indentSize, ' ');
    return std::string(level, '\t');
}

std::string SadFormatter::trimRight(const std::string& s) const {
    size_t end = s.find_last_not_of(" \t\r");
    return (end != std::string::npos) ? s.substr(0, end + 1) : "";
}

std::string SadFormatter::normalizeLineEndings(const std::string& s) const {
    std::string result;
    result.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '\r') {
            result += '\n';
            if (i + 1 < s.size() && s[i + 1] == '\n') i++;
        } else {
            result += s[i];
        }
    }
    return result;
}

std::vector<std::string> SadFormatter::splitLines(const std::string& s) const {
    std::vector<std::string> lines;
    std::istringstream stream(s);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

std::string SadFormatter::joinLines(const std::vector<std::string>& lines) const {
    std::ostringstream out;
    for (size_t i = 0; i < lines.size(); i++) {
        out << lines[i];
        if (i < lines.size() - 1) out << "\n";
    }
    return out.str();
}

int SadFormatter::lineLength(const std::string& line) const {
    int len = 0;
    for (size_t i = 0; i < line.size(); i++) {
        unsigned char c = static_cast<unsigned char>(line[i]);
        if (c < 0x80) {
            len++;
        } else if (c < 0xC0) {
            // continuation byte — don't count
        } else if (c < 0xE0) {
            len++; // 2-byte character
        } else if (c < 0xF0) {
            len++; // 3-byte character
        } else {
            len++; // 4-byte character
        }
    }
    return len;
}

// ============================================================================
// CLI — نقطة دخول سطر الأوامر
// ============================================================================

int formatter_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "╔══════════════════════════════════════════╗\n"
                  << "║  ⚡ منسّق كود لغة ص — sad-fmt            ║\n"
                  << "╚══════════════════════════════════════════╝\n\n"
                  << "الاستخدام:\n"
                  << "  sad-fmt <ملف.ص>              تنسيق ملف\n"
                  << "  sad-fmt <مجلد>               تنسيق مجلد\n"
                  << "  sad-fmt --check <ملف>        فحص بدون تعديل\n"
                  << "  sad-fmt --diff <ملف>         عرض الفروقات\n"
                  << "  sad-fmt --stdin              القراءة من stdin\n"
                  << "  sad-fmt --config <ملف>       ملف إعدادات\n"
                  << "  sad-fmt --init               إنشاء ملف إعدادات\n"
                  << "  sad-fmt --compact <ملف>      وضع مضغوط\n"
                  << "  sad-fmt --verbose <ملف>      وضع مطوّل\n"
                  << "  sad-fmt --dry-run <مجلد>     محاكاة بدون كتابة\n\n";
        return 1;
    }

    FormatterOptions options;
    std::string path;
    bool checkOnly = false, showDiff = false, fromStdin = false, dryRun = false, initConfig = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--check") checkOnly = true;
        else if (arg == "--diff") showDiff = true;
        else if (arg == "--stdin") fromStdin = true;
        else if (arg == "--dry-run") dryRun = true;
        else if (arg == "--init") initConfig = true;
        else if (arg == "--compact") options.applyProfile(FormatProfile::Compact);
        else if (arg == "--verbose") options.applyProfile(FormatProfile::Verbose);
        else if (arg == "--config" && i + 1 < argc) {
            options = FormatterOptions::fromFile(argv[++i]);
        }
        else path = arg;
    }

    SadFormatter formatter(options);

    if (initConfig) {
        std::string cfgPath = path.empty() ? ".sad-fmt" : path;
        options.saveToFile(cfgPath);
        std::cout << "تم إنشاء ملف الإعدادات: " << cfgPath << "\n";
        return 0;
    }

    if (fromStdin) {
        std::ostringstream buf;
        buf << std::cin.rdbuf();
        auto result = formatter.format(buf.str());
        if (result.success()) { std::cout << result.output; return 0; }
        std::cerr << result.error << "\n"; return 1;
    }

    if (path.empty()) { std::cerr << "خطأ: يجب تحديد ملف أو مجلد\n"; return 1; }

    // تحميل إعدادات محلية
    if (fs::is_regular_file(path)) {
        formatter.loadConfigFromDirectory(fs::path(path).parent_path().string());
    } else if (fs::is_directory(path)) {
        formatter.loadConfigFromDirectory(path);
    }

    if (fs::is_directory(path)) {
        auto stats = formatter.formatDirectory(path, true, dryRun || checkOnly);
        std::cout << "═══════════════════════════════════════\n";
        std::cout << "  ملفات: " << stats.filesProcessed << " | تغيّر: " << stats.filesChanged
                  << " | أسطر: " << stats.totalLinesChanged << "\n";
        if (dryRun) std::cout << "  (محاكاة — لم تُحفظ تغييرات)\n";
        for (const auto& f : stats.changedFiles) std::cout << "  ✓ " << f << "\n";
        for (const auto& e : stats.errors) std::cerr << "  ✗ " << e << "\n";
        std::cout << "═══════════════════════════════════════\n";
        return stats.errors.empty() ? 0 : 1;
    }

    if (fs::is_regular_file(path)) {
        if (checkOnly) {
            std::ifstream file(path);
            std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            auto result = formatter.check(source);
            if (result.changed) {
                std::cout << "❌ يحتاج تنسيق: " << path << "\n";
                if (showDiff && !result.diffs.empty()) std::cout << result.diffs[0] << "\n";
                return 1;
            }
            std::cout << "✓ منسّق بشكل صحيح: " << path << "\n";
            return 0;
        }

        if (showDiff) {
            std::ifstream file(path);
            std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            auto result = formatter.format(source);
            if (result.changed) std::cout << formatter.diff(source, result.output, path) << "\n";
            else std::cout << "لا توجد تغييرات\n";
            return 0;
        }

        auto result = formatter.formatFile(path, dryRun);
        if (result.success()) {
            if (result.changed) {
                std::cout << "✓ تم تنسيق: " << path << " (" << result.linesChanged << " سطر)\n";
            } else {
                std::cout << "— لا تغييرات: " << path << "\n";
            }
            return 0;
        }
        std::cerr << "✗ " << result.error << "\n";
        return 1;
    }

    std::cerr << "خطأ: الملف غير موجود: " << path << "\n";
    return 1;
}

} // namespace Format
} // namespace Sad
