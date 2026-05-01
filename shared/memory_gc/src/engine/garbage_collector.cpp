// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) تطبيق محرّك جامع القمامة الموحَّد
// (EN) Implementation of the unified GC engine
// ═══════════════════════════════════════════════════════════════════════════════════
//
// (AR) منقول حرفياً من llvm_runtime_io_gc.cpp (الدوال sad_llvm_gc_*) ومن
//      llvm_runtime.cpp (gc_init/gc_expand/gc_find_object) مع تحويلات هندسية:
//        - حذف bookkeeping اليدوي (capacity / count) واستخدام std::vector (CW-16: RAII)
//        - توحيد الحالة في صنف واحد بدل بنية + دوال حرة (CW-01: SRP، CW-19: DRY)
//        - حماية كل عملية بـ std::mutex (لم يكن آمناً للتزامن سابقاً)
//        - الحفاظ التام على الدلالات: نفس خوارزمية mark-and-sweep، نفس الـ fallback
//          المحافظ عند انعدام الجذور، نفس آلية الجمع التدريجي بطورين.
// ═══════════════════════════════════════════════════════════════════════════════════

#include "memory/gc/engine/garbage_collector.h"

#include <cstdlib>   // (AR) free() لتحرير الكائنات في sweep / (EN) free()
#include <algorithm> // (AR) std::min في الجمع التدريجي

namespace Sad
{
    namespace Memory
    {
        namespace GC
        {

            // ─────────────────────────────────────────────────────────────────────────────────
            // (AR) البنّاء/الهادم
            // ─────────────────────────────────────────────────────────────────────────────────

            GarbageCollector::GarbageCollector()
                : nextProviderId_(1), totalAllocated_(0), collections_(0), paused_(false), incPhase_(0), incIndex_(0)
            {
                // (AR) سعة ابتدائية مماثلة للقديمة (1024 كائن، 256 جذر) لتقليل reallocations.
                // (EN) Same initial capacity as the previous GCContext (1024/256).
                objects_.reserve(1024);
                sizes_.reserve(1024);
                destroyers_.reserve(1024);
                visitors_.reserve(1024);
                roots_.reserve(256);
            }

            GarbageCollector::~GarbageCollector() = default;

            // ─────────────────────────────────────────────────────────────────────────────────
            // (AR) دوال داخلية (تفترض أن mutex_ مأخوذ)
            // ─────────────────────────────────────────────────────────────────────────────────

            int64_t GarbageCollector::findIndexLocked(void *ptr) const
            {
                // (AR) بحث خطّي مطابق للسلوك القديم (gc_find_object). تعقيده O(n) لكل
                //      عملية register/collect — مقبول حالياً لأن workloads sadc صغيرة.
                //      تحسين مستقبلي محتمل: hash-map (CW-30: لا تُحسّن بدون قياس).
                // (EN) Linear search matching legacy behavior. O(n) per lookup; deferred
                //      optimization (no profiling evidence yet).
                for (size_t i = 0; i < objects_.size(); ++i)
                {
                    if (objects_[i] == ptr)
                    {
                        return static_cast<int64_t>(i);
                    }
                }
                return -1;
            }

