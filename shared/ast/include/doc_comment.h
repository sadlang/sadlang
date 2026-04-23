/**
 * @file doc_comment.h
 * @brief (AR) بنية تعليقات التوثيق ومحلل الوسوم للغة ص
 *        (EN) Documentation comment structure and tag parser for Sad language
 * @author Sad Language Team
 *
 * @details
 * (AR) يوفر هذا الملف:
 *   - DocParam: بنية لوصف معطى موثق
 *   - DocComment: بنية لتعليق توثيقي محلل بالكامل
 *   - DocCommentParser: أداة مساعدة لتحليل النص الخام إلى DocComment
 *
 * (EN) This file provides:
 *   - DocParam: structure for a documented parameter
 *   - DocComment: structure for a fully parsed doc comment
 *   - DocCommentParser: utility to parse raw text into DocComment
 *
 * الوسوم المدعومة / Supported tags:
 *   @معطى / @param         — وصف معطى
 *   @أرجع / @return        — وصف القيمة المرجعة
 *   @استثناء / @throws     — استثناء محتمل
 *   @انظر / @see           — مرجع ذو صلة
 *   @منذ / @since          — نسخة الإضافة
 *   @مهمل / @deprecated    — علامة إهمال
 *   @مؤلف / @author        — المؤلف
 *   @ملاحظة / @note        — ملاحظة
 *   @تحذير / @warning      — تحذير
 *   @مثال / @example       — مثال استخدام
 *   @نسخة / @version       — رقم النسخة لهذا التصريح
 *   @نوع / @type           — نوع المتغير/الحقل
 *   @تعقيد / @complexity   — التعقيد الزمني/المكاني (مثل O(n))
 *   @آمن_خيوط / @threadsafe — هل العملية آمنة للخيوط
 *   @تجريبي / @experimental — ميزة تجريبية قد تتغير
 *   @رخصة / @license       — الرخصة
 *   @رابط / @link          — رابط (URL أو مرجع نسبي)
 *   @افتراضي / @default    — القيمة الافتراضية
 */

