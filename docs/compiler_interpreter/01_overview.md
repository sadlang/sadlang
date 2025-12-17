# نظرة عامة: المفسر والمترجم
# Overview: Interpreter and Compiler

## 🔍 ما هو المفسر؟ / What is an Interpreter?

### التعريف بالعربية

المفسر (Interpreter) هو برنامج يقوم بقراءة وتنفيذ الكود البرمجي سطراً بسطر أو جملة بجملة بشكل مباشر دون تحويله إلى لغة الآلة مسبقاً. يقوم المفسر بالخطوات التالية:

1. **القراءة**: قراءة الكود المصدري
2. **التحليل**: تحليل معجمي ونحوي (Lexical and Syntax Analysis)
3. **بناء AST**: بناء شجرة التركيب النحوي المجردة
4. **التنفيذ المباشر**: تنفيذ الأوامر فوراً

### Definition in English

An Interpreter is a program that reads and executes code line-by-line or statement-by-statement directly without prior conversion to machine code. The interpreter performs these steps:

1. **Reading**: Reading source code
2. **Parsing**: Lexical and syntax analysis
3. **AST Building**: Building Abstract Syntax Tree
4. **Direct Execution**: Executing commands immediately

---

## 🔧 ما هو المترجم؟ / What is a Compiler?

### التعريف بالعربية

المترجم (Compiler) هو برنامج يقوم بتحويل الكود المصدري بالكامل إلى كود آلة قابل للتنفيذ (Binary/Executable) قبل التشغيل. يمر المترجم بالمراحل التالية:

1. **التحليل الأمامي** (Frontend):
   - التحليل المعجمي (Lexical Analysis)
   - التحليل النحوي (Syntax Analysis)
   - التحليل الدلالي (Semantic Analysis)
   
2. **التمثيل الوسيط** (Middle-end):
   - بناء IR (Intermediate Representation)
   - التحسينات المستقلة عن المنصة
   
3. **التحليل الخلفي** (Backend):
   - توليد الكود الآلي
   - التحسينات الخاصة بالمنصة
   - إنتاج الملف التنفيذي

### Definition in English

A Compiler is a program that converts the entire source code into executable machine code (Binary/Executable) before execution. The compiler goes through these phases:

1. **Frontend**:
   - Lexical Analysis
   - Syntax Analysis
   - Semantic Analysis
   
2. **Middle-end**:
   - Building IR (Intermediate Representation)
   - Platform-independent optimizations
   
3. **Backend**:
   - Machine code generation
   - Platform-specific optimizations
   - Producing executable file

---

## ⚖️ الفرق الأساسي / Core Difference

### بالعربية

| الخاصية | المفسر | المترجم |
|---------|--------|---------|
| **التنفيذ** | مباشر سطر بسطر | ترجمة كاملة ثم تنفيذ |
| **المخرجات** | لا يُنتج ملف تنفيذي | يُنتج ملف .exe أو binary |
| **الأخطاء** | تظهر عند التنفيذ | تظهر عند الترجمة |
| **السرعة** | أبطأ في التنفيذ | أسرع بكثير |
| **الذاكرة** | يحتاج runtime في الذاكرة | الملف التنفيذي مستقل |
| **التوزيع** | يحتاج المفسر مع الكود | ملف واحد قابل للتشغيل |

### In English

| Property | Interpreter | Compiler |
|----------|------------|----------|
| **Execution** | Direct line-by-line | Full translation then execution |
| **Output** | No executable file | Produces .exe or binary |
| **Errors** | Appear at runtime | Appear at compile time |
| **Speed** | Slower execution | Much faster |
| **Memory** | Needs runtime in memory | Executable is standalone |
| **Distribution** | Needs interpreter with code | Single executable file |

---

## 🏗️ الحالة الحالية للغة ص / Current State of Sad Language

### الوضع الحالي / Current Status

```
┌─────────────────────────────────────┐
│   لغة ص - Sad Language (v1.0)      │
│                                     │
│   ✅ المفسر مكتمل                   │
│      Interpreter Complete          │
│                                     │
│   📝 الكود المصدري (.s)            │
│      Source Code (.s)              │
│          ↓                         │
│   🔤 المحلل المعجمي (Lexer)        │
│          ↓                         │
│   🌳 المحلل النحوي (Parser)        │
│          ↓                         │
│   🎭 بناء AST                      │
│          ↓                         │
│   ⚡ تنفيذ مباشر (Interpreter)     │
│          ↓                         │
│   📤 النتائج (Output)              │
│                                     │
│   ❌ المترجم قيد التطوير            │
│      Compiler In Development       │
└─────────────────────────────────────┘
```

### المفسر الحالي / Current Interpreter

**المكونات المُنجَزة / Completed Components:**

