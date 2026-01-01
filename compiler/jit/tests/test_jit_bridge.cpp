// ============================================================================
// test_jit_bridge.cpp - اختبارات جسر التكامل
// JIT Bridge Integration Tests
// ============================================================================

#include "../include/jit_bridge.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

using namespace Sad::JIT;

// ============================================================================
// مساعدات الاختبار / Test Helpers
// ============================================================================

int test_count = 0;
int passed_count = 0;
int failed_count = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        test_count++; \
        std::cout << "Running: " << #name << "... "; \
        try { \
            test_##name(); \
            passed_count++; \
            std::cout << "✅ PASSED\n"; \
        } catch (const std::exception& e) { \
            failed_count++; \
            std::cout << "❌ FAILED: " << e.what() << "\n"; \
        } \
    } \
    void test_##name()

#define ASSERT(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { \
        throw std::runtime_error("Assertion failed: " #a " != " #b); \
    }

// ============================================================================
// الاختبارات / Tests
// ============================================================================

// اختبار الإنشاء والتهيئة / Test Creation and Initialization
TEST(bridge_initialization) {
    JITBridge bridge;
    ASSERT(!bridge.isInitialized());
    
    ASSERT(bridge.initialize());
    ASSERT(bridge.isInitialized());
}

// اختبار تسجيل دالة / Test Function Registration
TEST(bridge_register_function) {
    JITBridge bridge;
    bridge.initialize();
    
    SadFunction func;
    func.name = "test_func";
    func.source_code = "دالة test_func() { أرجع 42؛ }";
    func.return_type = "int";
    
    ASSERT(bridge.registerFunction(func));
    ASSERT_EQ(bridge.getRegisteredFunctionCount(), 1);
    
    const SadFunction* info = bridge.getFunctionInfo("test_func");
    ASSERT(info != nullptr);
    ASSERT_EQ(info->name, "test_func");
}

// اختبار تسجيل دالة أصلية / Test Native Function Registration
TEST(bridge_register_native) {
    JITBridge bridge;
    bridge.initialize();
    
    auto native_func = [](int64_t x) -> int64_t { return x * 2; };
    
    ASSERT(bridge.registerNativeFunction(
        "double_it",
        reinterpret_cast<void*>(+native_func),
        {"int"},
        "int"
    ));
    
    ASSERT_EQ(bridge.getRegisteredFunctionCount(), 1);
}

// اختبار إلغاء التسجيل / Test Unregister
TEST(bridge_unregister_function) {
    JITBridge bridge;
    bridge.initialize();
    
    SadFunction func;
    func.name = "temp_func";
    bridge.registerFunction(func);
    
    ASSERT_EQ(bridge.getRegisteredFunctionCount(), 1);
    ASSERT(bridge.unregisterFunction("temp_func"));
    ASSERT_EQ(bridge.getRegisteredFunctionCount(), 0);
}

// اختبار SadValue - النوع null / Test SadValue - null type
TEST(sad_value_null) {
    SadValue val = nullptr;
    ASSERT(std::holds_alternative<std::nullptr_t>(val));
    ASSERT_EQ(sadValueTypeName(val), "null");
}

// اختبار SadValue - النوع bool / Test SadValue - bool type
TEST(sad_value_bool) {
    SadValue val = true;
    ASSERT(std::holds_alternative<bool>(val));
    ASSERT_EQ(std::get<bool>(val), true);
    ASSERT_EQ(sadValueTypeName(val), "bool");
}

// اختبار SadValue - النوع int / Test SadValue - int type
TEST(sad_value_int) {
    SadValue val = static_cast<int64_t>(42);
    ASSERT(std::holds_alternative<int64_t>(val));
    ASSERT_EQ(std::get<int64_t>(val), 42);
    ASSERT_EQ(sadValueTypeName(val), "int");
}

// اختبار SadValue - النوع float / Test SadValue - float type
TEST(sad_value_float) {
    SadValue val = 3.14;
    ASSERT(std::holds_alternative<double>(val));
    ASSERT_EQ(std::get<double>(val), 3.14);
    ASSERT_EQ(sadValueTypeName(val), "float");
}

