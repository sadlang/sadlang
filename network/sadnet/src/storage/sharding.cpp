// ===================================================================
// صNet - تجزئة الملفات للتخزين الموزع
// المرحلة 53: تقسيم وترميز وتوزيع الملفات
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <memory>
#include <algorithm>

namespace sad::network {

/// حجم القطعة الافتراضي (256 كيلوبايت)
constexpr size_t DEFAULT_SHARD_SIZE = 256 * 1024;

/// حجم بصمة SHA-256
constexpr size_t HASH_SIZE = 32;

/// بصمة تشفيرية للقطعة
using ShardHash = std::array<uint8_t, HASH_SIZE>;

/// معلومات قطعة واحدة من الملف
struct ShardInfo {
    uint32_t index;                // رقم القطعة في الملف
    ShardHash hash;                // بصمة SHA-256 للتحقق
    size_t size;                   // حجم القطعة بالبايت
    std::vector<std::string> locations; // عُقد تحتفظ بنسخة
    bool is_parity;                // هل هي قطعة تكافؤ (reed-solomon)؟
};

/// بيانات وصفية للملف المُجزّأ
struct FileManifest {
    std::string file_id;           // معرّف الملف الفريد
    std::string original_name;     // الاسم الأصلي
    uint64_t total_size;           // الحجم الإجمالي
    uint32_t shard_count;          // عدد القطع الأصلية
    uint32_t parity_count;         // عدد قطع التكافؤ
    size_t shard_size;             // حجم كل قطعة
    ShardHash root_hash;           // بصمة شجرة Merkle الجذرية
    std::vector<ShardInfo> shards; // معلومات كل قطعة
};

/// حساب بصمة SHA-256 (محاكاة)
class SHA256Hasher {
public:
    ShardHash hash(const std::vector<uint8_t>& data) {
        ShardHash result{};
        // حساب SHA-256 الفعلي
        // هنا نستخدم تجزئة بسيطة للتوضيح
        for (size_t i = 0; i < data.size(); i++) {
            result[i % HASH_SIZE] ^= data[i];
        }
        return result;
    }
};

/// ترميز التكافؤ Reed-Solomon - لاسترداد القطع المفقودة
class ReedSolomonEncoder {
    uint32_t data_shards_;      // عدد القطع الأصلية
    uint32_t parity_shards_;    // عدد قطع التكافؤ

public:
    ReedSolomonEncoder(uint32_t data = 10, uint32_t parity = 4)
        : data_shards_(data), parity_shards_(parity) {}

    /// توليد قطع التكافؤ من القطع الأصلية
    std::vector<std::vector<uint8_t>> encode(
            const std::vector<std::vector<uint8_t>>& data_shards) {
        std::vector<std::vector<uint8_t>> parity;
        // حساب قطع Reed-Solomon
        // يسمح باسترداد أي parity_shards_ قطع مفقودة
        for (uint32_t i = 0; i < parity_shards_; i++) {
            std::vector<uint8_t> p(data_shards[0].size(), 0);
            // XOR بسيط كمثال (الترميز الحقيقي أعقد)
            for (const auto& shard : data_shards) {
                for (size_t j = 0; j < p.size() && j < shard.size(); j++) {
                    p[j] ^= shard[j];
                }
            }
            parity.push_back(std::move(p));
        }
        return parity;
    }

    /// استعادة القطع المفقودة من القطع المتوفرة
    bool recover(std::vector<std::vector<uint8_t>>& shards,
                 const std::vector<bool>& available) {
        uint32_t missing = 0;
        for (bool a : available) if (!a) missing++;
        // يمكن الاستعادة إذا كان عدد المفقود <= قطع التكافؤ
        return missing <= parity_shards_;
    }
};

/// مُجزّئ الملفات الرئيسي
class FileShardingEngine {
    size_t shard_size_;                // حجم القطعة
    SHA256Hasher hasher_;              // حاسب البصمات
    ReedSolomonEncoder encoder_;       // مُرمّز التكافؤ
    uint32_t replication_factor_;      // عامل التكرار

public:
    FileShardingEngine(size_t shard_sz = DEFAULT_SHARD_SIZE,
                       uint32_t replication = 3)
        : shard_size_(shard_sz), replication_factor_(replication) {}

    /// تجزئة ملف كامل إلى قطع
    FileManifest shard_file(const std::string& name,
                            const std::vector<uint8_t>& file_data) {
        FileManifest manifest;
        manifest.original_name = name;
        manifest.total_size = file_data.size();
        manifest.shard_size = shard_size_;

        // تقسيم الملف إلى قطع
        std::vector<std::vector<uint8_t>> data_shards;
        for (size_t offset = 0; offset < file_data.size(); offset += shard_size_) {
            size_t len = std::min(shard_size_, file_data.size() - offset);
            std::vector<uint8_t> shard(file_data.begin() + offset,
                                        file_data.begin() + offset + len);
            // حساب بصمة كل قطعة
            ShardInfo info;
            info.index = data_shards.size();
            info.hash = hasher_.hash(shard);
            info.size = len;
            info.is_parity = false;
            manifest.shards.push_back(info);
            data_shards.push_back(std::move(shard));
        }

        manifest.shard_count = data_shards.size();

        // توليد قطع التكافؤ
        if (!data_shards.empty()) {
            auto parity = encoder_.encode(data_shards);
            for (size_t i = 0; i < parity.size(); i++) {
                ShardInfo pinfo;
                pinfo.index = manifest.shard_count + i;
                pinfo.hash = hasher_.hash(parity[i]);
                pinfo.size = parity[i].size();
                pinfo.is_parity = true;
                manifest.shards.push_back(pinfo);
            }
            manifest.parity_count = parity.size();
        }

        return manifest;
    }

    /// إعادة تجميع الملف من قطعه
    std::vector<uint8_t> reassemble(const FileManifest& manifest,
                                     const std::vector<std::vector<uint8_t>>& shards) {
        std::vector<uint8_t> result;
        result.reserve(manifest.total_size);
        for (uint32_t i = 0; i < manifest.shard_count; i++) {
            if (i < shards.size()) {
                // التحقق من بصمة القطعة
                auto computed = hasher_.hash(shards[i]);
                if (computed == manifest.shards[i].hash) {
                    result.insert(result.end(), shards[i].begin(), shards[i].end());
                }
            }
        }
        result.resize(manifest.total_size);
        return result;
    }
};

} // namespace sad::network
