// ===================================================================
// مجدول العمليات العادل - صدOS
// cfs.cpp - Completely Fair Scheduler
// جدولة عادلة باستخدام شجرة أحمر-أسود مع دعم تعدد الأنوية
// المرحلة 52 - مشروع لغة صاد
// ===================================================================

#include <cstdint>
#include <cstddef>
#include <array>
#include <limits>

namespace sad::os::kernel::scheduler {

// === ثوابت المجدول ===
static constexpr uint64_t NICE_0_WEIGHT       = 1024;     // وزن الأولوية الافتراضية
static constexpr uint64_t MIN_GRANULARITY_NS   = 1000000;  // أقل شريحة زمنية (1 مللي ثانية)
static constexpr uint64_t TARGET_LATENCY_NS    = 6000000;  // زمن الاستجابة المستهدف (6 مللي ثانية)
static constexpr size_t   MAX_TASKS            = 4096;     // الحد الأقصى للمهام
static constexpr size_t   MAX_CORES            = 64;       // الحد الأقصى للأنوية

// === حالة المهمة ===
enum class TaskState : uint8_t {
    Running,    // تعمل حالياً
    Ready,      // جاهزة للتشغيل
    Blocked,    // محجوبة (تنتظر مورداً)
    Sleeping,   // نائمة
    Zombie      // منتهية (تنتظر التنظيف)
};

// === معلومات الجدولة لكل مهمة ===
struct SchedEntity {
    uint32_t pid;
    TaskState state;
    int8_t nice;                // قيمة اللطف (-20 إلى +19)
    uint64_t vruntime;          // وقت التشغيل الافتراضي (بالنانو ثانية)
    uint64_t exec_start;        // وقت بداية التنفيذ الحالي
    uint64_t sum_exec_runtime;  // مجموع وقت التنفيذ الفعلي
    uint64_t weight;            // الوزن المحسوب من قيمة اللطف
    uint32_t cpu_affinity;      // تقارب النواة
    bool is_active;             // هل المهمة نشطة في الشجرة؟

    // حقول شجرة أحمر-أسود مبسطة
    int32_t rb_left;            // فهرس الابن الأيسر (-1 = فارغ)
    int32_t rb_right;           // فهرس الابن الأيمن
    int32_t rb_parent;          // فهرس الأب
    bool rb_color;              // أحمر = true، أسود = false
};

// === طابور التشغيل لكل نواة ===
struct RunQueue {
    int32_t rb_root;             // جذر شجرة أحمر-أسود
    int32_t rb_leftmost;         // أقل vruntime (المهمة التالية)
    uint32_t nr_running;         // عدد المهام الجاهزة
    uint64_t min_vruntime;       // أقل وقت تشغيل افتراضي
    uint64_t total_weight;       // مجموع أوزان المهام
    uint32_t current_pid;        // المهمة الحالية
};

// === المجدول العادل تماماً ===
class CFScheduler {
public:
    // تهيئة المجدول
    bool initialize(uint32_t num_cores) {
        if (num_cores == 0 || num_cores > MAX_CORES) return false;
        num_cores_ = num_cores;
        task_count_ = 0;

        for (uint32_t i = 0; i < num_cores; i++) {
            run_queues_[i] = {-1, -1, 0, 0, 0, 0};
        }
        return true;
    }

    // إضافة مهمة جديدة إلى المجدول
    bool enqueue_task(uint32_t pid, int8_t nice, uint32_t target_cpu) {
        if (task_count_ >= MAX_TASKS) return false;
        if (target_cpu >= num_cores_) target_cpu = select_least_loaded_cpu();

        auto& entity = tasks_[task_count_];
        entity.pid = pid;
        entity.state = TaskState::Ready;
        entity.nice = nice;
        entity.vruntime = run_queues_[target_cpu].min_vruntime; // البدء من أقل vruntime
        entity.exec_start = 0;
        entity.sum_exec_runtime = 0;
        entity.weight = nice_to_weight(nice);
        entity.cpu_affinity = target_cpu;
        entity.is_active = true;
        entity.rb_left = entity.rb_right = entity.rb_parent = -1;
        entity.rb_color = true; // أحمر مبدئياً

        // إدراج في طابور التشغيل
        auto& rq = run_queues_[target_cpu];
        rq.nr_running++;
        rq.total_weight += entity.weight;
        rb_insert(rq, task_count_);

        task_count_++;
        return true;
    }

    // اختيار المهمة التالية للتشغيل (أقل vruntime)
    int32_t pick_next_task(uint32_t cpu) {
        if (cpu >= num_cores_) return -1;
        auto& rq = run_queues_[cpu];
        return rq.rb_leftmost; // أقصى يسار في الشجرة = أقل vruntime
    }

