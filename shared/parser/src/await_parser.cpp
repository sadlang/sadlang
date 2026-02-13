/**
 * @file await_parser.cpp
 * @brief (AR) محلل تعبير الانتظار (await)
 *        (EN) Await Expression Parser
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██████╗ ██╗    ██╗ ██████╗ ██╗████████╗                                   ║
 * ║    ██╔══██╗██║    ██║██╔══██╗██║╚══██╔══╝                                   ║
 * ║    ███████║██║ █╗ ██║███████║██║   ██║                                      ║
 * ║    ██╔══██║██║███╗██║██╔══██║██║   ██║                                      ║
 * ║    ██║  ██║╚███╔███╔╝██║  ██║██║   ██║                                      ║
 * ║    ╚═╝  ╚═╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝   ╚═╝                                      ║
 * ║                                                                               ║
 * ║              محلل تعبير الانتظار - انتظر                                     ║
 * ║              Await Expression Parser for Arabic                               ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يحلل تعبير 'انتظر' (await) بصيغ متعددة:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     صيغ استخدام 'انتظر'                                 │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // الصيغة الأساسية                                                   │
 *      │    دع نتيجة = انتظر دالة_غير_متزامنة()؛                                 │
 *      │                                                                         │
 *      │    // مع معالج الأخطاء                                                  │
 *      │    دع نتيجة = انتظر دالة_غير_متزامنة()؟؛                               │
 *      │                                                                         │
 *      │    // انتظار متسلسل                                                     │
 *      │    دع أ = انتظر جلب_أ()؛                                                │
 *      │    دع ب = انتظر جلب_ب()؛                                                │
 *      │                                                                         │
 *      │    // انتظار متوازي                                                     │
 *      │    دع (أ، ب) = انتظر انضم!(جلب_أ()، جلب_ب())؛                          │
 *      │                                                                         │
 *      │    // انتظار مع مهلة                                                    │
 *      │    دع نتيجة = انتظر مهلة(جلب_بيانات()، 5.ثانية)؟؛                      │
 *      │                                                                         │
 *      │    // انتظار السباق (الأول ينتصر)                                       │
 *      │    دع أول = انتظر سباق!(مصدر1()، مصدر2())؛                            │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "await_parser.hpp"
#include "../include/parser.hpp"
#include "../../ast/include/ast.hpp"
#include "../../lexer/include/token.hpp"
#include "../../errors/include/error_reporter.hpp"

#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <variant>

namespace sad::parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع تعبير الانتظار
//                    (EN) Await Expression Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع الانتظار
 */
enum class AwaitKind {
    SIMPLE,         // (AR) انتظار بسيط: انتظر تعبير
    CHAINED,        // (AR) انتظار متسلسل: انتظر أ.ب.ج
    TRY_AWAIT,      // (AR) انتظار مع محاولة: انتظر تعبير؟
    JOIN,           // (AR) انتظار متوازي: انتظر انضم!(...)
    RACE,           // (AR) انتظار سباق: انتظر سباق!(...)
    TIMEOUT,        // (AR) انتظار مع مهلة: انتظر مهلة(...)
    SELECT          // (AR) انتظار انتقائي: انتظر اختر! { ... }
};

/**
 * (AR) معلومات تعبير الانتظار
 */
struct AwaitInfo {
    AwaitKind kind;
    lexer::SourceLocation location;
    bool is_try;            // (AR) هل يستخدم ؟
    bool is_unsafe;         // (AR) هل داخل كتلة غير آمنة
    int precedence;         // (AR) أولوية العامل
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقد AST للانتظار
//                    (EN) Await AST Nodes
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة الانتظار البسيط
 */
struct SimpleAwaitNode : public ast::ExpressionNode {
    std::unique_ptr<ast::ExpressionNode> future_expr;
    AwaitInfo info;
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::AwaitExpr;
    }
    
    std::string toString() const override {
        return "SimpleAwait(" + (future_expr ? future_expr->toString() : "null") + ")";
    }
};

/**
 * (AR) عقدة الانتظار المتوازي (join)
 */
struct JoinAwaitNode : public ast::ExpressionNode {
    std::vector<std::unique_ptr<ast::ExpressionNode>> futures;
    AwaitInfo info;
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::JoinAwaitExpr;
    }
    
    std::string toString() const override {
        return "JoinAwait(count=" + std::to_string(futures.size()) + ")";
    }
};

/**
 * (AR) عقدة انتظار السباق
 */
struct RaceAwaitNode : public ast::ExpressionNode {
    std::vector<std::unique_ptr<ast::ExpressionNode>> futures;
    AwaitInfo info;
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::RaceAwaitExpr;
    }
    
    std::string toString() const override {
        return "RaceAwait(count=" + std::to_string(futures.size()) + ")";
    }
};