// اختبار SadValue - النوع string / Test SadValue - string type
TEST(sad_value_string) {
    SadValue val = std::string("hello");
    ASSERT(std::holds_alternative<std::string>(val));
    ASSERT_EQ(std::get<std::string>(val), "hello");
    ASSERT_EQ(sadValueTypeName(val), "string");
}

// اختبار تحويل SadValue إلى نص / Test SadValue to String
TEST(sad_value_to_string) {
    ASSERT_EQ(sadValueToString(nullptr), "null");
    ASSERT_EQ(sadValueToString(true), "true");
    ASSERT_EQ(sadValueToString(false), "false");
    ASSERT_EQ(sadValueToString(static_cast<int64_t>(42)), "42");
    ASSERT_EQ(sadValueToString(3.14), "3.14");
    ASSERT_EQ(sadValueToString(std::string("test")), "\"test\"");
}

// اختبار مقارنة SadValue / Test SadValue Equality
TEST(sad_value_equality) {
    SadValue a = static_cast<int64_t>(42);
    SadValue b = static_cast<int64_t>(42);
    SadValue c = static_cast<int64_t>(99);
    
    ASSERT(sadValuesEqual(a, b));
    ASSERT(!sadValuesEqual(a, c));
}

// اختبار وضع التنفيذ / Test Execution Mode
TEST(bridge_execution_mode) {
    JITBridge bridge;
    bridge.initialize();
    
    // الافتراضي: HYBRID / Default: HYBRID
    ASSERT_EQ(bridge.getConfig().execution_mode, ExecutionMode::HYBRID);
    
    // تغيير الوضع / Change mode
    bridge.setExecutionMode(ExecutionMode::JIT_ONLY);
    ASSERT_EQ(bridge.getConfig().execution_mode, ExecutionMode::JIT_ONLY);
    
    bridge.setExecutionMode(ExecutionMode::INTERPRETER_ONLY);
    ASSERT_EQ(bridge.getConfig().execution_mode, ExecutionMode::INTERPRETER_ONLY);
}

// اختبار منطق قرار JIT / Test JIT Decision Logic
TEST(bridge_should_use_jit) {
    JITBridgeConfig config;
    config.execution_mode = ExecutionMode::HYBRID;
    config.jit_threshold = 5;
    
    JITBridge bridge(config);
    bridge.initialize();
    
    SadFunction func;
    func.name = "loop_func";
    bridge.registerFunction(func);
    
    // أول 5 مرات: مفسر / First 5 times: interpreter
    for (int i = 0; i < 5; i++) {
        bridge.executeFunction("loop_func", {});
    }
    
    // بعد العتبة: JIT / After threshold: JIT
    // (في وضع HYBRID سيتحول تلقائياً) / (In HYBRID mode will switch automatically)
}

// اختبار الإحصائيات / Test Statistics
TEST(bridge_statistics) {
    JITBridge bridge;
    bridge.initialize();
    
    const auto& stats = bridge.getStatistics();
    
    // في البداية، كل شيء صفر / Initially, everything is zero
    ASSERT_EQ(stats.interpreted_calls, 0);
    ASSERT_EQ(stats.jit_calls, 0);
    ASSERT_EQ(stats.fallback_count, 0);
}

// اختبار إعادة تعيين الإحصائيات / Test Reset Statistics
TEST(bridge_reset_statistics) {
    JITBridge bridge;
    bridge.initialize();
    
    SadFunction func;
    func.name = "test";
    bridge.registerFunction(func);
    
    // تنفيذ بعض الاستدعاءات / Execute some calls
    bridge.executeFunction("test", {});
    bridge.executeFunction("test", {});
    
    // إعادة التعيين / Reset
    bridge.resetStatistics();
    
    const auto& stats = bridge.getStatistics();
    ASSERT_EQ(stats.interpreted_calls, 0);
    ASSERT_EQ(stats.jit_calls, 0);
}

// اختبار معالجة الأخطاء / Test Error Handling
TEST(bridge_error_handling) {
    JITBridge bridge;
    bridge.initialize();
    
    // محاولة تنفيذ دالة غير موجودة / Try to execute non-existent function
    auto result = bridge.executeFunction("non_existent", {});
    
    // يجب أن تُرجع null / Should return null
    ASSERT(std::holds_alternative<std::nullptr_t>(result));
    
    // يجب أن يكون هناك خطأ / Should have an error
    std::string error = bridge.getLastError();
    ASSERT(!error.empty());
}

