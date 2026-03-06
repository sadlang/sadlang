; ModuleID = 'acpi_نحلة'
source_filename = "acpi_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@RSDP_EBDA_START = constant i64 655360
@RSDP_BIOS_START = constant i64 917504
@RSDP_BIOS_END = constant i64 1048576
@RSDP_SIG_0 = constant i64 82
@RSDP_SIG_1 = constant i64 83
@RSDP_SIG_2 = constant i64 68
@RSDP_SIG_3 = constant i64 32
@RSDP_SIG_4 = constant i64 80
@RSDP_SIG_5 = constant i64 84
@RSDP_SIG_6 = constant i64 82
@RSDP_SIG_7 = constant i64 32
@SIG_FACP_0 = constant i64 70
@SIG_FACP_1 = constant i64 65
@SIG_FACP_2 = constant i64 67
@SIG_FACP_3 = constant i64 80
@SIG_APIC_0 = constant i64 65
@SIG_APIC_1 = constant i64 80
@SIG_APIC_2 = constant i64 73
@SIG_APIC_3 = constant i64 67
@SIG_HPET_0 = constant i64 72
@SIG_HPET_1 = constant i64 80
@SIG_HPET_2 = constant i64 69
@SIG_HPET_3 = constant i64 84
@SIG_MCFG_0 = constant i64 77
@SIG_MCFG_1 = constant i64 67
@SIG_MCFG_2 = constant i64 70
@SIG_MCFG_3 = constant i64 71
@SIG_RSDT_0 = constant i64 82
@SIG_RSDT_1 = constant i64 83
@SIG_RSDT_2 = constant i64 68
@SIG_RSDT_3 = constant i64 84
@SIG_XSDT_0 = constant i64 88
@SIG_XSDT_1 = constant i64 83
@SIG_XSDT_2 = constant i64 68
@SIG_XSDT_3 = constant i64 84
@MADT_LAPIC = constant i64 0
@MADT_IOAPIC = constant i64 1
@MADT_ISO = constant i64 2
@MADT_NMI = constant i64 3
@MADT_LAPIC_NMI = constant i64 4
@FADT_PM1A_CTL = constant i64 64
@FADT_PM1B_CTL = constant i64 68
@FADT_PM1A_EVT = constant i64 56
@FADT_DSDT = constant i64 40
@FADT_SMI_CMD = constant i64 48
@FADT_ACPI_ENABLE = constant i64 52
@FADT_ACPI_DISABLE = constant i64 53
@FADT_SLP_TYPA = constant i64 0
@FADT_SLP_TYPB = constant i64 0
@PM1_SCI_EN = constant i64 1
@PM1_SLP_EN = constant i64 8192
@ACPI_MAX_TABLES = constant i64 32
@ACPI_MAX_LAPICS = constant i64 16
@ACPI_MAX_IOAPICS = constant i64 4
@ACPI_MAX_ISOS = constant i64 16
@acpi_rsdp_addr = global i64 0
@acpi_revision = global i64 0
@acpi_rsdt_addr = global i64 0
@acpi_xsdt_addr = global i64 0
@acpi_fadt_addr = global i64 0
@acpi_dsdt_addr = global i64 0
@acpi_pm1a_ctl = global i64 0
@acpi_pm1b_ctl = global i64 0
@acpi_pm1a_evt = global i64 0
@acpi_smi_cmd = global i64 0
@acpi_enable_val = global i64 0
@acpi_disable_val = global i64 0
@acpi_slp_typa = global i64 0
@acpi_slp_typb = global i64 0
@acpi_enabled = global i64 0
@acpi_madt_addr = global i64 0
@acpi_lapic_addr = global i64 0
@acpi_hpet_addr = global i64 0
@acpi_hpet_base = global i64 0
@acpi_mcfg_addr = global i64 0
@acpi_pcie_base = global i64 0
@lapic_id_0 = global i64 0
@lapic_id_1 = global i64 0
@lapic_id_2 = global i64 0
@lapic_id_3 = global i64 0
@lapic_id_4 = global i64 0
@lapic_id_5 = global i64 0
@lapic_id_6 = global i64 0
@lapic_id_7 = global i64 0
@lapic_id_8 = global i64 0
@lapic_id_9 = global i64 0
@lapic_id_10 = global i64 0
@lapic_id_11 = global i64 0
@lapic_id_12 = global i64 0
@lapic_id_13 = global i64 0
@lapic_id_14 = global i64 0
@lapic_id_15 = global i64 0
@lapic_proc_0 = global i64 0
@lapic_proc_1 = global i64 0
@lapic_proc_2 = global i64 0
@lapic_proc_3 = global i64 0
@lapic_proc_4 = global i64 0
@lapic_proc_5 = global i64 0
@lapic_proc_6 = global i64 0
@lapic_proc_7 = global i64 0
@lapic_proc_8 = global i64 0
@lapic_proc_9 = global i64 0
@lapic_proc_10 = global i64 0
@lapic_proc_11 = global i64 0
@lapic_proc_12 = global i64 0
@lapic_proc_13 = global i64 0
@lapic_proc_14 = global i64 0
@lapic_proc_15 = global i64 0
@lapic_flags_0 = global i64 0
@lapic_flags_1 = global i64 0
@lapic_flags_2 = global i64 0
@lapic_flags_3 = global i64 0
@lapic_flags_4 = global i64 0
@lapic_flags_5 = global i64 0
@lapic_flags_6 = global i64 0
@lapic_flags_7 = global i64 0
@lapic_flags_8 = global i64 0
@lapic_flags_9 = global i64 0
@lapic_flags_10 = global i64 0
@lapic_flags_11 = global i64 0
@lapic_flags_12 = global i64 0
@lapic_flags_13 = global i64 0
@lapic_flags_14 = global i64 0
@lapic_flags_15 = global i64 0
@lapic_count = global i64 0
@ioapic_id_0 = global i64 0
@ioapic_id_1 = global i64 0
@ioapic_id_2 = global i64 0
@ioapic_id_3 = global i64 0
@ioapic_addr_0 = global i64 0
@ioapic_addr_1 = global i64 0
@ioapic_addr_2 = global i64 0
@ioapic_addr_3 = global i64 0
@ioapic_gsi_0 = global i64 0
@ioapic_gsi_1 = global i64 0
@ioapic_gsi_2 = global i64 0
@ioapic_gsi_3 = global i64 0
@ioapic_count = global i64 0
@iso_bus_0 = global i64 0
@iso_bus_1 = global i64 0
@iso_bus_2 = global i64 0
@iso_bus_3 = global i64 0
@iso_bus_4 = global i64 0
@iso_bus_5 = global i64 0
@iso_bus_6 = global i64 0
@iso_bus_7 = global i64 0
@iso_source_0 = global i64 0
@iso_source_1 = global i64 0
@iso_source_2 = global i64 0
@iso_source_3 = global i64 0
@iso_source_4 = global i64 0
@iso_source_5 = global i64 0
@iso_source_6 = global i64 0
@iso_source_7 = global i64 0
@iso_gsi_0 = global i64 0
@iso_gsi_1 = global i64 0
@iso_gsi_2 = global i64 0
@iso_gsi_3 = global i64 0
@iso_gsi_4 = global i64 0
@iso_gsi_5 = global i64 0
@iso_gsi_6 = global i64 0
@iso_gsi_7 = global i64 0
@iso_flags_0 = global i64 0
@iso_flags_1 = global i64 0
@iso_flags_2 = global i64 0
@iso_flags_3 = global i64 0
@iso_flags_4 = global i64 0
@iso_flags_5 = global i64 0
@iso_flags_6 = global i64 0
@iso_flags_7 = global i64 0
@iso_count = global i64 0
@acpi_table_addr_0 = global i64 0
@acpi_table_addr_1 = global i64 0
@acpi_table_addr_2 = global i64 0
@acpi_table_addr_3 = global i64 0
@acpi_table_addr_4 = global i64 0
@acpi_table_addr_5 = global i64 0
@acpi_table_addr_6 = global i64 0
@acpi_table_addr_7 = global i64 0
@acpi_table_addr_8 = global i64 0
@acpi_table_addr_9 = global i64 0
@acpi_table_addr_10 = global i64 0
@acpi_table_addr_11 = global i64 0
@acpi_table_addr_12 = global i64 0
@acpi_table_addr_13 = global i64 0
@acpi_table_addr_14 = global i64 0
@acpi_table_addr_15 = global i64 0
@acpi_table_sig0_0 = global i64 0
@acpi_table_sig0_1 = global i64 0
@acpi_table_sig0_2 = global i64 0
@acpi_table_sig0_3 = global i64 0
@acpi_table_sig0_4 = global i64 0
@acpi_table_sig0_5 = global i64 0
@acpi_table_sig0_6 = global i64 0
@acpi_table_sig0_7 = global i64 0
@acpi_table_sig0_8 = global i64 0
@acpi_table_sig0_9 = global i64 0
@acpi_table_sig0_10 = global i64 0
@acpi_table_sig0_11 = global i64 0
@acpi_table_sig0_12 = global i64 0
@acpi_table_sig0_13 = global i64 0
@acpi_table_sig0_14 = global i64 0
@acpi_table_sig0_15 = global i64 0
@acpi_table_count = global i64 0
@acpi_errors = global i64 0
@str.const = private unnamed_addr constant [29 x i8] c"[ACPI] Searching for RSDP...\00", align 1
@str.const.1 = private unnamed_addr constant [23 x i8] c"[ACPI] RSDP not found!\00", align 1
@str.const.2 = private unnamed_addr constant [24 x i8] c"[ACPI] RSDP found at 0x\00", align 1
@str.const.3 = private unnamed_addr constant [18 x i8] c"[ACPI] Revision: \00", align 1
@str.const.4 = private unnamed_addr constant [18 x i8] c"[ACPI] RSDT at 0x\00", align 1
@str.const.5 = private unnamed_addr constant [18 x i8] c"[ACPI] XSDT at 0x\00", align 1
@str.const.6 = private unnamed_addr constant [31 x i8] c"[ACPI] Invalid RSDT signature!\00", align 1
@str.const.7 = private unnamed_addr constant [20 x i8] c"[ACPI] RSDT length=\00", align 1
@str.const.8 = private unnamed_addr constant [14 x i8] c"[ACPI] Found \00", align 1
@str.const.9 = private unnamed_addr constant [8 x i8] c" tables\00", align 1
@str.const.10 = private unnamed_addr constant [15 x i8] c"[ACPI] Table: \00", align 1
@str.const.11 = private unnamed_addr constant [7 x i8] c" at 0x\00", align 1
@str.const.12 = private unnamed_addr constant [21 x i8] c"[ACPI] No FADT found\00", align 1
@str.const.13 = private unnamed_addr constant [23 x i8] c"[ACPI] Parsing FADT...\00", align 1
@str.const.14 = private unnamed_addr constant [18 x i8] c"[ACPI] DSDT at 0x\00", align 1
@str.const.15 = private unnamed_addr constant [19 x i8] c"[ACPI] PM1a_CTL=0x\00", align 1
@str.const.16 = private unnamed_addr constant [13 x i8] c" PM1a_EVT=0x\00", align 1
@str.const.17 = private unnamed_addr constant [18 x i8] c"[ACPI] SMI_CMD=0x\00", align 1
@str.const.18 = private unnamed_addr constant [10 x i8] c" ACPI_EN=\00", align 1
@str.const.19 = private unnamed_addr constant [21 x i8] c"[ACPI] No MADT found\00", align 1
@str.const.20 = private unnamed_addr constant [23 x i8] c"[ACPI] Parsing MADT...\00", align 1
@str.const.21 = private unnamed_addr constant [21 x i8] c"[ACPI] LAPIC base=0x\00", align 1
@str.const.22 = private unnamed_addr constant [15 x i8] c"[ACPI] LAPICs=\00", align 1
@str.const.23 = private unnamed_addr constant [10 x i8] c" IOAPICs=\00", align 1
@str.const.24 = private unnamed_addr constant [7 x i8] c" ISOs=\00", align 1
@str.const.25 = private unnamed_addr constant [20 x i8] c"[ACPI] LAPIC: proc=\00", align 1
@str.const.26 = private unnamed_addr constant [5 x i8] c" id=\00", align 1
@str.const.27 = private unnamed_addr constant [8 x i8] c" flags=\00", align 1
@str.const.28 = private unnamed_addr constant [19 x i8] c"[ACPI] IOAPIC: id=\00", align 1
@str.const.29 = private unnamed_addr constant [9 x i8] c" addr=0x\00", align 1
@str.const.30 = private unnamed_addr constant [6 x i8] c" GSI=\00", align 1
@str.const.31 = private unnamed_addr constant [16 x i8] c"[ACPI] ISO: IRQ\00", align 1
@str.const.32 = private unnamed_addr constant [8 x i8] c" -> GSI\00", align 1
@str.const.33 = private unnamed_addr constant [23 x i8] c"[ACPI] Parsing HPET...\00", align 1
@str.const.34 = private unnamed_addr constant [20 x i8] c"[ACPI] HPET base=0x\00", align 1
@str.const.35 = private unnamed_addr constant [23 x i8] c"[ACPI] Parsing MCFG...\00", align 1
@str.const.36 = private unnamed_addr constant [25 x i8] c"[ACPI] PCIe ECAM base=0x\00", align 1
@str.const.37 = private unnamed_addr constant [23 x i8] c"[ACPI] Already enabled\00", align 1
@str.const.38 = private unnamed_addr constant [27 x i8] c"[ACPI] No SMI command port\00", align 1
@str.const.39 = private unnamed_addr constant [23 x i8] c"[ACPI] Enable timeout!\00", align 1
@str.const.40 = private unnamed_addr constant [23 x i8] c"[ACPI] Enabled via SMI\00", align 1
@str.const.41 = private unnamed_addr constant [19 x i8] c"[ACPI] Shutdown...\00", align 1
@str.const.42 = private unnamed_addr constant [35 x i8] c"[ACPI] No PM1a_CTL, can't shutdown\00", align 1
@str.const.43 = private unnamed_addr constant [29 x i8] c"[ACPI] Shutdown command sent\00", align 1
@str.const.44 = private unnamed_addr constant [44 x i8] c"[ACPI] Rebooting via keyboard controller...\00", align 1
@str.const.45 = private unnamed_addr constant [38 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [ACPI] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.46 = private unnamed_addr constant [13 x i8] c"  Revision: \00", align 1
@str.const.47 = private unnamed_addr constant [11 x i8] c"  Tables: \00", align 1
@str.const.48 = private unnamed_addr constant [10 x i8] c"  RSDT=0x\00", align 1
@str.const.49 = private unnamed_addr constant [9 x i8] c" FADT=0x\00", align 1
@str.const.50 = private unnamed_addr constant [10 x i8] c"  MADT=0x\00", align 1
@str.const.51 = private unnamed_addr constant [9 x i8] c" LAPICs=\00", align 1
@str.const.53 = private unnamed_addr constant [17 x i8] c"  ACPI enabled: \00", align 1
@str.const.54 = private unnamed_addr constant [11 x i8] c"  Errors: \00", align 1
@str.const.55 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.56 = private unnamed_addr constant [26 x i8] c"[ACPI] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 ACPI...\00", align 1
@str.const.57 = private unnamed_addr constant [26 x i8] c"[ACPI] ACPI not available\00", align 1
@str.const.58 = private unnamed_addr constant [16 x i8] c"[ACPI] \D8\AC\D8\A7\D9\87\D8\B2\00", align 1

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

define i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%ch" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_0

merge_4:                                          ; preds = %while_body_1
  %"%COM1.load" = load i64, ptr @COM1, align 8
  %"%ch.load3" = load i64, ptr %"%ch", align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %"%ch.load3")
  %"%i.load4" = load i64, ptr %"%i", align 8
  %addtmp5 = add i64 %"%i.load4", 1
  store i64 %addtmp5, ptr %"%i", align 8
  br label %while_cond_0

then_3:                                           ; preds = %while_body_1
  ret i64 0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load", %"%i.load2"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%ch", align 8
  %"%ch.load" = load i64, ptr %"%ch", align 8
  %cmpeqtmp = icmp eq i64 %"%ch.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

while_cond_0:                                     ; preds = %merge_4, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 80
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret i64 0
}

define i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%d2" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%d4" = alloca i64, align 8
  %"%d5" = alloca i64, align 8
  %"%val" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%17.load" = load i64, ptr %"\D9\861", align 8
  store i64 %"%17.load", ptr %"%val", align 8
  %"%val.load" = load i64, ptr %"%val", align 8
  %cmplttmp = icmp slt i64 %"%val.load", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

else_11:                                          ; preds = %merge_8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp19 = icmp sge i64 %"%\D9\86.load", 10000
  br i1 %cmpgetmp19, label %then_12, label %merge_13

else_16:                                          ; preds = %merge_10
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp32 = icmp sge i64 %"%\D9\86.load31", 1000
  br i1 %cmpgetmp32, label %then_17, label %merge_18

else_21:                                          ; preds = %merge_15
  %"%\D9\86.load44" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp45 = icmp sge i64 %"%\D9\86.load44", 100
  br i1 %cmpgetmp45, label %then_22, label %merge_23

merge_10:                                         ; preds = %merge_13, %then_9
  %"%val.load20" = load i64, ptr %"%val", align 8
  %cmpgetmp21 = icmp sge i64 %"%val.load20", 100
  br i1 %cmpgetmp21, label %then_14, label %else_16

merge_13:                                         ; preds = %then_12, %else_11
  br label %merge_10

merge_15:                                         ; preds = %merge_18, %then_14
  %"%val.load33" = load i64, ptr %"%val", align 8
  %cmpgetmp34 = icmp sge i64 %"%val.load33", 10
  br i1 %cmpgetmp34, label %then_19, label %else_21

merge_18:                                         ; preds = %then_17, %else_16
  br label %merge_15

merge_20:                                         ; preds = %merge_23, %then_19
  %"%val.load46" = load i64, ptr %"%val", align 8
  %addtmp47 = add i64 48, %"%val.load46"
  %"%COM1.load48" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load48", i64 %addtmp47)
  ret i64 0

merge_23:                                         ; preds = %then_22, %else_21
  br label %merge_20

merge_6:                                          ; preds = %then_5, %entry
  %"%val.load3" = load i64, ptr %"%val", align 8
  %cmpgetmp = icmp sge i64 %"%val.load3", 10000
  br i1 %cmpgetmp, label %then_7, label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  %"%val.load9" = load i64, ptr %"%val", align 8
  %cmpgetmp10 = icmp sge i64 %"%val.load9", 1000
  br i1 %cmpgetmp10, label %then_9, label %else_11

then_12:                                          ; preds = %else_11
  %"%COM1.load22" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load22", i64 48)
  br label %merge_13

