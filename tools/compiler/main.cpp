// بسم الله الرحمن الرحيم
// sadc - Sad Compiler Main Entry Point
// نقطة الدخول الرئيسية لمترجم Sad
// Phase 7.2: Compiler Driver

#include "compiler_driver.h"
#include <iostream>
#include <cstdlib>

/**
 * @brief Main entry point / نقطة الدخول الرئيسية
 * 
 * (AR) برنامج sadc - المترجم الرئيسي للغة Sad
 * 
 * أمثلة:
 * - sadc program.s -o program
 * - sadc program.s -O3 -o program_optimized
 * - sadc program.s --emit-llvm -o program.ll
 * - sadc program.s -c -o program.o
 * - sadc *.o -o program
 * 
 * (EN) sadc program - Main Sad language compiler
 * 
 * Examples:
 * - sadc program.s -o program
 * - sadc program.s -O3 -o program_optimized
 * - sadc program.s --emit-llvm -o program.ll
 * - sadc program.s -c -o program.o
 * - sadc *.o -o program
 */
int main(int argc, char* argv[]) {
    // Create compiler driver
    sad::driver::CompilerDriver driver;
    
    // Run compilation
    int exit_code = driver.run(argc, argv);
    
    // Return exit code
    return exit_code;
}
