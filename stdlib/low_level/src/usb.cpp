/**
 * @file usb.cpp
 * @brief (AR) تنفيذ مدير USB — UHCI/EHCI/xHCI مع HID
 * @brief (EN) USB Manager implementation — UHCI/EHCI/xHCI + HID
 */

#include "usb.h"
#include "pci.h"
#include "io_ports.h"
#include <sstream>
#include <algorithm>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace Sad {
namespace LowLevel {

// ============================================================================
// جداول ASCII للوحة المفاتيح / ASCII Lookup Tables
// ============================================================================

const char USBManager::asciiTable_[128] = {
    0, 0, 0, 0,
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '1','2','3','4','5','6','7','8','9','0',
    '\n','\033','\b','\t',' ','-','=','[',']','\\',
    '#',';','\'','`',',','.','/',
    0, // Caps Lock
    0,0,0,0,0,0,0,0,0,0,0,0, // F1-F12
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const char USBManager::asciiTableShift_[128] = {
    0, 0, 0, 0,
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    '!','@','#','$','%','^','&','*','(',')',
    '\n','\033','\b','\t',' ','_','+','{','}','|',
    '~',':','"','~','<','>','?',
    0, // Caps Lock
    0,0,0,0,0,0,0,0,0,0,0,0, // F1-F12
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// ============================================================================
// قراءة/كتابة MMIO / MMIO Read/Write
// ============================================================================

uint32_t USBManager::readMMIO(volatile uint32_t* base, uint32_t offset) const {
    if (!base) return 0;
    return base[offset / 4];
}

void USBManager::writeMMIO(volatile uint32_t* base, uint32_t offset, uint32_t value) {
    if (!base) return;
    base[offset / 4] = value;
}

uint16_t USBManager::readIO16(uint16_t port) const {
#ifdef _MSC_VER
    return __inword(port);
#elif defined(__x86_64__) || defined(__i386__)
    uint16_t val;
    __asm__ volatile("inw %1, %0" : "=a"(val) : "dN"(port));
    return val;
#else
    (void)port;
    return 0; // I/O ports not available on this architecture
#endif
}

void USBManager::writeIO16(uint16_t port, uint16_t value) {
#ifdef _MSC_VER
    __outword(port, value);
#elif defined(__x86_64__) || defined(__i386__)
    __asm__ volatile("outw %0, %1" : : "a"(value), "dN"(port));
#else
    (void)port;
    (void)value; // I/O ports not available on this architecture
#endif
}

// ============================================================================
// التعداد والاكتشاف / Enumeration & Discovery
// ============================================================================

int USBManager::scanControllers() {
    controllers_.clear();

    auto& pci = PCIManager::getInstance();
    auto devices = pci.getDevices();

    for (const auto& dev : devices) {
        if (dev.classCode == USBConst::PCI_CLASS_SERIAL &&
            dev.subclass == USBConst::PCI_SUBCLASS_USB) {

            ControllerState ctrl;
            ctrl.info.busNumber = dev.address.bus;
            ctrl.info.pciSlot = dev.address.device;
            ctrl.info.pciFunction = dev.address.function;

            // تحديد النوع من progIF / Determine type from progIF
            switch (dev.progIF) {
                case USBConst::PCI_PROG_UHCI:
                    ctrl.info.type = USBControllerType::UHCI;
                    ctrl.info.name = "UHCI (USB 1.x)";
                    break;
                case USBConst::PCI_PROG_OHCI:
                    ctrl.info.type = USBControllerType::OHCI;
                    ctrl.info.name = "OHCI (USB 1.x)";
                    break;
                case USBConst::PCI_PROG_EHCI:
                    ctrl.info.type = USBControllerType::EHCI;
                    ctrl.info.name = "EHCI (USB 2.0)";
                    break;
                case USBConst::PCI_PROG_XHCI:
                    ctrl.info.type = USBControllerType::XHCI;
                    ctrl.info.name = "xHCI (USB 3.x)";
                    break;
                default:
                    ctrl.info.type = USBControllerType::UNKNOWN;
                    ctrl.info.name = "Unknown USB";
                    break;
            }

            // قراءة BAR0 للحصول على عنوان MMIO / Read BAR0 for MMIO base
            uint32_t bar0 = PCIManager::readConfig32(dev.address, 0x10);
            if (bar0 & 1) {
                // I/O space (UHCI uses I/O)
                ctrl.info.mmioBase = bar0 & ~0x3ULL;
            } else {
                // Memory space
                ctrl.info.mmioBase = bar0 & ~0xFULL;
                // Check for 64-bit BAR
                if ((bar0 & 0x6) == 0x4) {
                    uint32_t bar1 = PCIManager::readConfig32(dev.address, 0x14);
                    ctrl.info.mmioBase |= (static_cast<uint64_t>(bar1) << 32);
                }
            }

            controllers_.push_back(ctrl);
        }
    }

    return Sad::Security::SafeArithmetic::assertSafeCast<int>(controllers_.size(), "usb_size");
}

USBControllerInfo USBManager::getControllerInfo(size_t index) const {
    if (index >= controllers_.size()) return {};
    return controllers_[index].info;
}

int USBManager::initController(size_t index) {
    if (index >= controllers_.size()) return -1;

    auto& ctrl = controllers_[index];
    if (ctrl.initialized) return 0;

    // تعيين عنوان MMIO / Map MMIO base
    if (ctrl.info.mmioBase != 0) {
        ctrl.mmioBase = reinterpret_cast<volatile uint32_t*>(ctrl.info.mmioBase);
    }

    switch (ctrl.info.type) {
        case USBControllerType::XHCI: {
            if (!ctrl.mmioBase) return -1;
            // قراءة CAPLENGTH للحصول على طول القدرات / Read capability length
            uint8_t capLen = static_cast<uint8_t>(readMMIO(ctrl.mmioBase, 0x00));
            // قراءة HCSPARAMS1 لمعرفة عدد المنافذ / Read port count
            uint32_t hcs1 = readMMIO(ctrl.mmioBase, 0x04);
            ctrl.info.numPorts = static_cast<uint8_t>((hcs1 >> 24) & 0xFF);
            // إيقاف المتحكم / Stop controller
            uint32_t cmd = readMMIO(ctrl.mmioBase, capLen + 0x00);
            cmd &= ~1; // Clear Run/Stop
            writeMMIO(ctrl.mmioBase, capLen + 0x00, cmd);
            // انتظار التوقف / Wait for halt
            for (int i = 0; i < 100; i++) {
                uint32_t sts = readMMIO(ctrl.mmioBase, capLen + 0x04);
                if (sts & 1) break; // HCHalted
            }
            // إعادة التعيين / Reset
            cmd = readMMIO(ctrl.mmioBase, capLen + 0x00);
            cmd |= (1 << 1); // Host Controller Reset
            writeMMIO(ctrl.mmioBase, capLen + 0x00, cmd);
            for (int i = 0; i < 100; i++) {
                cmd = readMMIO(ctrl.mmioBase, capLen + 0x00);
                if (!(cmd & (1 << 1))) break;
            }
            ctrl.initialized = true;
            ctrl.info.enabled = true;
            break;
        }
        case USBControllerType::EHCI: {
            if (!ctrl.mmioBase) return -1;
            uint8_t capLen = static_cast<uint8_t>(readMMIO(ctrl.mmioBase, 0x00));
            uint32_t hcs = readMMIO(ctrl.mmioBase, 0x04);
            ctrl.info.numPorts = static_cast<uint8_t>(hcs & 0x0F);
            // إيقاف / Stop
            uint32_t cmd = readMMIO(ctrl.mmioBase, capLen + 0x00);
            cmd &= ~1;
            writeMMIO(ctrl.mmioBase, capLen + 0x00, cmd);
            for (int i = 0; i < 100; i++) {
                uint32_t sts = readMMIO(ctrl.mmioBase, capLen + 0x04);
                if (sts & (1 << 12)) break; // HCHalted
            }
            // إعادة التعيين / Reset
            cmd = readMMIO(ctrl.mmioBase, capLen + 0x00);
            cmd |= (1 << 1);
            writeMMIO(ctrl.mmioBase, capLen + 0x00, cmd);
            for (int i = 0; i < 100; i++) {
                cmd = readMMIO(ctrl.mmioBase, capLen + 0x00);
                if (!(cmd & (1 << 1))) break;
            }
            ctrl.initialized = true;
            ctrl.info.enabled = true;
            break;
        }
        case USBControllerType::UHCI: {
            // UHCI uses I/O ports
            uint16_t ioBase = static_cast<uint16_t>(ctrl.info.mmioBase);
            if (ioBase == 0) return -1;
            // Global reset
            writeIO16(ioBase, 0x0004); // GRESET
            for (volatile int i = 0; i < 100000; i++);
            writeIO16(ioBase, 0x0000);
            // Read port count (UHCI typically has 2 ports)
            ctrl.info.numPorts = 2;
            ctrl.initialized = true;
            ctrl.info.enabled = true;
            break;
        }
        default:
            return -1;
    }

    return 0;
}

int USBManager::resetController(size_t index) {
    if (index >= controllers_.size()) return -1;
    auto& ctrl = controllers_[index];
    ctrl.initialized = false;
    ctrl.info.enabled = false;
    return initController(index);
}

// ============================================================================
// إدارة المنافذ / Port Management
// ============================================================================

int USBManager::getPortCount(size_t controllerIndex) const {
    if (controllerIndex >= controllers_.size()) return 0;
    return controllers_[controllerIndex].info.numPorts;
}

uint32_t USBManager::getPortStatus(size_t controllerIndex, uint8_t port) const {
    if (controllerIndex >= controllers_.size()) return 0;
    const auto& ctrl = controllers_[controllerIndex];
    if (!ctrl.initialized) return 0;
    if (port >= ctrl.info.numPorts) return 0;

    switch (ctrl.info.type) {
        case USBControllerType::XHCI: {
            if (!ctrl.mmioBase) return 0;
            uint8_t capLen = static_cast<uint8_t>(readMMIO(ctrl.mmioBase, 0x00));
            uint32_t portBase = capLen + 0x400 + (port * 0x10);
            return readMMIO(ctrl.mmioBase, portBase);
        }
        case USBControllerType::EHCI: {
            if (!ctrl.mmioBase) return 0;
            uint8_t capLen = static_cast<uint8_t>(readMMIO(ctrl.mmioBase, 0x00));
            return readMMIO(ctrl.mmioBase, capLen + 0x44 + (port * 4));
        }
        case USBControllerType::UHCI: {
            uint16_t ioBase = static_cast<uint16_t>(ctrl.info.mmioBase);
            return readIO16(ioBase + 0x10 + (port * 2));
        }
        default:
            return 0;
    }
}

bool USBManager::isPortConnected(size_t controllerIndex, uint8_t port) const {
    uint32_t status = getPortStatus(controllerIndex, port);
    return (status & USBConst::PORT_CONNECTED) != 0;
}

int USBManager::resetPort(size_t controllerIndex, uint8_t port) {
    if (controllerIndex >= controllers_.size()) return -1;
    auto& ctrl = controllers_[controllerIndex];
    if (!ctrl.initialized || port >= ctrl.info.numPorts) return -1;

    switch (ctrl.info.type) {
        case USBControllerType::XHCI: {
            if (!ctrl.mmioBase) return -1;
            uint8_t capLen = static_cast<uint8_t>(readMMIO(ctrl.mmioBase, 0x00));
            uint32_t portBase = capLen + 0x400 + (port * 0x10);
            uint32_t sc = readMMIO(ctrl.mmioBase, portBase);
            sc |= USBConst::PORT_RESET;
            writeMMIO(ctrl.mmioBase, portBase, sc);
            for (int i = 0; i < 100; i++) {
                sc = readMMIO(ctrl.mmioBase, portBase);
                if (!(sc & USBConst::PORT_RESET)) break;
            }
            return 0;
        }
        case USBControllerType::EHCI: {
            if (!ctrl.mmioBase) return -1;
            uint8_t capLen = static_cast<uint8_t>(readMMIO(ctrl.mmioBase, 0x00));
            uint32_t portReg = capLen + 0x44 + (port * 4);
            uint32_t sc = readMMIO(ctrl.mmioBase, portReg);
            sc |= (1 << 8); // Port Reset
            writeMMIO(ctrl.mmioBase, portReg, sc);
            for (volatile int i = 0; i < 50000; i++);
            sc &= ~(1 << 8);
            writeMMIO(ctrl.mmioBase, portReg, sc);
            return 0;
        }
        case USBControllerType::UHCI: {
            uint16_t ioBase = static_cast<uint16_t>(ctrl.info.mmioBase);
            uint16_t portReg = ioBase + 0x10 + (port * 2);
            uint16_t sc = readIO16(portReg);
            sc |= (1 << 9); // Port Reset
            writeIO16(portReg, sc);
            for (volatile int i = 0; i < 50000; i++);
            sc &= ~(1 << 9);
            writeIO16(portReg, sc);
            sc |= (1 << 2); // Enable
            writeIO16(portReg, sc);
            return 0;
        }
        default:
            return -1;
    }
}

int USBManager::enablePort(size_t controllerIndex, uint8_t port, bool enable) {
    if (controllerIndex >= controllers_.size()) return -1;
    auto& ctrl = controllers_[controllerIndex];
    if (!ctrl.initialized || port >= ctrl.info.numPorts) return -1;

    if (ctrl.info.type == USBControllerType::UHCI) {
        uint16_t ioBase = static_cast<uint16_t>(ctrl.info.mmioBase);
        uint16_t portReg = ioBase + 0x10 + (port * 2);
        uint16_t sc = readIO16(portReg);
        if (enable)
            sc |= (1 << 2);
        else
            sc &= ~(1 << 2);
        writeIO16(portReg, sc);
        return 0;
    }
    return 0; // EHCI/xHCI ports auto-enable
}

// ============================================================================
// إدارة الأجهزة / Device Management
// ============================================================================

int USBManager::enumerateDevices() {
    devices_.clear();

    for (size_t c = 0; c < controllers_.size(); c++) {
        auto& ctrl = controllers_[c];
        if (!ctrl.initialized) continue;

        for (uint8_t p = 0; p < ctrl.info.numPorts; p++) {
            if (!isPortConnected(c, p)) continue;

            USBDeviceInfo dev;
            dev.port = p;

            // تحديد السرعة / Determine speed
            uint32_t status = getPortStatus(c, p);
            if (status & USBConst::PORT_LOW_SPEED)
                dev.speed = USBSpeed::LOW;
            else if (status & USBConst::PORT_HIGH_SPEED)
                dev.speed = USBSpeed::HIGH;
            else
                dev.speed = USBSpeed::FULL;

            // قراءة واصف الجهاز (تحتاج USB control transfer حقيقي)
            // على سطح المكتب هذا لن يعمل — نضيف معلومات وهمية للتوافق
            dev.vendorId = 0;
            dev.productId = 0;
            dev.address = static_cast<uint8_t>(devices_.size() + 1);

            devices_.push_back(dev);
        }
    }

    return Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "usb_size");
}

USBDeviceInfo USBManager::getDeviceInfo(size_t index) const {
    if (index >= devices_.size()) return {};
    return devices_[index];
}

int USBManager::setDeviceAddress(size_t deviceIndex, uint8_t address) {
    if (deviceIndex >= devices_.size()) return -1;
    devices_[deviceIndex].address = address;
    return 0;
}

int USBManager::setDeviceConfiguration(size_t deviceIndex, uint8_t config) {
    if (deviceIndex >= devices_.size()) return -1;
    (void)config; // Would send SET_CONFIGURATION request
    return 0;
}

// ============================================================================
// النقل / Transfers
// ============================================================================

int USBManager::controlTransfer(uint8_t address, uint8_t requestType,
                                uint8_t request, uint16_t value,
                                uint16_t index, void* data, uint16_t length) {
    (void)address; (void)requestType; (void)request;
    (void)value; (void)index; (void)data; (void)length;
    // في نظام حقيقي: إنشاء TD وإرسال عبر المتحكم
    // In real OS: create TD and submit through controller
    return 0;
}

int USBManager::interruptTransfer(uint8_t address, uint8_t endpoint,
                                  void* data, uint16_t length) {
    (void)address; (void)endpoint; (void)data; (void)length;
    return 0;
}

int USBManager::bulkTransfer(uint8_t address, uint8_t endpoint,
                             void* data, uint32_t length) {
    (void)address; (void)endpoint; (void)data; (void)length;
    return 0;
}

// ============================================================================
// HID — لوحة المفاتيح / Keyboard
// ============================================================================

int USBManager::initKeyboard() {
    // البحث عن جهاز HID keyboard بين الأجهزة / Find HID keyboard
    for (size_t i = 0; i < devices_.size(); i++) {
        if (devices_[i].isKeyboard) {
            keyboardAddress_ = devices_[i].address;
            keyboardConnected_ = true;
            hidInitialized_ = true;
            return 0;
        }
    }
    // إذا لم يوجد، نفترض عدم الاتصال / Not found
    keyboardConnected_ = false;
    return -1;
}

int USBManager::pollKeyboard() {
    if (!keyboardConnected_) return -1;

    // في نظام حقيقي: interrupt transfer لقراءة تقرير HID
    // In real OS: interrupt transfer to read HID report
    // uint8_t report[8];
    // int result = interruptTransfer(keyboardAddress_, keyboardEndpoint_,
    //                                report, sizeof(report));
    // if (result >= 0) processKeyboardReport(report, sizeof(report));
    return 0;
}

bool USBManager::isKeyPressed(uint8_t keycode) const {
    if (keycode >= 256) return false;
    return keyboardState_.keys[keycode];
}

int USBManager::setKeyboardLEDs(bool caps, bool num, bool scroll) {
    keyboardState_.capsLock = caps;
    keyboardState_.numLock = num;
    keyboardState_.scrollLock = scroll;
    // في نظام حقيقي: SET_REPORT USB request
    return 0;
}

char USBManager::keycodeToASCII(uint8_t keycode, uint8_t modifiers) const {
    if (keycode >= 128) return 0;
    bool shift = (modifiers & (KeyModifier::LEFT_SHIFT | KeyModifier::RIGHT_SHIFT)) != 0;
    bool caps = keyboardState_.capsLock;

    // Caps Lock toggles for letters only
    if (keycode >= 0x04 && keycode <= 0x1D) {
        shift = shift ^ caps;
    }

    return shift ? asciiTableShift_[keycode] : asciiTable_[keycode];
}

void USBManager::processKeyboardReport(const uint8_t* report, size_t length) {
    if (length < 8) return;

    // تقرير لوحة المفاتيح HID Boot Protocol:
    // byte 0: modifiers
    // byte 1: reserved
    // bytes 2-7: keycodes (up to 6 keys)
    keyboardState_.modifiers = report[0];

    // إعادة تعيين كل المفاتيح / Reset all keys
    std::memset(keyboardState_.keys, 0, sizeof(keyboardState_.keys));

    for (size_t i = 2; i < 8 && i < length; i++) {
        uint8_t keycode = report[i];
        if (keycode == 0) continue; // No key
        if (keycode <= 255) {
            keyboardState_.keys[keycode] = true;
            keyboardState_.lastKeycode = keycode;
        }
    }
}

// ============================================================================
// HID — الفأرة / Mouse
// ============================================================================

int USBManager::initMouse() {
    for (size_t i = 0; i < devices_.size(); i++) {
        if (devices_[i].isMouse) {
            mouseAddress_ = devices_[i].address;
            mouseConnected_ = true;
            hidInitialized_ = true;
            return 0;
        }
    }
    mouseConnected_ = false;
    return -1;
}

int USBManager::pollMouse() {
    if (!mouseConnected_) return -1;
    // في نظام حقيقي: interrupt transfer لقراءة تقرير HID
    return 0;
}

void USBManager::setMouseBounds(int32_t minX, int32_t minY,
                                int32_t maxX, int32_t maxY) {
    mouseMinX_ = minX;
    mouseMinY_ = minY;
    mouseMaxX_ = maxX;
    mouseMaxY_ = maxY;
    // Clamp current position
    mouseState_.x = std::clamp(mouseState_.x, minX, maxX);
    mouseState_.y = std::clamp(mouseState_.y, minY, maxY);
}

void USBManager::setMousePosition(int32_t x, int32_t y) {
    mouseState_.x = std::clamp(x, mouseMinX_, mouseMaxX_);
    mouseState_.y = std::clamp(y, mouseMinY_, mouseMaxY_);
    mouseState_.deltaX = 0;
    mouseState_.deltaY = 0;
}

void USBManager::processMouseReport(const uint8_t* report, size_t length) {
    if (length < 3) return;

    // تقرير الفأرة HID Boot Protocol:
    // byte 0: buttons (bit0=left, bit1=right, bit2=middle)
    // byte 1: X delta (signed)
    // byte 2: Y delta (signed)
    // byte 3: scroll wheel (optional)
    mouseState_.buttons = report[0];
    mouseState_.leftButton   = (report[0] & 0x01) != 0;
    mouseState_.rightButton  = (report[0] & 0x02) != 0;
    mouseState_.middleButton = (report[0] & 0x04) != 0;

    mouseState_.deltaX = static_cast<int8_t>(report[1]);
    mouseState_.deltaY = static_cast<int8_t>(report[2]);

    mouseState_.x = std::clamp(mouseState_.x + mouseState_.deltaX, mouseMinX_, mouseMaxX_);
    mouseState_.y = std::clamp(mouseState_.y + mouseState_.deltaY, mouseMinY_, mouseMaxY_);

    if (length >= 4) {
        mouseState_.scrollDelta = static_cast<int8_t>(report[3]);
    }
}

void USBManager::parseHIDReport(const uint8_t* data, size_t length,
                                USBDeviceInfo& dev) {
    (void)data; (void)length;
    // تحليل مبسط — في نظام حقيقي يتم تحليل Report Descriptor الكامل
    // Simplified — real OS would parse full Report Descriptor
    if (dev.subClass == 0x01 && dev.protocol == USBConst::HID_PROTOCOL_KEYBOARD) {
        dev.isKeyboard = true;
        dev.isHID = true;
    } else if (dev.subClass == 0x01 && dev.protocol == USBConst::HID_PROTOCOL_MOUSE) {
        dev.isMouse = true;
        dev.isHID = true;
    }
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string USBManager::generateReport() const {
    std::ostringstream ss;
    ss << "=== USB Manager Report / تقرير مدير USB ===\n";
    ss << "Controllers / متحكمات: " << controllers_.size() << "\n";
    for (size_t i = 0; i < controllers_.size(); i++) {
        const auto& c = controllers_[i].info;
        ss << "  [" << i << "] " << c.name
           << " ports=" << (int)c.numPorts
           << " enabled=" << (c.enabled ? "yes" : "no")
           << " base=0x" << std::hex << c.mmioBase << std::dec << "\n";
    }
    ss << "Devices / أجهزة: " << devices_.size() << "\n";
    for (size_t i = 0; i < devices_.size(); i++) {
        const auto& d = devices_[i];
        ss << "  [" << i << "] addr=" << (int)d.address
           << " vid=0x" << std::hex << d.vendorId
           << " pid=0x" << d.productId << std::dec
           << (d.isKeyboard ? " [Keyboard]" : "")
           << (d.isMouse ? " [Mouse]" : "")
           << "\n";
    }
    ss << "HID: " << (hidInitialized_ ? "initialized" : "not initialized")
       << " keyboard=" << (keyboardConnected_ ? "yes" : "no")
       << " mouse=" << (mouseConnected_ ? "yes" : "no") << "\n";
    if (mouseConnected_) {
        ss << "Mouse: (" << mouseState_.x << "," << mouseState_.y << ")"
           << " buttons=" << (int)mouseState_.buttons << "\n";
    }
    return ss.str();
}

} // namespace LowLevel
} // namespace Sad
