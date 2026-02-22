// ======================================================================
// lesson_content.cpp - تنفيذ نظام محتوى الدروس / Lesson Content Implementation
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   تنفيذ كامل لنظام إدارة محتوى الدروس التعليمية
//   يشمل تحميل الدروس، البحث، ربط الأخطاء بالدروس
//
// English Description:
//   Complete implementation of lesson content management system
//   Includes lesson loading, searching, error-to-lesson mapping
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include "lesson_content.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

namespace Sad {
namespace Teacher {

// ======================================================================
// تنفيذ فئة الدرس / Lesson Class Implementation
// ======================================================================

Lesson::Lesson(const std::string& id)
    : id_(id)
    , type_(LessonType::TEXT)
    , level_(LessonLevel::BEGINNER)
    , category_(LessonCategory::BASICS)
    , estimated_minutes_(10)
{
    // منشئ بسيط مع معرف فقط
}

Lesson::Lesson(const std::string& id,
               const std::string& title_ar,
               const std::string& title_en,
               LessonType type,
               LessonLevel level,
               LessonCategory category)
    : id_(id)
    , title_ar_(title_ar)
    , title_en_(title_en)
    , type_(type)
    , level_(level)
    , category_(category)
    , estimated_minutes_(10)
{
    // منشئ كامل
}

void Lesson::addSection(const LessonSection& section) {
    sections_.push_back(section);
}

void Lesson::addVideo(const VideoInfo& video) {
    videos_.push_back(video);
}

void Lesson::addExercise(const ExerciseInfo& exercise) {
    exercises_.push_back(exercise);
}

void Lesson::addPrerequisite(const std::string& lesson_id) {
    prerequisites_.push_back(lesson_id);
}

void Lesson::addRelatedLesson(const std::string& lesson_id) {
    related_lessons_.push_back(lesson_id);
}

void Lesson::addTag(const std::string& tag) {
    tags_.push_back(tag);
}

std::string Lesson::toJson() const {
    // تحويل بسيط إلى JSON
    // ملاحظة: في الإصدار الإنتاجي، استخدم مكتبة JSON مثل nlohmann/json
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << id_ << "\",\n";
    ss << "  \"title_ar\": \"" << title_ar_ << "\",\n";
    ss << "  \"title_en\": \"" << title_en_ << "\",\n";
    ss << "  \"description_ar\": \"" << description_ar_ << "\",\n";
    ss << "  \"description_en\": \"" << description_en_ << "\",\n";
    ss << "  \"type\": " << static_cast<int>(type_) << ",\n";
    ss << "  \"level\": " << static_cast<int>(level_) << ",\n";
    ss << "  \"category\": " << static_cast<int>(category_) << ",\n";
    ss << "  \"estimated_minutes\": " << estimated_minutes_ << "\n";
    ss << "}\n";
    return ss.str();
}

std::optional<Lesson> Lesson::fromJson(const std::string& json) {
    // تحميل من JSON - تنفيذ مبسط
    // ملاحظة: في الإصدار الإنتاجي، استخدم مكتبة JSON
    
    // للتبسيط، نعيد درساً فارغاً
    // TODO: تنفيذ كامل مع مكتبة JSON
    return std::nullopt;
}

// ======================================================================
// تنفيذ مدير محتوى الدروس / Lesson Content Manager Implementation
// ======================================================================

LessonContentManager& LessonContentManager::getInstance() {
    static LessonContentManager instance;
    return instance;
}

LessonContentManager::LessonContentManager()
    : loaded_(false)
{
    // تهيئة ربط الأخطاء بالدروس المدمجة
    initializeErrorLessonMappings();
}

void LessonContentManager::initializeErrorLessonMappings() {
    // ======================================================================
    // ربط أخطاء الملكية بدروس نظام الملكية
    // Map ownership errors to ownership system lessons
    // ======================================================================
    
    // خطأ: استخدام قيمة بعد نقلها
    // Error: Use of moved value
    error_lesson_map_["ص0001"] = "ownership_basics";
    
    // خطأ: استعارة متغيرة أثناء استعارة موجودة
    // Error: Mutable borrow while immutable borrow exists
    error_lesson_map_["ص0002"] = "borrowing_rules";
    
    // خطأ: قيمة لا تعيش طويلاً بما فيه الكفاية
    // Error: Value does not live long enough
    error_lesson_map_["ص0003"] = "lifetimes_intro";
    
    // خطأ: لا يمكن الاستعارة كمتغير لأنه مستعار بالفعل
    // Error: Cannot borrow as mutable because it is already borrowed
    error_lesson_map_["ص0004"] = "borrowing_advanced";
    
    // خطأ: محاولة استخدام مرجع معلق
    // Error: Dangling reference attempt
    error_lesson_map_["ص0005"] = "dangling_references";
    
    // ======================================================================
    // ربط أخطاء الأنواع بدروس نظام الأنواع
    // Map type errors to type system lessons
    // ======================================================================
    
    // خطأ: عدم تطابق الأنواع
    // Error: Type mismatch
    error_lesson_map_["ص1001"] = "type_basics";
    
    // خطأ: نوع غير معروف
    // Error: Unknown type
    error_lesson_map_["ص1002"] = "type_declarations";
    
    // خطأ: لا يمكن استنتاج النوع
    // Error: Cannot infer type
    error_lesson_map_["ص1003"] = "type_inference";
    
    // ======================================================================
    // ربط أخطاء الدوال بدروس الدوال
    // Map function errors to function lessons
    // ======================================================================
    
    // خطأ: عدد المعاملات غير صحيح
    // Error: Incorrect number of arguments
    error_lesson_map_["ص2001"] = "functions_basics";
    
    // خطأ: الدالة لا تُرجع قيمة
    // Error: Function does not return value
    error_lesson_map_["ص2002"] = "return_values";
    
    // ======================================================================
    // ربط أخطاء مطابقة الأنماط
    // Map pattern matching errors
    // ======================================================================
    
    // خطأ: الأنماط غير شاملة
    // Error: Non-exhaustive patterns
    error_lesson_map_["ص3001"] = "pattern_matching_intro";
    
    // خطأ: نمط غير قابل للدحض في موقع يتطلب ذلك
    // Error: Irrefutable pattern in refutable context
    error_lesson_map_["ص3002"] = "pattern_matching_advanced";
}

bool LessonContentManager::loadLessonsFromDirectory(const std::string& lessons_dir) {
    lessons_directory_ = lessons_dir;
    
    try {
        if (!fs::exists(lessons_dir)) {
            std::cerr << "مجلد الدروس غير موجود: " << lessons_dir << std::endl;
            return false;
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(lessons_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".json" || ext == ".lesson") {
                    auto lesson = loadLessonFromFile(entry.path().string());
                    if (lesson) {
                        lessons_[lesson->getId()] = *lesson;
                    }
                }
            }
        }
        
        loaded_ = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تحميل الدروس: " << e.what() << std::endl;
        return false;
    }
}

std::optional<Lesson> LessonContentManager::loadLessonFromFile(const std::string& file_path) {
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return Lesson::fromJson(buffer.str());
        
    } catch (...) {
        return std::nullopt;
    }
}

