// wizard.cpp — معالج تفاعلي لأداة المُولّد (sadc-create)
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct ProjectConfig {
    std::string name;
    std::string template_type; // "console", "webapp", "mobile", "library", "game", "embedded"
    std::string version = "0.1.0";
    std::string author;
    std::string description;
    bool init_git = true;
};

class Wizard {
public:
    ProjectConfig run_interactive() {
        ProjectConfig config;
        std::cout << "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\xD9\x8B \xD8\xA8\xD9\x85\xD9\x88\xD9\x84\xD9\x91\xD8\xAF \xD9\x85\xD8\xB4\xD8\xA7\xD8\xB1\xD9\x8A\xD8\xB9 \xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB5!\n\n"; // مرحباً بمولّد مشاريع لغة ص

        config.name = ask("\xD8\xA7\xD8\xB3\xD9\x85 \xD8\xA7\xD9\x84\xD9\x85\xD8\xB4\xD8\xB1\xD9\x88\xD8\xB9: "); // اسم المشروع
        config.template_type = choose_template();
        config.author = ask("\xD8\xA7\xD8\xB3\xD9\x85 \xD8\xA7\xD9\x84\xD9\x85\xD8\xA4\xD9\x84\xD9\x81: "); // اسم المؤلف
        config.description = ask("\xD9\x88\xD8\xB5\xD9\x81 \xD9\x85\xD8\xAE\xD8\xAA\xD8\xB5\xD8\xB1: "); // وصف مختصر

        return config;
    }

    bool create_project(const ProjectConfig& config) {
        const std::string base = config.name;
        try {
            fs::create_directories(base + "/src");
            fs::create_directories(base + "/tests");

            // Create project file
            std::ofstream proj(base + "/\xD9\x85\xD8\xB4\xD8\xB1\xD9\x88\xD8\xB9.\xD8\xB5"); // مشروع.ص
            proj << "[" << "\xD9\x85\xD8\xB4\xD8\xB1\xD9\x88\xD8\xB9" << "]\n"; // مشروع
            proj << "\xD8\xA7\xD8\xB3\xD9\x85 = \"" << config.name << "\"\n"; // اسم
            proj << "\xD9\x86\xD8\xB3\xD8\xAE\xD8\xA9 = \"" << config.version << "\"\n"; // نسخة
            proj << "\xD9\x85\xD8\xA4\xD9\x84\xD9\x81 = \"" << config.author << "\"\n"; // مؤلف
            proj << "\xD9\x88\xD8\xB5\xD9\x81 = \"" << config.description << "\"\n"; // وصف
            proj.close();

            // Create main file
            std::ofstream main_file(base + "/src/\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A.\xD8\xB5"); // رئيسي.ص
            main_file << "// " << config.name << "\n\n";
            main_file << "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9() {\n"; // دالة رئيسية
            main_file << "    \xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9(\"" << "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\xD9\x8B" << " " << config.name << "!\")\n"; // اطبع("مرحباً
            main_file << "}\n";
            main_file.close();

            if (config.init_git) {
                auto cmd = "cd " + base + " && git init";
                system(cmd.c_str());
            }

            std::cout << "\n\xE2\x9C\x93 " << "\xD8\xAA\xD9\x85 \xD8\xA5\xD9\x86\xD8\xB4\xD8\xA7\xD8\xA1 \xD8\xA7\xD9\x84\xD9\x85\xD8\xB4\xD8\xB1\xD9\x88\xD8\xB9 \xD8\xA8\xD9\x86\xD8\xAC\xD8\xA7\xD8\xAD!\n"; // تم إنشاء المشروع بنجاح
            return true;
        } catch (const std::exception& e) {
            std::cerr << "\xD8\xAE\xD8\xB7\xD8\xA3: " << e.what() << "\n"; // خطأ
            return false;
        }
    }

private:
    std::string ask(const std::string& prompt) {
        std::cout << prompt;
        std::string answer;
        std::getline(std::cin, answer);
        return answer;
    }

    std::string choose_template() {
        std::cout << "\n\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xB1 \xD9\x82\xD8\xA7\xD9\x84\xD8\xA8:\n"; // اختر قالب
        std::cout << "  1. \xD8\xAA\xD8\xB7\xD8\xA8\xD9\x8A\xD9\x82 \xD8\xB7\xD8\xB1\xD9\x81\xD9\x8A\xD8\xA9\n"; // 1. تطبيق طرفية
        std::cout << "  2. \xD8\xAA\xD8\xB7\xD8\xA8\xD9\x8A\xD9\x82 \xD9\x88\xD9\x8A\xD8\xA8\n"; // 2. تطبيق ويب
        std::cout << "  3. \xD8\xAA\xD8\xB7\xD8\xA8\xD9\x8A\xD9\x82 \xD9\x85\xD9\x88\xD8\xA8\xD8\xA7\xD9\x8A\xD9\x84\n"; // 3. تطبيق موبايل
        std::cout << "  4. \xD9\x85\xD9\x83\xD8\xAA\xD8\xA8\xD8\xA9\n"; // 4. مكتبة
        std::cout << "  5. \xD9\x84\xD8\xB9\xD8\xA8\xD8\xA9\n"; // 5. لعبة
        std::cout << "  6. \xD9\x86\xD8\xB8\xD8\xA7\xD9\x85 \xD9\x85\xD8\xB6\xD9\x85\xD9\x91\xD9\x86\n"; // 6. نظام مضمّن
        std::cout << "\xD8\xA7\xD8\xAE\xD8\xAA\xD9\x8A\xD8\xA7\xD8\xB1\xD9\x83 (1-6): "; // اختيارك

        std::string choice;
        std::getline(std::cin, choice);
        const std::vector<std::string> templates = {"console", "webapp", "mobile", "library", "game", "embedded"};
        int idx = std::stoi(choice) - 1;
        return (idx >= 0 && idx < (int)templates.size()) ? templates[idx] : "console";
    }
};

int main(int argc, char* argv[]) {
    Wizard wizard;
    if (argc > 1 && std::string(argv[1]) == "--non-interactive") {
        // Non-interactive mode with args
        ProjectConfig config;
        config.name = argc > 2 ? argv[2] : "my_project";
        config.template_type = argc > 3 ? argv[3] : "console";
        config.author = argc > 4 ? argv[4] : "developer";
        wizard.create_project(config);
    } else {
        auto config = wizard.run_interactive();
        wizard.create_project(config);
    }
    return 0;
}
