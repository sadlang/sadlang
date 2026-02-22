/**
 * =============================================================================
 * ملف: borrow_hover.cpp
 * الوصف: معلومات التمرير (Hover) للاستعارات
 * المهمة: T258 - Hover info for borrows
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * 🖱️ دليل المبتدئ لمعلومات التمرير
 * ═══════════════════════════════════════
 * 
 * ما هي معلومات التمرير؟
 * ──────────────────────
 * عند وضع المؤشر على متغير، تظهر نافذة صغيرة بمعلومات عنه.
 * 
 * مثال:
 * ─────
 * ```sad
 * متغير مرجع = &أصل
 *        ↑
 *        │ 🖱️ عند التمرير هنا:
 *        │
 *        │ ┌───────────────────────────────┐
 *        │ │ مرجع: &عدد                    │
 *        │ │ ────────────────────────────── │
 *        │ │ 📍 مُستعار من: أصل (سطر 1)    │
 *        │ │ ⏳ صالح حتى: سطر 5            │
 *        │ │ 🔒 نوع الاستعارة: ثابتة       │
 *        │ └───────────────────────────────┘
 * ```
 * 
 * المعلومات المعروضة:
 * ──────────────────
 * 1. نوع المتغير
 * 2. مصدر الاستعارة (من أين؟)
 * 3. دورة الحياة (حتى متى صالحة؟)
 * 4. نوع الاستعارة (ثابتة/متغيرة)
 * 5. الاستعارات الأخرى النشطة
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
// أنواع معلومات التمرير
// =============================================================================

/**
 * نوع معلومات الملكية
 */
enum class OwnershipInfoKind {
    Owner,          // مالك أصلي
    ImmutableBorrow,// استعارة للقراءة
    MutableBorrow,  // استعارة للكتابة
    MovedValue,     // قيمة منقولة
    DroppedValue,   // قيمة مُسقطة
    TemporaryValue  // قيمة مؤقتة
};

/**
 * معلومات استعارة واحدة
 */
struct BorrowDetails {
    std::string sourceName;     // اسم المتغير المُستعار منه
    int sourceDefinedLine;      // سطر تعريف المصدر
    int borrowStartLine;        // سطر بداية الاستعارة
    int borrowEndLine;          // سطر نهاية الاستعارة (تقدير)
    bool isMutable;             // هل متغيرة؟
    std::string lifetimeName;   // اسم دورة الحياة (إن وجد)
};

/**
 * معلومات متغير كاملة
 */
struct VariableHoverInfo {
    std::string name;
    std::string type;
    OwnershipInfoKind ownershipKind;
    std::optional<BorrowDetails> borrowInfo;
    std::vector<std::string> activeBorrows;  // استعارات نشطة من هذا المتغير
    std::optional<int> movedAtLine;
    std::optional<int> droppedAtLine;
    std::string documentation;
};

// =============================================================================
// مُنسق Markdown
// =============================================================================

class HoverMarkdownFormatter {
public:
    /**
     * تنسيق معلومات المتغير كـ Markdown
     */
    std::string format(const VariableHoverInfo& info) {
        std::ostringstream md;
        
        // العنوان: النوع
        md << "```sad\n";
        md << info.name << ": " << info.type << "\n";
        md << "```\n\n";
        
        // الفاصل
        md << "---\n\n";
        
        // معلومات الملكية
        md << formatOwnershipInfo(info);
        
        // معلومات الاستعارة
        if (info.borrowInfo) {
            md << "\n" << formatBorrowInfo(*info.borrowInfo);
        }
        
        // الاستعارات النشطة
        if (!info.activeBorrows.empty()) {
            md << "\n" << formatActiveBorrows(info.activeBorrows);
        }
        
        // حالة النقل/الإسقاط
        if (info.movedAtLine) {
            md << "\n⚠️ **تم نقله** في سطر " << *info.movedAtLine << "\n";
        }
        if (info.droppedAtLine) {
            md << "\n🗑️ **سيُسقط** في سطر " << *info.droppedAtLine << "\n";
        }
        
        // التوثيق
        if (!info.documentation.empty()) {
            md << "\n---\n\n";
            md << info.documentation << "\n";
        }
        
        return md.str();
    }
    
private:
    std::string formatOwnershipInfo(const VariableHoverInfo& info) {
        std::ostringstream md;
        
        switch (info.ownershipKind) {
            case OwnershipInfoKind::Owner:
                md << "👑 **مالك** - يملك القيمة بالكامل\n";
                break;
            case OwnershipInfoKind::ImmutableBorrow:
                md << "🔗 **استعارة ثابتة** - للقراءة فقط\n";
                break;
            case OwnershipInfoKind::MutableBorrow:
                md << "✏️ **استعارة متغيرة** - يمكن التعديل\n";
                break;
            case OwnershipInfoKind::MovedValue:
                md << "➡️ **قيمة منقولة** - لم تعد صالحة\n";
                break;
            case OwnershipInfoKind::DroppedValue:
                md << "🗑️ **قيمة مُسقطة** - تم تحريرها\n";
                break;
            case OwnershipInfoKind::TemporaryValue:
                md << "⏱️ **قيمة مؤقتة** - ستُسقط قريباً\n";
                break;
        }
        
        return md.str();
    }
    
