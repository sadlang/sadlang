// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file type_checker.cpp
 * @brief تنفيذ فاحص الأنواع / Type Checker Implementation
 * @phase Phase 5: US3 - نظام الأنواع المتقدم
 */

#include "semantic/type_checker.h"
#include "token.h"
#include "operator_domains_generated.h"
#include "lexer_keywords.h" // (AR) [RFC 0059] لفظُ «مقاطعة» من الجدولِ لا حرفيًّا
#include "class_nodes.h"
#include "pattern_nodes.h"    // (AR) [أ-م٢] ConstructorPattern / MatchStmt / CaseClause
#include "error_manager.h"    // (AR) [أ-م٢] بناء رسالة الكتالوج ثنائيّة اللغة

#include <iostream>
#include <algorithm>
#include <cassert>

namespace Sad
{
    namespace Semantic
    {

        using namespace Sad::Types;
        using TT = Lexer::TokenType;

        namespace
        {
            /**
             * @brief (AR) هل التعبيرُ حرفيّةٌ عدديّةٌ صحيحةٌ مجرَّدة (بإشارةٍ أحاديّةٍ أو بدونها)؟
             * @brief (EN) Is the expression a bare integer literal (with or without a unary sign)?
             *
             * (AR) 🔑 نوعُ الحرفيّةِ `1` الساكنُ هو «رقم» — لكنّه **افتراضٌ لا نيّة**.
             *      فمَن كتبَ `ط + 1` لم يُعلِنْ موقَّعيّةً، ومَن كتبَ `متغير رقم ر`
             *      أعلنَها. وهذا المُسنَدُ هو الحدُّ بين الاثنَين، ومنه يشتقُّ
             *      `SEM048` مَن يسألُه ومَن يُعفيه.
             * (EN) A literal's static type is «رقم», but that is a default rather
             *      than a declaration: `ط + 1` declares nothing while
             *      `متغير رقم ر` does. SEM048 draws its line here.
             *
             * (AR) والإشارةُ الأحاديّةُ تُقشَّرُ عمدًا: `-1` حرفيّةٌ في نيّةِ الكاتبِ
             *      وإن كانت `UnaryExpr` في الشجرة. **والمرساةُ النيّةُ لا الشكلُ
             *      النحويّ**، وإلّا لصارَ `ط + 1` يمرُّ و`ط + -1` يُرفَضُ لفرقٍ
             *      إملائيٍّ محض.
             */
            bool isBareIntegerLiteral(const AST::Expression *expression)
            {
                if (expression == nullptr)
                {
                    return false;
                }
                if (const auto *literal = dynamic_cast<const AST::LiteralExpr *>(expression))
                {
                    return literal->token.getType() == TT::NUMBER_INTEGER;
                }
                if (const auto *unary = dynamic_cast<const AST::UnaryExpr *>(expression))
                {
                    if (unary->op == TT::OP_MINUS || unary->op == TT::OP_PLUS)
                    {
                        return isBareIntegerLiteral(unary->operand.get());
                    }
                }
                return false;
            }
        } // namespace

        // ============================================================================
        // TypeCheckError
        // ============================================================================

        // ════════════════════════════════════════════════════════════════════
        // (AR) 🔑 اللافتةُ العامّةُ تُطبَعُ **حين لا هُويّةَ أخصَّ منها**.
        //      كانت `[خطأ نوع]` / `[Type Error]` تُصدَّرُ دائمًا، فلمّا صارت
        //      رسائلُ الكتالوجِ تحملُ رمزَها خرجَ السطرُ هكذا:
        //          [خطأ نوع] 'س': [SEM056] (AR) الخانة 'س' …
        //      لافتتانِ متتاليتان، والثانيةُ تُغني عن الأولى وتزيد: `SEM056`
        //      يُسمّي **هذه** العلّةَ بعينِها، و«خطأ نوع» يُسمّي صنفَها كلَّه.
        //      ⚠️ ولا تُحذَفُ اللافتةُ رأسًا: باعثونَ كثيرونَ ما زالوا يملؤون
        //      `message` نثرًا بلا رمزٍ (مقيسٌ: ثمانيةُ مواضعَ في هذه الطبقة)،
        //      فحذفُها يترُكُ تشخيصَهم **بلا أيِّ هُويّة**. فالشرطُ هو الجواب
        //      لا الحذف: تُطبَعُ ما لم تكن الرسالةُ تحملُ رمزًا سلفًا.
        // (EN) The generic label prints only when nothing more specific exists.
        //      It used to print unconditionally, so once catalog messages carried
        //      their code the line read «[خطأ نوع] 'x': [SEM056] (AR) …» — two
        //      labels where the second subsumes the first. Not deleted outright:
        //      many raisers still fill `message` with codeless prose, and deleting
        //      it would leave those diagnostics with no identity at all.
        // ════════════════════════════════════════════════════════════════════
        namespace
        {
            /// (AR) أتحملُ الرسالةُ رمزَ كتالوجٍ في مطلعِها؟ — «[ABC123] …»
            /// (EN) Does the message already open with a catalog code «[ABC123] …»?
            inline bool carriesCatalogCode(const std::string &m)
            {
                if (m.size() < 4 || m[0] != '[')
                    return false;
                const std::size_t close = m.find(']');
                if (close == std::string::npos || close < 2)
                    return false;
                bool sawAlpha = false;
                bool sawDigit = false;
                for (std::size_t i = 1; i < close; ++i)
                {
                    const unsigned char c = static_cast<unsigned char>(m[i]);
                    if (c >= 'A' && c <= 'Z')
                        sawAlpha = true;
                    else if (c >= '0' && c <= '9')
                        sawDigit = true;
                    else
                        return false;
                }
                return sawAlpha && sawDigit;
            }
        } // namespace

        std::string TypeCheckError::toEnglishString() const
        {
            std::ostringstream oss;
            // ════════════════════════════════════════════════════════════════
            // (AR) ⛔ **دعوى مراجعةٍ نُقِضَت بعقدٍ مكتوب.** قيلَ إنّ الرمزَ ينبغي
            //      أن يتصدّرَ السطرَ كمسارِ المحلّل، وإنّ لافتةَ الاسمِ قبلَه
            //      تكسرُ مراسي البذور. والعقدُ القائمُ يقولُ عكسَه **نصًّا**:
            //      `rules_matrix/…/203_diagnostic_carries_one_code.ص` بذرةُ P1
            //      تُعدِّدُ ثلاثةَ أشياءَ تُثبِّتُها، ثالثُها حرفيًّا «واسمُ الخانةِ
            //      `'س':` يسبقُ الرمزَ مباشرةً»، ومرساتُها `'س': [SEM056] (AR)`.
            //      فالترتيبُ **مقصودٌ ومحروس**، والرقعةُ أحمرت البذرةَ (مقيسٌ ٨
            //      أيلول) فنُقِضَت.
            //      🔑 والدرس: **ذوقُ الصياغةِ ليس عطبًا** — وتغييرُ شكلِ تشخيصٍ
            //      تحرسُه بذرةٌ قرارُ عقدٍ لا رقعةُ جودة.
            // (EN) A review finding DISPROVEN by a written contract. It argued the code
            //      should lead the line as on the parser path. The standing P1 seed
            //      203_diagnostic_carries_one_code.ص enumerates three pinned properties,
            //      the third being literally "the slot name precedes the code", anchored
            //      on `'س': [SEM056] (AR)`. The order is deliberate and guarded; the
            //      patch reddened the seed (measured) and was reverted. Wording taste is
            //      not a defect, and changing a guarded diagnostic's shape is a contract
            //      decision, not a quality patch.
            // ════════════════════════════════════════════════════════════════
            if (!carriesCatalogCode(message))
                oss << "[Type Error] ";
            if (!variableName.empty())
                oss << "'" << variableName << "': ";
            if (!message.empty())
                oss << message;
            else
                oss << "expected '" << expectedType << "' but got '" << actualType << "'";
            oss << " (line " << line << ", col " << column << ")";
            return oss.str();
        }

        std::string TypeCheckError::toArabicString() const
        {
            std::ostringstream oss;
            // (AR) ونظيرُه العربيُّ — انظر النقضَ المُدوَّنَ أعلاه.
            // (EN) The Arabic twin — see the recorded disproof above.
            if (!carriesCatalogCode(arabicMessage))
                oss << "[خطأ نوع] ";
            if (!variableName.empty())
                oss << "'" << variableName << "': ";
            if (!arabicMessage.empty())
                oss << arabicMessage;
            else
                oss << "متوقع '" << expectedType << "' لكن وُجد '" << actualType << "'";
            oss << " (سطر " << line << ", عمود " << column << ")";
            return oss.str();
        }

        // ============================================================================
        // TypeChecker - بناء / Construction
        // ============================================================================

        TypeChecker::TypeChecker()
            : useArabicMessages_(true), debugMode_(false), strictMode_(false), registry_(Sad::Types::SadTypeRegistry::instance()), lastInferredType_(nullptr), expectedReturnType_(nullptr)
        {
            // إنشاء بيئة الأنواع العامة / Create global type environment
            currentEnv_ = std::make_shared<TypeEnvironment>();
            scopeStack_.push_back(currentEnv_);
            // (AR) [Phase 5c] هيّئ النطاق العام لأسماء أصناف المتغيرات.
            // (EN) [Phase 5c] Initialize global scope for variable class names.
            variableClassNames_.emplace_back();
            // (AR) [SEM048] النطاقُ العامُّ لصيغةِ تصريحِ الأسماء.
            // (EN) [SEM048] Global scope for the declaration-form registry.
            variableTypeIsInferred_.emplace_back();

            // تسجيل الدوال المدمجة / Register built-in functions
            currentEnv_->bind("اطبع", registry_.getVoid());        // print
            currentEnv_->bind("اطبع_سطر", registry_.getVoid());    // println
            currentEnv_->bind("ادخل", registry_.getString());      // input
            currentEnv_->bind("ادخل_رقم", registry_.getInteger()); // input_number
            currentEnv_->bind("طول", registry_.getInteger());      // length
            currentEnv_->bind("نوع", registry_.getString());       // typeof

            // English aliases
            currentEnv_->bind("print", registry_.getVoid());
            currentEnv_->bind("println", registry_.getVoid());
            currentEnv_->bind("input", registry_.getString());
            currentEnv_->bind("len", registry_.getInteger());
            currentEnv_->bind("type", registry_.getString());
        }

        // ============================================================================
        // واجهة الفحص الرئيسية / Main Check Interface
        // ============================================================================

