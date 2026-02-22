/**
 * @file pipeline_parser.cpp
 * @brief (AR) محلل عامل الأنبوب (|>)
 *        (EN) Pipeline Operator Parser
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║    ██████╗ ██╗██████╗ ███████╗██╗     ██╗███╗   ██╗███████╗                   ║
 * ║    ██╔══██╗██║██╔══██╗██╔════╝██║     ██║████╗  ██║██╔════╝                   ║
 * ║    ██████╔╝██║██████╔╝█████╗  ██║     ██║██╔██╗ ██║█████╗                     ║
 * ║    ██╔═══╝ ██║██╔═══╝ ██╔══╝  ██║     ██║██║╚██╗██║██╔══╝                     ║
 * ║    ██║     ██║██║     ███████╗███████╗██║██║ ╚████║███████╗                   ║
 * ║    ╚═╝     ╚═╝╚═╝     ╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝╚══════╝                   ║
 * ║                                                                               ║
 * ║              محلل عامل الأنبوب للبرمجة الدالية                                ║
 * ║              Pipeline Operator for Functional Programming                     ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يحلل عامل الأنبوب (|>) للبرمجة الدالية:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                    عامل الأنبوب                                         │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // الصيغة الأساسية                                                   │
 *      │    قيمة |> دالة                                                         │
 *      │    // تكافئ: دالة(قيمة)                                                 │
 *      │                                                                         │
 *      │    // سلسلة أنابيب                                                       │
 *      │    قيمة |> دالة1 |> دالة2 |> دالة3                                       │
 *      │    // تكافئ: دالة3(دالة2(دالة1(قيمة)))                                   │
 *      │                                                                         │
 *      │    // مع معاملات إضافية                                                 │
 *      │    قيمة |> دالة(_, معامل2)                                              │
 *      │    // تكافئ: دالة(قيمة، معامل2)                                         │
 *      │                                                                         │
 *      │    // مع دوال مجهولة                                                     │
 *      │    قائمة |> تصفية(|س| س > 5) |> تحويل(|س| س²)                          │
 *      │                                                                         │
 *      │    // مثال عملي                                                         │
 *      │    أرقام                                                                │
 *      │        |> تصفية(|ر| ر % 2 == 0)     // الأرقام الزوجية                  │
 *      │        |> تحويل(|ر| ر * 2)          // مضاعفة                           │
 *      │        |> أخذ(5)                     // أول 5                           │
 *      │        |> إلى_قائمة()                // تحويل لقائمة                    │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "pipeline_parser.hpp"
#include "../include/parser.hpp"
#include "../../ast/include/ast.hpp"
#include "../../ast/include/comprehension_ast.hpp"
#include "../../lexer/include/token.hpp"
#include "../../errors/include/error_reporter.hpp"

#include <memory>
#include <vector>
#include <string>

namespace sad::parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) الرموز
//                    (EN) Tokens
// ═══════════════════════════════════════════════════════════════════════════════

namespace pipeline_tokens {
    constexpr const char* PIPE = "|>";
    constexpr const char* PLACEHOLDER = "_";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع عمليات الأنبوب
//                    (EN) Pipeline Operation Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع عملية الأنبوب
 */
enum class PipelineOpKind {
    FUNCTION_CALL,      // (AR) استدعاء دالة عادي
    PARTIAL_CALL,       // (AR) استدعاء جزئي مع _
    METHOD_CALL,        // (AR) استدعاء طريقة .method()
    CLOSURE,            // (AR) دالة مجهولة
    AWAIT               // (AR) انتظر مستقبل
};

/**
 * (AR) عقدة عملية الأنبوب
 */
struct PipelineOperationNode : public ast::ExpressionNode {
    PipelineOpKind kind;
    std::string function_name;
    std::unique_ptr<ast::ExpressionNode> callee;
    std::vector<std::unique_ptr<ast::ExpressionNode>> arguments;
    int placeholder_position;   // (AR) موضع _ في المعاملات (-1 إذا لم يوجد)
    
