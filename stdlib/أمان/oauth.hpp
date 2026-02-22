/**
 * =============================================================================
 * ملف: oauth.hpp
 * الوصف: تكامل OAuth/SSO لتسجيل الدخول بالحسابات الخارجية
 * المهمة: T1052 - Phase 96.2 (متابعة)
 * =============================================================================
 * 
 * 📚 دليل المبتدئ لـ OAuth و SSO
 * ═══════════════════════════════
 * 
 * ما هو OAuth؟
 * ───────────
 * 
 * تخيل أنك تريد دخول نادي خاص:
 * 
 *     ┌─────────────────────────────────────────────────────────────┐
 *     │   بدون OAuth:                                              │
 *     │   ───────────                                              │
 *     │   أنت ──→ أعطني اسمك وكلمة المرور                          │
 *     │                                                             │
 *     │   مع OAuth:                                                │
 *     │   ─────────                                                │
 *     │   أنت ──→ هل أنت عضو في Google/Facebook/GitHub؟            │
 *     │       │                                                     │
 *     │       └──→ Google يؤكد: "نعم، هذا أحمد"                    │
 *     │                  │                                          │
 *     │                  └──→ تفضل، أهلاً أحمد!                    │
 *     │                                                             │
 *     └─────────────────────────────────────────────────────────────┘
 * 
 * خطوات OAuth:
 * ────────────
 * 
 *     1. المستخدم يضغط "سجِّل دخول بـ Google"
 *                    │
 *                    ▼
 *     2. نُحوِّله لصفحة Google
 *                    │
 *                    ▼
 *     3. المستخدم يُوافق على مشاركة بياناته
 *                    │
 *                    ▼
 *     4. Google يُعيده إلينا مع "رمز تأكيد"
 *                    │
 *                    ▼
 *     5. نستبدل الرمز بـ "رمز وصول" (Access Token)
 *                    │
 *                    ▼
 *     6. نستخدم رمز الوصول لجلب معلومات المستخدم
 * 
 * ما هو SSO؟ (Single Sign-On)
 * ───────────────────────────
 * 
 * تسجيل دخول واحد لكل التطبيقات:
 * 
 *     سجِّل دخول في Google مرة واحدة
 *                    │
 *         ┌─────────┼─────────┐
 *         ▼         ▼         ▼
 *     YouTube   Gmail   Google Drive
 *     (لا تحتاج تسجيل دخول مرة أخرى)
 * 
 * =============================================================================
 */

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>

