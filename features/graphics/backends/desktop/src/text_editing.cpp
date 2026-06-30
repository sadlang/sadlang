/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: text_editing.cpp
 * المسار: features/graphics/backends/desktop/src/text_editing.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ نظام تحرير النصوص المتقدم.
 *
 * يشمل:
 * - TextEditState: المؤشر والتحديد
 * - UndoRedoManager: التراجع/الإعادة
 * - SadSyntaxHighlighter: التلوين النحوي للغة ص
 * - TextEditStateMap: ربط الحالات بالعقد
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "text_editing.h"
#include <algorithm>
#include <unordered_set>
#include <cstring>

namespace sad {
namespace ui {
namespace desktop {

// ═══════════════════════════════════════════════════════════════════════════════
// TextEditState — المؤشر والتحديد
// ═══════════════════════════════════════════════════════════════════════════════

size_t TextEditState::nextCharPos(size_t pos) const {
    if (pos >= text.size()) return text.size();
    unsigned char c = static_cast<unsigned char>(text[pos]);
    // تحديد طول الحرف UTF-8
    size_t len = 1;
    if ((c & 0x80) == 0)       len = 1;      // ASCII
    else if ((c & 0xE0) == 0xC0) len = 2;    // 2-byte
    else if ((c & 0xF0) == 0xE0) len = 3;    // 3-byte (عربي)
    else if ((c & 0xF8) == 0xF0) len = 4;    // 4-byte (إيموجي)
    size_t next = pos + len;
    return (next > text.size()) ? text.size() : next;
}

size_t TextEditState::prevCharPos(size_t pos) const {
    if (pos == 0) return 0;
    size_t p = pos - 1;
    // تراجع عبر continuation bytes
    while (p > 0 && (static_cast<unsigned char>(text[p]) & 0xC0) == 0x80) {
        --p;
    }
    return p;
}

std::string TextEditState::getSelectedText() const {
    if (!hasSelection) return "";
    size_t s = std::min(selectionStart, selectionEnd);
    size_t e = std::max(selectionStart, selectionEnd);
    if (s >= text.size()) return "";
    if (e > text.size()) e = text.size();
    return text.substr(s, e - s);
}

void TextEditState::deleteSelection() {
    if (!hasSelection) return;
    size_t s = std::min(selectionStart, selectionEnd);
    size_t e = std::max(selectionStart, selectionEnd);
    if (s >= text.size()) { hasSelection = false; return; }
    if (e > text.size()) e = text.size();
    text.erase(s, e - s);
    cursorPos = s;
    hasSelection = false;
}

void TextEditState::insertText(const std::string& newText) {
    if (hasSelection) {
        deleteSelection();
    }
    if (cursorPos > text.size()) cursorPos = text.size();
    text.insert(cursorPos, newText);
    cursorPos += newText.size();
}

void TextEditState::moveCursorRight(bool extendSelection) {
    size_t oldPos = cursorPos;
    if (hasSelection && !extendSelection) {
        // القفز إلى نهاية التحديد
        cursorPos = std::max(selectionStart, selectionEnd);
        hasSelection = false;
        return;
    }
    cursorPos = nextCharPos(cursorPos);
    if (extendSelection) {
        if (!hasSelection) {
            selectionStart = oldPos;
            hasSelection = true;
        }
        selectionEnd = cursorPos;
        if (selectionStart == selectionEnd) hasSelection = false;
    } else {
        hasSelection = false;
    }
}

void TextEditState::moveCursorLeft(bool extendSelection) {
    size_t oldPos = cursorPos;
    if (hasSelection && !extendSelection) {
        // القفز إلى بداية التحديد
        cursorPos = std::min(selectionStart, selectionEnd);
        hasSelection = false;
        return;
    }
    cursorPos = prevCharPos(cursorPos);
    if (extendSelection) {
        if (!hasSelection) {
            selectionStart = oldPos;
            hasSelection = true;
        }
        selectionEnd = cursorPos;
        if (selectionStart == selectionEnd) hasSelection = false;
    } else {
        hasSelection = false;
    }
}

void TextEditState::moveCursorHome(bool extendSelection) {
    size_t oldPos = cursorPos;
    cursorPos = 0;
    if (extendSelection) {
        if (!hasSelection) {
            selectionStart = oldPos;
            hasSelection = true;
        }
        selectionEnd = 0;
        if (selectionStart == selectionEnd) hasSelection = false;
    } else {
        hasSelection = false;
    }
}

void TextEditState::moveCursorEnd(bool extendSelection) {
    size_t oldPos = cursorPos;
    cursorPos = text.size();
    if (extendSelection) {
        if (!hasSelection) {
            selectionStart = oldPos;
            hasSelection = true;
        }
        selectionEnd = text.size();
        if (selectionStart == selectionEnd) hasSelection = false;
    } else {
        hasSelection = false;
    }
}

void TextEditState::selectAll() {
    selectionStart = 0;
    selectionEnd = text.size();
    cursorPos = text.size();
    hasSelection = (text.size() > 0);
}

void TextEditState::clearSelection() {
    hasSelection = false;
    selectionStart = selectionEnd = cursorPos;
}

void TextEditState::setCursorFromX(float x, float textStartX,
    std::function<float(const std::string&, size_t)> measureFunc) {
    if (text.empty()) {
        cursorPos = 0;
        return;
    }
    float relX = x - textStartX;
    if (relX <= 0) { cursorPos = 0; return; }

    // بحث ثنائي (binary search) على موقع الحرف
    size_t best = 0;
    float bestDist = relX;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t next = nextCharPos(pos);
        float w = measureFunc(text, next);
        float dist = std::abs(w - relX);
        if (dist < bestDist) {
            bestDist = dist;
            best = next;
        }
        if (w > relX) break;
        pos = next;
    }
    cursorPos = best;
}

// ═══════════════════════════════════════════════════════════════════════════════
// UndoRedoManager — التراجع/الإعادة
// ═══════════════════════════════════════════════════════════════════════════════

void UndoRedoManager::pushAction(const TextEditAction& action) {
    // مسح redo عند أي عملية جديدة
    redoStack_.clear();
    undoStack_.push_back(action);
    // تحديد حجم المكدس
    if (undoStack_.size() > MAX_UNDO_SIZE) {
        undoStack_.erase(undoStack_.begin());
    }
}

bool UndoRedoManager::undo(std::string& outText, size_t& outCursor) {
    if (undoStack_.empty()) return false;
    auto action = undoStack_.back();
    undoStack_.pop_back();
    redoStack_.push_back(action);
    outText = action.textBefore;
    outCursor = action.cursorBefore;
    return true;
}

bool UndoRedoManager::redo(std::string& outText, size_t& outCursor) {
    if (redoStack_.empty()) return false;
    auto action = redoStack_.back();
    redoStack_.pop_back();
    undoStack_.push_back(action);
    outText = action.textAfter;
    outCursor = action.cursorAfter;
    return true;
}

void UndoRedoManager::clear() {
    undoStack_.clear();
    redoStack_.clear();
}

// ═══════════════════════════════════════════════════════════════════════════════
// SadSyntaxHighlighter — تلوين نحوي للغة ص
// ═══════════════════════════════════════════════════════════════════════════════

bool SadSyntaxHighlighter::isIdentStart(unsigned char c, const std::string& text, size_t pos) {
    // حرف ASCII أو _ أو حرف عربي (UTF-8 يبدأ بـ 0xD8 أو 0xD9)
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') return true;
    if (c == 0xD8 || c == 0xD9) return true; // حروف عربية
    if (c >= 0xC0 && (c & 0xE0) == 0xC0) return true; // UTF-8 2-byte
    if (c >= 0xE0 && (c & 0xF0) == 0xE0) return true; // UTF-8 3-byte
    return false;
}

bool SadSyntaxHighlighter::isIdentPart(unsigned char c, const std::string& text, size_t pos) {
    if (isIdentStart(c, text, pos)) return true;
    if (c >= '0' && c <= '9') return true;
    // التشكيل العربي
    if ((c & 0xC0) == 0x80) return true; // continuation byte
    return false;
}

std::string SadSyntaxHighlighter::extractIdent(const std::string& text, size_t& pos) {
    size_t start = pos;
    while (pos < text.size()) {
        unsigned char c = static_cast<unsigned char>(text[pos]);
        if (!isIdentPart(c, text, pos)) break;
        if ((c & 0x80) == 0) { ++pos; continue; }
        // multi-byte UTF-8
        size_t len = 1;
        if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        pos += len;
    }
    return text.substr(start, pos - start);
}

SyntaxTokenType SadSyntaxHighlighter::classifyIdent(const std::string& ident) {
    // الكلمات المحجوزة (40)
    static const std::unordered_set<std::string> keywords = {
        "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9",             // دالة
        "\xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9",             // ارجع
        "\xd8\xb5\xd9\x86\xd9\x81",                       // صنف
        "\xd8\xa8\xd9\x86\xd9\x8a\xd8\xa9",             // بنية
        "\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf",     // تعداد
        "\xd9\x8a\xd8\xb1\xd8\xab",                       // يرث
        "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9",     // نهاية
        "\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf",             // جديد
        "\xd9\x87\xd8\xb0\xd8\xa7",                       // هذا
        "\xd8\xa8\xd8\xa7\xd9\x86\xd9\x8a",             // باني
        "\xd8\xa7\xd9\x84\xd8\xa3\xd8\xb3\xd8\xa7\xd8\xb3",  // الأساس
        "\xd8\xa5\xd8\xb0\xd8\xa7",                       // إذا
        "\xd8\xa7\xd8\xb0\xd8\xa7",                       // اذا
        "\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7",             // وإلا
        "\xd9\x88\xd8\xa7\xd9\x84\xd8\xa7",             // والا
        "\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7",     // بينما
        "\xd9\x84\xd9\x83\xd9\x84",                       // لكل
        "\xd9\x81\xd9\x8a",                               // في
        "\xd8\xaa\xd9\x88\xd9\x82\xd9\x81",             // توقف
        "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1",     // استمر
        "\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82",             // طابق
        "\xd8\xb9\xd9\x86\xd8\xaf\xd9\x85\xd8\xa7",     // عندما
        "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xb1\xd8\xa7\xd8\xb6\xd9\x8a",  // افتراضي
        "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84",             // حاول
        "\xd8\xa7\xd9\x85\xd8\xb3\xd9\x83",             // امسك
        "\xd8\xa7\xd8\xb1\xd9\x85\xd9\x8a",             // ارمي
        "\xd8\xa3\xd8\xae\xd9\x8a\xd8\xb1\xd8\xa7\xd9\x8b",  // أخيراً
        "\xd8\xb9\xd8\xa7\xd9\x85",                       // عام
        "\xd8\xae\xd8\xa7\xd8\xb5",                       // خاص
        "\xd9\x85\xd8\xad\xd9\x85\xd9\x8a",             // محمي
        "\xd9\x85\xd8\xac\xd8\xb1\xd8\xaf",             // مجرد
        "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf",  // استورد
        "\xd9\x85\xd9\x86",                               // من
        "\xd9\x83\xd9\x80",                               // كـ
        "\xd8\xb5\xd8\xaf\xd9\x91\xd8\xb1",             // صدّر
        "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1",     // متغير
        "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa",             // ثابت
        "\xd8\xb3\xd8\xa7\xd9\x83\xd9\x86",             // ساكن
        "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a",     // خارجي
    };

    // القيم المنطقية والعدم
    static const std::unordered_set<std::string> booleans = {
        "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad",             // صحيح
        "\xd8\xae\xd8\xb7\xd8\xa3",                       // خطأ
    };

    static const std::unordered_set<std::string> nullLiterals = {
        "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1",     // لاشيء
    };

    // الكلمات السياقية
    static const std::unordered_set<std::string> contextKeywords = {
        "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",  // غير_متزامن
        "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1",     // انتظر
        "\xd9\x84\xd8\xa7\xd9\x85\xd8\xaf\xd8\xa7",     // لامدا
        "\xd8\xb3\xd9\x85\xd8\xa9",                       // سمة
        "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9",     // واجهة
        "\xd9\x86\xd9\x81\xd9\x91\xd8\xb0",             // نفّذ
        "\xd9\x86\xd9\x81\xd8\xb0",                       // نفذ
        "\xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9",     // خاصية
        "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84",             // احصل
        "\xd8\xb9\xd9\x8a\xd9\x91\xd9\x86",             // عيّن
        "\xd9\x87\xd8\xaf\xd9\x85",                       // هدم
        "\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84",             // عامل
        "\xd8\xb1\xd8\xa6\xd9\x8a\xd8\xb3\xd9\x8a\xd8\xa9",  // رئيسية
    };

    // العوامل المنطقية
    static const std::unordered_set<std::string> logicalOps = {
        "\xd9\x88",                                       // و
        "\xd8\xa3\xd9\x88",                               // أو
        "\xd9\x84\xd9\x8a\xd8\xb3",                       // ليس
    };

    // أسماء الأنواع المدمجة
    static const std::unordered_set<std::string> typeNames = {
        "\xd8\xb1\xd9\x82\xd9\x85",                       // رقم
        "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a",             // عشري
        "\xd9\x86\xd8\xb5",                               // نص
        "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a",     // منطقي
        "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba",             // فراغ
        "\xd8\xb9\xd8\xaf\xd9\x85",                       // عدم
        "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9",  // مصفوفة
        "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9",     // خريطة
        "\xd8\xa3\xd9\x8a",                               // أي
    };

    // الدوال المدمجة
    static const std::unordered_set<std::string> builtins = {
        "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9",             // اطبع
        "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9_\xd8\xb3\xd8\xb7\xd8\xb1",  // اطبع_سطر
        "\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3",             // اقرأ
        "\xd8\xb7\xd9\x88\xd9\x84",                       // طول
        "\xd9\x86\xd9\x88\xd8\xb9",                       // نوع
    };

    if (keywords.count(ident)) return SyntaxTokenType::Keyword;
    if (logicalOps.count(ident)) return SyntaxTokenType::Keyword;
    if (booleans.count(ident)) return SyntaxTokenType::Boolean;
    if (nullLiterals.count(ident)) return SyntaxTokenType::NullLiteral;
    if (contextKeywords.count(ident)) return SyntaxTokenType::ContextKeyword;
    if (typeNames.count(ident)) return SyntaxTokenType::TypeName;
    if (builtins.count(ident)) return SyntaxTokenType::Builtin;
    return SyntaxTokenType::Normal;
}

std::vector<SyntaxToken> SadSyntaxHighlighter::tokenize(const std::string& text) const {
    std::vector<SyntaxToken> tokens;
    size_t i = 0;

    while (i < text.size()) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        // تخطي المسافات
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            ++i;
            continue;
        }

