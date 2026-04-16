/*
 * ============================================================================
 * تنفيذ نظام الملفات الافتراضي (VFS) - لغة ص
 * VFS Implementation - Sad Language
 * ============================================================================
 */

#include "vfs.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

VFSManager& VFSManager::getInstance() {
    static VFSManager instance;
    return instance;
}

VFSManager::VFSManager() : nextFd_(3), isInitialized_(true) {
    // fd 0, 1, 2 محجوزة (stdin, stdout, stderr) / Reserved
}

// ============================================================================
// تسجيل أنظمة الملفات / Filesystem Registration
// ============================================================================

bool VFSManager::registerFilesystem(const std::string& name, const FilesystemOps& ops) {
    // التحقق من عدم وجوده مسبقاً / Check if already registered
    if (findFilesystem(name)) return false;

    RegisteredFilesystem fs;
    fs.name = name;
    fs.ops = ops;
    fs.isDefault = filesystems_.empty();
    filesystems_.push_back(fs);
    return true;
}

bool VFSManager::unregisterFilesystem(const std::string& name) {
    auto it = std::remove_if(filesystems_.begin(), filesystems_.end(),
        [&](const RegisteredFilesystem& fs) { return fs.name == name; });
    if (it != filesystems_.end()) {
        filesystems_.erase(it, filesystems_.end());
        return true;
    }
    return false;
}

// ============================================================================
// التحميل والفصل / Mount/Unmount
// ============================================================================

bool VFSManager::mount(const std::string& devicePath, const std::string& mountPath,
                        const std::string& fsType, bool readOnly) {
    RegisteredFilesystem* fs = findFilesystem(fsType);
    if (!fs) return false;

    // التحقق من عدم وجود تحميل على نفس المسار / Check no existing mount
    for (const auto& mp : mountPoints_) {
        if (mp.path == mountPath) return false;
    }

    MountPoint mp;
    mp.path = mountPath;
    mp.fsName = fsType;
    mp.deviceId = 0; // في نظام حقيقي: من الجهاز / In real system: from device
    mp.privateData = nullptr;
    mp.readOnly = readOnly;

    if (fs->ops.mount) {
        if (!fs->ops.mount(mp.deviceId, mp.privateData)) {
            return false;
        }
    }

    mountPoints_.push_back(mp);
    return true;
}

bool VFSManager::unmount(const std::string& mountPath) {
    auto it = std::find_if(mountPoints_.begin(), mountPoints_.end(),
        [&](const MountPoint& mp) { return mp.path == mountPath; });

    if (it == mountPoints_.end()) return false;

    // إغلاق جميع الملفات المفتوحة على هذا التحميل / Close all open files
    for (auto& of : openFiles_) {
        if (of.isOpen && of.mountPath == mountPath) {
            close(of.fd);
        }
    }

    RegisteredFilesystem* fs = findFilesystem(it->fsName);
    if (fs && fs->ops.unmount) {
        fs->ops.unmount(it->privateData);
    }

    mountPoints_.erase(it);
    return true;
}

// ============================================================================
// عمليات الملفات / File Operations
// ============================================================================

int64_t VFSManager::open(const std::string& path, uint16_t mode) {
    MountPoint* mp = findMountPoint(path);
    if (!mp) return VFSConstants::INVALID_FD;

    // التحقق من القراءة فقط / Check read-only
    if (mp->readOnly && (mode & static_cast<uint16_t>(OpenMode::WRITE))) {
        return VFSConstants::INVALID_FD;
    }

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.open) return VFSConstants::INVALID_FD;

    std::string relPath = relativePath(path, mp->path);
    int64_t fsFd = fs->ops.open(relPath.c_str(), mode, mp->privateData);
    if (fsFd < 0) return VFSConstants::INVALID_FD;

    OpenFile of;
    of.fd = nextFd_++;
    of.path = path;
    of.mountPath = mp->path;
    of.mode = mode;
    of.position = 0;
    of.size = 0;
    of.isOpen = true;

    // الحصول على الحجم / Get size
    FileStat st;
    if (fs->ops.stat && fs->ops.stat(relPath.c_str(), &st, mp->privateData)) {
        of.size = st.size;
    }

    openFiles_.push_back(of);
    return of.fd;
}

