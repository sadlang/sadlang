/**
 * ==========================================================
 * ملف: encoder.cpp
 * الوصف: ترميز تعليمات Bytecode
 * المهمة: T272 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🤔 ما هو الـ Encoder (المُرمِّز)؟
 * --------------------------------
 * المُرمِّز يحوّل التعليمات من شكل مفهوم للإنسان إلى
 * بايتات يفهمها الحاسوب.
 * 
 * مثال:
 * ------
 * التعليمة: "اضف 5 و 3"
 *           ↓ (ترميز)
 * البايتات: 20 05 00 00 00 03 00 00 00
 *           ^  ^---------- ^----------
 *           |      |           |
 *           ADD  الرقم 5    الرقم 3
 * 
 * 📦 بنية التعليمة المُرمَّزة:
 * ---------------------------
 * كل تعليمة تتكون من:
 * 
 * ┌────────┬──────────┬───────────────────┐
 * │ Opcode │ Metadata │ Operands (معاملات) │
 * │ 1 بايت │ 1 بايت   │ متغير الطول        │
 * └────────┴──────────┴───────────────────┘
 * 
 * 📊 أنواع المعاملات:
 * ------------------
 * - فوري (Immediate): القيمة مباشرة في التعليمة
 * - مرجع (Reference): فهرس في جدول الثوابت
 * - محلي (Local): فهرس متغير محلي
 * - عام (Global): فهرس متغير عام
 */

#include "format.h"
#include <vector>
#include <cstring>
#include <stdexcept>
#include <algorithm>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 أنواع المعاملات
// ==========================================================

/**
 * 🏷️ نوع المعامل
 */
enum class OperandType : uint8_t {
    NONE = 0x00,            // لا معامل
    IMMEDIATE_I8 = 0x01,    // قيمة فورية 8 بت
    IMMEDIATE_I16 = 0x02,   // قيمة فورية 16 بت
    IMMEDIATE_I32 = 0x03,   // قيمة فورية 32 بت
    IMMEDIATE_I64 = 0x04,   // قيمة فورية 64 بت
    IMMEDIATE_F32 = 0x05,   // قيمة عشرية 32 بت
    IMMEDIATE_F64 = 0x06,   // قيمة عشرية 64 بت
    CONST_INDEX = 0x10,     // فهرس في جدول الثوابت
    LOCAL_INDEX = 0x11,     // فهرس متغير محلي
    GLOBAL_INDEX = 0x12,    // فهرس متغير عام
    FIELD_INDEX = 0x13,     // فهرس حقل
    FUNC_INDEX = 0x14,      // فهرس دالة
    LABEL_OFFSET = 0x20,    // إزاحة للقفز
    TYPE_INDEX = 0x21       // فهرس نوع
};

// ==========================================================
// 📌 فئة InstructionEncoder (مُرمِّز التعليمات)
// ==========================================================

/**
 * ✍️ مُرمِّز تعليمة واحدة
 * 
 * 📚 الاستخدام:
 * -------------
 * InstructionEncoder encoder;
 * 
 * // ترميز PUSH_INT32 5
 * encoder.encode(Opcode::PUSH_INT32, 5);
 * 
 * // الحصول على البايتات
 * auto bytes = encoder.getBytes();
 */
class InstructionEncoder {
private:
    std::vector<uint8_t> buffer_;
    
public:
    /**
     * 🧹 مسح المخزن
     */
    void clear() {
        buffer_.clear();
    }
    
    /**
     * 📊 الحصول على البايتات
     */
    const std::vector<uint8_t>& getBytes() const {
        return buffer_;
    }
    
    /**
     * 📊 حجم البايتات
     */
    size_t size() const {
        return buffer_.size();
    }
    
    // ═══════════════════════════════════════════════════════
    // كتابة البايتات الخام
    // ═══════════════════════════════════════════════════════
    
    void writeByte(uint8_t value) {
        buffer_.push_back(value);
    }
    
