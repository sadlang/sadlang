/**
 * ==========================================================
 * ملف: crdt_sync.cpp
 * الوصف: مزامنة CRDT للتعاون الفوري
 * المهمة: T308 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔄 ما هو CRDT؟
 * --------------
 * CRDT = Conflict-free Replicated Data Type
 * نوع بيانات يُمكن تعديله من عدة أماكن في نفس الوقت
 * دون تضارب!
 * 
 * 📝 مثال:
 * --------
 * أحمد يكتب في السطر 1: "مرحبا"
 * سارة تكتب في السطر 2: "عالم"
 * كلاهما يرى النتيجة النهائية نفسها!
 * 
 * 🎯 المبدأ:
 * ---------
 * كل حرف له معرّف فريد عالمياً.
 * عند الدمج، نرتب بالمعرّفات.
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <mutex>

namespace sad {
namespace collab {

// ==========================================================
// 📌 معرّف الحرف
// ==========================================================

/**
 * 🆔 معرّف فريد للحرف
 */
struct CharId {
    uint32_t siteId;    // معرّف الموقع (المستخدم)
    uint64_t clock;     // الساعة المنطقية
    
    bool operator<(const CharId& other) const {
        if (clock != other.clock) return clock < other.clock;
        return siteId < other.siteId;
    }
    
    bool operator==(const CharId& other) const {
        return siteId == other.siteId && clock == other.clock;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << siteId << ":" << clock;
        return oss.str();
    }
};

// ==========================================================
// 📌 عنصر CRDT
// ==========================================================

/**
 * 📝 عنصر (حرف) في المستند
 */
struct CRDTChar {
    CharId id;
    char32_t value;     // الحرف (UTF-32)
    CharId leftId;      // المعرّف الأيسر
    CharId rightId;     // المعرّف الأيمن
    bool deleted = false;
    
    bool operator<(const CRDTChar& other) const {
        return id < other.id;
    }
};

// ==========================================================
// 📌 عملية CRDT
// ==========================================================

/**
 * نوع العملية
 */
enum class OpType {
    INSERT,
    DELETE
};

/**
 * 📋 عملية CRDT
 */
struct CRDTOperation {
    OpType type;
    CharId charId;
    char32_t value;     // للإدراج فقط
    CharId leftId;      // للإدراج فقط
    CharId rightId;     // للإدراج فقط
    uint32_t siteId;
    uint64_t timestamp;
    
    /**
     * تحويل لـ bytes
     */
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        // Type
        data.push_back(static_cast<uint8_t>(type));
        
        // CharId
        writeUint32(data, charId.siteId);
        writeUint64(data, charId.clock);
        
        // Value (for INSERT)
        writeUint32(data, static_cast<uint32_t>(value));
        
        // LeftId
        writeUint32(data, leftId.siteId);
        writeUint64(data, leftId.clock);
        
        // RightId
        writeUint32(data, rightId.siteId);
        writeUint64(data, rightId.clock);
        
        // SiteId & timestamp
        writeUint32(data, siteId);
        writeUint64(data, timestamp);
        
        return data;
    }
    
    /**
     * من bytes
     */
    static CRDTOperation fromBytes(const uint8_t* data) {
        CRDTOperation op;
        size_t offset = 0;
        
        op.type = static_cast<OpType>(data[offset++]);
        
        op.charId.siteId = readUint32(data, offset); offset += 4;
        op.charId.clock = readUint64(data, offset); offset += 8;
        
        op.value = static_cast<char32_t>(readUint32(data, offset)); offset += 4;
        
        op.leftId.siteId = readUint32(data, offset); offset += 4;
        op.leftId.clock = readUint64(data, offset); offset += 8;
        
        op.rightId.siteId = readUint32(data, offset); offset += 4;
        op.rightId.clock = readUint64(data, offset); offset += 8;
        
        op.siteId = readUint32(data, offset); offset += 4;
        op.timestamp = readUint64(data, offset);
        
        return op;
    }
    
private:
    static void writeUint32(std::vector<uint8_t>& data, uint32_t value) {
        data.push_back(value & 0xFF);
        data.push_back((value >> 8) & 0xFF);
        data.push_back((value >> 16) & 0xFF);
        data.push_back((value >> 24) & 0xFF);
    }
    
    static void writeUint64(std::vector<uint8_t>& data, uint64_t value) {
        for (int i = 0; i < 8; ++i) {
            data.push_back((value >> (i * 8)) & 0xFF);
        }
    }
    
    static uint32_t readUint32(const uint8_t* data, size_t offset) {
        return data[offset] | (data[offset+1] << 8) |
               (data[offset+2] << 16) | (data[offset+3] << 24);
    }
    
    static uint64_t readUint64(const uint8_t* data, size_t offset) {
        uint64_t value = 0;
        for (int i = 0; i < 8; ++i) {
            value |= static_cast<uint64_t>(data[offset + i]) << (i * 8);
        }
        return value;
    }
};

