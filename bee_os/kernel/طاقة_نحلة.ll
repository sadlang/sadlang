; ModuleID = 'طاقة_نحلة'
source_filename = "\D8\B7\D8\A7\D9\82\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [17 x i8] c"0123456789ABCDEF\00", align 1
@str.const.1 = private unnamed_addr constant [28 x i8] c"[PM] RSDP \D8\B9\D8\AB\D8\B1 \D8\B9\D9\84\D9\8A\D9\87: \00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [23 x i8] c"[PM] \D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 ACPI: \00", align 1
@str.const.5 = private unnamed_addr constant [12 x i8] c"[PM] RSDT: \00", align 1
@str.const.7 = private unnamed_addr constant [19 x i8] c"[PM] RSDT \D8\B7\D9\88\D9\84: \00", align 1
@str.const.9 = private unnamed_addr constant [30 x i8] c"[PM] \D8\B9\D8\AF\D8\AF \D8\AC\D8\AF\D8\A7\D9\88\D9\84 ACPI: \00", align 1
@str.const.11 = private unnamed_addr constant [14 x i8] c"[PM]   FADT: \00", align 1
@str.const.13 = private unnamed_addr constant [14 x i8] c"[PM]   MADT: \00", align 1
@str.const.15 = private unnamed_addr constant [14 x i8] c"[PM]   HPET: \00", align 1
@str.const.17 = private unnamed_addr constant [14 x i8] c"[PM]   MCFG: \00", align 1
@str.const.19 = private unnamed_addr constant [30 x i8] c"[PM] FADT \D8\BA\D9\8A\D8\B1 \D9\85\D9\88\D8\AC\D9\88\D8\AF!\0A\00", align 1
@str.const.20 = private unnamed_addr constant [23 x i8] c"[PM] FADT \D8\AA\D8\AD\D9\84\D9\8A\D9\84:\0A\00", align 1
@str.const.21 = private unnamed_addr constant [14 x i8] c"[PM]   DSDT: \00", align 1
@str.const.23 = private unnamed_addr constant [18 x i8] c"[PM]   PM1a_CNT: \00", align 1
@str.const.25 = private unnamed_addr constant [16 x i8] c"[PM]   PM_TMR: \00", align 1
@str.const.27 = private unnamed_addr constant [17 x i8] c"[PM]   SCI IRQ: \00", align 1
@str.const.29 = private unnamed_addr constant [17 x i8] c"[PM]   SMI CMD: \00", align 1
@str.const.31 = private unnamed_addr constant [29 x i8] c"[PM] MADT \D8\BA\D9\8A\D8\B1 \D9\85\D9\88\D8\AC\D9\88\D8\AF\0A\00", align 1
@str.const.32 = private unnamed_addr constant [25 x i8] c"[PM] MADT: LAPIC \D8\B9\D9\86\D8\AF \00", align 1
@str.const.34 = private unnamed_addr constant [33 x i8] c"[PM] \D8\B9\D8\AF\D8\AF \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA: \00", align 1
@str.const.35 = private unnamed_addr constant [14 x i8] c"\D8\8C I/O APIC: \00", align 1
@str.const.37 = private unnamed_addr constant [13 x i8] c"[PM]   CPU #\00", align 1
@str.const.38 = private unnamed_addr constant [15 x i8] c" (\D9\85\D9\81\D8\B9\D9\91\D9\84)\0A\00", align 1
@str.const.39 = private unnamed_addr constant [18 x i8] c"[PM]   I/O APIC #\00", align 1
@str.const.40 = private unnamed_addr constant [9 x i8] c" \D8\B9\D9\86\D8\AF \00", align 1
@str.const.42 = private unnamed_addr constant [22 x i8] c"[PM]   IRQ Override: \00", align 1
@str.const.43 = private unnamed_addr constant [10 x i8] c" \E2\86\92 GSI \00", align 1
@str.const.45 = private unnamed_addr constant [70 x i8] c"[PM] SMI CMD port = 0 (ACPI \D9\82\D8\AF \D9\8A\D9\83\D9\88\D9\86 \D9\85\D9\81\D8\B9\D9\84\D8\A7\D9\8B \D8\A8\D8\A7\D9\84\D9\81\D8\B9\D9\84)\0A\00", align 1
@str.const.46 = private unnamed_addr constant [44 x i8] c"[PM] ACPI \D9\85\D9\81\D8\B9\D9\84 \D9\85\D8\B3\D8\A8\D9\82\D8\A7\D9\8B (SCI_EN=1)\0A\00", align 1
@str.const.47 = private unnamed_addr constant [25 x i8] c"[PM] \D8\AA\D9\81\D8\B9\D9\8A\D9\84 ACPI... \00", align 1
@str.const.48 = private unnamed_addr constant [9 x i8] c"\D9\81\D8\B4\D9\84!\0A\00", align 1
@str.const.49 = private unnamed_addr constant [9 x i8] c"\D9\86\D8\AC\D8\AD!\0A\00", align 1
@str.const.50 = private unnamed_addr constant [19 x i8] c"[PM] P-state \E2\86\92 P\00", align 1
@str.const.51 = private unnamed_addr constant [13 x i8] c" (\D8\AA\D8\B1\D8\AF\D8\AF: \00", align 1
@str.const.52 = private unnamed_addr constant [16 x i8] c" MHz\D8\8C \D8\AC\D9\87\D8\AF: \00", align 1
@str.const.53 = private unnamed_addr constant [6 x i8] c" mV)\0A\00", align 1
@str.const.54 = private unnamed_addr constant [36 x i8] c"[PM] \D8\AA\D8\AD\D8\B6\D9\8A\D8\B1 \D9\84\D9\84\D9\86\D9\88\D9\85 (S3)...\0A\00", align 1
@str.const.55 = private unnamed_addr constant [31 x i8] c"[PM] \D8\AA\D9\85 \D8\AD\D9\81\D8\B8 \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9\0A\00", align 1
@str.const.56 = private unnamed_addr constant [62 x i8] c"[PM] PM1a_CNT \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81 \E2\80\94 \D9\84\D8\A7 \D9\8A\D9\85\D9\83\D9\86 \D8\A7\D9\84\D9\86\D9\88\D9\85\0A\00", align 1
@str.const.57 = private unnamed_addr constant [21 x i8] c"[PM] \D8\AF\D8\AE\D9\88\D9\84 S3...\0A\00", align 1
@str.const.58 = private unnamed_addr constant [30 x i8] c"[PM] \D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8 \D9\85\D9\86 S3!\0A\00", align 1
@str.const.59 = private unnamed_addr constant [100 x i8] c"[PM] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.60 = private unnamed_addr constant [43 x i8] c"[PM]  \D8\A5\D9\8A\D9\82\D8\A7\D9\81 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\86\D8\AD\D9\84\D8\A9...\0A\00", align 1
@str.const.62 = private unnamed_addr constant [64 x i8] c"[PM] PM1a_CNT \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81 \E2\80\94 \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 QEMU shutdown\0A\00", align 1
@str.const.63 = private unnamed_addr constant [50 x i8] c"[PM] \D9\81\D8\B4\D9\84 \D8\A7\D9\84\D8\A5\D9\8A\D9\82\D8\A7\D9\81! \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 QEMU...\0A\00", align 1
@str.const.64 = private unnamed_addr constant [31 x i8] c"[PM] \D8\A5\D8\B9\D8\A7\D8\AF\D8\A9 \D8\AA\D8\B4\D8\BA\D9\8A\D9\84...\0A\00", align 1
@str.const.65 = private unnamed_addr constant [20 x i8] c"[PM] Triple fault!\0A\00", align 1
@str.const.66 = private unnamed_addr constant [51 x i8] c"\0A[PM] \E2\95\90\E2\95\90\E2\95\90 \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9 \E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.67 = private unnamed_addr constant [18 x i8] c"[PM] \D8\B3\D9\8A\D8\A7\D8\B3\D8\A9: \00", align 1
@str.const.68 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AF\D8\A7\D8\A1\00", align 1
@str.const.69 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\86\00", align 1
@str.const.70 = private unnamed_addr constant [11 x i8] c"\D8\AA\D9\88\D9\81\D9\8A\D8\B1\00", align 1
@str.const.71 = private unnamed_addr constant [13 x i8] c"\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9\00", align 1
@str.const.73 = private unnamed_addr constant [16 x i8] c"[PM] P-state: P\00", align 1
@str.const.74 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.75 = private unnamed_addr constant [8 x i8] c" MHz\D8\8C \00", align 1
@str.const.77 = private unnamed_addr constant [21 x i8] c"[PM] C-state CPU0: C\00", align 1
@str.const.79 = private unnamed_addr constant [31 x i8] c"[PM] \D8\B7\D8\A7\D9\82\D8\A9 \D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC: \00", align 1
@str.const.80 = private unnamed_addr constant [5 x i8] c" mW\0A\00", align 1
@str.const.81 = private unnamed_addr constant [33 x i8] c"[PM] \D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9 \D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9: \00", align 1
@str.const.83 = private unnamed_addr constant [26 x i8] c"[PM] \D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA C: \00", align 1
@str.const.84 = private unnamed_addr constant [7 x i8] c"\D8\8C P: \00", align 1
@str.const.86 = private unnamed_addr constant [25 x i8] c"[PM] \D8\A3\D8\AD\D8\AF\D8\A7\D8\AB \D9\86\D9\88\D9\85: \00", align 1
@str.const.87 = private unnamed_addr constant [20 x i8] c"\D8\8C \D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8: \00", align 1
@str.const.89 = private unnamed_addr constant [65 x i8] c"[PM] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\0A\00", align 1
@str.const.90 = private unnamed_addr constant [36 x i8] c"[PM] \D8\AA\D8\BA\D9\8A\D9\8A\D8\B1 \D8\A7\D9\84\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9 \E2\86\92 \00", align 1
@str.const.91 = private unnamed_addr constant [10 x i8] c"\D8\A3\D8\AF\D8\A7\D8\A1\0A\00", align 1
@str.const.92 = private unnamed_addr constant [14 x i8] c"\D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\86\0A\00", align 1
@str.const.93 = private unnamed_addr constant [12 x i8] c"\D8\AA\D9\88\D9\81\D9\8A\D8\B1\0A\00", align 1
@str.const.94 = private unnamed_addr constant [14 x i8] c"\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9\0A\00", align 1
@str.const.96 = private unnamed_addr constant [124 x i8] c"[PM] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.97 = private unnamed_addr constant [61 x i8] c"[PM]  \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9 \E2\80\94 BeeOS\0A\00", align 1
@str.const.99 = private unnamed_addr constant [54 x i8] c"[PM] RSDP \D8\BA\D9\8A\D8\B1 \D9\85\D9\88\D8\AC\D9\88\D8\AF \E2\80\94 ACPI \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D8\A7\D8\AD\0A\00", align 1
@str.const.100 = private unnamed_addr constant [51 x i8] c"[PM] \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9 \D9\8A\D8\B9\D9\85\D9\84 \D8\A8\D8\AF\D9\88\D9\86 ACPI\0A\00", align 1
@str.const.101 = private unnamed_addr constant [42 x i8] c"[PM] \E2\9C\93 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9 \D8\AC\D8\A7\D9\87\D8\B2\0A\0A\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_port_inl(i64)

