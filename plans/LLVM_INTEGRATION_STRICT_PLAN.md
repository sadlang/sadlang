# خطة دمج LLVM مع المترجم - التزام صارم
# LLVM Integration with Compiler - STRICT COMPLIANCE PLAN

**التاريخ:** 5 يناير 2026  
**الحالة:** خطة مفصلة - قبل البدء  
**الالتزام:** 100% بـ STRICT_CODING_RULES.md

---

## 🎯 الهدف النهائي

دمج خط أنابيب LLVM الكامل (7 مراحل) مع المترجم الرئيسي بحيث:
- ✅ يعمل الأمر: `sad --llvm program.s` ويُنتج executable
- ✅ 0 أخطاء compilation
- ✅ 0-5 تحذيرات كحد أقصى
- ✅ كل كود موثّق بالمصدر ورقم السطر

---

## 📋 المرحلة 0: القراءة الشاملة (يومين)

### اليوم 1: قراءة Headers الأساسية

#### ✅ المهمة 0.1: قراءة SIR Headers
```bash
# ملفات يجب قراءتها بالكامل:
1. compiler/frontend/include/sir_types.h (1-نهاية)
2. compiler/frontend/include/sir_module.h (1-نهاية)
3. compiler/frontend/include/sir_builder.h (1-نهاية)
4. compiler/frontend/include/sir_instruction.h (1-نهاية)
```

**توثيق إلزامي:**
```markdown
### sir_types.h - تحليل كامل

#### Classes المُعرّفة:
| Class | سطر البداية | المتغيرات الخاصة | الدوال العامة |
|-------|-------------|-------------------|----------------|
| SIRType | XX | type_, name_ | getType(), getName() |
| SIROperand | YY | value_, type_ | getValue(), isRegister() |

#### Enums المُعرّفة:
| Enum | سطر | القيم |
|------|-----|-------|
| SIROpcode | ZZ | ADD, SUB, MUL, DIV, ... |

#### Functions المُعرّفة:
| Function | سطر | التوقيع | inline؟ |
|----------|-----|---------|---------|
| makeRegister | AA | static SIROperand makeRegister(...) | نعم/لا |
```

#### ✅ المهمة 0.2: قراءة LLVM Pipeline Headers
```bash
# ملفات يجب قراءتها بالكامل:
1. compiler/pipeline/llvm/include/llvm_compiler_pipeline.h (1-374)
2. compiler/backends/llvm/llvm_codegen.h (1-نهاية)
3. compiler/backends/llvm/llvm_optimizer.h (1-نهاية)
4. compiler/backends/llvm/llvm_type_mapper.h (1-نهاية)
5. compiler/backends/llvm/llvm_expression_builder.h (1-نهاية)
```

**توثيق إلزامي:**
```markdown
### llvm_compiler_pipeline.h - تحليل كامل

#### Class: LLVMCompilerPipeline
**السطر:** 147-374

##### المتغيرات الخاصة:
| المتغير | النوع | السطر | الاستخدام |
|---------|-------|-------|-----------|
| lexer_ | std::unique_ptr<Lexer::LexerCore> | XXX | التحليل المعجمي |
| parser_ | std::unique_ptr<Parser::ParserCore> | YYY | التحليل النحوي |
| sirBuilder_ | std::unique_ptr<SIR::SIRBuilder> | ZZZ | بناء SIR |
| codeGen_ | std::unique_ptr<LLVMCodeGen> | AAA | توليد LLVM IR |

##### الدوال العامة (Non-inline):
| الدالة | السطر | التوقيع الكامل | const؟ |
|--------|-------|----------------|--------|
| initialize | 180 | bool initialize() | لا |
| compileFile | 186 | CompilationResult compileFile(const std::string&) | لا |
| emitLLVMIR | 197 | bool emitLLVMIR(const std::string&) | لا |
| emitAssembly | 203 | bool emitAssembly(const std::string&) | لا |
| emitObjectFile | 209 | bool emitObjectFile(const std::string&) | لا |
| emitExecutable | 215 | bool emitExecutable(const std::string&) | لا |

##### الدوال الـ inline (لا تُنفذ في .cpp):
| الدالة | السطر | سبب كونها inline |
|--------|-------|-------------------|
| getModule | 221 | { return module_; } |
```

