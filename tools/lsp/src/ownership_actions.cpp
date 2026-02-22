/**
 * =============================================================================
 * ملف: ownership_actions.cpp
 * الوصف: إجراءات الكود لإصلاح مشاكل الملكية
 * المهمة: T257 - Code actions for ownership fixes
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * 🔧 دليل المبتدئ لإجراءات الملكية
 * ═══════════════════════════════════════
 * 
 * ما هي إجراءات الكود؟
 * ─────────────────────
 * إصلاحات تلقائية يقترحها المحرر عند وجود مشكلة.
 * 
 * مثال:
 * ─────
 * ```sad
 * متغير س = نص::جديد("مرحبا")
 * متغير ع = س
 * اطبع(س)  // ❌ خطأ: س تم نقله
 *           //     💡 إجراءات مقترحة:
 *           //        1. استخدم &س بدلاً من نقل
 *           //        2. استنسخ س قبل النقل
 *           //        3. استخدم ع بدلاً من س
 * ```
 * 
 * أنواع الإجراءات:
 * ────────────────
 * 1. تحويل لاستعارة - إضافة &
 * 2. استنساخ - إضافة .استنسخ()
 * 3. تغيير الترتيب - إعادة ترتيب العمليات
 * 4. إضافة نطاق - تحديد عمر الاستعارة
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <sstream>

namespace sad::lsp {

// =============================================================================
// أنواع الأخطاء والإجراءات
// =============================================================================

/**
 * نوع خطأ الملكية
 */
enum class OwnershipErrorKind {
    UseAfterMove,       // استخدام بعد النقل
    DoubleBorrow,       // استعارتان متغيرتان
    BorrowWhileMutBorrowed, // استعارة أثناء استعارة متغيرة
    MutBorrowWhileBorrowed, // استعارة متغيرة أثناء استعارة
    MoveWhileBorrowed,  // نقل أثناء استعارة
    InvalidDrop,        // إسقاط غير صالح
    LifetimeTooShort    // دورة حياة قصيرة جداً
};

/**
 * نوع الإجراء المقترح
 */
enum class ActionKind {
    ConvertToBorrow,    // تحويل لاستعارة
    ConvertToMutBorrow, // تحويل لاستعارة متغيرة
    AddClone,           // إضافة استنساخ
    AddScope,           // إضافة نطاق جديد
    MoveEarlier,        // نقل العملية لأعلى
    MoveLater,          // نقل العملية لأسفل
    UseAlternative,     // استخدام متغير بديل
    RemoveBorrow,       // إزالة الاستعارة
    AddLifetimeAnnotation // إضافة توضيح دورة الحياة
};

// =============================================================================
// إجراء الكود
// =============================================================================

/**
 * تعديل نصي
 */
struct TextEdit {
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
    std::string newText;
    
    std::string toJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"range\": {\n";
        ss << "    \"start\": {\"line\": " << startLine << ", \"character\": " << startColumn << "},\n";
        ss << "    \"end\": {\"line\": " << endLine << ", \"character\": " << endColumn << "}\n";
        ss << "  },\n";
        ss << "  \"newText\": \"" << escapeJson(newText) << "\"\n";
        ss << "}";
        return ss.str();
    }
    
private:
    static std::string escapeJson(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

/**
 * إجراء كود واحد
 */
struct CodeAction {
    std::string title;
    ActionKind kind;
    std::string diagnosticMessage;
    std::vector<TextEdit> edits;
    bool isPreferred;
    
    std::string toJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"title\": \"" << title << "\",\n";
        ss << "  \"kind\": \"quickfix\",\n";
        ss << "  \"isPreferred\": " << (isPreferred ? "true" : "false") << ",\n";
        ss << "  \"edit\": {\n";
        ss << "    \"changes\": {\n";
        ss << "      \"file\": [\n";
        for (size_t i = 0; i < edits.size(); i++) {
            if (i > 0) ss << ",\n";
            ss << "        " << edits[i].toJson();
        }
        ss << "\n      ]\n";
        ss << "    }\n";
        ss << "  }\n";
        ss << "}";
        return ss.str();
    }
};

// =============================================================================
// معلومات التشخيص
// =============================================================================

