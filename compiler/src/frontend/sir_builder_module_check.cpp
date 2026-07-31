// ============================================================================
// sir_builder_module_check.cpp - فحص استيراد الوحدات القياسية
// ============================================================================
// (AR) هذا الملف ينفّذ منطق فحص استيراد الوحدات القياسية في المترجم.
//      الهدف: توحيد سلوك المترجم مع المفسر — الدوال المضمنة التي تنتمي
//      لوحدة قياسية (مثل رياضيات، نصوص، أساسيات) لا تعمل إلا بعد استيراد
//      الوحدة المناسبة، تماماً كما يفعل المفسر.
//
// (EN) This file implements stdlib module import checking for the compiler.
//      Goal: Unify compiler behavior with interpreter — builtin functions
//      belonging to stdlib modules (e.g. رياضيات, نصوص, أساسيات) only work
//      after importing the appropriate module, exactly as the interpreter does.
//
// المرجع: shared/builtins/include/module_definitions.h (السجل المركزي الموحّد ADR-003)
//         shared/builtins/include/builtin_registry.h
// ============================================================================

#include "sir_builder.h"
#include "module_definitions.h"
#include "builtin_registry.h"
#include <unordered_map>
#include <unordered_set>

// (AR) اختصارات لأسماء الدوال المركزية — مصدر حقيقة واحد
namespace Bc = Sad::Builtins::Names::Core;
namespace Bt = Sad::Builtins::Names::TypeCtor;
namespace Bm = Sad::Builtins::Names::Math;
namespace Bs = Sad::Builtins::Names::Strings;
namespace Ba = Sad::Builtins::Names::Arrays;
namespace Bb = Sad::Builtins::Names::Basics;
namespace Bcc = Sad::Builtins::Names::Concurrency;
namespace Bmp = Sad::Builtins::Names::Maps;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ================================================================
            // (AR) isStdlibModuleImported — تحقق من استيراد وحدة مع حل الأسماء المستعارة
            // (EN) Check if a module is imported with alias resolution
            //
            // (AR) يستخدم السجل المركزي الموحّد (ADR-003) بدلاً من خريطة مكررة.
            //      MODULE_ALIASES في module_definitions.h هو المصدر الوحيد للحقيقة.
            // (EN) Uses the unified central registry (ADR-003) instead of duplicated map.
            //      MODULE_ALIASES in module_definitions.h is the single source of truth.
            // ================================================================
            bool SIRBuilder::isStdlibModuleImported(const std::string &moduleName) const
            {
                // (AR) خطوة 1: حل اسم الوحدة إلى ModuleId عبر السجل المركزي
                // (EN) Step 1: Resolve module name to ModuleId via central registry
                auto targetId = Sad::Builtins::resolveModuleId(moduleName);
                if (targetId == Sad::Builtins::ModuleId::NONE)
                {
                    // (AR) اسم غير معروف — تحقق مباشر من القائمة (fallback)
                    // (EN) Unknown name — direct check (fallback)
                    return processedModules_.count(moduleName) > 0;
                }

                // (AR) خطوة 2: تحقق مما إذا كان أي اسم مستعار لنفس الوحدة مستورداً
                // (EN) Step 2: Check if any alias of the same module is imported
                for (const auto &entry : Sad::Builtins::MODULE_ALIASES)
                {
                    if (entry.moduleId == targetId)
                    {
                        if (processedModules_.count(std::string(entry.name)) > 0)
                        {
                            return true;
                        }
                    }
                }
                return false;
            }

            // ================================================================
            // (AR) getRequiredModuleForBuiltin — تحديد الوحدة المطلوبة لكل دالة
            // (EN) Determine required module for each builtin function
            //
            // (AR) الخريطة مبنية وفق سلوك المفسر:
            //      - builtin_module_strings.cpp → وحدة "نصوص"
            //      - builtin_module_math.cpp → وحدة "رياضيات"
            //      - builtin_module_basics.cpp → وحدة "أساسيات"
            //      - builtin_module_maps.cpp → وحدة "خرائط"
            //      - الدوال الأساسية (اطبع، طول، رقم...) → بدون وحدة (أساسية)
            //      - الدوال متعددة الوحدات: تُفصل بـ "|" (أي واحدة كافية)
            //
            // (EN) Map is built according to interpreter behavior:
            //      - builtin_module_strings.cpp → module "نصوص"
            //      - builtin_module_math.cpp → module "رياضيات"
            //      - builtin_module_basics.cpp → module "أساسيات"
            //      - builtin_module_maps.cpp → module "خرائط"
            //      - Core functions (print, length, int...) → no module (core)
            //      - Multi-module functions: separated by "|" (any one is sufficient)
            // ================================================================
            std::string SIRBuilder::getRequiredModuleForBuiltin(const std::string &funcName)
            {
                // (AR) خريطة ثابتة: اسم الدالة → الوحدة المطلوبة
                //      سلسلة فارغة = دالة أساسية (لا تحتاج استيراد)
                //      "|" = أي من الوحدات المذكورة كافية (مثال: "نصوص|رياضيات")
                // (EN) Static map: function name → required module
                //      Empty string = core function (no import needed)
                //      "|" = any of the listed modules is sufficient
                static const std::unordered_map<std::string, std::string> funcToModule = {
                    // ═══════════════════════════════════════════════════════════
                    // (AR) دوال أساسية — لا تحتاج استيراد (core builtins)
                    // (EN) Core builtins — no import needed
                    // ═══════════════════════════════════════════════════════════
                    // إدخال/إخراج
                    {std::string(Bc::LENGTH), ""},
                    {std::string(Bt::TO_INT), ""},
                    {std::string(Bt::TO_FLOAT), ""},
                    {std::string(Bt::TO_STRING), ""},
                    {std::string(Bt::TO_BOOL), ""},
                    {std::string(Bc::PRINT), ""},
                    {std::string(Bc::PRINTLN), ""},
                    {std::string(Bc::READ), ""},
                    {std::string(Bm::RANDOM), ""},
                    {"نم", ""},
                    {std::string(Bc::TYPE), ""},
                    {"قراءة_سطر", ""},
                    {"مسح_الشاشة", ""},
                    // رياضيات أساسية — تعمل بدون استيراد
                    {std::string(Bm::ABS), ""},
                    {std::string(Bm::MAX), ""},
                    {std::string(Bm::MIN), ""},
                    {std::string(Bm::ROUND), ""},
                    {std::string(Bm::FLOOR), ""},
                    {std::string(Bm::CEIL), ""},
                    {std::string(Bm::SQUARE), ""},
                    {std::string(Bm::POWER), ""},
                    {std::string(Bm::SIN), ""},
                    {std::string(Bm::TAN), ""},
                    {std::string(Bm::PI_ALT), ""},
                    {std::string(Bs::TRIM), ""},

                    // ═══════════════════════════════════════════════════════════
                    // (AR) وحدة "نصوص"
                    // ═══════════════════════════════════════════════════════════
                    {std::string(Bs::STR_LENGTH), "نصوص"},
                    {std::string(Bs::CHAR_CODE), "نصوص"},
                    {std::string(Bs::FIND), "نصوص"},
                    {std::string(Bs::REPLACE), "نصوص"},
                    {std::string(Bs::SUBSTRING), "نصوص"},
                    {std::string(Bs::TO_LOWER), "نصوص"},
                    {std::string(Bs::TO_UPPER), "نصوص"},
                    {std::string(Bs::SPLIT), "نصوص"},
                    {std::string(Bs::JOIN), "نصوص"},
                    {std::string(Bs::STARTS_WITH), "نصوص"},
                    {std::string(Bs::ENDS_WITH), "نصوص"},
                    {std::string(Bs::CONTAINS), "نصوص"},
                    // مصفوفات
                    {std::string(Ba::ADD), "نصوص"},
                    {std::string(Ba::REMOVE), "نصوص"},
                    {std::string(Ba::SIZE), "نصوص"},
                    {std::string(Ba::INDEX_OF), "نصوص"},
                    {std::string(Ba::ARRAY_CONTAINS), "نصوص"},
                    {std::string(Ba::REVERSE), "نصوص"},
                    {std::string(Ba::SORT), "نصوص"},
                    {std::string(Ba::FIRST), "نصوص"},
                    {std::string(Ba::LAST), "نصوص"},
                    {std::string(Ba::SLICE), "نصوص"},
                    // رياضيات في وحدة نصوص
                    {std::string(Bm::SQRT), "نصوص"},
                    {std::string(Bm::SUM), "نصوص"},
                    // فحص الأنواع
                    {"هو_رقم", "نصوص"},
                    {"هو_رقم_صحيح", "نصوص"},
                    {"هو_عشري", "نصوص"},
                    {"هو_رقم_عشري", "نصوص"},
                    {"هو_نص", "نصوص"},
                    {"هو_مصفوفة", "نصوص"},

                    // ═══════════════════════════════════════════════════════════
                    // (AR) وحدة "أساسيات"
                    // ═══════════════════════════════════════════════════════════
                    {std::string(Bb::RANGE), "أساسيات"},
                    {std::string(Bb::EXIT), "أساسيات"},
                    {std::string(Bb::READ_FILE), "أساسيات"},
                    {std::string(Bb::WRITE_FILE), "أساسيات"},
                    {std::string(Bb::WRITE_BYTES), "أساسيات"},
                    {std::string(Bb::READ_BYTES), "أساسيات"},
                    {"أضف_إلى_ملف", "أساسيات"},
                    {std::string(Bb::DELETE_FILE), "أساسيات"},
                    {"انسخ_ملف", "أساسيات"},
                    {"انقل_ملف", "أساسيات"},
                    {std::string(Bb::MKDIR), "أساسيات"},
                    {"اسرد_مجلد", "أساسيات"},

                    // ═══════════════════════════════════════════════════════════
                    // (AR) وحدة "خرائط" — تضمّ التعابيرَ النمطيّةَ ودوالَّ المسارات
                    //      الإضافيّة، ومنها «حجم_ملف» (انظر builtins/maps.yaml).
                    //      تُسجَّل هنا كي تكون رسالةُ الخطأ «تحتاج استيراد خرائط»
                    //      لا «دالة غير معرّفة» — فالفرقُ بينهما دقيقتان وساعة.
                    // (EN) The "خرائط" module also carries regex and extra path
                    //      helpers such as file_size; registering it here turns
                    //      "undefined function" into "needs `استورد خرائط`".
                    // ═══════════════════════════════════════════════════════════
                    {std::string(Bmp::FILE_SIZE), "خرائط"},

                    // ═══════════════════════════════════════════════════════════
                    // (AR) وحدة "رياضيات" — دوال متقدمة تحتاج استيراد
                    // ═══════════════════════════════════════════════════════════
                    {std::string(Bm::LOG), "رياضيات"},
                    {std::string(Bm::LOG10), "رياضيات"},
                    {std::string(Bm::LOG2), "رياضيات"},
                    {std::string(Bm::EXP), "رياضيات"},
                    {std::string(Bm::SIGN), "رياضيات"},
                    {std::string(Bm::CLAMP), "رياضيات"},
                    {std::string(Bm::TRUNCATE), "رياضيات"},
                    {std::string(Bm::FMOD), "رياضيات"},
                    {std::string(Bm::PI), "رياضيات"},
                    {std::string(Bm::E), "رياضيات"},
                    {std::string(Bm::LERP), "رياضيات"},
                    {std::string(Bm::RANDOM_RANGE), "رياضيات"},
                    {std::string(Bm::RANDOM_FLOAT), "رياضيات"},
                };

                auto it = funcToModule.find(funcName);
                if (it != funcToModule.end())
                {
                    return it->second;
                }

                // (AR) فحص السجل المركزي الموحّد — ADR-003
                //      يدعم الدوال المعرّفة في builtin_registry.h (60+ دالة)
                //      يُرجع اسم الوحدة الأساسي (canonical) أو فراغ إذا أساسية
                // (EN) Check unified central registry — ADR-003
                auto suggestion = Sad::Builtins::suggestModuleForFunction(
                    std::string_view(funcName));
                if (!suggestion.empty())
                {
                    return std::string(suggestion);
                }

                // (AR) دالة غير معروفة — لا وحدة مطلوبة (ستُعالج كدالة مستخدم)
                // (EN) Unknown function — no module required (will be handled as user function)
                return "";
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
