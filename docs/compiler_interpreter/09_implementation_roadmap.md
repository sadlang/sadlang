# خارطة طريق تطوير المترجم
# Compiler Implementation Roadmap

## 🗺️ نظرة عامة / Overview

هذه الوثيقة تقدم خطة مفصلة لتطوير مترجم لغة ص، مبنية على البنية الموجودة (SIR System).

This document provides a detailed plan for developing the Sad compiler, building on the existing infrastructure (SIR System).

---

## 📊 الحالة الحالية / Current Status

```
┌──────────────────────────────────────────────────────┐
│  Sad Language Project Status - December 2025        │
├──────────────────────────────────────────────────────┤
│                                                      │
│  ✅ Phase 1: Interpreter (100% Complete)            │
│     ├─ Lexer ✅                                      │
│     ├─ Parser ✅                                     │
│     ├─ AST ✅                                        │
│     ├─ Interpreter ✅                                │
│     ├─ OOP Support ✅                                │
│     └─ 60+ Builtin Functions ✅                      │
│                                                      │
│  🔄 Phase 2: Compiler Frontend (95% Complete)       │
│     ├─ SIR Module System ✅                          │
│     ├─ SIR Function System ✅                        │
│     ├─ SIR Basic Blocks ✅                           │
│     ├─ SIR Instructions (40+ opcodes) ✅             │
│     └─ AST → SIR Builder ⚠️ (needs testing)         │
│                                                      │
│  📋 Phase 3: Optimizer (0% - Planned)                │
│  📋 Phase 4: Backend (0% - Planned)                  │
│  📋 Phase 5: Linker (0% - Planned)                   │
│                                                      │
└──────────────────────────────────────────────────────┘
```

**الملفات الموجودة / Existing Files:**

```
compiler/frontend/
├─ include/sir/
│  ├─ sir_module.h        ✅ (Interface design complete)
│  ├─ sir_function.h      ✅ (Complete)
│  ├─ sir_basic_block.h   ✅ (Complete)
│  ├─ sir_instruction.h   ✅ (40+ instruction types)
│  └─ sir_builder.h       ⚠️ (Needs implementation)
└─ src/sir/
   ├─ sir_module.cpp      ✅ (Basic implementation)
   ├─ sir_function.cpp    ✅ (Complete)
   ├─ sir_basic_block.cpp ✅ (Complete)
   ├─ sir_instruction.cpp ✅ (Complete)
   └─ sir_builder.cpp     ⚠️ (Needs work)
```

---

## 🎯 خارطة الطريق / Roadmap

### Phase 3: تكملة SIR Builder (2-3 أسابيع)

**الأهداف / Objectives:**
- إكمال تطبيق AST → SIR converter
- اختبار شامل للتحويل
- معالجة الأخطاء

**المهام / Tasks:**

#### Week 1: Core Statement Conversion

```cpp
// sir_builder.cpp - تحويل الجمل الأساسية

class SIRBuilder {
public:
    // إكمال هذه الدوال
    void visitVariableDecl(VariableDecl* decl);
    void visitIfStmt(IfStmt* stmt);
    void visitWhileStmt(WhileStmt* stmt);
    void visitForStmt(ForStmt* stmt);
    void visitReturnStmt(ReturnStmt* stmt);
    void visitFunctionDecl(FunctionDecl* decl);
};
```

**مثال التحويل / Conversion Example:**

```sad
# Sad source code
دالة فيبوناتشي(ن: رقم) -> رقم {
    إذا(ن <= 1) {
        أرجع ن؛
    }
    أرجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)؛
}
```

```
; SIR output
function @fibonacci(i32 %n) -> i32 {
entry:
    %cmp = icmp_le i32 %n, 1
    br i1 %cmp, label %then, label %else

then:
    ret i32 %n

else:
    %n_1 = sub i32 %n, 1
    %call1 = call i32 @fibonacci(i32 %n_1)
    %n_2 = sub i32 %n, 2
    %call2 = call i32 @fibonacci(i32 %n_2)
    %result = add i32 %call1, %call2
    ret i32 %result
}
```

#### Week 2: Expression Conversion

