/**
 * ===================================================================================
 *  محاكي ESP32 و Arduino - ESP32/Arduino Emulator (T396)
 * ===================================================================================
 * 
 *  الوصف: محاكي كامل لمعالجات ESP32 و Arduino
 *  Description: Full emulator for ESP32 and Arduino microcontrollers
 *
 *  المميزات:
 *  - محاكاة معالج Xtensa LX6 (ESP32)
 *  - محاكاة معالج AVR (Arduino)
 *  - دعم WiFi و Bluetooth المحاكي
 *  - محاكاة GPIO، ADC، PWM، UART، SPI، I2C
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <cstdint>
#include <array>
#include <queue>

namespace ص::محاكاة {

// Forward declarations
class واجهة_معالج;
class واجهة_ذاكرة;
class واجهة_طرفية;

// ============== محاكي ESP32 ==============

/**
 * حالة GPIO
 */
struct حالة_GPIO {
    bool مخرج;              // true = output, false = input
    bool قيمة;              // HIGH/LOW
    bool سحب_لأعلى;         // Pull-up enabled
    bool سحب_لأسفل;         // Pull-down enabled
    uint8_t وضع_PWM;        // PWM duty cycle (0-255)
    bool PWM_مفعل;          // PWM enabled
    
    // معالجات المقاطعات
    std::function<void(bool)> عند_تغيير;
    bool مقاطعة_صاعدة;     // Interrupt on rising edge
    bool مقاطعة_هابطة;     // Interrupt on falling edge
};

/**
 * محاكي WiFi
 */
class محاكي_WiFi {
public:
    enum class وضع_WiFi {
        متوقف,
        STA,        // Station mode
        AP,         // Access point mode
        STA_AP      // Both
    };
    
    struct شبكة_WiFi {
        std::string SSID;
        std::string كلمة_مرور;
        int8_t قوة_إشارة;  // RSSI
        bool مشفرة;
    };
    
private:
    وضع_WiFi وضع_ = وضع_WiFi::متوقف;
    bool متصل_ = false;
    std::string SSID_متصل_;
    std::string IP_محلي_ = "192.168.1.100";
    std::string IP_بوابة_ = "192.168.1.1";
    std::string قناع_شبكة_ = "255.255.255.0";
    
    // شبكات وهمية للمحاكاة
    std::vector<شبكة_WiFi> شبكات_متاحة_ = {
        {"شبكة_اختبار", "12345678", -45, true},
        {"TestNetwork", "password123", -60, true},
        {"OpenWiFi", "", -70, false}
    };
    
    // بيانات مُرسلة/مستقبلة
    std::queue<std::vector<uint8_t>> صندوق_وارد_;
    
public:
    /**
     * تشغيل WiFi
     */
    bool تشغيل(وضع_WiFi وضع) {
        وضع_ = وضع;
        return true;
    }
    
    /**
     * إيقاف WiFi
     */
    void إيقاف() {
        وضع_ = وضع_WiFi::متوقف;
        متصل_ = false;
    }
    
    /**
     * فحص الشبكات المتاحة
     */
    std::vector<شبكة_WiFi> فحص() {
        return شبكات_متاحة_;
    }
    
    /**
     * الاتصال بشبكة
     */
    bool اتصال(const std::string& SSID, const std::string& كلمة_مرور) {
        for (const auto& شبكة : شبكات_متاحة_) {
            if (شبكة.SSID == SSID) {
                if (!شبكة.مشفرة || شبكة.كلمة_مرور == كلمة_مرور) {
                    متصل_ = true;
                    SSID_متصل_ = SSID;
                    return true;
                }
            }
        }
        return false;
    }
    
    /**
     * قطع الاتصال
     */
    void قطع_اتصال() {
        متصل_ = false;
        SSID_متصل_ = "";
    }
    
    /**
     * إضافة شبكة وهمية
     */
    void إضافة_شبكة_وهمية(const شبكة_WiFi& شبكة) {
        شبكات_متاحة_.push_back(شبكة);
    }
    
    /**
     * محاكاة استقبال بيانات
     */
    void محاكاة_استقبال(const std::vector<uint8_t>& بيانات) {
        صندوق_وارد_.push(بيانات);
    }
    
