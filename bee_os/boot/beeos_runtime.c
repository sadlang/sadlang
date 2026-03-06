/*
 * BeeOS Low-Level Runtime Stubs (32-bit i686, freestanding)
 * Provides all sad_ll_* FFI functions referenced by ص kernel modules.
 *
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 *
 * Categories:
 *   1. Port I/O (inb/outb/inw/outw/inl/outl)
 *   2. Memory-Mapped I/O (read/write 16/32/64)
 *   3. Memory Operations (memset, memcpy)
 *   4. Serial Port (COM1)
 *   5. Interrupt Control (enable/disable/EOI)
 *   6. PIC (8259A remap + mask)
 *   7. GDT/IDT (load, set gate, init)
 *   8. Timer (PIT)
 *   9. Paging (CR3, INVLPG)
 *  10. Page Allocator (bitmap-based)
 *  11. Heap (kmalloc/kfree — bump allocator)
 *  12. Context Switch (task switching stubs)
 *  13. Framebuffer (BGA)
 */

/* ================================================================
   Compiler directives
   ================================================================ */
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef long long          i64;

/* ================================================================
   1. Port I/O
   ================================================================ */

i64 sad_ll_port_inb(i64 port) {
    u8 ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"((u16)port));
    return (i64)ret;
}

void sad_ll_port_outb(i64 port, i64 val) {
    __asm__ volatile("outb %0, %1" : : "a"((u8)val), "Nd"((u16)port));
}

i64 sad_ll_port_inw(i64 port) {
    u16 ret;
    __asm__ volatile("inw %1, %0" : "=a"(ret) : "Nd"((u16)port));
    return (i64)ret;
}

void sad_ll_port_outw(i64 port, i64 val) {
    __asm__ volatile("outw %0, %1" : : "a"((u16)val), "Nd"((u16)port));
}

i64 sad_ll_port_inl(i64 port) {
    u32 ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"((u16)port));
    return (i64)ret;
}

void sad_ll_port_outl(i64 port, i64 val) {
    __asm__ volatile("outl %0, %1" : : "a"((u32)val), "Nd"((u16)port));
}

/* ================================================================
   2. Memory-Mapped I/O
   ================================================================ */

i64 sad_ll_mem_read64(i64 addr) {
    return *(volatile i64 *)(u32)addr;
}

void sad_ll_mem_write64(i64 addr, i64 val) {
    *(volatile i64 *)(u32)addr = val;
}

i64 sad_ll_mem_read32(i64 addr) {
    return (i64)(*(volatile u32 *)(u32)addr);
}

void sad_ll_mem_write32(i64 addr, i64 val) {
    *(volatile u32 *)(u32)addr = (u32)val;
}

i64 sad_ll_mem_read16(i64 addr) {
    return (i64)(*(volatile u16 *)(u32)addr);
}

void sad_ll_mem_write16(i64 addr, i64 val) {
    *(volatile u16 *)(u32)addr = (u16)val;
}

i64 sad_ll_mem_read8(i64 addr) {
    return (i64)(*(volatile u8 *)(u32)addr);
}

void sad_ll_mem_write8(i64 addr, i64 val) {
    *(volatile u8 *)(u32)addr = (u8)val;
}

/* Aliases for filesystem modules (short names) */
i64 mem_read_byte(i64 addr) { return sad_ll_mem_read8(addr); }
void mem_write_byte(i64 addr, i64 val) { sad_ll_mem_write8(addr, val); }
i64 mem_read_word(i64 addr) { return sad_ll_mem_read32(addr); }
void mem_write_word(i64 addr, i64 val) { sad_ll_mem_write32(addr, val); }
i64 mem_read_dword(i64 addr) { return sad_ll_mem_read32(addr); }
void mem_write_dword(i64 addr, i64 val) { sad_ll_mem_write32(addr, val); }

/* ================================================================
   3. Memory Operations
   ================================================================ */

void sad_ll_memset(i64 addr, i64 val, i64 len) {
    u8 *p = (u8 *)(u32)addr;
    u8 v = (u8)val;
    for (i64 i = 0; i < len; i++) p[i] = v;
}

