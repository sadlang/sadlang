/**
 * ==========================================================================
 * ملف: sad_ui_pipeline.cpp
 * الوصف: تنفيذ خط أنابيب SadUI الموحد
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/sad_ui_pipeline.hpp"
#include <sstream>

namespace sad {
namespace ui {
namespace ir {


SadUIPipeline::SadUIPipeline() = default;

void SadUIPipeline::setConfig(const PipelineConfig& config) {
    config_ = config;
}

void SadUIPipeline::setTargetPlatform(TargetPlatform platform) {
    config_.platform = platform;
}

void SadUIPipeline::setTheme(const ThemeIR& theme) {
    config_.theme = theme;
}


// ═══════════════════════════════════════════════════════════════════════════
// البناء من مصدر واحد
// ═══════════════════════════════════════════════════════════════════════════

PipelineResult SadUIPipeline::buildFromSource(
    const std::string& source,
    const std::string& appName
) {
    PipelineResult result;

    log(result, "\xd8\xa8\xd8\xaf\xd8\xa1 \xd8\xaa\xd8\xad\xd9\x84\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1..."); // بدء تحليل المصدر

    // 1. تحليل المصدر
    parser::UIParser parser;
    auto parseResult = parser.parseViewDefinition(source);

    if (!parseResult.success()) {
        log(result, "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x84\xd9\x8a\xd9\x84"); // فشل التحليل
        for (const auto& err : parseResult.errors) {
            result.codeGenResult.errors.push_back(err);
        }
        return result;
    }

    log(result, "\xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x84\xd9\x8a\xd9\x84 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"); // تم التحليل بنجاح

    // حزمة في ViewDefinition
    std::vector<parser::ViewDefinition> views;
    if (parseResult.value.has_value()) {
        views.push_back(std::move(*parseResult.value));
    }

    // إعدادات المشروع
    std::unordered_map<std::string, std::string> projectConfig;
    projectConfig["\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85"] = appName; // الاسم

    return buildFromViews(views, projectConfig);
}


// ═══════════════════════════════════════════════════════════════════════════
// البناء من تعريفات واجهات
// ═══════════════════════════════════════════════════════════════════════════

PipelineResult SadUIPipeline::buildFromViews(
    const std::vector<parser::ViewDefinition>& views,
    const std::unordered_map<std::string, std::string>& projectConfig
) {
    PipelineResult result;

    // 2. بناء SadUI IR
    log(result, "\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 SadUI IR..."); // بناء SadUI IR

    IRBuilderConfig builderConfig;
    builderConfig.targetPlatform = config_.platform;
    builderConfig.defaultRTL = config_.theme.isRTL;
    builderConfig.applyDefaultTheme = config_.applyTheme;
    builderConfig.defaultTheme = config_.theme;

    IRBuilder builder(builderConfig);
    auto irResult = builder.buildApp(views, projectConfig);

    if (!irResult.success()) {
        log(result, "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 IR"); // فشل بناء IR
        for (const auto& diag : irResult.diagnostics) {
            if (diag.level == DiagnosticLevel::Error) {
                result.codeGenResult.errors.push_back(diag.message);
            }
        }
        return result;
    }

    AppIR app = std::move(*irResult.value);

    log(result, "\xd8\xaa\xd9\x85 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 IR (" + std::to_string(app.screens.size())
                + " \xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9)"); // تم بناء IR (N شاشة)

    return buildFromIR(app);
}


// ═══════════════════════════════════════════════════════════════════════════
// البناء من IR جاهز
// ═══════════════════════════════════════════════════════════════════════════

PipelineResult SadUIPipeline::buildFromIR(AppIR& app) {
    PipelineResult result;

    // 3. التحقق من الصحة
    if (config_.validate) {
        log(result, "\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x82\xd9\x82 \xd9\x85\xd9\x86 \xd8\xa7\xd9\x84\xd8\xb5\xd8\xad\xd8\xa9..."); // التحقق من الصحة

        IRValidator validator(config_.platform);
        result.validationResult = validator.validate(app);

        if (!result.validationResult.isValid()) {
            log(result, "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x82\xd9\x82: " +
                        std::to_string(result.validationResult.errorCount()) +
                        " \xd8\xa3\xd8\xae\xd8\xb7\xd8\xa7\xd8\xa1"); // فشل التحقق: N أخطاء
            for (const auto& issue : result.validationResult.errors()) {
                result.codeGenResult.errors.push_back(issue.messageAR);
            }
            return result;
        }

        log(result, "\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x82\xd9\x82 \xd9\x86\xd8\xa7\xd8\xac\xd8\xad " +
                    std::to_string(result.validationResult.warningCount()) +
                    " \xd8\xaa\xd8\xad\xd8\xb0\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa)"); // التحقق ناجح (N تحذيرات)
    }

    // 4. التحسين
    if (config_.optimize) {
        log(result, "\xd8\xaa\xd8\xad\xd8\xb3\xd9\x8a\xd9\x86 IR..."); // تحسين IR

        IROptimizer optimizer;

        // تطبيق الثيمة
        if (config_.applyTheme) {
            for (auto& screen : app.screens) {
                if (screen.rootNode) {
                    optimizer.applyTheme(screen.rootNode, app.theme);
                }
            }
        }

        optimizer.optimize(app);
        result.optimizationStats = optimizer.getStats();

        log(result, "\xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xb3\xd9\x8a\xd9\x86: \xd8\xad\xd8\xb0\xd9\x81 " +
                    std::to_string(result.optimizationStats.removedNodes) +
                    " \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9, \xd8\xaf\xd9\x85\xd8\xac " +
                    std::to_string(result.optimizationStats.mergedStyles) +
                    " \xd9\x86\xd9\x85\xd8\xb7"); // تم التحسين: حذف N عقدة، دمج N نمط
    }

    // 5. توليد الكود
    log(result, "\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xa7\xd9\x84\xd9\x83\xd9\x88\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa3\xd8\xb5\xd9\x84\xd9\x8a..."); // توليد الكود الأصلي

    auto codeGen = createCodeGen();
    if (!codeGen) {
        log(result, "\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaf\xd8\xb9\xd9\x88\xd9\x85\xd8\xa9"); // منصة غير مدعومة
        result.codeGenResult.errors.push_back("Unsupported target platform");
        return result;
    }

    result.codeGenResult = codeGen->generate(app);

    if (result.codeGenResult.success) {
        log(result, "\xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x88\xd9\x84\xd9\x8a\xd8\xaf \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad: " +
                    std::to_string(result.codeGenResult.files.size()) +
                    " \xd9\x85\xd9\x84\xd9\x81"); // تم التوليد بنجاح: N ملف
    }

    result.success = result.codeGenResult.success;
    result.appIR = std::move(app);
    return result;
}


// ═══════════════════════════════════════════════════════════════════════════
// إنشاء مولد كود حسب المنصة
// ═══════════════════════════════════════════════════════════════════════════

std::unique_ptr<ICodeGenerator> SadUIPipeline::createCodeGen() {
    switch (config_.platform) {
        case TargetPlatform::Desktop:
            return std::make_unique<DesktopSDL2CodeGen>();

        case TargetPlatform::Android:
            return std::make_unique<AndroidComposeCodeGen>();

        case TargetPlatform::iOS:
            return std::make_unique<IOSSwiftUICodeGen>();

        case TargetPlatform::Web:
            return std::make_unique<WebHTMLCodeGen>();

        case TargetPlatform::All:
            // الافتراضي → Desktop
            return std::make_unique<DesktopSDL2CodeGen>();
    }
    return nullptr;
}


void SadUIPipeline::log(PipelineResult& result, const std::string& msg) {
    result.log.push_back(msg);
}


} // namespace ir
} // namespace ui
} // namespace sad
