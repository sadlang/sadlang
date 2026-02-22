/**
 * ===================================================================================
 *  مصحح مرئي للأجهزة المحاكاة - Visual Debugger for Emulated Devices (T400)
 * ===================================================================================
 * 
 *  الوصف: واجهة مرئية لتصحيح الأجهزة المدمجة المحاكاة
 *  Description: Visual interface for debugging emulated embedded devices
 *
 *  المميزات:
 *  - عرض حالة GPIO في الوقت الحقيقي
 *  - رسم بياني للقيم التناظرية
 *  - عرض Serial monitor
 *  - محاكاة المكونات المرئية (LED، شاشات، إلخ)
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <cstdint>
#include <deque>
#include <chrono>

namespace ص::محاكاة {

// ============== أنواع العرض ==============

/**
 * لون RGB
 */
struct لون {
    uint8_t أحمر = 0;
    uint8_t أخضر = 0;
    uint8_t أزرق = 0;
    uint8_t شفافية = 255;
    
    لون() = default;
    لون(uint8_t ح, uint8_t خ, uint8_t ز, uint8_t ش = 255)
        : أحمر(ح), أخضر(خ), أزرق(ز), شفافية(ش) {}
    
    static لون أحمر_() { return {255, 0, 0}; }
    static لون أخضر_() { return {0, 255, 0}; }
    static لون أزرق_() { return {0, 0, 255}; }
    static لون أبيض() { return {255, 255, 255}; }
    static لون أسود() { return {0, 0, 0}; }
    static لون رمادي() { return {128, 128, 128}; }
    static لون أصفر() { return {255, 255, 0}; }
    static لون برتقالي() { return {255, 165, 0}; }
};

/**
 * نقطة بيانات للرسم البياني
 */
struct نقطة_بيانات {
    double وقت;
    double قيمة;
};

/**
 * قناة بيانات للرسم البياني
 */
struct قناة_رسم {
    std::string اسم;
    لون لون_الخط;
    std::deque<نقطة_بيانات> بيانات;
    double حد_أدنى = 0;
    double حد_أعلى = 100;
    size_t حد_نقاط = 1000;
    
    void إضافة_نقطة(double وقت, double قيمة) {
        بيانات.push_back({وقت, قيمة});
        while (بيانات.size() > حد_نقاط) {
            بيانات.pop_front();
        }
    }
    
    void مسح() {
        بيانات.clear();
    }
};

// ============== عناصر العرض ==============

/**
 * عنصر GPIO مرئي
 */
struct عنصر_GPIO_مرئي {
    int رقم_PIN;
    std::string تسمية;
    bool مخرج;
    bool حالة;
    double قيمة_تناظرية;  // 0.0 - 1.0
    bool PWM_مفعل;
    لون لون_مرتفع = لون::أخضر_();
    لون لون_منخفض = لون::رمادي();
    
    // موقع العرض
    int x = 0;
    int y = 0;
    int عرض = 40;
    int ارتفاع = 40;
};

/**
 * عنصر LED مرئي
 */
struct عنصر_LED_مرئي {
    std::string تسمية;
    bool مضيء;
    uint8_t سطوع;          // 0-255
    لون لون_LED = لون::أحمر_();
    
    int x = 0;
    int y = 0;
    int نصف_قطر = 15;
};

/**
 * عنصر شاشة LCD مرئي
 */
struct عنصر_LCD_مرئي {
    std::string تسمية;
    int أعمدة;
    int صفوف;
    std::vector<std::string> محتوى;
    bool إضاءة_خلفية = true;
    لون لون_نص = لون(0, 100, 0);        // أخضر داكن
    لون لون_خلفية = لون(200, 230, 200);  // أخضر فاتح
    
    int x = 0;
    int y = 0;
    int عرض_حرف = 12;
    int ارتفاع_حرف = 18;
};

/**
 * عنصر محرك سيرفو مرئي
 */
struct عنصر_سيرفو_مرئي {
    std::string تسمية;
    double زاوية;          // 0-180
    double زاوية_أدنى = 0;
    double زاوية_أعلى = 180;
    
    int x = 0;
    int y = 0;
    int نصف_قطر = 40;
};

/**
 * عنصر مقياس (Gauge) مرئي
 */
