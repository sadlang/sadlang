/**
 * =============================================================================
 * ملف: lifetime_ast.cpp
 * الوصف: عقد AST لتعليقات العمر
 * المهمة: T203 - Implement lifetime AST nodes
 * المرحلة: Phase 20 - User Story 17 (Lifetime Annotations)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي شجرة AST؟
 * ────────────────
 * AST (Abstract Syntax Tree) هي تمثيل هيكلي للكود.
 * 
 * مثال:
 * 
 * الكود:
 *   دالة أطول<'أ>(س: &'أ نص, ص: &'أ نص) -> &'أ نص
 * 
 * AST:
 *   FunctionDecl
 *   ├── name: "أطول"
 *   ├── lifetimes: ['أ]
 *   ├── params:
 *   │   ├── Param(س, RefType(&'أ نص))
 *   │   └── Param(ص, RefType(&'أ نص))
 *   └── return: RefType(&'أ نص)
 * 
 * لماذا نحتاج عقد العمر في AST؟
 * ─────────────────────────────
 * 1. تمثيل العلاقات بين الأعمار
 * 2. تمكين التحقق من الأعمار
 * 3. تمرير معلومات العمر لمراحل الترجمة التالية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <variant>
#include <optional>
#include <sstream>
#include <iostream>
#include <functional>

namespace sad::ast::lifetime {

// =============================================================================
// موقع المصدر
// =============================================================================

/**
 * نطاق في الكود المصدري
 */
struct Span {
    std::string file;
    size_t startLine;
    size_t startCol;
    size_t endLine;
    size_t endCol;
    
    Span() : startLine(0), startCol(0), endLine(0), endCol(0) {}
    
    Span(size_t line, size_t col)
        : startLine(line), startCol(col), endLine(line), endCol(col) {}
    
    Span(size_t sLine, size_t sCol, size_t eLine, size_t eCol)
        : startLine(sLine), startCol(sCol), endLine(eLine), endCol(eCol) {}
    
    std::string toString() const {
        std::ostringstream ss;
        if (!file.empty()) ss << file << ":";
        ss << startLine << ":" << startCol;
        return ss.str();
    }
};

// =============================================================================
// عقد العمر الأساسية
// =============================================================================

/**
 * نوع عقدة AST
 */
enum class ASTNodeKind {
    // أعمار
    LifetimeParam,      // معامل عمر في قائمة عامة
    LifetimeBound,      // قيد عمر 'أ: 'ب
    LifetimeRef,        // مرجع بعمر &'أ
    
    // أنواع
    RefType,            // نوع مرجعي
    GenericType,        // نوع عام مع أعمار
    FunctionType,       // نوع دالة
    
    // تعريفات
    FunctionDecl,       // تعريف دالة
    StructDecl,         // تعريف بنية
    ImplDecl,           // كتلة تنفيذ
    TraitDecl,          // تعريف سمة
    
    // تعبيرات
    BorrowExpr,         // تعبير استعارة &x
    DerefExpr,          // تعبير فك المرجع *x
};

/**
 * عقدة AST أساسية
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual ASTNodeKind kind() const = 0;
    virtual std::string toString() const = 0;
    
    Span span;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

// =============================================================================
// عقد العمر
// =============================================================================

/**
 * نوع العمر
 */
enum class LifetimeKind {
    Named,      // 'أ
    Static,     // 'ثابت
    Anonymous,  // '_
    Elided,     // عمر محذوف (مستنتج)
};

/**
 * عقدة معامل عمر
 * 
 * في: <'أ, 'ب>
 * العقد: LifetimeParam("أ"), LifetimeParam("ب")
 */
class LifetimeParam : public ASTNode {
public:
    std::string name;
    LifetimeKind lifetimeKind;
    
    LifetimeParam(const std::string& n, LifetimeKind k = LifetimeKind::Named)
        : name(n), lifetimeKind(k) {}
    
    ASTNodeKind kind() const override { return ASTNodeKind::LifetimeParam; }
    
    std::string toString() const override {
        switch (lifetimeKind) {
            case LifetimeKind::Named: return "'" + name;
            case LifetimeKind::Static: return "'ثابت";
            case LifetimeKind::Anonymous: return "'_";
            case LifetimeKind::Elided: return "";
        }
        return "";
    }
    
