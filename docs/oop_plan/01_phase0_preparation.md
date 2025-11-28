# 🛠️ المرحلة 0: التحضير والإعداد / Phase 0: Preparation

**المدة:** 2 يوم  
**الأولوية:** P0 (حرجة)  
**الحالة:** قادمة

---

## 🎯 الأهداف / Objectives

### (AR) الأهداف الرئيسية
1. تصميم البنية التفصيلية لنظام OOP
2. إعداد الملفات والمجلدات المطلوبة
3. تحديد واجهات البرمجة (APIs)
4. إعداد بيئة الاختبار

### (EN) Main Objectives
1. Design detailed OOP system architecture
2. Prepare required files and folders
3. Define programming interfaces (APIs)
4. Setup testing environment

---

## 📋 المهام / Tasks

### المهمة 1: تحليل المعمارية الحالية (4 ساعات)

#### الوصف
دراسة شاملة للمعمارية الحالية لفهم كيفية دمج نظام OOP بسلاسة.

#### الخطوات

1. **دراسة Lexer الحالي**
   ```
   الملفات المطلوبة:
   - include/lexer/token.h
   - include/lexer/lexer_keywords.h
   - src/lexer/lexer_core.cpp
   ```
   
   **التحليل المطلوب:**
   - كيف يتم إضافة كلمات مفتاحية جديدة؟
   - كيف يتم التعامل مع الرموز الخاصة (: , .)?
   - هل يدعم Lexer الحالي جميع الرموز المطلوبة؟

2. **دراسة Parser الحالي**
   ```
   الملفات المطلوبة:
   - include/parser/parser_core.h
   - include/parser/ast/ast_node.h
   - src/parser/parser_expressions.cpp
   - src/parser/parser_statements.cpp
   ```
   
   **التحليل المطلوب:**
   - هيكل AST الحالي
   - كيفية إضافة عقد جديدة
   - نمط الزائر (Visitor Pattern) المستخدم

3. **دراسة Interpreter الحالي**
   ```
   الملفات المطلوبة:
   - include/interpreter/core/interpreter_core.h
   - src/interpreter/expressions/expr_evaluator.cpp
   - src/interpreter/statements/stmt_executor.cpp
   ```
   
   **التحليل المطلوب:**
   - كيفية تنفيذ العبارات
   - نظام إدارة النطاقات (Scopes)
   - آلية تقييم التعبيرات

4. **دراسة نظام البيانات الحالي**
   ```
   الملفات المطلوبة:
   - include/data/types/value.h
   - include/data/managers/variable_manager.h
   - include/data/managers/function_manager.h
   ```
   
   **التحليل المطلوب:**
   - نظام القيم الموحد (Value)
   - كيفية تخزين واسترجاع البيانات
   - إدارة النطاقات المتداخلة

#### المخرجات
- [ ] وثيقة تحليل تفصيلية (analysis_report.md)
- [ ] مخططات تدفق البيانات
- [ ] نقاط التكامل المحددة

---

### المهمة 2: تصميم البنية التفصيلية (6 ساعات)

#### الوصف
تصميم معماري شامل لنظام OOP مع مراعاة التوافق مع البنية الحالية.

#### 2.1 تصميم هيكل الملفات

```
include/
├── data/
│   ├── types/
│   │   ├── class_type.h              # NEW: تعريف نوع الصنف
│   │   ├── object_instance.h          # NEW: تعريف كائن
│   │   └── method_signature.h         # NEW: توقيع الطرق
│   └── managers/
│       ├── class_manager.h            # NEW: مدير الأصناف
│       └── object_manager.h           # NEW: مدير الكائنات
│
├── parser/
│   └── ast/
│       ├── class_nodes.h              # NEW: عقد الأصناف في AST
│       └── member_access_nodes.h      # NEW: عقد الوصول للأعضاء
│
├── interpreter/
│   ├── oop/
│   │   ├── class_interpreter.h        # NEW: تنفيذ الأصناف
│   │   ├── object_instantiator.h      # NEW: إنشاء الكائنات
│   │   └── method_dispatcher.h        # NEW: توزيع استدعاء الطرق
│   └── memory/
│       ├── gc_manager.h               # NEW: جامع القمامة
│       └── ref_counter.h              # NEW: عداد المراجع
│
└── lexer/
    └── oop_keywords.h                 # NEW: كلمات OOP المفتاحية

src/
├── data/
│   ├── types/
│   │   ├── class_type.cpp
│   │   ├── object_instance.cpp
│   │   └── method_signature.cpp
│   └── managers/
│       ├── class_manager.cpp
│       └── object_manager.cpp
│
├── parser/
│   ├── ast/
│   │   ├── class_nodes.cpp
│   │   └── member_access_nodes.cpp
│   └── parser_classes.cpp             # NEW: تحليل نحوي للأصناف
│
├── interpreter/
│   ├── oop/
│   │   ├── class_interpreter.cpp
│   │   ├── object_instantiator.cpp
│   │   └── method_dispatcher.cpp
│   └── memory/
│       ├── gc_manager.cpp
│       └── ref_counter.cpp
│
└── lexer/
    └── oop_keywords.cpp

tests/
├── oop_tests/
│   ├── class_declaration_tests.cpp    # NEW
│   ├── inheritance_tests.cpp          # NEW
│   ├── encapsulation_tests.cpp        # NEW
│   └── memory_management_tests.cpp    # NEW
│
└── integration_tests/
    └── oop_integration_tests.cpp      # NEW

examples/
└── oop/
    ├── 01_simple_class.s              # NEW
    ├── 02_inheritance.s               # NEW
    ├── 03_encapsulation.s             # NEW
    └── 04_polymorphism.s              # NEW
```

