/**
 * @file esp32.cpp
 * @brief (AR) دعم منصة ESP32
 *        (EN) ESP32 Platform Support
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ███████╗███████╗██████╗ ██████╗ ██████╗                                  ║
 * ║     ██╔════╝██╔════╝██╔══██╗╚════██╗╚════██╗                                 ║
 * ║     █████╗  ███████╗██████╔╝ █████╔╝ █████╔╝                                 ║
 * ║     ██╔══╝  ╚════██║██╔═══╝  ╚═══██╗██╔═══╝                                  ║
 * ║     ███████╗███████║██║     ██████╔╝███████╗                                 ║
 * ║     ╚══════╝╚══════╝╚═╝     ╚═════╝ ╚══════╝                                 ║
 * ║                                                                               ║
 * ║              دعم منصة ESP32 للإنترنت من الأشياء                              ║
 * ║              ESP32 IoT Platform Support                                       ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يوفر دعم منصة ESP32:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     شرائح ESP32 المدعومة                                │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    ┌───────────────┬──────────────┬──────────────┬────────────┐        │
 *      │    │ ESP32         │ ESP32-S2     │ ESP32-S3     │ ESP32-C3   │        │
 *      │    │ Xtensa LX6    │ Xtensa LX7   │ Xtensa LX7   │ RISC-V     │        │
 *      │    │ Dual Core     │ Single Core  │ Dual Core    │ Single     │        │
 *      │    │ 240 MHz       │ 240 MHz      │ 240 MHz      │ 160 MHz    │        │
 *      │    │ WiFi + BT     │ WiFi         │ WiFi + BLE   │ WiFi + BLE │        │
 *      │    │ 520KB RAM     │ 320KB RAM    │ 512KB RAM    │ 400KB RAM  │        │
 *      │    └───────────────┴──────────────┴──────────────┴────────────┘        │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <sstream>

namespace sad {
namespace compiler {
namespace targets {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع شرائح ESP32
//                    (EN) ESP32 Chip Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum ESP32ChipType
 * @brief (AR) أنواع شرائح ESP32
 *        (EN) ESP32 chip types
 */
enum class ESP32ChipType {
    ESP32,              // (AR) الأصلي - Xtensa LX6 ثنائي النواة
    ESP32_S2,           // (AR) S2 - Xtensa LX7 أحادي النواة
    ESP32_S3,           // (AR) S3 - Xtensa LX7 ثنائي النواة مع AI
    ESP32_C3,           // (AR) C3 - RISC-V أحادي النواة
    ESP32_C6,           // (AR) C6 - RISC-V مع WiFi 6
    ESP32_H2,           // (AR) H2 - RISC-V مع 802.15.4/Zigbee
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) معلومات الشريحة
//                    (EN) Chip Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct ESP32ChipInfo
 * @brief (AR) معلومات شريحة ESP32
 *        (EN) ESP32 chip information
 */
struct ESP32ChipInfo {
    ESP32ChipType type;
    std::string name;
    std::string nameArabic;
    std::string architecture;       // xtensa أو riscv32
    int cores;                      // عدد الأنوية
    int clockSpeedMHz;              // سرعة الساعة
    int flashSizeKB;                // حجم Flash الافتراضي
    int internalRamKB;              // حجم RAM الداخلي
    int psramSizeKB;                // حجم PSRAM (إن وجد)
    bool hasWiFi;
    bool hasBluetooth;
    bool hasBLE;
    bool hasZigbee;
    std::string idfTarget;          // اسم الهدف في ESP-IDF
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) قاعدة بيانات الشرائح
//                    (EN) Chip Database
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ESP32ChipDatabase
 * @brief (AR) قاعدة بيانات شرائح ESP32
 *        (EN) ESP32 chip database
 */
