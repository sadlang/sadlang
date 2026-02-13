/**
 * =============================================================================
 * ملف: lifetime_visualizer.cpp
 * الوصف: مُصور دورة حياة المتغيرات
 * المهمة: T253 - Lifetime visualization
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * ⌛ دليل المبتدئ لتصور دورة الحياة
 * ═══════════════════════════════════════
 * 
 * ما هي دورة الحياة (Lifetime)؟
 * ────────────────────────────────
 * المدة التي يكون فيها المتغير صالحاً للاستخدام.
 * 
 * ```sad
 * دالة مثال() {
 *     متغير س = 10    // ← س تبدأ هنا
 *     اطبع(س)         // ← س متاحة
 *     متغير ع = س + 5 // ← س لا تزال متاحة
 * }                    // ← س تنتهي هنا
 * ```
 * 
 * ماذا يفعل المُصور؟
 * ──────────────────
 * يرسم خطاً جانبياً يوضح:
 * 
 * ```
 * 1 │ دالة مثال() {
 * 2 │┌    متغير س = 10
 * 3 ││    اطبع(س)
 * 4 ││    متغير ع = س + 5
 * 5 │└}
 * ```
 * 
 * الألوان:
 * ────────
 * 🟢 أخضر: المتغير صالح
 * 🟡 أصفر: استعارة نشطة
 * 🔴 أحمر: تم نقله / إسقاطه
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <sstream>
#include <algorithm>

namespace sad::lsp {

// =============================================================================
// أنواع التصور
// =============================================================================

/**
 * حالة المتغير في سطر معين
 */
enum class VariableState {
    NotYetDefined, // لم يُعرف بعد
    Valid,         // صالح للاستخدام
    Borrowed,      // مُستعار (لا يمكن تعديله)
    MutBorrowed,   // مُستعار بشكل متغير
    PartiallyMoved,// بعض أجزائه نُقلت
    Moved,         // نُقل بالكامل
    Dropped        // أُسقط / خرج من النطاق
};

/**
 * لون التصور
 */
struct VisualizationColor {
    int r, g, b, a;
    
    static VisualizationColor valid()    { return {100, 200, 100, 255}; } // أخضر
    static VisualizationColor borrowed() { return {255, 200, 100, 255}; } // أصفر
    static VisualizationColor moved()    { return {255, 100, 100, 255}; } // أحمر
    static VisualizationColor dropped()  { return {150, 150, 150, 255}; } // رمادي
    
    std::string toHex() const {
        char buf[10];
        snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X", r, g, b, a);
        return buf;
    }
};

// =============================================================================
// نطاق دورة الحياة
// =============================================================================

/**
 * نطاق واحد من دورة الحياة
 */
struct LifetimeSpan {
    std::string variableName;
    int startLine;
    int endLine;
    std::vector<std::pair<int, VariableState>> stateChanges;  // سطر → حالة
    
    VariableState getStateAtLine(int line) const {
        if (line < startLine) return VariableState::NotYetDefined;
        if (line > endLine) return VariableState::Dropped;
        
        VariableState current = VariableState::Valid;
        for (const auto& [changeLine, state] : stateChanges) {
            if (changeLine <= line) {
                current = state;
            }
        }
        return current;
    }
};

/**
 * زخرفة سطر (للعرض)
 */
struct LineDecoration {
    int line;
    int startColumn;
    int endColumn;
    std::string color;
    std::string borderStyle;  // "solid", "dashed", "dotted"
    std::string hoverMessage;
    bool isStart;
    bool isEnd;
};

// =============================================================================
// مُحلل دورة الحياة
// =============================================================================

class LifetimeAnalyzer {
public:
    struct AnalysisResult {
        std::vector<LifetimeSpan> spans;
        std::map<std::string, std::vector<int>> usages;  // اسم → أسطر الاستخدام
    };
    
