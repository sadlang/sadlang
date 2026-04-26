/**
 * =============================================================================
 * ملف: sir_borrow_check.cpp
 * الوصف: فحص الاستعارات في SIR
 * المهمة: T264 - SIR borrow checking
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 * 
 * ✅ دليل المبتدئ لفحص الاستعارات
 * ════════════════════════════════════
 * 
 * ما هو فحص الاستعارات؟
 * ─────────────────────
 * نتأكد من أن الاستعارات تتبع القواعد:
 * 
 * قواعد الاستعارة:
 * ────────────────
 * 1. يمكن وجود عدة استعارات ثابتة معاً (&)
 * 2. أو استعارة متغيرة واحدة فقط (&متغير)
 * 3. لا يمكن الجمع بين الثابتة والمتغيرة
 * 4. الاستعارات لا تعيش أطول من المصدر
 * 
 * مثال صحيح:
 * ──────────
 * ```sad
 * متغير س = 10
 * متغير أ = &س      // ✅ استعارة ثابتة
 * متغير ب = &س      // ✅ استعارة ثابتة ثانية
 * ```
 * 
 * مثال خاطئ:
 * ──────────
 * ```sad
 * متغير س = 10
 * متغير أ = &س          // استعارة ثابتة
 * متغير ب = &متغير س    // ❌ خطأ! لا يمكن استعارة متغيرة
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
#include <algorithm>

namespace sad::sir {

// =============================================================================
// أنواع الأخطاء
// =============================================================================

/**
 * نوع خطأ الاستعارة
 */
enum class BorrowErrorKind {
    // تعارض الاستعارات
    SharedAndMut,               // استعارة ثابتة ومتغيرة معاً
    MultipleMut,                // أكثر من استعارة متغيرة
    
    // مشاكل دورة الحياة
    BorrowOutlivesSource,       // الاستعارة تعيش أطول من المصدر
    DanglingReference,          // مرجع معلق
    
    // استخدام غير صالح
    UseAfterMove,               // استخدام بعد النقل
    UseAfterFree,               // استخدام بعد التحرير
    
    // تعديل غير صالح
    MutateImmutable,            // تعديل قيمة ثابتة
    MutateThroughSharedRef      // تعديل عبر مرجع ثابت
};

/**
 * خطأ استعارة
 */
struct BorrowError {
    BorrowErrorKind kind;
    SourceLocation location;
    std::string message;
    std::vector<SourceLocation> relatedLocations;
    
    std::string kindName() const {
        switch (kind) {
            case BorrowErrorKind::SharedAndMut:
                return "تعارض استعارة ثابتة ومتغيرة";
            case BorrowErrorKind::MultipleMut:
                return "استعارات متغيرة متعددة";
            case BorrowErrorKind::BorrowOutlivesSource:
                return "استعارة تعيش أطول من المصدر";
            case BorrowErrorKind::DanglingReference:
                return "مرجع معلق";
            case BorrowErrorKind::UseAfterMove:
                return "استخدام بعد النقل";
            case BorrowErrorKind::UseAfterFree:
                return "استخدام بعد التحرير";
            case BorrowErrorKind::MutateImmutable:
                return "تعديل قيمة ثابتة";
            case BorrowErrorKind::MutateThroughSharedRef:
                return "تعديل عبر مرجع ثابت";
        }
        return "خطأ غير معروف";
    }
    
    std::string toString() const {
        std::string result = "خطأ [" + kindName() + "]: " + message;
        result += "\n  في: " + location.toString();
        
        for (const auto& rel : relatedLocations) {
            result += "\n  ذو صلة: " + rel.toString();
        }
        
        return result;
    }
};

// =============================================================================
// تتبع الاستعارات
// =============================================================================

/**
 * استعارة نشطة
 */
struct BorrowRecord {
    ValueId borrower;           // المُستعير
    ValueId lender;             // المُعير
    bool isMut;                 // متغيرة؟
    SourceLocation borrowedAt;
    int scopeDepth;             // عمق النطاق
};

/**
 * حالة قيمة
 */
struct BorrowState {
    bool isMoved = false;
    bool isDropped = false;
    std::vector<BorrowRecord> activeBorrows;
    
    bool hasSharedBorrow() const {
        return std::any_of(activeBorrows.begin(), activeBorrows.end(),
            [](const BorrowRecord& b) { return !b.isMut; });
    }
    
    bool hasMutBorrow() const {
        return std::any_of(activeBorrows.begin(), activeBorrows.end(),
            [](const BorrowRecord& b) { return b.isMut; });
    }
    
    int borrowCount() const {
        return activeBorrows.size();
    }
};

// =============================================================================
// فاحص الاستعارات
// =============================================================================

/**
 * فاحص الاستعارات
 */
