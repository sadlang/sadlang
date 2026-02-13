/**
 * =============================================================================
 * ملف: move_hints.cpp
 * الوصف: تلميحات مضمنة للنقل والنسخ
 * المهمة: T255 - Inline hints for moves/copies
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * 💡 دليل المبتدئ لتلميحات النقل
 * ═══════════════════════════════════════
 * 
 * ما هي التلميحات المضمنة؟
 * ────────────────────────
 * نصوص صغيرة تظهر بجانب الكود لتوضح ما يحدث:
 * 
 * ```sad
 * دالة مثال() {
 *     متغير س = نص::جديد("مرحبا")
 *     متغير ع = س           ← [نقل]
 *     //        ↑
 *     //    التلميح يظهر هنا
 * }
 * ```
 * 
 * أنواع التلميحات:
 * ────────────────
 * 
 * [نقل]   - الملكية تنتقل (س لم يعد صالحاً)
 * [نسخ]   - القيمة تُنسخ (س لا يزال صالحاً)
 * [استعارة] - مرجع فقط (لا نقل)
 * [إسقاط] - المتغير يُدمر هنا
 * 
 * متى يظهر كل نوع؟
 * ─────────────────
 * - نقل: عند تمرير قيمة لدالة أو إسنادها لمتغير آخر
 * - نسخ: مع الأنواع التي تُنسخ تلقائياً (أعداد، منطقي)
 * - استعارة: مع المراجع &
 * - إسقاط: نهاية النطاق
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <sstream>

namespace sad::lsp {

// =============================================================================
// أنواع التلميحات
// =============================================================================

/**
 * نوع التلميح
 */
enum class HintKind {
    Move,       // نقل ملكية
    Copy,       // نسخ قيمة
    Borrow,     // استعارة
    MutBorrow,  // استعارة متغيرة
    Drop,       // إسقاط
    Clone,      // استنساخ صريح
    Deref       // فك المرجع
};

/**
 * تلميح واحد
 */
struct InlineHint {
    int line;
    int column;
    std::string text;
    HintKind kind;
    std::string tooltip;
    bool paddingLeft = true;
    bool paddingRight = false;
    
    /**
     * الحصول على النص بالعربية
     */
    std::string getArabicText() const {
        switch (kind) {
            case HintKind::Move: return "نقل";
            case HintKind::Copy: return "نسخ";
            case HintKind::Borrow: return "&";
            case HintKind::MutBorrow: return "&متغير";
            case HintKind::Drop: return "إسقاط";
            case HintKind::Clone: return "استنساخ";
            case HintKind::Deref: return "*";
        }
        return "?";
    }
    
    /**
     * تحويل لـ JSON (بروتوكول LSP)
     */
    std::string toJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"position\": {\"line\": " << line << ", \"character\": " << column << "},\n";
        ss << "  \"label\": \"" << text << "\",\n";
        ss << "  \"kind\": " << static_cast<int>(kind) << ",\n";
        ss << "  \"tooltip\": \"" << tooltip << "\",\n";
        ss << "  \"paddingLeft\": " << (paddingLeft ? "true" : "false") << ",\n";
        ss << "  \"paddingRight\": " << (paddingRight ? "true" : "false") << "\n";
        ss << "}";
        return ss.str();
    }
};

// =============================================================================
// معلومات الأنواع
// =============================================================================

/**
 * معلومات نوع البيانات
 */
struct TypeInfo {
    std::string name;
    bool isCopy;      // هل يُنسخ تلقائياً؟
    bool isReference; // هل هو مرجع؟
    bool isMutable;   // هل هو متغير؟
    
    static TypeInfo integer() { return {"عدد", true, false, false}; }
    static TypeInfo float64() { return {"عشري64", true, false, false}; }
    static TypeInfo boolean() { return {"منطقي", true, false, false}; }
    static TypeInfo string() { return {"نص", false, false, false}; }
    static TypeInfo array(const std::string& elem) { 
        return {"مصفوفة<" + elem + ">", false, false, false}; 
    }
    static TypeInfo reference(const TypeInfo& inner) {
        return {"&" + inner.name, true, true, false};
    }
    static TypeInfo mutReference(const TypeInfo& inner) {
        return {"&متغير " + inner.name, true, true, true};
    }
};

