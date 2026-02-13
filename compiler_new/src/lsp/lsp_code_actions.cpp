/// تنفيذ إجراءات الكود

#include "lsp_code_actions.h"
#include <sstream>

namespace sad {
namespace lsp {

LspCodeActions::LspCodeActions(LspServer& server) : server_(server) {}

void LspCodeActions::registerHandlers() {
    server_.registerHandler("textDocument/codeAction",
        [this](const std::string& p) { return handleCodeAction(p); });
    server_.registerHandler("codeAction/resolve",
        [this](const std::string& p) { return handleCodeActionResolve(p); });
}

std::string LspCodeActions::handleCodeAction(const std::string& params) {
    /// قائمة الإصلاحات السريعة المتاحة للغة ص:
    /// - إضافة استيراد مفقود
    /// - إزالة استيراد غير مستخدم
    /// - إضافة نوع مفقود
    /// - تحويل إلى ثابت
    /// - تغليف بمعالج أخطاء
    // TODO: تحليل التشخيصات والسياق لتقديم الإصلاحات
    return "[]";
}

std::string LspCodeActions::handleCodeActionResolve(const std::string& params) {
    // TODO: حساب تعديلات النص لإجراء محدد
    return params;
}

} // namespace lsp
} // namespace sad
