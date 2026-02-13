/**
 * ==========================================================
 * ملف: constant_pool.cpp
 * الوصف: جدول الثوابت (Constant Pool)
 * المهمة: T274 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🤔 ما هو جدول الثوابت؟
 * ----------------------
 * جدول الثوابت هو مكان نخزن فيه القيم الثابتة التي
 * يستخدمها البرنامج، مثل:
 * - النصوص: "مرحباً بالعالم"
 * - الأرقام الكبيرة: 123456789
 * - الأرقام العشرية: 3.14159
 * 
 * 📦 لماذا نستخدم جدول ثوابت؟
 * ---------------------------
 * بدلاً من تكرار نفس القيمة عدة مرات في الكود:
 * 
 *   اطبع("مرحباً")   ← يخزن "مرحباً" هنا
 *   اطبع("مرحباً")   ← يخزن "مرحباً" مرة أخرى!
 *   اطبع("مرحباً")   ← ومرة ثالثة!
 * 
 * نخزنها مرة واحدة ونشير إليها:
 * 
 *   جدول الثوابت:
 *     [0] = "مرحباً"
 *   
 *   الكود:
 *     اطبع ثابت[0]   ← يشير للفهرس 0
 *     اطبع ثابت[0]   ← نفس الفهرس
 *     اطبع ثابت[0]   ← توفير كبير!
 * 
 * 🎯 المزايا:
 * -----------
 * 1. توفير المساحة (عدم التكرار)
 * 2. سرعة المقارنة (نقارن الفهارس)
 * 3. سهولة الإدارة (مكان واحد)
 */

#include "format.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 هيكل الثابت
// ==========================================================

/**
 * 📦 ثابت في جدول الثوابت
 */
struct Constant {
    ConstantType type;              // نوع الثابت
    std::vector<uint8_t> data;      // البيانات الخام
    
    // للراحة في الاستخدام
    union {
        bool boolValue;
        int64_t intValue;
        double floatValue;
    } cached;
    std::string stringValue;        // للنصوص
    
    /**
     * 🏗️ إنشاء ثابت فارغ
     */
    Constant() : type(ConstantType::NULL_VALUE) {
        cached.intValue = 0;
    }
    
    /**
     * 🏗️ إنشاء ثابت منطقي
     */
    static Constant fromBool(bool value) {
        Constant c;
        c.type = ConstantType::BOOL;
        c.data.push_back(value ? 1 : 0);
        c.cached.boolValue = value;
        return c;
    }
    
    /**
     * 🏗️ إنشاء ثابت صحيح
     */
    static Constant fromInt(int64_t value) {
        Constant c;
        c.cached.intValue = value;
        
        // تحديد النوع الأمثل
        if (value >= -128 && value <= 127) {
            c.type = ConstantType::INT8;
            c.data.push_back(static_cast<uint8_t>(value));
        } else if (value >= -32768 && value <= 32767) {
            c.type = ConstantType::INT16;
            auto v = static_cast<int16_t>(value);
            c.data.push_back(static_cast<uint8_t>(v & 0xFF));
            c.data.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        } else if (value >= INT32_MIN && value <= INT32_MAX) {
            c.type = ConstantType::INT32;
            auto v = static_cast<int32_t>(value);
            for (int i = 0; i < 4; ++i) {
                c.data.push_back(static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
            }
        } else {
            c.type = ConstantType::INT64;
            for (int i = 0; i < 8; ++i) {
                c.data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
            }
        }
        
        return c;
    }
    
    /**
     * 🏗️ إنشاء ثابت عشري
     */
    static Constant fromFloat(double value) {
        Constant c;
        c.type = ConstantType::FLOAT64;
        c.cached.floatValue = value;
        
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        for (int i = 0; i < 8; ++i) {
            c.data.push_back(static_cast<uint8_t>((bits >> (i * 8)) & 0xFF));
        }
        
        return c;
    }
    
    /**
     * 🏗️ إنشاء ثابت نصي
     */
    static Constant fromString(const std::string& value) {
        Constant c;
        c.stringValue = value;
        
        // تحديد إذا كان النص عربياً
        bool hasArabic = false;
        for (unsigned char ch : value) {
            if (ch >= 0xD8 && ch <= 0xDB) {
                hasArabic = true;
                break;
            }
        }
        
        c.type = hasArabic ? ConstantType::STRING_ARABIC : ConstantType::STRING;
        
        // الطول (16 بت)
        uint16_t len = static_cast<uint16_t>(
            std::min(value.size(), static_cast<size_t>(MAX_STRING_LENGTH)));
        c.data.push_back(static_cast<uint8_t>(len & 0xFF));
        c.data.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
        
        // المحتوى
        for (size_t i = 0; i < len; ++i) {
            c.data.push_back(static_cast<uint8_t>(value[i]));
        }
        
        return c;
    }
    
    /**
     * 📝 تحويل إلى string للعرض
     */
    std::string toString() const {
        std::ostringstream oss;
        
        switch (type) {
            case ConstantType::NULL_VALUE:
                oss << "فارغ";
                break;
            case ConstantType::BOOL:
                oss << (cached.boolValue ? "صح" : "خطأ");
                break;
            case ConstantType::INT8:
            case ConstantType::INT16:
            case ConstantType::INT32:
            case ConstantType::INT64:
                oss << cached.intValue;
                break;
            case ConstantType::FLOAT32:
            case ConstantType::FLOAT64:
                oss << cached.floatValue;
                break;
            case ConstantType::STRING:
            case ConstantType::STRING_ARABIC:
                oss << "\"" << stringValue << "\"";
                break;
            default:
                oss << "<" << constantTypeName(type) << ">";
                break;
        }
        
        return oss.str();
    }
    
    /**
     * 📊 الحجم بالبايتات
     */
    size_t byteSize() const {
        return 1 + data.size(); // 1 للنوع + البيانات
    }
};

// ==========================================================
// 📌 فئة ConstantPool (جدول الثوابت)
// ==========================================================

/**
 * 📚 جدول الثوابت
 * 
 * 📚 الاستخدام:
 * -------------
 * ConstantPool pool;
 * 
 * // إضافة ثوابت
 * uint16_t strIdx = pool.addString("مرحباً");
 * uint16_t numIdx = pool.addInt(42);
 * uint16_t floatIdx = pool.addFloat(3.14);
 * 
 * // استرجاع
 * const Constant& c = pool.get(strIdx);
 * std::cout << c.stringValue << std::endl;
 * 
 * // تسلسل
 * auto bytes = pool.serialize();
 */
class ConstantPool {
private:
    std::vector<Constant> constants_;
    
