// ======================================================================
// tutorial_engine.h - محرك الدروس التفاعلية / Interactive Tutorial Engine
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   محرك الدروس التفاعلية الذي يُدير عملية التعلم
//   يتتبع تقدم المستخدم ويقدم تمارين تفاعلية
//   يتكامل مع نظام الأخطاء لتقديم دروس مخصصة
//
// English Description:
//   Interactive tutorial engine that manages the learning process
//   Tracks user progress and provides interactive exercises
//   Integrates with error system to provide custom lessons
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#pragma once

#include "lesson_content.h"
#include <functional>
#include <chrono>

namespace Sad {
namespace Teacher {

// ======================================================================
// حالة التمرين / Exercise State
// ======================================================================

/**
 * @enum ExerciseState
 * @brief (AR) حالة التمرين الحالية
 * @brief (EN) Current exercise state
 */
enum class ExerciseState {
    NOT_STARTED,    ///< لم يبدأ / Not started
    IN_PROGRESS,    ///< قيد التنفيذ / In progress
    COMPLETED,      ///< مكتمل بنجاح / Completed successfully
    FAILED,         ///< فشل / Failed
    SKIPPED         ///< تم تخطيه / Skipped
};

/**
 * @struct ExerciseResult
 * @brief (AR) نتيجة محاولة حل تمرين
 * @brief (EN) Result of an exercise attempt
 */
struct ExerciseResult {
    bool success;                   ///< هل نجح؟
    std::string feedback_ar;        ///< تغذية راجعة بالعربية
    std::string feedback_en;        ///< تغذية راجعة بالإنجليزية
    std::string actual_output;      ///< المخرجات الفعلية
    std::string expected_output;    ///< المخرجات المتوقعة
    int attempts_remaining;         ///< المحاولات المتبقية
    std::vector<std::string> hints_shown; ///< التلميحات المعروضة
};

/**
 * @struct TutorialSession
 * @brief (AR) جلسة تعليمية واحدة
 * @brief (EN) Single tutorial session
 */
struct TutorialSession {
    std::string session_id;         ///< معرف الجلسة
    std::string lesson_id;          ///< معرف الدرس
    std::string user_id;            ///< معرف المستخدم
    
    std::chrono::system_clock::time_point start_time;  ///< وقت البدء
    std::chrono::system_clock::time_point end_time;    ///< وقت الانتهاء
    
    int current_section;            ///< القسم الحالي
    int current_exercise;           ///< التمرين الحالي
    int total_attempts;             ///< إجمالي المحاولات
    int successful_attempts;        ///< المحاولات الناجحة
    
    ExerciseState state;            ///< حالة الجلسة
    std::vector<ExerciseResult> results; ///< نتائج التمارين
};

// ======================================================================
// واجهة المستخدم للتعلم / Learning UI Interface
// ======================================================================

/**
 * @class ITutorialUI
 * @brief (AR) واجهة العرض للدروس التفاعلية
 * @brief (EN) Display interface for interactive tutorials
 * 
 * ملاحظات إضافية:
 * - AR: يمكن تنفيذها للـ CLI، GUI، أو IDE
 * - EN: Can be implemented for CLI, GUI, or IDE
 */
class ITutorialUI {
public:
    virtual ~ITutorialUI() = default;
    
    /**
     * @brief (AR) عرض محتوى الدرس
     * @brief (EN) Display lesson content
     */
    virtual void displayLesson(const Lesson& lesson) = 0;
    
    /**
     * @brief (AR) عرض قسم من الدرس
     * @brief (EN) Display a lesson section
     */
    virtual void displaySection(const LessonSection& section) = 0;
    
    /**
     * @brief (AR) عرض تمرين تفاعلي
     * @brief (EN) Display an interactive exercise
     */
    virtual void displayExercise(const ExerciseInfo& exercise) = 0;
    
    /**
     * @brief (AR) عرض نتيجة التمرين
     * @brief (EN) Display exercise result
     */
    virtual void displayResult(const ExerciseResult& result) = 0;
    
    /**
     * @brief (AR) عرض تلميح
     * @brief (EN) Display a hint
     */
    virtual void displayHint(const std::string& hint, int hint_number) = 0;
    
    /**
     * @brief (AR) الحصول على إدخال المستخدم (الكود)
     * @brief (EN) Get user input (code)
     */
    virtual std::string getUserCode() = 0;
    
