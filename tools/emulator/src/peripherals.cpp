/**
 * ===================================================================================
 *  محاكي الطرفيات المدمجة - Embedded Peripherals Emulator (T398)
 * ===================================================================================
 * 
 *  الوصف: محاكاة كاملة للطرفيات المدمجة (حساسات، محركات، شاشات، إلخ)
 *  Description: Full emulation of embedded peripherals (sensors, motors, displays, etc.)
 *
 *  المميزات:
 *  - حساسات: درجة الحرارة، الرطوبة، الضغط، التسارع، الجيروسكوب
 *  - مخرجات: LED، محركات، سيرفو، صوت
 *  - شاشات: LCD، OLED، TFT
 *  - اتصالات: RFID، NFC، GPS
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>

namespace ص::محاكاة {

// ============== واجهة الطرفية الأساسية ==============

/**
 * واجهة الطرفية - Base interface for all peripherals
 */
class واجهة_طرفية_مدمجة {
public:
    virtual ~واجهة_طرفية_مدمجة() = default;
    
    virtual std::string الاسم() const = 0;
    virtual std::string النوع() const = 0;
    virtual void إعادة_تعيين() = 0;
    virtual void تحديث(double وقت_ثانية) = 0;
    
    // I2C interface
    virtual uint8_t عنوان_I2C() const { return 0; }
    virtual std::vector<uint8_t> قراءة_I2C(uint8_t سجل, size_t عدد) { return {}; }
    virtual bool كتابة_I2C(uint8_t سجل, const std::vector<uint8_t>& بيانات) { return false; }
    
    // SPI interface
    virtual std::vector<uint8_t> نقل_SPI(const std::vector<uint8_t>& بيانات) { return {}; }
    
    // Analog interface
    virtual double قراءة_تناظرية() { return 0.0; }
};

// ============== حساسات درجة الحرارة ==============

/**
 * حساس DHT (DHT11, DHT22)
 */
class حساس_DHT : public واجهة_طرفية_مدمجة {
public:
    enum class نوع_DHT { DHT11, DHT22 };
    
private:
    نوع_DHT نوع_;
    double درجة_الحرارة_ = 25.0;  // °C
    double الرطوبة_ = 50.0;        // %
    
    // نطاقات المحاكاة
    double حد_حرارة_أدنى_;
    double حد_حرارة_أعلى_;
    double تقلب_حرارة_ = 0.5;
    double تقلب_رطوبة_ = 2.0;
    
    std::mt19937 مولد_;
    
public:
    حساس_DHT(نوع_DHT نوع = نوع_DHT::DHT22) : نوع_(نوع) {
        مولد_.seed(std::random_device{}());
        
        if (نوع == نوع_DHT::DHT11) {
            حد_حرارة_أدنى_ = 0;
            حد_حرارة_أعلى_ = 50;
        } else {
            حد_حرارة_أدنى_ = -40;
            حد_حرارة_أعلى_ = 80;
        }
    }
    
    std::string الاسم() const override {
        return نوع_ == نوع_DHT::DHT11 ? "DHT11" : "DHT22";
    }
    
    std::string النوع() const override { return "حساس_درجة_حرارة_ورطوبة"; }
    
    void إعادة_تعيين() override {
        درجة_الحرارة_ = 25.0;
        الرطوبة_ = 50.0;
    }
    
    void تحديث(double وقت_ثانية) override {
        // إضافة تقلبات عشوائية طبيعية
        std::normal_distribution<double> توزيع_حرارة(0, تقلب_حرارة_ * وقت_ثانية);
        std::normal_distribution<double> توزيع_رطوبة(0, تقلب_رطوبة_ * وقت_ثانية);
        
        درجة_الحرارة_ += توزيع_حرارة(مولد_);
        الرطوبة_ += توزيع_رطوبة(مولد_);
        
        // التأكد من البقاء ضمن النطاق
        درجة_الحرارة_ = std::clamp(درجة_الحرارة_, حد_حرارة_أدنى_, حد_حرارة_أعلى_);
        الرطوبة_ = std::clamp(الرطوبة_, 0.0, 100.0);
    }
    
    // ============== API خاص بالحساس ==============
    
    double درجة_الحرارة() const { return درجة_الحرارة_; }
    double الرطوبة() const { return الرطوبة_; }
    