declare void @sad_ll_port_outl(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_read64(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_mem_write64(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_kfree(i64)

declare void @sad_ll_interrupt_disable()

declare void @sad_ll_interrupt_enable()

define void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %arr.ptr.int = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %arr.len
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret void
}

define i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_9" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_8" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0" = alloca i64, align 8
  %"%\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

merge_11:                                         ; preds = %then_10, %merge_9
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load9" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load9", 2
  br i1 %cmpeqtmp10, label %then_12, label %merge_13

merge_13:                                         ; preds = %then_12, %merge_11
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load12" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load12", 3
  br i1 %cmpeqtmp13, label %then_14, label %merge_15

merge_15:                                         ; preds = %then_14, %merge_13
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load15" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load15", 4
  br i1 %cmpeqtmp16, label %then_16, label %merge_17

merge_17:                                         ; preds = %then_16, %merge_15
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load18" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp19 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load18", 5
  br i1 %cmpeqtmp19, label %then_18, label %merge_19

merge_19:                                         ; preds = %then_18, %merge_17
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load21" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp22 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load21", 6
  br i1 %cmpeqtmp22, label %then_20, label %merge_21

merge_21:                                         ; preds = %then_20, %merge_19
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load24" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp25 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load24", 7
  br i1 %cmpeqtmp25, label %then_22, label %merge_23

merge_23:                                         ; preds = %then_22, %merge_21
  %"%\D9\85\D8\A4\D9\82\D8\AA.load27" = load i64, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load28" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %subtmp29 = sub i64 %"%\D9\85\D8\A4\D9\82\D8\AA.load27", %"%\D8\A8\D8\A7\D9\82\D9\8A.load28"
  %divtmp = sdiv i64 %subtmp29, 10
  store i64 %divtmp, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load30" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load30", 1
  store i64 %addtmp, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  br label %while_cond_5

merge_28:                                         ; preds = %then_27, %while_body_25
  %"%\D8\B9.load33" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp34 = icmp eq i64 %"%\D8\B9.load33", 1
  br i1 %cmpeqtmp34, label %then_29, label %merge_30

merge_30:                                         ; preds = %then_29, %merge_28
  %"%\D8\B9.load35" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp36 = icmp eq i64 %"%\D8\B9.load35", 2
  br i1 %cmpeqtmp36, label %then_31, label %merge_32

merge_32:                                         ; preds = %then_31, %merge_30
  %"%\D8\B9.load37" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp38 = icmp eq i64 %"%\D8\B9.load37", 3
  br i1 %cmpeqtmp38, label %then_33, label %merge_34

merge_34:                                         ; preds = %then_33, %merge_32
  %"%\D8\B9.load39" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp40 = icmp eq i64 %"%\D8\B9.load39", 4
  br i1 %cmpeqtmp40, label %then_35, label %merge_36

merge_36:                                         ; preds = %then_35, %merge_34
  %"%\D8\B9.load41" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp42 = icmp eq i64 %"%\D8\B9.load41", 5
  br i1 %cmpeqtmp42, label %then_37, label %merge_38

merge_38:                                         ; preds = %then_37, %merge_36
  %"%\D8\B9.load43" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp44 = icmp eq i64 %"%\D8\B9.load43", 6
  br i1 %cmpeqtmp44, label %then_39, label %merge_40

merge_4:                                          ; preds = %entry
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  store i64 %"%\D9\86.load2", ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_8", align 8
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_9", align 8
  br label %while_cond_5

merge_40:                                         ; preds = %then_39, %merge_38
  %"%\D8\B9.load45" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp46 = icmp eq i64 %"%\D8\B9.load45", 7
  br i1 %cmpeqtmp46, label %then_41, label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %"%\D8\B9.load47" = load i64, ptr %"%\D8\B9", align 8
  %subtmp48 = sub i64 %"%\D8\B9.load47", 1
  store i64 %subtmp48, ptr %"%\D8\B9", align 8
  br label %while_cond_24

merge_9:                                          ; preds = %then_8, %while_body_6
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load6" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load6", 1
  br i1 %cmpeqtmp7, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load8" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load8", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  br label %merge_11

then_12:                                          ; preds = %merge_11
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load11" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load11", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  br label %merge_13

then_14:                                          ; preds = %merge_13
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load14" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load14", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  br label %merge_15

then_16:                                          ; preds = %merge_15
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load17" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load17", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  br label %merge_17

then_18:                                          ; preds = %merge_17
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load20" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load20", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  br label %merge_19

then_20:                                          ; preds = %merge_19
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load23" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load23", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  br label %merge_21

then_22:                                          ; preds = %merge_21
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load26" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load26", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  br label %merge_23

then_27:                                          ; preds = %while_body_25
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_28

then_29:                                          ; preds = %merge_28
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_30

then_3:                                           ; preds = %entry
  ret i64 0

then_31:                                          ; preds = %merge_30
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_32

then_33:                                          ; preds = %merge_32
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_34

then_35:                                          ; preds = %merge_34
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_36

then_37:                                          ; preds = %merge_36
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_38

then_39:                                          ; preds = %merge_38
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_40

then_41:                                          ; preds = %merge_40
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  br label %merge_42

then_8:                                           ; preds = %while_body_6
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  br label %merge_9

while_body_25:                                    ; preds = %while_cond_24
  store i64 0, ptr %"%\D8\B1\D9\82\D9\85_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\B9.load31" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp32 = icmp eq i64 %"%\D8\B9.load31", 0
  br i1 %cmpeqtmp32, label %then_27, label %merge_28

while_body_6:                                     ; preds = %while_cond_5
  %"%\D9\85\D8\A4\D9\82\D8\AA.load3" = load i64, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %modtmp = srem i64 %"%\D9\85\D8\A4\D9\82\D8\AA.load3", 10
  store i64 %modtmp, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load", 0
  br i1 %cmpeqtmp4, label %then_8, label %merge_9

while_cond_24:                                    ; preds = %merge_42, %while_exit_7
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9.load", 0
  br i1 %cmpgetmp, label %while_body_25, label %while_exit_26

while_cond_5:                                     ; preds = %merge_23, %merge_4
  %"%\D9\85\D8\A4\D9\82\D8\AA.load" = load i64, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\A4\D9\82\D8\AA.load", 0
  br i1 %cmpgttmp, label %while_body_6, label %while_exit_7

while_exit_26:                                    ; preds = %while_cond_24
  ret i64 0

while_exit_7:                                     ; preds = %while_cond_5
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load5" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %subtmp = sub i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load5", 1
  store i64 %subtmp, ptr %"%\D8\B9", align 8
  br label %while_cond_24
}

define i64 @pm_serial_hex(i64 %"\D9\86") {
entry:
  %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%\D8\AA\D9\85" = alloca i64, align 8
  %"%\D8\A3\D8\AD\D8\B1\D9\81" = alloca ptr, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  store ptr @str.const, ptr %"%\D8\A3\D8\AD\D8\B1\D9\81", align 8
  store i64 0, ptr %"%\D8\AA\D9\85", align 8
  store i64 28, ptr %"%\D8\B9", align 8
  br label %while_cond_43

merge_47:                                         ; preds = %then_46, %while_body_44
  %"%\D8\B9.load4" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\B9.load4", 24
  br i1 %cmpeqtmp5, label %then_48, label %merge_49

merge_49:                                         ; preds = %then_48, %merge_47
  %"%\D8\B9.load9" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D8\B9.load9", 20
  br i1 %cmpeqtmp10, label %then_50, label %merge_51

merge_51:                                         ; preds = %then_50, %merge_49
  %"%\D8\B9.load14" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D8\B9.load14", 16
  br i1 %cmpeqtmp15, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%\D8\B9.load19" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp20 = icmp eq i64 %"%\D8\B9.load19", 12
  br i1 %cmpeqtmp20, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %merge_53
  %"%\D8\B9.load24" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp25 = icmp eq i64 %"%\D8\B9.load24", 8
  br i1 %cmpeqtmp25, label %then_56, label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  %"%\D8\B9.load29" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp30 = icmp eq i64 %"%\D8\B9.load29", 4
  br i1 %cmpeqtmp30, label %then_58, label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  %"%\D8\B9.load34" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp35 = icmp eq i64 %"%\D8\B9.load34", 0
  br i1 %cmpeqtmp35, label %then_60, label %merge_61

merge_61:                                         ; preds = %then_60, %merge_59
  %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load" = load i64, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load", 0
  br i1 %cmpgttmp, label %then_62, label %merge_63

merge_63:                                         ; preds = %then_62, %merge_61
  %"%\D8\AA\D9\85.load38" = load i64, ptr %"%\D8\AA\D9\85", align 8
  %cmpeqtmp39 = icmp eq i64 %"%\D8\AA\D9\85.load38", 1
  br i1 %cmpeqtmp39, label %then_64, label %merge_65

merge_65:                                         ; preds = %then_64, %merge_63
  %"%\D8\B9.load40" = load i64, ptr %"%\D8\B9", align 8
  %subtmp = sub i64 %"%\D8\B9.load40", 4
  store i64 %subtmp, ptr %"%\D8\B9", align 8
  br label %while_cond_43

merge_67:                                         ; preds = %then_66, %while_exit_45
  ret i64 0

then_46:                                          ; preds = %while_body_44
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %shrtmp = lshr i64 %"%\D9\86.load", 28
  %andtmp = and i64 %shrtmp, 15
  store i64 %andtmp, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_47

then_48:                                          ; preds = %merge_47
  %"%\D9\86.load6" = load i64, ptr %"\D9\861", align 8
  %shrtmp7 = lshr i64 %"%\D9\86.load6", 24
  %andtmp8 = and i64 %shrtmp7, 15
  store i64 %andtmp8, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_49

then_50:                                          ; preds = %merge_49
  %"%\D9\86.load11" = load i64, ptr %"\D9\861", align 8
  %shrtmp12 = lshr i64 %"%\D9\86.load11", 20
  %andtmp13 = and i64 %shrtmp12, 15
  store i64 %andtmp13, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%\D9\86.load16" = load i64, ptr %"\D9\861", align 8
  %shrtmp17 = lshr i64 %"%\D9\86.load16", 16
  %andtmp18 = and i64 %shrtmp17, 15
  store i64 %andtmp18, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%\D9\86.load21" = load i64, ptr %"\D9\861", align 8
  %shrtmp22 = lshr i64 %"%\D9\86.load21", 12
  %andtmp23 = and i64 %shrtmp22, 15
  store i64 %andtmp23, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%\D9\86.load26" = load i64, ptr %"\D9\861", align 8
  %shrtmp27 = lshr i64 %"%\D9\86.load26", 8
  %andtmp28 = and i64 %shrtmp27, 15
  store i64 %andtmp28, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %shrtmp32 = lshr i64 %"%\D9\86.load31", 4
  %andtmp33 = and i64 %shrtmp32, 15
  store i64 %andtmp33, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%\D9\86.load36" = load i64, ptr %"\D9\861", align 8
  %andtmp37 = and i64 %"%\D9\86.load36", 15
  store i64 %andtmp37, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_61

then_62:                                          ; preds = %merge_61
  store i64 1, ptr %"%\D8\AA\D9\85", align 8
  br label %merge_63

then_64:                                          ; preds = %merge_63
  br label %merge_65

then_66:                                          ; preds = %while_exit_45
  br label %merge_67

while_body_44:                                    ; preds = %while_cond_43
  store i64 0, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  %"%\D8\B9.load2" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9.load2", 28
  br i1 %cmpeqtmp, label %then_46, label %merge_47

while_cond_43:                                    ; preds = %merge_65, %entry
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9.load", 0
  br i1 %cmpgetmp, label %while_body_44, label %while_exit_45

while_exit_45:                                    ; preds = %while_cond_43
  %"%\D8\AA\D9\85.load" = load i64, ptr %"%\D8\AA\D9\85", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AA\D9\85.load", 0
  br i1 %cmpeqtmp3, label %then_66, label %merge_67
}

define i64 @"acpi_\D8\A7\D8\A8\D8\AD\D8\AB_rsdp_\D9\81\D9\8A_\D9\86\D8\B7\D8\A7\D9\82"(i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB") {
entry:
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9" = alloca i64, align 8
  %"%sig_hi" = alloca i64, align 8
  %"%sig_lo" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB2" = alloca i64, align 8
  store i64 %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB", ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB2", align 8
  %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  %"%131.load" = load i64, ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  store i64 %"%131.load", ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  br label %while_cond_68

merge_72:                                         ; preds = %merge_74, %while_body_69
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load6" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load6", 16
  store i64 %addtmp7, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  br label %while_cond_68

merge_74:                                         ; preds = %merge_79, %then_71
  br label %merge_72

merge_79:                                         ; preds = %while_exit_77
  br label %merge_74

then_71:                                          ; preds = %while_body_69
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%sig_hi", align 8
  %"%sig_hi.load" = load i64, ptr %"%sig_hi", align 8
  %cmpeqtmp5 = icmp eq i64 %"%sig_hi.load", 0
  br i1 %cmpeqtmp5, label %then_73, label %merge_74

then_73:                                          ; preds = %then_71
  store i64 0, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_75

then_78:                                          ; preds = %while_exit_77
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load17" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %calltmp = call i64 @pm_serial_hex(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load17")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %loadtmp = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 %loadtmp

while_body_69:                                    ; preds = %while_cond_68
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  call void @sad_ll_mem_read32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3")
  store i64 0, ptr %"%sig_lo", align 8
  %"%sig_lo.load" = load i64, ptr %"%sig_lo", align 8
  %cmpeqtmp = icmp eq i64 %"%sig_lo.load", 0
  br i1 %cmpeqtmp, label %then_71, label %merge_72

while_body_76:                                    ; preds = %while_cond_75
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load9" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\A8.load10" = load i64, ptr %"%\D8\A8", align 8
  %addtmp11 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load9", %"%\D8\A8.load10"
  call void @sad_ll_mem_read8(i64 %addtmp11)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load" = load i64, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  %addtmp12 = add i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load", 0
  store i64 %addtmp12, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  %"%\D8\A8.load13" = load i64, ptr %"%\D8\A8", align 8
  %addtmp14 = add i64 %"%\D8\A8.load13", 1
  store i64 %addtmp14, ptr %"%\D8\A8", align 8
  br label %while_cond_75

while_cond_68:                                    ; preds = %merge_72, %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB.load" = load i64, ptr %"\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", %"%\D9\86\D9\87\D8\A7\D9\8A\D8\A9_\D8\A8\D8\AD\D8\AB.load"
  br i1 %cmplttmp, label %while_body_69, label %while_exit_70

while_cond_75:                                    ; preds = %while_body_76, %then_73
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %cmplttmp8 = icmp slt i64 %"%\D8\A8.load", 20
  br i1 %cmplttmp8, label %while_body_76, label %while_exit_77

while_exit_70:                                    ; preds = %while_cond_68
  ret i64 0

while_exit_77:                                    ; preds = %while_cond_75
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load15" = load i64, ptr %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9", align 8
  %andtmp = and i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load15", 255
  %cmpeqtmp16 = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp16, label %then_78, label %merge_79
}

