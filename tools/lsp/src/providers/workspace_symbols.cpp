// ============================================================================
// workspace_symbols.cpp — مزود رموز مساحة العمل لخادم LSP
// Workspace Symbols Provider for LSP Server
// ============================================================================
//
// الوصف (AR):
//   يوفر البحث في رموز مساحة العمل الكاملة (workspace/symbol) مما يتيح:
//   - البحث السريع عن الدوال والأصناف والمتغيرات عبر جميع الملفات
//   - دعم البحث بالعربية مع تجاهل التشكيل
//   - بحث ضبابي (fuzzy) للأسماء العربية والإنجليزية
//   - فلترة حسب نوع الرمز
//
// Description (EN):
//   Provides workspace-wide symbol search (workspace/symbol):
//   - Fast search for functions, classes, variables across all files
//   - Arabic search with diacritics-insensitive matching
//   - Fuzzy search for Arabic and English names
//   - Filter by symbol kind
//
// المهندس: عمر — أبريل 2026 / Engineer: Omar — April 2026
// ============================================================================

#include "../include/lsp_engine.h"
#include "../include/arabic_utils.h"

#include <algorithm>
#include <cctype>

namespace sad {
namespace lsp {

// ============================================================================
// تحويل أنواع الرموز / Symbol Kind Conversion
// ============================================================================

/// تحويل نوع الرمز المحلل إلى نوع LSP SymbolKind
static int analyzed_to_lsp_kind(AnalyzedSymbolKind kind) {
    switch (kind) {
        case AnalyzedSymbolKind::Function:    return 12; // Function
        case AnalyzedSymbolKind::Method:      return 6;  // Method
        case AnalyzedSymbolKind::Class:       return 5;  // Class
        case AnalyzedSymbolKind::Variable:    return 13; // Variable
        case AnalyzedSymbolKind::Constant:    return 14; // Constant
        case AnalyzedSymbolKind::Property:    return 7;  // Property
        case AnalyzedSymbolKind::Enum:        return 10; // Enum
        case AnalyzedSymbolKind::Constructor: return 9;  // Constructor
        case AnalyzedSymbolKind::Interface:   return 11; // Interface
        case AnalyzedSymbolKind::Struct:      return 23; // Struct
        case AnalyzedSymbolKind::Module:      return 2;  // Module
        case AnalyzedSymbolKind::Import:      return 2;  // Module
        case AnalyzedSymbolKind::Parameter:   return 13; // Variable
        default: return 13;
    }
}

// ============================================================================
// بحث ضبابي بالعربية / Arabic Fuzzy Search
// ============================================================================

/// حساب درجة التطابق الضبابي بين نمط بحث واسم رمز
/// يدعم العربية مع تجاهل التشكيل
/// Returns -1 if no match, higher = better match
static int fuzzy_match_score(const std::string& pattern, const std::string& target) {
    if (pattern.empty()) return 0;
    if (target.empty()) return -1;
    
    // تطابق تام / Exact match
    if (target == pattern) return 1000;
    
    // بداية التطابق / Starts with
    if (target.find(pattern) == 0) return 900;
    
    // يحتوي على / Contains
    if (target.find(pattern) != std::string::npos) return 500;
    
    // بحث ضبابي: كل حرف في النمط يجب أن يظهر بالترتيب
    // Fuzzy: each char in pattern must appear in order in target
    size_t patIdx = 0;
    size_t tarIdx = 0;
    int score = 0;
    int consecutive = 0;
    
    while (patIdx < pattern.size() && tarIdx < target.size()) {
        // للعربية: مقارنة متعددة البايت  
        // For Arabic: multi-byte comparison
        unsigned char pc = static_cast<unsigned char>(pattern[patIdx]);
        unsigned char tc = static_cast<unsigned char>(target[tarIdx]);
        
        // حرف ASCII عادي
        if (pc < 0x80 && tc < 0x80) {
            char pl = static_cast<char>(std::tolower(pc));
            char tl = static_cast<char>(std::tolower(tc));
            if (pl == tl) {
                score += 10 + consecutive * 5;
                consecutive++;
                patIdx++;
                tarIdx++;
            } else {
                consecutive = 0;
                tarIdx++;
            }
        }
        // حرف UTF-8 متعدد البايت
        else {
            // عدد بايتات الحرف
            int pLen = 1, tLen = 1;
            if (pc >= 0xC0) pLen = (pc >= 0xF0) ? 4 : (pc >= 0xE0) ? 3 : 2;
            if (tc >= 0xC0) tLen = (tc >= 0xF0) ? 4 : (tc >= 0xE0) ? 3 : 2;
            
            if (pLen == tLen && patIdx + pLen <= pattern.size() && tarIdx + tLen <= target.size()) {
                bool match = true;
                for (int i = 0; i < pLen && match; i++) {
                    if (pattern[patIdx + i] != target[tarIdx + i]) match = false;
                }
                if (match) {
                    score += 10 + consecutive * 5;
                    consecutive++;
                    patIdx += pLen;
                    tarIdx += tLen;
                } else {
                    consecutive = 0;
                    tarIdx += tLen;
                }
            } else {
                consecutive = 0;
                tarIdx++;
            }
        }
    }
    
    // هل تم العثور على كامل النمط؟
    if (patIdx >= pattern.size()) {
        return score;
    }
    
    return -1; // لم يتطابق
}

// ============================================================================
// مزود رموز مساحة العمل / Workspace Symbols Provider
// ============================================================================

/// البحث في جميع رموز مساحة العمل
/// @param store مخزن المستندات
/// @param query نص البحث (يمكن أن يكون فارغاً لإرجاع كل شيء)
/// @param max_results الحد الأقصى للنتائج
/// @return قائمة رموز مساحة العمل بتنسيق LSP JSON
std::string workspace_symbols(
    const DocumentStore& store,
    const std::string& query,
    int max_results = 100
) {
    struct ScoredSymbol {
        const AnalyzedSymbol* symbol;
        DocumentUri uri;
        int score;
    };
    
    std::vector<ScoredSymbol> matches;
    
    // جمع الرموز من جميع المستندات / Collect symbols from all documents
    auto uris = store.get_all_uris();
    for (const auto& uri : uris) {
        auto doc = store.get(uri);
        if (!doc) continue;
        
        for (const auto& sym : doc->symbols) {
            int score = 0;
            
            if (query.empty()) {
                // بدون بحث: أرجع كل الرموز المهمة
                if (sym.kind == AnalyzedSymbolKind::Function ||
                    sym.kind == AnalyzedSymbolKind::Class ||
                    sym.kind == AnalyzedSymbolKind::Enum ||
                    sym.kind == AnalyzedSymbolKind::Interface) {
                    score = 100;
                } else {
                    continue;
                }
            } else {
                // بحث في الاسم الأصلي والمطبّع
                score = fuzzy_match_score(query, sym.name);
                int normScore = fuzzy_match_score(query, sym.normalized_name);
                score = std::max(score, normScore);
                
                if (score < 0) continue;
            }
            
            matches.push_back({&sym, uri, score});
        }
    }
    
    // ترتيب حسب الدرجة / Sort by score
    std::sort(matches.begin(), matches.end(),
        [](const ScoredSymbol& a, const ScoredSymbol& b) {
            return a.score > b.score;
        });
    
    // تقليص النتائج / Limit results
    if (static_cast<int>(matches.size()) > max_results) {
        matches.resize(max_results);
    }
    
    // بناء JSON للاستجابة / Build JSON response
    std::string json = "[";
    for (size_t i = 0; i < matches.size(); i++) {
        const auto& m = *matches[i].symbol;
        if (i > 0) json += ",";
        
        json += "{\"name\":\"" + m.name + "\"";
        json += ",\"kind\":" + std::to_string(analyzed_to_lsp_kind(m.kind));
        json += ",\"location\":{";
        json += "\"uri\":\"" + matches[i].uri + "\"";
        json += ",\"range\":{";
        json += "\"start\":{\"line\":" + std::to_string(m.definition_range.start.line) +
                ",\"character\":" + std::to_string(m.definition_range.start.character) + "}";
        json += ",\"end\":{\"line\":" + std::to_string(m.definition_range.end.line) +
                ",\"character\":" + std::to_string(m.definition_range.end.character) + "}";
        json += "}}}";
    }
    json += "]";
    
    return json;
}

} // namespace lsp
} // namespace sad
