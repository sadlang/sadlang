// (AR) عرض رسائل المساعدة الموحَّدة لمركز الأدوات
// (EN) Unified help message rendering for the tool hub

#pragma once

#include "hub/tool_registry.h"
#include <string>

namespace Sad
{
    namespace Hub
    {

        /**
         * @brief (AR) عارض المساعدة — يُولّد رسائل --help و --list و --version
         * @brief (EN) Help renderer — generates --help, --list, --version messages
         */
        class HelpRenderer
        {
        public:
            explicit HelpRenderer(const ToolRegistry &registry);

            /**
             * @brief (AR) رسالة المساعدة العامة (sad --help)
             * @brief (EN) General help message (sad --help)
             */
            std::string renderMainHelp(const std::string &hubVersion) const;

            /**
             * @brief (AR) قائمة الأدوات المكتشَفة مع مساراتها (sad --list)
             * @brief (EN) List of discovered tools with their paths (sad --list)
             */
            std::string renderToolList() const;

            /**
             * @brief (AR) معلومات الإصدار (sad --version)
             * @brief (EN) Version info (sad --version)
             */
            std::string renderVersion(const std::string &hubVersion) const;

            /**
             * @brief (AR) رسالة "أمر غير معروف" مع اقتراح إصلاح
             * @brief (EN) "Unknown command" message with suggestion
             */
            std::string renderUnknownCommand(const std::string &given) const;

        private:
            const ToolRegistry &registry_;

            // (AR) أقرب اسم أداة لـ given بحساب Levenshtein
            // (EN) Nearest tool name to `given` using Levenshtein distance
            std::string findClosestMatch(const std::string &given) const;
        };

    } // namespace Hub
} // namespace Sad
