; ModuleID = 'شبكة_نحلة'
source_filename = "\D8\B4\D8\A8\D9\83\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [35 x i8] c"[NET] Scanning PCI bus for NIC...\0A\00", align 1
@str.const.1 = private unnamed_addr constant [30 x i8] c"[NET] No supported NIC found\0A\00", align 1
@str.const.2 = private unnamed_addr constant [28 x i8] c"[NET] Found RTL8139 at PCI \00", align 1
@str.const.3 = private unnamed_addr constant [26 x i8] c"[NET] Found E1000 at PCI \00", align 1
@str.const.4 = private unnamed_addr constant [16 x i8] c"[RTL] IO Base: \00", align 1
@str.const.5 = private unnamed_addr constant [29 x i8] c"[NET] Bus mastering enabled\0A\00", align 1
@str.const.6 = private unnamed_addr constant [22 x i8] c"[RTL] Reset timeout!\0A\00", align 1
@str.const.7 = private unnamed_addr constant [22 x i8] c"[RTL] Reset complete\0A\00", align 1
@str.const.8 = private unnamed_addr constant [12 x i8] c"[RTL] MAC: \00", align 1
@str.const.9 = private unnamed_addr constant [37 x i8] c"[RTL] Failed to allocate RX buffer!\0A\00", align 1
@str.const.10 = private unnamed_addr constant [37 x i8] c"[RTL] Failed to allocate TX buffer!\0A\00", align 1
@str.const.11 = private unnamed_addr constant [29 x i8] c"[RTL] Buffers allocated: RX=\00", align 1
@str.const.12 = private unnamed_addr constant [6 x i8] c" TX0=\00", align 1
@str.const.13 = private unnamed_addr constant [31 x i8] c"[RTL] Initializing RTL8139...\0A\00", align 1
@str.const.14 = private unnamed_addr constant [30 x i8] c"[RTL] RTL8139 initialized OK\0A\00", align 1
@str.const.15 = private unnamed_addr constant [20 x i8] c"[E1000] MMIO Base: \00", align 1
@str.const.16 = private unnamed_addr constant [24 x i8] c"[E1000] Reset complete\0A\00", align 1
@str.const.17 = private unnamed_addr constant [14 x i8] c"[E1000] MAC: \00", align 1
@str.const.18 = private unnamed_addr constant [44 x i8] c"[E1000] Failed to allocate RX descriptors!\0A\00", align 1
@str.const.19 = private unnamed_addr constant [44 x i8] c"[E1000] Failed to allocate TX descriptors!\0A\00", align 1
@str.const.20 = private unnamed_addr constant [35 x i8] c"[E1000] Descriptors allocated: RX=\00", align 1
@str.const.21 = private unnamed_addr constant [5 x i8] c" TX=\00", align 1
@str.const.22 = private unnamed_addr constant [23 x i8] c"[E1000] RX configured\0A\00", align 1
@str.const.23 = private unnamed_addr constant [23 x i8] c"[E1000] TX configured\0A\00", align 1
@str.const.24 = private unnamed_addr constant [31 x i8] c"[E1000] Initializing E1000...\0A\00", align 1
@str.const.25 = private unnamed_addr constant [28 x i8] c"[E1000] Invalid MMIO base!\0A\00", align 1
@str.const.26 = private unnamed_addr constant [20 x i8] c"[E1000] Link is UP\0A\00", align 1
@str.const.27 = private unnamed_addr constant [22 x i8] c"[E1000] Link is DOWN\0A\00", align 1
@str.const.28 = private unnamed_addr constant [30 x i8] c"[E1000] E1000 initialized OK\0A\00", align 1
@str.const.29 = private unnamed_addr constant [34 x i8] c"[NET] === Network Statistics ===\0A\00", align 1
@str.const.30 = private unnamed_addr constant [17 x i8] c"[NET] NIC Type: \00", align 1
@str.const.31 = private unnamed_addr constant [8 x i8] c"RTL8139\00", align 1
@str.const.32 = private unnamed_addr constant [6 x i8] c"E1000\00", align 1
@str.const.33 = private unnamed_addr constant [5 x i8] c"None\00", align 1
@str.const.34 = private unnamed_addr constant [12 x i8] c"[NET] MAC: \00", align 1
@str.const.35 = private unnamed_addr constant [11 x i8] c"[NET] IP: \00", align 1
@str.const.36 = private unnamed_addr constant [16 x i8] c"[NET] Gateway: \00", align 1
@str.const.37 = private unnamed_addr constant [11 x i8] c"[NET] TX: \00", align 1
@str.const.38 = private unnamed_addr constant [8 x i8] c" pkts, \00", align 1
@str.const.39 = private unnamed_addr constant [8 x i8] c" bytes\0A\00", align 1
@str.const.40 = private unnamed_addr constant [11 x i8] c"[NET] RX: \00", align 1
@str.const.43 = private unnamed_addr constant [18 x i8] c"[NET] Errors: TX=\00", align 1
@str.const.44 = private unnamed_addr constant [5 x i8] c" RX=\00", align 1
@str.const.45 = private unnamed_addr constant [13 x i8] c"[NET] Link: \00", align 1
@str.const.46 = private unnamed_addr constant [3 x i8] c"UP\00", align 1
@str.const.47 = private unnamed_addr constant [5 x i8] c"DOWN\00", align 1
@str.const.48 = private unnamed_addr constant [35 x i8] c"[NET] === Network System Init ===\0A\00", align 1
@str.const.49 = private unnamed_addr constant [44 x i8] c"[NET] No NIC found \E2\80\94 networking disabled\0A\00", align 1
@str.const.50 = private unnamed_addr constant [24 x i8] c"[NET] NIC init failed!\0A\00", align 1
@str.const.51 = private unnamed_addr constant [22 x i8] c"[NET] Network ready: \00", align 1
@str.const.52 = private unnamed_addr constant [4 x i8] c" @ \00", align 1
@str.const.53 = private unnamed_addr constant [27 x i8] c"[NET] ARP packet received\0A\00", align 1
@str.const.54 = private unnamed_addr constant [26 x i8] c"[NET] IP packet received\0A\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_port_inl(i64)

declare void @sad_ll_port_outl(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_kfree(i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

define i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  ret i64 0
}

define i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%\D8\B1\D9\82\D9\85\D9\8A" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%\D9\82\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86.load", 0
  br i1 %cmplttmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86.load2", 0
  br i1 %cmpeqtmp, label %then_2, label %merge_3

merge_3:                                          ; preds = %merge_1
  store i64 0, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %"%8.load" = load i64, ptr %"\D9\861", align 8
  store i64 %"%8.load", ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  br label %while_cond_4

then_0:                                           ; preds = %entry
  br label %merge_1

then_2:                                           ; preds = %merge_1
  ret i64 0

while_body_11:                                    ; preds = %while_cond_10
  %"%\D9\86.load10" = load i64, ptr %"\D9\861", align 8
  %"%\D9\82\D8\A7\D8\B3\D9\85.load11" = load i64, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  %divtmp12 = sdiv i64 %"%\D9\86.load10", %"%\D9\82\D8\A7\D8\B3\D9\85.load11"
  %modtmp = srem i64 %divtmp12, 10
  store i64 %modtmp, ptr %"%\D8\B1\D9\82\D9\85\D9\8A", align 8
  %"%\D9\82\D8\A7\D8\B3\D9\85.load13" = load i64, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  %divtmp14 = sdiv i64 %"%\D9\82\D8\A7\D8\B3\D9\85.load13", 10
  store i64 %divtmp14, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  br label %while_cond_10

while_body_5:                                     ; preds = %while_cond_4
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %addtmp = add i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load", 1
  store i64 %addtmp, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %"%\D9\85\D8\A4\D9\82\D8\AA.load3" = load i64, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %divtmp = sdiv i64 %"%\D9\85\D8\A4\D9\82\D8\AA.load3", 10
  store i64 %divtmp, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  br label %while_cond_4

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\82\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  %multmp = mul i64 %"%\D9\82\D8\A7\D8\B3\D9\85.load", 10
  store i64 %multmp, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  %"%\D8\B9.load6" = load i64, ptr %"%\D8\B9", align 8
  %addtmp7 = add i64 %"%\D8\B9.load6", 1
  store i64 %addtmp7, ptr %"%\D8\B9", align 8
  br label %while_cond_7

while_cond_10:                                    ; preds = %while_body_11, %while_exit_9
  %"%\D9\82\D8\A7\D8\B3\D9\85.load8" = load i64, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  %cmpgttmp9 = icmp sgt i64 %"%\D9\82\D8\A7\D8\B3\D9\85.load8", 0
  br i1 %cmpgttmp9, label %while_body_11, label %while_exit_12

while_cond_4:                                     ; preds = %while_body_5, %merge_3
  %"%\D9\85\D8\A4\D9\82\D8\AA.load" = load i64, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\A4\D9\82\D8\AA.load", 0
  br i1 %cmpgttmp, label %while_body_5, label %while_exit_6

while_cond_7:                                     ; preds = %while_body_8, %while_exit_6
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load4" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %cmplttmp5 = icmp slt i64 %"%\D8\B9.load", %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85.load4"
  br i1 %cmplttmp5, label %while_body_8, label %while_exit_9

while_exit_12:                                    ; preds = %while_cond_10
  ret i64 0

while_exit_6:                                     ; preds = %while_cond_4
  store i64 1, ptr %"%\D9\82\D8\A7\D8\B3\D9\85", align 8
  store i64 1, ptr %"%\D8\B9", align 8
  br label %while_cond_7

while_exit_9:                                     ; preds = %while_cond_7
  br label %while_cond_10
}

define i64 @net_serial_hex8(i64 %"\D9\86") {
entry:
  %"%\D8\B3\D9\81\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %shrtmp = lshr i64 %"%\D9\86.load", 4
  %andtmp = and i64 %shrtmp, 15
  store i64 %andtmp, ptr %"%\D8\B9\D8\A7\D9\84\D9\8A", align 8
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  %andtmp3 = and i64 %"%\D9\86.load2", 15
  store i64 %andtmp3, ptr %"%\D8\B3\D9\81\D9\84\D9\8A", align 8
  %"%\D8\B9\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\B9\D8\A7\D9\84\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\A7\D9\84\D9\8A.load", 10
  br i1 %cmplttmp, label %then_13, label %merge_14

merge_14:                                         ; preds = %then_13, %entry
  %"%\D8\B9\D8\A7\D9\84\D9\8A.load4" = load i64, ptr %"%\D8\B9\D8\A7\D9\84\D9\8A", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D8\A7\D9\84\D9\8A.load4", 10
  br i1 %cmpgetmp, label %then_15, label %merge_16

merge_16:                                         ; preds = %then_15, %merge_14
  %"%\D8\B3\D9\81\D9\84\D9\8A.load" = load i64, ptr %"%\D8\B3\D9\81\D9\84\D9\8A", align 8
  %cmplttmp5 = icmp slt i64 %"%\D8\B3\D9\81\D9\84\D9\8A.load", 10
  br i1 %cmplttmp5, label %then_17, label %merge_18

merge_18:                                         ; preds = %then_17, %merge_16
  %"%\D8\B3\D9\81\D9\84\D9\8A.load6" = load i64, ptr %"%\D8\B3\D9\81\D9\84\D9\8A", align 8
  %cmpgetmp7 = icmp sge i64 %"%\D8\B3\D9\81\D9\84\D9\8A.load6", 10
  br i1 %cmpgetmp7, label %then_19, label %merge_20

merge_20:                                         ; preds = %then_19, %merge_18
  ret i64 0

then_13:                                          ; preds = %entry
  br label %merge_14

then_15:                                          ; preds = %merge_14
  br label %merge_16

then_17:                                          ; preds = %merge_16
  br label %merge_18

then_19:                                          ; preds = %merge_18
  br label %merge_20
}

