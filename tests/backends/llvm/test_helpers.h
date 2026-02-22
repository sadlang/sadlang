/*
 * مساعدات الاختبار / Test Helpers
 * ================================
 * 
 * دوال وأدوات مساعدة لاختبارات LLVM
 * Helper functions and utilities for LLVM tests
 */

#ifndef SAD_TEST_HELPERS_H
#define SAD_TEST_HELPERS_H

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

namespace sad {
namespace testing {

/**
 * إنشاء وحدة LLVM بسيطة / Create simple LLVM module
 */
inline std::unique_ptr<llvm::Module> createSimpleModule(
    llvm::LLVMContext& context, 
    const std::string& name = "test_module") {
    return std::make_unique<llvm::Module>(name, context);
}

/**
 * إنشاء دالة بسيطة / Create simple function
 */
inline llvm::Function* createSimpleFunction(
    llvm::Module* module,
    const std::string& name,
    llvm::Type* return_type = nullptr) {
    
    if (!return_type) {
        return_type = llvm::Type::getInt32Ty(module->getContext());
    }
    
    auto func_type = llvm::FunctionType::get(return_type, false);
    auto function = llvm::Function::Create(
        func_type,
        llvm::Function::ExternalLinkage,
        name,
        module);
    
    // إنشاء كتلة أساسية / Create basic block
    auto bb = llvm::BasicBlock::Create(module->getContext(), "entry", function);
    llvm::IRBuilder<> builder(bb);
    
    // إرجاع قيمة افتراضية / Return default value
    if (return_type->isVoidTy()) {
        builder.CreateRetVoid();
    } else if (return_type->isIntegerTy()) {
        builder.CreateRet(llvm::ConstantInt::get(return_type, 0));
    } else if (return_type->isFloatingPointTy()) {
        builder.CreateRet(llvm::ConstantFP::get(return_type, 0.0));
    } else {
        builder.CreateRet(llvm::Constant::getNullValue(return_type));
    }
    
    return function;
}

/**
 * التحقق من صحة الوحدة / Verify module
 */
inline bool verifyModule(llvm::Module* module, std::string* error_msg = nullptr) {
    std::string errors;
    llvm::raw_string_ostream stream(errors);
    
    bool broken = llvm::verifyModule(*module, &stream);
    
    if (broken && error_msg) {
        *error_msg = stream.str();
    }
    
    return !broken;
}

/**
 * تحويل الوحدة إلى نص / Convert module to string
 */
inline std::string moduleToString(llvm::Module* module) {
    std::string output;
    llvm::raw_string_ostream stream(output);
    module->print(stream, nullptr);
    return stream.str();
}

/**
 * حفظ الوحدة إلى ملف / Save module to file
 */
inline bool saveModuleToFile(llvm::Module* module, const std::string& filename) {
    std::error_code ec;
    llvm::raw_fd_ostream file(filename, ec);
    
    if (ec) {
        return false;
    }
    
    module->print(file, nullptr);
    return true;
}

/**
 * قراءة ملف نصي / Read text file
 */
inline std::string readTextFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

/**
 * كتابة ملف نصي / Write text file
 */
inline bool writeTextFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

/**
 * التحقق من وجود ملف / Check if file exists
 */
inline bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

/**
 * حذف ملف / Delete file
 */
inline bool deleteFile(const std::string& filename) {
    return std::remove(filename.c_str()) == 0;
}

/**
 * إنشاء ملف كائن وهمي / Create dummy object file
 */
inline bool createDummyObjectFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // كتابة رأس ELF بسيط / Write simple ELF header
    const unsigned char elf_header[] = {
        0x7f, 0x45, 0x4c, 0x46,  // ELF magic
        0x02, 0x01, 0x01, 0x00   // 64-bit, little-endian
    };
    
    file.write(reinterpret_cast<const char*>(elf_header), sizeof(elf_header));
    return true;
}

/**
 * قياس وقت التنفيذ / Measure execution time
 */
template<typename Func>
inline double measureTime(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000.0;  // Return milliseconds
}

/**
 * إنشاء نص LLVM IR بسيط / Create simple LLVM IR text
 */
inline std::string createSimpleLLVMIR() {
    return R"(
; ModuleID = 'test_module'
source_filename = "test.ص"

define i32 @main() {
entry:
  ret i32 0
}
)";
}

/**
 * التحقق من احتواء نص على سلسلة / Check if string contains substring
 */
inline bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

/**
 * التحقق من بدء نص بسلسلة / Check if string starts with prefix
 */
inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

/**
 * التحقق من انتهاء نص بسلسلة / Check if string ends with suffix
 */
inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/**
 * تقسيم نص إلى أسطر / Split string into lines
 */
inline std::vector<std::string> splitLines(const std::string& str) {
    std::vector<std::string> lines;
    std::istringstream stream(str);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

/**
 * إحصاء حدوث سلسلة / Count occurrences of substring
 */
inline size_t countOccurrences(const std::string& str, const std::string& substr) {
    size_t count = 0;
    size_t pos = 0;
    
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        count++;
        pos += substr.size();
    }
    
    return count;
}

/**
 * تنظيف اسم الدالة للاختبار / Sanitize function name for testing
 */
inline std::string sanitizeName(const std::string& name) {
    std::string result = name;
    for (char& c : result) {
        if (!std::isalnum(c)) {
            c = '_';
        }
    }
    return result;
}

} // namespace testing
} // namespace sad

#endif // SAD_TEST_HELPERS_H