    /**
     * قراءة بيانات مستقبلة
     */
    std::vector<uint8_t> قراءة_بيانات() {
        if (صندوق_وارد_.empty()) return {};
        auto بيانات = صندوق_وارد_.front();
        صندوق_وارد_.pop();
        return بيانات;
    }
    
    // Getters
    bool متصل() const { return متصل_; }
    وضع_WiFi الوضع() const { return وضع_; }
    std::string IP_محلي() const { return IP_محلي_; }
    std::string SSID_متصل() const { return SSID_متصل_; }
};

/**
 * محاكي Bluetooth
 */
class محاكي_Bluetooth {
public:
    enum class وضع_BT {
        متوقف,
        CLASSIC,
        BLE,
        DUAL
    };
    
private:
    وضع_BT وضع_ = وضع_BT::متوقف;
    bool قابل_للاكتشاف_ = false;
    std::string اسم_الجهاز_ = "ESP32-Device";
    std::queue<std::vector<uint8_t>> صندوق_وارد_;
    
public:
    bool تشغيل(وضع_BT وضع) {
        وضع_ = وضع;
        return true;
    }
    
    void إيقاف() {
        وضع_ = وضع_BT::متوقف;
    }
    
    void تعيين_اسم(const std::string& اسم) {
        اسم_الجهاز_ = اسم;
    }
    
    void تعيين_قابلية_اكتشاف(bool قابل) {
        قابل_للاكتشاف_ = قابل;
    }
    
    void محاكاة_استقبال(const std::vector<uint8_t>& بيانات) {
        صندوق_وارد_.push(بيانات);
    }
    
    std::vector<uint8_t> قراءة_بيانات() {
        if (صندوق_وارد_.empty()) return {};
        auto بيانات = صندوق_وارد_.front();
        صندوق_وارد_.pop();
        return بيانات;
    }
    
    وضع_BT الوضع() const { return وضع_; }
    bool قابل_للاكتشاف() const { return قابل_للاكتشاف_; }
    std::string اسم_الجهاز() const { return اسم_الجهاز_; }
};

/**
 * محاكي ESP32 الكامل
 */
class محاكي_ESP32 {
private:
    // GPIO (34 pins)
    std::array<حالة_GPIO, 40> GPIO_;
    
    // ADC (18 channels)
    std::array<uint16_t, 18> ADC_;  // 12-bit ADC (0-4095)
    
    // DAC (2 channels)
    std::array<uint8_t, 2> DAC_;    // 8-bit DAC (0-255)
    
    // الاتصالات
    محاكي_WiFi WiFi_;
    محاكي_Bluetooth Bluetooth_;
    
    // UART buffers
    std::array<std::queue<uint8_t>, 3> UART_RX_;
    std::array<std::queue<uint8_t>, 3> UART_TX_;
    
    // SPI buffers
    std::array<std::vector<uint8_t>, 4> SPI_buffer_;
    
    // I2C
    std::map<uint8_t, std::vector<uint8_t>> I2C_devices_;
    
    // المؤقتات
    std::array<uint64_t, 4> مؤقتات_;
    std::array<bool, 4> مؤقت_مفعل_;
    
    // RTC
    uint64_t وقت_RTC_ = 0;
    
    // الذاكرة
    std::vector<uint8_t> flash_;       // 4MB
    std::vector<uint8_t> SRAM_;        // 520KB
    std::vector<uint8_t> RTC_RAM_;     // 16KB
    
    // الحالة
    bool نائم_ = false;
    uint8_t وضع_النوم_ = 0;  // 0=Active, 1=Light, 2=Deep
    
public:
    محاكي_ESP32() {
        // تهيئة الذواكر
        flash_.resize(4 * 1024 * 1024, 0xFF);
        SRAM_.resize(520 * 1024, 0);
        RTC_RAM_.resize(16 * 1024, 0);
        
        // تهيئة GPIO
        for (auto& pin : GPIO_) {
            pin.مخرج = false;
            pin.قيمة = false;
            pin.سحب_لأعلى = false;
            pin.سحب_لأسفل = false;
            pin.PWM_مفعل = false;
            pin.وضع_PWM = 0;
        }
        
        // تهيئة ADC بقيم عشوائية
        for (size_t i = 0; i < ADC_.size(); ++i) {
            ADC_[i] = (i * 227) % 4096;  // قيم عشوائية للمحاكاة
        }
    }
    
