// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file extern_parser.h
 * @brief (AR) محلل كتل الدوال الخارجية (extern) - تحليل تصريحات FFI
 *        (EN) External Function Block Parser - Parsing FFI declarations
 * 
 * @details
 * (AR) هذا الملف يحتوي على محلل نحوي لكتل الدوال الخارجية في لغة "ص".
 *      يدعم الصيغ التالية:
 *      
 *      1. تصريح دالة خارجية واحدة:
 *         خارجي_س دالة printf(تنسيق: *ثابت حرف_س, ...) -> صحيح_س
 *      
 *      2. كتلة دوال خارجية:
 *         خارجي_س {
 *             دالة malloc(حجم: حجم_س) -> *فراغ_س
 *             دالة free(مؤشر: *فراغ_س)
 *         }
 *      
 *      3. تصريح مع تعليقات الملكية:
 *         #[ملك_النتيجة]
 *         #[يُحرر_بـ: free]
 *         خارجي_س دالة strdup(نص: *ثابت حرف_س) -> *حرف_س
 * 
 * (EN) This file contains a parser for external function blocks in Sad language.
 *      Supports the following syntax:
 *      
 *      1. Single external function declaration:
 *         extern_c fn printf(format: *const c_char, ...) -> c_int
 *      
 *      2. External function block:
 *         extern_c {
 *             fn malloc(size: c_size_t) -> *c_void
 *             fn free(ptr: *c_void)
 *         }
 *      
 *      3. Declaration with ownership annotations:
 *         #[owns_result]
 *         #[freed_by: free]
 *         extern_c fn strdup(s: *const c_char) -> *c_char
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 * @license MIT License
 */

#ifndef SAD_PARSER_EXTERN_PARSER_H
#define SAD_PARSER_EXTERN_PARSER_H

#include "parser_core.h"
#include "lexer_core.h"
#include "ast_node.h"
#include "ffi/c_abi.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace Sad {
namespace Parser {

// ============================================================================
//                    (AR) إعادة توجيه الأنواع
//                    (EN) Type Forward Declarations
// ============================================================================

class ExternParser;
class ExternFunctionDeclAST;
class ExternBlockAST;

using ExternFunctionDeclASTPtr = std::shared_ptr<ExternFunctionDeclAST>;
using ExternBlockASTPtr = std::shared_ptr<ExternBlockAST>;

// ============================================================================
//                    (AR) الكلمات المفتاحية للـ Extern
//                    (EN) Extern Keywords
// ============================================================================

/**
 * @namespace ExternKeywords
 * @brief (AR) الكلمات المفتاحية المستخدمة في تصريحات الدوال الخارجية
 *        (EN) Keywords used in external function declarations
 */
namespace ExternKeywords {
    // --- (AR) أنواع الربط / (EN) Linkage types ---
    extern const char* const KHARIJI;           // خارجي - extern
    extern const char* const KHARIJI_C;         // خارجي_س - extern "C"
    extern const char* const KHARIJI_CPP;       // خارجي_سي++ - extern "C++"
    extern const char* const KHARIJI_NIZAM;     // خارجي_نظام - extern "system"
    
    // --- (AR) كلمات الدالة / (EN) Function keywords ---
    extern const char* const DALLA;             // دالة - fn/function
    extern const char* const THABIT;            // ثابت - const
    extern const char* const MUTAHARIK;         // متحرك - mut/mutable
    
    // --- (AR) اتفاقيات الاستدعاء / (EN) Calling conventions ---
    extern const char* const ISTIDAA_C;         // #[استدعاء_س] - cdecl
    extern const char* const ISTIDAA_STD;       // #[استدعاء_قياسي] - stdcall
    extern const char* const ISTIDAA_SARI;      // #[استدعاء_سريع] - fastcall
    
