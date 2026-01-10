/*
 * تنفيذ مدير الأهداف LLVM / LLVM Target Manager Implementation
 * ===========================================================
 */

#include "llvm_target.h"
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/TargetParser/Host.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace sad {

/**
 * المُنشئ / Constructor
 */
LLVMTargetManager::LLVMTargetManager()
    : initialized_(false) {
    target_triple_ = getDefaultTargetTriple();
}

/**
 * المُدمر / Destructor
 */
LLVMTargetManager::~LLVMTargetManager() {
}

/**
 * تهيئة مدير الأهداف / Initialize target manager
 */
bool LLVMTargetManager::initialize() {
    if (initialized_) {
        return true;
    }
    
    // تهيئة جميع الأهداف / Initialize all targets
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    
    // تعيين الهدف الافتراضي / Set default target
    if (!setTargetTriple(target_triple_)) {
        std::cerr << "خطأ: فشل تعيين الهدف الافتراضي / Error: Failed to set default target" << std::endl;
        return false;
    }
    
    initialized_ = true;
    return true;
}

/**
 * تعيين ثلاثية الهدف / Set target triple
 */
bool LLVMTargetManager::setTargetTriple(const std::string& triple) {
    target_triple_ = triple;
    
    // تحديث معلومات الهدف / Update target information
    updateTargetInfo();
    
    // البحث عن الهدف / Look up target
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple_, error);
    
    if (!target) {
        std::cerr << "خطأ في البحث عن الهدف / Target lookup error: " << error << std::endl;
        return false;
    }
    
    // إنشاء آلة الهدف بالخيارات الافتراضية / Create target machine with default options
    return createTargetMachine(default_options_);
}

/**
 * الحصول على ثلاثية الهدف الحالية / Get current target triple
 */
std::string LLVMTargetManager::getTargetTriple() const {
    return target_triple_;
}

/**
 * الحصول على معلومات الهدف / Get target information
 */
const TargetInfo& LLVMTargetManager::getTargetInfo() const {
    return target_info_;
}

/**
 * تعيين معالج محدد / Set specific CPU
 */
bool LLVMTargetManager::setCPU(const std::string& cpu) {
    target_info_.cpu = cpu;
    default_options_.cpu = cpu;
    
    // إعادة إنشاء آلة الهدف / Recreate target machine
    return createTargetMachine(default_options_);
}

/**
 * إضافة ميزة معالج / Add CPU feature
 */
void LLVMTargetManager::addFeature(const std::string& feature) {
    target_info_.features.push_back(feature);
    default_options_.features.push_back(feature);
}

/**
 * الحصول على آلة الهدف / Get target machine
 */
llvm::TargetMachine* LLVMTargetManager::getTargetMachine() const {
    return target_machine_.get();
}

/**
 * إنشاء آلة هدف جديدة / Create a new target machine
 */
