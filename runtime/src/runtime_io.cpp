/**
 * @file runtime_io.cpp
 * @brief تطبيق دوال الإدخال والإخراج / I/O Functions Implementation
 * @brief Implementation of print, input, and file operations
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/runtime.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace Sad {
namespace Runtime {

// ========================================
// FileManager Implementation
// ========================================

int FileManager::openFile(const std::string& filename, const std::string& mode) {
    std::ios_base::openmode flags = std::ios_base::in;
    
    // (AR) تحويل النمط إلى flags
    // (EN) Convert mode to flags
    if (mode == "r") {
        flags = std::ios_base::in;
    } else if (mode == "w") {
        flags = std::ios_base::out | std::ios_base::trunc;
    } else if (mode == "a") {
        flags = std::ios_base::out | std::ios_base::app;
    } else if (mode == "rb") {
        flags = std::ios_base::in | std::ios_base::binary;
    } else if (mode == "wb") {
        flags = std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;
    } else if (mode == "r+") {
        flags = std::ios_base::in | std::ios_base::out;
    } else {
        return -1; // Invalid mode
    }
    
    std::fstream* file = new std::fstream(filename, flags);
    
    if (!file->is_open()) {
        delete file;
        return -1;
    }
    
    int fd = nextFd_++;
    
    // (AR) وسّع المصفوفة إذا لزم الأمر
    // (EN) Expand array if needed
    if (static_cast<size_t>(fd) >= files_.size()) {
        files_.resize(fd + 1, nullptr);
    }
    
    files_[fd] = file;
    return fd;
}

bool FileManager::closeFile(int fd) {
    if (fd < 0 || static_cast<size_t>(fd) >= files_.size() || !files_[fd]) {
        return false;
    }
    
    files_[fd]->close();
    delete files_[fd];
    files_[fd] = nullptr;
    return true;
}

std::fstream* FileManager::getFile(int fd) {
    if (fd < 0 || static_cast<size_t>(fd) >= files_.size()) {
        return nullptr;
    }
    return files_[fd];
}

void FileManager::closeAll() {
    for (auto* file : files_) {
        if (file) {
            file->close();
            delete file;
        }
    }
    files_.clear();
}

// مدير ملفات عام / Global file manager
static FileManager g_fileManager;

// ========================================
// I/O Functions - Print
// ========================================

VM::Value runtime_print(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    for (int i = 0; i < argc; ++i) {
        VM::printValue(args[i]);
        if (i < argc - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::flush;
    return VM::Value::Null();
}

VM::Value runtime_println(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    runtime_print(vm, argc, args);
    std::cout << "\n";
    return VM::Value::Null();
}

VM::Value runtime_printf(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] printf requires format string\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isString()) {
        std::cerr << "[Error] printf format must be string\n";
        return VM::Value::Null();
    }
    
    VM::StringObject* formatObj = args[0].asString();
    std::string format(formatObj->chars, formatObj->length);
    
    std::ostringstream oss;
    int argIndex = 1;
    
    for (size_t i = 0; i < format.length(); ++i) {
        if (format[i] == '%' && i + 1 < format.length()) {
            char spec = format[i + 1];
            
            if (spec == '%') {
                oss << '%';
            } else if (argIndex < argc) {
                const VM::Value& arg = args[argIndex++];
                
                switch (spec) {
                    case 'd':
                    case 'i':
                        if (arg.isInt()) {
                            oss << arg.asInt();
                        } else {
                            oss << static_cast<int64_t>(vm->toFloat(arg));
                        }
                        break;
                        
                    case 'f':
                        oss << std::fixed << std::setprecision(6) << vm->toFloat(arg);
                        break;
                        
                    case 's':
                        if (arg.isString()) {
                            VM::StringObject* str = arg.asString();
                            oss << std::string(str->chars, str->length);
                        } else {
                            VM::printValue(arg, oss);
                        }
                        break;
                        
                    case 'c':
                        if (arg.isInt()) {
                            oss << static_cast<char>(arg.asInt());
                        }
                        break;
                        
                    default:
                        oss << '%' << spec;
                        break;
                }
            }
            
            i++; // Skip format specifier
        } else {
            oss << format[i];
        }
    }
    
    std::cout << oss.str() << std::flush;
    return VM::Value::Null();
}

// ========================================
// I/O Functions - Input
// ========================================

VM::Value runtime_input(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    // (AR) اطبع الرسالة إذا وُجدت
    // (EN) Print prompt if provided
    if (argc > 0) {
        runtime_print(vm, argc, args);
    }
    
    std::string line;
    if (!std::getline(std::cin, line)) {
        return VM::Value::Null();
    }
    
    // (AR) أنشئ كائن نص
    // (EN) Create string object
    VM::StringObject* str = static_cast<VM::StringObject*>(
        vm->allocateObject(VM::OBJ_STRING, sizeof(VM::StringObject))
    );
    
    str->length = line.length();
    str->chars = new char[line.length() + 1];
    std::memcpy(str->chars, line.c_str(), line.length() + 1);
    str->hash = std::hash<std::string>{}(line);
    
    return VM::Value::String(str);
}

VM::Value runtime_read_int(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    // اطبع الرسالة
    if (argc > 0) {
        runtime_print(vm, argc, args);
    }
    
    int64_t value;
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return VM::Value::Null();
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return VM::Value::Int(value);
}

VM::Value runtime_read_float(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc > 0) {
        runtime_print(vm, argc, args);
    }
    
    double value;
    if (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return VM::Value::Null();
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return VM::Value::Float(value);
}

// ========================================
// File Operations
// ========================================

VM::Value runtime_fopen(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] fopen requires filename and mode\n";
        return VM::Value::Int(-1);
    }
    
    if (!args[0].isString() || !args[1].isString()) {
        std::cerr << "[Error] fopen arguments must be strings\n";
        return VM::Value::Int(-1);
    }
    
    VM::StringObject* filenameObj = args[0].asString();
    VM::StringObject* modeObj = args[1].asString();
    
    std::string filename(filenameObj->chars, filenameObj->length);
    std::string mode(modeObj->chars, modeObj->length);
    
    int fd = g_fileManager.openFile(filename, mode);
    return VM::Value::Int(fd);
}

VM::Value runtime_fread(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] fread requires fd and size\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isInt() || !args[1].isInt()) {
        std::cerr << "[Error] fread arguments must be integers\n";
        return VM::Value::Null();
    }
    
    int fd = static_cast<int>(args[0].asInt());
    size_t size = static_cast<size_t>(args[1].asInt());
    
    std::fstream* file = g_fileManager.getFile(fd);
    if (!file) {
        std::cerr << "[Error] Invalid file descriptor\n";
        return VM::Value::Null();
    }
    
    std::vector<char> buffer(size);
    file->read(buffer.data(), size);
    size_t actualRead = file->gcount();
    
    // (AR) أنشئ كائن نص
    // (EN) Create string object
    VM::StringObject* str = static_cast<VM::StringObject*>(
        vm->allocateObject(VM::OBJ_STRING, sizeof(VM::StringObject))
    );
    
    str->length = actualRead;
    str->chars = new char[actualRead + 1];
    std::memcpy(str->chars, buffer.data(), actualRead);
    str->chars[actualRead] = '\0';
    str->hash = std::hash<std::string>{}(std::string(str->chars, actualRead));
    
    return VM::Value::String(str);
}

VM::Value runtime_fwrite(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] fwrite requires fd and data\n";
        return VM::Value::Int(-1);
    }
    
    if (!args[0].isInt()) {
        std::cerr << "[Error] fwrite fd must be integer\n";
        return VM::Value::Int(-1);
    }
    
    int fd = static_cast<int>(args[0].asInt());
    std::fstream* file = g_fileManager.getFile(fd);
    
    if (!file) {
        std::cerr << "[Error] Invalid file descriptor\n";
        return VM::Value::Int(-1);
    }
    
    // (AR) تحويل البيانات إلى نص
    // (EN) Convert data to string
    std::ostringstream oss;
    VM::printValue(args[1], oss);
    std::string data = oss.str();
    
    file->write(data.c_str(), data.length());
    return VM::Value::Int(static_cast<int64_t>(data.length()));
}

VM::Value runtime_fclose(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] fclose requires fd\n";
        return VM::Value::Int(-1);
    }
    
    if (!args[0].isInt()) {
        std::cerr << "[Error] fclose fd must be integer\n";
        return VM::Value::Int(-1);
    }
    
    int fd = static_cast<int>(args[0].asInt());
    bool success = g_fileManager.closeFile(fd);
    
    return VM::Value::Int(success ? 0 : -1);
}

VM::Value runtime_read_file(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] read_file requires filename\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isString()) {
        std::cerr << "[Error] read_file filename must be string\n";
        return VM::Value::Null();
    }
    
    VM::StringObject* filenameObj = args[0].asString();
    std::string filename(filenameObj->chars, filenameObj->length);
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open file: " << filename << "\n";
        return VM::Value::Null();
    }
    
    // (AR) اقرأ المحتوى كاملاً
    // (EN) Read entire contents
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string contents = oss.str();
    
    // (AR) أنشئ كائن نص
    // (EN) Create string object
    VM::StringObject* str = static_cast<VM::StringObject*>(
        vm->allocateObject(VM::OBJ_STRING, sizeof(VM::StringObject))
    );
    
    str->length = contents.length();
    str->chars = new char[contents.length() + 1];
    std::memcpy(str->chars, contents.c_str(), contents.length() + 1);
    str->hash = std::hash<std::string>{}(contents);
    
    return VM::Value::String(str);
}

VM::Value runtime_write_file(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] write_file requires filename and data\n";
        return VM::Value::Int(-1);
    }
    
    if (!args[0].isString()) {
        std::cerr << "[Error] write_file filename must be string\n";
        return VM::Value::Int(-1);
    }
    
    VM::StringObject* filenameObj = args[0].asString();
    std::string filename(filenameObj->chars, filenameObj->length);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot create file: " << filename << "\n";
        return VM::Value::Int(-1);
    }
    
    // (AR) تحويل البيانات إلى نص
    // (EN) Convert data to string
    std::ostringstream oss;
    VM::printValue(args[1], oss);
    std::string data = oss.str();
    
    file.write(data.c_str(), data.length());
    return VM::Value::Int(static_cast<int64_t>(data.length()));
}

// ========================================
// Registration Functions
// ========================================

void registerIOFunctions(VM::VirtualMachine* vm) {
    // Print functions
    vm->registerNative("print", runtime_print);
    vm->registerNative("println", runtime_println);
    vm->registerNative("printf", runtime_printf);
    
    // Input functions
    vm->registerNative("input", runtime_input);
    vm->registerNative("read_int", runtime_read_int);
    vm->registerNative("read_float", runtime_read_float);
    
    // File operations
    vm->registerNative("fopen", runtime_fopen);
    vm->registerNative("fread", runtime_fread);
    vm->registerNative("fwrite", runtime_fwrite);
    vm->registerNative("fclose", runtime_fclose);
    vm->registerNative("read_file", runtime_read_file);
    vm->registerNative("write_file", runtime_write_file);
}

} // namespace Runtime
} // namespace Sad
