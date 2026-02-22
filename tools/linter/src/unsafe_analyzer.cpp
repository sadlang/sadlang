/**
 * =============================================================================
 * ملف: unsafe_analyzer.cpp
 * الوصف: محلل كتل الكود غير الآمنة في لغة ص
 * المهمة: T219 - Unsafe block analyzer
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 🔓 دليل المبتدئ لفهم الكود غير الآمن
 * ═════════════════════════════════════
 * 
 * لماذا نحتاج "غير_آمن"؟
 * ────────────────────────
 * لغة ص آمنة جداً، تحميك من الأخطاء تلقائياً.
 * لكن أحياناً نحتاج للوصول للأجهزة مباشرة أو التعامل مع كود C.
 * 
 * مثال من الحياة:
 * ─────────────────
 * تخيل سيارة بها نظام أمان:
 * - لا تتحرك إذا لم تربط الحزام ← حماية تلقائية
 * - لكن رجال الإطفاء يحتاجون لإيقاف هذا النظام ← حالة خاصة
 * 
 * "غير_آمن" مثل مفتاح الطوارئ - استخدمه فقط عند الضرورة!
 * 
 * متى نستخدم غير_آمن؟
 * ─────────────────────
 * 1. الوصول المباشر للذاكرة (برمجة الأنظمة)
 * 2. استدعاء مكتبات C (FFI)
 * 3. تحسين الأداء الحرج
 * 4. برمجة الأجهزة المضمنة
 * 
 * ماذا يفعل هذا المحلل؟
 * ───────────────────────
 * 1. يجد كل كتل "غير_آمن" في الكود
 * 2. يُحلل ماذا تفعل هذه الكتل
 * 3. يُصنف مستوى الخطر
 * 4. يقترح بدائل آمنة إن وُجدت
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <iostream>
#include <stack>
#include <set>

namespace sad::security {

// =============================================================================
// أنواع العمليات غير الآمنة (Unsafe Operation Types)
// =============================================================================

/**
 * نوع العملية غير الآمنة
 * 
 * للمبتدئين:
 * كل نوع له مستوى خطر مختلف
 */
enum class UnsafeOperationType {
    RawPointerDeref,    // استخدام مؤشر خام
    RawPointerArith,    // حساب على مؤشرات
    FFICall,            // استدعاء دالة C
    InlineAsm,          // كود أسمبلي مدمج
    MutableStatic,      // متغير ثابت قابل للتغيير
    UnionAccess,        // الوصول لـ union
    MemoryAlloc,        // تخصيص ذاكرة يدوي
    TypeCast,           // تحويل نوع غير آمن
    VolatileAccess,     // وصول volatile
    BitManipulation,    // تلاعب بالبتات
    Unknown             // غير معروف
};

/**
 * تحويل نوع العملية لنص عربي
 */
inline std::string operationTypeToArabic(UnsafeOperationType type) {
    switch (type) {
        case UnsafeOperationType::RawPointerDeref:  return "استخدام مؤشر خام";
        case UnsafeOperationType::RawPointerArith:  return "حساب على مؤشرات";
        case UnsafeOperationType::FFICall:          return "استدعاء دالة خارجية";
        case UnsafeOperationType::InlineAsm:        return "كود أسمبلي";
        case UnsafeOperationType::MutableStatic:    return "متغير عام قابل للتغيير";
        case UnsafeOperationType::UnionAccess:      return "وصول لـ union";
        case UnsafeOperationType::MemoryAlloc:      return "تخصيص ذاكرة يدوي";
        case UnsafeOperationType::TypeCast:         return "تحويل نوع";
        case UnsafeOperationType::VolatileAccess:   return "وصول volatile";
        case UnsafeOperationType::BitManipulation:  return "تلاعب بالبتات";
        case UnsafeOperationType::Unknown:          return "عملية غير معروفة";
    }
    return "غير محدد";
}

// =============================================================================
// مستوى الخطر (Risk Level)
// =============================================================================

/**
 * مستوى خطر العملية
 */
