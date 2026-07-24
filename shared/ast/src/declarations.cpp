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

#include "declarations.h"
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
     *             (EN) SadTypeKind enum value (int, float, string, etc).
     * @return (AR) سلسلة نصية تمثل نوع البيانات.
     *         (EN) String representation of data type.
     */
    std::string typeToString(Types::SadTypeKind type) {
        switch (type) {
            case Types::SadTypeKind::Integer:   return "int";
            case Types::SadTypeKind::Float:     return "float";
            case Types::SadTypeKind::String:    return "string";
            case Types::SadTypeKind::Boolean:   return "bool";
            case Types::SadTypeKind::Void:      return "none";
            case Types::SadTypeKind::Array:     return "array";
            case Types::SadTypeKind::Map:       return "map";
            case Types::SadTypeKind::Tuple:     return "tuple";
            case Types::SadTypeKind::Function:  return "function";
            case Types::SadTypeKind::Class:    return "object";
            case Types::SadTypeKind::Enum:      return "enum";
            case Types::SadTypeKind::Byte:      return "byte";
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
    
    // (AR) إضافة المُزخرِفات في البداية
    // (EN) Add decorators at the beginning
    for (const auto& decorator : decorators) {
        oss << decorator->toString() << "\n";
    }
    
    if (isExported) {
        oss << "export ";
    }
    
    // (AR) إذا كانت الدالة الرئيسية، استخدام "main" بدلاً من اسم الدالة
    // (EN) If this is main function, use "main" instead of function name
    if (isMainFunction) {
        oss << "func main(";
    } else {
        oss << "func " << name << "(";
    }
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
        if (parameters[i].type != Types::SadTypeKind::Unknown) {
            oss << ": " << typeToString(parameters[i].type);
        }
        if (parameters[i].defaultValue) {
            oss << " = " << parameters[i].defaultValue->toString();
        }
    }
    
    oss << ")";
    
    if (returnType != Types::SadTypeKind::Void) {
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
    
    // (AR) طباعة جميع الأصناف الأساسية / (EN) Print all base classes
    if (!superclasses.empty()) {
        oss << " extends ";
        for (size_t i = 0; i < superclasses.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << superclasses[i];
        }
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
    
    if (type != Types::SadTypeKind::Unknown) {
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
        if (parameters[i].type != Types::SadTypeKind::Unknown) {
            oss << ": " << typeToString(parameters[i].type);
        }
    }
    
    oss << ")";
    
    if (returnType != Types::SadTypeKind::Void) {
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
        if (parameters[i].type != Types::SadTypeKind::Unknown) {
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
        const auto &member = members[i];
        oss << "  " << member.name;
        // (AR) حمولة موضعيّة (ADT): اسم(نوع اسم، ...) — النوع قبل الاسم (قاعدة ص)
        // (EN) Positional payload (ADT): name(type field, ...) — type before name
        if (member.hasData()) {
            oss << "(";
            for (size_t f = 0; f < member.fields.size(); ++f) {
                if (f > 0) {
                    oss << ", ";
                }
                const std::string &ft = f < member.fieldTypes.size() ? member.fieldTypes[f] : "";
                if (!ft.empty()) {
                    oss << ft << " ";
                }
                oss << member.fields[f];
            }
            oss << ")";
        }
        if (member.value) {
            oss << " = " << member.value->toString();
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
// (AR) ملاحظة: تم نقل ImportStmt إلى module_nodes.h/cpp
// (EN) Note: ImportStmt moved to module_nodes.h/cpp
// =========================================================================

// =========================================================================
// TemplateFunctionDecl Implementation / تنفيذ تصريح دالة القالب
// =========================================================================

/**
 * @brief (AR) يحول تصريح دالة القالب إلى تمثيل نصي.
 *        (EN) Converts template function declaration to string representation.
 * 
 * @return (AR) نص يمثل تصريح دالة القالب (معاملات الأنواع، الاسم، المعاملات، الجسم).
 *         (EN) String representing template function declaration.
 */
std::string TemplateFunctionDecl::toString() const {
    std::ostringstream oss;
    
    if (isExported) {
        oss << "export ";
    }
    
    // (AR) طباعة معاملات الأنواع / (EN) Print type parameters
    oss << "template<";
    for (size_t i = 0; i < typeParameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "typename " << typeParameters[i].name;
        if (!typeParameters[i].constraint.empty()) {
            oss << ": " << typeParameters[i].constraint;
        }
    }
    oss << ">\n";
    
    // (AR) طباعة تصريح الدالة / (EN) Print function declaration
    oss << "func " << name << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name << ": " << typeToString(parameters[i].type);
    }
    oss << ") -> " << typeToString(returnType);
    
    if (body) {
        oss << " " << body->toString();
    }
    
    return oss.str();
}

// =========================================================================
// TemplateClassDecl Implementation / تنفيذ تصريح صنف القالب
// =========================================================================

/**
 * @brief (AR) يحول تصريح صنف القالب إلى تمثيل نصي.
 *        (EN) Converts template class declaration to string representation.
 */
std::string TemplateClassDecl::toString() const {
    std::ostringstream oss;
    
    if (isExported) {
        oss << "export ";
    }
    
    // (AR) طباعة معاملات الأنواع / (EN) Print type parameters
    oss << "template<";
    for (size_t i = 0; i < typeParameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "typename " << typeParameters[i].name;
        if (!typeParameters[i].constraint.empty()) {
            oss << ": " << typeParameters[i].constraint;
        }
    }
    oss << ">\n";
    
    // (AR) طباعة تصريح الصنف / (EN) Print class declaration
    oss << "class " << name;
    
    if (!superclasses.empty()) {
        oss << " extends ";
        for (size_t i = 0; i < superclasses.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << superclasses[i];
        }
    }
    
    oss << " {\n";
    for (const auto& member : members) {
        oss << "  " << member->toString() << "\n";
    }
    oss << "}";
    
    return oss.str();
}

// =========================================================================
// TemplateInstantiation Implementation / تنفيذ تنفيذ القالب
// =========================================================================

/**
 * @brief (AR) يحول تنفيذ القالب إلى تمثيل نصي.
 *        (EN) Converts template instantiation to string representation.
 */
std::string TemplateInstantiation::toString() const {
    std::ostringstream oss;
    
    oss << templateName << "<";
    for (size_t i = 0; i < typeArguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << typeToString(typeArguments[i]);
    }
    oss << ">";
    
    return oss.str();
}

// =========================================================================
// NamespaceDecl Implementation / تنفيذ تصريح فضاء الأسماء
// =========================================================================

/**
 * @brief (AR) يحول تصريح فضاء الأسماء إلى تمثيل نصي.
 *        (EN) Converts namespace declaration to string representation.
 */
std::string NamespaceDecl::toString() const {
    std::ostringstream oss;
    
    oss << "namespace " << name << " {\n";
    for (const auto& member : members) {
        oss << "  " << member->toString() << "\n";
    }
    oss << "}";
    
    return oss.str();
}

// =========================================================================
// OperatorDecl Implementation / تنفيذ تصريح تحميل العامل
// =========================================================================

/**
 * @brief (AR) يحول تصريح تحميل العامل إلى تمثيل نصي.
 *        (EN) Converts operator overload declaration to string representation.
 */
std::string OperatorDecl::toString() const {
    std::ostringstream oss;
    
    oss << accessToString(access) << " operator" << operatorSymbol << "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name << ": " << typeToString(parameters[i].type);
    }
    oss << ") -> " << typeToString(returnType);
    
    if (body) {
        oss << " " << body->toString();
    }
    
    return oss.str();
}

} // namespace AST
} // namespace Sad
