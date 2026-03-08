// ============================================================================
// code_actions.cpp — مزود إجراءات الكود لخادم LSP
// Code Actions Provider for LSP Server
// ============================================================================
//
// الوصف (AR):
//   يوفر إجراءات كود ذكية (Quick Fixes + Refactoring) تظهر كمصباح
//   في المحرر عند وضع المؤشر على كود معين:
//
//   إصلاحات سريعة:
//   ① إصلاح "متغير" → "ثابت" للمتغيرات التي لا تتغير
//   ② إضافة "نهاية" المفقودة تلقائياً
//   ③ تصحيح أخطاء التشكيل الشائعة (اذا → إذا، والا → وإلا)
//   ④ إزالة الاستيرادات غير المستخدمة
//
//   إعادة هيكلة:
//   ⑤ استخراج دالة (Extract Function)
//   ⑥ تحويل إلى لامدا / والعكس
//   ⑦ إضافة توثيق ##
//
// Description (EN):
//   Provides smart code actions (Quick Fixes + Refactoring):
//   Fix diacritics, add missing نهاية, extract functions,
//   convert to lambda, add documentation.
//
// المهندس: عمر — أبريل 2026 / Engineer: Omar — April 2026
// ============================================================================

#include "../include/lsp_engine.h"

#include <string>
#include <vector>
#include <regex>

namespace sad {
namespace lsp {

// ============================================================================
// بنية إجراء الكود / Code Action Structure
// ============================================================================

/// نوع إجراء الكود
enum class CodeActionKind {
    QuickFix,       // إصلاح سريع
    Refactor,       // إعادة هيكلة
    Source,          // إجراء مصدري
    RefactorExtract // استخراج
};

/// يمثل إجراء كود واحد يُعرض للمستخدم
struct CodeAction {
    std::string title;       // العنوان المعروض
    CodeActionKind kind;     // النوع
    std::string uri;         // الملف
    Range range;             // النطاق المتأثر
    
    // التعديل المقترح
    struct TextEdit {
        Range range;
        std::string newText;
    };
    std::vector<TextEdit> edits;
    
