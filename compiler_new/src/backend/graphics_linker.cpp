/**
 * =============================================================================
 * ملف: graphics_linker.cpp
 * الوصف: ربط برامج الرسومات مع المكتبات المطلوبة
 * =============================================================================
 * 
 * 🔗 نظام ربط الرسومات
 * ═════════════════════════════════════════
 * 
 * يربط الملفات التنفيذية بمكتبات SDL2 والمكتبات الأخرى المطلوبة
 * على جميع أنظمة التشغيل.
 * 
 * المكتبات المطلوبة:
 * - SDL2          : النافذة والأحداث والرسم
 * - SDL2_ttf      : رسم النصوص (اختياري)
 * - SDL2_image    : تحميل الصور (اختياري)
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace sad::linker {

// =============================================================================
// تحديد المنصة
// Platform Detection
// =============================================================================

enum class Platform {
    Windows,
    Linux,
    macOS,
    Unknown
};

Platform detectPlatform() {
#if defined(_WIN32) || defined(_WIN64)
    return Platform::Windows;
#elif defined(__linux__)
    return Platform::Linux;
#elif defined(__APPLE__)
    return Platform::macOS;
#else
    return Platform::Unknown;
#endif
}

// =============================================================================
// GraphicsLinker — رابط الرسومات
// =============================================================================

/**
 * @brief رابط برامج الرسومات
 */
class GraphicsLinker {
public:
    GraphicsLinker() : platform_(detectPlatform()) {}
    
    /**
     * @brief الحصول على مكتبات SDL2 المطلوبة
     */
    std::vector<std::string> getSDL2Libraries() const {
        std::vector<std::string> libs;
        
        switch (platform_) {
            case Platform::Windows:
                libs.push_back("SDL2");
                libs.push_back("SDL2main");
                libs.push_back("SDL2_ttf");
                libs.push_back("SDL2_image");
                break;
                
            case Platform::Linux:
                libs.push_back("SDL2");
                libs.push_back("SDL2_ttf");
                libs.push_back("SDL2_image");
                libs.push_back("m");  // math library
                libs.push_back("dl"); // dynamic linking
                libs.push_back("pthread");
                break;
                
            case Platform::macOS:
                // على macOS نستخدم frameworks
                libs.push_back("-framework SDL2");
                libs.push_back("-framework SDL2_ttf");
                libs.push_back("-framework SDL2_image");
                libs.push_back("-framework Cocoa");
                break;
                
            default:
                break;
        }
        
        return libs;
    }
    
    /**
     * @brief الحصول على مسارات المكتبات
     */
    std::vector<std::string> getLibraryPaths() const {
        std::vector<std::string> paths;
        
        switch (platform_) {
            case Platform::Windows:
                // استخدام المسارات المعتادة على Windows
                paths.push_back("C:/SDL2/lib/x64");
                paths.push_back("C:/vcpkg/installed/x64-windows/lib");
                paths.push_back("C:/msys64/mingw64/lib");
                // مسارات المشروع
                paths.push_back("stdlib/graphics/third_party/SDL2/lib");
                break;
                
            case Platform::Linux:
                paths.push_back("/usr/lib");
                paths.push_back("/usr/local/lib");
                paths.push_back("/usr/lib/x86_64-linux-gnu");
                break;
                
            case Platform::macOS:
                paths.push_back("/usr/local/lib");
                paths.push_back("/opt/homebrew/lib");
                paths.push_back("/Library/Frameworks");
                break;
                
            default:
                break;
        }
        
        return paths;
    }
    
    /**
     * @brief الحصول على مسارات التضمين
     */
    std::vector<std::string> getIncludePaths() const {
        std::vector<std::string> paths;
        
        switch (platform_) {
            case Platform::Windows:
                paths.push_back("C:/SDL2/include");
                paths.push_back("C:/vcpkg/installed/x64-windows/include");
                paths.push_back("stdlib/graphics/third_party/SDL2/include");
                break;
                
            case Platform::Linux:
                paths.push_back("/usr/include/SDL2");
                paths.push_back("/usr/local/include/SDL2");
                break;
                
            case Platform::macOS:
                paths.push_back("/usr/local/include/SDL2");
                paths.push_back("/opt/homebrew/include/SDL2");
                break;
                
            default:
                break;
        }
        
        return paths;
    }
    
