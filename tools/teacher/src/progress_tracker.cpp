// ======================================================================
// progress_tracker.cpp - نظام تتبع التقدم التعليمي
// Progress Tracking System for Educational Content
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   نظام لتتبع تقدم المستخدم في الدروس التعليمية. يحفظ معلومات
//   الدروس المكتملة، النقاط المكتسبة، الإنجازات، والإحصائيات.
//
// English Description:
//   System for tracking user progress in educational lessons. Saves
//   information about completed lessons, earned points, achievements,
//   and statistics.
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include "../include/progress_tracker.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace Sad {
namespace Teacher {

// ======================================================================
// ProgressTracker Implementation / تنفيذ متتبع التقدم
// ======================================================================

ProgressTracker& ProgressTracker::getInstance() {
    static ProgressTracker instance;
    return instance;
}

ProgressTracker::ProgressTracker() 
    : m_loaded(false), m_dirty(false), m_autoSave(true) {
    
    // تحديد مسار ملف التقدم
    m_progressFilePath = getDefaultProgressFilePath();
    
    // تهيئة ملف تعريف المستخدم الافتراضي
    m_profile.userId = generateUserId();
    m_profile.displayName = "متعلم ص";
    m_profile.createdAt = getCurrentTimestamp();
    m_profile.lastActiveAt = m_profile.createdAt;
    m_profile.totalPoints = 0;
    m_profile.currentStreak = 0;
    m_profile.longestStreak = 0;
    m_profile.currentLevel = "مبتدئ";
}

ProgressTracker::~ProgressTracker() {
    // حفظ التقدم عند الإغلاق
    if (m_dirty && m_autoSave) {
        save();
    }
}

// ======================================================================
// تحميل وحفظ التقدم / Loading and Saving Progress
// ======================================================================

/**
 * @brief (AR) تحميل بيانات التقدم من ملف
 * @brief (EN) Load progress data from file
 */
bool ProgressTracker::load() {
    return loadFromFile(m_progressFilePath);
}

bool ProgressTracker::loadFromFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        // الملف غير موجود - سيتم إنشاؤه عند الحفظ
        m_loaded = true;
        return true;
    }
    
    try {
        std::string line;
        std::string section;
        
        while (std::getline(file, line)) {
            // تجاهل الأسطر الفارغة والتعليقات
            if (line.empty() || line[0] == '#') continue;
            
            // التحقق من القسم
            if (line[0] == '[') {
                size_t end = line.find(']');
                if (end != std::string::npos) {
                    section = line.substr(1, end - 1);
                }
                continue;
            }
            
            // تحليل المفتاح والقيمة
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;
            
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            
            // إزالة المسافات
            while (!key.empty() && key.back() == ' ') key.pop_back();
            while (!value.empty() && value.front() == ' ') value.erase(0, 1);
            
            // معالجة حسب القسم
            if (section == "profile") {
                parseProfileLine(key, value);
            }
            else if (section == "lessons") {
                parseLessonLine(key, value);
            }
            else if (section == "achievements") {
                parseAchievementLine(key, value);
            }
            else if (section == "statistics") {
                parseStatisticsLine(key, value);
            }
        }
        
        m_loaded = true;
        m_dirty = false;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ خطأ في تحميل ملف التقدم: " << e.what() << "\n";
        return false;
    }
}

/**
 * @brief (AR) حفظ بيانات التقدم إلى ملف
 * @brief (EN) Save progress data to file
 */
bool ProgressTracker::save() {
    return saveToFile(m_progressFilePath);
}

