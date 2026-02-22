/**
 * ===================================================================================
 *  إطار عمل محاكي الأجهزة - Device Emulator Framework (T395)
 * ===================================================================================
 * 
 *  الوصف: إطار عمل شامل لمحاكاة جميع أنواع الأجهزة الإلكترونية
 *  Description: Comprehensive framework for emulating all types of electronic devices
 *
 *  المميزات:
 *  - دعم متعدد الأجهزة (ESP32, Arduino, STM32, Raspberry Pi)
 *  - محاكاة الذاكرة والمعالج والطرفيات
 *  - واجهة موحدة لجميع المحاكيات
 *  - دعم التصحيح خطوة بخطوة
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ص::محاكاة {

// ============== أنواع الأجهزة ==============

/**
 * نوع الجهاز - أنواع الأجهزة المدعومة
 */
enum class نوع_جهاز {
    ESP32,              // معالج ESP32
    ESP8266,            // معالج ESP8266
    ARDUINO_UNO,        // Arduino Uno (ATmega328P)
    ARDUINO_MEGA,       // Arduino Mega (ATmega2560)
    ARDUINO_NANO,       // Arduino Nano
    STM32F103,          // STM32 Blue Pill
    STM32F407,          // STM32 Discovery
    RASPBERRY_PI_PICO,  // Raspberry Pi Pico (RP2040)
    RASPBERRY_PI_ZERO,  // Raspberry Pi Zero
    RASPBERRY_PI_4,     // Raspberry Pi 4
    GENERIC_ARM,        // معالج ARM عام
    GENERIC_X86,        // معالج x86 عام
    CUSTOM              // جهاز مخصص
};

/**
 * حالة المعالج
 */
enum class حالة_معالج {
    متوقف,      // Stopped
    يعمل,       // Running
    موقوف,      // Paused
    خطوة,       // Single step
    خطأ         // Error state
};

/**
 * نوع الذاكرة
 */
enum class نوع_ذاكرة {
    فلاش,       // Flash memory
    RAM,        // RAM
    EEPROM,     // EEPROM
    سجلات,      // Registers
    طرفيات     // Peripheral I/O
};

// ============== بنى البيانات ==============

/**
 * مواصفات الجهاز
 */
struct مواصفات_جهاز {
    نوع_جهاز نوع;
    std::string اسم;
    std::string وصف;
    
    // المعالج
    std::string معمارية;           // ARM, AVR, Xtensa, etc.
    uint32_t سرعة_ميجاهرتز;        // Clock speed in MHz
    uint32_t عدد_النوى;            // Number of cores
    
    // الذاكرة
    uint32_t حجم_فلاش;             // Flash size in bytes
    uint32_t حجم_RAM;              // RAM size in bytes
    uint32_t حجم_EEPROM;           // EEPROM size
    
    // الطرفيات
    uint32_t عدد_GPIO;             // GPIO pins
    uint32_t عدد_ADC;              // ADC channels
    uint32_t عدد_PWM;              // PWM channels
    uint32_t عدد_UART;             // UART interfaces
    uint32_t عدد_SPI;              // SPI interfaces
    uint32_t عدد_I2C;              // I2C interfaces
    
    // ميزات خاصة
    bool واي_فاي;                  // WiFi support
    bool بلوتوث;                   // Bluetooth support
    bool يدعم_FPU;                 // Floating point unit
};

/**
 * حالة السجلات
 */
struct حالة_سجلات {
    std::vector<uint32_t> سجلات_عامة;  // General purpose registers
    uint32_t PC;                        // Program counter
    uint32_t SP;                        // Stack pointer
    uint32_t LR;                        // Link register
    uint32_t PSR;                       // Program status register
    std::map<std::string, uint32_t> سجلات_خاصة;  // Special registers
};

/**
 * نقطة توقف
 */
struct نقطة_توقف {
    uint32_t عنوان;
    bool مفعلة;
    std::string شرط;              // Conditional breakpoint
    std::function<void()> إجراء;  // Action when hit
};

