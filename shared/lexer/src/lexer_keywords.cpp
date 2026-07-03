// ======================================================================
// lexer_keywords.cpp - تنفيذ جدول الكلمات (v4.1)
// (AR) لا يحتوي هذا الملف على أي سلاسل عربية أو قوائم كلمات يدوية.
//      كل البيانات مأخوذة من Generated::allEntries() المولَّدة من
//      language-truth/keywords.yaml.
// (EN) This file contains zero hand-written Arabic strings or keyword
//      lists. All data flows from Generated::allEntries() produced from
//      language-truth/keywords.yaml.
// ======================================================================

#include "lexer_keywords.h"

#include <algorithm>
#include <stdexcept>

namespace Sad {
namespace Lexer {

// ── (AR) متغيرات ثابتة ──
bool                                                            KeywordTable::initialized_ = false;
std::unordered_map<std::string, const KeywordTable::KeywordEntry*> KeywordTable::wordIndex_;
std::unordered_map<int, const KeywordTable::KeywordEntry*>         KeywordTable::typeIndex_;
std::vector<std::string>                                           KeywordTable::emittedWords_;
std::vector<std::vector<std::string>>                              KeywordTable::byCategory_;
std::unordered_map<std::string, std::vector<std::string>>          KeywordTable::byRole_;
const std::vector<std::string>                                     KeywordTable::kEmpty_{};

// ── (AR) أداة داخلية: يضمن التهيئة ──
namespace {
inline void ensureInit() {
    if (!KeywordTable::isInitialized()) {
        KeywordTable::initialize();
    }
}
} // namespace

// ──────────────────────────────────────────────────────────────────────
// (AR) التهيئة — تبني كل الفهارس مرة واحدة من Generated::allEntries()
// (EN) Initialization — builds all indices once from Generated::allEntries()
// ──────────────────────────────────────────────────────────────────────
void KeywordTable::initialize() {
    if (initialized_) return;

    const auto& entries = Generated::allEntries();

    // (AR) حجز الفئات الأربع
    byCategory_.assign(4, {});

    for (const auto& e : entries) {
        // (AR) فهرسة بالنوع — نوع واحد لكل إدخال
        typeIndex_.emplace(static_cast<int>(e.type), &e);

        // (AR) فهرسة بالكلمة الرئيسية
        wordIndex_.emplace(e.primaryWord, &e);

        // (AR) فهرسة بالأسماء البديلة (تشير لنفس الإدخال)
        for (const auto& alias : e.aliases) {
            wordIndex_.emplace(alias, &e);
        }

        // (AR) تجميع حسب الفئة (الكلمة الرئيسية فقط، بترتيب YAML)
        const auto catIdx = static_cast<std::size_t>(e.category);
        if (catIdx < byCategory_.size()) {
            byCategory_[catIdx].push_back(e.primaryWord);
        }

        // (AR) تجميع حسب الدور
        for (const auto& role : e.roles) {
            byRole_[role].push_back(e.primaryWord);
        }

        // (AR) كلمات Lexer المُصدرة فقط (للتوافق مع getAllKeywords)
        if (e.emittedByLexer) {
            emittedWords_.push_back(e.primaryWord);
            for (const auto& alias : e.aliases) {
                emittedWords_.push_back(alias);
            }
        }
    }

    std::sort(emittedWords_.begin(), emittedWords_.end());
    initialized_ = true;
}

bool KeywordTable::isInitialized() {
    return initialized_;
}

// ──────────────────────────────────────────────────────────────────────
// (AR) واجهة Lexer
// ──────────────────────────────────────────────────────────────────────
bool KeywordTable::isKeyword(std::string_view word) {
    ensureInit();
    auto it = wordIndex_.find(std::string(word));
    return it != wordIndex_.end() && it->second->emittedByLexer;
}

TokenType KeywordTable::getKeywordType(std::string_view word) {
    ensureInit();
    auto it = wordIndex_.find(std::string(word));
    if (it == wordIndex_.end() || !it->second->emittedByLexer) {
        throw std::runtime_error(
            "KeywordTable::getKeywordType: not an emitted keyword: " + std::string(word));
    }
    return it->second->type;
}

std::string KeywordTable::getKeyword(TokenType type) {
    ensureInit();
    auto it = typeIndex_.find(static_cast<int>(type));
    if (it == typeIndex_.end() || !it->second->emittedByLexer) {
        throw std::runtime_error(
            "KeywordTable::getKeyword: TokenType is not an emitted keyword");
    }
    return it->second->primaryWord;
}

// ──────────────────────────────────────────────────────────────────────
// (AR) واجهة Parser
// ──────────────────────────────────────────────────────────────────────
const KeywordTable::KeywordEntry* KeywordTable::getEntry(TokenType type) {
    ensureInit();
    auto it = typeIndex_.find(static_cast<int>(type));
    return (it == typeIndex_.end()) ? nullptr : it->second;
}

const KeywordTable::KeywordEntry* KeywordTable::getEntry(std::string_view word) {
    ensureInit();
    auto it = wordIndex_.find(std::string(word));
    return (it == wordIndex_.end()) ? nullptr : it->second;
}

std::optional<KeywordTable::KeywordCategory>
KeywordTable::getCategory(std::string_view word) {
    const auto* e = getEntry(word);
    if (!e) return std::nullopt;
    return e->category;
}

const std::vector<std::string>&
KeywordTable::getKeywordsByCategory(KeywordCategory category) {
    ensureInit();
    const auto idx = static_cast<std::size_t>(category);
    if (idx >= byCategory_.size()) return kEmpty_;
    return byCategory_[idx];
}

const std::vector<std::string>&
KeywordTable::getKeywordsByRole(std::string_view role) {
    ensureInit();
    auto it = byRole_.find(std::string(role));
    return (it == byRole_.end()) ? kEmpty_ : it->second;
}

bool KeywordTable::shouldEmitAsKeyword(TokenType type) {
    ensureInit();
    auto it = typeIndex_.find(static_cast<int>(type));
    return it != typeIndex_.end() && it->second->emittedByLexer;
}

std::vector<std::string> KeywordTable::getAllKeywords() {
    ensureInit();
    return emittedWords_;
}

} // namespace Lexer
} // namespace Sad
