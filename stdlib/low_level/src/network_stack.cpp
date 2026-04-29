/**
 * @file network_stack.cpp
 * @brief (AR) تنفيذ مكدس الشبكة المنخفض المستوى
 *        محاكاة آمنة مع دعم ARP cache، IP، ICMP ping، UDP
 * @brief (EN) Low-level network stack implementation
 *        Safe simulation with ARP cache, IP, ICMP ping, UDP
 */

#include "network_stack.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) المُنشئ / (EN) Constructor
// ============================================================================
NetworkManager::NetworkManager() : lastPingMs_(0) {
    std::memset(&stats_, 0, sizeof(stats_));
}

// ============================================================================
// (AR) تحليل عناوين / (EN) Address parsing helpers
// ============================================================================

bool NetworkManager::parseIP(const std::string& s, uint8_t out[4]) const {
    int parts[4] = {0, 0, 0, 0};
    int idx = 0;
    std::string num;
    for (char c : s) {
        if (c == '.') {
            if (idx >= 3 || num.empty()) return false;
            parts[idx++] = std::stoi(num);
            num.clear();
        } else if (c >= '0' && c <= '9') {
            num += c;
        } else {
            return false;
        }
    }
    if (idx != 3 || num.empty()) return false;
    parts[3] = std::stoi(num);
    for (int i = 0; i < 4; i++) {
        if (parts[i] < 0 || parts[i] > 255) return false;
        out[i] = static_cast<uint8_t>(parts[i]);
    }
    return true;
}

std::string NetworkManager::formatIP(const uint8_t ip[4]) const {
    std::ostringstream ss;
    ss << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3];
    return ss.str();
}

std::string NetworkManager::formatMAC(const uint8_t mac[6]) const {
    std::ostringstream ss;
    for (int i = 0; i < 6; i++) {
        if (i > 0) ss << ":";
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)mac[i];
    }
    return ss.str();
}

// ============================================================================
// (AR) 1. اكتشاف وتهيئة / (EN) 1. Discovery & Init
// ============================================================================

int NetworkManager::scanNICs() {
    nics_.clear();

    // (AR) بطاقة شبكة محاكاة — Intel I219-V
    // (EN) Simulated NIC — Intel I219-V
    NICInfo nic;
    nic.id = 0;
    nic.nicState = NICState::FOUND;
    nic.mac[0] = 0x00; nic.mac[1] = 0x1A; nic.mac[2] = 0x2B;
    nic.mac[3] = 0x3C; nic.mac[4] = 0x4D; nic.mac[5] = 0x5E;
    nic.ip[0] = 0; nic.ip[1] = 0; nic.ip[2] = 0; nic.ip[3] = 0;
    nic.subnet[0] = 255; nic.subnet[1] = 255; nic.subnet[2] = 255; nic.subnet[3] = 0;
    nic.gateway[0] = 0; nic.gateway[1] = 0; nic.gateway[2] = 0; nic.gateway[3] = 0;
    nic.model = "Intel I219-V Gigabit";
    nic.vendorId = 0x8086;
    nic.deviceId = 0x15B8;
    nic.txPackets = 0;
    nic.rxPackets = 0;
    nic.txBytes = 0;
    nic.rxBytes = 0;
    nic.linkSpeed = 1000;
    nics_.push_back(nic);

    // (AR) بطاقة ثانية — Realtek RTL8111
    // (EN) Second NIC — Realtek RTL8111
    NICInfo nic2;
    nic2.id = 1;
    nic2.nicState = NICState::FOUND;
    nic2.mac[0] = 0xAA; nic2.mac[1] = 0xBB; nic2.mac[2] = 0xCC;
    nic2.mac[3] = 0xDD; nic2.mac[4] = 0xEE; nic2.mac[5] = 0xFF;
    nic2.ip[0] = 0; nic2.ip[1] = 0; nic2.ip[2] = 0; nic2.ip[3] = 0;
    nic2.subnet[0] = 255; nic2.subnet[1] = 255; nic2.subnet[2] = 255; nic2.subnet[3] = 0;
    nic2.gateway[0] = 0; nic2.gateway[1] = 0; nic2.gateway[2] = 0; nic2.gateway[3] = 0;
    nic2.model = "Realtek RTL8111H";
    nic2.vendorId = 0x10EC;
    nic2.deviceId = 0x8168;
    nic2.txPackets = 0;
    nic2.rxPackets = 0;
    nic2.txBytes = 0;
    nic2.rxBytes = 0;
    nic2.linkSpeed = 100;
    nics_.push_back(nic2);

    return Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size");
}

