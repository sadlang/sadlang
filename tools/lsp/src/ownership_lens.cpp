/**
 * =============================================================================
 * ملف: ownership_lens.cpp
 * الوصف: مُزود عدسات الملكية لـ LSP
 * المهمة: T252 - Ownership lens provider
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * 🔍 دليل المبتدئ لعدسات الملكية
 * ════════════════════════════════════
 * 
 * ما هي عدسات الكود (Code Lenses)؟
 * ─────────────────────────────────
 * نصوص صغيرة تظهر فوق السطر في المحرر، مثل:
 * 
 * ```
 *   ◈ 3 استعارات نشطة
 * دالة معالجة(مرجع بيانات: &بيانات) {
 * ```
 * 
 * لماذا نحتاجها للملكية؟
 * ───────────────────────
 * تساعد المبرمج على فهم:
 * - كم استعارة نشطة لمتغير؟
 * - أين تنتقل الملكية؟
 * - ما دورة حياة المتغير؟
 * 
 * أنواع العدسات:
 * ───────────────
 * 1. عدسة الاستعارات: عدد المراجع النشطة
 * 2. عدسة النقل: أين تنتقل الملكية
 * 3. عدسة الحياة: مدة حياة المتغير
 * 4. عدسة المتغيرات: المتغيرات قابلة التغيير
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <sstream>
#include <algorithm>

namespace sad::lsp {

// =============================================================================
// أنواع الملكية
// =============================================================================

/**
 * نوع الملكية
 */
enum class OwnershipKind {
    Owned,      // ملكية كاملة
    Borrowed,   // استعارة غير متغيرة
    MutBorrowed,// استعارة متغيرة
    Moved,      // تم نقلها
    Dropped     // تم إسقاطها
};

/**
 * معلومات استعارة
 */
struct BorrowInfo {
    std::string variableName;
    int line;
    int column;
    OwnershipKind kind;
    std::optional<int> lifetimeEnd;
    bool isMutable;
};

/**
 * معلومات نقل الملكية
 */
struct MoveInfo {
    std::string variableName;
    int fromLine;
    int toLine;
    std::string targetVariable;
    bool isExplicit;  // نقل صريح أم ضمني
};

// =============================================================================
// عدسة الكود
// =============================================================================

/**
 * عدسة واحدة
 */
struct CodeLens {
    int line;
    int startColumn;
    int endColumn;
    std::string title;       // النص المعروض
    std::string tooltip;     // تلميح عند التمرير
    std::string command;     // الأمر عند النقر
    std::vector<std::string> arguments;  // معطيات الأمر
    
    /**
     * تحويل لـ JSON (بروتوكول LSP)
     */
    std::string toJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"range\": {\n";
        ss << "    \"start\": {\"line\": " << line << ", \"character\": " << startColumn << "},\n";
        ss << "    \"end\": {\"line\": " << line << ", \"character\": " << endColumn << "}\n";
        ss << "  },\n";
        ss << "  \"command\": {\n";
        ss << "    \"title\": \"" << title << "\",\n";
        ss << "    \"tooltip\": \"" << tooltip << "\",\n";
        ss << "    \"command\": \"" << command << "\",\n";
        ss << "    \"arguments\": [";
        for (size_t i = 0; i < arguments.size(); i++) {
            if (i > 0) ss << ", ";
            ss << "\"" << arguments[i] << "\"";
        }
        ss << "]\n";
        ss << "  }\n";
        ss << "}";
        return ss.str();
    }
};

// =============================================================================
// محلل الملكية
// =============================================================================

/**
 * محلل معلومات الملكية من الكود
 */
class OwnershipAnalyzer {
public:
    /**
     * تحليل ملف وإرجاع معلومات الملكية
     */
    struct AnalysisResult {
        std::vector<BorrowInfo> borrows;
        std::vector<MoveInfo> moves;
        std::map<std::string, int> borrowCounts;  // اسم المتغير → عدد الاستعارات
        std::map<std::string, int> lifetimes;     // اسم المتغير → آخر سطر
    };
    
    AnalysisResult analyze(const std::string& source) {
        AnalysisResult result;
        
        // تحليل مبسط - سيتم ربطه مع المحلل الفعلي
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // البحث عن أنماط الاستعارة
            if (line.find("&متغير") != std::string::npos ||
                line.find("&ثابت") != std::string::npos) {
                BorrowInfo borrow;
                borrow.line = lineNum;
                borrow.isMutable = line.find("&متغير") != std::string::npos;
                borrow.kind = borrow.isMutable ? OwnershipKind::MutBorrowed 
                                                : OwnershipKind::Borrowed;
                result.borrows.push_back(borrow);
            }
            
            // البحث عن أنماط النقل
            if (line.find("انقل") != std::string::npos) {
                MoveInfo move;
                move.fromLine = lineNum;
                move.isExplicit = true;
                result.moves.push_back(move);
            }
        }
        
