/*
 * ============================================================================
 * معالجة المقاطعات للغة ص - Interrupt Handling for Sad Language
 * ============================================================================
 * 
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر نظام معالجة المقاطعات (Interrupts).
 * يتضمن IDT (جدول وصف المقاطعات)، ISR (معالجات خدمة المقاطعات)،
 * وإدارة IRQ (طلبات المقاطعات الأجهزة).
 * 
 * English Description:
 * -------------------
 * This file provides the interrupt handling system.
 * Includes IDT (Interrupt Descriptor Table), ISR (Interrupt Service Routines),
 * and IRQ (Interrupt Request) management.
 * 
 * المؤلف / Author: Sad Language Team
 * التاريخ / Date: December 2025
 * الإصدار / Version: 1.0.0
 * ============================================================================
 */

#ifndef SAD_INTERRUPTS_H
#define SAD_INTERRUPTS_H

#include <cstdint>
#include <string>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت المقاطعات / Interrupt Constants
// ============================================================================

#define INT_DIVIDE_ERROR 0
#define INT_DEBUG 1
#define INT_NMI 2
#define INT_BREAKPOINT 3
#define INT_OVERFLOW 4
#define INT_BOUND_RANGE 5
#define INT_INVALID_OPCODE 6
#define INT_DEVICE_NOT_AVAILABLE 7
#define INT_DOUBLE_FAULT 8
#define INT_COPROCESSOR_SEGMENT 9
#define INT_INVALID_TSS 10
#define INT_SEGMENT_NOT_PRESENT 11
#define INT_STACK_SEGMENT_FAULT 12
#define INT_GENERAL_PROTECTION 13
#define INT_PAGE_FAULT 14
#define INT_X87_FPU_ERROR 16
#define INT_ALIGNMENT_CHECK 17
#define INT_MACHINE_CHECK 18
#define INT_SIMD_FP_EXCEPTION 19
#define INT_VIRTUALIZATION 20
#define INT_SECURITY_EXCEPTION 30

#define INT_IRQ_BASE 32
#define INT_IRQ0_TIMER 32
#define INT_IRQ1_KEYBOARD 33
#define INT_IRQ2_CASCADE 34
#define INT_IRQ3_COM2 35
#define INT_IRQ4_COM1 36
#define INT_IRQ5_LPT2 37
#define INT_IRQ6_FLOPPY 38
#define INT_IRQ7_LPT1 39
#define INT_IRQ8_RTC 40
#define INT_IRQ9_FREE1 41
#define INT_IRQ10_FREE2 42
#define INT_IRQ11_FREE3 43
#define INT_IRQ12_PS2_MOUSE 44
#define INT_IRQ13_COPROCESSOR 45
#define INT_IRQ14_PRIMARY_ATA 46
#define INT_IRQ15_SECONDARY_ATA 47

#define INT_SYSCALL 128

// ============================================================================
// بنية إدخال IDT / IDT Entry Structure
// ============================================================================

#pragma pack(push, 1)
struct IDTEntry {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offsetMiddle;
    uint32_t offsetHigh;
    uint32_t reserved;
    
    IDTEntry() : offsetLow(0), selector(0), ist(0), flags(0),
                 offsetMiddle(0), offsetHigh(0), reserved(0) {}
};
#pragma pack(pop)

// ============================================================================
// مؤشر IDT / IDT Pointer
// ============================================================================

#pragma pack(push, 1)
struct IDTPointer {
    uint16_t limit;
    uint64_t base;
};
#pragma pack(pop)

// ============================================================================
// إطار المقاطعة / Interrupt Frame
// ============================================================================

struct InterruptFrame {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t interruptNumber;
    uint64_t errorCode;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp_old;
    uint64_t ss;
};

// ============================================================================
// نوع معالج المقاطعة / Interrupt Handler Type
// ============================================================================

using InterruptHandler = std::function<void(InterruptFrame* frame)>;

// ============================================================================
// مدير المقاطعات / Interrupt Manager
// ============================================================================

class InterruptManager {
public:
    static InterruptManager& getInstance();
    
    // تهيئة النظام / System Initialization
    void initialize();
    void enable();
    void disable();
    bool isEnabled();
    
    // إدارة المعالجات / Handler Management
    void registerHandler(uint8_t number, InterruptHandler handler);
    void unregisterHandler(uint8_t number);
    InterruptHandler getHandler(uint8_t number);
    bool hasHandler(uint8_t number);
    
    // إدارة IRQ / IRQ Management
    void enableIRQ(uint8_t irq);
    void disableIRQ(uint8_t irq);
    void sendEOI(uint8_t irq);
    
    // إدارة IDT / IDT Management
    void setIDTEntry(uint8_t number, uint64_t handler, 
                     uint16_t selector, uint8_t flags);
    const IDTEntry* getIDTEntry(uint8_t number);
    void loadIDT();
    
    // معالجات الاستثناءات / Exception Handlers
    static void handleException(InterruptFrame* frame);
    static void handlePageFault(InterruptFrame* frame);
    static void handleGeneralProtection(InterruptFrame* frame);
    static void handleDivideError(InterruptFrame* frame);
    
    // معلومات وإحصائيات / Information & Statistics
    uint64_t getInterruptCount(uint8_t number);
    uint64_t getTotalInterrupts();
    std::string getInterruptName(uint8_t number);
    std::string getInterruptReport();
    void clearStatistics();
    
private:
    InterruptManager();
    ~InterruptManager() = default;
    InterruptManager(const InterruptManager&) = delete;
    InterruptManager& operator=(const InterruptManager&) = delete;
    
    void initializePIC();
    void remapPIC();
    void installDefaultHandlers();
    
    IDTEntry idt_[256];
    IDTPointer idtPointer_;
    InterruptHandler handlers_[256];
    uint64_t interruptCounts_[256];
    bool isInitialized_;
    bool isEnabled_;
};

// ============================================================================
// وظائف مساعدة / Helper Functions
// ============================================================================

bool disableInterruptsAndSave();
void restoreInterrupts(bool state);

class CriticalSection {
public:
    CriticalSection() : savedState_(disableInterruptsAndSave()) {}
    ~CriticalSection() { restoreInterrupts(savedState_); }
    
private:
    bool savedState_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_INTERRUPTS_H