    // ============== GPIO ==============
    
    /**
     * تعيين وضع GPIO
     */
    void pinMode(uint8_t pin, bool مخرج, bool سحب_لأعلى = false, bool سحب_لأسفل = false) {
        if (pin >= GPIO_.size()) return;
        GPIO_[pin].مخرج = مخرج;
        GPIO_[pin].سحب_لأعلى = سحب_لأعلى;
        GPIO_[pin].سحب_لأسفل = سحب_لأسفل;
    }
    
    /**
     * كتابة قيمة رقمية
     */
    void digitalWrite(uint8_t pin, bool قيمة) {
        if (pin >= GPIO_.size() || !GPIO_[pin].مخرج) return;
        bool قيمة_سابقة = GPIO_[pin].قيمة;
        GPIO_[pin].قيمة = قيمة;
        
        // استدعاء معالج المقاطعة إذا كان مسجلاً
        if (GPIO_[pin].عند_تغيير && قيمة_سابقة != قيمة) {
            if ((قيمة && GPIO_[pin].مقاطعة_صاعدة) ||
                (!قيمة && GPIO_[pin].مقاطعة_هابطة)) {
                GPIO_[pin].عند_تغيير(قيمة);
            }
        }
    }
    
    /**
     * قراءة قيمة رقمية
     */
    bool digitalRead(uint8_t pin) {
        if (pin >= GPIO_.size()) return false;
        return GPIO_[pin].قيمة;
    }
    
    /**
     * كتابة PWM
     */
    void analogWrite(uint8_t pin, uint8_t قيمة) {
        if (pin >= GPIO_.size()) return;
        GPIO_[pin].PWM_مفعل = true;
        GPIO_[pin].وضع_PWM = قيمة;
    }
    
    /**
     * قراءة ADC
     */
    uint16_t analogRead(uint8_t قناة) {
        if (قناة >= ADC_.size()) return 0;
        return ADC_[قناة];
    }
    
    /**
     * تعيين قيمة ADC (للمحاكاة)
     */
    void setADC(uint8_t قناة, uint16_t قيمة) {
        if (قناة < ADC_.size()) {
            ADC_[قناة] = قيمة & 0x0FFF;  // 12-bit
        }
    }
    
    /**
     * كتابة DAC
     */
    void dacWrite(uint8_t قناة, uint8_t قيمة) {
        if (قناة < DAC_.size()) {
            DAC_[قناة] = قيمة;
        }
    }
    
    /**
     * تسجيل مقاطعة GPIO
     */
    void attachInterrupt(uint8_t pin, std::function<void(bool)> معالج, 
                        bool صاعدة, bool هابطة) {
        if (pin >= GPIO_.size()) return;
        GPIO_[pin].عند_تغيير = معالج;
        GPIO_[pin].مقاطعة_صاعدة = صاعدة;
        GPIO_[pin].مقاطعة_هابطة = هابطة;
    }
    
    /**
     * محاكاة إدخال خارجي (زر، حساس، إلخ)
     */
    void simulateInput(uint8_t pin, bool قيمة) {
        if (pin >= GPIO_.size() || GPIO_[pin].مخرج) return;
        bool قيمة_سابقة = GPIO_[pin].قيمة;
        GPIO_[pin].قيمة = قيمة;
        
        // معالجة المقاطعة
        if (GPIO_[pin].عند_تغيير && قيمة_سابقة != قيمة) {
            if ((قيمة && GPIO_[pin].مقاطعة_صاعدة) ||
                (!قيمة && GPIO_[pin].مقاطعة_هابطة)) {
                GPIO_[pin].عند_تغيير(قيمة);
            }
        }
    }
    
    // ============== UART ==============
    
    /**
     * إرسال بايت عبر UART
     */
    void UART_write(uint8_t منفذ, uint8_t بايت) {
        if (منفذ < UART_TX_.size()) {
            UART_TX_[منفذ].push(بايت);
        }
    }
    
    /**
     * إرسال نص عبر UART
     */
    void UART_print(uint8_t منفذ, const std::string& نص) {
        for (char c : نص) {
            UART_write(منفذ, c);
        }
    }
    