    // للبحث السريع عن التكرارات
    std::unordered_map<std::string, uint16_t> stringIndex_;
    std::unordered_map<int64_t, uint16_t> intIndex_;
    std::unordered_map<double, uint16_t> floatIndex_;
    
public:
    /**
     * 📊 عدد الثوابت
     */
    size_t size() const { return constants_.size(); }
    
    /**
     * 🔍 هل فارغ؟
     */
    bool empty() const { return constants_.empty(); }
    
    /**
     * 📖 الحصول على ثابت
     */
    const Constant& get(uint16_t index) const {
        if (index >= constants_.size()) {
            throw std::out_of_range("فهرس خارج النطاق");
        }
        return constants_[index];
    }
    
    /**
     * 📖 الحصول على كل الثوابت
     */
    const std::vector<Constant>& getAll() const {
        return constants_;
    }
    
    // ═══════════════════════════════════════════════════════
    // إضافة ثوابت
    // ═══════════════════════════════════════════════════════
    
    /**
     * ➕ إضافة قيمة فارغة
     */
    uint16_t addNull() {
        Constant c;
        c.type = ConstantType::NULL_VALUE;
        return addConstant(c);
    }
    
    /**
     * ➕ إضافة قيمة منطقية
     */
    uint16_t addBool(bool value) {
        return addConstant(Constant::fromBool(value));
    }
    
    /**
     * ➕ إضافة عدد صحيح (مع تجنب التكرار)
     */
    uint16_t addInt(int64_t value) {
        auto it = intIndex_.find(value);
        if (it != intIndex_.end()) {
            return it->second;
        }
        
        uint16_t index = addConstant(Constant::fromInt(value));
        intIndex_[value] = index;
        return index;
    }
    
    /**
     * ➕ إضافة عدد عشري (مع تجنب التكرار)
     */
    uint16_t addFloat(double value) {
        auto it = floatIndex_.find(value);
        if (it != floatIndex_.end()) {
            return it->second;
        }
        
        uint16_t index = addConstant(Constant::fromFloat(value));
        floatIndex_[value] = index;
        return index;
    }
    
    /**
     * ➕ إضافة نص (مع تجنب التكرار)
     */
    uint16_t addString(const std::string& value) {
        auto it = stringIndex_.find(value);
        if (it != stringIndex_.end()) {
            return it->second;
        }
        
        uint16_t index = addConstant(Constant::fromString(value));
        stringIndex_[value] = index;
        return index;
    }
    
    /**
     * ➕ إضافة ثابت مباشر
     */
    uint16_t addConstant(const Constant& constant) {
        if (constants_.size() >= MAX_CONSTANT_POOL_SIZE) {
            throw std::overflow_error("جدول الثوابت ممتلئ");
        }
        
        uint16_t index = static_cast<uint16_t>(constants_.size());
        constants_.push_back(constant);
        return index;
    }
    
