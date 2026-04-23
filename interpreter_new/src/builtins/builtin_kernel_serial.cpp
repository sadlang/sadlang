/**
 * @file builtin_kernel_serial.cpp
 * @brief (AR) وحدة المنفذ التسلسلي — إدارة UART والطباعة للتصحيح
 * @brief (EN) Serial module — UART management and debug printing
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "value.h"
#include "serial.h"

#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelSerial(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. اكتشاف وتهيئة / Discovery & Init
    // ═══════════════════════════════════════════════════════════════
    // 1. serial_scan / تسلسلي_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().scanPorts());
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd9\x85\xd8\xb3\xd8\xad", f);
    }

    // 2. serial_port_count / تسلسلي_عدد_المنافذ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().getPortCount());
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0", f);
    }

    // 3. serial_init / تسلسلي_تهيئة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int baud = args.size() > 1 ? args[1]->toInt() : 9600;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().initPort(port, baud));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9", f);
    }

    // 4. serial_close / تسلسلي_اغلاق
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().closePort(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xba\xd9\x84\xd8\xa7\xd9\x82", f);
    }

    // 5. serial_is_open / تسلسلي_مفتوح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().isPortOpen(port) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd9\x85\xd9\x81\xd8\xaa\xd9\x88\xd8\xad", f);
    }

    // 6. serial_report / تسلسلي_تقرير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().generateReport());
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 2. إرسال / Send
    // ═══════════════════════════════════════════════════════════════
    // 7. serial_send_byte / تسلسلي_ارسال_بايت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int byte = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().sendByte(port, static_cast<uint8_t>(byte)));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84_\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa", f);
    }

    // 8. serial_send_string / تسلسلي_ارسال_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            std::string data = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().sendString(port, data));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84_\xd9\x86\xd8\xb5", f);
    }

    // 9. serial_send_hex / تسلسلي_ارسال_ست_عشري
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            std::string hex = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().sendHex(port, hex));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84_\xd8\xb3\xd8\xaa_\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", f);
    }

    // 10. serial_tx_ready / تسلسلي_جاهز_ارسال
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().isTxReady(port) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xac\xd8\xa7\xd9\x87\xd8\xb2_\xd8\xa7\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 3. استقبال / Receive
    // ═══════════════════════════════════════════════════════════════
    // 11. serial_receive_byte / تسلسلي_استقبال_بايت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().receiveByte(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84_\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa", f);
    }

    // 12. serial_receive_string / تسلسلي_استقبال_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int maxLen = args.size() > 1 ? args[1]->toInt() : 256;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().receiveString(port, maxLen));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84_\xd9\x86\xd8\xb5", f);
    }

    // 13. serial_available / تسلسلي_متاح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().available(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd9\x85\xd8\xaa\xd8\xa7\xd8\xad", f);
    }

    // 14. serial_rx_ready / تسلسلي_جاهز_استقبال
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().isRxReady(port) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xac\xd8\xa7\xd9\x87\xd8\xb2_\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 4. تهيئة / Configuration
    // ═══════════════════════════════════════════════════════════════
    // 15. serial_set_baud / تسلسلي_تعيين_بود
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int baud = args.size() > 1 ? args[1]->toInt() : 9600;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setBaudRate(port, baud));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xa8\xd9\x88\xd8\xaf", f);
    }

    // 16. serial_get_baud / تسلسلي_معدل_البود
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getBaudRate(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84_\xd8\xa7\xd9\x84\xd8\xa8\xd9\x88\xd8\xaf", f);
    }

    // 17. serial_set_data_bits / تسلسلي_بتات_بيانات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int bits = args.size() > 1 ? args[1]->toInt() : 8;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setDataBits(port, bits));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa8\xd8\xaa\xd8\xa7\xd8\xaa_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", f);
    }

    // 18. serial_set_stop_bits / تسلسلي_بتات_توقف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int bits = args.size() > 1 ? args[1]->toInt() : 1;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setStopBits(port, bits));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa8\xd8\xaa\xd8\xa7\xd8\xaa_\xd8\xaa\xd9\x88\xd9\x82\xd9\x81", f);
    }

    // 19. serial_set_parity / تسلسلي_تكافؤ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int par = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setParity(port, par));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xaa\xd9\x83\xd8\xa7\xd9\x81\xd8\xa4", f);
    }

    // 20. serial_set_flow / تسلسلي_تحكم_تدفق
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            bool rtscts = args.size() > 1 ? (args[1]->toInt() != 0) : false;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setFlowControl(port, rtscts));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xaa\xd8\xad\xd9\x83\xd9\x85_\xd8\xaa\xd8\xaf\xd9\x81\xd9\x82", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 5. حالة / Status
    // ═══════════════════════════════════════════════════════════════
    // 21. serial_line_status / تسلسلي_حالة_الخط
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getLineStatus(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7", f);
    }

    // 22. serial_modem_status / تسلسلي_حالة_المودم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getModemStatus(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xa7\xd9\x84\xd9\x85\xd9\x88\xd8\xaf\xd9\x85", f);
    }

    // 23. serial_tx_count / تسلسلي_عداد_ارسال
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::SerialManager::getInstance().getTxCount(port)));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xa7\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84", f);
    }

    // 24. serial_rx_count / تسلسلي_عداد_استقبال
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::SerialManager::getInstance().getRxCount(port)));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 6. وظائف خاصة / Special Functions
    // ═══════════════════════════════════════════════════════════════
    // 25. serial_loopback / تسلسلي_اختبار_حلقي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().loopbackTest(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1_\xd8\xad\xd9\x84\xd9\x82\xd9\x8a", f);
    }

    // 26. serial_clear / تسلسلي_مسح_المخزن
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().clearBuffers(port));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd9\x85\xd8\xae\xd8\xb2\xd9\x86", f);
    }

    // 27. serial_reset / تسلسلي_اعادة_تعيين
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            LowLevel::SerialManager::getInstance().reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86", f);
    }

    // 28. serial_port_exists / تسلسلي_منفذ_موجود
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getPortInfo(port).exists ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd9\x85\xd9\x86\xd9\x81\xd8\xb0_\xd9\x85\xd9\x88\xd8\xac\xd9\x88\xd8\xaf", f);
    }

    // 29. serial_port_addr / تسلسلي_عنوان_منفذ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                static_cast<int>(LowLevel::SerialManager::getInstance().getPortInfo(port).baseAddr));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd9\x85\xd9\x86\xd9\x81\xd8\xb0", f);
    }

    // 30. serial_port_state / تسلسلي_حالة_منفذ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                static_cast<int>(LowLevel::SerialManager::getInstance().getPortInfo(port).serialState));
        };
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd9\x86\xd9\x81\xd8\xb0", f);
    }

} // registerBuiltinsKernelSerial

} // namespace Interpreter
} // namespace Sad