        // تعليق سطري #
        if (c == '#') {
            size_t start = i;
            // تعليق كتلة #* ... *#
            if (i + 1 < text.size() && text[i + 1] == '*') {
                i += 2;
                while (i + 1 < text.size()) {
                    if (text[i] == '*' && text[i + 1] == '#') {
                        i += 2;
                        break;
                    }
                    ++i;
                }
                if (i >= text.size()) i = text.size();
                tokens.push_back({start, i - start, SyntaxTokenType::Comment});
                continue;
            }
            // تعليق سطري
            while (i < text.size() && text[i] != '\n') ++i;
            tokens.push_back({start, i - start, SyntaxTokenType::Comment});
            continue;
        }

        // نصوص حرفية "..." أو '...'
        if (c == '"' || c == '\'') {
            size_t start = i;
            char quote = static_cast<char>(c);
            ++i;
            while (i < text.size() && text[i] != quote) {
                if (text[i] == '\\' && i + 1 < text.size()) ++i; // تخطي escaped
                ++i;
            }
            if (i < text.size()) ++i; // تخطي قوس الإغلاق
            tokens.push_back({start, i - start, SyntaxTokenType::String});
            continue;
        }

        // أرقام
        if (c >= '0' && c <= '9') {
            size_t start = i;
            while (i < text.size()) {
                unsigned char d = static_cast<unsigned char>(text[i]);
                if ((d >= '0' && d <= '9') || d == '.' || d == '_') ++i;
                else break;
            }
            tokens.push_back({start, i - start, SyntaxTokenType::Number});
            continue;
        }

