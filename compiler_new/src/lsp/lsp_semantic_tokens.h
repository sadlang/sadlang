/// الرموز الدلالية LSP
///
/// يوفر تلوين دلالي متقدم بناءً على تحليل الكود.

#pragma once

#include "lsp_server.h"
#include <string>
#include <vector>

namespace sad {
namespace lsp {

/// أنواع الرموز الدلالية
enum class SemanticTokenType {
    Namespace,    // فضاء_أسماء
    Type,         // نوع
    Class,        // صنف
    Enum,         // تعداد
    Interface,    // واجهة
    Struct,       // هيكل
    TypeParameter,// معامل_نوع
    Parameter,    // معامل
    Variable,     // متغير
    Property,     // خاصية
    EnumMember,   // عضو_تعداد
    Event,        // حدث
    Function,     // دالة
    Method,       // طريقة
    Macro,        // ماكرو
    Keyword,      // كلمة_مفتاحية
    Modifier,     // معدِّل
    Comment,      // تعليق
    String,       // نص
    Number,       // عدد
    Regexp,       // تعبير_نمطي
    Operator,     // عامل
    Decorator,    // مُزيِّن
    COUNT
};

/// معدِّلات الرموز الدلالية
enum class SemanticTokenModifier {
    Declaration   = 1 << 0,
    Definition    = 1 << 1,
    Readonly      = 1 << 2,
    Static        = 1 << 3,
    Deprecated    = 1 << 4,
    Abstract      = 1 << 5,
    Async         = 1 << 6,
    Modification  = 1 << 7,
    Documentation = 1 << 8,
    DefaultLibrary= 1 << 9,
};

/// رمز دلالي واحد
struct SemanticToken {
    int line;
    int startChar;
    int length;
    SemanticTokenType type;
    int modifiers;
};

/// معالج الرموز الدلالية
class LspSemanticTokens {
public:
    explicit LspSemanticTokens(LspServer& server);
    void registerHandlers();

    /// إنشاء JSON لأسطورة الرموز الدلالية
    static std::string buildLegendJson();

private:
    std::string handleSemanticTokensFull(const std::string& params);
    std::string handleSemanticTokensDelta(const std::string& params);
    std::string handleSemanticTokensRange(const std::string& params);

    /// ترميز الرموز بالتنسيق النسبي
    std::string encodeTokens(const std::vector<SemanticToken>& tokens);

    LspServer& server_;
    int resultId_ = 0;
};

} // namespace lsp
} // namespace sad
