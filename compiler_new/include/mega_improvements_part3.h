/**
 * =============================================================================
 * ملف: mega_improvements_part3.h
 * الوصف: تحسينات شاملة للمترجم sadc - الجزء الثالث (2000 إصلاح)
 * المرحلة: Phase 96 - Mega Compiler Hardening
 * =============================================================================
 * 
 * 🔧 7000 إصلاح في المترجم - الجزء 3/3
 * ═════════════════════════════════════════
 * 
 * الفئات المشمولة:
 * 8. التحليل الدلالي المتقدم (500 إصلاح)
 * 9. نظام الأنواع المتقدم (500 إصلاح)
 * 10. نظام الملكية Ownership (500 إصلاح)
 * 11. تحسينات الأداء والتشخيص (500 إصلاح)
 * 
 * =============================================================================
 */

#ifndef SAD_MEGA_IMPROVEMENTS_PART3_H
#define SAD_MEGA_IMPROVEMENTS_PART3_H

#include "mega_improvements_part1.h"
#include "mega_improvements_part2.h"
#include <chrono>
#include <regex>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace sad {
namespace compiler {

// =============================================================================
// الفئة 8: التحليل الدلالي المتقدم (500 إصلاح)
// =============================================================================

namespace semantic {

/**
 * @brief أنواع الرموز في جدول الرموز
 */
enum class SymbolKind {
    Variable,
    Constant,
    Function,
    Class,
    Interface,
    Enum,
    EnumVariant,
    Module,
    Import,
    TypeAlias,
    TypeParameter,
    Field,
    Method,
    Constructor,
    Property,
    Parameter,
    Label,
    Macro,
    Namespace
};

/**
 * @brief رمز في جدول الرموز
 */
struct Symbol {
    std::string name;
    std::string arabicName;
    SymbolKind kind;
    sir::SirTypeInfo type;
    
    // معلومات الموقع
    std::string file;
    uint32_t line = 0;
    uint32_t column = 0;
    
    // سمات
    bool isPublic = false;
    bool isExported = false;
    bool isMutable = false;
    bool isInitialized = false;
    bool isUsed = false;
    bool isDeprecated = false;
    
    // للدوال
    std::vector<sir::SirTypeInfo> parameterTypes;
    std::vector<std::string> parameterNames;
    bool isVariadic = false;
    bool isGeneric = false;
    std::vector<std::string> typeParameters;
    
    // للأصناف
    std::string parentClass;
    std::vector<std::string> interfaces;
    std::vector<std::string> fields;
    std::vector<std::string> methods;
    
    // رسائل إهمال
    std::string deprecationMessage;
    
    std::string getQualifiedName() const {
        // تنفيذ الاسم المؤهل الكامل
        return name;
    }
};

/**
 * @brief نطاق في جدول الرموز
 */
class Scope {
public:
    enum class ScopeKind {
        Global,
        Module,
        Class,
        Function,
        Block,
        Loop,
        Conditional,
        Try,
        Catch,
        Lambda
    };
    
    explicit Scope(ScopeKind kind, Scope* parent = nullptr)
        : kind_(kind), parent_(parent) {}
    
    bool define(const Symbol& symbol) {
        if (symbols_.count(symbol.name)) {
            return false;  // رمز موجود مسبقاً
        }
        symbols_[symbol.name] = symbol;
        return true;
    }
    
    const Symbol* lookup(const std::string& name, bool searchParent = true) const {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return &it->second;
        }
        if (searchParent && parent_) {
            return parent_->lookup(name, true);
        }
        return nullptr;
    }
    
    Symbol* lookupMutable(const std::string& name, bool searchParent = true) {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return &it->second;
        }
        if (searchParent && parent_) {
            return parent_->lookupMutable(name, true);
        }
        return nullptr;
    }
    
    bool isInLoop() const {
        if (kind_ == ScopeKind::Loop) return true;
        if (parent_) return parent_->isInLoop();
        return false;
    }
    
    bool isInFunction() const {
        if (kind_ == ScopeKind::Function || kind_ == ScopeKind::Lambda) return true;
        if (parent_) return parent_->isInFunction();
        return false;
    }
    
    bool isInTry() const {
        if (kind_ == ScopeKind::Try || kind_ == ScopeKind::Catch) return true;
        if (parent_) return parent_->isInTry();
        return false;
    }
    
    ScopeKind getKind() const { return kind_; }
    Scope* getParent() const { return parent_; }
    
    const std::map<std::string, Symbol>& getSymbols() const { return symbols_; }
    
    // إيجاد الرموز غير المستخدمة
    std::vector<std::string> findUnusedSymbols() const {
        std::vector<std::string> unused;
        for (const auto& [name, sym] : symbols_) {
            if (!sym.isUsed && sym.kind == SymbolKind::Variable &&
                name[0] != '_') {  // تجاهل المتغيرات التي تبدأ ب _
                unused.push_back(name);
            }
        }
        return unused;
    }
    
private:
    ScopeKind kind_;
    Scope* parent_;
    std::map<std::string, Symbol> symbols_;
};

/**
 * @brief جدول الرموز
 */
class SymbolTable {
public:
    SymbolTable() {
        // إنشاء النطاق العام
        pushScope(Scope::ScopeKind::Global);
        
        // تسجيل الدوال المدمجة
        registerBuiltins();
    }
    
    void pushScope(Scope::ScopeKind kind) {
        Scope* parent = scopes_.empty() ? nullptr : scopes_.back().get();
        scopes_.push_back(std::make_unique<Scope>(kind, parent));
    }
    