then_14:                                          ; preds = %merge_10
  %"%val.load23" = load i64, ptr %"%val", align 8
  %divtmp24 = sdiv i64 %"%val.load23", 100
  store i64 %divtmp24, ptr %"%d3", align 8
  %"%d3.load" = load i64, ptr %"%d3", align 8
  %addtmp25 = add i64 48, %"%d3.load"
  %"%COM1.load26" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load26", i64 %addtmp25)
  %"%d3.load27" = load i64, ptr %"%d3", align 8
  %multmp28 = mul i64 %"%d3.load27", 100
  %"%val.load29" = load i64, ptr %"%val", align 8
  %subtmp30 = sub i64 %"%val.load29", %multmp28
  store i64 %subtmp30, ptr %"%val", align 8
  br label %merge_15

then_17:                                          ; preds = %else_16
  %"%COM1.load35" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load35", i64 48)
  br label %merge_18

then_19:                                          ; preds = %merge_15
  %"%val.load36" = load i64, ptr %"%val", align 8
  %divtmp37 = sdiv i64 %"%val.load36", 10
  store i64 %divtmp37, ptr %"%d2", align 8
  %"%d2.load" = load i64, ptr %"%d2", align 8
  %addtmp38 = add i64 48, %"%d2.load"
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 %addtmp38)
  %"%d2.load40" = load i64, ptr %"%d2", align 8
  %multmp41 = mul i64 %"%d2.load40", 10
  %"%val.load42" = load i64, ptr %"%val", align 8
  %subtmp43 = sub i64 %"%val.load42", %multmp41
  store i64 %subtmp43, ptr %"%val", align 8
  br label %merge_20

then_22:                                          ; preds = %else_21
  %"%COM1.load49" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load49", i64 48)
  br label %merge_23

then_5:                                           ; preds = %entry
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 45)
  %"%val.load2" = load i64, ptr %"%val", align 8
  %subtmp = sub i64 0, %"%val.load2"
  store i64 %subtmp, ptr %"%val", align 8
  br label %merge_6

then_7:                                           ; preds = %merge_6
  %"%val.load4" = load i64, ptr %"%val", align 8
  %divtmp = sdiv i64 %"%val.load4", 10000
  store i64 %divtmp, ptr %"%d5", align 8
  %"%d5.load" = load i64, ptr %"%d5", align 8
  %addtmp = add i64 48, %"%d5.load"
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 %addtmp)
  %"%d5.load6" = load i64, ptr %"%d5", align 8
  %multmp = mul i64 %"%d5.load6", 10000
  %"%val.load7" = load i64, ptr %"%val", align 8
  %subtmp8 = sub i64 %"%val.load7", %multmp
  store i64 %subtmp8, ptr %"%val", align 8
  br label %merge_8

then_9:                                           ; preds = %merge_8
  %"%val.load11" = load i64, ptr %"%val", align 8
  %divtmp12 = sdiv i64 %"%val.load11", 1000
  store i64 %divtmp12, ptr %"%d4", align 8
  %"%d4.load" = load i64, ptr %"%d4", align 8
  %addtmp13 = add i64 48, %"%d4.load"
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 %addtmp13)
  %"%d4.load15" = load i64, ptr %"%d4", align 8
  %multmp16 = mul i64 %"%d4.load15", 1000
  %"%val.load17" = load i64, ptr %"%val", align 8
  %subtmp18 = sub i64 %"%val.load17", %multmp16
  store i64 %subtmp18, ptr %"%val", align 8
  br label %merge_10
}

define i64 @acpi_serial_hex8(i64 %val) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 4
  store i64 %shrtmp, ptr %"%hi", align 8
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 15
  store i64 %andtmp, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %cmplttmp = icmp slt i64 %"%hi.load", 10
  br i1 %cmplttmp, label %then_24, label %else_26

else_26:                                          ; preds = %entry
  %"%hi.load4" = load i64, ptr %"%hi", align 8
  %addtmp5 = add i64 55, %"%hi.load4"
  %"%COM1.load6" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load6", i64 %addtmp5)
  br label %merge_25

else_29:                                          ; preds = %merge_25
  %"%lo.load11" = load i64, ptr %"%lo", align 8
  %addtmp12 = add i64 55, %"%lo.load11"
  %"%COM1.load13" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load13", i64 %addtmp12)
  br label %merge_28

merge_25:                                         ; preds = %else_26, %then_24
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %cmplttmp7 = icmp slt i64 %"%lo.load", 10
  br i1 %cmplttmp7, label %then_27, label %else_29

merge_28:                                         ; preds = %else_29, %then_27
  ret i64 0

then_24:                                          ; preds = %entry
  %"%hi.load3" = load i64, ptr %"%hi", align 8
  %addtmp = add i64 48, %"%hi.load3"
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %addtmp)
  br label %merge_25

then_27:                                          ; preds = %merge_25
  %"%lo.load8" = load i64, ptr %"%lo", align 8
  %addtmp9 = add i64 48, %"%lo.load8"
  %"%COM1.load10" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load10", i64 %addtmp9)
  br label %merge_28
}

define i64 @acpi_serial_hex32(i64 %val) {
entry:
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 24
  %calltmp = call i64 @acpi_serial_hex8(i64 %shrtmp)
  %"%val.load2" = load i64, ptr %val1, align 8
  %shrtmp3 = lshr i64 %"%val.load2", 16
  %andtmp = and i64 %shrtmp3, 255
  %calltmp4 = call i64 @acpi_serial_hex8(i64 %andtmp)
  %"%val.load5" = load i64, ptr %val1, align 8
  %shrtmp6 = lshr i64 %"%val.load5", 8
  %andtmp7 = and i64 %shrtmp6, 255
  %calltmp8 = call i64 @acpi_serial_hex8(i64 %andtmp7)
  %"%val.load9" = load i64, ptr %val1, align 8
  %andtmp10 = and i64 %"%val.load9", 255
  %calltmp11 = call i64 @acpi_serial_hex8(i64 %andtmp10)
  ret i64 0
}

define i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @acpi_serial_sig(i64 %addr) {
entry:
  %addr1 = alloca i64, align 8
  store i64 %addr, ptr %addr1, align 8
  %"%addr.load" = load i64, ptr %addr1, align 8
  call void @sad_ll_mem_read8(i64 %"%addr.load")
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 0)
  %"%addr.load2" = load i64, ptr %addr1, align 8
  %addtmp = add i64 %"%addr.load2", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  %"%COM1.load3" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load3", i64 0)
  %"%addr.load4" = load i64, ptr %addr1, align 8
  %addtmp5 = add i64 %"%addr.load4", 2
  call void @sad_ll_mem_read8(i64 %addtmp5)
  %"%COM1.load6" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load6", i64 0)
  %"%addr.load7" = load i64, ptr %addr1, align 8
  %addtmp8 = add i64 %"%addr.load7", 3
  call void @sad_ll_mem_read8(i64 %addtmp8)
  %"%COM1.load9" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load9", i64 0)
  ret i64 0
}

define i64 @acpi_set_lapic_id(i64 %idx, i64 %val) {
entry:
  %"%lapic_id_15" = alloca i64, align 8
  %"%lapic_id_14" = alloca i64, align 8
  %"%lapic_id_13" = alloca i64, align 8
  %"%lapic_id_12" = alloca i64, align 8
  %"%lapic_id_11" = alloca i64, align 8
  %"%lapic_id_10" = alloca i64, align 8
  %"%lapic_id_9" = alloca i64, align 8
  %"%lapic_id_8" = alloca i64, align 8
  %"%lapic_id_7" = alloca i64, align 8
  %"%lapic_id_6" = alloca i64, align 8
  %"%lapic_id_5" = alloca i64, align 8
  %"%lapic_id_4" = alloca i64, align 8
  %"%lapic_id_3" = alloca i64, align 8
  %"%lapic_id_2" = alloca i64, align 8
  %"%lapic_id_1" = alloca i64, align 8
  %"%lapic_id_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

merge_31:                                         ; preds = %then_30, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_32, label %merge_33

merge_33:                                         ; preds = %then_32, %merge_31
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_34, label %merge_35

merge_35:                                         ; preds = %then_34, %merge_33
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_36, label %merge_37

merge_37:                                         ; preds = %then_36, %merge_35
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_38, label %merge_39

merge_39:                                         ; preds = %then_38, %merge_37
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_40, label %merge_41

merge_41:                                         ; preds = %then_40, %merge_39
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_42, label %merge_43

merge_43:                                         ; preds = %then_42, %merge_41
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_44, label %merge_45

merge_45:                                         ; preds = %then_44, %merge_43
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_46, label %merge_47

merge_47:                                         ; preds = %then_46, %merge_45
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_48, label %merge_49

merge_49:                                         ; preds = %then_48, %merge_47
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_50, label %merge_51

merge_51:                                         ; preds = %then_50, %merge_49
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %merge_53
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_56, label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_58, label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_60, label %merge_61

merge_61:                                         ; preds = %then_60, %merge_59
  ret i64 0

then_30:                                          ; preds = %entry
  %"%149.load" = load i64, ptr %val2, align 8
  store i64 %"%149.load", ptr %"%lapic_id_0", align 8
  br label %merge_31

then_32:                                          ; preds = %merge_31
  %"%153.load" = load i64, ptr %val2, align 8
  store i64 %"%153.load", ptr %"%lapic_id_1", align 8
  br label %merge_33

then_34:                                          ; preds = %merge_33
  %"%157.load" = load i64, ptr %val2, align 8
  store i64 %"%157.load", ptr %"%lapic_id_2", align 8
  br label %merge_35

then_36:                                          ; preds = %merge_35
  %"%161.load" = load i64, ptr %val2, align 8
  store i64 %"%161.load", ptr %"%lapic_id_3", align 8
  br label %merge_37

then_38:                                          ; preds = %merge_37
  %"%165.load" = load i64, ptr %val2, align 8
  store i64 %"%165.load", ptr %"%lapic_id_4", align 8
  br label %merge_39

then_40:                                          ; preds = %merge_39
  %"%169.load" = load i64, ptr %val2, align 8
  store i64 %"%169.load", ptr %"%lapic_id_5", align 8
  br label %merge_41

then_42:                                          ; preds = %merge_41
  %"%173.load" = load i64, ptr %val2, align 8
  store i64 %"%173.load", ptr %"%lapic_id_6", align 8
  br label %merge_43

then_44:                                          ; preds = %merge_43
  %"%177.load" = load i64, ptr %val2, align 8
  store i64 %"%177.load", ptr %"%lapic_id_7", align 8
  br label %merge_45

then_46:                                          ; preds = %merge_45
  %"%181.load" = load i64, ptr %val2, align 8
  store i64 %"%181.load", ptr %"%lapic_id_8", align 8
  br label %merge_47

then_48:                                          ; preds = %merge_47
  %"%185.load" = load i64, ptr %val2, align 8
  store i64 %"%185.load", ptr %"%lapic_id_9", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%189.load" = load i64, ptr %val2, align 8
  store i64 %"%189.load", ptr %"%lapic_id_10", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%193.load" = load i64, ptr %val2, align 8
  store i64 %"%193.load", ptr %"%lapic_id_11", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%197.load" = load i64, ptr %val2, align 8
  store i64 %"%197.load", ptr %"%lapic_id_12", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%201.load" = load i64, ptr %val2, align 8
  store i64 %"%201.load", ptr %"%lapic_id_13", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%205.load" = load i64, ptr %val2, align 8
  store i64 %"%205.load", ptr %"%lapic_id_14", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%209.load" = load i64, ptr %val2, align 8
  store i64 %"%209.load", ptr %"%lapic_id_15", align 8
  br label %merge_61
}

define i64 @acpi_set_lapic_proc(i64 %idx, i64 %val) {
entry:
  %"%lapic_proc_15" = alloca i64, align 8
  %"%lapic_proc_14" = alloca i64, align 8
  %"%lapic_proc_13" = alloca i64, align 8
  %"%lapic_proc_12" = alloca i64, align 8
  %"%lapic_proc_11" = alloca i64, align 8
  %"%lapic_proc_10" = alloca i64, align 8
  %"%lapic_proc_9" = alloca i64, align 8
  %"%lapic_proc_8" = alloca i64, align 8
  %"%lapic_proc_7" = alloca i64, align 8
  %"%lapic_proc_6" = alloca i64, align 8
  %"%lapic_proc_5" = alloca i64, align 8
  %"%lapic_proc_4" = alloca i64, align 8
  %"%lapic_proc_3" = alloca i64, align 8
  %"%lapic_proc_2" = alloca i64, align 8
  %"%lapic_proc_1" = alloca i64, align 8
  %"%lapic_proc_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_62, label %merge_63

merge_63:                                         ; preds = %then_62, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_64, label %merge_65

merge_65:                                         ; preds = %then_64, %merge_63
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_66, label %merge_67

merge_67:                                         ; preds = %then_66, %merge_65
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_68, label %merge_69

merge_69:                                         ; preds = %then_68, %merge_67
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %merge_69
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_72, label %merge_73

merge_73:                                         ; preds = %then_72, %merge_71
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_74, label %merge_75

merge_75:                                         ; preds = %then_74, %merge_73
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %merge_75
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %merge_87
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  ret i64 0

then_62:                                          ; preds = %entry
  %"%214.load" = load i64, ptr %val2, align 8
  store i64 %"%214.load", ptr %"%lapic_proc_0", align 8
  br label %merge_63

then_64:                                          ; preds = %merge_63
  %"%218.load" = load i64, ptr %val2, align 8
  store i64 %"%218.load", ptr %"%lapic_proc_1", align 8
  br label %merge_65

then_66:                                          ; preds = %merge_65
  %"%222.load" = load i64, ptr %val2, align 8
  store i64 %"%222.load", ptr %"%lapic_proc_2", align 8
  br label %merge_67

then_68:                                          ; preds = %merge_67
  %"%226.load" = load i64, ptr %val2, align 8
  store i64 %"%226.load", ptr %"%lapic_proc_3", align 8
  br label %merge_69

then_70:                                          ; preds = %merge_69
  %"%230.load" = load i64, ptr %val2, align 8
  store i64 %"%230.load", ptr %"%lapic_proc_4", align 8
  br label %merge_71

then_72:                                          ; preds = %merge_71
  %"%234.load" = load i64, ptr %val2, align 8
  store i64 %"%234.load", ptr %"%lapic_proc_5", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%238.load" = load i64, ptr %val2, align 8
  store i64 %"%238.load", ptr %"%lapic_proc_6", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%242.load" = load i64, ptr %val2, align 8
  store i64 %"%242.load", ptr %"%lapic_proc_7", align 8
  br label %merge_77

then_78:                                          ; preds = %merge_77
  %"%246.load" = load i64, ptr %val2, align 8
  store i64 %"%246.load", ptr %"%lapic_proc_8", align 8
  br label %merge_79

then_80:                                          ; preds = %merge_79
  %"%250.load" = load i64, ptr %val2, align 8
  store i64 %"%250.load", ptr %"%lapic_proc_9", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%254.load" = load i64, ptr %val2, align 8
  store i64 %"%254.load", ptr %"%lapic_proc_10", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%258.load" = load i64, ptr %val2, align 8
  store i64 %"%258.load", ptr %"%lapic_proc_11", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%262.load" = load i64, ptr %val2, align 8
  store i64 %"%262.load", ptr %"%lapic_proc_12", align 8
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%266.load" = load i64, ptr %val2, align 8
  store i64 %"%266.load", ptr %"%lapic_proc_13", align 8
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%270.load" = load i64, ptr %val2, align 8
  store i64 %"%270.load", ptr %"%lapic_proc_14", align 8
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%274.load" = load i64, ptr %val2, align 8
  store i64 %"%274.load", ptr %"%lapic_proc_15", align 8
  br label %merge_93
}