1. ✅ **Lexer** - محلل معجمي كامل يدعم العربية
2. ✅ **Parser** - محلل نحوي شامل مع استرداد الأخطاء
3. ✅ **AST** - شجرة التركيب النحوي المجردة
4. ✅ **Type System** - نظام الأنواع (نص، عدد، منطقي، قائمة، قاموس)
5. ✅ **OOP Support** - دعم البرمجة الكائنية (صنف، وراثة)
6. ✅ **Builtin Functions** - 50+ دالة مدمجة
7. ✅ **Filesystem I/O** - عمليات الملفات
8. ✅ **Error Handling** - معالجة الأخطاء مع رسائل ثنائية اللغة

**الأداء الحالي / Current Performance:**

- وقت البدء: **~50-100ms** ⚡
- سرعة التنفيذ: **متوسطة** (أبطأ 10-50x من C++)
- استهلاك الذاكرة: **30-100 MB** للبرامج البسيطة

---

## 🎯 المترجم المقترح / Proposed Compiler

### الأهداف / Objectives

**الهدف الرئيسي / Main Goal:**  
تحويل كود لغة ص (.s) إلى ملفات تنفيذية أصلية (native executables) لتحسين الأداء والنشر.

**الأهداف الفرعية / Sub-goals:**

1. ⚡ **تحسين السرعة**: 20-100x أسرع من المفسر
2. 💾 **تقليل الذاكرة**: استهلاك أقل بنسبة 50-70%
3. 📦 **نشر سهل**: ملف واحد قابل للتشغيل
4. 🔒 **حماية الكود**: الكود المصدري غير مرئي
5. 🎯 **توافق**: دعم Windows, Linux, macOS

### الناتج المتوقع / Expected Output

```
مثال.s  →  [المترجم]  →  مثال.exe  (Windows)
                        →  مثال     (Linux/macOS)
```

**خصائص الملف التنفيذي / Executable Properties:**

- الحجم: **~500KB - 2MB** (بدون مكتبات خارجية)
- الاعتمادات: **مستقل تماماً** أو يحتاج runtime صغير
- السرعة: **قريبة من سرعة C++**
- التوافق: **ABI متوافق** مع المكتبات الأصلية

---

## 🔄 التشابهات / Similarities

على الرغم من الاختلافات، يشترك المفسر والمترجم في:

Despite differences, both interpreter and compiler share:

### المراحل المشتركة / Common Phases

1. **التحليل المعجمي** / Lexical Analysis
   - نفس Lexer يمكن استخدامه
   - نفس Tokens
   
2. **التحليل النحوي** / Syntax Analysis  
   - نفس Parser
   - نفس قواعد النحو (Grammar)
   
3. **بناء AST** / AST Construction
   - نفس هيكل الشجرة
   - نفس العقد (Nodes)
   
4. **التحليل الدلالي** / Semantic Analysis
   - التحقق من الأنواع
   - تحليل النطاق (Scope)

### الاختلاف يبدأ بعد AST / Difference Starts After AST

```
[Source Code] 
    ↓
[Lexer] ← مشترك / Shared
    ↓
[Parser] ← مشترك / Shared  
    ↓
[AST] ← مشترك / Shared
    ↓
    ├─→ [Interpreter] → تنفيذ مباشر / Direct Execution
    │
    └─→ [Compiler] → [IR] → [Code Gen] → [Binary]
```

---

## 📈 التطور التاريخي / Historical Evolution

### مراحل تطوير لغة ص / Sad Language Development Phases

```
Phase 1 (✅ مكتمل / Complete): المفسر الأساسي
├─ Basic Lexer & Parser
├─ Core Interpreter
└─ Essential Builtins

Phase 2 (✅ مكتمل / Complete): البرمجة الكائنية
├─ Class System
├─ Inheritance
└─ Method Calls

Phase 3 (✅ مكتمل / Complete): المكتبات المدمجة
├─ Filesystem I/O
├─ String Operations
└─ Collection Utilities

Phase 4 (🔄 حالي / Current): تحسينات المفسر
├─ Error Messages Enhancement
├─ Performance Optimization
└─ Standard Library

Phase 5 (📋 مخطط / Planned): المترجم
├─ IR Design
├─ LLVM Backend
└─ Native Code Generation
```

---

## 🎓 الخلاصة / Summary

- **المفسر**: مناسب للتطوير السريع والتعلم والتصحيح
- **المترجم**: مناسب للإنتاج والأداء العالي والنشر

- **Interpreter**: Suitable for rapid development, learning, and debugging
- **Compiler**: Suitable for production, high performance, and deployment

**الوضع المثالي للغة ص**:  
دعم كلا النمطين مع إمكانية الاختيار حسب الحاجة.

**Ideal State for Sad Language**:  
Support both modes with the ability to choose based on needs.

```bash
# التطوير / Development
sad run program.s          # مفسر سريع / Fast interpreter

# الإنتاج / Production  
sad compile program.s      # مترجم محسّن / Optimized compiler
./program                  # تشغيل مباشر / Direct execution
```

---

**التالي**: [البنية المعمارية / Architecture →](02_architecture.md)