struct عنصر_مقياس_مرئي {
    std::string تسمية;
    std::string وحدة;
    double قيمة;
    double حد_أدنى;
    double حد_أعلى;
    لون لون_مؤشر = لون::أحمر_();
    
    int x = 0;
    int y = 0;
    int نصف_قطر = 50;
};

// ============== مراقب Serial ==============

/**
 * سجل Serial
 */
struct سجل_Serial {
    enum class اتجاه { وارد, صادر };
    
    اتجاه الاتجاه;
    std::string بيانات;
    std::chrono::steady_clock::time_point وقت;
    لون لون_نص;
};

/**
 * مراقب Serial
 */
class مراقب_Serial {
private:
    std::deque<سجل_Serial> سجلات_;
    size_t حد_سجلات_ = 1000;
    
    // فلترة
    bool إظهار_وارد_ = true;
    bool إظهار_صادر_ = true;
    std::string فلتر_نص_;
    
    // تنسيق العرض
    enum class تنسيق_عرض { نص, سداسي_عشري, ASCII };
    تنسيق_عرض تنسيق_ = تنسيق_عرض::نص;
    
public:
    /**
     * إضافة سجل وارد
     */
    void إضافة_وارد(const std::string& بيانات) {
        سجل_Serial سجل;
        سجل.الاتجاه = سجل_Serial::اتجاه::وارد;
        سجل.بيانات = بيانات;
        سجل.وقت = std::chrono::steady_clock::now();
        سجل.لون_نص = لون::أخضر_();
        
        سجلات_.push_back(سجل);
        تقليم();
    }
    
    /**
     * إضافة سجل صادر
     */
    void إضافة_صادر(const std::string& بيانات) {
        سجل_Serial سجل;
        سجل.الاتجاه = سجل_Serial::اتجاه::صادر;
        سجل.بيانات = بيانات;
        سجل.وقت = std::chrono::steady_clock::now();
        سجل.لون_نص = لون::أزرق_();
        
        سجلات_.push_back(سجل);
        تقليم();
    }
    
    /**
     * مسح السجلات
     */
    void مسح() {
        سجلات_.clear();
    }
    
    /**
     * الحصول على السجلات المفلترة
     */
    std::vector<سجل_Serial> السجلات_المفلترة() const {
        std::vector<سجل_Serial> نتيجة;
        
        for (const auto& سجل : سجلات_) {
            // فلترة الاتجاه
            if (سجل.الاتجاه == سجل_Serial::اتجاه::وارد && !إظهار_وارد_) continue;
            if (سجل.الاتجاه == سجل_Serial::اتجاه::صادر && !إظهار_صادر_) continue;
            
            // فلترة النص
            if (!فلتر_نص_.empty() && 
                سجل.بيانات.find(فلتر_نص_) == std::string::npos) continue;
            
            نتيجة.push_back(سجل);
        }
        
        return نتيجة;
    }
    
    /**
     * تنسيق البيانات للعرض
     */
    std::string تنسيق(const std::string& بيانات) const {
        switch (تنسيق_) {
            case تنسيق_عرض::نص:
                return بيانات;
                
            case تنسيق_عرض::سداسي_عشري: {
                std::string نتيجة;
                for (unsigned char c : بيانات) {
                    char buf[4];
                    snprintf(buf, sizeof(buf), "%02X ", c);
                    نتيجة += buf;
                }
                return نتيجة;
            }
            
            case تنسيق_عرض::ASCII: {
                std::string نتيجة;
                for (unsigned char c : بيانات) {
                    if (c >= 32 && c < 127) {
                        نتيجة += c;
                    } else {
                        نتيجة += '.';
                    }
                }
                return نتيجة;
            }
        }
        
        return بيانات;
    }
    
    // Setters
    void تعيين_فلتر(const std::string& فلتر) { فلتر_نص_ = فلتر; }
    void تعيين_إظهار_وارد(bool إظهار) { إظهار_وارد_ = إظهار; }
    void تعيين_إظهار_صادر(bool إظهار) { إظهار_صادر_ = إظهار; }
    void تعيين_تنسيق(تنسيق_عرض تنسيق) { تنسيق_ = تنسيق; }
    
private:
    void تقليم() {
        while (سجلات_.size() > حد_سجلات_) {
            سجلات_.pop_front();
        }
    }
};

