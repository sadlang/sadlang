// ═══════════════════════════════════════════════════════════════════
// صNet — تنفيذ التخزين الموزع
// (AR) تقسيم ملفات، Content Addressing، تخزين واسترجاع محلي
// (EN) Distributed storage: chunking, content addressing, local store
// المكون: network/sadnet (التخزين الموزع)
// المسار: network/sadnet/src/storage/storage.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/storage/storage.h"
#include <algorithm>
#include <cstring>
#include <mutex>
#include <set>

namespace sad::net::storage
{

    static void w16(uint8_t *p, uint16_t v)
    {
        p[0] = (uint8_t)(v >> 8);
        p[1] = (uint8_t)v;
    }
    static void w32(uint8_t *p, uint32_t v)
    {
        p[0] = (uint8_t)(v >> 24);
        p[1] = (uint8_t)(v >> 16);
        p[2] = (uint8_t)(v >> 8);
        p[3] = (uint8_t)v;
    }
    static void w64(uint8_t *p, uint64_t v)
    {
        w32(p, (uint32_t)(v >> 32));
        w32(p + 4, (uint32_t)v);
    }
    static uint16_t r16(const uint8_t *p) { return ((uint16_t)p[0] << 8) | p[1]; }
    static uint32_t r32(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    }
    static uint64_t r64(const uint8_t *p) { return ((uint64_t)r32(p) << 32) | r32(p + 4); }

    // ═══════════════════════════════════════════════════════════════════
    // Chunk ترميز: [id:32][index:4][data_len:4][data:N]
    // ═══════════════════════════════════════════════════════════════════

    Buffer Chunk::serialize() const
    {
        Buffer buf(NODE_ID_SIZE + 4 + 4 + data.size());
        uint8_t *p = buf.data();
        std::memcpy(p, id.data.data(), NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        w32(p, index);
        p += 4;
        w32(p, (uint32_t)data.size());
        p += 4;
        if (!data.empty())
            std::memcpy(p, data.data(), data.size());
        return buf;
    }

    Chunk Chunk::deserialize(const uint8_t *d, size_t len)
    {
        Chunk c;
        if (len < NODE_ID_SIZE + 8)
            return c;
        std::memcpy(c.id.data.data(), d, NODE_ID_SIZE);
        c.index = r32(d + NODE_ID_SIZE);
        uint32_t dl = r32(d + NODE_ID_SIZE + 4);
        if (NODE_ID_SIZE + 8 + dl <= len)
            c.data = Buffer(d + NODE_ID_SIZE + 8, dl);
        return c;
    }

    // ═══════════════════════════════════════════════════════════════════
    // FileManifest ترميز
    // [root_id:32][name_len:2][name:N][total_size:8]
    // [chunk_count:4][chunk_ids:32*N][owner:32][created:8][sig_len:2][sig:M]
    // ═══════════════════════════════════════════════════════════════════

    Buffer FileManifest::serialize() const
    {
        size_t total = NODE_ID_SIZE + 2 + name.size() + 8 + 4 +
                       NODE_ID_SIZE * chunks.size() + NODE_ID_SIZE + 8 + 2 + signature.size();
        Buffer buf(total);
        uint8_t *p = buf.data();

        std::memcpy(p, root_id.data.data(), NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        w16(p, (uint16_t)name.size());
        p += 2;
        std::memcpy(p, name.c_str(), name.size());
        p += name.size();
        w64(p, total_size);
        p += 8;
        w32(p, (uint32_t)chunks.size());
        p += 4;
        for (const auto &cid : chunks)
        {
            std::memcpy(p, cid.data.data(), NODE_ID_SIZE);
            p += NODE_ID_SIZE;
        }
        std::memcpy(p, owner.data.data(), NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        w64(p, created_at);
        p += 8;
        w16(p, (uint16_t)signature.size());
        p += 2;
        if (!signature.empty())
            std::memcpy(p, signature.data(), signature.size());

        return buf;
    }

    FileManifest FileManifest::deserialize(const uint8_t *d, size_t len)
    {
        FileManifest m;
        size_t off = 0;
        if (len < NODE_ID_SIZE + 2)
            return m;

        std::memcpy(m.root_id.data.data(), d, NODE_ID_SIZE);
        off += NODE_ID_SIZE;
        uint16_t nlen = r16(d + off);
        off += 2;
        if (off + nlen > len)
            return m;
        m.name.assign((const char *)(d + off), nlen);
        off += nlen;

        if (off + 12 > len)
            return m;
        m.total_size = r64(d + off);
        off += 8;
        uint32_t cc = r32(d + off);
        off += 4;

        for (uint32_t i = 0; i < cc && off + NODE_ID_SIZE <= len; i++)
        {
            ContentId cid;
            std::memcpy(cid.data.data(), d + off, NODE_ID_SIZE);
            off += NODE_ID_SIZE;
            m.chunks.push_back(cid);
        }

        if (off + NODE_ID_SIZE + 8 + 2 > len)
            return m;
        std::memcpy(m.owner.data.data(), d + off, NODE_ID_SIZE);
        off += NODE_ID_SIZE;
        m.created_at = r64(d + off);
        off += 8;
        uint16_t slen = r16(d + off);
        off += 2;
        if (off + slen <= len)
            m.signature = Buffer(d + off, slen);

        return m;
    }

    // ═══════════════════════════════════════════════════════════════════
    // مدير التخزين
    // ═══════════════════════════════════════════════════════════════════

    struct StorageManager::Impl
    {
        NodeId self_id;
        crypto::KeyPair keys;

        std::map<ContentId, Chunk> chunks;
        std::map<ContentId, FileManifest> manifests;
        mutable std::mutex mutex;

        Impl(const NodeId &id, const crypto::KeyPair &k) : self_id(id), keys(k) {}
    };

    StorageManager::StorageManager(const NodeId &self_id, const crypto::KeyPair &keys)
        : impl_(std::make_unique<Impl>(self_id, keys)) {}

    StorageManager::~StorageManager() = default;

    Result<FileManifest> StorageManager::store_file(const std::string &name,
                                                    const Buffer &data)
    {
        if (data.size() > MAX_FILE_SIZE)
            return Result<FileManifest>::fail(NetError::INTERNAL_ERROR);

        std::lock_guard<std::mutex> lock(impl_->mutex);

        FileManifest manifest;
        manifest.name = name;
        manifest.total_size = data.size();
        manifest.owner = impl_->self_id;
        manifest.created_at = now_ms();

        // تقسيم إلى قطع
        for (size_t offset = 0; offset < data.size(); offset += CHUNK_SIZE)
        {
            size_t chunk_len = std::min(CHUNK_SIZE, data.size() - offset);

            Chunk chunk;
            chunk.data = Buffer(data.data() + offset, chunk_len);
            chunk.index = (uint32_t)(offset / CHUNK_SIZE);
            chunk.stored_at = now_ms();
            chunk.compute_id();

            manifest.chunks.push_back(chunk.id);
            impl_->chunks[chunk.id] = std::move(chunk);
        }

        manifest.compute_root_id();

        // توقيع البيان
        auto ser = manifest.serialize();
        auto sig_hash = crypto::hmac_sha256(impl_->keys.private_key.data(), impl_->keys.private_key.size(),
                                            ser.data(), ser.size());
        manifest.signature = Buffer(sig_hash.data(), sig_hash.size());

        impl_->manifests[manifest.root_id] = manifest;
        return Result<FileManifest>::success(manifest);
    }

    Result<void> StorageManager::store_chunk(const Chunk &chunk)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->chunks[chunk.id] = chunk;
        return Result<void>::success();
    }

    Result<Buffer> StorageManager::retrieve_file(const FileManifest &manifest)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        Buffer result;

        for (const auto &cid : manifest.chunks)
        {
            auto it = impl_->chunks.find(cid);
            if (it == impl_->chunks.end())
                return Result<Buffer>::fail(NetError::NOT_FOUND);
            result.append(it->second.data.data(), it->second.data.size());
        }

        return Result<Buffer>::success(std::move(result));
    }

    Result<Chunk> StorageManager::get_chunk(const ContentId &id) const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = impl_->chunks.find(id);
        if (it != impl_->chunks.end())
            return Result<Chunk>::success(it->second);
        return Result<Chunk>::fail(NetError::NOT_FOUND);
    }