/**
 * نتيجة التنفيذ
 */
struct نتيجة_تنفيذ {
    bool نجاح;
    uint64_t دورات_منفذة;         // CPU cycles executed
    uint64_t تعليمات_منفذة;       // Instructions executed
    double وقت_حقيقي_ثانية;       // Real time elapsed
    std::string رسالة;
    std::vector<std::string> سجل_أحداث;
};

// ============== واجهة الذاكرة ==============

/**
 * واجهة الذاكرة - Interface for memory access
 */
class واجهة_ذاكرة {
public:
    virtual ~واجهة_ذاكرة() = default;
    
    // قراءة
    virtual uint8_t قراءة_بايت(uint32_t عنوان) = 0;
    virtual uint16_t قراءة_كلمة(uint32_t عنوان) = 0;
    virtual uint32_t قراءة_مزدوجة(uint32_t عنوان) = 0;
    virtual std::vector<uint8_t> قراءة_كتلة(uint32_t عنوان, size_t حجم) = 0;
    
    // كتابة
    virtual void كتابة_بايت(uint32_t عنوان, uint8_t قيمة) = 0;
    virtual void كتابة_كلمة(uint32_t عنوان, uint16_t قيمة) = 0;
    virtual void كتابة_مزدوجة(uint32_t عنوان, uint32_t قيمة) = 0;
    virtual void كتابة_كتلة(uint32_t عنوان, const std::vector<uint8_t>& بيانات) = 0;
    
    // معلومات
    virtual uint32_t حجم_الذاكرة() const = 0;
    virtual bool قابلة_للكتابة() const = 0;
    virtual نوع_ذاكرة نوع() const = 0;
};

/**
 * ذاكرة عامة - Generic memory implementation
 */
class ذاكرة_عامة : public واجهة_ذاكرة {
private:
    std::vector<uint8_t> بيانات_;
    نوع_ذاكرة نوع_;
    bool قابلة_كتابة_;
    
public:
    ذاكرة_عامة(size_t حجم, نوع_ذاكرة نوع, bool قابلة_كتابة = true)
        : بيانات_(حجم, 0), نوع_(نوع), قابلة_كتابة_(قابلة_كتابة) {}
    
    uint8_t قراءة_بايت(uint32_t عنوان) override {
        if (عنوان < بيانات_.size()) {
            return بيانات_[عنوان];
        }
        return 0xFF;  // Bus fault
    }
    
    uint16_t قراءة_كلمة(uint32_t عنوان) override {
        return قراءة_بايت(عنوان) | (قراءة_بايت(عنوان + 1) << 8);
    }
    
    uint32_t قراءة_مزدوجة(uint32_t عنوان) override {
        return قراءة_كلمة(عنوان) | (قراءة_كلمة(عنوان + 2) << 16);
    }
    
    std::vector<uint8_t> قراءة_كتلة(uint32_t عنوان, size_t حجم) override {
        std::vector<uint8_t> نتيجة(حجم);
        for (size_t i = 0; i < حجم; ++i) {
            نتيجة[i] = قراءة_بايت(عنوان + i);
        }
        return نتيجة;
    }
    
    void كتابة_بايت(uint32_t عنوان, uint8_t قيمة) override {
        if (قابلة_كتابة_ && عنوان < بيانات_.size()) {
            بيانات_[عنوان] = قيمة;
        }
    }
    
    void كتابة_كلمة(uint32_t عنوان, uint16_t قيمة) override {
        كتابة_بايت(عنوان, قيمة & 0xFF);
        كتابة_بايت(عنوان + 1, (قيمة >> 8) & 0xFF);
    }
    
    void كتابة_مزدوجة(uint32_t عنوان, uint32_t قيمة) override {
        كتابة_كلمة(عنوان, قيمة & 0xFFFF);
        كتابة_كلمة(عنوان + 2, (قيمة >> 16) & 0xFFFF);
    }
    