void sad_ll_memcpy(i64 dst, i64 src, i64 len) {
    u8 *d = (u8 *)(u32)dst;
    const u8 *s = (const u8 *)(u32)src;
    for (i64 i = 0; i < len; i++) d[i] = s[i];
}

/* Standard C memset/memcpy — LLVM emits calls to these on i686 */
void *memset(void *s, int c, unsigned int n) {
    u8 *p = (u8 *)s;
    for (unsigned int i = 0; i < n; i++) p[i] = (u8)c;
    return s;
}

void *memcpy(void *dst, const void *src, unsigned int n) {
    u8 *d = (u8 *)dst;
    const u8 *s = (const u8 *)src;
    for (unsigned int i = 0; i < n; i++) d[i] = s[i];
    return dst;
}

/* malloc — stub for any LLVM-emitted calls */
i64 sad_ll_kmalloc(i64 size);  /* forward declaration */

void *malloc(unsigned int size) {
    return (void *)(u32)sad_ll_kmalloc((i64)size);
}

void free(void *ptr) {
    (void)ptr;
}

/* ================================================================
   3b. 64-bit division/modulo helpers for i686
       LLVM emits calls to these compiler-rt builtins
   ================================================================ */

i64 __divdi3(i64 a, i64 b) {
    if (b == 0) return 0;
    int neg = 0;
    u64 ua, ub;
    if (a < 0) { ua = (u64)(-a); neg = !neg; } else { ua = (u64)a; }
    if (b < 0) { ub = (u64)(-b); neg = !neg; } else { ub = (u64)b; }
    u64 q = 0, r = 0;
    for (int i = 63; i >= 0; i--) {
        r = (r << 1) | ((ua >> i) & 1);
        if (r >= ub) { r -= ub; q |= (1ULL << i); }
    }
    return neg ? -(i64)q : (i64)q;
}

i64 __moddi3(i64 a, i64 b) {
    if (b == 0) return 0;
    i64 q = __divdi3(a, b);
    return a - q * b;
}

u64 __udivdi3(u64 a, u64 b) {
    if (b == 0) return 0;
    u64 q = 0, r = 0;
    for (int i = 63; i >= 0; i--) {
        r = (r << 1) | ((a >> i) & 1);
        if (r >= b) { r -= b; q |= (1ULL << i); }
    }
    return q;
}

u64 __umoddi3(u64 a, u64 b) {
    if (b == 0) return 0;
    u64 q = __udivdi3(a, b);
    return a - q * b;
}

/* ================================================================
   4. Serial Port (COM1 = 0x3F8)
   ================================================================ */

#define COM1_PORT 0x3F8

void sad_ll_serial_init(void) {
    sad_ll_port_outb(COM1_PORT + 1, 0x00);  /* Disable interrupts */
    sad_ll_port_outb(COM1_PORT + 3, 0x80);  /* Enable DLAB */
    sad_ll_port_outb(COM1_PORT + 0, 0x03);  /* 38400 baud divisor low */
    sad_ll_port_outb(COM1_PORT + 1, 0x00);  /* Divisor high */
    sad_ll_port_outb(COM1_PORT + 3, 0x03);  /* 8N1 */
    sad_ll_port_outb(COM1_PORT + 2, 0xC7);  /* Enable FIFO */
    sad_ll_port_outb(COM1_PORT + 4, 0x0B);  /* IRQs enabled, RTS/DSR set */
}

void sad_ll_serial_write_char(i64 port, i64 ch) {
    /* Wait for transmit buffer empty */
    while ((sad_ll_port_inb(port + 5) & 0x20) == 0);
    sad_ll_port_outb(port, ch);
}

i64 sad_ll_serial_read_char(i64 port) {
    /* Check if data available */
    if ((sad_ll_port_inb(port + 5) & 0x01) == 0)
        return -1;
    return sad_ll_port_inb(port);
}

/* ================================================================
   5. Interrupt Control
   ================================================================ */

void sad_ll_interrupt_enable(void) {
    __asm__ volatile("sti");
}

