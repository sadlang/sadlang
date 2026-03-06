/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"  // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include "async_runtime.h"  // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "suggestions.h"    // (AR) نظام الاقتراحات الذكية / (EN) Smart suggestion engine
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

namespace Sad {
namespace Interpreter {

using namespace Data;
using namespace AST;
using namespace Lexer;

// (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
// (EN) Helper function to get filename from error manager
static inline std::string getSourceFilename() {
    const auto& fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
    return fn.empty() ? "<input>" : fn;
}


// =========================================================================
// (AR) تقييم التعابير الأحادية / (EN) Unary Expression Evaluation
// =========================================================================

void ExpressionEvaluator::visitUnaryExpr(UnaryExpr& node) {
    // تقييم المعامل / Evaluate operand
    node.operand->accept(*this);
    Value operand = lastResult_;
    
    switch (node.op) {
        case TokenType::OP_MINUS:  // -x
            if (operand.isInteger()) {
                lastResult_ = Value(-operand.toInt());
            } else if (operand.isDouble()) {
                lastResult_ = Value(-operand.toDouble());
            } else {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "السالب يتطلب قيمة رقمية",
                    "Negation requires numeric value"
                );
                lastResult_ = Value(0);
            }
            break;
        
        case TokenType::OP_NOT:    // !flag
            lastResult_ = Value(!operand.toBool());
            break;
        
        case TokenType::OP_PLUS:   // +x (no-op)
            if (!operand.isNumeric()) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "الموجب يتطلب قيمة رقمية",
                    "Positive requires numeric value"
                );
                lastResult_ = Value(0);
                break;
            }
            lastResult_ = operand;
            break;
        
        case TokenType::OP_INCREMENT: {  // ++x
            // (AR) الزيادة: ++x يزيد القيمة بـ 1 ويُرجع القيمة الجديدة
            // (EN) Increment: ++x adds 1 and returns new value
            Value newVal;
            if (operand.isInteger()) {
                newVal = Value(operand.toInt() + 1);
            } else if (operand.isDouble()) {
                newVal = Value(operand.toDouble() + 1.0);
            } else {
                throw Interpreter::SadException(
                    "(AR) العامل ++ يتطلب قيمة رقمية، ولكن القيمة من نوع '" + operand.getTypeName() + "'. "
                    "(EN) ++ requires numeric value, but got type '" + operand.getTypeName() + "'.",
                    "TypeError", node.position);
            }
            // (AR) تحديث المتغير إذا كان المعامل متغيراً
            // (EN) Update variable if operand is variable
            if (auto* varExpr = dynamic_cast<VariableExpr*>(node.operand.get())) {
                variableManager_.assign(varExpr->name, newVal);
            }
            lastResult_ = newVal;
            break;
        }
        
        case TokenType::OP_DECREMENT: {  // --x
            // (AR) النقصان: --x يُنقص القيمة بـ 1 ويُرجع القيمة الجديدة
            // (EN) Decrement: --x subtracts 1 and returns new value
            Value newVal;
            if (operand.isInteger()) {
                newVal = Value(operand.toInt() - 1);
            } else if (operand.isDouble()) {
                newVal = Value(operand.toDouble() - 1.0);
            } else {
                throw Interpreter::SadException(
                    "(AR) العامل -- يتطلب قيمة رقمية، ولكن القيمة من نوع '" + operand.getTypeName() + "'. "
                    "(EN) -- requires numeric value, but got type '" + operand.getTypeName() + "'.",
                    "TypeError", node.position);
            }
            // (AR) تحديث المتغير إذا كان المعامل متغيراً
            // (EN) Update variable if operand is variable
            if (auto* varExpr = dynamic_cast<VariableExpr*>(node.operand.get())) {
                variableManager_.assign(varExpr->name, newVal);
            }
            lastResult_ = newVal;
            break;
        }
        
        case TokenType::OP_BITWISE_NOT: {  // ~x
            // (AR) القلب البتّي: ~x يقلب جميع البتات
            // (EN) Bitwise NOT: ~x flips all bits
            if (operand.isInteger()) {
                lastResult_ = Value(~operand.toInt());
            } else {
                throw Interpreter::SadException(
                    "(AR) ~ يتطلب قيمة صحيحة / (EN) ~ requires integer value",
                    "TypeError", node.position);
            }
            break;
        }
        
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "عملية أحادية غير مدعومة",
                "Unsupported unary operation"
            );
            lastResult_ = Value(0);
    }
}

// =========================================================================
// (AR) تقييم التعبير الثلاثي الشرطي / (EN) Ternary Expression Evaluation
// =========================================================================

/**
 * @brief (AR) يُقيّم التعبير الثلاثي الشرطي: شرط ? صحيح : خطأ
 *        (EN) Evaluates ternary conditional expression: condition ? true : false
 * 
 * Evaluates the condition, then returns either true_expression or false_expression
 * based on the condition result.
 * 
 * يُقيّم الشرط، ثم يُرجع إما تعبير_صحيح أو تعبير_خطأ بناءً على نتيجة الشرط.
 * 
 * @example
 * x > 0 ? "positive" : "negative"
 * age >= 18 ? "adult" : "minor"
 * العمر >= 18 ؟ "بالغ" : "قاصر"
 */
void ExpressionEvaluator::visitTernaryExpr(TernaryExpr& node) {
    // Evaluate condition / تقييم الشرط
    node.condition->accept(*this);
    Value condition = lastResult_;
    
    // Based on condition, evaluate either true or false branch
    // بناءً على الشرط، قيّم إما الفرع الصحيح أو الخاطئ
    if (condition.toBool()) {
        // Condition is true, evaluate true branch
        // الشرط صحيح، قيّم الفرع الصحيح
        node.trueExpr->accept(*this);
    } else {
        // Condition is false, evaluate false branch
        // الشرط خاطئ، قيّم الفرع الخاطئ
        node.falseExpr->accept(*this);
    }
    
    // lastResult_ already contains the result from the evaluated branch
    // lastResult_ يحتوي بالفعل على النتيجة من الفرع المُقيّم
}

