/**
 * @file expression_evaluator_binary_ops.cpp
 * @brief (AR) تنفيذ التعبيرات الثنائية والعمليات المرتبطة في ExpressionEvaluator
 * @brief (EN) Binary expression operations in ExpressionEvaluator
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h" // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "directive_nodes.h"     // For SizeofExpr, AtomicExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
#include "async_runtime.h" // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "suggestions.h"   // (AR) نظام الاقتراحات الذكية / (EN) Smart suggestion engine
#include "builtin_registry.h" // (AR) لثوابت أسماء المدمَجات (أكبر/أصغر) / (EN) builtin name constants
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>
#include "sad_type_system.h"

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // =========================================================================
        // (AR) تحسين النصوص العربية / (EN) Arabic String Optimization
        // =========================================================================

        void ExpressionEvaluator::visitBinaryExpr(BinaryExpr &node)
        {
            // ═══════════════════════════════════════════════════════════════════
            // (AR) تقييم كسول للعمليات المنطقية && و || — لا يُقيّم الطرف الأيمن إلا عند الحاجة
            // (EN) Short-circuit evaluation for && and || — right-side evaluated only when needed
            // ═══════════════════════════════════════════════════════════════════
            if (node.op == TokenType::OP_AND)
            {
                node.left->accept(*this);
                Value left = std::move(lastResult_);
                if (!convertObjectToBool(left, node.position))
                {
                    lastResult_ = Value(false);
                    return;
                }
                node.right->accept(*this);
                lastResult_ = Value(convertObjectToBool(lastResult_, node.position));
                return;
            }
            if (node.op == TokenType::OP_OR)
            {
                node.left->accept(*this);
                Value left = lastResult_;
                if (convertObjectToBool(left, node.position))
                {
                    lastResult_ = Value(true);
                    return;
                }
                node.right->accept(*this);
                lastResult_ = Value(convertObjectToBool(lastResult_, node.position));
                return;
            }

            // (AR) تقييم الطرف الأيسر مع نقل القيمة بدلاً من نسخها
            // (EN) Evaluate left operand with move instead of copy
            node.left->accept(*this);
            Value left = std::move(lastResult_);

            // (AR) تقييم الطرف الأيمن مع نقل القيمة
            // (EN) Evaluate right operand with move
            node.right->accept(*this);
            Value right = std::move(lastResult_);

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص تحميل العامل الزائد على الكائنات — يدعم OBJECT و MAP
            // (EN) Check operator overloading on objects — supports OBJECT and MAP
            // ═══════════════════════════════════════════════════════════════════
            if (left.isObject() || left.isObjectLike())
            {
                std::string className = left.getClassName();
                if (!className.empty())
                {
                    auto *classManager = Data::ClassManager::getInstance();
                    Data::ClassType *classType = classManager->getClass(className);
                    if (classType)
                    {
                        // (AR) تحويل رمز العامل من TokenType إلى نص / (EN) Convert operator token to string
                        std::string opSymbol;
                        switch (node.op)
                        {
                        case TokenType::OP_PLUS:
                            opSymbol = "+";
                            break;
                        case TokenType::OP_MINUS:
                            opSymbol = "-";
                            break;
                        case TokenType::OP_MULTIPLY:
                            opSymbol = "*";
                            break;
                        case TokenType::OP_DIVIDE:
                            opSymbol = "/";
                            break;
                        case TokenType::OP_FLOOR_DIVIDE:
                            opSymbol = "//";
                            break;
                        case TokenType::OP_MODULO:
                            opSymbol = "%";
                            break;
                        case TokenType::OP_POWER:
                            opSymbol = "**";
                            break;
                        case TokenType::OP_EQUAL:
                            opSymbol = "==";
                            break;
                        case TokenType::OP_NOT_EQUAL:
                            opSymbol = "!=";
                            break;
                        case TokenType::OP_LESS:
                            opSymbol = "<";
                            break;
                        case TokenType::OP_LESS_EQUAL:
                            opSymbol = "<=";
                            break;
                        case TokenType::OP_GREATER:
                            opSymbol = ">";
                            break;
                        case TokenType::OP_GREATER_EQUAL:
                            opSymbol = ">=";
                            break;
                        // (AR) العوامل البتية / (EN) Bitwise operators
                        case TokenType::OP_XOR:
                            opSymbol = "^";
                            break;
                        case TokenType::OP_BITWISE_AND:
                            opSymbol = "&";
                            break;
                        case TokenType::OP_BITWISE_OR:
                            opSymbol = "|";
                            break;
                        case TokenType::OP_SHIFT_LEFT:
                            opSymbol = "<<";
                            break;
                        case TokenType::OP_SHIFT_RIGHT:
                            opSymbol = ">>";
                            break;
                        default:
                            break;
                        }

                        if (!opSymbol.empty())
                        {
                            Data::OperatorOverload *opOverload = classType->findOperator(opSymbol);
                            if (opOverload && opOverload->body)
                            {
                                // (AR) تنفيذ العامل المحمل زائداً / (EN) Execute operator overload
                                lastResult_ = executeOperatorOverload(left, *opOverload, right, node.position);
                                return;
                            }
                        }
                    }
                }
            }

            // تحديد نوع العملية / Determine operation type
            switch (node.op)
            {
            // (AR) عمليات حسابية / (EN) Arithmetic operations
            case TokenType::OP_PLUS:
            case TokenType::OP_MINUS:
            case TokenType::OP_MULTIPLY:
            case TokenType::OP_DIVIDE:
            case TokenType::OP_FLOOR_DIVIDE:
            case TokenType::OP_MODULO:
            case TokenType::OP_POWER:
            {
                // (AR) [طبقة طبيعي64 — الخطوة ٦] حسابٌ ملتفٌّ (+ − ×) حين يكون النوع
                //      السطحيّ للعمليّة طبيعي64. المترجم يلتفّ i64 دائمًا (CreateAdd/Sub/Mul
                //      بلا فحص طفح)، بينما المفسّر يرقّي طفح الموقَّع إلى double (safeAdd…).
                //      لطبيعي64 هذا انفراجٌ (٢^٦٣+٢^٦٣ ⇒ المترجم 0، المفسّر ‎-1.8e19‏). فحين
                //      يشارك **أيّ** معامل طبيعي64 (هيمنة، بخلاف المقارنة التي تلزم كليهما
                //      لأنّ إشارة CreateICmp تحتاجهما) نلتفّ بدل الترقية فيطابق المساران.
                //      Byte مستثنى (قيمه ٠–٢٥٥ لا تطفح i64 في + − ×؛ اقتطاعه خطوةٌ لاحقة).
                // (EN) [طبيعي64 layer — Step 6] Wrapping arithmetic (+ − ×) when the op's
                //      surface type is طبيعي64. The compiler always wraps i64 (CreateAdd/Sub/Mul
                //      with no overflow check), while the interpreter promotes signed overflow to
                //      double (safeAdd…). For طبيعي64 that diverges (2^63+2^63 ⇒ compiler 0,
                //      interpreter -1.8e19). So when **any** operand is طبيعي64 (dominance —
                //      unlike comparison which needs both, since CreateICmp's sign needs both) we
                //      wrap instead of promoting so both tracks agree. Byte is excluded (its 0–255
                //      values cannot overflow i64 in + − ×; its truncation is a later step).
                const bool wrapU64 =
                    resolveStaticType(node.left.get()) == Types::SadTypeKind::UInt64 ||
                    resolveStaticType(node.right.get()) == Types::SadTypeKind::UInt64;
                lastResult_ = evaluateArithmeticOp(left, node.op, right, node.position, wrapU64);
                break;
            }

            // (AR) عمليات مقارنة / (EN) Comparison operations
            case TokenType::OP_EQUAL:
            case TokenType::OP_NOT_EQUAL:
            case TokenType::OP_LESS:
            case TokenType::OP_LESS_EQUAL:
            case TokenType::OP_GREATER:
            case TokenType::OP_GREATER_EQUAL:
            {
                // (AR) [طبقة طبيعي64 — الخطوة ٥ · مُنقَّحة بقرارِ المالك 2026-08-16]
                //      مقارنةُ ترتيبٍ لا-موقَّعةٌ حين يكون **أيُّ** المعامِلَين طبيعي64
                //      (هيمنة، كالحساب) — لا حين يكونان كليهما.
                //
                //      🔴 والشرطُ كان `&&`، فانفرجت اللغةُ على نفسِها: `ط > ن` بين
                //      طبيعيَّين تُعطي «صحيح»، و`ط > 1` تُعطي **«خطأ»** لأكبرِ قيمةٍ
                //      لا-موقَّعة. والمحرّكان كانا متّفقَين على ذلك، فمرَّت بوّابةُ
                //      التكافؤِ ودُوِّنت في تقريرِ المصفوفة «مقارنةٌ غيرُ موجَّهة ✅»
                //      — 🔑 والتطابقُ بين المحرّكَين ليس برهانَ صواب، وإنّما ينفي
                //      الانفراجَ وحدَه.
                //
                //      وحكمُ المالك: تَهيمِن كما تَهيمِن في الحساب. والمساواةُ/عدمُها
                //      متطابقةٌ موقَّعةً ولا-موقَّعةً (مساواةُ بتّات) فلا تتأثّر.
                //      ⚠️ ولازمُه المُعلَن: `ط > -1` تصير «خطأ» — دلالةُ C، تُقال.
                // (EN) [طبيعي64 layer — Step 5, revised by owner ruling 2026-08-16]
                //      Unsigned ordering when EITHER operand is طبيعي64 (dominance, like
                //      arithmetic), not when both. The old `&&` made `ط > ن` true but
                //      `ط > 1` false; both engines agreed, so the equivalence gate passed
                //      it. Agreement between engines is not proof of correctness.
                const bool unsignedCmp =
                    resolveStaticType(node.left.get()) == Types::SadTypeKind::UInt64 ||
                    resolveStaticType(node.right.get()) == Types::SadTypeKind::UInt64;
                lastResult_ = evaluateComparisonOp(left, node.op, right, node.position, unsignedCmp);
                break;
            }

            // (AR) عمليات منطقية / (EN) Logical operations
            case TokenType::OP_AND:
            case TokenType::OP_OR:
                lastResult_ = evaluateLogicalOp(left, node.op, right, node.position);
                break;

            // (AR) عمليات البت / (EN) Bitwise operations
            case TokenType::OP_XOR:
            case TokenType::OP_BITWISE_AND:
            case TokenType::OP_BITWISE_OR:
            case TokenType::OP_SHIFT_LEFT:
            case TokenType::OP_SHIFT_RIGHT:
            {
                // (AR) [طبقة طبيعي64 — الخطوة ٨] إزاحةٌ يمنى منطقيّة (LShr) حين يكون النوع
                //      السطحيّ **للمعامل الأيسر** (القيمة المُزاحة) طبيعي64. المفسّر يستخدم
                //      `int64_t >> r` (حسابيّة، تحفظ الإشارة: MAX>>1=MAX) بينما طبيعي64 لا-موقَّع
                //      يلزمه المنطقيّة (MAX>>1=2^63-1). إشارةُ الإزاحة من المعامل الأيسر وحده
                //      (الأيمن عدّاد لا قيمة)، بخلاف هيمنة //،% . `<<` متطابقةٌ إشارةً فلا تتأثّر.
                // (EN) [طبيعي64 layer — Step 8] Logical right shift (LShr) when the LEFT operand's
                //      (the shifted value's) surface type is طبيعي64. The interpreter uses signed
                //      `int64_t >> r` (arithmetic, sign-preserving: MAX>>1=MAX) while طبيعي64 is
                //      unsigned and needs the logical shift (MAX>>1=2^63-1). The shift's signedness
                //      comes from the LEFT operand alone (the right is a count, not a value), unlike
                //      the //,% dominance. `<<` is signedness-identical so it is unaffected.
                const bool unsignedShr =
                    node.op == TokenType::OP_SHIFT_RIGHT &&
                    resolveStaticType(node.left.get()) == Types::SadTypeKind::UInt64;
                lastResult_ = evaluateBitwiseOp(left, node.op, right, node.position, unsignedShr);
                break;
            }

            // (AR) عامل العضوية: في / (EN) Membership operator: in
            case TokenType::KEYWORD_IN:
            {
                bool found = false;
                if (right.isArray())
                {
                    for (const auto &el : right.toArrayRef())
                    {
                        if ((left == el).toBool())
                        {
                            found = true;
                            break;
                        }
                    }
                }
                else if (right.isMap())
                {
                    std::string key = left.toString();
                    auto map = right.toMap();
                    found = (map.find(key) != map.end());
                }
                else if (right.isString())
                {
                    found = (right.toString().find(left.toString()) != std::string::npos);
                }
                else
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_IN_OPERATOR_RHS_INVALID,
                        node.position,
                        {{"type", right.getTypeName()}});
                }
                lastResult_ = Value(found);
                break;
            }

            default:
            {
                // (AR) حساب رمز العامل للرسالة المحسنة
                // (EN) Compute operator symbol for improved message
                std::string opStr = "?";
                switch (node.op)
                {
                case TokenType::OP_PLUS:
                    opStr = "+";
                    break;
                case TokenType::OP_MINUS:
                    opStr = "-";
                    break;
                case TokenType::OP_MULTIPLY:
                    opStr = "*";
                    break;
                case TokenType::OP_DIVIDE:
                    opStr = "/";
                    break;
                case TokenType::OP_FLOOR_DIVIDE:
                    opStr = "//";
                    break;
                case TokenType::OP_MODULO:
                    opStr = "%";
                    break;
                case TokenType::OP_POWER:
                    opStr = "**";
                    break;
                case TokenType::OP_XOR:
                    opStr = "^";
                    break;
                default:
                    break;
                }
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"op", opStr}, {"type", left.getTypeName() + " / " + right.getTypeName()}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        ::Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                        Sad::Errors::SourceLocation(
                            Sad::Errors::ErrorManager::getInstance().getSourceFilename().empty() ? "<input>" : Sad::Errors::ErrorManager::getInstance().getSourceFilename(),
                            static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                }
                lastResult_ = Value(); // Return null
            }
            }
        }

        // =========================================================================
        // (AR) العمليات الحسابية / (EN) Arithmetic Operations
        // =========================================================================

        Value ExpressionEvaluator::evaluateArithmeticOp(const Value &left, TokenType op, const Value &right, const Lexer::Position &pos, bool wrapU64)
        {
            // جمع النصوص (string concatenation) / String concatenation
            // (AR) يدعم عامل نص() الضمني للكائنات
            if (op == TokenType::OP_PLUS && (left.isString() || right.isString()))
            {
                return Value(convertObjectToString(left, pos) + convertObjectToString(right, pos));
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) عمليات المصفوفات: دمج (+) وتكرار (*)
            // (EN) Array operations: concatenation (+) and repetition (*)
            // ═══════════════════════════════════════════════════════════════════
            if (left.isArray() || right.isArray())
            {
                if (op == TokenType::OP_PLUS)
                {
                    // (AR) دمج المصفوفات: [1,2] + [3,4] → [1,2,3,4]
                    //      أو إضافة عنصر: [1,2] + 3 → [1,2,3]
                    return left + right;
                }
                if (op == TokenType::OP_MULTIPLY)
                {
                    // (AR) تكرار المصفوفة: [1,2] * 3 → [1,2,1,2,1,2]
                    return left * right;
                }
            }

            // (AR) تكرار النصوص: "ها" * 3 → "هاهاها"
            // (EN) String repetition: "ha" * 3 → "hahaha"
            if (op == TokenType::OP_MULTIPLY && (left.isString() || right.isString()))
            {
                return left * right;
            }

            // التأكد من أن الطرفين رقميين / Ensure both operands are numeric
            if (!left.isNumeric() || !right.isNumeric())
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"op", "حسابية / arithmetic"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_NUMERIC_REQUIRED, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                };
                return Value(0); // Return default
            }

            // تحويل لـ double إذا كان أحدهما double
            // Convert to double if either is double
            bool useDouble = left.isDouble() || right.isDouble();

            if (useDouble)
            {
                double l = left.toDouble();
                double r = right.toDouble();

                switch (op)
                {
                case TokenType::OP_PLUS:
                    return Value(l + r);
                case TokenType::OP_MINUS:
                    return Value(l - r);
                case TokenType::OP_MULTIPLY:
                    return Value(l * r);
                case TokenType::OP_DIVIDE:
                    if (r == 0.0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(l / r);
                case TokenType::OP_FLOOR_DIVIDE:
                    if (r == 0.0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_FLOOR_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(std::floor(l / r));
                case TokenType::OP_POWER:
                    return Value(std::pow(l, r));
                case TokenType::OP_MODULO:
                    if (r == 0.0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MODULO_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    return Value(std::fmod(l, r));
                default:
                    break;
                }
            }
            else
            {
                // (AR) الدقّة الكاملة 64-بت — كان toInt() (32-بت) يقتطع القيم الكبيرة
                //      ويباعد المفسّر عن المترجم (i64). النوع اللغويّ «رقم» يبقى كما هو؛
                //      هذا تصحيح للدقّة الداخليّة فقط.
                // (EN) Full 64-bit precision — toInt() (32-bit) used to truncate large
                //      values, diverging from the compiler (i64). Language type is
                //      unchanged; this only fixes internal precision.
                int64_t l = left.toInt64();
                int64_t r = right.toInt64();

                // (AR) حماية طفحان الأعداد الصحيحة — الترقية إلى double عند الطفحان.
                //      [الخطوة ٦] استثناء طبيعي64 (wrapU64): يلتفّ متمّمًا اثنينيًّا في
                //      uint64_t (سلوك معرَّف؛ التفاف int64_t الموقَّع سلوكٌ غير معرَّف) كي
                //      يطابق CreateAdd/Sub/Mul في المترجم بدل الترقية.
                // (EN) Integer overflow protection — promote to double on overflow.
                //      [Step 6] طبيعي64 exception (wrapU64): wrap two's-complement in uint64_t
                //      (defined; signed int64_t overflow is UB) to match the compiler's
                //      CreateAdd/Sub/Mul instead of promoting.
                auto safeAdd = [wrapU64](int64_t a, int64_t b) -> Value
                {
                    if (wrapU64)
                        return Value(static_cast<int64_t>(static_cast<uint64_t>(a) + static_cast<uint64_t>(b)));
                    if ((b > 0 && a > INT64_MAX - b) || (b < 0 && a < INT64_MIN - b))
                        return Value(static_cast<double>(a) + static_cast<double>(b));
                    return Value(a + b);
                };
                auto safeSub = [wrapU64](int64_t a, int64_t b) -> Value
                {
                    if (wrapU64)
                        return Value(static_cast<int64_t>(static_cast<uint64_t>(a) - static_cast<uint64_t>(b)));
                    if ((b < 0 && a > INT64_MAX + b) || (b > 0 && a < INT64_MIN + b))
                        return Value(static_cast<double>(a) - static_cast<double>(b));
                    return Value(a - b);
                };
                auto safeMul = [wrapU64](int64_t a, int64_t b) -> Value
                {
                    if (wrapU64)
                        return Value(static_cast<int64_t>(static_cast<uint64_t>(a) * static_cast<uint64_t>(b)));
                    if (a != 0 && b != 0)
                    {
                        if ((a > 0 && b > 0 && a > INT64_MAX / b) ||
                            (a < 0 && b < 0 && a < INT64_MAX / b) ||
                            (a > 0 && b < 0 && b < INT64_MIN / a) ||
                            (a < 0 && b > 0 && a < INT64_MIN / b))
                            return Value(static_cast<double>(a) * static_cast<double>(b));
                    }
                    return Value(a * b);
                };

                switch (op)
                {
                case TokenType::OP_PLUS:
                    return safeAdd(l, r);
                case TokenType::OP_MINUS:
                    return safeSub(l, r);
                case TokenType::OP_MULTIPLY:
                    return safeMul(l, r);
                case TokenType::OP_DIVIDE:
                    if (r == 0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    // (AR) [الخطوة ٧] النوع السطحيّ طبيعي64 ⇒ قسمة لا-موقَّعة، نظيرَ `//`:
                    //      لا سالب فالاقتطاع والأرضيّة سواء، ويطابق CreateUDiv في المترجم.
                    // (EN) [Step 7] طبيعي64 surface ⇒ unsigned division (mirrors `//`).
                    if (wrapU64)
                        return Value(static_cast<int64_t>(
                            static_cast<uint64_t>(l) / static_cast<uint64_t>(r)));
                    // (AR) 🔑 الحالةُ الوحيدةُ التي تفيض i64. في C سلوكٌ غيرُ معرَّف، وعلى x86
                    //      مصيدةُ #DE تقتل العمليّة. ولغةُ ص لا تخترع قيمةً بلا معنًى: ترمي
                    //      كما ترمي على القسمة على صفر بدل اللانهاية. وكانت تُرقّي إلى عشريّ
                    //      حين كانت `/` حقيقيّة — والترقيةُ تُعيد **نوعَ** النتيجة إلى زمنِ
                    //      التشغيل، وهو بالضبط ما يزيله هذا القرار.
                    // (EN) The only i64-overflowing division: UB in C, #DE on x86. Sad throws
                    //      rather than inventing a value, as it does for division by zero. It
                    //      used to promote to float — and promotion makes the result KIND a
                    //      runtime fact again, which is exactly what this change removes.
                    if (l == INT64_MIN && r == -1)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_NUMERIC_OVERFLOW, pos,
                            {{"a", std::to_string(l)}});
                    // (AR) 🔑 قسمةٌ صحيحةٌ باقتطاعٍ نحو الصفر — دلالةُ C: 7/2=3 و(-7)/2 = -3.
                    //      وكانت ترقّي إلى عشريّ عند وجودِ باقٍ (7/2=3.5). وتفترق عن الأرضيّة
                    //      `//` في السالبِ وحدَه: (-7) // 2 = -4 (نحو سالب اللانهاية).
                    // (EN) C-style truncating integer division: 7/2=3, -7/2=-3 (it used to
                    //      promote to float on a non-zero remainder). It differs from floor
                    //      `//` for negatives only: -7 // 2 = -4.
                    return Value(l / r);
                case TokenType::OP_FLOOR_DIVIDE:
                    if (r == 0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_FLOOR_DIVISION_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    // (AR) [الخطوة ٧] النوع السطحيّ طبيعي64 ⇒ قسمة أرضيّة لا-موقَّعة على uint64_t
                    //      (لا سالب فالأرضيّة = الاقتطاع) لتطابق CreateUDiv في المترجم بدل SDiv
                    //      الموقَّعة. المقسوم عليه ‎-1‏ يُعاد تفسيره MAX لا-موقَّعًا (MAX//MAX=…)،
                    //      ولا فيض حدّ أدنى (ذاك موقَّع). النطاق الكامل ٢^٦٤.
                    // (EN) [Step 7] طبيعي64 surface type ⇒ unsigned floor division on uint64_t
                    //      (no negatives so floor == truncation) to match the compiler's CreateUDiv
                    //      instead of signed SDiv. A -1 divisor is reinterpreted as unsigned MAX,
                    //      and there is no min-overflow (that is signed). Full 2^64 range.
                    if (wrapU64)
                        return Value(static_cast<int64_t>(
                            static_cast<uint64_t>(l) / static_cast<uint64_t>(r)));
                    if (l == INT64_MIN && r == -1)
                        return Value(-static_cast<double>(l));
                    // (AR) القسمة الصحيحة الأرضية: -7 // 2 → -4 (نحو سالب اللانهاية)
                    // (EN) Floor division: -7 // 2 → -4 (toward negative infinity)
                    {
                        int64_t q = l / r;
                        if ((l ^ r) < 0 && l % r != 0)
                            q -= 1;
                        return Value(q);
                    }
                case TokenType::OP_MODULO:
                    if (r == 0)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MODULO_BY_ZERO, pos,
                            {{"a", std::to_string(l)}});
                    // (AR) [الخطوة ٧] النوع السطحيّ طبيعي64 ⇒ باقٍ لا-موقَّع على uint64_t ليطابق
                    //      CreateURem في المترجم بدل SRem الموقَّعة. المقسوم عليه ‎-1‏ يُعاد تفسيره
                    //      MAX لا-موقَّعًا (لا حالة INT64_MIN%-1 الموقَّعة)، فـMAX%2 = 1 (لا ‎-1‏).
                    // (EN) [Step 7] طبيعي64 surface type ⇒ unsigned remainder on uint64_t to match
                    //      the compiler's CreateURem instead of signed SRem. A -1 divisor is
                    //      reinterpreted as unsigned MAX (no signed INT64_MIN%-1 case), so
                    //      MAX%2 = 1 (not -1).
                    if (wrapU64)
                        return Value(static_cast<int64_t>(
                            static_cast<uint64_t>(l) % static_cast<uint64_t>(r)));
                    // (AR) INT64_MIN % -1 سلوك غير محدَّد في C++ — النتيجة الرياضيّة صفر
                    // (EN) INT64_MIN % -1 is UB in C++ — mathematical result is zero
                    if (r == -1)
                        return Value(static_cast<int64_t>(0));
                    return Value(l % r);
                case TokenType::OP_POWER:
                {
                    double result = std::pow(static_cast<double>(l), static_cast<double>(r));
                    // (AR) حدود int64 كعشري: الحدّ الأعلى 2^63 حصراً لأنّ INT64_MAX لا يُمثَّل بدقّة
                    // (EN) int64 bounds as double: upper bound is 2^63 exclusive (INT64_MAX not exactly representable)
                    constexpr double kInt64LowerBoundD = -9223372036854775808.0; // -2^63
                    constexpr double kInt64UpperBoundD = 9223372036854775808.0;  // +2^63 (exclusive)
                    if (result >= kInt64LowerBoundD && result < kInt64UpperBoundD)
                        return Value(static_cast<int64_t>(result));
                    return Value(result);
                }
                default:
                    break;
                }
            }

            {
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"op", "حسابية/arithmetic"}, {"type", "المُعطى/given"}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_INVALID_OPERATION, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
            };
            return Value(0); // Return default
        }

        // =========================================================================
        // (AR) عمليات المقارنة / (EN) Comparison Operations
        // =========================================================================

        // =========================================================================
        // (AR) [طبقة طبيعي64 — الخطوة ٢] استنتاج النوع الساكن — مرآة انتشار SIR
        // (EN) [طبيعي64 layer — Step 2] Static type resolution — mirrors SIR propagation
        // =========================================================================
        Types::SadTypeKind ExpressionEvaluator::resolveStaticType(const AST::Expression *expr) const
        {
            // (AR) الافتراض المحايد Integer (موقَّع) — يُبقي السلوك القائم لأيّ عقدة غير مغطّاة.
            // (EN) Neutral default Integer (signed) — preserves existing behavior for uncovered nodes.
            if (!expr)
                return Types::SadTypeKind::Integer;

            // (AR) متغيّر → النوع السطحيّ المُصرَّح من البيئة (الخطوة ١).
            // (EN) Variable → declared surface type from the environment (Step 1).
            if (auto *var = dynamic_cast<const AST::VariableExpr *>(expr))
                return variableManager_.getDeclaredType(var->name);

            // (AR) نداء دالّة → نوع إرجاعها المُصرَّح. المترجم يمرّر نوع إرجاع الدالّة
            //      (من جدول الدوالّ) على سِجِلّ نتيجة النداء، فدالّةٌ تُرجِع طبيعي64
            //      تُقارَن لا-موقَّعةً هناك. نقرأ نفس المصدر — FunctionDecl::returnType
            //      من عقدة الـAST المحفوظة — فيتطابق المساران بالبناء. النداءات غير
            //      المباشرة (طريقة كائن/فهرسة) تبقى Integer موقَّعةً كما في المترجم.
            // (EN) Function call → its declared return type. The compiler stamps the
            //      function's return type (from its function table) onto the call's
            //      result register, so a طبيعي64-returning function compares unsigned
            //      there. We read the SAME source — FunctionDecl::returnType from the
            //      stored AST node — so both tracks agree by construction. Indirect
            //      calls (method/index callee) stay signed Integer, matching the compiler.
            if (auto *call = dynamic_cast<const AST::CallExpr *>(expr))
            {
                if (auto *callee = dynamic_cast<const AST::VariableExpr *>(call->callee.get()))
                {
                    // (AR) أكبر/أصغر المدمَجتان: نوعُ النتيجة يطابق الخلفيّةَ الأصليّة
                    //      (builtins_math.cpp): عائمٌ إن كان أحدُ الوسيطين عائمًا،
                    //      وطبيعي64 إن كانا معًا طبيعي64، وإلّا Integer موقَّع. هكذا
                    //      تُطبَع نتيجةُ أكبر(طبيعي64، طبيعي64) لا-موقَّعةً كما في
                    //      المترجم — فيتّحد المساران. [[التوحيد الكامل]]
                    // (EN) أكبر/أصغر builtins: result type mirrors the native backend
                    //      (builtins_math.cpp): Float if either arg is Float, UInt64 if
                    //      both args are UInt64, else signed Integer — so أكبر of two
                    //      UInt64 prints unsigned exactly as the compiler does.
                    if ((callee->name == Builtins::Names::Math::MAX ||
                         callee->name == Builtins::Names::Math::MIN) &&
                        call->arguments.size() == 2)
                    {
                        const Types::SadTypeKind a = resolveStaticType(call->arguments[0].get());
                        const Types::SadTypeKind b = resolveStaticType(call->arguments[1].get());
                        if (a == Types::SadTypeKind::Float || b == Types::SadTypeKind::Float)
                            return Types::SadTypeKind::Float;
                        if (a == Types::SadTypeKind::UInt64 && b == Types::SadTypeKind::UInt64)
                            return Types::SadTypeKind::UInt64;
                        return Types::SadTypeKind::Integer;
                    }
                    if (auto fn = functionManager_.getFunction(callee->name, call->arguments.size()))
                    {
                        if (auto *decl = dynamic_cast<AST::FunctionDecl *>(fn->getFunctionDecl().get()))
                            return decl->returnType;
                    }
                }
                return Types::SadTypeKind::Integer;
            }

            // (AR) حرفيّ → نوع رمزه؛ الصحيح موقَّع افتراضًا (اللا-موقَّعيّة تأتي من نوع الهدف).
            // (EN) Literal → its token kind; integers signed by default (unsigned-ness from target).
            if (auto *lit = dynamic_cast<const AST::LiteralExpr *>(expr))
            {
                switch (lit->token.getType())
                {
                case Lexer::TokenType::NUMBER_DOUBLE:
                    return Types::SadTypeKind::Float;
                default:
                    return Types::SadTypeKind::Integer;
                }
            }

            // (AR) ثنائيّ → هيمنة قانونيّة تُطبَّق **متطابقةً** في المترجم لاحقًا:
            //      Float ثمّ UInt64 ثمّ Byte ثمّ Integer. Float يهيمن لأنّ خلط صحيح
            //      (موقَّع أو لا) بعائم يُرقّى إلى عائم (`طبيعي64 + عائم` ⇒ عائم).
            //      (المقارنات تُنتج منطقيًّا وقت التشغيل؛ لا يُطبَّق التنسيق اللا-موقَّع إلا
            //      على قيمة صحيحة، فلا ضرر من إرجاع نوع المعامِلين هنا.)
            // (EN) Binary → canonical dominance to be applied **identically** in the compiler
            //      later: Float, then UInt64, then Byte, then Integer. Float dominates because
            //      mixing any integer (signed or not) with a float promotes to float
            //      (`طبيعي64 + عائم` ⇒ float). (Comparisons yield Boolean at runtime; unsigned
            //      formatting applies only to integer values, so the operand type is harmless.)
            if (auto *bin = dynamic_cast<const AST::BinaryExpr *>(expr))
            {
                const Types::SadTypeKind l = resolveStaticType(bin->left.get());
                const Types::SadTypeKind r = resolveStaticType(bin->right.get());
                if (l == Types::SadTypeKind::Float || r == Types::SadTypeKind::Float)
                    return Types::SadTypeKind::Float;
                if (l == Types::SadTypeKind::UInt64 || r == Types::SadTypeKind::UInt64)
                    return Types::SadTypeKind::UInt64;
                if (l == Types::SadTypeKind::Byte || r == Types::SadTypeKind::Byte)
                    return Types::SadTypeKind::Byte;
                return Types::SadTypeKind::Integer;
            }

            return Types::SadTypeKind::Integer;
        }

    } // namespace Interpreter
} // namespace Sad
