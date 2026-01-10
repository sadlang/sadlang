# خطة إعادة تنظيم مشروع لغة ص البرمجية

## نظرة عامة

### تاريخ الإعداد: 8 يناير 2026

---

## 📋 ملخص تنفيذي

هذه الخطة تهدف إلى إعادة تنظيم هيكل ملفات مشروع لغة ص البرمجية لتحقيق:
- **وضوح أفضل** في بنية المشروع
- **تقليل التكرار** في الملفات والمجلدات
- **تسهيل الصيانة** والتطوير المستقبلي
- **توحيد المسارات** لملفات الرأس والمصدر

---

## 🔍 تحليل الوضع الحالي

### المشاكل المكتشفة:

#### 1. تكرار الهياكل (Duplication)
| المشكلة | الموقع |
|---------|--------|
| مجلد `include/` موجود في عدة أماكن | الجذر، compiler، runtime، vm، bytecode، graphics |
| مجلد `src/` موجود في عدة أماكن | الجذر، compiler، runtime، vm، bytecode، graphics |
| ملفات LLVM متكررة | `compiler/backend/llvm`، `compiler/backends/llvm`، `compiler/pipeline/llvm` |
| ملفات الشبكة متكررة | `include/network`، `stdlib/network` |

#### 2. عدم الاتساق في الهيكل
| المشكلة | التفاصيل |
|---------|----------|
| بعض المكونات تستخدم include/src | مثل compiler/frontend |
| بعضها يضع الملفات مباشرة | مثل compiler/backends/llvm |
| خلط بين .h و .cpp في نفس المجلد | في stdlib |

#### 3. تداخل المسؤوليات
- `src/` في الجذر يحتوي على محلل ومفسر
- `compiler/frontend/` يحتوي على SIR builder
- `include/interpreter/` يحتوي على ملفات AST printer

---

## 📁 الهيكل الجديد المقترح

```
s_language/
├── 📂 src/                      # الكود المصدري الرئيسي (موحد)
│   ├── core/                    # النواة الأساسية
│   │   ├── lexer/              # المحلل اللغوي
│   │   ├── parser/             # المحلل النحوي
│   │   ├── ast/                # شجرة البناء المجرد
│   │   └── types/              # أنواع البيانات الأساسية
│   │
│   ├── compiler/               # المترجم
│   │   ├── frontend/           # الواجهة الأمامية (SIR)
│   │   ├── optimizer/          # المحسّن
│   │   ├── type_system/        # نظام الأنواع
│   │   ├── bytecode/           # توليد bytecode
│   │   └── llvm/               # توليد LLVM IR (موحد)
│   │
│   ├── runtime/                # بيئة التشغيل
│   │   ├── vm/                 # الآلة الافتراضية
│   │   ├── jit/                # التجميع الفوري
│   │   ├── memory/             # إدارة الذاكرة وGC
│   │   ├── ffi/                # واجهة الدوال الخارجية
│   │   └── thread/             # الخيوط والتزامن
│   │
│   ├── interpreter/            # المفسر
│   │   ├── evaluator/          # تقييم التعبيرات
│   │   ├── executor/           # تنفيذ العبارات
│   │   └── visitors/           # نمط الزائر
│   │
│   └── main.cpp                # نقطة الدخول
│
├── 📂 include/                  # ملفات الرأس (موحدة)
│   ├── core/
│   ├── compiler/
│   ├── runtime/
│   └── interpreter/
│
├── 📂 stdlib/                   # المكتبة القياسية
│   ├── core/                   # الدوال الأساسية
│   ├── io/                     # الإدخال/الإخراج
│   ├── math/                   # الرياضيات
│   ├── string/                 # النصوص
│   ├── collections/            # المجموعات (arrays, maps)
│   ├── filesystem/             # نظام الملفات
│   ├── network/                # الشبكات
│   │   ├── socket/             # TCP/UDP
│   │   ├── http/               # HTTP client/server
│   │   └── websocket/          # WebSocket
│   ├── crypto/                 # التشفير
│   ├── json/                   # JSON
│   ├── xml/                    # XML
│   ├── database/               # قواعد البيانات
│   └── graphics/               # الرسومات
│
├── 📂 tools/                    # الأدوات المساعدة
│   ├── sad/                    # CLI الرئيسي
│   ├── sad-lsp/                # خادم LSP
│   ├── sad-pkg/                # مدير الحزم
│   ├── sad-repl/               # البيئة التفاعلية
│   ├── sad-doc/                # مولد التوثيق
│   └── vscode-extension/       # إضافة VS Code
│
├── 📂 tests/                    # الاختبارات
│   ├── unit/                   # اختبارات الوحدات
│   │   ├── core/
│   │   ├── compiler/
│   │   ├── runtime/
│   │   └── stdlib/
│   ├── integration/            # اختبارات التكامل
│   ├── e2e/                    # اختبارات شاملة
│   └── benchmarks/             # قياسات الأداء
│
├── 📂 docs/                     # التوثيق
│   ├── api/                    # توثيق API
│   ├── tutorials/              # الدروس
│   └── design/                 # وثائق التصميم
│
├── 📂 examples/                 # الأمثلة
│
├── 📂 third_party/              # المكتبات الخارجية
│
├── CMakeLists.txt              # ملف البناء الرئيسي
├── README.md                   # التوثيق الرئيسي
└── LICENSE                     # الترخيص
```

