/*
 * مدير الأهداف LLVM / LLVM Target Manager
 * ======================================
 * 
 * هذا الملف يوفر واجهة لإدارة أهداف التجميع المختلفة وإنتاج ملفات الكائنات.
 * This file provides an interface for managing different compilation targets and producing object files.
 * 
 * المنصات المدعومة / Supported Platforms:
 * - x86_64 (Windows, Linux, macOS)
 * - ARM64 (Linux, macOS, Android, iOS)
 * - RISC-V (Linux)
 * - WebAssembly (Browser, Node.js)
 * 
 * أنواع الملفات المدعومة / Supported File Types:
 * - Object Files (.o, .obj)
 * - Assembly Files (.s, .asm)
 * - LLVM IR Files (.ll, .bc)
 * - Executable Files (.exe, no extension)
 * 
 * مثال الاستخدام / Usage Example:
 * ```cpp
 * LLVMTargetManager target_manager;
 * target_manager.initialize();
 * target_manager.setTargetTriple("x86_64-pc-windows-msvc");
 * target_manager.emitObjectFile(module.get(), "output.obj");
 * ```
 */

#ifndef LLVM_TARGET_H
#define LLVM_TARGET_H

#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/CodeGen.h>
#include <memory>
#include <string>
#include <vector>

namespace sad {

/**
 * أنواع الملفات المخرجة / Output File Types
 */
enum class OutputFileType {
    ObjectFile,      // ملف كائن / Object file (.o, .obj)
    AssemblyFile,    // ملف تجميع / Assembly file (.s, .asm)
    LLVMIRText,      // LLVM IR نصي / LLVM IR text (.ll)
    LLVMIRBitcode,   // LLVM IR ثنائي / LLVM IR bitcode (.bc)
    Executable       // ملف تنفيذي / Executable file
};

/**
 * مستويات التحسين للكود المُنتج / Code Generation Optimization Levels
 */
enum class CodeGenOptLevel {
    None,       // بدون تحسين / No optimization
    Less,       // تحسين أقل / Less optimization
    Default,    // تحسين افتراضي / Default optimization
    Aggressive  // تحسين عدواني / Aggressive optimization
};

/**
 * نماذج إعادة التوطين / Relocation Models
 */
enum class RelocModel {
    Static,     // ثابت / Static
    PIC,        // Position Independent Code
    DynamicNoPIC,
    ROPI,       // Read-Only Position Independent
    RWPI,       // Read-Write Position Independent
    ROPIRWPI    // Both ROPI and RWPI
};

/**
 * نماذج الكود / Code Models
 */
enum class CodeModel {
    Tiny,       // صغير جداً / Tiny
    Small,      // صغير / Small
    Kernel,     // نواة / Kernel
    Medium,     // متوسط / Medium
    Large       // كبير / Large
};

/**
 * معلومات الهدف / Target Information
 */
struct TargetInfo {
    std::string triple;              // ثلاثية الهدف / Target triple
    std::string arch;                // المعمارية / Architecture (x86_64, arm64, etc.)
    std::string vendor;              // البائع / Vendor (pc, apple, unknown, etc.)
    std::string os;                  // نظام التشغيل / OS (windows, linux, darwin, etc.)
    std::string environment;         // البيئة / Environment (msvc, gnu, etc.)
    std::string cpu;                 // المعالج / CPU (generic, skylake, cortex-a72, etc.)
    std::vector<std::string> features;  // الميزات / Features (+sse4.2, +avx, etc.)
    
    size_t pointer_size;             // حجم المؤشر بالبايتات / Pointer size in bytes
    bool is_little_endian;           // Little Endian؟ / Is little endian?
    bool is_64bit;                   // 64 بت؟ / Is 64-bit?
    
