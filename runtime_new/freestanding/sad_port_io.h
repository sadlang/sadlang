/**
 * =============================================================================
 * ملف: sad_port_io.h
 * الوصف: دوال إدخال/إخراج المنافذ (Port I/O) لمعالجات x86
 * المهمة: م-ج03 - واجهة إدخال/إخراج المنافذ
 * =============================================================================
 *
 * نواة BeeOS تحتاج التعامل مع أجهزة x86 عبر Port I/O:
 *   - UART (0x3F8) — المنفذ التسلسلي للتسجيل
 *   - PIC (0x20, 0xA0) — متحكم المقاطعات
 *   - PIT (0x40) — مؤقت النظام
 *   - PS/2 (0x60, 0x64) — لوحة المفاتيح والفأرة
 *   - VGA (0x3D4, 0x3D5) — التحكم بالعرض
 *
 * الدوال مُضمّنة دائماً (inline always) — بدون overhead.
 * =============================================================================
 */

#ifndef SAD_FREESTANDING_PORT_IO_H
#define SAD_FREESTANDING_PORT_IO_H

#include "sad_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//                     قراءة من منفذ (Port In)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * قراءة بايت واحد (8-bit) من منفذ
 * @param port رقم المنفذ (0-65535)
 * @return القيمة المقروءة
 */
static SAD_INLINE ع8 port_in_u8(ع16 port) {
#if defined(__GNUC__) || defined(__clang__)
    ع8 value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
#elif defined(_MSC_VER)
    return __inbyte(port);
#else
    (void)port;
    return 0;
#endif
}

/**
 * قراءة كلمة (16-bit) من منفذ
 */
static SAD_INLINE ع16 port_in_u16(ع16 port) {
#if defined(__GNUC__) || defined(__clang__)
    ع16 value;
    __asm__ volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
#elif defined(_MSC_VER)
    return __inword(port);
#else
    (void)port;
    return 0;
#endif
}

/**
 * قراءة كلمة مزدوجة (32-bit) من منفذ
 */
static SAD_INLINE ع32 port_in_u32(ع16 port) {
#if defined(__GNUC__) || defined(__clang__)
    ع32 value;
    __asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
#elif defined(_MSC_VER)
    return __indword(port);
#else
    (void)port;
    return 0;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                     كتابة إلى منفذ (Port Out)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * كتابة بايت واحد (8-bit) إلى منفذ
 * @param port رقم المنفذ
 * @param value القيمة للكتابة
 */
static SAD_INLINE void port_out_u8(ع16 port, ع8 value) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
#elif defined(_MSC_VER)
    __outbyte(port, value);
#else
    (void)port; (void)value;
#endif
}

/**
 * كتابة كلمة (16-bit) إلى منفذ
 */
static SAD_INLINE void port_out_u16(ع16 port, ع16 value) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("outw %0, %1" : : "a"(value), "Nd"(port));
#elif defined(_MSC_VER)
    __outword(port, value);
#else
    (void)port; (void)value;
#endif
}

/**
 * كتابة كلمة مزدوجة (32-bit) إلى منفذ
 */
static SAD_INLINE void port_out_u32(ع16 port, ع32 value) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("outl %0, %1" : : "a"(value), "Nd"(port));
#elif defined(_MSC_VER)
    __outdword(port, value);
#else
    (void)port; (void)value;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
//                     تأخير I/O (للأجهزة البطيئة)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تأخير قصير عبر I/O — مطلوب لبعض أجهزة x86 القديمة
 * يكتب بايت فارغ للمنفذ 0x80 (POST diagnostic port)
 */
static SAD_INLINE void port_io_wait(void) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
#elif defined(_MSC_VER)
    __outbyte(0x80, 0);
#endif
}

/**
 * قراءة مع تأخير
 */
static SAD_INLINE ع8 port_in_u8_wait(ع16 port) {
    ع8 value = port_in_u8(port);
    port_io_wait();
    return value;
}

/**
 * كتابة مع تأخير
 */
static SAD_INLINE void port_out_u8_wait(ع16 port, ع8 value) {
    port_out_u8(port, value);
    port_io_wait();
}

// ═══════════════════════════════════════════════════════════════════════════════
//                 أسماء عربية مُرادفة (لاستخدام في كود ص)
// ═══════════════════════════════════════════════════════════════════════════════

// قراءة من منفذ
#define منفذ_دخل_8(منفذ)           port_in_u8((منفذ))
#define منفذ_دخل_16(منفذ)          port_in_u16((منفذ))
#define منفذ_دخل_32(منفذ)          port_in_u32((منفذ))

// كتابة إلى منفذ
#define منفذ_خرج_8(منفذ, قيمة)     port_out_u8((منفذ), (قيمة))
#define منفذ_خرج_16(منفذ, قيمة)    port_out_u16((منفذ), (قيمة))
#define منفذ_خرج_32(منفذ, قيمة)    port_out_u32((منفذ), (قيمة))

// انتظار I/O
#define انتظر_io()                  port_io_wait()

// ═══════════════════════════════════════════════════════════════════════════════
//                     عناوين منافذ شائعة
// ═══════════════════════════════════════════════════════════════════════════════

// UART / المنفذ التسلسلي
#define PORT_COM1_DATA      0x3F8
#define PORT_COM1_INT       0x3F9
#define PORT_COM1_FIFO      0x3FA
#define PORT_COM1_LINE_CTRL 0x3FB
#define PORT_COM1_MODEM     0x3FC
#define PORT_COM1_LINE_STAT 0x3FD

// PIC — متحكم المقاطعات القابل للبرمجة
#define PORT_PIC1_CMD       0x20
#define PORT_PIC1_DATA      0x21
#define PORT_PIC2_CMD       0xA0
#define PORT_PIC2_DATA      0xA1

// PIT — مؤقت الفواصل القابل للبرمجة
#define PORT_PIT_CH0        0x40
#define PORT_PIT_CH1        0x41
#define PORT_PIT_CH2        0x42
#define PORT_PIT_CMD        0x43

// PS/2 — لوحة المفاتيح
#define PORT_PS2_DATA       0x60
#define PORT_PS2_STATUS     0x64
#define PORT_PS2_CMD        0x64

// VGA
#define PORT_VGA_INDEX      0x3D4
#define PORT_VGA_DATA       0x3D5

#ifdef __cplusplus
}
#endif

#endif // SAD_FREESTANDING_PORT_IO_H
