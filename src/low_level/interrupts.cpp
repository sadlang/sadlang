/*
 * ============================================================================
 * تنفيذ معالجة المقاطعات للغة ص
 * Interrupt Handling Implementation for Sad Language
 * ============================================================================
 */

#include "low_level/interrupts.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace Sad {
namespace LowLevel {

// ============================================================================
// Singleton Instance
// ============================================================================

InterruptManager& InterruptManager::getInstance() {
    static InterruptManager instance;
    return instance;
}

// ============================================================================
// المنشئ / Constructor
// ============================================================================

InterruptManager::InterruptManager() 
    : isInitialized_(false), isEnabled_(false) {
    // تصفير البيانات / Zero out data
    std::memset(idt_, 0, sizeof(idt_));
    std::memset(handlers_, 0, sizeof(handlers_));
    std::memset(interruptCounts_, 0, sizeof(interruptCounts_));
    
    // إعداد مؤشر IDT / Setup IDT pointer
    idtPointer_.limit = sizeof(idt_) - 1;
    idtPointer_.base = reinterpret_cast<uint64_t>(idt_);
}

// ============================================================================
// تهيئة النظام / System Initialization
// ============================================================================

void InterruptManager::initialize() {
    if (isInitialized_) {
        std::cout << "⚠️  تحذير: نظام المقاطعات مهيأ بالفعل\n";
        std::cout << "⚠️  Warning: Interrupt system already initialized\n";
        return;
    }
    
    std::cout << "🔧 تهيئة نظام المقاطعات...\n";
    std::cout << "🔧 Initializing interrupt system...\n";
    
    // 1. إعادة تخطيط PIC / Remap PIC
    remapPIC();
    
    // 2. تثبيت المعالجات الافتراضية / Install default handlers
    installDefaultHandlers();
    
    // 3. تحميل IDT / Load IDT
    loadIDT();
    
    isInitialized_ = true;
    
    std::cout << "✅ تم تهيئة نظام المقاطعات بنجاح\n";
    std::cout << "✅ Interrupt system initialized successfully\n";
}

void InterruptManager::remapPIC() {
    // إعادة تخطيط PIC (Programmable Interrupt Controller)
    // Remap PIC to avoid conflicts with CPU exceptions
    
    std::cout << "  📌 إعادة تخطيط PIC...\n";
    std::cout << "  📌 Remapping PIC...\n";
    
    // في بيئة محاكاة - نطبع فقط
    // In simulation environment - just print
    // في النظام الحقيقي، نستخدم منافذ I/O
    // In real system, we would use I/O ports
    
    /*
     * الأوامر الحقيقية للنظام:
     * Real system commands:
     * 
     * outb(0x20, 0x11);  // ICW1: بدء التهيئة / Initialize
     * outb(0xA0, 0x11);
     * outb(0x21, 0x20);  // ICW2: إزاحة IRQ0-7 إلى 32-39 / Offset IRQ0-7 to 32-39
     * outb(0xA1, 0x28);  // ICW2: إزاحة IRQ8-15 إلى 40-47 / Offset IRQ8-15 to 40-47
     * outb(0x21, 0x04);  // ICW3: تكوين التسلسل / Cascade configuration
     * outb(0xA1, 0x02);
     * outb(0x21, 0x01);  // ICW4: وضع 8086 / 8086 mode
     * outb(0xA1, 0x01);
     * outb(0x21, 0x00);  // تمكين جميع IRQs / Enable all IRQs
     * outb(0xA1, 0x00);
     */
    
    std::cout << "  ✓ تم إعادة تخطيط PIC: IRQ 0-15 -> INT 32-47\n";
    std::cout << "  ✓ PIC remapped: IRQ 0-15 -> INT 32-47\n";
}

void InterruptManager::initializePIC() {
    // تهيئة PIC بعد إعادة التخطيط
    // Initialize PIC after remapping
    
    std::cout << "  📌 تهيئة PIC...\n";
    std::cout << "  📌 Initializing PIC...\n";
}

void InterruptManager::installDefaultHandlers() {
    // تثبيت معالجات الاستثناءات الافتراضية
    // Install default exception handlers
    
    std::cout << "  📌 تثبيت المعالجات الافتراضية...\n";
    std::cout << "  📌 Installing default handlers...\n";
    
    // استثناءات المعالج / CPU Exceptions
    registerHandler(INT_DIVIDE_ERROR, handleDivideError);
    registerHandler(INT_PAGE_FAULT, handlePageFault);
    registerHandler(INT_GENERAL_PROTECTION, handleGeneralProtection);
    
    // معالج عام لباقي الاستثناءات / General handler for other exceptions
    for (uint8_t i = 0; i < 32; ++i) {
        if (!hasHandler(i)) {
            registerHandler(i, handleException);
        }
    }
    
    std::cout << "  ✓ تم تثبيت " << 32 << " معالج استثناء\n";
    std::cout << "  ✓ Installed " << 32 << " exception handlers\n";
}

void InterruptManager::enable() {
    if (!isInitialized_) {
        std::cout << "⚠️  خطأ: يجب تهيئة النظام أولاً\n";
        std::cout << "⚠️  Error: System must be initialized first\n";
        return;
    }
    
    // تفعيل المقاطعات (تعليمة STI)
    // Enable interrupts (STI instruction)
    
#ifdef _MSC_VER
    // MSVC: استخدام intrinsic
    // _enable();
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang: تعليمة مباشرة
    // asm volatile("sti");
#endif
    
    isEnabled_ = true;
    
    std::cout << "✅ تم تفعيل المقاطعات\n";
    std::cout << "✅ Interrupts enabled\n";
}

void InterruptManager::disable() {
    // تعطيل المقاطعات (تعليمة CLI)
    // Disable interrupts (CLI instruction)
    
#ifdef _MSC_VER
    // MSVC: استخدام intrinsic
    // _disable();
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang: تعليمة مباشرة
    // asm volatile("cli");
#endif
    
    isEnabled_ = false;
    
    std::cout << "🛑 تم تعطيل المقاطعات\n";
    std::cout << "🛑 Interrupts disabled\n";
}

bool InterruptManager::isEnabled() {
    // فحص حالة المقاطعات من خلال RFLAGS
    // Check interrupt state from RFLAGS register
    
#ifdef _MSC_VER
    // MSVC
    // return (__readeflags() & 0x200) != 0;
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang
    // uint64_t flags;
    // asm volatile("pushfq; pop %0" : "=r"(flags));
    // return (flags & 0x200) != 0;
#endif
    
    return isEnabled_;
}

// ============================================================================
// إدارة المعالجات / Handler Management
// ============================================================================

void InterruptManager::registerHandler(uint8_t number, InterruptHandler handler) {
    if (number >= 256) {
        std::cout << "⚠️  خطأ: رقم مقاطعة غير صالح: " << (int)number << "\n";
        std::cout << "⚠️  Error: Invalid interrupt number: " << (int)number << "\n";
        return;
    }
    
    handlers_[number] = handler;
    
    std::cout << "📝 تسجيل معالج للمقاطعة " << (int)number 
              << " (" << getInterruptName(number) << ")\n";
    std::cout << "📝 Registered handler for interrupt " << (int)number 
              << " (" << getInterruptName(number) << ")\n";
}

void InterruptManager::unregisterHandler(uint8_t number) {
    if (number >= 256) {
        return;
    }
    
    handlers_[number] = nullptr;
    
    std::cout << "🗑️  إلغاء تسجيل معالج المقاطعة " << (int)number << "\n";
    std::cout << "🗑️  Unregistered handler for interrupt " << (int)number << "\n";
}

InterruptHandler InterruptManager::getHandler(uint8_t number) {
    if (number >= 256) {
        return nullptr;
    }
    return handlers_[number];
}

bool InterruptManager::hasHandler(uint8_t number) {
    if (number >= 256) {
        return false;
    }
    return handlers_[number] != nullptr;
}

// ============================================================================
// إدارة IRQ / IRQ Management
// ============================================================================

void InterruptManager::enableIRQ(uint8_t irq) {
    if (irq >= 16) {
        std::cout << "⚠️  خطأ: رقم IRQ غير صالح: " << (int)irq << "\n";
        std::cout << "⚠️  Error: Invalid IRQ number: " << (int)irq << "\n";
        return;
    }
    
    // تفعيل IRQ في PIC
    // Enable IRQ in PIC
    
    /*
     * الأوامر الحقيقية:
     * Real commands:
     * 
     * uint16_t port = (irq < 8) ? 0x21 : 0xA1;
     * uint8_t value = inb(port) & ~(1 << (irq % 8));
     * outb(port, value);
     */
    
    std::cout << "✅ تم تفعيل IRQ" << (int)irq << "\n";
    std::cout << "✅ Enabled IRQ" << (int)irq << "\n";
}

void InterruptManager::disableIRQ(uint8_t irq) {
    if (irq >= 16) {
        std::cout << "⚠️  خطأ: رقم IRQ غير صالح: " << (int)irq << "\n";
        std::cout << "⚠️  Error: Invalid IRQ number: " << (int)irq << "\n";
        return;
    }
    
    // تعطيل IRQ في PIC
    // Disable IRQ in PIC
    
    /*
     * الأوامر الحقيقية:
     * Real commands:
     * 
     * uint16_t port = (irq < 8) ? 0x21 : 0xA1;
     * uint8_t value = inb(port) | (1 << (irq % 8));
     * outb(port, value);
     */
    
    std::cout << "🛑 تم تعطيل IRQ" << (int)irq << "\n";
    std::cout << "🛑 Disabled IRQ" << (int)irq << "\n";
}

void InterruptManager::sendEOI(uint8_t irq) {
    // إرسال End of Interrupt إلى PIC
    // Send End of Interrupt to PIC
    
    /*
     * الأوامر الحقيقية:
     * Real commands:
     * 
     * if (irq >= 8) {
     *     outb(0xA0, 0x20);  // إرسال EOI إلى Slave PIC
     * }
     * outb(0x20, 0x20);      // إرسال EOI إلى Master PIC
     */
    
    // في بيئة المحاكاة، لا نحتاج لفعل شيء
    // In simulation, we don't need to do anything
}

// ============================================================================
// إدارة IDT / IDT Management
// ============================================================================

void InterruptManager::setIDTEntry(uint8_t number, uint64_t handler, 
                                   uint16_t selector, uint8_t flags) {
    if (number >= 256) {
        return;
    }
    
    // تعيين إدخال IDT
    // Set IDT entry
    
    idt_[number].offsetLow = handler & 0xFFFF;
    idt_[number].selector = selector;
    idt_[number].ist = 0;
    idt_[number].flags = flags;
    idt_[number].offsetMiddle = (handler >> 16) & 0xFFFF;
    idt_[number].offsetHigh = (handler >> 32) & 0xFFFFFFFF;
    idt_[number].reserved = 0;
}

const IDTEntry* InterruptManager::getIDTEntry(uint8_t number) {
    if (number >= 256) {
        return nullptr;
    }
    return &idt_[number];
}

void InterruptManager::loadIDT() {
    // تحميل IDT إلى المعالج باستخدام تعليمة LIDT
    // Load IDT into CPU using LIDT instruction
    
    std::cout << "  📌 تحميل IDT إلى المعالج...\n";
    std::cout << "  📌 Loading IDT into CPU...\n";
    
#ifdef _MSC_VER
    // MSVC
    // __lidt(&idtPointer_);
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang
    // asm volatile("lidt %0" : : "m"(idtPointer_));
#endif
    
    std::cout << "  ✓ تم تحميل IDT (256 إدخال)\n";
    std::cout << "  ✓ IDT loaded (256 entries)\n";
}

// ============================================================================
// معالجات الاستثناءات / Exception Handlers
// ============================================================================

void InterruptManager::handleException(InterruptFrame* frame) {
    // معالج استثناء عام
    // General exception handler
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "🚨 استثناء المعالج / CPU Exception\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "رقم المقاطعة / Interrupt: " << frame->interruptNumber 
              << " (" << getInstance().getInterruptName(frame->interruptNumber) << ")\n";
    std::cout << "كود الخطأ / Error Code: 0x" << std::hex << frame->errorCode << std::dec << "\n";
    std::cout << "RIP: 0x" << std::hex << frame->rip << std::dec << "\n";
    std::cout << "RFLAGS: 0x" << std::hex << frame->rflags << std::dec << "\n";
    
    std::cout << std::string(60, '=') << "\n\n";
    
    // تحديث العداد
    getInstance().interruptCounts_[frame->interruptNumber]++;
}

void InterruptManager::handlePageFault(InterruptFrame* frame) {
    // معالج خطأ الصفحة
    // Page fault handler
    
    uint64_t faultingAddress;
    
#ifdef _MSC_VER
    // MSVC
    // faultingAddress = __readcr2();
    faultingAddress = 0;
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang
    // asm volatile("mov %%cr2, %0" : "=r"(faultingAddress));
    faultingAddress = 0;
#endif
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "🚨 خطأ صفحة / Page Fault\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "العنوان المسبب / Faulting Address: 0x" << std::hex << faultingAddress << std::dec << "\n";
    std::cout << "كود الخطأ / Error Code: 0x" << std::hex << frame->errorCode << std::dec << "\n";
    
    // تفسير كود الخطأ
    // Interpret error code
    std::cout << "\nتفسير الخطأ / Error interpretation:\n";
    std::cout << "  - " << ((frame->errorCode & 0x1) ? "حماية" : "صفحة غير موجودة") << " / ";
    std::cout << ((frame->errorCode & 0x1) ? "Protection violation" : "Page not present") << "\n";
    std::cout << "  - " << ((frame->errorCode & 0x2) ? "كتابة" : "قراءة") << " / ";
    std::cout << ((frame->errorCode & 0x2) ? "Write" : "Read") << "\n";
    std::cout << "  - " << ((frame->errorCode & 0x4) ? "وضع المستخدم" : "وضع النواة") << " / ";
    std::cout << ((frame->errorCode & 0x4) ? "User mode" : "Kernel mode") << "\n";
    
    std::cout << std::string(60, '=') << "\n\n";
    
    getInstance().interruptCounts_[frame->interruptNumber]++;
}

void InterruptManager::handleGeneralProtection(InterruptFrame* frame) {
    // معالج خطأ الحماية العامة
    // General protection fault handler
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "🚨 خطأ حماية عامة / General Protection Fault\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "كود الخطأ / Error Code: 0x" << std::hex << frame->errorCode << std::dec << "\n";
    std::cout << "RIP: 0x" << std::hex << frame->rip << std::dec << "\n";
    
    if (frame->errorCode != 0) {
        std::cout << "\nمحدد القطعة / Segment selector: 0x" << std::hex << (frame->errorCode & 0xFFF8) << std::dec << "\n";
        std::cout << "نوع الجدول / Table: " << ((frame->errorCode & 0x2) ? "LDT" : "GDT") << "\n";
        std::cout << "خارجي / External: " << ((frame->errorCode & 0x1) ? "نعم/Yes" : "لا/No") << "\n";
    }
    
    std::cout << std::string(60, '=') << "\n\n";
    
    getInstance().interruptCounts_[frame->interruptNumber]++;
}

void InterruptManager::handleDivideError(InterruptFrame* frame) {
    // معالج خطأ القسمة على صفر
    // Division by zero handler
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "🚨 خطأ قسمة على صفر / Division by Zero\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "RIP: 0x" << std::hex << frame->rip << std::dec << "\n";
    std::cout << "RAX: 0x" << std::hex << frame->rax << std::dec << "\n";
    std::cout << "RDX: 0x" << std::hex << frame->rdx << std::dec << "\n";
    
    std::cout << std::string(60, '=') << "\n\n";
    
    getInstance().interruptCounts_[frame->interruptNumber]++;
}

// ============================================================================
// معلومات وإحصائيات / Information & Statistics
// ============================================================================

uint64_t InterruptManager::getInterruptCount(uint8_t number) {
    if (number >= 256) {
        return 0;
    }
    return interruptCounts_[number];
}

uint64_t InterruptManager::getTotalInterrupts() {
    uint64_t total = 0;
    for (int i = 0; i < 256; ++i) {
        total += interruptCounts_[i];
    }
    return total;
}

std::string InterruptManager::getInterruptName(uint8_t number) {
    // الحصول على اسم المقاطعة
    // Get interrupt name
    
    switch (number) {
        // CPU Exceptions
        case 0: return "Divide Error / خطأ قسمة";
        case 1: return "Debug / تنقيح";
        case 2: return "NMI";
        case 3: return "Breakpoint / نقطة توقف";
        case 4: return "Overflow / فيض";
        case 5: return "Bound Range / تجاوز نطاق";
        case 6: return "Invalid Opcode / تعليمة غير صالحة";
        case 7: return "Device Not Available / جهاز غير متاح";
        case 8: return "Double Fault / خطأ مزدوج";
        case 9: return "Coprocessor Segment";
        case 10: return "Invalid TSS";
        case 11: return "Segment Not Present / قطعة غير موجودة";
        case 12: return "Stack-Segment Fault / خطأ قطعة المكدس";
        case 13: return "General Protection / حماية عامة";
        case 14: return "Page Fault / خطأ صفحة";
        case 16: return "x87 FPU Error";
        case 17: return "Alignment Check / فحص محاذاة";
        case 18: return "Machine Check / فحص آلة";
        case 19: return "SIMD Exception";
        case 20: return "Virtualization Exception";
        case 30: return "Security Exception / استثناء أمني";
        
        // Hardware IRQs
        case 32: return "IRQ0: Timer / مؤقت";
        case 33: return "IRQ1: Keyboard / لوحة مفاتيح";
        case 34: return "IRQ2: Cascade";
        case 35: return "IRQ3: COM2";
        case 36: return "IRQ4: COM1";
        case 37: return "IRQ5: LPT2";
        case 38: return "IRQ6: Floppy / قرص مرن";
        case 39: return "IRQ7: LPT1";
        case 40: return "IRQ8: RTC / ساعة";
        case 41: return "IRQ9: Free";
        case 42: return "IRQ10: Free";
        case 43: return "IRQ11: Free";
        case 44: return "IRQ12: PS/2 Mouse / فأرة";
        case 45: return "IRQ13: Coprocessor";
        case 46: return "IRQ14: Primary ATA / IDE أساسي";
        case 47: return "IRQ15: Secondary ATA / IDE ثانوي";
        
        // System calls
        case 128: return "System Call / استدعاء نظام";
        
        default:
            if (number < 32) {
                return "Reserved Exception / استثناء محجوز";
            } else if (number >= 32 && number < 48) {
                return "IRQ" + std::to_string(number - 32);
            } else {
                return "User Interrupt / مقاطعة مستخدم";
            }
    }
}

std::string InterruptManager::getInterruptReport() {
    // إنشاء تقرير شامل عن المقاطعات
    // Create comprehensive interrupt report
    
    std::ostringstream report;
    
    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير المقاطعات / Interrupt Report\n";
    report << std::string(70, '=') << "\n\n";
    
    // حالة النظام / System status
    report << "حالة النظام / System Status:\n";
    report << "  - مهيأ / Initialized: " << (isInitialized_ ? "نعم/Yes" : "لا/No") << "\n";
    report << "  - مفعل / Enabled: " << (isEnabled_ ? "نعم/Yes" : "لا/No") << "\n";
    report << "  - إجمالي المقاطعات / Total Interrupts: " << getTotalInterrupts() << "\n\n";
    
    // المقاطعات النشطة / Active interrupts
    report << "المقاطعات النشطة / Active Interrupts:\n";
    report << std::string(70, '-') << "\n";
    report << std::left << std::setw(6) << "رقم/No" 
           << std::setw(40) << "الاسم/Name" 
           << std::setw(10) << "العدد/Count" 
           << "المعالج/Handler\n";
    report << std::string(70, '-') << "\n";
    
    for (int i = 0; i < 256; ++i) {
        if (interruptCounts_[i] > 0 || hasHandler(i)) {
            report << std::left << std::setw(6) << i
                   << std::setw(40) << getInterruptName(i)
                   << std::setw(10) << interruptCounts_[i]
                   << (hasHandler(i) ? "✓" : "✗") << "\n";
        }
    }
    
    report << std::string(70, '=') << "\n\n";
    
    return report.str();
}

void InterruptManager::clearStatistics() {
    // مسح جميع الإحصائيات
    // Clear all statistics
    
    std::memset(interruptCounts_, 0, sizeof(interruptCounts_));
    
    std::cout << "🗑️  تم مسح إحصائيات المقاطعات\n";
    std::cout << "🗑️  Interrupt statistics cleared\n";
}

// ============================================================================
// وظائف مساعدة / Helper Functions
// ============================================================================

bool disableInterruptsAndSave() {
    // حفظ حالة المقاطعات الحالية وتعطيلها
    // Save current interrupt state and disable
    
    bool wasEnabled = InterruptManager::getInstance().isEnabled();
    InterruptManager::getInstance().disable();
    return wasEnabled;
}

void restoreInterrupts(bool state) {
    // استعادة حالة المقاطعات
    // Restore interrupt state
    
    if (state) {
        InterruptManager::getInstance().enable();
    } else {
        InterruptManager::getInstance().disable();
    }
}

} // namespace LowLevel
} // namespace Sad