        TypeCheckResult TypeChecker::check(AST::ASTNode *ast)
        {
            currentResult_ = TypeCheckResult();
            // (AR) تصفير سجلّ التعدادات بين نداءات الفحص كي لا تتراكم المعاملات مضاعَفةً
            //      (REPL/وحدات متعدّدة) فتختلق التباسًا كاذبًا (🟠-٥ من مراجعة أميليا).
            // (EN) Reset the enum registry between checks so variants don't accumulate
            //      (REPL/multi-module) and fabricate false ambiguity (Amelia 🟠-5).
            enumVariants_.clear();
            variantOwners_.clear();

            if (!ast)
            {
                currentResult_.addError({"", "", "", 0, 0,
                                         "Null AST provided to type checker",
                                         "تم تمرير شجرة AST فارغة لفاحص الأنواع"});
                return currentResult_;
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) [RFC 0059] مرورٌ تمهيديٌّ يجمعُ أسماءَ معالِجاتِ المقاطعةِ قبلَ فحصِ
            //      الأجسام. بدونَه يعتمدُ حارسُ منعِ النداءِ **ترتيبَ التصريح**: نداءٌ
            //      يسبقُ تعريفَ المعالجِ يمرُّ صامتًا (قِيس: EXIT=0 ونداءُ ccc إلى دالّةٍ
            //      x86_intrcc بإطارٍ `ptr null` — الانهيارُ الثلاثيُّ الذي وُضع الحارسُ
            //      لمنعِه). حارسٌ يعتمدُ الترتيبَ حارسٌ نصفُ مشتعل.
            // (EN) [RFC 0059] Pre-pass collecting interrupt-handler names before bodies
            //      are checked: without it the call ban depends on declaration order and
            //      a call placed before the handler slips through silently (measured).
            // ════════════════════════════════════════════════════════════════
            interruptHandlerNames_.clear();
            interruptWordShadowedByClass_ = false;
            collectInterruptHandlers(ast);

            try
            {
                ast->accept(*this);
            }
            catch (const std::exception &e)
            {
                currentResult_.addError({"", "", "", 0, 0,
                                         std::string("Internal type checker error: ") + e.what(),
                                         std::string("خطأ داخلي في فاحص الأنواع: ") + e.what()});
            }

            if (debugMode_)
            {
                if (useArabicMessages_)
                    std::cerr << currentResult_.getArabicSummary();
                else
                    std::cerr << currentResult_.getSummary();
            }

            return currentResult_;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) [RFC 0059] يجمعُ أسماءَ معالِجاتِ المقاطعةِ المصرَّحةِ في الوحدة. المسحُ
        //      يشملُ الكتلَ المتداخلةَ لأنّ التصريحاتِ العليا تصلُ كتلةً واحدةً كبرى؛
        //      ومعالِجُ المقاطعةِ دالّةٌ حرّةٌ لا طريقةَ صنفٍ (الطرائقُ تُرفَض في المحلّل).
        // (EN) [RFC 0059] Collect declared interrupt-handler names, descending into
        //      nested blocks (top-level declarations arrive as one outer block).
        // ════════════════════════════════════════════════════════════════════
        void TypeChecker::collectInterruptHandlers(AST::ASTNode *node)
        {
            if (!node)
                return;
            if (auto *fn = dynamic_cast<AST::FunctionDecl *>(node))
            {
                if (fn->isInterruptHandler)
                    interruptHandlerNames_.insert(fn->name);
                return;
            }
            // (AR) [RFC 0059] التباسٌ بنيويّ: «دالة مقاطعة اسم(...)» تحتملُ قراءتين —
            //      مُعدِّلَ بوّابةٍ، أو **نوعَ عائدٍ** صنفًا اسمُه «مقاطعة» (وقاعدةُ
            //      «معرِّفٌ يتلوه معرِّف» في المحلّل واحدةٌ في الحالتين). فإن صُرِّح في
            //      الوحدةِ صنفٌ بهذا الاسمِ صارت القراءةُ الصامتةُ خطرًا: دالّةٌ قصد
            //      كاتبُها إرجاعَ كائنٍ تصيرُ بوّابةَ عتادٍ تعودُ بـiretq (قِيس: EXIT=0
            //      وصفرُ تشخيص). فيُرصَد التصادمُ هنا ويُشخَّص بدل أن يُحسَم صامتًا.
            // (EN) [RFC 0059] Structural ambiguity: «دالة مقاطعة name(...)» reads either
            //      as the gate modifier or as a user-class return type named «مقاطعة».
            //      If such a class is declared, diagnose instead of silently choosing.
            // (AR) للصنفِ عقدتان في الشجرة: `ClassDecl` (ما يبنيه المحلّلُ فعلًا)
            //      و`ClassDeclStmt`. رصدُ إحداهما وحدَها حارسٌ لا يشتعل — قِيس:
            //      الالتباسُ مرَّ صامتًا لأنّ المرورَ كان يسألُ العقدةَ الأخرى.
            // (EN) Classes have two node types; checking only one made the guard dead.
            {
                std::string className;
                bool isClass = false;
                if (auto *cls = dynamic_cast<AST::ClassDecl *>(node))
                {
                    className = cls->name;
                    isClass = true;
                }
                else if (auto *clsStmt = dynamic_cast<AST::ClassDeclStmt *>(node))
                {
                    className = clsStmt->name;
                    isClass = true;
                }
                if (isClass)
                {
                    const auto *intrEntry =
                        Lexer::KeywordTable::getEntry(Lexer::TokenType::KEYWORD_INTERRUPT);
                    if (intrEntry && className == intrEntry->primaryWord)
                        interruptWordShadowedByClass_ = true;
                    return;
                }
            }
            if (auto *block = dynamic_cast<AST::BlockStmt *>(node))
            {
                for (auto &stmt : block->statements)
                    collectInterruptHandlers(stmt.get());
            }
        }

        void TypeChecker::printSummary() const
        {
            if (useArabicMessages_)
            {
                std::cerr << currentResult_.getArabicSummary();
            }
            else
            {
                std::cerr << currentResult_.getSummary();
            }

            // طباعة الأخطاء / Print errors
            for (const auto &err : currentResult_.errors)
            {
                if (useArabicMessages_)
                {
                    std::cerr << "  ❌ خطأ نوع";
                    if (!err.variableName.empty())
                        std::cerr << " في '" << err.variableName << "'";
                    std::cerr << ": " << err.arabicMessage;
                    if (err.line > 0)
                        std::cerr << " (سطر " << err.line << ")";
                    std::cerr << "\n";
                }
                else
                {
                    std::cerr << "  ❌ Type error";
                    if (!err.variableName.empty())
                        std::cerr << " in '" << err.variableName << "'";
                    std::cerr << ": " << err.message;
                    if (err.line > 0)
                        std::cerr << " (line " << err.line << ")";
                    std::cerr << "\n";
                }
            }

            // طباعة التحذيرات / Print warnings
            for (const auto &warn : currentResult_.warnings)
            {
                std::cerr << "  ⚠️ " << warn << "\n";
            }
        }

        void TypeChecker::reset()
        {
            currentResult_ = TypeCheckResult();
            lastInferredType_ = nullptr;
            expectedReturnType_ = nullptr;
            currentFunction_.clear();
            scopeStack_.clear();
            currentEnv_ = std::make_shared<TypeEnvironment>();
            scopeStack_.push_back(currentEnv_);
            // (AR) [Phase 5c] إعادة تهيئة سجل أسماء أصناف المتغيرات.
            // (EN) [Phase 5c] Reset variable class names registry.
            variableClassNames_.clear();
            variableClassNames_.emplace_back();
            // (AR) [SEM048] يُعادُ تهيئتُه مع أخيه وإلّا تسرّبت أسماءُ تشغيلٍ سابق.
            // (EN) [SEM048] Reset alongside its sibling, else names leak across runs.
            variableTypeIsInferred_.clear();
            variableTypeIsInferred_.emplace_back();
        }

        // ============================================================================
        // (AR) [S-TS-P2] تحويل SadTypeKind إلى TypePtr — المحور SadTypeKind
        // (EN) [S-TS-P2] Convert SadTypeKind to TypePtr — SadTypeKind-centric
        //
        // (AR) قراءات الـAST (DataType) تُجسَّر عبر Types::fromDataType عند نقطة الاستدعاء.
        //      هذا الجسر يُحذف في S-TS-P2.5a عند ترحيل حقول الـAST إلى SadTypeKind.
        // ============================================================================

