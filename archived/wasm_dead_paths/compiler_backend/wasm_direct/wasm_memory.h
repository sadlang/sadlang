/**
 * @file wasm_memory.h
 * @brief (AR) إدارة ذاكرة WASM — مُخصّص خطي + جمع قمامة بسيط
 * @brief (EN) WASM memory management — linear allocator + simple GC
 *
 * يوفر نظام إدارة ذاكرة مُحسّن لـ WebAssembly:
 * - مُخصّص خطي (bump allocator) للتخصيص السريع
 * - مُخصّص الكتل الحرة (free-list) لإعادة الاستخدام
 * - جمع قمامة بسيط بالعلامة والمسح (mark-and-sweep)
 * - مكدس مُدار للمتغيرات المحلية
 *
 * الذاكرة الخطية لـ WASM مقسمة كالتالي:
 * ┌─────────────┬──────────────┬───────────────┬──────────────┐
 * │ Data (ثوابت) │ Heap (كومة)  │ ← Free →     │ Stack (مكدس) │
 * │ 0..1KB      │ 1KB..        │               │ ..top        │
 * └─────────────┴──────────────┴───────────────┴──────────────┘
 */

#pragma once
#include "wasm_emitter.h"
#include <string>
#include <vector>
#include <cstdint>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

// ════════════════════════════════════════════════════════════════════════════════
//  ثوابت إدارة الذاكرة
// ════════════════════════════════════════════════════════════════════════════════

constexpr uint32_t WASM_PAGE_SIZE = 65536;      // 64KB لكل صفحة
constexpr uint32_t DATA_SECTION_START = 0;       // بداية قسم البيانات
constexpr uint32_t DATA_SECTION_SIZE = 1024;     // 1KB للبيانات الثابتة
constexpr uint32_t HEAP_START = 1024;            // بداية الكومة
constexpr uint32_t BLOCK_HEADER_SIZE = 8;        // حجم رأس الكتلة (4 حجم + 4 أعلام)
constexpr uint32_t MIN_BLOCK_SIZE = 16;          // أصغر كتلة ممكنة
constexpr uint32_t ALIGNMENT = 8;               // محاذاة 8 بايت
constexpr uint32_t GC_MARK_BIT = 0x80000000;    // بت العلامة لـ GC

// أعلام الكتلة
constexpr uint32_t BLOCK_FREE = 0x00;           // كتلة حرة
constexpr uint32_t BLOCK_USED = 0x01;           // كتلة مستخدمة
constexpr uint32_t BLOCK_MARKED = 0x02;         // كتلة مُعلّمة (GC)
constexpr uint32_t BLOCK_STRING = 0x04;         // كتلة نصية
constexpr uint32_t BLOCK_ARRAY = 0x08;          // كتلة مصفوفة
constexpr uint32_t BLOCK_OBJECT = 0x10;         // كتلة كائن

/**
 * خيارات إدارة الذاكرة
 */
struct WasmMemoryOptions {
    uint32_t initialPages = 16;         // 1MB ابتدائي
    uint32_t maxPages = 256;            // 16MB أقصى
    uint32_t stackSize = 65536;         // 64KB مكدس
    uint32_t gcThreshold = 1024 * 100;  // تشغيل GC كل 100KB تخصيص
    bool enableGC = true;               // تفعيل جمع القمامة
    bool enablePooling = true;          // تفعيل تجميع الكتل الصغيرة
    bool growableMemory = true;         // السماح بتنمية الذاكرة
};

/**
 * معلومات كتلة ذاكرة
 */
struct MemBlock {
    uint32_t address;       // عنوان البيانات (بعد الرأس)
    uint32_t size;          // حجم البيانات
    uint32_t flags;         // أعلام الكتلة
};

// ════════════════════════════════════════════════════════════════════════════════
//  مُنشئ كود إدارة الذاكرة
// ════════════════════════════════════════════════════════════════════════════════

/**
 * مولّد دوال إدارة الذاكرة لـ WASM
 * يُنشئ دوال malloc/free/gc كدوال WASM أصلية
 */
