// ======================================================================
// lesson_content.h - نظام محتوى الدروس / Lesson Content System
// ======================================================================
// بسم الله الرحمن الرحيم
// 
// الوصف بالعربية:
//   نظام إدارة محتوى الدروس التعليمية في لغة ص
//   يُوفر واجهة موحدة لتخزين واسترجاع وعرض الدروس
//   يدعم دروس نصية، فيديو، تمارين تفاعلية
//
// English Description:
//   Lesson content management system for Sad language education
//   Provides unified interface for storing, retrieving, and displaying lessons
//   Supports text lessons, videos, interactive exercises
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace Sad {
namespace Teacher {

// ======================================================================
// أنواع الدروس / Lesson Types
// ======================================================================

/**
 * @enum LessonType
 * @brief (AR) تعداد أنواع الدروس المتاحة
 * @brief (EN) Enumeration of available lesson types
 */
enum class LessonType {
    TEXT,           ///< درس نصي / Text lesson
    VIDEO,          ///< درس فيديو / Video lesson
    EXERCISE,       ///< تمرين تفاعلي / Interactive exercise
    QUIZ,           ///< اختبار قصير / Quiz
    PROJECT,        ///< مشروع عملي / Practical project
    MIXED           ///< درس مختلط / Mixed lesson
};

/**
 * @enum LessonLevel
 * @brief (AR) مستويات صعوبة الدروس
 * @brief (EN) Lesson difficulty levels
 */
enum class LessonLevel {
    BEGINNER,       ///< مبتدئ / Beginner
    INTERMEDIATE,   ///< متوسط / Intermediate
    ADVANCED,       ///< متقدم / Advanced
    EXPERT          ///< خبير / Expert
};

/**
 * @enum LessonCategory
 * @brief (AR) فئات الدروس حسب الموضوع
 * @brief (EN) Lesson categories by topic
 */
enum class LessonCategory {
    BASICS,              ///< أساسيات اللغة / Language basics
    VARIABLES,           ///< المتغيرات / Variables
    CONTROL_FLOW,        ///< التحكم في التدفق / Control flow
    FUNCTIONS,           ///< الدوال / Functions
    CLASSES,             ///< الأصناف / Classes
    OWNERSHIP,           ///< نظام الملكية / Ownership system
    BORROWING,           ///< الاستعارة / Borrowing
    LIFETIMES,           ///< فترات الحياة / Lifetimes
    ERROR_HANDLING,      ///< معالجة الأخطاء / Error handling
    PATTERN_MATCHING,    ///< مطابقة الأنماط / Pattern matching
    ASYNC,               ///< البرمجة غير المتزامنة / Async programming
    GENERICS,            ///< القوالب / Generics
    TRAITS,              ///< السمات / Traits
    MODULES,             ///< الوحدات / Modules
    TESTING,             ///< الاختبار / Testing
    OPTIMIZATION,        ///< التحسين / Optimization
    FFI,                 ///< التكامل مع C/C++ / C/C++ integration
    EMBEDDED,            ///< الأنظمة المدمجة / Embedded systems
    WEB,                 ///< تطوير الويب / Web development
    MOBILE               ///< تطوير الهاتف / Mobile development
};

// ======================================================================
// هيكل محتوى الدرس / Lesson Content Structure
// ======================================================================

/**
 * @struct LessonSection
 * @brief (AR) قسم من درس - يمكن أن يكون نص أو كود أو تمرين
 * @brief (EN) Section of a lesson - can be text, code, or exercise
 */
struct LessonSection {
    std::string title_ar;           ///< عنوان القسم بالعربية
    std::string title_en;           ///< عنوان القسم بالإنجليزية
    std::string content_ar;         ///< المحتوى بالعربية
    std::string content_en;         ///< المحتوى بالإنجليزية
    std::string code_example;       ///< مثال كود (إن وجد)
    std::string exercise_id;        ///< معرف التمرين (إن وجد)
    bool is_interactive;            ///< هل القسم تفاعلي؟
};

/**
 * @struct VideoInfo
 * @brief (AR) معلومات الفيديو المرتبط بالدرس
 * @brief (EN) Video information linked to lesson
 */
struct VideoInfo {
    std::string video_id;           ///< معرف الفيديو الفريد
    std::string url;                ///< رابط الفيديو
    std::string title_ar;           ///< عنوان الفيديو بالعربية
    std::string title_en;           ///< عنوان الفيديو بالإنجليزية
    int duration_seconds;           ///< مدة الفيديو بالثواني
    std::string thumbnail_url;      ///< رابط الصورة المصغرة
};

/**
 * @struct ExerciseInfo
 * @brief (AR) معلومات التمرين التفاعلي
 * @brief (EN) Interactive exercise information
 */
struct ExerciseInfo {
    std::string exercise_id;        ///< معرف التمرين الفريد
    std::string title_ar;           ///< عنوان التمرين بالعربية
    std::string title_en;           ///< عنوان التمرين بالإنجليزية
    std::string description_ar;     ///< وصف التمرين بالعربية
    std::string description_en;     ///< وصف التمرين بالإنجليزية
    std::string starter_code;       ///< الكود الابتدائي
    std::string expected_output;    ///< المخرجات المتوقعة
    std::string solution;           ///< الحل النموذجي
    std::vector<std::string> hints; ///< تلميحات المساعدة
    int max_attempts;               ///< الحد الأقصى للمحاولات
};

// ======================================================================
// فئة الدرس الرئيسية / Main Lesson Class
// ======================================================================

/**
 * @class Lesson
 * @brief (AR) فئة تمثل درساً تعليمياً كاملاً
 * @brief (EN) Class representing a complete lesson
 * 
 * ملاحظات إضافية:
 * - AR: الدرس يمكن أن يحتوي على عدة أقسام ووسائط متعددة
 * - EN: Lesson can contain multiple sections and multimedia
 * - AR: يدعم كلاً من العربية والإنجليزية
 * - EN: Supports both Arabic and English
 */
class Lesson {
public:
    // ==================================================================
    // المنشئات / Constructors
    // ==================================================================
    
    /**
     * @brief (AR) منشئ الدرس مع معرف فريد
     * @brief (EN) Lesson constructor with unique ID
     * 
     * @param id (std::string) — (AR) المعرف الفريد للدرس / (EN) Unique lesson ID
     */
    explicit Lesson(const std::string& id);
    
    /**
     * @brief (AR) منشئ كامل للدرس
     * @brief (EN) Full lesson constructor
     */
    Lesson(const std::string& id, 
           const std::string& title_ar,
           const std::string& title_en,
           LessonType type,
           LessonLevel level,
           LessonCategory category);
    
    // ==================================================================
    // الوظائف الاستعلامية / Getters
    // ==================================================================
    
    std::string getId() const { return id_; }
    std::string getTitleArabic() const { return title_ar_; }
    std::string getTitleEnglish() const { return title_en_; }
    std::string getDescriptionArabic() const { return description_ar_; }
    std::string getDescriptionEnglish() const { return description_en_; }
    LessonType getType() const { return type_; }
    LessonLevel getLevel() const { return level_; }
    LessonCategory getCategory() const { return category_; }
    
    const std::vector<LessonSection>& getSections() const { return sections_; }
    const std::vector<VideoInfo>& getVideos() const { return videos_; }
    const std::vector<ExerciseInfo>& getExercises() const { return exercises_; }
    const std::vector<std::string>& getPrerequisites() const { return prerequisites_; }
    const std::vector<std::string>& getRelatedLessons() const { return related_lessons_; }
    const std::vector<std::string>& getTags() const { return tags_; }
    
    int getEstimatedMinutes() const { return estimated_minutes_; }
    
    // ==================================================================
    // وظائف التعديل / Setters
    // ==================================================================
    
    void setTitleArabic(const std::string& title) { title_ar_ = title; }
    void setTitleEnglish(const std::string& title) { title_en_ = title; }
    void setDescriptionArabic(const std::string& desc) { description_ar_ = desc; }
    void setDescriptionEnglish(const std::string& desc) { description_en_ = desc; }
    void setType(LessonType type) { type_ = type; }
    void setLevel(LessonLevel level) { level_ = level; }
    void setCategory(LessonCategory category) { category_ = category; }
    void setEstimatedMinutes(int minutes) { estimated_minutes_ = minutes; }
    
    // ==================================================================
    // وظائف الإضافة / Add Functions
    // ==================================================================
    
    /**
     * @brief (AR) إضافة قسم جديد للدرس
     * @brief (EN) Add a new section to the lesson
     */
    void addSection(const LessonSection& section);
    
    /**
     * @brief (AR) إضافة فيديو للدرس
     * @brief (EN) Add a video to the lesson
     */
    void addVideo(const VideoInfo& video);
    
    /**
     * @brief (AR) إضافة تمرين للدرس
     * @brief (EN) Add an exercise to the lesson
     */
    void addExercise(const ExerciseInfo& exercise);
    
    /**
     * @brief (AR) إضافة درس متطلب سابق
     * @brief (EN) Add a prerequisite lesson
     */
    void addPrerequisite(const std::string& lesson_id);
    
    /**
     * @brief (AR) إضافة درس مرتبط
     * @brief (EN) Add a related lesson
     */
    void addRelatedLesson(const std::string& lesson_id);
    
    /**
     * @brief (AR) إضافة وسم للدرس
     * @brief (EN) Add a tag to the lesson
     */
    void addTag(const std::string& tag);
    
    // ==================================================================
    // وظائف التسلسل / Serialization Functions
    // ==================================================================
    
    /**
     * @brief (AR) تحويل الدرس إلى JSON
     * @brief (EN) Convert lesson to JSON
     */
    std::string toJson() const;
    
    /**
     * @brief (AR) تحميل الدرس من JSON
     * @brief (EN) Load lesson from JSON
     */
    static std::optional<Lesson> fromJson(const std::string& json);

private:
    std::string id_;                            ///< معرف الدرس الفريد
    std::string title_ar_;                      ///< العنوان بالعربية
    std::string title_en_;                      ///< العنوان بالإنجليزية
    std::string description_ar_;                ///< الوصف بالعربية
    std::string description_en_;                ///< الوصف بالإنجليزية
    LessonType type_;                           ///< نوع الدرس
    LessonLevel level_;                         ///< مستوى الصعوبة
    LessonCategory category_;                   ///< الفئة
    int estimated_minutes_;                     ///< الوقت المقدر بالدقائق
    
    std::vector<LessonSection> sections_;       ///< أقسام الدرس
    std::vector<VideoInfo> videos_;             ///< فيديوهات الدرس
    std::vector<ExerciseInfo> exercises_;       ///< تمارين الدرس
    std::vector<std::string> prerequisites_;    ///< الدروس المتطلبة
    std::vector<std::string> related_lessons_;  ///< دروس ذات صلة
    std::vector<std::string> tags_;             ///< وسوم الدرس
};

// ======================================================================
// مدير محتوى الدروس / Lesson Content Manager
// ======================================================================

/**
 * @class LessonContentManager
 * @brief (AR) مدير محتوى الدروس - مسؤول عن تحميل وتخزين الدروس
 * @brief (EN) Lesson content manager - responsible for loading and storing lessons
 * 
 * ملاحظات إضافية:
 * - AR: يستخدم نمط Singleton للوصول الموحد
 * - EN: Uses Singleton pattern for unified access
 * - AR: يدعم التحميل الكسول للدروس
 * - EN: Supports lazy loading of lessons
 */
class LessonContentManager {
public:
    // ==================================================================
    // Singleton Pattern
    // ==================================================================
    
    /**
     * @brief (AR) الحصول على النسخة الوحيدة من المدير
     * @brief (EN) Get the singleton instance of the manager
     */
    static LessonContentManager& getInstance();
    
    // حذف النسخ / Delete copy operations
    LessonContentManager(const LessonContentManager&) = delete;
    LessonContentManager& operator=(const LessonContentManager&) = delete;
    
    // ==================================================================
    // وظائف التحميل / Loading Functions
    // ==================================================================
    
    /**
     * @brief (AR) تحميل جميع الدروس من مجلد
     * @brief (EN) Load all lessons from a directory
     * 
     * @param lessons_dir (std::string) — (AR) مسار مجلد الدروس
     *                                    (EN) Path to lessons directory
     * @return (bool) — (AR) نجاح التحميل / (EN) Loading success
     */
    bool loadLessonsFromDirectory(const std::string& lessons_dir);
    
    /**
     * @brief (AR) تحميل درس واحد من ملف
     * @brief (EN) Load a single lesson from file
     */
    std::optional<Lesson> loadLessonFromFile(const std::string& file_path);
    
    /**
     * @brief (AR) تحميل الدروس المدمجة (مُترجمة مع البرنامج)
     * @brief (EN) Load embedded lessons (compiled with program)
     */
    void loadEmbeddedLessons();
    
    // ==================================================================
    // وظائف الاستعلام / Query Functions
    // ==================================================================
    
    /**
     * @brief (AR) الحصول على درس بمعرفه
     * @brief (EN) Get lesson by its ID
     */
    std::optional<Lesson> getLesson(const std::string& lesson_id) const;
    
    /**
     * @brief (AR) الحصول على دروس حسب الفئة
     * @brief (EN) Get lessons by category
     */
    std::vector<Lesson> getLessonsByCategory(LessonCategory category) const;
    
    /**
     * @brief (AR) الحصول على دروس حسب المستوى
     * @brief (EN) Get lessons by level
     */
    std::vector<Lesson> getLessonsByLevel(LessonLevel level) const;
    
    /**
     * @brief (AR) البحث في الدروس
     * @brief (EN) Search in lessons
     */
    std::vector<Lesson> searchLessons(const std::string& query) const;
    
    /**
     * @brief (AR) الحصول على جميع الدروس
     * @brief (EN) Get all lessons
     */
    std::vector<Lesson> getAllLessons() const;
    
    /**
     * @brief (AR) الحصول على الدروس المقترحة للمبتدئين
     * @brief (EN) Get recommended lessons for beginners
     */
    std::vector<Lesson> getBeginnerPath() const;
    
    // ==================================================================
    // وظائف ربط الأخطاء بالدروس / Error-to-Lesson Mapping
    // ==================================================================
    
    /**
     * @brief (AR) الحصول على درس مرتبط برمز خطأ معين
     * @brief (EN) Get lesson related to a specific error code
     * 
     * @param error_code (std::string) — (AR) رمز الخطأ (مثل ص0001)
     *                                   (EN) Error code (e.g., ص0001)
     * @return (std::optional<Lesson>) — (AR) الدرس المرتبط إن وجد
     *                                   (EN) Related lesson if found
     */
    std::optional<Lesson> getLessonForError(const std::string& error_code) const;
    
    /**
     * @brief (AR) ربط رمز خطأ بدرس معين
     * @brief (EN) Link an error code to a specific lesson
     */
    void mapErrorToLesson(const std::string& error_code, const std::string& lesson_id);
    
private:
    // منشئ خاص / Private constructor
    LessonContentManager();
    
    // تخزين الدروس / Lesson storage
    std::unordered_map<std::string, Lesson> lessons_;
    
    // ربط الأخطاء بالدروس / Error to lesson mapping
    std::unordered_map<std::string, std::string> error_lesson_map_;
    
    // مسار مجلد الدروس / Lessons directory path
    std::string lessons_directory_;
    
    // هل تم التحميل؟ / Has been loaded?
    bool loaded_;
};

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

/**
 * @brief (AR) تحويل نوع الدرس إلى نص عربي
 * @brief (EN) Convert lesson type to Arabic text
 */
std::string lessonTypeToArabic(LessonType type);

/**
 * @brief (AR) تحويل نوع الدرس إلى نص إنجليزي
 * @brief (EN) Convert lesson type to English text
 */
std::string lessonTypeToEnglish(LessonType type);

/**
 * @brief (AR) تحويل مستوى الدرس إلى نص عربي
 * @brief (EN) Convert lesson level to Arabic text
 */
std::string lessonLevelToArabic(LessonLevel level);

/**
 * @brief (AR) تحويل فئة الدرس إلى نص عربي
 * @brief (EN) Convert lesson category to Arabic text
 */
std::string lessonCategoryToArabic(LessonCategory category);

} // namespace Teacher
} // namespace Sad

#endif // SAD_TEACHER_LESSON_CONTENT_H
