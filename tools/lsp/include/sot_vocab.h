// بسم الله الرحمن الرحيم
// ─────────────────────────────────────────────────────────────────────────────
// ملف: sot_vocab.h
// الوصف: مفردات مشتقّة من مصدر الحقيقة (المعجم المُولَّد) — مشتركة بين ميزات الخادم
// ─────────────────────────────────────────────────────────────────────────────
//
// (AR) نقطة اشتقاق واحدة لمفردات المعجم التي تحتاجها عدّة ميزات (الإكمال،
//      الطيّ، فحص توازن الكتل). قبل هذه الوحدة كانت كلّ ميزة تهرّد قائمتها
//      يدويًّا فتتباعد عن المعجم (10 مقابل 24 كلمة فتح كتلة) — الاشتقاق من
//      Sad::Lexer::Generated::allEntries() يضمن التزامن التلقائيّ: كلمة جديدة
//      بدور «block_opener» في language-truth/keywords.yaml تنساب لكلّ الميزات
//      بلا لمس C++.
// (EN) Single derivation point for lexicon vocabulary shared across features
//      (completion, folding, block-balance checking). Each feature used to
//      hand-maintain its own list which drifted from the lexicon (10 vs 24
//      block openers); deriving from the generated lexicon keeps them in sync.
//
// الحمد لله رب العالمين
// ─────────────────────────────────────────────────────────────────────────────
#pragma once

#include <string>
#include <vector>

namespace sad {
namespace lsp {
namespace vocab {

/// (AR) كلمات فتح الكتل (تُغلَق بـ«نهاية») من المعجم المُولَّد: الكلمة الأساسيّة
///      وبدائلها لكلّ إدخال بدور «block_opener» (24 إدخالًا في المصدر اليوم).
///      تُبنى مرّة واحدة (كسولًا) وتبقى حيّة طوال عمر العمليّة.
/// (EN) Block-opening keywords (closed by «نهاية») from the generated lexicon:
///      primary word + aliases of every entry carrying the "block_opener" role.
///      Built lazily once; lives for the process lifetime.
const std::vector<std::string>& block_opener_words();

/// (AR) كلمات القالب («قالب» وبدائلها) من المعجم المُولَّد. القالب **غلاف** لا
///      يستهلك «نهاية» خاصّة به (TemplateDecl = 'قالب' TemplateParams
///      (FunctionDecl|ClassDecl) — «نهاية» يملكها التصريح الملفوف)، لذا يحتاجها
///      فحص التوازن ليتخطّى الغلاف ويَعُدّ التصريح الملفوف نفسه.
/// (EN) Template words («قالب» + aliases). A template is a *wrapper* owning no
///      «نهاية» of its own; balance checking skips it and counts the wrapped
///      declaration instead.
const std::vector<std::string>& template_words();

/// (AR) كلمات مُدرِكات الخاصّيّة («احصل»/«عيّن» وبدائلها) من المعجم المُولَّد.
///      كلّ مُدرِك يفتح كتلة تُغلَق بـ«نهاية» (سطريّة أو مضمّنة) رغم أنّها بلا
///      دور block_opener في المعجم (كلمات سياقيّة داخل «خاصية» فقط).
/// (EN) Property-accessor words («احصل»/«عيّن» + aliases). Each opens a block
///      closed by «نهاية» although carrying no block_opener role (contextual
///      keywords valid only inside «خاصية»).
const std::vector<std::string>& accessor_block_words();

/// (AR) كلمات لامدا من المعجم المُولَّد. لامدا تعبيرٌ يقع غالبًا وسط السطر،
///      ويفتح كتلة «نهاية» إلّا في صيغة السهم التعبيريّة
///      (Lambda = 'لامدا' '(' P ')' ( '=>' Expr | '=>' Block 'نهاية' | Block 'نهاية' )).
/// (EN) Lambda words. A lambda is an expression (often mid-line) opening a
///      «نهاية» block except in its arrow-expression form.
const std::vector<std::string>& lambda_words();

} // namespace vocab
} // namespace lsp
} // namespace sad
