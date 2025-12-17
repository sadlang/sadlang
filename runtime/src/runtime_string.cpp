/**
 * @file runtime_string.cpp
 * @brief تطبيق دوال معالجة النصوص / String Functions Implementation
 * @brief Implementation of string manipulation, search, and transformation
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/runtime.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstring>
#include <iostream>

namespace Sad {
namespace Runtime {

// ========================================
// Helper Functions
// ========================================

// (AR) دالة مساعدة لإنشاء كائن نص جديد
// (EN) Helper to create new string object
static VM::StringObject* createString(VM::VirtualMachine* vm, const std::string& str) {
    VM::StringObject* strObj = static_cast<VM::StringObject*>(
        vm->allocateObject(VM::OBJ_STRING, sizeof(VM::StringObject))
    );
    
    strObj->length = str.length();
    strObj->chars = new char[str.length() + 1];
    std::memcpy(strObj->chars, str.c_str(), str.length() + 1);
    strObj->hash = std::hash<std::string>{}(str);
    
    return strObj;
}

// (AR) دالة مساعدة لتحويل القيمة إلى نص
// (EN) Helper to convert Value to std::string
static std::string valueToString(const VM::Value& val) {
    if (val.isString()) {
        VM::StringObject* str = val.asString();
        return std::string(str->chars, str->length);
    }
    
    std::ostringstream oss;
    VM::printValue(val, oss);
    return oss.str();
}

// ========================================
// String Manipulation
// ========================================

VM::Value runtime_split(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] split requires string and delimiter\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::string delim = valueToString(args[1]);
    
    // (AR) أنشئ مصفوفة للنتائج
    // (EN) Create array for results
    VM::ArrayObject* arr = static_cast<VM::ArrayObject*>(
        vm->allocateObject(VM::OBJ_ARRAY, sizeof(VM::ArrayObject))
    );
    arr->elements = new std::vector<VM::Value>();
    
    if (delim.empty()) {
        // (AR) إذا كان الفاصل فارغًا، افصل كل حرف
        // (EN) If delimiter is empty, split into characters
        for (char c : str) {
            std::string charStr(1, c);
            arr->elements->push_back(VM::Value::String(createString(vm, charStr)));
        }
    } else {
        size_t start = 0;
        size_t end = str.find(delim);
        
        while (end != std::string::npos) {
            std::string part = str.substr(start, end - start);
            arr->elements->push_back(VM::Value::String(createString(vm, part)));
            
            start = end + delim.length();
            end = str.find(delim, start);
        }
        
        // (AR) أضف الجزء الأخير
        // (EN) Add last part
        std::string part = str.substr(start);
        arr->elements->push_back(VM::Value::String(createString(vm, part)));
    }
    
    return VM::Value::Array(arr);
}

VM::Value runtime_join(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] join requires array and separator\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] join first argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    std::string sep = valueToString(args[1]);
    
    std::ostringstream oss;
    for (size_t i = 0; i < arr->elements->size(); ++i) {
        oss << valueToString((*arr->elements)[i]);
        if (i < arr->elements->size() - 1) {
            oss << sep;
        }
    }
    
    return VM::Value::String(createString(vm, oss.str()));
}

VM::Value runtime_replace(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 3) {
        std::cerr << "[Error] replace requires string, old, and new\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::string oldStr = valueToString(args[1]);
    std::string newStr = valueToString(args[2]);
    
    if (oldStr.empty()) {
        return args[0]; // Can't replace empty string
    }
    
    size_t pos = 0;
    while ((pos = str.find(oldStr, pos)) != std::string::npos) {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    
    return VM::Value::String(createString(vm, str));
}

VM::Value runtime_trim(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] trim requires string\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    
    // (AR) احذف المسافات من البداية
    // (EN) Trim from start
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    
    // (AR) احذف المسافات من النهاية
    // (EN) Trim from end
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
    
    return VM::Value::String(createString(vm, str));
}

// ========================================
// Case Conversion
// ========================================

VM::Value runtime_to_upper(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] to_upper requires string\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::transform(str.begin(), str.end(), str.begin(), 
                   [](unsigned char c) { return std::toupper(c); });
    
    return VM::Value::String(createString(vm, str));
}

VM::Value runtime_to_lower(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] to_lower requires string\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return VM::Value::String(createString(vm, str));
}

// ========================================
// String Search
// ========================================

VM::Value runtime_starts_with(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] starts_with requires string and prefix\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::string prefix = valueToString(args[1]);
    
    bool result = str.length() >= prefix.length() &&
                  str.compare(0, prefix.length(), prefix) == 0;
    
    return VM::Value::Bool(result);
}

VM::Value runtime_ends_with(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] ends_with requires string and suffix\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::string suffix = valueToString(args[1]);
    
    bool result = str.length() >= suffix.length() &&
                  str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    
    return VM::Value::Bool(result);
}

VM::Value runtime_contains(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] contains requires string and substring\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::string substr = valueToString(args[1]);
    
    bool result = str.find(substr) != std::string::npos;
    return VM::Value::Bool(result);
}

VM::Value runtime_index_of(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] index_of requires string and substring\n";
        return VM::Value::Null();
    }
    
    std::string str = valueToString(args[0]);
    std::string substr = valueToString(args[1]);
    
    size_t pos = str.find(substr);
    if (pos == std::string::npos) {
        return VM::Value::Int(-1);
    }
    
    return VM::Value::Int(static_cast<int64_t>(pos));
}

// ========================================
// Registration Functions
// ========================================

void registerStringFunctions(VM::VirtualMachine* vm) {
    // Manipulation
    vm->registerNative("split", runtime_split);
    vm->registerNative("join", runtime_join);
    vm->registerNative("replace", runtime_replace);
    vm->registerNative("trim", runtime_trim);
    
    // Case conversion
    vm->registerNative("to_upper", runtime_to_upper);
    vm->registerNative("to_lower", runtime_to_lower);
    
    // Search
    vm->registerNative("starts_with", runtime_starts_with);
    vm->registerNative("ends_with", runtime_ends_with);
    vm->registerNative("contains", runtime_contains);
    vm->registerNative("index_of", runtime_index_of);
}

} // namespace Runtime
} // namespace Sad