    // --- (AR) تعليقات الملكية / (EN) Ownership annotations ---
    extern const char* const MALIK_NATIJA;      // #[ملك_النتيجة]
    extern const char* const YUHARRIR_BI;       // #[يُحرر_بـ:]
    extern const char* const LA_YUHARRIR;       // #[لا_يُحرر]
    extern const char* const MUSTAAR;           // #[مستعار]
    extern const char* const YANQUL;            // #[ينقل]
}

// ============================================================================
//                    (AR) نوع الربط الخارجي
//                    (EN) External Linkage Type
// ============================================================================

/**
 * @enum ExternLinkage
 * @brief (AR) نوع الربط الخارجي للدوال
 *        (EN) External linkage type for functions
 */
enum class ExternLinkage {
    C,          // خارجي_س - C ABI
    CPP,        // خارجي_سي++ - C++ ABI (mangled names)
    SYSTEM,     // خارجي_نظام - System ABI (platform-specific)
    UNKNOWN     // غير معروف
};

// ============================================================================
//                    (AR) عقدة AST لمعامل دالة خارجية
//                    (EN) AST Node for External Function Parameter
// ============================================================================

/**
 * @class ExternParamAST
 * @brief (AR) عقدة AST تمثل معامل في دالة خارجية
 *        (EN) AST node representing a parameter in external function
 */
class ExternParamAST : public AST::ASTNode {
public:
    /**
     * @struct OwnershipInfo
     * @brief (AR) معلومات ملكية المعامل
     *        (EN) Parameter ownership information
     */
    struct OwnershipInfo {
        bool isOwned = false;           // هل المعامل مملوك؟
        bool isBorrowed = false;        // هل المعامل مستعار؟
        bool transfers = false;         // هل ينتقل للمُستدعَى؟
        std::string freeFunction;       // دالة التحرير
    };
    
    /**
     * @brief (AR) بناء عقدة معامل خارجي
     *        (EN) Construct external parameter node
     * 
     * @param arabicName (AR) الاسم العربي للمعامل (EN) Arabic parameter name
     * @param originalName (AR) الاسم الأصلي (C) (EN) Original (C) name
     * @param cType (AR) نوع C للمعامل (EN) C type for parameter
     */
    ExternParamAST(const std::string& arabicName,
                   const std::string& originalName,
                   FFI::CTypePtr cType);
    
    // --- (AR) الوصول للخصائص / (EN) Property access ---
    
    const std::string& getArabicName() const { return arabicName_; }
    const std::string& getOriginalName() const { return originalName_; }
    FFI::CTypePtr getCType() const { return cType_; }
    const OwnershipInfo& getOwnership() const { return ownership_; }
    
    void setOwnership(const OwnershipInfo& info) { ownership_ = info; }
    
    // --- (AR) واجهة ASTNode / (EN) ASTNode interface ---
    
    void accept(AST::ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string arabicName_;        // الاسم العربي
    std::string originalName_;      // الاسم الأصلي (C)
    FFI::CTypePtr cType_;           // نوع C
    OwnershipInfo ownership_;       // معلومات الملكية
};

using ExternParamASTPtr = std::shared_ptr<ExternParamAST>;

// ============================================================================
//                    (AR) عقدة AST لتصريح دالة خارجية
//                    (EN) AST Node for External Function Declaration
// ============================================================================

/**
 * @class ExternFunctionDeclAST
 * @brief (AR) عقدة AST تمثل تصريح دالة خارجية (C/C++)
 *        (EN) AST node representing external function declaration (C/C++)
 * 
 * @details
 * (AR) تمثل هذه العقدة دالة مُعرّفة في كود C/C++ يمكن استدعاؤها من كود "ص".
 *      تحتوي على:
 *      - الاسم الأصلي (كما هو في مكتبة C)
 *      - الاسم العربي (للاستخدام في كود ص)
 *      - قائمة المعاملات مع أنواعها
 *      - نوع القيمة المُرجعة
 *      - معلومات اتفاقية الاستدعاء
 *      - تعليقات الملكية للذاكرة
 * 
 * (EN) This node represents a function defined in C/C++ code callable from Sad.
 *      Contains:
 *      - Original name (as in C library)
 *      - Arabic name (for use in Sad code)
 *      - Parameter list with types
 *      - Return type
 *      - Calling convention information
 *      - Memory ownership annotations
 */
class ExternFunctionDeclAST : public AST::Statement {
public:
    /**
     * @struct OwnershipAnnotations
     * @brief (AR) تعليقات ملكية الذاكرة للدالة
     *        (EN) Memory ownership annotations for function
     */
    struct OwnershipAnnotations {
        bool ownsResult = false;        // #[ملك_النتيجة]
        bool resultBorrowed = false;    // النتيجة مستعارة
        std::string freeFunction;       // دالة تحرير النتيجة
        bool noFree = false;            // #[لا_يُحرر] - لا تحرر النتيجة
    };
    
