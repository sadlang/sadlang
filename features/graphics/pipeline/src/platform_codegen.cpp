/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: platform_codegen.cpp
 * المسار: features/graphics/pipeline/src/platform_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) تنفيذُ بابِ توليدِ كودِ المنصّة (انظر platform_codegen.h): يبني وحدةَ IR
 *      من الجذرِ ويسلّمها لمولّدِ الهدف بخياراتِ المكتبةِ الافتراضيّة.
 *      لا يقرأ هذا الملفُّ إعداداتِ محرّكٍ ولا يكتب أخطاءَه — المحرّكُ يقرّر
 *      ماذا يفعل بالنصّ الفارغ.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/platform_codegen.h"

#ifndef SAD_NO_COMPOSE_CODEGEN
#include "sad_ui/android/compose_codegen.h"
#endif
#ifndef SAD_NO_SWIFTUI_CODEGEN
#include "sad_ui/ios/swiftui_codegen.h"
#endif
#include "sad_ui/web/html_codegen.h"
#ifndef SAD_NO_APPKIT_CODEGEN
#include "sad_ui/macos/appkit_codegen.h"
#endif

namespace sad {
namespace ui {

#if !defined(SAD_NO_COMPOSE_CODEGEN) || !defined(SAD_NO_SWIFTUI_CODEGEN) \
    || !defined(SAD_NO_APPKIT_CODEGEN)
namespace {

// (AR) الاسمُ الافتراضيُّ للمخرَجِ المولَّد، يملكه هذا البابُ وحدَه.
//      (Amelia مراجعة١، متوسطة) كان يُستعارُ من SwiftUICodegenOptions ثمّ
//      AppKitCodegenOptions، فيصيرُ **فارغًا** في بناءٍ يُبقي Compose ويُعطّل
//      الاثنين ⇒ `fun () {` — كوتلن مكسورةٌ صامتة. ثابتٌ مسمًّى واحدٌ هنا
//      يُنهي الاستعارةَ ويضمنُ اسمًا صالحًا في كلّ تشكيلاتِ البناء؛ ويبقى
//      المولّداتُ الثلاثةُ متّفقةً على المخرَجِ نفسِه.
constexpr const char* kDefaultGeneratedName = "SadApp";

} // namespace
#endif

std::string generatePlatformCode(PlatformCodegenTarget target,
                                 const std::shared_ptr<IRNode>& root,
                                 const std::string& name) {
    if (!root)
        return {};

    IRModule module;
    module.root = root;

    switch (target) {
        case PlatformCodegenTarget::Web: {
            // (AR) الويب وحدَه يُسمّي المخرَجَ بعنوانِ الصفحةِ لا باسمِ الوحدة
            //      (module.name لا يظهر في HTML)؛ الافتراضيُّ من خيارات المكتبة.
            web::HtmlCodegenOptions opts;
            if (!name.empty())
                opts.title = name;
            return web::HtmlCodegen(opts).generate(module);
        }
        case PlatformCodegenTarget::Android: {
#ifndef SAD_NO_COMPOSE_CODEGEN
            module.name = name.empty() ? kDefaultGeneratedName : name;
            return android::ComposeCodegen(android::ComposeCodegenOptions{}).generate(module);
#else
            return {};
#endif
        }
        case PlatformCodegenTarget::IOS: {
#ifndef SAD_NO_SWIFTUI_CODEGEN
            module.name = name.empty() ? kDefaultGeneratedName : name;
            return ios::SwiftUICodegen(ios::SwiftUICodegenOptions{}).generate(module);
#else
            return {};
#endif
        }
        case PlatformCodegenTarget::MacOS: {
#ifndef SAD_NO_APPKIT_CODEGEN
            module.name = name.empty() ? kDefaultGeneratedName : name;
            return macos::AppKitCodegen(macos::AppKitCodegenOptions{}).generate(module);
#else
            return {};
#endif
        }
    }
    return {};
}

} // namespace ui
} // namespace sad
