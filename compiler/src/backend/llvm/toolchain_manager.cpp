/*
 * تنفيذ ToolchainManager
 * =======================
 */

#include "toolchain_detection.h"

#include <sstream>
#include <cstdlib>

namespace sad
{

    // ============================================================================
    // ToolchainManager Implementation
    // ============================================================================

    ToolchainManager &ToolchainManager::getInstance()
    {
        static ToolchainManager instance;
        return instance;
    }

    ToolchainManager::ToolchainManager()
        : initialized_(false)
    {
    }

    ToolchainManager::~ToolchainManager()
    {
    }

    bool ToolchainManager::initialize()
    {
        if (initialized_)
        {
            return true;
        }

        detector_ = std::make_unique<ToolchainDetector>();

        auto best = detector_->getBestToolchain();
        if (best)
        {
            active_toolchain_ = *best;
            initialized_ = true;
            return true;
        }

        return false;
    }

    const ToolchainInfo *ToolchainManager::getActiveToolchain() const
    {
        if (active_toolchain_)
        {
            return &(*active_toolchain_);
        }
        return nullptr;
    }

    bool ToolchainManager::setActiveToolchain(ToolchainType type)
    {
        if (!detector_)
        {
            return false;
        }

        auto tc = detector_->detectToolchain(type);
        if (tc && tc->isValid())
        {
            active_toolchain_ = *tc;
            return true;
        }

        return false;
    }

    std::string ToolchainManager::getLinkerPath() const
    {
        if (active_toolchain_ && active_toolchain_->linker.available)
        {
            return active_toolchain_->linker.path;
        }
        return "";
    }

    std::vector<std::string> ToolchainManager::getLinkerFlags() const
    {
        if (active_toolchain_)
        {
            return active_toolchain_->default_linker_flags;
        }
        return {};
    }

    std::vector<std::string> ToolchainManager::getDefaultLibraries() const
    {
        if (active_toolchain_)
        {
            std::vector<std::string> libs = active_toolchain_->default_libs;
            libs.insert(libs.end(), active_toolchain_->runtime_libs.begin(),
                        active_toolchain_->runtime_libs.end());
            return libs;
        }
        return {};
    }

    std::vector<std::string> ToolchainManager::getLibraryPaths() const
    {
        if (active_toolchain_)
        {
            return active_toolchain_->library_paths;
        }
        return {};
    }

    std::string ToolchainManager::buildLinkCommand(const std::vector<std::string> &object_files,
                                                   const std::string &output_file,
                                                   const std::vector<std::string> &additional_libs)
    {
        if (!active_toolchain_ || !active_toolchain_->linker.available)
        {
            return "";
        }

        std::ostringstream cmd;

        // المسار للرابط
        cmd << "\"" << active_toolchain_->linker.path << "\"";

        if (active_toolchain_->type == ToolchainType::MSVC)
        {
            // MSVC linker
            cmd << " /NOLOGO";
            cmd << " /OUT:\"" << output_file << "\"";

            // ملفات الكائنات
            for (const auto &obj : object_files)
            {
                cmd << " \"" << obj << "\"";
            }

            // مسارات المكتبات
            for (const auto &path : active_toolchain_->library_paths)
            {
                cmd << " /LIBPATH:\"" << path << "\"";
            }

            // المكتبات الافتراضية
            for (const auto &lib : active_toolchain_->default_libs)
            {
                cmd << " " << lib;
            }
            for (const auto &lib : active_toolchain_->runtime_libs)
            {
                cmd << " " << lib;
            }

            // المكتبات الإضافية
            for (const auto &lib : additional_libs)
            {
                cmd << " " << lib;
            }

            // الأعلام
            for (const auto &flag : active_toolchain_->default_linker_flags)
            {
                cmd << " " << flag;
            }
        }
        else
        {
            // Unix-like linker
            cmd << " -o \"" << output_file << "\"";

            // ملفات الكائنات
            for (const auto &obj : object_files)
            {
                cmd << " \"" << obj << "\"";
            }

            // مسارات المكتبات
            for (const auto &path : active_toolchain_->library_paths)
            {
                cmd << " -L\"" << path << "\"";
            }

            // المكتبات
            for (const auto &lib : active_toolchain_->default_libs)
            {
                cmd << " -l" << lib;
            }

            // المكتبات الإضافية
            for (const auto &lib : additional_libs)
            {
                cmd << " -l" << lib;
            }

            // الأعلام
            for (const auto &flag : active_toolchain_->default_linker_flags)
            {
                cmd << " " << flag;
            }
        }

        return cmd.str();
    }

    bool ToolchainManager::executeLink(const std::vector<std::string> &object_files,
                                       const std::string &output_file,
                                       const std::vector<std::string> &additional_libs)
    {
        // (AR) تحقق من سلامة المسارات قبل التنفيذ
        // (EN) Validate path safety before shell execution
        auto isPathSafe = [](const std::string &path) -> bool
        {
            for (char c : path)
            {
                if (c == ';' || c == '|' || c == '&' || c == '`' ||
                    c == '$' || c == '(' || c == ')' || c == '\n' || c == '\r')
                {
                    return false;
                }
            }
            return !path.empty();
        };

        if (!isPathSafe(output_file))
        {
            return false;
        }
        for (const auto &obj : object_files)
        {
            if (!isPathSafe(obj))
            {
                return false;
            }
        }

        std::string cmd = buildLinkCommand(object_files, output_file, additional_libs);
        if (cmd.empty())
        {
            return false;
        }

        int result = std::system(cmd.c_str());
        return result == 0;
    }

} // namespace sad
