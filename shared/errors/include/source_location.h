/**
 * @file source_location.h
 * @brief (AR) تتبع موقع الأخطاء في الكود المصدري
 *        (EN) Track error locations in source code
 * 
 * (AR) يوفر هذا الملف هياكل لتمثيل المواقع والنطاقات في الكود المصدري
 *      يُستخدم لتحديد مكان الأخطاء بدقة (ملف:سطر:عمود)
 * 
 * (EN) This file provides structures to represent locations and ranges in source code
 *      Used to precisely identify error locations (file:line:column)
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <sstream>

namespace Sad {
namespace Errors {

/**
 * @struct SourceLocation
 * @brief (AR) موقع في الكود المصدري
 *        (EN) Location in source code
 * 
 * (AR) يمثل موقعاً محدداً في ملف الكود المصدري بما في ذلك:
 *      - اسم الملف
 *      - رقم السطر (يبدأ من 1)
 *      - رقم العمود (يبدأ من 1)
 *      - الإزاحة من بداية الملف
 *      - طول النص
 * 
 * (EN) Represents specific location in source code file including:
 *      - File name
 *      - Line number (1-based)
 *      - Column number (1-based)
 *      - Offset from file start
 *      - Text length
 */
struct SourceLocation {
    std::string filename;    ///< (AR) اسم الملف / (EN) File name
    size_t line;            ///< (AR) رقم السطر (1-based) / (EN) Line number (1-based)
    size_t column;          ///< (AR) رقم العمود (1-based) / (EN) Column number (1-based)
    size_t offset;          ///< (AR) الإزاحة من البداية / (EN) Offset from start
    size_t length;          ///< (AR) طول النص / (EN) Text length
    
    /**
     * @brief (AR) البناء الافتراضي - موقع غير صالح
     *        (EN) Default constructor - invalid location
     * 
     * (AR) ينشئ موقعاً غير صالح (line = 0, column = 0)
     * (EN) Creates invalid location (line = 0, column = 0)
     */
    SourceLocation()
        : filename(""), line(0), column(0), offset(0), length(0) {}
    
    /**
     * @brief (AR) بناء مع قيم
     *        (EN) Constructor with values
     * 
     * (AR) ينشئ موقعاً صالحاً مع جميع المعلومات
     * (EN) Creates valid location with all information
     * 
     * @param file (AR) اسم الملف / (EN) File name
     * @param ln (AR) رقم السطر / (EN) Line number
     * @param col (AR) رقم العمود / (EN) Column number
     * @param off (AR) الإزاحة (افتراضي 0) / (EN) Offset (default 0)
     * @param len (AR) الطول (افتراضي 1) / (EN) Length (default 1)
     */
    SourceLocation(const std::string& file, size_t ln, size_t col,
                   size_t off = 0, size_t len = 1)
        : filename(file), line(ln), column(col), offset(off), length(len) {}
    
    /**
     * @brief (AR) يتحقق من صحة الموقع
     *        (EN) Checks if location is valid
     * 
     * (AR) الموقع صالح إذا كان رقم السطر والعمود أكبر من صفر
     * (EN) Location is valid if line and column numbers are greater than zero
     * 
     * @return (AR) true إذا كان الموقع صالحاً / (EN) true if location is valid
     */
    bool isValid() const {
        return line > 0 && column > 0;
    }
    
    /**
     * @brief (AR) يحول الموقع إلى نص
     *        (EN) Converts location to string
     * 
     * (AR) يُرجع تمثيلاً نصياً للموقع بصيغة "file:line:col"
     * (EN) Returns string representation in format "file:line:col"
     * 
     * @return (AR) نص بصيغة "file:line:col" / (EN) String in format "file:line:col"
     * 
     * @example
     * @code
     * SourceLocation loc("test.ص", 5, 10);
     * std::string str = loc.toString();  // "test.ص:5:10"
     * @endcode
     */
    std::string toString() const {
        if (!isValid()) {
            return "<unknown location>";
        }
        
        std::ostringstream oss;
        if (!filename.empty()) {
            oss << filename << ":";
        }
        oss << line << ":" << column;
        return oss.str();
    }
    
    /**
     * @brief (AR) يحول الموقع إلى نص موسع
     *        (EN) Converts location to extended string
     * 
     * (AR) يُرجع تمثيلاً نصياً مفصلاً يتضمن طول النص إذا كان أكبر من 1
     * (EN) Returns detailed string representation including text length if > 1
     * 
     * @return (AR) نص مفصل / (EN) Detailed string
     * 
     * @example
     * @code
     * SourceLocation loc("test.ص", 5, 10, 42, 3);
     * std::string str = loc.toDetailedString();  // "test.ص:5:10 (length: 3)"
     * @endcode
     */
    std::string toDetailedString() const {
        std::ostringstream oss;
        oss << toString();
        if (length > 1) {
            oss << " (length: " << length << ")";
        }
        return oss.str();
    }
    
