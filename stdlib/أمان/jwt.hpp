/**
 * =============================================================================
 * ملف: jwt.hpp
 * الوصف: معالج JWT (JSON Web Tokens)
 * المهمة: T1053 - Phase 96.2 (متابعة)
 * =============================================================================
 * 
 * 📚 دليل المبتدئ لـ JWT
 * ═══════════════════════
 * 
 * ما هو JWT؟
 * ────────
 * 
 * JWT = تذكرة رقمية مُوقَّعة تثبت هويتك
 * 
 * تخيل تذكرة دخول حفلة:
 * 
 *     ┌─────────────────────────────────────────────────────────────┐
 *     │                     تذكرة دخول                              │
 *     │   ═══════════════════════════════════                      │
 *     │                                                             │
 *     │   الاسم: أحمد محمد                                          │
 *     │   رقم المقعد: VIP-15                                        │
 *     │   تاريخ الحفلة: 2024-01-15                                  │
 *     │   ────────────────────────────────                         │
 *     │   التوقيع: [ختم الشركة المُنظِّمة] ✓                        │
 *     │                                                             │
 *     │   ⚠️ أي تعديل على التذكرة يُبطِل التوقيع                    │
 *     │                                                             │
 *     └─────────────────────────────────────────────────────────────┘
 * 
 * JWT يعمل بنفس الطريقة!
 * 
 * مكونات JWT:
 * ───────────
 * 
 *     JWT = Header.Payload.Signature
 *           │       │        │
 *           │       │        └─→ التوقيع (يثبت أن البيانات صحيحة)
 *           │       └──────────→ البيانات (الاسم، الصلاحيات، إلخ)
 *           └──────────────────→ الرأس (نوع الخوارزمية)
 * 
 *     مثال حقيقي:
 *     eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.
 *     eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkFobWVkIiwiaWF0IjoxNTE2MjM5MDIyfQ.
 *     SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c
 * 
 * كيف يعمل؟
 * ─────────
 * 
 *     1. تسجيل الدخول:
 *        المستخدم ──→ اسم + كلمة مرور ──→ الخادم
 *                                         │
 *                                         ▼
 *                               يُنشئ JWT ويُوقِّعه
 *                                         │
 *        المستخدم ←────────── JWT ←───────┘
 * 
 *     2. كل طلب لاحق:
 *        المستخدم ──→ الطلب + JWT ──→ الخادم
 *                                      │
 *                                      ▼
 *                           يتحقق من التوقيع
 *                                      │
 *                         صحيح؟ ──→ نفِّذ الطلب
 *                         خطأ؟ ──→ ارفض
 * 
 * =============================================================================
 */

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <optional>
#include <sstream>