define i64 @net_serial_mac() {
entry:
  ret i64 0
}

define i64 @net_serial_ip(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  %"%a.load" = load i64, ptr %a1, align 8
  %calltmp = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%a.load")
  %"%b.load" = load i64, ptr %b2, align 8
  %calltmp5 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%b.load")
  %"%c.load" = load i64, ptr %c3, align 8
  %calltmp6 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%c.load")
  %"%d.load" = load i64, ptr %d4, align 8
  %calltmp7 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%d.load")
  ret i64 0
}

define i64 @net_pci_read(i64 %bus, i64 %dev, i64 %"\D9\81\D9\86\D9\83", i64 %offset) {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %offset4 = alloca i64, align 8
  store i64 %offset, ptr %offset4, align 8
  %"\D9\81\D9\86\D9\833" = alloca i64, align 8
  store i64 %"\D9\81\D9\86\D9\83", ptr %"\D9\81\D9\86\D9\833", align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 2147483648
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%bus.load" = load i64, ptr %bus1, align 8
  %shltmp = shl i64 %"%bus.load", 16
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load5" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp6 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load5", %shltmp
  store i64 %addtmp6, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %shltmp7 = shl i64 %"%dev.load", 11
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load8" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load8", %shltmp7
  store i64 %addtmp9, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D9\81\D9\86\D9\83.load" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %shltmp10 = shl i64 %"%\D9\81\D9\86\D9\83.load", 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load11" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp12 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load11", %shltmp10
  store i64 %addtmp12, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%offset.load" = load i64, ptr %offset4, align 8
  %andtmp = and i64 %"%offset.load", 255
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load13" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp14 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load13", %andtmp
  store i64 %addtmp14, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 0
}

define i64 @net_pci_write(i64 %bus, i64 %dev, i64 %"\D9\81\D9\86\D9\83", i64 %offset, i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A95" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A95", align 8
  %offset4 = alloca i64, align 8
  store i64 %offset, ptr %offset4, align 8
  %"\D9\81\D9\86\D9\833" = alloca i64, align 8
  store i64 %"\D9\81\D9\86\D9\83", ptr %"\D9\81\D9\86\D9\833", align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 2147483648
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%bus.load" = load i64, ptr %bus1, align 8
  %shltmp = shl i64 %"%bus.load", 16
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load6" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load6", %shltmp
  store i64 %addtmp7, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %shltmp8 = shl i64 %"%dev.load", 11
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load9" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp10 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load9", %shltmp8
  store i64 %addtmp10, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D9\81\D9\86\D9\83.load" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %shltmp11 = shl i64 %"%\D9\81\D9\86\D9\83.load", 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load12" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp13 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load12", %shltmp11
  store i64 %addtmp13, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%offset.load" = load i64, ptr %offset4, align 8
  %andtmp = and i64 %"%offset.load", 255
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load14" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp15 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load14", %andtmp
  store i64 %addtmp15, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 0
}

define i64 @net_pci_scan() {
entry:
  %"%device" = alloca i64, align 8
  %"%vendor" = alloca i64, align 8
  %"%vendor_device" = alloca i64, align 8
  %"%dev" = alloca i64, align 8
  %"%bus" = alloca i64, align 8
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  store i64 0, ptr %"%bus", align 8
  br label %while_cond_21

merge_28:                                         ; preds = %merge_30, %while_body_25
  %"%vendor.load10" = load i64, ptr %"%vendor", align 8
  %cmpeqtmp11 = icmp eq i64 %"%vendor.load10", 0
  br i1 %cmpeqtmp11, label %then_31, label %merge_32

merge_30:                                         ; preds = %then_27
  br label %merge_28

merge_32:                                         ; preds = %merge_34, %merge_28
  %"%dev.load19" = load i64, ptr %"%dev", align 8
  %addtmp20 = add i64 %"%dev.load19", 1
  store i64 %addtmp20, ptr %"%dev", align 8
  br label %while_cond_24

merge_34:                                         ; preds = %then_31
  br label %merge_32

then_27:                                          ; preds = %while_body_25
  %"%device.load" = load i64, ptr %"%device", align 8
  %cmpeqtmp9 = icmp eq i64 %"%device.load", 0
  br i1 %cmpeqtmp9, label %then_29, label %merge_30

then_29:                                          ; preds = %then_27
  %calltmp12 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%bus.load13" = load i64, ptr %"%bus", align 8
  %calltmp14 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%bus.load13")
  %"%dev.load15" = load i64, ptr %"%dev", align 8
  %calltmp16 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%dev.load15")
  ret i64 1

then_31:                                          ; preds = %merge_28
  %"%device.load17" = load i64, ptr %"%device", align 8
  %cmpeqtmp18 = icmp eq i64 %"%device.load17", 0
  br i1 %cmpeqtmp18, label %then_33, label %merge_34

then_33:                                          ; preds = %then_31
  %calltmp21 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%bus.load22" = load i64, ptr %"%bus", align 8
  %calltmp23 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%bus.load22")
  %"%dev.load24" = load i64, ptr %"%dev", align 8
  %calltmp25 = call i64 @"net_serial_\D8\B1\D9\82\D9\85"(i64 %"%dev.load24")
  ret i64 1

while_body_22:                                    ; preds = %while_cond_21
  store i64 0, ptr %"%dev", align 8
  br label %while_cond_24

while_body_25:                                    ; preds = %while_cond_24
  %"%bus.load3" = load i64, ptr %"%bus", align 8
  %"%dev.load4" = load i64, ptr %"%dev", align 8
  %calltmp5 = call i64 @net_pci_read(i64 %"%bus.load3", i64 %"%dev.load4", i64 0, i64 0)
  store i64 %calltmp5, ptr %"%vendor_device", align 8
  %"%vendor_device.load" = load i64, ptr %"%vendor_device", align 8
  %andtmp = and i64 %"%vendor_device.load", 65535
  store i64 %andtmp, ptr %"%vendor", align 8
  %"%vendor_device.load6" = load i64, ptr %"%vendor_device", align 8
  %shrtmp = lshr i64 %"%vendor_device.load6", 16
  %andtmp7 = and i64 %shrtmp, 65535
  store i64 %andtmp7, ptr %"%device", align 8
  %"%vendor.load" = load i64, ptr %"%vendor", align 8
  %cmpeqtmp = icmp eq i64 %"%vendor.load", 0
  br i1 %cmpeqtmp, label %then_27, label %merge_28

while_cond_21:                                    ; preds = %while_exit_26, %entry
  %"%bus.load" = load i64, ptr %"%bus", align 8
  %cmplttmp = icmp slt i64 %"%bus.load", 256
  br i1 %cmplttmp, label %while_body_22, label %while_exit_23

while_cond_24:                                    ; preds = %merge_32, %while_body_22
  %"%dev.load" = load i64, ptr %"%dev", align 8
  %cmplttmp2 = icmp slt i64 %"%dev.load", 32
  br i1 %cmplttmp2, label %while_body_25, label %while_exit_26

while_exit_23:                                    ; preds = %while_cond_21
  %calltmp1 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  ret i64 0

while_exit_26:                                    ; preds = %while_cond_24
  %"%bus.load8" = load i64, ptr %"%bus", align 8
  %addtmp = add i64 %"%bus.load8", 1
  store i64 %addtmp, ptr %"%bus", align 8
  br label %while_cond_21
}

define i64 @rtl_get_io_base() {
entry:
  %"%bar0" = alloca i64, align 8
  store i64 0, ptr %"%bar0", align 8
  %"%bar0.load" = load i64, ptr %"%bar0", align 8
  %andtmp = and i64 %"%bar0.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_35, label %merge_36

merge_36:                                         ; preds = %then_35, %entry
  %"%bar0.load1" = load i64, ptr %"%bar0", align 8
  %andtmp2 = and i64 %"%bar0.load1", 1
  %cmpeqtmp3 = icmp eq i64 %andtmp2, 0
  br i1 %cmpeqtmp3, label %then_37, label %merge_38

merge_38:                                         ; preds = %then_37, %merge_36
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  ret i64 0

then_35:                                          ; preds = %entry
  br label %merge_36

then_37:                                          ; preds = %merge_36
  br label %merge_38
}

define i64 @net_enable_bus_master() {
entry:
  %"%cmd" = alloca i64, align 8
  store i64 0, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 65535
  store i64 %andtmp, ptr %"%cmd", align 8
  %"%cmd.load1" = load i64, ptr %"%cmd", align 8
  %addtmp = add i64 %"%cmd.load1", 7
  %"%cmd.load2" = load i64, ptr %"%cmd", align 8
  %andtmp3 = and i64 %"%cmd.load2", 7
  %subtmp = sub i64 %addtmp, %andtmp3
  store i64 %subtmp, ptr %"%cmd", align 8
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  ret i64 0
}

define i64 @rtl_reset() {
entry:
  %"%cmd" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  call void @sad_ll_port_outb(i64 0, i64 0)
  store i64 0, ptr %"%timeout", align 8
  br label %while_cond_39

merge_43:                                         ; preds = %while_body_40
  %"%timeout.load2" = load i64, ptr %"%timeout", align 8
  %addtmp = add i64 %"%timeout.load2", 1
  store i64 %addtmp, ptr %"%timeout", align 8
  br label %while_cond_39

then_42:                                          ; preds = %while_body_40
  %calltmp1 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  ret i64 1

while_body_40:                                    ; preds = %while_cond_39
  call void @sad_ll_port_inb(i64 0)
  store i64 0, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %shrtmp = lshr i64 %"%cmd.load", 4
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_42, label %merge_43

while_cond_39:                                    ; preds = %merge_43, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmplttmp = icmp slt i64 %"%timeout.load", 10000
  br i1 %cmplttmp, label %while_body_40, label %while_exit_41

while_exit_41:                                    ; preds = %while_cond_39
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  ret i64 0
}

define i64 @rtl_read_mac() {
entry:
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %calltmp1 = call i64 @net_serial_mac()
  ret i64 0
}

define i64 @rtl_alloc_buffers() {
entry:
  br i1 true, label %then_44, label %merge_45

merge_45:                                         ; preds = %entry
  br i1 true, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  %calltmp2 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %calltmp3 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  ret i64 1

then_44:                                          ; preds = %entry
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  ret i64 0

then_46:                                          ; preds = %merge_45
  %calltmp1 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  ret i64 0
}

