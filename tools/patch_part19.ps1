$file = "C:\s\s-programming-language\interpreter_new\src\builtins\builtin_registry_part19.cpp"
$content = [IO.File]::ReadAllText($file, [Text.Encoding]::UTF8)

# Section 11b: Extended ACPI (insert after "acpi_reboot" registration, before section 12)
$acpiExtended = @'

    // 11b. ACPI — الدوال الموسّعة / Extended ACPI functions

    auto acpi_init_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.initialize();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تهيئة", acpi_init_fn);
    fm.registerBuiltinFunction("acpi_init", acpi_init_fn);

    auto acpi_init_rsdp = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.initializeFromRSDP(addr);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تهيئة_من_rsdp", acpi_init_rsdp);
    fm.registerBuiltinFunction("acpi_init_from_rsdp", acpi_init_rsdp);

    auto acpi_find_table = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        auto& acpi = LowLevel::ACPIManager::getInstance();
        auto ptr = acpi.findTable(args[0]->toString().c_str());
        return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(ptr)));
    };
    fm.registerBuiltinFunction("acpi_بحث_جدول", acpi_find_table);
    fm.registerBuiltinFunction("acpi_find_table", acpi_find_table);

    auto acpi_enable = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        bool ok = acpi.enableACPI();
        return std::make_shared<Data::Value>(ok ? 0 : -1);
    };
    fm.registerBuiltinFunction("acpi_تفعيل", acpi_enable);
    fm.registerBuiltinFunction("acpi_enable", acpi_enable);

    auto acpi_disable = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.disableACPI();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تعطيل", acpi_disable);
    fm.registerBuiltinFunction("acpi_disable", acpi_disable);

    auto acpi_sleep = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int state = args.empty() ? 3 : args[0]->toInt();
        auto& acpi = LowLevel::ACPIManager::getInstance();
        bool ok = acpi.enterSleepState(static_cast<LowLevel::SleepState>(state));
        return std::make_shared<Data::Value>(ok ? 0 : -1);
    };
    fm.registerBuiltinFunction("acpi_نوم", acpi_sleep);
    fm.registerBuiltinFunction("acpi_sleep", acpi_sleep);

    auto acpi_pm_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(acpi.readPMTimer()));
    };
    fm.registerBuiltinFunction("acpi_قراءة_مؤقت", acpi_pm_timer);
    fm.registerBuiltinFunction("acpi_read_pm_timer", acpi_pm_timer);

    auto acpi_timer_32 = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.isPMTimer32Bit() ? 1 : 0);
    };
    fm.registerBuiltinFunction("acpi_مؤقت_32بت", acpi_timer_32);
    fm.registerBuiltinFunction("acpi_is_pm_timer_32bit", acpi_timer_32);

    auto acpi_delay = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint32_t us = args.empty() ? 1000 : static_cast<uint32_t>(args[0]->toInt());
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.delayMicroseconds(us);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تأخير", acpi_delay);
    fm.registerBuiltinFunction("acpi_delay_us", acpi_delay);

    auto acpi_proc_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(acpi.getProcessorCount()));
    };
    fm.registerBuiltinFunction("acpi_عدد_معالجات", acpi_proc_count);
    fm.registerBuiltinFunction("acpi_processor_count", acpi_proc_count);

    auto acpi_version = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(acpi.getACPIVersion()));
    };
    fm.registerBuiltinFunction("acpi_إصدار", acpi_version);
    fm.registerBuiltinFunction("acpi_version", acpi_version);

    auto acpi_is_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.isInitialized() ? 1 : 0);
    };
    fm.registerBuiltinFunction("acpi_هل_مهيأ", acpi_is_init);
    fm.registerBuiltinFunction("acpi_is_initialized", acpi_is_init);

    auto acpi_apic_addr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(acpi.getLocalAPICAddress()));
    };
    fm.registerBuiltinFunction("acpi_عنوان_apic", acpi_apic_addr);
    fm.registerBuiltinFunction("acpi_local_apic_address", acpi_apic_addr);

    auto acpi_ecam = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint16_t seg = args.size() > 0 ? static_cast<uint16_t>(args[0]->toInt()) : 0;
        uint8_t bus = args.size() > 1 ? static_cast<uint8_t>(args[1]->toInt()) : 0;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(acpi.getECAMBase(seg, bus)));
    };
    fm.registerBuiltinFunction("acpi_ecam_قاعدة", acpi_ecam);
    fm.registerBuiltinFunction("acpi_ecam_base", acpi_ecam);

'@

