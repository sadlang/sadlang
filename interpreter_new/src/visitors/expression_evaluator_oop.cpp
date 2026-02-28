/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"  // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include "async_runtime.h"  // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

namespace Sad {
namespace Interpreter {

using namespace Data;
using namespace AST;
using namespace Lexer;



// =========================================================================
// (AR) تقييم إنشاء كائن جديد / (EN) New Object Creation Evaluation
// =========================================================================

void ExpressionEvaluator::visitNewExpr(NewExpr& node) {
    // ═══════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ تعبير إنشاء كائن جديد — 'جديد صنف(معاملات)' أو 'صنف(معاملات)'
    //
    //      الآن يُنشئ كائناً حقيقياً من نوع ObjectInstance بمؤشر مشترك
    //      بدلاً من استخدام MAP مؤقت. هذا يسمح بـ:
    //      ✓ تمرير الكائنات كمعاملات للدوال بالمرجع
    //      ✓ هوية فريدة لكل كائن (objectId)
    //      ✓ البحث في سلسلة الوراثة (baseInstance)
    //      ✓ إدارة الذاكرة تلقائياً عبر shared_ptr
    //
    // (EN) Execute new object expression — 'new Class(args)' or 'Class(args)'
    //
    //      Now creates a real ObjectInstance with shared_ptr
    //      instead of using a temporary MAP. This enables:
    //      ✓ Passing objects as function parameters by reference
    //      ✓ Unique identity for each object (objectId)
    //      ✓ Inheritance chain lookup (baseInstance)
    //      ✓ Automatic memory management via shared_ptr
    // ═══════════════════════════════════════════════════════════════════════
    
    #ifdef DEBUG_OOP
    std::cout << "[OOP] تنفيذ تعبير جديد: " << node.className << "\n";
#endif
    
    // (AR) دعم أصناف القوالب: جديد صنف<نوع>(معاملات)
    // (EN) Template class support: new Class<Type>(args)
    std::string effectiveClassName = node.className;
    if (!node.templateArguments.empty()) {
        // (AR) هذا إنشاء صنف قالب - نحتاج إنشاء نسخة ملموسة
        // (EN) This is a template class instantiation - need to create concrete version
        auto* templateDecl = statementExecutor_.getTemplateClass(node.className);
        if (templateDecl) {
            // (AR) إنشاء النسخة الملموسة تحت الاسم الأصلي (بدون تغيير)
            // (EN) Instantiate concrete version under original name (unchanged)
            // لأن المفسر ديناميكي النوع، النسخة واحدة تكفي لكل الأنواع
            statementExecutor_.instantiateTemplateClass(*templateDecl, node.className);
            effectiveClassName = node.className;
        } else {
            // (AR) ربما الصنف مسجّل بالفعل (من تنفيذ سابق)
            // (EN) Perhaps the class is already registered (from previous execution)
            // نتابع بالاسم الأصلي
        }
    }
    
    // (AR) الحصول على ClassType من ClassManager
    // (EN) Get ClassType from ClassManager
    auto* classManager = Data::ClassManager::getInstance();
    ClassType* classType = classManager->getClass(effectiveClassName);
    
    if (!classType) {
        // (AR) آخر محاولة: هل هو صنف قالب ولم نحدد الأنواع؟
        // (EN) Last attempt: is it a template class without type args?
        auto* templateDecl = statementExecutor_.getTemplateClass(node.className);
        if (templateDecl) {
            statementExecutor_.instantiateTemplateClass(*templateDecl, node.className);
            classType = classManager->getClass(node.className);
        }
        
        if (!classType) {
            std::string errMsg = "(AR) الصنف '" + effectiveClassName + "' غير موجود. ";
            errMsg += "(EN) Class '" + effectiveClassName + "' not found.";
            throw RuntimeError(errMsg, node.position);
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) فحص الصنف المجرد — لا يمكن إنشاء كائن من صنف مجرد
    //      يفحص السلسلة الهرمية بالكامل للبحث عن دوال مجردة غير منفذة
    // (EN) Abstract class check — cannot instantiate abstract class
    //      Walks the full inheritance chain to find unimplemented abstract methods
    // ═══════════════════════════════════════════════════════════════════
    {
        // (AR) جمع جميع الدوال المجردة من الأصناف الأب
        // (EN) Collect all abstract methods from parent classes
        std::set<std::string> abstractMethods;
        ClassType* checkClass = classType;
        while (checkClass) {
            for (const auto& method : checkClass->methods) {
                if (method.isAbstract) {
                    abstractMethods.insert(method.name);
                }
            }
            checkClass = checkClass->getBaseClass();
        }
        // (AR) إزالة الدوال التي تم تنفيذها (غير مجردة) في أي مستوى
        // (EN) Remove methods that have concrete implementations at any level
        checkClass = classType;
        while (checkClass) {
            for (const auto& method : checkClass->methods) {
                if (!method.isAbstract) {
                    abstractMethods.erase(method.name);
                }
            }
            checkClass = checkClass->getBaseClass();
        }
        if (!abstractMethods.empty()) {
            std::string methodName = *abstractMethods.begin();
            std::string errMsg = "(AR) لا يمكن إنشاء كائن من صنف مجرد '" + effectiveClassName + "'. ";
            errMsg += "الدالة '" + methodName + "' مجردة وبدون تنفيذ. ";
            errMsg += "(EN) Cannot instantiate abstract class '" + effectiveClassName + "'. ";
            errMsg += "Method '" + methodName + "' is abstract.";
            throw RuntimeError(errMsg, node.position);
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) إنشاء كائن حقيقي من نوع ObjectInstance باستخدام shared_ptr
    //      هذا يختلف عن السلوك القديم الذي كان يستخدم MAP
    //
    // (EN) Create a real ObjectInstance using shared_ptr
    //      This differs from the old behavior which used MAP
    // ═══════════════════════════════════════════════════════════════════
    size_t objId = generateObjectId();
    auto objectInstance = std::make_shared<ObjectInstance>(classType, objId);
    
    // (AR) تهيئة الحقول بقيم افتراضية (بما في ذلك الحقول الموروثة)
    // (EN) Initialize fields with default values (including inherited fields)
    std::vector<ClassField> allFields;
    
    // (AR) جمع جميع الحقول من السلسلة الهرمية
    // (EN) Collect all fields from the class hierarchy
    ClassType* currentClass = classType;
    while (currentClass) {
        for (const auto& field : currentClass->fields) {
            allFields.push_back(field);
        }
        currentClass = currentClass->getBaseClass();
    }
    
    // (AR) تهيئة جميع الحقول غير الثابتة في الكائن
    // (EN) Initialize all non-static fields in the object
    for (const auto& field : allFields) {
        if (field.isStatic) continue;
        objectInstance->fields[field.name] = Value();  // قيمة افتراضية VOID
    }
    
    #ifdef DEBUG_OOP
    std::cout << "[OOP] تم إنشاء كائن من صنف: " << node.className << " (ID: " << objId << ")\n";
    std::cout << "[OOP] عدد الحقول: " << allFields.size() << " (بما في ذلك الموروثة)\n";
#endif
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) استدعاء الباني إذا كان موجودًا
    // (EN) Call constructor if it exists
    // ═══════════════════════════════════════════════════════════════════
    AST::ConstructorDecl* constructor = nullptr;
    if (classType->constructor) {
        constructor = classType->constructor.get();
    } else {
        // (AR) البحث في مصادر نسخ القوالب
        // (EN) Look up constructor from template instance source
        auto* templateSrc = statementExecutor_.getTemplateInstanceSource(effectiveClassName);
        if (templateSrc) {
            for (auto& member : templateSrc->members) {
                if (auto* ctorDecl = dynamic_cast<AST::ConstructorDecl*>(member.get())) {
                    constructor = ctorDecl;
                    break;
                }
            }
        }
    }
    
    if (constructor) {
        // (AR) التحقق من عدد المعاملات
        // (EN) Verify argument count
        if (node.arguments.size() != constructor->parameters.size()) {
            std::string errMsg = "(AR) عدد المعاملات غير متطابق. توقع " + 
                std::to_string(constructor->parameters.size()) + " لكن حصل على " + 
                std::to_string(node.arguments.size()) + ". ";
            errMsg += "(EN) Argument count mismatch. Expected " + 
                std::to_string(constructor->parameters.size()) + " but got " + 
                std::to_string(node.arguments.size()) + ".";
            throw RuntimeError(errMsg, node.position);
        }
        
        // (AR) تقييم المعاملات
        // (EN) Evaluate arguments
        std::vector<Value> argValues;
        for (auto& arg : node.arguments) {
            arg->accept(*this);
            argValues.push_back(lastResult_);
        }
        
        // (AR) إنشاء scope جديد للباني
        // (EN) Create new scope for constructor
        variableManager_.enterScope(Data::ScopeType::FUNCTION, "constructor");
        
        // (AR) ربط المعاملات بالقيم
        // (EN) Bind parameters to values
        for (size_t i = 0; i < constructor->parameters.size(); ++i) {
            variableManager_.define(constructor->parameters[i].name, argValues[i]);
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) ربط 'هذا'/'this' بالكائن الحقيقي كقيمة OBJECT
        //      هذا يسمح للباني بتعديل حقول الكائن مباشرة
        //
        // (EN) Bind 'this'/هذا to the real object as OBJECT value
        //      This allows the constructor to modify object fields directly
        // ═══════════════════════════════════════════════════════════════
        Value objectValue(objectInstance);
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        
        // (AR) تعيين الصنف المنفذ حالياً لدعم سلسلة أساس() متعددة المستويات
        // (EN) Set executing constructor class to support multi-level أساس() chains
        variableManager_.define("__executing_constructor_class__", Value(effectiveClassName));
        
        // (AR) إضافة حقول الكائن للـ scope لسهولة الوصول المباشر
        // (EN) Add object fields to scope for direct access convenience
        for (const auto& [name, value] : objectInstance->fields) {
            variableManager_.define(name, value);
        }
        
        // (AR) إضافة الحقول الثابتة للـ scope
        // (EN) Add static fields to scope
        for (const auto& field : classType->fields) {
            if (field.isStatic) {
                Value* staticValue = classType->getStaticField(field.name);
                if (staticValue) {
                    variableManager_.define(field.name, *staticValue);
                }
            }
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) استدعاء باني الأب إذا كانت هناك معاملات
        // (EN) Call base constructor if superArgs exist
        // ═══════════════════════════════════════════════════════════════
        // (AR) إصلاح: دعم الوراثة متعددة المستويات بالتسلسل المتكرر
        //      مثال: قط_فارسي.superArgs → يستدعي باني قط
        //      → قط.superArgs → يستدعي باني حيوان (تسلسل تلقائي)
        // (EN) Fix: Support multi-level inheritance with recursive chaining
        // ═══════════════════════════════════════════════════════════════
        if (!constructor->superArgs.empty() && classType->getBaseClass()) {
            // (AR) دالة مساعدة لتنفيذ سلسلة البانين المتكررة
            // (EN) Helper to execute recursive constructor chain
            std::function<void(AST::ConstructorDecl*, ClassType*, const std::vector<Value>&)> executeCtorChain;
            executeCtorChain = [&](AST::ConstructorDecl* ctor, ClassType* cls, const std::vector<Value>& args) {
                // (AR) ربط معاملات هذا الباني
                // (EN) Bind this constructor's parameters
                if (args.size() == ctor->parameters.size()) {
                    for (size_t i = 0; i < ctor->parameters.size(); ++i) {
                        const auto& pname = ctor->parameters[i].name;
                        if (variableManager_.exists(pname)) {
                            variableManager_.assign(pname, args[i]);
                        } else {
                            variableManager_.define(pname, args[i]);
                        }
                    }
                }
                
                // (AR) أولاً: إذا كان لهذا الباني superArgs خاصة به، نستدعي الجد أولاً
                // (EN) First: if this constructor has its own superArgs, call grandparent first
                if (!ctor->superArgs.empty() && cls->getBaseClass()) {
                    ClassType* parentClass = cls->getBaseClass();
                    if (parentClass->constructor) {
                        AST::ConstructorDecl* parentCtor = parentClass->constructor.get();
                        // (AR) تقييم superArgs لهذا الباني
                        // (EN) Evaluate this constructor's superArgs
                        std::vector<Value> parentArgs;
                        for (auto& sarg : ctor->superArgs) {
                            sarg->accept(*this);
                            parentArgs.push_back(lastResult_);
                        }
                        // (AR) استدعاء متكرر لباني الأب
                        // (EN) Recursive call to parent constructor
                        executeCtorChain(parentCtor, parentClass, parentArgs);
                    }
                }
                
                // (AR) ثانياً: تنفيذ جسم هذا الباني
                // (EN) Second: execute this constructor's body
                try {
                    ctor->body->accept(statementExecutor_);
                } catch (const std::exception& e) {
                    // (AR) خطأ في الباني — إعادة رفعه
                    // (EN) Constructor error — re-throw
                    throw;
                } catch (...) {
                    throw Interpreter::SadException(
                        "(AR) خطأ غير معروف في تنفيذ الباني. "
                        "(EN) Unknown error executing constructor.",
                        "RuntimeError", node.position);
                }
                
                // (AR) ثالثاً: تحديث حقول الكائن من المتغيرات في النطاق
                // (EN) Third: update object fields from scope variables
                for (const auto& field : cls->fields) {
                    if (!field.isStatic) {
                        if (variableManager_.exists(field.name)) {
                            Value val = variableManager_.get(field.name);
                            objectInstance->fields[field.name] = val;
                        }
                        // (AR) الحقل لم يُعيَّن في الباني — متوقع
                        // (EN) Field not set in constructor — expected
                    }
                }
            };
            
            ClassType* baseClass = classType->getBaseClass();
            if (baseClass->constructor) {
                AST::ConstructorDecl* baseCtor = baseClass->constructor.get();
                // (AR) تقييم superArgs الأصلية
                // (EN) Evaluate original superArgs
                std::vector<Value> superArgValues;
                for (auto& sarg : constructor->superArgs) {
                    sarg->accept(*this);
                    superArgValues.push_back(lastResult_);
                }
                // (AR) بدء سلسلة التنفيذ المتكرر
                // (EN) Start recursive execution chain
                executeCtorChain(baseCtor, baseClass, superArgValues);
            }
        }

        // (AR) تنفيذ جسم الباني
        // (EN) Execute constructor body
        try {
            constructor->body->accept(statementExecutor_);
            
            // (AR) جمع القيم المحدثة من الـ scope إلى حقول الكائن
            // (EN) Collect updated values from scope into object fields
            for (const auto& field : allFields) {
                try {
                    Value updatedValue = variableManager_.get(field.name);
                    if (field.isStatic) {
                        // (AR) تحديث الحقل الثابت في ClassType
                        // (EN) Update static field in ClassType
                        ClassType* fc = classType;
                        while (fc) {
                            for (const auto& ff : fc->fields) {
                                if (ff.name == field.name && ff.isStatic) {
                                    fc->setStaticField(field.name, updatedValue);
                                    break;
                                }
                            }
                            fc = fc->getBaseClass();
                        }
                    } else {
                        // (AR) تحديث حقل الكائن الحقيقي
                        // (EN) Update real object field
                        objectInstance->fields[field.name] = updatedValue;
                    }
                } catch (const std::exception&) {
                    // (AR) الحقل لم يتم تعيينه في الباني — متوقع
                    // (EN) Field was not set in constructor — expected
                }
            }
            
            // (AR) الكتابة الراجعة للحقول الديناميكية التي أُنشئت في الباني
            //      (حقول أُضيفت عبر هذا.حقل = قيمة بدون إعلان مسبق)
            // (EN) Writeback dynamic fields created in constructor
            //      (fields added via this.field = value without prior declaration)
            // Note: objectInstance->fields may contain dynamically-set fields
            // that are not in allFields, so we don't need an additional loop here
            // since visitMemberAssignExpr already wrote them directly to objPtr->fields
        } catch (const std::exception&) {
            variableManager_.exitScope();
            throw;
        }
        
        variableManager_.exitScope();
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) وضع العلامة أن الباني اُستدعي → إرجاع الكائن كقيمة OBJECT
    // (EN) Mark constructed → Return object as OBJECT value
    // ═══════════════════════════════════════════════════════════════════
    objectInstance->markConstructed();
    lastResult_ = Value(objectInstance);
}

// =========================================================================
// (AR) استدعاء طريقة / (EN) Method Call
// =========================================================================

void ExpressionEvaluator::visitMethodCallExpr(MethodCallExpr& node) {
    #ifdef DEBUG_OOP
    std::cout << "[OOP] استدعاء طريقة: " << node.methodName << "\n";
#endif
    
    // تقييم الكائن
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    auto* classManager = Data::ClassManager::getInstance();
    std::string className;
    ClassType* classType = nullptr;
    Value::MapType fields;
    bool isStaticCall = false;
    
    // التحقق من الاستدعاء الثابت: ClassName.staticMethod()
    // Check for static call: ClassName.staticMethod()
    if (objectValue.isString()) {
        std::string possibleClassName = objectValue.toString();
        classType = classManager->getClass(possibleClassName);
        
        if (classType) {
            // هذا استدعاء ثابت: ClassName.staticMethod()
            // This is static call: ClassName.staticMethod()
            #ifdef DEBUG_OOP
            std::cout << "[OOP] استدعاء طريقة ثابتة: " << possibleClassName << "." << node.methodName << "\n";
#endif
            className = possibleClassName;
            isStaticCall = true;
        }
    }
    
    // إذا لم يكن استدعاء ثابت، فهو استدعاء عادي على كائن
    // If not static call, it's regular call on object
    if (!isStaticCall) {
        // ═══════════════════════════════════════════════════════════════════
        // (AR) نظام الطرق المدمجة على المصفوفات — الطرق المدعومة:
        // ═══════════════════════════════════════════════════════════════════
        //  ▸ الطول() / الحجم() / طول()         → حجم المصفوفة
        //  ▸ اضف(عنصر) / ادفع(عنصر)            → إضافة عنصر في النهاية
        //  ▸ احذف_اخير() / انزع()              → حذف وإرجاع آخر عنصر
        //  ▸ احذف(فهرس)                        → حذف عنصر بالفهرس
        //  ▸ ادخل(فهرس، عنصر)                  → إدخال عنصر في موقع محدد
        //  ▸ اول() / أول()                      → أول عنصر
        //  ▸ اخر() / آخر()                      → آخر عنصر
        //  ▸ يحتوي(عنصر)                       → هل يحتوي على عنصر
        //  ▸ فهرس(عنصر)                        → موقع العنصر (-1 إذا لم يُوجد)
        //  ▸ اقلب() / قلب()                    → عكس ترتيب العناصر
        //  ▸ رتب() / فرز()                     → ترتيب تصاعدي/تنازلي
        //  ▸ شريحة(بداية، نهاية)               → استخراج جزء
        //  ▸ فارغ() / فارغة()                   → هل المصفوفة فارغة
        //  ▸ امسح() / نظف()                    → مسح جميع العناصر
        //  ▸ صل(فاصل) / اربط(فاصل)            → دمج كنص
        //  ▸ نسخ() / انسخ() / استنسخ()         → نسخة مستقلة
        //  ▸ مسطح() / افرد()                   → تسطيح المصفوفات المتداخلة
        //  ▸ فريد() / مميز()                    → إزالة التكرارات
        //  ▸ عدّ(عنصر)                         → عدد مرات تكرار عنصر
        //  ▸ لكل(دالة)                         → تنفيذ دالة على كل عنصر
        //  ▸ خريطة(دالة) / حوّل(دالة)          → تحويل كل عنصر (map)
        //  ▸ رشح(دالة) / صفّي(دالة)            → تصفية (filter)
        //  ▸ اختزل(دالة، قاعدة)               → تجميع (reduce)
        //  ▸ أي(دالة) / بعض(دالة)              → هل يوجد عنصر واحد يحقق الشرط
        //  ▸ كل(دالة) / جميع(دالة)             → هل كل العناصر تحقق الشرط
        //  ▸ جد(دالة)                          → أول عنصر يحقق الشرط
        //  ▸ جد_فهرس(دالة)                     → فهرس أول عنصر يحقق الشرط
        //  ▸ زب() / ازدوج(مصفوفة)              → دمج مصفوفتين كأزواج (zip)
        //  ▸ مدى(بداية، نهاية)                 → إنشاء مصفوفة أرقام
        // ═══════════════════════════════════════════════════════════════════
        if (objectValue.isArray()) {
            // (AR) تقييم جميع المعاملات أولاً
            std::vector<Value> args;
            for (auto& arg : node.arguments) {
                arg->accept(*this);
                args.push_back(lastResult_);
            }
            
            Value::ArrayType arr = objectValue.toArray();
            const std::string& m = node.methodName;
            
            // ─── الطول / الحجم ───
            if (m == "الطول" || m == "الحجم" || m == "طول" || m == "length" || m == "size") {
                lastResult_ = Value(static_cast<int>(arr.size()));
                return;
            }
            // ─── إضافة عنصر (تعديل موضعي) ───
            if (m == "اضف" || m == "أضف" || m == "ادفع" || m == "push" || m == "append") {
                if (args.empty()) throw RuntimeError("(AR) اضف() يتطلب معاملاً واحداً على الأقل. (EN) push() requires at least one argument.", node.position);
                for (auto& a : args) arr.push_back(a);
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = newArr;
                return;
            }
            // ─── حذف وإرجاع آخر عنصر ───
            if (m == "احذف_اخير" || m == "انزع" || m == "pop") {
                if (arr.empty()) throw RuntimeError("(AR) لا يمكن الحذف من مصفوفة فارغة. (EN) Cannot pop from empty array.", node.position);
                Value last = arr.back();
                arr.pop_back();
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = last;
                return;
            }
            // ─── حذف عنصر بالفهرس ───
            if (m == "احذف" || m == "ازل" || m == "أزل" || m == "remove" || m == "removeAt") {
                if (args.empty()) throw RuntimeError("(AR) احذف() يتطلب فهرس العنصر. (EN) remove() requires an index.", node.position);
                int idx = args[0].toInt();
                if (idx < 0) idx = static_cast<int>(arr.size()) + idx;
                if (idx < 0 || idx >= static_cast<int>(arr.size())) 
                    throw RuntimeError("(AR) الفهرس " + std::to_string(idx) + " خارج النطاق. (EN) Index out of range.", node.position);
                Value removed = arr[idx];
                arr.erase(arr.begin() + idx);
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = removed;
                return;
            }
            // ─── إدخال عنصر في موقع محدد ───
            if (m == "ادخل" || m == "أدخل" || m == "insert") {
                if (args.size() < 2) throw RuntimeError("(AR) ادخل() يتطلب فهرساً وعنصراً. (EN) insert() requires index and value.", node.position);
                int idx = args[0].toInt();
                if (idx < 0) idx = static_cast<int>(arr.size()) + idx;
                if (idx < 0) idx = 0;
                if (idx > static_cast<int>(arr.size())) idx = static_cast<int>(arr.size());
                arr.insert(arr.begin() + idx, args[1]);
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = newArr;
                return;
            }
            // ─── أول عنصر ───
            if (m == "اول" || m == "أول" || m == "first") {
                if (arr.empty()) throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                lastResult_ = arr.front();
                return;
            }
            // ─── آخر عنصر ───
            if (m == "اخر" || m == "آخر" || m == "last") {
                if (arr.empty()) throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                lastResult_ = arr.back();
                return;
            }
            // ─── يحتوي ───
            if (m == "يحتوي" || m == "contains" || m == "includes") {
                if (args.empty()) throw RuntimeError("(AR) يحتوي() يتطلب معاملاً. (EN) contains() requires argument.", node.position);
                bool found = false;
                for (auto& el : arr) {
                    if ((el == args[0]).toBool()) { found = true; break; }
                }
                lastResult_ = Value(found);
                return;
            }
            // ─── فهرس العنصر ───
            if (m == "فهرس" || m == "indexOf" || m == "findIndex") {
                if (args.empty()) throw RuntimeError("(AR) فهرس() يتطلب معاملاً. (EN) indexOf() requires argument.", node.position);
                for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
                    if ((arr[i] == args[0]).toBool()) { lastResult_ = Value(i); return; }
                }
                lastResult_ = Value(-1);
                return;
            }
            // ─── عكس ───
            if (m == "اقلب" || m == "قلب" || m == "reverse") {
                std::reverse(arr.begin(), arr.end());
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = newArr;
                return;
            }
            // ─── ترتيب ───
            if (m == "رتب" || m == "فرز" || m == "sort") {
                bool ascending = true;
                if (!args.empty() && args[0].isBoolean()) ascending = args[0].toBool();
                if (!args.empty() && args[0].isString() && (args[0].toString() == "تنازلي" || args[0].toString() == "desc")) ascending = false;
                auto cmp = [](const Value& a, const Value& b) -> bool {
                    if (a.isNumeric() && b.isNumeric()) return a.toDouble() < b.toDouble();
                    return a.toString() < b.toString();
                };
                if (ascending) std::sort(arr.begin(), arr.end(), cmp);
                else std::sort(arr.begin(), arr.end(), [&cmp](const Value& a, const Value& b) { return cmp(b, a); });
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = newArr;
                return;
            }
            // ─── شريحة ───
            if (m == "شريحة" || m == "slice") {
                int start = args.empty() ? 0 : args[0].toInt();
                int end = args.size() < 2 ? static_cast<int>(arr.size()) : args[1].toInt();
                if (start < 0) start = std::max(0, static_cast<int>(arr.size()) + start);
                if (end < 0) end = std::max(0, static_cast<int>(arr.size()) + end);
                if (start > static_cast<int>(arr.size())) start = static_cast<int>(arr.size());
                if (end > static_cast<int>(arr.size())) end = static_cast<int>(arr.size());
                if (start >= end) { lastResult_ = Value(Value::ArrayType{}); return; }
                Value::ArrayType sliced(arr.begin() + start, arr.begin() + end);
                lastResult_ = Value(sliced);
                return;
            }
            // ─── فارغة ───
            if (m == "فارغ" || m == "فارغة" || m == "empty" || m == "isEmpty") {
                lastResult_ = Value(arr.empty());
                return;
            }
            // ─── مسح ───
            if (m == "امسح" || m == "نظف" || m == "clear") {
                Value newArr(Value::ArrayType{});
                writeBackChain(node.object.get(), newArr);
                lastResult_ = newArr;
                return;
            }
            // ─── صل / اربط (join) ───
            if (m == "صل" || m == "اربط" || m == "join") {
                std::string sep = args.empty() ? "" : args[0].toString();
                std::string result;
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) result += sep;
                    result += arr[i].toString();
                }
                lastResult_ = Value(result);
                return;
            }
            // ─── نسخ ───
            if (m == "نسخ" || m == "انسخ" || m == "استنسخ" || m == "clone" || m == "copy") {
                Value::ArrayType copy(arr.begin(), arr.end());
                lastResult_ = Value(copy);
                return;
            }
            // ─── تسطيح (flatten) ───
            if (m == "مسطح" || m == "افرد" || m == "flatten") {
                // (AR) حد العمق الأقصى لمنع التكرار اللانهائي
                // (EN) Max depth limit to prevent infinite recursion
                int maxDepth = 100;
                if (!args.empty() && args[0].isNumeric()) {
                    maxDepth = args[0].toInt();
                }
                Value::ArrayType flat;
                std::function<void(const Value::ArrayType&, int)> doFlatten;
                doFlatten = [&flat, &doFlatten](const Value::ArrayType& a, int depth) {
                    for (auto& el : a) {
                        if (el.isArray() && depth > 0) doFlatten(el.toArrayRef(), depth - 1);
                        else flat.push_back(el);
                    }
                };
                doFlatten(arr, maxDepth);
                lastResult_ = Value(flat);
                return;
            }
            // ─── فريد (unique) ───
            if (m == "فريد" || m == "مميز" || m == "unique" || m == "distinct") {
                Value::ArrayType unique;
                for (auto& el : arr) {
                    bool alreadySeen = false;
                    for (auto& u : unique) {
                        if ((u == el).toBool()) { alreadySeen = true; break; }
                    }
                    if (!alreadySeen) {
                        unique.push_back(el);
                    }
                }
                lastResult_ = Value(unique);
                return;
            }
            // ─── عدّ (count) ───
            if (m == "عدّ" || m == "عد" || m == "count") {
                if (args.empty()) { lastResult_ = Value(static_cast<int>(arr.size())); return; }
                int cnt = 0;
                for (auto& el : arr) { if ((el == args[0]).toBool()) ++cnt; }
                lastResult_ = Value(cnt);
                return;
            }
            
            // ═══════════════════════════════════════════════════════════════
            // (AR) العمليات الدالية (Functional) — تقبل دوال كمعاملات
            // ═══════════════════════════════════════════════════════════════
            
            // ─── دالة مساعدة لتنفيذ lambda/function على عنصر ───
            auto callFunction = [this, &node](const std::string& funcName, const std::vector<Value>& callArgs) -> Value {
                // (AR) البحث عن الدالة في مدير الدوال بعدد المعاملات الممررة
                auto funcDef = functionManager_.getFunction(funcName, callArgs.size());
                if (!funcDef) {
                    // (AR) بحث بالاسم الموسع __template_
                    funcDef = functionManager_.getFunction("__template_" + funcName, callArgs.size());
                }
                if (!funcDef) {
                    throw RuntimeError("(AR) الدالة '" + funcName + "' غير موجودة. (EN) Function '" + funcName + "' not found.", node.position);
                }
                
                variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);
                // (AR) ربط المعاملات باستخدام getParameters()
                const auto& params = funcDef->getParameters();
                for (size_t i = 0; i < params.size() && i < callArgs.size(); ++i) {
                    variableManager_.define(params[i].name, callArgs[i]);
                }
                
                Value result;
                try {
                    if (funcDef->hasBody()) {
                        // (AR) الحصول على جسم الدالة واستدعاء accept — تحويل آمن مباشر
                        // (EN) Get function body and call accept — direct safe access
                        auto bodyPtr = funcDef->getBody();
                        bodyPtr->accept(statementExecutor_);
                        if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                            result = statementExecutor_.getReturnValue();
                            statementExecutor_.resetFlowControl();
                        }
                    }
                } catch (...) {
                    variableManager_.exitScope();
                    throw;
                }
                variableManager_.exitScope();
                return result;
            };
            
