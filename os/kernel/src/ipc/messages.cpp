// ===================================================================
// نظام تمرير الرسائل بين العمليات - صدOS
// messages.cpp - IPC Message Passing
// اتصال قائم على المنافذ (شبيه بـ Mach) مع ذاكرة مشتركة وإشارات
// المرحلة 52 - مشروع لغة صاد
// ===================================================================

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>

namespace sad::os::kernel::ipc {

// === ثوابت نظام الرسائل ===
static constexpr size_t MAX_PORTS           = 1024;    // الحد الأقصى للمنافذ
static constexpr size_t MAX_MSG_SIZE        = 4096;    // الحد الأقصى لحجم الرسالة
static constexpr size_t MSG_QUEUE_CAPACITY  = 64;      // سعة طابور الرسائل لكل منفذ
static constexpr size_t MAX_SHARED_REGIONS  = 256;     // الحد الأقصى لمناطق الذاكرة المشتركة
static constexpr size_t MAX_SIGNALS         = 32;      // عدد الإشارات المدعومة

// === أنواع الرسائل ===
enum class MessageType : uint8_t {
    Data,          // بيانات عادية
    Request,       // طلب (ينتظر رداً)
    Reply,         // رد على طلب
    Notification,  // إشعار (لا ينتظر رداً)
    Signal,        // إشارة
    Control        // رسالة تحكم بالمنفذ
};

// === رأس الرسالة ===
struct MessageHeader {
    MessageType type;
    uint32_t sender_pid;        // معرف العملية المرسلة
    uint32_t sender_port;       // منفذ المرسل
    uint32_t dest_port;         // منفذ الوجهة
    uint32_t msg_id;            // معرف الرسالة (للربط بين الطلب والرد)
    uint32_t payload_size;      // حجم الحمولة بالبايت
    uint64_t timestamp;         // الطابع الزمني
};

// === الرسالة الكاملة ===
struct Message {
    MessageHeader header;
    uint8_t payload[MAX_MSG_SIZE];  // حمولة الرسالة

    bool is_valid() const {
        return header.payload_size <= MAX_MSG_SIZE;
    }
};

// === حالة المنفذ ===
enum class PortState : uint8_t {
    Free,       // حر (غير مستخدم)
    Active,     // نشط
    Blocked,    // محجوب (الطابور ممتلئ)
    Closed      // مغلق
};

// === حقوق المنفذ ===
enum class PortRight : uint8_t {
    Send     = 0x01,  // حق الإرسال
    Receive  = 0x02,  // حق الاستقبال
    SendOnce = 0x04,  // حق إرسال مرة واحدة
    PortSet  = 0x08   // مجموعة منافذ
};

// === طابور الرسائل ===
struct MessageQueue {
    std::array<Message, MSG_QUEUE_CAPACITY> messages;
    uint32_t head = 0;
    uint32_t tail = 0;
    uint32_t count = 0;

    // إضافة رسالة إلى الطابور
    bool enqueue(const Message& msg) {
        if (count >= MSG_QUEUE_CAPACITY) return false;
        messages[tail] = msg;
        tail = (tail + 1) % MSG_QUEUE_CAPACITY;
        count++;
        return true;
    }

    // سحب رسالة من الطابور
    bool dequeue(Message& out) {
        if (count == 0) return false;
        out = messages[head];
        head = (head + 1) % MSG_QUEUE_CAPACITY;
        count--;
        return true;
    }

    bool is_empty() const { return count == 0; }
    bool is_full() const  { return count >= MSG_QUEUE_CAPACITY; }
};

// === المنفذ (Port) - وحدة الاتصال الأساسية ===
struct Port {
    uint32_t port_id;
    uint32_t owner_pid;          // العملية المالكة
    PortState state;
    uint8_t rights;              // حقوق الوصول
    MessageQueue queue;           // طابور الرسائل الواردة
    uint32_t blocked_receiver;    // خيط محجوب ينتظر رسالة
};

// === منطقة الذاكرة المشتركة ===
struct SharedMemoryRegion {
    uint32_t region_id;
    uint64_t physical_base;      // العنوان الفيزيائي الأساسي
    uint64_t size;               // الحجم بالبايت
    uint32_t owner_pid;          // العملية المالكة
    uint32_t ref_count;          // عدد المراجع
    bool writable;               // قابلة للكتابة
    bool is_active;              // هل المنطقة نشطة؟
};

// === معالج الإشارات ===
using SignalHandler = void(*)(int);

struct SignalState {
    uint32_t pending_mask;        // قناع الإشارات المعلقة
    uint32_t blocked_mask;        // قناع الإشارات المحجوبة
    SignalHandler handlers[MAX_SIGNALS]; // معالجات الإشارات
};

// === نظام الاتصال بين العمليات ===
class IPCManager {
public:
    // تهيئة نظام IPC
    bool initialize() {
        port_count_ = 0;
        region_count_ = 0;
        next_port_id_ = 1;
        next_region_id_ = 1;
        next_msg_id_ = 1;

        for (auto& port : ports_) {
            port.state = PortState::Free;
        }
        for (auto& region : shared_regions_) {
            region.is_active = false;
        }
        return true;
    }

    // === إدارة المنافذ ===

    // إنشاء منفذ جديد
    int32_t create_port(uint32_t owner_pid, uint8_t rights) {
        if (port_count_ >= MAX_PORTS) return -1;

        for (auto& port : ports_) {
            if (port.state == PortState::Free) {
                port.port_id = next_port_id_++;
                port.owner_pid = owner_pid;
                port.state = PortState::Active;
                port.rights = rights;
                port.queue = {};
                port.blocked_receiver = UINT32_MAX;
                port_count_++;
                return static_cast<int32_t>(port.port_id);
            }
        }
        return -1;
    }