    /**
     * @brief (AR) بناء عقدة تصريح دالة خارجية
     *        (EN) Construct external function declaration node
     */
    ExternFunctionDeclAST();
    
    // --- (AR) إعداد الدالة / (EN) Function setup ---
    
    /**
     * @brief (AR) تعيين الاسم الأصلي (C)
     *        (EN) Set original (C) name
     */
    void setOriginalName(const std::string& name) { originalName_ = name; }
    std::string getOriginalName() const { return originalName_; }
    
    /**
     * @brief (AR) تعيين الاسم العربي
     *        (EN) Set Arabic name
     */
    void setArabicName(const std::string& name) { arabicName_ = name; }
    std::string getArabicName() const { return arabicName_; }
    
    /**
     * @brief (AR) تعيين نوع الربط
     *        (EN) Set linkage type
     */
    void setLinkage(ExternLinkage linkage) { linkage_ = linkage; }
    ExternLinkage getLinkage() const { return linkage_; }
    
    /**
     * @brief (AR) تعيين اتفاقية الاستدعاء
     *        (EN) Set calling convention
     */
    void setCallingConvention(FFI::CallingConvention conv) { callingConvention_ = conv; }
    FFI::CallingConvention getCallingConvention() const { return callingConvention_; }
    
    /**
     * @brief (AR) تعيين نوع القيمة المُرجعة
     *        (EN) Set return type
     */
    void setReturnType(FFI::CTypePtr type) { returnType_ = type; }
    FFI::CTypePtr getReturnType() const { return returnType_; }
    
    /**
     * @brief (AR) تعيين حالة variadic
     *        (EN) Set variadic state
     */
    void setVariadic(bool value) { isVariadic_ = value; }
    bool isVariadic() const { return isVariadic_; }
    
    /**
     * @brief (AR) تعيين اسم المكتبة
     *        (EN) Set library name
     */
    void setLibraryName(const std::string& lib) { libraryName_ = lib; }
    std::string getLibraryName() const { return libraryName_; }
    
    // --- (AR) المعاملات / (EN) Parameters ---
    
    /**
     * @brief (AR) إضافة معامل
     *        (EN) Add parameter
     */
    void addParameter(ExternParamASTPtr param);
    
    /**
     * @brief (AR) إرجاع قائمة المعاملات
     *        (EN) Returns parameter list
     */
    const std::vector<ExternParamASTPtr>& getParameters() const { return parameters_; }
    
    // --- (AR) تعليقات الملكية / (EN) Ownership annotations ---
    
    void setOwnershipAnnotations(const OwnershipAnnotations& annotations) { 
        ownershipAnnotations_ = annotations; 
    }
    const OwnershipAnnotations& getOwnershipAnnotations() const { 
        return ownershipAnnotations_; 
    }
    
    // --- (AR) التحويل إلى ExternFunctionDecl / (EN) Conversion to ExternFunctionDecl ---
    
    /**
     * @brief (AR) تحويل إلى كائن ExternFunctionDecl للاستخدام في توليد الكود
     *        (EN) Convert to ExternFunctionDecl object for code generation
     */
    FFI::ExternFunctionDecl toExternFunctionDecl() const;
    
    // --- (AR) واجهة Statement / (EN) Statement interface ---
    