bool LLVMTargetManager::createTargetMachine(const CodeGenOptions& options) {
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(target_triple_, error);
    
    if (!target) {
        std::cerr << "خطأ / Error: " << error << std::endl;
        return false;
    }
    
    // تحويل مستوى التحسين / Convert optimization level
    llvm::CodeGenOptLevel llvm_opt_level;
    switch (options.opt_level) {
        case CodeGenOptLevel::None:
            llvm_opt_level = llvm::CodeGenOptLevel::None;
            break;
        case CodeGenOptLevel::Less:
            llvm_opt_level = llvm::CodeGenOptLevel::Less;
            break;
        case CodeGenOptLevel::Default:
            llvm_opt_level = llvm::CodeGenOptLevel::Default;
            break;
        case CodeGenOptLevel::Aggressive:
            llvm_opt_level = llvm::CodeGenOptLevel::Aggressive;
            break;
        default:
            llvm_opt_level = llvm::CodeGenOptLevel::Default;
    }
    
    // تحويل نموذج إعادة التوطين / Convert relocation model
    std::optional<llvm::Reloc::Model> reloc_model;
    switch (options.reloc_model) {
        case RelocModel::Static:
            reloc_model = llvm::Reloc::Static;
            break;
        case RelocModel::PIC:
            reloc_model = llvm::Reloc::PIC_;
            break;
        case RelocModel::DynamicNoPIC:
            reloc_model = llvm::Reloc::DynamicNoPIC;
            break;
        case RelocModel::ROPI:
            reloc_model = llvm::Reloc::ROPI;
            break;
        case RelocModel::RWPI:
            reloc_model = llvm::Reloc::RWPI;
            break;
        case RelocModel::ROPIRWPI:
            reloc_model = llvm::Reloc::ROPI_RWPI;
            break;
    }
    
    // تحويل نموذج الكود / Convert code model
    std::optional<llvm::CodeModel::Model> code_model;
    switch (options.code_model) {
        case CodeModel::Tiny:
            code_model = llvm::CodeModel::Tiny;
            break;
        case CodeModel::Small:
            code_model = llvm::CodeModel::Small;
            break;
        case CodeModel::Kernel:
            code_model = llvm::CodeModel::Kernel;
            break;
        case CodeModel::Medium:
            code_model = llvm::CodeModel::Medium;
            break;
        case CodeModel::Large:
            code_model = llvm::CodeModel::Large;
            break;
    }
    
    // خيارات الهدف / Target options
    llvm::TargetOptions target_options;
    
    // بناء سلسلة الميزات / Build features string
    std::string features_str;
    if (!options.features.empty()) {
        for (size_t i = 0; i < options.features.size(); ++i) {
            if (i > 0) features_str += ",";
            features_str += options.features[i];
        }
    }
    
    // إنشاء آلة الهدف / Create target machine
    target_machine_.reset(target->createTargetMachine(
        target_triple_,
        options.cpu.empty() ? target_info_.cpu : options.cpu,
        features_str,
        target_options,
        reloc_model,
        code_model,
        llvm_opt_level
    ));
    
    if (!target_machine_) {
        std::cerr << "خطأ: فشل إنشاء آلة الهدف / Error: Failed to create target machine" << std::endl;
        return false;
    }
    
    return true;
}

/**
 * إصدار ملف كائن / Emit object file
 */
bool LLVMTargetManager::emitObjectFile(llvm::Module* module, const std::string& filename,
                                       const CodeGenOptions& options) {
    return emitFile(module, filename, llvm::CodeGenFileType::ObjectFile, options);
}

/**
 * إصدار ملف تجميع / Emit assembly file
 */
bool LLVMTargetManager::emitAssemblyFile(llvm::Module* module, const std::string& filename,
                                         const CodeGenOptions& options) {
    return emitFile(module, filename, llvm::CodeGenFileType::AssemblyFile, options);
}

/**
 * إصدار ملف LLVM IR / Emit LLVM IR file
 */
bool LLVMTargetManager::emitLLVMIRFile(llvm::Module* module, const std::string& filename, bool binary) {
    if (!module) {
        std::cerr << "خطأ: وحدة فارغة / Error: null module" << std::endl;
        return false;
    }
    
    std::error_code error_code;
    llvm::raw_fd_ostream output_stream(filename, error_code, llvm::sys::fs::OF_None);
    
    if (error_code) {
        std::cerr << "خطأ في فتح الملف / Error opening file: " << error_code.message() << std::endl;
        return false;
    }
    
    if (binary) {
        // إصدار bitcode / Emit bitcode
        llvm::WriteBitcodeToFile(*module, output_stream);
    } else {
        // إصدار نص IR / Emit IR text
        module->print(output_stream, nullptr);
    }
    
    output_stream.flush();
    return true;
}

/**
 * إصدار ملف تنفيذي / Emit executable file
 */
bool LLVMTargetManager::emitExecutable(llvm::Module* module, const std::string& filename,
                                       const CodeGenOptions& options) {
    // أولاً، إصدار ملف كائن / First, emit object file
    std::string object_file = filename + ".o";
    
    if (!emitObjectFile(module, object_file, options)) {
        std::cerr << "خطأ: فشل إصدار ملف الكائن / Error: Failed to emit object file" << std::endl;
        return false;
    }
    
    // ثانياً، استدعاء الرابط / Second, invoke linker
    if (!linkExecutable(object_file, filename, options)) {
        std::cerr << "خطأ: فشل الربط / Error: Failed to link" << std::endl;
        return false;
    }
    
    return true;
}

