// ======================================================================
// interpreter_classes.h - مُفسّر الأصناف / Class Interpreter
// ======================================================================
// الوصف بالعربية:
//   دوال تنفيذ لعقد AST الخاصة بالبرمجة الكائنية
//   يُنفّذ تعريفات الأصناف وإنشاء الكائنات واستدعاء الطرق
//
// English Description:
//   Execution functions for OOP AST nodes
//   Executes class definitions, object creation, and method calls
//
// ملاحظات / Notes:
//   - جزء من Interpreter الرئيسي
//     Part of main Interpreter
//   - يستخدم ClassManager و ObjectManager
//     Uses ClassManager and ObjectManager
//   - يتكامل مع نظام الزيارة (Visitor Pattern)
//     Integrates with Visitor Pattern
//   - توثيق ثنائي اللغة (عربي/إنجليزي)
//     Bilingual documentation (Arabic/English)
// ======================================================================

#pragma once

#include "class_nodes.h"
#include "object_instance.h"
#include "class_manager.h"
#include "object_manager.h"
#include <memory>

namespace Sad {
namespace Interpreter {

// Forward declaration
class Interpreter;

// ======================================================================
// دوال تنفيذ تعريفات الأصناف / Class Definition Execution
// ======================================================================

/**
 * @brief (AR) تنفيذ تعريف صنف
 * @brief (EN) Execute class declaration
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param node (AST::ClassDeclStmt*) — (AR) عقدة تعريف الصنف / (EN) class declaration node
 * 
 * ملاحظات إضافية:
 * - AR: يُسجّل الصنف في ClassManager
 * - EN: Registers class in ClassManager
 * - AR: يفحص الوراثة والتأكد من وجود الصنف الأساسي
 * - EN: Checks inheritance and base class existence
 * - AR: يُخزّن الباني والهدام والطرق
 * - EN: Stores constructor, destructor, and methods
 * 
 * مثال:
 * ```
 * صنف شخص
 *     عام نص الاسم
 *     باني(نص اسم)
 *         هذا.الاسم = اسم
 *     نهاية
 * نهاية
 * ```
 */
void executeClassDeclaration(Interpreter* interpreter, AST::ClassDeclStmt* node);

// ======================================================================
// دوال تنفيذ إنشاء الكائنات / Object Creation Execution
// ======================================================================

/**
 * @brief (AR) تنفيذ تعبير 'جديد' (new)
 * @brief (EN) Execute 'new' expression
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param node (AST::NewExpr*) — (AR) عقدة إنشاء كائن / (EN) new expression node
 * @return (Data::Value) — (AR) قيمة الكائن الجديد / (EN) new object value
 * 
 * ملاحظات إضافية:
 * - AR: يُنشئ كائن جديد عبر ObjectManager
 * - EN: Creates new object via ObjectManager
 * - AR: يستدعي الباني مع المعاملات
 * - EN: Calls constructor with arguments
 * - AR: يُرجع مرجع للكائن
 * - EN: Returns reference to object
 * 
 * مثال:
 * ```
 * شخص أحمد = جديد شخص("أحمد")
 * ```
 */
Data::Value executeNewExpression(Interpreter* interpreter, AST::NewExpr* node);

/**
 * @brief (AR) استدعاء الباني
 * @brief (EN) Call constructor
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param object (Data::ObjectInstance*) — (AR) الكائن / (EN) object
 * @param constructor (AST::ConstructorDecl*) — (AR) الباني / (EN) constructor
 * @param arguments (std::vector<Data::Value>) — (AR) المعاملات / (EN) arguments
 * 
 * ملاحظات إضافية:
 * - AR: يُنفّذ قائمة التهيئة أولاً
 * - EN: Executes initializer list first
 * - AR: ثم يُنفّذ جسم الباني
 * - EN: Then executes constructor body
 * - AR: يضبط علامة isConstructed
 * - EN: Sets isConstructed flag
 */
void callConstructor(Interpreter* interpreter, Data::ObjectInstance* object, 
                     AST::ConstructorDecl* constructor, const std::vector<Data::Value>& arguments);

/**
 * @brief (AR) استدعاء الهدام
 * @brief (EN) Call destructor
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param object (Data::ObjectInstance*) — (AR) الكائن / (EN) object
 * 
 * ملاحظات إضافية:
 * - AR: يُستدعى تلقائياً عند حذف الكائن
 * - EN: Called automatically when object is deleted
 * - AR: يستدعي هدام الصنف الأساسي أولاً
 * - EN: Calls base class destructor first
 */
void callDestructor(Interpreter* interpreter, Data::ObjectInstance* object);

// ======================================================================
// دوال تنفيذ الوصول للأعضاء / Member Access Execution
// ======================================================================

/**
 * @brief (AR) تنفيذ الوصول لخاصية
 * @brief (EN) Execute field access
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param node (AST::MemberAccessExpr*) — (AR) عقدة الوصول / (EN) access node
 * @return (Data::Value) — (AR) قيمة الخاصية / (EN) field value
 * 
 * ملاحظات إضافية:
 * - AR: يفحص الرؤية (عام، خاص، محمي)
 * - EN: Checks visibility (public, private, protected)
 * - AR: يبحث في الكائن والأصناف الأساسية
 * - EN: Searches in object and base classes
 * 
 * مثال:
 * ```
 * نص اسم = أحمد.الاسم
 * ```
 */
Data::Value executeFieldAccess(Interpreter* interpreter, AST::MemberAccessExpr* node);

/**
 * @brief (AR) تنفيذ تعيين قيمة خاصية
 * @brief (EN) Execute field assignment
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param object (Data::ObjectInstance*) — (AR) الكائن / (EN) object
 * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
 * @param value (Data::Value) — (AR) القيمة الجديدة / (EN) new value
 * 
 * ملاحظات إضافية:
 * - AR: يفحص الرؤية والنوع
 * - EN: Checks visibility and type
 * 
 * مثال:
 * ```
 * أحمد.الاسم = "محمد"
 * ```
 */
void executeFieldAssignment(Interpreter* interpreter, Data::ObjectInstance* object,
                            const std::string& fieldName, const Data::Value& value);

// ======================================================================
// دوال تنفيذ استدعاء الطرق / Method Call Execution
// ======================================================================

/**
 * @brief (AR) تنفيذ استدعاء طريقة
 * @brief (EN) Execute method call
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param node (AST::MethodCallExpr*) — (AR) عقدة الاستدعاء / (EN) call node
 * @return (Data::Value) — (AR) قيمة الإرجاع / (EN) return value
 * 
 * ملاحظات إضافية:
 * - AR: يفحص الرؤية ووجود الطريقة
 * - EN: Checks visibility and method existence
 * - AR: يُنشئ سياق جديد (scope) للطريقة
 * - EN: Creates new scope for method
 * - AR: يربط 'هذا' بالكائن الحالي
 * - EN: Binds 'this' to current object
 * 
 * مثال:
 * ```
 * أحمد.اطبع_معلومات()
 * رقم عمر = أحمد.احسب_العمر()
 * ```
 */
Data::Value executeMethodCall(Interpreter* interpreter, AST::MethodCallExpr* node);

/**
 * @brief (AR) تنفيذ طريقة
 * @brief (EN) Execute method
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param object (Data::ObjectInstance*) — (AR) الكائن / (EN) object
 * @param method (Data::ClassMethod*) — (AR) الطريقة / (EN) method
 * @param arguments (std::vector<Data::Value>) — (AR) المعاملات / (EN) arguments
 * @return (Data::Value) — (AR) قيمة الإرجاع / (EN) return value
 */
Data::Value executeMethod(Interpreter* interpreter, Data::ObjectInstance* object,
                          Data::ClassMethod* method, const std::vector<Data::Value>& arguments);

/**
 * @brief (AR) تنفيذ طريقة ثابتة
 * @brief (EN) Execute static method
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param classType (Data::ClassType*) — (AR) الصنف / (EN) class
 * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
 * @param arguments (std::vector<Data::Value>) — (AR) المعاملات / (EN) arguments
 * @return (Data::Value) — (AR) قيمة الإرجاع / (EN) return value
 * 
 * مثال:
 * ```
 * نص نوع = شخص::احصل_على_نوع()
 * ```
 */
Data::Value executeStaticMethod(Interpreter* interpreter, Data::ClassType* classType,
                                const std::string& methodName, const std::vector<Data::Value>& arguments);

// ======================================================================
// دوال تنفيذ 'هذا' و 'الأساس' / This and Super Execution
// ======================================================================

/**
 * @brief (AR) تنفيذ تعبير 'هذا'
 * @brief (EN) Execute 'this' expression
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param node (AST::ThisExpr*) — (AR) عقدة 'هذا' / (EN) 'this' node
 * @return (Data::Value) — (AR) قيمة الكائن الحالي / (EN) current object value
 * 
 * ملاحظات إضافية:
 * - AR: يُرجع مرجع للكائن الذي استُدعيت عليه الطريقة
 * - EN: Returns reference to object on which method was called
 * - AR: خطأ إذا استُخدم خارج طريقة
 * - EN: Error if used outside method
 * 
 * مثال:
 * ```
 * هذا.الاسم = "جديد"
 * ```
 */
Data::Value executeThisExpression(Interpreter* interpreter, AST::ThisExpr* node);

/**
 * @brief (AR) تنفيذ تعبير 'الأساس'
 * @brief (EN) Execute 'super' expression
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param node (AST::SuperExpr*) — (AR) عقدة 'الأساس' / (EN) 'super' node
 * @return (Data::Value) — (AR) قيمة من الصنف الأساسي / (EN) value from base class
 * 
 * ملاحظات إضافية:
 * - AR: يستدعي طريقة الصنف الأساسي
 * - EN: Calls base class method
 * - AR: خطأ إذا لم يكن هناك صنف أساسي
 * - EN: Error if no base class exists
 * 
 * مثال:
 * ```
 * الأساس.اطبع_معلومات()
 * ```
 */
Data::Value executeSuperExpression(Interpreter* interpreter, AST::SuperExpr* node);

// ======================================================================
// دوال تنفيذ الأعضاء الثابتة / Static Member Execution
// ======================================================================

/**
 * @brief (AR) تنفيذ الوصول لخاصية ثابتة
 * @brief (EN) Execute static field access
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param className (std::string) — (AR) اسم الصنف / (EN) class name
 * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
 * @return (Data::Value) — (AR) قيمة الخاصية / (EN) field value
 * 
 * مثال:
 * ```
 * رقم عدد = شخص::العدد_الكلي
 * ```
 */
Data::Value executeStaticFieldAccess(Interpreter* interpreter, 
                                     const std::string& className, const std::string& fieldName);

/**
 * @brief (AR) تنفيذ تعيين قيمة خاصية ثابتة
 * @brief (EN) Execute static field assignment
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param className (std::string) — (AR) اسم الصنف / (EN) class name
 * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
 * @param value (Data::Value) — (AR) القيمة الجديدة / (EN) new value
 * 
 * مثال:
 * ```
 * شخص::العدد_الكلي = 100
 * ```
 */
void executeStaticFieldAssignment(Interpreter* interpreter,
                                  const std::string& className, const std::string& fieldName,
                                  const Data::Value& value);

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

/**
 * @brief (AR) فحص رؤية عضو
 * @brief (EN) Check member visibility
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param classType (Data::ClassType*) — (AR) الصنف / (EN) class
 * @param memberName (std::string) — (AR) اسم العضو / (EN) member name
 * @param isField (bool) — (AR) هل خاصية؟ / (EN) is field?
 * @return (bool) — (AR) true إذا كان الوصول مسموح / (EN) true if access allowed
 */
bool checkMemberVisibility(Interpreter* interpreter, Data::ClassType* classType,
                           const std::string& memberName, bool isField);

/**
 * @brief (AR) الحصول على الكائن الحالي (this)
 * @brief (EN) Get current object (this)
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @return (Data::ObjectInstance*) — (AR) مؤشر للكائن الحالي / (EN) pointer to current object
 */
Data::ObjectInstance* getCurrentObject(Interpreter* interpreter);

/**
 * @brief (AR) تعيين الكائن الحالي (this)
 * @brief (EN) Set current object (this)
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param object (Data::ObjectInstance*) — (AR) الكائن / (EN) object
 */
void setCurrentObject(Interpreter* interpreter, Data::ObjectInstance* object);

/**
 * @brief (AR) فحص توافق المعاملات
 * @brief (EN) Check parameter compatibility
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param parameters (std::vector<AST::Parameter>) — (AR) المعاملات المتوقعة / (EN) expected parameters
 * @param arguments (std::vector<Data::Value>) — (AR) المعاملات الممررة / (EN) passed arguments
 * @return (bool) — (AR) true إذا متوافقة / (EN) true if compatible
 */
bool checkParameterCompatibility(Interpreter* interpreter,
                                 const std::vector<AST::Parameter>& parameters,
                                 const std::vector<Data::Value>& arguments);

/**
 * @brief (AR) تنفيذ قائمة تهيئة الباني
 * @brief (EN) Execute constructor initializer list
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param object (Data::ObjectInstance*) — (AR) الكائن / (EN) object
 * @param initializers (std::vector<AST::FieldInitializer>) — (AR) قائمة التهيئة / (EN) initializer list
 */
void executeInitializerList(Interpreter* interpreter, Data::ObjectInstance* object,
                            const std::vector<AST::FieldInitializer>& initializers);

/**
 * @brief (AR) البحث عن طريقة في سلسلة الوراثة
 * @brief (EN) Find method in inheritance chain
 * 
 * @param interpreter (Interpreter*) — (AR) المفسر / (EN) interpreter
 * @param classType (Data::ClassType*) — (AR) الصنف / (EN) class
 * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
 * @return (Data::ClassMethod*) — (AR) مؤشر للطريقة أو nullptr / (EN) pointer to method or nullptr
 */
Data::ClassMethod* findMethodInChain(Interpreter* interpreter, Data::ClassType* classType,
                                     const std::string& methodName);

} // namespace Interpreter
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