// ============== المصحح المرئي ==============

/**
 * المصحح المرئي للأجهزة المدمجة
 */
class مصحح_مرئي {
private:
    // عناصر GPIO
    std::vector<عنصر_GPIO_مرئي> GPIO_;
    
    // عناصر أخرى
    std::vector<عنصر_LED_مرئي> LEDs_;
    std::vector<عنصر_LCD_مرئي> LCDs_;
    std::vector<عنصر_سيرفو_مرئي> سيرفوات_;
    std::vector<عنصر_مقياس_مرئي> مقاييس_;
    
    // الرسم البياني
    std::map<std::string, قناة_رسم> قنوات_رسم_;
    double وقت_حالي_ = 0;
    
    // مراقب Serial
    مراقب_Serial مراقب_serial_;
    
    // الحالة
    bool تحديث_مستمر_ = true;
    double معدل_تحديث_ = 30.0;  // FPS
    
    // أبعاد العرض
    int عرض_عرض_ = 800;
    int ارتفاع_عرض_ = 600;
    
public:
    // ============== إعداد GPIO ==============
    
    /**
     * إضافة عنصر GPIO
     */
    void إضافة_GPIO(int رقم, const std::string& تسمية, int x, int y) {
        عنصر_GPIO_مرئي عنصر;
        عنصر.رقم_PIN = رقم;
        عنصر.تسمية = تسمية;
        عنصر.x = x;
        عنصر.y = y;
        GPIO_.push_back(عنصر);
    }
    
    /**
     * تحديث حالة GPIO
     */
    void تحديث_GPIO(int رقم, bool مخرج, bool حالة) {
        for (auto& gpio : GPIO_) {
            if (gpio.رقم_PIN == رقم) {
                gpio.مخرج = مخرج;
                gpio.حالة = حالة;
                break;
            }
        }
    }
    
    /**
     * تحديث قيمة PWM
     */
    void تحديث_PWM(int رقم, double قيمة) {
        for (auto& gpio : GPIO_) {
            if (gpio.رقم_PIN == رقم) {
                gpio.PWM_مفعل = true;
                gpio.قيمة_تناظرية = قيمة;
                break;
            }
        }
    }
    
    // ============== إعداد العناصر ==============
    
    /**
     * إضافة LED
     */
    void إضافة_LED(const std::string& تسمية, int x, int y, لون لون_LED = لون::أحمر_()) {
        عنصر_LED_مرئي led;
        led.تسمية = تسمية;
        led.x = x;
        led.y = y;
        led.لون_LED = لون_LED;
        LEDs_.push_back(led);
    }
    
    /**
     * تحديث LED
     */
    void تحديث_LED(const std::string& تسمية, bool مضيء, uint8_t سطوع = 255) {
        for (auto& led : LEDs_) {
            if (led.تسمية == تسمية) {
                led.مضيء = مضيء;
                led.سطوع = سطوع;
                break;
            }
        }
    }
    
    /**
     * إضافة شاشة LCD
     */
    void إضافة_LCD(const std::string& تسمية, int أعمدة, int صفوف, int x, int y) {
        عنصر_LCD_مرئي lcd;
        lcd.تسمية = تسمية;
        lcd.أعمدة = أعمدة;
        lcd.صفوف = صفوف;
        lcd.محتوى.resize(صفوف, std::string(أعمدة, ' '));
        lcd.x = x;
        lcd.y = y;
        LCDs_.push_back(lcd);
    }
    
    /**
     * تحديث شاشة LCD
     */
    void تحديث_LCD(const std::string& تسمية, const std::vector<std::string>& محتوى) {
        for (auto& lcd : LCDs_) {
            if (lcd.تسمية == تسمية) {
                lcd.محتوى = محتوى;
                break;
            }
        }
    }
    
    /**
     * إضافة سيرفو
     */
    void إضافة_سيرفو(const std::string& تسمية, int x, int y) {
        عنصر_سيرفو_مرئي سيرفو;
        سيرفو.تسمية = تسمية;
        سيرفو.x = x;
        سيرفو.y = y;
        سيرفوات_.push_back(سيرفو);
    }
    
