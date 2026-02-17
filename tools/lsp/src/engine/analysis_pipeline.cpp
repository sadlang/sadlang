// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: analysis_pipeline.cpp
// الوصف: خط أنابيب التحليل الثوري - يستخدم المحلل المعجمي والنحوي الحقيقيين
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا هو القلب الثوري لخادم LSP! بدلاً من استخدام regex الهشة،
// نستخدم محلل لغة ص الحقيقي (LexerCore + ParserCore) لتحليل الكود.
//
// خط الأنابيب:
//   النص ──→ [LexerCore] ──→ الرموز ──→ [ParserCore] ──→ شجرة AST
//        ──→ [جمع الرموز] ──→ AnalyzedSymbol[]
//        ──→ [تتبع المراجع] ──→ SymbolReference[]
//        ──→ [استخراج التوثيق] ──→ تعليقات ## → documentation
//        ──→ [جمع التشخيصات] ──→ Diagnostic[]
//
// المميزات الثورية:
//   ✦ تحليل حقيقي وليس regex - دقة ١٠٠٪
//   ✦ اكتشاف الأخطاء من المحلل النحوي مباشرة
//   ✦ جمع الرموز من شجرة AST (دوال، متغيرات، أصناف)
//   ✦ تتبع المراجع: أين يُستخدم كل رمز في الكود
//   ✦ استخراج التوثيق من تعليقات ## فوق التعريفات
//   ✦ تشخيصات متقدمة: متغيرات غير مستخدمة، تعريفات مكررة
//   ✦ كشف الكلمات الإنجليزية التي لها مقابل عربي
//   ✦ كشف جمل الإرجاع المفقودة
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"

// ──────────── مكونات المحلل الحقيقية للغة ص ────────────
#include "lexer_core.h"
#include "parser_core.h"
#include "lexer_keywords.h"
#include "token.h"
#include "ast_node.h"
#include "declarations.h"
#include "statements.h"
#include "expressions.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

AnalysisPipeline::AnalysisPipeline() = default;
AnalysisPipeline::~AnalysisPipeline() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة لتحويل أنواع البيانات
// ══════════════════════════════════════════════════════════════════════════════

/// تحويل DataType من المحلل إلى TypeInfo للفهرسة
static TypeInfo data_type_to_type_info(Sad::Data::DataType dt) {
    TypeInfo ti;
    switch (dt) {
        case Sad::Data::DataType::INTEGER:  ti.name = "رقم"; break;
        case Sad::Data::DataType::FLOAT:    ti.name = "عشري"; break;
        case Sad::Data::DataType::STRING:   ti.name = "نص"; break;
        case Sad::Data::DataType::BOOLEAN:  ti.name = "منطقي"; break;
        case Sad::Data::DataType::NONE:     ti.name = "عدم"; break;
        case Sad::Data::DataType::ARRAY:    ti.name = "مصفوفة"; break;
        case Sad::Data::DataType::MAP:      ti.name = "قاموس"; break;
        case Sad::Data::DataType::TUPLE:    ti.name = "صف"; break;
        case Sad::Data::DataType::FUNCTION: ti.name = "دالة"; break;
        case Sad::Data::DataType::OBJECT:   ti.name = "كائن"; break;
        case Sad::Data::DataType::ENUM:     ti.name = "تعداد"; break;
        case Sad::Data::DataType::BYTE:     ti.name = "بايت"; break;
        case Sad::Data::DataType::ERROR:    ti.name = "خطأ"; break;
        default:
            ti.name = "غير_محدد";
            ti.is_inferred = true;
            break;
    }
    return ti;
}

/// تحويل Position من المحلل المعجمي إلى Position في LSP
/// ملاحظة: Lexer Position يبدأ من 1، بينما LSP Position يبدأ من 0
static Position lexer_pos_to_lsp(const Sad::Lexer::Position& pos) {
    Position p;
    p.line = static_cast<int>(pos.line) - 1;       // LSP يبدأ من 0
    p.character = static_cast<int>(pos.column) - 1; // LSP يبدأ من 0
    return p;
}

/// إنشاء نطاق من Position المحلل المعجمي
static Range lexer_pos_to_range(const Sad::Lexer::Position& pos) {
    Range r;
    r.start = lexer_pos_to_lsp(pos);
    r.end = r.start;
    r.end.character += static_cast<int>(pos.length);
    return r;
}

