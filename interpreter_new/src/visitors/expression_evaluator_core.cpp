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
#include "ast_node.h"  // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "directive_nodes.h"    // For SizeofExpr, AtomicExpr
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
// (AR) تحسين النصوص العربية / (EN) Arabic String Optimization
// =========================================================================

bool ExpressionEvaluator::containsArabic(const std::string& str) {
    // (AR) فحص إذا كان النص يحتوي أحرف عربية (UTF-8)
    // Arabic Unicode range: U+0600-U+06FF (UTF-8: 0xD8 0x80 - 0xDB 0xBF)
    // (EN) Properly validate 2-byte UTF-8 sequence: leading byte 0xD8-0xDB + continuation 0x80-0xBF
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
    size_t len = str.size();
    for (size_t i = 0; i < len; ++i) {
        unsigned char b = bytes[i];
        // (AR) تخطي تسلسلات UTF-8 بـ 3 و 4 بايت لتجنب الإيجابيات الخاطئة
        if (b >= 0xF0 && i + 3 < len) { i += 3; continue; } // 4-byte seq
        if (b >= 0xE0 && i + 2 < len) { i += 2; continue; } // 3-byte seq
        // (AR) التحقق من تسلسل عربي مكون من 2 بايت
        if (b >= 0xD8 && b <= 0xDB && i + 1 < len) {
            unsigned char next = bytes[i + 1];
            if (next >= 0x80 && next <= 0xBF) return true; // Arabic confirmed
            i++; // skip continuation byte
        }
    }
    return false;
}

const std::string& ExpressionEvaluator::internString(const std::string& str) {
    arabicOptStats_.totalStrings++;
    
    if (containsArabic(str)) {
        arabicOptStats_.arabicStrings++;
    }
    
    auto [it, inserted] = stringPool_.insert(str);
    if (inserted) {
        // نص جديد - تم إضافته للمُجمّع
        arabicOptStats_.pooledStrings++;
    } else {
        // نص مكرر - تم توفير الذاكرة
        arabicOptStats_.poolHits++;
        arabicOptStats_.savedBytes += str.size();
    }
    
    return *it;
}

void ExpressionEvaluator::printArabicOptStats() const {
    std::cout << "\n========================================\n";
    std::cout << "إحصائيات التحسين العربي (المفسّر) / Arabic Optimization Statistics (Interpreter)\n";
    std::cout << "========================================\n\n";
    
    std::cout << "📝 تحسينات النصوص / String Optimizations:\n";
    std::cout << "  • مجموع النصوص / Total strings: " << arabicOptStats_.totalStrings << "\n";
    std::cout << "  • النصوص الفريدة / Unique strings: " << arabicOptStats_.pooledStrings << "\n";
    std::cout << "  • إصابات المُجمّع / Pool hits: " << arabicOptStats_.poolHits << "\n";
    std::cout << "  • النصوص العربية / Arabic strings: " << arabicOptStats_.arabicStrings << "\n";
    std::cout << "  • البايتات الموفرة / Saved bytes: " << arabicOptStats_.savedBytes << "\n";
    
    if (arabicOptStats_.totalStrings > 0) {
        double hitRate = (static_cast<double>(arabicOptStats_.poolHits) / arabicOptStats_.totalStrings) * 100.0;
        std::cout << "  • نسبة الإصابة / Hit rate: " << hitRate << "%\n";
    }
    
    std::cout << "========================================\n\n";
}

// =========================================================================
// (AR) تقييم القيم الحرفية / (EN) Literal Evaluation
// =========================================================================

void ExpressionEvaluator::visitLiteralExpr(LiteralExpr& node) {
    // (AR) استخدام ذاكرة التخزين المؤقت — يمنع إعادة تحليل stoll/stod في الحلقات
    // (EN) Use cache — prevents re-parsing stoll/stod in loops
    const void* key = static_cast<const void*>(&node);
    auto it = literalCache_.find(key);
    if (it != literalCache_.end()) {
        lastResult_ = it->second;
        return;
    }
    Value val = tokenToValue(node.token);
    literalCache_[key] = val;
    lastResult_ = std::move(val);
}

Value ExpressionEvaluator::tokenToValue(const Token& token) {
    switch (token.getType()) {
        case TokenType::NUMBER_INTEGER: {
            std::string value = token.getValue();
            
            // (AR) تحويل آمن من stoll إلى int64_t (كل الأعداد الصحيحة تبقى صحيحة)
            // (EN) Safe stoll→int64_t conversion (all integers stay as integers)
            auto safeToInt64 = [](long long val) -> Value {
                return Value(static_cast<int64_t>(val));
            };
            
            try {
                // Binary: 0b1010
                if (value.size() > 2 && value[0] == '0' && (value[1] == 'b' || value[1] == 'B')) {
                    return safeToInt64(std::stoll(value.substr(2), nullptr, 2));
                }
                
                // Octal: 0o17
                if (value.size() > 2 && value[0] == '0' && (value[1] == 'o' || value[1] == 'O')) {
                    return safeToInt64(std::stoll(value.substr(2), nullptr, 8));
                }
                
                // Hexadecimal: 0xFF
                if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
                    return safeToInt64(std::stoll(value.substr(2), nullptr, 16));
                }
                
                // Decimal: 42 — use stoll for range safety
                {
                    long long parsed = std::stoll(value);
                    return safeToInt64(parsed);
                }
            } catch (const std::invalid_argument&) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), 1, 1),
                    "تنسيق عدد غير صالح: " + value,
                    "Invalid number format: " + value
                );
                return Value(0);
            } catch (const std::out_of_range&) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), 1, 1),
                    "العدد خارج النطاق: " + value,
                    "Number out of range: " + value
                );
                return Value(0);
            }
        }
        
        case TokenType::NUMBER_DOUBLE: {
            try {
                return Value(std::stod(token.getValue()));
            } catch (const std::invalid_argument&) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), 1, 1),
                    "تنسيق عدد عشري غير صالح: " + token.getValue(),
                    "Invalid float format: " + token.getValue()
                );
                return Value(0.0);
            } catch (const std::out_of_range&) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), 1, 1),
                    "العدد العشري خارج النطاق: " + token.getValue(),
                    "Float out of range: " + token.getValue()
                );
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
            return Value();  // VOID
        
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation(getSourceFilename(), 1, 1),
                "نوع رمز غير مدعوم: " + token.getValue(),
                "Unsupported token type: " + token.getValue()
            );
            return Value(); // Return null
    }
}

