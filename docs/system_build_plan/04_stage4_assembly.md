# المرحلة 4: دعم لغة التجميع
# Stage 4: Assembly Language Support

**المدة المتوقعة / Duration:** 2-3 أشهر / months  
**الأولوية / Priority:** P1 (مهم / Important)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### العربية

هذه المرحلة تهدف لإضافة دعم كامل للغة التجميع (Assembly) في لغة ص، بما في ذلك inline assembly داخل الكود والربط مع ملفات assembly خارجية. هذا ضروري للتحكم الدقيق بالعتاد وكتابة kernel code محسّن.

**الهدف الرئيسي:** تمكين كتابة كود assembly مباشرة داخل لغة ص أو الربط مع ملفات asm.

### English

This stage aims to add complete assembly language support to Sad Language, including inline assembly within code and linking with external assembly files. This is essential for precise hardware control and writing optimized kernel code.

**Main Goal:** Enable writing assembly code directly in Sad or linking with asm files.

---

## 🎯 الأهداف التفصيلية / Detailed Goals

### العربية

1. ✅ دعم Inline Assembly في الكود
2. ✅ دعم Extended Inline Assembly (GCC style)
3. ✅ الربط مع ملفات `.asm` أو `.s` خارجية
4. ✅ دعم AT&T و Intel syntax
5. ✅ دعم Constraints و Clobbers
6. ✅ دعم معماريات مختلفة (x86-64, ARM, RISC-V)
7. ✅ أدوات تجميع مدمجة
8. ✅ Debug info لكود assembly

### English

1. ✅ Support inline assembly in code
2. ✅ Support extended inline assembly (GCC style)
3. ✅ Link with external `.asm` or `.s` files
4. ✅ Support AT&T and Intel syntax
5. ✅ Support constraints and clobbers
6. ✅ Support different architectures (x86-64, ARM, RISC-V)
7. ✅ Integrated assembly tools
8. ✅ Debug info for assembly code

---

## 🏗️ الأنواع المدعومة / Supported Types

### 1. Inline Assembly (Basic)

#### العربية

```sad
# تجميع بسيط inline
دالة قراءة_CR3() -> رقم {
    رقم قيمة ;
    تجميع {
        mov rax, cr3
        mov [قيمة], rax
    }
    أرجع قيمة ;
}

# مثال: تعطيل المقاطعات
دالة تعطيل_مقاطعات() {
    تجميع {
        cli
    }
}

# مثال: تفعيل المقاطعات
دالة تفعيل_مقاطعات() {
    تجميع {
        sti
    }
}
```

#### English

```sad
# Simple inline assembly
دالة read_CR3() -> number
    number value
    assembly
        mov rax, cr3
        mov [value], rax
    نهاية
    إرجاع value
نهاية

# Example: Disable interrupts
دالة disable_interrupts()
    assembly
        cli
    نهاية
نهاية

# Example: Enable interrupts
دالة enable_interrupts()
    assembly
        sti
    نهاية
نهاية
```

---

### 2. Extended Inline Assembly (GCC Style)

#### العربية

```sad
# تجميع موسّع مع inputs و outputs
دالة جمع_asm(رقم أ, رقم ب) -> رقم
    رقم نتيجة
    تجميع موسّع (
        "add %1, %0"
        : "=r" (نتيجة)      # output
        : "r" (أ), "0" (ب)   # inputs
        : "cc"              # clobbers
    )
    إرجاع نتيجة
نهاية

# مثال: CPUID
دالة cpuid(رقم ورقة) -> قاموس
    رقم eax, ebx, ecx, edx
    تجميع موسّع (
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (ورقة)
    )
    إرجاع {"eax": eax, "ebx": ebx, "ecx": ecx, "edx": edx}
نهاية
```

#### English

```sad
# Extended assembly with inputs and outputs
دالة add_asm(number a, number b) -> number
    number result
    extended_assembly (
        "add %1, %0"
        : "=r" (result)      # output
        : "r" (a), "0" (b)   # inputs
        : "cc"              # clobbers
    )
    إرجاع result
نهاية

# Example: CPUID
دالة cpuid(number leaf) -> map
    number eax, ebx, ecx, edx
    extended_assembly (
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (leaf)
    )
    إرجاع {"eax": eax, "ebx": ebx, "ecx": ecx, "edx": edx}
نهاية
```

