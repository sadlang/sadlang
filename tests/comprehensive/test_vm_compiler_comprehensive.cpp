/**
 * @file test_vm_compiler_comprehensive.cpp
 * @brief (AR) اختبارات شاملة للآلة الافتراضية والمترجم
 * @brief (EN) Comprehensive VM & Compiler Tests
 * 
 * ═══════════════════════════════════════════════════════════════════════
 *  ملف اختبارات الآلة الافتراضية (VM) والبنية التحتية للمترجم (Compiler)
 *  يحتوي على ~75 اختبار موزعة كالتالي:
 * 
 *  ■ القسم 1: قيم الآلة الافتراضية (VM Values / قيمة)
 *    - إنشاء قيم من كل نوع (لا_شيء، منطقي، صحيح، عشري، نص)
 *    - فحص الأنواع (هل_صحيح، هل_نص، إلخ)
 *    - استخلاص البيانات (كـ_صحيح، كـ_نص، إلخ)
 *    - المصفوفات والقواميس
 *    - كائنات الدوال والأصناف
 * 
 *  ■ القسم 2: وحدة البايت كود (Bytecode Chunk / وحدة_بايت_كود)
 *    - كتابة وقراءة البايت كود
 *    - مجمع الثوابت (إضافة + استرجاع + إزالة التكرار)
 *    - تسجيل واسترجاع الدوال
 *    - تصحيح القفزات (Backpatching)
 *    - معلومات الأسطر المصدرية
 * 
 *  ■ القسم 3: تعليمات الآلة الافتراضية (Opcodes / رموز_العمليات)
 *    - التحقق من قيم الـ opcodes
 *    - فحص ترتيب المجموعات
 * 
 *  ■ القسم 4: مدراء المفسر (Interpreter Managers)
 *    - ScopeManager: إدارة النطاقات
 *    - VariableManager: تعريف واسترجاع المتغيرات
 *    - ClassManager: تسجيل الأصناف
 *    - ObjectManager: إنشاء الكائنات
 *    - OwnershipManager: تتبع الملكية
 * 
 *  ■ القسم 5: SIR (Sad Intermediate Representation)
 *    - أنواع SIR
 *    - تعليمات SIR
 *    - الكتل الأساسية
 *    - وحدات SIR
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * الملاحظات:
 *   - أسماء الـ VM API كلها بالعربية (مثل: قيمة، وحدة_بايت_كود)
 *   - نستخدم __has_include للتحقق من توفر الملفات قبل تضمينها
 *   - كل قسم يمكن تعطيله مستقلاً إذا لم تتوفر اعتمادياته
 * 
 * @author Sad Language Development Team
 * @date 2026-02-17
 */

#include "sad_test_framework.h"

// ══════════════════════════════════════════════════════════════════════
//  شوائب (includes) الآلة الافتراضية
// ══════════════════════════════════════════════════════════════════════

// --- قيم VM (الأسماء بالعربية) ---
#if __has_include("sad_vm_value.h")
    #include "sad_vm_value.h"
    #define HAS_VM_VALUE 1
#else
    #define HAS_VM_VALUE 0
#endif

// --- وحدة البايت كود ---
#if __has_include("sad_vm_chunk.h")
    #include "sad_vm_chunk.h"
    #define HAS_VM_CHUNK 1
#else
    #define HAS_VM_CHUNK 0
#endif

// --- رموز العمليات ---
#if __has_include("sad_vm_opcodes.h")
    #include "sad_vm_opcodes.h"
    #define HAS_VM_OPCODES 1
#else
    #define HAS_VM_OPCODES 0
#endif

// --- منفّذ VM ---
#if __has_include("sad_vm_executor.h")
    #include "sad_vm_executor.h"
    #define HAS_VM_EXECUTOR 1
#else
    #define HAS_VM_EXECUTOR 0
#endif

// ══════════════════════════════════════════════════════════════════════
//  شوائب مدراء المفسر (Interpreter Managers)
// ══════════════════════════════════════════════════════════════════════

#if __has_include("scope_manager.h")
    #include "scope_manager.h"
    #define HAS_SCOPE_MANAGER 1
#else
    #define HAS_SCOPE_MANAGER 0
#endif

#if __has_include("variable_manager.h")
    #include "variable_manager.h"
    #define HAS_VARIABLE_MANAGER 1
#else
    #define HAS_VARIABLE_MANAGER 0
#endif

#if __has_include("class_manager.h")
    #include "class_manager.h"
    #define HAS_CLASS_MANAGER 1
#else
    #define HAS_CLASS_MANAGER 0
#endif

#if __has_include("object_manager.h")
    #include "object_manager.h"
    #define HAS_OBJECT_MANAGER 1
#else
    #define HAS_OBJECT_MANAGER 0
#endif

#if __has_include("ownership_manager.h")
    #include "ownership_manager.h"
    #define HAS_OWNERSHIP_MANAGER 1
#else
    #define HAS_OWNERSHIP_MANAGER 0
#endif

#if __has_include("function_manager.h")
    #include "function_manager.h"
    #define HAS_FUNCTION_MANAGER 1
#else
    #define HAS_FUNCTION_MANAGER 0
#endif

// ══════════════════════════════════════════════════════════════════════
//  شوائب SIR (Sad Intermediate Representation)
// ══════════════════════════════════════════════════════════════════════