### اليوم 2: قراءة Implementation Files

#### ✅ المهمة 0.3: فحص الملفات الموجودة
```bash
# ملفات .cpp يجب فحصها:
1. compiler/frontend/src/sir_builder.cpp (قراءة كاملة)
2. compiler/pipeline/llvm/src/llvm_compiler_pipeline.cpp (قراءة كاملة)
3. compiler/backends/llvm/llvm_codegen.cpp (قراءة كاملة)
```

**توثيق الحالة الحالية:**
```markdown
### sir_builder.cpp - حالة الملف

#### الدوال المُنفذة:
| الدالة | السطر | الحالة | المشاكل |
|--------|-------|--------|---------|
| buildModule | XXX | ❌ خطأ | يستخدم makeRegister غير موجود |
| buildExpression | YYY | ❌ خطأ | يستخدم addOperand غير موجود |

#### الأخطاء المكتشفة:
1. **السطر XXX**: استخدام makeRegister() - غير موجود في sir_types.h:YYY
2. **السطر ZZZ**: استخدام addOperand() - غير موجود في sir_instruction.h:AAA
3. **السطر BBB**: استخدام MemberAccessNode - غير موجود في AST headers

#### الإصلاحات المطلوبة:
- [ ] إضافة SIROperand::makeRegister() في sir_types.h
- [ ] إضافة SIRInstruction::addOperand() في sir_instruction.h
- [ ] إضافة AST::MemberAccessNode في ast/declarations.h
```

---

## 📋 المرحلة 1: إصلاح SIR (3-4 أيام)

### اليوم 3: إصلاح sir_types.h/cpp

#### ✅ المهمة 1.1: تحليل الواجهة الحالية
```bash
# الأمر:
read_file compiler/frontend/include/sir_types.h:250-350
```

**التوثيق:**
```markdown
### SIROperand - الواجهة الحالية

#### الدوال الموجودة (السطر XXX-YYY):
- getValue() const
- getType() const
- isRegister() const
- isConstant() const

#### الدوال الناقصة (مطلوبة في sir_builder.cpp):
- static SIROperand makeRegister(const std::string& name, SIRType type)
- static SIROperand makeConstant(int value)
- static SIROperand makeLabel(const std::string& label)
```

#### ✅ المهمة 1.2: إضافة الدوال الناقصة

**قبل الكتابة - التحقق:**
```markdown
- [ ] قرأت sir_types.h بالكامل
- [ ] حددت مكان الإضافة (بعد السطر XXX)
- [ ] تحققت من عدم وجود الدالة
- [ ] حددت نوع الإرجاع بدقة
- [ ] حددت المعاملات بدقة
```

**الكود المُوثّق:**
```cpp
// ============================================================================
// الملف: compiler/frontend/include/sir_types.h
// الإضافة بعد السطر: XXX (بعد getType() const)
// ============================================================================

    /**
     * @brief إنشاء معامل من نوع سجل / Create register operand
     * @param name اسم السجل / Register name
     * @param type نوع البيانات / Data type
     * @return SIROperand معامل سجل / Register operand
     * 
     * @note مطلوب بواسطة: sir_builder.cpp:484
     */
    static SIROperand makeRegister(const std::string& name, SIRType type);
    
    /**
     * @brief إنشاء معامل من نوع ثابت / Create constant operand
     * @param value القيمة الثابتة / Constant value
     * @return SIROperand معامل ثابت / Constant operand
     * 
     * @note مطلوب بواسطة: sir_builder.cpp:527
     */
    static SIROperand makeConstant(int value);
    
    /**
     * @brief إنشاء معامل من نوع تسمية / Create label operand
     * @param label نص التسمية / Label text
     * @return SIROperand معامل تسمية / Label operand
     * 
     * @note مطلوب بواسطة: sir_builder.cpp:495
     */
    static SIROperand makeLabel(const std::string& label);

// ============================================================================
```