define i64 @"acpi_\D8\A7\D8\A8\D8\AD\D8\AB_rsdp"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%ebda_addr" = alloca i64, align 8
  %"%ebda_seg" = alloca i64, align 8
  call void @sad_ll_mem_read16(i64 1038)
  store i64 0, ptr %"%ebda_seg", align 8
  %"%ebda_seg.load" = load i64, ptr %"%ebda_seg", align 8
  %shltmp = shl i64 %"%ebda_seg.load", 4
  store i64 %shltmp, ptr %"%ebda_addr", align 8
  %"%ebda_addr.load" = load i64, ptr %"%ebda_addr", align 8
  %cmpgttmp = icmp sgt i64 %"%ebda_addr.load", 0
  br i1 %cmpgttmp, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_83, %entry
  %calltmp4 = call i64 @"acpi_\D8\A7\D8\A8\D8\AD\D8\AB_rsdp_\D9\81\D9\8A_\D9\86\D8\B7\D8\A7\D9\82"(i64 917504, i64 1048576)
  ret i64 %calltmp4

merge_83:                                         ; preds = %then_80
  br label %merge_81

then_80:                                          ; preds = %entry
  %"%ebda_addr.load1" = load i64, ptr %"%ebda_addr", align 8
  %addtmp = add i64 %"%ebda_addr.load1", 1024
  %"%ebda_addr.load2" = load i64, ptr %"%ebda_addr", align 8
  %calltmp = call i64 @"acpi_\D8\A7\D8\A8\D8\AD\D8\AB_rsdp_\D9\81\D9\8A_\D9\86\D8\B7\D8\A7\D9\82"(i64 %"%ebda_addr.load2", i64 %addtmp)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpgttmp3 = icmp sgt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpgttmp3, label %then_82, label %merge_83

then_82:                                          ; preds = %then_80
  %loadtmp = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  ret i64 %loadtmp
}

define i64 @"acpi_\D8\AD\D9\84\D9\84_rsdp"(i64 %rsdp) {
entry:
  %rsdp1 = alloca i64, align 8
  store i64 %rsdp, ptr %rsdp1, align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  ret i64 0
}

