/**
 * @file arduino.cpp
 * @brief (AR) دعم منصة Arduino
 *        (EN) Arduino Platform Support
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║      █████╗ ██████╗ ██████╗ ██╗   ██╗██╗███╗   ██╗ ██████╗                   ║
 * ║     ██╔══██╗██╔══██╗██╔══██╗██║   ██║██║████╗  ██║██╔═══██╗                  ║
 * ║     ███████║██████╔╝██║  ██║██║   ██║██║██╔██╗ ██║██║   ██║                  ║
 * ║     ██╔══██║██╔══██╗██║  ██║██║   ██║██║██║╚██╗██║██║   ██║                  ║
 * ║     ██║  ██║██║  ██║██████╔╝╚██████╔╝██║██║ ╚████║╚██████╔╝                  ║
 * ║     ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝                  ║
 * ║                                                                               ║
 * ║              دعم منصة Arduino لتطوير المتحكمات الدقيقة                       ║
 * ║              Arduino Platform Support                                         ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يوفر دعم منصة Arduino:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     لوحات Arduino المدعومة                              │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    ┌───────────────┬──────────────┬──────────────┬────────────┐        │
 *      │    │ Arduino Uno   │ Arduino Mega │ Arduino Nano │ Leonardo   │        │
 *      │    │ ATmega328P    │ ATmega2560   │ ATmega328P   │ ATmega32U4 │        │
 *      │    │ 16 MHz        │ 16 MHz       │ 16 MHz       │ 16 MHz     │        │
 *      │    │ 32KB Flash    │ 256KB Flash  │ 32KB Flash   │ 32KB Flash │        │
 *      │    │ 2KB RAM       │ 8KB RAM      │ 2KB RAM      │ 2.5KB RAM  │        │
 *      │    └───────────────┴──────────────┴──────────────┴────────────┘        │
 *      │                                                                         │
 *      │    ┌───────────────┬──────────────┬──────────────┐                     │
 *      │    │ Arduino Due   │ Arduino Zero │ Arduino MKR  │                     │
 *      │    │ SAM3X8E (ARM) │ SAMD21 (ARM) │ SAMD21 (ARM) │                     │
 *      │    │ 84 MHz        │ 48 MHz       │ 48 MHz       │                     │
 *      │    │ 512KB Flash   │ 256KB Flash  │ 256KB Flash  │                     │
 *      │    │ 96KB RAM      │ 32KB RAM     │ 32KB RAM     │                     │
 *      │    └───────────────┴──────────────┴──────────────┘                     │
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
//                    (AR) أنواع لوحات Arduino
//                    (EN) Arduino Board Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum ArduinoBoardType
 * @brief (AR) أنواع لوحات Arduino المدعومة
 *        (EN) Supported Arduino board types
 */
enum class ArduinoBoardType {
    // (AR) لوحات AVR
    UNO,            // ATmega328P
    NANO,           // ATmega328P
    MEGA,           // ATmega2560
    LEONARDO,       // ATmega32U4
    MICRO,          // ATmega32U4
    PRO_MINI,       // ATmega328P
    
    // (AR) لوحات ARM
    DUE,            // SAM3X8E (ARM Cortex-M3)
    ZERO,           // SAMD21 (ARM Cortex-M0+)
    MKR_ZERO,       // SAMD21
    MKR_WIFI_1010,  // SAMD21 + NINA-W102
    MKR_1000,       // SAMD21 + WINC1500
    
    // (AR) لوحات أخرى
    PORTENTA_H7,    // STM32H747 (ARM Cortex-M7/M4)
    NANO_33_BLE,    // nRF52840 (ARM Cortex-M4)
    NANO_33_IOT,    // SAMD21 + NINA-W102
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) معلومات اللوحة
//                    (EN) Board Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct ArduinoBoardInfo
 * @brief (AR) معلومات لوحة Arduino
 *        (EN) Arduino board information
 */