    void writeInt16(int16_t value) {
        buffer_.push_back(static_cast<uint8_t>(value & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    }
    
    void writeUInt16(uint16_t value) {
        buffer_.push_back(static_cast<uint8_t>(value & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    }
    
    void writeInt32(int32_t value) {
        buffer_.push_back(static_cast<uint8_t>(value & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }
    
    void writeUInt32(uint32_t value) {
        buffer_.push_back(static_cast<uint8_t>(value & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        buffer_.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }
    
    void writeInt64(int64_t value) {
        for (int i = 0; i < 8; ++i) {
            buffer_.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    }
    
    void writeFloat32(float value) {
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        writeUInt32(bits);
    }
    
    void writeFloat64(double value) {
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        writeInt64(static_cast<int64_t>(bits));
    }
    
    // ═══════════════════════════════════════════════════════
    // ترميز التعليمات
    // ═══════════════════════════════════════════════════════
    
    /**
     * ✍️ ترميز تعليمة بدون معاملات
     */
    void encode(Opcode op) {
        writeByte(static_cast<uint8_t>(op));
    }
    
    /**
     * ✍️ ترميز تعليمة مع معامل بايت واحد
     */
    void encode(Opcode op, uint8_t operand) {
        writeByte(static_cast<uint8_t>(op));
        writeByte(operand);
    }
    
    /**
     * ✍️ ترميز تعليمة مع معامل 16 بت
     */
    void encode(Opcode op, uint16_t operand) {
        writeByte(static_cast<uint8_t>(op));
        writeUInt16(operand);
    }
    
    /**
     * ✍️ ترميز تعليمة مع معامل 32 بت
     */
    void encode(Opcode op, int32_t operand) {
        writeByte(static_cast<uint8_t>(op));
        writeInt32(operand);
    }
    
    /**
     * ✍️ ترميز تعليمة مع معامل 64 بت
     */
    void encode(Opcode op, int64_t operand) {
        writeByte(static_cast<uint8_t>(op));
        writeInt64(operand);
    }
    
    /**
     * ✍️ ترميز تعليمة مع معامل عشري 32 بت
     */
    void encodeFloat32(Opcode op, float operand) {
        writeByte(static_cast<uint8_t>(op));
        writeFloat32(operand);
    }
    
    /**
     * ✍️ ترميز تعليمة مع معامل عشري 64 بت
     */
    void encodeFloat64(Opcode op, double operand) {
        writeByte(static_cast<uint8_t>(op));
        writeFloat64(operand);
    }
    
    /**
     * ✍️ ترميز تعليمة مع معاملين
     */
    void encode(Opcode op, uint16_t operand1, uint16_t operand2) {
        writeByte(static_cast<uint8_t>(op));
        writeUInt16(operand1);
        writeUInt16(operand2);
    }
    
    // ═══════════════════════════════════════════════════════
    // تعليمات محددة
    // ═══════════════════════════════════════════════════════
    
    /**
     * 📦 ضع قيمة فارغة
     */
    void encodePushNull() {
        encode(Opcode::PUSH_NULL);
    }
    
    /**
     * 📦 ضع صح
     */
    void encodePushTrue() {
        encode(Opcode::PUSH_TRUE);
    }
    
    /**
     * 📦 ضع خطأ
     */
    void encodePushFalse() {
        encode(Opcode::PUSH_FALSE);
    }
    
    /**
     * 📦 ضع عدد صحيح
     */
    void encodePushInt(int64_t value) {
        if (value >= -128 && value <= 127) {
            encode(Opcode::PUSH_INT8, static_cast<uint8_t>(value));
        } else if (value >= -32768 && value <= 32767) {
            encode(Opcode::PUSH_INT16, static_cast<uint16_t>(value));
        } else if (value >= INT32_MIN && value <= INT32_MAX) {
            encode(Opcode::PUSH_INT32, static_cast<int32_t>(value));
        } else {
            encode(Opcode::PUSH_INT64, value);
        }
    }
    
    /**
     * 📦 ضع عدد عشري
     */
    void encodePushFloat(double value) {
        // نحاول float32 أولاً إذا كانت الدقة كافية
        float f32 = static_cast<float>(value);
        if (static_cast<double>(f32) == value) {
            encodeFloat32(Opcode::PUSH_FLOAT32, f32);
        } else {
            encodeFloat64(Opcode::PUSH_FLOAT64, value);
        }
    }
    
    /**
     * 📦 ضع ثابت من الجدول
     */
    void encodePushConst(uint16_t index) {
        encode(Opcode::PUSH_CONST, index);
    }
    
    /**
     * 📍 قفز
     */
    void encodeJump(int32_t offset) {
        encode(Opcode::JMP, offset);
    }
    
    /**
     * 📍 قفز مشروط
     */
    void encodeJumpIf(int32_t offset) {
        encode(Opcode::JMP_IF, offset);
    }
    
    /**
     * 📍 قفز إذا لا
     */
    void encodeJumpIfNot(int32_t offset) {
        encode(Opcode::JMP_IF_NOT, offset);
    }
    
    /**
     * 📞 استدعاء دالة
     */
    void encodeCall(uint16_t funcIndex, uint8_t argCount) {
        writeByte(static_cast<uint8_t>(Opcode::CALL));
        writeUInt16(funcIndex);
        writeByte(argCount);
    }
    
    /**
     * 📞 استدعاء طريقة
     */
    void encodeCallMethod(uint16_t methodIndex, uint8_t argCount) {
        writeByte(static_cast<uint8_t>(Opcode::CALL_METHOD));
        writeUInt16(methodIndex);
        writeByte(argCount);
    }
    
    /**
     * 💾 حمّل متغير محلي
     */
    void encodeLoadLocal(uint16_t index) {
        encode(Opcode::LOAD_LOCAL, index);
    }
    
    /**
     * 💾 خزّن في متغير محلي
     */
    void encodeStoreLocal(uint16_t index) {
        encode(Opcode::STORE_LOCAL, index);
    }
    
    /**
     * 💾 حمّل متغير عام
     */
    void encodeLoadGlobal(uint16_t index) {
        encode(Opcode::LOAD_GLOBAL, index);
    }
    
    /**
     * 💾 خزّن في متغير عام
     */
    void encodeStoreGlobal(uint16_t index) {
        encode(Opcode::STORE_GLOBAL, index);
    }
    
    // ═══════════════════════════════════════════════════════
    // تعليمات الملكية (خاصة بلغة ص!)
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🏠 تخصيص ذاكرة
     */
    void encodeAlloc(uint16_t typeIndex) {
        encode(Opcode::ALLOC, typeIndex);
    }
    
    /**
     * 🏠 تحرير ذاكرة
     */
    void encodeDealloc() {
        encode(Opcode::DEALLOC);
    }
    
    /**
     * 📖 اقتراض للقراءة
     */
    void encodeBorrow(uint16_t varIndex) {
        encode(Opcode::BORROW, varIndex);
    }
    
    /**
     * ✏️ اقتراض للتعديل
     */
    void encodeBorrowMut(uint16_t varIndex) {
        encode(Opcode::BORROW_MUT, varIndex);
    }
    
    /**
     * 🔚 إنهاء الاقتراض
     */
    void encodeEndBorrow() {
        encode(Opcode::END_BORROW);
    }
    
    /**
     * 📦 نقل الملكية
     */
    void encodeMove(uint16_t destIndex) {
        encode(Opcode::MOVE, destIndex);
    }
    
    /**
     * 📋 نسخ القيمة
     */
    void encodeCopy() {
        encode(Opcode::COPY);
    }
    
    /**
     * 🔄 استنساخ عميق
     */
    void encodeClone() {
        encode(Opcode::CLONE);
    }
    
    /**
     * 🗑️ إسقاط/تدمير
     */
    void encodeDrop() {
        encode(Opcode::DROP);
    }
};

// ==========================================================
// 📌 فئة BytecodeBuilder (بناء Bytecode كامل)
// ==========================================================

/**
 * 🏗️ بناء ملف Bytecode كامل
 * 
 * 📚 الاستخدام:
 * -------------
 * BytecodeBuilder builder;
 * 
 * // إضافة ثوابت
 * uint16_t strIdx = builder.addString("مرحباً");
 * uint16_t numIdx = builder.addInt(42);
 * 
 * // بدء دالة
 * builder.beginFunction("رئيسية", 0, 2);
 * 
 * // إضافة تعليمات
 * builder.pushConst(strIdx);
 * builder.call(printFuncIdx, 1);
 * builder.ret();
 * 
 * // إنهاء الدالة
 * builder.endFunction();
 * 
 * // الحصول على الملف
 * auto file = builder.build();
 */
class BytecodeBuilder {
private:
    // جدول الثوابت
    std::vector<uint8_t> constantPool_;
    uint16_t constantCount_ = 0;
    
    // جدول الرموز
    std::vector<SymbolEntry> symbols_;
    
    // الكود
    std::vector<uint8_t> code_;
    
    // الدالة الحالية
    FunctionInfo* currentFunction_ = nullptr;
    std::vector<FunctionInfo> functions_;
    
    // المُرمِّز
    InstructionEncoder encoder_;
    
    // للأعلام
    uint8_t fileFlags_ = 0;

public:
    // ═══════════════════════════════════════════════════════
    // إدارة الثوابت
    // ═══════════════════════════════════════════════════════
    
    /**
     * ➕ إضافة عدد صحيح
     */
    uint16_t addInt(int64_t value) {
        uint16_t index = constantCount_++;
        
        // تحديد الحجم الأمثل
        if (value >= -128 && value <= 127) {
            constantPool_.push_back(static_cast<uint8_t>(ConstantType::INT8));
            constantPool_.push_back(static_cast<uint8_t>(value));
        } else if (value >= -32768 && value <= 32767) {
            constantPool_.push_back(static_cast<uint8_t>(ConstantType::INT16));
            auto v = static_cast<int16_t>(value);
            constantPool_.push_back(static_cast<uint8_t>(v & 0xFF));
            constantPool_.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        } else if (value >= INT32_MIN && value <= INT32_MAX) {
            constantPool_.push_back(static_cast<uint8_t>(ConstantType::INT32));
            auto v = static_cast<int32_t>(value);
            for (int i = 0; i < 4; ++i) {
                constantPool_.push_back(static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
            }
        } else {
            constantPool_.push_back(static_cast<uint8_t>(ConstantType::INT64));
            for (int i = 0; i < 8; ++i) {
                constantPool_.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
            }
        }
        
        return index;
    }
    
    /**
     * ➕ إضافة عدد عشري
     */
    uint16_t addFloat(double value) {
        uint16_t index = constantCount_++;
        
        constantPool_.push_back(static_cast<uint8_t>(ConstantType::FLOAT64));
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        for (int i = 0; i < 8; ++i) {
            constantPool_.push_back(static_cast<uint8_t>((bits >> (i * 8)) & 0xFF));
        }
        
        return index;
    }
    
    /**
     * ➕ إضافة نص
     */
    uint16_t addString(const std::string& str) {
        uint16_t index = constantCount_++;
        
        // تحديد نوع النص (عربي أو عادي)
        bool hasArabic = false;
        for (unsigned char c : str) {
            if (c >= 0xD8 && c <= 0xDB) { // UTF-8 Arabic range
                hasArabic = true;
                break;
            }
        }
        
        constantPool_.push_back(static_cast<uint8_t>(
            hasArabic ? ConstantType::STRING_ARABIC : ConstantType::STRING));
        
        // الطول (16 بت)
        uint16_t len = static_cast<uint16_t>(std::min(str.size(), 
                                                      static_cast<size_t>(MAX_STRING_LENGTH)));
        constantPool_.push_back(static_cast<uint8_t>(len & 0xFF));
        constantPool_.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
        
        // المحتوى
        for (size_t i = 0; i < len; ++i) {
            constantPool_.push_back(static_cast<uint8_t>(str[i]));
        }
        
        return index;
    }
    
    /**
     * ➕ إضافة قيمة منطقية
     */
    uint16_t addBool(bool value) {
        uint16_t index = constantCount_++;
        constantPool_.push_back(static_cast<uint8_t>(ConstantType::BOOL));
        constantPool_.push_back(value ? 1 : 0);
        return index;
    }
    
    // ═══════════════════════════════════════════════════════
    // إدارة الدوال
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🏁 بدء دالة جديدة
     */
    void beginFunction(const std::string& name, uint16_t paramCount, uint16_t localCount) {
        FunctionInfo func;
        func.name_index = addString(name);
        func.code_offset = static_cast<uint32_t>(code_.size());
        func.param_count = paramCount;
        func.local_count = localCount;
        func.stack_size = 0;
        func.flags = 0;
        
        functions_.push_back(func);
        currentFunction_ = &functions_.back();
        
        encoder_.clear();
    }
    
    /**
     * 🏁 إنهاء الدالة
     */
    void endFunction() {
        if (currentFunction_) {
            // نسخ التعليمات إلى الكود الرئيسي
            const auto& bytes = encoder_.getBytes();
            code_.insert(code_.end(), bytes.begin(), bytes.end());
            
            currentFunction_->code_length = static_cast<uint32_t>(bytes.size());
            currentFunction_ = nullptr;
        }
    }
    
    // ═══════════════════════════════════════════════════════
    // إضافة التعليمات (تُضاف للدالة الحالية)
    // ═══════════════════════════════════════════════════════
    
    void pushNull() { encoder_.encodePushNull(); }
    void pushTrue() { encoder_.encodePushTrue(); }
    void pushFalse() { encoder_.encodePushFalse(); }
    void pushInt(int64_t value) { encoder_.encodePushInt(value); }
    void pushFloat(double value) { encoder_.encodePushFloat(value); }
    void pushConst(uint16_t index) { encoder_.encodePushConst(index); }
    
    void pop() { encoder_.encode(Opcode::POP); }
    void dup() { encoder_.encode(Opcode::DUP); }
    void swap() { encoder_.encode(Opcode::SWAP); }
    
    void add() { encoder_.encode(Opcode::ADD); }
    void sub() { encoder_.encode(Opcode::SUB); }
    void mul() { encoder_.encode(Opcode::MUL); }
    void div() { encoder_.encode(Opcode::DIV); }
    void mod() { encoder_.encode(Opcode::MOD); }
    void neg() { encoder_.encode(Opcode::NEG); }
    
    void eq() { encoder_.encode(Opcode::EQ); }
    void ne() { encoder_.encode(Opcode::NE); }
    void lt() { encoder_.encode(Opcode::LT); }
    void le() { encoder_.encode(Opcode::LE); }
    void gt() { encoder_.encode(Opcode::GT); }
    void ge() { encoder_.encode(Opcode::GE); }
    
    void andOp() { encoder_.encode(Opcode::AND); }
    void orOp() { encoder_.encode(Opcode::OR); }
    void notOp() { encoder_.encode(Opcode::NOT); }
    
    void jump(int32_t offset) { encoder_.encodeJump(offset); }
    void jumpIf(int32_t offset) { encoder_.encodeJumpIf(offset); }
    void jumpIfNot(int32_t offset) { encoder_.encodeJumpIfNot(offset); }
    
    void call(uint16_t funcIndex, uint8_t argCount) { 
        encoder_.encodeCall(funcIndex, argCount); 
    }
    void ret() { encoder_.encode(Opcode::RET); }
    void retValue() { encoder_.encode(Opcode::RET_VALUE); }
    
    void loadLocal(uint16_t index) { encoder_.encodeLoadLocal(index); }
    void storeLocal(uint16_t index) { encoder_.encodeStoreLocal(index); }
    void loadGlobal(uint16_t index) { encoder_.encodeLoadGlobal(index); }
    void storeGlobal(uint16_t index) { encoder_.encodeStoreGlobal(index); }
    
    // تعليمات الملكية
    void alloc(uint16_t typeIndex) { encoder_.encodeAlloc(typeIndex); }
    void dealloc() { encoder_.encodeDealloc(); }
    void borrow(uint16_t varIndex) { encoder_.encodeBorrow(varIndex); }
    void borrowMut(uint16_t varIndex) { encoder_.encodeBorrowMut(varIndex); }
    void endBorrow() { encoder_.encodeEndBorrow(); }
    void move(uint16_t destIndex) { encoder_.encodeMove(destIndex); }
    void copy() { encoder_.encodeCopy(); }
    void clone() { encoder_.encodeClone(); }
    void drop() { encoder_.encodeDrop(); }
    
    void halt() { encoder_.encode(Opcode::HALT); }
    
    // ═══════════════════════════════════════════════════════
    // الموقع الحالي (للقفز)
    // ═══════════════════════════════════════════════════════
    
    /**
     * 📍 الموقع الحالي في الكود
     */
    size_t currentOffset() const {
        return code_.size() + encoder_.size();
    }
    
    // ═══════════════════════════════════════════════════════
    // بناء الملف النهائي
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🏗️ بناء ملف Bytecode كامل
     */
    std::vector<uint8_t> build() {
        std::vector<uint8_t> file;
        
        // 1. حساب الأحجام
        size_t headerSize = sizeof(FileHeader);
        size_t sectionHeadersSize = 3 * sizeof(SectionHeader); // 3 أقسام
        size_t constantPoolSize = constantPool_.size();
        size_t codeSize = code_.size();
        
        size_t totalSize = headerSize + sectionHeadersSize + 
                          constantPoolSize + codeSize;
        
        file.reserve(totalSize);
        
        // 2. كتابة رأس الملف
        FileHeader header;
        writeMagic(header.magic);
        header.version_major = VERSION_MAJOR;
        header.version_minor = VERSION_MINOR;
        header.version_patch = VERSION_PATCH;
        header.flags = fileFlags_;
        header.section_count = 3;
        header.total_size = static_cast<uint32_t>(totalSize);
        
        const uint8_t* headerBytes = reinterpret_cast<const uint8_t*>(&header);
        file.insert(file.end(), headerBytes, headerBytes + sizeof(header));
        
        // 3. كتابة رؤوس الأقسام
        uint32_t offset = static_cast<uint32_t>(headerSize + sectionHeadersSize);
        
        // قسم الثوابت
        SectionHeader constSection;
        constSection.type = SectionType::CONSTANT_POOL;
        constSection.flags = 0;
        constSection.reserved = 0;
        constSection.offset = offset;
        constSection.size = static_cast<uint32_t>(constantPoolSize);
        constSection.checksum = calculateChecksum(constantPool_.data(), constantPoolSize);
        
        const uint8_t* constSectionBytes = reinterpret_cast<const uint8_t*>(&constSection);
        file.insert(file.end(), constSectionBytes, constSectionBytes + sizeof(constSection));
        offset += static_cast<uint32_t>(constantPoolSize);
        
        // قسم الكود
        SectionHeader codeSection;
        codeSection.type = SectionType::CODE;
        codeSection.flags = 0;
        codeSection.reserved = 0;
        codeSection.offset = offset;
        codeSection.size = static_cast<uint32_t>(codeSize);
        codeSection.checksum = calculateChecksum(code_.data(), codeSize);
        
        const uint8_t* codeSectionBytes = reinterpret_cast<const uint8_t*>(&codeSection);
        file.insert(file.end(), codeSectionBytes, codeSectionBytes + sizeof(codeSection));
        
        // 4. كتابة البيانات
        file.insert(file.end(), constantPool_.begin(), constantPool_.end());
        file.insert(file.end(), code_.begin(), code_.end());
        
        return file;
    }
    
    /**
     * 🏳️ تعيين أعلام الملف
     */
    void setFlags(uint8_t flags) {
        fileFlags_ = flags;
    }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء encoder جديد
 */
void* sad_instruction_encoder_new(void) {
    return new sad::bytecode::InstructionEncoder();
}

/**
 * تحرير encoder
 */
void sad_instruction_encoder_free(void* encoder) {
    delete static_cast<sad::bytecode::InstructionEncoder*>(encoder);
}

/**
 * مسح المخزن
 */
void sad_instruction_encoder_clear(void* encoder) {
    static_cast<sad::bytecode::InstructionEncoder*>(encoder)->clear();
}

/**
 * الحصول على البايتات
 */
const uint8_t* sad_instruction_encoder_bytes(void* encoder, size_t* length) {
    auto* enc = static_cast<sad::bytecode::InstructionEncoder*>(encoder);
    *length = enc->size();
    return enc->getBytes().data();
}

/**
 * ترميز تعليمة بدون معاملات
 */
void sad_encode_op(void* encoder, uint8_t opcode) {
    static_cast<sad::bytecode::InstructionEncoder*>(encoder)->encode(
        static_cast<sad::bytecode::Opcode>(opcode));
}

/**
 * إنشاء builder جديد
 */
void* sad_bytecode_builder_new(void) {
    return new sad::bytecode::BytecodeBuilder();
}

/**
 * تحرير builder
 */
void sad_bytecode_builder_free(void* builder) {
    delete static_cast<sad::bytecode::BytecodeBuilder*>(builder);
}

/**
 * إضافة نص للثوابت
 */
uint16_t sad_bytecode_builder_add_string(void* builder, const char* str) {
    return static_cast<sad::bytecode::BytecodeBuilder*>(builder)->addString(str);
}

/**
 * إضافة عدد صحيح للثوابت
 */
uint16_t sad_bytecode_builder_add_int(void* builder, int64_t value) {
    return static_cast<sad::bytecode::BytecodeBuilder*>(builder)->addInt(value);
}

/**
 * بناء الملف النهائي
 */
void sad_bytecode_builder_build(void* builder, uint8_t** output, size_t* length) {
    auto file = static_cast<sad::bytecode::BytecodeBuilder*>(builder)->build();
    *length = file.size();
    *output = new uint8_t[file.size()];
    std::copy(file.begin(), file.end(), *output);
}

} // extern "C"
