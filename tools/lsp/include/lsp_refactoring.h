/**
 * @file lsp_refactoring.h
 * @brief دعم إعادة الهيكلة في خادم LSP
 * @brief (EN) Refactoring support for LSP server
 * 
 * @author عمر
 * @date أكتوبر 2026
 */

#ifndef SAD_LSP_REFACTORING_H
#define SAD_LSP_REFACTORING_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;optional&gt;
#include &lt;memory&gt;

namespace Sad {
namespace LSP {

//==============================================================================
// أنواع التعديلات
//==============================================================================

/**
 * @brief موقع في الكود
 */
struct Position {
    int line;       // 0-based
    int character;  // 0-based (UTF-16)
    
    Position() : line(0), character(0) {}
    Position(int l, int c) : line(l), character(c) {}
};

/**
 * @brief نطاق في الكود
 */
struct Range {
    Position start;
    Position end;
    
    Range() = default;
    Range(Position s, Position e) : start(s), end(e) {}
    Range(int sl, int sc, int el, int ec) : start(sl, sc), end(el, ec) {}
};

/**
 * @brief تعديل نصي
 */
struct TextEdit {
    Range range;
    std::string newText;
    
    TextEdit() = default;
    TextEdit(Range r, const std::string&amp; text) : range(r), newText(text) {}
};

/**
 * @brief تعديل ملف
 */
struct DocumentEdit {
    std::string uri;
    std::vector&lt;TextEdit&gt; edits;
};

/**
 * @brief تعديل مساحة العمل
 */
struct WorkspaceEdit {
    std::vector&lt;DocumentEdit&gt; documentChanges;
    
    void addEdit(const std::string&amp; uri, const TextEdit&amp; edit) {
        for (auto&amp; doc : documentChanges) {
            if (doc.uri == uri) {
                doc.edits.push_back(edit);
                return;
            }
        }
        documentChanges.push_back({uri, {edit}});
    }
};

//==============================================================================
// إعادة التسمية (Rename)
//==============================================================================

/**
 * @brief نتيجة تحضير إعادة التسمية
 */
struct PrepareRenameResult {
    Range range;
    std::string placeholder;
    bool canRename = true;
    std::string error;
};

/**
 * @brief خدمة إعادة التسمية
 */
class RenameProvider {
public:
    /**
     * @brief تحضير إعادة التسمية
     */
    std::optional&lt;PrepareRenameResult&gt; prepareRename(
        const std::string&amp; uri,
        Position position
    );
    
    /**
     * @brief تنفيذ إعادة التسمية
     */
    std::optional&lt;WorkspaceEdit&gt; rename(
        const std::string&amp; uri,
        Position position,
        const std::string&amp; newName
    );

private:
    /**
     * @brief العثور على كل مراجع الرمز
     */
    std::vector&lt;std::pair&lt;std::string, Range&gt;&gt; findAllReferences(
        const std::string&amp; uri,
        Position position
    );
    
    /**
     * @brief التحقق من صحة الاسم الجديد
     */
    bool isValidIdentifier(const std::string&amp; name);
};

//==============================================================================
// استخراج دالة (Extract Function)
//==============================================================================

/**
 * @brief إجراء استخراج دالة
 */
class ExtractFunctionAction {
public:
    /**
     * @brief التحقق من إمكانية الاستخراج
     */
    bool canExtract(const std::string&amp; uri, Range selection);
    
    /**
     * @brief استخراج الكود المحدد إلى دالة جديدة
     */
    std::optional&lt;WorkspaceEdit&gt; extract(
        const std::string&amp; uri,
        Range selection,
        const std::string&amp; functionName
    );

private:
    /**
     * @brief تحليل المتغيرات المستخدمة
     */
    struct VariableAnalysis {
        std::vector&lt;std::string&gt; inputs;   // المتغيرات المقروءة
        std::vector&lt;std::string&gt; outputs;  // المتغيرات المكتوبة
        std::vector&lt;std::string&gt; locals;   // المتغيرات المحلية
    };
    
    VariableAnalysis analyzeVariables(const std::string&amp; code);
    
    /**
     * @brief توليد كود الدالة الجديدة
     */
    std::string generateFunction(
        const std::string&amp; name,
        const std::string&amp; body,
        const VariableAnalysis&amp; vars
    );
};

//==============================================================================
// استخراج متغير (Extract Variable)
//==============================================================================

/**
 * @brief إجراء استخراج متغير
 */
class ExtractVariableAction {
public:
    /**
     * @brief استخراج تعبير إلى متغير
     */
    std::optional&lt;WorkspaceEdit&gt; extract(
        const std::string&amp; uri,
        Range selection,
        const std::string&amp; variableName
    );
    
    /**
     * @brief استخراج مع استبدال كل التكرارات
     */
    std::optional&lt;WorkspaceEdit&gt; extractAll(
        const std::string&amp; uri,
        Range selection,
        const std::string&amp; variableName
    );
};

//==============================================================================
// تضمين دالة (Inline Function)
//==============================================================================

/**
 * @brief إجراء تضمين دالة
 */
class InlineFunctionAction {
public:
    /**
     * @brief التحقق من إمكانية التضمين
     */
    bool canInline(const std::string&amp; uri, Position position);
    