// =========================================================================
// (AR) تقييم المتغيرات / (EN) Variable Evaluation
// =========================================================================

void ExpressionEvaluator::visitVariableExpr(VariableExpr& node) {
    // (AR) بحث واحد بدلاً من exists() + get() — تحسين أداء مهم
    // (EN) Single lookup instead of exists() + get() — important performance optimization
    const Value* varPtr = variableManager_.tryGet(node.name);
    
    if (!varPtr) {
        // التحقق من وجود صنف بهذا الاسم (للوصول الثابت)
        // Check if class exists with this name (for static access)
        auto* classManager = Data::ClassManager::getInstance();
        ClassType* classType = classManager->getClass(node.name);
        
        if (classType) {
            lastResult_ = Value(node.name);
            return;
        }
        
        // (AR) التحقق من وجود دالة بهذا الاسم — دوال من الدرجة الأولى
        // (EN) Check if a function exists with this name — first-class functions
        if (functionManager_.hasFunction(node.name)) {
            // (AR) إنشاء مرجع دالة من نوع FUNCTION بدل STRING
            // (EN) Create FUNCTION type reference instead of STRING
            Data::FunctionRefKind kind = Data::FunctionRefKind::USER_DEFINED;
            int arity = -1;
            auto funcDef = functionManager_.getFunction(node.name, -1);
            if (funcDef) {
                switch (funcDef->getType()) {
                    case Data::FunctionType::BUILT_IN: kind = Data::FunctionRefKind::BUILT_IN; break;
                    case Data::FunctionType::LAMBDA:   kind = Data::FunctionRefKind::LAMBDA; break;
                    default: kind = Data::FunctionRefKind::USER_DEFINED; break;
                }
                arity = static_cast<int>(funcDef->getParameterCount());
            }
            auto funcRef = std::make_shared<Data::FunctionRef>(
                node.name, node.name, kind, arity
            );
            lastResult_ = Value(std::move(funcRef));
            return;
        }
        
        // متغير غير معرّف — مع اقتراح "هل قصدت؟"
        // (EN) Undefined variable — with "Did you mean?" suggestion
        std::string msgAr = "متغير غير معرّف: " + node.name;
        std::string msgEn = "Undefined variable: " + node.name;
        
        // (AR) بحث عن أسماء مشابهة / (EN) Search for similar names
        auto availableNames = variableManager_.getVariableNames();
        Sad::Errors::SuggestionEngine sugEngine;
        auto similar = sugEngine.findSimilarSymbols(node.name, availableNames);
        if (!similar.empty()) {
            msgAr += " — هل قصدت: '" + similar[0] + "'؟";
            msgEn += " — Did you mean: '" + similar[0] + "'?";
        }
        
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            msgAr,
            msgEn
        );
        lastResult_ = Value(); // Return null
        return;
    }
    
    // (AR) فحص الملكية: هل المتغير صالح للاستخدام؟ / (EN) Ownership check: is variable valid?
    if (ownershipManager_.isEnabled()) {
        auto error = ownershipManager_.useVariable(node.name);
        if (error.has_value()) {
            // (AR) المتغير منقول أو غير صالح / (EN) Variable moved or invalid
            throw SadException(
                error->arabicMessage + " / " + error->message,
                "OwnershipError",
                node.position
            );
        }
    }
    
    // (AR) استخدام المؤشر الذي حصلنا عليه من tryGet — بدون بحث ثانٍ
    // (EN) Use the pointer we got from tryGet — no second lookup
    lastResult_ = *varPtr;
}

void ExpressionEvaluator::visitBorrowExpr(BorrowExpr& node) {
    // (AR) التحقق من وجود المتغير / (EN) Check variable exists
    if (!variableManager_.exists(node.variableName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x91\xD9\x81: " + node.variableName,
            "Undefined variable: " + node.variableName
        );
        lastResult_ = Value();
        return;
    }
    
    // (AR) إنشاء استعارة في نظام الملكية / (EN) Create borrow in ownership system
    if (ownershipManager_.isEnabled()) {
        auto borrowKind = node.isMutable 
            ? Data::BorrowKind::Mutable 
            : Data::BorrowKind::Shared;
        // (AR) اسم المستعير المؤقت / (EN) Temporary borrower name
        std::string borrowerName = "__borrow_" + node.variableName;
        auto error = ownershipManager_.createBorrow(node.variableName, borrowerName, borrowKind);
        if (error.has_value()) {
            throw SadException(
                error->arabicMessage + " / " + error->message,
                "OwnershipError",
                node.position
            );
        }
    }
    
    // (AR) إرجاع قيمة المتغير (الاستعارة تُرجع نفس القيمة)
    // (EN) Return variable value (borrow returns same value)
    lastResult_ = variableManager_.get(node.variableName);
}

