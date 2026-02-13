/**
 * ==========================================================
 * ملف: session_manager.cpp
 * الوصف: إدارة جلسات التعاون
 * المهمة: T307 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🎛️ ما هو مدير الجلسات؟
 * -----------------------
 * برنامج يتتبع جميع جلسات التعاون النشطة.
 * مثل موظف الاستقبال في الفندق!
 * 
 * 📝 مثاله:
 * ---------
 * - إنشاء جلسة جديدة
 * - إضافة مشاركين
 * - تتبع من موجود
 * - إغلاق الجلسة
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <sstream>
#include <cstring>

namespace sad {
namespace collab {

// Forward declarations
struct SessionId;
struct UserId;

// ==========================================================
// 📌 معلومات المشارك
// ==========================================================

/**
 * 👤 مشارك في الجلسة
 */
struct Participant {
    uint32_t id;
    std::string username;
    std::string color;
    bool isHost;                // هل هو المضيف؟
    uint64_t joinedAt;          // وقت الانضمام
    uint64_t lastActivity;      // آخر نشاط
    
    // موقع المؤشر
    uint32_t cursorLine = 0;
    uint32_t cursorColumn = 0;
    std::string currentFile;
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"id\": " << id << ",\n";
        oss << "  \"username\": \"" << username << "\",\n";
        oss << "  \"color\": \"" << color << "\",\n";
        oss << "  \"isHost\": " << (isHost ? "true" : "false") << ",\n";
        oss << "  \"cursor\": {\"line\": " << cursorLine 
            << ", \"column\": " << cursorColumn << "}\n";
        oss << "}";
        return oss.str();
    }
};

// ==========================================================
// 📌 الجلسة
// ==========================================================

/**
 * 🎪 جلسة تعاون
 */
class Session {
private:
    std::string id_;
    std::string name_;
    uint32_t hostId_;
    uint64_t createdAt_;
    bool isActive_ = true;
    
    std::map<uint32_t, Participant> participants_;
    std::string currentDocument_;
    std::string filename_;
    
    // إعدادات
    int maxParticipants_ = 10;
    bool allowAnonymous_ = false;
    bool readOnly_ = false;
    
    std::mutex mutex_;
    
public:
    /**
     * إنشاء جلسة
     */
    Session(const std::string& id, const std::string& name, uint32_t hostId)
        : id_(id), name_(name), hostId_(hostId) {
        
        createdAt_ = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    // ═══════════════════════════════════════════════════════
    // Getters
    // ═══════════════════════════════════════════════════════
    
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    uint32_t hostId() const { return hostId_; }
    bool isActive() const { return isActive_; }
    int participantCount() const { return static_cast<int>(participants_.size()); }
    const std::string& document() const { return currentDocument_; }
    
    // ═══════════════════════════════════════════════════════
    // إدارة المشاركين
    // ═══════════════════════════════════════════════════════
    
    /**
     * إضافة مشارك
     */
    bool addParticipant(uint32_t id, const std::string& username,
                        const std::string& color) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isActive_) return false;
        if (static_cast<int>(participants_.size()) >= maxParticipants_) return false;
        
        Participant p;
        p.id = id;
        p.username = username;
        p.color = color;
        p.isHost = (id == hostId_);
        p.joinedAt = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        p.lastActivity = p.joinedAt;
        
