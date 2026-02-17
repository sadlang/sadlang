// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: auth.h
 * الوصف: نظام المصادقة والتحقق من الهوية لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يوفر نظام مصادقة كامل يشمل:
 *   ١. تسجيل المستخدمين الجدد مع التحقق من البيانات
 *   ٢. تسجيل الدخول وإنشاء رموز JWT
 *   ٣. التحقق من صحة الرموز عند كل طلب محمي
 *   ٤. إدارة رموز الوصول (API tokens) للنشر البرمجي
 *   ٥. تشفير كلمات المرور بخوارزمية آمنة
 *
 * تدفق المصادقة:
 *   ┌──────────┐     ┌──────────┐     ┌──────────┐
 *   │ تسجيل   │ ──→ │ تسجيل   │ ──→ │ استخدام  │
 *   │ الحساب  │     │ الدخول  │     │ الرمز   │
 *   └──────────┘     └──────────┘     └──────────┘
 *        │                │                │
 *        ▼                ▼                ▼
 *   حفظ كلمة         إنشاء رمز       إرسال الرمز
 *   مرور مشفرة       JWT              مع كل طلب
 *                                    (Authorization: Bearer xxx)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "../storage/database.h"
#include <string>
#include <optional>
#include <memory>
#include <chrono>

namespace sad {
namespace registry {

/**
 * @struct نتيجة_المصادقة
 * @brief نتيجة عملية تسجيل الدخول أو التحقق من الرمز
 */
struct AuthResult {
    bool success = false;            // هل نجحت المصادقة؟
    std::string token;               // رمز الجلسة (JWT)
    std::string error;               // رسالة الخطأ (إن فشلت)
    int64_t user_id = 0;             // معرف المستخدم
    std::string username;            // اسم المستخدم
};

/**
 * @class AuthManager
 * @brief مدير المصادقة الرئيسي
 *
 * المسؤوليات:
 *   - تسجيل حسابات جديدة مع التحقق من البيانات
 *   - تسجيل الدخول وإنشاء رموز JWT
 *   - التحقق من الرموز مع كل طلب محمي
 *   - إدارة رموز API للنشر من سطر الأوامر
 *   - تشفير كلمات المرور وفحصها
 *
 * الأمان:
 *   - كلمات المرور تُشفَّر بـ PBKDF2-HMAC-SHA256
 *   - الرموز تنتهي بعد ٣٠ يوماً افتراضياً
 *   - كل رمز يحتوي على معرف المستخدم وتاريخ الانتهاء
 *   - الرموز مُوقَّعة بمفتاح سري لمنع التزوير
 */
class AuthManager {
public:
    /**
     * @brief المُنشئ
     * @param db مؤشر على قاعدة البيانات
     *
     * يُنشئ مفتاح التوقيع السري عند عدم وجوده.
     * المفتاح يُخزَّن مع قاعدة البيانات.
     */
    explicit AuthManager(std::shared_ptr<Database> db);

    // ─────────────────────────────────────────────────────────────────────
    // تسجيل الحسابات
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل مستخدم جديد
     * @param username اسم المستخدم (عربي أو إنجليزي)
     * @param email البريد الإلكتروني
     * @param password كلمة المرور (نصية - سيتم تشفيرها)
     * @param display_name الاسم المعروض (اختياري)
     * @return نتيجة التسجيل مع رمز الجلسة
     *
     * التحققات:
     *   - اسم المستخدم: ٣-٣٠ حرفاً، عربية/لاتينية/أرقام/شرطة سفلية
     *   - البريد: صيغة صحيحة وفريد
     *   - كلمة المرور: ٨ أحرف على الأقل
     */
    AuthResult register_user(const std::string& username,
                             const std::string& email,
                             const std::string& password,
                             const std::string& display_name = "");

    // ─────────────────────────────────────────────────────────────────────
    // تسجيل الدخول
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل الدخول
     * @param username_or_email اسم المستخدم أو البريد الإلكتروني
     * @param password كلمة المرور
     * @return نتيجة المصادقة مع رمز JWT
     *
     * يبحث أولاً بالاسم، ثم بالبريد.
     * يقارن كلمة المرور مع البصمة المخزنة.
     */
    AuthResult login(const std::string& username_or_email,
                     const std::string& password);

