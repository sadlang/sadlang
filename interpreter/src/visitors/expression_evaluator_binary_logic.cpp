/**
 * @file expression_evaluator_binary_logic.cpp
 * @brief (AR) تنفيذ عمليات المقارنة والمنطق والبت في ExpressionEvaluator
 * @brief (EN) Comparison, logical, and bitwise operations in ExpressionEvaluator
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
#include "async_runtime.h" // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "suggestions.h"   // (AR) نظام الاقتراحات الذكية / (EN) Smart suggestion engine
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
        Value ExpressionEvaluator::evaluateComparisonOp(const Value &left, TokenType op, const Value &right, const Lexer::Position &pos, bool unsignedOrdering)
        {
            // المقارنة تعمل على أي نوعين / Comparison works on any two types

            // (AR) معالجة خاصة لـ عدم (Null) — S-TS-P1: عدم قيمة وحيدة تساوي نفسها فقط.
            //      عدم == عدم ⇐ صحيح؛ عدم == أي قيمة أخرى ⇐ خطأ (متمايز عن فراغ وعن 0/"").
            // (EN) Special handling for Null — S-TS-P1: null is a unit value equal only to itself.
            //      null can be compared (==/!=) against any type; ordering is not allowed.
            if (left.isNull() || right.isNull())
            {
                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(left.isNull() && right.isNull());
                case TokenType::OP_NOT_EQUAL:
                    return Value(!(left.isNull() && right.isNull()));
                default:
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"type", "عدم/null"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_ONLY_EQUALITY_ALLOWED, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                };
                    return Value(false);
                }
            }

            // معالجة خاصة لـ null (VOID) / Special handling for null (VOID)
            // null يمكن مقارنته بأي نوع / null can be compared with any type
            if (left.isVoid() || right.isVoid())
            {
                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(left.isVoid() && right.isVoid());
                case TokenType::OP_NOT_EQUAL:
                    return Value(!left.isVoid() || !right.isVoid());
                default:
                    {
                        Sad::Errors::RenderContext _rc;
                        _rc.placeholders = {{"type", "null/فارغ"}};
                        Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_ONLY_EQUALITY_ALLOWED, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                    };
                    return Value(false);
                }
            }

            // (AR) مقارنة الأنواع الرقمية المختلطة (int و double) — ترقية كلاهما إلى double
            // (EN) Mixed numeric comparison (int and double) — promote both to double
            if (left.isNumeric() && right.isNumeric() && left.getKind() != right.getKind())
            {
                double l = left.toDouble();
                double r = right.toDouble();
                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(l == r);
                case TokenType::OP_NOT_EQUAL:
                    return Value(l != r);
                case TokenType::OP_LESS:
                    return Value(l < r);
                case TokenType::OP_LESS_EQUAL:
                    return Value(l <= r);
                case TokenType::OP_GREATER:
                    return Value(l > r);
                case TokenType::OP_GREATER_EQUAL:
                    return Value(l >= r);
                default:
                    break;
                }
            }

            // مقارنة الأنواع المختلفة / Different types comparison
            if (left.getKind() != right.getKind())
            {
                // فقط == و != مسموح بهما / Only == and != allowed
                if (op == TokenType::OP_EQUAL)
                    return Value(false);
                if (op == TokenType::OP_NOT_EQUAL)
                    return Value(true);
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_COMPARE_DIFFERENT_TYPES, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column));
                return Value(false); // Return default
            }

            // مقارنة الأعداد / Numeric comparison
            // (AR) صحيح×صحيح: مقارنة دقيقة 64-بت — المرور عبر double يفقد الدقّة فوق 2^53
            // (EN) Int×Int: exact 64-bit comparison — going through double loses precision above 2^53
            if (left.isInteger() && right.isInteger())
            {
                int64_t l = left.toInt64();
                int64_t r = right.toInt64();
                // (AR) [طبقة طبيعي64 — الخطوة ٥] ترتيب لا-موقَّع حين يكون كلا المعامِلين
                //      طبيعي64: نقارن نمط البتّات كـuint64 (يُطابق المترجم ICmpULT/UGT).
                //      المساواة/عدمها متطابقة على المستويين فتبقى على int64.
                // (EN) [طبيعي64 layer — Step 5] Unsigned ordering when both operands are طبيعي64:
                //      compare the bit pattern as uint64 (mirrors the compiler's ICmpULT/UGT).
                //      Equality/inequality is identical either way, so it stays on int64.
                if (unsignedOrdering)
                {
                    const uint64_t ul = static_cast<uint64_t>(l);
                    const uint64_t ur = static_cast<uint64_t>(r);
                    switch (op)
                    {
                    case TokenType::OP_EQUAL:
                        return Value(ul == ur);
                    case TokenType::OP_NOT_EQUAL:
                        return Value(ul != ur);
                    case TokenType::OP_LESS:
                        return Value(ul < ur);
                    case TokenType::OP_LESS_EQUAL:
                        return Value(ul <= ur);
                    case TokenType::OP_GREATER:
                        return Value(ul > ur);
                    case TokenType::OP_GREATER_EQUAL:
                        return Value(ul >= ur);
                    default:
                        break;
                    }
                }
                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(l == r);
                case TokenType::OP_NOT_EQUAL:
                    return Value(l != r);
                case TokenType::OP_LESS:
                    return Value(l < r);
                case TokenType::OP_LESS_EQUAL:
                    return Value(l <= r);
                case TokenType::OP_GREATER:
                    return Value(l > r);
                case TokenType::OP_GREATER_EQUAL:
                    return Value(l >= r);
                default:
                    break;
                }
            }
            if (left.isNumeric() && right.isNumeric())
            {
                double l = left.toDouble();
                double r = right.toDouble();

                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(l == r);
                case TokenType::OP_NOT_EQUAL:
                    return Value(l != r);
                case TokenType::OP_LESS:
                    return Value(l < r);
                case TokenType::OP_LESS_EQUAL:
                    return Value(l <= r);
                case TokenType::OP_GREATER:
                    return Value(l > r);
                case TokenType::OP_GREATER_EQUAL:
                    return Value(l >= r);
                default:
                    break;
                }
            }

            // مقارنة النصوص / String comparison
            // NOTE(#20): (AR) المقارنة تتم على مستوى البايتات بدون تطبيع Unicode (NFC/NFD).
            //            هذا يعني أن نفس الحرف العربي مع تشكيل مختلف قد لا يتطابق.
            // NOTE(#20): (EN) Comparison is byte-level without Unicode normalization (NFC/NFD).
            //            This means the same Arabic character with different diacritics encoding may not match.
            if (left.isString() && right.isString())
            {
                std::string l = left.toString();
                std::string r = right.toString();

                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(l == r);
                case TokenType::OP_NOT_EQUAL:
                    return Value(l != r);
                case TokenType::OP_LESS:
                    return Value(l < r);
                case TokenType::OP_LESS_EQUAL:
                    return Value(l <= r);
                case TokenType::OP_GREATER:
                    return Value(l > r);
                case TokenType::OP_GREATER_EQUAL:
                    return Value(l >= r);
                default:
                    break;
                }
            }

            // مقارنة Boolean / Boolean comparison
            if (left.isBoolean() && right.isBoolean())
            {
                bool l = left.toBool();
                bool r = right.toBool();

                switch (op)
                {
                case TokenType::OP_EQUAL:
                    return Value(l == r);
                case TokenType::OP_NOT_EQUAL:
                    return Value(l != r);
                default:
                    {
                        Sad::Errors::RenderContext _rc;
                        _rc.placeholders = {{"type", "منطقي/bool"}};
                        Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_ONLY_EQUALITY_ALLOWED, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                    };
                    return Value(false);
                }
            }

            // (AR) مقارنة عميقة للمصفوفات / (EN) Deep array comparison
            if (left.isArray() && right.isArray())
            {
                if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL)
                {
                    const auto &lArr = left.toArrayRef();
                    const auto &rArr = right.toArrayRef();
                    bool equal = (lArr.size() == rArr.size());
                    if (equal)
                    {
                        for (size_t i = 0; i < lArr.size(); ++i)
                        {
                            Value cmp = evaluateComparisonOp(lArr[i], TokenType::OP_EQUAL, rArr[i], pos);
                            if (!cmp.toBool())
                            {
                                equal = false;
                                break;
                            }
                        }
                    }
                    return Value(op == TokenType::OP_EQUAL ? equal : !equal);
                }
            }

            // (AR) مقارنة عميقة للخرائط / (EN) Deep map comparison
            if (left.isMap() && right.isMap())
            {
                if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL)
                {
                    const auto &lMap = left.toMap();
                    const auto &rMap = right.toMap();
                    bool equal = (lMap.size() == rMap.size());
                    if (equal)
                    {
                        for (const auto &[k, v] : lMap)
                        {
                            auto it = rMap.find(k);
                            if (it == rMap.end())
                            {
                                equal = false;
                                break;
                            }
                            Value cmp = evaluateComparisonOp(v, TokenType::OP_EQUAL, it->second, pos);
                            if (!cmp.toBool())
                            {
                                equal = false;
                                break;
                            }
                        }
                    }
                    return Value(op == TokenType::OP_EQUAL ? equal : !equal);
                }
            }

            // (AR) مقارنة مراجع الدوال / (EN) Function reference comparison
            if (left.isFunction() && right.isFunction())
            {
                if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL)
                {
                    Value eqResult = (left == right);
                    bool equal = eqResult.toBool();
                    return Value(op == TokenType::OP_EQUAL ? equal : !equal);
                }
            }

            // (AR) مقارنة الكائنات بالمرجع (مقارنة المؤشرات) / (EN) Object reference comparison (pointer comparison)
            // (AR) يدعم مقارنة كائنات من أي نوع بما في ذلك القنوات والكائنات الداخلية
            // (EN) Supports comparing objects of any type including channels and internal objects
            if (left.isObject() && right.isObject())
            {
                if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL)
                {
                    Value eqResult = (left == right);
                    bool equal = eqResult.toBool();
                    return Value(op == TokenType::OP_EQUAL ? equal : !equal);
                }
                // (AR) العمليات الترتيبية غير مدعومة للكائنات
                // (EN) Ordering operations not supported for objects
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"type", "كائن/object"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_ONLY_EQUALITY_ALLOWED, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                };
                return Value(false);
            }

            // (AR) مقارنة عميقة للصفوف / (EN) Deep tuple comparison
            // (AR) المقارنة الترتيبية (<، >، <=، >=) تعمل بأسلوب قاموسي (lexicographic)
            // (EN) Ordering comparisons use lexicographic order, like Python tuples
            if (left.isTuple() && right.isTuple())
            {
                const auto &lTuple = left.toTupleRef().elements;
                const auto &rTuple = right.toTupleRef().elements;

                if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL)
                {
                    bool equal = (lTuple.size() == rTuple.size());
                    if (equal)
                    {
                        for (size_t i = 0; i < lTuple.size(); ++i)
                        {
                            Value cmp = evaluateComparisonOp(lTuple[i], TokenType::OP_EQUAL, rTuple[i], pos);
                            if (!cmp.toBool())
                            {
                                equal = false;
                                break;
                            }
                        }
                    }
                    return Value(op == TokenType::OP_EQUAL ? equal : !equal);
                }

                // (AR) مقارنة ترتيبية قاموسية: نقارن عنصراً بعنصر
                // (EN) Lexicographic ordering: compare element by element
                if (op == TokenType::OP_LESS || op == TokenType::OP_LESS_EQUAL ||
                    op == TokenType::OP_GREATER || op == TokenType::OP_GREATER_EQUAL)
                {
                    size_t minLen = std::min(lTuple.size(), rTuple.size());
                    for (size_t i = 0; i < minLen; ++i)
                    {
                        // (AR) تحقق إن كان العنصر الأيسر أصغر
                        Value lessRes = evaluateComparisonOp(lTuple[i], TokenType::OP_LESS, rTuple[i], pos);
                        if (lessRes.toBool())
                        {
                            // lTuple[i] < rTuple[i] → الأيسر أصغر
                            return Value(op == TokenType::OP_LESS || op == TokenType::OP_LESS_EQUAL);
                        }
                        Value greaterRes = evaluateComparisonOp(rTuple[i], TokenType::OP_LESS, lTuple[i], pos);
                        if (greaterRes.toBool())
                        {
                            // lTuple[i] > rTuple[i] → الأيسر أكبر
                            return Value(op == TokenType::OP_GREATER || op == TokenType::OP_GREATER_EQUAL);
                        }
                        // (AR) العنصران متساويان — نكمل للعنصر التالي
                    }
                    // (AR) جميع العناصر المشتركة متساوية — نقارن بالطول
                    // (EN) All shared elements equal — compare by length
                    if (lTuple.size() < rTuple.size())
                        return Value(op == TokenType::OP_LESS || op == TokenType::OP_LESS_EQUAL);
                    if (lTuple.size() > rTuple.size())
                        return Value(op == TokenType::OP_GREATER || op == TokenType::OP_GREATER_EQUAL);
                    // (AR) نفس الطول ونفس العناصر — متساويان
                    return Value(op == TokenType::OP_LESS_EQUAL || op == TokenType::OP_GREATER_EQUAL);
                }
            }

            {
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"op", "مقارنة/comparison"}, {"type", "المُعطى/given"}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_INVALID_OPERATION, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
            };
            return Value(false);
        }

        // =========================================================================
        // (AR) العمليات المنطقية / (EN) Logical Operations
        // =========================================================================

        Value ExpressionEvaluator::evaluateLogicalOp(const Value &left, TokenType op, const Value &right, const Lexer::Position &pos)
        {
            // (AR) يدعم عامل منطقي() الضمني للكائنات
            bool l = convertObjectToBool(left, pos);
            bool r = convertObjectToBool(right, pos);

            switch (op)
            {
            case TokenType::OP_AND:
                return Value(l && r);
            case TokenType::OP_OR:
                return Value(l || r);
            default:
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"op", "منطقية/logical"}, {"type", "المُعطى/given"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_INVALID_OPERATION, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                };
                return Value(false);
            }
        }

        // =========================================================================
        // (AR) عمليات البت / (EN) Bitwise Operations
        // =========================================================================

        Value ExpressionEvaluator::evaluateBitwiseOp(const Value &left, TokenType op, const Value &right, const Lexer::Position &pos, bool unsignedShr)
        {
            if (!left.isNumeric() || !right.isNumeric())
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"op", "عمليات البت / bitwise"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_NUMERIC_REQUIRED, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                };
                return Value(0);
            }

            // (AR) العمليات البتّيّة بعرض 64-بت — مطابقةً لتمثيل «رقم» الداخليّ i64 في المحرّكين
            // (EN) Bitwise ops at 64-bit width — matching the i64 internal Integer representation
            int64_t l = left.toInt64();
            int64_t r = right.toInt64();

            // (AR) عرض النوع الصحيح بالبتّات — حدّ الإزاحة الآمنة في C++
            // (EN) Integer bit width — safe shift limit in C++
            constexpr int64_t kIntegerBitWidth = 64;

            switch (op)
            {
            case TokenType::OP_XOR:
                return Value(l ^ r);
            case TokenType::OP_BITWISE_AND:
                return Value(l & r);
            case TokenType::OP_BITWISE_OR:
                return Value(l | r);
            case TokenType::OP_SHIFT_LEFT:
            {
                // (AR) حماية من إزاحة بقيم خطيرة (UB في C++)
                // (EN) Protect against dangerous shift values (UB in C++)
                if (r < 0 || r >= kIntegerBitWidth)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_OFFSET_OUT_OF_RANGE,
                        pos,
                        {{"offset", std::to_string(r)}});
                }
                return Value(l << r);
            }
            case TokenType::OP_SHIFT_RIGHT:
            {
                if (r < 0 || r >= kIntegerBitWidth)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_OFFSET_OUT_OF_RANGE,
                        pos,
                        {{"offset", std::to_string(r)}});
                }
                // (AR) [الخطوة ٨] طبيعي64 ⇒ إزاحةٌ منطقيّة على uint64_t (تُدخِل أصفارًا) لتطابق
                //      CreateLShr في المترجم بدل `int64_t >>` الحسابيّة (تمدّ الإشارة). MAX>>1 =
                //      2^63-1 لا MAX. الافتراض (موقَّع) يبقى للأنواع الأخرى.
                // (EN) [Step 8] طبيعي64 ⇒ logical shift on uint64_t (shifts in zeros) to match the
                //      compiler's CreateLShr instead of arithmetic signed `int64_t >>` (sign-extends).
                //      MAX>>1 = 2^63-1 not MAX. Default (signed) stays for other types.
                if (unsignedShr)
                    return Value(static_cast<int64_t>(static_cast<uint64_t>(l) >> r));
                return Value(l >> r);
            }
            default:
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"op", "بت/bitwise"}, {"type", "المُعطى/given"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::SEM_INVALID_OPERATION, Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column), _rc);
                };
                return Value(0);
            }
        }

        // =========================================================================
        // (AR) تنفيذ عامل محمل زائداً / (EN) Operator Overload Execution
        // =========================================================================

    } // namespace Interpreter
} // namespace Sad
