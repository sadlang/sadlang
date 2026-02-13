/**
 * ==========================================================
 * ملف: collab_lsp.cpp
 * الوصف: امتداد LSP للتعاون
 * المهمة: T312 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔌 ما هو LSP للتعاون؟
 * ---------------------
 * يجعل محرر الكود (VS Code مثلاً) يدعم التعاون.
 * ترى مؤشرات زملائك وتعديلاتهم مباشرة!
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <mutex>

namespace sad {
namespace collab {

// ==========================================================
// 📌 رسائل LSP للتعاون
// ==========================================================

/**
 * 📬 أنواع رسائل التعاون
 */
namespace CollabMessages {
    const char* SessionStart = "sad/collaboration/sessionStart";
    const char* SessionJoin = "sad/collaboration/sessionJoin";
    const char* SessionLeave = "sad/collaboration/sessionLeave";
    const char* ParticipantJoined = "sad/collaboration/participantJoined";
    const char* ParticipantLeft = "sad/collaboration/participantLeft";
    const char* CursorUpdate = "sad/collaboration/cursorUpdate";
    const char* SelectionUpdate = "sad/collaboration/selectionUpdate";
    const char* TextEdit = "sad/collaboration/textEdit";
    const char* ChatMessage = "sad/collaboration/chatMessage";
}

// ==========================================================
// 📌 إشعار التعاون
// ==========================================================

/**
 * 📢 إشعار LSP
 */
struct CollabNotification {
    std::string method;
    std::string params;
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"jsonrpc\": \"2.0\",\n";
        oss << "  \"method\": \"" << method << "\",\n";
        oss << "  \"params\": " << params << "\n";
        oss << "}";
        return oss.str();
    }
};

// ==========================================================
// 📌 زخارف المؤشر
// ==========================================================

/**
 * 🎨 زخرفة المؤشر البعيد
 */
struct CursorDecoration {
    uint32_t userId;
    std::string username;
    std::string color;
    uint32_t line;
    uint32_t column;
    std::string filename;
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << userId << ",\n";
        oss << "  \"username\": \"" << username << "\",\n";
        oss << "  \"color\": \"" << color << "\",\n";
        oss << "  \"range\": {\n";
        oss << "    \"start\": {\"line\": " << line << ", \"character\": " << column << "},\n";
        oss << "    \"end\": {\"line\": " << line << ", \"character\": " << (column + 1) << "}\n";
        oss << "  },\n";
        oss << "  \"renderOptions\": {\n";
        oss << "    \"border\": \"2px solid " << color << "\",\n";
        oss << "    \"after\": {\n";
        oss << "      \"contentText\": \" " << username << "\",\n";
        oss << "      \"color\": \"" << color << "\",\n";
        oss << "      \"fontSize\": \"10px\"\n";
        oss << "    }\n";
        oss << "  }\n";
        oss << "}";
        return oss.str();
    }
};

// ==========================================================
// 📌 زخرفة التحديد
// ==========================================================

/**
 * 🎨 زخرفة التحديد البعيد
 */
struct SelectionDecoration {
    uint32_t userId;
    std::string color;
    uint32_t startLine;
    uint32_t startColumn;
    uint32_t endLine;
    uint32_t endColumn;
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << userId << ",\n";
        oss << "  \"range\": {\n";
        oss << "    \"start\": {\"line\": " << startLine << ", \"character\": " << startColumn << "},\n";
        oss << "    \"end\": {\"line\": " << endLine << ", \"character\": " << endColumn << "}\n";
        oss << "  },\n";
        oss << "  \"renderOptions\": {\n";
        oss << "    \"backgroundColor\": \"" << color << "33\"\n"; // 20% opacity
        oss << "  }\n";
        oss << "}";
        return oss.str();
    }
};

// ==========================================================
// 📌 خادم LSP للتعاون
// ==========================================================

/**
 * 🖥️ خادم LSP للتعاون
 */
class CollabLSPServer {
private:
    std::string sessionId_;
    uint32_t localUserId_;
    bool isConnected_ = false;
    
    std::map<uint32_t, CursorDecoration> remoteCursors_;
    std::map<uint32_t, SelectionDecoration> remoteSelections_;
    std::vector<CollabNotification> pendingNotifications_;
    
    std::mutex mutex_;
    
public:
    /**
     * إنشاء الخادم
     */
    CollabLSPServer(uint32_t localUserId) : localUserId_(localUserId) {}
    
    // ═══════════════════════════════════════════════════════
    // إدارة الجلسة
    // ═══════════════════════════════════════════════════════
    
