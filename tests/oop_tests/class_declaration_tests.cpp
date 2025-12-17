// ======================================================================
// class_declaration_tests.cpp - اختبارات تصريح الأصناف / Class Declaration Tests
// ======================================================================
// الوصف بالعربية:
//   اختبارات شاملة لنظام البرمجة الكائنية في لغة ص
//   يشمل اختبارات الأصناف، الكائنات، الوراثة، والحقول والدوال
//
// English Description:
//   Comprehensive tests for S Language OOP system
//   Includes class, object, inheritance, field, and method tests
// ======================================================================

#include <iostream>
#include <cassert>
#include <string>
#include <memory>

// Include OOP headers
#include "../../include/data/types/class_type.h"
#include "../../include/data/types/object_instance.h"
#include "../../include/data/managers/class_manager.h"
#include "../../include/data/managers/object_manager.h"
#include "../../include/parser/ast/class_nodes.h"

using namespace Sad::Data;
using namespace Sad::AST;

// ======================================================================
// دوال مساعدة للاختبار / Test Helper Functions
// ======================================================================

void printTestHeader(const std::string& testName) {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║  " << testName << "\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
}

void printSuccess(const std::string& message) {
    std::cout << "✓ " << message << "\n";
}

void printError(const std::string& message) {
    std::cerr << "✗ خطأ / Error: " << message << "\n";
}

// ======================================================================
// اختبار 1: إنشاء صنف بسيط / Test 1: Create Simple Class
// ======================================================================

