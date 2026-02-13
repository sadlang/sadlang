// ======================================================================
// error_lessons.cpp - ربط الأخطاء بالدروس التعليمية
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   نظام ربط أكواد الأخطاء بالدروس التعليمية المناسبة
//   يُمكّن "أستاذ ص" من تقديم دروس مخصصة عند حدوث أخطاء
//   يشمل شروحات مفصلة ومقترحات للحلول
//
// English Description:
//   System for mapping error codes to appropriate educational lessons
//   Enables "أستاذ ص" to provide custom lessons when errors occur
//   Includes detailed explanations and solution suggestions
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include "error_manager.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>

namespace Sad {
namespace Errors {

// ======================================================================
// هيكل معلومات الدرس المرتبط بالخطأ
// Error Lesson Information Structure
// ======================================================================

/**
 * @struct ErrorLessonInfo
 * @brief (AR) معلومات الدرس المرتبط بخطأ معين
 * @brief (EN) Lesson information linked to a specific error
 */
struct ErrorLessonInfo {
    std::string error_code;         ///< رمز الخطأ (مثل ص0001)
    std::string lesson_id;          ///< معرف الدرس المرتبط
    std::string quick_explanation_ar; ///< شرح سريع بالعربية
    std::string quick_explanation_en; ///< شرح سريع بالإنجليزية
    std::string video_url;          ///< رابط فيديو توضيحي
    std::vector<std::string> solution_suggestions_ar; ///< مقترحات الحل بالعربية
    std::vector<std::string> solution_suggestions_en; ///< مقترحات الحل بالإنجليزية
    std::vector<std::string> related_lessons; ///< دروس ذات صلة
    std::string example_fix;        ///< مثال على الإصلاح
};

// ======================================================================
// قاعدة بيانات ربط الأخطاء بالدروس
// Error-to-Lesson Database
// ======================================================================

/**
 * @brief (AR) تهيئة قاعدة بيانات الأخطاء والدروس
 * @brief (EN) Initialize error-to-lesson database
 */
static std::unordered_map<std::string, ErrorLessonInfo> error_lesson_database;

/**
 * @brief (AR) تهيئة قاعدة البيانات بجميع الأخطاء والدروس
 * @brief (EN) Initialize database with all errors and lessons
 */
void initializeErrorLessonDatabase() {
    // ======================================================================
    // أخطاء نظام الملكية (Ownership Errors)
    // ======================================================================
    
    // ص0001: استخدام قيمة بعد نقلها
    {
        ErrorLessonInfo info;
        info.error_code = "ص0001";
        info.lesson_id = "ownership_basics";
        info.quick_explanation_ar = R"(
🔴 خطأ: استخدام قيمة بعد نقلها

عندما تُعيّن قيمة من متغير إلى آخر، تنتقل الملكية.
المتغير الأصلي لم يعد صالحاً للاستخدام.

مثال على المشكلة:
    نص س = "مرحبا"
    نص ص = س           // الملكية انتقلت إلى ص
    اطبع(س)            // ❌ خطأ! س لم يعد صالحاً
)";
        info.quick_explanation_en = R"(
🔴 Error: Use of moved value

When you assign a value from one variable to another, ownership moves.
The original variable is no longer valid.
)";
        info.video_url = "https://sad-lang.org/videos/ownership-basics";
        info.solution_suggestions_ar = {
            "استخدم .انسخ() لإنشاء نسخة بدلاً من نقل الملكية",
            "أعد ترتيب الكود ليستخدم المتغير قبل النقل",
            "استخدم الاستعارة (&) إذا كنت تحتاج القراءة فقط"
        };
        info.solution_suggestions_en = {
            "Use .clone() to create a copy instead of moving",
            "Reorder code to use variable before moving",
            "Use borrowing (&) if you only need to read"
        };
        info.related_lessons = {"borrowing_rules", "lifetimes_intro"};
        info.example_fix = R"(
// ❌ قبل الإصلاح:
نص س = "مرحبا"
نص ص = س
اطبع(س)    // خطأ!

// ✅ بعد الإصلاح (باستخدام النسخ):
نص س = "مرحبا"
نص ص = س.انسخ()
اطبع(س)    // يعمل!
اطبع(ص)    // يعمل!

// ✅ أو (باستخدام الاستعارة):
نص س = "مرحبا"
دع ص = &س
اطبع(س)    // يعمل!
اطبع(ص)    // يعمل!
)";
        error_lesson_database["ص0001"] = info;
    }
    
    // ص0002: استعارة متغيرة أثناء استعارة غير متغيرة
    {
        ErrorLessonInfo info;
        info.error_code = "ص0002";
        info.lesson_id = "borrowing_rules";
        info.quick_explanation_ar = R"(
🔴 خطأ: استعارة متغيرة أثناء استعارة غير متغيرة

لا يمكن أن تكون هناك استعارة متغيرة (&متغير) بينما توجد
استعارة غير متغيرة (&) نشطة.

القاعدة الذهبية:
• إما استعارة متغيرة واحدة
• أو أي عدد من الاستعارات غير المتغيرة
• لكن ليس كلاهما معاً!
)";
        info.quick_explanation_en = R"(