        Sad::Types::SadTypePtr TypeChecker::sadKindToTypePtr(Types::SadTypeKind kind) const
        {
            using K = Types::SadTypeKind;
            switch (kind)
            {
            case K::Integer:
                return registry_.getInteger();
            case K::Float:
                return registry_.getFloat();
            case K::String:
                return registry_.getString();
            case K::Boolean:
                return registry_.getBoolean();
            case K::Unit:
                return registry_.getVoid();
            // (AR) 🔑 و«عدم» له نوعُه — وكان يُردُّ نوعَ الوحدةِ بتعليقٍ يقول
            //      «لا تمييزَ دلاليًّا بعد». وكان ذلك بلا أثرٍ ما دامَ لا حارسَ
            //      يقرأُ الصنف؛ فلمّا صُنِّفَ «خالي» في `operand_classes`
            //      (2026-09-07) ورِثَ `عدم` حكمَه: `عدم س = لاشيء` ثمّ `س + 1`
            //      يُرَدُّ بـSEM054 ونصُّه يقولُ «خالي» — رفضٌ كاذبٌ وتشخيصٌ
            //      يُسمّي النوعَ باسمِ غيرِه.
            //      ⚠️ ودرسُه أنّ **تسويةً بين نوعَين تنامُ حتّى يُوقِظَها حارس**:
            //      لم يكن السطرُ خاطئًا يومَ كُتِب، بل صارَ خاطئًا يومَ صارَ
            //      للصنفِ أثر.
            // (EN) Null gets its own type. Sharing the unit type was inert while
            //      nothing read the kind; once unit became a classified operand
            //      class, null inherited its verdict — a false rejection whose
            //      text names the wrong type. A type conflation sleeps until a
            //      guard wakes it.
            case K::Null:
                return registry_.getNull();
            case K::Array:
                return registry_.makeArray(registry_.getAny()); // عنصر المصفوفة يُحدد لاحقاً
            case K::Map:
                return registry_.makeMap(registry_.getString(), registry_.getAny()); // مفتاح/قيمة يُحددان لاحقاً
            case K::Function:
                return registry_.makeFunction(std::vector<Sad::Types::SadTypePtr>{}, registry_.getVoid()); // توقيع الدالة يُحدد لاحقاً
            case K::Class:
                return registry_.getAny(); // يُحدد من StructRegistry عند الوصول
            case K::Enum:
                return registry_.getAny(); // يُحدد من EnumRegistry عند الوصول
            case K::Tuple:
                return registry_.makeTuple(std::vector<Sad::Types::SadTypePtr>{}); // عناصر الصف تُحدد لاحقاً
            // (AR) 🔑 أسرةُ الأعراضِ الثمانية. وكانت `طبيعي8` وحدَها مذكورةً ههنا
            //      تُردُّ «رقمًا»، **والسبعُ الباقياتُ تسقطُ في `default` فتُردُّ
            //      «مجهولًا»** — و«المجهول» تُجيزُه `areTypesCompatible` مع كلِّ
            //      شيء. فالمدقّقُ لم يكن يُسوّي الأعراضَ بـ«رقم» كما ظننتُ، بل
            //      كان **يُعميها**: `رقم32 س = "نصّ"` لا يجدُ من يردُّها ههنا.
            //      وحفظُ الصنفِ الآنَ هو ما يجعلُ حكمَ SEM048 ممكنًا في طبقةٍ
            //      واحدةٍ يشتركُ فيها المحرّكان بدلَ نسختَين تتباعدان.
            // (EN) The eight-width family. Only UInt8 was named here (mapped to
            //      Integer); the other seven fell into `default` → Unknown, which
            //      areTypesCompatible accepts against everything. The checker was
            //      not flattening widths — it was blind to them.
            case K::Int8:
            case K::Int16:
            case K::Int32:
            case K::UInt8:
            case K::UInt16:
            case K::UInt32:
            case K::UInt64:
                return registry_.getByKind(kind);
            case K::Error:
                return registry_.getUnknown();
            // (AR) [S-TS-P4] أنواع متقدّمة على مستوى الـkind (دون نوع داخلي هنا) → Any
            //      حتى لا يُطلِق المدقّق تحذيرات إسناد كاذبة لـ`رقم?` ونحوه. النوع الداخلي
            //      الغنيّ يُحمَل عبر sadType في العقد (تمثيل أغنى مخطّط لاحقًا).
            case K::Optional:
            case K::Result:
            case K::Future:
            case K::Generator:
                return registry_.getAny();
            // (AR) 🔑 `أي` نوعٌ صريحٌ لا غيابُ نوع. كان يسقط في `default` فيُرَدَّ
            //      `Unknown`، فيُهمِله `visitVarDeclStmt` («المُصرَّحُ مجهولٌ ⇒ خُذ
            //      المُستنتَج») ويُقيَّد المتغيّرُ بنوعِ مُهيِّئه. والمقيس: `أي س = 5`
            //      ثمّ `س = "نص"` تحت `--أنواع-صارمة` تُخفِق برسالة «متوقّع 'رقم'» —
            //      **حرفًا بحرفِ** رسالةِ `متغير س = 5` ورسالةِ `رقم س = 5`. أي أنّ
            //      الفاحصَ لم يكن يرى فرقًا بين الثلاثة: كانت `أي` عندَه **تعليقًا لا
            //      نوعًا**، في الوضعِ الوحيدِ الذي يُفترَض أن تحمل فيه وزنًا دلاليًّا.
            //      ⚠️ و`areTypesCompatible` تُجيز `Any` مع كلِّ شيءٍ منذ البداية —
            //      فالبابُ كان مفتوحًا والطريقُ إليه وحدَه مقطوعًا. ولذلك لا يُقاس
            //      هذا العطبُ بقراءةِ سلطةِ التوافق: هي سليمةٌ، والعلّةُ قبلَها بخطوة.
            // (EN) `أي` is an explicit type, not the absence of one. It fell into
            //      `default` and returned Unknown, so visitVarDeclStmt overrode it with
            //      the initializer's type. Measured: `أي س = 5` then `س = "نص"` under
            //      --strict-types failed with the SAME message as `متغير` and as `رقم` —
            //      the checker saw no difference between the three. areTypesCompatible
            //      has always accepted Any; only the road to it was cut, which is why
            //      reading the compatibility authority does not expose this defect.
            case K::Any:
                return registry_.getAny();
            case K::Unknown:
            default:
                return registry_.getUnknown();
            }
        }

        // ============================================================================
        // التحقق من توافق الأنواع / Check Type Compatibility
        // ============================================================================

        bool TypeChecker::areTypesCompatible(Sad::Types::SadTypePtr expected, Sad::Types::SadTypePtr actual) const
        {
            if (!expected || !actual)
                return true; // null types = no checking

            // أي نوع = Any يتوافق مع الكل / Any type is compatible with everything
            if (expected->isAny() || actual->isAny())
                return true;

            // Unknown = لم يُحدد بعد / Unknown = not yet determined
            if (expected->isUnknown() || actual->isUnknown())
                return true;

            // نفس النوع بالضبط / Exact same type
            if (expected->getKind() == actual->getKind())
                return true;

            // التحقق من التوافق الرقمي / Numeric compatibility
            if (expected->isNumeric() && actual->isNumeric())
                return true;

            // (AR) المصفوفات متوافقة مع الأعداد (التمثيل الداخلي i64 pointer)
            //      ومتوافقة مع أي نوع مصفوفة آخر (بغض النظر عن حجم العناصر)
            // (EN) Arrays are compatible with integers (internal i64 pointer representation)
            //      and with any other array type (regardless of element count)
            if (expected->isArray() || actual->isArray())
                return true;

            // ════════════════════════════════════════════════════════════
            // (AR) 📌 دَينٌ مُسمًّى: `خالي س = لاشيء` **يُحذَّرُ منه ولا يُرفَض**،
            //      والعدمُ فيه لا يُخزَّن.
            //
            //      وُضِعَت ههنا ذراعُ رفضٍ (`متوقَّعٌ خالي` × `مُعطًى عدم`) ثمّ
            //      **نُزِعَت لأنّ التنفيذَ لا يبلغُها**: `visitLiteralExpr`
            //      يُسنِدُ لـ`لاشيء` النوعَ `Unknown` عمدًا — بتعليقٍ يقولُ
            //      «متوافقٌ مع أيِّ نوعٍ لاحقٍ عند إعادةِ الإسناد» — فيُجيبُ
            //      شرطُ `isUnknown()` أعلاه بالقبولِ قبلَ أن يُسأَلَ أيُّ شرطٍ
            //      بعدَه. ⚠️ **وذراعٌ لا يبلغُها التنفيذُ أسوأُ من غيابِها**:
            //      تُقرَأُ حراسةً فيُطمأَنُّ إليها.
            //
            //      والمقيسُ (2026-09-07): خانةُ الوحدةِ حاملٌ `i8` **لا يسعُ
            //      حارسَ العدمِ** `kSadNullSentinel` (عرضُه ٦٤ بتًّا)، فيُبتَرُ
            //      إلى صفرٍ = قيمةِ الوحدةِ نفسِها. فسائرُ الأنواعِ تحفظُ العدمَ
            //      في خانتِها متمايزًا (‏`رقم س = لاشيء` يُجيبُ «عدم» ويعملُ
            //      `؟؟` بعدَه)، والوحدةُ **لا تستطيع**. فالفرقُ قدرةُ الخانةِ
            //      على حملِ الغياب، لا تشدّدٌ على نوع.
            //
            //      وما تغيّرَ فعلًا أنّ التشخيصَ صارَ يُبعَث: كان يمرُّ **بلا
            //      شيءٍ البتّة** بينما يُحذَّرُ نظيرُه العدديُّ، وقد رُفِعَ
            //      الاستثناءُ من `null_safety_analyzer.cpp` فصارَ يُحذَّرُ منه
            //      كسائرِ الأنواع. ورفعُه إلى **رفضٍ** يلزمُه أن يحملَ
            //      `visitLiteralExpr` نوعَ العدمِ حقًّا — وذلك يمسُّ كلَّ نوعٍ
            //      في اللغةِ، فمشروعٌ يُقاسُ وحدَه لا سطرٌ يُدَسُّ ههنا.
            // (EN) Named debt: `خالي س = لاشيء` is WARNED about, not rejected, and the
            //      null is not stored. A rejection arm (expected unit × actual null) was
            //      written here and then REMOVED because execution never reaches it:
            //      visitLiteralExpr deliberately types the null literal as Unknown — its
            //      comment says "compatible with any type on reassignment" — so the
            //      isUnknown() test above accepts before any later test is asked. An arm
            //      execution never reaches is worse than no arm: it reads as a guard and
            //      is trusted. Measured: a unit slot is an i8 carrier that cannot hold the
            //      64-bit null sentinel; it truncates to zero, which is the unit's own
            //      value. Every other type keeps null distinguishable in its slot, so
            //      `رقم س = لاشيء` answers «عدم» and `??` works after it; the unit cannot.
            //      The dividing line is whether the slot can carry absence, not extra
            //      severity for one type. What did change: the case now produces a
            //      diagnostic at all — it passed in complete silence while its numeric
            //      twin warned — since the exception was lifted in null_safety_analyzer.
            //      Raising it to a REJECTION requires visitLiteralExpr to carry a real
            //      null type, which touches every type in the language: a measured
            //      project of its own, not a line smuggled in here.
            // ════════════════════════════════════════════════════════════

            // Never يتوافق مع أي نوع / Never is subtype of everything
            if (actual->isNever())
                return true;

            // استخدم isAssignableTo / Use isAssignableTo
            return actual->isAssignableTo(expected.get());
        }

        // ============================================================================
        // استنتاج نوع تعبير / Infer Expression Type
        // ============================================================================

        Sad::Types::SadTypePtr TypeChecker::inferExprType(AST::Expression *expr)
        {
            if (!expr)
                return registry_.getUnknown();

            lastInferredType_ = nullptr;
            expr->accept(*this);

            Sad::Types::SadTypePtr result = lastInferredType_;
            if (!result)
            {
                // (AR) Fallback: نوع الـAST (DataType) مُجسَّرًا إلى SadTypeKind — S-TS-P2
                //      (الجسر يُحذف في S-TS-P2.5a عند ترحيل حقول الـAST).
                // (EN) Fallback: AST DataType bridged to SadTypeKind — S-TS-P2.
                result = sadKindToTypePtr(expr->getTypeKind());
            }

            currentResult_.totalInferred++;
            return result;
        }

        // ============================================================================
        // إدارة النطاقات / Scope Management
        // ============================================================================

        void TypeChecker::enterScope()
        {
            // إنشاء بيئة فرعية مع تمرير الأب / Create child env with parent
            // ملاحظة: لا نستخدم createChild() لأنها لا تمرر الأب بشكل صحيح
            auto childEnv = std::make_shared<TypeEnvironment>(currentEnv_);
            scopeStack_.push_back(childEnv);
            currentEnv_ = childEnv;
            // (AR) [Phase 5c] احفظ نطاق متوازٍ لأسماء أصناف المتغيرات.
            // (EN) [Phase 5c] Push parallel scope for variable class names.
            variableClassNames_.emplace_back();
            variableTypeIsInferred_.emplace_back();
        }

        void TypeChecker::exitScope()
        {
            if (scopeStack_.size() > 1)
            {
                scopeStack_.pop_back();
                currentEnv_ = scopeStack_.back();
            }
            // (AR) [Phase 5c] أخرج النطاق المتوازي.
            // (EN) [Phase 5c] Pop the parallel scope.
            if (!variableClassNames_.empty())
                variableClassNames_.pop_back();
            if (!variableTypeIsInferred_.empty())
                variableTypeIsInferred_.pop_back();
        }