void testSimpleClassCreation() {
    printTestHeader("Test 1: Simple Class Creation | اختبار إنشاء صنف بسيط");
    
    try {
        // (AR) إنشاء صنف "شخص"
        // (EN) Create "Person" class
        auto personClass = std::make_unique<ClassType>("شخص");
        
        assert(personClass != nullptr);
        assert(personClass->name == "شخص");
        
        printSuccess("تم إنشاء الصنف بنجاح / Class created successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 2: إضافة حقول / Test 2: Add Fields
// ======================================================================

void testAddFields() {
    printTestHeader("Test 2: Add Fields | اختبار إضافة حقول");
    
    try {
        auto personClass = std::make_unique<ClassType>("شخص");
        
        // (AR) إضافة حقل "الاسم"
        // (EN) Add "name" field
        static Type stringType("string");
        bool added1 = personClass->addField(
            "الاسم",
            &stringType,
            Visibility::PUBLIC,
            false,
            Value("غير معروف")
        );
        
        assert(added1 == true);
        
        // (AR) إضافة حقل "العمر"
        // (EN) Add "age" field
        static Type intType("int");
        bool added2 = personClass->addField(
            "العمر",
            &intType,
            Visibility::PUBLIC,
            false,
            Value(0)
        );
        
        assert(added2 == true);
        assert(personClass->fields.size() == 2);
        
        // (AR) اختبار عدم إضافة حقل مكرر
        // (EN) Test duplicate field rejection
        bool added3 = personClass->addField(
            "الاسم",
            &stringType,
            Visibility::PUBLIC,
            false,
            Value()
        );
        
        assert(added3 == false);
        
        printSuccess("تمت إضافة الحقول بنجاح / Fields added successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 3: إضافة دوال / Test 3: Add Methods
// ======================================================================

void testAddMethods() {
    printTestHeader("Test 3: Add Methods | اختبار إضافة دوال");
    
    try {
        auto personClass = std::make_unique<ClassType>("شخص");
        
        // (AR) إنشاء دالة "عرف"
        // (EN) Create "introduce" method
        ClassMethod method;
        method.name = "عرف";
        method.visibility = Visibility::PUBLIC;
        method.isStatic = false;
        method.isVirtual = false;
        method.isAbstract = false;
        
        static Type stringType("string");
        method.returnType = &stringType;
        
        bool added = personClass->addMethod(method);
        
        assert(added == true);
        assert(personClass->methods.size() == 1);
        
        // (AR) البحث عن الدالة
        // (EN) Find method
        const ClassMethod* found = personClass->findMethod("عرف");
        assert(found != nullptr);
        assert(found->name == "عرف");
        
        printSuccess("تمت إضافة الدوال بنجاح / Methods added successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 4: ClassManager - تسجيل الأصناف / Test 4: ClassManager - Register Classes
// ======================================================================

void testClassManagerRegistration() {
    printTestHeader("Test 4: ClassManager Registration | اختبار تسجيل الأصناف");
    
    try {
        ClassManager* mgr = ClassManager::getInstance();
        
        // (AR) إعادة تعيين للبدء من صفحة نظيفة
        // (EN) Reset for clean slate
        ClassManager::resetInstance();
        mgr = ClassManager::getInstance();
        
        // (AR) تسجيل صنف "حيوان"
        // (EN) Register "Animal" class
        auto animalClass = std::make_unique<ClassType>("حيوان");
        bool registered = mgr->registerClass(std::move(animalClass));
        
        assert(registered == true);
        assert(mgr->hasClass("حيوان") == true);
        assert(mgr->getClassCount() == 1);
        
        // (AR) محاولة تسجيل صنف بنفس الاسم
        // (EN) Try to register duplicate class
        auto duplicate = std::make_unique<ClassType>("حيوان");
        bool registered2 = mgr->registerClass(std::move(duplicate));
        
        assert(registered2 == false);
        assert(mgr->getClassCount() == 1);
        
        printSuccess("تم تسجيل الأصناف بنجاح / Classes registered successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 5: إنشاء كائنات / Test 5: Create Objects
// ======================================================================

void testObjectCreation() {
    printTestHeader("Test 5: Object Creation | اختبار إنشاء كائنات");
    
    try {
        // (AR) إعادة تعيين المديرين
        // (EN) Reset managers
        ClassManager::resetInstance();
        ObjectManager::resetInstance();
        
        ClassManager* classMgr = ClassManager::getInstance();
        ObjectManager* objMgr = ObjectManager::getInstance();
        
        // (AR) إنشاء وتسجيل صنف "سيارة"
        // (EN) Create and register "Car" class
        auto carClass = std::make_unique<ClassType>("سيارة");
        
        static Type stringType("string");
        carClass->addField("الموديل", &stringType, Visibility::PUBLIC, false, Value("غير معروف"));
        
        classMgr->registerClass(std::move(carClass));
        
        // (AR) إنشاء كائن من صنف "سيارة"
        // (EN) Create object from "Car" class
        ObjectInstance* car1 = objMgr->createObject("سيارة");
        
        assert(car1 != nullptr);
        assert(car1->getClassName() == "سيارة");
        assert(objMgr->getObjectCount() == 1);
        
        // (AR) إنشاء كائن آخر
        // (EN) Create another object
        ObjectInstance* car2 = objMgr->createObject("سيارة");
        
        assert(car2 != nullptr);
        assert(car2->objectId != car1->objectId);
        assert(objMgr->getObjectCount() == 2);
        
        printSuccess("تم إنشاء الكائنات بنجاح / Objects created successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 6: الوصول للحقول / Test 6: Field Access
// ======================================================================

void testFieldAccess() {
    printTestHeader("Test 6: Field Access | اختبار الوصول للحقول");
    
    try {
        // (AR) إعادة تعيين المديرين
        // (EN) Reset managers
        ClassManager::resetInstance();
        ObjectManager::resetInstance();
        
        ClassManager* classMgr = ClassManager::getInstance();
        ObjectManager* objMgr = ObjectManager::getInstance();
        
        // (AR) إنشاء صنف مع حقول
        // (EN) Create class with fields
        auto bookClass = std::make_unique<ClassType>("كتاب");
        
        static Type stringType("string");
        static Type intType("int");
        
        bookClass->addField("العنوان", &stringType, Visibility::PUBLIC, false, Value("بدون عنوان"));
        bookClass->addField("الصفحات", &intType, Visibility::PUBLIC, false, Value(0));
        
        classMgr->registerClass(std::move(bookClass));
        
        // (AR) إنشاء كائن
        // (EN) Create object
        ObjectInstance* book = objMgr->createObject("كتاب");
        book->initializeFields();
        
        // (AR) الوصول للحقول
        // (EN) Access fields
        assert(book->hasField("العنوان") == true);
        assert(book->hasField("الصفحات") == true);
        
        Value* title = book->getField("العنوان");
        assert(title != nullptr);
        
        // (AR) تعيين قيمة للحقل
        // (EN) Set field value
        book->setField("العنوان", Value("البرمجة بالعربية"));
        book->setField("الصفحات", Value(350));
        
        Value* newTitle = book->getField("العنوان");
        assert(newTitle != nullptr);
        
        printSuccess("تم الوصول للحقول بنجاح / Field access successful");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 7: الوراثة / Test 7: Inheritance
// ======================================================================

void testInheritance() {
    printTestHeader("Test 7: Inheritance | اختبار الوراثة");
    
    try {
        // (AR) إعادة تعيين المديرين
        // (EN) Reset managers
        ClassManager::resetInstance();
        
        ClassManager* mgr = ClassManager::getInstance();
        
        // (AR) إنشاء صنف أساسي "مخلوق"
        // (EN) Create base class "Creature"
        auto creatureClass = std::make_unique<ClassType>("مخلوق");
        
        static Type stringType("string");
        creatureClass->addField("الاسم", &stringType, Visibility::PUBLIC, false, Value());
        
        ClassType* creaturePtr = creatureClass.get();
        mgr->registerClass(std::move(creatureClass));
        
        // (AR) إنشاء صنف مشتق "إنسان"
        // (EN) Create derived class "Human"
        auto humanClass = std::make_unique<ClassType>("إنسان");
        humanClass->baseClass = creaturePtr;
        
        static Type intType("int");
        humanClass->addField("العمر", &intType, Visibility::PUBLIC, false, Value(0));
        
        mgr->registerClass(std::move(humanClass));
        
        // (AR) التحقق من الوراثة
        // (EN) Verify inheritance
        ClassType* human = mgr->getClass("إنسان");
        assert(human != nullptr);
        assert(human->baseClass != nullptr);
        assert(human->baseClass->name == "مخلوق");
        assert(human->inheritsFrom("مخلوق") == true);
        
        // (AR) التحقق من سلسلة الوراثة
        // (EN) Verify inheritance chain
        bool valid = mgr->validateInheritanceChain("إنسان");
        assert(valid == true);
        
        printSuccess("تم اختبار الوراثة بنجاح / Inheritance tested successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// اختبار 8: جمع القمامة / Test 8: Garbage Collection
// ======================================================================

void testGarbageCollection() {
    printTestHeader("Test 8: Garbage Collection | اختبار جمع القمامة");
    
    try {
        // (AR) إعادة تعيين المديرين
        // (EN) Reset managers
        ClassManager::resetInstance();
        ObjectManager::resetInstance();
        
        ClassManager* classMgr = ClassManager::getInstance();
        ObjectManager* objMgr = ObjectManager::getInstance();
        
        // (AR) إنشاء صنف
        // (EN) Create class
        auto itemClass = std::make_unique<ClassType>("عنصر");
        classMgr->registerClass(std::move(itemClass));
        
        // (AR) إنشاء عدة كائنات
        // (EN) Create multiple objects
        ObjectInstance* obj1 = objMgr->createObject("عنصر");
        ObjectInstance* obj2 = objMgr->createObject("عنصر");
        ObjectInstance* obj3 = objMgr->createObject("عنصر");
        
        assert(objMgr->getObjectCount() == 3);
        
        // (AR) تمييز بعض الكائنات كمستخدمة
        // (EN) Mark some objects as used
        objMgr->markObject(obj1->objectId);
        objMgr->markObject(obj3->objectId);
        
        // (AR) تشغيل جمع القمامة
        // (EN) Run garbage collection
        size_t deleted = objMgr->runGarbageCollection();
        
        assert(deleted == 1);  // obj2 تم حذفه / obj2 was deleted
        assert(objMgr->getObjectCount() == 2);
        assert(objMgr->hasObject(obj1->objectId) == true);
        assert(objMgr->hasObject(obj2->objectId) == false);
        assert(objMgr->hasObject(obj3->objectId) == true);
        
        printSuccess("تم اختبار جمع القمامة بنجاح / Garbage collection tested successfully");
    }
    catch (const std::exception& e) {
        printError(std::string("فشل الاختبار / Test failed: ") + e.what());
        throw;
    }
}

// ======================================================================
// الدالة الرئيسية / Main Function
// ======================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات البرمجة الكائنية في لغة ص                        ║\n";
    std::cout << "║  S Language OOP System Tests                                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    int testsRun = 0;
    int testsPassed = 0;
    
    try {
        testSimpleClassCreation();
        testsRun++; testsPassed++;
        
        testAddFields();
        testsRun++; testsPassed++;
        
        testAddMethods();
        testsRun++; testsPassed++;
        
        testClassManagerRegistration();
        testsRun++; testsPassed++;
        
        testObjectCreation();
        testsRun++; testsPassed++;
        
        testFieldAccess();
        testsRun++; testsPassed++;
        
        testInheritance();
        testsRun++; testsPassed++;
        
        testGarbageCollection();
        testsRun++; testsPassed++;
    }
    catch (...) {
        std::cerr << "\n⚠ فشل بعض الاختبارات / Some tests failed\n";
    }
    
    // (AR) ملخص النتائج
    // (EN) Results summary
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ملخص الاختبارات / Test Summary                             ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Tests Run: " << testsRun << "\n";
    std::cout << "║  Tests Passed: " << testsPassed << "\n";
    std::cout << "║  Tests Failed: " << (testsRun - testsPassed) << "\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    
    if (testsPassed == testsRun) {
        std::cout << "║  ✓ جميع الاختبارات نجحت! / All tests passed!               ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        return 0;
    } else {
        std::cout << "║  ✗ بعض الاختبارات فشلت / Some tests failed                 ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        return 1;
    }
}

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
