// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: symbol_index.cpp
// الوصف: فهرس الرموز - يفهرس كل الرموز عبر جميع الملفات
// ══════════════════════════════════════════════════════════════════════════════
//
// فهرس الرموز هو القاعدة المركزية التي تحتوي على:
//   ✦ جميع تعريفات الرموز (دوال، متغيرات، أصناف) من كل الملفات
//   ✦ جميع المراجع (أين يُستخدم كل رمز)
//   ✦ الدوال المدمجة (اطبع، ادخل، طول، إلخ)
//   ✦ بحث ضبابي يدعم العربية (تجاهل التشكيل وأشكال الألف)
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <algorithm>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

SymbolIndex::SymbolIndex() {
    // تسجيل الدوال المدمجة عند الإنشاء
    register_builtins();
}

SymbolIndex::~SymbolIndex() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  تحديث وحذف المستندات
// ══════════════════════════════════════════════════════════════════════════════

void SymbolIndex::update_document(const DocumentUri& uri,
                                   const std::vector<AnalyzedSymbol>& symbols,
                                   const std::vector<SymbolReference>& references) {
    std::unique_lock lock(mutex_);
    doc_symbols_[uri] = symbols;
    doc_references_[uri] = references;
    rebuild_name_index();
}

void SymbolIndex::remove_document(const DocumentUri& uri) {
    std::unique_lock lock(mutex_);
    doc_symbols_.erase(uri);
    doc_references_.erase(uri);
    rebuild_name_index();
}

// ══════════════════════════════════════════════════════════════════════════════
//  البحث عن تعريف رمز
// ══════════════════════════════════════════════════════════════════════════════

std::optional<AnalyzedSymbol> SymbolIndex::find_definition(
    const std::string& name,
    const DocumentUri& context_uri,
    const Position& /*context_pos*/) const
{
    std::shared_lock lock(mutex_);

    std::string normalized = arabic::normalize_arabic(name);

    // أولاً: بحث في الرموز المدمجة
    for (const auto& sym : builtins_) {
        if (sym.normalized_name == normalized || sym.name == name) {
            return sym;
        }
    }

    // ثانياً: بحث في المستند الحالي (الأولوية الأعلى)
    auto it = doc_symbols_.find(context_uri);
    if (it != doc_symbols_.end()) {
        for (const auto& sym : it->second) {
            if (sym.normalized_name == normalized || sym.name == name) {
                return sym;
            }
        }
    }

    // ثالثاً: بحث في كل المستندات الأخرى
    for (const auto& [uri, symbols] : doc_symbols_) {
        if (uri == context_uri) continue;
        for (const auto& sym : symbols) {
            if ((sym.normalized_name == normalized || sym.name == name) &&
                sym.is_exported) {
                return sym;
            }
        }
    }

    return std::nullopt;
}

// ══════════════════════════════════════════════════════════════════════════════
//  البحث عن مراجع رمز
// ══════════════════════════════════════════════════════════════════════════════

std::vector<SymbolReference> SymbolIndex::find_references(
    const std::string& name,
    const DocumentUri& /*context_uri*/) const
{
    std::shared_lock lock(mutex_);
    std::vector<SymbolReference> results;
    std::string normalized = arabic::normalize_arabic(name);

    // نبحث في مراجع كل المستندات
    // ملاحظة: المراجع يتم جمعها في مرحلة التحليل
    for (const auto& [uri, refs] : doc_references_) {
        for (const auto& ref : refs) {
            // نضيف المرجع فقط إذا كان نفس الرمز (فلترة بالاسم)
            std::string ref_normalized = arabic::normalize_arabic(ref.name);
            if (ref.name == name || ref_normalized == normalized) {
                results.push_back(ref);
            }
        }
    }

    // نضيف أيضاً مواقع التعريفات كمراجع
    for (const auto& [uri, symbols] : doc_symbols_) {
        for (const auto& sym : symbols) {
            if (sym.normalized_name == normalized || sym.name == name) {
                SymbolReference ref;
                ref.uri = sym.uri;
                ref.name = sym.name;
                ref.range = sym.name_range;
                ref.is_declaration = true;
                results.push_back(ref);
            }
        }
    }

    return results;
}