define i64 @rtl_init() {
entry:
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %calltmp1 = call i64 @net_enable_bus_master()
  %calltmp2 = call i64 @rtl_get_io_base()
  %calltmp3 = call i64 @rtl_reset()
  %cmpeqtmp = icmp eq i64 %calltmp3, 0
  br i1 %cmpeqtmp, label %then_48, label %merge_49

merge_49:                                         ; preds = %entry
  %calltmp4 = call i64 @rtl_read_mac()
  %calltmp5 = call i64 @rtl_alloc_buffers()
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 0
  br i1 %cmpeqtmp6, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  call void @sad_ll_port_outl(i64 0, i64 143)
  call void @sad_ll_port_outl(i64 0, i64 768)
  call void @sad_ll_port_outw(i64 0, i64 5)
  call void @sad_ll_port_outb(i64 0, i64 0)
  %calltmp7 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  ret i64 1

then_48:                                          ; preds = %entry
  ret i64 0

then_50:                                          ; preds = %merge_49
  ret i64 0
}

define i64 @rtl_send(i64 %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86", i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85") {
entry:
  %"%status" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%tsd_offset" = alloca i64, align 8
  %"%tx_addr" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A" = alloca i64, align 8
  %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85", ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  br i1 true, label %then_52, label %merge_53

merge_53:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load", 0
  br i1 %cmpgttmp, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%239.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  store i64 %"%239.load", ptr %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A", align 8
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load", 0
  br i1 %cmplttmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  br i1 false, label %then_58, label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  br i1 false, label %then_60, label %merge_61

merge_61:                                         ; preds = %then_60, %merge_59
  br i1 false, label %then_62, label %merge_63

merge_63:                                         ; preds = %then_62, %merge_61
  %"%tx_addr.load" = load i64, ptr %"%tx_addr", align 8
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load3" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  call void @sad_ll_memcpy(i64 %"%tx_addr.load", i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load3")
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load4" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load5" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %cmpgttmp6 = icmp sgt i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load4", %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load5"
  br i1 %cmpgttmp6, label %then_64, label %merge_65

merge_65:                                         ; preds = %then_64, %merge_63
  store i64 0, ptr %"%tsd_offset", align 8
  %"%tsd_offset.load" = load i64, ptr %"%tsd_offset", align 8
  %addtmp11 = add i64 0, %"%tsd_offset.load"
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load12" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A", align 8
  call void @sad_ll_port_outl(i64 %addtmp11, i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load12")
  store i64 0, ptr %"%timeout", align 8
  br label %while_cond_66

merge_70:                                         ; preds = %while_body_67
  %"%status.load16" = load i64, ptr %"%status", align 8
  %shrtmp17 = lshr i64 %"%status.load16", 14
  %andtmp18 = and i64 %shrtmp17, 1
  %cmpeqtmp19 = icmp eq i64 %andtmp18, 1
  br i1 %cmpeqtmp19, label %then_71, label %merge_72

merge_72:                                         ; preds = %merge_70
  %"%timeout.load20" = load i64, ptr %"%timeout", align 8
  %addtmp21 = add i64 %"%timeout.load20", 1
  store i64 %addtmp21, ptr %"%timeout", align 8
  br label %while_cond_66

then_52:                                          ; preds = %entry
  ret i64 0

then_54:                                          ; preds = %merge_53
  ret i64 0

then_56:                                          ; preds = %merge_55
  br label %merge_57

then_58:                                          ; preds = %merge_57
  br label %merge_59

then_60:                                          ; preds = %merge_59
  br label %merge_61

then_62:                                          ; preds = %merge_61
  br label %merge_63

then_64:                                          ; preds = %merge_63
  %"%tx_addr.load7" = load i64, ptr %"%tx_addr", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load8" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %addtmp = add i64 %"%tx_addr.load7", %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load8"
  %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load9" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load10" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %subtmp = sub i64 %"%\D8\AD\D8\AC\D9\85_\D9\81\D8\B9\D9\84\D9\8A.load9", %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load10"
  call void @sad_ll_memset(i64 %addtmp, i64 0, i64 %subtmp)
  br label %merge_65

then_69:                                          ; preds = %while_body_67
  ret i64 1

then_71:                                          ; preds = %merge_70
  ret i64 0

while_body_67:                                    ; preds = %while_cond_66
  %"%tsd_offset.load14" = load i64, ptr %"%tsd_offset", align 8
  %addtmp15 = add i64 0, %"%tsd_offset.load14"
  call void @sad_ll_port_inl(i64 %addtmp15)
  store i64 0, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %shrtmp = lshr i64 %"%status.load", 15
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_69, label %merge_70

while_cond_66:                                    ; preds = %merge_72, %merge_65
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmplttmp13 = icmp slt i64 %"%timeout.load", 100000
  br i1 %cmplttmp13, label %while_body_67, label %while_exit_68

while_exit_68:                                    ; preds = %while_cond_66
  ret i64 0
}

define i64 @rtl_recv(i64 %dest_addr, i64 %max_size) {
entry:
  %"%new_capr2" = alloca i64, align 8
  %"%data_len" = alloca i64, align 8
  %"%new_capr" = alloca i64, align 8
  %"%pkt_len" = alloca i64, align 8
  %"%len_hi" = alloca i64, align 8
  %"%len_lo" = alloca i64, align 8
  %"%pkt_status" = alloca i64, align 8
  %"%hdr_hi" = alloca i64, align 8
  %"%hdr_lo" = alloca i64, align 8
  %"%offset" = alloca i64, align 8
  %"%capr" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %max_size2 = alloca i64, align 8
  store i64 %max_size, ptr %max_size2, align 8
  %dest_addr1 = alloca i64, align 8
  store i64 %dest_addr, ptr %dest_addr1, align 8
  br i1 true, label %then_73, label %merge_74

merge_74:                                         ; preds = %entry
  call void @sad_ll_port_inb(i64 0)
  store i64 0, ptr %"%cmd", align 8
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_75, label %merge_76

merge_76:                                         ; preds = %merge_74
  call void @sad_ll_port_inw(i64 0)
  store i64 0, ptr %"%capr", align 8
  %"%capr.load" = load i64, ptr %"%capr", align 8
  %addtmp = add i64 %"%capr.load", 16
  %modtmp = srem i64 %addtmp, 0
  store i64 %modtmp, ptr %"%offset", align 8
  %"%offset.load" = load i64, ptr %"%offset", align 8
  %addtmp3 = add i64 0, %"%offset.load"
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%hdr_lo", align 8
  %addtmp4 = add i64 %addtmp3, 1
  call void @sad_ll_mem_read8(i64 %addtmp4)
  store i64 0, ptr %"%hdr_hi", align 8
  %"%hdr_hi.load" = load i64, ptr %"%hdr_hi", align 8
  %shltmp = shl i64 %"%hdr_hi.load", 8
  %"%hdr_lo.load" = load i64, ptr %"%hdr_lo", align 8
  %addtmp5 = add i64 %"%hdr_lo.load", %shltmp
  store i64 %addtmp5, ptr %"%pkt_status", align 8
  %addtmp6 = add i64 %addtmp3, 2
  call void @sad_ll_mem_read8(i64 %addtmp6)
  store i64 0, ptr %"%len_lo", align 8
  %addtmp7 = add i64 %addtmp3, 3
  call void @sad_ll_mem_read8(i64 %addtmp7)
  store i64 0, ptr %"%len_hi", align 8
  %"%len_hi.load" = load i64, ptr %"%len_hi", align 8
  %shltmp8 = shl i64 %"%len_hi.load", 8
  %"%len_lo.load" = load i64, ptr %"%len_lo", align 8
  %addtmp9 = add i64 %"%len_lo.load", %shltmp8
  store i64 %addtmp9, ptr %"%pkt_len", align 8
  %"%pkt_status.load" = load i64, ptr %"%pkt_status", align 8
  %andtmp10 = and i64 %"%pkt_status.load", 1
  %cmpeqtmp11 = icmp eq i64 %andtmp10, 0
  br i1 %cmpeqtmp11, label %then_77, label %merge_78

merge_78:                                         ; preds = %merge_76
  %"%pkt_len.load20" = load i64, ptr %"%pkt_len", align 8
  %subtmp21 = sub i64 %"%pkt_len.load20", 4
  store i64 %subtmp21, ptr %"%data_len", align 8
  %"%data_len.load" = load i64, ptr %"%data_len", align 8
  %"%max_size.load" = load i64, ptr %max_size2, align 8
  %cmpgttmp = icmp sgt i64 %"%data_len.load", %"%max_size.load"
  br i1 %cmpgttmp, label %then_79, label %merge_80

merge_80:                                         ; preds = %then_79, %merge_78
  %"%data_len.load23" = load i64, ptr %"%data_len", align 8
  %cmplttmp = icmp slt i64 %"%data_len.load23", 0
  br i1 %cmplttmp, label %then_81, label %merge_82

merge_82:                                         ; preds = %then_81, %merge_80
  %"%offset.load24" = load i64, ptr %"%offset", align 8
  %addtmp25 = add i64 0, %"%offset.load24"
  %addtmp26 = add i64 %addtmp25, 4
  %"%dest_addr.load" = load i64, ptr %dest_addr1, align 8
  %"%data_len.load27" = load i64, ptr %"%data_len", align 8
  call void @sad_ll_memcpy(i64 %"%dest_addr.load", i64 %addtmp26, i64 %"%data_len.load27")
  %"%offset.load28" = load i64, ptr %"%offset", align 8
  %"%pkt_len.load29" = load i64, ptr %"%pkt_len", align 8
  %addtmp30 = add i64 %"%offset.load28", %"%pkt_len.load29"
  %addtmp31 = add i64 %addtmp30, 4
  %addtmp32 = add i64 %addtmp31, 3
  %andtmp33 = and i64 %addtmp32, 3
  %subtmp34 = sub i64 %addtmp32, %andtmp33
  store i64 %subtmp34, ptr %"%new_capr2", align 8
  %"%new_capr2.load" = load i64, ptr %"%new_capr2", align 8
  %modtmp35 = srem i64 %"%new_capr2.load", 0
  %subtmp36 = sub i64 %modtmp35, 16
  store i64 %subtmp36, ptr %"%new_capr2", align 8
  %"%new_capr2.load37" = load i64, ptr %"%new_capr2", align 8
  call void @sad_ll_port_outw(i64 0, i64 %"%new_capr2.load37")
  %loadtmp = load i64, ptr %"%data_len", align 8
  ret i64 %loadtmp

then_73:                                          ; preds = %entry
  ret i64 0

then_75:                                          ; preds = %merge_74
  ret i64 0

then_77:                                          ; preds = %merge_76
  %"%offset.load12" = load i64, ptr %"%offset", align 8
  %"%pkt_len.load" = load i64, ptr %"%pkt_len", align 8
  %addtmp13 = add i64 %"%offset.load12", %"%pkt_len.load"
  %addtmp14 = add i64 %addtmp13, 4
  %addtmp15 = add i64 %addtmp14, 3
  %andtmp16 = and i64 %addtmp15, 3
  %subtmp = sub i64 %addtmp15, %andtmp16
  store i64 %subtmp, ptr %"%new_capr", align 8
  %"%new_capr.load" = load i64, ptr %"%new_capr", align 8
  %modtmp17 = srem i64 %"%new_capr.load", 0
  %subtmp18 = sub i64 %modtmp17, 16
  store i64 %subtmp18, ptr %"%new_capr", align 8
  %"%new_capr.load19" = load i64, ptr %"%new_capr", align 8
  call void @sad_ll_port_outw(i64 0, i64 %"%new_capr.load19")
  ret i64 0

then_79:                                          ; preds = %merge_78
  %"%max_size.load22" = load i64, ptr %max_size2, align 8
  store i64 %"%max_size.load22", ptr %"%data_len", align 8
  br label %merge_80

then_81:                                          ; preds = %merge_80
  store i64 0, ptr %"%data_len", align 8
  br label %merge_82
}

