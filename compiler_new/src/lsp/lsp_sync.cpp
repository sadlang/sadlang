/// تنفيذ مزامنة المستندات
///
/// يحافظ على نسخة محدّثة من المستندات المفتوحة
/// ويطبق التغييرات التزايدية بكفاءة.

#include "lsp_sync.h"
#include <sstream>
#include <algorithm>

namespace sad {
namespace lsp {

// ─── OpenDocument ───

/// تحديث السطور المخبأة من المحتوى
void OpenDocument::updateLines() {
    lines.clear();
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        // إزالة \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
    }
    // إضافة سطر فارغ أخير إذا انتهى المحتوى بسطر جديد
    if (!content.empty() && content.back() == '\n') {
        lines.emplace_back();
    }
}

/// الحصول على سطر بالفهرس
const std::string& OpenDocument::getLine(int index) const {
    static const std::string empty;
    if (index < 0 || index >= static_cast<int>(lines.size())) {
        return empty;
    }
    return lines[index];
}

/// تطبيق تغيير تزايدي
void OpenDocument::applyChange(int startLine, int startChar,
                                int endLine, int endChar,
                                const std::string& newText) {
    // حساب الموقع في النص الكامل
    size_t startOffset = 0;
    for (int i = 0; i < startLine && i < static_cast<int>(lines.size()); i++) {
        startOffset += lines[i].size() + 1; // +1 لـ \n
    }
    startOffset += startChar;
    
    size_t endOffset = 0;
    for (int i = 0; i < endLine && i < static_cast<int>(lines.size()); i++) {
        endOffset += lines[i].size() + 1;
    }
    endOffset += endChar;
    
    // تطبيق التغيير
    if (startOffset <= content.size() && endOffset <= content.size()) {
        content.replace(startOffset, endOffset - startOffset, newText);
    }
    
    // تحديث السطور المخبأة
    updateLines();
}

// ─── LspSync ───

LspSync::LspSync(LspServer& server) : server_(server) {}

/// تسجيل المعالجات
void LspSync::registerHandlers() {
    server_.registerNotificationHandler("textDocument/didOpen",
        [this](const std::string& params) { handleDidOpen(params); });
    
    server_.registerNotificationHandler("textDocument/didChange",
        [this](const std::string& params) { handleDidChange(params); });
    
    server_.registerNotificationHandler("textDocument/didSave",
        [this](const std::string& params) { handleDidSave(params); });
    
    server_.registerNotificationHandler("textDocument/didClose",
        [this](const std::string& params) { handleDidClose(params); });
}

/// الحصول على مستند مفتوح
OpenDocument* LspSync::getDocument(const std::string& uri) {
    auto it = documents_.find(uri);
    return it != documents_.end() ? &it->second : nullptr;
}

/// الحصول على مستند (ثابت)
const OpenDocument* LspSync::getDocument(const std::string& uri) const {
    auto it = documents_.find(uri);
    return it != documents_.end() ? &it->second : nullptr;
}

/// هل المستند مفتوح
bool LspSync::isOpen(const std::string& uri) const {
    return documents_.find(uri) != documents_.end();
}

/// معالجة إشعار didOpen
void LspSync::handleDidOpen(const std::string& params) {
    // تحليل JSON بسيط — استخراج uri, languageId, version, text
    // TODO: استخدام محلل JSON كامل
    
    OpenDocument doc;
    
    // استخراج uri
    size_t uri_pos = params.find("\"uri\"");
    if (uri_pos != std::string::npos) {
        size_t q1 = params.find('"', params.find(':', uri_pos) + 1);
        size_t q2 = params.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            doc.uri = params.substr(q1 + 1, q2 - q1 - 1);
        }
    }
    
    // استخراج languageId
    size_t lang_pos = params.find("\"languageId\"");
    if (lang_pos != std::string::npos) {
        size_t q1 = params.find('"', params.find(':', lang_pos) + 1);
        size_t q2 = params.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            doc.language_id = params.substr(q1 + 1, q2 - q1 - 1);
        }
    }
    
    // استخراج version
    size_t ver_pos = params.find("\"version\"");
    if (ver_pos != std::string::npos) {
        size_t colon = params.find(':', ver_pos);
        doc.version = std::atoi(params.c_str() + colon + 1);
    }
    
    // استخراج text
    size_t text_pos = params.find("\"text\"");
    if (text_pos != std::string::npos) {
        size_t q1 = params.find('"', params.find(':', text_pos) + 1);
        size_t q2 = params.rfind('"');
        if (q1 != std::string::npos && q2 > q1) {
            doc.content = params.substr(q1 + 1, q2 - q1 - 1);
            // فك ترميز أحرف JSON المهربة
            // TODO: فك ترميز كامل
        }
    }
    
    doc.updateLines();
    documents_[doc.uri] = std::move(doc);
}

/// معالجة إشعار didChange
void LspSync::handleDidChange(const std::string& params) {
    // استخراج uri
    std::string uri;
    size_t uri_pos = params.find("\"uri\"");
    if (uri_pos != std::string::npos) {
        size_t q1 = params.find('"', params.find(':', uri_pos) + 1);
        size_t q2 = params.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            uri = params.substr(q1 + 1, q2 - q1 - 1);
        }
    }
    
    auto* doc = getDocument(uri);
    if (!doc) return;
    
    // استخراج version
    size_t ver_pos = params.find("\"version\"");
    if (ver_pos != std::string::npos) {
        size_t colon = params.find(':', ver_pos);
        doc->version = std::atoi(params.c_str() + colon + 1);
    }
    
    // TODO: تحليل contentChanges وتطبيقها
    // حالياً نطلب النص الكامل
    size_t text_pos = params.find("\"text\"");
    if (text_pos != std::string::npos) {
        size_t q1 = params.find('"', params.find(':', text_pos) + 1);
        // البحث عن نهاية النص (معقد مع JSON المهربة)
        // TODO: محلل JSON كامل
    }
    
    doc->updateLines();
}

/// معالجة إشعار didSave
void LspSync::handleDidSave(const std::string& params) {
    // يمكن إطلاق تشخيصات كاملة هنا
    // TODO: تحليل URI وإطلاق تشخيصات
}

/// معالجة إشعار didClose
void LspSync::handleDidClose(const std::string& params) {
    // استخراج uri
    size_t uri_pos = params.find("\"uri\"");
    if (uri_pos != std::string::npos) {
        size_t q1 = params.find('"', params.find(':', uri_pos) + 1);
        size_t q2 = params.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos) {
            std::string uri = params.substr(q1 + 1, q2 - q1 - 1);
            documents_.erase(uri);
        }
    }
}

} // namespace lsp
} // namespace sad
