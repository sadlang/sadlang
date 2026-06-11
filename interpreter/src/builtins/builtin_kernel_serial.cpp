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
#include "builtin_registry.h"
namespace Kserial = Sad::Builtins::Names::KernelSerial;
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
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().scanPorts());
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_0), f);
    }

    // 2. serial_port_count / تسلسلي_عدد_المنافذ
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().getPortCount());
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_1), f);
    }

    // 3. serial_init / تسلسلي_تهيئة
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int baud = args.size() > 1 ? args[1]->toInt() : 9600;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().initPort(port, baud));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_2), f);
    }

    // 4. serial_close / تسلسلي_اغلاق
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().closePort(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_3), f);
    }

    // 5. serial_is_open / تسلسلي_مفتوح
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().isPortOpen(port) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_4), f);
    }

    // 6. serial_report / تسلسلي_تقرير
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::SerialManager::getInstance().generateReport());
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_5), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 2. إرسال / Send
    // ═══════════════════════════════════════════════════════════════
    // 7. serial_send_byte / تسلسلي_ارسال_بايت
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int byte = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().sendByte(port, static_cast<uint8_t>(byte)));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_6), f);
    }

    // 8. serial_send_string / تسلسلي_ارسال_نص
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            std::string data = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().sendString(port, data));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_7), f);
    }

    // 9. serial_send_hex / تسلسلي_ارسال_ست_عشري
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            std::string hex = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().sendHex(port, hex));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_8), f);
    }

    // 10. serial_tx_ready / تسلسلي_جاهز_ارسال
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().isTxReady(port) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_9), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 3. استقبال / Receive
    // ═══════════════════════════════════════════════════════════════
    // 11. serial_receive_byte / تسلسلي_استقبال_بايت
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().receiveByte(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_10), f);
    }

    // 12. serial_receive_string / تسلسلي_استقبال_نص
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int maxLen = args.size() > 1 ? args[1]->toInt() : 256;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().receiveString(port, maxLen));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_11), f);
    }

    // 13. serial_available / تسلسلي_متاح
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().available(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_12), f);
    }

    // 14. serial_rx_ready / تسلسلي_جاهز_استقبال
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().isRxReady(port) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_13), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 4. تهيئة / Configuration
    // ═══════════════════════════════════════════════════════════════
    // 15. serial_set_baud / تسلسلي_تعيين_بود
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int baud = args.size() > 1 ? args[1]->toInt() : 9600;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setBaudRate(port, baud));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_14), f);
    }

    // 16. serial_get_baud / تسلسلي_معدل_البود
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getBaudRate(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_15), f);
    }

    // 17. serial_set_data_bits / تسلسلي_بتات_بيانات
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int bits = args.size() > 1 ? args[1]->toInt() : 8;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setDataBits(port, bits));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_16), f);
    }

    // 18. serial_set_stop_bits / تسلسلي_بتات_توقف
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int bits = args.size() > 1 ? args[1]->toInt() : 1;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setStopBits(port, bits));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_17), f);
    }

    // 19. serial_set_parity / تسلسلي_تكافؤ
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            int par = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setParity(port, par));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_18), f);
    }

    // 20. serial_set_flow / تسلسلي_تحكم_تدفق
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            bool rtscts = args.size() > 1 ? (args[1]->toInt() != 0) : false;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().setFlowControl(port, rtscts));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_19), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 5. حالة / Status
    // ═══════════════════════════════════════════════════════════════
    // 21. serial_line_status / تسلسلي_حالة_الخط
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getLineStatus(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_20), f);
    }

    // 22. serial_modem_status / تسلسلي_حالة_المودم
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getModemStatus(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_21), f);
    }

    // 23. serial_tx_count / تسلسلي_عداد_ارسال
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::SerialManager::getInstance().getTxCount(port)));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_22), f);
    }

    // 24. serial_rx_count / تسلسلي_عداد_استقبال
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::SerialManager::getInstance().getRxCount(port)));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_23), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 6. وظائف خاصة / Special Functions
    // ═══════════════════════════════════════════════════════════════
    // 25. serial_loopback / تسلسلي_اختبار_حلقي
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().loopbackTest(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_24), f);
    }

    // 26. serial_clear / تسلسلي_مسح_المخزن
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().clearBuffers(port));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_25), f);
    }

    // 27. serial_reset / تسلسلي_اعادة_تعيين
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            LowLevel::SerialManager::getInstance().reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_26), f);
    }

    // 28. serial_port_exists / تسلسلي_منفذ_موجود
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::SerialManager::getInstance().getPortInfo(port).exists ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_27), f);
    }

    // 29. serial_port_addr / تسلسلي_عنوان_منفذ
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                static_cast<int>(LowLevel::SerialManager::getInstance().getPortInfo(port).baseAddr));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_28), f);
    }

    // 30. serial_port_state / تسلسلي_حالة_منفذ
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int port = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                static_cast<int>(LowLevel::SerialManager::getInstance().getPortInfo(port).serialState));
        };
        fm.registerBuiltinFunction(std::string(Kserial::SERIAL_29), f);
    }

} // registerBuiltinsKernelSerial

} // namespace Interpreter
} // namespace Sad