void sad_ll_interrupt_disable(void) {
    __asm__ volatile("cli");
}

void sad_ll_interrupt_eoi(i64 irq) {
    /* Send EOI to slave PIC if needed */
    if (irq >= 8) {
        sad_ll_port_outb(0xA0, 0x20);
    }
    /* Always send EOI to master PIC */
    sad_ll_port_outb(0x20, 0x20);
}

/* ================================================================
   6. PIC (8259A)
   ================================================================ */

void sad_ll_pic_remap(i64 offset1, i64 offset2) {
    /* Save masks */
    i64 mask1 = sad_ll_port_inb(0x21);
    i64 mask2 = sad_ll_port_inb(0xA1);

    /* ICW1: start initialization */
    sad_ll_port_outb(0x20, 0x11);
    sad_ll_port_outb(0xA0, 0x11);

    /* ICW2: vector offset */
    sad_ll_port_outb(0x21, offset1);
    sad_ll_port_outb(0xA1, offset2);

    /* ICW3: cascading */
    sad_ll_port_outb(0x21, 0x04);  /* Slave at IRQ2 */
    sad_ll_port_outb(0xA1, 0x02);  /* Cascade identity */

    /* ICW4: 8086 mode */
    sad_ll_port_outb(0x21, 0x01);
    sad_ll_port_outb(0xA1, 0x01);

    /* Restore masks */
    sad_ll_port_outb(0x21, mask1);
    sad_ll_port_outb(0xA1, mask2);
}

void sad_ll_pic_set_mask(i64 irq, i64 enabled) {
    u16 port = (irq < 8) ? 0x21 : 0xA1;
    u8 bit = (u8)(irq & 7);
    u8 val = (u8)sad_ll_port_inb(port);
    if (enabled)
        val &= ~(1 << bit);   /* Clear bit = enable */
    else
        val |= (1 << bit);    /* Set bit = disable */
    sad_ll_port_outb(port, val);
}

/* ================================================================
   7. GDT / IDT
   ================================================================ */

/* Simple flat GDT: null, code, data */
static u64 gdt_entries[3] __attribute__((aligned(16)));

struct gdt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

static struct gdt_ptr gdt_descriptor;

static void gdt_set_entry(int idx, u32 base, u32 limit, u8 access, u8 gran) {
    gdt_entries[idx] =
        ((u64)(limit & 0xFFFF)) |
        ((u64)(base & 0xFFFF) << 16) |
        ((u64)((base >> 16) & 0xFF) << 32) |
        ((u64)access << 40) |
        ((u64)((limit >> 16) & 0x0F) << 48) |
        ((u64)(gran & 0xF0) << 48) |
        ((u64)((base >> 24) & 0xFF) << 56);
}

void sad_ll_gdt_init(void) {
    gdt_set_entry(0, 0, 0, 0, 0);            /* Null */
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF); /* Code: ring0, execute/read, 32-bit */
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF); /* Data: ring0, read/write, 32-bit */
}

void sad_ll_gdt_load(i64 addr) {
    /* addr is pointer to GDT descriptor (limit + base) */
    /* If addr == 0, use our built-in GDT */
    if (addr == 0) {
        gdt_descriptor.limit = sizeof(gdt_entries) - 1;
        gdt_descriptor.base = (u32)gdt_entries;
        addr = (i64)(u32)&gdt_descriptor;
    }
    __asm__ volatile(
        "lgdt (%0)\n"
        "ljmp $0x08, $1f\n"
        "1:\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        : : "r"((u32)addr) : "ax"
    );
}

/* IDT: 256 entries */
struct idt_entry {
    u16 offset_lo;
    u16 selector;
    u8  zero;
    u8  type_attr;
    u16 offset_hi;
} __attribute__((packed));

static struct idt_entry idt_table[256] __attribute__((aligned(16)));

struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

static struct idt_ptr idt_descriptor;

void sad_ll_idt_init(void) {
    for (int i = 0; i < 256; i++) {
        idt_table[i].offset_lo = 0;
        idt_table[i].selector = 0;
        idt_table[i].zero = 0;
        idt_table[i].type_attr = 0;
        idt_table[i].offset_hi = 0;
    }
}