    bool isStatic() const { return lifetimeKind == LifetimeKind::Static; }
    bool isAnonymous() const { return lifetimeKind == LifetimeKind::Anonymous; }
    bool isElided() const { return lifetimeKind == LifetimeKind::Elided; }
    
    bool operator==(const LifetimeParam& other) const {
        return name == other.name && lifetimeKind == other.lifetimeKind;
    }
};

using LifetimeParamPtr = std::shared_ptr<LifetimeParam>;

/**
 * قيد عمر
 * 
 * 'أ: 'ب يعني: العمر 'أ يعيش على الأقل بقدر 'ب
 */
class LifetimeBound : public ASTNode {
public:
    LifetimeParamPtr longer;  // العمر الأطول
    LifetimeParamPtr shorter; // العمر الأقصر (أو 'ثابت)
    
    LifetimeBound(LifetimeParamPtr l, LifetimeParamPtr s)
        : longer(l), shorter(s) {}
    
    ASTNodeKind kind() const override { return ASTNodeKind::LifetimeBound; }
    
    std::string toString() const override {
        return longer->toString() + ": " + shorter->toString();
    }
};

using LifetimeBoundPtr = std::shared_ptr<LifetimeBound>;

// =============================================================================
// عقد الأنواع
// =============================================================================

/**
 * نوع مرجعي مع عمر
 * 
 * مثال: &'أ نص  أو  &'أ متغير صحيح
 */
class RefType : public ASTNode {
public:
    LifetimeParamPtr lifetime;
    bool isMutable;
    ASTNodePtr innerType;
    std::string typeName;  // للأنواع البسيطة
    
    RefType(LifetimeParamPtr lt, bool mut, const std::string& type)
        : lifetime(lt), isMutable(mut), typeName(type) {}
    
    ASTNodeKind kind() const override { return ASTNodeKind::RefType; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << "&";
        if (lifetime && !lifetime->isElided()) {
            ss << lifetime->toString() << " ";
        }
        if (isMutable) ss << "متغير ";
        ss << typeName;
        return ss.str();
    }
};

using RefTypePtr = std::shared_ptr<RefType>;

/**
 * نوع عام مع معاملات عمر
 * 
 * مثال: قائمة<'أ, عنصر>  أو  خريطة<'أ, 'ب, مفتاح, قيمة>
 */
class GenericType : public ASTNode {
public:
    std::string name;
    std::vector<LifetimeParamPtr> lifetimes;
    std::vector<ASTNodePtr> typeParams;
    
    GenericType(const std::string& n) : name(n) {}
    
    void addLifetime(LifetimeParamPtr lt) {
        lifetimes.push_back(lt);
    }
    
    void addTypeParam(ASTNodePtr type) {
        typeParams.push_back(type);
    }
    
    ASTNodeKind kind() const override { return ASTNodeKind::GenericType; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << name;
        
        if (!lifetimes.empty() || !typeParams.empty()) {
            ss << "<";
            bool first = true;
            
            for (const auto& lt : lifetimes) {
                if (!first) ss << ", ";
                ss << lt->toString();
                first = false;
            }
            
            for (const auto& tp : typeParams) {
                if (!first) ss << ", ";
                ss << tp->toString();
                first = false;
            }
            
            ss << ">";
        }
        
        return ss.str();
    }
};

using GenericTypePtr = std::shared_ptr<GenericType>;

// =============================================================================
// عقد التعريفات
// =============================================================================

/**
 * معامل دالة مع نوع
 */
struct FunctionParam {
    std::string name;
    ASTNodePtr type;
    
    std::string toString() const {
        return name + ": " + (type ? type->toString() : "?");
    }
};

/**
 * تعريف دالة مع أعمار
 * 
 * دالة أطول<'أ>(س: &'أ نص, ص: &'أ نص) -> &'أ نص { ... }
 */
