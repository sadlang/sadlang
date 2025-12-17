# المرحلة 6: Kernel تجريبي
# Stage 6: Experimental Kernel

**المدة المتوقعة / Duration:** 2-3 أشهر / months  
**الأولوية / Priority:** P1 (مهم / Important)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### العربية

هذه المرحلة تهدف لبناء kernel تجريبي كامل بلغة ص يعمل على x86-64. سيكون هذا Kernel بمثابة proof of concept ويوضح إمكانية استخدام لغة ص لبناء أنظمة تشغيل حقيقية.

**الهدف الرئيسي:** بناء kernel وظيفي يعمل على QEMU ويدعم الميزات الأساسية.

### English

This stage aims to build a complete experimental kernel in Sad Language running on x86-64. This kernel will serve as proof of concept and demonstrate the possibility of using Sad for real OS development.

**Main Goal:** Build functional kernel running on QEMU with basic features.

---

## 🎯 ميزات الـ Kernel / Kernel Features

### العربية

1. ✅ Boot و Initialization
2. ✅ Memory Management (Paging, Heap)
3. ✅ Interrupt Handling (IDT, ISRs)
4. ✅ Device Drivers (Keyboard, Serial, Timer)
5. ✅ Process Management (Basic Scheduler)
6. ✅ System Calls
7. ✅ Virtual File System (VFS)
8. ✅ Shell بسيط
9. ✅ Multi-tasking (Optional)

### English

1. ✅ Boot & Initialization
2. ✅ Memory Management (Paging, Heap)
3. ✅ Interrupt Handling (IDT, ISRs)
4. ✅ Device Drivers (Keyboard, Serial, Timer)
5. ✅ Process Management (Basic Scheduler)
6. ✅ System Calls
7. ✅ Virtual File System (VFS)
8. ✅ Simple Shell
9. ✅ Multi-tasking (Optional)

---

## 🏗️ معمارية الـ Kernel / Kernel Architecture

```
┌─────────────────────────────────────────┐
│         User Space / مساحة المستخدم      │
│  ┌──────────┐  ┌──────────┐  ┌────────┐ │
│  │  Shell   │  │ Programs │  │ Tests  │ │
│  └──────────┘  └──────────┘  └────────┘ │
└─────────────────┬───────────────────────┘
                  │ System Calls
┌─────────────────┴───────────────────────┐
│       Kernel Space / مساحة النواة        │
│  ┌──────────────────────────────────┐   │
│  │      System Call Interface       │   │
│  └──────────────────────────────────┘   │
│  ┌──────────┐  ┌──────────┐  ┌──────┐  │
│  │   VFS    │  │ Process  │  │ Memory│  │
│  │          │  │ Manager  │  │Manager│  │
│  └──────────┘  └──────────┘  └──────┘  │
│  ┌─────────────────────────────────┐   │
│  │       Device Drivers             │   │
│  │  [Keyboard][Serial][Timer][Disk] │   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │      Interrupt Handling          │   │
│  │        (IDT, ISRs, IRQs)         │   │
│  └─────────────────────────────────┘   │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────┴───────────────────────┐
│          Hardware / العتاد               │
└─────────────────────────────────────────┘
```

---

## 📝 المهام التفصيلية / Detailed Tasks

### 6.1 Boot و Initialization (أسبوع واحد)

#### العربية

```sad
# boot.s - Boot entry point

# Multiboot header
استيراد "multiboot.s" ;

# Constants
ثابت STACK_SIZE = 16384 ;  # 16KB stack

# Boot stack
قسم .bss
    مصفوفة<بايت> boot_stack[STACK_SIZE] ;
نهاية

# Entry point
قسم .text
دالة خارجي _start()
    # Set up stack
    تجميع
        mov esp, boot_stack + STACK_SIZE
        mov ebp, esp
    نهاية
    
    # Call kernel main
    kernel_main()
    
    # Halt if kernel returns
    بينما (صحيح)
        تجميع { hlt }
    نهاية
نهاية

# Kernel main
دالة kernel_main()
    # Initialize terminal
    terminal_init()
    
    اطبع("مرحباً من نواة ص!")
    اطبع("Hello from Sad Kernel!")
    
    # Initialize subsystems
    gdt_init()
    idt_init()
    memory_init()
    
    اطبع("تم تهيئة النواة بنجاح")
    اطبع("Kernel initialized successfully")
    
    # Enable interrupts
    تجميع { sti }
    
    # Start shell
    shell_main()
نهاية
```

**المهام:**
- [ ] Multiboot header implementation
- [ ] Stack setup
- [ ] Early terminal output
- [ ] GDT initialization
- [ ] IDT initialization
- [ ] Testing boot on QEMU

---

### 6.2 Memory Management (أسبوعان)

#### العربية

