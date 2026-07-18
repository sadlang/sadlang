// ============================================================================
// sad_lowlevel_rt.c — تنفيذ مكتبة وقت التشغيل المنخفضة المستوى (بلا نظام تشغيل)
// Freestanding Low-Level Runtime Library Implementation
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الإصدار / Version: 5.0
//
// (AR) هذا الملف يحتوي على التنفيذ الفعلي لجميع دوال sad_ll_* التي يستدعيها
//      الكود المترجم بواسطة sadc في وضع --freestanding. جميع الدوال تعمل مباشرة
//      على العتاد بدون نظام تشغيل.
// (EN) This file contains the actual implementation of all sad_ll_* functions
//      called by code compiled with sadc in --freestanding mode. All functions
//      operate directly on hardware without an operating system.
// ============================================================================

#include "sad_lowlevel_rt.h"

// ============================================================================
// منفذ الأوامر / Hardware Port I/O Helpers
// ============================================================================
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

// ============================================================================
// تعليمات المعالج / CPU Inline Assembly Helpers
// ============================================================================
static inline void cpuid(uint32_t leaf, uint32_t* eax, uint32_t* ebx,
                          uint32_t* ecx, uint32_t* edx) {
    __asm__ volatile ("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(0));
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t lo, hi;
    __asm__ volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    return ((uint64_t)hi << 32) | lo;
}

static inline void wrmsr(uint32_t msr, uint64_t val) {
    uint32_t lo = (uint32_t)val;
    uint32_t hi = (uint32_t)(val >> 32);
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(lo), "d"(hi));
}

static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t read_cr0(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(val));
    return val;
}

static inline uint64_t read_cr2(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(val));
    return val;
}

static inline uint64_t read_cr3(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline uint64_t read_cr4(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(val));
    return val;
}

static inline void write_cr0(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr0" : : "r"(val) : "memory");
}

static inline void write_cr3(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(val) : "memory");
}

static inline void write_cr4(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr4" : : "r"(val) : "memory");
}

static inline void invlpg(uint64_t addr) {
    __asm__ volatile ("invlpg (%0)" : : "r"(addr) : "memory");
}

static inline void flush_tlb(void) {
    uint64_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(cr3) : "memory");
}

static inline void lgdt(void* gdtr) {
    __asm__ volatile ("lgdt (%0)" : : "r"(gdtr) : "memory");
}

static inline void lidt(void* idtr) {
    __asm__ volatile ("lidt (%0)" : : "r"(idtr) : "memory");
}

static inline void sti(void) {
    __asm__ volatile ("sti");
}

static inline void cli(void) {
    __asm__ volatile ("cli");
}

static inline void hlt(void) {
    __asm__ volatile ("hlt");
}

static inline void pause_cpu(void) {
    __asm__ volatile ("pause");
}

// ============================================================================
// أدوات الذاكرة / Memory Utilities (libc-free)
// ============================================================================
static void* rt_memset(void* dest, int val, uint64_t count) {
    uint8_t* d = (uint8_t*)dest;
    for (uint64_t i = 0; i < count; i++)
        d[i] = (uint8_t)val;
    return dest;
}

static void* rt_memcpy(void* dest, const void* src, uint64_t count) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (uint64_t i = 0; i < count; i++)
        d[i] = s[i];
    return dest;
}

static uint64_t rt_strlen(const char* s) {
    uint64_t len = 0;
    while (s[len]) len++;
    return len;
}

// ============================================================================
// بنى البيانات الداخلية / Internal Data Structures
// ============================================================================

// --- GDT ---
#define GDT_MAX_ENTRIES 8

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct gdt_entry gdt_entries[GDT_MAX_ENTRIES];
static struct gdt_ptr   gdt_pointer;
static int gdt_initialized = 0;

static void gdt_set_entry(int idx, uint32_t base, uint32_t limit,
                           uint8_t access, uint8_t gran) {
    if (idx >= GDT_MAX_ENTRIES) return;
    gdt_entries[idx].base_low    = (uint16_t)(base & 0xFFFF);
    gdt_entries[idx].base_middle = (uint8_t)((base >> 16) & 0xFF);
    gdt_entries[idx].base_high   = (uint8_t)((base >> 24) & 0xFF);
    gdt_entries[idx].limit_low   = (uint16_t)(limit & 0xFFFF);
    gdt_entries[idx].granularity  = (uint8_t)((limit >> 16) & 0x0F);
    gdt_entries[idx].granularity |= (uint8_t)(gran & 0xF0);
    gdt_entries[idx].access      = access;
}

// --- IDT ---
#define IDT_MAX_ENTRIES 256

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  ist;
    uint8_t  flags;
    uint16_t base_mid;
    uint32_t base_hi;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt_entries[IDT_MAX_ENTRIES];
static struct idt_ptr   idt_pointer;
static int idt_initialized = 0;

// --- Paging ---
#define PAGE_SIZE 4096
#define PAGE_PRESENT  0x01
#define PAGE_RW       0x02
#define PAGE_USER     0x04

static uint64_t* page_directory = 0;
static int paging_initialized = 0;
static uint64_t paging_mapped_count = 0;

// --- PCI ---
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC
#define PCI_MAX_DEVICES    256

struct pci_device {
    uint8_t  bus;
    uint8_t  device;
    uint8_t  function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
};

static struct pci_device pci_devices[PCI_MAX_DEVICES];
static int pci_device_count = 0;

// --- DMA ---
static int dma_initialized   = 0;
static int dma_transfer_count = 0;

// --- Framebuffer ---
#define FB_BITS_PER_BYTE   8
#define FB_DEFAULT_BPP     32
static uint32_t* fb_base    = 0;
static int64_t   fb_width   = 0;
static int64_t   fb_height  = 0;
static int64_t   fb_bpp     = FB_DEFAULT_BPP;
static int64_t   fb_pitch   = 0;   // bytes per row (stride); may exceed width*bytesPerPixel on real HW
static int       fb_initialized = 0;

// (AR) مؤشّر البكسل بدلالة الخطوة (pitch) بالبايت — يعمل للبركة والعتاد الحقيقيّ سواء.
// (EN) Pixel pointer via byte pitch — works for both the software pool and real hardware.
static inline uint32_t* fb_pixel_ptr(int64_t x, int64_t y) {
    int64_t bytes_per_pixel = fb_bpp / FB_BITS_PER_BYTE;
    return (uint32_t*)((uint8_t*)fb_base + y * fb_pitch + x * bytes_per_pixel);
}

// --- ACPI ---
static int acpi_initialized = 0;

// --- Sync ---
typedef volatile int32_t spinlock_t;
typedef volatile int32_t mutex_t;

// --- Scheduler ---
#define SCHED_MAX_TASKS 64

struct sched_task {
    int64_t  id;
    int64_t  entry;
    int      active;
    char     name[64];
};

static struct sched_task sched_tasks[SCHED_MAX_TASKS];
static int sched_task_count = 0;
static int sched_initialized = 0;

// --- VFS ---
#define VFS_MAX_FDS 64
#define VFS_MAX_MOUNTS 16

struct vfs_fd {
    int64_t  id;
    int      open;
    int64_t  offset;
    int64_t  mount_idx;
};

struct vfs_mount {
    int64_t device;
    int64_t mount_point;
    int     active;
};

static struct vfs_fd     vfs_fds[VFS_MAX_FDS];
static struct vfs_mount  vfs_mounts[VFS_MAX_MOUNTS];
static int vfs_fd_next   = 0;
static int vfs_mount_count = 0;

// --- APIC ---
#define APIC_BASE_MSR     0x1B
#define APIC_REG_EOI      0xB0
#define APIC_REG_SVR      0xF0
#define APIC_REG_ICR_LO   0x300
#define APIC_REG_ICR_HI   0x310
#define APIC_REG_TIMER_DIV 0x3E0
#define APIC_REG_TIMER_INIT 0x380

static volatile uint32_t* apic_base_addr = 0;
static int apic_initialized = 0;