struct ArduinoBoardInfo {
    ArduinoBoardType type;
    std::string name;               // (AR) اسم اللوحة
    std::string nameArabic;         // (AR) الاسم بالعربية
    std::string mcu;                // (AR) المعالج الدقيق
    std::string architecture;       // (AR) المعمارية (avr, arm)
    int clockSpeed;                 // (AR) سرعة الساعة بـ Hz
    int flashSize;                  // (AR) حجم Flash بالكيلوبايت
    int ramSize;                    // (AR) حجم RAM بالكيلوبايت
    int eepromSize;                 // (AR) حجم EEPROM بالبايت
    int digitalPins;                // (AR) عدد المنافذ الرقمية
    int analogInputs;               // (AR) عدد مداخل التماثلية
    int pwmPins;                    // (AR) عدد منافذ PWM
    std::string uploadSpeed;        // (AR) سرعة الرفع
    std::string bootloader;         // (AR) نوع محمل الإقلاع
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) قاعدة بيانات اللوحات
//                    (EN) Board Database
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ArduinoBoardDatabase
 * @brief (AR) قاعدة بيانات لوحات Arduino
 *        (EN) Arduino board database
 */
class ArduinoBoardDatabase {
public:
    /**
     * @brief (AR) الحصول على معلومات لوحة
     *        (EN) Get board information
     */
    static ArduinoBoardInfo getBoardInfo(ArduinoBoardType type) {
        switch (type) {
            case ArduinoBoardType::UNO:
                return {
                    ArduinoBoardType::UNO,
                    "Arduino Uno",
                    u8"أردوينو أونو",
                    "ATmega328P",
                    "avr",
                    16000000,       // 16 MHz
                    32,             // 32 KB Flash
                    2,              // 2 KB RAM
                    1024,           // 1 KB EEPROM
                    14,             // Digital pins
                    6,              // Analog inputs
                    6,              // PWM pins
                    "115200",
                    "optiboot"
                };
                
            case ArduinoBoardType::NANO:
                return {
                    ArduinoBoardType::NANO,
                    "Arduino Nano",
                    u8"أردوينو نانو",
                    "ATmega328P",
                    "avr",
                    16000000,
                    32,
                    2,
                    1024,
                    22,
                    8,
                    6,
                    "57600",
                    "optiboot"
                };
                
            case ArduinoBoardType::MEGA:
                return {
                    ArduinoBoardType::MEGA,
                    "Arduino Mega 2560",
                    u8"أردوينو ميجا",
                    "ATmega2560",
                    "avr",
                    16000000,
                    256,
                    8,
                    4096,
                    54,
                    16,
                    15,
                    "115200",
                    "stk500v2"
                };
                
            case ArduinoBoardType::LEONARDO:
                return {
                    ArduinoBoardType::LEONARDO,
                    "Arduino Leonardo",
                    u8"أردوينو ليوناردو",
                    "ATmega32U4",
                    "avr",
                    16000000,
                    32,
                    2,              // 2.5 KB actually
                    1024,
                    20,
                    12,
                    7,
                    "57600",
                    "caterina"
                };
                
            case ArduinoBoardType::DUE:
                return {
                    ArduinoBoardType::DUE,
                    "Arduino Due",
                    u8"أردوينو ديو",
                    "SAM3X8E",
                    "arm",
                    84000000,       // 84 MHz
                    512,
                    96,
                    0,              // No EEPROM
                    54,
                    12,
                    12,
                    "115200",
                    "bossac"
                };
                
            case ArduinoBoardType::ZERO:
                return {
                    ArduinoBoardType::ZERO,
                    "Arduino Zero",
                    u8"أردوينو زيرو",
                    "SAMD21G18A",
                    "arm",
                    48000000,       // 48 MHz
                    256,
                    32,
                    0,
                    20,
                    6,
                    10,
                    "115200",
                    "bossac"
                };
                
            default:
                return getBoardInfo(ArduinoBoardType::UNO);
        }
    }
    
    /**
     * @brief (AR) الحصول على جميع اللوحات
     *        (EN) Get all boards
     */
    static std::vector<ArduinoBoardType> getAllBoards() {
        return {
            ArduinoBoardType::UNO,
            ArduinoBoardType::NANO,
            ArduinoBoardType::MEGA,
            ArduinoBoardType::LEONARDO,
            ArduinoBoardType::DUE,
            ArduinoBoardType::ZERO,
        };
    }
    