        // مُعرّفات (عربية أو لاتينية)
        if (isIdentStart(c, text, i)) {
            size_t start = i;
            std::string ident = extractIdent(text, i);
            SyntaxTokenType type = classifyIdent(ident);
            tokens.push_back({start, i - start, type});
            continue;
        }

        // أقواس
        if (c == '(' || c == ')' || c == '[' || c == ']') {
            tokens.push_back({i, 1, SyntaxTokenType::Bracket});
            ++i;
            continue;
        }

        // عوامل
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
            c == '=' || c == '!' || c == '<' || c == '>' || c == '&' ||
            c == '|' || c == '^' || c == '~' || c == '?' || c == ':') {
            size_t start = i;
            ++i;
            // عوامل مزدوجة (==, !=, <=, >=, &&, ||, **, ...)
            if (i < text.size()) {
                char next = text[i];
                if ((c == '=' && next == '=') || (c == '!' && next == '=') ||
                    (c == '<' && next == '=') || (c == '>' && next == '=') ||
                    (c == '&' && next == '&') || (c == '|' && next == '|') ||
                    (c == '*' && next == '*') || (c == '+' && next == '=') ||
                    (c == '-' && next == '=') || (c == '*' && next == '=') ||
                    (c == '/' && next == '=')) {
                    ++i;
                }
            }
            tokens.push_back({start, i - start, SyntaxTokenType::Operator});
            continue;
        }

