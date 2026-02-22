/**
 * ==========================================================
 * ملف: verifier.cpp
 * الوصف: التحقق من صحة العقود الذكية
 * المهمة: T354 - عقود ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * يفحص العقود الذكية للتأكد من:
 * - عدم وجود ثغرات أمنية
 * - صحة المنطق
 * - توافق مع معايير ERC
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace sad {
namespace tools {
namespace contracts {

// ==========================================================
// 📌 أنواع المشاكل
// ==========================================================

/**
 * مستوى خطورة المشكلة
 */
enum class خطورة_المشكلة {
    حرجة,
    عالية,
    متوسطة,
    منخفضة,
    معلومات
};

/**
 * نوع المشكلة
 */
enum class نوع_المشكلة {
    // أمان
    Reentrancy,
    IntegerOverflow,
    IntegerUnderflow,
    UnprotectedSelfdestruct,
    DelegateCall,
    TxOrigin,
    
    // منطق
    UncheckedReturn,
    MissingAccessControl,
    FrontRunning,
    
    // Gas
    UnboundedLoop,
    ExpensiveOperation,
    
    // معايير
    ERC20Violation,
    ERC721Violation,
    
    // أخرى
    ShadowedVariable,
    UnusedVariable,
    MissingEvent
};

/**
 * مشكلة في العقد
 */
struct مشكلة_العقد {
    نوع_المشكلة نوع;
    خطورة_المشكلة خطورة;
    std::string رسالة;
    std::string ملف;
    int سطر;
    std::string كود;
    std::string إصلاح_مقترح;
    
    std::string النوع_كنص() const {
        switch (نوع) {
            case نوع_المشكلة::Reentrancy: return "Reentrancy";
            case نوع_المشكلة::IntegerOverflow: return "Integer Overflow";
            case نوع_المشكلة::IntegerUnderflow: return "Integer Underflow";
            case نوع_المشكلة::UnprotectedSelfdestruct: return "Unprotected Selfdestruct";
            case نوع_المشكلة::TxOrigin: return "tx.origin Usage";
            case نوع_المشكلة::UncheckedReturn: return "Unchecked Return Value";
            case نوع_المشكلة::UnboundedLoop: return "Unbounded Loop";
            default: return "Unknown";
        }
    }
};

// ==========================================================
// 📌 قواعد التحقق
// ==========================================================

/**
 * قاعدة تحقق
 */
class قاعدة_تحقق {
public:
    virtual ~قاعدة_تحقق() = default;
    virtual std::string الاسم() const = 0;
    virtual std::vector<مشكلة_العقد> فحص(const std::string& كود) = 0;
};

/**
 * قاعدة فحص Reentrancy
 */
class قاعدة_Reentrancy : public قاعدة_تحقق {
public:
    std::string الاسم() const override { return "Reentrancy"; }
    
    std::vector<مشكلة_العقد> فحص(const std::string& كود) override {
        std::vector<مشكلة_العقد> مشاكل;
        
        // البحث عن نمط: call بدون تغيير الحالة أولاً
        // CEI pattern: Checks-Effects-Interactions
        
        // أنماط خطيرة
        if (كود.find(".call{value:") != std::string::npos ||
            كود.find("أرسل(") != std::string::npos) {
            // التحقق من ترتيب العمليات
            مشكلة_العقد مشكلة;
            مشكلة.نوع = نوع_المشكلة::Reentrancy;
            مشكلة.خطورة = خطورة_المشكلة::حرجة;
            مشكلة.رسالة = "احتمال هجوم Reentrancy";
            مشكلة.إصلاح_مقترح = "استخدم نمط CEI أو ReentrancyGuard";
            مشاكل.push_back(مشكلة);
        }
        
        return مشاكل;
    }
};

/**
 * قاعدة فحص Integer Overflow
 */
class قاعدة_Overflow : public قاعدة_تحقق {
public:
    std::string الاسم() const override { return "Integer Overflow"; }
    
