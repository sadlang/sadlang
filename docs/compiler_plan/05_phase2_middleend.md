# المرحلة 2: Middle-end
# Phase 2: Middle-end Development

**المدة / Duration:** 3-4 أسابيع / weeks  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

تطوير الطبقة الوسطى التي تحول AST إلى SIR وتطبق التحسينات:
- SIR Generator (AST → SIR)
- Optimization Passes
- Control Flow Analysis
- Data Flow Analysis
- Dead Code Elimination
- Constant Folding & Propagation

---

## 🎯 الأهداف / Goals

### (AR) العربية

✅ **الأهداف الرئيسية:**
1. تحويل AST إلى SIR بنجاح 100%
2. تطبيق 5+ optimization passes
3. تحليل control flow دقيق
4. تحسين performance بنسبة 20-30%
5. الحفاظ على semantics صحيح

---

## 📝 SIR Design / تصميم SIR

### (AR) العربية

**SIR Instructions:**

```
# Arithmetic
%t0 = add i64 %a, %b
%t1 = sub i64 %a, %b
%t2 = mul i64 %a, %b
%t3 = div i64 %a, %b
%t4 = mod i64 %a, %b

# Bitwise
%t5 = and i64 %a, %b
%t6 = or i64 %a, %b
%t7 = xor i64 %a, %b
%t8 = not i64 %a
%t9 = shl i64 %a, 2
%t10 = shr i64 %a, 2

# Comparison
%t11 = eq i64 %a, %b
%t12 = ne i64 %a, %b
%t13 = lt i64 %a, %b
%t14 = le i64 %a, %b
%t15 = gt i64 %a, %b
%t16 = ge i64 %a, %b

# Memory
%t17 = load ptr %addr
store ptr %addr, i64 %value
%t18 = alloc i64 100
free ptr %addr

# Pointers
%t19 = addr_of %var
%t20 = deref ptr %ptr
write_to ptr %ptr, i64 %value

# Control Flow
jmp label %target
br i1 %cond, label %true, label %false
call @func(i64 %arg1, i64 %arg2)
ret i64 %value

# SSA
%t21 = phi i64 [%val1, %bb1], [%val2, %bb2]
```

---

## 📝 المهام / Tasks

### 1. SIR Generator (أسبوع واحد)

```cpp
// compiler/ir/sir_builder.h
class SIRBuilder {
private:
    مؤشر<SIRModule> module
    مؤشر<SIRFunction> current_function
    مؤشر<SIRBasicBlock> current_block
    رقم temp_counter
    
public:
    # AST → SIR
    دالة generate(مؤشر<Program> program) -> مؤشر<SIRModule>
    
    # Functions
    دالة gen_function(مؤشر<FunctionDecl> func) -> مؤشر<SIRFunction>
    
    # Statements
    دالة gen_statement(مؤشر<Statement> stmt) -> فارغ
    دالة gen_if(مؤشر<IfStatement> if_stmt) -> فارغ
    دالة gen_while(مؤشر<WhileLoop> while_loop) -> فارغ
    دالة gen_return(مؤشر<ReturnStatement> ret) -> فارغ
    
    # Expressions
    دالة gen_expression(مؤشر<Expression> expr) -> نص
    دالة gen_binary(مؤشر<BinaryExpression> bin) -> نص
    دالة gen_call(مؤشر<CallExpression> call) -> نص
    
    # Helpers
    دالة create_temp() -> نص
    دالة create_label() -> نص
    دالة emit(مؤشر<SIRInstruction> inst) -> فارغ
نهاية
```

**المهام:**
- [ ] تطبيق SIR classes
- [ ] تطبيق SIRBuilder
- [ ] تحويل جميع AST nodes
- [ ] SSA form generation
- [ ] اختبارات SIR generation (50+ tests)

---

### 2. Control Flow Analysis (أسبوع واحد)

```cpp
// compiler/ir/cfg.h
class BasicBlock {
    نص label
    مصفوفة<مؤشر<SIRInstruction>> instructions
    مصفوفة<مؤشر<BasicBlock>> predecessors
    مصفوفة<مؤشر<BasicBlock>> successors
نهاية

class ControlFlowGraph {
    مؤشر<BasicBlock> entry
    مؤشر<BasicBlock> exit
    مصفوفة<مؤشر<BasicBlock>> blocks
    
    دالة build(مؤشر<SIRFunction> func) -> فارغ
    دالة compute_dominators() -> فارغ
    دالة compute_postdominators() -> فارغ
    دالة find_loops() -> مصفوفة<Loop>
نهاية
```

**المهام:**
- [ ] تطبيق CFG
- [ ] Dominator tree
- [ ] Loop detection
- [ ] اختبارات CFG (30+ tests)

---

### 3. Optimization Passes (أسبوعان)

```cpp
// compiler/ir/optimizer.h
class Optimizer {
public:
    دالة optimize(مؤشر<SIRModule> module, رقم level) -> فارغ
    
    # Level 0: No optimization
    # Level 1: Basic
    دالة constant_folding() -> فارغ
    دالة constant_propagation() -> فارغ
    دالة dead_code_elimination() -> فارغ
    
    # Level 2: Standard
    دالة common_subexpression_elimination() -> فارغ
    دالة copy_propagation() -> فارغ
    دالة strength_reduction() -> فارغ
    
    # Level 3: Aggressive
    دالة function_inlining() -> فارغ
    دالة loop_unrolling() -> فارغ
    دالة loop_invariant_code_motion() -> فارغ
نهاية
```

**التحسينات:**

**1. Constant Folding:**
```
Before:
  %t0 = add i64 2, 3
  %t1 = mul i64 %t0, 4

After:
  %t1 = mul i64 5, 4  → %t1 = 20
```

**2. Dead Code Elimination:**
```
Before:
  %t0 = add i64 %a, %b
  %t1 = mul i64 2, 3    # unused
  ret i64 %t0

After:
  %t0 = add i64 %a, %b
  ret i64 %t0
```

**3. Common Subexpression:**
```
Before:
  %t0 = add i64 %a, %b
  %t1 = add i64 %a, %b  # duplicate

After:
  %t0 = add i64 %a, %b
  %t1 = %t0
```

**المهام:**
- [ ] Constant folding
- [ ] Constant propagation
- [ ] Dead code elimination
- [ ] CSE
- [ ] Copy propagation
- [ ] Inlining (simple)
- [ ] اختبارات Optimization (60+ tests)

---

## 📊 معايير النجاح / Success Criteria

✅ **المرحلة مكتملة عندما:**
- [ ] SIR Generator يحول جميع AST
- [ ] CFG صحيح لجميع الدوال
- [ ] 5+ optimization passes تعمل
- [ ] Performance improvement 20-30%
- [ ] Test coverage > 80%
- [ ] جميع Stage 1 tests تمر

---

**السابق / Previous:** [المرحلة 1: Frontend](04_phase1_frontend.md)  
**التالي / Next:** [المرحلة 3: Bytecode Backend](06_phase3_bytecode.md)
