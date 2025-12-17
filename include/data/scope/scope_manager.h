/**
 * @file scope_manager.h
 * @brief (AR) مدير النطاقات - إدارة نطاقات المتغيرات والدوال
 * @brief (EN) Scope Manager - Manages variable and function scopes
 * 
 * Handles nested scopes for:
 * - Global scope
 * - Function scopes
 * - Block scopes (if, while, for)
 * - Loop scopes
 * - Lambda scopes
 * 
 * Supports:
 * - Scope stack management
 * - Variable shadowing
 * - Scope depth tracking
 * - Scope type identification
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <functional>
#include "../types/value.h"

namespace Sad {
namespace Data {

/**
 * @brief (AR) أنواع النطاقات المدعومة
 * @brief (EN) Supported scope types
 */
enum class ScopeType {
    GLOBAL,     ///< (AR) النطاق العام / (EN) Global scope
    FUNCTION,   ///< (AR) نطاق دالة / (EN) Function scope
    BLOCK,      ///< (AR) نطاق كتلة / (EN) Block scope (if, while, etc.)
    LOOP,       ///< (AR) نطاق حلقة / (EN) Loop scope (for, while)
    LAMBDA,     ///< (AR) نطاق لامدا / (EN) Lambda scope
    CLASS       ///< (AR) نطاق صنف / (EN) Class scope
};

/**
 * @brief (AR) فئة تمثل نطاق واحد
 * @brief (EN) Represents a single scope
 * 
 * Each scope contains:
 * - Variables defined in this scope
 * - Scope type (global, function, block, etc.)
 * - Parent scope reference
 */
class Scope {
public:
    /**
     * @brief (AR) إنشاء نطاق جديد
     * @brief (EN) Create a new scope
     * 
     * @param type (AR) نوع النطاق / (EN) Scope type
     * @param name (AR) اسم النطاق (اختياري) / (EN) Scope name (optional)
     * @param parent (AR) النطاق الأب / (EN) Parent scope
     */
    Scope(ScopeType type, const std::string& name = "", Scope* parent = nullptr);
    
    // Type information
    ScopeType getType() const { return type_; }
    std::string getName() const { return name_; }
    Scope* getParent() const { return parent_; }
    
    // Variable management (declarations only - values stored in VariableManager)
    bool hasVariable(const std::string& name) const;
    void declareVariable(const std::string& name);
    bool isVariableDeclared(const std::string& name) const;
    
    // Scope information
    int getDepth() const;
    bool isGlobal() const { return type_ == ScopeType::GLOBAL; }
    bool isFunction() const { return type_ == ScopeType::FUNCTION; }
    bool isLoop() const { return type_ == ScopeType::LOOP; }
    
    // Debug
    std::string debugString() const;
    std::string getTypeName() const;
    
private:
    ScopeType type_;
    std::string name_;
    Scope* parent_;
    std::unordered_map<std::string, bool> variables_;  // Variable declarations
};

/**
 * @brief (AR) مدير النطاقات - إدارة مكدس النطاقات
 * @brief (EN) Scope Manager - Manages the scope stack
 * 
 * Responsible for:
 * - Pushing and popping scopes
 * - Tracking current scope
 * - Finding variables in scope chain
 * - Managing scope lifecycle
 */
class ScopeManager {
public:
    /**
     * @brief (AR) إنشاء مدير النطاقات مع النطاق العام
     * @brief (EN) Create scope manager with global scope
     */
    ScopeManager();
    
    /**
     * @brief (AR) المُدمر - تنظيف جميع النطاقات
     * @brief (EN) Destructor - cleanup all scopes
     */
    ~ScopeManager();
    
    // Scope stack management
    
    /**
     * @brief (AR) دخول نطاق جديد (إضافة للمكدس)
     * @brief (EN) Enter a new scope (push to stack)
     * 
     * @param type (AR) نوع النطاق / (EN) Scope type
     * @param name (AR) اسم النطاق (اختياري) / (EN) Scope name (optional)
     * 
     * Example:
     * ```cpp
     * scopeMgr.pushScope(ScopeType::FUNCTION, "main");
     * // ... function code ...
     * scopeMgr.popScope();
     * ```
     */
    void pushScope(ScopeType type, const std::string& name = "");
    
    /**
     * @brief (AR) الخروج من النطاق الحالي (إزالة من المكدس)
     * @brief (EN) Exit current scope (pop from stack)
     * 
     * @throws std::runtime_error if trying to pop global scope
     */
    void popScope();
    
    /**
     * @brief (AR) الحصول على النطاق الحالي
     * @brief (EN) Get current scope
     * 
     * @return (AR) مؤشر للنطاق الحالي / (EN) Pointer to current scope
     */
    Scope* getCurrentScope() const { return currentScope_; }
    
    /**
     * @brief (AR) الحصول على النطاق العام
     * @brief (EN) Get global scope
     * 
     * @return (AR) مؤشر للنطاق العام / (EN) Pointer to global scope
     */
    Scope* getGlobalScope() const { return globalScope_; }
    
    // Scope queries
    
    /**
     * @brief (AR) الحصول على عمق النطاق الحالي
     * @brief (EN) Get current scope depth
     * 
     * @return (AR) العمق (0 = عام، 1+ = متداخل) / (EN) Depth (0 = global, 1+ = nested)
     */
    int getCurrentDepth() const;
    