**التنفيذ في .cpp:**
```cpp
// ============================================================================
// الملف: compiler/frontend/src/sir_types.cpp
// الدالة: SIROperand::makeRegister
// المصدر: sir_types.h:XXX (أضيفت بتاريخ 5 يناير 2026)
// التوقيع: static SIROperand makeRegister(const std::string& name, SIRType type)
// 
// المتغيرات المُستخدمة: لا يوجد (دالة static)
// الدوال المُستدعاة: 
//   - SIROperand() constructor (sir_types.h:YYY)
// ============================================================================
SIROperand SIROperand::makeRegister(const std::string& name, SIRType type) {
    SIROperand operand;
    operand.kind_ = OperandKind::REGISTER;  // ✅ المتغير موجود في sir_types.h:ZZZ
    operand.registerName_ = name;           // ✅ المتغير موجود في sir_types.h:AAA
    operand.type_ = type;                   // ✅ المتغير موجود في sir_types.h:BBB
    return operand;
}
```

### اليوم 4: إصلاح sir_instruction.h/cpp

#### ✅ المهمة 1.3: تحليل SIRInstruction

**قراءة إلزامية:**
```bash
read_file compiler/frontend/include/sir_instruction.h:1-نهاية
```

**التوثيق:**
```markdown
### SIRInstruction - الواجهة الحالية

#### المتغيرات الخاصة:
| المتغير | النوع | السطر | الاستخدام |
|---------|-------|-------|-----------|
| opcode_ | SIROpcode | XX | كود العملية |
| operands_ | std::vector<SIROperand> | YY | المعاملات |
| type_ | SIRType | ZZ | نوع النتيجة |

#### الدوال الناقصة:
- void addOperand(const SIROperand& operand)
- const std::vector<SIROperand>& getOperands() const
```

#### ✅ المهمة 1.4: إضافة الدوال الناقصة

**في sir_instruction.h:**
```cpp
// ============================================================================
// الإضافة بعد السطر: XXX
// ============================================================================

    /**
     * @brief إضافة معامل للتعليمة / Add operand to instruction
     * @param operand المعامل المراد إضافته / Operand to add
     * 
     * @note مطلوب بواسطة: sir_builder.cpp:484-485
     */
    void addOperand(const SIROperand& operand);
    
    /**
     * @brief الحصول على جميع المعاملات / Get all operands
     * @return const std::vector<SIROperand>& قائمة المعاملات / Operands list
     */
    const std::vector<SIROperand>& getOperands() const;

// ============================================================================
```

**في sir_instruction.cpp:**
```cpp
// ============================================================================
// الدالة: SIRInstruction::addOperand
// المصدر: sir_instruction.h:XXX
// التوقيع: void addOperand(const SIROperand& operand)
// 
// المتغيرات المُستخدمة:
//   - operands_: defined at sir_instruction.h:YY (std::vector<SIROperand>)
// الدوال المُستدعاة:
//   - std::vector::push_back (standard library)
// ============================================================================
void SIRInstruction::addOperand(const SIROperand& operand) {
    operands_.push_back(operand);  // ✅ operands_ موجود في sir_instruction.h:YY
}

// ============================================================================
// الدالة: SIRInstruction::getOperands
// المصدر: sir_instruction.h:ZZ
// التوقيع: const std::vector<SIROperand>& getOperands() const
// 
// المتغيرات المُستخدمة:
//   - operands_: defined at sir_instruction.h:YY
// الدوال المُستدعاة: لا يوجد
// ============================================================================
const std::vector<SIROperand>& SIRInstruction::getOperands() const {
    return operands_;  // ✅ operands_ موجود في sir_instruction.h:YY
}
```

### اليوم 5: إصلاح sir_module.h/cpp

#### ✅ المهمة 1.5: إضافة الدوال الناقصة

**قراءة إلزامية:**
```bash
read_file compiler/frontend/include/sir_module.h:1-نهاية
grep_search "getFunctions" compiler/frontend/include/sir_module.h
```

**إذا لم توجد getFunctions:**
```cpp
// ============================================================================
// الملف: compiler/frontend/include/sir_module.h
// الإضافة في Class SIRModule
// ============================================================================

    /**
     * @brief الحصول على جميع الدوال / Get all functions
     * @return const std::vector<std::shared_ptr<SIRFunction>>& قائمة الدوال
     * 
     * @note مطلوب بواسطة: llvm_codegen.cpp:XXX
     */
    const std::vector<std::shared_ptr<SIRFunction>>& getFunctions() const;

private:
    std::vector<std::shared_ptr<SIRFunction>> functions_;  // ✅ تحقق من السطر

// ============================================================================
```

