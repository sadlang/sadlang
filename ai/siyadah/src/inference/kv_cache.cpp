// ملف: kv_cache.cpp
// الوصف: ذاكرة التخزين المؤقت KV مع كتل مقسّمة وتجمع ذاكرة وسياسة إخلاء
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <unordered_map>
#include <deque>
#include <memory>
#include <cassert>
#include <algorithm>
#include <cstdint>

namespace sad::ai {

constexpr size_t DEFAULT_BLOCK_SIZE = 16;
constexpr size_t DEFAULT_POOL_BLOCKS = 256;

enum class EvictionPolicy { LRU, LFU, FIFO };

struct KVBlock {
    int block_id;
    size_t block_size;
    std::vector<float> keys;
    std::vector<float> values;
    int ref_count;
    uint64_t last_access;
    uint64_t access_count;
    bool occupied;

    KVBlock(int id, size_t bsize, size_t head_dim)
        : block_id(id), block_size(bsize), ref_count(0),
          last_access(0), access_count(0), occupied(false) {
        keys.resize(bsize * head_dim, 0.0f);
        values.resize(bsize * head_dim, 0.0f);
    }
};

class MemoryPool {
    std::vector<std::unique_ptr<KVBlock>> blocks_;
    std::deque<int> free_list_;
    size_t block_size_;
    size_t head_dim_;

public:
    MemoryPool(size_t num_blocks, size_t block_size, size_t head_dim)
        : block_size_(block_size), head_dim_(head_dim) {
        for (size_t i = 0; i < num_blocks; ++i) {
            blocks_.push_back(std::make_unique<KVBlock>(static_cast<int>(i), block_size, head_dim));
            free_list_.push_back(static_cast<int>(i));
        }
    }

    // تخصيص كتلة من التجمع
    KVBlock* allocate() {
        if (free_list_.empty()) return nullptr;
        int id = free_list_.front();
        free_list_.pop_front();
        blocks_[id]->occupied = true;
        blocks_[id]->ref_count = 1;
        return blocks_[id].get();
    }

    // إرجاع كتلة إلى التجمع
    void deallocate(int block_id) {
        assert(block_id >= 0 && block_id < static_cast<int>(blocks_.size()));
        blocks_[block_id]->occupied = false;
        blocks_[block_id]->ref_count = 0;
        free_list_.push_back(block_id);
    }

    size_t free_blocks() const { return free_list_.size(); }
    size_t total_blocks() const { return blocks_.size(); }
    KVBlock* get_block(int id) { return blocks_[id].get(); }
};

class KVCache {
    std::unique_ptr<MemoryPool> pool_;
    std::unordered_map<int, std::vector<int>> sequence_blocks_;
    EvictionPolicy policy_;
    size_t head_dim_;
    size_t block_size_;
    uint64_t global_timestamp_;

public:
    KVCache(size_t num_blocks = DEFAULT_POOL_BLOCKS, size_t block_size = DEFAULT_BLOCK_SIZE,
            size_t head_dim = 64, EvictionPolicy policy = EvictionPolicy::LRU)
        : policy_(policy), head_dim_(head_dim), block_size_(block_size), global_timestamp_(0) {
        pool_ = std::make_unique<MemoryPool>(num_blocks, block_size, head_dim);
    }

    // تخصيص كتل لتسلسل جديد
    bool allocate_sequence(int seq_id, size_t num_tokens) {
        size_t blocks_needed = (num_tokens + block_size_ - 1) / block_size_;
        std::vector<int> block_ids;
        for (size_t i = 0; i < blocks_needed; ++i) {
            auto* block = pool_->allocate();
            if (!block) {
                if (!evict_one()) { return false; }
                block = pool_->allocate();
                if (!block) return false;
            }
            block_ids.push_back(block->block_id);
        }
        sequence_blocks_[seq_id] = block_ids;
        return true;
    }

    // الوصول إلى ذاكرة التخزين المؤقت لتسلسل
    std::vector<float> get_cached_keys(int seq_id) {
        std::vector<float> all_keys;
        if (sequence_blocks_.count(seq_id) == 0) return all_keys;
        for (int bid : sequence_blocks_[seq_id]) {
            auto* block = pool_->get_block(bid);
            block->last_access = ++global_timestamp_;
            block->access_count++;
            all_keys.insert(all_keys.end(), block->keys.begin(), block->keys.end());
        }
        return all_keys;
    }

    // إخلاء كتلة واحدة بناءً على السياسة
    bool evict_one() {
        int victim_seq = -1;
        uint64_t best_score = UINT64_MAX;
        for (auto& [seq_id, blocks] : sequence_blocks_) {
            if (blocks.empty()) continue;
            auto* block = pool_->get_block(blocks.back());
            uint64_t score = (policy_ == EvictionPolicy::LRU) ? block->last_access :
                             (policy_ == EvictionPolicy::LFU) ? block->access_count :
                             block->last_access;
            if (score < best_score) { best_score = score; victim_seq = seq_id; }
        }
        if (victim_seq < 0) return false;
        auto& blocks = sequence_blocks_[victim_seq];
        pool_->deallocate(blocks.back());
        blocks.pop_back();
        if (blocks.empty()) sequence_blocks_.erase(victim_seq);
        return true;
    }

    // تحرير جميع كتل تسلسل
    void free_sequence(int seq_id) {
        if (sequence_blocks_.count(seq_id) == 0) return;
        for (int bid : sequence_blocks_[seq_id]) pool_->deallocate(bid);
        sequence_blocks_.erase(seq_id);
    }

    size_t free_blocks() const { return pool_->free_blocks(); }
    size_t active_sequences() const { return sequence_blocks_.size(); }
    size_t memory_usage_bytes() const {
        return (pool_->total_blocks() - pool_->free_blocks()) * block_size_ * head_dim_ * 2 * sizeof(float);
    }
};

} // namespace sad::ai