bool VFSManager::close(int64_t fd) {
    OpenFile* of = findOpenFile(fd);
    if (!of || !of->isOpen) return false;

    MountPoint* mp = findMountPoint(of->path);
    if (mp) {
        RegisteredFilesystem* fs = findFilesystem(mp->fsName);
        if (fs && fs->ops.close) {
            fs->ops.close(fd, mp->privateData);
        }
    }

    of->isOpen = false;
    return true;
}

int64_t VFSManager::read(int64_t fd, void* buffer, uint64_t size) {
    OpenFile* of = findOpenFile(fd);
    if (!of || !of->isOpen) return -1;

    if (!(of->mode & static_cast<uint16_t>(OpenMode::READ))) return -1;

    MountPoint* mp = findMountPoint(of->path);
    if (!mp) return -1;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.read) return -1;

    int64_t bytesRead = fs->ops.read(fd, buffer, size, mp->privateData);
    if (bytesRead > 0) {
        of->position += static_cast<uint64_t>(bytesRead);
    }
    return bytesRead;
}

int64_t VFSManager::write(int64_t fd, const void* buffer, uint64_t size) {
    OpenFile* of = findOpenFile(fd);
    if (!of || !of->isOpen) return -1;

    if (!(of->mode & static_cast<uint16_t>(OpenMode::WRITE))) return -1;

    MountPoint* mp = findMountPoint(of->path);
    if (!mp || mp->readOnly) return -1;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.write) return -1;

    int64_t bytesWritten = fs->ops.write(fd, buffer, size, mp->privateData);
    if (bytesWritten > 0) {
        of->position += static_cast<uint64_t>(bytesWritten);
        if (of->position > of->size) {
            of->size = of->position;
        }
    }
    return bytesWritten;
}

int64_t VFSManager::seek(int64_t fd, int64_t offset, SeekOrigin origin) {
    OpenFile* of = findOpenFile(fd);
    if (!of || !of->isOpen) return -1;

    int64_t newPos = 0;
    switch (origin) {
        case SeekOrigin::BEGIN:
            newPos = offset;
            break;
        case SeekOrigin::CURRENT:
            newPos = static_cast<int64_t>(of->position) + offset;
            break;
        case SeekOrigin::END:
            newPos = static_cast<int64_t>(of->size) + offset;
            break;
    }

    if (newPos < 0) return -1;
    of->position = static_cast<uint64_t>(newPos);
    return newPos;
}

bool VFSManager::stat(const std::string& path, FileStat* statBuf) {
    if (!statBuf) return false;

    MountPoint* mp = findMountPoint(path);
    if (!mp) return false;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.stat) return false;

    std::string relPath = relativePath(path, mp->path);
    return fs->ops.stat(relPath.c_str(), statBuf, mp->privateData);
}

// ============================================================================
// عمليات المجلدات / Directory Operations
// ============================================================================

bool VFSManager::mkdir(const std::string& path, uint16_t mode) {
    MountPoint* mp = findMountPoint(path);
    if (!mp || mp->readOnly) return false;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.mkdir) return false;

    std::string relPath = relativePath(path, mp->path);
    return fs->ops.mkdir(relPath.c_str(), mode, mp->privateData);
}

bool VFSManager::rmdir(const std::string& path) {
    MountPoint* mp = findMountPoint(path);
    if (!mp || mp->readOnly) return false;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.rmdir) return false;

    std::string relPath = relativePath(path, mp->path);
    return fs->ops.rmdir(relPath.c_str(), mp->privateData);
}