    void تعيين_درجة_الحرارة(double درجة) {
        درجة_الحرارة_ = std::clamp(درجة, حد_حرارة_أدنى_, حد_حرارة_أعلى_);
    }
    
    void تعيين_الرطوبة(double رطوبة) {
        الرطوبة_ = std::clamp(رطوبة, 0.0, 100.0);
    }
    
    // قراءة بروتوكول DHT (40 بت)
    std::vector<uint8_t> قراءة() {
        std::vector<uint8_t> بيانات(5);
        
        if (نوع_ == نوع_DHT::DHT11) {
            بيانات[0] = static_cast<uint8_t>(الرطوبة_);
            بيانات[1] = 0;
            بيانات[2] = static_cast<uint8_t>(درجة_الحرارة_);
            بيانات[3] = 0;
        } else {
            int16_t رطوبة_x10 = static_cast<int16_t>(الرطوبة_ * 10);
            int16_t حرارة_x10 = static_cast<int16_t>(درجة_الحرارة_ * 10);
            
            بيانات[0] = (رطوبة_x10 >> 8) & 0xFF;
            بيانات[1] = رطوبة_x10 & 0xFF;
            بيانات[2] = (حرارة_x10 >> 8) & 0xFF;
            بيانات[3] = حرارة_x10 & 0xFF;
        }
        
        // Checksum
        بيانات[4] = بيانات[0] + بيانات[1] + بيانات[2] + بيانات[3];
        
        return بيانات;
    }
};

/**
 * حساس BMP280 (ضغط جوي ودرجة حرارة)
 */
class حساس_BMP280 : public واجهة_طرفية_مدمجة {
private:
    double درجة_الحرارة_ = 25.0;    // °C
    double الضغط_ = 101325.0;        // Pa (1 atm)
    double الارتفاع_ = 0.0;          // meters
    
    uint8_t عنوان_ = 0x76;
    
public:
    std::string الاسم() const override { return "BMP280"; }
    std::string النوع() const override { return "حساس_ضغط_وحرارة"; }
    
    void إعادة_تعيين() override {
        درجة_الحرارة_ = 25.0;
        الضغط_ = 101325.0;
    }
    
    void تحديث(double وقت_ثانية) override {
        // تقلبات طبيعية
    }
    
    uint8_t عنوان_I2C() const override { return عنوان_; }
    
    std::vector<uint8_t> قراءة_I2C(uint8_t سجل, size_t عدد) override {
        std::vector<uint8_t> بيانات;
        
        switch (سجل) {
            case 0xD0:  // Chip ID
                بيانات.push_back(0x58);  // BMP280 ID
                break;
            case 0xF7:  // Pressure + Temperature data
                {
                    // Simplified: return raw values
                    uint32_t ضغط_خام = static_cast<uint32_t>(الضغط_ / 256.0);
                    uint32_t حرارة_خام = static_cast<uint32_t>((درجة_الحرارة_ + 40) * 256);
                    
                    بيانات.push_back((ضغط_خام >> 12) & 0xFF);
                    بيانات.push_back((ضغط_خام >> 4) & 0xFF);
                    بيانات.push_back((ضغط_خام << 4) & 0xFF);
                    بيانات.push_back((حرارة_خام >> 12) & 0xFF);
                    بيانات.push_back((حرارة_خام >> 4) & 0xFF);
                    بيانات.push_back((حرارة_خام << 4) & 0xFF);
                }
                break;
        }
        
        return بيانات;
    }
    
    double درجة_الحرارة() const { return درجة_الحرارة_; }
    double الضغط() const { return الضغط_; }
    double الارتفاع() const { return الارتفاع_; }
    
    void تعيين_درجة_الحرارة(double درجة) { درجة_الحرارة_ = درجة; }
    void تعيين_الضغط(double ضغط) { الضغط_ = ضغط; }
    
    void تعيين_الارتفاع(double ارتفاع) {
        الارتفاع_ = ارتفاع;
        // حساب الضغط من الارتفاع (صيغة تقريبية)
        الضغط_ = 101325.0 * std::pow(1 - ارتفاع / 44330.0, 5.255);
    }
};

// ============== حساسات الحركة ==============