    AnalysisResult analyze(const std::string& source) {
        AnalysisResult result;
        
        // تحليل مبسط - سيتم ربطه مع المحلل الفعلي
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        int scopeDepth = 0;
        std::vector<std::pair<std::string, int>> scopeStack;  // اسم، سطر البداية
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // تتبع النطاقات
            if (line.find('{') != std::string::npos || 
                line.find("نهاية") == std::string::npos && 
                (line.find("دالة") != std::string::npos || 
                 line.find("إذا") != std::string::npos ||
                 line.find("بينما") != std::string::npos)) {
                scopeDepth++;
            }
            
            if (line.find('}') != std::string::npos || 
                line.find("نهاية") != std::string::npos) {
                scopeDepth--;
                // إنهاء المتغيرات في هذا النطاق
            }
            
            // البحث عن تعريفات المتغيرات
            size_t varPos = line.find("متغير");
            if (varPos != std::string::npos) {
                // استخراج اسم المتغير (مبسط)
                size_t nameStart = line.find_first_not_of(" \t", varPos + 10);
                if (nameStart != std::string::npos) {
                    size_t nameEnd = line.find_first_of(" =:", nameStart);
                    std::string varName = line.substr(nameStart, nameEnd - nameStart);
                    
                    LifetimeSpan span;
                    span.variableName = varName;
                    span.startLine = lineNum;
                    span.endLine = lineNum + 10;  // تقدير مبدئي
                    span.stateChanges.push_back({lineNum, VariableState::Valid});
                    
                    result.spans.push_back(span);
                }
            }
            
            // تتبع الاستخدامات
            for (auto& span : result.spans) {
                if (line.find(span.variableName) != std::string::npos) {
                    result.usages[span.variableName].push_back(lineNum);
                }
            }
            
            // البحث عن النقل
            if (line.find("انقل") != std::string::npos) {
                // تحديث حالة المتغير
                for (auto& span : result.spans) {
                    if (line.find(span.variableName) != std::string::npos) {
                        span.stateChanges.push_back({lineNum, VariableState::Moved});
                    }
                }
            }
        }
        
        return result;
    }
};

// =============================================================================
// مُصور دورة الحياة
// =============================================================================

class LifetimeVisualizer {
public:
    struct VisualizationResult {
        std::vector<LineDecoration> decorations;
        std::string asciiVisualization;
        std::string svgVisualization;
    };
    
    /**
     * إنشاء التصور لملف
     */
    VisualizationResult visualize(const std::string& source) {
        VisualizationResult result;
        
        // تحليل دورات الحياة
        LifetimeAnalyzer analyzer;
        auto analysis = analyzer.analyze(source);
        
        // إنشاء الزخارف
        for (const auto& span : analysis.spans) {
            auto decos = createDecorations(span);
            result.decorations.insert(result.decorations.end(), 
                                       decos.begin(), decos.end());
        }
        
        // إنشاء التصور النصي
        result.asciiVisualization = createAsciiVisualization(source, analysis.spans);
        
        // إنشاء SVG
        result.svgVisualization = createSvgVisualization(source, analysis.spans);
        
        return result;
    }
    
    /**
     * الحصول على زخارف لعرضها في المحرر
     */
    std::vector<LineDecoration> getDecorations(const std::string& source) {
        auto result = visualize(source);
        return result.decorations;
    }
    
private:
    std::vector<LineDecoration> createDecorations(const LifetimeSpan& span) {
        std::vector<LineDecoration> decos;
        
        for (int line = span.startLine; line <= span.endLine; line++) {
            LineDecoration deco;
            deco.line = line - 1;  // صفر-indexed
            deco.startColumn = 0;
            deco.endColumn = 1;
            
            auto state = span.getStateAtLine(line);
            
            switch (state) {
                case VariableState::Valid:
                    deco.color = VisualizationColor::valid().toHex();
                    deco.hoverMessage = span.variableName + " صالح للاستخدام";
                    break;
                case VariableState::Borrowed:
                    deco.color = VisualizationColor::borrowed().toHex();
                    deco.hoverMessage = span.variableName + " مُستعار (للقراءة فقط)";
                    break;
                case VariableState::MutBorrowed:
                    deco.color = VisualizationColor::borrowed().toHex();
                    deco.hoverMessage = span.variableName + " مُستعار بشكل متغير";
                    break;
                case VariableState::Moved:
                    deco.color = VisualizationColor::moved().toHex();
                    deco.hoverMessage = span.variableName + " تم نقله - لم يعد صالحاً";
                    break;
                case VariableState::Dropped:
                    deco.color = VisualizationColor::dropped().toHex();
                    deco.hoverMessage = span.variableName + " خرج من النطاق";
                    break;
                default:
                    continue;
            }
            
            deco.borderStyle = "solid";
            deco.isStart = (line == span.startLine);
            deco.isEnd = (line == span.endLine);
            
            decos.push_back(deco);
        }
        
        return decos;
    }
    