        participants_[id] = p;
        return true;
    }
    
    /**
     * إزالة مشارك
     */
    bool removeParticipant(uint32_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = participants_.find(id);
        if (it == participants_.end()) return false;
        
        participants_.erase(it);
        
        // إذا غادر المضيف، نقل الدور أو إغلاق الجلسة
        if (id == hostId_) {
            if (participants_.empty()) {
                isActive_ = false;
            } else {
                // نقل لأول مشارك
                hostId_ = participants_.begin()->first;
                participants_[hostId_].isHost = true;
            }
        }
        
        return true;
    }
    
    /**
     * الحصول على مشارك
     */
    Participant* getParticipant(uint32_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = participants_.find(id);
        return it != participants_.end() ? &it->second : nullptr;
    }
    
    /**
     * قائمة المشاركين
     */
    std::vector<Participant> listParticipants() const {
        std::vector<Participant> list;
        for (const auto& [id, p] : participants_) {
            list.push_back(p);
        }
        return list;
    }
    
    // ═══════════════════════════════════════════════════════
    // إدارة المستند
    // ═══════════════════════════════════════════════════════
    
    /**
     * تعيين المستند
     */
    void setDocument(const std::string& content, const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentDocument_ = content;
        filename_ = filename;
    }
    
    /**
     * تطبيق تعديل
     */
    void applyEdit(uint32_t position, uint32_t deleteCount,
                   const std::string& insertText) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // تطبيق التعديل
        if (position > currentDocument_.length()) {
            position = static_cast<uint32_t>(currentDocument_.length());
        }
        
        // حذف
        if (deleteCount > 0 && position < currentDocument_.length()) {
            size_t endPos = std::min(
                static_cast<size_t>(position + deleteCount),
                currentDocument_.length()
            );
            currentDocument_.erase(position, endPos - position);
        }
        
        // إدراج
        if (!insertText.empty()) {
            currentDocument_.insert(position, insertText);
        }
    }
    
    // ═══════════════════════════════════════════════════════
    // تحديث المؤشر
    // ═══════════════════════════════════════════════════════
    
    /**
     * تحديث موقع مؤشر مشارك
     */
    void updateCursor(uint32_t participantId, uint32_t line, uint32_t column,
                      const std::string& file = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = participants_.find(participantId);
        if (it != participants_.end()) {
            it->second.cursorLine = line;
            it->second.cursorColumn = column;
            if (!file.empty()) {
                it->second.currentFile = file;
            }
            it->second.lastActivity = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
        }
    }
    
    // ═══════════════════════════════════════════════════════
    // الإعدادات
    // ═══════════════════════════════════════════════════════
    
    void setMaxParticipants(int max) { maxParticipants_ = max; }
    void setAllowAnonymous(bool allow) { allowAnonymous_ = allow; }
    void setReadOnly(bool readOnly) { readOnly_ = readOnly; }
    
    // ═══════════════════════════════════════════════════════
    // إغلاق الجلسة
    // ═══════════════════════════════════════════════════════
    
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        isActive_ = false;
        participants_.clear();
    }
    
    // ═══════════════════════════════════════════════════════
    // التحويل
    // ═══════════════════════════════════════════════════════
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"id\": \"" << id_ << "\",\n";
        oss << "  \"name\": \"" << name_ << "\",\n";
        oss << "  \"hostId\": " << hostId_ << ",\n";
        oss << "  \"isActive\": " << (isActive_ ? "true" : "false") << ",\n";
        oss << "  \"participantCount\": " << participants_.size() << ",\n";
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
};

// ==========================================================
// 📌 مدير الجلسات
// ==========================================================

/**
 * 🎛️ مدير الجلسات
 */
class SessionManager {
private:
    std::map<std::string, std::shared_ptr<Session>> sessions_;
    std::mutex mutex_;
    
    // إحصائيات
    int totalSessionsCreated_ = 0;
    int activeConnections_ = 0;
    
    SessionManager() = default;
    
public:
    /**
     * Singleton
     */
    static SessionManager& instance() {
        static SessionManager manager;
        return manager;
    }
    
    // ═══════════════════════════════════════════════════════
    // إدارة الجلسات
    // ═══════════════════════════════════════════════════════
    
    /**
     * إنشاء جلسة جديدة
     */
    std::shared_ptr<Session> createSession(const std::string& name,
                                            uint32_t hostId,
                                            const std::string& hostUsername) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // توليد معرّف
        std::string id = generateSessionId();
        
        // إنشاء الجلسة
        auto session = std::make_shared<Session>(id, name, hostId);
        session->addParticipant(hostId, hostUsername, "#00FF00");
        
        sessions_[id] = session;
        totalSessionsCreated_++;
        
