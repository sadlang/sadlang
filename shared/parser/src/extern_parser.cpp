// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file extern_parser.cpp
 * @brief (AR) تنفيذ محلل كتل الدوال الخارجية (extern) - تحليل تصريحات FFI
 *        (EN) External Function Block Parser Implementation - Parsing FFI declarations
 * 
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ المحلل النحوي لكتل الدوال الخارجية.
 *      يدعم:
 *      - تحليل تصريحات الدوال الخارجية بالعربية والإنجليزية
 *      - تحليل أنواع C وتحويلها
 *      - تحليل تعليقات الملكية للذاكرة
 *      - معالجة اتفاقيات الاستدعاء المختلفة
 * 
 * (EN) This file contains the implementation of the extern block parser.
 *      Supports:
 *      - Parsing external function declarations in Arabic and English
 *      - C type parsing and conversion
 *      - Memory ownership annotation parsing
 *      - Different calling convention handling
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 * @license MIT License
 */

#include "extern_parser.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Sad {
namespace Parser {

// ============================================================================
//                    (AR) تعريف الكلمات المفتاحية
//                    (EN) Keyword Definitions
// ============================================================================

namespace ExternKeywords {
    // --- (AR) أنواع الربط / (EN) Linkage types ---
    // خارجي - extern
    const char* const KHARIJI = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a";
    // خارجي_س - extern "C"
    const char* const KHARIJI_C = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3";
    // خارجي_سي++ - extern "C++"
    const char* const KHARIJI_CPP = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3\xd9\x8a++";
    // خارجي_نظام - extern "system"
    const char* const KHARIJI_NIZAM = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85";
    
    // --- (AR) كلمات الدالة / (EN) Function keywords ---
    // دالة - fn/function
    const char* const DALLA = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9";
    // ثابت - const
    const char* const THABIT = "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa";
    // متحرك - mut/mutable
    const char* const MUTAHARIK = "\xd9\x85\xd8\xaa\xd8\xad\xd8\xb1\xd9\x83";
    
    // --- (AR) اتفاقيات الاستدعاء / (EN) Calling conventions ---
    // #[استدعاء_س] - cdecl
    const char* const ISTIDAA_C = "#[\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd8\xb3]";
    // #[استدعاء_قياسي] - stdcall
    const char* const ISTIDAA_STD = "#[\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3\xd9\x8a]";
    // #[استدعاء_سريع] - fastcall
    const char* const ISTIDAA_SARI = "#[\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd8\xb3\xd8\xb1\xd9\x8a\xd8\xb9]";
    
