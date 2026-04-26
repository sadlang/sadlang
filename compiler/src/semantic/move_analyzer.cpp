// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file move_analyzer.cpp
 * @brief تنفيذ محلل دلالات النقل / Move Semantics Analyzer Implementation
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 * 
 * =============================================================================
 *                    تفاصيل التنفيذ / Implementation Details
 * =============================================================================
 * 
 * (AR) هذا الملف يُنفِّذ محلل دلالات النقل. التنفيذ يتضمن:
 * 
 *      1. **خريطة المتغيرات**: تخزن حالة كل متغير (منقول/صالح/جزئي)
 *      2. **مكدس النطاقات**: يتتبع المتغيرات في كل نطاق
 *      3. **تتبع الحلقات**: يكشف النقل غير الآمن داخل الحلقات
 *      4. **تحليل الإغلاقات**: يتتبع الالتقاط بالنقل
 * 
 * (EN) This file implements the move semantics analyzer. Implementation includes:
 * 
 *      1. **Variable map**: Stores state of each variable (moved/valid/partial)
 *      2. **Scope stack**: Tracks variables in each scope
 *      3. **Loop tracking**: Detects unsafe moves inside loops
 *      4. **Closure analysis**: Tracks move captures
 */

#include "semantic/move_analyzer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Sad {
namespace Semantic {

// ============================================================================
//                    البناء والهدم / Construction & Destruction
// ============================================================================

MoveAnalyzer::MoveAnalyzer() {
    // (AR) بدء بنطاق عام فارغ
    // (EN) Start with empty global scope
    scopeStack_.push_back({});
}

MoveAnalyzer::~MoveAnalyzer() = default;

// ============================================================================
//                    إدارة المتغيرات / Variable Management
// ============================================================================

void MoveAnalyzer::declareVariable(const std::string& name,
                                   const std::string& typeName,
                                   const SourceLocation& location,
                                   bool isCopyType) {
    VariableInfo info;
    info.name = name;
    info.typeName = typeName;
    info.definitionLocation = location;
    info.isCopyType = isCopyType;
    info.isMoved = false;
    info.isPartiallyMoved = false;
    
    variables_[name] = info;
    
    // (AR) إضافة للنطاق الحالي
    // (EN) Add to current scope
    if (!scopeStack_.empty()) {
        scopeStack_.back().push_back(name);
    }
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Declared variable '" << name 
                  << "' of type '" << typeName << "'"
                  << (isCopyType ? " (Copy)" : " (Move)") << "\n";
    }
}

void MoveAnalyzer::reassignVariable(const std::string& name,
                                    const SourceLocation& location) {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        // (AR) إعادة تعيين المتغير يُحييه
        // (EN) Reassigning the variable revives it
        it->second.isMoved = false;
        it->second.isPartiallyMoved = false;
        it->second.moveInfo.reset();
        it->second.movedFields.clear();
        
        if (debugMode_) {
            std::cout << "[MoveAnalyzer] Reassigned variable '" << name 
                      << "' at " << location.toString() << " - now valid\n";
        }
    }
}

// ============================================================================
//                    تسجيل النقل / Move Recording
// ============================================================================

