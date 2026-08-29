/**
 * @file statement_executor.cpp
 * @brief (AR) تنفيذ منفّذ الجُمل
 * @brief (EN) Statement Executor Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
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
#include "channel.h"
#include "sad_type_system.h"
#include "null_safety/null_safety_analyzer.h" // (AR) محور الصرامة D6 لحارس SEM045 / (EN) D6 strictness axis for the SEM045 guard
#include "visitors/sem045_report.h"           // (AR) باب إبلاغ SEM045 الواحد / (EN) single SEM045 reporting door
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <iostream>
#include <sstream>
#include <map>

namespace Sad
{
    namespace Interpreter
    {

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

        // =========================================================================
        // (AR) البناء والإعداد / (EN) Construction and Setup
        // =========================================================================

        StatementExecutor::StatementExecutor(Data::VariableManager &varMgr,
                                             Data::FunctionManager &funcMgr,
                                             Data::ScopeManager &scopeMgr,
                                             Data::OwnershipManager &ownershipMgr)
            : variableManager_(varMgr), functionManager_(funcMgr), scopeManager_(scopeMgr), ownershipManager_(ownershipMgr), flowControl_(FlowControl::NONE), returnValue_(), yieldValue_(), inGenerator_(false), loopDepth_(0), currentFunctionReturnType_(Types::SadTypeKind::Unknown), currentFunctionName_("")
        {
            // (AR) إنشاء مُقيِّم التعابير / (EN) Create expression evaluator
            // Note: Pass *this to allow ExpressionEvaluator to call back for function execution
            expressionEvaluator_ = std::make_unique<ExpressionEvaluator>(varMgr, funcMgr, scopeMgr, *this, ownershipMgr);
        }

        // =========================================================================
        // (AR) دوال مساعدة خاصة / (EN) Private Helper Functions
        // =========================================================================

        Data::Value StatementExecutor::evaluateExpression(AST::Expression &expr)
        {
            expr.accept(*expressionEvaluator_);
            return expressionEvaluator_->getResult();
        }

        // =========================================================================
        // (AR) تنفيذ زيارة الجُمل الأساسية / (EN) Basic Statement Visitors
        // =========================================================================

        void StatementExecutor::visitExprStmt(AST::ExprStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) تنفيذ التعبير وتجاهل النتيجة / (EN) Execute expression and ignore result
            evaluateExpression(*node.expression);
        }

        void StatementExecutor::visitVarDeclStmt(AST::VarDeclStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) تقييم القيمة الابتدائية إن وُجدت / (EN) Evaluate initializer if present
            Data::Value value;

            if (node.initializer)
            {
                value = evaluateExpression(*node.initializer);

                // (AR) تحويل النوع إذا لزم الأمر / (EN) Type conversion if needed
                // (AR) 🔑 معبَرُ العشريِّ والصحيحِ يُسأَلُ عن **عائلةِ** النوعِ لا عن
                //      لفظِه الواحد: كان الشرطُ مكتوبًا على `Integer` و`Float`
                //      وحدَهما، فلمّا فُتحت الأعراضُ المُسمّاةُ («رقم8/16/32» و
                //      «طبيعي8/16/32» و«عشري32») لم يعرِفْها المعبَرُ فمرَّت القيمةُ
                //      كما هي: `عشري32 س = 5` تخزنُ صحيحًا فتطبعُ «5» بينما يطبعُ
                //      المترجِمُ «5.0»، و`رقم8 أ = 2.7` تخزنُ عشريًّا فتطبعُ «2.7»
                //      بينما يطبعُ المترجِمُ «2». والسؤالُ بالعائلةِ يشملُ الألفاظَ
                //      المفتوحةَ اليومَ وما يُفتَحُ غدًا لأنّ المحمولَ مولَّدٌ من
                //      types.yaml لا مكتوبٌ باليد.
                // (EN) The int/float crossing must ask the type's FAMILY, not one
                //      spelling. The test named Integer and Float alone, so the
                //      newly opened widths (رقم8/16/32, طبيعي8/16/32, عشري32) were
                //      not recognized and the value passed through unconverted —
                //      diverging from the compiler in both directions. The family
                //      predicates are generated from types.yaml, so this also covers
                //      widths opened later.
                if (Types::sadTypeKindIsIntegerNumeric(node.type) &&
                    Types::sadTypeKindIsFloatNumeric(value.getKind()))
                {
                    // (AR) تحويل عشري → رقم صحيح بقطعِ الكسرِ فقط، بلا تطبيعٍ على
                    //      العرضِ المُعلَن. وهذا **وصفُ الواقعِ المقيسِ لا إقرارٌ به**:
                    //      المترجِمُ يجيبُ `رقم8 أ = 300.7` بـ«300» لا بـ«44»، أي أنّ
                    //      معبَرَ العشريِّ يفلتُ عندَه من قاعدةِ البتر (ق٢). فلو طبَّعنا
                    //      هنا لَفتحنا تباعُدًا جديدًا بينَ المحرّكَينِ بدلَ أن نغلقَ
                    //      واحدًا. وإفلاتُ ق٢ في هذا المعبَرِ ثغرةٌ مقيسةٌ في المحرّكَينِ
                    //      معًا، تُسَدُّ في الطبقتَينِ معًا لا في واحدةٍ منهما.
                    // (EN) double → integer by truncating the fraction only, WITHOUT
                    //      normalizing to the declared width. This DESCRIBES the
                    //      measured behavior, it does not endorse it: the compiler
                    //      answers `رقم8 أ = 300.7` with 300, not 44 — the float
                    //      crossing escapes the truncation rule there too. Normalizing
                    //      here would open a new divergence instead of closing one.
                    //      The escape is a defect in BOTH engines and must be sealed
                    //      in both at once.
                    value = Data::Value(static_cast<int64_t>(value.toDouble()));
                }
                else if (Types::sadTypeKindIsFloatNumeric(node.type) &&
                         value.getKind() == Types::SadTypeKind::Integer)
                {
                    // (AR) تحويل رقم صحيح → عشري / (EN) Convert integer → double
                    value = Data::Value(static_cast<double>(value.toInt64()));
                }
                else if (Types::sadTypeKindIsIntegerNumeric(node.type) &&
                         value.getKind() == Types::SadTypeKind::Integer)
                {
                    // (AR) 🔑 الخزنُ في خانةٍ مُعلَنةِ العرضِ يُطبِّعُ القيمةَ على ذلك
                    //      العرض: بترٌ ثمّ توسيعٌ بالإشارةِ أو بالصفرِ حسبَ `numeric`
                    //      المُعلَنِ في types.yaml. وكان الاقتطاعُ مكتوبًا لـ«بايت»
                    //      وحدَه (`& 0xFF`)، فلمّا فُتحت «رقم8/16/32» و«طبيعي16/32»
                    //      دخلَتها قيمٌ لا تسعُها فتُخزَنُ كما هي — اسمُ نوعٍ صادقٌ
                    //      على خانةٍ عرضُها ٦٤. والمُطبِّعُ محايدٌ لِما عرضُه ٦٤
                    //      («رقم» و«طبيعي») فلا يتغيّرُ به سلوكٌ مقيس.
                    //      ⚠️ الاقتطاعُ هنا **عندَ الخزنِ لا عندَ الحساب**: نتيجةُ
                    //      `رقم8 + 1` تبقى «رقم» بقاعدةِ الهيمنة (الأعرضُ يفوز)،
                    //      وهي دلالةُ الترقيةِ المُعلَنة.
                    // (EN) Storing into a slot of declared width normalizes to that
                    //      width — truncate then sign/zero-extend per the declared
                    //      `numeric` class. Truncation was hand-written for Byte
                    //      alone; the newly opened widths stored out-of-range values
                    //      verbatim. Identity for 64-bit kinds, so nothing measured
                    //      moves. Applies at the STORE, not at the arithmetic.
                    value = Data::Value(static_cast<int64_t>(
                        Types::sadTypeKindNormalizeInteger(node.type, value.toInt64())));
                }

                // (AR) والمعبَرُ الذي يقتطعُ هو نفسُه الذي يَسِمُ: العرضُ المُعلَنُ
                //      معروفٌ هنا وحدَه، ولا يُستنتَجُ بعدَه من خانةِ التخزين.
                // (EN) The crossing that truncates is the crossing that tags.
                value.tagDeclaredWidth(node.type);

                // ═══════════════════════════════════════════════════════════
                // (AR) التحقق من توافقية الأنواع عبر النظام الموحد
                // (EN) Unified type system compatibility check
                // ═══════════════════════════════════════════════════════════
                if (node.sadType && node.type != Types::SadTypeKind::Unknown)
                {
                    // ═══════════════════════════════════════════════════════════
                    // (AR) [S-TS-P9 / NS-02] أمان null — **حارس زمنيّ فقط** (D10):
                    //      الحالة الحرفيّة `= لاشيء` انتقلت إلى المحلّل المشترك الساكن
                    //      (NullSafetyAnalyzer، نقطة الحقيقة الواحدة D8). يبقى هنا الحارس
                    //      الزمنيّ للتعابير **غير الحرفيّة** التي تُنتج عدمًا وقت التشغيل
                    //      (`= دالة_ترجع_عدم()`) — لا يستطيع المحلّل الساكن إثباتها.
                    //      شرط `!isNullLiteralInit` يمنع التحذير المزدوج للحالة الحرفيّة.
                    // (EN) [S-TS-P9 / NS-02] Null safety — **runtime guard only** (D10):
                    //      the literal `= null` case moved to the shared static analyzer
                    //      (single source of truth, D8). This guard now covers only
                    //      non-literal expressions that yield null at runtime; the
                    //      `!isNullLiteralInit` test avoids double-reporting the literal.
                    // ═══════════════════════════════════════════════════════════
                    bool isNullLiteralInit = false;
                    if (auto *litInit =
                            dynamic_cast<AST::LiteralExpr *>(node.initializer.get()))
                    {
                        isNullLiteralInit =
                            litInit->token.getType() == Lexer::TokenType::LITERAL_NULL;
                    }
                    if (value.isNull() && !isNullLiteralInit &&
                        node.type != Types::SadTypeKind::Optional &&
                        node.type != Types::SadTypeKind::Any &&
                        node.type != Types::SadTypeKind::Null &&
                        node.type != Types::SadTypeKind::Void)
                    {
                        std::string wAr =
                            "تحذير: إسناد 'لاشيء' (عدم) لمتغير '" + node.name +
                            "' من نوع غير اختياري '" + node.sadType->arabicName() +
                            "'. اجعله اختياريًّا: '" + node.sadType->arabicName() + "؟'";
                        std::string wEn =
                            "Assigning 'null' to non-optional variable '" + node.name +
                            "' of type '" + node.sadType->englishName() + "'. Make it optional: 'T?'";
                        std::cerr << "[تحذير نوع] سطر " << node.position.line
                                  << ": " << wAr << std::endl;
                        Sad::Errors::SourceLocation locN(
                            "", node.position.line, node.position.column);
                        Sad::Errors::ErrorManager::getInstance().reportWarning(
                            Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH, locN, wAr, wEn);
                    }

                    // ═══════════════════════════════════════════════════════════
                    // (AR) [SEM045 / RFC عقد الغياب — المرحلة أ] الفراغُ لا يعبر
                    //      إلى خانةٍ مصنَّفة. قِيس (٢٠٢٦-٠٨-٢١) أنّ «متغير نص اسم =
                    //      <فراغ>» كان يمضي بتحذيرٍ استشاريٍّ وexit=0 — جدارُ
                    //      الأنواعِ مؤجَّلٌ إلى موضعِ الاستعمال. الدرجةُ تتبع محورَ
                    //      الصرامةِ القائم (D6): إنتاج=إيقاف، تعلم=تحذير، جامع=صمت.
                    //      والاختياريّةُ (Optional) **ليست** مستثناة: نطاقُها
                    //      {T، عدم} ولا يسع فراغًا — لا بابَ خلفيًّا. الصرامةُ
                    //      تُقرأ من سياسةِ الذاكرة (افتراضيّها Warnings) لا من
                    //      OwnershipManager الذي افتراضيُّ عضوِه Strict ولا يُضبط
                    //      إلا عند تمريرِ علمٍ — وإلا صار «بلا أعلام» = إنتاج.
                    //      نظيرُ إعادةِ الإسناد: expression_evaluator_core.cpp
                    //      (visitAssignExpr) — نفسُ العقدِ في المسارَين.
                    // (EN) [SEM045 / absence-contract RFC, stage A] Void does not
                    //      cross into a typed slot. Enforcement follows the D6
                    //      strictness axis; optionals are NOT exempt. Strictness
                    //      is read from the memory policy (default Warnings), not
                    //      OwnershipManager whose member defaults to Strict.
                    //      Reassignment peer: visitAssignExpr.
                    // ═══════════════════════════════════════════════════════════
                    bool voidCrossingHandled = false;
                    if (value.getKind() == Types::SadTypeKind::Void &&
                        Sad::Interpreter::Sem045::kindIsGuarded(node.type))
                    {
                        // (AR) الإبلاغُ عبر البابِ الواحد (sem045_report) — كانت الكتلةُ
                        //      منسوخةً في أربعةِ مواضع. المسنَدُ kindIsGuarded يوحّد طقمَ
                        //      الاستثناءِ أيضًا (كان الشرطُ هنا Any فقط — الباقي كان
                        //      يستبعده الشرطُ الخارجيّ وSEM040).
                        // (EN) Report through the single door (sem045_report) — the block
                        //      was copied at four sites. kindIsGuarded also unifies the
                        //      exemption set (this site tested only Any — the rest were
                        //      excluded by the outer condition and SEM040).
                        voidCrossingHandled = true;
                        Sad::Interpreter::Sem045::reportVoidCrossing(
                            node.name, node.sadType->arabicName(), node.position,
                            Sad::NullSafety::strictnessFromOwnershipMode(
                                memoryPolicy_.ownershipMode));
                    }

                    auto valueType = Types::SadType::fromValueType(value.getType());
                    if (!voidCrossingHandled && valueType &&
                        !valueType->isAssignableTo(node.sadType.get()))
                    {
                        // (AR) تحذير: عدم توافق الأنواع
                        // (EN) Warning: type mismatch
                        std::string warnAr =
                            "تحذير: تعيين قيمة من نوع '" + valueType->arabicName() +
                            "' لمتغير '" + node.name + "' من نوع '" +
                            node.sadType->arabicName() + "'";
                        std::string warnEn =
                            "Type mismatch: assigning '" + valueType->englishName() +
                            "' to variable '" + node.name + "' of type '" +
                            node.sadType->englishName() + "'";

                        // (AR) طباعة التحذير مباشرة / (EN) Print warning directly
                        std::cerr << "[تحذير نوع] سطر " << node.position.line
                                  << ": " << warnAr << std::endl;

                        // (AR) تسجيل في نظام التشخيصات أيضاً
                        Sad::Errors::SourceLocation loc(
                            "", node.position.line, node.position.column);
                        Sad::Errors::ErrorManager::getInstance().reportWarning(
                            Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                            loc, warnAr, warnEn);
                    }
                }
            }
            else
            {
                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 من مصدرِ الحقيقةِ الواحد — وكان هذا الجدولُ الخامسَ
                // ════════════════════════════════════════════════════════════
                //
                // (AR) كان هنا نسخةٌ خامسةٌ من جدولِ القيمِ الافتراضيّةِ **بلا
                //      «بايت» ولا «طبيعي»**، فيسقطان في `default:` ويصيران
                //      «لاشيء» — نقضًا لقرارِ المالكِ الذي ينصّ عليهما صراحةً.
                //      قِيس (2026-08-15): «طبيعي س» و«بايت ب» داخل دالّةٍ
                //      يطبعان «لاشيء» في المفسّر، ومكدّسًا غيرَ مهيّأ في
                //      المترجّم — أي أنّ **المحرّكَين يخالفان الحكمَ بطريقتَين**.
                //      وأُضيف النوعان إلى ثلاثةِ جداولَ ونُسيا هنا وفي المترجّم.
                //
                //      🔑 والدرسُ يُدوَّن حيث وقع: جدولٌ واحدٌ منسوخٌ خمسَ مرّاتٍ
                //      لا يُصان بالانضباط. النداءُ الواحدُ لا ينجرف، والنسخُ
                //      ينجرف حتمًا — والسؤالُ متى لا هل.
                // (EN) This was the FIFTH copy of the default-value table, and it
                //      lacked Byte and UInt64, so both fell to «لاشيء» against the
                //      owner rule. One call cannot drift; five copies must.
                // ════════════════════════════════════════════════════════════
                value = Data::defaultValueForTypeKind(node.type);
            }

            // (AR) تعريف المتغير / (EN) Define variable
            try
            {
                if (node.isConst)
                {
                    variableManager_.defineConst(node.name, value);
                }
                else
                {
                    variableManager_.define(node.name, value);
                }

                // (AR) [طبقة طبيعي — الخطوة ١] سجّل النوع السطحيّ المُصرَّح بعد التعريف
                //      كي يقرأه resolveStaticType لاحقًا (طباعة/عمليّات لا-موقَّعة). محايد:
                //      لا يمسّ القيمة ولا المخرَج حتّى يُستهلَك في الخطوة ٤+.
                // (EN) [طبيعي layer — Step 1] Record the declared surface type after define
                //      so resolveStaticType can read it later (unsigned printing/ops). Neutral:
                //      touches neither the value nor the output until consumed in Step 4+.
                if (node.type != Types::SadTypeKind::Unknown)
                {
                    variableManager_.setDeclaredType(node.name, node.type);
                }
                else
                {
                    // (AR) SEM045: إعادةُ تصريحٍ غيرِ مصنَّفةٍ لاسمٍ كان مصنَّفًا تمحو
                    //      تصنيفَه — وإلّا حَكَم حارسُ إعادةِ الإسنادِ خانةً مجرّدةً
                    //      بنوعِ تصريحٍ بائدٍ (قِيس؛ المحلّلُ الساكنُ يصمت هنا بحقّ).
                    // (EN) SEM045: an untyped re-declaration clears stale typedness —
                    //      otherwise the reassignment guard judged a now-bare slot by
                    //      a dead declared type (measured; the static analyzer is
                    //      rightly silent here).
                    variableManager_.clearDeclaredType(node.name);
                }

                // (AR) خطاف مصحح الأداء — تتبع إنشاء المتغيرات
                // (EN) Profiler hook — track variable creation
                if (auto *prof = Sad::Tools::getGlobalProfiler())
                {
                    prof->onVariableCreated(node.name, "");
                }

                // (AR) تسجيل المتغير في نظام الملكية / (EN) Register variable in ownership system
                if (ownershipManager_.isEnabled())
                {
                    std::string typeName;
                    switch (node.type)
                    {
                    case Types::SadTypeKind::Integer:
                        typeName = "INTEGER";
                        break;
                    case Types::SadTypeKind::Float:
                        typeName = "FLOAT";
                        break;
                    case Types::SadTypeKind::String:
                        typeName = "نص";
                        break;
                    case Types::SadTypeKind::Boolean:
                        typeName = "BOOLEAN";
                        break;
                    case Types::SadTypeKind::Array:
                        typeName = "مصفوفة";
                        break;
                    case Types::SadTypeKind::Map:
                        typeName = "قاموس";
                        break;
                    case Types::SadTypeKind::Class:
                        typeName = "كائن";
                        break;
                    default:
                    {
                        // (AR) استدلال النوع من القيمة عندما يكون النوع غير محدد (متغير)
                        // (EN) Infer type from value when type is unspecified (متغير keyword)
                        auto vk = value.getKind();
                        if (vk == Types::SadTypeKind::Integer)
                            typeName = "INTEGER";
                        else if (vk == Types::SadTypeKind::Float)
                            typeName = "FLOAT";
                        else if (vk == Types::SadTypeKind::String)
                            typeName = "نص";
                        else if (vk == Types::SadTypeKind::Boolean)
                            typeName = "BOOLEAN";
                        else if (vk == Types::SadTypeKind::Array)
                            typeName = "مصفوفة";
                        else if (vk == Types::SadTypeKind::Map)
                            typeName = "قاموس";
                        else
                            typeName = "";
                        break;
                    }
                    }
                    ownershipManager_.declareVariable(node.name, typeName);

                    // (AR) نقل الملكية: إذا كان المُهيّئ متغيراً، انقل ملكيته
                    // (EN) Move semantics: if initializer is a variable, move ownership from it
                    if (node.initializer)
                    {
                        auto *varExpr = dynamic_cast<AST::VariableExpr *>(node.initializer.get());
                        if (varExpr && !varExpr->name.empty() && varExpr->name != node.name)
                        {
                            auto moveError = ownershipManager_.moveVariable(varExpr->name);
                            if (moveError.has_value())
                            {
                                // (AR) تمرير الخطأ عبر dispatch() — السلوك يُحدَّد من سياسة الذاكرة
                                //     Disabled → تجاهل، Warnings → تحذير، Strict/UltraStrict → استثناء
                                // (EN) Route error through dispatch() — behavior determined by memory policy
                                //     Disabled → ignore, Warnings → warn, Strict/UltraStrict → throw
                                ::Sad::Errors::SourceLocation loc;
                                loc.filename = currentFilePath_;
                                loc.line = node.position.line;
                                loc.column = node.position.column;

                                auto dispResult = ::Sad::Errors::dispatch(
                                    moveError->kind, memoryPolicy_, loc, varExpr->name);

                                if (dispResult.shouldStop())
                                {
                                    throw UserThrownException(
                                        dispResult.messageAr + " / " + dispResult.messageEn,
                                        "OwnershipError",
                                        node.position);
                                }
                                // (AR) في --learn: طباعة التحذير والمضي
                                // (EN) In --learn: print warning and continue
                                if (dispResult.shouldEmit())
                                {
                                    std::cerr << "⚠ [ص-ملكية] " << dispResult.messageAr << "\n";
                                    if (dispResult.teachingNote.has_value())
                                    {
                                        std::cerr << dispResult.teachingNote.value() << "\n";
                                    }
                                }
                                // (AR) في --gc (Ignore): لا شيء — استمر بدون أي رسالة
                                // (EN) In --gc (Ignore): nothing — continue without any message
                            }
                        }
                    }
                }
            }
            catch (const std::runtime_error &e)
            {
                // (AR) إضافة معلومات الموقع للخطأ / (EN) Add position info to error
                std::ostringstream oss;
                oss << e.what() << "\n"
                    << "📍 (AR) الموقع / (EN) Location: السطر / Line " << node.position.line
                    << "، العمود / Column " << node.position.column;
                throw UserThrownException(oss.str(), "RuntimeError", node.position);
            }
        }

        void StatementExecutor::visitBlockStmt(AST::BlockStmt &node)
        {
            // (AR) دخول نطاق جديد / (EN) Enter new scope
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            ownershipManager_.enterScope();

            // (AR) خطاف مصحح الأداء — تتبع النطاقات
            // (EN) Profiler hook — track scopes
            if (auto *prof = Sad::Tools::getGlobalProfiler())
            {
                prof->onScopeEnter("block", scopeManager_.getCurrentDepth());
            }

            // (AR) تنفيذ جميع الجمل في الكتلة / (EN) Execute all statements in block
            try
            {
                for (auto &stmt : node.statements)
                {
                    // (AR) خطاف التصحيح لكل جملة / (EN) Debug hook for each statement
                    debugHook(stmt->position);

                    stmt->accept(*this);

                    // (AR) إيقاف التنفيذ إذا كان هناك تحكم بالتدفق / (EN) Stop if flow control active
                    if (shouldStopExecution())
                    {
                        break;
                    }
                }
            }
            catch (...)
            {
                // (AR) ضمان تنظيف النطاق عند انتشار الاستثناء (مثل ارمي داخل كتلة إذا/بينما)
                // (EN) Ensure scope cleanup when exception propagates (e.g. throw inside if/while block)
                ownershipManager_.exitScope();
                variableManager_.exitScope();
                throw;
            }

            // (AR) الخروج من النطاق / (EN) Exit scope
            ownershipManager_.exitScope();
            variableManager_.exitScope();

            // (AR) خطاف مصحح الأداء — خروج من النطاق
            // (EN) Profiler hook — scope exit
            if (auto *prof = Sad::Tools::getGlobalProfiler())
            {
                prof->onScopeExit("block", scopeManager_.getCurrentDepth());
            }
        }

        // =========================================================================
        // (AR) تنفيذ جُمل التحكم بالتدفق / (EN) Control Flow Statement Visitors
        // =========================================================================

        void StatementExecutor::visitIfStmt(AST::IfStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) تقييم الشرط / (EN) Evaluate condition
            Data::Value condition = evaluateExpression(*node.condition);

            // (AR) تنفيذ الفرع المناسب — يدعم عامل منطقي() الضمني
            // (EN) Execute appropriate branch — supports implicit منطقي() operator
            if (expressionEvaluator_->convertObjectToBool(condition, node.position))
            {
                node.thenBranch->accept(*this);
            }
            else if (node.elseBranch)
            {
                node.elseBranch->accept(*this);
            }
        }

        void StatementExecutor::visitWhileStmt(AST::WhileStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
            loopDepth_++;

            // (AR) تنفيذ الحلقة / (EN) Execute loop
            while (true)
            {
                // (AR) تقييم الشرط — يدعم عامل منطقي() الضمني
                // (EN) Evaluate condition — supports implicit منطقي() operator
                Data::Value condition = evaluateExpression(*node.condition);

                if (!expressionEvaluator_->convertObjectToBool(condition, node.position))
                {
                    break;
                }

                // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                node.body->accept(*this);

                // (AR) معالجة break / (EN) Handle break
                if (flowControl_ == FlowControl::BREAK)
                {
                    flowControl_ = FlowControl::NONE;
                    break;
                }

                // (AR) معالجة continue / (EN) Handle continue
                if (flowControl_ == FlowControl::CONTINUE)
                {
                    flowControl_ = FlowControl::NONE;
                    continue;
                }

                // (AR) معالجة return (يُمرر للأعلى) / (EN) Handle return (pass to caller)
                if (flowControl_ == FlowControl::RETURN)
                {
                    break;
                }
            }

            // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
            loopDepth_--;
        }

        void StatementExecutor::visitForStmt(AST::ForStmt &node)
        {
            // (AR) دخول نطاق جديد للحلقة / (EN) Enter new scope for loop
            variableManager_.enterScope(Data::ScopeType::LOOP);

            // (AR) تنفيذ التهيئة / (EN) Execute initializer
            if (node.initializer)
            {
                node.initializer->accept(*this);
            }

            // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
            loopDepth_++;

            // (AR) تنفيذ الحلقة / (EN) Execute loop
            while (true)
            {
                // (AR) تقييم الشرط / (EN) Evaluate condition
                if (node.condition)
                {
                    Data::Value condition = evaluateExpression(*node.condition);
                    if (!condition.toBool())
                    {
                        break;
                    }
                }

                // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                node.body->accept(*this);

                // (AR) معالجة break / (EN) Handle break
                if (flowControl_ == FlowControl::BREAK)
                {
                    flowControl_ = FlowControl::NONE;
                    break;
                }

                // (AR) معالجة continue / (EN) Handle continue
                if (flowControl_ == FlowControl::CONTINUE)
                {
                    flowControl_ = FlowControl::NONE;
                }

                // (AR) معالجة return / (EN) Handle return
                if (flowControl_ == FlowControl::RETURN)
                {
                    break;
                }

                // (AR) تنفيذ الزيادة / (EN) Execute increment
                if (node.increment)
                {
                    evaluateExpression(*node.increment);
                }
            }

            // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
            loopDepth_--;

            // (AR) الخروج من نطاق الحلقة / (EN) Exit loop scope
            variableManager_.exitScope();
        }

        void StatementExecutor::visitForRangeStmt(AST::ForRangeStmt &node)
        {
            // (AR) تقييم المجموعة القابلة للتكرار / (EN) Evaluate iterable
            Data::Value iterable = evaluateExpression(*node.iterable);

            // (AR) دخول نطاق جديد / (EN) Enter new scope
            variableManager_.enterScope(Data::ScopeType::LOOP);

            // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
            loopDepth_++;

            // (AR) التكرار حسب نوع المجموعة / (EN) Iterate based on collection type
            if (iterable.isArray())
            {
                const auto &arr = iterable.toArrayRef();

                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: تعريف متغير الحلقة مرة واحدة قبل التكرار
                //      ثم تحديثه فقط داخل الحلقة — يُلغي بحث exists() المزدوج
                // (EN) Performance: define loop var once before loop,
                //      then assign inside — eliminates double exists() lookup
                // ═══════════════════════════════════════════════════════════
                if (!arr.empty())
                {
                    variableManager_.define(node.variable, arr[0]);
                    node.body->accept(*this);

                    if (flowControl_ == FlowControl::BREAK)
                    {
                        flowControl_ = FlowControl::NONE;
                    }
                    else if (flowControl_ == FlowControl::RETURN)
                    { /* handled below */
                    }
                    else
                    {
                        if (flowControl_ == FlowControl::CONTINUE)
                        {
                            flowControl_ = FlowControl::NONE;
                        }

                        for (size_t i = 1; i < arr.size(); i++)
                        {
                            variableManager_.assign(node.variable, arr[i]);

                            // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                            node.body->accept(*this);

                            // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
                            if (flowControl_ == FlowControl::BREAK)
                            {
                                flowControl_ = FlowControl::NONE;
                                break;
                            }
                            if (flowControl_ == FlowControl::CONTINUE)
                            {
                                flowControl_ = FlowControl::NONE;
                                continue;
                            }
                            if (flowControl_ == FlowControl::RETURN)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            else if (iterable.isMap())
            {
                const auto &map = iterable.toMap();

                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: تعريف قبل الحلقة ثم تحديث فقط
                // (EN) Performance: define before loop, then assign only
                // ═══════════════════════════════════════════════════════════
                bool firstIteration = true;
                for (const auto &[key, value] : map)
                {
                    if (firstIteration)
                    {
                        variableManager_.define(node.variable, Data::Value(key));
                        if (!node.valueVar.empty())
                        {
                            variableManager_.define(node.valueVar, value);
                        }
                        firstIteration = false;
                    }
                    else
                    {
                        variableManager_.assign(node.variable, Data::Value(key));
                        if (!node.valueVar.empty())
                        {
                            variableManager_.assign(node.valueVar, value);
                        }
                    }

                    // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                    node.body->accept(*this);

                    // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
                    if (flowControl_ == FlowControl::BREAK)
                    {
                        flowControl_ = FlowControl::NONE;
                        break;
                    }
                    if (flowControl_ == FlowControl::CONTINUE)
                    {
                        flowControl_ = FlowControl::NONE;
                        continue;
                    }
                    if (flowControl_ == FlowControl::RETURN)
                    {
                        break;
                    }
                }
            }
            else if (iterable.isString())
            {
                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: بناء مصفوفة الأحرف UTF-8 مرة واحدة
                //      ثم التكرار عبرها — يُلغي substr و exists() المزدوج
                // (EN) Performance: build UTF-8 char array once,
                //      then iterate — eliminates substr + double exists()
                // ═══════════════════════════════════════════════════════════
                const std::string &text = iterable.toString();

                // (AR) بناء مصفوفة الأحرف مسبقاً / (EN) Pre-build character array
                std::vector<std::string> chars;
                const unsigned char *bytes = reinterpret_cast<const unsigned char *>(text.data());
                size_t len = text.size();
                size_t pos = 0;
                chars.reserve(len); // (AR) حجز أقصى عدد ممكن / (EN) Reserve max possible count

                while (pos < len)
                {
                    size_t charLen = 1;
                    unsigned char b = bytes[pos];
                    if (b >= 0xF0)
                        charLen = 4;
                    else if (b >= 0xE0)
                        charLen = 3;
                    else if (b >= 0xC0)
                        charLen = 2;
                    if (pos + charLen > len)
                        charLen = len - pos;
                    chars.push_back(text.substr(pos, charLen));
                    pos += charLen;
                }

                // (AR) التكرار مع تعريف مرة واحدة / (EN) Iterate with define-once pattern
                if (!chars.empty())
                {
                    variableManager_.define(node.variable, Data::Value(chars[0]));
                    node.body->accept(*this);

                    if (flowControl_ == FlowControl::BREAK)
                    {
                        flowControl_ = FlowControl::NONE;
                    }
                    else if (flowControl_ == FlowControl::RETURN)
                    { /* handled below */
                    }
                    else
                    {
                        if (flowControl_ == FlowControl::CONTINUE)
                        {
                            flowControl_ = FlowControl::NONE;
                        }

                        for (size_t i = 1; i < chars.size(); i++)
                        {
                            variableManager_.assign(node.variable, Data::Value(std::move(chars[i])));

                            node.body->accept(*this);

                            if (flowControl_ == FlowControl::BREAK)
                            {
                                flowControl_ = FlowControl::NONE;
                                break;
                            }
                            if (flowControl_ == FlowControl::CONTINUE)
                            {
                                flowControl_ = FlowControl::NONE;
                                continue;
                            }
                            if (flowControl_ == FlowControl::RETURN)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            // ═══════════════════════════════════════════════════════════════
            // (AR) تكرار عبر قناة — لكل عنصر في قناة ... نهاية
            //      يستقبل من القناة حتى تُغلق وتفرغ
            // (EN) Iterate over channel — for each element in channel ... end
            //      Receives from channel until closed and empty
            // ═══════════════════════════════════════════════════════════════
            else if (iterable.isObject())
            {
                auto objPtr = iterable.toObject();
                bool isChannel = false;
                if (objPtr)
                {
                    auto classFieldIt = objPtr->fields.find("__class__");
                    isChannel = (classFieldIt != objPtr->fields.end() &&
                                 classFieldIt->second.toString() == "__\xD9\x82\xD9\x86\xD8\xA7\xD8\xA9__"); // __قناة__
                }

                if (isChannel)
                {
                    // (AR) الحصول على القناة من السجل / (EN) Get channel from registry
                    auto channelIt = objPtr->fields.find("__channel_id__");
                    if (channelIt == objPtr->fields.end())
                    {
                        {
                            Sad::Errors::RenderContext _rc;
                            _rc.placeholders = {{"operation", "قناة/channel"}, {"reason", "كائن قناة تالف / corrupt channel object"}};
                            Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                        };
                    }
                    else
                    {
                        size_t channelId = static_cast<size_t>(channelIt->second.toInt());
                        auto &registry = SadChannelRegistry::getInstance();
                        auto channel = registry.getChannel(channelId);

                        if (!channel)
                        {
                            {
                                Sad::Errors::RenderContext _rc;
                                _rc.placeholders = {{"operation", "قناة/channel"}, {"reason", "قناة غير موجودة / channel not found"}};
                                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                            };
                        }
                        else
                        {
                            // (AR) التكرار: نستقبل من القناة حتى تُرجع لاشيء (مغلقة وفارغة)
                            // (EN) Iterate: receive from channel until null (closed and empty)
                            bool firstIteration = true;
                            while (true)
                            {
                                Data::Value received = channel->receive();

                                // (AR) إذا القناة مغلقة وفارغة، receive تُرجع Value() (void)
                                // (EN) If channel closed and empty, receive returns Value() (void)
                                if (received.isVoid() && channel->isClosed())
                                {
                                    break;
                                }

                                if (firstIteration)
                                {
                                    variableManager_.define(node.variable, received);
                                    firstIteration = false;
                                }
                                else
                                {
                                    variableManager_.assign(node.variable, received);
                                }

                                // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                                node.body->accept(*this);

                                // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
                                if (flowControl_ == FlowControl::BREAK)
                                {
                                    flowControl_ = FlowControl::NONE;
                                    break;
                                }
                                if (flowControl_ == FlowControl::CONTINUE)
                                {
                                    flowControl_ = FlowControl::NONE;
                                    continue;
                                }
                                if (flowControl_ == FlowControl::RETURN)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    {
                        Sad::Errors::RenderContext _rc;
                        _rc.placeholders = {{"type", "غير معروف/unknown"}};
                        Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_NOT_ITERABLE, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                    };
                }
            }
            else
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"type", "غير معروف/unknown"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_NOT_ITERABLE, Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                };
            }

            // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
            loopDepth_--;

            // (AR) الخروج من النطاق / (EN) Exit scope
            variableManager_.exitScope();
        }

        // =========================================================================

    } // namespace Interpreter
} // namespace Sad
