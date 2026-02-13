/// تنفيذ تهيئة بروتوكول LSP
///
/// يعالج المصافحة الأولية مع العميل ويحدد
/// قدرات الخادم المدعومة.

#include "lsp_initialize.h"
#include <sstream>

namespace sad {
namespace lsp {

LspInitialize::LspInitialize(LspServer& server) : server_(server) {}

/// تسجيل معالجات التهيئة
void LspInitialize::registerHandlers() {
    server_.registerHandler("initialize",
        [this](const std::string& params) { return handleInitialize(params); });
    
    server_.registerNotificationHandler("initialized",
        [this](const std::string& params) { handleInitialized(params); });
}

/// معالجة طلب initialize
std::string LspInitialize::handleInitialize(const std::string& params) {
    // تحليل rootUri من المعاملات
    // TODO: استخدام محلل JSON كامل
    
    std::ostringstream result;
    result << "{\"capabilities\":" << buildCapabilitiesJson()
           << ",\"serverInfo\":{\"name\":\"sadc-lsp\",\"version\":\"1.0.0\"}}";
    
    return result.str();
}

/// معالجة إشعار initialized
void LspInitialize::handleInitialized(const std::string& params) {
    // الخادم جاهز — يمكن إرسال إشعارات إلى العميل
    // مثل تسجيل تغييرات الإعدادات
}

/// بناء JSON قدرات الخادم
std::string LspInitialize::buildCapabilitiesJson() const {
    const auto& caps = server_.capabilities();
    std::ostringstream json;
    
    json << "{";
    
    // مزامنة المستندات
    json << "\"textDocumentSync\":{\"openClose\":true,\"change\":2,\"save\":{\"includeText\":true}}";
    
    // الإكمال التلقائي
    if (caps.completionProvider) {
        json << ",\"completionProvider\":{"
             << "\"triggerCharacters\":[\".\",\":\",\"\\\"\",\"'\",\"/\",\"،\"],"
             << "\"resolveProvider\":true}";
    }
    
    // التحويم
    if (caps.hoverProvider) {
        json << ",\"hoverProvider\":true";
    }
    
    // مساعدة التوقيع
    if (caps.signatureHelpProvider) {
        json << ",\"signatureHelpProvider\":{"
             << "\"triggerCharacters\":[\"(\",\",\",\"،\"]}";
    }
    
    // الذهاب للتعريف
    if (caps.definitionProvider) {
        json << ",\"definitionProvider\":true";
    }
    
    // المراجع
    if (caps.referencesProvider) {
        json << ",\"referencesProvider\":true";
    }
    
    // رموز المستند
    if (caps.documentSymbolProvider) {
        json << ",\"documentSymbolProvider\":true";
    }
    
    // إجراءات الكود
    if (caps.codeActionProvider) {
        json << ",\"codeActionProvider\":{"
             << "\"codeActionKinds\":[\"quickfix\",\"refactor\",\"source\","
             << "\"source.organizeImports\"]}";
    }
    
    // عدسات الكود
    if (caps.codeLensProvider) {
        json << ",\"codeLensProvider\":{\"resolveProvider\":true}";
    }
    
    // التنسيق
    if (caps.documentFormattingProvider) {
        json << ",\"documentFormattingProvider\":true";
    }
    if (caps.documentRangeFormattingProvider) {
        json << ",\"documentRangeFormattingProvider\":true";
    }
    if (caps.documentOnTypeFormattingProvider) {
        json << ",\"documentOnTypeFormattingProvider\":{"
             << "\"firstTriggerCharacter\":\"}\","
             << "\"moreTriggerCharacter\":[\";\",\"\\n\",\"؛\"]}";
    }
    
    // إعادة التسمية
    if (caps.renameProvider) {
        json << ",\"renameProvider\":{\"prepareProvider\":true}";
    }
    
    // الطي
    if (caps.foldingRangeProvider) {
        json << ",\"foldingRangeProvider\":true";
    }
    
    // نطاق التحديد
    if (caps.selectionRangeProvider) {
        json << ",\"selectionRangeProvider\":true";
    }
    
    // الرموز الدلالية
    if (caps.semanticTokensProvider) {
        json << ",\"semanticTokensProvider\":{"
             << "\"legend\":" << buildSemanticTokensLegend()
             << ",\"full\":true,\"range\":true}";
    }
    
    // تلميحات الإدراج
    if (caps.inlayHintProvider) {
        json << ",\"inlayHintProvider\":{\"resolveProvider\":true}";
    }
    
    // تسلسل الاستدعاءات
    if (caps.callHierarchyProvider) {
        json << ",\"callHierarchyProvider\":true";
    }
    
    // تسلسل الأنواع
    if (caps.typeHierarchyProvider) {
        json << ",\"typeHierarchyProvider\":true";
    }
    
    // رموز مجلد العمل
    if (caps.workspaceSymbolProvider) {
        json << ",\"workspaceSymbolProvider\":true";
    }
    
    json << "}";
    return json.str();
}

/// بناء وسيلة إيضاح الرموز الدلالية
std::string LspInitialize::buildSemanticTokensLegend() const {
    return "{"
        "\"tokenTypes\":["
            "\"namespace\",\"type\",\"class\",\"enum\",\"interface\","
            "\"struct\",\"typeParameter\",\"parameter\",\"variable\","
            "\"property\",\"enumMember\",\"event\",\"function\",\"method\","
            "\"macro\",\"keyword\",\"modifier\",\"comment\",\"string\","
            "\"number\",\"regexp\",\"operator\",\"decorator\""
        "],"
        "\"tokenModifiers\":["
            "\"declaration\",\"definition\",\"readonly\",\"static\","
            "\"deprecated\",\"abstract\",\"async\",\"modification\","
            "\"documentation\",\"defaultLibrary\""
        "]"
    "}";
}

} // namespace lsp
} // namespace sad
