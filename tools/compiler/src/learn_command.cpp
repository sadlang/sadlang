// ======================================================================
// learn_command.cpp - أمر التعلم في CLI / Learn Command Implementation
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   تنفيذ أمر "ص تعلّم" الذي يتيح للمستخدم تشغيل دروس تفاعلية
//   من سطر الأوامر. يدعم تحديد درس معين أو البدء من المسار التعليمي.
//
// English Description:
//   Implementation of "sad learn" command that allows users to run
//   interactive lessons from the command line. Supports specifying
//   a specific lesson or starting from the learning path.
//
// الاستخدام / Usage:
//   ص تعلّم                     # عرض قائمة الدروس المتاحة
//   ص تعلّم <معرف_الدرس>        # تشغيل درس معين
//   ص تعلّم --مبتدئ             # مسار المبتدئين
//   ص تعلّم --ملكية             # دروس الملكية
//   ص تعلّم --خطأ ص0001         # درس مرتبط بخطأ
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../teacher/include/lesson_content.h"
#include "../../teacher/include/tutorial_engine.h"

namespace Sad {
namespace CLI {

// ======================================================================
// ثوابت الأمر / Command Constants
// ======================================================================

const std::string COMMAND_NAME = "تعلّم";
const std::string COMMAND_NAME_EN = "learn";
const std::string VERSION = "1.0.0";

// ======================================================================
// دالة المساعدة / Help Function
// ======================================================================

/**
 * @brief (AR) طباعة رسالة المساعدة
 * @brief (EN) Print help message
 */
void printHelp() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════╗
║           🎓 أستاذ ص - نظام التعلم التفاعلي                   ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║   الاستخدام:                                                 ║
║       ص تعلّم                        عرض قائمة الدروس        ║
║       ص تعلّم <معرف_الدرس>           تشغيل درس معين          ║
║       ص تعلّم --مبتدئ                مسار المبتدئين          ║
║       ص تعلّم --ملكية                دروس نظام الملكية       ║
║       ص تعلّم --خطأ <رمز>            درس مرتبط بخطأ          ║
║       ص تعلّم --بحث <كلمة>           البحث في الدروس         ║
║                                                              ║
║   الخيارات:                                                  ║
║       --مساعدة, -م                   عرض هذه الرسالة         ║
║       --نسخة, -ن                     عرض رقم النسخة          ║
║       --إنجليزي, -en                 عرض بالإنجليزية         ║
║       --مفصل, -v                     وضع التفاصيل            ║
║                                                              ║
║   أمثلة:                                                     ║
║       ص تعلّم ownership_basics       تعلم أساسيات الملكية    ║
║       ص تعلّم --خطأ ص0001            تعلم من خطأ الملكية     ║
║       ص تعلّم --بحث استعارة           البحث عن دروس الاستعارة ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝
)";
}

/**
 * @brief (AR) طباعة قائمة الدروس المتاحة
 * @brief (EN) Print list of available lessons
 */
