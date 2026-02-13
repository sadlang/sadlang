// ======================================================================
// bee_format.cpp - تنفيذ صيغة .نحلة / .bee Format Implementation
// ======================================================================

#include "format/bee_format.h"
#include <fstream>
#include <cstring>

namespace Sad {
namespace BeeFormat {

bool BeeFileWriter::writeToFile(const std::string& path) const {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    
    // (AR) كتابة الرأس
    BeeHeader header{};
    std::memcpy(header.magic, BEE_MAGIC.data(), 8);
    header.versionMajor = BEE_VERSION_MAJOR;
    header.versionMinor = BEE_VERSION_MINOR;
    header.arch = static_cast<uint16_t>(arch_);
    header.type = static_cast<uint16_t>(type_);
    header.entryPoint = entryPoint_;
    header.sectionCount = static_cast<uint32_t>(sections_.size());
    header.sectionTableOffset = sizeof(BeeHeader);
    header.flags = 0;
    header.capabilitiesOffset = 0;
    header.signatureOffset = 0;
    std::memset(header.reserved, 0, sizeof(header.reserved));
    
    // (AR) كتابة الرأس
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    // (AR) حساب إزاحات الأقسام
    uint64_t dataOffset = sizeof(BeeHeader) + 
                          sections_.size() * sizeof(SectionHeader);
    
    // (AR) إنشاء جدول النصوص
    std::vector<uint8_t> stringTable;
    std::vector<uint32_t> nameOffsets;
    for (const auto& sec : sections_) {
        nameOffsets.push_back(static_cast<uint32_t>(stringTable.size()));
        stringTable.insert(stringTable.end(), sec.name.begin(), sec.name.end());
        stringTable.push_back(0); // null terminator
    }
    
    // (AR) كتابة رؤوس الأقسام
    uint64_t currentOffset = dataOffset;
    for (size_t i = 0; i < sections_.size(); ++i) {
        SectionHeader secHeader{};
        secHeader.nameOffset = nameOffsets[i];
        secHeader.type = static_cast<uint32_t>(sections_[i].type);
        secHeader.virtualAddr = 0; // (AR) يُحسب لاحقاً بواسطة المحمّل
        secHeader.fileOffset = currentOffset;
        secHeader.size = sections_[i].data.size();
        secHeader.alignment = 16;
        secHeader.flags = sections_[i].flags;
        
        file.write(reinterpret_cast<const char*>(&secHeader), sizeof(secHeader));
        currentOffset += sections_[i].data.size();
    }
    
    // (AR) كتابة بيانات الأقسام
    for (const auto& sec : sections_) {
        if (!sec.data.empty()) {
            file.write(reinterpret_cast<const char*>(sec.data.data()), sec.data.size());
        }
    }
    
    return file.good();
}

bool BeeFileReader::readFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return false;
    
    // (AR) قراءة الرأس
    file.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!file.good()) return false;
    
    // (AR) التحقق من الرقم السحري
    if (!isValidBeeFile()) return false;
    
    return true;
}

} // namespace BeeFormat
} // namespace Sad