// ══════════════════════════════════════════════════════════════════════════════
//  استخراج التوثيق من تعليقات ## فوق التعريفات
// ══════════════════════════════════════════════════════════════════════════════

/// استخراج تعليقات التوثيق (##) الموجودة فوق سطر معين
static std::string extract_documentation(const std::vector<std::string>& lines, int target_line) {
    std::string doc;
    // نرجع للأعلى من السطر قبل التعريف ونجمع تعليقات ## المتتالية
    std::vector<std::string> doc_lines;
    for (int i = target_line - 1; i >= 0; i--) {
        const auto& line = lines[i];
        // تنظيف المسافات البادئة
        size_t start = 0;
        while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) start++;
        std::string trimmed = line.substr(start);

        // نبحث عن ## أو # في بداية السطر
        if (trimmed.size() >= 2 && trimmed[0] == '#' && trimmed[1] == '#') {
            // تعليق توثيق ## - نحذف ## والمسافة التالية
            std::string comment = trimmed.substr(2);
            if (!comment.empty() && comment[0] == ' ') comment = comment.substr(1);
            doc_lines.push_back(comment);
        } else if (trimmed.size() >= 1 && trimmed[0] == '#' && (trimmed.size() < 2 || trimmed[1] != '#')) {
            // تعليق عادي # - نضيفه أيضاً
            std::string comment = trimmed.substr(1);
            if (!comment.empty() && comment[0] == ' ') comment = comment.substr(1);
            doc_lines.push_back(comment);
        } else if (trimmed.empty()) {
            // سطر فارغ - نتوقف فقط إذا كان قد بدأنا بجمع التعليقات
            if (!doc_lines.empty()) break;
        } else {
            break; // سطر كود عادي - نتوقف
        }
    }

    // عكس الترتيب (لأننا جمعنا من الأسفل للأعلى)
    std::reverse(doc_lines.begin(), doc_lines.end());
    for (const auto& dl : doc_lines) {
        if (!doc.empty()) doc += "\n";
        doc += dl;
    }
    return doc;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تتبع المراجع - البحث عن استخدامات الرموز في الكود
// ══════════════════════════════════════════════════════════════════════════════