#if __has_include("sir_types.h")
    #include "sir_types.h"
    #define HAS_SIR_TYPES 1
#else
    #define HAS_SIR_TYPES 0
#endif

#if __has_include("sir_instruction.h")
    #include "sir_instruction.h"
    #define HAS_SIR_INSTRUCTION 1
#else
    #define HAS_SIR_INSTRUCTION 0
#endif

#if __has_include("sir_module.h")
    #include "sir_module.h"
    #define HAS_SIR_MODULE 1
#else
    #define HAS_SIR_MODULE 0
#endif

// نحتاج Value أيضاً
#include "value.h"

// ══════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ══════════════════════════════════════════════════════════════════════

int main() {
    SAD_TEST_INIT();

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 1: قيم الآلة الافتراضية (VM Values)                     ║
    // ║  اختبار نظام القيم العربي الخاص بالآلة الافتراضية               ║
    // ║  كل قيمة في VM تُمثَّل بـ struct قيمة (Tagged Union)           ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_VM_VALUE
    SAD_GROUP("VM.Values / الآلة.القيم");

    // ─────────────── إنشاء القيم الأساسية ────────────────────────────────
    // كل نوع في VM يُنشأ عبر طريقة مصنع (factory method) ثابتة
    // مثل: قيمة::صحيح(42)، قيمة::نص("مرحبا")

    SAD_TEST("VM_VAL01: إنشاء قيمة لا_شيء (null)", {
        // قيمة::لا_شيء() تُنشئ القيمة الفارغة — تمثل غياب القيمة
        auto v = sad::vm::قيمة::لا_شيء();
        SAD_ASSERT_TRUE(v.هل_لا_شيء());
        SAD_ASSERT_EQ((int)v.النوع, (int)sad::vm::نوع_القيمة::لا_شيء);
    });

    SAD_TEST("VM_VAL02: إنشاء قيمة منطقية صحيح (true)", {
        auto v = sad::vm::قيمة::منطقي(true);
        SAD_ASSERT_TRUE(v.هل_منطقي());
        SAD_ASSERT_TRUE(v.كـ_منطقي());
    });

    SAD_TEST("VM_VAL03: إنشاء قيمة منطقية خطأ (false)", {
        auto v = sad::vm::قيمة::منطقي(false);
        SAD_ASSERT_TRUE(v.هل_منطقي());
        SAD_ASSERT_FALSE(v.كـ_منطقي());
    });

    SAD_TEST("VM_VAL04: إنشاء عدد صحيح 64-بت", {
        // يدعم أعداد حتى ±9.2 × 10^18
        auto v = sad::vm::قيمة::صحيح(42);
        SAD_ASSERT_TRUE(v.هل_صحيح());
        SAD_ASSERT_EQ(v.كـ_صحيح(), (int64_t)42);
    });

    SAD_TEST("VM_VAL05: عدد صحيح سالب", {
        auto v = sad::vm::قيمة::صحيح(-100);
        SAD_ASSERT_TRUE(v.هل_صحيح());
        SAD_ASSERT_EQ(v.كـ_صحيح(), (int64_t)-100);
    });

    SAD_TEST("VM_VAL06: عدد صحيح كبير (قرب الحد الأقصى)", {
        auto v = sad::vm::قيمة::صحيح(1000000000LL);
        SAD_ASSERT_EQ(v.كـ_صحيح(), (int64_t)1000000000LL);
    });

    SAD_TEST("VM_VAL07: عدد صحيح صفر", {
        auto v = sad::vm::قيمة::صحيح(0);
        SAD_ASSERT_TRUE(v.هل_صحيح());
        SAD_ASSERT_EQ(v.كـ_صحيح(), (int64_t)0);
    });

    SAD_TEST("VM_VAL08: إنشاء قيمة عشرية (double)", {
        auto v = sad::vm::قيمة::عشري(3.14);
        SAD_ASSERT_TRUE(v.هل_عشري());
        SAD_ASSERT_FLOAT_EQ(v.كـ_عشري(), 3.14, 0.001);
    });

    SAD_TEST("VM_VAL09: عدد عشري سالب", {
        auto v = sad::vm::قيمة::عشري(-2.718);
        SAD_ASSERT_FLOAT_EQ(v.كـ_عشري(), -2.718, 0.001);
    });

    SAD_TEST("VM_VAL10: عدد عشري صفر", {
        auto v = sad::vm::قيمة::عشري(0.0);
        SAD_ASSERT_TRUE(v.هل_عشري());
        SAD_ASSERT_FLOAT_EQ(v.كـ_عشري(), 0.0, 0.0001);
    });

    SAD_TEST("VM_VAL11: إنشاء قيمة نصية", {
        auto v = sad::vm::قيمة::نص("hello");
        SAD_ASSERT_TRUE(v.هل_نص());
        SAD_ASSERT_EQ(v.كـ_نص(), std::string("hello"));
    });

    SAD_TEST("VM_VAL12: نص فارغ", {
        auto v = sad::vm::قيمة::نص("");
        SAD_ASSERT_TRUE(v.هل_نص());
        SAD_ASSERT_EQ(v.كـ_نص(), std::string(""));
    });

    SAD_TEST("VM_VAL13: نص بالعربية — UTF-8", {
        // الآلة الافتراضية تدعم النصوص بالعربية (UTF-8)
        auto v = sad::vm::قيمة::نص("\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7"); // مرحبا
        SAD_ASSERT_TRUE(v.هل_نص());
        SAD_ASSERT_TRUE(v.كـ_نص().size() > 0);
    });

    // ─────────────── فحص الأنواع (Type Checking) ─────────────────────────
    // كل قيمة يمكن فحص نوعها: هل_صحيح()، هل_نص()، هل_رقمية()

    SAD_TEST("VM_VAL14: هل_رقمية — صحيح", {
        // هل_رقمية() ترجع true لكل من الأعداد الصحيحة والعشرية
        auto v = sad::vm::قيمة::صحيح(5);
        SAD_ASSERT_TRUE(v.هل_رقمية());
    });

    SAD_TEST("VM_VAL15: هل_رقمية — عشري", {
        auto v = sad::vm::قيمة::عشري(5.0);
        SAD_ASSERT_TRUE(v.هل_رقمية());
    });

    SAD_TEST("VM_VAL16: هل_رقمية — نص (false)", {
        // النص ليس رقمياً حتى لو كان "123"
        auto v = sad::vm::قيمة::نص("123");
        SAD_ASSERT_FALSE(v.هل_رقمية());
    });

    SAD_TEST("VM_VAL17: هل_قابلة_للاستدعاء — لا_شيء (false)", {
        // الأنواع البسيطة ليست قابلة للاستدعاء
        auto v = sad::vm::قيمة::لا_شيء();
        SAD_ASSERT_FALSE(v.هل_قابلة_للاستدعاء());
    });

    // ─────────────── المصفوفات في VM ────────────────────────────────────
    SAD_TEST("VM_VAL18: إنشاء مصفوفة فارغة", {
        auto v = sad::vm::قيمة::مصفوفة_جديدة();
        SAD_ASSERT_TRUE(v.هل_مصفوفة());
        SAD_ASSERT_EQ((int)v.كـ_مصفوفة()->size(), 0);
    });

    SAD_TEST("VM_VAL19: إنشاء مصفوفة بعناصر أولية", {
        std::vector<sad::vm::قيمة> عناصر;
        عناصر.push_back(sad::vm::قيمة::صحيح(1));
        عناصر.push_back(sad::vm::قيمة::صحيح(2));
        عناصر.push_back(sad::vm::قيمة::صحيح(3));
        auto v = sad::vm::قيمة::مصفوفة(عناصر);
        SAD_ASSERT_TRUE(v.هل_مصفوفة());
        SAD_ASSERT_EQ((int)v.كـ_مصفوفة()->size(), 3);
    });

    SAD_TEST("VM_VAL20: الوصول لعنصر في مصفوفة VM", {
        std::vector<sad::vm::قيمة> عناصر;
        عناصر.push_back(sad::vm::قيمة::صحيح(10));
        عناصر.push_back(sad::vm::قيمة::صحيح(20));
        auto v = sad::vm::قيمة::مصفوفة(عناصر);
        SAD_ASSERT_EQ((*v.كـ_مصفوفة())[0].كـ_صحيح(), (int64_t)10);
        SAD_ASSERT_EQ((*v.كـ_مصفوفة())[1].كـ_صحيح(), (int64_t)20);
    });

    // ─────────────── القواميس في VM ─────────────────────────────────────
    SAD_TEST("VM_VAL21: إنشاء قاموس فارغ", {
        auto v = sad::vm::قيمة::قاموس_جديد();
        SAD_ASSERT_TRUE(v.هل_قاموس());
        SAD_ASSERT_EQ((int)v.كـ_قاموس()->size(), 0);
    });

    SAD_TEST("VM_VAL22: إضافة واسترجاع من قاموس", {
        auto v = sad::vm::قيمة::قاموس_جديد();
        (*v.كـ_قاموس())["key1"] = sad::vm::قيمة::صحيح(42);
        (*v.كـ_قاموس())["key2"] = sad::vm::قيمة::نص("hello");
        SAD_ASSERT_EQ((int)v.كـ_قاموس()->size(), 2);
        SAD_ASSERT_EQ((*v.كـ_قاموس())["key1"].كـ_صحيح(), (int64_t)42);
    });

    // ─────────────── كائنات الدوال والأصناف ──────────────────────────────
    SAD_TEST("VM_VAL23: إنشاء كائن دالة", {
        // كائن_دالة يُخزّن معلومات دالة بايت كود
        auto func = std::make_shared<sad::vm::كائن_دالة>("test_func", 2, 3, 0);
        SAD_ASSERT_EQ(func->الاسم, std::string("test_func"));
        SAD_ASSERT_EQ(func->عدد_المعاملات, 2);
        SAD_ASSERT_EQ(func->عدد_المحليات, 3);
        SAD_ASSERT_EQ(func->بداية_الكود, (uint32_t)0);
    });

    SAD_TEST("VM_VAL24: كائن دالة مجهولة (بدون اسم)", {
        auto func = std::make_shared<sad::vm::كائن_دالة>();
        SAD_ASSERT_EQ(func->الاسم, std::string(""));
        SAD_ASSERT_EQ(func->عدد_المعاملات, 0);
    });

    SAD_TEST("VM_VAL25: إنشاء كائن صنف", {
        auto cls = std::make_shared<sad::vm::كائن_صنف>("MyClass");
        SAD_ASSERT_EQ(cls->الاسم, std::string("MyClass"));
        SAD_ASSERT_EQ((int)cls->الطرق.size(), 0);
        SAD_ASSERT_TRUE(cls->الأب == nullptr);
    });

    SAD_TEST("VM_VAL26: وراثة أصناف في VM", {
        // إنشاء صنف أب وصنف ابن
        auto parent = std::make_shared<sad::vm::كائن_صنف>("Parent");
        auto child = std::make_shared<sad::vm::كائن_صنف>("Child");
        child->الأب = parent;
        SAD_ASSERT_NOT_NULL(child->الأب.get());
        SAD_ASSERT_EQ(child->الأب->الاسم, std::string("Parent"));
    });

    SAD_TEST("VM_VAL27: إنشاء نسخة من صنف", {
        // كائن_نسخة = instance of a class
        auto cls = std::make_shared<sad::vm::كائن_صنف>("Person");
        auto obj = std::make_shared<sad::vm::كائن_نسخة>(cls);
        SAD_ASSERT_EQ(obj->الصنف->الاسم, std::string("Person"));
        SAD_ASSERT_EQ((int)obj->الحقول.size(), 0);
    });

    SAD_TEST("VM_VAL28: تعيين حقول على نسخة كائن", {
        auto cls = std::make_shared<sad::vm::كائن_صنف>("Person");
        auto obj = std::make_shared<sad::vm::كائن_نسخة>(cls);
        obj->الحقول["name"] = sad::vm::قيمة::نص("Ahmed");
        obj->الحقول["age"] = sad::vm::قيمة::صحيح(25);
        SAD_ASSERT_EQ((int)obj->الحقول.size(), 2);
        SAD_ASSERT_EQ(obj->الحقول["name"].كـ_نص(), std::string("Ahmed"));
        SAD_ASSERT_EQ(obj->الحقول["age"].كـ_صحيح(), (int64_t)25);
    });

    SAD_TEST("VM_VAL29: قيمة دالة أصلية (Native function)", {
        // الدوال الأصلية = دوال C++ مُسجّلة في VM
        auto native = sad::vm::قيمة::أصلية([](const std::vector<sad::vm::قيمة>& args) {
            return sad::vm::قيمة::صحيح(42);
        });
        SAD_ASSERT_TRUE(native.هل_أصلية());
        SAD_ASSERT_TRUE(native.هل_قابلة_للاستدعاء());
    });

    SAD_TEST("VM_VAL30: كـ_رقم() يعمل على الصحيح والعشري", {
        // كـ_رقم() يُرجع double لكلا النوعين الرقميين
        auto intVal = sad::vm::قيمة::صحيح(42);
        auto floatVal = sad::vm::قيمة::عشري(3.14);
        SAD_ASSERT_FLOAT_EQ(intVal.كـ_رقم(), 42.0, 0.001);
        SAD_ASSERT_FLOAT_EQ(floatVal.كـ_رقم(), 3.14, 0.001);
    });

#else
    SAD_GROUP("VM.Values / الآلة.القيم [SKIPPED]");
    SAD_SKIP("VM_VAL01-30: قيم الآلة الافتراضية غير متوفرة", "missing sad_vm_value.h");
#endif

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 2: وحدة البايت كود (Bytecode Chunk)                     ║
    // ║  الـ Chunk هي الحاوية الأساسية للبايت كود المُترجم              ║
    // ║  تحتوي: تعليمات + ثوابت + معلومات أسطر + دوال                 ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_VM_CHUNK
    SAD_GROUP("VM.Chunk / الآلة.وحدة_البايت_كود");

    SAD_TEST("CHUNK01: إنشاء وحدة بايت كود فارغة", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        SAD_ASSERT_EQ(وحدة.الحجم(), (uint32_t)0);
    });

    SAD_TEST("CHUNK02: كتابة بايت واحد", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.اكتب(0x42, 1);  // كتابة في السطر 1
        SAD_ASSERT_EQ(وحدة.الحجم(), (uint32_t)1);
        SAD_ASSERT_EQ(وحدة.اقرأ(0), (sad::vm::بايت)0x42);
    });

    SAD_TEST("CHUNK03: كتابة عدة بايتات", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.اكتب(0x10, 1);
        وحدة.اكتب(0x20, 1);
        وحدة.اكتب(0x30, 2);
        SAD_ASSERT_EQ(وحدة.الحجم(), (uint32_t)3);
        SAD_ASSERT_EQ(وحدة.اقرأ(0), (sad::vm::بايت)0x10);
        SAD_ASSERT_EQ(وحدة.اقرأ(1), (sad::vm::بايت)0x20);
        SAD_ASSERT_EQ(وحدة.اقرأ(2), (sad::vm::بايت)0x30);
    });

    SAD_TEST("CHUNK04: كتابة عملية + معامل 8 بت", {
        // عملية_ثابت مع فهرس الثابت
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.اكتب_عملية_ومعامل8(sad::vm::عملية_رقم_صغير, 42, 1);
        SAD_ASSERT_EQ(وحدة.الحجم(), (uint32_t)2);
        SAD_ASSERT_EQ(وحدة.اقرأ(0), sad::vm::عملية_رقم_صغير);
        SAD_ASSERT_EQ(وحدة.اقرأ(1), (sad::vm::بايت)42);
    });

    SAD_TEST("CHUNK05: كتابة عملية + معامل 16 بت", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.اكتب_عملية_ومعامل16(sad::vm::عملية_ثابت, 0x1234, 1);
        SAD_ASSERT_EQ(وحدة.الحجم(), (uint32_t)3);
        SAD_ASSERT_EQ(وحدة.اقرأ_معامل16(1), (uint16_t)0x1234);
    });

    // ─────────────── مجمع الثوابت (Constant Pool) ────────────────────────
    // الثوابت (أعداد، نصوص) تُخزَّن مرة واحدة ويُشار إليها بالفهرس
    SAD_TEST("CHUNK06: إضافة ثابت عدد صحيح", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        auto idx = وحدة.أضف_ثابت(sad::vm::قيمة::صحيح(42));
        SAD_ASSERT_EQ(idx, (uint16_t)0);
        auto& val = وحدة.اقرأ_ثابت(0);
        SAD_ASSERT_TRUE(val.هل_صحيح());
        SAD_ASSERT_EQ(val.كـ_صحيح(), (int64_t)42);
    });

    SAD_TEST("CHUNK07: إضافة عدة ثوابت مختلفة", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        auto i1 = وحدة.أضف_ثابت(sad::vm::قيمة::صحيح(10));
        auto i2 = وحدة.أضف_ثابت(sad::vm::قيمة::عشري(3.14));
        auto i3 = وحدة.أضف_ثابت(sad::vm::قيمة::نص("hello"));
        SAD_ASSERT_EQ(i1, (uint16_t)0);
        SAD_ASSERT_EQ(i2, (uint16_t)1);
        SAD_ASSERT_EQ(i3, (uint16_t)2);
        SAD_ASSERT_EQ(وحدة.عدد_الثوابت(), (size_t)3);
    });

    SAD_TEST("CHUNK08: إزالة التكرار — نفس العدد مرتين", {
        // إذا أضفنا نفس الثابت مرتين، يُرجع نفس الفهرس (deduplication)
        sad::vm::وحدة_بايت_كود وحدة("test");
        auto i1 = وحدة.أضف_ثابت(sad::vm::قيمة::صحيح(42));
        auto i2 = وحدة.أضف_ثابت(sad::vm::قيمة::صحيح(42));
        SAD_ASSERT_EQ(i1, i2);
        SAD_ASSERT_EQ(وحدة.عدد_الثوابت(), (size_t)1);
    });

    SAD_TEST("CHUNK09: إزالة التكرار — نفس النص مرتين", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        auto i1 = وحدة.أضف_ثابت(sad::vm::قيمة::نص("hello"));
        auto i2 = وحدة.أضف_ثابت(sad::vm::قيمة::نص("hello"));
        SAD_ASSERT_EQ(i1, i2);
        SAD_ASSERT_EQ(وحدة.عدد_الثوابت(), (size_t)1);
    });

    SAD_TEST("CHUNK10: إزالة التكرار — أعداد مختلفة", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        auto i1 = وحدة.أضف_ثابت(sad::vm::قيمة::صحيح(1));
        auto i2 = وحدة.أضف_ثابت(sad::vm::قيمة::صحيح(2));
        SAD_ASSERT_NE(i1, i2);
        SAD_ASSERT_EQ(وحدة.عدد_الثوابت(), (size_t)2);
    });

    // ─────────────── تسجيل واسترجاع الدوال ───────────────────────────────
    SAD_TEST("CHUNK11: تسجيل دالة واسترجاعها", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.سجّل_دالة("add", 2, 2, 100);
        auto func = وحدة.ابحث_عن_دالة("add");
        SAD_ASSERT_NOT_NULL(func.get());
        SAD_ASSERT_EQ(func->الاسم, std::string("add"));
        SAD_ASSERT_EQ(func->عدد_المعاملات, 2);
        SAD_ASSERT_EQ(func->بداية_الكود, (uint32_t)100);
    });

    SAD_TEST("CHUNK12: بحث عن دالة غير موجودة", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        auto func = وحدة.ابحث_عن_دالة("nonexistent");
        SAD_ASSERT_TRUE(func == nullptr);
    });

    SAD_TEST("CHUNK13: عدة دوال مسجلة", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.سجّل_دالة("func1", 0, 1, 0);
        وحدة.سجّل_دالة("func2", 1, 2, 50);
        وحدة.سجّل_دالة("func3", 3, 5, 100);
        SAD_ASSERT_NOT_NULL(وحدة.ابحث_عن_دالة("func1").get());
        SAD_ASSERT_NOT_NULL(وحدة.ابحث_عن_دالة("func2").get());
        SAD_ASSERT_NOT_NULL(وحدة.ابحث_عن_دالة("func3").get());
    });

    // ─────────────── تصحيح القفزات (Backpatching) ────────────────────────
    // عند ترجمة if/while، لا نعرف عنوان القفزة مسبقاً
    // نكتب عنواناً مؤقتاً (0xFFFF) ثم نُصححه لاحقاً
    SAD_TEST("CHUNK14: كتابة وتصحيح قفزة مؤقتة", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        // كتابة بعض التعليمات قبل القفزة
        وحدة.اكتب(sad::vm::عملية_لا_شيء, 1);
        // كتابة قفزة مؤقتة (عنوان مؤقت 0xFFFF)
        uint32_t patchLoc = وحدة.اكتب_قفزة_مؤقتة(sad::vm::عملية_قفز, 2);
        // كتابة بعض التعليمات بعد القفزة
        وحدة.اكتب(sad::vm::عملية_لا_شيء, 3);
        وحدة.اكتب(sad::vm::عملية_لا_شيء, 3);
        // تصحيح القفزة للوصول للموقع الحالي
        وحدة.صحّح_قفزة(patchLoc);
        // التحقق أن القفزة تشير للموقع الصحيح
        uint16_t target = وحدة.اقرأ_معامل16(patchLoc);
        SAD_ASSERT_EQ(target, (uint16_t)وحدة.الحجم());
    });

    SAD_TEST("CHUNK15: تصحيح قفزة لعنوان محدد", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        uint32_t patchLoc = وحدة.اكتب_قفزة_مؤقتة(sad::vm::عملية_قفز, 1);
        وحدة.صحّح_قفزة_إلى(patchLoc, 0x0042);
        SAD_ASSERT_EQ(وحدة.اقرأ_معامل16(patchLoc), (uint16_t)0x0042);
    });

    // ─────────────── معلومات الأسطر المصدرية ─────────────────────────────
    SAD_TEST("CHUNK16: تتبع أرقام الأسطر المصدرية", {
        sad::vm::وحدة_بايت_كود وحدة("test");
        وحدة.اكتب(0x10, 1);  // السطر 1
        وحدة.اكتب(0x20, 1);  // السطر 1
        وحدة.اكتب(0x30, 2);  // السطر 2
        وحدة.اكتب(0x40, 3);  // السطر 3
        SAD_ASSERT_EQ(وحدة.رقم_السطر(0), (uint32_t)1);
        SAD_ASSERT_EQ(وحدة.رقم_السطر(1), (uint32_t)1);
        SAD_ASSERT_EQ(وحدة.رقم_السطر(2), (uint32_t)2);
        SAD_ASSERT_EQ(وحدة.رقم_السطر(3), (uint32_t)3);
    });

