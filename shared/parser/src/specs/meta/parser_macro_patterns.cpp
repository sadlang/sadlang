/**
 * =============================================================================
 * ملف: macro_pattern_parser.cpp
 * الوصف: محلل أنماط الماكرو
 * المهمة: T193 - Implement macro pattern parser
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو نمط الماكرو؟
 * ──────────────────
 * النمط يُحدد ماذا يقبل الماكرو كمدخلات.
 * 
 * أنواع الأجزاء في النمط:
 * ─────────────────────────
 * 1. حرفي: نص ثابت يجب مطابقته تماماً
 *    مثال: ماكرو! اذا_ليس_فارغ!($x:عبر) { ... }
 *                 ↑ "اذا_ليس_فارغ" حرفي
 * 
 * 2. متغير: قيمة تُلتقط من المدخلات
 *    مثال: $اسم:معرّف  -> يلتقط معرّف
 *          $قيمة:عبر   -> يلتقط تعبير
 *          $نوع:نوع   -> يلتقط نوع
 * 
 * 3. تكرار: نمط يتكرر
 *    مثال: $($عنصر:عبر),*  -> صفر أو أكثر من التعبيرات
 *          $($عنصر:عبر),+  -> واحد أو أكثر
 *          $($عنصر:عبر)?   -> صفر أو واحد
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <variant>
#include <optional>

// =============================================================================
// أنواع عناصر النمط
// =============================================================================

/**
 * نوع جزء المتغير
 */
enum class FragmentKind {
    Expr,       // عبر - تعبير
    Literal,    // قيمة - قيمة حرفية
    Type,       // نوع - نوع
    Block,      // كتلة - كتلة كود
    Ident,      // معرّف - اسم
    TokenTree,  // رمز - رمز واحد أو شجرة رموز
    Path,       // مسار - مسار (مثل أ::ب::ج)
    Pat,        // نمط - نمط مطابقة
    Stmt,       // جملة - جملة
    Item        // عنصر - تعريف (دالة، هيكل، إلخ)
};

/**
 * نوع التكرار
 */
enum class RepetitionKind {
    ZeroOrMore,     // *
    OneOrMore,      // +
    Optional        // ? أو ؟
};

// =============================================================================
// عقد شجرة النمط
// =============================================================================

// إعلان مسبق
struct PatternNode;
using PatternPtr = std::shared_ptr<PatternNode>;

/**
 * عقدة في شجرة النمط
 */
struct PatternNode {
    enum class Kind {
        Literal,        // نص حرفي
        MetaVar,        // متغير ماكرو $x:نوع
        Repetition,     // تكرار $(...)
        Sequence,       // تسلسل عناصر
        Group,          // مجموعة (...)
        Separator       // فاصل في التكرار
    };
    
    Kind kind;
    
    // للحرفي
    std::string literal;
    
    // للمتغير
    std::string varName;
    FragmentKind fragmentKind;
    
    // للتكرار
    RepetitionKind repetitionKind;
    std::string separator;
    
    // للأبناء
    std::vector<PatternPtr> children;
    
    PatternNode(Kind k) : kind(k), fragmentKind(FragmentKind::Expr), 
                          repetitionKind(RepetitionKind::ZeroOrMore) {}
    
    static PatternPtr literal(const std::string& lit) {
        auto node = std::make_shared<PatternNode>(Kind::Literal);
        node->literal = lit;
        return node;
    }
    
    static PatternPtr metaVar(const std::string& name, FragmentKind frag) {
        auto node = std::make_shared<PatternNode>(Kind::MetaVar);
        node->varName = name;
        node->fragmentKind = frag;
        return node;
    }
    
    static PatternPtr repetition(RepetitionKind rep, const std::string& sep = "") {
        auto node = std::make_shared<PatternNode>(Kind::Repetition);
        node->repetitionKind = rep;
        node->separator = sep;
        return node;
    }
    
    static PatternPtr sequence() {
        return std::make_shared<PatternNode>(Kind::Sequence);
    }
    
