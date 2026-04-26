// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file lifetime_analyzer.cpp
 * @brief تنفيذ محلل الأعمار / Lifetime Analyzer Implementation
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 */

#include "semantic/lifetime_analyzer.h"
#include <iostream>
#include <algorithm>

namespace Sad {
namespace Semantic {

// ============================================================================
// بناء وهدم / Construction & Destruction
// ============================================================================

LifetimeAnalyzer::LifetimeAnalyzer()
    : nextLifetimeId_(1)
    , staticLifetime_(0, "static")
    , nllEnabled_(true)
    , nextScopeId_(0) {
    
    // (AR) إنشاء النطاق العالمي
    // (EN) Create global scope
    scopeStack_.emplace_back(0, 0);
}

LifetimeAnalyzer::~LifetimeAnalyzer() = default;

// ============================================================================
// إدارة الأعمار / Lifetime Management
// ============================================================================

LifetimeId LifetimeAnalyzer::createLifetime(const std::string& name) {
    LifetimeId id(nextLifetimeId_++, name);
    return id;
}

void LifetimeAnalyzer::bindLifetimeToVariable(const std::string& varName, 
                                               const LifetimeId& lifetime) {
    variableLifetimes_[varName] = lifetime;
    
    // (AR) إضافة المتغير للنطاق الحالي
    // (EN) Add variable to current scope
    if (!scopeStack_.empty()) {
        scopeStack_.back().variables.push_back(varName);
    }
}

std::optional<LifetimeId> LifetimeAnalyzer::getVariableLifetime(
    const std::string& varName) const {
    
    auto it = variableLifetimes_.find(varName);
    if (it != variableLifetimes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

// ============================================================================
// إدارة النطاقات / Scope Management
// ============================================================================

size_t LifetimeAnalyzer::enterScope(size_t startLine) {
    size_t newScopeId = ++nextScopeId_;
    scopeStack_.emplace_back(newScopeId, startLine);
    return newScopeId;
}

void LifetimeAnalyzer::exitScope(size_t endLine) {
    if (scopeStack_.size() <= 1) {
        // (AR) لا يمكن الخروج من النطاق العالمي
        // (EN) Cannot exit global scope
        return;
    }
    
    LifetimeScope& exitingScope = scopeStack_.back();
    exitingScope.endLine = endLine;
    
    // (AR) إزالة المتغيرات من الخريطة
    // (EN) Remove variables from map
    for (const auto& var : exitingScope.variables) {
        variableLifetimes_.erase(var);
    }
    
    scopeStack_.pop_back();
}

const LifetimeScope& LifetimeAnalyzer::currentScope() const {
    return scopeStack_.back();
}

// ============================================================================
// قيود الأعمار / Lifetime Constraints
// ============================================================================

void LifetimeAnalyzer::addOutlivesConstraint(const LifetimeId& longer,
                                              const LifetimeId& shorter,
                                              const SourceLocation& loc) {
    constraints_.emplace_back(shorter, longer, true, loc);
}

std::vector<LifetimeError> LifetimeAnalyzer::verifyConstraints() {
    std::vector<LifetimeError> violations;
    
    for (const auto& constraint : constraints_) {
        LifetimeRelation rel = compare(constraint.lifetime, constraint.bound);
        
        if (constraint.mustOutlive && rel == LifetimeRelation::Shorter) {
            // (AR) انتهاك: العمر أقصر مما يجب
            // (EN) Violation: lifetime is shorter than required
            auto error = makeError(LifetimeErrorKind::LifetimeTooShort,
                                 constraint.location);
            violations.push_back(error);
            errors_.push_back(error);
        }
    }
    
    return violations;
}

// ============================================================================
// التحليل / Analysis
// ============================================================================

bool LifetimeAnalyzer::outlivesScope(const LifetimeId& lifetime, 
                                      size_t scopeId) const {
    // (AR) العمر الساكن يعيش أطول من أي نطاق
    // (EN) Static lifetime outlives any scope
    if (isStatic(lifetime)) {
        return true;
    }
    
    // (AR) البحث عن وجود النطاق الهدف
    // (EN) Ensure target scope exists
    auto targetIt = std::find_if(scopeStack_.begin(), scopeStack_.end(),
        [scopeId](const LifetimeScope& s) { return s.scopeId == scopeId; });
    if (targetIt == scopeStack_.end()) {
        return false;
    }

    // تقريب NLL: العمر الأقدم (id أصغر) يُعتبر أطول عمراً من النطاقات الأحدث.
    // NLL approximation: older lifetime ids outlive newer lexical scopes.
    if (lifetime.id <= scopeId + 1) {
        return true;
    }

    // إذا كان العمر مربوطاً بمتغير لا يزال معرفاً في نطاق أقدم أو مساوٍ للنطاق الهدف
    // نعتبره outlives لهذا النطاق.
    for (const auto& [varName, varLifetime] : variableLifetimes_) {
        if (!(varLifetime == lifetime)) {
            continue;
        }
        for (const auto& scope : scopeStack_) {
            if (scope.scopeId > scopeId) {
                continue;
            }
            if (std::find(scope.variables.begin(), scope.variables.end(), varName) != scope.variables.end()) {
                return true;
            }
        }
    }

    return false;
}

LifetimeRelation LifetimeAnalyzer::compare(const LifetimeId& a, 
                                           const LifetimeId& b) const {
    // (AR) العمر الساكن يعيش أطول من أي شيء
    // (EN) Static lifetime outlives anything
    if (isStatic(a) && !isStatic(b)) {
        return LifetimeRelation::Outlives;
    }
    if (!isStatic(a) && isStatic(b)) {
        return LifetimeRelation::Shorter;
    }
    if (isStatic(a) && isStatic(b)) {
        return LifetimeRelation::Equal;
    }
    
    // (AR) مقارنة معرفات الأعمار (الأقدم يعيش أطول)
    // (EN) Compare lifetime IDs (older outlives newer)
    if (a.id < b.id) {
        return LifetimeRelation::Outlives;
    } else if (a.id > b.id) {
        return LifetimeRelation::Shorter;
    }
    
    return LifetimeRelation::Equal;
}

bool LifetimeAnalyzer::isStatic(const LifetimeId& lifetime) const {
    return lifetime.id == 0 || lifetime.name == "static" || lifetime.name == "ساكن";
}

// ============================================================================
// Lifetime Elision / حذف العمر
// ============================================================================

std::vector<LifetimeId> LifetimeAnalyzer::inferFunctionLifetimes(
    const std::vector<std::string>& paramTypes,
    const std::string& returnType,
    bool hasSelf) {
    
    std::vector<LifetimeId> lifetimes;
    std::vector<LifetimeId> inputLifetimes;
    
    // (AR) القاعدة 1: كل معامل مرجع يحصل على عمر مستقل
    // (EN) Rule 1: Each reference parameter gets distinct lifetime
    for (const auto& paramType : paramTypes) {
        if (isReferenceType(paramType)) {
            LifetimeId lt = createLifetime();
            lifetimes.push_back(lt);
            inputLifetimes.push_back(lt);
        }
    }
    
    // (AR) التحقق من نوع الإرجاع
    // (EN) Check return type
    if (!isReferenceType(returnType)) {
        return lifetimes;  // (AR) لا يحتاج عمر / (EN) No lifetime needed
    }
    
    // (AR) القاعدة 2: إذا كان هناك معامل واحد، يُعطى للمخرجات
    // (EN) Rule 2: If one input, outputs get its lifetime
    if (inputLifetimes.size() == 1) {
        lifetimes.push_back(inputLifetimes[0]);
        return lifetimes;
    }
    
    // (AR) القاعدة 3: إذا كان هناك &self، يُعطى عمره للمخرجات
    // (EN) Rule 3: If &self exists, outputs get its lifetime
    if (hasSelf && !inputLifetimes.empty()) {
        lifetimes.push_back(inputLifetimes[0]);
        return lifetimes;
    }
    
    // (AR) لا يمكن الاستنتاج - يجب التصريح الصريح
    // (EN) Cannot infer - explicit annotation required
    auto error = makeError(LifetimeErrorKind::AmbiguousLifetime,
                          SourceLocation());
    errors_.push_back(error);
    
    return lifetimes;
}

// ============================================================================
// NLL (Non-Lexical Lifetimes) / الأعمار غير المعجمية
// ============================================================================

void LifetimeAnalyzer::analyzeLastUsePoints() {
    if (!nllEnabled_) {
        return;
    }

    // تطبيق خفيف لـ NLL بدون CFG كامل:
    // نحذف القيود المحققة مسبقاً لتقليل الإنذارات الكاذبة وتضييق نطاق الفحص.
    // Lightweight NLL pass without full CFG:
    // prune already-satisfied constraints to reduce false positives.
    std::vector<LifetimeBound> remaining;
    remaining.reserve(constraints_.size());

    for (const auto& c : constraints_) {
        if (!c.mustOutlive) {
            remaining.push_back(c);
            continue;
        }

        LifetimeRelation rel = compare(c.lifetime, c.bound);
        if (rel == LifetimeRelation::Outlives || rel == LifetimeRelation::Equal) {
            continue; // هذا القيد محقق بالفعل
        }

        remaining.push_back(c);
    }

    constraints_.swap(remaining);
}

// ============================================================================
// تنقيح / Debugging
// ============================================================================

void LifetimeAnalyzer::dump() const {
    std::cout << "=== معلومات الأعمار / Lifetime Information ===\n";
    std::cout << "NLL: " << (nllEnabled_ ? "مفعّل/enabled" : "معطّل/disabled") << "\n";
    std::cout << "النطاقات النشطة / Active scopes: " << scopeStack_.size() << "\n";
    std::cout << "القيود / Constraints: " << constraints_.size() << "\n";
    std::cout << "المتغيرات / Variables:\n";
    
    for (const auto& [name, lifetime] : variableLifetimes_) {
        std::cout << "  " << name << ": " << lifetime.toString() << "\n";
    }
    
    std::cout << "الأخطاء / Errors: " << errors_.size() << "\n";
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

LifetimeError LifetimeAnalyzer::makeError(LifetimeErrorKind kind,
                                          const SourceLocation& loc,
                                          const SourceLocation& relatedLoc) {
    LifetimeError error;
    error.kind = kind;
    error.location = loc;
    error.relatedLocation = relatedLoc;
    error.message = getEnglishMessage(kind);
    error.arabicMessage = getArabicMessage(kind);
    error.suggestion = getSuggestion(kind);
    return error;
}

bool LifetimeAnalyzer::isReferenceType(const std::string& type) const {
    // (AR) تحقق من أن النوع مرجع
    // (EN) Check if type is a reference
    if (type.empty()) return false;
    
    // &T, &متغير T, مرجع<T>
    return type[0] == '&' || 
           type.substr(0, 4) == "مرجع" ||
           type.find("&") != std::string::npos;
}

std::string LifetimeAnalyzer::getArabicMessage(LifetimeErrorKind kind) const {
    switch (kind) {
        case LifetimeErrorKind::DanglingReference:
            return "مرجع معلق: المرجع يشير إلى ذاكرة محررة";
        
        case LifetimeErrorKind::LifetimeTooShort:
            return "العمر قصير جداً: المرجع لا يعيش طويلاً بما يكفي";
        
        case LifetimeErrorKind::ReturnLocalReference:
            return "لا يمكن إرجاع مرجع لمتغير محلي";
        
        case LifetimeErrorKind::StoreLongLived:
            return "لا يمكن تخزين مرجع يعيش أطول من المالك";
        
        case LifetimeErrorKind::BoundViolation:
            return "انتهاك قيد العمر";
        
        case LifetimeErrorKind::AmbiguousLifetime:
            return "لا يمكن استنتاج العمر تلقائياً، يرجى التصريح الصريح";
        
        default:
            return "خطأ عمر غير معروف";
    }
}

std::string LifetimeAnalyzer::getEnglishMessage(LifetimeErrorKind kind) const {
    switch (kind) {
        case LifetimeErrorKind::DanglingReference:
            return "dangling reference: reference points to freed memory";
        
        case LifetimeErrorKind::LifetimeTooShort:
            return "lifetime too short: reference does not live long enough";
        
        case LifetimeErrorKind::ReturnLocalReference:
            return "cannot return reference to local variable";
        
        case LifetimeErrorKind::StoreLongLived:
            return "cannot store reference that outlives owner";
        
        case LifetimeErrorKind::BoundViolation:
            return "lifetime bound violation";
        
        case LifetimeErrorKind::AmbiguousLifetime:
            return "cannot infer lifetime, explicit annotation required";
        
        default:
            return "unknown lifetime error";
    }
}

std::string LifetimeAnalyzer::getSuggestion(LifetimeErrorKind kind) const {
    switch (kind) {
        case LifetimeErrorKind::DanglingReference:
            return "تأكد من أن المرجع لا يعيش أطول من القيمة المشار إليها / ensure reference doesn't outlive referent";
        
        case LifetimeErrorKind::LifetimeTooShort:
            return "استخدم عمراً أطول أو أعد هيكلة الكود / use longer lifetime or restructure code";
        
        case LifetimeErrorKind::ReturnLocalReference:
            return "أرجع قيمة بدلاً من مرجع، أو استخدم معامل مرجع / return value instead of reference, or use reference parameter";
        
        case LifetimeErrorKind::StoreLongLived:
            return "استخدم نوعاً ملكياً بدلاً من مرجع / use owned type instead of reference";
        
        case LifetimeErrorKind::BoundViolation:
            return "أضف قيود العمر الصحيحة / add correct lifetime bounds";
        
        case LifetimeErrorKind::AmbiguousLifetime:
            return "أضف تصريحاً صريحاً للعمر مثل <'أ> / add explicit lifetime annotation like <'a>";
        
        default:
            return "";
    }
}

} // namespace Semantic
} // namespace Sad
