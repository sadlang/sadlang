/*
 * ============================================================================
 *  وحدة الاتصال التسلسلي — UART 16550 — لغة ص
 *  Serial/UART 16550 Module — Sad Language
 * ============================================================================
 *
 * @brief (AR) وحدة إدارة المنافذ التسلسلية (UART 16550/16550A)
 *             تدعم: COM1-COM4، إرسال واستقبال بايتات، تهيئة معدل البود،
 *             التحكم بالتدفق، وحالة المودم
 *
 * @brief (EN) UART 16550/16550A serial port management module
 *             Supports: COM1-COM4, byte send/receive, baud rate config,
 *             flow control, and modem status
 *
 * @details
 * (AR) توفر واجهة موحدة للمنافذ التسلسلية. على سطح المكتب: محاكاة مع
 *      مخزن مؤقت داخلي. في النواة: وصول مباشر لمنافذ I/O.
 *
 *      المنافذ المدعومة:
 *        COM1 (0x3F8)، COM2 (0x2F8)، COM3 (0x3E8)، COM4 (0x2E8)
 *
 *      معدلات البود:
 *        300، 1200، 2400، 4800، 9600، 19200، 38400، 57600، 115200
 *
 * (EN) Unified serial port API. Simulated with internal buffer on desktop,
 *      direct I/O port access on bare-metal.
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_SERIAL_H
#define SAD_LOW_LEVEL_SERIAL_H

#include <cstdint>
#include <string>
#include <vector>
#include <deque>

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) ثوابت المنفذ التسلسلي / (EN) Serial Constants
// ============================================================================
namespace SerialConstants {
    // (AR) عناوين المنافذ / (EN) Port base addresses
    constexpr uint16_t COM1_BASE = 0x3F8;
    constexpr uint16_t COM2_BASE = 0x2F8;
    constexpr uint16_t COM3_BASE = 0x3E8;
    constexpr uint16_t COM4_BASE = 0x2E8;
    // (AR) أقصى عدد منافذ / (EN) Max ports
    constexpr int MAX_PORTS = 4;
    // (AR) حجم المخزن المؤقت / (EN) Buffer size
    constexpr int BUFFER_SIZE = 4096;
    // (AR) إزاحات السجلات / (EN) Register offsets
    constexpr uint16_t REG_DATA = 0;
    constexpr uint16_t REG_IER  = 1;
    constexpr uint16_t REG_IIR  = 2;  // (AR) قراءة / (EN) Read
    constexpr uint16_t REG_FCR  = 2;  // (AR) كتابة / (EN) Write
    constexpr uint16_t REG_LCR  = 3;
    constexpr uint16_t REG_MCR  = 4;
    constexpr uint16_t REG_LSR  = 5;
    constexpr uint16_t REG_MSR  = 6;
    // (AR) بتات حالة الخط / (EN) LSR bits
    constexpr uint8_t LSR_DATA_READY = 0x01;
    constexpr uint8_t LSR_TX_EMPTY   = 0x20;
    constexpr uint8_t LSR_TX_IDLE    = 0x40;
}

// ============================================================================
// (AR) معدل البود / (EN) Baud Rate
// ============================================================================
enum class BaudRate : int {
    BAUD_300    = 300,
    BAUD_1200   = 1200,
    BAUD_2400   = 2400,
    BAUD_4800   = 4800,
    BAUD_9600   = 9600,
    BAUD_19200  = 19200,
    BAUD_38400  = 38400,
    BAUD_57600  = 57600,
    BAUD_115200 = 115200
};

// ============================================================================
// (AR) تهيئة المنفذ / (EN) Port Configuration
// ============================================================================
enum class DataBits : uint8_t { BITS_5 = 5, BITS_6 = 6, BITS_7 = 7, BITS_8 = 8 };
enum class StopBits : uint8_t { ONE = 1, TWO = 2 };
enum class Parity   : uint8_t { NONE = 0, ODD = 1, EVEN = 2 };

// ============================================================================
// (AR) حالة المنفذ التسلسلي / (EN) Serial Port State
// ============================================================================
enum class SerialState : uint8_t {
    CLOSED       = 0,  // (AR) مغلق / (EN) Closed
    OPEN         = 1,  // (AR) مفتوح / (EN) Open
    ERROR_STATE  = 2   // (AR) خطأ / (EN) Error
};

// ============================================================================
// (AR) معلومات المنفذ التسلسلي / (EN) Serial Port Info
// ============================================================================
struct SerialPortInfo {
    int id;                    // (AR) رقم المنفذ (0-3 = COM1-4) / (EN) Port number
    uint16_t baseAddr;         // (AR) عنوان القاعدة / (EN) Base I/O address
    SerialState serialState;   // (AR) الحالة / (EN) State
    int baudRate;              // (AR) معدل البود / (EN) Baud rate
    DataBits dataBits;         // (AR) بتات البيانات / (EN) Data bits
    StopBits stopBits;         // (AR) بتات التوقف / (EN) Stop bits
    Parity parity;             // (AR) التكافؤ / (EN) Parity
    uint64_t txCount;          // (AR) بايتات مرسلة / (EN) Bytes sent
    uint64_t rxCount;          // (AR) بايتات مستقبلة / (EN) Bytes received
    bool exists;               // (AR) هل المنفذ موجود / (EN) Port exists
};

// ============================================================================
// (AR) مدير المنافذ التسلسلية — نمط المفرد
// (EN) Serial Port Manager — Singleton
// ============================================================================
class SerialManager {
public:
    static SerialManager& getInstance() {
        static SerialManager instance;
        return instance;
    }

    // ════════════════════════════════════════════════════════════════
    // (AR) 1. اكتشاف وتهيئة / (EN) 1. Discovery & Init
    // ════════════════════════════════════════════════════════════════
    int scanPorts();
    int getPortCount() const;
    SerialPortInfo getPortInfo(int portId) const;
    int initPort(int portId, int baudRate);
    int closePort(int portId);
    bool isPortOpen(int portId) const;
    std::string generateReport() const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 2. إرسال / (EN) 2. Send
    // ════════════════════════════════════════════════════════════════
    int sendByte(int portId, uint8_t byte);
    int sendString(int portId, const std::string& data);
    int sendHex(int portId, const std::string& hexData);
    bool isTxReady(int portId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 3. استقبال / (EN) 3. Receive
    // ════════════════════════════════════════════════════════════════
    int receiveByte(int portId);
    std::string receiveString(int portId, int maxLen);
    int available(int portId) const;
    bool isRxReady(int portId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 4. تهيئة / (EN) 4. Configuration
    // ════════════════════════════════════════════════════════════════
    int setBaudRate(int portId, int baud);
    int getBaudRate(int portId) const;
    int setDataBits(int portId, int bits);
    int setStopBits(int portId, int bits);
    int setParity(int portId, int par);
    int setFlowControl(int portId, bool rtscts);

    // ════════════════════════════════════════════════════════════════
    // (AR) 5. حالة / (EN) 5. Status
    // ════════════════════════════════════════════════════════════════
    int getLineStatus(int portId) const;
    int getModemStatus(int portId) const;
    uint64_t getTxCount(int portId) const;
    uint64_t getRxCount(int portId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 6. وظائف خاصة / (EN) 6. Special Functions
    // ════════════════════════════════════════════════════════════════
    int loopbackTest(int portId);
    int clearBuffers(int portId);

    // (AR) إعادة تعيين / (EN) Reset
    void reset();

private:
    SerialManager();
    ~SerialManager() = default;
    SerialManager(const SerialManager&) = delete;
    SerialManager& operator=(const SerialManager&) = delete;

    std::vector<SerialPortInfo> ports_;
    std::vector<std::deque<uint8_t>> rxBuffers_;
    std::vector<std::deque<uint8_t>> txBuffers_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_SERIAL_H
