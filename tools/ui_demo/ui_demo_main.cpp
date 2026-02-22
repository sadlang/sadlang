/**
 * ==========================================================================
 * ملف: ui_demo_main.cpp
 * الوصف: البرنامج الرئيسي للتطبيق التجريبي — ٨ واجهات مستخدم
 * ==========================================================================
 *
 *   بناء:  cmake --build build --config Debug --target sad_ui_demo
 *   تشغيل: .\build\bin\Debug\sad_ui_demo.exe [--platform=desktop|web|android|ios]
 *
 *   - بدون وسائط: يولّد كود سطح المكتب (SDL2/C++)
 *   - مع --web:    يولّد HTML/CSS/JS
 *   - مع --android: يولّد Kotlin/Compose
 *   - مع --ios:     يولّد Swift/SwiftUI
 */

#include "ui/ui_demo_app.hpp"
#include "ui/sad_ui_pipeline.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

static void printArabicBanner() {
    std::cout << "\n";
    std::cout << "  SadUI Demo App - 8 screens\n";
    std::cout << "\n";
}

static sad::ui::ir::TargetPlatform parsePlatform(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--web" || arg == "--\xd9\x88\xd9\x8a\xd8\xa8") return sad::ui::ir::TargetPlatform::Web;
        if (arg == "--android" || arg == "--\xd8\xa7\xd9\x86\xd8\xaf\xd8\xb1\xd9\x88\xd9\x8a\xd8\xaf") return sad::ui::ir::TargetPlatform::Android;
        if (arg == "--ios" || arg == "--\xd8\xa7\xd9\x8a\xd9\x81\xd9\x88\xd9\x86") return sad::ui::ir::TargetPlatform::iOS;
        if (arg == "--desktop" || arg == "--\xd8\xb3\xd8\xb7\xd8\xad") return sad::ui::ir::TargetPlatform::Desktop;
        if (arg == "--all" || arg == "--\xd8\xa7\xd9\x84\xd9\x83\xd9\x84") return sad::ui::ir::TargetPlatform::All;

        if (arg.find("--platform=") == 0) {
            auto val = arg.substr(11);
            if (val == "web") return sad::ui::ir::TargetPlatform::Web;
            if (val == "android") return sad::ui::ir::TargetPlatform::Android;
            if (val == "ios") return sad::ui::ir::TargetPlatform::iOS;
            if (val == "desktop") return sad::ui::ir::TargetPlatform::Desktop;
            if (val == "all") return sad::ui::ir::TargetPlatform::All;
        }
    }
    return sad::ui::ir::TargetPlatform::Desktop;
}

static bool hasFlag(int argc, char* argv[], const std::string& flag) {
    for (int i = 1; i < argc; ++i)
        if (std::string(argv[i]) == flag) return true;
    return false;
}

static std::string platformName(sad::ui::ir::TargetPlatform p) {
    switch (p) {
        case sad::ui::ir::TargetPlatform::Desktop: return "\xd8\xb3\xd8\xb7\xd8\xad \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8 (SDL2/C++)";
        case sad::ui::ir::TargetPlatform::Android: return "\xd8\xa3\xd9\x86\xd8\xaf\xd8\xb1\xd9\x88\xd9\x8a\xd8\xaf (Kotlin/Compose)";
        case sad::ui::ir::TargetPlatform::iOS:     return "iOS (SwiftUI)";
        case sad::ui::ir::TargetPlatform::Web:     return "\xd9\x88\xd9\x8a\xd8\xa8 (HTML/CSS/JS)";
        case sad::ui::ir::TargetPlatform::All:     return "\xd8\xac\xd9\x85\xd9\x8a\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa7\xd8\xaa";
    }
    return "unknown";
}