enum class RiskLevel {
    Minimal,    // ضئيل - مثل تحويل نوع بسيط
    Low,        // منخفض - مثل الوصول لـ union
    Medium,     // متوسط - مثل FFI مع مكتبة موثوقة
    High,       // عالي - مثل تخصيص ذاكرة يدوي
    Critical    // حرج - مثل حساب على مؤشرات
};

inline std::string riskLevelToArabic(RiskLevel level) {
    switch (level) {
        case RiskLevel::Minimal:  return "🟢 ضئيل";
        case RiskLevel::Low:      return "🟡 منخفض";
        case RiskLevel::Medium:   return "🟠 متوسط";
        case RiskLevel::High:     return "🔴 عالي";
        case RiskLevel::Critical: return "⚫ حرج";
    }
    return "غير معروف";
}

// =============================================================================
// عملية غير آمنة (Unsafe Operation)
// =============================================================================

/**
 * تمثيل عملية غير آمنة واحدة
 */
struct UnsafeOperation {
    UnsafeOperationType type;       // نوع العملية
    RiskLevel risk;                 // مستوى الخطر
    std::string description;        // وصف العملية
    std::string code;               // الكود المعني
    int line;                       // رقم السطر
    int column;                     // رقم العمود
    std::string justification;      // تبرير الاستخدام (من التعليقات)
    std::string suggestion;         // اقتراح بديل
};

// =============================================================================
// كتلة غير آمنة (Unsafe Block)
// =============================================================================

/**
 * تمثيل كتلة غير_آمن
 * 
 * للمبتدئين:
 * كتلة "غير_آمن" هي جزء من الكود يتجاوز الحماية
 * 
 * مثال:
 * ```
 * غير_آمن
 *     # كود غير آمن هنا
 * نهاية
 * ```
 */
struct UnsafeBlock {
    int startLine;                          // سطر البداية
    int endLine;                            // سطر النهاية
    std::string file;                       // الملف
    std::vector<UnsafeOperation> operations; // العمليات داخل الكتلة
    std::string comment;                    // تعليق يشرح السبب
    bool hasJustification = false;          // هل هناك تبرير؟
    RiskLevel overallRisk = RiskLevel::Low; // الخطر الإجمالي
    
    /**
     * حساب الخطر الإجمالي
     */
    void calculateOverallRisk() {
        RiskLevel maxRisk = RiskLevel::Minimal;
        for (const auto& op : operations) {
            if (static_cast<int>(op.risk) > static_cast<int>(maxRisk)) {
                maxRisk = op.risk;
            }
        }
        overallRisk = maxRisk;
    }
    
    /**
     * تنسيق الكتلة للعرض
     */
    std::string format() const {
        std::ostringstream ss;
        
        ss << "\n╔══════════════════════════════════════════════════════════════╗\n";
        ss << "║  🔓 كتلة غير آمنة في " << file << "\n";
        ss << "╠══════════════════════════════════════════════════════════════╣\n";
        ss << "║  📍 الأسطر: " << startLine << " - " << endLine << "\n";
        ss << "║  ⚠️ الخطر: " << riskLevelToArabic(overallRisk) << "\n";
        
        if (hasJustification) {
            ss << "║  ✅ التبرير: " << comment << "\n";
        } else {
            ss << "║  ❌ لا يوجد تبرير! أضف تعليقاً يشرح سبب الحاجة لـ غير_آمن\n";
        }
        
        if (!operations.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  العمليات المكتشفة:\n";
            for (const auto& op : operations) {
                ss << "║  • " << operationTypeToArabic(op.type) 
                   << " [" << riskLevelToArabic(op.risk) << "]\n";
                ss << "║    " << op.description << "\n";
                if (!op.suggestion.empty()) {
                    ss << "║    💡 " << op.suggestion << "\n";
                }
            }
        }
        
        ss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
};

// =============================================================================
// محلل الكود غير الآمن (Unsafe Code Analyzer)
// =============================================================================

/**
 * محلل الكود غير الآمن
 * 
 * للمبتدئين:
 * هذا المحلل يقرأ الكود ويجد كل الأجزاء الخطرة
 */
class UnsafeAnalyzer {
public:
    /**
     * تحليل ملف
     */
    std::vector<UnsafeBlock> analyzeFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<UnsafeBlock> blocks;
        std::vector<std::string> lines;
        