void sad_ll_idt_set_gate(i64 num, i64 handler, i64 selector, i64 flags) {
    int n = (int)num;
    if (n < 0 || n >= 256) return;
    u32 h = (u32)handler;
    idt_table[n].offset_lo = (u16)(h & 0xFFFF);
    idt_table[n].selector  = (u16)selector;
    idt_table[n].zero      = 0;
    idt_table[n].type_attr = (u8)flags;
    idt_table[n].offset_hi = (u16)((h >> 16) & 0xFFFF);
}

void sad_ll_idt_load(i64 addr) {
    if (addr == 0) {
        idt_descriptor.limit = sizeof(idt_table) - 1;
        idt_descriptor.base = (u32)idt_table;
        addr = (i64)(u32)&idt_descriptor;
    }
    __asm__ volatile("lidt (%0)" : : "r"((u32)addr));
}

/* ================================================================
   8. Timer (PIT — 8253/8254)
   ================================================================ */

static volatile u32 timer_ticks = 0;

void sad_ll_timer_init(i64 frequency) {
    if (frequency <= 0) frequency = 100;
    u32 divisor = (u32)(1193180 / frequency);
    sad_ll_port_outb(0x43, 0x36);              /* Channel 0, lo/hi, rate gen */
    sad_ll_port_outb(0x40, divisor & 0xFF);    /* Low byte */
    sad_ll_port_outb(0x40, (divisor >> 8) & 0xFF); /* High byte */
    timer_ticks = 0;
}

i64 sad_ll_timer_get_ticks(void) {
    return (i64)timer_ticks;
}

/* Called from timer IRQ handler to increment tick count */
void sad_ll_timer_tick(void) {
    timer_ticks++;
}

/* ================================================================
   9. Paging (CR3, INVLPG)
   ================================================================ */

i64 sad_ll_read_cr3(void) {
    u32 val;
    __asm__ volatile("mov %%cr3, %0" : "=r"(val));
    return (i64)val;
}

void sad_ll_write_cr3(i64 val) {
    __asm__ volatile("mov %0, %%cr3" : : "r"((u32)val) : "memory");
}

void sad_ll_invlpg(i64 addr) {
    __asm__ volatile("invlpg (%0)" : : "r"((u32)addr) : "memory");
}

/* ---- CR0 (paging enable, write protect) ---- */
i64 sad_ll_read_cr0(void) {
    u32 val;
    __asm__ volatile("mov %%cr0, %0" : "=r"(val));
    return (i64)val;
}
void sad_ll_write_cr0(i64 val) {
    __asm__ volatile("mov %0, %%cr0" : : "r"((u32)val) : "memory");
}

/* ---- CR2 (page fault linear address) ---- */
i64 sad_ll_read_cr2(void) {
    u32 val;
    __asm__ volatile("mov %%cr2, %0" : "=r"(val));
    return (i64)val;
}

/* ---- CR4 (PAE, PSE, SMEP, etc.) ---- */
i64 sad_ll_read_cr4(void) {
    u32 val;
    __asm__ volatile("mov %%cr4, %0" : "=r"(val));
    return (i64)val;
}
void sad_ll_write_cr4(i64 val) {
    __asm__ volatile("mov %0, %%cr4" : : "r"((u32)val) : "memory");
}

/* ---- Flush entire TLB (reload CR3) ---- */
void sad_ll_flush_tlb(void) {
    u32 val;
    __asm__ volatile("mov %%cr3, %0\n\tmov %0, %%cr3" : "=r"(val) : : "memory");
}

/* ---- Get ESP (current stack pointer) ---- */
i64 sad_ll_get_esp(void) {
    u32 val;
    __asm__ volatile("mov %%esp, %0" : "=r"(val));
    return (i64)val;
}

/* ---- Get EFLAGS ---- */
i64 sad_ll_get_eflags(void) {
    u32 val;
    __asm__ volatile("pushf\n\tpop %0" : "=r"(val));
    return (i64)val;
}

