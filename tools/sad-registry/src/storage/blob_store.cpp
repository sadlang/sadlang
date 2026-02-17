// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: blob_store.cpp
 * الوصف: تنفيذ التخزين المعنون بالمحتوى لأرشيفات الحزم
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * التخزين المعنون بالمحتوى (Content-Addressable Storage):
 *   - كل ملف يُخزَّن تحت اسم = SHA256(محتوى الملف)
 *   - نفس المحتوى = نفس التجزئة = يُخزَّن مرة واحدة فقط
 *   - المسار: blobs/ab/cd/abcdef1234...zip
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "blob_store.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <array>

namespace sad {
namespace registry {

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ SHA-256 مُضمَّن
// ═══════════════════════════════════════════════════════════════════════════════
//
// تنفيذ مستقل لخوارزمية SHA-256 بدون تبعيات خارجية.
// يُستخدم لحساب بصمة الملفات عند التخزين والاسترجاع.

namespace {

/// الثوابت K في SHA-256 (أول 32 بت من الجذور التكعيبية لأول 64 عدداً أولياً)
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

inline uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }

/**
 * @brief حساب بصمة SHA-256 لمصفوفة بايتات
 * @param data مؤشر للبيانات
 * @param len طول البيانات
 * @return سلسلة سداسية عشرية (64 حرفاً)
 */
std::string compute_sha256(const uint8_t* data, size_t len) {
    // القيم الأولية H (أول 32 بت من الجذور التربيعية لأول 8 أعداد أولية)
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // حشو الرسالة: إضافة بت 1 ثم أصفار ثم الطول بالبتات
    size_t padded_len = ((len + 8) / 64 + 1) * 64;
    std::vector<uint8_t> padded(padded_len, 0);
    std::memcpy(padded.data(), data, len);
    padded[len] = 0x80;

    // كتابة طول الرسالة الأصلي (بالبتات) كعدد 64 بت big-endian
    uint64_t bit_len = static_cast<uint64_t>(len) * 8;
    for (int i = 0; i < 8; ++i) {
        padded[padded_len - 1 - i] = static_cast<uint8_t>(bit_len >> (i * 8));
    }

    // معالجة كل كتلة (512 بت = 64 بايت)
    for (size_t offset = 0; offset < padded_len; offset += 64) {
        uint32_t w[64];

        // تحضير جدول الرسائل
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint32_t>(padded[offset + i * 4]) << 24)
                 | (static_cast<uint32_t>(padded[offset + i * 4 + 1]) << 16)
                 | (static_cast<uint32_t>(padded[offset + i * 4 + 2]) << 8)
                 | (static_cast<uint32_t>(padded[offset + i * 4 + 3]));
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i-15], 7) ^ rotr(w[i-15], 18) ^ (w[i-15] >> 3);
            uint32_t s1 = rotr(w[i-2], 17) ^ rotr(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }

        // متغيرات العمل
        uint32_t a=h[0], b=h[1], c=h[2], d=h[3];
        uint32_t e=h[4], f=h[5], g=h[6], hh=h[7];

        // الحلقة الرئيسية (64 جولة)
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t t1 = hh + S1 + ch + SHA256_K[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t t2 = S0 + maj;

            hh = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }

        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d;
        h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }

    // تحويل النتيجة إلى سلسلة سداسية عشرية
    std::ostringstream oss;
    for (int i = 0; i < 8; ++i) {
        oss << std::hex << std::setfill('0') << std::setw(8) << h[i];
    }
    return oss.str();
}

} // مساحة أسماء مجهولة

// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ والتهيئة
// ═══════════════════════════════════════════════════════════════════════════════

BlobStore::BlobStore(const std::string& base_dir) : base_dir_(base_dir) {}

/**
 * @brief تهيئة مخزن الكائنات الثنائية
 *
 * ١. إنشاء مجلد التخزين الأساسي
 * ٢. إنشاء مجلد مؤقت للتحميلات الجارية
 * ٣. التحقق من صلاحيات الكتابة
 */