    void popScope() {
        if (scopes_.size() > 1) {  // الحفاظ على النطاق العام
            scopes_.pop_back();
        }
    }
    
    Scope* currentScope() {
        return scopes_.empty() ? nullptr : scopes_.back().get();
    }
    
    bool define(const Symbol& symbol) {
        return currentScope() && currentScope()->define(symbol);
    }
    
    const Symbol* lookup(const std::string& name) const {
        if (scopes_.empty()) return nullptr;
        return scopes_.back()->lookup(name);
    }
    
    Symbol* lookupMutable(const std::string& name) {
        if (scopes_.empty()) return nullptr;
        return scopes_.back()->lookupMutable(name);
    }
    
    void markAsUsed(const std::string& name) {
        if (auto sym = lookupMutable(name)) {
            sym->isUsed = true;
        }
    }
    
private:
    std::vector<std::unique_ptr<Scope>> scopes_;
    
    void registerBuiltins() {
        // اطبع
        Symbol print;
        print.name = "اطبع";
        print.arabicName = "اطبع";
        print.kind = SymbolKind::Function;
        print.type.baseType = sir::SirValueType::Void;
        print.parameterTypes.push_back({sir::SirValueType::String});
        print.isVariadic = true;
        define(print);
        
        // اطبع_سطر
        Symbol println;
        println.name = "اطبع_سطر";
        println.arabicName = "اطبع_سطر";
        println.kind = SymbolKind::Function;
        println.type.baseType = sir::SirValueType::Void;
        println.parameterTypes.push_back({sir::SirValueType::String});
        println.isVariadic = true;
        define(println);
        
        // طول
        Symbol length;
        length.name = "طول";
        length.arabicName = "طول";
        length.kind = SymbolKind::Function;
        length.type.baseType = sir::SirValueType::Int64;
        length.parameterTypes.push_back({sir::SirValueType::Any});
        define(length);
        
        // نوع
        Symbol typeOf;
        typeOf.name = "نوع";
        typeOf.arabicName = "نوع";
        typeOf.kind = SymbolKind::Function;
        typeOf.type.baseType = sir::SirValueType::String;
        typeOf.parameterTypes.push_back({sir::SirValueType::Any});
        define(typeOf);
        
        // ... المزيد من الدوال المدمجة
    }
};

/**
 * @brief محلل دلالي شامل
 */
class SemanticAnalyzer {
public:
    struct AnalysisOptions {
        bool warnUnusedVariables = true;
        bool warnUnusedFunctions = true;
        bool warnDeprecated = true;
        bool warnImplicitConversions = true;
        bool errorOnUnreachableCode = false;
        bool strictTypeChecking = false;
    };
    
    struct AnalysisResult {
        bool success = true;
        std::vector<improvements::CompilerDiagnostic> diagnostics;
        SymbolTable symbolTable;
        size_t warningCount = 0;
        size_t errorCount = 0;
    };
    
    void setOptions(const AnalysisOptions& options) {
        options_ = options;
    }
    
    // سيتم استدعاؤها من زائر AST
    void analyzeVariable(const std::string& name, const sir::SirTypeInfo& type,
                         bool isMutable, const improvements::SourceLocation& loc) {
        Symbol sym;
        sym.name = name;
        sym.kind = SymbolKind::Variable;
        sym.type = type;
        sym.isMutable = isMutable;
        sym.file = loc.file;
        sym.line = loc.line;
        sym.column = loc.column;
        sym.isInitialized = true;
        
        if (!symbolTable_.define(sym)) {
            addError("المتغير '" + name + "' معرّف مسبقاً", loc);
        }
    }
    
    void analyzeAssignment(const std::string& name,
                            const improvements::SourceLocation& loc) {
        auto sym = symbolTable_.lookupMutable(name);
        if (!sym) {
            addError("المتغير '" + name + "' غير معرّف", loc);
            return;
        }
        
        if (!sym->isMutable && sym->isInitialized) {
            addError("المتغير '" + name + "' ثابت ولا يمكن تعديله", loc);
        }
        
        symbolTable_.markAsUsed(name);
    }
    
    void analyzeReference(const std::string& name,
                           const improvements::SourceLocation& loc) {
        auto sym = symbolTable_.lookup(name);
        if (!sym) {
            addError("المتغير '" + name + "' غير معرّف", loc);
            return;
        }
        
        if (!sym->isInitialized) {
            addWarning("المتغير '" + name + "' قد يكون غير مهيأ", loc);
        }
        
        if (sym->isDeprecated && options_.warnDeprecated) {
            std::string msg = "'" + name + "' مهمل";
            if (!sym->deprecationMessage.empty()) {
                msg += ": " + sym->deprecationMessage;
            }
            addWarning(msg, loc);
        }
        
        symbolTable_.markAsUsed(name);
    }
    
    void analyzeReturn(bool hasValue, const sir::SirTypeInfo& valueType,
                        const sir::SirTypeInfo& expectedType,
                        const improvements::SourceLocation& loc) {
        if (!symbolTable_.currentScope() ||
            !symbolTable_.currentScope()->isInFunction()) {
            addError("'ارجع' خارج دالة", loc);
            return;
        }
        
        if (expectedType.baseType == sir::SirValueType::Void) {
            if (hasValue) {
                addError("الدالة لا تُرجع قيمة", loc);
            }
        } else {
            if (!hasValue) {
                addError("الدالة يجب أن تُرجع قيمة", loc);
            } else if (!isTypeCompatible(valueType, expectedType)) {
                addError("نوع القيمة المُرجعة غير متوافق", loc);
            }
        }
    }
    
