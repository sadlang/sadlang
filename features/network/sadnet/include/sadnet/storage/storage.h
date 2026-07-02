// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة التخزين الموزع (Distributed Storage)
// (AR) تخزين ملفات موزع مشابه لـ IPFS
//      تجزئة المحتوى (Content Addressing)، تكرار عبر DHT
//      تقسيم الملفات الكبيرة إلى قطع (chunks)
// (EN) Distributed storage: content-addressed, chunked, replicated
// المكون: features/network/sadnet (التخزين الموزع)
// المسار: features/network/sadnet/include/sadnet/storage/storage.h
// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_STORAGE_STORAGE_H
#define SAD_SADNET_STORAGE_STORAGE_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace sad::net::storage
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using sad::net::crypto::KeyPair;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ثوابت التخزين
    // ═════════════════════════════════════════════════════════════════════

    constexpr size_t CHUNK_SIZE = 256 * 1024;              // 256 KB لكل قطعة
    constexpr uint32_t REPLICATION = 3;                    // عدد النسخ
    constexpr uint32_t MAX_FILE_SIZE = 1024 * 1024 * 1024; // 1 GB

    // ═════════════════════════════════════════════════════════════════════
    // (AR) معرف المحتوى (Content ID) — SHA-256 للمحتوى
    // ═════════════════════════════════════════════════════════════════════

    using ContentId = NodeId; // نفس البنية: 32 بايت

    // ═════════════════════════════════════════════════════════════════════
    // (AR) قطعة بيانات (Chunk)
    // ═════════════════════════════════════════════════════════════════════

    struct Chunk
    {
        ContentId id; // SHA-256 للمحتوى
        Buffer data;
        uint32_t index = 0; // ترتيب القطعة في الملف
        uint64_t stored_at = 0;

        // (AR) حساب المعرف من المحتوى
        void compute_id()
        {
            auto hash = crypto::sha256(data);
            std::memcpy(id.data.data(), hash.data(), NODE_ID_SIZE);
        }

        Buffer serialize() const;
        static Chunk deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) بيان الملف (File Manifest) — يصف ملفاً كاملاً
    // ═════════════════════════════════════════════════════════════════════

    struct FileManifest
    {
        ContentId root_id;             // معرف الملف الجذري
        std::string name;              // اسم الملف
        uint64_t total_size = 0;       // الحجم الإجمالي
        std::vector<ContentId> chunks; // قائمة معرفات القطع
        NodeId owner;
        uint64_t created_at = 0;
        Buffer signature;

        // (AR) حساب المعرف الجذري
        void compute_root_id()
        {
            Buffer all_ids;
            for (const auto &cid : chunks)
                all_ids.append(cid.data.data(), NODE_ID_SIZE);
            auto hash = crypto::sha256(all_ids);
            std::memcpy(root_id.data.data(), hash.data(), NODE_ID_SIZE);
        }

        Buffer serialize() const;
        static FileManifest deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير التخزين الموزع
    // ═════════════════════════════════════════════════════════════════════

    using ChunkRequest = std::function<Result<Buffer>(const ContentId &id)>;
    using ChunkStore = std::function<Result<void>(const ContentId &id, const Buffer &data)>;

    class StorageManager
    {
    public:
        StorageManager(const NodeId &self_id, const crypto::KeyPair &keys);
        ~StorageManager();

        // ─── تخزين ───

        // (AR) تخزين ملف (يُقسم تلقائياً إلى قطع)
        Result<FileManifest> store_file(const std::string &name,
                                        const Buffer &data);

        // (AR) تخزين قطعة واحدة
        Result<void> store_chunk(const Chunk &chunk);

        // ─── استرجاع ───

        // (AR) استرجاع ملف كامل من البيان
        Result<Buffer> retrieve_file(const FileManifest &manifest);

        // (AR) استرجاع قطعة واحدة
        Result<Chunk> get_chunk(const ContentId &id) const;

        // (AR) هل القطعة موجودة محلياً؟
        bool has_chunk(const ContentId &id) const;

        // ─── إدارة ───

        // (AR) حذف ملف والقطع المرتبطة
        Result<void> remove_file(const ContentId &root_id);

        // (AR) قائمة الملفات المخزنة
        std::vector<FileManifest> list_files() const;

        // (AR) الحجم الإجمالي المستخدم (بالبايت)
        uint64_t used_space() const;

        // (AR) تنظيف القطع اليتيمة (بدون بيان)
        void garbage_collect();

        // (AR) التحقق من سلامة البيانات
        bool verify_chunk(const Chunk &chunk) const;
        bool verify_manifest(const FileManifest &manifest) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::storage

#endif // SAD_SADNET_STORAGE_STORAGE_H