    PipelineOperationNode() 
        : kind(PipelineOpKind::FUNCTION_CALL)
        , placeholder_position(-1) {}
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::PipelineOperation;
    }
    
    std::string toString() const override {
        return "PipeOp(" + function_name + ")";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل الأنبوب
//                    (EN) Pipeline Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) فئة محلل الأنبوب
 */
class PipelineParser {
private:
    Parser& parser_;
    errors::ErrorReporter& error_reporter_;
    
public:
    PipelineParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser)
        , error_reporter_(reporter)
    {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) كشف الأنبوب
    //                    (EN) Pipeline Detection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل الرمز الحالي عامل أنبوب؟
     */
    bool isPipelineOperator() const {
        if (!parser_.currentToken().is(lexer::TokenType::PIPE)) {
            return false;
        }
        
        // (AR) تحقق من > بعد |
        auto next = parser_.peekToken();
        return next.is(lexer::TokenType::GREATER);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل الأنبوب
    //                    (EN) Pipeline Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل تعبير الأنبوب الكامل
     * 
     * يبدأ بعد تحليل التعبير الأيسر
     */
    std::unique_ptr<ast::PipelineNode> parsePipeline(
        std::unique_ptr<ast::ExpressionNode> source
    ) {
        auto node = std::make_unique<ast::PipelineNode>();
        node->source = std::move(source);
        node->location = parser_.currentToken().location;
        
        while (isPipelineOperator()) {
            // (AR) استهلك |>
            consumePipelineOperator();
            
            // (AR) تحليل العملية التالية
            auto op = parsePipelineOperation();
            if (!op) {
                return nullptr;
            }
            
            node->operations.push_back(std::move(op));
        }
        
        if (node->operations.empty()) {
            error_reporter_.error(
                node->location,
                "توقعت عملية بعد '|>'",
                "Expected operation after '|>'"
            );
            return nullptr;
        }
        
        return node;
    }
    
    /**
     * (AR) تحليل تعبير أنبوب من البداية
     */
    std::unique_ptr<ast::ExpressionNode> parsePipelineExpression() {
        // (AR) تحليل المصدر
        auto source = parser_.parseUnaryExpression();
        if (!source) {
            return nullptr;
        }
        
        // (AR) إذا لم يكن هناك |>، أرجع المصدر
        if (!isPipelineOperator()) {
            return source;
        }
        
        return parsePipeline(std::move(source));
    }
    
private:
    /**
     * (AR) استهلاك عامل الأنبوب
     */
    void consumePipelineOperator() {
        // (AR) استهلك |
        if (parser_.currentToken().is(lexer::TokenType::PIPE)) {
            parser_.advance();
        }
        // (AR) استهلك >
        if (parser_.currentToken().is(lexer::TokenType::GREATER)) {
            parser_.advance();
        }
    }
    
    /**
     * (AR) تحليل عملية واحدة في الأنبوب
     */
    std::unique_ptr<ast::ExpressionNode> parsePipelineOperation() {
        // (AR) تحقق من await
        if (parser_.currentToken().text == "انتظر" ||
            parser_.currentToken().text == "await") {
            return parseAwaitOperation();
        }
        
        // (AR) تحليل اسم الدالة أو تعبير
        auto expr = parser_.parsePrimaryExpression();
        if (!expr) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت دالة بعد '|>'",
                "Expected function after '|>'"
            );
            return nullptr;
        }
        
        // (AR) إذا كان هناك قوس، فهو استدعاء
        if (parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            return parsePartialCall(std::move(expr));
        }
        
        // (AR) إذا كان هناك نقطة، فهو استدعاء طريقة
        if (parser_.currentToken().is(lexer::TokenType::DOT)) {
            return parseMethodChain(std::move(expr));
        }
        
        // (AR) افترض أنه استدعاء دالة بدون أقواس
        // (AR) القيمة من الأنبوب ستكون المعامل الأول
        return createImplicitCall(std::move(expr));
    }
    
    /**
     * (AR) تحليل استدعاء جزئي مع _
     */
    std::unique_ptr<ast::ExpressionNode> parsePartialCall(
        std::unique_ptr<ast::ExpressionNode> callee
    ) {
        auto call = std::make_unique<ast::CallExprNode>();
        call->callee = std::move(callee);
        
        // (AR) استهلك (
        parser_.advance();
        
        int placeholder_pos = -1;
        int arg_index = 0;
        
        while (!parser_.currentToken().is(lexer::TokenType::RPAREN) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            // (AR) تحقق من _
            if (parser_.currentToken().text == pipeline_tokens::PLACEHOLDER) {
                if (placeholder_pos >= 0) {
                    error_reporter_.error(
                        parser_.currentToken().location,
                        "يمكن استخدام '_' مرة واحدة فقط",
                        "Can only use '_' once"
                    );
                }
                placeholder_pos = arg_index;
                parser_.advance();
                
                // (AR) أضف placeholder وهمي
                call->arguments.push_back(nullptr);
            } else {
                auto arg = parser_.parseExpression();
                if (!arg) {
                    return nullptr;
                }
                call->arguments.push_back(std::move(arg));
            }
            
            arg_index++;
            
            if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                parser_.advance();
            } else {
                break;
            }
        }
        
        // (AR) استهلك )
        if (!parser_.expectToken(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')'",
                "Expected ')'"
            );
            return nullptr;
        }
        
        // (AR) إذا لم يكن هناك _، أضف القيمة كأول معامل
        if (placeholder_pos < 0) {
            // (AR) أدخل nullptr في البداية كـ placeholder
            call->arguments.insert(call->arguments.begin(), nullptr);
            placeholder_pos = 0;
        }
        
        // (AR) خزن موضع الـ placeholder
        call->placeholder_position = placeholder_pos;
        
        return call;
    }
    
    /**
     * (AR) تحليل سلسلة طرق
     */
    std::unique_ptr<ast::ExpressionNode> parseMethodChain(
        std::unique_ptr<ast::ExpressionNode> object
    ) {
        auto current = std::move(object);
        
        while (parser_.currentToken().is(lexer::TokenType::DOT)) {
            parser_.advance();
            
            if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت اسم طريقة",
                    "Expected method name"
                );
                return nullptr;
            }
            
            auto method_call = std::make_unique<ast::MethodCallNode>();
            method_call->object = std::move(current);
            method_call->method_name = parser_.currentToken().text;
            parser_.advance();
            
            // (AR) تحليل المعاملات إذا وجدت
            if (parser_.currentToken().is(lexer::TokenType::LPAREN)) {
                parser_.advance();
                
                while (!parser_.currentToken().is(lexer::TokenType::RPAREN) &&
                       !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
                    auto arg = parser_.parseExpression();
                    if (arg) {
                        method_call->arguments.push_back(std::move(arg));
                    }
                    
                    if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                        parser_.advance();
                    } else {
                        break;
                    }
                }
                
                parser_.expectToken(lexer::TokenType::RPAREN);
            }
            
            current = std::move(method_call);
        }
        
        return current;
    }
    
    /**
     * (AR) إنشاء استدعاء ضمني
     */
    std::unique_ptr<ast::ExpressionNode> createImplicitCall(
        std::unique_ptr<ast::ExpressionNode> callee
    ) {
        auto call = std::make_unique<ast::CallExprNode>();
        call->callee = std::move(callee);
        call->placeholder_position = 0;
        call->arguments.push_back(nullptr); // (AR) placeholder للقيمة من الأنبوب
        return call;
    }
    
    /**
     * (AR) تحليل await في الأنبوب
     */
    std::unique_ptr<ast::ExpressionNode> parseAwaitOperation() {
        auto await = std::make_unique<ast::AwaitExprNode>();
        parser_.advance(); // (AR) استهلك 'انتظر'
        
        // (AR) القيمة من الأنبوب ستكون المستقبل
        await->inner = nullptr; // (AR) سيُملأ لاحقاً
        
        return await;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) تحويل الأنبوب إلى استدعاءات