// =========================================================================
// (AR) تقييم المصفوفات / (EN) Array Evaluation
// =========================================================================

void ExpressionEvaluator::visitArrayExpr(ArrayExpr& node) {
    Value::ArrayType arr;
    
    // تقييم كل عنصر / Evaluate each element
    for (auto& elem : node.elements) {
        elem->accept(*this);
        arr.push_back(lastResult_);
    }
    
    lastResult_ = Value(arr);
}

// =========================================================================
// (AR) تقييم القواميس / (EN) Map Evaluation
// =========================================================================

void ExpressionEvaluator::visitMapExpr(MapExpr& node) {
    Value::MapType map;
    
    // تقييم كل زوج مفتاح-قيمة / Evaluate each key-value pair
    for (auto& pair : node.pairs) {
        // (AR) التحقق من spread / (EN) Check for spread
        if (pair.isSpread()) {
            // (AR) spread: دمج الخريطة المُنتشرة مع الخريطة الحالية
            // (EN) spread: merge the spread map into current map
            pair.value->accept(*this);
            Value spreadVal = lastResult_;
            if (spreadVal.isMap()) {
                for (const auto& [k, v] : spreadVal.toMapRef()) {
                    map[k] = v;
                }
            }
            continue;
        }
        
        // تقييم المفتاح / Evaluate key
        pair.key->accept(*this);
        std::string key = lastResult_.toString();
        
        // تقييم القيمة / Evaluate value
        pair.value->accept(*this);
        Value value = lastResult_;
        
        map[key] = value;
    }
    
    lastResult_ = Value(map);
}

// =========================================================================
// (AR) تقييم الفهرسة / (EN) Index Evaluation
// =========================================================================

void ExpressionEvaluator::visitIndexExpr(IndexExpr& node) {
    // تقييم الكائن المفهرس / Evaluate indexed object
    node.object->accept(*this);
    Value obj = lastResult_;
    
    // تقييم الفهرس / Evaluate index
    node.index->accept(*this);
    Value index = lastResult_;
    
    if (obj.isArray()) {
        // فهرسة مصفوفة / Array indexing
        // (AR) قبول الأعداد العشرية التي تمثل أعداداً صحيحة (مثل 4.0) أو اقتطاعها (مثل 4.5 → 4)
        // (EN) Accept doubles that represent whole numbers (e.g. 4.0) or truncate them (e.g. 4.5 → 4)
        if (!index.isNumeric()) {
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "فهرس المصفوفة يجب أن يكون رقماً",
                "Array index must be a number"
            );
            lastResult_ = Value();
            return;
        }
        
        int idx = index.isInteger() ? index.toInt() : static_cast<int>(index.toDouble());
        const Value::ArrayType& arr = obj.toArrayRef();
        
        // (AR) دعم الفهرسة السالبة على نمط بايثون: -1 = آخر عنصر
        // (EN) Support Python-style negative indexing: -1 = last element
        if (idx < 0) idx = static_cast<int>(arr.size()) + idx;
        
        if (idx < 0 || idx >= static_cast<int>(arr.size())) {
            int sz = static_cast<int>(arr.size());
            throw IndexOutOfRangeError(
                "(AR) الفهرس " + std::to_string(idx) + " خارج النطاق. الفهارس الصالحة: 0 إلى " + std::to_string(sz - 1) + " (أو -" + std::to_string(sz) + " إلى -1). الحجم: " + std::to_string(sz) + " / " +
                "(EN) Index " + std::to_string(idx) + " out of range. Valid indices: 0 to " + std::to_string(sz - 1) + " (or -" + std::to_string(sz) + " to -1). Size: " + std::to_string(sz),
                node.position
            );
        }
        
        lastResult_ = arr[idx];
        
    } else if (obj.isMap()) {
        // فهرسة قاموس / Map indexing
        // (AR) في حالة عدم وجود المفتاح، نُرجع عدم (null) بدلاً من خطأ
        // (EN) If key doesn't exist, return null instead of throwing error
        std::string key = index.toString();
        const Value::MapType& map = obj.toMapRef();
        
        auto it = map.find(key);
        if (it == map.end()) {
            lastResult_ = Value();  // null / عدم
            return;
        }
        
        lastResult_ = it->second;
        
    } else if (obj.isString()) {
        // (AR) فهرسة نص بأحرف UTF-8
        // (EN) String indexing by UTF-8 characters
        if (!index.isInteger()) {
            throw RuntimeError(
                "(AR) فهرس النص يجب أن يكون رقم صحيح. (EN) String index must be integer.",
                node.position
            );
        }
        std::string str = obj.toString();
        // (AR) تحويل النص إلى قائمة أحرف UTF-8
        std::vector<std::string> chars;
        for (size_t i = 0; i < str.size(); ) {
            unsigned char c = static_cast<unsigned char>(str[i]);
            size_t charLen = 1;
            if (c >= 0xF0) charLen = 4;
            else if (c >= 0xE0) charLen = 3;
            else if (c >= 0xC0) charLen = 2;
            if (i + charLen > str.size()) charLen = 1;
            chars.push_back(str.substr(i, charLen));
            i += charLen;
        }
        int idx = index.toInt();
        if (idx < 0) idx = static_cast<int>(chars.size()) + idx;
        if (idx < 0 || idx >= static_cast<int>(chars.size())) {
            throw RuntimeError(
                "(AR) فهرس النص " + std::to_string(idx) + " خارج النطاق (الطول: " + std::to_string(chars.size()) + "). " +
                "(EN) String index " + std::to_string(idx) + " out of range (length: " + std::to_string(chars.size()) + ").",
                node.position
            );
        }
        lastResult_ = Value(chars[idx]);

    } else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الفهرسة تعمل فقط على المصفوفات والقواميس والنصوص",
            "Indexing works only on arrays, maps, and strings"
        );
        lastResult_ = Value();
    }
}

// =========================================================================
// (AR) تقييم استدعاء الدالة / (EN) Function Call Evaluation
// =========================================================================