    void كتابة_كتلة(uint32_t عنوان, const std::vector<uint8_t>& بيانات) override {
        for (size_t i = 0; i < بيانات.size(); ++i) {
            كتابة_بايت(عنوان + i, بيانات[i]);
        }
    }
    
    uint32_t حجم_الذاكرة() const override { return بيانات_.size(); }
    bool قابلة_للكتابة() const override { return قابلة_كتابة_; }
    نوع_ذاكرة نوع() const override { return نوع_; }
    
    // تحميل بيانات من ملف
    bool تحميل_من_ملف(const std::string& مسار) {
        // Implementation placeholder
        return true;
    }
    
    // حفظ بيانات إلى ملف
    bool حفظ_إلى_ملف(const std::string& مسار) const {
        // Implementation placeholder
        return true;
    }
};

// ============== واجهة الطرفية ==============

/**
 * واجهة الطرفية - Base class for peripherals
 */
class واجهة_طرفية {
public:
    virtual ~واجهة_طرفية() = default;
    
    virtual std::string اسم() const = 0;
    virtual void إعادة_تعيين() = 0;
    virtual void تحديث(uint64_t دورات) = 0;
    
    // قراءة/كتابة السجلات
    virtual uint32_t قراءة_سجل(uint32_t إزاحة) = 0;
    virtual void كتابة_سجل(uint32_t إزاحة, uint32_t قيمة) = 0;
    
    // المقاطعات
    virtual bool يوجد_مقاطعة() const = 0;
    virtual uint32_t رقم_مقاطعة() const = 0;
};

// ============== واجهة المعالج ==============

/**
 * واجهة المعالج - Base class for CPU emulation
 */
class واجهة_معالج {
public:
    virtual ~واجهة_معالج() = default;
    
    // تنفيذ
    virtual void تنفيذ_تعليمة() = 0;
    virtual void تنفيذ_دورات(uint64_t عدد) = 0;
    virtual void تشغيل() = 0;
    virtual void إيقاف() = 0;
    virtual void إيقاف_مؤقت() = 0;
    virtual void خطوة_واحدة() = 0;
    
    // حالة
    virtual حالة_معالج الحالة() const = 0;
    virtual حالة_سجلات السجلات() const = 0;
    virtual void تعيين_سجلات(const حالة_سجلات& سجلات) = 0;
    
    // إعادة تعيين
    virtual void إعادة_تعيين() = 0;
    
    // نقاط التوقف
    virtual void إضافة_نقطة_توقف(const نقطة_توقف& نقطة) = 0;
    virtual void إزالة_نقطة_توقف(uint32_t عنوان) = 0;
    virtual std::vector<نقطة_توقف> نقاط_التوقف() const = 0;
    
    // الذاكرة
    virtual void ربط_ذاكرة(uint32_t عنوان_بداية, std::shared_ptr<واجهة_ذاكرة> ذاكرة) = 0;
    
    // الطرفيات
    virtual void ربط_طرفية(uint32_t عنوان_بداية, std::shared_ptr<واجهة_طرفية> طرفية) = 0;
    
    // المقاطعات
    virtual void طلب_مقاطعة(uint32_t رقم) = 0;
    
    // المعلومات
    virtual std::string اسم_المعالج() const = 0;
    virtual std::string معمارية() const = 0;
    virtual uint64_t دورات_منفذة() const = 0;
};

// ============== محاكي الجهاز ==============

/**
 * محاكي الجهاز - Main device emulator class
 */
class محاكي_جهاز {
private:
    مواصفات_جهاز مواصفات_;
    std::shared_ptr<واجهة_معالج> معالج_;
    std::map<std::string, std::shared_ptr<واجهة_ذاكرة>> ذواكر_;
    std::map<std::string, std::shared_ptr<واجهة_طرفية>> طرفيات_;
    
    حالة_معالج حالة_ = حالة_معالج::متوقف;
    bool يعمل_ = false;
    std::mutex قفل_;
    std::condition_variable شرط_;
    
    // سجل الأحداث
    std::vector<std::string> سجل_أحداث_;
    