define i64 @"acpi_\D8\AD\D9\84\D9\84_rsdt"(i64 %rsdt) {
entry:
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84" = alloca i64, align 8
  %rsdt1 = alloca i64, align 8
  store i64 %rsdt, ptr %rsdt1, align 8
  %"%rsdt.load" = load i64, ptr %rsdt1, align 8
  %addtmp = add i64 %"%rsdt.load", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load" = load i64, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  %calltmp = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load2" = load i64, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load2", 36
  %shrtmp = lshr i64 %subtmp, 2
  store i64 %shrtmp, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84", align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84", align 8
  %calltmp3 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_84

merge_88:                                         ; preds = %then_87, %while_body_85
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load11" = load i64, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load11", 0
  br i1 %cmpeqtmp12, label %then_89, label %merge_90

merge_90:                                         ; preds = %then_89, %merge_88
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load15" = load i64, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load15", 0
  br i1 %cmpeqtmp16, label %then_91, label %merge_92

merge_92:                                         ; preds = %then_91, %merge_90
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load19" = load i64, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9", align 8
  %cmpeqtmp20 = icmp eq i64 %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load19", 0
  br i1 %cmpeqtmp20, label %then_93, label %merge_94

merge_94:                                         ; preds = %then_93, %merge_92
  %"%\D8\B7.load23" = load i64, ptr %"%\D8\B7", align 8
  %addtmp24 = add i64 %"%\D8\B7.load23", 1
  store i64 %addtmp24, ptr %"%\D8\B7", align 8
  br label %while_cond_84

then_87:                                          ; preds = %while_body_85
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load9" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84", align 8
  %calltmp10 = call i64 @pm_serial_hex(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load9")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_88

then_89:                                          ; preds = %merge_88
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load13" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84", align 8
  %calltmp14 = call i64 @pm_serial_hex(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load13")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_90

then_91:                                          ; preds = %merge_90
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load17" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84", align 8
  %calltmp18 = call i64 @pm_serial_hex(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load17")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_92

then_93:                                          ; preds = %merge_92
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load21" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84", align 8
  %calltmp22 = call i64 @pm_serial_hex(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load21")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_94

while_body_85:                                    ; preds = %while_cond_84
  %"%rsdt.load5" = load i64, ptr %rsdt1, align 8
  %addtmp6 = add i64 %"%rsdt.load5", 36
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %shltmp = shl i64 %"%\D8\B7.load7", 2
  %addtmp8 = add i64 %addtmp6, %shltmp
  call void @sad_ll_mem_read32(i64 %addtmp8)
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84", align 8
  call void @sad_ll_mem_read32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AC\D8\AF\D9\88\D9\84.load")
  store i64 0, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9", align 8
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load" = load i64, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load", 0
  br i1 %cmpeqtmp, label %then_87, label %merge_88

while_cond_84:                                    ; preds = %merge_94, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84.load4" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84.load4"
  br i1 %cmplttmp, label %while_body_85, label %while_exit_86

while_exit_86:                                    ; preds = %while_cond_84
  %loadtmp = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AF\D8\A7\D8\AE\D9\84", align 8
  ret i64 %loadtmp
}

define i64 @"acpi_\D8\AD\D9\84\D9\84_fadt"(i64 %fadt) {
entry:
  %fadt1 = alloca i64, align 8
  store i64 %fadt, ptr %fadt1, align 8
  %"%fadt.load" = load i64, ptr %fadt1, align 8
  %cmpeqtmp = icmp eq i64 %"%fadt.load", 0
  br i1 %cmpeqtmp, label %then_95, label %merge_96

merge_96:                                         ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  ret i64 1

then_95:                                          ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  ret i64 0
}

define i64 @"acpi_\D8\AD\D9\84\D9\84_madt"(i64 %madt) {
entry:
  %"%gsi" = alloca i64, align 8
  %"%\D9\85\D8\B5\D8\AF\D8\B1_irq" = alloca i64, align 8
  %"%gsi_base" = alloca i64, align 8
  %"%ioapic_addr" = alloca i64, align 8
  %"%ioapic_id" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85" = alloca i64, align 8
  %"%apic_id" = alloca i64, align 8
  %"%proc_id" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84" = alloca i64, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_io_apic" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85" = alloca i64, align 8
  %"%lapic_addr" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84" = alloca i64, align 8
  %madt1 = alloca i64, align 8
  store i64 %madt, ptr %madt1, align 8
  %"%madt.load" = load i64, ptr %madt1, align 8
  %cmpeqtmp = icmp eq i64 %"%madt.load", 0
  br i1 %cmpeqtmp, label %then_97, label %merge_98

else_114:                                         ; preds = %merge_111
  %"%410.load" = load i64, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  store i64 %"%410.load", ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  br label %merge_113

merge_103:                                        ; preds = %then_102, %while_body_100
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load", 0
  br i1 %cmpeqtmp14, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_107, %merge_103
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load22" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load22", 1
  br i1 %cmpeqtmp23, label %then_108, label %merge_109

merge_107:                                        ; preds = %then_106, %then_104
  br label %merge_105

merge_109:                                        ; preds = %then_108, %merge_105
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load37" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %cmpeqtmp38 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load37", 2
  br i1 %cmpeqtmp38, label %then_110, label %merge_111

merge_111:                                        ; preds = %then_110, %merge_109
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load46" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load46", 0
  br i1 %cmpgttmp, label %then_112, label %else_114

merge_113:                                        ; preds = %else_114, %then_112
  br label %while_cond_99

merge_98:                                         ; preds = %entry
  %"%madt.load2" = load i64, ptr %madt1, align 8
  %addtmp = add i64 %"%madt.load2", 4
  call void @sad_ll_mem_read32(i64 %addtmp)
  store i64 0, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  %"%madt.load3" = load i64, ptr %madt1, align 8
  %addtmp4 = add i64 %"%madt.load3", 36
  call void @sad_ll_mem_read32(i64 %addtmp4)
  store i64 0, ptr %"%lapic_addr", align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  %"%lapic_addr.load" = load i64, ptr %"%lapic_addr", align 8
  %calltmp = call i64 @pm_serial_hex(i64 %"%lapic_addr.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%madt.load5" = load i64, ptr %madt1, align 8
  %addtmp6 = add i64 %"%madt.load5", 40
  call void @sad_ll_mem_read32(i64 %addtmp6)
  store i64 0, ptr %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_io_apic", align 8
  store i64 44, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  br label %while_cond_99

then_102:                                         ; preds = %while_body_100
  %"%322.load" = load i64, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  store i64 %"%322.load", ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  br label %merge_103

then_104:                                         ; preds = %merge_103
  %"%madt.load15" = load i64, ptr %madt1, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9.load16" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %addtmp17 = add i64 %"%madt.load15", %"%\D9\85\D9\88\D9\82\D8\B9.load16"
  %addtmp18 = add i64 %addtmp17, 2
  call void @sad_ll_mem_read8(i64 %addtmp18)
  store i64 0, ptr %"%proc_id", align 8
  %addtmp19 = add i64 %addtmp17, 3
  call void @sad_ll_mem_read8(i64 %addtmp19)
  store i64 0, ptr %"%apic_id", align 8
  %addtmp20 = add i64 %addtmp17, 4
  call void @sad_ll_mem_read32(i64 %addtmp20)
  store i64 0, ptr %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85", align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85.load" = load i64, ptr %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85", align 8
  %andtmp = and i64 %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85.load", 1
  %cmpeqtmp21 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp21, label %then_106, label %merge_107

then_106:                                         ; preds = %then_104
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA.load24" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA", align 8
  %addtmp25 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA.load24", 1
  store i64 %addtmp25, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA", align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %"%apic_id.load" = load i64, ptr %"%apic_id", align 8
  %calltmp26 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%apic_id.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  br label %merge_107

then_108:                                         ; preds = %merge_105
  %"%madt.load27" = load i64, ptr %madt1, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9.load28" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %addtmp29 = add i64 %"%madt.load27", %"%\D9\85\D9\88\D9\82\D8\B9.load28"
  %addtmp30 = add i64 %addtmp29, 2
  call void @sad_ll_mem_read8(i64 %addtmp30)
  store i64 0, ptr %"%ioapic_id", align 8
  %addtmp31 = add i64 %addtmp29, 4
  call void @sad_ll_mem_read32(i64 %addtmp31)
  store i64 0, ptr %"%ioapic_addr", align 8
  %addtmp32 = add i64 %addtmp29, 8
  call void @sad_ll_mem_read32(i64 %addtmp32)
  store i64 0, ptr %"%gsi_base", align 8
  %"%\D8\B9\D8\AF\D8\AF_io_apic.load33" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_io_apic", align 8
  %addtmp34 = add i64 %"%\D8\B9\D8\AF\D8\AF_io_apic.load33", 1
  store i64 %addtmp34, ptr %"%\D8\B9\D8\AF\D8\AF_io_apic", align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %"%ioapic_id.load" = load i64, ptr %"%ioapic_id", align 8
  %calltmp35 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%ioapic_id.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %"%ioapic_addr.load" = load i64, ptr %"%ioapic_addr", align 8
  %calltmp36 = call i64 @pm_serial_hex(i64 %"%ioapic_addr.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_109

then_110:                                         ; preds = %merge_109
  %"%madt.load39" = load i64, ptr %madt1, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9.load40" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %addtmp41 = add i64 %"%madt.load39", %"%\D9\85\D9\88\D9\82\D8\B9.load40"
  %addtmp42 = add i64 %addtmp41, 3
  call void @sad_ll_mem_read8(i64 %addtmp42)
  store i64 0, ptr %"%\D9\85\D8\B5\D8\AF\D8\B1_irq", align 8
  %addtmp43 = add i64 %addtmp41, 4
  call void @sad_ll_mem_read32(i64 %addtmp43)
  store i64 0, ptr %"%gsi", align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  %"%\D9\85\D8\B5\D8\AF\D8\B1_irq.load" = load i64, ptr %"%\D9\85\D8\B5\D8\AF\D8\B1_irq", align 8
  %calltmp44 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D8\B5\D8\AF\D8\B1_irq.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  %"%gsi.load" = load i64, ptr %"%gsi", align 8
  %calltmp45 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%gsi.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_111

then_112:                                         ; preds = %merge_111
  %"%\D9\85\D9\88\D9\82\D8\B9.load47" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load48" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %addtmp49 = add i64 %"%\D9\85\D9\88\D9\82\D8\B9.load47", %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load48"
  store i64 %addtmp49, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  br label %merge_113

then_97:                                          ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  ret i64 0

while_body_100:                                   ; preds = %while_cond_99
  %"%madt.load7" = load i64, ptr %madt1, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9.load8" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %addtmp9 = add i64 %"%madt.load7", %"%\D9\85\D9\88\D9\82\D8\B9.load8"
  call void @sad_ll_mem_read8(i64 %addtmp9)
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %addtmp10 = add i64 %addtmp9, 1
  call void @sad_ll_mem_read8(i64 %addtmp10)
  store i64 0, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load", 0
  br i1 %cmpeqtmp11, label %then_102, label %merge_103

while_cond_99:                                    ; preds = %merge_113, %merge_98
  %"%\D9\85\D9\88\D9\82\D8\B9.load" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load" = load i64, ptr %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D9\88\D9\82\D8\B9.load", %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load"
  br i1 %cmplttmp, label %while_body_100, label %while_exit_101

while_exit_101:                                   ; preds = %while_cond_99
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA", align 8
  %calltmp12 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %"%\D8\B9\D8\AF\D8\AF_io_apic.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_io_apic", align 8
  %calltmp13 = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B9\D8\AF\D8\AF_io_apic.load")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %loadtmp = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA", align 8
  ret i64 %loadtmp
}

define i64 @"acpi_\D9\81\D8\B9\D9\84"() {
entry:
  %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%pm1a_val" = alloca i64, align 8
  br i1 false, label %then_115, label %merge_116

merge_116:                                        ; preds = %entry
  br i1 true, label %then_117, label %merge_118

merge_118:                                        ; preds = %merge_116
  store i64 0, ptr %"%pm1a_val", align 8
  %"%pm1a_val.load" = load i64, ptr %"%pm1a_val", align 8
  %andtmp = and i64 %"%pm1a_val.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_119, label %merge_120

merge_120:                                        ; preds = %merge_118
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_121

merge_125:                                        ; preds = %while_body_122
  store i64 0, ptr %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1", align 8
  br label %while_cond_126

then_115:                                         ; preds = %entry
  ret i64 1

then_117:                                         ; preds = %merge_116
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  ret i64 1

then_119:                                         ; preds = %merge_118
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  ret i64 1

then_124:                                         ; preds = %while_body_122
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.49 to i64))
  ret i64 1

while_body_122:                                   ; preds = %while_cond_121
  store i64 0, ptr %"%pm1a_val", align 8
  %"%pm1a_val.load1" = load i64, ptr %"%pm1a_val", align 8
  %andtmp2 = and i64 %"%pm1a_val.load1", 1
  %cmpeqtmp3 = icmp eq i64 %andtmp2, 1
  br i1 %cmpeqtmp3, label %then_124, label %merge_125

while_body_127:                                   ; preds = %while_cond_126
  %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1.load5" = load i64, ptr %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1", align 8
  %addtmp = add i64 %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1.load5", 1
  store i64 %addtmp, ptr %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1", align 8
  br label %while_cond_126

while_cond_121:                                   ; preds = %while_exit_128, %merge_120
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 300
  br i1 %cmplttmp, label %while_body_122, label %while_exit_123

while_cond_126:                                   ; preds = %while_body_127, %merge_125
  %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1.load" = load i64, ptr %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1", align 8
  %cmplttmp4 = icmp slt i64 %"%\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1.load", 10000
  br i1 %cmplttmp4, label %while_body_127, label %while_exit_128

while_exit_123:                                   ; preds = %while_cond_121
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  ret i64 0

while_exit_128:                                   ; preds = %while_cond_126
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load6" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp7 = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load6", 1
  store i64 %addtmp7, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_121
}

define i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c1"() {
entry:
  ret i64 0
}

define i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c2"() {
entry:
  %"%p_lvl2" = alloca i64, align 8
  br i1 false, label %then_129, label %merge_130

merge_130:                                        ; preds = %merge_132, %entry
  ret i64 0

merge_132:                                        ; preds = %then_131, %then_129
  br label %merge_130

then_129:                                         ; preds = %entry
  call void @sad_ll_mem_read32(i64 116)
  store i64 0, ptr %"%p_lvl2", align 8
  %"%p_lvl2.load" = load i64, ptr %"%p_lvl2", align 8
  %cmpgttmp = icmp sgt i64 %"%p_lvl2.load", 0
  br i1 %cmpgttmp, label %then_131, label %merge_132

then_131:                                         ; preds = %then_129
  %"%p_lvl2.load1" = load i64, ptr %"%p_lvl2", align 8
  call void @sad_ll_port_inb(i64 %"%p_lvl2.load1")
  br label %merge_132
}

define i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c3"() {
entry:
  %"%p_lvl3" = alloca i64, align 8
  br i1 false, label %then_133, label %merge_134

merge_134:                                        ; preds = %merge_136, %entry
  ret i64 0

merge_136:                                        ; preds = %then_135, %then_133
  br label %merge_134

then_133:                                         ; preds = %entry
  call void @sad_ll_mem_read32(i64 120)
  store i64 0, ptr %"%p_lvl3", align 8
  %"%p_lvl3.load" = load i64, ptr %"%p_lvl3", align 8
  %cmpgttmp = icmp sgt i64 %"%p_lvl3.load", 0
  br i1 %cmpgttmp, label %then_135, label %merge_136

then_135:                                         ; preds = %then_133
  %"%p_lvl3.load1" = load i64, ptr %"%p_lvl3", align 8
  call void @sad_ll_port_inb(i64 %"%p_lvl3.load1")
  br label %merge_136
}

define i64 @"pm_\D8\A7\D8\AE\D8\AA\D8\B1_c_state"() {
entry:
  %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86.load" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load", 0
  store i64 %subtmp, ptr %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84", align 8
  %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84.load" = load i64, ptr %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84.load", 100
  br i1 %cmplttmp, label %then_137, label %merge_138

merge_138:                                        ; preds = %entry
  %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84.load1" = load i64, ptr %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84", align 8
  %cmplttmp2 = icmp slt i64 %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84.load1", 1000
  br i1 %cmplttmp2, label %then_139, label %merge_140

merge_140:                                        ; preds = %merge_138
  br i1 true, label %then_141, label %merge_142

merge_142:                                        ; preds = %merge_140
  %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84.load4" = load i64, ptr %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84", align 8
  %cmplttmp5 = icmp slt i64 %"%\D9\85\D8\AF\D8\A9_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84.load4", 10000
  br i1 %cmplttmp5, label %then_143, label %merge_144

merge_144:                                        ; preds = %merge_142
  %calltmp7 = call i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c3"()
  ret i64 3

then_137:                                         ; preds = %entry
  ret i64 0

then_139:                                         ; preds = %merge_138
  %calltmp = call i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c1"()
  ret i64 1

then_141:                                         ; preds = %merge_140
  %calltmp3 = call i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c1"()
  ret i64 1

then_143:                                         ; preds = %merge_142
  %calltmp6 = call i64 @"pm_\D8\A7\D8\AF\D8\AE\D9\84_c2"()
  ret i64 2
}

define i64 @"pm_\D8\A7\D8\AD\D8\B3\D8\A8_\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B3\D8\AA\D9\87\D8\AF\D9\81"(i64 %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC") {
entry:
  %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1" = alloca i64, align 8
  store i64 %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC", ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  br i1 true, label %then_145, label %merge_146

merge_146:                                        ; preds = %entry
  br i1 true, label %then_147, label %merge_148

merge_148:                                        ; preds = %merge_146
  br i1 true, label %then_153, label %merge_154

merge_150:                                        ; preds = %then_147
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load2" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp3 = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load2", 50
  br i1 %cmpgttmp3, label %then_151, label %merge_152

merge_152:                                        ; preds = %merge_150
  ret i64 0

merge_154:                                        ; preds = %merge_148
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load6" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp7 = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load6", 70
  br i1 %cmpgttmp7, label %then_157, label %merge_158

merge_156:                                        ; preds = %then_153
  ret i64 0

merge_158:                                        ; preds = %merge_154
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load8" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp9 = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load8", 40
  br i1 %cmpgttmp9, label %then_159, label %merge_160

merge_160:                                        ; preds = %merge_158
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load10" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp11 = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load10", 10
  br i1 %cmpgttmp11, label %then_161, label %merge_162

merge_162:                                        ; preds = %merge_160
  ret i64 0

then_145:                                         ; preds = %entry
  ret i64 0

then_147:                                         ; preds = %merge_146
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load", 80
  br i1 %cmpgttmp, label %then_149, label %merge_150

then_149:                                         ; preds = %then_147
  ret i64 0

then_151:                                         ; preds = %merge_150
  ret i64 0

then_153:                                         ; preds = %merge_148
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load4" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp5 = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load4", 90
  br i1 %cmpgttmp5, label %then_155, label %merge_156

then_155:                                         ; preds = %then_153
  ret i64 0

then_157:                                         ; preds = %merge_154
  ret i64 0

then_159:                                         ; preds = %merge_158
  ret i64 0

then_161:                                         ; preds = %merge_160
  ret i64 0
}

define i64 @"pm_\D8\B7\D8\A8\D9\82_p_state"(i64 %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8") {
entry:
  %"%p_new" = alloca i64, align 8
  %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81" = alloca i64, align 8
  store i64 %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8", ptr %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_163, label %merge_164

else_167:                                         ; preds = %merge_164
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load3" = load i64, ptr %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp4 = icmp sge i64 %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load3", 0
  br i1 %cmpgetmp4, label %then_168, label %else_170

else_170:                                         ; preds = %else_167
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load5" = load i64, ptr %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp6 = icmp sge i64 %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load5", 0
  br i1 %cmpgetmp6, label %then_171, label %else_173

else_173:                                         ; preds = %else_170
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load7" = load i64, ptr %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp8 = icmp sge i64 %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load7", 0
  br i1 %cmpgetmp8, label %then_174, label %else_176

else_176:                                         ; preds = %else_173
  store i64 4, ptr %"%p_new", align 8
  br label %merge_175

merge_164:                                        ; preds = %entry
  store i64 0, ptr %"%p_new", align 8
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load2" = load i64, ptr %"\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load2", 0
  br i1 %cmpgetmp, label %then_165, label %else_167

merge_166:                                        ; preds = %merge_169, %then_165
  %"%p_new.load" = load i64, ptr %"%p_new", align 8
  %cmpnetmp = icmp ne i64 %"%p_new.load", 0
  br i1 %cmpnetmp, label %then_177, label %merge_178

merge_169:                                        ; preds = %merge_172, %then_168
  br label %merge_166

merge_172:                                        ; preds = %merge_175, %then_171
  br label %merge_169

merge_175:                                        ; preds = %else_176, %then_174
  br label %merge_172

merge_178:                                        ; preds = %merge_188, %merge_166
  %loadtmp = load i64, ptr %"%p_new", align 8
  ret i64 %loadtmp

merge_180:                                        ; preds = %then_179, %then_177
  %"%p_new.load11" = load i64, ptr %"%p_new", align 8
  %cmpeqtmp12 = icmp eq i64 %"%p_new.load11", 1
  br i1 %cmpeqtmp12, label %then_181, label %merge_182

merge_182:                                        ; preds = %then_181, %merge_180
  %"%p_new.load13" = load i64, ptr %"%p_new", align 8
  %cmpeqtmp14 = icmp eq i64 %"%p_new.load13", 2
  br i1 %cmpeqtmp14, label %then_183, label %merge_184

merge_184:                                        ; preds = %then_183, %merge_182
  %"%p_new.load15" = load i64, ptr %"%p_new", align 8
  %cmpeqtmp16 = icmp eq i64 %"%p_new.load15", 3
  br i1 %cmpeqtmp16, label %then_185, label %merge_186

merge_186:                                        ; preds = %then_185, %merge_184
  %"%p_new.load17" = load i64, ptr %"%p_new", align 8
  %cmpeqtmp18 = icmp eq i64 %"%p_new.load17", 4
  br i1 %cmpeqtmp18, label %then_187, label %merge_188

merge_188:                                        ; preds = %then_187, %merge_186
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.50 to i64))
  %"%p_new.load19" = load i64, ptr %"%p_new", align 8
  %calltmp = call i64 @"pm_serial_\D8\B1\D9\82\D9\85"(i64 %"%p_new.load19")
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.51 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  br label %merge_178

