/**
 * =============================================================================
 * ملف: repl_line_editor.cpp
 * الوصف: تنفيذ مكوّنات الإدخال الغنيّ — المُكمِّل التلقائي ومحرّر السطر التفاعلي
 * (AR) @brief تنفيذ محرر السطر التفاعلي والإكمال التلقائي (يستعملهما REPLEngine)
 * (EN) @brief Interactive line editor and auto-completion (consumed by REPLEngine)
 * =============================================================================
 */

#include "repl_line_editor.h"
// (AR) كتالوج SoT المولَّد: أوامر REPL (kCommands) ومعجم الموزِّع العربيّ (kApplets) —
//      مصدر الإكمال السياقيّ بمراجع حيّة لا نسخٍ يدويّة (تتحدّث تلقائيًّا مع إعادة التوليد).
// (EN) Generated Tool-SoT catalog: REPL commands (kCommands) + Arabic dispatcher lexicon
//      (kApplets) — context-completion source by live reference, not hand copies.
#include "repl_sot_generated.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
// (AR) إكمال مسارات الملفّات (opendir) — POSIX فقط بعزل منصّيّ معلَن (الشريحة الأولى
//      تستهدف sad-os/لينكس؛ ويندوز يسقط لقوائم اللغة العامّة بلا كسر).
// (EN) File-path completion (opendir) — POSIX only by declared platform isolation
//      (first slice targets sad-os/Linux; Windows falls back to the generic lists).
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef VOID
#undef VOID
#endif

