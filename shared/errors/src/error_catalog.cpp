/**
 * @file error_catalog.cpp
 * @brief (AR) تنفيذ ErrorCatalog: تخزين القوالب + render + استبدال placeholders
 *        (EN) ErrorCatalog implementation: template storage + render + placeholder substitution
 */

#include "error_catalog.h"

#include <sstream>

namespace Sad
{
    namespace Errors
    {

        ErrorCatalog &ErrorCatalog::instance()
        {
            static ErrorCatalog inst;
            return inst;
        }

        void ErrorCatalog::registerTemplate(ErrorCode code, ErrorTemplate tmpl)
        {
            templates_[static_cast<int>(code)] = std::move(tmpl);
        }

        const ErrorTemplate *ErrorCatalog::getTemplate(ErrorCode code) const
        {
            auto it = templates_.find(static_cast<int>(code));
            if (it == templates_.end())
                return nullptr;
            return &it->second;
        }

        void ErrorCatalog::clear() { templates_.clear(); }

        // ─────────────────────────────────────────────────────────────────────────
        // (AR) استبدال placeholders بصيغة {key} — تنفيذ بسيط بدون regex
        // (EN) Placeholder substitution {key} — simple impl without regex
        // ─────────────────────────────────────────────────────────────────────────
        std::string ErrorCatalog::substitute(const std::string &tmpl,
                                             const RenderContext &ctx)
        {
            if (tmpl.find('{') == std::string::npos)
                return tmpl; // (AR) تسريع: لا فجوات / (EN) Fast path: no placeholders
            std::string out;
            out.reserve(tmpl.size() + 16);
            size_t i = 0;
            const size_t n = tmpl.size();
            while (i < n)
            {
                char c = tmpl[i];
                if (c == '{')
                {
                    size_t close = tmpl.find('}', i + 1);
                    if (close == std::string::npos)
                    {
                        // (AR) فجوة غير مغلقة — نسخها كما هي / (EN) Unclosed placeholder — copy as-is
                        out.append(tmpl, i, n - i);
                        break;
                    }
                    std::string key = tmpl.substr(i + 1, close - i - 1);
                    auto it = ctx.placeholders.find(key);
                    if (it != ctx.placeholders.end())
                    {
                        out.append(it->second);
                    }
                    else
                    {
                        // (AR) مفتاح غير موجود — أبقِ {key} كما هو لكشف الأخطاء
                        // (EN) Missing key — keep {key} as-is to surface bugs
                        out.push_back('{');
                        out.append(key);
                        out.push_back('}');
                    }
                    i = close + 1;
                }
                else
                {
                    out.push_back(c);
                    ++i;
                }
            }
            return out;
        }

        // ─────────────────────────────────────────────────────────────────────────
        // (AR) اختيار حقل النص حسب المستوى مع fallback تنازلي إلى brief ثم title
        // (EN) Pick text field per level with fallback chain detailed→normal→brief→title
        // ─────────────────────────────────────────────────────────────────────────
        const std::string &ErrorCatalog::pickAr(const ErrorTemplate &t, ExplanationLevel lvl)
        {
            switch (lvl)
            {
            case ExplanationLevel::TEACHER:
                if (!t.teacherAr.empty())
                    return t.teacherAr;
                // fall-through
            case ExplanationLevel::DETAILED:
                if (!t.detailedAr.empty())
                    return t.detailedAr;
                // fall-through
            case ExplanationLevel::NORMAL:
            case ExplanationLevel::BRIEF:
                if (!t.briefAr.empty())
                    return t.briefAr;
                return t.titleAr;
            }
            return t.titleAr;
        }

        const std::string &ErrorCatalog::pickEn(const ErrorTemplate &t, ExplanationLevel lvl)
        {
            switch (lvl)
            {
            case ExplanationLevel::TEACHER:
                if (!t.teacherEn.empty())
                    return t.teacherEn;
                // fall-through
            case ExplanationLevel::DETAILED:
                if (!t.detailedEn.empty())
                    return t.detailedEn;
                // fall-through
            case ExplanationLevel::NORMAL:
            case ExplanationLevel::BRIEF:
                if (!t.briefEn.empty())
                    return t.briefEn;
                return t.titleEn;
            }
            return t.titleEn;
        }

        ErrorCatalog::Rendered ErrorCatalog::render(ErrorCode code,
                                                    ExplanationLevel level,
                                                    Language lang,
                                                    const RenderContext &ctx) const
        {
            Rendered r;
            const ErrorTemplate *t = getTemplate(code);

            if (!t)
            {
                // (AR) لا قالب — رسالة عامة لتجنّب انهيار النظام
                // (EN) No template — generic fallback so system never collapses
                std::ostringstream osa, ose;
                osa << "خطأ غير موصوف (" << getErrorCodeString(code) << ")";
                ose << "Unspecified error (" << getErrorCodeString(code) << ")";
                r.messageAr = osa.str();
                r.messageEn = ose.str();
                return r;
            }

            // (AR) املأ النصوص حسب اللغة المطلوبة (ARABIC: ar فقط؛ ENGLISH: en فقط؛ BOTH: كلاهما)
            // (EN) Fill texts per requested language (ARABIC: ar only; ENGLISH: en only; BOTH: both)
            if (lang == Language::ARABIC || lang == Language::BOTH)
            {
                r.messageAr = substitute(pickAr(*t, level), ctx);
                if (!t->fixHintAr.empty())
                    r.fixHintAr = substitute(t->fixHintAr, ctx);
            }
            if (lang == Language::ENGLISH || lang == Language::BOTH)
            {
                r.messageEn = substitute(pickEn(*t, level), ctx);
                if (!t->fixHintEn.empty())
                    r.fixHintEn = substitute(t->fixHintEn, ctx);
            }

            // (AR) مثال الكود يُعرض في DETAILED+ فقط
            // (EN) Code example shown at DETAILED+ only
            if ((level == ExplanationLevel::DETAILED || level == ExplanationLevel::TEACHER) && !t->codeExample.empty())
            {
                r.codeExample = t->codeExample;
            }

            return r;
        }

    } // namespace Errors
} // namespace Sad
