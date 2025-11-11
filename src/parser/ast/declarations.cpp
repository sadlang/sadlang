/**
 * @file declarations.cpp
 * @brief (AR) تنفيذ عُقد الشجرة النحوية التجريدية للتصريحات.
 *        (EN) Implementation of AST declaration nodes.
 * 
 * @details
 *   (AR) يحتوي على تنفيذ دوال toString() لجميع أنواع التصريحات في لغة ص،
 *        مثل الدوال والأصناف والحقول والطرق والتعدادات والاستيراد والتصدير.
 *   (EN) Contains implementation of toString() functions for all declaration
 *        types in Sad language, such as functions, classes, fields, methods,
 *        enums, imports, and exports.
 * 
 * @author Sad Language Team
 * @date 11 November 2025
 * @copyright Copyright (c) 2025 Sad Language Project
 */

#include "../../../include/parser/ast/declarations.h"
#include <sstream>

namespace Sad {
namespace AST {

// =========================================================================
// Helper Functions / دوال مساعدة
// =========================================================================

namespace {
    /**
     * @brief (AR) يحول معدّل الوصول إلى تمثيل نصي.
     *        (EN) Converts access modifier enum to its string representation.
     * 
     * @param access (AR) معدّل الوصول (عام، خاص، محمي).
     *               (EN) AccessModifier enum value (public, private, protected).
     * @return (AR) سلسلة نصية تمثل معدّل الوصول.
     *         (EN) String representation of access modifier.
     */
    std::string accessToString(AccessModifier access) {
        switch (access) {
            case AccessModifier::PUBLIC:    return "public";
            case AccessModifier::PRIVATE:   return "private";
            case AccessModifier::PROTECTED: return "protected";
            default:                        return "";
        }
    }
    
    /**
     * @brief (AR) يحول نوع البيانات إلى تمثيل نصي.
     *        (EN) Converts data type enum to its string representation.
     * 
     * @param type (AR) نوع البيانات (صحيح، عائم، نص، إلخ).
     *             (EN) DataType enum value (int, float, string, etc).
     * @return (AR) سلسلة نصية تمثل نوع البيانات.
     *         (EN) String representation of data type.
     */
    std::string typeToString(Data::DataType type) {
        switch (type) {
            case Data::DataType::INTEGER:   return "int";
            case Data::DataType::FLOAT:     return "float";
            case Data::DataType::STRING:    return "string";
            case Data::DataType::BOOLEAN:   return "bool";
            case Data::DataType::NONE:      return "none";
            case Data::DataType::ARRAY:     return "array";
            case Data::DataType::MAP:       return "map";
            case Data::DataType::TUPLE:     return "tuple";
            case Data::DataType::FUNCTION:  return "function";
            case Data::DataType::OBJECT:    return "object";
            case Data::DataType::ENUM:      return "enum";
            case Data::DataType::BYTE:      return "byte";
            default:                        return "unknown";
        }
    }
}

// =========================================================================
// FunctionDecl Implementation / تنفيذ تصريح الدالة
// =========================================================================

/**
 * @brief (AR) يحول تصريح الدالة إلى تمثيل نصي.
 *        (EN) Converts function declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح الدالة بالكامل (الاسم، المعاملات، النوع، الجسم).
 *         (EN) String representing complete function declaration (name, params, type, body).
 * 
 * @note (AR) يتضمن كلمة export إذا كانت الدالة مصدَّرة.
 *       (EN) Includes 'export' keyword if function is exported.
 */
std::string FunctionDecl::toString() const {
    std::ostringstream oss;
    
    if (isExported) {
        oss << "export ";
    }
    
    oss << "func " << name << "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
        if (parameters[i].type != Data::DataType::UNKNOWN) {
            oss << ": " << typeToString(parameters[i].type);
        }
        if (parameters[i].defaultValue) {
            oss << " = " << parameters[i].defaultValue->toString();
        }
    }
    
    oss << ")";
    
    if (returnType != Data::DataType::NONE) {
        oss << " -> " << typeToString(returnType);
    }
    
