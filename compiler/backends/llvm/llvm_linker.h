/*
 * الرابط LLVM / LLVM Linker
 * ==========================
 * 
 * هذا الملف يوفر واجهة للربط باستخدام LLD (LLVM Linker).
 * This file provides an interface for linking using LLD (LLVM Linker).
 * 
 * أنواع الربط المدعومة / Supported Linking Types:
 * - Static Linking: ربط ثابت مع جميع المكتبات
 * - Dynamic Linking: ربط ديناميكي مع المكتبات المشتركة
 * - Shared Libraries: إنشاء مكتبات مشتركة (.so, .dll, .dylib)
 * - Position Independent Executable (PIE): تنفيذيات مستقلة عن الموقع
 * 
 * المكتبات المدعومة / Supported Libraries:
 * - System Libraries: مكتبات النظام (libc, libm, libpthread, etc.)
 * - Custom Libraries: مكتبات مخصصة (.a, .so, .lib, .dll)
 * - Runtime Libraries: مكتبات التشغيل (GC, IO, etc.)
 * 
 * الأهداف المدعومة / Supported Targets:
 * - Windows: MSVC, MinGW
 * - Linux: glibc, musl
 * - macOS: Darwin
 * - WebAssembly: WASI
 * 
 * مثال الاستخدام / Usage Example:
 * ```cpp
 * LLVMLinker linker;
 * linker.initialize();
 * linker.addObjectFile("program.o");
 * linker.addLibrary("m");        // -lm
 * linker.addLibraryPath("/usr/lib");
 * linker.setOutputFile("program");
 * linker.link();
 * ```
 */

#ifndef LLVM_LINKER_H
#define LLVM_LINKER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace sad {

/**
 * أنواع الربط / Linking Types
 */
enum class LinkingType {
    Executable,        // ملف تنفيذي / Executable file
    SharedLibrary,     // مكتبة مشتركة / Shared library (.so, .dll, .dylib)
    StaticLibrary      // مكتبة ثابتة / Static library (.a, .lib)
};

/**
 * أنماط الربط / Linking Modes
 */
enum class LinkingMode {
    Static,            // ربط ثابت / Static linking
    Dynamic,           // ربط ديناميكي / Dynamic linking
    PIE                // Position Independent Executable
};

/**
 * مستوى التحسين للربط / Link-Time Optimization Level
 */
enum class LTOLevel {
    None,              // بدون LTO / No LTO
    Thin,              // ThinLTO
    Full               // Full LTO
};

/**
 * خيارات الربط / Linking Options
 */
struct LinkingOptions {
    LinkingType type;                           // نوع الربط / Linking type
    LinkingMode mode;                           // نمط الربط / Linking mode
    LTOLevel lto_level;                         // مستوى LTO / LTO level
    
    bool verbose;                               // طباعة معلومات مفصلة / Verbose output
    bool strip_symbols;                         // إزالة الرموز / Strip symbols
    bool generate_map;                          // إنشاء ملف map / Generate map file
    bool no_stdlib;                             // بدون مكتبة قياسية / No stdlib
    bool no_default_libs;                       // بدون مكتبات افتراضية / No default libs
    
    std::string entry_point;                    // نقطة الدخول / Entry point (default: main)
    std::string output_file;                    // ملف الإخراج / Output file
    std::string map_file;                       // ملف الخريطة / Map file
    
    std::vector<std::string> library_paths;     // مسارات المكتبات / Library paths
    std::vector<std::string> libraries;         // المكتبات / Libraries
    std::vector<std::string> object_files;      // ملفات الكائنات / Object files
    std::vector<std::string> linker_flags;      // أعلام الرابط / Linker flags
    
    std::string target_triple;                  // ثلاثية الهدف / Target triple
    
    LinkingOptions()
        : type(LinkingType::Executable),
          mode(LinkingMode::Dynamic),
          lto_level(LTOLevel::None),
          verbose(false),
          strip_symbols(false),
          generate_map(false),
          no_stdlib(false),
          no_default_libs(false),
          entry_point("main") {}
};