// ══════════════════════════════════════════════════════════════════════════════
//  البحث الضبابي (للإكمال التلقائي)
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AnalyzedSymbol> SymbolIndex::fuzzy_search(
    const std::string& prefix,
    const DocumentUri& context_uri,
    int max_results) const
{
    std::shared_lock lock(mutex_);
    std::vector<std::pair<double, const AnalyzedSymbol*>> scored;

    std::string normalized_prefix = arabic::normalize_arabic(prefix);

    auto score_symbol = [&](const AnalyzedSymbol& sym) -> double {
        // مطابقة بالبحث الضبابي العربي
        if (arabic::fuzzy_match_arabic(normalized_prefix, sym.normalized_name)) {
            double score = arabic::similarity_score(normalized_prefix, sym.normalized_name);

            // مكافأة للرموز في نفس المستند
            if (sym.uri == context_uri) score += 0.1;

            // مكافأة للدوال المدمجة
            if (sym.is_builtin) score += 0.05;

            return score;
        }
        return -1.0;
    };

    // بحث في الرموز المدمجة
    for (const auto& sym : builtins_) {
        double score = score_symbol(sym);
        if (score >= 0) {
            scored.push_back({score, &sym});
        }
    }

    // بحث في المستند الحالي أولاً
    auto it = doc_symbols_.find(context_uri);
    if (it != doc_symbols_.end()) {
        for (const auto& sym : it->second) {
            double score = score_symbol(sym);
            if (score >= 0) {
                scored.push_back({score, &sym});
            }
        }
    }

    // بحث في باقي المستندات
    for (const auto& [uri, symbols] : doc_symbols_) {
        if (uri == context_uri) continue;
        for (const auto& sym : symbols) {
            if (sym.is_exported || sym.scope_depth == 0) {
                double score = score_symbol(sym);
                if (score >= 0) {
                    scored.push_back({score, &sym});
                }
            }
        }
    }

    // ترتيب بالنتيجة (الأعلى أولاً)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // إرجاع أفضل النتائج
    std::vector<AnalyzedSymbol> results;
    int count = 0;
    for (const auto& [score, sym] : scored) {
        if (count >= max_results) break;
        results.push_back(*sym);
        count++;
    }

    return results;
}

// ══════════════════════════════════════════════════════════════════════════════
//  رموز المستند ومساحة العمل
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AnalyzedSymbol> SymbolIndex::get_document_symbols(
    const DocumentUri& uri) const
{
    std::shared_lock lock(mutex_);
    auto it = doc_symbols_.find(uri);
    if (it != doc_symbols_.end()) {
        return it->second;
    }
    return {};
}

