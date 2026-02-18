/**
 * ==========================================================
 * ملف: symbol_table.cpp
 * الوصف: جدول الرموز وتسلسله
 * المهمة: T275 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🤔 ما هو جدول الرموز؟
 * ---------------------
 * جدول الرموز يخزن معلومات عن كل "اسم" في البرنامج:
 * - أسماء المتغيرات: عدد، اسم، نتيجة
 * - أسماء الدوال: اطبع، احسب، رئيسية
 * - أسماء الهياكل: شخص، سيارة، نقطة
 * 
 * 📝 ماذا يخزن لكل رمز؟
 * ----------------------
 * لكل رمز نخزن:
 * 1. الاسم: "عدد" أو "اطبع"
 * 2. النوع: متغير، دالة، هيكل، ...
 * 3. نوع البيانات: صحيح، نص، عشري، ...
 * 4. النطاق: محلي، عام، ...
 * 5. الموقع: أين يوجد في الكود
 * 
 * 🎯 لماذا نحتاج جدول الرموز؟
 * ---------------------------
 * 1. الربط: ربط الأسماء بمواقعها في الذاكرة
 * 2. الفحص: التأكد من أن المتغير موجود قبل استخدامه
 * 3. التنقيح: عرض أسماء المتغيرات عند الأخطاء
 * 4. التحسين: معرفة نوع المتغير لتحسين الأداء
 * 
 * 📦 بنية جدول الرموز في الملف:
 * -----------------------------
 * ┌─────────────────────────────────────┐
 * │  عدد الرموز (16 بت)                  │
 * ├─────────────────────────────────────┤
 * │  رمز 0:                              │
 * │    - فهرس الاسم (32 بت)              │
 * │    - النوع (8 بت)                    │
 * │    - الأعلام (16 بت)                 │
 * │    - فهرس نوع البيانات (32 بت)       │
 * │    - موقع القيمة (32 بت)             │
 * ├─────────────────────────────────────┤
 * │  رمز 1: ...                          │
 * └─────────────────────────────────────┘
 */

#include "format.h"
#include "constant_pool.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 هيكل الرمز
// ==========================================================

/**
 * 🏷️ رمز في جدول الرموز
 */
struct Symbol {
    std::string name;           // الاسم
    SymbolType type;            // نوع الرمز
    SymbolFlags flags;          // الأعلام
    std::string dataType;       // نوع البيانات
    uint32_t scopeDepth;        // عمق النطاق
    uint32_t offset;            // الموقع/الإزاحة
    
    // للدوال
    std::vector<std::string> paramTypes;    // أنواع المعاملات
    std::string returnType;                  // نوع الإرجاع
    
    // للهياكل
    std::vector<std::pair<std::string, std::string>> fields; // (اسم، نوع)
    
    /**
     * 🏗️ إنشاء رمز فارغ
     */
    Symbol()
        : type(SymbolType::VARIABLE), 
          flags(SymbolFlags::NONE),
          scopeDepth(0), 
          offset(0) {}
    
    /**
     * 🏗️ إنشاء متغير
     */
    static Symbol variable(const std::string& name, const std::string& dataType,
                          uint32_t scopeDepth = 0, bool mutable_ = true) {
        Symbol s;
        s.name = name;
        s.type = SymbolType::VARIABLE;
        s.dataType = dataType;
        s.scopeDepth = scopeDepth;
        s.flags = mutable_ ? SymbolFlags::MUTABLE : SymbolFlags::NONE;
        return s;
    }
    
    /**
     * 🏗️ إنشاء ثابت
     */
    static Symbol constant(const std::string& name, const std::string& dataType,
                          uint32_t scopeDepth = 0) {
        Symbol s;
        s.name = name;
        s.type = SymbolType::CONSTANT;
        s.dataType = dataType;
        s.scopeDepth = scopeDepth;
        s.flags = SymbolFlags::NONE;
        return s;
    }
    