void ExpressionEvaluator::visitThisExpr(ThisExpr& node) {
    // الحصول على this من النطاق الحالي
    // Get 'this' from current scope
    if (variableManager_.exists("هذا")) {
        lastResult_ = variableManager_.get("هذا");
    } else if (variableManager_.exists("this")) {
        lastResult_ = variableManager_.get("this");
    } else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "(AR) 'هذا' غير متاح في هذا السياق. (EN) 'this' is not available in this context.",
            "'this' keyword used outside of class context"
        );
        lastResult_ = Value();
    }
}

void ExpressionEvaluator::visitSuperExpr(SuperExpr& node) {
    // (AR) الحصول على super من النطاق الحالي — نبحث عن الصنف الأب
    // (EN) Get super from current scope — look for base class
    if (variableManager_.exists("الأساس")) {
        lastResult_ = variableManager_.get("الأساس");
    } else if (variableManager_.exists("super")) {
        lastResult_ = variableManager_.get("super");
    } else if (variableManager_.exists("هذا")) {
        // (AR) نحاول بناء مرجع الأساس من الكائن الحالي
        // (EN) Try to build super reference from current object
        Value thisValue = variableManager_.get("هذا");
        auto* classManager = Data::ClassManager::getInstance();
        std::string className;
        if (thisValue.isObject()) {
            auto objPtr = thisValue.toObject();
            if (objPtr) className = objPtr->getClassName();
        } else if (thisValue.isMap()) {
            auto fields = thisValue.toMap();
            auto it = fields.find("__class__");
            if (it != fields.end()) className = it->second.toString();
        }
        if (!className.empty()) {
            Data::ClassType* cls = classManager->getClass(className);
            if (cls && cls->getBaseClass()) {
                // (AR) نرجع اسم الصنف الأب كقيمة نصية
                // (EN) Return base class name as string value
                lastResult_ = Value(cls->getBaseClass()->name);
                return;
            }
        }
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "(AR) 'الأساس' غير متاح - الصنف لا يرث من صنف آخر. (EN) 'super' not available - class does not inherit.",
            "'super' keyword used in class without base class"
        );
        lastResult_ = Value();
    } else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "(AR) 'الأساس' غير متاح في هذا السياق. (EN) 'super' is not available in this context.",
            "'super' keyword used outside of class context or class without base"
        );
        lastResult_ = Value();
    }
}

// =========================================================================
// (AR) تقييم الإسناد / (EN) Assignment Evaluation
// =========================================================================