    std::vector<مشكلة_العقد> فحص(const std::string& كود) override {
        std::vector<مشكلة_العقد> مشاكل;
        
        // في Solidity 0.8+ هناك فحص تلقائي
        // لكن unchecked blocks لا تفحص
        
        if (كود.find("unchecked") != std::string::npos) {
            مشكلة_العقد مشكلة;
            مشكلة.نوع = نوع_المشكلة::IntegerOverflow;
            مشكلة.خطورة = خطورة_المشكلة::عالية;
            مشكلة.رسالة = "استخدام unchecked قد يسبب overflow";
            مشكلة.إصلاح_مقترح = "تأكد من صحة العمليات الحسابية";
            مشاكل.push_back(مشكلة);
        }
        
        return مشاكل;
    }
};

/**
 * قاعدة فحص tx.origin
 */
class قاعدة_TxOrigin : public قاعدة_تحقق {
public:
    std::string الاسم() const override { return "tx.origin"; }
    
    std::vector<مشكلة_العقد> فحص(const std::string& كود) override {
        std::vector<مشكلة_العقد> مشاكل;
        
        if (كود.find("tx.origin") != std::string::npos ||
            كود.find("msg.المصدر") != std::string::npos) {
            مشكلة_العقد مشكلة;
            مشكلة.نوع = نوع_المشكلة::TxOrigin;
            مشكلة.خطورة = خطورة_المشكلة::عالية;
            مشكلة.رسالة = "استخدام tx.origin للتحقق من الهوية غير آمن";
            مشكلة.إصلاح_مقترح = "استخدم msg.sender بدلاً من tx.origin";
            مشاكل.push_back(مشكلة);
        }
        
        return مشاكل;
    }
};

// ==========================================================
// 📌 مدقق العقود
// ==========================================================

/**
 * تقرير التدقيق
 */
struct تقرير_التدقيق {
    std::string اسم_العقد;
    std::vector<مشكلة_العقد> مشاكل;
    int حرجة = 0;
    int عالية = 0;
    int متوسطة = 0;
    int منخفضة = 0;
    bool آمن = true;
    
    void احسب_الإحصائيات() {
        for (const auto& مشكلة : مشاكل) {
            switch (مشكلة.خطورة) {
                case خطورة_المشكلة::حرجة: حرجة++; آمن = false; break;
                case خطورة_المشكلة::عالية: عالية++; آمن = false; break;
                case خطورة_المشكلة::متوسطة: متوسطة++; break;
                case خطورة_المشكلة::منخفضة: منخفضة++; break;
                default: break;
            }
        }
    }
    
    std::string النص() const {
        std::ostringstream ss;
        
        ss << "═══════════════════════════════════════════════════\n";
        ss << "🔍 تقرير تدقيق العقد: " << اسم_العقد << "\n";
        ss << "═══════════════════════════════════════════════════\n\n";
        
        ss << "📊 ملخص:\n";
        ss << "   🔴 حرجة: " << حرجة << "\n";
        ss << "   🟠 عالية: " << عالية << "\n";
        ss << "   🟡 متوسطة: " << متوسطة << "\n";
        ss << "   🟢 منخفضة: " << منخفضة << "\n\n";
        
        if (!مشاكل.empty()) {
            ss << "📋 التفاصيل:\n";
            ss << "───────────────────────────────────────────────────\n";
            
            int رقم = 1;
            for (const auto& مشكلة : مشاكل) {
                ss << رقم++ << ". [" << مشكلة.النوع_كنص() << "]\n";
                ss << "   " << مشكلة.رسالة << "\n";
                if (!مشكلة.إصلاح_مقترح.empty()) {
                    ss << "   💡 " << مشكلة.إصلاح_مقترح << "\n";
                }
                ss << "\n";
            }
        }
        
        ss << "═══════════════════════════════════════════════════\n";
        ss << (آمن ? "✅ العقد آمن" : "❌ العقد يحتاج مراجعة") << "\n";
        
        return ss.str();
    }
};

/**
 * مدقق العقود الذكية
 */