    /**
     * @brief (AR) يقارن موقعين - للترتيب
     *        (EN) Compares two locations - for ordering
     * 
     * (AR) تُستخدم للترتيب: الملف ثم السطر ثم العمود
     * (EN) Used for ordering: file then line then column
     * 
     * @param other (AR) الموقع الآخر / (EN) Other location
     * @return (AR) true إذا كان هذا الموقع أصغر / (EN) true if this location is smaller
     */
    bool operator<(const SourceLocation& other) const {
        if (filename != other.filename) return filename < other.filename;
        if (line != other.line) return line < other.line;
        return column < other.column;
    }
    
    /**
     * @brief (AR) يتحقق من تساوي موقعين
     *        (EN) Checks equality of two locations
     * 
     * @param other (AR) الموقع الآخر / (EN) Other location
     * @return (AR) true إذا كانا متساويين / (EN) true if equal
     */
    bool operator==(const SourceLocation& other) const {
        return filename == other.filename &&
               line == other.line &&
               column == other.column;
    }
    
    /**
     * @brief (AR) يتحقق من عدم تساوي موقعين
     *        (EN) Checks inequality of two locations
     * 
     * @param other (AR) الموقع الآخر / (EN) Other location
     * @return (AR) true إذا لم يكونا متساويين / (EN) true if not equal
     */
    bool operator!=(const SourceLocation& other) const {
        return !(*this == other);
    }
};

/**
 * @struct SourceRange
 * @brief (AR) نطاق في الكود المصدري
 *        (EN) Range in source code
 * 
 * (AR) يمثل نطاقاً بين نقطتين في الكود المصدري
 *      يُستخدم لتحديد منطقة كاملة (مثل تعبير أو جملة)
 * 
 * (EN) Represents range between two points in source code
 *      Used to identify complete region (like expression or statement)
 */
struct SourceRange {
    SourceLocation start;    ///< (AR) نقطة البداية / (EN) Start location
    SourceLocation end;      ///< (AR) نقطة النهاية / (EN) End location
    
    /**
     * @brief (AR) البناء الافتراضي
     *        (EN) Default constructor
     */
    SourceRange() = default;
    
    /**
     * @brief (AR) بناء مع نقطتي البداية والنهاية
     *        (EN) Constructor with start and end points
     * 
     * @param s (AR) نقطة البداية / (EN) Start location
     * @param e (AR) نقطة النهاية / (EN) End location
     */
    SourceRange(const SourceLocation& s, const SourceLocation& e)
        : start(s), end(e) {}
    
    /**
     * @brief (AR) يتحقق من صحة النطاق
     *        (EN) Checks if range is valid
     * 
     * (AR) النطاق صالح إذا كانت نقطتا البداية والنهاية صالحتين
     * (EN) Range is valid if both start and end locations are valid
     * 
     * @return (AR) true إذا كان صالحاً / (EN) true if valid
     */
    bool isValid() const {
        return start.isValid() && end.isValid();
    }
    
    /**
     * @brief (AR) يحول النطاق إلى نص
     *        (EN) Converts range to string
     * 
     * @return (AR) نص بصيغة "start to end" / (EN) String in format "start to end"
     */
    std::string toString() const {
        return start.toString() + " to " + end.toString();
    }
    
    /**
     * @brief (AR) يحسب طول النطاق (بالأحرف)
     *        (EN) Calculates range length (in characters)
     * 
     * (AR) ملاحظة: يعمل فقط إذا كان النطاق في نفس الملف
     * (EN) Note: Only works if range is in same file
     * 
     * @return (AR) طول النطاق / (EN) Range length
     */
    size_t getLength() const {
        if (!isValid() || start.filename != end.filename) {
            return 0;
        }
        return end.offset - start.offset;
    }
    
    /**
     * @brief (AR) يتحقق إذا كان موقع معين داخل النطاق
     *        (EN) Checks if location is within range
     * 
     * @param loc (AR) الموقع المراد التحقق منه / (EN) Location to check
     * @return (AR) true إذا كان الموقع داخل النطاق / (EN) true if location is within range
     */
    bool contains(const SourceLocation& loc) const {
        if (!isValid() || !loc.isValid()) {
            return false;
        }
        
        if (loc.filename != start.filename) {
            return false;
        }
        
        return !(loc < start) && (loc < end || loc == end);
    }
};

} // namespace Errors
} // namespace Sad
