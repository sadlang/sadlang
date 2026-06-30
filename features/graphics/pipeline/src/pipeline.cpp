/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: pipeline.cpp
 * المسار: features/graphics/pipeline/src/pipeline.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ خط أنابيب واجهات ص.
 *
 * هذا الملف يربط جميع مراحل المعالجة ببعضها:
 * 1. بناء التمثيل الوسيط (IR) من شجرة UINode
 * 2. تطبيق الأنماط والتوجيه الذكي
 * 3. توليد كود المنصة المستهدفة
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/pipeline.h"
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

#include <iostream>
#include <sstream>

namespace sad {
namespace ui {

// ═══════════════════════════════════════════════════════════════════════════════
// PipelineResult
// ═══════════════════════════════════════════════════════════════════════════════

std::string PipelineResult::formatErrors() const {
    if (errors.empty()) return "";

    std::ostringstream out;
    out << "═══ أخطاء واجهات ص (" << errors.size() << ") ═══\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        out << "  " << (i + 1) << ". " << errors[i] << "\n";
    }
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// UIPipeline — الإنشاء والإعدادات
// ═══════════════════════════════════════════════════════════════════════════════

UIPipeline::UIPipeline()
    : options_()
    , irBuilder_(IRBuildOptions{
        options_.target,
        options_.optimize,
        options_.includeDebugInfo,
        options_.enableSmartRouting
      })
{
}

UIPipeline::UIPipeline(const PipelineOptions& options)
    : options_(options)
    , irBuilder_(IRBuildOptions{
        options.target,
        options.optimize,
        options.includeDebugInfo,
        options.enableSmartRouting
      })
{
}

void UIPipeline::setTarget(TargetPlatform target) {
    options_.target = target;

    // تحديث IRBuilder بالمنصة الجديدة
    IRBuildOptions irOpts{
        target,
        options_.optimize,
        options_.includeDebugInfo,
        options_.enableSmartRouting
    };
    irBuilder_ = IRBuilder(irOpts);
}

void UIPipeline::setOptions(const PipelineOptions& options) {
    options_ = options;

    IRBuildOptions irOpts{
        options.target,
        options.optimize,
        options.includeDebugInfo,
        options.enableSmartRouting
    };
    irBuilder_ = IRBuilder(irOpts);
}

const PipelineOptions& UIPipeline::getOptions() const {
    return options_;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المعالجة الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

PipelineResult UIPipeline::processTree(const ViewDefinition& viewDef) {
    PipelineResult result;

    // إذا كان الوضع رسم مباشر — استخدم المسار الجديد
    if (options_.renderMode == RenderMode::DirectRender) {
        return renderDirectly(viewDef);
    }

    // المرحلة 1: التحقق من الصحة
    auto validationErrors = validate(viewDef);
    if (!validationErrors.empty()) {
        result.errors = std::move(validationErrors);
        return result;
    }

    // المرحلة 2: بناء التمثيل الوسيط
    auto irModule = irBuilder_.build(viewDef);
    result.irModule = irModule;

    if (!irModule.root) {
        result.errors.push_back("فشل بناء التمثيل الوسيط: لا توجد عقدة جذر");
        return result;
    }

    // المرحلة 3: توليد كود المنصة المستهدفة
    result.generatedCode = generateCode(irModule);

    if (result.generatedCode.empty()) {
        result.errors.push_back("فشل توليد الكود للمنصة المستهدفة");
        return result;
    }

    result.success = true;
    return result;
}

PipelineResult UIPipeline::processNode(
    std::shared_ptr<UINode> root,
    const std::string& name
) {
    // تحويل عقدة واحدة إلى ViewDefinition
    ViewDefinition viewDef;
    viewDef.name = name.empty() ? "عرض" : name;
    viewDef.rootNode = std::move(root);

    return processTree(viewDef);
}

// ═══════════════════════════════════════════════════════════════════════════════
// التحقق والتصحيح
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<std::string> UIPipeline::validate(const ViewDefinition& viewDef) const {
    std::vector<std::string> errors;

    if (!viewDef.rootNode) {
        errors.push_back("العرض '" + viewDef.name + "' لا يحتوي على عقدة جذر");
    }

    if (viewDef.name.empty()) {
        errors.push_back("اسم العرض فارغ");
    }

    return errors;
}

void UIPipeline::dumpIR(const IRModule& module) const {
    std::cout << "═══ التمثيل الوسيط: " << module.name << " ═══\n";

    if (module.root) {
        module.root->dump();
    } else {
        std::cout << "  (فارغ)\n";
    }

    if (!module.stateDefinitions.empty()) {
        std::cout << "\n── متغيرات الحالة ──\n";
        for (const auto& [name, type] : module.stateDefinitions) {
            std::cout << "  متغير " << name << " : " << type << "\n";
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// توليد الكود لكل منصة
// ═══════════════════════════════════════════════════════════════════════════════

std::string UIPipeline::generateCode(const IRModule& module) {
    switch (options_.target) {
        case TargetPlatform::Android:
            return generateCompose(module);

        case TargetPlatform::IOS:
            return generateSwiftUI(module);

        case TargetPlatform::Web:
            return generateHtml(module);

        case TargetPlatform::MacOS:
            return generateAppKit(module);

        case TargetPlatform::Desktop:
        case TargetPlatform::WASM:
        case TargetPlatform::All:
        default:
            // Desktop و WASM يستخدمان SDL2 مباشرة — لا يحتاجان توليد كود
            return "// Desktop/WASM: يُعرض مباشرة عبر SDL2+OpenGL\n";
    }
}

std::string UIPipeline::generateCompose(const IRModule& module) {
#ifndef SAD_NO_COMPOSE_CODEGEN
    android::ComposeCodegenOptions opts;
    opts.packageName = options_.packageName;

    android::ComposeCodegen codegen(opts);
    return codegen.generate(module);
#else
    return "// Compose codegen not available\n";
#endif
}

std::string UIPipeline::generateSwiftUI(const IRModule& module) {
#ifndef SAD_NO_SWIFTUI_CODEGEN
    ios::SwiftUICodegenOptions opts;
    opts.moduleName = options_.moduleName;

    ios::SwiftUICodegen codegen(opts);
    return codegen.generate(module);
#else
    return "// SwiftUI codegen not available\n";
#endif
}

std::string UIPipeline::generateHtml(const IRModule& module) {
    web::HtmlCodegenOptions opts;
    opts.title = options_.pageTitle;

    web::HtmlCodegen codegen(opts);
    return codegen.generate(module);
}

std::string UIPipeline::generateAppKit(const IRModule& module) {
#ifndef SAD_NO_APPKIT_CODEGEN
    macos::AppKitCodegenOptions opts;
    opts.moduleName = options_.moduleName;

    macos::AppKitCodegen codegen(opts);
    return codegen.generate(module);
#else
    return "// AppKit codegen not available\n";
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// الرسم المباشر (Direct Rendering)
// ═══════════════════════════════════════════════════════════════════════════════

PipelineResult UIPipeline::renderDirectly(const ViewDefinition& viewDef) {
    PipelineResult result;

    // التحقق من الصحة
    auto validationErrors = validate(viewDef);
    if (!validationErrors.empty()) {
        result.errors = std::move(validationErrors);
        return result;
    }

    // بناء التمثيل الوسيط
    auto irModule = irBuilder_.build(viewDef);
    result.irModule = irModule;

    if (!irModule.root) {
        result.errors.push_back(
            "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 "
            "\xd8\xa7\xd9\x84\xd8\xaa\xd9\x85\xd8\xab\xd9\x8a\xd9\x84 "
            "\xd8\xa7\xd9\x84\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7");
        return result;
    }

    return renderIR(irModule.root);
}

PipelineResult UIPipeline::renderDirectly(
    std::shared_ptr<UINode> root,
    const std::string& name
) {
    ViewDefinition viewDef;
    viewDef.name = name.empty() ? "\xd8\xb9\xd8\xb1\xd8\xb6" : name;
    viewDef.rootNode = std::move(root);
    return renderDirectly(viewDef);
}

PipelineResult UIPipeline::renderIR(std::shared_ptr<IRNode> irRoot) {
    PipelineResult result;

    if (!irRoot) {
        result.errors.push_back(
            "\xd9\x84\xd8\xa7 \xd8\xaa\xd9\x88\xd8\xac\xd8\xaf "
            "\xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 IR \xd9\x84\xd9\x84\xd8\xb1\xd8\xb3\xd9\x85");
        return result;
    }

    // الكشف عن المنصة أو استخدام المحددة
    TargetPlatform platform = options_.target;
    if (platform == TargetPlatform::All) {
        platform = PlatformFactory::detectPlatform();
    }

    // إنشاء نافذة المنصة
    window_ = PlatformFactory::createWindow(platform);
    if (!window_) {
        if (platform == TargetPlatform::Desktop) {
            // Desktop يستخدم نافذة SDL2 الموجودة
            result.generatedCode = "// Desktop: SDL2 window";
            result.success = true;
            return result;
        }
        result.errors.push_back(
            "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa5\xd9\x86\xd8\xb4\xd8\xa7\xd8\xa1 "
            "\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9 \xd9\x84\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9");
        return result;
    }

    // إعداد النافذة
    if (!stateStore_) {
        stateStore_ = std::make_shared<StateStore>();
    }

    window_->setStateStore(stateStore_);
    window_->setContent(irRoot);

    if (!window_->create(options_.windowOptions)) {
        result.errors.push_back(
            "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa5\xd9\x86\xd8\xb4\xd8\xa7\xd8\xa1 "
            "\xd8\xa7\xd9\x84\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9");
        return result;
    }

    // حلقة الأحداث — تحجب حتى الإغلاق
    window_->run();

    result.success = true;
    return result;
}

} // namespace ui
} // namespace sad