    /**
     * @brief (AR) عرض رسالة تقدم
     * @brief (EN) Display progress message
     */
    virtual void displayProgress(int current, int total, const std::string& message) = 0;
    
    /**
     * @brief (AR) سؤال المستخدم (نعم/لا)
     * @brief (EN) Ask user (yes/no)
     */
    virtual bool askYesNo(const std::string& question) = 0;
};

// ======================================================================
// محرك التمارين / Exercise Engine
// ======================================================================

/**
 * @class ExerciseEngine
 * @brief (AR) محرك تنفيذ وتقييم التمارين
 * @brief (EN) Exercise execution and evaluation engine
 */
class ExerciseEngine {
public:
    /**
     * @brief (AR) تنفيذ كود التمرين
     * @brief (EN) Execute exercise code
     * 
     * @param code (std::string) — (AR) الكود المُدخل
     * @param expected_output (std::string) — (AR) المخرجات المتوقعة
     * @return (ExerciseResult) — (AR) نتيجة التنفيذ
     */
    ExerciseResult executeCode(const std::string& code,
                               const std::string& expected_output);
    
    /**
     * @brief (AR) التحقق من صحة الكود (بدون تنفيذ)
     * @brief (EN) Validate code (without execution)
     */
    bool validateCode(const std::string& code, std::string& error_message);
    
    /**
     * @brief (AR) تنفيذ الكود والحصول على المخرجات
     * @brief (EN) Execute code and get output
     */
    std::string runCode(const std::string& code, bool& success);
    
    /**
     * @brief (AR) مقارنة المخرجات
     * @brief (EN) Compare outputs
     */
    bool compareOutput(const std::string& actual, const std::string& expected);
    
    /**
     * @brief (AR) تعيين مسار المترجم
     * @brief (EN) Set compiler path
     */
    void setCompilerPath(const std::string& path) { compiler_path_ = path; }

private:
    std::string compiler_path_ = "sad";  ///< مسار مترجم ص
};

// ======================================================================
// محرك الدروس التفاعلية / Interactive Tutorial Engine
// ======================================================================

/**
 * @class TutorialEngine
 * @brief (AR) المحرك الرئيسي للدروس التفاعلية
 * @brief (EN) Main engine for interactive tutorials
 * 
 * ملاحظات إضافية:
 * - AR: يُدير دورة التعلم الكاملة من البداية للنهاية
 * - EN: Manages complete learning cycle from start to finish
 * - AR: يتكامل مع نظام الأخطاء والتقدم
 * - EN: Integrates with error and progress systems
 */
class TutorialEngine {
public:
    // ==================================================================
    // المنشئات / Constructors
    // ==================================================================
    
    /**
     * @brief (AR) منشئ مع واجهة المستخدم
     * @brief (EN) Constructor with UI interface
     */
    explicit TutorialEngine(std::shared_ptr<ITutorialUI> ui);
    
    // ==================================================================
    // إدارة الجلسات / Session Management
    // ==================================================================
    
    /**
     * @brief (AR) بدء جلسة تعليمية جديدة
     * @brief (EN) Start a new tutorial session
     */
    TutorialSession startSession(const std::string& lesson_id,
                                 const std::string& user_id = "default");
    
    /**
     * @brief (AR) إنهاء الجلسة الحالية
     * @brief (EN) End current session
     */
    void endSession();
    
    /**
     * @brief (AR) الحصول على الجلسة الحالية
     * @brief (EN) Get current session
     */
    const TutorialSession& getCurrentSession() const { return current_session_; }
    
    // ==================================================================
    // تشغيل الدرس / Running Lessons
    // ==================================================================
    
    /**
     * @brief (AR) تشغيل درس كامل
     * @brief (EN) Run complete lesson
     * 
     * @param lesson_id (std::string) — (AR) معرف الدرس
     * @return (bool) — (AR) هل اكتمل بنجاح؟
     */
    bool runLesson(const std::string& lesson_id);
    
    /**
     * @brief (AR) تشغيل قسم محدد من الدرس
     * @brief (EN) Run specific lesson section
     */
    bool runSection(int section_index);
    
    /**
     * @brief (AR) تشغيل تمرين محدد
     * @brief (EN) Run specific exercise
     */
    ExerciseResult runExercise(const ExerciseInfo& exercise);
    
    // ==================================================================
    // التنقل / Navigation
    // ==================================================================
    