#### 2.2 تصميم الواجهات (APIs)

**ClassType Interface:**

```cpp
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Sad {
namespace Data {

// رؤية العضو / Member Visibility
enum class Visibility {
    PUBLIC,      // عام - يمكن الوصول من أي مكان
    PRIVATE,     // خاص - فقط من داخل الصنف
    PROTECTED    // محمي - من الصنف والأصناف الوارثة
};

// تعريف خاصية / Field Definition
struct FieldDef {
    std::string name;                  // اسم الخاصية
    Visibility visibility;             // الرؤية
    Type* type;                        // نوع البيانات
    std::unique_ptr<AST::Expr> defaultValue;  // القيمة الافتراضية
    bool isStatic;                     // هل ثابتة؟
};

// تعريف طريقة / Method Definition
struct MethodDef {
    std::string name;                  // اسم الطريقة
    Visibility visibility;             // الرؤية
    Type* returnType;                  // نوع الإرجاع
    std::vector<Parameter> parameters; // المعاملات
    std::unique_ptr<AST::BlockStmt> body;  // جسم الطريقة
    bool isStatic;                     // هل ثابتة؟
    bool isVirtual;                    // هل افتراضية؟
    bool isAbstract;                   // هل مجردة؟
};

// تعريف باني / Constructor Definition
struct ConstructorDef {
    std::vector<Parameter> parameters;
    std::vector<FieldInitializer> initializers;
    std::unique_ptr<AST::BlockStmt> body;
};

// تعريف هدام / Destructor Definition
struct DestructorDef {
    std::unique_ptr<AST::BlockStmt> body;
};

// نوع الصنف / Class Type
class ClassType {
public:
    // البناء
    explicit ClassType(const std::string& name);
    
    // معلومات أساسية
    std::string getName() const;
    ClassType* getBaseClass() const;
    std::vector<ClassType*> getInterfaces() const;
    
    // إدارة الخصائص
    void addField(const FieldDef& field);
    FieldDef* getField(const std::string& name);
    std::vector<FieldDef*> getAllFields() const;
    
    // إدارة الطرق
    void addMethod(const MethodDef& method);
    MethodDef* getMethod(const std::string& name);
    std::vector<MethodDef*> getAllMethods() const;
    
    // الباني والهدام
    void setConstructor(const ConstructorDef& ctor);
    void setDestructor(const DestructorDef& dtor);
    ConstructorDef* getConstructor();
    DestructorDef* getDestructor();
    
    // الوراثة
    void setBaseClass(ClassType* base);
    void addInterface(ClassType* interface);
    bool inheritsFrom(ClassType* other) const;
    
    // البحث مع الوراثة
    FieldDef* findField(const std::string& name);
    MethodDef* findMethod(const std::string& name);
    
    // فحص الوصول
    bool canAccess(const std::string& memberName, 
                   ClassType* fromClass) const;
    
private:
    std::string name_;
    ClassType* baseClass_;
    std::vector<ClassType*> interfaces_;
    
    std::map<std::string, FieldDef> fields_;
    std::map<std::string, MethodDef> methods_;
    std::unique_ptr<ConstructorDef> constructor_;
    std::unique_ptr<DestructorDef> destructor_;
    
    std::map<std::string, Value> staticFields_;
};

} // namespace Data
} // namespace Sad
```

**ObjectInstance Interface:**