🔴 Error: Mutable borrow while immutable borrow exists

Cannot have a mutable borrow (&mut) while an immutable borrow (&) 
is still active.

The golden rule:
• Either one mutable borrow
• Or any number of immutable borrows
• But not both at the same time!
)";
        info.video_url = "https://sad-lang.org/videos/borrowing-rules";
        info.solution_suggestions_ar = {
            "تأكد من انتهاء الاستعارة غير المتغيرة قبل الاستعارة المتغيرة",
            "ضع الاستعارات في نطاقات منفصلة باستخدام { }",
            "أعد تصميم الكود لتجنب الحاجة لاستعارات متعددة"
        };
        info.solution_suggestions_en = {
            "Ensure immutable borrow ends before mutable borrow",
            "Put borrows in separate scopes using { }",
            "Redesign code to avoid needing multiple borrows"
        };
        info.related_lessons = {"ownership_basics", "lifetimes_intro"};
        info.example_fix = R"(
// ❌ قبل الإصلاح:
متغير نص س = "مرحبا"
دع مرجع1 = &س
دع مرجع2 = &متغير س    // خطأ!

// ✅ بعد الإصلاح (نطاقات منفصلة):
متغير نص س = "مرحبا"
{
    دع مرجع1 = &س
    اطبع(مرجع1)
}   // مرجع1 انتهى هنا
دع مرجع2 = &متغير س    // الآن يعمل!
)";
        error_lesson_database["ص0002"] = info;
    }
    
    // ص0003: قيمة لا تعيش طويلاً بما فيه الكفاية
    {
        ErrorLessonInfo info;
        info.error_code = "ص0003";
        info.lesson_id = "lifetimes_intro";
        info.quick_explanation_ar = R"(
🔴 خطأ: قيمة لا تعيش طويلاً بما فيه الكفاية

المرجع يشير إلى قيمة ستُحذف قبل انتهاء استخدام المرجع.
هذا يُسمى "مرجع معلق" (dangling reference) وهو خطير جداً.

المشكلة:
    القيمة تُحذف عند نهاية نطاقها
    لكن المرجع لا يزال موجوداً!
)";
        info.quick_explanation_en = R"(
🔴 Error: Value does not live long enough

A reference points to a value that will be dropped before the 
reference is done being used. This is called a "dangling reference".
)";
        info.video_url = "https://sad-lang.org/videos/lifetimes";
        info.solution_suggestions_ar = {
            "تأكد أن القيمة المُشار إليها تعيش طويلاً بما فيه الكفاية",
            "انقل القيمة إلى نطاق أعلى",
            "استخدم الملكية بدلاً من الاستعارة"
        };
        info.solution_suggestions_en = {
            "Ensure the referenced value lives long enough",
            "Move the value to a higher scope",
            "Use ownership instead of borrowing"
        };
        info.related_lessons = {"ownership_basics", "borrowing_rules"};
        info.example_fix = R"(