/* ---- LGDT / LIDT ---- */
void sad_ll_load_gdt(i64 gdt_desc_addr) {
    __asm__ volatile("lgdt (%0)" : : "r"((u32)gdt_desc_addr) : "memory");
}
void sad_ll_load_idt(i64 idt_desc_addr) {
    __asm__ volatile("lidt (%0)" : : "r"((u32)idt_desc_addr) : "memory");
}

/* ---- CPUID ---- */
i64 sad_ll_cpuid_eax(i64 func) {
    u32 eax, ebx, ecx, edx;
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"((u32)func));
    return (i64)eax;
}
i64 sad_ll_cpuid_edx(i64 func) {
    u32 eax, ebx, ecx, edx;
    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"((u32)func));
    return (i64)edx;
}

/* ================================================================
   10. Page Allocator (simple bitmap, 4KB pages)
   ================================================================ */

#define PAGE_SIZE      4096
#define MAX_PAGES      4096  /* 16MB addressable (4096 * 4KB) */
#define BITMAP_SIZE    (MAX_PAGES / 8)   /* 512 bytes */

/* Memory pool starts at 4MB (above kernel) */
#define POOL_START     0x00400000

static u8 page_bitmap[BITMAP_SIZE];
static int page_allocator_initialized = 0;

static void page_alloc_init(void) {
    for (int i = 0; i < BITMAP_SIZE; i++) page_bitmap[i] = 0;
    page_allocator_initialized = 1;
}

static int page_bitmap_test(int page) {
    return (page_bitmap[page / 8] >> (page % 8)) & 1;
}

static void page_bitmap_set(int page) {
    page_bitmap[page / 8] |= (u8)(1 << (page % 8));
}

static void page_bitmap_clear(int page) {
    page_bitmap[page / 8] &= (u8)~(1 << (page % 8));
}

i64 sad_ll_page_alloc(void) {
    if (!page_allocator_initialized) page_alloc_init();
    for (int i = 0; i < MAX_PAGES; i++) {
        if (!page_bitmap_test(i)) {
            page_bitmap_set(i);
            u32 addr = POOL_START + (u32)i * PAGE_SIZE;
            /* Zero the page */
            u8 *p = (u8 *)addr;
            for (int j = 0; j < PAGE_SIZE; j++) p[j] = 0;
            return (i64)addr;
        }
    }
    return 0; /* Out of memory */
}

void sad_ll_page_free(i64 addr) {
    if (addr < POOL_START) return;
    u32 page = ((u32)addr - POOL_START) / PAGE_SIZE;
    if (page < MAX_PAGES)
        page_bitmap_clear((int)page);
}

i64 sad_ll_pages_alloc(i64 count) {
    if (!page_allocator_initialized) page_alloc_init();
    int n = (int)count;
    if (n <= 0) return 0;

    /* Find n contiguous free pages */
    for (int i = 0; i <= MAX_PAGES - n; i++) {
        int found = 1;
        for (int j = 0; j < n; j++) {
            if (page_bitmap_test(i + j)) {
                found = 0;
                i += j; /* Skip ahead */
                break;
            }
        }
        if (found) {
            for (int j = 0; j < n; j++)
                page_bitmap_set(i + j);
            u32 addr = POOL_START + (u32)i * PAGE_SIZE;
            /* Zero the pages */
            u8 *p = (u8 *)addr;
            for (int j = 0; j < n * PAGE_SIZE; j++) p[j] = 0;
            return (i64)addr;
        }
    }
    return 0;
}

void sad_ll_pages_free(i64 addr, i64 count) {
    if (addr < POOL_START) return;
    int start = (int)(((u32)addr - POOL_START) / PAGE_SIZE);
    int n = (int)count;
    for (int j = 0; j < n; j++) {
        if (start + j < MAX_PAGES)
            page_bitmap_clear(start + j);
    }
}

/* ================================================================
   11. Heap (kmalloc/kfree — simple bump allocator)
   ================================================================ */

/* Heap occupies a dedicated region: 8MB - 12MB */
#define HEAP_START  0x00800000
#define HEAP_SIZE   (4 * 1024 * 1024)  /* 4MB */