define i64 @acpi_set_lapic_flags(i64 %idx, i64 %val) {
entry:
  %"%lapic_flags_15" = alloca i64, align 8
  %"%lapic_flags_14" = alloca i64, align 8
  %"%lapic_flags_13" = alloca i64, align 8
  %"%lapic_flags_12" = alloca i64, align 8
  %"%lapic_flags_11" = alloca i64, align 8
  %"%lapic_flags_10" = alloca i64, align 8
  %"%lapic_flags_9" = alloca i64, align 8
  %"%lapic_flags_8" = alloca i64, align 8
  %"%lapic_flags_7" = alloca i64, align 8
  %"%lapic_flags_6" = alloca i64, align 8
  %"%lapic_flags_5" = alloca i64, align 8
  %"%lapic_flags_4" = alloca i64, align 8
  %"%lapic_flags_3" = alloca i64, align 8
  %"%lapic_flags_2" = alloca i64, align 8
  %"%lapic_flags_1" = alloca i64, align 8
  %"%lapic_flags_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_94, label %merge_95

merge_101:                                        ; preds = %then_100, %merge_99
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 4
  br i1 %cmpeqtmp10, label %then_102, label %merge_103

merge_103:                                        ; preds = %then_102, %merge_101
  %"%idx.load11" = load i64, ptr %idx1, align 8
  %cmpeqtmp12 = icmp eq i64 %"%idx.load11", 5
  br i1 %cmpeqtmp12, label %then_104, label %merge_105

merge_105:                                        ; preds = %then_104, %merge_103
  %"%idx.load13" = load i64, ptr %idx1, align 8
  %cmpeqtmp14 = icmp eq i64 %"%idx.load13", 6
  br i1 %cmpeqtmp14, label %then_106, label %merge_107

merge_107:                                        ; preds = %then_106, %merge_105
  %"%idx.load15" = load i64, ptr %idx1, align 8
  %cmpeqtmp16 = icmp eq i64 %"%idx.load15", 7
  br i1 %cmpeqtmp16, label %then_108, label %merge_109

merge_109:                                        ; preds = %then_108, %merge_107
  %"%idx.load17" = load i64, ptr %idx1, align 8
  %cmpeqtmp18 = icmp eq i64 %"%idx.load17", 8
  br i1 %cmpeqtmp18, label %then_110, label %merge_111

merge_111:                                        ; preds = %then_110, %merge_109
  %"%idx.load19" = load i64, ptr %idx1, align 8
  %cmpeqtmp20 = icmp eq i64 %"%idx.load19", 9
  br i1 %cmpeqtmp20, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %merge_111
  %"%idx.load21" = load i64, ptr %idx1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%idx.load21", 10
  br i1 %cmpeqtmp22, label %then_114, label %merge_115

merge_115:                                        ; preds = %then_114, %merge_113
  %"%idx.load23" = load i64, ptr %idx1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%idx.load23", 11
  br i1 %cmpeqtmp24, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%idx.load25" = load i64, ptr %idx1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%idx.load25", 12
  br i1 %cmpeqtmp26, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  %"%idx.load27" = load i64, ptr %idx1, align 8
  %cmpeqtmp28 = icmp eq i64 %"%idx.load27", 13
  br i1 %cmpeqtmp28, label %then_120, label %merge_121

merge_121:                                        ; preds = %then_120, %merge_119
  %"%idx.load29" = load i64, ptr %idx1, align 8
  %cmpeqtmp30 = icmp eq i64 %"%idx.load29", 14
  br i1 %cmpeqtmp30, label %then_122, label %merge_123

merge_123:                                        ; preds = %then_122, %merge_121
  %"%idx.load31" = load i64, ptr %idx1, align 8
  %cmpeqtmp32 = icmp eq i64 %"%idx.load31", 15
  br i1 %cmpeqtmp32, label %then_124, label %merge_125

merge_125:                                        ; preds = %then_124, %merge_123
  ret i64 0

merge_95:                                         ; preds = %then_94, %entry
  %"%idx.load3" = load i64, ptr %idx1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%idx.load3", 1
  br i1 %cmpeqtmp4, label %then_96, label %merge_97

merge_97:                                         ; preds = %then_96, %merge_95
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 2
  br i1 %cmpeqtmp6, label %then_98, label %merge_99

merge_99:                                         ; preds = %then_98, %merge_97
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 3
  br i1 %cmpeqtmp8, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %"%291.load" = load i64, ptr %val2, align 8
  store i64 %"%291.load", ptr %"%lapic_flags_3", align 8
  br label %merge_101

then_102:                                         ; preds = %merge_101
  %"%295.load" = load i64, ptr %val2, align 8
  store i64 %"%295.load", ptr %"%lapic_flags_4", align 8
  br label %merge_103

then_104:                                         ; preds = %merge_103
  %"%299.load" = load i64, ptr %val2, align 8
  store i64 %"%299.load", ptr %"%lapic_flags_5", align 8
  br label %merge_105

then_106:                                         ; preds = %merge_105
  %"%303.load" = load i64, ptr %val2, align 8
  store i64 %"%303.load", ptr %"%lapic_flags_6", align 8
  br label %merge_107

then_108:                                         ; preds = %merge_107
  %"%307.load" = load i64, ptr %val2, align 8
  store i64 %"%307.load", ptr %"%lapic_flags_7", align 8
  br label %merge_109

then_110:                                         ; preds = %merge_109
  %"%311.load" = load i64, ptr %val2, align 8
  store i64 %"%311.load", ptr %"%lapic_flags_8", align 8
  br label %merge_111

then_112:                                         ; preds = %merge_111
  %"%315.load" = load i64, ptr %val2, align 8
  store i64 %"%315.load", ptr %"%lapic_flags_9", align 8
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%319.load" = load i64, ptr %val2, align 8
  store i64 %"%319.load", ptr %"%lapic_flags_10", align 8
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%323.load" = load i64, ptr %val2, align 8
  store i64 %"%323.load", ptr %"%lapic_flags_11", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%327.load" = load i64, ptr %val2, align 8
  store i64 %"%327.load", ptr %"%lapic_flags_12", align 8
  br label %merge_119

then_120:                                         ; preds = %merge_119
  %"%331.load" = load i64, ptr %val2, align 8
  store i64 %"%331.load", ptr %"%lapic_flags_13", align 8
  br label %merge_121

then_122:                                         ; preds = %merge_121
  %"%335.load" = load i64, ptr %val2, align 8
  store i64 %"%335.load", ptr %"%lapic_flags_14", align 8
  br label %merge_123

then_124:                                         ; preds = %merge_123
  %"%339.load" = load i64, ptr %val2, align 8
  store i64 %"%339.load", ptr %"%lapic_flags_15", align 8
  br label %merge_125

then_94:                                          ; preds = %entry
  %"%279.load" = load i64, ptr %val2, align 8
  store i64 %"%279.load", ptr %"%lapic_flags_0", align 8
  br label %merge_95

then_96:                                          ; preds = %merge_95
  %"%283.load" = load i64, ptr %val2, align 8
  store i64 %"%283.load", ptr %"%lapic_flags_1", align 8
  br label %merge_97

then_98:                                          ; preds = %merge_97
  %"%287.load" = load i64, ptr %val2, align 8
  store i64 %"%287.load", ptr %"%lapic_flags_2", align 8
  br label %merge_99
}

define i64 @acpi_set_ioapic(i64 %idx, i64 %id_val, i64 %addr_val, i64 %gsi_val) {
entry:
  %"%ioapic_gsi_3" = alloca i64, align 8
  %"%ioapic_addr_3" = alloca i64, align 8
  %"%ioapic_id_3" = alloca i64, align 8
  %"%ioapic_gsi_2" = alloca i64, align 8
  %"%ioapic_addr_2" = alloca i64, align 8
  %"%ioapic_id_2" = alloca i64, align 8
  %"%ioapic_gsi_1" = alloca i64, align 8
  %"%ioapic_addr_1" = alloca i64, align 8
  %"%ioapic_id_1" = alloca i64, align 8
  %"%ioapic_gsi_0" = alloca i64, align 8
  %"%ioapic_addr_0" = alloca i64, align 8
  %"%ioapic_id_0" = alloca i64, align 8
  %gsi_val4 = alloca i64, align 8
  store i64 %gsi_val, ptr %gsi_val4, align 8
  %addr_val3 = alloca i64, align 8
  store i64 %addr_val, ptr %addr_val3, align 8
  %id_val2 = alloca i64, align 8
  store i64 %id_val, ptr %id_val2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_126, label %merge_127

merge_127:                                        ; preds = %then_126, %entry
  %"%idx.load5" = load i64, ptr %idx1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%idx.load5", 1
  br i1 %cmpeqtmp6, label %then_128, label %merge_129

merge_129:                                        ; preds = %then_128, %merge_127
  %"%idx.load7" = load i64, ptr %idx1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%idx.load7", 2
  br i1 %cmpeqtmp8, label %then_130, label %merge_131

merge_131:                                        ; preds = %then_130, %merge_129
  %"%idx.load9" = load i64, ptr %idx1, align 8
  %cmpeqtmp10 = icmp eq i64 %"%idx.load9", 3
  br i1 %cmpeqtmp10, label %then_132, label %merge_133

merge_133:                                        ; preds = %then_132, %merge_131
  ret i64 0

then_126:                                         ; preds = %entry
  %"%344.load" = load i64, ptr %id_val2, align 8
  store i64 %"%344.load", ptr %"%ioapic_id_0", align 8
  %"%346.load" = load i64, ptr %addr_val3, align 8
  store i64 %"%346.load", ptr %"%ioapic_addr_0", align 8
  %"%348.load" = load i64, ptr %gsi_val4, align 8
  store i64 %"%348.load", ptr %"%ioapic_gsi_0", align 8
  br label %merge_127

then_128:                                         ; preds = %merge_127
  %"%352.load" = load i64, ptr %id_val2, align 8
  store i64 %"%352.load", ptr %"%ioapic_id_1", align 8
  %"%354.load" = load i64, ptr %addr_val3, align 8
  store i64 %"%354.load", ptr %"%ioapic_addr_1", align 8
  %"%356.load" = load i64, ptr %gsi_val4, align 8
  store i64 %"%356.load", ptr %"%ioapic_gsi_1", align 8
  br label %merge_129

then_130:                                         ; preds = %merge_129
  %"%360.load" = load i64, ptr %id_val2, align 8
  store i64 %"%360.load", ptr %"%ioapic_id_2", align 8
  %"%362.load" = load i64, ptr %addr_val3, align 8
  store i64 %"%362.load", ptr %"%ioapic_addr_2", align 8
  %"%364.load" = load i64, ptr %gsi_val4, align 8
  store i64 %"%364.load", ptr %"%ioapic_gsi_2", align 8
  br label %merge_131

then_132:                                         ; preds = %merge_131
  %"%368.load" = load i64, ptr %id_val2, align 8
  store i64 %"%368.load", ptr %"%ioapic_id_3", align 8
  %"%370.load" = load i64, ptr %addr_val3, align 8
  store i64 %"%370.load", ptr %"%ioapic_addr_3", align 8
  %"%372.load" = load i64, ptr %gsi_val4, align 8
  store i64 %"%372.load", ptr %"%ioapic_gsi_3", align 8
  br label %merge_133
}

define i64 @acpi_set_iso(i64 %idx, i64 %bus, i64 %source, i64 %gsi, i64 %flags) {
entry:
  %"%iso_flags_7" = alloca i64, align 8
  %"%iso_gsi_7" = alloca i64, align 8
  %"%iso_source_7" = alloca i64, align 8
  %"%iso_bus_7" = alloca i64, align 8
  %"%iso_flags_6" = alloca i64, align 8
  %"%iso_gsi_6" = alloca i64, align 8
  %"%iso_source_6" = alloca i64, align 8
  %"%iso_bus_6" = alloca i64, align 8
  %"%iso_flags_5" = alloca i64, align 8
  %"%iso_gsi_5" = alloca i64, align 8
  %"%iso_source_5" = alloca i64, align 8
  %"%iso_bus_5" = alloca i64, align 8
  %"%iso_flags_4" = alloca i64, align 8
  %"%iso_gsi_4" = alloca i64, align 8
  %"%iso_source_4" = alloca i64, align 8
  %"%iso_bus_4" = alloca i64, align 8
  %"%iso_flags_3" = alloca i64, align 8
  %"%iso_gsi_3" = alloca i64, align 8
  %"%iso_source_3" = alloca i64, align 8
  %"%iso_bus_3" = alloca i64, align 8
  %"%iso_flags_2" = alloca i64, align 8
  %"%iso_gsi_2" = alloca i64, align 8
  %"%iso_source_2" = alloca i64, align 8
  %"%iso_bus_2" = alloca i64, align 8
  %"%iso_flags_1" = alloca i64, align 8
  %"%iso_gsi_1" = alloca i64, align 8
  %"%iso_source_1" = alloca i64, align 8
  %"%iso_bus_1" = alloca i64, align 8
  %"%iso_flags_0" = alloca i64, align 8
  %"%iso_gsi_0" = alloca i64, align 8
  %"%iso_source_0" = alloca i64, align 8
  %"%iso_bus_0" = alloca i64, align 8
  %flags5 = alloca i64, align 8
  store i64 %flags, ptr %flags5, align 8
  %gsi4 = alloca i64, align 8
  store i64 %gsi, ptr %gsi4, align 8
  %source3 = alloca i64, align 8
  store i64 %source, ptr %source3, align 8
  %bus2 = alloca i64, align 8
  store i64 %bus, ptr %bus2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_134, label %merge_135

merge_135:                                        ; preds = %then_134, %entry
  %"%idx.load6" = load i64, ptr %idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%idx.load6", 1
  br i1 %cmpeqtmp7, label %then_136, label %merge_137

merge_137:                                        ; preds = %then_136, %merge_135
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 2
  br i1 %cmpeqtmp9, label %then_138, label %merge_139

merge_139:                                        ; preds = %then_138, %merge_137
  %"%idx.load10" = load i64, ptr %idx1, align 8
  %cmpeqtmp11 = icmp eq i64 %"%idx.load10", 3
  br i1 %cmpeqtmp11, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %merge_139
  %"%idx.load12" = load i64, ptr %idx1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%idx.load12", 4
  br i1 %cmpeqtmp13, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %merge_141
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 5
  br i1 %cmpeqtmp15, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%idx.load16" = load i64, ptr %idx1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%idx.load16", 6
  br i1 %cmpeqtmp17, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %merge_145
  %"%idx.load18" = load i64, ptr %idx1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%idx.load18", 7
  br i1 %cmpeqtmp19, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %merge_147
  ret i64 0

then_134:                                         ; preds = %entry
  %"%377.load" = load i64, ptr %bus2, align 8
  store i64 %"%377.load", ptr %"%iso_bus_0", align 8
  %"%379.load" = load i64, ptr %source3, align 8
  store i64 %"%379.load", ptr %"%iso_source_0", align 8
  %"%381.load" = load i64, ptr %gsi4, align 8
  store i64 %"%381.load", ptr %"%iso_gsi_0", align 8
  %"%383.load" = load i64, ptr %flags5, align 8
  store i64 %"%383.load", ptr %"%iso_flags_0", align 8
  br label %merge_135

then_136:                                         ; preds = %merge_135
  %"%387.load" = load i64, ptr %bus2, align 8
  store i64 %"%387.load", ptr %"%iso_bus_1", align 8
  %"%389.load" = load i64, ptr %source3, align 8
  store i64 %"%389.load", ptr %"%iso_source_1", align 8
  %"%391.load" = load i64, ptr %gsi4, align 8
  store i64 %"%391.load", ptr %"%iso_gsi_1", align 8
  %"%393.load" = load i64, ptr %flags5, align 8
  store i64 %"%393.load", ptr %"%iso_flags_1", align 8
  br label %merge_137

then_138:                                         ; preds = %merge_137
  %"%397.load" = load i64, ptr %bus2, align 8
  store i64 %"%397.load", ptr %"%iso_bus_2", align 8
  %"%399.load" = load i64, ptr %source3, align 8
  store i64 %"%399.load", ptr %"%iso_source_2", align 8
  %"%401.load" = load i64, ptr %gsi4, align 8
  store i64 %"%401.load", ptr %"%iso_gsi_2", align 8
  %"%403.load" = load i64, ptr %flags5, align 8
  store i64 %"%403.load", ptr %"%iso_flags_2", align 8
  br label %merge_139

then_140:                                         ; preds = %merge_139
  %"%407.load" = load i64, ptr %bus2, align 8
  store i64 %"%407.load", ptr %"%iso_bus_3", align 8
  %"%409.load" = load i64, ptr %source3, align 8
  store i64 %"%409.load", ptr %"%iso_source_3", align 8
  %"%411.load" = load i64, ptr %gsi4, align 8
  store i64 %"%411.load", ptr %"%iso_gsi_3", align 8
  %"%413.load" = load i64, ptr %flags5, align 8
  store i64 %"%413.load", ptr %"%iso_flags_3", align 8
  br label %merge_141

then_142:                                         ; preds = %merge_141
  %"%417.load" = load i64, ptr %bus2, align 8
  store i64 %"%417.load", ptr %"%iso_bus_4", align 8
  %"%419.load" = load i64, ptr %source3, align 8
  store i64 %"%419.load", ptr %"%iso_source_4", align 8
  %"%421.load" = load i64, ptr %gsi4, align 8
  store i64 %"%421.load", ptr %"%iso_gsi_4", align 8
  %"%423.load" = load i64, ptr %flags5, align 8
  store i64 %"%423.load", ptr %"%iso_flags_4", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%427.load" = load i64, ptr %bus2, align 8
  store i64 %"%427.load", ptr %"%iso_bus_5", align 8
  %"%429.load" = load i64, ptr %source3, align 8
  store i64 %"%429.load", ptr %"%iso_source_5", align 8
  %"%431.load" = load i64, ptr %gsi4, align 8
  store i64 %"%431.load", ptr %"%iso_gsi_5", align 8
  %"%433.load" = load i64, ptr %flags5, align 8
  store i64 %"%433.load", ptr %"%iso_flags_5", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  %"%437.load" = load i64, ptr %bus2, align 8
  store i64 %"%437.load", ptr %"%iso_bus_6", align 8
  %"%439.load" = load i64, ptr %source3, align 8
  store i64 %"%439.load", ptr %"%iso_source_6", align 8
  %"%441.load" = load i64, ptr %gsi4, align 8
  store i64 %"%441.load", ptr %"%iso_gsi_6", align 8
  %"%443.load" = load i64, ptr %flags5, align 8
  store i64 %"%443.load", ptr %"%iso_flags_6", align 8
  br label %merge_147

then_148:                                         ; preds = %merge_147
  %"%447.load" = load i64, ptr %bus2, align 8
  store i64 %"%447.load", ptr %"%iso_bus_7", align 8
  %"%449.load" = load i64, ptr %source3, align 8
  store i64 %"%449.load", ptr %"%iso_source_7", align 8
  %"%451.load" = load i64, ptr %gsi4, align 8
  store i64 %"%451.load", ptr %"%iso_gsi_7", align 8
  %"%453.load" = load i64, ptr %flags5, align 8
  store i64 %"%453.load", ptr %"%iso_flags_7", align 8
  br label %merge_149
}