std::vector<DirectoryEntry> VFSManager::readdir(const std::string& path) {
    MountPoint* mp = findMountPoint(path);
    if (!mp) return {};

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.readdir) return {};

    std::string relPath = relativePath(path, mp->path);
    return fs->ops.readdir(relPath.c_str(), mp->privateData);
}

// ============================================================================
// عمليات أخرى / Other Operations
// ============================================================================

bool VFSManager::unlink(const std::string& path) {
    MountPoint* mp = findMountPoint(path);
    if (!mp || mp->readOnly) return false;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.unlink) return false;

    std::string relPath = relativePath(path, mp->path);
    return fs->ops.unlink(relPath.c_str(), mp->privateData);
}

bool VFSManager::rename(const std::string& oldPath, const std::string& newPath) {
    MountPoint* mp = findMountPoint(oldPath);
    if (!mp || mp->readOnly) return false;

    // التأكد أن كلا المسارين على نفس التحميل / Ensure same mount point
    MountPoint* mp2 = findMountPoint(newPath);
    if (mp != mp2) return false;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.rename) return false;

    std::string relOld = relativePath(oldPath, mp->path);
    std::string relNew = relativePath(newPath, mp->path);
    return fs->ops.rename(relOld.c_str(), relNew.c_str(), mp->privateData);
}

bool VFSManager::truncate(const std::string& path, uint64_t length) {
    MountPoint* mp = findMountPoint(path);
    if (!mp || mp->readOnly) return false;

    RegisteredFilesystem* fs = findFilesystem(mp->fsName);
    if (!fs || !fs->ops.truncate) return false;

    std::string relPath = relativePath(path, mp->path);
    return fs->ops.truncate(relPath.c_str(), length, mp->privateData);
}

bool VFSManager::exists(const std::string& path) {
    FileStat st;
    return stat(path, &st);
}

// ============================================================================
// مساعدات / Helpers
// ============================================================================

MountPoint* VFSManager::findMountPoint(const std::string& path) {
    MountPoint* best = nullptr;
    size_t bestLen = 0;

    for (auto& mp : mountPoints_) {
        if (path.compare(0, mp.path.size(), mp.path) == 0) {
            if (mp.path.size() > bestLen) {
                best = &mp;
                bestLen = mp.path.size();
            }
        }
    }
    return best;
}

RegisteredFilesystem* VFSManager::findFilesystem(const std::string& name) {
    for (auto& fs : filesystems_) {
        if (fs.name == name) return &fs;
    }
    return nullptr;
}

std::string VFSManager::relativePath(const std::string& path,
                                      const std::string& mountPath) const {
    if (path.size() <= mountPath.size()) return "/";
    std::string rel = path.substr(mountPath.size());
    if (rel.empty() || rel[0] != '/') rel = "/" + rel;
    return rel;
}

OpenFile* VFSManager::findOpenFile(int64_t fd) {
    for (auto& of : openFiles_) {
        if (of.fd == fd) return &of;
    }
    return nullptr;
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string VFSManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير VFS / VFS Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "أنظمة ملفات مسجلة / Registered FS: " << filesystems_.size() << "\n";
    for (const auto& fs : filesystems_) {
        report << "  - " << fs.name << (fs.isDefault ? " (افتراضي / Default)" : "") << "\n";
    }

    report << "\nنقاط التحميل / Mount Points: " << mountPoints_.size() << "\n";
    for (const auto& mp : mountPoints_) {
        report << "  " << mp.path << " [" << mp.fsName << "]"
               << (mp.readOnly ? " (للقراءة فقط / RO)" : " (قراءة/كتابة / RW)") << "\n";
    }

    uint32_t openCount = 0;
    for (const auto& of : openFiles_) {
        if (of.isOpen) openCount++;
    }
    report << "\nملفات مفتوحة / Open Files: " << openCount << "\n";
    for (const auto& of : openFiles_) {
        if (!of.isOpen) continue;
        report << "  fd=" << of.fd << " " << of.path
               << " (موضع / Pos: " << of.position << "/" << of.size << ")\n";
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