**أسبوع 1: Physical Memory Manager**

```sad
# memory/pmm.s - Physical Memory Manager

صنف PMM {
    خاص رقم total_memory ;
    خاص رقم used_memory ;
    خاص مؤشر<رقم> bitmap ;
    
    دالة init(رقم mem_size)
        total_memory = mem_size
        used_memory = 0
        
        # Initialize bitmap for page tracking
        bitmap = allocate_early_memory(mem_size / PAGE_SIZE / 8)
        
        # Mark kernel pages as used
        mark_kernel_pages()
        
        اطبع("PMM: ", total_memory / 1024 / 1024, " MB memory detected")
    نهاية
    
    دالة alloc_page() -> رقم
        # Find free page in bitmap
        رقم page = find_free_page()
        إذا (page != 0)
            set_page_used(page)
            used_memory = used_memory + PAGE_SIZE
        نهاية
        إرجاع page
    نهاية
    
    دالة free_page(رقم page)
        set_page_free(page)
        used_memory = used_memory - PAGE_SIZE
    نهاية
}
```

**أسبوع 2: Virtual Memory Manager**

```sad
# memory/vmm.s - Virtual Memory Manager

صنف VMM {
    خاص مؤشر<فارغ> page_directory ;
    
    دالة init()
        # Create initial page directory
        page_directory = PMM.alloc_page()
        
        # Identity map first 4MB
        identity_map(0, 4 * 1024 * 1024)
        
        # Map kernel higher half
        map_kernel_space()
        
        # Enable paging
        تجميع
            mov eax, page_directory
            mov cr3, eax
            mov eax, cr0
            or eax, 0x80000000  # Enable paging bit
            mov cr0, eax
        نهاية
        
        اطبع("VMM: Paging enabled")
    نهاية
    
    دالة map_page(رقم virt, رقم phys, رقم flags) -> منطقي
        # Get page directory entry
        رقم pd_index = virt >> 22
        رقم pt_index = (virt >> 12) & 0x3FF
        
        # Get or create page table
        مؤشر<رقم> page_table = get_page_table(pd_index)
        إذا (page_table == فارغ)
            page_table = create_page_table(pd_index)
        نهاية
        
        # Map page
        page_table[pt_index] = phys | flags
        
        # Flush TLB
        تجميع
            invlpg [virt]
        نهاية
        
        إرجاع صحيح
    نهاية
}
```

**المهام:**
- [ ] Physical memory manager
- [ ] Page frame allocator
- [ ] Virtual memory manager
- [ ] Paging setup (4KB pages)
- [ ] Kernel heap allocator
- [ ] Testing memory operations

---

### 6.3 Interrupt Handling (أسبوع واحد)

#### العربية

```sad
# interrupts/idt.s - Interrupt Descriptor Table

# IDT entry structure
صنف IDT_Entry {
    غير_صحيح16 base_low ;
    غير_صحيح16 selector ;
    غير_صحيح8 zero ;
    غير_صحيح8 flags ;
    غير_صحيح16 base_high ;
}

# IDT management
صنف IDT {
    خاص مصفوفة<IDT_Entry> entries[256] ;
    
    دالة init() {
        # Set up exception handlers (0-31)
        لكل (رقم i = 0 ; i < 32 ; i++) {
            set_gate(i, exception_handlers[i], 0x08, 0x8E) ;
        }
        
        # Set up IRQ handlers (32-47)
        لكل (رقم i = 32 ; i < 48 ; i++) {
            set_gate(i, irq_handlers[i - 32], 0x08, 0x8E) ;
        }
        
        # Load IDT
        تجميع {
            lidt [idt_ptr]
        }
        
        اطبع("IDT: Initialized") ;
    }
    
    دالة set_gate(رقم num, رقم handler, رقم selector, رقم flags) {
        entries[num].base_low = handler & 0xFFFF ;
        entries[num].base_high = (handler >> 16) & 0xFFFF ;
        entries[num].selector = selector ;
        entries[num].zero = 0 ;
        entries[num].flags = flags ;
    }
}

# Exception handler
دالة exception_handler(رقم int_no, رقم err_code) {
    اطبع("Exception ", int_no, ": Error code ", err_code) ;
    
    # Halt on exception
    بينما (صحيح) {
        تجميع { hlt }
    }
}

# IRQ handler
دالة irq_handler(رقم irq_no) {
    # Call registered handler
    إذا (irq_handlers[irq_no] != فارغ) {
        irq_handlers[irq_no]() ;
    }
    
    # Send EOI to PIC
    إذا (irq_no >= 8) {
        io_write(0xA0, 0x20) ;  # Slave PIC
    }
    io_write(0x20, 0x20) ;  # Master PIC
}
```