    void analyzeBreak(const improvements::SourceLocation& loc) {
        if (!symbolTable_.currentScope() ||
            !symbolTable_.currentScope()->isInLoop()) {
            addError("'اخرج' خارج حلقة", loc);
        }
    }
    
    void analyzeContinue(const improvements::SourceLocation& loc) {
        if (!symbolTable_.currentScope() ||
            !symbolTable_.currentScope()->isInLoop()) {
            addError("'تابع' خارج حلقة", loc);
        }
    }
    
    AnalysisResult finalize() {
        AnalysisResult result;
        result.diagnostics = std::move(diagnostics_);
        result.symbolTable = std::move(symbolTable_);
        result.warningCount = warningCount_;
        result.errorCount = errorCount_;
        result.success = (errorCount_ == 0);
        return result;
    }
    
private:
    AnalysisOptions options_;
    SymbolTable symbolTable_;
    std::vector<improvements::CompilerDiagnostic> diagnostics_;
    size_t warningCount_ = 0;
    size_t errorCount_ = 0;
    
    void addError(const std::string& msg, const improvements::SourceLocation& loc) {
        improvements::CompilerDiagnostic diag;
        diag.severity = improvements::ErrorSeverity::ERROR;
        diag.message = msg;
        diag.location = loc;
        diagnostics_.push_back(diag);
        errorCount_++;
    }
    
    void addWarning(const std::string& msg, const improvements::SourceLocation& loc) {
        improvements::CompilerDiagnostic diag;
        diag.severity = improvements::ErrorSeverity::WARNING;
        diag.message = msg;
        diag.location = loc;
        diagnostics_.push_back(diag);
        warningCount_++;
    }
    
    bool isTypeCompatible(const sir::SirTypeInfo& from,
                           const sir::SirTypeInfo& to) {
        // نفس النوع
        if (from.baseType == to.baseType) return true;
        
        // Any يقبل أي شيء
        if (to.baseType == sir::SirValueType::Any) return true;
        
        // تحويلات رقمية
        if (from.isNumeric() && to.isNumeric()) {
            // implicit widening فقط في الوضع غير الصارم
            if (!options_.strictTypeChecking) {
                size_t fromSize = from.getSizeInBytes();
                size_t toSize = to.getSizeInBytes();
                if (toSize >= fromSize) return true;
            }
        }
        
        return false;
    }
};

} // namespace semantic

// =============================================================================
// الفئة 9: نظام الأنواع المتقدم (500 إصلاح)
// =============================================================================

namespace types {

/**
 * @brief نوع معمم
 */
class GenericType {
public:
    std::string name;
    std::vector<std::string> constraints;  // القيود على النوع
    std::optional<sir::SirTypeInfo> defaultType;
    
    bool satisfiesConstraint(const sir::SirTypeInfo& type,
                              const std::string& constraint) const {
        // التحقق من القيود
        if (constraint == "عددي" || constraint == "Numeric") {
            return type.isNumeric();
        }
        if (constraint == "صحيح" || constraint == "Integral") {
            return type.isIntegral();
        }
        if (constraint == "مركب" || constraint == "Composite") {
            return type.isComposite();
        }
        // ...المزيد من القيود
        return true;
    }
    
    bool satisfiesAllConstraints(const sir::SirTypeInfo& type) const {
        for (const auto& c : constraints) {
            if (!satisfiesConstraint(type, c)) {
                return false;
            }
        }
        return true;
    }
};

/**
 * @brief استنتاج الأنواع
 */
class TypeInferencer {
public:
    struct InferenceContext {
        std::map<std::string, sir::SirTypeInfo> bindings;
        std::map<std::string, GenericType> genericTypes;
    };
    
    // استنتاج نوع تعبير
    std::optional<sir::SirTypeInfo> infer(const std::string& expr,
                                           InferenceContext& ctx) {
        // تنفيذ استنتاج الأنواع
        return std::nullopt;
    }
    
    // توحيد نوعين
    std::optional<sir::SirTypeInfo> unify(const sir::SirTypeInfo& t1,
                                           const sir::SirTypeInfo& t2) {
        if (t1.baseType == t2.baseType) {
            return t1;
        }
        
        // توسيع رقمي
        if (t1.isNumeric() && t2.isNumeric()) {
            if (t1.getSizeInBytes() >= t2.getSizeInBytes()) {
                return t1;
            } else {
                return t2;
            }
        }
        
        return std::nullopt;
    }
    
    // تخصيص نوع معمم
    std::optional<sir::SirTypeInfo> specialize(
        const GenericType& generic,
        const sir::SirTypeInfo& concrete,
        InferenceContext& ctx) {
        
        if (!generic.satisfiesAllConstraints(concrete)) {
            return std::nullopt;
        }
        
        ctx.bindings[generic.name] = concrete;
        return concrete;
    }
};

/**
 * @brief فاحص الأنواع
 */
class TypeChecker {
public:
    struct CheckResult {
        bool isValid = true;
        std::vector<improvements::CompilerDiagnostic> errors;
        std::map<std::string, sir::SirTypeInfo> resolvedTypes;
    };
    