    oss << " " << body->toString();
    return oss.str();
}

// =========================================================================
// ClassDecl Implementation / تنفيذ تصريح الصنف
// =========================================================================

/**
 * @brief (AR) يحول تصريح الصنف إلى تمثيل نصي.
 *        (EN) Converts class declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح الصنف (الاسم، الصنف الأب، الأعضاء).
 *         (EN) String representing class declaration (name, superclass, members).
 * 
 * @note (AR) يتضمن extends إذا كان هناك صنف أب.
 *       (EN) Includes 'extends' if there is a superclass.
 */
std::string ClassDecl::toString() const {
    std::ostringstream oss;
    
    if (isExported) {
        oss << "export ";
    }
    
    oss << "class " << name;
    
    if (!superclass.empty()) {
        oss << " extends " << superclass;
    }
    
    oss << " {\n";
    
    for (const auto& member : members) {
        oss << "  " << member->toString() << "\n";
    }
    
    oss << "}";
    return oss.str();
}

// =========================================================================
// FieldDecl Implementation / تنفيذ تصريح الحقل
// =========================================================================

/**
 * @brief (AR) يحول تصريح الحقل إلى تمثيل نصي.
 *        (EN) Converts field declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح الحقل (معدّل الوصول، static، الاسم، النوع، القيمة الأولية).
 *         (EN) String representing field declaration (access, static, name, type, initializer).
 * 
 * @note (AR) يتضمن القيمة الأولية إن وُجدت.
 *       (EN) Includes initializer if present.
 */
std::string FieldDecl::toString() const {
    std::ostringstream oss;
    
    std::string accessStr = accessToString(access);
    if (!accessStr.empty()) {
        oss << accessStr << " ";
    }
    
    if (isStatic) {
        oss << "static ";
    }
    
    oss << "var " << name;
    
    if (type != Data::DataType::UNKNOWN) {
        oss << ": " << typeToString(type);
    }
    
    if (initializer) {
        oss << " = " << initializer->toString();
    }
    
    oss << ";";
    return oss.str();
}

// =========================================================================
// MethodDecl Implementation / تنفيذ تصريح الطريقة
// =========================================================================

/**
 * @brief (AR) يحول تصريح الطريقة إلى تمثيل نصي.
 *        (EN) Converts method declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح الطريقة (معدّل الوصول، static، virtual، override، الاسم، المعاملات، النوع، الجسم).
 *         (EN) String representing method declaration (access, static, virtual, override, name, params, type, body).
 * 
 * @note (AR) يتضمن الكلمات المفتاحية الإضافية حسب خصائص الطريقة.
 *       (EN) Includes additional keywords based on method properties.
 */
std::string MethodDecl::toString() const {
    std::ostringstream oss;
    
    std::string accessStr = accessToString(access);
    if (!accessStr.empty()) {
        oss << accessStr << " ";
    }
    
    if (isStatic) {
        oss << "static ";
    }
    
    if (isVirtual) {
        oss << "virtual ";
    }
    
    if (isOverride) {
        oss << "override ";
    }
    
    oss << "func " << name << "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
        if (parameters[i].type != Data::DataType::UNKNOWN) {
            oss << ": " << typeToString(parameters[i].type);
        }
    }
    
    oss << ")";
    
    if (returnType != Data::DataType::NONE) {
        oss << " -> " << typeToString(returnType);
    }
    
    oss << " " << body->toString();
    return oss.str();
}

// =========================================================================
// ConstructorDecl Implementation / تنفيذ تصريح الباني
// =========================================================================

/**
 * @brief (AR) يحول تصريح الباني إلى تمثيل نصي.
 *        (EN) Converts constructor declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح الباني (المعاملات، استدعاء باني الصنف الأب، الجسم).
 *         (EN) String representing constructor declaration (params, super call, body).
 * 
 * @note (AR) يتضمن استدعاء باني الصنف الأب (super) إن وُجد.
 *       (EN) Includes super constructor call if present.
 */
std::string ConstructorDecl::toString() const {
    std::ostringstream oss;
    
    oss << "constructor(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
        if (parameters[i].type != Data::DataType::UNKNOWN) {
            oss << ": " << typeToString(parameters[i].type);
        }
    }
    
    oss << ")";
    
    if (!superArgs.empty()) {
        oss << " : super(";
        for (size_t i = 0; i < superArgs.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << superArgs[i]->toString();
        }
        oss << ")";
    }
    
    oss << " " << body->toString();
    return oss.str();
}

// =========================================================================
// EnumDecl Implementation / تنفيذ تصريح التعداد
// =========================================================================

/**
 * @brief (AR) يحول تصريح التعداد إلى تمثيل نصي.
 *        (EN) Converts enum declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح التعداد (الاسم، الأعضاء مع قيمهم).
 *         (EN) String representing enum declaration (name, members with values).
 * 
 * @note (AR) يتضمن قيم الأعضاء إن كانت محددة صراحة.
 *       (EN) Includes member values if explicitly specified.
 */
std::string EnumDecl::toString() const {
    std::ostringstream oss;
    
    if (isExported) {
        oss << "export ";
    }
    
    oss << "enum " << name << " {\n";
    
    for (size_t i = 0; i < members.size(); ++i) {
        oss << "  " << members[i].name;
        if (members[i].value) {
            oss << " = " << members[i].value->toString();
        }
        if (i < members.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "}";
    return oss.str();
}

// =========================================================================
// ImportStmt Implementation / تنفيذ جملة الاستيراد
// =========================================================================

/**
 * @brief (AR) يحول جملة الاستيراد إلى تمثيل نصي.
 *        (EN) Converts import statement to string representation.
 * 
 * @return (AR) نص يمثل جملة الاستيراد (مسار الوحدة، الاسم البديل، الرموز المحددة).
 *         (EN) String representing import statement (module path, alias, specific symbols).
 * 
 * @note (AR) يدعم أنماط الاستيراد المختلفة: الكل، محدد، مع اسم بديل.
 *       (EN) Supports different import patterns: all, specific, with alias.
 */
std::string ImportStmt::toString() const {
    std::ostringstream oss;
    
    if (importAll) {
        oss << "import * from \"" << modulePath << "\"";
    } else if (!symbols.empty()) {
        oss << "from \"" << modulePath << "\" import ";
        for (size_t i = 0; i < symbols.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << symbols[i];
        }
    } else {
        oss << "import \"" << modulePath << "\"";
        if (!alias.empty()) {
            oss << " as " << alias;
        }
    }
    
    oss << ";";
    return oss.str();
}

} // namespace AST
} // namespace Sad
