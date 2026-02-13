/**
 * ==========================================================
 * ملف: voice_chat.cpp
 * الوصف: الدردشة الصوتية للتعاون
 * المهمة: T310 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🎙️ ما هي الدردشة الصوتية؟
 * -------------------------
 * تتحدث مع زملائك بالصوت أثناء البرمجة!
 * مثل مكالمة هاتفية لكن داخل محرر الكود.
 * 
 * 📝 مثال:
 * --------
 * ص شراكة صوت --ابدأ
 * "مرحباً، هل تسمعني؟"
 * "نعم! دعنا نعمل على الدالة في السطر 50"
 */

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <chrono>

namespace sad {
namespace collab {

// ==========================================================
// 📌 إعدادات الصوت
// ==========================================================

/**
 * 🎛️ إعدادات الصوت
 */
struct AudioSettings {
    int sampleRate = 48000;     // معدل العينات
    int channels = 1;           // عدد القنوات (مونو)
    int bitsPerSample = 16;     // بت لكل عينة
    int frameSize = 960;        // حجم الإطار (20ms @ 48kHz)
    bool noiseSuppression = true;
    bool echoCancellation = true;
    
    int bytesPerFrame() const {
        return frameSize * channels * (bitsPerSample / 8);
    }
};

// ==========================================================
// 📌 إطار صوتي
// ==========================================================

/**
 * 🎵 إطار صوتي
 */
struct AudioFrame {
    uint32_t senderId;
    uint64_t timestamp;
    uint32_t sequenceNumber;
    std::vector<uint8_t> data;
    bool isSilence = false;
    
    /**
     * تحويل لـ bytes
     */
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> result;
        
        // senderId
        result.push_back(senderId & 0xFF);
        result.push_back((senderId >> 8) & 0xFF);
        result.push_back((senderId >> 16) & 0xFF);
        result.push_back((senderId >> 24) & 0xFF);
        
        // timestamp
        for (int i = 0; i < 8; ++i) {
            result.push_back((timestamp >> (i * 8)) & 0xFF);
        }
        
        // sequence
        result.push_back(sequenceNumber & 0xFF);
        result.push_back((sequenceNumber >> 8) & 0xFF);
        result.push_back((sequenceNumber >> 16) & 0xFF);
        result.push_back((sequenceNumber >> 24) & 0xFF);
        
        // isSilence
        result.push_back(isSilence ? 1 : 0);
        
        // data length
        uint32_t len = static_cast<uint32_t>(data.size());
        result.push_back(len & 0xFF);
        result.push_back((len >> 8) & 0xFF);
        result.push_back((len >> 16) & 0xFF);
        result.push_back((len >> 24) & 0xFF);
        
        // data
        result.insert(result.end(), data.begin(), data.end());
        
        return result;
    }
    
    /**
     * من bytes
     */
    static AudioFrame fromBytes(const uint8_t* bytes, size_t size) {
        AudioFrame frame;
        size_t offset = 0;
        
        frame.senderId = bytes[offset] | (bytes[offset+1] << 8) |
                        (bytes[offset+2] << 16) | (bytes[offset+3] << 24);
        offset += 4;
        
        frame.timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            frame.timestamp |= static_cast<uint64_t>(bytes[offset++]) << (i * 8);
        }
        
        frame.sequenceNumber = bytes[offset] | (bytes[offset+1] << 8) |
                              (bytes[offset+2] << 16) | (bytes[offset+3] << 24);
        offset += 4;
        
        frame.isSilence = bytes[offset++] != 0;
        
        uint32_t len = bytes[offset] | (bytes[offset+1] << 8) |
                      (bytes[offset+2] << 16) | (bytes[offset+3] << 24);
        offset += 4;
        
        if (offset + len <= size) {
            frame.data.assign(bytes + offset, bytes + offset + len);
        }
        
        return frame;
    }
};

// ==========================================================
// 📌 حالة المشارك الصوتي
// ==========================================================

/**
 * 👤 حالة مشارك صوتي
 */
struct VoiceParticipant {
    uint32_t userId;
    std::string username;
    bool isMuted = false;
    bool isDeafened = false;
    bool isSpeaking = false;
    float volume = 1.0f;        // مستوى الصوت (0.0 - 1.0)
    uint64_t lastActivity = 0;
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << userId << ",\n";
        oss << "  \"username\": \"" << username << "\",\n";
        oss << "  \"isMuted\": " << (isMuted ? "true" : "false") << ",\n";
        oss << "  \"isDeafened\": " << (isDeafened ? "true" : "false") << ",\n";
        oss << "  \"isSpeaking\": " << (isSpeaking ? "true" : "false") << ",\n";
        oss << "  \"volume\": " << volume << "\n";
        oss << "}";
        return oss.str();
    }
};