    /**
     * @brief تضمين استدعاء دالة واحد
     */
    std::optional&lt;WorkspaceEdit&gt; inlineCall(
        const std::string&amp; uri,
        Position position
    );
    
    /**
     * @brief تضمين كل استدعاءات الدالة وحذفها
     */
    std::optional&lt;WorkspaceEdit&gt; inlineAndRemove(
        const std::string&amp; uri,
        Position position
    );
};

//==============================================================================
// تحويل إلى صنف (Extract Class)
//==============================================================================

/**
 * @brief إجراء استخراج صنف
 */
class ExtractClassAction {
public:
    /**
     * @brief استخراج حقول ودوال إلى صنف جديد
     */
    std::optional&lt;WorkspaceEdit&gt; extract(
        const std::string&amp; uri,
        const std::string&amp; className,
        const std::vector&lt;std::string&gt;&amp; memberNames
    );
};

//==============================================================================
// تحويل لامدا إلى دالة
//==============================================================================

/**
 * @brief تحويل لامدا إلى دالة مسماة
 */
class ConvertLambdaAction {
public:
    std::optional&lt;WorkspaceEdit&gt; toFunction(
        const std::string&amp; uri,
        Range lambdaRange,
        const std::string&amp; functionName
    );
};

//==============================================================================
// تنظيم الاستيرادات
//==============================================================================

/**
 * @brief تنظيم عبارات الاستيراد
 */
class OrganizeImportsAction {
public:
    /**
     * @brief تنظيم الاستيرادات (ترتيب + إزالة غير المستخدمة)
     */
    std::optional&lt;WorkspaceEdit&gt; organize(const std::string&amp; uri);
    
    /**
     * @brief إزالة الاستيرادات غير المستخدمة فقط
     */
    std::optional&lt;WorkspaceEdit&gt; removeUnused(const std::string&amp; uri);
    
    /**
     * @brief ترتيب الاستيرادات أبجدياً
     */
    std::optional&lt;WorkspaceEdit&gt; sort(const std::string&amp; uri);
};

//==============================================================================
// إجراءات الكود (Code Actions)
//==============================================================================

/**
 * @brief نوع إجراء الكود
 */
enum class CodeActionKind {
    QUICK_FIX,              // إصلاح سريع
    REFACTOR,               // إعادة هيكلة
    REFACTOR_EXTRACT,       // استخراج
    REFACTOR_INLINE,        // تضمين
    REFACTOR_REWRITE,       // إعادة كتابة
    SOURCE,                 // إجراء مصدري
    SOURCE_ORGANIZE_IMPORTS // تنظيم الاستيرادات
};

/**
 * @brief إجراء كود
 */
struct CodeAction {
    std::string title;
    CodeActionKind kind;
    std::vector&lt;std::string&gt; diagnostics;
    bool isPreferred = false;
    std::optional&lt;WorkspaceEdit&gt; edit;
    std::string command;
    std::vector&lt;std::string&gt; arguments;
};

/**
 * @brief مزود إجراءات الكود
 */
class CodeActionsProvider {
public:
    /**
     * @brief الحصول على الإجراءات المتاحة
     */
    std::vector&lt;CodeAction&gt; getCodeActions(
        const std::string&amp; uri,
        Range range,
        const std::vector&lt;std::string&gt;&amp; diagnosticCodes
    );
    
    /**
     * @brief حل إجراء (تحميل التعديلات)
     */
    std::optional&lt;CodeAction&gt; resolveAction(const CodeAction&amp; action);

private:
    RenameProvider renameProvider_;
    ExtractFunctionAction extractFunction_;
    ExtractVariableAction extractVariable_;
    InlineFunctionAction inlineFunction_;
    OrganizeImportsAction organizeImports_;
    
    // إضافة إجراءات إصلاح سريع
    void addQuickFixes(std::vector&lt;CodeAction&gt;&amp; actions,
                       const std::string&amp; uri,
                       Range range,
                       const std::vector&lt;std::string&gt;&amp; diagnostics);
    
    // إضافة إجراءات إعادة الهيكلة
    void addRefactorings(std::vector&lt;CodeAction&gt;&amp; actions,
                         const std::string&amp; uri,
                         Range range);
};

//==============================================================================
// أسماء الإجراءات بالعربية
//==============================================================================

namespace Arabic {
    const std::string إعادة_التسمية = "إعادة التسمية";
    const std::string استخراج_دالة = "استخراج إلى دالة";
    const std::string استخراج_متغير = "استخراج إلى متغير";
    const std::string تضمين_دالة = "تضمين الدالة";
    const std::string استخراج_صنف = "استخراج إلى صنف";
    const std::string تنظيم_الاستيرادات = "تنظيم عبارات الاستيراد";
    const std::string تحويل_لامدا = "تحويل إلى دالة مسماة";
}

} // namespace LSP
} // namespace Sad

#endif // SAD_LSP_REFACTORING_H