bool ProgressTracker::saveToFile(const std::string& file_path) {
    // التأكد من وجود المجلد
    std::filesystem::path path(file_path);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "❌ فشل في فتح ملف التقدم للكتابة\n";
        return false;
    }
    
    try {
        // كتابة الرأس
        file << "# ملف تقدم أستاذ ص\n";
        file << "# Sad Teacher Progress File\n";
        file << "# تاريخ الحفظ: " << getCurrentTimestamp() << "\n\n";
        
        // قسم الملف التعريفي
        file << "[profile]\n";
        file << "userId=" << m_profile.userId << "\n";
        file << "displayName=" << m_profile.displayName << "\n";
        file << "createdAt=" << m_profile.createdAt << "\n";
        file << "lastActiveAt=" << getCurrentTimestamp() << "\n";
        file << "totalPoints=" << m_profile.totalPoints << "\n";
        file << "currentStreak=" << m_profile.currentStreak << "\n";
        file << "longestStreak=" << m_profile.longestStreak << "\n";
        file << "currentLevel=" << m_profile.currentLevel << "\n\n";
        
        // قسم الدروس
        file << "[lessons]\n";
        for (const auto& [id, progress] : m_lessonProgress) {
            file << id << ".status=" << static_cast<int>(progress.status) << "\n";
            file << id << ".attempts=" << progress.attempts << "\n";
            file << id << ".completedExercises=" << progress.completedExercises << "\n";
            file << id << ".totalExercises=" << progress.totalExercises << "\n";
            file << id << ".earnedPoints=" << progress.earnedPoints << "\n";
            file << id << ".bestScore=" << progress.bestScore << "\n";
            file << id << ".totalTimeMinutes=" << progress.totalTimeMinutes << "\n";
            file << id << ".lastAccessedAt=" << progress.lastAccessedAt << "\n";
            if (progress.completedAt != 0) {
                file << id << ".completedAt=" << progress.completedAt << "\n";
            }
        }
        file << "\n";
        
        // قسم الإنجازات
        file << "[achievements]\n";
        for (const auto& ach : m_achievements) {
            file << ach.id << ".unlockedAt=" << ach.unlockedAt << "\n";
        }
        file << "\n";
        
        // قسم الإحصائيات
        file << "[statistics]\n";
        file << "totalLessonsStarted=" << m_statistics.totalLessonsStarted << "\n";
        file << "totalLessonsCompleted=" << m_statistics.totalLessonsCompleted << "\n";
        file << "totalExercisesAttempted=" << m_statistics.totalExercisesAttempted << "\n";
        file << "totalExercisesPassed=" << m_statistics.totalExercisesPassed << "\n";
        file << "totalTimeMinutes=" << m_statistics.totalTimeMinutes << "\n";
        file << "totalHintsUsed=" << m_statistics.totalHintsUsed << "\n";
        file << "totalErrorsEncountered=" << m_statistics.totalErrorsEncountered << "\n";
        
        m_dirty = false;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ خطأ في حفظ ملف التقدم: " << e.what() << "\n";
        return false;
    }
}

// ======================================================================
// إدارة تقدم الدروس / Lesson Progress Management
// ======================================================================

/**
 * @brief (AR) بدء درس جديد
 * @brief (EN) Start a new lesson
 */
void ProgressTracker::startLesson(const std::string& lesson_id) {
    auto& progress = m_lessonProgress[lesson_id];
    
    if (progress.status == LessonProgressStatus::NOT_STARTED) {
        progress.status = LessonProgressStatus::IN_PROGRESS;
        m_statistics.totalLessonsStarted++;
    }
    
    progress.attempts++;
    progress.lastAccessedAt = std::time(nullptr);
    
    m_profile.lastActiveAt = getCurrentTimestamp();
    m_dirty = true;
    
    if (m_autoSave) save();
}

/**
 * @brief (AR) إكمال تمرين في درس
 * @brief (EN) Complete an exercise in a lesson
 */
void ProgressTracker::completeExercise(const std::string& lesson_id, 
                                        bool passed, 
                                        int points_earned) {
    auto& progress = m_lessonProgress[lesson_id];
    
    progress.completedExercises++;
    m_statistics.totalExercisesAttempted++;
    
    if (passed) {
        progress.earnedPoints += points_earned;
        m_profile.totalPoints += points_earned;
        m_statistics.totalExercisesPassed++;
    }
    
    m_dirty = true;
    
    // التحقق من إكمال الدرس
    checkLessonCompletion(lesson_id);
    
    // التحقق من الإنجازات
    checkAchievements();
    
    if (m_autoSave) save();
}

/**
 * @brief (AR) إكمال درس
 * @brief (EN) Complete a lesson
 */
