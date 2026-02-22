// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: document_store.cpp
// الوصف: مخزن المستندات - يدير كل المستندات المفتوحة في المحرر
// ══════════════════════════════════════════════════════════════════════════════
//
// مخزن المستندات هو المكون المسؤول عن:
//   ✦ تتبع كل المستندات المفتوحة وإصداراتها
//   ✦ تطبيق التغييرات التدريجية (incremental) بكفاءة
//   ✦ تقسيم المحتوى إلى أسطر لسهولة الوصول
//   ✦ ضمان الأمان عند الوصول من عدة خيوط
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

DocumentStore::DocumentStore() = default;
DocumentStore::~DocumentStore() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  عمليات دورة حياة المستند
// ══════════════════════════════════════════════════════════════════════════════

void DocumentStore::open(const TextDocumentItem& item) {
    std::unique_lock lock(mutex_);

    auto doc = std::make_shared<OpenDocument>();
    doc->uri = item.uri;
    doc->content = item.text;
    doc->version = item.version;
    doc->needs_reanalysis = true;
    doc->last_change = std::chrono::steady_clock::now();

    // تقسيم المحتوى إلى أسطر مسبقاً للوصول السريع
    recalculate_lines(*doc);

    documents_[item.uri] = doc;
}

void DocumentStore::update(const DocumentUri& uri,
                           const std::vector<TextDocumentContentChangeEvent>& changes,
                           int version) {
    std::unique_lock lock(mutex_);

    auto it = documents_.find(uri);
    if (it == documents_.end()) return;

    auto& doc = *it->second;
    doc.version = version;
    doc.last_change = std::chrono::steady_clock::now();

    for (const auto& change : changes) {
        if (change.range.has_value()) {
            // تغيير تدريجي - نطبق التعديل على النطاق المحدد
            apply_incremental_change(doc, *change.range, change.text);
        } else {
            // تحديث كامل - نستبدل المحتوى كله
            doc.content = change.text;
        }
    }

    // إعادة حساب الأسطر بعد التعديل
    recalculate_lines(doc);
    doc.needs_reanalysis = true;
}

void DocumentStore::close(const DocumentUri& uri) {
    std::unique_lock lock(mutex_);
    documents_.erase(uri);
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال الاستعلام
// ══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<OpenDocument> DocumentStore::get(const DocumentUri& uri) const {
    std::shared_lock lock(mutex_);
    auto it = documents_.find(uri);
    return (it != documents_.end()) ? it->second : nullptr;
}

std::vector<DocumentUri> DocumentStore::get_all_uris() const {
    std::shared_lock lock(mutex_);
    std::vector<DocumentUri> uris;
    uris.reserve(documents_.size());
    for (const auto& [uri, _] : documents_) {
        uris.push_back(uri);
    }
    return uris;
}

bool DocumentStore::is_open(const DocumentUri& uri) const {
    std::shared_lock lock(mutex_);
    return documents_.count(uri) > 0;
}

std::string DocumentStore::get_line(const DocumentUri& uri, int line) const {
    std::shared_lock lock(mutex_);
    auto it = documents_.find(uri);
    if (it == documents_.end()) return "";
    const auto& lines = it->second->lines;
    if (line >= 0 && line < static_cast<int>(lines.size())) {
        return lines[line];
    }
    return "";
}

std::string DocumentStore::get_word_at(const DocumentUri& uri, const Position& pos) const {
    std::string line = get_line(uri, pos.line);
    if (line.empty()) return "";
    return arabic::get_word_at(line, pos.character);
}

int DocumentStore::position_to_offset(const DocumentUri& uri, const Position& pos) const {
    std::shared_lock lock(mutex_);
    auto it = documents_.find(uri);
    if (it == documents_.end()) return -1;

    const auto& lines = it->second->lines;
    if (pos.line < 0 || pos.line >= static_cast<int>(lines.size())) return -1;

    // حساب الإزاحة من بداية المستند
    int offset = 0;
    for (int i = 0; i < pos.line; i++) {
        offset += static_cast<int>(lines[i].size()) + 1; // +1 لـ \n
    }

    // إضافة العمود (تحويل من UTF-16 إلى بايتات UTF-8)
    offset += arabic::utf16_column_to_utf8_offset(lines[pos.line], pos.character);

    return offset;
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال داخلية
// ══════════════════════════════════════════════════════════════════════════════

void DocumentStore::recalculate_lines(OpenDocument& doc) {
    doc.lines = arabic::split_lines(doc.content);
}

void DocumentStore::apply_incremental_change(OpenDocument& doc,
                                              const Range& range,
                                              const std::string& new_text) {
    const auto& lines = doc.lines;

    // حساب إزاحة البداية
    int start_offset = 0;
    for (int i = 0; i < range.start.line && i < static_cast<int>(lines.size()); i++) {
        start_offset += static_cast<int>(lines[i].size()) + 1;
    }
    if (range.start.line < static_cast<int>(lines.size())) {
        start_offset += arabic::utf16_column_to_utf8_offset(
            lines[range.start.line], range.start.character);
    }

    // حساب إزاحة النهاية
    int end_offset = 0;
    for (int i = 0; i < range.end.line && i < static_cast<int>(lines.size()); i++) {
        end_offset += static_cast<int>(lines[i].size()) + 1;
    }
    if (range.end.line < static_cast<int>(lines.size())) {
        end_offset += arabic::utf16_column_to_utf8_offset(
            lines[range.end.line], range.end.character);
    }

    // تطبيق التعديل
    if (start_offset >= 0 && end_offset >= start_offset &&
        start_offset <= static_cast<int>(doc.content.size())) {
        int replace_len = std::min(end_offset - start_offset,
                                    static_cast<int>(doc.content.size()) - start_offset);
        doc.content.replace(start_offset, replace_len, new_text);
    }
}

} // namespace lsp
} // namespace sad