    // --- (AR) تعليقات الملكية / (EN) Ownership annotations ---
    // #[ملك_النتيجة]
    const char* const MALIK_NATIJA = "#[\xd9\x85\xd9\x84\xd9\x83_\xd8\xa7\xd9\x84\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9]";
    // #[يُحرر_بـ:]
    const char* const YUHARRIR_BI = "#[\xd9\x8a\xd9\x8f\xd8\xad\xd8\xb1\xd8\xb1_\xd8\xa8\xd9\x80:]";
    // #[لا_يُحرر]
    const char* const LA_YUHARRIR = "#[\xd9\x84\xd8\xa7_\xd9\x8a\xd9\x8f\xd8\xad\xd8\xb1\xd8\xb1]";
    // #[مستعار]
    const char* const MUSTAAR = "#[\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xa7\xd8\xb1]";
    // #[ينقل]
    const char* const YANQUL = "#[\xd9\x8a\xd9\x86\xd9\x82\xd9\x84]";
}

// ============================================================================
//                    (AR) تنفيذ ExternParamAST
//                    (EN) ExternParamAST Implementation
// ============================================================================

ExternParamAST::ExternParamAST(
    const std::string& arabicName,
    const std::string& originalName,
    FFI::CTypePtr cType)
    : arabicName_(arabicName)
    , originalName_(originalName)
    , cType_(cType)
{
}

void ExternParamAST::accept(AST::ASTVisitor& visitor) {
    // (AR) زيارة العقدة - سيتم تنفيذها في نظام الزائر
    // (EN) Visit node - will be implemented in visitor system
}

std::string ExternParamAST::toString() const {
    std::stringstream ss;
    ss << arabicName_;
    if (!originalName_.empty() && originalName_ != arabicName_) {
        ss << " (" << originalName_ << ")";
    }
    ss << ": " << cType_->getArabicName();
    return ss.str();
}

// ============================================================================
//                    (AR) تنفيذ ExternFunctionDeclAST
//                    (EN) ExternFunctionDeclAST Implementation
// ============================================================================

ExternFunctionDeclAST::ExternFunctionDeclAST()
    : linkage_(ExternLinkage::C)
    , callingConvention_(FFI::CallingConvention::AUTO)
    , isVariadic_(false)
{
}

void ExternFunctionDeclAST::addParameter(ExternParamASTPtr param) {
    parameters_.push_back(param);
}

FFI::ExternFunctionDecl ExternFunctionDeclAST::toExternFunctionDecl() const {
    FFI::ExternFunctionDecl decl;
    
    // (AR) تعيين الأسماء / (EN) Set names
    decl.setOriginalName(originalName_);
    decl.setArabicName(arabicName_.empty() ? originalName_ : arabicName_);
    
    // (AR) تعيين نوع الإرجاع / (EN) Set return type
    decl.setReturnType(returnType_);
    
    // (AR) تعيين اتفاقية الاستدعاء / (EN) Set calling convention
    decl.setCallingConvention(callingConvention_);
    
    // (AR) تعيين variadic / (EN) Set variadic
    decl.setVariadic(isVariadic_);
    
    // (AR) تعيين اسم المكتبة / (EN) Set library name
    decl.setLibraryName(libraryName_);
    
    // (AR) إضافة المعاملات / (EN) Add parameters
    for (const auto& param : parameters_) {
        FFI::ExternFunctionDecl::OwnershipAnnotation ownership = 
            FFI::ExternFunctionDecl::OwnershipAnnotation::NONE;
        
        const auto& ownerInfo = param->getOwnership();
        if (ownerInfo.isOwned) {
            ownership = FFI::ExternFunctionDecl::OwnershipAnnotation::OWNS_RESULT;
        } else if (ownerInfo.isBorrowed) {
            ownership = FFI::ExternFunctionDecl::OwnershipAnnotation::BORROWED;
        } else if (ownerInfo.transfers) {
            ownership = FFI::ExternFunctionDecl::OwnershipAnnotation::TRANSFERS;
        }
        
        decl.addParameter(
            param->getOriginalName(),
            param->getArabicName(),
            param->getCType(),
            ownership,
            ownerInfo.freeFunction
        );
    }
    
    // (AR) تعيين ملكية النتيجة / (EN) Set result ownership
    if (ownershipAnnotations_.ownsResult) {
        decl.setResultOwnership(
            FFI::ExternFunctionDecl::OwnershipAnnotation::OWNS_RESULT,
            ownershipAnnotations_.freeFunction
        );
    } else if (ownershipAnnotations_.resultBorrowed) {
        decl.setResultOwnership(
            FFI::ExternFunctionDecl::OwnershipAnnotation::BORROWED,
            ""
        );
    } else if (ownershipAnnotations_.noFree) {
        decl.setResultOwnership(
            FFI::ExternFunctionDecl::OwnershipAnnotation::NONE,
            ""
        );
    }
    
    return decl;
}

void ExternFunctionDeclAST::accept(AST::ASTVisitor& visitor) {
    // (AR) زيارة العقدة - سيتم تنفيذها في نظام الزائر
    // (EN) Visit node - will be implemented in visitor system
}

std::string ExternFunctionDeclAST::toString() const {
    std::stringstream ss;
    
    // (AR) نوع الربط / (EN) Linkage type
    switch (linkage_) {
        case ExternLinkage::C:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3 ";  // خارجي_س
            break;
        case ExternLinkage::CPP:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3\xd9\x8a++ ";  // خارجي_سي++
            break;
        case ExternLinkage::SYSTEM:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 ";  // خارجي_نظام
            break;
        default:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a ";  // خارجي
            break;
    }
    
    // (AR) اسم الدالة / (EN) Function name
    ss << "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 ";  // دالة
    ss << (arabicName_.empty() ? originalName_ : arabicName_);
    
    // (AR) المعاملات / (EN) Parameters
    ss << "(";
    bool first = true;
    for (const auto& param : parameters_) {
        if (!first) ss << ", ";
        ss << param->toString();
        first = false;
    }
    if (isVariadic_) {
        if (!parameters_.empty()) ss << ", ";
        ss << "...";
    }
    ss << ")";
    
    // (AR) نوع الإرجاع / (EN) Return type
    if (returnType_) {
        ss << " -> " << returnType_->getArabicName();
    }
    
    return ss.str();
}

// ============================================================================
//                    (AR) تنفيذ ExternBlockAST
//                    (EN) ExternBlockAST Implementation
// ============================================================================

ExternBlockAST::ExternBlockAST(ExternLinkage linkage)
    : linkage_(linkage)
{
}

void ExternBlockAST::addFunction(ExternFunctionDeclASTPtr func) {
    // (AR) تطبيق اسم المكتبة إذا لم يكن محدداً في الدالة
    // (EN) Apply library name if not specified in function
    if (!libraryName_.empty() && func->getLibraryName().empty()) {
        func->setLibraryName(libraryName_);
    }
    
    // (AR) تطبيق نوع الربط
    // (EN) Apply linkage type
    if (func->getLinkage() == ExternLinkage::UNKNOWN) {
        func->setLinkage(linkage_);
    }
    
    functions_.push_back(func);
}

void ExternBlockAST::accept(AST::ASTVisitor& visitor) {
    // (AR) زيارة العقدة - سيتم تنفيذها في نظام الزائر
    // (EN) Visit node - will be implemented in visitor system
}

std::string ExternBlockAST::toString() const {
    std::stringstream ss;
    
    // (AR) نوع الربط / (EN) Linkage type
    switch (linkage_) {
        case ExternLinkage::C:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3";  // خارجي_س
            break;
        case ExternLinkage::CPP:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3\xd9\x8a++";  // خارجي_سي++
            break;
        case ExternLinkage::SYSTEM:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85";  // خارجي_نظام
            break;
        default:
            ss << "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a";  // خارجي
            break;
    }
    
    ss << " {\n";
    
    for (const auto& func : functions_) {
        ss << "    " << func->toString() << "\n";
    }
    
    ss << "}";
    
    return ss.str();
}

// ============================================================================
//                    (AR) تنفيذ ExternParser
//                    (EN) ExternParser Implementation
// ============================================================================

ExternParser::ExternParser(Lexer::LexerCore& lexer)
    : lexer_(lexer)
{
    // (AR) قراءة أول رمزين / (EN) Read first two tokens
    advance();
    advance();
}

void ExternParser::advance() {
    currentToken_ = peekToken_;
    peekToken_ = lexer_.nextToken();
}

bool ExternParser::check(Lexer::TokenType type) const {
    return currentToken_.type == type;
}

bool ExternParser::consume(Lexer::TokenType type, const std::string& errorMsg) {
    if (check(type)) {
        advance();
        return true;
    }
    reportError(errorMsg);
    return false;
}

bool ExternParser::checkArabicKeyword(const std::string& keyword) const {
    if (currentToken_.type != Lexer::TokenType::IDENTIFIER) {
        return false;
    }
    return currentToken_.value == keyword;
}

bool ExternParser::consumeArabicKeyword(const std::string& keyword, const std::string& errorMsg) {
    if (checkArabicKeyword(keyword)) {
        advance();
        return true;
    }
    reportError(errorMsg);
    return false;
}

void ExternParser::reportError(const std::string& message) {
    std::stringstream ss;
    ss << "\xd8\xae\xd8\xb7\xd8\xa3 [";  // خطأ [
    ss << currentToken_.line << ":" << currentToken_.column;
    ss << "]: " << message;
    errors_.push_back(ss.str());
}

void ExternParser::printErrors() const {
    for (const auto& error : errors_) {
        std::cerr << error << std::endl;
    }
}

void ExternParser::skipToRecovery() {
    // (AR) تخطي حتى نجد نقطة استعادة مناسبة
    // (EN) Skip until we find appropriate recovery point
    while (!check(Lexer::TokenType::END_OF_FILE)) {
        if (check(Lexer::TokenType::RIGHT_BRACE) ||
            check(Lexer::TokenType::SEMICOLON) ||
            checkArabicKeyword(ExternKeywords::DALLA) ||
            checkArabicKeyword(ExternKeywords::KHARIJI) ||
            checkArabicKeyword(ExternKeywords::KHARIJI_C)) {
            return;
        }
        advance();
    }
}

ExternLinkage ExternParser::parseLinkage() {
    // (AR) تحليل نوع الربط من الكلمة المفتاحية
    // (EN) Parse linkage type from keyword
    
    if (checkArabicKeyword(ExternKeywords::KHARIJI_C) || 
        currentToken_.value == "extern_c" ||
        currentToken_.value == "extern \"C\"") {
        advance();
        return ExternLinkage::C;
    }
    
    if (checkArabicKeyword(ExternKeywords::KHARIJI_CPP) ||
        currentToken_.value == "extern_cpp" ||
        currentToken_.value == "extern \"C++\"") {
        advance();
        return ExternLinkage::CPP;
    }
    
    if (checkArabicKeyword(ExternKeywords::KHARIJI_NIZAM) ||
        currentToken_.value == "extern_system") {
        advance();
        return ExternLinkage::SYSTEM;
    }
    
    if (checkArabicKeyword(ExternKeywords::KHARIJI) ||
        currentToken_.value == "extern") {
        advance();
        return ExternLinkage::C;  // (AR) افتراضي C / (EN) Default C
    }
    
    return ExternLinkage::UNKNOWN;
}

AST::StmtPtr ExternParser::parseExternDeclaration() {
    // (AR) تحليل نوع الربط أولاً
    // (EN) Parse linkage type first
    ExternLinkage linkage = parseLinkage();
    
    if (linkage == ExternLinkage::UNKNOWN) {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a");  // متوقع كلمة خارجي
        return nullptr;
    }
    
    // (AR) هل هي كتلة أم دالة واحدة؟
    // (EN) Is it a block or single function?
    if (check(Lexer::TokenType::LEFT_BRACE)) {
        // (AR) كتلة extern / (EN) extern block
        auto block = std::make_shared<ExternBlockAST>(linkage);
        advance();  // (AR) استهلاك { / (EN) consume {
        
        while (!check(Lexer::TokenType::RIGHT_BRACE) && 
               !check(Lexer::TokenType::END_OF_FILE)) {
            auto func = parseExternFunction(linkage);
            if (func) {
                block->addFunction(func);
            } else {
                skipToRecovery();
            }
        }
        
        consume(Lexer::TokenType::RIGHT_BRACE, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 }");  // متوقع }
        
        return block;
    } else {
        // (AR) دالة واحدة / (EN) single function
        return parseExternFunction(linkage);
    }
}

ExternBlockASTPtr ExternParser::parseExternBlock() {
    // (AR) تحليل نوع الربط
    // (EN) Parse linkage type
    ExternLinkage linkage = parseLinkage();
    
    if (linkage == ExternLinkage::UNKNOWN) {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a");  // متوقع كلمة خارجي
        return nullptr;
    }
    
    auto block = std::make_shared<ExternBlockAST>(linkage);
    
    if (!consume(Lexer::TokenType::LEFT_BRACE, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 {")) {  // متوقع {
        return nullptr;
    }
    
    while (!check(Lexer::TokenType::RIGHT_BRACE) && 
           !check(Lexer::TokenType::END_OF_FILE)) {
        auto func = parseExternFunction(linkage);
        if (func) {
            block->addFunction(func);
        } else {
            skipToRecovery();
        }
    }
    
    consume(Lexer::TokenType::RIGHT_BRACE, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 }");  // متوقع }
    
    return block;
}

ExternFunctionDeclASTPtr ExternParser::parseExternFunction(ExternLinkage linkage) {
    auto func = std::make_shared<ExternFunctionDeclAST>();
    func->setLinkage(linkage);
    
    // (AR) تحليل التعليقات التوضيحية (إن وجدت)
    // (EN) Parse annotations (if any)
    auto ownership = parseOwnershipAnnotations();
    func->setOwnershipAnnotations(ownership);
    
    // (AR) تحليل اتفاقية الاستدعاء (إن وجدت)
    // (EN) Parse calling convention (if any)
    auto callingConv = parseCallingConvention();
    func->setCallingConvention(callingConv);
    
    // (AR) كلمة دالة / (EN) function keyword
    if (!checkArabicKeyword(ExternKeywords::DALLA) && currentToken_.value != "fn") {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9");  // متوقع كلمة دالة
        return nullptr;
    }
    advance();
    
    // (AR) اسم الدالة / (EN) function name
    if (!check(Lexer::TokenType::IDENTIFIER)) {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9");  // متوقع اسم الدالة
        return nullptr;
    }
    
    std::string funcName = currentToken_.value;
    func->setOriginalName(funcName);
    func->setArabicName(funcName);  // (AR) افتراضياً نفس الاسم / (EN) default same name
    advance();
    
    // (AR) قائمة المعاملات / (EN) parameter list
    if (!consume(Lexer::TokenType::LEFT_PAREN, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 (")) {  // متوقع (
        return nullptr;
    }
    
    auto params = parseParameterList();
    for (const auto& param : params) {
        func->addParameter(param);
    }
    
    if (!consume(Lexer::TokenType::RIGHT_PAREN, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 )")) {  // متوقع )
        return nullptr;
    }
    
    // (AR) نوع الإرجاع (اختياري) / (EN) return type (optional)
    if (check(Lexer::TokenType::ARROW) || currentToken_.value == "->") {
        advance();
        auto returnType = parseCType();
        if (!returnType) {
            reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd9\x86\xd9\x88\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9");  // متوقع نوع الإرجاع
            return nullptr;
        }
        func->setReturnType(returnType);
    } else {
        // (AR) إذا لم يُحدد نوع الإرجاع، نفترض void
        // (EN) If return type not specified, assume void
        func->setReturnType(FFI::makeBasicType(FFI::CBasicType::VOID));
    }
    
    // (AR) الفاصلة المنقوطة (اختيارية)
    // (EN) Semicolon (optional)
    if (check(Lexer::TokenType::SEMICOLON)) {
        advance();
    }
    
    return func;
}

std::vector<ExternParamASTPtr> ExternParser::parseParameterList() {
    std::vector<ExternParamASTPtr> params;
    
    // (AR) قائمة فارغة / (EN) empty list
    if (check(Lexer::TokenType::RIGHT_PAREN)) {
        return params;
    }
    
    // (AR) تحليل أول معامل / (EN) parse first parameter
    auto firstParam = parseParameter();
    if (firstParam) {
        params.push_back(firstParam);
    }
    
    // (AR) تحليل بقية المعاملات / (EN) parse remaining parameters
    while (check(Lexer::TokenType::COMMA)) {
        advance();  // (AR) استهلاك الفاصلة / (EN) consume comma
        
        // (AR) التحقق من variadic (...) / (EN) check for variadic (...)
        if (check(Lexer::TokenType::DOT_DOT_DOT) || currentToken_.value == "...") {
            // (AR) variadic - تعليم في الدالة الأب
            // (EN) variadic - mark in parent function
            advance();
            break;
        }
        
        auto param = parseParameter();
        if (param) {
            params.push_back(param);
        }
    }
    
    return params;
}

ExternParamASTPtr ExternParser::parseParameter() {
    // (AR) اسم المعامل / (EN) parameter name
    if (!check(Lexer::TokenType::IDENTIFIER)) {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84");  // متوقع اسم المعامل
        return nullptr;
    }
    
    std::string paramName = currentToken_.value;
    advance();
    
    // (AR) النقطتان / (EN) colon
    if (!consume(Lexer::TokenType::COLON, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 :")) {  // متوقع :
        return nullptr;
    }
    
    // (AR) نوع المعامل / (EN) parameter type
    auto paramType = parseCType();
    if (!paramType) {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd9\x86\xd9\x88\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84");  // متوقع نوع المعامل
        return nullptr;
    }
    
    return std::make_shared<ExternParamAST>(paramName, paramName, paramType);
}

FFI::CTypePtr ExternParser::parseCType() {
    // (AR) التحقق من ثابت (const) / (EN) check for const
    bool isConst = false;
    if (checkArabicKeyword(ExternKeywords::THABIT) || currentToken_.value == "const") {
        isConst = true;
        advance();
    }
    
    // (AR) التحقق من مؤشر (*) / (EN) check for pointer (*)
    int pointerLevel = 0;
    while (check(Lexer::TokenType::STAR) || currentToken_.value == "*") {
        pointerLevel++;
        advance();
        
        // (AR) التحقق من const بعد * / (EN) check for const after *
        if (checkArabicKeyword(ExternKeywords::THABIT) || currentToken_.value == "const") {
            isConst = true;
            advance();
        }
    }
    
    // (AR) تحليل النوع الأساسي / (EN) parse basic type
    auto baseType = parseBasicType();
    if (!baseType) {
        return nullptr;
    }
    
    // (AR) تطبيق const على النوع الأساسي / (EN) apply const to base type
    if (isConst) {
        baseType->setConst(true);
    }
    
    // (AR) بناء أنواع المؤشرات / (EN) build pointer types
    FFI::CTypePtr resultType = baseType;
    for (int i = 0; i < pointerLevel; ++i) {
        resultType = FFI::makePointerType(resultType);
    }
    
    // (AR) التحقق من مصفوفة / (EN) check for array
    if (check(Lexer::TokenType::LEFT_BRACKET)) {
        resultType = parseArrayType(resultType);
    }
    
    return resultType;
}

FFI::CTypePtr ExternParser::parseBasicType() {
    if (!check(Lexer::TokenType::IDENTIFIER)) {
        reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd9\x86\xd9\x88\xd8\xb9");  // متوقع نوع
        return nullptr;
    }
    
    std::string typeName = currentToken_.value;
    
    // (AR) محاولة تحويل الاسم إلى نوع C / (EN) try to convert name to C type
    auto maybeType = arabicTypeToCBasicType(typeName);
    if (maybeType.has_value()) {
        advance();
        return FFI::makeBasicType(maybeType.value());
    }
    
    // (AR) أسماء C الإنجليزية / (EN) English C names
    FFI::CBasicType cType;
    if (typeName == "void" || typeName == "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba") {  // فراغ
        cType = FFI::CBasicType::VOID;
    } else if (typeName == "char") {
        cType = FFI::CBasicType::CHAR;
    } else if (typeName == "short") {
        cType = FFI::CBasicType::SHORT;
    } else if (typeName == "int") {
        cType = FFI::CBasicType::INT;
    } else if (typeName == "long") {
        cType = FFI::CBasicType::LONG;
    } else if (typeName == "float") {
        cType = FFI::CBasicType::FLOAT;
    } else if (typeName == "double") {
        cType = FFI::CBasicType::DOUBLE;
    } else if (typeName == "size_t") {
        cType = FFI::CBasicType::SIZE_T;
    } else if (typeName == "bool" || typeName == "_Bool") {
        cType = FFI::CBasicType::BOOL;
    } else if (typeName == "int8_t") {
        cType = FFI::CBasicType::INT8;
    } else if (typeName == "uint8_t") {
        cType = FFI::CBasicType::UINT8;
    } else if (typeName == "int16_t") {
        cType = FFI::CBasicType::INT16;
    } else if (typeName == "uint16_t") {
        cType = FFI::CBasicType::UINT16;
    } else if (typeName == "int32_t") {
        cType = FFI::CBasicType::INT32;
    } else if (typeName == "uint32_t") {
        cType = FFI::CBasicType::UINT32;
    } else if (typeName == "int64_t") {
        cType = FFI::CBasicType::INT64;
    } else if (typeName == "uint64_t") {
        cType = FFI::CBasicType::UINT64;
    } else {
        reportError("\xd9\x86\xd9\x88\xd8\xb9 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb9\xd8\xb1\xd9\x88\xd9\x81: " + typeName);  // نوع غير معروف:
        return nullptr;
    }
    
    advance();
    return FFI::makeBasicType(cType);
}

FFI::CTypePtr ExternParser::parsePointerType(FFI::CTypePtr baseType) {
    return FFI::makePointerType(baseType);
}

FFI::CTypePtr ExternParser::parseArrayType(FFI::CTypePtr elementType) {
    if (!consume(Lexer::TokenType::LEFT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 [")) {  // متوقع [
        return nullptr;
    }
    
    size_t size = 0;
    
    // (AR) حجم المصفوفة (اختياري) / (EN) array size (optional)
    if (check(Lexer::TokenType::NUMBER) || check(Lexer::TokenType::INTEGER_LITERAL)) {
        try {
            size = std::stoull(currentToken_.value);
        } catch (...) {
            reportError("\xd8\xad\xd8\xac\xd9\x85 \xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9 \xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb5\xd8\xa7\xd9\x84\xd8\xad");  // حجم مصفوفة غير صالح
            return nullptr;
        }
        advance();
    }
    
    if (!consume(Lexer::TokenType::RIGHT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 ]")) {  // متوقع ]
        return nullptr;
    }
    
    return FFI::makeArrayType(elementType, size);
}

ExternFunctionDeclAST::OwnershipAnnotations ExternParser::parseOwnershipAnnotations() {
    ExternFunctionDeclAST::OwnershipAnnotations annotations;
    
    // (AR) تحليل التعليقات التوضيحية بالصيغة #[...]
    // (EN) Parse annotations in #[...] format
    
    while (check(Lexer::TokenType::HASH) || currentToken_.value == "#") {
        advance();
        
        if (!consume(Lexer::TokenType::LEFT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 [")) {  // متوقع [
            break;
        }
        
        if (!check(Lexer::TokenType::IDENTIFIER)) {
            reportError("\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb9\xd9\x84\xd9\x8a\xd9\x82");  // متوقع اسم التعليق
            break;
        }
        
        std::string annotationName = currentToken_.value;
        advance();
        
        // (AR) ملك_النتيجة / (EN) owns_result
        if (annotationName == "\xd9\x85\xd9\x84\xd9\x83_\xd8\xa7\xd9\x84\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9" ||  // ملك_النتيجة
            annotationName == "owns_result") {
            annotations.ownsResult = true;
        }
        // (AR) لا_يُحرر / (EN) no_free
        else if (annotationName == "\xd9\x84\xd8\xa7_\xd9\x8a\xd9\x8f\xd8\xad\xd8\xb1\xd8\xb1" ||  // لا_يُحرر
                 annotationName == "no_free") {
            annotations.noFree = true;
        }
        // (AR) مستعار / (EN) borrowed
        else if (annotationName == "\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xa7\xd8\xb1" ||  // مستعار
                 annotationName == "borrowed") {
            annotations.resultBorrowed = true;
        }
        // (AR) يُحرر_بـ / (EN) freed_by
        else if (annotationName == "\xd9\x8a\xd9\x8f\xd8\xad\xd8\xb1\xd8\xb1_\xd8\xa8\xd9\x80" ||  // يُحرر_بـ
                 annotationName == "freed_by") {
            // (AR) توقع : واسم الدالة / (EN) expect : and function name
            if (check(Lexer::TokenType::COLON)) {
                advance();
                if (check(Lexer::TokenType::IDENTIFIER)) {
                    annotations.freeFunction = currentToken_.value;
                    advance();
                }
            }
        }
        
        consume(Lexer::TokenType::RIGHT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 ]");  // متوقع ]
    }
    
    return annotations;
}

FFI::CallingConvention ExternParser::parseCallingConvention() {
    // (AR) افتراضي: AUTO (يُحدد حسب المنصة)
    // (EN) default: AUTO (determined by platform)
    
    // (AR) تحليل التعليقات التوضيحية للاتفاقية
    // (EN) Parse calling convention annotations
    
    if (check(Lexer::TokenType::HASH) || currentToken_.value == "#") {
        // (AR) نحتفظ بالموقع للرجوع إليه إذا لم يكن تعليق اتفاقية
        // (EN) Save position to return if not a convention annotation
        auto savedToken = currentToken_;
        
        advance();
        
        if (check(Lexer::TokenType::LEFT_BRACKET)) {
            advance();
            
            if (check(Lexer::TokenType::IDENTIFIER)) {
                std::string convName = currentToken_.value;
                
                // استدعاء_س / cdecl
                if (convName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd8\xb3" ||
                    convName == "cdecl") {
                    advance();
                    consume(Lexer::TokenType::RIGHT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 ]");  // متوقع ]
                    return FFI::CallingConvention::CDECL;
                }
                // استدعاء_قياسي / stdcall
                else if (convName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3\xd9\x8a" ||
                         convName == "stdcall") {
                    advance();
                    consume(Lexer::TokenType::RIGHT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 ]");
                    return FFI::CallingConvention::STDCALL;
                }
                // استدعاء_سريع / fastcall
                else if (convName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd8\xb3\xd8\xb1\xd9\x8a\xd8\xb9" ||
                         convName == "fastcall") {
                    advance();
                    consume(Lexer::TokenType::RIGHT_BRACKET, "\xd9\x85\xd8\xaa\xd9\x88\xd9\x82\xd8\xb9 ]");
                    return FFI::CallingConvention::FASTCALL;
                }
            }
        }
        
        // (AR) ليس تعليق اتفاقية، الرجوع للموقع السابق
        // (EN) Not a convention annotation, return to previous position
        // Note: this simplified version doesn't properly handle backtracking
    }
    
    return FFI::CallingConvention::AUTO;
}

std::optional<FFI::CBasicType> ExternParser::arabicTypeToCBasicType(const std::string& name) const {
    // (AR) تحويل أسماء الأنواع العربية إلى CBasicType
    // (EN) Convert Arabic type names to CBasicType
    
    // فراغ - void
    if (name == "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba" || name == "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba_\xd8\xb3") {
        return FFI::CBasicType::VOID;
    }
    // حرف - char
    if (name == "\xd8\xad\xd8\xb1\xd9\x81" || name == "\xd8\xad\xd8\xb1\xd9\x81_\xd8\xb3") {
        return FFI::CBasicType::CHAR;
    }
    // قصير - short
    if (name == "\xd9\x82\xd8\xb5\xd9\x8a\xd8\xb1" || name == "\xd9\x82\xd8\xb5\xd9\x8a\xd8\xb1_\xd8\xb3") {
        return FFI::CBasicType::SHORT;
    }
    // صحيح - int
    if (name == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" || name == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad_\xd8\xb3") {
        return FFI::CBasicType::INT;
    }
    // طويل - long
    if (name == "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84" || name == "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xb3") {
        return FFI::CBasicType::LONG;
    }
    // طويل_جداً - long long
    if (name == "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xac\xd8\xaf\xd8\xa7\xd9\x8b" || 
        name == "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xac\xd8\xaf\xd8\xa7\xd9\x8b_\xd8\xb3") {
        return FFI::CBasicType::LONGLONG;
    }
    // عشري - float
    if (name == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a" || name == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a_\xd8\xb3") {
        return FFI::CBasicType::FLOAT;
    }
    // مزدوج - double
    if (name == "\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac" || name == "\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac_\xd8\xb3") {
        return FFI::CBasicType::DOUBLE;
    }
    // حجم - size_t
    if (name == "\xd8\xad\xd8\xac\xd9\x85" || name == "\xd8\xad\xd8\xac\xd9\x85_\xd8\xb3") {
        return FFI::CBasicType::SIZE_T;
    }
    // منطقي - bool
    if (name == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a") {
        return FFI::CBasicType::BOOL;
    }
    // ص8 - int8_t
    if (name == "\xd8\xb58") {
        return FFI::CBasicType::INT8;
    }
    // ص_غ8 - uint8_t
    if (name == "\xd8\xb5_\xd8\xba8") {
        return FFI::CBasicType::UINT8;
    }
    // ص16 - int16_t
    if (name == "\xd8\xb516") {
        return FFI::CBasicType::INT16;
    }
    // ص_غ16 - uint16_t
    if (name == "\xd8\xb5_\xd8\xba16") {
        return FFI::CBasicType::UINT16;
    }
    // ص32 - int32_t
    if (name == "\xd8\xb532") {
        return FFI::CBasicType::INT32;
    }
    // ص_غ32 - uint32_t
    if (name == "\xd8\xb5_\xd8\xba32") {
        return FFI::CBasicType::UINT32;
    }
    // ص64 - int64_t
    if (name == "\xd8\xb564") {
        return FFI::CBasicType::INT64;
    }
    // ص_غ64 - uint64_t
    if (name == "\xd8\xb5_\xd8\xba64") {
        return FFI::CBasicType::UINT64;
    }
    
    return std::nullopt;
}

// ============================================================================
//                    (AR) دوال مساعدة عامة
//                    (EN) General Helper Functions
// ============================================================================

std::string linkageToString(ExternLinkage linkage) {
    switch (linkage) {
        case ExternLinkage::C:
            return "C";
        case ExternLinkage::CPP:
            return "C++";
        case ExternLinkage::SYSTEM:
            return "system";
        default:
            return "unknown";
    }
}

ExternLinkage parseLinkageFromString(const std::string& str) {
    if (str == "C" || str == "c" || str == "\xd8\xb3") {  // س
        return ExternLinkage::C;
    }
    if (str == "C++" || str == "cpp" || str == "\xd8\xb3\xd9\x8a++") {  // سي++
        return ExternLinkage::CPP;
    }
    if (str == "system" || str == "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85") {  // نظام
        return ExternLinkage::SYSTEM;
    }
    return ExternLinkage::UNKNOWN;
}

bool isExternKeyword(const Lexer::Token& token) {
    if (token.type != Lexer::TokenType::IDENTIFIER) {
        return false;
    }
    
    return token.value == ExternKeywords::KHARIJI ||
           token.value == ExternKeywords::KHARIJI_C ||
           token.value == ExternKeywords::KHARIJI_CPP ||
           token.value == ExternKeywords::KHARIJI_NIZAM ||
           token.value == "extern" ||
           token.value == "extern_c" ||
           token.value == "extern_cpp";
}

} // namespace Parser
} // namespace Sad
