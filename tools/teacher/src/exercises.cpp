// ======================================================================
// exercises.cpp - نظام التمارين التفاعلية
// Interactive Exercises System Implementation
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   تنفيذ نظام التمارين التفاعلية الذي يتيح للمتعلمين ممارسة
//   المفاهيم التي تعلموها. يدعم أنواعاً مختلفة من التمارين:
//   - إكمال الكود
//   - إصلاح الأخطاء
//   - الاختيار من متعدد
//   - كتابة كود من الصفر
//
// English Description:
//   Implementation of interactive exercises system that allows learners
//   to practice concepts they've learned. Supports different types:
//   - Code completion
//   - Bug fixing
//   - Multiple choice
//   - Writing code from scratch
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <algorithm>
#include <sstream>

namespace Sad {
namespace Teacher {

// ======================================================================
// أنواع التمارين / Exercise Types
// ======================================================================

/**
 * @enum ExerciseType
 * @brief (AR) أنواع التمارين المدعومة
 * @brief (EN) Supported exercise types
 */
enum class ExerciseType {
    CODE_COMPLETION,      // إكمال الكود - املأ الفراغ
    BUG_FIX,              // إصلاح الخطأ
    MULTIPLE_CHOICE,      // اختيار من متعدد
    WRITE_CODE,           // كتابة كود كامل
    REORDER_CODE,         // إعادة ترتيب السطور
    MATCH_OUTPUT,         // تطابق المخرجات
    PREDICT_OUTPUT,       // توقع المخرجات
    FIND_ERROR            // إيجاد الخطأ
};

/**
 * @enum ExerciseDifficulty
 * @brief (AR) مستوى صعوبة التمرين
 * @brief (EN) Exercise difficulty level
 */
enum class ExerciseDifficulty {
    EASY,           // سهل - 5 نقاط
    MEDIUM,         // متوسط - 10 نقاط
    HARD,           // صعب - 20 نقاط
    EXPERT          // خبير - 30 نقاط
};

// ======================================================================
// هياكل البيانات / Data Structures
// ======================================================================

/**
 * @struct ExerciseChoice
 * @brief (AR) خيار في تمرين الاختيار من متعدد
 * @brief (EN) Choice in multiple choice exercise
 */
struct ExerciseChoice {
    std::string id;                     // معرف الخيار (أ، ب، ج، د)
    std::string text;                   // نص الخيار
    bool isCorrect;                     // هل هو الإجابة الصحيحة
    std::string explanation;            // شرح لماذا صحيح/خطأ
};

/**
 * @struct ExerciseHint
 * @brief (AR) تلميح للتمرين
 * @brief (EN) Exercise hint
 */
struct ExerciseHint {
    int level;                          // مستوى التلميح (1 = عام، 3 = مفصل)
    std::string text;                   // نص التلميح
    int pointsDeduction;                // خصم النقاط عند استخدامه
};

/**
 * @struct ExerciseTest
 * @brief (AR) حالة اختبار للتمرين
 * @brief (EN) Test case for exercise
 */
struct ExerciseTest {
    std::string name;                   // اسم الاختبار
    std::string input;                  // المدخلات
    std::string expectedOutput;         // المخرجات المتوقعة
    bool isHidden;                      // هل الاختبار مخفي
};

/**
 * @struct Exercise
 * @brief (AR) بنية التمرين الكاملة
 * @brief (EN) Complete exercise structure
 */
struct Exercise {
    std::string id;                             // معرف التمرين
    std::string lessonId;                       // معرف الدرس المرتبط
    ExerciseType type;                          // نوع التمرين
    ExerciseDifficulty difficulty;              // مستوى الصعوبة
    
    std::string titleArabic;                    // العنوان بالعربية
    std::string titleEnglish;                   // العنوان بالإنجليزية
    std::string descriptionArabic;              // الوصف بالعربية
    std::string descriptionEnglish;             // الوصف بالإنجليزية
    