    std::string formatBorrowInfo(const BorrowDetails& borrow) {
        std::ostringstream md;
        
        md << "### معلومات الاستعارة\n\n";
        md << "| الحقل | القيمة |\n";
        md << "|-------|--------|\n";
        md << "| 📍 المصدر | `" << borrow.sourceName << "` (سطر " 
           << borrow.sourceDefinedLine << ") |\n";
        md << "| ⏳ صالحة | من سطر " << borrow.borrowStartLine 
           << " إلى سطر " << borrow.borrowEndLine << " |\n";
        md << "| 🔒 النوع | " << (borrow.isMutable ? "متغيرة ✏️" : "ثابتة 📖") << " |\n";
        
        if (!borrow.lifetimeName.empty()) {
            md << "| 🏷️ دورة الحياة | `" << borrow.lifetimeName << "` |\n";
        }
        
        return md.str();
    }
    
    std::string formatActiveBorrows(const std::vector<std::string>& borrows) {
        std::ostringstream md;
        
        md << "### الاستعارات النشطة (" << borrows.size() << ")\n\n";
        
        for (const auto& b : borrows) {
            md << "- `" << b << "`\n";
        }
        
        return md.str();
    }
};

// =============================================================================
// محلل الاستعارات للتمرير
// =============================================================================

class HoverAnalyzer {
public:
    /**
     * الحصول على معلومات متغير في موقع معين
     */
    std::optional<VariableHoverInfo> getInfoAt(const std::string& source,
                                                int line, int column) {
        // تحليل الكود للعثور على المتغير
        std::string varName = findVariableAt(source, line, column);
        if (varName.empty()) {
            return std::nullopt;
        }
        
        // جمع المعلومات
        VariableHoverInfo info;
        info.name = varName;
        info.type = inferType(source, varName);
        info.ownershipKind = inferOwnership(source, varName, line);
        info.borrowInfo = findBorrowInfo(source, varName, line);
        info.activeBorrows = findActiveBorrows(source, varName, line);
        
        return info;
    }
    
private:
    std::string findVariableAt(const std::string& source, int line, int column) {
        std::istringstream iss(source);
        std::string lineStr;
        int lineNum = 0;
        
        while (std::getline(iss, lineStr) && lineNum < line) {
            lineNum++;
        }
        
        if (lineNum != line) return "";
        
        // البحث عن الكلمة في الموقع
        int start = column;
        int end = column;
        
        while (start > 0 && isWordChar(lineStr[start - 1])) start--;
        while (end < (int)lineStr.length() && isWordChar(lineStr[end])) end++;
        
        if (start >= end) return "";
        
        return lineStr.substr(start, end - start);
    }
    
    std::string inferType(const std::string& source, const std::string& varName) {
        // تحليل مبسط للنوع
        std::istringstream iss(source);
        std::string line;
        
        while (std::getline(iss, line)) {
            // البحث عن تعريف مع نوع صريح
            size_t varPos = line.find("متغير " + varName);
            if (varPos != std::string::npos) {
                size_t colonPos = line.find(':', varPos);
                if (colonPos != std::string::npos) {
                    size_t typeEnd = line.find_first_of(" =\n", colonPos + 1);
                    std::string type = line.substr(colonPos + 1, typeEnd - colonPos - 1);
                    // إزالة المسافات
                    size_t typeStart = type.find_first_not_of(" \t");
                    if (typeStart != std::string::npos) {
                        type = type.substr(typeStart);
                    }
                    return type;
                }
                
                // استنتاج من القيمة
                size_t eqPos = line.find('=', varPos);
                if (eqPos != std::string::npos) {
                    std::string value = line.substr(eqPos + 1);
                    return inferTypeFromValue(value);
                }
            }
        }
        
        return "غير معروف";
    }
    
