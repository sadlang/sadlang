// ======================================================================
// tutorial_engine.cpp - تنفيذ محرك الدروس التفاعلية
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   تنفيذ كامل لمحرك الدروس التفاعلية
//   يشمل تشغيل الدروس، التمارين، والتكامل مع الأخطاء
//
// English Description:
//   Complete implementation of interactive tutorial engine
//   Includes lesson running, exercises, and error integration
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include "tutorial_engine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Sad {
namespace Teacher {

// ======================================================================
// تنفيذ ExerciseEngine / Exercise Engine Implementation
// ======================================================================

ExerciseResult ExerciseEngine::executeCode(const std::string& code,
                                            const std::string& expected_output) {
    ExerciseResult result;
    result.expected_output = expected_output;
    
    // التحقق من صحة الكود أولاً
    std::string validation_error;
    if (!validateCode(code, validation_error)) {
        result.success = false;
        result.feedback_ar = "❌ خطأ في الكود: " + validation_error;
        result.feedback_en = "❌ Code error: " + validation_error;
        result.actual_output = "";
        return result;
    }
    
    // تنفيذ الكود
    bool run_success;
    result.actual_output = runCode(code, run_success);
    
    if (!run_success) {
        result.success = false;
        result.feedback_ar = "❌ فشل تنفيذ الكود";
        result.feedback_en = "❌ Code execution failed";
        return result;
    }
    
    // مقارنة المخرجات
    result.success = compareOutput(result.actual_output, expected_output);
    
    if (result.success) {
        result.feedback_ar = "✅ أحسنت! الحل صحيح!";
        result.feedback_en = "✅ Great job! Correct solution!";
    } else {
        result.feedback_ar = "❌ المخرجات غير متطابقة. حاول مرة أخرى.";
        result.feedback_en = "❌ Output mismatch. Try again.";
    }
    
    return result;
}

bool ExerciseEngine::validateCode(const std::string& code, std::string& error_message) {
    // كتابة الكود في ملف مؤقت
    std::string temp_file = "temp_exercise.s";
    {
        std::ofstream file(temp_file);
        if (!file.is_open()) {
            error_message = "لا يمكن إنشاء ملف مؤقت";
            return false;
        }
        file << code;
    }
    
    // استدعاء المترجم للتحقق فقط
    std::string cmd = compiler_path_ + " --check " + temp_file + " 2>&1";
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        error_message = "فشل تشغيل المترجم";
        std::filesystem::remove(temp_file);
        return false;
    }
    
    char buffer[128];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
    int exit_code = pclose(pipe);
    std::filesystem::remove(temp_file);
    
    if (exit_code != 0) {
        error_message = output;
        return false;
    }
    
    return true;
}

std::string ExerciseEngine::runCode(const std::string& code, bool& success) {
    // كتابة الكود في ملف مؤقت
    std::string temp_file = "temp_exercise.s";
    {
        std::ofstream file(temp_file);
        if (!file.is_open()) {
            success = false;
            return "لا يمكن إنشاء ملف مؤقت";
        }
        file << code;
    }
    
    // تنفيذ الكود
    std::string cmd = compiler_path_ + " " + temp_file + " 2>&1";
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        success = false;
        std::filesystem::remove(temp_file);
        return "فشل تشغيل المترجم";
    }
    
    char buffer[128];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    
    int exit_code = pclose(pipe);
    std::filesystem::remove(temp_file);
    
    success = (exit_code == 0);
    return output;
}

bool ExerciseEngine::compareOutput(const std::string& actual, const std::string& expected) {
    // تنظيف المخرجات من المسافات الزائدة
    auto trim = [](const std::string& str) {
        auto start = str.find_first_not_of(" \t\n\r");
        auto end = str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos) return std::string();
        return str.substr(start, end - start + 1);
    };
    
    return trim(actual) == trim(expected);
}

// ======================================================================
// تنفيذ TutorialEngine / Tutorial Engine Implementation
// ======================================================================

TutorialEngine::TutorialEngine(std::shared_ptr<ITutorialUI> ui)
    : ui_(ui)
{
    // تهيئة محرك التمارين
}

TutorialSession TutorialEngine::startSession(const std::string& lesson_id,
                                              const std::string& user_id) {
    initializeSession(lesson_id, user_id);
    return current_session_;
}

void TutorialEngine::initializeSession(const std::string& lesson_id,
                                        const std::string& user_id) {
    current_session_.session_id = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    current_session_.lesson_id = lesson_id;
    current_session_.user_id = user_id;
    current_session_.start_time = std::chrono::system_clock::now();
    current_session_.current_section = 0;
    current_session_.current_exercise = 0;
    current_session_.total_attempts = 0;
    current_session_.successful_attempts = 0;
    current_session_.state = ExerciseState::NOT_STARTED;
    current_session_.results.clear();
    current_hint_index_ = 0;
}

