/**
 * @file test_c_interop.cpp
 * @brief (AR) اختبارات التكامل لنظام FFI للغة "ص"
 *        (EN) FFI Integration Tests for Sad Language
 * 
 * @details
 * (AR) هذا الملف يحتوي على اختبارات شاملة للتحقق من:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. استدعاء دوال C من كود ص                                 │
 *      │ 2. تحويل الأنواع بين ص و C                                 │
 *      │ 3. إدارة الذاكرة عبر FFI                                   │
 *      │ 4. الأغلفة الآمنة                                          │
 *      │ 5. كشف التسريبات                                           │
 *      │ 6. اصطلاحات الاستدعاء                                      │
 *      └────────────────────────────────────────────────────────────┘
 * 
 *      بنية الاختبارات:
 *      @code
 *      TEST_F(FFIIntegrationTest, الاسم_بالعربية) {
 *          // ترتيب - Arrange
 *          // تنفيذ - Act
 *          // تأكيد - Assert
 *      }
 *      @endcode
 * 
 * (EN) This file contains comprehensive tests to verify:
 *      1. Calling C functions from Sad code
 *      2. Type conversion between Sad and C
 *      3. Memory management across FFI
 *      4. Safe wrappers
 *      5. Leak detection
 *      6. Calling conventions
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <atomic>

// (AR) تضمين ملفات FFI الخاصة بلغة ص
// (EN) Include Sad language FFI headers
#include "ffi/c_abi.h"
#include "types/c_types.h"
#include "ffi/ffi_wrapper.h"

using namespace sad::ffi;
using namespace sad::types;

// ============================================================================
//                    (AR) دوال C للاختبار
//                    (EN) C Functions for Testing
// ============================================================================

extern "C" {
    /**
     * @brief (AR) دالة اختبار بسيطة - جمع عددين
     *        (EN) Simple test function - add two numbers
     */
    int32_t test_add(int32_t a, int32_t b) {
        return a + b;
    }
    
    /**
     * @brief (AR) دالة اختبار الأعداد العشرية
     *        (EN) Test floating-point function
     */
    double test_multiply(double a, double b) {
        return a * b;
    }
    
    /**
     * @brief (AR) دالة اختبار المؤشرات
     *        (EN) Test pointer function
     */
    void test_increment(int32_t* value) {
        if (value) {
            (*value)++;
        }
    }
    
    /**
     * @brief (AR) دالة اختبار حجز الذاكرة
     *        (EN) Test memory allocation function
     */
    void* test_alloc(size_t size) {
        return malloc(size);
    }
    
    /**
     * @brief (AR) دالة اختبار تحرير الذاكرة
     *        (EN) Test memory free function
     */
    void test_free(void* ptr) {
        free(ptr);
    }
    
    /**
     * @brief (AR) دالة اختبار النصوص
     *        (EN) Test string function
     */
    size_t test_strlen(const char* str) {
        return str ? strlen(str) : 0;
    }
    
    /**
     * @brief (AR) دالة اختبار النسخ
     *        (EN) Test copy function
     */
    char* test_strdup(const char* str) {
        if (!str) return nullptr;
        size_t len = strlen(str) + 1;
        char* copy = (char*)malloc(len);
        if (copy) {
            memcpy(copy, str, len);
        }
        return copy;
    }
    
    /**
     * @brief (AR) دالة اختبار البنيات
     *        (EN) Test struct function
     */
    typedef struct {
        int32_t x;
        int32_t y;
    } TestPoint;
    
    TestPoint test_create_point(int32_t x, int32_t y) {
        TestPoint p = {x, y};
        return p;
    }
    
    int32_t test_point_sum(TestPoint p) {
        return p.x + p.y;
    }
    
    /**
     * @brief (AR) دالة اختبار استدعاء الدالة العكسي
     *        (EN) Test callback function
     */
    typedef int32_t (*TestCallback)(int32_t);
    
    int32_t test_with_callback(TestCallback cb, int32_t value) {
        return cb ? cb(value) : 0;
    }
    
    /**
     * @brief (AR) دالة callback للاختبار
     *        (EN) Test callback implementation
     */
    int32_t double_value(int32_t x) {
        return x * 2;
    }
    
    /**
     * @brief (AR) دالة اختبار المصفوفات
     *        (EN) Test array function
     */
    int32_t test_array_sum(int32_t* arr, size_t len) {
        int32_t sum = 0;
        for (size_t i = 0; i < len; i++) {
            sum += arr[i];
        }
        return sum;
    }
    
    /**
     * @brief (AR) دالة اختبار variadic (متعددة الوسائط)
     *        (EN) Test variadic function
     */
    int32_t test_sum_variadic(int32_t count, ...) {
        va_list args;
        va_start(args, count);
        int32_t sum = 0;
        for (int32_t i = 0; i < count; i++) {
            sum += va_arg(args, int32_t);
        }
        va_end(args);
        return sum;
    }
}

