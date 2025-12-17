# المرحلة 5: أدوات البناء والتجميع
# Stage 5: Build Tools & Toolchain

**المدة المتوقعة / Duration:** 2-3 أشهر / months  
**الأولوية / Priority:** P1 (مهم / Important)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### العربية

هذه المرحلة تهدف لتطوير مجموعة كاملة من أدوات البناء والتجميع الضرورية لبناء أنظمة تشغيل. تشمل هذه الأدوات cross-compiler، linker مخصص، bootloader integration، وأدوات اختبار وتصحيح.

**الهدف الرئيسي:** توفير toolchain كامل ومتكامل لتطوير أنظمة التشغيل بلغة ص.

### English

This stage aims to develop a complete set of build and toolchain tools necessary for OS development. These tools include cross-compiler, custom linker, bootloader integration, and testing/debugging tools.

**Main Goal:** Provide complete and integrated toolchain for OS development in Sad Language.

---

## 🎯 المكونات الرئيسية / Main Components

### العربية

1. ✅ Cross-Compiler للمعماريات المختلفة
2. ✅ Linker مخصص (sad-ld)
3. ✅ Bootloader Integration (GRUB, UEFI)
4. ✅ أدوات التصحيح (Debugger Integration)
5. ✅ Emulator Integration (QEMU, Bochs)
6. ✅ Build System (Make, CMake scripts)
7. ✅ Package Manager للمكتبات
8. ✅ Documentation Generator

### English

1. ✅ Cross-Compiler for different architectures
2. ✅ Custom Linker (sad-ld)
3. ✅ Bootloader Integration (GRUB, UEFI)
4. ✅ Debugger Integration
5. ✅ Emulator Integration (QEMU, Bochs)
6. ✅ Build System (Make, CMake scripts)
7. ✅ Package Manager for libraries
8. ✅ Documentation Generator

---

## 📝 المهام التفصيلية / Detailed Tasks

### 5.1 Cross-Compiler Setup (ثلاثة أسابيع)

#### العربية

**أسبوع 1: x86-64 Cross-Compiler**
- [ ] إعداد target triple: `x86_64-elf`
- [ ] تكوين LLVM لـ bare-metal target
- [ ] اختبار compilation لـ freestanding code
- [ ] إنتاج ELF binaries بدون libc

**أسبوع 2: ARM Cross-Compiler**
- [ ] إعداد target: `aarch64-elf`
- [ ] تكوين ARM backend
- [ ] اختبار على ARM emulator
- [ ] دعم ARM-specific features

**أسبوع 3: Build Scripts**
- [ ] إنشاء `sad-cross` command
- [ ] دعم sysroot للمكتبات
- [ ] scripts للتبديل بين targets
- [ ] documentation للاستخدام

#### English

**Week 1: x86-64 Cross-Compiler**
- [ ] Setup target triple: `x86_64-elf`
- [ ] Configure LLVM for bare-metal target
- [ ] Test compilation for freestanding code
- [ ] Produce ELF binaries without libc

**Week 2: ARM Cross-Compiler**
- [ ] Setup target: `aarch64-elf`
- [ ] Configure ARM backend
- [ ] Test on ARM emulator
- [ ] Support ARM-specific features

**Week 3: Build Scripts**
- [ ] Create `sad-cross` command
- [ ] Support sysroot for libraries
- [ ] Scripts to switch between targets
- [ ] Usage documentation

**أمثلة الاستخدام:**

```bash
# التجميع لـ x86-64
sadc --target=x86_64-elf kernel.s -o kernel.elf

# التجميع لـ ARM
sadc --target=aarch64-elf kernel.s -o kernel.elf

# عرض targets المتاحة
sadc --list-targets
```

---

### 5.2 Custom Linker (sad-ld) (أربعة أسابيع)

#### العربية

**أسبوع 1: Linker Core**
- [ ] تصميم معمارية sad-ld
- [ ] قراءة ELF object files
- [ ] symbol resolution
- [ ] relocation handling

**أسبوع 2: Linker Scripts**
- [ ] دعم custom linker scripts
- [ ] تحديد memory layout
- [ ] section placement
- [ ] symbol definition