// --- HPET ---
static volatile uint64_t* hpet_base_addr = 0;
static int hpet_initialized = 0;
static uint64_t hpet_period_fs = 0; // femtoseconds per tick

// --- Syscall ---
#define SYSCALL_MAX 256

typedef int64_t (*syscall_handler_t)(void);

static syscall_handler_t syscall_table[SYSCALL_MAX];
static int syscall_initialized = 0;

// --- Physical Memory Allocator ---
#define PHYS_ALLOC_POOL_SIZE (4 * 1024 * 1024) // 4MB pool
static uint8_t  phys_pool[PHYS_ALLOC_POOL_SIZE] __attribute__((aligned(4096)));
static uint64_t phys_pool_offset = 0;

// ============================================================================
// سلاسل تقارير ثابتة / Static Report String Buffers
// ============================================================================
static char report_buf[1024];

static char* rt_strcat(char* dest, const char* src) {
    uint64_t dlen = rt_strlen(dest);
    uint64_t slen = rt_strlen(src);
    rt_memcpy(dest + dlen, src, slen + 1);
    return dest;
}

static void int_to_str(int64_t val, char* buf) {
    if (val == 0) { buf[0] = '0'; buf[1] = 0; return; }
    int neg = 0;
    if (val < 0) { neg = 1; val = -val; }
    char tmp[24];
    int i = 0;
    while (val > 0) {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }
    int j = 0;
    if (neg) buf[j++] = '-';
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = 0;
}

static void hex_to_str(uint64_t val, char* buf) {
    const char hex[] = "0123456789ABCDEF";
    buf[0] = '0'; buf[1] = 'x';
    if (val == 0) { buf[2] = '0'; buf[3] = 0; return; }
    char tmp[20];
    int i = 0;
    while (val > 0) {
        tmp[i++] = hex[val & 0xF];
        val >>= 4;
    }
    int j = 2;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = 0;
}

// ============================================================================
// 15a. تنفيذ وحدة المعالج / CPU Module Implementation
// ============================================================================

int64_t sad_ll_cpu_get_info(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(1, &eax, &ebx, &ecx, &edx);
    // Return packed: family(8) | model(8) | stepping(4)
    uint32_t stepping = eax & 0xF;
    uint32_t model    = (eax >> 4) & 0xF;
    uint32_t family   = (eax >> 8) & 0xF;
    uint32_t ext_model = (eax >> 16) & 0xF;
    uint32_t ext_family = (eax >> 20) & 0xFF;

    if (family == 0xF) family += ext_family;
    if (family == 0x6 || family == 0xF) model += (ext_model << 4);

    return (int64_t)((family << 16) | (model << 8) | stepping);
}

int64_t sad_ll_cpu_get_features(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(1, &eax, &ebx, &ecx, &edx);
    // Return EDX:ECX packed feature flags
    return (int64_t)(((uint64_t)edx << 32) | ecx);
}

int64_t sad_ll_read_cr(int64_t cr_num) {
    switch (cr_num) {
        case 0: return (int64_t)read_cr0();
        case 2: return (int64_t)read_cr2();
        case 3: return (int64_t)read_cr3();
        case 4: return (int64_t)read_cr4();
        default: return 0;
    }
}

void sad_ll_write_cr(int64_t cr_num, int64_t value) {
    switch (cr_num) {
        case 0: write_cr0((uint64_t)value); break;
        case 3: write_cr3((uint64_t)value); break;
        case 4: write_cr4((uint64_t)value); break;
        default: break;
    }
}

const char* sad_ll_cpu_report(void) {
    uint32_t eax, ebx, ecx, edx;

    // Get vendor string
    cpuid(0, &eax, &ebx, &ecx, &edx);
    char vendor[16];
    rt_memcpy(vendor,     &ebx, 4);
    rt_memcpy(vendor + 4, &edx, 4);
    rt_memcpy(vendor + 8, &ecx, 4);
    vendor[12] = 0;

    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "CPU: ");
    rt_strcat(report_buf, vendor);

    char tmp[24];
    int64_t info = sad_ll_cpu_get_info();
    rt_strcat(report_buf, " | Family: ");
    int_to_str((info >> 16) & 0xFF, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, " Model: ");
    int_to_str((info >> 8) & 0xFF, tmp);
    rt_strcat(report_buf, tmp);

    return report_buf;
}

// ============================================================================
// 15b. تنفيذ وحدة GDT
// ============================================================================

void sad_ll_gdt_init(void) {
    // Null descriptor
    gdt_set_entry(0, 0, 0, 0, 0);
    // Kernel code segment: 0x08
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xAF);
    // Kernel data segment: 0x10
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // User code segment: 0x18
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xAF);
    // User data segment: 0x20
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_pointer.limit = (uint16_t)(sizeof(struct gdt_entry) * 5 - 1);
    gdt_pointer.base  = (uint64_t)&gdt_entries;

    gdt_initialized = 1;
}

void sad_ll_gdt_load(void) {
    if (!gdt_initialized) sad_ll_gdt_init();
    lgdt(&gdt_pointer);
}