    void accept(AST::ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string originalName_;      // الاسم الأصلي (C)
    std::string arabicName_;        // الاسم العربي
    ExternLinkage linkage_ = ExternLinkage::C;
    FFI::CallingConvention callingConvention_ = FFI::CallingConvention::AUTO;
    FFI::CTypePtr returnType_;      // نوع القيمة المُرجعة
    std::vector<ExternParamASTPtr> parameters_;  // قائمة المعاملات
    bool isVariadic_ = false;       // هل متغيرة المعاملات؟
    std::string libraryName_;       // اسم المكتبة
    OwnershipAnnotations ownershipAnnotations_;  // تعليقات الملكية
};

// ============================================================================
//                    (AR) عقدة AST لكتلة extern
//                    (EN) AST Node for extern Block
// ============================================================================

/**
 * @class ExternBlockAST
 * @brief (AR) عقدة AST تمثل كتلة extern تحتوي على عدة تصريحات
 *        (EN) AST node representing extern block containing multiple declarations
 * 
 * @example
 * @code
 * // كتلة extern / extern block
 * خارجي_س {
 *     دالة malloc(حجم: حجم_س) -> *فراغ_س
 *     دالة free(مؤشر: *فراغ_س)
 *     دالة realloc(مؤشر: *فراغ_س, حجم: حجم_س) -> *فراغ_س
 * }
 * @endcode
 */
class ExternBlockAST : public AST::Statement {
public:
    /**
     * @brief (AR) بناء عقدة كتلة extern
     *        (EN) Construct extern block node
     * 
     * @param linkage (AR) نوع الربط للكتلة (EN) Linkage type for block
     */
    explicit ExternBlockAST(ExternLinkage linkage = ExternLinkage::C);
    
    // --- (AR) خصائص الكتلة / (EN) Block properties ---
    
    ExternLinkage getLinkage() const { return linkage_; }
    void setLinkage(ExternLinkage linkage) { linkage_ = linkage; }
    
    /**
     * @brief (AR) تعيين اسم المكتبة لجميع الدوال في الكتلة
     *        (EN) Set library name for all functions in block
     */
    void setLibraryName(const std::string& lib) { libraryName_ = lib; }
    std::string getLibraryName() const { return libraryName_; }
    
    // --- (AR) التصريحات / (EN) Declarations ---
    
    /**
     * @brief (AR) إضافة تصريح دالة
     *        (EN) Add function declaration
     */
    void addFunction(ExternFunctionDeclASTPtr func);
    
    /**
     * @brief (AR) إرجاع قائمة التصريحات
     *        (EN) Returns declaration list
     */
    const std::vector<ExternFunctionDeclASTPtr>& getFunctions() const { return functions_; }
    
    // --- (AR) واجهة Statement / (EN) Statement interface ---
    
    void accept(AST::ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ExternLinkage linkage_;         // نوع الربط
    std::string libraryName_;       // اسم المكتبة
    std::vector<ExternFunctionDeclASTPtr> functions_;  // قائمة الدوال
};

// ============================================================================
//                    (AR) محلل الدوال الخارجية
//                    (EN) External Function Parser
// ============================================================================

/**
 * @class ExternParser
 * @brief (AR) محلل نحوي متخصص لتصريحات الدوال الخارجية (FFI)
 *        (EN) Specialized parser for external function declarations (FFI)
 * 
 * @details
 * (AR) هذا المحلل يتعامل مع جميع أنواع تصريحات FFI:
 *      - تصريحات دوال مفردة
 *      - كتل extern متعددة الدوال
 *      - تعليقات الملكية والاتفاقيات
 *      - تحويل أنواع C إلى أنواع ص والعكس
 * 
 * (EN) This parser handles all types of FFI declarations:
 *      - Single function declarations
 *      - Multi-function extern blocks
 *      - Ownership annotations and conventions
 *      - C type to Sad type conversion and vice versa
 * 
 * @example
 * @code{.cpp}
 * // استخدام المحلل / Using the parser
 * Lexer::LexerCore lexer(source);
 * ExternParser parser(lexer);
 * 
 * // تحليل كتلة extern / Parse extern block
 * auto block = parser.parseExternBlock();
 * 
 * // أو تحليل دالة واحدة / Or parse single function
 * auto func = parser.parseExternFunction();
 * @endcode
 */
class ExternParser {
public:
    /**
     * @brief (AR) بناء محلل الدوال الخارجية
     *        (EN) Construct external function parser
     * 
     * @param lexer (AR) المحلل المعجمي (EN) Lexer
     */
    explicit ExternParser(Lexer::LexerCore& lexer);
    
