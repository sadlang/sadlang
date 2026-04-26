// ============================================================================
// (AR) تنفيذ تشخيصات أخطاء الأذونات
//      يُنفِّذ نظام قوالب رسائل الأذونات لـ 15 فئة:
//      FILE_READ, FILE_WRITE, FILE_DELETE, FILE_CREATE,
//      NET_CONNECT, NET_LISTEN, SYSTEM_EXEC, SYSTEM_ENV,
//      CAMERA, MICROPHONE, LOCATION, NOTIFICATIONS, CLIPBOARD,
//      SUBPROCESS, FFI
//
//      التقنية المستخدمة:
//      - Singleton Pattern: مثال واحد يُشارك قوالب الرسائل كـ unordered_map
//      - قوالب TOML: تحتوي placeholder {target} يُستبدل عند الاستدعاء
//      - تنسيق صندوق: box-drawing مع رموز Unicode للطرفيات الداعمة
//
//      كيف يُستخدم: يُستدعى من runtime أو المترجم عند تجاوز صلاحية
//      مثال: permission_manager.cpp يستدعي build() ويمرر رسالة الخطأ للمستخدم
//
// (EN) Implementation of permission error diagnostics.
//      Implements a template-based message system for 15 permission categories.
//      Uses Singleton pattern + unordered_map for efficient template lookup.
//      TOML examples contain {target} placeholder replaced at call time.
//
// المكون: shared/errors
// المسار: shared/errors/src/permission_diagnostics.cpp
// استُبدلت به: compiler/src/diagnostics/permission_errors_ar.cpp (Fix #26)
// ============================================================================

// ─── القسم 1: الرأس الذاتي ───────────────────────────────────────────────────
#include "permission_diagnostics.h"

// ─── القسم 2: رؤوس المكتبة القياسية ─────────────────────────────────────────
#include <algorithm>
#include <sstream>

namespace Sad
{
    namespace Errors
    {
        namespace Permissions
        {

            // ============================================================================
            // (AR) قوالب رسائل الأخطاء / (EN) Error Message Templates
            // ============================================================================

            PermissionDiagnosticBuilder::PermissionDiagnosticBuilder()
            {
                initTemplates();
            }