define i64 @e1000_read(i64 %reg) {
entry:
  %reg1 = alloca i64, align 8
  store i64 %reg, ptr %reg1, align 8
  %"%reg.load" = load i64, ptr %reg1, align 8
  %addtmp = add i64 0, %"%reg.load"
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @e1000_write(i64 %reg, i64 %val) {
entry:
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %reg1 = alloca i64, align 8
  store i64 %reg, ptr %reg1, align 8
  %"%reg.load" = load i64, ptr %reg1, align 8
  %addtmp = add i64 0, %"%reg.load"
  %"%val.load" = load i64, ptr %val2, align 8
  call void @sad_ll_mem_write32(i64 %addtmp, i64 %"%val.load")
  ret i64 0
}

define i64 @e1000_get_mmio_base() {
entry:
  %"%bar0" = alloca i64, align 8
  store i64 0, ptr %"%bar0", align 8
  %"%bar0.load" = load i64, ptr %"%bar0", align 8
  %andtmp = and i64 %"%bar0.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_83, label %merge_84

merge_84:                                         ; preds = %then_83, %entry
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  ret i64 0

then_83:                                          ; preds = %entry
  br label %merge_84
}

define i64 @e1000_reset() {
entry:
  %"%w" = alloca i64, align 8
  %"%ctrl" = alloca i64, align 8
  store i64 0, ptr %"%ctrl", align 8
  store i64 0, ptr %"%w", align 8
  br label %while_cond_85

while_body_86:                                    ; preds = %while_cond_85
  %"%w.load1" = load i64, ptr %"%w", align 8
  %addtmp = add i64 %"%w.load1", 1
  store i64 %addtmp, ptr %"%w", align 8
  br label %while_cond_85

while_cond_85:                                    ; preds = %while_body_86, %entry
  %"%w.load" = load i64, ptr %"%w", align 8
  %cmplttmp = icmp slt i64 %"%w.load", 100000
  br i1 %cmplttmp, label %while_body_86, label %while_exit_87

while_exit_87:                                    ; preds = %while_cond_85
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  ret i64 1
}

define i64 @e1000_eeprom_read(i64 %eeaddr) {
entry:
  %"%val" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %eeaddr1 = alloca i64, align 8
  store i64 %eeaddr, ptr %eeaddr1, align 8
  store i64 0, ptr %"%timeout", align 8
  br label %while_cond_88

merge_92:                                         ; preds = %while_body_89
  %"%timeout.load5" = load i64, ptr %"%timeout", align 8
  %addtmp = add i64 %"%timeout.load5", 1
  store i64 %addtmp, ptr %"%timeout", align 8
  br label %while_cond_88

then_91:                                          ; preds = %while_body_89
  %"%val.load2" = load i64, ptr %"%val", align 8
  %shrtmp3 = lshr i64 %"%val.load2", 16
  %andtmp4 = and i64 %shrtmp3, 65535
  ret i64 %andtmp4

while_body_89:                                    ; preds = %while_cond_88
  store i64 0, ptr %"%val", align 8
  %"%val.load" = load i64, ptr %"%val", align 8
  %shrtmp = lshr i64 %"%val.load", 4
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_91, label %merge_92

while_cond_88:                                    ; preds = %merge_92, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmplttmp = icmp slt i64 %"%timeout.load", 100000
  br i1 %cmplttmp, label %while_body_89, label %while_exit_90

while_exit_90:                                    ; preds = %while_cond_88
  ret i64 0
}

define i64 @e1000_read_mac() {
entry:
  %"%word2" = alloca i64, align 8
  %"%word1" = alloca i64, align 8
  %"%word0" = alloca i64, align 8
  %calltmp = call i64 @e1000_eeprom_read(i64 0)
  store i64 %calltmp, ptr %"%word0", align 8
  %calltmp1 = call i64 @e1000_eeprom_read(i64 1)
  store i64 %calltmp1, ptr %"%word1", align 8
  %calltmp2 = call i64 @e1000_eeprom_read(i64 2)
  store i64 %calltmp2, ptr %"%word2", align 8
  %calltmp3 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %calltmp4 = call i64 @net_serial_mac()
  ret i64 0
}

define i64 @e1000_alloc_descriptors() {
entry:
  %"%desc_addr" = alloca i64, align 8
  %"%buf" = alloca i64, align 8
  %"%\D8\AA" = alloca i64, align 8
  br i1 true, label %then_93, label %merge_94

merge_101:                                        ; preds = %while_body_98
  %"%buf.load4" = load i64, ptr %"%buf", align 8
  call void @sad_ll_memset(i64 %"%buf.load4", i64 0, i64 2048)
  %"%\D8\AA.load5" = load i64, ptr %"%\D8\AA", align 8
  %multmp = mul i64 %"%\D8\AA.load5", 0
  %addtmp = add i64 0, %multmp
  store i64 %addtmp, ptr %"%desc_addr", align 8
  %"%desc_addr.load" = load i64, ptr %"%desc_addr", align 8
  %"%buf.load6" = load i64, ptr %"%buf", align 8
  call void @sad_ll_mem_write32(i64 %"%desc_addr.load", i64 %"%buf.load6")
  %"%desc_addr.load7" = load i64, ptr %"%desc_addr", align 8
  %addtmp8 = add i64 %"%desc_addr.load7", 4
  call void @sad_ll_mem_write32(i64 %addtmp8, i64 0)
  %"%\D8\AA.load9" = load i64, ptr %"%\D8\AA", align 8
  %addtmp10 = add i64 %"%\D8\AA.load9", 1
  store i64 %addtmp10, ptr %"%\D8\AA", align 8
  br label %while_cond_97

merge_94:                                         ; preds = %entry
  br i1 true, label %then_95, label %merge_96

merge_96:                                         ; preds = %merge_94
  store i64 0, ptr %"%\D8\AA", align 8
  br label %while_cond_97

then_100:                                         ; preds = %while_body_98
  ret i64 0

then_93:                                          ; preds = %entry
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  ret i64 0

then_95:                                          ; preds = %merge_94
  %calltmp1 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  ret i64 0

while_body_98:                                    ; preds = %while_cond_97
  call void @sad_ll_kmalloc(i64 2048)
  store i64 0, ptr %"%buf", align 8
  %"%buf.load" = load i64, ptr %"%buf", align 8
  %cmpeqtmp = icmp eq i64 %"%buf.load", 0
  br i1 %cmpeqtmp, label %then_100, label %merge_101

while_cond_97:                                    ; preds = %merge_101, %merge_96
  %"%\D8\AA.load" = load i64, ptr %"%\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AA.load", 0
  br i1 %cmplttmp, label %while_body_98, label %while_exit_99

while_exit_99:                                    ; preds = %while_cond_97
  %calltmp2 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %calltmp3 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  ret i64 1
}

define i64 @e1000_setup_rx() {
entry:
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  ret i64 0
}

define i64 @e1000_setup_tx() {
entry:
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  ret i64 0
}

define i64 @e1000_init() {
entry:
  %"%status" = alloca i64, align 8
  %"%ctrl" = alloca i64, align 8
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp1 = call i64 @net_enable_bus_master()
  %calltmp2 = call i64 @e1000_get_mmio_base()
  br i1 true, label %then_102, label %merge_103

merge_103:                                        ; preds = %entry
  %calltmp4 = call i64 @e1000_reset()
  %calltmp5 = call i64 @e1000_read_mac()
  %calltmp6 = call i64 @e1000_alloc_descriptors()
  %cmpeqtmp = icmp eq i64 %calltmp6, 0
  br i1 %cmpeqtmp, label %then_104, label %merge_105

merge_105:                                        ; preds = %merge_103
  store i64 0, ptr %"%ctrl", align 8
  %"%ctrl.load" = load i64, ptr %"%ctrl", align 8
  %addtmp = add i64 %"%ctrl.load", 0
  store i64 %addtmp, ptr %"%ctrl", align 8
  %calltmp7 = call i64 @e1000_setup_rx()
  %calltmp8 = call i64 @e1000_setup_tx()
  store i64 0, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %shrtmp = lshr i64 %"%status.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp9 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp9, label %then_106, label %merge_107

merge_107:                                        ; preds = %then_106, %merge_105
  %"%status.load11" = load i64, ptr %"%status", align 8
  %shrtmp12 = lshr i64 %"%status.load11", 1
  %andtmp13 = and i64 %shrtmp12, 1
  %cmpeqtmp14 = icmp eq i64 %andtmp13, 0
  br i1 %cmpeqtmp14, label %then_108, label %merge_109

merge_109:                                        ; preds = %then_108, %merge_107
  %calltmp16 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  ret i64 1

then_102:                                         ; preds = %entry
  %calltmp3 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  ret i64 0

then_104:                                         ; preds = %merge_103
  ret i64 0

then_106:                                         ; preds = %merge_105
  %calltmp10 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  br label %merge_107

then_108:                                         ; preds = %merge_107
  %calltmp15 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  br label %merge_109
}

define i64 @e1000_send(i64 %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86", i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85") {
entry:
  %"%desc" = alloca i64, align 8
  %"%buf" = alloca i64, align 8
  %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85", ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  br i1 true, label %then_110, label %merge_111

merge_111:                                        ; preds = %entry
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load", 64
  call void @sad_ll_kmalloc(i64 %addtmp)
  store i64 0, ptr %"%buf", align 8
  %"%buf.load" = load i64, ptr %"%buf", align 8
  %cmpeqtmp = icmp eq i64 %"%buf.load", 0
  br i1 %cmpeqtmp, label %then_112, label %merge_113

merge_113:                                        ; preds = %merge_111
  %"%buf.load3" = load i64, ptr %"%buf", align 8
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load4" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  call void @sad_ll_memcpy(i64 %"%buf.load3", i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load4")
  store i64 0, ptr %"%desc", align 8
  %"%desc.load" = load i64, ptr %"%desc", align 8
  %"%buf.load5" = load i64, ptr %"%buf", align 8
  call void @sad_ll_mem_write32(i64 %"%desc.load", i64 %"%buf.load5")
  %"%desc.load6" = load i64, ptr %"%desc", align 8
  %addtmp7 = add i64 %"%desc.load6", 4
  call void @sad_ll_mem_write32(i64 %addtmp7, i64 0)
  %"%desc.load8" = load i64, ptr %"%desc", align 8
  %addtmp9 = add i64 %"%desc.load8", 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load10" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %addtmp11 = add i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load10", 184549376
  call void @sad_ll_mem_write32(i64 %addtmp9, i64 %addtmp11)
  %"%desc.load12" = load i64, ptr %"%desc", align 8
  %addtmp13 = add i64 %"%desc.load12", 12
  call void @sad_ll_mem_write32(i64 %addtmp13, i64 0)
  ret i64 1

then_110:                                         ; preds = %entry
  ret i64 0

then_112:                                         ; preds = %merge_111
  ret i64 0
}