static u32 heap_current = HEAP_START;

i64 sad_ll_kmalloc(i64 size) {
    if (size <= 0) return 0;
    /* Align to 8 bytes */
    u32 aligned = ((u32)size + 7) & ~7;
    if (heap_current + aligned > HEAP_START + HEAP_SIZE)
        return 0; /* Out of heap */
    u32 addr = heap_current;
    heap_current += aligned;
    /* Zero the memory */
    u8 *p = (u8 *)addr;
    for (u32 i = 0; i < aligned; i++) p[i] = 0;
    return (i64)addr;
}

void sad_ll_kfree(i64 addr) {
    /* Bump allocator cannot free individual blocks */
    /* This is a no-op stub — the ص kernel implements its own slab allocator */
    (void)addr;
}

/* ================================================================
   12. Context Switch (task switching stubs)
   ================================================================ */

void sad_ll_context_switch(i64 old_stack_ptr, i64 new_stack_ptr) {
    /* Minimal context switch: save ESP to old, load from new */
    /* In a real implementation this would save/restore all registers */
    __asm__ volatile(
        "mov %%esp, (%0)\n"
        "mov (%1), %%esp\n"
        : : "r"((u32)old_stack_ptr), "r"((u32)new_stack_ptr)
        : "memory"
    );
}

i64 sad_ll_stack_init(i64 stack_top, i64 entry_point) {
    /* Set up initial stack frame for a new task:
       Push entry point as return address at top of stack */
    u32 *sp = (u32 *)(u32)stack_top;
    /* Simulate pusha + iret frame */
    *(--sp) = (u32)entry_point;  /* EIP (return address) */
    *(--sp) = 0;  /* EAX */
    *(--sp) = 0;  /* ECX */
    *(--sp) = 0;  /* EDX */
    *(--sp) = 0;  /* EBX */
    *(--sp) = 0;  /* ESP (placeholder) */
    *(--sp) = 0;  /* EBP */
    *(--sp) = 0;  /* ESI */
    *(--sp) = 0;  /* EDI */
    *(--sp) = 0x200; /* EFLAGS (IF=1) */
    return (i64)(u32)sp;
}

/* ================================================================
   13. Framebuffer (Bochs VGA Adapter stubs)
   ================================================================ */

static u32 *fb_addr = 0;
static u32 fb_width = 0;
static u32 fb_height = 0;
static u32 fb_pitch = 0;

void sad_ll_fb_init(i64 addr, i64 width, i64 height, i64 pitch) {
    fb_addr = (u32 *)(u32)addr;
    fb_width = (u32)width;
    fb_height = (u32)height;
    fb_pitch = (u32)pitch;
}

void sad_ll_fb_set_pixel(i64 x, i64 y, i64 color) {
    if (!fb_addr) return;
    if ((u32)x >= fb_width || (u32)y >= fb_height) return;
    u32 offset = (u32)y * (fb_pitch / 4) + (u32)x;
    fb_addr[offset] = (u32)color;
}

void sad_ll_fb_fill_rect(i64 x, i64 y, i64 w, i64 h, i64 color) {
    if (!fb_addr) return;
    for (u32 dy = 0; dy < (u32)h; dy++) {
        for (u32 dx = 0; dx < (u32)w; dx++) {
            u32 px = (u32)x + dx;
            u32 py = (u32)y + dy;
            if (px < fb_width && py < fb_height) {
                u32 offset = py * (fb_pitch / 4) + px;
                fb_addr[offset] = (u32)color;
            }
        }
    }
}

void sad_ll_fb_clear(i64 color) {
    if (!fb_addr) return;
    for (u32 y = 0; y < fb_height; y++)
        for (u32 x = 0; x < fb_width; x++)
            fb_addr[y * (fb_pitch / 4) + x] = (u32)color;
}

/* ================================================================
   14. Filesystem Support (aliases for kernel modules)
   ================================================================ */