---

### 3. External Assembly Files

#### العربية

```sad
# استيراد ملف assembly خارجي
استيراد_تجميع "boot.asm" ;

# استدعاء دالة من ملف assembly
خارجي دالة boot_entry() ;
خارجي دالة load_gdt(مؤشر<فارغ> gdt_ptr) ;

# في الاستخدام
boot_entry() ;
load_gdt(عنوان_GDT) ;
```

#### English

```sad
# Import external assembly file
import_assembly "boot.asm" ;

# Call function from assembly file
external function boot_entry() ;
external function load_gdt(pointer<void> gdt_ptr) ;

# Usage
boot_entry() ;
load_gdt(gdt_address) ;
```

**ملف boot.asm:**

```nasm
; boot.asm - x86-64 boot code
section .text
global boot_entry
global load_gdt

boot_entry:
    cli
    ; setup code...
    ret

load_gdt:
    lgdt [rdi]  ; rdi contains gdt_ptr
    ret
```

---

### 4. Intel vs AT&T Syntax

#### العربية

```sad
# Intel Syntax (default)
تجميع intel {
    mov rax, cr3
    add rax, rbx
}

# AT&T Syntax
تجميع att {
    movq %cr3, %rax
    addq %rbx, %rax
}
```

#### English

```sad
# Intel Syntax (default)
assembly intel {
    mov rax, cr3
    add rax, rbx
}

# AT&T Syntax
assembly att {
    movq %cr3, %rax
    addq %rbx, %rax
}
```

---

## 📝 المهام التفصيلية / Detailed Tasks

### 4.1 تصميم Syntax و Parser (أسبوعان)

#### العربية
- [ ] إضافة keywords: `تجميع`, `assembly`, `موسّع`, `extended`
- [ ] تصميم AST node لـ `InlineAssemblyStmt`
- [ ] تحديث Parser لتحليل inline assembly blocks
- [ ] دعم multi-line assembly code
- [ ] دعم template strings في assembly
- [ ] اختبار parsing لأمثلة مختلفة

#### English
- [ ] Add keywords: `تجميع`, `assembly`, `موسّع`, `extended`
- [ ] Design AST node for `InlineAssemblyStmt`
- [ ] Update Parser to parse inline assembly blocks
- [ ] Support multi-line assembly code
- [ ] Support template strings in assembly
- [ ] Test parsing for various examples

**الملفات المتأثرة / Affected Files:**
- `include/parser/ast/statements.h`
- `include/lexer/token.h`
- `src/parser/parser_core_impl.cpp`

---

### 4.2 تنفيذ Basic Inline Assembly (أسبوعان)

#### العربية
- [ ] تنفيذ `visitInlineAssemblyStmt()` في LLVM generator
- [ ] توليد LLVM inline asm من assembly code
- [ ] دعم constraints أساسية (r, m, i)
- [ ] دعم clobbers (cc, memory)
- [ ] اختبار مع تعليمات بسيطة (mov, add, sub)
- [ ] اختبار على x86-64

#### English
- [ ] Implement `visitInlineAssemblyStmt()` in LLVM generator
- [ ] Generate LLVM inline asm from assembly code
- [ ] Support basic constraints (r, m, i)
- [ ] Support clobbers (cc, memory)
- [ ] Test with simple instructions (mov, add, sub)
- [ ] Test on x86-64

**مثال LLVM IR:**

```llvm
; LLVM IR لـ inline assembly
%1 = call i64 asm "mov $0, cr3", "=r,~{dirflag},~{fpsr},~{flags}"()
```

---

### 4.3 تنفيذ Extended Inline Assembly (ثلاثة أسابيع)

#### العربية

**أسبوع 1: Input/Output Constraints**
- [ ] دعم output constraints (=r, =m, +r)
- [ ] دعم input constraints (r, m, i, n)
- [ ] دعم matching constraints (0, 1, 2)
- [ ] تحليل وتحويل constraints إلى LLVM format

