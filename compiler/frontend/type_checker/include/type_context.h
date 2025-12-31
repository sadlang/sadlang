/*
 * ============================================================================
 * Type Context - سياق ال Type Checking
 * Type Context - Type Checking Context
 * ============================================================================
 * 
 * الوصف (Description):
 *   هذا الملف يحتوي على TypeContext و TypeEnvironment اللذان يديران
 *   بيئة Type checking مع النطاقات (scopes) وجداول الرموز (symbol tables).
 * 
 *   This file contains TypeContext and TypeEnvironment which manage
 *   the type checking environment with scopes and symbol tables.
 * 
 * المكونات الرئيسية (Main Components):
 *   - Symbol: رمز (متغير أو دالة) مع نوعه / Symbol (variable/function) with type
 *   - Scope: نطاق يحتوي على رموز / Scope containing symbols
 *   - TypeEnvironment: بيئة الأنواع مع stack من النطاقات / Type environment with scope stack
 *   - TypeContext: سياق شامل للـ Type checking / Complete type checking context
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_TYPE_CONTEXT_H
#define SAD_TYPE_CONTEXT_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include "typed_ast.h"

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

class TypeEnvironment;
class Scope;

// ============================================================================
// Symbol - رمز في جدول الرموز
// Symbol - Symbol in symbol table
// ============================================================================

/**
 * رمز (متغير أو دالة أو صنف) في جدول الرموز
 * Symbol (variable, function, or class) in symbol table
 * 
 * يحتوي على:
 * - الاسم / Name
 * - النوع / Type
 * - هل هو ثابت / Is constant
 * - هل تم تهيئته / Is initialized
 * - معلومات إضافية / Additional info
 */
class Symbol {
public:
    /**
     * نوع الرمز / Symbol kind
     */
    enum class Kind {
        VARIABLE,   // متغير / Variable
        CONSTANT,   // ثابت / Constant
        FUNCTION,   // دالة / Function
        CLASS,      // صنف / Class
        PARAMETER,  // معامل دالة / Function parameter
        MODULE      // وحدة / Module
    };

private:
    std::string name_;                  // اسم الرمز / Symbol name
    std::shared_ptr<Type> type_;        // نوع الرمز / Symbol type
    Kind kind_;                         // نوع الرمز / Symbol kind
    bool isInitialized_;                // هل تم تهيئته / Is initialized
    bool isMutable_;                    // هل قابل للتعديل / Is mutable
    int scopeLevel_;                    // مستوى النطاق / Scope level
    
    // معلومات إضافية / Additional info
    std::unordered_map<std::string, std::string> attributes_;
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param name اسم الرمز / Symbol name
     * @param type نوع الرمز / Symbol type
     * @param kind نوع الرمز / Symbol kind
     * @param scopeLevel مستوى النطاق / Scope level
     */
    Symbol(
        const std::string& name,
        std::shared_ptr<Type> type,
        Kind kind,
        int scopeLevel
    )
        : name_(name)
        , type_(type)
        , kind_(kind)
        , isInitialized_(false)
        , isMutable_(true)
        , scopeLevel_(scopeLevel)
    {
        // الثوابت والدوال لا يمكن تعديلها / Constants and functions are immutable
        if (kind == Kind::CONSTANT || kind == Kind::FUNCTION) {
            isMutable_ = false;
        }
    }
    
    // Getters
    std::string getName() const { return name_; }
    std::shared_ptr<Type> getType() const { return type_; }
    Kind getKind() const { return kind_; }
    bool isInitialized() const { return isInitialized_; }
    bool isMutable() const { return isMutable_; }
    int getScopeLevel() const { return scopeLevel_; }
    
    // Setters
    void setInitialized(bool init) { isInitialized_ = init; }
    void setMutable(bool mut) { isMutable_ = mut; }
    void setType(std::shared_ptr<Type> type) { type_ = type; }
    