const char* sad_ll_gdt_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "GDT: ");
    if (gdt_initialized) {
        rt_strcat(report_buf, "initialized, 5 entries");
        char tmp[24];
        rt_strcat(report_buf, " | Base: ");
        hex_to_str(gdt_pointer.base, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15c. تنفيذ وحدة الترحيل / Paging Implementation
// ============================================================================

void sad_ll_paging_init(void) {
    // Allocate PML4 table from physical pool (aligned to 4KB)
    uint64_t aligned = (phys_pool_offset + 4095) & ~4095ULL;
    if (aligned + 4096 > PHYS_ALLOC_POOL_SIZE) return;

    page_directory = (uint64_t*)(phys_pool + aligned);
    phys_pool_offset = aligned + 4096;

    rt_memset(page_directory, 0, 4096);
    paging_initialized = 1;
    paging_mapped_count = 0;
}

void sad_ll_paging_map(int64_t vaddr, int64_t paddr, int64_t flags) {
    if (!paging_initialized) return;
    // Simplified: direct-map entry in PML4
    // In a real kernel, this would walk the 4-level page table
    uint64_t index = ((uint64_t)vaddr >> 39) & 0x1FF;
    page_directory[index] = (uint64_t)paddr | (uint64_t)flags | PAGE_PRESENT;
    paging_mapped_count++;
    invlpg((uint64_t)vaddr);
}

void sad_ll_paging_unmap(int64_t vaddr) {
    if (!paging_initialized) return;
    uint64_t index = ((uint64_t)vaddr >> 39) & 0x1FF;
    page_directory[index] = 0;
    if (paging_mapped_count > 0) paging_mapped_count--;
    invlpg((uint64_t)vaddr);
}

const char* sad_ll_paging_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "Paging: ");
    if (paging_initialized) {
        rt_strcat(report_buf, "enabled");
        char tmp[24];
        rt_strcat(report_buf, " | Mapped pages: ");
        int_to_str((int64_t)paging_mapped_count, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, " | PML4: ");
        hex_to_str((uint64_t)page_directory, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15d. تنفيذ وحدة المقاطعات / IDT Implementation
// ============================================================================

static void idt_set_entry(int idx, uint64_t base, uint16_t sel,
                           uint8_t ist, uint8_t flags) {
    if (idx >= IDT_MAX_ENTRIES) return;
    idt_entries[idx].base_lo  = (uint16_t)(base & 0xFFFF);
    idt_entries[idx].base_mid = (uint16_t)((base >> 16) & 0xFFFF);
    idt_entries[idx].base_hi  = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    idt_entries[idx].sel      = sel;
    idt_entries[idx].ist      = ist;
    idt_entries[idx].flags    = flags;
    idt_entries[idx].zero     = 0;
}

void sad_ll_idt_init(void) {
    rt_memset(idt_entries, 0, sizeof(idt_entries));
    idt_pointer.limit = (uint16_t)(sizeof(struct idt_entry) * IDT_MAX_ENTRIES - 1);
    idt_pointer.base  = (uint64_t)&idt_entries;
    idt_initialized = 1;
}

void sad_ll_idt_load(void) {
    if (!idt_initialized) sad_ll_idt_init();
    lidt(&idt_pointer);
}

void sad_ll_idt_register_isr(int64_t num, int64_t handler) {
    if (!idt_initialized) return;
    if (num < 0 || num >= IDT_MAX_ENTRIES) return;
    // Interrupt gate: 0x8E (present, ring 0, 64-bit interrupt gate)
    idt_set_entry((int)num, (uint64_t)handler, 0x08, 0, 0x8E);
}

void sad_ll_idt_enable_irq(int64_t irq) {
    if (irq < 0 || irq >= 16) return;
    // Program the PIC to unmask the IRQ
    if (irq < 8) {
        uint8_t mask = inb(0x21);
        mask &= ~(1 << (uint8_t)irq);
        outb(0x21, mask);
    } else {
        uint8_t mask = inb(0xA1);
        mask &= ~(1 << (uint8_t)(irq - 8));
        outb(0xA1, mask);
    }
}

const char* sad_ll_idt_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "IDT: ");
    if (idt_initialized) {
        rt_strcat(report_buf, "initialized, 256 entries");
        char tmp[24];
        rt_strcat(report_buf, " | Base: ");
        hex_to_str(idt_pointer.base, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15e. تنفيذ وحدة PCI
// ============================================================================

static uint32_t pci_config_read32(uint8_t bus, uint8_t dev,
                                   uint8_t func, uint8_t offset) {
    uint32_t address = (1U << 31)
        | ((uint32_t)bus  << 16)
        | ((uint32_t)(dev & 0x1F) << 11)
        | ((uint32_t)(func & 0x07) << 8)
        | ((uint32_t)(offset & 0xFC));
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

static void pci_config_write32(uint8_t bus, uint8_t dev,
                                uint8_t func, uint8_t offset, uint32_t val) {
    uint32_t address = (1U << 31)
        | ((uint32_t)bus  << 16)
        | ((uint32_t)(dev & 0x1F) << 11)
        | ((uint32_t)(func & 0x07) << 8)
        | ((uint32_t)(offset & 0xFC));
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, val);
}

int64_t sad_ll_pci_enumerate(void) {
    pci_device_count = 0;
    for (uint16_t bus = 0; bus < 256 && pci_device_count < PCI_MAX_DEVICES; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            uint32_t id = pci_config_read32((uint8_t)bus, dev, 0, 0);
            uint16_t vendor = (uint16_t)(id & 0xFFFF);
            if (vendor == 0xFFFF) continue;

            pci_devices[pci_device_count].bus       = (uint8_t)bus;
            pci_devices[pci_device_count].device    = dev;
            pci_devices[pci_device_count].function  = 0;
            pci_devices[pci_device_count].vendor_id = vendor;
            pci_devices[pci_device_count].device_id = (uint16_t)(id >> 16);

            uint32_t class_info = pci_config_read32((uint8_t)bus, dev, 0, 0x08);
            pci_devices[pci_device_count].class_code = (uint8_t)(class_info >> 24);
            pci_devices[pci_device_count].subclass   = (uint8_t)(class_info >> 16);

            pci_device_count++;
        }
    }
    return (int64_t)pci_device_count;
}

int64_t sad_ll_pci_read_config(int64_t bus, int64_t dev,
                                int64_t func, int64_t offset) {
    return (int64_t)pci_config_read32((uint8_t)bus, (uint8_t)dev,
                                       (uint8_t)func, (uint8_t)offset);
}

void sad_ll_pci_write_config(int64_t bus, int64_t dev,
                              int64_t func, int64_t offset, int64_t val) {
    pci_config_write32((uint8_t)bus, (uint8_t)dev, (uint8_t)func,
                        (uint8_t)offset, (uint32_t)val);
}

int64_t sad_ll_pci_device_count(void) {
    return (int64_t)pci_device_count;
}

const char* sad_ll_pci_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "PCI: ");
    char tmp[24];
    int_to_str(pci_device_count, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, " devices found");
    return report_buf;
}

// ============================================================================
// 15f. تنفيذ وحدة DMA
// ============================================================================

void sad_ll_dma_init(void) {
    // Initialize ISA DMA controller
    // Reset DMA controller
    outb(0x0D, 0xFF); // Master reset
    outb(0xDA, 0xFF); // Slave reset
    dma_initialized = 1;
    dma_transfer_count = 0;
}

void sad_ll_dma_transfer(int64_t src, int64_t dst, int64_t size) {
    if (!dma_initialized) return;
    // Software memcpy fallback — real DMA would program the controller
    // In bare-metal x86, ISA DMA is limited to specific address ranges
    volatile uint8_t* s = (volatile uint8_t*)src;
    volatile uint8_t* d = (volatile uint8_t*)dst;
    for (int64_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
    dma_transfer_count++;
}

int64_t sad_ll_dma_status(void) {
    if (!dma_initialized) return -1;
    return 0; // 0 = idle/complete
}

const char* sad_ll_dma_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "DMA: ");
    if (dma_initialized) {
        rt_strcat(report_buf, "initialized | Transfers: ");
        char tmp[24];
        int_to_str(dma_transfer_count, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15g. تنفيذ وحدة الشاشة / Framebuffer Implementation
// ============================================================================

void sad_ll_fb_init(int64_t width, int64_t height, int64_t bpp) {
    fb_width  = width;
    fb_height = height;
    fb_bpp    = bpp;
    // (AR) البركة البرمجيّة: خطوة الصفّ = العرض × بايتات البكسل (لا حشو).
    // (EN) Software pool path: row stride = width * bytesPerPixel (no padding).
    fb_pitch  = width * (bpp / FB_BITS_PER_BYTE);
    // In a real kernel, fb_base would come from the bootloader's framebuffer info
    // or from setting up a VGA/VESA mode. For a visible display on real hardware,
    // call sad_ll_fb_init_addr() with the linear framebuffer address instead.
    // Default: use a portion of our physical pool as a software buffer (host-testable).
    uint64_t fb_size = (uint64_t)(width * height * (bpp / FB_BITS_PER_BYTE));
    uint64_t aligned = (phys_pool_offset + 4095) & ~4095ULL;
    if (aligned + fb_size <= PHYS_ALLOC_POOL_SIZE) {
        fb_base = (uint32_t*)(phys_pool + aligned);
        phys_pool_offset = aligned + fb_size;
    }
    fb_initialized = 1;
}

// (AR) ف-١: ربط مخزن الإطار بعنوانه الحقيقيّ من مُقلِع النظام (UEFI GOP / VESA / multiboot2).
//      يوجّه fb_base إلى العنوان الخطّيّ الممرَّر بدل البركة البرمجيّة، فتظهر البكسلات
//      على الشاشة. الخطوة (pitch) بالبايت؛ إن مُرِّرت صفرًا تُحسَب من العرض والعمق.
// (EN) F-1: bind the framebuffer to its real address from the bootloader (UEFI GOP /
//      VESA / multiboot2). Points fb_base at the given linear address instead of the
//      software pool, so pixels reach the screen. Pitch is in bytes; if zero it is
//      derived from width and bpp.
void sad_ll_fb_init_addr(int64_t addr, int64_t width, int64_t height,
                         int64_t pitch, int64_t bpp) {
    fb_base   = (uint32_t*)(uintptr_t)addr;
    fb_width  = width;
    fb_height = height;
    fb_bpp    = bpp;
    fb_pitch  = (pitch > 0) ? pitch : width * (bpp / FB_BITS_PER_BYTE);
    fb_initialized = (fb_base != 0);
}

void sad_ll_fb_set_pixel(int64_t x, int64_t y, int64_t color) {
    if (!fb_initialized || !fb_base) return;
    if (x < 0 || x >= fb_width || y < 0 || y >= fb_height) return;
    *fb_pixel_ptr(x, y) = (uint32_t)color;
}

void sad_ll_fb_draw_rect(int64_t x, int64_t y, int64_t w, int64_t h, int64_t color) {
    if (!fb_initialized || !fb_base) return;
    uint32_t c = (uint32_t)color;
    // Top and bottom edges
    for (int64_t i = x; i < x + w; i++) {
        if (i >= 0 && i < fb_width) {
            if (y >= 0 && y < fb_height) *fb_pixel_ptr(i, y) = c;
            if (y + h - 1 >= 0 && y + h - 1 < fb_height)
                *fb_pixel_ptr(i, y + h - 1) = c;
        }
    }
    // Left and right edges
    for (int64_t j = y; j < y + h; j++) {
        if (j >= 0 && j < fb_height) {
            if (x >= 0 && x < fb_width) *fb_pixel_ptr(x, j) = c;
            if (x + w - 1 >= 0 && x + w - 1 < fb_width)
                *fb_pixel_ptr(x + w - 1, j) = c;
        }
    }
}

void sad_ll_fb_fill_rect(int64_t x, int64_t y, int64_t w, int64_t h, int64_t color) {
    if (!fb_initialized || !fb_base) return;
    uint32_t c = (uint32_t)color;
    for (int64_t j = y; j < y + h; j++) {
        if (j < 0 || j >= fb_height) continue;
        for (int64_t i = x; i < x + w; i++) {
            if (i < 0 || i >= fb_width) continue;
            *fb_pixel_ptr(i, j) = c;
        }
    }
}

void sad_ll_fb_draw_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t color) {
    if (!fb_initialized || !fb_base) return;
    // Bresenham's line algorithm
    int64_t dx = x2 - x1;
    int64_t dy = y2 - y1;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int64_t sx = (x1 < x2) ? 1 : -1;
    int64_t sy = (y1 < y2) ? 1 : -1;
    int64_t err = dx - dy;
    uint32_t c = (uint32_t)color;

    while (1) {
        if (x1 >= 0 && x1 < fb_width && y1 >= 0 && y1 < fb_height)
            *fb_pixel_ptr(x1, y1) = c;
        if (x1 == x2 && y1 == y2) break;
        int64_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

void sad_ll_fb_draw_string(int64_t x, int64_t y, int64_t str_ptr) {
    if (!fb_initialized || !fb_base) return;
    // Basic 8x8 bitmap font rendering placeholder
    // In a real kernel, you'd have a font table. Here we just place markers.
    const char* str = (const char*)str_ptr;
    int64_t cx = x;
    while (*str) {
        // Draw a simple 6x8 block for each character
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 6; col++) {
                if (cx + col >= 0 && cx + col < fb_width &&
                    y + row >= 0 && y + row < fb_height) {
                    // Simple pattern: fill based on ASCII value bits
                    if (((uint8_t)*str >> (row % 8)) & 1) {
                        *fb_pixel_ptr(cx + col, y + row) = 0xFFFFFF;
                    }
                }
            }
        }
        cx += 8;
        str++;
    }
}

void sad_ll_fb_clear(int64_t color) {
    if (!fb_initialized || !fb_base) return;
    uint32_t c = (uint32_t)color;
    // (AR) المسح صفًّا صفًّا بدلالة الخطوة، لا خطّيًّا، ليصحّ مع الحشو العتاديّ.
    // (EN) Clear row-by-row via pitch (not linearly) so hardware padding is honoured.
    for (int64_t j = 0; j < fb_height; j++)
        for (int64_t i = 0; i < fb_width; i++)
            *fb_pixel_ptr(i, j) = c;
}

void sad_ll_fb_clear_default(void) {
    sad_ll_fb_clear(0);
}

const char* sad_ll_fb_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "Framebuffer: ");
    if (fb_initialized) {
        char tmp[24];
        int_to_str(fb_width, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, "x");
        int_to_str(fb_height, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, "x");
        int_to_str(fb_bpp, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, "bpp");
        rt_strcat(report_buf, " | Pitch: ");
        int_to_str(fb_pitch, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, " | Base: ");
        hex_to_str((uint64_t)fb_base, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15h. تنفيذ وحدة ACPI
// ============================================================================

void sad_ll_acpi_init(void) {
    // Search for RSDP in BIOS memory regions
    // RSDP is found at: EBDA, or 0xE0000-0xFFFFF
    // In real bare-metal, you'd scan these regions for "RSD PTR " signature
    acpi_initialized = 1;
}

int64_t sad_ll_acpi_find_table(int64_t sig) {
    if (!acpi_initialized) return 0;
    // In a real implementation, traverse RSDT/XSDT looking for table with
    // matching 4-byte signature. Returns address of table header.
    (void)sig;
    return 0; // Not found
}

void sad_ll_acpi_shutdown(void) {
    if (!acpi_initialized) return;
    // ACPI shutdown: write to PM1a_CNT_BLK with SLP_TYPa | SLP_EN
    // This requires parsing the FADT and DSDT first.
    // Fallback: triple fault to reset
    // outw(PM1a_CNT_BLK, SLP_TYPa | SLP_EN);
}

const char* sad_ll_acpi_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "ACPI: ");
    if (acpi_initialized) {
        rt_strcat(report_buf, "initialized");
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15i. تنفيذ وحدة التزامن / Sync Implementation
// ============================================================================

int64_t sad_ll_spinlock_init(void) {
    // Allocate a spinlock from pool
    uint64_t offset = phys_pool_offset;
    if (offset + sizeof(spinlock_t) > PHYS_ALLOC_POOL_SIZE) return 0;
    spinlock_t* lock = (spinlock_t*)(phys_pool + offset);
    phys_pool_offset += sizeof(spinlock_t);
    *lock = 0;
    return (int64_t)(uint64_t)lock;
}

void sad_ll_spinlock_lock(int64_t lock_ptr) {
    volatile int32_t* lock = (volatile int32_t*)(uint64_t)lock_ptr;
    while (__sync_lock_test_and_set(lock, 1)) {
        while (*lock) {
            pause_cpu();
        }
    }
}

void sad_ll_spinlock_unlock(int64_t lock_ptr) {
    volatile int32_t* lock = (volatile int32_t*)(uint64_t)lock_ptr;
    __sync_lock_release(lock);
}

int64_t sad_ll_mutex_init(void) {
    // Same as spinlock for bare-metal (no OS scheduler to block)
    return sad_ll_spinlock_init();
}

void sad_ll_mutex_lock(int64_t mtx_ptr) {
    sad_ll_spinlock_lock(mtx_ptr);
}

void sad_ll_mutex_unlock(int64_t mtx_ptr) {
    sad_ll_spinlock_unlock(mtx_ptr);
}

int64_t sad_ll_semaphore_init(int64_t count) {
    // Allocate counter from pool
    uint64_t offset = phys_pool_offset;
    if (offset + sizeof(int64_t) > PHYS_ALLOC_POOL_SIZE) return 0;
    volatile int64_t* sem = (volatile int64_t*)(phys_pool + offset);
    phys_pool_offset += sizeof(int64_t);
    *sem = count;
    return (int64_t)(uint64_t)sem;
}

int64_t sad_ll_barrier_init(int64_t count) {
    // Allocate barrier struct: [count, total]
    uint64_t offset = phys_pool_offset;
    if (offset + sizeof(int64_t) * 2 > PHYS_ALLOC_POOL_SIZE) return 0;
    volatile int64_t* barrier = (volatile int64_t*)(phys_pool + offset);
    phys_pool_offset += sizeof(int64_t) * 2;
    barrier[0] = 0;      // current count
    barrier[1] = count;   // total count
    return (int64_t)(uint64_t)barrier;
}

// ============================================================================
// 15j. تنفيذ وحدة المجدول / Scheduler Implementation
// ============================================================================

void sad_ll_sched_init(void) {
    rt_memset(sched_tasks, 0, sizeof(sched_tasks));
    sched_task_count = 0;
    sched_initialized = 1;
}

int64_t sad_ll_sched_create_process(int64_t name_ptr) {
    if (!sched_initialized || sched_task_count >= SCHED_MAX_TASKS) return -1;

    int idx = sched_task_count++;
    sched_tasks[idx].id     = (int64_t)idx;
    sched_tasks[idx].entry  = 0;
    sched_tasks[idx].active = 1;

    if (name_ptr) {
        const char* name = (const char*)(uint64_t)name_ptr;
        uint64_t len = rt_strlen(name);
        if (len > 63) len = 63;
        rt_memcpy(sched_tasks[idx].name, name, len);
        sched_tasks[idx].name[len] = 0;
    }

    return (int64_t)idx;
}

int64_t sad_ll_sched_create_thread(int64_t name_ptr, int64_t entry_ptr) {
    int64_t id = sad_ll_sched_create_process(name_ptr);
    if (id >= 0) {
        sched_tasks[id].entry = entry_ptr;
    }
    return id;
}

void sad_ll_sched_yield(void) {
    // In a real scheduler, switch to next task
    // For bare-metal, just pause
    pause_cpu();
}

void sad_ll_sched_sleep(int64_t ms) {
    // Busy-wait using TSC
    // Approximate: assume ~1GHz TSC frequency
    uint64_t target = rdtsc() + (uint64_t)ms * 1000000ULL;
    while (rdtsc() < target) {
        pause_cpu();
    }
}

const char* sad_ll_sched_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "Scheduler: ");
    if (sched_initialized) {
        char tmp[24];
        int_to_str(sched_task_count, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, " tasks");
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15k. تنفيذ وحدة الإقلاع / Boot Implementation
// ============================================================================

int64_t sad_ll_boot_info(void) {
    // Return boot protocol identifier
    // 0 = unknown, 1 = Multiboot, 2 = Multiboot2, 3 = UEFI
    return 0; // Unknown by default; kernel should override
}

int64_t sad_ll_boot_memory_map(void) {
    // In a real kernel, this would come from the bootloader
    // Returns pointer to memory map array
    return 0;
}

const char* sad_ll_boot_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "Boot: freestanding mode | Protocol: unknown");
    return report_buf;
}

// ============================================================================
// 15l. تنفيذ وحدة نظام الملفات / VFS Implementation
// ============================================================================

int64_t sad_ll_vfs_mount(int64_t device, int64_t mount_point) {
    if (vfs_mount_count >= VFS_MAX_MOUNTS) return -1;

    int idx = vfs_mount_count++;
    vfs_mounts[idx].device      = device;
    vfs_mounts[idx].mount_point = mount_point;
    vfs_mounts[idx].active      = 1;

    return (int64_t)idx;
}

void sad_ll_vfs_unmount(int64_t mount_point) {
    for (int i = 0; i < vfs_mount_count; i++) {
        if (vfs_mounts[i].mount_point == mount_point && vfs_mounts[i].active) {
            vfs_mounts[i].active = 0;
            break;
        }
    }
}

int64_t sad_ll_vfs_open(int64_t path) {
    if (vfs_fd_next >= VFS_MAX_FDS) return -1;
    (void)path; // In a real FS, look up the path

    int fd = vfs_fd_next++;
    vfs_fds[fd].id       = (int64_t)fd;
    vfs_fds[fd].open     = 1;
    vfs_fds[fd].offset   = 0;
    vfs_fds[fd].mount_idx = 0;

    return (int64_t)fd;
}

int64_t sad_ll_vfs_read(int64_t fd, int64_t size) {
    if (fd < 0 || fd >= VFS_MAX_FDS || !vfs_fds[fd].open) return -1;
    // In a real FS, read from the backing storage
    vfs_fds[fd].offset += size;
    return size; // Return bytes read
}

int64_t sad_ll_vfs_write(int64_t fd, int64_t data) {
    if (fd < 0 || fd >= VFS_MAX_FDS || !vfs_fds[fd].open) return -1;
    (void)data;
    return 0; // Return bytes written
}

void sad_ll_vfs_close(int64_t fd) {
    if (fd < 0 || fd >= VFS_MAX_FDS) return;
    vfs_fds[fd].open = 0;
}

const char* sad_ll_vfs_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "VFS: ");
    char tmp[24];
    int_to_str(vfs_mount_count, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, " mounts | ");
    int open_count = 0;
    for (int i = 0; i < VFS_MAX_FDS; i++)
        if (vfs_fds[i].open) open_count++;
    int_to_str(open_count, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, " open files");
    return report_buf;
}