```cpp
// تحويل التعابير
SIRValue* visitBinaryExpr(BinaryExpr* expr);
SIRValue* visitUnaryExpr(UnaryExpr* expr);
SIRValue* visitCallExpr(CallExpr* expr);
SIRValue* visitMemberExpr(MemberExpr* expr);
SIRValue* visitIndexExpr(IndexExpr* expr);
```

#### Week 3: OOP Support

```cpp
// دعم البرمجة الكائنية
void visitClassDecl(ClassDecl* decl);
SIRValue* visitNewExpr(NewExpr* expr);
SIRValue* visitThisExpr(ThisExpr* expr);

// تحويل الأصناف إلى structs
struct SIRClass {
    std::vector<SIRField> fields;
    std::vector<SIRMethod> methods;
    SIRClass* parent;  // للوراثة
};
```

---

### Phase 4: Optimizer (3-4 أسابيع)

**الأهداف / Objectives:**
- تنفيذ تحسينات أساسية
- تحسينات متقدمة اختيارية
- قياس الأداء

**التحسينات المخططة / Planned Optimizations:**

#### Level 1: Basic Optimizations (أسبوع 1)

```cpp
namespace sad::optimizer {

// 1. Constant Folding
// قبل: x = 2 + 3;
// بعد: x = 5;
class ConstantFolder {
    SIRValue* fold(BinaryOp* op);
};

// 2. Dead Code Elimination
// إزالة الكود غير المستخدم
class DeadCodeEliminator {
    void eliminateUnreachableBlocks();
    void eliminateUnusedValues();
};

// 3. Common Subexpression Elimination
// قبل: a = x + y; b = x + y;
// بعد: temp = x + y; a = temp; b = temp;
class CSE {
    SIRValue* findCommonExpr(BinaryOp* op);
};

} // namespace sad::optimizer
```

#### Level 2: Control Flow Optimizations (أسبوع 2)

```cpp
// 4. Branch Optimization
// إزالة الـ branches الثابتة
if (true) { ... }  → { ... }
if (false) { ... } → // removed

// 5. Loop Optimization
// - Loop unrolling
// - Loop invariant code motion
```

#### Level 3: Function Optimizations (أسبوع 3)

```cpp
// 6. Inline Expansion
// استبدال استدعاءات الدوال الصغيرة بالكود مباشرة
دالة صغيرة(س) { أرجع س * 2؛ }
ص = صغيرة(5)؛
// ↓
ص = 5 * 2؛

// 7. Tail Call Optimization
// تحويل الـ tail recursion إلى loop
```

#### Level 4: Advanced (أسبوع 4 - اختياري)

```cpp
// 8. Register Allocation
// - Graph coloring algorithm
// - Live range analysis

// 9. Instruction Scheduling
// ترتيب التعليمات لتحسين pipeline

// 10. Vectorization
// استخدام SIMD instructions
```

**مثال التحسين / Optimization Example:**

```sad
# Original code
لكل(ع من مدى(1000)) {
    نتيجة = نتيجة + (5 * 2) + ع؛
}
```

```
; Before optimization
loop:
    %mul = mul i32 5, 2         ; يتكرر 1000 مرة!
    %add1 = add i32 %result, %mul
    %add2 = add i32 %add1, %i
    store i32 %add2, %result
    br label %loop

; After optimization
    %const = mul i32 5, 2       ; مرة واحدة قبل الـ loop
loop:
    %add = add i32 %result, %const
    %add2 = add i32 %add, %i
    store i32 %add2, %result
    br label %loop
```

---

### Phase 5: Code Generator - Backend (4-6 أسابيع)

**الخيارات / Options:**

#### Option A: LLVM Backend (مُوصى به)

**المميزات:**
- ✅ تحسينات عالمية المستوى
- ✅ دعم جميع المنصات
- ✅ JIT compilation ممكن
- ✅ debugging symbols (DWARF)

**العيوب:**
- ⚠️ اعتماد كبير على LLVM (2+ GB)
- ⚠️ API معقد
- ⚠️ وقت ترجمة أطول

**التنفيذ / Implementation:**

