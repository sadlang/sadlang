# المرحلة 6: تحليل مجلد tests

## 📅 التاريخ: 8 يناير 2026

---

## 📊 التكرار المكتشف

### 1. OOP Tests
- `tests/oop/` - اختبارات OOP
- `tests/oop_tests/` - **مكرر**

### 2. Parser Tests
- `tests/parser/` - اختبارات Parser
- `tests/parser_tests/` - **مكرر**

### 3. Stdlib Tests
- `tests/stdlib/` - اختبارات المكتبة القياسية
- `tests/stdlib_tests/` - **مكرر**

---

## 📊 تحليل المجلدات

| المجلد | عدد الملفات | الوصف |
|--------|------------|-------|
| async/ | ? | اختبارات async |
| backends/ | ? | اختبارات backends |
| compiler/ | ? | اختبارات المترجم |
| data_tests/ | ? | اختبارات البيانات |
| interpreter_tests/ | ? | اختبارات المفسر |
| jit/ | ? | اختبارات JIT |
| lexer_tests/ | ? | اختبارات Lexer |
| low_level/ | ? | اختبارات منخفض المستوى |
| lsp/ | ? | اختبارات LSP |
| modules/ | ? | اختبارات الوحدات |
| network/ | ? | اختبارات الشبكات |
| oop/ | ? | اختبارات OOP |
| oop_tests/ | ? | **مكرر** |
| optimizer/ | ? | اختبارات المحسّن |
| parser/ | ? | اختبارات Parser |
| parser_tests/ | ? | **مكرر** |
| pkg/ | ? | اختبارات Package |
| spec_rules/ | ? | قواعد المواصفات |
| stage1/ | ? | اختبارات Stage 1 |
| stdlib/ | ? | اختبارات stdlib |
| stdlib_tests/ | ? | **مكرر** |

---

## 📁 الملفات المتفرقة في الجذر

| الملف | الوصف |
|-------|-------|
| benchmark.cpp | قياسات الأداء |
| benchmark_arabic_opt.cpp | قياسات المحسّن العربي |
| comprehensive_test_suite.cpp | مجموعة اختبارات شاملة |
| simple_test.cpp | اختبار بسيط |
| test_arabic_optimizer.cpp | اختبار المحسّن العربي |
| test_lexer_comprehensive.py | اختبار Lexer شامل |
| test_llvm_pipeline.cpp | اختبار LLVM pipeline |
| test_suite.cpp | مجموعة اختبارات |
| toolchain_test.cpp | اختبار toolchain |
| run_comprehensive_tests.ps1 | سكريبت تشغيل |
| temp_test1.s | ملف مؤقت |

---

## 🎯 التوصيات

### 1. دمج المجلدات المكررة

| من | إلى |
|----|-----|
| oop_tests/ | oop/ |
| parser_tests/ | parser/ |
| stdlib_tests/ | stdlib/ |

### 2. تنظيم الملفات المتفرقة

```
tests/
├── benchmarks/          # نقل ملفات benchmark
│   ├── benchmark.cpp
│   └── benchmark_arabic_opt.cpp
│
├── integration/         # نقل ملفات integration
│   ├── comprehensive_test_suite.cpp
│   ├── test_suite.cpp
│   └── toolchain_test.cpp
│
└── scripts/             # نقل السكريبتات
    └── run_comprehensive_tests.ps1
```

---

## ⚠️ قرار التنفيذ

نظراً لمخاطر كسر CMakeLists.txt و CI/CD:
- **لن نقوم بحذف المجلدات المكررة الآن**
- تم توثيق التكرار للمراجعة المستقبلية
- يُنصح بدمج المجلدات يدوياً لاحقاً

---

## ✅ الحالة: تحليل مكتمل

الانتقال للمرحلة 7 (البناء النهائي والتحقق)