        // تقسيم الملف لأسطر
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
        
        // البحث عن كتل غير_آمن
        for (size_t i = 0; i < lines.size(); i++) {
            if (isUnsafeBlockStart(lines[i])) {
                auto block = parseUnsafeBlock(lines, i, filePath);
                if (block.startLine > 0) {
                    blocks.push_back(block);
                }
            }
        }
        
        return blocks;
    }
    
    /**
     * تحليل كتلة واحدة
     */
    UnsafeBlock analyzeBlock(
        const std::vector<std::string>& lines,
        int startIndex,
        const std::string& filePath
    ) {
        return parseUnsafeBlock(lines, startIndex, filePath);
    }
    
    /**
     * الحصول على إحصائيات
     */
    struct Statistics {
        int totalBlocks = 0;
        int criticalBlocks = 0;
        int highRiskBlocks = 0;
        int mediumRiskBlocks = 0;
        int lowRiskBlocks = 0;
        int blocksWithJustification = 0;
        int blocksWithoutJustification = 0;
        std::map<UnsafeOperationType, int> operationCounts;
    };
    
    Statistics getStatistics(const std::vector<UnsafeBlock>& blocks) const {
        Statistics stats;
        stats.totalBlocks = static_cast<int>(blocks.size());
        
        for (const auto& block : blocks) {
            switch (block.overallRisk) {
                case RiskLevel::Critical: stats.criticalBlocks++; break;
                case RiskLevel::High:     stats.highRiskBlocks++; break;
                case RiskLevel::Medium:   stats.mediumRiskBlocks++; break;
                case RiskLevel::Low:
                case RiskLevel::Minimal:  stats.lowRiskBlocks++; break;
            }
            
            if (block.hasJustification) {
                stats.blocksWithJustification++;
            } else {
                stats.blocksWithoutJustification++;
            }
            
            for (const auto& op : block.operations) {
                stats.operationCounts[op.type]++;
            }
        }
        
        return stats;
    }
    
