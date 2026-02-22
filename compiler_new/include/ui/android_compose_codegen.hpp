/**
 * ==========================================================================
 * ملف: android_compose_codegen.hpp
 * الوصف: مولد كود أندرويد — SadUI IR → Kotlin/Jetpack Compose
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * يُولّد مشروع Kotlin/Compose كامل من SadUI IR.
 */

#ifndef ANDROID_COMPOSE_CODEGEN_HPP
#define ANDROID_COMPOSE_CODEGEN_HPP

#include "sad_ui_codegen.hpp"
#include <sstream>

namespace sad {
namespace ui {
namespace ir {

/**
 * @brief إعدادات مولد أندرويد
 */
struct AndroidCodeGenConfig {
    int minSdk = 24;
    int targetSdk = 34;
    int compileSdk = 34;
    std::string kotlinVersion = "1.9.22";
    std::string composeVersion = "1.6.0";
    std::string gradleVersion = "8.2.0";
    std::vector<std::string> architectures = {"arm64-v8a", "x86_64"};
    std::vector<std::string> permissions;
};

/**
 * @brief مولد Kotlin/Jetpack Compose
 *
 * يُولّد:
 * - مشروع Gradle كامل
 * - Activity + Compose screens
 * - Navigation (NavHost)
 * - Theme (MaterialTheme)
 * - State management (mutableStateOf/ViewModel)
 */
class AndroidComposeCodeGen : public ICodeGenerator {
public:
    explicit AndroidComposeCodeGen(const AndroidCodeGenConfig& config = {});
    ~AndroidComposeCodeGen() override = default;

    CodeGenResult generate(const AppIR& app) override;
    CodeGenResult generateScreen(const ScreenIR& screen) override;
    TargetPlatform targetPlatform() const override { return TargetPlatform::Android; }
    std::string platformName() const override { return "Android (Compose)"; }

private:
    AndroidCodeGenConfig config_;

    /// توليد ملف Compose لشاشة
    std::string generateScreenKt(const ScreenIR& screen,
                                  const std::string& packageName,
                                  const ThemeIR& theme);

    /// توليد MainActivity.kt
    std::string generateMainActivity(const AppIR& app);

    /// توليد ملف الثيمة
    std::string generateThemeKt(const AppIR& app);

    /// توليد ملف التنقل
    std::string generateNavigationKt(const AppIR& app);

    /// توليد build.gradle.kts
    std::string generateBuildGradle(const AppIR& app);

    /// توليد AndroidManifest.xml
    std::string generateManifest(const AppIR& app);

    /// توليد settings.gradle.kts
    std::string generateSettingsGradle(const AppIR& app);

    /// توليد عقدة Compose (تكراري)
    void emitComposable(std::ostringstream& ss, const UINodeIRPtr& node,
                         const ThemeIR& theme, int indent);

    /// توليد Modifier من StyleIR
    std::string emitModifier(const StyleIR& style);

    /// تحويل لون إلى Color() في Compose
    std::string composeColor(const Color& c);

    /// مسافة بادئة
    std::string pad(int indent);
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // ANDROID_COMPOSE_CODEGEN_HPP
