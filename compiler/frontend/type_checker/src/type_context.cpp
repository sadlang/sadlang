/*
 * ============================================================================
 * Type Context - Implementation / تنفيذ سياق الأنواع
 * Type Context - Implementation
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "../include/type_context.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Symbol Implementation / تنفيذ Symbol
// ============================================================================

/**
 * تحويل الرمز لنص / Convert symbol to string
 * 
 * @return تمثيل نصي للرمز / String representation of symbol
 */
std::string Symbol::toString() const {
    std::stringstream ss;
    
    // إضافة نوع الرمز / Add symbol kind
    switch (kind_) {
        case Kind::VARIABLE:
            ss << "متغير: ";
            break;
        case Kind::CONSTANT:
            ss << "ثابت: ";
            break;
        case Kind::FUNCTION:
            ss << "دالة: ";
            break;
        case Kind::CLASS:
            ss << "صنف: ";
            break;
        case Kind::PARAMETER:
            ss << "معامل: ";
            break;
        case Kind::MODULE:
            ss << "وحدة: ";
            break;
    }
    
    // إضافة الاسم والنوع / Add name and type
    ss << name_ << " : " << type_->toString();
    
    // إضافة معلومات إضافية / Add additional info
    if (!isMutable_) {
        ss << " (غير قابل للتعديل)";
    }
    
    if (!isInitialized_) {
        ss << " (غير مهيأ)";
    }
    
    ss << " [نطاق " << scopeLevel_ << "]";
    
    return ss.str();
}

// ============================================================================
// Scope Implementation / تنفيذ Scope
// ============================================================================

/**
 * إضافة رمز للنطاق / Add symbol to scope
 * 
 * @param symbol الرمز المراد إضافته / Symbol to add
 * @return true إذا نجحت الإضافة / true if successful
 */
bool Scope::addSymbol(std::shared_ptr<Symbol> symbol) {
    // التحقق من أن الرمز غير موجود بالفعل / Check if symbol doesn't already exist
    if (contains(symbol->getName())) {
        return false; // الرمز موجود بالفعل / Symbol already exists
    }
    
    // إضافة الرمز / Add symbol
    symbols_[symbol->getName()] = symbol;
    return true;
}

/**
 * البحث عن رمز في النطاق الحالي فقط / Look up symbol in current scope only
 * 
 * @param name اسم الرمز / Symbol name
 * @return الرمز أو nullptr / Symbol or nullptr
 */
std::shared_ptr<Symbol> Scope::lookupLocal(const std::string& name) const {
    // البحث في جدول الرموز / Search in symbol table
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second;
    }
    
    return nullptr; // لم يتم العثور على الرمز / Symbol not found
}

/**
 * البحث عن رمز في النطاق الحالي والنطاقات الأبوية / Look up symbol in current and parent scopes
 * 
 * @param name اسم الرمز / Symbol name
 * @return الرمز أو nullptr / Symbol or nullptr
 */
std::shared_ptr<Symbol> Scope::lookup(const std::string& name) const {
    // البحث في النطاق الحالي أولاً / Search in current scope first
    auto symbol = lookupLocal(name);
    if (symbol) {
        return symbol;
    }
    
    // إذا لم يُعثر عليه، ابحث في النطاق الأب / If not found, search in parent scope
    if (parent_) {
        return parent_->lookup(name);
    }
    
    return nullptr; // لم يتم العثور على الرمز في أي نطاق / Symbol not found in any scope
}

/**
 * هل النطاق يحتوي على رمز (محلياً) / Contains symbol (locally)
 * 
 * @param name اسم الرمز / Symbol name
 * @return true إذا كان موجوداً / true if exists
 */
bool Scope::contains(const std::string& name) const {
    return symbols_.find(name) != symbols_.end();
}

/**
 * حذف رمز / Remove symbol
 * 
 * @param name اسم الرمز / Symbol name
 * @return true إذا تم الحذف / true if removed
 */
bool Scope::removeSymbol(const std::string& name) {
    // محاولة حذف الرمز / Try to remove symbol
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        symbols_.erase(it);
        return true;
    }
    
    return false; // الرمز غير موجود / Symbol not found
}

// ============================================================================
// TypeEnvironment Implementation / تنفيذ TypeEnvironment
// ============================================================================

/**
 * دفع نطاق جديد / Push new scope
 * 
 * @param type نوع النطاق / Scope type
 * @param name اسم النطاق / Scope name
 * @return النطاق الجديد / New scope
 */
std::shared_ptr<Scope> TypeEnvironment::pushScope(Scope::Type type, const std::string& name) {
    // زيادة المستوى / Increment level
    if (type != Scope::Type::GLOBAL) {
        currentLevel_++;
    }
    
    // الحصول على النطاق الأب / Get parent scope
    Scope* parent = nullptr;
    if (!scopeStack_.empty()) {
        parent = scopeStack_.back().get();
    }
    
    // إنشاء النطاق الجديد / Create new scope
    auto newScope = std::make_shared<Scope>(type, currentLevel_, parent, name);
    
    // إضافته للمكدس / Add to stack
    scopeStack_.push_back(newScope);
    
    return newScope;
}

/**
 * سحب النطاق الحالي / Pop current scope
 * 
 * @return النطاق المسحوب / Popped scope
 */
