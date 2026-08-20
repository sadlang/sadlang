// بسم الله الرحمن الرحيم
// UTF-8 Utilities for Arabic Filename Support
// أدوات UTF-8 لدعم أسماء الملفات العربية
//
// (AR) هذا الملف يوفر دوال مساعدة لدعم أسماء الملفات العربية
//      على نظام ويندوز. المشكلة أن std::ifstream(std::string) 
//      يستخدم ANSI codepage وليس UTF-8، فلا يفتح ملفات بأسماء عربية.
//
// (EN) This file provides helper functions for Arabic filename support
//      on Windows. The problem is std::ifstream(std::string) uses ANSI
//      codepage, not UTF-8, so it fails to open files with Arabic names.

#pragma once

#include <string>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <vector>
#include <optional>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>  // CommandLineToArgvW

// (AR) إزالة الماكروهات من windows.h التي تتعارض مع كود لغة ص
// (EN) Undefine Windows macros that conflict with S language code
#undef VOID
#undef ERROR
#undef FATAL
#undef NEAR
#undef FAR
#undef DELETE
#undef IN
#undef OUT
#undef OPTIONAL
#undef CYAN
#undef YELLOW
#undef RED
#undef GREEN
#undef BLUE
#undef MAGENTA
#undef WHITE
#endif

// 🔑 (AR) ترويسةُ نظامٍ تُضَمُّ **داخلَ** فضاءِ أسماءٍ تُصادَر إليه بأسرِها، ولا
//     يقف الأثرُ عند هذا الملفّ: حارسُ الضمِّ يجعل أوّلَ ضمٍّ هو الوحيد، فيصير
//     `<unistd.h>` المضمومُ داخلَ `sad::utf8` هو النسخةَ الوحيدةَ في وحدةِ
//     الترجمةِ كلِّها، ويُبتلَع ضمٌّ لاحقٌ في النطاقِ العامِّ صامتًا بحارسِه.
//     فيغيب `::getpid` و`::readlink` عن العالَمِ كلِّه — لا بعطبٍ في المُنادي
//     بل بحسبِ **ترتيبِ** الضمِّ عنده. ولذلك تُضَمُّ ترويساتُ المنصّةِ هنا
//     وحدَها، خارجَ كلِّ فضاءِ أسماء.
// 🔑 (EN) A system header included INSIDE a namespace is captured whole, and
//     the damage is not local: the include guard makes the first inclusion
//     the only one, so a later global #include is silently swallowed and
//     ::getpid / ::readlink vanish from global scope — not from a bug in the
//     includer, but from its include ORDER. Platform headers belong here,
//     outside every namespace.
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <limits.h>
#elif !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <unistd.h>
#include <limits.h>
#endif

namespace sad {
namespace utf8 {

// ============================================================================
// تحويلات الترميز / Encoding Conversions
// ============================================================================

#ifdef _WIN32

/**
 * @brief تحويل نص UTF-8 إلى wstring
 * Convert UTF-8 string to wide string (UTF-16)
 */
inline std::wstring to_wstring(const std::string& utf8_str) {
    if (utf8_str.empty()) return L"";
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, 
        utf8_str.c_str(), static_cast<int>(utf8_str.size()), 
        nullptr, 0);
    
    if (size_needed <= 0) return L"";
    
    std::wstring result(size_needed, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, 
        utf8_str.c_str(), static_cast<int>(utf8_str.size()),
        &result[0], size_needed);
    
    return result;
}

/**
 * @brief تحويل wstring إلى UTF-8
 * Convert wide string (UTF-16) to UTF-8 string
 */
inline std::string from_wstring(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0,
        wstr.c_str(), static_cast<int>(wstr.size()),
        nullptr, 0, nullptr, nullptr);
    
    if (size_needed <= 0) return "";
    
    std::string result(size_needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0,
        wstr.c_str(), static_cast<int>(wstr.size()),
        &result[0], size_needed, nullptr, nullptr);
    
    return result;
}

#endif // _WIN32