// =============================================================================
// محلل النقل والنسخ
// =============================================================================

class MoveAnalyzer {
public:
    struct Usage {
        std::string variableName;
        int line;
        int column;
        HintKind operation;
        std::string context;  // استدعاء دالة، إسناد، إلخ
    };
    
    std::vector<Usage> analyze(const std::string& source) {
        std::vector<Usage> usages;
        
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        // تتبع المتغيرات وأنواعها
        std::map<std::string, TypeInfo> variables;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // البحث عن تعريفات المتغيرات
            size_t varPos = line.find("متغير");
            if (varPos != std::string::npos) {
                // استخراج اسم المتغير
                size_t nameStart = line.find_first_not_of(" \t", varPos + 10);
                if (nameStart != std::string::npos) {
                    size_t nameEnd = line.find_first_of(" =:", nameStart);
                    std::string varName = line.substr(nameStart, nameEnd - nameStart);
                    
                    // تحديد النوع (مبسط)
                    TypeInfo type;
                    if (line.find("نص::") != std::string::npos) {
                        type = TypeInfo::string();
                    } else if (line.find("مصفوفة") != std::string::npos) {
                        type = TypeInfo::array("عدد");
                    } else {
                        type = TypeInfo::integer();  // افتراضي
                    }
                    
                    variables[varName] = type;
                }
            }
            
            // البحث عن استخدامات (نقل/نسخ)
            for (const auto& [varName, type] : variables) {
                size_t usePos = line.find(varName);
                if (usePos != std::string::npos && line.find("متغير") == std::string::npos) {
                    // تجاهل التعريف
                    
                    Usage usage;
                    usage.variableName = varName;
                    usage.line = lineNum;
                    usage.column = usePos + varName.length();
                    
                    // تحديد نوع العملية
                    if (line.find("&متغير " + varName) != std::string::npos) {
                        usage.operation = HintKind::MutBorrow;
                        usage.context = "استعارة متغيرة";
                    } else if (line.find("&" + varName) != std::string::npos ||
                               line.find("& " + varName) != std::string::npos) {
                        usage.operation = HintKind::Borrow;
                        usage.context = "استعارة";
                    } else if (line.find("استنسخ " + varName) != std::string::npos) {
                        usage.operation = HintKind::Clone;
                        usage.context = "استنساخ صريح";
                    } else if (type.isCopy) {
                        usage.operation = HintKind::Copy;
                        usage.context = "نسخ تلقائي";
                    } else {
                        usage.operation = HintKind::Move;
                        usage.context = "نقل ملكية";
                    }
                    
                    usages.push_back(usage);
                }
            }
        }
        
        return usages;
    }
};

// =============================================================================
// مُزود التلميحات
// =============================================================================

class MoveHintsProvider {
public:
    /**
     * الحصول على جميع التلميحات لملف
     */
    std::vector<InlineHint> provideHints(const std::string& source) {
        std::vector<InlineHint> hints;
        
        MoveAnalyzer analyzer;
        auto usages = analyzer.analyze(source);
        
        for (const auto& usage : usages) {
            InlineHint hint;
            hint.line = usage.line - 1;  // صفر-indexed
            hint.column = usage.column;
            hint.kind = usage.operation;
            hint.text = getHintText(usage.operation);
            hint.tooltip = usage.context;
            
            hints.push_back(hint);
        }
        
        return hints;
    }
    