            void PermissionDiagnosticBuilder::initTemplates()
            {
                // --- FILE_READ ---
                templates_[static_cast<int>(PermissionCategory::FILE_READ)] = {
                    "E-PERM-001",
                    "لا يمكن قراءة الملف",
                    "Cannot read file",
                    "برنامجك لا يملك إذن قراءة هذا الملف.\n"
                    "   لغة ص تطبّق نموذج أمان صارم: كل إذن يجب التصريح به.",
                    "Your program lacks permission to read this file.\n"
                    "   Sad enforces strict security: every permission must be declared.",
                    "أضف إذن القراءة في ملف ص.toml:\n"
                    "   [أذونات]\n"
                    "   ملفات = [\"قراءة:{target}\"]",
                    "Add read permission in ص.toml:\n"
                    "   [permissions]\n"
                    "   files = [\"read:{target}\"]",
                    "[أذونات]\nملفات = [\"قراءة:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/files"};

                // --- FILE_WRITE ---
                templates_[static_cast<int>(PermissionCategory::FILE_WRITE)] = {
                    "E-PERM-002",
                    "لا يمكن الكتابة في الملف",
                    "Cannot write to file",
                    "برنامجك لا يملك إذن الكتابة في هذا الملف.",
                    "Your program lacks permission to write to this file.",
                    "أضف إذن الكتابة في ص.toml:\n"
                    "   [أذونات]\n"
                    "   ملفات = [\"كتابة:{target}\"]",
                    "Add write permission in ص.toml:\n"
                    "   [permissions]\n"
                    "   files = [\"write:{target}\"]",
                    "[أذونات]\nملفات = [\"كتابة:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/files"};

                // --- FILE_DELETE ---
                templates_[static_cast<int>(PermissionCategory::FILE_DELETE)] = {
                    "E-PERM-003",
                    "لا يمكن حذف الملف",
                    "Cannot delete file",
                    "برنامجك لا يملك إذن حذف هذا الملف.",
                    "Your program lacks permission to delete this file.",
                    "أضف إذن الحذف في ص.toml:\n"
                    "   [أذونات]\n"
                    "   ملفات = [\"حذف:{target}\"]",
                    "Add delete permission in ص.toml:\n"
                    "   [permissions]\n"
                    "   files = [\"delete:{target}\"]",
                    "[أذونات]\nملفات = [\"حذف:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/files"};

                // --- FILE_CREATE ---
                templates_[static_cast<int>(PermissionCategory::FILE_CREATE)] = {
                    "E-PERM-004",
                    "لا يمكن إنشاء الملف",
                    "Cannot create file",
                    "برنامجك لا يملك إذن إنشاء ملفات في هذا المسار.",
                    "Your program lacks permission to create files in this path.",
                    "أضف إذن الإنشاء في ص.toml:\n"
                    "   [أذونات]\n"
                    "   ملفات = [\"إنشاء:{target}\"]",
                    "Add create permission in ص.toml.",
                    "[أذونات]\nملفات = [\"إنشاء:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/files"};

                // --- NET_CONNECT ---
                templates_[static_cast<int>(PermissionCategory::NET_CONNECT)] = {
                    "E-PERM-010",
                    "لا يمكن الاتصال بالشبكة",
                    "Cannot connect to network",
                    "برنامجك لا يملك إذن الاتصال بعناوين الشبكة الخارجية.",
                    "Your program lacks permission to make outbound network connections.",
                    "أضف إذن الشبكة في ص.toml:\n"
                    "   [أذونات]\n"
                    "   شبكة = [\"اتصال:{target}\"]",
                    "Add network permission in ص.toml:\n"
                    "   [permissions]\n"
                    "   network = [\"connect:{target}\"]",
                    "[أذونات]\nشبكة = [\"اتصال:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/network"};

                // --- NET_LISTEN ---
                templates_[static_cast<int>(PermissionCategory::NET_LISTEN)] = {
                    "E-PERM-011",
                    "لا يمكن الاستماع على المنفذ",
                    "Cannot listen on port",
                    "برنامجك لا يملك إذن الاستماع على هذا المنفذ.",
                    "Your program lacks permission to listen on this port.",
                    "أضف إذن الاستماع في ص.toml:\n"
                    "   [أذونات]\n"
                    "   شبكة = [\"استماع:{target}\"]",
                    "Add listen permission in ص.toml.",
                    "[أذونات]\nشبكة = [\"استماع:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/network"};

                // --- SYSTEM_EXEC ---
                templates_[static_cast<int>(PermissionCategory::SYSTEM_EXEC)] = {
                    "E-PERM-020",
                    "لا يمكن تنفيذ أوامر النظام",
                    "Cannot execute system commands",
                    "برنامجك لا يملك إذن تنفيذ أوامر النظام مباشرة.\n"
                    "   هذا الإذن خطير — تأكد أنك تحتاجه فعلاً.",
                    "Your program lacks permission to execute system commands.\n"
                    "   This is a dangerous permission — ensure you truly need it.",
                    "أضف إذن التنفيذ في ص.toml (بتحذير):\n"
                    "   [أذونات]\n"
                    "   نظام = [\"تنفيذ:{target}\"]  # ⚠️ خطير",
                    "Add exec permission in ص.toml (with caution).",
                    "[أذونات]\nنظام = [\"تنفيذ:{target}\"]  # خطير",
                    "https://sad-lang.dev/docs/permissions/system"};

                // --- SYSTEM_ENV ---
                templates_[static_cast<int>(PermissionCategory::SYSTEM_ENV)] = {
                    "E-PERM-021",
                    "لا يمكن قراءة متغيرات البيئة",
                    "Cannot read environment variables",
                    "برنامجك لا يملك إذن الوصول لمتغيرات البيئة.",
                    "Your program lacks permission to access environment variables.",
                    "أضف إذن البيئة في ص.toml:\n"
                    "   [أذونات]\n"
                    "   نظام = [\"بيئة:{target}\"]",
                    "Add environment permission in ص.toml.",
                    "[أذونات]\nنظام = [\"بيئة:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/system"};

                // --- CAMERA ---
                templates_[static_cast<int>(PermissionCategory::CAMERA)] = {
                    "E-PERM-030",
                    "لا يمكن الوصول للكاميرا",
                    "Cannot access camera",
                    "برنامجك يحتاج إذن صريحاً للوصول للكاميرا.",
                    "Your program requires explicit permission to access the camera.",
                    "أضف إذن الكاميرا في ص.toml:\n"
                    "   [أذونات]\n"
                    "   أجهزة = [\"كاميرا\"]",
                    "Add camera permission in ص.toml.",
                    "[أذونات]\nأجهزة = [\"كاميرا\"]",
                    "https://sad-lang.dev/docs/permissions/hardware"};

                // --- MICROPHONE ---
                templates_[static_cast<int>(PermissionCategory::MICROPHONE)] = {
                    "E-PERM-031",
                    "لا يمكن الوصول للميكروفون",
                    "Cannot access microphone",
                    "برنامجك يحتاج إذناً للميكروفون.",
                    "Your program requires microphone permission.",
                    "أضف إذن الميكروفون في ص.toml:\n"
                    "   [أذونات]\n"
                    "   أجهزة = [\"ميكروفون\"]",
                    "Add microphone permission in ص.toml.",
                    "[أذونات]\nأجهزة = [\"ميكروفون\"]",
                    "https://sad-lang.dev/docs/permissions/hardware"};

                // --- LOCATION ---
                templates_[static_cast<int>(PermissionCategory::LOCATION)] = {
                    "E-PERM-032",
                    "لا يمكن الوصول للموقع الجغرافي",
                    "Cannot access location",
                    "برنامجك يحتاج إذناً للموقع الجغرافي.",
                    "Your program requires location permission.",
                    "أضف إذن الموقع في ص.toml:\n"
                    "   [أذونات]\n"
                    "   أجهزة = [\"موقع\"]",
                    "Add location permission in ص.toml.",
                    "[أذونات]\nأجهزة = [\"موقع\"]",
                    "https://sad-lang.dev/docs/permissions/hardware"};

                // --- NOTIFICATIONS ---
                templates_[static_cast<int>(PermissionCategory::NOTIFICATIONS)] = {
                    "E-PERM-033",
                    "لا يمكن إرسال الإشعارات",
                    "Cannot send notifications",
                    "برنامجك يحتاج إذناً لإرسال الإشعارات.",
                    "Your program requires notification permission.",
                    "أضف إذن الإشعارات في ص.toml:\n"
                    "   [أذونات]\n"
                    "   أجهزة = [\"إشعارات\"]",
                    "Add notifications permission in ص.toml.",
                    "[أذونات]\nأجهزة = [\"إشعارات\"]",
                    "https://sad-lang.dev/docs/permissions/hardware"};

                // --- CLIPBOARD ---
                templates_[static_cast<int>(PermissionCategory::CLIPBOARD)] = {
                    "E-PERM-034",
                    "لا يمكن الوصول للحافظة",
                    "Cannot access clipboard",
                    "برنامجك يحتاج إذناً للحافظة.",
                    "Your program requires clipboard permission.",
                    "أضف إذن الحافظة في ص.toml:\n"
                    "   [أذونات]\n"
                    "   نظام = [\"حافظة\"]",
                    "Add clipboard permission in ص.toml.",
                    "[أذونات]\nنظام = [\"حافظة\"]",
                    "https://sad-lang.dev/docs/permissions/system"};

                // --- SUBPROCESS ---
                templates_[static_cast<int>(PermissionCategory::SUBPROCESS)] = {
                    "E-PERM-040",
                    "لا يمكن إنشاء عملية فرعية",
                    "Cannot create subprocess",
                    "برنامجك لا يملك إذن إنشاء عمليات فرعية.",
                    "Your program lacks permission to create subprocesses.",
                    "أضف إذن العملية الفرعية في ص.toml:\n"
                    "   [أذونات]\n"
                    "   نظام = [\"عملية_فرعية:{target}\"]",
                    "Add subprocess permission in ص.toml.",
                    "[أذونات]\nنظام = [\"عملية_فرعية:{target}\"]",
                    "https://sad-lang.dev/docs/permissions/system"};

                // --- FFI ---
                templates_[static_cast<int>(PermissionCategory::FFI)] = {
                    "E-PERM-050",
                    "لا يمكن استدعاء مكتبة خارجية (FFI)",
                    "Cannot call foreign library (FFI)",
                    "برنامجك يحتاج إذناً صريحاً للوصول لمكتبات C/C++ الخارجية.",
                    "Your program requires explicit permission to call external C/C++ libraries.",
                    "أضف إذن FFI في ص.toml:\n"
                    "   [أذونات]\n"
                    "   ffi = [\"{target}\"]",
                    "Add FFI permission in ص.toml:\n"
                    "   [permissions]\n"
                    "   ffi = [\"{target}\"]",
                    "[أذونات]\nffi = [\"{target}\"]",
                    "https://sad-lang.dev/docs/permissions/ffi"};
            }