void printLessonList() {
    auto& manager = Teacher::LessonContentManager::getInstance();
    manager.loadEmbeddedLessons();
    
    auto lessons = manager.getAllLessons();
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              📚 الدروس المتاحة في أستاذ ص                    ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    // تجميع الدروس حسب الفئة
    std::cout << "║                                                              ║\n";
    std::cout << "║  🔷 أساسيات اللغة:                                           ║\n";
    for (const auto& lesson : lessons) {
        if (lesson.getCategory() == Teacher::LessonCategory::BASICS ||
            lesson.getCategory() == Teacher::LessonCategory::VARIABLES ||
            lesson.getCategory() == Teacher::LessonCategory::FUNCTIONS) {
            std::cout << "║     • " << lesson.getId() << " - " 
                      << lesson.getTitleArabic() << "\n";
        }
    }
    
    std::cout << "║                                                              ║\n";
    std::cout << "║  🔶 نظام الملكية والاستعارة:                                  ║\n";
    for (const auto& lesson : lessons) {
        if (lesson.getCategory() == Teacher::LessonCategory::OWNERSHIP ||
            lesson.getCategory() == Teacher::LessonCategory::BORROWING ||
            lesson.getCategory() == Teacher::LessonCategory::LIFETIMES) {
            std::cout << "║     • " << lesson.getId() << " - " 
                      << lesson.getTitleArabic() << "\n";
        }
    }
    
    std::cout << "║                                                              ║\n";
    std::cout << "║  🔹 مواضيع متقدمة:                                           ║\n";
    for (const auto& lesson : lessons) {
        if (lesson.getCategory() == Teacher::LessonCategory::PATTERN_MATCHING ||
            lesson.getCategory() == Teacher::LessonCategory::ASYNC ||
            lesson.getCategory() == Teacher::LessonCategory::GENERICS) {
            std::cout << "║     • " << lesson.getId() << " - " 
                      << lesson.getTitleArabic() << "\n";
        }
    }
    
    std::cout << "║                                                              ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  💡 لتشغيل درس: ص تعلّم <معرف_الدرس>                         ║\n";
    std::cout << "║  💡 مثال: ص تعلّم ownership_basics                           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

/**
 * @brief (AR) طباعة مسار المبتدئين
 * @brief (EN) Print beginner path
 */
void printBeginnerPath() {
    auto& manager = Teacher::LessonContentManager::getInstance();
    manager.loadEmbeddedLessons();
    
    auto path = manager.getBeginnerPath();
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              🌟 مسار المبتدئين في لغة ص                       ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║  ابدأ رحلتك في تعلم لغة ص من هنا!                            ║\n";
    std::cout << "║  اتبع هذه الدروس بالترتيب للحصول على أفضل تجربة.             ║\n";
    std::cout << "║                                                              ║\n";
    
    int step = 1;
    for (const auto& lesson : path) {
        std::cout << "║  " << step++ << ". " << lesson.getTitleArabic() << "\n";
        std::cout << "║     📍 " << lesson.getId() << " | ⏱️ " 
                  << lesson.getEstimatedMinutes() << " دقيقة\n";
        std::cout << "║                                                              ║\n";
    }
    
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  💡 ابدأ بأول درس: ص تعلّم " << path[0].getId() << "\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

/**
 * @brief (AR) تشغيل درس معين
 * @brief (EN) Run a specific lesson
 */
int runLesson(const std::string& lesson_id, bool verbose) {
    auto& manager = Teacher::LessonContentManager::getInstance();
    manager.loadEmbeddedLessons();
    
    // إنشاء واجهة CLI
    auto ui = std::make_shared<Teacher::CLITutorialUI>();
    Teacher::TutorialEngine engine(ui);
    
    if (verbose) {
        engine.setVerboseMode(true);
    }
    
    // بدء الجلسة
    engine.startSession(lesson_id);
    
    // تشغيل الدرس
    bool success = engine.runLesson(lesson_id);
    
    // إنهاء الجلسة
    engine.endSession();
    
    return success ? 0 : 1;
}

/**
 * @brief (AR) تشغيل درس مرتبط بخطأ
 * @brief (EN) Run lesson for an error
 */
int runLessonForError(const std::string& error_code) {
    auto& manager = Teacher::LessonContentManager::getInstance();
    manager.loadEmbeddedLessons();
    
    auto ui = std::make_shared<Teacher::CLITutorialUI>();
    Teacher::TutorialEngine engine(ui);
    
    return engine.runLessonForError(error_code) ? 0 : 1;
}

/**
 * @brief (AR) البحث في الدروس
 * @brief (EN) Search lessons
 */
void searchLessons(const std::string& query) {
    auto& manager = Teacher::LessonContentManager::getInstance();
    manager.loadEmbeddedLessons();
    
    auto results = manager.searchLessons(query);
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              🔍 نتائج البحث عن: " << query << "\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    if (results.empty()) {
        std::cout << "║  ❌ لم يتم العثور على دروس مطابقة                            ║\n";
    } else {
        std::cout << "║  ✅ تم العثور على " << results.size() << " درس:\n";
        std::cout << "║                                                              ║\n";
        
        for (const auto& lesson : results) {
            std::cout << "║  • " << lesson.getId() << "\n";
            std::cout << "║    " << lesson.getTitleArabic() << "\n";
            std::cout << "║    ⏱️ " << lesson.getEstimatedMinutes() << " دقيقة | "
                      << "📊 " << Teacher::lessonLevelToArabic(lesson.getLevel()) << "\n";
            std::cout << "║                                                              ║\n";
        }
    }
    
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

// ======================================================================
// الدالة الرئيسية للأمر / Main Command Function
// ======================================================================

/**
 * @brief (AR) نقطة الدخول لأمر التعلم
 * @brief (EN) Entry point for learn command
 * 
 * @param argc (int) — عدد المعاملات
 * @param argv (char**) — المعاملات
 * @return (int) — رمز الخروج (0 = نجاح)
 */
int learnCommand(int argc, char* argv[]) {
    // تحليل المعاملات
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // بدون معاملات - عرض قائمة الدروس
    if (args.empty()) {
        printLessonList();
        return 0;
    }
    
    // تحليل الخيارات
    bool verbose = false;
    bool english = false;
    std::string lesson_id;
    std::string error_code;
    std::string search_query;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--مساعدة" || arg == "-م" || arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
        else if (arg == "--نسخة" || arg == "-ن" || arg == "--version") {
            std::cout << "أستاذ ص نسخة " << VERSION << "\n";
            return 0;
        }
        else if (arg == "--مفصل" || arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
        else if (arg == "--إنجليزي" || arg == "-en" || arg == "--english") {
            english = true;
        }
        else if (arg == "--مبتدئ" || arg == "--beginner") {
            printBeginnerPath();
            return 0;
        }
        else if (arg == "--ملكية" || arg == "--ownership") {
            lesson_id = "ownership_basics";
        }
        else if (arg == "--خطأ" || arg == "--error") {
            if (i + 1 < args.size()) {
                error_code = args[++i];
            } else {
                std::cerr << "❌ يجب تحديد رمز الخطأ بعد --خطأ\n";
                return 1;
            }
        }
        else if (arg == "--بحث" || arg == "--search") {
            if (i + 1 < args.size()) {
                search_query = args[++i];
            } else {
                std::cerr << "❌ يجب تحديد كلمة البحث بعد --بحث\n";
                return 1;
            }
        }
        else if (arg[0] != '-') {
            lesson_id = arg;
        }
    }
    
    // تنفيذ الأمر المناسب
    if (!search_query.empty()) {
        searchLessons(search_query);
        return 0;
    }
    
    if (!error_code.empty()) {
        return runLessonForError(error_code);
    }
    
    if (!lesson_id.empty()) {
        return runLesson(lesson_id, verbose);
    }
    
    // لم يتم تحديد أي خيار صالح
    printHelp();
    return 0;
}

} // namespace CLI
} // namespace Sad

// ======================================================================
// نقطة الدخول (إذا تم ترجمته كبرنامج مستقل)
// Entry Point (if compiled as standalone program)
// ======================================================================

#ifdef STANDALONE_LEARN_COMMAND
int main(int argc, char* argv[]) {
    return Sad::CLI::learnCommand(argc, argv);
}
#endif
