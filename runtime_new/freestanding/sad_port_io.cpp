/**
 * =============================================================================
 * ملف: sad_port_io.cpp
 * الوصف: تنفيذ دوال المنافذ + أدوات UART بسيطة للتشخيص
 * المهمة: م-ج03 - واجهة إدخال/إخراج المنافذ
 * =============================================================================
 *
 * يوفر:
 *   - تهيئة UART للتسجيل التسلسلي (serial logging)
 *   - كتابة نص عبر COM1 (مفيد للتشخيص في QEMU)
 *   - دوال mساعدة للمقاطعات (CLI/STI)
 * =============================================================================
 */

#include "sad_port_io.h"
#include "sad_core.h"

extern "C" {

// ═══════════════════════════════════════════════════════════════════════════════
//                           تهيئة UART COM1
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تهيئة المنفذ التسلسلي COM1 بمعدل 115200 baud
 * مفيد لتسجيل رسائل التشخيص في QEMU:
 *   qemu-system-x86_64 -serial stdio ...
 */
منطق sad_uart_init(void) {
    // تعطيل المقاطعات
    port_out_u8(PORT_COM1_INT, 0x00);
    
    // تفعيل DLAB (Divisor Latch Access Bit) لضبط معدل البود
    port_out_u8(PORT_COM1_LINE_CTRL, 0x80);
    
    // ضبط معدل البود = 115200 (divisor = 1)
    port_out_u8(PORT_COM1_DATA, 0x01);     // Divisor low byte
    port_out_u8(PORT_COM1_INT, 0x00);      // Divisor high byte
    
    // 8 بت بيانات, لا parity, 1 stop bit (8N1)
    port_out_u8(PORT_COM1_LINE_CTRL, 0x03);
    
    // تفعيل FIFO, مسح المخازن, حد 14 بايت
    port_out_u8(PORT_COM1_FIFO, 0xC7);
    
    // تفعيل IRQs, ضبط RTS/DSR
    port_out_u8(PORT_COM1_MODEM, 0x0B);
    
    // وضع اختبار (Loopback mode)
    port_out_u8(PORT_COM1_MODEM, 0x1E);
    
    // إرسال بايت اختبار
    port_out_u8(PORT_COM1_DATA, 0xAE);
    
    // تحقق من الاستلام
    if (port_in_u8(PORT_COM1_DATA) != 0xAE) {
        return خطأ; // COM1 لا يعمل
    }
    
    // إنهاء وضع الاختبار — تفعيل عادي
    port_out_u8(PORT_COM1_MODEM, 0x0F);
    
    return صحيح;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           كتابة UART
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * هل المنفذ التسلسلي جاهز للإرسال؟
 */
static منطق uart_is_transmit_empty(void) {
    return (port_in_u8(PORT_COM1_LINE_STAT) & 0x20) != 0;
}

/**
 * إرسال حرف واحد عبر COM1
 */
void sad_uart_putchar(char c) {
    // انتظار جاهزية الإرسال
    while (!uart_is_transmit_empty()) {
        // حلقة انتظار مشغولة (busy-wait)
    }
    
    port_out_u8(PORT_COM1_DATA, (ع8)c);
}

/**
 * إرسال نص عبر COM1
 */
void sad_uart_puts(const char* نص) {
    while (*نص) {
        if (*نص == '\n') {
            sad_uart_putchar('\r'); // CR+LF
        }
        sad_uart_putchar(*نص);
        نص++;
    }
}

/**
 * إرسال رقم عشري عبر COM1
 */
void sad_uart_put_uint(ع64 قيمة) {
    if (قيمة == 0) {
        sad_uart_putchar('0');
        return;
    }
    
    char مخزن[20];
    ح32 ن = 0;
    
    while (قيمة > 0) {
        مخزن[ن++] = '0' + (char)(قيمة % 10);
        قيمة /= 10;
    }
    
    // طباعة بالعكس
    while (ن > 0) {
        sad_uart_putchar(مخزن[--ن]);
    }
}

/**
 * إرسال رقم ست عشري عبر COM1
 */
void sad_uart_put_hex(ع64 قيمة) {
    const char أرقام[] = "0123456789ABCDEF";
    
    sad_uart_putchar('0');
    sad_uart_putchar('x');
    
    // أوجد أعلى nibble غير صفري
    منطق بدأ = خطأ;
    for (ح32 ن = 60; ن >= 0; ن -= 4) {
        ع8 nibble = (ع8)((قيمة >> ن) & 0xF);
        if (nibble != 0 || بدأ || ن == 0) {
            sad_uart_putchar(أرقام[nibble]);
            بدأ = صحيح;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           تحكم المقاطعات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تعطيل المقاطعات (CLI)
 */
void sad_interrupts_disable(void) {
#if defined(__GNUC__) || defined(__clang__)
    #if defined(SAD_ARCH_X86_64) || defined(SAD_ARCH_X86)
        __asm__ volatile("cli");
    #endif
#elif defined(_MSC_VER)
    _disable();
#endif
}

/**
 * تفعيل المقاطعات (STI)
 */
void sad_interrupts_enable(void) {
#if defined(__GNUC__) || defined(__clang__)
    #if defined(SAD_ARCH_X86_64) || defined(SAD_ARCH_X86)
        __asm__ volatile("sti");
    #endif
#elif defined(_MSC_VER)
    _enable();
#endif
}

/**
 * إيقاف المعالج حتى المقاطعة التالية (HLT)
 */
void sad_halt(void) {
#if defined(__GNUC__) || defined(__clang__)
    #if defined(SAD_ARCH_X86_64) || defined(SAD_ARCH_X86)
        __asm__ volatile("hlt");
    #elif defined(SAD_ARCH_AARCH64) || defined(SAD_ARCH_ARM)
        __asm__ volatile("wfi");
    #elif defined(SAD_ARCH_RISCV)
        __asm__ volatile("wfi");
    #endif
#endif
}

} // extern "C"