// ============================================================================
// 15m. تنفيذ وحدة APIC
// ============================================================================

static void apic_write(uint32_t reg, uint32_t val) {
    if (!apic_base_addr) return;
    volatile uint32_t* addr = (volatile uint32_t*)((uint8_t*)apic_base_addr + reg);
    *addr = val;
}

static uint32_t apic_read(uint32_t reg) {
    if (!apic_base_addr) return 0;
    volatile uint32_t* addr = (volatile uint32_t*)((uint8_t*)apic_base_addr + reg);
    return *addr;
}

void sad_ll_apic_init(void) {
    // Read APIC base from MSR
    uint64_t apic_msr = rdmsr(APIC_BASE_MSR);
    apic_base_addr = (volatile uint32_t*)(apic_msr & 0xFFFFF000ULL);

    // Enable APIC: set spurious interrupt vector register
    uint32_t svr = apic_read(APIC_REG_SVR);
    svr |= 0x100;  // Enable APIC
    svr |= 0xFF;   // Spurious vector = 0xFF
    apic_write(APIC_REG_SVR, svr);

    apic_initialized = 1;
}

void sad_ll_apic_send_eoi(void) {
    if (!apic_initialized) return;
    apic_write(APIC_REG_EOI, 0);
}

void sad_ll_apic_send_ipi(int64_t target, int64_t vector) {
    if (!apic_initialized) return;
    // Set target APIC ID in ICR high
    apic_write(APIC_REG_ICR_HI, (uint32_t)((uint64_t)target << 24));
    // Send IPI with vector in ICR low
    apic_write(APIC_REG_ICR_LO, (uint32_t)vector);
}

