/**
 * =============================================================================
 * ملف: doc_extractor.cpp
 * الوصف: استخراج التوثيق من AST
 * المهمة: T229 - Doc extraction from AST
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 📖 دليل المبتدئ لاستخراج التوثيق
 * ════════════════════════════════════
 * 
 * ما هو AST؟
 * ───────────
 * AST = Abstract Syntax Tree = شجرة بناء الجملة المجردة
 * 
 * هي تمثيل هيكلي للكود. تحول الكود من نص إلى شجرة.
 * 
 * مثال:
 * ```
 * دالة جمع(أ, ب)          ──→   FunctionNode
 *     أرجع أ + ب                    ├─ name: "جمع"
 * نهاية                             ├─ params: [أ, ب]
 *                                   └─ body: ReturnNode
 *                                              └─ AddExpr(أ, ب)
 * ```
 * 
 * لماذا نستخرج من AST؟
 * ─────────────────────
 * 1. نحصل على معلومات دقيقة (أنواع، معطيات)
 * 2. نربط التعليقات بالعناصر الصحيحة
 * 3. نكتشف العلاقات بين العناصر
 * 
 * ماذا نستخرج؟
 * ──────────────
 * - الدوال وتوقيعاتها
 * - الأصناف وخصائصها
 * - الثوابت والمتغيرات
 * - الوحدات والاستيراد
 * - العلاقات والوراثة
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <optional>

namespace sad::docs {

// =============================================================================
// Forward declarations
// =============================================================================

struct DocBlock;  // من doc_comment_parser.cpp

// =============================================================================
// عقد AST المبسطة
// =============================================================================

enum class ASTNodeType {
    Module,
    Function,
    Class,
    Struct,
    Enum,
    Constant,
    Variable,
    Property,
    Method,
    Interface,
    Import,
    TypeAlias
};

struct Parameter {
    std::string name;
    std::string type;
    std::optional<std::string> defaultValue;
    bool isOptional = false;
};

struct ASTNode {
    ASTNodeType type;
    std::string name;
    std::string fullName;           // مسار كامل: وحدة.صنف.دالة
    std::vector<Parameter> parameters;
    std::string returnType;
    std::string visibility;         // عام، خاص، محمي
    bool isStatic = false;
    bool isAsync = false;
    bool isAbstract = false;
    
    // للأصناف
    std::string parentClass;
    std::vector<std::string> interfaces;
    
    // الموقع
    std::string file;
    int startLine = 0;
    int endLine = 0;
    
    // التعليق التوثيقي المرفق
    std::string docComment;
    
    // العناصر الفرعية
    std::vector<std::shared_ptr<ASTNode>> children;
};

// =============================================================================
// عنصر التوثيق المستخرج
// =============================================================================

struct ExtractedDoc {
    // معلومات أساسية
    std::string name;
    std::string fullPath;           // المسار الكامل
    ASTNodeType type;
    std::string module;
    
    // التوقيع
    std::string signature;
    std::vector<Parameter> parameters;
    std::string returnType;
    
    // الوصول
    std::string visibility = "public";
    bool isStatic = false;
    bool isAsync = false;
    
    // التوثيق
    std::string summary;
    std::string description;
    std::map<std::string, std::string> paramDocs;
    std::string returnDoc;
    std::vector<std::string> examples;
    std::vector<std::string> seeAlso;
    std::string since;
    std::string deprecated;
    std::vector<std::string> notes;
    std::vector<std::string> warnings;
    
    // العلاقات
    std::string parentClass;
    std::vector<std::string> interfaces;
    std::vector<std::string> childClasses;  // أصناف مشتقة
    
    // الموقع
    std::string file;
    int line = 0;
    
    // Raw
    std::string sourceCode;
    
    /**
     * توليد التوقيع
     */
    std::string generateSignature() const {
        std::ostringstream ss;
        
        if (isStatic) ss << "ثابت ";
        if (isAsync) ss << "متزامن ";
        
        switch (type) {
            case ASTNodeType::Function:
            case ASTNodeType::Method:
                ss << "دالة " << name << "(";
                for (size_t i = 0; i < parameters.size(); i++) {
                    if (i > 0) ss << ", ";
                    ss << parameters[i].name;
                    if (!parameters[i].type.empty()) {
                        ss << ": " << parameters[i].type;
                    }
                    if (parameters[i].defaultValue) {
                        ss << " = " << *parameters[i].defaultValue;
                    }
                }
                ss << ")";
                if (!returnType.empty()) {
                    ss << " -> " << returnType;
                }
                break;
                
            case ASTNodeType::Class:
                ss << "صنف " << name;
                if (!parentClass.empty()) {
                    ss << " < " << parentClass;
                }
                break;
                
            case ASTNodeType::Struct:
                ss << "هيكل " << name;
                break;
                
            case ASTNodeType::Enum:
                ss << "تعداد " << name;
                break;
                
            case ASTNodeType::Constant:
                ss << "ثابت " << name;
                if (!returnType.empty()) ss << ": " << returnType;
                break;
                
            case ASTNodeType::Variable:
                ss << "متغير " << name;
                if (!returnType.empty()) ss << ": " << returnType;
                break;
                
            default:
                ss << name;
        }
        
        return ss.str();
    }
    
    /**
     * نوع العنصر بالعربي
     */
    std::string getTypeArabic() const {
        switch (type) {
            case ASTNodeType::Module:    return "وحدة";
            case ASTNodeType::Function:  return "دالة";
            case ASTNodeType::Class:     return "صنف";
            case ASTNodeType::Struct:    return "هيكل";
            case ASTNodeType::Enum:      return "تعداد";
            case ASTNodeType::Constant:  return "ثابت";
            case ASTNodeType::Variable:  return "متغير";
            case ASTNodeType::Property:  return "خاصية";
            case ASTNodeType::Method:    return "طريقة";
            case ASTNodeType::Interface: return "واجهة";
            case ASTNodeType::Import:    return "استيراد";
            case ASTNodeType::TypeAlias: return "نوع";
        }
        return "عنصر";
    }
};