namespace Sad {
namespace REPL {

namespace {

// ── ثوابت بنيويّة مسمّاة (لا سلاسل خام مبعثرة) / named structural constants ──
constexpr char kCommandSigil = ':';           // بادئة أوامر REPL / REPL command sigil
constexpr char kPathSeparator = '/';          // فاصل المسارات POSIX / POSIX path separator
constexpr const char* kCurrentDirPrefix = "./"; // بادئة المسار النسبيّ / relative-path prefix

// (AR) هل تبدو الكلمة مسار ملفّ؟ (تبدأ بـ«/» أو «./») / does the word look like a file path?
bool looksLikeFilePath(const std::string& word) {
    if (word.empty()) return false;
    if (word[0] == kPathSeparator) return true;
    return word.compare(0, 2, kCurrentDirPrefix) == 0;
}

// (AR) هل الاسم (بلا «:») هو أمر التشغيل؟ — بالإحالة الحيّة لـSoT (handler == RUN)
// (EN) is this (sigil-less) name the run command? — live SoT reference (handler == RUN)
bool isRunCommandName(const std::string& name) {
    for (std::size_t i = 0; i < SoT::kCommandsCount; ++i) {
        if (SoT::kCommands[i].handler != SoT::CommandHandler::RUN) continue;
        if (name == SoT::kCommands[i].name) return true;
        if (SoT::kCommands[i].arabicName && name == SoT::kCommands[i].arabicName) return true;
    }
    return false;
}

void sortUnique(std::vector<std::string>& v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
}

} // namespace

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

std::vector<std::string> AutoCompleter::completeInContext(const std::string& buffer,
                                                          size_t wordStart,
                                                          const std::string& prefix) const {
    if (prefix.empty()) return {};

    // (١) مسار ملفّ صريح («/» أو «./») — بصرف النظر عن موضع الكلمة:
    if (looksLikeFilePath(prefix)) {
        auto matches = completeFilePaths(prefix);
        sortUnique(matches);
        return matches;
    }

    const size_t firstNonSpace = buffer.find_first_not_of(" \t");
    const bool colonLine = firstNonSpace != std::string::npos &&
                           buffer[firstNonSpace] == kCommandSigil;
    if (colonLine) {
        std::vector<std::string> matches;
        if (wordStart <= firstNonSpace) {
            // (٢) الكلمة الأولى «:بادئة» — أوامر REPL (إنجليزيّ+عربيّ) + أسماء الموزِّع
            //     العربيّة (تصحّ مباشرةً بعد «:» — راجع REPLCommands::process):
            completeColonCommands(prefix, matches);
            sortUnique(matches);
            return matches;
        }
        // موضع وسيط: حدّد اسم الأمر (الكلمة الأولى بلا «:») وأوّل وسائطه.
        const size_t cmdEnd = buffer.find_first_of(" \t", firstNonSpace);
        const std::string cmdName = buffer.substr(
            firstNonSpace + 1,
            cmdEnd == std::string::npos ? std::string::npos
                                        : cmdEnd - firstNonSpace - 1);
        const size_t firstArgStart =
            cmdEnd == std::string::npos ? std::string::npos
                                        : buffer.find_first_not_of(" \t", cmdEnd);
        // (٣) الوسيط الأوّل بعد أمر التشغيل (:run/:شغّل) ⇒ أسماء applets العربيّة:
        if (firstArgStart == wordStart && isRunCommandName(cmdName)) {
            completeAppletNames(prefix, matches);
            sortUnique(matches);
            return matches;
        }
        // وسائط أخرى على سطر «:» لا نملك لها مصدرًا سياقيًّا ⇒ بلا اقتراح مضلِّل.
        return {};
    }

    // (٤) سطر لغة ص عاديّ ⇒ القوائم القائمة (كلمات/مدمجات/أنواع/معرّفات المستخدم):
    return complete(prefix);
}

void AutoCompleter::completeColonCommands(const std::string& prefix,
                                          std::vector<std::string>& out) const {
    // prefix يتضمّن «:» (ليست فاصل كلمات في المحرّر) — المرشَّح يُبنى بها ويُطابَق بها.
    auto add = [&](const char* name) {
        if (!name) return;
        const std::string candidate = std::string(1, kCommandSigil) + name;
        if (candidate.size() >= prefix.size() &&
            candidate.compare(0, prefix.size(), prefix) == 0) {
            out.push_back(candidate);
        }
    };
    for (std::size_t i = 0; i < SoT::kCommandsCount; ++i) {
        add(SoT::kCommands[i].name);
        add(SoT::kCommands[i].arabicName);
    }
    for (std::size_t i = 0; i < SoT::kAppletsCount; ++i) {
        add(SoT::kApplets[i].arabic);
    }
}

void AutoCompleter::completeAppletNames(const std::string& prefix,
                                        std::vector<std::string>& out) const {
    for (std::size_t i = 0; i < SoT::kAppletsCount; ++i) {
        const char* arabic = SoT::kApplets[i].arabic;
        if (!arabic) continue;
        const std::string candidate(arabic);
        if (candidate.size() >= prefix.size() &&
            candidate.compare(0, prefix.size(), prefix) == 0) {
            out.push_back(candidate);
        }
    }
}

std::vector<std::string> AutoCompleter::completeFilePaths(const std::string& prefix) {
#ifdef _WIN32
    // (AR) حدّ معلَن: إكمال المسارات POSIX فقط في هذه الشريحة (هدفها sad-os).
    // (EN) Declared limit: path completion is POSIX-only in this slice (sad-os target).
    (void)prefix;
    return {};
#else
    std::vector<std::string> matches;
    const size_t lastSep = prefix.rfind(kPathSeparator);
    if (lastSep == std::string::npos) return {}; // لا يحدث: looksLikeFilePath يضمن «/»
    const std::string dirPart = prefix.substr(0, lastSep + 1); // يشمل «/» الأخيرة
    const std::string basePart = prefix.substr(lastSep + 1);
    DIR* dir = opendir(dirPart.c_str());
    if (!dir) return {};
    while (const dirent* entry = readdir(dir)) {
        const std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        if (name.size() < basePart.size() ||
            name.compare(0, basePart.size(), basePart) != 0) {
            continue;
        }
        std::string candidate = dirPart + name;
        // (AR) لاحقة «/» للأدلّة كي يتسلسل الإكمال طبقةً فطبقة (d_type قد يكون
        //      DT_UNKNOWN على بعض أنظمة الملفّات ⇒ يُحسم بـstat).
        // (EN) Trailing '/' for directories so completion chains level by level
        //      (d_type may be DT_UNKNOWN on some filesystems ⇒ resolve via stat).
        bool isDir = entry->d_type == DT_DIR;
        if (entry->d_type == DT_UNKNOWN) {
            struct stat st{};
            isDir = ::stat(candidate.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
        }
        if (isDir) candidate += kPathSeparator;
        matches.push_back(candidate);
    }
    closedir(dir);
    return matches;
#endif
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

    // (AR) فعّل الوضع الخام **قبل** طباعة المحثّ: لو طُبع المحثّ أوّلًا لبقيت نافذة
    //      يكون فيها ECHO/ICANON القانونيّان نشطَين، فأيّ إدخال سريع يصل بعد رؤية
    //      المحثّ (كتابة مسبقة بشريّة أو سلك تسلسليّ آليّ) يُصدّيه النواةُ سطرًا
    //      كاملًا بسطر جديد ثم يعيد المحرّرُ رسمَه ثانيةً — صدًى مزدوج وسطر محثّ
    //      شبحيّ يُربكان أيّ كاشف محثّ (رُصد حيًّا 2026-07-17 على سلك sad-os).
    //      بترتيب «خام ثم محثّ» لا يظهر المحثّ إلّا والصدى القانونيّ معطَّل فعلًا.
    // (EN) Enable raw mode BEFORE printing the prompt: printing first leaves a window
    //      with canonical ECHO/ICANON still active, so fast input arriving after the
    //      prompt is seen (human type-ahead or an automated serial wire) gets echoed
    //      by the kernel as a full line plus newline and then re-echoed by the editor
    //      redraw — double echo and a ghost prompt line that confuse any prompt
    //      detector (observed live 2026-07-17 on the sad-os wire). With raw-then-
    //      prompt ordering, the prompt only appears once canonical echo is off.
    enableRawMode();

    std::cout << prompt;
    std::cout.flush();
    
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

    // (AR) الإكمال السياقيّ: أوامر «:» + أسماء الموزِّع + applets بعد أمر التشغيل +
    //      مسارات الملفّات — مع السقوط لقوائم اللغة القائمة (راجع completeInContext).
    // (EN) Context-aware completion with fallback to the language lists.
    auto matches = completer_->completeInContext(buffer_, wordStart, prefix);
    
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

namespace {

// (AR) نطاقات codepoint بعرضٍ طرفيٍّ خاصّ — كانت المعادلة القديمة «كلّ 3-بايت = عرض 2»
//      خاطئةً للعربيّة (عرضها 1 على طرفيّات الخلايا الأحاديّة) فتنحرف حسبة موضع المؤشّر.
// (EN) Codepoint ranges with special terminal width — the old "every 3-byte char is
//      width 2" rule was wrong for Arabic (width 1 on monospace cells), skewing the
//      cursor-position math.
struct CodepointRange { char32_t lo; char32_t hi; };

// نطاقات RTL العربيّة الأساس ⇒ عرض 1 / core Arabic RTL ranges ⇒ width 1:
constexpr CodepointRange kArabicWidth1Ranges[] = {
    {0x0600, 0x06FF},  // العربيّة / Arabic
    {0x0750, 0x077F},  // ملحق العربيّة / Arabic Supplement
    {0x0870, 0x089F},  // العربيّة الموسَّعة-ب / Arabic Extended-B
    {0x08A0, 0x08FF},  // العربيّة الموسَّعة-أ / Arabic Extended-A
    {0xFB50, 0xFDFF},  // أشكال العرض-أ / Arabic Presentation Forms-A
    {0xFE70, 0xFEFF},  // أشكال العرض-ب / Arabic Presentation Forms-B
};

// علامات تحكّم اتّجاهيّة/صفريّة العرض ⇒ عرض 0 / zero-width direction controls ⇒ width 0:
constexpr CodepointRange kZeroWidthRanges[] = {
    {0x061C, 0x061C},  // علامة الحرف العربيّ ALM
    {0x200B, 0x200F},  // ZWSP/ZWNJ/ZWJ/LRM/RLM
    {0x202A, 0x202E},  // تضمينات bidi / bidi embeddings (LRE..RLO)
    {0x2066, 0x2069},  // عوازل bidi / bidi isolates (LRI..PDI)
};

bool inRanges(char32_t cp, const CodepointRange* ranges, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (cp >= ranges[i].lo && cp <= ranges[i].hi) return true;
    }
    return false;
}

} // namespace

size_t LineEditor::displayWidth(const std::string& s) const {
    // (AR) فكّ codepoint فعليّ من البايتات (بدل العدّ البايتيّ الأعمى) ثم قرار العرض:
    //      0 لعلامات التحكّم الصفريّة، 1 للعربيّة الأساس (النطاقات أعلاه)، والباقي على
    //      السلوك السابق (2-بايت=1، 3-بايت=2 كـCJK، 4-بايت=2).
    //      حدود معلَنة (ليست wcwidth كاملة — قرار الشريحة الأولى في مذكّرة التصميم):
    //      • الحركات/التشكيل العربيّ المتّحد (064B–065F وأخواته) يُحسب عرض 1 لا 0؛
    //      • لا معالجة لعرض شرق-آسيويّ مزدوج خارج افتراض 3-بايت القديم؛
    //      • UTF-8 المشوّه يسقط إلى بايت=عرض 1 (سلوك آمن لا انهيار).
    // (EN) Decode real codepoints (not blind byte counting) then decide width:
    //      0 for zero-width controls, 1 for core Arabic, otherwise the previous
    //      behavior. Declared limits (not a full wcwidth — first-slice decision):
    //      Arabic combining marks count as width 1, no East-Asian double-width
    //      handling beyond the legacy 3-byte assumption, malformed UTF-8 degrades
    //      to one column per byte.
    size_t width = 0;
    for (size_t i = 0; i < s.size(); ) {
        const unsigned char lead = s[i];
        size_t len = 1;
        char32_t cp = lead;
        if ((lead & 0x80) == 0)          { len = 1; cp = lead; }
        else if ((lead & 0xE0) == 0xC0)  { len = 2; cp = lead & 0x1F; }
        else if ((lead & 0xF0) == 0xE0)  { len = 3; cp = lead & 0x0F; }
        else if ((lead & 0xF8) == 0xF0)  { len = 4; cp = lead & 0x07; }
        else { width++; i++; continue; } // بايت متابعة يتيم / stray continuation byte

        if (i + len > s.size()) { width++; i++; continue; } // تسلسل مبتور / truncated
        bool malformed = false;
        for (size_t k = 1; k < len; ++k) {
            const unsigned char cont = s[i + k];
            if ((cont & 0xC0) != 0x80) { malformed = true; break; }
            cp = (cp << 6) | (cont & 0x3F);
        }
        if (malformed) { width++; i++; continue; }

        if (inRanges(cp, kZeroWidthRanges,
                     sizeof(kZeroWidthRanges) / sizeof(kZeroWidthRanges[0]))) {
            // عرض 0 / width 0
        } else if (inRanges(cp, kArabicWidth1Ranges,
                            sizeof(kArabicWidth1Ranges) / sizeof(kArabicWidth1Ranges[0]))) {
            width += 1;
        } else if (len >= 3) {
            width += 2; // السلوك السابق لـCJK وأشباهه / legacy CJK-ish assumption
        } else {
            width += 1; // ASCII و2-بايت / ASCII and 2-byte
        }
        i += len;
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
    // (AR) TCSADRAIN لا TCSAFLUSH: ‏TCSAFLUSH يُسقط طابور الإدخال غير المقروء، فأيّ
    //      بايتات تصل بين طباعة المحثّ وتفعيل الوضع الخام تُبتلَع صامتةً — يَفقِد
    //      الكتابةَ المسبقة (type-ahead) للمستخدم السريع، ورُصد حيًّا 2026-07-17 على
    //      سلك sad-os التسلسليّ كضياعِ أوامرَ بالتناوب. TCSADRAIN يُصرّف المخرَجات
    //      المعلَّقة فقط ويُبقي الإدخال المطابور سليمًا فيقرؤه المحرّر بعد التفعيل.
    // (EN) TCSADRAIN, not TCSAFLUSH: TCSAFLUSH discards the unread input queue, so
    //      bytes arriving between the prompt print and raw-mode enable are silently
    //      swallowed — losing user type-ahead (observed live 2026-07-17 on the
    //      sad-os serial wire as alternating command loss). TCSADRAIN only drains
    //      pending output and preserves queued input for the editor to read.
    tcsetattr(STDIN_FILENO, TCSADRAIN, &raw);
    rawModeEnabled_ = true;
#endif
}

void LineEditor::disableRawMode() {
#ifdef _WIN32
    // Nothing to restore on Windows
#else
    if (rawModeEnabled_) {
        // (AR) نفس تعليل TCSADRAIN أعلاه: لا نُسقط ما كتبه المستخدم مسبقًا أثناء
        //      تنفيذ الأمر (يقرؤه getline/المحرّر التالي).
        // (EN) Same TCSADRAIN rationale: don't drop type-ahead typed during command
        //      execution (the next getline/editor read consumes it).
        tcsetattr(STDIN_FILENO, TCSADRAIN, &origTermios_);
        rawModeEnabled_ = false;
    }
#endif
}

} // namespace REPL
} // namespace Sad
