// (AR) واجهة وصف أداة في مركز الأدوات
// (EN) Tool descriptor interface for the tool hub

#pragma once

#include <string>
#include <vector>

namespace Sad
{
    namespace Hub
    {

        /**
         * @brief (AR) معلومات أداة مكتشَفة (الاسم، المسار، الإصدار، الوصف)
         * @brief (EN) Discovered tool info (name, path, version, description)
         */
        struct ToolDescriptor
        {
            std::string name;        // مثل: "run", "build", "fmt"
            std::string executable;  // المسار الكامل: C:/.../sad-run.exe
            std::string version;     // مثل: "1.0.0" أو "" إذا غير معروف
            std::string description; // وصف قصير من --description
            bool isBuiltin;          // true إذا مدمج في Hub (help, version)

            ToolDescriptor() : isBuiltin(false) {}

            ToolDescriptor(std::string n, std::string exe, std::string desc = "", bool builtin = false)
                : name(std::move(n)), executable(std::move(exe)),
                  description(std::move(desc)), isBuiltin(builtin) {}
        };

        using ToolList = std::vector<ToolDescriptor>;

    } // namespace Hub
} // namespace Sad