    std::string starterCode;                    // الكود المبدئي
    std::string solutionCode;                   // الحل النموذجي
    std::string solutionExplanation;            // شرح الحل
    
    std::vector<ExerciseChoice> choices;        // خيارات (للاختيار من متعدد)
    std::vector<ExerciseHint> hints;            // التلميحات
    std::vector<ExerciseTest> tests;            // حالات الاختبار
    
    std::vector<std::string> keywords;          // كلمات مفتاحية للتحقق
    std::vector<std::string> forbiddenPatterns; // أنماط ممنوعة
    
    int basePoints;                             // النقاط الأساسية
    int timeLimit;                              // الوقت المحدد بالثواني (0 = بلا حد)
    int maxAttempts;                            // أقصى عدد محاولات (0 = بلا حد)
};

/**
 * @struct ExerciseResult
 * @brief (AR) نتيجة تنفيذ التمرين
 * @brief (EN) Exercise execution result
 */
struct ExerciseResult {
    bool passed;                                // هل نجح
    int earnedPoints;                           // النقاط المكتسبة
    int totalPoints;                            // إجمالي النقاط الممكنة
    std::string feedback;                       // التغذية الراجعة
    std::string detailedExplanation;            // شرح مفصل
    std::vector<std::string> passedTests;       // الاختبارات الناجحة
    std::vector<std::string> failedTests;       // الاختبارات الفاشلة
    int hintsUsed;                              // التلميحات المستخدمة
    int timeSpent;                              // الوقت المستغرق بالثواني
};

// ======================================================================
// فئة محرك التمارين / Exercise Engine Class
// ======================================================================

/**
 * @class ExerciseEngine
 * @brief (AR) محرك تنفيذ وتقييم التمارين
 * @brief (EN) Engine for executing and evaluating exercises
 */
class ExerciseEngine {
public:
    ExerciseEngine() = default;
    ~ExerciseEngine() = default;
    
    // ============================================================
    // تحميل التمارين / Loading Exercises
    // ============================================================
    
    /**
     * @brief (AR) تحميل التمارين المضمنة
     * @brief (EN) Load embedded exercises
     */
    void loadEmbeddedExercises() {
        loadOwnershipExercises();
        loadBorrowingExercises();
        loadPatternMatchingExercises();
        loadBasicsExercises();
    }
    
    /**
     * @brief (AR) الحصول على تمرين بمعرفه
     * @brief (EN) Get exercise by ID
     */
    const Exercise* getExercise(const std::string& exercise_id) const {
        auto it = m_exercises.find(exercise_id);
        if (it != m_exercises.end()) {
            return &(it->second);
        }
        return nullptr;
    }
    
    /**
     * @brief (AR) الحصول على تمارين درس معين
     * @brief (EN) Get exercises for a specific lesson
     */
    std::vector<const Exercise*> getExercisesForLesson(const std::string& lesson_id) const {
        std::vector<const Exercise*> result;
        for (const auto& [id, ex] : m_exercises) {
            if (ex.lessonId == lesson_id) {
                result.push_back(&ex);
            }
        }
        return result;
    }
    
    // ============================================================
    // تقييم الإجابات / Evaluating Answers
    // ============================================================
    
