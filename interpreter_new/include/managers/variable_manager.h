/**
 * @file variable_manager.h
 * @brief (AR) مدير المتغيرات - تخزين واسترجاع قيم المتغيرات
 * @brief (EN) Variable Manager - Storage and retrieval of variable values
 * 
 * الوصف بالعربية:
 * =================
 * مدير المتغيرات مسؤول عن:
 * - تخزين قيم المتغيرات في النطاقات المختلفة
 * - إدارة دورة حياة المتغيرات (تعريف، تعيين، حذف)
 * - دعم تظليل المتغيرات (Variable Shadowing)
 * - البحث عن المتغيرات في سلسلة النطاقات
 * - التكامل مع مدير النطاقات (ScopeManager)
 * 
 * المميزات:
 * - دعم كامل لجميع أنواع Value (INTEGER, DOUBLE, STRING, BOOLEAN, VOID)
 * - تظليل المتغيرات: نطاق داخلي يمكنه تظليل متغير خارجي
 * - رسائل خطأ ثنائية اللغة (عربي + إنجليزي)
 * - إدارة تلقائية للذاكرة باستخدام Smart Pointers
 * - دعم العمليات: تعريف، تعيين، قراءة، فحص الوجود
 * 
 * مثال الاستخدام:
 * ```cpp
 * VariableManager varMgr;
 * 
 * // تعريف متغير في النطاق العام
 * varMgr.define("x", Value(42));
 * 
 * // قراءة قيمة المتغير
 * Value val = varMgr.get("x");
 * 
 * // الدخول في نطاق دالة
 * varMgr.enterScope(ScopeType::FUNCTION, "myFunc");
 * varMgr.define("y", Value(3.14));
 * 
 * // الخروج من النطاق
 * varMgr.exitScope();
 * ```
 * 
 * English Description:
 * ====================
 * Variable Manager is responsible for:
 * - Storing variable values in different scopes
 * - Managing variable lifecycle (define, assign, delete)
 * - Supporting variable shadowing
 * - Searching for variables in scope chain
 * - Integration with ScopeManager
 * 
 * Features:
 * - Full support for all Value types (INTEGER, DOUBLE, STRING, BOOLEAN, VOID)
 * - Variable shadowing: inner scope can shadow outer variable
 * - Bilingual error messages (Arabic + English)
 * - Automatic memory management with Smart Pointers
 * - Operations: define, assign, get, exists
 * 
 * Usage Example:
 * ```cpp
 * VariableManager varMgr;
 * 
 * // Define variable in global scope
 * varMgr.define("x", Value(42));
 * 
 * // Read variable value
 * Value val = varMgr.get("x");
 * 
 * // Enter function scope
 * varMgr.enterScope(ScopeType::FUNCTION, "myFunc");
 * varMgr.define("y", Value(3.14));
 * 
 * // Exit scope
 * varMgr.exitScope();
 * ```
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>
#include "value.h"
#include "scope_manager.h"

namespace Sad {
namespace Data {

/**
 * @brief (AR) مدير المتغيرات - يدمج تخزين القيم مع إدارة النطاقات
 * @brief (EN) Variable Manager - Integrates value storage with scope management
 * 
 * المسؤوليات / Responsibilities:
 * --------------------------------
 * (AR) 1. تخزين قيم المتغيرات المرتبطة بالنطاقات
 * (EN) 1. Store variable values associated with scopes
 * 
 * (AR) 2. دعم تعريف المتغيرات (define) - إنشاء متغير جديد
 * (EN) 2. Support variable definition (define) - create new variable
 * 
 * (AR) 3. دعم تعيين القيم (assign) - تحديث قيمة متغير موجود
 * (EN) 3. Support value assignment (assign) - update existing variable
 * 
 * (AR) 4. دعم قراءة القيم (get) - الحصول على قيمة متغير
 * (EN) 4. Support value reading (get) - retrieve variable value
 * 
 * (AR) 5. دعم فحص الوجود (exists) - التحقق من وجود متغير
 * (EN) 5. Support existence check (exists) - verify variable presence
 * 
 * (AR) 6. إدارة دورة حياة المتغيرات مع النطاقات
 * (EN) 6. Manage variable lifecycle with scopes
 * 
 * (AR) 7. دعم تظليل المتغيرات بين النطاقات المختلفة
 * (EN) 7. Support variable shadowing across different scopes
 */
