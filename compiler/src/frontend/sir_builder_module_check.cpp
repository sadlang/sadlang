// sir_builder_module_check.cpp - بوّابةُ استيرادِ الوحدات القياسيّة في المصرّف
//
// (AR) الغرض: تحديدُ الوحدةِ التي يلزم استيرادُها قبل نداءِ مدمَجةٍ ما — بحيث
//      يطابق المصرِّفُ المفسّرَ حرفًا، لأنّ **كليهما يقرأ الجدولَ المولَّدَ نفسَه**
//      من مصدر الحقيقة (`require_import` + `module` في language-truth/builtins/).
//
//      🔴 كانت هنا خريطةٌ مكتوبةٌ باليد (٨١ مدخلًا) تتجاوز مصدرَ الحقيقة، ونظيرتُها
//         في المفسّر (`builtin_registry.cpp`) تتجاوزه أيضًا وتخالفها. فتباعدت
//         الثلاثةُ: `جذر` — وهي رياضيّةٌ — كانت تطلب في المصرّف استيرادَ **«نصوص»**،
//         ويقبلها المفسّرُ حرّةً، ويقول SoT إنّها من «رياضيات». وكان أثرُ ذلك أنّ
//         ثلاثين مدمَجةً تعمل مفسَّرةً ولا تُصرَّف البتّة، برسالةٍ تدّعي أنّ الدالّةَ
//         **غيرُ معرَّفة** لا أنّ استيرادًا ينقص.
//
//      فحُذفت الخريطتان، وصار المصدرُ واحدًا مولَّدًا: `Sad::Builtins::IMPORT_GATE`.
//      ويمنع حارسُ scripts/codegen/check_no_handwritten_import_gate.py عودةَ أيّ
//      خريطةٍ يدويّةٍ إلى أيّ محرّك.
//
// (EN) Purpose: determine which module must be imported before calling a builtin,
//      so the compiler matches the interpreter exactly — both now read the SAME
//      generated table derived from the YAML SoT. Two hand-written maps (one per
//      engine) used to override the SoT and contradict each other; they are gone.
//
// المرجع: shared/builtins/generated/builtin_registry_generated.h (IMPORT_GATE)
//         shared/builtins/include/module_definitions.h (MODULE_ALIASES، ADR-003)

#include "sir_builder.h"
#include "module_definitions.h"
#include "builtin_registry.h"
#include <string>

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
            // (AR) getRequiredModuleForBuiltin — الوحدةُ اللازمةُ لمدمَجةٍ بعينها
            //      فراغٌ = أساسيّةٌ تعمل بلا استيراد.
            //
            //      لا منطقَ هنا: استعلامٌ عن الجدول المولَّد ليس إلّا. وكلُّ تعديلٍ
            //      في السلوك موضعُه **مصدرُ الحقيقة** (`require_import` / `module`)
            //      ثمّ `py -3.12 x.py gen`، لا هذا الملفّ.
            // (EN) The module a builtin requires; empty means core (no import).
            //      No logic here — a lookup into the generated table. Behaviour is
            //      changed in the YAML SoT and regenerated, never edited here.
            // ================================================================
            std::string SIRBuilder::getRequiredModuleForBuiltin(const std::string &funcName)
            {
                // (AR) الاسمُ قد يُحجَب في أكثرَ من وحدة (`أرسل` في «شبكة» و«تزامن_متقدم»)،
                //      فتُعاد كلُّها مفصولةً بـ«|» — والمنادي يقبل إن استُوردت أيٌّ منها.
                // (EN) A name may be gated by several modules (`أرسل` in both network and
                //      async); return them all '|'-separated — the caller accepts any one.
                std::string required;
                Sad::Builtins::importGateUnsatisfiedModuleName(
                    std::string_view(funcName),
                    [&required](std::string_view moduleName)
                    {
                        if (!required.empty())
                            required += '|';
                        required.append(moduleName);
                        return false; // (AR) اجمعْ كلَّها / (EN) collect them all
                    });
                return required;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