    /**
     * الحصول على التلميحات كـ JSON
     */
    std::string getHintsJson(const std::string& source) {
        auto hints = provideHints(source);
        
        std::ostringstream json;
        json << "[\n";
        for (size_t i = 0; i < hints.size(); i++) {
            if (i > 0) json << ",\n";
            json << "  " << hints[i].toJson();
        }
        json << "\n]";
        
        return json.str();
    }
    
private:
    std::string getHintText(HintKind kind) {
        switch (kind) {
            case HintKind::Move: return "← نقل";
            case HintKind::Copy: return "← نسخ";
            case HintKind::Borrow: return "← &";
            case HintKind::MutBorrow: return "← &متغير";
            case HintKind::Drop: return "← إسقاط";
            case HintKind::Clone: return "← استنساخ";
            case HintKind::Deref: return "← *";
        }
        return "";
    }
};

// =============================================================================
// إعدادات التلميحات
// =============================================================================

struct MoveHintSettings {
    bool enabled = true;
    bool showMoves = true;
    bool showCopies = true;
    bool showBorrows = true;
    bool showDrops = false;  // قد يكون مزعجاً
    bool useArabic = true;
    int maxHintsPerLine = 3;
    std::string style = "subtle";  // "subtle", "prominent", "minimal"
};

class MoveHintSettingsManager {
public:
    MoveHintSettings getSettings() const { return settings_; }
    
    void updateFromJson(const std::string& json) {
        // تحليل JSON مبسط
        if (json.find("\"enabled\": false") != std::string::npos) {
            settings_.enabled = false;
        }
        if (json.find("\"showMoves\": false") != std::string::npos) {
            settings_.showMoves = false;
        }
        if (json.find("\"showCopies\": false") != std::string::npos) {
            settings_.showCopies = false;
        }
        if (json.find("\"showBorrows\": false") != std::string::npos) {
            settings_.showBorrows = false;
        }
        if (json.find("\"showDrops\": true") != std::string::npos) {
            settings_.showDrops = true;
        }
        if (json.find("\"useArabic\": false") != std::string::npos) {
            settings_.useArabic = false;
        }
    }
    
    std::string toJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"enabled\": " << (settings_.enabled ? "true" : "false") << ",\n";
        ss << "  \"showMoves\": " << (settings_.showMoves ? "true" : "false") << ",\n";
        ss << "  \"showCopies\": " << (settings_.showCopies ? "true" : "false") << ",\n";
        ss << "  \"showBorrows\": " << (settings_.showBorrows ? "true" : "false") << ",\n";
        ss << "  \"showDrops\": " << (settings_.showDrops ? "true" : "false") << ",\n";
        ss << "  \"useArabic\": " << (settings_.useArabic ? "true" : "false") << ",\n";
        ss << "  \"maxHintsPerLine\": " << settings_.maxHintsPerLine << ",\n";
        ss << "  \"style\": \"" << settings_.style << "\"\n";
        ss << "}";
        return ss.str();
    }
    
private:
    MoveHintSettings settings_;
};

// =============================================================================
// مُولد الإسقاط
// =============================================================================

class DropHintsProvider {
public:
    struct DropPoint {
        std::string variableName;
        int line;
        std::string reason;  // "نهاية النطاق"، "إسناد جديد"، إلخ
    };
    
    std::vector<DropPoint> findDropPoints(const std::string& source) {
        std::vector<DropPoint> drops;
        
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        int scopeDepth = 0;
        
        // تتبع المتغيرات في كل نطاق
        std::map<int, std::vector<std::pair<std::string, int>>> scopeVars;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // تتبع النطاقات
            if (line.find('{') != std::string::npos) {
                scopeDepth++;
            }
            
            // تسجيل المتغيرات
            size_t varPos = line.find("متغير");
            if (varPos != std::string::npos) {
                size_t nameStart = line.find_first_not_of(" \t", varPos + 10);
                if (nameStart != std::string::npos) {
                    size_t nameEnd = line.find_first_of(" =:", nameStart);
                    std::string varName = line.substr(nameStart, nameEnd - nameStart);
                    scopeVars[scopeDepth].push_back({varName, lineNum});
                }
            }
            
            // نهاية النطاق
            if (line.find('}') != std::string::npos || 
                line.find("نهاية") != std::string::npos) {
                // إسقاط جميع متغيرات هذا النطاق
                for (const auto& [varName, defLine] : scopeVars[scopeDepth]) {
                    DropPoint drop;
                    drop.variableName = varName;
                    drop.line = lineNum;
                    drop.reason = "نهاية النطاق";
                    drops.push_back(drop);
                }
                scopeVars[scopeDepth].clear();
                scopeDepth--;
            }
        }
        
