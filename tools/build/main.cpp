// بسم الله الرحمن الرحيم
/**
 * @file main.cpp
 * @brief Sad Build System CLI
 * 
 * Main entry point for the Sad build system command-line interface.
 * 
 * Usage:
 *   sad build [options]
 *   sad run [args]
 *   sad test
 *   sad task <name>
 *   sad clean
 *   sad watch
 */

#include "include/build_system.h"
#include <iostream>
#include <vector>
#include <string>

using namespace sad::build;

void printHelp() {
    std::cout << "Sad Build System - نظام بناء لغة ص\n\n";
    std::cout << "Usage: sad <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  build [options]     Build the project\n";
    std::cout << "    --release         Release build with optimizations\n";
    std::cout << "    --clean           Clean build (rebuild all)\n";
    std::cout << "    --verbose         Verbose output\n";
    std::cout << "    --target <arch>   Target architecture (x86_64, arm64, wasm)\n";
    std::cout << "\n";
    std::cout << "  run [-- args]       Run the project\n";
    std::cout << "    --watch           Watch mode (auto-rebuild)\n";
    std::cout << "\n";
    std::cout << "  test [options]      Run tests\n";
    std::cout << "    --verbose         Verbose test output\n";
    std::cout << "    --coverage        Generate coverage report\n";
    std::cout << "\n";
    std::cout << "  task <name>         Run custom task\n";
    std::cout << "  tasks               List all tasks\n";
    std::cout << "\n";
    std::cout << "  clean               Clean build artifacts\n";
    std::cout << "    --all             Also clean cache\n";
    std::cout << "\n";
    std::cout << "  watch [paths]       Watch for changes\n";
    std::cout << "\n";
    std::cout << "  help                Show this help\n";
    std::cout << "  version             Show version\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  sad build\n";
    std::cout << "  sad build --release\n";
    std::cout << "  sad run -- --help\n";
    std::cout << "  sad watch\n";
    std::cout << "  sad task deploy\n";
}

void printVersion() {
    std::cout << "Sad Build System v" << getBuildSystemVersion() << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::string command = argv[1];

    try {
        // Initialize build system
        BuildSystem bs("sad.toml");

        if (command == "build") {
            BuildOptions opts;

            // Parse options
            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];
                if (arg == "--release") {
                    opts.release = true;
                } else if (arg == "--clean") {
                    opts.clean = true;
                } else if (arg == "--verbose") {
                    opts.verbose = true;
                } else if (arg == "--target" && i + 1 < argc) {
                    opts.target = argv[++i];
                }
            }

            printBanner();
            BuildResult result = bs.build(opts);
            return result.success ? 0 : 1;

        } else if (command == "run") {
            RunOptions opts;

            // Parse options
            bool found_separator = false;
            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];
                if (arg == "--watch") {
                    opts.watch = true;
                } else if (arg == "--") {
                    found_separator = true;
                } else if (found_separator) {
                    opts.args.push_back(arg);
                }
            }

            return bs.run(opts);

        } else if (command == "test") {
            TestOptions opts;

            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];
                if (arg == "--verbose") {
                    opts.verbose = true;
                } else if (arg == "--coverage") {
                    opts.coverage = true;
                }
            }

            BuildResult result = bs.test(opts);
            return result.success ? 0 : 1;

        } else if (command == "task") {
            if (argc < 3) {
                std::cerr << "Error: Task name required\n";
                std::cerr << "Usage: sad task <name>\n";
                return 1;
            }

            std::string task_name = argv[2];
            return bs.runTask(task_name);

        } else if (command == "tasks") {
            std::vector<std::string> tasks = bs.listTasks();
            
            if (tasks.empty()) {
                std::cout << "No tasks defined in sad.toml\n";
            } else {
                std::cout << "Available tasks:\n";
                for (const auto& task : tasks) {
                    std::cout << "  " << task << "\n";
                }
            }
            return 0;

        } else if (command == "clean") {
            bool all = false;
            for (int i = 2; i < argc; i++) {
                if (std::string(argv[i]) == "--all") {
                    all = true;
                }
            }

            return bs.clean(all) ? 0 : 1;

        } else if (command == "watch") {
            std::vector<std::string> paths;
            for (int i = 2; i < argc; i++) {
                paths.push_back(argv[i]);
            }

            bs.watch(paths);
            return 0;

        } else if (command == "help" || command == "--help" || command == "-h") {
            printHelp();
            return 0;

        } else if (command == "version" || command == "--version" || command == "-v") {
            printVersion();
            return 0;

        } else {
            std::cerr << "Unknown command: " << command << "\n";
            std::cerr << "Run 'sad help' for usage information\n";
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