    std::string inferTypeFromValue(const std::string& value) {
        // تحليل مبسط للقيمة
        std::string trimmed = value;
        size_t start = trimmed.find_first_not_of(" \t");
        if (start != std::string::npos) {
            trimmed = trimmed.substr(start);
        }
        
        if (trimmed[0] == '"') return "نص";
        if (trimmed.find("صحيح") == 0 || trimmed.find("خطأ") == 0) return "منطقي";
        if (trimmed.find("نص::") != std::string::npos) return "نص";
        if (trimmed.find("مصفوفة::") != std::string::npos) return "مصفوفة";
        if (trimmed.find('&') == 0) {
            if (trimmed.find("&متغير") == 0) return "&متغير _";
            return "&_";
        }
        
        bool hasDigit = false;
        bool hasDot = false;
        for (char c : trimmed) {
            if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
            if (c == '.') hasDot = true;
            if (c == ' ' || c == '\n') break;
        }
        
        if (hasDigit) {
            return hasDot ? "عشري64" : "عدد";
        }
        
        return "غير معروف";
    }
    
    OwnershipInfoKind inferOwnership(const std::string& source,
                                      const std::string& varName,
                                      int atLine) {
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // البحث عن التعريف
            if (line.find("متغير " + varName) != std::string::npos) {
                if (line.find("&متغير") != std::string::npos) {
                    return OwnershipInfoKind::MutableBorrow;
                }
                if (line.find('&') != std::string::npos) {
                    return OwnershipInfoKind::ImmutableBorrow;
                }
                return OwnershipInfoKind::Owner;
            }
            
            // البحث عن النقل
            if (lineNum < atLine) {
                if (line.find("انقل " + varName) != std::string::npos ||
                    line.find("= " + varName) != std::string::npos) {
                    // قد يكون تم نقله
                    return OwnershipInfoKind::MovedValue;
                }
            }
        }
        
        return OwnershipInfoKind::Owner;
    }
    
    std::optional<BorrowDetails> findBorrowInfo(const std::string& source,
                                                  const std::string& varName,
                                                  int atLine) {
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            // البحث عن تعريف الاستعارة
            if (line.find("متغير " + varName) != std::string::npos &&
                line.find('&') != std::string::npos) {
                
                BorrowDetails borrow;
                borrow.borrowStartLine = lineNum;
                borrow.borrowEndLine = lineNum + 10;  // تقدير
                borrow.isMutable = line.find("&متغير") != std::string::npos;
                
                // البحث عن اسم المصدر
                size_t ampPos = line.find('&');
                if (ampPos != std::string::npos) {
                    size_t srcStart = line.find_first_not_of(" &متغير", ampPos);
                    if (srcStart != std::string::npos) {
                        size_t srcEnd = line.find_first_of(" \t\n)", srcStart);
                        borrow.sourceName = line.substr(srcStart, srcEnd - srcStart);
                        borrow.sourceDefinedLine = findDefinitionLine(source, borrow.sourceName);
                    }
                }
                
                return borrow;
            }
        }
        
        return std::nullopt;
    }
    
    int findDefinitionLine(const std::string& source, const std::string& varName) {
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            if (line.find("متغير " + varName) != std::string::npos) {
                return lineNum;
            }
        }
        
        return 0;
    }
    
    std::vector<std::string> findActiveBorrows(const std::string& source,
                                                 const std::string& varName,
                                                 int atLine) {
        std::vector<std::string> borrows;
        
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            
            if (lineNum > atLine) break;
            
            // البحث عن استعارات من هذا المتغير
            size_t ampPos = line.find("&" + varName);
            if (ampPos == std::string::npos) {
                ampPos = line.find("& " + varName);
            }
            
            if (ampPos != std::string::npos) {
                // البحث عن اسم المتغير المُستعير
                size_t varDefPos = line.find("متغير ");
                if (varDefPos != std::string::npos && varDefPos < ampPos) {
                    size_t nameStart = varDefPos + 10;
                    size_t nameEnd = line.find_first_of(" =:", nameStart);
                    std::string borrowerName = line.substr(nameStart, nameEnd - nameStart);
                    borrows.push_back(borrowerName);
                }
            }
        }
        
        return borrows;
    }
    
    bool isWordChar(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || 
               c == '_' || 
               static_cast<unsigned char>(c) >= 0xD8;  // عربي
    }
};