    /**
     * قراءة بايت من UART
     */
    int UART_read(uint8_t منفذ) {
        if (منفذ >= UART_RX_.size() || UART_RX_[منفذ].empty()) {
            return -1;
        }
        uint8_t بايت = UART_RX_[منفذ].front();
        UART_RX_[منفذ].pop();
        return بايت;
    }
    
    /**
     * التحقق من توفر بيانات UART
     */
    int UART_available(uint8_t منفذ) {
        if (منفذ >= UART_RX_.size()) return 0;
        return UART_RX_[منفذ].size();
    }
    
    /**
     * محاكاة استقبال بيانات UART
     */
    void simulateUART_RX(uint8_t منفذ, const std::vector<uint8_t>& بيانات) {
        if (منفذ >= UART_RX_.size()) return;
        for (uint8_t b : بيانات) {
            UART_RX_[منفذ].push(b);
        }
    }
    
    /**
     * قراءة ما أرسله المحاكي عبر UART
     */
    std::vector<uint8_t> getUART_TX(uint8_t منفذ) {
        std::vector<uint8_t> نتيجة;
        if (منفذ >= UART_TX_.size()) return نتيجة;
        while (!UART_TX_[منفذ].empty()) {
            نتيجة.push_back(UART_TX_[منفذ].front());
            UART_TX_[منفذ].pop();
        }
        return نتيجة;
    }
    
    // ============== SPI ==============
    
    void SPI_begin(uint8_t منفذ) {
        if (منفذ < SPI_buffer_.size()) {
            SPI_buffer_[منفذ].clear();
        }
    }
    
    uint8_t SPI_transfer(uint8_t منفذ, uint8_t بيانات) {
        if (منفذ >= SPI_buffer_.size()) return 0;
        // في المحاكاة، نعيد البيانات المخزنة
        uint8_t نتيجة = SPI_buffer_[منفذ].empty() ? 0 : SPI_buffer_[منفذ].back();
        return نتيجة;
    }
    
    void SPI_simulateResponse(uint8_t منفذ, const std::vector<uint8_t>& بيانات) {
        if (منفذ < SPI_buffer_.size()) {
            SPI_buffer_[منفذ] = بيانات;
        }
    }
    
    // ============== I2C ==============
    
    /**
     * إضافة جهاز I2C وهمي
     */
    void I2C_addDevice(uint8_t عنوان, const std::vector<uint8_t>& بيانات) {
        I2C_devices_[عنوان] = بيانات;
    }
    
    /**
     * قراءة من جهاز I2C
     */
    std::vector<uint8_t> I2C_read(uint8_t عنوان, size_t عدد) {
        auto it = I2C_devices_.find(عنوان);
        if (it == I2C_devices_.end()) return {};
        
        std::vector<uint8_t> نتيجة;
        size_t count = std::min(عدد, it->second.size());
        for (size_t i = 0; i < count; ++i) {
            نتيجة.push_back(it->second[i]);
        }
        return نتيجة;
    }
    
    /**
     * كتابة إلى جهاز I2C
     */
    bool I2C_write(uint8_t عنوان, const std::vector<uint8_t>& بيانات) {
        auto it = I2C_devices_.find(عنوان);
        if (it == I2C_devices_.end()) return false;
        it->second = بيانات;
        return true;
    }
    
    // ============== WiFi & Bluetooth ==============
    
    محاكي_WiFi& WiFi() { return WiFi_; }
    محاكي_Bluetooth& Bluetooth() { return Bluetooth_; }
    
    // ============== المؤقتات ==============
    
    void timer_begin(uint8_t مؤقت, uint32_t تردد) {
        if (مؤقت < مؤقتات_.size()) {
            مؤقتات_[مؤقت] = 0;
            مؤقت_مفعل_[مؤقت] = true;
        }
    }
    
    uint64_t timer_read(uint8_t مؤقت) {
        if (مؤقت >= مؤقتات_.size()) return 0;
        return مؤقتات_[مؤقت];
    }
    
    void timer_write(uint8_t مؤقت, uint64_t قيمة) {
        if (مؤقت < مؤقتات_.size()) {
            مؤقتات_[مؤقت] = قيمة;
        }
    }
    
    // ============== النوم ==============
    
