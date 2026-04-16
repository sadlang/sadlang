// ═══════════════════════════════════════════════════════════════════
// صNet — طبقة التوافق مع بيئة Freestanding (بدون نظام تشغيل)
// (AR) هذا الملف يوفر تعريفات بديلة لـ std:: المطلوبة عند البناء
//      في بيئة فريستاندنج (kernel, bare-metal, embedded)
//      يُعرَّف SADNET_FREESTANDING عبر CMake أو سطر الأوامر
//
// (EN) Freestanding compatibility layer for SadNet
// المكون: network/sadnet (توافق Freestanding)
// المسار: network/sadnet/include/sadnet/freestanding/freestanding.h
//      Provides fallback definitions when building without OS
//      Define SADNET_FREESTANDING via CMake or command line
//
// الاستخدام:
//   cmake -DSADNET_FREESTANDING=ON ..
//   أو: -DSADNET_FREESTANDING=ON في سطر الترجمة
// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_FREESTANDING_FREESTANDING_H
#define SAD_SADNET_FREESTANDING_FREESTANDING_H

#ifdef SADNET_FREESTANDING

// ═════════════════════════════════════════════════════════════════════
// (AR) في بيئة فريستاندنج، لا تتوفر المكتبة القياسية بالكامل
//      نوفر هنا الحد الأدنى المطلوب من الدوال والأنواع
// ═════════════════════════════════════════════════════════════════════

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace sad::net::freestanding
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) واجهة النقل المجردة — يجب تنفيذها في النواة (kernel)
    // (EN) Abstract transport interface — must be implemented by the kernel
    // ═════════════════════════════════════════════════════════════════════

    // (AR) دوال يجب أن يوفرها النظام المضيف (النواة)
    // (EN) Functions that must be provided by the host system (kernel)

    struct NetworkCallbacks
    {
        // (AR) إرسال حزمة UDP خام
        // (EN) Send a raw UDP packet
        int (*udp_send)(const uint8_t *dest_ip, uint16_t dest_port,
                        const uint8_t *data, size_t len) = nullptr;

        // (AR) استقبال حزمة UDP (بلوكينج أو مع مهلة بالمللي ثانية)
        // (EN) Receive a UDP packet (blocking or with timeout in ms)
        int (*udp_recv)(uint8_t *src_ip, uint16_t *src_port,
                        uint8_t *buf, size_t buf_len, uint32_t timeout_ms) = nullptr;

        // (AR) ربط مقبس UDP على منفذ
        int (*udp_bind)(uint16_t port) = nullptr;

        // (AR) إغلاق مقبس
        void (*udp_close)(int sock_id) = nullptr;

        // (AR) الحصول على الوقت الحالي بالمللي ثانية
        // (EN) Get current time in milliseconds
        uint64_t (*get_time_ms)() = nullptr;

        // (AR) الحصول على بايتات عشوائية من مولد الأرقام في النواة
        // (EN) Get random bytes from kernel RNG
        void (*get_random)(uint8_t *buf, size_t len) = nullptr;

        // (AR) طباعة رسالة تشخيصية (اختياري)
        void (*debug_print)(const char *msg) = nullptr;
    };

    // (AR) المؤشرات العالمية للدوال — تُهيَّأ عبر sadnet_freestanding_init()
    extern NetworkCallbacks g_callbacks;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تهيئة صNet في بيئة فريستاندنج
    // (EN) Initialize SadNet in freestanding mode
    // ═════════════════════════════════════════════════════════════════════

    inline void init(const NetworkCallbacks &callbacks)
    {
        g_callbacks = callbacks;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) بدائل للدوال القياسية المعتمدة على الكولباك
    // ═════════════════════════════════════════════════════════════════════

    inline uint64_t get_time_ms()
    {
        if (g_callbacks.get_time_ms)
            return g_callbacks.get_time_ms();
        return 0;
    }

    inline void get_random_bytes(uint8_t *buf, size_t len)
    {
        if (g_callbacks.get_random)
        {
            g_callbacks.get_random(buf, len);
        }
        else
        {
            // (AR) إذا لم يتوفر مولد — نستخدم بديل ضعيف (غير آمن!)
            // (EN) Weak fallback if no RNG provided (NOT SECURE)
            for (size_t i = 0; i < len; ++i)
                buf[i] = (uint8_t)(i * 0x9E3779B9u >> 16);
        }
    }

    inline void debug_log(const char *msg)
    {
        if (g_callbacks.debug_print)
            g_callbacks.debug_print(msg);
    }

} // namespace sad::net::freestanding

// ═════════════════════════════════════════════════════════════════════
// (AR) إعادة توجيه now_ms() و random_bytes() في بيئة فريستاندنج
// ═════════════════════════════════════════════════════════════════════

// (AR) هذه الماكروز تُحوِّل استدعاءات وحدات صNet
//      لتستخدم الكولباك بدلاً من دوال النظام
#define SADNET_NOW_MS() sad::net::freestanding::get_time_ms()
#define SADNET_RANDOM_BYTES(buf, len) sad::net::freestanding::get_random_bytes(buf, len)
#define SADNET_DEBUG_LOG(msg) sad::net::freestanding::debug_log(msg)

#endif // SADNET_FREESTANDING

#endif // SAD_SADNET_FREESTANDING_FREESTANDING_H