// =============================================================================
// مُزود معلومات التمرير
// =============================================================================

class BorrowHoverProvider {
public:
    /**
     * الحصول على معلومات التمرير
     */
    std::optional<std::string> getHover(const std::string& source,
                                         int line, int column) {
        HoverAnalyzer analyzer;
        auto info = analyzer.getInfoAt(source, line, column);
        
        if (!info) {
            return std::nullopt;
        }
        
        HoverMarkdownFormatter formatter;
        return formatter.format(*info);
    }
    
    /**
     * الحصول على معلومات التمرير كـ JSON (بروتوكول LSP)
     */
    std::string getHoverJson(const std::string& source,
                              int line, int column) {
        auto hover = getHover(source, line, column);
        
        if (!hover) {
            return "null";
        }
        
        std::ostringstream json;
        json << "{\n";
        json << "  \"contents\": {\n";
        json << "    \"kind\": \"markdown\",\n";
        json << "    \"value\": \"" << escapeJson(*hover) << "\"\n";
        json << "  }\n";
        json << "}";
        
        return json.str();
    }
    
private:
    std::string escapeJson(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

} // namespace sad::lsp

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadBorrowHoverProvider {
    sad::lsp::BorrowHoverProvider* provider;
};

SadBorrowHoverProvider* sad_borrow_hover_new() {
    auto* ctx = new SadBorrowHoverProvider();
    ctx->provider = new sad::lsp::BorrowHoverProvider();
    return ctx;
}

const char* sad_borrow_hover_get(SadBorrowHoverProvider* ctx,
                                  const char* source,
                                  int line,
                                  int column) {
    static std::string result;
    result = ctx->provider->getHoverJson(source, line, column);
    return result.c_str();
}

void sad_borrow_hover_free(SadBorrowHoverProvider* ctx) {
    if (ctx) {
        delete ctx->provider;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef BORROW_HOVER_TEST

#include <iostream>
#include <cassert>

void testBasicHover() {
    std::cout << "🖱️ اختبار التمرير الأساسي...\n";
    
    sad::lsp::BorrowHoverProvider provider;
    
    std::string source = R"(متغير س = 10
متغير مرجع = &س
اطبع(مرجع)
)";
    
    auto hover = provider.getHover(source, 1, 5);
    
    // يجب أن تكون هناك معلومات
    assert(hover.has_value());
    assert(hover->find("س") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

void testBorrowHover() {
    std::cout << "🔗 اختبار تمرير الاستعارة...\n";
    
    sad::lsp::BorrowHoverProvider provider;
    
    std::string source = R"(متغير أصل = 100
متغير مرجع = &أصل
)";
    
    auto hover = provider.getHover(source, 2, 5);
    
    if (hover) {
        // يجب أن تشمل معلومات الاستعارة
        assert(hover->find("استعارة") != std::string::npos ||
               hover->find("مصدر") != std::string::npos);
    }
    
    std::cout << "   ✅ نجح\n";
}

void testMarkdownFormatting() {
    std::cout << "📝 اختبار تنسيق Markdown...\n";
    
    sad::lsp::HoverMarkdownFormatter formatter;
    
    sad::lsp::VariableHoverInfo info;
    info.name = "س";
    info.type = "عدد";
    info.ownershipKind = sad::lsp::OwnershipInfoKind::Owner;
    
    auto md = formatter.format(info);
    
    assert(md.find("```sad") != std::string::npos);
    assert(md.find("س") != std::string::npos);
    assert(md.find("عدد") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات معلومات التمرير للاستعارات\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicHover();
    testBorrowHover();
    testMarkdownFormatting();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // BORROW_HOVER_TEST
