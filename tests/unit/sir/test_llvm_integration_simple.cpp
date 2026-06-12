// ============================================================================
// test_llvm_integration_simple.cpp - اختبار بسيط للتكامل مع LLVM
// Simple LLVM Integration Test
// ============================================================================
// الغرض: اختبار التكامل الأساسي بين JIT Engine و LLVM Backend
// Purpose: Test basic integration between JIT Engine and LLVM Backend
// ============================================================================

#include <iostream>
#include <cassert>
#include <string>

// تضمين محرك JIT / Include JIT engine
// المصدر: compiler/jit/include/jit_engine.h / Source: compiler/jit/include/jit_engine.h
#include "../../compiler/jit/include/jit_engine.h"

using namespace Sad::JIT;

// ============================================================================
// main - الدالة الرئيسية / Main Function
// ============================================================================

int main() {
    std::cout << "============================================\n";
    std::cout << "  اختبار تكامل LLVM مع JIT Engine\n";
    std::cout << "  LLVM Integration Test with JIT Engine\n";
    std::cout << "============================================\n\n";
    
    // ============================================================================
    // الخطوة 1: إنشاء JIT Engine / Step 1: Create JIT Engine
    // ============================================================================
    
    std::cout << "1️⃣  Creating JIT Engine...\n";
    
    // إنشاء إعدادات / Create configuration
    // المصدر: jit_engine.h:JITConfig / Source: jit_engine.h:JITConfig
    JITConfig config;
    config.optimization_level = 2;        // O2
    config.enable_cache = true;           // تفعيل الذاكرة المؤقتة / Enable cache
    config.enable_tiered_compilation = false; // تعطيل التجميع المُدرّج / Disable tiered compilation
    config.verbose = true;                // طباعة تفاصيل / Print details
    
    // إنشاء المحرك / Create engine
    // المصدر: jit_engine.h:JITEngine / Source: jit_engine.h:JITEngine
    JITEngine engine(config);
    
    std::cout << "   ✅ JIT Engine created\n\n";
    
    // ============================================================================
    // الخطوة 2: تهيئة المحرك / Step 2: Initialize Engine
    // ============================================================================
    
    std::cout << "2️⃣  Initializing JIT Engine...\n";
    
    // تهيئة / Initialize
    // المصدر: jit_engine.h:initialize() / Source: jit_engine.h:initialize()
    if (!engine.initialize()) {
        std::cerr << "   ❌ Failed to initialize JIT Engine\n";
        return 1;
    }
    
    std::cout << "   ✅ JIT Engine initialized\n\n";
    
    // ============================================================================
    // الخطوة 3: تجميع دالة بسيطة / Step 3: Compile Simple Function
    // ============================================================================
    
    std::cout << "3️⃣  Compiling simple function...\n";
    
    // كود بسيط: دالة تُرجع 42 / Simple code: function returns 42
    // ملاحظة: هذا كود بسيط للاختبار، ليس Sad code كامل
    // Note: This is simple test code, not complete Sad code
    std::string source_code = R"(
        دالة main() {
            رجع 42;
        }
    )";
    
    // تجميع / Compile
    // المصدر: jit_engine.h:compileFunction() / Source: jit_engine.h:compileFunction()
    auto result = engine.compileFunction("main", source_code);
    
    // التحقق من النتيجة / Check result
    if (result.success) {
        std::cout << "   ✅ Compilation succeeded!\n";
        std::cout << "      Function: " << result.function_name << "\n";
        std::cout << "      Optimization Level: O" << result.optimization_level << "\n";
        std::cout << "      Code Size: " << result.code_size_bytes << " bytes\n";
        std::cout << "      Compilation Time: " << result.compilation_time_ms << " ms\n";
        std::cout << "      From Cache: " << (result.was_cached ? "Yes" : "No") << "\n";
        
#ifdef ENABLE_LLVM_BACKEND
        std::cout << "      Backend: LLVM (Real Compilation) ✅\n";
#else
        std::cout << "      Backend: Simulation Mode ⚠️\n";
#endif
        
    } else {
        std::cout << "   ❌ Compilation failed!\n";
        std::cout << "      Error: " << result.error_message << "\n";
        return 1;
    }
    
    std::cout << "\n";
    
    // ============================================================================
    // الخطوة 4: التحقق من الدالة المُجمّعة / Step 4: Verify Compiled Function
    // ============================================================================
    
    std::cout << "4️⃣  Verifying compiled function...\n";
    
    // التحقق من وجود الدالة / Check if function exists
    // المصدر: jit_engine.h:isFunctionCompiled() / Source: jit_engine.h:isFunctionCompiled()
    if (engine.isFunctionCompiled("main")) {
        std::cout << "   ✅ Function 'main' is compiled\n";
    } else {
        std::cout << "   ❌ Function 'main' is NOT compiled\n";
        return 1;
    }
    
    std::cout << "\n";
    
    // ============================================================================
    // الخطوة 5: الإحصائيات / Step 5: Statistics
    // ============================================================================
    
    std::cout << "5️⃣  JIT Engine Statistics:\n";
    
    // الحصول على الإحصائيات / Get statistics
    // المصدر: jit_engine.h:getStatistics() / Source: jit_engine.h:getStatistics()
    const auto& stats = engine.getStatistics();
    
    std::cout << "   Total Compilations: " << stats.total_compilations << "\n";
    std::cout << "   Successful: " << stats.successful_compilations << "\n";
    std::cout << "   Failed: " << stats.failed_compilations << "\n";
    std::cout << "   Cache Hits: " << stats.cache_hits << "\n";
    std::cout << "   Cache Misses: " << stats.cache_misses << "\n";
    std::cout << "   Total Time: " << stats.total_compilation_time_ms << " ms\n";
    
    std::cout << "\n";
    
    // ============================================================================
    // الخطوة 6: اختبار الذاكرة المؤقتة / Step 6: Test Cache
    // ============================================================================
    
    std::cout << "6️⃣  Testing cache functionality...\n";
    
    // تجميع نفس الدالة مرة أخرى / Compile same function again
    auto result2 = engine.compileFunction("main", source_code);
    
    if (result2.success && result2.was_cached) {
        std::cout << "   ✅ Function retrieved from cache!\n";
        std::cout << "      Cache Hit: Yes ✅\n";
    } else if (result2.success) {
        std::cout << "   ⚠️  Function compiled again (not cached)\n";
        std::cout << "      Cache Hit: No ⚠️\n";
    } else {
        std::cout << "   ❌ Second compilation failed\n";
        return 1;
    }
    
    std::cout << "\n";
    
    // ============================================================================
    // النتيجة النهائية / Final Result
    // ============================================================================
    
    std::cout << "============================================\n";
    std::cout << "  ✅ جميع الاختبارات نجحت!\n";
    std::cout << "  ✅ All tests passed!\n";
    std::cout << "============================================\n";
    
#ifdef ENABLE_LLVM_BACKEND
    std::cout << "\n🎉 LLVM Backend is ACTIVE and WORKING! 🎉\n";
#else
    std::cout << "\n⚠️  Running in SIMULATION mode (LLVM disabled)\n";
#endif
    
    return 0;
}

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
