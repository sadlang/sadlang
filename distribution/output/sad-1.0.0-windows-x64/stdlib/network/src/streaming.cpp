// ===================================================================
// تدفق_تحميل - التحميل المتدفق
// streaming.cpp - Streaming Download Support
// ===================================================================
// يوفر دعم التحميل المتدفق للملفات الكبيرة:
// - تحميل بأجزاء (Chunked)
// - تتبع التقدم
// - استئناف التحميل
// ===================================================================

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <cstdint>
#include <fstream>

namespace sad {
namespace stdlib {
namespace network {

/// حالة التحميل
enum class DownloadState {
    Idle,       // خامل
    Connecting, // جاري الاتصال
    Downloading,// جاري التحميل
    Paused,     // مُعلّق
    Completed,  // مكتمل
    Failed,     // فشل
    Cancelled,  // مُلغى
};

/// معلومات التقدم
struct StreamProgress {
    size_t bytes_downloaded;    // البايتات المحمّلة
    size_t total_bytes;         // الحجم الكلي (0 إذا غير معروف)
    double speed_bps;           // السرعة (بايت/ثانية)
    double elapsed_seconds;     // الوقت المنقضي
    
    /// نسبة الاكتمال (0-100)
    double percentage() const {
        if (total_bytes == 0) return 0.0;
        return (static_cast<double>(bytes_downloaded) / total_bytes) * 100.0;
    }
    
    /// الوقت المتبقي المقدر (ثواني)
    double estimatedRemaining() const {
        if (speed_bps <= 0 || total_bytes == 0) return -1.0;
        size_t remaining = total_bytes - bytes_downloaded;
        return static_cast<double>(remaining) / speed_bps;
    }
};

/// إعدادات التحميل المتدفق
struct StreamConfig {
    size_t chunk_size = 65536;          // حجم الجزء (64KB افتراضياً)
    bool enable_resume = true;          // السماح بالاستئناف
    size_t max_retries = 3;             // أقصى عدد محاولات
    size_t retry_delay_ms = 1000;       // تأخير بين المحاولات
    std::string output_path;            // مسار الإخراج
    bool verify_checksum = false;       // التحقق من المجموع التدقيقي
    std::string expected_checksum;      // المجموع التدقيقي المتوقع
};

/// مُعالج البث المتدفق
class StreamDownloader {
public:
    /// استدعاءات الأحداث
    using ProgressCallback = std::function<void(const StreamProgress&)>;
    using ChunkCallback = std::function<void(const std::vector<uint8_t>&)>;
    using CompletionCallback = std::function<void(bool success, const std::string& error)>;
    
    /// إنشاء محمّل جديد
    StreamDownloader(const std::string& url, StreamConfig config = {})
        : url_(url), config_(std::move(config)) {}
    
    /// تحديد استدعاء التقدم
    StreamDownloader& عند_تقدم(ProgressCallback callback) {
        on_progress_ = std::move(callback);
        return *this;
    }
    
    /// تحديد استدعاء استقبال جزء
    StreamDownloader& عند_جزء(ChunkCallback callback) {
        on_chunk_ = std::move(callback);
        return *this;
    }
    
    /// تحديد استدعاء الاكتمال
    StreamDownloader& عند_اكتمال(CompletionCallback callback) {
        on_complete_ = std::move(callback);
        return *this;
    }
    
    /// بدء التحميل
    void ابدأ() {
        state_ = DownloadState::Connecting;
        // سيتم التنفيذ مع مكتبة HTTP الفعلية
        state_ = DownloadState::Downloading;
    }
    
    /// إيقاف مؤقت
    void أوقف() {
        if (state_ == DownloadState::Downloading) {
            state_ = DownloadState::Paused;
        }
    }
    
    /// استئناف
    void استأنف() {
        if (state_ == DownloadState::Paused) {
            state_ = DownloadState::Downloading;
        }
    }
    
    /// إلغاء
    void ألغِ() {
        state_ = DownloadState::Cancelled;
    }
    
    /// الحالة الحالية
    DownloadState state() const { return state_; }
    
    /// معلومات التقدم
    const StreamProgress& progress() const { return progress_; }

private:
    std::string url_;
    StreamConfig config_;
    DownloadState state_ = DownloadState::Idle;
    StreamProgress progress_{};
    ProgressCallback on_progress_;
    ChunkCallback on_chunk_;
    CompletionCallback on_complete_;
};

/// إنشاء محمّل متدفق
inline StreamDownloader تحميل_متدفق(const std::string& url) {
    return StreamDownloader(url);
}

} // namespace network
} // namespace stdlib
} // namespace sad