    std::string createAsciiVisualization(const std::string& source,
                                          const std::vector<LifetimeSpan>& spans) {
        std::istringstream iss(source);
        std::ostringstream oss;
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // رسم الخطوط الجانبية
            std::string prefix;
            for (const auto& span : spans) {
                if (lineNum >= span.startLine && lineNum <= span.endLine) {
                    auto state = span.getStateAtLine(lineNum);
                    char c = '│';
                    if (lineNum == span.startLine) c = '┌';
                    else if (lineNum == span.endLine) c = '└';
                    
                    if (state == VariableState::Moved) c = '✗';
                    prefix += c;
                } else {
                    prefix += ' ';
                }
            }
            
            oss << std::setw(3) << lineNum << " " << prefix << " " << line << "\n";
        }
        
        return oss.str();
    }
    
    std::string createSvgVisualization(const std::string& source,
                                        const std::vector<LifetimeSpan>& spans) {
        std::ostringstream svg;
        
        int lineHeight = 20;
        int leftMargin = 30;
        int barWidth = 4;
        
        // حساب الأبعاد
        std::istringstream iss(source);
        int lineCount = 0;
        std::string line;
        while (std::getline(iss, line)) lineCount++;
        
        int width = 800;
        int height = lineCount * lineHeight + 40;
        
        svg << "<svg xmlns='http://www.w3.org/2000/svg' "
            << "width='" << width << "' height='" << height << "'>\n";
        
        // خلفية
        svg << "  <rect width='100%' height='100%' fill='#1e1e1e'/>\n";
        
        // رسم شرائط دورة الحياة
        int barX = leftMargin;
        for (const auto& span : spans) {
            int startY = (span.startLine - 1) * lineHeight + 20;
            int endY = (span.endLine) * lineHeight + 20;
            
            std::string color = VisualizationColor::valid().toHex();
            
            svg << "  <rect x='" << barX << "' y='" << startY << "' "
                << "width='" << barWidth << "' height='" << (endY - startY) << "' "
                << "fill='" << color << "' rx='2'/>\n";
            
            // تسمية
            svg << "  <text x='" << (barX + barWidth + 5) << "' y='" << (startY + 12) << "' "
                << "fill='#888' font-size='10'>" << span.variableName << "</text>\n";
            
            barX += 15;
        }
        
        svg << "</svg>";
        
        return svg.str();
    }
};

// =============================================================================
// مدير التصور
// =============================================================================

struct VisualizationSettings {
    bool enabled = true;
    bool showInGutter = true;
    bool showInMinimap = true;
    std::string theme = "default";  // "default", "colorblind", "minimal"
};

class LifetimeVisualizationManager {
public:
    void setSettings(const VisualizationSettings& settings) {
        settings_ = settings;
    }
    
    VisualizationSettings getSettings() const { return settings_; }
    
