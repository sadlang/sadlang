/**
 * @file builtin_registry_part23.cpp
 * @brief (AR) دوال USB/HID — لوحة مفاتيح، فأرة — بدون نظام تشغيل
 * @brief (EN) USB/HID functions — keyboard, mouse — bare-metal / freestanding
 */

#include "interpreter_core.h"
#include "value.h"
#include "usb.h"

#include <vector>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Sad {
namespace Interpreter {

// =====================================================================
// SEH wrappers (Windows) — USB accesses I/O ports and PCI MMIO
// =====================================================================
#ifdef _WIN32

static int sehUSBScan() {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.scanControllers();
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBInit(int idx) {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.initController(idx) ? 0 : -1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBReset(int idx) {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.resetController(idx) ? 0 : -1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBPortCount(int idx) {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.getPortCount(idx);
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBPortStatus(int ctrl, int port) {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.getPortStatus(ctrl, port);
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static int sehUSBPortConnected(int ctrl, int port) {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.isPortConnected(ctrl, port) ? 1 : 0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static int sehUSBResetPort(int ctrl, int port) {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.resetPort(ctrl, port) ? 0 : -1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBEnumerate() {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.enumerateDevices();
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBInitKB() {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.initKeyboard();
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBPollKB() {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.pollKeyboard() ? 1 : 0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static int sehUSBInitMouse() {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.initMouse();
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehUSBPollMouse() {
    __try {
        auto& usb = LowLevel::USBManager::getInstance();
        return usb.pollMouse() ? 1 : 0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

#endif // _WIN32

// =====================================================================
// Registration
// =====================================================================

void registerBuiltinsPart23(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // مسح واكتشاف / USB Scanning & Discovery (5 functions)
    // =================================================================

    // 1. usb_مسح / usb_scan
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBScan());
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.scanControllers());
#endif
        };
        fm.registerBuiltinFunction("usb_\xd9\x85\xd8\xb3\xd8\xad", f);
        fm.registerBuiltinFunction("usb_scan", f);
    }

    // 2. usb_عدد_متحكمات / usb_controller_count
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(usb.getControllerCount()));
        };
        fm.registerBuiltinFunction("usb_\xd8\xb9\xd8\xaf\xd8\xaf_\xd9\x85\xd8\xaa\xd8\xad\xd9\x83\xd9\x85\xd8\xa7\xd8\xaa", f);
        fm.registerBuiltinFunction("usb_controller_count", f);
    }

    // 3. usb_معلومات_متحكم / usb_controller_info
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int idx = args.empty() ? 0 : args[0]->toInt();
            auto& usb = LowLevel::USBManager::getInstance();
            auto info = usb.getControllerInfo(idx);
            std::string typeStr;
            switch (info.type) {
                case LowLevel::USBControllerType::UHCI: typeStr = "UHCI"; break;
                case LowLevel::USBControllerType::OHCI: typeStr = "OHCI"; break;
                case LowLevel::USBControllerType::EHCI: typeStr = "EHCI"; break;
                case LowLevel::USBControllerType::XHCI: typeStr = "xHCI"; break;
                default: typeStr = "UNKNOWN"; break;
            }
            std::string result = typeStr + ":" +
                std::to_string(info.busNumber) + ":" +
                std::to_string(info.pciSlot) + ":" +
                std::to_string(info.numPorts) + ":" +
                (info.enabled ? "1" : "0");
            return std::make_shared<Data::Value>(result);
        };
        fm.registerBuiltinFunction("usb_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa_\xd9\x85\xd8\xaa\xd8\xad\xd9\x83\xd9\x85", f);
        fm.registerBuiltinFunction("usb_controller_info", f);
    }

    // 4. usb_تهيئة_متحكم / usb_init_controller
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int idx = args.empty() ? 0 : args[0]->toInt();
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBInit(idx));
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.initController(idx) ? 0 : -1);
#endif
        };
        fm.registerBuiltinFunction("usb_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9_\xd9\x85\xd8\xaa\xd8\xad\xd9\x83\xd9\x85", f);
        fm.registerBuiltinFunction("usb_init_controller", f);
    }

    // 5. usb_إعادة_تعيين_متحكم / usb_reset_controller
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int idx = args.empty() ? 0 : args[0]->toInt();
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBReset(idx));
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.resetController(idx) ? 0 : -1);
#endif
        };
        fm.registerBuiltinFunction("usb_\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd8\xaa\xd8\xad\xd9\x83\xd9\x85", f);
        fm.registerBuiltinFunction("usb_reset_controller", f);
    }

    // =================================================================
    // منافذ / Port Management (4 functions)
    // =================================================================

    // 6. usb_عدد_منافذ / usb_port_count
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int idx = args.empty() ? 0 : args[0]->toInt();
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBPortCount(idx));
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.getPortCount(idx));
#endif
        };
        fm.registerBuiltinFunction("usb_\xd8\xb9\xd8\xaf\xd8\xaf_\xd9\x85\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0", f);
        fm.registerBuiltinFunction("usb_port_count", f);
    }

    // 7. usb_حالة_منفذ / usb_port_status
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(0);
            int ctrl = args[0]->toInt(), port = args[1]->toInt();
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBPortStatus(ctrl, port));
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(usb.getPortStatus(ctrl, port)));
#endif
        };
        fm.registerBuiltinFunction("usb_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd9\x86\xd9\x81\xd8\xb0", f);
        fm.registerBuiltinFunction("usb_port_status", f);
    }

    // 8. usb_متصل / usb_port_connected
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(0);
            int ctrl = args[0]->toInt(), port = args[1]->toInt();
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBPortConnected(ctrl, port));
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isPortConnected(ctrl, port) ? 1 : 0);
#endif
        };
        fm.registerBuiltinFunction("usb_\xd9\x85\xd8\xaa\xd8\xb5\xd9\x84", f);
        fm.registerBuiltinFunction("usb_port_connected", f);
    }

    // 9. usb_إعادة_تعيين_منفذ / usb_reset_port
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(-1);
            int ctrl = args[0]->toInt(), port = args[1]->toInt();
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBResetPort(ctrl, port));
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.resetPort(ctrl, port) ? 0 : -1);
#endif
        };
        fm.registerBuiltinFunction("usb_\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd9\x86\xd9\x81\xd8\xb0", f);
        fm.registerBuiltinFunction("usb_reset_port", f);
    }

    // =================================================================
    // أجهزة / Device Enumeration (3 functions)
    // =================================================================

    // 10. usb_اكتشاف_أجهزة / usb_enumerate
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBEnumerate());
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.enumerateDevices());
#endif
        };
        fm.registerBuiltinFunction("usb_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xb4\xd8\xa7\xd9\x81_\xd8\xa3\xd8\xac\xd9\x87\xd8\xb2\xd8\xa9", f);
        fm.registerBuiltinFunction("usb_enumerate", f);
    }

    // 11. usb_عدد_أجهزة / usb_device_count
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(usb.getDeviceCount()));
        };
        fm.registerBuiltinFunction("usb_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa3\xd8\xac\xd9\x87\xd8\xb2\xd8\xa9", f);
        fm.registerBuiltinFunction("usb_device_count", f);
    }

    // 12. usb_معلومات_جهاز / usb_device_info
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int idx = args.empty() ? 0 : args[0]->toInt();
            auto& usb = LowLevel::USBManager::getInstance();
            auto info = usb.getDeviceInfo(idx);
            std::string speedStr;
            switch (info.speed) {
                case LowLevel::USBSpeed::LOW: speedStr = "LOW"; break;
                case LowLevel::USBSpeed::FULL: speedStr = "FULL"; break;
                case LowLevel::USBSpeed::HIGH: speedStr = "HIGH"; break;
                case LowLevel::USBSpeed::SUPER: speedStr = "SUPER"; break;
                default: speedStr = "UNKNOWN"; break;
            }
            std::string result = std::to_string(info.address) + ":" +
                speedStr + ":" +
                std::to_string(info.vendorId) + ":" +
                std::to_string(info.productId) + ":" +
                std::to_string(info.classCode) + ":" +
                (info.isHID ? "1" : "0");
            return std::make_shared<Data::Value>(result);
        };
        fm.registerBuiltinFunction("usb_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa_\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2", f);
        fm.registerBuiltinFunction("usb_device_info", f);
    }

    // =================================================================
    // لوحة مفاتيح / Keyboard HID (8 functions)
    // =================================================================

    // 13. لوحة_تهيئة / keyboard_init
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBInitKB());
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.initKeyboard());
#endif
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9", f);
        fm.registerBuiltinFunction("keyboard_init", f);
    }

    // 14. لوحة_استطلاع / keyboard_poll
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBPollKB());
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.pollKeyboard() ? 1 : 0);
#endif
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x84\xd8\xa7\xd8\xb9", f);
        fm.registerBuiltinFunction("keyboard_poll", f);
    }

    // 15. لوحة_مفتاح_مضغوط / keyboard_is_pressed
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(0);
            uint8_t keycode = static_cast<uint8_t>(args[0]->toInt());
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isKeyPressed(keycode) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad_\xd9\x85\xd8\xb6\xd8\xba\xd9\x88\xd8\xb7", f);
        fm.registerBuiltinFunction("keyboard_is_pressed", f);
    }

    // 16. لوحة_آخر_مفتاح / keyboard_last_key
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(usb.getLastKeycode()));
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xa2\xd8\xae\xd8\xb1_\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", f);
        fm.registerBuiltinFunction("keyboard_last_key", f);
    }

    // 17. لوحة_آخر_فحص / keyboard_last_scancode
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(usb.getLastScancode()));
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xa2\xd8\xae\xd8\xb1_\xd9\x81\xd8\xad\xd8\xb5", f);
        fm.registerBuiltinFunction("keyboard_last_scancode", f);
    }

    // 18. لوحة_معدلات / keyboard_modifiers
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(usb.getKeyModifiers()));
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84\xd8\xa7\xd8\xaa", f);
        fm.registerBuiltinFunction("keyboard_modifiers", f);
    }

    // 19. لوحة_إلى_حرف / keyboard_to_ascii
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(0);
            uint8_t keycode = static_cast<uint8_t>(args[0]->toInt());
            bool shift = (args.size() > 1) ? (args[1]->toInt() != 0) : false;
            auto& usb = LowLevel::USBManager::getInstance();
            char c = usb.keycodeToASCII(keycode, shift);
            return std::make_shared<Data::Value>(static_cast<int>(c));
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xa5\xd9\x84\xd9\x89_\xd8\xad\xd8\xb1\xd9\x81", f);
        fm.registerBuiltinFunction("keyboard_to_ascii", f);
    }

    // 20. لوحة_متصلة / keyboard_connected
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isKeyboardConnected() ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd9\x85\xd8\xaa\xd8\xb5\xd9\x84\xd8\xa9", f);
        fm.registerBuiltinFunction("keyboard_connected", f);
    }

    // =================================================================
    // فأرة / Mouse HID (10 functions)
    // =================================================================

    // 21. فأرة_تهيئة / mouse_init
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBInitMouse());
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.initMouse());
#endif
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9", f);
        fm.registerBuiltinFunction("mouse_init", f);
    }

    // 22. فأرة_استطلاع / mouse_poll
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehUSBPollMouse());
#else
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.pollMouse() ? 1 : 0);
#endif
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x84\xd8\xa7\xd8\xb9", f);
        fm.registerBuiltinFunction("mouse_poll", f);
    }

    // 23. فأرة_س / mouse_x
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.getMouseX());
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xb3", f);
        fm.registerBuiltinFunction("mouse_x", f);
    }

    // 24. فأرة_ص / mouse_y
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.getMouseY());
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xb5", f);
        fm.registerBuiltinFunction("mouse_y", f);
    }

    // 25. فأرة_يسار / mouse_left
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isLeftButtonPressed() ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1", f);
        fm.registerBuiltinFunction("mouse_left", f);
    }

    // 26. فأرة_يمين / mouse_right
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isRightButtonPressed() ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86", f);
        fm.registerBuiltinFunction("mouse_right", f);
    }

    // 27. فأرة_وسط / mouse_middle
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isMiddleButtonPressed() ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd9\x88\xd8\xb3\xd8\xb7", f);
        fm.registerBuiltinFunction("mouse_middle", f);
    }

    // 28. فأرة_عجلة / mouse_scroll
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.getScrollDelta());
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xb9\xd8\xac\xd9\x84\xd8\xa9", f);
        fm.registerBuiltinFunction("mouse_scroll", f);
    }

    // 29. فأرة_متصلة / mouse_connected
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isMouseConnected() ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd9\x85\xd8\xaa\xd8\xb5\xd9\x84\xd8\xa9", f);
        fm.registerBuiltinFunction("mouse_connected", f);
    }

    // 30. فأرة_حدود / mouse_set_bounds
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.size() < 2) return std::make_shared<Data::Value>(-1);
            int32_t maxX = args[0]->toInt(), maxY = args[1]->toInt();
            auto& usb = LowLevel::USBManager::getInstance();
            usb.setMouseBounds(0, 0, maxX, maxY);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("\xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", f);
        fm.registerBuiltinFunction("mouse_set_bounds", f);
    }

    // =================================================================
    // عام / General USB (3 functions)
    // =================================================================

    // 31. usb_هل_hid_مهيأ / usb_hid_initialized
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.isHIDInitialized() ? 1 : 0);
        };
        fm.registerBuiltinFunction("usb_\xd9\x87\xd9\x84_hid_\xd9\x85\xd9\x87\xd9\x8a\xd8\xa3", f);
        fm.registerBuiltinFunction("usb_hid_initialized", f);
    }

    // 32. usb_تقرير / usb_report
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            return std::make_shared<Data::Value>(usb.generateReport());
        };
        fm.registerBuiltinFunction("usb_\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1", f);
        fm.registerBuiltinFunction("usb_report", f);
    }

    // 33. usb_إعادة_تعيين_الكل / usb_reset_all
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& usb = LowLevel::USBManager::getInstance();
            usb.resetAll();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("usb_\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84", f);
        fm.registerBuiltinFunction("usb_reset_all", f);
    }

    // =================================================================
    // ثوابت HID / HID Constants (exported as functions)
    // =================================================================

    // 34. لوحة_ثابت / keyboard_const — returns HID keycode constants
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(0);
            std::string name = args[0]->toString();
            // Map common key names to HID keycodes
            if (name == "ENTER" || name == "ادخال") return std::make_shared<Data::Value>(0x28);
            if (name == "ESCAPE" || name == "هروب") return std::make_shared<Data::Value>(0x29);
            if (name == "BACKSPACE" || name == "مسح") return std::make_shared<Data::Value>(0x2A);
            if (name == "TAB" || name == "جدولة") return std::make_shared<Data::Value>(0x2B);
            if (name == "SPACE" || name == "مسافة") return std::make_shared<Data::Value>(0x2C);
            if (name == "UP" || name == "أعلى") return std::make_shared<Data::Value>(0x52);
            if (name == "DOWN" || name == "أسفل") return std::make_shared<Data::Value>(0x51);
            if (name == "LEFT" || name == "يسار") return std::make_shared<Data::Value>(0x50);
            if (name == "RIGHT" || name == "يمين") return std::make_shared<Data::Value>(0x4F);
            if (name == "F1") return std::make_shared<Data::Value>(0x3A);
            if (name == "F2") return std::make_shared<Data::Value>(0x3B);
            if (name == "F3") return std::make_shared<Data::Value>(0x3C);
            if (name == "F4") return std::make_shared<Data::Value>(0x3D);
            if (name == "F5") return std::make_shared<Data::Value>(0x3E);
            if (name == "F10") return std::make_shared<Data::Value>(0x43);
            if (name == "F12") return std::make_shared<Data::Value>(0x45);
            if (name == "DELETE" || name == "حذف") return std::make_shared<Data::Value>(0x4C);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", f);
        fm.registerBuiltinFunction("keyboard_const", f);
    }

} // registerBuiltinsPart23

} // namespace Interpreter
} // namespace Sad