    /**
     * 🏗️ إنشاء دالة
     */
    static Symbol function(const std::string& name, 
                          const std::vector<std::string>& paramTypes,
                          const std::string& returnType,
                          bool isPublic = true) {
        Symbol s;
        s.name = name;
        s.type = SymbolType::FUNCTION;
        s.paramTypes = paramTypes;
        s.returnType = returnType;
        s.flags = isPublic ? SymbolFlags::PUBLIC : SymbolFlags::PRIVATE;
        return s;
    }
    
    /**
     * 🏗️ إنشاء هيكل
     */
    static Symbol struct_(const std::string& name,
                         const std::vector<std::pair<std::string, std::string>>& fields,
                         bool isPublic = true) {
        Symbol s;
        s.name = name;
        s.type = SymbolType::STRUCT;
        s.fields = fields;
        s.flags = isPublic ? SymbolFlags::PUBLIC : SymbolFlags::PRIVATE;
        return s;
    }
    
    /**
     * 📝 تحويل إلى string للعرض
     */
    std::string toString() const {
        std::ostringstream oss;
        
        // النوع
        switch (type) {
            case SymbolType::VARIABLE: oss << "متغير"; break;
            case SymbolType::CONSTANT: oss << "ثابت"; break;
            case SymbolType::FUNCTION: oss << "دالة"; break;
            case SymbolType::METHOD: oss << "طريقة"; break;
            case SymbolType::STRUCT: oss << "هيكل"; break;
            case SymbolType::ENUM: oss << "تعداد"; break;
            case SymbolType::MODULE: oss << "وحدة"; break;
            case SymbolType::PARAMETER: oss << "معامل"; break;
            case SymbolType::FIELD: oss << "حقل"; break;
            case SymbolType::GENERIC: oss << "عام"; break;
            case SymbolType::TRAIT: oss << "سمة"; break;
            case SymbolType::IMPL: oss << "تنفيذ"; break;
        }
        
        oss << " " << name;
        
        // نوع البيانات
        if (!dataType.empty()) {
            oss << ": " << dataType;
        }
        
        // للدوال
        if (type == SymbolType::FUNCTION) {
            oss << "(";
            for (size_t i = 0; i < paramTypes.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << paramTypes[i];
            }
            oss << ")";
            if (!returnType.empty()) {
                oss << " -> " << returnType;
            }
        }
        
        // الأعلام
        if (hasFlag(flags, SymbolFlags::PUBLIC)) oss << " [عام]";
        if (hasFlag(flags, SymbolFlags::MUTABLE)) oss << " [قابل_للتعديل]";
        if (hasFlag(flags, SymbolFlags::STATIC)) oss << " [ساكن]";
        if (hasFlag(flags, SymbolFlags::ASYNC)) oss << " [غير_متزامن]";
        
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة SymbolTable (جدول الرموز)
// ==========================================================

/**
 * 📚 جدول الرموز
 */
class SymbolTable {
private:
    std::vector<Symbol> symbols_;
    std::unordered_map<std::string, std::vector<uint32_t>> nameIndex_;
    uint32_t currentScope_ = 0;
    
public:
    /**
     * 📊 عدد الرموز
     */
    size_t size() const { return symbols_.size(); }
    
    /**
     * 🔍 هل فارغ؟
     */
    bool empty() const { return symbols_.empty(); }
    
    /**
     * 📖 الحصول على رمز
     */
    const Symbol& get(uint32_t index) const {
        if (index >= symbols_.size()) {
            throw std::out_of_range("فهرس خارج النطاق");
        }
        return symbols_[index];
    }
    
    /**
     * 📖 الحصول على كل الرموز
     */
    const std::vector<Symbol>& getAll() const {
        return symbols_;
    }
    
    // ═══════════════════════════════════════════════════════
    // إدارة النطاقات
    // ═══════════════════════════════════════════════════════
    
    /**
     * 📥 الدخول لنطاق جديد
     */
    void enterScope() {
        currentScope_++;
    }
    
    /**
     * 📤 الخروج من النطاق الحالي
     */
    void exitScope() {
        if (currentScope_ > 0) {
            currentScope_--;
        }
    }
    
    /**
     * 📊 عمق النطاق الحالي
     */
    uint32_t currentScopeDepth() const {
        return currentScope_;
    }
    
    // ═══════════════════════════════════════════════════════
    // إضافة رموز
    // ═══════════════════════════════════════════════════════
    
    /**
     * ➕ إضافة رمز
     */
    uint32_t add(const Symbol& symbol) {
        uint32_t index = static_cast<uint32_t>(symbols_.size());
        
        Symbol s = symbol;
        s.scopeDepth = currentScope_;
        
        symbols_.push_back(s);
        nameIndex_[s.name].push_back(index);
        
        return index;
    }
    
    /**
     * ➕ إضافة متغير
     */
    uint32_t addVariable(const std::string& name, const std::string& type,
                        bool mutable_ = true) {
        return add(Symbol::variable(name, type, currentScope_, mutable_));
    }
    
    /**
     * ➕ إضافة ثابت
     */
    uint32_t addConstant(const std::string& name, const std::string& type) {
        return add(Symbol::constant(name, type, currentScope_));
    }
    
    /**
     * ➕ إضافة دالة
     */
    uint32_t addFunction(const std::string& name,
                        const std::vector<std::string>& paramTypes,
                        const std::string& returnType) {
        return add(Symbol::function(name, paramTypes, returnType));
    }
    
    /**
     * ➕ إضافة هيكل
     */
    uint32_t addStruct(const std::string& name,
                      const std::vector<std::pair<std::string, std::string>>& fields) {
        return add(Symbol::struct_(name, fields));
    }
    
    // ═══════════════════════════════════════════════════════
    // البحث
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🔍 البحث عن رمز بالاسم (في النطاق الحالي وما فوقه)
     */
    int32_t find(const std::string& name) const {
        auto it = nameIndex_.find(name);
        if (it == nameIndex_.end()) {
            return -1;
        }
        
        // البحث من الأحدث للأقدم
        const auto& indices = it->second;
        for (auto rit = indices.rbegin(); rit != indices.rend(); ++rit) {
            const Symbol& s = symbols_[*rit];
            if (s.scopeDepth <= currentScope_) {
                return static_cast<int32_t>(*rit);
            }
        }
        
        return -1;
    }
    
    /**
     * 🔍 البحث عن رمز بالاسم (في نطاق محدد)
     */
    int32_t findInScope(const std::string& name, uint32_t scope) const {
        auto it = nameIndex_.find(name);
        if (it == nameIndex_.end()) {
            return -1;
        }
        
        for (uint32_t index : it->second) {
            if (symbols_[index].scopeDepth == scope) {
                return static_cast<int32_t>(index);
            }
        }
        
        return -1;
    }
    
    /**
     * 🔍 هل الاسم موجود في النطاق الحالي؟
     */
    bool existsInCurrentScope(const std::string& name) const {
        return findInScope(name, currentScope_) >= 0;
    }
    
    /**
     * 🔍 البحث عن كل الدوال
     */
    std::vector<uint32_t> findAllFunctions() const {
        std::vector<uint32_t> result;
        for (size_t i = 0; i < symbols_.size(); ++i) {
            if (symbols_[i].type == SymbolType::FUNCTION) {
                result.push_back(static_cast<uint32_t>(i));
            }
        }
        return result;
    }
    
    /**
     * 🔍 البحث عن كل الهياكل
     */
    std::vector<uint32_t> findAllStructs() const {
        std::vector<uint32_t> result;
        for (size_t i = 0; i < symbols_.size(); ++i) {
            if (symbols_[i].type == SymbolType::STRUCT) {
                result.push_back(static_cast<uint32_t>(i));
            }
        }
        return result;
    }
    
    // ═══════════════════════════════════════════════════════
    // التسلسل
    // ═══════════════════════════════════════════════════════
    
    /**
     * 💾 تسلسل جدول الرموز
     * 
     * يحتاج جدول ثوابت لتخزين الأسماء
     */
    std::vector<uint8_t> serialize(ConstantPool& constPool) const {
        std::vector<uint8_t> result;
        
        // عدد الرموز (16 بت)
        uint16_t count = static_cast<uint16_t>(symbols_.size());
        result.push_back(static_cast<uint8_t>(count & 0xFF));
        result.push_back(static_cast<uint8_t>((count >> 8) & 0xFF));
        
        // كل رمز
        for (const auto& sym : symbols_) {
            // فهرس الاسم في جدول الثوابت
            uint32_t nameIdx = constPool.addString(sym.name);
            for (int i = 0; i < 4; ++i) {
                result.push_back(static_cast<uint8_t>((nameIdx >> (i * 8)) & 0xFF));
            }
            
            // النوع
            result.push_back(static_cast<uint8_t>(sym.type));
            
            // عمق النطاق
            result.push_back(static_cast<uint8_t>(sym.scopeDepth));
            
            // الأعلام
            uint16_t flags = static_cast<uint16_t>(sym.flags);
            result.push_back(static_cast<uint8_t>(flags & 0xFF));
            result.push_back(static_cast<uint8_t>((flags >> 8) & 0xFF));
            
            // فهرس نوع البيانات
            uint32_t typeIdx = sym.dataType.empty() ? 0xFFFFFFFF : constPool.addString(sym.dataType);
            for (int i = 0; i < 4; ++i) {
                result.push_back(static_cast<uint8_t>((typeIdx >> (i * 8)) & 0xFF));
            }
            
            // الموقع
            for (int i = 0; i < 4; ++i) {
                result.push_back(static_cast<uint8_t>((sym.offset >> (i * 8)) & 0xFF));
            }
        }
        
        return result;
    }
    
    /**
     * 📖 فك تسلسل من بايتات
     */
    static SymbolTable deserialize(const uint8_t* data, size_t length,
                                   const ConstantPool& constPool) {
        SymbolTable table;
        
        if (length < 2) return table;
        
        uint16_t count = static_cast<uint16_t>(data[0] | (data[1] << 8));
        size_t offset = 2;
        
        for (uint16_t i = 0; i < count && offset + 16 <= length; ++i) {
            Symbol sym;
            
            // فهرس الاسم
            uint32_t nameIdx = 0;
            for (int j = 0; j < 4; ++j) {
                nameIdx |= static_cast<uint32_t>(data[offset++]) << (j * 8);
            }
            sym.name = constPool.get(static_cast<uint16_t>(nameIdx)).stringValue;
            
            // النوع
            sym.type = static_cast<SymbolType>(data[offset++]);
            
            // عمق النطاق
            sym.scopeDepth = data[offset++];
            
            // الأعلام
            uint16_t flags = static_cast<uint16_t>(data[offset] | (data[offset + 1] << 8));
            sym.flags = static_cast<SymbolFlags>(flags);
            offset += 2;
            
            // فهرس نوع البيانات
            uint32_t typeIdx = 0;
            for (int j = 0; j < 4; ++j) {
                typeIdx |= static_cast<uint32_t>(data[offset++]) << (j * 8);
            }
            if (typeIdx != 0xFFFFFFFF) {
                sym.dataType = constPool.get(static_cast<uint16_t>(typeIdx)).stringValue;
            }
            
            // الموقع
            sym.offset = 0;
            for (int j = 0; j < 4; ++j) {
                sym.offset |= static_cast<uint32_t>(data[offset++]) << (j * 8);
            }
            
            table.symbols_.push_back(sym);
            table.nameIndex_[sym.name].push_back(static_cast<uint32_t>(table.symbols_.size() - 1));
        }
        
        return table;
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
        oss << "        جدول الرموز\n";
        oss << "═══════════════════════════════════\n\n";
        
        oss << "عدد الرموز: " << symbols_.size() << "\n";
        oss << "عمق النطاق الحالي: " << currentScope_ << "\n\n";
        
        oss << "الفهرس │ النطاق │ الرمز\n";
        oss << "───────┼────────┼─────────────────────────────\n";
        
        for (size_t i = 0; i < symbols_.size(); ++i) {
            const auto& sym = symbols_[i];
            oss << std::setw(6) << i << " │ "
                << std::setw(6) << sym.scopeDepth << " │ "
                << sym.toString() << "\n";
        }
        
        return oss.str();
    }
    
    /**
     * 🧹 مسح الجدول
     */
    void clear() {
        symbols_.clear();
        nameIndex_.clear();
        currentScope_ = 0;
    }
};

// ==========================================================
// 📌 فئة ScopedSymbolTable (جدول رموز مع إدارة نطاقات)
// ==========================================================

/**
 * 📚 جدول رموز مع إدارة نطاقات تلقائية
 */
class ScopedSymbolTable {
private:
    SymbolTable table_;
    std::vector<std::vector<uint32_t>> scopeStack_; // الرموز في كل نطاق
    
public:
    /**
     * 📥 الدخول لنطاق جديد
     */
    void enterScope() {
        table_.enterScope();
        scopeStack_.push_back({});
    }
    
    /**
     * 📤 الخروج من النطاق (مع إزالة الرموز)
     */
    void exitScope() {
        // في التنفيذ الكامل، يمكن إزالة الرموز المحلية
        if (!scopeStack_.empty()) {
            scopeStack_.pop_back();
        }
        table_.exitScope();
    }
    
    /**
     * ➕ إضافة متغير في النطاق الحالي
     */
    uint32_t addVariable(const std::string& name, const std::string& type,
                        bool mutable_ = true) {
        // التحقق من التكرار في النطاق الحالي
        if (table_.existsInCurrentScope(name)) {
            throw std::runtime_error("الاسم '" + name + "' موجود بالفعل في هذا النطاق");
        }
        
        uint32_t index = table_.addVariable(name, type, mutable_);
        
        if (!scopeStack_.empty()) {
            scopeStack_.back().push_back(index);
        }
        
        return index;
    }
    
    /**
     * 🔍 البحث عن متغير
     */
    int32_t find(const std::string& name) const {
        return table_.find(name);
    }
    
    /**
     * 📊 الجدول الداخلي
     */
    const SymbolTable& table() const { return table_; }
    SymbolTable& table() { return table_; }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء جدول رموز جديد
 */
void* sad_symbol_table_new(void) {
    return new sad::bytecode::SymbolTable();
}

/**
 * تحرير جدول الرموز
 */
void sad_symbol_table_free(void* table) {
    delete static_cast<sad::bytecode::SymbolTable*>(table);
}

/**
 * عدد الرموز
 */
size_t sad_symbol_table_size(void* table) {
    return static_cast<sad::bytecode::SymbolTable*>(table)->size();
}

/**
 * الدخول لنطاق جديد
 */
void sad_symbol_table_enter_scope(void* table) {
    static_cast<sad::bytecode::SymbolTable*>(table)->enterScope();
}

/**
 * الخروج من النطاق
 */
void sad_symbol_table_exit_scope(void* table) {
    static_cast<sad::bytecode::SymbolTable*>(table)->exitScope();
}

/**
 * إضافة متغير
 */
uint32_t sad_symbol_table_add_variable(void* table, const char* name, 
                                        const char* type, int mutable_) {
    return static_cast<sad::bytecode::SymbolTable*>(table)->addVariable(
        name, type, mutable_ != 0);
}

/**
 * إضافة دالة
 */
uint32_t sad_symbol_table_add_function(void* table, const char* name,
                                        const char* returnType) {
    return static_cast<sad::bytecode::SymbolTable*>(table)->addFunction(
        name, {}, returnType);
}

/**
 * البحث عن رمز
 */
int32_t sad_symbol_table_find(void* table, const char* name) {
    return static_cast<sad::bytecode::SymbolTable*>(table)->find(name);
}

/**
 * تفريغ الجدول للعرض
 */
const char* sad_symbol_table_dump(void* table) {
    static std::string dump;
    dump = static_cast<sad::bytecode::SymbolTable*>(table)->dump();
    return dump.c_str();
}

} // extern "C"
