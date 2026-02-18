# بنية مشروع لغة ص - الهندسة المعمارية

> توثيق شامل لبنية المشروع والمكونات الموجودة

## نظرة عامة

مشروع لغة ص هو مشروع متكامل يتضمن مترجم (Compiler) ومفسر (Interpreter) وأدوات تطوير
للغة البرمجة العربية "ص".

## هيكل المشروع

```
s_language/
├── compiler_new/          # المترجم الجديد (C++17 + LLVM 17)
│   ├── CMakeLists.txt     # نظام البناء CMake
│   ├── include/           # الترويسات
│   │   ├── backend/       # LLVM backend headers
│   │   ├── build/         # Build system headers
│   │   ├── bytecode/      # Bytecode VM headers
│   │   ├── comptime/      # Compile-time evaluation
│   │   ├── crypto/        # Cryptography
│   │   ├── ffi/           # Foreign Function Interface
│   │   ├── format/        # Code formatting
│   │   ├── frontend/      # SIR (Sad Intermediate Representation)
│   │   ├── jit/           # Just-In-Time compilation
│   │   ├── kernel/        # OS kernel support
│   │   ├── memory/        # Memory management
│   │   ├── middle/        # Middle-end optimization
│   │   ├── optimizer/     # Optimization passes
│   │   ├── packages/      # Package management
│   │   ├── pipeline/      # Compilation pipeline
│   │   ├── runtime/       # Runtime library
│   │   ├── security/      # Security checking
│   │   ├── semantic/      # Semantic analysis
│   │   ├── testing/       # Testing framework
│   │   └── types/         # Type system
│   ├── src/               # الشيفرة المصدرية
│   │   ├── abstraction/   # Hardware abstraction
│   │   ├── attributes/    # Attribute processing
│   │   ├── backend/       # LLVM codegen (4931+ lines)
│   │   │   ├── llvm/      # LLVM IR generation
│   │   │   └── targets/   # Target architectures
│   │   ├── borrow/        # Borrow checking extensions
│   │   ├── bytecode/      # Bytecode generation
│   │   ├── codegen/       # Code generation guards
│   │   ├── compat/        # Compatibility layers
│   │   ├── comptime/      # Compile-time evaluation
│   │   ├── diagnostics/   # Error diagnostics
│   │   ├── ffi/           # FFI bridge
│   │   ├── format/        # Code formatter
│   │   ├── frontend/      # SIR builder
│   │   ├── incremental/   # Incremental compilation
│   │   ├── jit/           # JIT compilation
│   │   ├── lsp/           # LSP server (28 files)
│   │   ├── macro/         # Macro system
│   │   ├── memory/        # Memory management
│   │   ├── meta/          # Metaprogramming
│   │   ├── middle/        # Middle-end
│   │   ├── optimizer/     # Optimization passes
│   │   ├── pipeline/      # Compilation pipeline
│   │   ├── reflection/    # Reflection system
│   │   ├── security/      # Security analysis
│   │   ├── sema/          # Semantic analysis
│   │   ├── semantic/      # Ownership/borrow analysis
│   │   ├── sir/           # SIR layer
│   │   ├── targets/       # Cross-compilation
│   │   ├── types/         # Type system (36 files)
│   │   └── vm/            # Virtual machine
│   ├── runtime/           # Runtime support library
│   └── tests/             # Compiler-specific tests
│
├── interpreter_new/       # المفسر الجديد
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
│
├── shared/                # المكونات المشتركة
│   ├── ast/               # AST (15 headers, 10 sources)
│   ├── errors/            # نظام الأخطاء ثنائي اللغة
│   ├── lexer/             # المحلل المعجمي (40+ كلمة عربية)
│   ├── modules/           # نظام الوحدات
│   ├── parser/            # المحلل النحوي (27 source files)
│   ├── types/             # أنواع البيانات الأساسية
│   └── utils/             # أدوات مساعدة
│
├── stdlib/                # المكتبة القياسية (38+ وحدة)
│   ├── core/              # الأنواع الأساسية (اختياري، نتيجة)
│   ├── async/             # البرمجة غير المتزامنة
│   ├── crypto/            # التشفير
│   ├── database/          # قواعد البيانات
│   ├── http/              # بروتوكول HTTP
│   ├── io/                # الإدخال/الإخراج
│   ├── math/              # الرياضيات
│   ├── network/           # الشبكات
│   ├── web/               # تطوير الويب
│   └── ...                # وحدات إضافية عديدة
│
├── tools/                 # أدوات التطوير (50+ أداة)
│   ├── lsp/               # خادم LSP
│   ├── docgen/            # توليد التوثيق
│   ├── pkg/               # مدير الحزم
│   ├── linter/            # المُدقق
│   ├── repl/              # بيئة تفاعلية
│   └── ...                # أدوات إضافية
│
├── tests/                 # مجموعة الاختبارات الشاملة
│   ├── compiler/          # اختبارات المترجم
│   ├── integration/       # اختبارات التكامل
│   ├── lexer_tests/       # اختبارات المحلل المعجمي
│   ├── parser_tests/      # اختبارات المحلل النحوي
│   ├── ownership/         # اختبارات نظام الملكية
│   ├── type_system/       # اختبارات نظام الأنواع
│   ├── stdlib/            # اختبارات المكتبة القياسية
│   └── unit/              # اختبارات الوحدة
│
└── sa_ide/                # محرر أفق (Flutter IDE)
    └── lib/               # شيفرة Dart
```

