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
#include "builtin_registry.h"   // (AR) كتالوج المدمجات المُولَّد من مصدر الحقيقة
#include <algorithm>
#include <unordered_set>

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

    // (AR) ترتيب الحجب (shadowing): التعريف المحلّيّ يحجب المدمجة التي تحمل
    //      الاسم نفسه. لذا نبحث المستندَ الحاليّ ثم بقيّة المستندات ثمّ المدمجات
    //      أخيرًا. (مهمّ بعد توسيع المدمجات إلى 1036 اسمًا من مصدر الحقيقة: لو
    //      عرّف المستخدم «دالة طول()» فالقفز للتعريف يجب أن يصل تعريفه لا المدمجة.)
    // (EN) Locals shadow builtins: search current doc, then other docs, then the
    //      1036 source-of-truth builtins last — so a user-defined name wins.

    // أولاً: بحث في المستند الحالي (الأولوية الأعلى)
    auto it = doc_symbols_.find(context_uri);
    if (it != doc_symbols_.end()) {
        for (const auto& sym : it->second) {
            if (sym.normalized_name == normalized || sym.name == name) {
                return sym;
            }
        }
    }

    // ثانياً: بحث في كل المستندات الأخرى (المصدّرة فقط)
    for (const auto& [uri, symbols] : doc_symbols_) {
        if (uri == context_uri) continue;
        for (const auto& sym : symbols) {
            if ((sym.normalized_name == normalized || sym.name == name) &&
                sym.is_exported) {
                return sym;
            }
        }
    }

    // ثالثاً (أخيرًا): بحث في الرموز المدمجة — تُحجَب بأيّ تعريف محلّيّ أعلاه
    for (const auto& sym : builtins_) {
        if (sym.normalized_name == normalized || sym.name == name) {
            return sym;
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
    // ──────────────────────────────────────────────────────────────────────
    // (AR) مصدر الحقيقة الوحيد للمدمجات: المصفوفة المُولَّدة Sad::Builtins::ALL_BUILTINS
    //      (من language-truth/builtins/*.yaml عبر gen_builtins_registry.py).
    //      لا تهريد يدويّ بعد اليوم — تعديل المدمجات يكون في YAML ثم إعادة البناء.
    // (EN) Single source of truth for builtins: the generated ALL_BUILTINS array
    //      (from language-truth/builtins/*.yaml via gen_builtins_registry.py).
    //      No hand-maintained list — edit the YAML, then rebuild.
    // ──────────────────────────────────────────────────────────────────────

    // (AR) تقسيم سلسلة المعاملات إلى أسماء. الفاصل في المصدر هو الفاصلة العربية
    //      «،» (U+060C)، وقد يتبعها فراغ («، ») حسب توثيق BuiltinMeta.paramsCsv؛
    //      التقليم اللاحق يستوعب الحالتين. علامة الاختياريّة «؟» تبقى ضمن الاسم
    //      عمدًا لأنها إشارة عرض مفيدة في التلميح (مثل «رسالة؟»).
    //
    //      ⚠ النوع: مصدر الحقيقة لا يحمل أنواع المعاملات بعد، فنترك حقل النوع
    //      فارغًا — لا نختلق «أيّ». مزوّدات العرض (تحويم/إكمال/توقيع) تُسقط النوع
    //      الفارغ، فيظهر «اطبع(قيمة)» لا «اطبع(قيمة: أيّ)» المختلَق.
    auto split_params = [](std::string_view csv)
        -> std::vector<std::pair<std::string, std::string>> {
        std::vector<std::pair<std::string, std::string>> out;
        const std::string sep = "\xd8\x8c"; // ،
        std::string s(csv);
        size_t pos = 0;
        auto push = [&](std::string t) {
            size_t b = t.find_first_not_of(" \t");
            size_t e = t.find_last_not_of(" \t");
            if (b == std::string::npos) return;
            std::string name = t.substr(b, e - b + 1);
            if (!name.empty()) out.push_back({name, std::string{}}); // النوع غير معروف ⇒ فارغ
        };
        while ((pos = s.find(sep)) != std::string::npos) {
            push(s.substr(0, pos));
            s.erase(0, pos + sep.size());
        }
        push(s);
        return out;
    };

    // (AR) إزالة التكرار: ALL_BUILTINS يحوي 1073 مدخلًا منها ~37 اسمًا مكرّرًا عبر
    //      فضاءات مختلفة (مثل «أرسل»، «استبدل»). دونه يظهر الرمز نفسه عدّة مرّات في
    //      الإكمال. نُبقي أوّل ظهور (بترتيب _index.yaml كما يفعل findBuiltinMeta).
    std::unordered_set<std::string_view> seen;
    seen.reserve(Sad::Builtins::ALL_BUILTINS.size());
    builtins_.reserve(Sad::Builtins::ALL_BUILTINS.size());
    for (const auto& b : Sad::Builtins::ALL_BUILTINS) {
        if (!seen.insert(b.canonicalName).second)
            continue; // اسم مكرّر — تخطّاه
        AnalyzedSymbol sym;
        sym.name = std::string(b.canonicalName);
        sym.normalized_name = arabic::normalize_arabic(sym.name);
        sym.kind = AnalyzedSymbolKind::Function;
        sym.is_builtin = true;

        // التوثيق: الوصف العربيّ + تلميح الوحدة المطلوبة إن لزِم استيراد.
        std::string doc(b.descriptionAr);
        if (b.requireImport && !b.requiredModule.empty() &&
            b.requiredModule != "NONE") {
            if (!doc.empty()) doc += "\n";
            doc += "\xd9\x8a\xd8\xaa\xd8\xb7\xd9\x84\xd8\xa8 \xd8\xa7\xd8\xb3\xd8\xaa\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaf: "; // يتطلب استيراد:
            doc += std::string(b.requiredModule);
        }
        sym.documentation = doc;

        std::string ret(b.returnType);
        sym.type.name = ret;

        AnalyzedSymbol::FunctionInfo fi;
        fi.return_type.name = ret;
        for (auto& p : split_params(b.paramsCsv)) {
            fi.parameters.push_back({p.first, TypeInfo{p.second}});
        }
        sym.func_info = fi;

        builtins_.push_back(std::move(sym));
    }
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
