/*
 * ============================================================================
 * منافذ الإدخال/الإخراج - I/O Ports for Sad Language  
 * ============================================================================
 * 
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر الوصول المباشر لمنافذ الأجهزة (I/O ports).
 * يستخدم تعليمات assembly مضمنة (inline assembly) للتواصل مع الأجهزة.
 * ضروري لبرمجة أنظمة التشغيل وبرامج تشغيل الأجهزة.
 * 
 * المميزات:
 * - قراءة/كتابة بايت واحد (8-bit)
 * - قراءة/كتابة كلمة (16-bit)
 * - قراءة/كتابة كلمة مزدوجة (32-bit)
 * - دعم المنافذ الشائعة
 * - عمليات آمنة
 * 
 * English Description:
 * -------------------
 * This file provides direct access to hardware I/O ports.
 * Uses inline assembly instructions to communicate with hardware.
 * Essential for OS programming and device drivers.
 * 
 * Features:
 * - Read/Write byte (8-bit)
 * - Read/Write word (16-bit)
 * - Read/Write dword (32-bit)  
 * - Support for common ports
 * - Safe operations
 * 
 * المؤلف / Author: Sad Language Team
 * التاريخ / Date: December 2025
 * الإصدار / Version: 1.0.0
 * ============================================================================
 */

#ifndef SAD_IO_PORTS_H
#define SAD_IO_PORTS_H

#include <cstdint>
#include <string>

namespace Sad {
namespace LowLevel {

// ============================================================================
// منافذ شائعة / Common Ports
// ============================================================================

namespace StandardPorts {
    // منافذ PIC (متحكم المقاطعات القابل للبرمجة)
    // PIC (Programmable Interrupt Controller) ports
    constexpr uint16_t PIC1_COMMAND = 0x20;
    constexpr uint16_t PIC1_DATA = 0x21;
    constexpr uint16_t PIC2_COMMAND = 0xA0;
    constexpr uint16_t PIC2_DATA = 0xA1;
    
    // منافذ PIT (مؤقت الفواصل القابل للبرمجة)
    // PIT (Programmable Interval Timer) ports
    constexpr uint16_t PIT_CHANNEL0 = 0x40;
    constexpr uint16_t PIT_CHANNEL1 = 0x41;
    constexpr uint16_t PIT_CHANNEL2 = 0x42;
    constexpr uint16_t PIT_COMMAND = 0x43;
    
    // منافذ لوحة المفاتيح / Keyboard ports
    constexpr uint16_t KEYBOARD_DATA = 0x60;
    constexpr uint16_t KEYBOARD_STATUS = 0x64;
    constexpr uint16_t KEYBOARD_COMMAND = 0x64;
    
    // منافذ المنفذ التسلسلي / Serial port
    constexpr uint16_t COM1_BASE = 0x3F8;
    constexpr uint16_t COM2_BASE = 0x2F8;
    constexpr uint16_t COM3_BASE = 0x3E8;
    constexpr uint16_t COM4_BASE = 0x2E8;
    
    // منافذ المنفذ المتوازي / Parallel port
    constexpr uint16_t LPT1_BASE = 0x378;
    constexpr uint16_t LPT2_BASE = 0x278;
    
    // منافذ VGA / VGA ports
    constexpr uint16_t VGA_AC_INDEX = 0x3C0;
    constexpr uint16_t VGA_AC_WRITE = 0x3C0;
    constexpr uint16_t VGA_AC_READ = 0x3C1;
    constexpr uint16_t VGA_MISC_WRITE = 0x3C2;
    constexpr uint16_t VGA_SEQ_INDEX = 0x3C4;
    constexpr uint16_t VGA_SEQ_DATA = 0x3C5;
    constexpr uint16_t VGA_GC_INDEX = 0x3CE;
    constexpr uint16_t VGA_GC_DATA = 0x3CF;
    constexpr uint16_t VGA_CRTC_INDEX = 0x3D4;
    constexpr uint16_t VGA_CRTC_DATA = 0x3D5;
    
