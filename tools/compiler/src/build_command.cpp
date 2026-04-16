/**
 * =============================================================================
 * ملف: build_command.cpp
 * الوصف: تنفيذ أمر البناء (build) في CLI
 * =============================================================================
 * (AR) يستدعي المترجم sadc كعملية فرعية لترجمة ملفات .ص إلى ملفات تنفيذية
 * (EN) Invokes sadc compiler as subprocess to compile .ص files to executables
 * =============================================================================
 */

#include "cli_commands.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace sad {
namespace cli {

// ============================================================================
// (AR) البحث عن sadc في مسارات معروفة
// (EN) Find sadc in known paths
// ============================================================================
static std::string find_sadc() {
    // (AR) 1. بجانب الملف التنفيذي الحالي (sad.exe)
    // (EN) 1. Next to current executable (sad.exe)
#ifdef _WIN32
    char exe_path[MAX_PATH] = {0};
    if (GetModuleFileNameA(nullptr, exe_path, MAX_PATH) > 0) {
        fs::path exe_dir = fs::path(exe_path).parent_path();
        fs::path sadc_path = exe_dir / "sadc.exe";
        if (fs::exists(sadc_path)) return sadc_path.string();
        
        // (AR) 2. في المجلدات الشقيقة (Debug/Release)
        // (EN) 2. In sibling directories (Debug/Release)
        fs::path parent = exe_dir.parent_path();
        for (const auto& sibling : {"Release", "Debug", "RelWithDebInfo", "MinSizeRel"}) {
            fs::path sib_path = parent / sibling / "sadc.exe";
            if (fs::exists(sib_path)) return sib_path.string();
        }
    }
#endif

    // (AR) 3. في متغير PATH
    // (EN) 3. In system PATH
#ifdef _WIN32
    // (AR) محاولة تشغيل sadc --version للتحقق من وجوده في PATH
    if (std::system("where sadc.exe >nul 2>nul") == 0) {
        return "sadc.exe";
    }
#else
    if (std::system("which sadc >/dev/null 2>/dev/null") == 0) {
        return "sadc";
    }
#endif

    return "";
}

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
        if (!fs::exists(file)) {
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
        fs::path input_path(options.positional_args[0]);
        output_file = input_path.stem().string();
#ifdef _WIN32
        output_file += ".exe";
#endif
    }
    
    // (AR) البحث عن sadc
    // (EN) Find sadc compiler
    std::string sadc_path = find_sadc();
    if (sadc_path.empty()) {
        print_error_ar("لم يُعثر على المترجم sadc. تأكد من بنائه أو وجوده في PATH.");
        print_info_ar("لبناء sadc: cmake --build build --config Release --target sadc");
        return 1;
    }
    
    // (AR) بناء أمر sadc
    // (EN) Build sadc command line
    std::ostringstream cmd;
    
    // (AR) تغليف المسار بعلامات اقتباس لدعم المسافات والأحرف العربية
    // (EN) Quote path for spaces and Arabic characters
    cmd << "\"" << sadc_path << "\"";
    
    // (AR) إضافة ملفات الإدخال
    // (EN) Add input files
    for (const auto& file : options.positional_args) {
        cmd << " \"" << file << "\"";
    }
    
    // (AR) ملف الإخراج
    // (EN) Output file
    cmd << " -o \"" << output_file << "\"";
    
    // (AR) مستوى التحسين
    // (EN) Optimization level
    if (options.has("opt")) {
        std::string opt = options.get("opt");
        cmd << " -O" << opt;
    } else if (options.has("تحسين")) {
        std::string opt = options.get("تحسين");
        cmd << " -O" << opt;
    }
    
    // (AR) نوع الإخراج
    // (EN) Output type
    if (options.has("emit") || options.has("أصدر")) {
        std::string emit = options.has("emit") ? options.get("emit") : options.get("أصدر");
        if (emit == "llvm-ir" || emit == "llvm") {
            cmd << " --emit-llvm";
        } else if (emit == "asm" || emit == "تجميع") {
            cmd << " -S";
        } else if (emit == "obj" || emit == "كائن") {
            cmd << " -c";
        }
    }
    
    // (AR) معلومات التنقيح
    // (EN) Debug info
    if (options.has("debug") || options.has("تنقيح")) {
        cmd << " -g";
    }
    
    // (AR) الربط الثابت
    // (EN) Static linking
    if (options.has("static") || options.has("ثابت")) {
        cmd << " --static";
    }
    
    // (AR) المنصة الهدف
    // (EN) Target triple
    if (options.has("target")) {
        cmd << " --target=" << options.get("target");
    } else if (options.has("هدف")) {
        cmd << " --target=" << options.get("هدف");
    }
    
    std::string full_cmd = cmd.str();
    print_info_ar("🔨 جاري البناء: " + options.positional_args[0] + " → " + output_file);
    
    // (AR) تنفيذ المترجم
    // (EN) Execute compiler
    // (AR) على Windows، cmd.exe يحذف الاقتباسات الخارجية عند استخدام std::system
    //      لذا نحيط الأمر كاملاً باقتباسات إضافية
    // (EN) On Windows, cmd.exe strips outer quotes from std::system commands
    //      so we wrap the entire command in an extra set of quotes
#ifdef _WIN32
    full_cmd = "\"" + full_cmd + "\"";
#endif
    int exit_code = std::system(full_cmd.c_str());
    
    if (exit_code == 0) {
        print_info_ar("✅ تم البناء بنجاح: " + output_file);
    } else {
        print_error_ar("فشل البناء (كود الخروج: " + std::to_string(exit_code) + ")");
    }
    
    return exit_code;
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