define i64 @e1000_recv(i64 %dest_addr, i64 %max_size) {
entry:
  %"%buf_addr" = alloca i64, align 8
  %"%pkt_len" = alloca i64, align 8
  %"%len_cmd" = alloca i64, align 8
  %"%dd" = alloca i64, align 8
  %"%status" = alloca i64, align 8
  %"%desc" = alloca i64, align 8
  %"%next_tail" = alloca i64, align 8
  %max_size2 = alloca i64, align 8
  store i64 %max_size, ptr %max_size2, align 8
  %dest_addr1 = alloca i64, align 8
  store i64 %dest_addr, ptr %dest_addr1, align 8
  br i1 true, label %then_114, label %merge_115

merge_115:                                        ; preds = %entry
  store i64 poison, ptr %"%next_tail", align 8
  %"%next_tail.load" = load i64, ptr %"%next_tail", align 8
  %multmp = mul i64 %"%next_tail.load", 0
  %addtmp = add i64 0, %multmp
  store i64 %addtmp, ptr %"%desc", align 8
  %"%desc.load" = load i64, ptr %"%desc", align 8
  %addtmp3 = add i64 %"%desc.load", 12
  call void @sad_ll_mem_read32(i64 %addtmp3)
  store i64 0, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %andtmp = and i64 %"%status.load", 1
  store i64 %andtmp, ptr %"%dd", align 8
  %"%dd.load" = load i64, ptr %"%dd", align 8
  %cmpeqtmp = icmp eq i64 %"%dd.load", 0
  br i1 %cmpeqtmp, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%desc.load4" = load i64, ptr %"%desc", align 8
  %addtmp5 = add i64 %"%desc.load4", 8
  call void @sad_ll_mem_read32(i64 %addtmp5)
  store i64 0, ptr %"%len_cmd", align 8
  %"%len_cmd.load" = load i64, ptr %"%len_cmd", align 8
  %andtmp6 = and i64 %"%len_cmd.load", 65535
  store i64 %andtmp6, ptr %"%pkt_len", align 8
  %"%pkt_len.load" = load i64, ptr %"%pkt_len", align 8
  %"%max_size.load" = load i64, ptr %max_size2, align 8
  %cmpgttmp = icmp sgt i64 %"%pkt_len.load", %"%max_size.load"
  br i1 %cmpgttmp, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  %"%desc.load8" = load i64, ptr %"%desc", align 8
  call void @sad_ll_mem_read32(i64 %"%desc.load8")
  store i64 0, ptr %"%buf_addr", align 8
  %"%dest_addr.load" = load i64, ptr %dest_addr1, align 8
  %"%buf_addr.load" = load i64, ptr %"%buf_addr", align 8
  %"%pkt_len.load9" = load i64, ptr %"%pkt_len", align 8
  call void @sad_ll_memcpy(i64 %"%dest_addr.load", i64 %"%buf_addr.load", i64 %"%pkt_len.load9")
  %"%desc.load10" = load i64, ptr %"%desc", align 8
  %addtmp11 = add i64 %"%desc.load10", 12
  call void @sad_ll_mem_write32(i64 %addtmp11, i64 0)
  %loadtmp = load i64, ptr %"%pkt_len", align 8
  ret i64 %loadtmp

then_114:                                         ; preds = %entry
  ret i64 0

then_116:                                         ; preds = %merge_115
  ret i64 0

then_118:                                         ; preds = %merge_117
  %"%max_size.load7" = load i64, ptr %max_size2, align 8
  store i64 %"%max_size.load7", ptr %"%pkt_len", align 8
  br label %merge_119
}

define i64 @net_send(i64 %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86", i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85") {
entry:
  %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85", ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  br i1 true, label %then_120, label %merge_121

merge_121:                                        ; preds = %entry
  br i1 true, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  ret i64 0

then_120:                                         ; preds = %entry
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %calltmp = call i64 @rtl_send(i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load")
  ret i64 %calltmp

then_122:                                         ; preds = %merge_121
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3" = load i64, ptr %"\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load4" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %calltmp5 = call i64 @e1000_send(i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3", i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load4")
  ret i64 %calltmp5
}

define i64 @net_recv(i64 %dest_addr, i64 %max_size) {
entry:
  %max_size2 = alloca i64, align 8
  store i64 %max_size, ptr %max_size2, align 8
  %dest_addr1 = alloca i64, align 8
  store i64 %dest_addr, ptr %dest_addr1, align 8
  br i1 true, label %then_124, label %merge_125

merge_125:                                        ; preds = %entry
  br i1 true, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  ret i64 0

then_124:                                         ; preds = %entry
  %"%dest_addr.load" = load i64, ptr %dest_addr1, align 8
  %"%max_size.load" = load i64, ptr %max_size2, align 8
  %calltmp = call i64 @rtl_recv(i64 %"%dest_addr.load", i64 %"%max_size.load")
  ret i64 %calltmp

then_126:                                         ; preds = %merge_125
  %"%dest_addr.load3" = load i64, ptr %dest_addr1, align 8
  %"%max_size.load4" = load i64, ptr %max_size2, align 8
  %calltmp5 = call i64 @e1000_recv(i64 %"%dest_addr.load3", i64 %"%max_size.load4")
  ret i64 %calltmp5
}

define i64 @eth_build_header(i64 %buf, i64 %dst_0, i64 %dst_1, i64 %dst_2, i64 %dst_3, i64 %dst_4, i64 %dst_5, i64 %ether_type) {
entry:
  %ether_type8 = alloca i64, align 8
  store i64 %ether_type, ptr %ether_type8, align 8
  %dst_57 = alloca i64, align 8
  store i64 %dst_5, ptr %dst_57, align 8
  %dst_46 = alloca i64, align 8
  store i64 %dst_4, ptr %dst_46, align 8
  %dst_35 = alloca i64, align 8
  store i64 %dst_3, ptr %dst_35, align 8
  %dst_24 = alloca i64, align 8
  store i64 %dst_2, ptr %dst_24, align 8
  %dst_13 = alloca i64, align 8
  store i64 %dst_1, ptr %dst_13, align 8
  %dst_02 = alloca i64, align 8
  store i64 %dst_0, ptr %dst_02, align 8
  %buf1 = alloca i64, align 8
  store i64 %buf, ptr %buf1, align 8
  %"%buf.load" = load i64, ptr %buf1, align 8
  %"%dst_0.load" = load i64, ptr %dst_02, align 8
  call void @sad_ll_mem_write8(i64 %"%buf.load", i64 %"%dst_0.load")
  %"%buf.load9" = load i64, ptr %buf1, align 8
  %addtmp = add i64 %"%buf.load9", 1
  %"%dst_1.load" = load i64, ptr %dst_13, align 8
  call void @sad_ll_mem_write8(i64 %addtmp, i64 %"%dst_1.load")
  %"%buf.load10" = load i64, ptr %buf1, align 8
  %addtmp11 = add i64 %"%buf.load10", 2
  %"%dst_2.load" = load i64, ptr %dst_24, align 8
  call void @sad_ll_mem_write8(i64 %addtmp11, i64 %"%dst_2.load")
  %"%buf.load12" = load i64, ptr %buf1, align 8
  %addtmp13 = add i64 %"%buf.load12", 3
  %"%dst_3.load" = load i64, ptr %dst_35, align 8
  call void @sad_ll_mem_write8(i64 %addtmp13, i64 %"%dst_3.load")
  %"%buf.load14" = load i64, ptr %buf1, align 8
  %addtmp15 = add i64 %"%buf.load14", 4
  %"%dst_4.load" = load i64, ptr %dst_46, align 8
  call void @sad_ll_mem_write8(i64 %addtmp15, i64 %"%dst_4.load")
  %"%buf.load16" = load i64, ptr %buf1, align 8
  %addtmp17 = add i64 %"%buf.load16", 5
  %"%dst_5.load" = load i64, ptr %dst_57, align 8
  call void @sad_ll_mem_write8(i64 %addtmp17, i64 %"%dst_5.load")
  %"%buf.load18" = load i64, ptr %buf1, align 8
  %addtmp19 = add i64 %"%buf.load18", 12
  %"%ether_type.load" = load i64, ptr %ether_type8, align 8
  %shrtmp = lshr i64 %"%ether_type.load", 8
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_mem_write8(i64 %addtmp19, i64 %andtmp)
  %"%buf.load20" = load i64, ptr %buf1, align 8
  %addtmp21 = add i64 %"%buf.load20", 13
  %"%ether_type.load22" = load i64, ptr %ether_type8, align 8
  %andtmp23 = and i64 %"%ether_type.load22", 255
  call void @sad_ll_mem_write8(i64 %addtmp21, i64 %andtmp23)
  ret i64 0
}

define i64 @eth_send_broadcast(i64 %payload_addr, i64 %payload_len, i64 %ether_type) {
entry:
  %"%result" = alloca i64, align 8
  %"%total" = alloca i64, align 8
  %"%frame" = alloca i64, align 8
  %ether_type3 = alloca i64, align 8
  store i64 %ether_type, ptr %ether_type3, align 8
  %payload_len2 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len2, align 8
  %payload_addr1 = alloca i64, align 8
  store i64 %payload_addr, ptr %payload_addr1, align 8
  store i64 0, ptr %"%frame", align 8
  %"%frame.load" = load i64, ptr %"%frame", align 8
  %cmpeqtmp = icmp eq i64 %"%frame.load", 0
  br i1 %cmpeqtmp, label %then_128, label %merge_129

merge_129:                                        ; preds = %entry
  %"%frame.load4" = load i64, ptr %"%frame", align 8
  %"%ether_type.load" = load i64, ptr %ether_type3, align 8
  %calltmp = call i64 @eth_build_header(i64 %"%frame.load4", i64 255, i64 255, i64 255, i64 255, i64 255, i64 255, i64 %"%ether_type.load")
  %"%frame.load5" = load i64, ptr %"%frame", align 8
  %addtmp = add i64 %"%frame.load5", 0
  %"%payload_addr.load" = load i64, ptr %payload_addr1, align 8
  %"%payload_len.load" = load i64, ptr %payload_len2, align 8
  call void @sad_ll_memcpy(i64 %addtmp, i64 %"%payload_addr.load", i64 %"%payload_len.load")
  %"%payload_len.load6" = load i64, ptr %payload_len2, align 8
  %addtmp7 = add i64 0, %"%payload_len.load6"
  store i64 %addtmp7, ptr %"%total", align 8
  %"%frame.load8" = load i64, ptr %"%frame", align 8
  %"%total.load" = load i64, ptr %"%total", align 8
  %calltmp9 = call i64 @net_send(i64 %"%frame.load8", i64 %"%total.load")
  store i64 %calltmp9, ptr %"%result", align 8
  %"%frame.load10" = load i64, ptr %"%frame", align 8
  call void @sad_ll_kfree(i64 %"%frame.load10")
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_128:                                         ; preds = %entry
  ret i64 0
}

define i64 @eth_send_to(i64 %dst_0, i64 %dst_1, i64 %dst_2, i64 %dst_3, i64 %dst_4, i64 %dst_5, i64 %payload_addr, i64 %payload_len, i64 %ether_type) {
entry:
  %"%result" = alloca i64, align 8
  %"%total" = alloca i64, align 8
  %"%frame" = alloca i64, align 8
  %ether_type9 = alloca i64, align 8
  store i64 %ether_type, ptr %ether_type9, align 8
  %payload_len8 = alloca i64, align 8
  store i64 %payload_len, ptr %payload_len8, align 8
  %payload_addr7 = alloca i64, align 8
  store i64 %payload_addr, ptr %payload_addr7, align 8
  %dst_56 = alloca i64, align 8
  store i64 %dst_5, ptr %dst_56, align 8
  %dst_45 = alloca i64, align 8
  store i64 %dst_4, ptr %dst_45, align 8
  %dst_34 = alloca i64, align 8
  store i64 %dst_3, ptr %dst_34, align 8
  %dst_23 = alloca i64, align 8
  store i64 %dst_2, ptr %dst_23, align 8
  %dst_12 = alloca i64, align 8
  store i64 %dst_1, ptr %dst_12, align 8
  %dst_01 = alloca i64, align 8
  store i64 %dst_0, ptr %dst_01, align 8
  store i64 0, ptr %"%frame", align 8
  %"%frame.load" = load i64, ptr %"%frame", align 8
  %cmpeqtmp = icmp eq i64 %"%frame.load", 0
  br i1 %cmpeqtmp, label %then_130, label %merge_131

merge_131:                                        ; preds = %entry
  %"%frame.load10" = load i64, ptr %"%frame", align 8
  %"%dst_0.load" = load i64, ptr %dst_01, align 8
  %"%dst_1.load" = load i64, ptr %dst_12, align 8
  %"%dst_2.load" = load i64, ptr %dst_23, align 8
  %"%dst_3.load" = load i64, ptr %dst_34, align 8
  %"%dst_4.load" = load i64, ptr %dst_45, align 8
  %"%dst_5.load" = load i64, ptr %dst_56, align 8
  %"%ether_type.load" = load i64, ptr %ether_type9, align 8
  %calltmp = call i64 @eth_build_header(i64 %"%frame.load10", i64 %"%dst_0.load", i64 %"%dst_1.load", i64 %"%dst_2.load", i64 %"%dst_3.load", i64 %"%dst_4.load", i64 %"%dst_5.load", i64 %"%ether_type.load")
  %"%frame.load11" = load i64, ptr %"%frame", align 8
  %addtmp = add i64 %"%frame.load11", 0
  %"%payload_addr.load" = load i64, ptr %payload_addr7, align 8
  %"%payload_len.load" = load i64, ptr %payload_len8, align 8
  call void @sad_ll_memcpy(i64 %addtmp, i64 %"%payload_addr.load", i64 %"%payload_len.load")
  %"%payload_len.load12" = load i64, ptr %payload_len8, align 8
  %addtmp13 = add i64 0, %"%payload_len.load12"
  store i64 %addtmp13, ptr %"%total", align 8
  %"%frame.load14" = load i64, ptr %"%frame", align 8
  %"%total.load" = load i64, ptr %"%total", align 8
  %calltmp15 = call i64 @net_send(i64 %"%frame.load14", i64 %"%total.load")
  store i64 %calltmp15, ptr %"%result", align 8
  %"%frame.load16" = load i64, ptr %"%frame", align 8
  call void @sad_ll_kfree(i64 %"%frame.load16")
  %loadtmp = load i64, ptr %"%result", align 8
  ret i64 %loadtmp

then_130:                                         ; preds = %entry
  ret i64 0
}

define i64 @eth_parse_type(i64 %pkt_addr) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %pkt_addr1 = alloca i64, align 8
  store i64 %pkt_addr, ptr %pkt_addr1, align 8
  %"%pkt_addr.load" = load i64, ptr %pkt_addr1, align 8
  %addtmp = add i64 %"%pkt_addr.load", 12
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%hi", align 8
  %"%pkt_addr.load2" = load i64, ptr %pkt_addr1, align 8
  %addtmp3 = add i64 %"%pkt_addr.load2", 13
  call void @sad_ll_mem_read8(i64 %addtmp3)
  store i64 0, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %shltmp = shl i64 %"%hi.load", 8
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %addtmp4 = add i64 %shltmp, %"%lo.load"
  ret i64 %addtmp4
}