    bool StorageManager::has_chunk(const ContentId &id) const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        return impl_->chunks.count(id) > 0;
    }

    Result<void> StorageManager::remove_file(const ContentId &root_id)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = impl_->manifests.find(root_id);
        if (it == impl_->manifests.end())
            return Result<void>::fail(NetError::NOT_FOUND);

        for (const auto &cid : it->second.chunks)
            impl_->chunks.erase(cid);
        impl_->manifests.erase(it);
        return Result<void>::success();
    }

    std::vector<FileManifest> StorageManager::list_files() const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        std::vector<FileManifest> result;
        for (const auto &kv : impl_->manifests)
            result.push_back(kv.second);
        return result;
    }

    uint64_t StorageManager::used_space() const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        uint64_t total = 0;
        for (const auto &kv : impl_->chunks)
            total += kv.second.data.size();
        return total;
    }

    void StorageManager::garbage_collect()
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        // جمع معرفات القطع المرتبطة ببيانات
        std::set<ContentId> referenced;
        for (const auto &kv : impl_->manifests)
            for (const auto &cid : kv.second.chunks)
                referenced.insert(cid);

        // حذف غير المرجعية
        for (auto it = impl_->chunks.begin(); it != impl_->chunks.end();)
        {
            if (referenced.count(it->first) == 0)
                it = impl_->chunks.erase(it);
            else
                ++it;
        }
    }

    bool StorageManager::verify_chunk(const Chunk &chunk) const
    {
        auto hash = crypto::sha256(chunk.data);
        ContentId computed;
        std::memcpy(computed.data.data(), hash.data(), NODE_ID_SIZE);
        return computed == chunk.id;
    }

    bool StorageManager::verify_manifest(const FileManifest &manifest) const
    {
        // التحقق من root_id
        Buffer all_ids;
        for (const auto &cid : manifest.chunks)
            all_ids.append(cid.data.data(), NODE_ID_SIZE);
        auto hash = crypto::sha256(all_ids);
        ContentId computed;
        std::memcpy(computed.data.data(), hash.data(), NODE_ID_SIZE);
        return computed == manifest.root_id;
    }

} // namespace sad::net::storage