    // منافذ CMOS/RTC / CMOS/RTC ports
    constexpr uint16_t CMOS_ADDRESS = 0x70;
    constexpr uint16_t CMOS_DATA = 0x71;
    
    // منافذ PS/2
    constexpr uint16_t PS2_DATA = 0x60;
    constexpr uint16_t PS2_STATUS = 0x64;
    constexpr uint16_t PS2_COMMAND = 0x64;
}

// ============================================================================
// عمليات المنافذ الأساسية / Basic Port Operations
// ============================================================================

/**
 * فئة عمليات المنافذ
 * I/O Port Operations class
 */
class IOPorts {
public:
    // ========================================================================
    // قراءة بايت / Read Byte (8-bit)
    // ========================================================================
    
    /**
     * قراءة بايت من منفذ
     * Read byte from port
     * 
     * @param port رقم المنفذ / Port number
     * @return القيمة المقروءة / Read value
     */
    static uint8_t inb(uint16_t port);
    
    /**
     * قراءة بايت مع تأخير
     * Read byte with delay
     * 
     * @param port رقم المنفذ / Port number
     * @return القيمة المقروءة / Read value
     * 
     * ملاحظة: يضيف تأخيراً قصيراً للأجهزة البطيئة
     * Note: Adds small delay for slow devices
     */
    static uint8_t inb_p(uint16_t port);
    
    // ========================================================================
    // كتابة بايت / Write Byte (8-bit)
    // ========================================================================
    
    /**
     * كتابة بايت إلى منفذ
     * Write byte to port
     * 
     * @param port رقم المنفذ / Port number
     * @param value القيمة للكتابة / Value to write
     */
    static void outb(uint16_t port, uint8_t value);
    
    /**
     * كتابة بايت مع تأخير
     * Write byte with delay
     * 
     * @param port رقم المنفذ / Port number
     * @param value القيمة للكتابة / Value to write
     */
    static void outb_p(uint16_t port, uint8_t value);
    
    // ========================================================================
    // قراءة كلمة / Read Word (16-bit)
    // ========================================================================
    
    /**
     * قراءة كلمة من منفذ
     * Read word from port
     * 
     * @param port رقم المنفذ / Port number
     * @return القيمة المقروءة / Read value
     */
    static uint16_t inw(uint16_t port);
    
    /**
     * قراءة كلمة مع تأخير
     * Read word with delay
     */
    static uint16_t inw_p(uint16_t port);
    
    // ========================================================================
    // كتابة كلمة / Write Word (16-bit)
    // ========================================================================
    
    /**
     * كتابة كلمة إلى منفذ
     * Write word to port
     * 
     * @param port رقم المنفذ / Port number
     * @param value القيمة للكتابة / Value to write
     */
    static void outw(uint16_t port, uint16_t value);
    
    /**
     * كتابة كلمة مع تأخير
     * Write word with delay
     */
    static void outw_p(uint16_t port, uint16_t value);
    
    // ========================================================================
    // قراءة كلمة مزدوجة / Read Double Word (32-bit)
    // ========================================================================
    
    /**
     * قراءة كلمة مزدوجة من منفذ
     * Read double word from port
     * 
     * @param port رقم المنفذ / Port number
     * @return القيمة المقروءة / Read value
     */
    static uint32_t inl(uint16_t port);
    
    /**
     * قراءة كلمة مزدوجة مع تأخير
     * Read double word with delay
     */
    static uint32_t inl_p(uint16_t port);
    
    // ========================================================================
    // كتابة كلمة مزدوجة / Write Double Word (32-bit)
    // ========================================================================
    
    /**
     * كتابة كلمة مزدوجة إلى منفذ
     * Write double word to port
     * 
     * @param port رقم المنفذ / Port number
     * @param value القيمة للكتابة / Value to write
     */
    static void outl(uint16_t port, uint32_t value);
    
