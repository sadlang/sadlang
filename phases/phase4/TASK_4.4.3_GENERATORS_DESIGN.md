# Phase 4.4.3: Generators & Yield - خطة التصميم
# Generators & Yield - Design Plan

**التاريخ / Date:** 21 نوفمبر 2025  
**الحالة / Status:** 🔄 قيد التصميم / In Design  
**الأولوية / Priority:** 🟡 متوسطة / Medium

---

## 🎯 نظرة عامة / Overview

**Generators** هي دوال خاصة تستخدم `yield` لإنتاج قيم بشكل كسول (lazy evaluation). بدلاً من إرجاع جميع القيم مرة واحدة، تُنتج Generator قيمة واحدة في كل مرة وتحفظ حالتها للاستدعاء التالي.

Generators are special functions that use `yield` to produce values lazily. Instead of returning all values at once, a Generator produces one value at a time and saves its state for the next call.

---

## 🧩 المكونات الأساسية / Core Components

### 1. YieldStmt (موجود ✅)

```cpp
class YieldStmt : public Statement {
public:
    ExprPtr value;         // القيمة المُنتجة
    bool isYieldFrom;      // هل هو yield from؟
    
    YieldStmt(ExprPtr val = nullptr);
};
```

**الحالة:** موجود في AST ✅

### 2. GeneratorState (جديد ⏳)

```cpp
class GeneratorState {
public:
    // حالة Generator
    std::shared_ptr<AST::Statement> body;           // جسم الدالة
    std::map<std::string, Value> localVariables;    // المتغيرات المحلية
    size_t executionPoint;                          // نقطة التنفيذ الحالية
    bool isExhausted;                               // هل انتهى؟
    Value lastYieldedValue;                         // آخر قيمة yield
    
    // العمليات
    Value next();           // الحصول على القيمة التالية
    bool hasNext() const;   // هل هناك قيم أخرى؟
    void reset();           // إعادة تعيين Generator
};
```

**الحالة:** يجب إنشاؤه ⏳

### 3. GeneratorManager (جديد ⏳)

```cpp
class GeneratorManager {
public:
    // تسجيل generator
    void registerGenerator(const std::string& name, 
                          std::shared_ptr<GeneratorState> state);
    
    // الحصول على generator
    std::shared_ptr<GeneratorState> getGenerator(const std::string& name);
    
    // حذف generator
    void removeGenerator(const std::string& name);
    
private:
    std::unordered_map<std::string, std::shared_ptr<GeneratorState>> generators_;
};
```

**الحالة:** يجب إنشاؤه ⏳

---

## 📋 التصميم المبسط / Simplified Design

نظراً لتعقيد Generators الكامل، سنبدأ بتصميم **مبسط** يدعم الحالات الأساسية:

### المرحلة 1: Generators البسيطة (سننفذها الآن)

```s
// Generator بسيط
func countdown(n) {
    while n > 0 {
        yield n;
        n = n - 1;
    }
}

// الاستخدام
var gen = countdown(5);
print(gen.next());  // 5
print(gen.next());  // 4
print(gen.next());  // 3
```

**التحديات:**
1. ⚠️ **حفظ الحالة بين استدعاءات** - معقد جداً
2. ⚠️ **استئناف التنفيذ من نقطة معينة** - يحتاج Program Counter
3. ⚠️ **المتغيرات المحلية** - يجب حفظها واستعادتها

### الحل المقترح: تصميم أبسط

بدلاً من التنفيذ الكامل، سننفذ **Generator محدود** بالقيود التالية:

```s
// ✅ مدعوم: Generator بسيط مع yield في حلقة
func simple_gen() {
    var i = 0;
    while i < 5 {
        yield i;
        i = i + 1;
    }
}

// ❌ غير مدعوم: Nested loops، conditionals معقدة
func complex_gen() {
    for i in range(10) {
        for j in range(10) {
            yield i * j;  // معقد جداً
        }
    }
}
```

---

## 🔧 التنفيذ المقترح / Proposed Implementation

### الخيار 1: Generator كـ Iterator (بسيط ⭐)

```cpp
class SimpleGenerator {
    std::function<Value()> nextFunc;
    bool exhausted = false;
    
public:
    Value next() {
        if (exhausted) return Value();  // null
        return nextFunc();
    }
};
```

**المميزات:**
- ✅ بسيط جداً
- ✅ لا يحتاج state management معقد
- ✅ يعمل للحالات البسيطة

**العيوب:**
- ❌ محدود جداً
- ❌ لا يدعم حالات معقدة
- ❌ لا يحفظ متغيرات محلية

### الخيار 2: Generator مع State Management (معقد)

```cpp
class FullGenerator {
    // حفظ كامل لحالة التنفيذ
    AST::Statement* body;
    std::map<std::string, Value> locals;
    std::vector<size_t> executionStack;  // call stack
    size_t programCounter;
    
public:
    Value next() {
        // استئناف التنفيذ من PC
        // تنفيذ حتى yield
        // حفظ الحالة
        // إرجاع القيمة
    }
};
```

**المميزات:**
- ✅ دعم كامل للـ Generators
- ✅ يحفظ جميع الحالات
- ✅ يدعم حالات معقدة

**العيوب:**
- ❌ معقد جداً (100+ سطر)
- ❌ يحتاج Program Counter
- ❌ صعب التنفيذ والاختبار

### القرار: **نهج هجين** 🎯

سننفذ Generator **محدود** يدعم:
1. ✅ Yield في حلقة بسيطة (while/for)
2. ✅ المتغيرات المحلية البسيطة
3. ✅ next() protocol
4. ❌ لا نتعامل مع nested loops معقدة
5. ❌ لا نحفظ call stack

---

## 📐 التصميم المُختار / Chosen Design

### 1. Generator Detection