        void TypeChecker::declareVariable(const std::string &name, Sad::Types::SadTypePtr type)
        {
            currentEnv_->bind(name, type);
            // (AR) [SEM048] كلُّ رباطٍ يمرُّ من هنا يُسجَّلُ **مُصرَّحًا** ابتداءً:
            //      المعامَلاتُ ومتغيّراتُ الحلقاتِ والمَصائدِ وأسماءُ الدوالِّ. ثمّ
            //      يُصحِّحُ [[visitVarDeclStmt]] وحدَه ما استُنتِجَ نوعُه من مُهيِّئِه.
            //      والقصدُ أن يترُكَ كلُّ رباطٍ أثرًا في نطاقِه يوقفُ البحثَ عندَه،
            //      فلا يعبرُ الظِّلُّ إلى المظلَّل.
            // (EN) [SEM048] Every binding is recorded as "declared" first —
            //      parameters, loop and catch variables, function names — and only
            //      visitVarDeclStmt corrects the entries whose type was inferred.
            //      Every binding must leave a trace in its own scope so the lookup
            //      stops at the shadow instead of reaching the shadowed name.
            if (!variableTypeIsInferred_.empty())
            {
                variableTypeIsInferred_.back()[name] = false;
            }
        }

        bool TypeChecker::writerDeclaredSignedness(const AST::Expression *expression) const
        {
            if (expression == nullptr)
            {
                return false;
            }
            // (AR) حرفيّةٌ عدديّةٌ مجرّدة: لم تُعلِنْ شيئًا.
            if (isBareIntegerLiteral(expression))
            {
                return false;
            }
            // (AR) متغيّرٌ استُنتِجَ نوعُه من مُهيِّئِه: كاتبُه لم يكتبِ اسمَ نوعٍ
            //      قطُّ، فـ«رقم» افتراضٌ لا إعلان. والبحثُ من أحدثِ نطاقٍ إلى
            //      أقدمِه، **ويقفُ عند أوّلِ نطاقٍ يعرفُ الاسمَ** مُصرَّحًا كانَ أم
            //      مُستنتَجًا — وهذا ما يجعلُ التظليلَ صحيحًا: لو لم يُسجَّلِ
            //      المُصرَّحُ لَعبرَ البحثُ ظِلَّه إلى مستنتَجٍ أقدمَ يحملُ اسمَه.
            // (EN) Walk innermost-out and stop at the first scope that knows the
            //      name at all — declared or inferred. That is what makes shadowing
            //      correct: were declared names unrecorded, the walk would step
            //      past the shadow onto an older inferred binding of the same name.
            if (const auto *variable = dynamic_cast<const AST::VariableExpr *>(expression))
            {
                for (auto scope = variableTypeIsInferred_.rbegin();
                     scope != variableTypeIsInferred_.rend(); ++scope)
                {
                    const auto entry = scope->find(variable->name);
                    if (entry != scope->end())
                    {
                        return !entry->second;
                    }
                }
            }
            // (AR) وما عدا ذلكَ — `رقم`، أو «رقم» مُصرَّحٌ بالاسم، أو نداءُ
            //      دالّةٍ أُعلِنَ عائدُها — إعلانٌ يُسأَلُ عليه.
            return true;
        }

        Sad::Types::SadTypePtr TypeChecker::lookupVariable(const std::string &name) const
        {
            auto type = currentEnv_->lookup(name);
            return type ? type : nullptr;
        }

        // ============================================================================
        // مساعدات / Helpers
        // ============================================================================

        std::pair<size_t, size_t> TypeChecker::getLocation(AST::ASTNode *node) const
        {
            if (!node)
                return {0, 0};
            auto &pos = node->position;
            return {pos.line, pos.column};
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) 🔑 **والحكمُ على اليقينِ لا على النوع.** كان الشرطُ
        //      `getKind() == Unit` وحدَه، ومعاملُ قالبٍ أو معاملُ «عامل» بلا
        //      تنميطٍ كان يُصنَّفُ `Unit` في المحلّل — فيُرَدُّ **برنامجٌ صحيحٌ**
        //      بتشخيصٍ يُقرَأُ قاعدةَ لغة. وقد سُدَّ الجذرُ في المحلّلِ (الافتراضُ
        //      `Unknown`)، ويُحرَسُ ههنا ثانيًا: مَن لا يُعرَفُ نوعُه لا يُحاكَم.
        //      ⚠️ **والرفضُ الكاذبُ أخطرُ من الانهيار**: الانهيارُ يُقرَأُ عطبًا،
        //      والرفضُ يُقرَأُ قاعدةً.
        //      🔑 وكلمةُ العمليّةِ تصلُ من **معجمِ اللغة** (`KeywordTable`) لا
        //      مؤلَّفةً في هذا الملفّ: كانت ثلاثَ سلاسلَ عربيّةٍ خامٍّ في `.cpp`
        //      إحداها («شرطي») ليست كلمةَ لغةٍ أصلًا، وكانت تُحشى في المخرَجِ
        //      الإنجليزيِّ فيحملُ **لغةَ الآخَر** في خانةٍ واحدة.
        // (EN) Judge CERTAINTY, not the kind. The test was `getKind() == Unit`
        //      alone while an untyped template/operator parameter was classified
        //      Unit by the parser — rejecting CORRECT programs with a diagnosis that
        //      reads as a language rule. The root is fixed in the parser; this is the
        //      second guard: what is not known is not judged. The operation word now
        //      comes from the language's keyword table, not from three raw Arabic
        //      literals in this file (one of which was not a keyword at all) that
        //      leaked Arabic into the English rendering.
        // ════════════════════════════════════════════════════════════════════
        void TypeChecker::rejectUnitCondition(const Sad::Types::SadTypePtr &condType,
                                              AST::ASTNode *node,
                                              Lexer::TokenType operationKeyword)
        {
            if (!condType || condType->getKind() != Types::SadTypeKind::Unit)
                return;
            const auto *entry = Lexer::KeywordTable::getEntry(operationKeyword);
            const std::string operation = entry ? entry->primaryWord : std::string();
            Errors::RenderContext ctx;
            auto [line, col] = getLocation(node);
            ctx.location = Errors::SourceLocation("<input>", line, col);
            ctx.placeholders = {
                {"operation", operation},
                {"type", Types::sadTypeKindArabicName(Types::SadTypeKind::Unit)}};
            recordTypeError("", "", "", node,
                            Errors::ErrorManager::getInstance().buildBilingualMessage(
                                Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID, ctx));
        }

        void TypeChecker::recordTypeError(const std::string &varName,
                                          const std::string &expected,
                                          const std::string &actual,
                                          AST::ASTNode *node,
                                          const std::string &msg)
        {
            auto [line, col] = getLocation(node);

            TypeCheckError err;
            err.variableName = varName;
            err.expectedType = expected;
            err.actualType = actual;
            err.line = line;
            err.column = col;

            if (msg.empty())
            {
                // ════════════════════════════════════════════════════════
                // (AR) 🔑 من الكتالوجِ لا من نصٍّ مؤلَّفٍ ههنا. وهذه الذراعُ
                //      **المصبُّ الافتراضيُّ لثمانيةِ باعثين**، فكان أشهرُ
                //      خرقِ عقدٍ في اللغةِ يصلُ الكاتبَ بلا رمزٍ ولا علاج،
                //      ولا تجدُ بذرةٌ سالبةٌ ما تُرسى عليه غيرَ **نصِّ
                //      الرسالة** — مرسًى ينكسرُ بأوّلِ تحريرٍ ويَخضَرُّ على
                //      رسالةٍ أخرى تحملُ العبارةَ نفسَها.
                //      🔑 وكشفَه الحارسُ لا المراجعة: ثلاثُ بذورٍ جديدةٍ
                //      رفعت عدّادَ «سالبٌ بلا رمز» فوقَ سقفِه النازل.
                // (EN) From the catalog, not composed here. This arm is the
                //      DEFAULT SINK for eight raisers, so the language's most
                //      common contract breach reached the writer with no code
                //      and no fix hint, and a negative seed had nothing but the
                //      message TEXT to anchor on. Found by a guard, not a review.
                // ════════════════════════════════════════════════════════
                Errors::RenderContext ctx;
                ctx.location = Errors::SourceLocation("<input>", line, col);
                ctx.placeholders = {{"name", varName},
                                    {"expected", expected},
                                    {"found", actual}};
                // (AR) ولا بادئةَ رمزٍ ههنا: `buildBilingualMessage` تُصدِّرُها
                //      بنفسِها منذ 2026-09-03 — وإضافتُها ثانيةً تُخرِجُ
                //      «[SEM056] [SEM056] …».
                // (EN) No code prefix here: buildBilingualMessage emits it itself.
                const std::string tagged =
                    Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Errors::ErrorCode::SEM_SLOT_TYPE_MISMATCH, ctx);
                err.message = tagged;
                err.arabicMessage = tagged;
            }
            else
            {
                err.message = msg;
                err.arabicMessage = msg;
            }

            currentResult_.addError(err);
        }

        // ============================================================================
        // (AR) [أ-م٢] مساعدات التعداد بحمولة (ADT) / (EN) [A-M2] Tagged-enum helpers
        // ============================================================================

        const TypeChecker::EnumVariantInfo *
        TypeChecker::lookupVariant(const std::string &variantName,
                                   std::string &owningEnumOut,
                                   bool &ambiguousOut) const
        {
            ambiguousOut = false;
            owningEnumOut.clear();

            auto ownersIt = variantOwners_.find(variantName);
            if (ownersIt == variantOwners_.end() || ownersIt->second.empty())
            {
                return nullptr;
            }
            // (AR) معامل مشترك بين أكثر من تعداد ⇒ غامض (نأخذ الأوّل لكن نُعلِم المُستدعي).
            // (EN) Variant shared by more than one enum ⇒ ambiguous (take first, flag caller).
            ambiguousOut = ownersIt->second.size() > 1;
            const std::string &enumName = ownersIt->second.front();
            owningEnumOut = enumName;

            auto enumIt = enumVariants_.find(enumName);
            if (enumIt == enumVariants_.end())
            {
                return nullptr;
            }
            for (const auto &v : enumIt->second)
            {
                if (v.variantName == variantName)
                {
                    return &v;
                }
            }
            return nullptr;
        }