// ============================================================================
//                    (AR) فئة الاختبار الأساسية
//                    (EN) Base Test Class
// ============================================================================

/**
 * @class FFIIntegrationTest
 * @brief (AR) فئة اختبارات التكامل لـ FFI
 *        (EN) FFI Integration Test Class
 * 
 * (AR) توفر هذه الفئة:
 *      - إعداد وتنظيف قبل/بعد كل اختبار
 *      - تتبع الذاكرة المحجوزة
 *      - أدوات مساعدة للتحقق
 */
class FFIIntegrationTest : public ::testing::Test {
protected:
    // (AR) محول الأنواع / (EN) Type converter
    std::unique_ptr<CTypeMapper> typeMapper;
    
    // (AR) منشئ ABI / (EN) ABI builder  
    std::unique_ptr<CABIBuilder> abiBuilder;
    
    // (AR) متتبع التخصيصات / (EN) Allocation tracker
    std::vector<void*> allocations;
    
    // (AR) إعداد قبل كل اختبار
    // (EN) Setup before each test
    void SetUp() override {
        typeMapper = std::make_unique<CTypeMapper>();
        abiBuilder = std::make_unique<CABIBuilder>();
        allocations.clear();
        
        // (AR) إعادة تعيين كاشف التسريب
        // (EN) Reset leak detector
        LeakDetector::getInstance().reset();
    }
    
    // (AR) تنظيف بعد كل اختبار
    // (EN) Cleanup after each test
    void TearDown() override {
        // (AR) تحرير جميع التخصيصات المتبقية
        // (EN) Free all remaining allocations
        for (void* ptr : allocations) {
            free(ptr);
        }
        allocations.clear();
        
        // (AR) التحقق من عدم وجود تسريبات
        // (EN) Verify no leaks
        auto report = LeakDetector::getInstance().getReport();
        EXPECT_EQ(report.leakCount, 0) 
            << "(AR) تم اكتشاف تسريب ذاكرة! / (EN) Memory leak detected!";
    }
    
    // (AR) حجز ذاكرة مع تتبع
    // (EN) Allocate with tracking
    void* trackedAlloc(size_t size) {
        void* ptr = malloc(size);
        if (ptr) {
            allocations.push_back(ptr);
            OwnershipTracker::getInstance().registerAllocation(ptr, size, __FUNCTION__);
        }
        return ptr;
    }
    
    // (AR) تحرير ذاكرة مع إزالة من التتبع
    // (EN) Free with removal from tracking
    void trackedFree(void* ptr) {
        auto it = std::find(allocations.begin(), allocations.end(), ptr);
        if (it != allocations.end()) {
            allocations.erase(it);
            OwnershipTracker::getInstance().registerDeallocation(ptr, __FUNCTION__);
        }
        free(ptr);
    }
};

// ============================================================================
//                    (AR) اختبارات استدعاء الدوال الأساسية
//                    (EN) Basic Function Call Tests
// ============================================================================

