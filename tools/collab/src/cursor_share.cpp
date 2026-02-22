/**
 * ==========================================================
 * ملف: cursor_share.cpp
 * الوصف: مشاركة المؤشرات بين المتعاونين
 * المهمة: T309 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🖱️ ما هي مشاركة المؤشر؟
 * -----------------------
 * عندما تعمل مع شخص آخر على نفس الكود،
 * ترى أين مؤشره ويرى أين مؤشرك!
 * 
 * 📝 مثال:
 * --------
 * أحمد يكتب في السطر 5 ← ترى مؤشره باللون الأخضر
 * سارة تكتب في السطر 10 ← ترى مؤشرها باللون الأزرق
 */

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include <sstream>
#include <cstring>

namespace sad {
namespace collab {

// ==========================================================
// 📌 موقع المؤشر
// ==========================================================

/**
 * 📍 موقع المؤشر
 */
struct CursorLocation {
    uint32_t line;
    uint32_t column;
    std::string filename;
    uint64_t timestamp;
    
    std::string toString() const {
        std::ostringstream oss;
        oss << filename << ":" << line << ":" << column;
        return oss.str();
    }
};

// ==========================================================
// 📌 التحديد
// ==========================================================

/**
 * 📐 نطاق التحديد
 */
struct Selection {
    uint32_t startLine;
    uint32_t startColumn;
    uint32_t endLine;
    uint32_t endColumn;
    std::string filename;
    
    bool isEmpty() const {
        return startLine == endLine && startColumn == endColumn;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << filename << " [" << startLine << ":" << startColumn
            << " - " << endLine << ":" << endColumn << "]";
        return oss.str();
    }
};

// ==========================================================
// 📌 معلومات المؤشر
// ==========================================================

/**
 * 🖱️ معلومات مؤشر المستخدم
 */
struct CursorInfo {
    uint32_t userId;
    std::string username;
    std::string color;          // لون المؤشر (hex)
    CursorLocation location;
    Selection selection;
    bool isTyping = false;
    uint64_t lastUpdate = 0;
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"userId\": " << userId << ",\n";
        oss << "  \"username\": \"" << username << "\",\n";
        oss << "  \"color\": \"" << color << "\",\n";
        oss << "  \"location\": {\n";
        oss << "    \"line\": " << location.line << ",\n";
        oss << "    \"column\": " << location.column << ",\n";
        oss << "    \"filename\": \"" << location.filename << "\"\n";
        oss << "  },\n";
        oss << "  \"selection\": {\n";
        oss << "    \"start\": {\"line\": " << selection.startLine 
            << ", \"column\": " << selection.startColumn << "},\n";
        oss << "    \"end\": {\"line\": " << selection.endLine 
            << ", \"column\": " << selection.endColumn << "}\n";
        oss << "  },\n";
        oss << "  \"isTyping\": " << (isTyping ? "true" : "false") << "\n";
        oss << "}";
        return oss.str();
    }
    
    /**
     * تحويل لـ bytes
     */
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        // userId
        data.push_back(userId & 0xFF);
        data.push_back((userId >> 8) & 0xFF);
        data.push_back((userId >> 16) & 0xFF);
        data.push_back((userId >> 24) & 0xFF);
        
        // location
        data.push_back(location.line & 0xFF);
        data.push_back((location.line >> 8) & 0xFF);
        data.push_back((location.line >> 16) & 0xFF);
        data.push_back((location.line >> 24) & 0xFF);
        
        data.push_back(location.column & 0xFF);
        data.push_back((location.column >> 8) & 0xFF);
        data.push_back((location.column >> 16) & 0xFF);
        data.push_back((location.column >> 24) & 0xFF);
        
        // filename length + data
        uint16_t fnLen = static_cast<uint16_t>(location.filename.length());
        data.push_back(fnLen & 0xFF);
        data.push_back((fnLen >> 8) & 0xFF);
        data.insert(data.end(), location.filename.begin(), location.filename.end());
        
        // selection
        data.push_back(selection.startLine & 0xFF);
        data.push_back((selection.startLine >> 8) & 0xFF);
        data.push_back((selection.startLine >> 16) & 0xFF);
        data.push_back((selection.startLine >> 24) & 0xFF);
        
        data.push_back(selection.startColumn & 0xFF);
        data.push_back((selection.startColumn >> 8) & 0xFF);
        data.push_back((selection.startColumn >> 16) & 0xFF);
        data.push_back((selection.startColumn >> 24) & 0xFF);
        
        data.push_back(selection.endLine & 0xFF);
        data.push_back((selection.endLine >> 8) & 0xFF);
        data.push_back((selection.endLine >> 16) & 0xFF);
        data.push_back((selection.endLine >> 24) & 0xFF);
        