void ExpressionEvaluator::visitAssignExpr(AssignExpr& node) {
    // (AR) التحقق إذا كان الطرف الأيمن متغير (للنقل المحتمل)
    // (EN) Check if RHS is a variable (for potential move)
    std::string sourceVarName;
    auto* varExpr = dynamic_cast<VariableExpr*>(node.value.get());
    if (varExpr) {
        sourceVarName = varExpr->name;
    }
    
    // تقييم القيمة اليمنى / Evaluate right-hand side
    node.value->accept(*this);
    Value value = lastResult_;
    
    // (AR) التحقق إذا كان المتغير موجود / (EN) Check if variable exists
    // إذا لم يكن موجوداً، قم بتعريفه (استدلال النوع)
    // If not exists, define it (type inference)
    if (!variableManager_.exists(node.name)) {
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
        if (ownershipManager_.isEnabled()) {
            ownershipManager_.declareVariable(node.name);
            
            // (AR) نقل الملكية من المتغير المصدر (إن وُجد ولم يكن قابلاً للنسخ)
            // (EN) Move ownership from source variable (if exists and not copy type)
            if (!sourceVarName.empty() && sourceVarName != node.name) {
                auto moveError = ownershipManager_.moveVariable(sourceVarName);
                if (moveError.has_value()) {
                    throw SadException(
                        moveError->arabicMessage + " / " + moveError->message,
                        "OwnershipError",
                        node.position
                    );
                }
            }
        }
    } else {
        // (AR) فحص الملكية: هل يمكن التعديل؟ / (EN) Ownership check: can mutate?
        if (ownershipManager_.isEnabled()) {
            auto error = ownershipManager_.mutateVariable(node.name);
            if (error.has_value()) {
                throw SadException(
                    error->arabicMessage + " / " + error->message,
                    "OwnershipError",
                    node.position
                );
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

void ExpressionEvaluator::visitBinaryExpr(BinaryExpr& node) {
    // ═══════════════════════════════════════════════════════════════════
    // (AR) تقييم كسول للعمليات المنطقية && و || — لا يُقيّم الطرف الأيمن إلا عند الحاجة
    // (EN) Short-circuit evaluation for && and || — right-side evaluated only when needed
    // ═══════════════════════════════════════════════════════════════════
    if (node.op == TokenType::OP_AND) {
        node.left->accept(*this);
        Value left = std::move(lastResult_);
        if (!left.toBool()) {
            lastResult_ = Value(false);
            return;
        }
        node.right->accept(*this);
        lastResult_ = Value(lastResult_.toBool());
        return;
    }
    if (node.op == TokenType::OP_OR) {
        node.left->accept(*this);
        Value left = lastResult_;
        if (left.toBool()) {
            lastResult_ = Value(true);
            return;
        }
        node.right->accept(*this);
        lastResult_ = Value(lastResult_.toBool());
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
    if (left.isObject() || left.isObjectLike()) {
        std::string className = left.getClassName();
        if (!className.empty()) {
            auto* classManager = Data::ClassManager::getInstance();
            Data::ClassType* classType = classManager->getClass(className);
            if (classType) {
                // (AR) تحويل رمز العامل من TokenType إلى نص / (EN) Convert operator token to string
                std::string opSymbol;
                switch (node.op) {
                    case TokenType::OP_PLUS:          opSymbol = "+"; break;
                    case TokenType::OP_MINUS:         opSymbol = "-"; break;
                    case TokenType::OP_MULTIPLY:      opSymbol = "*"; break;
                    case TokenType::OP_DIVIDE:        opSymbol = "/"; break;
                    case TokenType::OP_MODULO:        opSymbol = "%"; break;
                    case TokenType::OP_POWER:         opSymbol = "**"; break;
                    case TokenType::OP_EQUAL:         opSymbol = "=="; break;
                    case TokenType::OP_NOT_EQUAL:     opSymbol = "!="; break;
                    case TokenType::OP_LESS:          opSymbol = "<"; break;
                    case TokenType::OP_LESS_EQUAL:    opSymbol = "<="; break;
                    case TokenType::OP_GREATER:       opSymbol = ">"; break;
                    case TokenType::OP_GREATER_EQUAL: opSymbol = ">="; break;
                    default: break;
                }
                
                if (!opSymbol.empty()) {
                    Data::OperatorOverload* opOverload = classType->findOperator(opSymbol);
                    if (opOverload && opOverload->body) {
                        // (AR) تنفيذ العامل المحمل زائداً / (EN) Execute operator overload
                        lastResult_ = executeOperatorOverload(left, *opOverload, right, node.position);
                        return;
                    }
                }
            }
        }
    }
    
    // تحديد نوع العملية / Determine operation type
    switch (node.op) {
        // (AR) عمليات حسابية / (EN) Arithmetic operations
        case TokenType::OP_PLUS:
        case TokenType::OP_MINUS:
        case TokenType::OP_MULTIPLY:
        case TokenType::OP_DIVIDE:
        case TokenType::OP_MODULO:
        case TokenType::OP_POWER:
            lastResult_ = evaluateArithmeticOp(left, node.op, right, node.position);
            break;
        
        // (AR) عمليات مقارنة / (EN) Comparison operations
        case TokenType::OP_EQUAL:
        case TokenType::OP_NOT_EQUAL:
        case TokenType::OP_LESS:
        case TokenType::OP_LESS_EQUAL:
        case TokenType::OP_GREATER:
        case TokenType::OP_GREATER_EQUAL:
            lastResult_ = evaluateComparisonOp(left, node.op, right, node.position);
            break;
        
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
            lastResult_ = evaluateBitwiseOp(left, node.op, right, node.position);
            break;

        // (AR) عامل العضوية: في / (EN) Membership operator: in
        case TokenType::KEYWORD_IN: {
            bool found = false;
            if (right.isArray()) {
                for (const auto& el : right.toArrayRef()) {
                    if ((left == el).toBool()) { found = true; break; }
                }
            } else if (right.isMap()) {
                std::string key = left.toString();
                auto map = right.toMap();
                found = (map.find(key) != map.end());
            } else if (right.isString()) {
                found = (right.toString().find(left.toString()) != std::string::npos);
            } else {
                throw RuntimeError(
                    "(AR) عامل 'في' يتطلب مصفوفة أو خريطة أو نص على اليمين، ولكن الموجود من نوع '" + right.getTypeName() + "'. "
                    "(EN) 'in' operator requires array, map, or string on right side, but got type '" + right.getTypeName() + "'.",
                    node.position
                );
            }
            lastResult_ = Value(found);
            break;
        }
        
        default: {
            // (AR) حساب رمز العامل للرسالة المحسنة
            // (EN) Compute operator symbol for improved message
            std::string opStr = "?";
            switch (node.op) {
                case TokenType::OP_PLUS: opStr = "+"; break;
                case TokenType::OP_MINUS: opStr = "-"; break;
                case TokenType::OP_MULTIPLY: opStr = "*"; break;
                case TokenType::OP_DIVIDE: opStr = "/"; break;
                case TokenType::OP_MODULO: opStr = "%"; break;
                case TokenType::OP_POWER: opStr = "**"; break;
                case TokenType::OP_XOR: opStr = "^"; break;
                default: break;
            }
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation(
                    Sad::Errors::ErrorManager::getInstance().getSourceFilename().empty() ? "<input>" : Sad::Errors::ErrorManager::getInstance().getSourceFilename(),
                    static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "العملية '" + opStr + "' غير مدعومة بين نوعي '" + left.getTypeName() + "' و '" + right.getTypeName() + "'",
                "Operation '" + opStr + "' not supported between types '" + left.getTypeName() + "' and '" + right.getTypeName() + "'"
            );
            lastResult_ = Value(); // Return null
        }
    }
}

// =========================================================================
// (AR) العمليات الحسابية / (EN) Arithmetic Operations
// =========================================================================

Value ExpressionEvaluator::evaluateArithmeticOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    // جمع النصوص (string concatenation) / String concatenation
    if (op == TokenType::OP_PLUS && (left.isString() || right.isString())) {
        return Value(left.toString() + right.toString());
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) عمليات المصفوفات: دمج (+) وتكرار (*)
    // (EN) Array operations: concatenation (+) and repetition (*)
    // ═══════════════════════════════════════════════════════════════════
    if (left.isArray() || right.isArray()) {
        if (op == TokenType::OP_PLUS) {
            // (AR) دمج المصفوفات: [1,2] + [3,4] → [1,2,3,4]
            //      أو إضافة عنصر: [1,2] + 3 → [1,2,3]
            return left + right;
        }
        if (op == TokenType::OP_MULTIPLY) {
            // (AR) تكرار المصفوفة: [1,2] * 3 → [1,2,1,2,1,2]
            return left * right;
        }
    }
    
    // (AR) تكرار النصوص: "ها" * 3 → "هاهاها"
    // (EN) String repetition: "ha" * 3 → "hahaha"
    if (op == TokenType::OP_MULTIPLY && (left.isString() || right.isString())) {
        return left * right;
    }
    
    // التأكد من أن الطرفين رقميين / Ensure both operands are numeric
    if (!left.isNumeric() || !right.isNumeric()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
            "العمليات الحسابية تتطلب قيم رقمية",
            "Arithmetic operations require numeric values"
        );
        return Value(0); // Return default
    }
    
    // تحويل لـ double إذا كان أحدهما double
    // Convert to double if either is double
    bool useDouble = left.isDouble() || right.isDouble();
    
    if (useDouble) {
        double l = left.toDouble();
        double r = right.toDouble();
        
        switch (op) {
            case TokenType::OP_PLUS:     return Value(l + r);
            case TokenType::OP_MINUS:    return Value(l - r);
            case TokenType::OP_MULTIPLY: return Value(l * r);
            case TokenType::OP_DIVIDE:
                if (r == 0.0) {
                    throw DivisionByZeroError(
                        "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero",
                        pos
                    );
                }
                return Value(l / r);
            case TokenType::OP_POWER:    return Value(std::pow(l, r));
            case TokenType::OP_MODULO:
                if (r == 0.0) {
                    throw DivisionByZeroError(
                        "(AR) لا يمكن القسمة على صفر (باقي القسمة) / (EN) Cannot modulo by zero",
                        pos
                    );
                }
                return Value(std::fmod(l, r));
            default: break;
        }
    } else {
        int l = left.toInt();
        int r = right.toInt();
        
        // (AR) حماية طفحان الأعداد الصحيحة — الترقية إلى double عند الطفحان
        // (EN) Integer overflow protection — promote to double on overflow
        auto safeAdd = [](int a, int b) -> Value {
            if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b))
                return Value(static_cast<double>(a) + static_cast<double>(b));
            return Value(a + b);
        };
        auto safeSub = [](int a, int b) -> Value {
            if ((b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b))
                return Value(static_cast<double>(a) - static_cast<double>(b));
            return Value(a - b);
        };
        auto safeMul = [](int a, int b) -> Value {
            if (a != 0 && b != 0) {
                if ((a > 0 && b > 0 && a > INT_MAX / b) ||
                    (a < 0 && b < 0 && a < INT_MAX / b) ||
                    (a > 0 && b < 0 && b < INT_MIN / a) ||
                    (a < 0 && b > 0 && a < INT_MIN / b))
                    return Value(static_cast<double>(a) * static_cast<double>(b));
            }
            return Value(a * b);
        };
        
        switch (op) {
            case TokenType::OP_PLUS:     return safeAdd(l, r);
            case TokenType::OP_MINUS:    return safeSub(l, r);
            case TokenType::OP_MULTIPLY: return safeMul(l, r);
            case TokenType::OP_DIVIDE:
                if (r == 0) {
                    throw DivisionByZeroError(
                        "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero",
                        pos
                    );
                }
                // (AR) ترقية إلى عشري عند وجود باقي (7/2 → 3.5) — سلوك القسمة الحقيقية
                // (EN) Promote to double when remainder exists (7/2 → 3.5) — true division
                if (l % r != 0)
                    return Value(static_cast<double>(l) / static_cast<double>(r));
                return Value(l / r);
            case TokenType::OP_MODULO:
                if (r == 0) {
                    throw DivisionByZeroError(
                        "(AR) لا يمكن حساب باقي القسمة على صفر / (EN) Cannot modulo by zero",
                        pos
                    );
                }
                return Value(l % r);
            case TokenType::OP_POWER: {
                double result = std::pow(static_cast<double>(l), static_cast<double>(r));
                if (result >= static_cast<double>(INT_MIN) && result <= static_cast<double>(INT_MAX))
                    return Value(static_cast<int>(result));
                return Value(result);
            }
            default: break;
        }
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
        Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
        "عملية حسابية غير مدعومة",
        "Unsupported arithmetic operation"
    );
    return Value(0); // Return default
}

// =========================================================================
// (AR) عمليات المقارنة / (EN) Comparison Operations
// =========================================================================

Value ExpressionEvaluator::evaluateComparisonOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    // المقارنة تعمل على أي نوعين / Comparison works on any two types
    
    // معالجة خاصة لـ null (VOID) / Special handling for null (VOID)
    // null يمكن مقارنته بأي نوع / null can be compared with any type
    if (left.isVoid() || right.isVoid()) {
        switch (op) {
            case TokenType::OP_EQUAL:
                return Value(left.isVoid() && right.isVoid());
            case TokenType::OP_NOT_EQUAL:
                return Value(!left.isVoid() || !right.isVoid());
            default:
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
                    "لا يمكن استخدام عمليات المقارنة (<، >، <=، >=) مع null. استخدم == أو != فقط",
                    "Cannot use comparison operators (<, >, <=, >=) with null. Use == or != only"
                );
                return Value(false);
        }
    }
    
    // (AR) مقارنة الأنواع الرقمية المختلطة (int و double) — ترقية كلاهما إلى double
    // (EN) Mixed numeric comparison (int and double) — promote both to double
    if (left.isNumeric() && right.isNumeric() && left.getType() != right.getType()) {
        double l = left.toDouble();
        double r = right.toDouble();
        switch (op) {
            case TokenType::OP_EQUAL:         return Value(l == r);
            case TokenType::OP_NOT_EQUAL:     return Value(l != r);
            case TokenType::OP_LESS:          return Value(l < r);
            case TokenType::OP_LESS_EQUAL:    return Value(l <= r);
            case TokenType::OP_GREATER:       return Value(l > r);
            case TokenType::OP_GREATER_EQUAL: return Value(l >= r);
            default: break;
        }
    }
    
    // مقارنة الأنواع المختلفة / Different types comparison
    if (left.getType() != right.getType()) {
        // فقط == و != مسموح بهما / Only == and != allowed
        if (op == TokenType::OP_EQUAL) return Value(false);
        if (op == TokenType::OP_NOT_EQUAL) return Value(true);
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
            "لا يمكن مقارنة أنواع مختلفة",
            "Cannot compare different types"
        );
        return Value(false); // Return default
    }
    
    // مقارنة الأعداد / Numeric comparison
    if (left.isNumeric() && right.isNumeric()) {
        double l = left.toDouble();
        double r = right.toDouble();
        
        switch (op) {
            case TokenType::OP_EQUAL:         return Value(l == r);
            case TokenType::OP_NOT_EQUAL:     return Value(l != r);
            case TokenType::OP_LESS:          return Value(l < r);
            case TokenType::OP_LESS_EQUAL:    return Value(l <= r);
            case TokenType::OP_GREATER:       return Value(l > r);
            case TokenType::OP_GREATER_EQUAL: return Value(l >= r);
            default: break;
        }
    }
    
    // مقارنة النصوص / String comparison
    // NOTE(#20): (AR) المقارنة تتم على مستوى البايتات بدون تطبيع Unicode (NFC/NFD).
    //            هذا يعني أن نفس الحرف العربي مع تشكيل مختلف قد لا يتطابق.
    // NOTE(#20): (EN) Comparison is byte-level without Unicode normalization (NFC/NFD).
    //            This means the same Arabic character with different diacritics encoding may not match.
    if (left.isString() && right.isString()) {
        std::string l = left.toString();
        std::string r = right.toString();
        
        switch (op) {
            case TokenType::OP_EQUAL:         return Value(l == r);
            case TokenType::OP_NOT_EQUAL:     return Value(l != r);
            case TokenType::OP_LESS:          return Value(l < r);
            case TokenType::OP_LESS_EQUAL:    return Value(l <= r);
            case TokenType::OP_GREATER:       return Value(l > r);
            case TokenType::OP_GREATER_EQUAL: return Value(l >= r);
            default: break;
        }
    }
    
    // مقارنة Boolean / Boolean comparison
    if (left.isBoolean() && right.isBoolean()) {
        bool l = left.toBool();
        bool r = right.toBool();
        
        switch (op) {
            case TokenType::OP_EQUAL:     return Value(l == r);
            case TokenType::OP_NOT_EQUAL: return Value(l != r);
            default:
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
                    "فقط == و != مسموح بهما للقيم المنطقية",
                    "Only == and != allowed for boolean values"
                );
                return Value(false);
        }
    }
    
    // (AR) مقارنة عميقة للمصفوفات / (EN) Deep array comparison
    if (left.isArray() && right.isArray()) {
        if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL) {
            const auto& lArr = left.toArrayRef();
            const auto& rArr = right.toArrayRef();
            bool equal = (lArr.size() == rArr.size());
            if (equal) {
                for (size_t i = 0; i < lArr.size(); ++i) {
                    Value cmp = evaluateComparisonOp(lArr[i], TokenType::OP_EQUAL, rArr[i], pos);
                    if (!cmp.toBool()) { equal = false; break; }
                }
            }
            return Value(op == TokenType::OP_EQUAL ? equal : !equal);
        }
    }

    // (AR) مقارنة عميقة للخرائط / (EN) Deep map comparison
    if (left.isMap() && right.isMap()) {
        if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL) {
            const auto& lMap = left.toMap();
            const auto& rMap = right.toMap();
            bool equal = (lMap.size() == rMap.size());
            if (equal) {
                for (const auto& [k, v] : lMap) {
                    auto it = rMap.find(k);
                    if (it == rMap.end()) { equal = false; break; }
                    Value cmp = evaluateComparisonOp(v, TokenType::OP_EQUAL, it->second, pos);
                    if (!cmp.toBool()) { equal = false; break; }
                }
            }
            return Value(op == TokenType::OP_EQUAL ? equal : !equal);
        }
    }
    
    // (AR) مقارنة مراجع الدوال / (EN) Function reference comparison
    if (left.isFunction() && right.isFunction()) {
        if (op == TokenType::OP_EQUAL || op == TokenType::OP_NOT_EQUAL) {
            Value eqResult = (left == right);
            bool equal = eqResult.toBool();
            return Value(op == TokenType::OP_EQUAL ? equal : !equal);
        }
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
        Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
        "عملية مقارنة غير مدعومة",
        "Unsupported comparison operation"
    );
    return Value(false);
}