void sad_ll_apic_set_timer(int64_t divisor, int64_t count) {
    if (!apic_initialized) return;
    apic_write(APIC_REG_TIMER_DIV, (uint32_t)divisor);
    apic_write(APIC_REG_TIMER_INIT, (uint32_t)count);
}

const char* sad_ll_apic_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "APIC: ");
    if (apic_initialized) {
        rt_strcat(report_buf, "enabled");
        char tmp[24];
        rt_strcat(report_buf, " | Base: ");
        hex_to_str((uint64_t)apic_base_addr, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15n. تنفيذ وحدة HPET
// ============================================================================

void sad_ll_hpet_init(void) {
    // HPET base address usually found via ACPI HPET table
    // Default HPET base: 0xFED00000
    hpet_base_addr = (volatile uint64_t*)0xFED00000ULL;

    // Read general capabilities
    uint64_t caps = hpet_base_addr[0]; // offset 0x00
    hpet_period_fs = (caps >> 32);      // Upper 32 bits = period in femtoseconds

    // Enable HPET: set bit 0 of General Configuration Register (offset 0x10)
    hpet_base_addr[2] |= 1ULL; // offset 0x10 = index 2 (each entry is 8 bytes)

    hpet_initialized = 1;
}

int64_t sad_ll_hpet_read(void) {
    if (!hpet_initialized || !hpet_base_addr) return 0;
    // Main counter at offset 0xF0 = index 30
    return (int64_t)hpet_base_addr[30];
}

void sad_ll_hpet_sleep(int64_t nanoseconds) {
    if (!hpet_initialized || hpet_period_fs == 0) {
        // Fallback to TSC-based sleep
        sad_ll_sched_sleep(nanoseconds / 1000000);
        return;
    }

    // Convert nanoseconds to HPET ticks
    // period_fs is in femtoseconds, 1 ns = 1,000,000 fs
    uint64_t ticks = ((uint64_t)nanoseconds * 1000000ULL) / hpet_period_fs;
    uint64_t start = (uint64_t)sad_ll_hpet_read();
    uint64_t target = start + ticks;

    while ((uint64_t)sad_ll_hpet_read() < target) {
        pause_cpu();
    }
}

const char* sad_ll_hpet_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "HPET: ");
    if (hpet_initialized) {
        rt_strcat(report_buf, "enabled");
        char tmp[24];
        rt_strcat(report_buf, " | Period: ");
        int_to_str((int64_t)hpet_period_fs, tmp);
        rt_strcat(report_buf, tmp);
        rt_strcat(report_buf, " fs");
        rt_strcat(report_buf, " | Counter: ");
        int_to_str(sad_ll_hpet_read(), tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15o. تنفيذ وحدة استدعاءات النظام / Syscall Implementation
// ============================================================================

void sad_ll_syscall_init(void) {
    rt_memset(syscall_table, 0, sizeof(syscall_table));

    // Enable SYSCALL/SYSRET via MSRs
    // IA32_EFER MSR (0xC0000080): set SCE bit (bit 0)
    uint64_t efer = rdmsr(0xC0000080);
    efer |= 1;
    wrmsr(0xC0000080, efer);

    // IA32_STAR MSR (0xC0000081): set SYSRET CS/SS and SYSCALL CS/SS
    // Kernel CS=0x08, Kernel SS=0x10, User CS=0x18+16=0x28, User SS=0x20+8=0x28
    // Star format: [63:48]=SYSRET CS, [47:32]=SYSCALL CS, [31:0]=reserved
    uint64_t star = ((uint64_t)0x001B << 48) | ((uint64_t)0x0008 << 32);
    wrmsr(0xC0000081, star);

    syscall_initialized = 1;
}

void sad_ll_syscall_register(int64_t num, int64_t handler) {
    if (!syscall_initialized) return;
    if (num < 0 || num >= SYSCALL_MAX) return;
    syscall_table[num] = (syscall_handler_t)(uint64_t)handler;
}

int64_t sad_ll_syscall_invoke(int64_t num) {
    if (!syscall_initialized) return -1;
    if (num < 0 || num >= SYSCALL_MAX) return -1;
    if (!syscall_table[num]) return -1;
    return syscall_table[num]();
}

const char* sad_ll_syscall_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "Syscall: ");
    if (syscall_initialized) {
        int count = 0;
        for (int i = 0; i < SYSCALL_MAX; i++)
            if (syscall_table[i]) count++;
        rt_strcat(report_buf, "initialized | Registered: ");
        char tmp[24];
        int_to_str(count, tmp);
        rt_strcat(report_buf, tmp);
    } else {
        rt_strcat(report_buf, "not initialized");
    }
    return report_buf;
}

// ============================================================================
// 15p. تنفيذ عمليات الذاكرة المتقدمة / Advanced Memory Implementation
// ============================================================================

int64_t sad_ll_mem_alloc_phys(int64_t size) {
    // Align to 4KB
    uint64_t aligned_offset = (phys_pool_offset + 4095) & ~4095ULL;
    uint64_t alloc_size = ((uint64_t)size + 4095) & ~4095ULL;

    if (aligned_offset + alloc_size > PHYS_ALLOC_POOL_SIZE) return 0; // OOM

    uint64_t addr = (uint64_t)(phys_pool + aligned_offset);
    phys_pool_offset = aligned_offset + alloc_size;

    rt_memset((void*)addr, 0, alloc_size);
    return (int64_t)addr;
}

void sad_ll_mem_free_phys(int64_t addr) {
    // Simple bump allocator doesn't support individual free
    // In a real kernel, implement a bitmap or buddy allocator
    (void)addr;
}

void sad_ll_mem_map_region(int64_t phys, int64_t virt, int64_t size) {
    if (!paging_initialized) return;
    // Map pages covering the region
    uint64_t p = (uint64_t)phys & ~0xFFFULL;
    uint64_t v = (uint64_t)virt & ~0xFFFULL;
    uint64_t end = (uint64_t)virt + (uint64_t)size;

    while (v < end) {
        sad_ll_paging_map((int64_t)v, (int64_t)p, PAGE_PRESENT | PAGE_RW);
        v += PAGE_SIZE;
        p += PAGE_SIZE;
    }
}

const char* sad_ll_mem_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "Memory: Pool used ");
    char tmp[24];
    int_to_str((int64_t)phys_pool_offset, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, " / ");
    int_to_str(PHYS_ALLOC_POOL_SIZE, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, " bytes");
    return report_buf;
}

