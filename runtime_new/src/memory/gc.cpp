/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تنفيذ جامع القمامة / Garbage Collector Implementation
 * ================================
 */

#include "memory/gc.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>

namespace Sad {
namespace Memory {

// ============================================================================
// Helper Functions / دوال مساعدة
// ============================================================================

static inline uint64_t get_microseconds() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()
    ).count();
}

// ============================================================================
// GarbageCollector Implementation / تنفيذ جامع القمامة
// ============================================================================

GarbageCollector::GarbageCollector()
    : initialized_(false)
    , enabled_(true)
    , collecting_(false)
    , objects_list_(nullptr)
    , allocated_bytes_(0)
    , threshold_(0)
{
    std::memset(&stats_, 0, sizeof(stats_));
}

GarbageCollector::~GarbageCollector() {
    shutdown();
}

GarbageCollector& GarbageCollector::instance() {
    static GarbageCollector gc;
    return gc;
}

bool GarbageCollector::initialize(const GCConfig* config) {
    if (initialized_) {
        return true;
    }
    
    config_ = config ? *config : GCConfig::default_config();
    threshold_ = config_.gc_threshold;
    
    // تهيئة المخصص / Initialize allocator
    if (!MemoryAllocator::instance().initialize()) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

void GarbageCollector::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(gc_mutex_);
    
    // تحرير جميع الكائنات / Free all objects
    GCObject* current = objects_list_;
    while (current) {
        GCObject* next = current->next;
        
        // استدعاء المدمر / Call finalizer
        if (current->finalize_fn) {
            current->finalize_fn();
        }
        
        MemoryAllocator::instance().deallocate(current);
        current = next;
    }
    
    objects_list_ = nullptr;
    roots_.clear();
    gray_list_.clear();
    
    initialized_ = false;
}

GCObject* GarbageCollector::allocate(GCObjectType type, size_t size) {
    if (!initialized_) {
        initialize();
    }
    
    // التحقق من الحاجة للجمع / Check if collection needed
    if (enabled_ && should_collect()) {
        collect();
    }
    
    // حساب الحجم الكلي / Calculate total size
    size_t total_size = sizeof(GCObject) + size - 1;
    
    // تخصيص الذاكرة / Allocate memory
    GCObject* obj = static_cast<GCObject*>(
        MemoryAllocator::instance().allocate(total_size)
    );
    
    if (!obj) {
        // محاولة الجمع وإعادة المحاولة / Try collecting and retry
        if (enabled_) {
            collect();
            obj = static_cast<GCObject*>(
                MemoryAllocator::instance().allocate(total_size)
            );
        }
        
        if (!obj) {
            return nullptr;
        }
    }
    
    // تهيئة الكائن / Initialize object
    obj->type = type;
    obj->state = GCObjectState::White;
    obj->size = size;
    obj->ref_count = 0;
    obj->pinned = false;
    obj->next = nullptr;
    obj->trace_fn = nullptr;
    obj->finalize_fn = nullptr;
    
    // إضافة للقائمة / Add to list
    {
        std::lock_guard<std::mutex> lock(gc_mutex_);
        obj->next = objects_list_;
        objects_list_ = obj;
        
        allocated_bytes_ += total_size;
        stats_.total_objects++;
        stats_.live_objects++;
    }
    
    return obj;
}

void GarbageCollector::register_root(void* location, RootType type, const char* name) {
    std::lock_guard<std::mutex> lock(gc_mutex_);
    
    GCRoot root;
    root.type = type;
    root.location = location;
    root.name = name;
    
    roots_.push_back(root);
}

void GarbageCollector::unregister_root(void* location) {
    std::lock_guard<std::mutex> lock(gc_mutex_);
    
    roots_.erase(
        std::remove_if(roots_.begin(), roots_.end(),
            [location](const GCRoot& root) {
                return root.location == location;
            }
        ),
        roots_.end()
    );
}

