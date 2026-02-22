/**
 * =============================================================================
 * ملف: build_command.cpp
 * الوصف: تنفيذ أمر البناء (build) في CLI
 * =============================================================================
 * (AR) يوفر التنفيذ الأساسي لأمر البناء الذي يُترجم ملفات .ص إلى ملفات تنفيذية
 * (EN) Provides basic implementation of the build command that compiles .ص files
 * =============================================================================
 */

#include "cli_commands.hpp"
#include <iostream>
#include <filesystem>

namespace sad {
namespace cli {

int BuildCommand::execute(const ParsedOptions& options) {
    // (AR) فحص وجود ملفات مدخلة
    // (EN) Check for input files
    if (options.positional_args.empty()) {
        print_error_ar("يجب تحديد ملف مصدري واحد على الأقل.");
        print_info_ar("استخدم: ص بناء <ملف.ص> [-o ملف_تنفيذي]");
        return 1;
    }
    
    // (AR) التحقق من وجود الملفات
    // (EN) Verify files exist
    for (const auto& file : options.positional_args) {
        if (!std::filesystem::exists(file)) {
            print_error_ar("الملف غير موجود: " + file);
            return 1;
        }
    }
    
    // (AR) تحديد ملف الإخراج
    // (EN) Determine output file
    std::string output_file;
    if (options.has("output")) {
        output_file = options.get("output");
    } else if (options.has("o")) {
        output_file = options.get("o");
    } else {
        // (AR) اسم افتراضي استناداً للملف المدخل
        // (EN) Default name based on input file
        std::filesystem::path input_path(options.positional_args[0]);
        output_file = input_path.stem().string();
#ifdef _WIN32
        output_file += ".exe";
#endif
    }
    
    print_info_ar("🔨 جاري البناء: " + options.positional_args[0] + " → " + output_file);
    
    // (AR) حالياً، أمر البناء يتطلب تفعيل LLVM backend
    // (EN) Currently, build command requires LLVM backend
#ifdef HAS_LLVM
    // TODO: استدعاء مسار الترجمة الكامل: Parse → SIR → LLVM IR → Object → Link
    print_info_ar("⚠️  مسار البناء الكامل قيد التطوير. استخدم sadc مباشرة.");
    print_info_ar("   sadc " + options.positional_args[0] + " -o " + output_file);
    return 0;
#else
    print_error_ar("أمر البناء يتطلب تفعيل LLVM backend.");
    print_info_ar("أعد البناء مع: -DENABLE_LLVM_BACKEND=ON");
    return 1;
#endif
}

std::vector<CommandOption> BuildCommand::get_options() const {
    return {
        {"-o", "--output", "--إخراج", "Output file name", "اسم ملف الإخراج", true, "", false},
        {"", "--opt", "--تحسين", "Optimization level (0-3, s, z)", "مستوى التحسين", true, "0", false},
        {"", "--emit", "--أصدر", "Output type (exe, lib, obj, llvm-ir, asm)", "نوع الإخراج", true, "exe", false},
        {"", "--target", "--هدف", "Target triple", "المنصة الهدف", true, "", false},
        {"", "--debug", "--تنقيح", "Include debug info", "تضمين معلومات التنقيح", false, "", true},
        {"", "--static", "--ثابت", "Static linking", "الربط الثابت", false, "", true},
    };
}

void BuildCommand::print_examples(std::ostream& os) const {
    os << "أمثلة:\n";
    os << "  ص بناء برنامج.ص                 # بناء ملف تنفيذي\n";
    os << "  ص بناء برنامج.ص -o app          # تحديد اسم الإخراج\n";
    os << "  ص بناء --تحسين 2 برنامج.ص       # مع تحسين المستوى 2\n";
    os << "  ص بناء --أصدر llvm-ir برنامج.ص  # إخراج LLVM IR\n";
    os << "\n";
    os << "Examples:\n";
    os << "  sad build program.ص             # Build executable\n";
    os << "  sad build program.ص -o app      # Specify output name\n";
    os << "  sad build --opt 2 program.ص     # With optimization level 2\n";
}

} // namespace cli
} // namespace sad
