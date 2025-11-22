# 🎨 رسم تخطيطي: بنية الدوال المضمنة

## المشكلة الحالية:

```
┌─────────────────────────────────────────────────────────────┐
│                   المستخدم: print("Hello")                 │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ PARSER: ExprStmt(CallExpr(print, [String("Hello")]))        │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ INTERPRETER: visitCallExpr()                                 │
│  - استخراج اسم: "print"                                     │
│  - تقييم معاملات: [Value("Hello")]                         │
│  - البحث: getFunction("print", 1)                          │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│ FUNCTION MANAGER: البحث في functions_ map                   │
│ ✅ وجدت: FunctionDefinition("print")                        │
│    - type_: BUILTIN                                         │
│    - nativeImplementation_: [LAMBDA]                        │
│    - body_: nullptr                                         │
└────────────────────────┬────────────────────────────────────┘
                         │
              ╔══════════╫══════════╗
              │          │          │
              ▼          ▼          ▼
    ❌ الحالي:   ✅ المتوقع  (المشكلة!)
    
  if (!func->    if (func->          
    hasBody()) {  hasNativeImpl())  
    error        {                 
  }            call it! ✅         
                }                 
```

## التدفق المفصل:

### ❌ التدفق الحالي (لا يعمل):

```
print("Hello")
   │
   ├─ Parsing ✅
   │  └─ ExprStmt(CallExpr(...))
   │
   ├─ Execution ✅
   │  └─ visitCallExpr()
   │     ├─ Extract name: "print" ✅
   │     ├─ Evaluate args: [String("Hello")] ✅
   │     ├─ Search function ✅
   │     │  └─ Found in functions_ map ✅
   │     │
   │     └─ Call function ❌
   │        ├─ Check if hasBody() ✅
   │        │  └─ false (it's builtin)
   │        │
   │        └─ Throw error ❌
   │           "(Function 'print' has no body"
   │
   └─ Result: 💥 ERROR - Nothing printed
```

### ✅ التدفق الصحيح (بعد التصحيح):

```
print("Hello")
   │
   ├─ Parsing ✅
   │  └─ ExprStmt(CallExpr(...))
   │
   ├─ Execution ✅
   │  └─ visitCallExpr()
   │     ├─ Extract name: "print" ✅
   │     ├─ Evaluate args: [String("Hello")] ✅
   │     ├─ Search function ✅
   │     │  └─ Found in functions_ map ✅
   │     │
   │     └─ Call function ✅
   │        ├─ Check if hasNativeImplementation() ✅
   │        │  └─ true
   │        │
   │        └─ Call native ✅
   │           └─ callNative([ValuePtr("Hello")])
   │              │
   │              ├─ BuiltinFunctions::print() ✅
   │              │  ├─ Convert ValuePtr → Value ✅
   │              │  │
   │              │  └─ IOFunctions::print() ✅
   │              │     ├─ Convert Value → string ✅
   │              │     │
   │              │     └─ std::cout << "Hello" ✅
   │              │
   │              └─ Return void ✅
   │
   └─ Result: ✅ OUTPUT: "Hello"
```

## بنية البيانات:

### ❌ الحالية (خاطئة):

```cpp
class FunctionDefinition {
private:
    std::string name_;
    FunctionType type_;
    std::vector<FunctionParameter> parameters_;
    std::shared_ptr<Parser::ASTNode> body_;
    
    // ❌ المشكلة:
    std::function<void()> nativeImplementation_;
    //                ↑
    //           لا يأخذ معاملات
    //           لا يعيد قيمة
    //           فارغ تماماً
};
```

### ✅ الصحيحة (بعد التصحيح):

```cpp
class FunctionDefinition {
private:
    std::string name_;
    FunctionType type_;
    std::vector<FunctionParameter> parameters_;
    std::shared_ptr<Parser::ASTNode> body_;
    
    // ✅ الصحيح:
    std::function<std::shared_ptr<Data::Value>(
        const std::vector<std::shared_ptr<Data::Value>>&)> 
    nativeImplementation_;
    //                 ↑
    //        يأخذ: vector of ValuePtr
    //        يعيد: ValuePtr
    
public:
    // ✅ دوال جديدة:
    bool hasNativeImplementation() const;
    std::shared_ptr<Data::Value> callNative(
        const std::vector<std::shared_ptr<Data::Value>>&) const;
};
```

## مراحل التسجيل:

### ❌ الحالية:

```
registerBuiltinFunction("print", lambda)
    │
    ├─ Remove old function ✅
    │
    ├─ Create wrapper:
    │  │
    │  └─ auto nativeWrapper = [func]() {
    │                               ↑
    │                      ❌ فارغ تماماً!
    │     }
    │
    ├─ Create FunctionDefinition ✅
    │
    └─ Store in functions_["print"] ✅
       └─ But nativeImplementation_ is empty ❌
```

### ✅ الصحيحة:

```
registerBuiltinFunction("print", lambda)
    │
    ├─ Remove old function ✅
    │
    ├─ Pass lambda directly ✅
    │  │
    │  └─ lambda: (ValuePtr args) → ValuePtr
    │     ├─ Convert ValuePtr → Value ✅
    │     │
    │     └─ Call IOFunctions::print() ✅
    │        └─ std::cout << "Hello" ✅
    │
    ├─ Create FunctionDefinition ✅
    │  └─ nativeImplementation_ = lambda ✅
    │
    └─ Store in functions_["print"] ✅
       └─ Ready to call! ✅
```

## المقارنة الجدولية:

| الخطوة | الحالية | المتوقع | الحل |
|------|---------|---------|------|
| **التسجيل** | ✅ يعمل | ✅ صحيح | - |
| **Wrapper** | ❌ فارغ | ✅ مع معاملات | تعديل بسيط |
| **التوقيع** | ❌ `void()` | ✅ `ValuePtr(ValuePtrVector)` | تغيير توقيع |
| **البحث** | ✅ يعمل | ✅ صحيح | - |
| **فحص Native** | ❌ غير موجود | ✅ `hasNativeImpl()` | إضافة دالة |
| **استدعاء** | ❌ لا يحدث | ✅ `callNative()` | إضافة دالة |
| **التنفيذ** | ❌ فارغ | ✅ يطبع النص | التصحيح |

## 🎯 الخلاصة:

**المشكلة**: Wrapper lambda فارغ ولا يُستدعى
**السبب**: توقيع خاطئ + عدم فحص native implementation
**الحل**: 3 ملفات تحتاج تعديل + 5 دوال جديدة
**التأثير**: جميع الدوال المضمنة ستعمل بشكل صحيح ✅