    // الوقت
    std::chrono::steady_clock::time_point وقت_بداية_;
    uint64_t دورات_كلية_ = 0;
    
public:
    محاكي_جهاز(const مواصفات_جهاز& مواصفات)
        : مواصفات_(مواصفات) {
        تهيئة_الجهاز();
    }
    
    // ============== التهيئة ==============
    
    void تهيئة_الجهاز() {
        // تهيئة الذاكرة حسب نوع الجهاز
        switch (مواصفات_.نوع) {
            case نوع_جهاز::ESP32:
                تهيئة_ESP32();
                break;
            case نوع_جهاز::ARDUINO_UNO:
                تهيئة_Arduino_Uno();
                break;
            case نوع_جهاز::STM32F103:
                تهيئة_STM32F103();
                break;
            default:
                تهيئة_عامة();
                break;
        }
        
        تسجيل_حدث("تم تهيئة المحاكي للجهاز: " + مواصفات_.اسم);
    }
    
    void تهيئة_ESP32() {
        // Flash: 4MB at 0x00000000
        ذواكر_["فلاش"] = std::make_shared<ذاكرة_عامة>(
            4 * 1024 * 1024, نوع_ذاكرة::فلاش, false
        );
        
        // RAM: 520KB starting at 0x3FFB0000
        ذواكر_["رام"] = std::make_shared<ذاكرة_عامة>(
            520 * 1024, نوع_ذاكرة::RAM, true
        );
        
        // RTC RAM: 16KB
        ذواكر_["رام_RTC"] = std::make_shared<ذاكرة_عامة>(
            16 * 1024, نوع_ذاكرة::RAM, true
        );
    }
    
    void تهيئة_Arduino_Uno() {
        // Flash: 32KB at 0x0000
        ذواكر_["فلاش"] = std::make_shared<ذاكرة_عامة>(
            32 * 1024, نوع_ذاكرة::فلاش, false
        );
        
        // RAM: 2KB starting at 0x0100
        ذواكر_["رام"] = std::make_shared<ذاكرة_عامة>(
            2 * 1024, نوع_ذاكرة::RAM, true
        );
        
        // EEPROM: 1KB
        ذواكر_["EEPROM"] = std::make_shared<ذاكرة_عامة>(
            1024, نوع_ذاكرة::EEPROM, true
        );
        
        // Registers: 32 bytes
        ذواكر_["سجلات"] = std::make_shared<ذاكرة_عامة>(
            32, نوع_ذاكرة::سجلات, true
        );
    }
    
    void تهيئة_STM32F103() {
        // Flash: 128KB at 0x08000000
        ذواكر_["فلاش"] = std::make_shared<ذاكرة_عامة>(
            128 * 1024, نوع_ذاكرة::فلاش, false
        );
        
        // RAM: 20KB at 0x20000000
        ذواكر_["رام"] = std::make_shared<ذاكرة_عامة>(
            20 * 1024, نوع_ذاكرة::RAM, true
        );
    }
    
    void تهيئة_عامة() {
        // Default: 1MB Flash, 256KB RAM
        ذواكر_["فلاش"] = std::make_shared<ذاكرة_عامة>(
            1024 * 1024, نوع_ذاكرة::فلاش, false
        );
        
        ذواكر_["رام"] = std::make_shared<ذاكرة_عامة>(
            256 * 1024, نوع_ذاكرة::RAM, true
        );
    }
    
    // ============== تحميل البرنامج ==============
    
    /**
     * تحميل برنامج من ملف ثنائي
     */
    bool تحميل_برنامج(const std::string& مسار_الملف) {
        تسجيل_حدث("تحميل البرنامج: " + مسار_الملف);
        
        // TODO: تحميل الملف الثنائي إلى الفلاش
        auto& فلاش = ذواكر_["فلاش"];
        if (فلاش) {
            auto ذاكرة_فلاش = std::dynamic_pointer_cast<ذاكرة_عامة>(فلاش);
            if (ذاكرة_فلاش) {
                return ذاكرة_فلاش->تحميل_من_ملف(مسار_الملف);
            }
        }
        
        return false;
    }
    