// =========================================================================
// (AR) العمليات المنطقية / (EN) Logical Operations
// =========================================================================

Value ExpressionEvaluator::evaluateLogicalOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    bool l = left.toBool();
    bool r = right.toBool();
    
    switch (op) {
        case TokenType::OP_AND: return Value(l && r);
        case TokenType::OP_OR:  return Value(l || r);
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
                "عملية منطقية غير مدعومة",
                "Unsupported logical operation"
            );
            return Value(false);
    }
}

// =========================================================================
// (AR) عمليات البت / (EN) Bitwise Operations
// =========================================================================

Value ExpressionEvaluator::evaluateBitwiseOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    if (!left.isNumeric() || !right.isNumeric()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
            "عمليات البت تتطلب قيم صحيحة",
            "Bitwise operations require integer values"
        );
        return Value(0);
    }

    int l = left.toInt();
    int r = right.toInt();

    switch (op) {
        case TokenType::OP_XOR:          return Value(l ^ r);
        case TokenType::OP_BITWISE_AND:  return Value(l & r);
        case TokenType::OP_BITWISE_OR:   return Value(l | r);
        case TokenType::OP_SHIFT_LEFT: {
            // (AR) حماية من إزاحة بقيم خطيرة (UB في C++)
            // (EN) Protect against dangerous shift values (UB in C++)
            if (r < 0 || r >= 32) {
                throw RuntimeError(
                    "(AR) خطأ: إزاحة بقيمة " + std::to_string(r) + " غير صالحة. يجب أن تكون بين 0 و 31.\n"
                    "(EN) Error: Shift amount " + std::to_string(r) + " is invalid. Must be between 0 and 31.",
                    pos
                );
            }
            return Value(l << r);
        }
        case TokenType::OP_SHIFT_RIGHT: {
            if (r < 0 || r >= 32) {
                throw RuntimeError(
                    "(AR) خطأ: إزاحة بقيمة " + std::to_string(r) + " غير صالحة. يجب أن تكون بين 0 و 31.\n"
                    "(EN) Error: Shift amount " + std::to_string(r) + " is invalid. Must be between 0 and 31.",
                    pos
                );
            }
            return Value(l >> r);
        }
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation(getSourceFilename(), pos.line, pos.column),
                "عملية بت غير مدعومة",
                "Unsupported bitwise operation"
            );
            return Value(0);
    }
}