define i64 @acpi_set_table(i64 %idx, i64 %addr, i64 %sig0) {
entry:
  %"%acpi_table_sig0_15" = alloca i64, align 8
  %"%acpi_table_sig0_14" = alloca i64, align 8
  %"%acpi_table_sig0_13" = alloca i64, align 8
  %"%acpi_table_sig0_12" = alloca i64, align 8
  %"%acpi_table_sig0_11" = alloca i64, align 8
  %"%acpi_table_sig0_10" = alloca i64, align 8
  %"%acpi_table_sig0_9" = alloca i64, align 8
  %"%acpi_table_sig0_8" = alloca i64, align 8
  %"%acpi_table_sig0_7" = alloca i64, align 8
  %"%acpi_table_sig0_6" = alloca i64, align 8
  %"%acpi_table_sig0_5" = alloca i64, align 8
  %"%acpi_table_sig0_4" = alloca i64, align 8
  %"%acpi_table_sig0_3" = alloca i64, align 8
  %"%acpi_table_sig0_2" = alloca i64, align 8
  %"%acpi_table_sig0_1" = alloca i64, align 8
  %"%acpi_table_sig0_0" = alloca i64, align 8
  %"%acpi_table_addr_15" = alloca i64, align 8
  %"%acpi_table_addr_14" = alloca i64, align 8
  %"%acpi_table_addr_13" = alloca i64, align 8
  %"%acpi_table_addr_12" = alloca i64, align 8
  %"%acpi_table_addr_11" = alloca i64, align 8
  %"%acpi_table_addr_10" = alloca i64, align 8
  %"%acpi_table_addr_9" = alloca i64, align 8
  %"%acpi_table_addr_8" = alloca i64, align 8
  %"%acpi_table_addr_7" = alloca i64, align 8
  %"%acpi_table_addr_6" = alloca i64, align 8
  %"%acpi_table_addr_5" = alloca i64, align 8
  %"%acpi_table_addr_4" = alloca i64, align 8
  %"%acpi_table_addr_3" = alloca i64, align 8
  %"%acpi_table_addr_2" = alloca i64, align 8
  %"%acpi_table_addr_1" = alloca i64, align 8
  %"%acpi_table_addr_0" = alloca i64, align 8
  %sig03 = alloca i64, align 8
  store i64 %sig0, ptr %sig03, align 8
  %addr2 = alloca i64, align 8
  store i64 %addr, ptr %addr2, align 8
  %idx1 = alloca i64, align 8
  store i64 %idx, ptr %idx1, align 8
  %"%idx.load" = load i64, ptr %idx1, align 8
  %cmpeqtmp = icmp eq i64 %"%idx.load", 0
  br i1 %cmpeqtmp, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %entry
  %"%idx.load4" = load i64, ptr %idx1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%idx.load4", 1
  br i1 %cmpeqtmp5, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %"%idx.load6" = load i64, ptr %idx1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%idx.load6", 2
  br i1 %cmpeqtmp7, label %then_154, label %merge_155

merge_155:                                        ; preds = %then_154, %merge_153
  %"%idx.load8" = load i64, ptr %idx1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%idx.load8", 3
  br i1 %cmpeqtmp9, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  %"%idx.load10" = load i64, ptr %idx1, align 8
  %cmpeqtmp11 = icmp eq i64 %"%idx.load10", 4
  br i1 %cmpeqtmp11, label %then_158, label %merge_159

merge_159:                                        ; preds = %then_158, %merge_157
  %"%idx.load12" = load i64, ptr %idx1, align 8
  %cmpeqtmp13 = icmp eq i64 %"%idx.load12", 5
  br i1 %cmpeqtmp13, label %then_160, label %merge_161

merge_161:                                        ; preds = %then_160, %merge_159
  %"%idx.load14" = load i64, ptr %idx1, align 8
  %cmpeqtmp15 = icmp eq i64 %"%idx.load14", 6
  br i1 %cmpeqtmp15, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %merge_161
  %"%idx.load16" = load i64, ptr %idx1, align 8
  %cmpeqtmp17 = icmp eq i64 %"%idx.load16", 7
  br i1 %cmpeqtmp17, label %then_164, label %merge_165

merge_165:                                        ; preds = %then_164, %merge_163
  %"%idx.load18" = load i64, ptr %idx1, align 8
  %cmpeqtmp19 = icmp eq i64 %"%idx.load18", 8
  br i1 %cmpeqtmp19, label %then_166, label %merge_167

merge_167:                                        ; preds = %then_166, %merge_165
  %"%idx.load20" = load i64, ptr %idx1, align 8
  %cmpeqtmp21 = icmp eq i64 %"%idx.load20", 9
  br i1 %cmpeqtmp21, label %then_168, label %merge_169

merge_169:                                        ; preds = %then_168, %merge_167
  %"%idx.load22" = load i64, ptr %idx1, align 8
  %cmpeqtmp23 = icmp eq i64 %"%idx.load22", 10
  br i1 %cmpeqtmp23, label %then_170, label %merge_171

merge_171:                                        ; preds = %then_170, %merge_169
  %"%idx.load24" = load i64, ptr %idx1, align 8
  %cmpeqtmp25 = icmp eq i64 %"%idx.load24", 11
  br i1 %cmpeqtmp25, label %then_172, label %merge_173

merge_173:                                        ; preds = %then_172, %merge_171
  %"%idx.load26" = load i64, ptr %idx1, align 8
  %cmpeqtmp27 = icmp eq i64 %"%idx.load26", 12
  br i1 %cmpeqtmp27, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %merge_173
  %"%idx.load28" = load i64, ptr %idx1, align 8
  %cmpeqtmp29 = icmp eq i64 %"%idx.load28", 13
  br i1 %cmpeqtmp29, label %then_176, label %merge_177

merge_177:                                        ; preds = %then_176, %merge_175
  %"%idx.load30" = load i64, ptr %idx1, align 8
  %cmpeqtmp31 = icmp eq i64 %"%idx.load30", 14
  br i1 %cmpeqtmp31, label %then_178, label %merge_179

merge_179:                                        ; preds = %then_178, %merge_177
  %"%idx.load32" = load i64, ptr %idx1, align 8
  %cmpeqtmp33 = icmp eq i64 %"%idx.load32", 15
  br i1 %cmpeqtmp33, label %then_180, label %merge_181

merge_181:                                        ; preds = %then_180, %merge_179
  %"%idx.load34" = load i64, ptr %idx1, align 8
  %cmpeqtmp35 = icmp eq i64 %"%idx.load34", 0
  br i1 %cmpeqtmp35, label %then_182, label %merge_183

merge_183:                                        ; preds = %then_182, %merge_181
  %"%idx.load36" = load i64, ptr %idx1, align 8
  %cmpeqtmp37 = icmp eq i64 %"%idx.load36", 1
  br i1 %cmpeqtmp37, label %then_184, label %merge_185

merge_185:                                        ; preds = %then_184, %merge_183
  %"%idx.load38" = load i64, ptr %idx1, align 8
  %cmpeqtmp39 = icmp eq i64 %"%idx.load38", 2
  br i1 %cmpeqtmp39, label %then_186, label %merge_187

merge_187:                                        ; preds = %then_186, %merge_185
  %"%idx.load40" = load i64, ptr %idx1, align 8
  %cmpeqtmp41 = icmp eq i64 %"%idx.load40", 3
  br i1 %cmpeqtmp41, label %then_188, label %merge_189

merge_189:                                        ; preds = %then_188, %merge_187
  %"%idx.load42" = load i64, ptr %idx1, align 8
  %cmpeqtmp43 = icmp eq i64 %"%idx.load42", 4
  br i1 %cmpeqtmp43, label %then_190, label %merge_191

merge_191:                                        ; preds = %then_190, %merge_189
  %"%idx.load44" = load i64, ptr %idx1, align 8
  %cmpeqtmp45 = icmp eq i64 %"%idx.load44", 5
  br i1 %cmpeqtmp45, label %then_192, label %merge_193

merge_193:                                        ; preds = %then_192, %merge_191
  %"%idx.load46" = load i64, ptr %idx1, align 8
  %cmpeqtmp47 = icmp eq i64 %"%idx.load46", 6
  br i1 %cmpeqtmp47, label %then_194, label %merge_195

merge_195:                                        ; preds = %then_194, %merge_193
  %"%idx.load48" = load i64, ptr %idx1, align 8
  %cmpeqtmp49 = icmp eq i64 %"%idx.load48", 7
  br i1 %cmpeqtmp49, label %then_196, label %merge_197

merge_197:                                        ; preds = %then_196, %merge_195
  %"%idx.load50" = load i64, ptr %idx1, align 8
  %cmpeqtmp51 = icmp eq i64 %"%idx.load50", 8
  br i1 %cmpeqtmp51, label %then_198, label %merge_199

merge_199:                                        ; preds = %then_198, %merge_197
  %"%idx.load52" = load i64, ptr %idx1, align 8
  %cmpeqtmp53 = icmp eq i64 %"%idx.load52", 9
  br i1 %cmpeqtmp53, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %merge_199
  %"%idx.load54" = load i64, ptr %idx1, align 8
  %cmpeqtmp55 = icmp eq i64 %"%idx.load54", 10
  br i1 %cmpeqtmp55, label %then_202, label %merge_203

merge_203:                                        ; preds = %then_202, %merge_201
  %"%idx.load56" = load i64, ptr %idx1, align 8
  %cmpeqtmp57 = icmp eq i64 %"%idx.load56", 11
  br i1 %cmpeqtmp57, label %then_204, label %merge_205

merge_205:                                        ; preds = %then_204, %merge_203
  %"%idx.load58" = load i64, ptr %idx1, align 8
  %cmpeqtmp59 = icmp eq i64 %"%idx.load58", 12
  br i1 %cmpeqtmp59, label %then_206, label %merge_207

merge_207:                                        ; preds = %then_206, %merge_205
  %"%idx.load60" = load i64, ptr %idx1, align 8
  %cmpeqtmp61 = icmp eq i64 %"%idx.load60", 13
  br i1 %cmpeqtmp61, label %then_208, label %merge_209

merge_209:                                        ; preds = %then_208, %merge_207
  %"%idx.load62" = load i64, ptr %idx1, align 8
  %cmpeqtmp63 = icmp eq i64 %"%idx.load62", 14
  br i1 %cmpeqtmp63, label %then_210, label %merge_211

merge_211:                                        ; preds = %then_210, %merge_209
  %"%idx.load64" = load i64, ptr %idx1, align 8
  %cmpeqtmp65 = icmp eq i64 %"%idx.load64", 15
  br i1 %cmpeqtmp65, label %then_212, label %merge_213

merge_213:                                        ; preds = %then_212, %merge_211
  ret i64 0

then_150:                                         ; preds = %entry
  %"%458.load" = load i64, ptr %addr2, align 8
  store i64 %"%458.load", ptr %"%acpi_table_addr_0", align 8
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %"%462.load" = load i64, ptr %addr2, align 8
  store i64 %"%462.load", ptr %"%acpi_table_addr_1", align 8
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %"%466.load" = load i64, ptr %addr2, align 8
  store i64 %"%466.load", ptr %"%acpi_table_addr_2", align 8
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %"%470.load" = load i64, ptr %addr2, align 8
  store i64 %"%470.load", ptr %"%acpi_table_addr_3", align 8
  br label %merge_157

then_158:                                         ; preds = %merge_157
  %"%474.load" = load i64, ptr %addr2, align 8
  store i64 %"%474.load", ptr %"%acpi_table_addr_4", align 8
  br label %merge_159

then_160:                                         ; preds = %merge_159
  %"%478.load" = load i64, ptr %addr2, align 8
  store i64 %"%478.load", ptr %"%acpi_table_addr_5", align 8
  br label %merge_161

then_162:                                         ; preds = %merge_161
  %"%482.load" = load i64, ptr %addr2, align 8
  store i64 %"%482.load", ptr %"%acpi_table_addr_6", align 8
  br label %merge_163

then_164:                                         ; preds = %merge_163
  %"%486.load" = load i64, ptr %addr2, align 8
  store i64 %"%486.load", ptr %"%acpi_table_addr_7", align 8
  br label %merge_165

then_166:                                         ; preds = %merge_165
  %"%490.load" = load i64, ptr %addr2, align 8
  store i64 %"%490.load", ptr %"%acpi_table_addr_8", align 8
  br label %merge_167

then_168:                                         ; preds = %merge_167
  %"%494.load" = load i64, ptr %addr2, align 8
  store i64 %"%494.load", ptr %"%acpi_table_addr_9", align 8
  br label %merge_169

then_170:                                         ; preds = %merge_169
  %"%498.load" = load i64, ptr %addr2, align 8
  store i64 %"%498.load", ptr %"%acpi_table_addr_10", align 8
  br label %merge_171

then_172:                                         ; preds = %merge_171
  %"%502.load" = load i64, ptr %addr2, align 8
  store i64 %"%502.load", ptr %"%acpi_table_addr_11", align 8
  br label %merge_173

then_174:                                         ; preds = %merge_173
  %"%506.load" = load i64, ptr %addr2, align 8
  store i64 %"%506.load", ptr %"%acpi_table_addr_12", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  %"%510.load" = load i64, ptr %addr2, align 8
  store i64 %"%510.load", ptr %"%acpi_table_addr_13", align 8
  br label %merge_177

then_178:                                         ; preds = %merge_177
  %"%514.load" = load i64, ptr %addr2, align 8
  store i64 %"%514.load", ptr %"%acpi_table_addr_14", align 8
  br label %merge_179

then_180:                                         ; preds = %merge_179
  %"%518.load" = load i64, ptr %addr2, align 8
  store i64 %"%518.load", ptr %"%acpi_table_addr_15", align 8
  br label %merge_181

then_182:                                         ; preds = %merge_181
  %"%522.load" = load i64, ptr %sig03, align 8
  store i64 %"%522.load", ptr %"%acpi_table_sig0_0", align 8
  br label %merge_183

then_184:                                         ; preds = %merge_183
  %"%526.load" = load i64, ptr %sig03, align 8
  store i64 %"%526.load", ptr %"%acpi_table_sig0_1", align 8
  br label %merge_185

then_186:                                         ; preds = %merge_185
  %"%530.load" = load i64, ptr %sig03, align 8
  store i64 %"%530.load", ptr %"%acpi_table_sig0_2", align 8
  br label %merge_187

then_188:                                         ; preds = %merge_187
  %"%534.load" = load i64, ptr %sig03, align 8
  store i64 %"%534.load", ptr %"%acpi_table_sig0_3", align 8
  br label %merge_189

then_190:                                         ; preds = %merge_189
  %"%538.load" = load i64, ptr %sig03, align 8
  store i64 %"%538.load", ptr %"%acpi_table_sig0_4", align 8
  br label %merge_191

then_192:                                         ; preds = %merge_191
  %"%542.load" = load i64, ptr %sig03, align 8
  store i64 %"%542.load", ptr %"%acpi_table_sig0_5", align 8
  br label %merge_193

then_194:                                         ; preds = %merge_193
  %"%546.load" = load i64, ptr %sig03, align 8
  store i64 %"%546.load", ptr %"%acpi_table_sig0_6", align 8
  br label %merge_195

then_196:                                         ; preds = %merge_195
  %"%550.load" = load i64, ptr %sig03, align 8
  store i64 %"%550.load", ptr %"%acpi_table_sig0_7", align 8
  br label %merge_197

then_198:                                         ; preds = %merge_197
  %"%554.load" = load i64, ptr %sig03, align 8
  store i64 %"%554.load", ptr %"%acpi_table_sig0_8", align 8
  br label %merge_199

then_200:                                         ; preds = %merge_199
  %"%558.load" = load i64, ptr %sig03, align 8
  store i64 %"%558.load", ptr %"%acpi_table_sig0_9", align 8
  br label %merge_201

then_202:                                         ; preds = %merge_201
  %"%562.load" = load i64, ptr %sig03, align 8
  store i64 %"%562.load", ptr %"%acpi_table_sig0_10", align 8
  br label %merge_203

then_204:                                         ; preds = %merge_203
  %"%566.load" = load i64, ptr %sig03, align 8
  store i64 %"%566.load", ptr %"%acpi_table_sig0_11", align 8
  br label %merge_205

then_206:                                         ; preds = %merge_205
  %"%570.load" = load i64, ptr %sig03, align 8
  store i64 %"%570.load", ptr %"%acpi_table_sig0_12", align 8
  br label %merge_207

then_208:                                         ; preds = %merge_207
  %"%574.load" = load i64, ptr %sig03, align 8
  store i64 %"%574.load", ptr %"%acpi_table_sig0_13", align 8
  br label %merge_209

then_210:                                         ; preds = %merge_209
  %"%578.load" = load i64, ptr %sig03, align 8
  store i64 %"%578.load", ptr %"%acpi_table_sig0_14", align 8
  br label %merge_211

then_212:                                         ; preds = %merge_211
  %"%582.load" = load i64, ptr %sig03, align 8
  store i64 %"%582.load", ptr %"%acpi_table_sig0_15", align 8
  br label %merge_213
}

define i64 @acpi_verify_checksum(i64 %addr, i64 %length) {
entry:
  %"%i" = alloca i64, align 8
  %"%sum" = alloca i64, align 8
  %length2 = alloca i64, align 8
  store i64 %length, ptr %length2, align 8
  %addr1 = alloca i64, align 8
  store i64 %addr, ptr %addr1, align 8
  store i64 0, ptr %"%sum", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_214

while_body_215:                                   ; preds = %while_cond_214
  %"%addr.load" = load i64, ptr %addr1, align 8
  %"%i.load3" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%addr.load", %"%i.load3"
  call void @sad_ll_mem_read8(i64 %addtmp)
  %"%sum.load" = load i64, ptr %"%sum", align 8
  %addtmp4 = add i64 %"%sum.load", 0
  store i64 %addtmp4, ptr %"%sum", align 8
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp6 = add i64 %"%i.load5", 1
  store i64 %addtmp6, ptr %"%i", align 8
  br label %while_cond_214

while_cond_214:                                   ; preds = %while_body_215, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%length.load" = load i64, ptr %length2, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%length.load"
  br i1 %cmplttmp, label %while_body_215, label %while_exit_216

while_exit_216:                                   ; preds = %while_cond_214
  %"%sum.load7" = load i64, ptr %"%sum", align 8
  %andtmp = and i64 %"%sum.load7", 255
  ret i64 %andtmp
}