    /**
     * تحديث سيرفو
     */
    void تحديث_سيرفو(const std::string& تسمية, double زاوية) {
        for (auto& سيرفو : سيرفوات_) {
            if (سيرفو.تسمية == تسمية) {
                سيرفو.زاوية = زاوية;
                break;
            }
        }
    }
    
    /**
     * إضافة مقياس
     */
    void إضافة_مقياس(const std::string& تسمية, const std::string& وحدة,
                     double حد_أدنى, double حد_أعلى, int x, int y) {
        عنصر_مقياس_مرئي مقياس;
        مقياس.تسمية = تسمية;
        مقياس.وحدة = وحدة;
        مقياس.حد_أدنى = حد_أدنى;
        مقياس.حد_أعلى = حد_أعلى;
        مقياس.x = x;
        مقياس.y = y;
        مقاييس_.push_back(مقياس);
    }
    
    /**
     * تحديث مقياس
     */
    void تحديث_مقياس(const std::string& تسمية, double قيمة) {
        for (auto& مقياس : مقاييس_) {
            if (مقياس.تسمية == تسمية) {
                مقياس.قيمة = قيمة;
                break;
            }
        }
    }
    
    // ============== الرسم البياني ==============
    
    /**
     * إضافة قناة رسم
     */
    void إضافة_قناة_رسم(const std::string& اسم, لون لون_خط,
                        double حد_أدنى = 0, double حد_أعلى = 100) {
        قناة_رسم قناة;
        قناة.اسم = اسم;
        قناة.لون_الخط = لون_خط;
        قناة.حد_أدنى = حد_أدنى;
        قناة.حد_أعلى = حد_أعلى;
        قنوات_رسم_[اسم] = قناة;
    }
    
    /**
     * إضافة نقطة بيانات
     */
    void إضافة_نقطة_رسم(const std::string& قناة, double قيمة) {
        auto it = قنوات_رسم_.find(قناة);
        if (it != قنوات_رسم_.end()) {
            it->second.إضافة_نقطة(وقت_حالي_, قيمة);
        }
    }
    
    /**
     * تقدم الوقت
     */
    void تقدم_وقت(double ثانية) {
        وقت_حالي_ += ثانية;
    }
    
    // ============== Serial ==============
    
    مراقب_Serial& المراقب_Serial() { return مراقب_serial_; }
    const مراقب_Serial& المراقب_Serial() const { return مراقب_serial_; }
    
    // ============== توليد العرض ==============
    