class VariableManager {
public:
    /**
     * @brief (AR) إنشاء مدير المتغيرات مع مرجع لمدير النطاقات
     * @brief (EN) Create variable manager with reference to scope manager
     * 
     * @param scopeManager (AR) مرجع لمدير النطاقات / (EN) Reference to scope manager
     */
    explicit VariableManager(ScopeManager& scopeManager);
    
    /**
     * @brief (AR) المُدمر - تنظيف جميع المتغيرات
     * @brief (EN) Destructor - cleanup all variables
     */
    ~VariableManager();
    
    // ========================================
    // (AR) عمليات المتغيرات الأساسية
    // (EN) Basic Variable Operations
    // ========================================
    
    /**
     * @brief (AR) تعريف متغير جديد في النطاق الحالي
     * @brief (EN) Define new variable in current scope
     * 
     * (AR) تعريف متغير جديد بقيمة ابتدائية. إذا كان المتغير معرفاً مسبقاً
     *      في النفس النطاق، يُرمى خطأ. لكن يُسمح بتظليل متغير من نطاق خارجي.
     * 
     * (EN) Define new variable with initial value. If variable already defined
     *      in same scope, throws error. But allows shadowing from outer scope.
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @param value (AR) القيمة الابتدائية / (EN) Initial value
     * 
     * @throws std::runtime_error (AR) إذا كان المتغير معرفاً في النطاق الحالي
     * @throws std::runtime_error (EN) if variable already defined in current scope
     * 
     * مثال / Example:
     * ```cpp
     * varMgr.define("x", Value(42));           // عدد صحيح / Integer
     * varMgr.define("pi", Value(3.14));        // عدد عشري / Double
     * varMgr.define("name", Value("أحمد"));    // نص / String
     * varMgr.define("flag", Value(true));      // منطقي / Boolean
     * ```
     */
    void define(const std::string& name, const Value& value);
    
    /**
     * @brief (AR) تعريف ثابت جديد (لا يمكن تعديل قيمته لاحقاً)
     * @brief (EN) Define a new constant (its value cannot be modified later)
     */
    void defineConst(const std::string& name, const Value& value);
    
    /**
     * @brief (AR) التحقق مما إذا كان المتغير ثابتاً
     * @brief (EN) Check if a variable is a constant
     */
    bool isConst(const std::string& name) const;
    
    /**
     * @brief (AR) تعيين قيمة جديدة لمتغير موجود
     * @brief (EN) Assign new value to existing variable
     * 
     * (AR) تحديث قيمة متغير معرف مسبقاً. يبحث في سلسلة النطاقات من الداخل
     *      للخارج ويحدث أول متغير يجده. إذا لم يجد المتغير، يُرمى خطأ.
     * 
     * (EN) Update value of previously defined variable. Searches scope chain
     *      from inner to outer and updates first found. If not found, throws error.
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @param value (AR) القيمة الجديدة / (EN) New value
     * 
     * @throws std::runtime_error (AR) إذا لم يكن المتغير معرفاً
     * @throws std::runtime_error (EN) if variable not defined
     * 
     * مثال / Example:
     * ```cpp
     * varMgr.define("x", Value(10));
     * varMgr.assign("x", Value(20));  // تحديث القيمة / Update value
     * ```
     */
    void assign(const std::string& name, const Value& value);
    
    /**
     * @brief (AR) تعريف أو تعيين — بحث واحد في سلسلة النطاقات
     * @brief (EN) Define or assign — single scope chain traversal
     * 
     * (AR) إذا كان المتغير موجوداً في أي نطاق، يُحدّث قيمته.
     *      وإلا يُعرّفه في النطاق الحالي. يُلغي الحاجة لـ exists()+assign()/define()
     *      مما يُقلل بحث سلسلة النطاقات من مرتين إلى مرة واحدة.
     * 
     * (EN) If variable exists in any scope, updates its value.
     *      Otherwise defines it in current scope. Eliminates the need for
     *      exists()+assign()/define(), reducing scope chain traversal from 2x to 1x.
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @param value (AR) القيمة / (EN) Value to set
     */
    void defineOrAssign(const std::string& name, const Value& value);
    