/**
 * @test اختبار_استدعاء_دالة_جمع
 * @brief (AR) اختبار استدعاء دالة C بسيطة تجمع عددين
 *        (EN) Test calling a simple C function that adds two numbers
 */
TEST_F(FFIIntegrationTest, استدعاء_دالة_جمع_بسيطة) {
    // (AR) ترتيب - تحديد القيم المدخلة
    // (EN) Arrange - define input values
    int32_t a = 5;
    int32_t b = 3;
    
    // (AR) تنفيذ - استدعاء الدالة
    // (EN) Act - call the function
    int32_t result = test_add(a, b);
    
    // (AR) تأكيد - التحقق من النتيجة
    // (EN) Assert - verify result
    EXPECT_EQ(result, 8);
}

/**
 * @test اختبار_الأعداد_العشرية
 * @brief (AR) اختبار تمرير وإرجاع الأعداد العشرية
 *        (EN) Test passing and returning floating-point numbers
 */
TEST_F(FFIIntegrationTest, تمرير_أعداد_عشرية) {
    // (AR) ترتيب
    double a = 3.14159;
    double b = 2.0;
    
    // (AR) تنفيذ
    double result = test_multiply(a, b);
    
    // (AR) تأكيد - مع هامش للأخطاء العشرية
    // (EN) Assert - with margin for floating-point errors
    EXPECT_NEAR(result, 6.28318, 0.00001);
}

/**
 * @test اختبار_القيم_السالبة
 * @brief (AR) اختبار التعامل مع الأرقام السالبة
 *        (EN) Test handling negative numbers
 */
TEST_F(FFIIntegrationTest, أرقام_سالبة) {
    EXPECT_EQ(test_add(-5, 3), -2);
    EXPECT_EQ(test_add(-5, -3), -8);
    EXPECT_EQ(test_add(5, -3), 2);
}

/**
 * @test اختبار_الحدود
 * @brief (AR) اختبار القيم القصوى
 *        (EN) Test boundary values
 */
TEST_F(FFIIntegrationTest, قيم_حدود) {
    // (AR) أكبر قيمة + 0
    EXPECT_EQ(test_add(INT32_MAX, 0), INT32_MAX);
    
    // (AR) أصغر قيمة + 0
    EXPECT_EQ(test_add(INT32_MIN, 0), INT32_MIN);
    
    // (AR) صفر + صفر
    EXPECT_EQ(test_add(0, 0), 0);
}

// ============================================================================
//                    (AR) اختبارات المؤشرات
//                    (EN) Pointer Tests
// ============================================================================

/**
 * @test اختبار_المؤشرات_المتغيرة
 * @brief (AR) اختبار تمرير مؤشر قابل للتعديل
 *        (EN) Test passing mutable pointer
 */
TEST_F(FFIIntegrationTest, مؤشر_متغير) {
    // (AR) ترتيب
    int32_t value = 10;
    
    // (AR) تنفيذ
    test_increment(&value);
    
    // (AR) تأكيد
    EXPECT_EQ(value, 11);
    
    // (AR) استدعاء متكرر
    test_increment(&value);
    test_increment(&value);
    EXPECT_EQ(value, 13);
}

/**
 * @test اختبار_المؤشر_الفارغ
 * @brief (AR) اختبار التعامل مع المؤشر null
 *        (EN) Test null pointer handling
 */
TEST_F(FFIIntegrationTest, مؤشر_فارغ) {
    // (AR) يجب ألا يحدث crash
    // (EN) Should not crash
    test_increment(nullptr);
    
    // (AR) نص فارغ
    EXPECT_EQ(test_strlen(nullptr), 0);
}

// ============================================================================
//                    (AR) اختبارات إدارة الذاكرة
//                    (EN) Memory Management Tests
// ============================================================================

/**
 * @test اختبار_حجز_ذاكرة_بسيط
 * @brief (AR) اختبار حجز وتحرير الذاكرة
 *        (EN) Test memory allocation and deallocation
 */
