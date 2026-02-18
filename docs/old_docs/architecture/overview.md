<div dir="rtl">

# 🏗️ البنية المعمارية - لغة ص

## نظرة عامة

```
                    الكود بلغة ص (.ص)
                         │
                         ▼
                ┌─────────────────┐
                │  المحلل المعجمي  │  shared/lexer/
                │     (Lexer)     │
                └────────┬────────┘
                         │ رموز (Tokens)
                         ▼
                ┌─────────────────┐
                │  المحلل النحوي   │  shared/parser/
                │    (Parser)     │
                └────────┬────────┘
                         │ شجرة AST
                         ▼
              ┌──────────┴──────────┐
              │                     │
              ▼                     ▼
    ┌─────────────────┐   ┌─────────────────┐
    │    المفسر       │   │    المترجم      │
    │ (Interpreter)   │   │   (Compiler)    │
    │ interpreter_new/│   │  compiler_new/  │
    └────────┬────────┘   └────────┬────────┘
             │                     │
             ▼                     ▼
      تنفيذ مباشر          SIR → LLVM IR → Native
```

## المكونات الرئيسية

### 1. المحلل المعجمي (Lexer) - `shared/lexer/`
- تحويل النص إلى رموز (tokens)
- دعم الكلمات المفتاحية العربية
- UTF-8 كامل

### 2. المحلل النحوي (Parser) - `shared/parser/`
- بناء شجرة AST من الرموز
- دعم OOP، الوحدات، الأنماط العامة
- رسائل خطأ ذكية

### 3. شجرة AST - `shared/ast/`
- تمثيل البرنامج كشجرة
- عُقد: تعبيرات، جمل، تصريحات

### 4. المفسر - `interpreter_new/`
- تنفيذ مباشر لشجرة AST
- إدارة المتغيرات والنطاقات
- 9 أجزاء من الدوال المضمنة

### 5. المترجم - `compiler_new/`
- **Frontend:** AST → SIR (Sad IR)
- **Optimizer:** تحسينات SIR
- **Backend:** SIR → LLVM IR → Native code

### 6. الآلة الافتراضية - `vm/`
- Bytecode VM بديلة
- يمكن تشغيلها بدون LLVM

## نظام البناء

```
CMakeLists.txt          ← الملف الرئيسي (~140 سطر)
├── cmake/llvm.cmake    ← إعدادات LLVM الاختيارية
├── cmake/platform.cmake← خيارات المنصة
├── cmake/sources.cmake ← ملفات المصدر
├── cmake/libraries.cmake← المكتبات (sad_core)
├── cmake/network.cmake ← الشبكات
├── cmake/executables.cmake ← البرامج (sad, sadc)
├── cmake/tools.cmake   ← الأدوات (LSP, fmt, pkg)
├── cmake/tests.cmake   ← الاختبارات
└── cmake/tests_comprehensive.cmake ← الاختبارات الشاملة
```

## المكتبات

| المكتبة | الوصف | الملف |
|---------|-------|-------|
| `sad_core` | النواة (lexer+parser+interpreter+stdlib) | cmake/libraries.cmake |
| `sad_shared` | المكونات المشتركة فقط | shared/CMakeLists.txt |
| `sad_graphics` | SDL2 + OpenGL | graphics/CMakeLists.txt |
| `sad_network` | TCP/UDP sockets | cmake/network.cmake |
| `sad_http` | HTTP client/server | cmake/network.cmake |
| `sad_websocket` | WebSocket | cmake/network.cmake |
| `sad_vm` | الآلة الافتراضية | vm/CMakeLists.txt |
| `sad_mobile` | تطبيقات الهاتف | cmake/executables.cmake |

</div>