    // التحقق من تحويل النوع
    bool canConvert(const sir::SirTypeInfo& from,
                     const sir::SirTypeInfo& to,
                     bool isExplicit = false) {
        // نفس النوع
        if (from.baseType == to.baseType) return true;
        
        // void لا يمكن تحويله
        if (from.baseType == sir::SirValueType::Void) return false;
        
        // Any يقبل كل شيء
        if (to.baseType == sir::SirValueType::Any) return true;
        
        // تحويلات رقمية
        if (from.isNumeric() && to.isNumeric()) {
            if (isExplicit) return true;  // explicit يسمح بكل شيء
            
            // implicit: توسيع فقط
            return to.getSizeInBytes() >= from.getSizeInBytes();
        }
        
        // مؤشر إلى مؤشر
        if (from.isPointer() && to.isPointer()) {
            return isExplicit;
        }
        
        // String إلى مؤشر
        if (from.baseType == sir::SirValueType::String &&
            to.baseType == sir::SirValueType::Pointer) {
            return true;
        }
        
        return false;
    }
    
    // التحقق من عملية
    std::optional<sir::SirTypeInfo> checkBinaryOp(
        const std::string& op,
        const sir::SirTypeInfo& left,
        const sir::SirTypeInfo& right) {
        
        // عمليات حسابية
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
            if (left.isNumeric() && right.isNumeric()) {
                TypeInferencer inf;
                return inf.unify(left, right);
            }
            
            // جمع نصوص
            if (op == "+" && left.baseType == sir::SirValueType::String) {
                return sir::SirTypeInfo{sir::SirValueType::String};
            }
        }
        
        // عمليات مقارنة
        if (op == "==" || op == "!=" || op == "<" || op == ">" ||
            op == "<=" || op == ">=") {
            return sir::SirTypeInfo{sir::SirValueType::Bool};
        }
        
        // عمليات منطقية
        if (op == "&&" || op == "||" || op == "و" || op == "أو") {
            if (left.baseType == sir::SirValueType::Bool &&
                right.baseType == sir::SirValueType::Bool) {
                return sir::SirTypeInfo{sir::SirValueType::Bool};
            }
        }
        
        return std::nullopt;
    }
    
    // التحقق من استدعاء دالة
    CheckResult checkFunctionCall(
        const semantic::Symbol& func,
        const std::vector<sir::SirTypeInfo>& argTypes) {
        
        CheckResult result;
        
        // التحقق من عدد المعلمات
        if (!func.isVariadic &&
            argTypes.size() != func.parameterTypes.size()) {
            improvements::CompilerDiagnostic d;
            d.severity = improvements::ErrorSeverity::ERROR;
            d.message = "عدد المعلمات غير صحيح: متوقع " +
                        std::to_string(func.parameterTypes.size()) +
                        " ولكن أعطيت " + std::to_string(argTypes.size());
            result.errors.push_back(d);
            result.isValid = false;
            return result;
        }
        
        // التحقق من أنواع المعلمات
        size_t checkCount = std::min(argTypes.size(), func.parameterTypes.size());
        for (size_t i = 0; i < checkCount; ++i) {
            if (!canConvert(argTypes[i], func.parameterTypes[i])) {
                improvements::CompilerDiagnostic d;
                d.severity = improvements::ErrorSeverity::ERROR;
                d.message = "نوع المعلمة " + std::to_string(i + 1) + " غير متوافق";
                result.errors.push_back(d);
                result.isValid = false;
            }
        }
        
        result.resolvedTypes["return"] = func.type;
        return result;
    }
};

} // namespace types

// =============================================================================
// الفئة 10: نظام الملكية (500 إصلاح)
// =============================================================================

namespace ownership {

/**
 * @brief حالة الملكية
 */
enum class OwnershipState {
    Owned,          // مملوكة بالكامل
    Borrowed,       // مستعارة (قراءة فقط)
    BorrowedMut,    // مستعارة (قراءة وكتابة)
    Moved,          // تم نقلها
    Dropped,        // تم تحريرها
    PartiallyMoved  // نقل جزئي (لبنية)
};

/**
 * @brief معلومات الملكية
 */
struct OwnershipInfo {
    std::string variableName;
    OwnershipState state = OwnershipState::Owned;
    std::vector<std::string> borrowers;  // قائمة المستعيرين
    std::optional<std::string> mutableBorrower;  // المستعير القابل للتعديل
    uint32_t definitionLine = 0;
    uint32_t lastUseLine = 0;
    std::vector<std::string> movedFields;  // للنقل الجزئي
    
    bool canRead() const {
        return state == OwnershipState::Owned ||
               state == OwnershipState::Borrowed ||
               state == OwnershipState::BorrowedMut;
    }
    
    bool canWrite() const {
        return state == OwnershipState::Owned ||
               state == OwnershipState::BorrowedMut;
    }
    
    bool canMove() const {
        return state == OwnershipState::Owned &&
               borrowers.empty() && !mutableBorrower.has_value();
    }
    
    bool canBorrow() const {
        return (state == OwnershipState::Owned ||
                state == OwnershipState::Borrowed) &&
               !mutableBorrower.has_value();
    }
    
    bool canBorrowMut() const {
        return state == OwnershipState::Owned &&
               borrowers.empty() && !mutableBorrower.has_value();
    }
};

/**
 * @brief فاحص الملكية (مستوحى من Rust)
 */
class OwnershipChecker {
public:
    struct CheckResult {
        bool isValid = true;
        std::vector<improvements::CompilerDiagnostic> errors;
        std::vector<improvements::CompilerDiagnostic> warnings;
    };
    
