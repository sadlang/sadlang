# ملخص خطة إعادة التنظيم

## 📋 نظرة عامة

هذه خطة شاملة لإعادة تنظيم ملفات مشروع "لغة ص" لتحسين الهيكل وتسهيل الصيانة.

---

## 📁 الهيكل الحالي vs الهيكل الجديد

### الهيكل الحالي (مبعثر)

```
s_language/
├── include/           # ~80 header files
├── src/               # ~70 source files
├── compiler/          # ~120 files
├── vm/                # ~10 files
├── runtime/           # ~15 files
├── bytecode/          # ~5 files
├── graphics/          # ~50 files
└── ...
```

### الهيكل الجديد (منظم)

```
s_language/
├── shared/            # المكونات المشتركة (~68 files)
│   ├── lexer/
│   ├── parser/
│   ├── ast/
│   ├── types/
│   ├── errors/
│   ├── modules/
│   └── utils/
│
├── interpreter/       # المفسر (~20 files)
│   ├── core/
│   ├── visitors/
│   ├── managers/
│   └── builtins/
│
├── compiler/          # المترجم (~102 files)
│   ├── frontend/      # SIR Builder
│   ├── middle/        # Optimizer
│   ├── backend/       # LLVM Codegen
│   ├── jit/           # JIT Compiler
│   ├── bytecode/
│   └── types/
│
├── runtime/           # وقت التشغيل (~24 files)
│   ├── vm/
│   ├── memory/
│   ├── thread/
│   ├── ffi/
│   └── exception/
│
├── tools/             # الأدوات
│   ├── repl/
│   ├── lsp/
│   ├── pkg/
│   └── docgen/
│
├── graphics/          # الرسوميات (بدون تغيير)
├── stdlib/            # المكتبة القياسية (بدون تغيير)
└── tests/             # الاختبارات
```

---

## 📊 ملخص الملفات

| المكون | Headers | Sources | المجموع |
|--------|---------|---------|---------|
| shared | ~40 | ~28 | ~68 |
| interpreter | ~10 | ~10 | ~20 |
| compiler | ~45 | ~57 | ~102 |
| runtime | ~13 | ~11 | ~24 |
| tools | ~8 | ~8 | ~16 |
| **المجموع** | **~116** | **~114** | **~230** |

---

## 📅 جدول المراحل

| المرحلة | الوصف | الوقت المقدر |
|---------|-------|-------------|
| 1 | إنشاء shared/ | 2-3 ساعات |
| 2 | إنشاء interpreter/ | 1-2 ساعة |
| 3 | إنشاء compiler/ | 3-4 ساعات |
| 4 | إنشاء runtime/ | 1-2 ساعة |
| 5 | تحديث CMake | 1-2 ساعة |
| 6 | الاختبار | 1-2 ساعة |
| 7 | إنشاء tools/ | 0.5-1 ساعة |
| **المجموع** | | **9.5-16 ساعة** |

---

## 🔗 مخطط التبعيات النهائي

```
                         +-------------+
                         |     sad     |
                         | (executable)|
                         +------+------+
                                |
          +---------------------+----------------------+
          |                     |                      |
          v                     v                      v
   +------+------+      +-------+-------+      +-------+-------+
   | sad_shared  |<-----| sad_interpreter|     | sad_compiler  |
   | (core lib)  |      |    (opt lib)  |     |   (opt lib)   |
   +------+------+      +-------+-------+     +-------+-------+
          ^                     |                      |
          |                     v                      v
          |             +-------+-------+      +-------+-------+
          +-------------|  sad_runtime  |      |     LLVM      |
                        +---------------+      +---------------+
```

---

## ✅ قائمة الملفات

| الملف | الوصف |
|-------|-------|
| [00_overview.md](00_overview.md) | نظرة عامة على الخطة |
| [01_shared_phase.md](01_shared_phase.md) | المرحلة 1: shared/ |
| [02_interpreter_phase.md](02_interpreter_phase.md) | المرحلة 2: interpreter/ |
| [03_compiler_phase.md](03_compiler_phase.md) | المرحلة 3: compiler/ |
| [04_runtime_phase.md](04_runtime_phase.md) | المرحلة 4: runtime/ |
| [05_cmake_update.md](05_cmake_update.md) | المرحلة 5: تحديث CMake |
| [06_testing.md](06_testing.md) | المرحلة 6: الاختبار |
| [07_tools_phase.md](07_tools_phase.md) | المرحلة 7: tools/ |

---

## ⚠️ ملاحظات مهمة

1. **النسخ الاحتياطي**: أنشئ نسخة احتياطية قبل البدء
2. **التنفيذ التدريجي**: نفذ مرحلة واحدة في كل مرة
3. **الاختبار المستمر**: اختبر بعد كل مرحلة
4. **Git Commits**: أنشئ commit بعد كل مرحلة ناجحة

---

## 🚀 البدء

للبدء في التنفيذ:

1. اقرأ [00_overview.md](00_overview.md) للفهم الشامل
2. أنشئ نسخة احتياطية
3. ابدأ بـ [01_shared_phase.md](01_shared_phase.md)
4. تابع بالترتيب

---

## 📞 المساعدة

إذا واجهت مشاكل:
1. راجع قسم "إصلاح المشاكل" في [06_testing.md](06_testing.md)
2. تحقق من رسائل الخطأ في CMake
3. استخدم النسخة الاحتياطية للاستعادة إذا لزم الأمر
