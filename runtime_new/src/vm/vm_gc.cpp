/**
 * @file vm_gc.cpp
 * @brief تطبيق جامع القمامة المتقدم / Advanced GC Implementation
 * @brief Implementation of tri-color marking with generational collection
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 2.0
 */

#include "vm/vm_gc.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstring>

namespace Sad {
namespace VM {

// ========================================
// GCStats Implementation
// ========================================

void GCStats::print() const {
    std::cout << "\n========================================\n";
    std::cout << "إحصائيات جامع القمامة / GC Statistics\n";
    std::cout << "========================================\n";
    
    // (AR) دورات GC / (EN) GC cycles
    std::cout << "دورات GC / GC Cycles:\n";
    std::cout << "  الجيل الصغير / Young Gen: " << youngGenCollections << "\n";
    std::cout << "  الجيل الكبير / Old Gen:   " << oldGenCollections << "\n";
    std::cout << "  الكامل / Full:             " << fullCollections << "\n";
    
    // (AR) الوقت / (EN) Time
    std::cout << "\nالوقت / Time (ms):\n";
    std::cout << "  المجموع / Total:           " << totalGCTime << "\n";
    std::cout << "  الجيل الصغير / Young Gen: " << youngGenTime << "\n";
    std::cout << "  الجيل الكبير / Old Gen:   " << oldGenTime << "\n";
    std::cout << "  أطول وقف / Max Pause:     " << maxPauseTime << "\n";
    
    // (AR) الكائنات / (EN) Objects
    std::cout << "\nالكائنات / Objects:\n";
    std::cout << "  المُخصَّصة / Allocated:    " << objectsAllocated << "\n";
    std::cout << "  المُحرَّرة / Freed:         " << objectsFreed << "\n";
    std::cout << "  المُرقّاة / Promoted:       " << objectsPromoted << "\n";
    std::cout << "  الحية (صغير) / Live (Y):  " << youngGenObjects << "\n";
    std::cout << "  الحية (كبير) / Live (O):  " << oldGenObjects << "\n";
    
    // (AR) الذاكرة / (EN) Memory
    double allocatedMB = bytesAllocated / (1024.0 * 1024.0);
    double freedMB = bytesFreed / (1024.0 * 1024.0);
    double peakMB = peakMemoryUsage / (1024.0 * 1024.0);
    
    std::cout << "\nالذاكرة / Memory (MB):\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  المُخصَّصة / Allocated:    " << allocatedMB << "\n";
    std::cout << "  المُحرَّرة / Freed:         " << freedMB << "\n";
    std::cout << "  الذروة / Peak:             " << peakMB << "\n";
    
    std::cout << "========================================\n\n";
}

void GCStats::reset() {
    youngGenCollections = 0;
    oldGenCollections = 0;
    fullCollections = 0;
    totalGCTime = 0;
    youngGenTime = 0;
    oldGenTime = 0;
    objectsAllocated = 0;
    objectsFreed = 0;
    objectsPromoted = 0;
    bytesAllocated = 0;
    bytesFreed = 0;
    peakMemoryUsage = 0;
    youngGenObjects = 0;
    oldGenObjects = 0;
    maxPauseTime = 0;
}

// ========================================
// GarbageCollector Implementation
// ========================================

GarbageCollector::GarbageCollector(VirtualMachine* vm, const GCConfig& config)
    : vm_(vm)
    , config_(config)
{
    // (AR) احجز مساحة للقوائم / (EN) Reserve space for lists
    youngGen_.reserve(1000);
    oldGen_.reserve(1000);
    grayObjects_.reserve(500);
}

GarbageCollector::~GarbageCollector() {
    // (AR) حرّر جميع الكائنات / (EN) Free all objects
    for (Object* obj : youngGen_) {
        freeObject(obj);
    }
    for (Object* obj : oldGen_) {
        freeObject(obj);
    }
}

// ========================================
// Allocation - التخصيص
// ========================================

Object* GarbageCollector::allocate(ObjectType type, size_t size) {
    // (AR) تحقق إذا كان يجب تشغيل GC / (EN) Check if GC should run
    maybeCollect();
    
    // (AR) خصص الكائن / (EN) Allocate object
    Object* obj = static_cast<Object*>(std::malloc(size));
    if (!obj) {
        // (AR) حاول GC ثم أعد المحاولة / (EN) Try GC and retry
        collectFull();
        obj = static_cast<Object*>(std::malloc(size));
        if (!obj) {
            throw std::bad_alloc();
        }
    }
    
    obj->type = type;
    obj->isMarked = false;  // (AR) إصلاح: الحقل الصحيح هو isMarked وليس marked
    obj->next = nullptr;
    
    // (AR) أضف للجيل الصغير / (EN) Add to young generation
    youngGen_.push_back(obj);
    
    // (AR) أنشئ البيانات الإضافية / (EN) Create metadata
    ObjectMetadata meta;
    meta.color = ObjectColor::WHITE;
    meta.generation = 0;
    meta.age = 0;
    meta.pinned = false;
    meta.size = size;
    metadata_[obj] = meta;
    
    // (AR) حدّث الإحصائيات / (EN) Update statistics
    stats_.objectsAllocated++;
    stats_.bytesAllocated += size;
    youngGenSize_ += size;
    
    if (stats_.bytesAllocated - stats_.bytesFreed > stats_.peakMemoryUsage) {
        stats_.peakMemoryUsage = stats_.bytesAllocated - stats_.bytesFreed;
    }
    
    if (config_.verbose) {
        std::cout << "[GC] Allocated " << size << " bytes for " 
                  << objectTypeToString(type) << "\n";
    }
    
    return obj;
}

void GarbageCollector::pinObject(Object* obj) {
    ObjectMetadata* meta = getMetadata(obj);
    if (meta) {
        meta->pinned = true;
    }
}

void GarbageCollector::unpinObject(Object* obj) {
    ObjectMetadata* meta = getMetadata(obj);
    if (meta) {
        meta->pinned = false;
    }
}

// ========================================
// Collection - الجمع
// ========================================

void GarbageCollector::collectYoung() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (config_.verbose) {
        std::cout << "[GC] Starting young generation collection...\n";
    }
    