// =========================================================================
// (AR) تنفيذ عامل محمل زائداً / (EN) Operator Overload Execution
// =========================================================================

Value ExpressionEvaluator::executeOperatorOverload(const Value& left, Data::OperatorOverload& overload, const Value& right, const Lexer::Position& pos) {
    // (AR) التحقق من عدد المعاملات / (EN) Verify parameter count
    if (overload.parameters.size() != 1) {
        throw RuntimeError(
            "(AR) العامل '" + overload.operatorSymbol + "' يجب أن يقبل معاملاً واحداً بالضبط. "
            "(EN) Operator '" + overload.operatorSymbol + "' must accept exactly one parameter.",
            pos
        );
    }
    
    // (AR) إنشاء نطاق جديد لتنفيذ العامل / (EN) Create new scope for operator execution
    variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator" + overload.operatorSymbol);
    
    // (AR) ربط 'هذا' بالكائن الأيسر / (EN) Bind 'this' to left object
    variableManager_.define("هذا", left);
    
    // (AR) ربط حقول الكائن الأيسر كمتغيرات محلية (محاكاة this.field → field)
    // (EN) Bind left object fields as local variables (simulate this.field → field)
    if (left.isObject()) {
        // ═══════════════════════════════════════════════════════════════
        // (AR) كائن حقيقي — نربط حقوله من ObjectInstance مباشرة
        // (EN) Real OBJECT — bind fields from ObjectInstance directly
        // ═══════════════════════════════════════════════════════════════
        auto objPtr = left.toObject();
        if (objPtr) {
            for (const auto& [fieldName, fieldValue] : objPtr->fields) {
                variableManager_.define(fieldName, fieldValue);
            }
        }
    } else if (left.isMap()) {
        // (AR) MAP قديم مع __class__ / (EN) Legacy MAP with __class__
        auto fields = left.toMap();
        for (const auto& [fieldName, fieldValue] : fields) {
            if (fieldName != "__class__") {
                variableManager_.define(fieldName, fieldValue);
            }
        }
    }
    
    // (AR) ربط المعامل الأيمن / (EN) Bind right operand
    variableManager_.define(overload.parameters[0].name, right);
    
    // (AR) تنفيذ جسم العامل / (EN) Execute operator body
    Value returnValue;
    try {
        overload.body->accept(statementExecutor_);
        
        if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
            returnValue = statementExecutor_.getReturnValue();
            statementExecutor_.resetFlowControl();
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) كتابة الحقول المحدثة إلى الكائن الأصلي (لدعم العوامل المتغيرة)
        //      مثل: عامل += قد يعدل حقول الكائن
        //
        // (EN) Write back updated fields to original object (for mutating operators)
        //      e.g.: operator += may modify object fields
        // ═══════════════════════════════════════════════════════════════
        if (left.isObject()) {
            auto objPtr = left.toObject();
            if (objPtr) {
                for (auto& [fieldName, fieldValue] : objPtr->fields) {
                    if (variableManager_.exists(fieldName)) {
                        Value updated = variableManager_.get(fieldName);
                        objPtr->fields[fieldName] = updated;
                    }
                }
            }
        }
    } catch (...) {
        variableManager_.exitScope();
        throw;
    }
    
    variableManager_.exitScope();
    return returnValue;
}