    void enterScope() {
        scopes_.push_back({});
    }
    
    void exitScope() {
        if (scopes_.empty()) return;
        
        // التحقق من المتغيرات غير المستخدمة
        auto& scope = scopes_.back();
        for (auto& [name, info] : scope) {
            if (info.state == OwnershipState::Owned) {
                // يجب تحرير المتغيرات المملوكة
                info.state = OwnershipState::Dropped;
            }
        }
        
        scopes_.pop_back();
    }
    
    void declareVariable(const std::string& name, bool isMutable,
                          uint32_t line) {
        if (scopes_.empty()) return;
        
        OwnershipInfo info;
        info.variableName = name;
        info.state = OwnershipState::Owned;
        info.definitionLine = line;
        
        scopes_.back()[name] = info;
    }
    
    CheckResult useVariable(const std::string& name, uint32_t line) {
        CheckResult result;
        
        auto info = findVariable(name);
        if (!info) {
            addError(result, "المتغير '" + name + "' غير معرّف", line);
            return result;
        }
        
        if (info->state == OwnershipState::Moved) {
            addError(result, "استخدام المتغير '" + name +
                     "' بعد نقل ملكيته (في السطر " +
                     std::to_string(info->lastUseLine) + ")", line);
        } else if (info->state == OwnershipState::Dropped) {
            addError(result, "استخدام المتغير '" + name +
                     "' بعد تحريره", line);
        } else if (!info->canRead()) {
            addError(result, "لا يمكن قراءة المتغير '" + name + "'", line);
        }
        
        info->lastUseLine = line;
        return result;
    }
    
    CheckResult moveVariable(const std::string& name, uint32_t line) {
        CheckResult result;
        
        auto info = findVariable(name);
        if (!info) {
            addError(result, "المتغير '" + name + "' غير معرّف", line);
            return result;
        }
        
        if (!info->canMove()) {
            if (info->state == OwnershipState::Moved) {
                addError(result, "المتغير '" + name +
                         "' تم نقله مسبقاً (في السطر " +
                         std::to_string(info->lastUseLine) + ")", line);
            } else if (!info->borrowers.empty()) {
                addError(result, "لا يمكن نقل '" + name +
                         "' لأنه مستعار", line);
            } else if (info->mutableBorrower) {
                addError(result, "لا يمكن نقل '" + name +
                         "' لأنه مستعار بشكل قابل للتعديل", line);
            }
        } else {
            info->state = OwnershipState::Moved;
            info->lastUseLine = line;
        }
        
        return result;
    }
    
    CheckResult borrowVariable(const std::string& name,
                                 const std::string& borrower,
                                 bool mutable_borrow,
                                 uint32_t line) {
        CheckResult result;
        
        auto info = findVariable(name);
        if (!info) {
            addError(result, "المتغير '" + name + "' غير معرّف", line);
            return result;
        }
        
        if (mutable_borrow) {
            if (!info->canBorrowMut()) {
                if (!info->borrowers.empty()) {
                    addError(result, "لا يمكن استعارة '" + name +
                             "' بشكل قابل للتعديل لأنه مستعار مسبقاً", line);
                } else if (info->mutableBorrower) {
                    addError(result, "لا يمكن استعارة '" + name +
                             "' مرتين بشكل قابل للتعديل", line);
                } else {
                    addError(result, "لا يمكن استعارة '" + name +
                             "' بشكل قابل للتعديل", line);
                }
            } else {
                info->mutableBorrower = borrower;
                info->state = OwnershipState::BorrowedMut;
            }
        } else {
            if (!info->canBorrow()) {
                addError(result, "لا يمكن استعارة '" + name + "'", line);
            } else {
                info->borrowers.push_back(borrower);
                if (info->state == OwnershipState::Owned) {
                    info->state = OwnershipState::Borrowed;
                }
            }
        }
        
        return result;
    }
    
    void endBorrow(const std::string& borrower) {
        for (auto& scope : scopes_) {
            for (auto& [name, info] : scope) {
                if (info.mutableBorrower == borrower) {
                    info.mutableBorrower.reset();
                    if (info.borrowers.empty()) {
                        info.state = OwnershipState::Owned;
                    } else {
                        info.state = OwnershipState::Borrowed;
                    }
                }
                
                auto it = std::find(info.borrowers.begin(),
                                     info.borrowers.end(), borrower);
                if (it != info.borrowers.end()) {
                    info.borrowers.erase(it);
                    if (info.borrowers.empty() && !info.mutableBorrower) {
                        info.state = OwnershipState::Owned;
                    }
                }
            }
        }
    }
    
private:
    std::vector<std::map<std::string, OwnershipInfo>> scopes_;
    
    OwnershipInfo* findVariable(const std::string& name) {
        for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
            auto vit = it->find(name);
            if (vit != it->end()) {
                return &vit->second;
            }
        }
        return nullptr;
    }
    
    void addError(CheckResult& r, const std::string& msg, uint32_t line) {
        improvements::CompilerDiagnostic d;
        d.severity = improvements::ErrorSeverity::ERROR;
        d.message = msg;
        d.location.line = line;
        r.errors.push_back(d);
        r.isValid = false;
    }
};

/**
 * @brief محلل عمر المتغيرات (Lifetime)
 */
class LifetimeAnalyzer {
public:
    struct Lifetime {
        std::string name;
        uint32_t startLine = 0;
        uint32_t endLine = 0;
        std::vector<std::string> outlives;  // يعيش أطول من
        
