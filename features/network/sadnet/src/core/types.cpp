// ===================================================================
// صNet — تنفيذ الأنواع الأساسية
// (AR) تحويل hex، أكواد الأخطاء، دوال مساعدة
// (EN) Core types implementation: hex conversion, error codes, helpers
// المكون: features/network/sadnet (الأنواع الأساسية)
// المسار: features/network/sadnet/src/core/types.cpp
// ===================================================================

#include "sadnet/core/types.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace sad::net
{

    // ─────────────────────────────────────────────────────────────────────
    // (AR) تحويل NodeId ↔ hex
    // ─────────────────────────────────────────────────────────────────────

    NodeId NodeId::from_hex(const std::string &hex)
    {
        NodeId id;
        if (hex.size() != NODE_ID_SIZE * 2)
            return id;
        for (size_t i = 0; i < NODE_ID_SIZE; i++)
        {
            unsigned byte = 0;
            auto hi = hex[i * 2];
            auto lo = hex[i * 2 + 1];
            auto hex_val = [](char c) -> unsigned
            {
                if (c >= '0' && c <= '9')
                    return c - '0';
                if (c >= 'a' && c <= 'f')
                    return 10 + c - 'a';
                if (c >= 'A' && c <= 'F')
                    return 10 + c - 'A';
                return 0;
            };
            byte = (hex_val(hi) << 4) | hex_val(lo);
            id.data[i] = static_cast<uint8_t>(byte);
        }
        return id;
    }

    std::string NodeId::to_hex() const
    {
        static const char digits[] = "0123456789abcdef";
        std::string result;
        result.reserve(NODE_ID_SIZE * 2);
        for (auto b : data)
        {
            result += digits[(b >> 4) & 0xF];
            result += digits[b & 0xF];
        }
        return result;
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) تحويل Buffer ↔ hex
    // ─────────────────────────────────────────────────────────────────────

    std::string Buffer::to_hex() const
    {
        static const char digits[] = "0123456789abcdef";
        std::string result;
        result.reserve(data_.size() * 2);
        for (auto b : data_)
        {
            result += digits[(b >> 4) & 0xF];
            result += digits[b & 0xF];
        }
        return result;
    }

    Buffer Buffer::from_hex(const std::string &hex)
    {
        Buffer buf;
        if (hex.size() % 2 != 0)
            return buf;
        buf.resize(hex.size() / 2);
        for (size_t i = 0; i < buf.size(); i++)
        {
            auto hex_val = [](char c) -> uint8_t
            {
                if (c >= '0' && c <= '9')
                    return c - '0';
                if (c >= 'a' && c <= 'f')
                    return 10 + c - 'a';
                if (c >= 'A' && c <= 'F')
                    return 10 + c - 'A';
                return 0;
            };
            buf[i] = (hex_val(hex[i * 2]) << 4) | hex_val(hex[i * 2 + 1]);
        }
        return buf;
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) رسائل الأخطاء بالعربية والإنجليزية
    // ─────────────────────────────────────────────────────────────────────

    const char *error_to_arabic(NetError e)
    {
        switch (e)
        {
        case NetError::OK:
            return "نجاح";
        case NetError::TIMEOUT:
            return "انتهاء المهلة";
        case NetError::CONNECTION_REFUSED:
            return "رفض الاتصال";
        case NetError::CONNECTION_RESET:
            return "إعادة تعيين الاتصال";
        case NetError::NETWORK_UNREACHABLE:
            return "الشبكة غير قابلة للوصول";
        case NetError::ADDRESS_IN_USE:
            return "العنوان مُستخدم بالفعل";
        case NetError::BUFFER_OVERFLOW:
            return "تجاوز المخزن المؤقت";
        case NetError::INVALID_ARGUMENT:
            return "وسيط غير صحيح";
        case NetError::CRYPTO_ERROR:
            return "خطأ تشفيري";
        case NetError::PROTOCOL_ERROR:
            return "خطأ بروتوكولي";
        case NetError::NOT_FOUND:
            return "غير موجود";
        case NetError::PERMISSION_DENIED:
            return "مرفوض";
        case NetError::INTERNAL_ERROR:
            return "خطأ داخلي";
        }
        return "خطأ غير معروف";
    }

    const char *error_to_english(NetError e)
    {
        switch (e)
        {
        case NetError::OK:
            return "OK";
        case NetError::TIMEOUT:
            return "Timeout";
        case NetError::CONNECTION_REFUSED:
            return "Connection refused";
        case NetError::CONNECTION_RESET:
            return "Connection reset";
        case NetError::NETWORK_UNREACHABLE:
            return "Network unreachable";
        case NetError::ADDRESS_IN_USE:
            return "Address in use";
        case NetError::BUFFER_OVERFLOW:
            return "Buffer overflow";
        case NetError::INVALID_ARGUMENT:
            return "Invalid argument";
        case NetError::CRYPTO_ERROR:
            return "Cryptographic error";
        case NetError::PROTOCOL_ERROR:
            return "Protocol error";
        case NetError::NOT_FOUND:
            return "Not found";
        case NetError::PERMISSION_DENIED:
            return "Permission denied";
        case NetError::INTERNAL_ERROR:
            return "Internal error";
        }
        return "Unknown error";
    }

} // namespace sad::net