        data.push_back(selection.endColumn & 0xFF);
        data.push_back((selection.endColumn >> 8) & 0xFF);
        data.push_back((selection.endColumn >> 16) & 0xFF);
        data.push_back((selection.endColumn >> 24) & 0xFF);
        
        // isTyping
        data.push_back(isTyping ? 1 : 0);
        
        return data;
    }
};

// ==========================================================
// 📌 مدير المؤشرات
// ==========================================================

/**
 * 🎛️ مدير مؤشرات الجلسة
 */
class CursorManager {
private:
    std::map<uint32_t, CursorInfo> cursors_;
    std::mutex mutex_;
    
    // الألوان المتاحة للمؤشرات
    std::vector<std::string> availableColors_ = {
        "#00FF00", // أخضر
        "#0000FF", // أزرق
        "#FF00FF", // وردي
        "#FFFF00", // أصفر
        "#00FFFF", // سماوي
        "#FFA500", // برتقالي
        "#800080", // بنفسجي
        "#008080"  // أخضر مزرق
    };
    int nextColorIndex_ = 0;
    
public:
    /**
     * إضافة مؤشر مستخدم
     */
    void addUser(uint32_t userId, const std::string& username,
                 const std::string& color = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        CursorInfo info;
        info.userId = userId;
        info.username = username;
        info.color = color.empty() ? getNextColor() : color;
        info.lastUpdate = currentTime();
        
        cursors_[userId] = info;
    }
    
    /**
     * إزالة مؤشر مستخدم
     */
    void removeUser(uint32_t userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        cursors_.erase(userId);
    }
    
    /**
     * تحديث موقع المؤشر
     */
    void updateLocation(uint32_t userId, uint32_t line, uint32_t column,
                        const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cursors_.find(userId);
        if (it != cursors_.end()) {
            it->second.location.line = line;
            it->second.location.column = column;
            it->second.location.filename = filename;
            it->second.location.timestamp = currentTime();
            it->second.lastUpdate = currentTime();
        }
    }
    
    /**
     * تحديث التحديد
     */
    void updateSelection(uint32_t userId, 
                         uint32_t startLine, uint32_t startColumn,
                         uint32_t endLine, uint32_t endColumn,
                         const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cursors_.find(userId);
        if (it != cursors_.end()) {
            it->second.selection.startLine = startLine;
            it->second.selection.startColumn = startColumn;
            it->second.selection.endLine = endLine;
            it->second.selection.endColumn = endColumn;
            it->second.selection.filename = filename;
            it->second.lastUpdate = currentTime();
        }
    }
    
    /**
     * تعيين حالة الكتابة
     */
    void setTyping(uint32_t userId, bool isTyping) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cursors_.find(userId);
        if (it != cursors_.end()) {
            it->second.isTyping = isTyping;
            it->second.lastUpdate = currentTime();
        }
    }
    
    /**
     * الحصول على مؤشر مستخدم
     */
    CursorInfo* getCursor(uint32_t userId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cursors_.find(userId);
        return it != cursors_.end() ? &it->second : nullptr;
    }
    
    /**
     * قائمة جميع المؤشرات
     */
    std::vector<CursorInfo> getAllCursors() const {
        std::vector<CursorInfo> list;
        for (const auto& [id, cursor] : cursors_) {
            list.push_back(cursor);
        }
        return list;
    }
    
    /**
     * المؤشرات في ملف معين
     */
    std::vector<CursorInfo> getCursorsInFile(const std::string& filename) const {
        std::vector<CursorInfo> list;
        for (const auto& [id, cursor] : cursors_) {
            if (cursor.location.filename == filename) {
                list.push_back(cursor);
            }
        }
        return list;
    }
    
    /**
     * تنظيف المؤشرات غير النشطة
     */
    void cleanupInactive(uint64_t timeout = 30000) { // 30 ثانية
        std::lock_guard<std::mutex> lock(mutex_);
        
        uint64_t now = currentTime();
        std::vector<uint32_t> toRemove;
        
        for (const auto& [id, cursor] : cursors_) {
            if (now - cursor.lastUpdate > timeout) {
                toRemove.push_back(id);
            }
        }
        
        for (uint32_t id : toRemove) {
            cursors_.erase(id);
        }
    }
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n  \"cursors\": [\n";
        
        bool first = true;
        for (const auto& [id, cursor] : cursors_) {
            if (!first) oss << ",\n";
            oss << "    " << cursor.toJson();
            first = false;
        }
        
        oss << "\n  ]\n}";
        return oss.str();
    }
    
private:
    std::string getNextColor() {
        std::string color = availableColors_[nextColorIndex_ % availableColors_.size()];
        nextColorIndex_++;
        return color;
    }
    
    uint64_t currentTime() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
};

// ==========================================================
// 📌 رسم المؤشرات
// ==========================================================

/**
 * 🎨 مُولِّد عرض المؤشرات
 */