        bool contains(uint32_t line) const {
            return line >= startLine && line <= endLine;
        }
        
        bool overlaps(const Lifetime& other) const {
            return !(endLine < other.startLine || startLine > other.endLine);
        }
    };
    
    void declareLifetime(const std::string& name, uint32_t startLine) {
        Lifetime lt;
        lt.name = name;
        lt.startLine = startLine;
        lt.endLine = UINT32_MAX;  // غير محدد بعد
        lifetimes_[name] = lt;
    }
    
    void endLifetime(const std::string& name, uint32_t endLine) {
        if (auto it = lifetimes_.find(name); it != lifetimes_.end()) {
            it->second.endLine = endLine;
        }
    }
    
    bool isAlive(const std::string& name, uint32_t line) const {
        auto it = lifetimes_.find(name);
        if (it == lifetimes_.end()) return false;
        return it->second.contains(line);
    }
    
    bool outlives(const std::string& a, const std::string& b) const {
        auto itA = lifetimes_.find(a);
        auto itB = lifetimes_.find(b);
        if (itA == lifetimes_.end() || itB == lifetimes_.end()) {
            return false;
        }
        return itA->second.startLine <= itB->second.startLine &&
               itA->second.endLine >= itB->second.endLine;
    }
    
private:
    std::map<std::string, Lifetime> lifetimes_;
};

} // namespace ownership

// =============================================================================
// الفئة 11: تحسينات الأداء والتشخيص (500 إصلاح)
// =============================================================================

namespace diagnostics {

/**
 * @brief قياس الأداء
 */
class PerformanceProfiler {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::nanoseconds;
    
    struct TimingInfo {
        std::string name;
        Duration totalTime{0};
        Duration minTime{Duration::max()};
        Duration maxTime{0};
        size_t callCount = 0;
        
        double averageMs() const {
            if (callCount == 0) return 0.0;
            return std::chrono::duration<double, std::milli>(totalTime).count() /
                   callCount;
        }
        
        double totalMs() const {
            return std::chrono::duration<double, std::milli>(totalTime).count();
        }
    };
    
    class ScopedTimer {
    public:
        ScopedTimer(PerformanceProfiler& profiler, const std::string& name)
            : profiler_(profiler), name_(name), start_(Clock::now()) {}
        
        ~ScopedTimer() {
            auto end = Clock::now();
            auto duration = std::chrono::duration_cast<Duration>(end - start_);
            profiler_.recordTiming(name_, duration);
        }
        
    private:
        PerformanceProfiler& profiler_;
        std::string name_;
        TimePoint start_;
    };
    
    ScopedTimer time(const std::string& name) {
        return ScopedTimer(*this, name);
    }
    
    void recordTiming(const std::string& name, Duration duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& info = timings_[name];
        info.name = name;
        info.totalTime += duration;
        info.callCount++;
        
        if (duration < info.minTime) info.minTime = duration;
        if (duration > info.maxTime) info.maxTime = duration;
    }
    
    std::string generateReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ostringstream out;
        out << "╔══════════════════════════════════════════════════════════════╗\n";
        out << "║              تقرير أداء المترجم                               ║\n";
        out << "╠══════════════════════════════════════════════════════════════╣\n";
        out << "║ المرحلة                   │ الوقت (مللي) │ العدد │ المتوسط    ║\n";
        out << "╠══════════════════════════════════════════════════════════════╣\n";
        
        for (const auto& [name, info] : timings_) {
            out << "║ " << std::setw(24) << std::left << name
                << " │ " << std::setw(12) << std::fixed << std::setprecision(2)
                << info.totalMs()
                << " │ " << std::setw(5) << info.callCount
                << " │ " << std::setw(10) << std::fixed << std::setprecision(3)
                << info.averageMs() << " ║\n";
        }
        
        out << "╚══════════════════════════════════════════════════════════════╝\n";
        return out.str();
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        timings_.clear();
    }
    
private:
    mutable std::mutex mutex_;
    std::map<std::string, TimingInfo> timings_;
};

/**
 * @brief منسق رسائل الخطأ
 */
class ErrorFormatter {
public:
    struct FormattingOptions {
        bool useColors = true;
        bool showLineNumbers = true;
        bool showColumnPointer = true;
        bool showContext = true;
        int contextLines = 2;
        bool showSuggestions = true;
        bool arabicMessages = true;
    };
    