    // ═══════════════════════════════════════════════════════
    // البحث
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🔍 البحث عن نص
     */
    int32_t findString(const std::string& value) const {
        auto it = stringIndex_.find(value);
        if (it != stringIndex_.end()) {
            return static_cast<int32_t>(it->second);
        }
        return -1;
    }
    
    /**
     * 🔍 البحث عن عدد صحيح
     */
    int32_t findInt(int64_t value) const {
        auto it = intIndex_.find(value);
        if (it != intIndex_.end()) {
            return static_cast<int32_t>(it->second);
        }
        return -1;
    }
    
    // ═══════════════════════════════════════════════════════
    // التسلسل (Serialization)
    // ═══════════════════════════════════════════════════════
    
    /**
     * 💾 تسلسل الجدول إلى بايتات
     */
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> result;
        
        // عدد الثوابت (16 بت)
        uint16_t count = static_cast<uint16_t>(constants_.size());
        result.push_back(static_cast<uint8_t>(count & 0xFF));
        result.push_back(static_cast<uint8_t>((count >> 8) & 0xFF));
        
        // كل ثابت
        for (const auto& constant : constants_) {
            // النوع
            result.push_back(static_cast<uint8_t>(constant.type));
            // البيانات
            result.insert(result.end(), constant.data.begin(), constant.data.end());
        }
        
        return result;
    }
    
    /**
     * 📖 فك تسلسل من بايتات
     */
    static ConstantPool deserialize(const uint8_t* data, size_t length) {
        ConstantPool pool;
        
        if (length < 2) {
            return pool;
        }
        
        // قراءة العدد
        uint16_t count = static_cast<uint16_t>(data[0] | (data[1] << 8));
        size_t offset = 2;
        
        for (uint16_t i = 0; i < count && offset < length; ++i) {
            Constant c;
            c.type = static_cast<ConstantType>(data[offset++]);
            
            // قراءة البيانات حسب النوع
            switch (c.type) {
                case ConstantType::NULL_VALUE:
                    break;
                    
                case ConstantType::BOOL:
                    c.data.push_back(data[offset++]);
                    c.cached.boolValue = c.data[0] != 0;
                    break;
                    
                case ConstantType::INT8:
                    c.data.push_back(data[offset++]);
                    c.cached.intValue = static_cast<int8_t>(c.data[0]);
                    break;
                    
                case ConstantType::INT16:
                    c.data.push_back(data[offset++]);
                    c.data.push_back(data[offset++]);
                    c.cached.intValue = static_cast<int16_t>(c.data[0] | (c.data[1] << 8));
                    break;
                    
                case ConstantType::INT32:
                    for (int j = 0; j < 4; ++j) {
                        c.data.push_back(data[offset++]);
                    }
                    c.cached.intValue = static_cast<int32_t>(
                        c.data[0] | (c.data[1] << 8) | 
                        (c.data[2] << 16) | (c.data[3] << 24));
                    break;
                    
                case ConstantType::INT64:
                    for (int j = 0; j < 8; ++j) {
                        c.data.push_back(data[offset++]);
                    }
                    c.cached.intValue = 0;
                    for (int j = 0; j < 8; ++j) {
                        c.cached.intValue |= static_cast<int64_t>(c.data[j]) << (j * 8);
                    }
                    break;
                    
                case ConstantType::FLOAT64:
                    for (int j = 0; j < 8; ++j) {
                        c.data.push_back(data[offset++]);
                    }
                    {
                        uint64_t bits = 0;
                        for (int j = 0; j < 8; ++j) {
                            bits |= static_cast<uint64_t>(c.data[j]) << (j * 8);
                        }
                        std::memcpy(&c.cached.floatValue, &bits, sizeof(bits));
                    }
                    break;
                    
                case ConstantType::STRING:
                case ConstantType::STRING_ARABIC:
                    {
                        uint16_t len = static_cast<uint16_t>(
                            data[offset] | (data[offset + 1] << 8));
                        c.data.push_back(data[offset++]);
                        c.data.push_back(data[offset++]);
                        
                        for (uint16_t j = 0; j < len && offset < length; ++j) {
                            c.data.push_back(data[offset]);
                            c.stringValue += static_cast<char>(data[offset++]);
                        }
                    }
                    break;
                    
                default:
                    break;
            }
            
            pool.constants_.push_back(c);
        }
        
        // إعادة بناء الفهارس
        for (uint16_t i = 0; i < pool.constants_.size(); ++i) {
            const auto& c = pool.constants_[i];
            switch (c.type) {
                case ConstantType::STRING:
                case ConstantType::STRING_ARABIC:
                    pool.stringIndex_[c.stringValue] = i;
                    break;
                case ConstantType::INT8:
                case ConstantType::INT16:
                case ConstantType::INT32:
                case ConstantType::INT64:
                    pool.intIndex_[c.cached.intValue] = i;
                    break;
                case ConstantType::FLOAT64:
                    pool.floatIndex_[c.cached.floatValue] = i;
                    break;
                default:
                    break;
            }
        }
        
        return pool;
    }
    