// ============================================================================
// إنشاء مسار من UTF-8 / Create path from UTF-8
// ============================================================================

/**
 * @brief إنشاء filesystem::path من نص UTF-8
 * Create a std::filesystem::path from a UTF-8 encoded string.
 * On Windows, this correctly handles Arabic/Unicode filenames.
 */
inline std::filesystem::path make_path(const std::string& utf8_path) {
#ifdef _WIN32
    // على ويندوز، نحوّل إلى wstring أولاً لأن path(string) يستخدم ANSI codepage
    return std::filesystem::path(to_wstring(utf8_path));
#else
    // على Linux/Mac، الترميز الافتراضي هو UTF-8
    return std::filesystem::path(utf8_path);
#endif
}

// ============================================================================
// فتح الملفات / File Opening
// ============================================================================

/**
 * @brief فتح ملف للقراءة مع دعم أسماء عربية
 * Open a file for reading with Arabic filename support.
 * Returns an ifstream that correctly handles UTF-8 paths on Windows.
 */
inline std::ifstream open_ifstream(const std::string& utf8_path) {
#ifdef _WIN32
    return std::ifstream(to_wstring(utf8_path));
#else
    return std::ifstream(utf8_path);
#endif
}

/**
 * @brief فتح ملف للكتابة مع دعم أسماء عربية
 * Open a file for writing with Arabic filename support.
 */
inline std::ofstream open_ofstream(const std::string& utf8_path) {
#ifdef _WIN32
    return std::ofstream(to_wstring(utf8_path));
#else
    return std::ofstream(utf8_path);
#endif
}

// ============================================================================
// عمليات الملفات / File Operations
// ============================================================================

/**
 * @brief التحقق من وجود ملف مع دعم أسماء عربية
 * Check if file exists with Arabic filename support.
 */
inline bool file_exists(const std::string& utf8_path) {
    return std::filesystem::exists(make_path(utf8_path));
}

/**
 * @brief قراءة ملف كامل كنص مع دعم أسماء عربية
 * Read entire file as string with Arabic filename support.
 */