    // Attributes / الخصائص
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return (it != attributes_.end()) ? it->second : "";
    }
    
    bool hasAttribute(const std::string& key) const {
        return attributes_.find(key) != attributes_.end();
    }
    
    // تحويل لنص / Convert to string
    std::string toString() const;
};

// ============================================================================
// Scope - نطاق
// Scope - Scope
// ============================================================================

/**
 * نطاق يحتوي على رموز
 * Scope containing symbols
 * 
 * الأنواع:
 * - Global: نطاق عام / Global scope
 * - Function: نطاق دالة / Function scope
 * - Block: نطاق كتلة (if, while, etc.) / Block scope
 * - Class: نطاق صنف / Class scope
 */
class Scope {
public:
    /**
     * نوع النطاق / Scope type
     */
    enum class Type {
        GLOBAL,     // عام / Global
        FUNCTION,   // دالة / Function
        BLOCK,      // كتلة / Block
        CLASS,      // صنف / Class
        MODULE      // وحدة / Module
    };

private:
    Type type_;                                     // نوع النطاق / Scope type
    int level_;                                     // مستوى النطاق / Scope level
    Scope* parent_;                                 // النطاق الأب / Parent scope
    std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols_; // الرموز / Symbols
    std::string name_;                              // اسم النطاق (اختياري) / Scope name (optional)
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param type نوع النطاق / Scope type
     * @param level مستوى النطاق / Scope level
     * @param parent النطاق الأب / Parent scope
     * @param name اسم النطاق / Scope name
     */
    Scope(Type type, int level, Scope* parent = nullptr, const std::string& name = "")
        : type_(type)
        , level_(level)
        , parent_(parent)
        , name_(name)
    {}
    
    // Getters
    Type getType() const { return type_; }
    int getLevel() const { return level_; }
    Scope* getParent() const { return parent_; }
    std::string getName() const { return name_; }
    
    /**
     * إضافة رمز للنطاق / Add symbol to scope
     * 
     * @param symbol الرمز المراد إضافته / Symbol to add
     * @return true إذا نجحت الإضافة / true if successful
     */
    bool addSymbol(std::shared_ptr<Symbol> symbol);
    
    /**
     * البحث عن رمز في النطاق الحالي فقط / Look up symbol in current scope only
     * 
     * @param name اسم الرمز / Symbol name
     * @return الرمز أو nullptr / Symbol or nullptr
     */
    std::shared_ptr<Symbol> lookupLocal(const std::string& name) const;
    
    /**
     * البحث عن رمز في النطاق الحالي والنطاقات الأبوية / Look up symbol in current and parent scopes
     * 
     * @param name اسم الرمز / Symbol name
     * @return الرمز أو nullptr / Symbol or nullptr
     */
    std::shared_ptr<Symbol> lookup(const std::string& name) const;
    
    /**
     * هل النطاق يحتوي على رمز (محلياً) / Contains symbol (locally)
     * 
     * @param name اسم الرمز / Symbol name
     * @return true إذا كان موجوداً / true if exists
     */
    bool contains(const std::string& name) const;
    
    /**
     * الحصول على جميع الرموز / Get all symbols
     * 
     * @return خريطة الرموز / Symbols map
     */
    const std::unordered_map<std::string, std::shared_ptr<Symbol>>& getSymbols() const {
        return symbols_;
    }
    
    /**
     * حذف رمز / Remove symbol
     * 
     * @param name اسم الرمز / Symbol name
     * @return true إذا تم الحذف / true if removed
     */
    bool removeSymbol(const std::string& name);
    
    /**
     * مسح جميع الرموز / Clear all symbols
     */
    void clear() {
        symbols_.clear();
    }
    
    /**
     * عدد الرموز / Number of symbols
     */
    size_t size() const {
        return symbols_.size();
    }
};

// ============================================================================
// TypeEnvironment - بيئة الأنواع
// TypeEnvironment - Type Environment
// ============================================================================

/**
 * بيئة الأنواع مع stack من النطاقات
 * Type environment with scope stack
 * 
 * يدير:
 * - Stack من النطاقات / Stack of scopes
 * - النطاق الحالي / Current scope
 * - البحث عن الرموز / Symbol lookup
 * - إضافة وحذف الرموز / Adding and removing symbols
 */
