// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file ownership_suggester.cpp
 * @brief تنفيذ مُقترِح تحويل الملكية / Ownership Suggester Implementation
 *
 * هذا الملف يوفر نظاماً تعليمياً ذكياً يساعد المبرمجين على:
 * 1. فهم كيفية تحويل كود GC إلى نظام ملكية
 * 2. اكتشاف الأنماط غير الآمنة
 * 3. تعلم أفضل الممارسات في إدارة الذاكرة
 *
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "memory/ownership_suggester.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iomanip>
#include <filesystem>

namespace Sad
{
    namespace Memory
    {

        // =============================================================================
        //                    تنفيذ داخلي
        // =============================================================================

        struct OwnershipSuggester::Impl
        {
            // هنا سيتم إضافة AST والتحليل الفعلي لاحقاً
            // حالياً نوفر الهيكل الأساسي
        };

        // =============================================================================
        //                    البناء والهدم
        // =============================================================================

        OwnershipSuggester::OwnershipSuggester()
            : options_(SuggesterOptions{}), impl_(std::make_unique<Impl>())
        {
        }

        OwnershipSuggester::OwnershipSuggester(const SuggesterOptions &options)
            : options_(options), impl_(std::make_unique<Impl>())
        {
        }

        OwnershipSuggester::~OwnershipSuggester() = default;

        // =============================================================================
        //                    التحليل الرئيسي
        // =============================================================================

        std::vector<OwnershipSuggestion> OwnershipSuggester::analyzeFile(const std::string &filePath)
        {
            // قراءة الملف
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                return {};
            }

            std::stringstream buffer;
            buffer << file.rdbuf();

            return analyzeSource(buffer.str(), filePath);
        }

        std::vector<OwnershipSuggestion> OwnershipSuggester::analyzeSource(
            const std::string &sourceCode,
            const std::string &fileName)
        {
            std::vector<OwnershipSuggestion> suggestions;

            // تحليل بسيط للأنماط الشائعة
            // (في الإصدار الكامل سيتم استخدام AST)

            std::istringstream stream(sourceCode);
            std::string line;
            size_t lineNumber = 0;

            while (std::getline(stream, line))
            {
                lineNumber++;

                // ======================================================================
                // اكتشاف استخدام جديد (new) بدون ملكية واضحة
                // ======================================================================
                if (line.find("جديد ") != std::string::npos || line.find("جديد(") != std::string::npos)
                {
                    // التحقق من عدم وجود مؤشر ذكي
                    if (line.find("عم<") == std::string::npos &&
                        line.find("عم_ذري<") == std::string::npos &&
                        line.find("Rc<") == std::string::npos &&
                        line.find("Arc<") == std::string::npos)
                    {

                        SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};

                        auto suggestion = createSuggestion(
                            SuggestionType::UseRc,
                            SuggestionSeverity::Suggestion,
                            loc,
                            line,
                            "// استبدل بـ: عم<نوع>::أنشئ(...)");

                        suggestion.explanationArabic =
                            "استخدام 'جديد' مباشرة قد يسبب تسرب ذاكرة إذا نسيت تحرير الكائن.\n"
                            "بدلاً من ذلك، استخدم عم<نوع> (عدّاد مرجعي) لضمان تحرير الذاكرة تلقائياً.\n\n"
                            "مثال:\n"
                            "  // قبل:\n"
                            "  قائمة* ق = جديد قائمة()\n\n"
                            "  // بعد:\n"
                            "  عم<قائمة> ق = عم<قائمة>::أنشئ()";

                        suggestion.explanationEnglish =
                            "Using 'new' directly may cause memory leaks if you forget to free the object.\n"
                            "Instead, use Rc<Type> (reference counted) to ensure automatic memory release.\n\n"
                            "Example:\n"
                            "  // Before:\n"
                            "  List* l = new List()\n\n"
                            "  // After:\n"
                            "  Rc<List> l = Rc<List>::create()";

                        suggestion.relatedConcepts = {"عدّاد_مرجعي", "RAII", "إدارة_الذاكرة_التلقائية"};
                        suggestion.confidence = 0.85f;
                        suggestion.isAutoFixable = false; // يحتاج مراجعة

                        suggestions.push_back(suggestion);
                    }
                }

                // ======================================================================
                // اكتشاف مراجع قد تصبح معلقة
                // ======================================================================
                std::regex refReturnPattern(R"(ارجع\s*&)");
                if (std::regex_search(line, refReturnPattern))
                {
                    SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};

                    auto suggestion = createSuggestion(
                        SuggestionType::AddLifetime,
                        SuggestionSeverity::Warning,
                        loc,
                        line,
                        "// تحقق من عمر المرجع المُرجَع");

                    suggestion.explanationArabic =
                        "⚠️ إرجاع مرجع من دالة قد يكون خطيراً!\n\n"
                        "إذا كان المرجع يشير إلى متغير محلي، سيصبح معلقاً (dangling) بعد انتهاء الدالة.\n\n"
                        "الحالات الآمنة:\n"
                        "  ✓ إرجاع مرجع لحقل في self\n"
                        "  ✓ إرجاع مرجع لمعامل مُمرَّر بالمرجع\n"
                        "  ✓ إرجاع مرجع لمتغير عام\n\n"
                        "الحالات الخطرة:\n"
                        "  ✗ إرجاع مرجع لمتغير محلي\n"
                        "  ✗ إرجاع مرجع لكائن مُنشأ في الدالة";

                    suggestion.explanationEnglish =
                        "⚠️ Returning a reference from a function can be dangerous!\n\n"
                        "If the reference points to a local variable, it becomes dangling after function returns.\n\n"
                        "Safe cases:\n"
                        "  ✓ Return reference to self's field\n"
                        "  ✓ Return reference to a by-ref parameter\n"
                        "  ✓ Return reference to global variable\n\n"
                        "Dangerous cases:\n"
                        "  ✗ Return reference to local variable\n"
                        "  ✗ Return reference to object created in function";

                    suggestion.relatedConcepts = {"مراجع_معلقة", "عمر_المرجع", "Dangling_Reference"};
                    suggestion.confidence = 0.7f;
                    suggestion.isAutoFixable = false;

                    suggestions.push_back(suggestion);
                }