    /**
     * توليد HTML للعرض
     */
    std::string توليد_HTML() const {
        std::string html = R"(
<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>مصحح الأجهزة المدمجة - ص</title>
    <style>
        body { font-family: Arial, sans-serif; background: #1e1e1e; color: #fff; }
        .container { display: flex; flex-wrap: wrap; gap: 20px; padding: 20px; }
        .panel { background: #2d2d2d; border-radius: 8px; padding: 15px; }
        .gpio-grid { display: grid; grid-template-columns: repeat(8, 1fr); gap: 10px; }
        .gpio-pin { width: 40px; height: 40px; border-radius: 50%; display: flex; 
                    align-items: center; justify-content: center; font-size: 12px; }
        .gpio-high { background: #00ff00; color: #000; }
        .gpio-low { background: #555; }
        .led { border-radius: 50%; box-shadow: 0 0 10px; }
        .lcd { font-family: monospace; background: #c8e6c9; color: #1b5e20; 
               padding: 10px; border: 3px solid #333; }
        .serial-monitor { height: 200px; overflow-y: auto; font-family: monospace; 
                         background: #000; padding: 10px; }
        .serial-in { color: #0f0; }
        .serial-out { color: #00f; }
        .chart { background: #1a1a1a; border: 1px solid #333; }
    </style>
</head>
<body>
    <div class="container">
)";
        
        // GPIO Panel
        html += "<div class='panel'><h3>GPIO</h3><div class='gpio-grid'>";
        for (const auto& gpio : GPIO_) {
            std::string صنف = gpio.حالة ? "gpio-high" : "gpio-low";
            html += "<div class='gpio-pin " + صنف + "'>" + 
                    std::to_string(gpio.رقم_PIN) + "</div>";
        }
        html += "</div></div>";
        
        // LEDs Panel
        if (!LEDs_.empty()) {
            html += "<div class='panel'><h3>LEDs</h3>";
            for (const auto& led : LEDs_) {
                std::string لون = led.مضيء ? 
                    "rgb(" + std::to_string(led.لون_LED.أحمر) + "," +
                            std::to_string(led.لون_LED.أخضر) + "," +
                            std::to_string(led.لون_LED.أزرق) + ")" : "#333";
                html += "<div class='led' style='width: 30px; height: 30px; "
                        "background: " + لون + "'></div>";
            }
            html += "</div>";
        }
        
        // LCD Panels
        for (const auto& lcd : LCDs_) {
            html += "<div class='panel'><h3>" + lcd.تسمية + "</h3><div class='lcd'>";
            for (const auto& صف : lcd.محتوى) {
                html += صف + "<br>";
            }
            html += "</div></div>";
        }
        
        // Serial Monitor
        html += "<div class='panel' style='width: 400px;'><h3>Serial Monitor</h3>";
        html += "<div class='serial-monitor'>";
        for (const auto& سجل : مراقب_serial_.السجلات_المفلترة()) {
            std::string صنف = (سجل.الاتجاه == سجل_Serial::اتجاه::وارد) ? 
                              "serial-in" : "serial-out";
            html += "<div class='" + صنف + "'>" + سجل.بيانات + "</div>";
        }
        html += "</div></div>";
        
        html += "</div></body></html>";
        
        return html;
    }
    
    /**
     * توليد SVG للرسم البياني
     */
    std::string توليد_SVG_رسم(int عرض = 600, int ارتفاع = 300) const {
        std::string svg = "<svg width='" + std::to_string(عرض) + 
                          "' height='" + std::to_string(ارتفاع) + 
                          "' xmlns='http://www.w3.org/2000/svg'>";
        
        // خلفية
        svg += "<rect width='100%' height='100%' fill='#1a1a1a'/>";
        
        // خطوط الشبكة
        for (int i = 0; i <= 10; ++i) {
            int y = ارتفاع * i / 10;
            svg += "<line x1='0' y1='" + std::to_string(y) + 
                   "' x2='" + std::to_string(عرض) + 
                   "' y2='" + std::to_string(y) + 
                   "' stroke='#333' stroke-width='1'/>";
        }
        
        // رسم القنوات
        for (const auto& [اسم, قناة] : قنوات_رسم_) {
            if (قناة.بيانات.empty()) continue;
            
            std::string نقاط;
            double نطاق_وقت = 10.0;  // 10 ثوانٍ
            double وقت_بداية = وقت_حالي_ - نطاق_وقت;
            
            for (const auto& نقطة : قناة.بيانات) {
                if (نقطة.وقت < وقت_بداية) continue;
                
                double x = (نقطة.وقت - وقت_بداية) / نطاق_وقت * عرض;
                double y = ارتفاع - (نقطة.قيمة - قناة.حد_أدنى) / 
                           (قناة.حد_أعلى - قناة.حد_أدنى) * ارتفاع;
                
                نقاط += std::to_string(x) + "," + std::to_string(y) + " ";
            }
            
            if (!نقاط.empty()) {
                std::string لون = "rgb(" + std::to_string(قناة.لون_الخط.أحمر) + "," +
                                          std::to_string(قناة.لون_الخط.أخضر) + "," +
                                          std::to_string(قناة.لون_الخط.أزرق) + ")";
                svg += "<polyline points='" + نقاط + 
                       "' fill='none' stroke='" + لون + "' stroke-width='2'/>";
            }
        }
        
        svg += "</svg>";
        return svg;
    }
    
    /**
     * توليد JSON للحالة
     */
    std::string توليد_JSON() const {
        std::string json = "{";
        
        // GPIO
        json += "\"gpio\": [";
        for (size_t i = 0; i < GPIO_.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"pin\": " + std::to_string(GPIO_[i].رقم_PIN) +
                    ", \"output\": " + (GPIO_[i].مخرج ? "true" : "false") +
                    ", \"state\": " + (GPIO_[i].حالة ? "true" : "false") + "}";
        }
        json += "],";
        
        // LEDs
        json += "\"leds\": [";
        for (size_t i = 0; i < LEDs_.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"name\": \"" + LEDs_[i].تسمية +
                    "\", \"on\": " + (LEDs_[i].مضيء ? "true" : "false") +
                    ", \"brightness\": " + std::to_string(LEDs_[i].سطوع) + "}";
        }
        json += "],";
        
        // Servos
        json += "\"servos\": [";
        for (size_t i = 0; i < سيرفوات_.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"name\": \"" + سيرفوات_[i].تسمية +
                    "\", \"angle\": " + std::to_string(سيرفوات_[i].زاوية) + "}";
        }
        json += "],";
        
        // Gauges
        json += "\"gauges\": [";
        for (size_t i = 0; i < مقاييس_.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"name\": \"" + مقاييس_[i].تسمية +
                    "\", \"value\": " + std::to_string(مقاييس_[i].قيمة) +
                    ", \"unit\": \"" + مقاييس_[i].وحدة + "\"}";
        }
        json += "]";
        
        json += "}";
        return json;
    }
    
    // ============== الإعدادات ==============
    
    void تعيين_أبعاد(int عرض, int ارتفاع) {
        عرض_عرض_ = عرض;
        ارتفاع_عرض_ = ارتفاع;
    }
    
    void تعيين_معدل_تحديث(double معدل) {
        معدل_تحديث_ = معدل;
    }
};

}  // namespace ص::محاكاة

// ============== C API ==============

extern "C" {

using namespace ص::محاكاة;

typedef struct مصحح_مرئي_مقبض {
    std::unique_ptr<مصحح_مرئي> مصحح;
} مصحح_مرئي_مقبض;

مصحح_مرئي_مقبض* sad_visual_debugger_create() {
    auto مقبض = new مصحح_مرئي_مقبض();
    مقبض->مصحح = std::make_unique<مصحح_مرئي>();
    return مقبض;
}

void sad_visual_debugger_destroy(مصحح_مرئي_مقبض* مقبض) {
    delete مقبض;
}

void sad_visual_add_gpio(مصحح_مرئي_مقبض* مقبض, int pin, const char* label, int x, int y) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->إضافة_GPIO(pin, label, x, y);
    }
}

void sad_visual_update_gpio(مصحح_مرئي_مقبض* مقبض, int pin, int output, int state) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->تحديث_GPIO(pin, output != 0, state != 0);
    }
}

void sad_visual_add_led(مصحح_مرئي_مقبض* مقبض, const char* label, int x, int y) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->إضافة_LED(label, x, y);
    }
}

void sad_visual_update_led(مصحح_مرئي_مقبض* مقبض, const char* label, int on, int brightness) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->تحديث_LED(label, on != 0, brightness);
    }
}