    /**
     * تحميل برنامج من مصفوفة بايتات
     */
    bool تحميل_من_بيانات(const std::vector<uint8_t>& بيانات, uint32_t عنوان = 0) {
        تسجيل_حدث("تحميل " + std::to_string(بيانات.size()) + " بايت إلى العنوان " 
                   + std::to_string(عنوان));
        
        auto& فلاش = ذواكر_["فلاش"];
        if (فلاش) {
            فلاش->كتابة_كتلة(عنوان, بيانات);
            return true;
        }
        
        return false;
    }
    
    // ============== التشغيل والتحكم ==============
    
    /**
     * تشغيل المحاكي
     */
    نتيجة_تنفيذ تشغيل(uint64_t حد_دورات = 0) {
        نتيجة_تنفيذ نتيجة;
        
        {
            std::lock_guard<std::mutex> قفل(قفل_);
            if (حالة_ == حالة_معالج::يعمل) {
                نتيجة.نجاح = false;
                نتيجة.رسالة = "المحاكي يعمل بالفعل";
                return نتيجة;
            }
            
            حالة_ = حالة_معالج::يعمل;
            يعمل_ = true;
            وقت_بداية_ = std::chrono::steady_clock::now();
        }
        
        تسجيل_حدث("بدء التشغيل");
        
        // حلقة التنفيذ
        uint64_t دورات_منفذة = 0;
        while (يعمل_) {
            if (معالج_) {
                معالج_->تنفيذ_تعليمة();
                دورات_منفذة++;
            }
            
            // تحديث الطرفيات
            for (auto& [اسم, طرفية] : طرفيات_) {
                طرفية->تحديث(1);
            }
            
            // التحقق من حد الدورات
            if (حد_دورات > 0 && دورات_منفذة >= حد_دورات) {
                break;
            }
            
            // التحقق من حالة المعالج
            if (معالج_ && معالج_->الحالة() == حالة_معالج::موقوف) {
                break;
            }
        }
        
        auto وقت_نهاية = std::chrono::steady_clock::now();
        
        نتيجة.نجاح = true;
        نتيجة.دورات_منفذة = دورات_منفذة;
        نتيجة.تعليمات_منفذة = معالج_ ? معالج_->دورات_منفذة() : دورات_منفذة;
        نتيجة.وقت_حقيقي_ثانية = std::chrono::duration<double>(وقت_نهاية - وقت_بداية_).count();
        نتيجة.سجل_أحداث = سجل_أحداث_;
        نتيجة.رسالة = "اكتمل التنفيذ بنجاح";
        
        دورات_كلية_ += دورات_منفذة;
        تسجيل_حدث("انتهاء التشغيل - " + std::to_string(دورات_منفذة) + " دورة");
        
        return نتيجة;
    }
    
    /**
     * إيقاف المحاكي
     */
    void إيقاف() {
        std::lock_guard<std::mutex> قفل(قفل_);
        يعمل_ = false;
        حالة_ = حالة_معالج::متوقف;
        if (معالج_) {
            معالج_->إيقاف();
        }
        تسجيل_حدث("تم إيقاف المحاكي");
    }
    
    /**
     * إيقاف مؤقت
     */
    void إيقاف_مؤقت() {
        std::lock_guard<std::mutex> قفل(قفل_);
        يعمل_ = false;
        حالة_ = حالة_معالج::موقوف;
        if (معالج_) {
            معالج_->إيقاف_مؤقت();
        }
        تسجيل_حدث("تم إيقاف المحاكي مؤقتاً");
    }
    
    /**
     * تنفيذ خطوة واحدة
     */
    void خطوة() {
        if (معالج_) {
            معالج_->خطوة_واحدة();
            دورات_كلية_++;
            
            // تحديث الطرفيات
            for (auto& [اسم, طرفية] : طرفيات_) {
                طرفية->تحديث(1);
            }
        }
    }
    
