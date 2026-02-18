// بسم الله الرحمن الرحيم
// Documentation Generator - Main Program
// البرنامج الرئيسي لمولد التوثيق
// Phase 7.4: Documentation Generator

#include "docgen.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace sad::docgen;
namespace fs = std::filesystem;

// ============================================================================
// Command Line Parser / محلل سطر الأوامر
// ============================================================================

struct Options {
    std::vector<std::string> input_dirs;
    std::string output_dir = "docs";
    OutputFormat format = OutputFormat::HTML;
    std::string project_name = "Sad Project";
    std::string project_name_ar = "مشروع Sad";
    std::string version = "1.0.0";
    bool verbose = false;
    bool help = false;
};

void print_usage(const char* program_name) {
    std::cout << R"(
بسم الله الرحمن الرحيم
Sad Documentation Generator / مولد التوثيق للغة Sad

Usage / الاستخدام:
    )" << program_name << R"( [options] <input_dirs...>

Options / الخيارات:
    -h, --help              Show this help message / عرض هذه الرسالة
    -o, --output <dir>      Output directory / مجلد الإخراج (default: docs)
    -f, --format <format>   Output format: html, markdown, json / صيغة الإخراج
                            (default: html)
    -n, --name <name>       Project name / اسم المشروع
    --name-ar <name>        Project name in Arabic / اسم المشروع بالعربية
    -v, --version <ver>     Project version / إصدار المشروع
    --verbose               Verbose output / إخراج مفصل

Examples / أمثلة:
    # Generate HTML docs from src/ directory
    )" << program_name << R"( src/

    # Generate Markdown with custom output
    )" << program_name << R"( -f markdown -o api_docs src/

    # Multiple directories with project info
    )" << program_name << R"( -n "My Project" --name-ar "مشروعي" -v 2.0.0 src/ lib/

الحمد لله - Documentation Generator v1.0.0
)";
}

Options parse_args(int argc, char* argv[]) {
    Options opts;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            opts.help = true;
            return opts;
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                opts.output_dir = argv[++i];
            }
        }
        else if (arg == "-f" || arg == "--format") {
            if (i + 1 < argc) {
                std::string fmt = argv[++i];
                if (fmt == "html") opts.format = OutputFormat::HTML;
                else if (fmt == "markdown" || fmt == "md") opts.format = OutputFormat::MARKDOWN;
                else if (fmt == "json") opts.format = OutputFormat::JSON;
                else {
                    std::cerr << "Unknown format: " << fmt << std::endl;
                }
            }
        }
        else if (arg == "-n" || arg == "--name") {
            if (i + 1 < argc) {
                opts.project_name = argv[++i];
            }
        }
        else if (arg == "--name-ar") {
            if (i + 1 < argc) {
                opts.project_name_ar = argv[++i];
            }
        }
        else if (arg == "-v" || arg == "--version") {
            if (i + 1 < argc) {
                opts.version = argv[++i];
            }
        }
        else if (arg == "--verbose") {
            opts.verbose = true;
        }
        else if (arg[0] != '-') {
            // Input directory
            opts.input_dirs.push_back(arg);
        }
    }
    
    return opts;
}

// ============================================================================
// الحمد لله - Main Function / الدالة الرئيسية
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "بسم الله الرحمن الرحيم" << std::endl;
    std::cout << "الحمد لله - Sad Documentation Generator" << std::endl;
    std::cout << std::endl;
    
    // Parse arguments
    Options opts = parse_args(argc, argv);
    
    if (opts.help) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (opts.input_dirs.empty()) {
        std::cerr << "Error: No input directories specified" << std::endl;
        std::cerr << "خطأ: لم يتم تحديد مجلدات الإدخال" << std::endl;
        print_usage(argv[0]);
        return 1;
    }
    
    // Create extractor and generator
    DocExtractor extractor;
    DocGenerator generator;
    
    // Configure generator
    DocGenerator::Settings settings;
    settings.project_name = opts.project_name;
    settings.project_name_ar = opts.project_name_ar;
    settings.version = opts.version;
    generator.set_settings(settings);
    
    // Extract documentation from all files
    std::cout << "📚 Extracting documentation... / استخراج التوثيق..." << std::endl;
    
    int file_count = 0;
    for (const auto& dir : opts.input_dirs) {
        if (!fs::exists(dir)) {
            std::cerr << "Warning: Directory not found: " << dir << std::endl;
            continue;
        }
        
        // Recursively process all .s files
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".ص" || entry.path().extension() == u8".ص")) {
                if (opts.verbose) {
                    std::cout << "  Processing: " << entry.path().string() << std::endl;
                }
                
                if (extractor.extract_from_file(entry.path().string())) {
                    ++file_count;
                }
            }
        }
    }
    
    const auto& comments = extractor.get_comments();
    
    std::cout << "✓ Extracted " << comments.size() << " documentation comments from " 
              << file_count << " files" << std::endl;
    std::cout << "✓ تم استخراج " << comments.size() << " تعليق توثيقي من " 
              << file_count << " ملف" << std::endl;
    std::cout << std::endl;
    
    if (comments.empty()) {
        std::cout << "⚠️ No documentation found / لم يتم العثور على توثيق" << std::endl;
        return 0;
    }
    
    // Generate documentation
    std::cout << "📝 Generating documentation... / توليد التوثيق..." << std::endl;
    
    if (!generator.generate(comments, opts.format, opts.output_dir)) {
        std::cerr << "✗ Failed to generate documentation / فشل توليد التوثيق" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Documentation generated successfully!" << std::endl;
    std::cout << "✓ تم توليد التوثيق بنجاح!" << std::endl;
    std::cout << "📁 Output: " << fs::absolute(opts.output_dir).string() << std::endl;
    std::cout << std::endl;
    
    // Print summary
    std::cout << "الحمد لله - Summary / ملخص:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    // Count by type
    std::map<DocType, int> type_counts;
    for (const auto& comment : comments) {
        type_counts[comment.type]++;
    }
    
    for (const auto& [type, count] : type_counts) {
        std::cout << "  " << doc_type_to_string_ar(type) 
                  << " (" << doc_type_to_string(type) << "): " 
                  << count << std::endl;
    }
    
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "الحمد لله رب العالمين! 🌟" << std::endl;
    
    return 0;
}