                // ======================================================================
                // اكتشاف أنماط الدورات المحتملة
                // ======================================================================
                if (options_.detectCycles)
                {
                    // نمط بسيط: كائن يحتوي على مرجع لنفس النوع
                    std::regex selfRefPattern(R"((\w+)\s*:\s*عم<\1>)");
                    std::smatch match;
                    if (std::regex_search(line, match, selfRefPattern))
                    {
                        SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};

                        auto suggestion = createSuggestion(
                            SuggestionType::PotentialCycle,
                            SuggestionSeverity::Warning,
                            loc,
                            line,
                            "// قد تحتاج استخدام مرجع_ضعيف<> لكسر الدورة");

                        suggestion.explanationArabic =
                            "🔄 تحذير: احتمال وجود دورة مراجع!\n\n"
                            "عندما يحتوي كائن على عم<> لنفس نوعه (أو أنواع تشير لبعضها)،\n"
                            "قد تتشكل دورة تمنع جامع القمامة من تحرير الكائنات.\n\n"
                            "الحل:\n"
                            "  • استخدم مرجع_ضعيف<> (Weak) للمراجع 'العكسية'\n"
                            "  • مثال: الأب يملك الأبناء بـ عم<>، الأبناء يشيرون للأب بـ مرجع_ضعيف<>\n\n"
                            "مثال الشجرة:\n"
                            "  صنف عقدة\n"
                            "    أبناء: مصفوفة<عم<عقدة>>      // ملكية قوية\n"
                            "    أب: مرجع_ضعيف<عقدة>         // مرجع ضعيف (لا يملك)\n"
                            "  نهاية";

                        suggestion.explanationEnglish =
                            "🔄 Warning: Potential reference cycle detected!\n\n"
                            "When an object contains Rc<> to its own type (or types that reference each other),\n"
                            "a cycle may form that prevents the GC from freeing the objects.\n\n"
                            "Solution:\n"
                            "  • Use Weak<> for 'back' references\n"
                            "  • Example: Parent owns children with Rc<>, children point to parent with Weak<>\n\n"
                            "Tree example:\n"
                            "  class Node\n"
                            "    children: Array<Rc<Node>>     // Strong ownership\n"
                            "    parent: Weak<Node>            // Weak reference (no ownership)\n"
                            "  end";

                        suggestion.relatedConcepts = {"دورة_مراجع", "مرجع_ضعيف", "Reference_Cycle"};
                        suggestion.confidence = 0.6f;
                        suggestion.isAutoFixable = false;

                        suggestions.push_back(suggestion);
                    }
                }

                // ======================================================================
                // اكتراح استخدام اختياري<> بدلاً من null
                // ======================================================================
                if (line.find("== لاشيء") != std::string::npos ||
                    line.find("!= لاشيء") != std::string::npos ||
                    line.find("== null") != std::string::npos ||
                    line.find("!= null") != std::string::npos)
                {

                    SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};

                    auto suggestion = createSuggestion(
                        SuggestionType::UseOption,
                        SuggestionSeverity::Hint,
                        loc,
                        line,
                        "// فكّر باستخدام اختياري<نوع> بدلاً من المقارنة مع null");

                    suggestion.explanationArabic =
                        "💡 استخدام اختياري<> أكثر أماناً من null!\n\n"
                        "المقارنة مع null/لاشيء قد تُنسى في بعض الأماكن، مما يسبب أخطاء.\n"
                        "نوع اختياري<> يجبرك على التعامل مع حالة 'لا قيمة' صراحةً.\n\n"
                        "قبل:\n"
                        "  مستخدم* م = ابحث_مستخدم(123)\n"
                        "  إذا (م != لاشيء)\n"
                        "    اطبع(م.اسم)\n"
                        "  نهاية\n\n"
                        "بعد:\n"
                        "  اختياري<مستخدم> م = ابحث_مستخدم(123)\n"
                        "  إذا (م.موجود())\n"
                        "    اطبع(م.قيمة().اسم)\n"
                        "  نهاية\n\n"
                        "أو بشكل أفضل:\n"
                        "  ابحث_مستخدم(123).تطابق(\n"
                        "    بعض: دالة(م) -> اطبع(م.اسم)،\n"
                        "    لاشيء: دالة() -> اطبع(\"غير موجود\")\n"
                        "  )";

                    suggestion.relatedConcepts = {"اختياري", "Option", "Null_Safety"};
                    suggestion.confidence = 0.75f;
                    suggestion.isAutoFixable = false;

                    suggestions.push_back(suggestion);
                }

                // ======================================================================
                // اقتراح نقل بدلاً من نسخ للكائنات الكبيرة
                // ======================================================================
                std::regex copyPattern(R"((\w+)\s*=\s*(\w+)\s*$)");
                std::smatch copyMatch;
                if (std::regex_search(line, copyMatch, copyPattern))
                {
                    // هذا تحليل مبسط - في الواقع نحتاج معرفة نوع المتغير
                    // حالياً نعرض كمعلومة تعليمية عامة
                }
            }

            // ترتيب بحسب الخطورة
            std::sort(suggestions.begin(), suggestions.end(),
                      [](const OwnershipSuggestion &a, const OwnershipSuggestion &b)
                      {
                          return static_cast<int>(a.severity) > static_cast<int>(b.severity);
                      });

            // تطبيق حد الاقتراحات
            if (suggestions.size() > options_.maxSuggestionsPerFile)
            {
                suggestions.resize(options_.maxSuggestionsPerFile);
            }

            return suggestions;
        }

        OwnershipSuggestionReport OwnershipSuggester::analyzeProject(const std::string &projectPath)
        {
            OwnershipSuggestionReport report;
            report.projectName = projectPath;
            report.analyzedPath = projectPath;

            // (AR) تحليل كل ملفات المشروع
            // (EN) Analyze all project files
            try
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator(projectPath))
                {
                    if (!entry.is_regular_file())
                        continue;
                    auto ext = entry.path().extension().string();
                    if (ext == ".sad" || ext == ".\xD8\xB5")
                    {
                        auto fileSuggestions = analyzeFile(entry.path().string());
                        // (AR) دمج النتائج
                        // (EN) Merge results
                        report.suggestions.insert(report.suggestions.end(),
                                                  fileSuggestions.begin(), fileSuggestions.end());
                        report.filesAnalyzed++;
                    }
                }
            }
            catch (const std::filesystem::filesystem_error &)
            {
                // (AR) تجاهل أخطاء نظام الملفات
                // (EN) Ignore filesystem errors
            }

            return report;
        }

        // =============================================================================
        //                    الفلترة والبحث
        // =============================================================================

        std::vector<OwnershipSuggestion> OwnershipSuggester::filterBySeverity(
            const std::vector<OwnershipSuggestion> &suggestions,
            SuggestionSeverity minSeverity)
        {
            std::vector<OwnershipSuggestion> filtered;
            std::copy_if(suggestions.begin(), suggestions.end(),
                         std::back_inserter(filtered),
                         [minSeverity](const OwnershipSuggestion &s)
                         {
                             return static_cast<int>(s.severity) >= static_cast<int>(minSeverity);
                         });
            return filtered;
        }

        std::vector<OwnershipSuggestion> OwnershipSuggester::filterByType(
            const std::vector<OwnershipSuggestion> &suggestions,
            SuggestionType type)
        {
            std::vector<OwnershipSuggestion> filtered;
            std::copy_if(suggestions.begin(), suggestions.end(),
                         std::back_inserter(filtered),
                         [type](const OwnershipSuggestion &s)
                         {
                             return s.type == type;
                         });
            return filtered;
        }

        std::vector<OwnershipSuggestion> OwnershipSuggester::getAutoFixable(
            const std::vector<OwnershipSuggestion> &suggestions)
        {
            std::vector<OwnershipSuggestion> filtered;
            std::copy_if(suggestions.begin(), suggestions.end(),
                         std::back_inserter(filtered),
                         [](const OwnershipSuggestion &s)
                         {
                             return s.isAutoFixable;
                         });
            return filtered;
        }

    } // namespace Memory
} // namespace Sad