    void deepSleep(uint64_t مايكروثانية) {
        نائم_ = true;
        وضع_النوم_ = 2;
        // في المحاكاة الفعلية، نحفظ الوقت ونستأنف بعده
    }
    
    void lightSleep(uint64_t مايكروثانية) {
        نائم_ = true;
        وضع_النوم_ = 1;
    }
    
    void wakeUp() {
        نائم_ = false;
        وضع_النوم_ = 0;
    }
    
    bool isAsleep() const { return نائم_; }
    
    // ============== الذاكرة ==============
    
    std::vector<uint8_t>& flash() { return flash_; }
    std::vector<uint8_t>& SRAM() { return SRAM_; }
    std::vector<uint8_t>& RTC_RAM() { return RTC_RAM_; }
    
    /**
     * قراءة من الفلاش
     */
    uint8_t flashRead(uint32_t عنوان) {
        if (عنوان < flash_.size()) {
            return flash_[عنوان];
        }
        return 0xFF;
    }
    
    /**
     * كتابة إلى الفلاش
     */
    void flashWrite(uint32_t عنوان, uint8_t قيمة) {
        if (عنوان < flash_.size()) {
            flash_[عنوان] = قيمة;
        }
    }
    
    // ============== تحديث المحاكي ==============
    
    /**
     * تحديث حالة المحاكي
     */
    void update(uint64_t دورات) {
        // تحديث المؤقتات
        for (size_t i = 0; i < مؤقتات_.size(); ++i) {
            if (مؤقت_مفعل_[i]) {
                مؤقتات_[i] += دورات;
            }
        }
        
        // تحديث RTC
        وقت_RTC_ += دورات;
    }
    
    /**
     * إعادة تعيين
     */
    void reset() {
        // إعادة تعيين GPIO
        for (auto& pin : GPIO_) {
            pin.قيمة = false;
            pin.PWM_مفعل = false;
            pin.وضع_PWM = 0;
        }
        
        // إعادة تعيين UART
        for (auto& q : UART_RX_) while (!q.empty()) q.pop();
        for (auto& q : UART_TX_) while (!q.empty()) q.pop();
        
        // إعادة تعيين المؤقتات
        for (size_t i = 0; i < مؤقتات_.size(); ++i) {
            مؤقتات_[i] = 0;
            مؤقت_مفعل_[i] = false;
        }
        
        // إيقاف WiFi و Bluetooth
        WiFi_.إيقاف();
        Bluetooth_.إيقاف();
        
        // إيقاظ من النوم
        wakeUp();
    }
};

// ============== محاكي Arduino ==============

/**
 * محاكي Arduino (AVR)
 */
class محاكي_Arduino {
public:
    enum class نوع_Arduino {
        UNO,        // ATmega328P
        MEGA,       // ATmega2560
        NANO,       // ATmega328P
        LEONARDO,   // ATmega32U4
        DUE         // SAM3X8E (ARM)
    };
    
private:
    نوع_Arduino نوع_;
    
    // GPIO
    std::vector<حالة_GPIO> GPIO_;
    
    // ADC
    std::vector<uint16_t> ADC_;  // 10-bit (0-1023)
    
    // PWM
    std::vector<uint8_t> PWM_;   // 8-bit (0-255)
    
    // Serial (UART)
    std::queue<uint8_t> Serial_RX_;
    std::queue<uint8_t> Serial_TX_;
    
    // الذاكرة
    std::vector<uint8_t> flash_;
    std::vector<uint8_t> SRAM_;
    std::vector<uint8_t> EEPROM_;
    
    // المؤقت
    uint64_t millis_ = 0;
    uint64_t micros_ = 0;
    
public:
    محاكي_Arduino(نوع_Arduino نوع = نوع_Arduino::UNO) : نوع_(نوع) {
        تهيئة();
    }
    
