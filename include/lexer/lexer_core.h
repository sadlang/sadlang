// ======================================================================
// lexer_core.h - المحلل المعجمي الأساسي / Lexer Core
// ======================================================================
// الوصف بالعربية:
//   المحلل المعجمي (Lexer) يحول النص المصدري إلى تدفق من الرموز (Tokens)
//   يتعرف على الكلمات المفتاحية، الأرقام، النصوص، المعرفات، والعوامل
//
// English Description:
//   Lexer converts source code text into a stream of tokens
//   Recognizes keywords, numbers, strings, identifiers, and operators
//
// ملاحظات / Notes:
//   - يدعم UTF-8 للنصوص العربية بالكامل / Full UTF-8 support for Arabic text
//   - يتخطى المسافات والتعليقات تلقائياً / Automatically skips whitespace and comments
//   - يحفظ معلومات الموقع الدقيقة لكل رمز / Stores precise position info for each token
// ======================================================================

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"
#include "lexer_keywords.h"

namespace Sad {
namespace Lexer {

/**
 * @class LexerCore
 * @brief (AR) المحلل المعجمي - يحول النص إلى رموز
 * @brief (EN) Lexer - converts text to tokens
 */
class LexerCore {
public:
    /**
     * @brief (AR) منشئ يستقبل النص المصدري
     * @brief (EN) Constructor receiving source code
     * 
     * @param source (std::string) — (AR) النص المصدري الكامل / (EN) complete source code
     * 
     * مثال: LexerCore lexer("رقم س = 5");
     */
    explicit LexerCore(const std::string& source);
    
    /**
     * @brief (AR) تحويل النص بالكامل إلى قائمة رموز
     * @brief (EN) Convert entire text to list of tokens
     * 
     * @return (std::vector<Token>) — (AR) قائمة جميع الرموز / (EN) list of all tokens
     * @throws (std::runtime_error) — (AR) عند وجود خطأ في التحليل / (EN) on parsing error
     */
    std::vector<Token> tokenize();
    
    /**
     * @brief (AR) الحصول على الرمز التالي فقط
     * @brief (EN) Get next token only
     * 
     * @return (Token) — (AR) الرمز التالي / (EN) next token
     */
    Token nextToken();
    
    /**
     * @brief (AR) النظر إلى الحرف الحالي دون تقدم
     * @brief (EN) Peek at current character without advancing
     * 
     * @return (char) — (AR) الحرف الحالي / (EN) current char
     */
    char peek() const;
    
    /**
     * @brief (AR) النظر إلى الحرف التالي
     * @brief (EN) Peek at next character
     */
    char peekNext(size_t offset = 1) const;
    
    /**
     * @brief (AR) التقدم حرف واحد وإرجاعه
     * @brief (EN) Advance one character and return it
     */
    char advance();
    
    /**
     * @brief (AR) التحقق من الوصول لنهاية الملف
     * @brief (EN) Check if reached end of file
     */
    bool isAtEnd() const;
    
    /**
     * @brief (AR) الحصول على الموقع الحالي
     * @brief (EN) Get current position
     */
    Position getCurrentPosition() const;
    
    /**
     * @brief (AR) تخطي المسافات والتابات
     * @brief (EN) Skip whitespace and tabs
     */
    void skipWhitespace();
    
    /**
     * @brief (AR) تخطي التعليقات
     * @brief (EN) Skip comments
     */
    void skipComment();
    
    /**
     * @brief (AR) معالجة رقم (صحيح أو عشري)
     * @brief (EN) Process number (integer or double)
     */
    Token scanNumber();
    
    /**
     * @brief (AR) معالجة نص بين علامات تنصيص
     * @brief (EN) Process string between quotes
     */
    Token scanString();
    
    /**
     * @brief (AR) معالجة معرف أو كلمة مفتاحية
     * @brief (EN) Process identifier or keyword
     */
    Token scanIdentifier();
    
    /**
     * @brief (AR) معالجة عامل (operator)
     * @brief (EN) Process operator
     */
    Token scanOperator();

private:
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    Token makeError(const std::string& message);

    std::string source_;
    size_t current_;
    size_t line_;
    size_t column_;
    Position start_position_;
};

} // namespace Lexer
} // namespace Sad