**التنفيذ:**
```cpp
// ============================================================================
// الملف: compiler/frontend/src/sir_module.cpp
// الدالة: SIRModule::getFunctions
// المصدر: sir_module.h:XXX
// التوقيع: const std::vector<std::shared_ptr<SIRFunction>>& getFunctions() const
// 
// المتغيرات المُستخدمة:
//   - functions_: defined at sir_module.h:YYY (std::vector<std::shared_ptr<SIRFunction>>)
// ============================================================================
const std::vector<std::shared_ptr<SIRFunction>>& SIRModule::getFunctions() const {
    return functions_;  // ✅ functions_ موجود في sir_module.h:YYY
}
```

### اليوم 6: إصلاح sir_builder.cpp

#### ✅ المهمة 1.6: إصلاح الأخطاء المكتشفة

**قبل الإصلاح - قائمة التحقق:**
```markdown
- [ ] قرأت sir_types.h وتأكدت من وجود makeRegister
- [ ] قرأت sir_instruction.h وتأكدت من وجود addOperand
- [ ] قرأت ast/declarations.h وتأكدت من وجود MemberAccessNode
- [ ] إذا لم يوجد MemberAccessNode - سأقوم بحذف الكود المرتبط به
```

**مثال الإصلاح:**
```cpp
// ============================================================================
// الملف: compiler/frontend/src/sir_builder.cpp
// السطر: 484-485 (قبل الإصلاح)
// المشكلة: استخدام makeRegister وaddOperand دون التحقق
// ============================================================================

// ❌ قبل الإصلاح:
SIRInstruction arraySetInst(SIROpcode::ARRAY_SET);
arraySetInst.addOperand(SIROperand::makeRegister(arrayResult.registerName, arrayResult.type));

// ✅ بعد الإصلاح:
// تحقق من وجود الدوال في:
//   - SIROperand::makeRegister: sir_types.h:XXX (أُضيفت)
//   - SIRInstruction::addOperand: sir_instruction.h:YYY (أُضيفت)
SIRInstruction arraySetInst(SIROpcode::ARRAY_SET);
arraySetInst.addOperand(  // ✅ addOperand موجود في sir_instruction.h:YYY
    SIROperand::makeRegister(  // ✅ makeRegister موجود في sir_types.h:XXX
        arrayResult.registerName, 
        arrayResult.type
    )
);
```

---

## 📋 المرحلة 2: دمج LLVM Pipeline (2-3 أيام)

### اليوم 7: تفعيل llvm_compiler_pipeline

#### ✅ المهمة 2.1: إزالة التعليقات من main.cpp

**قبل الإزالة - التحقق:**
```markdown
- [ ] قرأت llvm_compiler_pipeline.h:172 (Constructor)
- [ ] تأكدت أن Constructor لا يأخذ معاملات
- [ ] قرأت llvm_compiler_pipeline.h:180 (initialize)
- [ ] تأكدت من نوع الإرجاع: bool
- [ ] قرأت llvm_compiler_pipeline.h:186 (compileFile)
- [ ] تأكدت من المعامل: const std::string&
- [ ] تأكدت من نوع الإرجاع: CompilationResult
```

