/**
 * @file builtin_kernel_uefi.cpp
 * @brief (AR) وحدة UEFI — واجهة البرنامج الثابت الموحدة الممتدة
 * @brief (EN) UEFI module — Unified Extensible Firmware Interface
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "uefi.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelUEFI(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. التهيئة والتحكم / Initialization & Control
    // ═══════════════════════════════════════════════════════════════
    // uefi_تهيئة / uefi_initialize — تهيئة بيئة UEFI
    auto uefi_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        uint64_t handle = 0, sysTable = 0;
        if (args.size() >= 1) handle = static_cast<uint64_t>(args[0]->toInt());
        if (args.size() >= 2) sysTable = static_cast<uint64_t>(args[1]->toInt());
        auto status = mgr.initialize(handle, sysTable);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_إنهاء_خدمات_إقلاع / uefi_exit_boot_services
    auto uefi_exit_bs = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        uint64_t mapKey = 0;
        if (args.size() >= 1) mapKey = static_cast<uint64_t>(args[0]->toInt());
        auto status = mgr.exitBootServices(mapKey);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_هل_مهيأ / uefi_is_initialized
    auto uefi_is_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.isInitialized() ? 1 : 0);
    };

    // uefi_خدمات_إقلاع_منتهية / uefi_boot_services_exited
    auto uefi_bs_exited = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.bootServicesExited() ? 1 : 0);
    };

    // uefi_إعادة_تشغيل / uefi_reset_system
    auto uefi_reset = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        int resetType = 0; // بارد / cold
        if (args.size() >= 1) resetType = args[0]->toInt();
        mgr.resetSystem(resetType);
        return std::make_shared<Data::Value>(0);
    };

    // ═══════════════════════════════════════════════════════════════
    // 2. إدارة الذاكرة / Memory Services
    // ═══════════════════════════════════════════════════════════════
    // uefi_تخصيص_صفحات / uefi_allocate_pages
    auto uefi_alloc_pages = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 3) return std::make_shared<Data::Value>(-1);
        int allocType = args[0]->toInt();
        int memType = args[1]->toInt();
        uint64_t pages = static_cast<uint64_t>(args[2]->toInt());
        uint64_t address = 0;
        auto status = mgr.allocatePages(
            static_cast<LowLevel::EfiAllocateType>(allocType),
            static_cast<LowLevel::EfiMemoryType>(memType),
            pages, address);
        if (status == LowLevel::EfiStatus::SUCCESS) {
            return std::make_shared<Data::Value>(static_cast<double>(address));
        }
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_تحرير_صفحات / uefi_free_pages
    auto uefi_free_pages = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        uint64_t address = static_cast<uint64_t>(args[0]->toDouble());
        uint64_t pages = static_cast<uint64_t>(args[1]->toInt());
        auto status = mgr.freePages(address, pages);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_تخصيص_كتلة / uefi_allocate_pool
    auto uefi_alloc_pool = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        int memType = args[0]->toInt();
        uint64_t size = static_cast<uint64_t>(args[1]->toInt());
        uint64_t buffer = 0;
        auto status = mgr.allocatePool(
            static_cast<LowLevel::EfiMemoryType>(memType),
            size, buffer);
        if (status == LowLevel::EfiStatus::SUCCESS) {
            return std::make_shared<Data::Value>(static_cast<double>(buffer));
        }
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_تحرير_كتلة / uefi_free_pool
    auto uefi_free_pool = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>(-1);
        uint64_t buffer = static_cast<uint64_t>(args[0]->toDouble());
        auto status = mgr.freePool(buffer);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_خريطة_ذاكرة / uefi_get_memory_map
    auto uefi_memmap = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        uint64_t mapKey = 0;
        auto map = mgr.getMemoryMap(mapKey);

        std::ostringstream oss;
        oss << "مفتاح_الخريطة=" << mapKey << "\n";
        oss << "عدد_المناطق=" << map.size() << "\n";
        for (size_t i = 0; i < map.size(); i++) {
            oss << "---\n";
            oss << "نوع=" << static_cast<int>(map[i].type)
                << " (" << LowLevel::UefiManager::getMemoryTypeName(static_cast<LowLevel::EfiMemoryType>(map[i].type)) << ")\n";
            oss << "بداية=0x" << std::hex << map[i].physicalStart << std::dec << "\n";
            oss << "صفحات=" << map[i].numberOfPages << "\n";
            oss << "سمات=0x" << std::hex << map[i].attribute << std::dec << "\n";
        }
        return std::make_shared<Data::Value>(oss.str());
    };

    // uefi_مفتاح_خريطة / uefi_get_memory_map_key — إرجاع المفتاح فقط
    auto uefi_memmap_key = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        uint64_t mapKey = 0;
        mgr.getMemoryMap(mapKey);
        return std::make_shared<Data::Value>(static_cast<double>(mapKey));
    };

    // uefi_ذاكرة_متاحة / uefi_total_memory
    auto uefi_total_mem = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        uint64_t total = mgr.getTotalAvailableMemory();
        return std::make_shared<Data::Value>(static_cast<double>(total));
    };

    // ═══════════════════════════════════════════════════════════════
    // 3. بروتوكول الرسوميات GOP / Graphics Output Protocol
    // ═══════════════════════════════════════════════════════════════
    // uefi_تهيئة_رسوميات / uefi_init_gop
    auto uefi_init_gop = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        auto status = mgr.initializeGOP();
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_تعيين_وضع_رسوميات / uefi_set_gop_mode
    auto uefi_set_gop = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>(-1);
        uint32_t mode = static_cast<uint32_t>(args[0]->toInt());
        auto status = mgr.setGOPMode(mode);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_استعلام_وضع / uefi_query_gop_mode
    auto uefi_query_gop = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>("خطأ: يجب تحديد رقم الوضع");
        uint32_t modeNum = static_cast<uint32_t>(args[0]->toInt());
        LowLevel::UefiGopModeInfo info;
        auto status = mgr.queryGOPMode(modeNum, info);
        if (status != LowLevel::EfiStatus::SUCCESS) {
            return std::make_shared<Data::Value>("خطأ: وضع غير صالح");
        }
        std::ostringstream oss;
        oss << "وضع=" << modeNum
            << " عرض=" << info.horizontalResolution
            << " ارتفاع=" << info.verticalResolution
            << " تنسيق=" << static_cast<int>(info.pixelFormat)
            << " بكسل_لكل_سطر=" << info.pixelsPerScanLine;
        return std::make_shared<Data::Value>(oss.str());
    };

    // uefi_عدد_أوضاع_رسوميات / uefi_gop_mode_count
    auto uefi_gop_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(mgr.getGOPModeCount()));
    };

    // uefi_وضع_رسوميات_حالي / uefi_current_gop_mode
    auto uefi_cur_gop = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(mgr.getCurrentGOPMode()));
    };

    // uefi_عنوان_إطار / uefi_framebuffer_base
    auto uefi_fb_base = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(mgr.getFramebufferBase()));
    };

    // uefi_حجم_إطار / uefi_framebuffer_size
    auto uefi_fb_size = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(mgr.getFramebufferSize()));
    };

    // uefi_ملء_شاشة / uefi_fill_screen — ملء شاشة بلون (GOP BLT)
    auto uefi_fill = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 3) return std::make_shared<Data::Value>(-1);
        LowLevel::UefiGopBltPixel pixel;
        pixel.red   = static_cast<uint8_t>(args[0]->toInt());
        pixel.green = static_cast<uint8_t>(args[1]->toInt());
        pixel.blue  = static_cast<uint8_t>(args[2]->toInt());
        pixel.reserved = 0;
        auto& state = mgr.getGOPState();
        if (!state.initialized || state.modes.empty()) {
            return std::make_shared<Data::Value>(-1);
        }
        auto& mode = state.modes[state.currentMode];
        auto status = mgr.gopBlt(&pixel, LowLevel::UefiGopBltOperation::VideoFill,
                                  0, 0, 0, 0, mode.horizontalResolution, mode.verticalResolution);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_رسم_مستطيل / uefi_draw_rect — رسم مستطيل (GOP BLT)
    auto uefi_draw_rect = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        // args: x, y, width, height, red, green, blue
        if (args.size() < 7) return std::make_shared<Data::Value>(-1);
        uint32_t x = static_cast<uint32_t>(args[0]->toInt());
        uint32_t y = static_cast<uint32_t>(args[1]->toInt());
        uint32_t w = static_cast<uint32_t>(args[2]->toInt());
        uint32_t h = static_cast<uint32_t>(args[3]->toInt());
        LowLevel::UefiGopBltPixel pixel;
        pixel.red   = static_cast<uint8_t>(args[4]->toInt());
        pixel.green = static_cast<uint8_t>(args[5]->toInt());
        pixel.blue  = static_cast<uint8_t>(args[6]->toInt());
        pixel.reserved = 0;
        auto status = mgr.gopBlt(&pixel, LowLevel::UefiGopBltOperation::VideoFill,
                                  0, 0, x, y, w, h);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // ═══════════════════════════════════════════════════════════════
    // 4. خدمات وقت التشغيل / Runtime Services
    // ═══════════════════════════════════════════════════════════════
    // uefi_الوقت / uefi_get_time
    auto uefi_time = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        auto time = mgr.getTime();
        std::ostringstream oss;
        oss << time.year << "-"
            << (time.month < 10 ? "0" : "") << static_cast<int>(time.month) << "-"
            << (time.day < 10 ? "0" : "") << static_cast<int>(time.day) << " "
            << (time.hour < 10 ? "0" : "") << static_cast<int>(time.hour) << ":"
            << (time.minute < 10 ? "0" : "") << static_cast<int>(time.minute) << ":"
            << (time.second < 10 ? "0" : "") << static_cast<int>(time.second);
        return std::make_shared<Data::Value>(oss.str());
    };

    // uefi_قراءة_متغير / uefi_get_variable
    auto uefi_get_var = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>("خطأ: يجب تحديد اسم المتغير");
        std::string name = args[0]->toString();
        LowLevel::EFI_GUID guid = {0, 0, 0, {0}};
        std::vector<uint8_t> data;
        auto status = mgr.getVariable(name, guid, data);
        if (status != LowLevel::EfiStatus::SUCCESS) {
            return std::make_shared<Data::Value>(""); // متغير غير موجود
        }
        // تحويل البيانات إلى نص
        std::string result(data.begin(), data.end());
        return std::make_shared<Data::Value>(result);
    };

    // uefi_كتابة_متغير / uefi_set_variable
    auto uefi_set_var = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        std::string name = args[0]->toString();
        std::string value = args[1]->toString();
        LowLevel::EFI_GUID guid = {0, 0, 0, {0}};
        // تحويل النص إلى بايتات
        std::vector<uint8_t> data(value.begin(), value.end());
        uint32_t attrs = 0x07; // BS + RT + NV
        if (args.size() >= 3) attrs = static_cast<uint32_t>(args[2]->toInt());
        auto status = mgr.setVariable(name, guid, attrs, data);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // ═══════════════════════════════════════════════════════════════
    // 5. نظام الملفات / File System Protocol
    // ═══════════════════════════════════════════════════════════════
    // uefi_فتح_وحدة_تخزين / uefi_open_volume
    auto uefi_open_vol = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        int64_t fd = mgr.openVolume();
        return std::make_shared<Data::Value>(static_cast<int>(fd));
    };

    // uefi_فتح_ملف / uefi_open_file
    auto uefi_open_file = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        int64_t parentFd = static_cast<int64_t>(args[0]->toInt());
        std::string filename = args[1]->toString();
        uint64_t mode = LowLevel::EfiFileMode::READ;
        if (args.size() >= 3) mode = static_cast<uint64_t>(args[2]->toInt());
        int64_t fd = mgr.openFile(parentFd, filename, mode);
        return std::make_shared<Data::Value>(static_cast<int>(fd));
    };

    // uefi_قراءة_ملف / uefi_read_file
    auto uefi_read = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>("");
        int64_t fd = static_cast<int64_t>(args[0]->toInt());
        uint64_t size = 4096; // حجم افتراضي
        if (args.size() >= 2) size = static_cast<uint64_t>(args[1]->toInt());
        std::vector<uint8_t> buffer;
        int64_t bytesRead = mgr.readFile(fd, buffer, size);
        if (bytesRead < 0) {
            return std::make_shared<Data::Value>(""); // خطأ
        }
        std::string result(buffer.begin(), buffer.end());
        return std::make_shared<Data::Value>(result);
    };

    // uefi_كتابة_ملف / uefi_write_file
    auto uefi_write = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        int64_t fd = static_cast<int64_t>(args[0]->toInt());
        std::string data = args[1]->toString();
        std::vector<uint8_t> buffer(data.begin(), data.end());
        int64_t written = mgr.writeFile(fd, buffer);
        return std::make_shared<Data::Value>(static_cast<int>(written));
    };

    // uefi_إغلاق_ملف / uefi_close_file
    auto uefi_close = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>(-1);
        int64_t fd = static_cast<int64_t>(args[0]->toInt());
        auto status = mgr.closeFile(fd);
        return std::make_shared<Data::Value>(static_cast<int>(status));
    };

    // uefi_معلومات_ملف / uefi_file_info
    auto uefi_finfo = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>("خطأ: يجب تحديد واصف الملف");
        int64_t fd = static_cast<int64_t>(args[0]->toInt());
        auto info = mgr.getFileInfo(fd);
        std::ostringstream oss;
        oss << "اسم=" << info.fileName
            << " حجم=" << info.fileSize
            << " سمات=0x" << std::hex << info.attribute << std::dec;
        return std::make_shared<Data::Value>(oss.str());
    };

    // ═══════════════════════════════════════════════════════════════
    // 6. بروتوكولات ومعلومات النظام / Protocol & System Info
    // ═══════════════════════════════════════════════════════════════
    // uefi_بحث_بروتوكول / uefi_locate_protocol
    auto uefi_locate = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto& mgr = LowLevel::UefiManager::getInstance();
        if (args.size() < 1) return std::make_shared<Data::Value>(0);
        std::string guidName = args[0]->toString();
        // تحويل اسم مألوف إلى GUID
        LowLevel::EFI_GUID guid = {0, 0, 0, {0}};
        if (guidName == "GOP" || guidName == "رسوميات") {
            guid = LowLevel::EfiProtocol::GOP;
        } else if (guidName == "SimpleFS" || guidName == "نظام_ملفات") {
            guid = LowLevel::EfiProtocol::SIMPLE_FS;
        } else if (guidName == "LoadedImage" || guidName == "صورة_محملة") {
            guid = LowLevel::EfiProtocol::LOADED_IMAGE;
        } else if (guidName == "DevicePath" || guidName == "مسار_جهاز") {
            guid = LowLevel::EfiProtocol::DEVICE_PATH;
        }
        uint64_t handle = mgr.locateProtocol(guid);
        return std::make_shared<Data::Value>(static_cast<double>(handle));
    };

    // uefi_إصدار / uefi_revision
    auto uefi_rev = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.getUefiRevision());
    };

    // uefi_بائع / uefi_firmware_vendor
    auto uefi_vendor = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.getFirmwareVendor());
    };

    // uefi_إصدار_بائع / uefi_firmware_revision
    auto uefi_fw_rev = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(mgr.getFirmwareRevision()));
    };

    // ═══════════════════════════════════════════════════════════════
    // 7. التقارير / Reports
    // ═══════════════════════════════════════════════════════════════
    // uefi_تقرير / uefi_report — تقرير شامل
    auto uefi_report = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.generateReport());
    };

    // uefi_تقرير_ذاكرة / uefi_memory_report
    auto uefi_mem_report = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.generateMemoryMapReport());
    };

    // uefi_تقرير_رسوميات / uefi_gop_report
    auto uefi_gop_report = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mgr = LowLevel::UefiManager::getInstance();
        return std::make_shared<Data::Value>(mgr.generateGOPReport());
    };

} // registerBuiltinsKernelUEFI

} // namespace Interpreter
} // namespace Sad