void LessonContentManager::loadEmbeddedLessons() {
    // ======================================================================
    // تحميل الدروس المدمجة - Embedded Lessons
    // هذه الدروس مُترجمة مع البرنامج ومتاحة دائماً
    // ======================================================================
    
    // ----------------------------------------------------------------------
    // درس 1: أساسيات الملكية
    // Lesson 1: Ownership Basics
    // ----------------------------------------------------------------------
    {
        Lesson lesson("ownership_basics",
                      "أساسيات نظام الملكية في لغة ص",
                      "Ownership Basics in Sad Language",
                      LessonType::MIXED,
                      LessonLevel::BEGINNER,
                      LessonCategory::OWNERSHIP);
        
        lesson.setDescriptionArabic(
            "تعلّم أساسيات نظام الملكية الفريد في لغة ص. "
            "نظام الملكية هو ما يجعل لغة ص آمنة بدون جامع قمامة."
        );
        
        lesson.setDescriptionEnglish(
            "Learn the basics of Sad's unique ownership system. "
            "The ownership system is what makes Sad safe without a garbage collector."
        );
        
        LessonSection intro;
        intro.title_ar = "ما هي الملكية؟";
        intro.title_en = "What is Ownership?";
        intro.content_ar = R"(
في لغة ص، كل قيمة لها **مالك** واحد فقط في أي وقت.

القواعد الثلاث للملكية:
1. كل قيمة في ص لها متغير يُسمى مالكها
2. لا يمكن أن يكون هناك أكثر من مالك واحد في أي وقت
3. عندما يخرج المالك من النطاق، تُحذف القيمة

مثال بسيط:
)";
        intro.content_en = R"(
In Sad, every value has exactly **one owner** at any time.

The three rules of ownership:
1. Each value in Sad has a variable called its owner
2. There can only be one owner at a time
3. When the owner goes out of scope, the value is dropped

Simple example:
)";
        intro.code_example = R"(