void ProgressTracker::completeLesson(const std::string& lesson_id, int score) {
    auto& progress = m_lessonProgress[lesson_id];
    
    if (progress.status != LessonProgressStatus::COMPLETED) {
        progress.status = LessonProgressStatus::COMPLETED;
        progress.completedAt = std::time(nullptr);
        m_statistics.totalLessonsCompleted++;
        
        // تحديث أفضل نتيجة
        if (score > progress.bestScore) {
            progress.bestScore = score;
        }
        
        // تحديث الخط (streak)
        updateStreak();
    }
    
    m_dirty = true;
    
    // التحقق من الإنجازات
    checkAchievements();
    
    // تحديث المستوى
    updateLevel();
    
    if (m_autoSave) save();
}

/**
 * @brief (AR) الحصول على تقدم درس معين
 * @brief (EN) Get progress for a specific lesson
 */
LessonProgress ProgressTracker::getLessonProgress(const std::string& lesson_id) const {
    auto it = m_lessonProgress.find(lesson_id);
    if (it != m_lessonProgress.end()) {
        return it->second;
    }
    return LessonProgress(); // تقدم فارغ
}

/**
 * @brief (AR) التحقق من إكمال درس
 * @brief (EN) Check if a lesson is completed
 */
bool ProgressTracker::isLessonCompleted(const std::string& lesson_id) const {
    auto it = m_lessonProgress.find(lesson_id);
    if (it != m_lessonProgress.end()) {
        return it->second.status == LessonProgressStatus::COMPLETED;
    }
    return false;
}

/**
 * @brief (AR) الحصول على نسبة الإكمال لدرس
 * @brief (EN) Get completion percentage for a lesson
 */
float ProgressTracker::getLessonCompletionPercentage(const std::string& lesson_id) const {
    auto it = m_lessonProgress.find(lesson_id);
    if (it != m_lessonProgress.end() && it->second.totalExercises > 0) {
        return (static_cast<float>(it->second.completedExercises) / 
                it->second.totalExercises) * 100.0f;
    }
    return 0.0f;
}

// ======================================================================
// إدارة الإنجازات / Achievement Management
// ======================================================================

/**
 * @brief (AR) فتح إنجاز جديد
 * @brief (EN) Unlock a new achievement
 */
void ProgressTracker::unlockAchievement(const std::string& achievement_id) {
    // التحقق من عدم وجود الإنجاز مسبقاً
    for (const auto& ach : m_achievements) {
        if (ach.id == achievement_id) return;
    }
    
    Achievement ach;
    ach.id = achievement_id;
    ach.unlockedAt = std::time(nullptr);
    
    // الحصول على معلومات الإنجاز
    auto info = getAchievementInfo(achievement_id);
    ach.name = info.name;
    ach.description = info.description;
    ach.icon = info.icon;
    ach.points = info.points;
    
    m_achievements.push_back(ach);
    m_profile.totalPoints += ach.points;
    
    m_dirty = true;
    
    // إظهار رسالة الإنجاز
    printAchievementUnlocked(ach);
    
    if (m_autoSave) save();
}

/**
 * @brief (AR) التحقق من امتلاك إنجاز
 * @brief (EN) Check if an achievement is unlocked
 */
bool ProgressTracker::hasAchievement(const std::string& achievement_id) const {
    for (const auto& ach : m_achievements) {
        if (ach.id == achievement_id) return true;
    }
    return false;
}

/**
 * @brief (AR) الحصول على جميع الإنجازات
 * @brief (EN) Get all achievements
 */
std::vector<Achievement> ProgressTracker::getAchievements() const {
    return m_achievements;
}

// ======================================================================
// الإحصائيات / Statistics
// ======================================================================

/**
 * @brief (AR) الحصول على الإحصائيات العامة
 * @brief (EN) Get overall statistics
 */
LearningStatistics ProgressTracker::getStatistics() const {
    return m_statistics;
}

/**
 * @brief (AR) تسجيل استخدام تلميح
 * @brief (EN) Record hint usage
 */
void ProgressTracker::recordHintUsed() {
    m_statistics.totalHintsUsed++;
    m_dirty = true;
}

