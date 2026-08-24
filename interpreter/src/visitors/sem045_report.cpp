/**
 * @file sem045_report.cpp
 * (AR) تنفيذُ بابِ الإبلاغِ الواحد لـSEM045 — انظر العنوانَ المرافق.
 *      نصوصُ السطورِ وأسماءُ المتغيّراتِ القالبيّةِ ثوابتُ مسمّاةٌ هنا
 *      حصرًا (قاعدة «لا سلاسلَ مباشرةً متناثرة»)، وكلمةُ «فراغ» تُشتقُّ
 *      من kindToArabic لا تُكتبُ يدويًّا — نصُّ الرسالةِ الكاملُ يعيش في
 *      كتالوج الأخطاء وحدَه.
 * (EN) Implementation of the single SEM045 reporting door — see the header.
 *      The stderr fragments and template-placeholder keys are named
 *      constants here exclusively (no scattered raw literals), and the
 *      word for Void is derived from kindToArabic, never hand-written —
 *      the full message text lives in the error catalog alone.
 */
#include "visitors/sem045_report.h"

#include <iostream>
#include <map>
#include <utility>

#include "error_manager.h"
#include "runtime_throw.h"

namespace Sad
{
    namespace Interpreter
    {
        namespace Sem045
        {
            namespace
            {
                // (AR) مفاتيحُ قالبِ الكتالوج (error_messages_generated.cpp: SEM045)
                // (EN) Catalog template keys (error_messages_generated.cpp: SEM045)
                constexpr const char *kPlaceholderName = "name";
                constexpr const char *kPlaceholderTypeName = "type_name";
                constexpr const char *kPlaceholderVoidWord = "void_word";

                // (AR) شظايا سطرِ stderr — الصياغةُ المقيسةُ التي ترصدها فحوصُ
                //      ns07–ns10 (لا تُغيَّر دون تغييرِ الفحوص معًا).
                // (EN) stderr line fragments — the measured wording the ns07–ns10
                //      checks grep for (change only together with the checks).
                constexpr const char *kFatalPrefix = "[خطأ نوع SEM045] سطر ";
                constexpr const char *kWarnPrefix = "[تحذير نوع SEM045] سطر ";
                constexpr const char *kSlotOpen = ": الخانة '";
                constexpr const char *kSlotClose = "' أُسند إليها '";
                constexpr const char *kLineClose = "'";
            } // namespace

            bool kindIsGuarded(Sad::Types::SadTypeKind declaredKind)
            {
                using Sad::Types::SadTypeKind;
                return declaredKind != SadTypeKind::Unknown &&
                       declaredKind != SadTypeKind::Any &&
                       declaredKind != SadTypeKind::Void &&
                       declaredKind != SadTypeKind::Null;
            }

            void reportVoidCrossing(const std::string &slotName,
                                    const std::string &typeArabicName,
                                    const Sad::Lexer::Position &position,
                                    Sad::NullSafety::Strictness strictness)
            {
                if (strictness == Sad::NullSafety::Strictness::Ignore)
                    return;

                const std::string voidWord =
                    Sad::Types::kindToArabic(Sad::Types::SadTypeKind::Void);
                std::map<std::string, std::string> placeholders{
                    {kPlaceholderName, slotName},
                    {kPlaceholderTypeName, typeArabicName},
                    {kPlaceholderVoidWord, voidWord}};

                if (strictness == Sad::NullSafety::Strictness::Fatal)
                {
                    std::cerr << kFatalPrefix << position.line << kSlotOpen
                              << slotName << kSlotClose << voidWord << kLineClose
                              << std::endl;
                    // (AR) يبلّغ الكتالوج ثم يرمي RuntimeAbort — فيتوقف التنفيذ
                    //      موضعيًّا حتى داخل أجسام الدوال.
                    // (EN) Reports from catalog then throws RuntimeAbort — stops
                    //      locally even inside function bodies.
                    Sad::Errors::throwRuntime(
                        Sad::Errors::ErrorCode::SEM_VOID_ASSIGNED_TO_TYPED_SLOT,
                        position, std::move(placeholders));
                }

                std::cerr << kWarnPrefix << position.line << kSlotOpen << slotName
                          << kSlotClose << voidWord << kLineClose << std::endl;
                Sad::Errors::RenderContext renderContext(
                    Sad::Errors::SourceLocation("", position.line, position.column),
                    std::move(placeholders));
                Sad::Errors::ErrorManager::getInstance().reportWarningFromCatalog(
                    Sad::Errors::ErrorCode::SEM_VOID_ASSIGNED_TO_TYPED_SLOT,
                    Sad::Errors::SourceLocation("", position.line, position.column),
                    renderContext);
            }
        } // namespace Sem045
    } // namespace Interpreter
} // namespace Sad