/**
 * (AR) عقدة الانتظار مع مهلة
 */
struct TimeoutAwaitNode : public ast::ExpressionNode {
    std::unique_ptr<ast::ExpressionNode> future_expr;
    std::unique_ptr<ast::ExpressionNode> timeout_expr;
    AwaitInfo info;
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::TimeoutAwaitExpr;
    }
    
    std::string toString() const override {
        return "TimeoutAwait(" + 
               (future_expr ? future_expr->toString() : "null") + 
               ", timeout=" + (timeout_expr ? timeout_expr->toString() : "null") + ")";
    }
};

/**
 * (AR) فرع اختيار انتظار
 */
struct SelectBranch {
    std::unique_ptr<ast::PatternNode> pattern;      // (AR) النمط
    std::unique_ptr<ast::ExpressionNode> future;    // (AR) المستقبل
    std::unique_ptr<ast::BlockNode> body;           // (AR) الجسم
    std::optional<std::unique_ptr<ast::ExpressionNode>> guard; // (AR) الحارس
};

/**
 * (AR) عقدة الانتظار الانتقائي
 */
struct SelectAwaitNode : public ast::ExpressionNode {
    std::vector<SelectBranch> branches;
    std::optional<std::unique_ptr<ast::BlockNode>> default_branch;
    AwaitInfo info;
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::SelectAwaitExpr;
    }
    
    std::string toString() const override {
        return "SelectAwait(branches=" + std::to_string(branches.size()) + ")";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل الانتظار
//                    (EN) Await Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) الكلمات المفتاحية
 */
namespace await_keywords {
    constexpr const char* AWAIT_AR = "انتظر";
    constexpr const char* AWAIT_EN = "await";
    constexpr const char* JOIN_AR = "انضم";
    constexpr const char* JOIN_EN = "join";
    constexpr const char* RACE_AR = "سباق";
    constexpr const char* RACE_EN = "race";
    constexpr const char* TIMEOUT_AR = "مهلة";
    constexpr const char* TIMEOUT_EN = "timeout";
    constexpr const char* SELECT_AR = "اختر";
    constexpr const char* SELECT_EN = "select";
}

/**
 * (AR) فئة محلل الانتظار
 */
class AwaitParser {
private:
    Parser& parser_;
    errors::ErrorReporter& error_reporter_;
    bool in_async_context_;
    
public:
    /**
     * (AR) مُنشئ المحلل
     */
    AwaitParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser)
        , error_reporter_(reporter)
        , in_async_context_(false) {}
    
    /**
     * (AR) تعيين السياق غير المتزامن
     */
    void setAsyncContext(bool in_async) {
        in_async_context_ = in_async;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) التحقق من الكلمات المفتاحية
    //                    (EN) Keyword Detection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل الرمز الحالي 'انتظر'؟
     */
    bool isAwaitKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == await_keywords::AWAIT_AR || 
               text == await_keywords::AWAIT_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'انضم'؟
     */
    bool isJoinKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == await_keywords::JOIN_AR || 
               text == await_keywords::JOIN_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'سباق'؟
     */
    bool isRaceKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == await_keywords::RACE_AR || 
               text == await_keywords::RACE_EN;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل تعبيرات الانتظار
    //                    (EN) Await Expression Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل تعبير انتظار
     * 
     * يكتشف نوع الانتظار ويُحلله:
     *   - انتظر بسيط
     *   - انتظر انضم!
     *   - انتظر سباق!
     *   - انتظر مهلة
     *   - انتظر اختر!
     */
    std::unique_ptr<ast::ExpressionNode> parseAwaitExpression() {
        // (AR) التحقق من السياق
        if (!in_async_context_) {
            error_reporter_.error(
                parser_.currentToken().location,
                "'انتظر' يمكن استخدامها فقط داخل دالة غير متزامنة",
                "'await' can only be used inside an async function"
            );
            return nullptr;
        }
        
        auto location = parser_.currentToken().location;
        
        // (AR) استهلك 'انتظر'
        if (!consumeAwait()) {
            return nullptr;
        }
        
        // (AR) تحديد نوع الانتظار
        if (isJoinKeyword()) {
            return parseJoinAwait(location);
        } else if (isRaceKeyword()) {
            return parseRaceAwait(location);
        } else if (isTimeoutKeyword()) {
            return parseTimeoutAwait(location);
        } else if (isSelectKeyword()) {
            return parseSelectAwait(location);
        } else {
            return parseSimpleAwait(location);
        }
    }
    
    /**
     * (AR) تحليل انتظار بسيط
     * 
     * الصيغة: انتظر تعبير
     */
    std::unique_ptr<SimpleAwaitNode> parseSimpleAwait(
        const lexer::SourceLocation& location
    ) {
        auto node = std::make_unique<SimpleAwaitNode>();
        node->info.kind = AwaitKind::SIMPLE;
        node->info.location = location;
        node->info.is_try = false;
        
        // (AR) تحليل التعبير المُنتَظَر
        node->future_expr = parser_.parseUnaryExpression();
        if (!node->future_expr) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت تعبيراً بعد 'انتظر'",
                "Expected expression after 'await'"
            );
            return nullptr;
        }
        
        // (AR) تحقق من عامل ؟
        if (parser_.currentToken().is(lexer::TokenType::QUESTION)) {
            node->info.is_try = true;
            node->info.kind = AwaitKind::TRY_AWAIT;
            parser_.advance();
        }
        
        return node;
    }
    
    /**
     * (AR) تحليل انتظار متوازي (join)
     * 
     * الصيغة: انتظر انضم!(مستقبل1، مستقبل2، ...)
     */
    std::unique_ptr<JoinAwaitNode> parseJoinAwait(
        const lexer::SourceLocation& location
    ) {
        auto node = std::make_unique<JoinAwaitNode>();
        node->info.kind = AwaitKind::JOIN;
        node->info.location = location;
        
        // (AR) استهلك 'انضم!'
        parser_.advance(); // انضم
        if (!parser_.currentToken().is(lexer::TokenType::EXCLAIM)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '!' بعد 'انضم'",
                "Expected '!' after 'join'"
            );
            return nullptr;
        }
        parser_.advance(); // !
        
        // (AR) توقع '('
        if (!parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '(' بعد 'انضم!'",
                "Expected '(' after 'join!'"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل قائمة المستقبلات
        while (!parser_.currentToken().is(lexer::TokenType::RPAREN) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            auto future = parser_.parseExpression();
            if (!future) {
                return nullptr;
            }
            node->futures.push_back(std::move(future));
            
            if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                parser_.advance();
            } else {
                break;
            }
        }
        
        // (AR) توقع ')'
        if (!parser_.currentToken().is(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')' لإغلاق 'انضم!'",
                "Expected ')' to close 'join!'"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحقق من وجود مستقبلين على الأقل
        if (node->futures.size() < 2) {
            error_reporter_.warning(
                location,
                "'انضم!' يحتاج مستقبلين على الأقل للتنفيذ المتوازي",
                "'join!' needs at least 2 futures for parallel execution"
            );
        }
        
        return node;
    }
    
    /**
     * (AR) تحليل انتظار السباق
     * 
     * الصيغة: انتظر سباق!(مستقبل1، مستقبل2، ...)
     */
    std::unique_ptr<RaceAwaitNode> parseRaceAwait(
        const lexer::SourceLocation& location
    ) {
        auto node = std::make_unique<RaceAwaitNode>();
        node->info.kind = AwaitKind::RACE;
        node->info.location = location;
        
        // (AR) استهلك 'سباق!'
        parser_.advance(); // سباق
        if (!parser_.currentToken().is(lexer::TokenType::EXCLAIM)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '!' بعد 'سباق'",
                "Expected '!' after 'race'"
            );
            return nullptr;
        }
        parser_.advance(); // !
        
        // (AR) توقع '('
        if (!parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '(' بعد 'سباق!'",
                "Expected '(' after 'race!'"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل قائمة المستقبلات
        while (!parser_.currentToken().is(lexer::TokenType::RPAREN) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            auto future = parser_.parseExpression();
            if (!future) {
                return nullptr;
            }
            node->futures.push_back(std::move(future));
            
            if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                parser_.advance();
            } else {
                break;
            }
        }
        
        // (AR) توقع ')'
        if (!parser_.currentToken().is(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')' لإغلاق 'سباق!'",
                "Expected ')' to close 'race!'"
            );
            return nullptr;
        }
        parser_.advance();
        
        return node;
    }
    
    /**
     * (AR) تحليل انتظار مع مهلة
     * 
     * الصيغة: انتظر مهلة(مستقبل، مدة)
     */
    std::unique_ptr<TimeoutAwaitNode> parseTimeoutAwait(
        const lexer::SourceLocation& location
    ) {
        auto node = std::make_unique<TimeoutAwaitNode>();
        node->info.kind = AwaitKind::TIMEOUT;
        node->info.location = location;
        
        // (AR) استهلك 'مهلة'
        parser_.advance();
        
        // (AR) توقع '('
        if (!parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '(' بعد 'مهلة'",
                "Expected '(' after 'timeout'"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل المستقبل
        node->future_expr = parser_.parseExpression();
        if (!node->future_expr) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت تعبير المستقبل",
                "Expected future expression"
            );
            return nullptr;
        }
        
        // (AR) توقع ','
        if (!parser_.currentToken().is(lexer::TokenType::COMMA)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '،' بعد تعبير المستقبل",
                "Expected ',' after future expression"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل المهلة
        node->timeout_expr = parser_.parseExpression();
        if (!node->timeout_expr) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت تعبير المهلة",
                "Expected timeout expression"
            );
            return nullptr;
        }
        
        // (AR) توقع ')'
        if (!parser_.currentToken().is(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')' لإغلاق 'مهلة'",
                "Expected ')' to close 'timeout'"
            );
            return nullptr;
        }
        parser_.advance();
        
        return node;
    }
    
    /**
     * (AR) تحليل الانتظار الانتقائي
     * 
     * الصيغة:
     *     انتظر اختر! {
     *         نتيجة = مستقبل1 => { ... }
     *         نتيجة = مستقبل2 إذا شرط => { ... }
     *         افتراضي => { ... }
     *     }
     */
    std::unique_ptr<SelectAwaitNode> parseSelectAwait(
        const lexer::SourceLocation& location
    ) {
        auto node = std::make_unique<SelectAwaitNode>();
        node->info.kind = AwaitKind::SELECT;
        node->info.location = location;
        
        // (AR) استهلك 'اختر!'
        parser_.advance(); // اختر
        if (!parser_.currentToken().is(lexer::TokenType::EXCLAIM)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '!' بعد 'اختر'",
                "Expected '!' after 'select'"
            );
            return nullptr;
        }
        parser_.advance(); // !
        
        // (AR) توقع '{'
        if (!parser_.currentToken().is(lexer::TokenType::LBRACE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '{' بعد 'اختر!'",
                "Expected '{' after 'select!'"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل الفروع
        while (!parser_.currentToken().is(lexer::TokenType::RBRACE) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            // (AR) فحص الفرع الافتراضي
            if (isDefaultKeyword()) {
                parser_.advance();
                if (!parser_.currentToken().is(lexer::TokenType::FAT_ARROW)) {
                    error_reporter_.error(
                        parser_.currentToken().location,
                        "توقعت '=>' بعد 'افتراضي'",
                        "Expected '=>' after 'default'"
                    );
                    return nullptr;
                }
                parser_.advance();
                node->default_branch = parser_.parseBlock();
                continue;
            }
            
            // (AR) تحليل فرع عادي
            SelectBranch branch;
            
            // (AR) النمط
            branch.pattern = parser_.parsePattern();
            if (!branch.pattern) {
                return nullptr;
            }
            
            // (AR) '='
            if (!parser_.currentToken().is(lexer::TokenType::EQUAL)) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت '=' بعد النمط",
                    "Expected '=' after pattern"
                );
                return nullptr;
            }
            parser_.advance();
            
            // (AR) المستقبل
            branch.future = parser_.parseExpression();
            if (!branch.future) {
                return nullptr;
            }
            
            // (AR) الحارس (اختياري)
            if (isIfKeyword()) {
                parser_.advance();
                branch.guard = parser_.parseExpression();
            }
            
            // (AR) '=>'
            if (!parser_.currentToken().is(lexer::TokenType::FAT_ARROW)) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت '=>' قبل جسم الفرع",
                    "Expected '=>' before branch body"
                );
                return nullptr;
            }
            parser_.advance();
            
            // (AR) الجسم
            branch.body = parser_.parseBlock();
            if (!branch.body) {
                return nullptr;
            }
            
            node->branches.push_back(std::move(branch));
        }
        
        // (AR) توقع '}'
        if (!parser_.currentToken().is(lexer::TokenType::RBRACE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '}' لإغلاق 'اختر!'",
                "Expected '}' to close 'select!'"
            );
            return nullptr;
        }
        parser_.advance();
        
        return node;
    }
    
private:
    /**
     * (AR) استهلاك 'انتظر'
     */
    bool consumeAwait() {
        if (isAwaitKeyword()) {
            parser_.advance();
            return true;
        }
        return false;
    }
    
    /**
     * (AR) هل الرمز الحالي 'مهلة'؟
     */
    bool isTimeoutKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == await_keywords::TIMEOUT_AR || 
               text == await_keywords::TIMEOUT_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'اختر'؟
     */
    bool isSelectKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == await_keywords::SELECT_AR || 
               text == await_keywords::SELECT_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'افتراضي'؟
     */
    bool isDefaultKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == "افتراضي" || text == "default";
    }
    
    /**
     * (AR) هل الرمز الحالي 'إذا'؟
     */
    bool isIfKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == "إذا" || text == "if";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) إنشاء محلل انتظار جديد
 */
std::unique_ptr<AwaitParser> createAwaitParser(
    Parser& parser,
    errors::ErrorReporter& reporter
) {
    return std::make_unique<AwaitParser>(parser, reporter);
}

} // namespace sad::parser