void ExpressionEvaluator::visitCallExpr(CallExpr& node) {
    // (AR) فحص عمق الاستدعاء لمنع الاستدعاء التكراري اللانهائي
    // (EN) Check call depth to prevent infinite recursion
    if (++currentCallDepth_ > maxCallDepth_) {
        --currentCallDepth_;
        throw Interpreter::RuntimeError(
            "(AR) خطأ: تجاوز الحد الأقصى لعمق الاستدعاء (" + std::to_string(maxCallDepth_) + 
            "). ربما يوجد استدعاء تكراري لا نهائي.\n"
            "(EN) Error: Maximum call depth exceeded (" + std::to_string(maxCallDepth_) + 
            "). Possible infinite recursion.",
            node.position
        );
    }
    
    // (AR) حارس RAII لتقليل العدّاد عند الخروج
    // (EN) RAII guard to decrement counter on exit
    struct CallDepthGuard {
        size_t& depth;
        CallDepthGuard(size_t& d) : depth(d) {}
        ~CallDepthGuard() { --depth; }
    } guard(currentCallDepth_);
    
    // (AR) الحصول على اسم الدالة / (EN) Get function name
    std::string funcName;
    bool isTemplateInstantiation = false;
    
    // (AR) التحقق - هل callee هو TemplateInstantiation؟
    // (EN) Check - is callee a TemplateInstantiation?
    auto templateInst = dynamic_cast<TemplateInstantiation*>(node.callee.get());
    if (templateInst) {
        // (AR) هذا استدعاء دالة قالب!
        // (EN) This is a template function call!
        isTemplateInstantiation = true;
        funcName = "__template_" + templateInst->templateName;
        
        #ifdef DEBUG
        std::cout << "[Template] استدعاء دالة قالب: " << templateInst->templateName << "<";
        for (size_t i = 0; i < templateInst->typeArguments.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << static_cast<int>(templateInst->typeArguments[i]);
        }
        std::cout << ">" << std::endl;
        #endif
    } else {
        // (AR) التحقق - هل callee هو VariableExpr (اسم دالة) أم شيء آخر؟
        // (EN) Check - is callee a VariableExpr (function name) or something else?
        auto calleeVar = dynamic_cast<VariableExpr*>(node.callee.get());
        if (calleeVar) {
            // (AR) نتحقق أولاً - هل المتغير موجود؟ / (EN) Check first - does variable exist?
            if (variableManager_.exists(calleeVar->name)) {
                // (AR) قد يكون lambda أو اسم دالة مخزّن في متغير
                // (EN) May be lambda or function name stored in variable
                Value varValue = variableManager_.get(calleeVar->name);
                if (varValue.isString()) {
                    std::string strVal = varValue.toString();
                    if (strVal.find("__lambda_") == 0) {
                        // (AR) هذا lambda! / (EN) This is lambda!
                        funcName = strVal;
                    } else if (functionManager_.hasFunction(strVal)) {
                        // (AR) اسم دالة مخزّن في متغير (دالة من الدرجة الأولى)
                        // (EN) Function name stored in variable (first-class function)
                        funcName = strVal;
                    } else {
                        // (AR) دالة عادية / (EN) Regular function
                        funcName = calleeVar->name;
                    }
                } else {
                    // (AR) دالة عادية / (EN) Regular function
                    funcName = calleeVar->name;
                }
            } else {
                // (AR) دالة عادية غير مخزنة في متغير / (EN) Regular function not stored in variable
                funcName = calleeVar->name;
            }
        } else {
            // ═══════════════════════════════════════════════════════════════
            // (AR) استدعاء دالة معقد: callee ليس متغيراً بسيطاً
            //      يدعم: استدعاء متسلسل getFunc()("arg")
            //             فهرسة callbacks[0]("arg")
            //             عضو فرعي obj.field الذي يحمل lambda
            //             كائن قابل للاستدعاء مع __call__
            //
            // (EN) Complex function call: callee is not a simple variable
            //      Supports: chained calls getFunc()("arg")
            //                index access callbacks[0]("arg")
            //                member access obj.field holding lambda
            //                callable object with __call__
            // ═══════════════════════════════════════════════════════════════
            node.callee->accept(*this);
            Value calleeValue = lastResult_;
            
            if (calleeValue.isString()) {
                // (AR) القيمة نص — قد يكون اسم lambda أو دالة
                // (EN) String value — may be lambda name or function name
                funcName = calleeValue.toString();
            } else if (calleeValue.isMap()) {
                // (AR) خريطة — نبحث عن __callable__ أو __lambda__
                // (EN) Map — look for __callable__ or __lambda__ key
                auto mapVal = calleeValue.toMap();
                auto callableIt = mapVal.find("__callable__");
                if (callableIt != mapVal.end() && callableIt->second.isString()) {
                    funcName = callableIt->second.toString();
                } else {
                    auto lambdaIt = mapVal.find("__lambda__");
                    if (lambdaIt != mapVal.end() && lambdaIt->second.isString()) {
                        funcName = lambdaIt->second.toString();
                    } else {
                        throw Interpreter::RuntimeError(
                            "(AR) لا يمكن استدعاء خريطة كدالة. أضف مفتاح '__callable__' لتحديد الدالة.\n"
                            "(EN) Cannot call a map as a function. Add '__callable__' key to specify function.",
                            node.position
                        );
                    }
                }
            } else if (calleeValue.isObject()) {
                // (AR) كائن — نبحث عن طريقة __call__
                // (EN) Object — look for __call__ method
                auto objPtr = calleeValue.toObject();
                if (objPtr) {
                    auto callIt = objPtr->fields.find("__call__");
                    if (callIt != objPtr->fields.end() && callIt->second.isString()) {
                        funcName = callIt->second.toString();
                    } else {
                        std::string className = objPtr->getClassName();
                        auto* classManager = Data::ClassManager::getInstance();
                        auto* classType = classManager->getClass(className);
                        if (classType) {
                            auto* callMethod = classType->findMethod("__call__");
                            if (callMethod && callMethod->body) {
                                // (AR) تقييم المعاملات / (EN) Evaluate arguments
                                std::vector<Data::Value> callArgs;
                                for (const auto& arg : node.arguments) {
                                    arg->accept(*this);
                                    callArgs.push_back(lastResult_);
                                }
                                // (AR) تنفيذ __call__ كطريقة الكائن
                                // (EN) Execute __call__ as the object's method
                                variableManager_.enterScope(Data::ScopeType::FUNCTION, "__call__");
                                variableManager_.define("هذا", calleeValue);
                                variableManager_.define("this", calleeValue);
                                for (const auto& [fname, fval] : objPtr->fields) {
                                    variableManager_.define(fname, fval);
                                }
                                for (size_t i = 0; i < callMethod->parameters.size() && i < callArgs.size(); ++i) {
                                    variableManager_.define(callMethod->parameters[i].name, callArgs[i]);
                                }
                                try {
                                    // إصلاح: التحقق من وجود body قبل الاستدعاء
                                    if (!callMethod->body) {
                                        variableManager_.exitScope();
                                        throw Interpreter::RuntimeError(
                                            "(AR) جسم الطريقة '__call__' فارغ.\n"
                                            "(EN) Method '__call__' has no body.",
                                            node.position
                                        );
                                    }
                                    callMethod->body->accept(statementExecutor_);
                                    if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                                        lastResult_ = statementExecutor_.getReturnValue();
                                        statementExecutor_.resetFlowControl();
                                    } else {
                                        lastResult_ = Value();
                                    }
                                } catch (...) {
                                    variableManager_.exitScope();
                                    throw;
                                }
                                variableManager_.exitScope();
                                return;  // (AR) تم التنفيذ بالكامل / (EN) Fully executed
                            }
                        }
                        throw Interpreter::RuntimeError(
                            "(AR) لا يمكن استدعاء كائن من صنف '" + className + "' كدالة. أضف طريقة '__call__'.\n"
                            "(EN) Cannot call object of class '" + className + "' as function. Add '__call__' method.",
                            node.position
                        );
                    }
                } else {
                    throw Interpreter::RuntimeError(
                        "(AR) كائن فارغ لا يمكن استدعاؤه كدالة.\n"
                        "(EN) Null object cannot be called as a function.",
                        node.position
                    );
                }
            } else {
                // (AR) نوع غير قابل للاستدعاء
                // (EN) Non-callable type
                std::string typeStr = calleeValue.isInteger() ? "عدد/integer" :
                                     calleeValue.isDouble() ? "عشري/float" :
                                     calleeValue.isBoolean() ? "منطقي/boolean" :
                                     calleeValue.isArray() ? "مصفوفة/array" :
                                     "غير معروف/unknown";
                throw Interpreter::RuntimeError(
                    "(AR) لا يمكن استدعاء قيمة من نوع '" + typeStr + "' كدالة.\n"
                    "(EN) Cannot call value of type '" + typeStr + "' as a function.",
                    node.position
                );
            }
        }
    }
    
    // (AR) تقييم المعاملات / (EN) Evaluate arguments
    // (AR) يدعم الوسائط المسمّاة بصيغة Flutter: عمود(تباعد: 10، خلفية: "ابيض")
    // (EN) Supports Flutter-like named arguments: column(spacing: 10, bg: "white")
    std::vector<Data::Value> arguments;
    std::vector<std::pair<std::string, Data::Value>> namedArgs;
    for (const auto& arg : node.arguments) {
        // (AR) كشف الوسيط المسمّى / (EN) Detect named argument
        if (auto* named = dynamic_cast<AST::NamedArgExpr*>(arg.get())) {
            named->value->accept(*this);
            namedArgs.emplace_back(named->name, lastResult_);
        } else {
            arg->accept(*this);
            arguments.push_back(lastResult_);
        }
    }
    // (AR) إلحاق الوسائط المسمّاة كأزواج اسم/قيمة (متوافق مع _autoChildren)
    // (EN) Append named args as name/value pairs (compatible with _autoChildren)
    for (size_t ni = 0; ni < namedArgs.size(); ++ni) {
        arguments.push_back(Data::Value(namedArgs[ni].first));
        arguments.push_back(namedArgs[ni].second);
    }
    
    // (AR) أولوية: إذا كنا داخل طريقة صنف، نبحث أولاً عن الطريقة في الصنف الحالي
    //       قبل البحث في الدوال العامة (لتجنب تعارض الأسماء مع الدوال المضمنة)
    // (EN) Priority: if inside a class method, search class methods first
    //       before global functions (to avoid name conflicts with built-ins)
    {
        auto* classManager = Data::ClassManager::getInstance();
        if (variableManager_.exists("هذا")) {
            Value thisValue = variableManager_.get("هذا");
            // (AR) الحصول على اسم الصنف — يدعم OBJECT الحقيقي و MAP القديم
            // (EN) Get class name — supports real OBJECT and legacy MAP
            std::string thisClassName;
            Data::ClassType* thisClassType = nullptr;
            
            if (thisValue.isObject()) {
                // (AR) نوع OBJECT الحقيقي
                // (EN) Real OBJECT type
                auto objPtr = thisValue.toObject();
                if (objPtr) {
                    thisClassName = objPtr->getClassName();
                    thisClassType = classManager->getClass(thisClassName);
                }
            } else if (thisValue.isMap()) {
                // (AR) MAP القديم مع __class__
                // (EN) Legacy MAP with __class__
                auto thisFields = thisValue.toMap();
                auto classNameIt = thisFields.find("__class__");
                if (classNameIt != thisFields.end()) {
                    thisClassName = classNameIt->second.toString();
                    thisClassType = classManager->getClass(thisClassName);
                }
            }
            
            if (thisClassType) {
                        Data::ClassMethod* method = thisClassType->findMethod(funcName);
                        // (AR) البحث عن الجسم: أولاً في الطريقة نفسها، ثم في مصدر القالب
                        // (EN) Find body: first in method itself, then in template instance source
                        AST::Statement* methodBody = nullptr;
                        if (method && method->body) {
                            methodBody = method->body.get();
                        } else if (method) {
                            auto* tplSrc = statementExecutor_.getTemplateInstanceSource(thisClassName);
                            if (tplSrc) {
                                for (auto& m : tplSrc->members) {
                                    if (auto* md = dynamic_cast<AST::MethodDecl*>(m.get())) {
                                        if (md->name == funcName && md->body) {
                                            methodBody = md->body.get();
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        if (method && methodBody) {
                            // (AR) وجدنا طريقة في الصنف الحالي - ننفذها بأولوية
                            // (EN) Found method in current class - execute with priority
                            if (arguments.size() != method->parameters.size()) {
                                std::string errMsg = "(AR) عدد معاملات الطريقة '" + funcName + "' غير متطابق. ";
                                errMsg += "توقع " + std::to_string(method->parameters.size()) + " لكن حصل على " + std::to_string(arguments.size()) + ". ";
                                errMsg += "(EN) Argument count mismatch for method '" + funcName + "'.";
                                throw RuntimeError(errMsg, node.position);
                            }
                            
                            // (AR) ندفع نطاق فقط للمعاملات - الحقول موروثة من النطاق الأب
                            // (EN) Push scope only for parameters - fields inherited from parent scope
                            variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);
                            
                            for (size_t i = 0; i < method->parameters.size(); ++i) {
                                variableManager_.define(method->parameters[i].name, arguments[i]);
                            }
                            
                            Value returnValue;
                            try {
                                methodBody->accept(statementExecutor_);
                                if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                                    returnValue = statementExecutor_.getReturnValue();
                                    statementExecutor_.resetFlowControl();
                                }
                            } catch (...) {
                                variableManager_.exitScope();
                                throw;
                            }
                            
                            variableManager_.exitScope();
                            lastResult_ = returnValue;
                            return;
                        }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) التعامل مع استدعاء باني الأب: أساس(...) أو الأساس(...) أو super(...)
    // (EN) Handle super constructor call: أساس(...) or الأساس(...) or super(...)
    // ═══════════════════════════════════════════════════════════════
    if (funcName == "أساس" || funcName == "الأساس" || funcName == "الاساس" || 
        funcName == "اساس" || funcName == "super") {
        // (AR) نبحث عن هذا في النطاق لتحديد الصنف الحالي
        // (EN) Look for this in scope to determine current class
        if (variableManager_.exists("هذا")) {
            Value thisValue = variableManager_.get("هذا");
            auto* classManager = Data::ClassManager::getInstance();
            
            std::string currentClassName;
            Data::ObjectInstance* objPtr = nullptr;
            
            if (thisValue.isObject()) {
                auto objShared = thisValue.toObject();
                objPtr = objShared.get();
                if (objPtr) currentClassName = objPtr->getClassName();
            } else if (thisValue.isMap()) {
                auto fields = thisValue.toMap();
                auto it = fields.find("__class__");
                if (it != fields.end()) currentClassName = it->second.toString();
            }
            
            // ═══════════════════════════════════════════════════════════
            // (AR) إصلاح الوراثة متعددة المستويات: نتتبع أي صنف ننفذ بانيه حالياً
            // (EN) Fix multi-level inheritance: track which class constructor is executing
            // (AR) مثال: قط_فارسي.باني يستدعي أساس() → يجب أن ينادي باني قط (وليس قط_فارسي مرة أخرى)
            //       قط.باني يستدعي أساس() → يجب أن ينادي باني حيوان (وليس قط مرة أخرى)
            // ═══════════════════════════════════════════════════════════
            std::string executingClassName = currentClassName;
            if (variableManager_.exists("__executing_constructor_class__")) {
                executingClassName = variableManager_.get("__executing_constructor_class__").toString();
            }
            
            if (!executingClassName.empty()) {
                Data::ClassType* executingClass = classManager->getClass(executingClassName);
                if (executingClass) {
                    Data::ClassType* baseClass = executingClass->getBaseClass();
                    if (baseClass && baseClass->constructor) {
                        AST::ConstructorDecl* baseCtor = baseClass->constructor.get();
                        // (AR) ربط معاملات باني الأب
                        // (EN) Bind base constructor parameters
                        if (arguments.size() == baseCtor->parameters.size()) {
                            for (size_t i = 0; i < baseCtor->parameters.size(); ++i) {
                                const auto& pname = baseCtor->parameters[i].name;
                                // (AR) تحسين أداء: بحث واحد / (EN) Performance: single lookup
                                variableManager_.defineOrAssign(pname, arguments[i]);
                            }
                            // (AR) تعيين الصنف المنفذ حالياً لدعم السلسلة (A→B→C)
                            // (EN) Set executing class for chained super calls (A→B→C)
                            std::string baseClassName = baseClass->name;
                            bool hadPrevious = variableManager_.exists("__executing_constructor_class__");
                            Value previousVal;
                            if (hadPrevious) previousVal = variableManager_.get("__executing_constructor_class__");
                            
                            variableManager_.defineOrAssign("__executing_constructor_class__", Value(baseClassName));
                            
                            try {
                                baseCtor->body->accept(statementExecutor_);
                            } catch (const std::exception& e) {
                                // (AR) خطأ في تنفيذ باني الصنف الأساسي — إعادة رفعه
                                // (EN) Error executing base class constructor — re-throw
                                throw;
                            } catch (...) {
                                throw Interpreter::SadException(
                                    "(AR) خطأ غير معروف في باني الصنف الأساسي '" + baseClassName + "'. "
                                    "(EN) Unknown error in base class constructor '" + baseClassName + "'.",
                                    "RuntimeError", node.position);
                            }
                            
                            // (AR) استعادة القيمة السابقة
                            // (EN) Restore previous value
                            if (hadPrevious) {
                                variableManager_.assign("__executing_constructor_class__", previousVal);
                            }
                            
                            // (AR) قراءة القيم المحدثة من باني الأب وتحديث الكائن
                            // (EN) Read updated values from base constructor and update object
                            if (objPtr) {
                                // (AR) نجمع كل الحقول من سلسلة الوراثة
                                // (EN) Collect all fields from inheritance chain
                                std::function<void(Data::ClassType*)> collectFields;
                                collectFields = [&](Data::ClassType* cls) {
                                    if (!cls) return;
                                    if (cls->getBaseClass()) collectFields(cls->getBaseClass());
                                    for (const auto& field : cls->fields) {
                                        if (!field.isStatic) {
                                            if (variableManager_.exists(field.name)) {
                                                Value val = variableManager_.get(field.name);
                                                objPtr->fields[field.name] = val;
                                            }
                                            // (AR) الحقل لم يُعيَّن في باني الأب — متوقع
                                            // (EN) Field not set in base constructor — expected
                                        }
                                    }
                                };
                                collectFields(baseClass);
                            }
                        }
                        lastResult_ = Value();
                        return;
                    }
                }
            }
        }
        // (AR) لم نجد باني أب - نتجاهل الاستدعاء
        // (EN) No base constructor found - ignore the call
        lastResult_ = Value();
        return;
    }
    
    // (AR) البحث عن الدالة - flexible matching مع default parameters
    // (EN) Find function - flexible matching with default parameters
    auto allOverloads = functionManager_.getFunctionOverloads(funcName);
    std::shared_ptr<Data::FunctionDefinition> func = nullptr;
    
    // (AR) نبحث عن دالة تقبل هذا العدد من المعاملات (مع الافتراضيات)
    // (EN) Search for function that accepts this argument count (with defaults)
    // (AR) الأولوية 1: الدوال المعرفة من المستخدم التي تطابق عدد المعاملات
    // (EN) Priority 1: User-defined functions that match argument count
    for (const auto& candidate : allOverloads) {
        if (!candidate->hasNativeImplementation() && candidate->acceptsArgumentCount(arguments.size())) {
            func = candidate;
            break;
        }
    }
    // (AR) الأولوية 2: الدوال المضمنة (كنسخة احتياطية)
    // (EN) Priority 2: Built-in functions (as fallback)
    if (!func) {
        for (const auto& candidate : allOverloads) {
            if (candidate->hasNativeImplementation()) {
                func = candidate;
                break;
            }
        }
    }
    
    // (AR) إذا لم نجد دالة مناسبة
    // (EN) If no suitable function found
    if (!func) {
        // (AR) محاولة أخيرة - exact match (للدوال بدون defaults)
        // (EN) Last attempt - exact match (for functions without defaults)
        func = functionManager_.getFunction(funcName, arguments.size());
        
        if (!func) {
            // (AR) التحقق إذا كان الاسم هو اسم صنف مسجّل → إنشاء كائن بدون 'جديد'
            // (EN) Check if name is a registered class → create object without 'new' keyword
            // مثال: مكدس م1 = مكدس()  بدلاً من  متغير م1 = جديد مكدس()
            auto* classManager2 = Data::ClassManager::getInstance();
            Data::ClassType* classType = classManager2->getClass(funcName);
            if (classType) {
                // (AR) بناء عقدة NewExpr مؤقتة والنقل إليها
                // (EN) Build a temporary NewExpr node and delegate
                NewExpr tempNewExpr(funcName);
                tempNewExpr.position = node.position;
                // (AR) نقل المعاملات من الـ CallExpr إلى NewExpr
                // (EN) Move arguments from CallExpr to NewExpr
                for (auto& arg : node.arguments) {
                    tempNewExpr.arguments.push_back(std::move(arg));
                }
                visitNewExpr(tempNewExpr);
                // (AR) إعادة المعاملات للـ CallExpr لتجنب مؤشرات معلقة
                // (EN) Move arguments back to CallExpr to avoid dangling pointers
                node.arguments.clear();
                for (auto& arg : tempNewExpr.arguments) {
                    node.arguments.push_back(std::move(arg));
                }
                return;
            }
            
            // (AR) دالة غير معرّفة — مع اقتراح "هل قصدت؟"
            // (EN) Undefined function — with "Did you mean?" suggestion
            std::string msgAr = "الدالة '" + funcName + "' غير معرفة بعدد معاملات " + std::to_string(arguments.size());
            std::string msgEn = "Function '" + funcName + "' not defined with " + std::to_string(arguments.size()) + " parameters";
            
            // (AR) بحث عن أسماء دوال مشابهة / (EN) Search for similar function names
            auto availableFuncs = functionManager_.getFunctionNames();
            Sad::Errors::SuggestionEngine sugEngine;
            auto similar = sugEngine.findSimilarSymbols(funcName, availableFuncs);
            if (!similar.empty()) {
                msgAr += " — هل قصدت: '" + similar[0] + "'؟";
                msgEn += " — Did you mean: '" + similar[0] + "'?";
            }
            
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                msgAr,
                msgEn
            );
            lastResult_ = Value();
            return;
        }
    }
    
    // (AR) التحقق من وجود تنفيذ أصلي (دالة مضمنة) / (EN) Check for native implementation (built-in function)
    if (func->hasNativeImplementation()) {
        // ═══════════════════════════════════════════════════════════
        // (AR) تحسين أداء: حجز الذاكرة مسبقاً للمؤشرات
        // (EN) Performance: pre-reserve memory for argument pointers
        // ═══════════════════════════════════════════════════════════
        std::vector<std::shared_ptr<Data::Value>> valuePtrs;
        valuePtrs.reserve(arguments.size());
        for (const auto& arg : arguments) {
            valuePtrs.push_back(std::make_shared<Data::Value>(arg));
        }
        
        // (AR) استدعاء التنفيذ الأصلي / (EN) Call native implementation
        auto resultPtr = func->callNative(valuePtrs);
        
        if (resultPtr) {
            lastResult_ = *resultPtr;
        } else {
            lastResult_ = Data::Value();  // void return
        }
        
        return;
    }
    
    // (AR) التحقق من وجود جسم للدالة / (EN) Check if function has body
    if (!func->hasBody()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الدالة '" + funcName + "' ليس لها جسم",
            "Function '" + funcName + "' has no body"
        );
        lastResult_ = Value();
        return;
    }
    
    // (AR) إنشاء نطاق جديد للدالة / (EN) Create new scope for function
    variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);
    
    // (AR) حقن المتغيرات الملتقطة (للإغلاقات/لامدا) / (EN) Inject captured variables (for closures/lambda)
    if (func->hasCaptures()) {
        for (const auto& [capName, capVal] : func->getCaptures()) {
            variableManager_.define(capName, capVal);
        }
    }
    
    // (AR) الحصول على المعاملات / (EN) Get parameters
    const auto& params = func->getParameters();
    
    // (AR) تقييم القيم الافتراضية للمعاملات المفقودة
    // (EN) Evaluate default values for missing arguments
    
    // (AR) محاولة الحصول على FunctionDecl الأصلي للوصول للـ Parameters
    // (EN) Try to get original FunctionDecl to access Parameters
    auto funcDeclNode = func->getFunctionDecl();
    FunctionDecl* astFuncDecl = nullptr;
    
    if (funcDeclNode) {
        // (AR) تحويل آمن: AST::ASTNode إلى FunctionDecl
        // (EN) Safe cast: AST::ASTNode to FunctionDecl
        astFuncDecl = dynamic_cast<FunctionDecl*>(funcDeclNode.get());
    }
    
    while (arguments.size() < params.size()) {
        size_t i = arguments.size();
        const auto& param = params[i];
        
        if (!param.hasDefaultValue) {
            variableManager_.exitScope();
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "معامل إلزامي مفقود: " + param.name,
                "Required parameter missing: " + param.name
            );
            lastResult_ = Value();
            return;
        }
        
        Data::Value defaultVal;
        
        // (AR) إذا كان لدينا FunctionDecl، نقيّم ExprPtr من parameters
        // (EN) If we have FunctionDecl, evaluate ExprPtr from parameters
        if (astFuncDecl && i < astFuncDecl->parameters.size()) {
            const auto& astParam = astFuncDecl->parameters[i];
            
            if (astParam.defaultValue) {
                // (AR) تقييم التعبير الافتراضي
                // (EN) Evaluate default expression
                astParam.defaultValue->accept(*this);
                defaultVal = lastResult_;
            } else {
                // (AR) لا توجد قيمة افتراضية في AST (خطأ)
                // (EN) No default value in AST (error)
                variableManager_.exitScope();
                throw RuntimeError(
                    "(AR) معامل إلزامي مفقود: " + param.name + 
                    " / (EN) Required parameter missing: " + param.name
                );
            }
        } else {
            // (AR) Fallback: تحويل القيمة الافتراضية من string إلى Value
            // (EN) Fallback: Convert default value from string to Value
            const std::string& defaultStr = param.defaultValue;
            
            // (AR) محاولة تحويل النص إلى قيمة
            // (EN) Try to convert string to value
            if (defaultStr.empty()) {
                defaultVal = Data::Value();  // null
            } else if (defaultStr == "true" || defaultStr == "صحيح") {
                defaultVal = Data::Value(true);
            } else if (defaultStr == "false" || defaultStr == "خطأ") {
                defaultVal = Data::Value(false);
            } else if (defaultStr[0] == '"' || defaultStr[0] == '\'') {
                // String literal — guard against single-char strings
                std::string strVal = (defaultStr.length() >= 2) 
                    ? defaultStr.substr(1, defaultStr.length() - 2) 
                    : "";
                defaultVal = Data::Value(strVal);
            } else {
                // (AR) محاولة تحويل إلى رقم
                // (EN) Try to convert to number
                try {
                    if (defaultStr.find('.') != std::string::npos) {
                        defaultVal = Data::Value(std::stod(defaultStr));
                    } else {
                        defaultVal = Data::Value(std::stoi(defaultStr));
                    }
                } catch (...) {
                    // (AR) فشل التحويل - نستخدم النص كما هو
                    // (EN) Conversion failed - use string as is
                    defaultVal = Data::Value(defaultStr);
                }
            }
        }
        
        arguments.push_back(defaultVal);
    }
    
    // (AR) تعريف المعاملات كمتغيرات محلية / (EN) Define parameters as local variables
    for (size_t i = 0; i < params.size(); ++i) {
        variableManager_.define(params[i].name, arguments[i]);
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) التحقق من نوع المعامل في وقت التشغيل لأنواع الأصناف
        //      إذا حدد المبرمج نوع صنف (مثل: دالة عرض(شخص ش))
        //      نتحقق أن القيمة المرسلة هي فعلاً كائن من هذا الصنف
        //
        // (EN) Runtime type checking for class-typed parameters
        //      If programmer specified a class type (e.g.: func show(Person p))
        //      we verify the passed value is actually an object of that class
        // ═══════════════════════════════════════════════════════════════
        if (!params[i].typeName.empty()) {
            const std::string& expectedClass = params[i].typeName;
            const Value& argVal = arguments[i];
            
            if (argVal.isObject()) {
                // (AR) كائن حقيقي — نتحقق من اسم الصنف
                // (EN) Real object — check class name
                auto objPtr = argVal.toObject();
                if (objPtr && objPtr->getClassName() != expectedClass) {
                    // (AR) أيضاً نتحقق من الوراثة
                    // (EN) Also check inheritance
                    auto* classManager = Data::ClassManager::getInstance();
                    auto* expectedClassType = classManager->getClass(expectedClass);
                    if (!expectedClassType || !objPtr->isInstanceOf(expectedClassType)) {
                        throw RuntimeError(
                            "(AR) نوع المعامل '" + params[i].name + "' غير متطابق. توقع كائن من صنف '" + 
                            expectedClass + "' لكن حصل على '" + objPtr->getClassName() + "'. " +
                            "(EN) Type mismatch for parameter '" + params[i].name + "'. Expected object of class '" + 
                            expectedClass + "' but got '" + objPtr->getClassName() + "'.",
                            node.position
                        );
                    }
                }
            } else if (argVal.isObjectLike()) {
                // (AR) MAP قديم مع __class__ — نتحقق من اسم الصنف
                // (EN) Legacy MAP with __class__ — check class name
                std::string actualClass = argVal.getClassName();
                if (actualClass != expectedClass) {
                    throw RuntimeError(
                        "(AR) نوع المعامل '" + params[i].name + "' غير متطابق. توقع '" + 
                        expectedClass + "' لكن حصل على '" + actualClass + "'. " +
                        "(EN) Type mismatch for '" + params[i].name + "'. Expected '" + 
                        expectedClass + "' but got '" + actualClass + "'.",
                        node.position
                    );
                }
            } else {
                // (AR) القيمة ليست كائناً أصلاً
                // (EN) Value is not an object at all
                throw RuntimeError(
                    "(AR) نوع المعامل '" + params[i].name + "' غير متطابق. توقع كائن من صنف '" + 
                    expectedClass + "' لكن حصل على قيمة من نوع '" + argVal.getTypeName() + "'. " +
                    "(EN) Type mismatch for '" + params[i].name + "'. Expected object of class '" + 
                    expectedClass + "' but got value of type '" + argVal.getTypeName() + "'.",
                    node.position
                );
            }
        }
    }
    
    // (AR) تنفيذ جسم الدالة / (EN) Execute function body
    auto bodyNode = func->getBody();
    
    // (AR) Lambda body هو Expression، دوال عادية لها Statement body
    // (EN) Lambda body is Expression, regular functions have Statement body
    // (AR) getBody() يُرجع shared_ptr<AST::ASTNode> — تحويل آمن بـ dynamic_cast
    // (EN) getBody() returns shared_ptr<AST::ASTNode> — safe dynamic_cast
    auto bodyStmt = dynamic_cast<AST::Statement*>(bodyNode.get());
    
    if (bodyStmt) {
        // (AR) التحقق إذا كانت دالة مولّدة / (EN) Check if generator function
        if (func->isGenerator()) {
            // (AR) وضع المولّد - جمع كل قيم yield في مصفوفة
            // (EN) Generator mode - collect all yield values into array
            bool wasInGenerator = statementExecutor_.isInGenerator();
            statementExecutor_.setGeneratorMode(true);
            statementExecutor_.clearGeneratorYieldValues();
            
            statementExecutor_.executeFunctionBody(*bodyStmt);
            
            // (AR) تحويل القيم المجمّعة إلى مصفوفة
            // (EN) Convert collected values to array
            const auto& yields = statementExecutor_.getGeneratorYieldValues();
            std::vector<Data::Value> arr(yields.begin(), yields.end());
            lastResult_ = Data::Value(arr);
            
            statementExecutor_.clearGeneratorYieldValues();
            statementExecutor_.setGeneratorMode(wasInGenerator);
        } else {
            // ═══════════════════════════════════════════════════════════
            // (AR) التحقق إذا كانت دالة غير متزامنة (async)
            // (EN) Check if async function
            // ═══════════════════════════════════════════════════════════
            if (func->isAsync()) {
                // (AR) تنفيذ الدالة غير المتزامنة — ننشئ Future ونرجعه ككائن
                // (EN) Execute async function — create Future and return as object
                auto& runtime = AsyncRuntime::getInstance();
                
                // (AR) نلتقط المعاملات الحالية لاستخدامها في المهمة
                // (EN) Capture current parameters for use in task
                std::unordered_map<std::string, Data::Value> capturedVars;
                for (size_t i = 0; i < params.size(); ++i) {
                    capturedVars[params[i].name] = arguments[i];
                }
                // (AR) نلتقط أيضاً المتغيرات الملتقطة من الإغلاقات
                // (EN) Also capture closure variables
                if (func->hasCaptures()) {
                    for (const auto& [capName, capVal] : func->getCaptures()) {
                        capturedVars[capName] = capVal;
                    }
                }
                
                // (AR) تنفيذ الجسم بشكل متزامن ولكن نلف النتيجة في Future
                //      (المرحلة الأولى: تنفيذ متزامن مع واجهة async)
                //      في المستقبل يمكن تنفيذ الجسم في thread pool حقيقي
                // (EN) Execute body synchronously but wrap result in Future
                //      (Phase 1: sync execution with async interface)
                //      In the future, can execute body in real thread pool
                Value asyncResult;
                try {
                    asyncResult = statementExecutor_.executeFunctionBody(*bodyStmt);
                } catch (const std::exception& e) {
                    // (AR) في حالة خطأ — ننشئ كائن Future مرفوض
                    // (EN) On error — create rejected Future object
                    std::unordered_map<std::string, Data::Value> futureMap;
                    futureMap["__future__"] = Data::Value(true);
                    futureMap["__resolved__"] = Data::Value(false);
                    futureMap["__rejected__"] = Data::Value(true);
                    futureMap["__error__"] = Data::Value(std::string(e.what()));
                    variableManager_.exitScope();
                    lastResult_ = Data::Value(futureMap);
                    return;
                }
                
                // (AR) ننشئ كائن Future محلول بالنتيجة
                // (EN) Create resolved Future object with result
                std::unordered_map<std::string, Data::Value> futureMap;
                futureMap["__future__"] = Data::Value(true);
                futureMap["__resolved__"] = Data::Value(true);
                futureMap["__rejected__"] = Data::Value(false);
                futureMap["__value__"] = asyncResult;
                
                variableManager_.exitScope();
                lastResult_ = Data::Value(futureMap);
                return;
            }
            
            // (AR) دالة عادية - نستخدم StatementExecutor / (EN) Regular function - use StatementExecutor
            lastResult_ = statementExecutor_.executeFunctionBody(*bodyStmt);
        }
    } else {
        // (AR) هذه دالة Lambda - نقيّم التعبير مباشرةً / (EN) This is Lambda - evaluate expression directly
        auto bodyExpr = dynamic_cast<AST::Expression*>(bodyNode.get());
        
        if (!bodyExpr) {
            variableManager_.exitScope();
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "جسم الدالة فارغ",
                "Function body is null"
            );
            lastResult_ = Value();
            return;
        }
        
        bodyExpr->accept(*this);
        // lastResult_ already contains the result
    }
    
    // (AR) الخروج من نطاق الدالة / (EN) Exit function scope
    variableManager_.exitScope();
}

} // namespace Interpreter
} // namespace Sad