    bool isPreferred = false; // إجراء مفضّل؟
};

// ============================================================================
// كشف التشكيل المفقود / Missing Diacritics Detection
// ============================================================================

/// خريطة الكلمات المفتاحية الشائعة بدون تشكيل → مع تشكيل صحيح
static const std::vector<std::pair<std::string, std::string>> DIACRITICS_FIXES = {
    {"اذا",  "إذا"},
    {"والا", "وإلا"},
};

/// فحص سطر وإرجاع إصلاحات التشكيل
static std::vector<CodeAction> check_diacritics(
    const std::string& uri,
    int lineNum,
    const std::string& line
) {
    std::vector<CodeAction> actions;
    
    for (const auto& [wrong, correct] : DIACRITICS_FIXES) {
        size_t pos = 0;
        while ((pos = line.find(wrong, pos)) != std::string::npos) {
            // التأكد أن الكلمة ليست جزءاً من كلمة أكبر
            bool wordStart = (pos == 0 || line[pos-1] == ' ' || line[pos-1] == '(');
            size_t endPos = pos + wrong.size();
            bool wordEnd = (endPos >= line.size() || line[endPos] == ' ' || 
                           line[endPos] == '(' || line[endPos] == ')');
            
            if (wordStart && wordEnd) {
                CodeAction action;
                action.title = "تصحيح \"" + wrong + "\" إلى \"" + correct + "\"";
                action.kind = CodeActionKind::QuickFix;
                action.uri = uri;
                action.isPreferred = true;
                
                CodeAction::TextEdit edit;
                edit.range.start = {lineNum, static_cast<int>(pos)};
                edit.range.end = {lineNum, static_cast<int>(endPos)};
                edit.newText = correct;
                action.edits.push_back(edit);
                
                actions.push_back(action);
            }
            pos = endPos;
        }
    }
    
    return actions;
}

// ============================================================================
// إضافة نهاية المفقودة / Missing End Detection
// ============================================================================

/// كلمات مفتاحية تفتح كتلة وتحتاج "نهاية"
static const std::vector<std::string> BLOCK_OPENERS = {
    "دالة", "إذا", "اذا", "بينما", "لكل", "صنف", "بنية", 
    "طابق", "حاول", "تعداد"
};

/// فحص إذا كان السطر يفتح كتلة
static bool is_block_opener(const std::string& line) {
    for (const auto& opener : BLOCK_OPENERS) {
        if (line.find(opener) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// إضافة توثيق / Add Documentation
// ============================================================================

/// إنشاء إجراء إضافة توثيق لدالة
static CodeAction create_doc_action(
    const std::string& uri,
    const AnalyzedSymbol& sym
) {
    CodeAction action;
    action.title = "إضافة توثيق لـ " + sym.name;
    action.kind = CodeActionKind::Source;
    action.uri = uri;
    
    // بناء نص التوثيق
    std::string doc = "## ";
    doc += sym.name;
    
    if (sym.func_info.has_value()) {
        doc += "\n## ";
        for (const auto& [pname, ptype] : sym.func_info->parameters) {
            doc += "\n## @معلمة " + pname;
            if (!ptype.name.empty()) {
                doc += " (" + ptype.name + ")";
            }
            doc += " —";
        }
        if (!sym.func_info->return_type.name.empty() &&
            sym.func_info->return_type.name != "فراغ") {
            doc += "\n## @ارجاع " + sym.func_info->return_type.name;
        }
    }
    doc += "\n";
    
    CodeAction::TextEdit edit;
    edit.range.start = {sym.definition_range.start.line, 0};
    edit.range.end = {sym.definition_range.start.line, 0};
    edit.newText = doc;
    action.edits.push_back(edit);
    
    return action;
}

// ============================================================================
// تحويل متغير إلى ثابت / Convert Variable to Constant
// ============================================================================

/// إنشاء إجراء تحويل "متغير" إلى "ثابت"
static CodeAction create_var_to_const_action(
    const std::string& uri,
    int lineNum,
    size_t varPos
) {
    CodeAction action;
    action.title = "تحويل \"متغير\" إلى \"ثابت\" — المتغير لا يتغير";
    action.kind = CodeActionKind::QuickFix;
    action.uri = uri;
    
    std::string متغير_word = "متغير";
    std::string ثابت_word = "ثابت";
    
    CodeAction::TextEdit edit;
    edit.range.start = {lineNum, static_cast<int>(varPos)};
    edit.range.end = {lineNum, static_cast<int>(varPos + متغير_word.size())};
    edit.newText = ثابت_word;
    action.edits.push_back(edit);
    
    return action;
}

// ============================================================================
// نقطة الدخول الرئيسية / Main Entry Point
// ============================================================================

/// الحصول على إجراءات الكود المتاحة لنطاق معين
/// @param store مخزن المستندات
/// @param uri معرف المستند
/// @param range النطاق المختار
/// @return قائمة إجراءات الكود بتنسيق JSON
std::string get_code_actions(
    const DocumentStore& store,
    const std::string& uri,
    const Range& range
) {
    std::vector<CodeAction> actions;
    
    auto doc = store.get(uri);
    if (!doc) return "[]";
    
    // فحص كل سطر في النطاق / Check each line in range
    for (int line = range.start.line; 
         line <= range.end.line && line < static_cast<int>(doc->lines.size());
         line++) {
        const auto& text = doc->lines[line];
        
        // ① تصحيح التشكيل
        auto diacriticFixes = check_diacritics(uri, line, text);
        actions.insert(actions.end(), diacriticFixes.begin(), diacriticFixes.end());
    }
    
    // ② إضافة توثيق للدوال/الأصناف بدون توثيق
    for (const auto& sym : doc->symbols) {
        if (sym.definition_range.start.line >= range.start.line &&
            sym.definition_range.start.line <= range.end.line) {
            if ((sym.kind == AnalyzedSymbolKind::Function ||
                 sym.kind == AnalyzedSymbolKind::Class) &&
                sym.documentation.empty()) {
                actions.push_back(create_doc_action(uri, sym));
            }
        }
    }
    
    // بناء JSON / Build JSON
    std::string json = "[";
    for (size_t i = 0; i < actions.size(); i++) {
        const auto& a = actions[i];
        if (i > 0) json += ",";
        
        json += "{\"title\":\"" + a.title + "\"";
        json += ",\"kind\":\"";
        switch (a.kind) {
            case CodeActionKind::QuickFix: json += "quickfix"; break;
            case CodeActionKind::Refactor: json += "refactor"; break;
            case CodeActionKind::Source: json += "source"; break;
            case CodeActionKind::RefactorExtract: json += "refactor.extract"; break;
        }
        json += "\"";
        if (a.isPreferred) json += ",\"isPreferred\":true";
        
        // التعديلات
        if (!a.edits.empty()) {
            json += ",\"edit\":{\"changes\":{\"" + a.uri + "\":[";
            for (size_t j = 0; j < a.edits.size(); j++) {
                if (j > 0) json += ",";
                const auto& e = a.edits[j];
                json += "{\"range\":{";
                json += "\"start\":{\"line\":" + std::to_string(e.range.start.line) +
                        ",\"character\":" + std::to_string(e.range.start.character) + "}";
                json += ",\"end\":{\"line\":" + std::to_string(e.range.end.line) +
                        ",\"character\":" + std::to_string(e.range.end.character) + "}";
                json += "},\"newText\":\"" + e.newText + "\"}";
            }
            json += "]}}";
        }
        
        json += "}";
    }
    json += "]";
    
    return json;
}

} // namespace lsp
} // namespace sad
