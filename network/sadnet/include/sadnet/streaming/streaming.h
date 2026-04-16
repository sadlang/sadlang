// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة البث المباشر (Live Streaming)
// (AR) بث P2P مباشر مع شبكة شجرية تكيفية
//      قطع بث، معدل بت تكيفي، شبكة mesh، إحصائيات
// (EN) P2P live streaming: chunk-based, adaptive bitrate, mesh tree// المكون: network/sadnet (البث المباشر P2P)
// المسار: network/sadnet/include/sadnet/streaming/streaming.h// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_STREAMING_STREAMING_H
#define SAD_SADNET_STREAMING_STREAMING_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace sad::net::streaming
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ثوابت البث
    // ═════════════════════════════════════════════════════════════════════

    constexpr uint32_t DEFAULT_CHUNK_DURATION_MS = 2000; // مدة القطعة (مللي ثانية)
    constexpr uint32_t MAX_CHUNK_SIZE = 512 * 1024;      // 512KB حد أقصى
    constexpr uint32_t MAX_BITRATE_LEVELS = 8;
    constexpr uint32_t DEFAULT_BUFFER_CHUNKS = 5;      // مخزن مؤقت = 5 قطع
    constexpr uint32_t PEER_HEALTH_INTERVAL_MS = 5000; // فحص صحة كل 5 ثوانٍ

    // ═════════════════════════════════════════════════════════════════════
    // (AR) جودة البث — مستويات معدل البت
    // ═════════════════════════════════════════════════════════════════════

    struct QualityLevel
    {
        uint32_t bitrate; // بالبت/ثانية
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t fps = 30;
        std::string codec; // مثال "H264", "VP9", "AV1"
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) قطعة بث — وحدة النقل الأساسية
    // ═════════════════════════════════════════════════════════════════════

    struct StreamChunk
    {
        uint64_t stream_id = 0;
        uint64_t sequence = 0;      // رقم تسلسلي
        uint32_t quality_index = 0; // فهرس مستوى الجودة
        uint64_t timestamp_ms = 0;  // طابع زمني
        bool is_keyframe = false;   // هل هي إطار مفتاحي؟
        Buffer data;
        Buffer hash; // SHA-256 للتحقق من السلامة

        // (AR) حساب الهاش
        void compute_hash();

        // (AR) التحقق من السلامة
        bool verify() const;

        Buffer serialize() const;
        static StreamChunk deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) معلومات البث
    // ═════════════════════════════════════════════════════════════════════

    struct StreamInfo
    {
        uint64_t stream_id = 0;
        NodeId publisher;
        std::string title;
        std::string description;
        uint64_t started_at = 0;
        std::vector<QualityLevel> quality_levels;
        bool is_live = true;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) إحصائيات القرين في البث
    // ═════════════════════════════════════════════════════════════════════

    struct PeerStreamStats
    {
        NodeId peer_id;
        uint32_t chunks_received = 0;
        uint32_t chunks_forwarded = 0;
        uint32_t chunks_dropped = 0;
        double avg_latency_ms = 0.0;
        uint32_t current_bitrate = 0;
        uint64_t last_chunk_time = 0;

        double reliability() const
        {
            uint32_t total = chunks_received + chunks_dropped;
            return total == 0 ? 1.0 : (double)chunks_received / total;
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ناشر البث — يبث المحتوى للشبكة
    // ═════════════════════════════════════════════════════════════════════

    class StreamPublisher
    {
    public:
        StreamPublisher(const NodeId &self_id, uint64_t stream_id);
        ~StreamPublisher();

        // (AR) إعداد مستويات الجودة
        void set_quality_levels(const std::vector<QualityLevel> &levels);

        // (AR) نشر قطعة جديدة
        void publish_chunk(const Buffer &data, uint32_t quality_index, bool keyframe);

        // (AR) الحصول على معلومات البث
        StreamInfo get_info() const;

        // (AR) عدد المشاهدين المتصلين
        uint32_t viewer_count() const;

        // (AR) إضافة مشاهد مباشر
        void add_viewer(const NodeId &viewer);
        void remove_viewer(const NodeId &viewer);

        // (AR) إيقاف البث
        void stop();

        // (AR) استدعاء عند نشر قطعة (لإرسالها عبر الشبكة)
        using ChunkCallback = std::function<void(const StreamChunk &, const NodeId &target)>;
        void on_chunk_ready(ChunkCallback cb);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مشاهد البث — يستقبل ويعيد توزيع المحتوى
    // ═════════════════════════════════════════════════════════════════════

    class StreamViewer
    {
    public:
        StreamViewer(const NodeId &self_id, uint64_t stream_id,
                     uint32_t buffer_chunks = DEFAULT_BUFFER_CHUNKS);
        ~StreamViewer();

        // (AR) استقبال قطعة من الشبكة
        void receive_chunk(const StreamChunk &chunk, const NodeId &from);

        // (AR) الحصول على القطعة التالية للعرض
        bool next_chunk(StreamChunk &out);

        // (AR) التكيف التلقائي مع سعة النطاق
        uint32_t get_optimal_quality() const;
        void set_preferred_quality(uint32_t index);

        // (AR) إحصائيات
        PeerStreamStats get_stats() const;

        // (AR) قائمة الأقران المصدرين
        void add_source_peer(const NodeId &peer);
        void remove_source_peer(const NodeId &peer);

        // (AR) استدعاء عند جاهزية قطعة للعرض
        using PlaybackCallback = std::function<void(const StreamChunk &)>;
        void on_playback_ready(PlaybackCallback cb);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير البث — تنسيق النشر والمشاهدة
    // ═════════════════════════════════════════════════════════════════════

    class StreamManager
    {
    public:
        StreamManager(const NodeId &self_id);
        ~StreamManager();

        // (AR) إنشاء بث جديد
        uint64_t create_stream(const std::string &title,
                               const std::vector<QualityLevel> &levels);

        // (AR) الانضمام لبث موجود
        void join_stream(uint64_t stream_id, const NodeId &publisher);

        // (AR) إيقاف بث أو مغادرة
        void stop_stream(uint64_t stream_id);
        void leave_stream(uint64_t stream_id);

        // (AR) نشر بيانات لبث نشط
        void publish_data(uint64_t stream_id, const Buffer &data,
                          uint32_t quality, bool keyframe);

        // (AR) معالجة قطعة واردة
        void handle_incoming_chunk(const StreamChunk &chunk, const NodeId &from);

        // (AR) قائمة البثوث النشطة
        std::vector<StreamInfo> list_streams() const;

        // (AR) إحصائيات بث محدد
        PeerStreamStats get_stream_stats(uint64_t stream_id) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::streaming

#endif // SAD_SADNET_STREAMING_STREAMING_H