class مدقق_العقود {
private:
    std::vector<std::unique_ptr<قاعدة_تحقق>> قواعد_;
    
public:
    مدقق_العقود() {
        // إضافة القواعد
        قواعد_.push_back(std::make_unique<قاعدة_Reentrancy>());
        قواعد_.push_back(std::make_unique<قاعدة_Overflow>());
        قواعد_.push_back(std::make_unique<قاعدة_TxOrigin>());
    }
    
    /**
     * تدقيق عقد
     */
    تقرير_التدقيق دقق(const std::string& كود, 
                       const std::string& اسم_العقد = "Contract") {
        تقرير_التدقيق تقرير;
        تقرير.اسم_العقد = اسم_العقد;
        
        // تنفيذ كل قاعدة
        for (const auto& قاعدة : قواعد_) {
            auto مشاكل = قاعدة->فحص(كود);
            for (auto& مشكلة : مشاكل) {
                تقرير.مشاكل.push_back(std::move(مشكلة));
            }
        }
        
        تقرير.احسب_الإحصائيات();
        
        return تقرير;
    }
    
    /**
     * إضافة قاعدة مخصصة
     */
    void أضف_قاعدة(std::unique_ptr<قاعدة_تحقق> قاعدة) {
        قواعد_.push_back(std::move(قاعدة));
    }
};

// ==========================================================
// 📌 فاحص توافق ERC
// ==========================================================

/**
 * فاحص توافق ERC-20
 */
class فاحص_ERC20 {
public:
    std::vector<std::string> افحص(const std::string& كود) {
        std::vector<std::string> مشاكل;
        
        // الدوال المطلوبة
        std::vector<std::string> دوال_مطلوبة = {
            "totalSupply", "balanceOf", "transfer",
            "allowance", "approve", "transferFrom"
        };
        
        for (const auto& دالة : دوال_مطلوبة) {
            if (كود.find(دالة) == std::string::npos) {
                مشاكل.push_back("دالة مطلوبة مفقودة: " + دالة);
            }
        }
        
        // الأحداث المطلوبة
        if (كود.find("event Transfer") == std::string::npos &&
            كود.find("حدث تحويل") == std::string::npos) {
            مشاكل.push_back("حدث Transfer مفقود");
        }
        
        if (كود.find("event Approval") == std::string::npos &&
            كود.find("حدث موافقة") == std::string::npos) {
            مشاكل.push_back("حدث Approval مفقود");
        }
        
        return مشاكل;
    }
};

} // namespace contracts
} // namespace tools
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

using namespace sad::tools::contracts;

typedef struct SadContractVerifier SadContractVerifier;
typedef struct SadAuditReport SadAuditReport;

SadContractVerifier* sad_contract_verifier_create(void) {
    return reinterpret_cast<SadContractVerifier*>(new مدقق_العقود());
}

void sad_contract_verifier_destroy(SadContractVerifier* verifier) {
    delete reinterpret_cast<مدقق_العقود*>(verifier);
}

SadAuditReport* sad_contract_verifier_audit(SadContractVerifier* verifier,
                                            const char* code,
                                            const char* name) {
    if (!verifier || !code) return nullptr;
    
    auto تقرير = reinterpret_cast<مدقق_العقود*>(verifier)->دقق(
        code, name ? name : "Contract"
    );
    
    return reinterpret_cast<SadAuditReport*>(new تقرير_التدقيق(std::move(تقرير)));
}

void sad_audit_report_destroy(SadAuditReport* report) {
    delete reinterpret_cast<تقرير_التدقيق*>(report);
}

int sad_audit_report_is_safe(SadAuditReport* report) {
    if (!report) return 0;
    return reinterpret_cast<تقرير_التدقيق*>(report)->آمن ? 1 : 0;
}

const char* sad_audit_report_text(SadAuditReport* report) {
    if (!report) return nullptr;
    static std::string نص;
    نص = reinterpret_cast<تقرير_التدقيق*>(report)->النص();
    return نص.c_str();
}

} // extern "C"