/**
 * معلومات عن خطأ الملكية
 */
struct OwnershipDiagnostic {
    OwnershipErrorKind kind;
    int line;
    int column;
    int endColumn;
    std::string message;
    std::string variableName;
    std::optional<int> movedAtLine;
    std::optional<int> borrowedAtLine;
};

// =============================================================================
// مُولد الإجراءات
// =============================================================================

class OwnershipActionsProvider {
public:
    /**
     * الحصول على الإجراءات لتشخيص معين
     */
    std::vector<CodeAction> getActions(const OwnershipDiagnostic& diagnostic,
                                        const std::string& source) {
        std::vector<CodeAction> actions;
        
        switch (diagnostic.kind) {
            case OwnershipErrorKind::UseAfterMove:
                actions = getUseAfterMoveActions(diagnostic, source);
                break;
            case OwnershipErrorKind::DoubleBorrow:
                actions = getDoubleBorrowActions(diagnostic, source);
                break;
            case OwnershipErrorKind::BorrowWhileMutBorrowed:
                actions = getBorrowConflictActions(diagnostic, source);
                break;
            case OwnershipErrorKind::MutBorrowWhileBorrowed:
                actions = getMutBorrowConflictActions(diagnostic, source);
                break;
            case OwnershipErrorKind::MoveWhileBorrowed:
                actions = getMoveWhileBorrowedActions(diagnostic, source);
                break;
            case OwnershipErrorKind::LifetimeTooShort:
                actions = getLifetimeActions(diagnostic, source);
                break;
            default:
                break;
        }
        
        return actions;
    }
    
    /**
     * الحصول على جميع الإجراءات كـ JSON
     */
    std::string getActionsJson(const OwnershipDiagnostic& diagnostic,
                                const std::string& source) {
        auto actions = getActions(diagnostic, source);
        
        std::ostringstream ss;
        ss << "[\n";
        for (size_t i = 0; i < actions.size(); i++) {
            if (i > 0) ss << ",\n";
            ss << actions[i].toJson();
        }
        ss << "\n]";
        
        return ss.str();
    }
    
private:
    // ─────────────────────────────────────────────────────────────────────────
    // إجراءات "استخدام بعد النقل"
    // ─────────────────────────────────────────────────────────────────────────
    