    TargetInfo() : pointer_size(8), is_little_endian(true), is_64bit(true) {}
};

/**
 * خيارات إنتاج الكود / Code Generation Options
 */
struct CodeGenOptions {
    OutputFileType file_type;        // نوع الملف المخرج / Output file type
    CodeGenOptLevel opt_level;       // مستوى التحسين / Optimization level
    RelocModel reloc_model;          // نموذج إعادة التوطين / Relocation model
    CodeModel code_model;            // نموذج الكود / Code model
    
    bool verbose;                    // طباعة معلومات مفصلة / Print verbose info
    bool verify_module;              // التحقق من الوحدة / Verify module
    bool emit_dwarf;                 // إصدار معلومات DWARF / Emit DWARF info
    bool emit_llvm_ir;               // إصدار LLVM IR أيضاً / Also emit LLVM IR
    
    std::string cpu;                 // المعالج المستهدف / Target CPU
    std::vector<std::string> features;  // ميزات المعالج / CPU features
    
    CodeGenOptions()
        : file_type(OutputFileType::ObjectFile),
          opt_level(CodeGenOptLevel::Default),
          reloc_model(RelocModel::PIC),
          code_model(CodeModel::Small),
          verbose(false),
          verify_module(true),
          emit_dwarf(true),
          emit_llvm_ir(false) {}
};

/**
 * مدير الأهداف LLVM / LLVM Target Manager
 * 
 * هذه الفئة توفر واجهة عالية المستوى لإدارة أهداف التجميع المختلفة.
 * This class provides a high-level interface for managing different compilation targets.
 */
class LLVMTargetManager {
public:
    /**
     * المُنشئ / Constructor
     */
    LLVMTargetManager();
    
    /**
     * المُدمر / Destructor
     */
    ~LLVMTargetManager();
    
    /**
     * تهيئة مدير الأهداف / Initialize target manager
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize();
    
    /**
     * تعيين ثلاثية الهدف / Set target triple
     * @param triple ثلاثية الهدف (مثل: x86_64-pc-windows-msvc) / Target triple
     * @return true إذا نجح التعيين / true if setting succeeded
     */
    bool setTargetTriple(const std::string& triple);
    
    /**
     * الحصول على ثلاثية الهدف الحالية / Get current target triple
     * @return ثلاثية الهدف / Target triple
     */
    std::string getTargetTriple() const;
    
    /**
     * الحصول على معلومات الهدف / Get target information
     * @return معلومات الهدف / Target information
     */
    const TargetInfo& getTargetInfo() const;
    
    /**
     * تعيين معالج محدد / Set specific CPU
     * @param cpu اسم المعالج / CPU name (e.g., "skylake", "cortex-a72")
     * @return true إذا نجح التعيين / true if setting succeeded
     */
    bool setCPU(const std::string& cpu);
    
    /**
     * إضافة ميزة معالج / Add CPU feature
     * @param feature الميزة (مثل: "+sse4.2", "+avx") / Feature (e.g., "+sse4.2", "+avx")
     */
    void addFeature(const std::string& feature);
    
    /**
     * الحصول على آلة الهدف / Get target machine
     * @return آلة الهدف / Target machine
     */
    llvm::TargetMachine* getTargetMachine() const;
    
    /**
     * إنشاء آلة هدف جديدة / Create a new target machine
     * @param options خيارات إنتاج الكود / Code generation options
     * @return true إذا نجح الإنشاء / true if creation succeeded
     */
    bool createTargetMachine(const CodeGenOptions& options);
    
