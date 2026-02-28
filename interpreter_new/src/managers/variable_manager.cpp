/**
 * @file variable_manager.cpp
 * @brief (AR) تنفيذ مدير المتغيرات
 * @brief (EN) Variable manager implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#include "variable_manager.h"
#include <sstream>
#include <iostream>

namespace Sad {
namespace Data {

// ========================================
// (AR) البناء والهدم
// (EN) Constructor and Destructor
// ========================================

VariableManager::VariableManager(ScopeManager& scopeManager) 
    : scopeManager_(scopeManager) {
    // (AR) نستخدم ScopeManager الموجود بالفعل - لا حاجة لإنشاء scope عام هنا
    // (EN) Use existing ScopeManager - no need to create global scope here
}

VariableManager::~VariableManager() {
    // (AR) التنظيف التلقائي - ScopeManager يُدار من الخارج
    // (EN) Automatic cleanup - ScopeManager managed externally
}

// ========================================
// (AR) عمليات المتغيرات الأساسية
// (EN) Basic Variable Operations
// ========================================

void VariableManager::define(const std::string& name, const Value& value) {
    // (AR) الحصول على النطاق الحالي
    // (EN) Get current scope
    Scope* currentScope = scopeManager_.getCurrentScope();
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) إصلاح المشكلة 2: منع shadowing داخل الحلقات والكتل
    //      إذا كان المتغير موجوداً في نطاق أعلى (parent scope)،
    //      نُحدّث قيمته بدلاً من إنشاء متغير جديد يُخفيه.
    //      هذا يُصلح: متغير عداد = عداد + 1 داخل بينما
    // (EN) Fix issue #2: Prevent shadowing inside loops and blocks
    //      If variable exists in parent scope, update it instead of
    //      creating a new one that shadows it.
    // ═══════════════════════════════════════════════════════════════
    
    // (AR) أولاً: تحقق من النطاق الحالي
    // (EN) First: check current scope
    if (currentScope->hasVariable(name)) {
        // (AR) المتغير موجود في النطاق الحالي — نحدّث قيمته فقط
        // (EN) Variable exists in current scope — just update its value
        scopeVariables_[currentScope][name] = value;
        return;
    }
    
    // (AR) ثانياً: تحقق من النطاقات الأعلى (parent scopes)
    //      إذا وجدنا المتغير، نحدّثه بدلاً من إنشاء جديد
    // (EN) Second: check parent scopes
    //      If we find the variable, update it instead of creating new
    Scope* parentScope = currentScope->getParent();
    while (parentScope != nullptr) {
        auto scopeIt = scopeVariables_.find(parentScope);
        if (scopeIt != scopeVariables_.end()) {
            auto varIt = scopeIt->second.find(name);
            if (varIt != scopeIt->second.end()) {
                // (AR) وجدنا المتغير في نطاق أعلى — نحدّثه!
                //      هذا يُصلح: متغير عداد = عداد + 1 داخل حلقة
                // (EN) Found variable in parent scope — update it!
                //      This fixes: var counter = counter + 1 inside loop
                varIt->second = value;
                return;
            }
        }
        parentScope = parentScope->getParent();
    }
    
    // (AR) المتغير جديد — أنشئه في النطاق الحالي
    // (EN) Variable is new — create it in current scope
    scopeManager_.declareVariable(name);
    scopeVariables_[currentScope][name] = value;
}

void VariableManager::defineConst(const std::string& name, const Value& value) {
    // (AR) تعريف الثابت كمتغير عادي ثم تسجيله في قائمة الثوابت
    // (EN) Define as normal variable then register in const set
    define(name, value);
    constVariables_.insert(name);
}

bool VariableManager::isConst(const std::string& name) const {
    return constVariables_.count(name) > 0;
}

void VariableManager::assign(const std::string& name, const Value& value) {
    // (AR) التحقق من أن المتغير ليس ثابتاً
    // (EN) Check that the variable is not a constant
    if (constVariables_.count(name) > 0) {
        throwError(
            "لا يمكن تعديل الثابت '" + name + "'",
            "Cannot reassign constant '" + name + "'"
        );
    }
    
    // (AR) البحث المباشر عن المتغير وتحديثه — بحث واحد بدلاً من اثنين
    // (EN) Direct variable lookup and update — single pass instead of two
    Scope* scope = scopeManager_.getCurrentScope();
    
    while (scope != nullptr) {
        auto scopeIt = scopeVariables_.find(scope);
        if (scopeIt != scopeVariables_.end()) {
            auto varIt = scopeIt->second.find(name);
            if (varIt != scopeIt->second.end()) {
                varIt->second = value;
                return;
            }
        }
        scope = scope->getParent();
    }
    
    // (AR) إذا لم يُعثر على المتغير، رمي خطأ
    // (EN) If variable not found, throw error
    throwError(
        "المتغير '" + name + "' غير معرّف",
        "Variable '" + name + "' not defined"
    );
}

Value VariableManager::get(const std::string& name) const {
    // (AR) البحث المباشر عن المتغير بدون استدعاء مزدوج — تحسين أداء
    // (EN) Direct variable lookup without double-call — performance optimization
    
    Scope* scope = scopeManager_.getCurrentScope();
    
    while (scope != nullptr) {
        auto scopeIt = scopeVariables_.find(scope);
        if (scopeIt != scopeVariables_.end()) {
            auto varIt = scopeIt->second.find(name);
            if (varIt != scopeIt->second.end()) {
                return varIt->second;
            }
        }
        scope = scope->getParent();
    }
    
    // (AR) إذا لم يُعثر على المتغير، رمي خطأ
    // (EN) If variable not found, throw error
    throwError(
        "المتغير '" + name + "' غير معرّف",
        "Variable '" + name + "' not defined"
    );
    
    // (AR) لن نصل هنا أبداً
    // (EN) Never reached
    return Value();
}

bool VariableManager::exists(const std::string& name) const {
    // (AR) البحث عن المتغير في سلسلة النطاقات
    // (EN) Search for variable in scope chain
    // 
    // (AR) نبحث في خريطة القيم بدلاً من ScopeManager للتأكد من وجود القيمة فعلياً
    // (EN) Search in value map instead of ScopeManager to ensure value actually exists
    return findVariableScope(name) != nullptr;
}

bool VariableManager::remove(const std::string& name) {
    // (AR) الحصول على النطاق الحالي
    // (EN) Get current scope
    Scope* currentScope = scopeManager_.getCurrentScope();
    
    // (AR) التحقق من وجود المتغير في النطاق الحالي
    // (EN) Check if variable exists in current scope
    if (!currentScope->hasVariable(name)) {
        return false;
    }
    
    // (AR) حذف القيمة من الخريطة
    // (EN) Remove value from map
    auto scopeIt = scopeVariables_.find(currentScope);
    if (scopeIt != scopeVariables_.end()) {
        scopeIt->second.erase(name);
    }
    
    // (AR) ملاحظة: لا نحذف من ScopeManager لأنه يُدير التصريحات فقط
    // (EN) Note: We don't remove from ScopeManager as it only manages declarations
    
    return true;
}

// ========================================
// (AR) إدارة النطاقات
// (EN) Scope Management
// ========================================

void VariableManager::enterScope(ScopeType type, const std::string& name) {
    // (AR) إنشاء نطاق جديد في مدير النطاقات
    // (EN) Create new scope in scope manager
    scopeManager_.pushScope(type, name);
    
    // (AR) سيتم إنشاء خريطة المتغيرات للنطاق الجديد عند أول تعريف
    // (EN) Variable map for new scope will be created on first define
}

void VariableManager::exitScope() {
    // (AR) التحقق من أننا لسنا في النطاق العام قبل حذف المتغيرات
    // (EN) Verify not at global scope before erasing variables
    Scope* currentScope = scopeManager_.getCurrentScope();
    
    // (AR) إزالة النطاق أولاً — إذا فشل، لا نحذف المتغيرات
    // (EN) Pop scope first — if it fails, don't erase variables
    scopeManager_.popScope();
    
    // (AR) حذف جميع متغيرات هذا النطاق بعد النجاح
    // (EN) Delete all variables in this scope after successful pop
    scopeVariables_.erase(currentScope);
}

// ========================================
// (AR) استعلامات ومعلومات
// (EN) Queries and Information
// ========================================

size_t VariableManager::getVariableCount() const {
    // (AR) عدد المتغيرات في النطاق الحالي
    // (EN) Number of variables in current scope
    Scope* currentScope = scopeManager_.getCurrentScope();
    auto it = scopeVariables_.find(currentScope);
    
    if (it != scopeVariables_.end()) {
        return it->second.size();
    }
    
    return 0;
}

size_t VariableManager::getTotalVariableCount() const {
    // (AR) العدد الكلي للمتغيرات في جميع النطاقات
    // (EN) Total number of variables in all scopes
    size_t total = 0;
    for (const auto& scopePair : scopeVariables_) {
        total += scopePair.second.size();
    }
    return total;
}

std::vector<std::string> VariableManager::getVariableNames() const {
    // (AR) قائمة أسماء المتغيرات في النطاق الحالي
    // (EN) List of variable names in current scope
    std::vector<std::string> names;
    
    Scope* currentScope = scopeManager_.getCurrentScope();
    auto it = scopeVariables_.find(currentScope);
    
    if (it != scopeVariables_.end()) {
        for (const auto& varPair : it->second) {
            names.push_back(varPair.first);
        }
    }
    
    return names;
}

std::string VariableManager::getVariableInfo(const std::string& name) const {
    // (AR) معلومات عن المتغير
    // (EN) Information about variable
    std::ostringstream oss;
    
    Scope* varScope = findVariableScope(name);
    
    if (varScope == nullptr) {
        oss << "(AR) المتغير '" << name << "' غير معرّف "
            << "(EN) Variable '" << name << "' not defined";
        return oss.str();
    }
    
    // (AR) الحصول على القيمة
    // (EN) Get value
    auto scopeIt = scopeVariables_.find(varScope);
    if (scopeIt != scopeVariables_.end()) {
        auto varIt = scopeIt->second.find(name);
        if (varIt != scopeIt->second.end()) {
            const Value& val = varIt->second;
            oss << "Variable '" << name << "': "
                << val.debugString()
                << " (in " << varScope->debugString() << ")";
            return oss.str();
        }
    }
    
    oss << "(AR) خطأ داخلي: متغير بدون قيمة "
        << "(EN) Internal error: variable without value";
    return oss.str();
}

void VariableManager::clear() {
    // (AR) حذف جميع المتغيرات
    // (EN) Delete all variables
    scopeVariables_.clear();
    
    // (AR) العودة إلى النطاق العام (حذف جميع النطاقات الأخرى)
    // (EN) Return to global scope (remove all other scopes)
    while (!scopeManager_.isGlobalScope()) {
        scopeManager_.popScope();
    }
}

void VariableManager::printAllVariables() const {
    // (AR) طباعة جميع المتغيرات
    // (EN) Print all variables
    std::cout << "\n=== (AR) جميع المتغيرات / (EN) All Variables ===\n";
    std::cout << "(AR) العدد الكلي / (EN) Total: " << getTotalVariableCount() << "\n\n";
    
    for (const auto& scopePair : scopeVariables_) {
        Scope* scope = scopePair.first;
        const auto& variables = scopePair.second;
        
        std::cout << "  " << scope->debugString() << ":\n";
        
        if (variables.empty()) {
            std::cout << "    (AR) فارغ / (EN) Empty\n";
        } else {
            for (const auto& varPair : variables) {
                std::cout << "    - " << varPair.first << " = " 
                         << varPair.second.debugString() << "\n";
            }
        }
        std::cout << "\n";
    }
    std::cout << "===================================\n\n";
}

std::string VariableManager::debugString() const {
    // (AR) نص تصحيح
    // (EN) Debug string
    std::ostringstream oss;
    oss << "VariableManager["
        << "total_vars=" << getTotalVariableCount()
        << ", current_scope_vars=" << getVariableCount()
        << ", " << scopeManager_.debugString()
        << "]";
    return oss.str();
}

// ========================================
// (AR) دوال مساعدة خاصة
// (EN) Private Helper Functions
// ========================================

Scope* VariableManager::findVariableScope(const std::string& name) const {
    // (AR) البحث عن النطاق الذي يحتوي على المتغير
    // (EN) Find scope containing the variable
    // 
    // (AR) نبحث من النطاق الحالي صعوداً إلى النطاق العام
    // (EN) Search from current scope up to global scope
    // 
    // (AR) نبحث في خريطة القيم لضمان وجود القيمة فعلياً
    // (EN) Search in value map to ensure value actually exists
    
    Scope* scope = scopeManager_.getCurrentScope();
    
    while (scope != nullptr) {
        // (AR) التحقق من وجود المتغير في هذا النطاق
        // (EN) Check if variable exists in this scope
        auto scopeIt = scopeVariables_.find(scope);
        if (scopeIt != scopeVariables_.end()) {
            auto varIt = scopeIt->second.find(name);
            if (varIt != scopeIt->second.end()) {
                return scope;
            }
        }
        
        // (AR) الانتقال للنطاق الأب
        // (EN) Move to parent scope
        scope = scope->getParent();
    }
    
    return nullptr;
}

void VariableManager::throwError(const std::string& messageAr, const std::string& messageEn) const {
    // (AR) رمي خطأ ثنائي اللغة
    // (EN) Throw bilingual error
    std::ostringstream oss;
    oss << "(AR) " << messageAr << " (EN) " << messageEn;
    throw std::runtime_error(oss.str());
}

void VariableManager::cleanupScope(Scope* scope) {
    // (AR) حذف جميع المتغيرات المرتبطة بهذا النطاق
    // (EN) Delete all variables associated with this scope
    auto it = scopeVariables_.find(scope);
    if (it != scopeVariables_.end()) {
        scopeVariables_.erase(it);
    }
}

} // namespace Data
} // namespace Sad