/**
 * حساس MPU6050 (تسارع + جيروسكوب)
 */
class حساس_MPU6050 : public واجهة_طرفية_مدمجة {
private:
    // التسارع (g)
    double تسارع_x_ = 0.0;
    double تسارع_y_ = 0.0;
    double تسارع_z_ = 1.0;  // الجاذبية
    
    // السرعة الزاوية (°/s)
    double جيرو_x_ = 0.0;
    double جيرو_y_ = 0.0;
    double جيرو_z_ = 0.0;
    
    // الزوايا المحسوبة
    double ميل_ = 0.0;      // Roll
    double انحدار_ = 0.0;   // Pitch
    double دوران_ = 0.0;    // Yaw
    
    uint8_t عنوان_ = 0x68;
    
public:
    std::string الاسم() const override { return "MPU6050"; }
    std::string النوع() const override { return "حساس_تسارع_وجيروسكوب"; }
    
    void إعادة_تعيين() override {
        تسارع_x_ = تسارع_y_ = 0.0;
        تسارع_z_ = 1.0;
        جيرو_x_ = جيرو_y_ = جيرو_z_ = 0.0;
        ميل_ = انحدار_ = دوران_ = 0.0;
    }
    
    void تحديث(double وقت_ثانية) override {
        // تحديث الزوايا من الجيروسكوب
        ميل_ += جيرو_x_ * وقت_ثانية;
        انحدار_ += جيرو_y_ * وقت_ثانية;
        دوران_ += جيرو_z_ * وقت_ثانية;
        
        // تطبيع الزوايا
        ميل_ = std::fmod(ميل_, 360.0);
        انحدار_ = std::fmod(انحدار_, 360.0);
        دوران_ = std::fmod(دوران_, 360.0);
    }
    
    uint8_t عنوان_I2C() const override { return عنوان_; }
    
    std::vector<uint8_t> قراءة_I2C(uint8_t سجل, size_t عدد) override {
        std::vector<uint8_t> بيانات;
        
        switch (سجل) {
            case 0x75:  // WHO_AM_I
                بيانات.push_back(0x68);
                break;
            case 0x3B:  // ACCEL_XOUT_H (14 bytes: accel + temp + gyro)
                {
                    // Scale: ±2g = 16384 LSB/g
                    int16_t ax = static_cast<int16_t>(تسارع_x_ * 16384);
                    int16_t ay = static_cast<int16_t>(تسارع_y_ * 16384);
                    int16_t az = static_cast<int16_t>(تسارع_z_ * 16384);
                    
                    // Scale: ±250°/s = 131 LSB/°/s
                    int16_t gx = static_cast<int16_t>(جيرو_x_ * 131);
                    int16_t gy = static_cast<int16_t>(جيرو_y_ * 131);
                    int16_t gz = static_cast<int16_t>(جيرو_z_ * 131);
                    
                    بيانات.push_back((ax >> 8) & 0xFF);
                    بيانات.push_back(ax & 0xFF);
                    بيانات.push_back((ay >> 8) & 0xFF);
                    بيانات.push_back(ay & 0xFF);
                    بيانات.push_back((az >> 8) & 0xFF);
                    بيانات.push_back(az & 0xFF);
                    بيانات.push_back(0); بيانات.push_back(0);  // Temp placeholder
                    بيانات.push_back((gx >> 8) & 0xFF);
                    بيانات.push_back(gx & 0xFF);
                    بيانات.push_back((gy >> 8) & 0xFF);
                    بيانات.push_back(gy & 0xFF);
                    بيانات.push_back((gz >> 8) & 0xFF);
                    بيانات.push_back(gz & 0xFF);
                }
                break;
        }
        
        return بيانات;
    }
    
    // ============== API للمحاكاة ==============
    
    void تعيين_تسارع(double x, double y, double z) {
        تسارع_x_ = x;
        تسارع_y_ = y;
        تسارع_z_ = z;
    }
    
    void تعيين_جيروسكوب(double x, double y, double z) {
        جيرو_x_ = x;
        جيرو_y_ = y;
        جيرو_z_ = z;
    }
    
