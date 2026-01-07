# 🧪 استراتيجية الاختبار

## 📅 التاريخ: مستمر طوال المشروع

---

## 🎯 الهدف

ضمان جودة المترجم من خلال اختبارات شاملة ومنهجية.

---

## 📋 أنواع الاختبارات

### 1. اختبارات الوحدة (Unit Tests)

**الهدف:** اختبار كل مكون بشكل منفصل

**المكونات:**
| المكون | الملف | الاختبارات |
|--------|-------|----------|
| Lexer | `lexer_core.cpp` | تحليل الرموز |
| Parser | `parser_core_impl.cpp` | بناء AST |
| SIR Builder | `sir_builder.cpp` | توليد SIR |
| LLVM Codegen | `llvm_codegen.cpp` | توليد LLVM IR |
| Type Checker | `type_checker.cpp` | فحص الأنواع |

**موقع الاختبارات:** `tests/` مجلد

### 2. اختبارات التكامل (Integration Tests)

**الهدف:** اختبار تدفق البيانات بين المكونات

**السيناريوهات:**
- Source → Lexer → Parser → AST
- AST → SIR Builder → SIR
- SIR → LLVM Codegen → IR → Executable

### 3. اختبارات من طرف لطرف (E2E Tests)

**الهدف:** اختبار البرامج الكاملة

**الأسلوب:**
1. كتابة برنامج بلغة ص
2. ترجمته
3. تنفيذه
4. التحقق من النتيجة

### 4. اختبارات الأداء (Performance Tests)

**الهدف:** قياس سرعة الترجمة والتنفيذ

**المقاييس:**
- وقت الترجمة
- وقت التنفيذ
- استخدام الذاكرة

### 5. اختبارات الانحدار (Regression Tests)

**الهدف:** منع عودة الأخطاء المُصلحة

---

## 📁 هيكل ملفات الاختبار

```
tests/
├── unit/
│   ├── lexer/
│   │   ├── test_arabic_keywords.cpp
│   │   ├── test_numbers.cpp
│   │   ├── test_strings.cpp
│   │   └── test_operators.cpp
│   ├── parser/
│   │   ├── test_expressions.cpp
│   │   ├── test_statements.cpp
│   │   └── test_functions.cpp
│   ├── sir/
│   │   ├── test_arithmetic.cpp
│   │   ├── test_control_flow.cpp
│   │   └── test_function_calls.cpp
│   └── codegen/
│       └── test_llvm_codegen.cpp
├── integration/
│   ├── test_pipeline.cpp
│   └── test_modules.cpp
├── e2e/
│   ├── basic/
│   │   ├── test_hello.s
│   │   ├── test_arithmetic.s
│   │   └── test_variables.s
│   ├── functions/
│   │   ├── test_simple_func.s
│   │   ├── test_recursion.s
│   │   └── test_closures.s
│   ├── control/
│   │   ├── test_if.s
│   │   ├── test_while.s
│   │   └── test_for.s
│   ├── types/
│   │   ├── test_arrays.s
│   │   ├── test_maps.s
│   │   └── test_strings.s
│   └── oop/
│       ├── test_classes.s
│       ├── test_inheritance.s
│       └── test_polymorphism.s
└── performance/
    ├── bench_compile_time.cpp
    └── bench_runtime.cpp
```

---

## 📝 قالب ملف الاختبار (E2E)

```
// ملف: tests/e2e/basic/test_arithmetic.s
// الوصف: اختبار العمليات الحسابية الأساسية
// المتوقع: exit_code=42

دالة رئيسية()
    رقم أ = 10
    رقم ب = 32
    ارجع أ + ب
نهاية
```

**ملف التوقعات:**
```json
// tests/e2e/basic/test_arithmetic.expected.json
{
    "exit_code": 42,
    "stdout": "",
    "stderr": ""
}
```

---

## 🔄 سير عمل الاختبار

### 1. عند كتابة ميزة جديدة

```
1. كتابة الاختبار أولاً (TDD)
2. تنفيذ الميزة
3. التأكد من نجاح الاختبار
4. إضافة حالات حدودية
```