    // (AR) المرحلة 1: التلوين / (EN) Phase 1: Marking
    markWhite();
    markRoots();
    processGray();
    
    // (AR) المرحلة 2: الكنس / (EN) Phase 2: Sweeping
    size_t freedCount = sweep();
    
    // (AR) المرحلة 3: الترقية / (EN) Phase 3: Promotion
    promoteObjects();
    
    // (AR) حدّث الإحصائيات / (EN) Update statistics
    stats_.youngGenCollections++;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats_.youngGenTime += duration.count();
    stats_.totalGCTime += duration.count();
    
    if (duration.count() > stats_.maxPauseTime) {
        stats_.maxPauseTime = duration.count();
    }
    
    if (config_.verbose) {
        std::cout << "[GC] Young gen collection completed in " << duration.count() 
                  << "ms, freed " << freedCount << " objects\n";
    }
    
    // (AR) اضبط العتبات / (EN) Adjust thresholds
    adjustThresholds();
}

void GarbageCollector::collectOld() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (config_.verbose) {
        std::cout << "[GC] Starting old generation collection...\n";
    }
    
    // (AR) جمع الجيل الكبير مشابه للجيل الصغير لكن على oldGen_
    // (EN) Old gen collection similar to young but on oldGen_
    markWhite();
    markRoots();
    processGray();
    size_t freedCount = sweep();
    
    stats_.oldGenCollections++;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats_.oldGenTime += duration.count();
    stats_.totalGCTime += duration.count();
    
    if (duration.count() > stats_.maxPauseTime) {
        stats_.maxPauseTime = duration.count();
    }
    
    if (config_.verbose) {
        std::cout << "[GC] Old gen collection completed in " << duration.count() 
                  << "ms, freed " << freedCount << " objects\n";
    }
    
    adjustThresholds();
}

void GarbageCollector::collectFull() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (config_.verbose) {
        std::cout << "[GC] Starting full collection...\n";
    }
    
    // (AR) جمع كامل لكل الأجيال / (EN) Full collection of all generations
    markWhite();
    markRoots();
    processGray();
    size_t freedCount = sweep();
    
    stats_.fullCollections++;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats_.totalGCTime += duration.count();
    
    if (duration.count() > stats_.maxPauseTime) {
        stats_.maxPauseTime = duration.count();
    }
    
    if (config_.verbose) {
        std::cout << "[GC] Full collection completed in " << duration.count() 
                  << "ms, freed " << freedCount << " objects\n";
    }
    
    adjustThresholds();
}