/**
 * @brief (AR) تسجيل مواجهة خطأ
 * @brief (EN) Record error encountered
 */
void ProgressTracker::recordErrorEncountered(const std::string& error_code) {
    m_statistics.totalErrorsEncountered++;
    m_statistics.errorFrequency[error_code]++;
    m_dirty = true;
}

/**
 * @brief (AR) تسجيل وقت التعلم
 * @brief (EN) Record learning time
 */
void ProgressTracker::recordTimeSpent(int minutes) {
    m_statistics.totalTimeMinutes += minutes;
    m_dirty = true;
}

// ======================================================================
// الملف التعريفي / Profile Management
// ======================================================================

UserProfile ProgressTracker::getProfile() const {
    return m_profile;
}

void ProgressTracker::setDisplayName(const std::string& name) {
    m_profile.displayName = name;
    m_dirty = true;
    if (m_autoSave) save();
}

int ProgressTracker::getTotalPoints() const {
    return m_profile.totalPoints;
}

std::string ProgressTracker::getCurrentLevel() const {
    return m_profile.currentLevel;
}

int ProgressTracker::getCurrentStreak() const {
    return m_profile.currentStreak;
}

// ======================================================================
// طباعة التقرير / Report Printing
// ======================================================================

/**
 * @brief (AR) طباعة تقرير التقدم
 * @brief (EN) Print progress report
 */
void ProgressTracker::printProgressReport() const {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              📊 تقرير التقدم التعليمي                         ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  👤 المتعلم: " << m_profile.displayName << "\n";
    std::cout << "║  🏆 المستوى: " << m_profile.currentLevel << "\n";
    std::cout << "║  ⭐ النقاط: " << m_profile.totalPoints << "\n";
    std::cout << "║  🔥 سلسلة الأيام: " << m_profile.currentStreak << " يوم\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  📚 الدروس:                                                  ║\n";
    std::cout << "║     • مكتملة: " << m_statistics.totalLessonsCompleted << "\n";
    std::cout << "║     • قيد التقدم: " 
              << (m_statistics.totalLessonsStarted - m_statistics.totalLessonsCompleted) << "\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  📝 التمارين:                                                ║\n";
    std::cout << "║     • محاولات: " << m_statistics.totalExercisesAttempted << "\n";
    std::cout << "║     • ناجحة: " << m_statistics.totalExercisesPassed << "\n";
    if (m_statistics.totalExercisesAttempted > 0) {
        float successRate = (static_cast<float>(m_statistics.totalExercisesPassed) / 
                            m_statistics.totalExercisesAttempted) * 100.0f;
        std::cout << "║     • نسبة النجاح: " << std::fixed << std::setprecision(1) 
                  << successRate << "%\n";
    }
    std::cout << "║                                                              ║\n";
    std::cout << "║  ⏱️ الوقت الإجمالي: " << m_statistics.totalTimeMinutes << " دقيقة\n";
    std::cout << "║  💡 تلميحات مستخدمة: " << m_statistics.totalHintsUsed << "\n";
    std::cout << "║                                                              ║\n";
    
    // الإنجازات
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  🏅 الإنجازات (" << m_achievements.size() << "):\n";
    if (m_achievements.empty()) {
        std::cout << "║     لم تحصل على إنجازات بعد - استمر في التعلم!              ║\n";
    } else {
        for (const auto& ach : m_achievements) {
            std::cout << "║     " << ach.icon << " " << ach.name << "\n";
        }
    }
    
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

// ======================================================================
// الدوال المساعدة الداخلية / Internal Helper Functions
// ======================================================================

std::string ProgressTracker::getDefaultProgressFilePath() const {
    std::string base_path;
    
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        base_path = std::string(path) + "\\SadLang\\";
    } else {
        base_path = ".\\";
    }
#else
    const char* home = getenv("HOME");
    if (home == nullptr) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    base_path = std::string(home) + "/.sadlang/";
#endif
    
    return base_path + "progress.ini";
}

std::string ProgressTracker::generateUserId() const {
    // توليد معرف فريد بسيط
    std::time_t now = std::time(nullptr);
    std::stringstream ss;
    ss << "user_" << now << "_" << (rand() % 10000);
    return ss.str();
}

