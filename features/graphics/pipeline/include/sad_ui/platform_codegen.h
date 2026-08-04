/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: platform_codegen.h
 * المسار: features/graphics/pipeline/include/sad_ui/platform_codegen.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) بابُ توليدِ كودِ المنصّة الذي تفتحه لغةُ ص — نقطةُ الدخولِ الوحيدةُ التي
 *      يستدعيها **المحرّكان معًا**: المفسّر (مدمَجات توليد_ويب/أندرويد/…)
 *      ووقتُ تشغيلِ المترجم (sad_generate_*). كلُّ منطقِ التوليدِ وخياراتِه
 *      داخلَ المكتبة هنا، فلا يبقى للمحرّكَين إلّا التمريرُ ⇒ التكافؤُ البايتيُّ
 *      بينهما **بالبناء** لا بالمصادفة.
 *
 *      قبلَ هذا البابِ كانت مولّداتُ Compose/SwiftUI/AppKit تُبنى في كلّ ثنائيّ
 *      ولا يبلغها مستخدمُ لغةِ ص إلّا من C++ (ث٦ من تحقيقِ دعمِ المنصّات).
 *
 * (EN) The single library entry point for platform code generation, called by
 *      both engines (interpreter builtins and compiler runtime) — parity by
 *      construction rather than by coincidence.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_PLATFORM_CODEGEN_H
#define SAD_UI_PLATFORM_CODEGEN_H

#include "sad_ui/ir.h"

#include <cstdint>
#include <memory>
#include <string>

namespace sad {
namespace ui {

/**
 * @brief (AR) هدفُ توليدِ كودِ المنصّة المفتوحُ للغةِ ص.
 * @brief (EN) A platform code-generation target exposed to the Sad language.
 */
enum class PlatformCodegenTarget : uint8_t {
    Web,     ///< توليد_ويب     — HTML/CSS
    Android, ///< توليد_أندرويد — Kotlin/Jetpack Compose
    IOS,     ///< توليد_آي_أو_إس — Swift/SwiftUI
    MacOS,   ///< توليد_ماك     — Swift/AppKit
};

/**
 * @brief (AR) يولّد كودَ المنصّة من جذرِ شجرةِ IR.
 *
 * @param target الهدفُ المطلوب.
 * @param root   جذرُ الشجرة؛ مؤشّرٌ فارغٌ ⇒ نصٌّ فارغ.
 * @param name   الاسمُ الذي يُسمّي المخرَج — عنوانُ الصفحةِ للويب، واسمُ
 *               الدالّة/البنيةِ المولَّدةِ لأندرويد/آي_أو_إس/ماك. فارغٌ ⇒
 *               الافتراضيُّ المملوكُ للمكتبةِ (لا افتراضيَّ في المحرّكَين).
 * @return       نصُّ الكودِ المولَّد؛ فارغٌ إن كان المولّدُ مُعطَّلًا في هذا البناء.
 */
std::string generatePlatformCode(PlatformCodegenTarget target,
                                 const std::shared_ptr<IRNode>& root,
                                 const std::string& name);

} // namespace ui
} // namespace sad

#endif // SAD_UI_PLATFORM_CODEGEN_H