define i64 @eth_src_mac_byte(i64 %pkt_addr, i64 %index) {
entry:
  %index2 = alloca i64, align 8
  store i64 %index, ptr %index2, align 8
  %pkt_addr1 = alloca i64, align 8
  store i64 %pkt_addr, ptr %pkt_addr1, align 8
  %"%pkt_addr.load" = load i64, ptr %pkt_addr1, align 8
  %addtmp = add i64 %"%pkt_addr.load", 6
  %"%index.load" = load i64, ptr %index2, align 8
  %addtmp3 = add i64 %addtmp, %"%index.load"
  call void @sad_ll_mem_read8(i64 %addtmp3)
  ret i64 0
}

define i64 @eth_is_for_us(i64 %pkt_addr) {
entry:
  %"%m5" = alloca i64, align 8
  %"%m4" = alloca i64, align 8
  %"%m3" = alloca i64, align 8
  %"%m2" = alloca i64, align 8
  %"%m1" = alloca i64, align 8
  %"%m0" = alloca i64, align 8
  %"%b1" = alloca i64, align 8
  %"%b0" = alloca i64, align 8
  %pkt_addr1 = alloca i64, align 8
  store i64 %pkt_addr, ptr %pkt_addr1, align 8
  %"%pkt_addr.load" = load i64, ptr %pkt_addr1, align 8
  call void @sad_ll_mem_read8(i64 %"%pkt_addr.load")
  store i64 0, ptr %"%b0", align 8
  %"%b0.load" = load i64, ptr %"%b0", align 8
  %cmpeqtmp = icmp eq i64 %"%b0.load", 255
  br i1 %cmpeqtmp, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_135, %entry
  %"%pkt_addr.load4" = load i64, ptr %pkt_addr1, align 8
  call void @sad_ll_mem_read8(i64 %"%pkt_addr.load4")
  store i64 0, ptr %"%m0", align 8
  %"%m0.load" = load i64, ptr %"%m0", align 8
  %cmpnetmp = icmp ne i64 %"%m0.load", 0
  br i1 %cmpnetmp, label %then_136, label %merge_137

merge_135:                                        ; preds = %then_132
  br label %merge_133

merge_137:                                        ; preds = %merge_133
  %"%pkt_addr.load5" = load i64, ptr %pkt_addr1, align 8
  %addtmp6 = add i64 %"%pkt_addr.load5", 1
  call void @sad_ll_mem_read8(i64 %addtmp6)
  store i64 0, ptr %"%m1", align 8
  %"%m1.load" = load i64, ptr %"%m1", align 8
  %cmpnetmp7 = icmp ne i64 %"%m1.load", 0
  br i1 %cmpnetmp7, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  %"%pkt_addr.load8" = load i64, ptr %pkt_addr1, align 8
  %addtmp9 = add i64 %"%pkt_addr.load8", 2
  call void @sad_ll_mem_read8(i64 %addtmp9)
  store i64 0, ptr %"%m2", align 8
  %"%m2.load" = load i64, ptr %"%m2", align 8
  %cmpnetmp10 = icmp ne i64 %"%m2.load", 0
  br i1 %cmpnetmp10, label %then_140, label %merge_141

merge_141:                                        ; preds = %merge_139
  %"%pkt_addr.load11" = load i64, ptr %pkt_addr1, align 8
  %addtmp12 = add i64 %"%pkt_addr.load11", 3
  call void @sad_ll_mem_read8(i64 %addtmp12)
  store i64 0, ptr %"%m3", align 8
  %"%m3.load" = load i64, ptr %"%m3", align 8
  %cmpnetmp13 = icmp ne i64 %"%m3.load", 0
  br i1 %cmpnetmp13, label %then_142, label %merge_143

merge_143:                                        ; preds = %merge_141
  %"%pkt_addr.load14" = load i64, ptr %pkt_addr1, align 8
  %addtmp15 = add i64 %"%pkt_addr.load14", 4
  call void @sad_ll_mem_read8(i64 %addtmp15)
  store i64 0, ptr %"%m4", align 8
  %"%m4.load" = load i64, ptr %"%m4", align 8
  %cmpnetmp16 = icmp ne i64 %"%m4.load", 0
  br i1 %cmpnetmp16, label %then_144, label %merge_145

merge_145:                                        ; preds = %merge_143
  %"%pkt_addr.load17" = load i64, ptr %pkt_addr1, align 8
  %addtmp18 = add i64 %"%pkt_addr.load17", 5
  call void @sad_ll_mem_read8(i64 %addtmp18)
  store i64 0, ptr %"%m5", align 8
  %"%m5.load" = load i64, ptr %"%m5", align 8
  %cmpnetmp19 = icmp ne i64 %"%m5.load", 0
  br i1 %cmpnetmp19, label %then_146, label %merge_147

merge_147:                                        ; preds = %merge_145
  ret i64 1

then_132:                                         ; preds = %entry
  %"%pkt_addr.load2" = load i64, ptr %pkt_addr1, align 8
  %addtmp = add i64 %"%pkt_addr.load2", 1
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%b1", align 8
  %"%b1.load" = load i64, ptr %"%b1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%b1.load", 255
  br i1 %cmpeqtmp3, label %then_134, label %merge_135

then_134:                                         ; preds = %then_132
  ret i64 1

then_136:                                         ; preds = %merge_133
  ret i64 0

then_138:                                         ; preds = %merge_137
  ret i64 0

then_140:                                         ; preds = %merge_139
  ret i64 0

then_142:                                         ; preds = %merge_141
  ret i64 0

then_144:                                         ; preds = %merge_143
  ret i64 0

then_146:                                         ; preds = %merge_145
  ret i64 0
}

define i64 @"net_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %calltmp1 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  br i1 true, label %then_148, label %merge_149

merge_149:                                        ; preds = %then_148, %entry
  br i1 true, label %then_150, label %merge_151

merge_151:                                        ; preds = %then_150, %merge_149
  br i1 true, label %then_152, label %merge_153