دالة رئيسية()
    نص س = "مرحبا"    # س هو المالك
    نص ص = س          # الملكية انتقلت إلى ص
    # اطبع(س)         # ❌ خطأ! س لم يعد صالحاً
    اطبع(ص)           # ✅ صحيح
نهاية
)";
        intro.is_interactive = false;
        lesson.addSection(intro);
        
        // إضافة تمرين
        ExerciseInfo exercise;
        exercise.exercise_id = "ownership_ex1";
        exercise.title_ar = "تمرين: نقل الملكية";
        exercise.title_en = "Exercise: Moving Ownership";
        exercise.description_ar = "أصلح الكود التالي ليعمل بشكل صحيح";
        exercise.description_en = "Fix the following code to work correctly";
        exercise.starter_code = R"(
دالة رئيسية()
    نص رسالة = "مرحبا بالعالم"
    نص نسخة = رسالة
    اطبع(رسالة)    # هذا سيفشل
    اطبع(نسخة)
نهاية
)";
        exercise.expected_output = "مرحبا بالعالم\nمرحبا بالعالم\n";
        exercise.solution = R"(
دالة رئيسية()
    نص رسالة = "مرحبا بالعالم"
    نص نسخة = رسالة.انسخ()    # استخدم النسخ بدلاً من النقل
    اطبع(رسالة)
    اطبع(نسخة)
نهاية
)";
        exercise.hints.push_back("استخدم الدالة .انسخ() لإنشاء نسخة بدلاً من نقل الملكية");
        exercise.hints.push_back("أو يمكنك طباعة نسخة قبل رسالة");
        exercise.max_attempts = 3;
        
        lesson.addExercise(exercise);
        lesson.setEstimatedMinutes(15);
        lesson.addTag("ملكية");
        lesson.addTag("أساسيات");
        lesson.addTag("ownership");
        
        lessons_["ownership_basics"] = lesson;
    }
    
    // ----------------------------------------------------------------------
    // درس 2: قواعد الاستعارة
    // Lesson 2: Borrowing Rules
    // ----------------------------------------------------------------------
    {
        Lesson lesson("borrowing_rules",
                      "قواعد الاستعارة في لغة ص",
                      "Borrowing Rules in Sad Language",
                      LessonType::MIXED,
                      LessonLevel::BEGINNER,
                      LessonCategory::BORROWING);
        
        lesson.setDescriptionArabic(
            "تعلّم كيفية استعارة القيم بدون نقل ملكيتها. "
            "الاستعارة تتيح لك استخدام قيمة بدون امتلاكها."
        );
        
        LessonSection intro;
        intro.title_ar = "ما هي الاستعارة؟";
        intro.title_en = "What is Borrowing?";
        intro.content_ar = R"(
الاستعارة تتيح لك **الإشارة** إلى قيمة بدون امتلاكها.

أنواع الاستعارة:
1. **استعارة غير متغيرة** (&): للقراءة فقط، يمكن أن يكون هناك عدة استعارات
2. **استعارة متغيرة** (&متغير): للقراءة والكتابة، استعارة واحدة فقط

القاعدة الذهبية:
- إما استعارة متغيرة واحدة
- أو أي عدد من الاستعارات غير المتغيرة
- لكن ليس كلاهما معاً!
)";
        intro.code_example = R"(