std::shared_ptr<Scope> TypeEnvironment::popScope() {
    // التحقق من وجود نطاقات / Check if scopes exist
    if (scopeStack_.empty()) {
        return nullptr; // لا يوجد نطاقات / No scopes
    }
    
    // لا يمكن حذف النطاق العام / Cannot pop global scope
    if (scopeStack_.size() == 1) {
        return nullptr; // النطاق العام لا يُحذف / Global scope cannot be popped
    }
    
    // سحب النطاق / Pop scope
    auto poppedScope = scopeStack_.back();
    scopeStack_.pop_back();
    
    // تقليل المستوى / Decrement level
    if (poppedScope->getType() != Scope::Type::GLOBAL) {
        currentLevel_--;
    }
    
    return poppedScope;
}

/**
 * إضافة رمز للنطاق الحالي / Add symbol to current scope
 * 
 * @param name اسم الرمز / Symbol name
 * @param type نوع الرمز / Symbol type
 * @param kind نوع الرمز / Symbol kind
 * @return الرمز المُضاف / Added symbol
 */
std::shared_ptr<Symbol> TypeEnvironment::addSymbol(
    const std::string& name,
    std::shared_ptr<Type> type,
    Symbol::Kind kind
) {
    // الحصول على النطاق الحالي / Get current scope
    auto currentScope = getCurrentScope();
    if (!currentScope) {
        return nullptr; // لا يوجد نطاق حالي / No current scope
    }
    
    // إنشاء الرمز / Create symbol
    auto symbol = std::make_shared<Symbol>(name, type, kind, currentLevel_);
    
    // إضافته للنطاق / Add to scope
    if (currentScope->addSymbol(symbol)) {
        return symbol; // نجحت الإضافة / Addition successful
    }
    
    return nullptr; // فشلت الإضافة (الرمز موجود بالفعل) / Addition failed (symbol exists)
}

/**
 * البحث عن رمز / Look up symbol
 * 
 * @param name اسم الرمز / Symbol name
 * @return الرمز أو nullptr / Symbol or nullptr
 */
std::shared_ptr<Symbol> TypeEnvironment::lookupSymbol(const std::string& name) const {
    // البحث من النطاق الحالي / Search from current scope
    auto currentScope = getCurrentScope();
    if (currentScope) {
        return currentScope->lookup(name);
    }
    
    return nullptr; // لا يوجد نطاق حالي / No current scope
}

/**
 * البحث عن رمز في النطاق الحالي فقط / Look up symbol in current scope only
 * 
 * @param name اسم الرمز / Symbol name
 * @return الرمز أو nullptr / Symbol or nullptr
 */
std::shared_ptr<Symbol> TypeEnvironment::lookupSymbolLocal(const std::string& name) const {
    // البحث في النطاق الحالي فقط / Search in current scope only
    auto currentScope = getCurrentScope();
    if (currentScope) {
        return currentScope->lookupLocal(name);
    }
    
    return nullptr; // لا يوجد نطاق حالي / No current scope
}

/**
 * حذف رمز من النطاق الحالي / Remove symbol from current scope
 * 
 * @param name اسم الرمز / Symbol name
 * @return true إذا تم الحذف / true if removed
 */
bool TypeEnvironment::removeSymbol(const std::string& name) {
    // الحصول على النطاق الحالي / Get current scope
    auto currentScope = getCurrentScope();
    if (currentScope) {
        return currentScope->removeSymbol(name);
    }
    
    return false; // لا يوجد نطاق حالي / No current scope
}

/**
 * مسح جميع الرموز من النطاق الحالي / Clear all symbols from current scope
 */
void TypeEnvironment::clearCurrentScope() {
    // الحصول على النطاق الحالي / Get current scope
    auto currentScope = getCurrentScope();
    if (currentScope) {
        currentScope->clear();
    }
}

/**
 * إعادة تعيين البيئة / Reset environment
 */
void TypeEnvironment::reset() {
    // مسح جميع النطاقات / Clear all scopes
    scopeStack_.clear();
    currentLevel_ = 0;
    
    // إعادة إنشاء النطاق العام / Recreate global scope
    pushScope(Scope::Type::GLOBAL, "global");
}

// ============================================================================
// TypeContext Implementation / تنفيذ TypeContext
// ============================================================================

/**
 * طباعة الأخطاء / Print errors
 */
void TypeContext::printErrors() const {
    if (errors_.empty()) {
        std::cout << "لا توجد أخطاء في الأنواع.\n";
        std::cout << "No type errors.\n";
        return;
    }
    
    std::cout << "=================================================\n";
    std::cout << "أخطاء الأنواع / Type Errors (" << errors_.size() << ")\n";
    std::cout << "=================================================\n";
    
    // طباعة كل خطأ / Print each error
    for (size_t i = 0; i < errors_.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << errors_[i] << "\n";
    }
    
    std::cout << "=================================================\n";
}

/**
 * طباعة التحذيرات / Print warnings
 */
void TypeContext::printWarnings() const {
    if (warnings_.empty()) {
        return; // لا توجد تحذيرات / No warnings
    }
    
    std::cout << "=================================================\n";
    std::cout << "تحذيرات الأنواع / Type Warnings (" << warnings_.size() << ")\n";
    std::cout << "=================================================\n";
    
    // طباعة كل تحذير / Print each warning
    for (size_t i = 0; i < warnings_.size(); ++i) {
        std::cout << "[" << (i + 1) << "] " << warnings_[i] << "\n";
    }
    
    std::cout << "=================================================\n";
}

/**
 * إعادة تعيين السياق / Reset context
 */
void TypeContext::reset() {
    // إعادة تعيين البيئة / Reset environment
    environment_->reset();
    
    // مسح الأخطاء والتحذيرات / Clear errors and warnings
    clearErrors();
    
    // إعادة تعيين السياق الحالي / Reset current context
    currentFunction_ = nullptr;
    currentClass_ = nullptr;
    expectedReturnType_ = nullptr;
}

} // namespace TypeChecker
} // namespace Sad
