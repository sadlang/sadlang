// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة البث المباشر — التنفيذ
// (AR) ناشر بث مع قطع، مشاهد بمخزن تكيفي، مدير مركزي
// (EN) StreamPublisher, StreamViewer, StreamManager implementation
// المكون: network/sadnet (البث المباشر P2P)
// المسار: network/sadnet/src/streaming/streaming.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/streaming/streaming.h"
#include <algorithm>
#include <cstring>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace sad::net::streaming
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) قطعة البث — حساب الهاش والتحقق
    // ═════════════════════════════════════════════════════════════════════

    void StreamChunk::compute_hash()
    {
        auto h = crypto::sha256(data.data(), data.size());
        hash = Buffer(h.data(), h.size());
    }

    bool StreamChunk::verify() const
    {
        if (hash.empty() || data.empty())
            return false;
        auto computed = crypto::sha256(data.data(), data.size());
        if (hash.size() != computed.size())
            return false;
        return std::memcmp(computed.data(), hash.data(), hash.size()) == 0;
    }

    Buffer StreamChunk::serialize() const
    {
        Buffer out;
        // stream_id (8) + sequence (8) + quality_index (4) + timestamp (8) +
        // is_keyframe (1) + data_len (4) + data + hash_len (2) + hash
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(stream_id >> (i * 8)));
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(sequence >> (i * 8)));
        for (int i = 0; i < 4; ++i)
            out.push_back((uint8_t)(quality_index >> (i * 8)));
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(timestamp_ms >> (i * 8)));
        out.push_back(is_keyframe ? 1 : 0);

        uint32_t dlen = (uint32_t)data.size();
        for (int i = 0; i < 4; ++i)
            out.push_back((uint8_t)(dlen >> (i * 8)));
        out.insert(out.end(), data.begin(), data.end());

        uint16_t hlen = (uint16_t)hash.size();
        out.push_back((uint8_t)(hlen & 0xFF));
        out.push_back((uint8_t)(hlen >> 8));
        out.insert(out.end(), hash.begin(), hash.end());
        return out;
    }

    StreamChunk StreamChunk::deserialize(const uint8_t *d, size_t len)
    {
        StreamChunk c;
        if (len < 8 + 8 + 4 + 8 + 1 + 4 + 2)
            return c;
        size_t off = 0;

        c.stream_id = 0;
        for (int i = 0; i < 8; ++i)
            c.stream_id |= ((uint64_t)d[off++]) << (i * 8);
        c.sequence = 0;
        for (int i = 0; i < 8; ++i)
            c.sequence |= ((uint64_t)d[off++]) << (i * 8);
        c.quality_index = 0;
        for (int i = 0; i < 4; ++i)
            c.quality_index |= ((uint32_t)d[off++]) << (i * 8);
        c.timestamp_ms = 0;
        for (int i = 0; i < 8; ++i)
            c.timestamp_ms |= ((uint64_t)d[off++]) << (i * 8);
        c.is_keyframe = (d[off++] != 0);

        uint32_t dlen = 0;
        for (int i = 0; i < 4; ++i)
            dlen |= ((uint32_t)d[off++]) << (i * 8);
        if (off + dlen > len)
            return c;
        c.data = Buffer(d + off, dlen);
        off += dlen;

        if (off + 2 > len)
            return c;
        uint16_t hlen = (uint16_t)(d[off] | (d[off + 1] << 8));
        off += 2;
        if (off + hlen <= len)
        {
            c.hash = Buffer(d + off, hlen);
        }
        return c;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ناشر البث — التنفيذ
    // ═════════════════════════════════════════════════════════════════════

    struct StreamPublisher::Impl
    {
        NodeId self_id;
        StreamInfo info;
        uint64_t next_seq = 0;
        mutable std::mutex mtx;
        std::unordered_set<NodeId> viewers;
        ChunkCallback chunk_cb;
        bool stopped = false;

        Impl(const NodeId &id, uint64_t sid)
            : self_id(id)
        {
            info.stream_id = sid;
            info.publisher = id;
            info.started_at = now_ms();
            info.is_live = true;
        }
    };

    StreamPublisher::StreamPublisher(const NodeId &self_id, uint64_t stream_id)
        : impl_(std::make_unique<Impl>(self_id, stream_id)) {}

    StreamPublisher::~StreamPublisher() = default;

    void StreamPublisher::set_quality_levels(const std::vector<QualityLevel> &levels)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->info.quality_levels = levels;
    }

    void StreamPublisher::publish_chunk(const Buffer &data, uint32_t quality_index, bool keyframe)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        if (impl_->stopped)
            return;

        StreamChunk chunk;
        chunk.stream_id = impl_->info.stream_id;
        chunk.sequence = impl_->next_seq++;
        chunk.quality_index = quality_index;
        chunk.timestamp_ms = now_ms();
        chunk.is_keyframe = keyframe;
        chunk.data = data;
        chunk.compute_hash();

        // (AR) إرسال لكل المشاهدين
        if (impl_->chunk_cb)
        {
            for (auto &viewer : impl_->viewers)
            {
                impl_->chunk_cb(chunk, viewer);
            }
        }
    }

    StreamInfo StreamPublisher::get_info() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return impl_->info;
    }

    uint32_t StreamPublisher::viewer_count() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return (uint32_t)impl_->viewers.size();
    }

    void StreamPublisher::add_viewer(const NodeId &viewer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->viewers.insert(viewer);
    }

    void StreamPublisher::remove_viewer(const NodeId &viewer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->viewers.erase(viewer);
    }

    void StreamPublisher::stop()
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->stopped = true;
        impl_->info.is_live = false;
        impl_->viewers.clear();
    }

    void StreamPublisher::on_chunk_ready(ChunkCallback cb)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->chunk_cb = std::move(cb);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مشاهد البث — التنفيذ
    // ═════════════════════════════════════════════════════════════════════

    struct StreamViewer::Impl
    {
        NodeId self_id;
        uint64_t stream_id;
        uint32_t buffer_size;
        mutable std::mutex mtx;

        // (AR) مخزن مؤقت مرتب حسب الرقم التسلسلي
        std::deque<StreamChunk> buffer;
        uint64_t next_play_seq = 0;

        // (AR) أقران مصدرين
        std::unordered_set<NodeId> sources;

        // (AR) إحصائيات
        PeerStreamStats stats;

        // (AR) معدل البت التكيفي
        uint32_t preferred_quality = 0;
        std::deque<double> latency_samples; // آخر 20 عينة
        static constexpr size_t MAX_SAMPLES = 20;

        PlaybackCallback playback_cb;

        Impl(const NodeId &id, uint64_t sid, uint32_t buf)
            : self_id(id), stream_id(sid), buffer_size(buf)
        {
            stats.peer_id = id;
        }

        // (AR) إدراج قطعة في المخزن المرتب
        void insert_sorted(const StreamChunk &chunk)
        {
            // تجاهل القطع القديمة
            if (chunk.sequence < next_play_seq)
                return;

            // التحقق من عدم التكرار
            for (auto &c : buffer)
            {
                if (c.sequence == chunk.sequence)
                    return;
            }

            // إدراج مرتب
            auto it = std::lower_bound(buffer.begin(), buffer.end(), chunk,
                                       [](const StreamChunk &a, const StreamChunk &b)
                                       {
                                           return a.sequence < b.sequence;
                                       });
            buffer.insert(it, chunk);

            // (AR) تقليم المخزن إذا تجاوز الحد
            while (buffer.size() > buffer_size * 3)
            {
                buffer.pop_front();
                if (!buffer.empty())
                    next_play_seq = buffer.front().sequence;
            }
        }

        // (AR) تحديث معدل الكمون
        void update_latency(double lat)
        {
            latency_samples.push_back(lat);
            if (latency_samples.size() > MAX_SAMPLES)
            {
                latency_samples.pop_front();
            }

            double sum = 0;
            for (double l : latency_samples)
                sum += l;
            stats.avg_latency_ms = sum / latency_samples.size();
        }
    };

    StreamViewer::StreamViewer(const NodeId &self_id, uint64_t stream_id,
                               uint32_t buffer_chunks)
        : impl_(std::make_unique<Impl>(self_id, stream_id, buffer_chunks)) {}

    StreamViewer::~StreamViewer() = default;

    void StreamViewer::receive_chunk(const StreamChunk &chunk, const NodeId &from)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) التحقق من السلامة
        if (!chunk.verify())
        {
            ++impl_->stats.chunks_dropped;
            return;
        }

        // (AR) تحديث الكمون
        double latency = (double)(now_ms() - chunk.timestamp_ms);
        if (latency > 0 && latency < 60000)
        { // أقل من دقيقة
            impl_->update_latency(latency);
        }

        impl_->insert_sorted(chunk);
        ++impl_->stats.chunks_received;
        impl_->stats.last_chunk_time = now_ms();

        // (AR) استدعاء العرض إذا المخزن ممتلئ
        if (impl_->playback_cb && impl_->buffer.size() >= impl_->buffer_size)
        {
            if (!impl_->buffer.empty() && impl_->buffer.front().sequence == impl_->next_play_seq)
            {
                impl_->playback_cb(impl_->buffer.front());
            }
        }
    }

    bool StreamViewer::next_chunk(StreamChunk &out)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        if (impl_->buffer.empty())
            return false;

        // (AR) أخذ أقدم قطعة متسلسلة
        if (impl_->buffer.front().sequence == impl_->next_play_seq ||
            impl_->next_play_seq == 0)
        {
            out = impl_->buffer.front();
            impl_->buffer.pop_front();
            impl_->next_play_seq = out.sequence + 1;
            return true;
        }

        // (AR) فجوة — تخطي إلى أقرب keyframe
        for (auto it = impl_->buffer.begin(); it != impl_->buffer.end(); ++it)
        {
            if (it->is_keyframe)
            {
                out = *it;
                impl_->next_play_seq = out.sequence + 1;
                impl_->buffer.erase(impl_->buffer.begin(), it + 1);
                return true;
            }
        }

        return false;
    }

    uint32_t StreamViewer::get_optimal_quality() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) خوارزمية التكيف: كمون منخفض + موثوقية عالية = جودة أعلى
        double avg_lat = impl_->stats.avg_latency_ms;
        double reliability = impl_->stats.reliability();

        // (AR) صيغة بسيطة: كمون < 100ms وموثوقية > 95% = أعلى جودة
        if (avg_lat < 100 && reliability > 0.95)
            return 0; // أعلى
        if (avg_lat < 300 && reliability > 0.85)
            return 1; // عالي
        if (avg_lat < 500 && reliability > 0.70)
            return 2; // متوسط
        return 3;     // منخفض
    }

    void StreamViewer::set_preferred_quality(uint32_t index)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->preferred_quality = index;
    }

    PeerStreamStats StreamViewer::get_stats() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return impl_->stats;
    }

    void StreamViewer::add_source_peer(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->sources.insert(peer);
    }

    void StreamViewer::remove_source_peer(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->sources.erase(peer);
    }

    void StreamViewer::on_playback_ready(PlaybackCallback cb)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->playback_cb = std::move(cb);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير البث — التنفيذ
    // ═════════════════════════════════════════════════════════════════════

    struct StreamManager::Impl
    {
        NodeId self_id;
        mutable std::mutex mtx;
        uint64_t next_stream_id = 1;

        std::unordered_map<uint64_t, std::unique_ptr<StreamPublisher>> publishers;
        std::unordered_map<uint64_t, std::unique_ptr<StreamViewer>> viewers;
        std::unordered_map<uint64_t, StreamInfo> known_streams;

        Impl(const NodeId &id) : self_id(id)
        {
            // (AR) بذرة عشوائية لمعرّفات البث
            uint8_t seed_bytes[8];
            crypto::random_bytes(seed_bytes, 8);
            next_stream_id = 0;
            for (int i = 0; i < 8; ++i)
                next_stream_id |= ((uint64_t)seed_bytes[i]) << (i * 8);
            if (next_stream_id == 0)
                next_stream_id = 1;
        }
    };

    StreamManager::StreamManager(const NodeId &self_id)
        : impl_(std::make_unique<Impl>(self_id)) {}

    StreamManager::~StreamManager() = default;

    uint64_t StreamManager::create_stream(const std::string &title,
                                          const std::vector<QualityLevel> &levels)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        uint64_t sid = impl_->next_stream_id++;
        auto pub = std::make_unique<StreamPublisher>(impl_->self_id, sid);
        pub->set_quality_levels(levels);

        StreamInfo info = pub->get_info();
        info.title = title;
        info.quality_levels = levels;
        impl_->known_streams[sid] = info;
        impl_->publishers[sid] = std::move(pub);

        return sid;
    }

    void StreamManager::join_stream(uint64_t stream_id, const NodeId &publisher)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        if (impl_->viewers.count(stream_id))
            return; // مشترك بالفعل

        auto viewer = std::make_unique<StreamViewer>(impl_->self_id, stream_id);
        viewer->add_source_peer(publisher);
        impl_->viewers[stream_id] = std::move(viewer);
    }

    void StreamManager::stop_stream(uint64_t stream_id)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        auto it = impl_->publishers.find(stream_id);
        if (it != impl_->publishers.end())
        {
            it->second->stop();
            impl_->publishers.erase(it);
        }
        impl_->known_streams.erase(stream_id);
    }

    void StreamManager::leave_stream(uint64_t stream_id)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->viewers.erase(stream_id);
    }

    void StreamManager::publish_data(uint64_t stream_id, const Buffer &data,
                                     uint32_t quality, bool keyframe)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        auto it = impl_->publishers.find(stream_id);
        if (it == impl_->publishers.end())
            return;
        it->second->publish_chunk(data, quality, keyframe);
    }

    void StreamManager::handle_incoming_chunk(const StreamChunk &chunk, const NodeId &from)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        auto it = impl_->viewers.find(chunk.stream_id);
        if (it != impl_->viewers.end())
        {
            it->second->receive_chunk(chunk, from);
        }
    }

    std::vector<StreamInfo> StreamManager::list_streams() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        std::vector<StreamInfo> result;
        result.reserve(impl_->known_streams.size());
        for (auto &[_, info] : impl_->known_streams)
        {
            result.push_back(info);
        }
        return result;
    }

    PeerStreamStats StreamManager::get_stream_stats(uint64_t stream_id) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        auto it = impl_->viewers.find(stream_id);
        if (it != impl_->viewers.end())
        {
            return it->second->get_stats();
        }
        return PeerStreamStats{};
    }

} // namespace sad::net::streaming