define i64 @acpi_find_rsdp() {
entry:
  %"%cs" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%603.load" = load i64, ptr @RSDP_BIOS_START, align 8
  store i64 %"%603.load", ptr %"%addr", align 8
  br label %while_cond_217

merge_221:                                        ; preds = %merge_223, %while_body_218
  %"%addr.load7" = load i64, ptr %"%addr", align 8
  %addtmp8 = add i64 %"%addr.load7", 16
  store i64 %addtmp8, ptr %"%addr", align 8
  br label %while_cond_217

merge_223:                                        ; preds = %merge_225, %then_220
  br label %merge_221

merge_225:                                        ; preds = %merge_227, %then_222
  br label %merge_223

merge_227:                                        ; preds = %merge_229, %then_224
  br label %merge_225

merge_229:                                        ; preds = %merge_231, %then_226
  br label %merge_227

merge_231:                                        ; preds = %merge_233, %then_228
  br label %merge_229

merge_233:                                        ; preds = %then_230
  br label %merge_231

then_220:                                         ; preds = %while_body_218
  %"%addr.load5" = load i64, ptr %"%addr", align 8
  %addtmp = add i64 %"%addr.load5", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  %"%RSDP_SIG_1.load" = load i64, ptr @RSDP_SIG_1, align 8
  %cmpeqtmp6 = icmp eq i64 0, %"%RSDP_SIG_1.load"
  br i1 %cmpeqtmp6, label %then_222, label %merge_223

then_222:                                         ; preds = %then_220
  %"%addr.load9" = load i64, ptr %"%addr", align 8
  %addtmp10 = add i64 %"%addr.load9", 2
  call void @sad_ll_mem_read8(i64 %addtmp10)
  %"%RSDP_SIG_2.load" = load i64, ptr @RSDP_SIG_2, align 8
  %cmpeqtmp11 = icmp eq i64 0, %"%RSDP_SIG_2.load"
  br i1 %cmpeqtmp11, label %then_224, label %merge_225

then_224:                                         ; preds = %then_222
  %"%addr.load12" = load i64, ptr %"%addr", align 8
  %addtmp13 = add i64 %"%addr.load12", 3
  call void @sad_ll_mem_read8(i64 %addtmp13)
  %"%RSDP_SIG_3.load" = load i64, ptr @RSDP_SIG_3, align 8
  %cmpeqtmp14 = icmp eq i64 0, %"%RSDP_SIG_3.load"
  br i1 %cmpeqtmp14, label %then_226, label %merge_227

then_226:                                         ; preds = %then_224
  %"%addr.load15" = load i64, ptr %"%addr", align 8
  %addtmp16 = add i64 %"%addr.load15", 4
  call void @sad_ll_mem_read8(i64 %addtmp16)
  %"%RSDP_SIG_4.load" = load i64, ptr @RSDP_SIG_4, align 8
  %cmpeqtmp17 = icmp eq i64 0, %"%RSDP_SIG_4.load"
  br i1 %cmpeqtmp17, label %then_228, label %merge_229

then_228:                                         ; preds = %then_226
  %"%addr.load18" = load i64, ptr %"%addr", align 8
  %addtmp19 = add i64 %"%addr.load18", 5
  call void @sad_ll_mem_read8(i64 %addtmp19)
  %"%RSDP_SIG_5.load" = load i64, ptr @RSDP_SIG_5, align 8
  %cmpeqtmp20 = icmp eq i64 0, %"%RSDP_SIG_5.load"
  br i1 %cmpeqtmp20, label %then_230, label %merge_231

then_230:                                         ; preds = %then_228
  %"%addr.load21" = load i64, ptr %"%addr", align 8
  %calltmp22 = call i64 @acpi_verify_checksum(i64 %"%addr.load21", i64 20)
  store i64 %calltmp22, ptr %"%cs", align 8
  %"%cs.load" = load i64, ptr %"%cs", align 8
  %cmpeqtmp23 = icmp eq i64 %"%cs.load", 0
  br i1 %cmpeqtmp23, label %then_232, label %merge_233

then_232:                                         ; preds = %then_230
  %calltmp24 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%addr.load25" = load i64, ptr %"%addr", align 8
  %calltmp26 = call i64 @acpi_serial_hex32(i64 %"%addr.load25")
  %calltmp27 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr %"%addr", align 8
  ret i64 %loadtmp

while_body_218:                                   ; preds = %while_cond_217
  %"%addr.load2" = load i64, ptr %"%addr", align 8
  call void @sad_ll_mem_read8(i64 %"%addr.load2")
  %"%RSDP_SIG_0.load" = load i64, ptr @RSDP_SIG_0, align 8
  %cmpeqtmp = icmp eq i64 0, %"%RSDP_SIG_0.load"
  br i1 %cmpeqtmp, label %then_220, label %merge_221

while_cond_217:                                   ; preds = %merge_221, %entry
  %"%addr.load" = load i64, ptr %"%addr", align 8
  %"%RSDP_BIOS_END.load" = load i64, ptr @RSDP_BIOS_END, align 8
  %cmplttmp = icmp slt i64 %"%addr.load", %"%RSDP_BIOS_END.load"
  br i1 %cmplttmp, label %while_body_218, label %while_exit_219

while_exit_219:                                   ; preds = %while_cond_217
  %calltmp3 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp4 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @acpi_parse_rsdp(i64 %rsdp_addr) {
entry:
  %"%acpi_xsdt_addr" = alloca i64, align 8
  %"%acpi_rsdt_addr" = alloca i64, align 8
  %"%acpi_revision" = alloca i64, align 8
  %"%acpi_rsdp_addr" = alloca i64, align 8
  %rsdp_addr1 = alloca i64, align 8
  store i64 %rsdp_addr, ptr %rsdp_addr1, align 8
  %"%647.load" = load i64, ptr %rsdp_addr1, align 8
  store i64 %"%647.load", ptr %"%acpi_rsdp_addr", align 8
  %"%rsdp_addr.load" = load i64, ptr %rsdp_addr1, align 8
  %addtmp = add i64 %"%rsdp_addr.load", 15
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%acpi_revision", align 8
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%656.load" = load i64, ptr %"%acpi_revision", align 8
  %calltmp2 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%656.load")
  %calltmp3 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%rsdp_addr.load4" = load i64, ptr %rsdp_addr1, align 8
  %addtmp5 = add i64 %"%rsdp_addr.load4", 16
  call void @sad_ll_mem_read32(i64 %addtmp5)
  store i64 0, ptr %"%acpi_rsdt_addr", align 8
  %calltmp6 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%acpi_rsdt_addr.load" = load i64, ptr %"%acpi_rsdt_addr", align 8
  %calltmp7 = call i64 @acpi_serial_hex32(i64 %"%acpi_rsdt_addr.load")
  %calltmp8 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_revision.load" = load i64, ptr %"%acpi_revision", align 8
  %cmpgetmp = icmp sge i64 %"%acpi_revision.load", 2
  br i1 %cmpgetmp, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %entry
  ret i64 0

then_234:                                         ; preds = %entry
  %"%rsdp_addr.load9" = load i64, ptr %rsdp_addr1, align 8
  %addtmp10 = add i64 %"%rsdp_addr.load9", 24
  call void @sad_ll_mem_read32(i64 %addtmp10)
  store i64 0, ptr %"%acpi_xsdt_addr", align 8
  %calltmp11 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%acpi_xsdt_addr.load" = load i64, ptr %"%acpi_xsdt_addr", align 8
  %calltmp12 = call i64 @acpi_serial_hex32(i64 %"%acpi_xsdt_addr.load")
  %calltmp13 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_235
}

define i64 @acpi_match_sig(i64 %addr, i64 %s0, i64 %s1, i64 %s2, i64 %s3) {
entry:
  %s35 = alloca i64, align 8
  store i64 %s3, ptr %s35, align 8
  %s24 = alloca i64, align 8
  store i64 %s2, ptr %s24, align 8
  %s13 = alloca i64, align 8
  store i64 %s1, ptr %s13, align 8
  %s02 = alloca i64, align 8
  store i64 %s0, ptr %s02, align 8
  %addr1 = alloca i64, align 8
  store i64 %addr, ptr %addr1, align 8
  %"%addr.load" = load i64, ptr %addr1, align 8
  call void @sad_ll_mem_read8(i64 %"%addr.load")
  %"%s0.load" = load i64, ptr %s02, align 8
  %cmpnetmp = icmp ne i64 0, %"%s0.load"
  br i1 %cmpnetmp, label %then_236, label %merge_237

merge_237:                                        ; preds = %entry
  %"%addr.load6" = load i64, ptr %addr1, align 8
  %addtmp = add i64 %"%addr.load6", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  %"%s1.load" = load i64, ptr %s13, align 8
  %cmpnetmp7 = icmp ne i64 0, %"%s1.load"
  br i1 %cmpnetmp7, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_237
  %"%addr.load8" = load i64, ptr %addr1, align 8
  %addtmp9 = add i64 %"%addr.load8", 2
  call void @sad_ll_mem_read8(i64 %addtmp9)
  %"%s2.load" = load i64, ptr %s24, align 8
  %cmpnetmp10 = icmp ne i64 0, %"%s2.load"
  br i1 %cmpnetmp10, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  %"%addr.load11" = load i64, ptr %addr1, align 8
  %addtmp12 = add i64 %"%addr.load11", 3
  call void @sad_ll_mem_read8(i64 %addtmp12)
  %"%s3.load" = load i64, ptr %s35, align 8
  %cmpnetmp13 = icmp ne i64 0, %"%s3.load"
  br i1 %cmpnetmp13, label %then_242, label %merge_243

merge_243:                                        ; preds = %merge_241
  ret i64 1

then_236:                                         ; preds = %entry
  ret i64 0

then_238:                                         ; preds = %merge_237
  ret i64 0

then_240:                                         ; preds = %merge_239
  ret i64 0

then_242:                                         ; preds = %merge_241
  ret i64 0
}

define i64 @acpi_parse_rsdt() {
entry:
  %"%acpi_mcfg_addr" = alloca i64, align 8
  %"%acpi_hpet_addr" = alloca i64, align 8
  %"%acpi_madt_addr" = alloca i64, align 8
  %"%acpi_fadt_addr" = alloca i64, align 8
  %"%acpi_table_count" = alloca i64, align 8
  %"%sig0" = alloca i64, align 8
  %"%table_addr" = alloca i64, align 8
  %"%ptr" = alloca i64, align 8
  %"%entries_end" = alloca i64, align 8
  %"%entries_start" = alloca i64, align 8
  %"%length" = alloca i64, align 8
  %"%acpi_errors" = alloca i64, align 8
  %"%acpi_rsdt_addr.load" = load i64, ptr @acpi_rsdt_addr, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_rsdt_addr.load", 0
  br i1 %cmpeqtmp, label %then_244, label %merge_245

merge_245:                                        ; preds = %entry
  %"%acpi_rsdt_addr.load1" = load i64, ptr @acpi_rsdt_addr, align 8
  %"%SIG_RSDT_0.load" = load i64, ptr @SIG_RSDT_0, align 8
  %"%SIG_RSDT_1.load" = load i64, ptr @SIG_RSDT_1, align 8
  %"%SIG_RSDT_2.load" = load i64, ptr @SIG_RSDT_2, align 8
  %"%SIG_RSDT_3.load" = load i64, ptr @SIG_RSDT_3, align 8
  %calltmp = call i64 @acpi_match_sig(i64 %"%acpi_rsdt_addr.load1", i64 %"%SIG_RSDT_0.load", i64 %"%SIG_RSDT_1.load", i64 %"%SIG_RSDT_2.load", i64 %"%SIG_RSDT_3.load")
  %cmpeqtmp2 = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp2, label %then_246, label %merge_247

merge_247:                                        ; preds = %merge_245
  %"%acpi_rsdt_addr.load5" = load i64, ptr @acpi_rsdt_addr, align 8
  %addtmp6 = add i64 %"%acpi_rsdt_addr.load5", 4
  call void @sad_ll_mem_read32(i64 %addtmp6)
  store i64 0, ptr %"%length", align 8
  %calltmp7 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%725.load" = load i64, ptr %"%length", align 8
  %calltmp8 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%725.load")
  %calltmp9 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_rsdt_addr.load10" = load i64, ptr @acpi_rsdt_addr, align 8
  %addtmp11 = add i64 %"%acpi_rsdt_addr.load10", 36
  store i64 %addtmp11, ptr %"%entries_start", align 8
  %"%acpi_rsdt_addr.load12" = load i64, ptr @acpi_rsdt_addr, align 8
  %"%length.load" = load i64, ptr %"%length", align 8
  %addtmp13 = add i64 %"%acpi_rsdt_addr.load12", %"%length.load"
  store i64 %addtmp13, ptr %"%entries_end", align 8
  %"%732.load" = load i64, ptr %"%entries_start", align 8
  store i64 %"%732.load", ptr %"%ptr", align 8
  br label %while_cond_248

merge_252:                                        ; preds = %then_251, %while_body_249
  %"%ptr.load18" = load i64, ptr %"%ptr", align 8
  call void @sad_ll_mem_read32(i64 %"%ptr.load18")
  store i64 0, ptr %"%table_addr", align 8
  %"%table_addr.load" = load i64, ptr %"%table_addr", align 8
  %cmpgttmp = icmp sgt i64 %"%table_addr.load", 0
  br i1 %cmpgttmp, label %then_253, label %merge_254

merge_254:                                        ; preds = %merge_262, %merge_252
  %"%ptr.load35" = load i64, ptr %"%ptr", align 8
  %addtmp36 = add i64 %"%ptr.load35", 4
  store i64 %addtmp36, ptr %"%ptr", align 8
  br label %while_cond_248

merge_256:                                        ; preds = %then_255, %then_253
  %"%table_addr.load37" = load i64, ptr %"%table_addr", align 8
  %"%SIG_APIC_0.load" = load i64, ptr @SIG_APIC_0, align 8
  %"%SIG_APIC_1.load" = load i64, ptr @SIG_APIC_1, align 8
  %"%SIG_APIC_2.load" = load i64, ptr @SIG_APIC_2, align 8
  %"%SIG_APIC_3.load" = load i64, ptr @SIG_APIC_3, align 8
  %calltmp38 = call i64 @acpi_match_sig(i64 %"%table_addr.load37", i64 %"%SIG_APIC_0.load", i64 %"%SIG_APIC_1.load", i64 %"%SIG_APIC_2.load", i64 %"%SIG_APIC_3.load")
  %cmpeqtmp39 = icmp eq i64 %calltmp38, 1
  br i1 %cmpeqtmp39, label %then_257, label %merge_258

merge_258:                                        ; preds = %then_257, %merge_256
  %"%table_addr.load40" = load i64, ptr %"%table_addr", align 8
  %"%SIG_HPET_0.load" = load i64, ptr @SIG_HPET_0, align 8
  %"%SIG_HPET_1.load" = load i64, ptr @SIG_HPET_1, align 8
  %"%SIG_HPET_2.load" = load i64, ptr @SIG_HPET_2, align 8
  %"%SIG_HPET_3.load" = load i64, ptr @SIG_HPET_3, align 8
  %calltmp41 = call i64 @acpi_match_sig(i64 %"%table_addr.load40", i64 %"%SIG_HPET_0.load", i64 %"%SIG_HPET_1.load", i64 %"%SIG_HPET_2.load", i64 %"%SIG_HPET_3.load")
  %cmpeqtmp42 = icmp eq i64 %calltmp41, 1
  br i1 %cmpeqtmp42, label %then_259, label %merge_260

merge_260:                                        ; preds = %then_259, %merge_258
  %"%table_addr.load43" = load i64, ptr %"%table_addr", align 8
  %"%SIG_MCFG_0.load" = load i64, ptr @SIG_MCFG_0, align 8
  %"%SIG_MCFG_1.load" = load i64, ptr @SIG_MCFG_1, align 8
  %"%SIG_MCFG_2.load" = load i64, ptr @SIG_MCFG_2, align 8
  %"%SIG_MCFG_3.load" = load i64, ptr @SIG_MCFG_3, align 8
  %calltmp44 = call i64 @acpi_match_sig(i64 %"%table_addr.load43", i64 %"%SIG_MCFG_0.load", i64 %"%SIG_MCFG_1.load", i64 %"%SIG_MCFG_2.load", i64 %"%SIG_MCFG_3.load")
  %cmpeqtmp45 = icmp eq i64 %calltmp44, 1
  br i1 %cmpeqtmp45, label %then_261, label %merge_262

merge_262:                                        ; preds = %then_261, %merge_260
  br label %merge_254

then_244:                                         ; preds = %entry
  ret i64 -1

then_246:                                         ; preds = %merge_245
  %calltmp3 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %calltmp4 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_errors.load" = load i64, ptr @acpi_errors, align 8
  %addtmp = add i64 %"%acpi_errors.load", 1
  store i64 %addtmp, ptr %"%acpi_errors", align 8
  ret i64 -1

then_251:                                         ; preds = %while_body_249
  br label %merge_252

then_253:                                         ; preds = %merge_252
  %"%table_addr.load19" = load i64, ptr %"%table_addr", align 8
  call void @sad_ll_mem_read8(i64 %"%table_addr.load19")
  store i64 0, ptr %"%sig0", align 8
  %calltmp20 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%table_addr.load21" = load i64, ptr %"%table_addr", align 8
  %calltmp22 = call i64 @acpi_serial_sig(i64 %"%table_addr.load21")
  %calltmp23 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%table_addr.load24" = load i64, ptr %"%table_addr", align 8
  %calltmp25 = call i64 @acpi_serial_hex32(i64 %"%table_addr.load24")
  %calltmp26 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_table_count.load27" = load i64, ptr @acpi_table_count, align 8
  %"%table_addr.load28" = load i64, ptr %"%table_addr", align 8
  %"%sig0.load" = load i64, ptr %"%sig0", align 8
  %calltmp29 = call i64 @acpi_set_table(i64 %"%acpi_table_count.load27", i64 %"%table_addr.load28", i64 %"%sig0.load")
  %"%acpi_table_count.load30" = load i64, ptr @acpi_table_count, align 8
  %addtmp31 = add i64 %"%acpi_table_count.load30", 1
  store i64 %addtmp31, ptr %"%acpi_table_count", align 8
  %"%table_addr.load32" = load i64, ptr %"%table_addr", align 8
  %"%SIG_FACP_0.load" = load i64, ptr @SIG_FACP_0, align 8
  %"%SIG_FACP_1.load" = load i64, ptr @SIG_FACP_1, align 8
  %"%SIG_FACP_2.load" = load i64, ptr @SIG_FACP_2, align 8
  %"%SIG_FACP_3.load" = load i64, ptr @SIG_FACP_3, align 8
  %calltmp33 = call i64 @acpi_match_sig(i64 %"%table_addr.load32", i64 %"%SIG_FACP_0.load", i64 %"%SIG_FACP_1.load", i64 %"%SIG_FACP_2.load", i64 %"%SIG_FACP_3.load")
  %cmpeqtmp34 = icmp eq i64 %calltmp33, 1
  br i1 %cmpeqtmp34, label %then_255, label %merge_256

then_255:                                         ; preds = %then_253
  %"%758.load" = load i64, ptr %"%table_addr", align 8
  store i64 %"%758.load", ptr %"%acpi_fadt_addr", align 8
  br label %merge_256

then_257:                                         ; preds = %merge_256
  %"%763.load" = load i64, ptr %"%table_addr", align 8
  store i64 %"%763.load", ptr %"%acpi_madt_addr", align 8
  br label %merge_258

then_259:                                         ; preds = %merge_258
  %"%768.load" = load i64, ptr %"%table_addr", align 8
  store i64 %"%768.load", ptr %"%acpi_hpet_addr", align 8
  br label %merge_260

then_261:                                         ; preds = %merge_260
  %"%773.load" = load i64, ptr %"%table_addr", align 8
  store i64 %"%773.load", ptr %"%acpi_mcfg_addr", align 8
  br label %merge_262

while_body_249:                                   ; preds = %while_cond_248
  %"%acpi_table_count.load" = load i64, ptr @acpi_table_count, align 8
  %cmpgetmp = icmp sge i64 %"%acpi_table_count.load", 16
  br i1 %cmpgetmp, label %then_251, label %merge_252

while_cond_248:                                   ; preds = %merge_254, %merge_247
  %"%ptr.load" = load i64, ptr %"%ptr", align 8
  %"%entries_end.load" = load i64, ptr %"%entries_end", align 8
  %cmplttmp = icmp slt i64 %"%ptr.load", %"%entries_end.load"
  br i1 %cmplttmp, label %while_body_249, label %while_exit_250

while_exit_250:                                   ; preds = %while_cond_248
  %calltmp14 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%779.load" = load i64, ptr @acpi_table_count, align 8
  %calltmp15 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%779.load")
  %calltmp16 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp17 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr @acpi_table_count, align 8
  ret i64 %loadtmp
}

define i64 @acpi_parse_fadt() {
entry:
  %"%acpi_pm1b_ctl" = alloca i64, align 8
  %"%acpi_pm1a_ctl" = alloca i64, align 8
  %"%acpi_pm1a_evt" = alloca i64, align 8
  %"%acpi_disable_val" = alloca i64, align 8
  %"%acpi_enable_val" = alloca i64, align 8
  %"%acpi_smi_cmd" = alloca i64, align 8
  %"%acpi_dsdt_addr" = alloca i64, align 8
  %"%acpi_fadt_addr.load" = load i64, ptr @acpi_fadt_addr, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_fadt_addr.load", 0
  br i1 %cmpeqtmp, label %then_263, label %merge_264

merge_264:                                        ; preds = %entry
  %calltmp2 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %calltmp3 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_fadt_addr.load4" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp = add i64 %"%acpi_fadt_addr.load4", 40
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%acpi_dsdt_addr", align 8
  %calltmp5 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%acpi_dsdt_addr.load" = load i64, ptr %"%acpi_dsdt_addr", align 8
  %calltmp6 = call i64 @acpi_serial_hex32(i64 %"%acpi_dsdt_addr.load")
  %calltmp7 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_fadt_addr.load8" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp9 = add i64 %"%acpi_fadt_addr.load8", 48
  call void @sad_ll_mem_read32(i64 %addtmp9)
  store i64 0, ptr %"%acpi_smi_cmd", align 8
  %"%acpi_fadt_addr.load10" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp11 = add i64 %"%acpi_fadt_addr.load10", 52
  call void @sad_ll_mem_read8(i64 %addtmp11)
  store i64 0, ptr %"%acpi_enable_val", align 8
  %"%acpi_fadt_addr.load12" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp13 = add i64 %"%acpi_fadt_addr.load12", 53
  call void @sad_ll_mem_read8(i64 %addtmp13)
  store i64 0, ptr %"%acpi_disable_val", align 8
  %"%acpi_fadt_addr.load14" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp15 = add i64 %"%acpi_fadt_addr.load14", 56
  call void @sad_ll_mem_read32(i64 %addtmp15)
  store i64 0, ptr %"%acpi_pm1a_evt", align 8
  %"%acpi_fadt_addr.load16" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp17 = add i64 %"%acpi_fadt_addr.load16", 64
  call void @sad_ll_mem_read32(i64 %addtmp17)
  store i64 0, ptr %"%acpi_pm1a_ctl", align 8
  %"%acpi_fadt_addr.load18" = load i64, ptr @acpi_fadt_addr, align 8
  %addtmp19 = add i64 %"%acpi_fadt_addr.load18", 68
  call void @sad_ll_mem_read32(i64 %addtmp19)
  store i64 0, ptr %"%acpi_pm1b_ctl", align 8
  %calltmp20 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%acpi_pm1a_ctl.load" = load i64, ptr %"%acpi_pm1a_ctl", align 8
  %calltmp21 = call i64 @acpi_serial_hex32(i64 %"%acpi_pm1a_ctl.load")
  %calltmp22 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%acpi_pm1a_evt.load" = load i64, ptr %"%acpi_pm1a_evt", align 8
  %calltmp23 = call i64 @acpi_serial_hex32(i64 %"%acpi_pm1a_evt.load")
  %calltmp24 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp25 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%acpi_smi_cmd.load" = load i64, ptr %"%acpi_smi_cmd", align 8
  %calltmp26 = call i64 @acpi_serial_hex32(i64 %"%acpi_smi_cmd.load")
  %calltmp27 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%851.load" = load i64, ptr %"%acpi_enable_val", align 8
  %calltmp28 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%851.load")
  %calltmp29 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_263:                                         ; preds = %entry
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @acpi_parse_madt() {
entry:
  %"%iso_count" = alloca i64, align 8
  %"%flags85" = alloca i64, align 8
  %"%gsi" = alloca i64, align 8
  %"%source" = alloca i64, align 8
  %"%bus" = alloca i64, align 8
  %"%ioapic_count" = alloca i64, align 8
  %"%io_gsi" = alloca i64, align 8
  %"%io_addr" = alloca i64, align 8
  %"%io_id" = alloca i64, align 8
  %"%lapic_count" = alloca i64, align 8
  %"%flags" = alloca i64, align 8
  %"%apic_id" = alloca i64, align 8
  %"%proc_id" = alloca i64, align 8
  %"%entry_len" = alloca i64, align 8
  %"%entry_type" = alloca i64, align 8
  %"%end_ptr" = alloca i64, align 8
  %"%ptr" = alloca i64, align 8
  %"%madt_flags" = alloca i64, align 8
  %"%acpi_lapic_addr" = alloca i64, align 8
  %"%length" = alloca i64, align 8
  %"%acpi_madt_addr.load" = load i64, ptr @acpi_madt_addr, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_madt_addr.load", 0
  br i1 %cmpeqtmp, label %then_265, label %merge_266

merge_266:                                        ; preds = %entry
  %calltmp2 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp3 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_madt_addr.load4" = load i64, ptr @acpi_madt_addr, align 8
  %addtmp = add i64 %"%acpi_madt_addr.load4", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%length", align 8
  %"%acpi_madt_addr.load5" = load i64, ptr @acpi_madt_addr, align 8
  %addtmp6 = add i64 %"%acpi_madt_addr.load5", 36
  call void @sad_ll_mem_read32(i64 %addtmp6)
  store i64 0, ptr %"%acpi_lapic_addr", align 8
  %calltmp7 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%acpi_lapic_addr.load" = load i64, ptr %"%acpi_lapic_addr", align 8
  %calltmp8 = call i64 @acpi_serial_hex32(i64 %"%acpi_lapic_addr.load")
  %calltmp9 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_madt_addr.load10" = load i64, ptr @acpi_madt_addr, align 8
  %addtmp11 = add i64 %"%acpi_madt_addr.load10", 40
  call void @sad_ll_mem_read32(i64 %addtmp11)
  store i64 0, ptr %"%madt_flags", align 8
  %"%acpi_madt_addr.load12" = load i64, ptr @acpi_madt_addr, align 8
  %addtmp13 = add i64 %"%acpi_madt_addr.load12", 44
  store i64 %addtmp13, ptr %"%ptr", align 8
  %"%acpi_madt_addr.load14" = load i64, ptr @acpi_madt_addr, align 8
  %"%length.load" = load i64, ptr %"%length", align 8
  %addtmp15 = add i64 %"%acpi_madt_addr.load14", %"%length.load"
  store i64 %addtmp15, ptr %"%end_ptr", align 8
  br label %while_cond_267

merge_271:                                        ; preds = %then_270, %while_body_268
  %"%entry_type.load" = load i64, ptr %"%entry_type", align 8
  %"%MADT_LAPIC.load" = load i64, ptr @MADT_LAPIC, align 8
  %cmpeqtmp27 = icmp eq i64 %"%entry_type.load", %"%MADT_LAPIC.load"
  br i1 %cmpeqtmp27, label %then_272, label %merge_273

merge_273:                                        ; preds = %merge_275, %merge_271
  %"%entry_type.load29" = load i64, ptr %"%entry_type", align 8
  %"%MADT_IOAPIC.load" = load i64, ptr @MADT_IOAPIC, align 8
  %cmpeqtmp30 = icmp eq i64 %"%entry_type.load29", %"%MADT_IOAPIC.load"
  br i1 %cmpeqtmp30, label %then_276, label %merge_277

merge_275:                                        ; preds = %then_274, %then_272
  br label %merge_273

merge_277:                                        ; preds = %merge_279, %merge_273
  %"%entry_type.load53" = load i64, ptr %"%entry_type", align 8
  %"%MADT_ISO.load" = load i64, ptr @MADT_ISO, align 8
  %cmpeqtmp54 = icmp eq i64 %"%entry_type.load53", %"%MADT_ISO.load"
  br i1 %cmpeqtmp54, label %then_280, label %merge_281

merge_279:                                        ; preds = %then_278, %then_276
  br label %merge_277

merge_281:                                        ; preds = %merge_283, %merge_277
  %"%ptr.load74" = load i64, ptr %"%ptr", align 8
  %"%entry_len.load75" = load i64, ptr %"%entry_len", align 8
  %addtmp76 = add i64 %"%ptr.load74", %"%entry_len.load75"
  store i64 %addtmp76, ptr %"%ptr", align 8
  br label %while_cond_267

merge_283:                                        ; preds = %then_282, %then_280
  br label %merge_281

then_265:                                         ; preds = %entry
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_270:                                         ; preds = %while_body_268
  br label %merge_271

then_272:                                         ; preds = %merge_271
  %"%lapic_count.load" = load i64, ptr @lapic_count, align 8
  %"%ACPI_MAX_LAPICS.load" = load i64, ptr @ACPI_MAX_LAPICS, align 8
  %cmplttmp28 = icmp slt i64 %"%lapic_count.load", %"%ACPI_MAX_LAPICS.load"
  br i1 %cmplttmp28, label %then_274, label %merge_275

then_274:                                         ; preds = %then_272
  %"%ptr.load31" = load i64, ptr %"%ptr", align 8
  %addtmp32 = add i64 %"%ptr.load31", 2
  call void @sad_ll_mem_read8(i64 %addtmp32)
  store i64 0, ptr %"%proc_id", align 8
  %"%ptr.load33" = load i64, ptr %"%ptr", align 8
  %addtmp34 = add i64 %"%ptr.load33", 3
  call void @sad_ll_mem_read8(i64 %addtmp34)
  store i64 0, ptr %"%apic_id", align 8
  %"%ptr.load35" = load i64, ptr %"%ptr", align 8
  %addtmp36 = add i64 %"%ptr.load35", 4
  call void @sad_ll_mem_read32(i64 %addtmp36)
  store i64 0, ptr %"%flags", align 8
  %"%lapic_count.load37" = load i64, ptr @lapic_count, align 8
  %"%apic_id.load" = load i64, ptr %"%apic_id", align 8
  %calltmp38 = call i64 @acpi_set_lapic_id(i64 %"%lapic_count.load37", i64 %"%apic_id.load")
  %"%lapic_count.load39" = load i64, ptr @lapic_count, align 8
  %"%proc_id.load" = load i64, ptr %"%proc_id", align 8
  %calltmp40 = call i64 @acpi_set_lapic_proc(i64 %"%lapic_count.load39", i64 %"%proc_id.load")
  %"%lapic_count.load41" = load i64, ptr @lapic_count, align 8
  %"%flags.load" = load i64, ptr %"%flags", align 8
  %calltmp42 = call i64 @acpi_set_lapic_flags(i64 %"%lapic_count.load41", i64 %"%flags.load")
  %calltmp43 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%923.load" = load i64, ptr %"%proc_id", align 8
  %calltmp44 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%923.load")
  %calltmp45 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%928.load" = load i64, ptr %"%apic_id", align 8
  %calltmp46 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%928.load")
  %calltmp47 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%933.load" = load i64, ptr %"%flags", align 8
  %calltmp48 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%933.load")
  %calltmp49 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%lapic_count.load50" = load i64, ptr @lapic_count, align 8
  %addtmp51 = add i64 %"%lapic_count.load50", 1
  store i64 %addtmp51, ptr %"%lapic_count", align 8
  br label %merge_275

then_276:                                         ; preds = %merge_273
  %"%ioapic_count.load" = load i64, ptr @ioapic_count, align 8
  %"%ACPI_MAX_IOAPICS.load" = load i64, ptr @ACPI_MAX_IOAPICS, align 8
  %cmplttmp52 = icmp slt i64 %"%ioapic_count.load", %"%ACPI_MAX_IOAPICS.load"
  br i1 %cmplttmp52, label %then_278, label %merge_279

then_278:                                         ; preds = %then_276
  %"%ptr.load55" = load i64, ptr %"%ptr", align 8
  %addtmp56 = add i64 %"%ptr.load55", 2
  call void @sad_ll_mem_read8(i64 %addtmp56)
  store i64 0, ptr %"%io_id", align 8
  %"%ptr.load57" = load i64, ptr %"%ptr", align 8
  %addtmp58 = add i64 %"%ptr.load57", 4
  call void @sad_ll_mem_read32(i64 %addtmp58)
  store i64 0, ptr %"%io_addr", align 8
  %"%ptr.load59" = load i64, ptr %"%ptr", align 8
  %addtmp60 = add i64 %"%ptr.load59", 8
  call void @sad_ll_mem_read32(i64 %addtmp60)
  store i64 0, ptr %"%io_gsi", align 8
  %"%ioapic_count.load61" = load i64, ptr @ioapic_count, align 8
  %"%io_id.load" = load i64, ptr %"%io_id", align 8
  %"%io_addr.load" = load i64, ptr %"%io_addr", align 8
  %"%io_gsi.load" = load i64, ptr %"%io_gsi", align 8
  %calltmp62 = call i64 @acpi_set_ioapic(i64 %"%ioapic_count.load61", i64 %"%io_id.load", i64 %"%io_addr.load", i64 %"%io_gsi.load")
  %calltmp63 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%959.load" = load i64, ptr %"%io_id", align 8
  %calltmp64 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%959.load")
  %calltmp65 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%io_addr.load66" = load i64, ptr %"%io_addr", align 8
  %calltmp67 = call i64 @acpi_serial_hex32(i64 %"%io_addr.load66")
  %calltmp68 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  %"%967.load" = load i64, ptr %"%io_gsi", align 8
  %calltmp69 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%967.load")
  %calltmp70 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ioapic_count.load71" = load i64, ptr @ioapic_count, align 8
  %addtmp72 = add i64 %"%ioapic_count.load71", 1
  store i64 %addtmp72, ptr %"%ioapic_count", align 8
  br label %merge_279

then_280:                                         ; preds = %merge_277
  %"%iso_count.load" = load i64, ptr @iso_count, align 8
  %cmplttmp73 = icmp slt i64 %"%iso_count.load", 8
  br i1 %cmplttmp73, label %then_282, label %merge_283

then_282:                                         ; preds = %then_280
  %"%ptr.load77" = load i64, ptr %"%ptr", align 8
  %addtmp78 = add i64 %"%ptr.load77", 2
  call void @sad_ll_mem_read8(i64 %addtmp78)
  store i64 0, ptr %"%bus", align 8
  %"%ptr.load79" = load i64, ptr %"%ptr", align 8
  %addtmp80 = add i64 %"%ptr.load79", 3
  call void @sad_ll_mem_read8(i64 %addtmp80)
  store i64 0, ptr %"%source", align 8
  %"%ptr.load81" = load i64, ptr %"%ptr", align 8
  %addtmp82 = add i64 %"%ptr.load81", 4
  call void @sad_ll_mem_read32(i64 %addtmp82)
  store i64 0, ptr %"%gsi", align 8
  %"%ptr.load83" = load i64, ptr %"%ptr", align 8
  %addtmp84 = add i64 %"%ptr.load83", 8
  call void @sad_ll_mem_read16(i64 %addtmp84)
  store i64 0, ptr %"%flags85", align 8
  %"%iso_count.load86" = load i64, ptr @iso_count, align 8
  %"%bus.load" = load i64, ptr %"%bus", align 8
  %"%source.load" = load i64, ptr %"%source", align 8
  %"%gsi.load" = load i64, ptr %"%gsi", align 8
  %"%flags.load87" = load i64, ptr %"%flags85", align 8
  %calltmp88 = call i64 @acpi_set_iso(i64 %"%iso_count.load86", i64 %"%bus.load", i64 %"%source.load", i64 %"%gsi.load", i64 %"%flags.load87")
  %calltmp89 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%999.load" = load i64, ptr %"%source", align 8
  %calltmp90 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%999.load")
  %calltmp91 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%1004.load" = load i64, ptr %"%gsi", align 8
  %calltmp92 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1004.load")
  %calltmp93 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%iso_count.load94" = load i64, ptr @iso_count, align 8
  %addtmp95 = add i64 %"%iso_count.load94", 1
  store i64 %addtmp95, ptr %"%iso_count", align 8
  br label %merge_283

while_body_268:                                   ; preds = %while_cond_267
  %"%ptr.load16" = load i64, ptr %"%ptr", align 8
  call void @sad_ll_mem_read8(i64 %"%ptr.load16")
  store i64 0, ptr %"%entry_type", align 8
  %"%ptr.load17" = load i64, ptr %"%ptr", align 8
  %addtmp18 = add i64 %"%ptr.load17", 1
  call void @sad_ll_mem_read8(i64 %addtmp18)
  store i64 0, ptr %"%entry_len", align 8
  %"%entry_len.load" = load i64, ptr %"%entry_len", align 8
  %cmplttmp19 = icmp slt i64 %"%entry_len.load", 2
  br i1 %cmplttmp19, label %then_270, label %merge_271

while_cond_267:                                   ; preds = %merge_281, %merge_266
  %"%ptr.load" = load i64, ptr %"%ptr", align 8
  %"%end_ptr.load" = load i64, ptr %"%end_ptr", align 8
  %cmplttmp = icmp slt i64 %"%ptr.load", %"%end_ptr.load"
  br i1 %cmplttmp, label %while_body_268, label %while_exit_269

while_exit_269:                                   ; preds = %while_cond_267
  %calltmp20 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%1013.load" = load i64, ptr @lapic_count, align 8
  %calltmp21 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1013.load")
  %calltmp22 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1018.load" = load i64, ptr @ioapic_count, align 8
  %calltmp23 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1018.load")
  %calltmp24 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%1023.load" = load i64, ptr @iso_count, align 8
  %calltmp25 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1023.load")
  %calltmp26 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr @lapic_count, align 8
  ret i64 %loadtmp
}

define i64 @acpi_parse_hpet() {
entry:
  %"%acpi_hpet_base" = alloca i64, align 8
  %"%acpi_hpet_addr.load" = load i64, ptr @acpi_hpet_addr, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_hpet_addr.load", 0
  br i1 %cmpeqtmp, label %then_284, label %merge_285

merge_285:                                        ; preds = %entry
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_hpet_addr.load2" = load i64, ptr @acpi_hpet_addr, align 8
  %addtmp = add i64 %"%acpi_hpet_addr.load2", 44
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%acpi_hpet_base", align 8
  %calltmp3 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%acpi_hpet_base.load" = load i64, ptr %"%acpi_hpet_base", align 8
  %calltmp4 = call i64 @acpi_serial_hex32(i64 %"%acpi_hpet_base.load")
  %calltmp5 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_284:                                         ; preds = %entry
  ret i64 -1
}

define i64 @acpi_parse_mcfg() {
entry:
  %"%acpi_pcie_base" = alloca i64, align 8
  %"%length" = alloca i64, align 8
  %"%acpi_mcfg_addr.load" = load i64, ptr @acpi_mcfg_addr, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_mcfg_addr.load", 0
  br i1 %cmpeqtmp, label %then_286, label %merge_287

merge_287:                                        ; preds = %entry
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_mcfg_addr.load2" = load i64, ptr @acpi_mcfg_addr, align 8
  %addtmp = add i64 %"%acpi_mcfg_addr.load2", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%length", align 8
  %"%length.load" = load i64, ptr %"%length", align 8
  %cmpgttmp = icmp sgt i64 %"%length.load", 44
  br i1 %cmpgttmp, label %then_288, label %merge_289

merge_289:                                        ; preds = %then_288, %merge_287
  ret i64 0

then_286:                                         ; preds = %entry
  ret i64 -1

then_288:                                         ; preds = %merge_287
  %"%acpi_mcfg_addr.load3" = load i64, ptr @acpi_mcfg_addr, align 8
  %addtmp4 = add i64 %"%acpi_mcfg_addr.load3", 44
  call void @sad_ll_mem_read32(i64 %addtmp4)
  store i64 0, ptr %"%acpi_pcie_base", align 8
  %calltmp5 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %"%acpi_pcie_base.load" = load i64, ptr %"%acpi_pcie_base", align 8
  %calltmp6 = call i64 @acpi_serial_hex32(i64 %"%acpi_pcie_base.load")
  %calltmp7 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_289
}

define i64 @acpi_enable() {
entry:
  %"%acpi_errors" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%acpi_enabled" = alloca i64, align 8
  %"%pm1a" = alloca i64, align 8
  %"%acpi_pm1a_ctl.load" = load i64, ptr @acpi_pm1a_ctl, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_pm1a_ctl.load", 0
  br i1 %cmpeqtmp, label %then_290, label %merge_291

merge_291:                                        ; preds = %entry
  %"%acpi_pm1a_ctl.load1" = load i64, ptr @acpi_pm1a_ctl, align 8
  call void @sad_ll_port_inw(i64 %"%acpi_pm1a_ctl.load1")
  store i64 0, ptr %"%pm1a", align 8
  %"%pm1a.load" = load i64, ptr %"%pm1a", align 8
  %andtmp = and i64 %"%pm1a.load", 1
  %cmpeqtmp2 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp2, label %then_292, label %merge_293

merge_293:                                        ; preds = %merge_291
  %"%acpi_smi_cmd.load" = load i64, ptr @acpi_smi_cmd, align 8
  %cmpgttmp = icmp sgt i64 %"%acpi_smi_cmd.load", 0
  br i1 %cmpgttmp, label %then_294, label %merge_295

merge_295:                                        ; preds = %merge_293
  %calltmp5 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %calltmp6 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

merge_300:                                        ; preds = %while_body_297
  %"%timeout.load16" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load16", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_296

then_290:                                         ; preds = %entry
  ret i64 -1

then_292:                                         ; preds = %merge_291
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %calltmp3 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 1, ptr %"%acpi_enabled", align 8
  ret i64 0

then_294:                                         ; preds = %merge_293
  %"%acpi_smi_cmd.load4" = load i64, ptr @acpi_smi_cmd, align 8
  %"%acpi_enable_val.load" = load i64, ptr @acpi_enable_val, align 8
  call void @sad_ll_port_outb(i64 %"%acpi_smi_cmd.load4", i64 %"%acpi_enable_val.load")
  store i64 10000, ptr %"%timeout", align 8
  br label %while_cond_296

then_299:                                         ; preds = %while_body_297
  %calltmp14 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %calltmp15 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 1, ptr %"%acpi_enabled", align 8
  ret i64 0

while_body_297:                                   ; preds = %while_cond_296
  %"%acpi_pm1a_ctl.load8" = load i64, ptr @acpi_pm1a_ctl, align 8
  call void @sad_ll_port_inw(i64 %"%acpi_pm1a_ctl.load8")
  store i64 0, ptr %"%pm1a", align 8
  %"%pm1a.load9" = load i64, ptr %"%pm1a", align 8
  %andtmp10 = and i64 %"%pm1a.load9", 1
  %cmpeqtmp11 = icmp eq i64 %andtmp10, 1
  br i1 %cmpeqtmp11, label %then_299, label %merge_300

while_cond_296:                                   ; preds = %merge_300, %then_294
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp7 = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp7, label %while_body_297, label %while_exit_298

while_exit_298:                                   ; preds = %while_cond_296
  %calltmp12 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %calltmp13 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_errors.load" = load i64, ptr @acpi_errors, align 8
  %addtmp = add i64 %"%acpi_errors.load", 1
  store i64 %addtmp, ptr %"%acpi_errors", align 8
  ret i64 -1
}

define i64 @acpi_shutdown() {
entry:
  %"%slp_b" = alloca i64, align 8
  %"%slp" = alloca i64, align 8
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_pm1a_ctl.load" = load i64, ptr @acpi_pm1a_ctl, align 8
  %cmpeqtmp = icmp eq i64 %"%acpi_pm1a_ctl.load", 0
  br i1 %cmpeqtmp, label %then_301, label %merge_302

merge_302:                                        ; preds = %entry
  %"%acpi_slp_typa.load" = load i64, ptr @acpi_slp_typa, align 8
  %shltmp = shl i64 %"%acpi_slp_typa.load", 10
  %"%PM1_SLP_EN.load" = load i64, ptr @PM1_SLP_EN, align 8
  %addtmp = add i64 %shltmp, %"%PM1_SLP_EN.load"
  store i64 %addtmp, ptr %"%slp", align 8
  %"%acpi_pm1a_ctl.load4" = load i64, ptr @acpi_pm1a_ctl, align 8
  %"%slp.load" = load i64, ptr %"%slp", align 8
  call void @sad_ll_port_outw(i64 %"%acpi_pm1a_ctl.load4", i64 %"%slp.load")
  %"%acpi_pm1b_ctl.load" = load i64, ptr @acpi_pm1b_ctl, align 8
  %cmpgttmp = icmp sgt i64 %"%acpi_pm1b_ctl.load", 0
  br i1 %cmpgttmp, label %then_303, label %merge_304

merge_304:                                        ; preds = %then_303, %merge_302
  %calltmp9 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  %calltmp10 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_301:                                         ; preds = %entry
  %calltmp2 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  %calltmp3 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_303:                                         ; preds = %merge_302
  %"%acpi_slp_typb.load" = load i64, ptr @acpi_slp_typb, align 8
  %shltmp5 = shl i64 %"%acpi_slp_typb.load", 10
  %"%PM1_SLP_EN.load6" = load i64, ptr @PM1_SLP_EN, align 8
  %addtmp7 = add i64 %shltmp5, %"%PM1_SLP_EN.load6"
  store i64 %addtmp7, ptr %"%slp_b", align 8
  %"%acpi_pm1b_ctl.load8" = load i64, ptr @acpi_pm1b_ctl, align 8
  %"%slp_b.load" = load i64, ptr %"%slp_b", align 8
  call void @sad_ll_port_outw(i64 %"%acpi_pm1b_ctl.load8", i64 %"%slp_b.load")
  br label %merge_304
}

define i64 @acpi_reboot() {
entry:
  %"%sts" = alloca i64, align 8
  %"%good" = alloca i64, align 8
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%good", align 8
  br label %while_cond_305

merge_309:                                        ; preds = %then_308, %while_body_306
  br label %while_cond_305

then_308:                                         ; preds = %while_body_306
  store i64 1, ptr %"%good", align 8
  br label %merge_309

while_body_306:                                   ; preds = %while_cond_305
  call void @sad_ll_port_inb(i64 100)
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %shrtmp = lshr i64 %"%sts.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp2 = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp2, label %then_308, label %merge_309

while_cond_305:                                   ; preds = %merge_309, %entry
  %"%good.load" = load i64, ptr %"%good", align 8
  %cmpeqtmp = icmp eq i64 %"%good.load", 0
  br i1 %cmpeqtmp, label %while_body_306, label %while_exit_307

while_exit_307:                                   ; preds = %while_cond_305
  call void @sad_ll_port_outb(i64 100, i64 254)
  ret i64 0
}

define i64 @acpi_get_lapic_count() {
entry:
  %loadtmp = load i64, ptr @lapic_count, align 8
  ret i64 %loadtmp
}

define i64 @acpi_get_ioapic_count() {
entry:
  %loadtmp = load i64, ptr @ioapic_count, align 8
  ret i64 %loadtmp
}

define i64 @acpi_get_lapic_base() {
entry:
  %loadtmp = load i64, ptr @acpi_lapic_addr, align 8
  ret i64 %loadtmp
}

define i64 @acpi_get_hpet_base() {
entry:
  %loadtmp = load i64, ptr @acpi_hpet_base, align 8
  ret i64 %loadtmp
}

define i64 @acpi_get_pcie_base() {
entry:
  %loadtmp = load i64, ptr @acpi_pcie_base, align 8
  ret i64 %loadtmp
}

define i64 @acpi_get_table_count() {
entry:
  %loadtmp = load i64, ptr @acpi_table_count, align 8
  ret i64 %loadtmp
}

define i64 @acpi_is_enabled() {
entry:
  %loadtmp = load i64, ptr @acpi_enabled, align 8
  ret i64 %loadtmp
}

define i64 @"acpi_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  %"%1206.load" = load i64, ptr @acpi_revision, align 8
  %calltmp3 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1206.load")
  %calltmp4 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  %"%1211.load" = load i64, ptr @acpi_table_count, align 8
  %calltmp5 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1211.load")
  %calltmp6 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp7 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  %"%acpi_rsdt_addr.load" = load i64, ptr @acpi_rsdt_addr, align 8
  %calltmp8 = call i64 @acpi_serial_hex32(i64 %"%acpi_rsdt_addr.load")
  %"%acpi_fadt_addr.load" = load i64, ptr @acpi_fadt_addr, align 8
  %cmpgttmp = icmp sgt i64 %"%acpi_fadt_addr.load", 0
  br i1 %cmpgttmp, label %then_310, label %merge_311

merge_311:                                        ; preds = %then_310, %entry
  %calltmp12 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %"%acpi_madt_addr.load" = load i64, ptr @acpi_madt_addr, align 8
  %cmpgttmp13 = icmp sgt i64 %"%acpi_madt_addr.load", 0
  br i1 %cmpgttmp13, label %then_312, label %merge_313

merge_313:                                        ; preds = %then_312, %merge_311
  %calltmp22 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  %"%1242.load" = load i64, ptr @acpi_enabled, align 8
  %calltmp23 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1242.load")
  %calltmp24 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  %"%1247.load" = load i64, ptr @acpi_errors, align 8
  %calltmp25 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1247.load")
  %calltmp26 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp27 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.55 to i64))
  %calltmp28 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_310:                                         ; preds = %entry
  %calltmp9 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.49 to i64))
  %"%acpi_fadt_addr.load10" = load i64, ptr @acpi_fadt_addr, align 8
  %calltmp11 = call i64 @acpi_serial_hex32(i64 %"%acpi_fadt_addr.load10")
  br label %merge_311