class FunctionDecl : public ASTNode {
public:
    std::string name;
    std::vector<LifetimeParamPtr> lifetimeParams;
    std::vector<FunctionParam> params;
    ASTNodePtr returnType;
    std::vector<LifetimeBoundPtr> bounds;
    ASTNodePtr body;
    
    FunctionDecl(const std::string& n) : name(n) {}
    
    void addLifetimeParam(LifetimeParamPtr lt) {
        lifetimeParams.push_back(lt);
    }
    
    void addParam(const std::string& name, ASTNodePtr type) {
        params.push_back({name, type});
    }
    
    void addBound(LifetimeBoundPtr bound) {
        bounds.push_back(bound);
    }
    
    ASTNodeKind kind() const override { return ASTNodeKind::FunctionDecl; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << "دالة " << name;
        
        // معاملات العمر
        if (!lifetimeParams.empty()) {
            ss << "<";
            for (size_t i = 0; i < lifetimeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << lifetimeParams[i]->toString();
            }
            ss << ">";
        }
        
        // معاملات الدالة
        ss << "(";
        for (size_t i = 0; i < params.size(); i++) {
            if (i > 0) ss << ", ";
            ss << params[i].toString();
        }
        ss << ")";
        
        // نوع الإرجاع
        if (returnType) {
            ss << " -> " << returnType->toString();
        }
        
        // القيود
        if (!bounds.empty()) {
            ss << " حيث ";
            for (size_t i = 0; i < bounds.size(); i++) {
                if (i > 0) ss << ", ";
                ss << bounds[i]->toString();
            }
        }
        
        return ss.str();
    }
};

using FunctionDeclPtr = std::shared_ptr<FunctionDecl>;

/**
 * حقل بنية
 */
struct StructField {
    std::string name;
    ASTNodePtr type;
    
    std::string toString() const {
        return name + ": " + (type ? type->toString() : "?");
    }
};

/**
 * تعريف بنية مع أعمار
 * 
 * بنية مقتبس<'أ> {
 *     محتوى: &'أ نص
 * }
 */
class StructDecl : public ASTNode {
public:
    std::string name;
    std::vector<LifetimeParamPtr> lifetimeParams;
    std::vector<StructField> fields;
    
    StructDecl(const std::string& n) : name(n) {}
    
    void addLifetimeParam(LifetimeParamPtr lt) {
        lifetimeParams.push_back(lt);
    }
    
    void addField(const std::string& name, ASTNodePtr type) {
        fields.push_back({name, type});
    }
    
    ASTNodeKind kind() const override { return ASTNodeKind::StructDecl; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << "بنية " << name;
        
        if (!lifetimeParams.empty()) {
            ss << "<";
            for (size_t i = 0; i < lifetimeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << lifetimeParams[i]->toString();
            }
            ss << ">";
        }
        
        ss << " {\n";
        for (const auto& field : fields) {
            ss << "    " << field.toString() << "\n";
        }
        ss << "}";
        
        return ss.str();
    }
};

using StructDeclPtr = std::shared_ptr<StructDecl>;

/**
 * كتلة تنفيذ مع أعمار
 * 
 * تنفيذ<'أ> للـ مقتبس<'أ> { ... }
 */
class ImplDecl : public ASTNode {
public:
    std::vector<LifetimeParamPtr> lifetimeParams;
    std::string traitName;  // فارغ إذا لم يكن impl trait
    GenericTypePtr selfType;
    std::vector<FunctionDeclPtr> methods;
    
    ImplDecl() = default;
    
    void addLifetimeParam(LifetimeParamPtr lt) {
        lifetimeParams.push_back(lt);
    }
    
    void addMethod(FunctionDeclPtr method) {
        methods.push_back(method);
    }
    
    ASTNodeKind kind() const override { return ASTNodeKind::ImplDecl; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << "تنفيذ";
        
        if (!lifetimeParams.empty()) {
            ss << "<";
            for (size_t i = 0; i < lifetimeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << lifetimeParams[i]->toString();
            }
            ss << ">";
        }
        
        if (!traitName.empty()) {
            ss << " " << traitName << " للـ";
        }
        
        if (selfType) {
            ss << " " << selfType->toString();
        }
        
        ss << " { ... }";
        
        return ss.str();
    }
};