    // تدمير منفذ
    bool destroy_port(uint32_t port_id, uint32_t caller_pid) {
        auto* port = find_port(port_id);
        if (!port || port->owner_pid != caller_pid) return false;
        port->state = PortState::Closed;
        port_count_--;
        return true;
    }

    // === إرسال واستقبال الرسائل ===

    // إرسال رسالة إلى منفذ وجهة
    bool send_message(uint32_t dest_port_id, uint32_t sender_pid,
                      uint32_t sender_port_id, MessageType type,
                      const void* data, uint32_t size) {
        if (size > MAX_MSG_SIZE) return false;

        auto* dest = find_port(dest_port_id);
        if (!dest || dest->state != PortState::Active) return false;

        Message msg{};
        msg.header.type = type;
        msg.header.sender_pid = sender_pid;
        msg.header.sender_port = sender_port_id;
        msg.header.dest_port = dest_port_id;
        msg.header.msg_id = next_msg_id_++;
        msg.header.payload_size = size;
        msg.header.timestamp = read_timestamp();

        if (data && size > 0) {
            std::memcpy(msg.payload, data, size);
        }

        if (!dest->queue.enqueue(msg)) {
            return false; // الطابور ممتلئ
        }

        // إيقاظ مستقبل محجوب (إن وجد)
        if (dest->blocked_receiver != UINT32_MAX) {
            wake_thread(dest->blocked_receiver);
            dest->blocked_receiver = UINT32_MAX;
        }
        return true;
    }

    // استقبال رسالة من منفذ
    bool receive_message(uint32_t port_id, uint32_t caller_pid,
                         Message& out_msg) {
        auto* port = find_port(port_id);
        if (!port || port->owner_pid != caller_pid) return false;

        if (port->queue.dequeue(out_msg)) {
            return true;
        }

        // لا توجد رسائل - حجب المستقبل
        port->blocked_receiver = caller_pid;
        block_current_thread(caller_pid);
        return false; // سيُستأنف لاحقاً
    }

    // === الذاكرة المشتركة ===

    // إنشاء منطقة ذاكرة مشتركة
    int32_t create_shared_region(uint32_t owner_pid, uint64_t size, bool writable) {
        if (region_count_ >= MAX_SHARED_REGIONS || size == 0) return -1;

        for (auto& region : shared_regions_) {
            if (!region.is_active) {
                region.region_id = next_region_id_++;
                region.physical_base = allocate_physical_memory(size);
                region.size = size;
                region.owner_pid = owner_pid;
                region.ref_count = 1;
                region.writable = writable;
                region.is_active = true;
                region_count_++;
                return static_cast<int32_t>(region.region_id);
            }
        }
        return -1;
    }

    // ربط منطقة مشتركة بفضاء عنوان عملية
    uint64_t map_shared_region(uint32_t region_id, uint32_t target_pid) {
        auto* region = find_region(region_id);
        if (!region) return 0;
        region->ref_count++;
        // إرجاع العنوان الافتراضي المعين
        return 0x00007F8000000000ULL + region_id * 0x10000;
    }

    // فك ربط منطقة مشتركة
    bool unmap_shared_region(uint32_t region_id, uint32_t pid) {
        auto* region = find_region(region_id);
        if (!region) return false;
        region->ref_count--;
        if (region->ref_count == 0) {
            free_physical_memory(region->physical_base, region->size);
            region->is_active = false;
            region_count_--;
        }
        return true;
    }

    // === معالجة الإشارات ===

    // إرسال إشارة لعملية
    bool send_signal(uint32_t target_pid, uint8_t signal_num) {
        if (signal_num >= MAX_SIGNALS) return false;
        // تعليم الإشارة كمعلقة
        auto* state = find_signal_state(target_pid);
        if (!state) return false;
        state->pending_mask |= (1u << signal_num);
        return true;
    }

    // تسجيل معالج إشارة
    bool register_signal_handler(uint32_t pid, uint8_t signal_num, SignalHandler handler) {
        if (signal_num >= MAX_SIGNALS) return false;
        auto* state = find_signal_state(pid);
        if (!state) return false;
        state->handlers[signal_num] = handler;
        return true;
    }

    uint32_t active_ports() const   { return port_count_; }
    uint32_t active_regions() const { return region_count_; }

private:
    std::array<Port, MAX_PORTS> ports_;
    std::array<SharedMemoryRegion, MAX_SHARED_REGIONS> shared_regions_;
    uint32_t port_count_ = 0;
    uint32_t region_count_ = 0;
    uint32_t next_port_id_ = 1;
    uint32_t next_region_id_ = 1;
    uint32_t next_msg_id_ = 1;

    Port* find_port(uint32_t id) {
        for (auto& p : ports_) {
            if (p.state != PortState::Free && p.port_id == id) return &p;
        }
        return nullptr;
    }

    SharedMemoryRegion* find_region(uint32_t id) {
        for (auto& r : shared_regions_) {
            if (r.is_active && r.region_id == id) return &r;
        }
        return nullptr;
    }

    SignalState* find_signal_state(uint32_t) { return nullptr; /* من جدول العمليات */ }

    uint64_t read_timestamp()                          { return 0; }
    uint64_t allocate_physical_memory(uint64_t)        { return 0; }
    void free_physical_memory(uint64_t, uint64_t)      {}
    void block_current_thread(uint32_t)                {}
    void wake_thread(uint32_t)                         {}
};

} // namespace sad::os::kernel::ipc