    void تعيين_زوايا(double ميل, double انحدار, double دوران) {
        ميل_ = ميل;
        انحدار_ = انحدار;
        دوران_ = دوران;
        
        // حساب التسارع من الزوايا (تقريبي)
        double ميل_راد = ميل * M_PI / 180.0;
        double انحدار_راد = انحدار * M_PI / 180.0;
        
        تسارع_x_ = std::sin(انحدار_راد);
        تسارع_y_ = -std::sin(ميل_راد) * std::cos(انحدار_راد);
        تسارع_z_ = std::cos(ميل_راد) * std::cos(انحدار_راد);
    }
    
    double تسارع_x() const { return تسارع_x_; }
    double تسارع_y() const { return تسارع_y_; }
    double تسارع_z() const { return تسارع_z_; }
    double جيرو_x() const { return جيرو_x_; }
    double جيرو_y() const { return جيرو_y_; }
    double جيرو_z() const { return جيرو_z_; }
    double ميل() const { return ميل_; }
    double انحدار() const { return انحدار_; }
    double دوران() const { return دوران_; }
};

// ============== المخرجات ==============

/**
 * LED
 */
class LED : public واجهة_طرفية_مدمجة {
private:
    bool مضيء_ = false;
    uint8_t سطوع_ = 0;      // 0-255
    uint8_t لون_أحمر_ = 255;
    uint8_t لون_أخضر_ = 0;
    uint8_t لون_أزرق_ = 0;
    bool RGB_ = false;
    
public:
    LED(bool rgb = false) : RGB_(rgb) {}
    
    std::string الاسم() const override { return RGB_ ? "RGB LED" : "LED"; }
    std::string النوع() const override { return "مخرج_ضوئي"; }
    
    void إعادة_تعيين() override {
        مضيء_ = false;
        سطوع_ = 0;
    }
    
    void تحديث(double وقت_ثانية) override {}
    
    void تشغيل() { مضيء_ = true; سطوع_ = 255; }
    void إيقاف() { مضيء_ = false; سطوع_ = 0; }
    void تعيين_سطوع(uint8_t سطوع) { سطوع_ = سطوع; مضيء_ = سطوع > 0; }
    
    void تعيين_لون(uint8_t أحمر, uint8_t أخضر, uint8_t أزرق) {
        if (RGB_) {
            لون_أحمر_ = أحمر;
            لون_أخضر_ = أخضر;
            لون_أزرق_ = أزرق;
        }
    }
    
    bool مضيء() const { return مضيء_; }
    uint8_t سطوع() const { return سطوع_; }
    uint8_t أحمر() const { return لون_أحمر_; }
    uint8_t أخضر() const { return لون_أخضر_; }
    uint8_t أزرق() const { return لون_أزرق_; }
};

/**
 * محرك سيرفو
 */
class محرك_سيرفو : public واجهة_طرفية_مدمجة {
private:
    double زاوية_ = 90.0;           // 0-180 degrees
    double زاوية_هدف_ = 90.0;
    double سرعة_ = 300.0;           // degrees per second
    double زاوية_أدنى_ = 0.0;
    double زاوية_أعلى_ = 180.0;
    
public:
    std::string الاسم() const override { return "Servo Motor"; }
    std::string النوع() const override { return "محرك"; }
    
    void إعادة_تعيين() override {
        زاوية_ = زاوية_هدف_ = 90.0;
    }
    
    void تحديث(double وقت_ثانية) override {
        // تحريك تدريجي نحو الزاوية الهدف
        double فرق = زاوية_هدف_ - زاوية_;
        double حركة = سرعة_ * وقت_ثانية;
        
        if (std::abs(فرق) <= حركة) {
            زاوية_ = زاوية_هدف_;
        } else {
            زاوية_ += (فرق > 0) ? حركة : -حركة;
        }
    }
    
    void تعيين_زاوية(double زاوية) {
        زاوية_هدف_ = std::clamp(زاوية, زاوية_أدنى_, زاوية_أعلى_);
    }
    
    void تعيين_نبض_مايكروثانية(int نبض) {
        // 1000μs = 0°, 2000μs = 180°
        double زاوية = (نبض - 1000) * 180.0 / 1000.0;
        تعيين_زاوية(زاوية);
    }
    
    double الزاوية() const { return زاوية_; }
    double الزاوية_الهدف() const { return زاوية_هدف_; }
    bool وصل_للهدف() const { return std::abs(زاوية_ - زاوية_هدف_) < 0.1; }
};

