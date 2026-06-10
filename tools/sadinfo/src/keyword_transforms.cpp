// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/src/keyword_transforms.cpp
// (AR) تنفيذ دوال تحويل بيانات الكلمات المفتاحية.
// (EN) Implementation of keyword data transformation functions.
// ═══════════════════════════════════════════════════════════════════════════════

#include "keyword_transforms.h"
#include "token.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <string>

namespace Sad
{
    namespace Info
    {

        // ─────────────────────────────────────────────────────────────────────
        // (AR) دوال مساعدة / (EN) Helper functions
        // ─────────────────────────────────────────────────────────────────────

        static std::string toLowerStr(std::string s)
        {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            return s;
        }

        // (AR) استبدال المسافات والشرطات بـ underscore وتحويل للأحرف الصغيرة
        static std::string toSnakeCase(const std::string &s)
        {
            std::string out;
            out.reserve(s.size());
            for (char c : s)
            {
                if (c == ' ' || c == '-')
                    out.push_back('_');
                else
                    out.push_back(std::tolower(static_cast<unsigned char>(c)));
            }
            return out;
        }

        // ─────────────────────────────────────────────────────────────────────

        bool isDeprecated(const std::string &english)
        {
            // (AR) نطابق case-insensitive اللاحقة _deprecated
            const std::string suffix = "_deprecated";
            if (english.size() <= suffix.size())
                return false;
            auto end = toLowerStr(english.substr(english.size() - suffix.size()));
            return end == suffix;
        }

        std::string deriveStableId(
            Sad::Lexer::Generated::KeywordCategory cat,
            const std::string                     &english,
            Sad::Lexer::TokenType                  tt)
        {
            using Cat = Sad::Lexer::Generated::KeywordCategory;
            using TT  = Sad::Lexer::TokenType;

            // (AR) حالة خاصة: القيم الحرفية
            if (tt == TT::LITERAL_TRUE)  return "literal.true";
            if (tt == TT::LITERAL_FALSE) return "literal.false";
            if (tt == TT::LITERAL_NULL)  return "literal.null";

            // (AR) تحويل الاسم الإنجليزي لـ snake_case
            std::string base = toSnakeCase(english);

            // (AR) إزالة لاحقة _deprecated
            const std::string depr = "_deprecated";
            if (base.size() > depr.size() &&
                base.substr(base.size() - depr.size()) == depr)
            {
                base = base.substr(0, base.size() - depr.size());
            }

            switch (cat)
            {
                case Cat::RESERVED:     return "keyword."    + base;
                case Cat::OPERATOR:     return "operator."   + base;
                case Cat::CONTEXTUAL:   return "contextual." + base;
                case Cat::BUILTIN_TYPE: return "type."       + base;
                default:                return "keyword."    + base;
            }
        }

        ProcessedKeyword processEntry(const Sad::Lexer::Generated::KeywordEntry &e)
        {
            using Cat = Sad::Lexer::Generated::KeywordCategory;

            ProcessedKeyword pk;
            pk.word           = e.primaryWord;
            pk.english        = e.english;
            pk.emittedByLexer = e.emittedByLexer;
            pk.aliases        = e.aliases;
            pk.roles          = e.roles;
            pk.deprecated     = isDeprecated(e.english);
            pk.tokenType      = static_cast<int>(e.type);

            switch (e.category)
            {
                case Cat::RESERVED:     pk.category = "reserved";     break;
                case Cat::OPERATOR:     pk.category = "operator";     break;
                case Cat::CONTEXTUAL:   pk.category = "contextual";   break;
                case Cat::BUILTIN_TYPE: pk.category = "builtin_type"; break;
            }

            pk.id = deriveStableId(e.category, e.english, e.type);
            return pk;
        }

        std::vector<ProcessedKeyword> getAllProcessed()
        {
            const auto &entries = Sad::Lexer::Generated::allEntries();
            std::vector<ProcessedKeyword> result;
            result.reserve(entries.size());
            for (const auto &e : entries)
                result.push_back(processEntry(e));
            return result;
        }

        std::vector<ProcessedKeyword> applyFilters(
            const std::vector<ProcessedKeyword> &all,
            const std::vector<ParsedFilter>     &filters,
            bool                                 excludeDeprecated)
        {
            std::vector<ProcessedKeyword> out;
            for (const auto &e : all)
            {
                if (excludeDeprecated && e.deprecated)
                    continue;

                bool pass = true;
                for (const auto &f : filters)
                {
                    if (f.key == "category" && f.hasValue)
                    {
                        if (e.category != f.value) { pass = false; break; }
                    }
                    else if (f.key == "role" && f.hasValue)
                    {
                        auto &roles = e.roles;
                        if (std::find(roles.begin(), roles.end(), f.value) == roles.end())
                        { pass = false; break; }
                    }
                    else if (f.key == "token-type" && f.hasValue)
                    {
                        if (std::to_string(e.tokenType) != f.value)
                        { pass = false; break; }
                    }
                    else if (f.key == "has-aliases" && !f.hasValue)
                    {
                        if (e.aliases.empty()) { pass = false; break; }
                    }
                    else if (f.key == "deprecated" && !f.hasValue)
                    {
                        if (!e.deprecated) { pass = false; break; }
                    }
                }

                if (pass)
                    out.push_back(e);
            }
            return out;
        }

        GroupedKeywords groupByCategory(const std::vector<ProcessedKeyword> &all)
        {
            GroupedKeywords g;
            for (const auto &e : all)
            {
                if (e.category == "reserved")     g.reserved.push_back(e);
                else if (e.category == "operator") g.op.push_back(e);
                else if (e.category == "contextual") g.contextual.push_back(e);
                else if (e.category == "builtin_type") g.builtinType.push_back(e);
            }
            return g;
        }

        // (AR) المفاتيح الصالحة للفلترة
        static const std::set<std::string> kValidKeys = {
            "category", "role", "token-type", "has-aliases", "deprecated"
        };

        // (AR) المفاتيح التي تتطلب قيمة
        static const std::set<std::string> kRequireValue = {
            "category", "role", "token-type"
        };

        bool parseFilter(const std::string &expr, ParsedFilter &out, std::string &errMsg)
        {
            if (expr.empty())
            {
                errMsg = "Empty filter expression";
                return false;
            }

            auto eq = expr.find('=');
            std::string key   = (eq == std::string::npos) ? expr : expr.substr(0, eq);
            std::string value = (eq == std::string::npos) ? ""   : expr.substr(eq + 1);
            bool hasValue     = (eq != std::string::npos);

            if (kValidKeys.find(key) == kValidKeys.end())
            {
                errMsg = "Unknown filter key: '" + key + "'";
                return false;
            }

            if (kRequireValue.count(key) && !hasValue)
            {
                errMsg = "Filter '" + key + "' requires a value (key=value)";
                return false;
            }

            out = {key, value, hasValue};
            return true;
        }

    } // namespace Info
} // namespace Sad
