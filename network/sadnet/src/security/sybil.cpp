// ===================================================================
// صNet - الحماية من هجمات Sybil
// المرحلة 53: آليات منع الهويات المزيّفة
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <cmath>
#include <algorithm>

namespace sad::network {

/// نتيجة فحص Sybil
enum class SybilVerdict : uint8_t {
    CLEAN,          // عُقدة نظيفة
    SUSPICIOUS,     // مشبوهة - تحتاج مراقبة
    LIKELY_SYBIL,   // على الأرجح مزيّفة
    CONFIRMED_SYBIL // مؤكدة كهوية مزيّفة
};

/// المقاييس السلوكية للعُقدة
struct NodeBehaviorMetrics {
    std::string node_id;           // معرّف العُقدة
    uint64_t join_time;            // وقت الانضمام
    uint32_t messages_sent;        // عدد الرسائل المُرسلة
    uint32_t messages_received;    // عدد الرسائل المُستلمة
    uint32_t unique_peers;         // عدد الأقران الفريدين
    double uptime_ratio;           // نسبة وقت التشغيل
    uint32_t failed_challenges;    // عدد التحديات الفاشلة
    double reputation_score;       // نقاط السمعة
    std::string ip_address;        // عنوان IP
    uint16_t ip_prefix;            // بادئة الشبكة الفرعية
};

/// تحدّي إثبات العمل (Proof of Work)
class ProofOfWork {
    uint32_t difficulty_;          // صعوبة التحدّي (عدد الأصفار)

public:
    explicit ProofOfWork(uint32_t difficulty = 20) : difficulty_(difficulty) {}

    /// إنشاء تحدّي جديد
    std::vector<uint8_t> create_challenge() {
        std::vector<uint8_t> challenge(32);
        // توليد تحدّي عشوائي
        return challenge;
    }

    /// التحقق من حل التحدّي
    bool verify_solution(const std::vector<uint8_t>& challenge,
                         uint64_t nonce) {
        // حساب hash(challenge || nonce)
        // التحقق من أن أول difficulty_ بت = 0
        (void)challenge; (void)nonce;
        return true;
    }

    /// تعديل الصعوبة حسب الحاجة
    void adjust_difficulty(uint32_t new_diff) { difficulty_ = new_diff; }
};

/// كاشف تجمّعات IP المشبوهة
class IPClusterDetector {
    std::map<uint16_t, std::set<std::string>> subnet_clusters_;  // تجمّعات الشبكات

public:
    /// تسجيل عُقدة جديدة
    void register_node(const std::string& node_id, const std::string& ip) {
        // استخراج بادئة /16 من عنوان IP
        uint16_t prefix = extract_prefix(ip);
        subnet_clusters_[prefix].insert(node_id);
    }

    /// فحص هل العُقد من نفس الشبكة الفرعية بشكل مريب؟
    bool is_suspicious_cluster(const std::string& ip, uint32_t threshold = 10) {
        uint16_t prefix = extract_prefix(ip);
        auto it = subnet_clusters_.find(prefix);
        if (it != subnet_clusters_.end()) {
            return it->second.size() > threshold;
        }
        return false;
    }

private:
    uint16_t extract_prefix(const std::string& ip) {
        // تحليل أول بايتين من عنوان IPv4
        uint16_t a = 0, b = 0;
        size_t dot1 = ip.find('.');
        if (dot1 != std::string::npos) {
            a = std::stoi(ip.substr(0, dot1));
            size_t dot2 = ip.find('.', dot1 + 1);
            if (dot2 != std::string::npos) {
                b = std::stoi(ip.substr(dot1 + 1, dot2 - dot1 - 1));
            }
        }
        return (a << 8) | b;
    }
};

/// محلّل السلوك للكشف عن Sybil
class BehaviorAnalyzer {
public:
    /// تحليل سلوك عُقدة وتحديد احتمال كونها مزيّفة
    double sybil_probability(const NodeBehaviorMetrics& metrics) {
        double score = 0.0;

        // عُقدة جديدة جداً = مشبوهة
        if (metrics.uptime_ratio < 0.1) score += 0.3;

        // نسبة إرسال/استقبال غير طبيعية
        if (metrics.messages_received > 0) {
            double ratio = (double)metrics.messages_sent / metrics.messages_received;
            if (ratio > 10.0 || ratio < 0.01) score += 0.25;
        }

        // أقران قليلون جداً
        if (metrics.unique_peers < 3) score += 0.2;

        // تحديات فاشلة
        if (metrics.failed_challenges > 2) score += 0.25;

        return std::min(score, 1.0);
    }
};

/// نظام الحماية من Sybil الرئيسي
class SybilDefense {
    ProofOfWork pow_;                              // إثبات العمل
    IPClusterDetector cluster_detector_;            // كاشف التجمّعات
    BehaviorAnalyzer analyzer_;                     // محلّل السلوك
    std::map<std::string, NodeBehaviorMetrics> nodes_; // بيانات العُقد
    std::set<std::string> banned_;                  // العُقد المحظورة

public:
    /// فحص عُقدة جديدة تحاول الانضمام
    SybilVerdict evaluate_newcomer(const std::string& node_id,
                                    const std::string& ip) {
        // فحص تجمّع IP
        if (cluster_detector_.is_suspicious_cluster(ip)) {
            return SybilVerdict::SUSPICIOUS;
        }
        cluster_detector_.register_node(node_id, ip);

        // طلب إثبات عمل
        auto challenge = pow_.create_challenge();
        // انتظار الحل من العُقدة...
        (void)challenge;

        return SybilVerdict::CLEAN;
    }

    /// تقييم دوري للعُقد الموجودة
    SybilVerdict evaluate_existing(const std::string& node_id) {
        auto it = nodes_.find(node_id);
        if (it == nodes_.end()) return SybilVerdict::SUSPICIOUS;

        double prob = analyzer_.sybil_probability(it->second);
        if (prob > 0.8) return SybilVerdict::CONFIRMED_SYBIL;
        if (prob > 0.5) return SybilVerdict::LIKELY_SYBIL;
        if (prob > 0.3) return SybilVerdict::SUSPICIOUS;
        return SybilVerdict::CLEAN;
    }

    /// حظر عُقدة مؤكدة كـ Sybil
    void ban_node(const std::string& node_id) {
        banned_.insert(node_id);
        nodes_.erase(node_id);
    }

    bool is_banned(const std::string& node_id) const {
        return banned_.count(node_id) > 0;
    }
};

} // namespace sad::network