/**
 * محرك DC
 */
class محرك_DC : public واجهة_طرفية_مدمجة {
private:
    double سرعة_ = 0.0;             // -1.0 to 1.0
    double سرعة_فعلية_ = 0.0;       // Actual speed with inertia
    double تسارع_ = 5.0;            // acceleration rate
    bool تمكين_ = false;
    
public:
    std::string الاسم() const override { return "DC Motor"; }
    std::string النوع() const override { return "محرك"; }
    
    void إعادة_تعيين() override {
        سرعة_ = سرعة_فعلية_ = 0.0;
        تمكين_ = false;
    }
    
    void تحديث(double وقت_ثانية) override {
        // محاكاة القصور الذاتي
        double هدف = تمكين_ ? سرعة_ : 0.0;
        double فرق = هدف - سرعة_فعلية_;
        double تغيير = تسارع_ * وقت_ثانية;
        
        if (std::abs(فرق) <= تغيير) {
            سرعة_فعلية_ = هدف;
        } else {
            سرعة_فعلية_ += (فرق > 0) ? تغيير : -تغيير;
        }
    }
    
    void تعيين_سرعة(double سرعة) {
        سرعة_ = std::clamp(سرعة, -1.0, 1.0);
    }
    
    void تمكين() { تمكين_ = true; }
    void تعطيل() { تمكين_ = false; }
    
    double السرعة() const { return سرعة_; }
    double السرعة_الفعلية() const { return سرعة_فعلية_; }
    bool ممكّن() const { return تمكين_; }
};

// ============== الشاشات ==============

/**
 * شاشة LCD (HD44780)
 */
class شاشة_LCD : public واجهة_طرفية_مدمجة {
private:
    int عدد_أعمدة_ = 16;
    int عدد_صفوف_ = 2;
    std::vector<std::string> محتوى_;
    int موضع_عمود_ = 0;
    int موضع_صف_ = 0;
    bool إضاءة_خلفية_ = true;
    bool مرئي_ = true;
    
public:
    شاشة_LCD(int أعمدة = 16, int صفوف = 2) 
        : عدد_أعمدة_(أعمدة), عدد_صفوف_(صفوف) {
        محتوى_.resize(صفوف, std::string(أعمدة, ' '));
    }
    
    std::string الاسم() const override { return "LCD " + std::to_string(عدد_أعمدة_) + "x" + std::to_string(عدد_صفوف_); }
    std::string النوع() const override { return "شاشة"; }
    
    void إعادة_تعيين() override {
        مسح();
        موضع_عمود_ = موضع_صف_ = 0;
    }
    
    void تحديث(double وقت_ثانية) override {}
    
    void مسح() {
        for (auto& صف : محتوى_) {
            صف = std::string(عدد_أعمدة_, ' ');
        }
        موضع_عمود_ = موضع_صف_ = 0;
    }
    
    void تعيين_موضع(int عمود, int صف) {
        موضع_عمود_ = std::clamp(عمود, 0, عدد_أعمدة_ - 1);
        موضع_صف_ = std::clamp(صف, 0, عدد_صفوف_ - 1);
    }
    
    void كتابة(const std::string& نص) {
        for (char c : نص) {
            if (موضع_عمود_ < عدد_أعمدة_ && موضع_صف_ < عدد_صفوف_) {
                محتوى_[موضع_صف_][موضع_عمود_] = c;
                موضع_عمود_++;
                if (موضع_عمود_ >= عدد_أعمدة_) {
                    موضع_عمود_ = 0;
                    موضع_صف_++;
                }
            }
        }
    }
    
    void كتابة_حرف(char c) {
        if (موضع_عمود_ < عدد_أعمدة_ && موضع_صف_ < عدد_صفوف_) {
            محتوى_[موضع_صف_][موضع_عمود_] = c;
            موضع_عمود_++;
        }
    }
    
    std::string قراءة_صف(int صف) const {
        if (صف >= 0 && صف < عدد_صفوف_) {
            return محتوى_[صف];
        }
        return "";
    }
    
    std::vector<std::string> المحتوى() const { return محتوى_; }
    