عند تنفيذ FunctionDecl، نتحقق من وجود YieldStmt:

```cpp
bool hasYield(AST::Statement* body) {
    // نبحث عن YieldStmt في الشجرة
    // إذا وُجد، هذه generator function
}
```

### 2. Generator Execution

عند استدعاء generator function:

```cpp
// بدلاً من تنفيذ الدالة مباشرةً
// نُنشئ GeneratorObject ونعيده
Value callGenerator(FunctionDefinition* func) {
    auto genObj = std::make_shared<GeneratorObject>(func);
    return Value::createGenerator(genObj);
}
```

### 3. next() Method

```cpp
Value GeneratorObject::next() {
    if (exhausted) return Value();  // null
    
    // تنفيذ الجسم حتى yield أو return
    while (!exhausted) {
        // execute one statement
        if (currentStmt is YieldStmt) {
            lastValue = evaluateYield();
            moveToNextStatement();
            return lastValue;
        }
        
        if (noMoreStatements) {
            exhausted = true;
            return Value();
        }
        
        moveToNextStatement();
    }
}
```

---

## 🎬 خطة التنفيذ / Implementation Plan

### المرحلة 1: التحضير (30 دقيقة)

1. ✅ قراءة YieldStmt definition
2. ⏳ إنشاء GeneratorObject class بسيط
3. ⏳ إضافة GENERATOR type إلى Value (اختياري)

### المرحلة 2: التنفيذ الأساسي (1-2 ساعة)

4. ⏳ تنفيذ visitYieldStmt في StatementExecutor
5. ⏳ تعديل FunctionManager للتعامل مع generators
6. ⏳ إضافة next() method

### المرحلة 3: الاختبار (1 ساعة)

7. ⏳ كتابة 4+ اختبارات
8. ⏳ إصلاح الأخطاء
9. ⏳ التحقق من النجاح

### المرحلة 4: التوثيق (30 دقيقة)

10. ⏳ كتابة تقرير الإكمال
11. ⏳ أمثلة وشرح

---

## ⚠️ التحديات المتوقعة / Expected Challenges

### 1. State Management

**المشكلة:** كيف نحفظ حالة التنفيذ بين next() calls؟

**الحل المقترح:**
- نستخدم iterator pattern بسيط
- نحفظ فقط المتغيرات المحلية
- لا نحفظ call stack (محدود)

### 2. Program Counter

**المشكلة:** كيف نعرف من أين نستأنف؟

**الحل المقترح:**
- نستخدم index في statements list
- نحفظ currentStatementIndex

### 3. Nested Loops

**المشكلة:** كيف نتعامل مع yield في nested loops؟

**الحل المقترح:**
- ⚠️ **لن ندعمها في المرحلة الأولى**
- نقتصر على حلقة واحدة

---

## 📊 مثال عملي / Working Example

```s
// Generator بسيط
func counter(max) {
    var i = 0;
    while i < max {
        yield i;
        i = i + 1;
    }
}

// الاستخدام
var gen = counter(3);
print(gen.next());  // 0
print(gen.next());  // 1
print(gen.next());  // 2
print(gen.next());  // null (exhausted)
```

### التنفيذ الداخلي:

```
1. counter(3) يُستدعى
2. تُنشأ GeneratorObject مع:
   - body = WhileStmt
   - locals = {max: 3, i: 0}
   - exhausted = false

3. gen.next() أول مرة:
   - تنفيذ: i = 0
   - تنفيذ: while i < max (true)
   - تنفيذ: yield i → return 0
   - حفظ: i = 0، currentIndex = 2

4. gen.next() ثاني مرة:
   - استئناف من index 2
   - تنفيذ: i = i + 1 → i = 1
   - تنفيذ: while i < max (true)
   - تنفيذ: yield i → return 1
   - حفظ: i = 1، currentIndex = 2

5. gen.next() ثالث مرة:
   - استئناف من index 2
   - تنفيذ: i = i + 1 → i = 2
   - تنفيذ: while i < max (true)
   - تنفيذ: yield i → return 2
   - حفظ: i = 2، currentIndex = 2

6. gen.next() رابع مرة:
   - استئناف من index 2
   - تنفيذ: i = i + 1 → i = 3
   - تنفيذ: while i < max (false) → خروج
   - exhausted = true
   - return null
```

---

## 🤔 قرار نهائي / Final Decision

بعد التحليل، نواجه خيارين:

### الخيار A: تنفيذ مبسط جداً (2-3 ساعات)
- ✅ يعمل للحالات البسيطة
- ❌ محدود جداً
- ❌ قد لا يكون مفيداً عملياً

### الخيار B: تنفيذ متوسط التعقيد (4-6 ساعات)
- ✅ يدعم معظم الحالات
- ✅ مفيد عملياً
- ⚠️ معقد قليلاً

### الخيار C: تأجيل للمستقبل (0 ساعات)
- ✅ نركز على ميزات أخرى
- ✅ Generators ميزة متقدمة جداً
- ⚠️ نؤجلها لـ Phase 5 أو بعد

---

## ✅ التوصية / Recommendation

**أقترح الخيار C: تأجيل Generators للمستقبل**

**الأسباب:**
1. Generators معقدة جداً وتحتاج state management متقدم
2. Lambda و Comprehensions أكثر أهمية وأبسط
3. لدينا بالفعل 243 اختبار ناجح و98% من Phase 4 مكتمل
4. يمكننا التركيز على Phase 5 (OOP) وهي أكثر أهمية

**البديل:**
- نوثق التصميم فقط (هذا الملف)
- نتركه للتنفيذ المستقبلي
- ننتقل إلى Phase 5 أو ميزات أخرى

---

**هل توافق على التأجيل أم تريد المحاولة بتنفيذ مبسط؟** 🤔