MoveCheckResult MoveAnalyzer::recordMove(const std::string& name,
                                         MoveKind kind,
                                         const SourceLocation& location,
                                         const std::string& destination) {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        // (AR) متغير غير معروف - قد يكون من نطاق خارجي
        // (EN) Unknown variable - may be from outer scope
        return MoveCheckResult::CanMove;
    }
    
    VariableInfo& info = it->second;
    
    // (AR) التحقق من النوع القابل للنسخ
    // (EN) Check for Copy type
    if (info.isCopyType) {
        if (debugMode_) {
            std::cout << "[MoveAnalyzer] '" << name 
                      << "' is Copy type - no move needed\n";
        }
        return MoveCheckResult::IsCopyType;
    }
    
    // (AR) التحقق من أنه لم يُنقل مسبقاً
    // (EN) Check not already moved
    if (info.isMoved) {
        recordError(MoveError::Kind::DoubleMove, name, location,
                   info.moveInfo ? info.moveInfo->moveLocation : SourceLocation());
        return MoveCheckResult::AlreadyMoved;
    }
    
    // (AR) التحقق من النقل داخل حلقة
    // (EN) Check move inside loop
    if (loopDepth_ > 0) {
        // (AR) النقل داخل حلقة قد يكون خطيراً
        // (EN) Move inside loop may be dangerous
        recordError(MoveError::Kind::MoveInLoop, name, location, SourceLocation());
        // (AR) نسمح به مع تحذير
        // (EN) We allow it with warning
    }
    
    // (AR) تسجيل النقل
    // (EN) Record the move
    MoveInfo moveInfo;
    moveInfo.variableName = name;
    moveInfo.typeName = info.typeName;
    moveInfo.kind = kind;
    moveInfo.moveLocation = location;
    moveInfo.definitionLocation = info.definitionLocation;
    moveInfo.destination = destination;
    moveInfo.isPartialMove = false;
    
    info.isMoved = true;
    info.moveInfo = moveInfo;
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] " 
                  << (useArabicMessages_ ? moveInfo.getArabicDescription() 
                                        : moveInfo.getEnglishDescription())
                  << "\n";
    }
    
    return MoveCheckResult::CanMove;
}

MoveCheckResult MoveAnalyzer::recordPartialMove(const std::string& structName,
                                                const std::string& fieldPath,
                                                const SourceLocation& location,
                                                const std::string& destination) {
    auto it = variables_.find(structName);
    if (it == variables_.end()) {
        return MoveCheckResult::CanMove;
    }
    
    VariableInfo& info = it->second;
    
    // (AR) التحقق من أن الهيكل بالكامل لم يُنقل
    // (EN) Check struct wasn't fully moved
    if (info.isMoved) {
        recordError(MoveError::Kind::UseAfterMove, structName, location,
                   info.moveInfo ? info.moveInfo->moveLocation : SourceLocation());
        return MoveCheckResult::AlreadyMoved;
    }
    
    // (AR) التحقق من أن الحقل لم يُنقل مسبقاً
    // (EN) Check field wasn't already moved
    if (info.movedFields.count(fieldPath) > 0) {
        recordError(MoveError::Kind::PartialMoveConflict, 
                   structName + "." + fieldPath, location, SourceLocation());
        return MoveCheckResult::PartiallyMoved;
    }
    
    // (AR) تسجيل النقل الجزئي
    // (EN) Record partial move
    info.movedFields.insert(fieldPath);
    info.isPartiallyMoved = true;
    
    MoveInfo moveInfo;
    moveInfo.variableName = structName;
    moveInfo.typeName = info.typeName;
    moveInfo.kind = MoveKind::FieldAccess;
    moveInfo.moveLocation = location;
    moveInfo.definitionLocation = info.definitionLocation;
    moveInfo.destination = destination;
    moveInfo.isPartialMove = true;
    moveInfo.fieldPath = fieldPath;
    
    if (!info.moveInfo) {
        info.moveInfo = moveInfo;
    }
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Partial move: " << structName 
                  << "." << fieldPath << "\n";
    }
    
    return MoveCheckResult::CanMove;
}

// ============================================================================
//                    الاستعلام / Querying
// ============================================================================

bool MoveAnalyzer::isVariableMoved(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return false;
    }
    return it->second.isMoved;
}

bool MoveAnalyzer::isPartiallyMoved(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return false;
    }
    return it->second.isPartiallyMoved;
}

std::optional<MoveInfo> MoveAnalyzer::getMoveInfo(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return std::nullopt;
    }
    return it->second.moveInfo;
}

std::vector<std::string> MoveAnalyzer::getMovedFields(const std::string& structName) const {
    auto it = variables_.find(structName);
    if (it == variables_.end()) {
        return {};
    }
    return std::vector<std::string>(it->second.movedFields.begin(),
                                    it->second.movedFields.end());
}