// ❌ قبل الإصلاح:
دالة خطأ() ترجع &نص
    نص محلي = "مرحبا"
    إرجاع &محلي    // خطأ! محلي سيُحذف
نهاية

// ✅ بعد الإصلاح (إرجاع القيمة):
دالة صحيح() ترجع نص
    نص محلي = "مرحبا"
    إرجاع محلي    // نقل الملكية
نهاية
)";
        error_lesson_database["ص0003"] = info;
    }
    
    // ======================================================================
    // أخطاء نظام الأنواع (Type Errors)
    // ======================================================================
    
    // ص1001: عدم تطابق الأنواع
    {
        ErrorLessonInfo info;
        info.error_code = "ص1001";
        info.lesson_id = "type_basics";
        info.quick_explanation_ar = R"(
🔴 خطأ: عدم تطابق الأنواع

حاولت استخدام قيمة من نوع ما في مكان يتوقع نوعاً آخر.

أمثلة شائعة:
• تمرير نص حيث يُتوقع رقم
• إسناد عشري إلى متغير رقم صحيح
• إرجاع نوع مختلف عن المُعلن
)";
        info.quick_explanation_en = R"(
🔴 Error: Type mismatch

Tried to use a value of one type where another type was expected.
)";
        info.video_url = "https://sad-lang.org/videos/type-system";
        info.solution_suggestions_ar = {
            "تحقق من أنواع المتغيرات والمعاملات",
            "استخدم دوال التحويل (مثل .إلى_نص()، .إلى_رقم())",
            "تأكد من أن نوع الإرجاع يتطابق مع التوقيع"
        };
        info.solution_suggestions_en = {
            "Check variable and parameter types",
            "Use conversion functions (e.g., .to_string(), .to_int())",
            "Ensure return type matches function signature"
        };
        info.related_lessons = {"functions_basics", "type_inference"};
        info.example_fix = R"(
// ❌ قبل الإصلاح:
دالة مثال(رقم س)
    نص ص = س    // خطأ! رقم ≠ نص
نهاية

// ✅ بعد الإصلاح:
دالة مثال(رقم س)
    نص ص = س.إلى_نص()    // تحويل صريح
نهاية
)";
        error_lesson_database["ص1001"] = info;
    }
    
    // ======================================================================
    // أخطاء مطابقة الأنماط (Pattern Matching Errors)
    // ======================================================================
    
    // ص3001: الأنماط غير شاملة
    {
        ErrorLessonInfo info;
        info.error_code = "ص3001";
        info.lesson_id = "pattern_matching_intro";
        info.quick_explanation_ar = R"(
🔴 خطأ: الأنماط غير شاملة

عبارة "طابق" يجب أن تغطي جميع الحالات الممكنة.
هناك قيم لم تُغطَّ بأي نمط.

الحل: أضف نمطاً افتراضياً باستخدام _ (wildcard)
)";
        info.quick_explanation_en = R"(
🔴 Error: Non-exhaustive patterns

A "match" statement must cover all possible values.
Some values are not covered by any pattern.

Solution: Add a default pattern using _ (wildcard)
)";
        info.video_url = "https://sad-lang.org/videos/pattern-matching";
        info.solution_suggestions_ar = {
            "أضف حالة _ (أي قيمة) في نهاية طابق",
            "تأكد من تغطية جميع حالات التعداد",
            "استخدم نطاقات للأرقام (مثل 0..10)"
        };
        info.solution_suggestions_en = {
            "Add a _ (wildcard) case at the end of match",
            "Ensure all enum variants are covered",
            "Use ranges for numbers (e.g., 0..10)"
        };
        info.related_lessons = {"pattern_matching_advanced"};
        info.example_fix = R"(
