// ============================================================================
// sad_lowlevel_rt.h — رأسية مكتبة وقت التشغيل المنخفضة المستوى
// Freestanding Low-Level Runtime Library Header
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الإصدار / Version: 5.0
//
// (AR) هذا الملف يوفر واجهة C لجميع دوال المكتبة المنخفضة المستوى (19 وحدة).
//      يُستخدم مع المترجم sadc في وضع --freestanding لبناء أنوية نظام تشغيل.
//      لا يعتمد على أي مكتبة قياسية (libc-free).
// (EN) This file provides a C interface for all low-level library functions (19 modules).
//      Used with sadc compiler in --freestanding mode for building OS kernels.
//      Does not depend on any standard library (libc-free).
// ============================================================================

#ifndef SAD_LOWLEVEL_RT_H
#define SAD_LOWLEVEL_RT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// 15a. وحدة المعالج المتقدمة / Advanced CPU Module
// ============================================================================
int64_t  sad_ll_cpu_get_info(void);
int64_t  sad_ll_cpu_get_features(void);
int64_t  sad_ll_read_cr(int64_t cr_num);
void     sad_ll_write_cr(int64_t cr_num, int64_t value);
const char* sad_ll_cpu_report(void);

// ============================================================================
// 15b. وحدة GDT
// ============================================================================
void     sad_ll_gdt_init(void);
void     sad_ll_gdt_load(void);
const char* sad_ll_gdt_report(void);

// ============================================================================
// 15c. وحدة الترحيل / Paging
// ============================================================================
void     sad_ll_paging_init(void);
void     sad_ll_paging_map(int64_t vaddr, int64_t paddr, int64_t flags);
void     sad_ll_paging_unmap(int64_t vaddr);
const char* sad_ll_paging_report(void);

// ============================================================================
// 15d. وحدة المقاطعات / IDT
// ============================================================================
void     sad_ll_idt_init(void);
void     sad_ll_idt_load(void);
void     sad_ll_idt_register_isr(int64_t num, int64_t handler);
void     sad_ll_idt_enable_irq(int64_t irq);
const char* sad_ll_idt_report(void);

// ============================================================================
// 15e. وحدة PCI
// ============================================================================
int64_t  sad_ll_pci_enumerate(void);
int64_t  sad_ll_pci_read_config(int64_t bus, int64_t dev, int64_t func, int64_t offset);
void     sad_ll_pci_write_config(int64_t bus, int64_t dev, int64_t func, int64_t offset, int64_t val);
int64_t  sad_ll_pci_device_count(void);
const char* sad_ll_pci_report(void);

// ============================================================================
// 15f. وحدة DMA
// ============================================================================
void     sad_ll_dma_init(void);
void     sad_ll_dma_transfer(int64_t src, int64_t dst, int64_t size);
int64_t  sad_ll_dma_status(void);
const char* sad_ll_dma_report(void);

// ============================================================================
// 15g. وحدة الشاشة / Framebuffer
// ============================================================================
void     sad_ll_fb_init(int64_t width, int64_t height, int64_t bpp);
// (AR) ف-١: ربط مخزن الإطار بعنوانه الحقيقيّ من المُقلِع (بدل البركة البرمجيّة).
// (EN) F-1: bind framebuffer to its real bootloader-provided address (not the pool).
void     sad_ll_fb_init_addr(int64_t addr, int64_t width, int64_t height,
                             int64_t pitch, int64_t bpp);
void     sad_ll_fb_set_pixel(int64_t x, int64_t y, int64_t color);
void     sad_ll_fb_draw_rect(int64_t x, int64_t y, int64_t w, int64_t h, int64_t color);
void     sad_ll_fb_fill_rect(int64_t x, int64_t y, int64_t w, int64_t h, int64_t color);
void     sad_ll_fb_draw_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t color);
void     sad_ll_fb_draw_string(int64_t x, int64_t y, int64_t str_ptr);
void     sad_ll_fb_clear(int64_t color);
void     sad_ll_fb_clear_default(void);
const char* sad_ll_fb_report(void);

// ============================================================================
// 15h. وحدة ACPI
// ============================================================================
void     sad_ll_acpi_init(void);
int64_t  sad_ll_acpi_find_table(int64_t sig);
void     sad_ll_acpi_shutdown(void);
const char* sad_ll_acpi_report(void);

// ============================================================================
// 15i. وحدة التزامن / Sync
// ============================================================================
int64_t  sad_ll_spinlock_init(void);
void     sad_ll_spinlock_lock(int64_t lock_ptr);
void     sad_ll_spinlock_unlock(int64_t lock_ptr);
int64_t  sad_ll_mutex_init(void);
void     sad_ll_mutex_lock(int64_t mtx_ptr);
void     sad_ll_mutex_unlock(int64_t mtx_ptr);
int64_t  sad_ll_semaphore_init(int64_t count);
int64_t  sad_ll_barrier_init(int64_t count);