**الكود:**
```cpp
// ============================================================================
// الملف: src/main.cpp
// الدالة: compileLLVM
// المرجع: llvm_compiler_pipeline.h:172-215
// ============================================================================
int compileLLVM(const std::string& filename) {
    try {
        // ✅ Constructor - llvm_compiler_pipeline.h:172
        // التوقيع: LLVMCompilerPipeline()
        Sad::Compiler::LLVM::LLVMCompilerPipeline pipeline;
        
        // ✅ initialize - llvm_compiler_pipeline.h:180
        // التوقيع: bool initialize()
        if (!pipeline.initialize()) {
            std::cerr << "❌ فشلت التهيئة" << std::endl;
            return 1;
        }
        
        // ✅ compileFile - llvm_compiler_pipeline.h:186
        // التوقيع: CompilationResult compileFile(const std::string& sourceFile)
        auto result = pipeline.compileFile(filename);
        
        // ✅ result.success - llvm_compiler_pipeline.h:115
        // النوع: bool
        if (!result.success) {
            // ✅ result.errorMessage - llvm_compiler_pipeline.h:116
            // النوع: std::string
            std::cerr << "خطأ: " << result.errorMessage << std::endl;
            
            // ✅ result.errors - llvm_compiler_pipeline.h:117
            // النوع: std::vector<std::string>
            for (const auto& error : result.errors) {
                std::cerr << "  " << error << std::endl;
            }
            return 1;
        }
        
        // ✅ emitLLVMIR - llvm_compiler_pipeline.h:197
        // التوقيع: bool emitLLVMIR(const std::string& filename)
        std::string irFile = filename.substr(0, filename.find_last_of('.')) + ".ll";
        pipeline.emitLLVMIR(irFile);
        
        // ✅ emitAssembly - llvm_compiler_pipeline.h:203
        // التوقيع: bool emitAssembly(const std::string& filename)
        std::string asmFile = filename.substr(0, filename.find_last_of('.')) + ".asm";
        pipeline.emitAssembly(asmFile);
        
        // ✅ emitObjectFile - llvm_compiler_pipeline.h:209
        // التوقيع: bool emitObjectFile(const std::string& filename)
        std::string objFile = filename.substr(0, filename.find_last_of('.')) + ".obj";
        pipeline.emitObjectFile(objFile);
        
        // ✅ emitExecutable - llvm_compiler_pipeline.h:215
        // التوقيع: bool emitExecutable(const std::string& filename)
        std::string exeFile = filename.substr(0, filename.find_last_of('.')) + ".exe";
        pipeline.emitExecutable(exeFile);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ خطأ: " << e.what() << std::endl;
        return 1;
    }
}
```

### اليوم 8: إصلاح أخطاء البناء

#### ✅ المهمة 2.2: البناء التجريبي

```bash
# الأمر:
cmake --build build --config Release --target sad
```

**توثيق الأخطاء:**
```markdown
### أخطاء البناء - جلسة 1

| # | الخطأ | الملف | السطر | السبب | الحل |
|---|-------|-------|-------|-------|------|
| 1 | unresolved external symbol SIRBuilder::SIRBuilder | llvm_compiler_pipeline.cpp | XXX | Constructor غير منفذ | تنفيذه في sir_builder.cpp |
| 2 | unresolved external symbol SIRBuilder::buildModule | llvm_compiler_pipeline.cpp | YYY | buildModule غير منفذ | تنفيذه في sir_builder.cpp |

#### خطة الإصلاح:
1. [ ] قراءة sir_builder.h بالكامل
2. [ ] التحقق من Constructor
3. [ ] تنفيذ Constructor في sir_builder.cpp
4. [ ] التحقق من buildModule
5. [ ] تنفيذ buildModule في sir_builder.cpp
```

### اليوم 9: الاختبار النهائي

#### ✅ المهمة 2.3: اختبار الأمر

```bash
# اختبار 1: ملف بسيط
sad --llvm test_simple.s

# اختبار 2: ملف متوسط
sad --llvm test_functions.s

# اختبار 3: ملف معقد
sad --llvm test_advanced.s
```

**توثيق النتائج:**
```markdown
### نتائج الاختبار

| الملف | الحالة | الأخطاء | التحذيرات | الناتج |
|-------|--------|---------|------------|--------|
| test_simple.s | ✅ | 0 | 2 | test_simple.exe |
| test_functions.s | ✅ | 0 | 3 | test_functions.exe |
| test_advanced.s | ❌ | 1 | 5 | - |

#### الأخطاء المتبقية:
1. **test_advanced.s**: خطأ في Type Checking (السطر 45)
   - السبب: نوع غير معروف
   - الحل المقترح: إضافة الدعم للنوع في type_mapper
```

---

## 📋 المرحلة 3: التحسين والتوثيق (1 يوم)

### اليوم 10: التحسين النهائي