NICInfo NetworkManager::getNICInfo(int nicId) const {
    if (nicId >= 0 && nicId < Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size"))
        return nics_[nicId];
    NICInfo empty;
    empty.id = -1;
    empty.nicState = NICState::NOT_FOUND;
    std::memset(empty.mac, 0, 6);
    std::memset(empty.ip, 0, 4);
    std::memset(empty.subnet, 0, 4);
    std::memset(empty.gateway, 0, 4);
    empty.model = "";
    empty.vendorId = 0;
    empty.deviceId = 0;
    empty.txPackets = 0;
    empty.rxPackets = 0;
    empty.txBytes = 0;
    empty.rxBytes = 0;
    empty.linkSpeed = 0;
    return empty;
}

int NetworkManager::initNIC(int nicId) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    nics_[nicId].nicState = NICState::LINK_UP;
    return 0;
}

std::string NetworkManager::generateReport() const {
    std::ostringstream ss;
    ss << "=== \xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9 / Network Report ===\n";
    ss << "\xd8\xb9\xd8\xaf\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa7\xd8\xaa / NIC count: " << nics_.size() << "\n\n";
    for (const auto& n : nics_) {
        ss << "--- NIC #" << n.id << " ---\n";
        ss << "  Model: " << n.model << "\n";
        ss << "  MAC: " << formatMAC(n.mac) << "\n";
        ss << "  IP: " << formatIP(n.ip) << "\n";
        ss << "  Link: " << n.linkSpeed << " Mbps\n";
        ss << "  State: " << static_cast<int>(n.nicState) << "\n\n";
    }
    return ss.str();
}

// ============================================================================
// (AR) 2. تهيئة IP / (EN) 2. IP Configuration
// ============================================================================

int NetworkManager::setIP(int nicId, const std::string& ip) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    if (!parseIP(ip, nics_[nicId].ip)) return -1;
    return 0;
}

int NetworkManager::setSubnet(int nicId, const std::string& mask) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    if (!parseIP(mask, nics_[nicId].subnet)) return -1;
    return 0;
}

int NetworkManager::setGateway(int nicId, const std::string& gw) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    if (!parseIP(gw, nics_[nicId].gateway)) return -1;
    return 0;
}

std::string NetworkManager::getIP(int nicId) const {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return "0.0.0.0";
    return formatIP(nics_[nicId].ip);
}

std::string NetworkManager::getMAC(int nicId) const {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return "00:00:00:00:00:00";
    return formatMAC(nics_[nicId].mac);
}

std::string NetworkManager::getSubnet(int nicId) const {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return "0.0.0.0";
    return formatIP(nics_[nicId].subnet);
}

std::string NetworkManager::getGateway(int nicId) const {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return "0.0.0.0";
    return formatIP(nics_[nicId].gateway);
}

// ============================================================================
// (AR) 3. ARP / (EN) 3. ARP
// ============================================================================

int NetworkManager::arpRequest(int nicId, const std::string& targetIP) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    stats_.arpRequests++;

    uint8_t ip[4];
    if (!parseIP(targetIP, ip)) return -1;

    // (AR) محاكاة: إضافة إدخال ARP وهمي / (EN) Simulation: add dummy ARP entry
    ARPEntry entry;
    std::memcpy(entry.ip, ip, 4);
    // (AR) MAC وهمي مبني على IP / (EN) Fake MAC derived from IP
    entry.mac[0] = 0x02;
    entry.mac[1] = 0x00;
    entry.mac[2] = ip[0];
    entry.mac[3] = ip[1];
    entry.mac[4] = ip[2];
    entry.mac[5] = ip[3];
    entry.valid = true;
    entry.ttl = 300;

    // (AR) تحقق من الوجود أولاً / (EN) Check if already exists
    for (auto& e : arpTable_) {
        if (std::memcmp(e.ip, ip, 4) == 0) {
            e = entry;
            stats_.arpReplies++;
            return 0;
        }
    }
    arpTable_.push_back(entry);
    stats_.arpReplies++;
    return 0;
}

std::string NetworkManager::arpLookup(const std::string& ip) const {
    uint8_t ipBytes[4];
    if (!parseIP(ip, ipBytes)) return "";
    for (const auto& e : arpTable_) {
        if (e.valid && std::memcmp(e.ip, ipBytes, 4) == 0)
            return formatMAC(e.mac);
    }
    return "";
}

