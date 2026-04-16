/**
 * @file audio3d.h
 * @brief مكتبة الصوت ثلاثي الأبعاد - stdlib/audio3d
 * @brief (EN) 3D Audio library - stdlib/audio3d
 * 
 * @author كامل
 * @date نوفمبر 2026
 */

#ifndef SAD_STDLIB_AUDIO3D_H
#define SAD_STDLIB_AUDIO3D_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;memory&gt;
#include &lt;cmath&gt;
#include &lt;functional&gt;
#include &lt;map&gt;
#include &lt;cstdint&gt;

namespace Sad {
namespace Stdlib {
namespace Audio3D {

//==============================================================================
// أنواع البيانات الأساسية
//==============================================================================

/**
 * @brief متجه ثلاثي الأبعاد
 */
struct Vec3 {
    float x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3&amp; o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3&amp; o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(float s) const { return {x*s, y*s, z*s}; }
    
    float طول() const { return std::sqrt(x*x + y*y + z*z); }
    Vec3 وحدة() const {
        float len = طول();
        return len &gt; 0 ? Vec3{x/len, y/len, z/len} : Vec3{};
    }
    
    float حاصل_ضرب(const Vec3&amp; o) const { return x*o.x + y*o.y + z*o.z; }
};

/**
 * @brief تنسيق الصوت
 */
enum class AudioFormat {
    WAV,
    MP3,
    OGG,
    FLAC,
    AAC
};

/**
 * @brief حالة التشغيل
 */
enum class PlayState {
    STOPPED,    // متوقف
    PLAYING,    // يعمل
    PAUSED,     // مؤقت
    FADING_IN,  // يظهر تدريجياً
    FADING_OUT  // يختفي تدريجياً
};

/**
 * @brief نموذج التوهين
 */
enum class AttenuationModel {
    NONE,               // بدون توهين
    INVERSE_DISTANCE,   // عكس المسافة
    LINEAR,             // خطي
    EXPONENTIAL         // أسي
};

//==============================================================================
// مصدر الصوت
//==============================================================================

/**
 * @brief مصدر صوت في الفضاء
 */
class مصدر_صوت {
public:
    مصدر_صوت(const std::string&amp; ملف);
    ~مصدر_صوت();
    
    // التحكم في التشغيل
    void شغّل(bool تكرار = false);
    void أوقف();
    void ألغِ();
    
    PlayState الحالة() const { return state_; }
    
    // الموقع والسرعة
    void عيّن_الموقع(const Vec3&amp; موقع);
    void عيّن_السرعة(const Vec3&amp; سرعة);
    Vec3 الموقع() const { return position_; }
    Vec3 السرعة() const { return velocity_; }
    
    // خصائص الصوت
    void عيّن_الصوت(float مستوى); // 0.0 - 1.0
    void عيّن_الطبقة(float طبقة); // 0.5 - 2.0
    float الصوت() const { return volume_; }
    float الطبقة() const { return pitch_; }
    
    // التوهين
    void عيّن_نموذج_التوهين(AttenuationModel نموذج);
    void عيّن_المسافة_الدنيا(float مسافة);
    void عيّن_المسافة_القصوى(float مسافة);
    
    // التأثيرات
    void عيّن_تأخير(float ثواني, float مزج = 0.3f);
    void عيّن_صدى(float تأخير, float توهين);
    void عيّن_مرشح_منخفض(float تردد);
    void عيّن_مرشح_عالي(float تردد);
    
    // التلاشي
    void تلاشي_دخول(float ثواني);
    void تلاشي_خروج(float ثواني);
    
    // معلومات
    float المدة() const { return duration_; }
    float الموقع_الحالي() const { return currentPosition_; }
    void اذهب_إلى(float ثواني);

private:
    std::string filePath_;
    PlayState state_ = PlayState::STOPPED;
    
    Vec3 position_;
    Vec3 velocity_;
    float volume_ = 1.0f;
    float pitch_ = 1.0f;
    
    AttenuationModel attenuationModel_ = AttenuationModel::INVERSE_DISTANCE;
    float minDistance_ = 1.0f;
    float maxDistance_ = 100.0f;
    
    float duration_ = 0.0f;
    float currentPosition_ = 0.0f;
    bool looping_ = false;
    