دالة رئيسية()
    متغير نص س = "مرحبا"
    
    # استعارة غير متغيرة - للقراءة فقط
    دع مرجع1 = &س
    دع مرجع2 = &س    # ✅ يمكن عدة استعارات للقراءة
    اطبع(مرجع1)
    اطبع(مرجع2)
    
    # استعارة متغيرة - للتعديل
    دع مرجع_متغير = &متغير س
    مرجع_متغير = "وداعاً"
    اطبع(س)          # يطبع: وداعاً
نهاية
)";
        lesson.addSection(intro);
        lesson.addPrerequisite("ownership_basics");
        lesson.setEstimatedMinutes(20);
        
        lessons_["borrowing_rules"] = lesson;
    }
    
    // ----------------------------------------------------------------------
    // درس 3: مطابقة الأنماط
    // Lesson 3: Pattern Matching
    // ----------------------------------------------------------------------
    {
        Lesson lesson("pattern_matching_intro",
                      "مقدمة في مطابقة الأنماط",
                      "Introduction to Pattern Matching",
                      LessonType::MIXED,
                      LessonLevel::INTERMEDIATE,
                      LessonCategory::PATTERN_MATCHING);
        
        lesson.setDescriptionArabic(
            "مطابقة الأنماط هي طريقة قوية لمقارنة القيم واستخراج البيانات. "
            "تُستخدم مع الكلمة المفتاحية 'طابق'."
        );
        
        LessonSection intro;
        intro.title_ar = "استخدام طابق";
        intro.title_en = "Using Match";
        intro.content_ar = R"(
كلمة 'طابق' تتيح لك مقارنة قيمة مع عدة أنماط:

الصيغة الأساسية:
```
طابق قيمة {
    حالة نمط1: كود
    حالة نمط2: كود
    حالة _: كود_افتراضي
}
```

النمط _ يطابق أي قيمة (الحالة الافتراضية).
)";
        intro.code_example = R"(
دالة رئيسية()
    رقم درجة = 85
    
    طابق درجة {
        حالة 90..100: اطبع("ممتاز")
        حالة 80..89: اطبع("جيد جداً")
        حالة 70..79: اطبع("جيد")
        حالة 60..69: اطبع("مقبول")
        حالة _: اطبع("راسب")
    }
نهاية
)";
        lesson.addSection(intro);
        lesson.setEstimatedMinutes(25);
        
        lessons_["pattern_matching_intro"] = lesson;
    }
    
    // ----------------------------------------------------------------------
    // درس 4: أساسيات الأنواع
    // Lesson 4: Type Basics
    // ----------------------------------------------------------------------
    {
        Lesson lesson("type_basics",
                      "أساسيات نظام الأنواع",
                      "Type System Basics",
                      LessonType::TEXT,
                      LessonLevel::BEGINNER,
                      LessonCategory::BASICS);
        
        lesson.setDescriptionArabic(
            "تعرّف على أنواع البيانات الأساسية في لغة ص وكيفية استخدامها."
        );
        
        LessonSection intro;
        intro.title_ar = "الأنواع الأساسية";
        intro.title_en = "Basic Types";
        intro.content_ar = R"(
الأنواع الأساسية في لغة ص:

| النوع | الوصف | مثال |
|-------|-------|------|
| رقم | عدد صحيح | رقم س = 42 |
| عشري | عدد عشري | عشري ط = 3.14 |
| نص | سلسلة نصية | نص اسم = "أحمد" |
| منطقي | صحيح/خطأ | منطقي نشط = صحيح |
| حرف | حرف واحد | حرف ح = 'أ' |
)";
        intro.code_example = R"(