    void تعيين_إضاءة_خلفية(bool تشغيل) { إضاءة_خلفية_ = تشغيل; }
    bool إضاءة_خلفية() const { return إضاءة_خلفية_; }
};

/**
 * شاشة OLED (SSD1306)
 */
class شاشة_OLED : public واجهة_طرفية_مدمجة {
private:
    int عرض_ = 128;
    int ارتفاع_ = 64;
    std::vector<uint8_t> ذاكرة_عرض_;  // Framebuffer
    uint8_t عنوان_ = 0x3C;
    
public:
    شاشة_OLED(int عرض = 128, int ارتفاع = 64)
        : عرض_(عرض), ارتفاع_(ارتفاع) {
        ذاكرة_عرض_.resize(عرض * ارتفاع / 8, 0);
    }
    
    std::string الاسم() const override { return "OLED SSD1306"; }
    std::string النوع() const override { return "شاشة"; }
    
    void إعادة_تعيين() override {
        std::fill(ذاكرة_عرض_.begin(), ذاكرة_عرض_.end(), 0);
    }
    
    void تحديث(double وقت_ثانية) override {}
    
    uint8_t عنوان_I2C() const override { return عنوان_; }
    
    bool كتابة_I2C(uint8_t سجل, const std::vector<uint8_t>& بيانات) override {
        if (سجل == 0x40) {  // Data
            // Copy to framebuffer
            size_t حجم = std::min(بيانات.size(), ذاكرة_عرض_.size());
            std::copy(بيانات.begin(), بيانات.begin() + حجم, ذاكرة_عرض_.begin());
        }
        return true;
    }
    
    void مسح() {
        std::fill(ذاكرة_عرض_.begin(), ذاكرة_عرض_.end(), 0);
    }
    
    void تعيين_بكسل(int x, int y, bool مضيء) {
        if (x < 0 || x >= عرض_ || y < 0 || y >= ارتفاع_) return;
        
        int بايت = x + (y / 8) * عرض_;
        int بت = y % 8;
        
        if (مضيء) {
            ذاكرة_عرض_[بايت] |= (1 << بت);
        } else {
            ذاكرة_عرض_[بايت] &= ~(1 << بت);
        }
    }
    
    bool قراءة_بكسل(int x, int y) const {
        if (x < 0 || x >= عرض_ || y < 0 || y >= ارتفاع_) return false;
        
        int بايت = x + (y / 8) * عرض_;
        int بت = y % 8;
        
        return (ذاكرة_عرض_[بايت] >> بت) & 1;
    }
    
    const std::vector<uint8_t>& ذاكرة_العرض() const { return ذاكرة_عرض_; }
    int العرض() const { return عرض_; }
    int الارتفاع() const { return ارتفاع_; }
};

// ============== مدير الطرفيات ==============

/**
 * مدير الطرفيات المدمجة
 */
class مدير_طرفيات {
private:
    std::map<std::string, std::unique_ptr<واجهة_طرفية_مدمجة>> طرفيات_;
    
public:
    /**
     * إضافة طرفية
     */
    template<typename T, typename... Args>
    T* إضافة(const std::string& اسم, Args&&... args) {
        auto طرفية = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = طرفية.get();
        طرفيات_[اسم] = std::move(طرفية);
        return ptr;
    }
    
    /**
     * الحصول على طرفية
     */
    واجهة_طرفية_مدمجة* طرفية(const std::string& اسم) {
        auto it = طرفيات_.find(اسم);
        return (it != طرفيات_.end()) ? it->second.get() : nullptr;
    }
    
    template<typename T>
    T* طرفية_كـ(const std::string& اسم) {
        return dynamic_cast<T*>(طرفية(اسم));
    }
    
    /**
     * تحديث جميع الطرفيات
     */
    void تحديث(double وقت_ثانية) {
        for (auto& [اسم, طرفية] : طرفيات_) {
            طرفية->تحديث(وقت_ثانية);
        }
    }
    
    /**
     * إعادة تعيين جميع الطرفيات
     */
    void إعادة_تعيين() {
        for (auto& [اسم, طرفية] : طرفيات_) {
            طرفية->إعادة_تعيين();
        }
    }
    