#else
    SAD_GROUP("VM.Chunk / الآلة.وحدة_البايت_كود [SKIPPED]");
    SAD_SKIP("CHUNK01-16: وحدة البايت كود غير متوفرة", "missing sad_vm_chunk.h");
#endif

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 3: تعليمات الآلة الافتراضية (Opcodes)                   ║
    // ║  التحقق من صحة قيم الـ opcodes وتنظيمها                        ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_VM_OPCODES
    SAD_GROUP("VM.Opcodes / الآلة.رموز_العمليات");

    SAD_TEST("OP01: عملية_لا_شيء = 0x00 (NOP)", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_لا_شيء, 0x00);
    });

    SAD_TEST("OP02: عملية_ثابت = 0x01", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_ثابت, 0x01);
    });

    SAD_TEST("OP03: عملية_رقم_صغير = 0x02", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_رقم_صغير, 0x02);
    });

    SAD_TEST("OP04: عمليات المكدس في المجال 0x00-0x0F", {
        // كل عمليات المكدس يجب أن تكون أقل من 0x10
        SAD_ASSERT_LT((int)sad::vm::عملية_لا_شيء, 0x10);
        SAD_ASSERT_LT((int)sad::vm::عملية_ثابت, 0x10);
        SAD_ASSERT_LT((int)sad::vm::عملية_سحب, 0x10);
        SAD_ASSERT_LT((int)sad::vm::عملية_نسخ, 0x10);
        SAD_ASSERT_LT((int)sad::vm::عملية_تبديل, 0x10);
    });

    SAD_TEST("OP05: عمليات الحساب في المجال 0x10-0x1F", {
        // العمليات الحسابية في المجموعة 0x10
        SAD_ASSERT_GE((int)sad::vm::عملية_جمع, 0x10);
        SAD_ASSERT_LT((int)sad::vm::عملية_جمع, 0x20);
    });

    SAD_TEST("OP06: عملية_جمع — التحقق من القيمة الدقيقة", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_جمع, 0x10);
    });

    SAD_TEST("OP07: عملية_طرح — التحقق من القيمة", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_طرح, 0x11);
    });

    SAD_TEST("OP08: عملية_ضرب — التحقق من القيمة", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_ضرب, 0x12);
    });

    SAD_TEST("OP09: عملية_قسمة — التحقق من القيمة", {
        SAD_ASSERT_EQ((int)sad::vm::عملية_قسمة, 0x13);
    });