// =============================================================================
// مستخرج التوثيق
// =============================================================================

class DocExtractor {
public:
    /**
     * استخراج التوثيق من AST
     */
    std::vector<ExtractedDoc> extract(const std::shared_ptr<ASTNode>& root) {
        std::vector<ExtractedDoc> docs;
        extractFromNode(root, "", docs);
        return docs;
    }
    
    /**
     * استخراج من كود مصدري (يستخدم المحلل)
     */
    std::vector<ExtractedDoc> extractFromSource(
        const std::string& source,
        const std::string& filename
    ) {
        // تحليل الكود إلى AST (مبسط)
        auto ast = parseSimple(source, filename);
        return extract(ast);
    }
    
    /**
     * استخراج قائمة الرموز
     */
    std::vector<std::string> extractSymbols(const std::shared_ptr<ASTNode>& root) {
        std::vector<std::string> symbols;
        extractSymbolsRecursive(root, "", symbols);
        return symbols;
    }
    
private:
    /**
     * استخراج من عقدة
     */
    void extractFromNode(
        const std::shared_ptr<ASTNode>& node,
        const std::string& prefix,
        std::vector<ExtractedDoc>& docs
    ) {
        if (!node) return;
        
        ExtractedDoc doc;
        doc.name = node->name;
        doc.fullPath = prefix.empty() ? node->name : prefix + "." + node->name;
        doc.type = node->type;
        doc.parameters = node->parameters;
        doc.returnType = node->returnType;
        doc.visibility = node->visibility;
        doc.isStatic = node->isStatic;
        doc.isAsync = node->isAsync;
        doc.parentClass = node->parentClass;
        doc.interfaces = node->interfaces;
        doc.file = node->file;
        doc.line = node->startLine;
        
        // معالجة التعليق التوثيقي
        if (!node->docComment.empty()) {
            parseDocComment(node->docComment, doc);
        }
        
        doc.signature = doc.generateSignature();
        
        // إضافة للقائمة
        if (!doc.name.empty()) {
            docs.push_back(doc);
        }
        
        // معالجة العناصر الفرعية
        std::string childPrefix = doc.fullPath;
        for (const auto& child : node->children) {
            extractFromNode(child, childPrefix, docs);
        }
    }
    