class BorrowChecker {
public:
    /**
     * فحص دالة
     */
    std::vector<BorrowError> check(const SirFunction& func) {
        errors_.clear();
        states_.clear();
        currentScope_ = 0;
        
        for (const auto& block : func.blocks) {
            checkBlock(block);
        }
        
        return errors_;
    }
    
private:
    /**
     * فحص كتلة
     */
    void checkBlock(const SirBasicBlock& block) {
        for (const auto& inst : block.instructions) {
            checkInstruction(inst);
        }
    }
    
    /**
     * فحص تعليمة
     */
    void checkInstruction(const SirInstruction& inst) {
        switch (inst.opcode) {
            case Opcode::Borrow:
                checkBorrow(inst, false);
                break;
                
            case Opcode::BorrowMut:
                checkBorrow(inst, true);
                break;
                
            case Opcode::Move:
                checkMove(inst);
                break;
                
            case Opcode::Drop:
                checkDrop(inst);
                break;
                
            case Opcode::EndBorrow:
                handleEndBorrow(inst);
                break;
                
            case Opcode::Store:
                checkStore(inst);
                break;
                
            default:
                checkOperands(inst);
                break;
        }
    }
    
    /**
     * فحص استعارة
     */
    void checkBorrow(const SirInstruction& inst, bool isMut) {
        if (!inst.result || inst.operands.empty()) return;
        
        ValueId borrower = *inst.result;
        ValueId lender = std::get<ValueOperand>(inst.operands[0]).id;
        
        auto& state = states_[lender];
        
        // التحقق من الحالة
        if (state.isMoved) {
            reportError(BorrowErrorKind::UseAfterMove, inst.location,
                "لا يمكن الاستعارة من قيمة منقولة");
            return;
        }
        
        if (state.isDropped) {
            reportError(BorrowErrorKind::UseAfterFree, inst.location,
                "لا يمكن الاستعارة من قيمة مُحررة");
            return;
        }
        
        // فحص تعارض الاستعارات
        if (isMut) {
            // استعارة متغيرة
            if (state.hasSharedBorrow()) {
                reportError(BorrowErrorKind::SharedAndMut, inst.location,
                    "لا يمكن الاستعارة المتغيرة أثناء وجود استعارات ثابتة");
                
                // إضافة مواقع الاستعارات الأخرى
                for (const auto& b : state.activeBorrows) {
                    if (!b.isMut) {
                        errors_.back().relatedLocations.push_back(b.borrowedAt);
                    }
                }
                return;
            }
            
            if (state.hasMutBorrow()) {
                reportError(BorrowErrorKind::MultipleMut, inst.location,
                    "لا يمكن وجود أكثر من استعارة متغيرة");
                
                for (const auto& b : state.activeBorrows) {
                    if (b.isMut) {
                        errors_.back().relatedLocations.push_back(b.borrowedAt);
                    }
                }
                return;
            }
        } else {
            // استعارة ثابتة
            if (state.hasMutBorrow()) {
                reportError(BorrowErrorKind::SharedAndMut, inst.location,
                    "لا يمكن الاستعارة الثابتة أثناء استعارة متغيرة نشطة");
                
                for (const auto& b : state.activeBorrows) {
                    if (b.isMut) {
                        errors_.back().relatedLocations.push_back(b.borrowedAt);
                    }
                }
                return;
            }
        }
        
        // تسجيل الاستعارة
        state.activeBorrows.push_back({
            borrower, lender, isMut, inst.location, currentScope_
        });
    }
    
    /**
     * فحص النقل
     */
    void checkMove(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        ValueId source = std::get<ValueOperand>(inst.operands[0]).id;
        auto& state = states_[source];
        
        // التحقق من الحالة
        if (state.isMoved) {
            reportError(BorrowErrorKind::UseAfterMove, inst.location,
                "محاولة نقل قيمة تم نقلها مسبقاً");
            return;
        }
        
        if (state.isDropped) {
            reportError(BorrowErrorKind::UseAfterFree, inst.location,
                "محاولة نقل قيمة مُحررة");
            return;
        }
        
        // التحقق من الاستعارات النشطة
        if (!state.activeBorrows.empty()) {
            reportError(BorrowErrorKind::UseAfterMove, inst.location,
                "لا يمكن نقل قيمة أثناء وجود استعارات نشطة");
            
            for (const auto& b : state.activeBorrows) {
                errors_.back().relatedLocations.push_back(b.borrowedAt);
            }
            return;
        }
        
        // تنفيذ النقل
        state.isMoved = true;
    }
    
    /**
     * فحص الإسقاط
     */
    void checkDrop(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        ValueId value = std::get<ValueOperand>(inst.operands[0]).id;
        auto& state = states_[value];
        
        if (state.isDropped) {
            reportError(BorrowErrorKind::UseAfterFree, inst.location,
                "محاولة إسقاط قيمة تم إسقاطها مسبقاً");
            return;
        }
        
        // لا بأس إذا كانت منقولة
        if (!state.isMoved) {
            // التحقق من الاستعارات النشطة
            if (!state.activeBorrows.empty()) {
                reportError(BorrowErrorKind::DanglingReference, inst.location,
                    "لا يمكن إسقاط قيمة أثناء وجود استعارات نشطة");
                
                for (const auto& b : state.activeBorrows) {
                    errors_.back().relatedLocations.push_back(b.borrowedAt);
                }
                return;
            }
        }
        
        state.isDropped = true;
    }
    