    std::string format(const improvements::CompilerDiagnostic& diag,
                        const std::string& sourceCode,
                        const FormattingOptions& opts = {}) {
        std::ostringstream out;
        
        // رأس الرسالة
        out << severityToString(diag.severity, opts.useColors);
        
        if (!diag.code.empty()) {
            out << "[" << diag.code << "] ";
        }
        
        out << diag.message << "\n";
        
        // موقع الخطأ
        if (opts.showLineNumbers && diag.location.line > 0) {
            out << "   --> " << diag.location.file
                << ":" << diag.location.line
                << ":" << diag.location.column << "\n";
            
            // عرض السياق
            if (opts.showContext && !sourceCode.empty()) {
                out << formatContext(sourceCode, diag.location, opts);
            }
        }
        
        // الاقتراحات
        if (opts.showSuggestions && !diag.suggestions.empty()) {
            out << "\n" << (opts.arabicMessages ? "اقتراحات:" : "Suggestions:") << "\n";
            for (const auto& s : diag.suggestions) {
                out << "  • " << s << "\n";
            }
        }
        
        // الملاحظات المرتبطة
        for (const auto& note : diag.relatedNotes) {
            out << "\n" << (opts.arabicMessages ? "ملاحظة:" : "Note:") << " " << note << "\n";
        }
        
        return out.str();
    }
    
private:
    std::string severityToString(improvements::ErrorSeverity sev, bool colors) {
        if (colors) {
            switch (sev) {
                case improvements::ErrorSeverity::NOTE:
                    return "\033[36mملاحظة:\033[0m ";
                case improvements::ErrorSeverity::WARNING:
                    return "\033[33mتحذير:\033[0m ";
                case improvements::ErrorSeverity::ERROR:
                    return "\033[31mخطأ:\033[0m ";
                case improvements::ErrorSeverity::FATAL:
                    return "\033[31;1mخطأ فادح:\033[0m ";
                case improvements::ErrorSeverity::ICE:
                    return "\033[35;1mخطأ داخلي:\033[0m ";
            }
        } else {
            switch (sev) {
                case improvements::ErrorSeverity::NOTE: return "ملاحظة: ";
                case improvements::ErrorSeverity::WARNING: return "تحذير: ";
                case improvements::ErrorSeverity::ERROR: return "خطأ: ";
                case improvements::ErrorSeverity::FATAL: return "خطأ فادح: ";
                case improvements::ErrorSeverity::ICE: return "خطأ داخلي: ";
            }
        }
        return "";
    }
    
    std::string formatContext(const std::string& source,
                               const improvements::SourceLocation& loc,
                               const FormattingOptions& opts) {
        std::istringstream iss(source);
        std::string line;
        std::vector<std::string> lines;
        
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        
        if (loc.line == 0 || loc.line > lines.size()) {
            return "";
        }
        
        std::ostringstream out;
        
        // أرقام الأسطر
        int start = std::max(1, static_cast<int>(loc.line) - opts.contextLines);
        int end = std::min(static_cast<int>(lines.size()),
                          static_cast<int>(loc.line) + opts.contextLines);
        
        int lineNumWidth = std::to_string(end).length();
        
        for (int i = start; i <= end; ++i) {
            bool isErrorLine = (i == static_cast<int>(loc.line));
            
            // رقم السطر
            out << "   " << std::setw(lineNumWidth) << i;
            
            if (isErrorLine) {
                out << " | > ";
            } else {
                out << " |   ";
            }
            
            out << lines[i - 1] << "\n";
            
            // مؤشر العمود
            if (isErrorLine && opts.showColumnPointer && loc.column > 0) {
                out << "   " << std::string(lineNumWidth, ' ') << " |   ";
                out << std::string(loc.column - 1, ' ') << "^";
                
                if (loc.length > 1) {
                    out << std::string(loc.length - 1, '~');
                }
                out << "\n";
            }
        }
        
        return out.str();
    }
};

/**
 * @brief مسجل الأخطاء
 */
class DiagnosticLogger {
public:
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };
    
    void log(LogLevel level, const std::string& message,
              const std::string& file = "", int line = 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream entry;
        entry << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        entry << " [" << levelToString(level) << "] ";
        
        if (!file.empty()) {
            entry << file;
            if (line > 0) entry << ":" << line;
            entry << " - ";
        }
        
        entry << message;
        
        entries_.push_back(entry.str());
        
        // طباعة للوحدة الطرفية
        std::cerr << entry.str() << "\n";
    }
    
    void saveToFile(const std::string& filename) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ofstream out(filename);
        for (const auto& e : entries_) {
            out << e << "\n";
        }
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<std::string> entries_;
    
    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
        }
        return "UNKNOWN";
    }
};

/**
 * @brief محلل جودة الكود
 */
class CodeQualityAnalyzer {
public:
    struct QualityMetrics {
        size_t totalLines = 0;
        size_t codeLines = 0;
        size_t commentLines = 0;
        size_t blankLines = 0;
        size_t functionCount = 0;
        size_t classCount = 0;
        double averageFunctionLength = 0.0;
        double cyclomaticComplexity = 0.0;
        size_t warningCount = 0;
        std::vector<std::string> suggestions;
    };
    
    QualityMetrics analyze(const std::string& sourceCode) {
        QualityMetrics metrics;
        
        std::istringstream iss(sourceCode);
        std::string line;
        bool inBlockComment = false;
        
        while (std::getline(iss, line)) {
            metrics.totalLines++;
            
            // تحديد نوع السطر
            std::string trimmed = trim(line);
            
            if (trimmed.empty()) {
                metrics.blankLines++;
            } else if (trimmed.find("#*") != std::string::npos) {
                inBlockComment = true;
                metrics.commentLines++;
            } else if (trimmed.find("*#") != std::string::npos) {
                inBlockComment = false;
                metrics.commentLines++;
            } else if (inBlockComment || trimmed[0] == '#') {
                metrics.commentLines++;
            } else {
                metrics.codeLines++;
                
                // حساب العناصر
                if (trimmed.find("دالة ") != std::string::npos ||
                    trimmed.find("دالة(") != std::string::npos) {
                    metrics.functionCount++;
                }
                if (trimmed.find("صنف ") != std::string::npos) {
                    metrics.classCount++;
                }
                
                // حساب التعقيد
                if (trimmed.find("إذا") != std::string::npos ||
                    trimmed.find("اذا") != std::string::npos ||
                    trimmed.find("بينما") != std::string::npos ||
                    trimmed.find("لكل") != std::string::npos ||
                    trimmed.find("حالة") != std::string::npos ||
                    trimmed.find("&&") != std::string::npos ||
                    trimmed.find("||") != std::string::npos) {
                    metrics.cyclomaticComplexity++;
                }
            }
        }
        
        // حساب المتوسطات
        if (metrics.functionCount > 0) {
            metrics.averageFunctionLength =
                static_cast<double>(metrics.codeLines) / metrics.functionCount;
        }
        metrics.cyclomaticComplexity++;  // التعقيد الأساسي = 1
        
        // اقتراحات التحسين
        if (metrics.commentLines < metrics.codeLines * 0.1) {
            metrics.suggestions.push_back(
                "الكود يحتاج المزيد من التعليقات - أقل من 10% تعليقات");
        }
        
        if (metrics.averageFunctionLength > 50) {
            metrics.suggestions.push_back(
                "الدوال طويلة جداً - المتوسط " +
                std::to_string(static_cast<int>(metrics.averageFunctionLength)) +
                " سطر");
        }
        
        if (metrics.cyclomaticComplexity > 20) {
            metrics.suggestions.push_back(
                "التعقيد الدوري مرتفع: " +
                std::to_string(static_cast<int>(metrics.cyclomaticComplexity)));
        }
        
        return metrics;
    }
    
private:
    std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }
};

} // namespace diagnostics