    std::string getDecorationsJson(const std::string& source) {
        if (!settings_.enabled) {
            return "[]";
        }
        
        LifetimeVisualizer visualizer;
        auto decos = visualizer.getDecorations(source);
        
        std::ostringstream json;
        json << "[\n";
        for (size_t i = 0; i < decos.size(); i++) {
            const auto& d = decos[i];
            if (i > 0) json << ",\n";
            json << "  {\n";
            json << "    \"range\": {\"startLine\": " << d.line 
                 << ", \"startColumn\": " << d.startColumn
                 << ", \"endLine\": " << d.line
                 << ", \"endColumn\": " << d.endColumn << "},\n";
            json << "    \"color\": \"" << d.color << "\",\n";
            json << "    \"hoverMessage\": \"" << d.hoverMessage << "\"\n";
            json << "  }";
        }
        json << "\n]";
        
        return json.str();
    }
    
private:
    VisualizationSettings settings_;
};

} // namespace sad::lsp

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadLifetimeVisualizer {
    sad::lsp::LifetimeVisualizer* visualizer;
    sad::lsp::LifetimeVisualizationManager* manager;
};

SadLifetimeVisualizer* sad_lifetime_visualizer_new() {
    auto* ctx = new SadLifetimeVisualizer();
    ctx->visualizer = new sad::lsp::LifetimeVisualizer();
    ctx->manager = new sad::lsp::LifetimeVisualizationManager();
    return ctx;
}

const char* sad_lifetime_visualize(SadLifetimeVisualizer* ctx, const char* source) {
    auto result = ctx->visualizer->visualize(source);
    static std::string ascii;
    ascii = result.asciiVisualization;
    return ascii.c_str();
}

const char* sad_lifetime_get_decorations(SadLifetimeVisualizer* ctx, const char* source) {
    static std::string json;
    json = ctx->manager->getDecorationsJson(source);
    return json.c_str();
}

void sad_lifetime_visualizer_free(SadLifetimeVisualizer* ctx) {
    if (ctx) {
        delete ctx->visualizer;
        delete ctx->manager;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef LIFETIME_VISUALIZER_TEST

#include <iostream>
#include <cassert>
#include <iomanip>

void testLifetimeSpan() {
    std::cout << "⌛ اختبار نطاق دورة الحياة...\n";
    
    sad::lsp::LifetimeSpan span;
    span.variableName = "س";
    span.startLine = 2;
    span.endLine = 5;
    span.stateChanges = {
        {2, sad::lsp::VariableState::Valid},
        {4, sad::lsp::VariableState::Moved}
    };
    
    assert(span.getStateAtLine(1) == sad::lsp::VariableState::NotYetDefined);
    assert(span.getStateAtLine(2) == sad::lsp::VariableState::Valid);
    assert(span.getStateAtLine(3) == sad::lsp::VariableState::Valid);
    assert(span.getStateAtLine(4) == sad::lsp::VariableState::Moved);
    assert(span.getStateAtLine(6) == sad::lsp::VariableState::Dropped);
    
    std::cout << "   ✅ نجح\n";
}

void testVisualization() {
    std::cout << "🎨 اختبار التصور...\n";
    
    sad::lsp::LifetimeVisualizer visualizer;
    
    std::string source = R"(دالة مثال() {
    متغير س = 10
    اطبع(س)
    متغير ع = انقل س
}
)";
    
    auto result = visualizer.visualize(source);
    
    // يجب أن يكون هناك تصور ASCII
    assert(!result.asciiVisualization.empty());
    
    // يجب أن تكون هناك زخارف
    assert(!result.decorations.empty());
    
    std::cout << "   التصور النصي:\n";
    std::cout << result.asciiVisualization << "\n";
    
    std::cout << "   ✅ نجح\n";
}

void testDecorations() {
    std::cout << "🖌️ اختبار الزخارف...\n";
    
    sad::lsp::LifetimeVisualizationManager manager;
    
    std::string source = "متغير س = 10\nاطبع(س)\n";
    
    auto json = manager.getDecorationsJson(source);
    
    assert(json.find("range") != std::string::npos);
    assert(json.find("color") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مُصور دورة الحياة\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testLifetimeSpan();
    testVisualization();
    testDecorations();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // LIFETIME_VISUALIZER_TEST
