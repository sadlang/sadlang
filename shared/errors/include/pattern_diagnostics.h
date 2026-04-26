/**
 * @file pattern_diagnostics.h
 * @brief (AR) تشخيصات تعارض الأنماط — اقتراحات الإصلاح والعرض البصري
 *        (EN) Pattern conflict diagnostics — fix suggestions and visual rendering
 *
 * المكون: shared/errors (نظام الأخطاء المشترك)
 * المسار: shared/errors/include/pattern_diagnostics.h
 *
 * (AR) يوفر هذا الملف أدواتين متكاملتين لتشخيص تعارضات أنماط match/طابق:
 *
 *   1. PatternFixGenerator — مُولِّد اقتراحات الإصلاح:
 *      - يُحلِّل التداخل بين نطاقين رقميين (مثال: 1..18 و 15..65)
 *      - يُولِّد اقتراحات مرتبة بالأولوية: تقسيم/نقل/إزالة مكرر
 *      - كل اقتراح يحمل التغييرات الدقيقة على الكود (CodeChange)
 *
 *   2. VisualPatternRenderer — مُولِّد التحذيرات البصرية:
 *      - يرسم تمثيلاً مرئياً لتداخل النطاقات بالرسم الحرفي (box-drawing)
 *      - يدعم ألوان ANSI للطرفيات الداعمة
 *      - يُظهر مقياس أرقام + شريطي النمط + نقطة التداخل الحمراء
 *
 *   الاستخدام الموصى به:
 *   @code
 *   // في كود الأخطاء عند اكتشاف تداخل
 *   Sad::Errors::Pattern::PatternFixGenerator gen;
 *   auto fixes = gen.suggestFixes({1, 18, "قاصر", 5}, {15, 65, "بالغ", 6});
 *   for (auto& f : fixes) diagnostic_output << f.format();
 *
 *   Sad::Errors::Pattern::VisualPatternRenderer renderer;
 *   std::cerr << renderer.renderOverlap(1,18,"قاصر", 15,65,"بالغ", 5,6);
 *   @endcode
 *
 *   استُبدلت به:
 *      - compiler/src/diagnostics/pattern_fix_suggestions.cpp (Fix #26)
 *      - compiler/src/diagnostics/visual_pattern_warning.cpp (Fix #26)
 *   السبب: هذه الأدوات تنفع المفسر والمترجم معاً → الموضع الصحيح shared/errors/
 *
 * (EN) Provides two complementary tools for diagnosing match/pattern conflicts:
 *      1. PatternFixGenerator: generates prioritized fix suggestions for overlapping ranges
 *      2. VisualPatternRenderer: renders box-drawing visual warnings with ANSI colors
 *
 * @author Sad Language Development Team
 * @version 2.0 (unified in shared/errors — Fix #26)
 */

#pragma once

// ─── القسم 1: رؤوس المشروع الضرورية فقط ────────────────────────────────────
#include "fix_suggestions.h"