    ~ExternParser() = default;
    
    // ========================================================================
    // (AR) دوال التحليل الرئيسية
    // (EN) Main Parsing Functions
    // ========================================================================
    
    /**
     * @brief (AR) تحليل تصريح extern (دالة واحدة أو كتلة)
     *        (EN) Parse extern declaration (single function or block)
     * 
     * (AR) يكتشف تلقائياً إذا كان التصريح دالة واحدة أو كتلة.
     * (EN) Automatically detects if declaration is single function or block.
     * 
     * @return (AR) عقدة AST للتصريح (EN) AST node for declaration
     */
    AST::StmtPtr parseExternDeclaration();
    
    /**
     * @brief (AR) تحليل كتلة extern
     *        (EN) Parse extern block
     * 
     * @return (AR) عقدة كتلة extern (EN) Extern block node
     */
    ExternBlockASTPtr parseExternBlock();
    
    /**
     * @brief (AR) تحليل تصريح دالة خارجية واحدة
     *        (EN) Parse single external function declaration
     * 
     * @param linkage (AR) نوع الربط (EN) Linkage type
     * @return (AR) عقدة تصريح الدالة (EN) Function declaration node
     */
    ExternFunctionDeclASTPtr parseExternFunction(ExternLinkage linkage = ExternLinkage::C);
    
    // ========================================================================
    // (AR) تحليل الأنواع
    // (EN) Type Parsing
    // ========================================================================
    
    /**
     * @brief (AR) تحليل نوع C
     *        (EN) Parse C type
     * 
     * (AR) يحلل أنواع C بما في ذلك:
     *      - الأنواع الأساسية (int, char, etc.)
     *      - المؤشرات (*T, **T)
     *      - المصفوفات (T[N])
     *      - مؤهلات const و volatile
     * 
     * (EN) Parses C types including:
     *      - Basic types (int, char, etc.)
     *      - Pointers (*T, **T)
     *      - Arrays (T[N])
     *      - const and volatile qualifiers
     * 
     * @return (AR) نوع C (EN) C type
     */
    FFI::CTypePtr parseCType();
    
    /**
     * @brief (AR) تحليل نوع أساسي
     *        (EN) Parse basic type
     * 
     * @return (AR) نوع أساسي (EN) Basic type
     */
    FFI::CTypePtr parseBasicType();
    
    /**
     * @brief (AR) تحليل نوع مؤشر
     *        (EN) Parse pointer type
     * 
     * @param baseType (AR) النوع الأساسي (EN) Base type
     * @return (AR) نوع المؤشر (EN) Pointer type
     */
    FFI::CTypePtr parsePointerType(FFI::CTypePtr baseType);
    
    /**
     * @brief (AR) تحليل نوع مصفوفة
     *        (EN) Parse array type
     * 
     * @param elementType (AR) نوع العناصر (EN) Element type
     * @return (AR) نوع المصفوفة (EN) Array type
     */
    FFI::CTypePtr parseArrayType(FFI::CTypePtr elementType);
    
    // ========================================================================
    // (AR) تحليل التعليقات التوضيحية
    // (EN) Annotation Parsing
    // ========================================================================
    
    /**
     * @brief (AR) تحليل تعليقات الملكية
     *        (EN) Parse ownership annotations
     * 
     * (AR) يحلل التعليقات مثل:
     *      - #[ملك_النتيجة]
     *      - #[يُحرر_بـ: free]
     *      - #[مستعار]
     * 
     * @return (AR) معلومات الملكية (EN) Ownership information
     */
    ExternFunctionDeclAST::OwnershipAnnotations parseOwnershipAnnotations();
    