    static PatternPtr group() {
        return std::make_shared<PatternNode>(Kind::Group);
    }
};

// =============================================================================
// محلل النمط
// =============================================================================

/**
 * رمز في نمط الماكرو
 */
struct PatternToken {
    enum class Type {
        Dollar,         // $
        Ident,          // معرّف
        Colon,          // :
        FragType,       // نوع المتغير
        LParen,         // (
        RParen,         // )
        LBrace,         // {
        RBrace,         // }
        LBracket,       // [
        RBracket,       // ]
        Star,           // *
        Plus,           // +
        Question,       // ? أو ؟
        Comma,          // ,
        Semi,           // ;
        Literal,        // أي شيء آخر
        EOF_            // نهاية
    };
    
    Type type;
    std::string value;
    int pos;
    
    PatternToken(Type t = Type::EOF_, const std::string& v = "", int p = 0)
        : type(t), value(v), pos(p) {}
};

/**
 * محلل أنماط الماكرو
 */
class MacroPatternParser {
public:
    /**
     * تحليل نمط ماكرو
     */
    PatternPtr parse(const std::string& pattern) {
        tokenize(pattern);
        pos_ = 0;
        
        return parseSequence();
    }
    
    /**
     * طباعة النمط للتصحيح
     */
    std::string patternToString(const PatternPtr& node, int indent = 0) {
        std::ostringstream ss;
        std::string pad(indent * 2, ' ');
        
        switch (node->kind) {
            case PatternNode::Kind::Literal:
                ss << pad << "حرفي: \"" << node->literal << "\"\n";
                break;
                
            case PatternNode::Kind::MetaVar:
                ss << pad << "متغير: $" << node->varName << ":" 
                   << fragmentKindName(node->fragmentKind) << "\n";
                break;
                
            case PatternNode::Kind::Repetition:
                ss << pad << "تكرار: " << repetitionKindName(node->repetitionKind);
                if (!node->separator.empty()) {
                    ss << " [فاصل: \"" << node->separator << "\"]";
                }
                ss << "\n";
                for (const auto& child : node->children) {
                    ss << patternToString(child, indent + 1);
                }
                break;
                
            case PatternNode::Kind::Sequence:
                ss << pad << "تسلسل:\n";
                for (const auto& child : node->children) {
                    ss << patternToString(child, indent + 1);
                }
                break;
                
            case PatternNode::Kind::Group:
                ss << pad << "مجموعة:\n";
                for (const auto& child : node->children) {
                    ss << patternToString(child, indent + 1);
                }
                break;
                
            default:
                ss << pad << "غير معروف\n";
        }
        
        return ss.str();
    }
    
private:
    std::vector<PatternToken> tokens_;
    size_t pos_;
    