            // ============================================================================
            // PermissionDiagnosticBuilder::getTemplate()
            // ============================================================================

            const PermissionErrorTemplate &PermissionDiagnosticBuilder::getTemplate(
                PermissionCategory category) const
            {
                auto it = templates_.find(static_cast<int>(category));
                if (it == templates_.end())
                {
                    // (AR) قالب افتراضي لفئات غير معروفة
                    static PermissionErrorTemplate fallback = {
                        "E-PERM-999",
                        "خطأ أذونات غير معروف",
                        "Unknown permission error",
                        "البرنامج لا يملك الإذن المطلوب.",
                        "The program lacks the required permission.",
                        "راجع ملف ص.toml وأضف الإذن المناسب.",
                        "Check ص.toml and add the appropriate permission.",
                        "",
                        "https://sad-lang.dev/docs/permissions"};
                    return fallback;
                }
                return it->second;
            }

            // ============================================================================
            // PermissionDiagnosticBuilder::build()
            // ============================================================================

            std::string PermissionDiagnosticBuilder::build(
                PermissionCategory category,
                const std::string &target,
                const std::string &file,
                int line,
                bool lang_ar) const
            {
                const auto &tmpl = getTemplate(category);
                std::ostringstream ss;

                // ترويسة
                ss << "\n"
                   << "╔══════════════════════════════════════════════════════════════════╗\n"
                   << "║  ❌ خطأ أذونات [" << tmpl.code << "]"
                   << std::string(44 - tmpl.code.size(), ' ')
                   << "║\n"
                   << "╚══════════════════════════════════════════════════════════════════╝\n\n";

                // العنوان
                ss << "📛 " << (lang_ar ? tmpl.title_ar : tmpl.title_en) << "\n\n";

                // الموقع
                if (!file.empty())
                {
                    ss << "📍 الموقع: " << file;
                    if (line > 0)
                        ss << ":" << line;
                    ss << "\n\n";
                }

                // الهدف
                if (!target.empty())
                    ss << "🎯 الهدف: " << target << "\n\n";

                // الشرح
                ss << "📖 الشرح:\n   "
                   << (lang_ar ? tmpl.explain_ar : tmpl.explain_en) << "\n\n";

                // الحل
                ss << "💡 الحل:\n   "
                   << (lang_ar ? tmpl.solution_ar : tmpl.solution_en) << "\n\n";

                // مثال TOML
                if (!tmpl.toml_example.empty())
                {
                    // استبدال {target}
                    std::string example = tmpl.toml_example;
                    size_t pos;
                    while ((pos = example.find("{target}")) != std::string::npos)
                        example.replace(pos, 8, target);

                    ss << "📝 أضف إلى ص.toml:\n"
                       << "   ┌────────────────────────────────────────┐\n";

                    std::istringstream stream(example);
                    std::string l;
                    while (std::getline(stream, l))
                        ss << "   │ " << l << "\n";

                    ss << "   └────────────────────────────────────────┘\n\n";
                }

                // رابط المساعدة
                if (!tmpl.help_url.empty())
                    ss << "🔗 للمزيد: " << tmpl.help_url << "\n";

                ss << "\n";
                return ss.str();
            }

        } // namespace Permissions
    } // namespace Errors
} // namespace Sad
