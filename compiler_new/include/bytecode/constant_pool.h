/**
 * @file constant_pool.h
 * @brief (AR) جدول الثوابت لملفات البايت كود
 *        (EN) Constant pool for bytecode files
 *
 * @details
 * (AR) يحدد هذا الملف بنية الثابت (Constant) وجدول الثوابت (ConstantPool).
 * (EN) Defines the Constant struct and ConstantPool class.
 */

#pragma once

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
// هيكل الثابت / Constant Structure
// ==========================================================

struct Constant {
    ConstantType type;
    std::vector<uint8_t> data;
    
    union {
        bool boolValue;
        int64_t intValue;
        double floatValue;
    } cached;
    std::string stringValue;
    
    Constant() : type(ConstantType::NULL_VALUE) {
        cached.intValue = 0;
    }
    
    static Constant fromBool(bool value) {
        Constant c;
        c.type = ConstantType::BOOL;
        c.data.push_back(value ? 1 : 0);
        c.cached.boolValue = value;
        return c;
    }
    
    static Constant fromInt(int64_t value) {
        Constant c;
        c.cached.intValue = value;
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
            for (int i = 0; i < 4; ++i)
                c.data.push_back(static_cast<uint8_t>((v >> (i * 8)) & 0xFF));
        } else {
            c.type = ConstantType::INT64;
            for (int i = 0; i < 8; ++i)
                c.data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
        return c;
    }
    
    static Constant fromFloat(double value) {
        Constant c;
        c.type = ConstantType::FLOAT64;
        c.cached.floatValue = value;
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(bits));
        for (int i = 0; i < 8; ++i)
            c.data.push_back(static_cast<uint8_t>((bits >> (i * 8)) & 0xFF));
        return c;
    }
    
    static Constant fromString(const std::string& value) {
        Constant c;
        c.stringValue = value;
        bool hasArabic = false;
        for (unsigned char ch : value) {
            if (ch >= 0xD8 && ch <= 0xDB) { hasArabic = true; break; }
        }
        c.type = hasArabic ? ConstantType::STRING_ARABIC : ConstantType::STRING;
        uint16_t len = static_cast<uint16_t>(
            std::min(value.size(), static_cast<size_t>(MAX_STRING_LENGTH)));
        c.data.push_back(static_cast<uint8_t>(len & 0xFF));
        c.data.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
        for (size_t i = 0; i < len; ++i)
            c.data.push_back(static_cast<uint8_t>(value[i]));
        return c;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        switch (type) {
            case ConstantType::NULL_VALUE: oss << "null"; break;
            case ConstantType::BOOL: oss << (cached.boolValue ? "true" : "false"); break;
            case ConstantType::INT8: case ConstantType::INT16:
            case ConstantType::INT32: case ConstantType::INT64:
                oss << cached.intValue; break;
            case ConstantType::FLOAT32: case ConstantType::FLOAT64:
                oss << cached.floatValue; break;
            case ConstantType::STRING: case ConstantType::STRING_ARABIC:
                oss << "\"" << stringValue << "\""; break;
            default: oss << "<" << constantTypeName(type) << ">"; break;
        }
        return oss.str();
    }
    
    size_t byteSize() const { return 1 + data.size(); }
};

// ==========================================================
// جدول الثوابت / Constant Pool
// ==========================================================

class ConstantPool {
private:
    std::vector<Constant> constants_;
    std::unordered_map<std::string, uint16_t> stringIndex_;
    std::unordered_map<int64_t, uint16_t> intIndex_;
    std::unordered_map<double, uint16_t> floatIndex_;
    
public:
    size_t size() const { return constants_.size(); }
    bool empty() const { return constants_.empty(); }
    
    const Constant& get(uint16_t index) const {
        if (index >= constants_.size())
            throw std::out_of_range("Constant pool index out of range");
        return constants_[index];
    }
    
    const std::vector<Constant>& getAll() const { return constants_; }
    
    uint16_t addNull() {
        Constant c;
        c.type = ConstantType::NULL_VALUE;
        return addConstant(c);
    }
    
    uint16_t addBool(bool value) { return addConstant(Constant::fromBool(value)); }
    
    uint16_t addInt(int64_t value) {
        auto it = intIndex_.find(value);
        if (it != intIndex_.end()) return it->second;
        uint16_t index = addConstant(Constant::fromInt(value));
        intIndex_[value] = index;
        return index;
    }
    
    uint16_t addFloat(double value) {
        auto it = floatIndex_.find(value);
        if (it != floatIndex_.end()) return it->second;
        uint16_t index = addConstant(Constant::fromFloat(value));
        floatIndex_[value] = index;
        return index;
    }
    
    uint16_t addString(const std::string& value) {
        auto it = stringIndex_.find(value);
        if (it != stringIndex_.end()) return it->second;
        uint16_t index = addConstant(Constant::fromString(value));
        stringIndex_[value] = index;
        return index;
    }
    
    uint16_t addConstant(const Constant& constant) {
        if (constants_.size() >= MAX_CONSTANT_POOL_SIZE)
            throw std::overflow_error("Constant pool is full");
        uint16_t index = static_cast<uint16_t>(constants_.size());
        constants_.push_back(constant);
        return index;
    }
    
    int32_t findString(const std::string& value) const {
        auto it = stringIndex_.find(value);
        return (it != stringIndex_.end()) ? static_cast<int32_t>(it->second) : -1;
    }
    
    int32_t findInt(int64_t value) const {
        auto it = intIndex_.find(value);
        return (it != intIndex_.end()) ? static_cast<int32_t>(it->second) : -1;
    }
    
    std::vector<uint8_t> serialize() const;
    static ConstantPool deserialize(const uint8_t* data, size_t length);
    std::string dump() const;
    size_t totalBytes() const;
    
    void clear() {
        constants_.clear();
        stringIndex_.clear();
        intIndex_.clear();
        floatIndex_.clear();
    }
};

} // namespace bytecode
} // namespace sad