std::string ProgressTracker::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    
    std::stringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void ProgressTracker::parseProfileLine(const std::string& key, const std::string& value) {
    if (key == "userId") m_profile.userId = value;
    else if (key == "displayName") m_profile.displayName = value;
    else if (key == "createdAt") m_profile.createdAt = value;
    else if (key == "lastActiveAt") m_profile.lastActiveAt = value;
    else if (key == "totalPoints") m_profile.totalPoints = std::stoi(value);
    else if (key == "currentStreak") m_profile.currentStreak = std::stoi(value);
    else if (key == "longestStreak") m_profile.longestStreak = std::stoi(value);
    else if (key == "currentLevel") m_profile.currentLevel = value;
}

void ProgressTracker::parseLessonLine(const std::string& key, const std::string& value) {
    // تحليل المفتاح: lesson_id.property
    size_t dot = key.find('.');
    if (dot == std::string::npos) return;
    
    std::string lesson_id = key.substr(0, dot);
    std::string property = key.substr(dot + 1);
    
    auto& progress = m_lessonProgress[lesson_id];
    
    if (property == "status") progress.status = static_cast<LessonProgressStatus>(std::stoi(value));
    else if (property == "attempts") progress.attempts = std::stoi(value);
    else if (property == "completedExercises") progress.completedExercises = std::stoi(value);
    else if (property == "totalExercises") progress.totalExercises = std::stoi(value);
    else if (property == "earnedPoints") progress.earnedPoints = std::stoi(value);
    else if (property == "bestScore") progress.bestScore = std::stoi(value);
    else if (property == "totalTimeMinutes") progress.totalTimeMinutes = std::stoi(value);
    else if (property == "lastAccessedAt") progress.lastAccessedAt = std::stoll(value);
    else if (property == "completedAt") progress.completedAt = std::stoll(value);
}

void ProgressTracker::parseAchievementLine(const std::string& key, const std::string& value) {
    // تحليل المفتاح: achievement_id.property
    size_t dot = key.find('.');
    if (dot == std::string::npos) return;
    
    std::string ach_id = key.substr(0, dot);
    std::string property = key.substr(dot + 1);
    
    // البحث عن الإنجاز أو إنشاؤه
    Achievement* ach = nullptr;
    for (auto& a : m_achievements) {
        if (a.id == ach_id) {
            ach = &a;
            break;
        }
    }
    
    if (ach == nullptr) {
        m_achievements.push_back(Achievement());
        ach = &m_achievements.back();
        ach->id = ach_id;
        
        // الحصول على معلومات الإنجاز
        auto info = getAchievementInfo(ach_id);
        ach->name = info.name;
        ach->description = info.description;
        ach->icon = info.icon;
        ach->points = info.points;
    }
    
    if (property == "unlockedAt") ach->unlockedAt = std::stoll(value);
}

void ProgressTracker::parseStatisticsLine(const std::string& key, const std::string& value) {
    if (key == "totalLessonsStarted") m_statistics.totalLessonsStarted = std::stoi(value);
    else if (key == "totalLessonsCompleted") m_statistics.totalLessonsCompleted = std::stoi(value);
    else if (key == "totalExercisesAttempted") m_statistics.totalExercisesAttempted = std::stoi(value);
    else if (key == "totalExercisesPassed") m_statistics.totalExercisesPassed = std::stoi(value);
    else if (key == "totalTimeMinutes") m_statistics.totalTimeMinutes = std::stoi(value);
    else if (key == "totalHintsUsed") m_statistics.totalHintsUsed = std::stoi(value);
    else if (key == "totalErrorsEncountered") m_statistics.totalErrorsEncountered = std::stoi(value);
}

void ProgressTracker::checkLessonCompletion(const std::string& lesson_id) {
    auto& progress = m_lessonProgress[lesson_id];
    
    if (progress.completedExercises >= progress.totalExercises &&
        progress.totalExercises > 0) {
        completeLesson(lesson_id, progress.bestScore);
    }
}