**المهام:**
- [ ] IDT setup
- [ ] Exception handlers (0-31)
- [ ] IRQ handlers (32-47)
- [ ] PIC initialization
- [ ] Testing interrupts

---

### 6.4 Device Drivers (أسبوعان)

#### العربية

**Timer Driver:**

```sad
# drivers/timer.s

صنف Timer {
    خاص رقم ticks ;
    
    دالة init(رقم frequency) {
        ticks = 0 ;
        
        # Calculate divisor
        رقم divisor = 1193180 / frequency ;
        
        # Set PIT frequency
        io_write(0x43, 0x36) ;
        io_write(0x40, divisor & 0xFF) ;
        io_write(0x40, (divisor >> 8) & 0xFF) ;
        
        # Register IRQ handler
        register_irq_handler(0, timer_callback) ;
        
        اطبع("Timer: Initialized at ", frequency, " Hz") ;
    }
    
    دالة timer_callback() {
        ticks = ticks + 1 ;
    }
    
    دالة get_ticks() -> رقم {
        أرجع ticks ;
    }
    
    دالة sleep(رقم ms) {
        رقم target = ticks + ms ;
        بينما (ticks < target) {
            # Wait
        }
    }
}
```

**Keyboard Driver:**

```sad
# drivers/keyboard.s

صنف Keyboard {
    دالة init() {
        # Register IRQ handler
        register_irq_handler(1, keyboard_callback) ;
        
        اطبع("Keyboard: Initialized") ;
    }
    
    دالة keyboard_callback() {
        # Read scancode
        رقم scancode = io_read(0x60) ;
        
        # Convert to ASCII
        نص key = scancode_to_ascii(scancode) ;
        
        # Send to input buffer
        input_buffer_push(key) ;
    }
    
    دالة read_char() -> نص {
        بينما (input_buffer_empty()) {
            # Wait for input
        }
        أرجع input_buffer_pop() ;
    }
}
```

**Serial Driver:**

```sad
# drivers/serial.s

صنف Serial {
    ثابت COM1 = 0x3F8 ;
    
    دالة init() {
        # Initialize COM1
        io_write(COM1 + 1, 0x00) ;  # Disable interrupts
        io_write(COM1 + 3, 0x80) ;  # Enable DLAB
        io_write(COM1 + 0, 0x03) ;  # Set divisor (low byte)
        io_write(COM1 + 1, 0x00) ;  # Set divisor (high byte)
        io_write(COM1 + 3, 0x03) ;  # 8 bits, no parity, one stop bit
        io_write(COM1 + 2, 0xC7) ;  # Enable FIFO
        io_write(COM1 + 4, 0x0B) ;  # Enable IRQs, RTS/DSR set
        
        اطبع("Serial: COM1 initialized") ;
    }
    
    دالة write_char(نص c) {
        # Wait for transmit buffer empty
        بينما ((io_read(COM1 + 5) & 0x20) == 0) { }
        
        io_write(COM1, c) ;
    }
    
    دالة write_string(نص str) {
        لكل (رقم i = 0 ; i < طول(str) ; i++) {
            write_char(str[i]) ;
        }
    }
}
```

**المهام:**
- [ ] Timer driver (PIT)
- [ ] Keyboard driver
- [ ] Serial port driver
- [ ] VGA text mode driver
- [ ] Testing all drivers

---

### 6.5 Process Management (أسبوعان)

#### العربية

```sad
# process/process.s

صنف Process {
    رقم pid ;
    نص name ;
    حالة_عملية state ;
    مؤشر<فارغ> page_directory ;
    registers_t registers ;
}

صنف Scheduler {
    خاص مصفوفة<Process> processes ;
    خاص رقم current_pid ;
    
    دالة init() {
        processes = [] ;
        current_pid = 0 ;
        
        # Create kernel process
        Process kernel_proc ;
        kernel_proc.pid = 0 ;
        kernel_proc.name = "kernel" ;
        kernel_proc.state = RUNNING ;
        
        processes = أضف_عنصر(processes, kernel_proc) ;
        
        اطبع("Scheduler: Initialized") ;
    }
    
    دالة create_process(نص name, مؤشر<فارغ> entry_point) -> رقم {
        Process proc ;
        proc.pid = next_pid++ ;
        proc.name = name ;
        proc.state = READY ;
        
        # Allocate page directory
        proc.page_directory = VMM.create_address_space() ;
        
        # Set up initial registers
        proc.registers.eip = entry_point ;
        proc.registers.esp = allocate_stack() ;
        
        processes = أضف_عنصر(processes, proc) ;
        
        أرجع proc.pid ;
    }
    
    دالة schedule() {
        # Simple round-robin
        current_pid = (current_pid + 1) % طول(processes) ;
        
        إذا (processes[current_pid].state == READY) {
            processes[current_pid].state = RUNNING ;
            switch_to_process(processes[current_pid]) ;
        }
    }
}
```

