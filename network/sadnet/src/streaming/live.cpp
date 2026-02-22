// ===================================================================
// صNet - البث المباشر للفيديو والصوت
// المرحلة 53: بثّ لامركزي بين الأقران
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <deque>
#include <functional>

namespace sad::network {

/// نوع الوسائط
enum class MediaType : uint8_t {
    VIDEO,      // فيديو
    AUDIO,      // صوت
    SCREEN      // مشاركة شاشة
};

/// برنامج ترميز الصوت/الفيديو
enum class Codec : uint8_t {
    H264,       // فيديو H.264
    H265,       // فيديو H.265/HEVC
    VP9,        // فيديو VP9
    AV1,        // فيديو AV1
    OPUS,       // صوت Opus
    AAC         // صوت AAC
};

/// جودة البث
struct StreamQuality {
    uint32_t width, height;    // الدقة
    uint32_t bitrate_kbps;     // معدل البت (كيلوبت/ثانية)
    uint8_t fps;               // الإطارات في الثانية
    Codec codec;               // البرنامج الترميزي
};

/// قطعة وسائط واحدة
struct MediaChunk {
    uint64_t sequence;             // رقم التسلسل
    uint64_t timestamp_ms;         // الطابع الزمني
    MediaType type;                // نوع الوسائط
    std::vector<uint8_t> data;     // البيانات المُرمّزة
    bool is_keyframe;              // هل هو إطار مفتاحي؟
    uint32_t duration_ms;          // المدة بالمللي ثانية
};

/// معلومات بث مباشر
struct LiveStream {
    std::string stream_id;         // معرّف البث الفريد
    std::string broadcaster_id;    // معرّف المُذيع
    std::string title;             // عنوان البث
    MediaType type;                // نوع المحتوى
    StreamQuality quality;         // جودة البث
    uint64_t started_at;           // وقت بدء البث
    uint32_t viewer_count;         // عدد المشاهدين
    bool is_live;                  // هل البث مباشر الآن؟
};

/// مخزن مؤقت للقطع - تخفيف التقطّع
class ChunkBuffer {
    std::deque<MediaChunk> buffer_;    // قائمة القطع
    size_t max_chunks_;                // الحد الأقصى
    uint64_t next_expected_;           // القطعة المتوقعة التالية

public:
    explicit ChunkBuffer(size_t max = 30)
        : max_chunks_(max), next_expected_(0) {}

    /// إضافة قطعة للمخزن المؤقت
    void push(MediaChunk chunk) {
        buffer_.push_back(std::move(chunk));
        if (buffer_.size() > max_chunks_) {
            buffer_.pop_front();
        }
    }

    /// استخراج القطعة التالية للعرض
    MediaChunk pop() {
        if (buffer_.empty()) return {};
        auto chunk = std::move(buffer_.front());
        buffer_.pop_front();
        next_expected_ = chunk.sequence + 1;
        return chunk;
    }

    size_t size() const { return buffer_.size(); }
    bool ready() const { return buffer_.size() >= 3; }
};

/// شبكة توزيع البث بين الأقران
class PeerMesh {
    std::string stream_id_;                            // معرّف البث
    std::map<std::string, bool> peers_;                // الأقران المتصلون
    uint32_t max_upload_peers_;                        // أقصى عدد للرفع

public:
    explicit PeerMesh(const std::string& stream_id, uint32_t max_up = 5)
        : stream_id_(stream_id), max_upload_peers_(max_up) {}

    /// إضافة قرين للشبكة
    void add_peer(const std::string& peer_id) {
        if (peers_.size() < max_upload_peers_ * 3) {
            peers_[peer_id] = true;
        }
    }

    /// إزالة قرين من الشبكة
    void remove_peer(const std::string& peer_id) {
        peers_.erase(peer_id);
    }

    /// توزيع قطعة على الأقران
    void distribute_chunk(const MediaChunk& chunk) {
        for (const auto& [peer_id, active] : peers_) {
            if (active) {
                // إرسال القطعة للقرين عبر WebRTC أو UDP
                (void)chunk;
            }
        }
    }

    size_t peer_count() const { return peers_.size(); }
};

/// محرك البث المباشر
class LiveStreamEngine {
    std::map<std::string, LiveStream> active_streams_;  // البثوث النشطة
    std::map<std::string, ChunkBuffer> buffers_;        // المخازن المؤقتة
    std::map<std::string, std::unique_ptr<PeerMesh>> meshes_; // شبكات التوزيع

public:
    /// بدء بث مباشر جديد
    std::string start_broadcast(const std::string& broadcaster_id,
                                 const std::string& title,
                                 const StreamQuality& quality) {
        LiveStream stream;
        stream.stream_id = "stream_" + broadcaster_id;
        stream.broadcaster_id = broadcaster_id;
        stream.title = title;
        stream.quality = quality;
        stream.type = MediaType::VIDEO;
        stream.viewer_count = 0;
        stream.is_live = true;

        active_streams_[stream.stream_id] = stream;
        meshes_[stream.stream_id] =
            std::make_unique<PeerMesh>(stream.stream_id);
        return stream.stream_id;
    }

    /// إرسال قطعة وسائط من المُذيع
    void push_chunk(const std::string& stream_id, MediaChunk chunk) {
        auto mesh_it = meshes_.find(stream_id);
        if (mesh_it != meshes_.end()) {
            mesh_it->second->distribute_chunk(chunk);
        }
    }

    /// الانضمام كمشاهد
    bool join_stream(const std::string& stream_id,
                      const std::string& viewer_id) {
        auto it = active_streams_.find(stream_id);
        if (it == active_streams_.end() || !it->second.is_live) return false;
        it->second.viewer_count++;
        buffers_[viewer_id] = ChunkBuffer(30);
        meshes_[stream_id]->add_peer(viewer_id);
        return true;
    }

    /// إيقاف البث
    void stop_broadcast(const std::string& stream_id) {
        auto it = active_streams_.find(stream_id);
        if (it != active_streams_.end()) {
            it->second.is_live = false;
        }
    }
};

} // namespace sad::network