/**
 * الحصول على قائمة الأهداف المتاحة / Get available targets
 */
std::vector<std::string> LLVMTargetManager::getAvailableTargets() {
    std::vector<std::string> targets;
    
    // المرور على جميع الأهداف المسجلة / Iterate through all registered targets
    for (auto& target : llvm::TargetRegistry::targets()) {
        targets.push_back(target.getName());
    }
    
    return targets;
}

/**
 * الحصول على ثلاثية الهدف الافتراضية / Get default target triple
 */
std::string LLVMTargetManager::getDefaultTargetTriple() {
    return llvm::sys::getDefaultTargetTriple();
}

/**
 * الحصول على المعالج الافتراضي / Get default CPU
 */
std::string LLVMTargetManager::getDefaultCPU() {
    return "generic"; // Host CPU detection not available
}

/**
 * التحقق من دعم الهدف / Check if target is supported
 */
bool LLVMTargetManager::isTargetSupported(const std::string& triple) {
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, error);
    return target != nullptr;
}

/**
 * تحليل ثلاثية الهدف / Parse target triple
 */
bool LLVMTargetManager::parseTargetTriple(const std::string& triple, TargetInfo& info) {
    info.triple = triple;
    
    // تحليل الثلاثية / Parse triple
    llvm::Triple llvm_triple(triple);
    
    // استخراج المكونات / Extract components
    info.arch = llvm_triple.getArchName().str();
    info.vendor = llvm_triple.getVendorName().str();
    info.os = llvm_triple.getOSName().str();
    info.environment = llvm_triple.getEnvironmentName().str();
    
    // تحديد حجم المؤشر / Determine pointer size
    info.is_64bit = llvm_triple.isArch64Bit();
    info.pointer_size = info.is_64bit ? 8 : 4;
    
    // تحديد Endianness / Determine endianness
    info.is_little_endian = llvm_triple.isLittleEndian();
    
    return true;
}

/**
 * طباعة معلومات الهدف / Print target information
 */
void LLVMTargetManager::printTargetInfo() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "معلومات الهدف / Target Information" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "الثلاثية / Triple: " << target_info_.triple << std::endl;
    std::cout << "المعمارية / Architecture: " << target_info_.arch << std::endl;
    std::cout << "البائع / Vendor: " << target_info_.vendor << std::endl;
    std::cout << "نظام التشغيل / OS: " << target_info_.os << std::endl;
    std::cout << "البيئة / Environment: " << target_info_.environment << std::endl;
    std::cout << "المعالج / CPU: " << target_info_.cpu << std::endl;
    
    if (!target_info_.features.empty()) {
        std::cout << "الميزات / Features: ";
        for (size_t i = 0; i < target_info_.features.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << target_info_.features[i];
        }
        std::cout << std::endl;
    }
    
    std::cout << "حجم المؤشر / Pointer Size: " << target_info_.pointer_size << " bytes" << std::endl;
    std::cout << "64 بت / 64-bit: " << (target_info_.is_64bit ? "نعم / Yes" : "لا / No") << std::endl;
    std::cout << "Little Endian: " << (target_info_.is_little_endian ? "نعم / Yes" : "لا / No") << std::endl;
    
    std::cout << "========================================\n" << std::endl;
}

/**
 * تعيين خيارات إنتاج الكود الافتراضية / Set default code generation options
 */
void LLVMTargetManager::setDefaultCodeGenOptions(const CodeGenOptions& options) {
    default_options_ = options;
}

/**
 * تحديث معلومات الهدف / Update target information
 */
void LLVMTargetManager::updateTargetInfo() {
    parseTargetTriple(target_triple_, target_info_);
    
    // تعيين المعالج الافتراضي إذا لم يكن محدداً / Set default CPU if not specified
    if (target_info_.cpu.empty()) {
        target_info_.cpu = "generic";
    }
}

/**
 * إصدار ملف بنوع محدد / Emit file with specific type
 */