void ProgressTracker::checkAchievements() {
    // أول درس مكتمل
    if (m_statistics.totalLessonsCompleted >= 1 && !hasAchievement("first_lesson")) {
        unlockAchievement("first_lesson");
    }
    
    // 5 دروس مكتملة
    if (m_statistics.totalLessonsCompleted >= 5 && !hasAchievement("five_lessons")) {
        unlockAchievement("five_lessons");
    }
    
    // 10 دروس مكتملة
    if (m_statistics.totalLessonsCompleted >= 10 && !hasAchievement("ten_lessons")) {
        unlockAchievement("ten_lessons");
    }
    
    // سلسلة 7 أيام
    if (m_profile.currentStreak >= 7 && !hasAchievement("week_streak")) {
        unlockAchievement("week_streak");
    }
    
    // 100 نقطة
    if (m_profile.totalPoints >= 100 && !hasAchievement("hundred_points")) {
        unlockAchievement("hundred_points");
    }
    
    // 500 نقطة
    if (m_profile.totalPoints >= 500 && !hasAchievement("five_hundred_points")) {
        unlockAchievement("five_hundred_points");
    }
    
    // بدون تلميحات في 5 تمارين متتالية
    // (يحتاج تتبع إضافي)
}

void ProgressTracker::updateStreak() {
    // تحديث سلسلة الأيام المتتالية
    // (منطق مبسط - يحتاج تتبع التاريخ اليومي)
    m_profile.currentStreak++;
    if (m_profile.currentStreak > m_profile.longestStreak) {
        m_profile.longestStreak = m_profile.currentStreak;
    }
}

void ProgressTracker::updateLevel() {
    // تحديث المستوى بناءً على النقاط
    int points = m_profile.totalPoints;
    
    if (points >= 5000) {
        m_profile.currentLevel = "خبير";
    } else if (points >= 2000) {
        m_profile.currentLevel = "متقدم جداً";
    } else if (points >= 1000) {
        m_profile.currentLevel = "متقدم";
    } else if (points >= 500) {
        m_profile.currentLevel = "متوسط";
    } else if (points >= 100) {
        m_profile.currentLevel = "مبتدئ متقدم";
    } else {
        m_profile.currentLevel = "مبتدئ";
    }
}

AchievementInfo ProgressTracker::getAchievementInfo(const std::string& achievement_id) const {
    // قاعدة بيانات الإنجازات
    static std::map<std::string, AchievementInfo> achievements = {
        {"first_lesson", {"الخطوة الأولى", "أكملت أول درس لك", "🎯", 10}},
        {"five_lessons", {"متعلم نشيط", "أكملت 5 دروس", "📚", 25}},
        {"ten_lessons", {"طالب مجتهد", "أكملت 10 دروس", "🎓", 50}},
        {"week_streak", {"مثابر", "تعلمت 7 أيام متتالية", "🔥", 30}},
        {"hundred_points", {"جامع النقاط", "حصلت على 100 نقطة", "⭐", 15}},
        {"five_hundred_points", {"نجم ساطع", "حصلت على 500 نقطة", "🌟", 40}},
        {"ownership_master", {"سيد الملكية", "أتقنت نظام الملكية", "👑", 100}},
        {"no_hints", {"عبقري", "أكملت 5 تمارين بدون تلميحات", "🧠", 35}},
        {"speed_learner", {"سريع التعلم", "أكملت درساً في أقل من 10 دقائق", "⚡", 20}},
        {"night_owl", {"بومة الليل", "تعلمت بعد منتصف الليل", "🦉", 10}},
    };
    
    auto it = achievements.find(achievement_id);
    if (it != achievements.end()) {
        return it->second;
    }
    
    return {"إنجاز غير معروف", "وصف غير متوفر", "🏅", 0};
}

void ProgressTracker::printAchievementUnlocked(const Achievement& ach) const {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          🎉 تهانينا! حصلت على إنجاز جديد!                    ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║     " << ach.icon << " " << ach.name << "\n";
    std::cout << "║     " << ach.description << "\n";
    std::cout << "║     +" << ach.points << " نقطة\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

} // namespace Teacher
} // namespace Sad