inline std::optional<std::string> read_file(const std::string& utf8_path) {
    auto file = open_ifstream(utf8_path);
    if (!file || !file.is_open()) {
        return std::nullopt;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * @brief الحصول على امتداد الملف
 * Get file extension from UTF-8 path.
 */
inline std::string get_extension(const std::string& utf8_path) {
    auto p = make_path(utf8_path);
#ifdef _WIN32
    return from_wstring(p.extension().wstring());
#else
    return p.extension().string();
#endif
}

/**
 * @brief تغيير امتداد الملف
 * Change file extension.
 */
inline std::string change_extension(const std::string& utf8_path, const std::string& new_ext) {
    auto p = make_path(utf8_path);
    p.replace_extension(new_ext);
#ifdef _WIN32
    return from_wstring(p.wstring());
#else
    return p.string();
#endif
}

/**
 * @brief الحصول على اسم الملف بدون امتداد
 * Get filename stem (without extension).
 */
inline std::string get_stem(const std::string& utf8_path) {
    auto p = make_path(utf8_path);
#ifdef _WIN32
    return from_wstring(p.stem().wstring());
#else
    return p.stem().string();
#endif
}

/**
 * @brief الحصول على المجلد الأب
 * Get parent directory.
 */
inline std::string get_parent(const std::string& utf8_path) {
    auto p = make_path(utf8_path);
#ifdef _WIN32
    return from_wstring(p.parent_path().wstring());
#else
    return p.parent_path().string();
#endif
}

// ============================================================================
// تشغيل الأوامر الخارجيّة / External command execution
// ============================================================================

/**
 * @brief تشغيل أمرٍ خارجيٍّ بمسارٍ آمنِ الترميز
 *        Run an external command with encoding-safe paths.
 *
 * 🔑 (AR) `std::system` تأخذ `char*`، فيُعيد ويندوز تفسيرَ بايتاتِ UTF-8 بترميزِ
 *      النظامِ (ACP). فإن حوى الأمرُ مسارًا عربيًّا وكان الترميزُ لاتينيًّا أو
 *      عبريًّا، **نجح** الأمرُ ورمزُ عائدِه صفرٌ، وكُتِب المخرَجُ باسمٍ مشوَّه.
 *      لا خطأَ ولا تشخيصَ يشي بذلك — يغيب التنفيذيُّ عن موضعِه وحسب. لذلك
 *      يُمرَّرُ الأمرُ هنا UTF-16 فلا يمرُّ بترميزٍ ضيّقٍ أصلًا.
 * 🔑 (EN) std::system takes char*, so Windows reinterprets the UTF-8 bytes
 *      through the system ANSI codepage. With an Arabic path under a Latin or
 *      Hebrew ACP the command SUCCEEDS with exit code 0 and the output lands
 *      under a mangled name — no error, no diagnostic; the executable simply
 *      is not where it was asked for. So the command goes out as UTF-16 and
 *      never passes through a narrow encoding at all.
 */
inline int run_command(const std::string& utf8_command) {
#ifdef _WIN32
    return _wsystem(to_wstring(utf8_command).c_str());
#else
    return std::system(utf8_command.c_str());
#endif
}


/**
 * @brief فتحُ أنبوبِ قراءةٍ لأمرٍ خارجيٍّ بمسارٍ آمنِ الترميز
 *        Open a read pipe for an external command with encoding-safe paths.
 *
 * (AR) نظيرُ `run_command` للأوامرِ التي يُقرأ خرجُها. العلّةُ واحدةٌ: `_popen`
 *      ضيّقةٌ فتمرّ بايتاتُ UTF-8 عبرَ ترميزِ النظام.
 * (EN) The read-output twin of run_command; _popen is narrow the same way.
 *      Windows-only: its only caller is, and popen() is not portable.
 */
#ifdef _WIN32
inline FILE* open_pipe_read(const std::string& utf8_command) {
    return _wpopen(to_wstring(utf8_command).c_str(), L"r");
}
#endif
// ============================================================================
// سطر الأوامر / Command Line Arguments
// ============================================================================

#ifdef _WIN32

/**
 * @brief الحصول على معاملات سطر الأوامر بترميز UTF-8
 * Get command line arguments as UTF-8 encoded strings.
 * Uses GetCommandLineW + CommandLineToArgvW to properly handle Arabic filenames.
 */
inline std::vector<std::string> get_utf8_args() {
    int wargc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    
    if (!wargv) return {};
    
    std::vector<std::string> args;
    args.reserve(wargc);
    
    for (int i = 0; i < wargc; i++) {
        args.push_back(from_wstring(wargv[i]));
    }
    
    LocalFree(wargv);
    return args;
}

/**
 * @brief الحصول على مسار البرنامج التنفيذي (Unicode)
 * Get executable directory path with Unicode support.
 */
inline std::filesystem::path get_executable_dir() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::filesystem::path exe_path(buffer);
    return exe_path.parent_path();
}

#elif defined(__EMSCRIPTEN__)

/**
 * @brief WASM stub: no command line args
 */
inline std::vector<std::string> get_utf8_args() {
    return {};
}

/**
 * @brief WASM stub: return current path
 */
inline std::filesystem::path get_executable_dir() {
    return std::filesystem::current_path();
}

#elif defined(__APPLE__)

/**
 * @brief الحصول على مسار البرنامج التنفيذي على macOS
 * Get executable directory path (macOS).
 */
inline std::filesystem::path get_executable_dir() {
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        char resolved[PATH_MAX];
        if (realpath(buffer, resolved) != nullptr) {
            return std::filesystem::path(resolved).parent_path();
        }
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
}

#else

/**
 * @brief الحصول على مسار البرنامج التنفيذي على Linux
 * Get executable directory path (Linux).
 */
inline std::filesystem::path get_executable_dir() {
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
}

#endif

} // namespace utf8
} // namespace sad