```cpp
#pragma once
#include "class_type.h"
#include "value.h"
#include <map>

namespace Sad {
namespace Data {

// كائن / Object Instance
class ObjectInstance {
public:
    // البناء
    explicit ObjectInstance(ClassType* classType);
    
    // معلومات أساسية
    ClassType* getClassType() const;
    
    // إدارة الخصائص
    void setField(const std::string& name, const Value& value);
    Value getField(const std::string& name) const;
    bool hasField(const std::string& name) const;
    
    // استدعاء الطرق
    Value callMethod(const std::string& name,
                    const std::vector<Value>& args,
                    class InterpreterCore* interpreter);
    
    // إدارة الذاكرة
    void addReference();
    void releaseReference();
    int getReferenceCount() const;
    
    // الهدام
    void callDestructor();
    
private:
    ClassType* classType_;
    std::map<std::string, Value> fields_;
    int referenceCount_;
    bool destructorCalled_;
};

} // namespace Data
} // namespace Sad
```

#### المخرجات
- [ ] ملف تصميم معماري (architecture_design.md)
- [ ] مخططات UML للأصناف
- [ ] تعريفات الواجهات الأساسية

---

### المهمة 3: إعداد الملفات الأساسية (4 ساعات)

#### الوصف
إنشاء جميع الملفات المطلوبة مع هياكل فارغة جاهزة للتنفيذ.

#### الخطوات

1. **إنشاء ملفات Header**
   ```powershell
   # إنشاء الملفات
   New-Item -ItemType File -Path "include/data/types/class_type.h"
   New-Item -ItemType File -Path "include/data/types/object_instance.h"
   New-Item -ItemType File -Path "include/data/managers/class_manager.h"
   New-Item -ItemType File -Path "include/parser/ast/class_nodes.h"
   # ... وغيرها
   ```

2. **إنشاء ملفات Source**
   ```powershell
   New-Item -ItemType File -Path "src/data/types/class_type.cpp"
   New-Item -ItemType File -Path "src/data/types/object_instance.cpp"
   # ... وغيرها
   ```

3. **إنشاء ملفات الاختبار**
   ```powershell
   New-Item -ItemType File -Path "tests/oop_tests/class_declaration_tests.cpp"
   New-Item -ItemType File -Path "tests/oop_tests/inheritance_tests.cpp"
   # ... وغيرها
   ```

4. **إنشاء الأمثلة**
   ```powershell
   New-Item -ItemType File -Path "examples/oop/01_simple_class.s"
   New-Item -ItemType File -Path "examples/oop/02_inheritance.s"
   # ... وغيرها
   ```

5. **تحديث CMakeLists.txt**
   ```cmake
   # إضافة ملفات OOP الجديدة
   set(OOP_SOURCES
       src/data/types/class_type.cpp
       src/data/types/object_instance.cpp
       src/data/managers/class_manager.cpp
       src/data/managers/object_manager.cpp
       src/parser/ast/class_nodes.cpp
       src/parser/parser_classes.cpp
       src/interpreter/oop/class_interpreter.cpp
       src/interpreter/oop/object_instantiator.cpp
       src/interpreter/memory/gc_manager.cpp
   )
   
   # إضافة اختبارات OOP
   set(OOP_TEST_SOURCES
       tests/oop_tests/class_declaration_tests.cpp
       tests/oop_tests/inheritance_tests.cpp
       tests/oop_tests/encapsulation_tests.cpp
   )
   ```

#### المخرجات
- [ ] جميع ملفات Header فارغة مع التوثيق
- [ ] جميع ملفات Source فارغة
- [ ] ملفات الاختبار الأساسية
- [ ] CMakeLists.txt محدث

---

### المهمة 4: إعداد بيئة الاختبار (2 ساعة)

#### الوصف
إعداد إطار الاختبار وكتابة الاختبارات الأساسية.

#### 4.1 اختيار إطار الاختبار

**الخيارات المتاحة:**
1. **Google Test (gtest)** - موصى به
2. **Catch2**
3. **Boost.Test**

**التوصية:** استخدام Google Test لأنه:
- الأكثر انتشاراً
- دعم ممتاز للتوثيق
- سهل الاستخدام

#### 4.2 إعداد Google Test

```cmake
# في CMakeLists.txt
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

enable_testing()

# إنشاء executable للاختبارات
add_executable(oop_tests
    ${OOP_TEST_SOURCES}
)

target_link_libraries(oop_tests
    GTest::gtest_main
    sad_core
)

add_test(NAME OOPTests COMMAND oop_tests)
```

#### 4.3 كتابة اختبار تجريبي