size_t GarbageCollector::collect() {
    if (!enabled_ || collecting_.exchange(true)) {
        return 0; // Already collecting
    }
    
    uint64_t start_time = get_microseconds();
    
    std::lock_guard<std::mutex> lock(gc_mutex_);
    
    // 1. إعادة تعيين الحالات / Reset states
    reset_states();
    
    // 2. مرحلة التعليم / Mark phase
    mark();
    
    // 3. مرحلة المسح / Sweep phase
    size_t freed = sweep();
    
    // 4. مرحلة الضغط (اختياري) / Compact phase (optional)
    if (config_.enable_compaction) {
        compact();
    }
    
    // تحديث الإحصائيات / Update statistics
    uint64_t collection_time = get_microseconds() - start_time;
    
    stats_.total_collections++;
    stats_.freed_memory += freed;
    stats_.last_collection_time = collection_time;
    stats_.total_collection_time += collection_time;
    
    if (collection_time > stats_.max_pause_time) {
        stats_.max_pause_time = collection_time;
    }
    
    // تحديث العتبة / Update threshold
    threshold_ = allocated_bytes_ * config_.growth_factor;
    if (threshold_ > config_.heap_max_size) {
        threshold_ = config_.heap_max_size;
    }
    
    collecting_ = false;
    return freed;
}

bool GarbageCollector::should_collect() const {
    return allocated_bytes_ >= threshold_;
}

void GarbageCollector::retain(GCObject* obj) {
    if (!obj) return;
    
    obj->ref_count++;
}

void GarbageCollector::release(GCObject* obj) {
    if (!obj) return;
    
    if (obj->ref_count > 0) {
        obj->ref_count--;
    }
}

void GarbageCollector::pin(GCObject* obj) {
    if (obj) {
        obj->pinned = true;
    }
}

void GarbageCollector::unpin(GCObject* obj) {
    if (obj) {
        obj->pinned = false;
    }
}

void GarbageCollector::write_barrier(GCObject* parent, GCObject* child) {
    if (!parent || !child) return;
    
    // إذا كان الأب أسود والابن أبيض، أضف الابن للقائمة الرمادية
    if (parent->state == GCObjectState::Black && 
        child->state == GCObjectState::White) {
        
        std::lock_guard<std::mutex> lock(gc_mutex_);
        child->state = GCObjectState::Gray;
        gray_list_.push_back(child);
    }
}

GCStatistics GarbageCollector::get_statistics() const {
    std::lock_guard<std::mutex> lock(gc_mutex_);
    
    stats_.total_memory = allocated_bytes_;
    stats_.used_memory = allocated_bytes_;
    
    return stats_;
}

void GarbageCollector::reset_statistics() {
    std::lock_guard<std::mutex> lock(gc_mutex_);
    
    // الاحتفاظ بالكائنات الحية فقط / Keep only live objects
    size_t live = stats_.live_objects;
    std::memset(&stats_, 0, sizeof(stats_));
    stats_.live_objects = live;
}

void GarbageCollector::print_statistics() const {
    GCStatistics stats = get_statistics();
    
    std::cout << "=== إحصائيات جامع القمامة / GC Statistics ===" << std::endl;
    std::cout << "إجمالي العمليات / Total Collections: " 
              << stats.total_collections << std::endl;
    std::cout << "إجمالي الكائنات / Total Objects: " 
              << stats.total_objects << std::endl;
    std::cout << "الكائنات الحية / Live Objects: " 
              << stats.live_objects << std::endl;
    std::cout << "الكائنات الميتة / Dead Objects: " 
              << stats.dead_objects << std::endl;
    std::cout << "إجمالي الذاكرة / Total Memory: " 
              << stats.total_memory << " bytes" << std::endl;
    std::cout << "الذاكرة المستخدمة / Used Memory: " 
              << stats.used_memory << " bytes" << std::endl;
    std::cout << "الذاكرة المحررة / Freed Memory: " 
              << stats.freed_memory << " bytes" << std::endl;
    std::cout << "آخر وقت جمع / Last Collection Time: " 
              << stats.last_collection_time << " μs" << std::endl;
    std::cout << "إجمالي وقت الجمع / Total Collection Time: " 
              << stats.total_collection_time << " μs" << std::endl;
    std::cout << "أقصى وقت توقف / Max Pause Time: " 
              << stats.max_pause_time << " μs" << std::endl;
}