---

## 📅 مراحل التنفيذ

| المرحلة | الوصف | الملف |
|---------|-------|-------|
| **المرحلة 1** | توحيد ملفات LLVM | `01_phase1_llvm_consolidation.md` |
| **المرحلة 2** | إعادة تنظيم compiler | `02_phase2_compiler_reorganization.md` |
| **المرحلة 3** | توحيد include و src | `03_phase3_include_src_unification.md` |
| **المرحلة 4** | تنظيم runtime و vm | `04_phase4_runtime_vm_merge.md` |
| **المرحلة 5** | تنظيم stdlib | `05_phase5_stdlib_cleanup.md` |
| **المرحلة 6** | تنظيم الاختبارات | `06_phase6_tests_organization.md` |
| **المرحلة 7** | تحديث CMakeLists | `07_phase7_cmake_update.md` |

---

## ⚠️ ملاحظات هامة

1. **النسخ الاحتياطي**: يجب عمل نسخة احتياطية كاملة قبل أي تغيير
2. **التدرج**: تنفيذ كل مرحلة وبناء المشروع للتأكد من عدم وجود أخطاء
3. **Git**: استخدام commits منفصلة لكل مرحلة
4. **الاختبارات**: تشغيل الاختبارات بعد كل مرحلة

---

## 📊 الجدول الزمني المقترح

| المرحلة | الوقت المقدر |
|---------|-------------|
| المرحلة 1 | 2-3 ساعات |
| المرحلة 2 | 3-4 ساعات |
| المرحلة 3 | 4-5 ساعات |
| المرحلة 4 | 2-3 ساعات |
| المرحلة 5 | 2-3 ساعات |
| المرحلة 6 | 2-3 ساعات |
| المرحلة 7 | 3-4 ساعات |
| **المجموع** | **18-25 ساعة** |

---

## 🔗 الملفات التالية

- [المرحلة 1: توحيد LLVM](01_phase1_llvm_consolidation.md)
- [المرحلة 2: تنظيم compiler](02_phase2_compiler_reorganization.md)
- [المرحلة 3: توحيد include/src](03_phase3_include_src_unification.md)
- [المرحلة 4: دمج runtime و vm](04_phase4_runtime_vm_merge.md)
- [المرحلة 5: تنظيف stdlib](05_phase5_stdlib_cleanup.md)
- [المرحلة 6: تنظيم الاختبارات](06_phase6_tests_organization.md)
- [المرحلة 7: تحديث CMake](07_phase7_cmake_update.md)
