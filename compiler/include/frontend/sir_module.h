// ======================================================================
// sir_module.h - وحدة SIR / SIR Module
// ======================================================================
// الوصف بالعربية:
//   تمثيل وحدة SIR الكاملة (برنامج كامل)
//   تحتوي على دوال، متغيرات عامة، وأصناف
//
// English Description:
//   Representation of complete SIR module (whole program)
//   Contains functions, global variables, and classes
//
// الميزات / Features:
//   - إدارة الدوال / Function management
//   - المتغيرات العامة / Global variables
//   - الأصناف والبنى / Classes and structs
//   - التحقق من الصحة / Validation
//
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: December 2, 2025
// الإصدار / Version: 1.0
// ======================================================================

#pragma once

#include "sir_types.h"
#include "sir_instruction.h"
#include "sir_constants.h" // (AR) kSadNullSentinel لتبطين الوسيط المُغفَل / (EN) null pad sentinel
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <memory>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // Forward declarations
            class SIRFunction;
            class SIRGlobalVariable;
            class SIRClass;

            // ======================================================================
            // (AR) سمة SIR / (EN) SIR Trait
            // ======================================================================

            /**
             * @brief (AR) معلومات دالة مطلوبة في السمة
             * @brief (EN) Required method info in a trait
             */
            struct SIRTraitMethod
            {
                std::string name;                           ///< (AR) اسم الدالة / (EN) Method name
                std::vector<SadTypeKind> paramTypes;        ///< (AR) أنواع المعاملات / (EN) Parameter types
                SadTypeKind returnType = SadTypeKind::Void; ///< (AR) نوع الإرجاع / (EN) Return type
                bool hasDefaultImpl = false;                ///< (AR) هل لها تنفيذ افتراضي / (EN) Has default impl
            };

            /**
             * @brief (AR) تعريف سمة/واجهة
             * @brief (EN) Trait/interface definition
             */
            struct SIRTrait
            {
                std::string name;                     ///< (AR) اسم السمة / (EN) Trait name
                std::vector<SIRTraitMethod> methods;  ///< (AR) الدوال المطلوبة / (EN) Required methods
                std::vector<std::string> superTraits; ///< (AR) السمات الأب / (EN) Super traits
            };

            // ======================================================================
            // فئة الوحدة / Module Class
            // ======================================================================

            /**
             * @brief (AR) وحدة SIR - تمثل برنامج كامل
             * @brief (EN) SIR Module - represents a complete program
             *
             * الوحدة هي أعلى مستوى في التمثيل الوسيط وتحتوي على:
             * Module is the top-level in IR and contains:
             * - قائمة الدوال / List of functions
             * - متغيرات عامة / Global variables
             * - تعريفات الأصناف / Class definitions
             * - ثوابت النصوص / String constants
             *
             * @example
             * @code{.cpp}
             * SIRModule module("my_program");
             *
             * // إضافة دالة / Add function
             * auto func = std::make_shared<SIRFunction>("main", makeVoidType());
             * module.addFunction(func);
             *
             * // إضافة متغير عام / Add global variable
             * auto globalVar = std::make_shared<SIRGlobalVariable>("PI", makeF64Type(), 3.14159);
             * module.addGlobalVariable(globalVar);
             * @endcode
             */
            class SIRModule
            {
            public:
                std::string name; ///< (AR) اسم الوحدة / (EN) Module name

                /**
                 * @brief (AR) منشئ الوحدة
                 * @brief (EN) Module constructor
                 *
                 * @param moduleName اسم الوحدة / Module name
                 */
                explicit SIRModule(const std::string &moduleName);

                /**
                 * @brief (AR) إضافة دالة
                 * @brief (EN) Add function
                 *
                 * @param function الدالة المراد إضافتها / Function to add
                 */
                void addFunction(std::shared_ptr<SIRFunction> function);

                /**
                 * @brief (AR) الحصول على دالة بالاسم
                 * @brief (EN) Get function by name
                 *
                 * @param name اسم الدالة / Function name
                 * @return مؤشر للدالة أو nullptr / Pointer to function or nullptr
                 */
                std::shared_ptr<SIRFunction> getFunction(const std::string &name) const;

                /**
                 * @brief (AR) الحصول على جميع الدوال
                 * @brief (EN) Get all functions
                 *
                 * @return قائمة الدوال / List of functions
                 */
                const std::vector<std::shared_ptr<SIRFunction>> &getFunctions() const;

                /**
                 * @brief (AR) إضافة متغير عام
                 * @brief (EN) Add global variable
                 *
                 * @param globalVar المتغير العام / Global variable
                 */
                void addGlobalVariable(std::shared_ptr<SIRGlobalVariable> globalVar);

                /**
                 * @brief (AR) الحصول على متغير عام بالاسم
                 * @brief (EN) Get global variable by name
                 *
                 * @param name اسم المتغير / Variable name
                 * @return مؤشر للمتغير أو nullptr / Pointer to variable or nullptr
                 */
                std::shared_ptr<SIRGlobalVariable> getGlobalVariable(const std::string &name) const;

                /**
                 * @brief (AR) إضافة صنف
                 * @brief (EN) Add class
                 *
                 * @param sirClass الصنف / Class
                 */
                void addClass(std::shared_ptr<SIRClass> sirClass);

                /**
                 * @brief (AR) الحصول على صنف بالاسم
                 * @brief (EN) Get class by name
                 *
                 * @param name اسم الصنف / Class name
                 * @return مؤشر للصنف أو nullptr / Pointer to class or nullptr
                 */
                std::shared_ptr<SIRClass> getClass(const std::string &name) const;

                /**
                 * @brief (AR) إضافة ثابت نصي
                 * @brief (EN) Add string constant
                 *
                 * @param str النص / String
                 * @return معرف الثابت / Constant ID
                 */
                std::string addStringConstant(const std::string &str);

                /**
                 * @brief (AR) التحقق من صحة الوحدة
                 * @brief (EN) Validate module
                 *
                 * @return true إذا كانت صحيحة / if valid
                 */
                bool validate() const;

                /**
                 * @brief (AR) طباعة الوحدة بصيغة SIR
                 * @brief (EN) Print module in SIR format
                 *
                 * @return النص الممثل للوحدة / String representation
                 */
                std::string toString() const;

                /**
                 * @brief (AR) طباعة الوحدة للـ console
                 * @brief (EN) Print module to console
                 */
                void print() const;

                /**
                 * @brief (AR) الحصول على عدد الدوال
                 * @brief (EN) Get function count
                 */
                size_t getFunctionCount() const { return functions_.size(); }

                /**
                 * @brief (AR) الحصول على عدد المتغيرات العامة
                 * @brief (EN) Get global variable count
                 */
                size_t getGlobalCount() const { return globalVariables_.size(); }

                /**
                 * @brief (AR) الحصول على إجمالي عدد التعليمات
                 * @brief (EN) Get total instruction count
                 */
                size_t getTotalInstructions() const;

                /**
                 * @brief (AR) الحصول على قائمة المتغيرات العامة
                 * @brief (EN) Get global variables list
                 */
                const std::vector<std::shared_ptr<SIRGlobalVariable>> &getGlobalVariables() const
                {
                    return globalVariables_;
                }

                /**
                 * @brief (AR) الحصول على قائمة الثوابت
                 * @brief (EN) Get constants list
                 */
                const std::vector<std::string> &getConstants() const
                {
                    return stringConstants_;
                }

                /**
                 * @brief (AR) الحصول على قائمة الأصناف
                 * @brief (EN) Get classes list
                 */
                const std::vector<std::shared_ptr<SIRClass>> &getClasses() const
                {
                    return classes_;
                }

                /**
                 * @brief (AR) تسجيل سمة
                 * @brief (EN) Register a trait
                 */
                void addTrait(const SIRTrait &trait)
                {
                    traits_[trait.name] = trait;
                }

                /**
                 * @brief (AR) الحصول على سمة بالاسم
                 * @brief (EN) Get trait by name
                 */
                const SIRTrait *getTrait(const std::string &name) const
                {
                    auto it = traits_.find(name);
                    return it != traits_.end() ? &it->second : nullptr;
                }

                /**
                 * @brief (AR) هل السمة موجودة
                 * @brief (EN) Does trait exist
                 */
                bool hasTrait(const std::string &name) const
                {
                    return traits_.find(name) != traits_.end();
                }

            private:
                std::vector<std::shared_ptr<SIRFunction>> functions_;
                std::unordered_map<std::string, std::shared_ptr<SIRFunction>> functionMap_;

                std::vector<std::shared_ptr<SIRGlobalVariable>> globalVariables_;
                std::unordered_map<std::string, std::shared_ptr<SIRGlobalVariable>> globalVarMap_;

                std::vector<std::shared_ptr<SIRClass>> classes_;
                std::unordered_map<std::string, std::shared_ptr<SIRClass>> classMap_;

                std::unordered_map<std::string, SIRTrait> traits_;

                std::vector<std::string> stringConstants_;
                std::unordered_map<std::string, std::string> stringConstantMap_;
                int nextStringId_;
            };

            // ======================================================================
            // فئة المعامل / Parameter Struct
            // ======================================================================

            /**
             * @brief (AR) معامل دالة
             * @brief (EN) Function parameter
             */
            struct SIRParameter
            {
                std::string name;                            ///< (AR) اسم المعامل / (EN) Parameter name
                SadTypeKind type;                            ///< (AR) نوع المعامل / (EN) Parameter type
                SadTypeKind elementType = SadTypeKind::Void; ///< (AR) نوع عنصر المصفوفة (للمصفوفات) / (EN) Array element type (for arrays)
                /// (AR) اسم الصنف للمعامل المصرَّح بصنفٍ مسجَّل (نحو `دالة معالج(حدث ح)`) —
                ///      تبذره الخلفيّة في objectClassMap فلا يُترك حلّ الحقول لتخمين الاسم
                ///      (معالِج الحدث لا يُستدعى من كود ص فلا يصله استدلال مواقع الاستدعاء).
                /// (EN) Class name for a param declared with a registered class (e.g.
                ///      `دالة معالج(حدث ح)`) — the backend seeds objectClassMap from it so
                ///      field resolution never relies on the name-guess fallback (event
                ///      handlers are never called from ص code, so call-site inference
                ///      cannot reach them).
                std::string className;

                /// (AR) SEM045: النوع السطحيّ **المصرَّح** للمعامل من تعليق الـAST خامًّا —
                ///      لا `type` الذي قد يُرقّى بالاستنتاج. حارسُ المعامل في موضع النداء
                ///      يقرأ هذا الحقلَ لا سجلَّ `declaredTypedSlots`: ذاك فضاءٌ مسطّحٌ
                ///      يخلط المحلّيّات بالمعاملات، فمحلّيٌّ مصنَّفٌ باسم معاملٍ مجرّدٍ كان
                ///      يُحرَس كذبًا، ومحلّيٌّ مجرّدٌ باسم معاملٍ مصنَّفٍ كان يمحو حراستَه
                ///      (إيجابيٌّ كاذبٌ وسالبٌ كاذبٌ قِيسا معًا — المراجعة العدائية).
                /// (EN) SEM045: the parameter's RAW declared surface type from the AST
                ///      annotation (not the inference-promoted `type`). The call-site
                ///      param guard reads THIS, not declaredTypedSlots — that record's
                ///      flat namespace mixes locals with params, producing a measured
                ///      false-positive/false-negative pair under name collision.
                SadTypeKind declaredSurfaceType = SadTypeKind::Unknown;

                SIRParameter(const std::string &paramName, SadTypeKind paramType)
                    : name(paramName), type(paramType), elementType(SadTypeKind::Void) {}

                std::string toString() const;
            };

            // ======================================================================
            // (AR) تبطينُ الوسائطِ المُغفَلةِ بـ«عدم» — سلطةٌ واحدةٌ لكلِّ مواضعِ الاستدعاء
            // (EN) Padding omitted arguments with null — one authority for all call sites
            // ======================================================================

            /**
             * @brief (AR) هل يُخفَّض هذا النوعُ إلى **مؤشّرٍ** في الخلفيّة؟
             * @brief (EN) Does this kind lower to a **pointer** in the backend?
             *
             * (AR) المرجعُ الوحيد: `LLVMTypeMapper::mapSIRType` — الأنواعُ المعدودةُ هنا
             *      هي عينُ التي تُرجِعُ منها `getStringPtrType()`.
             * (EN) Single reference: `LLVMTypeMapper::mapSIRType` — the kinds listed here
             *      are exactly those that return `getStringPtrType()` there.
             */
            inline bool sirKindLowersToPointer(SadTypeKind kind)
            {
                return kind == SadTypeKind::Pointer ||
                       kind == SadTypeKind::String ||
                       kind == SadTypeKind::Array ||
                       kind == SadTypeKind::Struct ||
                       kind == SadTypeKind::Map ||
                       kind == SadTypeKind::Function ||
                       kind == SadTypeKind::Error;
            }

            /**
             * @brief (AR) هل يعبرُ حارسُ العدمِ i64 إلى هذا النوعِ **سليمًا**؟
             * @brief (EN) Does the i64 null sentinel reach this kind **intact**?
             *
             * (AR) الخلفيّةُ تُكيّفُ الوسيطَ إلى نوعِ معاملِ المستدعَى (cf_branch_call): فحارسُ
             *      i64 يبقى حارسًا إن كان المعاملُ i64، ويُشوَّه فيما عداه —
             *      `SIToFP` يجعلُه عشريًّا هائلًا، و`Trunc` إلى i1/i8 يجعلُه 1، و`IntToPtr`
             *      يجعلُه مؤشّرًا شاردًا. فلا يُبطَّنُ بالعدمِ إلّا ما يحفظُه.
             * (EN) The backend coerces each argument to the callee's parameter type
             *      (cf_branch_call): the i64 sentinel survives only into an i64 parameter and is
             *      corrupted otherwise — `SIToFP` turns it into a huge double, `Trunc` to i1/i8
             *      turns it into 1, `IntToPtr` turns it into a wild pointer. So only kinds that
             *      preserve it are padded with null.
             */
            inline bool sirKindPreservesNullSentinel(SadTypeKind kind)
            {
                return kind == SadTypeKind::Integer ||
                       kind == SadTypeKind::UInt64 ||
                       kind == SadTypeKind::Null ||
                       kind == SadTypeKind::Unknown;
            }

            /**
             * @brief (AR) هل يحتاج النوعُ العدميُّ `T؟` وسمًا **خارجَ نطاقِ** القيمة؟
             * @brief (EN) Does the nullable type `T?` need an **out-of-band** tag?
             *
             * (AR) 🔑 العقدُ المُقاس: العدمُ ينجو حيثما كانت الخانةُ ٦٤ بتًّا (`i64`/`ptr`)،
             *      ويُدمَّر حيثما ضاق العرضُ أو تبدّلت الدلالة. و`منطقي` معامِلًا يُخفَّض
             *      `i1`، فوسيطُ `لاشيء` ووسيطُ `صحيح` **بتّاتٌ متطابقةٌ حرفيًّا** —
             *      فلا مستهلِكَ في الدنيا يفرّق بينهما، وأيُّ حارسٍ عند الطابعِ أو عند
             *      `طابق` رقعةٌ في الطبقةِ الخطأ.
             *      فتُخزَّن هذه الأنواعُ في `Any` أي `%SadDyn` = {وسمٌ i8، حمولةٌ i64}:
             *      وسمٌ خارجَ النطاقِ لا تنازعُه قيمةٌ مشروعة.
             *      وهذا **لا يبني تمثيلًا جديدًا** — يُوجّه العدميَّ إلى تمثيلٍ قائمٍ
             *      مقيسٍ (ISSUE-063/076): المسحُ المسبق يبذر منه، و`mem_alloca` يختاره،
             *      و`dynToString`/`dynCompare`/`emitPhi` تستهلكه.
             * (EN) 🔑 Measured contract: null survives wherever the slot is 64-bit
             *      (`i64`/`ptr`) and is destroyed wherever the width narrows or the
             *      semantics change. A `منطقي` parameter lowers to `i1`, so the `لاشيء`
             *      argument and the `صحيح` argument are **literally identical bits** — no
             *      consumer can tell them apart, and a guard at the printer or at `طابق`
             *      would be a patch in the wrong layer. Such types are therefore stored as
             *      `Any`, i.e. `%SadDyn` = {i8 tag, i64 payload}: an out-of-band tag that no
             *      legitimate value can contend for. This builds **no new representation** —
             *      it routes nullables into the existing, measured one (ISSUE-063/076).
             *
             * ⚠️ (AR) هذه سلطةٌ **واحدة**؛ نسخةٌ ثانيةٌ تسهو عن نوعٍ تُعيد التباعُدَ الذي
             *      وُضِعت لسدِّه. وبقيّةُ الأنواعِ العدميّةِ تُضاف هنا وحدَه (م‑ج فما بعدها).
             * ⚠️ (EN) ONE authority; a second copy that forgets a kind reinstates the very
             *      divergence this closes. Remaining nullable kinds are added here alone.
             */
            inline bool sirNullableNeedsOutOfBandTag(SadTypeKind innerKind)
            {
                // (AR) م‑ب: `منطقي؟` — المعامِلُ كان `i1`، فوسيطُ `لاشيء` ووسيطُ `صحيح`
                //      بتّاتٌ متطابقةٌ حرفيًّا؛ لا مستهلِكَ يفرّق بينهما.
                // (EN) م‑ب: `bool?` — the parameter was `i1`, so the `null` and `true`
                //      arguments were literally identical bits.
                if (innerKind == SadTypeKind::Boolean)
                {
                    return true;
                }

                // (AR) م‑ج: `عشري؟` — الخانةُ `double` والحارسُ i64، فالتكييفُ `sitofp`
                //      يُحوّله عددًا عشريًّا هائلًا (`-9223372036854775808.0`) لا وسمًا.
                //      وهو **تحويلُ دلالةٍ لا بترُ عرض**: الخانةُ ٦٤ بتًّا كاملةً ومع
                //      ذلك يضيع العدم — فالعرضُ وحدَه ليس شرطَ النجاة.
                //      ⚠️ ولا يُعالَج ببثِّ بتّاتِ الحارسِ في `double`: نمطُ البتّات
                //      `0x8000000000000001` **عددٌ عشريٌّ شرعيٌّ** (تحت-طبيعيّ)، فحسابٌ
                //      يُنتجه بالضبط يُقرَأ «لاشيء» — يُمدَّد التصادمُ ولا يُلغى.
                // (EN) م‑ج: `float?` — the slot is `double` while the sentinel is i64, so
                //      the `sitofp` coercion turns it into a huge float, not a tag. This is
                //      a CHANGE OF MEANING, not a width truncation: the slot is a full 64
                //      bits and null still dies — width alone is not the survival condition.
                //      ⚠️ Not fixable by bit-casting the sentinel into the `double`: the bit
                //      pattern 0x8000000000000001 IS a legitimate (subnormal) float, so a
                //      computation producing it exactly would read as null — that extends
                //      the collision instead of ending it.
                if (innerKind == SadTypeKind::Float)
                {
                    return true;
                }

                // (AR) م‑د: `بايت؟` و`طبيعي؟` — وخانتاهما `i64` تحفظان الحارسَ
                //      بتًّا ببتّ، فليست العلّةُ تكييفًا كما في `منطقي` و`عشري`.
                //      العلّةُ **تصادمُ قيمةٍ شرعيّة**: `kSadNullSentinel` عددٌ
                //      صحيحٌ مشروعٌ في كِلا النوعَين، فكان فحصُ الحارسِ **مُستثنًى
                //      فيهما عمدًا** لئلّا تُقرأ قيمةٌ حقيقيّةٌ عدمًا. والاستثناءُ
                //      ثمنُه أنّ العدمَ لا يُقرَأ عدمًا أبدًا.
                //      والوسمُ خارجَ النطاقِ يُنهي المفاضلةَ: لا استثناءَ ولا تصادم.
                //
                // (AR) 🔑 وهذا الصفُّ يُكمِل قاعدةَ النجاة: العدمُ يموت في ثلاثِ
                //      صورٍ لا صورتَين — **بترُ عرضٍ** (`منطقي` ⇒ i1) · **تحويلُ
                //      دلالةٍ** (`عشري` ⇒ sitofp) · و**تصادمُ قيمةٍ شرعيّةٍ**
                //      (`بايت`/`طبيعي` ⇒ الخانةُ سليمةٌ والحارسُ غيرُ مميَّز).
                //      والثالثةُ لا يُصلحها عرضٌ ولا تكييفٌ — **الوسمُ وحدَه**.
                // (EN) م‑د: `byte?` and `uint64?` — their `i64` slots preserve the
                //      sentinel bit for bit, so the cause is not coercion as with bool
                //      and float. The cause is a LEGAL-VALUE COLLISION: the sentinel is
                //      a legitimate integer in both, so the null check was deliberately
                //      EXCLUDED for them lest a real value read as null — at the price
                //      that null never reads as null. An out-of-band tag ends the
                //      trade-off: no exclusion, no collision.
                //      🔑 This completes the survival rule: null dies in THREE ways, not
                //      two — width truncation, meaning change, and legal-value collision.
                //      The third is curable by neither width nor cast — only by a tag.
                return innerKind == SadTypeKind::UInt8 ||
                       innerKind == SadTypeKind::UInt64;
            }

            /**
             * @brief (AR) نوعُ التخزينِ لِـ`T؟` — إمّا `T` نفسُه وإمّا `Any` (وسمٌ خارجَ النطاق)
             * @brief (EN) The storage kind for `T?` — either `T` itself or `Any` (out-of-band tag)
             */
            inline SadTypeKind sirNullableStorageKind(SadTypeKind innerKind)
            {
                return sirNullableNeedsOutOfBandTag(innerKind) ? SadTypeKind::Any : innerKind;
            }

            /**
             * @brief (AR) يبني قيمةَ التبطينِ لخانةِ معاملٍ واحدةٍ لم يبلغْها وسيط
             * @brief (EN) Builds the pad value for a single parameter slot no argument reached
             *
             * (AR) الخانةُ غيرُ المُمرَّرةِ ولا الافتراضيّةُ **عدمٌ** لا فراغٌ ولا صفر — نظيرُ
             *      ما يفعلُه المفسّرُ عند ربطِ معاملاتِ الباني. بلا تبطينٍ تُبطّنُ الخلفيّةُ
             *      بـ`Constant::getNullValue` أي **صفرًا**، والصفرُ يساوي رقمًا مشروعًا فتعطي
             *      `س == لاشيء` خطأً في المترجَمِ وصحيحًا في المفسّر ⇒ تباعُدُ محرّكَين.
             *      والوسمُ `Null` (لا `Integer`) هو وسمُ حرفيِّ `لاشيء` نفسِه، فتتوافقُ معه
             *      مساراتُ المقارنةِ و`??` و`نوع()` وتغليفُ «أي» (toDyn).
             *      والمعاملُ غيرُ المُصرَّحِ بنوعٍ يُخفَّض إلى `Integer` (astTypeToSIRType:
             *      `Unknown ⇒ Integer`)، وهو الحالُ الغالبُ في المكتبةِ القياسيّة، فيُبطَّنُ
             *      بالعدمِ كما ينبغي.
             * (EN) A slot neither passed nor defaulted is **null** — not void, not zero —
             *      mirroring the interpreter's constructor parameter binding. Without padding
             *      the backend pads with `Constant::getNullValue`, i.e. **zero**, which is a
             *      legitimate number, so `x == null` is false in compiled code and true in the
             *      interpreter: an engine divergence. The `Null` tag (not `Integer`) is the very
             *      tag the `لاشيء` literal carries, so comparison/`??`/`نوع()`/`أي`-boxing agree.
             *      An untyped parameter lowers to `Integer` (astTypeToSIRType: `Unknown ⇒
             *      Integer`) — the prevailing case across the standard library — so it does get
             *      the null pad.
             *
             * 🚧 (AR) ثغرةٌ معلَنةٌ لا يُدَّعى سدُّها: الخانةُ التي لا يعبرُها الحارسُ سليمًا
             *      (نصٌّ · مصفوفةٌ · خريطةٌ · دالّةٌ · عشريٌّ · منطقيٌّ · بايت) تُبطَّنُ
             *      **بصفرِ نوعِها** — وهو عينُ ما تُبطّنُ به الخلفيّةُ اليوم، فلا يتغيّرُ
             *      سلوكُها ولا يُزرَعُ فيها مؤشّرٌ شاردٌ ولا `صحيح` كاذب. تمثيلُ العدمِ في
             *      هذه الأنواعِ يحتاجُ عقدًا مستقلًّا يُقاسُ على المحرّكَين.
             * 🚧 (EN) Declared gap, not claimed closed: a slot the sentinel cannot reach intact
             *      (string · array · map · function · float · boolean · byte) is padded with
             *      **its own type's zero** — exactly what the backend pads with today, so its
             *      behaviour is unchanged and it grows neither a wild pointer nor a bogus
             *      `true`. Representing null in those types needs its own dual-engine contract.
             *
             * (AR) وهذه صيغةُ الخانةِ الواحدة، ليشتركَ فيها موضعُ الاستدعاءِ ومُضمِّنُ
             *      الأماميّةِ (sir_frontend_optimizer_passes2) على منطقٍ واحد: فالمُضمِّنُ
             *      يستبدلُ المعاملَ في **جسمِ** المستدعَى مباشرةً بلا مرورٍ بتكييفِ الخلفيّة،
             *      فالحارسُ أعلاه هو حِمايتُه الوحيدة. ولهذا يُشارَكُ المنطقُ ولا يُكرَّر:
             *      نسخةٌ ثانيةٌ تسهو عن الحارسِ تزرعُ مؤشّرًا شاردًا لا تُصحّحُه الخلفيّة.
             * (EN) The single-slot form, so the call site and the frontend inliner
             *      (sir_frontend_optimizer_passes2) share one rule: the inliner substitutes the
             *      parameter straight into the callee's **body**, bypassing the backend's
             *      coercion, so the guard above is its only protection. Hence sharing rather than
             *      duplicating: a second copy that forgot the guard would plant a wild pointer
             *      the backend never gets to fix.
             *
             * @param declaredKind (AR) نوعُ المعاملِ المُصرَّحِ الذي لم يبلغْه وسيط
             */
            inline SIROperand makeOmittedArgPad(SadTypeKind declaredKind)
            {
                if (sirKindPreservesNullSentinel(declaredKind))
                {
                    SIROperand nullPad = SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel);
                    nullPad.dataType = SadTypeKind::Null;
                    return nullPad;
                }

                // (AR) خانةُ `Any` تحمل الوسمَ **خارجَ** النطاق، فالعدمُ يبلغُها سليمًا:
                //      الوسمُ `Null` هو ما تُعلّبه `toDyn` وتقرؤه `dynToString`/`dynCompare`.
                //      وهذه هي خانةُ كلِّ نوعٍ عدميٍّ يُرجِعُ له `sirNullableNeedsOutOfBandTag`
                //      صوابًا، فبطنُها بصفرِها كان يزرعُ `خطأ` مكانَ `لاشيء`.
                // (EN) An `Any` slot carries its tag OUT of band, so null reaches it intact:
                //      the `Null` tag is what `toDyn` packs and `dynToString`/`dynCompare`
                //      read. This is the slot of every nullable kind for which
                //      `sirNullableNeedsOutOfBandTag` is true; padding it with its zero
                //      planted `false` where `null` belonged.
                if (declaredKind == SadTypeKind::Any)
                {
                    SIROperand dynNullPad = SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel);
                    dynNullPad.dataType = SadTypeKind::Null;
                    return dynNullPad;
                }

                // (AR) صفرُ النوعِ — مطابقٌ لـ`Constant::getNullValue` نوعًا بنوع.
                // (EN) The type's zero — matches `Constant::getNullValue` kind for kind.
                if (declaredKind == SadTypeKind::Float)
                {
                    return SIROperand::ConstantF64(0.0);
                }

                if (declaredKind == SadTypeKind::Boolean)
                {
                    return SIROperand::ConstantBool(false);
                }

                if (sirKindLowersToPointer(declaredKind))
                {
                    // (AR) وسمُ `Pointer` يجعلُ الخلفيّةَ تُصدِر `IntToPtr(0)` أي مؤشّرًا
                    //      صفريًّا — لا `Constant String` بمحتوى فارغ.
                    // (EN) The `Pointer` tag makes the backend emit `IntToPtr(0)`, i.e. a
                    //      null pointer — not an empty string constant.
                    SIROperand pointerPad = SIROperand::ConstantI64(0);
                    pointerPad.dataType = SadTypeKind::Pointer;
                    return pointerPad;
                }

                SIROperand zeroPad = SIROperand::ConstantI64(0);
                zeroPad.dataType = declaredKind;
                return zeroPad;
            }

            /**
             * @brief (AR) يُبطّنُ `args` حتّى يبلغَ عددَ المعاملاتِ المُصرَّحةِ للمستدعَى
             * @brief (EN) Pads `args` up to the callee's declared parameter count
             *
             * (AR) صيغةُ موضعِ الاستدعاء: كلُّ خانةٍ زائدةٍ تأخذُ ما يبنيه
             *      `makeOmittedArgPad` لنوعِها. ولا يمسُّ الوسائطَ المُمرَّرةَ فعلًا.
             * (EN) The call-site form: every surplus slot takes whatever
             *      `makeOmittedArgPad` builds for its kind. Arguments actually passed are
             *      left untouched.
             *
             * @param declaredParams (AR) معاملاتُ المستدعَى المُصرَّحة
             * @param args           (AR) الوسائطُ — تُوسَّعُ في مكانِها
             */
            inline void padOmittedArgsWithNull(const std::vector<SIRParameter> &declaredParams,
                                               std::vector<SIROperand> &args)
            {
                for (size_t index = args.size(); index < declaredParams.size(); ++index)
                {
                    args.push_back(makeOmittedArgPad(declaredParams[index].type));
                }
            }

            // ======================================================================
            // فئة الدالة / Function Class
            // ======================================================================

            /**
             * @brief (AR) دالة SIR
             * @brief (EN) SIR Function
             *
             * تمثل دالة كاملة مع معاملاتها وجسمها من التعليمات
             * Represents a complete function with parameters and instruction body
             */
            class SIRFunction
            {
            public:
                std::string name;                                        ///< (AR) اسم الدالة / (EN) Function name
                std::string linkName;                                    ///< (AR) اسم الربط الخارجي (FFI) / (EN) FFI link name (empty = use function name)
                SadTypeKind returnType;                                  ///< (AR) نوع الإرجاع / (EN) Return type

                /// (AR) [RFC عقد الغياب — موجة وسم حدّ المعامل] هل نوعُ الإرجاعِ **مصرَّحٌ
                ///      في المصدر** (`دالة رقم ضاعف(...)`) لا مستنتَجٌ؟ التصريحُ عقدُ
                ///      الكاتبِ: ممرُّ الخاناتِ الديناميّةِ في الخلفيّةِ يرقّي عائدَ دالّةٍ
                ///      تُرجِع قيمةً موسومةً إلى «أي» (%SadDyn) كي لا يُقتطَع الوسمُ عند
                ///      الحدّ — وذلك صوابٌ للعائدِ المستنتَجِ وكذبٌ على المصرَّح: عشريٌّ
                ///      موسومٌ عائدًا من «دالة رقم» كان يفلت 5.0 بينما يقسره المفسّرُ
                ///      إلى 5 عند الحدِّ المصرَّح (قِيس — كشفُ المراجعةِ العدائيّة).
                ///      فالممرُّ يحترم هذا العلَمَ ويُبقي العائدَ المصرَّحَ على تمثيلِه،
                ///      وبابُ RET يفكُّ الموسومَ بوسمِه (unpackI64: عشريّ⇒fptosi).
                /// (EN) Is the return type SOURCE-DECLARED (author's contract) rather than
                ///      inferred? The backend dyn-slot pass promotes a function returning
                ///      a tagged value to an Any (%SadDyn) return so the tag is not
                ///      stripped at the boundary — right for inferred returns, a lie for
                ///      declared ones: a Float-tagged value escaping a declared «رقم»
                ///      printed 5.0 where the interpreter coerces to 5 (measured,
                ///      adversarial review). The pass honours this flag; the RET door
                ///      unpacks by tag (unpackI64: Float⇒fptosi).
                bool returnTypeIsDeclared = false;

                /// (AR) النوعُ **السطحيُّ** المُصرَّحُ للعائدِ (رقم8/طبيعي16/…) قبلَ خفضِه
                ///      إلى نوعِ الخزن. و`returnType` أعلاه نوعُ خزنٍ: «الخيار ب» يجعلُه
                ///      i64 لكلِّ عرضٍ دونَ ٦٤، فسؤالُه عن العرضِ يُجيبُ دائمًا «٦٤ فلا
                ///      بترَ» — ومقيسٌ أنّ `دالة رقم8` تُرجِع ٤٠٠ فتُطبَعُ ٤٠٠ في
                ///      المحرّكَين معًا. وهذا نظيرُ `declaredSurfaceType` على المتغيّر.
                ///      Unknown = لا تصريحَ سطحيّ (مستنتَج) ⇒ لا تطبيع.
                /// (EN) The DECLARED SURFACE return kind before lowering to a storage kind.
                ///      `returnType` above is storage: Option B makes it i64 for every
                ///      sub-64 width, so asking it for the width always answers "64, no
                ///      truncation". Sibling of VariableInfo::declaredSurfaceType.
                SadTypeKind declaredSurfaceReturnType = SadTypeKind::Unknown;

                bool isConstructor = false;                              ///< (AR) بانٍ — يملك خانات حقول الكائن فلا يُدمج سطريًّا / (EN) Constructor — owns object field slots, never inlined

                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 هل بُنيت هذه الدالّةُ عبرَ مسارِ الدوالِّ العامِّ الذي يمرُّ
                //      بـ`emitDeferFrameBegin`؟ (الدوالُّ الحرّةُ والمتداخلةُ والإغلاقات
                //      — لا الطرائقُ ولا البناة.)
                //
                //      ولمَ يُسجَّلُ هذا أصلًا: حتّى ٢٦ آب ٢٠٢٦ كانَ ذلك المسارُ يُصدِرُ
                //      سقّالةَ تأجيلٍ **بلا شرط**، فتزيدُ كتلَ الدالّةِ فوقَ حدِّ
                //      «كتلتَين» في `FunctionInliningFrontendPass::shouldInline` —
                //      أي أنّ الدمجَ السطريَّ كانَ **ميتًا** لكلِّ ما بُنيَ بهذا المسار،
                //      وحيًّا للطرائقِ وحدَها. ثمّ رُفِعَ حارسُ السقّالةِ (مكسبٌ مقيسٌ
                //      ٢٦ ضعفًا في زمنِ الترجمة) فاشتعلَ المَمَرُّ لهذه الدوالِّ لأوّلِ
                //      مرّةٍ **فاحمرّت ٢٦ بذرة**.
                //
                //      فهذا العلَمُ يفصلُ الأمرَين: يُبقي أهليّةَ الدمجِ **كما كانت
                //      بالضبط** قبلَ تغييرِ السقّالة، فلا يحملُ مكسبُ الأداءِ معه
                //      تشغيلَ مُحسِّنٍ لم يُبرهَنْ قطُّ. والتفصيلُ الكاملُ لعائلاتِ
                //      عطبِه في `shouldInline`.
                //
                //      ⚠️ وليسَ علَمَ «فيها أجّل» — إصدارُ السقّالةِ نفسُه صارَ مشروطًا
                //      بالمسحِ الساكن. هذا علَمُ **المسارِ** لا علَمُ المحتوى.
                // (EN) 🔑 Was this function built through the general function path that
                //      goes via `emitDeferFrameBegin` (free/nested functions and closures —
                //      not methods, not constructors)? Until 2026-08-26 that path emitted a
                //      defer scaffold unconditionally, pushing the block count past the
                //      "2 blocks" cutoff in FunctionInliningFrontendPass::shouldInline —
                //      so frontend inlining was DEAD for everything built that way and live
                //      only for methods. Lifting the scaffold gate (a measured 26×
                //      compile-time win) lit that path up for the first time and turned 26
                //      seeds red. This flag separates the two concerns: inlining eligibility
                //      stays exactly what it was, so the performance win does not smuggle in
                //      an optimizer that was never proven. See shouldInline for the fault
                //      families. NOTE: not a "has defer" flag — scaffold emission is now
                //      driven by a static scan. This marks the PATH, not the content.
                // ════════════════════════════════════════════════════════════
                bool usesFunctionDeferFramePath = false;
                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 نوعُ **القيمةِ** التي يحملُها وعدُ الكوروتين — لا نوعُ ما
                //      تُرجِعه الدالّة. فالكوروتينُ يُرجِعُ مقبضَ إطارٍ (`Pointer`)
                //      دائمًا، والقيمةُ الحقيقيّةُ تُخزَّن في `coro.promise`.
                //      و`returnType` يُدهَسُ بـ`Pointer` عند وسمِ الدالّةِ كوروتينًا،
                //      فيضيعُ النوعُ المستنتَجُ من `ارجع` ولا يبقى له حاملٌ — ومَن
                //      يقرأُ الوعدَ بعدَه لا يملكُ إلّا عرضَ وعائه (i64).
                //      ولذلك كان `انتظر` على دالّةٍ تُرجِعُ نصًّا يطبعُ **عنوانًا**.
                // (EN) The type of the VALUE the coroutine's promise carries — not what
                //      the function returns, which is always a frame handle. `returnType`
                //      is overwritten with Pointer when the function is marked a
                //      coroutine, so the type inferred from `ارجع` had no carrier left,
                //      and whoever read the promise knew only its container's width.
                // ════════════════════════════════════════════════════════════
                SadTypeKind coroutineValueType = SadTypeKind::Void;

                bool isCoroutine = false;                                ///< (AR) دالة غير متزامنة (كوروتين) / (EN) Async function (coroutine)
                bool isGenerator = false;                                ///< (AR) دالة مولّد / (EN) Generator function
                bool isExported = false;                                 ///< (AR) دالة مُصدّرة (صدّر) / (EN) Exported function (export)
                bool isNoReturn = false;                                 ///< (AR) دالة لا_ترجع — لا تعود أبداً / (EN) noreturn → LLVM NoReturn (brick 3.15)
                bool isInterruptHandler = false;                         ///< (AR) دالة مقاطعة — تُخفَّض باتّفاقيّة x86_intrcc (RFC 0059) / (EN) interrupt handler → CallingConv::X86_INTR
                bool isExtern = false;                                   ///< (AR) دالة خارجية (استيراد FFI)؟ — يفعّل تمرير/إرجاع بنية @تمثيل_سي بالقيمة [RFC #53 F2-ج] / (EN) External (FFI-imported) function? — enables C-repr by-value struct pass/return
                std::string returnClassName;                             ///< (AR) اسم صنف العائد (لبنية @تمثيل_سي المُرجَعة بالقيمة) [RFC #53 F2-ج] / (EN) Return struct class name (C-repr by-value return)
                std::vector<SIRParameter> parameters;                    ///< (AR) المعاملات / (EN) Parameters
                std::vector<std::shared_ptr<SIRBasicBlock>> basicBlocks; ///< (AR) الكتل الأساسية / (EN) Basic blocks

                /// (AR) سمات الدالة [[سمة]] — تُترجم إلى LLVM function attributes
                /// (EN) Function attributes [[attr]] — lowered to LLVM function attributes
                std::vector<std::string> attributes;

                /// (AR) أسماءُ ما صُرِّح **محلّيًّا** في هذه الدالّة (معاملاتٍ ومتغيّراتٍ).
                ///      يسجّلها الوجهُ الأماميُّ في `addVariable` حين `isGlobal == false`،
                ///      لأنّه وحدَه يملكُ النطاقات؛ وSIR لا تعليمةَ تصريحٍ فيها البتّة —
                ///      الخاناتُ ضِمنيّةٌ من `STORE`، فلا يمكن للخلفيّةِ أن تميّز محلّيًّا
                ///      يُظلِّلُ عامًّا من إسنادٍ إلى العامِّ نفسِه. وبغيابِ هذا السجلّ كانت
                ///      `dynGlobalSlots_` — وهي **مجموعةٌ على مستوى الوحدةِ مفتاحُها الاسمُ
                ///      المجرَّد** — تُطبَّق على كلِّ دالّةٍ فيها الاسمُ نفسُه: عامٌّ اسمُه
                ///      `القيمة` نوعُه «أي» يجعلُ محلّيًّا صحيحًا اسمُه `القيمة` في دالّةٍ
                ///      أخرى ديناميًّا، فيُصدَر `sext %SadDyn` ويسقطُ `verifyModule`.
                ///      واسمُ المتغيّرِ وحدَه هو ما يقرّر — إعادةُ تسميتِه تُصلِح الترجمة.
                /// (EN) Names declared **locally** in this function (parameters and variables).
                ///      Recorded by the front end in `addVariable` when `isGlobal == false`,
                ///      since only it owns the scopes: SIR has no declaration instruction at
                ///      all — slots are implicit in `STORE` — so the backend cannot tell a
                ///      local shadowing a global from an assignment to that global. Without
                ///      this record, `dynGlobalSlots_` — a **module-wide, bare-name-keyed
                ///      set** — applied to every function using the same name: an Any-typed
                ///      global `القيمة` made an integer local `القيمة` in another function
                ///      dynamic, emitting `sext %SadDyn` and failing `verifyModule`. The
                ///      variable's name alone decided it; renaming it fixed the build.
                std::set<std::string> localSlotNames;

                /// (AR) SEM045 (أ٢): الخاناتُ المحلّيةُ **المصرَّحُ نوعُها صراحةً**
                ///      (اسم مجرَّد ⇒ النوع السطحيّ المصرَّح). يسجّلها الوجهُ الأماميُّ
                ///      في `addVariable` — كسجلِّ `localSlotNames` تمامًا: SIR لا
                ///      تعليمةَ تصريحٍ فيها، فالخلفيّةُ لا تملك تمييزَ خانةٍ **أعلنت
                ///      نوعَها** من خانةٍ اُستُنتجَ نوعُها من مُهيِّئها — والفرقُ هو
                ///      عقدُ SEM045 كلُّه: المستنتَجةُ تقبل «فراغ» بحرّيّة، والمعلنةُ
                ///      تُحرَس قبل STORE. (تصريحاتُ المستوى الأعلى تُسجَّل هنا أيضًا —
                ///      الوجهُ الأماميُّ يبنيها محلّيّاتٍ لـmain بالباني نفسِه.)
                /// (EN) SEM045 (stage أ٢): locally-declared slots with an EXPLICIT
                ///      surface type (bare name ⇒ declared kind). Recorded by the
                ///      front end in addVariable, mirroring localSlotNames: SIR has
                ///      no declaration instruction, and declared-vs-inferred is the
                ///      whole SEM045 contract. Top-level decls land here too (built
                ///      as main's locals by the same builder).
                std::map<std::string, SadTypeKind> declaredTypedSlots;

                /**
                 * @brief (AR) منشئ الدالة
                 * @brief (EN) Function constructor
                 *
                 * @param funcName اسم الدالة / Function name
                 * @param retType نوع الإرجاع / Return type
                 */
                SIRFunction(const std::string &funcName, SadTypeKind retType);

                /**
                 * @brief (AR) إضافة معامل
                 * @brief (EN) Add parameter
                 *
                 * @param param المعامل / Parameter
                 */
                void addParameter(const SIRParameter &param);

                /**
                 * @brief (AR) إضافة كتلة أساسية
                 * @brief (EN) Add basic block
                 *
                 * @param block الكتلة / Block
                 */
                void addBasicBlock(std::shared_ptr<SIRBasicBlock> block);

                /**
                 * @brief (AR) الحصول على كتلة بالاسم
                 * @brief (EN) Get block by name
                 *
                 * @param label اسم الكتلة / Block label
                 * @return مؤشر للكتلة أو nullptr / Pointer to block or nullptr
                 */
                std::shared_ptr<SIRBasicBlock> getBasicBlock(const std::string &label);

                /**
                 * @brief (AR) الحصول على المعاملات
                 * @brief (EN) Get parameters
                 */
                const std::vector<SIRParameter> &getParameters() const { return parameters; }

                /**
                 * @brief (AR) الحصول على المعاملات (قابل للتعديل)
                 * @brief (EN) Get parameters (mutable)
                 */
                std::vector<SIRParameter> &getMutableParameters() { return parameters; }

                /**
                 * @brief (AR) الحصول على الكتل الأساسية
                 * @brief (EN) Get basic blocks
                 */
                const std::vector<std::shared_ptr<SIRBasicBlock>> &getBasicBlocks() const { return basicBlocks; }

                /**
                 * @brief (AR) الحصول على اسم الدالة
                 * @brief (EN) Get function name
                 */
                const std::string &getName() const { return name; }

                /**
                 * @brief (AR) الحصول على اسم الربط الخارجي (أو اسم الدالة إذا لم يُحدد)
                 * @brief (EN) Get the link name (or function name if not specified)
                 */
                const std::string &getLinkName() const { return linkName.empty() ? name : linkName; }

                /**
                 * @brief (AR) الحصول على نوع الإرجاع
                 * @brief (EN) Get return type
                 */
                SadTypeKind getReturnType() const { return returnType; }

                /**
                 * @brief (AR) التحقق من صحة الدالة
                 * @brief (EN) Validate function
                 *
                 * @return true إذا كانت صحيحة / if valid
                 */
                bool validate() const;

                /**
                 * @brief (AR) طباعة الدالة بصيغة SIR
                 * @brief (EN) Print function in SIR format
                 *
                 * @return النص الممثل / String representation
                 */
                std::string toString() const;
            };

            // ======================================================================
            // فئة المتغير العام / Global Variable Class
            // ======================================================================

            /**
             * @brief (AR) متغير عام
             * @brief (EN) Global variable
             */
            class SIRGlobalVariable
            {
            public:
                std::string name;         ///< (AR) اسم المتغير / (EN) Variable name
                SadTypeKind type;         ///< (AR) نوع المتغير / (EN) Variable type
                std::string initialValue; ///< (AR) القيمة الأولية (اختياري) / (EN) Initial value (optional)
                // (AR) وجودُ القيمةِ الأوليّةِ عَلَمٌ مستقلٌّ عن نصِّها — ولا يُشتقّ منه بـ‏`.empty()`:
                //      النصُّ الفارغُ `""` قيمةٌ أوليّةٌ **موجودةٌ** تمامًا كغيرها، فاشتقاقُ الوجودِ
                //      من الفراغِ كان يجعل `ثابت فارغ = ""` غيرَ مُهيَّإٍ فيُصفَّر مؤشّرُه فيُطبع «void».
                // (EN) Presence of an initial value is a flag independent of its text — never derive
                //      it via `.empty()`: the empty string `""` is a present initial value like any
                //      other. Deriving presence from emptiness left `ثابت فارغ = ""` uninitialised,
                //      so its pointer was zeroed and printed as "void".
                bool hasInitialValue = false; ///< (AR) هل عُيِّنت قيمةٌ أوليّةٌ أصلًا / (EN) Was an initial value assigned at all
                // (AR) 🔑 نوعُ **القيمةِ الأوليّةِ نفسِها** — مستقلٌّ عن `type` أعلاه.
                //      حين تكون الخانةُ `أي` فـ`type` = `Any` ولا يُشتقّ منه صنفُ الحرفيّة،
                //      ونصُّ `initialValue` وحدَه **لا يكفي**: «1» تُكتَب للعددِ ١ وللقيمةِ
                //      «صحيح» سواءً بسواء، فلا يميّزهما مُصدِرٌ يشمُّ النصَّ. والمُصدِرُ كان
                //      يسقط إلى `default:` فيُصفِّر خانةَ %SadDyn — وصفرُ الوسمِ **هو وسمُ
                //      العدم** — فتُقرأ «لاشيء» ببناءٍ ناجحٍ ورمزِ خروجٍ صفر.
                //      المقيس: `ثابت أي س = 5` ⇒ المفسّرُ «5»/«رقم» والمُترجَمُ
                //      «لاشيء»/«عدم»؛ والضابطُ `ثابت رقم ص = 5` يعطي «5» في المحرّكَين.
                //      ⚠️ ويبقى `Unknown` لكلِّ خانةٍ لم تُملأ، فتظلّ `type` هي الحاكمةَ
                //      هناك ولا يتبدّل مسارٌ قائم.
                // (EN) The kind of the INITIAL VALUE itself, independent of `type` above.
                //      For an `أي` slot `type` is Any and carries no literal kind, and the
                //      text alone cannot substitute: "1" serialises both the integer 1 and
                //      the boolean true. The emitter fell to `default:` and left the
                //      %SadDyn slot zeroed — and a zero tag IS the null tag — so it read
                //      back as «لاشيء» with a successful build and exit code 0.
                //      Stays Unknown wherever unset, so `type` still governs there.
                SadTypeKind initialValueKind = SadTypeKind::Unknown;
                bool isConstant;          ///< (AR) ثابت / (EN) Constant
                // (AR) سمات تخزين ساكن (اللبنة 3.14)
                std::string linkName;     ///< (AR) رمز رابط مُصدَّر ثابت (@رمز) — فارغ = اسم داخليّ مُشوَّه
                bool isVolatile = false;  ///< (AR) متطاير (@متطاير) — يوسم قراءات/كتابات المخزن volatile
                bool isByteBlob = false;  ///< (AR) بيانات مضمَّنة بايتات(...) ⇒ ConstantDataArray في .rodata
                std::vector<uint8_t> byteData; ///< (AR) بايتات الكتلة حين isByteBlob
                bool isZeroArray = false; ///< (AR) اللبنة 3.16: مصفوفة .bss مصفَّرة (ساكن مصفوفة[N]) ⇒ [N x i8] zeroinitializer
                uint64_t zeroArrayCount = 0; ///< (AR) عدد بايتات المصفوفة الساكنة N حين isZeroArray

                /**
                 * @brief (AR) منشئ المتغير العام
                 * @brief (EN) Global variable constructor
                 *
                 * @param varName اسم المتغير / Variable name
                 * @param varType نوع المتغير / Variable type
                 * @param initValue القيمة الأولية / Initial value
                 * @param constant ثابت / Constant
                 */
                SIRGlobalVariable(
                    const std::string &varName,
                    SadTypeKind varType,
                    const std::string &initValue = "",
                    bool constant = false);

                /**
                 * @brief (AR) طباعة المتغير بصيغة SIR
                 * @brief (EN) Print variable in SIR format
                 *
                 * @return النص الممثل / String representation
                 */
                std::string toString() const;

                /**
                 * @brief (AR) الحصول على اسم المتغير
                 * @brief (EN) Get variable name
                 */
                const std::string &getName() const { return name; }

                /**
                 * @brief (AR) الحصول على نوع المتغير
                 * @brief (EN) Get variable type
                 */
                SadTypeKind getType() const { return type; }

                /**
                 * @brief (AR) التحقق من كون المتغير ثابت
                 * @brief (EN) Check if variable is constant
                 */
                bool getIsConstant() const { return isConstant; }

                /**
                 * @brief (AR) رمز الرابط المُصدَّر — يعيد linkName إن ضُبط، وإلّا الاسم
                 * @brief (EN) Exported linker symbol — linkName if set, else name
                 */
                const std::string &getLinkName() const { return linkName.empty() ? name : linkName; }
            };

            // ======================================================================
            // فئة الصنف / Class
            // ======================================================================

            /**
             * @brief (AR) صنف SIR
             * @brief (EN) SIR Class
             */
            class SIRClass
            {
            public:
                std::string name;                                                       ///< (AR) اسم الصنف / (EN) Class name
                std::string parentClass;                                                ///< (AR) الصنف الأب (للوراثة) / (EN) Parent class (for inheritance)
                bool isAbstract = false;                                                ///< (AR) هل الصنف مجرد / (EN) Is class abstract
                bool isSealed = false;                                                  ///< (AR) هل الصنف محكم / (EN) Is class sealed
                bool isCRepr = false;                                                   ///< (AR) تخطيط C-متوافق (@تمثيل_سي): بلا ترويسة vtable في الحقل 0 [RFC #53 F2-ب] / (EN) C-ABI layout: no vtable header at field 0
                std::string sourceFile;                                                 ///< (AR) مسار الملف المصدري (لفحص الوراثة المحكمة) / (EN) Source file path (sealed inheritance check)
                std::vector<std::string> implementedTraits;                             ///< (AR) السمات المُنفَّذة / (EN) Implemented traits
                std::unordered_map<std::string, SadTypeKind> fields_;                   ///< (AR) الحقول / (EN) Fields
                /// (AR) أسماء أصناف الحقول الكائنيّة (لتمكين الوصول المتسلسل: كائن.حقل_كائن.حقل)
                /// (EN) Class names of object-typed fields (enables chained access: obj.objField.field)
                std::unordered_map<std::string, std::string> fieldClassNames_;
                std::vector<std::string> fieldOrder_;                                   ///< (AR) ترتيب الحقول / (EN) Field insertion order
                std::unordered_map<std::string, std::shared_ptr<SIRFunction>> methods_; ///< (AR) الدوال / (EN) Methods

                /// (AR) خريطة ربط معاملات الباني بالحقول (لاستنتاج الأنواع)
                /// (EN) Constructor param → field mapping (for type inference)
                /// e.g., if constructor has هذا.الاسم = اسم, then: "اسم" → "الاسم"
                std::unordered_map<std::string, std::string> paramToFieldMap_;

                /// (AR) ربط الحقل بـ«معامِل.عضو» للاستنتاج المتعدّي [ISSUE-050b]
                ///      مثال: باني(ب) هذا.ق = ب.ق  ⇒  "ق" → {"ب", "ق"}
                ///      حين يكون المعامل بنية/صنفًا معروفًا من موقع النداء (Phase 2B) نشتقّ
                ///      نوع الحقل من نوع عضو ذلك الصنف (المعامل غير مُنوَّع وقت بناء الباني).
                /// (EN) field → (paramName, memberName) for transitive inference [ISSUE-050b]
                ///      e.g. ctor(p) this.f = p.m  ⇒  "f" → {"p","m"}. When the param is a
                ///      known struct/class at the call site (Phase 2B), derive the field type
                ///      from that class's member (the param is untyped at ctor-build time).
                std::unordered_map<std::string, std::pair<std::string, std::string>> fieldFromParamMember_;

                /// (AR) ربط وسائط الأساس بمعاملات الباني (لتتبع أنواع حقول الأب)
                /// (EN) Super arg → child param mapping (for parent field type propagation)
                /// Maps parent ctor param index (0-based, excl self) → child ctor param name
                /// e.g., الأساس(ضلع, ضلع) → {0: "ضلع", 1: "ضلع"}
                std::map<int, std::string> superParamMapping_;

                /// (AR) ثوابت وسائط الأساس (لتحديث أنواع حقول الأب مباشرةً)
                /// (EN) Super arg constants (to update parent field types directly)
                /// Maps parent ctor param index (0-based, excl self) → {type, value}
                /// e.g., الأساس("مستطيل") → {0: {STRING, "مستطيل"}}
                std::map<int, std::pair<SadTypeKind, std::string>> superConstantMapping_;

                /// (AR) حقول المصفوفات: الحقول المُهيَّأة بـ [] (مصفوفة فارغة)
                /// (EN) Array fields: fields initialized with [] (empty array)
                /// These need explicit SadArray allocation in the constructor, since memset(0)
                /// leaves them as null pointers which would crash on push (أضف).
                std::unordered_set<std::string> arrayFields_;

                /// (AR) القيم الابتدائية للحقول: حقول مُهيَّأة بقيم ثابتة في تعريف الصنف
                ///      مثال: `متغير عام س = 10` → fieldDefaultValues_["س"] = {"10", Integer}
                ///      تُستخدم في emitConstructorCall لتعيين القيم بعد memset(0)
                /// (EN) Field default values: fields initialized with constant values in class definition
                ///      Example: `var public x = 10` → fieldDefaultValues_["x"] = {"10", Integer}
                ///      Used in emitConstructorCall to set values after memset(0)
                std::unordered_map<std::string, std::pair<std::string, SadTypeKind>> fieldDefaultValues_;

                /// (AR) 🔑 حقولٌ نوعُها صنف: الاسمُ ⇒ اسمُ صنفِ الحقل.
                ///
                ///      قرارُ المالك: «يأخذ كلُّ متغيّرٍ القيمةَ الافتراضيّةَ لنوعِه»،
                ///      والقيمةُ الافتراضيّةُ لنوعٍ صنفيٍّ **كائنٌ مُنشَأ** لا مؤشّرٌ صفريّ.
                ///      والمحلّلُ يُحلّي ذلك سلفًا إلى `NewExpr` بلا وسائط، لكنّ
                ///      `fieldDefaultValues_` لا يحمل إلّا الحرفيّاتِ النصّيّة — فكان
                ///      المُهيّئُ الصنفيُّ **يُسقَط صامتًا** ويبقى الحقلُ صفرًا بعد
                ///      `memset(0)`، فينهار `ك.د.ق` بـ`rc=139` (مقيس 2026-08-16)
                ///      بينما يطبع المفسّرُ `1`.
                ///
                ///      وهو جدولٌ مستقلٌّ لا توسيعٌ للأوّل لأنّ قيمتَه ليست حرفيّةً
                ///      تُخزَّن بل **إنشاءٌ يُنفَّذ**، ولأنّ قارئَه (البانِي) يحتاجه
                ///      قبل تطبيقِ الحرفيّاتِ لا معها.
                /// (EN) Class-typed fields: name ⇒ its class name. The parser desugars
                ///      them to an argument-less NewExpr, but fieldDefaultValues_ only
                ///      carries literals, so the initializer was silently dropped and
                ///      the field stayed null after memset(0) ⇒ measured rc=139 where
                ///      the interpreter prints 1. A separate table because its value is
                ///      an executed construction, not a stored literal.
                std::unordered_map<std::string, std::string> classFieldTypes_;

                /// (AR) تسجيل حقل كمصفوفة
                /// (EN) Mark a field as an array field
                void markFieldAsArray(const std::string &fieldName)
                {
                    arrayFields_.insert(fieldName);
                }

                /// (AR) هل الحقل مصفوفة؟
                /// (EN) Is this field an array?
                bool isArrayField(const std::string &fieldName) const
                {
                    return arrayFields_.count(fieldName) > 0;
                }

                /**
                 * @brief (AR) منشئ الصنف
                 * @brief (EN) Class constructor
                 *
                 * @param className اسم الصنف / Class name
                 * @param parent الصنف الأب (اختياري) / Parent class (optional)
                 */
                SIRClass(const std::string &className, const std::string &parent = "");

                /**
                 * @brief (AR) إضافة حقل
                 * @brief (EN) Add field
                 *
                 * @param fieldName اسم الحقل / Field name
                 * @param fieldType نوع الحقل / Field type
                 */
                void addField(const std::string &fieldName, SadTypeKind fieldType);

                /**
                 * @brief (AR) إضافة دالة
                 * @brief (EN) Add method
                 *
                 * @param method الدالة / Method
                 */
                void addMethod(std::shared_ptr<SIRFunction> method);

                /**
                 * @brief (AR) الحصول على حقل
                 * @brief (EN) Get field
                 *
                 * @param name اسم الحقل / Field name
                 * @return نوع الحقل / Field type
                 */
                const SadTypeKind *getField(const std::string &name) const;

                /**
                 * @brief (AR) الحصول على دالة
                 * @brief (EN) Get method
                 *
                 * @param name اسم الدالة / Method name
                 * @return مؤشر للدالة / Pointer to method
                 */
                std::shared_ptr<SIRFunction> getMethod(const std::string &name) const;

                /**
                 * @brief (AR) الحصول على اسم الصنف
                 * @brief (EN) Get class name
                 */
                const std::string &getName() const { return name; }

                /**
                 * @brief (AR) الحصول على الصنف الأب
                 * @brief (EN) Get parent class
                 */
                const std::string &getParentClass() const { return parentClass; }

                /**
                 * @brief (AR) طباعة الصنف بصيغة SIR
                 * @brief (EN) Print class in SIR format
                 *
                 * @return النص الممثل / String representation
                 */
                std::string toString() const;
            };

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