then_312:                                         ; preds = %merge_311
  %calltmp14 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.50 to i64))
  %"%acpi_madt_addr.load15" = load i64, ptr @acpi_madt_addr, align 8
  %calltmp16 = call i64 @acpi_serial_hex32(i64 %"%acpi_madt_addr.load15")
  %calltmp17 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.51 to i64))
  %"%1231.load" = load i64, ptr @lapic_count, align 8
  %calltmp18 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1231.load")
  %calltmp19 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  %"%1236.load" = load i64, ptr @ioapic_count, align 8
  %calltmp20 = call i64 @"acpi_serial_\D8\B1\D9\82\D9\85"(i64 %"%1236.load")
  %calltmp21 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_313
}

define i64 @"acpi_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%rsdp" = alloca i64, align 8
  %calltmp = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  %calltmp1 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @acpi_find_rsdp()
  store i64 %calltmp2, ptr %"%rsdp", align 8
  %"%rsdp.load" = load i64, ptr %"%rsdp", align 8
  %cmpeqtmp = icmp eq i64 %"%rsdp.load", 0
  br i1 %cmpeqtmp, label %then_314, label %merge_315

merge_315:                                        ; preds = %entry
  %"%rsdp.load5" = load i64, ptr %"%rsdp", align 8
  %calltmp6 = call i64 @acpi_parse_rsdp(i64 %"%rsdp.load5")
  %calltmp7 = call i64 @acpi_parse_rsdt()
  %calltmp8 = call i64 @acpi_parse_fadt()
  %calltmp9 = call i64 @acpi_parse_madt()
  %calltmp10 = call i64 @acpi_parse_hpet()
  %calltmp11 = call i64 @acpi_parse_mcfg()
  %calltmp12 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  %calltmp13 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0

