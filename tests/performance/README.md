# 📊 Performance Benchmarks — Sad vs Python vs Node.js

قياس الأداء المقارن بين لغة ص و Python و Node.js.

## 🔧 المتطلبات

- **sad.exe** — مفسر لغة ص (يتم الكشف تلقائياً من `build/bin/`)
- **Python 3.x** — اختياري
- **Node.js** — اختياري

## 🚀 التشغيل السريع

```powershell
# تشغيل مع الإعدادات الافتراضية (5 تكرارات)
.\run_benchmarks.ps1

# 10 تكرارات للدقة
.\run_benchmarks.ps1 -Iterations 10

# تصدير JSON للـ CI
.\run_benchmarks.ps1 -OutputJson results.json

# تخطي Python
.\run_benchmarks.ps1 -SkipPython
```

## 📋 الاختبارات

| الاختبار | الوصف | التعقيد |
|----------|-------|---------|
| **Fibonacci (25)** | حساب فيبوناتشي تكراري | O(2^n) |
| **Loop Sum (100K)** | مجموع أرقام 1-100000 | O(n) |
| **Array Operations (10K)** | إنشاء وجمع مصفوفة | O(n) |
| **String Concat (1K)** | دمج نصوص 1000 مرة | O(n²) |
| **Class/Object (5K)** | إنشاء 5000 كائن | O(n) |

## 📁 بنية الملفات

```
tests/performance/
├── run_benchmarks.ps1      # سكريبت التشغيل الرئيسي
├── README.md               # هذا الملف
└── benchmarks/             # ملفات الاختبار (تُنشأ تلقائياً)
    ├── fib.ص / fib.py / fib.js
    ├── loop.ص / loop.py / loop.js
    ├── array.ص / array.py / array.js
    ├── string.ص / string.py / string.js
    └── class.ص / class.py / class.js
```

## 📊 مثال الإخراج

```
  الاختبار / Test              Sad (ms)      Python     Node.js
  -------------------------------------------------------------
  Fibonacci (25)                  234.5       312.8        45.2
  Loop Sum (100K)                  45.0        82.3        12.1
  Array Operations (10K)           67.8        45.2        18.5
  String Concat (1K)              123.4        89.7        34.2
  Class/Object (5K)               189.2       156.8        67.3
```

## 🔄 دمج CI/CD

```yaml
# في .github/workflows/nightly.yml
- name: Run Performance Benchmarks
  run: |
    cd tests/performance
    pwsh -File run_benchmarks.ps1 -Iterations 3 -OutputJson benchmark_results.json

- name: Upload Benchmark Results
  uses: actions/upload-artifact@v4
  with:
    name: benchmark-results
    path: tests/performance/benchmark_results.json
```

## 📈 تفسير النتائج

- **أرقام أقل = أفضل** (الوقت بالميلي ثانية)
- Fibonacci يختبر أداء الاستدعاء التكراري
- Loop/Array يختبران أداء الحلقات
- String يختبر أداء التعامل مع النصوص
- Class يختبر أداء OOP

## 🔧 إضافة اختبارات جديدة

1. أنشئ الملفات الثلاثة: `name.ص`, `name.py`, `name.js`
2. أضف استدعاء في `run_benchmarks.ps1`:

```powershell
Run-Benchmark "Test Name" `
    (Join-Path $BenchDir "name.ص") `
    (Join-Path $BenchDir "name.py") `
    (Join-Path $BenchDir "name.js")
```

---

**المؤلف:** عمر — مهندس البنية التحتية  
**التاريخ:** 2026-03-06