    /**
     * طباعة تقرير الإحصائيات
     */
    std::string formatStatistics(const Statistics& stats) const {
        std::ostringstream ss;
        
        ss << "\n╔══════════════════════════════════════════════════════════════╗\n";
        ss << "║  📊 إحصائيات تحليل الكود غير الآمن\n";
        ss << "╠══════════════════════════════════════════════════════════════╣\n";
        ss << "║  إجمالي الكتل: " << stats.totalBlocks << "\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  حسب الخطورة:\n";
        ss << "║    ⚫ حرج:    " << stats.criticalBlocks << "\n";
        ss << "║    🔴 عالي:   " << stats.highRiskBlocks << "\n";
        ss << "║    🟠 متوسط: " << stats.mediumRiskBlocks << "\n";
        ss << "║    🟢 منخفض: " << stats.lowRiskBlocks << "\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  التبرير:\n";
        ss << "║    ✅ مع تبرير:  " << stats.blocksWithJustification << "\n";
        ss << "║    ❌ بدون تبرير: " << stats.blocksWithoutJustification << "\n";
        
        if (!stats.operationCounts.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  العمليات:\n";
            for (const auto& [type, count] : stats.operationCounts) {
                ss << "║    • " << operationTypeToArabic(type) 
                   << ": " << count << "\n";
            }
        }
        
        ss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
    
private:
    /**
     * التحقق من بداية كتلة غير_آمن
     */
    bool isUnsafeBlockStart(const std::string& line) const {
        std::regex pattern(R"(^\s*(غير_آمن|unsafe)\s*$)");
        return std::regex_match(line, pattern);
    }
    
    /**
     * التحقق من نهاية كتلة (نهاية)
     * 
     * ملاحظة مهمة: في لغة ص، الكتل تنتهي بكلمة "نهاية" وليس }
     */
    bool isBlockEnd(const std::string& line) const {
        std::regex pattern(R"(^\s*(نهاية|end)\s*$)");
        return std::regex_match(line, pattern);
    }
    
    /**
     * تحليل كتلة غير_آمن
     */
    UnsafeBlock parseUnsafeBlock(
        const std::vector<std::string>& lines,
        size_t startIndex,
        const std::string& filePath
    ) {
        UnsafeBlock block;
        block.file = filePath;
        block.startLine = static_cast<int>(startIndex) + 1;
        
        // البحث عن تعليق قبل الكتلة (تبرير)
        if (startIndex > 0) {
            std::string prevLine = lines[startIndex - 1];
            std::regex commentPattern(R"(#\s*سبب:\s*(.+)|//\s*reason:\s*(.+))");
            std::smatch match;
            if (std::regex_search(prevLine, match, commentPattern)) {
                block.hasJustification = true;
                block.comment = match[1].matched ? match[1].str() : match[2].str();
            }
        }
        
        // تحليل محتوى الكتلة
        int depth = 1; // عمق التداخل
        for (size_t i = startIndex + 1; i < lines.size() && depth > 0; i++) {
            const std::string& line = lines[i];
            
            // التحقق من نهاية الكتلة
            if (isBlockEnd(line)) {
                depth--;
                if (depth == 0) {
                    block.endLine = static_cast<int>(i) + 1;
                    break;
                }
            }
            
            // التحقق من كتل متداخلة
            if (isUnsafeBlockStart(line)) {
                depth++;
            }
            
            // تحليل العمليات في السطر
            auto ops = analyzeLineForUnsafeOps(line, static_cast<int>(i) + 1);
            block.operations.insert(block.operations.end(), ops.begin(), ops.end());
        }
        
        // حساب الخطر الإجمالي
        block.calculateOverallRisk();
        
        return block;
    }
    
    /**
     * تحليل سطر للعمليات غير الآمنة
     */
    std::vector<UnsafeOperation> analyzeLineForUnsafeOps(
        const std::string& line,
        int lineNumber
    ) {
        std::vector<UnsafeOperation> ops;
        
        // 1. استخدام مؤشر خام (dereference)
        std::regex derefPattern(R"(\*\s*(\w+))");
        std::smatch match;
        if (std::regex_search(line, match, derefPattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::RawPointerDeref;
            op.risk = RiskLevel::High;
            op.description = "استخدام مؤشر خام: " + match.str();
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "استخدم مرجع آمن (&) بدلاً من المؤشر الخام";
            ops.push_back(op);
        }
        
        // 2. حساب على مؤشرات
        std::regex arithPattern(R"(\w+\s*[\+\-]\s*\d+\s*كـ\s*\*|ptr\s*[\+\-]\s*\d+)");
        if (std::regex_search(line, match, arithPattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::RawPointerArith;
            op.risk = RiskLevel::Critical;
            op.description = "حساب على مؤشرات: " + match.str();
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "استخدم فهرسة الشريحة [i] بدلاً من حساب المؤشرات";
            ops.push_back(op);
        }
        
        // 3. استدعاء FFI
        std::regex ffiPattern(R"(خارجي\s*\(|extern\s*\(|استدعاء_c|call_c)");
        if (std::regex_search(line, match, ffiPattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::FFICall;
            op.risk = RiskLevel::Medium;
            op.description = "استدعاء دالة خارجية (C/FFI)";
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "تأكد من صحة المعاملات قبل الاستدعاء";
            ops.push_back(op);
        }
        
        // 4. كود أسمبلي
        std::regex asmPattern(R"(أسمبلي\s*\{|asm\s*\{|__asm)");
        if (std::regex_search(line, match, asmPattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::InlineAsm;
            op.risk = RiskLevel::Critical;
            op.description = "كود أسمبلي مدمج";
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "استخدم دوال لغة ص إن أمكن بدلاً من الأسمبلي";
            ops.push_back(op);
        }
        
        // 5. تخصيص ذاكرة يدوي
        std::regex allocPattern(R"(خصص|تخصيص|alloc|malloc|new\s+\[)");
        if (std::regex_search(line, match, allocPattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::MemoryAlloc;
            op.risk = RiskLevel::High;
            op.description = "تخصيص ذاكرة يدوي";
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "استخدم المتجهات (متجه<T>) أو المؤشرات الذكية";
            ops.push_back(op);
        }
        
        // 6. تحويل نوع غير آمن
        std::regex castPattern(R"(كـ\s*\*|as\s+\*|transmute|reinterpret)");
        if (std::regex_search(line, match, castPattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::TypeCast;
            op.risk = RiskLevel::Medium;
            op.description = "تحويل نوع غير آمن";
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "تأكد من توافق الأنواع";
            ops.push_back(op);
        }
        
        // 7. وصول volatile
        std::regex volatilePattern(R"(متطاير|volatile)");
        if (std::regex_search(line, match, volatilePattern)) {
            UnsafeOperation op;
            op.type = UnsafeOperationType::VolatileAccess;
            op.risk = RiskLevel::Low;
            op.description = "وصول volatile للأجهزة";
            op.code = line;
            op.line = lineNumber;
            op.suggestion = "تأكد من تزامن الوصول مع الأجهزة";
            ops.push_back(op);
        }
        
        return ops;
    }
};

// =============================================================================
// تقرير التحليل (Analysis Report)
// =============================================================================

/**
 * تقرير تحليل شامل
 */
class UnsafeAnalysisReport {
public:
    void addBlock(const UnsafeBlock& block) {
        blocks_.push_back(block);
    }
    
    void addBlocks(const std::vector<UnsafeBlock>& blocks) {
        blocks_.insert(blocks_.end(), blocks.begin(), blocks.end());
    }
    
    /**
     * توليد التقرير
     */
    std::string generate(bool detailed = true) const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                    📋 تقرير تحليل الكود غير الآمن                     ║\n";
        ss << "║                         Unsafe Code Analysis Report                   ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        UnsafeAnalyzer analyzer;
        auto stats = analyzer.getStatistics(blocks_);
        ss << analyzer.formatStatistics(stats);
        
        if (detailed && !blocks_.empty()) {
            ss << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            ss << "                         التفاصيل\n";
            ss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            
            for (const auto& block : blocks_) {
                ss << block.format();
            }
        }
        
        // التوصيات
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                           💡 التوصيات                                ║\n";
        ss << "╠══════════════════════════════════════════════════════════════════════╣\n";
        
        if (stats.blocksWithoutJustification > 0) {
            ss << "║  ⚠️ " << stats.blocksWithoutJustification 
               << " كتل بدون تبرير - أضف تعليقاً يشرح السبب\n";
        }
        
        if (stats.criticalBlocks > 0) {
            ss << "║  🔴 " << stats.criticalBlocks 
               << " كتل حرجة - راجعها بعناية\n";
        }
        
        if (stats.totalBlocks == 0) {
            ss << "║  ✅ لا يوجد كود غير آمن - ممتاز!\n";
        } else if (stats.totalBlocks < 5) {
            ss << "║  ✅ عدد قليل من الكتل غير الآمنة - جيد\n";
        } else {
            ss << "║  ⚠️ عدد كبير من الكتل غير الآمنة - حاول تقليلها\n";
        }
        
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
    
    /**
     * هل التقرير نظيف؟
     */
    bool isClean() const {
        UnsafeAnalyzer analyzer;
        auto stats = analyzer.getStatistics(blocks_);
        return stats.criticalBlocks == 0 && 
               stats.blocksWithoutJustification == 0;
    }
    
    /**
     * الحصول على عدد المشاكل
     */
    int getIssueCount() const {
        UnsafeAnalyzer analyzer;
        auto stats = analyzer.getStatistics(blocks_);
        return stats.criticalBlocks + stats.highRiskBlocks + 
               stats.blocksWithoutJustification;
    }
    
private:
    std::vector<UnsafeBlock> blocks_;
};

} // namespace sad::security

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

struct SadUnsafeAnalyzer {
    sad::security::UnsafeAnalyzer* analyzer;
    sad::security::UnsafeAnalysisReport* report;
};

/**
 * إنشاء محلل جديد
 */
SadUnsafeAnalyzer* sad_unsafe_analyzer_new() {
    auto* ctx = new SadUnsafeAnalyzer();
    ctx->analyzer = new sad::security::UnsafeAnalyzer();
    ctx->report = new sad::security::UnsafeAnalysisReport();
    return ctx;
}

/**
 * تحرير المحلل
 */
void sad_unsafe_analyzer_free(SadUnsafeAnalyzer* ctx) {
    if (ctx) {
        delete ctx->analyzer;
        delete ctx->report;
        delete ctx;
    }
}

/**
 * تحليل ملف
 */
int sad_unsafe_analyze_file(
    SadUnsafeAnalyzer* ctx,
    const char* content,
    const char* filePath
) {
    if (!ctx || !content || !filePath) return -1;
    
    auto blocks = ctx->analyzer->analyzeFile(content, filePath);
    ctx->report->addBlocks(blocks);
    
    return static_cast<int>(blocks.size());
}

/**
 * طباعة التقرير
 */
void sad_unsafe_print_report(SadUnsafeAnalyzer* ctx, int detailed) {
    if (!ctx) return;
    std::cout << ctx->report->generate(detailed != 0);
}

/**
 * الحصول على عدد المشاكل
 */
int sad_unsafe_get_issue_count(SadUnsafeAnalyzer* ctx) {
    if (!ctx) return -1;
    return ctx->report->getIssueCount();
}

} // extern "C"

// =============================================================================
// اختبارات (Tests)
// =============================================================================

#ifdef UNSAFE_ANALYZER_TEST

#include <cassert>

void testBasicAnalysis() {
    sad::security::UnsafeAnalyzer analyzer;
    
    std::string code = R"(
دالة اختبار()
    متغير x = 10
    
    # سبب: نحتاج للوصول المباشر للذاكرة
    غير_آمن
        متغير مؤشر = *x
        خارجي(دالة_c, مؤشر)
    نهاية
نهاية
)";
    
    auto blocks = analyzer.analyzeFile(code, "test.ص");
    
    assert(blocks.size() == 1);
    assert(blocks[0].hasJustification == true);
    assert(blocks[0].operations.size() >= 1);
    
    std::cout << "✅ اختبار التحليل الأساسي نجح\n";
}

void testNestedBlocks() {
    sad::security::UnsafeAnalyzer analyzer;
    
    std::string code = R"(
غير_آمن
    متغير a = 1
    غير_آمن
        متغير b = 2
    نهاية
نهاية
)";
    
    auto blocks = analyzer.analyzeFile(code, "test.ص");
    
    // يجب أن يكتشف الكتلة الخارجية
    assert(blocks.size() >= 1);
    
    std::cout << "✅ اختبار الكتل المتداخلة نجح\n";
}

void testStatistics() {
    sad::security::UnsafeAnalyzer analyzer;
    
    std::string code = R"(
# سبب: FFI
غير_آمن
    خارجي(func1)
نهاية

غير_آمن
    *ptr
نهاية
)";
    
    auto blocks = analyzer.analyzeFile(code, "test.ص");
    auto stats = analyzer.getStatistics(blocks);
    
    assert(stats.totalBlocks == 2);
    assert(stats.blocksWithJustification == 1);
    assert(stats.blocksWithoutJustification == 1);
    
    std::cout << "✅ اختبار الإحصائيات نجح\n";
}

void testReport() {
    sad::security::UnsafeAnalyzer analyzer;
    sad::security::UnsafeAnalysisReport report;
    
    std::string code = R"(
# سبب: تحسين الأداء
غير_آمن
    خصص(1024)
    *بيانات
نهاية
)";
    
    auto blocks = analyzer.analyzeFile(code, "test.ص");
    report.addBlocks(blocks);
    
    std::string reportText = report.generate(true);
    assert(!reportText.empty());
    assert(reportText.find("تقرير") != std::string::npos);
    
    std::cout << "✅ اختبار التقرير نجح\n";
    std::cout << reportText;
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات محلل الكود غير الآمن\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicAnalysis();
    testNestedBlocks();
    testStatistics();
    testReport();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // UNSAFE_ANALYZER_TEST
