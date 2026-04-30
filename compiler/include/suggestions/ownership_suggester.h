// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file ownership_suggester.h
 * @brief مُقترِح تحويل الملكية / Ownership Conversion Suggester
 *
 * نظام ذكي يحلل الكود المكتوب بوضع GC ويقترح كيفية تحويله
 * لنظام الملكية للحصول على أداء أفضل.
 *
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_SUGGESTIONS_OWNERSHIP_SUGGESTER_H
#define SAD_SUGGESTIONS_OWNERSHIP_SUGGESTER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <functional>

namespace Sad
{
    namespace Memory
    {

        // =============================================================================
        //                    أنواع الاقتراحات
        // =============================================================================

        /**
         * @brief مستوى خطورة الاقتراح
         */
        enum class SuggestionSeverity
        {
            Info,       // معلومة تعليمية
            Hint,       // تلميح اختياري
            Suggestion, // اقتراح مفيد
            Warning,    // تحذير: قد يكون هناك مشكلة
            Critical    // حرج: يجب التصحيح قبل الإنتاج
        };

        /**
         * @brief نوع الاقتراح
         */
        enum class SuggestionType
        {
            // اقتراحات الملكية
            UseOwned,    // استخدم قيمة مملوكة بدلاً من مشتركة
            UseBorrowed, // استخدم استعارة بدلاً من نسخ
            UseMove,     // انقل بدلاً من النسخ
            AddLifetime, // أضف تحديد عمر

            // اقتراحات المؤشرات الذكية
            UseRc,   // استخدم Rc للمشاركة
            UseArc,  // استخدم Arc للمشاركة الذرية
            UseWeak, // استخدم Weak لكسر الدورات

            // اقتراحات الأنماط
            UseOption, // استخدم اختياري بدلاً من null
            UseResult, // استخدم نتيجة بدلاً من الاستثناءات
            UseGuard,  // استخدم حارس RAII

            // اقتراحات الدورات
            PotentialCycle,     // دورة محتملة في المراجع
            BreakCycleWithWeak, // اكسر الدورة بـ Weak

            // اقتراحات الأداء
            AvoidAllocation,    // تجنب التخصيص غير الضروري
            UseStackAllocation, // استخدم المكدس بدلاً من الكومة
            PoolAllocation      // استخدم مجمع ذاكرة
        };

        // =============================================================================
        //                    بنية الاقتراح
        // =============================================================================

        /**
         * @brief موقع في الكود المصدري
         */
        struct SourceLocation
        {
            std::string filePath; // مسار الملف
            size_t line;          // رقم السطر
            size_t column;        // رقم العمود
            size_t endLine;       // نهاية السطر (للنطاقات)
            size_t endColumn;     // نهاية العمود

            std::string toString() const
            {
                return filePath + ":" + std::to_string(line) + ":" + std::to_string(column);
            }
        };

        /**
         * @brief اقتراح تحويل الملكية
         */
        struct OwnershipSuggestion
        {
            // الهوية
            std::string id;              // معرف فريد
            SuggestionType type;         // نوع الاقتراح
            SuggestionSeverity severity; // خطورة الاقتراح

            // الموقع
            SourceLocation location; // موقع في الكود

            // الرسائل
            std::string titleArabic;        // العنوان بالعربية
            std::string titleEnglish;       // العنوان بالإنجليزية
            std::string descriptionArabic;  // الوصف بالعربية
            std::string descriptionEnglish; // الوصف بالإنجليزية
            std::string explanationArabic;  // شرح تعليمي بالعربية
            std::string explanationEnglish; // شرح تعليمي بالإنجليزية

            // الكود
            std::string originalCode;  // الكود الأصلي
            std::string suggestedCode; // الكود المقترح

            // معلومات إضافية
            std::vector<std::string> relatedConcepts; // مفاهيم ذات صلة
            std::string documentationLink;            // رابط للتوثيق