// ======================================================================
// (AR) زوار التوجيهات التعبيرية @ — Expression Directive Visitors
// ======================================================================

/**
 * @brief (AR) @حجم(نوع) — يُرجع حجم النوع بالبايتات
 * @brief (EN) @sizeof(type) — returns size of type in bytes
 */
void ExpressionEvaluator::visitSizeofExpr(AST::SizeofExpr& expr) {
    // (AR) خريطة أحجام الأنواع الأساسية
    // (EN) Basic type size map
    static const std::unordered_map<std::string, int64_t> typeSizes = {
        {"رقم", 8},      // int64_t = 8 bytes
        {"عشري", 8},     // double = 8 bytes
        {"نص", 32},      // std::string (approx)
        {"منطقي", 1},    // bool = 1 byte
        {"فراغ", 0},     // void = 0
        {"عدم", 0},      // null = 0
        {"مصفوفة", 24},  // vector (approx)
        {"خريطة", 48},   // map (approx)
        {"أي", 72},      // Value variant (approx)
        // Sized integer types
        {"u8", 1}, {"i8", 1},
        {"u16", 2}, {"i16", 2},
        {"u32", 4}, {"i32", 4},
        {"u64", 8}, {"i64", 8},
        {"usize", 8}, {"isize", 8},
        {"ptr", 8},
    };

    auto it = typeSizes.find(expr.typeName);
    if (it != typeSizes.end()) {
        lastResult_ = Data::Value(it->second);
    } else {
        // (AR) نوع غير معروف — نرجع 0
        // (EN) Unknown type — return 0
        lastResult_ = Data::Value(static_cast<int64_t>(0));
    }
}