then_163:                                         ; preds = %entry
  ret i64 0

then_165:                                         ; preds = %merge_164
  store i64 0, ptr %"%p_new", align 8
  br label %merge_166

then_168:                                         ; preds = %else_167
  store i64 1, ptr %"%p_new", align 8
  br label %merge_169

then_171:                                         ; preds = %else_170
  store i64 2, ptr %"%p_new", align 8
  br label %merge_172

then_174:                                         ; preds = %else_173
  store i64 3, ptr %"%p_new", align 8
  br label %merge_175

then_177:                                         ; preds = %merge_166
  %"%p_new.load9" = load i64, ptr %"%p_new", align 8
  %cmpeqtmp10 = icmp eq i64 %"%p_new.load9", 0
  br i1 %cmpeqtmp10, label %then_179, label %merge_180

then_179:                                         ; preds = %then_177
  br label %merge_180

then_181:                                         ; preds = %merge_180
  br label %merge_182

then_183:                                         ; preds = %merge_182
  br label %merge_184

then_185:                                         ; preds = %merge_184
  br label %merge_186

then_187:                                         ; preds = %merge_186
  br label %merge_188
}

define i64 @"pm_\D8\AD\D8\B6\D8\B1_\D9\84\D9\84\D9\86\D9\88\D9\85"() {
entry:
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  call void @sad_ll_interrupt_disable()
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.55 to i64))
  ret i64 1
}