**أسبوع 2: Clobbers و Side Effects**
- [ ] دعم register clobbers (rax, rbx, etc.)
- [ ] دعم memory clobber
- [ ] دعم volatile assembly
- [ ] دعم goto targets

**أسبوع 3: Advanced Features**
- [ ] دعم symbolic names
- [ ] دعم multiple outputs
- [ ] دعم early-clobber (&)
- [ ] اختبار مع أمثلة معقدة

#### English

**Week 1: Input/Output Constraints**
- [ ] Support output constraints (=r, =m, +r)
- [ ] Support input constraints (r, m, i, n)
- [ ] Support matching constraints (0, 1, 2)
- [ ] Parse and convert constraints to LLVM format

**Week 2: Clobbers & Side Effects**
- [ ] Support register clobbers (rax, rbx, etc.)
- [ ] Support memory clobber
- [ ] Support volatile assembly
- [ ] Support goto targets

**Week 3: Advanced Features**
- [ ] Support symbolic names
- [ ] Support multiple outputs
- [ ] Support early-clobber (&)
- [ ] Test with complex examples

---

### 4.4 الربط مع ملفات Assembly خارجية (أسبوعان)

#### العربية
- [ ] دعم `استيراد_تجميع` / `import_assembly`
- [ ] تكامل مع assembler (NASM, GAS)
- [ ] دعم `خارجي` / `external` functions
- [ ] الربط مع `.o` files من assembler
- [ ] دعم calling conventions (cdecl, stdcall, fastcall)
- [ ] اختبار مع ملفات asm معقدة

#### English
- [ ] Support `استيراد_تجميع` / `import_assembly`
- [ ] Integrate with assembler (NASM, GAS)
- [ ] Support `external` functions
- [ ] Link with `.o` files from assembler
- [ ] Support calling conventions (cdecl, stdcall, fastcall)
- [ ] Test with complex asm files

**Build Pipeline:**

```
program.s → sadc → program.ll
boot.asm → nasm → boot.o
                ↓
            linker → executable
```

---

### 4.5 دعم AT&T Syntax (أسبوع واحد)

#### العربية
- [ ] إضافة mode selector (intel/att)
- [ ] تحويل syntax في code generator
- [ ] دعم AT&T operand order
- [ ] دعم % prefix للregisters
- [ ] دعم $ prefix للimmediates
- [ ] اختبار مقارنة بين Intel و AT&T

#### English
- [ ] Add mode selector (intel/att)
- [ ] Convert syntax in code generator
- [ ] Support AT&T operand order
- [ ] Support % prefix for registers
- [ ] Support $ prefix for immediates
- [ ] Test comparison between Intel and AT&T

---

### 4.6 دعم معماريات مختلفة (أسبوعان)

#### العربية

**x86-64 (أولوية P0)**
- [x] دعم جميع GPRs (rax, rbx, rcx, rdx, rsi, rdi, etc.)
- [ ] دعم XMM registers
- [ ] دعم system registers (cr0-cr4, etc.)
- [ ] دعم segment registers

**ARM (أولوية P1)**
- [ ] دعم ARM64 (AArch64) instructions
- [ ] دعم ARM registers (x0-x30, sp, pc)
- [ ] دعم NEON SIMD
- [ ] دعم system registers

**RISC-V (أولوية P2)**
- [ ] دعم RV64 instructions
- [ ] دعم RISC-V registers (x0-x31)
- [ ] دعم CSRs (Control and Status Registers)

#### English

**x86-64 (Priority P0)**
- [x] Support all GPRs (rax, rbx, rcx, rdx, rsi, rdi, etc.)
- [ ] Support XMM registers
- [ ] Support system registers (cr0-cr4, etc.)
- [ ] Support segment registers

**ARM (Priority P1)**
- [ ] Support ARM64 (AArch64) instructions
- [ ] Support ARM registers (x0-x30, sp, pc)
- [ ] Support NEON SIMD
- [ ] Support system registers

**RISC-V (Priority P2)**
- [ ] Support RV64 instructions
- [ ] Support RISC-V registers (x0-x31)
- [ ] Support CSRs (Control and Status Registers)

---

