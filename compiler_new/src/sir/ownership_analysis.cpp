/**
 * =============================================================================
 * ملف: ownership_analysis.cpp
 * الوصف: تحليل الملكية في SIR
 * المهمة: T263 - SIR ownership analysis
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 * 
 * 🔍 دليل المبتدئ لتحليل الملكية
 * ═══════════════════════════════════
 * 
 * ما هو تحليل الملكية؟
 * ────────────────────
 * نتتبع من يملك كل قيمة ومن يستعيرها.
 * 
 * القواعد الأساسية:
 * ─────────────────
 * 1. كل قيمة لها مالك واحد فقط
 * 2. عند النقل، المالك القديم يفقد الملكية
 * 3. الاستعارة لا تنقل الملكية
 * 4. لا يمكن استخدام قيمة بعد نقلها
 * 
 * مثال التحليل:
 * ─────────────
 * ```
 * متغير س = 10           ; س: مملوك
 * متغير ص = &س           ; س: مملوك، ص: مستعار من س
 * متغير ع = انقل س       ; س: منقول (غير صالح)، ع: مملوك
 * اطبع(س)                ; ❌ خطأ! س منقولة
 * ```
 * 
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>

namespace sad::sir {

// =============================================================================
// نتائج التحليل
// =============================================================================

/**
 * نوع الخطأ
 */
enum class OwnershipErrorKind {
    UseAfterMove,           // استخدام بعد النقل
    UseAfterDrop,           // استخدام بعد الإسقاط
    DoubleFree,             // تحرير مزدوج
    DoubleMove,             // نقل مزدوج
    BorrowOfMoved,          // استعارة من قيمة منقولة
    MutBorrowWhileBorrowed, // استعارة متغيرة أثناء وجود استعارات
    UseWhileMutBorrowed,    // استخدام أثناء استعارة متغيرة
    InvalidBorrow,          // استعارة غير صالحة
    MoveWhileBorrowed,      // نقل أثناء الاستعارة
    UninitializedUse        // استخدام قبل التهيئة
};

/**
 * خطأ ملكية
 */
struct OwnershipError {
    OwnershipErrorKind kind;
    ValueId value;
    SourceLocation errorLocation;
    std::optional<SourceLocation> relatedLocation;  // مثل: موقع النقل السابق
    std::string message;
    
    std::string toString() const {
        std::string kindStr;
        switch (kind) {
            case OwnershipErrorKind::UseAfterMove:
                kindStr = "استخدام بعد النقل";
                break;
            case OwnershipErrorKind::UseAfterDrop:
                kindStr = "استخدام بعد الإسقاط";
                break;
            case OwnershipErrorKind::DoubleFree:
                kindStr = "تحرير مزدوج";
                break;
            case OwnershipErrorKind::DoubleMove:
                kindStr = "نقل مزدوج";
                break;
            case OwnershipErrorKind::BorrowOfMoved:
                kindStr = "استعارة من قيمة منقولة";
                break;
            case OwnershipErrorKind::MutBorrowWhileBorrowed:
                kindStr = "استعارة متغيرة أثناء وجود استعارات";
                break;
            case OwnershipErrorKind::UseWhileMutBorrowed:
                kindStr = "استخدام أثناء استعارة متغيرة";
                break;
            case OwnershipErrorKind::InvalidBorrow:
                kindStr = "استعارة غير صالحة";
                break;
            case OwnershipErrorKind::MoveWhileBorrowed:
                kindStr = "نقل أثناء الاستعارة";
                break;
            case OwnershipErrorKind::UninitializedUse:
                kindStr = "استخدام قبل التهيئة";
                break;
        }
        
        return "خطأ [" + kindStr + "]: " + message + 
               " في " + errorLocation.toString();
    }
};

/**
 * حالة قيمة أثناء التحليل
 */
struct ValueState {
    OwnershipState state = OwnershipState::Uninitialized;
    std::set<ValueId> borrowedBy;           // من يستعير منها
    std::optional<ValueId> mutBorrowedBy;   // من يستعير متغيراً
    std::optional<SourceLocation> movedAt;
    std::optional<SourceLocation> droppedAt;
    
