/**
 * @file image_processing.h
 * @brief مكتبة معالجة الصور - stdlib/image
 * @brief (EN) Image processing library - stdlib/image
 * 
 * @author كامل
 * @date نوفمبر 2026
 */

#ifndef SAD_STDLIB_IMAGE_H
#define SAD_STDLIB_IMAGE_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;cstdint&gt;
#include &lt;memory&gt;
#include &lt;functional&gt;
#include &lt;cmath&gt;
#include &lt;algorithm&gt;

namespace Sad {
namespace Stdlib {
namespace Image {

//==============================================================================
// أنواع البيانات الأساسية
//==============================================================================

/**
 * @brief لون RGBA
 */
struct Color {
    uint8_t r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    // ألوان مسماة
    static Color أحمر() { return Color(255, 0, 0); }
    static Color أخضر() { return Color(0, 255, 0); }
    static Color أزرق() { return Color(0, 0, 255); }
    static Color أبيض() { return Color(255, 255, 255); }
    static Color أسود() { return Color(0, 0, 0); }
    static Color شفاف() { return Color(0, 0, 0, 0); }
    
    // تحويلات
    static Color منHex(const std::string&amp; hex);
    std::string إلىHex() const;
    
    // دمج ألوان
    Color دمج(const Color&amp; other, float alpha) const;
    
    // سطوع
    uint8_t سطوع() const {
        return static_cast&lt;uint8_t&gt;(0.299 * r + 0.587 * g + 0.114 * b);
    }
};

/**
 * @brief تنسيق الصورة
 */
enum class ImageFormat {
    PNG,
    JPEG,
    BMP,
    GIF,
    WEBP,
    TIFF
};

/**
 * @brief وضع الالتفاف
 */
enum class WrapMode {
    CLAMP,      // تثبيت عند الحافة
    REPEAT,     // تكرار
    MIRROR,     // انعكاس
    TRANSPARENT // شفاف
};

//==============================================================================
// صنف الصورة
//==============================================================================

/**
 * @brief صورة نقطية
 */
class صورة {
public:
    /**
     * @brief إنشاء صورة فارغة
     */
    صورة(int عرض, int ارتفاع);
    
    /**
     * @brief تحميل صورة من ملف
     */
    static std::unique_ptr&lt;صورة&gt; حمّل(const std::string&amp; مسار);
    
    /**
     * @brief حفظ الصورة
     */
    bool احفظ(const std::string&amp; مسار, ImageFormat تنسيق = ImageFormat::PNG);
    
    // الأبعاد
    int العرض() const { return width_; }
    int الارتفاع() const { return height_; }
    
    // الوصول للبكسل
    Color اقرأ_بكسل(int س, int ص) const;
    void اكتب_بكسل(int س, int ص, const Color&amp; لون);
    
    // العمليات الأساسية
    
    /**
     * @brief تغيير الحجم
     */
    std::unique_ptr&lt;صورة&gt; غيّر_الحجم(int عرض_جديد, int ارتفاع_جديد);
    
    /**
     * @brief قص جزء من الصورة
     */
    std::unique_ptr&lt;صورة&gt; قص(int س, int ص, int عرض, int ارتفاع);
    
    /**
     * @brief دوران بزاوية
     */
    std::unique_ptr&lt;صورة&gt; دوّر(double زاوية);
    
    /**
     * @brief انعكاس أفقي
     */
    void اعكس_أفقياً();
    
    /**
     * @brief انعكاس عمودي
     */
    void اعكس_عمودياً();
    
    // المرشحات (Filters)
    
    /**
     * @brief تحويل للرمادي
     */
    void إلى_رمادي();
    
    /**
     * @brief انعكاس الألوان
     */
    void اعكس_الألوان();
    
    /**
     * @brief تعديل السطوع
     */
    void عدّل_السطوع(float نسبة);
    
    /**
     * @brief تعديل التباين
     */
    void عدّل_التباين(float نسبة);
    
    /**
     * @brief تعديل التشبع
     */
    void عدّل_التشبع(float نسبة);
    
    /**
     * @brief ضبابية غاوسية
     */
    void ضبابية(float نصف_قطر);
    
    /**
     * @brief حدة
     */
    void حدّد();
    
    /**
     * @brief كشف الحواف
     */
    void اكشف_الحواف();
    
    /**
     * @brief نقش (Emboss)
     */
    void نقش();
    
    /**
     * @brief تأثير سيبيا
     */
    void سيبيا();
    
    /**
     * @brief تأثير بكسل (Pixelate)
     */
    void بكسل(int حجم_الكتلة);
    
    /**
     * @brief تأثير فينييت
     */
    void فينييت(float قوة = 0.5f);
    
    // التراكب والدمج
    
    /**
     * @brief رسم صورة فوق أخرى
     */
    void ارسم(const صورة&amp; أخرى, int س, int ص, float شفافية = 1.0f);
    
    /**
     * @brief دمج صورتين
     */
    static std::unique_ptr&lt;صورة&gt; ادمج(const صورة&amp; أ, const صورة&amp; ب, float نسبة);
    
    // الرسم
    
    /**
     * @brief رسم خط
     */
    void ارسم_خط(int س1, int ص1, int س2, int ص2, const Color&amp; لون, int سمك = 1);
    
