// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: blob_store.h
 * الوصف: مخزن الكائنات - تخزين ملفات الحزم المضغوطة
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مخزن الكائنات (Blob Store) هو المكون المسؤول عن تخزين وإدارة
 * الملفات الفعلية للحزم (أرشيفات ZIP).
 *
 * يستخدم نمط التخزين المحتوى-الموجّه (Content-Addressable Storage - CAS):
 *   - كل ملف يُعرَّف ببصمته SHA256
 *   - نفس المحتوى لا يُخزَّن مرتين (deduplication)
 *   - المسار يُشتق من البصمة: ab/cd/abcdef1234...
 *   - التحقق من سلامة الملف تلقائي عند القراءة
 *
 * البنية على القرص:
 *   blobs/
 *   ├── ab/
 *   │   ├── cd/
 *   │   │   └── abcdef1234567890.zip   ← ملف حزمة
 *   │   └── ef/
 *   │       └── abef9876543210.zip
 *   ├── temp/                           ← ملفات مؤقتة أثناء الرفع
 *   └── metadata.json                   ← إحصائيات المخزن
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <cstdint>

namespace sad {
namespace registry {

/**
 * @struct معلومات_الكائن
 * @brief معلومات عن كائن (ملف) مخزّن
 */
struct BlobInfo {
    std::string hash;            // بصمة SHA256
    std::string path;            // المسار على القرص
    int64_t size = 0;            // الحجم بالبايت
    std::string created_at;      // تاريخ التخزين
};

/**
 * @class BlobStore
 * @brief مخزن الكائنات للحزم المضغوطة
 *
 * يخزن ملفات الحزم بطريقة محتوى-موجّهة.
 * كل ملف يُعرَّف ببصمته SHA256 مما يضمن:
 *   - عدم تكرار التخزين (ملفان بنفس المحتوى = تخزين واحد)
 *   - التحقق التلقائي من سلامة الملفات
 *   - مسارات حتمية (deterministic) من البصمة
 *
 * مبدأ التخزين:
 *   بصمة: a1b2c3d4e5f6...
 *   مسار: blobs/a1/b2/a1b2c3d4e5f6...zip
 *   (أول بايتين من البصمة = اسم المجلد الأول، البايتان التاليان = المجلد الفرعي)
 */
class BlobStore {
public:
    /**
     * @brief المُنشئ
     * @param base_dir المجلد الأساسي للتخزين
     *
     * لا يُنشئ المجلدات فوراً - يجب استدعاء initialize() أولاً.
     */
    explicit BlobStore(const std::string& base_dir);

    /**
     * @brief تهيئة المخزن
     * @return true إذا نجحت التهيئة
     *
     * تُنشئ المجلدات اللازمة وتتحقق من صلاحيات الكتابة.
     */
    bool initialize();

    /**
     * @brief تخزين ملف جديد
     * @param data محتوى الملف (البايتات)
     * @return بصمة SHA256 للملف المخزّن
     *
     * الخطوات:
     *   ١. حساب بصمة SHA256 للمحتوى
     *   ٢. التحقق: هل الملف موجود مسبقاً؟ (بنفس البصمة)
     *   ٣. إذا لا: كتابة الملف في المسار المشتق من البصمة
     *   ٤. إرجاع البصمة
     *
     * ملاحظة: إذا كان الملف موجوداً مسبقاً بنفس المحتوى، لا يُكتب مرة أخرى (dedup).
     */
    std::string store(const std::vector<uint8_t>& data);

    /**
     * @brief قراءة ملف مخزّن
     * @param hash بصمة SHA256
     * @return محتوى الملف، أو مصفوفة فارغة إذا لم يُوجد
     *
     * يتحقق من سلامة الملف بعد القراءة:
     *   - يحسب بصمة المحتوى المقروء
     *   - يقارنها بالبصمة المطلوبة
     *   - إذا لم تتطابق: يُبلغ عن تلف وينظف الملف
     */
    std::vector<uint8_t> retrieve(const std::string& hash);

    /**
     * @brief التحقق من وجود ملف
     * @param hash بصمة SHA256
     * @return true إذا كان الملف موجوداً
     */
    bool exists(const std::string& hash);

    /**
     * @brief حذف ملف
     * @param hash بصمة SHA256
     * @return true إذا تم الحذف بنجاح
     *
     * تحذير: الحذف لا يُرجع! يُستخدم فقط عند سحب الحزم نهائياً.
     */
    bool remove(const std::string& hash);

    /**
     * @brief الحصول على مسار ملف مخزّن
     * @param hash بصمة SHA256
     * @return المسار الكامل على القرص
     */
    std::string get_blob_path(const std::string& hash);

    /**
     * @brief معلومات عن ملف مخزّن
     */
    BlobInfo get_info(const std::string& hash);

    /**
     * @brief إجمالي المساحة المستخدمة (بالبايت)
     */
    int64_t total_size();

    /**
     * @brief عدد الكائنات المخزنة
     */
    int64_t total_count();

    /**
     * @brief حساب بصمة SHA256 لبيانات
     * @param data البيانات المراد حساب بصمتها
     * @return البصمة كنص سداسي عشري (64 حرفاً)
     *
     * تستخدم خوارزمية SHA-256 المنفذة بالكامل في C++ بدون تبعيات.
     * هذه نفس الخوارزمية المستخدمة في Bitcoin و TLS وغيرها.
     */
    static std::string sha256(const std::vector<uint8_t>& data);

private:
    std::string base_dir_;       // المجلد الأساسي
    std::string temp_dir_;       // مجلد الملفات المؤقتة
};

} // namespace registry
} // namespace sad