            void GarbageCollector::markFromRootsLocked(std::vector<bool> &marked) const
            {
                // (AR) جمع الجذور من المصدرين: المجموعة الثابتة (roots_) + الموفّرون الديناميكيون.
                //      (B-step5b-iii) الموفّرون يُصدرون مؤشرات حيّة مرتبطة بحالة المفسّر.
                // (EN) Collect roots from both: the static set (roots_) and dynamic providers.
                std::vector<void *> worklist;
                worklist.reserve(roots_.size() + 64);
                for (void *r : roots_)
                {
                    if (r != nullptr)
                    {
                        worklist.push_back(r);
                    }
                }
                for (const auto &entry : rootProviders_)
                {
                    const auto &provider = entry.second;
                    if (!provider)
                    {
                        continue;
                    }
                    provider([&worklist](void *p)
                             {
                        if (p != nullptr)
                        {
                            worklist.push_back(p);
                        } });
                }

                // (AR) إذا لم نحصل على أي جذر من المصدرين، نُبقي الـ fallback المحافظ
                //      الذي يعلّم كل الكائنات حيّة — نفس سلوك السابق لتجنّب التراجع.
                // (EN) If neither static roots nor providers emitted anything, keep the
                //      conservative fallback that marks all objects alive.
                if (worklist.empty())
                {
                    for (size_t i = 0; i < objects_.size(); ++i)
                    {
                        if (objects_[i] != nullptr)
                        {
                            marked[i] = true;
                        }
                    }
                    return;
                }

                // (AR) BFS عبر الـ worklist.
                while (!worklist.empty())
                {
                    void *cur = worklist.back();
                    worklist.pop_back();
                    if (cur == nullptr)
                    {
                        continue;
                    }
                    int64_t idx = findIndexLocked(cur);
                    if (idx < 0 || marked[static_cast<size_t>(idx)])
                    {
                        continue;
                    }
                    marked[static_cast<size_t>(idx)] = true;

                    // (AR) لو وُجد visitor دقيق لهذا الكائن، استخدمه — وإلا الـ conservative scan.
                    auto &visitor = visitors_[static_cast<size_t>(idx)];
                    if (visitor)
                    {
                        visitor(cur, [&](void *child)
                                {
                            if (child == nullptr)
                            {
                                return;
                            }
                            int64_t cidx = findIndexLocked(child);
                            if (cidx >= 0 && !marked[static_cast<size_t>(cidx)])
                            {
                                worklist.push_back(child);
                            } });
                        continue;
                    }

                    const uint64_t objSize = sizes_[static_cast<size_t>(idx)];
                    void **objData = static_cast<void **>(cur);
                    const uint64_t ptrCount = objSize / sizeof(void *);

                    for (uint64_t p = 0; p < ptrCount; ++p)
                    {
                        void *maybeRef = objData[p];
                        if (maybeRef == nullptr)
                        {
                            continue;
                        }
                        int64_t refIdx = findIndexLocked(maybeRef);
                        if (refIdx >= 0 && !marked[static_cast<size_t>(refIdx)])
                        {
                            worklist.push_back(maybeRef);
                        }
                    }
                }
            }

            void GarbageCollector::sweepUnmarkedLocked(std::vector<bool> &marked, uint64_t &freedCount)
            {
                // (AR) إزالة الكائنات غير المعلَّمة من القوائم المتوازية.
                //      إذا كان للكائن دالة هدم مسجَّلة فاستدعها — وإلا std::free() (السلوك القديم).
                // (EN) Erase unmarked entries. Use destroyer if registered, else std::free().
                freedCount = 0;
                for (size_t i = 0; i < objects_.size();)
                {
                    if (!marked[i] && objects_[i] != nullptr)
                    {
                        if (destroyers_[i])
                        {
                            destroyers_[i](objects_[i]);
                        }
                        else
                        {
                            std::free(objects_[i]);
                        }
                        totalAllocated_ -= sizes_[i];
                        objects_.erase(objects_.begin() + static_cast<std::ptrdiff_t>(i));
                        sizes_.erase(sizes_.begin() + static_cast<std::ptrdiff_t>(i));
                        destroyers_.erase(destroyers_.begin() + static_cast<std::ptrdiff_t>(i));
                        visitors_.erase(visitors_.begin() + static_cast<std::ptrdiff_t>(i));
                        marked.erase(marked.begin() + static_cast<std::ptrdiff_t>(i));
                        ++freedCount;
                    }
                    else
                    {
                        ++i;
                    }
                }
            }

            // ─────────────────────────────────────────────────────────────────────────────────
            // (AR) واجهة عامة
            // ─────────────────────────────────────────────────────────────────────────────────

            void GarbageCollector::registerObject(void *ptr, uint64_t size)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                objects_.push_back(ptr);
                sizes_.push_back(size);
                destroyers_.emplace_back();
                visitors_.emplace_back();
                totalAllocated_ += size;
            }

            void GarbageCollector::registerObject(void *ptr, uint64_t size,
                                                  std::function<void(void *)> destroyer)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                objects_.push_back(ptr);
                sizes_.push_back(size);
                destroyers_.push_back(std::move(destroyer));
                visitors_.emplace_back();
                totalAllocated_ += size;
            }