# Section 16b: Extended APIC (insert after "apic_calibrate_timer" registration, before section 17)
$apicExtended = @'

    // 16b. APIC — الدوال الموسّعة / Extended APIC functions

    auto apic_supported = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.isAPICSupported() ? 1 : 0);
    };
    fm.registerBuiltinFunction("apic_مدعوم", apic_supported);
    fm.registerBuiltinFunction("apic_supported", apic_supported);

    auto apic_x2_supported = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.isX2APICSupported() ? 1 : 0);
    };
    fm.registerBuiltinFunction("apic_x2_مدعوم", apic_x2_supported);
    fm.registerBuiltinFunction("apic_x2_supported", apic_x2_supported);

    auto apic_id = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(apic.getAPICId()));
    };
    fm.registerBuiltinFunction("apic_معرّف", apic_id);
    fm.registerBuiltinFunction("apic_id", apic_id);

    auto apic_set_priority = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t prio = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.setTaskPriority(prio);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أولوية_مهمة", apic_set_priority);
    fm.registerBuiltinFunction("apic_set_priority", apic_set_priority);

    auto apic_init_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t vec = args.size() > 0 ? static_cast<uint8_t>(args[0]->toInt()) : 32;
        int mode = args.size() > 1 ? args[1]->toInt() : 0;
        int div = args.size() > 2 ? args[2]->toInt() : 3;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.initTimer(vec, static_cast<LowLevel::TimerMode>(mode), static_cast<LowLevel::TimerDivide>(div));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_تهيئة_مؤقت", apic_init_timer);
    fm.registerBuiltinFunction("apic_init_timer", apic_init_timer);

    auto apic_start_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint32_t count = args.empty() ? 1000000 : static_cast<uint32_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.startTimer(count);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_بدء_مؤقت", apic_start_timer);
    fm.registerBuiltinFunction("apic_start_timer", apic_start_timer);

    auto apic_stop_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.stopTimer();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_إيقاف_مؤقت", apic_stop_timer);
    fm.registerBuiltinFunction("apic_stop_timer", apic_stop_timer);

    auto apic_timer_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(apic.getTimerCount()));
    };
    fm.registerBuiltinFunction("apic_عداد_مؤقت", apic_timer_count);
    fm.registerBuiltinFunction("apic_timer_count", apic_timer_count);

    auto apic_send_ipi_all = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t vec = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        bool self = args.size() > 1 ? (args[1]->toInt() != 0) : false;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendIPIToAll(vec, self);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_للكل", apic_send_ipi_all);
    fm.registerBuiltinFunction("apic_send_ipi_all", apic_send_ipi_all);

    auto apic_send_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t dest = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendINIT(dest);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_init", apic_send_init);
    fm.registerBuiltinFunction("apic_send_init", apic_send_init);

    auto apic_send_sipi = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        uint8_t dest = static_cast<uint8_t>(args[0]->toInt());
        uint8_t page = static_cast<uint8_t>(args[1]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendSIPI(dest, page);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_sipi", apic_send_sipi);
    fm.registerBuiltinFunction("apic_send_sipi", apic_send_sipi);

    auto apic_wait_delivery = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.waitForDelivery();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_انتظر_تسليم", apic_wait_delivery);
    fm.registerBuiltinFunction("apic_wait_delivery", apic_wait_delivery);

    auto apic_mask_irq = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t irq = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.maskIRQ(irq);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_قناع_irq", apic_mask_irq);
    fm.registerBuiltinFunction("apic_mask_irq", apic_mask_irq);

    auto apic_unmask_irq = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t irq = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.unmaskIRQ(irq);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_إلغاء_قناع_irq", apic_unmask_irq);
    fm.registerBuiltinFunction("apic_unmask_irq", apic_unmask_irq);

    auto apic_io_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(apic.getIOAPICCount()));
    };
    fm.registerBuiltinFunction("apic_عدد_io", apic_io_count);
    fm.registerBuiltinFunction("apic_io_count", apic_io_count);

    auto apic_init_io = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>(-1);
        uint8_t id = static_cast<uint8_t>(args[0]->toInt());
        uint64_t base = static_cast<uint64_t>(args[1]->toDouble());
        uint32_t gsib = static_cast<uint32_t>(args[2]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        bool ok = apic.initIOAPIC(id, base, gsib);
        return std::make_shared<Data::Value>(ok ? 0 : -1);
    };
    fm.registerBuiltinFunction("apic_تهيئة_io", apic_init_io);
    fm.registerBuiltinFunction("apic_init_io", apic_init_io);

'@

# Insert section 11b after "acpi_reboot" registration  
$marker1 = 'fm.registerBuiltinFunction("acpi_reboot", acpi_reboot);'
$insertPos1 = $content.IndexOf($marker1)
if ($insertPos1 -ge 0) {
    $insertPos1 += $marker1.Length
    $content = $content.Insert($insertPos1, $acpiExtended)
    Write-Host "Inserted section 11b (Extended ACPI) at position $insertPos1"
} else {
    Write-Host "ERROR: Could not find marker for section 11b"
}

# Insert section 16b after "apic_calibrate_timer" registration
$marker2 = 'fm.registerBuiltinFunction("apic_calibrate_timer", apic_timer_calibrate);'
$insertPos2 = $content.IndexOf($marker2)
if ($insertPos2 -ge 0) {
    $insertPos2 += $marker2.Length
    $content = $content.Insert($insertPos2, $apicExtended)
    Write-Host "Inserted section 16b (Extended APIC) at position $insertPos2"
} else {
    Write-Host "ERROR: Could not find marker for section 16b"
}

# Write back
[IO.File]::WriteAllText($file, $content, [Text.UTF8Encoding]::new($false))
$newLines = [IO.File]::ReadAllText($file).Split("`n").Count
Write-Host "File now has $newLines lines (was 1001)"