    // تحديث وقت التشغيل الافتراضي بعد تشغيل مهمة
    void update_vruntime(uint32_t task_idx, uint64_t delta_ns) {
        if (task_idx >= task_count_) return;
        auto& task = tasks_[task_idx];

        // حساب الزيادة في vruntime بناءً على الوزن
        // المهام ذات الوزن الأعلى تتراكم vruntime أبطأ
        uint64_t vruntime_delta = (delta_ns * NICE_0_WEIGHT) / task.weight;
        task.vruntime += vruntime_delta;
        task.sum_exec_runtime += delta_ns;

        // تحديث أقل vruntime في طابور التشغيل
        auto& rq = run_queues_[task.cpu_affinity];
        if (task.vruntime > rq.min_vruntime) {
            rq.min_vruntime = task.vruntime;
        }
    }

    // حساب الشريحة الزمنية لمهمة
    uint64_t calc_time_slice(uint32_t task_idx) const {
        if (task_idx >= task_count_) return MIN_GRANULARITY_NS;
        auto& task = tasks_[task_idx];
        auto& rq = run_queues_[task.cpu_affinity];

        if (rq.nr_running == 0) return TARGET_LATENCY_NS;

        // الشريحة = (وزن المهمة / مجموع الأوزان) × زمن الاستجابة المستهدف
        uint64_t slice = (task.weight * TARGET_LATENCY_NS) / rq.total_weight;
        return (slice < MIN_GRANULARITY_NS) ? MIN_GRANULARITY_NS : slice;
    }

    // موازنة الحمل بين الأنوية
    void load_balance() {
        uint32_t max_cpu = 0, min_cpu = 0;
        uint32_t max_load = 0, min_load = UINT32_MAX;

        // إيجاد النواة الأكثر والأقل حملاً
        for (uint32_t i = 0; i < num_cores_; i++) {
            if (run_queues_[i].nr_running > max_load) {
                max_load = run_queues_[i].nr_running;
                max_cpu = i;
            }
            if (run_queues_[i].nr_running < min_load) {
                min_load = run_queues_[i].nr_running;
                min_cpu = i;
            }
        }

        // نقل مهمة إذا كان الفرق كبيراً
        if (max_load - min_load >= 2) {
            migrate_task(max_cpu, min_cpu);
        }
    }

    uint32_t active_tasks() const { return task_count_; }
    uint32_t core_count() const   { return num_cores_; }

private:
    std::array<SchedEntity, MAX_TASKS> tasks_;
    std::array<RunQueue, MAX_CORES> run_queues_;
    uint32_t task_count_ = 0;
    uint32_t num_cores_ = 1;

    // تحويل قيمة اللطف إلى وزن
    uint64_t nice_to_weight(int8_t nice) {
        // الأوزان تتضاعف/تنصف كل 1 nice تقريباً
        static const uint64_t weights[] = {
            88761, 71755, 56483, 46273, 36291,  // -20 إلى -16
            29154, 23254, 18705, 14949, 11916,  // -15 إلى -11
            9548,  7620,  6100,  4904,  3906,   // -10 إلى -6
            3121,  2501,  1991,  1586,  1277,   // -5  إلى -1
            1024,  820,   655,   526,   423,    //  0  إلى  4
            335,   272,   215,   172,   137,    //  5  إلى  9
            110,   87,    70,    56,    45,     // 10  إلى 14
            36,    29,    23,    18,    15      // 15  إلى 19
        };
        int idx = nice + 20;
        if (idx < 0) idx = 0;
        if (idx > 39) idx = 39;
        return weights[idx];
    }

    uint32_t select_least_loaded_cpu() {
        uint32_t min_cpu = 0;
        uint32_t min_load = UINT32_MAX;
        for (uint32_t i = 0; i < num_cores_; i++) {
            if (run_queues_[i].nr_running < min_load) {
                min_load = run_queues_[i].nr_running;
                min_cpu = i;
            }
        }
        return min_cpu;
    }

    void rb_insert(RunQueue& rq, uint32_t idx) {
        // إدراج بسيط مبدئي (الإصدار الكامل يتطلب إعادة توازن)
        if (rq.rb_root == -1) {
            rq.rb_root = idx;
            rq.rb_leftmost = idx;
        } else if (tasks_[idx].vruntime <= tasks_[rq.rb_leftmost].vruntime) {
            tasks_[rq.rb_leftmost].rb_left = idx;
            tasks_[idx].rb_parent = rq.rb_leftmost;
            rq.rb_leftmost = idx;
        }
    }

    void migrate_task(uint32_t from_cpu, uint32_t to_cpu) {
        // نقل مهمة بين الأنوية (مبسط)
        if (run_queues_[from_cpu].nr_running > 1) {
            run_queues_[from_cpu].nr_running--;
            run_queues_[to_cpu].nr_running++;
        }
    }
};

} // namespace sad::os::kernel::scheduler