bool GarbageCollector::incrementalStep() {
    if (!incrementalInProgress_) {
        // (AR) ابدأ دورة جديدة / (EN) Start new cycle
        markWhite();
        markRoots();
        incrementalInProgress_ = true;
        incrementalPosition_ = 0;
        return false;
    }
    
    // (AR) عالج بعض الكائنات الرمادية / (EN) Process some gray objects
    size_t processed = 0;
    while (processed < config_.incrementalStepSize && !grayObjects_.empty()) {
        Object* obj = grayObjects_.back();
        grayObjects_.pop_back();
        processObject(obj);
        processed++;
    }
    
    // (AR) إذا انتهت الكائنات الرمادية، اكنس / (EN) If no more gray objects, sweep
    if (grayObjects_.empty()) {
        sweep();
        incrementalInProgress_ = false;
        return true; // (AR) اكتملت الدورة / (EN) Cycle completed
    }
    
    return false;
}

void GarbageCollector::maybeCollect() {
    // (AR) تحقق من عتبة الجيل الصغير / (EN) Check young gen threshold
    if (youngGenSize_ >= config_.youngGenThreshold) {
        if (config_.enableIncremental) {
            // (AR) عدة خطوات تدريجية / (EN) Multiple incremental steps
            for (int i = 0; i < 5; ++i) {
                if (incrementalStep()) break;
            }
        } else {
            collectYoung();
        }
    }
    
    // (AR) تحقق من عتبة الجيل الكبير / (EN) Check old gen threshold
    if (oldGenSize_ >= config_.oldGenThreshold) {
        collectOld();
    }
}

// ========================================
// Write Barriers - حواجز الكتابة
// ========================================

void GarbageCollector::writeBarrier(Object* from, Object* to) {
    if (!from || !to) return;
    
    ObjectMetadata* fromMeta = getMetadata(from);
    ObjectMetadata* toMeta = getMetadata(to);
    
    if (!fromMeta || !toMeta) return;
    
    // (AR) إذا كان الكائن المصدر أسود والهدف أبيض، لوّن الهدف بالرمادي
    // (EN) If source is black and target is white, mark target as gray
    if (fromMeta->color == ObjectColor::BLACK && 
        toMeta->color == ObjectColor::WHITE) {
        toMeta->color = ObjectColor::GRAY;
        grayObjects_.push_back(to);
    }
    
    // (AR) إذا كان المرجع من الجيل الكبير للصغير، سجله
    // (EN) If reference from old to young, record it
    if (fromMeta->generation == 1 && toMeta->generation == 0) {
        // (AR) أضف للجذور المؤقتة / (EN) Add to temporary roots
        roots_.insert(to);
    }
}

// ========================================
// Roots Management
// ========================================

void GarbageCollector::addRoot(Object* obj) {
    if (obj) {
        roots_.insert(obj);
    }
}

void GarbageCollector::removeRoot(Object* obj) {
    roots_.erase(obj);
}

// ========================================
// Configuration
// ========================================

void GarbageCollector::setConfig(const GCConfig& config) {
    config_ = config;
}

size_t GarbageCollector::getMemoryUsage() const {
    return stats_.bytesAllocated - stats_.bytesFreed;
}

// ========================================
// Internal Methods - الطرق الداخلية
// ========================================

void GarbageCollector::markWhite() {
    // (AR) لوّن جميع الكائنات بالأبيض / (EN) Mark all objects as white
    for (Object* obj : youngGen_) {
        ObjectMetadata* meta = getMetadata(obj);
        if (meta) {
            meta->color = ObjectColor::WHITE;
        }
    }
    
    for (Object* obj : oldGen_) {
        ObjectMetadata* meta = getMetadata(obj);
        if (meta) {
            meta->color = ObjectColor::WHITE;
        }
    }
    
    grayObjects_.clear();
}

void GarbageCollector::markRoots() {
    // (AR) لوّن الجذور بالرمادي / (EN) Mark roots as gray
    for (Object* root : roots_) {
        ObjectMetadata* meta = getMetadata(root);
        if (meta && meta->color == ObjectColor::WHITE) {
            meta->color = ObjectColor::GRAY;
            grayObjects_.push_back(root);
        }
    }
    
    // (AR) أضف كائنات المكدس كجذور / (EN) Add stack objects as roots
    if (vm_) {
        // (AR) نستخدم getStack() و getGlobals() للوصول للمكدس والمتغيرات العامة
        // (EN) Use getStack() and getGlobals() to access stack and globals
        const auto& vmStack = vm_->getStack();
        for (size_t i = 0; i < vmStack.size(); ++i) {
            const Value& val = vmStack[i];
            if (val.isObject()) {
                Object* obj = val.asObject();
                ObjectMetadata* meta = getMetadata(obj);
                if (meta && meta->color == ObjectColor::WHITE) {
                    meta->color = ObjectColor::GRAY;
                    grayObjects_.push_back(obj);
                }
            }
        }
    }
}

