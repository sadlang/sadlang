/**
 * @file data_types.h
 * @brief Data type definitions for Sad language / تعريفات أنواع البيانات للغة ص
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_DATA_TYPES_H
#define SAD_DATA_TYPES_H

namespace Sad {
namespace Data {

/**
 * @brief Data types supported by Sad language / أنواع البيانات المدعومة في لغة ص
 * 
 * These types correspond to the types defined in the language specification.
 * هذه الأنواع تتوافق مع الأنواع المعرّفة في مواصفات اللغة.
 */
// Windows defines ERROR as a macro; undefine to avoid conflict
// ويندوز يعرّف ERROR كماكرو؛ إلغاء التعريف لتجنب التعارض
#ifdef ERROR
#undef ERROR
#endif

enum class DataType {
    UNKNOWN,      ///< Unknown/unresolved type / نوع غير معروف أو غير محدد
    INTEGER,      ///< رقم / Integer (int64_t)
    FLOAT,        ///< عشري / Float (double)
    STRING,       ///< نص / String
    BOOLEAN,      ///< منطقي / Boolean
    NONE,         ///< لاشيء / None/null
    ARRAY,        ///< مصفوفة / Array
    MAP,          ///< قاموس / Dictionary/Map
    TUPLE,        ///< ثنائي/ثلاثي / Tuple
    FUNCTION,     ///< دالة / Function
    OBJECT,       ///< كائن / Object (class instance)
    ENUM,         ///< تعداد / Enum
    BYTE,         ///< بايت / Byte
    ERROR         ///< خطأ / Error/Exception type
};

} // namespace Data
} // namespace Sad

#endif // SAD_DATA_TYPES_H
