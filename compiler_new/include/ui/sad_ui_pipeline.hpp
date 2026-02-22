/**
 * ==========================================================================
 * ملف: sad_ui_pipeline.hpp
 * الوصف: خط أنابيب SadUI الموحد — من المصدر إلى الكود الأصلي
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                     خط أنابيب SadUI الموحد                               ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  .ص مصدر → UIParser → UINode AST → IRBuilder → SadUI IR               ║
 * ║                                                   ↓                     ║
 * ║                                            IRValidator                  ║
 * ║                                                   ↓                     ║
 * ║                                            IROptimizer                  ║
 * ║                                                   ↓                     ║
 * ║                                  ┌──────────┬──────────┐               ║
 * ║                                  ↓          ↓          ↓               ║
 * ║                               Desktop   Android     iOS                ║
 * ║                              (SDL2)   (Compose)  (SwiftUI)            ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * الاستخدام:
 * @code
 * SadUIPipeline pipeline;
 * pipeline.setTargetPlatform(TargetPlatform::Android);
 *
 * // من ملف مصدر
 * auto result = pipeline.buildFromSource(sourceCode, "مشروعي");
 *
 * // من ملف مشروع
 * auto result = pipeline.buildFromProject("مشروع.صم");
 * @endcode
 */

#ifndef SAD_UI_PIPELINE_HPP
#define SAD_UI_PIPELINE_HPP

#include "sad_ui_ir.hpp"
#include "sad_ui_ir_builder.hpp"
#include "sad_ui_ir_validator.hpp"
#include "sad_ui_ir_optimizer.hpp"
#include "sad_ui_codegen.hpp"
#include "desktop_sdl2_codegen.hpp"
#include "android_compose_codegen.hpp"
#include "ios_swiftui_codegen.hpp"
#include "web_html_codegen.hpp"
#include <memory>

namespace sad {
namespace ui {
namespace ir {

/**
 * @brief إعدادات خط الأنابيب
 */
struct PipelineConfig {
    TargetPlatform platform = TargetPlatform::Desktop;
    bool validate = true;           ///< التحقق من صحة IR
    bool optimize = true;           ///< تحسين IR
    bool applyTheme = true;         ///< تطبيق الثيمة
    bool verbose = false;           ///< طباعة تفاصيل التقدم
    ThemeIR theme;                  ///< ثيمة التطبيق
    std::string outputDir;          ///< مجلد الإخراج
};

/**
 * @brief نتيجة خط الأنابيب
 */
struct PipelineResult {
    bool success = false;
    CodeGenResult codeGenResult;
    ValidationResult validationResult;
    OptimizationStats optimizationStats;
    std::optional<AppIR> appIR;     ///< IR النهائي (للتشخيص)
    std::vector<std::string> log;   ///< سجل التقدم
};

/**
 * @brief خط أنابيب SadUI الموحد
 *
 * يُدير العملية الكاملة:
 * 1. تحليل المصدر → UINode AST
 * 2. بناء SadUI IR
 * 3. التحقق من الصحة
 * 4. التحسين
 * 5. توليد الكود الأصلي
 */
class SadUIPipeline {
public:
    SadUIPipeline();
    ~SadUIPipeline() = default;

    /**
     * @brief بناء من ملف مصدر واحد
     * @param source كود المصدر
     * @param appName اسم التطبيق
     * @return نتيجة خط الأنابيب
     */
    PipelineResult buildFromSource(
        const std::string& source,
        const std::string& appName = "\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82\xd9\x8a" // تطبيقي
    );

    /**
     * @brief بناء من عدة تعريفات واجهة (بعد التحليل)
     * @param views تعريفات الواجهات
     * @param projectConfig إعدادات المشروع
     * @return نتيجة خط الأنابيب
     */
    PipelineResult buildFromViews(
        const std::vector<parser::ViewDefinition>& views,
        const std::unordered_map<std::string, std::string>& projectConfig
    );

    /**
     * @brief بناء من AppIR جاهز (تخطي التحليل والبناء)
     * @param app تعريف التطبيق IR
     * @return نتيجة خط الأنابيب
     */
    PipelineResult buildFromIR(AppIR& app);

    /// تعيين الإعدادات
    void setConfig(const PipelineConfig& config);

    /// الحصول على الإعدادات
    const PipelineConfig& getConfig() const { return config_; }

    /// تعيين المنصة المستهدفة
    void setTargetPlatform(TargetPlatform platform);

    /// تعيين الثيمة
    void setTheme(const ThemeIR& theme);

private:
    PipelineConfig config_;

    /// إنشاء مولد كود حسب المنصة
    std::unique_ptr<ICodeGenerator> createCodeGen();

    /// إضافة للسجل
    void log(PipelineResult& result, const std::string& msg);
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // SAD_UI_PIPELINE_HPP