define i64 @"pm_\D9\86\D9\85_s3"() {
entry:
  %"%val_a" = alloca i64, align 8
  %calltmp = call i64 @"pm_\D8\AD\D8\B6\D8\B1_\D9\84\D9\84\D9\86\D9\88\D9\85"()
  br i1 true, label %then_189, label %merge_190

merge_190:                                        ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  store i64 0, ptr %"%val_a", align 8
  br i1 false, label %then_191, label %merge_192

merge_192:                                        ; preds = %then_191, %merge_190
  call void @sad_ll_interrupt_enable()
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  ret i64 1

then_189:                                         ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @sad_ll_interrupt_enable()
  ret i64 0

then_191:                                         ; preds = %merge_190
  br label %merge_192
}

define i64 @"pm_\D8\A3\D8\B7\D9\81\D8\A6"() {
entry:
  %"%val" = alloca i64, align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.59 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.60 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.59 to i64))
  call void @sad_ll_interrupt_disable()
  br i1 true, label %then_193, label %merge_194

merge_194:                                        ; preds = %entry
  store i64 0, ptr %"%val", align 8
  br i1 false, label %then_195, label %merge_196

merge_196:                                        ; preds = %then_195, %merge_194
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.63 to i64))
  call void @sad_ll_port_outw(i64 1540, i64 8192)
  br label %while_cond_197

then_193:                                         ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.62 to i64))
  call void @sad_ll_port_outw(i64 1540, i64 8192)
  call void @sad_ll_port_outw(i64 46888, i64 8193)
  ret i64 0

then_195:                                         ; preds = %merge_194
  br label %merge_196

while_body_198:                                   ; preds = %while_cond_197
  br label %while_cond_197

while_cond_197:                                   ; preds = %while_body_198, %merge_196
  br i1 true, label %while_body_198, label %while_exit_199

while_exit_199:                                   ; preds = %while_cond_197
  ret i64 0
}

define void @"pm_\D8\A3\D8\B9\D8\AF_\D8\A7\D9\84\D8\AA\D8\B4\D8\BA\D9\8A\D9\84"() {
entry:
  %"%reset_val" = alloca i64, align 8
  %"%reset_reg" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9" = alloca i64, align 8
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.64 to i64))
  call void @sad_ll_interrupt_disable()
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  br label %while_cond_200

merge_204:                                        ; preds = %then_203, %while_body_201
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load1" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load1", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  br label %while_cond_200

merge_206:                                        ; preds = %merge_208, %while_exit_202
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.65 to i64))
  br label %while_cond_211

merge_208:                                        ; preds = %merge_210, %then_205
  br label %merge_206

merge_210:                                        ; preds = %then_209, %then_207
  br label %merge_208

then_203:                                         ; preds = %while_body_201
  store i64 1000, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  br label %merge_204

then_205:                                         ; preds = %while_exit_202
  br i1 false, label %then_207, label %merge_208

then_207:                                         ; preds = %then_205
  call void @sad_ll_mem_read64(i64 128)
  store i64 0, ptr %"%reset_reg", align 8
  call void @sad_ll_mem_read8(i64 136)
  store i64 0, ptr %"%reset_val", align 8
  %"%reset_reg.load" = load i64, ptr %"%reset_reg", align 8
  %cmpgttmp = icmp sgt i64 %"%reset_reg.load", 0
  br i1 %cmpgttmp, label %then_209, label %merge_210

then_209:                                         ; preds = %then_207
  %"%reset_reg.load2" = load i64, ptr %"%reset_reg", align 8
  %"%reset_val.load" = load i64, ptr %"%reset_val", align 8
  call void @sad_ll_port_outb(i64 %"%reset_reg.load2", i64 %"%reset_val.load")
  br label %merge_210

while_body_201:                                   ; preds = %while_cond_200
  call void @sad_ll_port_inb(i64 100)
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84", align 8
  %"%\D8\AD\D8\A7\D9\84.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84", align 8
  %shrtmp = lshr i64 %"%\D8\AD\D8\A7\D9\84.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_203, label %merge_204

while_body_212:                                   ; preds = %while_cond_211
  br label %while_cond_211

while_cond_200:                                   ; preds = %merge_204, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load", 1000
  br i1 %cmplttmp, label %while_body_201, label %while_exit_202

while_cond_211:                                   ; preds = %while_body_212, %merge_206
  br i1 true, label %while_body_212, label %while_exit_213

while_exit_202:                                   ; preds = %while_cond_200
  call void @sad_ll_port_outb(i64 100, i64 254)
  br i1 false, label %then_205, label %merge_206

while_exit_213:                                   ; preds = %while_cond_211
  ret void
}

