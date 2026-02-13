// وضع GC للمبتدئين — Garbage Collector Fallback
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace sadc {
namespace memory {

struct GcObject {
    void* data;
    size_t size;
    bool marked;
    std::vector<GcObject*> references;
};

class GarbageCollector {
    std::vector<GcObject*> heap;
    std::vector<GcObject*> roots;
    std::mutex gc_mutex;
    size_t total_allocated = 0;
    size_t gc_threshold = 1024 * 1024; // 1MB
    bool enabled = false;

public:
    void enable() { enabled = true; }
    void disable() { enabled = false; }
    bool isEnabled() const { return enabled; }

    void* allocate(size_t size) {
        std::lock_guard<std::mutex> lock(gc_mutex);
        auto obj = new GcObject();
        obj->data = malloc(size);
        obj->size = size;
        obj->marked = false;
        heap.push_back(obj);
        total_allocated += size;
        if (enabled && total_allocated > gc_threshold) {
            collectInternal();
        }
        return obj->data;
    }

    void addRoot(GcObject* obj) {
        std::lock_guard<std::mutex> lock(gc_mutex);
        roots.push_back(obj);
    }

    void removeRoot(GcObject* obj) {
        std::lock_guard<std::mutex> lock(gc_mutex);
        roots.erase(std::remove(roots.begin(), roots.end(), obj), roots.end());
    }

    void collect() {
        std::lock_guard<std::mutex> lock(gc_mutex);
        collectInternal();
    }

    size_t getTotalAllocated() const { return total_allocated; }
    size_t getHeapSize() const { return heap.size(); }

private:
    void collectInternal() {
        // Mark phase
        for (auto* obj : heap) obj->marked = false;
        for (auto* root : roots) mark(root);

        // Sweep phase
        auto it = heap.begin();
        while (it != heap.end()) {
            if (!(*it)->marked) {
                total_allocated -= (*it)->size;
                free((*it)->data);
                delete *it;
                it = heap.erase(it);
            } else {
                ++it;
            }
        }

        gc_threshold = total_allocated * 2;
        if (gc_threshold < 1024 * 1024) gc_threshold = 1024 * 1024;
    }

    void mark(GcObject* obj) {
        if (!obj || obj->marked) return;
        obj->marked = true;
        for (auto* ref : obj->references) {
            mark(ref);
        }
    }
};

} // namespace memory
} // namespace sadc