دالة رئيسية()
    # الأنواع الأساسية
    رقم عمر = 25
    عشري طول = 1.75
    نص اسم = "محمد"
    منطقي متزوج = خطأ
    
    اطبع("الاسم: " + اسم)
    اطبع("العمر: " + عمر.إلى_نص())
نهاية
)";
        lesson.addSection(intro);
        lesson.setEstimatedMinutes(10);
        
        lessons_["type_basics"] = lesson;
    }
    
    // ----------------------------------------------------------------------
    // درس 5: أساسيات الدوال
    // Lesson 5: Functions Basics
    // ----------------------------------------------------------------------
    {
        Lesson lesson("functions_basics",
                      "أساسيات الدوال في لغة ص",
                      "Function Basics in Sad Language",
                      LessonType::MIXED,
                      LessonLevel::BEGINNER,
                      LessonCategory::FUNCTIONS);
        
        lesson.setDescriptionArabic(
            "تعلّم كيفية إنشاء واستدعاء الدوال في لغة ص."
        );
        
        LessonSection intro;
        intro.title_ar = "تعريف الدوال";
        intro.title_en = "Defining Functions";
        intro.content_ar = R"(
الدوال هي كتل من الكود يمكن إعادة استخدامها.

صيغة تعريف الدالة:
```
دالة اسم_الدالة(معامل1: نوع، معامل2: نوع) ترجع نوع_الإرجاع
    # الكود
    إرجاع قيمة
نهاية
```

ملاحظات مهمة:
- اسم الدالة يجب أن يكون وصفياً
- المعاملات اختيارية
- نوع الإرجاع اختياري (افتراضي: فراغ)
)";
        intro.code_example = R"(
# دالة بدون معاملات
دالة قل_مرحبا()
    اطبع("مرحبا!")
نهاية

# دالة مع معاملات وإرجاع
دالة اجمع(رقم أ، رقم ب) ترجع رقم
    إرجاع أ + ب
نهاية

دالة رئيسية()
    قل_مرحبا()
    
    رقم نتيجة = اجمع(5، 3)
    اطبع("المجموع: " + نتيجة.إلى_نص())
نهاية
)";
        lesson.addSection(intro);
        lesson.setEstimatedMinutes(15);
        
        lessons_["functions_basics"] = lesson;
    }
    
    loaded_ = true;
}

