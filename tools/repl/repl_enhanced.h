/**
 * =============================================================================
 * ملف: repl_enhanced.h
 * الوصف: REPL محسّن مع تنقل التاريخ وإكمال تلقائي وتحرير تفاعلي
 * (AR) @brief REPL محسّن مع دعم مفاتيح الأسهم وTab
 * (EN) @brief Enhanced REPL with arrow keys, Tab completion, interactive editing
 * المهمة: T306 - REPL improvements
 * =============================================================================
 */

#pragma once

#include "repl_engine.h"
#include <string>
#include <vector>
#include <set>

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

// =============================================================================
// REPL المحسّن / Enhanced REPL
// =============================================================================

/**
 * (AR) REPL محسّن يستخدم محرر السطر التفاعلي بدلاً من std::getline
 * (EN) Enhanced REPL that uses interactive line editor instead of std::getline
 */
class EnhancedREPL {
public:
    explicit EnhancedREPL(const REPLConfig& config = REPLConfig{});
    ~EnhancedREPL();
    
    /**
     * (AR) تشغيل REPL المحسّن
     * (EN) Run enhanced REPL
     */
    int run();

private:
    REPLConfig config_;
    std::unique_ptr<REPLEngine> engine_;
    std::unique_ptr<AutoCompleter> completer_;
    std::unique_ptr<LineEditor> editor_;
    
    void extractIdentifiers(const std::string& code);
};

} // namespace REPL
} // namespace Sad