    /**
     * معالجة إنهاء استعارة
     */
    void handleEndBorrow(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        ValueId borrower = std::get<ValueOperand>(inst.operands[0]).id;
        
        // إزالة الاستعارة من جميع المُعيرين
        for (auto& [id, state] : states_) {
            state.activeBorrows.erase(
                std::remove_if(state.activeBorrows.begin(), state.activeBorrows.end(),
                    [borrower](const BorrowRecord& b) { return b.borrower == borrower; }),
                state.activeBorrows.end()
            );
        }
    }
    
    /**
     * فحص التخزين
     */
    void checkStore(const SirInstruction& inst) {
        if (inst.operands.size() < 2) return;
        
        ValueId ptr = std::get<ValueOperand>(inst.operands[0]).id;
        
        // التحقق من أن المؤشر قابل للكتابة
        // (في تطبيق حقيقي، نتحقق من نوع المؤشر)
        
        checkValueUsable(ptr, inst.location);
    }
    
    /**
     * فحص المعاملات
     */
    void checkOperands(const SirInstruction& inst) {
        for (const auto& op : inst.operands) {
            if (auto* valOp = std::get_if<ValueOperand>(&op)) {
                checkValueUsable(valOp->id, inst.location);
            }
        }
    }
    
    /**
     * التحقق من قابلية استخدام قيمة
     */
    void checkValueUsable(ValueId id, const SourceLocation& loc) {
        auto& state = states_[id];
        
        if (state.isMoved) {
            reportError(BorrowErrorKind::UseAfterMove, loc,
                "استخدام قيمة بعد نقلها");
        }
        
        if (state.isDropped) {
            reportError(BorrowErrorKind::UseAfterFree, loc,
                "استخدام قيمة بعد تحريرها");
        }
    }
    
    /**
     * تقرير خطأ
     */
    void reportError(BorrowErrorKind kind, const SourceLocation& loc,
                     const std::string& msg) {
        errors_.push_back({kind, loc, msg, {}});
    }
    
    // الحالة الداخلية
    std::map<ValueId, BorrowState> states_;
    std::vector<BorrowError> errors_;
    int currentScope_ = 0;
};

// =============================================================================
// تقرير فحص الاستعارات
// =============================================================================

/**
 * تقرير الفحص
 */
class BorrowCheckReport {
public:
    void addErrors(const std::vector<BorrowError>& errors) {
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
            return "✅ فحص الاستعارات نجح! لا توجد أخطاء.\n";
        }
        
        std::string report = "❌ فشل فحص الاستعارات!\n";
        report += "تم العثور على " + std::to_string(errors_.size()) + " خطأ:\n\n";
        
        for (size_t i = 0; i < errors_.size(); i++) {
            report += std::to_string(i + 1) + ". " + 
                      errors_[i].toString() + "\n\n";
        }
        
        report += "─────────────────────────────────────────\n";
        report += "💡 تلميحات:\n";
        report += "• تأكد من عدم وجود استعارة ثابتة ومتغيرة معاً\n";
        report += "• لا تستخدم قيمة بعد نقلها\n";
        report += "• أنهِ الاستعارات قبل إسقاط القيمة\n";
        
        return report;
    }
    
private:
    std::vector<BorrowError> errors_;
};

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadBorrowChecker {
    sad::sir::BorrowChecker* checker;
    sad::sir::BorrowCheckReport* report;
};

SadBorrowChecker* sad_borrow_checker_new() {
    auto* ctx = new SadBorrowChecker();
    ctx->checker = new sad::sir::BorrowChecker();
    ctx->report = new sad::sir::BorrowCheckReport();
    return ctx;
}

int sad_borrow_check(SadBorrowChecker* ctx, void* function) {
    if (!ctx || !function) return -1;
    
    auto* func = static_cast<sad::sir::SirFunction*>(function);
    auto errors = ctx->checker->check(*func);
    ctx->report->addErrors(errors);
    
    return errors.size();
}

const char* sad_borrow_check_report(SadBorrowChecker* ctx) {
    if (!ctx || !ctx->report) return "";
    thread_local std::string report;
    report = ctx->report->toString();
    return report.c_str();
}

int sad_borrow_check_passed(SadBorrowChecker* ctx) {
    if (!ctx || !ctx->report) return 0;
    return ctx->report->hasErrors() ? 0 : 1;
}

void sad_borrow_checker_free(SadBorrowChecker* ctx) {
    if (ctx) {
        delete ctx->checker;
        delete ctx->report;
        delete ctx;
    }
}

} // extern "C"
