/**
 * @file usb.h
 * @brief (AR) مدير USB الموحد — UHCI/EHCI/xHCI مع HID (لوحة مفاتيح/فأرة)
 * @brief (EN) Unified USB Manager — UHCI/EHCI/xHCI with HID (keyboard/mouse)
 *
 * يدعم التشغيل بدون نظام تشغيل (Freestanding/OS-dev) وعلى سطح المكتب.
 * Supports both freestanding/OS-dev and desktop environments.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <cstring>

namespace Sad {
namespace LowLevel {

// ============================================================================
// أنواع متحكم USB / USB Controller Types
// ============================================================================

/// @brief نوع متحكم USB / USB Controller type
enum class USBControllerType : uint8_t {
    UNKNOWN = 0,
    UHCI    = 1,   // Universal Host Controller Interface (USB 1.x)
    OHCI    = 2,   // Open Host Controller Interface (USB 1.x)
    EHCI    = 3,   // Enhanced Host Controller Interface (USB 2.0)
    XHCI    = 4    // Extensible Host Controller Interface (USB 3.x)
};

/// @brief سرعة جهاز USB / USB Device speed
enum class USBSpeed : uint8_t {
    LOW     = 0,   // 1.5 Mbps (USB 1.0)
    FULL    = 1,   // 12 Mbps (USB 1.1)
    HIGH    = 2,   // 480 Mbps (USB 2.0)
    SUPER   = 3,   // 5 Gbps (USB 3.0)
    SUPER_PLUS = 4 // 10+ Gbps (USB 3.1+)
};

/// @brief حالة نقطة النهاية / Endpoint transfer type
enum class USBTransferType : uint8_t {
    CONTROL     = 0,
    ISOCHRONOUS = 1,
    BULK        = 2,
    INTERRUPT   = 3
};

/// @brief اتجاه النقل / Transfer direction
enum class USBDirection : uint8_t {
    OUT = 0,  // من المضيف إلى الجهاز / Host to device
    IN  = 1   // من الجهاز إلى المضيف / Device to host
};

/// @brief حالة طلب USB / USB Request status
enum class USBStatus : int8_t {
    SUCCESS     =  0,
    STALL       = -1,
    NAK         = -2,
    TIMEOUT     = -3,
    ERROR       = -4,
    NOT_FOUND   = -5,
    NO_DEVICE   = -6
};

// ============================================================================
// واصفات USB القياسية / Standard USB Descriptors
// ============================================================================

#pragma pack(push, 1)

/// @brief واصف الجهاز / Device Descriptor (18 bytes)
struct USBDeviceDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;    // 0x01
    uint16_t bcdUSB;             // e.g. 0x0200 = USB 2.0
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
};

/// @brief واصف الإعداد / Configuration Descriptor
struct USBConfigDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;    // 0x02
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;          // in 2mA units
};

/// @brief واصف الواجهة / Interface Descriptor
struct USBInterfaceDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;    // 0x04
    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;    // 0x03 = HID
    uint8_t  bInterfaceSubClass; // 0x01 = Boot Interface
    uint8_t  bInterfaceProtocol; // 0x01 = Keyboard, 0x02 = Mouse
    uint8_t  iInterface;
};

/// @brief واصف نقطة النهاية / Endpoint Descriptor
struct USBEndpointDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;    // 0x05
    uint8_t  bEndpointAddress;   // bit 7 = direction, bits 0-3 = endpoint number
    uint8_t  bmAttributes;       // bits 0-1 = transfer type
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;          // polling interval in ms
};

/// @brief واصف HID / HID Descriptor
struct USBHIDDescriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;    // 0x21
    uint16_t bcdHID;
    uint8_t  bCountryCode;
    uint8_t  bNumDescriptors;
    uint8_t  bDescriptorType2;   // 0x22 = Report
    uint16_t wDescriptorLength;
};

#pragma pack(pop)

// ============================================================================
// بنى معلومات USB / USB Information Structures
// ============================================================================

/// @brief معلومات متحكم USB / USB Controller info
struct USBControllerInfo {
    USBControllerType type  = USBControllerType::UNKNOWN;
    uint64_t mmioBase       = 0;
    uint32_t mmioSize       = 0;
    uint8_t  busNumber      = 0;
    uint8_t  pciSlot        = 0;
    uint8_t  pciFunction    = 0;
    uint8_t  numPorts       = 0;
    bool     enabled        = false;
    std::string name;
};

/// @brief معلومات جهاز USB / USB Device info
struct USBDeviceInfo {
    uint16_t vendorId       = 0;
    uint16_t productId      = 0;
    uint8_t  address        = 0;
    uint8_t  port           = 0;
    uint8_t  classCode      = 0;
    uint8_t  subClass       = 0;
    uint8_t  protocol       = 0;
    USBSpeed speed          = USBSpeed::FULL;
    bool     isHID          = false;
    bool     isKeyboard     = false;
    bool     isMouse        = false;
    bool     isHub          = false;
    std::string manufacturer;
    std::string product;
    std::string serial;
};

/// @brief معلومات نقطة نهاية / Endpoint info
struct USBEndpointInfo {
    uint8_t        number    = 0;
    USBDirection   direction = USBDirection::OUT;
    USBTransferType type     = USBTransferType::CONTROL;
    uint16_t       maxPacket = 0;
    uint8_t        interval  = 0;
};

// ============================================================================
// بنى HID — لوحة مفاتيح وفأرة / HID — Keyboard & Mouse
// ============================================================================

/// @brief حالة لوحة المفاتيح / Keyboard state
struct KeyboardState {
    bool     keys[256]     = {};   // حالة كل مفتاح / Key state
    uint8_t  modifiers     = 0;    // Shift/Ctrl/Alt/GUI
    uint8_t  lastScancode  = 0;    // آخر مفتاح / Last scancode
    uint8_t  lastKeycode   = 0;    // آخر كود مفتاح / Last keycode
    bool     capsLock      = false;
    bool     numLock       = false;
    bool     scrollLock    = false;
    uint32_t repeatCount   = 0;    // عدد التكرار / Repeat count
};

/// @brief حالة الفأرة / Mouse state
struct MouseState {
    int32_t  x             = 0;    // موقع X / X position
    int32_t  y             = 0;    // موقع Y / Y position
    int32_t  deltaX        = 0;    // تغيير X / X delta
    int32_t  deltaY        = 0;    // تغيير Y / Y delta
    int8_t   scrollDelta   = 0;    // عجلة / Scroll wheel delta
    uint8_t  buttons       = 0;    // bits: 0=left, 1=right, 2=middle
    bool     leftButton    = false;
    bool     rightButton   = false;
    bool     middleButton  = false;
};

/// @brief مُعدّلات لوحة المفاتيح / Keyboard modifiers
namespace KeyModifier {
    constexpr uint8_t NONE       = 0x00;
    constexpr uint8_t LEFT_CTRL  = 0x01;
    constexpr uint8_t LEFT_SHIFT = 0x02;
    constexpr uint8_t LEFT_ALT   = 0x04;
    constexpr uint8_t LEFT_GUI   = 0x08;
    constexpr uint8_t RIGHT_CTRL = 0x10;
    constexpr uint8_t RIGHT_SHIFT= 0x20;
    constexpr uint8_t RIGHT_ALT  = 0x40;
    constexpr uint8_t RIGHT_GUI  = 0x80;
}

/// @brief أكواد مفاتيح HID القياسية / Standard HID keycodes
namespace HIDKeycode {
    constexpr uint8_t KEY_NONE   = 0x00;
    constexpr uint8_t KEY_A      = 0x04;
    constexpr uint8_t KEY_B      = 0x05;
    constexpr uint8_t KEY_Z      = 0x1D;
    constexpr uint8_t KEY_1      = 0x1E;
    constexpr uint8_t KEY_0      = 0x27;
    constexpr uint8_t KEY_ENTER  = 0x28;
    constexpr uint8_t KEY_ESCAPE = 0x29;
    constexpr uint8_t KEY_BSPACE = 0x2A;
    constexpr uint8_t KEY_TAB    = 0x2B;
    constexpr uint8_t KEY_SPACE  = 0x2C;
    constexpr uint8_t KEY_F1     = 0x3A;
    constexpr uint8_t KEY_F12    = 0x45;
    constexpr uint8_t KEY_UP     = 0x52;
    constexpr uint8_t KEY_DOWN   = 0x51;
    constexpr uint8_t KEY_LEFT   = 0x50;
    constexpr uint8_t KEY_RIGHT  = 0x4F;
    constexpr uint8_t KEY_DELETE = 0x4C;
    constexpr uint8_t KEY_HOME   = 0x4A;
    constexpr uint8_t KEY_END    = 0x4D;
    constexpr uint8_t KEY_PGUP   = 0x4B;
    constexpr uint8_t KEY_PGDN   = 0x4E;
}

// ============================================================================
// ثوابت USB / USB Constants
// ============================================================================

namespace USBConst {
    // طلبات الأمر القياسية / Standard request types
    constexpr uint8_t REQ_GET_STATUS       = 0x00;
    constexpr uint8_t REQ_CLEAR_FEATURE    = 0x01;
    constexpr uint8_t REQ_SET_FEATURE      = 0x03;
    constexpr uint8_t REQ_SET_ADDRESS      = 0x05;
    constexpr uint8_t REQ_GET_DESCRIPTOR   = 0x06;
    constexpr uint8_t REQ_SET_CONFIGURATION= 0x09;
    constexpr uint8_t REQ_SET_INTERFACE    = 0x0B;
    
    // أنواع الواصفات / Descriptor types
    constexpr uint8_t DESC_DEVICE          = 0x01;
    constexpr uint8_t DESC_CONFIGURATION   = 0x02;
    constexpr uint8_t DESC_STRING          = 0x03;
    constexpr uint8_t DESC_INTERFACE       = 0x04;
    constexpr uint8_t DESC_ENDPOINT        = 0x05;
    constexpr uint8_t DESC_HID             = 0x21;
    constexpr uint8_t DESC_HID_REPORT      = 0x22;
    
    // فئات USB / USB classes
    constexpr uint8_t CLASS_HID            = 0x03;
    constexpr uint8_t CLASS_HUB            = 0x09;
    constexpr uint8_t CLASS_MASS_STORAGE   = 0x08;
    
    // بروتوكولات HID / HID protocols
    constexpr uint8_t HID_PROTOCOL_NONE    = 0x00;
    constexpr uint8_t HID_PROTOCOL_KEYBOARD= 0x01;
    constexpr uint8_t HID_PROTOCOL_MOUSE   = 0x02;

    // PCI class/subclass for USB controllers
    constexpr uint8_t PCI_CLASS_SERIAL     = 0x0C;
    constexpr uint8_t PCI_SUBCLASS_USB     = 0x03;
    constexpr uint8_t PCI_PROG_UHCI       = 0x00;
    constexpr uint8_t PCI_PROG_OHCI       = 0x10;
    constexpr uint8_t PCI_PROG_EHCI       = 0x20;
    constexpr uint8_t PCI_PROG_XHCI       = 0x30;
    
    // حالة المنفذ / Port status bits
    constexpr uint16_t PORT_CONNECTED      = 0x0001;
    constexpr uint16_t PORT_ENABLED        = 0x0002;
    constexpr uint16_t PORT_RESET          = 0x0010;
    constexpr uint16_t PORT_POWER          = 0x0100;
    constexpr uint16_t PORT_LOW_SPEED      = 0x0200;
    constexpr uint16_t PORT_HIGH_SPEED     = 0x0400;
}

// ============================================================================
// مدير USB / USB Manager
// ============================================================================

/**
 * @brief (AR) مدير USB الموحد — يدعم UHCI/EHCI/xHCI مع HID
 * @brief (EN) Unified USB Manager — UHCI/EHCI/xHCI + HID (keyboard/mouse)
 *
 * Pattern: Singleton
 * Thread-safe: No (use external sync)
 */