        void TypeChecker::reportCatalogError(
            Errors::ErrorCode code,
            const std::map<std::string, std::string> &placeholders,
            AST::ASTNode *node)
        {
            auto [line, col] = getLocation(node);

            // (AR) رمز الخطأ من كتالوج مصدر الحقيقة حصرًا؛ الرسالة تُبنى من القالب.
            // (EN) Error code from the SoT catalog exclusively; message built from template.
            Errors::RenderContext ctx;
            ctx.location = Errors::SourceLocation("<input>", line, col);
            ctx.placeholders = placeholders;

            const std::string rendered =
                Errors::ErrorManager::getInstance().buildBilingualMessage(code, ctx);

            // ────────────────────────────────────────────────────────────────
            // (AR) ⚠️ **تصويبٌ مُدوَّنٌ (2026-09-07)، ودرسُه أهمُّ من سطرِه.**
            //      كان ههنا بادئةُ رمزٍ ثانيةٌ — `"[" + code + "] " + rendered` —
            //      ونصُّها يقول: «`buildBilingualMessage` تُصيغُ بلا رمز، والرقعةُ
            //      تُوضَعُ في أضيقِ طبقةٍ تسعُ العلّة». وكان ذلك **صادقًا يومَ
            //      كُتِب**، ثمّ سُدَّت العلّةُ في المصبِّ نفسِه (2026-09-03) فصارت
            //      الرسالةُ تحملُ رمزَها هناك — ولم يُنزَعِ الوصلُ ههنا.
            //      فالنتيجةُ رقعتانِ لعلّةٍ واحدةٍ لا تعرفُ إحداهما بالأخرى،
            //      وكلتاهما تحملُ حُجّةً مُقنِعة، والمخرَجُ **«[SEM054] [SEM054] …»**.
            //
            //      🔑 والقاعدةُ المستفادة: **رقعةٌ في طبقةٍ ضيّقةٍ دَينٌ حتّى
            //      تُنزَع.** فحين تُسَدُّ العلّةُ في مصبِّها لا يبطلُ الالتفافُ
            //      من تلقائِه، بل يبقى يعملُ **مرّتَين**. وتعليقُه هو ما يحميه:
            //      يقرؤه المراجعُ فيراهُ مُبرَّرًا، ولا يسألُ أَبقيَ مُبرَّرًا.
            //      وهي علّةُ «شفرةٌ صحيحةٌ تحتَ فرضيّةٍ انقلبت» نفسُها التي حكمت
            //      هذا العملَ كلَّه — ههنا في تعليقٍ لا في `SadTypeKind`.
            // (EN) Documented correction (2026-09-07). A SECOND code prefix lived
            //      here, its comment arguing that buildBilingualMessage renders
            //      without a code so the patch belongs in the narrowest layer. That
            //      was TRUE when written; the sink itself was then fixed
            //      (2026-09-03) and this workaround was never removed — two patches
            //      for one defect, each persuasive, output «[SEM054] [SEM054] …».
            //      The rule: a narrow-layer workaround is DEBT until removed. When
            //      the root is fixed the workaround does not lapse — it runs twice.
            //      Its own comment is what shields it: a reviewer reads the
            //      justification and never asks whether it still holds.
            // ────────────────────────────────────────────────────────────────
            const std::string tagged = rendered;

            TypeCheckError err;
            err.line = line;
            err.column = col;
            err.message = tagged;
            err.arabicMessage = tagged;
            currentResult_.addError(err);
        }

        Sad::Types::SadTypePtr TypeChecker::checkEnumConstruction(
            const std::string &variantName,
            const EnumVariantInfo &info,
            const std::vector<AST::ExprPtr> &args,
            AST::ASTNode *node)
        {
            // (AR) فحص عدد الحمولة — نعيد استعمال رمز الكتالوج القائم SEM_WRONG_ARG_COUNT.
            // (EN) Payload arity check — reuse the existing SEM_WRONG_ARG_COUNT catalog code.
            const size_t expected = info.fieldTypes.size();
            const size_t found = args.size();
            if (found != expected)
            {
                reportCatalogError(
                    Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                    {{"name", variantName},
                     {"expected", std::to_string(expected)},
                     {"found", std::to_string(found)}},
                    node);
            }
            else
            {
                // (AR) فحص أنواع الحمولة عبر fieldTypes — للوسائط المصرَّح نوعها بنوعٍ
                //      مدمجٍ معروف فقط (رقم/عشري/نص/منطقي). الحقول غير المُصنَّفة ("")
                //      أو أنواع الأصناف تُتجاوز في أ-م٢ (استدلالها غير موثوق بعد).
                // (EN) Payload type check via fieldTypes — only for args whose declared
                //      field type is a known built-in (رقم/عشري/نص/منطقي). Untyped ("")
                //      or class-typed fields are skipped in A-M2 (inference not reliable yet).
                for (size_t i = 0; i < expected; ++i)
                {
                    const std::string &ftName = info.fieldTypes[i];
                    if (ftName.empty() || !args[i])
                        continue;

                    // (AR) خرائط اسم النوع المدمج → SadTypeKind (تجاوز غير المعروف).
                    // (EN) Map built-in type name → SadTypeKind (skip if unknown).
                    using K = Types::SadTypeKind;
                    K expKind;
                    if (ftName == "رقم")
                        expKind = K::Integer;
                    else if (ftName == "عشري")
                        expKind = K::Float;
                    else if (ftName == "نص")
                        expKind = K::String;
                    else if (ftName == "منطقي")
                        expKind = K::Boolean;
                    else
                        continue; // (AR) نوع صنف/غير مدمج / (EN) class/non-built-in type

                    Sad::Types::SadTypePtr argT = inferExprType(args[i].get());
                    if (!argT || argT->isUnknown() || argT->getKind() == SadTypeKind::Any)
                        continue; // (AR) نوع الوسيط غير معروف / (EN) unknown arg type

                    Sad::Types::SadTypePtr expT = sadKindToTypePtr(expKind);
                    if (expT && !areTypesCompatible(expT, argT))
                    {
                        reportCatalogError(
                            Errors::ErrorCode::SEM_TYPE_MISMATCH,
                            {{"expected", expT->toString()},
                             {"found", argT->toString()}},
                            node);
                    }
                }
            }

            // (AR) نوع القيمة المُنشأة: نستعمل Class مبدئيًّا (لا EnumKind بعد) — نظير
            //      visitEnumDecl. codegen الاتّحاد الموسوم مؤجَّل لـأ-م٤.
            // (EN) Constructed value's type: Class placeholder (no EnumKind yet) — mirrors
            //      visitEnumDecl. Tagged-union codegen deferred to A-M4.
            // (AR) 🔑 **فخٌّ مقيسٌ في الدمج، وهو أخطرُ ما كشفَته الموجة.**
            //      كان النداءُ `internPrimitiveType(Class)` في سجلِّ الهرمِ
            //      الثاني، وهو يردُّ `Any` صراحةً لـ`Class` و`Trait` («ليسا
            //      بدائيَّين فلا PrimitiveType لهما»). ونظيرُه في سجلِّ الهرمِ
            //      الواحدِ `getByKind` يردُّ لهما **`nullptr`**. فبدلٌ اسمِيٌّ
            //      بين دالّتَين «متكافئتَين» كان يُسرِّبُ نوعًا عدميًّا إلى
            //      `declareVariable` — **والشوطُ الكاملُ بقيَ أخضرَ**، لأنّ
            //      النوعَ العدميَّ يُقرَأُ «لا نوعَ» فيُسكِتُ الفحصَ ولا يكسرُه.
            //      ⚠️ ودرسُه أنّ **الأخضرَ لا يُثبِتُ التكافؤَ في طبقةٍ يُقرَأُ
            //      غيابُها سكوتًا**؛ فالتكافؤُ يُقاسُ على الجدولَين لا على
            //      المخرَج. فأُرسِيَ الموضعانِ على `sadKindToTypePtr` — وهي
            //      خريطةُ الأصنافِ الواحدةُ في هذه الطبقة، وفيها `Class` ⇒ `Any`
            //      حرفًا بحرفٍ كما كان.
            // (EN) The merge's sharpest trap: internPrimitiveType(Class) returned
            //      Any, while its "equivalent" getByKind(Class) returns nullptr —
            //      and the full run stayed GREEN, because a null type reads as
            //      "no type" and silences the check instead of breaking it.
            return sadKindToTypePtr(SadTypeKind::Class);
        }

        // ============================================================================
        // زيارة التعابير / Visit Expressions
        // ============================================================================