void TutorialEngine::endSession() {
    current_session_.end_time = std::chrono::system_clock::now();
    
    // حساب الإحصائيات
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(
        current_session_.end_time - current_session_.start_time
    ).count();
    
    // عرض ملخص الجلسة
    std::stringstream summary;
    summary << "\n";
    summary << "╔════════════════════════════════════════╗\n";
    summary << "║       📊 ملخص الجلسة التعليمية        ║\n";
    summary << "╠════════════════════════════════════════╣\n";
    summary << "║ المدة: " << duration << " دقيقة\n";
    summary << "║ المحاولات: " << current_session_.total_attempts << "\n";
    summary << "║ النجاح: " << current_session_.successful_attempts << "\n";
    summary << "╚════════════════════════════════════════╝\n";
    
    std::cout << summary.str();
}

bool TutorialEngine::runLesson(const std::string& lesson_id) {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLesson(lesson_id);
    
    if (!lesson_opt) {
        std::cerr << "❌ الدرس غير موجود: " << lesson_id << std::endl;
        return false;
    }
    
    const Lesson& lesson = *lesson_opt;
    current_session_.state = ExerciseState::IN_PROGRESS;
    
    // عرض الدرس
    ui_->displayLesson(lesson);
    
    // تشغيل كل قسم
    const auto& sections = lesson.getSections();
    for (size_t i = 0; i < sections.size(); ++i) {
        current_session_.current_section = static_cast<int>(i);
        
        ui_->displayProgress(static_cast<int>(i + 1), 
                            static_cast<int>(sections.size()),
                            sections[i].title_ar);
        
        ui_->displaySection(sections[i]);
        
        // إذا كان القسم تفاعلياً، انتظر تأكيد المستخدم
        if (sections[i].is_interactive) {
            if (!ui_->askYesNo("هل أنت جاهز للمتابعة؟")) {
                break;
            }
        }
    }
    
    // تشغيل التمارين
    const auto& exercises = lesson.getExercises();
    for (size_t i = 0; i < exercises.size(); ++i) {
        current_session_.current_exercise = static_cast<int>(i);
        
        ui_->displayProgress(static_cast<int>(i + 1),
                            static_cast<int>(exercises.size()),
                            "تمرين: " + exercises[i].title_ar);
        
        ExerciseResult result = runExercise(exercises[i]);
        saveExerciseResult(result);
        
        if (!result.success) {
            if (!ui_->askYesNo("هل تريد إعادة المحاولة؟")) {
                skipExercise();
            }
        }
    }
    
    current_session_.state = ExerciseState::COMPLETED;
    return true;
}

bool TutorialEngine::runSection(int section_index) {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLesson(current_session_.lesson_id);
    
    if (!lesson_opt) return false;
    
    const auto& sections = lesson_opt->getSections();
    if (section_index < 0 || section_index >= static_cast<int>(sections.size())) {
        return false;
    }
    
    ui_->displaySection(sections[section_index]);
    current_session_.current_section = section_index;
    
    return true;
}

ExerciseResult TutorialEngine::runExercise(const ExerciseInfo& exercise) {
    ui_->displayExercise(exercise);
    
    current_hint_index_ = 0;
    int attempts = 0;
    const int max_attempts = exercise.max_attempts > 0 ? exercise.max_attempts : 3;
    
    ExerciseResult final_result;
    final_result.attempts_remaining = max_attempts;
    
    while (attempts < max_attempts) {
        // الحصول على كود المستخدم
        std::string user_code = ui_->getUserCode();
        
        if (user_code.empty() || user_code == "تخطي" || user_code == "skip") {
            final_result.success = false;
            final_result.feedback_ar = "تم تخطي التمرين";
            final_result.feedback_en = "Exercise skipped";
            break;
        }
        
        if (user_code == "تلميح" || user_code == "hint") {
            showNextHint();
            continue;
        }
        
        if (user_code == "حل" || user_code == "solution") {
            showSolution();
            final_result.success = false;
            final_result.feedback_ar = "تم عرض الحل - التمرين غير مكتمل";
            final_result.feedback_en = "Solution shown - Exercise incomplete";
            break;
        }
        
        // تنفيذ وتقييم الكود
        attempts++;
        current_session_.total_attempts++;
        
        final_result = exercise_engine_.executeCode(user_code, exercise.expected_output);
        final_result.attempts_remaining = max_attempts - attempts;
        
        ui_->displayResult(final_result);
        
        if (final_result.success) {
            current_session_.successful_attempts++;
            break;
        }
        
        // عرض تلميح تلقائي بعد محاولتين فاشلتين
        if (auto_hints_ && attempts >= 2 && current_hint_index_ < static_cast<int>(exercise.hints.size())) {
            ui_->displayHint(exercise.hints[current_hint_index_], current_hint_index_ + 1);
            current_hint_index_++;
        }
    }
    
    return final_result;
}