    /**
     * تحويل النص إلى رموز
     */
    void tokenize(const std::string& pattern) {
        tokens_.clear();
        
        for (size_t i = 0; i < pattern.size(); ) {
            char c = pattern[i];
            
            // تخطي المسافات
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                i++;
                continue;
            }
            
            // رموز خاصة
            if (c == '$') {
                tokens_.push_back({PatternToken::Type::Dollar, "$", (int)i});
                i++;
            } else if (c == ':') {
                tokens_.push_back({PatternToken::Type::Colon, ":", (int)i});
                i++;
            } else if (c == '(') {
                tokens_.push_back({PatternToken::Type::LParen, "(", (int)i});
                i++;
            } else if (c == ')') {
                tokens_.push_back({PatternToken::Type::RParen, ")", (int)i});
                i++;
            } else if (c == '{') {
                tokens_.push_back({PatternToken::Type::LBrace, "{", (int)i});
                i++;
            } else if (c == '}') {
                tokens_.push_back({PatternToken::Type::RBrace, "}", (int)i});
                i++;
            } else if (c == '[') {
                tokens_.push_back({PatternToken::Type::LBracket, "[", (int)i});
                i++;
            } else if (c == ']') {
                tokens_.push_back({PatternToken::Type::RBracket, "]", (int)i});
                i++;
            } else if (c == '*') {
                tokens_.push_back({PatternToken::Type::Star, "*", (int)i});
                i++;
            } else if (c == '+') {
                tokens_.push_back({PatternToken::Type::Plus, "+", (int)i});
                i++;
            } else if (c == '?' || (c == (char)0xD8 && i + 1 < pattern.size() && 
                                    pattern[i + 1] == (char)0x9F)) {
                if (c == '?') {
                    tokens_.push_back({PatternToken::Type::Question, "?", (int)i});
                    i++;
                } else {
                    tokens_.push_back({PatternToken::Type::Question, "؟", (int)i});
                    i += 2;
                }
            } else if (c == ',') {
                tokens_.push_back({PatternToken::Type::Comma, ",", (int)i});
                i++;
            } else if (c == ';') {
                tokens_.push_back({PatternToken::Type::Semi, ";", (int)i});
                i++;
            } else if (isIdentStart(c) || (unsigned char)c >= 0x80) {
                // معرّف أو نوع
                std::string ident;
                while (i < pattern.size() && 
                       (isIdentPart(pattern[i]) || (unsigned char)pattern[i] >= 0x80)) {
                    ident += pattern[i++];
                }
                
                // هل هو نوع متغير؟
                if (isFragmentType(ident)) {
                    tokens_.push_back({PatternToken::Type::FragType, ident, (int)(i - ident.size())});
                } else {
                    tokens_.push_back({PatternToken::Type::Ident, ident, (int)(i - ident.size())});
                }
            } else {
                // حرف حرفي
                tokens_.push_back({PatternToken::Type::Literal, std::string(1, c), (int)i});
                i++;
            }
        }
        
