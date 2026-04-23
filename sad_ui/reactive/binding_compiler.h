/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: binding_compiler.h
 * المسار: sad_ui/reactive/binding_compiler.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) مُجمّع الربطات — يحلل لامدات لغة ص ويُنتج دوال تحويل C++ مباشرة
 * @brief (EN) Binding compiler — analyzes Sad lambdas and produces direct C++ transform functions
 *
 * الغرض:
 * ──────
 * في نظام التفاعل الدقيق (ADR-UI-10)، عندما يربط المبرمج إشارة بخاصية
 * عنصر واجهة عبر لامدا:
 *
 *   زر.عنوان = العداد.ربط(لامدا(ق) ارجع "العدد: " + نص(ق) نهاية)
 *
 * بدلاً من استدعاء المفسر الشجري (tree-walker) لتنفيذ هذه اللامدا
 * عند كل تغيير (بطيء ~100μs)، يحلل BindingCompiler عقدة AST اللامدا
 * ويحدد إذا كانت "بسيطة" بما يكفي لتحويلها إلى دالة C++ مباشرة (~1μs).
 *
 * 3 طبقات من الأداء:
 * ───────────────────
 * | الطبقة | الوصف | السرعة |
 * |--------|-------|--------|
 * | 1. ربط مباشر | خاصية = إشارة (بدون لامدا) | ~ميكروثانية (C++ فقط) |
 * | 2. لامدا مُجمّعة | BindingCompiler حلّل اللامدا → C++ | ~1μs |
 * | 3. لامدا مُفسَّرة | المفسر ينفذ اللامدا | ~100μs |
 *
 * العمليات المدعومة (قابلة للتجميع):
 * ────────────────────────────────────
 * - دمج نصوص: "مرحبا " + اسم → string concat
 * - تحويل أنواع: نص(رقم) → std::to_string()
 * - عمليات حسابية بسيطة: ق + 1، ق * 2
 * - مقارنات بسيطة: ق > 0
 * - الوصول لمعامل اللامدا مباشرة: لامدا(ق) ارجع ق نهاية
 *
 * العمليات التي تبقى مُفسَّرة:
 * ──────────────────────────────
 * - استدعاء دوال مخصصة (غير مضمنة)
 * - شروط if/else معقدة
 * - حلقات
 * - وصول لمتغيرات خارجية (closures)
 * - تعبيرات متداخلة أكثر من 3 مستويات
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_BINDING_COMPILER_H
#define SAD_UI_BINDING_COMPILER_H

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <cstdint>