/* Forward: serial_putc for serial output */
static void _serial_putc(char c) {
    while ((sad_ll_port_inb(COM1_PORT + 5) & 0x20) == 0);
    sad_ll_port_outb(COM1_PORT, (i64)c);
}

static void _serial_puts(const char *s) {
    while (*s) { _serial_putc(*s); s++; }
}

/* kernel_print_string — print null-terminated string via serial + VGA */
void kernel_print_string(i64 str_addr) {
    const char *s = (const char *)(u32)str_addr;
    if (!s) return;
    _serial_puts(s);
    /* Also print to VGA if available */
    extern void vga_puts(const char *s);
    vga_puts(s);
}

/* kernel_print_number — print integer; if 10, print newline */
void kernel_print_number(i64 num) {
    if (num == 10) {
        _serial_putc('\r');
        _serial_putc('\n');
        extern void vga_putc(char c);
        vga_putc('\n');
        return;
    }
    /* Print integer */
    char buf[24];
    int neg = 0;
    u64 val;
    if (num < 0) { neg = 1; val = (u64)(-num); } else { val = (u64)num; }
    int i = 0;
    if (val == 0) { buf[i++] = '0'; }
    else { while (val > 0) { buf[i++] = '0' + (val % 10); val /= 10; } }
    if (neg) buf[i++] = '-';
    /* Reverse and print */
    for (int j = i - 1; j >= 0; j--) _serial_putc(buf[j]);
    extern void vga_putc(char c);
    for (int j = i - 1; j >= 0; j--) vga_putc(buf[j]);
}

/* serial_put_int — print integer to serial only */
void serial_put_int(i64 num) {
    char buf[24];
    int neg = 0;
    u64 val;
    if (num < 0) { neg = 1; val = (u64)(-num); } else { val = (u64)num; }
    int i = 0;
    if (val == 0) { buf[i++] = '0'; }
    else { while (val > 0) { buf[i++] = '0' + (val % 10); val /= 10; } }
    if (neg) buf[i++] = '-';
    for (int j = i - 1; j >= 0; j--) _serial_putc(buf[j]);
}

/* serial_put_hex — print hex to serial only */
void serial_put_hex(i64 num) {
    const char *hex = "0123456789ABCDEF";
    _serial_puts("0x");
    u32 val = (u32)num;
    for (int i = 28; i >= 0; i -= 4) {
        _serial_putc(hex[(val >> i) & 0xF]);
    }
}

/* disk_read_sector — read a 512-byte sector via ATA PIO (LBA28) */
void disk_read_sector(i64 drive, i64 sector, i64 buffer) {
    u16 base = (drive == 0) ? 0x1F0 : 0x170;
    u32 lba = (u32)sector;
    u8 *buf = (u8 *)(u32)buffer;

    /* Wait for drive ready */
    while (sad_ll_port_inb(base + 7) & 0x80);

    /* Send LBA28 parameters */
    sad_ll_port_outb(base + 2, 1);                   /* Sector count = 1 */
    sad_ll_port_outb(base + 3, lba & 0xFF);           /* LBA low */
    sad_ll_port_outb(base + 4, (lba >> 8) & 0xFF);    /* LBA mid */
    sad_ll_port_outb(base + 5, (lba >> 16) & 0xFF);   /* LBA high */
    sad_ll_port_outb(base + 6, 0xE0 | ((lba >> 24) & 0x0F)); /* LBA bits 24-27 + LBA mode */
    sad_ll_port_outb(base + 7, 0x20);                 /* READ SECTORS command */

    /* Wait for data ready */
    while (!(sad_ll_port_inb(base + 7) & 0x08));

    /* Read 256 words (512 bytes) */
    for (int i = 0; i < 256; i++) {
        u16 data = (u16)sad_ll_port_inw(base);
        buf[i * 2] = (u8)(data & 0xFF);
        buf[i * 2 + 1] = (u8)(data >> 8);
    }
}

/* kernel_allocate — alias for kmalloc */
i64 kernel_allocate(i64 size) {
    return sad_ll_kmalloc(size);
}

/* kernel_free — alias for kfree */
void kernel_free(i64 addr) {
    sad_ll_kfree(addr);
}