class TypeEnvironment {
private:
    std::vector<std::shared_ptr<Scope>> scopeStack_; // مكدس النطاقات / Scope stack
    int currentLevel_;                               // المستوى الحالي / Current level
    
public:
    /**
     * المنشئ / Constructor
     */
    TypeEnvironment()
        : currentLevel_(0)
    {
        // إنشاء النطاق العام / Create global scope
        pushScope(Scope::Type::GLOBAL, "global");
    }
    
    /**
     * المدمر / Destructor
     */
    ~TypeEnvironment() = default;
    
    /**
     * دفع نطاق جديد / Push new scope
     * 
     * @param type نوع النطاق / Scope type
     * @param name اسم النطاق / Scope name
     * @return النطاق الجديد / New scope
     */
    std::shared_ptr<Scope> pushScope(Scope::Type type, const std::string& name = "");
    
    /**
     * سحب النطاق الحالي / Pop current scope
     * 
     * @return النطاق المسحوب / Popped scope
     */
    std::shared_ptr<Scope> popScope();
    
    /**
     * الحصول على النطاق الحالي / Get current scope
     * 
     * @return النطاق الحالي / Current scope
     */
    std::shared_ptr<Scope> getCurrentScope() const {
        return scopeStack_.empty() ? nullptr : scopeStack_.back();
    }
    
    /**
     * الحصول على النطاق العام / Get global scope
     * 
     * @return النطاق العام / Global scope
     */
    std::shared_ptr<Scope> getGlobalScope() const {
        return scopeStack_.empty() ? nullptr : scopeStack_.front();
    }
    
    /**
     * المستوى الحالي / Current level
     */
    int getCurrentLevel() const {
        return currentLevel_;
    }
    
    /**
     * عدد النطاقات / Number of scopes
     */
    size_t getScopeCount() const {
        return scopeStack_.size();
    }
    
    /**
     * إضافة رمز للنطاق الحالي / Add symbol to current scope
     * 
     * @param name اسم الرمز / Symbol name
     * @param type نوع الرمز / Symbol type
     * @param kind نوع الرمز / Symbol kind
     * @return الرمز المُضاف / Added symbol
     */
    std::shared_ptr<Symbol> addSymbol(
        const std::string& name,
        std::shared_ptr<Type> type,
        Symbol::Kind kind = Symbol::Kind::VARIABLE
    );
    
    /**
     * البحث عن رمز / Look up symbol
     * 
     * @param name اسم الرمز / Symbol name
     * @return الرمز أو nullptr / Symbol or nullptr
     */
    std::shared_ptr<Symbol> lookupSymbol(const std::string& name) const;
    
    /**
     * البحث عن رمز في النطاق الحالي فقط / Look up symbol in current scope only
     * 
     * @param name اسم الرمز / Symbol name
     * @return الرمز أو nullptr / Symbol or nullptr
     */
    std::shared_ptr<Symbol> lookupSymbolLocal(const std::string& name) const;
    
    /**
     * هل الرمز موجود / Symbol exists
     * 
     * @param name اسم الرمز / Symbol name
     * @return true إذا كان موجوداً / true if exists
     */
    bool symbolExists(const std::string& name) const {
        return lookupSymbol(name) != nullptr;
    }
    
    /**
     * حذف رمز من النطاق الحالي / Remove symbol from current scope
     * 
     * @param name اسم الرمز / Symbol name
     * @return true إذا تم الحذف / true if removed
     */
    bool removeSymbol(const std::string& name);
    
    /**
     * مسح جميع الرموز من النطاق الحالي / Clear all symbols from current scope
     */
    void clearCurrentScope();
    
    /**
     * إعادة تعيين البيئة / Reset environment
     */
    void reset();
};

// ============================================================================
// TypeContext - سياق Type Checking الشامل
// TypeContext - Complete Type Checking Context
// ============================================================================