            // ─── لكل (forEach) ───
            if (m == "لكل" || m == "forEach" || m == "each") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) لكل() يتطلب اسم دالة. (EN) forEach() requires function name.", node.position);
                std::string funcName = args[0].toString();
                for (size_t i = 0; i < arr.size(); ++i) {
                    callFunction(funcName, {arr[i], Value(static_cast<int>(i))});
                }
                lastResult_ = Value();
                return;
            }
            // ─── خريطة / حوّل (map) ───
            if (m == "خريطة" || m == "حوّل" || m == "حول" || m == "map" || m == "transform") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) خريطة() يتطلب اسم دالة. (EN) map() requires function name.", node.position);
                std::string funcName = args[0].toString();
                Value::ArrayType result;
                for (size_t i = 0; i < arr.size(); ++i) {
                    result.push_back(callFunction(funcName, {arr[i], Value(static_cast<int>(i))}));
                }
                lastResult_ = Value(result);
                return;
            }
            // ─── رشح / صفّي (filter) ───
            if (m == "رشح" || m == "صفّي" || m == "صفي" || m == "filter") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) رشح() يتطلب اسم دالة. (EN) filter() requires function name.", node.position);
                std::string funcName = args[0].toString();
                Value::ArrayType result;
                for (size_t i = 0; i < arr.size(); ++i) {
                    Value cond = callFunction(funcName, {arr[i], Value(static_cast<int>(i))});
                    if (cond.toBool()) result.push_back(arr[i]);
                }
                lastResult_ = Value(result);
                return;
            }
            // ─── اختزل (reduce) ───
            if (m == "اختزل" || m == "reduce" || m == "fold") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) اختزل() يتطلب اسم دالة. (EN) reduce() requires function name.", node.position);
                std::string funcName = args[0].toString();
                if (arr.empty()) { 
                    lastResult_ = args.size() > 1 ? args[1] : Value(); 
                    return; 
                }
                Value accumulator = args.size() > 1 ? args[1] : arr[0];
                size_t startIdx = args.size() > 1 ? 0 : 1;
                for (size_t i = startIdx; i < arr.size(); ++i) {
                    accumulator = callFunction(funcName, {accumulator, arr[i]});
                }
                lastResult_ = accumulator;
                return;
            }
            // ─── أي / بعض (some/any) ───
            if (m == "أي" || m == "اي" || m == "بعض" || m == "some" || m == "any") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) أي() يتطلب اسم دالة. (EN) some() requires function name.", node.position);
                std::string funcName = args[0].toString();
                for (auto& el : arr) {
                    if (callFunction(funcName, {el}).toBool()) { lastResult_ = Value(true); return; }
                }
                lastResult_ = Value(false);
                return;
            }
            // ─── كل / جميع (every) ───
            if (m == "كل" || m == "جميع" || m == "every" || m == "all") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) كل() يتطلب اسم دالة. (EN) every() requires function name.", node.position);
                std::string funcName = args[0].toString();
                for (auto& el : arr) {
                    if (!callFunction(funcName, {el}).toBool()) { lastResult_ = Value(false); return; }
                }
                lastResult_ = Value(true);
                return;
            }
            // ─── جد (find) ───
            if (m == "جد" || m == "find") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) جد() يتطلب اسم دالة. (EN) find() requires function name.", node.position);
                std::string funcName = args[0].toString();
                for (auto& el : arr) {
                    if (callFunction(funcName, {el}).toBool()) { lastResult_ = el; return; }
                }
                lastResult_ = Value();
                return;
            }
            // ─── جد_فهرس (findIndex) ───
            if (m == "جد_فهرس" || m == "findIndex") {
                if (args.empty() || !args[0].isString()) throw RuntimeError("(AR) جد_فهرس() يتطلب اسم دالة. (EN) findIndex() requires function name.", node.position);
                std::string funcName = args[0].toString();
                for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
                    if (callFunction(funcName, {arr[i]}).toBool()) { lastResult_ = Value(i); return; }
                }
                lastResult_ = Value(-1);
                return;
            }
            // ─── ازدوج / zip ───
            if (m == "ازدوج" || m == "zip") {
                if (args.empty() || !args[0].isArray()) throw RuntimeError("(AR) ازدوج() يتطلب مصفوفة ثانية. (EN) zip() requires another array.", node.position);
                Value::ArrayType other = args[0].toArray();
                Value::ArrayType result;
                size_t minLen = std::min(arr.size(), other.size());
                for (size_t i = 0; i < minLen; ++i) {
                    Value::ArrayType pair;
                    pair.push_back(arr[i]);
                    pair.push_back(other[i]);
                    result.push_back(Value(pair));
                }
                lastResult_ = Value(result);
                return;
            }
            // ─── عكس المصفوفة بدون تعديل (reversed) ───
            if (m == "معكوس" || m == "reversed") {
                Value::ArrayType rev(arr.rbegin(), arr.rend());
                lastResult_ = Value(rev);
                return;
            }
            // ─── حد_أقصى / max ───
            if (m == "حد_اقصى" || m == "أقصى" || m == "max") {
                if (arr.empty()) throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                Value mx = arr[0];
                for (size_t i = 1; i < arr.size(); ++i) {
                    if (arr[i].isNumeric() && mx.isNumeric()) {
                        if (arr[i].toDouble() > mx.toDouble()) mx = arr[i];
                    } else if (arr[i].toString() > mx.toString()) mx = arr[i];
                }
                lastResult_ = mx;
                return;
            }
            // ─── حد_أدنى / min ───
            if (m == "حد_ادنى" || m == "أدنى" || m == "min") {
                if (arr.empty()) throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                Value mn = arr[0];
                for (size_t i = 1; i < arr.size(); ++i) {
                    if (arr[i].isNumeric() && mn.isNumeric()) {
                        if (arr[i].toDouble() < mn.toDouble()) mn = arr[i];
                    } else if (arr[i].toString() < mn.toString()) mn = arr[i];
                }
                lastResult_ = mn;
                return;
            }
            // ─── مجموع / sum ───
            if (m == "مجموع" || m == "sum") {
                double sum = 0;
                for (auto& el : arr) {
                    if (el.isNumeric()) sum += el.toDouble();
                }
                if (sum == static_cast<int>(sum)) lastResult_ = Value(static_cast<int>(sum));
                else lastResult_ = Value(sum);
                return;
            }
            // ─── متوسط / average ───
            if (m == "متوسط" || m == "average" || m == "avg") {
                if (arr.empty()) throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                double sum = 0;
                int count = 0;
                for (auto& el : arr) {
                    if (el.isNumeric()) { sum += el.toDouble(); ++count; }
                }
                if (count == 0) { lastResult_ = Value(0); return; }
                lastResult_ = Value(sum / count);
                return;
            }
            // ─── ملء / fill ───
            if (m == "املأ" || m == "املا" || m == "fill") {
                if (args.empty()) throw RuntimeError("(AR) املأ() يتطلب قيمة. (EN) fill() requires a value.", node.position);
                for (auto& el : arr) el = args[0];
                Value newArr(arr);
                writeBackChain(node.object.get(), newArr);
                lastResult_ = newArr;
                return;
            }
            
            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال المصفوفات متعددة الأبعاد (matrix / multidimensional)
            // (EN) Multidimensional array (matrix) methods
            // ═══════════════════════════════════════════════════════════════

            // ─── أبعاد / شكل / dimensions / shape ───
            // (AR) تُرجع مصفوفة بأبعاد المصفوفة، مثلاً [3, 4] لمصفوفة 3×4
            // (EN) Returns an array with the dimensions, e.g. [3, 4] for a 3×4 matrix
            if (m == "أبعاد" || m == "ابعاد" || m == "شكل" || m == "dimensions" || m == "shape") {
                Value::ArrayType dims;
                std::function<void(const Value::ArrayType&)> getDims;
                getDims = [&dims, &getDims](const Value::ArrayType& a) {
                    dims.push_back(Value(static_cast<int64_t>(a.size())));
                    if (!a.empty() && a[0].isArray()) {
                        getDims(a[0].toArrayRef());
                    }
                };
                getDims(arr);
                lastResult_ = Value(dims);
                return;
            }

            // ─── صفوف / rows ───
            // (AR) تُرجع عدد الصفوف (البُعد الأول)
            // (EN) Returns the number of rows (first dimension)
            if (m == "صفوف" || m == "rows") {
                lastResult_ = Value(static_cast<int64_t>(arr.size()));
                return;
            }

            // ─── أعمدة / cols / columns ───
            // (AR) تُرجع عدد الأعمدة (البُعد الثاني) أو 0 إذا كانت المصفوفة أحادية
            // (EN) Returns the number of columns (second dimension) or 0 if 1D
            if (m == "أعمدة" || m == "اعمدة" || m == "cols" || m == "columns") {
                if (!arr.empty() && arr[0].isArray()) {
                    lastResult_ = Value(static_cast<int64_t>(arr[0].toArrayRef().size()));
                } else {
                    lastResult_ = Value(static_cast<int64_t>(0));
                }
                return;
            }

            // ─── قلب_محوري / transpose ───
            // (AR) تقلب المصفوفة ثنائية الأبعاد (تبادل الصفوف والأعمدة)
            // (EN) Transpose a 2D matrix (swap rows and columns)
            if (m == "قلب_محوري" || m == "تبديل" || m == "transpose") {
                if (arr.empty()) { lastResult_ = Value(arr); return; }
                if (!arr[0].isArray()) {
                    throw RuntimeError("(AR) قلب_محوري() تعمل فقط على مصفوفة ثنائية الأبعاد. (EN) transpose() only works on 2D arrays.", node.position);
                }
                size_t rows = arr.size();
                size_t cols = arr[0].toArrayRef().size();
                Value::ArrayType result;
                for (size_t c = 0; c < cols; ++c) {
                    Value::ArrayType newRow;
                    for (size_t r = 0; r < rows; ++r) {
                        auto& row = arr[r].toArrayRef();
                        if (c < row.size()) {
                            newRow.push_back(row[c]);
                        } else {
                            newRow.push_back(Value());
                        }
                    }
                    result.push_back(Value(newRow));
                }
                lastResult_ = Value(result);
                return;
            }

            // ─── أعد_تشكيل / reshape ───
            // (AR) تُعيد تشكيل مصفوفة مسطحة إلى أبعاد جديدة، مثال: [1,2,3,4,5,6].أعد_تشكيل(2, 3) → [[1,2,3],[4,5,6]]
            // (EN) Reshape a flat array into new dimensions, e.g. [1,2,3,4,5,6].reshape(2, 3) → [[1,2,3],[4,5,6]]
            if (m == "أعد_تشكيل" || m == "اعد_تشكيل" || m == "reshape") {
                if (args.size() < 2) {
                    throw RuntimeError("(AR) أعد_تشكيل() يتطلب بُعدين على الأقل (صفوف، أعمدة). (EN) reshape() requires at least 2 dimensions (rows, cols).", node.position);
                }
                // (AR) تسطيح المصفوفة أولاً إن كانت متداخلة
                Value::ArrayType flat;
                std::function<void(const Value::ArrayType&)> flattenAll;
                flattenAll = [&flat, &flattenAll](const Value::ArrayType& a) {
                    for (auto& el : a) {
                        if (el.isArray()) flattenAll(el.toArrayRef());
                        else flat.push_back(el);
                    }
                };
                flattenAll(arr);

                int64_t rows = args[0].toInt();
                int64_t cols = args[1].toInt();
                if (static_cast<int64_t>(flat.size()) < rows * cols) {
                    throw RuntimeError("(AR) عدد العناصر (" + std::to_string(flat.size()) + ") لا يكفي للأبعاد المطلوبة (" + std::to_string(rows) + "×" + std::to_string(cols) + "). (EN) Not enough elements.", node.position);
                }
                if (args.size() >= 3) {
                    // (AR) إعادة تشكيل ثلاثية الأبعاد
                    int64_t depth = args[2].toInt();
                    if (static_cast<int64_t>(flat.size()) < rows * cols * depth) {
                        throw RuntimeError("(AR) عدد العناصر لا يكفي للأبعاد الثلاثية. (EN) Not enough elements for 3D reshape.", node.position);
                    }
                    Value::ArrayType result3d;
                    size_t idx = 0;
                    for (int64_t i = 0; i < rows; ++i) {
                        Value::ArrayType plane;
                        for (int64_t j = 0; j < cols; ++j) {
                            Value::ArrayType row;
                            for (int64_t k = 0; k < depth; ++k) {
                                row.push_back(flat[idx++]);
                            }
                            plane.push_back(Value(row));
                        }
                        result3d.push_back(Value(plane));
                    }
                    lastResult_ = Value(result3d);
                } else {
                    Value::ArrayType result2d;
                    size_t idx = 0;
                    for (int64_t i = 0; i < rows; ++i) {
                        Value::ArrayType row;
                        for (int64_t j = 0; j < cols; ++j) {
                            row.push_back(flat[idx++]);
                        }
                        result2d.push_back(Value(row));
                    }
                    lastResult_ = Value(result2d);
                }
                return;
            }

            // ─── ضرب_مصفوفات / matmul / dot ───
            // (AR) ضرب مصفوفتين ثنائيتي الأبعاد: أ(m×n) · ب(n×p) = ج(m×p)
            // (EN) Matrix multiplication: A(m×n) · B(n×p) = C(m×p)
            if (m == "ضرب_مصفوفات" || m == "ضرب" || m == "matmul" || m == "dot") {
                if (args.empty() || !args[0].isArray()) {
                    throw RuntimeError("(AR) ضرب_مصفوفات() يتطلب مصفوفة ثانية. (EN) matmul() requires another matrix.", node.position);
                }
                if (arr.empty() || !arr[0].isArray()) {
                    throw RuntimeError("(AR) ضرب_مصفوفات() تعمل فقط على مصفوفات ثنائية الأبعاد. (EN) matmul() only works on 2D arrays.", node.position);
                }
                auto& b = args[0].toArrayRef();
                if (b.empty() || !b[0].isArray()) {
                    throw RuntimeError("(AR) المصفوفة الثانية يجب أن تكون ثنائية الأبعاد. (EN) Second matrix must be 2D.", node.position);
                }
                size_t m_rows = arr.size();
                size_t n_cols_a = arr[0].toArrayRef().size();
                size_t n_rows_b = b.size();
                size_t p_cols = b[0].toArrayRef().size();
                if (n_cols_a != n_rows_b) {
                    throw RuntimeError("(AR) أبعاد المصفوفتين غير متوافقة للضرب (" + std::to_string(n_cols_a) + " != " + std::to_string(n_rows_b) + "). (EN) Incompatible matrix dimensions.", node.position);
                }
                Value::ArrayType result;
                for (size_t i = 0; i < m_rows; ++i) {
                    Value::ArrayType row;
                    auto& aRow = arr[i].toArrayRef();
                    for (size_t j = 0; j < p_cols; ++j) {
                        double sum = 0.0;
                        for (size_t k = 0; k < n_cols_a; ++k) {
                            sum += aRow[k].toDouble() * b[k].toArrayRef()[j].toDouble();
                        }
                        if (sum == static_cast<int64_t>(sum)) row.push_back(Value(static_cast<int64_t>(sum)));
                        else row.push_back(Value(sum));
                    }
                    result.push_back(Value(row));
                }
                lastResult_ = Value(result);
                return;
            }

            // ─── عنصر / element / at ───
            // (AR) الوصول إلى عنصر بإحداثيات متعددة: م.عنصر(ص، ع) = م[ص][ع]
            // (EN) Access element by multiple indices: m.element(r, c) = m[r][c]
            if (m == "عنصر" || m == "element" || m == "at") {
                Value current(arr);
                for (size_t i = 0; i < args.size(); ++i) {
                    if (!current.isArray()) {
                        throw RuntimeError("(AR) فهرس زائد عن أبعاد المصفوفة. (EN) Index exceeds array dimensions.", node.position);
                    }
                    int idx = args[i].toInt();
                    auto& currentArr = current.toArrayRef();
                    if (idx < 0 || idx >= static_cast<int>(currentArr.size())) {
                        throw RuntimeError("(AR) فهرس خارج النطاق: " + std::to_string(idx) + ". (EN) Index out of bounds.", node.position);
                    }
                    current = currentArr[idx];
                }
                lastResult_ = current;
                return;
            }

            // ─── عمود / column ───
            // (AR) تُرجع عموداً معيناً من مصفوفة ثنائية الأبعاد كمصفوفة أحادية
            // (EN) Returns a specific column from a 2D matrix as a 1D array
            if (m == "عمود" || m == "column") {
                if (args.empty()) throw RuntimeError("(AR) عمود() يتطلب رقم العمود. (EN) column() requires column index.", node.position);
                int colIdx = args[0].toInt();
                Value::ArrayType col;
                for (auto& rowVal : arr) {
                    if (rowVal.isArray()) {
                        auto& row = rowVal.toArrayRef();
                        if (colIdx >= 0 && colIdx < static_cast<int>(row.size())) {
                            col.push_back(row[colIdx]);
                        } else {
                            col.push_back(Value());
                        }
                    }
                }
                lastResult_ = Value(col);
                return;
            }

            // ─── صف / row ───
            // (AR) تُرجع صفاً معيناً من المصفوفة (مكافئ لـ م[ف])
            // (EN) Returns a specific row (equivalent to m[r])
            if (m == "صف" || m == "row") {
                if (args.empty()) throw RuntimeError("(AR) صف() يتطلب رقم الصف. (EN) row() requires row index.", node.position);
                int rowIdx = args[0].toInt();
                if (rowIdx < 0 || rowIdx >= static_cast<int>(arr.size())) {
                    throw RuntimeError("(AR) فهرس الصف خارج النطاق. (EN) Row index out of bounds.", node.position);
                }
                lastResult_ = arr[rowIdx];
                return;
            }

            // ─── مصفوفة_وحدة / identity ───
            // (AR) إنشاء مصفوفة وحدة n×n (مصفوفة ساكنة — تُستدعى على أي مصفوفة)
            // (EN) Create an n×n identity matrix
            if (m == "مصفوفة_وحدة" || m == "وحدة" || m == "identity") {
                int64_t n = args.empty() ? static_cast<int64_t>(arr.size()) : args[0].toInt();
                Value::ArrayType result;
                for (int64_t i = 0; i < n; ++i) {
                    Value::ArrayType row;
                    for (int64_t j = 0; j < n; ++j) {
                        row.push_back(Value(static_cast<int64_t>(i == j ? 1 : 0)));
                    }
                    result.push_back(Value(row));
                }
                lastResult_ = Value(result);
                return;
            }

            // ─── البُعد / dimension / ndim ───
            // (AR) تُرجع عدد الأبعاد (عمق التداخل)
            // (EN) Returns the number of dimensions (nesting depth)
            if (m == "البُعد" || m == "البعد" || m == "بعد" || m == "dimension" || m == "ndim") {
                int depth = 1;
                const Value::ArrayType* current = &arr;
                while (!current->empty() && (*current)[0].isArray()) {
                    depth++;
                    current = &((*current)[0].toArrayRef());
                }
                lastResult_ = Value(static_cast<int64_t>(depth));
                return;
            }

            // (AR) طريقة غير معروفة على المصفوفة
            throw RuntimeError(
                "(AR) الطريقة '" + m + "' غير موجودة على المصفوفة. (EN) Method '" + m + "' not found on array.",
                node.position
            );
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // (AR) نظام الطرق المدمجة على النصوص
        // ═══════════════════════════════════════════════════════════════════
        if (objectValue.isString()) {
            std::vector<Value> args;
            for (auto& arg : node.arguments) {
                arg->accept(*this);
                args.push_back(lastResult_);
            }
            
            std::string str = objectValue.toString();
            const std::string& m = node.methodName;
            
            if (m == "الطول" || m == "طول" || m == "length" || m == "size") {
                // (AR) نحسب عدد أحرف UTF-8 (وليس البايتات)
                // (EN) Count UTF-8 characters, not bytes
                int charCount = 0;
                for (size_t i = 0; i < str.size(); ) {
                    unsigned char c = static_cast<unsigned char>(str[i]);
                    if (c >= 0xF0) i += 4;
                    else if (c >= 0xE0) i += 3;
                    else if (c >= 0xC0) i += 2;
                    else i += 1;
                    ++charCount;
                }
                lastResult_ = Value(charCount);
                return;
            }
            if (m == "يحتوي" || m == "contains" || m == "includes") {
                if (args.empty()) throw RuntimeError("(AR) يحتوي() يتطلب معاملاً. (EN) contains() requires argument.", node.position);
                lastResult_ = Value(str.find(args[0].toString()) != std::string::npos);
                return;
            }
            if (m == "قسّم" || m == "قسم" || m == "split") {
                std::string sep = args.empty() ? " " : args[0].toString();
                Value::ArrayType parts;
                size_t pos = 0, found;
                while ((found = str.find(sep, pos)) != std::string::npos) {
                    parts.push_back(Value(str.substr(pos, found - pos)));
                    pos = found + sep.size();
                }
                parts.push_back(Value(str.substr(pos)));
                lastResult_ = Value(parts);
                return;
            }
            if (m == "فارغ" || m == "فارغة" || m == "empty" || m == "isEmpty") {
                lastResult_ = Value(str.empty());
                return;
            }
            if (m == "استبدل" || m == "replace") {
                if (args.size() < 2) throw RuntimeError("(AR) استبدل() يتطلب معاملين. (EN) replace() requires 2 arguments.", node.position);
                std::string from = args[0].toString(), to = args[1].toString();
                std::string result = str;
                size_t pos = 0;
                while ((pos = result.find(from, pos)) != std::string::npos) {
                    result.replace(pos, from.length(), to);
                    pos += to.length();
                }
                lastResult_ = Value(result);
                return;
            }
            if (m == "جزء" || m == "substr" || m == "substring") {
                // (AR) فهرسة بالحروف (UTF-8) وليس بالبايتات
                // (EN) Index by UTF-8 characters, not bytes
                // (AR) نحول النص إلى قائمة حروف UTF-8 أولاً
                std::vector<std::string> chars;
                {
                    size_t i = 0;
                    while (i < str.size()) {
                        unsigned char c = static_cast<unsigned char>(str[i]);
                        size_t charLen = 1;
                        if (c >= 0xF0) charLen = 4;
                        else if (c >= 0xE0) charLen = 3;
                        else if (c >= 0xC0) charLen = 2;
                        if (i + charLen > str.size()) charLen = 1; // safety
                        chars.push_back(str.substr(i, charLen));
                        i += charLen;
                    }
                }
                int start = args.empty() ? 0 : args[0].toInt();
                int len = args.size() < 2 ? static_cast<int>(chars.size()) - start : args[1].toInt();
                if (start < 0) start = std::max(0, static_cast<int>(chars.size()) + start);
                if (start > static_cast<int>(chars.size())) start = static_cast<int>(chars.size());
                if (len < 0) len = 0;
                if (start + len > static_cast<int>(chars.size())) len = static_cast<int>(chars.size()) - start;
                std::string result;
                for (int ci = start; ci < start + len; ++ci) result += chars[ci];
                lastResult_ = Value(result);
                return;
            }
            if (m == "حرف_عند" || m == "charAt" || m == "at") {
                if (args.empty()) throw RuntimeError("(AR) حرف_عند() يتطلب فهرساً. (EN) charAt() requires index.", node.position);
                // (AR) فهرسة بالحروف (UTF-8) وليس بالبايتات
                // (EN) Index by UTF-8 characters, not bytes
                std::vector<std::string> chars;
                {
                    size_t i = 0;
                    while (i < str.size()) {
                        unsigned char c = static_cast<unsigned char>(str[i]);
                        size_t charLen = 1;
                        if (c >= 0xF0) charLen = 4;
                        else if (c >= 0xE0) charLen = 3;
                        else if (c >= 0xC0) charLen = 2;
                        if (i + charLen > str.size()) charLen = 1;
                        chars.push_back(str.substr(i, charLen));
                        i += charLen;
                    }
                }
                int idx = args[0].toInt();
                if (idx < 0) idx = static_cast<int>(chars.size()) + idx;
                if (idx < 0 || idx >= static_cast<int>(chars.size())) throw RuntimeError("(AR) الفهرس خارج النطاق. (EN) Index out of range.", node.position);
                lastResult_ = Value(chars[idx]);
                return;
            }
            if (m == "يبدأ_بـ" || m == "يبدأ" || m == "startsWith") {
                if (args.empty()) throw RuntimeError("(AR) يبدأ_بـ() يتطلب معاملاً. (EN) startsWith() requires argument.", node.position);
                std::string prefix = args[0].toString();
                lastResult_ = Value(str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix);
                return;
            }
            if (m == "ينتهي_بـ" || m == "ينتهي" || m == "endsWith") {
                if (args.empty()) throw RuntimeError("(AR) ينتهي_بـ() يتطلب معاملاً. (EN) endsWith() requires argument.", node.position);
                std::string suffix = args[0].toString();
                lastResult_ = Value(str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix);
                return;
            }
            if (m == "قص" || m == "trim") {
                std::string result = str;
                result.erase(0, result.find_first_not_of(" \t\r\n"));
                result.erase(result.find_last_not_of(" \t\r\n") + 1);
                lastResult_ = Value(result);
                return;
            }
            if (m == "كرر" || m == "repeat") {
                if (args.empty()) throw RuntimeError("(AR) كرر() يتطلب عدداً. (EN) repeat() requires count.", node.position);
                int count = args[0].toInt();
                std::string result;
                for (int i = 0; i < count; ++i) result += str;
                lastResult_ = Value(result);
                return;
            }
            if (m == "عكس" || m == "reverse") {
                // (AR) عكس بأحرف UTF-8 (وليس بايتات)
                // (EN) Reverse by UTF-8 codepoints, not bytes
                std::vector<std::string> chars;
                for (size_t i = 0; i < str.size(); ) {
                    unsigned char c = static_cast<unsigned char>(str[i]);
                    size_t charLen = 1;
                    if (c >= 0xF0) charLen = 4;
                    else if (c >= 0xE0) charLen = 3;
                    else if (c >= 0xC0) charLen = 2;
                    if (i + charLen > str.size()) charLen = 1;
                    chars.push_back(str.substr(i, charLen));
                    i += charLen;
                }
                std::string result;
                for (int ci = static_cast<int>(chars.size()) - 1; ci >= 0; --ci) {
                    result += chars[ci];
                }
                lastResult_ = Value(result);
                return;
            }
            if (m == "تحويل_صغير" || m == "لأصغر" || m == "toLower" || m == "lowercase") {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                lastResult_ = Value(result);
                return;
            }
            if (m == "تحويل_كبير" || m == "لأكبر" || m == "toUpper" || m == "uppercase") {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                    [](unsigned char c) { return std::toupper(c); });
                lastResult_ = Value(result);
                return;
            }
            if (m == "بحث" || m == "جد" || m == "find" || m == "indexOf") {
                if (args.empty()) throw RuntimeError("(AR) بحث() يتطلب معاملاً. (EN) find() requires argument.", node.position);
                std::string target = args[0].toString();
                auto pos = str.find(target);
                lastResult_ = Value(pos != std::string::npos ? static_cast<int>(pos) : -1);
                return;
            }
            
            // (AR) طريقة غير معروفة على النص
            throw RuntimeError(
                "(AR) الطريقة '" + m + "' غير موجودة على النص. (EN) Method '" + m + "' not found on string.",
                node.position
            );
        }
        
        // ═══════════════════════════════════════════════════════════════════
        // (AR) نظام الطرق المدمجة على الخرائط (Map)
        // ═══════════════════════════════════════════════════════════════════
        //  ▸ مفاتيح() / keys()                  → مصفوفة المفاتيح
        //  ▸ القيم() / values()                  → مصفوفة القيم
        //  ▸ الطول() / الحجم() / size()          → عدد العناصر
        //  ▸ يحتوي(مفتاح) / has() / containsKey() → هل يحتوي على مفتاح
        //  ▸ احذف(مفتاح) / remove() / delete()   → حذف عنصر بالمفتاح
        //  ▸ دمج(خريطة) / merge()                → دمج خريطتين
        //  ▸ امسح() / نظف() / clear()            → مسح جميع العناصر
        //  ▸ فارغ() / فارغة() / empty()          → هل الخريطة فارغة
        //  ▸ نسخ() / clone() / copy()            → نسخة مستقلة
        //  ▸ عناصر() / entries()                  → مصفوفة من [مفتاح، قيمة]
        // ═══════════════════════════════════════════════════════════════════
        if (objectValue.isMap()) {
            auto mapData = objectValue.toMap();
            // (AR) تحقق: إذا كانت تحتوي على __class__ فهي كائن وليست خريطة عادية
            // (EN) Check: if it has __class__, it's an object not a plain map
            auto classIt = mapData.find("__class__");
            if (classIt == mapData.end()) {
                // (AR) خريطة عادية — معالجة الطرق المدمجة
                std::vector<Value> args;
                for (auto& arg : node.arguments) {
                    arg->accept(*this);
                    args.push_back(lastResult_);
                }
                const std::string& m = node.methodName;

                // ─── المفاتيح ───
                if (m == "مفاتيح" || m == "keys") {
                    Value::ArrayType keys;
                    for (const auto& [k, v] : mapData) {
                        keys.push_back(Value(k));
                    }
                    lastResult_ = Value(keys);
                    return;
                }
                // ─── القيم ───
                if (m == "القيم" || m == "قيم" || m == "values") {
                    Value::ArrayType vals;
                    for (const auto& [k, v] : mapData) {
                        vals.push_back(v);
                    }
                    lastResult_ = Value(vals);
                    return;
                }
                // ─── الطول / الحجم ───
                if (m == "الطول" || m == "الحجم" || m == "طول" || m == "حجم" || m == "length" || m == "size") {
                    lastResult_ = Value(static_cast<int>(mapData.size()));
                    return;
                }
                // ─── يحتوي (على مفتاح) ───
                if (m == "يحتوي" || m == "يحتوي_مفتاح" || m == "has" || m == "containsKey" || m == "contains") {
                    if (args.empty()) throw RuntimeError("(AR) يحتوي() يتطلب معاملاً. (EN) has() requires argument.", node.position);
                    std::string key = args[0].toString();
                    lastResult_ = Value(mapData.find(key) != mapData.end());
                    return;
                }
                // ─── احذف ───
                if (m == "احذف" || m == "أزل" || m == "remove" || m == "delete") {
                    if (args.empty()) throw RuntimeError("(AR) احذف() يتطلب مفتاحاً. (EN) remove() requires a key.", node.position);
                    std::string key = args[0].toString();
                    mapData.erase(key);
                    Value newMap(mapData);
                    writeBackChain(node.object.get(), newMap);
                    lastResult_ = newMap;
                    return;
                }
                // ─── دمج ───
                if (m == "دمج" || m == "merge") {
                    if (args.empty() || !args[0].isMap()) throw RuntimeError("(AR) دمج() يتطلب خريطة. (EN) merge() requires a map argument.", node.position);
                    auto otherMap = args[0].toMap();
                    for (const auto& [k, v] : otherMap) {
                        mapData[k] = v;
                    }
                    Value newMap(mapData);
                    writeBackChain(node.object.get(), newMap);
                    lastResult_ = newMap;
                    return;
                }
                // ─── امسح / نظف ───
                if (m == "امسح" || m == "نظف" || m == "clear") {
                    Value newMap(Value::MapType{});
                    writeBackChain(node.object.get(), newMap);
                    lastResult_ = newMap;
                    return;
                }
                // ─── فارغ / فارغة ───
                if (m == "فارغ" || m == "فارغة" || m == "empty" || m == "isEmpty") {
                    lastResult_ = Value(mapData.empty());
                    return;
                }
                // ─── نسخ ───
                if (m == "نسخ" || m == "انسخ" || m == "استنسخ" || m == "clone" || m == "copy") {
                    Value::MapType copy(mapData.begin(), mapData.end());
                    lastResult_ = Value(copy);
                    return;
                }
                // ─── عناصر (entries) ───
                if (m == "عناصر" || m == "entries") {
                    Value::ArrayType entries;
                    for (const auto& [k, v] : mapData) {
                        Value::ArrayType pair;
                        pair.push_back(Value(k));
                        pair.push_back(v);
                        entries.push_back(Value(pair));
                    }
                    lastResult_ = Value(entries);
                    return;
                }

                // (AR) طريقة غير معروفة على الخريطة
                throw RuntimeError(
                    "(AR) الطريقة '" + m + "' غير موجودة على الخريطة. (EN) Method '" + m + "' not found on map.",
                    node.position
                );
            }
        }

        // ═══════════════════════════════════════════════════════════════════
        // (AR) التحقق من أن القيمة كائن — تدعم كلاً من:
        //      1. نوع OBJECT الحقيقي (الجديد) مع ObjectInstance
        //      2. نوع MAP القديم الذي يحتوي على __class__ (للتوافق)
        //
        // (EN) Check if value is an object — supports both:
        //      1. Real OBJECT type (new) with ObjectInstance
        //      2. Legacy MAP type containing __class__ (backward compat)
        // ═══════════════════════════════════════════════════════════════════
        if (objectValue.isObject()) {
            // (AR) نوع OBJECT الحقيقي — الحصول على اسم الصنف من ObjectInstance
            // (EN) Real OBJECT type — get class name from ObjectInstance
            auto objPtr = objectValue.toObject();
            if (objPtr) {
                className = objPtr->getClassName();
                classType = classManager->getClass(className);
                // (AR) تحويل الحقول إلى MAP للتوافق مع باقي الكود
                // (EN) Convert fields to MAP for compatibility with rest of code
                fields = objPtr->fields;
                fields["__class__"] = Value(className);
            }
        } else if (objectValue.isMap()) {
            // (AR) نوع MAP القديم — التحقق من وجود __class__
            // (EN) Legacy MAP type — check for __class__
            fields = objectValue.toMap();
            auto classNameIt = fields.find("__class__");
            if (classNameIt == fields.end()) {
                throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.", node.position);
            }
            className = classNameIt->second.toString();
            classType = classManager->getClass(className);
        } else {
            std::string errMsg = "(AR) لا يمكن استدعاء طريقة على قيمة ليست كائن. ";
            errMsg += "(EN) Cannot call method on non-object value.";
            throw RuntimeError(errMsg, node.position);
        }
    }
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // البحث عن الطريقة (في السلسلة الهرمية)
    ClassMethod* method = classType->findMethod(node.methodName);
    if (!method) {
        std::string errMsg = "(AR) الطريقة '" + node.methodName + "' غير موجودة في الصنف '" + className + "'. ";
        errMsg += "(EN) Method '" + node.methodName + "' not found in class '" + className + "'.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // التحقق من التطابق بين نوع الاستدعاء ونوع الطريقة
    // Verify call type matches method type
    if (isStaticCall && !method->isStatic) {
        std::string errMsg = "(AR) لا يمكن استدعاء طريقة غير ثابتة '" + node.methodName + "' من خلال اسم الصنف. ";
        errMsg += "(EN) Cannot call non-static method '" + node.methodName + "' through class name.";
        throw RuntimeError(errMsg, node.position);
    }
    if (!isStaticCall && method->isStatic) {
        std::string errMsg = "(AR) يجب استدعاء الطريقة الثابتة '" + node.methodName + "' من خلال اسم الصنف. ";
        errMsg += "(EN) Static method '" + node.methodName + "' should be called through class name.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(method->visibility, node.methodName, classType);
    
    // التحقق من عدد المعاملات
    if (node.arguments.size() != method->parameters.size()) {
        std::string errMsg = "(AR) عدد المعاملات غير متطابق. توقع " + 
            std::to_string(method->parameters.size()) + " لكن حصل على " + 
            std::to_string(node.arguments.size()) + ". ";
        errMsg += "(EN) Argument count mismatch.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // تقييم المعاملات
    std::vector<Value> argValues;
    for (auto& arg : node.arguments) {
        arg->accept(*this);
        argValues.push_back(lastResult_);
    }
    
    // إنشاء scope جديد للطريقة
    variableManager_.enterScope(Data::ScopeType::FUNCTION, node.methodName);
    
    // ربط المعاملات بالقيم
    for (size_t i = 0; i < method->parameters.size(); ++i) {
        variableManager_.define(method->parameters[i].name, argValues[i]);
    }
    
    // إضافة حقول الكائن للـ scope (محاكاة 'this') - فقط للطرق غير الثابتة
    // Add object fields to scope (simulate 'this') - only for non-static methods
    if (!isStaticCall) {
        // إضافة 'this' reference للكائن الحالي
        // Add 'this' reference to current object
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        
        for (const auto& [name, value] : fields) {
            if (name != "__class__") {
                variableManager_.define(name, value);
            }
        }
    }
    
    // إضافة الحقول الثابتة للـ scope (متاحة لكل الطرق)
    // Add static fields to scope (available to all methods)
    for (const auto& field : classType->fields) {
        if (field.isStatic) {
            Value* staticValue = classType->getStaticField(field.name);
            if (staticValue) {
                variableManager_.define(field.name, *staticValue);
            }
        }
    }
    
    // تنفيذ جسم الطريقة
    // (AR) نبحث عن الجسم في ClassType أولاً، ثم في مصدر القالب
    // (EN) Look for body in ClassType first, then in template instance source
    AST::Statement* methodBody = nullptr;
    if (method->body) {
        methodBody = method->body.get();
    } else {
        // (AR) البحث في مصدر القالب
        // (EN) Look up method body from template instance source
        auto* templateSrc = statementExecutor_.getTemplateInstanceSource(className);
        if (templateSrc) {
            for (auto& member : templateSrc->members) {
                if (auto* methodDecl = dynamic_cast<AST::MethodDecl*>(member.get())) {
                    if (methodDecl->name == node.methodName && methodDecl->body) {
                        methodBody = methodDecl->body.get();
                        break;
                    }
                }
            }
        }
    }
    
    Value returnValue;
    try {
        if (methodBody) {
            methodBody->accept(statementExecutor_);
            
            // التحقق من وجود return
            if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                returnValue = statementExecutor_.getReturnValue();
                statementExecutor_.resetFlowControl();
            }
            
            // ═══════════════════════════════════════════════════════════════
            // (AR) جمع القيم المحدثة للحقول — تحديث الكائن الحقيقي
            //      إذا كان الكائن من نوع OBJECT، نحدّث حقول ObjectInstance 
            //      مباشرة (بالمرجع). إذا كان MAP قديم، نعيد بناء MAP.
            //
            // (EN) Collect updated field values — update the real object
            //      If object is OBJECT type, update ObjectInstance fields
            //      directly (by reference). If legacy MAP, rebuild MAP.
            // ═══════════════════════════════════════════════════════════════
            ClassType* currentClass = classType;
            while (currentClass) {
                for (const auto& field : currentClass->fields) {
                    try {
                        Value updatedValue = variableManager_.get(field.name);
                        if (field.isStatic) {
                            // (AR) تحديث الحقل الثابت في ClassType
                            // (EN) Update static field in ClassType
                            currentClass->setStaticField(field.name, updatedValue);
                        } else if (!isStaticCall) {
                            // (AR) تحديث حقل الكائن (MAP مؤقت)
                            // (EN) Update object field (temporary MAP)
                            fields[field.name] = updatedValue;
                        }
                    } catch (const std::exception&) {
                        // (AR) الحقل لم يتم تعديله — متوقع إذا لم يكن في النطاق
                        // (EN) Field was not modified — expected if not in scope
                    }
                }
                currentClass = currentClass->getBaseClass();
            }
            
            // ═══════════════════════════════════════════════════════════════
            // (AR) تحديث الكائن الأصلي — فقط للطرق غير الثابتة
            //      إذا كان MAP قديم: نبني MAP جديد ونعيّنه.
            //      إذا كان OBJECT حقيقي: لا نحتاج فعل شيء لأن التحديثات
            //      تحدث مباشرة في objPtr->fields عبر visitMemberAssignExpr.
            //
            // (EN) Update original object — only for non-static methods
            //      If legacy MAP: build new MAP and assign it.
            //      If real OBJECT: no need to do anything because updates
            //      happen directly in objPtr->fields via visitMemberAssignExpr.
            //
            // ═══════════════════════════════════════════════════════════════
            // (AR) إصلاح المشكلة 18: كان الكود القديم يكتب `fields` (نسخة قديمة)
            //      فوق objPtr->fields بعد تنفيذ الطريقة، مما يمحي التغييرات
            //      التي حدثت أثناء تنفيذ الطريقة (مثل هذا.زد() في الباني).
            //      الحل: لا نُعيد الكتابة على OBJECT حقيقي.
            //
            // (EN) Fix issue 18: old code was writing `fields` (stale copy)
            //      over objPtr->fields after method execution, erasing changes
            //      made during method execution (like this.increment() in ctor).
            //      Solution: don't overwrite real OBJECT.
            // ═══════════════════════════════════════════════════════════════
            if (!isStaticCall) {
                if (objectValue.isObject()) {
                    // (AR) الكائنات الحقيقية (OBJECT type): لا نفعل شيئاً!
                    //      التعيين المباشر في visitMemberAssignExpr يُحدّث
                    //      objPtr->fields مباشرة بالمرجع، فلا نحتاج كتابة راجعة.
                    // (EN) Real objects (OBJECT type): do nothing!
                    //      Direct assignment in visitMemberAssignExpr updates
                    //      objPtr->fields directly by reference, no writeback needed.
                } else if (auto* varExpr = dynamic_cast<VariableExpr*>(node.object.get())) {
                    // (AR) MAP القديم: نبني MAP جديد من الحقول المحدثة
                    // (EN) Legacy MAP: build new MAP from updated fields
                    Value modifiedObject(fields);
                    variableManager_.assign(varExpr->name, modifiedObject);
                }
            }
        }
    } catch (const std::exception&) {
        variableManager_.exitScope();
        throw;
    }
    
    variableManager_.exitScope();
    
    #ifdef DEBUG_OOP
    
    std::cout << "[OOP] ✅ تم تنفيذ الطريقة: " << node.methodName << "\n";
#endif
    // ═══════════════════════════════════════════════════════════════════
    // (AR) إرجاع ضمني لـ 'هذا': إذا لم تُرجع الدالة قيمة صريحة
    //      ولم تكن الدالة ثابتة، نُرجع الكائن نفسه تلقائياً
    //      لدعم تسلسل الاستدعاءات: كائن.دالة1().دالة2().دالة3()
    //
    // (EN) Implicit 'this' return: if method has no explicit return
    //      and is not static, automatically return the object itself
    //      to support method chaining: obj.method1().method2().method3()
    // ═══════════════════════════════════════════════════════════════════
    if (returnValue.isVoid() && !isStaticCall) {
        lastResult_ = objectValue;
    } else {
        lastResult_ = returnValue;
    }
}


} // namespace Interpreter
} // namespace Sad
