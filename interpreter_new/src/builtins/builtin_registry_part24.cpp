/**
 * @file builtin_registry_part24.cpp
 * @brief (AR) دوال التخزين NVMe/AHCI — اكتشاف، قراءة/كتابة، S.M.A.R.T، أقسام
 * @brief (EN) NVMe/AHCI Storage functions — discovery, R/W, S.M.A.R.T, partitions
 */

#include "interpreter_core.h"
#include "value.h"
#include "nvme.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart24(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // 1. اكتشاف الأقراص / Drive Discovery
    // =================================================================

    // 1. storage_scan / تخزين_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.scanDrives());
        };
        fm.registerBuiltinFunction("storage_scan", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb3\xd8\xad", f);
    }

    // 2. storage_drive_count / تخزين_عدد_الأقراص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveCount());
        };
        fm.registerBuiltinFunction("storage_drive_count", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7\xd8\xb5", f);
    }

    // 3. storage_drive_model / تخزين_موديل_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveInfo(id).model);
        };
        fm.registerBuiltinFunction("storage_drive_model", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x85\xd9\x88\xd8\xaf\xd9\x8a\xd9\x84_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd8\xb5", f);
    }

    // 4. storage_drive_serial / تخزين_رقم_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveInfo(id).serial);
        };
        fm.registerBuiltinFunction("storage_drive_serial", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xb1\xd9\x82\xd9\x85_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd8\xb5", f);
    }

    // 5. storage_drive_type / تخزين_نوع_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveInfo(id).type));
        };
        fm.registerBuiltinFunction("storage_drive_type", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x86\xd9\x88\xd8\xb9_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd8\xb5", f);
    }

    // 6. storage_drive_state / تخزين_حاله_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveInfo(id).state));
        };
        fm.registerBuiltinFunction("storage_drive_state", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd9\x87_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd8\xb5", f);
    }

    // 7. storage_report / تخزين_تقرير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.generateReport());
        };
        fm.registerBuiltinFunction("storage_report", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1", f);
    }

    // =================================================================
    // 2. قراءة وكتابة القطاعات / Sector Read/Write
    // =================================================================

    // 8. storage_read_sector / تخزين_قراءة_قطاع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            uint64_t lba = args.size() > 1 ? static_cast<uint64_t>(args[1]->toDouble()) : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.readSector(driveId, lba));
        };
        fm.registerBuiltinFunction("storage_read_sector", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_\xd9\x82\xd8\xb7\xd8\xa7\xd8\xb9", f);
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
        fm.registerBuiltinFunction("storage_write_sector", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x83\xd8\xaa\xd8\xa7\xd8\xa8\xd8\xa9_\xd9\x82\xd8\xb7\xd8\xa7\xd8\xb9", f);
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
        fm.registerBuiltinFunction("storage_read_sectors", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_\xd9\x82\xd8\xb7\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaa", f);
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
        fm.registerBuiltinFunction("storage_zero_sectors", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xb5\xd9\x81\xd9\x8a\xd8\xb1_\xd9\x82\xd8\xb7\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaa", f);
    }

    // =================================================================
    // 3. S.M.A.R.T / معلومات الصحة
    // =================================================================

    // 12. storage_smart_healthy / تخزين_صحة_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.isDriveHealthy(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("storage_smart_healthy", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xb5\xd8\xad\xd8\xa9_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd8\xb5", f);
    }

    // 13. storage_temperature / تخزين_حرارة_القرص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveTemperature(id));
        };
        fm.registerBuiltinFunction("storage_temperature", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xad\xd8\xb1\xd8\xa7\xd8\xb1\xd8\xa9_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd8\xb5", f);
    }

    // 14. storage_smart_report / تخزين_تقرير_صحة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).report);
        };
        fm.registerBuiltinFunction("storage_smart_report", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1_\xd8\xb5\xd8\xad\xd8\xa9", f);
    }

    // 15. storage_smart_hours / تخزين_ساعات_التشغيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).powerOnHours);
        };
        fm.registerBuiltinFunction("storage_smart_hours", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84", f);
    }

    // 16. storage_smart_percent / تخزين_نسبة_الاستخدام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).percentUsed);
        };
        fm.registerBuiltinFunction("storage_smart_percent", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd8\xa7\xd9\x85", f);
    }

    // =================================================================
    // 4. أقسام / Partitions
    // =================================================================

    // 17. storage_partition_count / تخزين_عدد_الأقسام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getPartitionCount(id));
        };
        fm.registerBuiltinFunction("storage_partition_count", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa7\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85", f);
    }

    // 18. storage_partition_label / تخزين_اسم_القسم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            int partIdx = args.size() > 1 ? args[1]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getPartitionInfo(driveId, partIdx).label);
        };
        fm.registerBuiltinFunction("storage_partition_label", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb3\xd9\x85_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb3\xd9\x85", f);
    }

    // 19. storage_partition_bootable / تخزين_القسم_اقلاعي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int driveId = args.size() > 0 ? args[0]->toInt() : 0;
            int partIdx = args.size() > 1 ? args[1]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getPartitionInfo(driveId, partIdx).bootable ? 1 : 0);
        };
        fm.registerBuiltinFunction("storage_partition_bootable", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb3\xd9\x85_\xd8\xa7\xd9\x82\xd9\x84\xd8\xa7\xd8\xb9\xd9\x8a", f);
    }

    // 20. storage_partition_scheme / تخزين_نظام_الأقسام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getPartitionScheme(id)));
        };
        fm.registerBuiltinFunction("storage_partition_scheme", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85_\xd8\xa7\xd9\x84\xd8\xa7\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85", f);
    }

    // =================================================================
    // 5. عمليات متقدمة / Advanced Operations
    // =================================================================

    // 21. storage_capacity_mb / تخزين_السعة_ميغا
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveCapacityMB(id)));
        };
        fm.registerBuiltinFunction("storage_capacity_mb", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xb9\xd8\xa9_\xd9\x85\xd9\x8a\xd8\xba\xd8\xa7", f);
    }

    // 22. storage_is_nvme / تخزين_هل_nvme
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.isNVMe(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("storage_is_nvme", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x87\xd9\x84_nvme", f);
    }

    // 23. storage_is_ahci / تخزين_هل_ahci
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.isAHCI(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("storage_is_ahci", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x87\xd9\x84_ahci", f);
    }

    // 24. storage_flush / تخزين_مزامنة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.flushCache(id));
        };
        fm.registerBuiltinFunction("storage_flush", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86\xd8\xa9", f);
    }

    // 25. storage_create_ramdisk / تخزين_انشاء_قرص_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            uint64_t size = args.size() > 0 ? static_cast<uint64_t>(args[0]->toDouble()) : 65536;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.createRamdisk(size));
        };
        fm.registerBuiltinFunction("storage_create_ramdisk", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x86\xd8\xb4\xd8\xa7\xd8\xa1_\xd9\x82\xd8\xb1\xd8\xb5_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f);
    }

    // 26. storage_read_partitions / تخزين_قراءة_الأقسام
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.readPartitionTable(id));
        };
        fm.registerBuiltinFunction("storage_read_partitions", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xa7\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85", f);
    }

    // 27. storage_firmware / تخزين_البرنامج_الثابت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getDriveInfo(id).firmware);
        };
        fm.registerBuiltinFunction("storage_firmware", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x84\xd8\xa8\xd8\xb1\xd9\x86\xd8\xa7\xd9\x85\xd8\xac_\xd8\xa7\xd9\x84\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", f);
    }

    // 28. storage_sector_size / تخزين_حجم_القطاع
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(static_cast<int>(sm.getDriveInfo(id).sectorSize));
        };
        fm.registerBuiltinFunction("storage_sector_size", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd9\x82\xd8\xb7\xd8\xa7\xd8\xb9", f);
    }

    // 29. storage_reset / تخزين_اعادة_تعيين
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            auto& sm = LowLevel::StorageManager::getInstance();
            sm.reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("storage_reset", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86", f);
    }

    // 30. storage_smart_spare / تخزين_الاحتياطي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            auto& sm = LowLevel::StorageManager::getInstance();
            return std::make_shared<Data::Value>(sm.getSmartInfo(id).availableSpare);
        };
        fm.registerBuiltinFunction("storage_smart_spare", f);
        fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x84\xd8\xa7\xd8\xad\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb7\xd9\x8a", f);
    }

} // registerBuiltinsPart24

} // namespace Interpreter
} // namespace Sad