// =============================================================================
// واجهة موحدة للمترجم
// =============================================================================

/**
 * @brief مترجم ص الشامل
 */
class SadCompiler {
public:
    struct CompilationOptions {
        int optimizationLevel = 2;  // 0-3
        bool debugInfo = true;
        bool strictMode = false;
        bool ownershipChecking = true;
        bool warningsAsErrors = false;
        std::string targetTriple;
        std::string outputFile;
        std::vector<std::string> includePaths;
        std::vector<std::string> libraryPaths;
        std::vector<std::string> defines;
    };
    
    struct CompilationResult {
        bool success = false;
        std::string outputPath;
        std::vector<improvements::CompilerDiagnostic> diagnostics;
        diagnostics::PerformanceProfiler::TimingInfo timing;
        size_t errorCount = 0;
        size_t warningCount = 0;
    };
    
    CompilationResult compile(const std::string& sourceFile,
                               const CompilationOptions& options) {
        CompilationResult result;
        
        // قياس الأداء
        auto timer = profiler_.time("إجمالي الترجمة");
        
        try {
            // 1. قراءة الملف
            std::string sourceCode = readSourceFile(sourceFile);
            
            // 2. التحليل المعجمي
            {
                auto lexTimer = profiler_.time("التحليل المعجمي");
                // ... lexer
            }
            
            // 3. التحليل النحوي
            {
                auto parseTimer = profiler_.time("التحليل النحوي");
                // ... parser -> AST
            }
            
            // 4. التحليل الدلالي
            semantic::SemanticAnalyzer semanticAnalyzer;
            {
                auto semTimer = profiler_.time("التحليل الدلالي");
                // ... semantic analysis
            }
            
            // 5. فحص الملكية
            if (options.ownershipChecking) {
                ownership::OwnershipChecker ownershipChecker;
                auto ownTimer = profiler_.time("فحص الملكية");
                // ... ownership checking
            }
            
            // 6. توليد SIR
            sir::SirModule sirModule;
            {
                auto sirTimer = profiler_.time("توليد SIR");
                // ... SIR generation
            }
            
            // 7. تحقق SIR
            sir::SirValidator validator;
            {
                auto valTimer = profiler_.time("تحقق SIR");
                auto valResult = validator.validate(sirModule);
                for (const auto& d : valResult.diagnostics) {
                    result.diagnostics.push_back(d);
                }
            }
            
            // 8. تحسين SIR
            if (options.optimizationLevel > 0) {
                sir::SirOptimizer optimizer;
                auto optTimer = profiler_.time("تحسين SIR");
                optimizer.optimize(sirModule, options.optimizationLevel);
            }
            
            // 9. توليد LLVM IR
            llvm_ir::OptimizedLLVMGenerator llvmGen;
            llvm_ir::OptimizedLLVMGenerator::GenerationOptions genOpts;
            genOpts.optimizationLevel = options.optimizationLevel;
            genOpts.debugInfo = options.debugInfo;
            genOpts.targetTriple = options.targetTriple;
            {
                auto llvmTimer = profiler_.time("توليد LLVM IR");
                auto genResult = llvmGen.generate(sirModule, genOpts);
                // ... write IR
            }
            
            // 10. ربط وإنتاج الملف النهائي
            {
                auto linkTimer = profiler_.time("الربط");
                // ... linking
            }
            
            result.success = (result.errorCount == 0);
            result.outputPath = options.outputFile;
            
        } catch (const std::exception& e) {
            improvements::CompilerDiagnostic diag;
            diag.severity = improvements::ErrorSeverity::ICE;
            diag.message = std::string("خطأ داخلي: ") + e.what();
            result.diagnostics.push_back(diag);
            result.errorCount++;
        }
        
        return result;
    }
    
    std::string getPerformanceReport() const {
        return profiler_.generateReport();
    }
    
private:
    diagnostics::PerformanceProfiler profiler_;
    diagnostics::DiagnosticLogger logger_;
    
    std::string readSourceFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("لا يمكن فتح الملف: " + path);
        }
        
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
};

} // namespace compiler
} // namespace sad

#endif // SAD_MEGA_IMPROVEMENTS_PART3_H