```cpp
// src/backend/llvm/llvm_codegen.cpp
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace sad::backend::llvm {

class LLVMCodeGenerator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module* module;
    
public:
    void generateModule(SIRModule* sirModule) {
        module = new llvm::Module("sad_module", context);
        
        // تحويل كل دالة SIR إلى LLVM IR
        for (auto& func : sirModule->functions()) {
            generateFunction(func);
        }
    }
    
    void generateFunction(SIRFunction* sirFunc) {
        // إنشاء LLVM function
        llvm::Function* llvmFunc = createLLVMFunction(sirFunc);
        
        // تحويل كل basic block
        for (auto& block : sirFunc->blocks()) {
            generateBasicBlock(block, llvmFunc);
        }
    }
    
    void generateBasicBlock(SIRBasicBlock* sirBlock, 
                           llvm::Function* func) {
        llvm::BasicBlock* bb = 
            llvm::BasicBlock::Create(context, 
                                    sirBlock->name(), 
                                    func);
        builder.SetInsertPoint(bb);
        
        // تحويل كل instruction
        for (auto& inst : sirBlock->instructions()) {
            generateInstruction(inst);
        }
    }
    
    void generateInstruction(SIRInstruction* sirInst) {
        switch (sirInst->opcode()) {
            case SIR_ADD:
                builder.CreateAdd(/* operands */);
                break;
            case SIR_CALL:
                builder.CreateCall(/* function, args */);
                break;
            // ... 40+ instructions
        }
    }
    
    void emitObjectFile(const std::string& filename) {
        // كتابة .obj/.o file
        llvm::legacy::PassManager pm;
        // ... LLVM machinery
    }
};

} // namespace sad::backend::llvm
```

**الاستخدام / Usage:**

```bash
# ترجمة باستخدام LLVM
$ sad compile --backend=llvm program.s -o program.exe

# عملية الترجمة:
# 1. program.s → AST
# 2. AST → SIR
# 3. SIR → LLVM IR
# 4. LLVM IR → optimizations
# 5. LLVM IR → machine code
# 6. machine code → program.exe
```

#### Option B: Custom Backend (أكثر تحكم)

**المميزات:**
- ✅ لا اعتماد على LLVM
- ✅ حجم أصغر
- ✅ ترجمة أسرع
- ✅ تحكم كامل

**العيوب:**
- ⚠️ تطوير أطول (شهور)
- ⚠️ تحسينات أقل
- ⚠️ يحتاج خبرة عميقة

**التنفيذ / Implementation:**

```cpp
// src/backend/x86/x86_codegen.cpp
namespace sad::backend::x86 {

class X86CodeGenerator {
private:
    std::vector<uint8_t> machineCode;
    
public:
    void generateFunction(SIRFunction* func) {
        // Function prologue
        emitPrologue();
        
        // Generate instructions
        for (auto& block : func->blocks()) {
            for (auto& inst : block->instructions()) {
                emitInstruction(inst);
            }
        }
        
        // Function epilogue
        emitEpilogue();
    }
    
    void emitInstruction(SIRInstruction* inst) {
        switch (inst->opcode()) {
            case SIR_ADD:
                // x86: add rax, rbx
                emit_byte(0x48);  // REX.W prefix
                emit_byte(0x01);  // add opcode
                emit_byte(0xD8);  // ModR/M (rax, rbx)
                break;
                
            case SIR_MUL:
                // x86: imul rax, rbx
                emit_byte(0x48);
                emit_byte(0x0F);
                emit_byte(0xAF);
                emit_byte(0xC3);
                break;
                
            // ... كل التعليمات
        }
    }
    
    void emitObjectFile(const std::string& filename) {
        // كتابة PE (Windows) أو ELF (Linux) format
        // تحتاج تنفيذ PE/ELF writer
    }
};

} // namespace sad::backend::x86
```

---

### Phase 6: Linker (2-3 أسابيع)

**الأهداف / Objectives:**
- ربط ملفات الـ object
- resolve symbols
- إنشاء executable

**الخيارات / Options:**

#### Option A: استخدام System Linker

```cpp
// src/linker/system_linker.cpp
void linkExecutable(const std::vector<std::string>& objFiles,
                    const std::string& output) {
    std::string cmd;
    
    #ifdef _WIN32
        // استخدام link.exe من Visual Studio
        cmd = "link.exe /OUT:" + output;
        for (const auto& obj : objFiles) {
            cmd += " " + obj;
        }
        cmd += " /SUBSYSTEM:CONSOLE";
    #else
        // استخدام ld على Linux/macOS
        cmd = "ld -o " + output;
        for (const auto& obj : objFiles) {
            cmd += " " + obj;
        }
    #endif
    
    system(cmd.c_str());
}
```