namespace sad {
namespace أمان {

// ═══════════════════════════════════════════════════════════════════════════════
//                              خوارزميات التوقيع
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * خوارزمية التوقيع
 */
enum class خوارزمية_JWT {
    HS256,    // HMAC-SHA256 (متماثل - مفتاح واحد)
    HS384,    // HMAC-SHA384
    HS512,    // HMAC-SHA512
    RS256,    // RSA-SHA256 (غير متماثل - مفتاح عام/خاص)
    RS384,
    RS512,
    ES256,    // ECDSA-SHA256
    ES384,
    ES512,
    none,     // بدون توقيع (غير آمن!)
};

/**
 * تحويل الخوارزمية إلى نص
 */
inline std::string خوارزمية_إلى_نص(خوارزمية_JWT خ) {
    switch (خ) {
        case خوارزمية_JWT::HS256: return "HS256";
        case خوارزمية_JWT::HS384: return "HS384";
        case خوارزمية_JWT::HS512: return "HS512";
        case خوارزمية_JWT::RS256: return "RS256";
        case خوارزمية_JWT::RS384: return "RS384";
        case خوارزمية_JWT::RS512: return "RS512";
        case خوارزمية_JWT::ES256: return "ES256";
        case خوارزمية_JWT::ES384: return "ES384";
        case خوارزمية_JWT::ES512: return "ES512";
        case خوارزمية_JWT::none: return "none";
    }
    return "HS256";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              حمولة JWT (Claims)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * الحمولة (Claims) في JWT
 * 
 * Claims القياسية:
 * - iss (issuer): مَن أصدر الرمز
 * - sub (subject): مَن يمثل الرمز (ID المستخدم)
 * - aud (audience): لمَن الرمز
 * - exp (expiration): متى ينتهي
 * - nbf (not before): لا يصلح قبل هذا الوقت
 * - iat (issued at): متى أُصدر
 * - jti (JWT ID): معرف فريد
 */
class حمولة_JWT {
public:
    // ────────────────────────────────────────────────────────────────
    //                     Claims قياسية
    // ────────────────────────────────────────────────────────────────
    
    /**
     * المُصدِر (iss)
     */
    حمولة_JWT& المُصدِر(const std::string& قيمة) {
        m_claims["iss"] = قيمة;
        return *this;
    }
    
    /**
     * الموضوع/المستخدم (sub)
     */
    حمولة_JWT& الموضوع(const std::string& قيمة) {
        m_claims["sub"] = قيمة;
        return *this;
    }
    
    /**
     * الجمهور (aud)
     */
    حمولة_JWT& الجمهور(const std::string& قيمة) {
        m_claims["aud"] = قيمة;
        return *this;
    }
    
    /**
     * انتهاء الصلاحية (exp)
     */
    حمولة_JWT& ينتهي_في(std::chrono::seconds من_الآن) {
        auto الآن = std::chrono::system_clock::now();
        auto انتهاء = الآن + من_الآن;
        auto timestamp = std::chrono::system_clock::to_time_t(انتهاء);
        m_claims["exp"] = std::to_string(timestamp);
        return *this;
    }
    
    /**
     * لا يصلح قبل (nbf)
     */
    حمولة_JWT& لا_قبل(std::chrono::system_clock::time_point وقت) {
        auto timestamp = std::chrono::system_clock::to_time_t(وقت);
        m_claims["nbf"] = std::to_string(timestamp);
        return *this;
    }
    
    /**
     * وقت الإصدار (iat) - يُضاف تلقائياً
     */
    حمولة_JWT& وقت_الإصدار_الآن() {
        auto الآن = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(الآن);
        m_claims["iat"] = std::to_string(timestamp);
        return *this;
    }
    
    /**
     * معرف JWT فريد (jti)
     */
    حمولة_JWT& المعرف(const std::string& قيمة) {
        m_claims["jti"] = قيمة;
        return *this;
    }
    
    // ────────────────────────────────────────────────────────────────
    //                     Claims مخصصة
    // ────────────────────────────────────────────────────────────────
    
    /**
     * إضافة claim مخصص
     */
    حمولة_JWT& أضف(const std::string& مفتاح, const std::string& قيمة) {
        m_claims[مفتاح] = قيمة;
        return *this;
    }
    
    /**
     * إضافة claim رقمي
     */
    حمولة_JWT& أضف_رقم(const std::string& مفتاح, int64_t قيمة) {
        m_claims_رقمية[مفتاح] = قيمة;
        return *this;
    }
    
    /**
     * إضافة claim منطقي
     */
    حمولة_JWT& أضف_منطقي(const std::string& مفتاح, bool قيمة) {
        m_claims_منطقية[مفتاح] = قيمة;
        return *this;
    }
    
    /**
     * إضافة قائمة (مثل الأدوار)
     */
    حمولة_JWT& أضف_قائمة(const std::string& مفتاح, const std::vector<std::string>& قيم) {
        m_claims_قوائم[مفتاح] = قيم;
        return *this;
    }
    
    // ────────────────────────────────────────────────────────────────
    //                     قراءة Claims
    // ────────────────────────────────────────────────────────────────
    
    /**
     * قراءة claim نصي
     */
    std::optional<std::string> احصل(const std::string& مفتاح) const {
        auto it = m_claims.find(مفتاح);
        if (it != m_claims.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * قراءة claim رقمي
     */
    std::optional<int64_t> احصل_رقم(const std::string& مفتاح) const {
        auto it = m_claims_رقمية.find(مفتاح);
        if (it != m_claims_رقمية.end()) {
            return it->second;
        }
        
        // حاول قراءته من النصي
        auto it2 = m_claims.find(مفتاح);
        if (it2 != m_claims.end()) {
            try {
                return std::stoll(it2->second);
            } catch (...) {}
        }
        
        return std::nullopt;
    }
    
    /**
     * قراءة قائمة
     */
    std::optional<std::vector<std::string>> احصل_قائمة(const std::string& مفتاح) const {
        auto it = m_claims_قوائم.find(مفتاح);
        if (it != m_claims_قوائم.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // ────────────────────────────────────────────────────────────────
    //                     التحقق
    // ────────────────────────────────────────────────────────────────
    
    /**
     * هل انتهت الصلاحية؟
     */
    bool منتهي_الصلاحية() const {
        auto exp = احصل_رقم("exp");
        if (!exp.has_value()) return false;
        
        auto الآن = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(الآن);
        
        return timestamp > *exp;
    }
    
    /**
     * هل صالح الآن؟ (بين nbf و exp)
     */
    bool صالح_الآن() const {
        auto الآن = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(الآن);
        
        // تحقق من exp
        auto exp = احصل_رقم("exp");
        if (exp.has_value() && timestamp > *exp) {
            return false;
        }
        
        // تحقق من nbf
        auto nbf = احصل_رقم("nbf");
        if (nbf.has_value() && timestamp < *nbf) {
            return false;
        }
        
        return true;
    }
    
    // ────────────────────────────────────────────────────────────────
    //                     التسلسل
    // ────────────────────────────────────────────────────────────────
    
    /**
     * تحويل إلى JSON
     */
    std::string إلى_JSON() const {
        std::stringstream ss;
        ss << "{";
        
        bool أول = true;
        
        // Claims نصية
        for (const auto& [مفتاح, قيمة] : m_claims) {
            if (!أول) ss << ",";
            ss << "\"" << مفتاح << "\":\"" << escape_json(قيمة) << "\"";
            أول = false;
        }
        
        // Claims رقمية
        for (const auto& [مفتاح, قيمة] : m_claims_رقمية) {
            if (!أول) ss << ",";
            ss << "\"" << مفتاح << "\":" << قيمة;
            أول = false;
        }
        
        // Claims منطقية
        for (const auto& [مفتاح, قيمة] : m_claims_منطقية) {
            if (!أول) ss << ",";
            ss << "\"" << مفتاح << "\":" << (قيمة ? "true" : "false");
            أول = false;
        }
        
        // القوائم
        for (const auto& [مفتاح, قيم] : m_claims_قوائم) {
            if (!أول) ss << ",";
            ss << "\"" << مفتاح << "\":[";
            for (size_t i = 0; i < قيم.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << escape_json(قيم[i]) << "\"";
            }
            ss << "]";
            أول = false;
        }
        
        ss << "}";
        return ss.str();
    }
    
private:
    std::unordered_map<std::string, std::string> m_claims;
    std::unordered_map<std::string, int64_t> m_claims_رقمية;
    std::unordered_map<std::string, bool> m_claims_منطقية;
    std::unordered_map<std::string, std::vector<std::string>> m_claims_قوائم;
    
    static std::string escape_json(const std::string& نص) {
        std::string نتيجة;
        for (char c : نص) {
            switch (c) {
                case '"': نتيجة += "\\\""; break;
                case '\\': نتيجة += "\\\\"; break;
                case '\n': نتيجة += "\\n"; break;
                case '\r': نتيجة += "\\r"; break;
                case '\t': نتيجة += "\\t"; break;
                default: نتيجة += c;
            }
        }
        return نتيجة;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              معالج JWT
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * نتيجة التحقق من JWT
 */
struct نتيجة_تحقق_JWT {
    bool صالح = false;
    std::string الخطأ;
    حمولة_JWT الحمولة;
};

/**
 * معالج JWT الرئيسي
 */
class معالج_JWT {
public:
    /**
     * إنشاء معالج بمفتاح سري
     */
    explicit معالج_JWT(
        const std::string& مفتاح_سري,
        خوارزمية_JWT خوارزمية = خوارزمية_JWT::HS256
    ) : m_مفتاح(مفتاح_سري), m_خوارزمية(خوارزمية) {}
    
    /**
     * إنشاء JWT جديد
     * 
     * مثال:
     *     معالج_JWT jwt("my_secret_key");
     *     
     *     auto حمولة = حمولة_JWT()
     *         .الموضوع("user_123")
     *         .ينتهي_في(std::chrono::hours(24))
     *         .أضف_قائمة("أدوار", {"مستخدم", "مشرف"});
     *     
     *     std::string token = jwt.أنشئ(حمولة);
     */
    std::string أنشئ(const حمولة_JWT& حمولة) {
        // الرأس
        std::string header = "{\"alg\":\"" + خوارزمية_إلى_نص(m_خوارزمية) + "\",\"typ\":\"JWT\"}";
        
        // الحمولة
        std::string payload = حمولة.إلى_JSON();
        
        // Base64 URL encode
        std::string header_b64 = base64url_encode(header);
        std::string payload_b64 = base64url_encode(payload);
        
        // التوقيع
        std::string data = header_b64 + "." + payload_b64;
        std::string signature = وقِّع(data);
        std::string signature_b64 = base64url_encode(signature);
        
        return data + "." + signature_b64;
    }
    
    /**
     * التحقق من JWT
     * 
     * مثال:
     *     auto نتيجة = jwt.تحقق(token);
     *     
     *     إذا نتيجة.صالح:
     *         auto user_id = نتيجة.الحمولة.احصل("sub");
     *         auto أدوار = نتيجة.الحمولة.احصل_قائمة("أدوار");
     */
    نتيجة_تحقق_JWT تحقق(const std::string& token) {
        // قسِّم الرمز
        auto أجزاء = قسِّم(token, '.');
        if (أجزاء.size() != 3) {
            return {false, "تنسيق JWT غير صالح", {}};
        }
        
        std::string header_b64 = أجزاء[0];
        std::string payload_b64 = أجزاء[1];
        std::string signature_b64 = أجزاء[2];
        
        // تحقق من التوقيع
        std::string data = header_b64 + "." + payload_b64;
        std::string signature_expected = base64url_encode(وقِّع(data));
        
        if (signature_b64 != signature_expected) {
            return {false, "التوقيع غير صالح", {}};
        }
        
        // فك الحمولة
        std::string payload_json = base64url_decode(payload_b64);
        حمولة_JWT حمولة = حلل_حمولة(payload_json);
        
        // تحقق من الصلاحية الزمنية
        if (!حمولة.صالح_الآن()) {
            if (حمولة.منتهي_الصلاحية()) {
                return {false, "JWT منتهي الصلاحية", حمولة};
            }
            return {false, "JWT لم يصبح صالحاً بعد", حمولة};
        }
        
        return {true, "", حمولة};
    }
    
    /**
     * فك JWT بدون تحقق (للقراءة فقط - غير آمن)
     */
    std::optional<حمولة_JWT> فك_بدون_تحقق(const std::string& token) {
        auto أجزاء = قسِّم(token, '.');
        if (أجزاء.size() != 3) {
            return std::nullopt;
        }
        
        std::string payload_json = base64url_decode(أجزاء[1]);
        return حلل_حمولة(payload_json);
    }
    
private:
    std::string m_مفتاح;
    خوارزمية_JWT m_خوارزمية;
    
    /**
     * توقيع البيانات (تنفيذ مبسط - استخدم مكتبة تشفير حقيقية)
     */
    std::string وقِّع(const std::string& data) {
        // HMAC-SHA256 مبسط (في الحقيقة استخدم OpenSSL أو libsodium)
        std::string نتيجة;
        for (size_t i = 0; i < data.size(); i++) {
            نتيجة += data[i] ^ m_مفتاح[i % m_مفتاح.size()];
        }
        return نتيجة;
    }
    
    /**
     * Base64 URL Encode
     */
    static std::string base64url_encode(const std::string& data) {
        static const char* أحرف = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        
        std::string نتيجة;
        int val = 0, valb = -6;
        
        for (unsigned char c : data) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                نتيجة += أحرف[(val >> valb) & 0x3F];
                valb -= 6;
            }
        }
        
        if (valb > -6) {
            نتيجة += أحرف[((val << 8) >> (valb + 8)) & 0x3F];
        }
        
        // بدون padding في URL-safe
        return نتيجة;
    }
    
    /**
     * Base64 URL Decode
     */
    static std::string base64url_decode(const std::string& data) {
        static const int جدول[] = {
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,
            52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
            -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
            15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,63,
            -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
            41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1
        };
        
        std::string نتيجة;
        int val = 0, valb = -8;
        
        for (unsigned char c : data) {
            if (c >= sizeof(جدول) || جدول[c] == -1) continue;
            val = (val << 6) + جدول[c];
            valb += 6;
            if (valb >= 0) {
                نتيجة += char((val >> valb) & 0xFF);
                valb -= 8;
            }
        }
        
        return نتيجة;
    }
    
    /**
     * تقسيم نص
     */
    static std::vector<std::string> قسِّم(const std::string& نص, char فاصل) {
        std::vector<std::string> أجزاء;
        std::stringstream ss(نص);
        std::string جزء;
        
        while (std::getline(ss, جزء, فاصل)) {
            أجزاء.push_back(جزء);
        }
        
        return أجزاء;
    }
    
    /**
     * تحليل JSON للحمولة (مبسط)
     */
    static حمولة_JWT حلل_حمولة(const std::string& json) {
        حمولة_JWT حمولة;
        // تحليل JSON مبسط - في الحقيقة استخدم مكتبة JSON
        // هذا للتوضيح فقط
        return حمولة;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              ماكرو مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ماكرو للتحقق من JWT في middleware
 */
#define SAD_VERIFY_JWT(token, jwt_handler, on_invalid) \
    do { \
        auto __نتيجة = (jwt_handler).تحقق(token); \
        if (!__نتيجة.صالح) { \
            on_invalid(__نتيجة.الخطأ); \
        } \
    } while(0)

} // namespace أمان
} // namespace sad