        return session;
    }
    
    /**
     * الحصول على جلسة
     */
    std::shared_ptr<Session> getSession(const std::string& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = sessions_.find(id);
        return it != sessions_.end() ? it->second : nullptr;
    }
    
    /**
     * إغلاق جلسة
     */
    bool closeSession(const std::string& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = sessions_.find(id);
        if (it == sessions_.end()) return false;
        
        it->second->close();
        sessions_.erase(it);
        return true;
    }
    
    /**
     * قائمة الجلسات النشطة
     */
    std::vector<std::shared_ptr<Session>> listActiveSessions() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::shared_ptr<Session>> list;
        for (const auto& [id, session] : sessions_) {
            if (session->isActive()) {
                list.push_back(session);
            }
        }
        return list;
    }
    
    // ═══════════════════════════════════════════════════════
    // الانضمام والمغادرة
    // ═══════════════════════════════════════════════════════
    
    /**
     * انضمام لجلسة
     */
    bool joinSession(const std::string& sessionId, uint32_t userId,
                     const std::string& username, const std::string& color) {
        auto session = getSession(sessionId);
        if (!session) return false;
        
        bool joined = session->addParticipant(userId, username, color);
        if (joined) {
            std::lock_guard<std::mutex> lock(mutex_);
            activeConnections_++;
        }
        return joined;
    }
    
    /**
     * مغادرة جلسة
     */
    bool leaveSession(const std::string& sessionId, uint32_t userId) {
        auto session = getSession(sessionId);
        if (!session) return false;
        
        bool left = session->removeParticipant(userId);
        if (left) {
            std::lock_guard<std::mutex> lock(mutex_);
            activeConnections_--;
            
            // إزالة الجلسة إذا فارغة
            if (!session->isActive()) {
                sessions_.erase(sessionId);
            }
        }
        return left;
    }
    
    // ═══════════════════════════════════════════════════════
    // الإحصائيات
    // ═══════════════════════════════════════════════════════
    
    /**
     * إحصائيات
     */
    std::string stats() const {
        std::ostringstream oss;
        oss << "═══════════════════════════════════\n";
        oss << "      إحصائيات مدير الجلسات\n";
        oss << "═══════════════════════════════════\n\n";
        oss << "📊 الجلسات النشطة: " << sessions_.size() << "\n";
        oss << "📊 إجمالي الجلسات: " << totalSessionsCreated_ << "\n";
        oss << "📊 الاتصالات النشطة: " << activeConnections_ << "\n";
        return oss.str();
    }
    
    // ═══════════════════════════════════════════════════════
    // التنظيف
    // ═══════════════════════════════════════════════════════
    
    /**
     * تنظيف الجلسات غير النشطة
     */
    void cleanup() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> toRemove;
        for (const auto& [id, session] : sessions_) {
            if (!session->isActive() || session->participantCount() == 0) {
                toRemove.push_back(id);
            }
        }
        
        for (const auto& id : toRemove) {
            sessions_.erase(id);
        }
    }
    
private:
    /**
     * توليد معرّف جلسة
     */
    std::string generateSessionId() {
        static const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string id;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 35);
        
        for (int i = 0; i < 6; ++i) {
            id += chars[dis(gen)];
        }
        
        return id;
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء جلسة
 */
char* sad_session_create(const char* name, uint32_t hostId,
                          const char* hostUsername) {
    auto session = sad::collab::SessionManager::instance().createSession(
        name, hostId, hostUsername
    );
    
    std::string id = session->id();
    char* result = new char[id.size() + 1];
    std::strcpy(result, id.c_str());
    return result;
}

/**
 * الانضمام لجلسة
 */
int sad_session_join(const char* sessionId, uint32_t userId,
                     const char* username, const char* color) {
    return sad::collab::SessionManager::instance().joinSession(
        sessionId, userId, username, color
    ) ? 1 : 0;
}

/**
 * مغادرة جلسة
 */
int sad_session_leave(const char* sessionId, uint32_t userId) {
    return sad::collab::SessionManager::instance().leaveSession(
        sessionId, userId
    ) ? 1 : 0;
}

/**
 * إغلاق جلسة
 */
int sad_session_close(const char* sessionId) {
    return sad::collab::SessionManager::instance().closeSession(
        sessionId
    ) ? 1 : 0;
}

/**
 * الحصول على معلومات الجلسة
 */
char* sad_session_info(const char* sessionId) {
    auto session = sad::collab::SessionManager::instance().getSession(sessionId);
    if (!session) {
        return nullptr;
    }
    
    std::string json = session->toJson();
    char* result = new char[json.size() + 1];
    std::strcpy(result, json.c_str());
    return result;
}

/**
 * إحصائيات
 */
char* sad_session_stats(void) {
    std::string stats = sad::collab::SessionManager::instance().stats();
    char* result = new char[stats.size() + 1];
    std::strcpy(result, stats.c_str());
    return result;
}

/**
 * تحرير نص
 */
void sad_session_free_string(char* str) {
    delete[] str;
}

} // extern "C"
