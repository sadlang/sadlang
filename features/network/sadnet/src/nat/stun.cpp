// ═══════════════════════════════════════════════════════════════════
// صNet — عميل STUN: طلب الربط واكتشاف نوع NAT
// (AR) تنفيذ RFC 5389 (STUN) مع دعم XOR-MAPPED-ADDRESS
//      واكتشاف نوع NAT عبر خوارزمية RFC 3489
// (EN) STUN client: binding request, XOR-MAPPED-ADDRESS, NAT type
// المكون: features/network/sadnet (عميل STUN)
// المسار: features/network/sadnet/src/nat/stun.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/nat/nat.h"
#include "sadnet/crypto/crypto.h"
#include <cstring>

namespace sad::net::nat
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مساعدات البايتات — ترميز وفك ترميز big-endian
    // ═════════════════════════════════════════════════════════════════════

    static void write_u16_be(uint8_t *p, uint16_t v)
    {
        p[0] = (uint8_t)(v >> 8);
        p[1] = (uint8_t)(v);
    }

    static void write_u32_be(uint8_t *p, uint32_t v)
    {
        p[0] = (uint8_t)(v >> 24);
        p[1] = (uint8_t)(v >> 16);
        p[2] = (uint8_t)(v >> 8);
        p[3] = (uint8_t)(v);
    }

    static uint16_t read_u16_be(const uint8_t *p)
    {
        return ((uint16_t)p[0] << 8) | p[1];
    }

    static uint32_t read_u32_be(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
               ((uint32_t)p[2] << 8) | p[3];
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) بناء طلب STUN Binding Request
    //      الهيكل: 20 بايت رأس + سمات اختيارية
    //      رأس: [نوع:2][طول:2][كوكي:4][معرف:12]
    // ═════════════════════════════════════════════════════════════════════

    Buffer StunClient::build_binding_request(uint8_t transaction_id[12],
                                             uint32_t change_flags)
    {
        // الحد الأقصى: رأس 20 + سمة CHANGE-REQUEST 8
        Buffer packet(20 + (change_flags ? 8 : 0));
        uint8_t *p = packet.data();

        // نوع الرسالة: Binding Request
        write_u16_be(p, STUN_BINDING_REQUEST);

        // طول الجسم (بدون الرأس)
        uint16_t body_len = change_flags ? 8 : 0;
        write_u16_be(p + 2, body_len);

        // Magic Cookie
        write_u32_be(p + 4, STUN_MAGIC_COOKIE);

        // معرف المعاملة (12 بايت)
        std::memcpy(p + 8, transaction_id, 12);

        // سمة CHANGE-REQUEST (إذا مطلوبة)
        if (change_flags)
        {
            uint8_t *attr = p + 20;
            write_u16_be(attr, STUN_ATTR_CHANGE_REQUEST);
            write_u16_be(attr + 2, 4); // طول القيمة
            write_u32_be(attr + 4, change_flags);
        }

        return packet;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تحليل استجابة STUN
    //      البحث عن XOR-MAPPED-ADDRESS أو MAPPED-ADDRESS
    // ═════════════════════════════════════════════════════════════════════

    StunResult StunClient::parse_binding_response(const uint8_t *data, size_t len,
                                                  const uint8_t transaction_id[12])
    {
        StunResult result;
        result.success = false;

        // تحقق من الحد الأدنى للطول (20 بايت رأس)
        if (len < 20)
            return result;

        // تحقق من نوع الرسالة
        uint16_t msg_type = read_u16_be(data);
        if (msg_type != STUN_BINDING_RESPONSE)
            return result;

        uint16_t msg_len = read_u16_be(data + 2);
        uint32_t cookie = read_u32_be(data + 4);

        // تحقق من Magic Cookie
        if (cookie != STUN_MAGIC_COOKIE)
            return result;

        // تحقق من معرف المعاملة
        if (std::memcmp(data + 8, transaction_id, 12) != 0)
            return result;

        // تحقق من تطابق طول الجسم
        if (msg_len + 20 > len)
            return result;

        // تحليل السمات
        size_t offset = 20;
        while (offset + 4 <= 20 + msg_len)
        {
            uint16_t attr_type = read_u16_be(data + offset);
            uint16_t attr_len = read_u16_be(data + offset + 2);
            offset += 4;

            if (offset + attr_len > len)
                break;

            if (attr_type == STUN_ATTR_XOR_MAPPED)
            {
                // XOR-MAPPED-ADDRESS: [padding:1][family:1][xor_port:2][xor_ip:4]
                if (attr_len >= 8)
                {
                    uint8_t family = data[offset + 1];
                    if (family == 0x01)
                    { // IPv4
                        uint16_t xor_port = read_u16_be(data + offset + 2) ^ (uint16_t)(STUN_MAGIC_COOKIE >> 16);
                        uint32_t xor_ip = read_u32_be(data + offset + 4) ^ STUN_MAGIC_COOKIE;

                        result.mapped_address.address =
                            std::to_string((xor_ip >> 24) & 0xFF) + "." +
                            std::to_string((xor_ip >> 16) & 0xFF) + "." +
                            std::to_string((xor_ip >> 8) & 0xFF) + "." +
                            std::to_string(xor_ip & 0xFF);
                        result.mapped_address.port = xor_port;
                        result.success = true;
                        return result;
                    }
                }
            }
            else if (attr_type == STUN_ATTR_MAPPED_ADDR && !result.success)
            {
                // MAPPED-ADDRESS: [padding:1][family:1][port:2][ip:4]
                if (attr_len >= 8)
                {
                    uint8_t family = data[offset + 1];
                    if (family == 0x01)
                    { // IPv4
                        uint16_t port = read_u16_be(data + offset + 2);
                        uint32_t ip = read_u32_be(data + offset + 4);

                        result.mapped_address.address =
                            std::to_string((ip >> 24) & 0xFF) + "." +
                            std::to_string((ip >> 16) & 0xFF) + "." +
                            std::to_string((ip >> 8) & 0xFF) + "." +
                            std::to_string(ip & 0xFF);
                        result.mapped_address.port = port;
                        result.success = true;
                        // لا نرجع فوراً — ربما نجد XOR-MAPPED بعدها
                    }
                }
            }

            // محاذاة السمة على 4 بايتات
            offset += attr_len;
            if (attr_len % 4 != 0)
                offset += 4 - (attr_len % 4);
        }

        return result;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طلب ربط STUN مع إعادة محاولة
    // ═════════════════════════════════════════════════════════════════════

    StunResult StunClient::binding_request(UdpSocket &sock, const Endpoint &stun_server)
    {
        // إنشاء معرف معاملة عشوائي
        uint8_t tx_id[12];
        crypto::random_bytes(tx_id, 12);

        auto packet = build_binding_request(tx_id);

        // إعادة المحاولة (3 مرات مع زيادة المهلة)
        uint32_t timeouts_ms[] = {500, 1000, 2000};

        for (uint32_t timeout : timeouts_ms)
        {
            sock.set_recv_timeout(timeout);
            auto send_r = sock.send_to(stun_server, packet);
            if (!send_r.ok())
                continue;

            auto recv_r = sock.recv_from();
            if (!recv_r.ok())
                continue;

            auto result = parse_binding_response(
                recv_r.value.data.data(), recv_r.value.data.size(), tx_id);

            if (result.success)
                return result;
        }

        StunResult fail;
        return fail;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) اكتشاف نوع NAT — خوارزمية RFC 3489 المبسطة
    //
    // الخوارزمية:
    // 1. أرسل Binding Request عادي → هل يصل؟
    //    - لا → UNKNOWN (ربما UDP محجوب)
    //    - نعم → قارن mapped مع local
    //      - متطابق → OPEN
    //      - مختلف → اختبار 2
    // 2. أرسل مع CHANGE-IP | CHANGE-PORT → هل يصل؟
    //    - نعم → FULL_CONE
    //    - لا → اختبار 3
    // 3. أرسل Binding Request لخادم آخر → قارن mapped
    //    - مختلف عن 1 → SYMMETRIC
    //    - متطابق → اختبار 4
    // 4. أرسل مع CHANGE-PORT فقط → هل يصل؟
    //    - نعم → RESTRICTED_CONE
    //    - لا → PORT_RESTRICTED
    // ═════════════════════════════════════════════════════════════════════

    NATType StunClient::detect_nat_type(UdpSocket &sock,
                                        const Endpoint &server1,
                                        const Endpoint &server2)
    {
        // الاختبار 1: طلب ربط عادي
        auto result1 = binding_request(sock, server1);
        if (!result1.success)
            return NATType::UNKNOWN;

        // هل نتيجة الربط هي عنواننا المحلي؟
        // (لا نملك طريقة سهلة لمعرفة عنواننا المحلي — نفترض أنه مختلف)
        // في التنفيذ الكامل، قارن مع getsockname

        // الاختبار 2: طلب مع تغيير العنوان والمنفذ
        uint8_t tx2[12];
        crypto::random_bytes(tx2, 12);
        auto pkt2 = build_binding_request(tx2, 0x06); // CHANGE_IP | CHANGE_PORT

        sock.set_recv_timeout(2000);
        sock.send_to(server1, pkt2);
        auto recv2 = sock.recv_from();

        if (recv2.ok())
        {
            auto r2 = parse_binding_response(recv2.value.data.data(),
                                             recv2.value.data.size(), tx2);
            if (r2.success)
                return NATType::FULL_CONE;
        }

        // الاختبار 3: طلب ربط من خادم آخر
        auto result3 = binding_request(sock, server2);
        if (!result3.success)
            return NATType::UNKNOWN;

        // مقارنة العناوين المكتشفة
        if (result1.mapped_address.address != result3.mapped_address.address ||
            result1.mapped_address.port != result3.mapped_address.port)
        {
            return NATType::SYMMETRIC;
        }

        // الاختبار 4: طلب مع تغيير المنفذ فقط
        uint8_t tx4[12];
        crypto::random_bytes(tx4, 12);
        auto pkt4 = build_binding_request(tx4, 0x02); // CHANGE_PORT فقط

        sock.set_recv_timeout(2000);
        sock.send_to(server1, pkt4);
        auto recv4 = sock.recv_from();

        if (recv4.ok())
        {
            auto r4 = parse_binding_response(recv4.value.data.data(),
                                             recv4.value.data.size(), tx4);
            if (r4.success)
                return NATType::RESTRICTED_CONE;
        }

        return NATType::PORT_RESTRICTED;
    }

} // namespace sad::net::nat