bool BlobStore::initialize() {
    try {
        // إنشاء مجلد التخزين الأساسي
        fs::create_directories(base_dir_);

        // إنشاء مجلد مؤقت للتحميلات
        fs::create_directories(base_dir_ + "/temp");

        // اختبار صلاحيات الكتابة
        std::string test_path = base_dir_ + "/temp/.write_test";
        std::ofstream test(test_path);
        if (!test.is_open()) {
            std::cerr << "  [خطأ] لا يمكن الكتابة في مجلد التخزين: " << base_dir_ << "\n";
            return false;
        }
        test << "test";
        test.close();
        fs::remove(test_path);

        std::cerr << "  [معلومات] تم تهيئة مخزن الكائنات: " << base_dir_ << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "  [خطأ] فشل تهيئة المخزن: " << e.what() << "\n";
        return false;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// التخزين
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تخزين بيانات وإرجاع بصمتها
 *
 * العملية:
 *   ١. حساب SHA-256 للبيانات
 *   ٢. إنشاء مسار التخزين: blobs/ab/cd/abcdef...
 *   ٣. إذا كانت البصمة موجودة مسبقاً → إرجاع البصمة مباشرة (تكرار مكشوف)
 *   ٤. كتابة الملف أولاً إلى المجلد المؤقت ثم نقله (عملية ذرية)
 *
 * @param data البيانات المراد تخزينها
 * @return بصمة SHA-256 (سلسلة 64 حرفاً)
 */
std::string BlobStore::store(const std::vector<uint8_t>& data) {
    // حساب البصمة
    std::string hash = sha256(data);

    // اشتقاق المسار
    std::string blob_path = get_blob_path(hash);

    // إذا كان الملف موجوداً مسبقاً (نفس المحتوى) → لا حاجة لإعادة التخزين
    if (fs::exists(blob_path)) {
        return hash;
    }

    // إنشاء المجلدات الفرعية
    fs::path dir = fs::path(blob_path).parent_path();
    fs::create_directories(dir);

    // كتابة إلى ملف مؤقت أولاً (لضمان الذرية)
    std::string temp_path = base_dir_ + "/temp/" + hash + ".tmp";
    {
        std::ofstream out(temp_path, std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "  [خطأ] فشل كتابة الملف المؤقت: " << temp_path << "\n";
            return "";
        }
        out.write(reinterpret_cast<const char*>(data.data()), data.size());
        out.close();
    }

    // نقل من المؤقت إلى الموقع النهائي
    try {
        fs::rename(temp_path, blob_path);
    } catch (...) {
        // في حالة فشل النقل (محرك أقراص مختلف) → نسخ ثم حذف
        fs::copy_file(temp_path, blob_path, fs::copy_options::overwrite_existing);
        fs::remove(temp_path);
    }

    return hash;
}

// ═══════════════════════════════════════════════════════════════════════════════
// الاسترجاع
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief استرجاع بيانات من المخزن بالبصمة
 *
 * @param hash بصمة SHA-256 للكائن
 * @return البيانات إذا وُجدت، أو مصفوفة فارغة
 */
std::vector<uint8_t> BlobStore::retrieve(const std::string& hash) {
    std::string blob_path = get_blob_path(hash);

    if (!fs::exists(blob_path)) {
        return {};
    }

    // قراءة الملف
    std::ifstream in(blob_path, std::ios::binary | std::ios::ate);
    if (!in.is_open()) return {};

    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(size);
    in.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

// ═══════════════════════════════════════════════════════════════════════════════
// الاستعلام والحذف
// ═══════════════════════════════════════════════════════════════════════════════

bool BlobStore::exists(const std::string& hash) {
    return fs::exists(get_blob_path(hash));
}

bool BlobStore::remove(const std::string& hash) {
    std::string path = get_blob_path(hash);
    if (fs::exists(path)) {
        return fs::remove(path);
    }
    return false;
}

BlobInfo BlobStore::get_info(const std::string& hash) {
    BlobInfo info;
    info.hash = hash;

    std::string path = get_blob_path(hash);
    if (fs::exists(path)) {
        info.size = fs::file_size(path);
        info.path = path;
    } else {
        info.size = 0;
    }

    return info;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief اشتقاق مسار الملف من البصمة
 *
 * النمط: base_dir/ab/cd/abcdef1234567890...
 * التقسيم إلى مجلدات فرعية يمنع تراكم آلاف الملفات في مجلد واحد
 */
std::string BlobStore::get_blob_path(const std::string& hash) {
    if (hash.size() < 4) return base_dir_ + "/" + hash;

    // أول بايتين → مجلد أول، ثاني بايتين → مجلد فرعي
    std::string dir1 = hash.substr(0, 2);
    std::string dir2 = hash.substr(2, 2);

    return base_dir_ + "/" + dir1 + "/" + dir2 + "/" + hash;
}

/**
 * @brief حساب SHA-256 لمصفوفة بايتات
 */
std::string BlobStore::sha256(const std::vector<uint8_t>& data) {
    return compute_sha256(data.data(), data.size());
}

} // namespace registry
} // namespace sad