bool TutorialEngine::nextSection() {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLesson(current_session_.lesson_id);
    
    if (!lesson_opt) return false;
    
    const auto& sections = lesson_opt->getSections();
    if (current_session_.current_section + 1 < static_cast<int>(sections.size())) {
        current_session_.current_section++;
        return runSection(current_session_.current_section);
    }
    
    return false;
}

bool TutorialEngine::previousSection() {
    if (current_session_.current_section > 0) {
        current_session_.current_section--;
        return runSection(current_session_.current_section);
    }
    return false;
}

void TutorialEngine::skipExercise() {
    ExerciseResult result;
    result.success = false;
    result.feedback_ar = "تم تخطي التمرين";
    result.feedback_en = "Exercise skipped";
    
    saveExerciseResult(result);
    current_session_.current_exercise++;
}

void TutorialEngine::showNextHint() {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLesson(current_session_.lesson_id);
    
    if (!lesson_opt) return;
    
    const auto& exercises = lesson_opt->getExercises();
    if (current_session_.current_exercise >= static_cast<int>(exercises.size())) return;
    
    const auto& exercise = exercises[current_session_.current_exercise];
    if (current_hint_index_ < static_cast<int>(exercise.hints.size())) {
        ui_->displayHint(exercise.hints[current_hint_index_], current_hint_index_ + 1);
        current_hint_index_++;
    } else {
        std::cout << "💡 لا توجد تلميحات إضافية" << std::endl;
    }
}

void TutorialEngine::showSolution() {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLesson(current_session_.lesson_id);
    
    if (!lesson_opt) return;
    
    const auto& exercises = lesson_opt->getExercises();
    if (current_session_.current_exercise >= static_cast<int>(exercises.size())) return;
    
    const auto& exercise = exercises[current_session_.current_exercise];
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║           📝 الحل النموذجي             ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << exercise.solution << "\n";
    std::cout << "╚════════════════════════════════════════╝\n";
}

bool TutorialEngine::runLessonForError(const std::string& error_code) {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLessonForError(error_code);
    
    if (!lesson_opt) {
        std::cerr << "⚠️ لا يوجد درس مرتبط بهذا الخطأ: " << error_code << std::endl;
        return false;
    }
    
    // عرض رسالة ترحيبية
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║   🎓 أستاذ ص - تعلّم من أخطائك!                   ║\n";
    std::cout << "╠══════════════════════════════════════════════════╣\n";
    std::cout << "║ رمز الخطأ: " << error_code << "\n";
    std::cout << "║ الدرس المقترح: " << lesson_opt->getTitleArabic() << "\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n";
    
    if (ui_->askYesNo("هل تريد بدء الدرس الآن؟")) {
        return runLesson(lesson_opt->getId());
    }
    
    return false;
}

std::string TutorialEngine::getErrorExplanation(const std::string& error_code) const {
    auto& content_manager = LessonContentManager::getInstance();
    auto lesson_opt = content_manager.getLessonForError(error_code);
    
    if (!lesson_opt) {
        return "لا يوجد شرح متاح لهذا الخطأ";
    }
    
    // إعادة الوصف العربي للدرس كشرح سريع
    return lesson_opt->getDescriptionArabic();
}

void TutorialEngine::saveExerciseResult(const ExerciseResult& result) {
    current_session_.results.push_back(result);
}

// ======================================================================
// تنفيذ CLITutorialUI / CLI Tutorial UI Implementation
// ======================================================================

void CLITutorialUI::displayLesson(const Lesson& lesson) {
    printSeparator('═', 60);
    std::cout << "\n";
    std::cout << "  📚 " << lesson.getTitleArabic() << "\n";
    std::cout << "     " << lesson.getTitleEnglish() << "\n";
    std::cout << "\n";
    printSeparator('─', 60);
    std::cout << "\n";
    std::cout << "  📝 " << lesson.getDescriptionArabic() << "\n";
    std::cout << "\n";
    std::cout << "  ⏱️  الوقت المقدر: " << lesson.getEstimatedMinutes() << " دقيقة\n";
    std::cout << "  📊 المستوى: " << lessonLevelToArabic(lesson.getLevel()) << "\n";
    std::cout << "  📁 الفئة: " << lessonCategoryToArabic(lesson.getCategory()) << "\n";
    std::cout << "\n";
    printSeparator('═', 60);
}