    /**
     * كتابة كلمة مزدوجة مع تأخير
     * Write double word with delay
     */
    static void outl_p(uint16_t port, uint32_t value);
    
    // ========================================================================
    // عمليات متعددة / Multiple Operations
    // ========================================================================
    
    /**
     * قراءة عدة بايتات
     * Read multiple bytes
     * 
     * @param port رقم المنفذ / Port number
     * @param buffer المخزن المؤقت / Buffer
     * @param count العدد / Count
     */
    static void insb(uint16_t port, void* buffer, size_t count);
    
    /**
     * قراءة عدة كلمات
     * Read multiple words
     */
    static void insw(uint16_t port, void* buffer, size_t count);
    
    /**
     * قراءة عدة كلمات مزدوجة
     * Read multiple double words
     */
    static void insl(uint16_t port, void* buffer, size_t count);
    
    /**
     * كتابة عدة بايتات
     * Write multiple bytes
     */
    static void outsb(uint16_t port, const void* buffer, size_t count);
    
    /**
     * كتابة عدة كلمات
     * Write multiple words
     */
    static void outsw(uint16_t port, const void* buffer, size_t count);
    
    /**
     * كتابة عدة كلمات مزدوجة
     * Write multiple double words
     */
    static void outsl(uint16_t port, const void* buffer, size_t count);
    
    // ========================================================================
    // عمليات مساعدة / Helper Operations
    // ========================================================================
    
    /**
     * تأخير قصير I/O
     * Short I/O delay
     * 
     * يستخدم للتزامن مع الأجهزة البطيئة
     * Used for synchronization with slow devices
     */
    static void io_wait();
    
    /**
     * فحص صحة رقم منفذ
     * Validate port number
     */
    static bool isValidPort(uint16_t port);
    
    /**
     * الحصول على اسم منفذ إذا كان معروفاً
     * Get port name if known
     */
    static std::string getPortName(uint16_t port);
};

// ============================================================================
// واجهة عالية المستوى للأجهزة الشائعة
// High-level interface for common devices
// ============================================================================

/**
 * لوحة المفاتيح / Keyboard
 */
namespace Keyboard {
    /**
     * قراءة مفتاح من لوحة المفاتيح
     * Read key from keyboard
     */
    uint8_t readKey();
    
    /**
     * فحص إذا كان مفتاح متاحاً
     * Check if key is available
     */
    bool hasKey();
    
    /**
     * تفعيل/تعطيل لوحة المفاتيح
     * Enable/disable keyboard
     */
    void enable();
    void disable();
}

/**
 * المنفذ التسلسلي / Serial Port
 */
namespace SerialPort {
    /**
     * تهيئة منفذ تسلسلي
     * Initialize serial port
     */
    bool initialize(uint16_t port, uint32_t baudRate = 9600);
    
    /**
     * إرسال بايت
     * Send byte
     */
    void writeByte(uint16_t port, uint8_t data);
    
    /**
     * استقبال بايت
     * Receive byte
     */
    uint8_t readByte(uint16_t port);
    
    /**
     * إرسال نص
     * Send string
     */
    void writeString(uint16_t port, const char* str);
}

/**
 * مؤقت النظام / System Timer (PIT)
 */
namespace SystemTimer {
    /**
     * تهيئة المؤقت
     * Initialize timer
     * 
     * @param frequency التردد بالهرتز / Frequency in Hz
     */
    void initialize(uint32_t frequency);
    
    /**
     * الحصول على عدد العلامات
     * Get tick count
     */
    uint64_t getTicks();
    
    /**
     * الانتظار لمدة معينة
     * Wait for duration
     * 
     * @param milliseconds المدة بالميلي ثانية / Duration in milliseconds
     */
    void sleep(uint32_t milliseconds);
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_IO_PORTS_H
