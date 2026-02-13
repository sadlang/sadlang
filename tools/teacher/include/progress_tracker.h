// ======================================================================
// progress_tracker.h - واجهة نظام تتبع التقدم التعليمي
// Progress Tracking System Interface
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   واجهة نظام تتبع تقدم المستخدم في الدروس التعليمية. يتضمن
//   هياكل البيانات للتقدم والإنجازات والإحصائيات والملف التعريفي.
//
// English Description:
//   Interface for the user progress tracking system in educational
//   lessons. Includes data structures for progress, achievements,
//   statistics, and user profile.
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#ifndef SAD_TEACHER_PROGRESS_TRACKER_H
#define SAD_TEACHER_PROGRESS_TRACKER_H

#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace Sad {
namespace Teacher {

// ======================================================================
// أنواع حالة التقدم / Progress Status Types
// ======================================================================

/**
 * @enum LessonProgressStatus
 * @brief (AR) حالة تقدم الدرس
 * @brief (EN) Lesson progress status
 */
enum class LessonProgressStatus {
    NOT_STARTED,    // لم يبدأ
    IN_PROGRESS,    // قيد التقدم
    COMPLETED,      // مكتمل
    SKIPPED         // تم تخطيه
};

// ======================================================================
// هياكل البيانات / Data Structures
// ======================================================================

/**
 * @struct LessonProgress
 * @brief (AR) تقدم درس واحد
 * @brief (EN) Progress for a single lesson
 */
struct LessonProgress {
    std::string lessonId;                           // معرف الدرس
    LessonProgressStatus status = LessonProgressStatus::NOT_STARTED;
    int attempts = 0;                               // عدد المحاولات
    int completedExercises = 0;                     // التمارين المكتملة
    int totalExercises = 0;                         // إجمالي التمارين
    int earnedPoints = 0;                           // النقاط المكتسبة
    int bestScore = 0;                              // أفضل نتيجة
    int totalTimeMinutes = 0;                       // الوقت الإجمالي بالدقائق
    std::time_t lastAccessedAt = 0;                 // آخر وصول
    std::time_t completedAt = 0;                    // تاريخ الإكمال
};

/**
 * @struct Achievement
 * @brief (AR) إنجاز المستخدم
 * @brief (EN) User achievement
 */
struct Achievement {
    std::string id;                                 // معرف الإنجاز
    std::string name;                               // اسم الإنجاز
    std::string description;                        // وصف الإنجاز
    std::string icon;                               // رمز الإنجاز
    int points;                                     // النقاط الممنوحة
    std::time_t unlockedAt = 0;                     // تاريخ الفتح
};

/**
 * @struct AchievementInfo
 * @brief (AR) معلومات الإنجاز (بدون تاريخ الفتح)
 * @brief (EN) Achievement info (without unlock date)
 */
struct AchievementInfo {
    std::string name;
    std::string description;
    std::string icon;
    int points;
};

/**
 * @struct LearningStatistics
 * @brief (AR) إحصائيات التعلم العامة
 * @brief (EN) Overall learning statistics
 */
struct LearningStatistics {
    int totalLessonsStarted = 0;                    // الدروس التي بدأت
    int totalLessonsCompleted = 0;                  // الدروس المكتملة
    int totalExercisesAttempted = 0;                // التمارين المحاولة
    int totalExercisesPassed = 0;                   // التمارين الناجحة
    int totalTimeMinutes = 0;                       // الوقت الإجمالي
    int totalHintsUsed = 0;                         // التلميحات المستخدمة
    int totalErrorsEncountered = 0;                 // الأخطاء المواجهة
    std::map<std::string, int> errorFrequency;      // تكرار كل خطأ
    std::map<std::string, int> topicProgress;       // التقدم في كل موضوع
};

/**
 * @struct UserProfile
 * @brief (AR) ملف تعريف المستخدم
 * @brief (EN) User profile
 */
struct UserProfile {
    std::string userId;                             // معرف المستخدم
    std::string displayName;                        // اسم العرض
    std::string createdAt;                          // تاريخ الإنشاء
    std::string lastActiveAt;                       // آخر نشاط
    int totalPoints = 0;                            // إجمالي النقاط
    int currentStreak = 0;                          // سلسلة الأيام الحالية
    int longestStreak = 0;                          // أطول سلسلة أيام
    std::string currentLevel;                       // المستوى الحالي
};

// ======================================================================
// فئة متتبع التقدم / Progress Tracker Class
// ======================================================================

/**
 * @class ProgressTracker
 * @brief (AR) نظام تتبع التقدم التعليمي (Singleton)
 * @brief (EN) Educational progress tracking system (Singleton)
 * 
 * @details
 * (AR) يدير تتبع تقدم المستخدم في الدروس التعليمية، بما في ذلك:
 *      - تقدم الدروس الفردية
 *      - الإنجازات المفتوحة
 *      - الإحصائيات العامة
 *      - ملف تعريف المستخدم
 *      - الحفظ والتحميل من ملف
 * 
 * (EN) Manages user progress tracking in educational lessons, including:
 *      - Individual lesson progress
 *      - Unlocked achievements
 *      - Overall statistics
 *      - User profile
 *      - Save/load from file
 */
class ProgressTracker {
public:
    // ============================================================
    // Singleton Pattern / نمط المفرد
    // ============================================================
    
