/**
 * @file wasm_memory.cpp
 * @brief (AR) تطبيق إدارة ذاكرة WASM — مُخصّص + جمع قمامة
 * @brief (EN) WASM memory management implementation — allocator + GC
 *
 * يولّد دوال إدارة الذاكرة كتعليمات WASM أصلية:
 * - sad_malloc: تخصيص كتلة (bump + free-list)
 * - sad_free: تحرير كتلة (إضافة للقائمة الحرة)
 * - sad_realloc: إعادة تخصيص مع نسخ
 * - sad_gc_collect: جمع قمامة (mark-sweep بسيط)
 * - sad_str_alloc: تخصيص نص مع رأس
 * - sad_array_alloc: تخصيص مصفوفة مع رأس
 */

#include "wasm_memory.h"

namespace Sad {
namespace Compiler {
namespace WasmDirect {

// ════════════════════════════════════════════════════════════════════════════════
//  WasmMemoryManager
// ════════════════════════════════════════════════════════════════════════════════

WasmMemoryManager::WasmMemoryManager(WasmEmitter& emitter, 
                                       const WasmMemoryOptions& options)
    : emitter_(emitter), options_(options) {}

void WasmMemoryManager::registerMemoryFunctions() {
    // إعداد الذاكرة
    emitter_.setMemory(options_.initialPages, options_.maxPages);
    
    // تسجيل المتغيرات العامة
    setupGlobals();

    // بناء الدوال
    buildMemset();
    buildMemcpy();
    buildMalloc();
    buildFree();
    buildRealloc();
    buildStrAlloc();
    buildArrayAlloc();
    buildObjAlloc();
    
    if (options_.enableGC) {
        buildGCCollect();
    }
}

void WasmMemoryManager::setupGlobals() {
    uint32_t initialMem = options_.initialPages * WASM_PAGE_SIZE;
    
    // مؤشر الكومة (يبدأ بعد قسم البيانات)
    heapPtrGlobal_ = emitter_.addGlobal(ValType::I32, true, HEAP_START);
    
    // مؤشر المكدس (يبدأ من أعلى الذاكرة)
    stackPtrGlobal_ = emitter_.addGlobal(ValType::I32, true, 
                                          static_cast<int32_t>(initialMem));
    
    // رأس القائمة الحرة (0 = فارغة)
    freePtrGlobal_ = emitter_.addGlobal(ValType::I32, true, 0);
    
    // عداد التخصيصات (لتشغيل GC)
    allocCountGlobal_ = emitter_.addGlobal(ValType::I32, true, 0);
    
    // إجمالي الذاكرة المخصصة
    totalAllocGlobal_ = emitter_.addGlobal(ValType::I32, true, 0);
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_malloc(size: i32) → ptr: i32
//  خوارزمية: أولاً يبحث في القائمة الحرة، ثم bump allocation
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildMalloc() {
    WasmFuncType type;
    type.params = {ValType::I32};   // size
    type.results = {ValType::I32};  // ptr
    mallocIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_malloc", mallocIdx_);

    WasmCodeBuilder code;
    // المعاملات: 0=size
    // المحلي: 1=aligned_size, 2=current_free, 3=prev_free, 4=block_size, 5=ptr
    code.addLocal(ValType::I32, 5); // locals 1-5

    // محاذاة الحجم على ALIGNMENT + BLOCK_HEADER_SIZE
    // aligned_size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1)
    code.localGet(0);                               // size
    code.i32Const(ALIGNMENT + BLOCK_HEADER_SIZE - 1);
    code.i32Add();
    code.i32Const(~(ALIGNMENT - 1));
    code.i32And();
    code.localSet(1);                               // aligned_size

    // ═══ البحث في القائمة الحرة ═══
    code.globalGet(freePtrGlobal_);                 // current_free
    code.localSet(2);
    code.i32Const(0);
    code.localSet(3);                               // prev_free = 0

    code.block();                                    // $not_found
    code.loop();                                     // $search_loop
    
    // إذا current_free == 0 → لا شيء في القائمة
    code.localGet(2);
    code.i32Eqz();
    code.brIf(1);                                    // br $not_found

    // قراءة حجم الكتلة الحرة
    code.localGet(2);
    code.i32Load();                                  // block_size = mem[current_free]
    code.localSet(4);

    // إذا block_size >= aligned_size → وجدنا كتلة
    code.localGet(4);
    code.localGet(1);
    code.i32GeS();
    code.ifBlock(static_cast<uint8_t>(ValType::I32));
        // إزالة الكتلة من القائمة الحرة
        // prev_free.next = current_free.next
        code.localGet(3);
        code.i32Eqz();
        code.ifBlock();
            // الكتلة هي رأس القائمة
            code.localGet(2);
            code.i32Const(4);
            code.i32Add();
            code.i32Load();                          // next ptr
            code.globalSet(freePtrGlobal_);
        code.elseBlock();
            // الكتلة في المنتصف
            code.localGet(3);
            code.i32Const(4);
            code.i32Add();
            code.localGet(2);
            code.i32Const(4);
            code.i32Add();
            code.i32Load();
            code.i32Store();
        code.end();
        
        // تعيين أعلام الكتلة = USED
        code.localGet(2);
        code.i32Const(4);
        code.i32Add();
        code.i32Const(BLOCK_USED);
        code.i32Store();
        
        // إرجاع العنوان بعد الرأس
        code.localGet(2);
        code.i32Const(BLOCK_HEADER_SIZE);
        code.i32Add();
        code.ret();

        code.i32Const(0); // placeholder for if result
    code.elseBlock();
        // الانتقال للكتلة التالية
        code.localGet(2);
        code.localSet(3);                            // prev = current
        code.localGet(2);
        code.i32Const(4);
        code.i32Add();
        code.i32Load();                              // current = current.next
        code.localSet(2);
        code.br(1);                                  // continue search

        code.i32Const(0); // placeholder
    code.end();

    code.drop();
    code.end();                                      // end loop
    code.end();                                      // end block

    // ═══ Bump allocation (لم نجد كتلة حرة مناسبة) ═══
    // ptr = heap_ptr
    code.globalGet(heapPtrGlobal_);
    code.localSet(5);                                // ptr = heap_ptr

    // كتابة حجم الكتلة
    code.localGet(5);
    code.localGet(1);
    code.i32Store();                                 // mem[ptr] = aligned_size

    // كتابة أعلام الكتلة
    code.localGet(5);
    code.i32Const(4);
    code.i32Add();
    code.i32Const(BLOCK_USED);
    code.i32Store();                                 // mem[ptr+4] = USED

    // heap_ptr += aligned_size
    code.localGet(5);
    code.localGet(1);
    code.i32Add();
    code.globalSet(heapPtrGlobal_);

    // تحديث عداد التخصيصات
    code.globalGet(allocCountGlobal_);
    code.i32Const(1);
    code.i32Add();
    code.globalSet(allocCountGlobal_);

    // تحديث إجمالي الذاكرة
    code.globalGet(totalAllocGlobal_);
    code.localGet(1);
    code.i32Add();
    code.globalSet(totalAllocGlobal_);

    // التحقق من نمو الذاكرة
    if (options_.growableMemory) {
        code.globalGet(heapPtrGlobal_);
        code.globalGet(stackPtrGlobal_);
        code.localGet(1);
        code.i32Sub();
        code.i32GeS();
        code.ifBlock();
            // نحتاج صفحات إضافية
            code.i32Const(1);                        // صفحة واحدة
            code.memoryGrow();
            code.i32Const(-1);
            code.i32Eq();
            code.ifBlock();
                // فشل النمو — خطأ
                code.i32Const(0);
                code.ret();
            code.end();
            // تحديث مؤشر المكدس
            code.memorySize();
            code.i32Const(WASM_PAGE_SIZE);
            code.i32Mul();
            code.globalSet(stackPtrGlobal_);
        code.end();
    }

    // إرجاع ptr + HEADER_SIZE
    code.localGet(5);
    code.i32Const(BLOCK_HEADER_SIZE);
    code.i32Add();

    emitter_.setFunctionBody(mallocIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_free(ptr: i32) → void
//  يُضيف الكتلة لرأس القائمة الحرة
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildFree() {
    WasmFuncType type;
    type.params = {ValType::I32};  // ptr
    freeIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_free", freeIdx_);

    WasmCodeBuilder code;
    // المعاملات: 0=ptr
    // المحلي: 1=block_ptr
    code.addLocal(ValType::I32, 1);

    // التحقق من صحة المؤشر
    code.localGet(0);
    code.i32Eqz();
    code.ifBlock();
        code.ret();
    code.end();

    // block_ptr = ptr - HEADER_SIZE
    code.localGet(0);
    code.i32Const(BLOCK_HEADER_SIZE);
    code.i32Sub();
    code.localSet(1);

    // تعيين كعلام = FREE
    code.localGet(1);
    code.i32Const(4);
    code.i32Add();
    code.i32Const(BLOCK_FREE);
    code.i32Store();

    // إضافة لرأس القائمة الحرة
    // block.next = free_list_head
    code.localGet(1);
    code.i32Const(4);
    code.i32Add();
    code.globalGet(freePtrGlobal_);
    code.i32Store();

    // free_list_head = block_ptr
    code.localGet(1);
    code.globalSet(freePtrGlobal_);

    // تحديث عداد التخصيصات
    code.globalGet(allocCountGlobal_);
    code.i32Const(1);
    code.i32Sub();
    code.globalSet(allocCountGlobal_);

    emitter_.setFunctionBody(freeIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_realloc(ptr: i32, new_size: i32) → new_ptr: i32
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildRealloc() {
    WasmFuncType type;
    type.params = {ValType::I32, ValType::I32};  // ptr, new_size
    type.results = {ValType::I32};               // new_ptr
    reallocIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_realloc", reallocIdx_);

    WasmCodeBuilder code;
    // المعاملات: 0=ptr, 1=new_size
    // المحلي: 2=new_ptr, 3=old_size, 4=copy_size
    code.addLocal(ValType::I32, 3);

    // إذا ptr == 0 → malloc
    code.localGet(0);
    code.i32Eqz();
    code.ifBlock(static_cast<uint8_t>(ValType::I32));
        code.localGet(1);
        code.call(mallocIdx_);
        code.ret();
        code.i32Const(0);
    code.elseBlock();
        code.i32Const(0);
    code.end();
    code.drop();

    // قراءة الحجم القديم
    code.localGet(0);
    code.i32Const(BLOCK_HEADER_SIZE);
    code.i32Sub();
    code.i32Load();
    code.i32Const(BLOCK_HEADER_SIZE);
    code.i32Sub();
    code.localSet(3);                    // old_size

    // تخصيص كتلة جديدة
    code.localGet(1);
    code.call(mallocIdx_);
    code.localSet(2);                    // new_ptr

    // النسخ: copy_size = min(old_size, new_size)
    code.localGet(3);
    code.localGet(1);
    code.localGet(3);
    code.localGet(1);
    code.i32LtS();
    code.select();
    code.localSet(4);                    // copy_size

    // نسخ البيانات عبر memcpy
    code.localGet(2);                    // dest
    code.localGet(0);                    // src
    code.localGet(4);                    // size
    code.call(memcpyIdx_);

    // تحرير القديم
    code.localGet(0);
    code.call(freeIdx_);

    // إرجاع المؤشر الجديد
    code.localGet(2);

    emitter_.setFunctionBody(reallocIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_gc_collect() → freed_count: i32
//  جمع قمامة بسيط — يمسح القائمة الحرة ويدمج الكتل المتجاورة
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildGCCollect() {
    WasmFuncType type;
    type.results = {ValType::I32};  // freed_count
    gcCollectIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_gc_collect", gcCollectIdx_);

    WasmCodeBuilder code;
    // المحلي: 0=current, 1=freed_count, 2=block_size, 3=next_addr
    code.addLocal(ValType::I32, 4);

    code.i32Const(0);
    code.localSet(1);                    // freed_count = 0

    // بدء المسح من بداية الكومة
    code.i32Const(HEAP_START);
    code.localSet(0);                    // current = HEAP_START

    code.block();                        // $done
    code.loop();                         // $scan

    // إذا وصلنا لنهاية الكومة → انتهى
    code.localGet(0);
    code.globalGet(heapPtrGlobal_);
    code.i32GeS();
    code.brIf(1);                        // br $done

    // قراءة حجم الكتلة
    code.localGet(0);
    code.i32Load();
    code.localSet(2);                    // block_size

    // التحقق من الأعلام
    code.localGet(0);
    code.i32Const(4);
    code.i32Add();
    code.i32Load();
    
    code.i32Const(BLOCK_FREE);
    code.i32Eq();
    code.ifBlock();
        // كتلة حرة — محاولة الدمج مع الكتلة التالية
        code.localGet(0);
        code.localGet(2);
        code.i32Add();
        code.localSet(3);               // next_addr
        
        code.localGet(3);
        code.globalGet(heapPtrGlobal_);
        code.i32LtS();
        code.ifBlock();
            // التحقق هل الكتلة التالية حرة أيضاً
            code.localGet(3);
            code.i32Const(4);
            code.i32Add();
            code.i32Load();
            code.i32Const(BLOCK_FREE);
            code.i32Eq();
            code.ifBlock();
                // دمج الكتلتين
                code.localGet(0);
                code.localGet(2);
                code.localGet(3);
                code.i32Load();
                code.i32Add();
                code.i32Store();         // current.size += next.size
                
                code.localGet(1);
                code.i32Const(1);
                code.i32Add();
                code.localSet(1);        // freed_count++
                
                code.br(2);             // أعد المسح من نفس الموقع
            code.end();
        code.end();
    code.end();

    // الانتقال للكتلة التالية
    code.localGet(0);
    code.localGet(2);
    code.i32Add();
    code.localSet(0);

    code.br(0);                          // continue scan
    code.end();                          // end loop
    code.end();                          // end block

    code.localGet(1);                    // return freed_count

    emitter_.setFunctionBody(gcCollectIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_str_alloc(len: i32) → ptr: i32
//  تخصيص نص: [4 bytes length][data...][null]
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildStrAlloc() {
    WasmFuncType type;
    type.params = {ValType::I32};   // len
    type.results = {ValType::I32};  // ptr
    strAllocIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_str_alloc", strAllocIdx_);

    WasmCodeBuilder code;
    // المعاملات: 0=len
    // المحلي: 1=ptr
    code.addLocal(ValType::I32, 1);

    // تخصيص: 4 (طول) + len + 1 (null terminator)
    code.localGet(0);
    code.i32Const(5);               // +4 header + 1 null
    code.i32Add();
    code.call(mallocIdx_);
    code.localSet(1);

    // كتابة الطول
    code.localGet(1);
    code.localGet(0);
    code.i32Store();

    // تعيين علامة النص في رأس الكتلة
    code.localGet(1);
    code.i32Const(BLOCK_HEADER_SIZE + 4);
    code.i32Sub();
    code.i32Const(BLOCK_USED | BLOCK_STRING);
    code.i32Store();

    // إرجاع ptr + 4 (بعد الطول)
    code.localGet(1);
    code.i32Const(4);
    code.i32Add();

    emitter_.setFunctionBody(strAllocIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_array_alloc(count: i32, elem_size: i32) → ptr: i32
//  تخصيص مصفوفة: [4 bytes count][4 bytes elem_size][data...]
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildArrayAlloc() {
    WasmFuncType type;
    type.params = {ValType::I32, ValType::I32};  // count, elem_size
    type.results = {ValType::I32};               // ptr
    arrayAllocIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_array_alloc", arrayAllocIdx_);

    WasmCodeBuilder code;
    // المعاملات: 0=count, 1=elem_size
    // المحلي: 2=total_size, 3=ptr
    code.addLocal(ValType::I32, 2);

    // total_size = 8 (header) + count * elem_size
    code.localGet(0);
    code.localGet(1);
    code.i32Mul();
    code.i32Const(8);          // header: count + elem_size
    code.i32Add();
    code.localSet(2);

    // تخصيص
    code.localGet(2);
    code.call(mallocIdx_);
    code.localSet(3);

    // كتابة عدد العناصر
    code.localGet(3);
    code.localGet(0);
    code.i32Store();

    // كتابة حجم العنصر
    code.localGet(3);
    code.i32Const(4);
    code.i32Add();
    code.localGet(1);
    code.i32Store();

    // تعيين علامة المصفوفة
    code.localGet(3);
    code.i32Const(BLOCK_HEADER_SIZE + 4);
    code.i32Sub();
    code.i32Const(BLOCK_USED | BLOCK_ARRAY);
    code.i32Store();

    // إرجاع ptr + 8 (بعد الرأس)
    code.localGet(3);
    code.i32Const(8);
    code.i32Add();

    emitter_.setFunctionBody(arrayAllocIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  sad_obj_alloc(field_count: i32) → ptr: i32
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildObjAlloc() {
    WasmFuncType type;
    type.params = {ValType::I32};   // field_count
    type.results = {ValType::I32};  // ptr
    objAllocIdx_ = emitter_.addFunction(type);
    emitter_.exportFunction("sad_obj_alloc", objAllocIdx_);

    WasmCodeBuilder code;
    // المعاملات: 0=field_count
    // المحلي: 1=ptr
    code.addLocal(ValType::I32, 1);

    // تخصيص: 4 (عدد الحقول) + field_count * 8 (كل حقل: 4 اسم + 4 قيمة)
    code.localGet(0);
    code.i32Const(8);
    code.i32Mul();
    code.i32Const(4);
    code.i32Add();
    code.call(mallocIdx_);
    code.localSet(1);

    // كتابة عدد الحقول
    code.localGet(1);
    code.localGet(0);
    code.i32Store();

    // تعيين علامة الكائن
    code.localGet(1);
    code.i32Const(BLOCK_HEADER_SIZE + 4);
    code.i32Sub();
    code.i32Const(BLOCK_USED | BLOCK_OBJECT);
    code.i32Store();

    // إرجاع ptr + 4
    code.localGet(1);
    code.i32Const(4);
    code.i32Add();

    emitter_.setFunctionBody(objAllocIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  memcpy(dest: i32, src: i32, size: i32)
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildMemcpy() {
    WasmFuncType type;
    type.params = {ValType::I32, ValType::I32, ValType::I32}; // dest, src, size
    memcpyIdx_ = emitter_.addFunction(type);

    WasmCodeBuilder code;
    // المعاملات: 0=dest, 1=src, 2=size
    // المحلي: 3=i
    code.addLocal(ValType::I32, 1);

    code.i32Const(0);
    code.localSet(3);                    // i = 0

    code.block();
    code.loop();
    
    code.localGet(3);
    code.localGet(2);
    code.i32GeS();
    code.brIf(1);                        // if i >= size → break

    // dest[i] = src[i]
    code.localGet(0);
    code.localGet(3);
    code.i32Add();
    code.localGet(1);
    code.localGet(3);
    code.i32Add();
    code.i32Load8U();
    code.i32Store8();

    // i++
    code.localGet(3);
    code.i32Const(1);
    code.i32Add();
    code.localSet(3);

    code.br(0);
    code.end();
    code.end();

    emitter_.setFunctionBody(memcpyIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  memset(dest: i32, val: i32, size: i32)
// ════════════════════════════════════════════════════════════════════════════════

void WasmMemoryManager::buildMemset() {
    WasmFuncType type;
    type.params = {ValType::I32, ValType::I32, ValType::I32}; // dest, val, size
    memsetIdx_ = emitter_.addFunction(type);

    WasmCodeBuilder code;
    // المعاملات: 0=dest, 1=val, 2=size
    // المحلي: 3=i
    code.addLocal(ValType::I32, 1);

    code.i32Const(0);
    code.localSet(3);

    code.block();
    code.loop();
    
    code.localGet(3);
    code.localGet(2);
    code.i32GeS();
    code.brIf(1);

    code.localGet(0);
    code.localGet(3);
    code.i32Add();
    code.localGet(1);
    code.i32Store8();

    code.localGet(3);
    code.i32Const(1);
    code.i32Add();
    code.localSet(3);

    code.br(0);
    code.end();
    code.end();

    emitter_.setFunctionBody(memsetIdx_, code.buildBody());
}

// ════════════════════════════════════════════════════════════════════════════════
//  WasmStackManager
// ════════════════════════════════════════════════════════════════════════════════

void WasmStackManager::emitPushFrame(WasmCodeBuilder& code, uint32_t stackPtrGlobal,
                                       uint32_t frameSize) {
    // old_sp = stack_ptr
    // stack_ptr -= frameSize (المكدس ينمو للأسفل)
    code.globalGet(stackPtrGlobal);
    code.globalGet(stackPtrGlobal);
    code.i32Const(static_cast<int32_t>(frameSize));
    code.i32Sub();
    code.globalSet(stackPtrGlobal);
    // old_sp على المكدس
}

void WasmStackManager::emitPopFrame(WasmCodeBuilder& code, uint32_t stackPtrGlobal) {
    // stack_ptr = old_sp (القيمة على المكدس)
    code.globalSet(stackPtrGlobal);
}

void WasmStackManager::emitStackAlloc(WasmCodeBuilder& code, uint32_t stackPtrGlobal,
                                        uint32_t size) {
    // محاذاة الحجم
    uint32_t aligned = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    code.globalGet(stackPtrGlobal);
    code.i32Const(static_cast<int32_t>(aligned));
    code.i32Sub();
    code.globalSet(stackPtrGlobal);
    code.globalGet(stackPtrGlobal);
    // العنوان المُخصص على المكدس
}

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