TEST_F(FFIIntegrationTest, حجز_ذاكرة_بسيط) {
    // (AR) حجز ذاكرة
    void* ptr = test_alloc(1024);
    ASSERT_NE(ptr, nullptr) << "(AR) فشل حجز الذاكرة";
    
    // (AR) التحقق من إمكانية الكتابة
    memset(ptr, 0, 1024);
    
    // (AR) تحرير
    test_free(ptr);
}

/**
 * @test اختبار_تسريب_الذاكرة
 * @brief (AR) اختبار كشف تسريب الذاكرة
 *        (EN) Test memory leak detection
 */
TEST_F(FFIIntegrationTest, كشف_تسريب_الذاكرة) {
    // (AR) حجز ذاكرة بدون تتبع (للاختبار فقط)
    void* ptr = test_alloc(512);
    ASSERT_NE(ptr, nullptr);
    
    // (AR) تسجيل التخصيص يدوياً
    OwnershipTracker::getInstance().registerAllocation(ptr, 512, "test_leak");
    
    // (AR) التحقق من وجود تخصيص نشط
    auto& tracker = OwnershipTracker::getInstance();
    EXPECT_TRUE(tracker.isAllocated(ptr));
    
    // (AR) تحرير صحيح
    OwnershipTracker::getInstance().registerDeallocation(ptr, "test_leak");
    test_free(ptr);
    
    EXPECT_FALSE(tracker.isAllocated(ptr));
}

/**
 * @test اختبار_الغلاف_الآمن
 * @brief (AR) اختبار SafePointerWrapper للتحرير التلقائي
 *        (EN) Test SafePointerWrapper for automatic cleanup
 */
TEST_F(FFIIntegrationTest, غلاف_المؤشر_الآمن) {
    // (AR) إنشاء غلاف آمن
    {
        auto wrapper = SafePointerWrapper<int32_t>::create(
            static_cast<int32_t*>(test_alloc(sizeof(int32_t) * 10)),
            [](int32_t* p) { test_free(p); }
        );
        
        ASSERT_TRUE(wrapper.isValid());
        
        // (AR) استخدام المؤشر
        wrapper.get()[0] = 42;
        EXPECT_EQ(wrapper.get()[0], 42);
        
        // (AR) عند الخروج من النطاق، يتم التحرير تلقائياً
    }
    
    // (AR) لا حاجة لتحرير يدوي - تم التحرير عند انتهاء النطاق
}

/**
 * @test اختبار_نقل_الملكية
 * @brief (AR) اختبار نقل ملكية المؤشر
 *        (EN) Test pointer ownership transfer
 */
TEST_F(FFIIntegrationTest, نقل_ملكية_المؤشر) {
    auto wrapper1 = SafePointerWrapper<char>::create(
        static_cast<char*>(test_alloc(100)),
        [](char* p) { test_free(p); }
    );
    
    ASSERT_TRUE(wrapper1.isValid());
    strcpy(wrapper1.get(), "Hello FFI");
    
    // (AR) نقل الملكية
    auto wrapper2 = std::move(wrapper1);
    
    // (AR) wrapper1 لم يعد صالحاً
    EXPECT_FALSE(wrapper1.isValid());
    
    // (AR) wrapper2 يحتوي على البيانات
    EXPECT_TRUE(wrapper2.isValid());
    EXPECT_STREQ(wrapper2.get(), "Hello FFI");
}

// ============================================================================
//                    (AR) اختبارات النصوص
//                    (EN) String Tests
// ============================================================================

/**
 * @test اختبار_طول_النص
 * @brief (AR) اختبار حساب طول النص
 *        (EN) Test string length calculation
 */
TEST_F(FFIIntegrationTest, طول_النص) {
    EXPECT_EQ(test_strlen(""), 0);
    EXPECT_EQ(test_strlen("Hello"), 5);
    EXPECT_EQ(test_strlen("مرحبا"), 10);  // (AR) UTF-8: 5 أحرف عربية × 2 بايت
}