    /**
     * قائمة الطرفيات
     */
    std::vector<std::string> قائمة() const {
        std::vector<std::string> أسماء;
        for (const auto& [اسم, _] : طرفيات_) {
            أسماء.push_back(اسم);
        }
        return أسماء;
    }
};

}  // namespace ص::محاكاة

// ============== C API ==============

extern "C" {

using namespace ص::محاكاة;

typedef struct مدير_طرفيات_مقبض {
    std::unique_ptr<مدير_طرفيات> مدير;
} مدير_طرفيات_مقبض;

مدير_طرفيات_مقبض* sad_peripherals_create() {
    auto مقبض = new مدير_طرفيات_مقبض();
    مقبض->مدير = std::make_unique<مدير_طرفيات>();
    return مقبض;
}

void sad_peripherals_destroy(مدير_طرفيات_مقبض* مقبض) {
    delete مقبض;
}

void* sad_peripherals_add_dht(مدير_طرفيات_مقبض* مقبض, const char* اسم, int نوع) {
    if (!مقبض || !مقبض->مدير) return nullptr;
    return مقبض->مدير->إضافة<حساس_DHT>(اسم, static_cast<حساس_DHT::نوع_DHT>(نوع));
}

void* sad_peripherals_add_mpu6050(مدير_طرفيات_مقبض* مقبض, const char* اسم) {
    if (!مقبض || !مقبض->مدير) return nullptr;
    return مقبض->مدير->إضافة<حساس_MPU6050>(اسم);
}

void* sad_peripherals_add_led(مدير_طرفيات_مقبض* مقبض, const char* اسم, int rgb) {
    if (!مقبض || !مقبض->مدير) return nullptr;
    return مقبض->مدير->إضافة<LED>(اسم, rgb != 0);
}

void* sad_peripherals_add_servo(مدير_طرفيات_مقبض* مقبض, const char* اسم) {
    if (!مقبض || !مقبض->مدير) return nullptr;
    return مقبض->مدير->إضافة<محرك_سيرفو>(اسم);
}

void* sad_peripherals_add_lcd(مدير_طرفيات_مقبض* مقبض, const char* اسم, int cols, int rows) {
    if (!مقبض || !مقبض->مدير) return nullptr;
    return مقبض->مدير->إضافة<شاشة_LCD>(اسم, cols, rows);
}

void sad_peripherals_update(مدير_طرفيات_مقبض* مقبض, double dt) {
    if (مقبض && مقبض->مدير) {
        مقبض->مدير->تحديث(dt);
    }
}

void sad_peripherals_reset(مدير_طرفيات_مقبض* مقبض) {
    if (مقبض && مقبض->مدير) {
        مقبض->مدير->إعادة_تعيين();
    }
}

// DHT specific
double sad_dht_temperature(void* حساس) {
    auto* dht = static_cast<حساس_DHT*>(حساس);
    return dht ? dht->درجة_الحرارة() : 0.0;
}

double sad_dht_humidity(void* حساس) {
    auto* dht = static_cast<حساس_DHT*>(حساس);
    return dht ? dht->الرطوبة() : 0.0;
}

void sad_dht_set_temperature(void* حساس, double temp) {
    auto* dht = static_cast<حساس_DHT*>(حساس);
    if (dht) dht->تعيين_درجة_الحرارة(temp);
}

// Servo specific
void sad_servo_set_angle(void* سيرفو, double angle) {
    auto* s = static_cast<محرك_سيرفو*>(سيرفو);
    if (s) s->تعيين_زاوية(angle);
}

double sad_servo_get_angle(void* سيرفو) {
    auto* s = static_cast<محرك_سيرفو*>(سيرفو);
    return s ? s->الزاوية() : 0.0;
}

// LCD specific
void sad_lcd_clear(void* شاشة) {
    auto* lcd = static_cast<شاشة_LCD*>(شاشة);
    if (lcd) lcd->مسح();
}

void sad_lcd_write(void* شاشة, const char* text) {
    auto* lcd = static_cast<شاشة_LCD*>(شاشة);
    if (lcd) lcd->كتابة(text);
}

void sad_lcd_set_cursor(void* شاشة, int col, int row) {
    auto* lcd = static_cast<شاشة_LCD*>(شاشة);
    if (lcd) lcd->تعيين_موضع(col, row);
}

}  // extern "C"