int NetworkManager::clearARPTable() {
    int count = Sad::Security::SafeArithmetic::assertSafeCast<int>(arpTable_.size(), "network_stack_size");
    arpTable_.clear();
    return count;
}

// ============================================================================
// (AR) 4. ICMP / (EN) 4. ICMP (Ping)
// ============================================================================

int NetworkManager::ping(int nicId, const std::string& targetIP) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    if (nics_[nicId].nicState < NICState::INITIALIZED) return -1;

    uint8_t ip[4];
    if (!parseIP(targetIP, ip)) return -1;

    stats_.icmpSent++;
    // (AR) محاكاة ping — إرجاع وقت وهمي / (EN) Simulated ping response
    // (AR) عناوين محلية: سريعة، عناوين بعيدة: أبطأ
    if (ip[0] == 192 || ip[0] == 10 || ip[0] == 127) {
        lastPingMs_ = 1;  // LAN
    } else {
        lastPingMs_ = 25; // WAN
    }
    stats_.icmpReceived++;
    nics_[nicId].txPackets++;
    nics_[nicId].rxPackets++;
    nics_[nicId].txBytes += NetConstants::ETH_HEADER_SIZE + NetConstants::IPV4_HEADER_SIZE + NetConstants::ICMP_HEADER_SIZE + 64;
    nics_[nicId].rxBytes += NetConstants::ETH_HEADER_SIZE + NetConstants::IPV4_HEADER_SIZE + NetConstants::ICMP_HEADER_SIZE + 64;
    return lastPingMs_;
}

// ============================================================================
// (AR) 5. UDP / (EN) 5. UDP
// ============================================================================

int NetworkManager::udpSend(int nicId, const std::string& destIP, int destPort,
                            int srcPort, const std::string& data) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    (void)destIP; (void)destPort; (void)srcPort;
    stats_.udpSent++;
    nics_[nicId].txPackets++;
    nics_[nicId].txBytes += NetConstants::ETH_HEADER_SIZE + NetConstants::IPV4_HEADER_SIZE
                          + NetConstants::UDP_HEADER_SIZE + data.size();
    return Sad::Security::SafeArithmetic::assertSafeCast<int>(data.size(), "network_stack_size");
}

std::string NetworkManager::udpReceive(int nicId, int port) {
    (void)nicId;
    for (auto it = udpBuffers_.begin(); it != udpBuffers_.end(); ++it) {
        if (it->first == port) {
            std::string data = it->second;
            udpBuffers_.erase(it);
            stats_.udpReceived++;
            return data;
        }
    }
    return "";
}

int NetworkManager::udpBind(int nicId, int port) {
    (void)nicId;
    // (AR) تسجيل منفذ / (EN) Register port (no-op in simulation)
    (void)port;
    return 0;
}

int NetworkManager::udpUnbind(int nicId, int port) {
    (void)nicId;
    (void)port;
    return 0;
}

// ============================================================================
// (AR) 6. إرسال خام / (EN) 6. Raw Send
// ============================================================================

int NetworkManager::sendRawFrame(int nicId, const std::string& hexData) {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return -1;
    int bytes = Sad::Security::SafeArithmetic::assertSafeCast<int>(hexData.size() / 2, "network_stack_hexbytes");
    nics_[nicId].txPackets++;
    nics_[nicId].txBytes += bytes;
    stats_.totalTx++;
    return bytes;
}

// ============================================================================
// (AR) 7. إحصائيات / (EN) 7. Statistics
// ============================================================================

bool NetworkManager::isLinkUp(int nicId) const {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return false;
    return nics_[nicId].nicState == NICState::LINK_UP ||
           nics_[nicId].nicState == NICState::INITIALIZED;
}

int NetworkManager::getLinkSpeed(int nicId) const {
    if (nicId < 0 || nicId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(nics_.size(), "network_stack_size")) return 0;
    return nics_[nicId].linkSpeed;
}

// ============================================================================
// (AR) 8. إعادة تعيين / (EN) 8. Reset
// ============================================================================

void NetworkManager::reset() {
    nics_.clear();
    arpTable_.clear();
    udpBuffers_.clear();
    std::memset(&stats_, 0, sizeof(stats_));
    lastPingMs_ = 0;
}

} // namespace LowLevel
} // namespace Sad