    bool isValid() const {
        return state == OwnershipState::Owned || 
               state == OwnershipState::Borrowed ||
               state == OwnershipState::MutBorrowed;
    }
    
    bool hasBorrows() const {
        return !borrowedBy.empty() || mutBorrowedBy.has_value();
    }
};

// =============================================================================
// محلل الملكية
// =============================================================================

/**
 * محلل الملكية
 */
class OwnershipAnalyzer {
public:
    /**
     * تحليل دالة
     */
    std::vector<OwnershipError> analyze(const SirFunction& func) {
        errors_.clear();
        valueStates_.clear();
        
        // تحليل كل كتلة
        for (const auto& block : func.blocks) {
            analyzeBlock(block);
        }
        
        return errors_;
    }
    
    /**
     * تحليل كتلة
     */
    void analyzeBlock(const SirBasicBlock& block) {
        for (const auto& inst : block.instructions) {
            analyzeInstruction(inst);
        }
    }
    
    /**
     * تحليل تعليمة
     */
    void analyzeInstruction(const SirInstruction& inst) {
        switch (inst.opcode) {
            case Opcode::Alloc:
                handleAlloc(inst);
                break;
                
            case Opcode::Borrow:
                handleBorrow(inst, false);
                break;
                
            case Opcode::BorrowMut:
                handleBorrow(inst, true);
                break;
                
            case Opcode::Move:
                handleMove(inst);
                break;
                
            case Opcode::Copy:
                handleCopy(inst);
                break;
                
            case Opcode::Drop:
                handleDrop(inst);
                break;
                
            case Opcode::EndBorrow:
                handleEndBorrow(inst);
                break;
                
            case Opcode::Load:
            case Opcode::Store:
                handleMemoryAccess(inst);
                break;
                
            default:
                // التحقق من استخدام القيم
                checkOperandValidity(inst);
                break;
        }
    }
    
private:
    // معالجة التخصيص
    void handleAlloc(const SirInstruction& inst) {
        if (inst.result) {
            valueStates_[*inst.result] = {OwnershipState::Owned};
        }
    }
    
    // معالجة الاستعارة
    void handleBorrow(const SirInstruction& inst, bool isMut) {
        if (!inst.result || inst.operands.empty()) return;
        
        ValueId resultId = *inst.result;
        ValueId sourceId = std::get<ValueOperand>(inst.operands[0]).id;
        
        // التحقق من صلاحية المصدر
        auto& sourceState = valueStates_[sourceId];
        
        if (sourceState.state == OwnershipState::Moved) {
            errors_.push_back({
                OwnershipErrorKind::BorrowOfMoved,
                sourceId,
                inst.location,
                sourceState.movedAt,
                "محاولة استعارة من قيمة منقولة"
            });
            return;
        }
        
        if (sourceState.state == OwnershipState::Dropped) {
            errors_.push_back({
                OwnershipErrorKind::UseAfterDrop,
                sourceId,
                inst.location,
                sourceState.droppedAt,
                "محاولة استعارة من قيمة مُسقطة"
            });
            return;
        }
        
        if (isMut) {
            // استعارة متغيرة
            if (sourceState.hasBorrows()) {
                errors_.push_back({
                    OwnershipErrorKind::MutBorrowWhileBorrowed,
                    sourceId,
                    inst.location,
                    std::nullopt,
                    "لا يمكن الاستعارة المتغيرة أثناء وجود استعارات أخرى"
                });
                return;
            }
            
            sourceState.mutBorrowedBy = resultId;
            sourceState.state = OwnershipState::MutBorrowed;
            valueStates_[resultId] = {OwnershipState::MutBorrowed};
        } else {
            // استعارة ثابتة
            if (sourceState.mutBorrowedBy) {
                errors_.push_back({
                    OwnershipErrorKind::UseWhileMutBorrowed,
                    sourceId,
                    inst.location,
                    std::nullopt,
                    "لا يمكن الاستعارة أثناء استعارة متغيرة نشطة"
                });
                return;
            }
            
            sourceState.borrowedBy.insert(resultId);
            sourceState.state = OwnershipState::Borrowed;
            valueStates_[resultId] = {OwnershipState::Borrowed};
        }
    }
    
