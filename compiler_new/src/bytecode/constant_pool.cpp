/**
 * ==========================================================
 * constant_pool.cpp
 * Constant Pool implementation
 * Task: T274 - Bytecode System
 * ==========================================================
 */

#include "constant_pool.h"
#include <algorithm>
#include <cstring>

namespace sad {
namespace bytecode {

// ==========================================================
// Out-of-line implementations of ConstantPool methods
// ==========================================================

std::vector<uint8_t> ConstantPool::serialize() const {
    std::vector<uint8_t> result;
    uint16_t count = static_cast<uint16_t>(constants_.size());
    result.push_back(static_cast<uint8_t>(count & 0xFF));
    result.push_back(static_cast<uint8_t>((count >> 8) & 0xFF));
    for (const auto& constant : constants_) {
        result.push_back(static_cast<uint8_t>(constant.type));
        result.insert(result.end(), constant.data.begin(), constant.data.end());
    }
    return result;
}

ConstantPool ConstantPool::deserialize(const uint8_t* data, size_t length) {
    ConstantPool pool;
    if (length < 2) return pool;
    uint16_t count = static_cast<uint16_t>(data[0] | (data[1] << 8));
    size_t offset = 2;
    for (uint16_t i = 0; i < count && offset < length; ++i) {
        Constant c;
        c.type = static_cast<ConstantType>(data[offset++]);
        switch (c.type) {
            case ConstantType::NULL_VALUE: break;
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
                for (int j = 0; j < 4; ++j) c.data.push_back(data[offset++]);
                c.cached.intValue = static_cast<int32_t>(
                    c.data[0] | (c.data[1] << 8) | (c.data[2] << 16) | (c.data[3] << 24));
                break;
            case ConstantType::INT64:
                for (int j = 0; j < 8; ++j) c.data.push_back(data[offset++]);
                c.cached.intValue = 0;
                for (int j = 0; j < 8; ++j)
                    c.cached.intValue |= static_cast<int64_t>(c.data[j]) << (j * 8);
                break;
            case ConstantType::FLOAT64:
                for (int j = 0; j < 8; ++j) c.data.push_back(data[offset++]);
                { uint64_t bits = 0;
                  for (int j = 0; j < 8; ++j) bits |= static_cast<uint64_t>(c.data[j]) << (j * 8);
                  std::memcpy(&c.cached.floatValue, &bits, sizeof(bits)); }
                break;
            case ConstantType::STRING: case ConstantType::STRING_ARABIC:
                { uint16_t len = static_cast<uint16_t>(data[offset] | (data[offset+1] << 8));
                  c.data.push_back(data[offset++]);
                  c.data.push_back(data[offset++]);
                  for (uint16_t j = 0; j < len && offset < length; ++j) {
                      c.data.push_back(data[offset]);
                      c.stringValue += static_cast<char>(data[offset++]);
                  } }
                break;
            default: break;
        }
        pool.constants_.push_back(c);
    }
    for (uint16_t i = 0; i < pool.constants_.size(); ++i) {
        const auto& co = pool.constants_[i];
        switch (co.type) {
            case ConstantType::STRING: case ConstantType::STRING_ARABIC:
                pool.stringIndex_[co.stringValue] = i; break;
            case ConstantType::INT8: case ConstantType::INT16:
            case ConstantType::INT32: case ConstantType::INT64:
                pool.intIndex_[co.cached.intValue] = i; break;
            case ConstantType::FLOAT64:
                pool.floatIndex_[co.cached.floatValue] = i; break;
            default: break;
        }
    }
    return pool;
}

std::string ConstantPool::dump() const {
    std::ostringstream oss;
    oss << "===================================\n";
    oss << "        Constant Pool\n";
    oss << "===================================\n\n";
    oss << "Count: " << constants_.size() << "\n\n";
    for (size_t i = 0; i < constants_.size(); ++i) {
        const auto& c = constants_[i];
        oss << std::setw(6) << i << " | "
            << std::setw(12) << constantTypeName(c.type) << " | "
            << c.toString() << "\n";
    }
    return oss.str();
}

size_t ConstantPool::totalBytes() const {
    size_t total = 2;
    for (const auto& c : constants_) total += c.byteSize();
    return total;
}

// ==========================================================
// ConstantPoolBuilder
// ==========================================================

class ConstantPoolBuilder {
private:
    ConstantPool pool_;
public:
    uint16_t addLiteral(const std::string& type, const std::string& value) {
        if (type == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" || type == "int") {
            return pool_.addInt(std::stoll(value));
        } else if (type == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a" || type == "float") {
            return pool_.addFloat(std::stod(value));
        } else if (type == "\xd9\x86\xd8\xb5" || type == "string") {
            return pool_.addString(value);
        } else if (type == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a" || type == "bool") {
            return pool_.addBool(value == "\xd8\xb5\xd8\xad" || value == "true");
        } else {
            return pool_.addNull();
        }
    }
    ConstantPool build() { return std::move(pool_); }
    ConstantPool& current() { return pool_; }
};

} // namespace bytecode
} // namespace sad

// ==========================================================
// C API
// ==========================================================

extern "C" {

void* sad_constant_pool_new(void) {
    return new sad::bytecode::ConstantPool();
}

void sad_constant_pool_free(void* pool) {
    delete static_cast<sad::bytecode::ConstantPool*>(pool);
}

size_t sad_constant_pool_size(void* pool) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->size();
}

uint16_t sad_constant_pool_add_int(void* pool, int64_t value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->addInt(value);
}

uint16_t sad_constant_pool_add_float(void* pool, double value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->addFloat(value);
}

uint16_t sad_constant_pool_add_string(void* pool, const char* value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->addString(value);
}

int32_t sad_constant_pool_find_string(void* pool, const char* value) {
    return static_cast<sad::bytecode::ConstantPool*>(pool)->findString(value);
}

void sad_constant_pool_serialize(void* pool, uint8_t** output, size_t* length) {
    auto bytes = static_cast<sad::bytecode::ConstantPool*>(pool)->serialize();
    *length = bytes.size();
    *output = new uint8_t[bytes.size()];
    std::copy(bytes.begin(), bytes.end(), *output);
}

void* sad_constant_pool_deserialize(const uint8_t* data, size_t length) {
    auto pool = new sad::bytecode::ConstantPool();
    *pool = sad::bytecode::ConstantPool::deserialize(data, length);
    return pool;
}

const char* sad_constant_pool_dump(void* pool) {
    static std::string dump;
    dump = static_cast<sad::bytecode::ConstantPool*>(pool)->dump();
    return dump.c_str();
}

} // extern "C"