namespace sad {
namespace أمان {

// ═══════════════════════════════════════════════════════════════════════════════
//                              أنواع OAuth
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مُزوِّد OAuth
 */
enum class مُزوِّد_OAuth {
    Google,
    Facebook,
    GitHub,
    Microsoft,
    Apple,
    Twitter,
    مخصص,
};

/**
 * نطاقات الوصول (Scopes)
 */
enum class نطاق_وصول {
    profile,      // الاسم والصورة
    email,        // البريد الإلكتروني
    openid,       // معرف المستخدم
    calendar,     // التقويم
    contacts,     // جهات الاتصال
};

/**
 * حالة OAuth
 */
enum class حالة_OAuth {
    غير_مبدوء,
    في_انتظار_المصادقة,
    تم_استلام_الرمز,
    تم_الوصول,
    فشل,
    منتهي_الصلاحية,
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              إعدادات OAuth
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إعدادات مُزوِّد OAuth
 */
struct إعدادات_مُزوِّد {
    مُزوِّد_OAuth النوع;
    std::string client_id;
    std::string client_secret;
    std::string redirect_uri;
    std::string authorize_url;
    std::string token_url;
    std::string userinfo_url;
    std::vector<std::string> النطاقات;
};

/**
 * إعدادات Google المُعدَّة مسبقاً
 */
inline إعدادات_مُزوِّد إعدادات_Google(
    const std::string& client_id,
    const std::string& client_secret,
    const std::string& redirect_uri
) {
    return {
        مُزوِّد_OAuth::Google,
        client_id,
        client_secret,
        redirect_uri,
        "https://accounts.google.com/o/oauth2/v2/auth",
        "https://oauth2.googleapis.com/token",
        "https://www.googleapis.com/oauth2/v3/userinfo",
        {"openid", "profile", "email"}
    };
}

/**
 * إعدادات GitHub المُعدَّة مسبقاً
 */
inline إعدادات_مُزوِّد إعدادات_GitHub(
    const std::string& client_id,
    const std::string& client_secret,
    const std::string& redirect_uri
) {
    return {
        مُزوِّد_OAuth::GitHub,
        client_id,
        client_secret,
        redirect_uri,
        "https://github.com/login/oauth/authorize",
        "https://github.com/login/oauth/access_token",
        "https://api.github.com/user",
        {"user:email", "read:user"}
    };
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الرموز (Tokens)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * رموز OAuth
 */
struct رموز_OAuth {
    std::string access_token;          // رمز الوصول
    std::string refresh_token;         // رمز التجديد (اختياري)
    std::string token_type;            // عادة "Bearer"
    int expires_in = 0;                // صلاحية بالثواني
    std::string scope;                 // النطاقات الممنوحة
    
    // توقيتات
    std::chrono::steady_clock::time_point وقت_الحصول;
    
    /**
     * هل انتهت الصلاحية؟
     */
    bool منتهي_الصلاحية() const {
        if (expires_in <= 0) return false;
        
        auto الآن = std::chrono::steady_clock::now();
        auto مضى = std::chrono::duration_cast<std::chrono::seconds>(الآن - وقت_الحصول);
        
        return مضى.count() >= expires_in;
    }
    
    /**
     * الوقت المتبقي بالثواني
     */
    int الوقت_المتبقي() const {
        if (expires_in <= 0) return -1;
        
        auto الآن = std::chrono::steady_clock::now();
        auto مضى = std::chrono::duration_cast<std::chrono::seconds>(الآن - وقت_الحصول);
        
        return std::max(0, expires_in - static_cast<int>(مضى.count()));
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              معلومات المستخدم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * معلومات المستخدم من OAuth
 */
struct معلومات_مستخدم_OAuth {
    std::string المعرف;           // ID من المُزوِّد
    std::string البريد;           // البريد الإلكتروني
    std::string الاسم;            // الاسم الكامل
    std::string الاسم_الأول;
    std::string اسم_العائلة;
    std::string الصورة;           // URL للصورة
    std::string اللغة;            // locale
    bool البريد_مُتحقق = false;
    
    // بيانات إضافية خام
    std::unordered_map<std::string, std::string> بيانات_إضافية;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              عميل OAuth
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * نتيجة عملية OAuth
 */
template<typename T>
struct نتيجة_OAuth {
    bool نجح = false;
    std::optional<T> القيمة;
    std::string الخطأ;
    std::string وصف_الخطأ;
};

/**
 * عميل OAuth
 */
class عميل_OAuth {
public:
    /**
     * إنشاء عميل لمُزوِّد معين
     */
    explicit عميل_OAuth(إعدادات_مُزوِّد إعدادات)
        : m_إعدادات(std::move(إعدادات)),
          m_الحالة(حالة_OAuth::غير_مبدوء) {}
    
    /**
     * إنشاء رابط تسجيل الدخول
     * 
     * مثال:
     *     عميل_OAuth oauth(إعدادات_Google(...));
     *     std::string رابط = oauth.رابط_المصادقة();
     *     // وجِّه المستخدم إلى هذا الرابط
     */
    std::string رابط_المصادقة(const std::string& state = "") {
        std::string رابط = m_إعدادات.authorize_url;
        
        رابط += "?client_id=" + url_encode(m_إعدادات.client_id);
        رابط += "&redirect_uri=" + url_encode(m_إعدادات.redirect_uri);
        رابط += "&response_type=code";
        
        // النطاقات
        std::string نطاقات_نص;
        for (size_t i = 0; i < m_إعدادات.النطاقات.size(); i++) {
            if (i > 0) نطاقات_نص += " ";
            نطاقات_نص += m_إعدادات.النطاقات[i];
        }
        رابط += "&scope=" + url_encode(نطاقات_نص);
        
        // State للحماية من CSRF
        m_state = state.empty() ? ولِّد_state() : state;
        رابط += "&state=" + url_encode(m_state);
        
        m_الحالة = حالة_OAuth::في_انتظار_المصادقة;
        
        return رابط;
    }
    
    /**
     * معالجة Callback بعد المصادقة
     * 
     * مثال:
     *     // عندما يعود المستخدم من Google
     *     auto نتيجة = oauth.عالج_callback(code_from_url, state_from_url);
     *     إذا نتيجة.نجح:
     *         اطبع("مرحباً " + نتيجة.القيمة->الاسم)
     */
    نتيجة_OAuth<معلومات_مستخدم_OAuth> عالج_callback(
        const std::string& code,
        const std::string& state
    ) {
        // تحقق من state
        if (state != m_state) {
            return {false, {}, "invalid_state", "State غير متطابق - محاولة CSRF محتملة"};
        }
        
        // استبدل الرمز بـ Access Token
        auto نتيجة_رموز = استبدل_الرمز(code);
        if (!نتيجة_رموز.نجح) {
            return {false, {}, نتيجة_رموز.الخطأ, نتيجة_رموز.وصف_الخطأ};
        }
        
        m_رموز = *نتيجة_رموز.القيمة;
        m_الحالة = حالة_OAuth::تم_استلام_الرمز;
        
        // اجلب معلومات المستخدم
        return اجلب_معلومات_المستخدم();
    }
    
    /**
     * تجديد رمز الوصول
     */
    نتيجة_OAuth<رموز_OAuth> جدِّد_الرمز() {
        if (m_رموز.refresh_token.empty()) {
            return {false, {}, "no_refresh_token", "لا يوجد رمز تجديد"};
        }
        
        // في التنفيذ الحقيقي: أرسل طلب HTTP
        // POST token_url
        // grant_type=refresh_token
        // refresh_token=...
        
        // هذا تنفيذ وهمي
        return {true, m_رموز, "", ""};
    }
    
    /**
     * الحالة الحالية
     */
    حالة_OAuth الحالة() const { return m_الحالة; }
    
    /**
     * الرموز الحالية
     */
    const رموز_OAuth& الرموز() const { return m_رموز; }
    
private:
    إعدادات_مُزوِّد m_إعدادات;
    حالة_OAuth m_الحالة;
    std::string m_state;
    رموز_OAuth m_رموز;
    
    /**
     * استبدال الرمز بـ Access Token
     */
    نتيجة_OAuth<رموز_OAuth> استبدل_الرمز(const std::string& code) {
        // في التنفيذ الحقيقي: أرسل طلب HTTP
        // POST token_url
        // grant_type=authorization_code
        // code=...
        // client_id=...
        // client_secret=...
        // redirect_uri=...
        
        // هذا تنفيذ وهمي للتوضيح
        رموز_OAuth رموز;
        رموز.access_token = "mock_access_token_" + code;
        رموز.token_type = "Bearer";
        رموز.expires_in = 3600;
        رموز.وقت_الحصول = std::chrono::steady_clock::now();
        
        return {true, رموز, "", ""};
    }
    
    /**
     * جلب معلومات المستخدم
     */
    نتيجة_OAuth<معلومات_مستخدم_OAuth> اجلب_معلومات_المستخدم() {
        // في التنفيذ الحقيقي: أرسل طلب HTTP
        // GET userinfo_url
        // Authorization: Bearer {access_token}
        
        // هذا تنفيذ وهمي
        معلومات_مستخدم_OAuth معلومات;
        معلومات.المعرف = "user_123";
        معلومات.الاسم = "مستخدم تجريبي";
        معلومات.البريد = "test@example.com";
        معلومات.البريد_مُتحقق = true;
        
        m_الحالة = حالة_OAuth::تم_الوصول;
        
        return {true, معلومات, "", ""};
    }
    
    /**
     * توليد state عشوائي
     */
    std::string ولِّد_state() {
        static const char أحرف[] = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(أحرف) - 2);
        
        std::string state;
        for (int i = 0; i < 32; i++) {
            state += أحرف[dis(gen)];
        }
        
        return state;
    }
    
    /**
     * تشفير URL
     */
    static std::string url_encode(const std::string& نص) {
        std::string نتيجة;
        
        for (char c : نص) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                نتيجة += c;
            } else {
                char hex[4];
                snprintf(hex, sizeof(hex), "%%%02X", static_cast<unsigned char>(c));
                نتيجة += hex;
            }
        }
        
        return نتيجة;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مدير SSO
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مدير تسجيل الدخول الموحد
 */
class مدير_SSO {
public:
    /**
     * الحصول على المثال العالمي
     */
    static مدير_SSO& المثال() {
        static مدير_SSO مثال;
        return مثال;
    }
    
    /**
     * تسجيل مُزوِّد OAuth
     */
    void سجِّل_مُزوِّد(const std::string& اسم, إعدادات_مُزوِّد إعدادات) {
        m_المُزوِّدون[اسم] = std::make_shared<عميل_OAuth>(std::move(إعدادات));
    }
    
    /**
     * الحصول على عميل مُزوِّد
     */
    std::shared_ptr<عميل_OAuth> مُزوِّد(const std::string& اسم) {
        auto it = m_المُزوِّدون.find(اسم);
        if (it != m_المُزوِّدون.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    /**
     * قائمة المُزوِّدين المتاحين
     */
    std::vector<std::string> المُزوِّدون_المتاحون() const {
        std::vector<std::string> قائمة;
        for (const auto& [اسم, _] : m_المُزوِّدون) {
            قائمة.push_back(اسم);
        }
        return قائمة;
    }
    
private:
    مدير_SSO() = default;
    std::unordered_map<std::string, std::shared_ptr<عميل_OAuth>> m_المُزوِّدون;
};

} // namespace أمان
} // namespace sad