// ==========================================================
// 📌 مدير الدردشة الصوتية
// ==========================================================

/**
 * 🎙️ مدير الدردشة الصوتية
 */
class VoiceChatManager {
private:
    std::string sessionId_;
    AudioSettings settings_;
    std::map<uint32_t, VoiceParticipant> participants_;
    std::queue<AudioFrame> outgoingFrames_;
    std::queue<AudioFrame> incomingFrames_;
    std::mutex mutex_;
    
    std::atomic<bool> isActive_{false};
    std::atomic<bool> isMuted_{false};
    std::atomic<bool> isDeafened_{false};
    
    uint32_t localUserId_ = 0;
    uint32_t sequenceNumber_ = 0;
    
public:
    /**
     * إنشاء مدير
     */
    VoiceChatManager(const std::string& sessionId, uint32_t localUserId)
        : sessionId_(sessionId), localUserId_(localUserId) {}
    
    // ═══════════════════════════════════════════════════════
    // التحكم
    // ═══════════════════════════════════════════════════════
    
    /**
     * بدء الدردشة
     */
    bool start() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (isActive_) return false;
        
        // تهيئة الصوت (في التنفيذ الحقيقي)
        isActive_ = true;
        
        return true;
    }
    
    /**
     * إيقاف الدردشة
     */
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        isActive_ = false;
        
        // تنظيف الطوابير
        while (!outgoingFrames_.empty()) outgoingFrames_.pop();
        while (!incomingFrames_.empty()) incomingFrames_.pop();
    }
    
    /**
     * هل نشطة؟
     */
    bool isActive() const { return isActive_; }
    
    // ═══════════════════════════════════════════════════════
    // كتم الصوت
    // ═══════════════════════════════════════════════════════
    
    /**
     * كتم الميكروفون
     */
    void mute() { isMuted_ = true; }
    
    /**
     * إلغاء كتم الميكروفون
     */
    void unmute() { isMuted_ = false; }
    
    /**
     * تبديل الكتم
     */
    void toggleMute() { isMuted_ = !isMuted_; }
    
    /**
     * هل مكتوم؟
     */
    bool isMuted() const { return isMuted_; }
    
    /**
     * كتم الاستماع
     */
    void deafen() { isDeafened_ = true; }
    
    /**
     * إلغاء كتم الاستماع
     */
    void undeafen() { isDeafened_ = false; }
    
    /**
     * هل الاستماع مكتوم؟
     */
    bool isDeafened() const { return isDeafened_; }
    
    // ═══════════════════════════════════════════════════════
    // المشاركين
    // ═══════════════════════════════════════════════════════
    
    /**
     * إضافة مشارك
     */
    void addParticipant(uint32_t userId, const std::string& username) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        VoiceParticipant p;
        p.userId = userId;
        p.username = username;
        p.lastActivity = currentTime();
        
        participants_[userId] = p;
    }
    
    /**
     * إزالة مشارك
     */
    void removeParticipant(uint32_t userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        participants_.erase(userId);
    }
    
    /**
     * تعيين مستوى صوت مشارك
     */
    void setParticipantVolume(uint32_t userId, float volume) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = participants_.find(userId);
        if (it != participants_.end()) {
            it->second.volume = std::max(0.0f, std::min(1.0f, volume));
        }
    }
    
    /**
     * قائمة المشاركين
     */
    std::vector<VoiceParticipant> listParticipants() const {
        std::vector<VoiceParticipant> list;
        for (const auto& [id, p] : participants_) {
            list.push_back(p);
        }
        return list;
    }
    
    // ═══════════════════════════════════════════════════════
    // إرسال/استقبال
    // ═══════════════════════════════════════════════════════
    
    /**
     * إرسال إطار صوتي
     */
    void sendFrame(const uint8_t* data, size_t size) {
        if (!isActive_ || isMuted_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        AudioFrame frame;
        frame.senderId = localUserId_;
        frame.timestamp = currentTime();
        frame.sequenceNumber = sequenceNumber_++;
        frame.data.assign(data, data + size);
        
        // اكتشاف الصمت (مبسط)
        frame.isSilence = isSilent(data, size);
        
        outgoingFrames_.push(frame);
    }
    
    /**
     * استقبال إطار صوتي
     */
    void receiveFrame(const AudioFrame& frame) {
        if (!isActive_ || isDeafened_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // تحديث حالة المشارك
        auto it = participants_.find(frame.senderId);
        if (it != participants_.end()) {
            it->second.isSpeaking = !frame.isSilence;
            it->second.lastActivity = currentTime();
        }
        
        incomingFrames_.push(frame);
    }
    
    /**
     * الحصول على الإطار التالي للإرسال
     */
    bool getNextOutgoingFrame(AudioFrame& frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (outgoingFrames_.empty()) return false;
        
        frame = outgoingFrames_.front();
        outgoingFrames_.pop();
        return true;
    }
    
    /**
     * الحصول على الإطار التالي للتشغيل
     */
    bool getNextIncomingFrame(AudioFrame& frame) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (incomingFrames_.empty()) return false;
        
        frame = incomingFrames_.front();
        incomingFrames_.pop();
        return true;
    }
    
    // ═══════════════════════════════════════════════════════
    // الإعدادات
    // ═══════════════════════════════════════════════════════
    
    const AudioSettings& settings() const { return settings_; }
    void setSettings(const AudioSettings& settings) { settings_ = settings; }
    
    // ═══════════════════════════════════════════════════════
    // التحويل
    // ═══════════════════════════════════════════════════════
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        
        oss << "{\n";
        oss << "  \"sessionId\": \"" << sessionId_ << "\",\n";
        oss << "  \"isActive\": " << (isActive_ ? "true" : "false") << ",\n";
        oss << "  \"isMuted\": " << (isMuted_ ? "true" : "false") << ",\n";
        oss << "  \"isDeafened\": " << (isDeafened_ ? "true" : "false") << ",\n";
        oss << "  \"participants\": [\n";
        
        bool first = true;
        for (const auto& [id, p] : participants_) {
            if (!first) oss << ",\n";
            oss << "    " << p.toJson();
            first = false;
        }
        
        oss << "\n  ]\n}";
        return oss.str();
    }
    