static bool writeFile(const fs::path& filePath, const std::string& content) {
    try {
        fs::create_directories(filePath.parent_path());
    } catch (...) {
        // ignore
    }
    std::ofstream out(filePath, std::ios::binary);
    if (!out) return false;
    out.write(content.data(), content.size());
    return out.good();
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Enable UTF-8 output on Windows console
    system("chcp 65001 > nul 2>&1");
#endif

    std::cout << "=== SadUI Demo Starting ===" << std::endl;
    std::cout.flush();

    printArabicBanner();

    bool verbose = hasFlag(argc, argv, "-v") || hasFlag(argc, argv, "--verbose");
    auto platform = parsePlatform(argc, argv);

    std::cout << "  Platform: " << platformName(platform) << std::endl;

    // ═══════════════════════════════════════════════════════════════════
    // 1. إنشاء التطبيق (٨ واجهات)
    // ═══════════════════════════════════════════════════════════════════
    std::cout << "  [1/4] Creating app (8 screens)..." << std::endl;
    
    sad::ui::ir::AppIR app;
    try {
        app = sad::ui::demo::createDemoApp();
        app.targetPlatform = platform;
        std::cout << "    OK: " << app.screens.size() << " screens created" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    EXCEPTION in createDemoApp: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "    UNKNOWN EXCEPTION in createDemoApp" << std::endl;
        return 1;
    }

    for (const auto& s : app.screens) {
        int nodeCount = s.rootNode ? s.rootNode->subtreeSize() : 0;
        std::cout << "      - " << s.name << " (" << nodeCount << " nodes)" << std::endl;
    }

    // ═══════════════════════════════════════════════════════════════════
    // 2. تشغيل خط الأنابيب
    // ═══════════════════════════════════════════════════════════════════
    std::cout << "\n  [2/4] Running pipeline (IR -> CodeGen)..." << std::endl;

    sad::ui::ir::SadUIPipeline pipeline;
    sad::ui::ir::PipelineConfig config;
    config.platform = platform;
    config.validate = true;
    config.optimize = true;
    config.applyTheme = true;
    config.verbose = verbose;
    config.theme = app.theme;
    config.outputDir = "build_demo";
    pipeline.setConfig(config);

    sad::ui::ir::PipelineResult result;
    try {
        result = pipeline.buildFromIR(app);
        std::cout << "    Pipeline finished, success=" << result.success << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    EXCEPTION in pipeline: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "    UNKNOWN EXCEPTION in pipeline" << std::endl;
        return 1;
    }

    if (!result.success) {
        std::cerr << "\n  \xe2\x9d\x8c \xd9\x81\xd8\xb4\xd9\x84 \xd8\xae\xd8\xb7 \xd8\xa7\xd9\x84\xd8\xa3\xd9\x86\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xa8!\n";

        // Validation errors
        auto valErrors = result.validationResult.errors();
        if (!valErrors.empty()) {
            std::cerr << "  \xd8\xa3\xd8\xae\xd8\xb7\xd8\xa7\xd8\xa1 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x82\xd9\x82:\n";
            for (const auto& err : valErrors) {
                std::cerr << "    - " << err.messageAR << "\n";
            }
        }

        if (!result.log.empty()) {
            std::cerr << "  \xd8\xa7\xd9\x84\xd8\xb3\xd8\xac\xd9\x84:\n";
            for (const auto& l : result.log) {
                std::cerr << "    " << l << "\n";
            }
        }
        return 1;
    }

    std::cout << "    Generated " << result.codeGenResult.files.size() << " files" << std::endl;

    // ═══════════════════════════════════════════════════════════════════
    // 3. Write files
    // ═══════════════════════════════════════════════════════════════════
    std::cout << "  [3/4] Writing files..." << std::endl;

    fs::path outputDir = "build_demo";
    int written = 0;
    try {
        for (const auto& file : result.codeGenResult.files) {
            // Use u8path for proper Unicode path support on Windows
            auto filePath = outputDir / fs::u8path(file.path);
            if (writeFile(filePath, file.content)) {
                ++written;
                std::cout << "    File: " << file.path
                          << " (" << file.content.size() << " bytes)" << std::endl;
            } else {
                std::cerr << "    FAILED to write: " << file.path << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "    EXCEPTION writing files: " << e.what() << std::endl;
    }

    // ═══════════════════════════════════════════════════════════════════
    // 4. Summary (use result.appIR since pipeline moved app)
    // ═══════════════════════════════════════════════════════════════════
    std::cout << "\n  [4/4] Summary" << std::endl;
    if (result.appIR.has_value()) {
        auto& finalApp = *result.appIR;
        std::cout << "  App:       " << finalApp.appName << std::endl;
        std::cout << "  Version:   " << finalApp.version << std::endl;
        std::cout << "  Screens:   " << finalApp.screens.size() << std::endl;
    }
    std::cout << "  Files:     " << written << std::endl;
    std::cout << "  Platform:  " << platformName(platform) << std::endl;
    std::cout << "  Output:    " << outputDir.string() << "/" << std::endl;

    // Optimization stats
    auto& optStats = result.optimizationStats;
    if (optStats.removedNodes > 0 || optStats.mergedStyles > 0) {
        std::cout << "\n  Optimization:" << std::endl;
        std::cout << "    Removed:   " << optStats.removedNodes << std::endl;
        std::cout << "    Merged:    " << optStats.mergedStyles << std::endl;
        std::cout << "    Flattened: " << optStats.flattenedGroups << std::endl;
    }

    if (verbose && !result.log.empty()) {
        std::cout << "\n  Log:" << std::endl;
        for (const auto& l : result.log) {
            std::cout << "    " << l << std::endl;
        }
    }

    std::cout << "\n=== Done ===" << std::endl;
    return 0;
}