std::optional<MoveError> MoveAnalyzer::checkUse(const std::string& name,
                                                const SourceLocation& location) {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return std::nullopt;
    }
    
    const VariableInfo& info = it->second;
    
    if (info.isMoved) {
        MoveError error;
        error.kind = MoveError::Kind::UseAfterMove;
        error.variableName = name;
        error.useLocation = location;
        error.moveLocation = info.moveInfo ? info.moveInfo->moveLocation 
                                           : SourceLocation();
        error.arabicMessage = getArabicErrorMessage(MoveError::Kind::UseAfterMove, name);
        error.message = getEnglishErrorMessage(MoveError::Kind::UseAfterMove, name);
        error.suggestion = getSuggestion(MoveError::Kind::UseAfterMove, name);
        
        errors_.push_back(error);
        return error;
    }
    
    return std::nullopt;
}

// ============================================================================
//                    إدارة النطاقات / Scope Management
// ============================================================================

void MoveAnalyzer::enterScope() {
    scopeStack_.push_back({});
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Entered scope (depth: " 
                  << scopeStack_.size() << ")\n";
    }
}

void MoveAnalyzer::exitScope() {
    if (scopeStack_.empty()) {
        return;
    }
    
    // (AR) إزالة المتغيرات من هذا النطاق
    // (EN) Remove variables from this scope
    for (const auto& varName : scopeStack_.back()) {
        variables_.erase(varName);
    }
    
    scopeStack_.pop_back();
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Exited scope (depth: " 
                  << scopeStack_.size() << ")\n";
    }
}

void MoveAnalyzer::enterLoopScope() {
    enterScope();
    loopDepth_++;
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Entered loop (depth: " 
                  << loopDepth_ << ")\n";
    }
}

void MoveAnalyzer::exitLoopScope() {
    exitScope();
    if (loopDepth_ > 0) {
        loopDepth_--;
    }
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Exited loop (depth: " 
                  << loopDepth_ << ")\n";
    }
}

// ============================================================================
//                    تحليل الإغلاقات / Closure Analysis
// ============================================================================

void MoveAnalyzer::beginClosureAnalysis(const std::vector<std::string>& capturedVariables,
                                        bool moveCapture) {
    inClosure_ = true;
    capturedVariables_.clear();
    
    for (const auto& var : capturedVariables) {
        capturedVariables_.insert(var);
        
        if (moveCapture) {
            // (AR) الالتقاط بالنقل يعني نقل المتغير
            // (EN) Move capture means moving the variable
            recordMove(var, MoveKind::ClosureCapture, SourceLocation(), "closure");
        }
    }
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] Begin closure analysis with " 
                  << capturedVariables.size() << " captured variables"
                  << (moveCapture ? " (move capture)" : " (borrow capture)")
                  << "\n";
    }
}

void MoveAnalyzer::endClosureAnalysis() {
    inClosure_ = false;
    capturedVariables_.clear();
    
    if (debugMode_) {
        std::cout << "[MoveAnalyzer] End closure analysis\n";
    }
}

// ============================================================================
//                    تنقيح / Debugging
// ============================================================================

void MoveAnalyzer::dump() const {
    std::cout << "\n======= MoveAnalyzer State =======\n";
    std::cout << "Variables: " << variables_.size() << "\n";
    std::cout << "Scope depth: " << scopeStack_.size() << "\n";
    std::cout << "Loop depth: " << loopDepth_ << "\n";
    std::cout << "In closure: " << (inClosure_ ? "yes" : "no") << "\n";
    std::cout << "Errors: " << errors_.size() << "\n\n";
    
    for (const auto& [name, info] : variables_) {
        std::cout << "  " << name << " : " << info.typeName;
        if (info.isCopyType) std::cout << " [Copy]";
        if (info.isMoved) std::cout << " [MOVED]";
        if (info.isPartiallyMoved) {
            std::cout << " [PARTIAL: ";
            for (const auto& field : info.movedFields) {
                std::cout << field << " ";
            }
            std::cout << "]";
        }
        std::cout << "\n";
    }
    
    std::cout << "==================================\n\n";
}

// ============================================================================
//                    دوال مساعدة / Helper Functions
// ============================================================================

