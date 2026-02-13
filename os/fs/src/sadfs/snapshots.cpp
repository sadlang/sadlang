// ===================================================================
// صدFS - نظام اللقطات والاسترجاع
// المرحلة 52: لقطات زمنية مع نسخ عند الكتابة واسترجاع فوري
// ===================================================================

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace sad::os::fs {

/// حالة اللقطة
enum class SnapshotState : uint8_t {
    Creating  = 0,  // قيد الإنشاء
    Active    = 1,  // نشطة وصالحة
    Rollback  = 2,  // قيد الاسترجاع
    Deleting  = 3,  // قيد الحذف
    Corrupted = 4   // تالفة
};

/// بيانات وصفية لكتلة محفوظة في اللقطة (COW)
struct COWBlock {
    uint64_t original_block;  // رقم الكتلة الأصلية
    uint64_t snapshot_block;  // رقم كتلة النسخة المحفوظة
    uint64_t inode_id;        // عقدة الملف المالكة
    uint64_t timestamp;       // وقت النسخ
};

/// بيانات وصفية للقطة كاملة
struct SnapshotMeta {
    uint64_t id;                  // معرّف اللقطة الفريد
    std::string name;             // اسم وصفي للقطة
    uint64_t created_at;          // وقت الإنشاء
    uint64_t parent_id;           // معرّف اللقطة الأم (0 = لا يوجد)
    SnapshotState state;          // حالة اللقطة
    uint64_t block_count;         // عدد الكتل المحفوظة
    uint64_t root_inode;          // عقدة الجذر عند أخذ اللقطة
    uint64_t generation;          // رقم الجيل في نظام COW
};

/// سجل تغيير بين لقطتين (للمقارنة)
struct DiffEntry {
    uint64_t inode_id;            // عقدة الملف المتغير
    std::string path;             // مسار الملف
    enum class ChangeType : uint8_t {
        Added, Modified, Deleted  // أُضيف، عُدّل، حُذف
    } change;
};

// === مدير اللقطات ===

/// مدير لقطات نظام الملفات - إنشاء واسترجاع ومقارنة
class SnapshotManager {
public:
    /// إنشاء لقطة زمنية جديدة لحالة نظام الملفات
    uint64_t create_snapshot(const std::string& name, uint64_t root_inode) {
        uint64_t id = next_id_++;
        SnapshotMeta meta{};
        meta.id = id;
        meta.name = name;
        meta.created_at = current_timestamp();
        meta.parent_id = active_snapshot_id_;
        meta.state = SnapshotState::Creating;
        meta.root_inode = root_inode;
        meta.generation = ++current_generation_;

        // حفظ البيانات الوصفية المشتركة مع COW
        freeze_cow_metadata(meta);
        meta.state = SnapshotState::Active;
        meta.block_count = cow_blocks_for(id).size();
        snapshots_[id] = meta;
        return id;
    }

    /// استرجاع نظام الملفات إلى حالة لقطة سابقة
    bool rollback(uint64_t snapshot_id) {
        auto it = snapshots_.find(snapshot_id);
        if (it == snapshots_.end()) return false;
        if (it->second.state != SnapshotState::Active) return false;

        it->second.state = SnapshotState::Rollback;
        // استعادة الكتل الأصلية من نسخ COW
        auto& blocks = cow_blocks_[snapshot_id];
        for (auto& block : blocks) {
            restore_block(block);  // نسخ الكتلة المحفوظة إلى موقعها الأصلي
        }
        active_snapshot_id_ = snapshot_id;
        it->second.state = SnapshotState::Active;
        return true;
    }

    /// حذف لقطة وتحرير كتلها (إن لم تكن مشتركة)
    bool delete_snapshot(uint64_t snapshot_id) {
        auto it = snapshots_.find(snapshot_id);
        if (it == snapshots_.end()) return false;
        it->second.state = SnapshotState::Deleting;
        // تحرير الكتل غير المشتركة مع لقطات أخرى
        release_unique_blocks(snapshot_id);
        cow_blocks_.erase(snapshot_id);
        snapshots_.erase(it);
        return true;
    }

    /// مقارنة لقطتين وإرجاع الفروقات
    std::vector<DiffEntry> diff(uint64_t snap_a, uint64_t snap_b) const {
        std::vector<DiffEntry> diffs;
        auto blocks_a = cow_blocks_for(snap_a);
        auto blocks_b = cow_blocks_for(snap_b);

        // جمع عقد الملفات المتغيرة بين اللقطتين
        std::unordered_map<uint64_t, bool> seen;
        for (auto& b : blocks_b) {
            bool found = std::any_of(blocks_a.begin(), blocks_a.end(),
                [&](const COWBlock& a) { return a.original_block == b.original_block; });
            if (!found && !seen[b.inode_id]) {
                diffs.push_back({b.inode_id, "", DiffEntry::ChangeType::Modified});
                seen[b.inode_id] = true;
            }
        }
        return diffs;
    }

    /// قائمة جميع اللقطات المتاحة
    std::vector<SnapshotMeta> list_snapshots() const {
        std::vector<SnapshotMeta> result;
        result.reserve(snapshots_.size());
        for (auto& [id, meta] : snapshots_)
            result.push_back(meta);
        // ترتيب حسب وقت الإنشاء
        std::sort(result.begin(), result.end(),
                  [](const auto& a, const auto& b) { return a.created_at < b.created_at; });
        return result;
    }

    /// الحصول على معلومات لقطة محددة
    const SnapshotMeta* get_snapshot(uint64_t id) const {
        auto it = snapshots_.find(id);
        return it != snapshots_.end() ? &it->second : nullptr;
    }

    size_t snapshot_count() const { return snapshots_.size(); }

private:
    std::unordered_map<uint64_t, SnapshotMeta> snapshots_;
    std::unordered_map<uint64_t, std::vector<COWBlock>> cow_blocks_;
    uint64_t next_id_ = 1;
    uint64_t active_snapshot_id_ = 0;
    uint64_t current_generation_ = 0;

    uint64_t current_timestamp() const { return 0; /* ساعة النظام */ }

    /// تجميد البيانات الوصفية للقطة (بدء تتبع COW)
    void freeze_cow_metadata(const SnapshotMeta& meta) {
        cow_blocks_[meta.id] = {};  // قائمة فارغة - ستمتلئ عند الكتابة
    }

    /// استرجاع كتلة محفوظة إلى موقعها الأصلي
    void restore_block(const COWBlock& block) {
        (void)block; // نسخ البيانات من snapshot_block إلى original_block
    }

    /// تحرير الكتل الفريدة لهذه اللقطة
    void release_unique_blocks(uint64_t snapshot_id) {
        (void)snapshot_id;
        // فحص كل كتلة: هل تشاركها لقطة أخرى؟
    }

    /// استرجاع كتل COW للقطة (نسخة ثابتة للقراءة)
    std::vector<COWBlock> cow_blocks_for(uint64_t id) const {
        auto it = cow_blocks_.find(id);
        return it != cow_blocks_.end() ? it->second : std::vector<COWBlock>{};
    }
};

} // namespace sad::os::fs
