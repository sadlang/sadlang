/*
 * ============================================================================
 * نظام المؤشرات للغة ص - Pointer System for Sad Language
 * ============================================================================
 * 
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يحتوي على تعريفات نظام المؤشرات في لغة ص.
 * المؤشرات هي متغيرات تخزن عناوين الذاكرة، مما يسمح بالوصول المباشر
 * إلى الذاكرة وهو ضروري لبرمجة أنظمة التشغيل.
 * 
 * English Description:
 * -------------------
 * This file contains the pointer system definitions for Sad Language.
 * Pointers are variables that store memory addresses, allowing direct
 * memory access which is essential for OS programming.
 * 
 * المؤلف / Author: Sad Language Team
 * التاريخ / Date: December 2025
 * الإصدار / Version: 1.0.0
 * ============================================================================
 */

#ifndef SAD_POINTER_TYPE_H
#define SAD_POINTER_TYPE_H

#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>

namespace Sad {
namespace LowLevel {

// ============================================================================
// تعريفات الأنواع الأساسية / Basic Type Definitions
// ============================================================================

/**
 * أنواع البيانات الأساسية / Basic data types
 */
enum class BasicType {
    INTEGER,     // رقم صحيح / Integer
    FLOAT,       // رقم عشري / Float
    STRING,      // نص / String
    BOOLEAN,     // منطقي / Boolean
    VOID         // فراغ / Void
};

/**
 * نوع المؤشر - Pointer Type
 * 
 * يمثل نوع المؤشر في نظام الأنواع
 * Represents a pointer type in the type system
 */
class PointerType {
public:
    /**
     * المنشئ - Constructor
     * @param elementType نوع العنصر المشار إليه / Type of pointed-to element
     */
    explicit PointerType(BasicType elementType);
    
    /**
     * المنشئ للمؤشرات متعددة المستويات
     * Constructor for multi-level pointers
     * @param elementType نوع العنصر / Element type
     * @param level مستوى المؤشر / Pointer level (1, 2, 3...)
     */
    PointerType(BasicType elementType, int level);
    
    /**
     * الحصول على نوع العنصر
     * Get element type
     */
    BasicType getElementType() const { return elementType_; }
    
    /**
     * الحصول على مستوى المؤشر
     * Get pointer level
     */
    int getLevel() const { return level_; }
    
    /**
     * تحويل إلى نص
     * Convert to string
     */
    std::string toString() const;
    
private:
    BasicType elementType_;
    int level_;  // 1 for *, 2 for **, etc.
};

// ============================================================================
// معلومات المؤشر / Pointer Information
// ============================================================================

/**
 * معلومات عن مؤشر محدد
 * Information about a specific pointer
 */
struct PointerInfo {
    uint64_t address;          // العنوان / Address
    PointerType type;          // النوع / Type
    bool isAllocated;          // هل مخصص؟ / Is allocated?
    size_t size;               // الحجم / Size
    
    // المنشئ الافتراضي / Default constructor
    PointerInfo()
        : address(0)
        , type(BasicType::VOID)
        , isAllocated(false)
        , size(0) {}
    
    // المنشئ الكامل / Full constructor
    PointerInfo(uint64_t addr, PointerType t, bool alloc = false, size_t sz = 0)
        : address(addr), type(t), isAllocated(alloc), size(sz) {}
};

// ============================================================================
// عمليات المؤشرات / Pointer Operations
// ============================================================================

/**
 * نوع عملية المؤشر
 * Type of pointer operation
 */
enum class PointerOp {
    DEREFERENCE,    // * (إلغاء المرجعية / dereference)
    ADDRESS_OF,     // & (الحصول على العنوان / address-of)
    ADD,            // + (إضافة / add)
    SUBTRACT,       // - (طرح / subtract)
    COMPARE         // == != < > <= >= (مقارنة / compare)
};

// ============================================================================
// مدير المؤشرات / Pointer Manager
// ============================================================================

/**
 * مدير المؤشرات - يدير جميع المؤشرات في النظام
 * Pointer Manager - manages all pointers in the system
 */
class PointerManager {
public:
    /**
     * الحصول على المثيل الوحيد (Singleton)
     * Get singleton instance
     */
    static PointerManager& getInstance();
    
    /**
     * إنشاء مؤشر جديد
     * Create a new pointer
     * @param type نوع المؤشر / Pointer type
     * @param address العنوان / Address
     * @return معرف المؤشر / Pointer ID
     */
    uint64_t createPointer(const PointerType& type, uint64_t address);
    
    /**
     * إلغاء مرجعية مؤشر (قراءة القيمة)
     * Dereference pointer (read value)
     * @param pointerId معرف المؤشر / Pointer ID
     * @return القيمة / Value
     */
    uint64_t dereference(uint64_t pointerId);
    
    /**
     * كتابة قيمة إلى موقع المؤشر
     * Write value to pointer location
     * @param pointerId معرف المؤشر / Pointer ID
     * @param value القيمة / Value
     */
    void writeThrough(uint64_t pointerId, uint64_t value);
    
    /**
     * الحصول على عنوان متغير
     * Get address of variable
     * @param varName اسم المتغير / Variable name
     * @return العنوان / Address
     */
    uint64_t getAddressOf(const std::string& varName);
    
    /**
     * حساب المؤشر (pointer arithmetic)
     * Pointer arithmetic
     * @param pointerId معرف المؤشر / Pointer ID
     * @param offset الإزاحة / Offset
     * @return المؤشر الجديد / New pointer
     */
    uint64_t pointerAdd(uint64_t pointerId, int64_t offset);
    uint64_t pointerSub(uint64_t pointerId, int64_t offset);
    
    /**
     * مقارنة المؤشرات
     * Compare pointers
     */
    bool pointerEquals(uint64_t ptr1, uint64_t ptr2);
    bool pointerLessThan(uint64_t ptr1, uint64_t ptr2);
    
    /**
     * فحص صحة المؤشر
     * Validate pointer
     * @param pointerId معرف المؤشر / Pointer ID
     * @return هل المؤشر صحيح؟ / Is pointer valid?
     */
    bool isValidPointer(uint64_t pointerId);
    
    /**
     * فحص NULL
     * Check for NULL
     */
    bool isNull(uint64_t pointerId);
    
    /**
     * الحصول على معلومات المؤشر
     * Get pointer information
     */
    const PointerInfo* getPointerInfo(uint64_t pointerId);
    
    /**
     * مسح جميع المؤشرات
     * Clear all pointers
     */
    void clear();
    
private:
    PointerManager() = default;
    ~PointerManager() = default;
    
    // منع النسخ
    // Prevent copying
    PointerManager(const PointerManager&) = delete;
    PointerManager& operator=(const PointerManager&) = delete;
    
    // خريطة المؤشرات
    // Pointer map
    std::unordered_map<uint64_t, PointerInfo> pointers_;
    
    // خريطة المتغيرات إلى عناوينها
    // Variable to address map
    std::unordered_map<std::string, uint64_t> variables_;
    
    // عداد المعرفات
    // ID counter
    uint64_t nextId_ = 1;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_POINTER_TYPE_H
