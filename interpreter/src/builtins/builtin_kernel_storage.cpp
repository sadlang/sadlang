/**
 * @file builtin_kernel_storage.cpp
 * @brief (AR) وحدة التخزين — إدارة الأقراص ونظام الملفات
 * @brief (EN) Storage module — disk and filesystem management
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
namespace Kstorage = Sad::Builtins::Names::KernelStorage;
#include "value.h"
#include "nvme.h"

#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelStorage(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. اكتشاف الأقراص / Drive Discovery
    // ═══════════════════════════════════════════════════════════════
    // 1. storage_scan / تخزين_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.scanDrives());
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_0), f);
    }

    // 2. storage_drive_count / تخزين_عدد_الأقراص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveCount());
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_1), f);
    }

    // 3. storage_drive_model / تخزين_موديل_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveInfo(id).model);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_2), f);
    }

    // 4. storage_drive_serial / تخزين_رقم_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveInfo(id).serial);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_3), f);
    }

    // 5. storage_drive_type / تخزين_نوع_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveInfo(id).type));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_4), f);
    }

    // 6. storage_drive_state / تخزين_حاله_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveInfo(id).state));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_5), f);
    }

    // 7. storage_report / تخزين_تقرير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.generateReport());
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_6), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 2. قراءة وكتابة القطاعات / Sector Read/Write
    // ═══════════════════════════════════════════════════════════════
    // 8. storage_read_sector / تخزين_قراءة_قطاع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            uint64_t lba = args.size() > 1 ? static_cast<uint64_t>(args[1]->toDouble()) : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.readSector(driveId, lba));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_7), f);
    }

    // 9. storage_write_sector / تخزين_كتابة_قطاع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            uint64_t lba = args.size() > 1 ? static_cast<uint64_t>(args[1]->toDouble()) : 0;
            std::string data = args.size() > 2 ? args[2]->toString() : "";
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.writeSector(driveId, lba, data));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_8), f);
    }

    // 10. storage_read_sectors / تخزين_قراءة_قطاعات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            uint64_t lba = args.size() > 1 ? static_cast<uint64_t>(args[1]->toDouble()) : 0;
            int count = args.size() > 2 ? args[2]->toInt() : 1;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.readSectors(driveId, lba, count));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_9), f);
    }

    // 11. storage_zero_sectors / تخزين_تصفير_قطاعات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            uint64_t lba = args.size() > 1 ? static_cast<uint64_t>(args[1]->toDouble()) : 0;
            int count = args.size() > 2 ? args[2]->toInt() : 1;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.zeroSectors(driveId, lba, count));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_10), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 3. S.M.A.R.T / معلومات الصحة
    // ═══════════════════════════════════════════════════════════════
    // 12. storage_smart_healthy / تخزين_صحة_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.isDriveHealthy(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_11), f);
    }

    // 13. storage_temperature / تخزين_حرارة_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveTemperature(id));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_12), f);
    }

    // 14. storage_smart_report / تخزين_تقرير_صحة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).report);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_13), f);
    }

    // 15. storage_smart_hours / تخزين_ساعات_التشغيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).powerOnHours);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_14), f);
    }

    // 16. storage_smart_percent / تخزين_نسبة_الاستخدام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).percentUsed);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_15), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 4. أقسام / Partitions
    // ═══════════════════════════════════════════════════════════════
    // 17. storage_partition_count / تخزين_عدد_الأقسام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getPartitionCount(id));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_16), f);
    }

    // 18. storage_partition_label / تخزين_اسم_القسم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            int partIdx = args.size() > 1 ? args[1]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getPartitionInfo(driveId, partIdx).label);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_17), f);
    }

    // 19. storage_partition_bootable / تخزين_القسم_اقلاعي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            int partIdx = args.size() > 1 ? args[1]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getPartitionInfo(driveId, partIdx).bootable ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_18), f);
    }

    // 20. storage_partition_scheme / تخزين_نظام_الأقسام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getPartitionScheme(id)));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_19), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 5. عمليات متقدمة / Advanced Operations
    // ═══════════════════════════════════════════════════════════════
    // 21. storage_capacity_mb / تخزين_السعة_ميغا
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveCapacityMB(id)));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_20), f);
    }

    // 22. storage_is_nvme / تخزين_هل_nvme
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.isNVMe(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_21), f);
    }

    // 23. storage_is_ahci / تخزين_هل_ahci
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.isAHCI(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_22), f);
    }

    // 24. storage_flush / تخزين_مزامنة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.flushCache(id));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_23), f);
    }

    // 25. storage_create_ramdisk / تخزين_انشاء_قرص_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            uint64_t size = args.size() > 0 ? static_cast<uint64_t>(args[0]->toDouble()) : 65536;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.createRamdisk(size));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_24), f);
    }

    // 26. storage_read_partitions / تخزين_قراءة_الأقسام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.readPartitionTable(id));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_25), f);
    }

    // 27. storage_firmware / تخزين_البرنامج_الثابت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveInfo(id).firmware);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_26), f);
    }

    // 28. storage_sector_size / تخزين_حجم_القطاع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveInfo(id).sectorSize));
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_27), f);
    }

    // 29. storage_reset / تخزين_اعادة_تعيين
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            sm.reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_28), f);
    }

    // 30. storage_smart_spare / تخزين_الاحتياطي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).availableSpare);
        };
        fm.registerBuiltinFunction(std::string(Kstorage::STORAGE_29), f);
    }

} // registerBuiltinsKernelStorage

} // namespace Interpreter
} // namespace Sad