    /**
     * @brief (AR) هل نحن في النطاق العام؟
     * @brief (EN) Are we in global scope?
     */
    bool isGlobalScope() const { return currentScope_ == globalScope_; }
    
    /**
     * @brief (AR) هل نحن داخل دالة؟
     * @brief (EN) Are we inside a function?
     */
    bool isInFunction() const;
    
    /**
     * @brief (AR) هل نحن داخل حلقة؟
     * @brief (EN) Are we inside a loop?
     */
    bool isInLoop() const;
    
    /**
     * @brief (AR) البحث عن أقرب نطاق من نوع معين
     * @brief (EN) Find nearest scope of given type
     * 
     * @param type (AR) نوع النطاق المطلوب / (EN) Desired scope type
     * @return (AR) مؤشر للنطاق أو nullptr / (EN) Pointer to scope or nullptr
     */
    Scope* findNearestScope(ScopeType type) const;
    
    // Variable lookup (declarations only)
    
    /**
     * @brief (AR) البحث عن متغير في سلسلة النطاقات
     * @brief (EN) Look up variable in scope chain
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) true إذا كان المتغير معرفاً / (EN) true if variable is declared
     */
    bool isVariableDeclared(const std::string& name) const;
    
    /**
     * @brief (AR) تعريف متغير في النطاق الحالي
     * @brief (EN) Declare variable in current scope
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @throws std::runtime_error if variable already declared in current scope
     */
    void declareVariable(const std::string& name);
    
    /**
     * @brief (AR) البحث عن النطاق الذي يحتوي على المتغير
     * @brief (EN) Find scope containing variable
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) مؤشر للنطاق أو nullptr / (EN) Pointer to scope or nullptr
     */
    Scope* findVariableScope(const std::string& name) const;
    
    // Debug and information
    
    /**
     * @brief (AR) الحصول على معلومات النطاق الحالي
     * @brief (EN) Get current scope information
     */
    std::string debugString() const;
    
    /**
     * @brief (AR) طباعة مكدس النطاقات
     * @brief (EN) Print scope stack
     */
    void printScopeStack() const;
    
    /**
     * @brief (AR) الحصول على عدد النطاقات في المكدس
     * @brief (EN) Get number of scopes in stack
     */
    size_t getScopeCount() const { return scopeStack_.size(); }
    
private:
    Scope* globalScope_;                    ///< (AR) النطاق العام / (EN) Global scope
    Scope* currentScope_;                   ///< (AR) النطاق الحالي / (EN) Current scope
    std::vector<std::unique_ptr<Scope>> scopeStack_;  ///< (AR) مكدس النطاقات / (EN) Scope stack
    
    /**
     * @brief (AR) رمي خطأ ثنائي اللغة
     * @brief (EN) Throw bilingual error
     */
    void throwError(const std::string& messageAr, const std::string& messageEn) const;
};

/**
 * @brief (AR) حارس النطاق - تنظيف تلقائي للنطاق عند الخروج
 * @brief (EN) Scope Guard - Automatic scope cleanup on exit
 * 
 * استخدام RAII لضمان تنظيف النطاق حتى في حالة حدوث استثناء
 * Uses RAII to ensure scope cleanup even when exceptions occur
 * 
 * @example
 * ```cpp
 * {
     *     ScopeGuard guard(scopeManager, ScopeType::FUNCTION, "myFunc");
 *     // ... code that might throw ...
 * } // popScope() called automatically here
 * ```
 */
class ScopeGuard {
public:
    /**
     * @brief (AR) إنشاء حارس نطاق وفتح نطاق جديد
     * @brief (EN) Create scope guard and push new scope
     */
    ScopeGuard(ScopeManager& manager, ScopeType type, const std::string& name = "")
        : manager_(manager), scopeToCleanup_(nullptr), active_(true) {
        manager_.pushScope(type, name);
        scopeToCleanup_ = manager_.getCurrentScope();
    }
    
    /**
     * @brief (AR) المدمر - إغلاق النطاق تلقائياً
     * @brief (EN) Destructor - pop scope automatically
     */
    ~ScopeGuard() {
        if (active_ && scopeToCleanup_) {
            if (cleanupCallback_) {
                cleanupCallback_(scopeToCleanup_);
            }
            manager_.popScope();
        }
    }
    
    /**
     * @brief (AR) تعيين دالة callback للتنظيف
     * @brief (EN) Set cleanup callback function
     */
    void setCleanupCallback(std::function<void(Scope*)> callback) {
        cleanupCallback_ = callback;
    }
    
    /**
     * @brief (AR) إلغاء تفعيل الحارس (عدم إغلاق النطاق)
     * @brief (EN) Deactivate guard (don't pop scope)
     */
    void dismiss() {
        active_ = false;
    }
    
    // منع النسخ / Prevent copying
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    // منع النقل / Prevent moving
    ScopeGuard(ScopeGuard&&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;
    
private:
    ScopeManager& manager_;
    Scope* scopeToCleanup_;
    bool active_;
    std::function<void(Scope*)> cleanupCallback_;
};

} // namespace Data
} // namespace Sad