**أسبوع 3: Static & Dynamic Linking**
- [ ] static linking مع libraries
- [ ] shared library support (.so)
- [ ] position-independent code (PIC)
- [ ] lazy binding

**أسبوع 4: Optimizations**
- [ ] dead code elimination
- [ ] identical code folding (ICF)
- [ ] link-time optimization (LTO)
- [ ] strip debug symbols

#### English

**Week 1: Linker Core**
- [ ] Design sad-ld architecture
- [ ] Read ELF object files
- [ ] Symbol resolution
- [ ] Relocation handling

**Week 2: Linker Scripts**
- [ ] Support custom linker scripts
- [ ] Define memory layout
- [ ] Section placement
- [ ] Symbol definition

**Week 3: Static & Dynamic Linking**
- [ ] Static linking with libraries
- [ ] Shared library support (.so)
- [ ] Position-independent code (PIC)
- [ ] Lazy binding

**Week 4: Optimizations**
- [ ] Dead code elimination
- [ ] Identical code folding (ICF)
- [ ] Link-time optimization (LTO)
- [ ] Strip debug symbols

**مثال Linker Script:**

```ld
/* kernel.ld - Linker script للـ kernel */
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    . = 1M;
    
    .text ALIGN(4K) : {
        *(.multiboot)
        *(.text)
    }
    
    .rodata ALIGN(4K) : {
        *(.rodata)
    }
    
    .data ALIGN(4K) : {
        *(.data)
    }
    
    .bss ALIGN(4K) : {
        *(.bss)
    }
}
```

**استخدام:**

```bash
sad-ld -T kernel.ld kernel.o -o kernel.bin
```

---

### 5.3 Bootloader Integration (ثلاثة أسابيع)

#### العربية

**أسبوع 1: GRUB/Multiboot**
- [ ] إضافة multiboot header generator
- [ ] إنشاء ISO image مع GRUB
- [ ] testing على QEMU
- [ ] documentation لـ GRUB setup

**أسبوع 2: UEFI**
- [ ] إنتاج PE32+ executables
- [ ] UEFI boot services integration
- [ ] FAT32 filesystem لـ ESP
- [ ] testing على QEMU UEFI

**أسبوع 3: Custom Bootloader**
- [ ] simple bootloader بلغة ص
- [ ] loading kernel من disk
- [ ] entering protected/long mode
- [ ] passing boot information

#### English

**Week 1: GRUB/Multiboot**
- [ ] Add multiboot header generator
- [ ] Create ISO image with GRUB
- [ ] Testing on QEMU
- [ ] GRUB setup documentation

**Week 2: UEFI**
- [ ] Produce PE32+ executables
- [ ] UEFI boot services integration
- [ ] FAT32 filesystem for ESP
- [ ] Testing on QEMU UEFI

**Week 3: Custom Bootloader**
- [ ] Simple bootloader in Sad
- [ ] Loading kernel from disk
- [ ] Entering protected/long mode
- [ ] Passing boot information

**مثال Multiboot Header:**

```sad
# multiboot.s - Multiboot header
قسم .multiboot

ثابت MULTIBOOT_MAGIC = 0x1BADB002 ;
ثابت MULTIBOOT_FLAGS = 0x00000003 ;
ثابت MULTIBOOT_CHECKSUM = -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS) ;

تجميع {
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long MULTIBOOT_CHECKSUM
}
```

**إنشاء ISO:**

```bash
# Build kernel
sadc kernel.s -o kernel.bin

# Create ISO with GRUB
sad-mkiso kernel.bin -o sad-os.iso

# Test on QEMU
qemu-system-x86_64 -cdrom sad-os.iso
```

---

### 5.4 Debugger Integration (أسبوعان)

#### العربية

**أسبوع 1: GDB Integration**
- [ ] توليد DWARF debug info كامل
- [ ] symbol table generation
- [ ] source-level debugging
- [ ] GDB remote protocol support

**أسبوع 2: Custom Debugger UI**
- [ ] واجهة نصية للتصحيح (TUI)
- [ ] breakpoint management
- [ ] memory inspection
- [ ] register viewing

#### English

**Week 1: GDB Integration**
- [ ] Generate complete DWARF debug info
- [ ] Symbol table generation
- [ ] Source-level debugging
- [ ] GDB remote protocol support