#ifndef SAD_AST_DOC_COMMENT_H
#define SAD_AST_DOC_COMMENT_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace Sad
{
    namespace AST
    {

        // =========================================================================
        // (AR) معطى موثق / (EN) Documented parameter
        // =========================================================================

        /**
         * @brief (AR) يمثل معطى موثق بوسم @معطى/@param
         *        (EN) Represents a parameter documented with @معطى/@param tag
         */
        struct DocParam
        {
            std::string name;        ///< (AR) اسم المعطى (EN) Parameter name
            std::string description; ///< (AR) وصف المعطى (EN) Parameter description
        };

        // =========================================================================
        // (AR) استثناء موثق / (EN) Documented exception
        // =========================================================================

        /**
         * @brief (AR) يمثل استثناء موثق بوسم @استثناء/@throws
         *        (EN) Represents an exception documented with @استثناء/@throws tag
         */
        struct DocException
        {
            std::string type;        ///< (AR) نوع الاستثناء (EN) Exception type
            std::string description; ///< (AR) وصف الاستثناء (EN) Exception description
        };

        // =========================================================================
        // (AR) تعليق توثيقي محلل / (EN) Parsed documentation comment
        // =========================================================================

        /**
         * @brief (AR) يمثل تعليق توثيقي محلل بالكامل مع جميع الوسوم المستخرجة
         *        (EN) Represents a fully parsed documentation comment with all extracted tags
         *
         * @example
         * (AR) مثال الاستخدام:
         * @code{.cpp}
         * auto doc = DocCommentParser::parse(rawText);
         * if (!doc.summary.empty()) {
         *     std::cout << "الملخص: " << doc.summary << std::endl;
         * }
         * for (const auto& p : doc.params) {
         *     std::cout << p.name << ": " << p.description << std::endl;
         * }
         * @endcode
         */
        struct DocComment
        {
            std::string summary;                  ///< (AR) ملخص مختصر (أول سطر) (EN) Brief summary (first line)
            std::string description;              ///< (AR) وصف مفصل (EN) Detailed description
            std::vector<DocParam> params;         ///< (AR) المعطيات الموثقة (EN) Documented parameters
            std::string returnDescription;        ///< (AR) وصف القيمة المرجعة (EN) Return value description
            std::vector<DocException> exceptions; ///< (AR) الاستثناءات الموثقة (EN) Documented exceptions
            std::vector<std::string> seeAlso;     ///< (AR) مراجع ذات صلة (EN) Related references
            std::string since;                    ///< (AR) نسخة الإضافة (EN) Version added
            std::string deprecated;               ///< (AR) رسالة الإهمال (EN) Deprecation message
            std::string author;                   ///< (AR) المؤلف (EN) Author
            std::vector<std::string> notes;       ///< (AR) ملاحظات (EN) Notes
            std::vector<std::string> warnings;    ///< (AR) تحذيرات (EN) Warnings
            std::vector<std::string> examples;    ///< (AR) أمثلة (EN) Examples
            std::string version;                  ///< (AR) نسخة التصريح (EN) Declaration version
            std::string typeHint;                 ///< (AR) نوع التصريح المُوثَّق (EN) Documented type hint
            std::string complexity;               ///< (AR) تعقيد زمني/مكاني (EN) Time/space complexity
            std::string threadSafety;             ///< (AR) أمان الخيوط (EN) Thread safety note
            bool isExperimental = false;          ///< (AR) ميزة تجريبية (EN) Experimental flag
            std::string experimentalNote;         ///< (AR) شرح الميزة التجريبية (EN) Experimental description
            std::string license;                  ///< (AR) رخصة (EN) License
            std::vector<std::string> links;       ///< (AR) روابط (EN) Hyperlinks/references
            std::string defaultValue;             ///< (AR) قيمة افتراضية (EN) Default value

            /**
             * @brief (AR) هل التعليق فارغ؟ (EN) Is the comment empty?
             */
            bool isEmpty() const
            {
                return summary.empty() && description.empty() && params.empty();
            }

            /**
             * @brief (AR) تحويل إلى نص Markdown (EN) Convert to Markdown text
             */
            std::string toMarkdown() const
            {
                std::ostringstream ss;

                if (!summary.empty())
                {
                    ss << summary << "\n\n";
                }
                if (!description.empty())
                {
                    ss << description << "\n\n";
                }
                if (!params.empty())
                {
                    ss << "**المعطيات:**\n";
                    for (const auto &p : params)
                    {
                        ss << "- `" << p.name << "`: " << p.description << "\n";
                    }
                    ss << "\n";
                }
                if (!returnDescription.empty())
                {
                    ss << "**القيمة المرجعة:** " << returnDescription << "\n\n";
                }
                if (!exceptions.empty())
                {
                    ss << "**الاستثناءات:**\n";
                    for (const auto &e : exceptions)
                    {
                        ss << "- `" << e.type << "`: " << e.description << "\n";
                    }
                    ss << "\n";
                }
                if (!deprecated.empty())
                {
                    ss << "> **مهمل:** " << deprecated << "\n\n";
                }

                return ss.str();
            }
        };

        // =========================================================================
        // (AR) محلل تعليقات التوثيق / (EN) Documentation comment parser
        // =========================================================================

        /**
         * @brief (AR) أداة لتحليل النص الخام لتعليق توثيقي إلى بنية DocComment
         *        (EN) Utility to parse raw doc comment text into DocComment structure
         *
         * @details
         * (AR) يُستخدم عند الحاجة للوصول المنظم لوسوم التوثيق (LSP، توليد وثائق)
         * (EN) Used when structured access to doc tags is needed (LSP, doc generation)
         */
        class DocCommentParser
        {
        public:
            /**
             * @brief (AR) تحليل نص خام إلى DocComment
             *        (EN) Parse raw text into DocComment
             *
             * @param raw (AR) النص الخام من رمز DOC_COMMENT (EN) Raw text from DOC_COMMENT token
             * @return (AR) بنية DocComment محللة (EN) Parsed DocComment structure
             */
            static DocComment parse(const std::string &raw)
            {
                DocComment doc;

                if (raw.empty())
                    return doc;

                std::istringstream stream(raw);
                std::string line;
                std::ostringstream descBuffer;
                bool summaryDone = false;
                bool inExample = false;
                std::ostringstream exampleBuffer;
                // (AR) BF-04: حالة @example بدون كتلة ``` — نجمع السطور التالية
                //      حتى نلقى وسماً آخر أو سطراً فارغاً
                // (EN) BF-04: @example without ``` block — collect following lines
                //      until next tag or empty line
                bool inInlineExample = false;
                std::ostringstream inlineExampleBuffer;

                auto flushInlineExample = [&]()
                {
                    if (inInlineExample)
                    {
                        std::string ex = trim(inlineExampleBuffer.str());
                        if (!ex.empty())
                            doc.examples.push_back(ex);
                        inlineExampleBuffer.str("");
                        inInlineExample = false;
                    }
                };

                while (std::getline(stream, line))
                {
                    std::string trimmed = trim(line);

                    // (AR) معالجة كتل الأمثلة
                    // (EN) Handle example code blocks
                    if (inExample)
                    {
                        if (trimmed == "```" || trimmed == "```ص" || trimmed == "```sad")
                        {
                            // (AR) بداية أو نهاية كتلة كود
                            if (exampleBuffer.str().empty())
                            {
                                continue; // (AR) بداية الكتلة
                            }
                            doc.examples.push_back(trim(exampleBuffer.str()));
                            exampleBuffer.str("");
                            inExample = false;
                        }
                        else
                        {
                            exampleBuffer << line << "\n";
                        }
                        continue;
                    }

                    if (trimmed.substr(0, 3) == "```")
                    {
                        flushInlineExample();
                        inExample = true;
                        continue;
                    }

                    // (AR) معالجة الوسوم
                    // (EN) Handle tags
                    if (!trimmed.empty() && trimmed[0] == '@')
                    {
                        flushInlineExample();
                        // (AR) فحص خاص: @example/@مثال بدون نص → ادخل وضع inline
                        size_t spacePos = trimmed.find(' ', 1);
                        std::string tagOnly = (spacePos != std::string::npos)
                                                  ? trimmed.substr(1, spacePos - 1)
                                                  : trimmed.substr(1);
                        bool hasRest = (spacePos != std::string::npos) && !trim(trimmed.substr(spacePos + 1)).empty();
                        if (!hasRest && (tagOnly == "example" || tagOnly == "مثال"))
                        {
                            inInlineExample = true;
                            continue;
                        }
                        parseTag(trimmed, doc);
                        continue;
                    }

                    // (AR) داخل @example الـ inline: نجمع كل سطر غير فارغ
                    if (inInlineExample)
                    {
                        if (trimmed.empty())
                        {
                            flushInlineExample();
                        }
                        else
                        {
                            inlineExampleBuffer << line << "\n";
                        }
                        continue;
                    }

                    // (AR) الملخص = أول سطر غير فارغ
                    // (EN) Summary = first non-empty line
                    if (!summaryDone && !trimmed.empty())
                    {
                        doc.summary = trimmed;
                        summaryDone = true;
                        continue;
                    }

                    // (AR) الباقي يدخل في الوصف
                    // (EN) Rest goes into description
                    if (summaryDone)
                    {
                        descBuffer << line << "\n";
                    }
                }

                // (AR) تنظيف نهائي للحالات غير المغلقة
                flushInlineExample();

                doc.description = trim(descBuffer.str());
                return doc;
            }

        private:
            /**
             * @brief (AR) تحليل وسم واحد (EN) Parse a single tag
             */
            static void parseTag(const std::string &line, DocComment &doc)
            {
                // (AR) استخراج اسم الوسم والباقي
                // (EN) Extract tag name and rest
                size_t spacePos = line.find(' ', 1);
                std::string tagName = (spacePos != std::string::npos)
                                          ? line.substr(1, spacePos - 1)
                                          : line.substr(1);
                std::string rest = (spacePos != std::string::npos)
                                       ? trim(line.substr(spacePos + 1))
                                       : "";

                // @معطى / @param
                if (tagName == "معطى" || tagName == "param" || tagName == "parameter")
                {
                    DocParam param;
                    size_t nameEnd = rest.find(' ');
                    if (nameEnd != std::string::npos)
                    {
                        param.name = rest.substr(0, nameEnd);
                        param.description = trim(rest.substr(nameEnd + 1));
                    }
                    else
                    {
                        param.name = rest;
                    }
                    doc.params.push_back(std::move(param));
                }
                // @أرجع / @return
                else if (tagName == "أرجع" || tagName == "return" || tagName == "returns")
                {
                    doc.returnDescription = rest;
                }
                // @استثناء / @throws
                else if (tagName == "استثناء" || tagName == "throws" || tagName == "exception")
                {
                    DocException exc;
                    size_t nameEnd = rest.find(' ');
                    if (nameEnd != std::string::npos)
                    {
                        exc.type = rest.substr(0, nameEnd);
                        exc.description = trim(rest.substr(nameEnd + 1));
                    }
                    else
                    {
                        exc.type = rest;
                    }
                    doc.exceptions.push_back(std::move(exc));
                }
                // @انظر / @see
                else if (tagName == "انظر" || tagName == "see")
                {
                    if (!rest.empty())
                        doc.seeAlso.push_back(rest);
                }
                // @منذ / @since
                else if (tagName == "منذ" || tagName == "since")
                {
                    doc.since = rest;
                }
                // @مهمل / @deprecated
                else if (tagName == "مهمل" || tagName == "deprecated")
                {
                    doc.deprecated = rest.empty() ? "مهمل" : rest;
                }
                // @مؤلف / @author
                else if (tagName == "مؤلف" || tagName == "author")
                {
                    doc.author = rest;
                }
                // @ملاحظة / @note
                else if (tagName == "ملاحظة" || tagName == "note")
                {
                    if (!rest.empty())
                        doc.notes.push_back(rest);
                }
                // @تحذير / @warning
                else if (tagName == "تحذير" || tagName == "warning")
                {
                    if (!rest.empty())
                        doc.warnings.push_back(rest);
                }
                // @مثال / @example
                else if (tagName == "مثال" || tagName == "example")
                {
                    if (!rest.empty())
                        doc.examples.push_back(rest);
                }
                // @نسخة / @version
                else if (tagName == "نسخة" || tagName == "version")
                {
                    doc.version = rest;
                }
                // @نوع / @type
                else if (tagName == "نوع" || tagName == "type")
                {
                    doc.typeHint = rest;
                }
                // @تعقيد / @complexity
                else if (tagName == "تعقيد" || tagName == "complexity")
                {
                    doc.complexity = rest;
                }
                // @آمن_خيوط / @threadsafe
                else if (tagName == "آمن_خيوط" || tagName == "threadsafe" ||
                         tagName == "thread_safe" || tagName == "thread-safe")
                {
                    doc.threadSafety = rest.empty() ? "نعم" : rest;
                }
                // @تجريبي / @experimental
                else if (tagName == "تجريبي" || tagName == "experimental")
                {
                    doc.isExperimental = true;
                    doc.experimentalNote = rest;
                }
                // @رخصة / @license
                else if (tagName == "رخصة" || tagName == "license")
                {
                    doc.license = rest;
                }
                // @رابط / @link
                else if (tagName == "رابط" || tagName == "link" || tagName == "url")
                {
                    if (!rest.empty())
                        doc.links.push_back(rest);
                }
                // @افتراضي / @default
                else if (tagName == "افتراضي" || tagName == "default")
                {
                    doc.defaultValue = rest;
                }
            }

            /**
             * @brief (AR) إزالة المسافات من الأطراف (EN) Trim whitespace from both ends
             */
            static std::string trim(const std::string &s)
            {
                size_t start = s.find_first_not_of(" \t\n\r");
                if (start == std::string::npos)
                    return "";
                size_t end = s.find_last_not_of(" \t\n\r");
                return s.substr(start, end - start + 1);
            }
        };

    } // namespace AST
} // namespace Sad

#endif // SAD_AST_DOC_COMMENT_H