    /**
     * @brief (AR) الانتقال للقسم التالي
     * @brief (EN) Go to next section
     */
    bool nextSection();
    
    /**
     * @brief (AR) الانتقال للقسم السابق
     * @brief (EN) Go to previous section
     */
    bool previousSection();
    
    /**
     * @brief (AR) تخطي التمرين الحالي
     * @brief (EN) Skip current exercise
     */
    void skipExercise();
    
    // ==================================================================
    // التلميحات / Hints
    // ==================================================================
    
    /**
     * @brief (AR) عرض التلميح التالي
     * @brief (EN) Show next hint
     */
    void showNextHint();
    
    /**
     * @brief (AR) عرض الحل
     * @brief (EN) Show solution
     */
    void showSolution();
    
    // ==================================================================
    // التكامل مع الأخطاء / Error Integration
    // ==================================================================
    
    /**
     * @brief (AR) تشغيل درس مرتبط بخطأ
     * @brief (EN) Run lesson related to an error
     * 
     * @param error_code (std::string) — (AR) رمز الخطأ (مثل ص0001)
     * @return (bool) — (AR) هل تم العثور على درس؟
     */
    bool runLessonForError(const std::string& error_code);
    
    /**
     * @brief (AR) الحصول على شرح لخطأ
     * @brief (EN) Get explanation for error
     */
    std::string getErrorExplanation(const std::string& error_code) const;
    
    // ==================================================================
    // الإعدادات / Settings
    // ==================================================================
    
    /**
     * @brief (AR) تعيين اللغة المفضلة
     * @brief (EN) Set preferred language
     */
    void setPreferredLanguage(const std::string& lang) { preferred_language_ = lang; }
    
    /**
     * @brief (AR) تفعيل/تعطيل التلميحات التلقائية
     * @brief (EN) Enable/disable auto hints
     */
    void setAutoHints(bool enabled) { auto_hints_ = enabled; }
    
    /**
     * @brief (AR) تفعيل/تعطيل الشرح المفصل
     * @brief (EN) Enable/disable verbose explanations
     */
    void setVerboseMode(bool enabled) { verbose_mode_ = enabled; }

private:
    std::shared_ptr<ITutorialUI> ui_;           ///< واجهة المستخدم
    TutorialSession current_session_;            ///< الجلسة الحالية
    ExerciseEngine exercise_engine_;             ///< محرك التمارين
    
    std::string preferred_language_ = "ar";      ///< اللغة المفضلة
    bool auto_hints_ = true;                     ///< تلميحات تلقائية
    bool verbose_mode_ = false;                  ///< وضع التفاصيل
    
    int current_hint_index_ = 0;                 ///< فهرس التلميح الحالي
    
    /**
     * @brief (AR) تهيئة جلسة جديدة
     * @brief (EN) Initialize new session
     */
    void initializeSession(const std::string& lesson_id, const std::string& user_id);
    
    /**
     * @brief (AR) حفظ نتيجة التمرين
     * @brief (EN) Save exercise result
     */
    void saveExerciseResult(const ExerciseResult& result);
};

// ======================================================================
// واجهة CLI للدروس / CLI Tutorial UI
// ======================================================================

/**
 * @class CLITutorialUI
 * @brief (AR) واجهة سطر الأوامر للدروس
 * @brief (EN) Command line interface for tutorials
 */
class CLITutorialUI : public ITutorialUI {
public:
    void displayLesson(const Lesson& lesson) override;
    void displaySection(const LessonSection& section) override;
    void displayExercise(const ExerciseInfo& exercise) override;
    void displayResult(const ExerciseResult& result) override;
    void displayHint(const std::string& hint, int hint_number) override;
    std::string getUserCode() override;
    void displayProgress(int current, int total, const std::string& message) override;
    bool askYesNo(const std::string& question) override;

private:
    /**
     * @brief (AR) طباعة فاصل
     * @brief (EN) Print separator
     */
    void printSeparator(char c = '=', int length = 60);
    
    /**
     * @brief (AR) طباعة عنوان
     * @brief (EN) Print header
     */
    void printHeader(const std::string& title);
    
    /**
     * @brief (AR) طباعة كود مع تنسيق
     * @brief (EN) Print formatted code
     */
    void printCode(const std::string& code);
};

} // namespace Teacher
} // namespace Sad

#endif // SAD_TEACHER_TUTORIAL_ENGINE_H