merge_153:                                        ; preds = %then_152, %merge_151
  %calltmp5 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  %calltmp6 = call i64 @net_serial_mac()
  %calltmp7 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %calltmp8 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %calltmp9 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %calltmp10 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %calltmp11 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %calltmp12 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %calltmp13 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %calltmp14 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %calltmp15 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  %calltmp16 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  %calltmp17 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  br i1 false, label %then_154, label %merge_155

merge_155:                                        ; preds = %then_154, %merge_153
  br i1 true, label %then_156, label %merge_157

merge_157:                                        ; preds = %then_156, %merge_155
  ret i64 0

then_148:                                         ; preds = %entry
  %calltmp2 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  br label %merge_149

then_150:                                         ; preds = %merge_149
  %calltmp3 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  br label %merge_151

then_152:                                         ; preds = %merge_151
  %calltmp4 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  br label %merge_153

then_154:                                         ; preds = %merge_153
  %calltmp18 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  br label %merge_155

then_156:                                         ; preds = %merge_155
  %calltmp19 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  br label %merge_157
}

define i64 @"net_\D8\B9\D9\8A\D9\86_ip"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  ret i64 0
}

define i64 @"net_\D8\B9\D9\8A\D9\86_gw"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  ret i64 0
}

define i64 @"net_\D8\B9\D9\8A\D9\86_dns"(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %d4 = alloca i64, align 8
  store i64 %d, ptr %d4, align 8
  %c3 = alloca i64, align 8
  store i64 %c, ptr %c3, align 8
  %b2 = alloca i64, align 8
  store i64 %b, ptr %b2, align 8
  %a1 = alloca i64, align 8
  store i64 %a, ptr %a1, align 8
  ret i64 0
}

define i64 @"net_\D9\81\D8\AD\D8\B5_\D8\B1\D8\A7\D8\A8\D8\B7"() {
entry:
  %"%status" = alloca i64, align 8
  br i1 true, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_163, %entry
  ret i64 0

merge_161:                                        ; preds = %then_160, %then_158
  %"%status.load1" = load i64, ptr %"%status", align 8
  %shrtmp2 = lshr i64 %"%status.load1", 1
  %andtmp3 = and i64 %shrtmp2, 1
  %cmpeqtmp4 = icmp eq i64 %andtmp3, 0
  br i1 %cmpeqtmp4, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %merge_161
  br label %merge_159

then_158:                                         ; preds = %entry
  store i64 0, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %shrtmp = lshr i64 %"%status.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_160, label %merge_161

then_160:                                         ; preds = %then_158
  br label %merge_161

then_162:                                         ; preds = %merge_161
  br label %merge_163
}

define i64 @"net_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  %calltmp1 = call i64 @net_pci_scan()
  %cmpeqtmp = icmp eq i64 %calltmp1, 0
  br i1 %cmpeqtmp, label %then_164, label %merge_165

merge_165:                                        ; preds = %entry
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  br i1 true, label %then_166, label %merge_167

merge_167:                                        ; preds = %then_166, %merge_165
  br i1 true, label %then_168, label %merge_169

merge_169:                                        ; preds = %then_168, %merge_167
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpeqtmp5, label %then_170, label %merge_171

merge_171:                                        ; preds = %merge_169
  %calltmp7 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.51 to i64))
  %calltmp8 = call i64 @net_serial_mac()
  %calltmp9 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  ret i64 1

then_164:                                         ; preds = %entry
  %calltmp2 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.49 to i64))
  ret i64 0

then_166:                                         ; preds = %merge_165
  %calltmp3 = call i64 @rtl_init()
  store i64 %calltmp3, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  br label %merge_167

then_168:                                         ; preds = %merge_167
  %calltmp4 = call i64 @e1000_init()
  store i64 %calltmp4, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  br label %merge_169

then_170:                                         ; preds = %merge_169
  %calltmp6 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.50 to i64))
  ret i64 0
}

define i64 @"net_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%etype" = alloca i64, align 8
  %"%len" = alloca i64, align 8
  %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9" = alloca i64, align 8
  %"%pkt_buf" = alloca i64, align 8
  br i1 true, label %then_172, label %merge_173

merge_173:                                        ; preds = %entry
  %calltmp = call i64 @"net_\D9\81\D8\AD\D8\B5_\D8\B1\D8\A7\D8\A8\D8\B7"()
  store i64 0, ptr %"%pkt_buf", align 8
  %"%pkt_buf.load" = load i64, ptr %"%pkt_buf", align 8
  %cmpeqtmp = icmp eq i64 %"%pkt_buf.load", 0
  br i1 %cmpeqtmp, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  store i64 0, ptr %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9", align 8
  store i64 0, ptr %"%len", align 8
  br label %while_cond_176

merge_180:                                        ; preds = %merge_184, %while_body_177
  %"%pkt_buf.load8" = load i64, ptr %"%pkt_buf", align 8
  %calltmp9 = call i64 @eth_is_for_us(i64 %"%pkt_buf.load8")
  %cmpeqtmp10 = icmp eq i64 %calltmp9, 0
  br i1 %cmpeqtmp10, label %then_185, label %merge_186

merge_182:                                        ; preds = %then_181, %then_179
  %"%etype.load12" = load i64, ptr %"%etype", align 8
  %cmpeqtmp13 = icmp eq i64 %"%etype.load12", 0
  br i1 %cmpeqtmp13, label %then_183, label %merge_184

merge_184:                                        ; preds = %then_183, %merge_182
  %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9.load" = load i64, ptr %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9", align 8
  %addtmp = add i64 %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9.load", 1
  store i64 %addtmp, ptr %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9", align 8
  br label %merge_180

merge_186:                                        ; preds = %then_185, %merge_180
  %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9.load15" = load i64, ptr %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9", align 8
  %cmpgttmp16 = icmp sgt i64 %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9.load15", 16
  br i1 %cmpgttmp16, label %then_187, label %merge_188

merge_188:                                        ; preds = %merge_186
  store i64 0, ptr %"%len", align 8
  br label %while_cond_176

then_172:                                         ; preds = %entry
  ret i64 0

then_174:                                         ; preds = %merge_173
  ret i64 0

then_179:                                         ; preds = %while_body_177
  %"%pkt_buf.load5" = load i64, ptr %"%pkt_buf", align 8
  %calltmp6 = call i64 @eth_parse_type(i64 %"%pkt_buf.load5")
  store i64 %calltmp6, ptr %"%etype", align 8
  %"%etype.load" = load i64, ptr %"%etype", align 8
  %cmpeqtmp7 = icmp eq i64 %"%etype.load", 0
  br i1 %cmpeqtmp7, label %then_181, label %merge_182

then_181:                                         ; preds = %then_179
  %calltmp11 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  br label %merge_182

then_183:                                         ; preds = %merge_182
  %calltmp14 = call i64 @"net_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  br label %merge_184

then_185:                                         ; preds = %merge_180
  br label %merge_186

then_187:                                         ; preds = %merge_186
  br label %while_exit_178

while_body_177:                                   ; preds = %while_cond_176
  %"%pkt_buf.load1" = load i64, ptr %"%pkt_buf", align 8
  %calltmp2 = call i64 @eth_is_for_us(i64 %"%pkt_buf.load1")
  %cmpeqtmp3 = icmp eq i64 %calltmp2, 1
  br i1 %cmpeqtmp3, label %then_179, label %merge_180

while_cond_176:                                   ; preds = %merge_188, %merge_175
  %"%len.load" = load i64, ptr %"%len", align 8
  %cmpgttmp = icmp sgt i64 %"%len.load", 0
  br i1 %cmpgttmp, label %while_body_177, label %while_exit_178

while_exit_178:                                   ; preds = %then_187, %while_cond_176
  %"%pkt_buf.load4" = load i64, ptr %"%pkt_buf", align 8
  call void @sad_ll_kfree(i64 %"%pkt_buf.load4")
  %loadtmp = load i64, ptr %"%\D8\AD\D8\B2\D9\85_\D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9", align 8
  ret i64 %loadtmp
}