/**
 * @test اختبار_نسخ_النص
 * @brief (AR) اختبار نسخ النص مع حجز ذاكرة
 *        (EN) Test string duplication with allocation
 */
TEST_F(FFIIntegrationTest, نسخ_النص) {
    const char* original = "Hello World";
    
    // (AR) نسخ
    char* copy = test_strdup(original);
    ASSERT_NE(copy, nullptr);
    
    // (AR) التحقق من المحتوى
    EXPECT_STREQ(copy, original);
    
    // (AR) التحقق من أنهما مؤشرات مختلفة
    EXPECT_NE(copy, original);
    
    // (AR) تحرير
    test_free(copy);
}

/**
 * @test اختبار_النص_العربي
 * @brief (AR) اختبار التعامل مع النصوص العربية UTF-8
 *        (EN) Test Arabic UTF-8 string handling
 */
TEST_F(FFIIntegrationTest, نص_عربي_UTF8) {
    const char* arabic = u8"مرحبا بالعالم";
    
    char* copy = test_strdup(arabic);
    ASSERT_NE(copy, nullptr);
    
    EXPECT_STREQ(copy, arabic);
    EXPECT_EQ(test_strlen(copy), strlen(arabic));
    
    test_free(copy);
}

// ============================================================================
//                    (AR) اختبارات البنيات
//                    (EN) Struct Tests
// ============================================================================

/**
 * @test اختبار_إنشاء_بنية
 * @brief (AR) اختبار إنشاء بنية من دالة C
 *        (EN) Test creating struct from C function
 */
