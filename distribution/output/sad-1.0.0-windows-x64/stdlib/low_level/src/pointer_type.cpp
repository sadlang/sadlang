/*
 * ============================================================================
 * تنفيذ نظام المؤشرات للغة ص - Pointer System Implementation for Sad Language
 * ============================================================================
 * 
 * هذا الملف يحتوي على التنفيذ الفعلي لنظام المؤشرات
 * This file contains the actual implementation of the pointer system
 * ============================================================================
 */

#include "pointer_type.h"
#include <stdexcept>
#include <sstream>

namespace Sad {
namespace LowLevel {

// ============================================================================
// PointerType Implementation
// ============================================================================

PointerType::PointerType(BasicType elementType)
    : elementType_(elementType)
    , level_(1) {
}

PointerType::PointerType(BasicType elementType, int level)
    : elementType_(elementType)
    , level_(level) {
    if (level < 1) {
        throw std::runtime_error(
            "خطأ: مستوى المؤشر يجب أن يكون 1 على الأقل\n"
            "Error: Pointer level must be at least 1"
        );
    }
}

std::string PointerType::toString() const {
    std::ostringstream oss;
    
    // Build type name with proper level
    // بناء اسم النوع مع المستوى الصحيح
    std::string baseType;
    switch (elementType_) {
        case BasicType::INTEGER:
            baseType = "عدد_صحيح / int";
            break;
        case BasicType::FLOAT:
            baseType = "عدد_عشري / float";
            break;
        case BasicType::STRING:
            baseType = "نص / string";
            break;
        case BasicType::BOOLEAN:
            baseType = "منطقي / bool";
            break;
        case BasicType::VOID:
            baseType = "فراغ / void";
            break;
    }
    
    // Add pointer symbols
    // إضافة رموز المؤشر
    for (int i = 0; i < level_; ++i) {
        baseType += "*";
    }
    
    oss << "مؤشر<" << baseType << ">";
    return oss.str();
}

// ============================================================================
// PointerManager Implementation
// ============================================================================

PointerManager& PointerManager::getInstance() {
    static PointerManager instance;
    return instance;
}

uint64_t PointerManager::createPointer(const PointerType& type, uint64_t address) {
    PointerInfo info(address, type, false, type.getElementType() == BasicType::VOID ? 0 : sizeof(uint64_t));
    
    uint64_t pointerId = nextId_++;
    pointers_[pointerId] = info;
    
    return pointerId;
}

uint64_t PointerManager::dereference(uint64_t pointerId) {
    // Validate pointer before dereferencing
    // التحقق من صلاحية المؤشر قبل إلغاء المرجعية
    if (!isValidPointer(pointerId)) {
        throw std::runtime_error(
            "خطأ: محاولة إلغاء مرجعية مؤشر غير صالح\n"
            "Error: Attempting to dereference invalid pointer"
        );
    }
    
    if (isNull(pointerId)) {
        throw std::runtime_error(
            "خطأ: محاولة إلغاء مرجعية مؤشر NULL\n"
            "Error: Attempting to dereference NULL pointer"
        );
    }
    
    return pointers_[pointerId].address;
}

void PointerManager::writeThrough(uint64_t pointerId, uint64_t value) {
    // Validate pointer before writing
    // التحقق من صلاحية المؤشر قبل الكتابة
    if (!isValidPointer(pointerId)) {
        throw std::runtime_error(
            "خطأ: محاولة الكتابة عبر مؤشر غير صالح\n"
            "Error: Attempting to write through invalid pointer"
        );
    }
    
    if (isNull(pointerId)) {
        throw std::runtime_error(
            "خطأ: محاولة الكتابة عبر مؤشر NULL\n"
            "Error: Attempting to write through NULL pointer"
        );
    }
    
    // In a real implementation, this would write to actual memory
    // في تنفيذ حقيقي، سيتم الكتابة إلى الذاكرة الفعلية
    // For now, we just update the address value (simulated memory)
    pointers_[pointerId].address = value;
}

uint64_t PointerManager::getAddressOf(const std::string& varName) {
    // Check if variable already has an address
    // فحص إن كان للمتغير عنوان بالفعل
    auto it = variables_.find(varName);
    if (it != variables_.end()) {
        return it->second;
    }
    
    // Allocate new address for variable
    // تخصيص عنوان جديد للمتغير
    uint64_t newAddress = nextId_++;
    variables_[varName] = newAddress;
    
    return newAddress;
}

uint64_t PointerManager::pointerAdd(uint64_t pointerId, int64_t offset) {
    if (!isValidPointer(pointerId)) {
        throw std::runtime_error(
            "خطأ: محاولة إضافة إزاحة لمؤشر غير صالح\n"
            "Error: Attempting to add offset to invalid pointer"
        );
    }
    
    const PointerInfo& ptr = pointers_[pointerId];
    
    // Calculate new address based on element size
    // حساب العنوان الجديد بناءً على حجم العنصر
    uint64_t elementSize = ptr.size;
    if (elementSize == 0) {
        elementSize = 1;  // Treat void* as byte pointer
    }
    
    uint64_t newAddress = ptr.address + (offset * elementSize);
    
    // Create new pointer with calculated address
    // إنشاء مؤشر جديد بالعنوان المحسوب
    return createPointer(ptr.type, newAddress);
}

uint64_t PointerManager::pointerSub(uint64_t pointerId, int64_t offset) {
    if (!isValidPointer(pointerId)) {
        throw std::runtime_error(
            "خطأ: محاولة طرح إزاحة من مؤشر غير صالح\n"
            "Error: Attempting to subtract offset from invalid pointer"
        );
    }
    
    const PointerInfo& ptr = pointers_[pointerId];
    
    // Calculate new address based on element size
    // حساب العنوان الجديد بناءً على حجم العنصر
    uint64_t elementSize = ptr.size;
    if (elementSize == 0) {
        elementSize = 1;  // Treat void* as byte pointer
    }
    
    uint64_t newAddress = ptr.address - (offset * elementSize);
    
    // Create new pointer with calculated address
    // إنشاء مؤشر جديد بالعنوان المحسوب
    return createPointer(ptr.type, newAddress);
}

bool PointerManager::pointerEquals(uint64_t ptr1, uint64_t ptr2) {
    if (!isValidPointer(ptr1) || !isValidPointer(ptr2)) {
        return false;
    }
    
    return pointers_[ptr1].address == pointers_[ptr2].address;
}

bool PointerManager::pointerLessThan(uint64_t ptr1, uint64_t ptr2) {
    if (!isValidPointer(ptr1) || !isValidPointer(ptr2)) {
        return false;
    }
    
    return pointers_[ptr1].address < pointers_[ptr2].address;
}

bool PointerManager::isValidPointer(uint64_t pointerId) {
    return pointers_.find(pointerId) != pointers_.end();
}

bool PointerManager::isNull(uint64_t pointerId) {
    if (!isValidPointer(pointerId)) {
        return true;  // Invalid pointers are treated as NULL
    }
    
    return pointers_[pointerId].address == 0;
}

const PointerInfo* PointerManager::getPointerInfo(uint64_t pointerId) {
    if (!isValidPointer(pointerId)) {
        return nullptr;
    }
    
    return &pointers_[pointerId];
}

void PointerManager::clear() {
    pointers_.clear();
    variables_.clear();
    nextId_ = 1;
}

} // namespace LowLevel
} // namespace Sad