define i64 @"pm_\D9\82\D8\AF\D8\B1_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D9\87\D9\84\D8\A7\D9\83"() {
entry:
  br i1 true, label %then_214, label %merge_215

merge_215:                                        ; preds = %then_214, %entry
  br i1 false, label %then_216, label %merge_217

merge_217:                                        ; preds = %then_216, %merge_215
  br i1 false, label %then_218, label %merge_219

merge_219:                                        ; preds = %then_218, %merge_217
  br i1 false, label %then_220, label %merge_221

merge_221:                                        ; preds = %then_220, %merge_219
  br i1 false, label %then_222, label %merge_223

merge_223:                                        ; preds = %then_222, %merge_221
  br i1 false, label %then_224, label %merge_225

merge_225:                                        ; preds = %then_224, %merge_223
  br i1 false, label %then_226, label %merge_227

merge_227:                                        ; preds = %then_226, %merge_225
  br i1 false, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %merge_227
  ret i64 0

then_214:                                         ; preds = %entry
  br label %merge_215

then_216:                                         ; preds = %merge_215
  br label %merge_217

then_218:                                         ; preds = %merge_217
  br label %merge_219

then_220:                                         ; preds = %merge_219
  br label %merge_221

then_222:                                         ; preds = %merge_221
  br label %merge_223

then_224:                                         ; preds = %merge_223
  br label %merge_225

then_226:                                         ; preds = %merge_225
  br label %merge_227

then_228:                                         ; preds = %merge_227
  br label %merge_229
}

define i64 @"pm_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"pm_\D9\82\D8\AF\D8\B1_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D9\87\D9\84\D8\A7\D9\83"()
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.66 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.67 to i64))
  br i1 true, label %then_230, label %merge_231

merge_231:                                        ; preds = %then_230, %entry
  br i1 false, label %then_232, label %merge_233

merge_233:                                        ; preds = %then_232, %merge_231
  br i1 false, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %merge_233
  br i1 false, label %then_236, label %merge_237

merge_237:                                        ; preds = %then_236, %merge_235
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.73 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.74 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.75 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.77 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.79 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.80 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.81 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.80 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.83 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.84 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.86 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.87 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.89 to i64))
  ret i64 0

then_230:                                         ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.68 to i64))
  br label %merge_231

then_232:                                         ; preds = %merge_231
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.69 to i64))
  br label %merge_233

then_234:                                         ; preds = %merge_233
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.70 to i64))
  br label %merge_235

then_236:                                         ; preds = %merge_235
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.71 to i64))
  br label %merge_237
}

define i64 @"pm_\D8\BA\D9\8A\D8\B1_\D8\A7\D9\84\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9"(i64 %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9") {
entry:
  %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp, label %then_238, label %merge_239

merge_239:                                        ; preds = %entry
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load2" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load2", 3
  br i1 %cmpgttmp, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.90 to i64))
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load3" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load3", 0
  br i1 %cmpeqtmp, label %then_242, label %merge_243

merge_243:                                        ; preds = %then_242, %merge_241
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load4" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load4", 1
  br i1 %cmpeqtmp5, label %then_244, label %merge_245

merge_245:                                        ; preds = %then_244, %merge_243
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load6" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load6", 2
  br i1 %cmpeqtmp7, label %then_246, label %merge_247

merge_247:                                        ; preds = %then_246, %merge_245
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load8" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load8", 3
  br i1 %cmpeqtmp9, label %then_248, label %merge_249

merge_249:                                        ; preds = %then_248, %merge_247
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load10" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load10", 0
  br i1 %cmpeqtmp11, label %then_250, label %merge_251

merge_251:                                        ; preds = %then_250, %merge_249
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load12" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load12", 0
  br i1 %cmpeqtmp13, label %then_252, label %merge_253

merge_253:                                        ; preds = %then_252, %merge_251
  %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load14" = load i64, ptr %"\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load14", 0
  br i1 %cmpeqtmp15, label %then_254, label %merge_255

merge_255:                                        ; preds = %then_254, %merge_253
  ret i64 1

then_238:                                         ; preds = %entry
  ret i64 0

then_240:                                         ; preds = %merge_239
  ret i64 0

then_242:                                         ; preds = %merge_241
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.91 to i64))
  br label %merge_243

then_244:                                         ; preds = %merge_243
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.92 to i64))
  br label %merge_245

then_246:                                         ; preds = %merge_245
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.93 to i64))
  br label %merge_247

then_248:                                         ; preds = %merge_247
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.94 to i64))
  br label %merge_249

then_250:                                         ; preds = %merge_249
  br label %merge_251

then_252:                                         ; preds = %merge_251
  %calltmp = call i64 @"pm_\D8\B7\D8\A8\D9\82_p_state"(i64 0)
  br label %merge_253

then_254:                                         ; preds = %merge_253
  %calltmp16 = call i64 @"pm_\D8\B7\D8\A8\D9\82_p_state"(i64 0)
  br label %merge_255
}

define i64 @"pm_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\A4\D9\82\D8\AA"() {
entry:
  br i1 true, label %then_256, label %merge_257

merge_257:                                        ; preds = %entry
  ret i64 0

then_256:                                         ; preds = %entry
  call void @sad_ll_timer_get_ticks()
  ret i64 0
}

define i64 @"pm_\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_us"(i64 %"\D9\85\D9\8A\D9\83\D8\B1\D9\88") {
entry:
  %"%\D9\81\D8\A7\D8\B1\D9\82" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86_t" = alloca i64, align 8
  %"%start" = alloca i64, align 8
  %"%ticks" = alloca i64, align 8
  %"%\D8\B9\D8\AF" = alloca i64, align 8
  %"\D9\85\D9\8A\D9\83\D8\B1\D9\881" = alloca i64, align 8
  store i64 %"\D9\85\D9\8A\D9\83\D8\B1\D9\88", ptr %"\D9\85\D9\8A\D9\83\D8\B1\D9\881", align 8
  br i1 true, label %then_258, label %merge_259

merge_259:                                        ; preds = %entry
  %"%\D9\85\D9\8A\D9\83\D8\B1\D9\88.load2" = load i64, ptr %"\D9\85\D9\8A\D9\83\D8\B1\D9\881", align 8
  %multmp3 = mul i64 %"%\D9\85\D9\8A\D9\83\D8\B1\D9\88.load2", 3
  %"%\D9\85\D9\8A\D9\83\D8\B1\D9\88.load4" = load i64, ptr %"\D9\85\D9\8A\D9\83\D8\B1\D9\881", align 8
  %multmp5 = mul i64 %"%\D9\85\D9\8A\D9\83\D8\B1\D9\88.load4", 579
  %divtmp = sdiv i64 %multmp5, 1000
  %addtmp = add i64 %multmp3, %divtmp
  store i64 %addtmp, ptr %"%ticks", align 8
  store i64 0, ptr %"%start", align 8
  br label %while_cond_263

merge_267:                                        ; preds = %then_266, %while_body_264
  %"%\D9\81\D8\A7\D8\B1\D9\82.load10" = load i64, ptr %"%\D9\81\D8\A7\D8\B1\D9\82", align 8
  %"%ticks.load" = load i64, ptr %"%ticks", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D8\A7\D8\B1\D9\82.load10", %"%ticks.load"
  br i1 %cmpgetmp, label %then_268, label %merge_269

merge_269:                                        ; preds = %merge_267
  br label %while_cond_263

then_258:                                         ; preds = %entry
  %"%\D9\85\D9\8A\D9\83\D8\B1\D9\88.load" = load i64, ptr %"\D9\85\D9\8A\D9\83\D8\B1\D9\881", align 8
  %multmp = mul i64 %"%\D9\85\D9\8A\D9\83\D8\B1\D9\88.load", 10
  store i64 %multmp, ptr %"%\D8\B9\D8\AF", align 8
  br label %while_cond_260

then_266:                                         ; preds = %while_body_264
  %"%\D9\81\D8\A7\D8\B1\D9\82.load8" = load i64, ptr %"%\D9\81\D8\A7\D8\B1\D9\82", align 8
  %addtmp9 = add i64 %"%\D9\81\D8\A7\D8\B1\D9\82.load8", 16777216
  store i64 %addtmp9, ptr %"%\D9\81\D8\A7\D8\B1\D9\82", align 8
  br label %merge_267

then_268:                                         ; preds = %merge_267
  ret i64 0

while_body_261:                                   ; preds = %while_cond_260
  %"%\D8\B9\D8\AF.load6" = load i64, ptr %"%\D8\B9\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF.load6", 1
  store i64 %subtmp, ptr %"%\D8\B9\D8\AF", align 8
  br label %while_cond_260

while_body_264:                                   ; preds = %while_cond_263
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86_t", align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86_t.load" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86_t", align 8
  %"%start.load" = load i64, ptr %"%start", align 8
  %subtmp7 = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86_t.load", %"%start.load"
  store i64 %subtmp7, ptr %"%\D9\81\D8\A7\D8\B1\D9\82", align 8
  %"%\D9\81\D8\A7\D8\B1\D9\82.load" = load i64, ptr %"%\D9\81\D8\A7\D8\B1\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D8\A7\D8\B1\D9\82.load", 0
  br i1 %cmplttmp, label %then_266, label %merge_267

while_cond_260:                                   ; preds = %while_body_261, %then_258
  %"%\D8\B9\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF.load", 0
  br i1 %cmpgttmp, label %while_body_261, label %while_exit_262

while_cond_263:                                   ; preds = %merge_269, %merge_259
  br i1 true, label %while_body_264, label %while_exit_265

while_exit_262:                                   ; preds = %while_cond_260
  ret i64 0

while_exit_265:                                   ; preds = %while_cond_263
  ret i64 0
}

define i64 @"pm_\D9\87\D9\8A\D8\A6"() {
entry:
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.96 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.97 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.96 to i64))
  br i1 true, label %then_270, label %merge_271

merge_271:                                        ; preds = %entry
  br i1 false, label %then_272, label %merge_273

merge_273:                                        ; preds = %then_272, %merge_271
  br i1 false, label %then_274, label %merge_275

merge_275:                                        ; preds = %then_274, %merge_273
  br i1 false, label %then_276, label %merge_277

merge_277:                                        ; preds = %then_276, %merge_275
  %calltmp = call i64 @"acpi_\D9\81\D8\B9\D9\84"()
  %calltmp1 = call i64 @"pm_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"()
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.101 to i64))
  ret i64 1

