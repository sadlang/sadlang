// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file constraint_parser.cpp
 * @brief تنفيذ محلل القيود العربية
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "constraint_parser.hpp"
#include <algorithm>
#include <cctype>

namespace Sad {
namespace Parser {

// ═══════════════════════════════════════════════════════════════════════════
//                    المنشئ / Constructor
// ═══════════════════════════════════════════════════════════════════════════

ConstraintParser::ConstraintParser()
    : arabicMessages_(true) {}

// ═══════════════════════════════════════════════════════════════════════════
//                    تحليل جملة حيث / Parse Where Clause
// ═══════════════════════════════════════════════════════════════════════════

ConstraintParseResult<WhereClause> ConstraintParser::parseWhereClause(const std::string& source) {
    WhereClause result;
    size_t pos = 0;
    
    skipWhitespace(source, pos);
    
    // التحقق من كلمة "حيث" أو "where"
    bool foundWhere = false;
    if (matchKeyword(source, pos, "\xD8\xAD\xD9\x8A\xD8\xAB")) { // حيث
        foundWhere = true;
    } else if (matchKeyword(source, pos, "where")) {
        foundWhere = true;
    }
    
    if (!foundWhere) {
        return ConstraintParseResult<WhereClause>::error(
            makeError("متوقع كلمة 'حيث' في بداية جملة القيود",
                     "Expected 'where' keyword at start of constraint clause",
                     std::to_string(pos),
                     "أضف 'حيث' قبل القيود"));
    }
    
    // تحليل القيود
    bool expectComma = false;
    while (pos < source.size()) {
        skipWhitespace(source, pos);
        
        if (pos >= source.size()) break;
        
        // التحقق من نهاية جملة حيث
        char ch = source[pos];
        if (ch == '{' || ch == ';' || ch == '\n') break;
        
        // التحقق من الفاصلة
        if (expectComma) {
            // فاصلة عربية أو إنجليزية
            if (ch == ',' || 
                (pos + 1 < source.size() && 
                 static_cast<unsigned char>(source[pos]) == 0xD8 && 
                 static_cast<unsigned char>(source[pos + 1]) == 0x8C)) {
                if (static_cast<unsigned char>(source[pos]) == 0xD8) {
                    pos += 2;
                } else {
                    pos++;
                }
                expectComma = false;
                continue;
            }
            break; // نهاية القيود
        }
        
        // قراءة اسم المعامل
        std::string paramName = readIdentifier(source, pos);
        if (paramName.empty()) {
            return ConstraintParseResult<WhereClause>::error(
                makeError("متوقع اسم معامل في جملة 'حيث'",
                         "Expected parameter name in where clause",
                         std::to_string(pos)));
        }
        
        WhereConstraint constraint(paramName);
        constraint.location = std::to_string(pos);
        
        // التحقق من وجود ':'
        skipWhitespace(source, pos);
        if (pos >= source.size() || source[pos] != ':') {
            return ConstraintParseResult<WhereClause>::error(
                makeError("متوقع ':' بعد اسم المعامل",
                         "Expected ':' after parameter name",
                         std::to_string(pos),
                         "أضف ':' ثم اسم السمة"));
        }
        pos++; // تخطي ':'
        
        // تحليل قيود السمات
        while (pos < source.size()) {
            skipWhitespace(source, pos);
            
            // قراءة اسم السمة
            std::string traitName = readIdentifier(source, pos);
            if (traitName.empty()) {
                return ConstraintParseResult<WhereClause>::error(
                    makeError("متوقع اسم سمة بعد ':'",
                             "Expected trait name after ':'",
                             std::to_string(pos)));
            }
            
            TraitBound bound(traitName);
            bound.location = std::to_string(pos);
            
            // التحقق من وجود وسائط نوع
            skipWhitespace(source, pos);
            if (pos < source.size() && source[pos] == '<') {
                pos++; // تخطي '<'
                
                // قراءة وسائط النوع
                while (pos < source.size()) {
                    skipWhitespace(source, pos);
                    
                    if (source[pos] == '>') {
                        pos++;
                        break;
                    }
                    
                    std::string typeArg = readIdentifier(source, pos);
                    if (!typeArg.empty()) {
                        bound.typeArguments.push_back(typeArg);
                    }
                    
                    skipWhitespace(source, pos);
                    if (source[pos] == ',') {
                        pos++;
                    } else if (pos + 1 < source.size() && 
                               static_cast<unsigned char>(source[pos]) == 0xD8 && 
                               static_cast<unsigned char>(source[pos + 1]) == 0x8C) {
                        pos += 2;
                    }
                }
            }
            
            constraint.addBound(bound);
            
            // التحقق من وجود '+' للقيود الإضافية
            skipWhitespace(source, pos);
            if (pos < source.size() && source[pos] == '+') {
                pos++;
                continue;
            }
            break;
        }
        
        result.addConstraint(constraint);
        expectComma = true;
    }
    
    result.location = "0-" + std::to_string(pos);
    return ConstraintParseResult<WhereClause>::ok(result);
}

ConstraintParseResult<WhereConstraint> ConstraintParser::parseConstraint(const std::string& source) {
    size_t pos = 0;
    skipWhitespace(source, pos);
    
    // قراءة اسم المعامل
    std::string paramName = readIdentifier(source, pos);
    if (paramName.empty()) {
        return ConstraintParseResult<WhereConstraint>::error(
            makeError("متوقع اسم معامل",
                     "Expected parameter name"));
    }
    
    WhereConstraint constraint(paramName);
    
    // التحقق من وجود ':'
    skipWhitespace(source, pos);
    if (pos >= source.size() || source[pos] != ':') {
        return ConstraintParseResult<WhereConstraint>::error(
            makeError("متوقع ':' بعد اسم المعامل",
                     "Expected ':' after parameter name"));
    }
    pos++;
    
    // تحليل القيود
    while (pos < source.size()) {
        skipWhitespace(source, pos);
        
        auto boundResult = parseTraitBound(source.substr(pos));
        if (!boundResult.success) {
            return ConstraintParseResult<WhereConstraint>::error(boundResult.errors[0]);
        }
        
        constraint.addBound(boundResult.value);
        
        // تحديث الموقع
        pos += boundResult.value.traitName.size();
        if (!boundResult.value.typeArguments.empty()) {
            // تقدير طول وسائط النوع
            pos += 2; // < و >
            for (const auto& arg : boundResult.value.typeArguments) {
                pos += arg.size() + 1;
            }
        }
        
        skipWhitespace(source, pos);
        if (pos < source.size() && source[pos] == '+') {
            pos++;
            continue;
        }
        break;
    }
    
    return ConstraintParseResult<WhereConstraint>::ok(constraint);
}

ConstraintParseResult<TraitBound> ConstraintParser::parseTraitBound(const std::string& source) {
    size_t pos = 0;
    skipWhitespace(source, pos);
    
    std::string traitName = readIdentifier(source, pos);
    if (traitName.empty()) {
        return ConstraintParseResult<TraitBound>::error(
            makeError("متوقع اسم سمة",
                     "Expected trait name"));
    }
    
    if (!isValidTraitName(traitName)) {
        return ConstraintParseResult<TraitBound>::error(
            makeError("اسم سمة غير صحيح: " + traitName,
                     "Invalid trait name: " + traitName));
    }
    
    TraitBound bound(traitName);
    
    // التحقق من وجود وسائط نوع
    skipWhitespace(source, pos);
    if (pos < source.size() && source[pos] == '<') {
        pos++;
        
        while (pos < source.size()) {
            skipWhitespace(source, pos);
            
            if (source[pos] == '>') {
                pos++;
                break;
            }
            
            std::string typeArg = readIdentifier(source, pos);
            if (!typeArg.empty()) {
                bound.typeArguments.push_back(typeArg);
            }
            
            skipWhitespace(source, pos);
            if (source[pos] == ',') pos++;
        }
    }
    
    return ConstraintParseResult<TraitBound>::ok(bound);
}

// ═══════════════════════════════════════════════════════════════════════════
//                    التحقق / Validation
// ═══════════════════════════════════════════════════════════════════════════

bool ConstraintParser::startsWithWhere(const std::string& source) const {
    size_t pos = 0;
    
    // تخطي المسافات
    while (pos < source.size() && std::isspace(static_cast<unsigned char>(source[pos]))) {
        pos++;
    }
    
    // التحقق من "حيث"
    if (pos + 5 < source.size() &&
        static_cast<unsigned char>(source[pos]) == 0xD8 &&
        static_cast<unsigned char>(source[pos + 1]) == 0xAD &&
        static_cast<unsigned char>(source[pos + 2]) == 0xD9 &&
        static_cast<unsigned char>(source[pos + 3]) == 0x8A &&
        static_cast<unsigned char>(source[pos + 4]) == 0xD8 &&
        static_cast<unsigned char>(source[pos + 5]) == 0xAB) {
        return true;
    }
    
    // التحقق من "where"
    if (source.substr(pos, 5) == "where") {
        return true;
    }
    
    return false;
}

bool ConstraintParser::isValidTraitName(const std::string& name) const {
    if (name.empty()) return false;
    
    // أسماء السمات المعروفة
    static const std::vector<std::string> knownTraits = {
        // عربية
        "قابل_للنسخ", "قابل_للاستنساخ", "قابل_للمقارنة",
        "قابل_للترتيب", "قابل_للتجزئة", "قابل_للعرض",
        "قابل_للتنقيح", "افتراضي", "من", "إلى",
        "قابل_للتكرار", "قابل_للتكرار_عليه",
        "عامل_جمع", "عامل_طرح", "عامل_ضرب", "عامل_قسمة",
        // إنجليزية
        "Copy", "Clone", "Eq", "PartialEq", "Ord", "PartialOrd",
        "Hash", "Display", "Debug", "Default", "From", "Into",
        "Iterator", "IntoIterator", "Add", "Sub", "Mul", "Div"
    };
    
    for (const auto& trait : knownTraits) {
        if (name == trait) return true;
    }
    
    // السماح بأي اسم معرف صحيح
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════════════

void ConstraintParser::skipWhitespace(const std::string& source, size_t& pos) {
    while (pos < source.size() && 
           std::isspace(static_cast<unsigned char>(source[pos]))) {
        pos++;
    }
}

std::string ConstraintParser::readIdentifier(const std::string& source, size_t& pos) {
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

std::string ConstraintParser::readUnicodeChar(const std::string& source, size_t& pos) {
    if (pos >= source.size()) return "";
    
    unsigned char first = static_cast<unsigned char>(source[pos]);
    
    // ASCII
    if (first < 0x80) {
        return std::string(1, source[pos++]);
    }
    
    // UTF-8 متعدد البايتات
    int bytes = 0;
    if ((first & 0xE0) == 0xC0) bytes = 2;
    else if ((first & 0xF0) == 0xE0) bytes = 3;
    else if ((first & 0xF8) == 0xF0) bytes = 4;
    else return std::string(1, source[pos++]);
    
    if (pos + bytes > source.size()) {
        return std::string(1, source[pos++]);
    }
    
    std::string result = source.substr(pos, bytes);
    pos += bytes;
    return result;
}

bool ConstraintParser::isIdentifierStart(const std::string& ch) const {
    if (ch.empty()) return false;
    
    unsigned char first = static_cast<unsigned char>(ch[0]);
    
    // ASCII letter or underscore
    if (first < 0x80) {
        char c = ch[0];
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') || 
               c == '_';
    }
    
    // Arabic letters
    if (ch.size() >= 2) {
        unsigned char b1 = static_cast<unsigned char>(ch[0]);
        unsigned char b2 = static_cast<unsigned char>(ch[1]);
        
        if ((b1 == 0xD8 || b1 == 0xD9) && 
            ((b1 == 0xD8 && b2 >= 0x80) || (b1 == 0xD9 && b2 <= 0x8A))) {
            return true;
        }
    }
    
    return false;
}

bool ConstraintParser::isIdentifierPart(const std::string& ch) const {
    if (isIdentifierStart(ch)) return true;
    
    if (ch.size() == 1) {
        char c = ch[0];
        return (c >= '0' && c <= '9') || c == '_';
    }
    
    return false;
}

bool ConstraintParser::matchKeyword(const std::string& source, size_t& pos, const std::string& keyword) {
    skipWhitespace(source, pos);
    
    if (pos + keyword.size() > source.size()) return false;
    
    if (source.substr(pos, keyword.size()) == keyword) {
        // التأكد من نهاية الكلمة
        size_t nextPos = pos + keyword.size();
        if (nextPos < source.size()) {
            char nextChar = source[nextPos];
            if (std::isalnum(static_cast<unsigned char>(nextChar)) || nextChar == '_') {
                return false; // الكلمة جزء من معرف أطول
            }
        }
        pos += keyword.size();
        return true;
    }
    
    return false;
}

ConstraintParseError ConstraintParser::makeError(const std::string& ar, 
                                                  const std::string& en,
                                                  const std::string& loc,
                                                  const std::string& sug) {
    return ConstraintParseError(ar, en, loc, sug);
}

} // namespace Parser
} // namespace Sad