// Private Methods / الدوال الخاصة

void GarbageCollector::mark() {
    // تتبع الجذور / Trace roots
    trace_roots();
    
    // معالجة القائمة الرمادية / Process gray list
    process_gray_list();
}

void GarbageCollector::mark_object(GCObject* obj) {
    if (!obj || obj->state != GCObjectState::White) {
        return;
    }
    
    // تعليم الكائن كرمادي / Mark as gray
    obj->state = GCObjectState::Gray;
    gray_list_.push_back(obj);
}

size_t GarbageCollector::sweep() {
    size_t freed_bytes = 0;
    size_t dead_count = 0;
    
    GCObject** current = &objects_list_;
    
    while (*current) {
        GCObject* obj = *current;
        
        // إذا كان أبيض ولم يكن مثبتًا، قم بتحريره
        if (obj->state == GCObjectState::White && 
            !obj->pinned && 
            obj->ref_count == 0) {
            
            // إزالة من القائمة / Remove from list
            *current = obj->next;
            
            // استدعاء المدمر / Call finalizer
            if (obj->finalize_fn) {
                obj->finalize_fn();
            }
            
            // تحرير الذاكرة / Free memory
            size_t obj_size = sizeof(GCObject) + obj->size - 1;
            freed_bytes += obj_size;
            dead_count++;
            
            MemoryAllocator::instance().deallocate(obj);
        }
        else {
            // الانتقال للتالي / Move to next
            current = &(obj->next);
        }
    }
    
    allocated_bytes_ -= freed_bytes;
    stats_.dead_objects += dead_count;
    stats_.live_objects -= dead_count;
    
    return freed_bytes;
}