// اختبار مسح الأخطاء / Test Clear Errors
TEST(bridge_clear_errors) {
    JITBridge bridge;
    bridge.initialize();
    
    bridge.executeFunction("non_existent", {});
    ASSERT(!bridge.getLastError().empty());
    
    bridge.clearErrors();
    ASSERT(bridge.getLastError().empty());
}

// اختبار الحصول على أسماء الدوال / Test Get Function Names
TEST(bridge_get_function_names) {
    JITBridge bridge;
    bridge.initialize();
    
    SadFunction func1, func2, func3;
    func1.name = "func1";
    func2.name = "func2";
    func3.name = "func3";
    
    bridge.registerFunction(func1);
    bridge.registerFunction(func2);
    bridge.registerFunction(func3);
    
    auto names = bridge.getRegisteredFunctionNames();
    ASSERT_EQ(names.size(), 3);
}

// اختبار الوصول إلى محرك JIT / Test JIT Engine Access
TEST(bridge_jit_engine_access) {
    JITBridge bridge;
    bridge.initialize();
    
    JITEngine* engine = bridge.getJITEngine();
    ASSERT(engine != nullptr);
}

// اختبار الإيقاف / Test Shutdown
TEST(bridge_shutdown) {
    JITBridge bridge;
    bridge.initialize();
    
    ASSERT(bridge.isInitialized());
    
    bridge.shutdown();
    ASSERT(!bridge.isInitialized());
}

// اختبار التزامن / Test Thread Safety
TEST(bridge_thread_safety) {
    JITBridge bridge;
    bridge.initialize();
    
    SadFunction func;
    func.name = "shared_func";
    bridge.registerFunction(func);
    
    // تنفيذ من عدة خيوط / Execute from multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&bridge]() {
            for (int j = 0; j < 50; j++) {
                bridge.executeFunction("shared_func", {});
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // يجب أن يكون كل شيء بخير / Everything should be fine
    ASSERT(bridge.isInitialized());
}

// اختبار تحديث الإعدادات / Test Config Update
TEST(bridge_update_config) {
    JITBridge bridge;
    bridge.initialize();
    
    JITBridgeConfig new_config;
    new_config.execution_mode = ExecutionMode::ADAPTIVE;
    new_config.jit_threshold = 20;
    
    bridge.updateConfig(new_config);
    
    const auto& config = bridge.getConfig();
    ASSERT_EQ(config.execution_mode, ExecutionMode::ADAPTIVE);
    ASSERT_EQ(config.jit_threshold, 20);
}

// ============================================================================
// تشغيل جميع الاختبارات / Run All Tests
// ============================================================================

int main() {
    std::cout << "\n========================================\n";
    std::cout << " JIT Bridge Tests\n";
    std::cout << " اختبارات جسر JIT\n";
    std::cout << "========================================\n\n";
    
    run_test_bridge_initialization();
    run_test_bridge_register_function();
    run_test_bridge_register_native();
    run_test_bridge_unregister_function();
    run_test_sad_value_null();
    run_test_sad_value_bool();
    run_test_sad_value_int();
    run_test_sad_value_float();
    run_test_sad_value_string();
    run_test_sad_value_to_string();
    run_test_sad_value_equality();
    run_test_bridge_execution_mode();
    run_test_bridge_should_use_jit();
    run_test_bridge_statistics();
    run_test_bridge_reset_statistics();
    run_test_bridge_error_handling();
    run_test_bridge_clear_errors();
    run_test_bridge_get_function_names();
    run_test_bridge_jit_engine_access();
    run_test_bridge_shutdown();
    run_test_bridge_thread_safety();
    run_test_bridge_update_config();
    
    std::cout << "\n========================================\n";
    std::cout << " Test Results / نتائج الاختبارات\n";
    std::cout << "========================================\n";
    std::cout << "Total Tests:   " << test_count << "\n";
    std::cout << "Passed:        " << passed_count << " ✅\n";
    std::cout << "Failed:        " << failed_count << " ❌\n";
    std::cout << "Success Rate:  " << (100.0 * passed_count / test_count) << "%\n";
    std::cout << "========================================\n\n";
    
    return failed_count == 0 ? 0 : 1;
}

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