    // مقابض داخلية
    uint32_t sourceHandle_ = 0;
    uint32_t bufferHandle_ = 0;
};

//==============================================================================
// المستمع
//==============================================================================

/**
 * @brief المستمع في الفضاء
 */
class المستمع {
public:
    static المستمع&amp; المثيل() {
        static المستمع instance;
        return instance;
    }
    
    // الموقع والاتجاه
    void عيّن_الموقع(const Vec3&amp; موقع);
    void عيّن_الاتجاه(const Vec3&amp; أمام, const Vec3&amp; أعلى);
    void عيّن_السرعة(const Vec3&amp; سرعة);
    
    Vec3 الموقع() const { return position_; }
    Vec3 الأمام() const { return forward_; }
    Vec3 الأعلى() const { return up_; }
    
    // الصوت الرئيسي
    void عيّن_الصوت_الرئيسي(float مستوى);
    float الصوت_الرئيسي() const { return masterVolume_; }
    
    // تأثير دوبلر
    void عيّن_عامل_دوبلر(float عامل);
    
    // سرعة الصوت
    void عيّن_سرعة_الصوت(float سرعة); // متر/ثانية

private:
    المستمع() = default;
    
    Vec3 position_;
    Vec3 forward_{0, 0, -1};
    Vec3 up_{0, 1, 0};
    Vec3 velocity_;
    
    float masterVolume_ = 1.0f;
    float dopplerFactor_ = 1.0f;
    float speedOfSound_ = 343.3f;
};

//==============================================================================
// بيئة الصوت
//==============================================================================

/**
 * @brief إعدادات الصدى المسبقة
 */
enum class ReverbPreset {
    NONE,           // بدون صدى
    ROOM,           // غرفة صغيرة
    HALL,           // قاعة
    CAVE,           // كهف
    ARENA,          // ساحة
    UNDERWATER,     // تحت الماء
    FOREST,         // غابة
    CITY            // مدينة
};

/**
 * @brief بيئة الصوت ثلاثي الأبعاد
 */
class بيئة_صوت {
public:
    static بيئة_صوت&amp; المثيل() {
        static بيئة_صوت instance;
        return instance;
    }
    
    // الصدى
    void عيّن_الصدى(ReverbPreset إعداد);
    void عيّن_الصدى_مخصص(float تأخير, float توهين, float رطوبة);
    
    // المنطقة
    void أضف_منطقة(const Vec3&amp; مركز, float نصف_قطر, ReverbPreset صدى);
    void أزل_المناطق();
    
    // الحواجز
    void أضف_حاجز(const Vec3&amp; بداية, const Vec3&amp; نهاية, float توهين);

private:
    بيئة_صوت() = default;
};

//==============================================================================
// مدير الصوت
//==============================================================================

/**
 * @brief مدير الصوت الرئيسي
 */
class مدير_الصوت {
public:
    static مدير_الصوت&amp; المثيل() {
        static مدير_الصوت instance;
        return instance;
    }
    
    /**
     * @brief تهيئة النظام
     */
    bool هيّئ();
    
    /**
     * @brief إنهاء النظام
     */
    void أنهِ();
    
    /**
     * @brief تحديث الإطار
     */
    void حدّث(float deltaTime);
    
    // إدارة المصادر
    
    /**
     * @brief تحميل صوت مسبقاً
     */
    bool حمّل_مسبقاً(const std::string&amp; مسار, const std::string&amp; معرف);
    
    /**
     * @brief إنشاء مصدر
     */
    std::shared_ptr&lt;مصدر_صوت&gt; أنشئ_مصدر(const std::string&amp; معرف);
    
    /**
     * @brief تشغيل صوت سريع
     */
    void شغّل(const std::string&amp; معرف, const Vec3&amp; موقع = Vec3(), float صوت = 1.0f);
    
    /**
     * @brief إيقاف كل الأصوات
     */
    void أوقف_الكل();
    
    // الموسيقى
    
    /**
     * @brief تشغيل موسيقى خلفية
     */
    void شغّل_موسيقى(const std::string&amp; مسار, float تلاشي = 0.0f);
    
    /**
     * @brief إيقاف الموسيقى
     */
    void أوقف_الموسيقى(float تلاشي = 0.0f);
    