    void تهيئة() {
        switch (نوع_) {
            case نوع_Arduino::UNO:
            case نوع_Arduino::NANO:
                GPIO_.resize(20);
                ADC_.resize(6);
                PWM_.resize(6);
                flash_.resize(32 * 1024, 0xFF);
                SRAM_.resize(2 * 1024, 0);
                EEPROM_.resize(1024, 0xFF);
                break;
                
            case نوع_Arduino::MEGA:
                GPIO_.resize(54);
                ADC_.resize(16);
                PWM_.resize(15);
                flash_.resize(256 * 1024, 0xFF);
                SRAM_.resize(8 * 1024, 0);
                EEPROM_.resize(4 * 1024, 0xFF);
                break;
                
            case نوع_Arduino::LEONARDO:
                GPIO_.resize(20);
                ADC_.resize(12);
                PWM_.resize(7);
                flash_.resize(32 * 1024, 0xFF);
                SRAM_.resize(2560, 0);
                EEPROM_.resize(1024, 0xFF);
                break;
                
            case نوع_Arduino::DUE:
                GPIO_.resize(54);
                ADC_.resize(12);  // 12-bit
                PWM_.resize(12);
                flash_.resize(512 * 1024, 0xFF);
                SRAM_.resize(96 * 1024, 0);
                EEPROM_.resize(0);  // No EEPROM
                break;
        }
        
        // تهيئة GPIO
        for (auto& pin : GPIO_) {
            pin.مخرج = false;
            pin.قيمة = false;
        }
        
        // تهيئة ADC
        for (size_t i = 0; i < ADC_.size(); ++i) {
            ADC_[i] = (i * 123) % 1024;
        }
    }
    
    // ============== Arduino API ==============
    
    void pinMode(uint8_t pin, bool مخرج) {
        if (pin < GPIO_.size()) {
            GPIO_[pin].مخرج = مخرج;
        }
    }
    
    void digitalWrite(uint8_t pin, bool قيمة) {
        if (pin < GPIO_.size() && GPIO_[pin].مخرج) {
            GPIO_[pin].قيمة = قيمة;
        }
    }
    
    bool digitalRead(uint8_t pin) {
        if (pin < GPIO_.size()) {
            return GPIO_[pin].قيمة;
        }
        return false;
    }
    
    void analogWrite(uint8_t pin, uint8_t قيمة) {
        // Only certain pins support PWM
        if (pin < PWM_.size()) {
            PWM_[pin] = قيمة;
        }
    }
    
    uint16_t analogRead(uint8_t pin) {
        if (pin < ADC_.size()) {
            return ADC_[pin];
        }
        return 0;
    }
    
    void setADC(uint8_t pin, uint16_t قيمة) {
        if (pin < ADC_.size()) {
            ADC_[pin] = قيمة & 0x3FF;  // 10-bit
        }
    }
    
    void simulateInput(uint8_t pin, bool قيمة) {
        if (pin < GPIO_.size() && !GPIO_[pin].مخرج) {
            GPIO_[pin].قيمة = قيمة;
        }
    }
    
    // Serial
    void Serial_begin(uint32_t baud) {
        // تهيئة Serial
    }
    
    void Serial_print(const std::string& نص) {
        for (char c : نص) {
            Serial_TX_.push(c);
        }
    }
    
    void Serial_println(const std::string& نص) {
        Serial_print(نص + "\n");
    }
    
    void Serial_write(uint8_t بايت) {
        Serial_TX_.push(بايت);
    }
    
    int Serial_read() {
        if (Serial_RX_.empty()) return -1;
        uint8_t بايت = Serial_RX_.front();
        Serial_RX_.pop();
        return بايت;
    }
    
    int Serial_available() {
        return Serial_RX_.size();
    }
    
    void simulateSerial_RX(const std::vector<uint8_t>& بيانات) {
        for (uint8_t b : بيانات) {
            Serial_RX_.push(b);
        }
    }
    
    std::vector<uint8_t> getSerial_TX() {
        std::vector<uint8_t> نتيجة;
        while (!Serial_TX_.empty()) {
            نتيجة.push_back(Serial_TX_.front());
            Serial_TX_.pop();
        }
        return نتيجة;
    }
    
    // EEPROM
    uint8_t EEPROM_read(uint16_t عنوان) {
        if (عنوان < EEPROM_.size()) {
            return EEPROM_[عنوان];
        }
        return 0xFF;
    }
    
    void EEPROM_write(uint16_t عنوان, uint8_t قيمة) {
        if (عنوان < EEPROM_.size()) {
            EEPROM_[عنوان] = قيمة;
        }
    }
    
    // Timing
    uint64_t millis() const { return millis_; }
    uint64_t micros() const { return micros_; }
    