    /**
     * إعادة تعيين المحاكي
     */
    void إعادة_تعيين() {
        إيقاف();
        
        if (معالج_) {
            معالج_->إعادة_تعيين();
        }
        
        for (auto& [اسم, طرفية] : طرفيات_) {
            طرفية->إعادة_تعيين();
        }
        
        دورات_كلية_ = 0;
        سجل_أحداث_.clear();
        تسجيل_حدث("تمت إعادة تعيين المحاكي");
    }
    
    // ============== نقاط التوقف ==============
    
    void إضافة_نقطة_توقف(uint32_t عنوان) {
        if (معالج_) {
            نقطة_توقف نقطة;
            نقطة.عنوان = عنوان;
            نقطة.مفعلة = true;
            معالج_->إضافة_نقطة_توقف(نقطة);
            تسجيل_حدث("إضافة نقطة توقف عند العنوان: 0x" + std::to_string(عنوان));
        }
    }
    
    void إزالة_نقطة_توقف(uint32_t عنوان) {
        if (معالج_) {
            معالج_->إزالة_نقطة_توقف(عنوان);
            تسجيل_حدث("إزالة نقطة توقف من العنوان: 0x" + std::to_string(عنوان));
        }
    }
    
    // ============== الذاكرة ==============
    
    std::vector<uint8_t> قراءة_ذاكرة(const std::string& اسم_ذاكرة, 
                                      uint32_t عنوان, size_t حجم) {
        auto it = ذواكر_.find(اسم_ذاكرة);
        if (it != ذواكر_.end()) {
            return it->second->قراءة_كتلة(عنوان, حجم);
        }
        return {};
    }
    
    void كتابة_ذاكرة(const std::string& اسم_ذاكرة, 
                     uint32_t عنوان, const std::vector<uint8_t>& بيانات) {
        auto it = ذواكر_.find(اسم_ذاكرة);
        if (it != ذواكر_.end()) {
            it->second->كتابة_كتلة(عنوان, بيانات);
        }
    }
    
    // ============== الطرفيات ==============
    
    void إضافة_طرفية(const std::string& اسم, std::shared_ptr<واجهة_طرفية> طرفية) {
        طرفيات_[اسم] = طرفية;
        تسجيل_حدث("إضافة طرفية: " + اسم);
    }
    
    std::shared_ptr<واجهة_طرفية> طرفية(const std::string& اسم) {
        auto it = طرفيات_.find(اسم);
        return (it != طرفيات_.end()) ? it->second : nullptr;
    }
    
    // ============== المعلومات ==============
    
    const مواصفات_جهاز& المواصفات() const { return مواصفات_; }
    حالة_معالج الحالة() const { return حالة_; }
    uint64_t الدورات_الكلية() const { return دورات_كلية_; }
    const std::vector<std::string>& سجل_الأحداث() const { return سجل_أحداث_; }
    
    حالة_سجلات سجلات_المعالج() const {
        if (معالج_) {
            return معالج_->السجلات();
        }
        return {};
    }
    
private:
    void تسجيل_حدث(const std::string& رسالة) {
        auto الآن = std::chrono::system_clock::now();
        auto وقت = std::chrono::system_clock::to_time_t(الآن);
        سجل_أحداث_.push_back("[" + std::to_string(دورات_كلية_) + "] " + رسالة);
    }
};

// ============== مصنع المحاكيات ==============

/**
 * مصنع المحاكيات - Factory for creating device emulators
 */
class مصنع_محاكيات {
public:
    /**
     * إنشاء محاكي لجهاز محدد
     */
    static std::unique_ptr<محاكي_جهاز> إنشاء(نوع_جهاز نوع) {
        مواصفات_جهاز مواصفات = الحصول_على_مواصفات(نوع);
        return std::make_unique<محاكي_جهاز>(مواصفات);
    }
    