/**
 * معلومات الربط / Linking Information
 */
struct LinkingInfo {
    size_t total_symbols;                       // عدد الرموز الكلي / Total symbols
    size_t undefined_symbols;                   // الرموز غير المعرّفة / Undefined symbols
    size_t output_size;                         // حجم الإخراج / Output size in bytes
    double linking_time_ms;                     // وقت الربط بالميلي ثانية / Linking time
    
    std::vector<std::string> warnings;          // التحذيرات / Warnings
    std::vector<std::string> errors;            // الأخطاء / Errors
    
    LinkingInfo()
        : total_symbols(0), undefined_symbols(0),
          output_size(0), linking_time_ms(0.0) {}
};

/**
 * الرابط LLVM / LLVM Linker
 * 
 * هذه الفئة توفر واجهة عالية المستوى للربط باستخدام LLD.
 * This class provides a high-level interface for linking using LLD.
 */
class LLVMLinker {
public:
    /**
     * المُنشئ / Constructor
     */
    LLVMLinker();
    
    /**
     * المُدمر / Destructor
     */
    ~LLVMLinker();
    
    /**
     * تهيئة الرابط / Initialize linker
     * @param target_triple ثلاثية الهدف / Target triple
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(const std::string& target_triple = "");
    
    /**
     * تعيين نوع الربط / Set linking type
     * @param type نوع الربط / Linking type
     */
    void setLinkingType(LinkingType type);
    
    /**
     * تعيين نمط الربط / Set linking mode
     * @param mode نمط الربط / Linking mode
     */
    void setLinkingMode(LinkingMode mode);
    
    /**
     * تعيين مستوى LTO / Set LTO level
     * @param level مستوى LTO / LTO level
     */
    void setLTOLevel(LTOLevel level);
    
    /**
     * تعيين ملف الإخراج / Set output file
     * @param filename اسم الملف / Filename
     */
    void setOutputFile(const std::string& filename);
    
    /**
     * تعيين نقطة الدخول / Set entry point
     * @param entry_point نقطة الدخول / Entry point
     */
    void setEntryPoint(const std::string& entry_point);
    
    /**
     * إضافة ملف كائن / Add object file
     * @param filename اسم ملف الكائن / Object filename
     */
    void addObjectFile(const std::string& filename);
    
    /**
     * إضافة عدة ملفات كائنات / Add multiple object files
     * @param filenames أسماء ملفات الكائنات / Object filenames
     */
    void addObjectFiles(const std::vector<std::string>& filenames);
    
    /**
     * إضافة مكتبة / Add library
     * @param library اسم المكتبة (بدون lib- أو .a/.so) / Library name
     */
    void addLibrary(const std::string& library);
    
    /**
     * إضافة عدة مكتبات / Add multiple libraries
     * @param libraries أسماء المكتبات / Library names
     */
    void addLibraries(const std::vector<std::string>& libraries);
    
    /**
     * إضافة مسار مكتبات / Add library path
     * @param path مسار المكتبات / Library path
     */
    void addLibraryPath(const std::string& path);
    
    /**
     * إضافة عدة مسارات مكتبات / Add multiple library paths
     * @param paths مسارات المكتبات / Library paths
     */
    void addLibraryPaths(const std::vector<std::string>& paths);
    
    /**
     * إضافة علم رابط / Add linker flag
     * @param flag العلم / Flag
     */
    void addLinkerFlag(const std::string& flag);
    
    /**
     * تعيين إزالة الرموز / Set strip symbols
     * @param strip true لإزالة الرموز / true to strip symbols
     */
    void setStripSymbols(bool strip);
    
    /**
     * تعيين إنشاء ملف خريطة / Set generate map file
     * @param generate true لإنشاء ملف خريطة / true to generate map file
     * @param map_file اسم ملف الخريطة (اختياري) / Map filename (optional)
     */
    void setGenerateMap(bool generate, const std::string& map_file = "");
    