void GarbageCollector::processGray() {
    // (AR) عالج جميع الكائنات الرمادية / (EN) Process all gray objects
    while (!grayObjects_.empty()) {
        Object* obj = grayObjects_.back();
        grayObjects_.pop_back();
        processObject(obj);
    }
}

void GarbageCollector::processObject(Object* obj) {
    ObjectMetadata* meta = getMetadata(obj);
    if (!meta || meta->color != ObjectColor::GRAY) {
        return;
    }
    
    // (AR) لوّن الكائن بالأسود / (EN) Mark object as black
    meta->color = ObjectColor::BLACK;
    
    // (AR) لوّن المراجع بالرمادي / (EN) Mark references as gray
    switch (obj->type) {
        case ObjectType::OBJ_ARRAY: {
            ArrayObject* arr = static_cast<ArrayObject*>(obj);
            // (AR) الـ elements هو vector — نمر على عناصره مباشرة
            // (EN) elements is a vector — iterate directly
            for (const Value& val : arr->elements) {
                if (val.isObject()) {
                    Object* child = val.asObject();
                    ObjectMetadata* childMeta = getMetadata(child);
                    if (childMeta && childMeta->color == ObjectColor::WHITE) {
                        childMeta->color = ObjectColor::GRAY;
                        grayObjects_.push_back(child);
                    }
                }
            }
            break;
        }
        
        case ObjectType::OBJ_FUNCTION: {
            // (AR) FunctionObject لا يحتوي على ثوابت — لا شيء لتلوينه
            // (EN) FunctionObject has no constants pool — nothing to mark
            break;
        }
        
        case ObjectType::OBJ_STRING:
        case ObjectType::OBJ_NATIVE:
            // (AR) لا توجد مراجع / (EN) No references
            break;
            
        default:
            break;
    }
}

size_t GarbageCollector::sweep() {
    size_t freedCount = 0;
    
    // (AR) اكنس الجيل الصغير / (EN) Sweep young generation
    auto youngIt = youngGen_.begin();
    while (youngIt != youngGen_.end()) {
        Object* obj = *youngIt;
        ObjectMetadata* meta = getMetadata(obj);
        
        if (!meta || meta->color == ObjectColor::WHITE) {
            // (AR) حرّر الكائن / (EN) Free object
            freeObject(obj);
            metadata_.erase(obj);
            youngIt = youngGen_.erase(youngIt);
            freedCount++;
        } else {
            ++youngIt;
        }
    }
    
    // (AR) اكنس الجيل الكبير / (EN) Sweep old generation
    auto oldIt = oldGen_.begin();
    while (oldIt != oldGen_.end()) {
        Object* obj = *oldIt;
        ObjectMetadata* meta = getMetadata(obj);
        
        if (!meta || meta->color == ObjectColor::WHITE) {
            freeObject(obj);
            metadata_.erase(obj);
            oldIt = oldGen_.erase(oldIt);
            freedCount++;
        } else {
            ++oldIt;
        }
    }
    
    stats_.objectsFreed += freedCount;
    return freedCount;
}