    /**
     * @brief (AR) تحليل اتفاقية الاستدعاء
     *        (EN) Parse calling convention
     * 
     * @return (AR) اتفاقية الاستدعاء (EN) Calling convention
     */
    FFI::CallingConvention parseCallingConvention();
    
    // ========================================================================
    // (AR) التحقق والأخطاء
    // (EN) Validation and Errors
    // ========================================================================
    
    /**
     * @brief (AR) هل هناك أخطاء في التحليل؟
     *        (EN) Are there parsing errors?
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief (AR) إرجاع قائمة الأخطاء
     *        (EN) Returns error list
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * @brief (AR) طباعة الأخطاء
     *        (EN) Print errors
     */
    void printErrors() const;

private:
    Lexer::LexerCore& lexer_;       // المحلل المعجمي
    Lexer::Token currentToken_;     // الرمز الحالي
    Lexer::Token peekToken_;        // الرمز التالي
    std::vector<std::string> errors_;  // قائمة الأخطاء
    
    // ========================================================================
    // (AR) دوال مساعدة
    // (EN) Helper Functions
    // ========================================================================
    
    /**
     * @brief (AR) قراءة الرمز التالي
     *        (EN) Read next token
     */
    void advance();
    
    /**
     * @brief (AR) التحقق من نوع الرمز الحالي
     *        (EN) Check current token type
     */
    bool check(Lexer::TokenType type) const;
    
    /**
     * @brief (AR) استهلاك رمز معين
     *        (EN) Consume specific token
     */
    bool consume(Lexer::TokenType type, const std::string& errorMsg);
    
    /**
     * @brief (AR) التحقق من كلمة مفتاحية عربية
     *        (EN) Check for Arabic keyword
     */
    bool checkArabicKeyword(const std::string& keyword) const;
    
    /**
     * @brief (AR) استهلاك كلمة مفتاحية عربية
     *        (EN) Consume Arabic keyword
     */
    bool consumeArabicKeyword(const std::string& keyword, const std::string& errorMsg);
    
    /**
     * @brief (AR) تحليل نوع الربط
     *        (EN) Parse linkage type
     */
    ExternLinkage parseLinkage();
    
    /**
     * @brief (AR) تحليل قائمة المعاملات
     *        (EN) Parse parameter list
     */
    std::vector<ExternParamASTPtr> parseParameterList();
    
    /**
     * @brief (AR) تحليل معامل واحد
     *        (EN) Parse single parameter
     */
    ExternParamASTPtr parseParameter();
    
    /**
     * @brief (AR) تسجيل خطأ
     *        (EN) Report error
     */
    void reportError(const std::string& message);
    
    /**
     * @brief (AR) تخطي حتى نقطة استعادة
     *        (EN) Skip to recovery point
     */
    void skipToRecovery();
    
    /**
     * @brief (AR) تحويل اسم النوع العربي إلى CBasicType
     *        (EN) Convert Arabic type name to CBasicType
     */
    std::optional<FFI::CBasicType> arabicTypeToCBasicType(const std::string& name) const;
};

// ============================================================================
//                    (AR) دوال مساعدة عامة
//                    (EN) General Helper Functions
// ============================================================================

/**
 * @brief (AR) تحويل نوع الربط إلى نص
 *        (EN) Convert linkage type to string
 */
std::string linkageToString(ExternLinkage linkage);

/**
 * @brief (AR) تحليل نوع الربط من نص
 *        (EN) Parse linkage type from string
 */
ExternLinkage parseLinkageFromString(const std::string& str);

/**
 * @brief (AR) هل هذا الرمز بداية تصريح extern؟
 *        (EN) Is this token start of extern declaration?
 */
bool isExternKeyword(const Lexer::Token& token);

} // namespace Parser
} // namespace Sad

#endif // SAD_PARSER_EXTERN_PARSER_H