std::vector<AnalyzedSymbol> SymbolIndex::get_workspace_symbols(
    const std::string& query) const
{
    std::shared_lock lock(mutex_);
    std::vector<AnalyzedSymbol> results;

    if (query.empty()) {
        // إرجاع كل الرموز
        for (const auto& [uri, symbols] : doc_symbols_) {
            results.insert(results.end(), symbols.begin(), symbols.end());
        }
    } else {
        // بحث ضبابي
        std::string normalized = arabic::normalize_arabic(query);
        for (const auto& [uri, symbols] : doc_symbols_) {
            for (const auto& sym : symbols) {
                if (arabic::fuzzy_match_arabic(normalized, sym.normalized_name)) {
                    results.push_back(sym);
                }
            }
        }
    }

    return results;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تسجيل الدوال المدمجة
// ══════════════════════════════════════════════════════════════════════════════

void SymbolIndex::register_builtins() {
    // ────────────────────────────────────────────────────
    // تسجيل كل الدوال المدمجة في لغة ص
    // هذه الدوال متاحة في كل مكان بدون استيراد
    // ────────────────────────────────────────────────────

    auto make_builtin = [](const std::string& name,
                           const std::string& doc,
                           const std::vector<std::pair<std::string, std::string>>& params,
                           const std::string& ret_type) -> AnalyzedSymbol {
        AnalyzedSymbol sym;
        sym.name = name;
        sym.normalized_name = arabic::normalize_arabic(name);
        sym.kind = AnalyzedSymbolKind::Function;
        sym.is_builtin = true;
        sym.documentation = doc;
        sym.type.name = ret_type;

        AnalyzedSymbol::FunctionInfo fi;
        fi.return_type.name = ret_type;
        for (const auto& [pname, ptype] : params) {
            fi.parameters.push_back({pname, TypeInfo{ptype}});
        }
        sym.func_info = fi;
        return sym;
    };

    // ──── دوال الإدخال/الإخراج ────
    builtins_.push_back(make_builtin(
        "\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9", // اطبع
        "طباعة قيمة أو أكثر على الشاشة.\n"
        "مثال: اطبع(\"مرحباً بالعالم\")\n"
        "يمكن طباعة أي نوع: أرقام، نصوص، مصفوفات.",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}}, // قيمة
        "\xd8\xb9\xd8\xaf\xd9\x85" // عدم
    ));

    builtins_.push_back(make_builtin(
        "\xd8\xa7\xd8\xaf\xd8\xae\xd9\x84", // ادخل
        "قراءة سطر من المستخدم.\n"
        "مثال: متغير الاسم = ادخل(\"ما اسمك؟ \")\n"
        "ترجع دائماً نصاً.",
        {{"\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9", "نص"}}, // رسالة
        "\xd9\x86\xd8\xb5" // نص
    ));

    // ──── دوال النوع ────
    builtins_.push_back(make_builtin(
        "\xd9\x86\xd9\x88\xd8\xb9", // نوع
        "إرجاع نوع القيمة كنص.\n"
        "مثال: نوع(42) ← \"رقم\"",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}},
        "\xd9\x86\xd8\xb5"
    ));

    // ──── دوال التحويل ────
    builtins_.push_back(make_builtin(
        "\xd8\xb1\xd9\x82\xd9\x85", // رقم
        "تحويل قيمة إلى رقم صحيح.\n"
        "مثال: رقم(\"42\") ← 42",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}},
        "\xd8\xb1\xd9\x82\xd9\x85"
    ));

    builtins_.push_back(make_builtin(
        "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", // عشري
        "تحويل قيمة إلى عدد عشري.\n"
        "مثال: عشري(\"3.14\") ← 3.14",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}},
        "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a"
    ));

    builtins_.push_back(make_builtin(
        "\xd9\x86\xd8\xb5", // نص
        "تحويل قيمة إلى نص.\n"
        "مثال: نص(42) ← \"42\"",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}},
        "\xd9\x86\xd8\xb5"
    ));

    builtins_.push_back(make_builtin(
        "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", // منطقي
        "تحويل قيمة إلى منطقي (صحيح/خطأ).\n"
        "مثال: منطقي(1) ← صحيح",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}},
        "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a"
    ));

    // ──── دوال المصفوفات ────
    builtins_.push_back(make_builtin(
        "\xd8\xb7\xd9\x88\xd9\x84", // طول
        "إرجاع طول مصفوفة أو نص.\n"
        "مثال: طول([1, 2, 3]) ← 3\n"
        "مثال: طول(\"مرحباً\") ← 6",
        {{"\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "أي"}},
        "\xd8\xb1\xd9\x82\xd9\x85"
    ));

    builtins_.push_back(make_builtin(
        "\xd8\xa3\xd8\xb6\xd9\x81", // أضف
        "إضافة عنصر إلى نهاية مصفوفة.\n"
        "مثال: أضف(قائمة، 42)",
        {{"\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9", "مصفوفة"},
         {"\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", "أي"}},
        "\xd8\xb9\xd8\xaf\xd9\x85"
    ));

    builtins_.push_back(make_builtin(
        "\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81", // احذف
        "حذف عنصر من مصفوفة.\n"
        "مثال: احذف(قائمة، 0) // يحذف العنصر الأول",
        {{"\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9", "مصفوفة"},
         {"\xd9\x81\xd9\x87\xd8\xb1\xd8\xb3", "رقم"}},
        "\xd8\xb9\xd8\xaf\xd9\x85"
    ));

    // ──── دوال الرياضيات ────
    builtins_.push_back(make_builtin(
        "\xd8\xac\xd8\xb0\xd8\xb1", // جذر
        "حساب الجذر التربيعي.\n"
        "مثال: جذر(16) ← 4.0",
        {{"\xd8\xb1\xd9\x82\xd9\x85", "عشري"}},
        "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a"
    ));

    builtins_.push_back(make_builtin(
        "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9_\xd9\x85\xd8\xb7\xd9\x84\xd9\x82\xd8\xa9", // قيمة_مطلقة
        "حساب القيمة المطلقة.\n"
        "مثال: قيمة_مطلقة(-5) ← 5",
        {{"\xd8\xb1\xd9\x82\xd9\x85", "عشري"}},
        "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a"
    ));

    builtins_.push_back(make_builtin(
        "\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89", // أقصى
        "إرجاع أكبر قيمة.\n"
        "مثال: أقصى(3, 7) ← 7",
        {{"\xd8\xa3", "أي"}, {"\xd8\xa8", "أي"}},
        "\xd8\xa3\xd9\x8a"
    ));

    builtins_.push_back(make_builtin(
        "\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89", // أدنى
        "إرجاع أصغر قيمة.\n"
        "مثال: أدنى(3, 7) ← 3",
        {{"\xd8\xa3", "أي"}, {"\xd8\xa8", "أي"}},
        "\xd8\xa3\xd9\x8a"
    ));

    // ──── دوال النصوص ────
    builtins_.push_back(make_builtin(
        "\xd9\x82\xd8\xb5", // قص
        "قص جزء من نص.\n"
        "مثال: قص(\"مرحباً\", 0, 3) ← \"مرح\"",
        {{"\xd9\x86\xd8\xb5", "نص"},
         {"\xd8\xa8\xd8\xaf\xd8\xa7\xd9\x8a\xd8\xa9", "رقم"},
         {"\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9", "رقم"}},
        "\xd9\x86\xd8\xb5"
    ));

    // ──── دوال عامة ────
    builtins_.push_back(make_builtin(
        "\xd9\x86\xd8\xb7\xd8\xa7\xd9\x82", // نطاق
        "إنشاء مصفوفة أرقام متسلسلة.\n"
        "مثال: نطاق(5) ← [0, 1, 2, 3, 4]\n"
        "مثال: نطاق(1, 10) ← [1, 2, ..., 9]",
        {{"\xd8\xa8\xd8\xaf\xd8\xa7\xd9\x8a\xd8\xa9", "رقم"},
         {"\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9", "رقم"}},
        "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9"
    ));
}

// ══════════════════════════════════════════════════════════════════════════════
//  إعادة بناء الفهرس العكسي
// ══════════════════════════════════════════════════════════════════════════════

void SymbolIndex::rebuild_name_index() {
    // يجب استدعاء هذه الدالة داخل قفل كتابة
    name_index_.clear();

    for (auto& [uri, symbols] : doc_symbols_) {
        for (auto& sym : symbols) {
            name_index_[sym.normalized_name].push_back(&sym);
            if (sym.name != sym.normalized_name) {
                name_index_[sym.name].push_back(&sym);
            }
        }
    }

    // إضافة المدمجات
    for (auto& sym : builtins_) {
        name_index_[sym.normalized_name].push_back(&sym);
        if (sym.name != sym.normalized_name) {
            name_index_[sym.name].push_back(&sym);
        }
    }
}

} // namespace lsp
} // namespace sad