    /**
     * @brief (AR) تقييم إجابة تمرين
     * @brief (EN) Evaluate exercise answer
     */
    ExerciseResult evaluateAnswer(const std::string& exercise_id, 
                                   const std::string& answer,
                                   int hints_used = 0,
                                   int time_spent = 0) {
        ExerciseResult result;
        result.passed = false;
        result.earnedPoints = 0;
        result.hintsUsed = hints_used;
        result.timeSpent = time_spent;
        
        const Exercise* exercise = getExercise(exercise_id);
        if (!exercise) {
            result.feedback = "❌ التمرين غير موجود";
            return result;
        }
        
        result.totalPoints = exercise->basePoints;
        
        // التقييم حسب نوع التمرين
        switch (exercise->type) {
            case ExerciseType::MULTIPLE_CHOICE:
                evaluateMultipleChoice(*exercise, answer, result);
                break;
            case ExerciseType::CODE_COMPLETION:
                evaluateCodeCompletion(*exercise, answer, result);
                break;
            case ExerciseType::BUG_FIX:
                evaluateBugFix(*exercise, answer, result);
                break;
            case ExerciseType::WRITE_CODE:
                evaluateWriteCode(*exercise, answer, result);
                break;
            case ExerciseType::PREDICT_OUTPUT:
                evaluatePredictOutput(*exercise, answer, result);
                break;
            default:
                evaluateGeneric(*exercise, answer, result);
                break;
        }
        
        // خصم نقاط التلميحات
        if (hints_used > 0 && result.passed) {
            int deduction = 0;
            for (int i = 0; i < hints_used && i < (int)exercise->hints.size(); ++i) {
                deduction += exercise->hints[i].pointsDeduction;
            }
            result.earnedPoints = std::max(0, result.earnedPoints - deduction);
        }
        
        return result;
    }
    
    // ============================================================
    // التلميحات / Hints
    // ============================================================
    
    /**
     * @brief (AR) الحصول على تلميح
     * @brief (EN) Get a hint
     */
    std::string getHint(const std::string& exercise_id, int hint_level) const {
        const Exercise* exercise = getExercise(exercise_id);
        if (!exercise) return "";
        
        for (const auto& hint : exercise->hints) {
            if (hint.level == hint_level) {
                return hint.text;
            }
        }
        
        return "";
    }
    
    /**
     * @brief (AR) عدد التلميحات المتاحة
     * @brief (EN) Number of available hints
     */
    int getHintCount(const std::string& exercise_id) const {
        const Exercise* exercise = getExercise(exercise_id);
        if (!exercise) return 0;
        return static_cast<int>(exercise->hints.size());
    }
    
    // ============================================================
    // عرض التمرين / Display Exercise
    // ============================================================
    
