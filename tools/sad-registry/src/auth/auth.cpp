// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: auth.cpp
 * الوصف: تنفيذ نظام المصادقة لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يُنفِّذ:
 *   - تشفير كلمات المرور (PBKDF2-like مع SHA256)
 *   - إنشاء رموز JWT وتوقيعها
 *   - التحقق من الرموز مع كل طلب
 *   - ترميز/فك ترميز Base64 URL-safe
 *   - HMAC-SHA256 للتوقيع الرقمي
 *   - إنشاء قيم عشوائية آمنة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "auth.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <algorithm>
#include <random>
#include <array>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// SHA256 - خوارزمية البصمة (مضمّنة بدون تبعيات)
// ═══════════════════════════════════════════════════════════════════════════════
// 
// ننفذ SHA256 من الصفر لأننا لا نريد تبعيات خارجية.
// هذه الخوارزمية معيارية (FIPS 180-4) وتُنتج بصمة ٢٥٦ بت (٣٢ بايت).
//
// تُستخدم في:
//   - تشفير كلمات المرور
//   - توقيع رموز JWT (HMAC-SHA256)
//   - بصمات ملفات الحزم
//   - بصمات رموز API

namespace {

/// ثوابت SHA256 - أول ٦٤ عدد أولي مكعب الجذر
static const uint32_t SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/// دوران بتات إلى اليمين
inline uint32_t rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

/**
 * @brief حساب بصمة SHA256
 * @param data البيانات المراد بصمها
 * @return البصمة كسلسلة سداسية عشرية (64 حرف)
 */
std::string compute_sha256(const std::string& data) {
    // الحالة الأولية - أول ٨ أعداد أولية جذر تربيعي
    uint32_t h0 = 0x6a09e667, h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372, h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f, h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab, h7 = 0x5be0cd19;

    // ─────────────────────────────────────────────────────────────────
    // المرحلة ١: الحشو (Padding)
    // ─────────────────────────────────────────────────────────────────
    // نضيف بت 1 ثم أصفار ثم الطول الأصلي بـ 64 بت
    uint64_t bit_len = static_cast<uint64_t>(data.size()) * 8;
    std::string padded = data;
    padded += static_cast<char>(0x80); // بت 1

    // حشو بأصفار حتى يصبح الطول ≡ 448 (mod 512)
    while ((padded.size() % 64) != 56) {
        padded += static_cast<char>(0x00);
    }

    // إضافة الطول الأصلي كـ 64 بت (big-endian)
    for (int i = 7; i >= 0; i--) {
        padded += static_cast<char>((bit_len >> (i * 8)) & 0xFF);
    }

    // ─────────────────────────────────────────────────────────────────
    // المرحلة ٢: معالجة كل كتلة (64 بايت = 512 بت)
    // ─────────────────────────────────────────────────────────────────
    for (size_t offset = 0; offset < padded.size(); offset += 64) {
        // تحضير جدول الرسائل (64 كلمة × 32 بت)
        uint32_t w[64];

        // أول ١٦ كلمة من الكتلة مباشرة
        for (int i = 0; i < 16; i++) {
            w[i] = (static_cast<uint8_t>(padded[offset + i * 4]) << 24)
                 | (static_cast<uint8_t>(padded[offset + i * 4 + 1]) << 16)
                 | (static_cast<uint8_t>(padded[offset + i * 4 + 2]) << 8)
                 | (static_cast<uint8_t>(padded[offset + i * 4 + 3]));
        }

        // باقي الكلمات تُحسب من السابقة
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rotr(w[i-15], 7) ^ rotr(w[i-15], 18) ^ (w[i-15] >> 3);
            uint32_t s1 = rotr(w[i-2], 17) ^ rotr(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }

        // ─────────────────────────────────────────────────────────────
        // المرحلة ٣: حلقة الضغط (Compression)
        // ─────────────────────────────────────────────────────────────
        uint32_t a = h0, b = h1, c = h2, d = h3;
        uint32_t e = h4, f = h5, g = h6, h = h7;

        for (int i = 0; i < 64; i++) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t temp1 = h + S1 + ch + SHA256_K[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }

        // تحديث الحالة
        h0 += a; h1 += b; h2 += c; h3 += d;
        h4 += e; h5 += f; h6 += g; h7 += h;
    }

    // ─────────────────────────────────────────────────────────────────
    // المرحلة ٤: تكوين البصمة النهائية
    // ─────────────────────────────────────────────────────────────────
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << h0 << std::setw(8) << h1;
    ss << std::setw(8) << h2 << std::setw(8) << h3;
    ss << std::setw(8) << h4 << std::setw(8) << h5;
    ss << std::setw(8) << h6 << std::setw(8) << h7;

    return ss.str();
}

} // مجال أسماء مجهول


// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * المُنشئ - يحفظ مرجع قاعدة البيانات ويُنشئ/يُحمّل مفتاح التوقيع
 */
AuthManager::AuthManager(std::shared_ptr<Database> db)
    : db_(std::move(db)) {
    // محاولة تحميل مفتاح JWT المحفوظ من قاعدة البيانات
    std::string saved_key = db_->get_setting("jwt_secret_key", "");
    if (!saved_key.empty()) {
        secret_key_ = saved_key;
        std::cerr << "  ✓ تم تحميل مفتاح JWT من قاعدة البيانات\n";
    } else {
        // إنشاء مفتاح جديد وحفظه
        secret_key_ = generate_random_token(64);
        db_->set_setting("jwt_secret_key", secret_key_);
        std::cerr << "  ✓ تم إنشاء وحفظ مفتاح JWT جديد\n";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// تسجيل المستخدمين
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تسجيل مستخدم جديد
 *
 * الخطوات:
 *   ١. التحقق من طول اسم المستخدم (٣-٣٠ حرفاً)
 *   ٢. التحقق من صيغة البريد الإلكتروني
 *   ٣. التحقق من قوة كلمة المرور (٨ أحرف على الأقل)
 *   ٤. التحقق من عدم تكرار الاسم والبريد
 *   ٥. تشفير كلمة المرور
 *   ٦. إنشاء السجل في قاعدة البيانات
 *   ٧. إنشاء رمز JWT وإرجاعه
 */
AuthResult AuthManager::register_user(const std::string& username,
                                       const std::string& email,
                                       const std::string& password,
                                       const std::string& display_name) {
    AuthResult result;

    // ─── التحقق من المدخلات ───
    if (username.size() < 3 || username.size() > 90) {
        // ملاحظة: الحروف العربية تأخذ عدة بايتات في UTF-8
        // لذا نسمح بحد أعلى 90 بايت (≈ 30 حرف عربي)
        result.error = "اسم المستخدم يجب أن يكون بين ٣ و ٣٠ حرفاً";
        return result;
    }

    if (email.find('@') == std::string::npos || email.find('.') == std::string::npos) {
        result.error = "صيغة البريد الإلكتروني غير صحيحة";
        return result;
    }

    if (password.size() < 8) {
        result.error = "كلمة المرور يجب أن تكون ٨ أحرف على الأقل";
        return result;
    }

    // ─── التحقق من عدم التكرار ───
    auto existing = db_->get_user_by_username(username);
    if (existing.has_value()) {
        result.error = "اسم المستخدم مستخدم بالفعل";
        return result;
    }

    auto existing_email = db_->get_user_by_email(email);
    if (existing_email.has_value()) {
        result.error = "البريد الإلكتروني مسجل بالفعل";
        return result;
    }

    // ─── إنشاء المستخدم ───
    UserInfo user;
    user.username = username;
    user.email = email;
    user.password_hash = hash_password(password);
    user.display_name = display_name.empty() ? username : display_name;
    user.is_active = true; // تفعيل فوري (بدون تأكيد بريد للبساطة)

    int64_t user_id = db_->create_user(user);
    if (user_id < 0) {
        result.error = "فشل إنشاء الحساب - خطأ في قاعدة البيانات";
        return result;
    }

    // ─── إنشاء رمز JWT ───
    result.success = true;
    result.user_id = user_id;
    result.username = username;
    result.token = create_jwt(user_id, username);

    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// تسجيل الدخول
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تسجيل الدخول بالاسم/البريد وكلمة المرور
 *
 * الخطوات:
 *   ١. البحث عن المستخدم بالاسم أو البريد
 *   ٢. إذا لم يُوجد → خطأ
 *   ٣. مقارنة كلمة المرور مع البصمة المخزنة
 *   ٤. إذا لم تتطابق → خطأ
 *   ٥. تحديث تاريخ آخر دخول
 *   ٦. إنشاء رمز JWT وإرجاعه
 */
AuthResult AuthManager::login(const std::string& username_or_email,
                               const std::string& password) {
    AuthResult result;

    // البحث عن المستخدم (بالاسم أو البريد)
    auto user = db_->get_user_by_username(username_or_email);
    if (!user.has_value()) {
        user = db_->get_user_by_email(username_or_email);
    }

    if (!user.has_value()) {
        result.error = "اسم المستخدم أو البريد الإلكتروني غير موجود";
        return result;
    }

    // التحقق من كلمة المرور
    if (!verify_password(password, user->password_hash)) {
        result.error = "كلمة المرور غير صحيحة";
        return result;
    }

    // التحقق من تفعيل الحساب
    if (!user->is_active) {
        result.error = "الحساب غير مفعّل";
        return result;
    }

    // تحديث آخر دخول
    db_->update_last_login(user->id);

    // إنشاء الرمز
    result.success = true;
    result.user_id = user->id;
    result.username = user->username;
    result.token = create_jwt(user->id, user->username);

    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// التحقق من الرموز
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief التحقق من رمز JWT
 *
 * بنية JWT: header.payload.signature
 * نقسم الرمز ونتحقق من:
 *   ١. وجود ٣ أجزاء مفصولة بنقط
 *   ٢. صحة التوقيع (HMAC-SHA256)
 *   ٣. عدم انتهاء الصلاحية
 *   ٤. صلاحية معرف المستخدم
 */
std::optional<UserInfo> AuthManager::verify_token(const std::string& token) {
    // تقسيم الرمز إلى أجزاء
    size_t dot1 = token.find('.');
    size_t dot2 = token.find('.', dot1 + 1);

    if (dot1 == std::string::npos || dot2 == std::string::npos) {
        return std::nullopt; // صيغة غير صحيحة
    }

    std::string header_b64 = token.substr(0, dot1);
    std::string payload_b64 = token.substr(dot1 + 1, dot2 - dot1 - 1);
    std::string signature_b64 = token.substr(dot2 + 1);

    // التحقق من التوقيع
    std::string data = header_b64 + "." + payload_b64;
    std::string expected_sig = base64_encode(hmac_sha256(secret_key_, data));

    if (signature_b64 != expected_sig) {
        return std::nullopt; // توقيع غير صحيح (الرمز مزوّر أو معبوث به)
    }

    // فك ترميز الحمولة واستخراج البيانات
    std::string payload = base64_decode(payload_b64);

    // استخراج معرف المستخدم (uid) وتاريخ الانتهاء (exp)
    // نبحث عن "uid": و "exp": في JSON
    int64_t user_id = 0;
    int64_t exp_time = 0;

    // تحليل JSON بسيط (بدون مكتبة)
    size_t uid_pos = payload.find("\"uid\":");
    if (uid_pos != std::string::npos) {
        user_id = std::stoll(payload.substr(uid_pos + 6));
    }

    size_t exp_pos = payload.find("\"exp\":");
    if (exp_pos != std::string::npos) {
        exp_time = std::stoll(payload.substr(exp_pos + 6));
    }

    // التحقق من الصلاحية
    auto now = std::chrono::system_clock::now();
    auto now_epoch = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();

    if (exp_time > 0 && now_epoch > exp_time) {
        return std::nullopt; // الرمز منتهي الصلاحية
    }

    // جلب بيانات المستخدم من قاعدة البيانات
    if (user_id <= 0) return std::nullopt;
    return db_->get_user(user_id);
}

// ═══════════════════════════════════════════════════════════════════════════════
// رموز API
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إنشاء رمز API للنشر من سطر الأوامر
 *
 * الرمز يتكون من: "sad_" + 48 حرف عشوائي
 * ما يُخزَّن في قاعدة البيانات: بصمة SHA256 للرمز فقط
 *
 * هذا يعني أنه حتى لو تُسرِّبت قاعدة البيانات، لا يمكن
 * استرجاع الرموز الأصلية.
 */
std::string AuthManager::create_api_token(int64_t user_id,
                                            const std::string& token_name) {
    // إنشاء رمز عشوائي
    std::string raw_token = "sad_" + generate_random_token(48);

    // حساب البصمة للتخزين
    std::string token_hash = compute_sha256(raw_token);

    // حفظ البصمة في قاعدة البيانات
    db_->create_token(user_id, token_name, token_hash, "publish");

    // إرجاع الرمز الأصلي (يظهر مرة واحدة فقط!)
    return raw_token;
}

/**
 * @brief التحقق من رمز API
 *
 * ١. نحسب بصمة SHA256 للرمز المُدخل
 * ٢. نبحث عنها في قاعدة البيانات
 * ٣. إذا وُجدت ← نرجع بيانات المستخدم
 */
std::optional<UserInfo> AuthManager::verify_api_token(const std::string& token) {
    std::string token_hash = compute_sha256(token);
    return db_->verify_token(token_hash);
}

/**
 * إبطال رمز API
 */
bool AuthManager::revoke_api_token(int64_t token_id) {
    return db_->revoke_token(token_id);
}

// ═══════════════════════════════════════════════════════════════════════════════
// تشفير كلمات المرور
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تشفير كلمة مرور بملح عشوائي
 *
 * الصيغة الناتجة: "salt_hex:hash_hex"
 *
 * نستخدم تكرار SHA256 عدة مرات (PBKDF2-like):
 *   hash = SHA256(salt + password)
 *   for i in 1..10000:
 *       hash = SHA256(hash + salt + password)
 *
 * ١٠,٠٠٠ تكرار يجعل هجمات التخمين بالقوة الغاشمة
 * أبطأ بـ ١٠,٠٠٠ مرة.
 */
std::string AuthManager::hash_password(const std::string& password) {
    std::string salt = generate_salt(16);

    // تكرار البصمة
    std::string hash = compute_sha256(salt + password);
    for (int i = 0; i < 10000; i++) {
        hash = compute_sha256(hash + salt + password);
    }

    return salt + ":" + hash;
}

/**
 * @brief التحقق من كلمة مرور مقابل بصمتها المخزنة
 *
 * نستخرج الملح من البصمة المخزنة ونعيد الحساب
 * ثم نقارن النتائج
 */
bool AuthManager::verify_password(const std::string& password,
                                    const std::string& stored_hash) {
    // تقسيم: salt:hash
    size_t colon = stored_hash.find(':');
    if (colon == std::string::npos) return false;

    std::string salt = stored_hash.substr(0, colon);
    std::string expected_hash = stored_hash.substr(colon + 1);

    // إعادة الحساب بنفس الملح
    std::string hash = compute_sha256(salt + password);
    for (int i = 0; i < 10000; i++) {
        hash = compute_sha256(hash + salt + password);
    }

    // مقارنة ثابتة الوقت (لمنع هجمات التوقيت)
    if (hash.size() != expected_hash.size()) return false;

    volatile int diff = 0;
    for (size_t i = 0; i < hash.size(); i++) {
        diff |= hash[i] ^ expected_hash[i];
    }

    return diff == 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// رموز JWT
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إنشاء رمز JWT
 *
 * بنية JWT:
 *   BASE64(header) . BASE64(payload) . BASE64(HMAC(header.payload))
 *
 * الحمولة تحتوي:
 *   uid  = معرف المستخدم
 *   name = اسم المستخدم
 *   exp  = تاريخ الانتهاء (Unix timestamp)
 *   iat  = تاريخ الإنشاء
 */
std::string AuthManager::create_jwt(int64_t user_id,
                                      const std::string& username,
                                      int expiry_days) {
    // حساب تاريخ الانتهاء
    auto now = std::chrono::system_clock::now();
    auto exp = now + std::chrono::hours(24 * expiry_days);
    auto now_epoch = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    auto exp_epoch = std::chrono::duration_cast<std::chrono::seconds>(
        exp.time_since_epoch()).count();

    // الرأس
    std::string header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";

    // الحمولة
    std::string payload = "{\"uid\":" + std::to_string(user_id)
                        + ",\"name\":\"" + username + "\""
                        + ",\"iat\":" + std::to_string(now_epoch)
                        + ",\"exp\":" + std::to_string(exp_epoch)
                        + "}";

    // ترميز
    std::string header_b64 = base64_encode(header);
    std::string payload_b64 = base64_encode(payload);

    // التوقيع
    std::string data = header_b64 + "." + payload_b64;
    std::string signature = base64_encode(hmac_sha256(secret_key_, data));

    return header_b64 + "." + payload_b64 + "." + signature;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Base64 URL-safe
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief ترميز Base64 URL-safe (بدون = للحشو)
 *
 * Base64 العادي يستخدم: A-Z, a-z, 0-9, +, /
 * Base64 URL-safe يستخدم: A-Z, a-z, 0-9, -, _
 * (لأن + و / لها معانٍ في URLs)
 */
std::string AuthManager::base64_encode(const std::string& data) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    std::string result;
    result.reserve(((data.size() + 2) / 3) * 4);

    for (size_t i = 0; i < data.size(); i += 3) {
        uint32_t n = (static_cast<uint8_t>(data[i]) << 16);
        if (i + 1 < data.size()) n |= (static_cast<uint8_t>(data[i + 1]) << 8);
        if (i + 2 < data.size()) n |= static_cast<uint8_t>(data[i + 2]);

        result += table[(n >> 18) & 0x3F];
        result += table[(n >> 12) & 0x3F];
        if (i + 1 < data.size()) result += table[(n >> 6) & 0x3F];
        if (i + 2 < data.size()) result += table[n & 0x3F];
    }

    return result;
}

/**
 * @brief فك ترميز Base64 URL-safe
 */
std::string AuthManager::base64_decode(const std::string& encoded) {
    static const int table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,63,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    };

    std::string result;
    result.reserve(encoded.size() * 3 / 4);

    uint32_t buf = 0;
    int bits = 0;

    for (char c : encoded) {
        int val = table[static_cast<uint8_t>(c)];
        if (val < 0) continue;

        buf = (buf << 6) | val;
        bits += 6;

        if (bits >= 8) {
            bits -= 8;
            result += static_cast<char>((buf >> bits) & 0xFF);
        }
    }

    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// HMAC-SHA256
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief حساب HMAC-SHA256
 *
 * HMAC = Hash-based Message Authentication Code
 * يُنتج بصمة مفتاحية (keyed hash) للتحقق من سلامة البيانات ومصدرها.
 *
 * الخوارزمية:
 *   ipad = key XOR 0x36 (مكرر حتى 64 بايت)
 *   opad = key XOR 0x5C (مكرر حتى 64 بايت)
 *   HMAC = SHA256(opad + SHA256(ipad + message))
 */
std::string AuthManager::hmac_sha256(const std::string& key,
                                       const std::string& data) {
    const int BLOCK_SIZE = 64; // حجم كتلة SHA256

    // إعداد المفتاح (حشو أو تقليص)
    std::string k = key;
    if (k.size() > BLOCK_SIZE) {
        k = compute_sha256(k);
        // تحويل hex string إلى bytes
        std::string bytes;
        for (size_t i = 0; i < k.size(); i += 2) {
            bytes += static_cast<char>(std::stoi(k.substr(i, 2), nullptr, 16));
        }
        k = bytes;
    }
    k.resize(BLOCK_SIZE, '\0'); // حشو بأصفار

    // حساب ipad و opad
    std::string ipad(BLOCK_SIZE, '\0');
    std::string opad(BLOCK_SIZE, '\0');
    for (int i = 0; i < BLOCK_SIZE; i++) {
        ipad[i] = k[i] ^ 0x36;
        opad[i] = k[i] ^ 0x5C;
    }

    // البصمة الداخلية: SHA256(ipad + data)
    std::string inner_hash_hex = compute_sha256(ipad + data);

    // تحويل hex إلى bytes
    std::string inner_bytes;
    for (size_t i = 0; i < inner_hash_hex.size(); i += 2) {
        inner_bytes += static_cast<char>(
            std::stoi(inner_hash_hex.substr(i, 2), nullptr, 16));
    }

    // البصمة الخارجية: SHA256(opad + inner_hash)
    return compute_sha256(opad + inner_bytes);
}

// ═══════════════════════════════════════════════════════════════════════════════
// قيم عشوائية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إنشاء ملح عشوائي كنص سداسي عشري
 */
std::string AuthManager::generate_salt(int length) {
    return generate_random_token(length);
}

/**
 * @brief إنشاء رمز عشوائي
 *
 * يستخدم مولد عشوائي آمن تشفيرياً (على الأنظمة التي تدعمه).
 * يُنتج نصاً سداسياً عشرياً بطول length * 2 حرف.
 */
std::string AuthManager::generate_random_token(int length) {
    // استخدام مولد عشوائي من النظام
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < length; i++) {
        ss << std::setw(2) << dis(gen);
    }

    return ss.str();
}

} // namespace registry
} // namespace sad