            // الثقة
            float confidence;   // نسبة الثقة (0.0 - 1.0)
            bool isAutoFixable; // هل يمكن الإصلاح التلقائي؟
        };

        // =============================================================================
        //                    تقرير الاقتراحات
        // =============================================================================

        /**
         * @brief ملخص تقرير الاقتراحات
         */
        struct SuggestionReportSummary
        {
            size_t totalSuggestions = 0;
            size_t infoCount = 0;
            size_t hintCount = 0;
            size_t suggestionCount = 0;
            size_t warningCount = 0;
            size_t criticalCount = 0;
            size_t autoFixableCount = 0;

            // بحسب النوع
            std::unordered_map<SuggestionType, size_t> byType;

            // الإحصائيات
            float averageConfidence = 0.0f;
            float estimatedEffortHours = 0.0f;    // الجهد المقدر للتحويل
            float estimatedSpeedupPercent = 0.0f; // التحسن المتوقع في الأداء
        };

        /**
         * @brief تقرير اقتراحات الملكية الكامل
         */
        struct OwnershipSuggestionReport
        {
            // الاقتراحات
            std::vector<OwnershipSuggestion> suggestions;

            // الملخص
            SuggestionReportSummary summary;

            // معلومات المشروع
            std::string projectName;
            std::string analyzedPath;
            size_t filesAnalyzed = 0;
            size_t linesAnalyzed = 0;

            // التوصيات
            std::vector<std::string> recommendations; // توصيات عامة
            bool readyForProduction = false;          // جاهز للإنتاج؟
            std::string productionReadinessReason;    // سبب عدم الجاهزية
        };

        // =============================================================================
        //                    مُقترِح الملكية
        // =============================================================================

        /**
         * @brief خيارات المحلل
         */
        struct SuggesterOptions
        {
            bool includeInfoMessages = true;    // تضمين الرسائل المعلوماتية
            bool includeHints = true;           // تضمين التلميحات
            bool analyzePerformance = true;     // تحليل الأداء
            bool detectCycles = true;           // اكتشاف الدورات
            bool teacherMode = true;            // وضع المعلم (شروحات مفصلة)
            size_t maxSuggestionsPerFile = 100; // حد الاقتراحات لكل ملف
            float minimumConfidence = 0.5f;     // أقل ثقة مقبولة
        };

        /**
         * @brief مُقترِح تحويل الملكية
         *
         * يحلل الكود المكتوب بوضع التطوير (GC) ويقترح كيفية
         * تحويله لنظام الملكية للحصول على أداء أفضل في الإنتاج.
         */
        class OwnershipSuggester
        {
        public:
            /**
             * @brief إنشاء مُقترِح جديد
             */
            OwnershipSuggester();

            /**
             * @brief إنشاء مع خيارات مخصصة
             */
            explicit OwnershipSuggester(const SuggesterOptions &options);

            ~OwnershipSuggester();

            // ==========================================================================
            //                    التحليل الرئيسي
            // ==========================================================================

            /**
             * @brief تحليل ملف واحد
             * @param filePath مسار الملف
             * @return قائمة الاقتراحات
             */
            std::vector<OwnershipSuggestion> analyzeFile(const std::string &filePath);

            /**
             * @brief تحليل كود مصدري مباشرة
             * @param sourceCode الكود المصدري
             * @param fileName اسم الملف (للرسائل)
             * @return قائمة الاقتراحات
             */
            std::vector<OwnershipSuggestion> analyzeSource(
                const std::string &sourceCode,
                const std::string &fileName = "<input>");

            /**
             * @brief تحليل مشروع كامل
             * @param projectPath مسار المشروع
             * @return تقرير شامل
             */
            OwnershipSuggestionReport analyzeProject(const std::string &projectPath);

            // ==========================================================================
            //                    الفلترة والبحث
            // ==========================================================================

            /**
             * @brief فلترة بحسب الخطورة
             */
            static std::vector<OwnershipSuggestion> filterBySeverity(
                const std::vector<OwnershipSuggestion> &suggestions,
                SuggestionSeverity minSeverity);