    /**
     * @brief رسم مستطيل
     */
    void ارسم_مستطيل(int س, int ص, int عرض, int ارتفاع, const Color&amp; لون, bool ملون = false);
    
    /**
     * @brief رسم دائرة
     */
    void ارسم_دائرة(int مركز_س, int مركز_ص, int نصف_قطر, const Color&amp; لون, bool ملونة = false);
    
    /**
     * @brief ملء بلون
     */
    void املأ(const Color&amp; لون);
    
    // الهستوغرام والتحليل
    
    /**
     * @brief حساب الهستوغرام
     */
    std::vector&lt;int&gt; هستوغرام(int قناة = -1) const; // -1 = سطوع
    
    /**
     * @brief متوسط اللون
     */
    Color متوسط_اللون() const;
    
    /**
     * @brief اللون السائد
     */
    Color اللون_السائد() const;

private:
    int width_;
    int height_;
    std::vector&lt;Color&gt; pixels_;
    
    // دوال مساعدة
    void applyKernel(const std::vector&lt;float&gt;&amp; kernel, int size);
    Color sampleBilinear(float x, float y) const;
};

//==============================================================================
// معالجة دفعية
//==============================================================================

/**
 * @brief معالجة مجموعة صور
 */
class معالج_دفعي {
public:
    using عملية = std::function&lt;void(صورة&amp;)&gt;;
    
    /**
     * @brief إضافة عملية
     */
    void أضف_عملية(عملية ع) {
        operations_.push_back(ع);
    }
    
    /**
     * @brief معالجة مجلد كامل
     */
    int عالج_مجلد(const std::string&amp; مجلد_الإدخال,
                   const std::string&amp; مجلد_الإخراج,
                   ImageFormat تنسيق = ImageFormat::PNG);
    
    /**
     * @brief معالجة قائمة ملفات
     */
    int عالج_ملفات(const std::vector&lt;std::string&gt;&amp; ملفات,
                    const std::string&amp; مجلد_الإخراج);

private:
    std::vector&lt;عملية&gt; operations_;
};

//==============================================================================
// إنشاء صور
//==============================================================================

/**
 * @brief مولد صور
 */
class مولد_صور {
public:
    /**
     * @brief إنشاء صورة بلون موحد
     */
    static std::unique_ptr&lt;صورة&gt; لون_موحد(int عرض, int ارتفاع, const Color&amp; لون);
    
    /**
     * @brief إنشاء تدرج أفقي
     */
    static std::unique_ptr&lt;صورة&gt; تدرج_أفقي(int عرض, int ارتفاع, 
                                              const Color&amp; بداية, const Color&amp; نهاية);
    
    /**
     * @brief إنشاء تدرج شعاعي
     */
    static std::unique_ptr&lt;صورة&gt; تدرج_شعاعي(int عرض, int ارتفاع,
                                               const Color&amp; مركز, const Color&amp; حافة);
    
    /**
     * @brief إنشاء نمط شطرنج
     */
    static std::unique_ptr&lt;صورة&gt; شطرنج(int عرض, int ارتفاع, int حجم_المربع,
                                          const Color&amp; لون1, const Color&amp; لون2);
    
    /**
     * @brief إنشاء ضوضاء
     */
    static std::unique_ptr&lt;صورة&gt; ضوضاء(int عرض, int ارتفاع, float كثافة = 0.5f);
    
    /**
     * @brief إنشاء ضوضاء Perlin
     */
    static std::unique_ptr&lt;صورة&gt; ضوضاء_بيرلن(int عرض, int ارتفاع, float مقياس = 1.0f);
};

//==============================================================================
// تحويل التنسيقات
//==============================================================================

/**
 * @brief محول تنسيقات
 */
class محول {
public:
    /**
     * @brief تحويل ملف
     */
    static bool حوّل(const std::string&amp; مصدر, const std::string&amp; هدف, ImageFormat تنسيق);
    
    /**
     * @brief تحويل من بيانات خام
     */
    static std::unique_ptr&lt;صورة&gt; من_بيانات(const uint8_t* بيانات, size_t حجم);
    
    /**
     * @brief تصدير لبيانات خام
     */
    static std::vector&lt;uint8_t&gt; إلى_بيانات(const صورة&amp; صورة, ImageFormat تنسيق);
};

//==============================================================================
// واجهة Sad النصية
//==============================================================================

/**
 * @brief تسجيل الدوال في المفسر
 */
void registerImageFunctions();

/**
 * @brief أمثلة الاستخدام بلغة ص
 * 
 * @code
 * استورد "image"
 * 
 * # تحميل صورة
 * متغير ص = image.حمّل("photo.jpg")
 * 
 * # تغيير الحجم
 * متغير صغيرة = ص.غيّر_الحجم(800، 600)
 * 
 * # تطبيق مرشح
 * صغيرة.إلى_رمادي()
 * صغيرة.عدّل_التباين(1.2)
 * 
 * # حفظ
 * صغيرة.احفظ("output.png")
 * 
 * # إنشاء صورة جديدة
 * متغير جديدة = image.أنشئ(400، 300)
 * جديدة.ارسم_دائرة(200، 150، 100، image.لون.أحمر، صحيح)
 * جديدة.احفظ("circle.png")
 * @endcode
 */

} // namespace Image
} // namespace Stdlib
} // namespace Sad

#endif // SAD_STDLIB_IMAGE_H