    // ─────────────────────────────────────────────────────────────────────
    // التحقق من الرموز
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief التحقق من رمز JWT
     * @param token الرمز المراد التحقق منه
     * @return بيانات المستخدم، أو nullopt إذا كان الرمز غير صالح
     *
     * خطوات التحقق:
     *   ١. فك ترميز الرمز (Base64)
     *   ٢. التحقق من التوقيع (HMAC)
     *   ٣. التحقق من عدم انتهاء الصلاحية
     *   ٤. جلب بيانات المستخدم من قاعدة البيانات
     *   ٥. التحقق من أن الحساب لا يزال مفعلاً
     */
    std::optional<UserInfo> verify_token(const std::string& token);

    // ─────────────────────────────────────────────────────────────────────
    // رموز API
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء رمز API للنشر
     * @param user_id معرف المستخدم
     * @param token_name اسم الرمز (لتمييزه عن غيره)
     * @return الرمز (يُعرض مرة واحدة فقط!)
     *
     * رموز API تُستخدم من sad-pkg لنشر الحزم من سطر الأوامر.
     * الرمز يُعرض مرة واحدة فقط عند الإنشاء - لا يمكن استرجاعه لاحقاً.
     * ما يُخزن في قاعدة البيانات هو بصمة SHA256 للرمز فقط.
     */
    std::string create_api_token(int64_t user_id,
                                  const std::string& token_name = "default");

    /**
     * @brief التحقق من رمز API
     * @return بيانات المستخدم صاحب الرمز
     */
    std::optional<UserInfo> verify_api_token(const std::string& token);

    /**
     * @brief إبطال رمز API
     */
    bool revoke_api_token(int64_t token_id);

private:
    std::shared_ptr<Database> db_;    // قاعدة البيانات
    std::string secret_key_;          // مفتاح توقيع JWT

    // ─────────────────────────────────────────────────────────────────────
    // تشفير كلمات المرور
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تشفير كلمة مرور
     * @param password كلمة المرور النصية
     * @return البصمة المشفرة (بصيغة: salt:hash)
     *
     * نستخدم PBKDF2 مع:
     *   - ملح عشوائي (16 بايت)
     *   - ١٠٠٠٠ تكرار (يبطئ هجمات التخمين)
     *   - SHA256 كدالة البصمة
     */
    std::string hash_password(const std::string& password);

    /**
     * @brief التحقق من كلمة مرور مقابل بصمتها
     * @param password كلمة المرور المُدخلة
     * @param hash البصمة المخزنة
     * @return true إذا تطابقت
     */
    bool verify_password(const std::string& password, const std::string& hash);

    // ─────────────────────────────────────────────────────────────────────
    // رموز JWT
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء رمز JWT
     * @param user_id معرف المستخدم
     * @param username اسم المستخدم
     * @param expiry_days مدة الصلاحية بالأيام
     * @return الرمز المُوقَّع
     *
     * بنية JWT المبسطة:
     *   header.payload.signature
     *
     *   header: {"alg":"HS256"}
     *   payload: {"uid":123,"name":"أحمد","exp":1740000000}
     *   signature: HMAC-SHA256(header.payload, secret)
     */
    std::string create_jwt(int64_t user_id, const std::string& username,
                           int expiry_days = 30);

    /**
     * @brief ترميز Base64 (URL-safe)
     */
    static std::string base64_encode(const std::string& data);

    /**
     * @brief فك ترميز Base64 (URL-safe)
     */
    static std::string base64_decode(const std::string& encoded);

    /**
     * @brief حساب HMAC-SHA256
     */
    static std::string hmac_sha256(const std::string& key,
                                    const std::string& data);

    /**
     * @brief إنشاء ملح عشوائي
     */
    static std::string generate_salt(int length = 16);

    /**
     * @brief إنشاء رمز عشوائي
     */
    static std::string generate_random_token(int length = 32);
};

} // namespace registry
} // namespace sad