    /**
     * @brief (AR) الحصول على لوحات AVR
     *        (EN) Get AVR boards
     */
    static std::vector<ArduinoBoardType> getAvrBoards() {
        return {
            ArduinoBoardType::UNO,
            ArduinoBoardType::NANO,
            ArduinoBoardType::MEGA,
            ArduinoBoardType::LEONARDO,
            ArduinoBoardType::PRO_MINI,
        };
    }
    
    /**
     * @brief (AR) الحصول على لوحات ARM
     *        (EN) Get ARM boards
     */
    static std::vector<ArduinoBoardType> getArmBoards() {
        return {
            ArduinoBoardType::DUE,
            ArduinoBoardType::ZERO,
            ArduinoBoardType::MKR_ZERO,
        };
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد الهدف Arduino
//                    (EN) Arduino Target Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ArduinoTargetGenerator
 * @brief (AR) مولّد كود الهدف Arduino
 *        (EN) Arduino target code generator
 */
class ArduinoTargetGenerator {
public:
    ArduinoTargetGenerator(ArduinoBoardType board) 
        : board_(board), boardInfo_(ArduinoBoardDatabase::getBoardInfo(board)) {
    }
    
    /**
     * @brief (AR) توليد نص الربط (Linker Script) لـ AVR
     *        (EN) Generate AVR linker script
     */
    std::string generateAvrLinkerScript() {
        std::ostringstream script;
        
        script << "/* ═══════════════════════════════════════════════════════════════ */\n";
        script << "/* " << u8"نص الربط لـ " << boardInfo_.name << " */\n";
        script << "/* " << u8"المعالج: " << boardInfo_.mcu << " */\n";
        script << "/* ═══════════════════════════════════════════════════════════════ */\n\n";
        
        // (AR) تعريف الذاكرة
        script << "MEMORY {\n";
        script << "  text   (rx)   : ORIGIN = 0x0000, LENGTH = " << (boardInfo_.flashSize * 1024) << "\n";
        script << "  data   (rw!x) : ORIGIN = 0x800100, LENGTH = " << (boardInfo_.ramSize * 1024) << "\n";
        
        if (boardInfo_.eepromSize > 0) {
            script << "  eeprom (rw!x) : ORIGIN = 0x810000, LENGTH = " << boardInfo_.eepromSize << "\n";
        }
        
        script << "}\n\n";
        
        // (AR) الأقسام
        script << "SECTIONS {\n";
        script << "  .text : {\n";
        script << "    . = 0x0;\n";
        script << "    *(.vectors)\n";
        script << "    *(.text)\n";
        script << "    *(.text.*)\n";
        script << "  } > text\n\n";
        
        script << "  .data : {\n";
        script << "    *(.data)\n";
        script << "    *(.data.*)\n";
        script << "    *(.rodata)\n";
        script << "    *(.rodata.*)\n";
        script << "  } > data AT > text\n\n";
        
        script << "  .bss : {\n";
        script << "    *(.bss)\n";
        script << "    *(.bss.*)\n";
        script << "  } > data\n";
        script << "}\n";
        
        return script.str();
    }
    
    /**
     * @brief (AR) توليد جدول النواقل لـ AVR
     *        (EN) Generate AVR vector table
     */
    std::string generateAvrVectorTable() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"جدول نواقل المقاطعات لـ " << boardInfo_.mcu << "\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        code << ".section .vectors, \"ax\", @progbits\n";
        code << ".global __vectors\n";
        code << "__vectors:\n";
        
        // (AR) المقاطعات الأساسية لـ ATmega328P
        if (boardInfo_.mcu == "ATmega328P") {
            std::vector<std::string> vectors = {
                "RESET",        // 0
                "INT0",         // 1
                "INT1",         // 2
                "PCINT0",       // 3
                "PCINT1",       // 4
                "PCINT2",       // 5
                "WDT",          // 6
                "TIMER2_COMPA", // 7
                "TIMER2_COMPB", // 8
                "TIMER2_OVF",   // 9
                "TIMER1_CAPT",  // 10
                "TIMER1_COMPA", // 11
                "TIMER1_COMPB", // 12
                "TIMER1_OVF",   // 13
                "TIMER0_COMPA", // 14
                "TIMER0_COMPB", // 15
                "TIMER0_OVF",   // 16
                "SPI_STC",      // 17
                "USART_RX",     // 18
                "USART_UDRE",   // 19
                "USART_TX",     // 20
                "ADC",          // 21
                "EE_READY",     // 22
                "ANALOG_COMP",  // 23
                "TWI",          // 24
                "SPM_READY",    // 25
            };
            
            for (size_t i = 0; i < vectors.size(); i++) {
                code << "  jmp __vector_" << i << "  ; " << vectors[i] << "\n";
            }
        }
        
        code << "\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد دالة setup/loop wrapper
     *        (EN) Generate setup/loop wrapper
     */
    std::string generateSetupLoopWrapper() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"غلاف دالتي إعداد() و حلقة()" << "\n";
        code << "; Arduino setup/loop wrapper\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        code << "define void @main() {\n";
        code << "entry:\n";
        code << "  ; " << u8"تهيئة الأجهزة" << "\n";
        code << "  call void @__arduino_init()\n";
        code << "\n";
        code << "  ; " << u8"استدعاء دالة الإعداد" << "\n";
        code << "  call void @_Z5setupv()  ; setup()\n";
        code << "\n";
        code << "  ; " << u8"تفعيل المقاطعات" << "\n";
        code << "  call void asm sideeffect \"sei\", \"\"()\n";
        code << "\n";
        code << "  ; " << u8"حلقة لا نهائية" << "\n";
        code << "  br label %loop\n";
        code << "loop:\n";
        code << "  call void @_Z4loopv()  ; loop()\n";
        code << "  br label %loop\n";
        code << "}\n\n";
        
        // (AR) دالة تهيئة الأجهزة
        code << "define internal void @__arduino_init() {\n";
        code << "entry:\n";
        code << "  ; " << u8"تهيئة الساعة" << "\n";
        code << "  call void @__init_clock()\n";
        code << "  ; " << u8"تهيئة المؤقت للدالة millis()" << "\n";
        code << "  call void @__init_timer0()\n";
        code << "  ; " << u8"تهيئة ADC" << "\n";
        code << "  call void @__init_adc()\n";
        code << "  ret void\n";
        code << "}\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد ثوابت المنافذ
     *        (EN) Generate pin constants
     */
    std::string generatePinConstants() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"ثوابت المنافذ لـ " << boardInfo_.name << "\n";
        code << "; Pin Constants\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        if (boardInfo_.mcu == "ATmega328P") {
            // (AR) منافذ Arduino Uno/Nano
            code << "; " << u8"المنافذ الرقمية" << "\n";
            code << "@PIN_D0 = constant i8 0\n";
            code << "@PIN_D1 = constant i8 1\n";
            code << "@PIN_D2 = constant i8 2\n";
            code << "@PIN_D3 = constant i8 3  ; PWM\n";
            code << "@PIN_D4 = constant i8 4\n";
            code << "@PIN_D5 = constant i8 5  ; PWM\n";
            code << "@PIN_D6 = constant i8 6  ; PWM\n";
            code << "@PIN_D7 = constant i8 7\n";
            code << "@PIN_D8 = constant i8 8\n";
            code << "@PIN_D9 = constant i8 9  ; PWM\n";
            code << "@PIN_D10 = constant i8 10 ; PWM, SS\n";
            code << "@PIN_D11 = constant i8 11 ; PWM, MOSI\n";
            code << "@PIN_D12 = constant i8 12 ; MISO\n";
            code << "@PIN_D13 = constant i8 13 ; SCK, LED\n";
            code << "\n";
            
            code << "; " << u8"المنافذ التماثلية" << "\n";
            code << "@PIN_A0 = constant i8 14\n";
            code << "@PIN_A1 = constant i8 15\n";
            code << "@PIN_A2 = constant i8 16\n";
            code << "@PIN_A3 = constant i8 17\n";
            code << "@PIN_A4 = constant i8 18 ; SDA\n";
            code << "@PIN_A5 = constant i8 19 ; SCL\n";
            code << "\n";
            
            code << "; " << u8"أوضاع المنافذ" << "\n";
            code << "@INPUT = constant i8 0\n";
            code << "@OUTPUT = constant i8 1\n";
            code << "@INPUT_PULLUP = constant i8 2\n";
            code << "\n";
            
            code << "; " << u8"قيم المنافذ" << "\n";
            code << "@LOW = constant i8 0\n";
            code << "@HIGH = constant i8 1\n";
            code << "\n";
            
            // (AR) عناوين السجلات
            code << "; " << u8"عناوين سجلات المنافذ" << "\n";
            code << "@DDRB = constant i8* inttoptr (i16 36 to i8*)\n";
            code << "@PORTB = constant i8* inttoptr (i16 37 to i8*)\n";
            code << "@PINB = constant i8* inttoptr (i16 35 to i8*)\n";
            code << "@DDRC = constant i8* inttoptr (i16 39 to i8*)\n";
            code << "@PORTC = constant i8* inttoptr (i16 40 to i8*)\n";
            code << "@PINC = constant i8* inttoptr (i16 38 to i8*)\n";
            code << "@DDRD = constant i8* inttoptr (i16 42 to i8*)\n";
            code << "@PORTD = constant i8* inttoptr (i16 43 to i8*)\n";
            code << "@PIND = constant i8* inttoptr (i16 41 to i8*)\n";
        }
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد دوال Arduino الأساسية
     *        (EN) Generate core Arduino functions
     */
    std::string generateCoreFunctions() {
        std::ostringstream code;
        
        code << "; ═══════════════════════════════════════════════════════════════\n";
        code << "; " << u8"دوال Arduino الأساسية" << "\n";
        code << "; Core Arduino Functions\n";
        code << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) pinMode
        code << "; " << u8"تحديد وضع المنفذ - pinMode(pin, mode)" << "\n";
        code << "define void @pinMode(i8 %pin, i8 %mode) {\n";
        code << "entry:\n";
        code << "  ; " << u8"حساب المنفذ والبت" << "\n";
        code << "  %port_idx = udiv i8 %pin, 8\n";
        code << "  %bit_pos = urem i8 %pin, 8\n";
        code << "  %bit_mask = shl i8 1, %bit_pos\n";
        code << "  ; " << u8"تعيين اتجاه المنفذ" << "\n";
        code << "  ; ... (implementation)\n";
        code << "  ret void\n";
        code << "}\n\n";
        
        // (AR) digitalWrite
        code << "; " << u8"كتابة رقمية - digitalWrite(pin, value)" << "\n";
        code << "define void @digitalWrite(i8 %pin, i8 %value) {\n";
        code << "entry:\n";
        code << "  %port_idx = udiv i8 %pin, 8\n";
        code << "  %bit_pos = urem i8 %pin, 8\n";
        code << "  %bit_mask = shl i8 1, %bit_pos\n";
        code << "  ; " << u8"تعيين قيمة المنفذ" << "\n";
        code << "  ret void\n";
        code << "}\n\n";
        
        // (AR) digitalRead
        code << "; " << u8"قراءة رقمية - digitalRead(pin)" << "\n";
        code << "define i8 @digitalRead(i8 %pin) {\n";
        code << "entry:\n";
        code << "  %port_idx = udiv i8 %pin, 8\n";
        code << "  %bit_pos = urem i8 %pin, 8\n";
        code << "  %bit_mask = shl i8 1, %bit_pos\n";
        code << "  ; " << u8"قراءة قيمة المنفذ" << "\n";
        code << "  ret i8 0\n";
        code << "}\n\n";
        
        // (AR) analogRead
        code << "; " << u8"قراءة تماثلية - analogRead(pin)" << "\n";
        code << "define i16 @analogRead(i8 %pin) {\n";
        code << "entry:\n";
        code << "  ; " << u8"تحديد قناة ADC" << "\n";
        code << "  ; " << u8"بدء التحويل" << "\n";
        code << "  ; " << u8"انتظار الانتهاء" << "\n";
        code << "  ; " << u8"قراءة النتيجة" << "\n";
        code << "  ret i16 0\n";
        code << "}\n\n";
        
        // (AR) analogWrite (PWM)
        code << "; " << u8"كتابة تماثلية (PWM) - analogWrite(pin, value)" << "\n";
        code << "define void @analogWrite(i8 %pin, i8 %value) {\n";
        code << "entry:\n";
        code << "  ; " << u8"تهيئة PWM للمنفذ" << "\n";
        code << "  ; " << u8"تعيين دورة العمل" << "\n";
        code << "  ret void\n";
        code << "}\n\n";
        
        // (AR) delay
        code << "; " << u8"تأخير بالمللي ثانية - delay(ms)" << "\n";
        code << "define void @delay(i32 %ms) {\n";
        code << "entry:\n";
        code << "  ; " << u8"استخدام المؤقت للتأخير" << "\n";
        code << "  ret void\n";
        code << "}\n\n";
        
        // (AR) millis
        code << "; " << u8"الوقت منذ البدء بالمللي ثانية - millis()" << "\n";
        code << "@__millis_counter = internal global i32 0\n";
        code << "define i32 @millis() {\n";
        code << "entry:\n";
        code << "  %val = load volatile i32, i32* @__millis_counter\n";
        code << "  ret i32 %val\n";
        code << "}\n\n";
        
        // (AR) micros
        code << "; " << u8"الوقت منذ البدء بالمايكرو ثانية - micros()" << "\n";
        code << "define i32 @micros() {\n";
        code << "entry:\n";
        code << "  ; " << u8"حساب الوقت من المؤقت" << "\n";
        code << "  ret i32 0\n";
        code << "}\n\n";
        
        return code.str();
    }
    
    /**
     * @brief (AR) الحصول على علامات المترجم
     *        (EN) Get compiler flags
     */
    std::vector<std::string> getCompilerFlags() {
        std::vector<std::string> flags;
        
        if (boardInfo_.architecture == "avr") {
            flags.push_back("-target");
            flags.push_back("avr");
            flags.push_back("-mmcu=" + boardInfo_.mcu);
            flags.push_back("-DF_CPU=" + std::to_string(boardInfo_.clockSpeed) + "L");
            flags.push_back("-Os");  // (AR) تحسين الحجم
            flags.push_back("-ffunction-sections");
            flags.push_back("-fdata-sections");
        } else if (boardInfo_.architecture == "arm") {
            flags.push_back("-target");
            flags.push_back("arm-none-eabi");
            flags.push_back("-mcpu=cortex-m3");  // (AR) يعتمد على اللوحة
            flags.push_back("-mthumb");
            flags.push_back("-DF_CPU=" + std::to_string(boardInfo_.clockSpeed) + "L");
            flags.push_back("-Os");
        }
        
        return flags;
    }
    
    /**
     * @brief (AR) الحصول على علامات الرابط
     *        (EN) Get linker flags
     */
    std::vector<std::string> getLinkerFlags() {
        std::vector<std::string> flags;
        
        if (boardInfo_.architecture == "avr") {
            flags.push_back("-Wl,--gc-sections");
            flags.push_back("-mmcu=" + boardInfo_.mcu);
        } else if (boardInfo_.architecture == "arm") {
            flags.push_back("-Wl,--gc-sections");
            flags.push_back("-nostartfiles");
            flags.push_back("--specs=nano.specs");
        }
        
        return flags;
    }
    
private:
    ArduinoBoardType board_;
    ArduinoBoardInfo boardInfo_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير أهداف Arduino
//                    (EN) Arduino Target Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ArduinoTargetManager
 * @brief (AR) مدير أهداف Arduino
 *        (EN) Arduino target manager
 */
class ArduinoTargetManager {
public:
    /**
     * @brief (AR) تسجيل لوحة جديدة
     *        (EN) Register new board
     */
    void registerBoard(ArduinoBoardType type) {
        generators_[type] = std::make_unique<ArduinoTargetGenerator>(type);
    }
    
    /**
     * @brief (AR) الحصول على مولّد اللوحة
     *        (EN) Get board generator
     */
    ArduinoTargetGenerator* getGenerator(ArduinoBoardType type) {
        auto it = generators_.find(type);
        if (it != generators_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    /**
     * @brief (AR) تهيئة جميع اللوحات المدعومة
     *        (EN) Initialize all supported boards
     */
    void initializeAllBoards() {
        for (auto type : ArduinoBoardDatabase::getAllBoards()) {
            registerBoard(type);
        }
    }
    
private:
    std::map<ArduinoBoardType, std::unique_ptr<ArduinoTargetGenerator>> generators_;
};

} // namespace targets
} // namespace compiler
} // namespace sad
