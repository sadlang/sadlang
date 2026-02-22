/**
 * ==========================================================================
 * ملف: desktop_sdl2_codegen.hpp
 * الوصف: مولد كود سطح المكتب — SadUI IR → استدعاءات SDL2/UI Framework
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * يُولّد كود C++ يستخدم نظام sad_ui_framework.h الموجود في stdlib/graphics/
 * لعرض الواجهات على سطح المكتب (Windows/Linux/macOS).
 */

#ifndef DESKTOP_SDL2_CODEGEN_HPP
#define DESKTOP_SDL2_CODEGEN_HPP

#include "sad_ui_codegen.hpp"
#include <sstream>

namespace sad {
namespace ui {
namespace ir {

/**
 * @brief إعدادات مولد سطح المكتب
 */
struct DesktopCodeGenConfig {
    int windowWidth = 400;
    int windowHeight = 700;
    std::string windowTitle = "تطبيق ص";
    bool enableHotReload = false;   ///< دعم إعادة التحميل السريع
    bool generateLauncher = true;   ///< توليد main.cpp
    bool useSDL2 = true;           ///< استخدام SDL2 (الافتراضي)
};

/**
 * @brief مولد كود سطح المكتب (SDL2)
 *
 * يُولّد كود C++ يستخدم:
 * - sad_ui_framework.h (Widget/Theme/RGBA)
 * - sad_ui_api.cpp (createWidget/renderWidget)
 * - sad_state.h (الحالة التفاعلية)
 * - sad_navigator.h (التنقل)
 *
 * النتيجة: ملفات C++ جاهزة للترجمة مع SDL2.
 */
class DesktopSDL2CodeGen : public ICodeGenerator {
public:
    explicit DesktopSDL2CodeGen(const DesktopCodeGenConfig& config = {});
    ~DesktopSDL2CodeGen() override = default;

    CodeGenResult generate(const AppIR& app) override;
    CodeGenResult generateScreen(const ScreenIR& screen) override;
    TargetPlatform targetPlatform() const override { return TargetPlatform::Desktop; }
    std::string platformName() const override { return "Desktop (SDL2)"; }

private:
    DesktopCodeGenConfig config_;
    int nextVarId_ = 0; ///< عداد لأسماء المتغيرات الفريدة

    /// توليد ملف شاشة واحدة
    std::string generateScreenSource(const ScreenIR& screen, const ThemeIR& theme);

    /// توليد ملف main.cpp
    std::string generateMainCpp(const AppIR& app);

    /// توليد كود CMakeLists.txt
    std::string generateCMakeLists(const AppIR& app);

    /// توليد تعريف widget تكراريًا — يرجع اسم المتغير
    std::string emitWidget(std::ostringstream& ss, const UINodeIRPtr& node,
                    const ThemeIR& theme, int indent);

    /// توليد ربط الحالة
    void emitStateDeclarations(std::ostringstream& ss,
                                const std::vector<StateBindingIR>& bindings, int indent);

    /// توليد معالجات الأحداث
    void emitEventHandlers(std::ostringstream& ss,
                           const std::vector<EventHandlerIR>& handlers,
                           const std::string& widgetVar, int indent);

    /// تحويل لون إلى RGBA
    std::string colorToRGBA(const Color& c);

    /// توليد استدعاء دالة المصنع المناسبة حسب نوع الـ Widget
    std::string widgetFactoryCall(const UINodeIRPtr& node);

    /// إضافة مسافة بادئة
    std::string pad(int indent);
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // DESKTOP_SDL2_CODEGEN_HPP