void GarbageCollector::promoteObjects() {
    // (AR) رقّي الكائنات الناجية القديمة إلى الجيل الكبير
    // (EN) Promote old surviving objects to old generation
    auto it = youngGen_.begin();
    while (it != youngGen_.end()) {
        Object* obj = *it;
        ObjectMetadata* meta = getMetadata(obj);
        
        if (meta && meta->color == ObjectColor::BLACK) {
            meta->age++;
            
            if (meta->age >= config_.promotionAge && !meta->pinned) {
                // (AR) رقّي للجيل الكبير / (EN) Promote to old gen
                meta->generation = 1;
                oldGen_.push_back(obj);
                youngGenSize_ -= meta->size;
                oldGenSize_ += meta->size;
                it = youngGen_.erase(it);
                stats_.objectsPromoted++;
                
                if (config_.verbose) {
                    std::cout << "[GC] Promoted object (age " << (int)meta->age << ")\n";
                }
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
    
    // (AR) حدّث إحصائيات الكائنات الحية / (EN) Update live object stats
    stats_.youngGenObjects = youngGen_.size();
    stats_.oldGenObjects = oldGen_.size();
}

void GarbageCollector::freeObject(Object* obj) {
    if (!obj) return;
    
    ObjectMetadata* meta = getMetadata(obj);
    size_t size = meta ? meta->size : calculateObjectSize(obj);
    
    // (AR) حرّر ذاكرة الكائن حسب نوعه / (EN) Free object memory by type
    switch (obj->type) {
        case ObjectType::OBJ_STRING: {
            StringObject* str = static_cast<StringObject*>(obj);
            delete[] str->chars;
            break;
        }
        
        case ObjectType::OBJ_ARRAY: {
            // (AR) الـ elements هو vector — ندمره بشكل صحيح
            // (EN) elements is a vector — destroy it properly
            ArrayObject* arr = static_cast<ArrayObject*>(obj);
            arr->elements.~vector<Value>();
            break;
        }
        
        case ObjectType::OBJ_FUNCTION: {
            // (AR) الـ code هو vector — ندمره بشكل صحيح
            // (EN) code is a vector — destroy it properly
            FunctionObject* func = static_cast<FunctionObject*>(obj);
            func->code.~vector<uint8_t>();
            func->lines.~vector<int>();
            break;
        }
        
        default:
            break;
    }
    
    std::free(obj);
    
    // (AR) حدّث الإحصائيات / (EN) Update statistics
    stats_.bytesFreed += size;
    
    if (meta) {
        if (meta->generation == 0) {
            youngGenSize_ -= size;
        } else {
            oldGenSize_ -= size;
        }
    }
}

ObjectMetadata* GarbageCollector::getMetadata(Object* obj) {
    auto it = metadata_.find(obj);
    return (it != metadata_.end()) ? &it->second : nullptr;
}

void GarbageCollector::adjustThresholds() {
    // (AR) اضبط العتبات ديناميكيًا بناءً على معدل النجاة
    // (EN) Adjust thresholds dynamically based on survival rate
    
    if (stats_.youngGenCollections > 0) {
        double survivalRate = static_cast<double>(stats_.youngGenObjects) / 
                            (stats_.youngGenObjects + stats_.objectsFreed + 1);
        
        if (survivalRate > 0.7) {
            // (AR) معدل نجاة عالٍ، زد العتبة / (EN) High survival, increase threshold
            config_.youngGenThreshold = static_cast<size_t>(config_.youngGenThreshold * 1.2);
        } else if (survivalRate < 0.3) {
            // (AR) معدل نجاة منخفض، قلل العتبة / (EN) Low survival, decrease threshold
            config_.youngGenThreshold = static_cast<size_t>(config_.youngGenThreshold * 0.8);
        }
        
        // (AR) حدود معقولة / (EN) Reasonable limits
        config_.youngGenThreshold = std::max<size_t>(256 * 1024, 
                                    std::min<size_t>(4 * 1024 * 1024, 
                                                     config_.youngGenThreshold));
    }
}

// ========================================
// Helper Functions
// ========================================

const char* colorToString(ObjectColor color) {
    switch (color) {
        case ObjectColor::WHITE: return "أبيض/White";
        case ObjectColor::GRAY:  return "رمادي/Gray";
        case ObjectColor::BLACK: return "أسود/Black";
        default: return "غير معروف/Unknown";
    }
}

size_t calculateObjectSize(Object* obj) {
    switch (obj->type) {
        case ObjectType::OBJ_STRING: {
            StringObject* str = static_cast<StringObject*>(obj);
            return sizeof(StringObject) + str->length + 1;
        }
        case ObjectType::OBJ_ARRAY: {
            ArrayObject* arr = static_cast<ArrayObject*>(obj);
            // (AR) الـ elements هو vector — نحسب سعته الفعلية
            // (EN) elements is a vector — calculate its actual capacity
            return sizeof(ArrayObject) + arr->elements.capacity() * sizeof(Value);
        }
        case ObjectType::OBJ_FUNCTION: {
            FunctionObject* func = static_cast<FunctionObject*>(obj);
            // (AR) الـ code هو vector — نحسب حجمه
            // (EN) code is a vector — calculate its size
            return sizeof(FunctionObject) + func->code.size();
        }
        case ObjectType::OBJ_NATIVE:
            return sizeof(NativeObject);
        default:
            return sizeof(Object);
    }
}

} // namespace VM
} // namespace Sad
