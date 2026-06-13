// ============================================================================
// docs_extractor.h — مُستخرج التوثيق من شجرة AST للغة ص
// (AR) Documentation Extractor from AST tree for Sad language
// (EN) Documentation Extractor from AST tree for Sad language
// ----------------------------------------------------------------------------
// (AR) هذا المُستخرج يمشي على شجرة AST الناتجة من المحلل النحوي ويجمع
//      التعليقات التوثيقية المرتبطة فعلاً بكل تصريح (دالة / صنف / تعداد /
//      بنية / سمة) ثم يُولّد توثيقاً منظماً بصيغة Markdown.
//
//      الميزة الجوهرية: لا يقرأ الملف كنص خام (مثل tools/docgen)
//      بل يستخدم نفس مسار اللغة (Lexer → Parser → AST → docComment)
//      لذلك يضمن أن التوثيق المُستخرج مرتبط دلالياً بالعنصر الصحيح.
//
// (EN) This extractor walks the AST produced by the parser and collects
//      documentation comments actually attached to each declaration
//      (function / class / enum / struct / trait), then emits structured
//      Markdown.
//
//      Key feature: it does NOT scan the source text (unlike tools/docgen);
//      it uses the language's own pipeline (Lexer → Parser → AST → docComment),
//      guaranteeing that extracted docs are semantically attached to the
//      correct declaration.
//
// (AR) القواعد المتبعة من copilot-instructions.md:
//      - CW-01 (SRP): مهمة واحدة — استخراج توثيق وعرضه Markdown
//      - CW-08: ثنائي اللغة في APIs العامة
//      - CW-19 (DRY): دالة مساعدة واحدة لتنسيق كل تصريح
//      - CW-21: كل التنفيذ مخفي داخل namespace غير مكشوف
// ============================================================================

#ifndef SAD_AST_DOCS_EXTRACTOR_H
#define SAD_AST_DOCS_EXTRACTOR_H

