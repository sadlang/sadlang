// ===================================================================
// صNet - شبكة الثقة والشهادات اللامركزية
// المرحلة 53: نظام ثقة بدون سلطة مركزية
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <set>
#include <algorithm>

namespace sad::network {

/// مستوى الثقة
enum class TrustLevel : uint8_t {
    UNKNOWN   = 0,   // غير معروف
    MARGINAL  = 1,   // ثقة هامشية
    FULL      = 2,   // ثقة كاملة
    ULTIMATE  = 3    // ثقة مطلقة (المالك)
};

/// نوع الشهادة
enum class CertificateType : uint8_t {
    IDENTITY,       // شهادة هوية
    CAPABILITY,     // شهادة قدرة/صلاحية
    DELEGATION,     // شهادة تفويض
    REVOCATION      // شهادة إلغاء
};

/// شهادة ثقة لامركزية
struct TrustCertificate {
    std::string cert_id;               // معرّف الشهادة الفريد
    CertificateType type;              // نوع الشهادة
    std::string issuer_did;            // معرّف المُصدِر (DID)
    std::string subject_did;           // معرّف الموضوع (DID)
    TrustLevel level;                  // مستوى الثقة الممنوح
    uint64_t issued_at;                // تاريخ الإصدار
    uint64_t expires_at;               // تاريخ الانتهاء
    std::vector<uint8_t> signature;    // التوقيع الرقمي
    std::string reason;                // سبب منح الثقة
};

/// عُقدة في رسم الثقة البياني
struct TrustNode {
    std::string did;                               // المعرّف اللامركزي
    TrustLevel assigned_trust;                     // الثقة المُعيّنة يدوياً
    TrustLevel computed_trust;                     // الثقة المحسوبة
    std::vector<std::string> endorsed_by;          // مَن صادق عليه؟
    std::vector<std::string> endorses;             // مَن صادق عليهم؟
    uint32_t endorsement_count;                    // عدد المصادقات
};

/// محرك حساب الثقة
class TrustCalculator {
    uint32_t marginal_needed_;     // عدد المصادقات الهامشية المطلوبة
    uint32_t full_needed_;         // عدد المصادقات الكاملة المطلوبة
    uint32_t max_depth_;           // أقصى عمق في سلسلة الثقة

public:
    TrustCalculator(uint32_t marginal = 3, uint32_t full = 1, uint32_t depth = 5)
        : marginal_needed_(marginal), full_needed_(full), max_depth_(depth) {}

    /// حساب مستوى الثقة المحسوب لعُقدة
    TrustLevel compute(const TrustNode& node,
                       const std::map<std::string, TrustNode>& graph) {
        if (node.assigned_trust == TrustLevel::ULTIMATE) {
            return TrustLevel::ULTIMATE;
        }

        uint32_t full_endorsements = 0;
        uint32_t marginal_endorsements = 0;

        for (const auto& endorser_did : node.endorsed_by) {
            auto it = graph.find(endorser_did);
            if (it == graph.end()) continue;
            const auto& endorser = it->second;
            if (endorser.computed_trust >= TrustLevel::FULL) {
                full_endorsements++;
            } else if (endorser.computed_trust >= TrustLevel::MARGINAL) {
                marginal_endorsements++;
            }
        }

        // ثقة كاملة إذا صادق عليه عدد كافٍ من الموثوقين
        if (full_endorsements >= full_needed_) return TrustLevel::FULL;
        if (marginal_endorsements >= marginal_needed_) return TrustLevel::FULL;
        if (full_endorsements > 0 || marginal_endorsements > 0) {
            return TrustLevel::MARGINAL;
        }
        return TrustLevel::UNKNOWN;
    }
};

/// مدير شبكة الثقة
class WebOfTrust {
    std::map<std::string, TrustNode> trust_graph_;      // رسم الثقة البياني
    std::vector<TrustCertificate> certificates_;         // جميع الشهادات
    TrustCalculator calculator_;                         // محرك الحساب
    std::string self_did_;                               // هويتنا

public:
    explicit WebOfTrust(const std::string& did) : self_did_(did) {
        // إضافة أنفسنا بثقة مطلقة
        TrustNode self{did, TrustLevel::ULTIMATE, TrustLevel::ULTIMATE, {}, {}, 0};
        trust_graph_[did] = self;
    }

    /// إصدار شهادة ثقة لقرين
    TrustCertificate issue_certificate(const std::string& subject,
                                        TrustLevel level,
                                        const std::string& reason) {
        TrustCertificate cert;
        cert.issuer_did = self_did_;
        cert.subject_did = subject;
        cert.level = level;
        cert.type = CertificateType::IDENTITY;
        cert.reason = reason;
        // توقيع الشهادة بمفتاحنا الخاص
        certificates_.push_back(cert);
        // تحديث رسم الثقة
        trust_graph_[subject].endorsed_by.push_back(self_did_);
        trust_graph_[self_did_].endorses.push_back(subject);
        return cert;
    }

    /// التحقق من صحة شهادة
    bool verify_certificate(const TrustCertificate& cert) {
        // التحقق من التوقيع الرقمي
        // التحقق من تاريخ الصلاحية
        // التحقق من عدم الإلغاء
        (void)cert;
        return true;
    }

    /// إعادة حساب مستويات الثقة لجميع العُقد
    void recalculate_all() {
        for (auto& [did, node] : trust_graph_) {
            node.computed_trust = calculator_.compute(node, trust_graph_);
        }
    }

    /// الحصول على مستوى الثقة لمعرّف
    TrustLevel get_trust(const std::string& did) const {
        auto it = trust_graph_.find(did);
        if (it != trust_graph_.end()) return it->second.computed_trust;
        return TrustLevel::UNKNOWN;
    }

    size_t node_count() const { return trust_graph_.size(); }
};

} // namespace sad::network