class WasmMemoryManager {
public:
    explicit WasmMemoryManager(WasmEmitter& emitter, 
                                const WasmMemoryOptions& options = {});

    /**
     * تسجيل جميع دوال إدارة الذاكرة في المُصدر
     * يُسجّل: sad_malloc, sad_free, sad_realloc, sad_gc_collect,
     *         sad_str_alloc, sad_array_alloc, sad_obj_alloc
     */
    void registerMemoryFunctions();

    /**
     * الحصول على فهرس دالة malloc
     */
    uint32_t getMallocIndex() const { return mallocIdx_; }

    /**
     * الحصول على فهرس دالة free
     */
    uint32_t getFreeIndex() const { return freeIdx_; }

    /**
     * الحصول على فهرس دالة realloc
     */
    uint32_t getReallocIndex() const { return reallocIdx_; }

    /**
     * الحصول على فهرس دالة GC
     */
    uint32_t getGCCollectIndex() const { return gcCollectIdx_; }

    /**
     * الحصول على فهرس دالة تخصيص النصوص
     */
    uint32_t getStrAllocIndex() const { return strAllocIdx_; }

    /**
     * الحصول على فهرس دالة تخصيص المصفوفات
     */
    uint32_t getArrayAllocIndex() const { return arrayAllocIdx_; }

    /**
     * فهارس المتغيرات العامة
     */
    uint32_t getHeapPtrGlobal() const { return heapPtrGlobal_; }
    uint32_t getStackPtrGlobal() const { return stackPtrGlobal_; }
    uint32_t getFreePtrGlobal() const { return freePtrGlobal_; }
    uint32_t getAllocCountGlobal() const { return allocCountGlobal_; }

private:
    WasmEmitter& emitter_;
    WasmMemoryOptions options_;

    // فهارس الدوال
    uint32_t mallocIdx_ = 0;
    uint32_t freeIdx_ = 0;
    uint32_t reallocIdx_ = 0;
    uint32_t gcCollectIdx_ = 0;
    uint32_t strAllocIdx_ = 0;
    uint32_t arrayAllocIdx_ = 0;
    uint32_t objAllocIdx_ = 0;

    // فهارس المتغيرات العامة
    uint32_t heapPtrGlobal_ = 0;
    uint32_t stackPtrGlobal_ = 0;
    uint32_t freePtrGlobal_ = 0;
    uint32_t allocCountGlobal_ = 0;
    uint32_t totalAllocGlobal_ = 0;

    // بناء الدوال
    void buildMalloc();
    void buildFree();
    void buildRealloc();
    void buildGCCollect();
    void buildStrAlloc();
    void buildArrayAlloc();
    void buildObjAlloc();
    void buildMemcpy();
    void buildMemset();
    void setupGlobals();

    // فهرس دالة memcpy/memset
    uint32_t memcpyIdx_ = 0;
    uint32_t memsetIdx_ = 0;
};

// ════════════════════════════════════════════════════════════════════════════════
//  مدير المكدس
// ════════════════════════════════════════════════════════════════════════════════

/**
 * مدير مكدس الإطارات (Stack Frame Manager)
 * يُدير إطارات الاستدعاء على مكدس WASM
 */
class WasmStackManager {
public:
    /**
     * إنشاء كود دفع إطار جديد
     * push_frame(size) → old_sp
     */
    static void emitPushFrame(WasmCodeBuilder& code, uint32_t stackPtrGlobal, 
                               uint32_t frameSize);

    /**
     * إنشاء كود سحب إطار
     * pop_frame(old_sp)
     */
    static void emitPopFrame(WasmCodeBuilder& code, uint32_t stackPtrGlobal);

    /**
     * تخصيص على المكدس (stack alloca)
     * @return إزاحة من قاعدة الإطار
     */
    static void emitStackAlloc(WasmCodeBuilder& code, uint32_t stackPtrGlobal,
                                uint32_t size);
};

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
