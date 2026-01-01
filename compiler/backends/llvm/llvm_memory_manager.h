/*
 * ============================================================================
 * إدارة الذاكرة التلقائية و ARC - ملف الرأس
 * Automatic Memory Management & ARC - Header File
 * ============================================================================
 * 
 * هذا الملف يوفر نظام Automatic Reference Counting (ARC) لإدارة الذاكرة
 * This file provides Automatic Reference Counting (ARC) system for memory management
 * 
 * الميزات / Features:
 * - عد مرجعي تلقائي (automatic reference counting)
 * - retain/release تلقائية (automatic retain/release)
 * - كشف الدورات (cycle detection)
 * - تحرير ذاكرة تلقائي (automatic deallocation)
 * - GC hooks للتكامل المستقبلي (GC hooks for future integration)
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Days 6-7)
 * ============================================================================
 */

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

namespace Sad {
namespace LLVM {

// Forward declarations / التصريحات المسبقة
class LLVMTypeMapper;

// ============================================================================
// هيكل معلومات الكائن المُدار / Managed Object Info Structure
// ============================================================================

/**
 * معلومات الكائن المُدار بواسطة ARC
 * Information about ARC-managed object
 */
struct ManagedObjectInfo {
    llvm::Value* object;              ///< مؤشر الكائن / Object pointer
    llvm::Type* type;                 ///< نوع الكائن / Object type
    unsigned refCount;                ///< عدد المراجع / Reference count
    bool isStrong;                    ///< مرجع قوي؟ / Strong reference?
    bool needsDealloc;                ///< يحتاج تحرير؟ / Needs deallocation?
};

// ============================================================================
// فئة إدارة الذاكرة التلقائية / Automatic Memory Management Class
// ============================================================================

/**
 * LLVMMemoryManager - إدارة الذاكرة التلقائية مع ARC
 * LLVMMemoryManager - Automatic memory management with ARC
 */
class LLVMMemoryManager {
public:
    // ========================================================================
    // المنشئ / Constructor
    // ========================================================================
    
    /**
     * منشئ مدير الذاكرة
     * Memory manager constructor
     */
    LLVMMemoryManager(llvm::LLVMContext& context,
                      llvm::IRBuilder<>& builder,
                      LLVMTypeMapper* typeMapper);
    
    /**
     * مدمر مدير الذاكرة
     * Memory manager destructor
     */
    ~LLVMMemoryManager() = default;
    
    // ========================================================================
    // تخصيص الذاكرة / Memory Allocation
    // ========================================================================
    
    /**
     * تخصيص كائن مُدار
     * Allocate managed object
     */
    llvm::Value* allocateObject(llvm::Type* type, const std::string& name = "obj");
    
    /**
     * تخصيص مصفوفة مُدارة
     * Allocate managed array
     */
    llvm::Value* allocateArray(llvm::Type* elementType, llvm::Value* count);
    
    /**
     * تخصيص ذاكرة خام (غير مُدارة)
     * Allocate raw memory (unmanaged)
     */
    llvm::Value* allocateRaw(llvm::Value* size);
    
    // ========================================================================
    // إدارة المراجع / Reference Management
    // ========================================================================
    
    /**
     * زيادة عداد المراجع (retain)
     * Increment reference count (retain)
     */
    void retain(llvm::Value* object);
    
    /**
     * تقليل عداد المراجع (release)
     * Decrement reference count (release)
     */
    void release(llvm::Value* object);
    
    /**
     * نقل ملكية (move)
     * Transfer ownership (move)
     */
    llvm::Value* move(llvm::Value* object);
    
    /**
     * إنشاء مرجع ضعيف (weak reference)
     * Create weak reference
     */
    llvm::Value* createWeakRef(llvm::Value* object);
    
    /**
     * ترقية مرجع ضعيف لقوي (weak to strong)
     * Upgrade weak reference to strong
     */
    llvm::Value* upgradeWeakRef(llvm::Value* weakRef);
    
    // ========================================================================
    // عمليات ARC التلقائية / Automatic ARC Operations
    // ========================================================================
    
    /**
     * إدراج retain تلقائية عند التعيين
     * Auto-insert retain on assignment
     */
    void autoRetain(llvm::Value* object);
    
    /**
     * إدراج release تلقائية عند نهاية scope
     * Auto-insert release at scope end
     */
    void autoRelease(llvm::Value* object);
    
    /**
     * تسجيل كائن للتحرير التلقائي
     * Register object for automatic deallocation
     */
    void registerForAutoRelease(llvm::Value* object);
    
    /**
     * تحرير جميع الكائنات المُسجلة
     * Release all registered objects
     */
    void releaseAll();
    
    // ========================================================================
    // تحرير الذاكرة / Memory Deallocation
    // ========================================================================
    
    /**
     * تحرير كائن
     * Deallocate object
     */
    void deallocate(llvm::Value* object);
    
    /**
     * تحرير مصفوفة
     * Deallocate array
     */
    void deallocateArray(llvm::Value* array);
    