    std::vector<CodeAction> getUseAfterMoveActions(
        const OwnershipDiagnostic& diagnostic,
        const std::string& source)
    {
        std::vector<CodeAction> actions;
        std::string var = diagnostic.variableName;
        
        // 1. استنساخ قبل النقل
        if (diagnostic.movedAtLine) {
            CodeAction cloneAction;
            cloneAction.title = "استنسخ " + var + " قبل النقل";
            cloneAction.kind = ActionKind::AddClone;
            cloneAction.isPreferred = true;
            
            // إيجاد موقع النقل وإضافة .استنسخ()
            TextEdit edit;
            edit.startLine = *diagnostic.movedAtLine - 1;
            edit.startColumn = 0;  // سيتم تحديده بدقة
            edit.endLine = edit.startLine;
            edit.endColumn = 0;
            edit.newText = var + ".استنسخ()";  // مبسط
            
            cloneAction.edits.push_back(edit);
            actions.push_back(cloneAction);
        }
        
        // 2. تحويل لاستعارة
        {
            CodeAction borrowAction;
            borrowAction.title = "استخدم استعارة &" + var + " بدلاً من النقل";
            borrowAction.kind = ActionKind::ConvertToBorrow;
            borrowAction.isPreferred = false;
            
            if (diagnostic.movedAtLine) {
                TextEdit edit;
                edit.startLine = *diagnostic.movedAtLine - 1;
                edit.startColumn = 0;
                edit.endLine = edit.startLine;
                edit.endColumn = var.length();
                edit.newText = "&" + var;
                
                borrowAction.edits.push_back(edit);
            }
            
            actions.push_back(borrowAction);
        }
        
        // 3. استخدام المتغير الجديد
        {
            CodeAction useNewAction;
            useNewAction.title = "استخدم المتغير الذي استلم القيمة";
            useNewAction.kind = ActionKind::UseAlternative;
            useNewAction.isPreferred = false;
            
            // سيحتاج لمعرفة اسم المتغير الجديد
            actions.push_back(useNewAction);
        }
        
        return actions;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // إجراءات "استعارتان متغيرتان"
    // ─────────────────────────────────────────────────────────────────────────
    
    std::vector<CodeAction> getDoubleBorrowActions(
        const OwnershipDiagnostic& diagnostic,
        const std::string& source)
    {
        std::vector<CodeAction> actions;
        
        // 1. إضافة نطاق جديد
        {
            CodeAction scopeAction;
            scopeAction.title = "أضف نطاقاً جديداً لتقليل عمر الاستعارة الأولى";
            scopeAction.kind = ActionKind::AddScope;
            scopeAction.isPreferred = true;
            
            // سيضيف { ... } حول الاستعارة الأولى
            actions.push_back(scopeAction);
        }
        
        // 2. دمج الاستعارتين
        {
            CodeAction mergeAction;
            mergeAction.title = "ادمج الاستعارتين في واحدة";
            mergeAction.kind = ActionKind::RemoveBorrow;
            mergeAction.isPreferred = false;
            
            actions.push_back(mergeAction);
        }
        
        return actions;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // إجراءات تعارض الاستعارات
    // ─────────────────────────────────────────────────────────────────────────
    
    std::vector<CodeAction> getBorrowConflictActions(
        const OwnershipDiagnostic& diagnostic,
        const std::string& source)
    {
        std::vector<CodeAction> actions;
        
        // 1. أنهِ الاستعارة المتغيرة أولاً
        {
            CodeAction endBorrowAction;
            endBorrowAction.title = "أضف نطاقاً لإنهاء الاستعارة المتغيرة قبل الاستعارة الجديدة";
            endBorrowAction.kind = ActionKind::AddScope;
            endBorrowAction.isPreferred = true;
            
            actions.push_back(endBorrowAction);
        }
        
        // 2. غيّر الترتيب
        {
            CodeAction reorderAction;
            reorderAction.title = "أعد ترتيب العمليات";
            reorderAction.kind = ActionKind::MoveEarlier;
            reorderAction.isPreferred = false;
            
            actions.push_back(reorderAction);
        }
        
        return actions;
    }
    
    std::vector<CodeAction> getMutBorrowConflictActions(
        const OwnershipDiagnostic& diagnostic,
        const std::string& source)
    {
        std::vector<CodeAction> actions;
        
        // 1. انتظر انتهاء الاستعارات الحالية
        {
            CodeAction waitAction;
            waitAction.title = "أضف نطاقاً لإنهاء الاستعارات الحالية أولاً";
            waitAction.kind = ActionKind::AddScope;
            waitAction.isPreferred = true;
            
            actions.push_back(waitAction);
        }
        
        // 2. استخدم استعارة غير متغيرة
        {
            CodeAction immutableAction;
            immutableAction.title = "استخدم استعارة غير متغيرة & بدلاً من &متغير";
            immutableAction.kind = ActionKind::ConvertToBorrow;
            immutableAction.isPreferred = false;
            
            actions.push_back(immutableAction);
        }
        
        return actions;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // إجراءات "نقل أثناء استعارة"
    // ─────────────────────────────────────────────────────────────────────────
    
    std::vector<CodeAction> getMoveWhileBorrowedActions(
        const OwnershipDiagnostic& diagnostic,
        const std::string& source)
    {
        std::vector<CodeAction> actions;
        
        // 1. أنهِ الاستعارة أولاً
        {
            CodeAction endFirstAction;
            endFirstAction.title = "أضف نطاقاً لإنهاء الاستعارة قبل النقل";
            endFirstAction.kind = ActionKind::AddScope;
            endFirstAction.isPreferred = true;
            
            actions.push_back(endFirstAction);
        }
        
        // 2. استنسخ بدلاً من النقل
        {
            CodeAction cloneAction;
            cloneAction.title = "استنسخ بدلاً من النقل";
            cloneAction.kind = ActionKind::AddClone;
            cloneAction.isPreferred = false;
            
            actions.push_back(cloneAction);
        }
        
        return actions;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // إجراءات دورة الحياة
    // ─────────────────────────────────────────────────────────────────────────
    
    std::vector<CodeAction> getLifetimeActions(
        const OwnershipDiagnostic& diagnostic,
        const std::string& source)
    {
        std::vector<CodeAction> actions;
        
        // 1. إضافة توضيح دورة الحياة
        {
            CodeAction annotateAction;
            annotateAction.title = "أضف توضيح دورة الحياة";
            annotateAction.kind = ActionKind::AddLifetimeAnnotation;
            annotateAction.isPreferred = true;
            
            actions.push_back(annotateAction);
        }
        
        // 2. استنساخ
        {
            CodeAction cloneAction;
            cloneAction.title = "استنسخ القيمة لإطالة عمرها";
            cloneAction.kind = ActionKind::AddClone;
            cloneAction.isPreferred = false;
            
            actions.push_back(cloneAction);
        }
        
        return actions;
    }
};

} // namespace sad::lsp

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadOwnershipActionsProvider {
    sad::lsp::OwnershipActionsProvider* provider;
};

SadOwnershipActionsProvider* sad_ownership_actions_new() {
    auto* ctx = new SadOwnershipActionsProvider();
    ctx->provider = new sad::lsp::OwnershipActionsProvider();
    return ctx;
}

const char* sad_ownership_actions_get(SadOwnershipActionsProvider* ctx,
                                      int errorKind,
                                      int line,
                                      int column,
                                      const char* varName,
                                      const char* source) {
    sad::lsp::OwnershipDiagnostic diag;
    diag.kind = static_cast<sad::lsp::OwnershipErrorKind>(errorKind);
    diag.line = line;
    diag.column = column;
    diag.variableName = varName;
    
    static std::string result;
    result = ctx->provider->getActionsJson(diag, source);
    return result.c_str();
}

void sad_ownership_actions_free(SadOwnershipActionsProvider* ctx) {
    if (ctx) {
        delete ctx->provider;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef OWNERSHIP_ACTIONS_TEST

#include <iostream>
#include <cassert>

void testUseAfterMoveActions() {
    std::cout << "🔧 اختبار إجراءات الاستخدام بعد النقل...\n";
    
    sad::lsp::OwnershipActionsProvider provider;
    
    sad::lsp::OwnershipDiagnostic diag;
    diag.kind = sad::lsp::OwnershipErrorKind::UseAfterMove;
    diag.line = 5;
    diag.column = 10;
    diag.variableName = "س";
    diag.movedAtLine = 3;
    
    auto actions = provider.getActions(diag, "");
    
    // يجب أن يكون هناك على الأقل إجراءان
    assert(actions.size() >= 2);
    
    // يجب أن يكون أحدها مفضلاً
    bool hasPreferred = false;
    for (const auto& a : actions) {
        if (a.isPreferred) {
            hasPreferred = true;
            break;
        }
    }
    assert(hasPreferred);
    
    std::cout << "   ✅ نجح\n";
}

void testDoubleBorrowActions() {
    std::cout << "🔧 اختبار إجراءات الاستعارة المزدوجة...\n";
    
    sad::lsp::OwnershipActionsProvider provider;
    
    sad::lsp::OwnershipDiagnostic diag;
    diag.kind = sad::lsp::OwnershipErrorKind::DoubleBorrow;
    diag.line = 5;
    diag.column = 10;
    diag.variableName = "س";
    
    auto actions = provider.getActions(diag, "");
    
    assert(!actions.empty());
    
    std::cout << "   ✅ نجح\n";
}

void testActionsJson() {
    std::cout << "📝 اختبار JSON للإجراءات...\n";
    
    sad::lsp::OwnershipActionsProvider provider;
    
    sad::lsp::OwnershipDiagnostic diag;
    diag.kind = sad::lsp::OwnershipErrorKind::UseAfterMove;
    diag.line = 5;
    diag.column = 10;
    diag.variableName = "س";
    diag.movedAtLine = 3;
    
    auto json = provider.getActionsJson(diag, "");
    
    assert(json.find("title") != std::string::npos);
    assert(json.find("quickfix") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات إجراءات الملكية\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testUseAfterMoveActions();
    testDoubleBorrowActions();
    testActionsJson();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // OWNERSHIP_ACTIONS_TEST