class USBManager {
public:
    /// @brief الحصول على النسخة الوحيدة / Get singleton instance
    static USBManager& getInstance() {
        static USBManager instance;
        return instance;
    }

    // ==== التعداد والاكتشاف / Enumeration & Discovery ====

    /// @brief البحث عن متحكمات USB عبر PCI / Scan PCI for USB controllers
    int scanControllers();

    /// @brief عدد المتحكمات المكتشفة / Number of discovered controllers
    size_t getControllerCount() const { return controllers_.size(); }

    /// @brief معلومات متحكم محدد / Get controller info
    USBControllerInfo getControllerInfo(size_t index) const;

    /// @brief تهيئة متحكم محدد / Initialize specific controller
    int initController(size_t index);

    /// @brief إعادة تعيين متحكم / Reset controller
    int resetController(size_t index);

    // ==== إدارة المنافذ / Port Management ====

    /// @brief عدد المنافذ في متحكم / Number of ports in controller
    int getPortCount(size_t controllerIndex) const;

    /// @brief حالة منفذ / Port status
    uint32_t getPortStatus(size_t controllerIndex, uint8_t port) const;

    /// @brief هل المنفذ متصل بجهاز / Is port connected
    bool isPortConnected(size_t controllerIndex, uint8_t port) const;