        // الفاصلة العربية ، أو الفاصلة اللاتينية
        if (c == ',' || (c == 0xD8 && i + 1 < text.size() && 
            static_cast<unsigned char>(text[i + 1]) == 0x8C)) {
            // الفاصلة العربية ، = 0xD8 0x8C
            size_t start = i;
            if (c == 0xD8) i += 2; else ++i;
            tokens.push_back({start, i - start, SyntaxTokenType::Operator});
            continue;
        }

        // أي شيء آخر — تخطي
        ++i;
    }

    return tokens;
}

SadSyntaxHighlighter::TokenColor SadSyntaxHighlighter::getColor(SyntaxTokenType type, bool darkTheme) {
    if (darkTheme) {
        switch (type) {
            case SyntaxTokenType::Keyword:        return {0.78f, 0.47f, 0.99f, 1.0f};  // بنفسجي فاتح
            case SyntaxTokenType::ContextKeyword:  return {0.78f, 0.47f, 0.85f, 1.0f};  // بنفسجي
            case SyntaxTokenType::TypeName:        return {0.31f, 0.82f, 0.82f, 1.0f};  // سماوي
            case SyntaxTokenType::String:          return {0.81f, 0.54f, 0.31f, 1.0f};  // برتقالي
            case SyntaxTokenType::Number:          return {0.71f, 0.84f, 0.43f, 1.0f};  // أخضر فاتح
            case SyntaxTokenType::Comment:         return {0.45f, 0.51f, 0.42f, 1.0f};  // رمادي أخضر
            case SyntaxTokenType::Operator:        return {0.86f, 0.86f, 0.86f, 1.0f};  // رمادي فاتح
            case SyntaxTokenType::Bracket:         return {1.00f, 0.84f, 0.40f, 1.0f};  // ذهبي
            case SyntaxTokenType::Builtin:         return {0.38f, 0.68f, 0.94f, 1.0f};  // أزرق
            case SyntaxTokenType::Boolean:         return {0.34f, 0.71f, 0.91f, 1.0f};  // أزرق فاتح
            case SyntaxTokenType::NullLiteral:     return {0.34f, 0.71f, 0.91f, 1.0f};  // أزرق فاتح
            default:                               return {0.86f, 0.86f, 0.86f, 1.0f};  // أبيض
        }
    } else {
        switch (type) {
            case SyntaxTokenType::Keyword:        return {0.51f, 0.11f, 0.68f, 1.0f};  // بنفسجي
            case SyntaxTokenType::ContextKeyword:  return {0.51f, 0.11f, 0.55f, 1.0f};
            case SyntaxTokenType::TypeName:        return {0.00f, 0.50f, 0.50f, 1.0f};  // سماوي غامق
            case SyntaxTokenType::String:          return {0.64f, 0.08f, 0.08f, 1.0f};  // أحمر قاتم
            case SyntaxTokenType::Number:          return {0.02f, 0.45f, 0.02f, 1.0f};  // أخضر
            case SyntaxTokenType::Comment:         return {0.42f, 0.48f, 0.39f, 1.0f};  // رمادي أخضر
            case SyntaxTokenType::Operator:        return {0.20f, 0.20f, 0.20f, 1.0f};  // أسود تقريباً
            case SyntaxTokenType::Bracket:         return {0.30f, 0.30f, 0.00f, 1.0f};  // بني غامق
            case SyntaxTokenType::Builtin:         return {0.00f, 0.30f, 0.60f, 1.0f};  // أزرق
            case SyntaxTokenType::Boolean:         return {0.00f, 0.00f, 0.80f, 1.0f};  // أزرق
            case SyntaxTokenType::NullLiteral:     return {0.00f, 0.00f, 0.80f, 1.0f};
            default:                               return {0.10f, 0.10f, 0.10f, 1.0f};  // أسود
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// TextEditStateMap — خريطة ربط الحالات
// ═══════════════════════════════════════════════════════════════════════════════

TextEditState& TextEditStateMap::getState(const ::sad::ui::IRNode* node) {
    return states_[node].state;
}

UndoRedoManager& TextEditStateMap::getUndoManager(const ::sad::ui::IRNode* node) {
    return states_[node].undoManager;
}

void TextEditStateMap::clearState(const ::sad::ui::IRNode* node) {
    states_.erase(node);
}

void TextEditStateMap::clearAll() {
    states_.clear();
}

} // namespace desktop
} // namespace ui
} // namespace sad