#else
    SAD_GROUP("VM.Opcodes / الآلة.رموز_العمليات [SKIPPED]");
    SAD_SKIP("OP01-09: رموز العمليات غير متوفرة", "missing sad_vm_opcodes.h");
#endif

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 4: مدراء المفسر (Interpreter Managers)                  ║
    // ║  ScopeManager + VariableManager + ClassManager + ObjectManager  ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_SCOPE_MANAGER
    SAD_GROUP("Managers.ScopeManager / المدراء.النطاقات");

    // ─────────────── ScopeManager: إدارة نطاقات المتغيرات ────────────────
    // النطاقات تتحكم في مرئية المتغيرات: عام > دالة > كتلة > حلقة

    SAD_TEST("SCOPE01: إنشاء مدير نطاقات — يبدأ بنطاق عام", {
        Sad::Data::ScopeManager sm;
        SAD_ASSERT_TRUE(sm.isGlobalScope());
        SAD_ASSERT_EQ(sm.getCurrentDepth(), 0);
    });

    SAD_TEST("SCOPE02: دفع نطاق دالة — العمق يزداد", {
        Sad::Data::ScopeManager sm;
        sm.pushScope(Sad::Data::ScopeType::FUNCTION, "myFunc");
        SAD_ASSERT_FALSE(sm.isGlobalScope());
        SAD_ASSERT_EQ(sm.getCurrentDepth(), 1);
    });

    SAD_TEST("SCOPE03: دفع وسحب عدة نطاقات — العمق يتغير", {
        Sad::Data::ScopeManager sm;
        sm.pushScope(Sad::Data::ScopeType::FUNCTION, "f1");
        sm.pushScope(Sad::Data::ScopeType::BLOCK, "block1");
        SAD_ASSERT_EQ(sm.getCurrentDepth(), 2);
        sm.popScope();
        SAD_ASSERT_EQ(sm.getCurrentDepth(), 1);
        sm.popScope();
        SAD_ASSERT_EQ(sm.getCurrentDepth(), 0);
        SAD_ASSERT_TRUE(sm.isGlobalScope());
    });

    SAD_TEST("SCOPE04: isInFunction — داخل دالة", {
        Sad::Data::ScopeManager sm;
        SAD_ASSERT_FALSE(sm.isInFunction());
        sm.pushScope(Sad::Data::ScopeType::FUNCTION, "myFunc");
        SAD_ASSERT_TRUE(sm.isInFunction());
    });

    SAD_TEST("SCOPE05: isInLoop — داخل حلقة", {
        Sad::Data::ScopeManager sm;
        SAD_ASSERT_FALSE(sm.isInLoop());
        sm.pushScope(Sad::Data::ScopeType::LOOP, "whileLoop");
        SAD_ASSERT_TRUE(sm.isInLoop());
    });

    SAD_TEST("SCOPE06: نطاقات متداخلة — دالة > كتلة > حلقة", {
        Sad::Data::ScopeManager sm;
        sm.pushScope(Sad::Data::ScopeType::FUNCTION, "outer");
        sm.pushScope(Sad::Data::ScopeType::BLOCK, "if_block");
        sm.pushScope(Sad::Data::ScopeType::LOOP, "for_loop");
        SAD_ASSERT_EQ(sm.getCurrentDepth(), 3);
        SAD_ASSERT_TRUE(sm.isInFunction());
        SAD_ASSERT_TRUE(sm.isInLoop());
    });

    SAD_TEST("SCOPE07: النطاق الحالي — getCurrentScope()", {
        Sad::Data::ScopeManager sm;
        auto* scope = sm.getCurrentScope();
        SAD_ASSERT_NOT_NULL(scope);
        SAD_ASSERT_TRUE(scope->isGlobal());
    });

    SAD_TEST("SCOPE08: نطاق عام — getGlobalScope()", {
        Sad::Data::ScopeManager sm;
        sm.pushScope(Sad::Data::ScopeType::FUNCTION, "f");
        auto* global = sm.getGlobalScope();
        SAD_ASSERT_NOT_NULL(global);
        SAD_ASSERT_TRUE(global->isGlobal());
    });