### 2. قبل الدمج (Merge)

```
1. تشغيل جميع اختبارات الوحدة
2. تشغيل اختبارات التكامل
3. تشغيل اختبارات E2E
4. فحص التغطية
```

### 3. ليلياً

```
1. تشغيل جميع الاختبارات
2. اختبارات الأداء
3. تقرير التغطية
```

---

## 📊 مصفوفة تغطية الاختبارات

| الميزة | وحدة | تكامل | E2E |
|--------|------|-------|-----|
| المتغيرات | ✅ | ✅ | ✅ |
| الثوابت | ✅ | ✅ | ✅ |
| العمليات الحسابية | ✅ | ✅ | ✅ |
| العمليات المنطقية | ✅ | ✅ | ✅ |
| الشروط (if) | ⏳ | ⏳ | ⏳ |
| الحلقات (while) | ⏳ | ⏳ | ⏳ |
| الحلقات (for) | ⏳ | ⏳ | ⏳ |
| الدوال | ⏳ | ⏳ | ⏳ |
| الأصناف | ⏳ | ⏳ | ⏳ |
| الوراثة | ⏳ | ⏳ | ⏳ |
| المصفوفات | ⏳ | ⏳ | ⏳ |
| القواميس | ⏳ | ⏳ | ⏳ |

**الرموز:**
- ✅ مكتمل
- ⏳ قيد الانتظار
- ❌ فاشل

---

## 🛠️ أدوات الاختبار

### 1. Google Test

للاختبارات المكتوبة بـ C++

```cpp
TEST(SIRBuilderTest, BuildBinaryAdd) {
    // Setup
    auto expr = createAddExpr(1, 2);
    SIRBuilder builder;
    
    // Execute
    auto result = builder.buildBinaryOp(expr);
    
    // Verify
    EXPECT_EQ(result.opcode, SIROpcode::ADD_I64);
}
```

### 2. سكربت الاختبار

للاختبارات E2E

```powershell
# run_tests.ps1
param([string]$filter = "*")

$tests = Get-ChildItem "tests/e2e/**/*.s" | Where-Object { $_.Name -like $filter }

foreach ($test in $tests) {
    # Compile
    & ./build/bin/Debug/sad.exe $test.FullName -o temp.exe
    
    # Run
    $result = & ./temp.exe
    $exitCode = $LASTEXITCODE
    
    # Check
    $expected = Get-Content ($test.FullName -replace ".s$", ".expected.json") | ConvertFrom-Json
    
    if ($exitCode -eq $expected.exit_code) {
        Write-Host "✅ $($test.Name)" -ForegroundColor Green
    } else {
        Write-Host "❌ $($test.Name) - Expected: $($expected.exit_code), Got: $exitCode" -ForegroundColor Red
    }
}
```

---

## 📈 أهداف التغطية

| المرحلة | هدف التغطية |
|---------|-------------|
| المرحلة 1 | 90% |
| المرحلة 2 | 85% |
| المرحلة 3 | 85% |
| المرحلة 4 | 80% |
| المرحلة 5 | 80% |
| المرحلة 6 | 75% |
| المرحلة 7 | 70% |
| المرحلة 8 | 70% |

---

## 🐛 تتبع الأخطاء

### قالب تقرير الخطأ

```markdown
## وصف الخطأ
[وصف مختصر]

## خطوات إعادة الإنتاج
1. ...
2. ...

## السلوك المتوقع
[ما كان يجب أن يحدث]

## السلوك الفعلي
[ما حدث فعلاً]

## كود الاختبار
```s
// الكود الذي يُظهر الخطأ
```

## معلومات إضافية
- نظام التشغيل: ...
- إصدار المترجم: ...
```

---

## 📅 جدول الاختبارات

| النشاط | التكرار |
|--------|---------|
| اختبارات الوحدة | عند كل تغيير |
| اختبارات التكامل | قبل الدمج |
| اختبارات E2E | يومياً |
| اختبارات الأداء | أسبوعياً |
| مراجعة التغطية | شهرياً |