    /**
     * @brief (AR) طباعة التمرين
     * @brief (EN) Print exercise
     */
    void printExercise(const std::string& exercise_id) const {
        const Exercise* exercise = getExercise(exercise_id);
        if (!exercise) {
            std::cout << "❌ التمرين غير موجود\n";
            return;
        }
        
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  📝 " << exercise->titleArabic << "\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║  " << getDifficultyString(exercise->difficulty) 
                  << " | " << exercise->basePoints << " نقطة";
        if (exercise->timeLimit > 0) {
            std::cout << " | ⏱️ " << exercise->timeLimit << " ثانية";
        }
        std::cout << "\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  📋 المطلوب:                                                  ║\n";
        std::cout << "║  " << exercise->descriptionArabic << "\n";
        std::cout << "║                                                              ║\n";
        
        // عرض الكود المبدئي
        if (!exercise->starterCode.empty()) {
            std::cout << "║  📄 الكود المبدئي:                                           ║\n";
            std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << exercise->starterCode << "\n";
            std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        }
        
        // عرض الخيارات (للاختيار من متعدد)
        if (exercise->type == ExerciseType::MULTIPLE_CHOICE) {
            std::cout << "║  🔘 الخيارات:                                                ║\n";
            for (const auto& choice : exercise->choices) {
                std::cout << "║     " << choice.id << ". " << choice.text << "\n";
            }
            std::cout << "║                                                              ║\n";
        }
        
        std::cout << "║  💡 تلميحات متاحة: " << exercise->hints.size() << "\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }
    
    /**
     * @brief (AR) طباعة النتيجة
     * @brief (EN) Print result
     */
    void printResult(const ExerciseResult& result) const {
        std::cout << "\n";
        if (result.passed) {
            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║          ✅ أحسنت! إجابة صحيحة!                               ║\n";
            std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << "║  🎯 النقاط: " << result.earnedPoints << " / " 
                      << result.totalPoints << "\n";
        } else {
            std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║          ❌ حاول مرة أخرى                                     ║\n";
            std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        }
        
        std::cout << "║                                                              ║\n";
        std::cout << "║  💬 " << result.feedback << "\n";
        
        if (!result.detailedExplanation.empty()) {
            std::cout << "║                                                              ║\n";
            std::cout << "║  📖 " << result.detailedExplanation << "\n";
        }
        
        // عرض الاختبارات
        if (!result.passedTests.empty() || !result.failedTests.empty()) {
            std::cout << "║                                                              ║\n";
            for (const auto& test : result.passedTests) {
                std::cout << "║  ✓ " << test << "\n";
            }
            for (const auto& test : result.failedTests) {
                std::cout << "║  ✗ " << test << "\n";
            }
        }
        
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }

private:
    std::map<std::string, Exercise> m_exercises;
    
    // ============================================================
    // دوال التقييم الداخلية / Internal Evaluation Functions
    // ============================================================
    
    void evaluateMultipleChoice(const Exercise& exercise, 
                                const std::string& answer,
                                ExerciseResult& result) {
        std::string normalizedAnswer = answer;
        // تحويل الأحرف العربية إلى إنجليزية
        if (normalizedAnswer == "أ" || normalizedAnswer == "ا") normalizedAnswer = "a";
        else if (normalizedAnswer == "ب") normalizedAnswer = "b";
        else if (normalizedAnswer == "ج") normalizedAnswer = "c";
        else if (normalizedAnswer == "د") normalizedAnswer = "d";
        
        // تحويل إلى حرف صغير
        std::transform(normalizedAnswer.begin(), normalizedAnswer.end(), 
                      normalizedAnswer.begin(), ::tolower);
        
        for (const auto& choice : exercise.choices) {
            std::string choiceId = choice.id;
            std::transform(choiceId.begin(), choiceId.end(), choiceId.begin(), ::tolower);
            
            if (choiceId == normalizedAnswer) {
                if (choice.isCorrect) {
                    result.passed = true;
                    result.earnedPoints = exercise.basePoints;
                    result.feedback = "إجابة صحيحة!";
                    result.detailedExplanation = choice.explanation;
                } else {
                    result.passed = false;
                    result.feedback = "إجابة خاطئة";
                    result.detailedExplanation = choice.explanation;
                }
                return;
            }
        }
        
        result.feedback = "خيار غير صالح. اختر من الخيارات المتاحة.";
    }
    
    void evaluateCodeCompletion(const Exercise& exercise,
                                const std::string& answer,
                                ExerciseResult& result) {
        // التحقق من وجود الكلمات المفتاحية المطلوبة
        bool hasAllKeywords = true;
        for (const auto& keyword : exercise.keywords) {
            if (answer.find(keyword) == std::string::npos) {
                hasAllKeywords = false;
                result.failedTests.push_back("لم يتم العثور على: " + keyword);
            } else {
                result.passedTests.push_back("تم العثور على: " + keyword);
            }
        }
        
        // التحقق من عدم وجود أنماط ممنوعة
        bool hasForbidden = false;
        for (const auto& pattern : exercise.forbiddenPatterns) {
            if (answer.find(pattern) != std::string::npos) {
                hasForbidden = true;
                result.failedTests.push_back("تم استخدام نمط ممنوع: " + pattern);
            }
        }
        
        if (hasAllKeywords && !hasForbidden) {
            result.passed = true;
            result.earnedPoints = exercise.basePoints;
            result.feedback = "كود صحيح! أحسنت!";
            result.detailedExplanation = exercise.solutionExplanation;
        } else {
            result.feedback = "الكود يحتاج تعديلات";
            result.detailedExplanation = "راجع الاختبارات الفاشلة أعلاه";
        }
    }
    
    void evaluateBugFix(const Exercise& exercise,
                        const std::string& answer,
                        ExerciseResult& result) {
        // التحقق من إصلاح الخطأ
        bool hasAllKeywords = true;
        for (const auto& keyword : exercise.keywords) {
            if (answer.find(keyword) == std::string::npos) {
                hasAllKeywords = false;
            }
        }
        
        // التحقق من إزالة الكود الخاطئ
        bool fixedBugs = true;
        for (const auto& pattern : exercise.forbiddenPatterns) {
            if (answer.find(pattern) != std::string::npos) {
                fixedBugs = false;
                result.failedTests.push_back("الخطأ لا يزال موجوداً");
            }
        }
        
        if (hasAllKeywords && fixedBugs) {
            result.passed = true;
            result.earnedPoints = exercise.basePoints;
            result.feedback = "تم إصلاح الخطأ بنجاح!";
            result.detailedExplanation = exercise.solutionExplanation;
        } else {
            result.feedback = "الخطأ لم يُصلح بالكامل";
        }
    }
    
    void evaluateWriteCode(const Exercise& exercise,
                           const std::string& answer,
                           ExerciseResult& result) {
        // تقييم مشابه لإكمال الكود
        evaluateCodeCompletion(exercise, answer, result);
    }
    
    void evaluatePredictOutput(const Exercise& exercise,
                               const std::string& answer,
                               ExerciseResult& result) {
        // مقارنة المخرجات المتوقعة
        for (const auto& test : exercise.tests) {
            std::string normalizedAnswer = answer;
            std::string normalizedExpected = test.expectedOutput;
            
            // إزالة المسافات الزائدة
            while (!normalizedAnswer.empty() && 
                   (normalizedAnswer.back() == ' ' || normalizedAnswer.back() == '\n')) {
                normalizedAnswer.pop_back();
            }
            while (!normalizedExpected.empty() && 
                   (normalizedExpected.back() == ' ' || normalizedExpected.back() == '\n')) {
                normalizedExpected.pop_back();
            }
            
            if (normalizedAnswer == normalizedExpected) {
                result.passed = true;
                result.earnedPoints = exercise.basePoints;
                result.feedback = "توقع صحيح!";
                result.passedTests.push_back(test.name);
            } else {
                result.failedTests.push_back(test.name + " - المتوقع: " + normalizedExpected);
            }
        }
        
        if (!result.passed) {
            result.feedback = "التوقع غير صحيح";
        }
    }
    
    void evaluateGeneric(const Exercise& exercise,
                         const std::string& answer,
                         ExerciseResult& result) {
        // تقييم عام
        if (answer == exercise.solutionCode) {
            result.passed = true;
            result.earnedPoints = exercise.basePoints;
            result.feedback = "إجابة صحيحة!";
        } else {
            result.feedback = "حاول مرة أخرى";
        }
    }
    
    std::string getDifficultyString(ExerciseDifficulty diff) const {
        switch (diff) {
            case ExerciseDifficulty::EASY: return "🟢 سهل";
            case ExerciseDifficulty::MEDIUM: return "🟡 متوسط";
            case ExerciseDifficulty::HARD: return "🔴 صعب";
            case ExerciseDifficulty::EXPERT: return "⚫ خبير";
            default: return "❓ غير محدد";
        }
    }
    
    // ============================================================
    // تحميل التمارين المضمنة / Loading Embedded Exercises
    // ============================================================
    
    void loadOwnershipExercises() {
        // تمرين 1: فهم الملكية الأساسي
        Exercise ex1;
        ex1.id = "ownership_basic_1";
        ex1.lessonId = "ownership_basics";
        ex1.type = ExerciseType::MULTIPLE_CHOICE;
        ex1.difficulty = ExerciseDifficulty::EASY;
        ex1.titleArabic = "فهم نقل الملكية";
        ex1.titleEnglish = "Understanding Ownership Transfer";
        ex1.descriptionArabic = "ما الذي يحدث عندما ننقل قيمة من متغير إلى آخر في لغة ص؟";
        ex1.descriptionEnglish = "What happens when we transfer a value from one variable to another in Sad?";
        
        ex1.choices = {
            {"أ", "يتم نسخ القيمة ويبقى المتغير الأصلي صالحاً", false, 
             "هذا ما يحدث في لغات مثل Python، لكن ليس في لغة ص"},
            {"ب", "تنتقل الملكية ويصبح المتغير الأصلي غير صالح", true,
             "صحيح! في لغة ص، نقل القيمة ينقل الملكية معها"},
            {"ج", "يتم مشاركة القيمة بين المتغيرين", false,
             "لا، المشاركة تتطلب استخدام الاستعارة (&)"},
            {"د", "يحدث خطأ في وقت التشغيل", false,
             "لا، نظام الملكية يمنع الأخطاء في وقت الترجمة"}
        };
        
        ex1.hints = {
            {1, "فكر في كيفية تعامل لغة ص مع الذاكرة", 1},
            {2, "الملكية تعني أن مالكاً واحداً فقط يمكنه الوصول للقيمة", 2},
            {3, "عند النقل، المتغير القديم يفقد ملكيته", 3}
        };
        
        ex1.basePoints = 10;
        m_exercises[ex1.id] = ex1;
        
        // تمرين 2: إكمال كود الملكية
        Exercise ex2;
        ex2.id = "ownership_complete_1";
        ex2.lessonId = "ownership_basics";
        ex2.type = ExerciseType::CODE_COMPLETION;
        ex2.difficulty = ExerciseDifficulty::MEDIUM;
        ex2.titleArabic = "إكمال كود الملكية";
        ex2.titleEnglish = "Complete Ownership Code";
        ex2.descriptionArabic = "أكمل الكود التالي لنقل ملكية النص بشكل صحيح";
        ex2.descriptionEnglish = "Complete the following code to correctly transfer string ownership";
        
        ex2.starterCode = R"(
دالة رئيسية() {
    متغير نص1 = "مرحباً"
    // أكمل السطر التالي لنقل الملكية
    متغير نص2 = _____
    اطبع(نص2)
}
)";
        
        ex2.solutionCode = "نص1";
        ex2.keywords = {"نص1"};
        ex2.solutionExplanation = "لنقل الملكية، نستخدم اسم المتغير مباشرة دون أي معاملات إضافية";
        
        ex2.hints = {
            {1, "الملكية تنتقل عند الإسناد المباشر", 2},
            {2, "استخدم اسم المتغير الأول مباشرة", 3},
            {3, "الجواب هو: نص1", 5}
        };
        
        ex2.basePoints = 15;
        m_exercises[ex2.id] = ex2;
    }
    