#### ✅ المهمة 3.1: تحسين الأداء

```cpp
// ============================================================================
// تحسين: استخدام reserve() لتقليل allocations
// الملف: sir_builder.cpp
// الدالة: buildModule
// ============================================================================
std::vector<std::shared_ptr<SIRFunction>> functions;
functions.reserve(ast.size());  // ✅ تحسين الأداء
```

#### ✅ المهمة 3.2: كتابة التوثيق النهائي

**ملف: LLVM_INTEGRATION_COMPLETE.md**
```markdown
# دمج LLVM - مكتمل
## LLVM Integration - Complete

**التاريخ:** [تاريخ الإنجاز]
**الحالة:** ✅ مكتمل 100%

### الملفات المُعدّلة:
| الملف | الأسطر المُضافة | الأسطر المُعدّلة | التوثيق |
|-------|-----------------|-------------------|----------|
| sir_types.h | 15 | 0 | ✅ |
| sir_types.cpp | 45 | 0 | ✅ |
| sir_instruction.h | 8 | 0 | ✅ |
| sir_instruction.cpp | 20 | 0 | ✅ |
| sir_module.h | 5 | 0 | ✅ |
| sir_module.cpp | 10 | 0 | ✅ |
| sir_builder.cpp | 0 | 150 | ✅ |
| main.cpp | 80 | 30 | ✅ |

### النتائج:
- ✅ 0 أخطاء compilation
- ✅ 5 تحذيرات فقط (كلها من LLVM headers)
- ✅ كل دالة موثقة بالمصدر
- ✅ كل متغير محقق من وجوده
- ✅ 100% التزام بـ STRICT_CODING_RULES.md
```

---

## 📊 نظام التتبع

### قائمة التحقق الشاملة

#### المرحلة 0: القراءة (يومان)
- [ ] قراءة sir_types.h كاملاً (توثيق: ✅/❌)
- [ ] قراءة sir_module.h كاملاً (توثيق: ✅/❌)
- [ ] قراءة sir_builder.h كاملاً (توثيق: ✅/❌)
- [ ] قراءة sir_instruction.h كاملاً (توثيق: ✅/❌)
- [ ] قراءة llvm_compiler_pipeline.h كاملاً (توثيق: ✅/❌)
- [ ] قراءة llvm_codegen.h كاملاً (توثيق: ✅/❌)
- [ ] فحص sir_builder.cpp للأخطاء (توثيق: ✅/❌)

#### المرحلة 1: إصلاح SIR (4 أيام)
- [ ] إضافة SIROperand::makeRegister (توثيق: ✅/❌)
- [ ] إضافة SIROperand::makeConstant (توثيق: ✅/❌)
- [ ] إضافة SIROperand::makeLabel (توثيق: ✅/❌)
- [ ] إضافة SIRInstruction::addOperand (توثيق: ✅/❌)
- [ ] إضافة SIRInstruction::getOperands (توثيق: ✅/❌)
- [ ] إضافة SIRModule::getFunctions (توثيق: ✅/❌)
- [ ] إصلاح sir_builder.cpp (توثيق: ✅/❌)
- [ ] بناء sad_core.lib بنجاح (أخطاء: 0)

#### المرحلة 2: دمج LLVM (3 أيام)
- [ ] تفعيل compileLLVM في main.cpp (توثيق: ✅/❌)
- [ ] بناء sad.exe بنجاح (أخطاء: 0)
- [ ] اختبار sad --llvm test.s (نتيجة: ✅/❌)
- [ ] توليد .ll/.asm/.obj/.exe (نتيجة: ✅/❌)
- [ ] تشغيل الـ executable (نتيجة: ✅/❌)

#### المرحلة 3: التحسين (يوم واحد)
- [ ] تحسين الأداء (توثيق: ✅/❌)
- [ ] كتابة التوثيق النهائي (✅/❌)
- [ ] مراجعة شاملة (✅/❌)

---

## 🎯 معايير النجاح النهائية