    /**
     * @brief (AR) الحصول على قيمة متغير
     * @brief (EN) Get variable value
     * 
     * (AR) قراءة قيمة متغير. يبحث في سلسلة النطاقات من الداخل للخارج
     *      ويرجع قيمة أول متغير يجده. إذا لم يجد المتغير، يُرمى خطأ.
     * 
     * (EN) Read variable value. Searches scope chain from inner to outer
     *      and returns first found value. If not found, throws error.
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) قيمة المتغير / (EN) Variable value
     * 
     * @throws std::runtime_error (AR) إذا لم يكن المتغير معرفاً
     * @throws std::runtime_error (EN) if variable not defined
     * 
     * مثال / Example:
     * ```cpp
     * varMgr.define("x", Value(42));
     * Value val = varMgr.get("x");
     * assert(val.toInt() == 42);
     * ```
     */
    const Value& get(const std::string& name) const;
    
    /**
     * @brief (AR) محاولة الحصول على مؤشر لقيمة المتغير — بحث واحد بدلاً من exists+get
     * @brief (EN) Try to get pointer to variable value — single lookup instead of exists+get
     * 
     * (AR) أسرع من استدعاء exists() ثم get() لأنه يمشي سلسلة النطاقات مرة واحدة فقط
     * (EN) Faster than calling exists() then get() because it walks scope chain only once
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) مؤشر للقيمة أو nullptr إذا لم يُعثر عليه / (EN) Pointer to value or nullptr if not found
     */
    const Value* tryGet(const std::string& name) const;
    
    /**
     * @brief (AR) التحقق من وجود متغير
     * @brief (EN) Check if variable exists
     * 
     * (AR) فحص إذا كان متغير معرف في أي نطاق (من الحالي إلى العام).
     *      لا يُرمى خطأ، بل يرجع true أو false.
     * 
     * (EN) Check if variable is defined in any scope (current to global).
     *      Does not throw error, returns true or false.
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) true إذا كان المتغير موجوداً / (EN) true if variable exists
     * 
     * مثال / Example:
     * ```cpp
     * if (varMgr.exists("x")) {
     *     Value val = varMgr.get("x");
     * }
     * ```
     */
    bool exists(const std::string& name) const;
    
    /**
     * @brief (AR) حذف متغير من النطاق الحالي
     * @brief (EN) Remove variable from current scope
     * 
     * (AR) حذف متغير من النطاق الحالي فقط. إذا كان هناك متغير بنفس الاسم
     *      في نطاق خارجي، سيبقى موجوداً.
     * 
     * (EN) Remove variable from current scope only. If variable with same name
     *      exists in outer scope, it remains.
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) true إذا تم الحذف / (EN) true if removed
     */
    bool remove(const std::string& name);
    
    // ========================================
    // (AR) إدارة النطاقات
    // (EN) Scope Management
    // ========================================
    
    /**
     * @brief (AR) الدخول في نطاق جديد
     * @brief (EN) Enter new scope
     * 
     * (AR) إنشاء نطاق جديد والدخول فيه. جميع المتغيرات المعرفة بعد ذلك
     *      ستكون في هذا النطاق الجديد.
     * 
     * (EN) Create and enter new scope. All variables defined after this
     *      will be in this new scope.
     * 
     * @param type (AR) نوع النطاق / (EN) Scope type
     * @param name (AR) اسم النطاق (اختياري) / (EN) Scope name (optional)
     * 
     * مثال / Example:
     * ```cpp
     * varMgr.enterScope(ScopeType::FUNCTION, "calculate");
     * varMgr.define("result", Value(0));
     * // ... function code ...
     * varMgr.exitScope();
     * ```
     */
    void enterScope(ScopeType type, const std::string& name = "");
    
    /**
     * @brief (AR) الخروج من النطاق الحالي
     * @brief (EN) Exit current scope
     * 
     * (AR) الخروج من النطاق الحالي وحذف جميع متغيراته. العودة إلى النطاق الأب.
     * (EN) Exit current scope and delete all its variables. Return to parent scope.
     * 
     * @throws std::runtime_error (AR) إذا حاولت الخروج من النطاق العام
     * @throws std::runtime_error (EN) if trying to exit global scope
     */
    void exitScope();
    
