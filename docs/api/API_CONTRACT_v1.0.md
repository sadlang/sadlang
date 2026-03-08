# Sad Language API v1.0 - Contract Definition
# تعريف عقد واجهة برمجة التطبيقات v1.0

> **Status:** FROZEN ❄️  
> **Date:** December 2026  
> **Version:** 1.0.0

---

## Overview

This document defines the stable public API for Sad Language v1.0. All interfaces documented here are **frozen** and will maintain backward compatibility for all 1.x releases.

---

## Core Runtime API

### Value Types

```cpp
namespace Sad::Data {
    enum class ValueType {
        NIL,        // لاشيء
        BOOLEAN,    // منطقي
        INTEGER,    // رقم
        FLOAT,      // عشري
        STRING,     // نص
        ARRAY,      // مصفوفة
        MAP,        // خريطة
        OBJECT,     // كائن
        FUNCTION,   // دالة
        CLOSURE,    // إغلاق
        // Reserved for future expansion
        USERDATA = 100,
        RESERVED_200 = 200
    };
    
    class Value {
    public:
        // STABLE: Core constructors
        Value();
        Value(std::nullptr_t);
        Value(bool val);
        Value(int64_t val);
        Value(double val);
        Value(const std::string& val);
        Value(const std::vector<Value>& val);
        Value(const std::map<std::string, Value>& val);
        
        // STABLE: Type checking
        ValueType type() const;
        bool isNil() const;
        bool isBoolean() const;
        bool isNumber() const;
        bool isInteger() const;
        bool isFloat() const;
        bool isString() const;
        bool isArray() const;
        bool isMap() const;
        bool isObject() const;
        bool isFunction() const;
        bool isCallable() const;
        
        // STABLE: Value extraction
        bool toBool() const;
        int64_t toInteger() const;
        double toFloat() const;
        const std::string& toString() const;
        std::vector<Value>& toArray();
        std::map<std::string, Value>& toMap();
        
        // STABLE: Operators
        bool operator==(const Value& other) const;
        bool operator!=(const Value& other) const;
        bool operator<(const Value& other) const;
        
        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
    };
}
```

### Interpreter API

```cpp
namespace Sad::Interpreter {
    class InterpreterCore {
    public:
        // STABLE: Lifecycle
        InterpreterCore();
        ~InterpreterCore();
        
        // STABLE: Execution
        Value execute(const std::string& code);
        Value executeFile(const std::string& path);
        
        // STABLE: Variables
        void setGlobal(const std::string& name, const Value& value);
        Value getGlobal(const std::string& name) const;
        bool hasGlobal(const std::string& name) const;
        
        // STABLE: Functions
        void registerFunction(const std::string& name, NativeFunction fn);
        Value call(const std::string& name, const std::vector<Value>& args);
        
        // STABLE: Error handling
        std::string getLastError() const;
        void clearError();
        
        // STABLE: Configuration
        void setOption(const std::string& key, const Value& value);
        Value getOption(const std::string& key) const;
    };
    
    using NativeFunction = std::function<Value(const std::vector<Value>&)>;
}
```

### Compiler API

```cpp
namespace Sad::Compiler {
    enum class OptLevel { O0, O1, O2, O3, Os, Oz };
    enum class Target { NATIVE, WASM, LLVM_IR, SIR };
    
    struct CompileOptions {
        OptLevel optimization = OptLevel::O0;
        Target target = Target::NATIVE;
        bool debug = false;
        bool verbose = false;
        std::string outputPath;
        std::vector<std::string> includePaths;
        std::vector<std::string> libraryPaths;
    };
    
    class Compiler {
    public:
        // STABLE: Lifecycle
        Compiler();
        ~Compiler();
        
        // STABLE: Compilation
        bool compile(const std::string& source, const CompileOptions& opts);
        bool compileFile(const std::string& path, const CompileOptions& opts);
        
        // STABLE: Errors
        std::vector<Error> getErrors() const;
        std::vector<Warning> getWarnings() const;
        
        // STABLE: Info
        std::string getVersion() const;
        std::vector<std::string> getSupportedTargets() const;
    };
}
```

---

## Standard Library API

### Core Module (stdlib/core)

```sad
# STABLE: Print functions
دالة اطبع(قيمة: أي) -> فراغ
دالة اطبع_سطر(قيمة: أي) -> فراغ

# STABLE: Input functions
دالة اقرأ() -> نص
دالة اقرأ_رقم() -> رقم

# STABLE: Type functions
دالة نوع(قيمة: أي) -> نص
دالة طول(قيمة: أي) -> رقم

# STABLE: Conversion functions
دالة نص(قيمة: أي) -> نص
دالة رقم(قيمة: نص) -> رقم
دالة عشري(قيمة: نص) -> عشري
دالة منطقي(قيمة: أي) -> منطقي
```