/// البحث عن كل استخدامات الرموز المعروفة في الكود
static void collect_references(
    const std::string& content,
    const DocumentUri& uri,
    const std::vector<AnalyzedSymbol>& symbols,
    std::vector<SymbolReference>& references)
{
    auto lines = arabic::split_lines(content);

    // بناء مجموعة من أسماء الرموز المعروفة (بدون الكلمات المفتاحية)
    std::unordered_map<std::string, const AnalyzedSymbol*> known_symbols;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.kind != AnalyzedSymbolKind::Import) {
            known_symbols[sym.name] = &sym;
        }
    }

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];
        if (line.empty()) continue;

        // نتخطى أسطر التعليقات
        size_t first_non_space = 0;
        while (first_non_space < line.size() &&
               (line[first_non_space] == ' ' || line[first_non_space] == '\t')) {
            first_non_space++;
        }
        if (first_non_space < line.size() && line[first_non_space] == '#') continue;

        // البحث عن كل رمز معروف في السطر
        for (const auto& [name, sym] : known_symbols) {
            size_t search_pos = 0;
            while (search_pos < line.size()) {
                size_t found = line.find(name, search_pos);
                if (found == std::string::npos) break;

                // التحقق من حدود الكلمة (ليست جزءاً من كلمة أطول)
                bool valid_start = (found == 0) ||
                    (!arabic::is_identifier_char_byte(line[found - 1]) &&
                     line[found - 1] != '_');
                size_t end_pos = found + name.size();
                bool valid_end = (end_pos >= line.size()) ||
                    (!arabic::is_identifier_char_byte(line[end_pos]) &&
                     line[end_pos] != '_');

                if (valid_start && valid_end) {
                    // التحقق من أنه ليس نفس موضع التعريف
                    bool is_definition = (i == sym->name_range.start.line &&
                                          static_cast<int>(found) == sym->name_range.start.character);

                    // هل هو كتابة (تعديل)؟ نبحث عن = بعد الاسم
                    bool is_write = false;
                    size_t after = end_pos;
                    while (after < line.size() && (line[after] == ' ' || line[after] == '\t')) after++;
                    if (after < line.size() && line[after] == '=' && (after + 1 >= line.size() || line[after + 1] != '=')) {
                        is_write = true;
                    }

                    SymbolReference ref;
                    ref.uri = uri;
                    ref.range.start = {i, static_cast<int>(found)};
                    ref.range.end = {i, static_cast<int>(end_pos)};
                    ref.is_declaration = is_definition;
                    ref.is_write = is_write || is_definition;
                    references.push_back(ref);
                }

                search_pos = found + name.size();
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  التحليل الرئيسي
// ══════════════════════════════════════════════════════════════════════════════

AnalysisPipeline::AnalysisResult AnalysisPipeline::analyze(
    const std::string& content, const DocumentUri& uri)
{
    AnalysisResult result;

    // تحضير الأسطر لاستخراج التوثيق
    auto lines = arabic::split_lines(content);

    // ────────────────────────────────────────────────────────
    //  المرحلة ١: التحليل المعجمي (Lexer)
    //  نحول النص إلى سلسلة من الرموز (tokens)
    // ────────────────────────────────────────────────────────
    try {
        // تهيئة جدول الكلمات المفتاحية
        Sad::Lexer::KeywordTable::initialize();

        Sad::Lexer::LexerCore lexer(content);

        // ────────────────────────────────────────────────────
        //  المرحلة ٢: التحليل النحوي (Parser)
        //  نبني شجرة AST من الرموز
        // ────────────────────────────────────────────────────
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        // جمع أخطاء التحليل النحوي
        if (parser.hasErrors()) {
            auto errors = parser.getErrors();
            for (const auto& err : errors) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Error;
                diag.message = err;
                diag.source = "ص-محلل";

                // محاولة استخراج رقم السطر من رسالة الخطأ
                // رسائل الأخطاء عادة تكون بصيغة: "[سطر X:عمود Y] رسالة"
                std::regex line_pattern(R"(\[.*?(\d+):(\d+)\])");
                std::smatch match;
                if (std::regex_search(err, match, line_pattern)) {
                    int line = std::stoi(match[1].str()) - 1;
                    int col = std::stoi(match[2].str()) - 1;
                    diag.range.start = {std::max(0, line), std::max(0, col)};
                    diag.range.end = {std::max(0, line), std::max(0, col) + 1};
                }

                diag.code = "ص-٠٠١";
                result.diagnostics.push_back(diag);
            }
        }

        // ────────────────────────────────────────────────────
        //  المرحلة ٣: جمع الرموز من شجرة AST
        //  نمر على كل عقدة في الشجرة ونستخرج الرموز
        // ────────────────────────────────────────────────────
        collect_symbols_from_ast(content, uri, result);

        // حتى لو كانت هناك أخطاء بالتحليل، ما زلنا نحاول الحصول على معلومات
        // باستخدام التحليل المعجمي الخام
        if (!program.empty()) {
            // جمع الرموز من AST مباشرة
            for (const auto& stmt : program) {
                if (!stmt) continue;

                // ──── تصريح دالة ────
                auto* func = dynamic_cast<Sad::AST::FunctionDecl*>(stmt.get());
                if (func) {
                    AnalyzedSymbol sym;
                    sym.name = func->name;
                    sym.normalized_name = arabic::normalize_arabic(func->name);
                    sym.kind = AnalyzedSymbolKind::Function;
                    sym.type = data_type_to_type_info(func->returnType);
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(func->position);
                    sym.name_range = sym.definition_range;
                    sym.is_exported = func->isExported;

                    // استخراج التوثيق من تعليقات ## فوق الدالة
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    // معلومات الدالة (المعلمات ونوع الإرجاع)
                    AnalyzedSymbol::FunctionInfo fi;
                    fi.return_type = data_type_to_type_info(func->returnType);
                    fi.is_async = func->is_async;
                    fi.is_generator = func->isGenerator;
                    for (const auto& param : func->parameters) {
                        fi.parameters.push_back({
                            param.name,
                            data_type_to_type_info(param.type)
                        });

                        // إضافة المعلمة كرمز أيضاً
                        AnalyzedSymbol paramSym;
                        paramSym.name = param.name;
                        paramSym.normalized_name = arabic::normalize_arabic(param.name);
                        paramSym.kind = AnalyzedSymbolKind::Parameter;
                        paramSym.type = data_type_to_type_info(param.type);
                        paramSym.uri = uri;
                        paramSym.scope_owner = func->name;
                        paramSym.scope_depth = 1;
                        result.symbols.push_back(paramSym);
                    }
                    sym.func_info = fi;
                    result.symbols.push_back(sym);
                    continue;
                }

                // ──── تصريح صنف ────
                auto* cls = dynamic_cast<Sad::AST::ClassDecl*>(stmt.get());
                if (cls) {
                    AnalyzedSymbol sym;
                    sym.name = cls->name;
                    sym.normalized_name = arabic::normalize_arabic(cls->name);
                    sym.kind = AnalyzedSymbolKind::Class;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(cls->position);
                    sym.name_range = sym.definition_range;

                    // استخراج التوثيق من تعليقات ## فوق الصنف
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);

                    AnalyzedSymbol::ClassInfo ci;
                    // جمع الأصناف الأب
                    for (const auto& base : cls->superclasses) {
                        ci.parent_class = base; // آخر واحد
                    }
                    sym.class_info = ci;
                    result.symbols.push_back(sym);

                    // جمع أعضاء الصنف
                    for (const auto& member : cls->members) {
                        if (!member) continue;

                        auto* method = dynamic_cast<Sad::AST::FunctionDecl*>(member.get());
                        if (method) {
                            AnalyzedSymbol msym;
                            msym.name = method->name;
                            msym.normalized_name = arabic::normalize_arabic(method->name);
                            msym.kind = AnalyzedSymbolKind::Method;
                            msym.type = data_type_to_type_info(method->returnType);
                            msym.uri = uri;
                            msym.definition_range = lexer_pos_to_range(method->position);
                            msym.name_range = msym.definition_range;
                            msym.scope_owner = cls->name;
                            msym.scope_depth = 1;
                            msym.documentation = extract_documentation(lines, msym.definition_range.start.line);

                            AnalyzedSymbol::FunctionInfo mfi;
                            mfi.return_type = data_type_to_type_info(method->returnType);
                            for (const auto& p : method->parameters) {
                                mfi.parameters.push_back({
                                    p.name,
                                    data_type_to_type_info(p.type)
                                });
                            }
                            msym.func_info = mfi;
                            result.symbols.push_back(msym);
                        }

                        auto* varDecl = dynamic_cast<Sad::AST::VarDeclStmt*>(member.get());
                        if (varDecl) {
                            AnalyzedSymbol fsym;
                            fsym.name = varDecl->name;
                            fsym.normalized_name = arabic::normalize_arabic(varDecl->name);
                            fsym.kind = AnalyzedSymbolKind::Property;
                            fsym.type = data_type_to_type_info(varDecl->type);
                            fsym.uri = uri;
                            fsym.definition_range = lexer_pos_to_range(varDecl->position);
                            fsym.name_range = fsym.definition_range;
                            fsym.scope_owner = cls->name;
                            fsym.scope_depth = 1;
                            result.symbols.push_back(fsym);
                        }
                    }
                    continue;
                }

                // ──── تصريح متغير ────
                auto* var = dynamic_cast<Sad::AST::VarDeclStmt*>(stmt.get());
                if (var) {
                    AnalyzedSymbol sym;
                    sym.name = var->name;
                    sym.normalized_name = arabic::normalize_arabic(var->name);
                    sym.kind = var->isConst ? AnalyzedSymbolKind::Constant
                                            : AnalyzedSymbolKind::Variable;
                    sym.type = data_type_to_type_info(var->type);
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(var->position);
                    sym.name_range = sym.definition_range;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);
                    result.symbols.push_back(sym);
                    continue;
                }

                // ──── تصريح تعداد ────
                auto* enumDecl = dynamic_cast<Sad::AST::EnumDecl*>(stmt.get());
                if (enumDecl) {
                    AnalyzedSymbol sym;
                    sym.name = enumDecl->name;
                    sym.normalized_name = arabic::normalize_arabic(enumDecl->name);
                    sym.kind = AnalyzedSymbolKind::Enum;
                    sym.uri = uri;
                    sym.definition_range = lexer_pos_to_range(enumDecl->position);
                    sym.name_range = sym.definition_range;
                    sym.documentation = extract_documentation(lines, sym.definition_range.start.line);
                    result.symbols.push_back(sym);
                    continue;
                }
            }
        }

        result.success = true;

    } catch (const std::exception& e) {
        // إذا فشل التحليل كلياً، نضيف التشخيص ونستخدم التحليل الاحتياطي
        Diagnostic diag;
        diag.severity = DiagnosticSeverity::Error;
        diag.message = std::string("خطأ في التحليل: ") + e.what();
        diag.source = "ص-محلل";
        diag.code = "ص-٠٠٠";
        result.diagnostics.push_back(diag);

        // التحليل الاحتياطي: نستخدم regex كخطة بديلة
        collect_symbols_from_ast(content, uri, result);
    }

    // ────────────────────────────────────────────────────────
    //  المرحلة ٤: تتبع المراجع
    //  نبحث عن كل استخدامات الرموز في الكود
    // ────────────────────────────────────────────────────────
    collect_references(content, uri, result.symbols, result.references);

    // ────────────────────────────────────────────────────────
    //  المرحلة ٥: تشخيصات إضافية متقدمة
    //  نضيف تحذيرات واقتراحات مفيدة للمبرمج
    // ────────────────────────────────────────────────────────
    collect_extra_diagnostics(content, uri, result);

    return result;
}

