// ===================================================================
// صNet - جدول التجزئة الموزع (DHT) المبني على خوارزمية Kademlia
// المرحلة 53: الشبكة اللامركزية - الطبقة الأساسية
// ===================================================================

#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <chrono>

namespace sad::network {

/// حجم المعرّف في بتّات (160 بت مثل BitTorrent)
constexpr size_t NODE_ID_BITS = 160;
constexpr size_t NODE_ID_BYTES = NODE_ID_BITS / 8;

/// معرّف العُقدة - 160 بت
using NodeId = std::array<uint8_t, NODE_ID_BYTES>;

/// حساب مسافة XOR بين معرّفين
NodeId xor_distance(const NodeId& a, const NodeId& b) {
    NodeId result{};
    for (size_t i = 0; i < NODE_ID_BYTES; i++) {
        result[i] = a[i] ^ b[i];
    }
    return result;
}

/// معلومات عُقدة في الشبكة
struct NodeInfo {
    NodeId id;                 // المعرّف الفريد
    std::string address;       // عنوان IP أو اسم المضيف
    uint16_t port;             // رقم المنفذ
    uint64_t last_seen;        // آخر وقت اتصال (طابع زمني)
    bool is_active;            // هل العُقدة نشطة؟
};

/// دلو k في جدول التوجيه - يحتوي على أقرب العُقد
class KBucket {
    static constexpr size_t K = 20;   // الحد الأقصى لحجم الدلو
    std::vector<NodeInfo> nodes_;      // قائمة العُقد

public:
    /// إضافة عُقدة للدلو
    bool add_node(const NodeInfo& node) {
        // البحث عن العُقدة الموجودة
        for (auto& n : nodes_) {
            if (n.id == node.id) {
                n.last_seen = node.last_seen;
                n.is_active = true;
                return true;   // تحديث فقط
            }
        }
        // إضافة عُقدة جديدة إذا لم يمتلئ الدلو
        if (nodes_.size() < K) {
            nodes_.push_back(node);
            return true;
        }
        return false;  // الدلو ممتلئ
    }

    /// إزالة العُقد غير النشطة
    void evict_stale(uint64_t timeout_ms) {
        auto now = std::chrono::system_clock::now()
            .time_since_epoch().count();
        nodes_.erase(
            std::remove_if(nodes_.begin(), nodes_.end(),
                [&](const NodeInfo& n) {
                    return (now - n.last_seen) > timeout_ms;
                }),
            nodes_.end()
        );
    }

    const std::vector<NodeInfo>& nodes() const { return nodes_; }
    size_t size() const { return nodes_.size(); }
    bool is_full() const { return nodes_.size() >= K; }
};

/// قيمة مُخزّنة في DHT
struct DHTValue {
    std::vector<uint8_t> data;     // البيانات المُخزّنة
    uint64_t timestamp;            // وقت التخزين
    uint32_t ttl_seconds;          // مدة الصلاحية بالثواني
    NodeId publisher;              // العُقدة الناشرة
};

/// جدول التوجيه - 160 دلو حسب مسافة XOR
class RoutingTable {
    NodeId self_id_;                                  // معرّف هذه العُقدة
    std::array<KBucket, NODE_ID_BITS> buckets_;       // أدلّة التوجيه

public:
    explicit RoutingTable(const NodeId& self) : self_id_(self) {}

    /// تحديد الدلو المناسب للعُقدة
    size_t bucket_index(const NodeId& target) const {
        NodeId dist = xor_distance(self_id_, target);
        // إيجاد أعلى بت غير صفري
        for (size_t i = 0; i < NODE_ID_BYTES; i++) {
            if (dist[i] != 0) {
                for (int bit = 7; bit >= 0; bit--) {
                    if (dist[i] & (1 << bit)) {
                        return i * 8 + (7 - bit);
                    }
                }
            }
        }
        return NODE_ID_BITS - 1;
    }

    /// إيجاد أقرب k عُقدة للهدف
    std::vector<NodeInfo> find_closest(const NodeId& target, size_t k = 20) {
        std::vector<NodeInfo> all_nodes;
        for (const auto& bucket : buckets_) {
            for (const auto& node : bucket.nodes()) {
                all_nodes.push_back(node);
            }
        }
        // ترتيب حسب المسافة من الهدف
        std::sort(all_nodes.begin(), all_nodes.end(),
            [&](const NodeInfo& a, const NodeInfo& b) {
                return xor_distance(a.id, target) < xor_distance(b.id, target);
            });
        if (all_nodes.size() > k) all_nodes.resize(k);
        return all_nodes;
    }
};

/// نواة DHT الرئيسية
class DHTCore {
    RoutingTable routing_;                         // جدول التوجيه
    std::map<NodeId, DHTValue> storage_;            // التخزين المحلي
    NodeId self_id_;                                // معرّف هذه العُقدة

public:
    explicit DHTCore(const NodeId& id) : routing_(id), self_id_(id) {}

    /// تخزين قيمة في الشبكة (STORE)
    bool store(const NodeId& key, const std::vector<uint8_t>& data) {
        DHTValue value{data, 0, 3600, self_id_};
        storage_[key] = std::move(value);
        return true;
    }

    /// البحث عن قيمة في الشبكة (FIND_VALUE)
    std::vector<uint8_t> find_value(const NodeId& key) {
        auto it = storage_.find(key);
        if (it != storage_.end()) {
            return it->second.data;
        }
        return {};  // غير موجود محلياً - يجب البحث في العُقد القريبة
    }

    /// الانضمام للشبكة عبر عُقدة بذرية
    void bootstrap(const NodeInfo& seed) {
        // إجراء FIND_NODE على معرّفنا لملء جدول التوجيه
        (void)seed;
    }
};

} // namespace sad::network