    // معالجة النقل
    void handleMove(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        ValueId resultId = *inst.result;
        ValueId sourceId = std::get<ValueOperand>(inst.operands[0]).id;
        
        auto& sourceState = valueStates_[sourceId];
        
        // التحقق من صلاحية المصدر
        if (sourceState.state == OwnershipState::Moved) {
            errors_.push_back({
                OwnershipErrorKind::DoubleMove,
                sourceId,
                inst.location,
                sourceState.movedAt,
                "محاولة نقل قيمة تم نقلها مسبقاً"
            });
            return;
        }
        
        if (sourceState.state == OwnershipState::Dropped) {
            errors_.push_back({
                OwnershipErrorKind::UseAfterDrop,
                sourceId,
                inst.location,
                sourceState.droppedAt,
                "محاولة نقل قيمة مُسقطة"
            });
            return;
        }
        
        // التحقق من الاستعارات
        if (sourceState.hasBorrows()) {
            errors_.push_back({
                OwnershipErrorKind::MoveWhileBorrowed,
                sourceId,
                inst.location,
                std::nullopt,
                "لا يمكن نقل قيمة أثناء وجود استعارات نشطة"
            });
            return;
        }
        
        // تنفيذ النقل
        sourceState.state = OwnershipState::Moved;
        sourceState.movedAt = inst.location;
        
        valueStates_[resultId] = {OwnershipState::Owned};
    }
    
    // معالجة النسخ
    void handleCopy(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        ValueId sourceId = std::get<ValueOperand>(inst.operands[0]).id;
        
        // التحقق من صلاحية المصدر
        checkValueValid(sourceId, inst.location, "نسخ");
        
        // النسخ لا يؤثر على المصدر
        valueStates_[*inst.result] = {OwnershipState::Owned};
    }
    
    // معالجة الإسقاط
    void handleDrop(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        ValueId valueId = std::get<ValueOperand>(inst.operands[0]).id;
        auto& state = valueStates_[valueId];
        
        // التحقق من عدم الإسقاط المزدوج
        if (state.state == OwnershipState::Dropped) {
            errors_.push_back({
                OwnershipErrorKind::DoubleFree,
                valueId,
                inst.location,
                state.droppedAt,
                "محاولة إسقاط قيمة تم إسقاطها مسبقاً"
            });
            return;
        }
        
        if (state.state == OwnershipState::Moved) {
            // لا بأس، القيمة منقولة فلا حاجة للإسقاط
            return;
        }
        
        // التحقق من الاستعارات
        if (state.hasBorrows()) {
            errors_.push_back({
                OwnershipErrorKind::MoveWhileBorrowed,
                valueId,
                inst.location,
                std::nullopt,
                "لا يمكن إسقاط قيمة أثناء وجود استعارات نشطة"
            });
            return;
        }
        
        // تنفيذ الإسقاط
        state.state = OwnershipState::Dropped;
        state.droppedAt = inst.location;
    }
    
    // معالجة إنهاء الاستعارة
    void handleEndBorrow(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        ValueId borrowId = std::get<ValueOperand>(inst.operands[0]).id;
        
        // إزالة الاستعارة من المصدر
        for (auto& [id, state] : valueStates_) {
            state.borrowedBy.erase(borrowId);
            if (state.mutBorrowedBy == borrowId) {
                state.mutBorrowedBy = std::nullopt;
                state.state = OwnershipState::Owned;
            }
        }
        
        // تحديث حالة الاستعارة
        valueStates_[borrowId].state = OwnershipState::Dropped;
    }
    
    // معالجة الوصول للذاكرة
    void handleMemoryAccess(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        ValueId ptrId = std::get<ValueOperand>(inst.operands[0]).id;
        checkValueValid(ptrId, inst.location, 
                       inst.opcode == Opcode::Load ? "قراءة" : "كتابة");
        
        // للتخزين، نتحقق من القيمة أيضاً
        if (inst.opcode == Opcode::Store && inst.operands.size() > 1) {
            ValueId valId = std::get<ValueOperand>(inst.operands[1]).id;
            checkValueValid(valId, inst.location, "تخزين");
        }
    }
    
