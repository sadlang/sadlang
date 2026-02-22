/// مزامنة المستندات (Document Sync)
///
/// يعالج إشعارات فتح/تغيير/إغلاق/حفظ المستندات
/// ويحافظ على نسخة محدّثة من كل مستند مفتوح.

#pragma once

#include "lsp_server.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {
namespace lsp {

/// نسخة مستند مفتوح
struct OpenDocument {
    /// عنوان URI
    std::string uri;
    /// معرف اللغة
    std::string language_id;
    /// رقم الإصدار
    int version = 0;
    /// المحتوى الكامل
    std::string content;
    /// السطور (مخبأة)
    std::vector<std::string> lines;
    
    /// تحديث السطور المخبأة
    void updateLines();
    
    /// الحصول على سطر بالفهرس
    const std::string& getLine(int index) const;
    
    /// عدد السطور
    int lineCount() const { return static_cast<int>(lines.size()); }
    
    /// تطبيق تغيير تزايدي
    void applyChange(int startLine, int startChar, int endLine, int endChar,
                     const std::string& newText);
};

/// معالج مزامنة المستندات
class LspSync {
public:
    explicit LspSync(LspServer& server);

    /// تسجيل المعالجات
    void registerHandlers();

    /// الحصول على مستند مفتوح
    OpenDocument* getDocument(const std::string& uri);
    
    /// الحصول على مستند (ثابت)
    const OpenDocument* getDocument(const std::string& uri) const;
    
    /// هل المستند مفتوح
    bool isOpen(const std::string& uri) const;
    
    /// عدد المستندات المفتوحة
    size_t openDocumentCount() const { return documents_.size(); }

private:
    /// معالجة إشعار didOpen
    void handleDidOpen(const std::string& params);
    
    /// معالجة إشعار didChange
    void handleDidChange(const std::string& params);
    
    /// معالجة إشعار didSave
    void handleDidSave(const std::string& params);
    
    /// معالجة إشعار didClose
    void handleDidClose(const std::string& params);

    LspServer& server_;
    std::unordered_map<std::string, OpenDocument> documents_;
};

} // namespace lsp
} // namespace sad