    void loadBorrowingExercises() {
        // تمرين الاستعارة 1
        Exercise ex;
        ex.id = "borrowing_basic_1";
        ex.lessonId = "borrowing_rules";
        ex.type = ExerciseType::BUG_FIX;
        ex.difficulty = ExerciseDifficulty::MEDIUM;
        ex.titleArabic = "إصلاح خطأ الاستعارة";
        ex.titleEnglish = "Fix Borrowing Bug";
        ex.descriptionArabic = "الكود التالي يحتوي خطأ في الاستعارة. أصلحه ليعمل بشكل صحيح";
        ex.descriptionEnglish = "The following code has a borrowing error. Fix it to work correctly";
        
        ex.starterCode = R"(
دالة رئيسية() {
    متغير نص = "مرحباً"
    متغير مرجع1 = &متغير نص    // استعارة قابلة للتعديل
    متغير مرجع2 = &متغير نص    // خطأ! استعارتان قابلتان للتعديل
    اطبع(مرجع1)
    اطبع(مرجع2)
}
)";
        
        ex.solutionCode = R"(
دالة رئيسية() {
    متغير نص = "مرحباً"
    متغير مرجع1 = &نص    // استعارة غير قابلة للتعديل
    متغير مرجع2 = &نص    // استعارة غير قابلة للتعديل - مسموح!
    اطبع(مرجع1)
    اطبع(مرجع2)
}
)";
        