// ============================================================================
// 15j. وحدة المجدول / Scheduler
// ============================================================================
void     sad_ll_sched_init(void);
int64_t  sad_ll_sched_create_process(int64_t name_ptr);
int64_t  sad_ll_sched_create_thread(int64_t name_ptr, int64_t entry_ptr);
void     sad_ll_sched_yield(void);
void     sad_ll_sched_sleep(int64_t ms);
const char* sad_ll_sched_report(void);

// ============================================================================
// 15k. وحدة الإقلاع / Boot
// ============================================================================
int64_t  sad_ll_boot_info(void);
int64_t  sad_ll_boot_memory_map(void);
const char* sad_ll_boot_report(void);

// ============================================================================
// 15l. وحدة نظام الملفات / VFS
// ============================================================================
int64_t  sad_ll_vfs_mount(int64_t device, int64_t mount_point);
void     sad_ll_vfs_unmount(int64_t mount_point);
int64_t  sad_ll_vfs_open(int64_t path);
int64_t  sad_ll_vfs_read(int64_t fd, int64_t size);
int64_t  sad_ll_vfs_write(int64_t fd, int64_t data);
void     sad_ll_vfs_close(int64_t fd);
const char* sad_ll_vfs_report(void);

// ============================================================================
// 15m. وحدة APIC
// ============================================================================
void     sad_ll_apic_init(void);
void     sad_ll_apic_send_eoi(void);
void     sad_ll_apic_send_ipi(int64_t target, int64_t vector);
void     sad_ll_apic_set_timer(int64_t divisor, int64_t count);
const char* sad_ll_apic_report(void);

// ============================================================================
// 15n. وحدة HPET
// ============================================================================
void     sad_ll_hpet_init(void);
int64_t  sad_ll_hpet_read(void);
void     sad_ll_hpet_sleep(int64_t nanoseconds);
const char* sad_ll_hpet_report(void);

// ============================================================================
// 15o. وحدة استدعاءات النظام / Syscall
// ============================================================================
void     sad_ll_syscall_init(void);
void     sad_ll_syscall_register(int64_t num, int64_t handler);
int64_t  sad_ll_syscall_invoke(int64_t num);
const char* sad_ll_syscall_report(void);

// ============================================================================
// 15p. عمليات الذاكرة المتقدمة / Advanced Memory
// ============================================================================
int64_t  sad_ll_mem_alloc_phys(int64_t size);
void     sad_ll_mem_free_phys(int64_t addr);
void     sad_ll_mem_map_region(int64_t phys, int64_t virt, int64_t size);
const char* sad_ll_mem_report(void);

// ============================================================================
// القسم 16: بروتوكول UEFI / UEFI Boot Protocol
// ============================================================================
//
// (AR) دوال runtime لبروتوكول UEFI في وضع freestanding.
//      على بيئة UEFI حقيقية: تستدعي خدمات UEFI عبر جدول النظام.
//      على بيئة محاكاة: تُرجع قيم تجريبية للاختبار.
//
// (EN) Runtime functions for UEFI protocol in freestanding mode.
//      On real UEFI: calls UEFI services via System Table.
//      On simulation: returns test values.
// ============================================================================

// 16a. التهيئة والتحكم / Initialization & Control
int64_t  sad_ll_uefi_init(int64_t image_handle, int64_t system_table);
int64_t  sad_ll_uefi_exit_boot_services(int64_t map_key);
int32_t  sad_ll_uefi_is_initialized(void);
int32_t  sad_ll_uefi_bs_exited(void);
void     sad_ll_uefi_reset_system(int32_t reset_type);

// 16b. إدارة الذاكرة / Memory Services
int64_t  sad_ll_uefi_alloc_pages(int32_t alloc_type, int32_t mem_type, int64_t pages);
int64_t  sad_ll_uefi_free_pages(int64_t address, int64_t pages);
int64_t  sad_ll_uefi_alloc_pool(int32_t mem_type, int64_t size);
int64_t  sad_ll_uefi_free_pool(int64_t buffer);
const char* sad_ll_uefi_get_memory_map(void);
int64_t  sad_ll_uefi_get_memmap_key(void);
int64_t  sad_ll_uefi_total_memory(void);

// 16c. بروتوكول الرسوميات GOP / Graphics Output Protocol
int64_t  sad_ll_uefi_init_gop(void);
int64_t  sad_ll_uefi_set_gop_mode(int32_t mode_number);
const char* sad_ll_uefi_query_gop_mode(int32_t mode_number);
int32_t  sad_ll_uefi_gop_mode_count(void);
int32_t  sad_ll_uefi_current_gop_mode(void);
int64_t  sad_ll_uefi_framebuffer_base(void);
int64_t  sad_ll_uefi_framebuffer_size(void);
int64_t  sad_ll_uefi_fill_screen(int32_t r, int32_t g, int32_t b);
int64_t  sad_ll_uefi_draw_rect(int32_t x, int32_t y, int32_t w, int32_t h,
                                int32_t r, int32_t g, int32_t b);
