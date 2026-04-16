/**
 * @file serial.cpp
 * @brief (AR) تنفيذ مدير المنافذ التسلسلية — UART 16550
 *        محاكاة آمنة مع مخزن مؤقت داخلي
 * @brief (EN) Serial Port Manager implementation — UART 16550
 *        Safe simulation with internal buffer
 */

#include "serial.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) المُنشئ / (EN) Constructor
// ============================================================================
SerialManager::SerialManager() {}

// ============================================================================
// (AR) 1. اكتشاف وتهيئة / (EN) 1. Discovery & Init
// ============================================================================

int SerialManager::scanPorts() {
    ports_.clear();
    rxBuffers_.clear();
    txBuffers_.clear();

    // (AR) المنافذ التسلسلية القياسية الأربعة / (EN) Standard 4 COM ports
    const uint16_t bases[] = {
        SerialConstants::COM1_BASE,
        SerialConstants::COM2_BASE,
        SerialConstants::COM3_BASE,
        SerialConstants::COM4_BASE
    };

    for (int i = 0; i < SerialConstants::MAX_PORTS; ++i) {
        SerialPortInfo p;
        p.id = i;
        p.baseAddr = bases[i];
        p.serialState = SerialState::CLOSED;
        p.baudRate = 9600;
        p.dataBits = DataBits::BITS_8;
        p.stopBits = StopBits::ONE;
        p.parity = Parity::NONE;
        p.txCount = 0;
        p.rxCount = 0;
        // (AR) COM1 و COM2 موجودان عادةً / (EN) COM1 & COM2 typically exist
        p.exists = (i < 2);
        ports_.push_back(p);
        rxBuffers_.push_back(std::deque<uint8_t>());
        txBuffers_.push_back(std::deque<uint8_t>());
    }

    return getPortCount();
}

int SerialManager::getPortCount() const {
    int count = 0;
    for (const auto& p : ports_) {
        if (p.exists) count++;
    }
    return count;
}

SerialPortInfo SerialManager::getPortInfo(int portId) const {
    if (portId >= 0 && portId < static_cast<int>(ports_.size()))
        return ports_[portId];
    SerialPortInfo empty;
    empty.id = -1;
    empty.baseAddr = 0;
    empty.serialState = SerialState::CLOSED;
    empty.baudRate = 0;
    empty.dataBits = DataBits::BITS_8;
    empty.stopBits = StopBits::ONE;
    empty.parity = Parity::NONE;
    empty.txCount = 0;
    empty.rxCount = 0;
    empty.exists = false;
    return empty;
}

int SerialManager::initPort(int portId, int baudRate) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    if (!ports_[portId].exists) return -1;

    ports_[portId].baudRate = baudRate;
    ports_[portId].serialState = SerialState::OPEN;
    ports_[portId].txCount = 0;
    ports_[portId].rxCount = 0;
    rxBuffers_[portId].clear();
    txBuffers_[portId].clear();

    return 0;
}

int SerialManager::closePort(int portId) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    ports_[portId].serialState = SerialState::CLOSED;
    return 0;
}

bool SerialManager::isPortOpen(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return false;
    return ports_[portId].serialState == SerialState::OPEN;
}

std::string SerialManager::generateReport() const {
    std::ostringstream ss;
    ss << "=== \xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0 / Serial Report ===\n";
    ss << "\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0 / Ports: " << getPortCount() << "\n\n";
    for (const auto& p : ports_) {
        if (!p.exists) continue;
        ss << "--- COM" << (p.id + 1) << " (0x" << std::hex << p.baseAddr << std::dec << ") ---\n";
        ss << "  State: " << (p.serialState == SerialState::OPEN ? "Open" : "Closed") << "\n";
        ss << "  Baud: " << p.baudRate << "\n";
        ss << "  TX: " << p.txCount << " bytes, RX: " << p.rxCount << " bytes\n\n";
    }
    return ss.str();
}

// ============================================================================
// (AR) 2. إرسال / (EN) 2. Send
// ============================================================================

int SerialManager::sendByte(int portId, uint8_t byte) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    if (ports_[portId].serialState != SerialState::OPEN) return -1;

    // (AR) محاكاة: البيانات المرسلة تظهر في المخزن المؤقت للاستقبال (loopback)
    // (EN) Simulation: sent data appears in RX buffer (loopback)
    txBuffers_[portId].push_back(byte);
    rxBuffers_[portId].push_back(byte);
    ports_[portId].txCount++;
    return 0;
}

