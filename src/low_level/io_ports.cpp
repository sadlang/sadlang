/*
 * ============================================================================
 * تنفيذ منافذ I/O - I/O Ports Implementation
 * ============================================================================
 * 
 * هذا الملف يحتوي على التنفيذ الفعلي لعمليات المنافذ
 * This file contains the actual implementation of port operations
 * 
 * ملاحظة: inline assembly يعتمد على المترجم والمنصة
 * Note: inline assembly depends on compiler and platform
 * ============================================================================
 */

#include "low_level/io_ports.h"
#include <cstring>

namespace Sad {
namespace LowLevel {

// ============================================================================
// عمليات المنافذ الأساسية / Basic Port Operations
// ============================================================================

uint8_t IOPorts::inb(uint16_t port) {
    /*
     * قراءة بايت من منفذ
     * Read byte from port
     * 
     * يستخدم تعليمة in في x86
     * Uses in instruction in x86
     */
    
#if defined(_MSC_VER)
    // Microsoft Visual C++
    // Note: __inbyte is an intrinsic function
    return 0; // Placeholder - would use __inbyte(port) in real kernel mode
#elif defined(__GNUC__) || defined(__clang__)
    // GCC or Clang
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
#else
    // Fallback for other compilers
    return 0;
#endif
}

uint8_t IOPorts::inb_p(uint16_t port) {
    /*
     * قراءة بايت مع تأخير
     * Read byte with delay
     */
    uint8_t result = inb(port);
    io_wait();
    return result;
}

void IOPorts::outb(uint16_t port, uint8_t value) {
    /*
     * كتابة بايت إلى منفذ
     * Write byte to port
     * 
     * يستخدم تعليمة out في x86
     * Uses out instruction in x86
     */
    
#if defined(_MSC_VER)
    // Microsoft Visual C++
    // Placeholder - would use __outbyte(port, value) in real kernel mode
#elif defined(__GNUC__) || defined(__clang__)
    // GCC or Clang
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
#endif
}

void IOPorts::outb_p(uint16_t port, uint8_t value) {
    /*
     * كتابة بايت مع تأخير
     * Write byte with delay
     */
    outb(port, value);
    io_wait();
}

// ============================================================================
// عمليات 16-bit / 16-bit Operations
// ============================================================================

uint16_t IOPorts::inw(uint16_t port) {
    /*
     * قراءة كلمة (16-bit) من منفذ
     * Read word (16-bit) from port
     */
    
#if defined(_MSC_VER)
    return 0; // Placeholder - would use __inword(port)
#elif defined(__GNUC__) || defined(__clang__)
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
#else
    return 0;
#endif
}

uint16_t IOPorts::inw_p(uint16_t port) {
    uint16_t result = inw(port);
    io_wait();
    return result;
}

void IOPorts::outw(uint16_t port, uint16_t value) {
    /*
     * كتابة كلمة (16-bit) إلى منفذ
     * Write word (16-bit) to port
     */
    
#if defined(_MSC_VER)
    // Placeholder - would use __outword(port, value)
#elif defined(__GNUC__) || defined(__clang__)
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
#endif
}

void IOPorts::outw_p(uint16_t port, uint16_t value) {
    outw(port, value);
    io_wait();
}

// ============================================================================
// عمليات 32-bit / 32-bit Operations
// ============================================================================

uint32_t IOPorts::inl(uint16_t port) {
    /*
     * قراءة كلمة مزدوجة (32-bit) من منفذ
     * Read double word (32-bit) from port
     */
    
#if defined(_MSC_VER)
    return 0; // Placeholder - would use __indword(port)
#elif defined(__GNUC__) || defined(__clang__)
    uint32_t result;
    __asm__ volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
#else
    return 0;
#endif
}

uint32_t IOPorts::inl_p(uint16_t port) {
    uint32_t result = inl(port);
    io_wait();
    return result;
}

void IOPorts::outl(uint16_t port, uint32_t value) {
    /*
     * كتابة كلمة مزدوجة (32-bit) إلى منفذ
     * Write double word (32-bit) to port
     */
    
#if defined(_MSC_VER)
    // Placeholder - would use __outdword(port, value)
#elif defined(__GNUC__) || defined(__clang__)
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
#endif
}

void IOPorts::outl_p(uint16_t port, uint32_t value) {
    outl(port, value);
    io_wait();
}

// ============================================================================
// عمليات متعددة / Multiple Operations
// ============================================================================

void IOPorts::insb(uint16_t port, void* buffer, size_t count) {
    /*
     * قراءة عدة بايتات من منفذ
     * Read multiple bytes from port
     * 
     * يستخدم تعليمة rep insb
     * Uses rep insb instruction
     */
    
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile (
        "cld\n\t"
        "rep insb"
        : "+D"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
#else
    // Fallback: loop manually
    uint8_t* buf = static_cast<uint8_t*>(buffer);
    for (size_t i = 0; i < count; i++) {
        buf[i] = inb(port);
    }
#endif
}

void IOPorts::insw(uint16_t port, void* buffer, size_t count) {
    /*
     * قراءة عدة كلمات من منفذ
     * Read multiple words from port
     */
    
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile (
        "cld\n\t"
        "rep insw"
        : "+D"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
#else
    uint16_t* buf = static_cast<uint16_t*>(buffer);
    for (size_t i = 0; i < count; i++) {
        buf[i] = inw(port);
    }
#endif
}

void IOPorts::insl(uint16_t port, void* buffer, size_t count) {
    /*
     * قراءة عدة كلمات مزدوجة من منفذ
     * Read multiple double words from port
     */
    
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile (
        "cld\n\t"
        "rep insl"
        : "+D"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
#else
    uint32_t* buf = static_cast<uint32_t*>(buffer);
    for (size_t i = 0; i < count; i++) {
        buf[i] = inl(port);
    }
#endif
}

void IOPorts::outsb(uint16_t port, const void* buffer, size_t count) {
    /*
     * كتابة عدة بايتات إلى منفذ
     * Write multiple bytes to port
     */
    
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile (
        "cld\n\t"
        "rep outsb"
        : "+S"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
#else
    const uint8_t* buf = static_cast<const uint8_t*>(buffer);
    for (size_t i = 0; i < count; i++) {
        outb(port, buf[i]);
    }
#endif
}

void IOPorts::outsw(uint16_t port, const void* buffer, size_t count) {
    /*
     * كتابة عدة كلمات إلى منفذ
     * Write multiple words to port
     */
    
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile (
        "cld\n\t"
        "rep outsw"
        : "+S"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
#else
    const uint16_t* buf = static_cast<const uint16_t*>(buffer);
    for (size_t i = 0; i < count; i++) {
        outw(port, buf[i]);
    }
#endif
}

void IOPorts::outsl(uint16_t port, const void* buffer, size_t count) {
    /*
     * كتابة عدة كلمات مزدوجة إلى منفذ
     * Write multiple double words to port
     */
    
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile (
        "cld\n\t"
        "rep outsl"
        : "+S"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
#else
    const uint32_t* buf = static_cast<const uint32_t*>(buffer);
    for (size_t i = 0; i < count; i++) {
        outl(port, buf[i]);
    }
#endif
}

// ============================================================================
// عمليات مساعدة / Helper Operations
// ============================================================================

void IOPorts::io_wait() {
    /*
     * تأخير قصير I/O
     * Short I/O delay
     * 
     * يستخدم المنفذ 0x80 (منفذ غير مستخدم للتشخيص)
     * Uses port 0x80 (unused diagnostic port)
     * 
     * الكتابة إلى هذا المنفذ تستغرق وقتاً كافياً للتزامن
     * Writing to this port takes enough time for synchronization
     */
    outb(0x80, 0);
}

bool IOPorts::isValidPort(uint16_t port) {
    /*
     * فحص صحة رقم منفذ
     * Validate port number
     * 
     * جميع أرقام المنافذ 16-bit صالحة
     * All 16-bit port numbers are valid
     */
    return true;  // All 16-bit values are valid port numbers
}

std::string IOPorts::getPortName(uint16_t port) {
    /*
     * الحصول على اسم منفذ معروف
     * Get name of known port
     */
    
    using namespace StandardPorts;
    
    // PIC ports
    if (port == PIC1_COMMAND) return "PIC1_COMMAND";
    if (port == PIC1_DATA) return "PIC1_DATA";
    if (port == PIC2_COMMAND) return "PIC2_COMMAND";
    if (port == PIC2_DATA) return "PIC2_DATA";
    
    // PIT ports
    if (port == PIT_CHANNEL0) return "PIT_CHANNEL0";
    if (port == PIT_CHANNEL1) return "PIT_CHANNEL1";
    if (port == PIT_CHANNEL2) return "PIT_CHANNEL2";
    if (port == PIT_COMMAND) return "PIT_COMMAND";
    
    // Keyboard ports
    if (port == KEYBOARD_DATA) return "KEYBOARD_DATA";
    if (port == KEYBOARD_STATUS) return "KEYBOARD_STATUS";
    
    // Serial ports
    if (port == COM1_BASE) return "COM1";
    if (port == COM2_BASE) return "COM2";
    if (port == COM3_BASE) return "COM3";
    if (port == COM4_BASE) return "COM4";
    
    // Parallel ports
    if (port == LPT1_BASE) return "LPT1";
    if (port == LPT2_BASE) return "LPT2";
    
    // CMOS ports
    if (port == CMOS_ADDRESS) return "CMOS_ADDRESS";
    if (port == CMOS_DATA) return "CMOS_DATA";
    
    return "UNKNOWN";
}

// ============================================================================
// لوحة المفاتيح / Keyboard
// ============================================================================

namespace Keyboard {

uint8_t readKey() {
    /*
     * قراءة مفتاح من لوحة المفاتيح
     * Read key from keyboard
     */
    return IOPorts::inb(StandardPorts::KEYBOARD_DATA);
}

bool hasKey() {
    /*
     * فحص إذا كان مفتاح متاحاً
     * Check if key is available
     * 
     * البت 0 في سجل الحالة يشير إلى توفر البيانات
     * Bit 0 in status register indicates data availability
     */
    uint8_t status = IOPorts::inb(StandardPorts::KEYBOARD_STATUS);
    return (status & 0x01) != 0;
}

void enable() {
    /*
     * تفعيل لوحة المفاتيح
     * Enable keyboard
     */
    uint8_t status = IOPorts::inb(StandardPorts::KEYBOARD_STATUS);
    status &= ~0x10;  // Clear disable bit
    IOPorts::outb(StandardPorts::KEYBOARD_COMMAND, status);
}

void disable() {
    /*
     * تعطيل لوحة المفاتيح
     * Disable keyboard
     */
    uint8_t status = IOPorts::inb(StandardPorts::KEYBOARD_STATUS);
    status |= 0x10;  // Set disable bit
    IOPorts::outb(StandardPorts::KEYBOARD_COMMAND, status);
}

} // namespace Keyboard

// ============================================================================
// المنفذ التسلسلي / Serial Port
// ============================================================================

namespace SerialPort {

bool initialize(uint16_t port, uint32_t baudRate) {
    /*
     * تهيئة منفذ تسلسلي
     * Initialize serial port
     * 
     * الخطوات / Steps:
     * 1. تعطيل المقاطعات / Disable interrupts
     * 2. ضبط معدل البود / Set baud rate
     * 3. تهيئة تحكم الخط / Initialize line control
     * 4. تفعيل FIFO / Enable FIFO
     */
    
    // تعطيل المقاطعات / Disable interrupts
    IOPorts::outb(port + 1, 0x00);
    
    // تفعيل DLAB (ضبط معدل البود) / Enable DLAB (set baud rate)
    IOPorts::outb(port + 3, 0x80);
    
    // ضبط معدل البود / Set baud rate (divisor)
    uint16_t divisor = 115200 / baudRate;
    IOPorts::outb(port + 0, divisor & 0xFF);        // Low byte
    IOPorts::outb(port + 1, (divisor >> 8) & 0xFF); // High byte
    
    // 8 bits, no parity, one stop bit
    IOPorts::outb(port + 3, 0x03);
    
    // تفعيل FIFO، مسح الطابور، 14-byte threshold
    // Enable FIFO, clear queue, 14-byte threshold
    IOPorts::outb(port + 2, 0xC7);
    
    // IRQs enabled, RTS/DSR set
    IOPorts::outb(port + 4, 0x0B);
    
    return true;
}

void writeByte(uint16_t port, uint8_t data) {
    /*
     * إرسال بايت
     * Send byte
     * 
     * انتظار حتى يصبح المخزن المؤقت فارغاً
     * Wait until buffer is empty
     */
    
    // انتظار حتى يصبح جاهزاً للإرسال
    // Wait until ready to transmit
    while ((IOPorts::inb(port + 5) & 0x20) == 0) {
        // Busy wait
    }
    
    IOPorts::outb(port, data);
}

uint8_t readByte(uint16_t port) {
    /*
     * استقبال بايت
     * Receive byte
     * 
     * انتظار حتى تتوفر البيانات
     * Wait until data is available
     */
    
    // انتظار حتى تتوفر البيانات
    // Wait until data is available
    while ((IOPorts::inb(port + 5) & 0x01) == 0) {
        // Busy wait
    }
    
    return IOPorts::inb(port);
}

void writeString(uint16_t port, const char* str) {
    /*
     * إرسال نص
     * Send string
     */
    while (*str) {
        writeByte(port, *str++);
    }
}

} // namespace SerialPort

// ============================================================================
// مؤقت النظام / System Timer
// ============================================================================

namespace SystemTimer {

static uint64_t tickCount = 0;
static uint32_t timerFrequency = 0;

void initialize(uint32_t frequency) {
    /*
     * تهيئة مؤقت النظام (PIT)
     * Initialize system timer (PIT)
     * 
     * PIT frequency = 1193182 Hz
     * Divisor = 1193182 / desired_frequency
     */
    
    timerFrequency = frequency;
    uint32_t divisor = 1193182 / frequency;
    
    // أمر: channel 0, lobyte/hibyte, rate generator
    // Command: channel 0, lobyte/hibyte, rate generator
    IOPorts::outb(StandardPorts::PIT_COMMAND, 0x36);
    
    // إرسال القاسم / Send divisor
    IOPorts::outb(StandardPorts::PIT_CHANNEL0, divisor & 0xFF);
    IOPorts::outb(StandardPorts::PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

uint64_t getTicks() {
    /*
     * الحصول على عدد العلامات
     * Get tick count
     */
    return tickCount;
}

void sleep(uint32_t milliseconds) {
    /*
     * الانتظار لمدة معينة
     * Wait for duration
     * 
     * ملاحظة: في نظام حقيقي، سيستخدم المقاطعات
     * Note: In real system, would use interrupts
     */
    
    if (timerFrequency == 0) {
        return;  // Timer not initialized
    }
    
    uint64_t targetTicks = tickCount + (milliseconds * timerFrequency / 1000);
    while (tickCount < targetTicks) {
        // Busy wait (في نظام حقيقي، سنستخدم halt أو sleep)
        // Busy wait (in real system, would use halt or sleep)
    }
}

} // namespace SystemTimer

} // namespace LowLevel
} // namespace Sad
