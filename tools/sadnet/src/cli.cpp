// ===================================================================
// صNet CLI - أداة سطر الأوامر لشبكة صد اللامركزية
// المرحلة 53: واجهة المستخدم النصية لإدارة العُقدة
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <functional>
#include <sstream>

namespace sad::network {

/// أمر CLI مُسجّل
struct CLICommand {
    std::string name;                           // اسم الأمر
    std::string description;                    // وصف بالعربية
    std::string usage;                          // طريقة الاستخدام
    std::function<int(const std::vector<std::string>&)> handler; // المُعالج
};

/// محلّل وسائط سطر الأوامر
class ArgumentParser {
    std::map<std::string, std::string> flags_;     // الأعلام: --flag value
    std::vector<std::string> positional_;          // الوسائط الموقعية

public:
    /// تحليل وسائط سطر الأوامر
    void parse(int argc, const char* argv[]) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg.starts_with("--")) {
                std::string key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    flags_[key] = argv[++i];
                } else {
                    flags_[key] = "true";
                }
            } else if (arg.starts_with("-")) {
                std::string key = arg.substr(1);
                if (i + 1 < argc) {
                    flags_[key] = argv[++i];
                }
            } else {
                positional_.push_back(arg);
            }
        }
    }

    std::string get_flag(const std::string& key,
                          const std::string& default_val = "") const {
        auto it = flags_.find(key);
        return (it != flags_.end()) ? it->second : default_val;
    }

    bool has_flag(const std::string& key) const {
        return flags_.count(key) > 0;
    }

    const std::vector<std::string>& positional() const { return positional_; }
};

/// تطبيق CLI لشبكة صد
class SadNetCLI {
    std::map<std::string, CLICommand> commands_;   // الأوامر المُسجّلة
    std::string version_;                          // إصدار الأداة
    bool verbose_;                                 // وضع التفصيل

public:
    SadNetCLI() : version_("0.53.0"), verbose_(false) {
        register_commands();
    }

    /// تنفيذ أمر من سطر الأوامر
    int run(int argc, const char* argv[]) {
        if (argc < 2) {
            print_help();
            return 0;
        }

        std::string cmd_name = argv[1];
        if (cmd_name == "--help" || cmd_name == "-h") {
            print_help();
            return 0;
        }
        if (cmd_name == "--version" || cmd_name == "-v") {
            std::cout << "صNet v" << version_ << "\n";
            return 0;
        }

        auto it = commands_.find(cmd_name);
        if (it == commands_.end()) {
            std::cerr << "خطأ: أمر غير معروف '" << cmd_name << "'\n";
            return 1;
        }

        std::vector<std::string> args;
        for (int i = 2; i < argc; i++) args.emplace_back(argv[i]);
        return it->second.handler(args);
    }

private:
    /// تسجيل جميع الأوامر المدعومة
    void register_commands() {
        commands_["init"] = {"init", "تهيئة عُقدة صNet جديدة",
            "sadnet init [--port PORT]",
            [this](const auto& args) { return cmd_init(args); }};

        commands_["join"] = {"join", "الانضمام لشبكة صد",
            "sadnet join <عنوان_البذرة>",
            [this](const auto& args) { return cmd_join(args); }};

        commands_["peers"] = {"peers", "عرض الأقران المتصلين",
            "sadnet peers [--verbose]",
            [this](const auto& args) { return cmd_peers(args); }};

        commands_["store"] = {"store", "تخزين ملف في الشبكة",
            "sadnet store <مسار_الملف>",
            [this](const auto& args) { return cmd_store(args); }};

        commands_["fetch"] = {"fetch", "جلب محتوى من الشبكة",
            "sadnet fetch <معرّف_المحتوى>",
            [this](const auto& args) { return cmd_fetch(args); }};

        commands_["status"] = {"status", "حالة العُقدة الحالية",
            "sadnet status",
            [this](const auto& args) { return cmd_status(args); }};
    }

    /// طباعة المساعدة
    void print_help() {
        std::cout << "╔══════════════════════════════════╗\n";
        std::cout << "║      صNet - الشبكة اللامركزية    ║\n";
        std::cout << "╚══════════════════════════════════╝\n\n";
        std::cout << "الأوامر المتاحة:\n";
        for (const auto& [name, cmd] : commands_) {
            std::cout << "  " << name << "\t" << cmd.description << "\n";
        }
    }

    int cmd_init(const std::vector<std::string>&) {
        std::cout << "جارٍ تهيئة عُقدة صNet...\n";
        std::cout << "تم إنشاء مفاتيح الهوية.\n";
        return 0;
    }

    int cmd_join(const std::vector<std::string>& args) {
        if (args.empty()) { std::cerr << "أدخل عنوان البذرة\n"; return 1; }
        std::cout << "جارٍ الانضمام عبر: " << args[0] << "\n";
        return 0;
    }

    int cmd_peers(const std::vector<std::string>&) {
        std::cout << "الأقران المتصلون: 0\n";
        return 0;
    }

    int cmd_store(const std::vector<std::string>& args) {
        if (args.empty()) { std::cerr << "أدخل مسار الملف\n"; return 1; }
        std::cout << "جارٍ تخزين: " << args[0] << "\n";
        return 0;
    }

    int cmd_fetch(const std::vector<std::string>& args) {
        if (args.empty()) { std::cerr << "أدخل معرّف المحتوى\n"; return 1; }
        std::cout << "جارٍ جلب: " << args[0] << "\n";
        return 0;
    }

    int cmd_status(const std::vector<std::string>&) {
        std::cout << "حالة العُقدة: نشطة\n";
        std::cout << "الإصدار: " << version_ << "\n";
        return 0;
    }
};

} // namespace sad::network

/// نقطة الدخول
int main(int argc, const char* argv[]) {
    sad::network::SadNetCLI cli;
    return cli.run(argc, argv);
}