        return drops;
    }
};

} // namespace sad::lsp

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadMoveHintsProvider {
    sad::lsp::MoveHintsProvider* provider;
    sad::lsp::MoveHintSettingsManager* settings;
};

SadMoveHintsProvider* sad_move_hints_new() {
    auto* ctx = new SadMoveHintsProvider();
    ctx->provider = new sad::lsp::MoveHintsProvider();
    ctx->settings = new sad::lsp::MoveHintSettingsManager();
    return ctx;
}

const char* sad_move_hints_get(SadMoveHintsProvider* ctx, const char* source) {
    static std::string result;
    result = ctx->provider->getHintsJson(source);
    return result.c_str();
}

const char* sad_move_hints_settings(SadMoveHintsProvider* ctx) {
    static std::string result;
    result = ctx->settings->toJson();
    return result.c_str();
}

void sad_move_hints_update_settings(SadMoveHintsProvider* ctx, const char* json) {
    ctx->settings->updateFromJson(json);
}

void sad_move_hints_free(SadMoveHintsProvider* ctx) {
    if (ctx) {
        delete ctx->provider;
        delete ctx->settings;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef MOVE_HINTS_TEST

#include <iostream>
#include <cassert>

void testMoveDetection() {
    std::cout << "→ اختبار اكتشاف النقل...\n";
    
    sad::lsp::MoveAnalyzer analyzer;
    
    std::string source = R"(
دالة مثال() {
    متغير س = نص::جديد("مرحبا")
    متغير ع = س
}
)";
    
    auto usages = analyzer.analyze(source);
    
    // يجب أن يكون هناك استخدام (نقل)
    bool hasMove = false;
    for (const auto& u : usages) {
        if (u.operation == sad::lsp::HintKind::Move) {
            hasMove = true;
            break;
        }
    }
    
    assert(hasMove);
    std::cout << "   ✅ نجح\n";
}

void testCopyDetection() {
    std::cout << "📋 اختبار اكتشاف النسخ...\n";
    
    sad::lsp::MoveAnalyzer analyzer;
    
    std::string source = R"(
دالة مثال() {
    متغير س = 10
    متغير ع = س
}
)";
    
    auto usages = analyzer.analyze(source);
    
    // الأعداد تُنسخ
    bool hasCopy = false;
    for (const auto& u : usages) {
        if (u.operation == sad::lsp::HintKind::Copy) {
            hasCopy = true;
            break;
        }
    }
    
    assert(hasCopy);
    std::cout << "   ✅ نجح\n";
}

void testBorrowDetection() {
    std::cout << "🔗 اختبار اكتشاف الاستعارة...\n";
    
    sad::lsp::MoveAnalyzer analyzer;
    
    std::string source = R"(
دالة مثال() {
    متغير س = 10
    متغير م = &س
}
)";
    
    auto usages = analyzer.analyze(source);
    
    bool hasBorrow = false;
    for (const auto& u : usages) {
        if (u.operation == sad::lsp::HintKind::Borrow) {
            hasBorrow = true;
            break;
        }
    }
    
    assert(hasBorrow);
    std::cout << "   ✅ نجح\n";
}

void testHintsJson() {
    std::cout << "📝 اختبار JSON للتلميحات...\n";
    
    sad::lsp::MoveHintsProvider provider;
    
    std::string source = "متغير س = 10\nمتغير ع = س\n";
    
    auto json = provider.getHintsJson(source);
    
    assert(json.find("position") != std::string::npos);
    assert(json.find("label") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات تلميحات النقل\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testMoveDetection();
    testCopyDetection();
    testBorrowDetection();
    testHintsJson();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // MOVE_HINTS_TEST