    /**
     * إصدار ملف كائن / Emit object file
     * @param module الوحدة المراد تجميعها / Module to compile
     * @param filename اسم الملف المخرج / Output filename
     * @param options خيارات إنتاج الكود / Code generation options
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitObjectFile(llvm::Module* module, const std::string& filename,
                        const CodeGenOptions& options = CodeGenOptions());
    
    /**
     * إصدار ملف تجميع / Emit assembly file
     * @param module الوحدة المراد تجميعها / Module to compile
     * @param filename اسم الملف المخرج / Output filename
     * @param options خيارات إنتاج الكود / Code generation options
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitAssemblyFile(llvm::Module* module, const std::string& filename,
                          const CodeGenOptions& options = CodeGenOptions());
    
    /**
     * إصدار ملف LLVM IR / Emit LLVM IR file
     * @param module الوحدة / Module
     * @param filename اسم الملف المخرج / Output filename
     * @param binary true لإصدار bitcode، false لإصدار نص / true for bitcode, false for text
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitLLVMIRFile(llvm::Module* module, const std::string& filename, bool binary = false);
    
    /**
     * إصدار ملف تنفيذي / Emit executable file
     * @param module الوحدة / Module
     * @param filename اسم الملف المخرج / Output filename
     * @param options خيارات إنتاج الكود / Code generation options
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitExecutable(llvm::Module* module, const std::string& filename,
                        const CodeGenOptions& options = CodeGenOptions());
    
    /**
     * الحصول على قائمة الأهداف المتاحة / Get list of available targets
     * @return قائمة بأسماء الأهداف / List of target names
     */
    static std::vector<std::string> getAvailableTargets();
    
    /**
     * الحصول على ثلاثية الهدف الافتراضية للنظام الحالي / Get default target triple for current system
     * @return ثلاثية الهدف الافتراضية / Default target triple
     */
    static std::string getDefaultTargetTriple();
    
    /**
     * الحصول على المعالج الافتراضي للنظام الحالي / Get default CPU for current system
     * @return اسم المعالج الافتراضي / Default CPU name
     */
    static std::string getDefaultCPU();
    
    /**
     * التحقق من دعم الهدف / Check if target is supported
     * @param triple ثلاثية الهدف / Target triple
     * @return true إذا كان الهدف مدعوماً / true if target is supported
     */
    static bool isTargetSupported(const std::string& triple);
    
    /**
     * تحليل ثلاثية الهدف / Parse target triple
     * @param triple ثلاثية الهدف / Target triple
     * @param info معلومات الهدف المخرجة / Output target information
     * @return true إذا نجح التحليل / true if parsing succeeded
     */
    static bool parseTargetTriple(const std::string& triple, TargetInfo& info);
    
    /**
     * طباعة معلومات الهدف / Print target information
     */
    void printTargetInfo() const;
    
    /**
     * تعيين خيارات إنتاج الكود الافتراضية / Set default code generation options
     * @param options الخيارات / Options
     */
    void setDefaultCodeGenOptions(const CodeGenOptions& options);

private:
    /**
     * تحديث معلومات الهدف / Update target information
     */
    void updateTargetInfo();
    
    /**
     * إصدار ملف بنوع محدد / Emit file with specific type
     * @param module الوحدة / Module
     * @param filename اسم الملف / Filename
     * @param file_type نوع الملف / File type
     * @param options الخيارات / Options
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitFile(llvm::Module* module, const std::string& filename,
                  llvm::CodeGenFileType file_type, const CodeGenOptions& options);
    
    /**
     * التحقق من الوحدة / Verify module
     * @param module الوحدة / Module
     * @return true إذا كانت الوحدة صحيحة / true if module is valid
     */
    bool verifyModule(llvm::Module* module);
    
    /**
     * استدعاء الرابط لإنشاء ملف تنفيذي / Invoke linker to create executable
     * @param object_file ملف الكائن / Object file
     * @param executable_file الملف التنفيذي / Executable file
     * @param options الخيارات / Options
     * @return true إذا نجح الربط / true if linking succeeded
     */
    bool linkExecutable(const std::string& object_file, const std::string& executable_file,
                       const CodeGenOptions& options);

private:
    std::string target_triple_;                      // ثلاثية الهدف / Target triple
    TargetInfo target_info_;                         // معلومات الهدف / Target information
    std::unique_ptr<llvm::TargetMachine> target_machine_;  // آلة الهدف / Target machine
    CodeGenOptions default_options_;                 // الخيارات الافتراضية / Default options
    
    bool initialized_;                               // هل تم التهيئة؟ / Is initialized?
};

} // namespace sad

#endif // LLVM_TARGET_H