    /**
     * تحرير ذاكرة خام
     * Deallocate raw memory
     */
    void deallocateRaw(llvm::Value* ptr);
    
    // ========================================================================
    // كشف الدورات / Cycle Detection
    // ========================================================================
    
    /**
     * كشف دورات المراجع
     * Detect reference cycles
     */
    bool detectCycles(llvm::Value* object);
    
    /**
     * كسر دورات المراجع
     * Break reference cycles
     */
    void breakCycles(llvm::Value* object);
    
    /**
     * وضع علامة على الكائنات القابلة للوصول
     * Mark reachable objects
     */
    void markReachable(llvm::Value* root);
    
    /**
     * مسح الكائنات غير القابلة للوصول
     * Sweep unreachable objects
     */
    void sweepUnreachable();
    
    // ========================================================================
    // دوال مساعدة ARC / ARC Helper Functions
    // ========================================================================
    
    /**
     * الحصول على عدد المراجع
     * Get reference count
     */
    llvm::Value* getRefCount(llvm::Value* object);
    
    /**
     * تعيين عدد المراجع
     * Set reference count
     */
    void setRefCount(llvm::Value* object, llvm::Value* count);
    
    /**
     * التحقق من كون الكائن حياً
     * Check if object is alive
     */
    llvm::Value* isAlive(llvm::Value* object);
    
    // ========================================================================
    // GC Hooks للتكامل المستقبلي / GC Hooks for Future Integration
    // ========================================================================
    
    /**
     * تسجيل hook لـ GC
     * Register GC hook
     */
    void registerGCHook(llvm::Function* hook);
    
    /**
     * استدعاء GC hooks
     * Invoke GC hooks
     */
    void invokeGCHooks();
    
    /**
     * تمكين/تعطيل GC
     * Enable/disable GC
     */
    void setGCEnabled(bool enabled);
    
    // ========================================================================
    // تحسينات الأداء / Performance Optimizations
    // ========================================================================
    
    /**
     * دمج retain/release المتتالية
     * Merge consecutive retain/release
     */
    void optimizeRetainRelease();
    
    /**
     * إزالة retain/release غير الضرورية
     * Remove unnecessary retain/release
     */
    void eliminateRedundantOperations();
    
    /**
     * تجميع التحريرات (batching)
     * Batch deallocations
     */
    void batchDeallocations();
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;      ///< سياق LLVM / LLVM context
    llvm::IRBuilder<>& builder_;      ///< بناء IR / IR builder
    LLVMTypeMapper* typeMapper_;      ///< محول الأنواع / Type mapper
    
    std::vector<llvm::Value*> autoReleasePool_;        ///< مجموعة التحرير التلقائي / Auto-release pool
    std::unordered_set<llvm::Value*> managedObjects_;  ///< الكائنات المُدارة / Managed objects
    std::vector<llvm::Function*> gcHooks_;             ///< GC hooks
    bool gcEnabled_;                                    ///< GC مفعّل؟ / GC enabled?
    
    llvm::StructType* refCountedType_;  ///< نوع الكائن مع ref count / Ref-counted object type
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * إنشاء رأس الكائن المُدار
     * Create managed object header
     */
    llvm::Value* createObjectHeader(llvm::Type* type);
    
    /**
     * الحصول على مؤشر ref count
     * Get ref count pointer
     */
    llvm::Value* getRefCountPtr(llvm::Value* object);
    
    /**
     * الحصول على أو إنشاء نوع ref-counted
     * Get or create ref-counted type
     */
    llvm::StructType* getOrCreateRefCountedType();
    
    /**
     * استدعاء دالة runtime للذاكرة
     * Call memory runtime function
     */
    llvm::Value* callMemoryRuntime(const std::string& functionName,
                                    const std::vector<llvm::Value*>& args);
};

// ============================================================================
// فئة RAII Guard لـ ARC / RAII Guard for ARC
// ============================================================================

/**
 * ARCGuard - RAII wrapper لضمان release تلقائية
 * ARCGuard - RAII wrapper to ensure automatic release
 */
class ARCGuard {
public:
    /**
     * منشئ ARCGuard
     * ARCGuard constructor
     */
    ARCGuard(LLVMMemoryManager* manager, llvm::Value* object)
        : manager_(manager)
        , object_(object)
    {
        if (manager_ && object_) {
            manager_->retain(object_);
        }
    }
    
    /**
     * مدمر ARCGuard - تحرير تلقائي
     * ARCGuard destructor - automatic release
     */
    ~ARCGuard() {
        if (manager_ && object_) {
            manager_->release(object_);
        }
    }
    
    // منع النسخ / Prevent copying
    ARCGuard(const ARCGuard&) = delete;
    ARCGuard& operator=(const ARCGuard&) = delete;
    
    // السماح بالنقل / Allow moving
    ARCGuard(ARCGuard&& other) noexcept
        : manager_(other.manager_)
        , object_(other.object_)
    {
        other.manager_ = nullptr;
        other.object_ = nullptr;
    }
    
private:
    LLVMMemoryManager* manager_;
    llvm::Value* object_;
};

} // namespace LLVM
} // namespace Sad