    /**
     * @brief (AR) الحصول على مدير النطاقات
     * @brief (EN) Get scope manager
     * 
     * @return (AR) مرجع لمدير النطاقات / (EN) Reference to scope manager
     */
    ScopeManager& getScopeManager() { return scopeManager_; }
    const ScopeManager& getScopeManager() const { return scopeManager_; }
    
    // ========================================
    // (AR) استعلامات ومعلومات
    // (EN) Queries and Information
    // ========================================
    
    /**
     * @brief (AR) الحصول على عدد المتغيرات في النطاق الحالي
     * @brief (EN) Get number of variables in current scope
     * 
     * @return (AR) عدد المتغيرات / (EN) Number of variables
     */
    size_t getVariableCount() const;
    
    /**
     * @brief (AR) الحصول على عدد المتغيرات في جميع النطاقات
     * @brief (EN) Get total number of variables in all scopes
     * 
     * @return (AR) العدد الكلي للمتغيرات / (EN) Total variable count
     */
    size_t getTotalVariableCount() const;
    
    /**
     * @brief (AR) الحصول على قائمة بأسماء المتغيرات في النطاق الحالي
     * @brief (EN) Get list of variable names in current scope
     * 
     * @return (AR) قائمة الأسماء / (EN) List of names
     */
    std::vector<std::string> getVariableNames() const;
    
    /**
     * @brief (AR) الحصول على معلومات عن متغير
     * @brief (EN) Get information about variable
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) نص يحتوي على معلومات المتغير / (EN) String with variable info
     */
    std::string getVariableInfo(const std::string& name) const;
    
    /**
     * @brief (AR) مسح جميع المتغيرات والعودة للنطاق العام
     * @brief (EN) Clear all variables and return to global scope
     */
    void clear();
    
    /**
     * @brief (AR) طباعة جميع المتغيرات (للتصحيح)
     * @brief (EN) Print all variables (for debugging)
     */
    void printAllVariables() const;
    
    /**
     * @brief (AR) الحصول على نص تصحيح
     * @brief (EN) Get debug string
     * 
     * @return (AR) معلومات عن حالة المدير / (EN) Manager state information
     */
    std::string debugString() const;
    
    /**
     * @brief (AR) تنظيف المتغيرات المرتبطة بنطاق معين
     * @brief (EN) Cleanup variables associated with a specific scope
     * 
     * يُستدعى عند إغلاق نطاق لحذف جميع المتغيرات المرتبطة به
     * Called when closing a scope to delete all associated variables
     * 
     * @param scope (AR) مؤشر للنطاق المراد تنظيفه / (EN) Pointer to scope to cleanup
     */
    void cleanupScope(Scope* scope);
    
private:
    // (AR) مرجع لمدير النطاقات (يُدار من الخارج)
    // (EN) Reference to scope manager (managed externally)
    ScopeManager& scopeManager_;
    
    // (AR) خريطة لتخزين القيم: المفتاح = (نطاق، اسم المتغير)، القيمة = قيمة المتغير
    // (EN) Map for storing values: key = (scope, variable name), value = variable value
    // نستخدم Scope* كمفتاح لربط القيم بالنطاقات
    // We use Scope* as key to associate values with scopes
    std::unordered_map<Scope*, std::unordered_map<std::string, Value>> scopeVariables_;
    
    // (AR) خريطة الثوابت — مرتبطة بالنطاق: كل نطاق يعرف ثوابته
    // (EN) Scope-aware const tracking — each scope knows its own constants
    std::unordered_map<Scope*, std::unordered_set<std::string>> constVariables_;
    
    /**
     * @brief (AR) البحث عن النطاق الذي يحتوي على متغير معين
     * @brief (EN) Find scope containing a specific variable
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) مؤشر للنطاق أو nullptr / (EN) Pointer to scope or nullptr
     */
    Scope* findVariableScope(const std::string& name) const;
    
    /**
     * @brief (AR) رمي خطأ ثنائي اللغة
     * @brief (EN) Throw bilingual error
     * 
     * @param messageAr (AR) الرسالة بالعربية / (EN) Arabic message
     * @param messageEn (AR) الرسالة بالإنجليزية / (EN) English message
     */
    void throwError(const std::string& messageAr, const std::string& messageEn) const;
};

} // namespace Data
} // namespace Sad