**Week 2: Custom Debugger UI**
- [ ] Text UI for debugging (TUI)
- [ ] Breakpoint management
- [ ] Memory inspection
- [ ] Register viewing

**استخدام GDB:**

```bash
# تشغيل kernel على QEMU مع GDB server
qemu-system-x86_64 -kernel kernel.bin -s -S

# في terminal آخر، توصيل GDB
sad-gdb kernel.bin
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
```

---

### 5.5 Emulator Integration (أسبوع واحد)

#### العربية
- [ ] scripts لتشغيل على QEMU
- [ ] دعم serial console output
- [ ] network testing على emulator
- [ ] disk image creation tools
- [ ] automated testing scripts

#### English
- [ ] Scripts to run on QEMU
- [ ] Serial console output support
- [ ] Network testing on emulator
- [ ] Disk image creation tools
- [ ] Automated testing scripts

**مثال:**

```bash
# تشغيل kernel
sad-qemu kernel.bin

# مع serial console
sad-qemu kernel.bin --serial

# مع network
sad-qemu kernel.bin --network

# مع disk image
sad-qemu kernel.bin --disk disk.img
```

---

### 5.6 Build System (أسبوعان)

#### العربية

**أسبوع 1: CMake Integration**
- [ ] CMake modules لـ Sad
- [ ] دعم cross-compilation
- [ ] dependency management
- [ ] testing integration

**أسبوع 2: Make Templates**
- [ ] Makefile templates للمشاريع
- [ ] auto-generated build rules
- [ ] parallel building support
- [ ] clean/install targets

#### English

**Week 1: CMake Integration**
- [ ] CMake modules for Sad
- [ ] Cross-compilation support
- [ ] Dependency management
- [ ] Testing integration

**Week 2: Make Templates**
- [ ] Makefile templates for projects
- [ ] Auto-generated build rules
- [ ] Parallel building support
- [ ] Clean/install targets

**مثال CMakeLists.txt:**

```cmake
cmake_minimum_required(VERSION 3.20)
project(SadOS LANGUAGES Sad)

# Find Sad compiler
find_package(SadCompiler REQUIRED)

# Add kernel target
add_sad_executable(kernel
    kernel.s
    memory.s
    interrupts.s
)

# Set linker script
set_target_properties(kernel PROPERTIES
    LINK_FLAGS "-T ${CMAKE_SOURCE_DIR}/kernel.ld"
)

# Create bootable ISO
add_custom_target(iso
    COMMAND sad-mkiso $<TARGET_FILE:kernel> -o ${PROJECT_NAME}.iso
    DEPENDS kernel
)
```

---

### 5.7 Package Manager (أسبوعان)

#### العربية

**أسبوع 1: Package Format**
- [ ] تصميم `.sadpkg` format
- [ ] metadata specification
- [ ] dependency resolution
- [ ] version management

**أسبوع 2: Package Commands**
- [ ] `sad-pkg install`
- [ ] `sad-pkg search`
- [ ] `sad-pkg update`
- [ ] repository management

#### English

**Week 1: Package Format**
- [ ] Design `.sadpkg` format
- [ ] Metadata specification
- [ ] Dependency resolution
- [ ] Version management

**Week 2: Package Commands**
- [ ] `sad-pkg install`
- [ ] `sad-pkg search`
- [ ] `sad-pkg update`
- [ ] Repository management

**مثال package.json:**

```json
{
  "name": "sad-drivers",
  "version": "1.0.0",
  "description": "Common device drivers for SadOS",
  "author": "Sad Team",
  "license": "MIT",
  "dependencies": {
    "sad-stdlib": "^2.0.0"
  },
  "files": [
    "keyboard.s",
    "serial.s",
    "disk.s"
  ]
}
```

**استخدام:**

```bash
# تثبيت package
sad-pkg install sad-drivers

# البحث عن packages
sad-pkg search network

# تحديث packages
sad-pkg update
```

---

### 5.8 Documentation Generator (أسبوع واحد)

#### العربية
- [ ] parser للتعليقات documentation
- [ ] توليد HTML docs
- [ ] توليد markdown
- [ ] API reference generation
- [ ] example code extraction