using ImplDeclPtr = std::shared_ptr<ImplDecl>;

// =============================================================================
// عقد التعبيرات
// =============================================================================

/**
 * تعبير استعارة &x أو &متغير x
 */
class BorrowExpr : public ASTNode {
public:
    LifetimeParamPtr lifetime;  // قد يكون nullptr للعمر المستنتج
    bool isMutable;
    ASTNodePtr operand;
    std::string varName;  // للمتغيرات البسيطة
    
    BorrowExpr(bool mut, const std::string& var)
        : isMutable(mut), varName(var) {}
    
    ASTNodeKind kind() const override { return ASTNodeKind::BorrowExpr; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << "&";
        if (lifetime && !lifetime->isElided()) {
            ss << lifetime->toString() << " ";
        }
        if (isMutable) ss << "متغير ";
        if (!varName.empty()) {
            ss << varName;
        } else if (operand) {
            ss << operand->toString();
        }
        return ss.str();
    }
};

using BorrowExprPtr = std::shared_ptr<BorrowExpr>;

/**
 * تعبير فك المرجع *x
 */
class DerefExpr : public ASTNode {
public:
    ASTNodePtr operand;
    std::string varName;
    
    DerefExpr(const std::string& var) : varName(var) {}
    
    ASTNodeKind kind() const override { return ASTNodeKind::DerefExpr; }
    
    std::string toString() const override {
        std::ostringstream ss;
        ss << "*";
        if (!varName.empty()) {
            ss << varName;
        } else if (operand) {
            ss << operand->toString();
        }
        return ss.str();
    }
};

using DerefExprPtr = std::shared_ptr<DerefExpr>;

// =============================================================================
// منشئ العقد
// =============================================================================

/**
 * منشئ عقد AST للأعمار
 * يوفر دوال مساعدة لإنشاء العقد
 */
class LifetimeASTFactory {
public:
    // أعمار
    LifetimeParamPtr namedLifetime(const std::string& name) {
        return std::make_shared<LifetimeParam>(name, LifetimeKind::Named);
    }
    
    LifetimeParamPtr staticLifetime() {
        return std::make_shared<LifetimeParam>("ثابت", LifetimeKind::Static);
    }
    
    LifetimeParamPtr anonymousLifetime() {
        return std::make_shared<LifetimeParam>("_", LifetimeKind::Anonymous);
    }
    
    LifetimeParamPtr elidedLifetime() {
        return std::make_shared<LifetimeParam>("", LifetimeKind::Elided);
    }
    
    // قيود
    LifetimeBoundPtr bound(LifetimeParamPtr longer, LifetimeParamPtr shorter) {
        return std::make_shared<LifetimeBound>(longer, shorter);
    }
    
    // أنواع
    RefTypePtr refType(LifetimeParamPtr lt, bool mut, const std::string& type) {
        return std::make_shared<RefType>(lt, mut, type);
    }
    
    GenericTypePtr genericType(const std::string& name) {
        return std::make_shared<GenericType>(name);
    }
    
    // تعريفات
    FunctionDeclPtr functionDecl(const std::string& name) {
        return std::make_shared<FunctionDecl>(name);
    }
    
    StructDeclPtr structDecl(const std::string& name) {
        return std::make_shared<StructDecl>(name);
    }
    
    ImplDeclPtr implDecl() {
        return std::make_shared<ImplDecl>();
    }
    
    // تعبيرات
    BorrowExprPtr borrowExpr(bool mut, const std::string& var) {
        return std::make_shared<BorrowExpr>(mut, var);
    }
    
    DerefExprPtr derefExpr(const std::string& var) {
        return std::make_shared<DerefExpr>(var);
    }
};

} // namespace sad::ast::lifetime

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::ast::lifetime;

/**
 * إنشاء منشئ العقد
 */
void* sad_lifetime_ast_factory_new() {
    return new LifetimeASTFactory();
}

/**
 * تحرير المنشئ
 */
void sad_lifetime_ast_factory_free(void* factory) {
    delete static_cast<LifetimeASTFactory*>(factory);
}

/**
 * إنشاء عمر مسمى
 */