    /**
     * تحليل التعليق التوثيقي
     */
    void parseDocComment(const std::string& comment, ExtractedDoc& doc) {
        std::istringstream stream(comment);
        std::string line;
        bool inDescription = false;
        
        while (std::getline(stream, line)) {
            // إزالة ///
            if (line.substr(0, 3) == "///") {
                line = line.substr(3);
                if (!line.empty() && line[0] == ' ') {
                    line = line.substr(1);
                }
            }
            
            // علامات @
            if (!line.empty() && line[0] == '@') {
                parseTag(line, doc);
                inDescription = false;
                continue;
            }
            
            // عنوان
            if (line.substr(0, 2) == "# ") {
                doc.summary = line.substr(2);
                inDescription = true;
                continue;
            }
            
            // ملخص أو وصف
            if (doc.summary.empty() && !line.empty()) {
                doc.summary = line;
            } else if (inDescription && !line.empty()) {
                if (!doc.description.empty()) {
                    doc.description += "\n";
                }
                doc.description += line;
            }
        }
    }
    
    /**
     * تحليل علامة @
     */
    void parseTag(const std::string& line, ExtractedDoc& doc) {
        std::regex paramRegex(R"(@(?:معطى|param)\s+(\S+)\s*(.*))");
        std::regex returnRegex(R"(@(?:أرجع|return)\s*(.*))");
        std::regex seeRegex(R"(@(?:انظر|see)\s+(\S+))");
        std::regex sinceRegex(R"(@(?:منذ|since)\s+(\S+))");
        std::regex deprecatedRegex(R"(@(?:مهمل|deprecated)\s*(.*))");
        std::regex noteRegex(R"(@(?:ملاحظة|note)\s*(.*))");
        std::regex warnRegex(R"(@(?:تحذير|warning)\s*(.*))");
        std::regex exampleRegex(R"(@(?:مثال|example)\s*(.*))");
        
        std::smatch match;
        
        if (std::regex_search(line, match, paramRegex)) {
            doc.paramDocs[match[1]] = match[2];
        }
        else if (std::regex_search(line, match, returnRegex)) {
            doc.returnDoc = match[1];
        }
        else if (std::regex_search(line, match, seeRegex)) {
            doc.seeAlso.push_back(match[1]);
        }
        else if (std::regex_search(line, match, sinceRegex)) {
            doc.since = match[1];
        }
        else if (std::regex_search(line, match, deprecatedRegex)) {
            doc.deprecated = match[1].str().empty() ? "مهمل" : match[1].str();
        }
        else if (std::regex_search(line, match, noteRegex)) {
            doc.notes.push_back(match[1]);
        }
        else if (std::regex_search(line, match, warnRegex)) {
            doc.warnings.push_back(match[1]);
        }
        else if (std::regex_search(line, match, exampleRegex)) {
            doc.examples.push_back(match[1]);
        }
    }
    
    /**
     * استخراج الرموز
     */
    void extractSymbolsRecursive(
        const std::shared_ptr<ASTNode>& node,
        const std::string& prefix,
        std::vector<std::string>& symbols
    ) {
        if (!node) return;
        
        std::string fullName = prefix.empty() ? node->name : prefix + "." + node->name;
        if (!node->name.empty()) {
            symbols.push_back(fullName);
        }
        
        for (const auto& child : node->children) {
            extractSymbolsRecursive(child, fullName, symbols);
        }
    }
    
