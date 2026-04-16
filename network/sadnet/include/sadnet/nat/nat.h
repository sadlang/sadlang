// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة اجتياز NAT: STUN و TURN و ICE
// (AR) اكتشاف نوع NAT، الحصول على عنوان خارجي، إنشاء قنوات TURN،
//      تجميع مرشحات ICE وفحص الاتصال
// (EN) NAT traversal: STUN binding, TURN relay, ICE connectivity// المكون: network/sadnet (اجتياز NAT)
// المسار: network/sadnet/include/sadnet/nat/nat.h// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_NAT_NAT_H
#define SAD_SADNET_NAT_NAT_H

#include "sadnet/core/types.h"
#include "sadnet/core/transport.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sad::net::nat
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::Endpoint;
    using sad::net::NetError;
    using sad::net::Result;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) أنواع NAT المكتشفة
    // ═════════════════════════════════════════════════════════════════════

    enum class NATType : uint8_t
    {
        OPEN = 0,            // لا يوجد NAT — الاتصال المباشر ممكن
        FULL_CONE = 1,       // مخروط كامل — أي عنوان خارجي يصل
        RESTRICTED_CONE = 2, // مخروط مقيد — فقط العناوين المعروفة
        PORT_RESTRICTED = 3, // مقيد بالمنفذ — العنوان + المنفذ معاً
        SYMMETRIC = 4,       // متماثل — منفذ مختلف لكل وجهة (أصعب اجتياز)
        UNKNOWN = 5
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) بروتوكول STUN — RFC 5389
    //      طلب ربط بسيط للحصول على العنوان الخارجي (Mapped Address)
    // ═════════════════════════════════════════════════════════════════════

    // ثوابت STUN
    constexpr uint16_t STUN_BINDING_REQUEST = 0x0001;
    constexpr uint16_t STUN_BINDING_RESPONSE = 0x0101;
    constexpr uint32_t STUN_MAGIC_COOKIE = 0x2112A442;
    constexpr uint16_t STUN_ATTR_MAPPED_ADDR = 0x0001;
    constexpr uint16_t STUN_ATTR_XOR_MAPPED = 0x0020;
    constexpr uint16_t STUN_ATTR_CHANGE_REQUEST = 0x0003;
    constexpr uint16_t STUN_ATTR_ERROR_CODE = 0x0009;

    // نتيجة طلب STUN
    struct StunResult
    {
        bool success = false;
        Endpoint mapped_address; // العنوان الخارجي المكتشف
        NATType nat_type = NATType::UNKNOWN;
    };

    // عميل STUN
    class StunClient
    {
    public:
        // (AR) إرسال طلب ربط (Binding Request) والحصول على العنوان الخارجي
        // (EN) Send binding request to STUN server, get external mapped address
        static StunResult binding_request(UdpSocket &sock, const Endpoint &stun_server);

        // (AR) اكتشاف نوع NAT باستخدام خوارزمية RFC 3489
        //      يحتاج خادمين STUN على عنوانين مختلفين
        // (EN) Detect NAT type using RFC 3489 algorithm (needs 2 STUN servers)
        static NATType detect_nat_type(UdpSocket &sock,
                                       const Endpoint &server1,
                                       const Endpoint &server2);

        // (AR) بناء حزمة طلب STUN
        static Buffer build_binding_request(uint8_t transaction_id[12],
                                            uint32_t change_flags = 0);

        // (AR) تحليل استجابة STUN
        static StunResult parse_binding_response(const uint8_t *data, size_t len,
                                                 const uint8_t transaction_id[12]);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) بروتوكول TURN — RFC 5766
    //      ترحيل البيانات عبر خادم وسيط عندما يفشل STUN
    // ═════════════════════════════════════════════════════════════════════

    // أنواع رسائل TURN
    constexpr uint16_t TURN_ALLOCATE_REQUEST = 0x0003;
    constexpr uint16_t TURN_ALLOCATE_RESPONSE = 0x0103;
    constexpr uint16_t TURN_REFRESH_REQUEST = 0x0004;
    constexpr uint16_t TURN_REFRESH_RESPONSE = 0x0104;
    constexpr uint16_t TURN_SEND_INDICATION = 0x0016;
    constexpr uint16_t TURN_DATA_INDICATION = 0x0017;
    constexpr uint16_t TURN_CREATE_PERM_REQ = 0x0008;
    constexpr uint16_t TURN_CREATE_PERM_RESP = 0x0108;
    constexpr uint16_t TURN_CHANNEL_BIND_REQ = 0x0009;
    constexpr uint16_t TURN_CHANNEL_BIND_RESP = 0x0109;

    // سمات TURN
    constexpr uint16_t TURN_ATTR_LIFETIME = 0x000D;
    constexpr uint16_t TURN_ATTR_XOR_PEER_ADDR = 0x0012;
    constexpr uint16_t TURN_ATTR_DATA = 0x0013;
    constexpr uint16_t TURN_ATTR_XOR_RELAY_ADDR = 0x0016;
    constexpr uint16_t TURN_ATTR_REQUESTED_TRANSPORT = 0x0019;
    constexpr uint16_t TURN_ATTR_CHANNEL_NUMBER = 0x000C;

    // معلومات التخصيص
    struct TurnAllocation
    {
        Endpoint relay_address;  // العنوان المرحّل
        uint32_t lifetime = 600; // العمر بالثواني
        uint64_t expires_at = 0; // وقت الانتهاء
        bool valid = false;
    };

    // عميل TURN
    class TurnClient
    {
    public:
        TurnClient(UdpSocket &sock, const Endpoint &turn_server);
        ~TurnClient();

        // (AR) تخصيص عنوان مرحّل (Allocate)
        Result<TurnAllocation> allocate();

        // (AR) تجديد التخصيص
        Result<void> refresh(uint32_t lifetime = 600);

        // (AR) إنشاء إذن (CreatePermission) لعنوان نظير
        Result<void> create_permission(const Endpoint &peer);

        // (AR) ربط قناة (ChannelBind) لنقل بيانات سريع
        Result<uint16_t> channel_bind(const Endpoint &peer);

        // (AR) إرسال بيانات عبر القناة المربوطة
        Result<void> send_via_channel(uint16_t channel, const Buffer &data);

        // (AR) إرسال عبر مؤشر الإرسال (SendIndication)
        Result<void> send_indication(const Endpoint &peer, const Buffer &data);

        // (AR) هل التخصيص صالح؟
        bool is_allocated() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) بروتوكول ICE — RFC 8445
    //      تجميع المرشحين، فحص الاتصال (Connectivity Check)
    // ═════════════════════════════════════════════════════════════════════

    // أنواع مرشحات ICE
    enum class CandidateType : uint8_t
    {
        HOST = 0,  // عنوان محلي مباشر
        SRFLX = 1, // عنوان مكتشف عبر STUN (Server Reflexive)
        RELAY = 2, // عنوان مرحّل عبر TURN
        PRFLX = 3  // عنوان مكتشف أثناء الفحص (Peer Reflexive)
    };

    struct ICECandidate
    {
        CandidateType type;
        Endpoint address;
        uint32_t priority = 0;
        std::string foundation;    // معرف فريد للمرشح
        uint32_t component_id = 1; // 1 = RTP

        // (AR) حساب الأولوية حسب RFC 8445 Section 5.1.2.1
        static uint32_t compute_priority(CandidateType type, uint16_t local_pref = 65535);
    };

    // زوج مرشحين
    struct CandidatePair
    {
        ICECandidate local;
        ICECandidate remote;
        uint64_t priority = 0; // الأولوية المركبة
        enum class State
        {
            FROZEN,
            WAITING,
            IN_PROGRESS,
            SUCCEEDED,
            FAILED
        } state = State::FROZEN;

        // (AR) حساب الأولوية المركبة حسب RFC 8445
        static uint64_t compute_pair_priority(uint32_t controlling_prio,
                                              uint32_t controlled_prio,
                                              bool is_controlling);
    };

    // حالة ICE
    enum class ICEState : uint8_t
    {
        NEW,
        GATHERING,
        CHECKING,
        CONNECTED,
        COMPLETED,
        FAILED,
        DISCONNECTED
    };

    // معالج أحداث ICE
    using ICEStateCallback = std::function<void(ICEState state, const CandidatePair *pair)>;

    // وكيل ICE
    class ICEAgent
    {
    public:
        ICEAgent(bool controlling = true);
        ~ICEAgent();

        // (AR) تجميع المرشحين المحليين
        Result<std::vector<ICECandidate>> gather_candidates(
            uint16_t local_port,
            const Endpoint *stun_server = nullptr,
            TurnClient *turn_client = nullptr);

        // (AR) إضافة مرشح بعيد
        void add_remote_candidate(const ICECandidate &candidate);

        // (AR) بدء فحوصات الاتصال
        Result<void> start_checks(UdpSocket &sock);

        // (AR) الحصول على أفضل زوج متصل
        const CandidatePair *selected_pair() const;

        // (AR) حالة ICE الحالية
        ICEState state() const;

        // (AR) تسجيل معالج أحداث
        void on_state_change(ICEStateCallback callback);

        // (AR) إلغاء وإيقاف
        void cancel();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::nat

#endif // SAD_SADNET_NAT_NAT_H
