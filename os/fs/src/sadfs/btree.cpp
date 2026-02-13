// ===================================================================
// صدFS - شجرة B+ للفهرسة السريعة
// المرحلة 52: بنية بيانات الفهرسة لنظام الملفات
// ===================================================================

#include <cstdint>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>

namespace sad::os::fs {

/// الدرجة الافتراضية لشجرة B+ (أقصى عدد مفاتيح في العقدة)
constexpr int BTREE_ORDER = 128;

/// زوج مفتاح-قيمة في شجرة B+
struct BTreeEntry {
    uint64_t key;     // المفتاح (معرّف الملف أو رقم الكتلة)
    uint64_t value;   // القيمة (عنوان الكتلة على القرص)
};

/// عقدة في شجرة B+
struct BTreeNode {
    bool is_leaf;                              // هل العقدة ورقية؟
    std::vector<uint64_t> keys;                // المفاتيح المرتبة
    std::vector<uint64_t> values;              // القيم (للأوراق فقط)
    std::vector<std::unique_ptr<BTreeNode>> children; // الأبناء (للعقد الداخلية)
    BTreeNode* next_leaf = nullptr;            // مؤشر الورقة التالية (للنطاقات)

    explicit BTreeNode(bool leaf) : is_leaf(leaf) {}
};

// === شجرة B+ ===

/// شجرة B+ - فهرسة سريعة للبحث والنطاقات
class BPlusTree {
public:
    explicit BPlusTree(int order = BTREE_ORDER) : order_(order) {
        root_ = std::make_unique<BTreeNode>(true);
    }

    /// إدراج زوج مفتاح-قيمة في الشجرة
    void insert(uint64_t key, uint64_t value) {
        if (root_->keys.size() >= static_cast<size_t>(order_ - 1)) {
            // الجذر ممتلئ - نقسمه وننشئ جذراً جديداً
            auto new_root = std::make_unique<BTreeNode>(false);
            new_root->children.push_back(std::move(root_));
            split_child(new_root.get(), 0);
            root_ = std::move(new_root);
        }
        insert_non_full(root_.get(), key, value);
        size_++;
    }

    /// البحث عن قيمة بمفتاحها
    std::optional<uint64_t> search(uint64_t key) const {
        return search_node(root_.get(), key);
    }

    /// حذف مفتاح من الشجرة
    bool remove(uint64_t key) {
        if (!root_) return false;
        bool removed = remove_from_node(root_.get(), key);
        if (removed) size_--;
        // إذا أصبح الجذر فارغاً ولديه ابن واحد، نرفع الابن
        if (!root_->is_leaf && root_->keys.empty() && !root_->children.empty()) {
            root_ = std::move(root_->children[0]);
        }
        return removed;
    }

    /// استعلام نطاق - إرجاع جميع القيم بين مفتاحين
    std::vector<BTreeEntry> range_query(uint64_t low, uint64_t high) const {
        std::vector<BTreeEntry> results;
        auto* leaf = find_leaf(root_.get(), low);
        while (leaf) {
            for (size_t i = 0; i < leaf->keys.size(); ++i) {
                if (leaf->keys[i] >= low && leaf->keys[i] <= high)
                    results.push_back({leaf->keys[i], leaf->values[i]});
                if (leaf->keys[i] > high) return results;
            }
            leaf = leaf->next_leaf;  // الانتقال للورقة التالية
        }
        return results;
    }

    /// تحميل جماعي من بيانات مرتبة مسبقاً
    void bulk_load(std::vector<BTreeEntry>& entries) {
        std::sort(entries.begin(), entries.end(),
                  [](const auto& a, const auto& b) { return a.key < b.key; });
        root_ = std::make_unique<BTreeNode>(true);
        size_ = 0;
        for (auto& e : entries) {
            insert(e.key, e.value);
        }
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

private:
    std::unique_ptr<BTreeNode> root_;
    int order_;
    size_t size_ = 0;

    /// إدراج في عقدة غير ممتلئة
    void insert_non_full(BTreeNode* node, uint64_t key, uint64_t value) {
        if (node->is_leaf) {
            // إدراج في الورقة بالترتيب الصحيح
            auto pos = std::lower_bound(node->keys.begin(), node->keys.end(), key);
            auto idx = pos - node->keys.begin();
            node->keys.insert(pos, key);
            node->values.insert(node->values.begin() + idx, value);
        } else {
            // إيجاد الابن المناسب والنزول إليه
            auto pos = std::upper_bound(node->keys.begin(), node->keys.end(), key);
            int idx = static_cast<int>(pos - node->keys.begin());
            if (node->children[idx]->keys.size() >= static_cast<size_t>(order_ - 1)) {
                split_child(node, idx);
                if (key > node->keys[idx]) idx++;
            }
            insert_non_full(node->children[idx].get(), key, value);
        }
    }

    /// تقسيم ابن ممتلئ إلى عقدتين
    void split_child(BTreeNode* parent, int index) {
        auto* child = parent->children[index].get();
        auto new_node = std::make_unique<BTreeNode>(child->is_leaf);
        int mid = static_cast<int>(child->keys.size()) / 2;
        uint64_t mid_key = child->keys[mid];

        new_node->keys.assign(child->keys.begin() + mid + (child->is_leaf ? 0 : 1),
                              child->keys.end());
        child->keys.resize(mid);

        if (child->is_leaf) {
            new_node->values.assign(child->values.begin() + mid, child->values.end());
            child->values.resize(mid);
            new_node->next_leaf = child->next_leaf;
            child->next_leaf = new_node.get();
        } else if (!child->children.empty()) {
            for (size_t i = mid + 1; i < child->children.size(); ++i)
                new_node->children.push_back(std::move(child->children[i]));
            child->children.resize(mid + 1);
        }

        parent->keys.insert(parent->keys.begin() + index, mid_key);
        parent->children.insert(parent->children.begin() + index + 1, std::move(new_node));
    }

    /// البحث في عقدة وأبنائها
    std::optional<uint64_t> search_node(const BTreeNode* node, uint64_t key) const {
        if (!node) return std::nullopt;
        auto pos = std::lower_bound(node->keys.begin(), node->keys.end(), key);
        int idx = static_cast<int>(pos - node->keys.begin());
        if (node->is_leaf) {
            if (pos != node->keys.end() && *pos == key)
                return node->values[idx];
            return std::nullopt;
        }
        if (pos != node->keys.end() && *pos == key) idx++;
        return search_node(node->children[idx].get(), key);
    }

    /// إيجاد الورقة التي تحتوي أو ستحتوي المفتاح
    const BTreeNode* find_leaf(const BTreeNode* node, uint64_t key) const {
        if (!node) return nullptr;
        if (node->is_leaf) return node;
        auto pos = std::upper_bound(node->keys.begin(), node->keys.end(), key);
        int idx = static_cast<int>(pos - node->keys.begin());
        return find_leaf(node->children[idx].get(), key);
    }

    /// حذف مفتاح من عقدة (تنفيذ مبسط)
    bool remove_from_node(BTreeNode* node, uint64_t key) {
        if (!node) return false;
        auto pos = std::lower_bound(node->keys.begin(), node->keys.end(), key);
        int idx = static_cast<int>(pos - node->keys.begin());
        if (node->is_leaf) {
            if (pos != node->keys.end() && *pos == key) {
                node->keys.erase(pos);
                node->values.erase(node->values.begin() + idx);
                return true;
            }
            return false;
        }
        if (pos != node->keys.end() && *pos == key) idx++;
        return remove_from_node(node->children[idx].get(), key);
    }
};

} // namespace sad::os::fs