std::optional<Lesson> LessonContentManager::getLesson(const std::string& lesson_id) const {
    auto it = lessons_.find(lesson_id);
    if (it != lessons_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Lesson> LessonContentManager::getLessonsByCategory(LessonCategory category) const {
    std::vector<Lesson> result;
    for (const auto& [id, lesson] : lessons_) {
        if (lesson.getCategory() == category) {
            result.push_back(lesson);
        }
    }
    return result;
}

std::vector<Lesson> LessonContentManager::getLessonsByLevel(LessonLevel level) const {
    std::vector<Lesson> result;
    for (const auto& [id, lesson] : lessons_) {
        if (lesson.getLevel() == level) {
            result.push_back(lesson);
        }
    }
    return result;
}

std::vector<Lesson> LessonContentManager::searchLessons(const std::string& query) const {
    std::vector<Lesson> result;
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    
    for (const auto& [id, lesson] : lessons_) {
        // البحث في العنوان العربي
        if (lesson.getTitleArabic().find(query) != std::string::npos) {
            result.push_back(lesson);
            continue;
        }
        
        // البحث في العنوان الإنجليزي
        std::string title_lower = lesson.getTitleEnglish();
        std::transform(title_lower.begin(), title_lower.end(), title_lower.begin(), ::tolower);
        if (title_lower.find(lower_query) != std::string::npos) {
            result.push_back(lesson);
            continue;
        }
        
        // البحث في الوسوم
        for (const auto& tag : lesson.getTags()) {
            if (tag.find(query) != std::string::npos) {
                result.push_back(lesson);
                break;
            }
        }
    }
    
    return result;
}

std::vector<Lesson> LessonContentManager::getAllLessons() const {
    std::vector<Lesson> result;
    for (const auto& [id, lesson] : lessons_) {
        result.push_back(lesson);
    }
    return result;
}

std::vector<Lesson> LessonContentManager::getBeginnerPath() const {
    // ترتيب الدروس للمبتدئين
    std::vector<std::string> path_order = {
        "type_basics",
        "functions_basics",
        "ownership_basics",
        "borrowing_rules",
        "pattern_matching_intro"
    };
    
    std::vector<Lesson> result;
    for (const auto& id : path_order) {
        auto lesson = getLesson(id);
        if (lesson) {
            result.push_back(*lesson);
        }
    }
    return result;
}

std::optional<Lesson> LessonContentManager::getLessonForError(const std::string& error_code) const {
    auto it = error_lesson_map_.find(error_code);
    if (it != error_lesson_map_.end()) {
        return getLesson(it->second);
    }
    return std::nullopt;
}

void LessonContentManager::mapErrorToLesson(const std::string& error_code, const std::string& lesson_id) {
    error_lesson_map_[error_code] = lesson_id;
}

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

std::string lessonTypeToArabic(LessonType type) {
    switch (type) {
        case LessonType::TEXT:      return "درس نصي";
        case LessonType::VIDEO:     return "درس فيديو";
        case LessonType::EXERCISE:  return "تمرين تفاعلي";
        case LessonType::QUIZ:      return "اختبار قصير";
        case LessonType::PROJECT:   return "مشروع عملي";
        case LessonType::MIXED:     return "درس مختلط";
        default:                    return "غير معروف";
    }
}

std::string lessonTypeToEnglish(LessonType type) {
    switch (type) {
        case LessonType::TEXT:      return "Text Lesson";
        case LessonType::VIDEO:     return "Video Lesson";
        case LessonType::EXERCISE:  return "Interactive Exercise";
        case LessonType::QUIZ:      return "Quiz";
        case LessonType::PROJECT:   return "Practical Project";
        case LessonType::MIXED:     return "Mixed Lesson";
        default:                    return "Unknown";
    }
}

std::string lessonLevelToArabic(LessonLevel level) {
    switch (level) {
        case LessonLevel::BEGINNER:     return "مبتدئ";
        case LessonLevel::INTERMEDIATE: return "متوسط";
        case LessonLevel::ADVANCED:     return "متقدم";
        case LessonLevel::EXPERT:       return "خبير";
        default:                        return "غير معروف";
    }
}

std::string lessonCategoryToArabic(LessonCategory category) {
    switch (category) {
        case LessonCategory::BASICS:           return "أساسيات اللغة";
        case LessonCategory::VARIABLES:        return "المتغيرات";
        case LessonCategory::CONTROL_FLOW:     return "التحكم في التدفق";
        case LessonCategory::FUNCTIONS:        return "الدوال";
        case LessonCategory::CLASSES:          return "الأصناف";
        case LessonCategory::OWNERSHIP:        return "نظام الملكية";
        case LessonCategory::BORROWING:        return "الاستعارة";
        case LessonCategory::LIFETIMES:        return "فترات الحياة";
        case LessonCategory::ERROR_HANDLING:   return "معالجة الأخطاء";
        case LessonCategory::PATTERN_MATCHING: return "مطابقة الأنماط";
        case LessonCategory::ASYNC:            return "البرمجة غير المتزامنة";
        case LessonCategory::GENERICS:         return "القوالب";
        case LessonCategory::TRAITS:           return "السمات";
        case LessonCategory::MODULES:          return "الوحدات";
        case LessonCategory::TESTING:          return "الاختبار";
        case LessonCategory::OPTIMIZATION:     return "التحسين";
        case LessonCategory::FFI:              return "التكامل مع C/C++";
        case LessonCategory::EMBEDDED:         return "الأنظمة المدمجة";
        case LessonCategory::WEB:              return "تطوير الويب";
        case LessonCategory::MOBILE:           return "تطوير الهاتف";
        default:                               return "غير معروف";
    }
}

} // namespace Teacher
} // namespace Sad