    /**
     * @brief صوت الموسيقى
     */
    void عيّن_صوت_الموسيقى(float مستوى);
    
    // المجموعات
    
    /**
     * @brief إنشاء مجموعة صوتية
     */
    void أنشئ_مجموعة(const std::string&amp; اسم);
    
    /**
     * @brief صوت المجموعة
     */
    void عيّن_صوت_مجموعة(const std::string&amp; اسم, float مستوى);
    
    /**
     * @brief كتم مجموعة
     */
    void اكتم_مجموعة(const std::string&amp; اسم, bool كتم);
    
    // الإحصائيات
    
    int عدد_المصادر_النشطة() const { return activeSources_; }
    float استخدام_الذاكرة() const { return memoryUsage_; }

private:
    مدير_الصوت() = default;
    
    bool initialized_ = false;
    int activeSources_ = 0;
    float memoryUsage_ = 0;
    
    std::map&lt;std::string, uint32_t&gt; loadedBuffers_;
    std::vector&lt;std::shared_ptr&lt;مصدر_صوت&gt;&gt; sources_;
    
    std::shared_ptr&lt;مصدر_صوت&gt; musicSource_;
    std::map&lt;std::string, std::vector&lt;std::shared_ptr&lt;مصدر_صوت&gt;&gt;&gt; groups_;
};

//==============================================================================
// أدوات الصوت
//==============================================================================

/**
 * @brief محول تنسيقات الصوت
 */
class محول_صوت {
public:
    /**
     * @brief تحويل ملف صوتي
     */
    static bool حوّل(const std::string&amp; مصدر, const std::string&amp; هدف, AudioFormat تنسيق);
    
    /**
     * @brief استخراج معلومات الصوت
     */
    struct معلومات_صوت {
        float المدة;
        int معدل_العينات;
        int عدد_القنوات;
        int عمق_البت;
        AudioFormat التنسيق;
    };
    
    static معلومات_صوت معلومات(const std::string&amp; مسار);
};

/**
 * @brief مولد الأصوات
 */
class مولد_صوت {
public:
    /**
     * @brief توليد موجة جيبية
     */
    static std::vector&lt;float&gt; موجة_جيبية(float تردد, float مدة, int معدل_عينات = 44100);
    
    /**
     * @brief توليد موجة مربعة
     */
    static std::vector&lt;float&gt; موجة_مربعة(float تردد, float مدة, int معدل_عينات = 44100);
    
    /**
     * @brief توليد ضوضاء بيضاء
     */
    static std::vector&lt;float&gt; ضوضاء_بيضاء(float مدة, int معدل_عينات = 44100);
    
    /**
     * @brief توليد ضوضاء وردية
     */
    static std::vector&lt;float&gt; ضوضاء_وردية(float مدة, int معدل_عينات = 44100);
};

//==============================================================================
// واجهة Sad النصية
//==============================================================================

/**
 * @brief تسجيل دوال الصوت
 */
void registerAudio3DFunctions();

/**
 * @brief أمثلة الاستخدام بلغة ص
 * 
 * @code
 * استورد "audio3d"
 * 
 * # تهيئة
 * audio3d.هيّئ()
 * 
 * # تحميل الأصوات
 * audio3d.حمّل_مسبقاً("sounds/explosion.wav"، "انفجار")
 * audio3d.حمّل_مسبقاً("sounds/footstep.wav"، "خطوة")
 * 
 * # المستمع (الكاميرا)
 * متغير مستمع = audio3d.المستمع()
 * مستمع.عيّن_الموقع(audio3d.متجه(0، 1.7، 0))
 * 
 * # تشغيل صوت ثلاثي الأبعاد
 * متغير انفجار = audio3d.أنشئ_مصدر("انفجار")
 * انفجار.عيّن_الموقع(audio3d.متجه(10، 0، -5))
 * انفجار.شغّل()
 * 
 * # موسيقى خلفية
 * audio3d.شغّل_موسيقى("music/ambient.mp3"، 2.0)
 * 
 * # في حلقة اللعبة
 * بينما (صحيح)
 *     audio3d.حدّث(0.016)
 * نهاية
 * @endcode
 */

} // namespace Audio3D
} // namespace Stdlib
} // namespace Sad

#endif // SAD_STDLIB_AUDIO3D_H
