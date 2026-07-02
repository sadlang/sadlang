/**
 * @file hot_reload_engine.h
 * @brief (AR) محرك إعادة التحميل الحي — يُنسّق مراقبة الملفات وتحديث المفسر
 * @brief (EN) Hot Reload Engine — coordinates file watching and interpreter updates
 * 
 * يعمل مثل Flutter Hot Reload:
 * 1. يراقب ملفات .ص للتغييرات
 * 2. عند اكتشاف تغيير: يعيد التحليل (Lexer → Parser → AST)
 * 3. يحفظ حالة المتغيرات العمومية والكائنات
 * 4. يُحدّث تعريفات الدوال والأصناف فقط
 * 5. يعيد بناء الواجهة عبر rebuildUI()
 * 
 * @author فريق لغة ص
 * @date July 2025
 */

#pragma once

#include "file_watcher.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "ast_node.h"
#include "statements.h"
#include "value.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <atomic>
#include <chrono>
#include <unordered_set>

// (AR) تصريح تقديمي / (EN) Forward declarations
namespace Sad::Interpreter { class Interpreter; }

namespace Sad {
namespace HotReload {

/**
 * @brief (AR) حالة إعادة التحميل
 * @brief (EN) Hot Reload status
 */
enum class ReloadStatus {
    Success,        ///< (AR) نجح إعادة التحميل / (EN) Reload succeeded
    ParseError,     ///< (AR) خطأ في التحليل النحوي / (EN) Parse error
    RuntimeError,   ///< (AR) خطأ في وقت التشغيل / (EN) Runtime error
    NoChange,       ///< (AR) لم يتغير شيء / (EN) Nothing changed
    Skipped         ///< (AR) تم تخطيه (debounce) / (EN) Skipped (debounce)
};

/**
 * @brief (AR) نتيجة إعادة التحميل
 * @brief (EN) Hot Reload result
 */
struct ReloadResult {
    ReloadStatus status;
    std::string message;                ///< (AR) رسالة للمستخدم / (EN) User message
    int updatedFunctions = 0;           ///< (AR) عدد الدوال المُحدّثة / (EN) Updated functions count
    int updatedClasses = 0;             ///< (AR) عدد الأصناف المُحدّثة / (EN) Updated classes count
    double reloadTimeMs = 0.0;          ///< (AR) زمن إعادة التحميل / (EN) Reload time
    std::vector<std::string> errors;    ///< (AR) الأخطاء (إن وُجدت) / (EN) Errors (if any)
};

/**
 * @brief (AR) دالة استدعاء عند إعادة التحميل
 * @brief (EN) Callback on reload
 */
using ReloadCallback = std::function<void(const ReloadResult&)>;

/**
 * @brief (AR) دالة استدعاء لإعادة بناء الواجهة
 * @brief (EN) Callback to rebuild UI
 */
using RebuildUICallback = std::function<void()>;

/**
 * @brief (AR) خيارات محرك إعادة التحميل
 * @brief (EN) Hot Reload Engine options
 */
struct HotReloadOptions {
    int debounceMs = 300;               ///< (AR) فترة الانتظار / (EN) Debounce period
    bool preserveState = true;          ///< (AR) حفظ الحالة / (EN) Preserve state
    bool showNotifications = true;      ///< (AR) إظهار الإشعارات / (EN) Show notifications
    bool autoRestart = false;           ///< (AR) إعادة تشغيل تلقائية عند فشل / (EN) Auto restart on failure
    bool watchImports = true;           ///< (AR) مراقبة الملفات المستوردة / (EN) Watch imported files
};

/**
 * @brief (AR) محرك إعادة التحميل الحي
 * @brief (EN) Hot Reload Engine
 * 
 * ينسّق بين مراقب الملفات والمفسر لتحقيق إعادة تحميل حية.
 * 
 * @example
 * @code{.cpp}
 * HotReloadEngine engine;
 * engine.attach(interpreter, "main.ص");
 * engine.setRebuildUICallback([]() { bridge->rebuildUI(); });
 * engine.start();
 * // ... في حلقة الأحداث:
 * engine.poll();
 * @endcode
 */
class HotReloadEngine {
public:
    explicit HotReloadEngine(const HotReloadOptions& options = HotReloadOptions());
    ~HotReloadEngine();

    // (AR) منع النسخ / (EN) Non-copyable
    HotReloadEngine(const HotReloadEngine&) = delete;
    HotReloadEngine& operator=(const HotReloadEngine&) = delete;

    /**
     * @brief (AR) ربط المحرك بالمفسر والملف الرئيسي
     * @brief (EN) Attach engine to interpreter and main file
     * 
     * @param interpreter (AR) المفسر / (EN) Interpreter
     * @param mainFilePath (AR) مسار الملف الرئيسي / (EN) Main file path
     */
    void attach(Sad::Interpreter::Interpreter* interpreter, const std::string& mainFilePath);

    /**
     * @brief (AR) إضافة ملف إضافي للمراقبة
     * @brief (EN) Add additional file to watch
     */
    void addWatchFile(const std::string& filePath);

    /**
     * @brief (AR) إضافة مجلد للمراقبة
     * @brief (EN) Add directory to watch
     */
    void addWatchDirectory(const std::string& dirPath);