        void TypeChecker::visitLiteralExpr(AST::LiteralExpr &expr)
        {
            currentResult_.totalExpressions++;

            switch (expr.token.getType())
            {
            case TT::NUMBER_INTEGER:
                lastInferredType_ = registry_.getInteger();
                break;
            case TT::NUMBER_DOUBLE:
                lastInferredType_ = registry_.getFloat();
                break;
            case TT::STRING_LITERAL:
                lastInferredType_ = registry_.getString();
                break;
            case TT::LITERAL_TRUE:
            case TT::LITERAL_FALSE:
                lastInferredType_ = registry_.getBoolean();
                break;
            case TT::LITERAL_NULL:
                // (AR) لاشيء = nullable — متوافق مع أي نوع لاحق عند إعادة الإسناد
                // (EN) null literal = nullable — compatible with any type on reassignment
                lastInferredType_ = registry_.getUnknown();
                break;
            default:
                lastInferredType_ = registry_.getUnknown();
                break;
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] Literal: " << (lastInferredType_ ? lastInferredType_->toString() : "null") << "\n";
            }
        }

        void TypeChecker::visitVariableExpr(AST::VariableExpr &expr)
        {
            currentResult_.totalExpressions++;

            // (AR) [RFC 0059] المعالجُ لا يُؤخَذ **قيمةً** أيضًا، لا نداءً فحسب: قِيس أنّ
            //      `متغير م = عالج` ثمّ `م(0)` يُنزَّل نداءً مباشرًا للبوّابةِ بإطارٍ
            //      `ptr null` (إزالةُ الافتراضيّةِ تعيدُ النداءَ اسميًّا) — فحارسُ موضعِ
            //      النداءِ وحدَه بابٌ نصفُ مغلق. العنوانُ يُؤخَذ بـ«عنوان_رمز» حصرًا
            //      (وهو نصٌّ حرفيٌّ لا مرجعُ دالّةٍ فلا يمرُّ من هنا).
            // (EN) [RFC 0059] A handler may not be taken as a value either: measured
            //      that binding it then calling devirtualises back into a direct call.
            if (interruptHandlerNames_.count(expr.name) > 0)
            {
                reportCatalogError(
                    Errors::ErrorCode::SEM_INTERRUPT_HANDLER_CONTRACT,
                    {{"detail",
                      "«" + expr.name + "»: معالجُ المقاطعةِ لا يُؤخَذ قيمةً ولا يُنادى — "
                      "خذ عنوانَه بـ«عنوان_رمز» وسجّله في جدولِ المقاطعات"}},
                    &expr);
            }

            auto type = lookupVariable(expr.name);
            if (type)
            {
                lastInferredType_ = type;
            }
            else
            {
                // متغير غير معرّف — ليس خطأ أنواع بل خطأ دلالي
                // Undeclared variable — not a type error, semantic error
                lastInferredType_ = registry_.getUnknown();

                if (strictMode_)
                {
                    recordTypeError(expr.name, "", "", &expr,
                                    "Undeclared variable '" + expr.name + "'");
                }
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] Variable '" << expr.name << "': "
                          << (lastInferredType_ ? lastInferredType_->toString() : "unknown") << "\n";
            }
        }

        void TypeChecker::visitBinaryExpr(AST::BinaryExpr &expr)
        {
            currentResult_.totalExpressions++;

            Sad::Types::SadTypePtr leftType = inferExprType(expr.left.get());
            Sad::Types::SadTypePtr rightType = inferExprType(expr.right.get());

            // ════════════════════════════════════════════════════════════════
            // (AR) ⚠️ **ذراعٌ حُذفت (2026-09-07) — ونصُّها يبقى لأنّ الدرسَ أبقى.**
            //      كانت ههنا ذراعٌ مكتوبةٌ باليدِ ترفضُ «فراغًا ناتجَ نداءٍ طرفًا
            //      في عمليّةٍ حسابيّة»: جدولُ عواملَ سِتٌّ مهجّاةٌ في الشفرة،
            //      وقيدُ `CallExpr` أُضيفَ ترقيعًا لإيجابيّةٍ كاذبةٍ على وصولِ
            //      العضو، ورسالةٌ بلا اسمِ خانةٍ ونوعُها المُعلَنُ **«قيمة»** —
            //      وهي ليست نوعًا في اللغةِ أصلًا.
            //
            //      ومقيسٌ أنّ الذراعَ بعدَ رقعةِ `SEM056` صارت تُخرِج:
            //          [SEM056] الخانة '' من نوع 'قيمة' وأُسند إليها 'خالي'
            //          → … أو اجعلْها «متغير» فيُستنتَجَ نوعُها …
            //      أي **رمزَ إسنادِ خانةٍ على خطأِ عامل**، بخانةٍ فارغةِ الاسم،
            //      وعلاجٍ يوجّهُ إلى خانةٍ لا وجودَ لها.
            //
            //      🔑 وحُذفت لأنّ حكمَها صارَ **مُشتقًّا**: «خالي» صُنِّفَ في
            //      `operand_classes` بمصدرِ الحقيقة، فيرفضُه كلُّ عاملٍ محروسٍ
            //      بـ`SEM054` — بجدولٍ مولَّدٍ لا مهجًّى، وبنوّابٍ صادقةٍ
            //      (`{op}` و`{left}` و`{right}`)، وبلا قيدِ `CallExpr` لأنّ
            //      الحكمَ يقعُ على **النوعِ لا على شكلِ التعبير**.
            //      ودرسُه: **ذراعٌ يدويّةٌ تبقى بعدَ أن يُشتَقَّ حكمُها لا تصيرُ
            //      زائدةً بل مُنافِسةً** — تسبقُ المُشتقَّ فتُصدِرُ حكمَه برمزٍ
            //      أدنى دقّةً، وهي نفسُها قاعدةُ «الرقعةُ الضيّقةُ دَينٌ حتّى
            //      تُنزَع» التي أخرجت البادئةَ المكرَّرةَ قبلَ ساعات.
            // (EN) A hand-written arm was DELETED here; the note stays because the
            //      lesson does. It rejected "a call-typed Void as an arithmetic
            //      operand" with a six-operator table spelled out in source, a
            //      CallExpr restriction patched in against a false positive, and a
            //      message with no slot name whose declared type was «قيمة» — not a
            //      type in the language. After the SEM056 patch it emitted a
            //      SLOT-assignment code for an OPERATOR error. Deleted because the
            //      verdict is now DERIVED: unit is a classified operand class in the
            //      SoT, so every guarded operator rejects it via SEM054 from a
            //      generated table with truthful placeholders and no CallExpr
            //      restriction — the judgement is on the TYPE, not the syntax.
            //      A hand-written arm outliving its derivation is not redundant but
            //      COMPETING: it runs first and answers with a less precise code.
            // ════════════════════════════════════════════════════════════════

            // ════════════════════════════════════════════════════════════════
            // (AR) SEM048 — الخانةُ الثامنةُ وحدَها: «طبيعي» مع موقَّعٍ بعرضِه.
            //      قاعدةُ الهيمنةِ («الأعرضُ يغلب، وعندَ التساوي اللا-موقَّع»)
            //      حاسمةٌ في سبعِ خاناتٍ من ثمانٍ لأنّ ما دونَ ٦٤ يُرقَّى إلى
            //      «رقم» فيسعُ المدَيَين معًا. والثامنةُ لا نوعَ فوقَها: تغليبُ
            //      اللا-موقَّعِ يجعلُ `طبيعي(١٠) + رقم(−٥٠)` قريبًا من ١٨
            //      كوينتليون بدلَ «−٤٠»، وتغليبُ الموقَّعِ يجعلُ أكبرَ طبيعيٍّ
            //      ناقصَ واحدٍ يساوي «−٢». وكلاهما كذبٌ صريح — فالوقوفُ ههنا
            //      وطلبُ متغيّرٍ وسيطٍ مُعلَنِ النوعِ أصدقُ من اختيارٍ صامتٍ يكذبُ في
            //      طرف — ولا تعبيرَ تحويلٍ في «ص» يُطلَبُ بدلًا منه.
            //      ⚠️ والمقارناتُ خارجَ المرحلةِ الأولى قصدًا.
            // (EN) SEM048 — only the eighth cell: طبيعي mixed with a same-width
            //      signed operand. Nothing spans both ranges, so either silent
            //      choice lies; stop and require an intermediate variable of a declared type
            //      (Sad has no cast expression).
            //      Comparisons deliberately stay outside stage one.
            // ════════════════════════════════════════════════════════════════
            {
                const bool arithmeticOrBitwise =
                    expr.op == TT::OP_PLUS || expr.op == TT::OP_MINUS ||
                    expr.op == TT::OP_MULTIPLY || expr.op == TT::OP_DIVIDE ||
                    expr.op == TT::OP_FLOOR_DIVIDE || expr.op == TT::OP_MODULO ||
                    expr.op == TT::OP_POWER;
                if (arithmeticOrBitwise && leftType && rightType &&
                    leftType->isIntegerNumeric() && rightType->isIntegerNumeric())
                {
                    const Types::SadTypeKind leftKind = leftType->getKind();
                    const Types::SadTypeKind rightKind = rightType->getKind();
                    const bool leftUnsigned = Types::sadTypeKindIsUnsignedInteger(leftKind);
                    const bool rightUnsigned = Types::sadTypeKindIsUnsignedInteger(rightKind);
                    // (AR) 🔑 المرساةُ **إعلانُ الكاتبِ** لا وسمُ النوعِ في مصدرِ
                    //      الحقيقة. وقد مرَّت المرساةُ بثلاثِ صيغٍ يُذكَرُ سقوطُ
                    //      اثنتَين لأنّ سببَ سقوطِهما هو ما يمنعُ إعادتَهما:
                    //
                    //      ① «موقَّعٌ بعرضِ ٦٤» مطلقًا — التقطَ «رقم»، وهو نوعُ
                    //         الحرفيّةِ الساكن، فصارَ `ط + 1` خطأً ورُفِضَ ٢٦ موضعًا
                    //         في المصفوفةِ ثلاثةٌ منها فقط مقصودة.
                    //      ② «`sized_primitive` للطرفَين» — سدَّ ① لكنّه أعفى «رقم»
                    //         **بأَسْرِه**، فمرَّ `طبيعي + رقم` صامتًا مُعطيًا
                    //         ١٨٤٤٦٧٤٤٠٧٣٧٠٩٥٥١٥٧٦ — عينَ الكذبةِ التي وُجِدَ
                    //         الحارسُ لأجلِها، وعبرَ **النوعِ الافتراضيِّ للّغة**.
                    //      ③ (القائمة، قرارُ المالك ٢٦ آب ٢٠٢٦) المتغيّرُ المُصرَّحُ
                    //         `رقم` يُسأَلُ والحرفيّةُ تُعفى. لأنّ ② أعفى بالوسمِ
                    //         حيثُ المقصودُ النيّة: مَن كتبَ `متغير رقم ر` أعلنَ
                    //         موقَّعيّةً ٦٤-بتّيّةً فسؤالُه واجب، ومَن كتبَ `1` لم
                    //         يُعلِنْ شيئًا فالهيمنةُ تسعُه.
                    //
                    //      ⚠️ ولازمُ ③ يُقالُ ولا يُخبَّأ: `ط + 1` يمرُّ و`ط + ر`
                    //      (حيثُ `ر = 1`) يُرفَض. فإخراجُ حرفيّةٍ إلى متغيّرٍ
                    //      **يُغيِّرُ الحكم** — وهو فرقُ فعلٍ لا فرقُ إملاء، لكنّه
                    //      يُفاجئُ في إعادةِ الهيكلة.
                    // (EN) Anchor on the writer's declaration, not the SoT tag.
                    //      ① "signed and 64 bits" also caught رقم (the literal's
                    //         static type): 26 matrix sites, only 3 intended.
                    //      ② "sized_primitive on both sides" exempted رقم wholesale,
                    //         letting `طبيعي + رقم` print 18446744073709551576 —
                    //         the very lie this guard exists to stop, via the
                    //         language's DEFAULT integer type.
                    //      ③ (current) a declared رقم variable is questioned; a bare
                    //         integer literal is exempt. Consequence, stated openly:
                    //         extracting a literal into a variable changes the verdict.
                    const bool leftIsUnsignedSide = leftUnsigned;
                    const Types::SadTypeKind unsignedKind = leftIsUnsignedSide ? leftKind : rightKind;
                    const Types::SadTypeKind signedKind = leftIsUnsignedSide ? rightKind : leftKind;
                    const AST::Expression *signedOperand =
                        leftIsUnsignedSide ? expr.right.get() : expr.left.get();

                    // (AR) الطرفُ اللا-موقَّعُ يلزمُه عرضٌ مُصرَّحٌ دائمًا: لا نوعَ
                    //      لا-موقَّعٍ افتراضيٍّ في «ص» فلا حرفيّةَ تصلُ من هنا.
                    const bool unsignedSideQualifies =
                        Types::sadTypeKindIsSizedPrimitive(unsignedKind) &&
                        Types::sadTypeKindNumericBits(unsignedKind) == 64;

                    // (AR) والطرفُ الموقَّعُ يُسأَلُ إن أعلنَ عرضَه (`رقم`) أو كانَ
                    //      «رقم» **أعلنَ كاتبُه إشارتَه** — لا حرفيّةً ولا متغيّرًا
                    //      استُنتِجَ نوعُه من مُهيِّئِه.
                    const bool signedSideQualifies =
                        Types::sadTypeKindNumericBits(signedKind) == 64 &&
                        (Types::sadTypeKindIsSizedPrimitive(signedKind) ||
                         writerDeclaredSignedness(signedOperand));

                    if (leftUnsigned != rightUnsigned &&
                        unsignedSideQualifies && signedSideQualifies)
                    {
                        reportCatalogError(
                            Errors::ErrorCode::SEM_UNSIGNED64_MIXED_WITH_SIGNED,
                            {{"left_type", Types::sadTypeKindArabicName(leftKind)},
                             {"right_type", Types::sadTypeKindArabicName(rightKind)}},
                            &expr);
                        lastInferredType_ = registry_.getUnknown();
                        return;
                    }
                }
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 نطاقُ المعاملات: من مصدرِ الحقيقةِ لا من جدولٍ يدويّ.
            //
            //      كان الجدولُ مكتوبًا أسفلَ هذا السطرِ في ذراعَي `+` و`- * / // %`،
            //      ومصدرُ الحقيقةِ لا يعرفُ منه شيئًا: لا حقلَ لنطاقِ المعاملاتِ في
            //      مخطَّطِ العواملِ إطلاقًا. وقِيسَ (١٩٣ قياسًا، ٢٠٢٦-٠٩-٠٣) أنّ
            //      السبعةَ الحسابيّةَ الثنائيّةَ لا يرفضُ منها شيئًا إلّا `**`؛
            //      والبقيّةُ تقبلُ كلَّ تركيبةٍ فتُنتِجُ قمامةً أو تنهار:
            //      `10 - [1، 2، 3]` يطبعُ عنوانًا برمزِ خروجٍ **صفر**، و
            //      `100 - "عشرة"` ينهارُ بإشارة — والعطبُ واحدٌ والفرقُ صدفةُ عنوان.
            //
            //      ⚠️ **والمِطلاقُ على اليقينِ لا على النوع.** الجدولُ اليدويُّ كان
            //      يُطلِقُ على `!isNumeric()`، و«لا أعرف» ليس عدديًّا — فكان خلفَ
            //      `--أنواع-صارمة` يُحمِّرُ **٣٠ بذرةً صحيحة**، أبسطُها
            //      `ارجع أ + ب` في دالّةٍ بمعاملَين عاريَين. فـ`violates` لا تحكمُ
            //      إلّا حين يكونُ الطرفانِ **كلاهما** مصنَّفَين في مصدرِ الحقيقة،
            //      وما عداه (`أي` · `عدم` · `فراغ` · `مجهول` · صنفٌ · تعدادٌ ·
            //      اختياريّ · معاملٌ عارٍ) يمرُّ بلا حكم. والرفضُ الكاذبُ أسوأُ من
            //      الانهيارِ الذي حلَّ محلَّه.
            //
            //      ولذلك خرجَ الحارسُ من `strictMode_`: عَلَمٌ يعيشُ بلا مُنادٍ يبقى
            //      **غيرَ مقيسٍ** لا أخضر — وقد عاشَ `--أنواع-صارمة` كذلك سنتَين.
            //      والقاعدتانِ وحدَهما انتقلتا؛ وبقيّةُ الوضعِ الصارمِ (٤٨٩ تحميرةً
            //      لا شأنَ لها بهذا الجذر) خلفَ عَلَمِها كما كانت.
            // (EN) 🔑 Operand domains now come from the SoT, not a hand-written
            //      table. Measured: of the seven binary arithmetic operators only `**`
            //      rejected anything; the rest accept everything and produce garbage or
            //      crash. The trigger is CERTAINTY, not type: `violates` judges only
            //      when BOTH operands are classified by the SoT — anything unknown
            //      passes. The old trigger fired on !isNumeric(), and "unknown" is not
            //      numeric, so it reddened 30 correct seeds. Moved out of strictMode_
            //      because a flag with no caller stays unmeasured, not green.
            // ════════════════════════════════════════════════════════════════
            if (leftType && rightType &&
                Types::OperandDomains::violates(expr.op,
                                                leftType->getKind(),
                                                rightType->getKind()))
            {
                reportCatalogError(
                    Errors::ErrorCode::SEM_OPERAND_DOMAIN,
                    {{"op", std::string(Types::OperandDomains::symbolOf(expr.op))},
                     {"left", Types::sadTypeKindArabicName(leftType->getKind())},
                     {"right", Types::sadTypeKindArabicName(rightType->getKind())}},
                    &expr);
                lastInferredType_ = registry_.getUnknown();
                return;
            }

            switch (expr.op)
            {
            // عمليات حسابية / Arithmetic
            case TT::OP_PLUS:
                // يمكن أن تكون جمع أرقام أو دمج نصوص
                if (leftType && rightType)
                {
                    if (leftType->isString() || rightType->isString())
                    {
                        lastInferredType_ = registry_.getString();
                    }
                    else if (leftType->isNumeric() && rightType->isNumeric())
                    {
                        // float يسود / float dominates
                        if (leftType->isFloat() || rightType->isFloat())
                        {
                            lastInferredType_ = registry_.getFloat();
                        }
                        else
                        {
                            lastInferredType_ = registry_.getInteger();
                        }
                    }
                    else
                    {
                        // (AR) 🔑 كان هنا نصٌّ إنجليزيٌّ خامٌّ خلفَ `strictMode_`
                        //      («Cannot add incompatible types») بلا رمزٍ ولا عربيّة.
                        //      والحكمُ انتقلَ إلى حارسِ النطاقِ أعلاه: يُبَثُّ بـ`SEM054`
                        //      من الكتالوج، ويجري افتراضيًّا، ولا يُطلَقُ على المجهول.
                        //      فما يبلغُ هنا اليومَ **مجهولُ الصنفِ لا محالة** — صنفٌ
                        //      أو تعدادٌ أو اختياريّ — فلا حكمَ عليه، ونوعُه مجهول.
                        // (EN) A raw English string behind strictMode_, with no code and
                        //      no Arabic. The verdict moved to the domain guard above,
                        //      which emits SEM054 from the catalog by default and never
                        //      fires on unknowns. What reaches here is unclassified.
                        lastInferredType_ = registry_.getUnknown();
                    }
                }
                else
                {
                    lastInferredType_ = registry_.getUnknown();
                }
                break;

            case TT::OP_MINUS:
            case TT::OP_MULTIPLY:
            case TT::OP_DIVIDE:
            case TT::OP_FLOOR_DIVIDE:
            case TT::OP_MODULO:
                // عمليات حسابية تتطلب أرقام / Arithmetic requires numbers
                if (leftType && rightType)
                {
                    if (leftType->isNumeric() && rightType->isNumeric())
                    {
                        if (expr.op == TT::OP_FLOOR_DIVIDE)
                        {
                            // (AR) القسمة الصحيحة // دائماً تنتج عدد صحيح
                            lastInferredType_ = registry_.getInteger();
                        }
                        else if (leftType->isFloat() || rightType->isFloat() || expr.op == TT::OP_DIVIDE)
                        {
                            lastInferredType_ = registry_.getFloat();
                        }
                        else
                        {
                            lastInferredType_ = registry_.getInteger();
                        }
                    }
                    else
                    {
                        // (AR) انظر الملحوظةَ في ذراعِ `+` أعلاه: الحكمُ صارَ في
                        //      حارسِ النطاقِ برمزِ `SEM054`، والنصُّ الخامُّ حُذف.
                        // (EN) See the note on the `+` arm above.
                        lastInferredType_ = registry_.getUnknown();
                    }
                }
                else
                {
                    lastInferredType_ = registry_.getUnknown();
                }
                break;

            // عمليات المقارنة / Comparison
            case TT::OP_EQUAL:
            case TT::OP_NOT_EQUAL:
            case TT::OP_LESS:
            case TT::OP_LESS_EQUAL:
            case TT::OP_GREATER:
            case TT::OP_GREATER_EQUAL:
                lastInferredType_ = registry_.getBoolean();
                break;

            // عمليات منطقية / Logical
            case TT::OP_AND:
            case TT::OP_OR:
                lastInferredType_ = registry_.getBoolean();
                break;

            default:
                lastInferredType_ = registry_.getUnknown();
                break;
            }
        }

        void TypeChecker::visitUnaryExpr(AST::UnaryExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr operandType = inferExprType(expr.operand.get());

            switch (expr.op)
            {
            case TT::OP_MINUS:
                if (operandType && operandType->isNumeric())
                {
                    lastInferredType_ = operandType;
                }
                else
                {
                    lastInferredType_ = registry_.getUnknown();
                    if (strictMode_)
                    {
                        recordTypeError("", "numeric",
                                        operandType ? operandType->toString() : "unknown",
                                        &expr, "Unary minus requires numeric operand");
                    }
                }
                break;

            case TT::OP_NOT:
                lastInferredType_ = registry_.getBoolean();
                break;

            default:
                lastInferredType_ = operandType ? operandType : registry_.getUnknown();
                break;
            }
        }

        void TypeChecker::visitTernaryExpr(AST::TernaryExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr condType = inferExprType(expr.condition.get());
            Sad::Types::SadTypePtr trueType = inferExprType(expr.trueExpr.get());
            Sad::Types::SadTypePtr falseType = inferExprType(expr.falseExpr.get());

            // الشرط يجب أن يكون منطقي / Condition should be boolean
            if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
            {
                recordTypeError("", "boolean", condType->toString(), &expr,
                                "Ternary condition should be boolean");
            }
            // (AR) وموضعُ التشخيصِ من عقدةِ **الشرطِ** لا من عقدةِ الجملة: كان
            //      «اذا (())» في السطرِ الأوّلِ يُبلَّغُ عنه في سطرِ «نهاية».
            // (EN) The location comes from the CONDITION node, not the statement's.
            rejectUnitCondition(condType,
                                expr.condition ? static_cast<AST::ASTNode *>(expr.condition.get())
                                               : static_cast<AST::ASTNode *>(&expr),
                                Lexer::TokenType::KEYWORD_IF);

            // إذا تطابق الفرعين / If branches match
            if (trueType && falseType && trueType->getKind() == falseType->getKind())
            {
                lastInferredType_ = trueType;
            }
            else
            {
                // Union type would be ideal, fall back to broader type
                lastInferredType_ = trueType ? trueType : registry_.getUnknown();
            }
        }

        void TypeChecker::visitAssignExpr(AST::AssignExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr valueType = inferExprType(expr.value.get());

            auto varType = lookupVariable(expr.name);
            if (varType && valueType)
            {
                // ================================================================
                // (AR) [Fix #53] لغة ص ديناميكية — إعادة الإسناد عبر الأنواع مسموحة:
                //      متغير س = 42      →  نوع: رقم
                //      س = "نص الآن"      →  نوع: نص (مسموح — ديناميكية الأنواع)
                //      في الوضع الصارم (strictMode_) فقط نُبلّغ عن عدم التطابق.
                //      بدون هذا: المترجم يرفض إعادة الإسناد ولا يُنشئ ملف تنفيذي.
                // (EN) [Fix #53] Sad is dynamically typed — cross-type reassignment is allowed:
                //      Only report mismatch in strict mode.
                //      Without this: compiler rejects reassignment and fails to compile.
                // ================================================================
                if (strictMode_ && !areTypesCompatible(varType, valueType))
                {
                    recordTypeError(expr.name,
                                    varType->toString(), valueType->toString(),
                                    &expr);
                }

                // (AR) تحديث نوع المتغير في بيئة الأنواع ليعكس النوع الجديد
                // (EN) Update variable type in type environment to reflect new type
                // ================================================================
                // (AR) ⚠️ ولا يضيقُ `أي` بأوّلِ إسناد: التتبّعُ التدفّقيُّ صوابٌ لخانةٍ
                //      استُنتِج نوعُها، وخطأٌ لخانةٍ **صرّح كاتبُها** أنّها ديناميّة.
                //      ولولا هذا القيدُ لصحَّ `أي س = 5` ثمّ `س = "نص"` وأخفق الثالثُ
                //      `س = 7` — فيصير قبولُ `أي` رهنَ **عددِ** الإسناداتِ لا معناها،
                //      وهو أسوأُ من الرفضِ الصريح لأنّه يُقرأ عشوائيًّا.
                //      والفحصُ على النوعِ المربوطِ لا على تصريحٍ محفوظ، فالفاحصُ لا
                //      يحتفظ بالمُصرَّح؛ وأثرُه لا يزيد على تعطيلِ تضييقٍ، فلا يُنشئ
                //      إخفاقًا جديدًا في أيِّ حال.
                // (EN) An `أي` slot must not be narrowed by its first assignment: flow
                //      tracking is right for an inferred slot and wrong for one the
                //      author declared dynamic. Otherwise the 2nd assignment passes and
                //      the 3rd fails — acceptance by assignment COUNT, which reads as
                //      random. The test is on the bound type (the checker keeps no record
                //      of the declaration); it can only disable a narrowing, never create
                //      a new failure.
                // ================================================================
                if (valueType && !valueType->isUnknown() && !(varType && varType->isAny()))
                {
                    currentEnv_->bind(expr.name, valueType);
                }
            }

            lastInferredType_ = valueType;
        }

        void TypeChecker::visitCallExpr(AST::CallExpr &expr)
        {
            currentResult_.totalExpressions++;

            // تحقق من أنواع المعاملات / Check argument types
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }

            // ============================================================
            // (AR) [أ-م٢] حسم هويّة بناء معامل تعداد بحمولة: «عدد(٥)» يُحلَّل نحويًّا
            //      كاستدعاء دالّة، لكن إن كان اسم المُستدعى معامل تعداد معروفًا فهو
            //      بناء قيمة موسومة (ADT) لا استدعاء دالّة غير معرّفة. نفحص الحمولة
            //      (عددًا/أنواعًا) ونثبّت النوع المُستنتَج ونعود مبكّرًا (لا نعامله دالّة).
            //      بناء القيمة الفعليّة (الاتّحاد الموسوم) في المفسّر/codegen (أ-م٣/أ-م٤).
            // (EN) [A-M2] Resolve tagged-enum variant construction: «عدد(5)» parses as a
            //      function call, but if the callee name is a known enum variant it is a
            //      tagged (ADT) value construction, not an undefined function call. We check
            //      the payload (arity/types), set the inferred type, and return early.
            //      Actual value building (tagged union) is in interpreter/codegen (A-M3/A-M4).
            // ============================================================
            // ════════════════════════════════════════════════════════════════
            // (AR) [RFC 0059 — ح٢] لا تُنادى دالّةُ المقاطعة. بوّابةُ المقاطعةِ تُدخَل
            //      بإطارٍ يدفعُه العتادُ وتُغادَر بـiretq، فنداؤها كدالّةٍ عاديّةٍ يدخلُها
            //      بإطارٍ لم يوجد ثمّ يخرجُ على مكدّسٍ مكسور: انهيارٌ صامتٌ لا يمسكُه
            //      اختبار. عنوانُها يُؤخَذ بـ«عنوان_رمز» ويُسجَّل في جدولِ المقاطعات.
            //      الحارسُ هنا في موضعِ النداءِ الاسميّ — والنداءُ غيرُ المباشرِ عبر
            //      مرجعِ دالّةٍ دَينٌ معلَنٌ (لا يُلتقَط اسمًا في هذه الطبقة).
            // (EN) [RFC 0059] An interrupt handler is never called from Sad.
            // ════════════════════════════════════════════════════════════════
            if (auto *intrCallee = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
            {
                if (interruptHandlerNames_.count(intrCallee->name) > 0)
                {
                    reportCatalogError(
                        Errors::ErrorCode::SEM_INTERRUPT_HANDLER_CONTRACT,
                        {{"detail",
                          "«" + intrCallee->name + "»: معالجُ المقاطعةِ لا يُنادى — "
                          "خذ عنوانَه بـ«عنوان_رمز» وسجّله في جدولِ المقاطعات"}},
                        &expr);
                }
            }

            if (auto *calleeVar = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
            {
                // (AR) المُعامِل يخسر التنازع أمام دالّة/رمزٍ مُصرَّحٍ صراحةً (🔴-٢ من مراجعة أميليا):
                //      لا نختطف نداءً إلى دالّة مستخدم اسمُها يطابق اسم معامل تعداد.
                // (EN) A variant loses to an explicitly declared function/symbol (Amelia 🔴-2):
                //      do not hijack a call to a user function whose name matches a variant.
                Sad::Types::SadTypePtr declared = lookupVariable(calleeVar->name);
                const bool isDeclaredCallable =
                    declared && std::dynamic_pointer_cast<const Sad::Types::SadFunctionType>(declared) != nullptr;
                if (!isDeclaredCallable)
                {
                    std::string owningEnum;
                    bool ambiguous = false;
                    const EnumVariantInfo *variant =
                        lookupVariant(calleeVar->name, owningEnum, ambiguous);
                    if (variant)
                    {
                        lastInferredType_ = checkEnumConstruction(
                            calleeVar->name, *variant, expr.arguments, &expr);
                        return;
                    }
                }
            }

            // ============================================================
            // (AR) [Phase 5b] استدلال نوعي للقوالب: إن كان الاستدعاء بدون
            //      <...> لكنه يستهدف اسم قالب معروف، حاول استنتاج
            //      وسائط الأنواع من أنواع الوسائط المُمرَّرة وفرض القيود.
            // (EN) [Phase 5b] Template type-arg inference: if the call has
            //      no <...> but targets a known template name, try to
            //      infer type args from passed argument types and enforce
            //      the corresponding constraints.
            // ============================================================
            if (auto *idExpr = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
            {
                auto tIt = userTemplates_.find(idExpr->name);
                if (tIt != userTemplates_.end())
                {
                    const auto &tmpl = tIt->second;

                    // (AR) اجمع أسماء أنواع الوسائط بشكل آمن.
                    // (EN) Collect argument type-name strings safely.
                    std::vector<std::string> argTypeNames;
                    argTypeNames.reserve(expr.arguments.size());
                    for (auto &a : expr.arguments)
                        argTypeNames.push_back(extractTypeNameFromExpr(a.get()));

                    std::vector<std::string> inferred;
                    if (inferTemplateTypeArguments(tmpl, argTypeNames, inferred))
                    {
                        // (AR) فرض القيود على كل معامل نوعي مُستدلّ.
                        // (EN) Enforce constraints on each inferred type param.
                        std::vector<const ParamInfo *> typeParams;
                        for (const auto &p : tmpl.typeParameters)
                        {
                            if (!p.isConst)
                                typeParams.push_back(&p);
                        }
                        size_t n = std::min(typeParams.size(), inferred.size());
                        for (size_t i = 0; i < n; ++i)
                        {
                            enforceParamConstraints(*typeParams[i],
                                                    inferred[i],
                                                    expr.position);
                        }
                        // (AR) فرض جملة حيث (where) أيضاً.
                        // (EN) Enforce where clause too.
                        for (const auto &item : tmpl.whereItems)
                        {
                            int idx = -1;
                            for (size_t k = 0; k < typeParams.size(); ++k)
                            {
                                if (typeParams[k]->name == item.typeName)
                                {
                                    idx = static_cast<int>(k);
                                    break;
                                }
                            }
                            if (idx < 0 || idx >= static_cast<int>(inferred.size()))
                                continue;
                            ParamInfo synthetic;
                            synthetic.name = item.typeName;
                            synthetic.isConst = false;
                            synthetic.constraints = item.constraints;
                            enforceParamConstraints(synthetic,
                                                    inferred[idx],
                                                    expr.position);
                        }

                        if (debugMode_)
                        {
                            std::cerr << "  [TC][5b] Inferred template '"
                                      << idExpr->name << "' type args:";
                            for (const auto &t : inferred)
                                std::cerr << " " << t;
                            std::cerr << "\n";
                        }
                    }
                }
            }

            // استنتج نوع الإرجاع من المُستدعى / Infer return type from callee
            Sad::Types::SadTypePtr calleeType = inferExprType(expr.callee.get());

            // إذا كان CalleeType function type، استخرج نوع الإرجاع
            if (calleeType && calleeType->getKind() == SadTypeKind::Function)
            {
                auto *fnType = static_cast<const Sad::Types::SadFunctionType *>(calleeType.get());

                // (AR) [Zone 2] فحص أنواع الوسائط مقابل أنواع المعاملات المُصرَّحة.
                //      نُقارن عدد الوسائط وأنواعها. نتجاهل المعاملات ذات النوع UNKNOWN/ANY.
                // (EN) [Zone 2] Check argument types against declared parameter types.
                //      Compare argument count and types. Skip UNKNOWN/ANY params.
                const auto &paramTypes = fnType->getParamTypes();
                size_t nArgs = expr.arguments.size();
                size_t nParams = paramTypes.size();
                for (size_t i = 0; i < nArgs && i < nParams; ++i)
                {
                    if (!expr.arguments[i])
                        continue;
                    Sad::Types::SadTypePtr paramT = paramTypes[i];
                    Sad::Types::SadTypePtr argT = inferExprType(expr.arguments[i].get());
                    // (AR) تجاهل المعاملات غير المعروفة الأنواع أو ANY
                    // (EN) Skip unknown/any parameter types
                    if (!paramT || paramT->isUnknown() || paramT->getKind() == SadTypeKind::Any)
                        continue;
                    // (AR) تجاهل الوسائط غير المعروفة الأنواع
                    // (EN) Skip unknown argument types
                    if (!argT || argT->isUnknown() || argT->getKind() == SadTypeKind::Any)
                        continue;
                    if (!areTypesCompatible(paramT, argT))
                    {
                        std::string funcName;
                        if (auto *idExpr = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
                            funcName = idExpr->name;
                        recordTypeError(funcName,
                                        paramT->toString(), argT->toString(),
                                        &expr,
                                        "Argument type mismatch in call to '" + funcName +
                                            "': parameter " + std::to_string(i + 1));
                    }
                }

                Sad::Types::SadTypePtr retType = fnType->getReturnType();
                // (AR) إذا لم يُحدَّد نوع الإرجاع، نفترض رقم (مثل int في C)
                // (EN) If return type is unspecified, default to integer (like C's implicit int)
                lastInferredType_ = retType ? retType : registry_.getInteger();
            }
            else
            {
                // (AR) SEM045 (انحدار t001): نوعُ المستدعى هنا هو نوعُ النداء (تسجيلُ
                //      القوالب بخانةِ الإرجاع) — وصارت الخانةُ صادقةً في المنبع:
                //      visitTemplateFunctionDecl يميّز «الفراغَ اليقينيَّ» عن المُغفَلِ
                //      المُرجِعِ قيمةً (يُدوَّن مجهولًا)، فلا تحويلَ دفاعيًّا هنا — تحويلُ
                //      Void→Unknown جملةً أضاع D8 عن القالبِ الفراغيِّ حقًّا (قِيس).
                // (EN) SEM045 (regression t001): the callee type IS the call type here
                //      (templates register by their return slot) — and the slot is now
                //      truthful at the source: visitTemplateFunctionDecl distinguishes
                //      certainly-void from undeclared-value-returning (Unknown), so no
                //      defensive conversion happens here — a blanket Void→Unknown lost
                //      D8 for genuinely void templates (measured).
                lastInferredType_ = calleeType ? calleeType : registry_.getUnknown();
            }
        }

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