    /// @brief إعادة تعيين منفذ / Reset port
    int resetPort(size_t controllerIndex, uint8_t port);

    /// @brief تفعيل/تعطيل منفذ / Enable/disable port
    int enablePort(size_t controllerIndex, uint8_t port, bool enable);

    // ==== إدارة الأجهزة / Device Management ====

    /// @brief تعداد الأجهزة المتصلة / Enumerate connected devices
    int enumerateDevices();

    /// @brief عدد الأجهزة المكتشفة / Number of discovered devices
    size_t getDeviceCount() const { return devices_.size(); }

    /// @brief معلومات جهاز / Get device info
    USBDeviceInfo getDeviceInfo(size_t index) const;

    /// @brief تعيين عنوان جهاز / Set device address
    int setDeviceAddress(size_t deviceIndex, uint8_t address);

    /// @brief تعيين إعداد جهاز / Set device configuration
    int setDeviceConfiguration(size_t deviceIndex, uint8_t config);

    // ==== النقل / Transfers ====

    /// @brief نقل تحكم / Control transfer
    int controlTransfer(uint8_t address, uint8_t requestType, uint8_t request,
                        uint16_t value, uint16_t index,
                        void* data, uint16_t length);

    /// @brief نقل مقاطعة / Interrupt transfer (for HID)
    int interruptTransfer(uint8_t address, uint8_t endpoint,
                          void* data, uint16_t length);