### يجب تحقيق:
1. ✅ **0 أخطاء compilation**
2. ✅ **0-5 تحذيرات** (من LLVM headers فقط)
3. ✅ **كل دالة موثقة** بالمصدر ورقم السطر
4. ✅ **لا دوال مخترعة** - كل شيء من headers
5. ✅ **لا متغيرات مخترعة** - كل شيء محقق
6. ✅ **الأمر يعمل**: `sad --llvm program.s` → ينتج executable
7. ✅ **الـ executable يعمل**: `program.exe` → ينفذ البرنامج

---

## 🔒 التعهد النهائي

**أتعهد بما يلي:**

1. ✅ لن أكتب أي سطر كود قبل قراءة الـ header المرتبط به
2. ✅ لن أستخدم أي متغير إلا بعد التحقق من وجوده في الـ header
3. ✅ لن أستخدم أي دالة إلا بعد التحقق من توقيعها الكامل
4. ✅ لن أخترع أي دالة أو متغير جديد دون إضافته للـ header أولاً
5. ✅ سأوثق كل استخدام بالمصدر ورقم السطر
6. ✅ سأتحقق من أنواع البيانات بدقة (unique_ptr vs shared_ptr vs *)
7. ✅ لن أعيد تنفيذ أي دالة inline
8. ✅ سأختبر البناء بعد كل تعديل رئيسي

---

## ⚠️ إجراءات الطوارئ

### إذا ظهرت أخطاء compilation:

#### خطوة 1: التوقف فوراً
```
❌ STOP - لا تستمر في الكتابة
```

#### خطوة 2: تحليل الخطأ
```markdown
### تحليل الخطأ

**رسالة الخطأ الكاملة:**
```
[نسخ الرسالة هنا]
```

**الملف:** [اسم الملف]
**السطر:** [رقم السطر]
**السبب المحتمل:**
- [ ] متغير غير موجود
- [ ] دالة غير موجودة
- [ ] نوع بيانات خاطئ
- [ ] دالة inline معاد تنفيذها
```

#### خطوة 3: الإصلاح
```markdown
### خطة الإصلاح

1. [ ] قراءة الـ header مرة أخرى
2. [ ] التحقق من وجود المتغير/الدالة
3. [ ] التحقق من النوع بدقة
4. [ ] تطبيق الإصلاح
5. [ ] إعادة البناء
6. [ ] التوثيق
```

---

## 📈 التقدم اليومي

### نموذج التقرير اليومي

```markdown
# تقرير اليوم [X]
## التاريخ: [التاريخ]

### المهام المُنجزة:
- ✅ [مهمة 1]: [الوصف]
- ✅ [مهمة 2]: [الوصف]

### الملفات المُعدّلة:
| الملف | التعديلات | الحالة |
|-------|-----------|--------|
| [ملف 1] | [+X -Y] | ✅/❌ |

### الأخطاء المكتشفة:
1. **[خطأ 1]**: [الوصف] - [الحل]
2. **[خطأ 2]**: [الوصف] - [الحل]

### الدروس المستفادة:
- [درس 1]
- [درس 2]

### التقييم الذاتي:
- الالتزام بالقواعد: [X/10]
- جودة الكود: [X/10]
- التوثيق: [X/10]
- **المجموع:** [X/30]

### خطة الغد:
- [ ] [مهمة 1]
- [ ] [مهمة 2]
```

---

## 🏁 الخلاصة

هذه خطة صارمة **100% ملتزمة بـ STRICT_CODING_RULES.md**

### المبادئ الأساسية:
1. ✅ **اقرأ قبل أن تكتب**
2. ✅ **تحقق قبل أن تستخدم**
3. ✅ **وثّق كل شيء**
4. ✅ **لا اختراع - فقط استخدام ما هو موجود**

### الوقت المقدّر:
- **القراءة:** 2 يوم
- **إصلاح SIR:** 4 أيام
- **دمج LLVM:** 3 أيام
- **التحسين:** 1 يوم
- **المجموع:** **10 أيام عمل**

### النتيجة المتوقعة:
✅ **sad --llvm program.s** → يعمل بنجاح 100%

---

**نهاية الخطة الصارمة**

*آخر تحديث: 5 يناير 2026*  
*الالتزام: 100% بـ STRICT_CODING_RULES.md*
*لا مجال للخطأ - الدقة فوق كل شيء*
