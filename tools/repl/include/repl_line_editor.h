/**
 * =============================================================================
 * ملف: repl_line_editor.h
 * الوصف: مكوّنات الإدخال الغنيّ لـREPL — إكمال تلقائي + محرّر سطر تفاعلي
 * (AR) @brief مُكمِّل تلقائي (Tab) ومحرّر سطر (أسهم/Home/End) يستعملهما REPLEngine
 * (EN) @brief Auto-completer (Tab) and interactive line editor (arrows/Home/End)
 *      consumed by REPLEngine. No standalone REPL class — one unified engine.
 * =============================================================================
 */

#pragma once

// (AR) لا نعتمد repl_engine.h (تفاديًا للتضمين الدائريّ) — نحتاج فقط HistoryManager
// (EN) Do NOT depend on repl_engine.h (avoids a circular include) — only HistoryManager
#include "history_manager.h"
#include <string>
#include <vector>
#include <set>

#ifndef _WIN32
#include <termios.h>
#endif

namespace Sad {
namespace REPL {

// =============================================================================
// مُكمل تلقائي / Auto-completer
// =============================================================================

/**
 * (AR) نظام الإكمال التلقائي مع كلمات مفتاحية ومُعرّفات المستخدم
 * (EN) Auto-completion system with keywords and user identifiers
 */
class AutoCompleter {
public:
    AutoCompleter();
    
    /**
     * (AR) الحصول على اقتراحات الإكمال لبادئة
     * (EN) Get completion suggestions for a prefix
     */
    std::vector<std::string> complete(const std::string& prefix) const;
    
    /**
     * (AR) إضافة مُعرّف مستخدم (متغير أو دالة عرّفها المستخدم)
     * (EN) Add user identifier (variable or function defined by user)
     */
    void addUserIdentifier(const std::string& name);
    
    /**
     * (AR) مسح مُعرّفات المستخدم
     * (EN) Clear user identifiers
     */
    void clearUserIdentifiers();

private:
    std::vector<std::string> keywords_;       ///< كلمات مفتاحية
    std::vector<std::string> builtins_;       ///< دوال مدمجة
    std::vector<std::string> types_;          ///< أسماء أنواع
    std::set<std::string> userIdentifiers_;   ///< مُعرّفات المستخدم
    
    void initKeywords();
    void initBuiltins();
    void initTypes();
};

// =============================================================================
// محرر السطر التفاعلي / Interactive Line Editor
// =============================================================================

/**
 * (AR) محرر سطر تفاعلي مع دعم مفاتيح الأسهم، Home/End، Delete
 * (EN) Interactive line editor with arrow keys, Home/End, Delete support
 */
class LineEditor {
public:
    LineEditor(HistoryManager* history, AutoCompleter* completer);
    
    /**
     * (AR) قراءة سطر مع تحرير تفاعلي
     * (EN) Read line with interactive editing
     * @param prompt الموجه / Prompt string
     * @return السطر المقروء أو فارغ عند EOF / Read line or empty on EOF
     */
    std::string readLine(const std::string& prompt);
    
    /**
     * (AR) هل تم الضغط على Ctrl+D؟
     * (EN) Was Ctrl+D pressed?
     */
    bool isEof() const { return eof_; }

private:
    HistoryManager* history_;
    AutoCompleter* completer_;
    bool eof_ = false;
    
    // حالة التحرير / Editing state
    std::string buffer_;
    size_t cursorPos_ = 0;
    int completionIndex_ = -1;
    std::vector<std::string> lastCompletions_;
    
    // إدخال الأحرف / Character input
    int readChar();
    void handleArrowKey(int key);
    void handleTab();
    void handleBackspace();
    void handleDelete();
    void handleHome();
    void handleEnd();
    void handleNormalChar(int ch);
    
    // عرض / Display
    void refreshLine(const std::string& prompt);
    void clearLine();
    size_t displayWidth(const std::string& s) const;
    
    // أداء المنصة / Platform
    void enableRawMode();
    void disableRawMode();
    
#ifdef _WIN32
    // لا حاجة لحفظ حالة على Windows
#else
    struct termios origTermios_;
    bool rawModeEnabled_ = false;
#endif
};

// (AR) ملاحظة: أُزيل الصنف EnhancedREPL — دُمجت قدراته (المُكمِّل + محرّر السطر)
//      في REPLEngine مباشرةً، فلا توجد نسختان من REPL. راجع repl_engine.{h,cpp}.
// (EN) Note: the EnhancedREPL class was removed — its capabilities (completer +
//      line editor) were folded directly into REPLEngine, so there is a single
//      unified REPL, not two. See repl_engine.{h,cpp}.

} // namespace REPL
} // namespace Sad