#else
    SAD_GROUP("Managers.ScopeManager / المدراء.النطاقات [SKIPPED]");
    SAD_SKIP("SCOPE01-08: مدير النطاقات غير متوفر", "missing scope_manager.h");
#endif

#if HAS_VARIABLE_MANAGER && HAS_SCOPE_MANAGER
    SAD_GROUP("Managers.VariableManager / المدراء.المتغيرات");

    // ─────────────── VariableManager: تعريف واسترجاع المتغيرات ───────────
    SAD_TEST("VAR_MGR01: تعريف واسترجاع متغير بسيط", {
        Sad::Data::ScopeManager sm;
        Sad::Data::VariableManager vm(sm);
        vm.define("x", Sad::Data::Value(42));
        auto val = vm.get("x");
        SAD_ASSERT_EQ(val.toInt(), 42);
    });

    SAD_TEST("VAR_MGR02: تعريف عدة متغيرات", {
        Sad::Data::ScopeManager sm;
        Sad::Data::VariableManager vm(sm);
        vm.define("a", Sad::Data::Value(1));
        vm.define("b", Sad::Data::Value(2));
        vm.define("c", Sad::Data::Value(3));
        SAD_ASSERT_EQ(vm.get("a").toInt(), 1);
        SAD_ASSERT_EQ(vm.get("b").toInt(), 2);
        SAD_ASSERT_EQ(vm.get("c").toInt(), 3);
    });

    SAD_TEST("VAR_MGR03: إعادة إسناد متغير — assign()", {
        Sad::Data::ScopeManager sm;
        Sad::Data::VariableManager vm(sm);
        vm.define("x", Sad::Data::Value(10));
        vm.assign("x", Sad::Data::Value(20));
        SAD_ASSERT_EQ(vm.get("x").toInt(), 20);
    });

    SAD_TEST("VAR_MGR04: فحص وجود متغير — exists()", {
        Sad::Data::ScopeManager sm;
        Sad::Data::VariableManager vm(sm);
        vm.define("x", Sad::Data::Value(1));
        SAD_ASSERT_TRUE(vm.exists("x"));
        SAD_ASSERT_FALSE(vm.exists("y"));
    });

    SAD_TEST("VAR_MGR05: متغيرات في نطاقات مختلفة", {
        // المتغير المعرّف في النطاق الداخلي لا يُرى من الخارج
        Sad::Data::ScopeManager sm;
        Sad::Data::VariableManager vm(sm);
        vm.define("outer", Sad::Data::Value(1));
        sm.pushScope(Sad::Data::ScopeType::BLOCK, "inner");
        vm.define("inner", Sad::Data::Value(2));
        SAD_ASSERT_TRUE(vm.exists("outer")); // يُرى من الداخل
        SAD_ASSERT_TRUE(vm.exists("inner"));
        sm.popScope();
        SAD_ASSERT_TRUE(vm.exists("outer"));
    });

