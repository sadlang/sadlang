// ======================================================================
// parser_classes.h - محلل الأصناف / Class Parser
// ======================================================================
// الوصف بالعربية:
//   دوال تحليل نحوي لتعريفات الأصناف والبرمجة الكائنية
//   يحوّل الرموز (tokens) إلى عقد AST
//
// English Description:
//   Parsing functions for class definitions and OOP constructs
//   Converts tokens to AST nodes
//
// ملاحظات / Notes:
//   - جزء من Parser الرئيسي
//     Part of main Parser
//   - يُستدعى من parseStatement() عند رؤية 'صنف'
//     Called from parseStatement() when seeing 'class'
//   - يتكامل مع الـ AST الموجود
//     Integrates with existing AST
//   - توثيق ثنائي اللغة (عربي/إنجليزي)
//     Bilingual documentation (Arabic/English)
// ======================================================================

#pragma once

#include "class_nodes.h"
#include "token.h"
#include <vector>
#include <memory>

namespace Sad {
namespace Parser {

// Forward declaration
class Parser;

// ======================================================================
// دوال التحليل النحوي للأصناف / Class Parsing Functions
// ======================================================================

/**
 * @brief (AR) تحليل تعريف صنف كامل
 * @brief (EN) Parse complete class declaration
 * 
 * Syntax / النحو:
 * ```
 * صنف <اسم> [يرث <صنف_أساسي>] | [: <صنف_أساسي>]
 *     [<خصائص>]
 *     [<باني>]
 *     [<طرق>]
 *     [<هدام>]
 * نهاية
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل الرئيسي / (EN) main parser
 * @return (std::unique_ptr<AST::ClassDeclStmt>) — (AR) عقدة تعريف الصنف / (EN) class declaration node
 * 
 * مثال:
 * ```
 * صنف شخص
 *     عام نص الاسم
 *     خاص رقم العمر
 *     
 *     باني(نص اسم، رقم عمر)
 *         هذا.الاسم = اسم
 *         هذا.العمر = عمر
 *     نهاية
 *     
 *     عام دالة اطبع_معلومات()
 *         اطبع("الاسم: " + هذا.الاسم)
 *     نهاية
 * نهاية
 * ```
 */
std::unique_ptr<AST::ClassDeclStmt> parseClassDeclaration(Parser* parser);

/**
 * @brief (AR) تحليل تعريف خاصية
 * @brief (EN) Parse field declaration
 * 
 * Syntax / النحو:
 * ```
 * [عام|خاص|محمي] [ثابت] <نوع> <اسم> [= <قيمة>]
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @param visibility (AST::Visibility) — (AR) الرؤية الحالية / (EN) current visibility
 * @return (std::unique_ptr<AST::FieldDecl>) — (AR) عقدة تعريف خاصية / (EN) field declaration node
 * 
 * مثال:
 * ```
 * عام نص الاسم = "افتراضي"
 * خاص رقم العمر
 * ثابت عام رقم العدد_الكلي = 0
 * ```
 */
std::unique_ptr<AST::FieldDecl> parseFieldDeclaration(Parser* parser, AST::Visibility visibility);

/**
 * @brief (AR) تحليل تعريف طريقة
 * @brief (EN) Parse method declaration
 * 
 * Syntax / النحو:
 * ```
 * [عام|خاص|محمي] [ثابت] [افتراضي] دالة [<نوع_إرجاع>] <اسم>([<معاملات>])
 *     <جسم_الطريقة>
 * نهاية
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @param visibility (AST::Visibility) — (AR) الرؤية / (EN) visibility
 * @return (std::unique_ptr<AST::MethodDecl>) — (AR) عقدة تعريف طريقة / (EN) method declaration node
 * 
 * مثال:
 * ```
 * عام دالة رقم احسب_العمر()
 *     إرجاع هذا.العمر
 * نهاية
 * 
 * ثابت عام دالة نص احصل_على_نوع()
 *     إرجاع "شخص"
 * نهاية
 * ```
 */
std::unique_ptr<AST::MethodDecl> parseMethodDeclaration(Parser* parser, AST::Visibility visibility);

/**
 * @brief (AR) تحليل تعريف باني
 * @brief (EN) Parse constructor declaration
 * 
 * Syntax / النحو:
 * ```
 * باني([<معاملات>]) [: <قائمة_تهيئة>]
 *     <جسم_الباني>
 * نهاية
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::unique_ptr<AST::ConstructorDecl>) — (AR) عقدة تعريف باني / (EN) constructor declaration node
 * 
 * مثال:
 * ```
 * باني(نص اسم) : الاسم(اسم)
 *     اطبع("تم إنشاء كائن")
 * نهاية
 * ```
 */
std::unique_ptr<AST::ConstructorDecl> parseConstructorDeclaration(Parser* parser);

/**
 * @brief (AR) تحليل تعريف هدام
 * @brief (EN) Parse destructor declaration
 * 
 * Syntax / النحو:
 * ```
 * مدمر()
 *     <جسم_الهدام>
 * نهاية
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::unique_ptr<AST::DestructorDecl>) — (AR) عقدة تعريف هدام / (EN) destructor declaration node
 * 
 * مثال:
 * ```
 * مدمر()
 *     اطبع("تم حذف الكائن")
 * نهاية
 * ```
 */
std::unique_ptr<AST::DestructorDecl> parseDestructorDeclaration(Parser* parser);

/**
 * @brief (AR) تحليل قائمة تهيئة الباني
 * @brief (EN) Parse constructor initializer list
 * 
 * Syntax / النحو:
 * ```
 * : <خاصية>(<قيمة>)[، <خاصية>(<قيمة>)]*
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::vector<AST::FieldInitializer>) — (AR) قائمة التهيئة / (EN) initializer list
 * 
 * مثال:
 * ```
 * : الاسم("افتراضي")، العمر(0)
 * ```
 */
std::vector<AST::FieldInitializer> parseInitializerList(Parser* parser);

/**
 * @brief (AR) تحليل قائمة معاملات
 * @brief (EN) Parse parameter list
 * 
 * Syntax / النحو:
 * ```
 * (<نوع> <اسم> [= <قيمة_افتراضية>][، <نوع> <اسم> [= <قيمة_افتراضية>]]*)
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::vector<AST::Parameter>) — (AR) قائمة المعاملات / (EN) parameter list
 * 
 * مثال:
 * ```
 * (نص الاسم، رقم العمر = 0، منطق نشط = صحيح)
 * ```
 */
std::vector<AST::Parameter> parseParameterList(Parser* parser);

/**
 * @brief (AR) تحليل كلمة رؤية (عام، خاص، محمي)
 * @brief (EN) Parse visibility keyword (public, private, protected)
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (AST::Visibility) — (AR) الرؤية / (EN) visibility
 * 
 * ملاحظات إضافية:
 * - AR: يُرجع PUBLIC إذا لم تُوجد كلمة رؤية
 * - EN: Returns PUBLIC if no visibility keyword found
 */
AST::Visibility parseVisibility(Parser* parser);

// ======================================================================
// دوال التحليل النحوي للتعبيرات الكائنية / OOP Expression Parsing
// ======================================================================

/**
 * @brief (AR) تحليل تعبير 'جديد' (new)
 * @brief (EN) Parse 'new' expression
 * 
 * Syntax / النحو:
 * ```
 * جديد <اسم_صنف>([<معاملات>])
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::unique_ptr<AST::NewExpr>) — (AR) عقدة إنشاء كائن / (EN) object creation node
 * 
 * مثال:
 * ```
 * شخص أحمد = جديد شخص("أحمد"، 25)
 * ```
 */
std::unique_ptr<AST::NewExpr> parseNewExpression(Parser* parser);

/**
 * @brief (AR) تحليل الوصول لعضو (.)
 * @brief (EN) Parse member access (.)
 * 
 * Syntax / النحو:
 * ```
 * <كائن>.<عضو>
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @param object (std::unique_ptr<AST::Expr>) — (AR) الكائن / (EN) object
 * @return (std::unique_ptr<AST::MemberAccessExpr>) — (AR) عقدة الوصول / (EN) access node
 * 
 * مثال:
 * ```
 * أحمد.الاسم
 * هذا.العمر
 * ```
 */
std::unique_ptr<AST::MemberAccessExpr> parseMemberAccess(Parser* parser, std::unique_ptr<AST::Expr> object);

/**
 * @brief (AR) تحليل استدعاء طريقة
 * @brief (EN) Parse method call
 * 
 * Syntax / النحو:
 * ```
 * <كائن>.<طريقة>([<معاملات>])
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @param object (std::unique_ptr<AST::Expr>) — (AR) الكائن / (EN) object
 * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
 * @return (std::unique_ptr<AST::MethodCallExpr>) — (AR) عقدة استدعاء / (EN) call node
 * 
 * مثال:
 * ```
 * أحمد.اطبع_معلومات()
 * هذا.احسب_العمر()
 * ```
 */
std::unique_ptr<AST::MethodCallExpr> parseMethodCall(Parser* parser, std::unique_ptr<AST::Expr> object, const std::string& methodName);

/**
 * @brief (AR) تحليل تعبير 'هذا' (this)
 * @brief (EN) Parse 'this' expression
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::unique_ptr<AST::ThisExpr>) — (AR) عقدة 'هذا' / (EN) 'this' node
 * 
 * مثال:
 * ```
 * هذا.الاسم
 * ```
 */
std::unique_ptr<AST::ThisExpr> parseThisExpression(Parser* parser);

/**
 * @brief (AR) تحليل تعبير 'الأساس' (super)
 * @brief (EN) Parse 'super' expression
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::unique_ptr<AST::SuperExpr>) — (AR) عقدة 'الأساس' / (EN) 'super' node
 * 
 * مثال:
 * ```
 * الأساس.طريقة()
 * ```
 */
std::unique_ptr<AST::SuperExpr> parseSuperExpression(Parser* parser);

/**
 * @brief (AR) تحليل الوصول لعضو ثابت (::)
 * @brief (EN) Parse static member access (::)
 * 
 * Syntax / النحو:
 * ```
 * <صنف>::<عضو_ثابت>
 * ```
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @param className (std::string) — (AR) اسم الصنف / (EN) class name
 * @return (std::unique_ptr<AST::Expr>) — (AR) عقدة الوصول / (EN) access node
 * 
 * مثال:
 * ```
 * شخص::العدد_الكلي
 * Math::PI
 * ```
 */
std::unique_ptr<AST::Expr> parseStaticMemberAccess(Parser* parser, const std::string& className);

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

/**
 * @brief (AR) فحص هل الرمز الحالي كلمة رؤية؟
 * @brief (EN) Check if current token is visibility keyword
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (bool) — (AR) true إذا كان كلمة رؤية / (EN) true if visibility keyword
 */
bool isVisibilityKeyword(Parser* parser);

/**
 * @brief (AR) فحص هل الرمز الحالي نوع بيانات؟
 * @brief (EN) Check if current token is data type
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (bool) — (AR) true إذا كان نوع بيانات / (EN) true if data type
 */
bool isDataType(Parser* parser);

/**
 * @brief (AR) تحليل نوع بيانات
 * @brief (EN) Parse data type
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (Type*) — (AR) مؤشر للنوع / (EN) pointer to type
 */
Type* parseType(Parser* parser);

/**
 * @brief (AR) فحص هل نحن داخل تعريف صنف؟
 * @brief (EN) Check if we're inside class definition
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (bool) — (AR) true إذا كنا داخل صنف / (EN) true if inside class
 */
bool isInsideClass(Parser* parser);

/**
 * @brief (AR) فحص هل نحن داخل طريقة؟
 * @brief (EN) Check if we're inside method
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (bool) — (AR) true إذا كنا داخل طريقة / (EN) true if inside method
 */
bool isInsideMethod(Parser* parser);

/**
 * @brief (AR) الحصول على الصنف الحالي
 * @brief (EN) Get current class
 * 
 * @param parser (Parser*) — (AR) المحلل / (EN) parser
 * @return (std::string) — (AR) اسم الصنف الحالي / (EN) current class name
 */
std::string getCurrentClassName(Parser* parser);

} // namespace Parser
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