//                    (EN) Pipeline Desugaring
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) تحويل تعبير الأنبوب إلى استدعاءات متداخلة
 */
class PipelineDesugarer {
public:
    /**
     * (AR) تحويل الأنبوب
     * 
     * أ |> ب |> ج
     * يتحول إلى:
     * ج(ب(أ))
     */
    std::unique_ptr<ast::ExpressionNode> desugar(const ast::PipelineNode* pipeline) {
        std::unique_ptr<ast::ExpressionNode> current = pipeline->source->clone();
        
        for (const auto& op : pipeline->operations) {
            current = applyOperation(std::move(current), op.get());
        }
        
        return current;
    }
    
private:
    std::unique_ptr<ast::ExpressionNode> applyOperation(
        std::unique_ptr<ast::ExpressionNode> value,
        const ast::ExpressionNode* op
    ) {
        // (AR) إذا كان استدعاء دالة مع placeholder
        if (auto* call = dynamic_cast<const ast::CallExprNode*>(op)) {
            auto new_call = std::make_unique<ast::CallExprNode>();
            new_call->callee = call->callee->clone();
            
            for (size_t i = 0; i < call->arguments.size(); ++i) {
                if (static_cast<int>(i) == call->placeholder_position) {
                    new_call->arguments.push_back(std::move(value));
                } else if (call->arguments[i]) {
                    new_call->arguments.push_back(call->arguments[i]->clone());
                }
            }
            
            return new_call;
        }
        
        // (AR) إذا كان await
        if (auto* await = dynamic_cast<const ast::AwaitExprNode*>(op)) {
            auto new_await = std::make_unique<ast::AwaitExprNode>();
            new_await->inner = std::move(value);
            return new_await;
        }
        
        // (AR) استدعاء عادي
        auto call = std::make_unique<ast::CallExprNode>();
        call->callee = op->clone();
        call->arguments.push_back(std::move(value));
        return call;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) إنشاء محلل الأنبوب
 */
std::unique_ptr<PipelineParser> createPipelineParser(
    Parser& parser,
    errors::ErrorReporter& reporter
) {
    return std::make_unique<PipelineParser>(parser, reporter);
}

/**
 * (AR) إنشاء محول الأنبوب
 */
std::unique_ptr<PipelineDesugarer> createPipelineDesugarer() {
    return std::make_unique<PipelineDesugarer>();
}

/**
 * (AR) هل النص عامل أنبوب؟
 */
bool isPipelineOperator(const std::string& text) {
    return text == pipeline_tokens::PIPE;
}

} // namespace sad::parser