#include "ast_node.h"
#include "declarations.h"
#include "module_nodes.h"
#include "statements.h"
#include "doc_comment.h"
#include "data_types.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <set>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace Sad
{
    namespace AST
    {

        // ============================================================================
        // (AR) المُستخرج / (EN) Extractor
        // ============================================================================

        /**
         * @brief (AR) أداة استخراج التوثيق من شجرة AST وتحويله إلى Markdown
         *        (EN) Utility to extract documentation from AST and emit Markdown
         *
         * @details
         * (AR) الاستخدام: ادعُ extractMarkdown مع قائمة الجمل الناتجة من
         *      ParserCore::parseProgram() واسم الملف. ستحصل على نص Markdown
         *      منظَّم يحوي قسماً لكل تصريح يحمل تعليقاً توثيقياً.
         *
         * (EN) Usage: call extractMarkdown with the StmtList from
         *      ParserCore::parseProgram() and the filename. You receive a
         *      structured Markdown document with one section per documented decl.
         */
        class DocsExtractor
        {
        public:
            /**
             * @brief (AR) ينتج توثيق Markdown كامل من شجرة AST
             *        (EN) Produces full Markdown documentation from AST
             *
             * @param program (AR) قائمة الجمل العليا (EN) Top-level statement list
             * @param filename (AR) اسم الملف المصدري (EN) Source filename
             * @return (AR) نص Markdown (EN) Markdown text
             */
            // ========================================================================
            // (AR) إحصائيات التوثيق / (EN) Documentation statistics
            // ========================================================================
            struct DocStats
            {
                size_t functions = 0;
                size_t classes = 0;
                size_t enums = 0;
                size_t structs = 0;
                size_t traits = 0;
                size_t variables = 0;
                size_t constants = 0;

                size_t total() const
                {
                    return functions + classes + enums + structs + traits + variables + constants;
                }
            };

            static std::string extractMarkdown(const StmtList &program,
                                               const std::string &filename)
            {
                std::ostringstream out;

                // (AR) رأس المستند / (EN) Document header
                out << "# " << titleFromFilename(filename) << "\n\n";
                out << "> (AR) توثيق مُستخرج تلقائياً من ملف `" << filename << "`\n";
                out << "> (EN) Documentation auto-extracted from `" << filename << "`\n\n";

                // (AR) المرحلة 1: حساب الإحصائيات وبناء فهرس المحتويات
                // (EN) Phase 1: compute stats and build a Table of Contents
                DocStats stats;
                std::ostringstream toc;
                size_t tocItems = 0;

                for (const auto &stmt : program)
                {
                    if (!stmt)
                        continue;
                    appendTocEntry(*stmt, toc, stats, tocItems);
                }

                // (AR) المرحلة 2: إصدار الجسم الفعلي / (EN) Phase 2: emit actual body
                size_t documentedCount = 0;
                std::ostringstream body;
                for (const auto &stmt : program)
                {
                    if (!stmt)
                        continue;
                    if (emitDecl(*stmt, body, /*headingLevel=*/2))
                        ++documentedCount;
                }

                // (AR) ملخص + إحصائيات / (EN) Summary + stats
                out << "**(AR) عدد العناصر الموثقة:** " << documentedCount << "  \n";
                out << "**(EN) Documented items:** " << documentedCount << "\n\n";

                if (stats.total() > 0)
                {
                    out << "| النوع / Kind | العدد / Count |\n";
                    out << "|---|---:|\n";
                    if (stats.functions)
                        out << "| 🔧 دوال / functions | " << stats.functions << " |\n";
                    if (stats.classes)
                        out << "| 🏛 أصناف / classes | " << stats.classes << " |\n";
                    if (stats.enums)
                        out << "| 📊 تعدادات / enums | " << stats.enums << " |\n";
                    if (stats.structs)
                        out << "| 🧱 بُنى / structs | " << stats.structs << " |\n";
                    if (stats.traits)
                        out << "| 🎭 سمات / traits | " << stats.traits << " |\n";
                    if (stats.variables)
                        out << "| 📦 متغيرات / variables | " << stats.variables << " |\n";
                    if (stats.constants)
                        out << "| 🔒 ثوابت / constants | " << stats.constants << " |\n";
                    out << "\n";
                }

                // (AR) جدول المحتويات / (EN) Table of Contents
                if (tocItems > 0)
                {
                    out << "## 📑 جدول المحتويات / Table of Contents\n\n";
                    out << toc.str();
                    out << "\n";
                }

                out << "---\n\n";

                if (documentedCount == 0)
                {
                    out << "_(AR) لم يُعثر على أي تعليق توثيقي (## أو #** **#) "
                           "مرفق بتصريح._\n";
                    out << "_(EN) No documentation comments (## or #** **#) "
                           "attached to any declaration were found._\n";
                }
                else
                {
                    out << body.str();
                }

                return out.str();
            }

            // ========================================================================
            // (AR) معلومات ملف موثَّق ضمن مشروع
            // (EN) Documented file info within a project
            // ========================================================================
            struct FileEntry
            {
                std::string path;        ///< (AR) المسار النسبي/الكامل (EN) relative or full path
                const StmtList *program; ///< (AR) شجرة AST المُحلَّلة (EN) parsed AST
            };

            /**
             * @brief (AR) ينتج توثيق مشروع متعدد الملفات بصيغة Markdown
             *        (EN) Produces multi-file project Markdown documentation
             *
             * @param projectName (AR) اسم المشروع (للعنوان) (EN) project name for the title
             * @param files (AR) قائمة الملفات الموثقة (مع AST لكل منها)
             *              (EN) list of documented files (each with its AST)
             * @return (AR) نص Markdown كامل للمشروع (EN) full project Markdown
             *
             * @details
             * (AR) يبني المخرج فهرساً بالملفات في الأعلى ثم قسماً لكل ملف يحوي
             *      تصريحاته الموثقة. كل ملف يصبح H2، تصريحاته H3 وما بعدها.
             *      الملفات بدون تصريحات موثقة تُحذف من الفهرس والمحتوى.
             *
             * (EN) Builds an index at the top, then a section per file containing
             *      its documented declarations. Each file becomes H2, its decls H3+.
             *      Files with no documented declarations are omitted entirely.
             */
            static std::string extractProjectMarkdown(
                const std::string &projectName,
                const std::vector<FileEntry> &files)
            {
                // (AR) المرحلة 1: حساب التصريحات الموثقة لكل ملف وبناء أقسامه
                // (EN) Phase 1: count documented decls per file and build sections
                struct FileSection
                {
                    std::string path;
                    std::string anchor;
                    size_t count;
                    std::string body;
                    DocStats stats; // (AR) إحصائيات هذا الملف وحده
                };

                std::vector<FileSection> sections;
                size_t totalDocumented = 0;
                DocStats grandTotal; // (AR) المجموع التراكمي لكل المشروع

                for (const auto &fe : files)
                {
                    if (!fe.program)
                        continue;

                    std::ostringstream body;
                    size_t count = 0;
                    DocStats fileStats;
                    // (AR) تمرير سريع: حساب الإحصائيات لهذا الملف
                    // (EN) Quick pass: compute stats for this file
                    {
                        std::ostringstream dummyToc;
                        size_t dummy = 0;
                        for (const auto &stmt : *fe.program)
                        {
                            if (!stmt)
                                continue;
                            appendTocEntry(*stmt, dummyToc, fileStats, dummy);
                        }
                    }
                    for (const auto &stmt : *fe.program)
                    {
                        if (!stmt)
                            continue;
                        // (AR) داخل المشروع: عناوين التصريحات تبدأ من H3 (تحت H2 للملف)
                        // (EN) Within project: decl headings start at H3 (under file H2)
                        if (emitDecl(*stmt, body, /*headingLevel=*/3))
                        {
                            ++count;
                        }
                    }

                    if (count == 0)
                        continue; // (AR) تخطي الملفات بدون توثيق

                    FileSection sec;
                    sec.path = fe.path;
                    sec.anchor = makeAnchor(fe.path);
                    sec.count = count;
                    sec.body = body.str();
                    sec.stats = fileStats;
                    sections.push_back(std::move(sec));
                    totalDocumented += count;
                    // (AR) تجميع الإحصائيات على مستوى المشروع
                    grandTotal.functions += fileStats.functions;
                    grandTotal.classes += fileStats.classes;
                    grandTotal.enums += fileStats.enums;
                    grandTotal.structs += fileStats.structs;
                    grandTotal.traits += fileStats.traits;
                    grandTotal.variables += fileStats.variables;
                    grandTotal.constants += fileStats.constants;
                }

                // (AR) المرحلة 2: بناء المخرج النهائي
                // (EN) Phase 2: build final output
                std::ostringstream out;
                out << "# " << projectName << "\n\n";
                out << "> (AR) توثيق مشروع مُستخرج تلقائياً من شجرة AST\n";
                out << "> (EN) Project documentation auto-extracted from AST\n\n";
                out << "**(AR) عدد الملفات الموثقة:** " << sections.size()
                    << " — **(AR) إجمالي التصريحات:** " << totalDocumented << "  \n";
                out << "**(EN) Documented files:** " << sections.size()
                    << " — **(EN) Total declarations:** " << totalDocumented << "\n\n";

                // (AR) جدول إحصائيات إجمالي على مستوى المشروع
                // (EN) Aggregate project-wide statistics table
                if (grandTotal.total() > 0)
                {
                    out << "### 📊 (AR) إحصائيات المشروع / (EN) Project Statistics\n\n";
                    out << "| النوع / Kind | العدد / Count |\n";
                    out << "|---|---:|\n";
                    if (grandTotal.functions)
                        out << "| 🔧 دوال / functions | " << grandTotal.functions << " |\n";
                    if (grandTotal.classes)
                        out << "| 🏛 أصناف / classes | " << grandTotal.classes << " |\n";
                    if (grandTotal.enums)
                        out << "| 📊 تعدادات / enums | " << grandTotal.enums << " |\n";
                    if (grandTotal.structs)
                        out << "| 🧱 بنى / structs | " << grandTotal.structs << " |\n";
                    if (grandTotal.traits)
                        out << "| 🎭 سمات / traits | " << grandTotal.traits << " |\n";
                    if (grandTotal.variables)
                        out << "| 📦 متغيرات / variables | " << grandTotal.variables << " |\n";
                    if (grandTotal.constants)
                        out << "| 🔒 ثوابت / constants | " << grandTotal.constants << " |\n";
                    out << "\n";
                }

                out << "---\n\n";

                if (sections.empty())
                {
                    out << "_(AR) لم يُعثر على أي ملف يحوي تعليقات توثيقية مرفقة بتصريح._\n";
                    out << "_(EN) No file with documentation comments attached to a "
                           "declaration was found._\n";
                    return out.str();
                }

                // (AR) فهرس الملفات مع تفصيل سريع / (EN) File index with quick breakdown
                out << "## (AR) فهرس الملفات / (EN) File Index\n\n";
                for (const auto &s : sections)
                {
                    out << "- [`" << s.path << "`](#" << s.anchor << ") — "
                        << s.count << " "
                        << (s.count == 1 ? "تصريح" : "تصريحات");
                    // (AR) تفصيل مختصر بالرموز إن أمكن
                    std::ostringstream brk;
                    auto add = [&](const std::string &emoji, size_t n)
                    {
                        if (n)
                            brk << " " << emoji << n;
                    };
                    add("🔧", s.stats.functions);
                    add("🏛", s.stats.classes);
                    add("📊", s.stats.enums);
                    add("🧱", s.stats.structs);
                    add("🎭", s.stats.traits);
                    add("📦", s.stats.variables);
                    add("🔒", s.stats.constants);
                    std::string brkStr = brk.str();
                    if (!brkStr.empty())
                        out << " (" << brkStr.substr(1) << ")";
                    out << "\n";
                }
                out << "\n---\n\n";

                // (AR) الأقسام / (EN) Sections
                for (const auto &s : sections)
                {
                    // (AR) عنوان الملف H2 مع مرساة مخصصة (HTML) لمطابقة الفهرس
                    // (EN) File H2 heading with explicit HTML anchor for index links
                    out << "<a id=\"" << s.anchor << "\"></a>\n";
                    out << "## 📄 `" << s.path << "`\n\n";
                    out << s.body;
                }

                return out.str();
            }

            // ========================================================================
            // (AR) تصدير JSON — مفيد لأدوات خارجية وموقع التوثيق
            // (EN) JSON export — useful for external tools and docs websites
            // ========================================================================
            static std::string extractJson(const StmtList &program,
                                           const std::string &filename)
            {
                std::ostringstream js;
                js << "{\n";
                js << "  \"file\": " << jsonStr(filename) << ",\n";
                js << "  \"declarations\": [\n";

                bool first = true;
                for (const auto &stmt : program)
                {
                    if (!stmt)
                        continue;
                    std::string item = declToJson(*stmt);
                    if (item.empty())
                        continue;
                    if (!first)
                        js << ",\n";
                    js << item;
                    first = false;
                }
                js << "\n  ]\n}\n";
                return js.str();
            }

            // ========================================================================
            // ========================================================================
            // (AR) معالجة العناصر السطرية في Markdown: bold/italic/code/links
            // (EN) Process inline Markdown elements: bold/italic/code/links
            // ========================================================================
            static std::string mdInlineToHtml(const std::string &line)
            {
                // (AR) نُعالج بالترتيب: code أولاً (لأن المحتوى لا يُحلّل ثانية)
                //      ثم links، ثم bold، ثم italic
                std::string s;
                s.reserve(line.size() + 16);
                size_t i = 0, n = line.size();
                while (i < n)
                {
                    // (AR) inline code: `...`
                    if (line[i] == '`')
                    {
                        size_t end = line.find('`', i + 1);
                        if (end != std::string::npos)
                        {
                            s += "<code>";
                            s += htmlEsc(line.substr(i + 1, end - i - 1));
                            s += "</code>";
                            i = end + 1;
                            continue;
                        }
                    }
                    // (AR) link: [text](url)
                    if (line[i] == '[')
                    {
                        size_t close = line.find(']', i + 1);
                        if (close != std::string::npos && close + 1 < n && line[close + 1] == '(')
                        {
                            size_t paren = line.find(')', close + 2);
                            if (paren != std::string::npos)
                            {
                                std::string text = line.substr(i + 1, close - i - 1);
                                std::string url = line.substr(close + 2, paren - close - 2);
                                s += "<a href=\"";
                                s += htmlEsc(url);
                                s += "\">";
                                s += mdInlineToHtml(text); // (AR) recursion للـ inline داخل النص
                                s += "</a>";
                                i = paren + 1;
                                continue;
                            }
                        }
                    }
                    // (AR) bold: **text**
                    if (i + 1 < n && line[i] == '*' && line[i + 1] == '*')
                    {
                        size_t end = line.find("**", i + 2);
                        if (end != std::string::npos)
                        {
                            s += "<strong>";
                            s += mdInlineToHtml(line.substr(i + 2, end - i - 2));
                            s += "</strong>";
                            i = end + 2;
                            continue;
                        }
                    }
                    // (AR) italic: _text_  (نتجنب التطابق مع منتصف كلمة)
                    if (line[i] == '_')
                    {
                        bool startBoundary = (i == 0) || std::isspace((unsigned char)line[i - 1]) || std::ispunct((unsigned char)line[i - 1]);
                        if (startBoundary)
                        {
                            size_t end = line.find('_', i + 1);
                            if (end != std::string::npos && end > i + 1)
                            {
                                bool endBoundary = (end + 1 == n) || std::isspace((unsigned char)line[end + 1]) || std::ispunct((unsigned char)line[end + 1]);
                                if (endBoundary)
                                {
                                    s += "<em>";
                                    s += mdInlineToHtml(line.substr(i + 1, end - i - 1));
                                    s += "</em>";
                                    i = end + 1;
                                    continue;
                                }
                            }
                        }
                    }
                    // (AR) escape HTML للحرف العادي
                    char c = line[i];
                    switch (c)
                    {
                    case '<':
                        s += "&lt;";
                        break;
                    case '>':
                        s += "&gt;";
                        break;
                    case '&':
                        s += "&amp;";
                        break;
                    default:
                        s += c;
                        break;
                    }
                    ++i;
                }
                return s;
            }

            // ========================================================================
            // (AR) ينشئ معرّف anchor آمن من نص عنوان (للعناوين العربية والإنجليزية)
            // (EN) Build a safe anchor ID from heading text (works for Arabic + ASCII)
            // ========================================================================
            static std::string slugify(const std::string &text)
            {
                std::string out;
                out.reserve(text.size());
                bool lastDash = false;
                for (unsigned char c : text)
                {
                    if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
                    {
                        out += (char)c;
                        lastDash = false;
                    }
                    else if (c >= 'A' && c <= 'Z')
                    {
                        out += (char)(c - 'A' + 'a');
                        lastDash = false;
                    }
                    else if (c >= 0x80)
                    {
                        // (AR) نمرّر بايتات UTF-8 كما هي — المتصفح يقبلها في id
                        out += (char)c;
                        lastDash = false;
                    }
                    else
                    {
                        if (!lastDash && !out.empty())
                        {
                            out += '-';
                            lastDash = true;
                        }
                    }
                }
                while (!out.empty() && out.back() == '-')
                    out.pop_back();
                if (out.empty())
                    out = "section";
                return out;
            }

            // ========================================================================
            // (AR) مُحوّل Markdown → HTML مُبسّط لتغذية تصدير PDF.
            //      يدعم: عناوين بمعرّفات، قوائم، جداول، code fences، blockquote،
            //              hr، روابط، bold/italic/code، تمرير سطور <a id="..."> الخام.
            // (EN) Minimal Markdown → HTML converter feeding PDF export.
            // ========================================================================
            static std::string mdToHtmlBody(const std::string &md)
            {
                std::ostringstream out;
                std::istringstream in(md);
                std::string line;

                bool inCode = false;
                bool inList = false;
                bool inTable = false;
                bool tableHeaderEmitted = false;

                auto closeList = [&]()
                {
                    if (inList)
                    {
                        out << "</ul>\n";
                        inList = false;
                    }
                };
                auto closeTable = [&]()
                {
                    if (inTable)
                    {
                        out << "</tbody></table>\n";
                        inTable = false;
                        tableHeaderEmitted = false;
                    }
                };

                while (std::getline(in, line))
                {
                    // (AR) إزالة CR من نهاية السطر (Windows)
                    if (!line.empty() && line.back() == '\r')
                        line.pop_back();

                    // (AR) داخل code fence: نمرّر النص الخام مع escape
                    if (inCode)
                    {
                        if (line.rfind("```", 0) == 0)
                        {
                            out << "</code></pre>\n";
                            inCode = false;
                        }
                        else
                        {
                            out << htmlEsc(line) << "\n";
                        }
                        continue;
                    }

                    // (AR) فتح code fence: ```lang
                    if (line.rfind("```", 0) == 0)
                    {
                        closeList();
                        closeTable();
                        std::string lang = line.size() > 3 ? line.substr(3) : "";
                        out << "<pre><code";
                        if (!lang.empty())
                            out << " class=\"language-" << htmlEsc(lang) << "\"";
                        out << ">";
                        inCode = true;
                        continue;
                    }

                    // (AR) سطر فارغ يُغلق القوائم/الجداول
                    if (line.empty())
                    {
                        closeList();
                        closeTable();
                        continue;
                    }

                    // (AR) تمرير HTML الخام: <a id="..."> أو أي عنصر يبدأ بـ <
                    if (line[0] == '<')
                    {
                        closeList();
                        closeTable();
                        out << line << "\n";
                        continue;
                    }

                    // (AR) خط أفقي: ---
                    if (line == "---" || line == "***")
                    {
                        closeList();
                        closeTable();
                        out << "<hr>\n";
                        continue;
                    }

                    // (AR) العناوين #/##/###/####
                    int hLevel = 0;
                    while (hLevel < 6 && (size_t)hLevel < line.size() && line[hLevel] == '#')
                        ++hLevel;
                    if (hLevel > 0 && hLevel < (int)line.size() && line[hLevel] == ' ')
                    {
                        closeList();
                        closeTable();
                        std::string text = line.substr(hLevel + 1);
                        std::string id = slugify(text);
                        out << "<h" << hLevel << " id=\"" << htmlEsc(id) << "\">"
                            << mdInlineToHtml(text) << "</h" << hLevel << ">\n";
                        continue;
                    }

                    // (AR) blockquote: > text
                    if (line.size() >= 2 && line[0] == '>' && line[1] == ' ')
                    {
                        closeList();
                        closeTable();
                        out << "<blockquote>" << mdInlineToHtml(line.substr(2))
                            << "</blockquote>\n";
                        continue;
                    }

                    // (AR) جدول: السطر يحتوي على | في البداية والنهاية
                    if (!line.empty() && line[0] == '|' && line.back() == '|')
                    {
                        // (AR) فحص هل هذا سطر فاصل |---|---|
                        bool isSep = true;
                        for (char c : line)
                        {
                            if (c != '|' && c != '-' && c != ':' && c != ' ')
                            {
                                isSep = false;
                                break;
                            }
                        }
                        if (isSep)
                        {
                            // (AR) السطر السابق كان رؤوس الأعمدة — نتحول من thead إلى tbody
                            if (inTable && !tableHeaderEmitted)
                            {
                                out << "</thead><tbody>\n";
                                tableHeaderEmitted = true;
                            }
                            continue;
                        }

                        closeList();
                        if (!inTable)
                        {
                            out << "<table><thead>\n";
                            inTable = true;
                        }
                        out << "<tr>";
                        // (AR) تقسيم على | مع تجاهل أول وآخر فراغ
                        std::string inner = line.substr(1, line.size() - 2);
                        std::string cell;
                        std::string tag = tableHeaderEmitted ? "td" : "th";
                        for (size_t i = 0; i <= inner.size(); ++i)
                        {
                            if (i == inner.size() || inner[i] == '|')
                            {
                                // (AR) قص المسافات
                                size_t a = 0, b = cell.size();
                                while (a < b && std::isspace((unsigned char)cell[a]))
                                    ++a;
                                while (b > a && std::isspace((unsigned char)cell[b - 1]))
                                    --b;
                                out << "<" << tag << ">"
                                    << mdInlineToHtml(cell.substr(a, b - a))
                                    << "</" << tag << ">";
                                cell.clear();
                            }
                            else
                            {
                                cell += inner[i];
                            }
                        }
                        out << "</tr>\n";
                        continue;
                    }

                    // (AR) قائمة: - item
                    if (line.size() >= 2 && line[0] == '-' && line[1] == ' ')
                    {
                        closeTable();
                        if (!inList)
                        {
                            out << "<ul>\n";
                            inList = true;
                        }
                        out << "<li>" << mdInlineToHtml(line.substr(2)) << "</li>\n";
                        continue;
                    }
                    // (AR) قائمة فرعية: "  - item"
                    if (line.size() >= 4 && line[0] == ' ' && line[1] == ' ' && line[2] == '-' && line[3] == ' ')
                    {
                        if (!inList)
                        {
                            out << "<ul>\n";
                            inList = true;
                        }
                        out << "<li style=\"margin-right:1em\">"
                            << mdInlineToHtml(line.substr(4)) << "</li>\n";
                        continue;
                    }

                    // (AR) فقرة عادية
                    closeList();
                    closeTable();
                    out << "<p>" << mdInlineToHtml(line) << "</p>\n";
                }

                if (inCode)
                    out << "</code></pre>\n";
                if (inList)
                    out << "</ul>\n";
                if (inTable)
                    out << "</tbody></table>\n";

                return out.str();
            }

            // ========================================================================
            // (AR) لف نص Markdown داخل قالب HTML قابل للطباعة (للتحويل إلى PDF)
            // (EN) Wrap Markdown text in a printable HTML shell (for PDF conversion)
            // (AR) يُستخدم لتصدير المشاريع متعددة الملفات إلى PDF — يحوّل Markdown
            //      إلى HTML حقيقي (عناوين + روابط) ليعمل الفهرس بالنقر داخل PDF.
            // ========================================================================
            static std::string markdownToPrintableHtml(const std::string &md,
                                                       const std::string &title,
                                                       const std::string &mermaidDiagram = "")
            {
                std::ostringstream h;
                h << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n";
                h << "  <meta charset=\"UTF-8\">\n";
                h << "  <title>" << htmlEsc(title) << "</title>\n";
                h << "  <style>\n";
                h << "    @page { size: A4; margin: 2cm 1.5cm; }\n";
                h << "    @media print {\n";
                h << "      body { font-size: 10.5pt; }\n";
                h << "      h1, h2, h3 { page-break-after: avoid; }\n";
                h << "      pre, table { page-break-inside: avoid; }\n";
                h << "      h2 { page-break-before: auto; }\n";
                h << "      a { color: #1d4ed8; text-decoration: none; }\n";
                h << "    }\n";
                h << "    body { font-family: 'Segoe UI', 'Tahoma', sans-serif;\n";
                h << "           color: #1a1a1a; background: #ffffff;\n";
                h << "           line-height: 1.65; margin: 0; padding: 1em 1.5em; }\n";
                h << "    h1 { color: #2c7be5; border-bottom: 3px solid #2c7be5;\n";
                h << "         padding-bottom: .3em; font-size: 1.8em; }\n";
                h << "    h2 { color: #2c7be5; border-bottom: 1px solid #d1d5db;\n";
                h << "         padding-bottom: .2em; margin-top: 1.5em; font-size: 1.4em; }\n";
                h << "    h3 { color: #1f2937; margin-top: 1.2em; font-size: 1.15em; }\n";
                h << "    h4 { color: #374151; margin-top: 1em; font-size: 1.05em; }\n";
                h << "    a { color: #1d4ed8; text-decoration: none; }\n";
                h << "    a:hover { text-decoration: underline; }\n";
                h << "    code { background: #f3f4f6; padding: 1px 5px; border-radius: 3px;\n";
                h << "           font-family: 'Cascadia Code', 'Consolas', monospace;\n";
                h << "           font-size: .9em; color: #be185d; }\n";
                h << "    pre { background: #f8fafc; padding: .8em 1em;\n";
                h << "          border: 1px solid #e5e7eb; border-radius: 4px;\n";
                h << "          direction: ltr; overflow-x: auto;\n";
                h << "          white-space: pre-wrap; word-wrap: break-word; }\n";
                h << "    pre code { background: transparent; padding: 0; color: inherit; }\n";
                h << "    table { border-collapse: collapse; width: 100%; margin: .8em 0;\n";
                h << "            font-size: .92em; }\n";
                h << "    th, td { border: 1px solid #d1d5db; padding: .4em .8em; text-align: right; }\n";
                h << "    th { background: #f3f4f6; font-weight: 600; }\n";
                h << "    blockquote { border-right: 4px solid #2c7be5; margin: .8em 0;\n";
                h << "                 padding: .4em 1em; background: #f8fafc; color: #4b5563; }\n";
                h << "    hr { border: 0; border-top: 1px solid #e5e7eb; margin: 1.2em 0; }\n";
                h << "    ul { padding-right: 1.5em; }\n";
                h << "    .footer { margin-top: 3em; padding-top: 1em;\n";
                h << "              border-top: 1px solid #e5e7eb;\n";
                h << "              color: #9ca3af; font-size: .8em; text-align: center; }\n";
                h << "  </style>\n</head>\n<body>\n";
                // (AR) مخطط Mermaid قبل المحتوى الأساسي إن وُجد
                // (EN) Mermaid diagram before the main content if any
                if (!mermaidDiagram.empty())
                {
                    h << "  <h2>📊 مخطط الفئات / Class Diagram</h2>\n";
                    h << "  <div class=\"mermaid\" style=\"background:#f8fafc;"
                      << "padding:1em;border:1px solid #e5e7eb;border-radius:6px;"
                      << "direction:ltr;text-align:center;page-break-inside:avoid;"
                      << "margin:1em 0;\">\n";
                    h << mermaidDiagram;
                    h << "  </div>\n";
                }
                h << mdToHtmlBody(md);
                h << "  <div class=\"footer\">📄 وثيقة مشروع — Generated by Sad / sadc</div>\n";
                // (AR) سكربت Mermaid لتصيير المخطط (إن وُجد)
                if (!mermaidDiagram.empty())
                {
                    h << "  <script type=\"module\">\n";
                    h << "    import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';\n";
                    h << "    mermaid.initialize({ startOnLoad: true, theme: 'default',\n";
                    h << "                          securityLevel: 'loose' });\n";
                    h << "  </script>\n";
                }
                h << "</body>\n</html>\n";
                return h.str();
            }

            // ========================================================================
            // (AR) إنتاج HTML محسّن للطباعة / تصدير PDF
            // (EN) Produce printable HTML optimized for PDF export
            // ========================================================================
            static std::string extractPrintableHtml(const StmtList &program,
                                                    const std::string &filename)
            {
                std::ostringstream h;
                h << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n";
                h << "  <meta charset=\"UTF-8\">\n";
                h << "  <title>" << htmlEsc(titleFromFilename(filename)) << "</title>\n";
                h << "  <style>\n";
                h << "    @page { size: A4; margin: 2cm 1.5cm; }\n";
                h << "    @media print {\n";
                h << "      body { font-size: 10.5pt; }\n";
                h << "      h1, h2, h3 { page-break-after: avoid; }\n";
                h << "      pre, table { page-break-inside: avoid; }\n";
                h << "    }\n";
                h << "    body { font-family: 'Segoe UI', 'Tahoma', sans-serif;\n";
                h << "           color: #1a1a1a; background: #ffffff;\n";
                h << "           line-height: 1.65; max-width: 100%; margin: 0; padding: 0; }\n";
                h << "    h1 { color: #2c7be5; border-bottom: 3px solid #2c7be5;\n";
                h << "         padding-bottom: .3em; font-size: 1.8em; }\n";
                h << "    h2 { color: #2c7be5; border-bottom: 1px solid #d1d5db;\n";
                h << "         padding-bottom: .2em; margin-top: 1.5em; font-size: 1.4em; }\n";
                h << "    h3 { color: #1f2937; margin-top: 1.2em; font-size: 1.15em; }\n";
                h << "    code { background: #f3f4f6; padding: 1px 5px; border-radius: 3px;\n";
                h << "           font-family: 'Cascadia Code', 'Consolas', monospace; font-size: .9em;\n";
                h << "           color: #be185d; }\n";
                h << "    pre { background: #f8fafc; padding: .8em 1em;\n";
                h << "          border: 1px solid #e5e7eb; border-radius: 4px;\n";
                h << "          direction: ltr; overflow-x: auto;\n";
                h << "          white-space: pre-wrap; word-wrap: break-word; }\n";
                h << "    pre code { background: transparent; padding: 0; color: inherit; }\n";
                h << "    table { border-collapse: collapse; width: 100%; margin: .8em 0;\n";
                h << "            font-size: .92em; }\n";
                h << "    th, td { border: 1px solid #d1d5db; padding: .4em .8em; text-align: right; }\n";
                h << "    th { background: #f3f4f6; font-weight: 600; }\n";
                h << "    blockquote { border-right: 4px solid #2c7be5; margin: .8em 0;\n";
                h << "                 padding: .4em 1em; background: #f8fafc; color: #4b5563; }\n";
                h << "    .meta { color: #6b7280; font-style: italic; font-size: .9em; }\n";
                h << "    .footer { margin-top: 3em; padding-top: 1em;\n";
                h << "              border-top: 1px solid #e5e7eb;\n";
                h << "              color: #9ca3af; font-size: .8em; text-align: center; }\n";
                h << "  </style>\n</head>\n<body>\n";

                h << "  <h1>📘 " << htmlEsc(titleFromFilename(filename)) << "</h1>\n";
                h << "  <p class=\"meta\">توثيق مُستخرج تلقائياً من <code>"
                  << htmlEsc(filename) << "</code></p>\n";

                std::string md = extractMarkdown(program, filename);
                // (AR) مخطط Mermaid لشجرة الوراثة قبل المحتوى
                std::string mer = buildClassDiagram(program);
                if (!mer.empty())
                {
                    h << "  <h2>📊 مخطط الفئات / Class Diagram</h2>\n";
                    h << "  <div class=\"mermaid\" style=\"background:#f8fafc;"
                      << "padding:1em;border:1px solid #e5e7eb;border-radius:6px;"
                      << "direction:ltr;text-align:center;page-break-inside:avoid;"
                      << "margin:1em 0;\">\n"
                      << mer << "  </div>\n";
                }
                h << "  <pre style=\"background:#ffffff;border:0;color:#1a1a1a;"
                  << "white-space:pre-wrap;direction:rtl;text-align:right;font-family:inherit;"
                  << "padding:0;\">" << htmlEsc(md) << "</pre>\n";

                h << "  <div class=\"footer\">📄 وثيقة جاهزة للطباعة — Generated by Sad / sadc</div>\n";
                if (!mer.empty())
                {
                    h << "  <script type=\"module\">\n";
                    h << "    import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';\n";
                    h << "    mermaid.initialize({ startOnLoad: true, theme: 'default',\n";
                    h << "                          securityLevel: 'loose' });\n";
                    h << "  </script>\n";
                }
                h << "</body>\n</html>\n";
                return h.str();
            }

            // ========================================================================
            // (AR) جمع أسماء جميع التصاريح القابلة للربط (دوال/أصناف/تعدادات/...)
            // (EN) Collect names of all linkable declarations
            // (AR) يُستخدم لإنشاء روابط متبادلة في HTML — كل ذكر للاسم يصبح
            //      رابطاً قابلاً للنقر يقفز إلى تعريفه عبر anchor.
            // ========================================================================
            static void collectDeclNames(const ASTNode &node,
                                         std::set<std::string> &names)
            {
                if (auto *fn = dynamic_cast<const FunctionDecl *>(&node))
                {
                    if (!fn->name.empty())
                        names.insert(fn->name);
                    return;
                }
                if (auto *cls = dynamic_cast<const ClassDecl *>(&node))
                {
                    if (!cls->name.empty())
                        names.insert(cls->name);
                    for (const auto &m : cls->members)
                        if (m)
                            collectDeclNames(*m, names);
                    return;
                }
                if (auto *en = dynamic_cast<const EnumDecl *>(&node))
                {
                    if (!en->name.empty())
                        names.insert(en->name);
                    return;
                }
                if (auto *st = dynamic_cast<const StructDecl *>(&node))
                {
                    if (!st->name.empty())
                        names.insert(st->name);
                    return;
                }
                if (auto *tr = dynamic_cast<const TraitDecl *>(&node))
                {
                    if (!tr->name.empty())
                        names.insert(tr->name);
                    return;
                }
                if (auto *md = dynamic_cast<const MethodDecl *>(&node))
                {
                    if (!md->name.empty())
                        names.insert(md->name);
                    return;
                }
                if (auto *ns = dynamic_cast<const NamespaceDecl *>(&node))
                {
                    if (!ns->name.empty())
                        names.insert(ns->name);
                    for (const auto &m : ns->members)
                        if (m)
                            collectDeclNames(*m, names);
                    return;
                }
                if (auto *exp = dynamic_cast<const ExportStmt *>(&node))
                {
                    if (exp->declaration)
                        collectDeclNames(*exp->declaration, names);
                    return;
                }
                if (auto *vd = dynamic_cast<const VarDeclStmt *>(&node))
                {
                    if (!vd->name.empty())
                        names.insert(vd->name);
                    return;
                }
            }

            // ========================================================================
            // (AR) جمع علاقات الوراثة من الأصناف (للرسم بـ Mermaid)
            // (EN) Collect class inheritance relationships (for Mermaid diagram)
            // (AR) النتيجة: vector<(child, parent)> — كل زوج يمثل سهم وراثة
            // ========================================================================
            static void collectInheritance(const ASTNode &node,
                                           std::vector<std::pair<std::string, std::string>> &edges,
                                           std::set<std::string> &allClasses)
            {
                if (auto cls = dynamic_cast<const ClassDecl *>(&node))
                {
                    allClasses.insert(cls->name);
                    for (const auto &base : cls->superclasses)
                    {
                        if (!base.empty())
                        {
                            allClasses.insert(base);
                            edges.emplace_back(cls->name, base);
                        }
                    }
                    return;
                }
                if (auto ns = dynamic_cast<const NamespaceDecl *>(&node))
                {
                    for (const auto &m : ns->members)
                        if (m)
                            collectInheritance(*m, edges, allClasses);
                    return;
                }
                if (auto exp = dynamic_cast<const ExportStmt *>(&node))
                {
                    if (exp->declaration)
                        collectInheritance(*exp->declaration, edges, allClasses);
                    return;
                }
            }

            // ========================================================================
            // (AR) بناء تعريف Mermaid classDiagram من علاقات الوراثة
            // (EN) Build a Mermaid classDiagram definition from inheritance edges
            // (AR) يُرجع فارغاً إذا لم يوجد أي صنف موثق.
            // ========================================================================
            static std::string buildClassDiagram(const StmtList &program)
            {
                std::vector<std::pair<std::string, std::string>> edges;
                std::set<std::string> allClasses;
                for (const auto &stmt : program)
                    if (stmt)
                        collectInheritance(*stmt, edges, allClasses);

                if (allClasses.empty())
                    return std::string();

                std::ostringstream m;
                m << "classDiagram\n";
                // (AR) إعلان كل الأصناف لضمان ظهور المعزولة منها
                for (const auto &c : allClasses)
                    m << "    class " << c << "\n";
                // (AR) أسهم الوراثة: parent <|-- child
                for (const auto &e : edges)
                    m << "    " << e.second << " <|-- " << e.first << "\n";
                return m.str();
            }

            // ========================================================================
            // (AR) يبني مخطط Mermaid لمشروع متعدد الملفات (يدمج كل أصنافه)
            // (EN) Build Mermaid class diagram for multi-file project (merges all)
            // ========================================================================
            static std::string buildProjectClassDiagram(const std::vector<FileEntry> &files)
            {
                std::vector<std::pair<std::string, std::string>> edges;
                std::set<std::string> allClasses;
                for (const auto &fe : files)
                {
                    if (!fe.program)
                        continue;
                    for (const auto &stmt : *fe.program)
                        if (stmt)
                            collectInheritance(*stmt, edges, allClasses);
                }
                if (allClasses.empty())
                    return std::string();

                std::ostringstream m;
                m << "classDiagram\n";
                for (const auto &c : allClasses)
                    m << "    class " << c << "\n";
                for (const auto &e : edges)
                    m << "    " << e.second << " <|-- " << e.first << "\n";
                return m.str();
            }

        public:
            // ========================================================================
            // (AR) تصدير HTML بسيط — صفحة قائمة بذاتها
            // (EN) Simple HTML export — self-contained page
            // ========================================================================
            static std::string extractHtml(const StmtList &program,
                                           const std::string &filename)
            {
                std::ostringstream h;
                h << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n";
                h << "  <meta charset=\"UTF-8\">\n";
                h << "  <meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n";
                h << "  <title>" << htmlEsc(titleFromFilename(filename)) << "</title>\n";
                h << "  <style>\n";
                h << "    :root { --primary:#2c7be5; --bg:#fafafa; --text:#1a1a1a;\n";
                h << "            --muted:#6b7280; --border:#e5e7eb; --code-bg:#f3f4f6; }\n";
                h << "    @media (prefers-color-scheme: dark) {\n";
                h << "      :root { --bg:#0f172a; --text:#e2e8f0; --muted:#94a3b8;\n";
                h << "              --border:#334155; --code-bg:#1e293b; --primary:#60a5fa; }\n";
                h << "    }\n";
                h << "    * { box-sizing: border-box; }\n";
                h << "    body { font-family: -apple-system, 'Segoe UI', Tahoma, sans-serif;\n";
                h << "           max-width: 1100px; margin: 0 auto; padding: 1em;\n";
                h << "           line-height: 1.7; color: var(--text); background: var(--bg); }\n";
                h << "    h1 { border-bottom: 3px solid var(--primary); padding-bottom: .3em; }\n";
                h << "    h2 { border-bottom: 1px solid var(--border); padding-bottom: .2em;\n";
                h << "         color: var(--primary); margin-top: 2em; }\n";
                h << "    h3 { color: var(--text); margin-top: 1.5em; }\n";
                h << "    code { background: var(--code-bg); padding: 2px 6px; border-radius: 4px;\n";
                h << "           font-family: 'Cascadia Code', Consolas, monospace; font-size:.92em; }\n";
                h << "    pre { background: var(--code-bg); padding: 1em;\n";
                h << "          border-radius: 8px; overflow-x: auto; direction: ltr;\n";
                h << "          border:1px solid var(--border); }\n";
                h << "    pre code { background: transparent; padding: 0; }\n";
                h << "    table { border-collapse: collapse; margin: 1em 0; width:100%; }\n";
                h << "    th, td { border: 1px solid var(--border); padding: .5em 1em; text-align: right; }\n";
                h << "    th { background: var(--code-bg); }\n";
                h << "    blockquote { border-right: 4px solid var(--primary); margin: 1em 0;\n";
                h << "                 padding: .5em 1em; background: var(--code-bg); }\n";
                h << "    /* (AR) شريط البحث الثابت / (EN) Sticky search bar */\n";
                h << "    .search-bar { position: sticky; top: 0; z-index: 100;\n";
                h << "                  background: var(--bg); padding: .8em 0;\n";
                h << "                  border-bottom: 2px solid var(--primary);\n";
                h << "                  margin-bottom: 1em; }\n";
                h << "    .search-bar input { width: 100%; padding: .7em 1em;\n";
                h << "                        font-size: 1em; border: 2px solid var(--border);\n";
                h << "                        border-radius: 8px; background: var(--bg);\n";
                h << "                        color: var(--text); outline: none; }\n";
                h << "    .search-bar input:focus { border-color: var(--primary); }\n";
                h << "    .search-bar .stats { font-size: .85em; color: var(--muted);\n";
                h << "                          margin-top: .4em; }\n";
                h << "    /* (AR) كل تصريح في قسم منفصل قابل للإخفاء */\n";
                h << "    .decl-section { padding: .5em 0; transition: opacity .15s; }\n";
                h << "    .decl-section.hidden { display: none; }\n";
                h << "    .decl-section.dim { opacity: .35; }\n";
                h << "    mark { background: #fef08a; color: #000; padding: 0 2px;\n";
                h << "           border-radius: 2px; }\n";
                h << "    .toc { background: var(--code-bg); padding: 1em 1.5em;\n";
                h << "           border-radius: 8px; border:1px solid var(--border); }\n";
                h << "    .toc ul { margin: .3em 0; padding-right: 1.5em; }\n";
                h << "  </style>\n</head>\n<body>\n";

                h << "  <h1>📘 " << htmlEsc(titleFromFilename(filename)) << "</h1>\n";
                h << "  <p><em>توثيق مُستخرج تلقائياً من <code>"
                  << htmlEsc(filename) << "</code></em></p>\n";

                // (AR) شريط البحث الحي
                // (EN) Live search bar
                h << "  <div class=\"search-bar\">\n";
                h << "    <input type=\"search\" id=\"docSearch\" "
                  << "placeholder=\"🔍 ابحث في التوثيق... / Search docs...\" "
                  << "autocomplete=\"off\" spellcheck=\"false\">\n";
                h << "    <div class=\"stats\" id=\"searchStats\"></div>\n";
                h << "  </div>\n";

                // (AR) المحتوى داخل قسم قابل للبحث
                std::string md = extractMarkdown(program, filename);

                // (AR) مخطط Mermaid لشجرة الوراثة (إن وُجدت أصناف)
                // (EN) Mermaid class diagram for inheritance tree (if classes exist)
                std::string mermaid = buildClassDiagram(program);
                if (!mermaid.empty())
                {
                    h << "  <details open style=\"margin:1em 0\">\n";
                    h << "    <summary style=\"cursor:pointer;font-weight:600;color:var(--primary);"
                      << "padding:.4em 0\">📊 مخطط الفئات / Class Diagram</summary>\n";
                    h << "    <div class=\"mermaid\" style=\"background:var(--code-bg);"
                      << "padding:1em;border-radius:8px;border:1px solid var(--border);"
                      << "direction:ltr;text-align:center\">\n";
                    h << mermaid;
                    h << "    </div>\n";
                    h << "  </details>\n";
                }

                h << "  <div id=\"docContent\">\n";
                h << "    <pre style=\"background:transparent;border:0;color:var(--text);"
                  << "white-space:pre-wrap;direction:rtl;text-align:right;font-family:inherit;\">"
                  << htmlEsc(md) << "</pre>\n";
                h << "  </div>\n";

                // (AR) جمع أسماء التصاريح للروابط المتبادلة
                // (EN) Collect declaration names for cross-references
                std::set<std::string> declNames;
                for (const auto &stmt : program)
                    if (stmt)
                        collectDeclNames(*stmt, declNames);
                std::ostringstream namesJs;
                namesJs << "[";
                bool firstN = true;
                for (const auto &n : declNames)
                {
                    if (!firstN)
                        namesJs << ",";
                    firstN = false;
                    namesJs << jsonStr(n);
                }
                namesJs << "]";

                // (AR) JS بحث حي بسيط — يطابق سطراً سطراً ويُبرز
                // (EN) Simple live search JS — line-by-line match with highlight
                h << "  <script>\n";
                h << "    (function() {\n";
                h << "      const input = document.getElementById('docSearch');\n";
                h << "      const stats = document.getElementById('searchStats');\n";
                h << "      const pre = document.querySelector('#docContent pre');\n";
                h << "      const original = pre.textContent;\n";
                h << "      const lines = original.split('\\n');\n";
                h << "      const declNames = " << namesJs.str() << ";\n";
                h << "      // (AR) خريطة: اسم -> id الـ anchor (مرّة واحدة لأول تعريف)\n";
                h << "      const anchorMap = {};\n";
                h << "      let anchorCounter = 0;\n";
                h << "      function makeAnchorId(name) {\n";
                h << "        if (!anchorMap[name]) anchorMap[name] = 'def-' + (++anchorCounter);\n";
                h << "        return anchorMap[name];\n";
                h << "      }\n";
                h << "      function escapeHtml(s) {\n";
                h << "        return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');\n";
                h << "      }\n";
                h << "      function escapeRe(s) { return s.replace(/[.*+?^${}()|[\\]\\\\]/g,'\\\\$&'); }\n";
                h << "      // (AR) تطبيق الروابط المتبادلة على سطر معروض (HTML آمن)\n";
                h << "      // (EN) Apply cross-reference links on a rendered line (safe HTML)\n";
                h << "      function linkify(htmlLine, rawLine, isHeading) {\n";
                h << "        for (const name of declNames) {\n";
                h << "          if (!name || name.length < 2) continue;\n";
                h << "          const escName = escapeHtml(name);\n";
                h << "          const re = new RegExp('`' + escapeRe(escName) + '`', 'g');\n";
                h << "          if (isHeading && rawLine.indexOf('`' + name + '`') !== -1 && !anchorMap[name]) {\n";
                h << "            const id = makeAnchorId(name);\n";
                h << "            htmlLine = htmlLine.replace(re,\n";
                h << "              '<span id=\"' + id + '\" style=\"scroll-margin-top:5em\">`' + escName + '`</span>');\n";
                h << "          } else {\n";
                h << "            const id = makeAnchorId(name);\n";
                h << "            htmlLine = htmlLine.replace(re,\n";
                h << "              '<a href=\"#' + id + '\" style=\"color:var(--primary);text-decoration:none\">`' + escName + '`</a>');\n";
                h << "          }\n";
                h << "        }\n";
                h << "        return htmlLine;\n";
                h << "      }\n";
                h << "      function renderAll(filterRe) {\n";
                h << "        // (AR) إعادة الـ anchor map كل عرض كامل\n";
                h << "        for (const k in anchorMap) delete anchorMap[k];\n";
                h << "        anchorCounter = 0;\n";
                h << "        let matches = 0;\n";
                h << "        const flagged = lines.map(line => {\n";
                h << "          if (filterRe && line.match(filterRe)) { matches++; return true; }\n";
                h << "          return false;\n";
                h << "        });\n";
                h << "        let visible = null;\n";
                h << "        if (filterRe) {\n";
                h << "          visible = new Set();\n";
                h << "          flagged.forEach((f, i) => {\n";
                h << "            if (f) for (let k = Math.max(0,i-2); k <= Math.min(lines.length-1,i+2); k++) visible.add(k);\n";
                h << "          });\n";
                h << "        }\n";
                h << "        const out = [];\n";
                h << "        lines.forEach((line, i) => {\n";
                h << "          if (visible && !visible.has(i)) return;\n";
                h << "          let html = escapeHtml(line);\n";
                h << "          if (filterRe) html = html.replace(filterRe, m => '<mark>'+escapeHtml(m)+'</mark>');\n";
                h << "          const isHeading = /^#{1,6} /.test(line);\n";
                h << "          html = linkify(html, line, isHeading);\n";
                h << "          out.push(html);\n";
                h << "        });\n";
                h << "        pre.innerHTML = out.length ? out.join('\\n')\n";
                h << "          : '<em style=\"color:var(--muted)\">لا نتائج / No matches</em>';\n";
                h << "        if (filterRe) stats.textContent = matches + ' نتيجة / matches';\n";
                h << "        else stats.textContent = '';\n";
                h << "      }\n";
                h << "      function update() {\n";
                h << "        const q = input.value.trim();\n";
                h << "        renderAll(q ? new RegExp(escapeRe(q), 'gi') : null);\n";
                h << "      }\n";
                h << "      // (AR) عرض أولي مع الروابط فقط (بدون فلترة)\n";
                h << "      renderAll(null);\n";
                h << "      let timer;\n";
                h << "      input.addEventListener('input', () => {\n";
                h << "        clearTimeout(timer);\n";
                h << "        timer = setTimeout(update, 100);\n";
                h << "      });\n";
                h << "      input.addEventListener('keydown', e => {\n";
                h << "        if (e.key === 'Escape') { input.value = ''; update(); }\n";
                h << "      });\n";
                h << "      // (AR) اختصار / لتركيز البحث\n";
                h << "      document.addEventListener('keydown', e => {\n";
                h << "        if (e.key === '/' && document.activeElement !== input) {\n";
                h << "          e.preventDefault(); input.focus();\n";
                h << "        }\n";
                h << "      });\n";
                h << "    })();\n";
                h << "  </script>\n";

                // (AR) سكربت Mermaid لتصيير مخطط الفئات (إن وُجد)
                // (EN) Mermaid script for rendering class diagram (if any)
                if (!mermaid.empty())
                {
                    h << "  <script type=\"module\">\n";
                    h << "    import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';\n";
                    h << "    const isDark = window.matchMedia('(prefers-color-scheme: dark)').matches;\n";
                    h << "    mermaid.initialize({ startOnLoad: true,\n";
                    h << "                          theme: isDark ? 'dark' : 'default',\n";
                    h << "                          securityLevel: 'loose' });\n";
                    h << "  </script>\n";
                }

                h << "</body>\n</html>\n";
                return h.str();
            }

        private:
            // ========================================================================
            // (AR) بناء عنصر فهرس المحتويات لتصريح واحد + تحديث الإحصائيات
            // (EN) Build TOC entry for one decl + update stats
            // ========================================================================
            static void appendTocEntry(const ASTNode &node,
                                       std::ostringstream &toc,
                                       DocStats &stats,
                                       size_t &count)
            {
                auto addLine = [&](const std::string &emoji,
                                   const std::string &kindAr,
                                   const std::string &kindEn,
                                   const std::string &name)
                {
                    // (AR) ندرج الاسم فقط في النص — كل اسم تصريح فريد عملياً
                    // (EN) Display only the name — declaration names are unique in practice
                    toc << "- " << emoji << " " << kindAr << " / " << kindEn
                        << " `" << name << "`\n";
                    ++count;
                };

                if (auto *fn = dynamic_cast<const FunctionDecl *>(&node))
                {
                    if (!fn->docComment.empty())
                    {
                        ++stats.functions;
                        addLine("🔧", "دالة", "function", fn->name);
                    }
                    return;
                }
                if (auto *cls = dynamic_cast<const ClassDecl *>(&node))
                {
                    bool selfDoc = !cls->docComment.empty();
                    bool memberDoc = false;
                    for (const auto &m : cls->members)
                    {
                        if (m && hasDocComment(*m))
                        {
                            memberDoc = true;
                            break;
                        }
                    }
                    if (selfDoc || memberDoc)
                    {
                        ++stats.classes;
                        addLine("🏛", "صنف", "class", cls->name);
                    }
                    return;
                }
                if (auto *en = dynamic_cast<const EnumDecl *>(&node))
                {
                    if (!en->docComment.empty())
                    {
                        ++stats.enums;
                        addLine("📊", "تعداد", "enum", en->name);
                    }
                    return;
                }
                if (auto *st = dynamic_cast<const StructDecl *>(&node))
                {
                    if (!st->docComment.empty())
                    {
                        ++stats.structs;
                        addLine("🧱", "بنية", "struct", st->name);
                    }
                    return;
                }
                if (auto *tr = dynamic_cast<const TraitDecl *>(&node))
                {
                    if (!tr->docComment.empty())
                    {
                        ++stats.traits;
                        addLine("🎭", "سمة", "trait", tr->name);
                    }
                    return;
                }
                if (auto *vd = dynamic_cast<const VarDeclStmt *>(&node))
                {
                    if (!vd->docComment.empty())
                    {
                        if (vd->isConst)
                        {
                            ++stats.constants;
                            addLine("🔒", "ثابت", "constant", vd->name);
                        }
                        else
                        {
                            ++stats.variables;
                            addLine("📦", "متغير", "variable", vd->name);
                        }
                    }
                    return;
                }
                if (auto *exp = dynamic_cast<const ExportStmt *>(&node))
                {
                    if (exp->declaration)
                        appendTocEntry(*exp->declaration, toc, stats, count);
                }
                // (AR) فضاء أسماء — يظهر في TOC ثم تتدرج أعضاؤه تحته
                // (EN) Namespace — appears in TOC, members nested under it
                if (auto *ns = dynamic_cast<const NamespaceDecl *>(&node))
                {
                    // (AR) نحسب عدد الأعضاء الموثقة قبل القرار
                    DocStats childStats;
                    std::ostringstream childToc;
                    size_t childCount = 0;
                    for (const auto &m : ns->members)
                    {
                        if (m)
                            appendTocEntry(*m, childToc, childStats, childCount);
                    }
                    if (childCount > 0 || !ns->docComment.empty())
                    {
                        toc << "- 📁 فضاء / namespace `" << ns->name << "`\n";
                        ++count;
                        // (AR) إدراج الأعضاء بمسافة بادئة
                        std::string childToc_str = childToc.str();
                        std::istringstream iss(childToc_str);
                        std::string line;
                        while (std::getline(iss, line))
                        {
                            if (!line.empty())
                                toc << "  " << line << "\n";
                        }
                        // (AR) ندمج إحصائيات الأعضاء في الإحصائيات الأم
                        stats.functions += childStats.functions;
                        stats.classes += childStats.classes;
                        stats.enums += childStats.enums;
                        stats.structs += childStats.structs;
                        stats.traits += childStats.traits;
                        stats.variables += childStats.variables;
                        stats.constants += childStats.constants;
                        count += childCount;
                    }
                }
            }

            // (AR) فحص سريع: هل العقدة تحمل أي توثيق؟
            // (EN) Quick check: does this node carry any doc comment?
            static bool hasDocComment(const ASTNode &node)
            {
                if (auto *fd = dynamic_cast<const FieldDecl *>(&node))
                    return !fd->docComment.empty();
                if (auto *md = dynamic_cast<const MethodDecl *>(&node))
                    return !md->docComment.empty();
                if (auto *cd = dynamic_cast<const ConstructorDecl *>(&node))
                    return !cd->docComment.empty();
                return !node.docComment.empty();
            }

            // ========================================================================
            // (AR) إصدار قسم Markdown لتصريح واحد / (EN) Emit one Markdown section
            //
            // @return true إذا كان التصريح موثقاً وأُصدر قسم له
            //         true if the declaration was documented and a section was emitted
            // ========================================================================
            static bool emitDecl(const ASTNode &node, std::ostringstream &out, int level)
            {
                // (AR) محاولة كل نوع تصريح موثق / (EN) Try each documented decl type
                if (auto *fn = dynamic_cast<const FunctionDecl *>(&node))
                    return emitFunction(*fn, out, level);
                if (auto *cls = dynamic_cast<const ClassDecl *>(&node))
                    return emitClass(*cls, out, level);
                if (auto *en = dynamic_cast<const EnumDecl *>(&node))
                    return emitEnum(*en, out, level);
                if (auto *st = dynamic_cast<const StructDecl *>(&node))
                    return emitStruct(*st, out, level);
                if (auto *tr = dynamic_cast<const TraitDecl *>(&node))
                    return emitTrait(*tr, out, level);

                // (AR) أعضاء الأصناف (تُستدعى من emitClass للأعضاء الموثقة)
                // (EN) Class members (called from emitClass for documented members)
                if (auto *md = dynamic_cast<const MethodDecl *>(&node))
                    return emitMethod(*md, out, level);
                if (auto *fd = dynamic_cast<const FieldDecl *>(&node))
                    return emitField(*fd, out, level);
                if (auto *cd = dynamic_cast<const ConstructorDecl *>(&node))
                    return emitConstructor(*cd, out, level);

                // (AR) ExportStmt يغلّف تصريحاً — نمشي للداخل
                // (EN) ExportStmt wraps a decl — recurse into it
                if (auto *exp = dynamic_cast<const ExportStmt *>(&node))
                {
                    if (exp->declaration)
                        return emitDecl(*exp->declaration, out, level);
                }

                // (AR) متغير/ثابت على المستوى الأعلى
                // (EN) Top-level variable/constant declaration
                if (auto *vd = dynamic_cast<const VarDeclStmt *>(&node))
                    return emitVariable(*vd, out, level);

                // (AR) فضاء أسماء — يطبع عنواناً ثم يمشي تكرارياً للأعضاء
                // (EN) Namespace — emits a heading then recurses into members
                if (auto *ns = dynamic_cast<const NamespaceDecl *>(&node))
                    return emitNamespace(*ns, out, level);

                return false;
            }

            // ========================================================================
            // (AR) فضاء أسماء — يجمع الأعضاء الموثقة تحت عنوانه
            // (EN) Namespace — groups documented members under its heading
            // ========================================================================
            static bool emitNamespace(const NamespaceDecl &ns,
                                      std::ostringstream &out, int level)
            {
                // (AR) المرحلة 1: جمع الأعضاء الموثقة في مخزن مؤقت
                // (EN) Phase 1: collect documented members in a buffer
                std::ostringstream membersBody;
                size_t documentedMembers = 0;
                for (const auto &member : ns.members)
                {
                    if (!member)
                        continue;
                    // (AR) الأعضاء داخل الفضاء يبدأون من المستوى التالي
                    if (emitDecl(*member, membersBody, level + 1))
                        ++documentedMembers;
                }

                // (AR) إن لم يوجد عضو موثق ولا توثيق للفضاء نفسه، تخطّ
                // (EN) If no documented members and no namespace doc, skip
                if (documentedMembers == 0 && ns.docComment.empty())
                    return false;

                out << headingPrefix(level) << " 📁 " << kindBadge("فضاء", "namespace")
                    << " `" << ns.name << "`\n\n";

                if (!ns.docComment.empty())
                {
                    emitDocBody(ns.docComment, /*params=*/{}, out);
                }

                if (documentedMembers > 0)
                {
                    out << "_(AR) عدد الأعضاء الموثقة: " << documentedMembers
                        << " / (EN) Documented members: " << documentedMembers << "_\n\n";
                    out << membersBody.str();
                }

                out << "---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) طريقة (عضو في صنف) / (EN) Method (class member)
            // ========================================================================
            static bool emitMethod(const MethodDecl &m,
                                   std::ostringstream &out, int level)
            {
                if (m.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("طريقة", "method")
                    << " `" << m.name << "`\n\n";

                out << "```sad\n";
                out << accessKeyword(m.access);
                if (m.isStatic)
                    out << "ساكن ";
                if (m.isVirtual)
                    out << "افتراضي ";
                if (m.isAbstract)
                    out << "مجرد ";
                if (m.isAsync)
                    out << "غير_متزامن ";
                out << "دالة " << m.name << "(";
                for (size_t i = 0; i < m.parameters.size(); ++i)
                {
                    if (i)
                        out << "، ";
                    out << m.parameters[i].name;
                    std::string tn = paramTypeName(m.parameters[i]);
                    if (!tn.empty())
                        out << ": " << tn;
                }
                out << ")";
                std::string rt = dataTypeName(m.returnType);
                if (!rt.empty())
                    out << " -> " << rt;
                out << "\n```\n\n";

                emitDocBody(m.docComment, m.parameters, out);
                out << "\n";
                return true;
            }

            // ========================================================================
            // (AR) حقل (عضو في صنف) / (EN) Field (class member)
            // ========================================================================
            static bool emitField(const FieldDecl &f,
                                  std::ostringstream &out, int level)
            {
                if (f.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("حقل", "field")
                    << " `" << f.name << "`\n\n";

                out << "```sad\n";
                out << accessKeyword(f.access);
                if (f.isStatic)
                    out << "ساكن ";
                out << "متغير " << f.name;
                std::string tn = dataTypeName(f.type);
                if (!tn.empty())
                    out << ": " << tn;
                out << "\n```\n\n";

                emitDocBody(f.docComment, /*params=*/{}, out);
                out << "\n";
                return true;
            }

            // ========================================================================
            // (AR) باني (عضو في صنف) / (EN) Constructor (class member)
            // ========================================================================
            static bool emitConstructor(const ConstructorDecl &c,
                                        std::ostringstream &out, int level)
            {
                if (c.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("باني", "constructor")
                    << "\n\n";

                out << "```sad\nباني(";
                for (size_t i = 0; i < c.parameters.size(); ++i)
                {
                    if (i)
                        out << "، ";
                    out << c.parameters[i].name;
                    std::string tn = paramTypeName(c.parameters[i]);
                    if (!tn.empty())
                        out << ": " << tn;
                }
                out << ")\n```\n\n";

                emitDocBody(c.docComment, c.parameters, out);
                out << "\n";
                return true;
            }

            // ========================================================================
            // (AR) متغير/ثابت على المستوى الأعلى
            // (EN) Top-level variable/constant declaration
            // ========================================================================
            static bool emitVariable(const VarDeclStmt &v,
                                     std::ostringstream &out, int level)
            {
                if (v.docComment.empty())
                    return false;

                const char *kindAr = v.isConst ? "ثابت" : "متغير";
                const char *kindEn = v.isConst ? "constant" : "variable";

                out << headingPrefix(level) << " " << kindBadge(kindAr, kindEn)
                    << " `" << v.name << "`\n\n";

                out << "```sad\n";
                out << (v.isConst ? "ثابت " : "متغير ") << v.name;
                std::string tn = dataTypeName(v.type);
                if (!tn.empty())
                    out << ": " << tn;
                out << "\n```\n\n";

                emitDocBody(v.docComment, /*params=*/{}, out);
                out << "\n---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) دالة / (EN) Function
            // ========================================================================
            static bool emitFunction(const FunctionDecl &fn,
                                     std::ostringstream &out, int level)
            {
                if (fn.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("دالة", "function")
                    << " `" << fn.name << "`\n\n";

                // (AR) التوقيع / (EN) Signature
                out << "```sad\n";
                if (fn.is_async)
                    out << "غير_متزامن ";
                if (fn.isExtern)
                    out << "خارجي ";
                if (fn.isExported)
                    out << "صدّر ";
                out << "دالة " << fn.name << "(";
                for (size_t i = 0; i < fn.parameters.size(); ++i)
                {
                    if (i)
                        out << "، ";
                    out << fn.parameters[i].name;
                    std::string tn = paramTypeName(fn.parameters[i]);
                    if (!tn.empty())
                        out << ": " << tn;
                }
                out << ")";
                std::string rt = dataTypeName(fn.returnType);
                if (!rt.empty())
                    out << " -> " << rt;
                out << "\n```\n\n";

                emitDocBody(fn.docComment, fn.parameters, out);

                // (AR) فاصل / (EN) Separator
                out << "\n---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) صنف / (EN) Class
            // ========================================================================
            static bool emitClass(const ClassDecl &cls,
                                  std::ostringstream &out, int level)
            {
                bool selfDocumented = !cls.docComment.empty();

                // (AR) جمع الأعضاء الموثقة قبل الإصدار — للتأكد من فائدة القسم
                // (EN) Pre-collect documented members to ensure the section is useful
                std::ostringstream membersBuf;
                size_t membersDocumented = 0;
                for (const auto &m : cls.members)
                {
                    if (m && emitDecl(*m, membersBuf, level + 1))
                        ++membersDocumented;
                }

                if (!selfDocumented && membersDocumented == 0)
                    return false;

                out << headingPrefix(level) << " " << kindBadge("صنف", "class")
                    << " `" << cls.name << "`\n\n";

                // (AR) التوقيع / (EN) Signature
                out << "```sad\n";
                if (cls.isAbstract)
                    out << "مجرد ";
                if (cls.isExported)
                    out << "صدّر ";
                out << "صنف " << cls.name;
                if (!cls.superclasses.empty())
                {
                    out << " يرث ";
                    for (size_t i = 0; i < cls.superclasses.size(); ++i)
                    {
                        if (i)
                            out << "، ";
                        out << cls.superclasses[i];
                    }
                }
                out << "\n```\n\n";

                if (selfDocumented)
                {
                    emitDocBody(cls.docComment, /*params=*/{}, out);
                }

                if (membersDocumented > 0)
                {
                    out << "\n**(AR) الأعضاء الموثقة (" << membersDocumented << ")**  \n";
                    out << "**(EN) Documented members (" << membersDocumented << ")**\n\n";
                    out << membersBuf.str();
                }

                out << "\n---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) تعداد / (EN) Enum
            // ========================================================================
            static bool emitEnum(const EnumDecl &en,
                                 std::ostringstream &out, int level)
            {
                if (en.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("تعداد", "enum")
                    << " `" << en.name << "`\n\n";

                out << "```sad\n";
                if (en.isExported)
                    out << "صدّر ";
                out << "تعداد " << en.name << "\n";
                for (const auto &m : en.members)
                {
                    out << "    " << m.name;
                    if (m.hasData())
                    {
                        out << "(";
                        for (size_t i = 0; i < m.fields.size(); ++i)
                        {
                            if (i)
                                out << "، ";
                            out << m.fields[i];
                        }
                        out << ")";
                    }
                    out << "\n";
                }
                out << "نهاية\n```\n\n";

                emitDocBody(en.docComment, /*params=*/{}, out);
                out << "\n---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) بنية / (EN) Struct
            // ========================================================================
            static bool emitStruct(const StructDecl &st,
                                   std::ostringstream &out, int level)
            {
                if (st.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("بنية", "struct")
                    << " `" << st.name << "`\n\n";

                out << "```sad\n";
                if (st.isExported)
                    out << "صدّر ";
                if (st.isPacked)
                    out << "محزوم ";
                out << "بنية " << st.name << "\n";
                for (const auto &f : st.fields)
                {
                    std::string tn = dataTypeName(f.type);
                    out << "    " << f.name;
                    if (!tn.empty())
                        out << ": " << tn;
                    out << "\n";
                }
                out << "نهاية\n```\n\n";

                emitDocBody(st.docComment, /*params=*/{}, out);
                out << "\n---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) سمة / (EN) Trait
            // ========================================================================
            static bool emitTrait(const TraitDecl &tr,
                                  std::ostringstream &out, int level)
            {
                if (tr.docComment.empty())
                    return false;

                out << headingPrefix(level) << " " << kindBadge("سمة", "trait")
                    << " `" << tr.name << "`\n\n";

                out << "```sad\n";
                if (tr.isExported)
                    out << "صدّر ";
                out << "سمة " << tr.name << "\n";
                for (const auto &m : tr.methods)
                {
                    out << "    دالة " << m.name << "(";
                    for (size_t i = 0; i < m.params.size(); ++i)
                    {
                        if (i)
                            out << "، ";
                        out << m.params[i].name;
                    }
                    out << ")\n";
                }
                out << "نهاية\n```\n\n";

                emitDocBody(tr.docComment, /*params=*/{}, out);
                out << "\n---\n\n";
                return true;
            }

            // ========================================================================
            // (AR) جسم التوثيق المُحلَّل / (EN) Parsed doc body
            // ========================================================================
            static void emitDocBody(const std::string &raw,
                                    const std::vector<Parameter> &params,
                                    std::ostringstream &out)
            {
                DocComment doc = DocCommentParser::parse(raw);

                if (!doc.summary.empty())
                    out << "_" << doc.summary << "_\n\n";
                if (!doc.description.empty())
                    out << doc.description << "\n\n";

                if (!doc.params.empty() || !params.empty())
                {
                    out << "**(AR) المعاملات / (EN) Parameters**\n\n";
                    out << "| (AR) الاسم / (EN) Name | (AR) الوصف / (EN) Description |\n";
                    out << "|---|---|\n";

                    // (AR) ادمج المعاملات الفعلية مع وسوم @معطى المُحلّلة
                    // (EN) Merge actual params with parsed @معطى/@param tags
                    for (const auto &p : params)
                    {
                        std::string desc = "—";
                        for (const auto &dp : doc.params)
                        {
                            if (dp.name == p.name)
                            {
                                desc = dp.description;
                                break;
                            }
                        }
                        out << "| `" << p.name << "` | " << desc << " |\n";
                    }

                    // (AR) معاملات في التوثيق غير موجودة في التوقيع — للسمة/التعداد
                    // (EN) Params in docs not in signature — for traits/enums
                    for (const auto &dp : doc.params)
                    {
                        bool inSig = false;
                        for (const auto &p : params)
                            if (p.name == dp.name)
                            {
                                inSig = true;
                                break;
                            }
                        if (!inSig)
                            out << "| `" << dp.name << "` | " << dp.description << " |\n";
                    }
                    out << "\n";
                }

                if (!doc.returnDescription.empty())
                    out << "**(AR) القيمة المرجعة / (EN) Returns:** "
                        << doc.returnDescription << "\n\n";

                if (!doc.exceptions.empty())
                {
                    out << "**(AR) الاستثناءات / (EN) Throws**\n\n";
                    for (const auto &e : doc.exceptions)
                        out << "- `" << e.type << "` — " << e.description << "\n";
                    out << "\n";
                }

                if (!doc.examples.empty())
                {
                    out << "**(AR) أمثلة / (EN) Examples**\n\n";
                    for (const auto &ex : doc.examples)
                        out << "```sad\n"
                            << ex << "\n```\n\n";
                }

                if (!doc.deprecated.empty())
                    out << "> ⚠ **(AR) مهمل / (EN) Deprecated:** "
                        << doc.deprecated << "\n\n";
                if (doc.isExperimental)
                    out << "> 🧪 **(AR) تجريبي / (EN) Experimental:** "
                        << (doc.experimentalNote.empty() ? "قد تتغير هذه الميزة في المستقبل" : doc.experimentalNote)
                        << "\n\n";
                if (!doc.complexity.empty())
                    out << "**(AR) التعقيد / (EN) Complexity:** `"
                        << doc.complexity << "`\n\n";
                if (!doc.threadSafety.empty())
                    out << "**(AR) أمان الخيوط / (EN) Thread Safety:** "
                        << doc.threadSafety << "\n\n";
                if (!doc.typeHint.empty())
                    out << "**(AR) النوع / (EN) Type:** `"
                        << doc.typeHint << "`\n\n";
                if (!doc.defaultValue.empty())
                    out << "**(AR) القيمة الافتراضية / (EN) Default:** `"
                        << doc.defaultValue << "`\n\n";
                if (!doc.version.empty())
                    out << "_(AR) النسخة / (EN) Version: "
                        << doc.version << "_\n\n";
                if (!doc.since.empty())
                    out << "_(AR) منذ / (EN) Since: " << doc.since << "_\n\n";
                if (!doc.author.empty())
                    out << "_(AR) المؤلف / (EN) Author: " << doc.author << "_\n\n";
                if (!doc.license.empty())
                    out << "_(AR) الرخصة / (EN) License: " << doc.license << "_\n\n";
                if (!doc.notes.empty())
                {
                    out << "**(AR) ملاحظات / (EN) Notes**\n\n";
                    for (const auto &n : doc.notes)
                        out << "- " << n << "\n";
                    out << "\n";
                }
                if (!doc.warnings.empty())
                {
                    out << "**(AR) تحذيرات / (EN) Warnings**\n\n";
                    for (const auto &w : doc.warnings)
                        out << "> ⚠️ " << w << "\n\n";
                }
                if (!doc.seeAlso.empty())
                {
                    out << "**(AR) انظر أيضاً / (EN) See Also**\n\n";
                    for (const auto &s : doc.seeAlso)
                        out << "- " << s << "\n";
                    out << "\n";
                }
                if (!doc.links.empty())
                {
                    out << "**(AR) روابط / (EN) Links**\n\n";
                    for (const auto &l : doc.links)
                        out << "- " << l << "\n";
                    out << "\n";
                }
            }

            // ========================================================================
            // (AR) دوال مساعدة / (EN) Helpers
            // ========================================================================
            static std::string headingPrefix(int level)
            {
                if (level < 1)
                    level = 1;
                if (level > 6)
                    level = 6;
                return std::string(static_cast<size_t>(level), '#');
            }

            static std::string kindBadge(const std::string &ar, const std::string &en)
            {
                return ar + " / " + en;
            }

            static std::string titleFromFilename(const std::string &filename)
            {
                // (AR) آخر مكون من المسار / (EN) Last path component
                size_t slash = filename.find_last_of("/\\");
                std::string base = (slash == std::string::npos)
                                       ? filename
                                       : filename.substr(slash + 1);
                return base;
            }

            /**
             * @brief (AR) يحوّل مسار ملف إلى مرساة Markdown آمنة
             *        (EN) Converts a file path to a safe Markdown anchor id
             */
            static std::string makeAnchor(const std::string &path)
            {
                std::string a;
                a.reserve(path.size());
                for (unsigned char c : path)
                {
                    if (c == '/' || c == '\\' || c == '.' || c == ' ')
                        a.push_back('-');
                    else if (c < 0x80)
                        a.push_back(static_cast<char>(std::tolower(c)));
                    else
                        a.push_back(static_cast<char>(c)); // (AR) إبقاء UTF-8 كما هو
                }
                return a;
            }

            /**
             * @brief (AR) يجمع كل ملفات لغة ص (.ص و .sad) من مجلد بشكل تكراري
             *        (EN) Recursively collects all Sad source files (.ص and .sad)
             *
             * @param root (AR) المجلد الجذر للبحث (EN) root directory to scan
             * @return (AR) قائمة مسارات مرتبة أبجدياً (EN) alphabetically sorted paths
             *
             * @details
             * (AR) يتجاهل بصمت أي خطأ نظام ملفات (مجلدات غير قابلة للقراءة، روابط
             *      رمزية معطوبة، إلخ) ولا يرمي استثناءات.
             * (EN) Silently ignores filesystem errors (unreadable dirs, broken
             *      symlinks, etc.) and never throws.
             */
        public:
            static std::vector<std::string> collectSadFiles(const std::string &root)
            {
                namespace fs = std::filesystem;
                std::vector<std::string> result;
                std::error_code ec;

                fs::path rootPath(root);
                if (!fs::exists(rootPath, ec) || ec)
                    return result;

                // (AR) ملف مفرد بدلاً من مجلد / (EN) Single file instead of directory
                if (fs::is_regular_file(rootPath, ec) && !ec)
                {
                    auto ext = rootPath.extension().u8string();
                    if (ext == u8".ص" || ext == ".sad")
                        result.push_back(rootPath.u8string());
                    return result;
                }

                if (!fs::is_directory(rootPath, ec) || ec)
                    return result;

                fs::recursive_directory_iterator it(
                    rootPath,
                    fs::directory_options::skip_permission_denied,
                    ec);
                fs::recursive_directory_iterator end;

                for (; it != end; it.increment(ec))
                {
                    if (ec)
                    {
                        ec.clear();
                        continue;
                    }
                    std::error_code ec2;
                    if (!it->is_regular_file(ec2) || ec2)
                        continue;

                    auto ext = it->path().extension().u8string();
                    if (ext == u8".ص" || ext == ".sad")
                    {
                        result.push_back(it->path().u8string());
                    }
                }

                std::sort(result.begin(), result.end());
                return result;
            }

            /**
             * @brief (AR) نسخة مع أنماط استبعاد — يحذف الملفات التي مسارها يحوي أيًا من الأنماط
             *        (EN) Variant with exclude patterns — drops files whose path contains any pattern
             *
             * @param root         (AR) المجلد الجذر (EN) root directory
             * @param excludeSubs  (AR) قائمة سلاسل فرعية — أي ملف يحوي أحدها يُستبعد
             *                     (EN) substrings — any file containing one is excluded
             */
            static std::vector<std::string> collectSadFiles(
                const std::string &root,
                const std::vector<std::string> &excludeSubs)
            {
                auto all = collectSadFiles(root);
                if (excludeSubs.empty())
                    return all;
                std::vector<std::string> kept;
                kept.reserve(all.size());
                for (const auto &p : all)
                {
                    bool skip = false;
                    for (const auto &sub : excludeSubs)
                    {
                        if (!sub.empty() && p.find(sub) != std::string::npos)
                        {
                            skip = true;
                            break;
                        }
                    }
                    if (!skip)
                        kept.push_back(p);
                }
                return kept;
            }

        private:
            static std::string paramTypeName(const Parameter &p)
            {
                if (!p.typeName.empty())
                    return p.typeName;
                return dataTypeName(p.type);
            }

            static std::string accessKeyword(AccessModifier a)
            {
                switch (a)
                {
                case AccessModifier::PUBLIC:
                    return "عام ";
                case AccessModifier::PRIVATE:
                    return "خاص ";
                case AccessModifier::PROTECTED:
                    return "محمي ";
                default:
                    return "";
                }
            }

            static std::string dataTypeName(Types::SadTypeKind t)
            {
                using DT = Types::SadTypeKind;
                switch (t)
                {
                case Types::SadTypeKind::Integer:
                    return "رقم";
                case Types::SadTypeKind::Float:
                    return "عشري";
                case Types::SadTypeKind::String:
                    return "نص";
                case Types::SadTypeKind::Boolean:
                    return "منطقي";
                case Types::SadTypeKind::Void:
                    return "لاشيء";
                case Types::SadTypeKind::Array:
                    return "مصفوفة";
                case Types::SadTypeKind::Map:
                    return "خريطة";
                case Types::SadTypeKind::Tuple:
                    return "صف";
                case Types::SadTypeKind::Function:
                    return "دالة";
                case Types::SadTypeKind::Class:
                    return "كائن";
                case Types::SadTypeKind::Enum:
                    return "تعداد";
                case Types::SadTypeKind::Byte:
                    return "بايت";
                case Types::SadTypeKind::Error:
                    return "خطأ";
                default:
                    return ""; // UNKNOWN — لا تُعرض
                }
            }

            // ========================================================================
            // (AR) ترميز نص JSON آمن / (EN) Safe JSON string encoding
            // ========================================================================
            static std::string jsonStr(const std::string &s)
            {
                std::ostringstream o;
                o << '"';
                for (unsigned char c : s)
                {
                    switch (c)
                    {
                    case '"':
                        o << "\\\"";
                        break;
                    case '\\':
                        o << "\\\\";
                        break;
                    case '\b':
                        o << "\\b";
                        break;
                    case '\f':
                        o << "\\f";
                        break;
                    case '\n':
                        o << "\\n";
                        break;
                    case '\r':
                        o << "\\r";
                        break;
                    case '\t':
                        o << "\\t";
                        break;
                    default:
                        if (c < 0x20)
                        {
                            char buf[8];
                            std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                            o << buf;
                        }
                        else
                        {
                            o << static_cast<char>(c);
                        }
                    }
                }
                o << '"';
                return o.str();
            }

            // ========================================================================
            // (AR) ترميز HTML آمن / (EN) Safe HTML escape
            // ========================================================================
            static std::string htmlEsc(const std::string &s)
            {
                std::string r;
                r.reserve(s.size());
                for (char c : s)
                {
                    switch (c)
                    {
                    case '<':
                        r += "&lt;";
                        break;
                    case '>':
                        r += "&gt;";
                        break;
                    case '&':
                        r += "&amp;";
                        break;
                    case '"':
                        r += "&quot;";
                        break;
                    default:
                        r += c;
                    }
                }
                return r;
            }

            // ========================================================================
            // (AR) تحويل تصريح إلى عنصر JSON / (EN) Convert decl to JSON object
            // ========================================================================
            static std::string declToJson(const ASTNode &node)
            {
                auto build = [](const std::string &kind,
                                const std::string &name,
                                const std::string &doc) -> std::string
                {
                    if (doc.empty())
                        return "";
                    DocComment dc = DocCommentParser::parse(doc);
                    std::ostringstream j;
                    j << "    {\n";
                    j << "      \"kind\": " << jsonStr(kind) << ",\n";
                    j << "      \"name\": " << jsonStr(name) << ",\n";
                    j << "      \"summary\": " << jsonStr(dc.summary) << ",\n";
                    j << "      \"description\": " << jsonStr(dc.description) << ",\n";
                    j << "      \"params\": [";
                    for (size_t i = 0; i < dc.params.size(); ++i)
                    {
                        if (i)
                            j << ", ";
                        j << "{\"name\":" << jsonStr(dc.params[i].name)
                          << ",\"description\":" << jsonStr(dc.params[i].description) << "}";
                    }
                    j << "],\n";
                    j << "      \"returns\": " << jsonStr(dc.returnDescription) << ",\n";
                    j << "      \"since\": " << jsonStr(dc.since) << ",\n";
                    j << "      \"version\": " << jsonStr(dc.version) << ",\n";
                    j << "      \"author\": " << jsonStr(dc.author) << ",\n";
                    j << "      \"deprecated\": " << jsonStr(dc.deprecated) << ",\n";
                    j << "      \"experimental\": " << (dc.isExperimental ? "true" : "false") << ",\n";
                    j << "      \"complexity\": " << jsonStr(dc.complexity) << ",\n";
                    j << "      \"threadSafety\": " << jsonStr(dc.threadSafety) << ",\n";
                    j << "      \"license\": " << jsonStr(dc.license) << "\n";
                    j << "    }";
                    return j.str();
                };

                if (auto *fn = dynamic_cast<const FunctionDecl *>(&node))
                    return build("function", fn->name, fn->docComment);
                if (auto *cls = dynamic_cast<const ClassDecl *>(&node))
                    return build("class", cls->name, cls->docComment);
                if (auto *en = dynamic_cast<const EnumDecl *>(&node))
                    return build("enum", en->name, en->docComment);
                if (auto *st = dynamic_cast<const StructDecl *>(&node))
                    return build("struct", st->name, st->docComment);
                if (auto *tr = dynamic_cast<const TraitDecl *>(&node))
                    return build("trait", tr->name, tr->docComment);
                if (auto *vd = dynamic_cast<const VarDeclStmt *>(&node))
                    return build(vd->isConst ? "constant" : "variable",
                                 vd->name, vd->docComment);
                if (auto *exp = dynamic_cast<const ExportStmt *>(&node))
                {
                    if (exp->declaration)
                        return declToJson(*exp->declaration);
                }
                // (AR) فضاء أسماء — نُصدِر كائناً مع مصفوفة members
                // (EN) Namespace — emit object with nested members array
                if (auto *ns = dynamic_cast<const NamespaceDecl *>(&node))
                {
                    std::ostringstream j;
                    j << "    {\n";
                    j << "      \"kind\": \"namespace\",\n";
                    j << "      \"name\": " << jsonStr(ns->name) << ",\n";
                    DocComment dc = ns->docComment.empty()
                                        ? DocComment{}
                                        : DocCommentParser::parse(ns->docComment);
                    j << "      \"summary\": " << jsonStr(dc.summary) << ",\n";
                    j << "      \"members\": [";
                    bool first = true;
                    for (const auto &m : ns->members)
                    {
                        if (!m)
                            continue;
                        std::string item = declToJson(*m);
                        if (item.empty())
                            continue;
                        if (!first)
                            j << ", ";
                        // (AR) نزيل المسافة البادئة من العنصر الفرعي ونضيف سطراً
                        j << "\n      " << item.substr(item.find_first_not_of(' '));
                        first = false;
                    }
                    j << "\n      ]\n";
                    j << "    }";
                    return j.str();
                }
                return "";
            }
        };

    } // namespace AST
} // namespace Sad

#endif // SAD_AST_DOCS_EXTRACTOR_H
