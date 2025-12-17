/**
 * @file bytecode_file.cpp
 * @brief تطبيق نظام ملفات البايت كود / Bytecode File System Implementation
 * @brief Implementation of .sbc file writer and loader
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/bytecode_file.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <chrono>

namespace Sad {
namespace Bytecode {

// ========================================
// CRC32 Table - جدول CRC32
// ========================================

static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

static void init_crc32_table() {
    if (crc32_table_initialized) return;
    
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
        crc32_table[i] = crc;
    }
    
    crc32_table_initialized = true;
}

uint32_t calculateCRC32(const void* data, size_t size) {
    init_crc32_table();
    
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ bytes[i]) & 0xFF];
    }
    
    return ~crc;
}

// ========================================
// FileHeader Implementation
// ========================================

bool FileHeader::isValid() const {
    return magic == MAGIC_NUMBER &&
           versionMajor == FORMAT_VERSION_MAJOR &&
           versionMinor <= FORMAT_VERSION_MINOR;
}

void FileHeader::print() const {
    std::cout << "========================================\n";
    std::cout << "معلومات ملف البايت كود / Bytecode File Info\n";
    std::cout << "========================================\n";
    std::cout << "الإصدار / Version: " << versionMajor << "." << versionMinor << "\n";
    std::cout << "العلامات / Flags: 0x" << std::hex << flags << std::dec << "\n";
    
    if (flags & FLAG_DEBUG_INFO) std::cout << "  - معلومات تصحيح / Debug Info\n";
    if (flags & FLAG_OPTIMIZED) std::cout << "  - مُحسَّن / Optimized\n";
    if (flags & FLAG_COMPRESSED) std::cout << "  - مضغوط / Compressed\n";
    if (flags & FLAG_64BIT) std::cout << "  - 64 بت / 64-bit\n";
    
    std::cout << "حجم الكود / Code Size: " << codeSize << " bytes\n";
    std::cout << "عدد الثوابت / Constants: " << constantsCount << "\n";
    std::cout << "عدد الرموز / Symbols: " << symbolsCount << "\n";
    
    // (AR) تحويل الطابع الزمني / (EN) Convert timestamp
    time_t time = static_cast<time_t>(timestamp);
    std::cout << "وقت الترجمة / Compiled: " << std::ctime(&time);
    std::cout << "========================================\n";
}

// ========================================
// BytecodeWriter Implementation
// ========================================

BytecodeWriter::BytecodeWriter(const std::string& filename)
    : checksum_(0)
    , finalized_(false)
{
    file_.open(filename, std::ios::binary);
    if (!file_.is_open()) {
        throw std::runtime_error("فشل فتح الملف للكتابة / Failed to open file for writing: " + filename);
    }
    
    init_crc32_table();
}

BytecodeWriter::~BytecodeWriter() {
    if (file_.is_open() && !finalized_) {
        finalize();
    }
}

bool BytecodeWriter::writeHeader(const FileHeader& header) {
    if (!file_.is_open()) return false;
    
    // (AR) اكتب الترويسة / (EN) Write header
    writeUInt32(header.magic);
    writeUInt16(header.versionMajor);
    writeUInt16(header.versionMinor);
    writeUInt32(header.flags);
    writeUInt64(header.timestamp);
    writeUInt32(header.codeSize);
    writeUInt32(header.constantsCount);
    writeUInt32(header.symbolsCount);
    
    return file_.good();
}

bool BytecodeWriter::writeConstants(const std::vector<VM::Value>& constants) {
    if (!file_.is_open()) return false;
    
    // (AR) اكتب علامة القسم / (EN) Write section marker
    writeByte(static_cast<uint8_t>(SectionType::CONSTANTS));
    
    // (AR) اكتب عدد الثوابت / (EN) Write constants count
    writeUInt32(static_cast<uint32_t>(constants.size()));
    
    // (AR) اكتب كل ثابت / (EN) Write each constant
    for (const VM::Value& value : constants) {
        if (value.isNull()) {
            writeByte(static_cast<uint8_t>(ConstantType::NULL_VALUE));
        }
        else if (value.isBool()) {
            writeByte(static_cast<uint8_t>(ConstantType::BOOL_VALUE));
            writeByte(value.asBool() ? 1 : 0);
        }
        else if (value.isInt()) {
            writeByte(static_cast<uint8_t>(ConstantType::INT_VALUE));
            writeUInt64(static_cast<uint64_t>(value.asInt()));
        }
        else if (value.isFloat()) {
            writeByte(static_cast<uint8_t>(ConstantType::FLOAT_VALUE));
            writeDouble(value.asFloat());
        }
        else if (value.isString()) {
            writeByte(static_cast<uint8_t>(ConstantType::STRING_VALUE));
            VM::StringObject* str = value.asString();
            std::string strValue(str->chars, str->length);
            writeString(strValue);
        }
    }
    
    return file_.good();
}

bool BytecodeWriter::writeCode(const uint8_t* code, size_t size) {
    if (!file_.is_open() || !code) return false;
    
    // (AR) اكتب علامة القسم / (EN) Write section marker
    writeByte(static_cast<uint8_t>(SectionType::CODE));
    
    // (AR) اكتب حجم الكود / (EN) Write code size
    writeUInt32(static_cast<uint32_t>(size));
    
    // (AR) اكتب الكود / (EN) Write code
    file_.write(reinterpret_cast<const char*>(code), size);
    updateChecksum(code, size);
    
    return file_.good();
}

bool BytecodeWriter::writeDebugInfo(const DebugInfo& debug) {
    if (!file_.is_open()) return false;
    
    // (AR) اكتب علامة القسم / (EN) Write section marker
    writeByte(static_cast<uint8_t>(SectionType::DEBUG));
    
    // (AR) اكتب أرقام الأسطر / (EN) Write line numbers
    writeUInt32(static_cast<uint32_t>(debug.lineNumbers.size()));
    for (const auto& [offset, line] : debug.lineNumbers) {
        writeUInt32(offset);
        writeUInt32(line);
    }
    
    // (AR) اكتب ملفات المصدر / (EN) Write source files
    writeUInt32(static_cast<uint32_t>(debug.sourceFiles.size()));
    for (const std::string& file : debug.sourceFiles) {
        writeString(file);
    }
    
    return file_.good();
}

bool BytecodeWriter::writeSymbols(const std::vector<SymbolEntry>& symbols) {
    if (!file_.is_open()) return false;
    
    // (AR) اكتب علامة القسم / (EN) Write section marker
    writeByte(static_cast<uint8_t>(SectionType::SYMBOLS));
    
    // (AR) اكتب عدد الرموز / (EN) Write symbols count
    writeUInt32(static_cast<uint32_t>(symbols.size()));
    
    // (AR) اكتب كل رمز / (EN) Write each symbol
    for (const SymbolEntry& symbol : symbols) {
        writeUInt32(symbol.nameOffset);
        writeUInt32(symbol.nameLength);
        writeByte(symbol.type);
        writeByte(symbol.scope);
        writeUInt16(symbol.index);
    }
    
    return file_.good();
}

bool BytecodeWriter::writeChecksum() {
    if (!file_.is_open()) return false;
    
    // (AR) اكتب علامة القسم / (EN) Write section marker
    writeByte(static_cast<uint8_t>(SectionType::CHECKSUM));
    
    // (AR) اكتب المجموع الاختباري / (EN) Write checksum
    writeUInt32(checksum_);
    
    return file_.good();
}

bool BytecodeWriter::finalize() {
    if (!file_.is_open() || finalized_) return false;
    
    // (AR) اكتب المجموع الاختباري / (EN) Write checksum
    writeChecksum();
    
    file_.close();
    finalized_ = true;
    
    return true;
}

void BytecodeWriter::updateChecksum(const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        checksum_ = (checksum_ >> 8) ^ crc32_table[(checksum_ ^ bytes[i]) & 0xFF];
    }
}

void BytecodeWriter::writeByte(uint8_t value) {
    file_.write(reinterpret_cast<const char*>(&value), 1);
    updateChecksum(&value, 1);
}

void BytecodeWriter::writeUInt16(uint16_t value) {
    file_.write(reinterpret_cast<const char*>(&value), 2);
    updateChecksum(&value, 2);
}

void BytecodeWriter::writeUInt32(uint32_t value) {
    file_.write(reinterpret_cast<const char*>(&value), 4);
    updateChecksum(&value, 4);
}

void BytecodeWriter::writeUInt64(uint64_t value) {
    file_.write(reinterpret_cast<const char*>(&value), 8);
    updateChecksum(&value, 8);
}

void BytecodeWriter::writeDouble(double value) {
    file_.write(reinterpret_cast<const char*>(&value), 8);
    updateChecksum(&value, 8);
}

void BytecodeWriter::writeString(const std::string& str) {
    writeUInt32(static_cast<uint32_t>(str.length()));
    file_.write(str.c_str(), str.length());
    updateChecksum(str.c_str(), str.length());
}

// ========================================
// BytecodeLoader Implementation
// ========================================

BytecodeLoader::BytecodeLoader(const std::string& filename)
    : checksum_(0xFFFFFFFF)
    , calculatedChecksum_(0)
{
    file_.open(filename, std::ios::binary);
    if (!file_.is_open()) {
        setError("فشل فتح الملف للقراءة / Failed to open file for reading: " + filename);
    }
    
    init_crc32_table();
}

BytecodeLoader::~BytecodeLoader() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool BytecodeLoader::load() {
    if (!file_.is_open()) return false;
    
    // (AR) اقرأ الترويسة / (EN) Read header
    if (!readHeader()) return false;
    
    // (AR) اقرأ الأقسام / (EN) Read sections
    while (file_.good() && file_.peek() != EOF) {
        uint8_t sectionType = readByte();
        
        switch (static_cast<SectionType>(sectionType)) {
            case SectionType::CONSTANTS:
                if (!readConstants()) return false;
                break;
                
            case SectionType::CODE:
                if (!readCode()) return false;
                break;
                
            case SectionType::DEBUG:
                if (!readDebugInfo()) return false;
                break;
                
            case SectionType::SYMBOLS:
                if (!readSymbols()) return false;
                break;
                
            case SectionType::CHECKSUM:
                if (!readChecksum()) return false;
                return true; // (AR) انتهى التحميل / (EN) Load complete
                
            default:
                setError("نوع قسم غير معروف / Unknown section type");
                return false;
        }
    }
    
    return true;
}

bool BytecodeLoader::verify() {
    if (calculatedChecksum_ != checksum_) {
        setError("فشل التحقق من المجموع الاختباري / Checksum verification failed");
        return false;
    }
    
    if (!header_.isValid()) {
        setError("ترويسة غير صحيحة / Invalid header");
        return false;
    }
    
    return true;
}

bool BytecodeLoader::readHeader() {
    header_.magic = readUInt32();
    header_.versionMajor = readUInt16();
    header_.versionMinor = readUInt16();
    header_.flags = readUInt32();
    header_.timestamp = readUInt64();
    header_.codeSize = readUInt32();
    header_.constantsCount = readUInt32();
    header_.symbolsCount = readUInt32();
    
    if (!header_.isValid()) {
        setError("ترويسة ملف غير صحيحة / Invalid file header");
        return false;
    }
    
    return true;
}

bool BytecodeLoader::readConstants() {
    uint32_t count = readUInt32();
    constants_.reserve(count);
    
    for (uint32_t i = 0; i < count; ++i) {
        uint8_t typeB = readByte();
        ConstantType type = static_cast<ConstantType>(typeB);
        
        switch (type) {
            case ConstantType::NULL_VALUE:
                constants_.push_back(VM::Value::Null());
                break;
                
            case ConstantType::BOOL_VALUE: {
                bool value = readByte() != 0;
                constants_.push_back(VM::Value::Bool(value));
                break;
            }
            
            case ConstantType::INT_VALUE: {
                int64_t value = static_cast<int64_t>(readUInt64());
                constants_.push_back(VM::Value::Int(value));
                break;
            }
            
            case ConstantType::FLOAT_VALUE: {
                double value = readDouble();
                constants_.push_back(VM::Value::Float(value));
                break;
            }
            
            case ConstantType::STRING_VALUE: {
                uint32_t length = readUInt32();
                std::string str = readString(length);
                
                // Create StringObject
                VM::StringObject* strObj = new VM::StringObject();
                strObj->length = str.length();
                strObj->chars = new char[str.length() + 1];
                std::memcpy(strObj->chars, str.c_str(), str.length() + 1);
                strObj->hash = std::hash<std::string>{}(str);
                
                constants_.push_back(VM::Value::String(strObj));
                break;
            }
            
            default:
                setError("نوع ثابت غير معروف / Unknown constant type");
                return false;
        }
    }
    
    return true;
}

bool BytecodeLoader::readCode() {
    uint32_t size = readUInt32();
    code_.resize(size);
    
    file_.read(reinterpret_cast<char*>(code_.data()), size);
    updateChecksum(code_.data(), size);
    
    return file_.good();
}

bool BytecodeLoader::readDebugInfo() {
    // (AR) اقرأ أرقام الأسطر / (EN) Read line numbers
    uint32_t lineCount = readUInt32();
    debug_.lineNumbers.reserve(lineCount);
    
    for (uint32_t i = 0; i < lineCount; ++i) {
        uint32_t offset = readUInt32();
        uint32_t line = readUInt32();
        debug_.lineNumbers.emplace_back(offset, line);
    }
    
    // (AR) اقرأ ملفات المصدر / (EN) Read source files
    uint32_t fileCount = readUInt32();
    debug_.sourceFiles.reserve(fileCount);
    
    for (uint32_t i = 0; i < fileCount; ++i) {
        uint32_t length = readUInt32();
        debug_.sourceFiles.push_back(readString(length));
    }
    
    return true;
}

bool BytecodeLoader::readSymbols() {
    uint32_t count = readUInt32();
    symbols_.reserve(count);
    
    for (uint32_t i = 0; i < count; ++i) {
        SymbolEntry symbol;
        symbol.nameOffset = readUInt32();
        symbol.nameLength = readUInt32();
        symbol.type = readByte();
        symbol.scope = readByte();
        symbol.index = readUInt16();
        symbols_.push_back(symbol);
    }
    
    return true;
}

bool BytecodeLoader::readChecksum() {
    checksum_ = readUInt32();
    calculatedChecksum_ = ~calculatedChecksum_;
    return true;
}

void BytecodeLoader::updateChecksum(const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        calculatedChecksum_ = (calculatedChecksum_ >> 8) ^ 
                             crc32_table[(calculatedChecksum_ ^ bytes[i]) & 0xFF];
    }
}

uint8_t BytecodeLoader::readByte() {
    uint8_t value;
    file_.read(reinterpret_cast<char*>(&value), 1);
    updateChecksum(&value, 1);
    return value;
}

uint16_t BytecodeLoader::readUInt16() {
    uint16_t value;
    file_.read(reinterpret_cast<char*>(&value), 2);
    updateChecksum(&value, 2);
    return value;
}

uint32_t BytecodeLoader::readUInt32() {
    uint32_t value;
    file_.read(reinterpret_cast<char*>(&value), 4);
    updateChecksum(&value, 4);
    return value;
}

uint64_t BytecodeLoader::readUInt64() {
    uint64_t value;
    file_.read(reinterpret_cast<char*>(&value), 8);
    updateChecksum(&value, 8);
    return value;
}

double BytecodeLoader::readDouble() {
    double value;
    file_.read(reinterpret_cast<char*>(&value), 8);
    updateChecksum(&value, 8);
    return value;
}

std::string BytecodeLoader::readString(uint32_t length) {
    std::vector<char> buffer(length);
    file_.read(buffer.data(), length);
    updateChecksum(buffer.data(), length);
    return std::string(buffer.begin(), buffer.end());
}

void BytecodeLoader::setError(const std::string& msg) {
    error_ = msg;
    std::cerr << "[Error] " << msg << "\n";
}

} // namespace Bytecode
} // namespace Sad
