# شرح بنية الدوال المضمنة في مشروع لغة ص

## 📁 الملفات المشاركة:

### 1. تسجيل الدوال (Registration)
**ملف**: `src/interpreter/core/builtin_registry.cpp`
```
- يحتوي على registerBuiltinFunctions()
- يسجل جميع الدوال المدمجة في FunctionManager
- مثال: interpreter.getFunctionManager().registerBuiltinFunction("print", print_func)
```

### 2. واجهة الدوال المضمنة (Interface)
**ملفات**: 
- `include/stdlib/core/builtins.h` - تصريحات
- `src/stdlib/core/builtins.cpp` - تنفيذ wrapper functions

```
التدفق:
  registerBuiltinFunction("print", lambda)
    ↓
  lambda → BuiltinFunctions::print(ValuePtr args)
    ↓
  (تحويل من ValuePtr إلى Value)
    ↓
  IOFunctions::print(Value args)
    ↓
  std::cout << value
```

### 3. تنفيذ الدوال الفعلي (Implementation)
**ملفات**:
- `include/stdlib/io/io_functions.h` - الواجهة
- `src/stdlib/io/io_functions.cpp` - التنفيذ الحقيقي

```cpp
Data::Value IOFunctions::print(const std::vector<Data::Value>& args) {
    for (const auto& arg : args) {
        std::cout << arg.toString();
    }
    std::cout.flush();
    return Data::Value();  // VOID
}
```

### 4. إدارة الدوال (Function Manager)
**ملفات**:
- `include/data/managers/function_manager.h` - واجهة
- `src/data/managers/function_manager.cpp` - التنفيذ

```
المسؤولية:
  - تخزين تعريفات الدوال
  - البحث عن الدوال حسب الاسم والعدد
  - استدعاء الدوال
```

### 5. تنفيذ الاستدعاءات (Call Execution)
**ملف**: `src/interpreter/visitors/expression_evaluator.cpp`

```cpp
void ExpressionEvaluator::visitCallExpr(CallExpr& node) {
    // 1. استخراج اسم الدالة
    std::string funcName = calleeVar->name;
    
    // 2. تقييم المعاملات
    std::vector<Data::Value> arguments;
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
        arguments.push_back(lastResult_);
    }
    
    // 3. البحث عن الدالة
    auto func = functionManager_.getFunction(funcName, arguments.size());
    
    // 4. استدعاء الدالة
    if (func->hasBody()) {
        // تنفيذ الـ body (AST)
    }
}
```

## 🔴 المشاكل الموجودة:

### المشكلة 1: `registerBuiltinFunction()` فارغة! ❌

**الملف**: `src/data/managers/function_manager.cpp` (السطر 458)

```cpp
void FunctionManager::registerBuiltinFunction(
    const std::string& name,
    const std::function<std::shared_ptr<Data::Value>
    (const std::vector<std::shared_ptr<Data::Value>>&)>& func) {
    
    removeFunction(name);
    
    // ❌ هنا المشكلة:
    auto nativeWrapper = [func]() {
        // فارغ تماماً!
        // لم يتم تمرير المعاملات ولا استدعاء func!
    };
    
    std::vector<FunctionParameter> params;
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, nativeWrapper);
    functions_[name].push_back(funcDef);
}
```

**التأثير**:
- الدوال تُسجل في `functions_` map لكن
- `nativeWrapper` فارغ فلا يفعل شيء
- عند استدعاء الدالة، لا شيء يحدث!

### المشكلة 2: عدم استدعاء `native implementations`

**الملف**: `src/interpreter/visitors/expression_evaluator.cpp` (السطر 421)

```cpp
void ExpressionEvaluator::visitCallExpr(CallExpr& node) {
    // ... كود البحث عن الدالة ...
    
    if (!func) {
        throw RuntimeError("Function '" + funcName + "' not defined");
    }
    
    // ❌ هنا المشكلة:
    if (!func->hasBody()) {
        throw RuntimeError("Function '" + funcName + "' has no body");
    }
    
    // تنفيذ الـ body فقط (AST)
    // لا يوجد فحص للـ native implementation!
}
```

**التأثير**:
- حتى لو كانت `nativeWrapper` صحيحة
- لن يتم استدعاؤها!
- لأن الكود يفحص `hasBody()` فقط

### المشكلة 3: عدم تمرير المعاملات للـ lambda

```cpp
// ما الذي يجب أن يحدث:
auto nativeWrapper = [func](const std::vector<std::shared_ptr<Value>>& args) {
    return func(args);
};

// ما هو الموجود الآن:
auto nativeWrapper = [func]() {
    // لا يأخذ معاملات!
    // لا يستدعي func!
};
```

## 📊 جدول المقارنة:

```
الخطوة              | الحالة الحالية      | المتوقع
─────────────────────────────────────────────────────────
1. التسجيل          | ✅ يعمل             | ✅ صحيح
2. Wrapper          | ❌ فارغ             | ❌ لا يمرر معاملات
3. البحث عن الدالة   | ✅ يعمل             | ✅ صحيح
4. فحص hasBody()    | ✅ يعمل لكن         | ❌ لا يفحص native impl
5. الاستدعاء        | ❌ لا يحدث          | ❌ لا يستدعي lambdas
```

## 🔧 الحلول الممكنة:

### الحل 1: تصحيح `registerBuiltinFunction()` ✅ (الأسهل)

```cpp
void FunctionManager::registerBuiltinFunction(
    const std::string& name,
    const std::function<std::shared_ptr<Data::Value>
    (const std::vector<std::shared_ptr<Data::Value>>&)>& func) {
    
    removeFunction(name);
    
    // ✅ الحل الصحيح:
    auto nativeWrapper = [func](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return func(args);  // استدعي lambda مع المعاملات
    };
    
    std::vector<FunctionParameter> params;
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, nativeWrapper);
    functions_[name].push_back(funcDef);
}
```

لكن هذا لن يعمل لأن `std::function<void()>` لا تأخذ معاملات!

### الحل 2: تغيير توقيع `nativeImplementation_` ✅ (الأفضل)

في `function_manager.h`:

```cpp
class FunctionDefinition {
private:
    // ❌ الحالي:
    std::function<void()> nativeImplementation_;
    
    // ✅ الصحيح:
    std::function<std::shared_ptr<Data::Value>
    (const std::vector<std::shared_ptr<Data::Value>>&)> nativeImplementation_;
};
```

ثم في `expression_evaluator.cpp`:

```cpp
if (func->hasNativeImplementation()) {
    lastResult_ = func->callNative(arguments);
} else {
    // تنفيذ AST العادي
}
```

### الحل 3: خريطة منفصلة للدوال المضمنة ✅ (البديل)

بدلاً من استخدام `nativeImplementation_`، أضف:

```cpp
class FunctionManager {
private:
    std::map<std::string, 
        std::function<std::shared_ptr<Data::Value>
        (const std::vector<std::shared_ptr<Data::Value>>&)>> builtinFunctions_;
};
```

## 📈 الخلاصة:

| المرحلة | الحالة |
|--------|--------|
| **التسجيل** | ✅ يعمل |
| **التحويل (Wrapper)** | ❌ فارغ وغير صحيح |
| **البحث** | ✅ يعمل |
| **الاستدعاء** | ❌ لا يستدعي native impl |
| **التنفيذ** | ❌ لا يحدث شيء |

**النتيجة**: الدوال المضمنة تُسجل بنجاح لكن لا تُستدعى أبداً! 🚫
