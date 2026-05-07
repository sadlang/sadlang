/**
 * @file explanation_level.cpp
 * @brief (AR) تنفيذ parsers ExplanationLevel و Language
 *        (EN) Implementation of ExplanationLevel and Language parsers
 */

#include "explanation_level.h"
#include <algorithm>
#include <cctype>

namespace Sad
{
    namespace Errors
    {

        namespace
        {
            // (AR) تحويل النص إلى أحرف صغيرة لمقارنة غير حساسة لحالة الأحرف
            // (EN) Lowercase string for case-insensitive matching
            std::string toLower(std::string s)
            {
                std::transform(s.begin(), s.end(), s.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });
                return s;
            }
        } // anonymous namespace

        ExplanationLevel parseExplanationLevel(const std::string &s)
        {
            const std::string v = toLower(s);
            if (v == "brief" || v == "موجز")
                return ExplanationLevel::BRIEF;
            if (v == "normal" || v == "عادي")
                return ExplanationLevel::NORMAL;
            if (v == "detailed" || v == "مفصل")
                return ExplanationLevel::DETAILED;
            if (v == "teacher" || v == "معلم")
                return ExplanationLevel::TEACHER;
            return ExplanationLevel::NORMAL; // (AR) الافتراضي / (EN) Default
        }

        std::string toString(ExplanationLevel level)
        {
            switch (level)
            {
            case ExplanationLevel::BRIEF:
                return "brief";
            case ExplanationLevel::NORMAL:
                return "normal";
            case ExplanationLevel::DETAILED:
                return "detailed";
            case ExplanationLevel::TEACHER:
                return "teacher";
            }
            return "normal";
        }

        Language parseLanguage(const std::string &s)
        {
            const std::string v = toLower(s);
            if (v == "ar" || v == "arabic" || v == "عربي")
                return Language::ARABIC;
            if (v == "en" || v == "english" || v == "إنجليزي")
                return Language::ENGLISH;
            if (v == "both" || v == "كلاهما")
                return Language::BOTH;
            return Language::BOTH; // (AR) الافتراضي ثنائي اللغة / (EN) Default bilingual
        }

        std::string toString(Language lang)
        {
            switch (lang)
            {
            case Language::ARABIC:
                return "ar";
            case Language::ENGLISH:
                return "en";
            case Language::BOTH:
                return "both";
            }
            return "both";
        }

    } // namespace Errors
} // namespace Sad