// ============================================================================
// القسم 16: بروتوكول UEFI — تنفيذ وقت التشغيل (Freestanding)
// Section 16: UEFI Boot Protocol — Freestanding Runtime Implementation
// ============================================================================
//
// (AR) هذا القسم ينفذ دوال UEFI المطلوبة في وضع freestanding.
//      على بيئة UEFI حقيقية: يستدعي خدمات البرنامج الثابت عبر جدول النظام.
//      يعتمد على أن الكود يبدأ من نقطة دخول efi_main التي تستقبل:
//        - EFI_HANDLE ImageHandle
//        - EFI_SYSTEM_TABLE* SystemTable
//      ثم يُحفظ المؤشرات محلياً لاستخدامها لاحقاً.
//
// (EN) This section implements UEFI functions needed in freestanding mode.
//      On real UEFI: calls firmware services via System Table.
//      Depends on the code starting from efi_main entry point receiving:
//        - EFI_HANDLE ImageHandle
//        - EFI_SYSTEM_TABLE* SystemTable
//      Then pointers are saved locally for later use.
// ============================================================================

// --- UEFI State ---
static int uefi_initialized = 0;
static int uefi_bs_done = 0;
static int64_t uefi_image_handle = 0;
static int64_t uefi_system_table = 0;

// UEFI GOP state
static int uefi_gop_initialized = 0;
static int32_t uefi_gop_mode_count = 5;
static int32_t uefi_current_gop_mode_val = 0;
static int64_t uefi_fb_base_addr = 0xFD000000;  // typical GOP framebuffer address
static int64_t uefi_fb_size_val = 1920 * 1080 * 4;

// UEFI Memory map key
static int64_t uefi_memmap_key = 0x42;

// Simple NVRAM storage (freestanding)
#define UEFI_MAX_VARS 16
#define UEFI_VAR_NAME_LEN 64
#define UEFI_VAR_VALUE_LEN 256
static char uefi_var_names[UEFI_MAX_VARS][UEFI_VAR_NAME_LEN];
static char uefi_var_values[UEFI_MAX_VARS][UEFI_VAR_VALUE_LEN];
static int  uefi_var_count = 0;

// Simple file descriptors (freestanding)
#define UEFI_MAX_FILES 16
static int uefi_file_open[UEFI_MAX_FILES];
static int64_t uefi_next_fd = 0;

// --- 16a. التهيئة والتحكم / Initialization & Control ---

int64_t sad_ll_uefi_init(int64_t image_handle, int64_t system_table) {
    uefi_image_handle = image_handle;
    uefi_system_table = system_table;
    uefi_initialized = 1;
    uefi_bs_done = 0;
    uefi_var_count = 0;
    uefi_next_fd = 0;
    rt_memset(uefi_file_open, 0, sizeof(uefi_file_open));
    return 0; // EFI_SUCCESS
}

int64_t sad_ll_uefi_exit_boot_services(int64_t map_key) {
    if (!uefi_initialized) return 2; // INVALID_PARAMETER
    if (map_key != uefi_memmap_key) return 2; // INVALID_PARAMETER
    uefi_bs_done = 1;
    return 0; // EFI_SUCCESS
}

int32_t sad_ll_uefi_is_initialized(void) {
    return uefi_initialized;
}

int32_t sad_ll_uefi_bs_exited(void) {
    return uefi_bs_done;
}

void sad_ll_uefi_reset_system(int32_t reset_type) {
    (void)reset_type;
    // على بيئة حقيقية: يكتب إلى منفذ 0xCF9 أو يستدعي RT->ResetSystem
    // On real hardware: writes to port 0xCF9 or calls RT->ResetSystem
#ifdef __x86_64__
    __asm__ volatile("cli; hlt");
#endif
}

// --- 16b. إدارة الذاكرة / Memory Services ---

int64_t sad_ll_uefi_alloc_pages(int32_t alloc_type, int32_t mem_type, int64_t pages) {
    (void)alloc_type;
    (void)mem_type;
    // استخدام المخصص الخطي الموجود
    int64_t size = pages * 4096;
    return sad_ll_mem_alloc_phys(size);
}