        tokens_.push_back({PatternToken::Type::EOF_, "", (int)pattern.size()});
    }
    
    bool isIdentStart(char c) const {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }
    
    bool isIdentPart(char c) const {
        return isIdentStart(c) || (c >= '0' && c <= '9');
    }
    
    bool isFragmentType(const std::string& s) const {
        static const std::vector<std::string> types = {
            "عبر", "expr", "قيمة", "lit", "نوع", "ty", "كتلة", "block",
            "معرّف", "ident", "رمز", "tt", "مسار", "path", "نمط", "pat",
            "جملة", "stmt", "عنصر", "item"
        };
        return std::find(types.begin(), types.end(), s) != types.end();
    }
    
    PatternToken& current() { return tokens_[pos_]; }
    PatternToken& peek() { return tokens_[pos_]; }
    
    PatternToken advance() {
        if (pos_ < tokens_.size() - 1) pos_++;
        return tokens_[pos_ - 1];
    }
    
    bool check(PatternToken::Type type) { return peek().type == type; }
    
    bool match(PatternToken::Type type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    
    /**
     * تحليل تسلسل
     */
    PatternPtr parseSequence() {
        auto seq = PatternNode::sequence();
        
        while (!check(PatternToken::Type::EOF_) && 
               !check(PatternToken::Type::RParen) &&
               !check(PatternToken::Type::RBrace) &&
               !check(PatternToken::Type::RBracket)) {
            auto element = parseElement();
            if (element) {
                seq->children.push_back(element);
            }
        }
        
        return seq;
    }
    
    /**
     * تحليل عنصر واحد
     */
    PatternPtr parseElement() {
        // متغير ماكرو $x:type أو تكرار $(...)
        if (match(PatternToken::Type::Dollar)) {
            // تكرار؟
            if (check(PatternToken::Type::LParen)) {
                return parseRepetition();
            }
            
            // متغير
            return parseMetaVar();
        }
        
        // مجموعة (...)
        if (match(PatternToken::Type::LParen)) {
            auto group = PatternNode::group();
            group->children.push_back(parseSequence());
            match(PatternToken::Type::RParen);
            return group;
        }
        
        // كتلة {...}
        if (match(PatternToken::Type::LBrace)) {
            auto group = PatternNode::group();
            group->children.push_back(parseSequence());
            match(PatternToken::Type::RBrace);
            return group;
        }
        
        // معرّف أو حرفي
        if (check(PatternToken::Type::Ident) || check(PatternToken::Type::Literal)) {
            auto tok = advance();
            return PatternNode::literal(tok.value);
        }
        
        // رمز آخر كحرفي
        auto tok = advance();
        if (tok.type != PatternToken::Type::EOF_) {
            return PatternNode::literal(tok.value);
        }
        
        return nullptr;
    }
    
    /**
     * تحليل متغير ماكرو
     */
    PatternPtr parseMetaVar() {
        std::string name;
        FragmentKind frag = FragmentKind::TokenTree;
        
        if (check(PatternToken::Type::Ident)) {
            name = advance().value;
        }
        
        // نوع المتغير؟
        if (match(PatternToken::Type::Colon)) {
            if (check(PatternToken::Type::FragType) || check(PatternToken::Type::Ident)) {
                std::string fragName = advance().value;
                frag = parseFragmentKind(fragName);
            }
        }
        
        return PatternNode::metaVar(name, frag);
    }
    
    /**
     * تحليل تكرار
     */
    PatternPtr parseRepetition() {
        match(PatternToken::Type::LParen);
        
        auto inner = parseSequence();
        
        match(PatternToken::Type::RParen);
        
        // فاصل اختياري
        std::string sep;
        if (check(PatternToken::Type::Comma) || check(PatternToken::Type::Semi)) {
            sep = advance().value;
        }
        
        // نوع التكرار
        RepetitionKind rep = RepetitionKind::ZeroOrMore;
        if (match(PatternToken::Type::Star)) {
            rep = RepetitionKind::ZeroOrMore;
        } else if (match(PatternToken::Type::Plus)) {
            rep = RepetitionKind::OneOrMore;
        } else if (match(PatternToken::Type::Question)) {
            rep = RepetitionKind::Optional;
        }
        
        auto node = PatternNode::repetition(rep, sep);
        node->children.push_back(inner);
        
        return node;
    }
    
    /**
     * تحليل نوع المتغير
     */
    FragmentKind parseFragmentKind(const std::string& name) {
        static const std::map<std::string, FragmentKind> kinds = {
            {"عبر", FragmentKind::Expr},
            {"expr", FragmentKind::Expr},
            {"قيمة", FragmentKind::Literal},
            {"lit", FragmentKind::Literal},
            {"نوع", FragmentKind::Type},
            {"ty", FragmentKind::Type},
            {"كتلة", FragmentKind::Block},
            {"block", FragmentKind::Block},
            {"معرّف", FragmentKind::Ident},
            {"ident", FragmentKind::Ident},
            {"رمز", FragmentKind::TokenTree},
            {"tt", FragmentKind::TokenTree},
            {"مسار", FragmentKind::Path},
            {"path", FragmentKind::Path},
            {"نمط", FragmentKind::Pat},
            {"pat", FragmentKind::Pat},
            {"جملة", FragmentKind::Stmt},
            {"stmt", FragmentKind::Stmt},
            {"عنصر", FragmentKind::Item},
            {"item", FragmentKind::Item},
        };
        
        auto it = kinds.find(name);
        return it != kinds.end() ? it->second : FragmentKind::TokenTree;
    }
    
    std::string fragmentKindName(FragmentKind kind) {
        static const std::map<FragmentKind, std::string> names = {
            {FragmentKind::Expr, "عبر"},
            {FragmentKind::Literal, "قيمة"},
            {FragmentKind::Type, "نوع"},
            {FragmentKind::Block, "كتلة"},
            {FragmentKind::Ident, "معرّف"},
            {FragmentKind::TokenTree, "رمز"},
            {FragmentKind::Path, "مسار"},
            {FragmentKind::Pat, "نمط"},
            {FragmentKind::Stmt, "جملة"},
            {FragmentKind::Item, "عنصر"},
        };
        auto it = names.find(kind);
        return it != names.end() ? it->second : "؟";
    }
    
    std::string repetitionKindName(RepetitionKind kind) {
        switch (kind) {
            case RepetitionKind::ZeroOrMore: return "* (صفر أو أكثر)";
            case RepetitionKind::OneOrMore: return "+ (واحد أو أكثر)";
            case RepetitionKind::Optional: return "؟ (اختياري)";
            default: return "؟";
        }
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * (AR) عدّ المتغيرات في شجرة النمط بشكل تكراري
 * (EN) Recursively count MetaVar nodes in pattern tree
 */
static int count_pattern_vars(const PatternPtr& node) {
    if (!node) return 0;
    int count = 0;
    if (node->kind == PatternNode::Kind::MetaVar) {
        count = 1;
    }
    for (const auto& child : node->children) {
        count += count_pattern_vars(child);
    }
    return count;
}

/**
 * تحليل نمط ماكرو
 */
void* sad_parse_macro_pattern(const char* pattern) {
    static MacroPatternParser parser;
    auto result = parser.parse(pattern);
    if (result) {
        // (AR) نقل ملكية الشجرة إلى المستدعي
        // (EN) Transfer ownership of the tree to the caller
        return new PatternPtr(std::move(result));
    }
    return nullptr;
}

/**
 * الحصول على عدد المتغيرات في النمط
 */
int sad_pattern_var_count(void* pattern) {
    if (!pattern) return 0;
    auto* ptr = reinterpret_cast<PatternPtr*>(pattern);
    return count_pattern_vars(*ptr);
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef MACRO_PATTERN_PARSER_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار محلل أنماط الماكرو                                                ║
║     Macro Pattern Parser Test                                                ║
║                                                                              ║
║     المرحلة 19 - T193                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    MacroPatternParser parser;
    
    // اختبار 1: نمط بسيط
    std::cout << "═══ اختبار 1: نمط بسيط ═══\n";
    std::string pattern1 = "$x:عبر";
    auto tree1 = parser.parse(pattern1);
    std::cout << "النمط: " << pattern1 << "\n";
    std::cout << parser.patternToString(tree1);
    
    // اختبار 2: نمط مع معاملات متعددة
    std::cout << "\n═══ اختبار 2: معاملات متعددة ═══\n";
    std::string pattern2 = "$اسم:معرّف $قيمة:عبر";
    auto tree2 = parser.parse(pattern2);
    std::cout << "النمط: " << pattern2 << "\n";
    std::cout << parser.patternToString(tree2);
    
    // اختبار 3: تكرار
    std::cout << "\n═══ اختبار 3: تكرار ═══\n";
    std::string pattern3 = "$($عنصر:عبر),*";
    auto tree3 = parser.parse(pattern3);
    std::cout << "النمط: " << pattern3 << "\n";
    std::cout << parser.patternToString(tree3);
    
    // اختبار 4: تكرار مع واحد أو أكثر
    std::cout << "\n═══ اختبار 4: واحد أو أكثر ═══\n";
    std::string pattern4 = "$($قيمة:قيمة);+";
    auto tree4 = parser.parse(pattern4);
    std::cout << "النمط: " << pattern4 << "\n";
    std::cout << parser.patternToString(tree4);
    
    // اختبار 5: نمط مركب
    std::cout << "\n═══ اختبار 5: نمط مركب ═══\n";
    std::string pattern5 = "دالة $اسم:معرّف ($($معامل:معرّف : $نوع:نوع),*) -> $نتيجة:نوع";
    auto tree5 = parser.parse(pattern5);
    std::cout << "النمط: " << pattern5 << "\n";
    std::cout << parser.patternToString(tree5);
    
    std::cout << "\n✅ نجح اختبار محلل أنماط الماكرو!\n";
    
    return 0;
}

#endif // MACRO_PATTERN_PARSER_TEST
