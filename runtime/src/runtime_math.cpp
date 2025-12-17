/**
 * @file runtime_math.cpp
 * @brief تطبيق الدوال الرياضية / Math Functions Implementation
 * @brief Implementation of trigonometry, logarithms, and math utilities
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/runtime.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>

namespace Sad {
namespace Runtime {

// ========================================
// Trigonometric Functions
// ========================================

VM::Value runtime_sin(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] sin requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::sin(x));
}

VM::Value runtime_cos(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] cos requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::cos(x));
}

VM::Value runtime_tan(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] tan requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::tan(x));
}

// ========================================
// Logarithmic Functions
// ========================================

VM::Value runtime_log(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] log requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    if (x <= 0.0) {
        std::cerr << "[Error] log domain error: x must be > 0\n";
        return VM::Value::Null();
    }
    
    return VM::Value::Float(std::log(x));
}

VM::Value runtime_log10(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] log10 requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    if (x <= 0.0) {
        std::cerr << "[Error] log10 domain error: x must be > 0\n";
        return VM::Value::Null();
    }
    
    return VM::Value::Float(std::log10(x));
}

VM::Value runtime_exp(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] exp requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::exp(x));
}

// ========================================
// Rounding Functions
// ========================================

VM::Value runtime_floor(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] floor requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::floor(x));
}

VM::Value runtime_ceil(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] ceil requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::ceil(x));
}

VM::Value runtime_round(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] round requires one argument\n";
        return VM::Value::Null();
    }
    
    double x = vm->toFloat(args[0]);
    return VM::Value::Float(std::round(x));
}

// ========================================
// Utility Functions
// ========================================

VM::Value runtime_min(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] min requires at least two arguments\n";
        return VM::Value::Null();
    }
    
    double minVal = vm->toFloat(args[0]);
    for (int i = 1; i < argc; ++i) {
        double val = vm->toFloat(args[i]);
        minVal = std::min(minVal, val);
    }
    
    return VM::Value::Float(minVal);
}

VM::Value runtime_max(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] max requires at least two arguments\n";
        return VM::Value::Null();
    }
    
    double maxVal = vm->toFloat(args[0]);
    for (int i = 1; i < argc; ++i) {
        double val = vm->toFloat(args[i]);
        maxVal = std::max(maxVal, val);
    }
    
    return VM::Value::Float(maxVal);
}

VM::Value runtime_random(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    if (argc == 0) {
        // (AR) رقم عشوائي بين 0 و 1
        // (EN) Random float between 0 and 1
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        return VM::Value::Float(dis(gen));
    } else if (argc == 1) {
        // (AR) رقم عشوائي بين 0 و max
        // (EN) Random integer between 0 and max
        int64_t max = vm->toInteger(args[0]);
        if (max <= 0) {
            std::cerr << "[Error] random max must be > 0\n";
            return VM::Value::Null();
        }
        std::uniform_int_distribution<int64_t> dis(0, max - 1);
        return VM::Value::Int(dis(gen));
    } else {
        // (AR) رقم عشوائي بين min و max
        // (EN) Random integer between min and max
        int64_t min = vm->toInteger(args[0]);
        int64_t max = vm->toInteger(args[1]);
        if (min >= max) {
            std::cerr << "[Error] random min must be < max\n";
            return VM::Value::Null();
        }
        std::uniform_int_distribution<int64_t> dis(min, max - 1);
        return VM::Value::Int(dis(gen));
    }
}

// ========================================
// Registration Functions
// ========================================

void registerMathFunctions(VM::VirtualMachine* vm) {
    // Trigonometric
    vm->registerNative("sin", runtime_sin);
    vm->registerNative("cos", runtime_cos);
    vm->registerNative("tan", runtime_tan);
    
    // Logarithmic
    vm->registerNative("log", runtime_log);
    vm->registerNative("log10", runtime_log10);
    vm->registerNative("exp", runtime_exp);
    
    // Rounding
    vm->registerNative("floor", runtime_floor);
    vm->registerNative("ceil", runtime_ceil);
    vm->registerNative("round", runtime_round);
    
    // Utilities
    vm->registerNative("min", runtime_min);
    vm->registerNative("max", runtime_max);
    vm->registerNative("random", runtime_random);
}

} // namespace Runtime
} // namespace Sad