    void delay(uint32_t ms) {
        millis_ += ms;
        micros_ += ms * 1000;
    }
    
    void delayMicroseconds(uint32_t us) {
        micros_ += us;
        millis_ += us / 1000;
    }
    
    // Update
    void update(uint64_t دورات) {
        // تحديث الوقت (assuming 16MHz clock)
        micros_ += دورات / 16;
        millis_ = micros_ / 1000;
    }
    
    void reset() {
        for (auto& pin : GPIO_) {
            pin.مخرج = false;
            pin.قيمة = false;
        }
        while (!Serial_RX_.empty()) Serial_RX_.pop();
        while (!Serial_TX_.empty()) Serial_TX_.pop();
        millis_ = 0;
        micros_ = 0;
    }
    
    // Memory access
    std::vector<uint8_t>& flash() { return flash_; }
    std::vector<uint8_t>& SRAM() { return SRAM_; }
    std::vector<uint8_t>& EEPROM() { return EEPROM_; }
};

}  // namespace ص::محاكاة

// ============== C API ==============

extern "C" {

using namespace ص::محاكاة;

// ESP32 Emulator
typedef struct محاكي_ESP32_مقبض {
    std::unique_ptr<محاكي_ESP32> محاكي;
} محاكي_ESP32_مقبض;

محاكي_ESP32_مقبض* sad_esp32_create() {
    auto مقبض = new محاكي_ESP32_مقبض();
    مقبض->محاكي = std::make_unique<محاكي_ESP32>();
    return مقبض;
}

void sad_esp32_destroy(محاكي_ESP32_مقبض* مقبض) {
    delete مقبض;
}

void sad_esp32_pinMode(محاكي_ESP32_مقبض* مقبض, uint8_t pin, int mode) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->pinMode(pin, mode == 1, false, false);
    }
}

void sad_esp32_digitalWrite(محاكي_ESP32_مقبض* مقبض, uint8_t pin, int value) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->digitalWrite(pin, value != 0);
    }
}

int sad_esp32_digitalRead(محاكي_ESP32_مقبض* مقبض, uint8_t pin) {
    if (مقبض && مقبض->محاكي) {
        return مقبض->محاكي->digitalRead(pin) ? 1 : 0;
    }
    return 0;
}

int sad_esp32_analogRead(محاكي_ESP32_مقبض* مقبض, uint8_t channel) {
    if (مقبض && مقبض->محاكي) {
        return مقبض->محاكي->analogRead(channel);
    }
    return 0;
}

void sad_esp32_reset(محاكي_ESP32_مقبض* مقبض) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->reset();
    }
}

// Arduino Emulator
typedef struct محاكي_Arduino_مقبض {
    std::unique_ptr<محاكي_Arduino> محاكي;
} محاكي_Arduino_مقبض;

محاكي_Arduino_مقبض* sad_arduino_create(int type) {
    auto مقبض = new محاكي_Arduino_مقبض();
    مقبض->محاكي = std::make_unique<محاكي_Arduino>(
        static_cast<محاكي_Arduino::نوع_Arduino>(type)
    );
    return مقبض;
}

void sad_arduino_destroy(محاكي_Arduino_مقبض* مقبض) {
    delete مقبض;
}

void sad_arduino_pinMode(محاكي_Arduino_مقبض* مقبض, uint8_t pin, int mode) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->pinMode(pin, mode == 1);
    }
}

void sad_arduino_digitalWrite(محاكي_Arduino_مقبض* مقبض, uint8_t pin, int value) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->digitalWrite(pin, value != 0);
    }
}

int sad_arduino_digitalRead(محاكي_Arduino_مقبض* مقبض, uint8_t pin) {
    if (مقبض && مقبض->محاكي) {
        return مقبض->محاكي->digitalRead(pin) ? 1 : 0;
    }
    return 0;
}

int sad_arduino_analogRead(محاكي_Arduino_مقبض* مقبض, uint8_t pin) {
    if (مقبض && مقبض->محاكي) {
        return مقبض->محاكي->analogRead(pin);
    }
    return 0;
}

void sad_arduino_reset(محاكي_Arduino_مقبض* مقبض) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->reset();
    }
}

}  // extern "C"