    /**
     * @brief (AR) الحصول على المثيل الوحيد
     * @brief (EN) Get singleton instance
     */
    static ProgressTracker& getInstance();
    
    // حذف نسخ المثيل
    ProgressTracker(const ProgressTracker&) = delete;
    ProgressTracker& operator=(const ProgressTracker&) = delete;
    
    // ============================================================
    // التحميل والحفظ / Loading and Saving
    // ============================================================
    
    /**
     * @brief (AR) تحميل التقدم من الملف الافتراضي
     * @brief (EN) Load progress from default file
     */
    bool load();
    
    /**
     * @brief (AR) تحميل التقدم من ملف محدد
     * @brief (EN) Load progress from specific file
     */
    bool loadFromFile(const std::string& file_path);
    
    /**
     * @brief (AR) حفظ التقدم إلى الملف الافتراضي
     * @brief (EN) Save progress to default file
     */
    bool save();
    
    /**
     * @brief (AR) حفظ التقدم إلى ملف محدد
     * @brief (EN) Save progress to specific file
     */
    bool saveToFile(const std::string& file_path);
    
    /**
     * @brief (AR) تفعيل/تعطيل الحفظ التلقائي
     * @brief (EN) Enable/disable auto-save
     */
    void setAutoSave(bool enabled) { m_autoSave = enabled; }
    
    // ============================================================
    // إدارة تقدم الدروس / Lesson Progress Management
    // ============================================================
    
    /**
     * @brief (AR) بدء درس
     * @brief (EN) Start a lesson
     */
    void startLesson(const std::string& lesson_id);
    
    /**
     * @brief (AR) إكمال تمرين
     * @brief (EN) Complete an exercise
     */
    void completeExercise(const std::string& lesson_id, 
                          bool passed, 
                          int points_earned = 0);
    
    /**
     * @brief (AR) إكمال درس
     * @brief (EN) Complete a lesson
     */
    void completeLesson(const std::string& lesson_id, int score = 100);
    
    /**
     * @brief (AR) الحصول على تقدم درس
     * @brief (EN) Get lesson progress
     */
    LessonProgress getLessonProgress(const std::string& lesson_id) const;
    
    /**
     * @brief (AR) التحقق من إكمال درس
     * @brief (EN) Check if lesson is completed
     */
    bool isLessonCompleted(const std::string& lesson_id) const;
    
    /**
     * @brief (AR) الحصول على نسبة الإكمال
     * @brief (EN) Get completion percentage
     */
    float getLessonCompletionPercentage(const std::string& lesson_id) const;
    
    /**
     * @brief (AR) الحصول على جميع تقدم الدروس
     * @brief (EN) Get all lesson progress
     */
    const std::map<std::string, LessonProgress>& getAllLessonProgress() const {
        return m_lessonProgress;
    }
    