void GarbageCollector::compact() {
    // تنفيذ ضغط الذاكرة / Memory compaction implementation
    // هذا يقلل التجزئة ويحسن cache locality
    // This reduces fragmentation and improves cache locality
    
    if (!config_.enable_compaction) {
        return; // الضغط معطل / Compaction disabled
    }
    
    // Phase 1: حساب العناوين الجديدة / Calculate new addresses
    // نقوم بترتيب الكائنات الحية بدون فجوات
    // We arrange live objects without gaps
    
    std::vector<GCObject*> live_objects;
    size_t new_offset = 0;
    
    GCObject* current = objects_list_;
    while (current) {
        if (current->state == GCObjectState::Black || 
            current->pinned || 
            current->ref_count > 0) {
            // كائن حي / Live object
            live_objects.push_back(current);
            
            // تخزين العنوان الجديد في forwarding pointer
            // Store new address in forwarding pointer
            current->forwarding_address = reinterpret_cast<void*>(new_offset);
            
            size_t obj_size = sizeof(GCObject) + current->size - 1;
            new_offset += obj_size;
        }
        current = current->next;
    }
    
    if (live_objects.empty()) {
        return; // لا يوجد كائنات للضغط / No objects to compact
    }
    
    // Phase 2: تخصيص مساحة جديدة / Allocate new space
    void* new_heap = MemoryAllocator::instance().allocate(new_offset);
    if (!new_heap) {
        // فشل التخصيص - نتخطى الضغط / Allocation failed - skip compaction
        return;
    }
    
    // Phase 3: نسخ الكائنات للمساحة الجديدة / Copy objects to new space
    char* dest = static_cast<char*>(new_heap);
    
    for (GCObject* obj : live_objects) {
        size_t obj_size = sizeof(GCObject) + obj->size - 1;
        
        // نسخ الكائن / Copy object
        GCObject* new_obj = reinterpret_cast<GCObject*>(dest);
        std::memcpy(new_obj, obj, obj_size);
        
        // تحديث forwarding pointer للعنوان الفعلي
        // Update forwarding pointer to actual address
        obj->forwarding_address = new_obj;
        
        dest += obj_size;
    }
    
    // Phase 4: تحديث جميع المراجع / Update all references
    // تحديث الجذور / Update roots
    for (GCRoot& root : roots_) {
        if (!root.location) continue;
        
        GCObject** obj_ptr = static_cast<GCObject**>(root.location);
        if (*obj_ptr && (*obj_ptr)->forwarding_address) {
            *obj_ptr = static_cast<GCObject*>((*obj_ptr)->forwarding_address);
        }
    }
    
    // تحديث المراجع الداخلية / Update internal references
    GCObject* new_current = static_cast<GCObject*>(new_heap);
    GCObject* prev = nullptr;
    
    for (size_t i = 0; i < live_objects.size(); i++) {
        // تحديث قائمة الربط / Update linked list
        if (prev) {
            prev->next = new_current;
        }
        
        // تحديث المراجع داخل الكائن / Update references within object
        if (new_current->trace_fn) {
            // استدعاء trace_fn لتحديث المراجع
            // Call trace_fn to update references
            // ملاحظة: يجب أن يتم تعديل trace_fn لتحديث المراجع
            // Note: trace_fn should be modified to update references
        }
        
        prev = new_current;
        size_t obj_size = sizeof(GCObject) + new_current->size - 1;
        new_current = reinterpret_cast<GCObject*>(
            reinterpret_cast<char*>(new_current) + obj_size);
    }
    
    if (prev) {
        prev->next = nullptr;
    }
    
    // Phase 5: تحرير الكومة القديمة وتحديث المؤشر / Free old heap and update pointer
    // تحرير الكائنات القديمة / Free old objects
    current = objects_list_;
    while (current) {
        GCObject* next = current->next;
        // لا نحرر الذاكرة هنا لأن الكائنات منسوخة
        // We don't free memory here as objects are copied
        current = next;
    }
    
    // تحديث قائمة الكائنات / Update objects list
    objects_list_ = static_cast<GCObject*>(new_heap);
    
    // تحديث الإحصائيات / Update statistics
    stats_.compactions++;
    
    // حساب الذاكرة المحررة بعد الضغط / Calculate freed memory after compaction
    size_t old_allocated = allocated_bytes_;
    allocated_bytes_ = new_offset;
    size_t freed = old_allocated - new_offset;
    
    stats_.bytes_freed += freed;
}

void GarbageCollector::trace_roots() {
    for (const GCRoot& root : roots_) {
        if (!root.location) continue;
        
        // الحصول على الكائن من الجذر / Get object from root
        GCObject* obj = *static_cast<GCObject**>(root.location);
        
        if (obj) {
            mark_object(obj);
        }
    }
}

void GarbageCollector::process_gray_list() {
    while (!gray_list_.empty()) {
        GCObject* obj = gray_list_.back();
        gray_list_.pop_back();
        
        // تعليم الكائن كأسود / Mark as black
        obj->state = GCObjectState::Black;
        
        // تتبع المراجع / Trace references
        if (obj->trace_fn) {
            obj->trace_fn(this);
        }
    }
}

void GarbageCollector::reset_states() {
    gray_list_.clear();
    
    GCObject* current = objects_list_;
    while (current) {
        // إعادة تعيين الحالة إلى أبيض / Reset to white
        if (current->state != GCObjectState::White) {
            current->state = GCObjectState::White;
        }
        current = current->next;
    }
}

} // namespace Memory
} // namespace Sad
