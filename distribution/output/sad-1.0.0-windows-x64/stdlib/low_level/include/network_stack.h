/*
 * ============================================================================
 *  وحدة الشبكة المنخفضة المستوى — لغة ص
 *  Low-Level Network Stack Module — Sad Language
 * ============================================================================
 *
 * @brief (AR) وحدة شبكة منخفضة المستوى بدون نظام تشغيل
 *             تدعم: Ethernet II، ARP، IPv4، ICMP، UDP
 *             تعمل مباشرة مع بطاقات الشبكة عبر MMIO/PCI
 *
 * @brief (EN) Bare-metal network stack module
 *             Supports: Ethernet II, ARP, IPv4, ICMP, UDP
 *             Works directly with NICs via MMIO/PCI
 *
 * @details
 * (AR) هذه الوحدة توفر مكدس شبكة بسيط للعمل بدون نظام تشغيل.
 *      على سطح المكتب تعمل كمحاكاة. في بيئة النواة تصل مباشرة للعتاد.
 *
 *      الطبقات المدعومة:
 *        1. Ethernet — إرسال واستقبال إطارات خام
 *        2. ARP — ترجمة عناوين IP إلى MAC
 *        3. IPv4 — إرسال واستقبال حزم IP
 *        4. ICMP — رسائل ping/pong
 *        5. UDP — إرسال واستقبال حزم UDP بسيطة
 *
 * (EN) Simple bare-metal network stack. Simulated on desktop,
 *      direct hardware access on bare-metal.
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_NETWORK_STACK_H
#define SAD_LOW_LEVEL_NETWORK_STACK_H

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) ثوابت الشبكة / (EN) Network Constants
// ============================================================================
namespace NetConstants {
    constexpr int MAC_SIZE = 6;
    constexpr int IPV4_SIZE = 4;
    constexpr int ETH_HEADER_SIZE = 14;
    constexpr int IPV4_HEADER_SIZE = 20;
    constexpr int UDP_HEADER_SIZE = 8;
    constexpr int ICMP_HEADER_SIZE = 8;
    constexpr int ARP_PACKET_SIZE = 28;
    constexpr int MTU = 1500;
    constexpr uint16_t ETHERTYPE_IPV4 = 0x0800;
    constexpr uint16_t ETHERTYPE_ARP  = 0x0806;
    constexpr uint8_t PROTO_ICMP = 1;
    constexpr uint8_t PROTO_UDP  = 17;
    constexpr uint8_t PCI_CLASS_NETWORK = 0x02;
}

// ============================================================================
// (AR) حالة بطاقة الشبكة / (EN) NIC State
// ============================================================================
enum class NICState : uint8_t {
    NOT_FOUND    = 0,   // (AR) غير موجودة / (EN) Not found
    FOUND        = 1,   // (AR) موجودة لكن غير مهيأة / (EN) Found but not init
    INITIALIZED  = 2,   // (AR) مهيأة / (EN) Initialized
    LINK_UP      = 3,   // (AR) الارتباط فعال / (EN) Link is up
    LINK_DOWN    = 4,   // (AR) الارتباط معطل / (EN) Link is down
    ERROR_STATE  = 5    // (AR) خطأ / (EN) Error
};

// ============================================================================
// (AR) معلومات بطاقة الشبكة / (EN) NIC Information
// ============================================================================
struct NICInfo {
    int id;                     // (AR) معرّف البطاقة / (EN) NIC ID
    NICState nicState;          // (AR) حالة البطاقة / (EN) NIC state
    uint8_t mac[6];             // (AR) عنوان MAC / (EN) MAC address
    uint8_t ip[4];              // (AR) عنوان IP / (EN) IP address
    uint8_t subnet[4];          // (AR) قناع الشبكة / (EN) Subnet mask
    uint8_t gateway[4];         // (AR) البوابة / (EN) Gateway
    std::string model;          // (AR) اسم الموديل / (EN) Model name
    uint16_t vendorId;          // (AR) معرّف المصنّع / (EN) Vendor ID
    uint16_t deviceId;          // (AR) معرّف الجهاز / (EN) Device ID
    uint64_t txPackets;         // (AR) الحزم المرسلة / (EN) Transmitted packets
    uint64_t rxPackets;         // (AR) الحزم المستقبلة / (EN) Received packets
    uint64_t txBytes;           // (AR) البايتات المرسلة / (EN) Bytes sent
    uint64_t rxBytes;           // (AR) البايتات المستقبلة / (EN) Bytes received
    int linkSpeed;              // (AR) سرعة الارتباط (Mbps) / (EN) Link speed
};

// ============================================================================
// (AR) إدخال ARP / (EN) ARP Entry
// ============================================================================
struct ARPEntry {
    uint8_t ip[4];
    uint8_t mac[6];
    bool valid;
    int ttl;    // (AR) وقت البقاء (ثوانٍ) / (EN) Time to live (seconds)
};

// ============================================================================
// (AR) إحصائيات الشبكة / (EN) Network Statistics
// ============================================================================
struct NetStats {
    uint64_t totalTx;
    uint64_t totalRx;
    uint64_t txErrors;
    uint64_t rxErrors;
    uint64_t arpRequests;
    uint64_t arpReplies;
    uint64_t icmpSent;
    uint64_t icmpReceived;
    uint64_t udpSent;
    uint64_t udpReceived;
};

// ============================================================================
// (AR) مدير الشبكة — نمط المفرد / (EN) Network Manager — Singleton
// ============================================================================
class NetworkManager {
public:
    static NetworkManager& getInstance() {
        static NetworkManager instance;
        return instance;
    }

    // ════════════════════════════════════════════════════════════════
    // (AR) 1. اكتشاف وتهيئة / (EN) 1. Discovery & Init
    // ════════════════════════════════════════════════════════════════
    int scanNICs();
    int getNICCount() const { return static_cast<int>(nics_.size()); }
    NICInfo getNICInfo(int nicId) const;
    int initNIC(int nicId);
    std::string generateReport() const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 2. تهيئة IP / (EN) 2. IP Configuration
    // ════════════════════════════════════════════════════════════════
    int setIP(int nicId, const std::string& ip);
    int setSubnet(int nicId, const std::string& mask);
    int setGateway(int nicId, const std::string& gw);
    std::string getIP(int nicId) const;
    std::string getMAC(int nicId) const;
    std::string getSubnet(int nicId) const;
    std::string getGateway(int nicId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 3. ARP / (EN) 3. ARP
    // ════════════════════════════════════════════════════════════════
    int arpRequest(int nicId, const std::string& targetIP);
    std::string arpLookup(const std::string& ip) const;
    int getARPTableSize() const { return static_cast<int>(arpTable_.size()); }
    int clearARPTable();

    // ════════════════════════════════════════════════════════════════
    // (AR) 4. ICMP / (EN) 4. ICMP (Ping)
    // ════════════════════════════════════════════════════════════════
    int ping(int nicId, const std::string& targetIP);
    int getLastPingTime() const { return lastPingMs_; }

    // ════════════════════════════════════════════════════════════════
    // (AR) 5. UDP / (EN) 5. UDP
    // ════════════════════════════════════════════════════════════════
    int udpSend(int nicId, const std::string& destIP, int destPort,
                int srcPort, const std::string& data);
    std::string udpReceive(int nicId, int port);
    int udpBind(int nicId, int port);
    int udpUnbind(int nicId, int port);

    // ════════════════════════════════════════════════════════════════
    // (AR) 6. إرسال خام / (EN) 6. Raw Send
    // ════════════════════════════════════════════════════════════════
    int sendRawFrame(int nicId, const std::string& hexData);

    // ════════════════════════════════════════════════════════════════
    // (AR) 7. إحصائيات / (EN) 7. Statistics
    // ════════════════════════════════════════════════════════════════
    NetStats getStats() const { return stats_; }
    bool isLinkUp(int nicId) const;
    int getLinkSpeed(int nicId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 8. إعادة تعيين / (EN) 8. Reset
    // ════════════════════════════════════════════════════════════════
    void reset();

private:
    NetworkManager();
    ~NetworkManager() = default;
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    // (AR) تحليل عنوان IP نصي / (EN) Parse IP string
    bool parseIP(const std::string& s, uint8_t out[4]) const;
    std::string formatIP(const uint8_t ip[4]) const;
    std::string formatMAC(const uint8_t mac[6]) const;

    std::vector<NICInfo> nics_;
    std::vector<ARPEntry> arpTable_;
    NetStats stats_;
    int lastPingMs_;
    std::vector<std::pair<int, std::string>> udpBuffers_; // (AR) port → data
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_NETWORK_STACK_H