int64_t  sad_ll_uefi_gop_blt(void* buffer, int32_t operation,
                              int32_t srcX, int32_t srcY,
                              int32_t dstX, int32_t dstY,
                              int32_t width, int32_t height);

// 16d. خدمات وقت التشغيل / Runtime Services
const char* sad_ll_uefi_get_time(void);
int64_t  sad_ll_uefi_set_time(int32_t year, int32_t month, int32_t day,
                               int32_t hour, int32_t min, int32_t sec);
const char* sad_ll_uefi_get_variable(const char* name);
int64_t  sad_ll_uefi_set_variable(const char* name, const char* value, int32_t attrs);

// 16e. نظام الملفات / File System
int64_t  sad_ll_uefi_open_volume(void);
int64_t  sad_ll_uefi_open_file(int64_t parent_fd, const char* filename, int64_t mode);
const char* sad_ll_uefi_read_file(int64_t fd, int64_t size);
int64_t  sad_ll_uefi_write_file(int64_t fd, const char* data, int64_t size);
int64_t  sad_ll_uefi_close_file(int64_t fd);
const char* sad_ll_uefi_file_info(int64_t fd);

// 16f. بروتوكولات ومعلومات / Protocols & System Info
int64_t  sad_ll_uefi_locate_protocol(const char* guid_name);
const char* sad_ll_uefi_revision(void);
const char* sad_ll_uefi_vendor(void);
int32_t  sad_ll_uefi_fw_revision(void);
const char* sad_ll_uefi_report(void);

// ============================================================================
// القسم 17: ACPI الموسّع / Extended ACPI (Section 17)
// ============================================================================

// 17a. تهيئة وتفعيل / Init & Control
void        sad_ll_acpi_init_full(void);
void        sad_ll_acpi_init_rsdp(int64_t addr);
int32_t     sad_ll_acpi_enable(void);
void        sad_ll_acpi_disable(void);
int32_t     sad_ll_acpi_is_initialized(void);
int32_t     sad_ll_acpi_version(void);

// 17b. إدارة الطاقة / Power Management
void        sad_ll_acpi_reboot(void);
int32_t     sad_ll_acpi_sleep(int32_t state);
void        sad_ll_acpi_delay_us(int32_t us);

// 17c. مؤقت ومعالجات / Timer & Processors
int64_t     sad_ll_acpi_read_pm_timer(void);
int32_t     sad_ll_acpi_is_pm_32bit(void);
int32_t     sad_ll_acpi_processor_count(void);
int64_t     sad_ll_acpi_local_apic_addr(void);

// 17d. PCIe ECAM
int64_t     sad_ll_acpi_ecam_base(int32_t seg, int32_t bus);

// ============================================================================
// القسم 18: APIC الموسّع / Extended APIC (Section 18)
// ============================================================================

// 18a. استعلام ومعلومات / Query & Info
int32_t     sad_ll_apic_supported(void);
int32_t     sad_ll_apic_x2_supported(void);
int32_t     sad_ll_apic_id(void);
int32_t     sad_ll_apic_io_count(void);

// 18b. مؤقت APIC / APIC Timer
void        sad_ll_apic_init_timer(int32_t vec, int32_t mode, int32_t div);
void        sad_ll_apic_start_timer(int32_t count);
void        sad_ll_apic_stop_timer(void);
int32_t     sad_ll_apic_timer_count(void);
int32_t     sad_ll_apic_calibrate(int32_t hz);

// 18c. أولوية ومقاطعات / Priority & Interrupts
void        sad_ll_apic_set_priority(int32_t prio);
void        sad_ll_apic_disable_pic(void);
void        sad_ll_apic_mask_irq(int32_t irq);
void        sad_ll_apic_unmask_irq(int32_t irq);
void        sad_ll_apic_route_irq(int32_t irq, int32_t vec, int32_t dest);

// 18d. IPI متقدم / Advanced IPI
void        sad_ll_apic_send_ipi_all(int32_t vec, int32_t include_self);
void        sad_ll_apic_send_init_ipi(int32_t dest);
void        sad_ll_apic_send_sipi(int32_t dest, int32_t page);
void        sad_ll_apic_wait_delivery(void);
int32_t     sad_ll_apic_init_io(int32_t id, int64_t base, int32_t gsib);

#ifdef __cplusplus
}
#endif

#endif // SAD_LOWLEVEL_RT_H