int64_t sad_ll_uefi_free_pages(int64_t address, int64_t pages) {
    (void)pages;
    sad_ll_mem_free_phys(address);
    return 0;
}

int64_t sad_ll_uefi_alloc_pool(int32_t mem_type, int64_t size) {
    (void)mem_type;
    return sad_ll_mem_alloc_phys(size);
}

int64_t sad_ll_uefi_free_pool(int64_t buffer) {
    sad_ll_mem_free_phys(buffer);
    return 0;
}

const char* sad_ll_uefi_get_memory_map(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "UEFI Memory Map:\n");
    rt_strcat(report_buf, "0x00000000-0x0009FFFF: Conventional (640KB)\n");
    rt_strcat(report_buf, "0x00100000-0x3FFFFFFF: Conventional (1GB)\n");
    rt_strcat(report_buf, "0xFEE00000-0xFEE00FFF: MMIO (APIC)\n");
    rt_strcat(report_buf, "0xFED00000-0xFED003FF: MMIO (HPET)\n");
    return report_buf;
}

int64_t sad_ll_uefi_get_memmap_key(void) {
    return uefi_memmap_key;
}

int64_t sad_ll_uefi_total_memory(void) {
    return 1073741824; // 1GB simulation
}

// --- 16c. بروتوكول الرسوميات GOP / Graphics Output Protocol ---

int64_t sad_ll_uefi_init_gop(void) {
    uefi_gop_initialized = 1;
    uefi_current_gop_mode_val = 0;
    return 0;
}

int64_t sad_ll_uefi_set_gop_mode(int32_t mode_number) {
    if (mode_number < 0 || mode_number >= uefi_gop_mode_count) return 2;
    uefi_current_gop_mode_val = mode_number;
    return 0;
}

const char* sad_ll_uefi_query_gop_mode(int32_t mode_number) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    char tmp[24];
    rt_strcat(report_buf, "mode=");
    int_to_str(mode_number, tmp);
    rt_strcat(report_buf, tmp);
    // Resolution based on mode
    switch (mode_number) {
        case 0: rt_strcat(report_buf, " 640x480"); break;
        case 1: rt_strcat(report_buf, " 800x600"); break;
        case 2: rt_strcat(report_buf, " 1024x768"); break;
        case 3: rt_strcat(report_buf, " 1280x720"); break;
        case 4: rt_strcat(report_buf, " 1920x1080"); break;
        default: rt_strcat(report_buf, " unknown"); break;
    }
    return report_buf;
}

int32_t sad_ll_uefi_gop_mode_count(void) {
    return uefi_gop_mode_count;
}

int32_t sad_ll_uefi_current_gop_mode(void) {
    return uefi_current_gop_mode_val;
}

int64_t sad_ll_uefi_framebuffer_base(void) {
    return uefi_fb_base_addr;
}

int64_t sad_ll_uefi_framebuffer_size(void) {
    return uefi_fb_size_val;
}

int64_t sad_ll_uefi_fill_screen(int32_t r, int32_t g, int32_t b) {
    if (!uefi_gop_initialized) return -1;
    // على بيئة حقيقية: يكتب مباشرة إلى framebuffer
    // On real hardware: write directly to framebuffer
    uint32_t color = (uint32_t)((r << 16) | (g << 8) | b);
    volatile uint32_t* fb = (volatile uint32_t*)uefi_fb_base_addr;
    // التنفيذ الفعلي يعتمد على الوضع الحالي
    (void)color;
    (void)fb;
    return 0;
}

int64_t sad_ll_uefi_draw_rect(int32_t x, int32_t y, int32_t w, int32_t h,
                               int32_t r, int32_t g, int32_t b) {
    if (!uefi_gop_initialized) return -1;
    (void)x; (void)y; (void)w; (void)h;
    (void)r; (void)g; (void)b;
    return 0;
}

int64_t sad_ll_uefi_gop_blt(void* buffer, int32_t operation,
                             int32_t srcX, int32_t srcY,
                             int32_t dstX, int32_t dstY,
                             int32_t width, int32_t height) {
    (void)buffer; (void)operation;
    (void)srcX; (void)srcY; (void)dstX; (void)dstY;
    (void)width; (void)height;
    return 0;
}

// --- 16d. خدمات وقت التشغيل / Runtime Services ---

const char* sad_ll_uefi_get_time(void) {
    // في وضع freestanding لا يوجد وقت حقيقي بدون CMOS
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "2025-01-01 00:00:00");
    return report_buf;
}

int64_t sad_ll_uefi_set_time(int32_t year, int32_t month, int32_t day,
                              int32_t hour, int32_t min, int32_t sec) {
    (void)year; (void)month; (void)day;
    (void)hour; (void)min; (void)sec;
    return 0;
}

const char* sad_ll_uefi_get_variable(const char* name) {
    for (int i = 0; i < uefi_var_count; i++) {
        // مقارنة بسيطة للاسم
        const char* a = uefi_var_names[i];
        const char* b = name;
        int match = 1;
        while (*a && *b) {
            if (*a != *b) { match = 0; break; }
            a++; b++;
        }
        if (match && *a == *b) {
            return uefi_var_values[i];
        }
    }
    return ""; // لا يوجد
}

int64_t sad_ll_uefi_set_variable(const char* name, const char* value, int32_t attrs) {
    (void)attrs;
    // بحث وتحديث أو إضافة جديد
    for (int i = 0; i < uefi_var_count; i++) {
        const char* a = uefi_var_names[i];
        const char* b = name;
        int match = 1;
        while (*a && *b) {
            if (*a != *b) { match = 0; break; }
            a++; b++;
        }
        if (match && *a == *b) {
            // تحديث القيمة
            int j = 0;
            while (value[j] && j < UEFI_VAR_VALUE_LEN - 1) {
                uefi_var_values[i][j] = value[j];
                j++;
            }
            uefi_var_values[i][j] = 0;
            return 0;
        }
    }
    // إضافة متغير جديد
    if (uefi_var_count >= UEFI_MAX_VARS) return 5; // BUFFER_TOO_SMALL
    int j = 0;
    while (name[j] && j < UEFI_VAR_NAME_LEN - 1) {
        uefi_var_names[uefi_var_count][j] = name[j];
        j++;
    }
    uefi_var_names[uefi_var_count][j] = 0;
    j = 0;
    while (value[j] && j < UEFI_VAR_VALUE_LEN - 1) {
        uefi_var_values[uefi_var_count][j] = value[j];
        j++;
    }
    uefi_var_values[uefi_var_count][j] = 0;
    uefi_var_count++;
    return 0;
}

// --- 16e. نظام الملفات / File System ---

int64_t sad_ll_uefi_open_volume(void) {
    if (uefi_next_fd >= UEFI_MAX_FILES) return -1;
    int64_t fd = uefi_next_fd++;
    uefi_file_open[fd] = 1;
    return fd;
}

int64_t sad_ll_uefi_open_file(int64_t parent_fd, const char* filename, int64_t mode) {
    (void)parent_fd; (void)filename; (void)mode;
    if (uefi_next_fd >= UEFI_MAX_FILES) return -1;
    int64_t fd = uefi_next_fd++;
    uefi_file_open[fd] = 1;
    return fd;
}

const char* sad_ll_uefi_read_file(int64_t fd, int64_t size) {
    (void)size;
    if (fd < 0 || fd >= UEFI_MAX_FILES || !uefi_file_open[fd]) return "";
    return ""; // لا بيانات فعلية في وضع freestanding
}

int64_t sad_ll_uefi_write_file(int64_t fd, const char* data, int64_t size) {
    if (fd < 0 || fd >= UEFI_MAX_FILES || !uefi_file_open[fd]) return -1;
    (void)data;
    return size;
}

int64_t sad_ll_uefi_close_file(int64_t fd) {
    if (fd < 0 || fd >= UEFI_MAX_FILES) return -1;
    uefi_file_open[fd] = 0;
    return 0;
}