// ─── القسم 2: رؤوس المكتبة القياسية ─────────────────────────────────────────
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace Sad
{
    namespace Errors
    {
        namespace Pattern
        {

            // ============================================================================
            // (AR) ألوان ANSI / (EN) ANSI Colors
            // ============================================================================

            namespace Ansi
            {
                inline const std::string RED = "\033[31m";
                inline const std::string GREEN = "\033[32m";
                inline const std::string YELLOW = "\033[33m";
                inline const std::string BLUE = "\033[34m";
                inline const std::string MAGENTA = "\033[35m";
                inline const std::string CYAN = "\033[36m";
                inline const std::string RESET = "\033[0m";
                inline const std::string BOLD = "\033[1m";
                inline const std::string UNDER = "\033[4m";
                inline const std::string BG_RED = "\033[41m";
                inline const std::string BG_YEL = "\033[43m";
            }

            // ============================================================================
            // (AR) نوع إصلاح الأنماط / (EN) Pattern Fix Type
            // ============================================================================

            /**
             * @enum PatternFixType
             * @brief (AR) نوع إصلاح تعارض الأنماط
             *        (EN) Type of pattern conflict fix
             */
            enum class PatternFixType
            {
                SPLIT_RANGES,      ///< (AR) تقسيم النطاقات المتداخلة / (EN) Split overlapping ranges
                REORDER,           ///< (AR) إعادة ترتيب الأنماط / (EN) Reorder patterns
                REMOVE_DUPLICATE,  ///< (AR) إزالة النمط المكرر / (EN) Remove duplicate pattern
                ADJUST_BOUNDS,     ///< (AR) ضبط حدود النطاقات / (EN) Adjust range boundaries
                ADD_GUARD,         ///< (AR) إضافة شرط حارس / (EN) Add guard condition
                CONVERT_TO_OR,     ///< (AR) تحويل لنمط بديل | / (EN) Convert to OR pattern
                MOVE_UP,           ///< (AR) نقل النمط للأعلى / (EN) Move pattern up
                DELETE_UNREACHABLE ///< (AR) حذف نمط غير مُتاح / (EN) Delete unreachable pattern
            };

            // ============================================================================
            // (AR) النطاق الرقمي / (EN) Numeric Range
            // ============================================================================

            /**
             * @struct NumericRange
             * @brief (AR) نطاق رقمي في نمط match
             *        (EN) Numeric range in a match pattern
             */
            struct NumericRange
            {
                int64_t min;      ///< (AR) بداية النطاق / (EN) Range start
                int64_t max;      ///< (AR) نهاية النطاق / (EN) Range end
                std::string name; ///< (AR) اسم النمط / (EN) Pattern name
                int line = 0;     ///< (AR) رقم السطر / (EN) Line number

                /**
                 * @brief (AR) التحقق من تداخل النطاق مع نطاق آخر
                 *        (EN) Check if this range overlaps with another
                 */
                bool overlaps(const NumericRange &other) const
                {
                    return min <= other.max && other.min <= max;
                }

                /**
                 * @brief (AR) التحقق من احتواء نطاق آخر بالكامل
                 *        (EN) Check if this range fully contains another
                 */
                bool contains(const NumericRange &other) const
                {
                    return min <= other.min && max >= other.max;
                }

                /**
                 * @brief (AR) التحقق من تطابق النطاقين
                 *        (EN) Check if ranges are identical
                 */
                bool equals(const NumericRange &other) const
                {
                    return min == other.min && max == other.max;
                }
            };

            // ============================================================================
            // (AR) تغيير في الكود / (EN) Code Change
            // ============================================================================

            /**
             * @struct CodeChange
             * @brief (AR) تعديل واحد في الكود المصدري
             *        (EN) Single modification in source code
             */
            struct CodeChange
            {
                int line;             ///< (AR) رقم السطر / (EN) Line number
                int col_start;        ///< (AR) عمود البداية / (EN) Start column
                int col_end;          ///< (AR) عمود النهاية / (EN) End column
                std::string old_text; ///< (AR) النص القديم / (EN) Old text
                std::string new_text; ///< (AR) النص الجديد / (EN) New text

                /**
                 * @brief (AR) تحويل التغيير لنص مقروء
                 *        (EN) Convert change to readable string
                 */
                std::string toString() const
                {
                    std::ostringstream ss;
                    ss << "سطر " << line << ": \"" << old_text << "\" → \"" << new_text << "\"";
                    return ss.str();
                }
            };

            // ============================================================================
            // (AR) اقتراح إصلاح النمط / (EN) Pattern Fix Suggestion
            // ============================================================================

            /**
             * @struct PatternFixSuggestion
             * @brief (AR) اقتراح إصلاح كامل لتعارض أنماط
             *        (EN) Complete fix suggestion for pattern conflict
             */
            struct PatternFixSuggestion
            {
                PatternFixType type;             ///< نوع الإصلاح
                std::string title_ar;            ///< العنوان بالعربية
                std::string title_en;            ///< العنوان بالإنجليزية
                std::string explanation_ar;      ///< الشرح بالعربية
                std::string explanation_en;      ///< الشرح بالإنجليزية
                std::vector<CodeChange> changes; ///< التغييرات المطلوبة
                int priority = 0;                ///< الأولوية (الأعلى = الأفضل)
                bool is_safe = true;             ///< هل الإصلاح آمن؟

                /**
                 * @brief (AR) تحويل الاقتراح لنص مُنسَّق
                 *        (EN) Convert suggestion to formatted text
                 */
                std::string format() const;
            };

            // ============================================================================
            // (AR) مولِّد اقتراحات إصلاح الأنماط / (EN) Pattern Fix Suggestion Generator
            // ============================================================================

            /**
             * @class PatternFixGenerator
             * @brief (AR) يُولِّد اقتراحات إصلاح لتعارضات الأنماط الرقمية
             *        (EN) Generates fix suggestions for numeric pattern conflicts
             *
             * (AR) يحلل التعارضات بين نطاقين رقميين ويقترح حلولاً مرتبة بالأولوية
             * (EN) Analyzes conflicts between two numeric ranges and suggests prioritized solutions
             *
             * @example
             * @code
             * Sad::Errors::Pattern::PatternFixGenerator gen;
             * auto r1 = Sad::Errors::Pattern::NumericRange{1, 18, "قاصر", 5};
             * auto r2 = Sad::Errors::Pattern::NumericRange{15, 65, "بالغ", 6};
             * auto suggestions = gen.suggestFixes(r1, r2);
             * for (auto& s : suggestions) std::cout << s.format();
             * @endcode
             */
            class PatternFixGenerator
            {
            public:
                /**
                 * @brief (AR) يُولِّد اقتراحات لتداخل نطاقين
                 *        (EN) Generates suggestions for overlapping ranges
                 *
                 * @param first  (AR) النطاق الأول / (EN) First range
                 * @param second (AR) النطاق الثاني / (EN) Second range
                 * @return (AR) قائمة الاقتراحات مرتبة تنازلياً بالأولوية
                 *         (EN) List of suggestions sorted by priority descending
                 */
                std::vector<PatternFixSuggestion> suggestFixes(
                    const NumericRange &first,
                    const NumericRange &second) const;

                /**
                 * @brief (AR) يُولِّد اقتراح لتقسيم نطاقين متداخلين
                 *        (EN) Generates split suggestion for two overlapping ranges
                 */
                PatternFixSuggestion makeSplitSuggestion(
                    const NumericRange &first,
                    const NumericRange &second) const;

                /**
                 * @brief (AR) يُولِّد اقتراح لنقل نمط محتوى للأعلى
                 *        (EN) Generates move-up suggestion for contained pattern
                 */
                PatternFixSuggestion makeMoveUpSuggestion(
                    const NumericRange &inner,
                    const NumericRange &outer) const;

                /**
                 * @brief (AR) يُولِّد اقتراح لإزالة نمط مكرر
                 *        (EN) Generates remove-duplicate suggestion
                 */
                PatternFixSuggestion makeRemoveDuplicateSuggestion(
                    const NumericRange &duplicate) const;
            };

            // ============================================================================
            // (AR) إعدادات العرض البصري / (EN) Visual Display Settings
            // ============================================================================

            /**
             * @struct VisualSettings
             * @brief (AR) إعدادات العرض البصري للتحذيرات
             *        (EN) Visual display settings for warnings
             */
            struct VisualSettings
            {
                int chart_width = 50;    ///< عرض الرسم بالأحرف
                bool use_colors = true;  ///< استخدم ألوان ANSI
                bool use_symbols = true; ///< استخدم رموز تداخل ▓▒█

                std::string sym_first = "▓";   ///< رمز النمط الأول
                std::string sym_second = "▒";  ///< رمز النمط الثاني
                std::string sym_overlap = "█"; ///< رمز التداخل
                std::string sym_empty = " ";   ///< رمز فراغ

                std::string box_tl = "┌"; ///< زاوية علوية يسرى
                std::string box_tr = "┐"; ///< زاوية علوية يمنى
                std::string box_bl = "└"; ///< زاوية سفلية يسرى
                std::string box_br = "┘"; ///< زاوية سفلية يمنى
                std::string box_h = "─";  ///< خط أفقي
                std::string box_v = "│";  ///< خط عمودي
            };

            // ============================================================================
            // (AR) مُولِّد التحذيرات البصرية / (EN) Visual Warning Renderer
            // ============================================================================

            /**
             * @class VisualPatternRenderer
             * @brief (AR) يُولِّد تمثيلاً بصرياً لتعارضات نطاقات الأنماط
             *        (EN) Generates visual representation of pattern range conflicts
             *
             * (AR) يستخدم رسم الصناديق (box-drawing) وألوان ANSI لعرض التداخل بوضوح
             * (EN) Uses box-drawing characters and ANSI colors to clearly display overlap
             *
             * @example
             * @code
             * Sad::Errors::Pattern::VisualPatternRenderer renderer;
             * std::string diagram = renderer.renderOverlap(1, 10, "صغير", 5, 15, "متوسط");
             * std::cout << diagram;
             * @endcode
             */
            class VisualPatternRenderer
            {
            public:
                /**
                 * @brief (AR) ينشئ مُولِّداً بإعدادات مخصصة
                 *        (EN) Constructs renderer with custom settings
                 */
                explicit VisualPatternRenderer(VisualSettings settings = {})
                    : settings_(std::move(settings)) {}

                /**
                 * @brief (AR) يرسم تحذيراً بصرياً لتداخل نطاقين
                 *        (EN) Renders visual warning for two overlapping ranges
                 *
                 * @param min1   (AR) بداية النطاق الأول / (EN) First range start
                 * @param max1   (AR) نهاية النطاق الأول / (EN) First range end
                 * @param name1  (AR) اسم النمط الأول / (EN) First pattern name
                 * @param min2   (AR) بداية النطاق الثاني / (EN) Second range start
                 * @param max2   (AR) نهاية النطاق الثاني / (EN) Second range end
                 * @param name2  (AR) اسم النمط الثاني / (EN) Second pattern name
                 * @param line1  (AR) رقم سطر النمط الأول / (EN) First pattern line number
                 * @param line2  (AR) رقم سطر النمط الثاني / (EN) Second pattern line number
                 * @return (AR) النص المُنسَّق مع رسم الصناديق / (EN) Formatted text with box-drawing
                 */
                std::string renderOverlap(
                    int64_t min1, int64_t max1, const std::string &name1,
                    int64_t min2, int64_t max2, const std::string &name2,
                    int line1 = 0, int line2 = 0) const;

                /**
                 * @brief (AR) يرسم قسماً واحداً من النطاق
                 *        (EN) Renders a single range bar
                 */
                std::string renderBar(
                    int64_t range_min, int64_t range_max,
                    int64_t bar_min, int64_t bar_max,
                    const std::string &symbol,
                    const std::string &color = "") const;

            private:
                VisualSettings settings_;

                void drawHLine(std::ostringstream &ss) const;
                void drawScale(std::ostringstream &ss, int64_t lo, int64_t hi) const;
                void drawTicks(std::ostringstream &ss, int64_t lo, int64_t hi) const;
            };

        } // namespace Pattern
    } // namespace Errors
} // namespace Sad