#### Option B: Custom Linker (متقدم)

```cpp
// تنفيذ linker كامل
class SadLinker {
    void resolveSymbols();
    void relocate();
    void writeExecutable();
};
```

---

## 📅 الجدول الزمني / Timeline

```
┌─────────────────────────────────────────────────────┐
│  Compiler Development Timeline                      │
├─────────────────────────────────────────────────────┤
│                                                     │
│  Month 1: Complete SIR Builder                      │
│  ├─ Week 1: Core statements ✅                      │
│  ├─ Week 2: Expressions ✅                          │
│  ├─ Week 3: OOP support ✅                          │
│  └─ Week 4: Testing & bug fixes 🔧                  │
│                                                     │
│  Month 2: Optimizer                                 │
│  ├─ Week 1: Basic optimizations                    │
│  ├─ Week 2: Control flow                           │
│  ├─ Week 3: Function optimizations                 │
│  └─ Week 4: Testing & benchmarking                 │
│                                                     │
│  Month 3-4: Backend (LLVM)                          │
│  ├─ Week 1-2: LLVM integration                     │
│  ├─ Week 3-4: SIR → LLVM IR                        │
│  ├─ Week 5-6: Testing & optimization flags         │
│  └─ Week 7-8: Debugging support (DWARF)            │
│                                                     │
│  Month 5: Linker & Integration                      │
│  ├─ Week 1-2: System linker integration            │
│  ├─ Week 3: Full pipeline testing                  │
│  └─ Week 4: Documentation & examples               │
│                                                     │
│  Month 6: Polish & Release                          │
│  ├─ Week 1-2: Performance tuning                   │
│  ├─ Week 3: User testing & feedback                │
│  └─ Week 4: Release v1.0 🎉                        │
│                                                     │
└─────────────────────────────────────────────────────┘

Total Time: 5-6 months
Team Size: 2-3 developers recommended
```

---

## 🎯 Milestones

### Milestone 1: "Hello Compiled World" (Month 1)

```bash
$ cat hello.s
اطبع("مرحباً من المترجم!")؛

$ sad compile hello.s -o hello.exe
✓ Compiled successfully

$ ./hello.exe
مرحباً من المترجم!
```

### Milestone 2: "Fibonacci Benchmark" (Month 2)

```bash
$ sad compile --optimize fibonacci.s
$ time ./fibonacci
Fibonacci(30) = 832040
Time: 0.18s  # vs 8.5s interpreter = 47x faster!
```

### Milestone 3: "Full OOP Support" (Month 3)

```bash
$ sad compile --llvm oop_program.s
✓ Classes compiled
✓ Inheritance working
✓ Virtual methods resolved
```

### Milestone 4: "Production Ready" (Month 5)

```bash
$ sad compile -O2 --static large_app.s -o app.exe
✓ Optimized build
✓ Size: 1.2 MB
✓ No dependencies
✓ Ready for distribution
```

---

## 💡 توصيات / Recommendations

### للبدء الآن / To Start Now

1. **أكمل SIR Builder**
   ```bash
   cd compiler/frontend/src/sir
   # افتح sir_builder.cpp
   # ابدأ بتنفيذ visitVariableDecl()
   ```

2. **اكتب الاختبارات**
   ```cpp
   // tests/sir_builder_test.cpp
   TEST(SIRBuilder, SimpleVariable) {
       auto ast = parse("متغير س = 5؛");
       SIRBuilder builder;
       auto sir = builder.build(ast);
       
       EXPECT_EQ(sir->instructions().size(), 2);
       // ALLOCA + STORE
   }
   ```

3. **استخدم LLVM** (موصى به بشدة)
   - أسرع للتطوير
   - نتائج أفضل
   - دعم ممتاز

### للنجاح / For Success

✅ **ابدأ صغيراً**: compile hello world أولاً  
✅ **اختبر كثيراً**: كل feature يحتاج test  
✅ **قيّم الأداء**: قارن مع C++ دائماً  
✅ **وثّق كل شيء**: للمساهمين المستقبليين  

---

**التالي**: [الخلاصة / Conclusion →](10_conclusion.md)