class ESP32ChipDatabase {
public:
    /**
     * @brief (AR) الحصول على معلومات شريحة
     *        (EN) Get chip information
     */
    static ESP32ChipInfo getChipInfo(ESP32ChipType type) {
        switch (type) {
            case ESP32ChipType::ESP32:
                return {
                    ESP32ChipType::ESP32,
                    "ESP32",
                    u8"إي إس بي 32",
                    "xtensa",
                    2,              // ثنائي النواة
                    240,            // 240 MHz
                    4096,           // 4 MB Flash (افتراضي)
                    520,            // 520 KB RAM
                    0,              // لا يوجد PSRAM افتراضياً
                    true,           // WiFi
                    true,           // Bluetooth Classic
                    true,           // BLE
                    false,          // لا Zigbee
                    "esp32"
                };
                
            case ESP32ChipType::ESP32_S2:
                return {
                    ESP32ChipType::ESP32_S2,
                    "ESP32-S2",
                    u8"إي إس بي 32-إس 2",
                    "xtensa",
                    1,
                    240,
                    4096,
                    320,
                    0,
                    true,           // WiFi
                    false,          // لا Bluetooth Classic
                    false,          // لا BLE
                    false,
                    "esp32s2"
                };
                
            case ESP32ChipType::ESP32_S3:
                return {
                    ESP32ChipType::ESP32_S3,
                    "ESP32-S3",
                    u8"إي إس بي 32-إس 3",
                    "xtensa",
                    2,
                    240,
                    8192,           // 8 MB Flash عادة
                    512,
                    8192,           // 8 MB PSRAM متاح
                    true,
                    false,
                    true,           // BLE 5.0
                    false,
                    "esp32s3"
                };
                
            case ESP32ChipType::ESP32_C3:
                return {
                    ESP32ChipType::ESP32_C3,
                    "ESP32-C3",
                    u8"إي إس بي 32-سي 3",
                    "riscv32",
                    1,
                    160,
                    4096,
                    400,
                    0,
                    true,
                    false,
                    true,           // BLE 5.0
                    false,
                    "esp32c3"
                };
                
            case ESP32ChipType::ESP32_C6:
                return {
                    ESP32ChipType::ESP32_C6,
                    "ESP32-C6",
                    u8"إي إس بي 32-سي 6",
                    "riscv32",
                    1,
                    160,
                    4096,
                    512,
                    0,
                    true,           // WiFi 6
                    false,
                    true,           // BLE 5.0
                    true,           // 802.15.4/Zigbee
                    "esp32c6"
                };
                
            case ESP32ChipType::ESP32_H2:
                return {
                    ESP32ChipType::ESP32_H2,
                    "ESP32-H2",
                    u8"إي إس بي 32-إتش 2",
                    "riscv32",
                    1,
                    96,
                    4096,
                    320,
                    0,
                    false,          // لا WiFi
                    false,
                    true,           // BLE 5.0
                    true,           // 802.15.4/Zigbee/Thread
                    "esp32h2"
                };
                
            default:
                return getChipInfo(ESP32ChipType::ESP32);
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد الهدف ESP32
//                    (EN) ESP32 Target Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ESP32TargetGenerator
 * @brief (AR) مولّد كود الهدف ESP32
 *        (EN) ESP32 target code generator
 */
class ESP32TargetGenerator {
public:
    ESP32TargetGenerator(ESP32ChipType chip) 
        : chip_(chip), chipInfo_(ESP32ChipDatabase::getChipInfo(chip)) {
    }
    
    /**
     * @brief (AR) توليد نص الربط
     *        (EN) Generate linker script
     */
    std::string generateLinkerScript() {
        std::ostringstream script;
        
        script << "/* ═══════════════════════════════════════════════════════════════ */\n";
        script << "/* " << u8"نص الربط لـ " << chipInfo_.name << " */\n";
        script << "/* ═══════════════════════════════════════════════════════════════ */\n\n";
        
        // (AR) تعريف الذاكرة - يعتمد على ESP-IDF
        script << "MEMORY {\n";
        
        if (chipInfo_.architecture == "xtensa") {
            // (AR) ESP32/S2/S3 - معمارية Xtensa
            script << "  /* " << u8"ذاكرة التعليمات (IRAM)" << " */\n";
            script << "  iram0_0_seg (RX) : org = 0x40080000, len = 0x20000\n";
            script << "  /* " << u8"ذاكرة البيانات (DRAM)" << " */\n";
            script << "  dram0_0_seg (RW) : org = 0x3FFB0000, len = " 
                   << (chipInfo_.internalRamKB * 1024) << "\n";
            script << "  /* " << u8"ذاكرة Flash الخارجية" << " */\n";
            script << "  irom_seg (RX) : org = 0x400D0000, len = 0x330000\n";
            script << "  drom_seg (R) : org = 0x3F400000, len = 0x400000\n";
        } else {
            // (AR) ESP32-C3/C6/H2 - معمارية RISC-V
            script << "  /* " << u8"ذاكرة التعليمات (IRAM)" << " */\n";
            script << "  iram0_0_seg (RX) : org = 0x4037C000, len = 0x60000\n";
            script << "  /* " << u8"ذاكرة البيانات (DRAM)" << " */\n";
            script << "  dram0_0_seg (RW) : org = 0x3FC80000, len = " 
                   << (chipInfo_.internalRamKB * 1024) << "\n";
            script << "  /* " << u8"ذاكرة Flash الخارجية" << " */\n";
            script << "  irom_seg (RX) : org = 0x42000000, len = 0x400000\n";
            script << "  drom_seg (R) : org = 0x3C000000, len = 0x400000\n";
        }
        
        script << "}\n\n";
        
        return script.str();
    }
    
    /**
     * @brief (AR) توليد ثوابت المنافذ GPIO
     *        (EN) Generate GPIO pin constants
     */
    std::string generateGpioConstants() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"ثوابت GPIO لـ " << chipInfo_.name << "\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) عدد منافذ GPIO يعتمد على الشريحة
        int maxGpio = 40;  // ESP32 الأصلي
        if (chip_ == ESP32ChipType::ESP32_S2) maxGpio = 46;
        if (chip_ == ESP32ChipType::ESP32_S3) maxGpio = 48;
        if (chip_ == ESP32ChipType::ESP32_C3) maxGpio = 22;
        if (chip_ == ESP32ChipType::ESP32_C6) maxGpio = 30;
        
        for (int i = 0; i < maxGpio; i++) {
            code << "@GPIO_" << i << " = constant i8 " << i << "\n";
        }
        
        code << "\n";
        
        // (AR) عناوين سجلات GPIO
        code << "; " << u8"عناوين سجلات GPIO" << "\n";
        code << "@GPIO_OUT_REG = constant i32* inttoptr (i32 0x3FF44004 to i32*)\n";
        code << "@GPIO_IN_REG = constant i32* inttoptr (i32 0x3FF4403C to i32*)\n";
        code << "@GPIO_ENABLE_REG = constant i32* inttoptr (i32 0x3FF44020 to i32*)\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد دوال WiFi الأساسية
     *        (EN) Generate WiFi base functions
     */
    std::string generateWiFiFunctions() {
        if (!chipInfo_.hasWiFi) {
            return "; " + std::string(u8"هذه الشريحة لا تدعم WiFi") + "\n";
        }
        
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"دوال WiFi" << "\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) تهيئة WiFi
        code << "; " << u8"تهيئة WiFi" << "\n";
        code << "declare void @wifi_init_sta()\n";
        code << "declare void @wifi_init_ap()\n";
        code << "declare void @wifi_init_apsta()\n\n";
        
        // (AR) الاتصال
        code << "; " << u8"الاتصال بالشبكة" << "\n";
        code << "declare i32 @wifi_connect(i8* %ssid, i8* %password)\n";
        code << "declare void @wifi_disconnect()\n";
        code << "declare i1 @wifi_is_connected()\n\n";
        
        // (AR) معلومات الاتصال
        code << "; " << u8"معلومات الاتصال" << "\n";
        code << "declare void @wifi_get_ip(i8* %buffer)\n";
        code << "declare i32 @wifi_get_rssi()\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد دوال Bluetooth
     *        (EN) Generate Bluetooth functions
     */
    std::string generateBluetoothFunctions() {
        if (!chipInfo_.hasBLE && !chipInfo_.hasBluetooth) {
            return "; " + std::string(u8"هذه الشريحة لا تدعم Bluetooth") + "\n";
        }
        
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"دوال Bluetooth" << "\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        if (chipInfo_.hasBLE) {
            code << "; " << u8"دوال BLE" << "\n";
            code << "declare void @ble_init()\n";
            code << "declare void @ble_deinit()\n";
            code << "declare void @ble_set_device_name(i8* %name)\n";
            code << "declare void @ble_start_advertising()\n";
            code << "declare void @ble_stop_advertising()\n\n";
        }
        
        if (chipInfo_.hasBluetooth) {
            code << "; " << u8"دوال Bluetooth Classic" << "\n";
            code << "declare void @bt_init()\n";
            code << "declare void @bt_deinit()\n";
        }
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد دوال FreeRTOS
     *        (EN) Generate FreeRTOS functions
     */
    std::string generateFreeRTOSFunctions() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"دوال FreeRTOS" << "\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) إدارة المهام
        code << "; " << u8"إدارة المهام" << "\n";
        code << "declare i32 @xTaskCreate(void (i8*)* %task, i8* %name, i32 %stack, ";
        code << "i8* %param, i32 %priority, i8** %handle)\n";
        code << "declare void @vTaskDelete(i8* %handle)\n";
        code << "declare void @vTaskDelay(i32 %ticks)\n";
        code << "declare void @vTaskDelayUntil(i32* %lastWake, i32 %period)\n\n";
        
        // (AR) الإشارات
        code << "; " << u8"الإشارات (Semaphores)" << "\n";
        code << "declare i8* @xSemaphoreCreateBinary()\n";
        code << "declare i8* @xSemaphoreCreateMutex()\n";
        code << "declare i32 @xSemaphoreTake(i8* %sem, i32 %timeout)\n";
        code << "declare i32 @xSemaphoreGive(i8* %sem)\n\n";
        
        // (AR) الطوابير
        code << "; " << u8"الطوابير (Queues)" << "\n";
        code << "declare i8* @xQueueCreate(i32 %length, i32 %itemSize)\n";
        code << "declare i32 @xQueueSend(i8* %queue, i8* %item, i32 %timeout)\n";
        code << "declare i32 @xQueueReceive(i8* %queue, i8* %buffer, i32 %timeout)\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد دوال النوم العميق
     *        (EN) Generate deep sleep functions
     */
    std::string generateDeepSleepFunctions() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"دوال النوم العميق" << "\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        code << "; " << u8"تفعيل مصدر الاستيقاظ" << "\n";
        code << "declare void @esp_sleep_enable_timer_wakeup(i64 %time_us)\n";
        code << "declare void @esp_sleep_enable_ext0_wakeup(i32 %gpio, i32 %level)\n";
        code << "declare void @esp_sleep_enable_ext1_wakeup(i64 %mask, i32 %mode)\n\n";
        
        code << "; " << u8"الدخول في النوم" << "\n";
        code << "declare void @esp_deep_sleep_start()\n";
        code << "declare void @esp_light_sleep_start()\n\n";
        
        code << "; " << u8"معرفة سبب الاستيقاظ" << "\n";
        code << "declare i32 @esp_sleep_get_wakeup_cause()\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) الحصول على علامات المترجم
     *        (EN) Get compiler flags
     */
    std::vector<std::string> getCompilerFlags() {
        std::vector<std::string> flags;
        
        if (chipInfo_.architecture == "xtensa") {
            flags.push_back("-target");
            flags.push_back("xtensa");
            
            if (chip_ == ESP32ChipType::ESP32) {
                flags.push_back("-mcpu=esp32");
            } else if (chip_ == ESP32ChipType::ESP32_S2) {
                flags.push_back("-mcpu=esp32s2");
            } else if (chip_ == ESP32ChipType::ESP32_S3) {
                flags.push_back("-mcpu=esp32s3");
            }
        } else {
            flags.push_back("-target");
            flags.push_back("riscv32-esp-elf");
            flags.push_back("-march=rv32imc");
            flags.push_back("-mabi=ilp32");
        }
        
        // (AR) علامات ESP-IDF المشتركة
        flags.push_back("-DIDF_VER=\"5.0\"");
        flags.push_back("-DESP_PLATFORM");
        flags.push_back("-mlongcalls");
        flags.push_back("-ffunction-sections");
        flags.push_back("-fdata-sections");
        
        return flags;
    }
    
    /**
     * @brief (AR) الحصول على معلومات الشريحة
     *        (EN) Get chip information
     */
    const ESP32ChipInfo& getChipInfo() const {
        return chipInfo_;
    }
    
private:
    ESP32ChipType chip_;
    ESP32ChipInfo chipInfo_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير أهداف ESP32
//                    (EN) ESP32 Target Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ESP32TargetManager
 * @brief (AR) مدير أهداف ESP32
 *        (EN) ESP32 target manager
 */
class ESP32TargetManager {
public:
    /**
     * @brief (AR) تسجيل شريحة
     *        (EN) Register chip
     */
    void registerChip(ESP32ChipType type) {
        generators_[type] = std::make_unique<ESP32TargetGenerator>(type);
    }
    
    /**
     * @brief (AR) الحصول على المولّد
     *        (EN) Get generator
     */
    ESP32TargetGenerator* getGenerator(ESP32ChipType type) {
        auto it = generators_.find(type);
        if (it != generators_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    /**
     * @brief (AR) تهيئة جميع الشرائح
     *        (EN) Initialize all chips
     */
    void initializeAllChips() {
        registerChip(ESP32ChipType::ESP32);
        registerChip(ESP32ChipType::ESP32_S2);
        registerChip(ESP32ChipType::ESP32_S3);
        registerChip(ESP32ChipType::ESP32_C3);
        registerChip(ESP32ChipType::ESP32_C6);
        registerChip(ESP32ChipType::ESP32_H2);
    }
    
private:
    std::map<ESP32ChipType, std::unique_ptr<ESP32TargetGenerator>> generators_;
};

} // namespace targets
} // namespace compiler
} // namespace sad