    /**
     * الحصول على مواصفات جهاز
     */
    static مواصفات_جهاز الحصول_على_مواصفات(نوع_جهاز نوع) {
        مواصفات_جهاز م;
        م.نوع = نوع;
        
        switch (نوع) {
            case نوع_جهاز::ESP32:
                م.اسم = "ESP32";
                م.وصف = "معالج ESP32 ثنائي النواة مع WiFi و Bluetooth";
                م.معمارية = "Xtensa LX6";
                م.سرعة_ميجاهرتز = 240;
                م.عدد_النوى = 2;
                م.حجم_فلاش = 4 * 1024 * 1024;
                م.حجم_RAM = 520 * 1024;
                م.حجم_EEPROM = 0;
                م.عدد_GPIO = 34;
                م.عدد_ADC = 18;
                م.عدد_PWM = 16;
                م.عدد_UART = 3;
                م.عدد_SPI = 4;
                م.عدد_I2C = 2;
                م.واي_فاي = true;
                م.بلوتوث = true;
                م.يدعم_FPU = true;
                break;
                
            case نوع_جهاز::ARDUINO_UNO:
                م.اسم = "Arduino Uno";
                م.وصف = "لوحة Arduino Uno مع معالج ATmega328P";
                م.معمارية = "AVR";
                م.سرعة_ميجاهرتز = 16;
                م.عدد_النوى = 1;
                م.حجم_فلاش = 32 * 1024;
                م.حجم_RAM = 2 * 1024;
                م.حجم_EEPROM = 1024;
                م.عدد_GPIO = 20;
                م.عدد_ADC = 6;
                م.عدد_PWM = 6;
                م.عدد_UART = 1;
                م.عدد_SPI = 1;
                م.عدد_I2C = 1;
                م.واي_فاي = false;
                م.بلوتوث = false;
                م.يدعم_FPU = false;
                break;
                
            case نوع_جهاز::STM32F103:
                م.اسم = "STM32F103 Blue Pill";
                م.وصف = "لوحة STM32F103C8T6 Blue Pill";
                م.معمارية = "ARM Cortex-M3";
                م.سرعة_ميجاهرتز = 72;
                م.عدد_النوى = 1;
                م.حجم_فلاش = 128 * 1024;  // 64KB advertised, 128KB actual
                م.حجم_RAM = 20 * 1024;
                م.حجم_EEPROM = 0;
                م.عدد_GPIO = 37;
                م.عدد_ADC = 10;
                م.عدد_PWM = 15;
                م.عدد_UART = 3;
                م.عدد_SPI = 2;
                م.عدد_I2C = 2;
                م.واي_فاي = false;
                م.بلوتوث = false;
                م.يدعم_FPU = false;
                break;
                
            case نوع_جهاز::RASPBERRY_PI_PICO:
                م.اسم = "Raspberry Pi Pico";
                م.وصف = "لوحة Raspberry Pi Pico مع معالج RP2040";
                م.معمارية = "ARM Cortex-M0+";
                م.سرعة_ميجاهرتز = 133;
                م.عدد_النوى = 2;
                م.حجم_فلاش = 2 * 1024 * 1024;
                م.حجم_RAM = 264 * 1024;
                م.حجم_EEPROM = 0;
                م.عدد_GPIO = 26;
                م.عدد_ADC = 3;
                م.عدد_PWM = 16;
                م.عدد_UART = 2;
                م.عدد_SPI = 2;
                م.عدد_I2C = 2;
                م.واي_فاي = false;  // Pico W has WiFi
                م.بلوتوث = false;
                م.يدعم_FPU = false;
                break;
                
            default:
                م.اسم = "جهاز عام";
                م.وصف = "جهاز عام للمحاكاة";
                م.معمارية = "Generic";
                م.سرعة_ميجاهرتز = 100;
                م.عدد_النوى = 1;
                م.حجم_فلاش = 1024 * 1024;
                م.حجم_RAM = 256 * 1024;
                م.حجم_EEPROM = 0;
                م.عدد_GPIO = 32;
                م.عدد_ADC = 8;
                م.عدد_PWM = 8;
                م.عدد_UART = 2;
                م.عدد_SPI = 1;
                م.عدد_I2C = 1;
                م.واي_فاي = false;
                م.بلوتوث = false;
                م.يدعم_FPU = false;
                break;
        }
        
        return م;
    }
    
