/**
 * @file statement_executor_control.cpp
 * @brief (AR) تنفيذ جمل التحكم المتقدمة في StatementExecutor
 * @brief (EN) Advanced control-flow statement execution for StatementExecutor
 *
 * (AR) ملف مُستخرج من statement_executor.cpp لتقليل حجم الملف وفق CW-05
 */

#include "statement_executor.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "error_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "object_instance.h"
#include "debug_server.h"
#include "class_manager.h"
#include "utils/class_module_captures.h" // (AR) ع-1: حقن ثوابت وحدة التعريف
#include "channel.h"
#include "sad_type_system.h"
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include "lexer_keywords.h" // (AR) تهجئةُ الكلماتِ من معجمِ SoT / (EN) SoT keyword spellings
#include <iostream>
#include <sstream>
#include <map>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) تهجئةُ الكلمةِ المفتاحيّةِ كما في معجمِ مصدرِ الحقيقة — لا نصًّا مكتوبًا هنا.
        //      كانت تشخيصاتُ SEM013 تذكر «اخرج/break» و«تابع/continue» وهما تهجئتان
        //      لا وجودَ لهما في اللغة، فيقرأ المستعمِلُ كلمةً لا يستطيع كتابتَها.
        // (EN) Keyword spelling straight from the SoT lexicon — never hard-coded here.
        //      SEM013 used to name «اخرج/break» and «تابع/continue», spellings the
        //      language does not have, so the reader saw a word they cannot write.
        static inline std::string keywordSpelling(Lexer::TokenType type)
        {
            const auto *entry = Lexer::KeywordTable::getEntry(type);
            return entry ? entry->primaryWord : std::string();
        }

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) خطاف التصحيح — يُستدعى قبل تنفيذ كل جملة
        // (EN) Debug hook — called before each statement execution
        // ═══════════════════════════════════════════════════════════════
        static inline void debugHook(const Sad::Lexer::Position &pos)
        {
            auto *dbg = Sad::Debug::DebugServer::getInstance();
            if (dbg && dbg->isConnected())
            {
                dbg->onBeforeStatement(
                    getSourceFilename(),
                    static_cast<int>(pos.line),
                    static_cast<int>(pos.column));
                if (dbg->shouldStop())
                {
                    throw std::runtime_error("تم فصل المصحح / Debugger disconnected");
                }
            }
        }
        // (AR) تنفيذ جملة Switch-Case / (EN) Switch-Case Statement Execution
        // =========================================================================

        /**
         * @brief (AR) يُنفذ جملة switch-case: حالة تعبير ... نهاية
         *        (EN) Executes switch-case statement: case expression ... end
         *
         * Evaluates the switch expression, then compares it against each case value.
         * Executes the body of the first matching case, or the default case if no match.
         *
         * يُقيّم تعبير switch، ثم يقارنه مع كل قيمة حالة.
         * يُنفذ جسم أول حالة مطابقة، أو الحالة الافتراضية إذا لم يوجد تطابق.
         *
         * Syntax / النحو:
         *   حالة <expression>
         *       عندما <value>: <statement>
         *       [افتراضي: <statement>]
         *   نهاية
         *
         * @note Unlike C, there's no fall-through - only one case executes
         * @note على عكس C، لا يوجد fall-through - تُنفذ حالة واحدة فقط
         */
        void StatementExecutor::visitSwitchStmt(AST::SwitchStmt &node)
        {
            // (AR) تقييم تعبير Switch / (EN) Evaluate switch expression
            Data::Value switchValue = evaluateExpression(*node.expression);

            // (AR) علامة لمعرفة إذا وجدنا تطابق / (EN) Flag to track if we found a match
            bool foundMatch = false;

            // (AR) التكرار عبر حالات Case / (EN) Iterate through case branches
            for (const auto &caseItem : node.cases)
            {
                // (AR) تقييم قيمة الحالة / (EN) Evaluate case value
                Data::Value caseValue = evaluateExpression(*caseItem.value);

                // (AR) مقارنة القيم / (EN) Compare values
                // Use operator== which returns Value, then convert to bool
                if ((switchValue == caseValue).toBool())
                {
                    // (AR) وجدنا تطابق، نفذ جسم الحالة / (EN) Found match, execute case body
                    foundMatch = true;

                    // (AR) تنفيذ جسم الحالة في نطاق خاص / (EN) Execute case body in its own scope
                    variableManager_.enterScope(Data::ScopeType::BLOCK);
                    caseItem.body->accept(*this);
                    variableManager_.exitScope();

                    // (AR) لا يوجد fall-through، اخرج بعد التنفيذ / (EN) No fall-through, exit after execution
                    break;
                }
            }

            // (AR) إذا لم نجد تطابق، نفذ الحالة الافتراضية إن وُجدت / (EN) If no match, execute default case if present
            if (!foundMatch && node.defaultCase)
            {
                variableManager_.enterScope(Data::ScopeType::BLOCK);
                node.defaultCase->accept(*this);
                variableManager_.exitScope();
            }

            // (AR) ملاحظة: لا حاجة لإدارة break/continue لأن switch ليس حلقة
            // (EN) Note: No need to handle break/continue as switch is not a loop
        }

        // =========================================================================
        // (AR) تنفيذ جُمل إدارة التدفق / (EN) Flow Management Statement Visitors
        // =========================================================================

        void StatementExecutor::visitReturnStmt(AST::ReturnStmt &node)
        {
            // (AR) تقييم قيمة الإرجاع إن وُجدت / (EN) Evaluate return value if present
            if (node.value)
            {
                returnValue_ = evaluateExpression(*node.value);

                // ═══════════════════════════════════════════════════════════
                // (AR) التحقق من توافق نوع الإرجاع عبر النظام الموحد
                //      إذا كان للدالة الحالية نوع إرجاع معلن (sadReturnType)
                //      نتحقق أن القيمة المرجعة متوافقة معه
                // (EN) Check return type compatibility via unified type system
                //      If current function has declared return type (sadReturnType)
                //      verify the returned value is compatible
                // ═══════════════════════════════════════════════════════════
                if (currentFunctionSadReturnType_ &&
                    currentFunctionSadReturnType_->getKind() != Types::SadTypeKind::Unknown &&
                    currentFunctionSadReturnType_->getKind() != Types::SadTypeKind::Any &&
                    currentFunctionSadReturnType_->getKind() != Types::SadTypeKind::Void)
                {
                    auto valType = Types::SadType::fromValueType(returnValue_.getType());
                    if (valType && !valType->isAssignableTo(currentFunctionSadReturnType_.get()))
                    {
                        std::cerr << "[تحذير نوع] سطر " << node.position.line
                                  << ": دالة '" << currentFunctionName_
                                  << "' تتوقع إرجاع '" << currentFunctionSadReturnType_->arabicName()
                                  << "' لكن حصلت على '" << valType->arabicName()
                                  << "'" << std::endl;

                        Sad::Errors::SourceLocation loc(
                            "", node.position.line, node.position.column);
                        Sad::Errors::ErrorManager::getInstance().reportWarning(
                            Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                            loc,
                            "دالة '" + currentFunctionName_ + "' تتوقع إرجاع '" +
                                currentFunctionSadReturnType_->arabicName() + "' لكن حصلت على '" +
                                valType->arabicName() + "'",
                            "Function '" + currentFunctionName_ + "' expects return type '" +
                                currentFunctionSadReturnType_->englishName() + "' but got '" +
                                valType->englishName() + "'");
                    }

                    // ════════════════════════════════════════════════════════════
                    // (AR) 🔑 موافقةُ القيمةِ للنوعِ المصرَّح — بعدَ الفحصِ لا قبلَه.
                    //      التصريحُ يحكم كما في C: `دالة عشري` تُرجِع صحيحًا ⇒ عشريّ،
                    //      و`دالة رقم` تُرجِع عشريًّا ⇒ يُقتطَع نحو الصفر. وكان هذا
                    //      الموضعُ يقرأ النوعَ المصرَّحَ **ليُحذّر فقط**، فيُترَك النوعُ
                    //      كما هو ويفترق المحرّكان في القيمةِ نفسِها: `ق / 2` في
                    //      `دالة عشري` كانت 2 مفسَّرةً و2.5 مترجَمةً.
                    //      والترتيبُ مقصود: لو سبقَ التحويلُ الفحصَ لصار النوعُ مطابقًا
                    //      دائمًا فيُخرَس تحذيرٌ قائمٌ — يُقال ما وقع، ثمّ يُوافَق.
                    //      وكشفَ هذا كلَّه قرارُ جعلِ `/` قسمةً صحيحة: قبلَه كانت 5/2
                    //      عشريّةً أصلًا فلم يُمتحَن التحويلُ قطُّ.
                    // (EN) Coerce the value to the declared type — after the check, not
                    //      before. The declaration governs, as in C: a `عشري` (float)
                    //      function returning an integer yields a float, and a `رقم` (int)
                    //      function returning a float truncates toward zero. This site read
                    //      the declared type ONLY to warn, leaving the type as-is, so the two
                    //      engines differed in the VALUE itself.
                    //      The order is deliberate: coercing first would make the types always
                    //      match and silence a real warning — say what happened, then coerce.
                    //      All of this surfaced when `/` became integer division: before that,
                    //      5/2 was already a float, so the conversion was never exercised.
                    // ════════════════════════════════════════════════════════════
                    const auto declaredKind = currentFunctionSadReturnType_->getKind();
                    if (declaredKind == Types::SadTypeKind::Float && returnValue_.isInteger())
                    {
                        returnValue_ = Data::Value(static_cast<double>(returnValue_.toInt64()));
                    }
                    else if (declaredKind == Types::SadTypeKind::Integer && returnValue_.isDouble())
                    {
                        returnValue_ = Data::Value(static_cast<int64_t>(returnValue_.toDouble()));
                    }
                }

                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 «بايت» عائدًا — معبَرٌ ثالثٌ كان يكذبُ العقد. مصدرُ
                //      الحقيقةِ يُعلن «بايت» u8 مدىً 0–255، والاقتطاعُ كان مسدودًا
                //      عند التصريحِ وإعادةِ الإسنادِ وحدَهما: `دالة بايت ص() ارجع 300`
                //      كانت تُخرِج 300 في المحرّكَين معًا — لا تباعُدَ بينهما بل
                //      عقدٌ غيرُ مصونٍ في كليهما. والقناعُ `& 0xFF` هو نفسُه المطبَّق
                //      في `visitVarDecl` و`visitAssignExpr`، فيبقى معنى النوعِ واحدًا
                //      مهما كان المعبَر. ونظيرُه في المترجّم TRUNCATE_U8 عند `ارجع`.
                // (AR) والمرساةُ `currentFunctionReturnType_` لا `currentFunctionSadReturnType_`:
                //      الثاني يُحفَظ ويُستعاد في `executeFunctionBodyWithReturnType` ولا
                //      يُسنَد إليه قطُّ — فحارسٌ عليه حارسٌ لا يُطلَق. الأوّلُ يُسنَد من
                //      `functionReturnTypes_` فعلًا، فهو الطبقةُ التي تحملُ الواقع.
                // (EN) Byte as a return value — the third crossing that broke the
                //      contract. The SoT declares Byte as u8 (0-255), yet truncation
                //      was sealed only at declaration and reassignment: a `بايت`
                //      function returning 300 printed 300 in BOTH engines — not a
                //      divergence between them but a contract unhonoured by both.
                //      Anchored on currentFunctionReturnType_, NOT the SadType pointer:
                //      the latter is saved and restored but never assigned, so a guard
                //      hung on it could never fire.
                // ════════════════════════════════════════════════════════════
                // (AR) عُمِّم من «بايت» إلى كلِّ عرضٍ مُعلَنٍ دونَ ٦٤: مقيسًا كانت
                //      `دالة رقم8 ك() ارجع 400` تُخرِج ٤٠٠ في المحرّكَين معًا.
                //      والمُطبِّعُ محايدٌ عندَ عرضِ ٦٤ فلا يتغيّرُ عائدٌ مقيسٌ اليوم.
                // (EN) Generalized from Byte to every declared sub-64 width; identity
                //      at 64 bits, so no measured return moves.
                // (AR) 🔑 والشرطُ «دونَ ٦٤» لا «عددٌ صحيح»: `رقم` عرضُه ٦٤ فالمطبِّعُ
                //      محايدٌ عليه، لكنّ ذراعَ العشريِّ أدناه ليست محايدة — فتعميمُ
                //      الشرطِ إلى كلِّ صحيحٍ جعلَ طريقةً مصرَّحةً «رقم» تقصُر عائدَها
                //      العشريَّ ٥٫٠ إلى ٥، وذلك عقدٌ مقيسٌ عمدًا في البذرتَين
                //      126/146: المفسّرُ **لا يقسر** عوائدَ الطرائق. فالحارسُ يُشترَط
                //      على العرضِ المُعلَنِ دونَ ٦٤ حصرًا — وهو مدى «بايت» و«رقم8»
                //      و«طبيعي16» وأخواتِها لا غير.
                // (EN) The guard is "sub-64 width", not "any integer": «رقم» is 64-bit
                //      so the normalizer is identity on it, but the float arm below is
                //      NOT — widening the guard to every integer made a method declared
                //      «رقم» truncate its 5.0 return to 5, breaking the deliberately
                //      measured contract of seeds 126/146 (the interpreter does not
                //      coerce METHOD returns). Restricted to declared sub-64 widths.
                const int declaredReturnBits =
                    Types::sadTypeKindNumericBits(currentFunctionReturnType_);
                if (Types::sadTypeKindIsIntegerNumeric(currentFunctionReturnType_) &&
                    declaredReturnBits != Types::kSadTypeSizeUnknown &&
                    declaredReturnBits > 0 && declaredReturnBits < 64)
                {
                    // (AR) والعشريُّ يُقصَرُ نحوَ الصفرِ ثمّ يُقنَّع — نظيرُ ذراعِ
                    //      `رقم` أعلاه ونظيرُ `needNarrow` في المترجّم. ولولاه
                    //      لبقيَ `دالة بايت ص() ارجع 300.0` عشريًّا في المفسّرِ
                    //      بينما يقتطعُه المترجّم — تباعُدٌ في القيمةِ نفسِها.
                    // (EN) A float is truncated toward zero then masked — the sibling
                    //      of the `رقم` arm above and of needNarrow in the compiler.
                    if (returnValue_.getKind() == Types::SadTypeKind::Integer)
                    {
                        returnValue_ = Data::Value(static_cast<int64_t>(
                            Types::sadTypeKindNormalizeInteger(currentFunctionReturnType_,
                                                               returnValue_.toInt64())));
                    }
                    else if (returnValue_.isDouble())
                    {
                        returnValue_ = Data::Value(static_cast<int64_t>(
                            Types::sadTypeKindNormalizeInteger(
                                currentFunctionReturnType_,
                                static_cast<long long>(returnValue_.toDouble()))));
                    }
                }
                // (AR) والوسمُ خارجَ حارسِ «دونَ ٦٤»: `دالة رقم` عرضُها ٦٤ فلا
                //      تُقتطَع، لكنّ `نوع()` عليها يجبُ أن يقولَ «رقم» لا «رقم» —
                //      فالاقتطاعُ والوسمُ سؤالانِ مختلفانِ لهما شرطانِ مختلفان.
                // (EN) Tagging sits OUTSIDE the sub-64 guard: a `رقم` return is not
                //      truncated (64-bit) yet نوع() must still answer «رقم».
                //      Truncation and tagging are different questions.
                returnValue_.tagDeclaredWidth(currentFunctionReturnType_);
            }
            else
            {
                returnValue_ = Data::Value(); // VOID
            }

            // (AR) تعيين حالة التحكم / (EN) Set flow control state
            flowControl_ = FlowControl::RETURN;
        }

        void StatementExecutor::visitBreakStmt(AST::BreakStmt &node)
        {
            // (AR) التحقق من أننا داخل حلقة / (EN) Check that we're inside a loop
            if (!isInLoop())
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"keyword", keywordSpelling(Lexer::TokenType::KEYWORD_BREAK)}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_CONTROL_OUTSIDE_LOOP, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                };
                return;
            }

            // (AR) تعيين حالة التحكم / (EN) Set flow control state
            flowControl_ = FlowControl::BREAK;
        }

        void StatementExecutor::visitContinueStmt(AST::ContinueStmt &node)
        {
            // (AR) التحقق من أننا داخل حلقة / (EN) Check that we're inside a loop
            if (!isInLoop())
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"keyword", keywordSpelling(Lexer::TokenType::KEYWORD_CONTINUE)}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_CONTROL_OUTSIDE_LOOP, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                };
                return;
            }

            // (AR) تعيين حالة التحكم / (EN) Set flow control state
            flowControl_ = FlowControl::CONTINUE;
        }

        // =========================================================================
        // (AR) تنفيذ جملة yield (للمولّدات) / (EN) Yield Statement (for generators)
        // =========================================================================

        void StatementExecutor::visitYieldStmt(AST::YieldStmt &node)
        {
            // (AR) التحقق من أننا داخل مولّد / (EN) Check that we're inside a generator
            if (!inGenerator_)
            {
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_YIELD_OUTSIDE_GENERATOR, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)));
                return;
            }

            // (AR) التحقق من نوع yield / (EN) Check yield type
            if (node.isYieldFrom)
            {
                // (AR) yield from - تفويض إلى مولّد آخر
                // (EN) yield from - delegate to another generator
                if (node.value)
                {
                    Data::Value iterable = evaluateExpression(*node.value);

                    if (!iterable.isArray())
                    {
                        {
                            Sad::Errors::RenderContext _rc;
                            _rc.placeholders = {{"type", iterable.getTypeName()}};
                            Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_NOT_ITERABLE, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                        };
                        return;
                    }

                    yieldValue_ = iterable;
                }
                else
                {
                    yieldValue_ = Data::Value(); // VOID
                }
            }
            else
            {
                // (AR) yield عادي - إنتاج قيمة واحدة
                // (EN) Regular yield - produce single value
                if (node.value)
                {
                    yieldValue_ = evaluateExpression(*node.value);
                }
                else
                {
                    yieldValue_ = Data::Value(); // VOID
                }
            }

            // (AR) في وضع المولّد: جمع القيم بدون إيقاف التنفيذ
            // (EN) In generator mode: collect values without stopping execution
            if (inGenerator_)
            {
                generatorYieldValues_.push_back(yieldValue_);
                // (AR) لا نُعيّن flowControl_ حتى يستمر التنفيذ
                // (EN) Don't set flowControl_ so execution continues
                return;
            }

            // (AR) تعيين حالة التحكم / (EN) Set flow control state
            flowControl_ = FlowControl::YIELD;
        }

        // =========================================================================
        // (AR) تنفيذ مدير السياق (With Statement) / (EN) Context Manager (With Statement) Visitors
        // =========================================================================

        void StatementExecutor::visitWithStmt(AST::WithStmt &node)
        {
            // (AR) تقييم تعبير المورد / (EN) Evaluate resource expression
            Data::Value resource = evaluateExpression(*node.resource);

            // (AR) القيمة التي ستُعيّن للمتغير المستعار / (EN) Value to assign to alias variable
            Data::Value contextValue = resource;

            // (AR) إذا كان المورد كائناً، نحاول استدعاء __دخول__() أو __enter__()
            // (EN) If resource is object, try calling __دخول__() or __enter__()
            bool hasEnterExit = false;
            if (resource.isObject())
            {
                auto obj = resource.toObject();
                if (obj && (obj->hasMethod("__دخول__") || obj->hasMethod("__enter__")))
                {
                    hasEnterExit = true;
                    // (AR) استدعاء __دخول__() — نبحث عن الطريقة ونستدعيها
                    // (EN) Call __enter__() — find the method and invoke it
                    std::string enterName = obj->hasMethod("__دخول__") ? "__دخول__" : "__enter__";
                    auto *enterMethod = obj->getMethod(enterName);
                    if (enterMethod && enterMethod->getBody())
                    {
                        variableManager_.enterScope(Data::ScopeType::FUNCTION, enterName);
                        // (AR) ع-1: ثوابت وحدة التعريف قبل «هذا»
                        // (EN) ع-1: defining module's constants before «هذا»
                        Utils::injectClassModuleCaptures(obj->getClass(), variableManager_);
                        variableManager_.define("هذا", resource);
                        enterMethod->getBody()->accept(*this);
                        contextValue = returnValue_;
                        variableManager_.exitScope();
                    }
                }
            }

            // (AR) دخول نطاق جديد لـ with / (EN) Enter new scope for with
            variableManager_.enterScope(Data::ScopeType::BLOCK);

            // (AR) تعريف المتغير المستعار إذا وُجد / (EN) Define alias variable if present
            if (!node.alias.empty())
            {
                variableManager_.define(node.alias, contextValue);
            }

            // (AR) تنفيذ جسم كتلة with / (EN) Execute with block body
            bool exceptionOccurred = false;
            std::string exceptionMessage;

            try
            {
                node.body->accept(*this);
            }
            catch (const UserThrownException &e)
            {
                exceptionOccurred = true;
                exceptionMessage = e.getMessage();
            }
            catch (...)
            {
                exceptionOccurred = true;
                exceptionMessage = "Unknown exception";
            }

            // (AR) استدعاء __خروج__() أو __exit__() إذا كان الكائن يدعمها
            // (EN) Call __خروج__() or __exit__() if the object supports it
            if (hasEnterExit && resource.isObject())
            {
                auto obj = resource.toObject();
                if (obj)
                {
                    std::string exitName = obj->hasMethod("__خروج__") ? "__خروج__" : "__exit__";
                    auto *exitMethod = obj->getMethod(exitName);
                    if (exitMethod && exitMethod->getBody())
                    {
                        try
                        {
                            variableManager_.enterScope(Data::ScopeType::FUNCTION, exitName);
                            // (AR) ع-1: ثوابت وحدة التعريف قبل «هذا»
                            // (EN) ع-1: defining module's constants before «هذا»
                            Utils::injectClassModuleCaptures(obj->getClass(), variableManager_);
                            variableManager_.define("هذا", resource);
                            // (AR) تمرير معلومات الاستثناء إن وجد
                            // (EN) Pass exception info if any
                            variableManager_.define("__استثناء__",
                                                    exceptionOccurred ? Data::Value(exceptionMessage) : Data::Value());
                            exitMethod->getBody()->accept(*this);
                            // (AR) إذا أرجعت __خروج__ صحيح، نبتلع الاستثناء
                            // (EN) If __exit__ returns true, suppress the exception
                            if (returnValue_.isBoolean() && returnValue_.toBool())
                            {
                                exceptionOccurred = false;
                                exceptionMessage.clear();
                            }
                            variableManager_.exitScope();
                        }
                        catch (const std::exception &exitErr)
                        {
                            // (AR) خطأ في __خروج__ نفسها — تسجيل تحذير ومتابعة
                            // (EN) Error in __exit__ itself — log warning and continue
                            std::cerr << "(AR) تحذير: خطأ في دالة __خروج__: " << exitErr.what()
                                      << " / (EN) Warning: error in __exit__: " << exitErr.what() << std::endl;
                            variableManager_.exitScope();
                        }
                        catch (...)
                        {
                            // (AR) خطأ غير معروف في __خروج__ — تسجيل تحذير
                            // (EN) Unknown error in __exit__ — log warning
                            std::cerr << "(AR) تحذير: خطأ غير معروف في دالة __خروج__ / "
                                      << "(EN) Warning: unknown error in __exit__" << std::endl;
                            variableManager_.exitScope();
                        }
                    }
                }
            }

            // (AR) خروج من نطاق with / (EN) Exit with scope
            variableManager_.exitScope();

            // (AR) إعادة رمي الاستثناء إذا لم يُعالج / (EN) Re-throw exception if not handled
            if (exceptionOccurred && !exceptionMessage.empty())
            {
                throw UserThrownException(exceptionMessage, "WithError");
            }
        }

        // =========================================================================
        // (AR) تنفيذ جملة التأجيل (أجّل) / (EN) Defer Statement Execution
        // =========================================================================

        void StatementExecutor::visitDeferStmt(AST::DeferStmt &node)
        {
            // (AR) تسجيل الجملة المؤجلة في مكدس التأجيل الحالي.
            //      لا نُنفّذ الجملة الآن — بل نُضيفها إلى المكدس.
            //      ستُنفَّذ عند خروج الدالة بترتيب LIFO (الأخير أولاً).
            // (EN) Register the deferred statement in the current defer stack.
            //      Do NOT execute it now — just push it onto the stack.
            //      It will execute on function exit in LIFO order.

            if (!deferStacks_.empty() && node.body)
            {
                deferStacks_.back().push_back(node.body.get());
            }
            // (AR) إذا لم يكن هناك مكدس تأجيل (خارج دالة)، نتجاهل — لكن هذا لا يحدث عمليًا
            // (EN) If no defer stack exists (outside function), ignore — shouldn't happen in practice
        }

        // =========================================================================
        // (AR) تنفيذ جملة أطلق (Goroutine) / (EN) Go Statement Implementation
        // =========================================================================

        void StatementExecutor::visitGoStmt(AST::GoStmt &node)
        {
            // (AR) ═══════════════════════════════════════════════════════════
            //      جملة «أطلق» — تُنشئ goroutine مستقل بسياق تنفيذ خاص
            // ═══════════════════════════════════════════════════════════════
            //
            // (AR) لتحقيق أمان الخيوط (Thread Safety)، نُنشئ لكل goroutine
            //      مجموعة مستقلة من المديرين:
            //        - ScopeManager خاص (مكدس نطاقات مستقل)
            //        - VariableManager خاص (تخزين متغيرات مستقل)
            //        - OwnershipManager خاص (تتبع ملكية مستقل)
            //        - StatementExecutor خاص (مع ExpressionEvaluator خاص)
            //
            //      نشارك فقط FunctionManager (للقراءة فقط — آمن للتزامن)
            //      والمتغيرات المرئية تُنسخ كـ snapshot (القنوات/الكائنات
            //      تُشارك عبر shared_ptr — وهو المطلوب للتواصل عبر القنوات).
            //
            // (EN) For Thread Safety, each goroutine gets its own independent
            //      set of managers (ScopeManager, VariableManager, OwnershipManager,
            //      StatementExecutor + ExpressionEvaluator). Only FunctionManager
            //      is shared (read-only, thread-safe). Visible variables are copied
            //      as a snapshot (channels/objects share via shared_ptr — required
            //      for inter-goroutine communication).
            // ═══════════════════════════════════════════════════════════════

            auto &goroutineMgr = GoroutineManager::getInstance();

            // (AR) التقاط لقطة من المتغيرات المرئية قبل الإطلاق
            // (EN) Capture snapshot of visible variables before launch
            auto capturedVars = variableManager_.captureVisibleVariables();

            // (AR) مرجع للـ FunctionManager — للقراءة فقط، آمن للتزامن
            // (EN) Reference to FunctionManager — read-only, thread-safe
            auto &funcMgr = functionManager_;

            if (node.expression)
            {
                // (AR) أطلق تعبير (استدعاء دالة أو لامدا)
                // (EN) Go with expression (function call or lambda)
                AST::Expression *exprPtr = node.expression.get();

                goroutineMgr.launch([capturedVars = std::move(capturedVars), &funcMgr, exprPtr]()
                                    {
                    try
                    {
                        // (AR) إنشاء مديرين مستقلين لهذا الـ goroutine
                        // (EN) Create independent managers for this goroutine
                        Data::ScopeManager localScopeMgr;
                        Data::VariableManager localVarMgr(localScopeMgr);
                        Data::OwnershipManager localOwnershipMgr;

                        // (AR) نسخ المتغيرات الملتقطة إلى النطاق العام الجديد
                        // (EN) Copy captured variables to the new global scope
                        for (const auto& pair : capturedVars)
                        {
                            localVarMgr.define(pair.first, pair.second);
                        }

                        // (AR) إنشاء executor مستقل — يُنشئ ExpressionEvaluator خاصاً
                        // (EN) Create independent executor — creates its own ExpressionEvaluator
                        StatementExecutor localExecutor(localVarMgr, funcMgr, localScopeMgr, localOwnershipMgr);

                        // (AR) تقييم التعبير في السياق المستقل
                        // (EN) Evaluate expression in the independent context
                        localExecutor.evaluateExpression(*exprPtr);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "خطأ في goroutine: " << e.what() << std::endl;
                    } });
            }
            else if (node.blockBody)
            {
                // (AR) أطلق كتلة (أطلق ... نهاية)
                // (EN) Go with block (أطلق ... نهاية)
                AST::Statement *blockPtr = node.blockBody.get();

                goroutineMgr.launch([capturedVars = std::move(capturedVars), &funcMgr, blockPtr]()
                                    {
                    try
                    {
                        // (AR) إنشاء مديرين مستقلين لهذا الـ goroutine
                        // (EN) Create independent managers for this goroutine
                        Data::ScopeManager localScopeMgr;
                        Data::VariableManager localVarMgr(localScopeMgr);
                        Data::OwnershipManager localOwnershipMgr;

                        // (AR) نسخ المتغيرات الملتقطة إلى النطاق العام الجديد
                        // (EN) Copy captured variables to the new global scope
                        for (const auto& pair : capturedVars)
                        {
                            localVarMgr.define(pair.first, pair.second);
                        }

                        // (AR) إنشاء executor مستقل
                        // (EN) Create independent executor
                        StatementExecutor localExecutor(localVarMgr, funcMgr, localScopeMgr, localOwnershipMgr);

                        // (AR) تنفيذ الكتلة في السياق المستقل
                        // (EN) Execute block in the independent context
                        blockPtr->accept(localExecutor);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "خطأ في goroutine: " << e.what() << std::endl;
                    } });
            }
        }

        // =========================================================================
        // (AR) تنفيذ حالة اختيار / (EN) Select Case Implementation
        // =========================================================================

        void StatementExecutor::visitSelectCase(AST::SelectCase &node)
        {
            // (AR) لا تُستدعى مباشرة — تُعالج من داخل visitSelectStmt
            // (EN) Not called directly — processed from within visitSelectStmt
            for (auto &stmt : node.body)
            {
                if (stmt)
                {
                    stmt->accept(*this);
                    if (flowControl_ != FlowControl::NONE)
                        return;
                }
            }
        }

        // =========================================================================
        // (AR) تنفيذ جملة اختر (Select) / (EN) Select Statement Implementation
        // =========================================================================

        void StatementExecutor::visitSelectStmt(AST::SelectStmt &node)
        {
            // (AR) جملة اختر تنتظر أول قناة جاهزة وتنفذ حالتها.
            //      الخوارزمية:
            //      1. نمر على كل حالة ونحاول tryReceive/trySend
            //      2. إذا نجحت أي حالة، ننفذ جسمها
            //      3. إذا فشلت كلها وتوجد حالة افتراضية، ننفذها
            //      4. إذا فشلت كلها ولا توجد افتراضية، نحجب حتى تتوفر قناة
            // (EN) Select waits for the first ready channel and executes its case.
            //      Algorithm:
            //      1. Try tryReceive/trySend on each case
            //      2. If any succeeds, execute its body
            //      3. If all fail and default exists, execute it
            //      4. If all fail and no default, block until a channel is ready

            const int MAX_POLL_ATTEMPTS = 1000; // (AR) عدد محاولات الاستطلاع
            const int POLL_DELAY_MS = 1;        // (AR) تأخير بين المحاولات

            bool hasDefault = !node.defaultBody.empty();

            for (int attempt = 0; attempt < MAX_POLL_ATTEMPTS; ++attempt)
            {
                // (AR) محاولة كل حالة
                // (EN) Try each case
                for (auto &selectCase : node.cases)
                {
                    if (!selectCase || !selectCase->channelExpr)
                        continue;

                    // (AR) نُقيّم تعبير القناة — يجب أن يكون استدعاء طريقة
                    //      مثل: ق.استقبل() أو ق.حاول_استقبل()
                    // (EN) Evaluate channel expression — should be method call
                    //      like: ch.receive() or ch.tryReceive()

                    try
                    {
                        // (AR) نحاول تقييم التعبير — إذا كان tryReceive يرجع قيمة
                        // (EN) Try evaluating expression — if tryReceive returns a value
                        Data::Value result = evaluateExpression(*selectCase->channelExpr);

                        // (AR) القناة جاهزة فقط إذا أعاد حاول_استقبل قيمةً فعليّة —
                        //      لا فراغ (Void) ولا عدم (Null). القناة الفارغة تُرجع
                        //      makeNull() (عدم)، فبدون استثناء عدم كانت select تلتقط
                        //      قناةً فارغة خطأً (تباعُد 059). يطابق مقارنة المترجم
                        //      «try_recv != null-sentinel».
                        // (EN) A channel is ready only if try_recv returned a real
                        //      value — neither Void nor Null. An empty channel yields
                        //      makeNull() (Null); without excluding Null, select wrongly
                        //      picked an empty channel (059 divergence). Matches the
                        //      compiler's «try_recv != null-sentinel» check.
                        if (result.getKind() != Types::SadTypeKind::Void &&
                            result.getKind() != Types::SadTypeKind::Null)
                        {
                            // (AR) ربط المتغير إذا وُجد
                            if (!selectCase->bindVar.empty())
                            {
                                variableManager_.defineOrAssign(selectCase->bindVar, result);
                            }
                            // (AR) تنفيذ جسم الحالة
                            for (auto &stmt : selectCase->body)
                            {
                                if (stmt)
                                {
                                    stmt->accept(*this);
                                    if (flowControl_ != FlowControl::NONE)
                                        return;
                                }
                            }
                            return; // (AR) خروج بعد تنفيذ الحالة الناجحة
                        }
                    }
                    catch (...)
                    {
                        // (AR) القناة غير جاهزة — نتابع للحالة التالية
                        // (EN) Channel not ready — continue to next case
                        continue;
                    }
                }

                // (AR) إذا لم تنجح أي حالة وتوجد حالة افتراضية
                // (EN) If no case succeeded and default exists
                if (hasDefault)
                {
                    for (auto &stmt : node.defaultBody)
                    {
                        if (stmt)
                        {
                            stmt->accept(*this);
                            if (flowControl_ != FlowControl::NONE)
                                return;
                        }
                    }
                    return;
                }

                // (AR) إذا لم تنجح أي حالة ولا توجد افتراضية — ننتظر قليلاً ونحاول مجدداً
                // (EN) If no case succeeded and no default — wait briefly and retry
                std::this_thread::sleep_for(std::chrono::milliseconds(POLL_DELAY_MS));
            }

            // (AR) تجاوز عدد المحاولات — خطأ
            // (EN) Exceeded poll attempts — error
            ::Sad::Errors::throwRuntime(::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                                        Sad::Lexer::Position{},
                                        {{"operation", "اختر / select"},
                                         {"reason", "تجاوزت الحد الأقصى للانتظار — جميع القنوات محجوبة / max wait exceeded — all channels blocked"}});
        }

    } // namespace Interpreter
} // namespace Sad
