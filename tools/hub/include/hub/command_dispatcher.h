// (AR) موزِّع الأوامر: يُحوّل "sad <cmd> args" إلى استدعاء sad-<cmd>.exe
// (EN) Command dispatcher: translates "sad <cmd> args" into sad-<cmd>.exe call

#pragma once

#include "hub/tool_registry.h"
#include <string>
#include <vector>

namespace Sad
{
    namespace Hub
    {

        /**
         * @brief (AR) نتيجة توزيع أمر
         * @brief (EN) Dispatch result
         */
        struct DispatchResult
        {
            int exitCode;      // 0 = نجاح، غير 0 = فشل / 0 = success, non-zero = failure
            std::string error; // رسالة خطأ إذا فشل التوزيع نفسه (لا الأداة)
                               // error message if dispatch itself failed (not the tool)
            bool dispatched;   // هل تم استدعاء أداة فعلاً / was a tool actually invoked
        };

        /**
         * @brief (AR) موزِّع الأوامر — يُشغّل الأداة المطلوبة كـ subprocess
         * @brief (EN) Command dispatcher — runs requested tool as subprocess
         */
        class CommandDispatcher
        {
        public:
            explicit CommandDispatcher(const ToolRegistry &registry);

            /**
             * @brief (AR) توزيع أمر: تشغيل أداة باسمها مع تمرير المعطيات
             * @brief (EN) Dispatch a command: run tool by name with forwarded args
             * @param commandName (AR) اسم الأمر مثل "run" أو "build"
             *                    (EN) command name like "run" or "build"
             * @param args (AR) المعطيات التي ستُمرَّر للأداة
             *             (EN) arguments to forward to the tool
             */
            DispatchResult dispatch(const std::string &commandName,
                                    const std::vector<std::string> &args) const;

        private:
            const ToolRegistry &registry_;

            int runSubprocess(const std::string &executable,
                              const std::vector<std::string> &args) const;
        };

    } // namespace Hub
} // namespace Sad
