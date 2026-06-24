// ============================================================================
// (AR) ملف: pdf_exporter.h — مُحوّل HTML→PDF عبر محرك متصفح بدون واجهة
// (EN) File: pdf_exporter.h — HTML→PDF converter via headless browser
// ============================================================================
//
// (AR) الغرض:
//      البحث عن محرّك متاح (msedge / chrome / wkhtmltopdf) واستدعاؤه
//      لتحويل ملف HTML إلى PDF.
//
// (EN) Purpose:
//      Locate an available engine (msedge / chrome / wkhtmltopdf) and invoke
//      it to convert an HTML file to PDF.
//
// (AR) الترتيب: msedge ← chrome ← wkhtmltopdf
//      كلها تعطي نفس النتيجة عملياً للمحتوى البسيط.
// ============================================================================

#pragma once

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace Sad
{
    namespace AST
    {
        class PdfExporter
        {
        public:
            // ================================================================
            // (AR) يبحث عن أول محرك PDF متاح في النظام
            // (EN) Find the first available PDF engine on the system
            // ================================================================
            struct Engine
            {
                std::string path; // (AR) المسار الكامل للمنفّذ
                std::string kind; // (AR) "edge" | "chrome" | "wkhtmltopdf"
                bool found = false;
            };

            static Engine findEngine()
            {
                Engine e;

                // (AR) نقاط البحث الشائعة على Windows
#ifdef _WIN32
                std::vector<std::pair<std::string, std::string>> candidates = {
                    {"edge", R"(C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe)"},
                    {"edge", R"(C:\Program Files\Microsoft\Edge\Application\msedge.exe)"},
                    {"chrome", R"(C:\Program Files\Google\Chrome\Application\chrome.exe)"},
                    {"chrome", R"(C:\Program Files (x86)\Google\Chrome\Application\chrome.exe)"},
                    {"wkhtmltopdf", R"(C:\Program Files\wkhtmltopdf\bin\wkhtmltopdf.exe)"},
                };
#elif defined(__APPLE__)
                // (AR) على macOS التطبيقات حزم .app — المسار التنفيذي الفعلي تحت
                //      Contents/MacOS/، وليس /usr/bin/ كما في لينكس.
                // (EN) On macOS apps are .app bundles — the real executable lives
                //      under Contents/MacOS/, unlike Linux's /usr/bin/.
                std::vector<std::pair<std::string, std::string>> candidates = {
                    {"chrome", "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"},
                    {"edge", "/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge"},
                    {"chrome", "/Applications/Chromium.app/Contents/MacOS/Chromium"},
                    {"wkhtmltopdf", "/opt/homebrew/bin/wkhtmltopdf"},
                    {"wkhtmltopdf", "/usr/local/bin/wkhtmltopdf"},
                };
#else
                std::vector<std::pair<std::string, std::string>> candidates = {
                    {"chrome", "/usr/bin/google-chrome"},
                    {"chrome", "/usr/bin/chromium"},
                    {"chrome", "/usr/bin/chromium-browser"},
                    {"wkhtmltopdf", "/usr/bin/wkhtmltopdf"},
                    {"wkhtmltopdf", "/usr/local/bin/wkhtmltopdf"},
                };
#endif

                for (const auto &c : candidates)
                {
                    std::error_code ec;
                    if (std::filesystem::exists(c.second, ec))
                    {
                        e.path = c.second;
                        e.kind = c.first;
                        e.found = true;
                        return e;
                    }
                }
                return e;
            }

            // ================================================================
            // (AR) يحوّل ملف HTML إلى PDF.
            //      htmlPath: مسار ملف HTML (يجب أن يكون موجوداً)
            //      pdfPath: مسار الناتج PDF
            //      يرجع: true عند النجاح
            // (EN) Convert an HTML file to PDF.
            // ================================================================
            static bool convert(const std::string &htmlPath,
                                const std::string &pdfPath,
                                std::string &errorMsg)
            {
                Engine eng = findEngine();
                if (!eng.found)
                {
                    errorMsg = "(AR) لم يُعثر على محرك PDF متاح. ثبّت Edge/Chrome أو wkhtmltopdf.\n"
                               "(EN) No PDF engine found. Install Edge/Chrome or wkhtmltopdf.";
                    return false;
                }

                // (AR) بناء مسار URI للملف المحلي (file:///)
                std::filesystem::path absHtml = std::filesystem::absolute(htmlPath);
                std::string uri = "file:///" + absHtml.generic_string();
                std::filesystem::path absPdf = std::filesystem::absolute(pdfPath);

                std::string cmd;
                if (eng.kind == "edge" || eng.kind == "chrome")
                {
                    // (AR) قوالب الترويسة والتذييل: ترويسة فارغة + تذييل برقم الصفحة
                    // (EN) Header/footer templates: empty header + footer with page number
                    // (AR) ملاحظة: نستخدم اقتباس مزدوج محايد + escape للـ HTML
                    //      "X / Y" تعمل عبر class="pageNumber" / class="totalPages"
                    const char *headerTpl = "<div></div>";
                    const char *footerTpl =
                        "<div style='font-size:9px;width:100%;text-align:center;"
                        "color:#888;font-family:Segoe UI,sans-serif;'>"
                        "<span class='pageNumber'></span> / "
                        "<span class='totalPages'></span>"
                        "</div>";

                    cmd = "\"" + eng.path + "\"" + " --headless --disable-gpu" + " --virtual-time-budget=10000" + " --run-all-compositor-stages-before-draw" + " --print-to-pdf-header-template=\"" + headerTpl + "\"" + " --print-to-pdf-footer-template=\"" + footerTpl + "\"" + " --print-to-pdf=\"" + absPdf.string() + "\"" + " \"" + uri + "\"";
                }
                else if (eng.kind == "wkhtmltopdf")
                {
                    // (AR) wkhtmltopdf يدعم --footer-center مباشرة
                    cmd = "\"" + eng.path + "\" --quiet" + " --footer-center \"[page] / [topage]\"" + " --footer-font-size 9" + " \"" + absHtml.string() + "\" \"" + absPdf.string() + "\"";
                }

                // (AR) على ويندوز يستعمل system() صدفة cmd.exe التي تتطلّب تغليف
                //      الأمر كلّه باقتباس خارجيّ إضافيّ حين يحوي مسار البرنامج اقتباسًا
                //      (cmd /c ""prog" args"). على POSIX يستعمل /bin/sh الذي يكسر هذا
                //      التغليف فتصير < > في قالب HTML إعادة توجيه (redirection unexpected).
                // (EN) On Windows, system() uses cmd.exe needing an extra outer quote
                //      wrap (cmd /c ""prog" args"). On POSIX, /bin/sh breaks on that wrap
                //      turning < > in the HTML template into redirections — so wrap only
                //      on Windows.
#ifdef _WIN32
                cmd = "\"" + cmd + "\"";
#endif

                int rc = std::system(cmd.c_str());
                if (rc != 0)
                {
                    errorMsg = "(AR) فشل تنفيذ محرك PDF (" + eng.kind + ") — رمز الخروج: " + std::to_string(rc);
                    return false;
                }

                std::error_code ec;
                if (!std::filesystem::exists(absPdf, ec))
                {
                    errorMsg = "(AR) لم يُنشأ ملف PDF رغم نجاح الأمر.";
                    return false;
                }

                // (AR) Chrome/Edge headless تُعيد رمز الخروج فور انتهاء العملية
                //      الأساسية، لكن قد تُكمل بعض إصداراتها تدفّق الكتابة
                //      النهائية لملف PDF من عملية/خيط داخلي تابع للحظات قصيرة
                //      بعد ذلك — خاصة تحت ضغط I/O/CPU (اختبارات متوازية).
                //      فحص استقرار الحجم (يتطابق مرتين متتاليتين بفاصل قصير)
                //      يضمن اكتمال الكتابة فعلياً قبل أن يستخدمه المستدعي.
                // (EN) Chrome/Edge headless return as soon as the main process
                //      exits, but some versions finish flushing the PDF's
                //      final write from a short-lived child/thread a moment
                //      later — especially under I/O/CPU pressure (parallel
                //      tests). Polling for a stable size (matches twice in a
                //      row) ensures the write is truly complete before the
                //      caller reads it.
                auto stableSize = std::filesystem::file_size(absPdf, ec);
                for (int attempt = 0; !ec && attempt < 20; ++attempt)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    auto nextSize = std::filesystem::file_size(absPdf, ec);
                    if (ec)
                        break;
                    if (nextSize == stableSize && nextSize > 0)
                        break;
                    stableSize = nextSize;
                }
                return true;
            }
        };
    } // namespace AST
} // namespace Sad
