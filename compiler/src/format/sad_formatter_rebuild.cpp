// ============================================================================
// sad_formatter_rebuild.cpp — إعادة بناء الملف المنسق ودوال مساعدة
// (AR) إعادة بناء النص من الرموز، ترتيب الاستيرادات، التفاف الأسطر
// (EN) Rebuild formatted text from tokens, import sorting, line wrapping
// تم استخراج هذا الملف من sad_formatter.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "format/sad_formatter.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace Sad
{
    namespace Format
    {

        // (AR) دوال مساعدة ثابتة — منسوخة من sad_formatter.cpp (نفس الملف الأصلي)
        // (EN) Static helper functions — copied from sad_formatter.cpp (same original file)
        static bool isBlockOpener(const std::string &s)
        {
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

        static bool isBlockCloser(const std::string &s)
        {
            return s == "†‡״§״©" || s == "†‡״§״©_״§״³״×״®״¯״§…" || s == "†‡״§״©_״¶״§״¡";
        }

        static bool isInterBlock(const std::string &s)
        {
            return s == "ˆ״¥„״§" || s == "ˆ״§„״§" || s == "ˆ״§„״§_״§״°״§" || s == "ˆ״¥„״§_״¥״°״§" ||
                   s == "״§…״³ƒ" || s == "״£״®״±״§‹" ||
                   s == "״¹†״¯…״§" || s == "״§״×״±״§״¶";
        }
        std::string SadFormatter::rebuild(const std::vector<FmtToken> &tokens)
        {
            std::ostringstream out;
            int indentLevel = 0;
            bool startOfLine = true;
            int consecutiveBlankLines = 0;
            bool lastWasNewline = false;
            bool inImportBlock = false;

            const FmtToken *prev = nullptr;

            for (size_t i = 0; i < tokens.size(); ++i)
            {
                const auto &tok = tokens[i];
                const FmtToken *next = (i + 1 < tokens.size()) ? &tokens[i + 1] : nullptr;

                switch (tok.type)
                {
                case FmtTokenType::NEWLINE:
                {
                    if (lastWasNewline)
                    {
                        consecutiveBlankLines++;
                        if (consecutiveBlankLines > options_.maxConsecutiveBlankLines)
                        {
                            prev = &tok;
                            continue;
                        }
                    }
                    else
                    {
                        consecutiveBlankLines = 0;
                    }

                    if (options_.trimTrailingWhitespace)
                    {
                        std::string s = out.str();
                        while (!s.empty() && (s.back() == ' ' || s.back() == '\t'))
                            s.pop_back();
                        out.str(s);
                        out.seekp(0, std::ios_base::end);
                    }

                    out << "\n";
                    startOfLine = true;
                    lastWasNewline = true;
                    break;
                }

                case FmtTokenType::WHITESPACE:
                    // ״×״¬״§‡„ ״§„…״³״§״§״×  ״¨״¯״§״© ״§„״³״·״± ג€” ״³†״¶ …״³״§״© ״¨״§״¯״¦״©
                    if (!startOfLine)
                    {
                        // …״³״§״© ˆ״§״­״¯״© ‚״·
                        out << " ";
                    }
                    break;

                case FmtTokenType::KEYWORD:
                case FmtTokenType::IMPORT_KEYWORD:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;

                    if (tok.type == FmtTokenType::IMPORT_KEYWORD)
                        inImportBlock = true;

                    // ״§„…״³״§״© ״§„״¨״§״¯״¦״© ˆ״§„…״³״×ˆ‰
                    if (isInterBlock(tok.value))
                    {
                        // ˆ״¥„״§ / ״§…״³ƒ / ״¹†״¯…״§ ג€” …״³״×ˆ‰ ״£‚„ …״₪‚״×״§‹
                        if (startOfLine)
                        {
                            out << getIndent(std::max(0, indentLevel - 1));
                            startOfLine = false;
                        }
                        out << tok.value;
                    }
                    else if (isBlockCloser(tok.value))
                    {
                        // †‡״§״©
                        indentLevel = std::max(0, indentLevel - 1);
                        if (startOfLine)
                        {
                            out << getIndent(indentLevel);
                            startOfLine = false;
                        }
                        out << tok.value;
                    }
                    else
                    {
                        if (startOfLine)
                        {
                            out << getIndent(indentLevel);
                            startOfLine = false;
                        }
                        out << tok.value;
                        if (isBlockOpener(tok.value))
                        {
                            indentLevel++;
                        }
                    }

                    // …״³״§״© ״¨״¹״¯ ״§„ƒ„…״© ״§„…״×״§״­״©
                    if (options_.spaceAfterKeywords && next &&
                        next->type != FmtTokenType::NEWLINE &&
                        next->type != FmtTokenType::END_OF_FILE &&
                        next->type != FmtTokenType::SEMICOLON)
                    {
                        // „״§ ״×״¶ …״³״§״© ״¥״°״§ ƒ״§† ״§„״×״§„ ‚ˆ״³ ״×״­ ˆ„״÷״© ״µ „״§ ״×״­״×״§״¬‡
                        if (next->value != "(" || options_.spaceBeforeParenInDef ||
                            !isBlockOpener(tok.value))
                        {
                            out << " ";
                        }
                    }
                    break;
                }

                case FmtTokenType::IDENTIFIER:
                case FmtTokenType::NUMBER:
                case FmtTokenType::STRING:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }
                    out << tok.value;
                    break;
                }

                case FmtTokenType::OPERATOR:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }

                    bool spaceBefore = options_.spaceAroundOperators;
                    bool spaceAfter = options_.spaceAroundOperators;

                    // ״³״§„״¨ ״£״­״§״¯
                    if ((tok.value == "-" || tok.value == "+") && prev &&
                        (prev->type == FmtTokenType::OPERATOR ||
                         prev->value == "(" || prev->value == "," ||
                         prev->value == "\xD8\x8C" ||
                         prev->type == FmtTokenType::KEYWORD ||
                         prev->type == FmtTokenType::COMMA))
                    {
                        spaceBefore = true;
                        spaceAfter = false;
                    }
                    // ! ״£״­״§״¯
                    if (tok.value == "!" || tok.value == "~")
                    {
                        spaceBefore = true;
                        spaceAfter = false;
                    }

                    if (spaceBefore && prev &&
                        prev->type != FmtTokenType::WHITESPACE &&
                        prev->type != FmtTokenType::NEWLINE)
                    {
                        out << " ";
                    }
                    out << tok.value;
                    if (spaceAfter && next &&
                        next->type != FmtTokenType::WHITESPACE &&
                        next->type != FmtTokenType::NEWLINE &&
                        next->type != FmtTokenType::END_OF_FILE)
                    {
                        out << " ";
                    }
                    break;
                }

                case FmtTokenType::ARROW:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }

                    if (options_.spaceAroundArrow && prev &&
                        prev->type != FmtTokenType::WHITESPACE)
                        out << " ";
                    out << "->";
                    if (options_.spaceAroundArrow && next &&
                        next->type != FmtTokenType::WHITESPACE &&
                        next->type != FmtTokenType::NEWLINE)
                        out << " ";
                    break;
                }

                case FmtTokenType::COLON:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }

                    if (options_.spaceBeforeColon && prev &&
                        prev->type != FmtTokenType::WHITESPACE)
                        out << " ";
                    out << ":";
                    if (options_.spaceAfterColon && next &&
                        next->type != FmtTokenType::WHITESPACE &&
                        next->type != FmtTokenType::NEWLINE)
                        out << " ";
                    break;
                }

                case FmtTokenType::DOT:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }
                    out << ".";
                    break;
                }

                case FmtTokenType::COMMA:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }

                    std::string commaChar = tok.value;
                    if (options_.normalizeArabicComma && commaChar == "\xD8\x8C")
                        commaChar = ",";
                    out << commaChar;
                    if (options_.spaceAfterComma && next &&
                        next->type != FmtTokenType::NEWLINE &&
                        next->type != FmtTokenType::END_OF_FILE)
                    {
                        out << " ";
                    }
                    break;
                }

                case FmtTokenType::SEMICOLON:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }
                    if (!options_.removeTrailingSemicolons || (next && next->type != FmtTokenType::NEWLINE))
                    {
                        std::string sc = tok.value;
                        if (options_.normalizeSemicolon && sc == "\xD8\x9B")
                            sc = ";";
                        out << sc;
                    }
                    break;
                }

                case FmtTokenType::PUNCTUATION:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }

                    if (tok.value == "(")
                    {
                        out << "(";
                        if (options_.spaceInsideParens && next && next->value != ")")
                            out << " ";
                    }
                    else if (tok.value == ")")
                    {
                        if (options_.spaceInsideParens && prev && prev->value != "(")
                            out << " ";
                        out << ")";
                    }
                    else if (tok.value == "[")
                    {
                        out << "[";
                        if (options_.spaceInsideBrackets && next && next->value != "]")
                            out << " ";
                    }
                    else if (tok.value == "]")
                    {
                        if (options_.spaceInsideBrackets && prev && prev->value != "[")
                            out << " ";
                        out << "]";
                    }
                    else
                    {
                        out << tok.value;
                    }
                    break;
                }

                case FmtTokenType::COMMENT_LINE:
                case FmtTokenType::COMMENT_BLOCK:
                case FmtTokenType::COMMENT_DOC:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }
                    out << tok.value;
                    break;
                }

                case FmtTokenType::DIRECTIVE:
                {
                    lastWasNewline = false;
                    consecutiveBlankLines = 0;
                    if (startOfLine)
                    {
                        out << getIndent(indentLevel);
                        startOfLine = false;
                    }
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
        // ״×״±״×״¨ ״§„״§״³״×״±״§״¯״§״×
        // ============================================================================

        bool SadFormatter::isImportLine(const std::string &line) const
        {
            std::string trimmed = line;
            size_t start = trimmed.find_first_not_of(" \t");
            if (start == std::string::npos)
                return false;
            trimmed = trimmed.substr(start);
            return trimmed.find("״§״³״×ˆ״±״¯") == 0;
        }

        std::string SadFormatter::sortAndGroupImports(const std::string &source) const
        {
            auto lines = splitLines(source);
            std::vector<ImportLine> imports;
            size_t importStart = std::string::npos;
            size_t importEnd = 0;

            for (size_t i = 0; i < lines.size(); i++)
            {
                if (isImportLine(lines[i]))
                {
                    if (importStart == std::string::npos)
                        importStart = i;
                    importEnd = i;
                    ImportLine il;
                    il.text = lines[i];
                    il.originalOrder = (int)i;
                    // …״×״§״­ ״§„״×״±״×״¨: ״§״³… ״§„ˆ״­״¯״© ״¨״¹״¯ "״§״³״×ˆ״±״¯"
                    size_t pos = lines[i].find("״§״³״×ˆ״±״¯");
                    if (pos != std::string::npos)
                    {
                        std::string rest = lines[i].substr(pos);
                        // ״×״®״· ƒ„…״© "״§״³״×ˆ״±״¯"
                        size_t ws = rest.find(' ');
                        il.sortKey = (ws != std::string::npos) ? rest.substr(ws) : rest;
                    }
                    else
                    {
                        il.sortKey = lines[i];
                    }
                    imports.push_back(il);
                }
            }

            if (imports.size() < 2)
                return source;

            std::stable_sort(imports.begin(), imports.end(),
                             [](const ImportLine &a, const ImportLine &b)
                             {
                                 return a.sortKey < b.sortKey;
                             });

            // ״¥״¹״§״¯״© ״¨†״§״¡
            std::vector<std::string> result;
            for (size_t i = 0; i < importStart; i++)
                result.push_back(lines[i]);
            for (const auto &imp : imports)
                result.push_back(imp.text);
            // ״£״³״·״± ״§״±״÷״© ״¨״¹״¯ ״§„״§״³״×״±״§״¯״§״×
            bool hasFollowing = importEnd + 1 < lines.size();
            if (hasFollowing)
            {
                size_t nextNonEmpty = importEnd + 1;
                while (nextNonEmpty < lines.size() && lines[nextNonEmpty].find_first_not_of(" \t\r\n") == std::string::npos)
                {
                    nextNonEmpty++;
                }
                for (int b = 0; b < options_.blankLinesAfterImports; b++)
                    result.emplace_back("");
                for (size_t i = nextNonEmpty; i < lines.size(); i++)
                    result.push_back(lines[i]);
            }

            return joinLines(result);
        }

        // ============================================================================
        // „‘ ״§„״£״³״·״±
        // ============================================================================

        std::string SadFormatter::wrapLine(const std::string &line, int currentIndent) const
        {
            if (lineLength(line) <= options_.maxLineLength)
                return line;

            int breakAt = findBreakPoint(line, options_.maxLineLength);
            if (breakAt <= 0 || breakAt >= (int)line.size() - 1)
                return line;

            std::string firstPart = line.substr(0, breakAt);
            std::string rest = line.substr(breakAt);

            // ״­״° …״³״§״© ״¨״¯״§״© ״§„״¬״²״¡ ״§„״«״§†
            size_t rStart = rest.find_first_not_of(" \t");
            if (rStart != std::string::npos)
                rest = rest.substr(rStart);

            std::string wrapIndent(currentIndent + options_.wrapIndent, ' ');
            std::string secondLine = wrapIndent + rest;

            // „‘ ״§„״¬״²״¡ ״§„״«״§† ״¥״°״§ ״·ˆ„ ״£״¶״§‹
            if (lineLength(secondLine) > options_.maxLineLength)
            {
                secondLine = wrapLine(secondLine, currentIndent + options_.wrapIndent);
            }

            return firstPart + "\n" + secondLine;
        }

        int SadFormatter::findBreakPoint(const std::string &line, int maxCol) const
        {
            int bestBreak = -1;

            // ״£ˆ„ˆ״© ״§„ƒ״³״±: ״¨״¹״¯ ״§„״§״µ„״©״ ״«… ‚״¨„ ״§„״¹״§…„״ ״«… ״¨״¹״¯ ‚ˆ״³
            for (int i = maxCol; i > maxCol / 3; i--)
            {
                if (i < 0 || i >= (int)line.size())
                    continue;
                char c = line[i];
                if (c == ',' || c == ';')
                    return i + 1;
            }
            for (int i = maxCol; i > maxCol / 3; i--)
            {
                if (i < 0 || i >= (int)line.size())
                    continue;
                char c = line[i];
                if (c == '+' || c == '-' || c == '|' || c == '&')
                {
                    if (options_.breakAfterOperator)
                        return i + 1;
                    else
                        return i;
                }
            }
            for (int i = maxCol; i > maxCol / 3; i--)
            {
                if (i < 0 || i >= (int)line.size())
                    continue;
                if (line[i] == ' ' || line[i] == '\t')
                    bestBreak = i;
            }

            return bestBreak;
        }

        // ============================================================================
        // ״§„…״³״§״¹״¯״§״×
        // ============================================================================

        std::string SadFormatter::getIndent(int level) const
        {
            if (level <= 0)
                return "";
            if (options_.useSpaces)
                return std::string(level * options_.indentSize, ' ');
            return std::string(level, '\t');
        }

        std::string SadFormatter::trimRight(const std::string &s) const
        {
            size_t end = s.find_last_not_of(" \t\r");
            return (end != std::string::npos) ? s.substr(0, end + 1) : "";
        }

        std::string SadFormatter::normalizeLineEndings(const std::string &s) const
        {
            std::string result;
            result.reserve(s.size());
            for (size_t i = 0; i < s.size(); i++)
            {
                if (s[i] == '\r')
                {
                    result += '\n';
                    if (i + 1 < s.size() && s[i + 1] == '\n')
                        i++;
                }
                else
                {
                    result += s[i];
                }
            }
            return result;
        }

        std::vector<std::string> SadFormatter::splitLines(const std::string &s) const
        {
            std::vector<std::string> lines;
            std::istringstream stream(s);
            std::string line;
            while (std::getline(stream, line))
            {
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                lines.push_back(line);
            }
            return lines;
        }

        std::string SadFormatter::joinLines(const std::vector<std::string> &lines) const
        {
            std::ostringstream out;
            for (size_t i = 0; i < lines.size(); i++)
            {
                out << lines[i];
                if (i < lines.size() - 1)
                    out << "\n";
            }
            return out.str();
        }

        int SadFormatter::lineLength(const std::string &line) const
        {
            int len = 0;
            for (size_t i = 0; i < line.size(); i++)
            {
                unsigned char c = static_cast<unsigned char>(line[i]);
                if (c < 0x80)
                {
                    len++;
                }
                else if (c < 0xC0)
                {
                    // continuation byte ג€” don't count
                }
                else if (c < 0xE0)
                {
                    len++; // 2-byte character
                }
                else if (c < 0xF0)
                {
                    len++; // 3-byte character
                }
                else
                {
                    len++; // 4-byte character
                }
            }
            return len;
        }

        // ============================================================================
        // CLI ג€” †‚״·״© ״¯״®ˆ„ ״³״·״± ״§„״£ˆ״§…״±
        // ============================================================================

        int formatter_main(int argc, char *argv[])
        {
            // (AR) معالجة الأعلام القياسية أولاً: --version / --help
            // (EN) Standard flags first: --version / --help
            for (int i = 1; i < argc; ++i)
            {
                std::string a = argv[i];
                if (a == "--version" || a == "-v")
                {
                    std::cout << "sad-fmt version 1.0.0\n"
                              << "Sad Language Code Formatter\n";
                    return 0;
                }
                if (a == "--help" || a == "-h")
                {
                    std::cout
                        << "sad-fmt 1.0.0 - Sad Language Code Formatter\n\n"
                        << "Usage:\n"
                        << "  sad-fmt <file>             Format a single file\n"
                        << "  sad-fmt <directory>        Format a directory recursively\n"
                        << "  sad-fmt --check <path>     Check only (no write)\n"
                        << "  sad-fmt --diff <path>      Show diff\n"
                        << "  sad-fmt --stdin            Read source from stdin\n"
                        << "  sad-fmt --config <file>    Use custom config file\n"
                        << "  sad-fmt --init             Create default config file\n"
                        << "  sad-fmt --compact <path>   Apply compact profile\n"
                        << "  sad-fmt --verbose <path>   Apply verbose profile\n"
                        << "  sad-fmt --dry-run <dir>    Dry-run a directory\n"
                        << "  sad-fmt --version          Show version\n"
                        << "  sad-fmt --help             Show this help\n";
                    return 0;
                }
            }

            if (argc < 2)
            {
                std::cout << "ג•”ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•—\n"
                          << "ג•‘  ג¡ …†״³‘‚ ƒˆ״¯ „״÷״© ״µ ג€” sad-fmt            ג•‘\n"
                          << "ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•\n\n"
                          << "״§„״§״³״×״®״¯״§…:\n"
                          << "  sad-fmt <…„.״µ>              ״×†״³‚ …„\n"
                          << "  sad-fmt <…״¬„״¯>               ״×†״³‚ …״¬„״¯\n"
                          << "  sad-fmt --check <…„>        ״­״µ ״¨״¯ˆ† ״×״¹״¯„\n"
                          << "  sad-fmt --diff <…„>         ״¹״±״¶ ״§„״±ˆ‚״§״×\n"
                          << "  sad-fmt --stdin              ״§„‚״±״§״¡״© …† stdin\n"
                          << "  sad-fmt --config <…„>       …„ ״¥״¹״¯״§״¯״§״×\n"
                          << "  sad-fmt --init               ״¥†״´״§״¡ …„ ״¥״¹״¯״§״¯״§״×\n"
                          << "  sad-fmt --compact <…„>      ˆ״¶״¹ …״¶״÷ˆ״·\n"
                          << "  sad-fmt --verbose <…„>      ˆ״¶״¹ …״·ˆ‘„\n"
                          << "  sad-fmt --dry-run <…״¬„״¯>     …״­״§ƒ״§״© ״¨״¯ˆ† ƒ״×״§״¨״©\n\n";
                return 1;
            }

            FormatterOptions options;
            std::string path;
            bool checkOnly = false, showDiff = false, fromStdin = false, dryRun = false, initConfig = false;

            for (int i = 1; i < argc; ++i)
            {
                std::string arg = argv[i];
                if (arg == "--check")
                    checkOnly = true;
                else if (arg == "--diff")
                    showDiff = true;
                else if (arg == "--stdin")
                    fromStdin = true;
                else if (arg == "--dry-run")
                    dryRun = true;
                else if (arg == "--init")
                    initConfig = true;
                else if (arg == "--compact")
                    options.applyProfile(FormatProfile::Compact);
                else if (arg == "--verbose")
                    options.applyProfile(FormatProfile::Verbose);
                else if (arg == "--config" && i + 1 < argc)
                {
                    options = FormatterOptions::fromFile(argv[++i]);
                }
                else if (arg.size() > 1 && arg[0] == '-')
                {
                    // (AR) رفض الأعلام غير المعروفة بدلاً من معاملتها كمسار
                    // (EN) Reject unknown flags instead of treating them as a path
                    std::cerr << "sad-fmt: unknown option: " << arg << "\n"
                              << "Use --help for usage.\n";
                    return 1;
                }
                else
                    path = arg;
            }

            SadFormatter formatter(options);

            if (initConfig)
            {
                std::string cfgPath = path.empty() ? ".sad-fmt" : path;
                options.saveToFile(cfgPath);
                std::cout << "״×… ״¥†״´״§״¡ …„ ״§„״¥״¹״¯״§״¯״§״×: " << cfgPath << "\n";
                return 0;
            }

            if (fromStdin)
            {
                std::ostringstream buf;
                buf << std::cin.rdbuf();
                auto result = formatter.format(buf.str());
                if (result.success())
                {
                    std::cout << result.output;
                    return 0;
                }
                std::cerr << result.error << "\n";
                return 1;
            }

            if (path.empty())
            {
                std::cerr << "״®״·״£: ״¬״¨ ״×״­״¯״¯ …„ ״£ˆ …״¬„״¯\n";
                return 1;
            }

            // ״×״­…„ ״¥״¹״¯״§״¯״§״× …״­„״©
            if (fs::is_regular_file(path))
            {
                formatter.loadConfigFromDirectory(fs::path(path).parent_path().string());
            }
            else if (fs::is_directory(path))
            {
                formatter.loadConfigFromDirectory(path);
            }

            if (fs::is_directory(path))
            {
                auto stats = formatter.formatDirectory(path, true, dryRun || checkOnly);
                std::cout << "ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•\n";
                std::cout << "  …„״§״×: " << stats.filesProcessed << " | ״×״÷‘״±: " << stats.filesChanged
                          << " | ״£״³״·״±: " << stats.totalLinesChanged << "\n";
                if (dryRun)
                    std::cout << "  (…״­״§ƒ״§״© ג€” „… ״×״­״¸ ״×״÷״±״§״×)\n";
                for (const auto &f : stats.changedFiles)
                    std::cout << "  ג“ " << f << "\n";
                for (const auto &e : stats.errors)
                    std::cerr << "  ג— " << e << "\n";
                std::cout << "ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•\n";
                return stats.errors.empty() ? 0 : 1;
            }

            if (fs::is_regular_file(path))
            {
                if (checkOnly)
                {
                    std::ifstream file(path);
                    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    auto result = formatter.check(source);
                    if (result.changed)
                    {
                        std::cout << "ג ״­״×״§״¬ ״×†״³‚: " << path << "\n";
                        if (showDiff && !result.diffs.empty())
                            std::cout << result.diffs[0] << "\n";
                        return 1;
                    }
                    std::cout << "ג“ …†״³‘‚ ״¨״´ƒ„ ״µ״­״­: " << path << "\n";
                    return 0;
                }

                if (showDiff)
                {
                    std::ifstream file(path);
                    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                    auto result = formatter.format(source);
                    if (result.changed)
                        std::cout << formatter.diff(source, result.output, path) << "\n";
                    else
                        std::cout << "„״§ ״×ˆ״¬״¯ ״×״÷״±״§״×\n";
                    return 0;
                }

                auto result = formatter.formatFile(path, dryRun);
                if (result.success())
                {
                    if (result.changed)
                    {
                        std::cout << "ג“ ״×… ״×†״³‚: " << path << " (" << result.linesChanged << " ״³״·״±)\n";
                    }
                    else
                    {
                        std::cout << "ג€” „״§ ״×״÷״±״§״×: " << path << "\n";
                    }
                    return 0;
                }
                std::cerr << "ג— " << result.error << "\n";
                return 1;
            }

            std::cerr << "״®״·״£: ״§„…„ ״÷״± …ˆ״¬ˆ״¯: " << path << "\n";
            return 1;
        }

    } // namespace Format
} // namespace Sad