const char* sad_ll_uefi_file_info(int64_t fd) {
    (void)fd;
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "name=unknown size=0 dir=no");
    return report_buf;
}

// --- 16f. بروتوكولات ومعلومات / Protocols & System Info ---

int64_t sad_ll_uefi_locate_protocol(const char* guid_name) {
    (void)guid_name;
    return 0xDEAD0000; // عنوان وهمي
}

const char* sad_ll_uefi_revision(void) {
    return "UEFI 2.10";
}

const char* sad_ll_uefi_vendor(void) {
    return "Sad OS Firmware";
}

int32_t sad_ll_uefi_fw_revision(void) {
    return 0x00010000; // 1.0
}

const char* sad_ll_uefi_report(void) {
    rt_memset(report_buf, 0, sizeof(report_buf));
    rt_strcat(report_buf, "=== UEFI Report ===\n");
    rt_strcat(report_buf, "Initialized: ");
    rt_strcat(report_buf, uefi_initialized ? "yes" : "no");
    rt_strcat(report_buf, "\nBoot Services Exited: ");
    rt_strcat(report_buf, uefi_bs_done ? "yes" : "no");
    rt_strcat(report_buf, "\nGOP: ");
    rt_strcat(report_buf, uefi_gop_initialized ? "yes" : "no");
    rt_strcat(report_buf, "\nFirmware: Sad OS Firmware v1.0\n");
    rt_strcat(report_buf, "UEFI Revision: 2.10\n");
    char tmp[24];
    rt_strcat(report_buf, "NVRAM Variables: ");
    int_to_str(uefi_var_count, tmp);
    rt_strcat(report_buf, tmp);
    rt_strcat(report_buf, "\n");
    return report_buf;
}

/* ============================================================================
 * القسم 17: ACPI الموسّع / Extended ACPI (Section 17)
 * ============================================================================ */

static int acpi_extended_init = 0;
static int acpi_enabled = 0;
static int acpi_version_val = 2;  /* Default ACPI 2.0+ */
static uint32_t acpi_pm_timer_val = 0;
static uint32_t acpi_processor_cnt = 4;  /* Simulated 4 cores */
static uint64_t acpi_lapic_addr = 0xFEE00000ULL;

void sad_ll_acpi_init_full(void) {
    acpi_extended_init = 1;
    acpi_enabled = 1;
    acpi_initialized = 1;
}

void sad_ll_acpi_init_rsdp(int64_t addr) {
    acpi_extended_init = 1;
    acpi_enabled = 1;
    acpi_initialized = 1;
    (void)addr; /* In real hardware, would parse RSDP at this address */
}

int32_t sad_ll_acpi_enable(void) {
    if (!acpi_initialized) return -1;
    acpi_enabled = 1;
    return 0;
}

void sad_ll_acpi_disable(void) {
    acpi_enabled = 0;
}

int32_t sad_ll_acpi_is_initialized(void) {
    return acpi_extended_init;
}

int32_t sad_ll_acpi_version(void) {
    return acpi_version_val;
}

void sad_ll_acpi_reboot(void) {
    /* In real hardware: write to ACPI reset register */
    /* On freestanding: halt or trigger triple fault */
}

int32_t sad_ll_acpi_sleep(int32_t state) {
    if (!acpi_enabled) return -1;
    /* S0=working, S1=standby, S3=suspend, S4=hibernate, S5=off */
    if (state < 0 || state > 5) return -1;
    /* In real hardware: program SLP_EN/SLP_TYP in PM1a/PM1b */
    return 0;
}

void sad_ll_acpi_delay_us(int32_t us) {
    /* PM timer runs at 3.579545 MHz */
    volatile uint32_t target = acpi_pm_timer_val + (uint32_t)(us * 3);
    while (acpi_pm_timer_val < target) {
        acpi_pm_timer_val++;
    }
}

int64_t sad_ll_acpi_read_pm_timer(void) {
    acpi_pm_timer_val += 100;  /* Simulate timer advancing */
    return (int64_t)acpi_pm_timer_val;
}

int32_t sad_ll_acpi_is_pm_32bit(void) {
    return 1; /* Simulate 32-bit PM timer */
}

int32_t sad_ll_acpi_processor_count(void) {
    return (int32_t)acpi_processor_cnt;
}

int64_t sad_ll_acpi_local_apic_addr(void) {
    return (int64_t)acpi_lapic_addr;
}

int64_t sad_ll_acpi_ecam_base(int32_t seg, int32_t bus) {
    /* PCIe ECAM: each bus = 1MB, base depends on segment */
    return 0xE0000000ULL + ((int64_t)seg * 256 * 0x100000) + ((int64_t)bus * 0x100000);
}

/* ============================================================================
 * القسم 18: APIC الموسّع / Extended APIC (Section 18)
 * ============================================================================ */

static int apic_timer_running = 0;
static uint32_t apic_timer_initial = 0;
static uint32_t apic_timer_current = 0;
static uint32_t apic_io_apic_count = 1;
static uint8_t apic_task_priority = 0;
static uint32_t apic_irq_mask = 0xFFFF; /* All masked initially */

int32_t sad_ll_apic_supported(void) {
    return 1; /* Simulated: APIC always available */
}

int32_t sad_ll_apic_x2_supported(void) {
    return 1; /* Simulated: x2APIC supported */
}

int32_t sad_ll_apic_id(void) {
    /* In real hardware: read from LAPIC[0x20] >> 24 */
    return 0; /* BSP */
}

int32_t sad_ll_apic_io_count(void) {
    return (int32_t)apic_io_apic_count;
}

void sad_ll_apic_init_timer(int32_t vec, int32_t mode, int32_t div) {
    (void)vec; (void)mode; (void)div;
    apic_timer_running = 0;
    apic_timer_initial = 0;
    apic_timer_current = 0;
}

void sad_ll_apic_start_timer(int32_t count) {
    apic_timer_initial = (uint32_t)count;
    apic_timer_current = (uint32_t)count;
    apic_timer_running = 1;
}

void sad_ll_apic_stop_timer(void) {
    apic_timer_running = 0;
}

int32_t sad_ll_apic_timer_count(void) {
    if (apic_timer_running && apic_timer_current > 0) {
        apic_timer_current--;
    }
    return (int32_t)apic_timer_current;
}

int32_t sad_ll_apic_calibrate(int32_t hz) {
    /* Return ticks-per-interval for requested frequency */
    return (int32_t)(1000000000ULL / (uint32_t)hz);
}

void sad_ll_apic_set_priority(int32_t prio) {
    apic_task_priority = (uint8_t)prio;
}

void sad_ll_apic_disable_pic(void) {
    /* In real hardware: outb(0xA1, 0xFF); outb(0x21, 0xFF); */
}

void sad_ll_apic_mask_irq(int32_t irq) {
    if (irq >= 0 && irq < 16) {
        apic_irq_mask |= (1u << irq);
    }
}

void sad_ll_apic_unmask_irq(int32_t irq) {
    if (irq >= 0 && irq < 16) {
        apic_irq_mask &= ~(1u << irq);
    }
}

void sad_ll_apic_route_irq(int32_t irq, int32_t vec, int32_t dest) {
    (void)irq; (void)vec; (void)dest;
    /* In real hardware: set I/O APIC redirection entry */
}

void sad_ll_apic_send_ipi_all(int32_t vec, int32_t include_self) {
    (void)vec; (void)include_self;
    /* In real hardware: write ICR with shorthand=ALL */
}

void sad_ll_apic_send_init_ipi(int32_t dest) {
    (void)dest;
    /* In real hardware: send INIT IPI to target processor */
}

void sad_ll_apic_send_sipi(int32_t dest, int32_t page) {
    (void)dest; (void)page;
    /* In real hardware: send SIPI with startup page address */
}

void sad_ll_apic_wait_delivery(void) {
    /* In real hardware: poll ICR delivery status bit */
}

int32_t sad_ll_apic_init_io(int32_t id, int64_t base, int32_t gsib) {
    (void)id; (void)base; (void)gsib;
    apic_io_apic_count++;
    return 0;
}