        return result;
    }
    
    /**
     * حساب عدد الاستعارات النشطة في سطر معين
     */
    int countActiveBorrows(const AnalysisResult& result, int line) {
        int count = 0;
        for (const auto& b : result.borrows) {
            if (b.line <= line && 
                (!b.lifetimeEnd || *b.lifetimeEnd >= line)) {
                count++;
            }
        }
        return count;
    }
};

// =============================================================================
// مُزود العدسات
// =============================================================================

/**
 * مُزود عدسات الملكية
 */
class OwnershipLensProvider {
public:
    /**
     * الحصول على جميع العدسات لملف
     */
    std::vector<CodeLens> provideLenses(const std::string& source, 
                                         const std::string& filePath) {
        std::vector<CodeLens> lenses;
        
        // تحليل الملف
        OwnershipAnalyzer analyzer;
        auto analysis = analyzer.analyze(source);
        
        // إنشاء عدسات الاستعارات
        auto borrowLenses = createBorrowLenses(analysis);
        lenses.insert(lenses.end(), borrowLenses.begin(), borrowLenses.end());
        
        // إنشاء عدسات النقل
        auto moveLenses = createMoveLenses(analysis);
        lenses.insert(lenses.end(), moveLenses.begin(), moveLenses.end());
        
        // إنشاء عدسات دورة الحياة
        auto lifetimeLenses = createLifetimeLenses(analysis);
        lenses.insert(lenses.end(), lifetimeLenses.begin(), lifetimeLenses.end());
        
        return lenses;
    }
    
    /**
     * حل (resolve) عدسة - إضافة تفاصيل إضافية
     */
    CodeLens resolveLens(const CodeLens& lens) {
        CodeLens resolved = lens;
        
        // إضافة tooltip تفصيلي
        if (lens.command == "sad.showBorrows") {
            resolved.tooltip = "انقر لعرض جميع الاستعارات";
        } else if (lens.command == "sad.showMoves") {
            resolved.tooltip = "انقر لعرض مسار نقل الملكية";
        } else if (lens.command == "sad.showLifetime") {
            resolved.tooltip = "انقر لعرض دورة حياة المتغير";
        }
        
        return resolved;
    }
    
private:
    std::vector<CodeLens> createBorrowLenses(
        const OwnershipAnalyzer::AnalysisResult& analysis) 
    {
        std::vector<CodeLens> lenses;
        
        // تجميع الاستعارات حسب السطر
        std::map<int, std::vector<BorrowInfo>> borrowsByLine;
        for (const auto& b : analysis.borrows) {
            borrowsByLine[b.line].push_back(b);
        }
        
        // إنشاء عدسة لكل سطر به استعارات
        for (const auto& [line, borrows] : borrowsByLine) {
            CodeLens lens;
            lens.line = line - 1;  // LSP يستخدم صفر-indexed
            lens.startColumn = 0;
            lens.endColumn = 0;
            
            int mutCount = 0, immutCount = 0;
            for (const auto& b : borrows) {
                if (b.isMutable) mutCount++;
                else immutCount++;
            }
            
            // بناء العنوان
            std::ostringstream title;
            if (mutCount > 0 && immutCount > 0) {
                title << "◈ " << mutCount << " استعارة متغيرة، " 
                      << immutCount << " ثابتة";
            } else if (mutCount > 0) {
                title << "◈ " << mutCount << " استعارة متغيرة";
            } else {
                title << "◈ " << immutCount << " استعارة ثابتة";
            }
            
            lens.title = title.str();
            lens.command = "sad.showBorrows";
            lens.arguments = {std::to_string(line)};
            
            lenses.push_back(lens);
        }
        
        return lenses;
    }
    
    std::vector<CodeLens> createMoveLenses(
        const OwnershipAnalyzer::AnalysisResult& analysis)
    {
        std::vector<CodeLens> lenses;
        
        for (const auto& move : analysis.moves) {
            CodeLens lens;
            lens.line = move.fromLine - 1;
            lens.startColumn = 0;
            lens.endColumn = 0;
            
            std::ostringstream title;
            title << "→ نقل";
            if (!move.targetVariable.empty()) {
                title << " إلى " << move.targetVariable;
            }
            if (move.toLine > 0) {
                title << " (سطر " << move.toLine << ")";
            }
            
            lens.title = title.str();
            lens.command = "sad.showMoves";
            lens.arguments = {std::to_string(move.fromLine)};
            
            lenses.push_back(lens);
        }
        
        return lenses;
    }
    