    /**
     * تحليل مبسط للكود
     */
    std::shared_ptr<ASTNode> parseSimple(
        const std::string& source,
        const std::string& filename
    ) {
        auto root = std::make_shared<ASTNode>();
        root->type = ASTNodeType::Module;
        root->name = filename;
        root->file = filename;
        
        std::istringstream stream(source);
        std::string line;
        int lineNum = 0;
        std::string currentDocComment;
        
        while (std::getline(stream, line)) {
            lineNum++;
            std::string trimmed = trim(line);
            
            // تعليق توثيقي
            if (trimmed.substr(0, 3) == "///") {
                currentDocComment += trimmed + "\n";
                continue;
            }
            
            // دالة
            std::regex funcRegex(R"((?:دالة|function)\s+(\w+)\s*\(([^)]*)\)(?:\s*->\s*(\w+))?)");
            std::smatch match;
            
            if (std::regex_search(trimmed, match, funcRegex)) {
                auto funcNode = std::make_shared<ASTNode>();
                funcNode->type = ASTNodeType::Function;
                funcNode->name = match[1];
                funcNode->returnType = match[3];
                funcNode->file = filename;
                funcNode->startLine = lineNum;
                funcNode->docComment = currentDocComment;
                
                // تحليل المعطيات
                std::string params = match[2];
                parseParameters(params, funcNode->parameters);
                
                root->children.push_back(funcNode);
                currentDocComment.clear();
                continue;
            }
            
            // صنف
            std::regex classRegex(R"((?:صنف|class)\s+(\w+)(?:\s*<\s*(\w+))?)");
            if (std::regex_search(trimmed, match, classRegex)) {
                auto classNode = std::make_shared<ASTNode>();
                classNode->type = ASTNodeType::Class;
                classNode->name = match[1];
                classNode->parentClass = match[2];
                classNode->file = filename;
                classNode->startLine = lineNum;
                classNode->docComment = currentDocComment;
                
                root->children.push_back(classNode);
                currentDocComment.clear();
                continue;
            }
            
            // هيكل
            std::regex structRegex(R"((?:هيكل|struct)\s+(\w+))");
            if (std::regex_search(trimmed, match, structRegex)) {
                auto structNode = std::make_shared<ASTNode>();
                structNode->type = ASTNodeType::Struct;
                structNode->name = match[1];
                structNode->file = filename;
                structNode->startLine = lineNum;
                structNode->docComment = currentDocComment;
                
                root->children.push_back(structNode);
                currentDocComment.clear();
                continue;
            }
            
            // ثابت
            std::regex constRegex(R"((?:ثابت|const)\s+(\w+)(?:\s*:\s*(\w+))?\s*=)");
            if (std::regex_search(trimmed, match, constRegex)) {
                auto constNode = std::make_shared<ASTNode>();
                constNode->type = ASTNodeType::Constant;
                constNode->name = match[1];
                constNode->returnType = match[2];
                constNode->file = filename;
                constNode->startLine = lineNum;
                constNode->docComment = currentDocComment;
                
                root->children.push_back(constNode);
                currentDocComment.clear();
                continue;
            }
            
            // سطر آخر - إعادة تعيين التعليق
            if (!trimmed.empty() && trimmed[0] != '#') {
                currentDocComment.clear();
            }
        }
        
        return root;
    }
    
    /**
     * تحليل المعطيات
     */
    void parseParameters(const std::string& params, std::vector<Parameter>& result) {
        if (params.empty()) return;
        
        std::regex paramRegex(R"((\w+)(?:\s*:\s*(\w+))?(?:\s*=\s*([^,]+))?)");
        std::string::const_iterator searchStart(params.cbegin());
        std::smatch match;
        
        while (std::regex_search(searchStart, params.cend(), match, paramRegex)) {
            Parameter param;
            param.name = match[1];
            param.type = match[2];
            if (match[3].matched) {
                param.defaultValue = match[3];
                param.isOptional = true;
            }
            result.push_back(param);
            searchStart = match.suffix().first;
        }
    }
    
    /**
     * إزالة المسافات
     */
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }
};

// =============================================================================
// بناء فهرس التوثيق
// =============================================================================

class DocIndex {
public:
    void addDoc(const ExtractedDoc& doc) {
        docs_.push_back(doc);
        byName_[doc.name].push_back(docs_.size() - 1);
        byType_[doc.type].push_back(docs_.size() - 1);
        
        if (!doc.module.empty()) {
            byModule_[doc.module].push_back(docs_.size() - 1);
        }
    }
    