    // ═══════════════════════════════════════════════════════
    // العرض والتنقيح
    // ═══════════════════════════════════════════════════════
    
    /**
     * 📝 تفريغ الجدول للعرض
     */
    std::string dump() const {
        std::ostringstream oss;
        
        oss << "═══════════════════════════════════\n";
        oss << "        جدول الثوابت\n";
        oss << "═══════════════════════════════════\n\n";
        
        oss << "عدد الثوابت: " << constants_.size() << "\n\n";
        
        oss << "الفهرس │ النوع        │ القيمة\n";
        oss << "───────┼──────────────┼─────────────────\n";
        
        for (size_t i = 0; i < constants_.size(); ++i) {
            const auto& c = constants_[i];
            oss << std::setw(6) << i << " │ "
                << std::setw(12) << constantTypeName(c.type) << " │ "
                << c.toString() << "\n";
        }
        
        return oss.str();
    }
    
    /**
     * 📊 الحجم الكلي بالبايتات
     */
    size_t totalBytes() const {
        size_t total = 2; // عدد الثوابت
        for (const auto& c : constants_) {
            total += c.byteSize();
        }
        return total;
    }
    
    /**
     * 🧹 مسح الجدول
     */
    void clear() {
        constants_.clear();
        stringIndex_.clear();
        intIndex_.clear();
        floatIndex_.clear();
    }
};

// ==========================================================
// 📌 فئة ConstantPoolBuilder (بناء جدول ثوابت)
// ==========================================================

/**
 * 🏗️ بناء جدول ثوابت بشكل تسلسلي
 */
class ConstantPoolBuilder {
private:
    ConstantPool pool_;
    
public:
    /**
     * ➕ إضافة من AST أو IR
     */
    uint16_t addLiteral(const std::string& type, const std::string& value) {
        if (type == "صحيح" || type == "int") {
            return pool_.addInt(std::stoll(value));
        } else if (type == "عشري" || type == "float") {
            return pool_.addFloat(std::stod(value));
        } else if (type == "نص" || type == "string") {
            return pool_.addString(value);
        } else if (type == "منطقي" || type == "bool") {
            return pool_.addBool(value == "صح" || value == "true");
        } else {
            return pool_.addNull();
        }
    }
    
    /**
     * 🏗️ بناء الجدول النهائي
     */
    ConstantPool build() {
        return std::move(pool_);
    }
    
    /**
     * 📊 الجدول الحالي
     */
    ConstantPool& current() {
        return pool_;
    }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء جدول ثوابت جديد
 */
void* sad_constant_pool_new(void) {
    return new sad::bytecode::ConstantPool();
}

/**
 * تحرير جدول الثوابت
 */
void sad_constant_pool_free(void* pool) {
    delete static_cast<sad::bytecode::ConstantPool*>(pool);
}

/**
 * عدد الثوابت
 */
size_t sad_constant_pool_size(void* pool) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->size();
}

/**
 * إضافة عدد صحيح
 */
uint16_t sad_constant_pool_add_int(void* pool, int64_t value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->addInt(value);
}

/**
 * إضافة عدد عشري
 */
uint16_t sad_constant_pool_add_float(void* pool, double value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->addFloat(value);
}

/**
 * إضافة نص
 */
uint16_t sad_constant_pool_add_string(void* pool, const char* value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->addString(value);
}

/**
 * البحث عن نص
 */
int32_t sad_constant_pool_find_string(void* pool, const char* value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->findString(value);
}

/**
 * تسلسل الجدول
 */
void sad_constant_pool_serialize(void* pool, uint8_t** output, size_t* length) {
    auto bytes = static_cast<sad::bytecode::ConstantPool*>(pool)->serialize();
    *length = bytes.size();
    *output = new uint8_t[bytes.size()];
    std::copy(bytes.begin(), bytes.end(), *output);
}

/**
 * فك تسلسل الجدول
 */
void* sad_constant_pool_deserialize(const uint8_t* data, size_t length) {
    auto pool = new sad::bytecode::ConstantPool();
    *pool = sad::bytecode::ConstantPool::deserialize(data, length);
    return pool;
}

/**
 * تفريغ الجدول للعرض
 */
const char* sad_constant_pool_dump(void* pool) {
    static std::string dump;
    dump = static_cast<sad::bytecode::ConstantPool*>(pool)->dump();
    return dump.c_str();
}

} // extern "C"
