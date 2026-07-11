// بسم الله الرحمن الرحيم
// ─────────────────────────────────────────────────────────────────────────────
// ملف: sot_vocab.cpp
// الوصف: اشتقاق المفردات المشتركة من المعجم المُولَّد (مصدر الحقيقة)
// ─────────────────────────────────────────────────────────────────────────────
// الحمد لله رب العالمين

#include "sot_vocab.h"

// (AR) المعجم المُولَّد من language-truth/keywords.yaml (keywords_generated.h في
//      shared/lexer/generated) — يُضمَّن عبر مسار التضمين لا بمسار نسبيّ هشّ.
// (EN) The lexicon generated from language-truth/keywords.yaml.
#include "keywords_generated.h"

namespace sad {
namespace lsp {
namespace vocab {

namespace {
/// (AR) اسم دور فتح الكتلة كما في مصدر الحقيقة (keywords.yaml → roles).
/// (EN) The block-opener role name as spelled in the SoT.
constexpr const char* kBlockOpenerRole = "block_opener";

/// (AR) اشتقاق كلمات (أساسيّة + بدائل) كلّ الإدخالات ذات نوع رمز معيّن.
/// (EN) Derive primary + alias words of all entries with a given token type.
std::vector<std::string> words_of_token(Sad::Lexer::TokenType type) {
    std::vector<std::string> out;
    for (const auto& entry : Sad::Lexer::Generated::allEntries()) {
        if (entry.type != type) continue;
        out.emplace_back(entry.primaryWord);
        for (const auto& alias : entry.aliases) out.emplace_back(alias);
    }
    return out;
}
} // namespace

const std::vector<std::string>& block_opener_words() {
    static const std::vector<std::string> words = [] {
        std::vector<std::string> out;
        for (const auto& entry : Sad::Lexer::Generated::allEntries()) {
            for (const auto& role : entry.roles) {
                if (role == kBlockOpenerRole) {
                    out.emplace_back(entry.primaryWord);
                    // (AR) البدائل الإملائيّة (مثل «اذا» لـ«إذا») يقبلها المعجم،
                    //      فيجب أن تقبلها الميزات النصّيّة أيضًا.
                    // (EN) Spelling aliases (e.g. «اذا») are accepted by the
                    //      lexer, so text-based features must accept them too.
                    for (const auto& alias : entry.aliases) out.emplace_back(alias);
                    break;
                }
            }
        }
        return out;
    }();
    return words;
}

const std::vector<std::string>& template_words() {
    static const std::vector<std::string> words =
        words_of_token(Sad::Lexer::TokenType::KEYWORD_TEMPLATE);
    return words;
}

const std::vector<std::string>& accessor_block_words() {
    static const std::vector<std::string> words = [] {
        std::vector<std::string> out =
            words_of_token(Sad::Lexer::TokenType::KEYWORD_GET);
        for (auto& w : words_of_token(Sad::Lexer::TokenType::KEYWORD_SET))
            out.emplace_back(std::move(w));
        return out;
    }();
    return words;
}

const std::vector<std::string>& lambda_words() {
    static const std::vector<std::string> words =
        words_of_token(Sad::Lexer::TokenType::KEYWORD_LAMBDA);
    return words;
}

} // namespace vocab
} // namespace lsp
} // namespace sad
