// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file generics_parser.cpp
 * @brief تنفيذ محلل المعممات
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              تنفيذ محلل المعممات في لغة ص (Generics Parser Implementation)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "generics_parser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace Sad {
namespace Parser {

// ═══════════════════════════════════════════════════════════════════════════
//                    المنشئ / Constructor
// ═══════════════════════════════════════════════════════════════════════════

GenericsParser::GenericsParser()
    : arabicMessages_(true) {}

// ═══════════════════════════════════════════════════════════════════════════
//                    تحليل المعاملات / Parse Parameters
// ═══════════════════════════════════════════════════════════════════════════

GenericParseResult<GenericParametersAST> GenericsParser::parseParameters(const std::string& source) {
    GenericParametersAST result;
    size_t pos = 0;
    
    // تخطي المسافات البيضاء في البداية
    skipWhitespace(source, pos);
    
    // التحقق من وجود '<'
    if (pos >= source.size() || source[pos] != '<') {
        return GenericParseResult<GenericParametersAST>::error(
            makeError("متوقع '<' في بداية معاملات المعممات",
                     "Expected '<' at start of generic parameters",
                     std::to_string(pos),
                     "أضف '<' قبل المعاملات"));
    }
    pos++; // تخطي '<'
    
    // تحليل المعاملات
    bool expectComma = false;
    while (pos < source.size()) {
        skipWhitespace(source, pos);
        
        // التحقق من نهاية القائمة
        if (source[pos] == '>') {
            pos++; // تخطي '>'
            result.location = "0-" + std::to_string(pos);
            return GenericParseResult<GenericParametersAST>::ok(result);
        }
        
        // التحقق من الفاصلة
        if (expectComma) {
            // دعم الفاصلة العربية '،' والإنجليزية ','
            if (source[pos] == ',' || 
                (pos + 1 < source.size() && 
                 static_cast<unsigned char>(source[pos]) == 0xD8 && 
                 static_cast<unsigned char>(source[pos + 1]) == 0x8C)) {
                // فاصلة عربية تأخذ بايتين
                if (static_cast<unsigned char>(source[pos]) == 0xD8) {
                    pos += 2;
                } else {
                    pos++;
                }
                expectComma = false;
                continue;
            } else {
                return GenericParseResult<GenericParametersAST>::error(
                    makeError("متوقع '،' أو '>' بعد المعامل",
                             "Expected ',' or '>' after parameter",
                             std::to_string(pos)));
            }
        }
        
        // قراءة اسم المعامل
        std::string paramName = readIdentifier(source, pos);
        if (paramName.empty()) {
            return GenericParseResult<GenericParametersAST>::error(
                makeError("متوقع اسم معامل",
                         "Expected parameter name",
                         std::to_string(pos)));
        }
        
        // التحقق من صحة الاسم
        if (!isValidParameterName(paramName)) {
            return GenericParseResult<GenericParametersAST>::error(
                makeError("اسم معامل غير صحيح: " + paramName,
                         "Invalid parameter name: " + paramName,
                         std::to_string(pos),
                         "استخدم حرفاً عربياً مثل: ت، س، ن"));
        }
        
        GenericParameterAST param(paramName, std::to_string(pos));
        
        // التحقق من وجود قيود
        skipWhitespace(source, pos);
        if (pos < source.size() && source[pos] == ':') {
            pos++; // تخطي ':'
            
            // تحليل القيود
            while (pos < source.size()) {
                skipWhitespace(source, pos);
                
                // قراءة اسم السمة
                std::string traitName = readIdentifier(source, pos);
                if (traitName.empty()) {
                    return GenericParseResult<GenericParametersAST>::error(
                        makeError("متوقع اسم سمة بعد ':'",
                                 "Expected trait name after ':'",
                                 std::to_string(pos)));
                }
                
                if (!isValidTraitName(traitName)) {
                    return GenericParseResult<GenericParametersAST>::error(
                        makeError("اسم سمة غير صحيح: " + traitName,
                                 "Invalid trait name: " + traitName,
                                 std::to_string(pos)));
                }
                
                param.addConstraint(GenericConstraint(traitName, std::to_string(pos)));
                
                // التحقق من وجود '+' للقيود الإضافية
                skipWhitespace(source, pos);
                if (pos < source.size() && source[pos] == '+') {
                    pos++; // تخطي '+'
                    continue;
                }
                break;
            }
        }
        
        result.addParameter(param);
        expectComma = true;
    }
    
    // لم نجد '>' في النهاية
    return GenericParseResult<GenericParametersAST>::error(
        makeError("متوقع '>' في نهاية معاملات المعممات",
                 "Expected '>' at end of generic parameters",
                 std::to_string(pos)));
}

GenericParseResult<GenericParameterAST> GenericsParser::parseParameter(const std::string& source) {
    size_t pos = 0;
    skipWhitespace(source, pos);
    
    // قراءة اسم المعامل
    std::string paramName = readIdentifier(source, pos);
    if (paramName.empty()) {
        return GenericParseResult<GenericParameterAST>::error(
            makeError("متوقع اسم معامل", "Expected parameter name"));
    }
    
    if (!isValidParameterName(paramName)) {
        return GenericParseResult<GenericParameterAST>::error(
            makeError("اسم معامل غير صحيح: " + paramName,
                     "Invalid parameter name: " + paramName));
    }
    
    GenericParameterAST param(paramName);
    
    // التحقق من وجود قيود
    skipWhitespace(source, pos);
    if (pos < source.size() && source[pos] == ':') {
        pos++; // تخطي ':'
        
        // تحليل القيود
        auto constraintsResult = parseConstraints(source.substr(pos));
        if (!constraintsResult.success) {
            return GenericParseResult<GenericParameterAST>::error(constraintsResult.errors[0]);
        }
        
        for (const auto& constraint : constraintsResult.value) {
            param.addConstraint(constraint);
        }
    }
    
    return GenericParseResult<GenericParameterAST>::ok(param);
}

GenericParseResult<std::vector<GenericConstraint>> GenericsParser::parseConstraints(const std::string& source) {
    std::vector<GenericConstraint> constraints;
    size_t pos = 0;
    
    while (pos < source.size()) {
        skipWhitespace(source, pos);
        
        if (pos >= source.size()) break;
        
        // التحقق من نهاية القيود
        char ch = source[pos];
        if (ch == ',' || ch == '>' || ch == ')' || ch == '{') {
            break;
        }
        
        // قراءة اسم السمة
        std::string traitName = readIdentifier(source, pos);
        if (traitName.empty()) {
            break;
        }
        
        if (!isValidTraitName(traitName)) {
            return GenericParseResult<std::vector<GenericConstraint>>::error(
                makeError("اسم سمة غير صحيح: " + traitName,
                         "Invalid trait name: " + traitName));
        }
        
        constraints.emplace_back(traitName, std::to_string(pos));
        
        // التحقق من وجود '+'
        skipWhitespace(source, pos);
        if (pos < source.size() && source[pos] == '+') {
            pos++;
            continue;
        }
        break;
    }
    
    return GenericParseResult<std::vector<GenericConstraint>>::ok(constraints);
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تحليل الأنواع / Parse Types
// ═══════════════════════════════════════════════════════════════════════════

GenericParseResult<GenericTypeAST> GenericsParser::parseType(const std::string& source) {
    size_t pos = 0;
    skipWhitespace(source, pos);
    
    // قراءة اسم النوع الأساسي
    std::string baseName = readIdentifier(source, pos);
    if (baseName.empty()) {
        return GenericParseResult<GenericTypeAST>::error(
            makeError("متوقع اسم نوع", "Expected type name"));
    }
    
    if (!isValidTypeName(baseName)) {
        return GenericParseResult<GenericTypeAST>::error(
            makeError("اسم نوع غير صحيح: " + baseName,
                     "Invalid type name: " + baseName));
    }
    
    GenericTypeAST result(baseName);
    
    // التحقق من وجود وسائط نوع
    skipWhitespace(source, pos);
    if (pos < source.size() && source[pos] == '<') {
        auto argsResult = parseTypeArguments(source.substr(pos));
        if (!argsResult.success) {
            return GenericParseResult<GenericTypeAST>::error(argsResult.errors[0]);
        }
        
        for (const auto& arg : argsResult.value) {
            result.addTypeArgument(arg);
        }
    }
    
    return GenericParseResult<GenericTypeAST>::ok(result);
}

GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>> 
GenericsParser::parseTypeArguments(const std::string& source) {
    std::vector<std::shared_ptr<GenericTypeAST>> args;
    size_t pos = 0;
    
    skipWhitespace(source, pos);
    
    // التحقق من وجود '<'
    if (pos >= source.size() || source[pos] != '<') {
        return GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>>::error(
            makeError("متوقع '<' في بداية وسائط النوع",
                     "Expected '<' at start of type arguments"));
    }
    pos++;
    
    // تحليل الوسائط
    bool expectComma = false;
    int depth = 1;
    std::string currentArg;
    
    while (pos < source.size() && depth > 0) {
        char ch = source[pos];
        
        if (ch == '<') {
            depth++;
            currentArg += ch;
        } else if (ch == '>') {
            depth--;
            if (depth > 0) {
                currentArg += ch;
            }
        } else if ((ch == ',' || 
                   (pos + 1 < source.size() && 
                    static_cast<unsigned char>(ch) == 0xD8 && 
                    static_cast<unsigned char>(source[pos + 1]) == 0x8C)) 
                   && depth == 1) {
            // فاصلة على المستوى الأول
            if (!currentArg.empty()) {
                auto typeResult = parseType(currentArg);
                if (!typeResult.success) {
                    return GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>>::error(
                        typeResult.errors[0]);
                }
                args.push_back(std::make_shared<GenericTypeAST>(typeResult.value));
            }
            currentArg.clear();
            
            // تخطي الفاصلة العربية
            if (static_cast<unsigned char>(ch) == 0xD8) {
                pos += 2;
                continue;
            }
        } else {
            currentArg += ch;
        }
        pos++;
    }
    
    // معالجة آخر وسيط
    // إزالة المسافات من البداية والنهاية
    size_t start = 0, end = currentArg.size();
    while (start < end && std::isspace(static_cast<unsigned char>(currentArg[start]))) start++;
    while (end > start && std::isspace(static_cast<unsigned char>(currentArg[end - 1]))) end--;
    currentArg = currentArg.substr(start, end - start);
    
    if (!currentArg.empty()) {
        auto typeResult = parseType(currentArg);
        if (!typeResult.success) {
            return GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>>::error(
                typeResult.errors[0]);
        }
        args.push_back(std::make_shared<GenericTypeAST>(typeResult.value));
    }
    
    if (depth != 0) {
        return GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>>::error(
            makeError("متوقع '>' في نهاية وسائط النوع",
                     "Expected '>' at end of type arguments"));
    }
    
    return GenericParseResult<std::vector<std::shared_ptr<GenericTypeAST>>>::ok(args);
}

// ═══════════════════════════════════════════════════════════════════════════
//                    التحقق / Validation
// ═══════════════════════════════════════════════════════════════════════════

bool GenericsParser::isValidParameterName(const std::string& name) const {
    if (name.empty()) return false;
    
    // السماح بالحروف العربية المفردة الشائعة
    // ت (تاء)، س (سين)، ن (نون)، ع (عين)، أ، ب، ج، إلخ
    
    // السماح أيضاً بـ T, U, V للتوافق
    if (name.size() == 1) {
        char ch = name[0];
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            return true;
        }
    }
    
    // التحقق من أن الاسم يبدأ بحرف صحيح
    if (!isIdentifierStart(name.substr(0, std::min(name.size(), size_t(4))))) {
        return false;
    }
    
    return true;
}

bool GenericsParser::isValidTraitName(const std::string& name) const {
    if (name.empty()) return false;
    
    // أسماء السمات العربية المعروفة
    static const std::vector<std::string> knownTraits = {
        // عربية
        "قابل_للنسخ", "قابل_للمقارنة", "قابل_للترتيب",
        "قابل_للتجزئة", "قابل_للعرض", "قابل_للاستنساخ",
        // إنجليزية
        "Copy", "Clone", "Eq", "Ord", "Hash", "Display", "Debug"
    };
    
    // التحقق من الأسماء المعروفة
    for (const auto& trait : knownTraits) {
        if (name == trait) return true;
    }
    
    // السماح بأي اسم معرف صحيح
    return isIdentifierStart(name.substr(0, std::min(name.size(), size_t(4))));
}

bool GenericsParser::isValidTypeName(const std::string& name) const {
    if (name.empty()) return false;
    return isIdentifierStart(name.substr(0, std::min(name.size(), size_t(4))));
}

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════════════

std::string GenericsParser::readUnicodeChar(const std::string& source, size_t& pos) {
    if (pos >= source.size()) return "";
    
    unsigned char first = static_cast<unsigned char>(source[pos]);
    
    // ASCII
    if (first < 0x80) {
        return std::string(1, source[pos++]);
    }
    
    // UTF-8 متعدد البايتات
    int bytes = 0;
    if ((first & 0xE0) == 0xC0) bytes = 2;      // 110xxxxx
    else if ((first & 0xF0) == 0xE0) bytes = 3; // 1110xxxx
    else if ((first & 0xF8) == 0xF0) bytes = 4; // 11110xxx
    else return std::string(1, source[pos++]);  // بايت غير صحيح
    
    if (pos + bytes > source.size()) {
        return std::string(1, source[pos++]);
    }
    
    std::string result = source.substr(pos, bytes);
    pos += bytes;
    return result;
}

void GenericsParser::skipWhitespace(const std::string& source, size_t& pos) {
    while (pos < source.size() && 
           std::isspace(static_cast<unsigned char>(source[pos]))) {
        pos++;
    }
}

std::string GenericsParser::readIdentifier(const std::string& source, size_t& pos) {
    skipWhitespace(source, pos);
    
    if (pos >= source.size()) return "";
    
    std::string result;
    size_t startPos = pos;
    
    // قراءة أول حرف
    std::string firstChar = readUnicodeChar(source, pos);
    if (!isIdentifierStart(firstChar)) {
        pos = startPos;
        return "";
    }
    result = firstChar;
    
    // قراءة بقية الحروف
    while (pos < source.size()) {
        size_t savedPos = pos;
        std::string ch = readUnicodeChar(source, pos);
        
        if (isIdentifierPart(ch)) {
            result += ch;
        } else {
            pos = savedPos;
            break;
        }
    }
    
    return result;
}

bool GenericsParser::isIdentifierStart(const std::string& ch) const {
    if (ch.empty()) return false;
    
    unsigned char first = static_cast<unsigned char>(ch[0]);
    
    // ASCII letter or underscore
    if (first < 0x80) {
        char c = ch[0];
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') || 
               c == '_';
    }
    
    // Arabic letters (Unicode range 0x0600-0x06FF)
    // Most common Arabic letters start with 0xD8 or 0xD9 in UTF-8
    if (ch.size() >= 2) {
        unsigned char b1 = static_cast<unsigned char>(ch[0]);
        unsigned char b2 = static_cast<unsigned char>(ch[1]);
        
        // Arabic Unicode range
        if ((b1 == 0xD8 || b1 == 0xD9) && 
            ((b1 == 0xD8 && b2 >= 0x80) || (b1 == 0xD9 && b2 <= 0x8A))) {
            return true;
        }
    }
    
    return false;
}

bool GenericsParser::isIdentifierPart(const std::string& ch) const {
    if (isIdentifierStart(ch)) return true;
    
    if (ch.size() == 1) {
        char c = ch[0];
        // Digits and underscore
        return (c >= '0' && c <= '9') || c == '_';
    }
    
    return false;
}

GenericParseError GenericsParser::makeError(const std::string& msgAr, 
                                            const std::string& msgEn,
                                            const std::string& loc,
                                            const std::string& suggestion) {
    return GenericParseError(
        arabicMessages_ ? msgAr : msgEn,
        loc,
        suggestion
    );
}

} // namespace Parser
} // namespace Sad