then_314:                                         ; preds = %entry
  %calltmp3 = call i64 @"acpi_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  %calltmp4 = call i64 @"acpi_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1
}

define i64 @"acpi_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%acpi_errors" = alloca i64, align 8
  %"%acpi_table_count" = alloca i64, align 8
  %"%acpi_table_sig0_15" = alloca i64, align 8
  %"%acpi_table_sig0_14" = alloca i64, align 8
  %"%acpi_table_sig0_13" = alloca i64, align 8
  %"%acpi_table_sig0_12" = alloca i64, align 8
  %"%acpi_table_sig0_11" = alloca i64, align 8
  %"%acpi_table_sig0_10" = alloca i64, align 8
  %"%acpi_table_sig0_9" = alloca i64, align 8
  %"%acpi_table_sig0_8" = alloca i64, align 8
  %"%acpi_table_sig0_7" = alloca i64, align 8
  %"%acpi_table_sig0_6" = alloca i64, align 8
  %"%acpi_table_sig0_5" = alloca i64, align 8
  %"%acpi_table_sig0_4" = alloca i64, align 8
  %"%acpi_table_sig0_3" = alloca i64, align 8
  %"%acpi_table_sig0_2" = alloca i64, align 8
  %"%acpi_table_sig0_1" = alloca i64, align 8
  %"%acpi_table_sig0_0" = alloca i64, align 8
  %"%acpi_table_addr_15" = alloca i64, align 8
  %"%acpi_table_addr_14" = alloca i64, align 8
  %"%acpi_table_addr_13" = alloca i64, align 8
  %"%acpi_table_addr_12" = alloca i64, align 8
  %"%acpi_table_addr_11" = alloca i64, align 8
  %"%acpi_table_addr_10" = alloca i64, align 8
  %"%acpi_table_addr_9" = alloca i64, align 8
  %"%acpi_table_addr_8" = alloca i64, align 8
  %"%acpi_table_addr_7" = alloca i64, align 8
  %"%acpi_table_addr_6" = alloca i64, align 8
  %"%acpi_table_addr_5" = alloca i64, align 8
  %"%acpi_table_addr_4" = alloca i64, align 8
  %"%acpi_table_addr_3" = alloca i64, align 8
  %"%acpi_table_addr_2" = alloca i64, align 8
  %"%acpi_table_addr_1" = alloca i64, align 8
  %"%acpi_table_addr_0" = alloca i64, align 8
  %"%iso_count" = alloca i64, align 8
  %"%iso_flags_7" = alloca i64, align 8
  %"%iso_flags_6" = alloca i64, align 8
  %"%iso_flags_5" = alloca i64, align 8
  %"%iso_flags_4" = alloca i64, align 8
  %"%iso_flags_3" = alloca i64, align 8
  %"%iso_flags_2" = alloca i64, align 8
  %"%iso_flags_1" = alloca i64, align 8
  %"%iso_flags_0" = alloca i64, align 8
  %"%iso_gsi_7" = alloca i64, align 8
  %"%iso_gsi_6" = alloca i64, align 8
  %"%iso_gsi_5" = alloca i64, align 8
  %"%iso_gsi_4" = alloca i64, align 8
  %"%iso_gsi_3" = alloca i64, align 8
  %"%iso_gsi_2" = alloca i64, align 8
  %"%iso_gsi_1" = alloca i64, align 8
  %"%iso_gsi_0" = alloca i64, align 8
  %"%iso_source_7" = alloca i64, align 8
  %"%iso_source_6" = alloca i64, align 8
  %"%iso_source_5" = alloca i64, align 8
  %"%iso_source_4" = alloca i64, align 8
  %"%iso_source_3" = alloca i64, align 8
  %"%iso_source_2" = alloca i64, align 8
  %"%iso_source_1" = alloca i64, align 8
  %"%iso_source_0" = alloca i64, align 8
  %"%iso_bus_7" = alloca i64, align 8
  %"%iso_bus_6" = alloca i64, align 8
  %"%iso_bus_5" = alloca i64, align 8
  %"%iso_bus_4" = alloca i64, align 8
  %"%iso_bus_3" = alloca i64, align 8
  %"%iso_bus_2" = alloca i64, align 8
  %"%iso_bus_1" = alloca i64, align 8
  %"%iso_bus_0" = alloca i64, align 8
  %"%ioapic_count" = alloca i64, align 8
  %"%ioapic_gsi_3" = alloca i64, align 8
  %"%ioapic_gsi_2" = alloca i64, align 8
  %"%ioapic_gsi_1" = alloca i64, align 8
  %"%ioapic_gsi_0" = alloca i64, align 8
  %"%ioapic_addr_3" = alloca i64, align 8
  %"%ioapic_addr_2" = alloca i64, align 8
  %"%ioapic_addr_1" = alloca i64, align 8
  %"%ioapic_addr_0" = alloca i64, align 8
  %"%ioapic_id_3" = alloca i64, align 8
  %"%ioapic_id_2" = alloca i64, align 8
  %"%ioapic_id_1" = alloca i64, align 8
  %"%ioapic_id_0" = alloca i64, align 8
  %"%lapic_count" = alloca i64, align 8
  %"%lapic_flags_15" = alloca i64, align 8
  %"%lapic_flags_14" = alloca i64, align 8
  %"%lapic_flags_13" = alloca i64, align 8
  %"%lapic_flags_12" = alloca i64, align 8
  %"%lapic_flags_11" = alloca i64, align 8
  %"%lapic_flags_10" = alloca i64, align 8
  %"%lapic_flags_9" = alloca i64, align 8
  %"%lapic_flags_8" = alloca i64, align 8
  %"%lapic_flags_7" = alloca i64, align 8
  %"%lapic_flags_6" = alloca i64, align 8
  %"%lapic_flags_5" = alloca i64, align 8
  %"%lapic_flags_4" = alloca i64, align 8
  %"%lapic_flags_3" = alloca i64, align 8
  %"%lapic_flags_2" = alloca i64, align 8
  %"%lapic_flags_1" = alloca i64, align 8
  %"%lapic_flags_0" = alloca i64, align 8
  %"%lapic_proc_15" = alloca i64, align 8
  %"%lapic_proc_14" = alloca i64, align 8
  %"%lapic_proc_13" = alloca i64, align 8
  %"%lapic_proc_12" = alloca i64, align 8
  %"%lapic_proc_11" = alloca i64, align 8
  %"%lapic_proc_10" = alloca i64, align 8
  %"%lapic_proc_9" = alloca i64, align 8
  %"%lapic_proc_8" = alloca i64, align 8
  %"%lapic_proc_7" = alloca i64, align 8
  %"%lapic_proc_6" = alloca i64, align 8
  %"%lapic_proc_5" = alloca i64, align 8
  %"%lapic_proc_4" = alloca i64, align 8
  %"%lapic_proc_3" = alloca i64, align 8
  %"%lapic_proc_2" = alloca i64, align 8
  %"%lapic_proc_1" = alloca i64, align 8
  %"%lapic_proc_0" = alloca i64, align 8
  %"%lapic_id_15" = alloca i64, align 8
  %"%lapic_id_14" = alloca i64, align 8
  %"%lapic_id_13" = alloca i64, align 8
  %"%lapic_id_12" = alloca i64, align 8
  %"%lapic_id_11" = alloca i64, align 8
  %"%lapic_id_10" = alloca i64, align 8
  %"%lapic_id_9" = alloca i64, align 8
  %"%lapic_id_8" = alloca i64, align 8
  %"%lapic_id_7" = alloca i64, align 8
  %"%lapic_id_6" = alloca i64, align 8
  %"%lapic_id_5" = alloca i64, align 8
  %"%lapic_id_4" = alloca i64, align 8
  %"%lapic_id_3" = alloca i64, align 8
  %"%lapic_id_2" = alloca i64, align 8
  %"%lapic_id_1" = alloca i64, align 8
  %"%lapic_id_0" = alloca i64, align 8
  %"%acpi_pcie_base" = alloca i64, align 8
  %"%acpi_mcfg_addr" = alloca i64, align 8
  %"%acpi_hpet_base" = alloca i64, align 8
  %"%acpi_hpet_addr" = alloca i64, align 8
  %"%acpi_lapic_addr" = alloca i64, align 8
  %"%acpi_madt_addr" = alloca i64, align 8
  %"%acpi_enabled" = alloca i64, align 8
  %"%acpi_slp_typb" = alloca i64, align 8
  %"%acpi_slp_typa" = alloca i64, align 8
  %"%acpi_disable_val" = alloca i64, align 8
  %"%acpi_enable_val" = alloca i64, align 8
  %"%acpi_smi_cmd" = alloca i64, align 8
  %"%acpi_pm1a_evt" = alloca i64, align 8
  %"%acpi_pm1b_ctl" = alloca i64, align 8
  %"%acpi_pm1a_ctl" = alloca i64, align 8
  %"%acpi_dsdt_addr" = alloca i64, align 8
  %"%acpi_fadt_addr" = alloca i64, align 8
  %"%acpi_xsdt_addr" = alloca i64, align 8
  %"%acpi_rsdt_addr" = alloca i64, align 8
  %"%acpi_revision" = alloca i64, align 8
  %"%acpi_rsdp_addr" = alloca i64, align 8
  %"%ACPI_MAX_ISOS" = alloca i64, align 8
  %"%ACPI_MAX_IOAPICS" = alloca i64, align 8
  %"%ACPI_MAX_LAPICS" = alloca i64, align 8
  %"%ACPI_MAX_TABLES" = alloca i64, align 8
  %"%PM1_SLP_EN" = alloca i64, align 8
  %"%PM1_SCI_EN" = alloca i64, align 8
  %"%FADT_SLP_TYPB" = alloca i64, align 8
  %"%FADT_SLP_TYPA" = alloca i64, align 8
  %"%FADT_ACPI_DISABLE" = alloca i64, align 8
  %"%FADT_ACPI_ENABLE" = alloca i64, align 8
  %"%FADT_SMI_CMD" = alloca i64, align 8
  %"%FADT_DSDT" = alloca i64, align 8
  %"%FADT_PM1A_EVT" = alloca i64, align 8
  %"%FADT_PM1B_CTL" = alloca i64, align 8
  %"%FADT_PM1A_CTL" = alloca i64, align 8
  %"%MADT_LAPIC_NMI" = alloca i64, align 8
  %"%MADT_NMI" = alloca i64, align 8
  %"%MADT_ISO" = alloca i64, align 8
  %"%MADT_IOAPIC" = alloca i64, align 8
  %"%MADT_LAPIC" = alloca i64, align 8
  %"%SIG_XSDT_3" = alloca i64, align 8
  %"%SIG_XSDT_2" = alloca i64, align 8
  %"%SIG_XSDT_1" = alloca i64, align 8
  %"%SIG_XSDT_0" = alloca i64, align 8
  %"%SIG_RSDT_3" = alloca i64, align 8
  %"%SIG_RSDT_2" = alloca i64, align 8
  %"%SIG_RSDT_1" = alloca i64, align 8
  %"%SIG_RSDT_0" = alloca i64, align 8
  %"%SIG_MCFG_3" = alloca i64, align 8
  %"%SIG_MCFG_2" = alloca i64, align 8
  %"%SIG_MCFG_1" = alloca i64, align 8
  %"%SIG_MCFG_0" = alloca i64, align 8
  %"%SIG_HPET_3" = alloca i64, align 8
  %"%SIG_HPET_2" = alloca i64, align 8
  %"%SIG_HPET_1" = alloca i64, align 8
  %"%SIG_HPET_0" = alloca i64, align 8
  %"%SIG_APIC_3" = alloca i64, align 8
  %"%SIG_APIC_2" = alloca i64, align 8
  %"%SIG_APIC_1" = alloca i64, align 8
  %"%SIG_APIC_0" = alloca i64, align 8
  %"%SIG_FACP_3" = alloca i64, align 8
  %"%SIG_FACP_2" = alloca i64, align 8
  %"%SIG_FACP_1" = alloca i64, align 8
  %"%SIG_FACP_0" = alloca i64, align 8
  %"%RSDP_SIG_7" = alloca i64, align 8
  %"%RSDP_SIG_6" = alloca i64, align 8
  %"%RSDP_SIG_5" = alloca i64, align 8
  %"%RSDP_SIG_4" = alloca i64, align 8
  %"%RSDP_SIG_3" = alloca i64, align 8
  %"%RSDP_SIG_2" = alloca i64, align 8
  %"%RSDP_SIG_1" = alloca i64, align 8
  %"%RSDP_SIG_0" = alloca i64, align 8
  %"%RSDP_BIOS_END" = alloca i64, align 8
  %"%RSDP_BIOS_START" = alloca i64, align 8
  %"%RSDP_EBDA_START" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 655360, ptr %"%RSDP_EBDA_START", align 8
  store i64 917504, ptr %"%RSDP_BIOS_START", align 8
  store i64 1048576, ptr %"%RSDP_BIOS_END", align 8
  store i64 82, ptr %"%RSDP_SIG_0", align 8
  store i64 83, ptr %"%RSDP_SIG_1", align 8
  store i64 68, ptr %"%RSDP_SIG_2", align 8
  store i64 32, ptr %"%RSDP_SIG_3", align 8
  store i64 80, ptr %"%RSDP_SIG_4", align 8
  store i64 84, ptr %"%RSDP_SIG_5", align 8
  store i64 82, ptr %"%RSDP_SIG_6", align 8
  store i64 32, ptr %"%RSDP_SIG_7", align 8
  store i64 70, ptr %"%SIG_FACP_0", align 8
  store i64 65, ptr %"%SIG_FACP_1", align 8
  store i64 67, ptr %"%SIG_FACP_2", align 8
  store i64 80, ptr %"%SIG_FACP_3", align 8
  store i64 65, ptr %"%SIG_APIC_0", align 8
  store i64 80, ptr %"%SIG_APIC_1", align 8
  store i64 73, ptr %"%SIG_APIC_2", align 8
  store i64 67, ptr %"%SIG_APIC_3", align 8
  store i64 72, ptr %"%SIG_HPET_0", align 8
  store i64 80, ptr %"%SIG_HPET_1", align 8
  store i64 69, ptr %"%SIG_HPET_2", align 8
  store i64 84, ptr %"%SIG_HPET_3", align 8
  store i64 77, ptr %"%SIG_MCFG_0", align 8
  store i64 67, ptr %"%SIG_MCFG_1", align 8
  store i64 70, ptr %"%SIG_MCFG_2", align 8
  store i64 71, ptr %"%SIG_MCFG_3", align 8
  store i64 82, ptr %"%SIG_RSDT_0", align 8
  store i64 83, ptr %"%SIG_RSDT_1", align 8
  store i64 68, ptr %"%SIG_RSDT_2", align 8
  store i64 84, ptr %"%SIG_RSDT_3", align 8
  store i64 88, ptr %"%SIG_XSDT_0", align 8
  store i64 83, ptr %"%SIG_XSDT_1", align 8
  store i64 68, ptr %"%SIG_XSDT_2", align 8
  store i64 84, ptr %"%SIG_XSDT_3", align 8
  store i64 0, ptr %"%MADT_LAPIC", align 8
  store i64 1, ptr %"%MADT_IOAPIC", align 8
  store i64 2, ptr %"%MADT_ISO", align 8
  store i64 3, ptr %"%MADT_NMI", align 8
  store i64 4, ptr %"%MADT_LAPIC_NMI", align 8
  store i64 64, ptr %"%FADT_PM1A_CTL", align 8
  store i64 68, ptr %"%FADT_PM1B_CTL", align 8
  store i64 56, ptr %"%FADT_PM1A_EVT", align 8
  store i64 40, ptr %"%FADT_DSDT", align 8
  store i64 48, ptr %"%FADT_SMI_CMD", align 8
  store i64 52, ptr %"%FADT_ACPI_ENABLE", align 8
  store i64 53, ptr %"%FADT_ACPI_DISABLE", align 8
  store i64 0, ptr %"%FADT_SLP_TYPA", align 8
  store i64 0, ptr %"%FADT_SLP_TYPB", align 8
  store i64 1, ptr %"%PM1_SCI_EN", align 8
  store i64 8192, ptr %"%PM1_SLP_EN", align 8
  store i64 32, ptr %"%ACPI_MAX_TABLES", align 8
  store i64 16, ptr %"%ACPI_MAX_LAPICS", align 8
  store i64 4, ptr %"%ACPI_MAX_IOAPICS", align 8
  store i64 16, ptr %"%ACPI_MAX_ISOS", align 8
  store i64 0, ptr %"%acpi_rsdp_addr", align 8
  store i64 0, ptr %"%acpi_revision", align 8
  store i64 0, ptr %"%acpi_rsdt_addr", align 8
  store i64 0, ptr %"%acpi_xsdt_addr", align 8
  store i64 0, ptr %"%acpi_fadt_addr", align 8
  store i64 0, ptr %"%acpi_dsdt_addr", align 8
  store i64 0, ptr %"%acpi_pm1a_ctl", align 8
  store i64 0, ptr %"%acpi_pm1b_ctl", align 8
  store i64 0, ptr %"%acpi_pm1a_evt", align 8
  store i64 0, ptr %"%acpi_smi_cmd", align 8
  store i64 0, ptr %"%acpi_enable_val", align 8
  store i64 0, ptr %"%acpi_disable_val", align 8
  store i64 0, ptr %"%acpi_slp_typa", align 8
  store i64 0, ptr %"%acpi_slp_typb", align 8
  store i64 0, ptr %"%acpi_enabled", align 8
  store i64 0, ptr %"%acpi_madt_addr", align 8
  store i64 0, ptr %"%acpi_lapic_addr", align 8
  store i64 0, ptr %"%acpi_hpet_addr", align 8
  store i64 0, ptr %"%acpi_hpet_base", align 8
  store i64 0, ptr %"%acpi_mcfg_addr", align 8
  store i64 0, ptr %"%acpi_pcie_base", align 8
  store i64 0, ptr %"%lapic_id_0", align 8
  store i64 0, ptr %"%lapic_id_1", align 8
  store i64 0, ptr %"%lapic_id_2", align 8
  store i64 0, ptr %"%lapic_id_3", align 8
  store i64 0, ptr %"%lapic_id_4", align 8
  store i64 0, ptr %"%lapic_id_5", align 8
  store i64 0, ptr %"%lapic_id_6", align 8
  store i64 0, ptr %"%lapic_id_7", align 8
  store i64 0, ptr %"%lapic_id_8", align 8
  store i64 0, ptr %"%lapic_id_9", align 8
  store i64 0, ptr %"%lapic_id_10", align 8
  store i64 0, ptr %"%lapic_id_11", align 8
  store i64 0, ptr %"%lapic_id_12", align 8
  store i64 0, ptr %"%lapic_id_13", align 8
  store i64 0, ptr %"%lapic_id_14", align 8
  store i64 0, ptr %"%lapic_id_15", align 8
  store i64 0, ptr %"%lapic_proc_0", align 8
  store i64 0, ptr %"%lapic_proc_1", align 8
  store i64 0, ptr %"%lapic_proc_2", align 8
  store i64 0, ptr %"%lapic_proc_3", align 8
  store i64 0, ptr %"%lapic_proc_4", align 8
  store i64 0, ptr %"%lapic_proc_5", align 8
  store i64 0, ptr %"%lapic_proc_6", align 8
  store i64 0, ptr %"%lapic_proc_7", align 8
  store i64 0, ptr %"%lapic_proc_8", align 8
  store i64 0, ptr %"%lapic_proc_9", align 8
  store i64 0, ptr %"%lapic_proc_10", align 8
  store i64 0, ptr %"%lapic_proc_11", align 8
  store i64 0, ptr %"%lapic_proc_12", align 8
  store i64 0, ptr %"%lapic_proc_13", align 8
  store i64 0, ptr %"%lapic_proc_14", align 8
  store i64 0, ptr %"%lapic_proc_15", align 8
  store i64 0, ptr %"%lapic_flags_0", align 8
  store i64 0, ptr %"%lapic_flags_1", align 8
  store i64 0, ptr %"%lapic_flags_2", align 8
  store i64 0, ptr %"%lapic_flags_3", align 8
  store i64 0, ptr %"%lapic_flags_4", align 8
  store i64 0, ptr %"%lapic_flags_5", align 8
  store i64 0, ptr %"%lapic_flags_6", align 8
  store i64 0, ptr %"%lapic_flags_7", align 8
  store i64 0, ptr %"%lapic_flags_8", align 8
  store i64 0, ptr %"%lapic_flags_9", align 8
  store i64 0, ptr %"%lapic_flags_10", align 8
  store i64 0, ptr %"%lapic_flags_11", align 8
  store i64 0, ptr %"%lapic_flags_12", align 8
  store i64 0, ptr %"%lapic_flags_13", align 8
  store i64 0, ptr %"%lapic_flags_14", align 8
  store i64 0, ptr %"%lapic_flags_15", align 8
  store i64 0, ptr %"%lapic_count", align 8
  store i64 0, ptr %"%ioapic_id_0", align 8
  store i64 0, ptr %"%ioapic_id_1", align 8
  store i64 0, ptr %"%ioapic_id_2", align 8
  store i64 0, ptr %"%ioapic_id_3", align 8
  store i64 0, ptr %"%ioapic_addr_0", align 8
  store i64 0, ptr %"%ioapic_addr_1", align 8
  store i64 0, ptr %"%ioapic_addr_2", align 8
  store i64 0, ptr %"%ioapic_addr_3", align 8
  store i64 0, ptr %"%ioapic_gsi_0", align 8
  store i64 0, ptr %"%ioapic_gsi_1", align 8
  store i64 0, ptr %"%ioapic_gsi_2", align 8
  store i64 0, ptr %"%ioapic_gsi_3", align 8
  store i64 0, ptr %"%ioapic_count", align 8
  store i64 0, ptr %"%iso_bus_0", align 8
  store i64 0, ptr %"%iso_bus_1", align 8
  store i64 0, ptr %"%iso_bus_2", align 8
  store i64 0, ptr %"%iso_bus_3", align 8
  store i64 0, ptr %"%iso_bus_4", align 8
  store i64 0, ptr %"%iso_bus_5", align 8
  store i64 0, ptr %"%iso_bus_6", align 8
  store i64 0, ptr %"%iso_bus_7", align 8
  store i64 0, ptr %"%iso_source_0", align 8
  store i64 0, ptr %"%iso_source_1", align 8
  store i64 0, ptr %"%iso_source_2", align 8
  store i64 0, ptr %"%iso_source_3", align 8
  store i64 0, ptr %"%iso_source_4", align 8
  store i64 0, ptr %"%iso_source_5", align 8
  store i64 0, ptr %"%iso_source_6", align 8
  store i64 0, ptr %"%iso_source_7", align 8
  store i64 0, ptr %"%iso_gsi_0", align 8
  store i64 0, ptr %"%iso_gsi_1", align 8
  store i64 0, ptr %"%iso_gsi_2", align 8
  store i64 0, ptr %"%iso_gsi_3", align 8
  store i64 0, ptr %"%iso_gsi_4", align 8
  store i64 0, ptr %"%iso_gsi_5", align 8
  store i64 0, ptr %"%iso_gsi_6", align 8
  store i64 0, ptr %"%iso_gsi_7", align 8
  store i64 0, ptr %"%iso_flags_0", align 8
  store i64 0, ptr %"%iso_flags_1", align 8
  store i64 0, ptr %"%iso_flags_2", align 8
  store i64 0, ptr %"%iso_flags_3", align 8
  store i64 0, ptr %"%iso_flags_4", align 8
  store i64 0, ptr %"%iso_flags_5", align 8
  store i64 0, ptr %"%iso_flags_6", align 8
  store i64 0, ptr %"%iso_flags_7", align 8
  store i64 0, ptr %"%iso_count", align 8
  store i64 0, ptr %"%acpi_table_addr_0", align 8
  store i64 0, ptr %"%acpi_table_addr_1", align 8
  store i64 0, ptr %"%acpi_table_addr_2", align 8
  store i64 0, ptr %"%acpi_table_addr_3", align 8
  store i64 0, ptr %"%acpi_table_addr_4", align 8
  store i64 0, ptr %"%acpi_table_addr_5", align 8
  store i64 0, ptr %"%acpi_table_addr_6", align 8
  store i64 0, ptr %"%acpi_table_addr_7", align 8
  store i64 0, ptr %"%acpi_table_addr_8", align 8
  store i64 0, ptr %"%acpi_table_addr_9", align 8
  store i64 0, ptr %"%acpi_table_addr_10", align 8
  store i64 0, ptr %"%acpi_table_addr_11", align 8
  store i64 0, ptr %"%acpi_table_addr_12", align 8
  store i64 0, ptr %"%acpi_table_addr_13", align 8
  store i64 0, ptr %"%acpi_table_addr_14", align 8
  store i64 0, ptr %"%acpi_table_addr_15", align 8
  store i64 0, ptr %"%acpi_table_sig0_0", align 8
  store i64 0, ptr %"%acpi_table_sig0_1", align 8
  store i64 0, ptr %"%acpi_table_sig0_2", align 8
  store i64 0, ptr %"%acpi_table_sig0_3", align 8
  store i64 0, ptr %"%acpi_table_sig0_4", align 8
  store i64 0, ptr %"%acpi_table_sig0_5", align 8
  store i64 0, ptr %"%acpi_table_sig0_6", align 8
  store i64 0, ptr %"%acpi_table_sig0_7", align 8
  store i64 0, ptr %"%acpi_table_sig0_8", align 8
  store i64 0, ptr %"%acpi_table_sig0_9", align 8
  store i64 0, ptr %"%acpi_table_sig0_10", align 8
  store i64 0, ptr %"%acpi_table_sig0_11", align 8
  store i64 0, ptr %"%acpi_table_sig0_12", align 8
  store i64 0, ptr %"%acpi_table_sig0_13", align 8
  store i64 0, ptr %"%acpi_table_sig0_14", align 8
  store i64 0, ptr %"%acpi_table_sig0_15", align 8
  store i64 0, ptr %"%acpi_table_count", align 8
  store i64 0, ptr %"%acpi_errors", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