private:
    uint64_t currentTime() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    /**
     * اكتشاف الصمت
     */
    bool isSilent(const uint8_t* data, size_t size) {
        // حساب مستوى الصوت المتوسط
        int32_t sum = 0;
        const int16_t* samples = reinterpret_cast<const int16_t*>(data);
        size_t numSamples = size / sizeof(int16_t);
        
        for (size_t i = 0; i < numSamples; ++i) {
            sum += std::abs(samples[i]);
        }
        
        int32_t avg = numSamples > 0 ? sum / numSamples : 0;
        
        // عتبة الصمت
        return avg < 500; // قيمة تجريبية
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مدير الدردشة الصوتية
 */
void* sad_voice_chat_new(const char* sessionId, uint32_t userId) {
    return new sad::collab::VoiceChatManager(sessionId, userId);
}

/**
 * تحرير المدير
 */
void sad_voice_chat_free(void* manager) {
    delete static_cast<sad::collab::VoiceChatManager*>(manager);
}

/**
 * بدء الدردشة
 */
int sad_voice_chat_start(void* manager) {
    return static_cast<sad::collab::VoiceChatManager*>(manager)->start() ? 1 : 0;
}

/**
 * إيقاف الدردشة
 */
void sad_voice_chat_stop(void* manager) {
    static_cast<sad::collab::VoiceChatManager*>(manager)->stop();
}

/**
 * كتم/إلغاء كتم
 */
void sad_voice_chat_mute(void* manager) {
    static_cast<sad::collab::VoiceChatManager*>(manager)->mute();
}

void sad_voice_chat_unmute(void* manager) {
    static_cast<sad::collab::VoiceChatManager*>(manager)->unmute();
}

/**
 * إضافة مشارك
 */
void sad_voice_chat_add_participant(void* manager, uint32_t userId,
                                     const char* username) {
    auto* mgr = static_cast<sad::collab::VoiceChatManager*>(manager);
    mgr->addParticipant(userId, username);
}

/**
 * إرسال إطار صوتي
 */
void sad_voice_chat_send(void* manager, const uint8_t* data, size_t size) {
    auto* mgr = static_cast<sad::collab::VoiceChatManager*>(manager);
    mgr->sendFrame(data, size);
}

/**
 * الحصول على JSON
 */
char* sad_voice_chat_to_json(void* manager) {
    auto* mgr = static_cast<sad::collab::VoiceChatManager*>(manager);
    std::string json = mgr->toJson();
    
    char* result = new char[json.size() + 1];
    std::strcpy(result, json.c_str());
    return result;
}

/**
 * تحرير نص
 */
void sad_voice_chat_free_string(char* str) {
    delete[] str;
}

} // extern "C"