    /// @brief نقل بالجملة / Bulk transfer
    int bulkTransfer(uint8_t address, uint8_t endpoint,
                     void* data, uint32_t length);

    // ==== HID — لوحة المفاتيح / Keyboard ====

    /// @brief تهيئة لوحة المفاتيح HID / Initialize HID keyboard
    int initKeyboard();

    /// @brief استقصاء لوحة المفاتيح / Poll keyboard for new events
    int pollKeyboard();

    /// @brief الحصول على حالة لوحة المفاتيح / Get keyboard state
    const KeyboardState& getKeyboardState() const { return keyboardState_; }

    /// @brief هل مفتاح مضغوط / Is key pressed
    bool isKeyPressed(uint8_t keycode) const;

    /// @brief آخر مفتاح مضغوط / Last pressed keycode
    uint8_t getLastKeycode() const { return keyboardState_.lastKeycode; }

    /// @brief آخر scancode / Last scancode
    uint8_t getLastScancode() const { return keyboardState_.lastScancode; }

    /// @brief المُعدّلات الحالية / Current modifiers (shift/ctrl/alt)
    uint8_t getKeyModifiers() const { return keyboardState_.modifiers; }

    /// @brief تعيين مصابيح LED / Set keyboard LEDs (caps/num/scroll)
    int setKeyboardLEDs(bool caps, bool num, bool scroll);

    /// @brief تحويل keycode إلى حرف ASCII / Convert keycode to ASCII
    char keycodeToASCII(uint8_t keycode, uint8_t modifiers) const;

    // ==== HID — الفأرة / Mouse ====

    /// @brief تهيئة الفأرة HID / Initialize HID mouse
    int initMouse();

    /// @brief استقصاء الفأرة / Poll mouse
    int pollMouse();

    /// @brief الحصول على حالة الفأرة / Get mouse state
    const MouseState& getMouseState() const { return mouseState_; }

    /// @brief موقع الفأرة X / Mouse X position
    int32_t getMouseX() const { return mouseState_.x; }

