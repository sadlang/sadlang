/**
 * @file test_array_map.cpp
 * @brief (AR) اختبار شامل لأنواع المصفوفات والقواميس
 * @brief (EN) Comprehensive test for Array and Map types
 * 
 * يختبر جميع عمليات المصفوفات والقواميس:
 * Tests all Array and Map operations:
 * - Array creation and indexing
 * - Array operations (push, pop, size, clear)
 * - Map creation and key access
 * - Map operations (hasKey, remove, keys, values)
 * - Mixed operations
 * - Error handling
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "data/types/value.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

using namespace Sad::Data;

// ============================================================================
// Test Helpers / مساعدات الاختبار
// ============================================================================

int g_testsRun = 0;
int g_testsPassed = 0;
int g_testsFailed = 0;

void TEST_START(const std::string& testName) {
    g_testsRun++;
    std::cout << "  Testing: " << testName << " ... ";
}

void TEST_PASS() {
    g_testsPassed++;
    std::cout << "✅ PASS\n";
}

void TEST_FAIL(const std::string& reason) {
    g_testsFailed++;
    std::cout << "❌ FAIL: " << reason << "\n";
}

// ============================================================================
// Array Creation Tests / اختبارات إنشاء المصفوفات
// ============================================================================

void test_create_empty_array() {
    TEST_START("test_create_empty_array");
    
    Value::ArrayType arr;
    Value v(arr);
    
    assert(v.isArray());
    assert(!v.isMap());
    assert(v.size() == 0);
    assert(v.isEmpty());
    assert(v.getType() == ::Sad::Types::SadTypeKind::Array);
    
    TEST_PASS();
}

void test_create_array_with_integers() {
    TEST_START("test_create_array_with_integers");
    
    Value::ArrayType arr = { Value(1), Value(2), Value(3), Value(4), Value(5) };
    Value v(arr);
    
    assert(v.isArray());
    assert(v.size() == 5);
    assert(!v.isEmpty());
    
    TEST_PASS();
}

void test_create_array_with_mixed_types() {
    TEST_START("test_create_array_with_mixed_types");
    
    Value::ArrayType arr = {
        Value(42),
        Value(3.14),
        Value("hello"),
        Value(true)
    };
    Value v(arr);
    
    assert(v.isArray());
    assert(v.size() == 4);
    
    TEST_PASS();
}

void test_create_nested_array() {
    TEST_START("test_create_nested_array");
    
    Value::ArrayType inner = { Value(1), Value(2) };
    Value::ArrayType outer = { Value(inner), Value(inner) };
    Value v(outer);
    
    assert(v.isArray());
    assert(v.size() == 2);
    
    TEST_PASS();
}

// ============================================================================
// Array Indexing Tests / اختبارات فهرسة المصفوفات
// ============================================================================

void test_array_indexing() {
    TEST_START("test_array_indexing");
    
    Value::ArrayType arr = { Value(10), Value(20), Value(30) };
    Value v(arr);
    
    assert(v[0].toInt() == 10);
    assert(v[1].toInt() == 20);
    assert(v[2].toInt() == 30);
    
    TEST_PASS();
}

void test_array_indexing_modification() {
    TEST_START("test_array_indexing_modification");
    
    Value::ArrayType arr = { Value(1), Value(2), Value(3) };
    Value v(arr);
    
    v[0] = Value(100);
    v[1] = Value(200);
    
    assert(v[0].toInt() == 100);
    assert(v[1].toInt() == 200);
    assert(v[2].toInt() == 3);
    
    TEST_PASS();
}

void test_array_out_of_bounds() {
    TEST_START("test_array_out_of_bounds");
    
    Value::ArrayType arr = { Value(1), Value(2) };
    Value v(arr);
    
    try {
        Value x = v[10];  // Out of bounds
        TEST_FAIL("Should throw out_of_range");
    } catch (const std::out_of_range&) {
        TEST_PASS();
    }
}

// ============================================================================
// Array Operations Tests / اختبارات عمليات المصفوفات
// ============================================================================

void test_array_push() {
    TEST_START("test_array_push");
    
    Value::ArrayType arr;
    Value v(arr);
    
    assert(v.size() == 0);
    
    v.push(Value(10));
    assert(v.size() == 1);
    assert(v[0].toInt() == 10);
    
    v.push(Value(20));
    assert(v.size() == 2);
    assert(v[1].toInt() == 20);
    
    v.push(Value(30));
    assert(v.size() == 3);
    assert(v[2].toInt() == 30);
    
    TEST_PASS();
}

void test_array_pop() {
    TEST_START("test_array_pop");
    
    Value::ArrayType arr = { Value(1), Value(2), Value(3) };
    Value v(arr);
    
    assert(v.size() == 3);
    
    Value last = v.pop();
    assert(last.toInt() == 3);
    assert(v.size() == 2);
    
    last = v.pop();
    assert(last.toInt() == 2);
    assert(v.size() == 1);
    
    TEST_PASS();
}

void test_array_pop_empty() {
    TEST_START("test_array_pop_empty");
    
    Value::ArrayType arr;
    Value v(arr);
    
    try {
        v.pop();
        TEST_FAIL("Should throw error on empty array");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

void test_array_clear() {
    TEST_START("test_array_clear");
    
    Value::ArrayType arr = { Value(1), Value(2), Value(3), Value(4) };
    Value v(arr);
    
    assert(v.size() == 4);
    assert(!v.isEmpty());
    
    v.clear();
    
    assert(v.size() == 0);
    assert(v.isEmpty());
    
    TEST_PASS();
}

void test_array_append_alias() {
    TEST_START("test_array_append_alias");
    
    Value::ArrayType arr;
    Value v(arr);
    
    v.append(Value(100));  // append is alias for push
    assert(v.size() == 1);
    assert(v[0].toInt() == 100);
    
    TEST_PASS();
}

// ============================================================================
// Map Creation Tests / اختبارات إنشاء القواميس
// ============================================================================

void test_create_empty_map() {
    TEST_START("test_create_empty_map");
    
    Value::MapType map;
    Value v(map);
    
    assert(v.isMap());
    assert(!v.isArray());
    assert(v.size() == 0);
    assert(v.isEmpty());
    assert(v.getType() == ::Sad::Types::SadTypeKind::Map);
    
    TEST_PASS();
}

void test_create_map_with_data() {
    TEST_START("test_create_map_with_data");
    
    Value::MapType map = {
        {"name", Value("Ahmed")},
        {"age", Value(25)},
        {"height", Value(1.75)}
    };
    Value v(map);
    
    assert(v.isMap());
    assert(v.size() == 3);
    assert(!v.isEmpty());
    
    TEST_PASS();
}

void test_create_nested_map() {
    TEST_START("test_create_nested_map");
    
    Value::MapType inner = {
        {"x", Value(10)},
        {"y", Value(20)}
    };
    
    Value::MapType outer = {
        {"point", Value(inner)},
        {"name", Value("origin")}
    };
    
    Value v(outer);
    assert(v.isMap());
    assert(v.size() == 2);
    
    TEST_PASS();
}

// ============================================================================
// Map Access Tests / اختبارات الوصول للقواميس
// ============================================================================

void test_map_key_access() {
    TEST_START("test_map_key_access");
    
    Value::MapType map = {
        {"name", Value("Sara")},
        {"age", Value(30)},
        {"city", Value("Cairo")}
    };
    Value v(map);
    
    assert(v["name"].toString() == "Sara");
    assert(v["age"].toInt() == 30);
    assert(v["city"].toString() == "Cairo");
    
    TEST_PASS();
}

void test_map_key_modification() {
    TEST_START("test_map_key_modification");
    
    Value::MapType map = {
        {"counter", Value(0)},
        {"status", Value("inactive")}
    };
    Value v(map);
    
    v["counter"] = Value(100);
    v["status"] = Value("active");
    
    assert(v["counter"].toInt() == 100);
    assert(v["status"].toString() == "active");
    
    TEST_PASS();
}

void test_map_key_not_found() {
    TEST_START("test_map_key_not_found");
    
    Value::MapType map = {
        {"existing", Value(42)}
    };
    Value v(map);
    
    try {
        const Value& cv = v;
        Value x = cv["nonexistent"];
        TEST_FAIL("Should throw out_of_range");
    } catch (const std::out_of_range&) {
        TEST_PASS();
    }
}

// ============================================================================
// Map Operations Tests / اختبارات عمليات القواميس
// ============================================================================

void test_map_has_key() {
    TEST_START("test_map_has_key");
    
    Value::MapType map = {
        {"key1", Value(10)},
        {"key2", Value(20)}
    };
    Value v(map);
    
    assert(v.hasKey("key1"));
    assert(v.hasKey("key2"));
    assert(!v.hasKey("key3"));
    assert(!v.hasKey("nonexistent"));
    
    TEST_PASS();
}

void test_map_remove() {
    TEST_START("test_map_remove");
    
    Value::MapType map = {
        {"a", Value(1)},
        {"b", Value(2)},
        {"c", Value(3)}
    };
    Value v(map);
    
    assert(v.size() == 3);
    assert(v.hasKey("b"));
    
    bool removed = v.remove("b");
    assert(removed);
    assert(v.size() == 2);
    assert(!v.hasKey("b"));
    
    // Remove non-existent
    removed = v.remove("nonexistent");
    assert(!removed);
    assert(v.size() == 2);
    
    TEST_PASS();
}

void test_map_keys() {
    TEST_START("test_map_keys");
    
    Value::MapType map = {
        {"name", Value("Test")},
        {"age", Value(25)},
        {"city", Value("Cairo")}
    };
    Value v(map);
    
    auto keys = v.keys();
    assert(keys.size() == 3);
    
    // Check all keys exist
    bool hasName = false, hasAge = false, hasCity = false;
    for (const auto& key : keys) {
        if (key == "name") hasName = true;
        if (key == "age") hasAge = true;
        if (key == "city") hasCity = true;
    }
    
    assert(hasName && hasAge && hasCity);
    
    TEST_PASS();
}

void test_map_values() {
    TEST_START("test_map_values");
    
    Value::MapType map = {
        {"x", Value(10)},
        {"y", Value(20)},
        {"z", Value(30)}
    };
    Value v(map);
    
    auto values = v.values();
    assert(values.size() == 3);
    
    // Check values sum
    int sum = 0;
    for (const auto& val : values) {
        sum += val.toInt();
    }
    assert(sum == 60);  // 10 + 20 + 30
    
    TEST_PASS();
}

void test_map_clear() {
    TEST_START("test_map_clear");
    
    Value::MapType map = {
        {"a", Value(1)},
        {"b", Value(2)},
        {"c", Value(3)}
    };
    Value v(map);
    
    assert(v.size() == 3);
    assert(!v.isEmpty());
    
    v.clear();
    
    assert(v.size() == 0);
    assert(v.isEmpty());
    
    TEST_PASS();
}

// ============================================================================
// Type Conversion Tests / اختبارات تحويل الأنواع
// ============================================================================

void test_array_toString() {
    TEST_START("test_array_toString");
    
    Value::ArrayType arr = { Value(1), Value(2), Value(3) };
    Value v(arr);
    
    std::string str = v.toString();
    assert(str == "[1, 2, 3]");
    
    TEST_PASS();
}

void test_map_toString() {
    TEST_START("test_map_toString");
    
    Value::MapType map = {
        {"x", Value(10)},
        {"y", Value(20)}
    };
    Value v(map);
    
    std::string str = v.toString();
    // Note: unordered_map order is not guaranteed
    assert(str.find("x: 10") != std::string::npos);
    assert(str.find("y: 20") != std::string::npos);
    
    TEST_PASS();
}

void test_array_toBool() {
    TEST_START("test_array_toBool");
    
    Value::ArrayType empty;
    Value v1(empty);
    assert(v1.toBool() == false);  // Empty array is false
    
    Value::ArrayType nonEmpty = { Value(1) };
    Value v2(nonEmpty);
    assert(v2.toBool() == true);   // Non-empty array is true
    
    TEST_PASS();
}

void test_map_toBool() {
    TEST_START("test_map_toBool");
    
    Value::MapType empty;
    Value v1(empty);
    assert(v1.toBool() == false);  // Empty map is false
    
    Value::MapType nonEmpty = {{"key", Value(1)}};
    Value v2(nonEmpty);
    assert(v2.toBool() == true);   // Non-empty map is true
    
    TEST_PASS();
}

void test_array_toArray() {
    TEST_START("test_array_toArray");
    
    Value::ArrayType arr = { Value(1), Value(2) };
    Value v(arr);
    
    Value::ArrayType result = v.toArray();
    assert(result.size() == 2);
    assert(result[0].toInt() == 1);
    assert(result[1].toInt() == 2);
    
    TEST_PASS();
}

void test_map_toMap() {
    TEST_START("test_map_toMap");
    
    Value::MapType map = {{"key", Value(42)}};
    Value v(map);
    
    Value::MapType result = v.toMap();
    assert(result.size() == 1);
    assert(result["key"].toInt() == 42);
    
    TEST_PASS();
}

// ============================================================================
// Error Handling Tests / اختبارات معالجة الأخطاء
// ============================================================================

void test_cannot_index_non_array() {
    TEST_START("test_cannot_index_non_array");
    
    Value v(42);  // Integer, not array
    
    try {
        Value x = v[0];
        TEST_FAIL("Should throw error");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

void test_cannot_key_access_non_map() {
    TEST_START("test_cannot_key_access_non_map");
    
    Value v("hello");  // String, not map
    
    try {
        Value x = v["key"];
        TEST_FAIL("Should throw error");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

void test_cannot_push_to_non_array() {
    TEST_START("test_cannot_push_to_non_array");
    
    Value v(100);
    
    try {
        v.push(Value(1));
        TEST_FAIL("Should throw error");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

void test_cannot_hasKey_on_non_map() {
    TEST_START("test_cannot_hasKey_on_non_map");
    
    Value v(3.14);
    
    try {
        v.hasKey("key");
        TEST_FAIL("Should throw error");
    } catch (const std::runtime_error&) {
        TEST_PASS();
    }
}

// ============================================================================
// Mixed Operations Tests / اختبارات العمليات المختلطة
// ============================================================================

void test_array_of_maps() {
    TEST_START("test_array_of_maps");
    
    Value::MapType map1 = {{"name", Value("Alice")}, {"age", Value(25)}};
    Value::MapType map2 = {{"name", Value("Bob")}, {"age", Value(30)}};
    
    Value::ArrayType arr = { Value(map1), Value(map2) };
    Value v(arr);
    
    assert(v.isArray());
    assert(v.size() == 2);
    assert(v[0].isMap());
    assert(v[1].isMap());
    
    TEST_PASS();
}

void test_map_of_arrays() {
    TEST_START("test_map_of_arrays");
    
    Value::ArrayType arr1 = { Value(1), Value(2), Value(3) };
    Value::ArrayType arr2 = { Value(4), Value(5), Value(6) };
    
    Value::MapType map = {
        {"numbers1", Value(arr1)},
        {"numbers2", Value(arr2)}
    };
    Value v(map);
    
    assert(v.isMap());
    assert(v.size() == 2);
    assert(v["numbers1"].isArray());
    assert(v["numbers2"].isArray());
    
    TEST_PASS();
}

void test_complex_nested_structure() {
    TEST_START("test_complex_nested_structure");
    
    // Create person object
    Value::MapType person = {
        {"name", Value("Ahmed")},
        {"age", Value(28)},
        {"hobbies", Value(Value::ArrayType{
            Value("reading"),
            Value("coding"),
            Value("gaming")
        })}
    };
    
    Value v(person);
    
    assert(v.isMap());
    assert(v["name"].toString() == "Ahmed");
    assert(v["age"].toInt() == 28);
    assert(v["hobbies"].isArray());
    assert(v["hobbies"].size() == 3);
    
    TEST_PASS();
}

// ============================================================================
// Main Test Runner / منفذ الاختبارات الرئيسي
// ============================================================================

int main() {
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  ARRAY/MAP TYPES COMPREHENSIVE TEST SUITE       ║\n";
    std::cout << "║  (AR) مجموعة اختبار شاملة للمصفوفات والقواميس  ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "=== Array Creation ===\n";
    test_create_empty_array();
    test_create_array_with_integers();
    test_create_array_with_mixed_types();
    test_create_nested_array();
    
    std::cout << "\n=== Array Indexing ===\n";
    test_array_indexing();
    test_array_indexing_modification();
    test_array_out_of_bounds();
    
    std::cout << "\n=== Array Operations ===\n";
    test_array_push();
    test_array_pop();
    test_array_pop_empty();
    test_array_clear();
    test_array_append_alias();
    
    std::cout << "\n=== Map Creation ===\n";
    test_create_empty_map();
    test_create_map_with_data();
    test_create_nested_map();
    
    std::cout << "\n=== Map Access ===\n";
    test_map_key_access();
    test_map_key_modification();
    test_map_key_not_found();
    
    std::cout << "\n=== Map Operations ===\n";
    test_map_has_key();
    test_map_remove();
    test_map_keys();
    test_map_values();
    test_map_clear();
    
    std::cout << "\n=== Type Conversion ===\n";
    test_array_toString();
    test_map_toString();
    test_array_toBool();
    test_map_toBool();
    test_array_toArray();
    test_map_toMap();
    
    std::cout << "\n=== Error Handling ===\n";
    test_cannot_index_non_array();
    test_cannot_key_access_non_map();
    test_cannot_push_to_non_array();
    test_cannot_hasKey_on_non_map();
    
    std::cout << "\n=== Mixed Operations ===\n";
    test_array_of_maps();
    test_map_of_arrays();
    test_complex_nested_structure();
    
    // Print summary
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    if (g_testsFailed == 0) {
        std::cout << "║   ✅ ALL TESTS PASSED!                            ║\n";
        std::cout << "║   ✅ جميع الاختبارات نجحت!                       ║\n";
    } else {
        std::cout << "║   ❌ SOME TESTS FAILED!                           ║\n";
        std::cout << "║   ❌ بعض الاختبارات فشلت!                        ║\n";
    }
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 Results:\n";
    std::cout << "  ✅ Passed: " << g_testsPassed << "\n";
    std::cout << "  ❌ Failed: " << g_testsFailed << "\n";
    std::cout << "  📈 Total: " << g_testsRun << "\n";
    
    if (g_testsRun > 0) {
        double successRate = (g_testsPassed * 100.0) / g_testsRun;
        std::cout << "  📊 Success Rate: " << successRate << "%\n";
    }
    
    return (g_testsFailed == 0) ? 0 : 1;
}