class CursorRenderer {
public:
    /**
     * توليد CSS للمؤشر
     */
    static std::string generateCSS(const CursorInfo& cursor) {
        std::ostringstream oss;
        
        oss << ".cursor-" << cursor.userId << " {\n";
        oss << "  position: absolute;\n";
        oss << "  width: 2px;\n";
        oss << "  background-color: " << cursor.color << ";\n";
        oss << "  animation: blink 1s infinite;\n";
        oss << "}\n\n";
        
        oss << ".cursor-label-" << cursor.userId << " {\n";
        oss << "  position: absolute;\n";
        oss << "  background-color: " << cursor.color << ";\n";
        oss << "  color: white;\n";
        oss << "  font-size: 10px;\n";
        oss << "  padding: 2px 4px;\n";
        oss << "  border-radius: 2px;\n";
        oss << "  white-space: nowrap;\n";
        oss << "}\n\n";
        
        // التحديد
        oss << ".selection-" << cursor.userId << " {\n";
        oss << "  background-color: " << cursor.color << "33;\n"; // 20% opacity
        oss << "}\n";
        
        return oss.str();
    }
    
    /**
     * توليد HTML للمؤشر
     */
    static std::string generateHTML(const CursorInfo& cursor) {
        std::ostringstream oss;
        
        oss << "<div class=\"remote-cursor cursor-" << cursor.userId << "\" ";
        oss << "style=\"top: " << (cursor.location.line * 20) << "px; ";
        oss << "left: " << (cursor.location.column * 8) << "px;\">\n";
        oss << "  <div class=\"cursor-label-" << cursor.userId << "\">";
        oss << cursor.username;
        if (cursor.isTyping) {
            oss << " ✏️";
        }
        oss << "</div>\n";
        oss << "</div>\n";
        
        return oss.str();
    }
    
    /**
     * توليد معلومات LSP
     */
    static std::string generateLSPDecoration(const CursorInfo& cursor) {
        std::ostringstream oss;
        
        oss << "{\n";
        oss << "  \"range\": {\n";
        oss << "    \"start\": {\"line\": " << cursor.location.line 
            << ", \"character\": " << cursor.location.column << "},\n";
        oss << "    \"end\": {\"line\": " << cursor.location.line 
            << ", \"character\": " << (cursor.location.column + 1) << "}\n";
        oss << "  },\n";
        oss << "  \"renderOptions\": {\n";
        oss << "    \"before\": {\n";
        oss << "      \"contentText\": \"\",\n";
        oss << "      \"backgroundColor\": \"" << cursor.color << "\",\n";
        oss << "      \"width\": \"2px\",\n";
        oss << "      \"height\": \"100%\"\n";
        oss << "    }\n";
        oss << "  },\n";
        oss << "  \"hoverMessage\": \"" << cursor.username << "\"\n";
        oss << "}";
        
        return oss.str();
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مدير مؤشرات
 */
void* sad_cursor_manager_new(void) {
    return new sad::collab::CursorManager();
}

/**
 * تحرير المدير
 */
void sad_cursor_manager_free(void* manager) {
    delete static_cast<sad::collab::CursorManager*>(manager);
}

/**
 * إضافة مستخدم
 */
void sad_cursor_add_user(void* manager, uint32_t userId,
                         const char* username, const char* color) {
    auto* mgr = static_cast<sad::collab::CursorManager*>(manager);
    mgr->addUser(userId, username, color ? color : "");
}

/**
 * إزالة مستخدم
 */
void sad_cursor_remove_user(void* manager, uint32_t userId) {
    auto* mgr = static_cast<sad::collab::CursorManager*>(manager);
    mgr->removeUser(userId);
}

/**
 * تحديث الموقع
 */
void sad_cursor_update(void* manager, uint32_t userId,
                       uint32_t line, uint32_t column,
                       const char* filename) {
    auto* mgr = static_cast<sad::collab::CursorManager*>(manager);
    mgr->updateLocation(userId, line, column, filename);
}

/**
 * تحديث التحديد
 */
void sad_cursor_select(void* manager, uint32_t userId,
                       uint32_t startLine, uint32_t startCol,
                       uint32_t endLine, uint32_t endCol,
                       const char* filename) {
    auto* mgr = static_cast<sad::collab::CursorManager*>(manager);
    mgr->updateSelection(userId, startLine, startCol, endLine, endCol, filename);
}

/**
 * الحصول على JSON
 */
char* sad_cursor_to_json(void* manager) {
    auto* mgr = static_cast<sad::collab::CursorManager*>(manager);
    std::string json = mgr->toJson();
    
    char* result = new char[json.size() + 1];
    std::strcpy(result, json.c_str());
    return result;
}

/**
 * تحرير نص
 */
void sad_cursor_free_string(char* str) {
    delete[] str;
}

} // extern "C"