    /**
     * بدء جلسة
     */
    std::string startSession(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // توليد معرّف (في التنفيذ الحقيقي، من الخادم)
        sessionId_ = "SES_" + std::to_string(localUserId_);
        isConnected_ = true;
        
        // إرسال إشعار
        CollabNotification notif;
        notif.method = CollabMessages::SessionStart;
        notif.params = "{\"sessionId\": \"" + sessionId_ + "\", \"name\": \"" + name + "\"}";
        pendingNotifications_.push_back(notif);
        
        return sessionId_;
    }
    
    /**
     * الانضمام لجلسة
     */
    bool joinSession(const std::string& sessionId, const std::string& username) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        sessionId_ = sessionId;
        isConnected_ = true;
        
        CollabNotification notif;
        notif.method = CollabMessages::SessionJoin;
        std::ostringstream oss;
        oss << "{\"sessionId\": \"" << sessionId 
            << "\", \"userId\": " << localUserId_
            << ", \"username\": \"" << username << "\"}";
        notif.params = oss.str();
        pendingNotifications_.push_back(notif);
        
        return true;
    }
    
    /**
     * مغادرة الجلسة
     */
    void leaveSession() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isConnected_) return;
        
        CollabNotification notif;
        notif.method = CollabMessages::SessionLeave;
        notif.params = "{\"userId\": " + std::to_string(localUserId_) + "}";
        pendingNotifications_.push_back(notif);
        
        sessionId_.clear();
        isConnected_ = false;
        remoteCursors_.clear();
        remoteSelections_.clear();
    }
    
    // ═══════════════════════════════════════════════════════
    // تحديثات المؤشر
    // ═══════════════════════════════════════════════════════
    
    /**
     * إرسال تحديث المؤشر المحلي
     */
    void sendCursorUpdate(uint32_t line, uint32_t column,
                          const std::string& filename) {
        if (!isConnected_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        CollabNotification notif;
        notif.method = CollabMessages::CursorUpdate;
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << localUserId_ << ",\n";
        oss << "  \"position\": {\"line\": " << line << ", \"character\": " << column << "},\n";
        oss << "  \"filename\": \"" << filename << "\"\n";
        oss << "}";
        notif.params = oss.str();
        pendingNotifications_.push_back(notif);
    }
    
    /**
     * استقبال تحديث مؤشر بعيد
     */
    void receiveCursorUpdate(uint32_t userId, const std::string& username,
                              const std::string& color,
                              uint32_t line, uint32_t column,
                              const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        CursorDecoration dec;
        dec.userId = userId;
        dec.username = username;
        dec.color = color;
        dec.line = line;
        dec.column = column;
        dec.filename = filename;
        
        remoteCursors_[userId] = dec;
    }
    
    /**
     * إرسال تحديث التحديد
     */
    void sendSelectionUpdate(uint32_t startLine, uint32_t startCol,
                              uint32_t endLine, uint32_t endCol,
                              const std::string& filename) {
        if (!isConnected_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        CollabNotification notif;
        notif.method = CollabMessages::SelectionUpdate;
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << localUserId_ << ",\n";
        oss << "  \"range\": {\n";
        oss << "    \"start\": {\"line\": " << startLine << ", \"character\": " << startCol << "},\n";
        oss << "    \"end\": {\"line\": " << endLine << ", \"character\": " << endCol << "}\n";
        oss << "  },\n";
        oss << "  \"filename\": \"" << filename << "\"\n";
        oss << "}";
        notif.params = oss.str();
        pendingNotifications_.push_back(notif);
    }
    
    // ═══════════════════════════════════════════════════════
    // تحريرات النص
    // ═══════════════════════════════════════════════════════
    
    /**
     * إرسال تحرير نص
     */
    void sendTextEdit(const std::string& filename,
                      uint32_t startLine, uint32_t startCol,
                      uint32_t endLine, uint32_t endCol,
                      const std::string& newText) {
        if (!isConnected_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        CollabNotification notif;
        notif.method = CollabMessages::TextEdit;
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << localUserId_ << ",\n";
        oss << "  \"filename\": \"" << filename << "\",\n";
        oss << "  \"range\": {\n";
        oss << "    \"start\": {\"line\": " << startLine << ", \"character\": " << startCol << "},\n";
        oss << "    \"end\": {\"line\": " << endLine << ", \"character\": " << endCol << "}\n";
        oss << "  },\n";
        oss << "  \"newText\": \"" << escapeJson(newText) << "\"\n";
        oss << "}";
        notif.params = oss.str();
        pendingNotifications_.push_back(notif);
    }
    
    // ═══════════════════════════════════════════════════════
    // الدردشة
    // ═══════════════════════════════════════════════════════
    
    /**
     * إرسال رسالة دردشة
     */
    void sendChatMessage(const std::string& message) {
        if (!isConnected_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        CollabNotification notif;
        notif.method = CollabMessages::ChatMessage;
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << localUserId_ << ",\n";
        oss << "  \"message\": \"" << escapeJson(message) << "\"\n";
        oss << "}";
        notif.params = oss.str();
        pendingNotifications_.push_back(notif);
    }
    
    // ═══════════════════════════════════════════════════════
    // الحصول على البيانات
    // ═══════════════════════════════════════════════════════
    
    /**
     * الحصول على زخارف المؤشرات للملف
     */
    std::string getCursorDecorations(const std::string& filename) const {
        std::ostringstream oss;
        oss << "[";
        
        bool first = true;
        for (const auto& [id, cursor] : remoteCursors_) {
            if (cursor.filename != filename) continue;
            if (!first) oss << ", ";
            oss << cursor.toJson();
            first = false;
        }
        
        oss << "]";
        return oss.str();
    }
    
    /**
     * الحصول على زخارف التحديد للملف
     */
    std::string getSelectionDecorations(const std::string& filename) const {
        std::ostringstream oss;
        oss << "[";
        
        bool first = true;
        for (const auto& [id, sel] : remoteSelections_) {
            if (!first) oss << ", ";
            oss << sel.toJson();
            first = false;
        }
        
        oss << "]";
        return oss.str();
    }
    
    /**
     * الحصول على الإشعارات المعلقة
     */
    std::vector<std::string> getPendingNotifications() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> result;
        for (const auto& notif : pendingNotifications_) {
            result.push_back(notif.toJson());
        }
        pendingNotifications_.clear();
        
        return result;
    }
    
    // ═══════════════════════════════════════════════════════
    // معالجة طلبات LSP
    // ═══════════════════════════════════════════════════════
    
    /**
     * معالجة طلب
     */
    std::string handleRequest(const std::string& method,
                               const std::string& params) {
        // في التنفيذ الحقيقي، نحلل JSON ونعالج
        return "{}";
    }
    
    /**
     * الحالة
     */
    bool isConnected() const { return isConnected_; }
    const std::string& sessionId() const { return sessionId_; }
    
private:
    static std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء خادم LSP للتعاون
 */
void* sad_collab_lsp_new(uint32_t userId) {
    return new sad::collab::CollabLSPServer(userId);
}

/**
 * تحرير الخادم
 */
void sad_collab_lsp_free(void* server) {
    delete static_cast<sad::collab::CollabLSPServer*>(server);
}

/**
 * بدء جلسة
 */
char* sad_collab_lsp_start(void* server, const char* name) {
    auto* srv = static_cast<sad::collab::CollabLSPServer*>(server);
    std::string id = srv->startSession(name);
    
    char* result = new char[id.size() + 1];
    std::strcpy(result, id.c_str());
    return result;
}

/**
 * الانضمام لجلسة
 */
int sad_collab_lsp_join(void* server, const char* sessionId,
                        const char* username) {
    auto* srv = static_cast<sad::collab::CollabLSPServer*>(server);
    return srv->joinSession(sessionId, username) ? 1 : 0;
}

/**
 * مغادرة الجلسة
 */
void sad_collab_lsp_leave(void* server) {
    auto* srv = static_cast<sad::collab::CollabLSPServer*>(server);
    srv->leaveSession();
}

/**
 * إرسال تحديث المؤشر
 */
void sad_collab_lsp_cursor(void* server, uint32_t line, uint32_t col,
                           const char* filename) {
    auto* srv = static_cast<sad::collab::CollabLSPServer*>(server);
    srv->sendCursorUpdate(line, col, filename);
}

/**
 * الحصول على زخارف المؤشرات
 */
char* sad_collab_lsp_get_cursors(void* server, const char* filename) {
    auto* srv = static_cast<sad::collab::CollabLSPServer*>(server);
    std::string json = srv->getCursorDecorations(filename);
    
    char* result = new char[json.size() + 1];
    std::strcpy(result, json.c_str());
    return result;
}

/**
 * تحرير نص
 */
void sad_collab_lsp_free_string(char* str) {
    delete[] str;
}

} // extern "C"