#### English
- [ ] Parser for documentation comments
- [ ] Generate HTML docs
- [ ] Generate markdown
- [ ] API reference generation
- [ ] Example code extraction

**مثال:**

```sad
/**
 * @brief يقرأ قيمة من منفذ I/O
 * @brief Reads value from I/O port
 * 
 * @param port رقم المنفذ / Port number
 * @return قيمة 8-bit من المنفذ / 8-bit value from port
 * 
 * @example
 * رقم قيمة = io_read(0x3F8)
 */
دالة io_read(رقم port) -> رقم
    رقم value
    تجميع
        mov dx, port
        in al, dx
        mov value, al
    نهاية
    إرجاع value
نهاية
```

```bash
# توليد documentation
sad-doc src/ -o docs/
```

---

## 🔧 الأدوات النهائية / Final Toolchain

### العربية

عند اكتمال هذه المرحلة، ستكون لدينا الأدوات التالية:

1. **sadc** - المترجم الرئيسي
2. **sad-cross** - Cross-compiler wrapper
3. **sad-ld** - Linker مخصص
4. **sad-mkiso** - ISO image creator
5. **sad-gdb** - Debugger wrapper
6. **sad-qemu** - Emulator launcher
7. **sad-pkg** - Package manager
8. **sad-doc** - Documentation generator

### English

Upon completion, we'll have these tools:

1. **sadc** - Main compiler
2. **sad-cross** - Cross-compiler wrapper
3. **sad-ld** - Custom linker
4. **sad-mkiso** - ISO image creator
5. **sad-gdb** - Debugger wrapper
6. **sad-qemu** - Emulator launcher
7. **sad-pkg** - Package manager
8. **sad-doc** - Documentation generator

---

## 🧪 الاختبارات / Testing

### العربية

#### اختبارات الوحدة
- اختبار كل أداة بشكل منفصل
- اختبار cross-compilation لكل target
- اختبار linker مع سيناريوهات مختلفة
- اختبار package manager operations

#### اختبارات التكامل
- بناء kernel كامل باستخدام toolchain
- إنشاء ISO و boot على QEMU
- debugging session كامل
- package installation و usage

### English

#### Unit Tests
- Test each tool separately
- Test cross-compilation for each target
- Test linker with different scenarios
- Test package manager operations

#### Integration Tests
- Build complete kernel using toolchain
- Create ISO and boot on QEMU
- Complete debugging session
- Package installation and usage

---

## 📊 معايير النجاح / Success Criteria

### العربية

✅ **المرحلة مكتملة عندما:**

1. جميع الأدوات تعمل بشكل صحيح
2. Cross-compilation يعمل لـ x86-64 و ARM
3. Linker ينتج bootable binaries
4. GRUB و UEFI boot يعملان
5. Debugging مع GDB يعمل
6. Package manager functional
7. Documentation generator ينتج docs جيدة
8. Build system سهل الاستخدام

### English

✅ **Stage Complete When:**

1. All tools work correctly
2. Cross-compilation works for x86-64 and ARM
3. Linker produces bootable binaries
4. GRUB and UEFI boot work
5. Debugging with GDB works
6. Package manager functional
7. Documentation generator produces good docs
8. Build system is easy to use

---

## 📚 موارد إضافية / Additional Resources

### العربية

- [GNU Binutils Documentation](https://sourceware.org/binutils/docs/)
- [LLVM Linker (LLD)](https://lld.llvm.org/)
- [GRUB Manual](https://www.gnu.org/software/grub/manual/)
- [UEFI Specification](https://uefi.org/specifications)
- [QEMU Documentation](https://www.qemu.org/docs/master/)
- [GDB User Manual](https://sourceware.org/gdb/documentation/)

### English

- [GNU Binutils Documentation]
- [LLVM Linker (LLD)]
- [GRUB Manual]
- [UEFI Specification]
- [QEMU Documentation]
- [GDB User Manual]

---

**السابق / Previous:** [المرحلة 4: دعم لغة التجميع](04_stage4_assembly.md)  
**التالي / Next:** [المرحلة 6: Kernel تجريبي](06_stage6_kernel.md)