TEST_F(FFIIntegrationTest, إنشاء_بنية) {
    // (AR) إنشاء نقطة
    TestPoint p = test_create_point(10, 20);
    
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

/**
 * @test اختبار_تمرير_بنية
 * @brief (AR) اختبار تمرير بنية لدالة C
 *        (EN) Test passing struct to C function
 */
TEST_F(FFIIntegrationTest, تمرير_بنية) {
    TestPoint p = {5, 7};
    
    int32_t sum = test_point_sum(p);
    
    EXPECT_EQ(sum, 12);
}

/**
 * @test اختبار_سلسلة_بنيات
 * @brief (AR) اختبار إنشاء وتمرير سلسلة من البنيات
 *        (EN) Test creating and passing chain of structs
 */
TEST_F(FFIIntegrationTest, سلسلة_بنيات) {
    TestPoint p1 = test_create_point(1, 2);
    TestPoint p2 = test_create_point(3, 4);
    TestPoint p3 = test_create_point(5, 6);
    
    int32_t total = test_point_sum(p1) + test_point_sum(p2) + test_point_sum(p3);
    
    EXPECT_EQ(total, 3 + 7 + 11);  // 21
}

// ============================================================================
//                    (AR) اختبارات الدوال العكسية (Callbacks)
//                    (EN) Callback Function Tests
// ============================================================================

/**
 * @test اختبار_استدعاء_عكسي
 * @brief (AR) اختبار تمرير مؤشر دالة لدالة C
 *        (EN) Test passing function pointer to C function
 */
TEST_F(FFIIntegrationTest, استدعاء_عكسي_بسيط) {
    int32_t result = test_with_callback(double_value, 5);
    
    EXPECT_EQ(result, 10);
}

/**
 * @test اختبار_استدعاء_عكسي_فارغ
 * @brief (AR) اختبار استدعاء عكسي null
 *        (EN) Test null callback
 */
TEST_F(FFIIntegrationTest, استدعاء_عكسي_فارغ) {
    int32_t result = test_with_callback(nullptr, 5);
    
    EXPECT_EQ(result, 0);
}

/**
 * @test اختبار_استدعاء_عكسي_لامدا
 * @brief (AR) اختبار استدعاء عكسي باستخدام دالة لامدا (C++11)
 *        (EN) Test callback with lambda function (C++11)
 * 
 * (AR) ملاحظة: لامدا بدون التقاط يمكن تحويلها لمؤشر دالة C
 */
TEST_F(FFIIntegrationTest, استدعاء_عكسي_لامدا) {
    // (AR) لامدا بدون التقاط
    auto triple = [](int32_t x) -> int32_t { return x * 3; };
    
    int32_t result = test_with_callback(triple, 7);
    
    EXPECT_EQ(result, 21);
}

// ============================================================================
//                    (AR) اختبارات المصفوفات
//                    (EN) Array Tests
// ============================================================================

/**
 * @test اختبار_مجموع_مصفوفة
 * @brief (AR) اختبار تمرير مصفوفة لدالة C
 *        (EN) Test passing array to C function
 */
TEST_F(FFIIntegrationTest, مجموع_مصفوفة) {
    int32_t arr[] = {1, 2, 3, 4, 5};
    
    int32_t sum = test_array_sum(arr, 5);
    
    EXPECT_EQ(sum, 15);
}

/**
 * @test اختبار_مصفوفة_ديناميكية
 * @brief (AR) اختبار مصفوفة محجوزة ديناميكياً
 *        (EN) Test dynamically allocated array
 */
TEST_F(FFIIntegrationTest, مصفوفة_ديناميكية) {
    size_t size = 100;
    int32_t* arr = static_cast<int32_t*>(test_alloc(size * sizeof(int32_t)));
    ASSERT_NE(arr, nullptr);
    
    // (AR) ملء المصفوفة
    for (size_t i = 0; i < size; i++) {
        arr[i] = static_cast<int32_t>(i + 1);
    }
    
    // (AR) حساب المجموع: 1+2+...+100 = 5050
    int32_t sum = test_array_sum(arr, size);
    EXPECT_EQ(sum, 5050);
    
    test_free(arr);
}

/**
 * @test اختبار_مصفوفة_فارغة
 * @brief (AR) اختبار مصفوفة بطول صفر
 *        (EN) Test zero-length array
 */
TEST_F(FFIIntegrationTest, مصفوفة_فارغة) {
    int32_t arr[] = {};
    
    int32_t sum = test_array_sum(arr, 0);
    
    EXPECT_EQ(sum, 0);
}

// ============================================================================
//                    (AR) اختبارات تحويل الأنواع
//                    (EN) Type Conversion Tests
// ============================================================================

/**
 * @test اختبار_تحويل_أنواع_أساسية
 * @brief (AR) اختبار تحويل الأنواع بين ص و C
 *        (EN) Test type conversion between Sad and C
 */
TEST_F(FFIIntegrationTest, تحويل_أنواع_أساسية) {
    // (AR) التحقق من تحويل الأنواع الصحيحة
    // (EN) Verify integer type conversions
    
    auto int8Info = typeMapper->sadToC("ص8");
    EXPECT_EQ(int8Info.cTypeName, "int8_t");
    EXPECT_EQ(int8Info.size, 1);
    
    auto int16Info = typeMapper->sadToC("ص16");
    EXPECT_EQ(int16Info.cTypeName, "int16_t");
    EXPECT_EQ(int16Info.size, 2);
    
    auto int32Info = typeMapper->sadToC("ص32");
    EXPECT_EQ(int32Info.cTypeName, "int32_t");
    EXPECT_EQ(int32Info.size, 4);
    
    auto int64Info = typeMapper->sadToC("ص64");
    EXPECT_EQ(int64Info.cTypeName, "int64_t");
    EXPECT_EQ(int64Info.size, 8);
}

/**
 * @test اختبار_تحويل_أنواع_عشرية
 * @brief (AR) اختبار تحويل الأنواع العشرية
 *        (EN) Test floating-point type conversions
 */
TEST_F(FFIIntegrationTest, تحويل_أنواع_عشرية) {
    auto float32Info = typeMapper->sadToC("عش32");
    EXPECT_EQ(float32Info.cTypeName, "float");
    EXPECT_EQ(float32Info.size, 4);
    
    auto float64Info = typeMapper->sadToC("عش64");
    EXPECT_EQ(float64Info.cTypeName, "double");
    EXPECT_EQ(float64Info.size, 8);
}

/**
 * @test اختبار_تحويل_عكسي
 * @brief (AR) اختبار التحويل من C إلى ص
 *        (EN) Test conversion from C to Sad
 */
TEST_F(FFIIntegrationTest, تحويل_من_C_إلى_ص) {
    EXPECT_EQ(typeMapper->cToSad("int8_t"), u8"ص8");
    EXPECT_EQ(typeMapper->cToSad("int16_t"), u8"ص16");
    EXPECT_EQ(typeMapper->cToSad("int32_t"), u8"ص32");
    EXPECT_EQ(typeMapper->cToSad("int64_t"), u8"ص64");
    EXPECT_EQ(typeMapper->cToSad("float"), u8"عش32");
    EXPECT_EQ(typeMapper->cToSad("double"), u8"عش64");
    EXPECT_EQ(typeMapper->cToSad("void"), u8"فراغ");
    EXPECT_EQ(typeMapper->cToSad("char"), u8"حرف");
    EXPECT_EQ(typeMapper->cToSad("_Bool"), u8"منطقي");
}

// ============================================================================
//                    (AR) اختبارات اصطلاحات الاستدعاء
//                    (EN) Calling Convention Tests
// ============================================================================

/**
 * @test اختبار_اصطلاح_CDECL
 * @brief (AR) اختبار اصطلاح استدعاء CDECL
 *        (EN) Test CDECL calling convention
 */
TEST_F(FFIIntegrationTest, اصطلاح_CDECL) {
    abiBuilder->setCallingConvention(CallingConvention::CDECL);
    
    // (AR) التحقق من الإعدادات
    auto info = abiBuilder->build("test_add");
    EXPECT_EQ(info.callingConvention, CallingConvention::CDECL);
}

/**
 * @test اختبار_محاذاة_المكدس
 * @brief (AR) اختبار محاذاة المكدس للمنصة
 *        (EN) Test stack alignment for platform
 */
TEST_F(FFIIntegrationTest, محاذاة_المكدس) {
#ifdef _WIN64
    // (AR) Windows x64: محاذاة 16 بايت
    EXPECT_EQ(abiBuilder->getStackAlignment(), 16);
#elif defined(__linux__) && defined(__x86_64__)
    // (AR) Linux x64 (SysV): محاذاة 16 بايت
    EXPECT_EQ(abiBuilder->getStackAlignment(), 16);
#endif
}

// ============================================================================
//                    (AR) اختبارات الأداء
//                    (EN) Performance Tests
// ============================================================================

/**
 * @test اختبار_أداء_الاستدعاء
 * @brief (AR) اختبار أداء استدعاء دالة C متكرر
 *        (EN) Test performance of repeated C function calls
 */
TEST_F(FFIIntegrationTest, أداء_استدعاء_متكرر) {
    const int iterations = 1000000;
    int32_t sum = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        sum += test_add(i, 1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // (AR) طباعة الوقت للتحليل
    std::cout << "(AR) وقت مليون استدعاء: " << duration.count() << " مايكروثانية" << std::endl;
    
    // (AR) التأكد من أن النتيجة صحيحة (لمنع التحسين من حذف الحلقة)
    EXPECT_GT(sum, 0);
    
    // (AR) التحقق من أن الأداء معقول (أقل من ثانية)
    EXPECT_LT(duration.count(), 1000000);
}

/**
 * @test اختبار_أداء_حجز_ذاكرة
 * @brief (AR) اختبار أداء حجز وتحرير الذاكرة
 *        (EN) Test memory allocation/deallocation performance
 */
TEST_F(FFIIntegrationTest, أداء_حجز_ذاكرة) {
    const int iterations = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        void* ptr = test_alloc(1024);
        ASSERT_NE(ptr, nullptr);
        test_free(ptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "(AR) وقت 10000 حجز/تحرير: " << duration.count() << " مايكروثانية" << std::endl;
    
    // (AR) أقل من ثانية
    EXPECT_LT(duration.count(), 1000000);
}

// ============================================================================
//                    (AR) اختبارات التزامن
//                    (EN) Concurrency Tests
// ============================================================================

/**
 * @test اختبار_استدعاء_متعدد_الخيوط
 * @brief (AR) اختبار استدعاء دوال C من خيوط متعددة
 *        (EN) Test calling C functions from multiple threads
 */
TEST_F(FFIIntegrationTest, استدعاء_متعدد_الخيوط) {
    const int numThreads = 4;
    const int iterationsPerThread = 10000;
    std::atomic<int32_t> totalSum{0};
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([&]() {
            int32_t localSum = 0;
            for (int i = 0; i < iterationsPerThread; i++) {
                localSum += test_add(1, 1);
            }
            totalSum += localSum;
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // (AR) كل استدعاء يرجع 2، مجموع = 4 * 10000 * 2 = 80000
    EXPECT_EQ(totalSum.load(), numThreads * iterationsPerThread * 2);
}

/**
 * @test اختبار_حجز_ذاكرة_متعدد_الخيوط
 * @brief (AR) اختبار حجز الذاكرة من خيوط متعددة
 *        (EN) Test memory allocation from multiple threads
 */
TEST_F(FFIIntegrationTest, حجز_ذاكرة_متعدد_الخيوط) {
    const int numThreads = 4;
    const int allocsPerThread = 100;
    std::atomic<int> successCount{0};
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([&]() {
            std::vector<void*> localAllocs;
            
            for (int i = 0; i < allocsPerThread; i++) {
                void* ptr = test_alloc(256);
                if (ptr) {
                    localAllocs.push_back(ptr);
                    successCount++;
                }
            }
            
            // (AR) تحرير
            for (void* ptr : localAllocs) {
                test_free(ptr);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // (AR) جميع التخصيصات يجب أن تنجح
    EXPECT_EQ(successCount.load(), numThreads * allocsPerThread);
}

// ============================================================================
//                    (AR) اختبارات الأخطاء والاستثناءات
//                    (EN) Error and Exception Tests
// ============================================================================

/**
 * @test اختبار_FFIResult_نجاح
 * @brief (AR) اختبار FFIResult مع نجاح
 *        (EN) Test FFIResult with success
 */
TEST_F(FFIIntegrationTest, FFIResult_نجاح) {
    FFIResult<int32_t> result = FFIResult<int32_t>::success(42);
    
    EXPECT_TRUE(result.isSuccess());
    EXPECT_FALSE(result.isError());
    EXPECT_EQ(result.getValue(), 42);
}

/**
 * @test اختبار_FFIResult_خطأ
 * @brief (AR) اختبار FFIResult مع خطأ
 *        (EN) Test FFIResult with error
 */
TEST_F(FFIIntegrationTest, FFIResult_خطأ) {
    FFIResult<int32_t> result = FFIResult<int32_t>::error(
        FFIError{FFIErrorCode::NULL_POINTER, u8"مؤشر فارغ", "test"}
    );
    
    EXPECT_FALSE(result.isSuccess());
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(result.getError().code, FFIErrorCode::NULL_POINTER);
}

// ============================================================================
//                    (AR) دالة main للاختبارات
//                    (EN) Main function for tests
// ============================================================================

int main(int argc, char** argv) {
    // (AR) تهيئة Google Test
    // (EN) Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << u8"═══════════════════════════════════════════════════════════" << std::endl;
    std::cout << u8"  اختبارات التكامل لنظام FFI للغة ص                          " << std::endl;
    std::cout << u8"  FFI Integration Tests for Sad Language                    " << std::endl;
    std::cout << u8"═══════════════════════════════════════════════════════════" << std::endl;
    
    // (AR) تشغيل جميع الاختبارات
    // (EN) Run all tests
    int result = RUN_ALL_TESTS();
    
    std::cout << u8"═══════════════════════════════════════════════════════════" << std::endl;
    
    return result;
}