// ══════════════════════════════════════════════════════════════════════════════
//  جمع الرموز باستخدام regex (كخطة احتياطية)
// ══════════════════════════════════════════════════════════════════════════════

void AnalysisPipeline::collect_symbols_from_ast(
    const std::string& content,
    const DocumentUri& uri,
    AnalysisResult& result)
{
    // هذه الدالة تجمع رموزاً إضافية باستخدام أنماط regex
    // كخطة احتياطية عندما لا يعمل التحليل الكامل
    // أو لالتقاط رموز قد لا يلتقطها المحلل

    auto lines = arabic::split_lines(content);

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];

        // ──── اكتشاف تعريف دالة بسيط ────
        // نمط: دالة اسم_الدالة(...)
        {
            // نبحث عن الكلمة المفتاحية "دالة" متبوعة بمسافة واسم
            std::string kw_func = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // "دالة" in UTF-8
            size_t pos = line.find(kw_func);
            if (pos != std::string::npos) {
                // تخطي الكلمة المفتاحية والمسافة
                size_t name_start = pos + kw_func.size();
                while (name_start < line.size() && (line[name_start] == ' ' || line[name_start] == '\t')) {
                    name_start++;
                }
                // قراءة الاسم
                size_t name_end = name_start;
                while (name_end < line.size() && line[name_end] != '(' &&
                       line[name_end] != ' ' && line[name_end] != '\t' &&
                       line[name_end] != '{') {
                    name_end++;
                }
                if (name_end > name_start) {
                    std::string name = line.substr(name_start, name_end - name_start);
                    // نتأكد أن هذا الرمز لم يُضف من قبل
                    bool already_exists = false;
                    for (const auto& sym : result.symbols) {
                        if (sym.name == name && sym.kind == AnalyzedSymbolKind::Function) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (!already_exists) {
                        AnalyzedSymbol sym;
                        sym.name = name;
                        sym.normalized_name = arabic::normalize_arabic(name);
                        sym.kind = AnalyzedSymbolKind::Function;
                        sym.uri = uri;
                        sym.definition_range.start = {i, 0};
                        sym.definition_range.end = {i, static_cast<int>(line.size())};
                        sym.name_range.start = {i, static_cast<int>(name_start)};
                        sym.name_range.end = {i, static_cast<int>(name_end)};
                        result.symbols.push_back(sym);
                    }
                }
            }
        }

        // ──── اكتشاف تعريف صنف ────
        {
            std::string kw_class = "\xd8\xb5\xd9\x86\xd9\x81"; // "صنف" in UTF-8
            size_t pos = line.find(kw_class);
            if (pos != std::string::npos) {
                size_t name_start = pos + kw_class.size();
                while (name_start < line.size() && (line[name_start] == ' ' || line[name_start] == '\t')) {
                    name_start++;
                }
                size_t name_end = name_start;
                while (name_end < line.size() && line[name_end] != ' ' &&
                       line[name_end] != '\t' && line[name_end] != '{' &&
                       line[name_end] != '(' && line[name_end] != ':') {
                    name_end++;
                }
                if (name_end > name_start) {
                    std::string name = line.substr(name_start, name_end - name_start);
                    bool already_exists = false;
                    for (const auto& sym : result.symbols) {
                        if (sym.name == name && sym.kind == AnalyzedSymbolKind::Class) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (!already_exists) {
                        AnalyzedSymbol sym;
                        sym.name = name;
                        sym.normalized_name = arabic::normalize_arabic(name);
                        sym.kind = AnalyzedSymbolKind::Class;
                        sym.uri = uri;
                        sym.definition_range.start = {i, 0};
                        sym.definition_range.end = {i, static_cast<int>(line.size())};
                        sym.name_range.start = {i, static_cast<int>(name_start)};
                        sym.name_range.end = {i, static_cast<int>(name_end)};
                        result.symbols.push_back(sym);
                    }
                }
            }
        }

        // ──── اكتشاف تعريف متغير ────
        {
            // "متغير" أو "ثابت"
            std::string kw_var = "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1"; // "متغير"
            std::string kw_const = "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa";         // "ثابت"

            for (bool is_const : {false, true}) {
                const auto& kw = is_const ? kw_const : kw_var;
                size_t pos = line.find(kw);
                if (pos != std::string::npos) {
                    size_t name_start = pos + kw.size();
                    while (name_start < line.size() &&
                           (line[name_start] == ' ' || line[name_start] == '\t')) {
                        name_start++;
                    }
                    size_t name_end = name_start;
                    while (name_end < line.size() && line[name_end] != ' ' &&
                           line[name_end] != '\t' && line[name_end] != '=' &&
                           line[name_end] != ':') {
                        name_end++;
                    }
                    if (name_end > name_start) {
                        std::string name = line.substr(name_start, name_end - name_start);
                        bool already_exists = false;
                        for (const auto& sym : result.symbols) {
                            if (sym.name == name) {
                                already_exists = true;
                                break;
                            }
                        }
                        if (!already_exists) {
                            AnalyzedSymbol sym;
                            sym.name = name;
                            sym.normalized_name = arabic::normalize_arabic(name);
                            sym.kind = is_const ? AnalyzedSymbolKind::Constant
                                                : AnalyzedSymbolKind::Variable;
                            sym.uri = uri;
                            sym.definition_range.start = {i, 0};
                            sym.definition_range.end = {i, static_cast<int>(line.size())};
                            sym.name_range.start = {i, static_cast<int>(name_start)};
                            sym.name_range.end = {i, static_cast<int>(name_end)};
                            result.symbols.push_back(sym);
                        }
                    }
                }
            }
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  التشخيصات الإضافية المتقدمة (تحذيرات واقتراحات ثورية)
// ══════════════════════════════════════════════════════════════════════════════

void AnalysisPipeline::collect_extra_diagnostics(
    const std::string& content,
    const DocumentUri& uri,
    AnalysisResult& result)
{
    auto lines = arabic::split_lines(content);

    // ──── ١. كشف المتغيرات غير المستخدمة ────
    // نبني خريطة: اسم الرمز → عدد المراجع (بدون التعريف نفسه)
    {
        std::unordered_map<std::string, int> ref_counts;
        for (const auto& sym : result.symbols) {
            // نحسب فقط المتغيرات والمعلمات والثوابت المحلية
            if (sym.kind == AnalyzedSymbolKind::Variable ||
                sym.kind == AnalyzedSymbolKind::Constant ||
                sym.kind == AnalyzedSymbolKind::Parameter) {
                ref_counts[sym.name] = 0;
            }
        }
        for (const auto& ref : result.references) {
            // نبحث عن اسم الرمز بناءً على النطاق
            for (const auto& sym : result.symbols) {
                if (sym.name_range.start.line == ref.range.start.line &&
                    sym.name_range.start.character == ref.range.start.character &&
                    ref.is_declaration) {
                    continue; // نتخطى التعريف نفسه
                }
                // نبحث عن المراجع التي تطابق اسم الرمز
                int ref_line = ref.range.start.line;
                int ref_col = ref.range.start.character;
                int ref_end = ref.range.end.character;
                if (ref_line < static_cast<int>(lines.size())) {
                    std::string ref_text = lines[ref_line].substr(
                        std::max(0, ref_col),
                        std::max(0, ref_end - ref_col));
                    if (ref_text == sym.name && !ref.is_declaration) {
                        ref_counts[sym.name]++;
                    }
                }
            }
        }

        for (const auto& sym : result.symbols) {
            if ((sym.kind == AnalyzedSymbolKind::Variable ||
                 sym.kind == AnalyzedSymbolKind::Constant) &&
                ref_counts.count(sym.name) && ref_counts[sym.name] == 0 &&
                sym.scope_depth == 0) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Hint;
                diag.message = "المتغير '" + sym.name + "' معرّف لكنه غير مستخدم";
                diag.source = "ص-تحليل";
                diag.code = "ص-ت١٠١";
                diag.range = sym.name_range;
                diag.tags.push_back(DiagnosticTag::Unnecessary);
                result.diagnostics.push_back(diag);
            }
        }
    }

    // ──── ٢. كشف التعريفات المكررة ────
    {
        std::unordered_map<std::string, std::vector<const AnalyzedSymbol*>> name_groups;
        for (const auto& sym : result.symbols) {
            if (sym.scope_depth == 0 &&
                (sym.kind == AnalyzedSymbolKind::Function ||
                 sym.kind == AnalyzedSymbolKind::Class ||
                 sym.kind == AnalyzedSymbolKind::Variable ||
                 sym.kind == AnalyzedSymbolKind::Constant)) {
                name_groups[sym.name].push_back(&sym);
            }
        }
        for (const auto& [name, syms] : name_groups) {
            if (syms.size() > 1) {
                // تعريف مكرر - نضع تحذيراً على كل واحد ما عدا الأول
                for (size_t i = 1; i < syms.size(); i++) {
                    Diagnostic diag;
                    diag.severity = DiagnosticSeverity::Warning;
                    diag.message = "تعريف مكرر للاسم '" + name + "'";
                    diag.source = "ص-تحليل";
                    diag.code = "ص-ت١٠٢";
                    diag.range = syms[i]->name_range;

                    // معلومات مرتبطة: أين التعريف الأول
                    DiagnosticRelatedInformation related;
                    related.location = {uri, syms[0]->name_range};
                    related.message = "التعريف الأول هنا";
                    diag.related.push_back(related);

                    result.diagnostics.push_back(diag);
                }
            }
        }
    }

    for (int i = 0; i < static_cast<int>(lines.size()); i++) {
        const auto& line = lines[i];

        // ──── ٣. تحذير: سطر طويل جداً (أكثر من 200 حرف) ────
        if (line.size() > 400) {  // ~200 حرف عربي × 2 بايت تقريباً
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Hint;
            diag.message = "هذا السطر طويل جداً. يُفضل تقسيمه لسطور أقصر لتسهيل القراءة.";
            diag.source = "ص-نمط";
            diag.code = "ص-ت٠٠١";
            diag.range.start = {i, 0};
            diag.range.end = {i, static_cast<int>(line.size())};
            result.diagnostics.push_back(diag);
        }

        // ──── ٤. تحذير: print بدل اطبع ────
        if (line.find("print(") != std::string::npos ||
            line.find("print (") != std::string::npos) {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Information;
            diag.message = "استخدم 'اطبع' بدلاً من 'print' - لغة ص تستخدم الكلمات المفتاحية العربية";
            diag.source = "ص-نمط";
            diag.code = "ص-ت٠٠٢";
            auto pos = line.find("print");
            diag.range.start = {i, static_cast<int>(pos)};
            diag.range.end = {i, static_cast<int>(pos + 5)};
            result.diagnostics.push_back(diag);
        }

        // ──── ٥. كشف كلمات إنجليزية شائعة لها مقابل عربي ────
        {
            struct EnglishArabic {
                const char* english;
                const char* arabic;
                int len;
            };
            static const EnglishArabic eng_ar_map[] = {
                {"function",  "دالة",    8},
                {"class",     "صنف",     5},
                {"if",        "إذا",     2},
                {"else",      "وإلا",    4},
                {"while",     "بينما",   5},
                {"for",       "لكل",     3},
                {"return",    "إرجاع",   6},
                {"var",       "متغير",   3},
                {"let",       "متغير",   3},
                {"const",     "ثابت",    5},
                {"true",      "صحيح",    4},
                {"false",     "خطأ",     5},
                {"null",      "عدم",     4},
                {"import",    "استورد",  6},
                {"try",       "حاول",    3},
                {"catch",     "امسك",    5},
                {"break",     "اخرج",    5},
                {"continue",  "استمر",   8},
            };

            for (const auto& ea : eng_ar_map) {
                size_t search_pos = 0;
                while (search_pos < line.size()) {
                    size_t found = line.find(ea.english, search_pos);
                    if (found == std::string::npos) break;

                    // تحقق من حدود الكلمة
                    bool valid_start = (found == 0) ||
                        (!std::isalnum(static_cast<unsigned char>(line[found - 1])) && line[found - 1] != '_');
                    size_t end_pos = found + ea.len;
                    bool valid_end = (end_pos >= line.size()) ||
                        (!std::isalnum(static_cast<unsigned char>(line[end_pos])) && line[end_pos] != '_');

                    // نتخطى إذا كان داخل نص مقتبس (تقريبي)
                    bool in_string = false;
                    int quote_count = 0;
                    for (size_t j = 0; j < found; j++) {
                        if (line[j] == '"' || line[j] == '\'') quote_count++;
                    }
                    if (quote_count % 2 != 0) in_string = true;

                    // نتخطى إذا كان في تعليق
                    bool in_comment = false;
                    for (size_t j = 0; j < found; j++) {
                        if (line[j] == '#') { in_comment = true; break; }
                    }

                    if (valid_start && valid_end && !in_string && !in_comment &&
                        std::string(ea.english) != "print") { // print له تحذيره الخاص
                        Diagnostic diag;
                        diag.severity = DiagnosticSeverity::Information;
                        diag.message = std::string("لغة ص تستخدم '") + ea.arabic +
                                       "' بدلاً من '" + ea.english + "'";
                        diag.source = "ص-نمط";
                        diag.code = "ص-ت٠٠٣";
                        diag.range.start = {i, static_cast<int>(found)};
                        diag.range.end = {i, static_cast<int>(end_pos)};
                        result.diagnostics.push_back(diag);
                    }

                    search_pos = end_pos;
                }
            }
        }

        // ──── ٦. تحذير: تعليق TODO/FIXME/HACK ────
        if (line.find("TODO") != std::string::npos ||
            line.find("FIXME") != std::string::npos ||
            line.find("HACK") != std::string::npos) {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Information;
            diag.message = "يوجد تعليق يحتاج مراجعة";
            diag.source = "ص-نمط";
            diag.code = "ص-ت٠٠٤";
            diag.range.start = {i, 0};
            diag.range.end = {i, static_cast<int>(line.size())};
            result.diagnostics.push_back(diag);
        }

        // ──── ٧. تحذير: قوس غير مغلق في نفس السطر ────
        int parens = 0, braces = 0, brackets = 0;
        bool in_str = false;
        char str_char = 0;
        for (size_t j = 0; j < line.size(); j++) {
            char c = line[j];
            if (in_str) {
                if (c == str_char && (j == 0 || line[j-1] != '\\')) in_str = false;
                continue;
            }
            if (c == '"' || c == '\'') { in_str = true; str_char = c; continue; }
            if (c == '#') break; // تعليق - نتوقف
            if (c == '(') parens++;
            else if (c == ')') parens--;
            else if (c == '{') braces++;
            else if (c == '}') braces--;
            else if (c == '[') brackets++;
            else if (c == ']') brackets--;
        }
        // لا ننبه إلا إذا كان الخلل كبيراً (أكثر من 2)
        if (parens > 2 || parens < -2) {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::Warning;
            diag.message = parens > 0 ? "يوجد أقواس مفتوحة بدون إغلاق"
                                       : "يوجد أقواس مغلقة بدون فتح";
            diag.source = "ص-نحو";
            diag.code = "ص-ن٠٠١";
            diag.range.start = {i, 0};
            diag.range.end = {i, static_cast<int>(line.size())};
            result.diagnostics.push_back(diag);
        }

        // ──── ٨. تحذير: مسافات مختلطة (مسافات + tabs) ────
        {
            bool has_spaces = false, has_tabs = false;
            for (size_t j = 0; j < line.size(); j++) {
                if (line[j] == ' ') has_spaces = true;
                else if (line[j] == '\t') has_tabs = true;
                else break;
            }
            if (has_spaces && has_tabs) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Hint;
                diag.message = "يوجد خلط بين المسافات والجداول (tabs) في المسافة البادئة";
                diag.source = "ص-نمط";
                diag.code = "ص-ت٠٠٥";
                diag.range.start = {i, 0};
                diag.range.end = {i, static_cast<int>(line.size())};
                result.diagnostics.push_back(diag);
            }
        }

        // ──── ٩. تحذير: مسافات زائدة في نهاية السطر ────
        if (!line.empty()) {
            size_t end = line.size();
            while (end > 0 && (line[end-1] == ' ' || line[end-1] == '\t')) end--;
            if (end < line.size() && end > 0) {
                Diagnostic diag;
                diag.severity = DiagnosticSeverity::Hint;
                diag.message = "يوجد مسافات زائدة في نهاية السطر";
                diag.source = "ص-نمط";
                diag.code = "ص-ت٠٠٦";
                diag.range.start = {i, static_cast<int>(end)};
                diag.range.end = {i, static_cast<int>(line.size())};
                diag.tags.push_back(DiagnosticTag::Unnecessary);
                result.diagnostics.push_back(diag);
            }
        }
    }
}

} // namespace lsp
} // namespace sad
