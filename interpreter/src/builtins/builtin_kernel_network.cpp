/**
 * @file builtin_kernel_network.cpp
 * @brief (AR) وحدة الشبكة — إدارة بطاقات الشبكة والحزم
 * @brief (EN) Network module — NIC management and packet handling
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
namespace Knet = Sad::Builtins::Names::KernelNet;
#include "value.h"
#include "network_stack.h"

#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelNetwork(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. اكتشاف وتهيئة / Discovery & Init
    // ═══════════════════════════════════════════════════════════════
    // 1. net_scan / شبكة_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().scanNICs());
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_0), f);
    }

    // 2. net_nic_count / شبكة_عدد_البطاقات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getNICCount());
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_1), f);
    }

    // 3. net_nic_model / شبكة_موديل_البطاقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getNICInfo(id).model);
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_2), f);
    }

    // 4. net_init / شبكة_تهيئة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().initNIC(id));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_3), f);
    }

    // 5. net_report / شبكة_تقرير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().generateReport());
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_4), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 2. تهيئة IP / IP Config
    // ═══════════════════════════════════════════════════════════════
    // 6. net_set_ip / شبكة_تعيين_ip
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string ip = args.size() > 1 ? args[1]->toString() : "0.0.0.0";
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().setIP(id, ip));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_5), f);
    }

    // 7. net_get_ip / شبكة_عنوان_ip
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getIP(id));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_6), f);
    }

    // 8. net_get_mac / شبكة_عنوان_mac
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getMAC(id));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_7), f);
    }

    // 9. net_set_subnet / شبكة_تعيين_القناع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string mask = args.size() > 1 ? args[1]->toString() : "255.255.255.0";
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().setSubnet(id, mask));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_8), f);
    }

    // 10. net_set_gateway / شبكة_تعيين_البوابة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string gw = args.size() > 1 ? args[1]->toString() : "0.0.0.0";
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().setGateway(id, gw));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_9), f);
    }

    // 11. net_get_subnet / شبكة_القناع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getSubnet(id));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_10), f);
    }

    // 12. net_get_gateway / شبكة_البوابة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getGateway(id));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_11), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 3. ARP
    // ═══════════════════════════════════════════════════════════════
    // 13. net_arp_request / شبكة_طلب_arp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string ip = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().arpRequest(id, ip));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_12), f);
    }

    // 14. net_arp_lookup / شبكة_بحث_arp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string ip = args.size() > 0 ? args[0]->toString() : "";
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().arpLookup(ip));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_13), f);
    }

    // 15. net_arp_table_size / شبكة_حجم_جدول_arp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getARPTableSize());
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_14), f);
    }

    // 16. net_arp_clear / شبكة_مسح_جدول_arp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().clearARPTable());
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_15), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 4. ICMP / Ping
    // ═══════════════════════════════════════════════════════════════
    // 17. net_ping / شبكة_بنج
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string ip = args.size() > 1 ? args[1]->toString() : "127.0.0.1";
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().ping(id, ip));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_16), f);
    }

    // 18. net_last_ping / شبكة_اخر_بنج
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::NetworkManager::getInstance().getLastPingTime());
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_17), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 5. UDP
    // ═══════════════════════════════════════════════════════════════
    // 19. net_udp_send / شبكة_ارسال_udp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int nicId = args.size() > 0 ? args[0]->toInt() : 0;
            std::string destIP = args.size() > 1 ? args[1]->toString() : "";
            int destPort = args.size() > 2 ? args[2]->toInt() : 0;
            int srcPort = args.size() > 3 ? args[3]->toInt() : 0;
            std::string data = args.size() > 4 ? args[4]->toString() : "";
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().udpSend(nicId, destIP, destPort, srcPort, data));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_18), f);
    }

    // 20. net_udp_receive / شبكة_استقبال_udp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int nicId = args.size() > 0 ? args[0]->toInt() : 0;
            int port = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().udpReceive(nicId, port));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_19), f);
    }

    // 21. net_udp_bind / شبكة_ربط_udp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int nicId = args.size() > 0 ? args[0]->toInt() : 0;
            int port = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().udpBind(nicId, port));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_20), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 6. إحصائيات / Statistics
    // ═══════════════════════════════════════════════════════════════
    // 22. net_link_up / شبكة_الارتباط_فعال
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().isLinkUp(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_21), f);
    }

    // 23. net_link_speed / شبكة_سرعة_الارتباط
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().getLinkSpeed(id));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_22), f);
    }

    // 24. net_send_raw / شبكة_ارسال_خام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string hex = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().sendRawFrame(id, hex));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_23), f);
    }

    // 25. net_tx_packets / شبكة_الحزم_المرسلة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto info = LowLevel::NetworkManager::getInstance().getNICInfo(id);
            return std::make_shared<Data::Value>(static_cast<int>(info.txPackets));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_24), f);
    }

    // 26. net_rx_packets / شبكة_الحزم_المستقبلة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto info = LowLevel::NetworkManager::getInstance().getNICInfo(id);
            return std::make_shared<Data::Value>(static_cast<int>(info.rxPackets));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_25), f);
    }

    // 27. net_nic_state / شبكة_حاله_البطاقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                static_cast<int>(LowLevel::NetworkManager::getInstance().getNICInfo(id).nicState));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_26), f);
    }

    // 28. net_reset / شبكة_اعادة_تعيين
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            LowLevel::NetworkManager::getInstance().reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_27), f);
    }

    // 29. net_nic_vendor / شبكة_مصنع_البطاقة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto info = LowLevel::NetworkManager::getInstance().getNICInfo(id);
            return std::make_shared<Data::Value>(static_cast<int>(info.vendorId));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_28), f);
    }

    // 30. net_udp_unbind / شبكة_فك_ربط_udp
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int nicId = args.size() > 0 ? args[0]->toInt() : 0;
            int port = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::NetworkManager::getInstance().udpUnbind(nicId, port));
        };
        fm.registerBuiltinFunction(std::string(Knet::NET_29), f);
    }

} // registerBuiltinsKernelNetwork

} // namespace Interpreter
} // namespace Sad