## المكونات الرئيسية

### 1. المحلل المعجمي (Lexer)
- **الموقع**: `shared/lexer/`
- **الحجم**: ~2700 سطر
- يدعم 40+ كلمة مفتاحية عربية
- يدعم الأرقام العربية (٠-٩)
- يدعم Unicode الكامل

### 2. المحلل النحوي (Parser)
- **الموقع**: `shared/parser/`
- **الحجم**: ~15000 سطر (27 ملف مصدري)
- Recursive Descent Parser
- يدعم: OOP, Async, Generics, Lifetime, Macros, UI, Comprehensions

### 3. شجرة البنية المجردة (AST)
- **الموقع**: `shared/ast/`
- **الحجم**: 25 ملف (15 ترويسة + 10 مصدر)
- يدعم نمط الزائر (Visitor Pattern)

### 4. نظام الأنواع
- **مشترك**: `shared/types/` (أنواع بسيطة للمفسر)
- **المترجم**: `compiler_new/src/types/` (36 ملف - generics, traits, constraints)

### 5. التمثيل الوسيط (SIR)
- **الموقع**: `compiler_new/include/frontend/`, `compiler_new/src/frontend/`
- Sad Intermediate Representation
- جسر بين AST و LLVM IR

### 6. الواجهة الخلفية LLVM
- **الموقع**: `compiler_new/src/backend/llvm/` (35 ملف)
- توليد LLVM IR من SIR
- دعم أهداف: x86_64, ARM64, WASM, bare-metal
- مُحسّن مع Arabic optimizer مخصص

### 7. الرابط (Linker)
- **الموقع**: `compiler_new/src/backend/llvm/llvm_linker.cpp` (705 سطر)
- اكتشاف أدوات البناء تلقائياً
- دعم Windows (PE/COFF) و Linux (ELF) و macOS (Mach-O)

### 8. نظام الأخطاء
- **الموقع**: `shared/errors/` (14 ترويسة + 21 مصدر)
- رسائل ثنائية اللغة (عربي + إنجليزي)
- اقتراحات ذكية للإصلاح
- وضع المعلم

### 9. نظام الملكية والاستعارة
- **الموقع**: `compiler_new/src/semantic/`
- تتبع الملكية (ownership_tracker.cpp)
- فحص الاستعارة (borrow_checker.cpp)
- تحليل النقل (move_analyzer.cpp)
- تحليل الأعمار (lifetime_analyzer.cpp)
- NLL (Non-Lexical Lifetimes)

### 10. خادم LSP
- **الموقع**: `compiler_new/src/lsp/` (28 ملف)
- إكمال تلقائي، تعريفات، مراجع، إعادة تسمية
- تشخيصات، تلميحات، إجراءات الكود
- رموز دلالية، توقيعات الدوال

## الحالة الحالية

| المكون | نسبة الاكتمال | ملاحظات |
|--------|--------------|---------|
| المحلل المعجمي | 95% | كامل مع دعم عربي شامل |
| المحلل النحوي | 90% | 27 ملف، دعم واسع |
| AST | 90% | بنية كاملة مع زائر |
| نظام الأنواع | 85% | 36 ملف في المترجم |
| LLVM Backend | 80% | 35 ملف، ~10000 سطر |
| نظام الملكية | 90% | كامل مع NLL |
| LSP | 85% | 28 ملف |
| المكتبة القياسية | 70% | 38+ وحدة |
| أدوات التطوير | 60% | 50+ أداة |
