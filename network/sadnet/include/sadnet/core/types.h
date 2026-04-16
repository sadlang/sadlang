// ===================================================================
// صNet — الأنواع الأساسية المشتركة
// (AR) هذا الملف يُعرّف الأنواع الأساسية المُستخدمة في جميع وحدات
//      المكتبة: معرّفات، عناوين، مخازن مؤقتة، وأكواد أخطاء.
// (EN) Core types shared across all sadnet modules.
// المكون: network/sadnet (الأنواع الأساسية)
// المسار: network/sadnet/include/sadnet/core/types.h
// ===================================================================
#ifndef SAD_SADNET_CORE_TYPES_H
#define SAD_SADNET_CORE_TYPES_H

#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <vector>
#include <chrono>

namespace sad::net
{

    // ─────────────────────────────────────────────────────────────────────
    // (AR) ثوابت عامة
    // ─────────────────────────────────────────────────────────────────────

    /// حجم معرّف العقدة بالبايت (256 بت = 32 بايت)
    constexpr size_t NODE_ID_SIZE = 32;

    /// حجم بصمة SHA-256
    constexpr size_t SHA256_SIZE = 32;

    /// حجم مفتاح AES-256
    constexpr size_t AES_KEY_SIZE = 32;

    /// حجم nonce لـ AES-GCM
    constexpr size_t AES_NONCE_SIZE = 12;

    /// حجم وسم المصادقة GCM
    constexpr size_t AES_TAG_SIZE = 16;

    /// حجم مفتاح X25519
    constexpr size_t X25519_KEY_SIZE = 32;

    /// الحد الأقصى لحجم حزمة UDP
    constexpr size_t MAX_UDP_PACKET = 65507;

    /// المنفذ الافتراضي لبروتوكول صNet
    constexpr uint16_t DEFAULT_PORT = 7654;

    // ─────────────────────────────────────────────────────────────────────
    // (AR) معرّف العقدة — 256 بت (SHA-256 من المفتاح العام)
    // ─────────────────────────────────────────────────────────────────────

    struct NodeId
    {
        std::array<uint8_t, NODE_ID_SIZE> data{};

        NodeId() = default;

        /// إنشاء من مصفوفة بايت
        explicit NodeId(const std::array<uint8_t, NODE_ID_SIZE> &d) : data(d) {}

        /// إنشاء من نص سداسي عشري
        static NodeId from_hex(const std::string &hex);

        /// تحويل إلى نص سداسي عشري
        std::string to_hex() const;

        /// حساب مسافة XOR مع معرّف آخر
        NodeId xor_distance(const NodeId &other) const
        {
            NodeId result;
            for (size_t i = 0; i < NODE_ID_SIZE; i++)
            {
                result.data[i] = data[i] ^ other.data[i];
            }
            return result;
        }

        /// إيجاد فهرس أعلى بت غير صفري (0-255)
        int leading_zero_bits() const
        {
            for (size_t i = 0; i < NODE_ID_SIZE; i++)
            {
                if (data[i] != 0)
                {
                    for (int bit = 7; bit >= 0; bit--)
                    {
                        if (data[i] & (1 << bit))
                            return static_cast<int>(i * 8 + (7 - bit));
                    }
                }
            }
            return NODE_ID_SIZE * 8;
        }

        /// هل المعرّف فارغ (كله أصفار)؟
        bool is_zero() const
        {
            for (auto b : data)
                if (b != 0)
                    return false;
            return true;
        }

        bool operator==(const NodeId &o) const { return data == o.data; }
        bool operator!=(const NodeId &o) const { return data != o.data; }
        bool operator<(const NodeId &o) const { return data < o.data; }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) عنوان شبكي (IP + منفذ)
    // ─────────────────────────────────────────────────────────────────────

    struct Endpoint
    {
        std::string address; // عنوان IP (v4 أو v6)
        uint16_t port = 0;   // رقم المنفذ
        bool is_ipv6 = false;

        Endpoint() = default;
        Endpoint(std::string addr, uint16_t p, bool v6 = false)
            : address(std::move(addr)), port(p), is_ipv6(v6) {}

        /// تحويل إلى نص "IP:port"
        std::string to_string() const
        {
            if (is_ipv6)
                return "[" + address + "]:" + std::to_string(port);
            return address + ":" + std::to_string(port);
        }

        bool operator==(const Endpoint &o) const
        {
            return address == o.address && port == o.port;
        }
        bool operator<(const Endpoint &o) const
        {
            return std::tie(address, port) < std::tie(o.address, o.port);
        }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) مخزن مؤقت (Buffer) آمن — يُستخدم لتمرير البيانات الثنائية
    // ─────────────────────────────────────────────────────────────────────

    class Buffer
    {
        std::vector<uint8_t> data_;

    public:
        Buffer() = default;
        explicit Buffer(size_t size) : data_(size, 0) {}
        explicit Buffer(std::vector<uint8_t> d) : data_(std::move(d)) {}
        Buffer(const uint8_t *p, size_t len) : data_(p, p + len) {}
        Buffer(const std::string &s) : data_(s.begin(), s.end()) {}