    /**
     * @brief توليد أمر الربط
     */
    std::string generateLinkCommand(
        const std::string& objectFile,
        const std::string& outputFile
    ) const {
        std::string cmd;
        
        switch (platform_) {
            case Platform::Windows:
                cmd = "link.exe /NOLOGO /OUT:" + outputFile + ".exe ";
                cmd += objectFile + " ";
                for (const auto& lib : getSDL2Libraries()) {
                    cmd += lib + ".lib ";
                }
                for (const auto& path : getLibraryPaths()) {
                    cmd += "/LIBPATH:" + path + " ";
                }
                cmd += "/SUBSYSTEM:WINDOWS";
                break;
                
            case Platform::Linux:
                cmd = "gcc -o " + outputFile + " ";
                cmd += objectFile + " ";
                for (const auto& path : getLibraryPaths()) {
                    cmd += "-L" + path + " ";
                }
                for (const auto& lib : getSDL2Libraries()) {
                    cmd += "-l" + lib + " ";
                }
                break;
                
            case Platform::macOS:
                cmd = "clang -o " + outputFile + " ";
                cmd += objectFile + " ";
                for (const auto& lib : getSDL2Libraries()) {
                    cmd += lib + " ";
                }
                break;
                
            default:
                cmd = "echo 'Unsupported platform'";
                break;
        }
        
        return cmd;
    }
    
    /**
     * @brief ربط ملف object إلى ملف تنفيذي
     */
    bool link(const std::string& objectFile,
              const std::string& outputFile) const {
        std::string cmd = generateLinkCommand(objectFile, outputFile);
        
        std::cout << "🔗 ربط: " << cmd << std::endl;
        
        int result = std::system(cmd.c_str());
        return result == 0;
    }
    
    /**
     * @brief نسخ مكتبات SDL2 DLL إلى مجلد الإخراج (Windows فقط)
     */
    bool copySDL2DLLs(const std::string& outputDir) const {
        if (platform_ != Platform::Windows) {
            return true; // لا حاجة على أنظمة أخرى
        }
        
        namespace fs = std::filesystem;
        
        std::vector<std::string> dlls = {
            "SDL2.dll",
            "SDL2_ttf.dll",
            "SDL2_image.dll"
        };
        
        std::vector<std::string> searchPaths = {
            "C:/SDL2/lib/x64",
            "C:/vcpkg/installed/x64-windows/bin",
            "stdlib/graphics/third_party/SDL2/bin"
        };
        
        for (const auto& dll : dlls) {
            bool found = false;
            for (const auto& path : searchPaths) {
                fs::path src = fs::path(path) / dll;
                if (fs::exists(src)) {
                    fs::path dst = fs::path(outputDir) / dll;
                    try {
                        fs::copy_file(src, dst, 
                            fs::copy_options::overwrite_existing);
                        found = true;
                        break;
                    } catch (const fs::filesystem_error& e) {
                        std::cerr << "⚠️ خطأ في نسخ " << dll << ": " 
                                  << e.what() << std::endl;
                    }
                }
            }
            if (!found) {
                std::cerr << "⚠️ لم يتم العثور على " << dll << std::endl;
            }
        }
        
        return true;
    }

private:
    Platform platform_;
};

// =============================================================================
// دوال مساعدة عامة
// Global Helper Functions
// =============================================================================

/**
 * @brief التحقق من وجود SDL2
 */
bool isSDL2Installed() {
    GraphicsLinker linker;
    
    // فحص بعض المسارات المعتادة
    for (const auto& path : linker.getLibraryPaths()) {
        std::filesystem::path sdlLib;
        
        auto platform = detectPlatform();
        if (platform == Platform::Windows) {
            sdlLib = std::filesystem::path(path) / "SDL2.lib";
        } else if (platform == Platform::Linux) {
            sdlLib = std::filesystem::path(path) / "libSDL2.so";
        } else if (platform == Platform::macOS) {
            sdlLib = std::filesystem::path(path) / "libSDL2.dylib";
        }
        
        if (std::filesystem::exists(sdlLib)) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief الحصول على إصدار SDL2
 */
std::string getSDL2Version() {
    // يمكن استدعاء SDL_GetVersion في وقت التشغيل
    // هنا نعيد قيمة ثابتة
    return "2.28.0";
}

} // namespace sad::linker
