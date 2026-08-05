// بسم الله الرحمن الرحيم
// ======================================================================
// ui_tests_shared.h — الثوابتُ المشترَكةُ بين اختباراتِ رسمِ الواجهة
// ======================================================================
// (AR) لماذا رأسٌ مشترَكٌ لا نسخةٌ في كلِّ اختبار: هذه سلاسلُ **بايتاتٍ**
//      يقرؤها `renderNode` من العقدةِ مباشرةً، فنسخُها في ملفَّين يعني أنّ
//      إعادةَ تسميةِ مفتاحٍ في المحرّكِ تحتاجُ تعديلَين، وأنّ نسيانَ أحدِهما
//      **تباعُدٌ صامت**: الاختبارُ المنسيُّ يُسند مفتاحًا لا يقرؤه أحدٌ فيمرُّ
//      أخضرَ على عقدةٍ فارغة.
// (EN) Shared because these are raw byte keys read by renderNode: duplicating
//      them means a rename needs two edits, and forgetting one leaves a test
//      silently green while setting a property nobody reads.
// ======================================================================

#ifndef SAD_TESTS_UI_TESTS_SHARED_H
#define SAD_TESTS_UI_TESTS_SHARED_H

#include "sad_ui/types.h"

#include <cstdlib>
#include <string>

// ── (أ) مفاتيح مُشتقّة من مصدر الحقيقة (أسماء معدّلات SoT) ──────────────
// تُشتقّ من modifierTypeToArabicName (types.cpp) بدل كتابتها يدويًّا — فأيّ
// تباعد بين الاختبار وSoT يُكشَف بالبناء، ولا نكرّر السلسلة العربيّة.
static const std::string K_COLOR = sad::ui::modifierTypeToArabicName(sad::ui::ModifierType::ForegroundColor); // لون
static const std::string K_ALIGN = sad::ui::modifierTypeToArabicName(sad::ui::ModifierType::Alignment);       // محاذاة
static const std::string K_GRAD = sad::ui::modifierTypeToArabicName(sad::ui::ModifierType::GradientColor);    // تدرج

// ── (ب) مفاتيح خصائص IR خاصّة بالمحرّك (ليست أسماء معدّلات) ─────────────
// هذه مفاتيح يقرؤها renderNode مباشرةً من IRNode ولا تقابل معدّلًا في SoT
// (أو تخالف اسمه)، فتبقى ثوابت hex موثَّقة بموضع قراءتها في platform_renderer.cpp:
static const std::string K_TITLE = "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86";                 // عنوان — نصّ الزرّ (فرع Button)
static const std::string K_CONTENT = "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89";               // محتوى — نصّ العنصر النصّيّ (فرع Text)
static const std::string K_TEXT = "\xd9\x86\xd8\xb5";                                          // نص — مفتاح نصّ احتياطيّ عامّ
static const std::string K_BG = "\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9";                     // خلفية — مفتاح IR (المعدّل الرسميّ «لون_خلفية»)
static const std::string K_WIDTH = "\xd8\xb9\xd8\xb1\xd8\xb6";                                  // عرض — عرض ثابت للعنصر
static const std::string K_FONTSZ = "\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7"; // حجم_الخط — يقرؤه renderNode (المعدّل «حجم_خط»)
static const std::string K_GRAD_END = "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9"; // تدرج_نهاية — لون نهاية التدرّج (فرع Button)
static const std::string K_VALUE = "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9";                          // قيمة — قيمة حقل النصّ (فرع TextField)
static const std::string K_OPACITY = "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9";         // شفافية — مفتاح IR (المعدّل «عتامة»)
static const std::string K_SRC = "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1";                            // مصدر — مسار الصورة (فرع Image)

// ── قيَم نصّيّة للاختبار ───────────────────────────────────────────────
static const std::string V_HELLO_AR = "\xd9\x85\xd8\xb1\xd8\xad\xd8\xa8\xd8\xa7";               // مرحبا

// ── متغيّرا بيئةِ وضعِ تحديثِ الذهبيّات ────────────────────────────────
// (AR) مشترَكانِ لا مكرَّرانِ: الشرطُ `التحديثُ && !CI` هو البابُ الخلفيُّ
//      الوحيدُ للبوّابتَينِ معًا، وكتابتُه حرفيًّا في كلِّ موضعٍ تعني أنّ
//      تصحيحَ اسمٍ في موضعٍ ونسيانَه في آخرَ يتركُ البابَ مفتوحًا هناك بلا
//      أيِّ إشارةٍ من المصرّف.
// (EN) Shared, not duplicated: `dump && !CI` is the only back door into both
//      gates; spelling it literally at each site means fixing one and missing
//      another leaves that door open with no compiler signal.
static const char *const kDumpGoldenEnv = "SAD_UI_DUMP_GOLDEN";
static const char *const kContinuousIntegrationEnv = "CI";

inline bool goldenDumpRequested() { return std::getenv(kDumpGoldenEnv) != nullptr; }
inline bool insideContinuousIntegration() { return std::getenv(kContinuousIntegrationEnv) != nullptr; }

#endif // SAD_TESTS_UI_TESTS_SHARED_H