        const uint8_t *data() const { return data_.data(); }
        uint8_t *data() { return data_.data(); }
        size_t size() const { return data_.size(); }
        bool empty() const { return data_.empty(); }

        void resize(size_t n) { data_.resize(n); }
        void clear() { data_.clear(); }
        void push_back(uint8_t b) { data_.push_back(b); }

        // (AR) إتاحة المكررات للتوافق مع خوارزميات STL
        auto begin() { return data_.begin(); }
        auto end() { return data_.end(); }
        auto begin() const { return data_.begin(); }
        auto end() const { return data_.end(); }

        void insert(std::vector<uint8_t>::iterator pos,
                    std::vector<uint8_t>::const_iterator first,
                    std::vector<uint8_t>::const_iterator last)
        {
            data_.insert(pos, first, last);
        }

        void append(const uint8_t *p, size_t len)
        {
            data_.insert(data_.end(), p, p + len);
        }
        void append(const Buffer &other)
        {
            data_.insert(data_.end(), other.data_.begin(), other.data_.end());
        }

        uint8_t operator[](size_t i) const { return data_[i]; }
        uint8_t &operator[](size_t i) { return data_[i]; }

        /// تحويل إلى نص سداسي عشري
        std::string to_hex() const;

        /// إنشاء من نص سداسي عشري
        static Buffer from_hex(const std::string &hex);

        /// تحويل إلى نص UTF-8
        std::string to_string() const
        {
            return std::string(data_.begin(), data_.end());
        }

        bool operator==(const Buffer &o) const { return data_ == o.data_; }

        /// مسح آمن — يُصفّر الذاكرة قبل التحرير (لمنع تسريب الأسرار)
        void secure_clear()
        {
            volatile uint8_t *p = data_.data();
            for (size_t i = 0; i < data_.size(); i++)
                p[i] = 0;
            data_.clear();
        }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) طابع زمني — يُغلّف std::chrono بواجهة بسيطة
    // ─────────────────────────────────────────────────────────────────────

    /// يُرجع الطابع الزمني الحالي بالمللي ثانية (Unix epoch)
    inline uint64_t now_ms()
    {
        using namespace std::chrono;
        return static_cast<uint64_t>(
            duration_cast<milliseconds>(
                system_clock::now().time_since_epoch())
                .count());
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) أكواد الأخطاء المشتركة
    // ─────────────────────────────────────────────────────────────────────

    enum class NetError : uint8_t
    {
        OK = 0,              // نجاح
        TIMEOUT,             // انتهاء المهلة
        CONNECTION_REFUSED,  // رفض الاتصال
        CONNECTION_RESET,    // إعادة تعيين الاتصال
        NETWORK_UNREACHABLE, // الشبكة غير قابلة للوصول
        ADDRESS_IN_USE,      // العنوان مُستخدم بالفعل
        BUFFER_OVERFLOW,     // تجاوز المخزن المؤقت
        INVALID_ARGUMENT,    // وسيط غير صحيح
        CRYPTO_ERROR,        // خطأ تشفيري
        PROTOCOL_ERROR,      // خطأ بروتوكولي
        NOT_FOUND,           // غير موجود
        PERMISSION_DENIED,   // مرفوض
        INTERNAL_ERROR       // خطأ داخلي
    };

    /// تحويل كود خطأ إلى نص عربي
    const char *error_to_arabic(NetError e);

    /// تحويل كود خطأ إلى نص إنجليزي
    const char *error_to_english(NetError e);

    // ─────────────────────────────────────────────────────────────────────
    // (AR) نتيجة مع خطأ (Result pattern)
    // ─────────────────────────────────────────────────────────────────────

    template <typename T>
    struct Result
    {
        T value;
        NetError error = NetError::OK;

        bool ok() const { return error == NetError::OK; }
        explicit operator bool() const { return ok(); }

        static Result success(T val) { return {std::move(val), NetError::OK}; }
        static Result fail(NetError e) { return {T{}, e}; }
    };

    /// تخصيص لنتيجة بدون قيمة
    template <>
    struct Result<void>
    {
        NetError error = NetError::OK;
        bool ok() const { return error == NetError::OK; }
        explicit operator bool() const { return ok(); }

        static Result success() { return {NetError::OK}; }
        static Result fail(NetError e) { return {e}; }
    };

} // namespace sad::net

// ─────────────────────────────────────────────────────────────────────
// (AR) تخصيص std::hash لـ NodeId و Endpoint
// ─────────────────────────────────────────────────────────────────────
namespace std
{
    template <>
    struct hash<sad::net::NodeId>
    {
        size_t operator()(const sad::net::NodeId &id) const noexcept
        {
            size_t h = 0;
            for (size_t i = 0; i < 8 && i < sad::net::NODE_ID_SIZE; i++)
                h = (h << 8) | id.data[i];
            return h;
        }
    };

    template <>
    struct hash<sad::net::Endpoint>
    {
        size_t operator()(const sad::net::Endpoint &ep) const noexcept
        {
            size_t h = std::hash<std::string>{}(ep.address);
            h ^= std::hash<uint16_t>{}(ep.port) << 1;
            return h;
        }
    };
} // namespace std

#endif // SAD_SADNET_CORE_TYPES_H
