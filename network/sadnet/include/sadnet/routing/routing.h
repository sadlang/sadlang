// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة التوجيه البصلي (Onion Routing)
// (AR) تمرير مجهول الهوية عبر سلسلة من 3+ عُقد وسيطة
//      كل عُقدة تفك طبقة تشفير واحدة فقط ولا تعرف المصدر والوجهة معاً
// (EN) Onion routing: anonymous relay through circuit of 3+ nodes// المكون: network/sadnet (التوجيه البصلي)
// المسار: network/sadnet/include/sadnet/routing/routing.h// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_ROUTING_ROUTING_H
#define SAD_SADNET_ROUTING_ROUTING_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace sad::net::routing
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::Endpoint;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using sad::net::crypto::KeyPair;
    using sad::net::crypto::X25519Key;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ثوابت التوجيه البصلي
    // ═════════════════════════════════════════════════════════════════════

    constexpr uint32_t DEFAULT_HOPS = 3;        // عدد القفزات الافتراضي
    constexpr uint32_t MAX_HOPS = 7;            // الحد الأقصى
    constexpr uint32_t CIRCUIT_TIMEOUT = 30000; // 30 ثانية
    constexpr size_t MAX_ONION_PAYLOAD = 4096;  // أقصى حجم للحمولة

    // ═════════════════════════════════════════════════════════════════════
    // (AR) أنواع خلايا البصل (Onion Cell Types)
    // ═════════════════════════════════════════════════════════════════════

    enum class CellType : uint8_t
    {
        CREATE = 1,   // إنشاء حلقة (DH handshake)
        CREATED = 2,  // تأكيد الإنشاء
        RELAY = 3,    // بيانات مُرحّلة
        DESTROY = 4,  // تدمير الحلقة
        EXTEND = 5,   // تمديد الحلقة عبر عُقدة جديدة
        EXTENDED = 6, // تأكيد التمديد
        DATA = 7,     // بيانات المستخدم النهائية
        PADDING = 8   // حشو (لإخفاء أنماط الحركة)
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) خلية بصلية (Onion Cell) — الوحدة الأساسية للنقل
    //      حجم ثابت لمنع التحليل الحجمي
    // ═════════════════════════════════════════════════════════════════════

    struct OnionCell
    {
        uint32_t circuit_id = 0; // معرف الحلقة
        CellType type = CellType::PADDING;
        Buffer payload;

        Buffer serialize() const;
        static OnionCell deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طبقة تشفير واحدة — مرتبطة بعُقدة واحدة في الحلقة
    // ═════════════════════════════════════════════════════════════════════

    struct CircuitHop
    {
        NodeId node_id;
        Endpoint endpoint;
        Buffer shared_secret; // السر المشترك (X25519)
        Buffer forward_key;   // مفتاح التشفير الأمامي
        Buffer backward_key;  // مفتاح التشفير الخلفي
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) حلقة بصلية (Circuit) — سلسلة من القفزات المشفرة
    // ═════════════════════════════════════════════════════════════════════

    class Circuit
    {
    public:
        Circuit();
        ~Circuit();

        // (AR) دعم النقل (move) — مطلوب لأن Impl يستخدم unique_ptr
        Circuit(Circuit &&other) noexcept;
        Circuit &operator=(Circuit &&other) noexcept;

        // (AR) منع النسخ
        Circuit(const Circuit &) = delete;
        Circuit &operator=(const Circuit &) = delete;

        // (AR) إضافة قفزة بعد تبادل المفاتيح
        void add_hop(const CircuitHop &hop);

        // (AR) تغليف البيانات بطبقات التشفير (من الأخير إلى الأول)
        Result<Buffer> wrap(const Buffer &plaintext) const;

        // (AR) فك طبقة تشفير واحدة (للعُقد الوسيطة)
        Result<Buffer> peel_layer(const Buffer &data, uint32_t hop_index) const;

        // (AR) معرف الحلقة
        uint32_t id() const;

        // (AR) عدد القفزات
        size_t hop_count() const;

        // (AR) هل الحلقة جاهزة للاستخدام؟
        bool is_ready() const;

        // (AR) الحصول على معلومات قفزة
        const CircuitHop &hop(size_t index) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) باني الحلقات — ينشئ حلقة بصلية تدريجياً
    // ═════════════════════════════════════════════════════════════════════

    // معالج لإرسال/استقبال الخلايا البصلية
    using CellSender = std::function<Result<Buffer>(const Endpoint &dest,
                                                    const Buffer &cell_data)>;

    class CircuitBuilder
    {
    public:
        CircuitBuilder(const KeyPair &identity, CellSender sender);
        ~CircuitBuilder();

        // (AR) بناء حلقة كاملة عبر قائمة من العُقد
        Result<Circuit> build(const std::vector<std::pair<NodeId, Endpoint>> &path);

        // (AR) تمديد حلقة موجودة بعُقدة جديدة
        Result<void> extend(Circuit &circuit, const NodeId &node, const Endpoint &ep);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) عُقدة ترحيل بصلية (Relay Node)
    //      تعالج الخلايا الواردة: تفك طبقة وتُعيد توجيه
    // ═════════════════════════════════════════════════════════════════════

    using RelayForwarder = std::function<void(const Endpoint &next,
                                              const Buffer &data)>;

    class RelayNode
    {
    public:
        RelayNode(const KeyPair &identity);
        ~RelayNode();

        // (AR) معالجة خلية واردة
        Result<void> process_cell(const OnionCell &cell,
                                  const Endpoint &from,
                                  RelayForwarder forwarder);

        // (AR) عدد الحلقات النشطة
        size_t active_circuits() const;

        // (AR) تنظيف الحلقات المنتهية
        void cleanup_expired();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير التوجيه المجهول — واجهة عالية المستوى
    // ═════════════════════════════════════════════════════════════════════

    class OnionRouter
    {
    public:
        OnionRouter(const KeyPair &identity, uint32_t num_hops = DEFAULT_HOPS);
        ~OnionRouter();

        // (AR) إرسال بيانات مجهولة الهوية عبر حلقة بصلية
        Result<Buffer> send_anonymous(const Endpoint &destination,
                                      const Buffer &data,
                                      CellSender sender);

        // (AR) إنشاء حلقة مسبقاً (للإرسال السريع لاحقاً)
        Result<Circuit> create_circuit(const std::vector<std::pair<NodeId, Endpoint>> &path,
                                       CellSender sender);

        // (AR) إرسال عبر حلقة موجودة
        Result<Buffer> send_via_circuit(Circuit &circuit,
                                        const Buffer &data,
                                        CellSender sender);

        // (AR) اختيار مسار عشوائي من العُقد المتاحة
        std::vector<std::pair<NodeId, Endpoint>> select_path(
            const std::vector<std::pair<NodeId, Endpoint>> &available_nodes) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::routing

#endif // SAD_SADNET_ROUTING_ROUTING_H