    // التحقق من صلاحية المعاملات
    void checkOperandValidity(const SirInstruction& inst) {
        for (const auto& op : inst.operands) {
            if (auto* valOp = std::get_if<ValueOperand>(&op)) {
                checkValueValid(valOp->id, inst.location, "استخدام");
            }
        }
    }
    
    // التحقق من صلاحية قيمة
    void checkValueValid(ValueId id, const SourceLocation& loc, 
                         const std::string& operation) {
        auto it = valueStates_.find(id);
        if (it == valueStates_.end()) {
            errors_.push_back({
                OwnershipErrorKind::UninitializedUse,
                id,
                loc,
                std::nullopt,
                "استخدام قيمة غير مُهيأة"
            });
            return;
        }
        
        auto& state = it->second;
        
        if (state.state == OwnershipState::Moved) {
            errors_.push_back({
                OwnershipErrorKind::UseAfterMove,
                id,
                loc,
                state.movedAt,
                "محاولة " + operation + " قيمة تم نقلها"
            });
        } else if (state.state == OwnershipState::Dropped) {
            errors_.push_back({
                OwnershipErrorKind::UseAfterDrop,
                id,
                loc,
                state.droppedAt,
                "محاولة " + operation + " قيمة تم إسقاطها"
            });
        } else if (state.state == OwnershipState::Uninitialized) {
            errors_.push_back({
                OwnershipErrorKind::UninitializedUse,
                id,
                loc,
                std::nullopt,
                "استخدام قيمة قبل تهيئتها"
            });
        }
    }
    
    // الحالة الداخلية
    std::map<ValueId, ValueState> valueStates_;
    std::vector<OwnershipError> errors_;
};

// =============================================================================
// تقرير التحليل
// =============================================================================

/**
 * تقرير تحليل الملكية
 */
class OwnershipReport {
public:
    void addErrors(const std::vector<OwnershipError>& errors) {
        errors_.insert(errors_.end(), errors.begin(), errors.end());
    }
    
    bool hasErrors() const {
        return !errors_.empty();
    }
    
    size_t errorCount() const {
        return errors_.size();
    }
    
    std::string toString() const {
        if (errors_.empty()) {
            return "✅ لا توجد أخطاء ملكية\n";
        }
        
        std::string report = "❌ تم العثور على " + 
                            std::to_string(errors_.size()) + 
                            " خطأ ملكية:\n\n";
        
        for (size_t i = 0; i < errors_.size(); i++) {
            report += std::to_string(i + 1) + ". " + 
                      errors_[i].toString() + "\n";
        }
        
        return report;
    }
    
    const std::vector<OwnershipError>& getErrors() const {
        return errors_;
    }
    
private:
    std::vector<OwnershipError> errors_;
};

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadOwnershipAnalyzer {
    sad::sir::OwnershipAnalyzer* analyzer;
    sad::sir::OwnershipReport* report;
};

SadOwnershipAnalyzer* sad_ownership_analyzer_new() {
    auto* ctx = new SadOwnershipAnalyzer();
    ctx->analyzer = new sad::sir::OwnershipAnalyzer();
    ctx->report = new sad::sir::OwnershipReport();
    return ctx;
}

int sad_ownership_analyze(SadOwnershipAnalyzer* ctx, 
                          void* function /* SirFunction* */) {
    if (!ctx || !function) return -1;
    
    auto* func = static_cast<sad::sir::SirFunction*>(function);
    auto errors = ctx->analyzer->analyze(*func);
    ctx->report->addErrors(errors);
    
    return errors.size();
}

const char* sad_ownership_report(SadOwnershipAnalyzer* ctx) {
    if (!ctx || !ctx->report) return "";
    thread_local std::string report;
    report = ctx->report->toString();
    return report.c_str();
}

int sad_ownership_has_errors(SadOwnershipAnalyzer* ctx) {
    if (!ctx || !ctx->report) return 0;
    return ctx->report->hasErrors() ? 1 : 0;
}

void sad_ownership_analyzer_free(SadOwnershipAnalyzer* ctx) {
    if (ctx) {
        delete ctx->analyzer;
        delete ctx->report;
        delete ctx;
    }
}

} // extern "C"