            void GarbageCollector::setDestroyer(void *ptr, std::function<void(void *)> destroyer)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                int64_t idx = findIndexLocked(ptr);
                if (idx < 0)
                {
                    return;
                }
                destroyers_[static_cast<size_t>(idx)] = std::move(destroyer);
            }

            void GarbageCollector::setVisitor(void *ptr,
                                              std::function<void(void *,
                                                                 const std::function<void(void *)> &)>
                                                  visitor)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                int64_t idx = findIndexLocked(ptr);
                if (idx < 0)
                {
                    return;
                }
                visitors_[static_cast<size_t>(idx)] = std::move(visitor);
            }

            void GarbageCollector::unregisterObject(void *ptr)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                int64_t idx = findIndexLocked(ptr);
                if (idx < 0)
                {
                    return;
                }
                totalAllocated_ -= sizes_[static_cast<size_t>(idx)];
                objects_.erase(objects_.begin() + static_cast<std::ptrdiff_t>(idx));
                sizes_.erase(sizes_.begin() + static_cast<std::ptrdiff_t>(idx));
                destroyers_.erase(destroyers_.begin() + static_cast<std::ptrdiff_t>(idx));
                visitors_.erase(visitors_.begin() + static_cast<std::ptrdiff_t>(idx));
            }

            void GarbageCollector::updateObjectAfterRealloc(void *oldPtr, void *newPtr, uint64_t newSize)
            {
                if (oldPtr == nullptr || newPtr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                int64_t idx = findIndexLocked(oldPtr);
                if (idx < 0)
                {
                    return;
                }
                objects_[static_cast<size_t>(idx)] = newPtr;
                totalAllocated_ -= sizes_[static_cast<size_t>(idx)];
                sizes_[static_cast<size_t>(idx)] = newSize;
                totalAllocated_ += newSize;
            }

            void GarbageCollector::collect()
            {
                std::lock_guard<std::mutex> guard(mutex_);
                if (paused_)
                {
                    return;
                }
                ++collections_;
                if (objects_.empty())
                {
                    return;
                }

                std::vector<bool> marked(objects_.size(), false);
                markFromRootsLocked(marked);

                uint64_t freed = 0;
                sweepUnmarkedLocked(marked, freed);
                (void)freed; // (AR) محجوز للإحصائيات المستقبلية (نفس حالة الكود الأصلي)
            }

            void GarbageCollector::collectIncremental(uint64_t steps)
            {
                std::lock_guard<std::mutex> guard(mutex_);
                if (paused_)
                {
                    return;
                }

                if (incPhase_ == 0)
                {
                    // (AR) طور الوضع — تهيئة الـ marked عند بداية دورة جديدة
                    if (incIndex_ == 0)
                    {
                        incMarked_.assign(objects_.size(), false);
                    }

                    const uint64_t end = (std::min)(incIndex_ + steps,
                                                    static_cast<uint64_t>(objects_.size()));
                    for (uint64_t i = incIndex_; i < end; ++i)
                    {
                        if (objects_[static_cast<size_t>(i)] != nullptr)
                        {
                            incMarked_[static_cast<size_t>(i)] = true;
                        }
                    }

                    incIndex_ = end;
                    if (incIndex_ >= static_cast<uint64_t>(objects_.size()))
                    {
                        incPhase_ = 1;
                        incIndex_ = 0;
                    }
                }
                else
                {
                    // (AR) طور المسح — يمسح حتى steps كائنات أو نهاية القائمة
                    uint64_t swept = 0;
                    while (incIndex_ < static_cast<uint64_t>(objects_.size()) && swept < steps)
                    {
                        const size_t i = static_cast<size_t>(incIndex_);
                        if (!incMarked_[i] && objects_[i] != nullptr)
                        {
                            if (destroyers_[i])
                            {
                                destroyers_[i](objects_[i]);
                            }
                            else
                            {
                                std::free(objects_[i]);
                            }
                            totalAllocated_ -= sizes_[i];
                            objects_.erase(objects_.begin() + static_cast<std::ptrdiff_t>(i));
                            sizes_.erase(sizes_.begin() + static_cast<std::ptrdiff_t>(i));
                            destroyers_.erase(destroyers_.begin() + static_cast<std::ptrdiff_t>(i));
                            visitors_.erase(visitors_.begin() + static_cast<std::ptrdiff_t>(i));
                            incMarked_.erase(incMarked_.begin() + static_cast<std::ptrdiff_t>(i));
                            // (AR) لا نزيد incIndex_ — العنصر التالي شغل نفس الموقع
                        }
                        else
                        {
                            ++incIndex_;
                        }
                        ++swept;
                    }

                    if (incIndex_ >= static_cast<uint64_t>(objects_.size()))
                    {
                        // (AR) اكتملت دورة كاملة (mark + sweep)
                        incPhase_ = 0;
                        incIndex_ = 0;
                        incMarked_.clear();
                        ++collections_;
                    }
                }
            }

            GCStats GarbageCollector::getStats() const
            {
                std::lock_guard<std::mutex> guard(mutex_);
                GCStats s;
                s.totalAllocated = totalAllocated_;
                s.objectCount = static_cast<uint64_t>(objects_.size());
                s.collections = collections_;
                s.rootCount = static_cast<uint64_t>(roots_.size());
                s.paused = paused_;
                return s;
            }

            void GarbageCollector::pause()
            {
                std::lock_guard<std::mutex> guard(mutex_);
                paused_ = true;
            }

            void GarbageCollector::resume()
            {
                std::lock_guard<std::mutex> guard(mutex_);
                paused_ = false;
            }

            void GarbageCollector::addRoot(void *ptr)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                roots_.push_back(ptr);
            }

            void GarbageCollector::removeRoot(void *ptr)
            {
                if (ptr == nullptr)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                for (auto it = roots_.begin(); it != roots_.end(); ++it)
                {
                    if (*it == ptr)
                    {
                        roots_.erase(it);
                        return;
                    }
                }
            }

            bool GarbageCollector::isTracked(void *ptr) const
            {
                std::lock_guard<std::mutex> guard(mutex_);
                return findIndexLocked(ptr) >= 0;
            }

            // ─────────────────────────────────────────────────────────────────────────────────
            // (AR) موفّرو الجذور الديناميكيون (B-step5b-iii)
            //   كل تسجيل يولّد مُعرّفاً تصاعدياً (يبدأ من 1) لاستخدامه عند الإلغاء.
            //   الموفّرون يُحفظون في vector لأن العدد متوقّع أن يكون صغيراً جداً
            //   (1-3 عادةً: VariableManager، ScopeManager، WidgetBuilder roots) ولأن
            //   استدعاءهم يحدث ضمن mark phase الذي يحتاج ترتيب إدراج مستقر.
            // (EN) Dynamic root providers — appended to a vector keyed by ID.
            //   Expected count is tiny (1-3); preserves insertion order.
            // ─────────────────────────────────────────────────────────────────────────────────

            int GarbageCollector::addRootProvider(RootProvider provider)
            {
                if (!provider)
                {
                    // (AR) تجاهل الموفّر الفارغ — يُرجع مُعرّفاً غير صالح (0).
                    return 0;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                int id = nextProviderId_++;
                rootProviders_.emplace_back(id, std::move(provider));
                return id;
            }

            void GarbageCollector::removeRootProvider(int id)
            {
                if (id <= 0)
                {
                    return;
                }
                std::lock_guard<std::mutex> guard(mutex_);
                for (auto it = rootProviders_.begin(); it != rootProviders_.end(); ++it)
                {
                    if (it->first == id)
                    {
                        rootProviders_.erase(it);
                        return;
                    }
                }
            }

            // ─────────────────────────────────────────────────────────────────────────────────
            // (AR) المحرك الافتراضي العالمي (Meyer's singleton — آمن للتهيئة من خيوط متعدِّدة
            //      ابتداءً من C++11). يُشكّل الجسر بين extern "C" wrappers وبقية النظام.
            // (EN) Default process-wide engine (function-local static, C++11 thread-safe init).
            // ─────────────────────────────────────────────────────────────────────────────────

            GarbageCollector &defaultEngine()
            {
                static GarbageCollector instance;
                return instance;
            }

        } // namespace GC
    } // namespace Memory
} // namespace Sad
