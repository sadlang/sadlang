/**
 * ==========================================================================
 * ملف: ios_swiftui_codegen.hpp
 * الوصف: مولد كود iOS — SadUI IR → Swift/SwiftUI
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * يُولّد مشروع Xcode/SwiftUI كامل من SadUI IR.
 */

#ifndef IOS_SWIFTUI_CODEGEN_HPP
#define IOS_SWIFTUI_CODEGEN_HPP

#include "sad_ui_codegen.hpp"
#include <sstream>

namespace sad {
namespace ui {
namespace ir {

/**
 * @brief إعدادات مولد iOS SwiftUI
 */
struct IOSCodeGenConfig {
    /// الحد الأدنى لنظام التشغيل
    std::string deploymentTarget = "16.0";

    /// إصدار Swift
    std::string swiftVersion = "5.9";

    /// Bundle Identifier
    std::string bundleIdentifier = "com.sad.app";

    /// اسم الفريق (code signing)
    std::string teamId = "";

    /// استخدام Observable (iOS 17+)
    bool useObservable = false;

    /// توليد Preview providers
    bool generatePreviews = true;

    /// توليد تعليقات عربية
    bool includeArabicComments = true;

    /// توليد widget SnapKit أو SwiftUI
    bool useSwiftUI = true;

    /// دعم iPad
    bool supportIPad = true;
};

/**
 * @brief مولد Swift/SwiftUI
 *
 * يُولّد:
 * - مشروع Xcode أساسي (Package.swift أو project structure)
 * - SwiftUI Views (struct : View)
 * - Navigation (NavigationStack)
 * - Theme/Appearance
 * - State management (@State, @StateObject, @Observable)
 */
class IOSSwiftUICodeGen : public ICodeGenerator {
public:
    explicit IOSSwiftUICodeGen(const IOSCodeGenConfig& config = {});
    ~IOSSwiftUICodeGen() override = default;

    CodeGenResult generate(const AppIR& app) override;
    CodeGenResult generateScreen(const ScreenIR& screen) override;
    TargetPlatform targetPlatform() const override { return TargetPlatform::iOS; }
    std::string platformName() const override { return "iOS (SwiftUI)"; }

private:
    IOSCodeGenConfig config_;

    /// توليد ملف SwiftUI لشاشة
    std::string generateScreenSwift(const ScreenIR& screen,
                                     const ThemeIR& theme);

    /// توليد App.swift (entry point)
    std::string generateAppSwift(const AppIR& app);

    /// توليد ملف الثيمة
    std::string generateThemeSwift(const AppIR& app);

    /// توليد Router/Navigation
    std::string generateNavigationSwift(const AppIR& app);

    /// توليد Package.swift (SPM)
    std::string generatePackageSwift(const AppIR& app);

    /// توليد Info.plist
    std::string generateInfoPlist(const AppIR& app);

    /// توليد كود SwiftUI لعقدة (تكراري)
    void emitView(std::ostringstream& ss, const UINodeIRPtr& node,
                   const ThemeIR& theme, int indent);

    /// توليد Modifier chain من StyleIR
    void emitModifiers(std::ostringstream& ss, const StyleIR& style, int indent);

    /// توليد State declarations
    void emitStateProperties(std::ostringstream& ss,
                              const std::vector<StateBindingIR>& bindings,
                              int indent);

    /// تحويل لون إلى Color() في SwiftUI
    std::string swiftColor(const Color& c);

    /// مسافة بادئة
    std::string pad(int indent);
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // IOS_SWIFTUI_CODEGEN_HPP