void* sad_create_named_lifetime(void* factory, const char* name) {
    if (!factory || !name) return nullptr;
    auto* f = static_cast<LifetimeASTFactory*>(factory);
    auto lt = f->namedLifetime(name);
    return new LifetimeParamPtr(lt);
}

/**
 * إنشاء تعريف دالة
 */
void* sad_create_function_decl(void* factory, const char* name) {
    if (!factory || !name) return nullptr;
    auto* f = static_cast<LifetimeASTFactory*>(factory);
    auto fn = f->functionDecl(name);
    return new FunctionDeclPtr(fn);
}

/**
 * الحصول على تمثيل نصي لعقدة
 */
const char* sad_ast_node_to_string(void* node) {
    if (!node) return "";
    auto* ptr = static_cast<ASTNodePtr*>(node);
    static std::string result;  // للحفاظ على الذاكرة
    result = (*ptr)->toString();
    return result.c_str();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef LIFETIME_AST_TEST

#include <cassert>

void testLifetimeParam() {
    sad::ast::lifetime::LifetimeASTFactory factory;
    
    auto ltA = factory.namedLifetime("أ");
    assert(ltA->name == "أ");
    assert(ltA->toString() == "'أ");
    
    auto ltStatic = factory.staticLifetime();
    assert(ltStatic->isStatic());
    assert(ltStatic->toString() == "'ثابت");
    
    auto ltAnon = factory.anonymousLifetime();
    assert(ltAnon->isAnonymous());
    assert(ltAnon->toString() == "'_");
    
    std::cout << "✅ اختبار معاملات العمر نجح!\n";
}

void testRefType() {
    sad::ast::lifetime::LifetimeASTFactory factory;
    
    auto lt = factory.namedLifetime("أ");
    auto ref = factory.refType(lt, false, "نص");
    
    assert(ref->toString() == "&'أ نص");
    
    auto mutRef = factory.refType(lt, true, "صحيح");
    assert(mutRef->toString() == "&'أ متغير صحيح");
    
    std::cout << "✅ اختبار الأنواع المرجعية نجح!\n";
}

void testFunctionDecl() {
    sad::ast::lifetime::LifetimeASTFactory factory;
    
    auto fn = factory.functionDecl("أطول");
    auto ltA = factory.namedLifetime("أ");
    
    fn->addLifetimeParam(ltA);
    fn->addParam("س", factory.refType(ltA, false, "نص"));
    fn->addParam("ص", factory.refType(ltA, false, "نص"));
    fn->returnType = factory.refType(ltA, false, "نص");
    
    std::string expected = "دالة أطول<'أ>(س: &'أ نص, ص: &'أ نص) -> &'أ نص";
    assert(fn->toString() == expected);
    
    std::cout << "✅ اختبار تعريف الدالة نجح!\n";
}

void testStructDecl() {
    sad::ast::lifetime::LifetimeASTFactory factory;
    
    auto st = factory.structDecl("مقتبس");
    auto ltA = factory.namedLifetime("أ");
    
    st->addLifetimeParam(ltA);
    st->addField("محتوى", factory.refType(ltA, false, "نص"));
    
    std::string str = st->toString();
    assert(str.find("بنية مقتبس<'أ>") != std::string::npos);
    assert(str.find("محتوى: &'أ نص") != std::string::npos);
    
    std::cout << "✅ اختبار تعريف البنية نجح!\n";
}

void testImplDecl() {
    sad::ast::lifetime::LifetimeASTFactory factory;
    
    auto impl = factory.implDecl();
    auto ltA = factory.namedLifetime("أ");
    
    impl->addLifetimeParam(ltA);
    impl->selfType = factory.genericType("مقتبس");
    impl->selfType->addLifetime(ltA);
    
    std::string str = impl->toString();
    assert(str.find("تنفيذ<'أ>") != std::string::npos);
    assert(str.find("مقتبس<'أ>") != std::string::npos);
    
    std::cout << "✅ اختبار كتلة التنفيذ نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات عقد AST للأعمار\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testLifetimeParam();
    testRefType();
    testFunctionDecl();
    testStructDecl();
    testImplDecl();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // LIFETIME_AST_TEST