            /**
             * @brief فلترة بحسب النوع
             */
            static std::vector<OwnershipSuggestion> filterByType(
                const std::vector<OwnershipSuggestion> &suggestions,
                SuggestionType type);

            /**
             * @brief الحصول على القابلة للإصلاح التلقائي فقط
             */
            static std::vector<OwnershipSuggestion> getAutoFixable(
                const std::vector<OwnershipSuggestion> &suggestions);

            // ==========================================================================
            //                    التقارير
            // ==========================================================================

            /**
             * @brief إنشاء تقرير نصي
             * @param suggestions قائمة الاقتراحات
             * @param arabic هل باللغة العربية؟
             * @return التقرير النصي
             */
            std::string generateTextReport(
                const std::vector<OwnershipSuggestion> &suggestions,
                bool arabic = true) const;

            /**
             * @brief إنشاء تقرير JSON
             */
            std::string generateJsonReport(
                const OwnershipSuggestionReport &report) const;

            /**
             * @brief إنشاء تقرير HTML تفاعلي
             */
            std::string generateHtmlReport(
                const OwnershipSuggestionReport &report) const;

            // ==========================================================================
            //                    الإصلاح التلقائي
            // ==========================================================================

            /**
             * @brief تطبيق الإصلاحات التلقائية
             * @param filePath مسار الملف
             * @param suggestions الاقتراحات للتطبيق
             * @return عدد الإصلاحات المطبقة
             */
            size_t applyAutoFixes(
                const std::string &filePath,
                const std::vector<OwnershipSuggestion> &suggestions);

            /**
             * @brief معاينة الإصلاحات قبل التطبيق
             * @return الكود بعد التعديلات المقترحة
             */
            std::string previewAutoFixes(
                const std::string &sourceCode,
                const std::vector<OwnershipSuggestion> &suggestions) const;

            // ==========================================================================
            //                    الإعدادات
            // ==========================================================================

            void setOptions(const SuggesterOptions &options);
            const SuggesterOptions &getOptions() const;

        private:
            SuggesterOptions options_;

            // أدوات التحليل الداخلية
            struct Impl;
            std::unique_ptr<Impl> impl_;

            // دوال التحليل الداخلية
            void analyzeVariableUsage(/* ... */);
            void analyzeReferencePatterns(/* ... */);
            void analyzeLifetimes(/* ... */);
            void analyzeAllocationPatterns(/* ... */);
            void detectPotentialCycles(/* ... */);

            // إنشاء الاقتراحات
            OwnershipSuggestion createSuggestion(
                SuggestionType type,
                SuggestionSeverity severity,
                const SourceLocation &location,
                const std::string &originalCode,
                const std::string &suggestedCode) const;

            // الرسائل المترجمة
            std::pair<std::string, std::string> getTitleForType(SuggestionType type) const;
            std::pair<std::string, std::string> getDescriptionForType(SuggestionType type) const;
            std::pair<std::string, std::string> getExplanationForType(SuggestionType type) const;
        };

        // =============================================================================
        //                    الدوال المساعدة
        // =============================================================================

        /**
         * @brief تحويل خطورة الاقتراح لنص عربي
         */
        std::string severityToArabic(SuggestionSeverity severity);

        /**
         * @brief تحويل خطورة الاقتراح لنص إنجليزي
         */
        std::string severityToEnglish(SuggestionSeverity severity);

        /**
         * @brief تحويل نوع الاقتراح لنص عربي
         */
        std::string suggestionTypeToArabic(SuggestionType type);

        /**
         * @brief تحويل نوع الاقتراح لنص إنجليزي
         */
        std::string suggestionTypeToEnglish(SuggestionType type);

        /**
         * @brief الحصول على لون الخطورة (للـ terminal)
         */
        std::string severityToAnsiColor(SuggestionSeverity severity);

    } // namespace Memory
} // namespace Sad

#endif // SAD_SUGGESTIONS_OWNERSHIP_SUGGESTER_H