// ==========================================================
// 📌 مستند CRDT
// ==========================================================

/**
 * 📄 مستند CRDT
 */
class CRDTDocument {
private:
    uint32_t siteId_;
    uint64_t clock_ = 0;
    
    std::vector<CRDTChar> chars_;
    std::mutex mutex_;
    
    // المعرّفات الخاصة للبداية والنهاية
    static const CharId START_ID;
    static const CharId END_ID;
    
public:
    /**
     * إنشاء مستند جديد
     */
    CRDTDocument(uint32_t siteId) : siteId_(siteId) {
        // إضافة عناصر البداية والنهاية
        CRDTChar startChar, endChar;
        startChar.id = START_ID;
        startChar.value = 0;
        endChar.id = END_ID;
        endChar.value = 0;
        
        chars_.push_back(startChar);
        chars_.push_back(endChar);
    }
    
    // ═══════════════════════════════════════════════════════
    // العمليات
    // ═══════════════════════════════════════════════════════
    
    /**
     * إدراج حرف
     */
    CRDTOperation insert(size_t position, char32_t value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // تحديث الساعة
        clock_++;
        
        // الحصول على المعرّفات المجاورة
        CharId leftId = START_ID;
        CharId rightId = END_ID;
        
        size_t visiblePos = 0;
        for (size_t i = 0; i < chars_.size(); ++i) {
            if (chars_[i].deleted) continue;
            
            if (visiblePos == position) {
                rightId = chars_[i].id;
                break;
            }
            leftId = chars_[i].id;
            visiblePos++;
        }
        
        // إنشاء الحرف الجديد
        CRDTChar newChar;
        newChar.id = {siteId_, clock_};
        newChar.value = value;
        newChar.leftId = leftId;
        newChar.rightId = rightId;
        
        // إدراج في المكان الصحيح
        insertChar(newChar);
        
        // إنشاء العملية
        CRDTOperation op;
        op.type = OpType::INSERT;
        op.charId = newChar.id;
        op.value = value;
        op.leftId = leftId;
        op.rightId = rightId;
        op.siteId = siteId_;
        op.timestamp = clock_;
        
        return op;
    }
    
    /**
     * حذف حرف
     */
    CRDTOperation remove(size_t position) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // البحث عن الحرف
        size_t visiblePos = 0;
        for (size_t i = 0; i < chars_.size(); ++i) {
            if (chars_[i].deleted) continue;
            if (chars_[i].id == START_ID || chars_[i].id == END_ID) continue;
            
            if (visiblePos == position) {
                chars_[i].deleted = true;
                
                CRDTOperation op;
                op.type = OpType::DELETE;
                op.charId = chars_[i].id;
                op.siteId = siteId_;
                op.timestamp = clock_;
                
                return op;
            }
            visiblePos++;
        }
        