    /**
     * @brief (AR) تعيين دالة إعادة بناء الواجهة
     * @brief (EN) Set UI rebuild callback
     */
    void setRebuildUICallback(RebuildUICallback callback);

    /**
     * @brief (AR) تعيين دالة الاستدعاء عند إعادة التحميل
     * @brief (EN) Set reload callback
     */
    void setReloadCallback(ReloadCallback callback);

    /**
     * @brief (AR) تعيين دالة تصفير حالة الواجهة عند إعادة التنفيذ الكاملة (Hot Restart).
     *        تُستدعى بعد interpreter->reset() مباشرةً لتصفير حالةٍ حيّة تشير إلى المفسّر
     *        المُعاد تصفيره (مثل مكدّس تنقّل sad::ui::nav المُسرَّب الذي يحمل إدخالات بانٍ
     *        تلتقط المفسّر). الحفاظ على شفافية المحرّك تجاه الرسومات: القلب يستدعي دالّةً
     *        معتمة تزوّدها طبقة الواجهة (UIBridge). — إصلاح مراجعة Amelia (HIGH-1).
     * @brief (EN) Set UI-state reset callback invoked after interpreter->reset() on hot
     *        restart, to clear live state referencing the reset interpreter (e.g. the
     *        leaked sad::ui::nav stack whose builder entries capture the interpreter).
     */
    void setStateResetCallback(RebuildUICallback callback);

    /**
     * @brief (AR) بدء المراقبة (في خيط منفصل)
     * @brief (EN) Start watching (in background thread)
     */
    void start();

    /**
     * @brief (AR) إيقاف المراقبة
     * @brief (EN) Stop watching
     */
    void stop();

    /**
     * @brief (AR) فحص يدوي — لاستخدامه في حلقة أحداث
     * @brief (EN) Manual poll — for use in event loop
     * 
     * يفحص التغييرات ويُنفّذ إعادة التحميل إذا لزم الأمر.
     * يُستدعى من حلقة أحداث النافذة أو حلقة الأحداث الرئيسية.
     * 
     * @return (AR) نتيجة إعادة التحميل / (EN) Reload result
     */
    ReloadResult poll();

    /**
     * @brief (AR) إعادة تحميل يدوية (بدون انتظار تغيير)
     * @brief (EN) Manual reload (without waiting for change)
     */
    ReloadResult forceReload();

    /**
     * @brief (AR) هل المحرك نشط؟
     * @brief (EN) Is engine active?
     */
    bool isActive() const { return active_.load(); }

    /**
     * @brief (AR) عدد مرات إعادة التحميل الناجحة
     * @brief (EN) Successful reload count
     */
    int getReloadCount() const { return reloadCount_.load(); }

    /**
     * @brief (AR) آخر نتيجة إعادة تحميل
     * @brief (EN) Last reload result
     */
    const ReloadResult& getLastResult() const { return lastResult_; }

private:
    HotReloadOptions options_;
    Sad::Interpreter::Interpreter* interpreter_ = nullptr;
    std::string mainFilePath_;
    std::string mainSource_;  // (AR) آخر كود مصدري / (EN) Last source code
    
    std::unique_ptr<FileWatcher> fileWatcher_;
    ReloadCallback reloadCallback_;
    RebuildUICallback rebuildUICallback_;
    RebuildUICallback stateResetCallback_; ///< (HIGH-1) تصفير حالة الواجهة عند Hot Restart
    
    std::atomic<bool> active_{false};
    std::atomic<bool> pendingReload_{false};
    std::atomic<int> reloadCount_{0};
    ReloadResult lastResult_;
    
    std::mutex reloadMutex_;
    std::string pendingFilePath_;

    /**
     * @brief (AR) تنفيذ إعادة التحميل لملف محدد
     * @brief (EN) Perform reload for specific file
     */
    ReloadResult performReload(const std::string& filePath);

    /**
     * @brief (AR) إعادة تحليل الملف المصدري
     * @brief (EN) Re-parse source file
     */
    bool reparseSource(const std::string& source, const std::string& filename,
                       std::vector<std::unique_ptr<AST::Statement>>& outProgram,
                       std::vector<std::string>& outErrors);

    /**
     * @brief (AR) تحديث الدوال في المفسر (حفظ الحالة)
     * @brief (EN) Update functions in interpreter (preserve state)
     */
    int updateFunctions(const std::vector<std::unique_ptr<AST::Statement>>& program);

    /**
     * @brief (AR) تحديث الأصناف في المفسر (حفظ الحالة)
     * @brief (EN) Update classes in interpreter (preserve state)
     */
    int updateClasses(const std::vector<std::unique_ptr<AST::Statement>>& program);

    /**
     * @brief (AR) تنفيذ الجمل العمومية الجديدة فقط
     * @brief (EN) Execute only new global statements
     */
    void executeNewStatements(const std::vector<std::unique_ptr<AST::Statement>>& program);

    /**
     * @brief (AR) البحث عن ملفات مستوردة وإضافتها للمراقبة
     * @brief (EN) Find imported files and add them to watch
     */
    void discoverImports(const std::vector<std::unique_ptr<AST::Statement>>& program);

    /**
     * @brief (AR) قراءة ملف UTF-8
     * @brief (EN) Read UTF-8 file
     */
    static std::string readFile(const std::string& path);
};

} // namespace HotReload
} // namespace Sad