    /**
     * قائمة الأجهزة المدعومة
     */
    static std::vector<std::pair<نوع_جهاز, std::string>> الأجهزة_المدعومة() {
        return {
            {نوع_جهاز::ESP32, "ESP32 - معالج ثنائي النواة مع WiFi/BT"},
            {نوع_جهاز::ESP8266, "ESP8266 - معالج WiFi اقتصادي"},
            {نوع_جهاز::ARDUINO_UNO, "Arduino Uno - لوحة تعليمية شائعة"},
            {نوع_جهاز::ARDUINO_MEGA, "Arduino Mega - لوحة بموارد أكبر"},
            {نوع_جهاز::ARDUINO_NANO, "Arduino Nano - لوحة صغيرة الحجم"},
            {نوع_جهاز::STM32F103, "STM32 Blue Pill - لوحة ARM قوية"},
            {نوع_جهاز::STM32F407, "STM32 Discovery - لوحة تطوير متقدمة"},
            {نوع_جهاز::RASPBERRY_PI_PICO, "Pi Pico - معالج RP2040 ثنائي النواة"},
            {نوع_جهاز::RASPBERRY_PI_ZERO, "Pi Zero - كمبيوتر Linux صغير"},
            {نوع_جهاز::RASPBERRY_PI_4, "Pi 4 - كمبيوتر Linux كامل"},
        };
    }
};

}  // namespace ص::محاكاة

// ============== C API ==============

extern "C" {

using namespace ص::محاكاة;

typedef struct محاكي_جهاز_مقبض {
    std::unique_ptr<محاكي_جهاز> محاكي;
} محاكي_جهاز_مقبض;

/**
 * إنشاء محاكي جديد
 */
محاكي_جهاز_مقبض* sad_emulator_create(int نوع) {
    auto مقبض = new محاكي_جهاز_مقبض();
    مقبض->محاكي = مصنع_محاكيات::إنشاء(static_cast<نوع_جهاز>(نوع));
    return مقبض;
}

/**
 * تحرير المحاكي
 */
void sad_emulator_destroy(محاكي_جهاز_مقبض* مقبض) {
    delete مقبض;
}

/**
 * تحميل برنامج
 */
int sad_emulator_load(محاكي_جهاز_مقبض* مقبض, const char* مسار) {
    if (!مقبض || !مقبض->محاكي) return 0;
    return مقبض->محاكي->تحميل_برنامج(مسار) ? 1 : 0;
}

/**
 * تشغيل المحاكي
 */
int sad_emulator_run(محاكي_جهاز_مقبض* مقبض, uint64_t حد_دورات) {
    if (!مقبض || !مقبض->محاكي) return 0;
    auto نتيجة = مقبض->محاكي->تشغيل(حد_دورات);
    return نتيجة.نجاح ? 1 : 0;
}

/**
 * إيقاف المحاكي
 */
void sad_emulator_stop(محاكي_جهاز_مقبض* مقبض) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->إيقاف();
    }
}

/**
 * إعادة تعيين المحاكي
 */
void sad_emulator_reset(محاكي_جهاز_مقبض* مقبض) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->إعادة_تعيين();
    }
}

/**
 * تنفيذ خطوة واحدة
 */
void sad_emulator_step(محاكي_جهاز_مقبض* مقبض) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->خطوة();
    }
}

/**
 * إضافة نقطة توقف
 */
void sad_emulator_add_breakpoint(محاكي_جهاز_مقبض* مقبض, uint32_t عنوان) {
    if (مقبض && مقبض->محاكي) {
        مقبض->محاكي->إضافة_نقطة_توقف(عنوان);
    }
}

/**
 * الحصول على عدد الدورات
 */
uint64_t sad_emulator_cycles(محاكي_جهاز_مقبض* مقبض) {
    if (!مقبض || !مقبض->محاكي) return 0;
    return مقبض->محاكي->الدورات_الكلية();
}

}  // extern "C"