int SerialManager::sendString(int portId, const std::string& data) {
    for (uint8_t c : data) {
        int r = sendByte(portId, c);
        if (r != 0) return r;
    }
    return 0;
}

int SerialManager::sendHex(int portId, const std::string& hexData) {
    for (size_t i = 0; i + 1 < hexData.size(); i += 2) {
        std::string byteStr = hexData.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::strtoul(byteStr.c_str(), nullptr, 16));
        int r = sendByte(portId, byte);
        if (r != 0) return r;
    }
    return 0;
}

bool SerialManager::isTxReady(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return false;
    return ports_[portId].serialState == SerialState::OPEN;
}

// ============================================================================
// (AR) 3. استقبال / (EN) 3. Receive
// ============================================================================

int SerialManager::receiveByte(int portId) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    if (rxBuffers_[portId].empty()) return -1;
    uint8_t byte = rxBuffers_[portId].front();
    rxBuffers_[portId].pop_front();
    ports_[portId].rxCount++;
    return static_cast<int>(byte);
}

std::string SerialManager::receiveString(int portId, int maxLen) {
    std::string result;
    for (int i = 0; i < maxLen; ++i) {
        int byte = receiveByte(portId);
        if (byte < 0) break;
        result += static_cast<char>(byte);
    }
    return result;
}

int SerialManager::available(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return 0;
    return static_cast<int>(rxBuffers_[portId].size());
}

bool SerialManager::isRxReady(int portId) const {
    return available(portId) > 0;
}

// ============================================================================
// (AR) 4. تهيئة / (EN) 4. Configuration
// ============================================================================

int SerialManager::setBaudRate(int portId, int baud) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    ports_[portId].baudRate = baud;
    return 0;
}

int SerialManager::getBaudRate(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return 0;
    return ports_[portId].baudRate;
}

int SerialManager::setDataBits(int portId, int bits) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    if (bits < 5 || bits > 8) return -1;
    ports_[portId].dataBits = static_cast<DataBits>(bits);
    return 0;
}

int SerialManager::setStopBits(int portId, int bits) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    ports_[portId].stopBits = (bits == 2) ? StopBits::TWO : StopBits::ONE;
    return 0;
}

int SerialManager::setParity(int portId, int par) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    ports_[portId].parity = static_cast<Parity>(std::min(par, 2));
    return 0;
}

int SerialManager::setFlowControl(int portId, bool rtscts) {
    (void)portId; (void)rtscts;
    // (AR) محاكاة / (EN) Simulation
    return 0;
}

// ============================================================================
// (AR) 5. حالة / (EN) 5. Status
// ============================================================================

int SerialManager::getLineStatus(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return 0;
    int lsr = 0;
    if (!rxBuffers_[portId].empty())
        lsr |= SerialConstants::LSR_DATA_READY;
    if (ports_[portId].serialState == SerialState::OPEN)
        lsr |= SerialConstants::LSR_TX_EMPTY | SerialConstants::LSR_TX_IDLE;
    return lsr;
}

int SerialManager::getModemStatus(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return 0;
    // (AR) محاكاة: CTS + DSR مفعلان / (EN) Simulation: CTS + DSR set
    return 0x30;
}

uint64_t SerialManager::getTxCount(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return 0;
    return ports_[portId].txCount;
}

uint64_t SerialManager::getRxCount(int portId) const {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return 0;
    return ports_[portId].rxCount;
}

// ============================================================================
// (AR) 6. وظائف خاصة / (EN) 6. Special Functions
// ============================================================================

int SerialManager::loopbackTest(int portId) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    if (ports_[portId].serialState != SerialState::OPEN) return -1;

    // (AR) إرسال 0xAA واستقباله / (EN) Send 0xAA and receive it
    sendByte(portId, 0xAA);
    int received = receiveByte(portId);
    return (received == 0xAA) ? 0 : -1;
}

int SerialManager::clearBuffers(int portId) {
    if (portId < 0 || portId >= static_cast<int>(ports_.size())) return -1;
    rxBuffers_[portId].clear();
    txBuffers_[portId].clear();
    return 0;
}

// ============================================================================
// (AR) إعادة تعيين / (EN) Reset
// ============================================================================

void SerialManager::reset() {
    ports_.clear();
    rxBuffers_.clear();
    txBuffers_.clear();
}

} // namespace LowLevel
} // namespace Sad