    /**
     * تعيين بدون مكتبة قياسية / Set no stdlib
     * @param no_stdlib true لعدم ربط المكتبة القياسية / true to not link stdlib
     */
    void setNoStdlib(bool no_stdlib);
    
    /**
     * تعيين بدون مكتبات افتراضية / Set no default libs
     * @param no_default_libs true لعدم ربط المكتبات الافتراضية / true to not link default libs
     */
    void setNoDefaultLibs(bool no_default_libs);
    
    /**
     * تعيين مطوّل / Set verbose
     * @param verbose true لطباعة معلومات مفصلة / true for verbose output
     */
    void setVerbose(bool verbose);
    
    /**
     * الربط / Link
     * @return true إذا نجح الربط / true if linking succeeded
     */
    bool link();
    
    /**
     * الربط مع خيارات مخصصة / Link with custom options
     * @param options خيارات الربط / Linking options
     * @return true إذا نجح الربط / true if linking succeeded
     */
    bool link(const LinkingOptions& options);
    
    /**
     * الحصول على معلومات الربط / Get linking information
     * @return معلومات الربط / Linking information
     */
    const LinkingInfo& getLinkingInfo() const;
    
    /**
     * طباعة معلومات الربط / Print linking information
     */
    void printLinkingInfo() const;
    
    /**
     * الحصول على أعلام الرابط الافتراضية للهدف / Get default linker flags for target
     * @param target_triple ثلاثية الهدف / Target triple
     * @return قائمة الأعلام / List of flags
     */
    static std::vector<std::string> getDefaultLinkerFlags(const std::string& target_triple);
    
    /**
     * الحصول على المكتبات الافتراضية للهدف / Get default libraries for target
     * @param target_triple ثلاثية الهدف / Target triple
     * @return قائمة المكتبات / List of libraries
     */
    static std::vector<std::string> getDefaultLibraries(const std::string& target_triple);
    
    /**
     * التحقق من توفر LLD / Check if LLD is available
     * @return true إذا كان LLD متاحاً / true if LLD is available
     */
    static bool isLLDAvailable();

private:
    /**
     * بناء سطر أوامر الرابط / Build linker command line
     * @param options الخيارات / Options
     * @return سطر الأوامر / Command line
     */
    std::string buildLinkerCommandLine(const LinkingOptions& options);
    
    /**
     * تنفيذ الرابط / Execute linker
     * @param command سطر الأوامر / Command line
     * @return true إذا نجح التنفيذ / true if execution succeeded
     */
    bool executeLinker(const std::string& command);
    
    /**
     * استخدام LLD مباشرة / Use LLD directly
     * @param options الخيارات / Options
     * @return true إذا نجح الربط / true if linking succeeded
     */
    bool useLLD(const LinkingOptions& options);
    
    /**
     * استخدام رابط النظام / Use system linker
     * @param options الخيارات / Options
     * @return true إذا نجح الربط / true if linking succeeded
     */
    bool useSystemLinker(const LinkingOptions& options);
    
    /**
     * جمع المعلومات من الإخراج / Collect information from output
     * @param output إخراج الرابط / Linker output
     */
    void collectLinkingInfo(const std::string& output);
    
    /**
     * إضافة المكتبات الافتراضية / Add default libraries
     * @param options الخيارات / Options
     */
    void addDefaultLibraries(LinkingOptions& options);
    
    /**
     * التحقق من الملفات / Verify files
     * @param options الخيارات / Options
     * @return true إذا كانت الملفات صحيحة / true if files are valid
     */
    bool verifyFiles(const LinkingOptions& options);

private:
    std::string target_triple_;                 // ثلاثية الهدف / Target triple
    LinkingOptions options_;                    // خيارات الربط / Linking options
    LinkingInfo info_;                          // معلومات الربط / Linking information
    
    bool initialized_;                          // هل تم التهيئة؟ / Is initialized?
    bool use_lld_;                              // استخدام LLD؟ / Use LLD?
};

} // namespace sad

#endif // LLVM_LINKER_H