### IO Module (stdlib/io)

```sad
# STABLE: File operations
دالة io.اقرأ_ملف(مسار: نص) -> نص
دالة io.اكتب_ملف(مسار: نص, محتوى: نص) -> منطقي
دالة io.أضف_لملف(مسار: نص, محتوى: نص) -> منطقي
دالة io.موجود(مسار: نص) -> منطقي
دالة io.احذف(مسار: نص) -> منطقي
دالة io.أنشئ_مجلد(مسار: نص) -> منطقي
دالة io.قائمة_الملفات(مسار: نص) -> مصفوفة[نص]
```

### Math Module (stdlib/math)

```sad
# STABLE: Constants
ثابت math.ط: عشري  # PI
ثابت math.هـ: عشري  # E

# STABLE: Functions
دالة math.جذر(س: عشري) -> عشري
دالة math.قوة(أساس: عشري, أس: عشري) -> عشري
دالة math.جا(زاوية: عشري) -> عشري
دالة math.جتا(زاوية: عشري) -> عشري
دالة math.ظا(زاوية: عشري) -> عشري
دالة math.مطلق(س: عشري) -> عشري
دالة math.أدنى(أ: عشري, ب: عشري) -> عشري
دالة math.أقصى(أ: عشري, ب: عشري) -> عشري
دالة math.تقريب(س: عشري) -> رقم
دالة math.أرضية(س: عشري) -> رقم
دالة math.سقف(س: عشري) -> رقم
دالة math.لو(س: عشري) -> عشري
دالة math.لو10(س: عشري) -> عشري
```

### String Module (stdlib/string)

```sad
# STABLE: String operations
دالة string.قص(نص: نص) -> نص
دالة string.علوي(نص: نص) -> نص
دالة string.سفلي(نص: نص) -> نص
دالة string.يحتوي(نص: نص, جزء: نص) -> منطقي
دالة string.يبدأ_ب(نص: نص, بداية: نص) -> منطقي
دالة string.ينتهي_ب(نص: نص, نهاية: نص) -> منطقي
دالة string.استبدل(نص: نص, قديم: نص, جديد: نص) -> نص
دالة string.قسّم(نص: نص, فاصل: نص) -> مصفوفة[نص]
دالة string.اربط(مصفوفة: مصفوفة[نص], فاصل: نص) -> نص
دالة string.موضع(نص: نص, جزء: نص) -> رقم
دالة string.قطع(نص: نص, من: رقم, إلى: رقم) -> نص
```

### JSON Module (stdlib/json)

```sad
# STABLE: JSON operations
دالة json.حلل(نص: نص) -> أي
دالة json.نصّص(قيمة: أي) -> نص
دالة json.نصّص_منسق(قيمة: أي) -> نص
```

### HTTP Module (stdlib/http)

```sad
# STABLE: HTTP operations
دالة http.احصل(رابط: نص) -> استجابة
دالة http.انشر(رابط: نص, بيانات: أي) -> استجابة
دالة http.حمّل(رابط: نص, مسار: نص) -> منطقي

صنف استجابة
    خاصية الجسم: نص
    خاصية الحالة: رقم
    خاصية الرؤوس: خريطة[نص, نص]
نهاية
```

---

## Deprecated APIs

The following APIs are deprecated and will be removed in v2.0:

| API | Replacement | Removal |
|-----|-------------|---------|
| `طباعة()` | `اطبع()` | v2.0 |
| `قائمة` | `مصفوفة` | v2.0 |

---

## Versioning Policy

1. **Patch releases (1.0.x):** Bug fixes only, no API changes
2. **Minor releases (1.x.0):** New features, backward compatible
3. **Major releases (2.0.0):** Breaking changes allowed

---

## Compatibility Matrix

| Sad Version | C++ Standard | LLVM Version | Windows | macOS | Linux |
|-------------|--------------|--------------|---------|-------|-------|
| 1.0.x | C++17 | 18.x | 10+ | 12+ | Ubuntu 20.04+ |
| 1.1.x | C++17 | 18.x-19.x | 10+ | 12+ | Ubuntu 20.04+ |

---

## ABI Stability

The following are ABI-stable:
- `Value` class layout
- `NativeFunction` signature
- Error codes
- Exit codes

---

## Contract Violations

If you find a contract violation, please report it to:
- GitHub Issues: https://github.com/sad-lang/sad/issues
- Email: api@sad-lang.org