        ex.keywords = {"&نص", "&نص"};
        ex.forbiddenPatterns = {"&متغير نص"};
        ex.solutionExplanation = "يمكن وجود استعارات متعددة غير قابلة للتعديل (&)، لكن استعارة واحدة قابلة للتعديل فقط (&متغير)";
        
        ex.hints = {
            {1, "فرق بين & و &متغير", 2},
            {2, "الاستعارة غير القابلة للتعديل تستخدم & فقط", 3},
            {3, "يمكن وجود عدة استعارات & في نفس الوقت", 5}
        };
        
        ex.basePoints = 20;
        m_exercises[ex.id] = ex;
    }
    
    void loadPatternMatchingExercises() {
        Exercise ex;
        ex.id = "pattern_match_1";
        ex.lessonId = "pattern_matching_intro";
        ex.type = ExerciseType::WRITE_CODE;
        ex.difficulty = ExerciseDifficulty::MEDIUM;
        ex.titleArabic = "كتابة مطابقة النمط";
        ex.titleEnglish = "Write Pattern Matching";
        ex.descriptionArabic = "اكتب دالة تستخدم طابق للتحقق من نوع القيمة";
        ex.descriptionEnglish = "Write a function using match to check value type";
        
        ex.starterCode = R"(
دالة تحقق_من_القيمة(قيمة: نوع_اختياري<عدد>) {
    // اكتب كود المطابقة هنا
}
)";
        