// ❌ قبل الإصلاح:
طابق رقم {
    حالة 1: اطبع("واحد")
    حالة 2: اطبع("اثنان")
    // ماذا عن 3، 4، 5، ...؟
}

// ✅ بعد الإصلاح:
طابق رقم {
    حالة 1: اطبع("واحد")
    حالة 2: اطبع("اثنان")
    حالة _: اطبع("رقم آخر")    // يغطي كل شيء آخر
}
)";
        error_lesson_database["ص3001"] = info;
    }
}

// ======================================================================
// واجهة الحصول على معلومات الدرس
// Interface for Getting Lesson Information
// ======================================================================

/**
 * @brief (AR) الحصول على معلومات الدرس المرتبط بخطأ
 * @brief (EN) Get lesson information for an error
 */
std::optional<ErrorLessonInfo> getErrorLessonInfo(const std::string& error_code) {
    // تهيئة القاعدة إذا لم تُهيأ
    static bool initialized = false;
    if (!initialized) {
        initializeErrorLessonDatabase();
        initialized = true;
    }
    
    auto it = error_lesson_database.find(error_code);
    if (it != error_lesson_database.end()) {
        return it->second;
    }
    return std::nullopt;
}

/**
 * @brief (AR) طباعة معلومات الخطأ والدرس بشكل مُنسق
 * @brief (EN) Print formatted error and lesson information
 */
void printErrorLessonInfo(const std::string& error_code, bool arabic) {
    auto info_opt = getErrorLessonInfo(error_code);
    
    if (!info_opt) {
        if (arabic) {
            std::cerr << "⚠️ لا توجد معلومات إضافية لهذا الخطأ\n";
        } else {
            std::cerr << "⚠️ No additional information for this error\n";
        }
        return;
    }
    
    const auto& info = *info_opt;
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║   🎓 أستاذ ص - تعلّم من هذا الخطأ!                        ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════╣\n";
    
    if (arabic) {
        std::cout << info.quick_explanation_ar << "\n";
        
        std::cout << "\n💡 مقترحات الحل:\n";
        int i = 1;
        for (const auto& suggestion : info.solution_suggestions_ar) {
            std::cout << "   " << i++ << ". " << suggestion << "\n";
        }
        
        std::cout << "\n📝 مثال على الإصلاح:\n";
        std::cout << info.example_fix << "\n";
        
        if (!info.video_url.empty()) {
            std::cout << "\n🎬 فيديو توضيحي: " << info.video_url << "\n";
        }
        
        std::cout << "\n📚 للتعلم أكثر، شغّل: ص تعلّم " << info.lesson_id << "\n";
    } else {
        std::cout << info.quick_explanation_en << "\n";
        
        std::cout << "\n💡 Solution suggestions:\n";
        int i = 1;
        for (const auto& suggestion : info.solution_suggestions_en) {
            std::cout << "   " << i++ << ". " << suggestion << "\n";
        }
        
        std::cout << "\n📝 Example fix:\n";
        std::cout << info.example_fix << "\n";
        
        if (!info.video_url.empty()) {
            std::cout << "\n🎬 Video tutorial: " << info.video_url << "\n";
        }
        
        std::cout << "\n📚 To learn more, run: sad learn " << info.lesson_id << "\n";
    }
    
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
}

/**
 * @brief (AR) هل يوجد درس مرتبط بهذا الخطأ؟
 * @brief (EN) Is there a lesson linked to this error?
 */
bool hasLessonForError(const std::string& error_code) {
    return getErrorLessonInfo(error_code).has_value();
}

/**
 * @brief (AR) الحصول على معرف الدرس المرتبط بالخطأ
 * @brief (EN) Get lesson ID linked to error
 */
std::string getLessonIdForError(const std::string& error_code) {
    auto info_opt = getErrorLessonInfo(error_code);
    if (info_opt) {
        return info_opt->lesson_id;
    }
    return "";
}

} // namespace Errors
} // namespace Sad
