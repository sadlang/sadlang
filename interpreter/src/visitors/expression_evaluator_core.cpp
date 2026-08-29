/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
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
#include "null_safety/null_safety_analyzer.h" // (AR) محور الصرامة D6 لحارس SEM045 / (EN) D6 strictness axis for the SEM045 guard
#include "visitors/sem045_report.h"           // (AR) باب إبلاغ SEM045 الواحد / (EN) single SEM045 reporting door
#include "sad_type_utils.h"                   // (AR) kindToArabic لرسالة SEM045 / (EN) kindToArabic for SEM045 message
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

        bool ExpressionEvaluator::containsArabic(const std::string &str)
        {
            // (AR) فحص إذا كان النص يحتوي أحرف عربية (UTF-8)
            // Arabic Unicode range: U+0600-U+06FF (UTF-8: 0xD8 0x80 - 0xDB 0xBF)
            // (EN) Properly validate 2-byte UTF-8 sequence: leading byte 0xD8-0xDB + continuation 0x80-0xBF
            const unsigned char *bytes = reinterpret_cast<const unsigned char *>(str.data());
            size_t len = str.size();
            for (size_t i = 0; i < len; ++i)
            {
                unsigned char b = bytes[i];
                // (AR) تخطي تسلسلات UTF-8 بـ 3 و 4 بايت لتجنب الإيجابيات الخاطئة
                if (b >= 0xF0 && i + 3 < len)
                {
                    i += 3;
                    continue;
                } // 4-byte seq
                if (b >= 0xE0 && i + 2 < len)
                {
                    i += 2;
                    continue;
                } // 3-byte seq
                // (AR) التحقق من تسلسل عربي مكون من 2 بايت
                if (b >= 0xD8 && b <= 0xDB && i + 1 < len)
                {
                    unsigned char next = bytes[i + 1];
                    if (next >= 0x80 && next <= 0xBF)
                        return true; // Arabic confirmed
                    i++;             // skip continuation byte
                }
            }
            return false;
        }

        const std::string &ExpressionEvaluator::internString(const std::string &str)
        {
            arabicOptStats_.totalStrings++;

            if (containsArabic(str))
            {
                arabicOptStats_.arabicStrings++;
            }

            auto [it, inserted] = stringPool_.insert(str);
            if (inserted)
            {
                // نص جديد - تم إضافته للمُجمّع
                arabicOptStats_.pooledStrings++;
            }
            else
            {
                // نص مكرر - تم توفير الذاكرة
                arabicOptStats_.poolHits++;
                arabicOptStats_.savedBytes += str.size();
            }

            return *it;
        }

        void ExpressionEvaluator::printArabicOptStats() const
        {
            std::cout << "\n========================================\n";
            std::cout << "إحصائيات التحسين العربي (المفسّر) / Arabic Optimization Statistics (Interpreter)\n";
            std::cout << "========================================\n\n";

            std::cout << "📝 تحسينات النصوص / String Optimizations:\n";
            std::cout << "  • مجموع النصوص / Total strings: " << arabicOptStats_.totalStrings << "\n";
            std::cout << "  • النصوص الفريدة / Unique strings: " << arabicOptStats_.pooledStrings << "\n";
            std::cout << "  • إصابات المُجمّع / Pool hits: " << arabicOptStats_.poolHits << "\n";
            std::cout << "  • النصوص العربية / Arabic strings: " << arabicOptStats_.arabicStrings << "\n";
            std::cout << "  • البايتات الموفرة / Saved bytes: " << arabicOptStats_.savedBytes << "\n";

            if (arabicOptStats_.totalStrings > 0)
            {
                double hitRate = (static_cast<double>(arabicOptStats_.poolHits) / arabicOptStats_.totalStrings) * 100.0;
                std::cout << "  • نسبة الإصابة / Hit rate: " << hitRate << "%\n";
            }

            std::cout << "========================================\n\n";
        }

        // =========================================================================
        // (AR) تقييم القيم الحرفية / (EN) Literal Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitLiteralExpr(LiteralExpr &node)
        {
            // (AR) خطأ/صحيح/لاشيء قد تكون أسماء متغيرات مُعرّفة (مثلاً في بنود امسك)
            // (EN) false/true/null keywords may be defined variable names (e.g., in catch clauses)
            auto tokenType = node.token.getType();
            if (tokenType == TokenType::LITERAL_FALSE ||
                tokenType == TokenType::LITERAL_TRUE ||
                tokenType == TokenType::LITERAL_NULL)
            {
                const Value *varPtr = variableManager_.tryGet(node.token.getValue());
                if (varPtr)
                {
                    lastResult_ = *varPtr;
                    return; // (AR) لا نُخزّن في الكاش — قيمة المتغير قد تتغير
                }
            }

            // (AR) استخدام ذاكرة التخزين المؤقت — يمنع إعادة تحليل stoll/stod في الحلقات
            // (EN) Use cache — prevents re-parsing stoll/stod in loops
            const void *key = static_cast<const void *>(&node);
            auto it = literalCache_.find(key);
            if (it != literalCache_.end())
            {
                lastResult_ = it->second;
                return;
            }
            Value val = tokenToValue(node.token);
            literalCache_[key] = val;
            lastResult_ = std::move(val);
        }

        Value ExpressionEvaluator::tokenToValue(const Token &token)
        {
            switch (token.getType())
            {
            case TokenType::NUMBER_INTEGER:
            {
                std::string value = token.getValue();

                // (AR) تحويل آمن من stoll إلى int64_t (كل الأعداد الصحيحة تبقى صحيحة)
                // (EN) Safe stoll→int64_t conversion (all integers stay as integers)
                auto safeToInt64 = [](long long val) -> Value
                {
                    return Value(static_cast<int64_t>(val));
                };

                try
                {
                    // Binary: 0b1010
                    if (value.size() > 2 && value[0] == '0' && (value[1] == 'b' || value[1] == 'B'))
                    {
                        // (AR) الأرقام الثنائية قد تكون كبيرة — نستخدم stoull ثم نحوّل
                        // (EN) Binary numbers may be large — use stoull then convert
                        unsigned long long parsed = std::stoull(value.substr(2), nullptr, 2);
                        return safeToInt64(static_cast<long long>(parsed));
                    }

                    // Octal: 0o17
                    if (value.size() > 2 && value[0] == '0' && (value[1] == 'o' || value[1] == 'O'))
                    {
                        unsigned long long parsed = std::stoull(value.substr(2), nullptr, 8);
                        return safeToInt64(static_cast<long long>(parsed));
                    }

                    // Hexadecimal: 0xFF
                    // (AR) نستخدم stoull لدعم القيم الكبيرة مثل 0xFFFFFFFF80000000 (عناوين النواة)
                    // (EN) Use stoull to support large values like 0xFFFFFFFF80000000 (kernel addresses)
                    if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
                    {
                        unsigned long long parsed = std::stoull(value.substr(2), nullptr, 16);
                        return safeToInt64(static_cast<long long>(parsed));
                    }

                    // Decimal: 42 — use stoull to support the full طبيعي range
                    // (AR) نستخدم stoull ليشمل [0، UINT64_MAX] (حرفيّات طبيعي فوق
                    //      INT64_MAX)؛ الحرفيّ دائمًا غير سالب (السالب عمليّة أحاديّة)،
                    //      ونحفظ نمط البتّات إلى int64 مطابقةً لمسار السِّتّ-عشريّ.
                    // (EN) Use stoull to cover [0, UINT64_MAX] (طبيعي literals above
                    //      INT64_MAX); the literal is always non-negative (negation is a
                    //      unary op), and we keep the bit pattern as int64 like the hex path.
                    {
                        unsigned long long parsed = std::stoull(value);
                        return safeToInt64(static_cast<long long>(parsed));
                    }
                }
                catch (const std::invalid_argument &)
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"detail", value}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        Sad::Errors::ErrorCode::SEM_INVALID_NUMBER_LITERAL,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(token.getPosition().line), static_cast<int>(token.getPosition().column)), _rc);
                    return Value(0);
                }
                catch (const std::out_of_range &)
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"detail", value}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        Sad::Errors::ErrorCode::SEM_INVALID_NUMBER_LITERAL,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(token.getPosition().line), static_cast<int>(token.getPosition().column)), _rc);
                    return Value(0);
                }
            }

            case TokenType::NUMBER_DOUBLE:
            {
                try
                {
                    return Value(std::stod(token.getValue()));
                }
                catch (const std::invalid_argument &)
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"detail", token.getValue()}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        Sad::Errors::ErrorCode::SEM_INVALID_NUMBER_LITERAL,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(token.getPosition().line), static_cast<int>(token.getPosition().column)), _rc);
                    return Value(0.0);
                }
                catch (const std::out_of_range &)
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"detail", token.getValue()}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        Sad::Errors::ErrorCode::SEM_INVALID_NUMBER_LITERAL,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(token.getPosition().line), static_cast<int>(token.getPosition().column)), _rc);
                    return Value(0.0);
                }
            }

            case TokenType::STRING_LITERAL:
                return Value(internString(token.getValue()));

            case TokenType::LITERAL_TRUE:
                return Value(true);

            case TokenType::LITERAL_FALSE:
                return Value(false);

            case TokenType::LITERAL_NULL:
                return Value::makeNull(); // (AR) عدم — متمايز عن فراغ (S-TS-P1)

            default:
            {
                // (AR) خرق ثابت داخلي: المُحلِّل سلّم رمزاً ليس حرفيّاً لمُقيِّم القيم الحرفية (ICE)
                // (EN) Internal invariant violation: parser handed a non-literal token to the literal evaluator (ICE)
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"detail", token.getValue()}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    Sad::Errors::ErrorCode::INT_INTERP_UNEXPECTED_TOKEN,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(token.getPosition().line), static_cast<int>(token.getPosition().column)), _rc);
                return Value(); // Return null
            }
            }
        }

        // =========================================================================
        // (AR) تقييم المتغيرات / (EN) Variable Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitVariableExpr(VariableExpr &node)
        {
            // (AR) بحث واحد بدلاً من exists() + get() — تحسين أداء مهم
            // (EN) Single lookup instead of exists() + get() — important performance optimization
            const Value *varPtr = variableManager_.tryGet(node.name);

            if (!varPtr)
            {
                // التحقق من وجود صنف بهذا الاسم (للوصول الثابت)
                // Check if class exists with this name (for static access)
                auto *classManager = Data::ClassManager::getInstance();
                ClassType *classType = classManager->getClass(node.name);

                if (classType)
                {
                    lastResult_ = Value(node.name);
                    return;
                }

                // (AR) التحقق من وجود دالة بهذا الاسم — دوال من الدرجة الأولى
                // (EN) Check if a function exists with this name — first-class functions
                if (functionManager_.hasFunction(node.name))
                {
                    // (AR) إنشاء مرجع دالة من نوع FUNCTION بدل STRING
                    // (EN) Create FUNCTION type reference instead of STRING
                    Data::FunctionRefKind kind = Data::FunctionRefKind::USER_DEFINED;
                    int arity = -1;
                    auto funcDef = functionManager_.getFunction(node.name, -1);
                    if (funcDef)
                    {
                        switch (funcDef->getType())
                        {
                        case Data::FunctionType::BUILT_IN:
                            kind = Data::FunctionRefKind::BUILT_IN;
                            break;
                        case Data::FunctionType::LAMBDA:
                            kind = Data::FunctionRefKind::LAMBDA;
                            break;
                        default:
                            kind = Data::FunctionRefKind::USER_DEFINED;
                            break;
                        }
                        arity = static_cast<int>(funcDef->getParameterCount());
                    }
                    auto funcRef = std::make_shared<Data::FunctionRef>(
                        node.name, node.name, kind, arity);
                    lastResult_ = Value(std::move(funcRef));
                    return;
                }

                // متغير غير معرّف — مع اقتراح "هل قصدت؟"
                // (EN) Undefined variable — with "Did you mean?" suggestion
                // (AR) بحث عن أسماء مشابهة للاقتراح "هل قصدت؟"
                auto availableNames = variableManager_.getVariableNames();
                Sad::Errors::SuggestionEngine sugEngine;
                auto similar = sugEngine.findSimilarSymbols(node.name, availableNames);
                std::string suggestion;
                if (!similar.empty())
                    suggestion = " — هل قصدت/did you mean: '" + similar[0] + "'؟";
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"name", node.name}, {"suggestion", suggestion}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                lastResult_ = Value(); // Return null
                return;
            }

            // (AR) فحص الملكية: هل المتغير صالح للاستخدام؟ / (EN) Ownership check: is variable valid?
            if (ownershipManager_.isEnabled())
            {
                auto error = ownershipManager_.useVariable(node.name);
                if (error.has_value())
                {
                    // (AR) المتغير منقول أو غير صالح — تمرير عبر dispatch()
                    // (EN) Variable moved or invalid — route through dispatch()
                    ::Sad::Errors::SourceLocation loc;
                    loc.line = node.position.line;
                    loc.column = node.position.column;

                    auto dispResult = ::Sad::Errors::dispatch(
                        error->kind,
                        statementExecutor_.getMemoryPolicy(),
                        loc, node.name);

                    if (dispResult.shouldStop())
                    {
                        throw UserThrownException(
                            dispResult.messageAr + " / " + dispResult.messageEn,
                            "OwnershipError",
                            node.position);
                    }
                    if (dispResult.shouldEmit())
                    {
                        std::cerr << "⚠ [ص-ملكية] " << dispResult.messageAr << "\n";
                        if (dispResult.teachingNote.has_value())
                        {
                            std::cerr << dispResult.teachingNote.value() << "\n";
                        }
                    }
                }
            }

            // (AR) استخدام المؤشر الذي حصلنا عليه من tryGet — بدون بحث ثانٍ
            // (EN) Use the pointer we got from tryGet — no second lookup
            lastResult_ = *varPtr;
        }

        void ExpressionEvaluator::visitBorrowExpr(BorrowExpr &node)
        {
            // (AR) التحقق من وجود المتغير / (EN) Check variable exists
            if (!variableManager_.exists(node.variableName))
            {
                {
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"name", node.variableName}, {"suggestion", ""}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    ::Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                }
                lastResult_ = Value();
                return;
            }

            // (AR) إنشاء استعارة في نظام الملكية / (EN) Create borrow in ownership system
            if (ownershipManager_.isEnabled())
            {
                auto borrowKind = node.isMutable
                                      ? Data::BorrowKind::Mutable
                                      : Data::BorrowKind::Shared;
                // (AR) اسم المستعير المؤقت / (EN) Temporary borrower name
                std::string borrowerName = "__borrow_" + node.variableName;
                auto error = ownershipManager_.createBorrow(node.variableName, borrowerName, borrowKind);
                if (error.has_value())
                {
                    // (AR) تمرير خطأ الاستعارة عبر dispatch()
                    // (EN) Route borrow error through dispatch()
                    ::Sad::Errors::SourceLocation loc;
                    loc.line = node.position.line;
                    loc.column = node.position.column;

                    auto dispResult = ::Sad::Errors::dispatch(
                        error->kind,
                        statementExecutor_.getMemoryPolicy(),
                        loc, node.variableName);

                    if (dispResult.shouldStop())
                    {
                        throw UserThrownException(
                            dispResult.messageAr + " / " + dispResult.messageEn,
                            "OwnershipError",
                            node.position);
                    }
                    if (dispResult.shouldEmit())
                    {
                        std::cerr << "⚠ [ص-ملكية] " << dispResult.messageAr << "\n";
                        if (dispResult.teachingNote.has_value())
                        {
                            std::cerr << dispResult.teachingNote.value() << "\n";
                        }
                    }
                }
            }

            // (AR) إرجاع قيمة المتغير (الاستعارة تُرجع نفس القيمة)
            // (EN) Return variable value (borrow returns same value)
            lastResult_ = variableManager_.get(node.variableName);
        }

        void ExpressionEvaluator::visitThisExpr(ThisExpr &node)
        {
            // الحصول على this من النطاق الحالي
            // Get 'this' from current scope
            if (variableManager_.exists("هذا"))
            {
                lastResult_ = variableManager_.get("هذا");
            }
            else if (variableManager_.exists("this"))
            {
                lastResult_ = variableManager_.get("this");
            }
            else
            {
                {
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"keyword", "هذا/this"}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    Sad::Errors::ErrorCode::SEM_THIS_SUPER_UNAVAILABLE,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                }
                lastResult_ = Value();
            }
        }

        void ExpressionEvaluator::visitSuperExpr(SuperExpr &node)
        {
            // (AR) الحصول على super من النطاق الحالي — نبحث عن الصنف الأب
            // (EN) Get super from current scope — look for base class
            if (variableManager_.exists("الأساس"))
            {
                lastResult_ = variableManager_.get("الأساس");
            }
            else if (variableManager_.exists("super"))
            {
                lastResult_ = variableManager_.get("super");
            }
            else if (variableManager_.exists("هذا"))
            {
                // (AR) نحاول بناء مرجع الأساس من الكائن الحالي
                // (EN) Try to build super reference from current object
                Value thisValue = variableManager_.get("هذا");
                auto *classManager = Data::ClassManager::getInstance();
                std::string className;
                if (thisValue.isObject())
                {
                    auto objPtr = thisValue.toObject();
                    if (objPtr)
                        className = objPtr->getClassName();
                }
                else if (thisValue.isMap())
                {
                    auto fields = thisValue.toMap();
                    auto it = fields.find("__class__");
                    if (it != fields.end())
                        className = it->second.toString();
                }
                if (!className.empty())
                {
                    Data::ClassType *cls = classManager->getClass(className);
                    if (cls && cls->getBaseClass())
                    {
                        // (AR) نرجع اسم الصنف الأب كقيمة نصية
                        // (EN) Return base class name as string value
                        lastResult_ = Value(cls->getBaseClass()->name);
                        return;
                    }
                }
                {
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"keyword", "الأساس/super"}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    Sad::Errors::ErrorCode::SEM_THIS_SUPER_UNAVAILABLE,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                }
                lastResult_ = Value();
            }
            else
            {
                {
                Sad::Errors::RenderContext _rc;
                _rc.placeholders = {{"keyword", "الأساس/super"}};
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    Sad::Errors::ErrorCode::SEM_THIS_SUPER_UNAVAILABLE,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                }
                lastResult_ = Value();
            }
        }

        // =========================================================================
        // (AR) تقييم الإسناد / (EN) Assignment Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitAssignExpr(AssignExpr &node)
        {
            // (AR) التحقق إذا كان الطرف الأيمن متغير (للنقل المحتمل)
            // (EN) Check if RHS is a variable (for potential move)
            std::string sourceVarName;
            auto *varExpr = dynamic_cast<VariableExpr *>(node.value.get());
            if (varExpr)
            {
                sourceVarName = varExpr->name;
            }

            // تقييم القيمة اليمنى / Evaluate right-hand side
            node.value->accept(*this);
            Value value = lastResult_;

            // (AR) التحقق إذا كان المتغير موجود / (EN) Check if variable exists
            // إذا لم يكن موجوداً، قم بتعريفه (استدلال النوع)
            // If not exists, define it (type inference)
            if (!variableManager_.exists(node.name))
            {
                // (AR) المتغير غير موجود — تعريف تلقائي مع تحذير
                // (EN) Variable doesn't exist — auto-define with warning
#ifdef DEBUG_INTERPRETER
                std::cerr << "[WARNING] متغير '"
                          << node.name
                          << "' غير مُعرّف مسبقاً، يتم تعريفه تلقائياً. / "
                          << "Variable '" << node.name << "' not previously declared, auto-defining.\n";
#endif
                variableManager_.define(node.name, value);

                // (AR) تسجيل في نظام الملكية / (EN) Register in ownership system
                if (ownershipManager_.isEnabled())
                {
                    ownershipManager_.declareVariable(node.name);

                    // (AR) نقل الملكية من المتغير المصدر (إن وُجد ولم يكن قابلاً للنسخ)
                    // (EN) Move ownership from source variable (if exists and not copy type)
                    if (!sourceVarName.empty() && sourceVarName != node.name)
                    {
                        auto moveError = ownershipManager_.moveVariable(sourceVarName);
                        if (moveError.has_value())
                        {
                            // (AR) تمرير الخطأ عبر dispatch() — السلوك يُحدَّد من سياسة الذاكرة
                            // (EN) Route error through dispatch() — behavior determined by memory policy
                            ::Sad::Errors::SourceLocation loc;
                            loc.filename = "";
                            loc.line = node.position.line;
                            loc.column = node.position.column;

                            auto dispResult = ::Sad::Errors::dispatch(
                                moveError->kind,
                                statementExecutor_.getMemoryPolicy(),
                                loc, sourceVarName);

                            if (dispResult.shouldStop())
                            {
                                throw UserThrownException(
                                    dispResult.messageAr + " / " + dispResult.messageEn,
                                    "OwnershipError",
                                    node.position);
                            }
                            if (dispResult.shouldEmit())
                            {
                                std::cerr << "⚠ [ص-ملكية] " << dispResult.messageAr << "\n";
                                if (dispResult.teachingNote.has_value())
                                {
                                    std::cerr << dispResult.teachingNote.value() << "\n";
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                // (AR) فحص الملكية: هل يمكن التعديل؟ / (EN) Ownership check: can mutate?
                if (ownershipManager_.isEnabled())
                {
                    auto error = ownershipManager_.mutateVariable(node.name);
                    if (error.has_value())
                    {
                        // (AR) تمرير خطأ التعديل عبر dispatch()
                        // (EN) Route mutation error through dispatch()
                        ::Sad::Errors::SourceLocation loc;
                        loc.line = node.position.line;
                        loc.column = node.position.column;

                        auto dispResult = ::Sad::Errors::dispatch(
                            error->kind,
                            statementExecutor_.getMemoryPolicy(),
                            loc, node.name);

                        if (dispResult.shouldStop())
                        {
                            throw UserThrownException(
                                dispResult.messageAr + " / " + dispResult.messageEn,
                                "OwnershipError",
                                node.position);
                        }
                        if (dispResult.shouldEmit())
                        {
                            std::cerr << "⚠ [ص-ملكية] " << dispResult.messageAr << "\n";
                            if (dispResult.teachingNote.has_value())
                            {
                                std::cerr << dispResult.teachingNote.value() << "\n";
                            }
                        }
                    }
                }

                // (AR) بايت (u8): يُقتطع 0–255 عند إعادة الإسناد أيضًا — لا التهيئة
                //      فقط (statement_executor.cpp:119). النوعُ الساكن المُصرَّح يُقرأ من
                //      getDeclaredType (مرآةُ resolveStaticType)، ونظيرُه في المترجم اقتطاعُ
                //      buildAssignment (`AND 0xFF`) حفظًا لتكافؤ المسارَين. getDeclaredType
                //      يُرجِع Integer محايدًا لغير المُصرَّح ⇒ لا أثر إلا على بايتٍ صريح.
                // (EN) Byte (u8): truncate to 0-255 on REASSIGNMENT too — not just init
                //      (statement_executor.cpp:119). The declared static type is read from
                //      getDeclaredType (mirroring resolveStaticType); its compiler peer is
                //      buildAssignment's `AND 0xFF`, keeping both tracks in parity.
                //      getDeclaredType returns neutral Integer for undeclared vars ⇒ affects
                //      only an explicitly-declared byte.
                const Types::SadTypeKind declaredKind =
                    variableManager_.getDeclaredType(node.name);
                if (Types::sadTypeKindIsIntegerNumeric(declaredKind) &&
                    value.getKind() == Types::SadTypeKind::Integer)
                {
                    // (AR) عمّمَ الاقتطاعُ على كلِّ عرضٍ مُعلَنٍ بدلَ «بايت» وحدَه.
                    //      و`getDeclaredType` تُرجِعُ Integer محايدًا لغيرِ المُصرَّح،
                    //      والمُطبِّعُ محايدٌ عندَ عرضِ ٦٤ — فلا أثرَ على غيرِ المُصرَّح.
                    // (EN) Generalized from Byte to every declared width; neutral for
                    //      undeclared vars (Integer) since normalize is identity at 64.
                    value = Data::Value(static_cast<int64_t>(
                        Types::sadTypeKindNormalizeInteger(declaredKind, value.toInt64())));
                }
                // (AR) والمعبَرُ الذي يقتطعُ هو نفسُه الذي يَسِمُ — نظيرُ التصريح.
                // (EN) The crossing that truncates is the crossing that tags.
                value.tagDeclaredWidth(declaredKind);

                // ═══════════════════════════════════════════════════════════
                // (AR) [SEM045 / RFC عقد الغياب — المرحلة أ] الفراغُ لا يعبر إلى
                //      خانةٍ مصنَّفة **عند إعادةِ الإسناد أيضًا** — الـRFC ينصّ:
                //      «يشمل الإسنادَ عند التصريح وإعادةَ الإسناد». نظيرُ التصريح:
                //      statement_executor.cpp (visitVarDeclStmt). hasDeclaredType
                //      واجبٌ هنا: getDeclaredType يُرجع Integer محايدًا لغيرِ
                //      المُصرَّح فيُفبرك حارسًا على خاناتٍ ديناميكية.
                // (EN) [SEM045 / absence-contract RFC, stage A] Void must not cross
                //      into a typed slot on REASSIGNMENT either. Declaration peer:
                //      visitVarDeclStmt. hasDeclaredType is mandatory: the neutral
                //      Integer fallback would fabricate a guard on dynamic slots.
                // ═══════════════════════════════════════════════════════════
                if (value.getKind() == Types::SadTypeKind::Void &&
                    variableManager_.hasDeclaredType(node.name))
                {
                    auto declaredKind = variableManager_.getDeclaredType(node.name);
                    // (AR) الإبلاغُ والمسنَدُ عبر البابِ الواحد (sem045_report).
                    //      توحيدُ المسنَدِ أضاف Null إلى طقمِ الاستثناءِ هنا —
                    //      لا خانةَ تصرَّح بكِيان Null (علامةُ «عدمي» ليست نوعًا)
                    //      فالسلوكُ المقيسُ لا يتغيّر.
                    // (EN) Reporting and predicate through the single door
                    //      (sem045_report). Unifying the predicate adds Null to
                    //      this site's exemption set — no slot declares kind Null
                    //      (the «عدمي» marker is not a kind), so measured
                    //      behavior is unchanged.
                    if (Sad::Interpreter::Sem045::kindIsGuarded(declaredKind))
                    {
                        Sad::Interpreter::Sem045::reportVoidCrossing(
                            node.name, Types::kindToArabic(declaredKind),
                            node.position,
                            Sad::NullSafety::strictnessFromOwnershipMode(
                                statementExecutor_.getMemoryPolicy().ownershipMode));
                    }
                }

                // إسناد للمتغير الموجود / Assign to existing variable
                variableManager_.assign(node.name, value);
            }

            // الإسناد يُرجع القيمة المُسندة / Assignment returns assigned value
            lastResult_ = value;
        }

        // =========================================================================
        // (AR) تقييم التعابير الثنائية / (EN) Binary Expression Evaluation
        // =========================================================================

    } // namespace Interpreter
} // namespace Sad