```cpp
// tests/oop_tests/class_declaration_tests.cpp
#include <gtest/gtest.h>
#include "data/types/class_type.h"

using namespace Sad::Data;

class ClassTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // إعداد قبل كل اختبار
    }
    
    void TearDown() override {
        // تنظيف بعد كل اختبار
    }
};

// اختبار تجريبي
TEST_F(ClassTypeTest, CanCreateEmptyClass) {
    // Arrange
    std::string className = "شخص";
    
    // Act
    ClassType classType(className);
    
    // Assert
    EXPECT_EQ(classType.getName(), className);
    EXPECT_EQ(classType.getAllFields().size(), 0);
    EXPECT_EQ(classType.getAllMethods().size(), 0);
}

// اختبار إضافة خاصية
TEST_F(ClassTypeTest, CanAddField) {
    // Arrange
    ClassType classType("شخص");
    FieldDef field;
    field.name = "الاسم";
    field.visibility = Visibility::PUBLIC;
    
    // Act
    classType.addField(field);
    
    // Assert
    EXPECT_EQ(classType.getAllFields().size(), 1);
    auto* retrievedField = classType.getField("الاسم");
    ASSERT_NE(retrievedField, nullptr);
    EXPECT_EQ(retrievedField->name, "الاسم");
}
```

#### المخرجات
- [ ] Google Test مثبت ومكون
- [ ] اختبارات تجريبية تعمل بنجاح
- [ ] سكريبت لتشغيل الاختبارات

---

### المهمة 5: توثيق الإعداد (2 ساعة)

#### الوصف
كتابة وثائق شاملة لمرحلة الإعداد.

#### المحتوى المطلوب

1. **دليل الإعداد (setup_guide.md)**
   - متطلبات النظام
   - خطوات الإعداد
   - التحقق من الإعداد

2. **دليل البناء (build_guide.md)**
   - أوامر البناء
   - خيارات البناء
   - استكشاف الأخطاء

3. **دليل الاختبار (testing_guide.md)**
   - كيفية كتابة الاختبارات
   - كيفية تشغيل الاختبارات
   - معايير تغطية الاختبار

4. **دليل المساهمة (contributing_guide.md)**
   - معايير الكود
   - عملية المراجعة
   - قوالب Pull Request

#### المخرجات
- [ ] جميع الأدلة موثقة
- [ ] أمثلة عملية في كل دليل
- [ ] روابط للموارد الخارجية

---

## ✅ قائمة التحقق / Checklist

### قبل البدء
- [ ] قراءة ROADMAP.md
- [ ] قراءة docs/language_spec/rules/03_oop.md
- [ ] قراءة معمارية_لغة_ص.md

### خلال الإعداد
- [ ] تحليل المعمارية الحالية مكتمل
- [ ] التصميم التفصيلي موثق
- [ ] جميع الملفات الأساسية منشأة
- [ ] بيئة الاختبار جاهزة
- [ ] الوثائق مكتملة

### بعد الإعداد
- [ ] البناء ينجح بدون أخطاء
- [ ] الاختبارات التجريبية تمر
- [ ] الفريق مستعد للبدء
- [ ] مراجعة الكود الأولية مكتملة

---

## 📊 المخرجات المتوقعة / Expected Outputs

1. **الوثائق:**
   - analysis_report.md
   - architecture_design.md
   - setup_guide.md
   - build_guide.md
   - testing_guide.md

2. **الملفات الأساسية:**
   - 20+ ملف header
   - 20+ ملف source
   - 10+ ملف اختبار
   - 5+ ملف مثال

3. **البنية التحتية:**
   - CMakeLists.txt محدث
   - Google Test مكون
   - سكريبتات البناء والاختبار

---

## ⚠️ المخاطر المحتملة / Potential Risks

| المخاطرة | الاحتمالية | التأثير | خطة التخفيف |
|----------|------------|---------|--------------|
| عدم توافق مع البنية الحالية | متوسطة | عالي | مراجعة دقيقة للمعمارية |
| تأخير في الإعداد | منخفضة | متوسط | احتياطي وقت إضافي |
| مشاكل في إعداد الاختبارات | منخفضة | متوسط | استخدام أدوات معروفة |

---

## 🎯 معايير القبول / Acceptance Criteria

- [ ] جميع التحليلات موثقة ومراجعة
- [ ] التصميم المعماري معتمد من الفريق
- [ ] جميع الملفات الأساسية موجودة ومنظمة
- [ ] البناء ينجح بدون أخطاء أو تحذيرات
- [ ] اختبار تجريبي واحد على الأقل يعمل
- [ ] الوثائق شاملة وواضحة

---

**التالي:** [المرحلة 1: الأصناف الأساسية](02_phase1_basic_classes.md)