void CLITutorialUI::displaySection(const LessonSection& section) {
    std::cout << "\n";
    printHeader(section.title_ar);
    std::cout << "\n";
    std::cout << section.content_ar << "\n";
    
    if (!section.code_example.empty()) {
        std::cout << "\n";
        std::cout << "📝 مثال:\n";
        printCode(section.code_example);
    }
    std::cout << "\n";
}

void CLITutorialUI::displayExercise(const ExerciseInfo& exercise) {
    printSeparator('═', 60);
    std::cout << "\n";
    std::cout << "  🏋️ تمرين: " << exercise.title_ar << "\n";
    std::cout << "\n";
    printSeparator('─', 60);
    std::cout << "\n";
    std::cout << "  " << exercise.description_ar << "\n";
    std::cout << "\n";
    
    if (!exercise.starter_code.empty()) {
        std::cout << "📝 الكود الابتدائي:\n";
        printCode(exercise.starter_code);
    }
    
    std::cout << "\n";
    std::cout << "💡 أوامر مساعدة:\n";
    std::cout << "   • 'تلميح' أو 'hint' - للحصول على تلميح\n";
    std::cout << "   • 'حل' أو 'solution' - لرؤية الحل\n";
    std::cout << "   • 'تخطي' أو 'skip' - لتخطي التمرين\n";
    std::cout << "\n";
    printSeparator('═', 60);
}

void CLITutorialUI::displayResult(const ExerciseResult& result) {
    std::cout << "\n";
    
    if (result.success) {
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║         ✅ " << result.feedback_ar << "           ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
    } else {
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║         " << result.feedback_ar << "\n";
        std::cout << "║ المحاولات المتبقية: " << result.attempts_remaining << "\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        
        if (!result.actual_output.empty()) {
            std::cout << "\n📤 المخرجات الفعلية:\n";
            std::cout << result.actual_output << "\n";
        }
        
        if (!result.expected_output.empty()) {
            std::cout << "\n📥 المخرجات المتوقعة:\n";
            std::cout << result.expected_output << "\n";
        }
    }
    
    std::cout << "\n";
}

void CLITutorialUI::displayHint(const std::string& hint, int hint_number) {
    std::cout << "\n";
    std::cout << "💡 تلميح #" << hint_number << ": " << hint << "\n";
    std::cout << "\n";
}

std::string CLITutorialUI::getUserCode() {
    std::cout << "\n";
    std::cout << "📝 اكتب الكود (أدخل سطراً فارغاً للإنهاء):\n";
    std::cout << "───────────────────────────────────────\n";
    
    std::string code;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        code += line + "\n";
    }
    
    return code;
}

void CLITutorialUI::displayProgress(int current, int total, const std::string& message) {
    std::cout << "\n";
    std::cout << "📊 التقدم: [" << current << "/" << total << "] " << message << "\n";
    
    // شريط التقدم
    int bar_width = 40;
    float progress = static_cast<float>(current) / static_cast<float>(total);
    int filled = static_cast<int>(progress * bar_width);
    
    std::cout << "   [";
    for (int i = 0; i < bar_width; ++i) {
        if (i < filled) std::cout << "█";
        else std::cout << "░";
    }
    std::cout << "] " << static_cast<int>(progress * 100) << "%\n";
}

bool CLITutorialUI::askYesNo(const std::string& question) {
    std::cout << "\n" << question << " (نعم/لا): ";
    std::string answer;
    std::getline(std::cin, answer);
    
    return answer == "نعم" || answer == "yes" || answer == "y" || answer == "ن";
}

void CLITutorialUI::printSeparator(char c, int length) {
    for (int i = 0; i < length; ++i) {
        std::cout << c;
    }
    std::cout << "\n";
}

void CLITutorialUI::printHeader(const std::string& title) {
    std::cout << "┌";
    for (size_t i = 0; i < title.length() + 4; ++i) std::cout << "─";
    std::cout << "┐\n";
    
    std::cout << "│  " << title << "  │\n";
    
    std::cout << "└";
    for (size_t i = 0; i < title.length() + 4; ++i) std::cout << "─";
    std::cout << "┘\n";
}

void CLITutorialUI::printCode(const std::string& code) {
    std::cout << "┌────────────────────────────────────────┐\n";
    
    std::istringstream stream(code);
    std::string line;
    int line_num = 1;
    
    while (std::getline(stream, line)) {
        std::cout << "│ " << std::setw(3) << line_num << " │ " << line << "\n";
        line_num++;
    }
    
    std::cout << "└────────────────────────────────────────┘\n";
}

} // namespace Teacher
} // namespace Sad