    std::vector<const ExtractedDoc*> findByName(const std::string& name) const {
        std::vector<const ExtractedDoc*> result;
        auto it = byName_.find(name);
        if (it != byName_.end()) {
            for (size_t idx : it->second) {
                result.push_back(&docs_[idx]);
            }
        }
        return result;
    }
    
    std::vector<const ExtractedDoc*> findByType(ASTNodeType type) const {
        std::vector<const ExtractedDoc*> result;
        auto it = byType_.find(type);
        if (it != byType_.end()) {
            for (size_t idx : it->second) {
                result.push_back(&docs_[idx]);
            }
        }
        return result;
    }
    
    std::vector<const ExtractedDoc*> getAll() const {
        std::vector<const ExtractedDoc*> result;
        for (const auto& doc : docs_) {
            result.push_back(&doc);
        }
        return result;
    }
    
private:
    std::vector<ExtractedDoc> docs_;
    std::map<std::string, std::vector<size_t>> byName_;
    std::map<ASTNodeType, std::vector<size_t>> byType_;
    std::map<std::string, std::vector<size_t>> byModule_;
};

} // namespace sad::docs

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadDocExtractor {
    sad::docs::DocExtractor* extractor;
    sad::docs::DocIndex* index;
};

SadDocExtractor* sad_doc_extractor_new() {
    auto* ctx = new SadDocExtractor();
    ctx->extractor = new sad::docs::DocExtractor();
    ctx->index = new sad::docs::DocIndex();
    return ctx;
}

void sad_doc_extractor_free(SadDocExtractor* ctx) {
    if (ctx) {
        delete ctx->extractor;
        delete ctx->index;
        delete ctx;
    }
}

int sad_doc_extractor_extract(SadDocExtractor* ctx, const char* source, const char* filename) {
    if (!ctx) return 0;
    auto docs = ctx->extractor->extractFromSource(source, filename);
    for (const auto& doc : docs) {
        ctx->index->addDoc(doc);
    }
    return static_cast<int>(docs.size());
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef DOC_EXTRACTOR_TEST

#include <cassert>

void testExtraction() {
    sad::docs::DocExtractor extractor;
    
    std::string code = R"(
/// # دالة الجمع
/// 
/// تجمع عددين وترجع النتيجة.
/// 
/// @معطى أ العدد الأول
/// @معطى ب العدد الثاني
/// @أرجع مجموع العددين
دالة جمع(أ: عدد, ب: عدد) -> عدد
    أرجع أ + ب
نهاية

/// صنف يمثل نقطة في المستوى
صنف نقطة
    س: عدد
    ص: عدد
نهاية
)";
    
    auto docs = extractor.extractFromSource(code, "test.ص");
    
    assert(docs.size() >= 2);
    
    // التحقق من الدالة
    bool foundFunc = false;
    for (const auto& doc : docs) {
        if (doc.name == "جمع") {
            foundFunc = true;
            assert(doc.type == sad::docs::ASTNodeType::Function);
            assert(doc.parameters.size() == 2);
            assert(doc.summary.find("الجمع") != std::string::npos);
        }
    }
    assert(foundFunc);
    
    std::cout << "✅ اختبار الاستخراج نجح\n";
}

void testIndex() {
    sad::docs::DocExtractor extractor;
    sad::docs::DocIndex index;
    
    std::string code = R"(
دالة أ()
نهاية

دالة ب()
نهاية

صنف ج
نهاية
)";
    
    auto docs = extractor.extractFromSource(code, "test.ص");
    for (const auto& doc : docs) {
        index.addDoc(doc);
    }
    
    auto functions = index.findByType(sad::docs::ASTNodeType::Function);
    auto classes = index.findByType(sad::docs::ASTNodeType::Class);
    
    assert(functions.size() == 2);
    assert(classes.size() == 1);
    
    std::cout << "✅ اختبار الفهرس نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مستخرج التوثيق\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testExtraction();
    testIndex();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // DOC_EXTRACTOR_TEST