        // لم يُوجد
        CRDTOperation op;
        op.type = OpType::DELETE;
        op.charId = {0, 0};
        return op;
    }
    
    /**
     * تطبيق عملية (من موقع آخر)
     */
    void apply(const CRDTOperation& op) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // تحديث الساعة
        if (op.timestamp > clock_) {
            clock_ = op.timestamp;
        }
        
        if (op.type == OpType::INSERT) {
            // التحقق من عدم الوجود
            for (const auto& c : chars_) {
                if (c.id == op.charId) return; // موجود مسبقاً
            }
            
            CRDTChar newChar;
            newChar.id = op.charId;
            newChar.value = op.value;
            newChar.leftId = op.leftId;
            newChar.rightId = op.rightId;
            
            insertChar(newChar);
        } else {
            // حذف
            for (auto& c : chars_) {
                if (c.id == op.charId) {
                    c.deleted = true;
                    break;
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════
    // الحصول على النص
    // ═══════════════════════════════════════════════════════
    
    /**
     * الحصول على النص الكامل
     */
    std::string toString() const {
        std::string result;
        
        for (const auto& c : chars_) {
            if (c.deleted) continue;
            if (c.id == START_ID || c.id == END_ID) continue;
            
            // تحويل UTF-32 لـ UTF-8
            if (c.value < 0x80) {
                result += static_cast<char>(c.value);
            } else if (c.value < 0x800) {
                result += static_cast<char>(0xC0 | (c.value >> 6));
                result += static_cast<char>(0x80 | (c.value & 0x3F));
            } else if (c.value < 0x10000) {
                result += static_cast<char>(0xE0 | (c.value >> 12));
                result += static_cast<char>(0x80 | ((c.value >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (c.value & 0x3F));
            } else {
                result += static_cast<char>(0xF0 | (c.value >> 18));
                result += static_cast<char>(0x80 | ((c.value >> 12) & 0x3F));
                result += static_cast<char>(0x80 | ((c.value >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (c.value & 0x3F));
            }
        }
        
        return result;
    }
    
    /**
     * طول النص المرئي
     */
    size_t length() const {
        size_t len = 0;
        for (const auto& c : chars_) {
            if (!c.deleted && c.id.clock != 0) len++;
        }
        return len;
    }
    
    /**
     * الساعة الحالية
     */
    uint64_t clock() const { return clock_; }
    
private:
    /**
     * إدراج حرف في المكان الصحيح
     */
    void insertChar(const CRDTChar& newChar) {
        // البحث عن المكان الصحيح
        size_t pos = 0;
        for (size_t i = 0; i < chars_.size(); ++i) {
            // بعد العنصر الأيسر
            if (chars_[i].id == newChar.leftId) {
                pos = i + 1;
                continue;
            }
            
            // قبل العنصر الأيمن
            if (chars_[i].id == newChar.rightId) {
                break;
            }
            
            // ترتيب بين العناصر المتساوية
            if (i >= pos && chars_[i].id < newChar.id) {
                pos = i + 1;
            }
        }
        
        chars_.insert(chars_.begin() + pos, newChar);
    }
};

// تعريف المعرّفات الثابتة
const CharId CRDTDocument::START_ID = {0, 0};
const CharId CRDTDocument::END_ID = {UINT32_MAX, UINT64_MAX};

// ==========================================================
// 📌 مزامن CRDT
// ==========================================================

/**
 * 🔄 مزامن CRDT
 */
class CRDTSync {
private:
    std::map<std::string, std::shared_ptr<CRDTDocument>> documents_;
    std::vector<CRDTOperation> pendingOps_;
    std::mutex mutex_;
    
public:
    /**
     * إنشاء مستند جديد
     */
    std::shared_ptr<CRDTDocument> createDocument(const std::string& id,
                                                   uint32_t siteId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto doc = std::make_shared<CRDTDocument>(siteId);
        documents_[id] = doc;
        return doc;
    }
    
    /**
     * الحصول على مستند
     */
    std::shared_ptr<CRDTDocument> getDocument(const std::string& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = documents_.find(id);
        return it != documents_.end() ? it->second : nullptr;
    }
    
    /**
     * إضافة عملية للمزامنة
     */
    void addOperation(const CRDTOperation& op) {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingOps_.push_back(op);
    }
    
    /**
     * الحصول على العمليات المعلقة
     */
    std::vector<CRDTOperation> getPendingOperations() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto ops = std::move(pendingOps_);
        pendingOps_.clear();
        return ops;
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مستند CRDT
 */
void* sad_crdt_document_new(uint32_t siteId) {
    return new sad::collab::CRDTDocument(siteId);
}

/**
 * تحرير المستند
 */
void sad_crdt_document_free(void* doc) {
    delete static_cast<sad::collab::CRDTDocument*>(doc);
}

/**
 * إدراج حرف
 */
size_t sad_crdt_insert(void* doc, size_t position, const char* utf8Char,
                       uint8_t* opBuffer, size_t bufferSize) {
    auto* document = static_cast<sad::collab::CRDTDocument*>(doc);
    
    // تحويل UTF-8 لـ UTF-32 (مبسط)
    char32_t value = 0;
    if ((utf8Char[0] & 0x80) == 0) {
        value = utf8Char[0];
    } else if ((utf8Char[0] & 0xE0) == 0xC0) {
        value = ((utf8Char[0] & 0x1F) << 6) | (utf8Char[1] & 0x3F);
    } else if ((utf8Char[0] & 0xF0) == 0xE0) {
        value = ((utf8Char[0] & 0x0F) << 12) | ((utf8Char[1] & 0x3F) << 6) |
                (utf8Char[2] & 0x3F);
    }
    
    auto op = document->insert(position, value);
    auto bytes = op.toBytes();
    
    if (bytes.size() <= bufferSize) {
        memcpy(opBuffer, bytes.data(), bytes.size());
        return bytes.size();
    }
    return 0;
}

/**
 * حذف حرف
 */
size_t sad_crdt_delete(void* doc, size_t position,
                       uint8_t* opBuffer, size_t bufferSize) {
    auto* document = static_cast<sad::collab::CRDTDocument*>(doc);
    
    auto op = document->remove(position);
    auto bytes = op.toBytes();
    
    if (bytes.size() <= bufferSize) {
        memcpy(opBuffer, bytes.data(), bytes.size());
        return bytes.size();
    }
    return 0;
}

/**
 * تطبيق عملية
 */
void sad_crdt_apply(void* doc, const uint8_t* opData, size_t opSize) {
    auto* document = static_cast<sad::collab::CRDTDocument*>(doc);
    auto op = sad::collab::CRDTOperation::fromBytes(opData);
    document->apply(op);
}

/**
 * الحصول على النص
 */
char* sad_crdt_to_string(void* doc) {
    auto* document = static_cast<sad::collab::CRDTDocument*>(doc);
    std::string text = document->toString();
    
    char* result = new char[text.size() + 1];
    std::strcpy(result, text.c_str());
    return result;
}

/**
 * تحرير نص
 */
void sad_crdt_free_string(char* str) {
    delete[] str;
}

} // extern "C"