/**
 * @brief (AR) @ذري(عملية, ...) — عمليات ذرية
 * @brief (EN) @atomic(op, ...) — atomic operations
 * 
 * (AR) في المفسر: نحاكي العمليات الذرية بدون قفل حقيقي
 *      لأن المفسر أحادي الخيط. القيمة الحقيقية تكون في المترجم.
 * (EN) In interpreter: simulate atomic ops without real locking
 *      because interpreter is single-threaded. Real value is in compiler.
 */
void ExpressionEvaluator::visitAtomicExpr(AST::AtomicExpr& expr) {
    const std::string& op = expr.operation;
    
    if (op == "تحميل" || op == "load") {
        // @ذري(تحميل, متغير) — قراءة ذرية
        if (!expr.operands.empty()) {
            expr.operands[0]->accept(*this);
            // lastResult_ already set
        } else {
            lastResult_ = Data::Value(static_cast<int64_t>(0));
        }
    }
    else if (op == "تخزين" || op == "store") {
        // @ذري(تخزين, متغير, قيمة) — كتابة ذرية
        if (expr.operands.size() >= 2) {
            // (AR) نحسب القيمة
            expr.operands[1]->accept(*this);
            Data::Value val = lastResult_;
            
            // (AR) نحصل على اسم المتغير ونعيّنه
            if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr.operands[0].get())) {
                variableManager_.assign(varExpr->name, std::move(val));
            }
            lastResult_ = Data::Value(static_cast<int64_t>(0));
        }
    }
    else if (op == "إضافة" || op == "add") {
        // @ذري(إضافة, متغير, قيمة) — إضافة ذرية
        if (expr.operands.size() >= 2) {
            expr.operands[0]->accept(*this);
            Data::Value current = lastResult_;
            expr.operands[1]->accept(*this);
            Data::Value addend = lastResult_;
            
            if (current.isInteger() && addend.isInteger()) {
                int64_t newVal = current.toInt64() + addend.toInt64();
                lastResult_ = Data::Value(newVal);
                
                // (AR) تحديث المتغير
                if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr.operands[0].get())) {
                    variableManager_.assign(varExpr->name, Data::Value(newVal));
                }
            }
        }
    }
    else if (op == "طرح" || op == "sub") {
        // @ذري(طرح, متغير, قيمة) — طرح ذري
        if (expr.operands.size() >= 2) {
            expr.operands[0]->accept(*this);
            Data::Value current = lastResult_;
            expr.operands[1]->accept(*this);
            Data::Value subtrahend = lastResult_;
            
            if (current.isInteger() && subtrahend.isInteger()) {
                int64_t newVal = current.toInt64() - subtrahend.toInt64();
                lastResult_ = Data::Value(newVal);
                
                if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr.operands[0].get())) {
                    variableManager_.assign(varExpr->name, Data::Value(newVal));
                }
            }
        }
    }
    else if (op == "مقارنة_وتبديل" || op == "compare_and_swap" || op == "cas") {
        // @ذري(مقارنة_وتبديل, متغير, متوقع, جديد)
        if (expr.operands.size() >= 3) {
            expr.operands[0]->accept(*this);
            Data::Value current = lastResult_;
            expr.operands[1]->accept(*this);
            Data::Value expected = lastResult_;
            expr.operands[2]->accept(*this);
            Data::Value newVal = lastResult_;
            
            // (AR) مقارنة وتبديل: إذا القيمة الحالية == المتوقعة، نضع الجديدة
            if (current.isInteger() && expected.isInteger() && newVal.isInteger()) {
                if (current.toInt64() == expected.toInt64()) {
                    if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr.operands[0].get())) {
                        variableManager_.assign(varExpr->name, Data::Value(newVal.toInt64()));
                    }
                    lastResult_ = Data::Value(true); // (AR) نجح / (EN) succeeded
                } else {
                    lastResult_ = Data::Value(false); // (AR) فشل / (EN) failed
                }
            }
        }
    }
    else {
        // (AR) عملية ذرية غير معروفة
        lastResult_ = Data::Value(static_cast<int64_t>(0));
    }
}

} // namespace Interpreter
} // namespace Sad