**المهام:**
- [ ] Process structure
- [ ] Scheduler (round-robin)
- [ ] Context switching
- [ ] Process creation
- [ ] Testing with multiple processes

---

### 6.6 System Calls (أسبوع واحد)

#### العربية

```sad
# syscall/syscall.s

# System call numbers
ثابت SYS_EXIT = 1 ;
ثابت SYS_WRITE = 4 ;
ثابت SYS_READ = 3 ;

دالة syscall_handler(registers_t* regs) {
    رقم syscall_num = regs->eax ;
    
    اختر (syscall_num) {
        حالة SYS_EXIT:
            sys_exit(regs->ebx) ;
        حالة SYS_WRITE:
            sys_write(regs->ebx, regs->ecx, regs->edx) ;
        حالة SYS_READ:
            sys_read(regs->ebx, regs->ecx, regs->edx) ;
        افتراضي:
            اطبع("Unknown syscall: ", syscall_num) ;
    }
}

دالة sys_write(رقم fd, مؤشر<نص> buf, رقم count) -> رقم {
    إذا (fd == 1 || fd == 2) {  # stdout or stderr
        Terminal.write(buf, count) ;
        أرجع count ;
    }
    أرجع -1 ;
}
```

**المهام:**
- [ ] Syscall interface (int 0x80)
- [ ] Basic syscalls (exit, read, write)
- [ ] User space library
- [ ] Testing syscalls

---

### 6.7 Shell (أسبوع واحد)

#### العربية

```sad
# shell/shell.s

دالة shell_main() {
    اطبع("SadOS Shell v1.0") ;
    اطبع("اكتب 'مساعدة' للحصول على قائمة الأوامر") ;
    اطبع("Type 'help' for command list") ;
    
    بينما (صحيح) {
        اطبع("\n> ") ;
        
        نص command = read_line() ;
        
        إذا (command == "مساعدة" || command == "help") {
            print_help() ;
        } وإلا إذا (command == "معلومات" || command == "info") {
            print_system_info() ;
        } وإلا إذا (command == "ذاكرة" || command == "mem") {
            print_memory_info() ;
        } وإلا إذا (command == "عمليات" || command == "ps") {
            print_processes() ;
        } وإلا إذا (command == "مسح" || command == "clear") {
            Terminal.clear() ;
        } وإلا إذا (command == "إعادة" || command == "reboot") {
            reboot() ;
        } وإلا إذا (command != "") {
            اطبع("أمر غير معروف: ", command) ;
        }
    }
}

دالة print_system_info() {
    اطبع("SadOS - نظام تشغيل بلغة ص") ;
    اطبع("النسخة: 0.1.0") ;
    اطبع("المعمارية: x86-64") ;
    اطبع("الذاكرة: ", total_memory() / 1024, " KB") ;
}
```

**المهام:**
- [ ] Command prompt
- [ ] Command parsing
- [ ] Built-in commands
- [ ] User input handling

---

## 🧪 الاختبارات / Testing

### العربية

#### اختبارات الـ Kernel
- [ ] Boot test على QEMU
- [ ] Memory allocation/deallocation
- [ ] Interrupt handling
- [ ] Device driver operations
- [ ] Process switching
- [ ] System calls
- [ ] Shell commands

#### اختبارات التكامل
- [ ] برنامج user space بسيط
- [ ] Multi-process execution
- [ ] Driver communication
- [ ] Full system test

### English

#### Kernel Tests
- [ ] Boot test on QEMU
- [ ] Memory allocation/deallocation
- [ ] Interrupt handling
- [ ] Device driver operations
- [ ] Process switching
- [ ] System calls
- [ ] Shell commands

#### Integration Tests
- [ ] Simple user space program
- [ ] Multi-process execution
- [ ] Driver communication
- [ ] Full system test

---

## 📊 معايير النجاح / Success Criteria

### العربية

✅ **المرحلة مكتملة عندما:**

1. Kernel يعمل على QEMU
2. Memory management functional
3. جميع drivers تعمل
4. Shell تستجيب للأوامر
5. System calls تعمل
6. Process switching يعمل (إذا تم تنفيذه)
7. Kernel stable بدون crashes
8. Documentation كاملة

### English

✅ **Stage Complete When:**

1. Kernel boots on QEMU
2. Memory management functional
3. All drivers working
4. Shell responds to commands
5. System calls working
6. Process switching works (if implemented)
7. Kernel stable without crashes
8. Complete documentation

---

**السابق / Previous:** [المرحلة 5: أدوات البناء والتجميع](05_stage5_toolchain.md)  
**التالي / Next:** [المرحلة 7: التوثيق والمجتمع](07_stage7_documentation.md)