        ex.keywords = {"طابق", "بعض", "فارغ", "نهاية"};
        ex.solutionExplanation = "نستخدم طابق للتحقق من الحالات المختلفة للقيمة الاختيارية";
        
        ex.hints = {
            {1, "استخدم طابق للبدء", 2},
            {2, "تحقق من حالة بعض(ق) وحالة فارغ", 4},
            {3, "لا تنسَ نهاية في آخر الطابق", 5}
        };
        
        ex.basePoints = 25;
        m_exercises[ex.id] = ex;
    }
    
    void loadBasicsExercises() {
        // تمرين توقع المخرجات
        Exercise ex;
        ex.id = "basics_output_1";
        ex.lessonId = "type_basics";
        ex.type = ExerciseType::PREDICT_OUTPUT;
        ex.difficulty = ExerciseDifficulty::EASY;
        ex.titleArabic = "توقع المخرجات";
        ex.titleEnglish = "Predict Output";
        ex.descriptionArabic = "ما مخرجات الكود التالي؟";
        ex.descriptionEnglish = "What is the output of the following code?";
        
        ex.starterCode = R"(
دالة رئيسية() {
    متغير س = 5
    متغير ص = س + 3
    اطبع(ص)
}
)";
        
        ex.tests = {
            {"اختبار المخرجات", "", "8", false}
        };
        
        ex.hints = {
            {1, "قم بحساب العملية الحسابية", 1},
            {2, "5 + 3 = ?", 2}
        };
        
        ex.basePoints = 5;
        m_exercises[ex.id] = ex;
    }
};

// ======================================================================
// دالة إنشاء المحرك العامة / Global Engine Creation
// ======================================================================

/**
 * @brief (AR) الحصول على محرك التمارين (Singleton)
 * @brief (EN) Get exercise engine (Singleton)
 */
ExerciseEngine& getExerciseEngine() {
    static ExerciseEngine engine;
    static bool initialized = false;
    
    if (!initialized) {
        engine.loadEmbeddedExercises();
        initialized = true;
    }
    
    return engine;
}

} // namespace Teacher
} // namespace Sad