### 4.7 أدوات تجميع مدمجة (أسبوع واحد)

#### العربية
- [ ] تكامل مع NASM
- [ ] تكامل مع GAS (GNU Assembler)
- [ ] دعم تجميع inline من command line
- [ ] عرض generated assembly code
- [ ] أدوات disassembly

#### English
- [ ] Integrate with NASM
- [ ] Integrate with GAS (GNU Assembler)
- [ ] Support inline assembly from command line
- [ ] Display generated assembly code
- [ ] Disassembly tools

**أوامر الاستخدام:**

```bash
# عرض assembly code
sadc program.s -S -o program.asm

# تجميع من assembly
sadc program.asm -c -o program.o

# Disassemble executable
sad-disasm program
```

---

### 4.8 Debug Info لكود Assembly (أسبوع واحد)

#### العربية
- [ ] توليد DWARF debug info لـ assembly
- [ ] mapping بين source lines و assembly instructions
- [ ] دعم breakpoints في assembly code
- [ ] دعم stepping في debugger
- [ ] اختبار debugging مع GDB

#### English
- [ ] Generate DWARF debug info for assembly
- [ ] Mapping between source lines and assembly instructions
- [ ] Support breakpoints in assembly code
- [ ] Support stepping in debugger
- [ ] Test debugging with GDB

---

## 🧪 الاختبارات / Testing

### العربية

#### اختبارات الوحدة
- اختبار parsing لكل نوع من inline assembly
- اختبار توليد LLVM IR صحيح
- اختبار constraints و clobbers
- اختبار الربط مع ملفات خارجية

#### اختبارات التكامل
- برامج تجمع بين Sad code و assembly
- kernel code يستخدم inline assembly
- drivers تستخدم port I/O عبر assembly

#### اختبارات الأداء
- مقارنة سرعة inline assembly vs C intrinsics
- قياس overhead لـ extended inline assembly

### English

#### Unit Tests
- Test parsing for each inline assembly type
- Test correct LLVM IR generation
- Test constraints and clobbers
- Test linking with external files

#### Integration Tests
- Programs combining Sad code and assembly
- Kernel code using inline assembly
- Drivers using port I/O via assembly

#### Performance Tests
- Compare inline assembly speed vs C intrinsics
- Measure overhead of extended inline assembly

---

## 📊 معايير النجاح / Success Criteria

### العربية

✅ **المرحلة مكتملة عندما:**

1. Basic inline assembly يعمل على x86-64
2. Extended inline assembly مع constraints كاملة
3. الربط مع ملفات .asm خارجية يعمل
4. دعم Intel و AT&T syntax
5. جميع الاختبارات تمر بنجاح
6. التوثيق كامل مع أمثلة
7. Debug info يعمل مع GDB
8. دعم على الأقل x86-64 و ARM

### English

✅ **Stage Complete When:**

1. Basic inline assembly works on x86-64
2. Extended inline assembly with full constraints
3. Linking with external .asm files works
4. Support for Intel and AT&T syntax
5. All tests pass successfully
6. Complete documentation with examples
7. Debug info works with GDB
8. Support for at least x86-64 and ARM

---

## 📚 موارد إضافية / Additional Resources

### العربية

- [GCC Inline Assembly](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)
- [LLVM Inline Assembly](https://llvm.org/docs/LangRef.html#inline-assembler-expressions)
- [Intel x86-64 Instruction Reference](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [ARM64 Instruction Set](https://developer.arm.com/documentation/ddi0596/latest)
- [RISC-V ISA Specification](https://riscv.org/technical/specifications/)
- [NASM Documentation](https://www.nasm.us/xdoc/2.15.05/html/nasmdoc0.html)

### English

- [GCC Inline Assembly]
- [LLVM Inline Assembly]
- [Intel x86-64 Instruction Reference]
- [ARM64 Instruction Set]
- [RISC-V ISA Specification]
- [NASM Documentation]

---

**السابق / Previous:** [المرحلة 3: المكتبة القياسية للنظام](03_stage3_stdlib.md)  
**التالي / Next:** [المرحلة 5: أدوات البناء والتجميع](05_stage5_toolchain.md)