void sad_visual_add_servo(مصحض_مرئي_مقبض* مقبض, const char* label, int x, int y) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->إضافة_سيرفو(label, x, y);
    }
}

void sad_visual_update_servo(مصحح_مرئي_مقبض* مقبض, const char* label, double angle) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->تحديث_سيرفو(label, angle);
    }
}

void sad_visual_add_chart_channel(مصحح_مرئي_مقبض* مقبض, const char* name, 
                                   int r, int g, int b, double min, double max) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->إضافة_قناة_رسم(name, لون(r, g, b), min, max);
    }
}

void sad_visual_add_chart_point(مصحح_مرئي_مقبض* مقبض, const char* channel, double value) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->إضافة_نقطة_رسم(channel, value);
    }
}

void sad_visual_serial_rx(مصحح_مرئي_مقبض* مقبض, const char* data) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->المراقب_Serial().إضافة_وارد(data);
    }
}

void sad_visual_serial_tx(مصحح_مرئي_مقبض* مقبض, const char* data) {
    if (مقبض && مقبض->مصحح) {
        مقبض->مصحح->المراقب_Serial().إضافة_صادر(data);
    }
}

const char* sad_visual_generate_html(مصحح_مرئي_مقبض* مقبض) {
    static std::string نتيجة;
    if (مقبض && مقبض->مصحح) {
        نتيجة = مقبض->مصحح->توليد_HTML();
        return نتيجة.c_str();
    }
    return "";
}

const char* sad_visual_generate_json(مصحح_مرئي_مقبض* مقبض) {
    static std::string نتيجة;
    if (مقبض && مقبض->مصحح) {
        نتيجة = مقبض->مصحح->توليد_JSON();
        return نتيجة.c_str();
    }
    return "";
}

}  // extern "C"