bool LLVMTargetManager::emitFile(llvm::Module* module, const std::string& filename,
                                 llvm::CodeGenFileType file_type, const CodeGenOptions& options) {
    if (!module) {
        std::cerr << "خطأ: وحدة فارغة / Error: null module" << std::endl;
        return false;
    }
    
    if (!target_machine_) {
        std::cerr << "خطأ: آلة الهدف غير مهيئة / Error: target machine not initialized" << std::endl;
        return false;
    }
    
    // التحقق من الوحدة / Verify module
    if (options.verify_module) {
        if (!verifyModule(module)) {
            std::cerr << "خطأ: الوحدة غير صحيحة / Error: module is invalid" << std::endl;
            return false;
        }
    }
    
    // تعيين data layout للوحدة / Set data layout for module
    module->setDataLayout(target_machine_->createDataLayout());
    module->setTargetTriple(target_triple_);
    
    // فتح ملف الإخراج / Open output file
    std::error_code error_code;
    llvm::raw_fd_ostream output_stream(filename, error_code, llvm::sys::fs::OF_None);
    
    if (error_code) {
        std::cerr << "خطأ في فتح الملف / Error opening file: " << error_code.message() << std::endl;
        return false;
    }
    
    // إنشاء مدير التمريرات / Create pass manager
    llvm::legacy::PassManager pass_manager;
    
    // إضافة تمريرة إصدار الكود / Add code emission pass
    if (target_machine_->addPassesToEmitFile(pass_manager, output_stream, nullptr, file_type)) {
        std::cerr << "خطأ: لا يدعم الهدف إصدار هذا النوع من الملفات / Error: target does not support this file type" << std::endl;
        return false;
    }
    
    // تشغيل التمريرات / Run passes
    pass_manager.run(*module);
    output_stream.flush();
    
    if (options.verbose) {
        std::cout << "تم إصدار الملف بنجاح / File emitted successfully: " << filename << std::endl;
    }
    
    // إصدار LLVM IR إذا طُلب / Emit LLVM IR if requested
    if (options.emit_llvm_ir) {
        std::string ir_filename = filename + ".ll";
        emitLLVMIRFile(module, ir_filename, false);
    }
    
    return true;
}

/**
 * التحقق من الوحدة / Verify module
 */
bool LLVMTargetManager::verifyModule(llvm::Module* module) {
    if (!module) return false;
    
    std::string error_msg;
    llvm::raw_string_ostream error_stream(error_msg);
    
    bool has_errors = llvm::verifyModule(*module, &error_stream);
    
    if (has_errors) {
        std::cerr << "خطأ في التحقق من الوحدة / Module verification failed:" << std::endl;
        std::cerr << error_msg << std::endl;
        return false;
    }
    
    return true;
}

/**
 * استدعاء الرابط / Invoke linker
 */
bool LLVMTargetManager::linkExecutable(const std::string& object_file, const std::string& executable_file,
                                       const CodeGenOptions& options) {
    // تحديد الرابط المناسب حسب النظام / Determine appropriate linker for system
    std::string linker_cmd;
    
    #ifdef _WIN32
        // استخدام link.exe على Windows / Use link.exe on Windows
        linker_cmd = "link.exe /OUT:" + executable_file + " " + object_file;
        
        // إضافة مكتبات runtime إذا لزم الأمر / Add runtime libraries if needed
        linker_cmd += " /DEFAULTLIB:msvcrt.lib /DEFAULTLIB:kernel32.lib";
    #else
        // استخدام clang أو gcc على Unix / Use clang or gcc on Unix
        linker_cmd = "clang -o " + executable_file + " " + object_file;
        
        // إضافة مكتبات runtime / Add runtime libraries
        linker_cmd += " -lm -lpthread";
    #endif
    
    if (options.verbose) {
        std::cout << "تشغيل الرابط / Running linker: " << linker_cmd << std::endl;
    }
    
    // تنفيذ أمر الربط / Execute linker command
    int result = std::system(linker_cmd.c_str());
    
    if (result != 0) {
        std::cerr << "خطأ: فشل الربط / Error: Linker failed with code " << result << std::endl;
        return false;
    }
    
    if (options.verbose) {
        std::cout << "تم إنشاء الملف التنفيذي بنجاح / Executable created successfully: " 
                  << executable_file << std::endl;
    }
    
    return true;
}

} // namespace sad