then_270:                                         ; preds = %entry
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.99 to i64))
  call void @"pm_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.100 to i64))
  ret i64 0

then_272:                                         ; preds = %merge_271
  br label %merge_273

then_274:                                         ; preds = %merge_273
  br label %merge_275

then_276:                                         ; preds = %merge_275
  br label %merge_277
}

define i64 @"pm_\D8\AF\D9\88\D8\B1\D8\A9"(i64 %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC") {
entry:
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8" = alloca i64, align 8
  %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1" = alloca i64, align 8
  store i64 %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC", ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load", 5
  br i1 %cmpgttmp, label %then_278, label %merge_279

merge_279:                                        ; preds = %then_278, %entry
  %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load2" = load i64, ptr %"\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC1", align 8
  %calltmp = call i64 @"pm_\D8\A7\D8\AD\D8\B3\D8\A8_\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B3\D8\AA\D9\87\D8\AF\D9\81"(i64 %"%\D8\AD\D9\85\D9\84_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC.load2")
  store i64 %calltmp, ptr %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8", align 8
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8", align 8
  %calltmp3 = call i64 @"pm_\D8\B7\D8\A8\D9\82_p_state"(i64 %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D9\85\D8\B7\D9\84\D9\88\D8\A8.load")
  %calltmp4 = call i64 @"pm_\D9\82\D8\AF\D8\B1_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D9\87\D9\84\D8\A7\D9\83"()
  ret i64 0

then_278:                                         ; preds = %entry
  br label %merge_279
}

define void @__sad_main() {
entry:
  %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c3" = alloca i64, align 8
  %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c2" = alloca i64, align 8
  %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c1" = alloca i64, align 8
  %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c0" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D9\86\D9\88\D9\85" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA_p" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA_c" = alloca i64, align 8
  %"%\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_mw" = alloca i64, align 8
  %"%\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_mw" = alloca i64, align 8
  %"%\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC_mw" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_mw" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\A9_\D8\AE\D9\85\D9\88\D9\84_\D9\82\D8\A8\D9\84_\D9\86\D9\88\D9\85" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\A7\D8\AF_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84" = alloca i64, align 8
  %"%\D9\88\D9\82\D8\AA_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A" = alloca i64, align 8
  %"%p_state_\D9\85\D8\B3\D8\AA\D9\87\D8\AF\D9\81" = alloca i64, align 8
  %"%p_state_\D8\A3\D9\82\D8\B5\D9\89" = alloca i64, align 8
  %"%p_state_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_7" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_6" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_5" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_4" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_3" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_2" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_1" = alloca i64, align 8
  %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_0" = alloca i64, align 8
  %"%acpi_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%acpi_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\B7\D8\A7\D9\82\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%sci_int" = alloca i64, align 8
  %"%smi_cmd_port" = alloca i64, align 8
  %"%acpi_disable_cmd" = alloca i64, align 8
  %"%acpi_enable_cmd" = alloca i64, align 8
  %"%century_reg" = alloca i64, align 8
  %"%slp_typa_s3" = alloca i64, align 8
  %"%slp_typb_s5" = alloca i64, align 8
  %"%slp_typa_s5" = alloca i64, align 8
  %"%pm_tmr_len" = alloca i64, align 8
  %"%pm_tmr_blk" = alloca i64, align 8
  %"%pm1a_evt_blk" = alloca i64, align 8
  %"%pm1b_cnt_blk" = alloca i64, align 8
  %"%pm1a_cnt_blk" = alloca i64, align 8
  %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_acpi" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_mcfg" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_hpet" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dsdt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_madt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_fadt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_rsdt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_rsdp" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_PM_TMR" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_PM1a_EVT" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_PM1b_CNT" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_PM1a_CNT" = alloca i64, align 8
  %"%PM1_SCI_EN" = alloca i64, align 8
  %"%PM1_SLP_TYP_S5" = alloca i64, align 8
  %"%PM1_SLP_TYP_S3" = alloca i64, align 8
  %"%PM1_SLP_TYP_S0" = alloca i64, align 8
  %"%PM1_SLP_EN" = alloca i64, align 8
  %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\AA\D9\88\D9\81\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\86" = alloca i64, align 8
  %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\A3\D8\AF\D8\A7\D8\A1" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_P4" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_P3" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_P2" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_P1" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_P0" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_C3" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_C2" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_C1" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_C0" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_S5" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_S4" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_S3" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_S2" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_S1" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_S0" = alloca i64, align 8
  %"%SIG_WAET" = alloca i64, align 8
  %"%SIG_BGRT" = alloca i64, align 8
  %"%SIG_MCFG" = alloca i64, align 8
  %"%SIG_HPET" = alloca i64, align 8
  %"%SIG_SSDT" = alloca i64, align 8
  %"%SIG_DSDT" = alloca i64, align 8
  %"%SIG_APIC" = alloca i64, align 8
  %"%SIG_FACP" = alloca i64, align 8
  %"%RSDP_SIG_HI" = alloca i64, align 8
  %"%RSDP_SIG_LO" = alloca i64, align 8
  store i64 541348690, ptr %"%RSDP_SIG_LO", align 8
  store i64 542266448, ptr %"%RSDP_SIG_HI", align 8
  store i64 1346584902, ptr %"%SIG_FACP", align 8
  store i64 1128878145, ptr %"%SIG_APIC", align 8
  store i64 1413763908, ptr %"%SIG_DSDT", align 8
  store i64 1413763923, ptr %"%SIG_SSDT", align 8
  store i64 1413828680, ptr %"%SIG_HPET", align 8
  store i64 1195787085, ptr %"%SIG_MCFG", align 8
  store i64 1414678338, ptr %"%SIG_BGRT", align 8
  store i64 1413824855, ptr %"%SIG_WAET", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_S0", align 8
  store i64 1, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_S1", align 8
  store i64 2, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_S2", align 8
  store i64 3, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_S3", align 8
  store i64 4, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_S4", align 8
  store i64 5, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_S5", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_C0", align 8
  store i64 1, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_C1", align 8
  store i64 2, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_C2", align 8
  store i64 3, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_C3", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_P0", align 8
  store i64 1, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_P1", align 8
  store i64 2, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_P2", align 8
  store i64 3, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_P3", align 8
  store i64 4, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_P4", align 8
  store i64 0, ptr %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\A3\D8\AF\D8\A7\D8\A1", align 8
  store i64 1, ptr %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D9\85\D8\AA\D9\88\D8\A7\D8\B2\D9\86", align 8
  store i64 2, ptr %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\AA\D9\88\D9\81\D9\8A\D8\B1", align 8
  store i64 3, ptr %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9", align 8
  store i64 8192, ptr %"%PM1_SLP_EN", align 8
  store i64 0, ptr %"%PM1_SLP_TYP_S0", align 8
  store i64 5120, ptr %"%PM1_SLP_TYP_S3", align 8
  store i64 13312, ptr %"%PM1_SLP_TYP_S5", align 8
  store i64 1, ptr %"%PM1_SCI_EN", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D9\81\D8\B0_PM1a_CNT", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D9\81\D8\B0_PM1b_CNT", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D9\81\D8\B0_PM1a_EVT", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D9\81\D8\B0_PM_TMR", align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 64, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 128, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 16, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_rsdp", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_rsdt", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_fadt", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_madt", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dsdt", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_hpet", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_mcfg", align 8
  store i64 0, ptr %"%\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1_acpi", align 8
  store i64 0, ptr %"%pm1a_cnt_blk", align 8
  store i64 0, ptr %"%pm1b_cnt_blk", align 8
  store i64 0, ptr %"%pm1a_evt_blk", align 8
  store i64 0, ptr %"%pm_tmr_blk", align 8
  store i64 0, ptr %"%pm_tmr_len", align 8
  store i64 0, ptr %"%slp_typa_s5", align 8
  store i64 0, ptr %"%slp_typb_s5", align 8
  store i64 0, ptr %"%slp_typa_s3", align 8
  store i64 0, ptr %"%century_reg", align 8
  store i64 0, ptr %"%acpi_enable_cmd", align 8
  store i64 0, ptr %"%acpi_disable_cmd", align 8
  store i64 0, ptr %"%smi_cmd_port", align 8
  store i64 0, ptr %"%sci_int", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\B7\D8\A7\D9\82\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%\D8\B3\D9\8A\D8\A7\D8\B3\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%acpi_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 0, ptr %"%acpi_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_0", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_1", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_2", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_3", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_4", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_5", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_6", align 8
  store i64 0, ptr %"%c_state_\D9\84\D9\83\D9\84_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_7", align 8
  store i64 0, ptr %"%p_state_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%p_state_\D8\A3\D9\82\D8\B5\D9\89", align 8
  store i64 0, ptr %"%p_state_\D9\85\D8\B3\D8\AA\D9\87\D8\AF\D9\81", align 8
  store i64 3000, ptr %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A", align 8
  store i64 3000, ptr %"%\D8\AA\D8\B1\D8\AF\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 1200, ptr %"%\D8\AC\D9\87\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF_\D8\A7\D9\84\D8\AE\D9\85\D9\88\D9\84", align 8
  store i64 300000, ptr %"%\D9\85\D8\AF\D8\A9_\D8\AE\D9\85\D9\88\D9\84_\D9\82\D8\A8\D9\84_\D9\86\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9_mw", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D9\85\D8\B9\D8\A7\D9\84\D8\AC_mw", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_mw", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_\D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_mw", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA_c", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA_p", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D9\86\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\A7\D8\B3\D8\AA\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c0", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c1", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c2", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D9\81\D9\8A_c3", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