    /// @brief موقع الفأرة Y / Mouse Y position
    int32_t getMouseY() const { return mouseState_.y; }

    /// @brief تحريك الفأرة (delta) / Mouse delta X
    int32_t getMouseDeltaX() const { return mouseState_.deltaX; }

    /// @brief تحريك الفأرة (delta) / Mouse delta Y
    int32_t getMouseDeltaY() const { return mouseState_.deltaY; }

    /// @brief هل زر الفأرة الأيسر مضغوط / Is left button pressed
    bool isLeftButtonPressed() const { return mouseState_.leftButton; }

    /// @brief هل زر الفأرة الأيمن مضغوط / Is right button pressed
    bool isRightButtonPressed() const { return mouseState_.rightButton; }

    /// @brief هل زر الفأرة الأوسط مضغوط / Is middle button pressed
    bool isMiddleButtonPressed() const { return mouseState_.middleButton; }

    /// @brief عجلة التمرير / Scroll wheel delta
    int8_t getScrollDelta() const { return mouseState_.scrollDelta; }

    /// @brief تعيين حدود الفأرة / Set mouse boundaries
    void setMouseBounds(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);

    /// @brief تعيين موقع الفأرة / Set mouse position
    void setMousePosition(int32_t x, int32_t y);

    // ==== استعلام عام / General Query ====

    /// @brief هل HID مهيأ / Is HID initialized
    bool isHIDInitialized() const { return hidInitialized_; }

    /// @brief هل لوحة المفاتيح متصلة / Is keyboard connected
    bool isKeyboardConnected() const { return keyboardConnected_; }

    /// @brief هل الفأرة متصلة / Is mouse connected
    bool isMouseConnected() const { return mouseConnected_; }

    /// @brief إنشاء تقرير / Generate status report
    std::string generateReport() const;

    /// @brief إعادة تعيين الأمان / Safety reset (for SEH on desktop)
    void resetAll() {
        controllers_.clear();
        devices_.clear();
        hidInitialized_ = false;
        keyboardConnected_ = false;
        mouseConnected_ = false;
        keyboardState_ = {};
        mouseState_ = {};
    }

private:
    USBManager() = default;
    USBManager(const USBManager&) = delete;
    USBManager& operator=(const USBManager&) = delete;

    // متحكمات USB / USB Controllers
    struct ControllerState {
        USBControllerInfo info;
        volatile uint32_t* mmioBase = nullptr;
        bool initialized = false;
    };
    std::vector<ControllerState> controllers_;

    // أجهزة USB / USB Devices
    std::vector<USBDeviceInfo> devices_;

    // حالة HID / HID State
    bool hidInitialized_     = false;
    bool keyboardConnected_  = false;
    bool mouseConnected_     = false;
    uint8_t keyboardAddress_ = 0;
    [[maybe_unused]] uint8_t keyboardEndpoint_= 0;
    uint8_t mouseAddress_    = 0;
    [[maybe_unused]] uint8_t mouseEndpoint_   = 0;

    KeyboardState keyboardState_;
    MouseState mouseState_;

    // حدود الفأرة / Mouse bounds
    int32_t mouseMinX_ = 0, mouseMinY_ = 0;
    int32_t mouseMaxX_ = 1920, mouseMaxY_ = 1080;

    // ==== دوال مساعدة / Helper functions ====

    // قراءة/كتابة MMIO
    uint32_t readMMIO(volatile uint32_t* base, uint32_t offset) const;
    void writeMMIO(volatile uint32_t* base, uint32_t offset, uint32_t value);

    // قراءة/كتابة I/O ports (for UHCI)
    uint16_t readIO16(uint16_t port) const;
    void writeIO16(uint16_t port, uint16_t value);

    // تحليل واصفات HID / Parse HID descriptors
    void parseHIDReport(const uint8_t* data, size_t length, USBDeviceInfo& dev);

    // معالجة تقرير لوحة المفاتيح / Process keyboard report
    void processKeyboardReport(const uint8_t* report, size_t length);

    // معالجة تقرير الفأرة / Process mouse report
    void processMouseReport(const uint8_t* report, size_t length);

    // ASCII lookup table
    static const char asciiTable_[128];
    static const char asciiTableShift_[128];
};

} // namespace LowLevel
} // namespace Sad