    // ============================================================
    // إدارة الإنجازات / Achievement Management
    // ============================================================
    
    /**
     * @brief (AR) فتح إنجاز
     * @brief (EN) Unlock achievement
     */
    void unlockAchievement(const std::string& achievement_id);
    
    /**
     * @brief (AR) التحقق من وجود إنجاز
     * @brief (EN) Check if has achievement
     */
    bool hasAchievement(const std::string& achievement_id) const;
    
    /**
     * @brief (AR) الحصول على جميع الإنجازات
     * @brief (EN) Get all achievements
     */
    std::vector<Achievement> getAchievements() const;
    
    // ============================================================
    // الإحصائيات / Statistics
    // ============================================================
    
    /**
     * @brief (AR) الحصول على الإحصائيات
     * @brief (EN) Get statistics
     */
    LearningStatistics getStatistics() const;
    
    /**
     * @brief (AR) تسجيل استخدام تلميح
     * @brief (EN) Record hint usage
     */
    void recordHintUsed();
    
    /**
     * @brief (AR) تسجيل مواجهة خطأ
     * @brief (EN) Record error encountered
     */
    void recordErrorEncountered(const std::string& error_code);
    
    /**
     * @brief (AR) تسجيل وقت التعلم
     * @brief (EN) Record learning time
     */
    void recordTimeSpent(int minutes);
    
    // ============================================================
    // الملف التعريفي / Profile
    // ============================================================
    
    /**
     * @brief (AR) الحصول على الملف التعريفي
     * @brief (EN) Get user profile
     */
    UserProfile getProfile() const;
    
    /**
     * @brief (AR) تعيين اسم العرض
     * @brief (EN) Set display name
     */
    void setDisplayName(const std::string& name);
    
    /**
     * @brief (AR) الحصول على إجمالي النقاط
     * @brief (EN) Get total points
     */
    int getTotalPoints() const;
    
    /**
     * @brief (AR) الحصول على المستوى الحالي
     * @brief (EN) Get current level
     */
    std::string getCurrentLevel() const;
    
    /**
     * @brief (AR) الحصول على سلسلة الأيام
     * @brief (EN) Get current streak
     */
    int getCurrentStreak() const;
    
    // ============================================================
    // طباعة التقارير / Report Printing
    // ============================================================
    
    /**
     * @brief (AR) طباعة تقرير التقدم
     * @brief (EN) Print progress report
     */
    void printProgressReport() const;
    
private:
    // ============================================================
    // المُنشئ الخاص / Private Constructor
    // ============================================================
    ProgressTracker();
    ~ProgressTracker();
    
    // ============================================================
    // الدوال المساعدة / Helper Functions
    // ============================================================
    std::string getDefaultProgressFilePath() const;
    std::string generateUserId() const;
    std::string getCurrentTimestamp() const;
    
    void parseProfileLine(const std::string& key, const std::string& value);
    void parseLessonLine(const std::string& key, const std::string& value);
    void parseAchievementLine(const std::string& key, const std::string& value);
    void parseStatisticsLine(const std::string& key, const std::string& value);
    
    void checkLessonCompletion(const std::string& lesson_id);
    void checkAchievements();
    void updateStreak();
    void updateLevel();
    
    AchievementInfo getAchievementInfo(const std::string& achievement_id) const;
    void printAchievementUnlocked(const Achievement& ach) const;
    
    // ============================================================
    // البيانات / Data Members
    // ============================================================
    std::map<std::string, LessonProgress> m_lessonProgress;  // تقدم الدروس
    std::vector<Achievement> m_achievements;                  // الإنجازات
    LearningStatistics m_statistics;                         // الإحصائيات
    UserProfile m_profile;                                   // الملف التعريفي
    
    std::string m_progressFilePath;                          // مسار ملف التقدم
    bool m_loaded;                                           // هل تم التحميل
    bool m_dirty;                                            // هل هناك تغييرات غير محفوظة
    bool m_autoSave;                                         // الحفظ التلقائي
};

} // namespace Teacher
} // namespace Sad

#endif // SAD_TEACHER_PROGRESS_TRACKER_H