    std::vector<CodeLens> createLifetimeLenses(
        const OwnershipAnalyzer::AnalysisResult& analysis)
    {
        std::vector<CodeLens> lenses;
        
        // عدسات لدورة حياة المتغيرات
        for (const auto& [varName, lastLine] : analysis.lifetimes) {
            CodeLens lens;
            lens.line = 0;  // سيتم تحديده من المحلل
            lens.startColumn = 0;
            lens.endColumn = 0;
            lens.title = "⌛ حياة " + varName + " تنتهي في سطر " + std::to_string(lastLine);
            lens.command = "sad.showLifetime";
            lens.arguments = {varName};
            
            lenses.push_back(lens);
        }
        
        return lenses;
    }
};

// =============================================================================
// إعدادات العدسات
// =============================================================================

struct OwnershipLensSettings {
    bool enableBorrowLenses = true;
    bool enableMoveLenses = true;
    bool enableLifetimeLenses = true;
    bool showMutableBorrowsOnly = false;
    int minBorrowsToShow = 1;
    bool useArabicText = true;
    std::string iconStyle = "emoji";  // "emoji", "ascii", "none"
};

/**
 * مدير إعدادات العدسات
 */
class OwnershipLensSettingsManager {
public:
    OwnershipLensSettings getSettings() const { return settings_; }
    
    void updateSettings(const std::string& json) {
        // تحليل JSON وتحديث الإعدادات
        if (json.find("\"enableBorrowLenses\": false") != std::string::npos) {
            settings_.enableBorrowLenses = false;
        }
        if (json.find("\"enableMoveLenses\": false") != std::string::npos) {
            settings_.enableMoveLenses = false;
        }
        if (json.find("\"enableLifetimeLenses\": false") != std::string::npos) {
            settings_.enableLifetimeLenses = false;
        }
        if (json.find("\"showMutableBorrowsOnly\": true") != std::string::npos) {
            settings_.showMutableBorrowsOnly = true;
        }
        if (json.find("\"useArabicText\": false") != std::string::npos) {
            settings_.useArabicText = false;
        }
    }
    
private:
    OwnershipLensSettings settings_;
};

} // namespace sad::lsp

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadOwnershipLensProvider {
    sad::lsp::OwnershipLensProvider* provider;
};

SadOwnershipLensProvider* sad_ownership_lens_new() {
    auto* ctx = new SadOwnershipLensProvider();
    ctx->provider = new sad::lsp::OwnershipLensProvider();
    return ctx;
}

const char* sad_ownership_lens_provide(SadOwnershipLensProvider* ctx,
                                       const char* source,
                                       const char* filePath) {
    auto lenses = ctx->provider->provideLenses(source, filePath);
    
    static std::string result;
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < lenses.size(); i++) {
        if (i > 0) ss << ",\n";
        ss << lenses[i].toJson();
    }
    ss << "]";
    result = ss.str();
    
    return result.c_str();
}

void sad_ownership_lens_free(SadOwnershipLensProvider* ctx) {
    if (ctx) {
        delete ctx->provider;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef OWNERSHIP_LENS_TEST

#include <cassert>

void testBorrowLens() {
    std::cout << "🔍 اختبار عدسة الاستعارات...\n";
    
    sad::lsp::OwnershipLensProvider provider;
    
    std::string source = R"(
دالة رئيسية() {
    متغير س = 10
    متغير مرجع = &متغير س
    اطبع(مرجع)
}
)";
    
    auto lenses = provider.provideLenses(source, "test.ص");
    
    // يجب أن توجد عدسة للاستعارة
    bool hasBorrowLens = false;
    for (const auto& lens : lenses) {
        if (lens.command == "sad.showBorrows") {
            hasBorrowLens = true;
            break;
        }
    }
    
    assert(hasBorrowLens);
    std::cout << "   ✅ نجح\n";
}

void testMoveLens() {
    std::cout << "→ اختبار عدسة النقل...\n";
    
    sad::lsp::OwnershipLensProvider provider;
    
    std::string source = R"(
دالة رئيسية() {
    متغير س = نص::جديد("مرحبا")
    متغير ع = انقل س
    اطبع(ع)
}
)";
    
    auto lenses = provider.provideLenses(source, "test.ص");
    
    bool hasMoveLens = false;
    for (const auto& lens : lenses) {
        if (lens.command == "sad.showMoves") {
            hasMoveLens = true;
            break;
        }
    }
    
    assert(hasMoveLens);
    std::cout << "   ✅ نجح\n";
}

void testLensJson() {
    std::cout << "📝 اختبار تحويل JSON...\n";
    
    sad::lsp::CodeLens lens;
    lens.line = 5;
    lens.startColumn = 0;
    lens.endColumn = 10;
    lens.title = "◈ 2 استعارات";
    lens.command = "sad.showBorrows";
    lens.arguments = {"5"};
    
    auto json = lens.toJson();
    
    assert(json.find("\"line\": 5") != std::string::npos);
    assert(json.find("◈ 2 استعارات") != std::string::npos);
    assert(json.find("sad.showBorrows") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات عدسات الملكية\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBorrowLens();
    testMoveLens();
    testLensJson();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // OWNERSHIP_LENS_TEST