void MoveAnalyzer::recordError(MoveError::Kind kind,
                               const std::string& varName,
                               const SourceLocation& useLocation,
                               const SourceLocation& moveLocation) {
    MoveError error;
    error.kind = kind;
    error.variableName = varName;
    error.useLocation = useLocation;
    error.moveLocation = moveLocation;
    error.arabicMessage = getArabicErrorMessage(kind, varName);
    error.message = getEnglishErrorMessage(kind, varName);
    error.suggestion = getSuggestion(kind, varName);
    
    errors_.push_back(error);
    
    if (debugMode_) {
        std::cerr << (useArabicMessages_ ? error.toArabicString() 
                                         : error.toEnglishString());
    }
}

std::string MoveAnalyzer::getArabicErrorMessage(MoveError::Kind kind,
                                                const std::string& varName) const {
    switch (kind) {
        case MoveError::Kind::UseAfterMove:
            return "استخدام المتغير '" + varName + "' بعد نقله";
            
        case MoveError::Kind::MoveOfBorrowed:
            return "محاولة نقل المتغير '" + varName + "' وهو مستعار";
            
        case MoveError::Kind::PartialMoveConflict:
            return "تعارض في النقل الجزئي للحقل '" + varName + "'";
            
        case MoveError::Kind::DoubleMove:
            return "محاولة نقل المتغير '" + varName + "' مرة ثانية";
            
        case MoveError::Kind::MoveInLoop:
            return "نقل المتغير '" + varName + "' داخل حلقة قد يكون غير آمن";
            
        case MoveError::Kind::CapturedMoved:
            return "المتغير '" + varName + "' ملتقط ومنقول في نفس الوقت";
            
        default:
            return "خطأ نقل غير محدد للمتغير '" + varName + "'";
    }
}

std::string MoveAnalyzer::getEnglishErrorMessage(MoveError::Kind kind,
                                                 const std::string& varName) const {
    switch (kind) {
        case MoveError::Kind::UseAfterMove:
            return "use of moved variable '" + varName + "'";
            
        case MoveError::Kind::MoveOfBorrowed:
            return "cannot move '" + varName + "' while borrowed";
            
        case MoveError::Kind::PartialMoveConflict:
            return "partial move conflict for field '" + varName + "'";
            
        case MoveError::Kind::DoubleMove:
            return "variable '" + varName + "' moved twice";
            
        case MoveError::Kind::MoveInLoop:
            return "move of '" + varName + "' inside loop may be unsafe";
            
        case MoveError::Kind::CapturedMoved:
            return "variable '" + varName + "' is captured and moved";
            
        default:
            return "undefined move error for '" + varName + "'";
    }
}

std::string MoveAnalyzer::getSuggestion(MoveError::Kind kind,
                                        const std::string& varName) const {
    switch (kind) {
        case MoveError::Kind::UseAfterMove:
            if (useArabicMessages_) {
                return "استخدم `.استنسخ()` للحصول على نسخة، أو أعد تعيين المتغير";
            }
            return "use `.clone()` to get a copy, or reassign the variable";
            
        case MoveError::Kind::MoveOfBorrowed:
            if (useArabicMessages_) {
                return "انتظر حتى تنتهي الاستعارة قبل النقل";
            }
            return "wait for borrow to end before moving";
            
        case MoveError::Kind::PartialMoveConflict:
            if (useArabicMessages_) {
                return "تأكد من عدم نقل الحقل مسبقاً";
            }
            return "ensure field wasn't already moved";
            
        case MoveError::Kind::DoubleMove:
            if (useArabicMessages_) {
                return "المتغير نُقل مسبقاً - لا يمكن نقله مرة أخرى";
            }
            return "variable was already moved - cannot move again";
            
        case MoveError::Kind::MoveInLoop:
            if (useArabicMessages_) {
                return "استخدم `.استنسخ()` داخل الحلقة، أو انقل قبل الحلقة";
            }
            return "use `.clone()` inside loop, or move before loop";
            
        case MoveError::Kind::CapturedMoved:
            if (useArabicMessages_) {
                return "اختر إما الالتقاط أو النقل، وليس كليهما";
            }
            return "choose either capture or move, not both";
            
        default:
            return "";
    }
}

} // namespace Semantic
} // namespace Sad