/**
 * سياق شامل لعملية Type checking
 * Complete context for type checking process
 * 
 * يحتوي على:
 * - TypeEnvironment: بيئة الأنواع / Type environment
 * - ErrorList: قائمة الأخطاء / Error list
 * - Current function: الدالة الحالية / Current function
 * - Current class: الصنف الحالي / Current class
 */
class TypeContext {
private:
    std::shared_ptr<TypeEnvironment> environment_;  // بيئة الأنواع / Type environment
    std::vector<std::string> errors_;               // قائمة الأخطاء / Error list
    std::vector<std::string> warnings_;             // قائمة التحذيرات / Warning list
    
    // السياق الحالي / Current context
    std::shared_ptr<FunctionType> currentFunction_; // الدالة الحالية / Current function
    std::shared_ptr<ClassType> currentClass_;       // الصنف الحالي / Current class
    std::shared_ptr<Type> expectedReturnType_;      // نوع الإرجاع المتوقع / Expected return type
    
    // خيارات / Options
    bool strictMode_;                               // الوضع الصارم / Strict mode
    bool allowImplicitCasts_;                       // السماح بالتحويلات الضمنية / Allow implicit casts
    
public:
    /**
     * المنشئ / Constructor
     */
    TypeContext()
        : environment_(std::make_shared<TypeEnvironment>())
        , currentFunction_(nullptr)
        , currentClass_(nullptr)
        , expectedReturnType_(nullptr)
        , strictMode_(false)
        , allowImplicitCasts_(true)
    {}
    
    // Getters
    std::shared_ptr<TypeEnvironment> getEnvironment() const { return environment_; }
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    std::shared_ptr<FunctionType> getCurrentFunction() const { return currentFunction_; }
    std::shared_ptr<ClassType> getCurrentClass() const { return currentClass_; }
    std::shared_ptr<Type> getExpectedReturnType() const { return expectedReturnType_; }
    bool isStrictMode() const { return strictMode_; }
    bool allowsImplicitCasts() const { return allowImplicitCasts_; }
    
    // Setters
    void setCurrentFunction(std::shared_ptr<FunctionType> func) { currentFunction_ = func; }
    void setCurrentClass(std::shared_ptr<ClassType> cls) { currentClass_ = cls; }
    void setExpectedReturnType(std::shared_ptr<Type> type) { expectedReturnType_ = type; }
    void setStrictMode(bool strict) { strictMode_ = strict; }
    void setAllowImplicitCasts(bool allow) { allowImplicitCasts_ = allow; }
    
    /**
     * إضافة خطأ / Add error
     * 
     * @param message رسالة الخطأ / Error message
     */
    void addError(const std::string& message) {
        errors_.push_back(message);
    }
    
    /**
     * إضافة تحذير / Add warning
     * 
     * @param message رسالة التحذير / Warning message
     */
    void addWarning(const std::string& message) {
        warnings_.push_back(message);
    }
    
    /**
     * هل يوجد أخطاء / Has errors
     * 
     * @return true إذا كان يوجد أخطاء / true if has errors
     */
    bool hasErrors() const {
        return !errors_.empty();
    }
    
    /**
     * عدد الأخطاء / Number of errors
     */
    size_t getErrorCount() const {
        return errors_.size();
    }
    
    /**
     * عدد التحذيرات / Number of warnings
     */
    size_t getWarningCount() const {
        return warnings_.size();
    }
    
    /**
     * مسح جميع الأخطاء والتحذيرات / Clear all errors and warnings
     */
    void clearErrors() {
        errors_.clear();
        warnings_.clear();
    }
    
    /**
     * طباعة الأخطاء / Print errors
     */
    void printErrors() const;
    
    /**
     * طباعة التحذيرات / Print warnings
     */
    void printWarnings() const;
    
    /**
     * إعادة تعيين السياق / Reset context
     */
    void reset();
};

} // namespace TypeChecker
} // namespace Sad

#endif // SAD_TYPE_CONTEXT_H
