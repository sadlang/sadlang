// (AR) سجل الأدوات: اكتشاف وتسجيل كل الأدوات المتوفرة في النظام
// (EN) Tool registry: discover and register all available tools in the system

#pragma once

#include "hub/tool_descriptor.h"
#include <string>
#include <vector>
#include <optional>

namespace Sad
{
    namespace Hub
    {

        /**
         * @brief (AR) سجل الأدوات — يكتشف ويُدير قائمة الأدوات المثبَّتة
         * @brief (EN) Tool registry — discovers and manages installed tools
         *
         * @details
         * (AR) ترتيب البحث:
         *   1. مجلد التنفيذي نفسه (نفس مجلد sad.exe)
         *   2. <مجلد التنفيذي>/tools/
         *   3. متغير البيئة SAD_TOOLS_PATH (مفصول بـ ; على Windows أو : على Unix)
         *   4. PATH العام للنظام
         *
         * (EN) Search order:
         *   1. Hub executable's own directory
         *   2. <hub-dir>/tools/
         *   3. SAD_TOOLS_PATH env var (semicolon on Windows, colon on Unix)
         *   4. System PATH
         */
        class ToolRegistry
        {
        public:
            ToolRegistry();
            ~ToolRegistry();

            /**
             * @brief (AR) اكتشاف كل الأدوات (يُملأ القائمة الداخلية)
             * @brief (EN) Discover all tools (fills internal list)
             * @param hubExecutablePath (AR) المسار الكامل لتنفيذي Hub
             *                         (EN) Full path to Hub executable
             */
            void discover(const std::string &hubExecutablePath);

            /**
             * @brief (AR) البحث عن أداة بالاسم (مثل "run", "build")
             * @brief (EN) Find tool by name
             */
            std::optional<ToolDescriptor> find(const std::string &name) const;

            /**
             * @brief (AR) قائمة كل الأدوات المكتشَفة (للعرض في --help)
             * @brief (EN) List of all discovered tools (for --help display)
             */
            const ToolList &all() const { return tools_; }

            /**
             * @brief (AR) إضافة أداة مدمجة (builtin) مثل help/version
             * @brief (EN) Add a builtin tool like help/version
             */
            void addBuiltin(const std::string &name, const std::string &description);

            /**
             * @brief (AR) قائمة المسارات التي بُحث فيها (للتشخيص)
             * @brief (EN) Paths that were searched (for diagnostics)
             */
            const std::vector<std::string> &searchedPaths() const { return searchedPaths_; }

        private:
            void scanDirectory(const std::string &dir);
            void addToolFromPath(const std::string &path);
            bool isToolExecutable(const std::string &filename, std::string &outName) const;

            ToolList tools_;
            std::vector<std::string> searchedPaths_;
        };

    } // namespace Hub
} // namespace Sad
