// بسم الله الرحمن الرحيم
// ============================================================================
// formatter_keywords.cpp — تنفيذ تصنيف الكلمات لأدوات لغة ص (v4.1)
// Implementation of keyword classification for Sad tooling (v4.1)
// ============================================================================
//
// (AR) كل البيانات تُستمد من المعجم المولَّد من YAML
//      `language-truth/keywords.yaml` عبر `Generated::allEntries()`.
//      لا توجد سلاسل عربية مكتوبة يدوياً في هذا الملف. أي كلمة جديدة أو
//      تعديل إملاء يحدث في YAML فقط — يَنتشر تلقائياً.
//
// (EN) All data is derived from the YAML-generated lexicon
//      `language-truth/keywords.yaml` via `Generated::allEntries()`.
//      There are NO hand-written Arabic strings in this file. Any new word
//      or spelling change happens in YAML only — propagates automatically.
// ============================================================================

#include "tools_shared/formatter_keywords.h"
#include "lexer_keywords.h"
#include "keywords_generated.h"

#include <mutex>
#include <unordered_set>
#include <vector>

namespace Sad
{
    namespace ToolsShared
    {

        namespace
        {

            using ::Sad::Lexer::KeywordTable;
            using ::Sad::Lexer::Generated::KeywordCategory;
            using ::Sad::Lexer::Generated::allEntries;

            // (AR) ضمان تهيئة جدول الكلمات مرة واحدة عند أول استخدام.
            // (EN) Lazy, thread-safe initialization of the shared keyword table.
            void ensureKeywordTableReady()
            {
                static std::once_flag flag;
                std::call_once(flag, []() { KeywordTable::initialize(); });
            }

            // (AR) أضف الكلمة الرئيسية وكل الأسماء البديلة لإدخال إلى مجموعة.
            // (EN) Insert primaryWord + all aliases of an entry into a set.
            template <typename Set>
            void insertEntry(Set& s, const ::Sad::Lexer::Generated::KeywordEntry& e)
            {
                s.insert(e.primaryWord);
                for (const auto& a : e.aliases) s.insert(a);
            }

            // (AR) أضف الكلمة الرئيسية وكل الأسماء البديلة لإدخال إلى متجه.
            // (EN) Append primaryWord + all aliases of an entry into a vector.
            void appendEntry(std::vector<std::string>& v,
                             const ::Sad::Lexer::Generated::KeywordEntry& e)
            {
                v.push_back(e.primaryWord);
                for (const auto& a : e.aliases) v.push_back(a);
            }

            // (AR) قائمة الكلمات السياقية الكاملة (الرئيسية + البدائل).
            // (EN) Full contextual keyword list (primaries + aliases).
            const std::vector<std::string>& contextualKeywordsList()
            {
                static const std::vector<std::string> kw = []() {
                    ensureKeywordTableReady();
                    std::vector<std::string> out;
                    for (const auto& e : allEntries())
                    {
                        if (e.category == KeywordCategory::CONTEXTUAL)
                            appendEntry(out, e);
                    }
                    return out;
                }();
                return kw;
            }

            // (AR) قائمة أسماء الأنواع المدمجة (ليست محجوزة).
            // (EN) Built-in type names (not reserved).
            const std::vector<std::string>& builtInTypeNamesList()
            {
                static const std::vector<std::string> tn = []() {
                    ensureKeywordTableReady();
                    std::vector<std::string> out;
                    for (const auto& e : allEntries())
                    {
                        if (e.category == KeywordCategory::BUILTIN_TYPE)
                            appendEntry(out, e);
                    }
                    return out;
                }();
                return tn;
            }

            // (AR) مجموعة الكلمات التي تحمل دوراً معيناً (فاتح/مغلق/إلخ).
            // (EN) Set of words carrying a specific role (opener/closer/etc.).
            std::unordered_set<std::string> buildRoleSet(const std::string& role)
            {
                ensureKeywordTableReady();
                std::unordered_set<std::string> r;
                for (const auto& e : allEntries())
                {
                    for (const auto& er : e.roles)
                    {
                        if (er == role) { insertEntry(r, e); break; }
                    }
                }
                return r;
            }

            const std::unordered_set<std::string>& blockOpenersSet()
            {
                static const std::unordered_set<std::string> s = buildRoleSet("block_opener");
                return s;
            }

            const std::unordered_set<std::string>& blockClosersSet()
            {
                static const std::unordered_set<std::string> s = buildRoleSet("block_closer");
                return s;
            }

            const std::unordered_set<std::string>& interBlockSet()
            {
                static const std::unordered_set<std::string> s = buildRoleSet("inter_block");
                return s;
            }

            const std::unordered_set<std::string>& importKeywordsSet()
            {
                static const std::unordered_set<std::string> s = buildRoleSet("import_keyword");
                return s;
            }

            const std::unordered_set<std::string>& builtInTypesSet()
            {
                static const std::unordered_set<std::string> s = []() {
                    std::unordered_set<std::string> r;
                    for (const auto& n : builtInTypeNamesList()) r.insert(n);
                    return r;
                }();
                return s;
            }

            const std::unordered_set<std::string>& contextualSet()
            {
                static const std::unordered_set<std::string> s = []() {
                    std::unordered_set<std::string> r;
                    for (const auto& n : contextualKeywordsList()) r.insert(n);
                    return r;
                }();
                return s;
            }

        } // anonymous namespace

        // ====================================================================
        // التنفيذ العام / Public Implementation
        // ====================================================================

        bool isToolKeyword(const std::string& word)
        {
            ensureKeywordTableReady();
            if (KeywordTable::isKeyword(word)) return true;
            return contextualSet().count(word) > 0;
        }

        bool isImportKeyword(const std::string& word)
        {
            return importKeywordsSet().count(word) > 0;
        }

        bool isBlockOpenerKeyword(const std::string& word)
        {
            return blockOpenersSet().count(word) > 0;
        }

        bool isBlockCloserKeyword(const std::string& word)
        {
            return blockClosersSet().count(word) > 0;
        }

        bool isInterBlockKeyword(const std::string& word)
        {
            return interBlockSet().count(word) > 0;
        }

        bool isBuiltInTypeName(const std::string& word)
        {
            return builtInTypesSet().count(word) > 0;
        }

        const std::vector<std::string>& getContextualKeywords()
        {
            return contextualKeywordsList();
        }

        const std::vector<std::string>& getBuiltInTypeNames()
        {
            return builtInTypeNamesList();
        }

    } // namespace ToolsShared
} // namespace Sad