// (AR) إعلان مسبق — لا نشمل ملفات AST هنا لأن BindingCompiler
//      يعمل على المستوى التفاعلي (reactive) وليس مستوى التحليل (parser)
// (EN) Forward declarations — we don't include AST headers here because
//      BindingCompiler operates at the reactive level, not the parser level
namespace Sad
{
    namespace AST
    {
        class Expression;
        class LambdaExpr;
        class BinaryExpr;
        class UnaryExpr;
        class LiteralExpr;
        class VariableExpr;
        class CallExpr;
    } // namespace AST
} // namespace Sad

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════
        // (AR) قيمة الربط — نوع موحد خفيف لقيم التحويل
        // (EN) Binding value — lightweight unified type for transform values
        // ═══════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) قيمة خفيفة الوزن لاستخدامها في دوال التحويل المُجمّعة
         * @brief (EN) Lightweight value for use in compiled transform functions
         *
         * بدلاً من استخدام Sad::Data::Value الثقيل (مع variant كامل)،
         * نستخدم هذا النوع الخفيف الذي يدعم فقط الأنواع الأساسية
         * اللازمة لتحويلات الربط الشائعة.
         */
        struct BindingValue
        {
            /// (AR) الأنواع المدعومة في قيم الربط
            /// (EN) Supported types in binding values
            enum class Kind : uint8_t
            {
                Null,    ///< لاشيء
                Integer, ///< رقم صحيح
                Double,  ///< رقم عشري
                String,  ///< نص
                Boolean  ///< منطقي
            };

            Kind kind = Kind::Null;

            // (AR) تخزين القيم — نستخدم variant بدلاً من union لأمان الأنواع
            // (EN) Value storage — using variant instead of union for type safety
            std::variant<std::monostate, int64_t, double, std::string, bool> data;

            // ─── المُنشئات ──────────────────────────────

            BindingValue() : kind(Kind::Null), data(std::monostate{}) {}

            explicit BindingValue(int64_t v) : kind(Kind::Integer), data(v) {}
            explicit BindingValue(int v) : kind(Kind::Integer), data(static_cast<int64_t>(v)) {}
            explicit BindingValue(double v) : kind(Kind::Double), data(v) {}
            explicit BindingValue(const std::string &v) : kind(Kind::String), data(v) {}
            explicit BindingValue(std::string &&v) : kind(Kind::String), data(std::move(v)) {}
            explicit BindingValue(bool v) : kind(Kind::Boolean), data(v) {}

            // ─── الوصول للقيم ────────────────────────────

            bool isNull() const { return kind == Kind::Null; }
            bool isInteger() const { return kind == Kind::Integer; }
            bool isDouble() const { return kind == Kind::Double; }
            bool isString() const { return kind == Kind::String; }
            bool isBoolean() const { return kind == Kind::Boolean; }
            bool isNumeric() const { return isInteger() || isDouble(); }

            int64_t asInteger() const { return std::get<int64_t>(data); }
            double asDouble() const { return std::get<double>(data); }
            const std::string &asString() const { return std::get<std::string>(data); }
            bool asBoolean() const { return std::get<bool>(data); }

            /// (AR) تحويل أي قيمة إلى نص (لعمليات الدمج)
            /// (EN) Convert any value to string (for concat operations)
            std::string toString() const;

            /// (AR) تحويل إلى عشري (للعمليات الحسابية المختلطة)
            /// (EN) Convert to double (for mixed arithmetic)
            double toDouble() const;

            /// (AR) مقارنة
            /// (EN) Comparison
            bool operator==(const BindingValue &other) const;
            bool operator!=(const BindingValue &other) const { return !(*this == other); }
        };

        // ═══════════════════════════════════════════════════════════════════
        // (AR) دالة التحويل المُجمّعة
        // (EN) Compiled transform function
        // ═══════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) نوع دالة التحويل — تأخذ قيمة الإشارة وتُرجع القيمة المحوّلة
         * @brief (EN) Transform function type — takes signal value and returns transformed value
         */
        using CompiledTransform = std::function<BindingValue(const BindingValue &input)>;

        // ═══════════════════════════════════════════════════════════════════
        // (AR) نتيجة التجميع
        // (EN) Compilation result
        // ═══════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) نتيجة محاولة تجميع لامدا
         * @brief (EN) Result of attempting to compile a lambda
         */
        struct CompilationResult
        {
            /// (AR) هل نجح التجميع؟
            /// (EN) Did compilation succeed?
            bool compiled = false;

            /// (AR) الدالة المُجمّعة (فارغة إذا فشل التجميع)
            /// (EN) Compiled function (empty if compilation failed)
            CompiledTransform transform;

            /// (AR) سبب فشل التجميع (للتصحيح)
            /// (EN) Reason for compilation failure (for debugging)
            std::string failureReason;

            /// (AR) عدد العقد في AST اللامدا (مقياس التعقيد)
            /// (EN) Node count in lambda AST (complexity metric)
            size_t nodeCount = 0;

            /// (AR) هل اللامدا تقرأ المعامل فقط (pure transform)؟
            /// (EN) Does the lambda only read the parameter (pure transform)?
            bool isPure = false;

            // ─── مُنشئات مساعدة ─────────────────────────

            /// (AR) نتيجة ناجحة
            /// (EN) Successful result
            static CompilationResult success(CompiledTransform fn, size_t nodes, bool pure)
            {
                CompilationResult r;
                r.compiled = true;
                r.transform = std::move(fn);
                r.nodeCount = nodes;
                r.isPure = pure;
                return r;
            }

            /// (AR) نتيجة فاشلة
            /// (EN) Failure result
            static CompilationResult failure(const std::string &reason)
            {
                CompilationResult r;
                r.compiled = false;
                r.failureReason = reason;
                return r;
            }
        };

        // ═══════════════════════════════════════════════════════════════════
        // (AR) نوع عقدة التعبير المُحلّل (IR مصغّر داخلي)
        // (EN) Analyzed expression node type (internal mini-IR)
        // ═══════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) عقدة تعبير مُحلّلة — تمثيل وسيط بين AST ودالة C++
         * @brief (EN) Analyzed expression node — intermediate representation between AST and C++ function
         *
         * بدلاً من تحويل AST → C++ مباشرة (هش)، نبني IR مصغّراً:
         *   AST → BindingExprNode → CompiledTransform (C++ function)
         *
         * هذا يسمح بـ:
         * 1. التحقق من الصحة قبل التجميع
         * 2. تحسينات بسيطة (طيّ الثوابت)
         * 3. تحديد التعقيد بدقة
         */
        struct BindingExprNode
        {
            /// (AR) أنواع العقد في IR المصغّر
            /// (EN) Node types in the mini-IR
            enum class Type : uint8_t
            {
                // ─── قيم حرفية ───
                LiteralInteger, ///< عدد صحيح ثابت
                LiteralDouble,  ///< عدد عشري ثابت
                LiteralString,  ///< نص ثابت
                LiteralBoolean, ///< قيمة منطقية ثابتة

                // ─── مراجع ───
                ParamRef, ///< إشارة لمعامل اللامدا (ق)

                // ─── عمليات ───
                BinaryAdd, ///< جمع (+) — حسابي أو دمج نصوص
                BinarySub, ///< طرح (-)
                BinaryMul, ///< ضرب (*)
                BinaryDiv, ///< قسمة (/)
                BinaryMod, ///< باقي القسمة (%)
                BinaryGt,  ///< أكبر من (>)
                BinaryLt,  ///< أصغر من (<)
                BinaryGte, ///< أكبر من أو يساوي (>=)
                BinaryLte, ///< أصغر من أو يساوي (<=)
                BinaryEq,  ///< يساوي (==)
                BinaryNeq, ///< لا يساوي (!=)
                BinaryAnd, ///< و المنطقي (&&)
                BinaryOr,  ///< أو المنطقي (||)

                UnaryNeg, ///< سالب (-)
                UnaryNot, ///< نفي (!)

                // ─── تحويل أنواع مضمن ───
                ToStringBuiltin, ///< نص(ق) — تحويل لنص
                ToIntBuiltin,    ///< رقم(ق) — تحويل لرقم
                ToDoubleBuiltin, ///< عشري(ق) — تحويل لعشري
                ToBoolBuiltin,   ///< منطقي(ق) — تحويل لمنطقي
            };

            Type type;

            // ─── بيانات حسب النوع ─────────────────────

            // (AR) قيم حرفية
            int64_t intVal = 0;
            double doubleVal = 0.0;
            std::string strVal;
            bool boolVal = false;

            // (AR) اسم المعامل المرجعي (ParamRef)
            std::string paramName;

            // (AR) أبناء العمليات الثنائية/الأحادية/التحويل
            std::unique_ptr<BindingExprNode> left;
            std::unique_ptr<BindingExprNode> right; // (AR) فارغ للعمليات الأحادية

            // ─── مُنشئات مساعدة ─────────────────────

            static std::unique_ptr<BindingExprNode> makeLiteralInt(int64_t v);
            static std::unique_ptr<BindingExprNode> makeLiteralDouble(double v);
            static std::unique_ptr<BindingExprNode> makeLiteralString(const std::string &v);
            static std::unique_ptr<BindingExprNode> makeLiteralBool(bool v);
            static std::unique_ptr<BindingExprNode> makeParamRef(const std::string &name);
            static std::unique_ptr<BindingExprNode> makeBinary(Type op,
                                                               std::unique_ptr<BindingExprNode> l,
                                                               std::unique_ptr<BindingExprNode> r);
            static std::unique_ptr<BindingExprNode> makeUnary(Type op,
                                                              std::unique_ptr<BindingExprNode> operand);
            static std::unique_ptr<BindingExprNode> makeBuiltinConvert(Type conv,
                                                                       std::unique_ptr<BindingExprNode> arg);

            /// (AR) حساب عدد العقد (لقياس التعقيد)
            /// (EN) Count nodes (for complexity measurement)
            size_t countNodes() const;
        };

        // ═══════════════════════════════════════════════════════════════════
        // (AR) مُجمّع الربطات — المكوّن الرئيسي
        // (EN) Binding compiler — main component
        // ═══════════════════════════════════════════════════════════════════

        /**
         * @class BindingCompiler
         * @brief (AR) يحلل لامدات لغة ص ويحوّلها لدوال C++ عند الإمكان
         * @brief (EN) Analyzes Sad lambdas and converts them to C++ functions when possible
         *
         * مخطط العمل:
         * ───────────
         *   1. analyze(LambdaExpr*) → BindingExprNode (IR مصغّر)
         *   2. validate(BindingExprNode) → هل التعقيد مقبول؟
         *   3. compile(BindingExprNode) → CompiledTransform (C++ function)
         *
         * مثال:
         * ─────
         *   BindingCompiler compiler;
         *   auto result = compiler.compile(lambdaExpr);
         *   if (result.compiled) {
         *       // استخدم result.transform بدلاً من المفسر
         *       auto val = result.transform(BindingValue(42));
         *   } else {
         *       // fallback إلى المفسر
         *   }
         *
         * إعدادات:
         * ────────
         * - maxNodeCount: الحد الأقصى لعقد التعبير (افتراضي: 20)
         * - maxDepth: الحد الأقصى لعمق التداخل (افتراضي: 5)
         */
        class BindingCompiler
        {
        public:
            BindingCompiler() = default;

            // ═══════════════════════════════════════════════════════
            // (AR) التجميع — الواجهة الرئيسية
            // (EN) Compilation — main interface
            // ═══════════════════════════════════════════════════════

            /**
             * @brief (AR) محاولة تجميع لامدا AST إلى دالة C++ مباشرة
             * @brief (EN) Attempt to compile an AST lambda to a direct C++ function
             *
             * @param lambda مؤشر لعقدة اللامدا في AST (لا ينقل الملكية)
             * @return نتيجة التجميع — .compiled==true إذا نجح
             */
            CompilationResult compile(const Sad::AST::LambdaExpr *lambda);

            // ═══════════════════════════════════════════════════════
            // (AR) التحليل — تحويل AST → IR مصغّر (عام للاختبار)
            // (EN) Analysis — AST → mini-IR (public for testing)
            // ═══════════════════════════════════════════════════════

            /**
             * @brief (AR) تحليل تعبير AST وتحويله لعقدة IR مصغّرة
             * @brief (EN) Analyze an AST expression and convert to mini-IR node
             *
             * @param expr التعبير المراد تحليله
             * @param paramName اسم معامل اللامدا (لتمييز المراجع)
             * @return عقدة IR مصغّرة، أو nullptr إذا التعبير غير مدعوم
             */
            std::unique_ptr<BindingExprNode> analyze(const Sad::AST::Expression *expr,
                                                     const std::string &paramName);

            /**
             * @brief (AR) تجميع عقدة IR مصغّرة إلى دالة C++
             * @brief (EN) Compile a mini-IR node to a C++ function
             *
             * @param node عقدة IR المُحلّلة
             * @return دالة التحويل، أو nullptr إذا فشل التجميع
             */
            CompiledTransform compileNode(const BindingExprNode *node);

            // ═══════════════════════════════════════════════════════
            // (AR) الإعدادات
            // (EN) Configuration
            // ═══════════════════════════════════════════════════════

            /**
             * @brief (AR) الحد الأقصى لعدد عقد التعبير
             * @brief (EN) Maximum expression node count
             *
             * لامدات أكبر من هذا الحد تُرفض وتبقى مُفسَّرة.
             * القيمة الافتراضية 20 تغطي 90%+ من الحالات العملية.
             */
            static constexpr size_t DEFAULT_MAX_NODE_COUNT = 20;

            /**
             * @brief (AR) الحد الأقصى لعمق التداخل
             * @brief (EN) Maximum nesting depth
             */
            static constexpr size_t DEFAULT_MAX_DEPTH = 5;

            void setMaxNodeCount(size_t count) { maxNodeCount_ = count; }
            void setMaxDepth(size_t depth) { maxDepth_ = depth; }

            size_t getMaxNodeCount() const { return maxNodeCount_; }
            size_t getMaxDepth() const { return maxDepth_; }

            // ═══════════════════════════════════════════════════════
            // (AR) إحصائيات (للقياس والتصحيح)
            // (EN) Statistics (for benchmarking and debugging)
            // ═══════════════════════════════════════════════════════

            struct Stats
            {
                size_t totalAttempts = 0;   ///< إجمالي المحاولات
                size_t successCount = 0;    ///< عدد التجميعات الناجحة
                size_t failureCount = 0;    ///< عدد الفشل
                size_t tooComplexCount = 0; ///< عدد اللامدات المعقدة جداً
            };

            const Stats &getStats() const { return stats_; }
            void resetStats() { stats_ = Stats{}; }

        private:
            // ═══════════════════════════════════════════════════════
            // (AR) محللات AST الداخلية (كل نوع عقدة AST له محلل)
            // (EN) Internal AST analyzers (each AST node type has its analyzer)
            // ═══════════════════════════════════════════════════════

            std::unique_ptr<BindingExprNode> analyzeBinary(const Sad::AST::BinaryExpr *expr,
                                                           const std::string &paramName);

            std::unique_ptr<BindingExprNode> analyzeUnary(const Sad::AST::UnaryExpr *expr,
                                                          const std::string &paramName);

            std::unique_ptr<BindingExprNode> analyzeVariable(const Sad::AST::VariableExpr *expr,
                                                             const std::string &paramName);

            std::unique_ptr<BindingExprNode> analyzeFunctionCall(const Sad::AST::CallExpr *expr,
                                                                 const std::string &paramName);

            std::unique_ptr<BindingExprNode> analyzeLiteral(const Sad::AST::LiteralExpr *expr);

            // ═══════════════════════════════════════════════════════
            // (AR) فحص التعقيد
            // (EN) Complexity checks
            // ═══════════════════════════════════════════════════════

            /**
             * @brief (AR) فحص عمق التداخل لا يتجاوز الحد الأقصى
             * @brief (EN) Check nesting depth doesn't exceed maximum
             */
            bool checkDepth(const BindingExprNode *node, size_t currentDepth = 0) const;

            /**
             * @brief (AR) هل يقرأ التعبير المعامل فقط (بدون متغيرات خارجية)؟
             * @brief (EN) Does the expression only read the parameter (no external variables)?
             */
            bool isPureExpression(const BindingExprNode *node) const;

            // ═══════════════════════════════════════════════════════
            // (AR) تقييم العقد — بناء دالة C++ من IR المصغّر
            // (EN) Node evaluation — build C++ function from mini-IR
            // ═══════════════════════════════════════════════════════

            /**
             * @brief (AR) تقييم عقدة واحدة بالقيمة المُدخلة
             * @brief (EN) Evaluate a single node with given input value
             */
            static BindingValue evaluateNode(const BindingExprNode *node,
                                             const BindingValue &input);

            // ─── الحقول ──────────────────────────────────

            size_t maxNodeCount_ = DEFAULT_MAX_NODE_COUNT;
            size_t maxDepth_ = DEFAULT_MAX_DEPTH;
            Stats stats_;
        };

    } // namespace ui
} // namespace sad

#endif // SAD_UI_BINDING_COMPILER_H