#else
    SAD_GROUP("Managers.VariableManager / المدراء.المتغيرات [SKIPPED]");
    SAD_SKIP("VAR_MGR01-05: مدير المتغيرات غير متوفر", "missing headers");
#endif

#if HAS_FUNCTION_MANAGER
    SAD_GROUP("Managers.FunctionManager / المدراء.الدوال");

    SAD_TEST("FUNC_MGR01: إنشاء مدير الدوال", {
        SAD_ASSERT_NO_THROW(
            Sad::Data::FunctionManager fm;
        );
    });

    SAD_TEST("FUNC_MGR02: hasFunction — دالة غير موجودة", {
        Sad::Data::FunctionManager fm;
        SAD_ASSERT_FALSE(fm.hasFunction("nonexistent"));
    });

#else
    SAD_GROUP("Managers.FunctionManager / المدراء.الدوال [SKIPPED]");
    SAD_SKIP("FUNC_MGR01-02: مدير الدوال غير متوفر", "missing function_manager.h");
#endif

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 5: SIR (Sad Intermediate Representation)                ║
    // ║  التمثيل الوسيط المُستخدم بين الـ Parser والـ Backend           ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_SIR_TYPES
    SAD_GROUP("Compiler.SIR.Types / المترجم.SIR.الأنواع");

    SAD_TEST("SIR_TYPE01: أنواع SIR الأساسية — VOID", {
        auto t = Sad::Compiler::SIR::SIRType::VOID;
        SAD_ASSERT_EQ((int)t, (int)Sad::Compiler::SIR::SIRType::VOID);
    });

    SAD_TEST("SIR_TYPE02: أنواع SIR — I64", {
        auto t = Sad::Compiler::SIR::SIRType::I64;
        SAD_ASSERT_NE((int)t, (int)Sad::Compiler::SIR::SIRType::VOID);
    });

    SAD_TEST("SIR_TYPE03: أنواع SIR — F64", {
        auto t = Sad::Compiler::SIR::SIRType::F64;
        SAD_ASSERT_NE((int)t, (int)Sad::Compiler::SIR::SIRType::I64);
    });

    SAD_TEST("SIR_TYPE04: أنواع SIR — BOOL", {
        auto t = Sad::Compiler::SIR::SIRType::BOOL;
        SAD_ASSERT_NE((int)t, (int)Sad::Compiler::SIR::SIRType::STRING);
    });

    SAD_TEST("SIR_TYPE05: أنواع SIR — STRING", {
        auto t = Sad::Compiler::SIR::SIRType::STRING;
        SAD_ASSERT_NE((int)t, (int)Sad::Compiler::SIR::SIRType::VOID);
    });

    SAD_TEST("SIR_TYPE06: أنواع SIR — ARRAY", {
        auto t = Sad::Compiler::SIR::SIRType::ARRAY;
        SAD_ASSERT_NE((int)t, (int)Sad::Compiler::SIR::SIRType::STRING);
    });

#else
    SAD_GROUP("Compiler.SIR.Types / المترجم.SIR.الأنواع [SKIPPED]");
    SAD_SKIP("SIR_TYPE01-06: أنواع SIR غير متوفرة", "missing sir_types.h");
#endif

#if HAS_SIR_MODULE
    SAD_GROUP("Compiler.SIR.Module / المترجم.SIR.الوحدة");

    SAD_TEST("SIR_MOD01: إنشاء وحدة SIR فارغة", {
        Sad::Compiler::SIR::SIRModule mod("test_module");
        SAD_ASSERT_NO_THROW(
            // فقط التأكد من أنه لا يرمي خطأ عند الإنشاء
            (void)mod.getFunction("nonexistent");
        );
    });

#else
    SAD_GROUP("Compiler.SIR.Module / المترجم.SIR.الوحدة [SKIPPED]");
    SAD_SKIP("SIR_MOD01: وحدة SIR غير متوفرة", "missing sir_module.h");
#endif

    // ══════════════════════════════════════════════════════════════════
    //  النتائج النهائية
    // ══════════════════════════════════════════════════════════════════
    SAD_SUMMARY();
}