define void @__sad_main() {
entry:
  %"%\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_\D8\B0\D9\8A\D9\84" = alloca i64, align 8
  %"%\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_\D8\B1\D8\A3\D8\B3" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1" = alloca i64, align 8
  %"%dhcp_\D9\85\D9\83\D8\AA\D9\85\D9\84" = alloca i64, align 8
  %"%\D8\B4\D8\A8\D9\83\D8\A9_\D8\B1\D8\A7\D8\A8\D8\B7_\D9\81\D8\B9\D8\A7\D9\84" = alloca i64, align 8
  %"%\D8\B4\D8\A8\D9\83\D8\A9_\D8\AC\D8\A7\D9\87\D8\B2\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B1\D8\B3\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B7\D8\A7\D8\B1\D8\A7\D8\AA_dropped" = alloca i64, align 8
  %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_rx" = alloca i64, align 8
  %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_tx" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_rx" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_tx" = alloca i64, align 8
  %"%tx_desc_tail" = alloca i64, align 8
  %"%rx_desc_tail" = alloca i64, align 8
  %"%tx_desc_addr" = alloca i64, align 8
  %"%rx_desc_addr" = alloca i64, align 8
  %"%tx_current" = alloca i64, align 8
  %"%tx_buffer_3" = alloca i64, align 8
  %"%tx_buffer_2" = alloca i64, align 8
  %"%tx_buffer_1" = alloca i64, align 8
  %"%tx_buffer_0" = alloca i64, align 8
  %"%rx_buffer_addr" = alloca i64, align 8
  %"%dns_3" = alloca i64, align 8
  %"%dns_2" = alloca i64, align 8
  %"%dns_1" = alloca i64, align 8
  %"%dns_0" = alloca i64, align 8
  %"%gw_3" = alloca i64, align 8
  %"%gw_2" = alloca i64, align 8
  %"%gw_1" = alloca i64, align 8
  %"%gw_0" = alloca i64, align 8
  %"%mask_3" = alloca i64, align 8
  %"%mask_2" = alloca i64, align 8
  %"%mask_1" = alloca i64, align 8
  %"%mask_0" = alloca i64, align 8
  %"%ip_3" = alloca i64, align 8
  %"%ip_2" = alloca i64, align 8
  %"%ip_1" = alloca i64, align 8
  %"%ip_0" = alloca i64, align 8
  %"%mac_5" = alloca i64, align 8
  %"%mac_4" = alloca i64, align 8
  %"%mac_3" = alloca i64, align 8
  %"%mac_2" = alloca i64, align 8
  %"%mac_1" = alloca i64, align 8
  %"%mac_0" = alloca i64, align 8
  %"%nic_mmio_base" = alloca i64, align 8
  %"%nic_io_base" = alloca i64, align 8
  %"%nic_pci_fn" = alloca i64, align 8
  %"%nic_pci_dev" = alloca i64, align 8
  %"%nic_pci_bus" = alloca i64, align 8
  %"%nic_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%DESC_SIZE" = alloca i64, align 8
  %"%NUM_RX_DESC" = alloca i64, align 8
  %"%NUM_TX_DESC" = alloca i64, align 8
  %"%TX_BUF_SIZE" = alloca i64, align 8
  %"%RX_BUF_SIZE" = alloca i64, align 8
  %"%PCI_DEVICE_E1000" = alloca i64, align 8
  %"%PCI_VENDOR_INTEL" = alloca i64, align 8
  %"%PCI_DEVICE_8139" = alloca i64, align 8
  %"%PCI_VENDOR_REALTEK" = alloca i64, align 8
  %"%NIC_E1000" = alloca i64, align 8
  %"%NIC_RTL8139" = alloca i64, align 8
  %"%NIC_NONE" = alloca i64, align 8
  %"%E1000_CTRL_SLU" = alloca i64, align 8
  %"%E1000_CTRL_RST" = alloca i64, align 8
  %"%E1000_RAH0" = alloca i64, align 8
  %"%E1000_RAL0" = alloca i64, align 8
  %"%E1000_TDT" = alloca i64, align 8
  %"%E1000_TDH" = alloca i64, align 8
  %"%E1000_TDLEN" = alloca i64, align 8
  %"%E1000_TDBAH" = alloca i64, align 8
  %"%E1000_TDBAL" = alloca i64, align 8
  %"%E1000_RDT" = alloca i64, align 8
  %"%E1000_RDH" = alloca i64, align 8
  %"%E1000_RDLEN" = alloca i64, align 8
  %"%E1000_RDBAH" = alloca i64, align 8
  %"%E1000_RDBAL" = alloca i64, align 8
  %"%E1000_TCTL" = alloca i64, align 8
  %"%E1000_RCTL" = alloca i64, align 8
  %"%E1000_IMC" = alloca i64, align 8
  %"%E1000_IMS" = alloca i64, align 8
  %"%E1000_ICR" = alloca i64, align 8
  %"%E1000_EERD" = alloca i64, align 8
  %"%E1000_STATUS" = alloca i64, align 8
  %"%E1000_CTRL" = alloca i64, align 8
  %"%RTL_CMD_TX_ENABLE" = alloca i64, align 8
  %"%RTL_CMD_RX_ENABLE" = alloca i64, align 8
  %"%RTL_CMD_RESET" = alloca i64, align 8
  %"%RTL_CONFIG1" = alloca i64, align 8
  %"%RTL_RCR" = alloca i64, align 8
  %"%RTL_TCR" = alloca i64, align 8
  %"%RTL_ISR" = alloca i64, align 8
  %"%RTL_IMR" = alloca i64, align 8
  %"%RTL_CBR" = alloca i64, align 8
  %"%RTL_CAPR" = alloca i64, align 8
  %"%RTL_CMD" = alloca i64, align 8
  %"%RTL_RBSTART" = alloca i64, align 8
  %"%RTL_TSAD0" = alloca i64, align 8
  %"%RTL_TSD0" = alloca i64, align 8
  %"%RTL_MAR0" = alloca i64, align 8
  %"%RTL_IDR4" = alloca i64, align 8
  %"%RTL_IDR0" = alloca i64, align 8
  %"%PCI_CONFIG_DATA" = alloca i64, align 8
  %"%PCI_CONFIG_ADDR" = alloca i64, align 8
  %"%ETH_MIN_FRAME" = alloca i64, align 8
  %"%ETH_MAX_FRAME" = alloca i64, align 8
  %"%ETH_MTU" = alloca i64, align 8
  %"%ETH_HEADER_SIZE" = alloca i64, align 8
  %"%ETHER_TYPE_IPV6" = alloca i64, align 8
  %"%ETHER_TYPE_ARP" = alloca i64, align 8
  %"%ETHER_TYPE_IP" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 2048, ptr %"%ETHER_TYPE_IP", align 8
  store i64 2054, ptr %"%ETHER_TYPE_ARP", align 8
  store i64 34525, ptr %"%ETHER_TYPE_IPV6", align 8
  store i64 14, ptr %"%ETH_HEADER_SIZE", align 8
  store i64 1500, ptr %"%ETH_MTU", align 8
  store i64 1518, ptr %"%ETH_MAX_FRAME", align 8
  store i64 64, ptr %"%ETH_MIN_FRAME", align 8
  store i64 3320, ptr %"%PCI_CONFIG_ADDR", align 8
  store i64 3324, ptr %"%PCI_CONFIG_DATA", align 8
  store i64 0, ptr %"%RTL_IDR0", align 8
  store i64 4, ptr %"%RTL_IDR4", align 8
  store i64 8, ptr %"%RTL_MAR0", align 8
  store i64 16, ptr %"%RTL_TSD0", align 8
  store i64 32, ptr %"%RTL_TSAD0", align 8
  store i64 48, ptr %"%RTL_RBSTART", align 8
  store i64 55, ptr %"%RTL_CMD", align 8
  store i64 56, ptr %"%RTL_CAPR", align 8
  store i64 58, ptr %"%RTL_CBR", align 8
  store i64 60, ptr %"%RTL_IMR", align 8
  store i64 62, ptr %"%RTL_ISR", align 8
  store i64 64, ptr %"%RTL_TCR", align 8
  store i64 68, ptr %"%RTL_RCR", align 8
  store i64 82, ptr %"%RTL_CONFIG1", align 8
  store i64 16, ptr %"%RTL_CMD_RESET", align 8
  store i64 8, ptr %"%RTL_CMD_RX_ENABLE", align 8
  store i64 4, ptr %"%RTL_CMD_TX_ENABLE", align 8
  store i64 0, ptr %"%E1000_CTRL", align 8
  store i64 8, ptr %"%E1000_STATUS", align 8
  store i64 20, ptr %"%E1000_EERD", align 8
  store i64 192, ptr %"%E1000_ICR", align 8
  store i64 208, ptr %"%E1000_IMS", align 8
  store i64 216, ptr %"%E1000_IMC", align 8
  store i64 256, ptr %"%E1000_RCTL", align 8
  store i64 1024, ptr %"%E1000_TCTL", align 8
  store i64 10240, ptr %"%E1000_RDBAL", align 8
  store i64 10244, ptr %"%E1000_RDBAH", align 8
  store i64 10248, ptr %"%E1000_RDLEN", align 8
  store i64 10256, ptr %"%E1000_RDH", align 8
  store i64 10264, ptr %"%E1000_RDT", align 8
  store i64 14336, ptr %"%E1000_TDBAL", align 8
  store i64 14340, ptr %"%E1000_TDBAH", align 8
  store i64 14344, ptr %"%E1000_TDLEN", align 8
  store i64 14352, ptr %"%E1000_TDH", align 8
  store i64 14360, ptr %"%E1000_TDT", align 8
  store i64 21504, ptr %"%E1000_RAL0", align 8
  store i64 21508, ptr %"%E1000_RAH0", align 8
  store i64 67108864, ptr %"%E1000_CTRL_RST", align 8
  store i64 64, ptr %"%E1000_CTRL_SLU", align 8
  store i64 0, ptr %"%NIC_NONE", align 8
  store i64 1, ptr %"%NIC_RTL8139", align 8
  store i64 2, ptr %"%NIC_E1000", align 8
  store i64 4332, ptr %"%PCI_VENDOR_REALTEK", align 8
  store i64 33057, ptr %"%PCI_DEVICE_8139", align 8
  store i64 32902, ptr %"%PCI_VENDOR_INTEL", align 8
  store i64 4096, ptr %"%PCI_DEVICE_E1000", align 8
  store i64 8192, ptr %"%RX_BUF_SIZE", align 8
  store i64 1536, ptr %"%TX_BUF_SIZE", align 8
  store i64 4, ptr %"%NUM_TX_DESC", align 8
  store i64 32, ptr %"%NUM_RX_DESC", align 8
  store i64 16, ptr %"%DESC_SIZE", align 8
  %"%NIC_NONE.load" = load i64, ptr %"%NIC_NONE", align 8
  store i64 %"%NIC_NONE.load", ptr %"%nic_\D9\86\D9\88\D8\B9", align 8
  store i64 0, ptr %"%nic_pci_bus", align 8
  store i64 0, ptr %"%nic_pci_dev", align 8
  store i64 0, ptr %"%nic_pci_fn", align 8
  store i64 0, ptr %"%nic_io_base", align 8
  store i64 0, ptr %"%nic_mmio_base", align 8
  store i64 0, ptr %"%mac_0", align 8
  store i64 0, ptr %"%mac_1", align 8
  store i64 0, ptr %"%mac_2", align 8
  store i64 0, ptr %"%mac_3", align 8
  store i64 0, ptr %"%mac_4", align 8
  store i64 0, ptr %"%mac_5", align 8
  store i64 10, ptr %"%ip_0", align 8
  store i64 0, ptr %"%ip_1", align 8
  store i64 2, ptr %"%ip_2", align 8
  store i64 15, ptr %"%ip_3", align 8
  store i64 255, ptr %"%mask_0", align 8
  store i64 255, ptr %"%mask_1", align 8
  store i64 255, ptr %"%mask_2", align 8
  store i64 0, ptr %"%mask_3", align 8
  store i64 10, ptr %"%gw_0", align 8
  store i64 0, ptr %"%gw_1", align 8
  store i64 2, ptr %"%gw_2", align 8
  store i64 2, ptr %"%gw_3", align 8
  store i64 8, ptr %"%dns_0", align 8
  store i64 8, ptr %"%dns_1", align 8
  store i64 8, ptr %"%dns_2", align 8
  store i64 8, ptr %"%dns_3", align 8
  store i64 0, ptr %"%rx_buffer_addr", align 8
  store i64 0, ptr %"%tx_buffer_0", align 8
  store i64 0, ptr %"%tx_buffer_1", align 8
  store i64 0, ptr %"%tx_buffer_2", align 8
  store i64 0, ptr %"%tx_buffer_3", align 8
  store i64 0, ptr %"%tx_current", align 8
  store i64 0, ptr %"%rx_desc_addr", align 8
  store i64 0, ptr %"%tx_desc_addr", align 8
  store i64 0, ptr %"%rx_desc_tail", align 8
  store i64 0, ptr %"%tx_desc_tail", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_tx", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_rx", align 8
  store i64 0, ptr %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_tx", align 8
  store i64 0, ptr %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_rx", align 8
  store i64 0, ptr %"%\D8\A5\D8\B7\D8\A7\D8\B1\D8\A7\D8\AA_dropped", align 8
  store i64 0, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B1\D8\B3\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D8\B4\D8\A8\D9\83\D8\A9_\D8\AC\D8\A7\D9\87\D8\B2\D8\A9", align 8
  store i64 0, ptr %"%\D8\B4\D8\A8\D9\83\D8\A9_\D8\B1\D8\A7\D8\A8\D8\B7_\D9\81\D8\B9\D8\A7\D9\84", align 8
  store i64 0, ptr %"%dhcp_\D9\85\D9\83\D8\AA\D9\85\D9\84", align 8
  store i64 64, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1", align 8
  store i64 0, ptr %"%\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_\D8\B1\D8\A3\D8\B3", align 8
  store i64 0, ptr %"%\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_\D8\B0\D9\8A\D9\84", align 8
  store i64 0, ptr %"%\D8\AD\D8\B2\D9\85_\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1_\D8\B9\D8\AF\D8\AF", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
