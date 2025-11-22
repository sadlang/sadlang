# حل مشكلة الدوال المضمنة - خطة التصحيح

## 🎯 الهدف:
جعل الدوال المضمنة مثل `print()` تعمل بشكل صحيح

## 🔴 المشكلة الأساسية:

عند استدعاء `print("Hello")`:
1. ✅ يتم البحث عن الدالة بنجاح
2. ✅ يتم العثور عليها في `functions_` map
3. ❌ لكن الـ native implementation فارغ
4. ❌ فلا يتم استدعاء lambda الذي يطبع النص

## 📋 الملفات التي تحتاج تعديل:

### 1. `include/data/managers/function_manager.h`

**المشكلة**: `nativeImplementation_` له توقيع خاطئ

```cpp
// ❌ الحالي:
std::function<void()> nativeImplementation_;

// ✅ الصحيح:
std::function<std::shared_ptr<Data::Value>(
    const std::vector<std::shared_ptr<Data::Value>>&)> nativeImplementation_;
```

**الحل**: إضافة دالة للتحقق والاستدعاء:

```cpp
public:
    bool hasNativeImplementation() const { 
        return static_cast<bool>(nativeImplementation_); 
    }
    
    std::shared_ptr<Data::Value> callNative(
        const std::vector<std::shared_ptr<Data::Value>>& args) const {
        if (nativeImplementation_) {
            return nativeImplementation_(args);
        }
        return std::make_shared<Data::Value>();
    }
```

### 2. `src/data/managers/function_manager.cpp`

**المشكلة**: الـ Constructor والـ registerBuiltinFunction فارغة

```cpp
// الـ Constructor للدوال المضمنة:
FunctionDefinition::FunctionDefinition(
    const std::string& name,
    const std::vector<FunctionParameter>& params,
    std::function<std::shared_ptr<Data::Value>(
        const std::vector<std::shared_ptr<Data::Value>>&)> nativeImpl)
    : name_(name), type_(FunctionType::BUILTIN), 
      parameters_(params), nativeImplementation_(nativeImpl) {}

// تسجيل الدالة المضمنة:
void FunctionManager::registerBuiltinFunction(
    const std::string& name,
    const std::function<std::shared_ptr<Data::Value>(
        const std::vector<std::shared_ptr<Data::Value>>&)>& func) {
    
    removeFunction(name);
    
    // ✅ الصحيح:
    std::vector<FunctionParameter> params;
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, func);
    
    functions_[name].push_back(funcDef);
}
```

### 3. `src/interpreter/visitors/expression_evaluator.cpp`

**المشكلة**: عدم فحص وليس استدعاء native implementations

في `visitCallExpr()`:

```cpp
// قبل الاستدعاء:
if (!func) {
    throw RuntimeError("Function '" + funcName + "' not defined");
}

// ✅ أضف هذا الفحص:
if (func->hasNativeImplementation()) {
    // استدعي الـ native implementation
    lastResult_ = func->callNative(
        std::vector<std::shared_ptr<Data::Value>>(
            arguments.begin(), arguments.end()));
    return;
}

// الكود الموجود للـ AST functions:
if (!func->hasBody()) {
    throw RuntimeError("Function '" + funcName + "' has no body");
}

// ... تنفيذ الـ body ...
```

**ملاحظة**: تحويل `arguments` (التي هي `std::vector<Value>`) 
إلى `std::vector<std::shared_ptr<Value>>`:

```cpp
std::vector<std::shared_ptr<Data::Value>> argPtrs;
for (const auto& arg : arguments) {
    argPtrs.push_back(std::make_shared<Data::Value>(arg));
}
lastResult_ = func->callNative(argPtrs);
```

## 📝 خطوات التطبيق:

### الخطوة 1: تعديل function_manager.h
- تغيير توقيع `nativeImplementation_`
- إضافة `hasNativeImplementation()` و `callNative()`

### الخطوة 2: تعديل function_manager.cpp
- تعديل Constructor للدوال المضمنة
- تصحيح `registerBuiltinFunction()`

### الخطوة 3: تعديل expression_evaluator.cpp
- إضافة فحص `hasNativeImplementation()`
- استدعاء `callNative()` قبل محاولة تنفيذ الـ body

### الخطوة 4: البناء والاختبار
```bash
cmake --build . --config Release
sad.exe examples/test_print_en.s
```

**النتيجة المتوقعة**:
```
Hello World
```

## 🎬 مثال التدفق بعد التصحيح:

```
print("Hello")
    ↓
visitCallExpr() في expression_evaluator
    ↓
البحث عن 'print' → func = functionManager_.getFunction("print", 1)
    ↓
فحص: func->hasNativeImplementation() → true ✅
    ↓
استدعاء: func->callNative([StringValue("Hello")])
    ↓
BuiltinFunctions::print() → IOFunctions::print()
    ↓
std::cout << "Hello"
    ↓
✅ OUTPUT: "Hello"
```

## ⚠️ نقاط مهمة:

1. **توقيع lambda يجب أن يكون متطابقاً**:
   - يأخذ `std::vector<std::shared_ptr<Value>>&`
   - يعيد `std::shared_ptr<Value>`

2. **يجب تحويل Value إلى ValuePtr**:
   - لأن البيئة الحالية تستخدم Value مباشرة
   - لكن الدوال المضمنة تتوقع ValuePtr

3. **الترتيب مهم**:
   - فحص native implementation أولاً
   - ثم الـ AST body ثانياً
   - لأن الدوال المضمنة يجب أن تأخذ الأولوية

## 📊 التأثير:

بعد التصحيح:
- ✅ جميع الدوال المضمنة ستعمل (print, input, len, إلخ)
- ✅ Phase 1 من المكتبة القياسية سيكون كامل
- ✅ جاهز للمرحلة 2 (String Functions)
