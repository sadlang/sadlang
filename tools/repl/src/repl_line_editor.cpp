/**
 * =============================================================================
 * ملف: repl_line_editor.cpp
 * الوصف: تنفيذ مكوّنات الإدخال الغنيّ — المُكمِّل التلقائي ومحرّر السطر التفاعلي
 * (AR) @brief تنفيذ محرر السطر التفاعلي والإكمال التلقائي (يستعملهما REPLEngine)
 * (EN) @brief Interactive line editor and auto-completion (consumed by REPLEngine)
 * =============================================================================
 */

#include "repl_line_editor.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#ifdef VOID
#undef VOID
#endif

namespace Sad {
namespace REPL {

// ============================================================================
// AutoCompleter — الإكمال التلقائي
// ============================================================================

AutoCompleter::AutoCompleter() {
    initKeywords();
    initBuiltins();
    initTypes();
}

void AutoCompleter::initKeywords() {
    keywords_ = {
        // كلمات محجوزة (40)
        "دالة", "ارجع", "صنف", "بنية", "تعداد", "يرث", "نهاية",
        "جديد", "هذا", "باني", "الأساس",
        "إذا", "وإلا", "بينما", "لكل", "في", "توقف", "استمر",
        "طابق", "عندما", "افتراضي",
        "حاول", "امسك", "ارمي", "أخيراً",
        "عام", "خاص", "محمي", "مجرد",
        "استورد", "من", "كـ", "صدّر",
        "متغير", "ثابت", "ساكن", "خارجي",
        "صحيح", "خطأ", "لاشيء",
        // عوامل منطقية
        "و", "أو", "ليس",
        // كلمات سياقية
        "غير_متزامن", "انتظر", "لامدا", "أنتج", "مولد",
        "باستخدام", "نهاية_استخدام",
        "سمة", "واجهة", "نفّذ", "نفذ", "قالب",
        "فضاء", "نهاية_فضاء",
        "اختبر", "خاصية", "احصل", "عيّن", "هدم", "عامل", "رئيسية", "حالة"
    };
}

void AutoCompleter::initBuiltins() {
    builtins_ = {
        "اطبع", "اطبع_سطر", "طول", "نوع", "حول",
        "ادخل", "ادخل_رقم", "ادخل_عشري",
        "قائمة", "خريطة_جديدة", "نطاق",
        "جذر", "مطلق", "عشوائي", "قوة", "سقف", "أرضية",
        "وقت", "ساعة", "دقيقة", "ثانية",
    };
}

void AutoCompleter::initTypes() {
    types_ = {
        "رقم", "عشري", "نص", "منطقي", "فراغ", "عدم",
        "مصفوفة", "خريطة", "أي"
    };
}

std::vector<std::string> AutoCompleter::complete(const std::string& prefix) const {
    if (prefix.empty()) return {};
    
    std::vector<std::string> matches;
    
    auto check = [&](const std::string& candidate) {
        if (candidate.size() >= prefix.size() &&
            candidate.compare(0, prefix.size(), prefix) == 0) {
            matches.push_back(candidate);
        }
    };
    
    for (const auto& kw : keywords_) check(kw);
    for (const auto& bi : builtins_) check(bi);
    for (const auto& ty : types_) check(ty);
    for (const auto& ui : userIdentifiers_) check(ui);
    
    std::sort(matches.begin(), matches.end());
    matches.erase(std::unique(matches.begin(), matches.end()), matches.end());
    
    return matches;
}

void AutoCompleter::addUserIdentifier(const std::string& name) {
    userIdentifiers_.insert(name);
}

void AutoCompleter::clearUserIdentifiers() {
    userIdentifiers_.clear();
}

// ============================================================================
// LineEditor — محرر السطر التفاعلي
// ============================================================================

LineEditor::LineEditor(HistoryManager* history, AutoCompleter* completer)
    : history_(history), completer_(completer) {}

std::string LineEditor::readLine(const std::string& prompt) {
    buffer_.clear();
    cursorPos_ = 0;
    completionIndex_ = -1;
    lastCompletions_.clear();
    eof_ = false;
    
    std::cout << prompt;
    std::cout.flush();
    
    enableRawMode();
    
    while (true) {
        int ch = readChar();
        
        if (ch < 0) {
            // EOF
            eof_ = true;
            disableRawMode();
            return "";
        }
        
        if (ch == '\n' || ch == '\r') {
            std::cout << "\n";
            disableRawMode();
            return buffer_;
        }
        
        if (ch == 4) { // Ctrl+D
            if (buffer_.empty()) {
                eof_ = true;
                disableRawMode();
                return "";
            }
            handleDelete();
            refreshLine(prompt);
            continue;
        }
        
        if (ch == 3) { // Ctrl+C
            buffer_.clear();
            cursorPos_ = 0;
            std::cout << "\n";
            disableRawMode();
            return "";
        }
        
        if (ch == '\t') {
            handleTab();
            refreshLine(prompt);
            continue;
        }
        
        if (ch == 127 || ch == 8) { // Backspace
            handleBackspace();
            refreshLine(prompt);
            continue;
        }
        
        if (ch == 1) { // Ctrl+A = Home
            handleHome();
            refreshLine(prompt);
            continue;
        }
        
        if (ch == 5) { // Ctrl+E = End
            handleEnd();
            refreshLine(prompt);
            continue;
        }
        
        if (ch == 12) { // Ctrl+L = Clear screen
            std::cout << "\033[2J\033[H";
            refreshLine(prompt);
            continue;
        }
        
#ifdef _WIN32
        if (ch == 224 || ch == 0) { // Extended key on Windows
            int ext = readChar();
            switch (ext) {
                case 72: handleArrowKey(0); break; // Up
                case 80: handleArrowKey(1); break; // Down
                case 75: handleArrowKey(2); break; // Left
                case 77: handleArrowKey(3); break; // Right
                case 71: handleHome(); break;       // Home
                case 79: handleEnd(); break;        // End
                case 83: handleDelete(); break;     // Delete
            }
            refreshLine(prompt);
            continue;
        }
#else
        if (ch == 27) { // Escape sequence
            int next1 = readChar();
            if (next1 == '[') {
                int next2 = readChar();
                switch (next2) {
                    case 'A': handleArrowKey(0); break; // Up
                    case 'B': handleArrowKey(1); break; // Down
                    case 'D': handleArrowKey(2); break; // Left
                    case 'C': handleArrowKey(3); break; // Right
                    case 'H': handleHome(); break;       // Home
                    case 'F': handleEnd(); break;        // End
                    case '3': {
                        int next3 = readChar();
                        if (next3 == '~') handleDelete(); // Delete
                        break;
                    }
                }
            }
            refreshLine(prompt);
            continue;
        }
#endif
        
        // Normal character (including UTF-8 multi-byte)
        handleNormalChar(ch);
        refreshLine(prompt);
    }
}

int LineEditor::readChar() {
#ifdef _WIN32
    int ch = _getch();
    if (ch == EOF) return -1;
    return ch;
#else
    unsigned char c;
    int n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) return -1;
    return c;
#endif
}

void LineEditor::handleArrowKey(int key) {
    switch (key) {
        case 0: // Up - previous history
            if (history_) {
                std::string prev = history_->previous();
                if (!prev.empty()) {
                    buffer_ = prev;
                    cursorPos_ = buffer_.size();
                }
            }
            break;
        case 1: // Down - next history
            if (history_) {
                std::string nxt = history_->next();
                buffer_ = nxt;
                cursorPos_ = buffer_.size();
            }
            break;
        case 2: // Left
            if (cursorPos_ > 0) {
                // Handle UTF-8: skip continuation bytes
                do {
                    cursorPos_--;
                } while (cursorPos_ > 0 && 
                         (buffer_[cursorPos_] & 0xC0) == 0x80);
            }
            break;
        case 3: // Right
            if (cursorPos_ < buffer_.size()) {
                // Handle UTF-8: skip continuation bytes
                cursorPos_++;
                while (cursorPos_ < buffer_.size() && 
                       (buffer_[cursorPos_] & 0xC0) == 0x80) {
                    cursorPos_++;
                }
            }
            break;
    }
}

void LineEditor::handleTab() {
    // Extract the current word (from last space/newline to cursor)
    size_t wordStart = cursorPos_;
    while (wordStart > 0) {
        size_t prev = wordStart - 1;
        // Skip UTF-8 continuation bytes backward
        while (prev > 0 && (buffer_[prev] & 0xC0) == 0x80) prev--;
        char c = buffer_[prev];
        if (c == ' ' || c == '\t' || c == '(' || c == ')' || 
            c == '[' || c == ']' || c == ',' || c == ';' ||
            c == '\xD8' || c == '\xD9') {
            // Check if it's a space-like separator
            // For Arabic UTF-8, first bytes are 0xD8/0xD9 — these are word chars
            if (c == ' ' || c == '\t' || c == '(' || c == ')' || 
                c == '[' || c == ']' || c == ',' || c == ';') {
                break;
            }
        }
        wordStart = prev;
    }
    
    std::string prefix = buffer_.substr(wordStart, cursorPos_ - wordStart);
    
    if (prefix.empty()) return;
    
    auto matches = completer_->complete(prefix);
    
    if (matches.empty()) return;
    
    if (matches.size() == 1) {
        // Single match — complete it
        std::string completion = matches[0].substr(prefix.size());
        buffer_.insert(cursorPos_, completion);
        cursorPos_ += completion.size();
        completionIndex_ = -1;
    } else {
        // Multiple matches — cycle through them
        completionIndex_++;
        if (completionIndex_ >= static_cast<int>(matches.size())) {
            completionIndex_ = 0;
        }
        
        if (completionIndex_ == 0 && lastCompletions_ == matches) {
            // Show all options
            std::cout << "\n";
            for (size_t i = 0; i < matches.size(); i++) {
                if (i > 0) std::cout << "  ";
                std::cout << matches[i];
            }
            std::cout << "\n";
        } else {
            // Replace prefix with match
            buffer_.erase(wordStart, cursorPos_ - wordStart);
            buffer_.insert(wordStart, matches[completionIndex_]);
            cursorPos_ = wordStart + matches[completionIndex_].size();
        }
        
        lastCompletions_ = matches;
    }
}

void LineEditor::handleBackspace() {
    if (cursorPos_ > 0) {
        size_t deleteFrom = cursorPos_ - 1;
        // Handle UTF-8
        while (deleteFrom > 0 && (buffer_[deleteFrom] & 0xC0) == 0x80) {
            deleteFrom--;
        }
        buffer_.erase(deleteFrom, cursorPos_ - deleteFrom);
        cursorPos_ = deleteFrom;
    }
}

void LineEditor::handleDelete() {
    if (cursorPos_ < buffer_.size()) {
        size_t deleteTo = cursorPos_ + 1;
        // Handle UTF-8
        while (deleteTo < buffer_.size() && (buffer_[deleteTo] & 0xC0) == 0x80) {
            deleteTo++;
        }
        buffer_.erase(cursorPos_, deleteTo - cursorPos_);
    }
}

void LineEditor::handleHome() {
    cursorPos_ = 0;
}

void LineEditor::handleEnd() {
    cursorPos_ = buffer_.size();
}

void LineEditor::handleNormalChar(int ch) {
    completionIndex_ = -1;
    
#ifdef _WIN32
    // On Windows _getch returns bytes; for UTF-8, read continuation bytes
    std::string utf8;
    utf8 += static_cast<char>(ch);
    
    if ((ch & 0x80) != 0) {
        // Multi-byte UTF-8
        int extraBytes = 0;
        if ((ch & 0xE0) == 0xC0) extraBytes = 1;
        else if ((ch & 0xF0) == 0xE0) extraBytes = 2;
        else if ((ch & 0xF8) == 0xF0) extraBytes = 3;
        
        for (int i = 0; i < extraBytes; i++) {
            int next = readChar();
            if (next >= 0) utf8 += static_cast<char>(next);
        }
    }
    
    buffer_.insert(cursorPos_, utf8);
    cursorPos_ += utf8.size();
#else
    std::string utf8;
    utf8 += static_cast<char>(ch);
    
    if ((ch & 0x80) != 0) {
        int extraBytes = 0;
        if ((ch & 0xE0) == 0xC0) extraBytes = 1;
        else if ((ch & 0xF0) == 0xE0) extraBytes = 2;
        else if ((ch & 0xF8) == 0xF0) extraBytes = 3;
        
        for (int i = 0; i < extraBytes; i++) {
            int next = readChar();
            if (next >= 0) utf8 += static_cast<char>(next);
        }
    }
    
    buffer_.insert(cursorPos_, utf8);
    cursorPos_ += utf8.size();
#endif
}

void LineEditor::refreshLine(const std::string& prompt) {
    // Move cursor to beginning, clear line, reprint
    std::cout << "\r\033[K" << prompt << buffer_;
    
    // Move cursor to correct position
    if (cursorPos_ < buffer_.size()) {
        size_t charsAfter = displayWidth(buffer_.substr(cursorPos_));
        if (charsAfter > 0) {
            std::cout << "\033[" << charsAfter << "D";
        }
    }
    std::cout.flush();
}

void LineEditor::clearLine() {
    std::cout << "\r\033[K";
    std::cout.flush();
}

size_t LineEditor::displayWidth(const std::string& s) const {
    size_t width = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = s[i];
        if ((c & 0x80) == 0) { width++; i++; }         // ASCII
        else if ((c & 0xE0) == 0xC0) { width++; i += 2; } // 2-byte
        else if ((c & 0xF0) == 0xE0) { width += 2; i += 3; } // 3-byte (Arabic/CJK)
        else if ((c & 0xF8) == 0xF0) { width += 2; i += 4; } // 4-byte
        else { width++; i++; }
    }
    return width;
}

void LineEditor::enableRawMode() {
#ifdef _WIN32
    // On Windows, _getch already works in raw mode
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
#else
    struct termios raw;
    tcgetattr(STDIN_FILENO, &origTermios_);
    raw = origTermios_;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    rawModeEnabled_ = true;
#endif
}

void LineEditor::disableRawMode() {
#ifdef _WIN32
    // Nothing to restore on Windows
#else
    if (rawModeEnabled_) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios_);
        rawModeEnabled_ = false;
    }
#endif
}

} // namespace REPL
} // namespace Sad
