; ModuleID = 'طاقة_أجهزة_نحلة'
source_filename = "\D8\B7\D8\A7\D9\82\D8\A9_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [4 x i8] c"CPU\00", align 1
@str.const.1 = private unnamed_addr constant [4 x i8] c"GPU\00", align 1
@str.const.2 = private unnamed_addr constant [8 x i8] c"Display\00", align 1
@str.const.3 = private unnamed_addr constant [8 x i8] c"Storage\00", align 1
@str.const.4 = private unnamed_addr constant [8 x i8] c"Network\00", align 1
@str.const.5 = private unnamed_addr constant [4 x i8] c"USB\00", align 1
@str.const.6 = private unnamed_addr constant [6 x i8] c"Audio\00", align 1
@str.const.7 = private unnamed_addr constant [6 x i8] c"Input\00", align 1
@str.const.8 = private unnamed_addr constant [6 x i8] c"Other\00", align 1
@str.const.9 = private unnamed_addr constant [11 x i8] c"D0(Active)\00", align 1
@str.const.10 = private unnamed_addr constant [9 x i8] c"D1(Idle)\00", align 1
@str.const.11 = private unnamed_addr constant [9 x i8] c"D2(Deep)\00", align 1
@str.const.12 = private unnamed_addr constant [6 x i8] c"D3hot\00", align 1
@str.const.13 = private unnamed_addr constant [7 x i8] c"D3cold\00", align 1
@str.const.14 = private unnamed_addr constant [10 x i8] c"[DP] PCI \00", align 1
@str.const.15 = private unnamed_addr constant [2 x i8] c":\00", align 1
@str.const.16 = private unnamed_addr constant [2 x i8] c".\00", align 1
@str.const.17 = private unnamed_addr constant [6 x i8] c" \E2\86\92 \00", align 1
@str.const.18 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.19 = private unnamed_addr constant [29 x i8] c"[DP] \D9\81\D8\AD\D8\B5 \D9\86\D8\A7\D9\82\D9\84 PCI...\0A\00", align 1
@str.const.20 = private unnamed_addr constant [35 x i8] c"[DP] \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A3\D8\AC\D9\87\D8\B2\D8\A9 PCI: \00", align 1
@str.const.22 = private unnamed_addr constant [12 x i8] c"[DP]   PCI \00", align 1
@str.const.24 = private unnamed_addr constant [15 x i8] c".0 \E2\80\94 Vendor=\00", align 1
@str.const.25 = private unnamed_addr constant [6 x i8] c" Dev=\00", align 1
@str.const.26 = private unnamed_addr constant [8 x i8] c" Class=\00", align 1
@str.const.27 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.28 = private unnamed_addr constant [6 x i8] c" [PM@\00", align 1
@str.const.29 = private unnamed_addr constant [2 x i8] c"]\00", align 1
@str.const.31 = private unnamed_addr constant [15 x i8] c"[DP] DPMS \E2\86\92 \00", align 1
@str.const.32 = private unnamed_addr constant [4 x i8] c"ON\0A\00", align 1
@str.const.33 = private unnamed_addr constant [9 x i8] c"STANDBY\0A\00", align 1
@str.const.34 = private unnamed_addr constant [9 x i8] c"SUSPEND\0A\00", align 1
@str.const.35 = private unnamed_addr constant [5 x i8] c"OFF\0A\00", align 1
@str.const.36 = private unnamed_addr constant [29 x i8] c"[DP] \D8\B3\D8\B7\D9\88\D8\B9 \D8\A7\D9\84\D8\B4\D8\A7\D8\B4\D8\A9: \00", align 1
@str.const.37 = private unnamed_addr constant [3 x i8] c"%\0A\00", align 1
@str.const.38 = private unnamed_addr constant [15 x i8] c"[DP] \D8\AC\D9\87\D8\A7\D8\B2 \00", align 1
@str.const.39 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.40 = private unnamed_addr constant [4 x i8] c"): \00", align 1
@str.const.43 = private unnamed_addr constant [36 x i8] c"[DP] RPM: GPU \E2\86\92 D3hot (\D8\AE\D9\85\D9\88\D9\84)\0A\00", align 1
@str.const.44 = private unnamed_addr constant [37 x i8] c"[DP] RPM: Storage \E2\86\92 D2 (\D8\AE\D9\85\D9\88\D9\84)\0A\00", align 1
@str.const.45 = private unnamed_addr constant [33 x i8] c"[DP] RPM: USB \E2\86\92 D2 (\D8\AE\D9\85\D9\88\D9\84)\0A\00", align 1
@str.const.46 = private unnamed_addr constant [38 x i8] c"[DP] RPM: Audio \E2\86\92 D3hot (\D8\AE\D9\85\D9\88\D9\84)\0A\00", align 1
@str.const.47 = private unnamed_addr constant [26 x i8] c"[DP] \D8\A5\D9\8A\D9\82\D8\A7\D8\B8 \D8\AC\D9\87\D8\A7\D8\B2 \00", align 1
@str.const.48 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\B9\D9\84\D8\A7\D9\85: \00", align 1
@str.const.50 = private unnamed_addr constant [44 x i8] c"[DP] \D8\A5\D9\8A\D9\82\D8\A7\D8\B8 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9...\0A\00", align 1
@str.const.51 = private unnamed_addr constant [43 x i8] c"[DP] \E2\9C\93 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D9\86\D8\B4\D8\B7\D8\A9\0A\00", align 1
@str.const.52 = private unnamed_addr constant [44 x i8] c"[DP] \D8\A5\D9\8A\D9\82\D8\A7\D9\81 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9...\0A\00", align 1
@str.const.53 = private unnamed_addr constant [57 x i8] c"[DP] \E2\9C\93 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D9\81\D9\8A \D9\88\D8\B6\D8\B9 \D8\A7\D9\84\D9\86\D9\88\D9\85\0A\00", align 1
@str.const.54 = private unnamed_addr constant [62 x i8] c"\0A[DP] \E2\95\90\E2\95\90\E2\95\90 \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\B7\D8\A7\D9\82\D8\A9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.55 = private unnamed_addr constant [9 x i8] c"[DP] #0 \00", align 1
@str.const.56 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.57 = private unnamed_addr constant [6 x i8] c" \E2\80\94 \00", align 1
@str.const.58 = private unnamed_addr constant [5 x i8] c" mW\0A\00", align 1
@str.const.59 = private unnamed_addr constant [9 x i8] c"[DP] #1 \00", align 1
@str.const.63 = private unnamed_addr constant [9 x i8] c"[DP] #2 \00", align 1
@str.const.66 = private unnamed_addr constant [4 x i8] c" mW\00", align 1
@str.const.67 = private unnamed_addr constant [13 x i8] c" (\D8\B3\D8\B7\D9\88\D8\B9: \00", align 1
@str.const.68 = private unnamed_addr constant [3 x i8] c"%)\00", align 1
@str.const.70 = private unnamed_addr constant [9 x i8] c"[DP] #3 \00", align 1
@str.const.74 = private unnamed_addr constant [9 x i8] c"[DP] #4 \00", align 1
@str.const.78 = private unnamed_addr constant [7 x i8] c" [WoL]\00", align 1
@str.const.80 = private unnamed_addr constant [9 x i8] c"[DP] #5 \00", align 1
@str.const.84 = private unnamed_addr constant [9 x i8] c"[DP] #6 \00", align 1
@str.const.88 = private unnamed_addr constant [82 x i8] c"[DP] \E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\0A\00", align 1
@str.const.89 = private unnamed_addr constant [20 x i8] c"[DP] \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A: \00", align 1
@str.const.90 = private unnamed_addr constant [6 x i8] c" mW (\00", align 1
@str.const.91 = private unnamed_addr constant [5 x i8] c" W)\0A\00", align 1
@str.const.92 = private unnamed_addr constant [29 x i8] c"[DP] \D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D9\86\D8\A7\D8\A6\D9\85\D8\A9: \00", align 1
@str.const.95 = private unnamed_addr constant [26 x i8] c"[DP] \D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA D: \00", align 1
@str.const.96 = private unnamed_addr constant [20 x i8] c"\D8\8C \D8\A5\D9\8A\D9\82\D8\A7\D8\B8\D8\A7\D8\AA: \00", align 1
@str.const.98 = private unnamed_addr constant [22 x i8] c"[DP] \D8\A3\D8\AC\D9\87\D8\B2\D8\A9 PCI: \00", align 1
@str.const.100 = private unnamed_addr constant [12 x i8] c"[DP] DPMS: \00", align 1
@str.const.101 = private unnamed_addr constant [3 x i8] c"ON\00", align 1
@str.const.102 = private unnamed_addr constant [8 x i8] c"STANDBY\00", align 1
@str.const.103 = private unnamed_addr constant [8 x i8] c"SUSPEND\00", align 1
@str.const.104 = private unnamed_addr constant [4 x i8] c"OFF\00", align 1
@str.const.106 = private unnamed_addr constant [89 x i8] c"[DP] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\0A\00", align 1
@str.const.108 = private unnamed_addr constant [124 x i8] c"[DP] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.109 = private unnamed_addr constant [63 x i8] c"[DP]  \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\B7\D8\A7\D9\82\D8\A9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \E2\80\94 BeeOS\0A\00", align 1
@str.const.111 = private unnamed_addr constant [37 x i8] c"[DP] \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A9:\0A\00", align 1
@str.const.112 = private unnamed_addr constant [9 x i8] c"[DP]   #\00", align 1
@str.const.114 = private unnamed_addr constant [57 x i8] c"[DP] \E2\9C\93 \D8\A5\D8\AF\D8\A7\D8\B1\D8\A9 \D8\B7\D8\A7\D9\82\D8\A9 \D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9 \D8\AC\D8\A7\D9\87\D8\B2\D8\A9\0A\0A\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_port_inl(i64)

declare void @sad_ll_port_outl(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_timer_get_ticks()

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_kfree(i64)

define void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%\D8\B1\D9\82" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_7" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_6" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_5" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_4" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_3" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_2" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_1" = alloca i64, align 8
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_0" = alloca i64, align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86" = alloca i64, align 8
  %"%\D8\B3\D8\A7\D9\84\D8\A8" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

merge_11:                                         ; preds = %then_10, %while_body_8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load7" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load7", 1
  br i1 %cmpeqtmp8, label %then_12, label %merge_13

merge_13:                                         ; preds = %then_12, %merge_11
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load10" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load10", 2
  br i1 %cmpeqtmp11, label %then_14, label %merge_15

merge_15:                                         ; preds = %then_14, %merge_13
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load13" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load13", 3
  br i1 %cmpeqtmp14, label %then_16, label %merge_17

merge_17:                                         ; preds = %then_16, %merge_15
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load16" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load16", 4
  br i1 %cmpeqtmp17, label %then_18, label %merge_19

merge_19:                                         ; preds = %then_18, %merge_17
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load19" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp20 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load19", 5
  br i1 %cmpeqtmp20, label %then_20, label %merge_21

merge_21:                                         ; preds = %then_20, %merge_19
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load22" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load22", 6
  br i1 %cmpeqtmp23, label %then_22, label %merge_23

merge_23:                                         ; preds = %then_22, %merge_21
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load25" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp26 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load25", 7
  br i1 %cmpeqtmp26, label %then_24, label %merge_25

merge_25:                                         ; preds = %then_24, %merge_23
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load28" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load29" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  %subtmp30 = sub i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load28", %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load29"
  %divtmp = sdiv i64 %subtmp30, 10
  store i64 %divtmp, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load31" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load31", 1
  store i64 %addtmp, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  br label %while_cond_7

merge_27:                                         ; preds = %then_26, %while_exit_9
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load32" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %subtmp33 = sub i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load32", 1
  store i64 %subtmp33, ptr %"%\D8\B9", align 8
  br label %while_cond_28

merge_32:                                         ; preds = %then_31, %while_body_29
  %"%\D8\B9.load36" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp37 = icmp eq i64 %"%\D8\B9.load36", 1
  br i1 %cmpeqtmp37, label %then_33, label %merge_34

merge_34:                                         ; preds = %then_33, %merge_32
  %"%\D8\B9.load38" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp39 = icmp eq i64 %"%\D8\B9.load38", 2
  br i1 %cmpeqtmp39, label %then_35, label %merge_36

merge_36:                                         ; preds = %then_35, %merge_34
  %"%\D8\B9.load40" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp41 = icmp eq i64 %"%\D8\B9.load40", 3
  br i1 %cmpeqtmp41, label %then_37, label %merge_38

merge_38:                                         ; preds = %then_37, %merge_36
  %"%\D8\B9.load42" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp43 = icmp eq i64 %"%\D8\B9.load42", 4
  br i1 %cmpeqtmp43, label %then_39, label %merge_40

merge_4:                                          ; preds = %entry
  store i64 0, ptr %"%\D8\B3\D8\A7\D9\84\D8\A8", align 8
  %"%10.load" = load i64, ptr %"\D9\861", align 8
  store i64 %"%10.load", ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

merge_40:                                         ; preds = %then_39, %merge_38
  %"%\D8\B9.load44" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp45 = icmp eq i64 %"%\D8\B9.load44", 5
  br i1 %cmpeqtmp45, label %then_41, label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %"%\D8\B9.load46" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp47 = icmp eq i64 %"%\D8\B9.load46", 6
  br i1 %cmpeqtmp47, label %then_43, label %merge_44

merge_44:                                         ; preds = %then_43, %merge_42
  %"%\D8\B9.load48" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp49 = icmp eq i64 %"%\D8\B9.load48", 7
  br i1 %cmpeqtmp49, label %then_45, label %merge_46

merge_46:                                         ; preds = %then_45, %merge_44
  %"%\D8\B9.load50" = load i64, ptr %"%\D8\B9", align 8
  %subtmp51 = sub i64 %"%\D8\B9.load50", 1
  store i64 %subtmp51, ptr %"%\D8\B9", align 8
  br label %while_cond_28

merge_6:                                          ; preds = %then_5, %merge_4
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  store i64 0, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  br label %while_cond_7

then_10:                                          ; preds = %while_body_8
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  br label %merge_11

then_12:                                          ; preds = %merge_11
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load9" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load9", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  br label %merge_13

then_14:                                          ; preds = %merge_13
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load12" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load12", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  br label %merge_15

then_16:                                          ; preds = %merge_15
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load15" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load15", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  br label %merge_17

then_18:                                          ; preds = %merge_17
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load18" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load18", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  br label %merge_19

then_20:                                          ; preds = %merge_19
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load21" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load21", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  br label %merge_21

then_22:                                          ; preds = %merge_21
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load24" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load24", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  br label %merge_23

then_24:                                          ; preds = %merge_23
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load27" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82.load27", ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  br label %merge_25

then_26:                                          ; preds = %while_exit_9
  br label %merge_27

then_3:                                           ; preds = %entry
  ret i64 0

then_31:                                          ; preds = %while_body_29
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_0.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_0.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_32

then_33:                                          ; preds = %merge_32
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_1.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_1.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_34

then_35:                                          ; preds = %merge_34
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_2.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_2.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_36

then_37:                                          ; preds = %merge_36
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_3.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_3.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_38

then_39:                                          ; preds = %merge_38
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_4.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_4.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_40

then_41:                                          ; preds = %merge_40
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_5.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_5.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_42

then_43:                                          ; preds = %merge_42
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_6.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_6.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_44

then_45:                                          ; preds = %merge_44
  %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_7.load" = load i64, ptr %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  store i64 %"%\D8\A7\D8\B1\D9\82\D8\A7\D9\85_7.load", ptr %"%\D8\B1\D9\82", align 8
  br label %merge_46

then_5:                                           ; preds = %merge_4
  store i64 1, ptr %"%\D8\B3\D8\A7\D9\84\D8\A8", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load2" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %subtmp = sub i64 0, %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load2"
  store i64 %subtmp, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  br label %merge_6

while_body_29:                                    ; preds = %while_cond_28
  store i64 0, ptr %"%\D8\B1\D9\82", align 8
  %"%\D8\B9.load34" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp35 = icmp eq i64 %"%\D8\B9.load34", 0
  br i1 %cmpeqtmp35, label %then_31, label %merge_32

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load4" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %modtmp = srem i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load4", 10
  store i64 %modtmp, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D9\82", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load", 0
  br i1 %cmpeqtmp5, label %then_10, label %merge_11

while_cond_28:                                    ; preds = %merge_46, %merge_27
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9.load", 0
  br i1 %cmpgetmp, label %while_body_29, label %while_exit_30

while_cond_7:                                     ; preds = %merge_25, %merge_6
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load3" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D9\86.load3", 0
  br i1 %cmpgttmp, label %while_body_8, label %while_exit_9

while_exit_30:                                    ; preds = %while_cond_28
  ret i64 0

while_exit_9:                                     ; preds = %while_cond_7
  %"%\D8\B3\D8\A7\D9\84\D8\A8.load" = load i64, ptr %"%\D8\B3\D8\A7\D9\84\D8\A8", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\B3\D8\A7\D9\84\D8\A8.load", 1
  br i1 %cmpeqtmp6, label %then_26, label %merge_27
}

define i64 @dp_serial_hex(i64 %"\D9\86") {
entry:
  %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A" = alloca i64, align 8
  %"%\D9\8A" = alloca i64, align 8
  %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  store i64 0, ptr %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9", align 8
  store i64 28, ptr %"%\D9\8A", align 8
  br label %while_cond_47

merge_51:                                         ; preds = %then_50, %while_body_48
  %"%\D9\8A.load4" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\8A.load4", 24
  br i1 %cmpeqtmp5, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%\D9\8A.load9" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D9\8A.load9", 20
  br i1 %cmpeqtmp10, label %then_54, label %merge_55

merge_55:                                         ; preds = %then_54, %merge_53
  %"%\D9\8A.load14" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\8A.load14", 16
  br i1 %cmpeqtmp15, label %then_56, label %merge_57

merge_57:                                         ; preds = %then_56, %merge_55
  %"%\D9\8A.load19" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp20 = icmp eq i64 %"%\D9\8A.load19", 12
  br i1 %cmpeqtmp20, label %then_58, label %merge_59

merge_59:                                         ; preds = %then_58, %merge_57
  %"%\D9\8A.load24" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp25 = icmp eq i64 %"%\D9\8A.load24", 8
  br i1 %cmpeqtmp25, label %then_60, label %merge_61

merge_61:                                         ; preds = %then_60, %merge_59
  %"%\D9\8A.load29" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp30 = icmp eq i64 %"%\D9\8A.load29", 4
  br i1 %cmpeqtmp30, label %then_62, label %merge_63

merge_63:                                         ; preds = %then_62, %merge_61
  %"%\D9\8A.load34" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp35 = icmp eq i64 %"%\D9\8A.load34", 0
  br i1 %cmpeqtmp35, label %then_64, label %merge_65

merge_65:                                         ; preds = %then_64, %merge_63
  %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load" = load i64, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load", 0
  br i1 %cmpgttmp, label %then_66, label %merge_67

merge_67:                                         ; preds = %then_66, %merge_65
  %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9.load38" = load i64, ptr %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9", align 8
  %cmpeqtmp39 = icmp eq i64 %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9.load38", 1
  br i1 %cmpeqtmp39, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_73, %merge_67
  %"%\D9\8A.load41" = load i64, ptr %"%\D9\8A", align 8
  %subtmp = sub i64 %"%\D9\8A.load41", 4
  store i64 %subtmp, ptr %"%\D9\8A", align 8
  br label %while_cond_47

merge_71:                                         ; preds = %then_70, %then_68
  %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load42" = load i64, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  %cmpgetmp43 = icmp sge i64 %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load42", 10
  br i1 %cmpgetmp43, label %then_72, label %merge_73

merge_73:                                         ; preds = %then_72, %merge_71
  br label %merge_69

merge_75:                                         ; preds = %then_74, %while_exit_49
  ret i64 0

then_50:                                          ; preds = %while_body_48
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %shrtmp = lshr i64 %"%\D9\86.load", 28
  %andtmp = and i64 %shrtmp, 15
  store i64 %andtmp, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  %"%\D9\86.load6" = load i64, ptr %"\D9\861", align 8
  %shrtmp7 = lshr i64 %"%\D9\86.load6", 24
  %andtmp8 = and i64 %shrtmp7, 15
  store i64 %andtmp8, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_53
  %"%\D9\86.load11" = load i64, ptr %"\D9\861", align 8
  %shrtmp12 = lshr i64 %"%\D9\86.load11", 20
  %andtmp13 = and i64 %shrtmp12, 15
  store i64 %andtmp13, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_55

then_56:                                          ; preds = %merge_55
  %"%\D9\86.load16" = load i64, ptr %"\D9\861", align 8
  %shrtmp17 = lshr i64 %"%\D9\86.load16", 16
  %andtmp18 = and i64 %shrtmp17, 15
  store i64 %andtmp18, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_57

then_58:                                          ; preds = %merge_57
  %"%\D9\86.load21" = load i64, ptr %"\D9\861", align 8
  %shrtmp22 = lshr i64 %"%\D9\86.load21", 12
  %andtmp23 = and i64 %shrtmp22, 15
  store i64 %andtmp23, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_59

then_60:                                          ; preds = %merge_59
  %"%\D9\86.load26" = load i64, ptr %"\D9\861", align 8
  %shrtmp27 = lshr i64 %"%\D9\86.load26", 8
  %andtmp28 = and i64 %shrtmp27, 15
  store i64 %andtmp28, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_61

then_62:                                          ; preds = %merge_61
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %shrtmp32 = lshr i64 %"%\D9\86.load31", 4
  %andtmp33 = and i64 %shrtmp32, 15
  store i64 %andtmp33, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_63

then_64:                                          ; preds = %merge_63
  %"%\D9\86.load36" = load i64, ptr %"\D9\861", align 8
  %andtmp37 = and i64 %"%\D9\86.load36", 15
  store i64 %andtmp37, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  br label %merge_65

then_66:                                          ; preds = %merge_65
  store i64 1, ptr %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9", align 8
  br label %merge_67

then_68:                                          ; preds = %merge_67
  %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load40" = load i64, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A.load40", 10
  br i1 %cmplttmp, label %then_70, label %merge_71

then_70:                                          ; preds = %then_68
  br label %merge_71

then_72:                                          ; preds = %merge_71
  br label %merge_73

then_74:                                          ; preds = %while_exit_49
  br label %merge_75

while_body_48:                                    ; preds = %while_cond_47
  store i64 0, ptr %"%\D8\B1\D8\A8\D8\A7\D8\B9\D9\8A", align 8
  %"%\D9\8A.load2" = load i64, ptr %"%\D9\8A", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\8A.load2", 28
  br i1 %cmpeqtmp, label %then_50, label %merge_51

while_cond_47:                                    ; preds = %merge_69, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\8A.load", 0
  br i1 %cmpgetmp, label %while_body_48, label %while_exit_49

while_exit_49:                                    ; preds = %while_cond_47
  %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9.load" = load i64, ptr %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AA\D9\85_\D8\B7\D8\A8\D8\B9.load", 0
  br i1 %cmpeqtmp3, label %then_74, label %merge_75
}

define i64 @"dp_\D8\A7\D8\B7\D8\A8\D8\B9_\D9\86\D9\88\D8\B9"(i64 %"\D9\86\D9\88\D8\B9_\D8\AC") {
entry:
  %"\D9\86\D9\88\D8\B9_\D8\AC1" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\AC", ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load", 0
  br i1 %cmpeqtmp, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %entry
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load2" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load2", 1
  br i1 %cmpeqtmp3, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load4" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load4", 2
  br i1 %cmpeqtmp5, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load6" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load6", 3
  br i1 %cmpeqtmp7, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load8" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load8", 4
  br i1 %cmpeqtmp9, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load10" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load10", 5
  br i1 %cmpeqtmp11, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load12" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load12", 6
  br i1 %cmpeqtmp13, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %merge_87
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load14" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load14", 7
  br i1 %cmpeqtmp15, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load16" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load16", 8
  br i1 %cmpeqtmp17, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  ret i64 0

then_76:                                          ; preds = %entry
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  br label %merge_77

then_78:                                          ; preds = %merge_77
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  br label %merge_79

then_80:                                          ; preds = %merge_79
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  br label %merge_81

then_82:                                          ; preds = %merge_81
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  br label %merge_83

then_84:                                          ; preds = %merge_83
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  br label %merge_85

then_86:                                          ; preds = %merge_85
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  br label %merge_87

then_88:                                          ; preds = %merge_87
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  br label %merge_89

then_90:                                          ; preds = %merge_89
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  br label %merge_91

then_92:                                          ; preds = %merge_91
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  br label %merge_93
}

define i64 @"dp_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\A7\D9\84"(i64 %d_state) {
entry:
  %d_state1 = alloca i64, align 8
  store i64 %d_state, ptr %d_state1, align 8
  %"%d_state.load" = load i64, ptr %d_state1, align 8
  %cmpeqtmp = icmp eq i64 %"%d_state.load", 0
  br i1 %cmpeqtmp, label %then_94, label %merge_95

merge_101:                                        ; preds = %then_100, %merge_99
  %"%d_state.load8" = load i64, ptr %d_state1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%d_state.load8", 4
  br i1 %cmpeqtmp9, label %then_102, label %merge_103

merge_103:                                        ; preds = %then_102, %merge_101
  ret i64 0

merge_95:                                         ; preds = %then_94, %entry
  %"%d_state.load2" = load i64, ptr %d_state1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%d_state.load2", 1
  br i1 %cmpeqtmp3, label %then_96, label %merge_97

merge_97:                                         ; preds = %then_96, %merge_95
  %"%d_state.load4" = load i64, ptr %d_state1, align 8
  %cmpeqtmp5 = icmp eq i64 %"%d_state.load4", 2
  br i1 %cmpeqtmp5, label %then_98, label %merge_99

merge_99:                                         ; preds = %then_98, %merge_97
  %"%d_state.load6" = load i64, ptr %d_state1, align 8
  %cmpeqtmp7 = icmp eq i64 %"%d_state.load6", 3
  br i1 %cmpeqtmp7, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  br label %merge_101

then_102:                                         ; preds = %merge_101
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  br label %merge_103

then_94:                                          ; preds = %entry
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  br label %merge_95

then_96:                                          ; preds = %merge_95
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  br label %merge_97

then_98:                                          ; preds = %merge_97
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  br label %merge_99
}

define i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %bus, i64 %dev, i64 %"\D9\81\D9\86\D9\83", i64 %offset) {
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

define i64 @"pci_\D8\A7\D9\83\D8\AA\D8\A8"(i64 %bus, i64 %dev, i64 %"\D9\81\D9\86\D9\83", i64 %offset, i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
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

define i64 @"pci_\D8\A7\D8\A8\D8\AD\D8\AB_pm_cap"(i64 %bus, i64 %dev, i64 %"\D9\81\D9\86\D9\83") {
entry:
  %"%cap_id" = alloca i64, align 8
  %"%cap_hdr" = alloca i64, align 8
  %"%\D8\AD\D8\AF" = alloca i64, align 8
  %"%cap_ptr" = alloca i64, align 8
  %"%status" = alloca i64, align 8
  %"%status_cmd" = alloca i64, align 8
  %"\D9\81\D9\86\D9\833" = alloca i64, align 8
  store i64 %"\D9\81\D9\86\D9\83", ptr %"\D9\81\D9\86\D9\833", align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%bus.load" = load i64, ptr %bus1, align 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %"%\D9\81\D9\86\D9\83.load" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %calltmp = call i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%bus.load", i64 %"%dev.load", i64 %"%\D9\81\D9\86\D9\83.load", i64 4)
  store i64 %calltmp, ptr %"%status_cmd", align 8
  %"%status_cmd.load" = load i64, ptr %"%status_cmd", align 8
  %shrtmp = lshr i64 %"%status_cmd.load", 16
  %andtmp = and i64 %shrtmp, 65535
  store i64 %andtmp, ptr %"%status", align 8
  %"%status.load" = load i64, ptr %"%status", align 8
  %shrtmp4 = lshr i64 %"%status.load", 4
  %andtmp5 = and i64 %shrtmp4, 1
  %cmpeqtmp = icmp eq i64 %andtmp5, 0
  br i1 %cmpeqtmp, label %then_104, label %merge_105

merge_105:                                        ; preds = %entry
  %"%bus.load6" = load i64, ptr %bus1, align 8
  %"%dev.load7" = load i64, ptr %dev2, align 8
  %"%\D9\81\D9\86\D9\83.load8" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %calltmp9 = call i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%bus.load6", i64 %"%dev.load7", i64 %"%\D9\81\D9\86\D9\83.load8", i64 52)
  store i64 %calltmp9, ptr %"%cap_ptr", align 8
  %"%cap_ptr.load" = load i64, ptr %"%cap_ptr", align 8
  %andtmp10 = and i64 %"%cap_ptr.load", 255
  store i64 %andtmp10, ptr %"%cap_ptr", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF", align 8
  br label %while_cond_106

merge_110:                                        ; preds = %while_body_107
  %"%bus.load13" = load i64, ptr %bus1, align 8
  %"%dev.load14" = load i64, ptr %dev2, align 8
  %"%\D9\81\D9\86\D9\83.load15" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %"%cap_ptr.load16" = load i64, ptr %"%cap_ptr", align 8
  %calltmp17 = call i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%bus.load13", i64 %"%dev.load14", i64 %"%\D9\81\D9\86\D9\83.load15", i64 %"%cap_ptr.load16")
  store i64 %calltmp17, ptr %"%cap_hdr", align 8
  %"%cap_hdr.load" = load i64, ptr %"%cap_hdr", align 8
  %andtmp18 = and i64 %"%cap_hdr.load", 255
  store i64 %andtmp18, ptr %"%cap_id", align 8
  %"%cap_id.load" = load i64, ptr %"%cap_id", align 8
  %cmpeqtmp19 = icmp eq i64 %"%cap_id.load", 1
  br i1 %cmpeqtmp19, label %then_111, label %merge_112

merge_112:                                        ; preds = %merge_110
  %"%cap_hdr.load20" = load i64, ptr %"%cap_hdr", align 8
  %shrtmp21 = lshr i64 %"%cap_hdr.load20", 8
  %andtmp22 = and i64 %shrtmp21, 255
  store i64 %andtmp22, ptr %"%cap_ptr", align 8
  %"%\D8\AD\D8\AF.load23" = load i64, ptr %"%\D8\AD\D8\AF", align 8
  %addtmp = add i64 %"%\D8\AD\D8\AF.load23", 1
  store i64 %addtmp, ptr %"%\D8\AD\D8\AF", align 8
  br label %while_cond_106

then_104:                                         ; preds = %entry
  ret i64 0

then_109:                                         ; preds = %while_body_107
  ret i64 0

then_111:                                         ; preds = %merge_110
  %loadtmp = load i64, ptr %"%cap_ptr", align 8
  ret i64 %loadtmp

while_body_107:                                   ; preds = %while_cond_106
  %"%\D8\AD\D8\AF.load" = load i64, ptr %"%\D8\AD\D8\AF", align 8
  %cmpgttmp12 = icmp sgt i64 %"%\D8\AD\D8\AF.load", 48
  br i1 %cmpgttmp12, label %then_109, label %merge_110

while_cond_106:                                   ; preds = %merge_112, %merge_105
  %"%cap_ptr.load11" = load i64, ptr %"%cap_ptr", align 8
  %cmpgttmp = icmp sgt i64 %"%cap_ptr.load11", 0
  br i1 %cmpgttmp, label %while_body_107, label %while_exit_108

while_exit_108:                                   ; preds = %while_cond_106
  ret i64 0
}

define i64 @"pci_\D8\B9\D9\8A\D9\86_d_state"(i64 %bus, i64 %dev, i64 %"\D9\81\D9\86\D9\83", i64 %pm_cap, i64 %d_state) {
entry:
  %"%pm_ctrl" = alloca i64, align 8
  %d_state5 = alloca i64, align 8
  store i64 %d_state, ptr %d_state5, align 8
  %pm_cap4 = alloca i64, align 8
  store i64 %pm_cap, ptr %pm_cap4, align 8
  %"\D9\81\D9\86\D9\833" = alloca i64, align 8
  store i64 %"\D9\81\D9\86\D9\83", ptr %"\D9\81\D9\86\D9\833", align 8
  %dev2 = alloca i64, align 8
  store i64 %dev, ptr %dev2, align 8
  %bus1 = alloca i64, align 8
  store i64 %bus, ptr %bus1, align 8
  %"%pm_cap.load" = load i64, ptr %pm_cap4, align 8
  %cmpeqtmp = icmp eq i64 %"%pm_cap.load", 0
  br i1 %cmpeqtmp, label %then_113, label %merge_114

merge_114:                                        ; preds = %entry
  %"%pm_cap.load6" = load i64, ptr %pm_cap4, align 8
  %addtmp = add i64 %"%pm_cap.load6", 0
  %"%bus.load" = load i64, ptr %bus1, align 8
  %"%dev.load" = load i64, ptr %dev2, align 8
  %"%\D9\81\D9\86\D9\83.load" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %calltmp = call i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%bus.load", i64 %"%dev.load", i64 %"%\D9\81\D9\86\D9\83.load", i64 %addtmp)
  store i64 %calltmp, ptr %"%pm_ctrl", align 8
  %"%pm_ctrl.load" = load i64, ptr %"%pm_ctrl", align 8
  %andtmp = and i64 %"%pm_ctrl.load", 3
  %"%pm_ctrl.load7" = load i64, ptr %"%pm_ctrl", align 8
  %subtmp = sub i64 %"%pm_ctrl.load7", %andtmp
  store i64 %subtmp, ptr %"%pm_ctrl", align 8
  %"%d_state.load" = load i64, ptr %d_state5, align 8
  %cmpeqtmp8 = icmp eq i64 %"%d_state.load", 0
  br i1 %cmpeqtmp8, label %then_115, label %merge_116

merge_116:                                        ; preds = %then_115, %merge_114
  %"%d_state.load9" = load i64, ptr %d_state5, align 8
  %cmpeqtmp10 = icmp eq i64 %"%d_state.load9", 0
  br i1 %cmpeqtmp10, label %then_117, label %merge_118

merge_118:                                        ; preds = %then_117, %merge_116
  %"%d_state.load13" = load i64, ptr %d_state5, align 8
  %cmpeqtmp14 = icmp eq i64 %"%d_state.load13", 0
  br i1 %cmpeqtmp14, label %then_119, label %merge_120

merge_120:                                        ; preds = %then_119, %merge_118
  %"%d_state.load17" = load i64, ptr %d_state5, align 8
  %cmpeqtmp18 = icmp eq i64 %"%d_state.load17", 0
  br i1 %cmpeqtmp18, label %then_121, label %merge_122

merge_122:                                        ; preds = %then_121, %merge_120
  %"%pm_cap.load21" = load i64, ptr %pm_cap4, align 8
  %addtmp22 = add i64 %"%pm_cap.load21", 0
  %"%bus.load23" = load i64, ptr %bus1, align 8
  %"%dev.load24" = load i64, ptr %dev2, align 8
  %"%\D9\81\D9\86\D9\83.load25" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %"%pm_ctrl.load26" = load i64, ptr %"%pm_ctrl", align 8
  %calltmp27 = call i64 @"pci_\D8\A7\D9\83\D8\AA\D8\A8"(i64 %"%bus.load23", i64 %"%dev.load24", i64 %"%\D9\81\D9\86\D9\83.load25", i64 %addtmp22, i64 %"%pm_ctrl.load26")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %"%bus.load28" = load i64, ptr %bus1, align 8
  %calltmp29 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%bus.load28")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%dev.load30" = load i64, ptr %dev2, align 8
  %calltmp31 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%dev.load30")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%\D9\81\D9\86\D9\83.load32" = load i64, ptr %"\D9\81\D9\86\D9\833", align 8
  %calltmp33 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\81\D9\86\D9\83.load32")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%d_state.load34" = load i64, ptr %d_state5, align 8
  %calltmp35 = call i64 @"dp_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\A7\D9\84"(i64 %"%d_state.load34")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  ret i64 1

then_113:                                         ; preds = %entry
  ret i64 0

then_115:                                         ; preds = %merge_114
  br label %merge_116

then_117:                                         ; preds = %merge_116
  %"%pm_ctrl.load11" = load i64, ptr %"%pm_ctrl", align 8
  %addtmp12 = add i64 %"%pm_ctrl.load11", 1
  store i64 %addtmp12, ptr %"%pm_ctrl", align 8
  br label %merge_118

then_119:                                         ; preds = %merge_118
  %"%pm_ctrl.load15" = load i64, ptr %"%pm_ctrl", align 8
  %addtmp16 = add i64 %"%pm_ctrl.load15", 2
  store i64 %addtmp16, ptr %"%pm_ctrl", align 8
  br label %merge_120

then_121:                                         ; preds = %merge_120
  %"%pm_ctrl.load19" = load i64, ptr %"%pm_ctrl", align 8
  %addtmp20 = add i64 %"%pm_ctrl.load19", 3
  store i64 %addtmp20, ptr %"%pm_ctrl", align 8
  br label %merge_122
}

define i64 @"pci_\D9\81\D8\AD\D8\B5_\D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9"() {
entry:
  %"%pm" = alloca i64, align 8
  %"%subclass" = alloca i64, align 8
  %"%class_code" = alloca i64, align 8
  %"%class_reg" = alloca i64, align 8
  %"%device_id" = alloca i64, align 8
  %"%vendor_id" = alloca i64, align 8
  %"%vendor_device" = alloca i64, align 8
  %"%dev" = alloca i64, align 8
  %"%bus" = alloca i64, align 8
  %"%\D9\88\D8\AC\D8\AF" = alloca i64, align 8
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  store i64 0, ptr %"%\D9\88\D8\AC\D8\AF", align 8
  store i64 0, ptr %"%bus", align 8
  br label %while_cond_123

merge_130:                                        ; preds = %merge_132, %while_body_127
  %"%dev.load8" = load i64, ptr %"%dev", align 8
  %addtmp9 = add i64 %"%dev.load8", 1
  store i64 %addtmp9, ptr %"%dev", align 8
  br label %while_cond_126

merge_132:                                        ; preds = %merge_152, %then_129
  br label %merge_130

merge_134:                                        ; preds = %then_133, %then_131
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%\D9\88\D8\AC\D8\AF.load34" = load i64, ptr %"%\D9\88\D8\AC\D8\AF", align 8
  %addtmp35 = add i64 %"%\D9\88\D8\AC\D8\AF.load34", 1
  store i64 %addtmp35, ptr %"%\D9\88\D8\AC\D8\AF", align 8
  %"%bus.load36" = load i64, ptr %"%bus", align 8
  %cmpeqtmp = icmp eq i64 %"%bus.load36", 0
  br i1 %cmpeqtmp, label %then_135, label %merge_136

merge_136:                                        ; preds = %merge_138, %merge_134
  %"%bus.load39" = load i64, ptr %"%bus", align 8
  %cmpeqtmp40 = icmp eq i64 %"%bus.load39", 0
  br i1 %cmpeqtmp40, label %then_139, label %merge_140

merge_138:                                        ; preds = %then_137, %then_135
  br label %merge_136

merge_140:                                        ; preds = %merge_142, %merge_136
  %"%bus.load43" = load i64, ptr %"%bus", align 8
  %cmpeqtmp44 = icmp eq i64 %"%bus.load43", 0
  br i1 %cmpeqtmp44, label %then_143, label %merge_144

merge_142:                                        ; preds = %then_141, %then_139
  br label %merge_140

merge_144:                                        ; preds = %merge_146, %merge_140
  %"%bus.load47" = load i64, ptr %"%bus", align 8
  %cmpeqtmp48 = icmp eq i64 %"%bus.load47", 0
  br i1 %cmpeqtmp48, label %then_147, label %merge_148

merge_146:                                        ; preds = %then_145, %then_143
  br label %merge_144

merge_148:                                        ; preds = %merge_150, %merge_144
  %"%bus.load51" = load i64, ptr %"%bus", align 8
  %cmpeqtmp52 = icmp eq i64 %"%bus.load51", 0
  br i1 %cmpeqtmp52, label %then_151, label %merge_152

merge_150:                                        ; preds = %then_149, %then_147
  br label %merge_148

merge_152:                                        ; preds = %merge_154, %merge_148
  br label %merge_132

merge_154:                                        ; preds = %then_153, %then_151
  br label %merge_152

then_129:                                         ; preds = %while_body_127
  %"%vendor_id.load6" = load i64, ptr %"%vendor_id", align 8
  %cmpnetmp7 = icmp ne i64 %"%vendor_id.load6", 0
  br i1 %cmpnetmp7, label %then_131, label %merge_132

then_131:                                         ; preds = %then_129
  %"%vendor_device.load10" = load i64, ptr %"%vendor_device", align 8
  %shrtmp = lshr i64 %"%vendor_device.load10", 16
  %andtmp11 = and i64 %shrtmp, 65535
  store i64 %andtmp11, ptr %"%device_id", align 8
  %"%bus.load12" = load i64, ptr %"%bus", align 8
  %"%dev.load13" = load i64, ptr %"%dev", align 8
  %calltmp14 = call i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%bus.load12", i64 %"%dev.load13", i64 0, i64 8)
  store i64 %calltmp14, ptr %"%class_reg", align 8
  %"%class_reg.load" = load i64, ptr %"%class_reg", align 8
  %shrtmp15 = lshr i64 %"%class_reg.load", 24
  %andtmp16 = and i64 %shrtmp15, 255
  store i64 %andtmp16, ptr %"%class_code", align 8
  %"%class_reg.load17" = load i64, ptr %"%class_reg", align 8
  %shrtmp18 = lshr i64 %"%class_reg.load17", 16
  %andtmp19 = and i64 %shrtmp18, 255
  store i64 %andtmp19, ptr %"%subclass", align 8
  %"%bus.load20" = load i64, ptr %"%bus", align 8
  %"%dev.load21" = load i64, ptr %"%dev", align 8
  %calltmp22 = call i64 @"pci_\D8\A7\D8\A8\D8\AD\D8\AB_pm_cap"(i64 %"%bus.load20", i64 %"%dev.load21", i64 0)
  store i64 %calltmp22, ptr %"%pm", align 8
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  %"%bus.load23" = load i64, ptr %"%bus", align 8
  %calltmp24 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%bus.load23")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%dev.load25" = load i64, ptr %"%dev", align 8
  %calltmp26 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%dev.load25")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %"%vendor_id.load27" = load i64, ptr %"%vendor_id", align 8
  %calltmp28 = call i64 @dp_serial_hex(i64 %"%vendor_id.load27")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%device_id.load" = load i64, ptr %"%device_id", align 8
  %calltmp29 = call i64 @dp_serial_hex(i64 %"%device_id.load")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %"%class_code.load" = load i64, ptr %"%class_code", align 8
  %calltmp30 = call i64 @dp_serial_hex(i64 %"%class_code.load")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  %"%subclass.load" = load i64, ptr %"%subclass", align 8
  %calltmp31 = call i64 @dp_serial_hex(i64 %"%subclass.load")
  %"%pm.load" = load i64, ptr %"%pm", align 8
  %cmpgttmp = icmp sgt i64 %"%pm.load", 0
  br i1 %cmpgttmp, label %then_133, label %merge_134

then_133:                                         ; preds = %then_131
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%pm.load32" = load i64, ptr %"%pm", align 8
  %calltmp33 = call i64 @dp_serial_hex(i64 %"%pm.load32")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  br label %merge_134

then_135:                                         ; preds = %merge_134
  %"%dev.load37" = load i64, ptr %"%dev", align 8
  %cmpeqtmp38 = icmp eq i64 %"%dev.load37", 0
  br i1 %cmpeqtmp38, label %then_137, label %merge_138

then_137:                                         ; preds = %then_135
  br label %merge_138

then_139:                                         ; preds = %merge_136
  %"%dev.load41" = load i64, ptr %"%dev", align 8
  %cmpeqtmp42 = icmp eq i64 %"%dev.load41", 0
  br i1 %cmpeqtmp42, label %then_141, label %merge_142

then_141:                                         ; preds = %then_139
  br label %merge_142

then_143:                                         ; preds = %merge_140
  %"%dev.load45" = load i64, ptr %"%dev", align 8
  %cmpeqtmp46 = icmp eq i64 %"%dev.load45", 0
  br i1 %cmpeqtmp46, label %then_145, label %merge_146

then_145:                                         ; preds = %then_143
  br label %merge_146

then_147:                                         ; preds = %merge_144
  %"%dev.load49" = load i64, ptr %"%dev", align 8
  %cmpeqtmp50 = icmp eq i64 %"%dev.load49", 0
  br i1 %cmpeqtmp50, label %then_149, label %merge_150

then_149:                                         ; preds = %then_147
  br label %merge_150

then_151:                                         ; preds = %merge_148
  %"%dev.load53" = load i64, ptr %"%dev", align 8
  %cmpeqtmp54 = icmp eq i64 %"%dev.load53", 0
  br i1 %cmpeqtmp54, label %then_153, label %merge_154

then_153:                                         ; preds = %then_151
  br label %merge_154

while_body_124:                                   ; preds = %while_cond_123
  store i64 0, ptr %"%dev", align 8
  br label %while_cond_126

while_body_127:                                   ; preds = %while_cond_126
  %"%bus.load2" = load i64, ptr %"%bus", align 8
  %"%dev.load3" = load i64, ptr %"%dev", align 8
  %calltmp4 = call i64 @"pci_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%bus.load2", i64 %"%dev.load3", i64 0, i64 0)
  store i64 %calltmp4, ptr %"%vendor_device", align 8
  %"%vendor_device.load" = load i64, ptr %"%vendor_device", align 8
  %andtmp = and i64 %"%vendor_device.load", 65535
  store i64 %andtmp, ptr %"%vendor_id", align 8
  %"%vendor_id.load" = load i64, ptr %"%vendor_id", align 8
  %cmpnetmp = icmp ne i64 %"%vendor_id.load", 65535
  br i1 %cmpnetmp, label %then_129, label %merge_130

while_cond_123:                                   ; preds = %while_exit_128, %entry
  %"%bus.load" = load i64, ptr %"%bus", align 8
  %cmplttmp = icmp slt i64 %"%bus.load", 0
  br i1 %cmplttmp, label %while_body_124, label %while_exit_125

while_cond_126:                                   ; preds = %merge_130, %while_body_124
  %"%dev.load" = load i64, ptr %"%dev", align 8
  %cmplttmp1 = icmp slt i64 %"%dev.load", 0
  br i1 %cmplttmp1, label %while_body_127, label %while_exit_128

while_exit_125:                                   ; preds = %while_cond_123
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%\D9\88\D8\AC\D8\AF.load" = load i64, ptr %"%\D9\88\D8\AC\D8\AF", align 8
  %calltmp = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\88\D8\AC\D8\AF.load")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %loadtmp = load i64, ptr %"%\D9\88\D8\AC\D8\AF", align 8
  ret i64 %loadtmp

while_exit_128:                                   ; preds = %while_cond_126
  %"%bus.load5" = load i64, ptr %"%bus", align 8
  %addtmp = add i64 %"%bus.load5", 1
  store i64 %addtmp, ptr %"%bus", align 8
  br label %while_cond_123
}

define i64 @"dpms_\D8\B9\D9\8A\D9\86"(i64 %"\D9\88\D8\B6\D8\B9") {
entry:
  %"%seq_s" = alloca i64, align 8
  %"%seq" = alloca i64, align 8
  %"%val" = alloca i64, align 8
  %"\D9\88\D8\B6\D8\B91" = alloca i64, align 8
  store i64 %"\D9\88\D8\B6\D8\B9", ptr %"\D9\88\D8\B6\D8\B91", align 8
  %"%\D9\88\D8\B6\D8\B9.load" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load", 0
  br i1 %cmpeqtmp, label %then_155, label %merge_156

merge_156:                                        ; preds = %entry
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  %"%\D9\88\D8\B6\D8\B9.load2" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load2", 0
  br i1 %cmpeqtmp3, label %then_157, label %merge_158

merge_158:                                        ; preds = %then_157, %merge_156
  %"%\D9\88\D8\B6\D8\B9.load4" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load4", 0
  br i1 %cmpeqtmp5, label %then_159, label %merge_160

merge_160:                                        ; preds = %then_159, %merge_158
  %"%\D9\88\D8\B6\D8\B9.load7" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load7", 0
  br i1 %cmpeqtmp8, label %then_161, label %merge_162

merge_162:                                        ; preds = %then_161, %merge_160
  %"%\D9\88\D8\B6\D8\B9.load10" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load10", 0
  br i1 %cmpeqtmp11, label %then_163, label %merge_164

merge_164:                                        ; preds = %then_163, %merge_162
  %"%\D9\88\D8\B6\D8\B9.load12" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load12", 0
  br i1 %cmpeqtmp13, label %then_165, label %merge_166

merge_166:                                        ; preds = %then_165, %merge_164
  %"%\D9\88\D8\B6\D8\B9.load14" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load14", 0
  br i1 %cmpeqtmp15, label %then_167, label %merge_168

merge_168:                                        ; preds = %then_167, %merge_166
  %"%\D9\88\D8\B6\D8\B9.load16" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load16", 0
  br i1 %cmpeqtmp17, label %then_169, label %merge_170

merge_170:                                        ; preds = %then_169, %merge_168
  %"%\D9\88\D8\B6\D8\B9.load18" = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  %cmpeqtmp19 = icmp eq i64 %"%\D9\88\D8\B6\D8\B9.load18", 0
  br i1 %cmpeqtmp19, label %then_171, label %merge_172

merge_172:                                        ; preds = %then_171, %merge_170
  ret i64 1

then_155:                                         ; preds = %entry
  ret i64 0

then_157:                                         ; preds = %merge_156
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  store i64 0, ptr %"%val", align 8
  %"%val.load" = load i64, ptr %"%val", align 8
  %addtmp = add i64 %"%val.load", 1
  store i64 %addtmp, ptr %"%val", align 8
  br label %merge_158

then_159:                                         ; preds = %merge_158
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  store i64 0, ptr %"%seq", align 8
  %"%seq.load" = load i64, ptr %"%seq", align 8
  %addtmp6 = add i64 %"%seq.load", 32
  store i64 %addtmp6, ptr %"%seq", align 8
  br label %merge_160

then_161:                                         ; preds = %merge_160
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  store i64 0, ptr %"%seq_s", align 8
  %"%seq_s.load" = load i64, ptr %"%seq_s", align 8
  %addtmp9 = add i64 %"%seq_s.load", 32
  store i64 %addtmp9, ptr %"%seq_s", align 8
  br label %merge_162

then_163:                                         ; preds = %merge_162
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  br label %merge_164

then_165:                                         ; preds = %merge_164
  br label %merge_166

then_167:                                         ; preds = %merge_166
  br label %merge_168

then_169:                                         ; preds = %merge_168
  br label %merge_170

then_171:                                         ; preds = %merge_170
  br label %merge_172
}

define i64 @"dpms_\D8\B3\D8\B7\D9\88\D8\B9"(i64 %"\D9\86\D8\B3\D8\A8\D8\A9") {
entry:
  %"\D9\86\D8\B3\D8\A8\D8\A91" = alloca i64, align 8
  store i64 %"\D9\86\D8\B3\D8\A8\D8\A9", ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load", 0
  br i1 %cmplttmp, label %then_173, label %merge_174

merge_174:                                        ; preds = %then_173, %entry
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load2" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load2", 100
  br i1 %cmpgttmp, label %then_175, label %merge_176

merge_176:                                        ; preds = %then_175, %merge_174
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load3" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %calltmp = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load3")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load4" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load4", 0
  br i1 %cmpeqtmp, label %then_177, label %merge_178

merge_178:                                        ; preds = %then_177, %merge_176
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load5" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load5", 0
  br i1 %cmpnetmp, label %then_179, label %merge_180

merge_180:                                        ; preds = %merge_182, %merge_178
  ret i64 0

merge_182:                                        ; preds = %then_181, %then_179
  br label %merge_180

then_173:                                         ; preds = %entry
  br label %merge_174

then_175:                                         ; preds = %merge_174
  br label %merge_176

then_177:                                         ; preds = %merge_176
  br label %merge_178

then_179:                                         ; preds = %merge_178
  br i1 false, label %then_181, label %merge_182

then_181:                                         ; preds = %then_179
  br label %merge_182
}

define i64 @"dpms_\D9\81\D8\AD\D8\B5_\D9\85\D9\87\D9\84\D8\A9"() {
entry:
  %"%\D9\85\D8\B6\D9\89" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86.load" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load", 0
  store i64 %subtmp, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %"%\D9\85\D8\B6\D9\89.load" = load i64, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\B6\D9\89.load", 0
  br i1 %cmpgttmp, label %then_183, label %merge_184

merge_184:                                        ; preds = %merge_186, %entry
  %"%\D9\85\D8\B6\D9\89.load1" = load i64, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %cmpletmp = icmp sle i64 %"%\D9\85\D8\B6\D9\89.load1", 0
  br i1 %cmpletmp, label %then_187, label %merge_188

merge_186:                                        ; preds = %then_185, %then_183
  br label %merge_184

merge_188:                                        ; preds = %merge_196, %merge_184
  ret i64 0

merge_190:                                        ; preds = %merge_192, %then_187
  %"%\D9\85\D8\B6\D9\89.load4" = load i64, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %cmpletmp5 = icmp sle i64 %"%\D9\85\D8\B6\D9\89.load4", 0
  br i1 %cmpletmp5, label %then_195, label %merge_196

merge_192:                                        ; preds = %merge_194, %then_189
  br label %merge_190

merge_194:                                        ; preds = %then_193, %then_191
  br label %merge_192

merge_196:                                        ; preds = %merge_198, %merge_190
  br label %merge_188

merge_198:                                        ; preds = %merge_200, %then_195
  br label %merge_196

merge_200:                                        ; preds = %then_199, %then_197
  br label %merge_198

then_183:                                         ; preds = %entry
  br i1 false, label %then_185, label %merge_186

then_185:                                         ; preds = %then_183
  br label %merge_186

then_187:                                         ; preds = %merge_184
  %"%\D9\85\D8\B6\D9\89.load2" = load i64, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %cmpgttmp3 = icmp sgt i64 %"%\D9\85\D8\B6\D9\89.load2", 0
  br i1 %cmpgttmp3, label %then_189, label %merge_190

then_189:                                         ; preds = %then_187
  br i1 false, label %then_191, label %merge_192

then_191:                                         ; preds = %then_189
  br i1 false, label %then_193, label %merge_194

then_193:                                         ; preds = %then_191
  br label %merge_194

then_195:                                         ; preds = %merge_190
  %"%\D9\85\D8\B6\D9\89.load6" = load i64, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %cmpgttmp7 = icmp sgt i64 %"%\D9\85\D8\B6\D9\89.load6", 0
  br i1 %cmpgttmp7, label %then_197, label %merge_198

then_197:                                         ; preds = %then_195
  br i1 true, label %then_199, label %merge_200

then_199:                                         ; preds = %then_197
  br label %merge_200
}

define i64 @"dpms_\D9\86\D8\B4\D8\A7\D8\B7"() {
entry:
  br i1 false, label %then_201, label %merge_202

merge_202:                                        ; preds = %then_201, %entry
  ret i64 0

then_201:                                         ; preds = %entry
  br label %merge_202
}

define i64 @"dp_\D8\B9\D9\8A\D9\86_\D8\AD\D8\A7\D9\84"(i64 %"\D8\AC\D9\87\D8\A7\D8\B2_id", i64 %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF") {
entry:
  %"%pm_c" = alloca i64, align 8
  %"%pci_f" = alloca i64, align 8
  %"%pci_d" = alloca i64, align 8
  %"%pci_b" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AC" = alloca i64, align 8
  %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2" = alloca i64, align 8
  store i64 %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF", ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %"\D8\AC\D9\87\D8\A7\D8\B2_id1" = alloca i64, align 8
  store i64 %"\D8\AC\D9\87\D8\A7\D8\B2_id", ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load", 0
  br i1 %cmplttmp, label %then_203, label %merge_204

merge_204:                                        ; preds = %entry
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load3" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load3", 0
  br i1 %cmpgetmp, label %then_205, label %merge_206

merge_206:                                        ; preds = %merge_204
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pci_b", align 8
  store i64 0, ptr %"%pci_d", align 8
  store i64 0, ptr %"%pci_f", align 8
  store i64 0, ptr %"%pm_c", align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load4" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load4", 0
  br i1 %cmpeqtmp, label %then_207, label %merge_208

merge_208:                                        ; preds = %then_207, %merge_206
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load5" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load5", 1
  br i1 %cmpeqtmp6, label %then_209, label %merge_210

merge_210:                                        ; preds = %then_209, %merge_208
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load7" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load7", 2
  br i1 %cmpeqtmp8, label %then_211, label %merge_212

merge_212:                                        ; preds = %then_211, %merge_210
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load9" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load9", 3
  br i1 %cmpeqtmp10, label %then_213, label %merge_214

merge_214:                                        ; preds = %then_213, %merge_212
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load11" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load11", 4
  br i1 %cmpeqtmp12, label %then_215, label %merge_216

merge_216:                                        ; preds = %then_215, %merge_214
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load13" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load13", 5
  br i1 %cmpeqtmp14, label %then_217, label %merge_218

merge_218:                                        ; preds = %then_217, %merge_216
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load15" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load15", 6
  br i1 %cmpeqtmp16, label %then_219, label %merge_220

merge_220:                                        ; preds = %then_219, %merge_218
  %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A.load", %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load"
  br i1 %cmpeqtmp17, label %then_221, label %merge_222

merge_222:                                        ; preds = %merge_220
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load18" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %calltmp = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load18")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  %calltmp19 = call i64 @"dp_\D8\A7\D8\B7\D8\A8\D8\B9_\D9\86\D9\88\D8\B9"(i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A.load20" = load i64, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %calltmp21 = call i64 @"dp_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\A7\D9\84"(i64 %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A.load20")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load22" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %calltmp23 = call i64 @"dp_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\A7\D9\84"(i64 %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load22")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%pm_c.load" = load i64, ptr %"%pm_c", align 8
  %cmpgttmp = icmp sgt i64 %"%pm_c.load", 0
  br i1 %cmpgttmp, label %then_223, label %merge_224

merge_224:                                        ; preds = %then_223, %merge_222
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load27" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  %cmpeqtmp28 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load27", 0
  br i1 %cmpeqtmp28, label %then_225, label %merge_226

merge_226:                                        ; preds = %merge_234, %merge_224
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load31" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp32 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load31", 0
  br i1 %cmpeqtmp32, label %then_235, label %merge_236

merge_228:                                        ; preds = %then_227, %then_225
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load33" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %cmpeqtmp34 = icmp eq i64 %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load33", 0
  br i1 %cmpeqtmp34, label %then_229, label %merge_230

merge_230:                                        ; preds = %then_229, %merge_228
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load35" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %cmpeqtmp36 = icmp eq i64 %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load35", 0
  br i1 %cmpeqtmp36, label %then_231, label %merge_232

merge_232:                                        ; preds = %then_231, %merge_230
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load37" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %cmpgetmp38 = icmp sge i64 %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load37", 0
  br i1 %cmpgetmp38, label %then_233, label %merge_234

merge_234:                                        ; preds = %then_233, %merge_232
  br label %merge_226

merge_236:                                        ; preds = %then_235, %merge_226
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load39" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp40 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load39", 1
  br i1 %cmpeqtmp40, label %then_237, label %merge_238

merge_238:                                        ; preds = %then_237, %merge_236
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load41" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp42 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load41", 2
  br i1 %cmpeqtmp42, label %then_239, label %merge_240

merge_240:                                        ; preds = %then_239, %merge_238
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load43" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp44 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load43", 3
  br i1 %cmpeqtmp44, label %then_241, label %merge_242

merge_242:                                        ; preds = %then_241, %merge_240
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load45" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp46 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load45", 4
  br i1 %cmpeqtmp46, label %then_243, label %merge_244

merge_244:                                        ; preds = %then_243, %merge_242
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load47" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp48 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load47", 5
  br i1 %cmpeqtmp48, label %then_245, label %merge_246

merge_246:                                        ; preds = %then_245, %merge_244
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load49" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp50 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load49", 6
  br i1 %cmpeqtmp50, label %then_247, label %merge_248

merge_248:                                        ; preds = %then_247, %merge_246
  ret i64 1

then_203:                                         ; preds = %entry
  ret i64 0

then_205:                                         ; preds = %merge_204
  ret i64 0

then_207:                                         ; preds = %merge_206
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_208

then_209:                                         ; preds = %merge_208
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pci_b", align 8
  store i64 0, ptr %"%pci_d", align 8
  store i64 0, ptr %"%pci_f", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_210

then_211:                                         ; preds = %merge_210
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_212

then_213:                                         ; preds = %merge_212
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pci_b", align 8
  store i64 0, ptr %"%pci_d", align 8
  store i64 0, ptr %"%pci_f", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_214

then_215:                                         ; preds = %merge_214
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pci_b", align 8
  store i64 0, ptr %"%pci_d", align 8
  store i64 0, ptr %"%pci_f", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_216

then_217:                                         ; preds = %merge_216
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pci_b", align 8
  store i64 0, ptr %"%pci_d", align 8
  store i64 0, ptr %"%pci_f", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_218

then_219:                                         ; preds = %merge_218
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 0, ptr %"%pci_b", align 8
  store i64 0, ptr %"%pci_d", align 8
  store i64 0, ptr %"%pci_f", align 8
  store i64 0, ptr %"%pm_c", align 8
  br label %merge_220

then_221:                                         ; preds = %merge_220
  ret i64 0

then_223:                                         ; preds = %merge_222
  %"%pci_b.load" = load i64, ptr %"%pci_b", align 8
  %"%pci_d.load" = load i64, ptr %"%pci_d", align 8
  %"%pci_f.load" = load i64, ptr %"%pci_f", align 8
  %"%pm_c.load24" = load i64, ptr %"%pm_c", align 8
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load25" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %calltmp26 = call i64 @"pci_\D8\B9\D9\8A\D9\86_d_state"(i64 %"%pci_b.load", i64 %"%pci_d.load", i64 %"%pci_f.load", i64 %"%pm_c.load24", i64 %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load25")
  br label %merge_224

then_225:                                         ; preds = %merge_224
  %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load29" = load i64, ptr %"d_state_\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %cmpeqtmp30 = icmp eq i64 %"%d_state_\D8\AC\D8\AF\D9\8A\D8\AF.load29", 0
  br i1 %cmpeqtmp30, label %then_227, label %merge_228

then_227:                                         ; preds = %then_225
  br label %merge_228

then_229:                                         ; preds = %merge_228
  br label %merge_230

then_231:                                         ; preds = %merge_230
  br label %merge_232

then_233:                                         ; preds = %merge_232
  br label %merge_234

then_235:                                         ; preds = %merge_226
  br label %merge_236

then_237:                                         ; preds = %merge_236
  br label %merge_238

then_239:                                         ; preds = %merge_238
  br label %merge_240

then_241:                                         ; preds = %merge_240
  br label %merge_242

then_243:                                         ; preds = %merge_242
  br label %merge_244

then_245:                                         ; preds = %merge_244
  br label %merge_246

then_247:                                         ; preds = %merge_246
  br label %merge_248
}

define i64 @"dp_\D9\86\D8\B4\D8\A7\D8\B7_\D8\AC\D9\87\D8\A7\D8\B2"(i64 %"\D8\AC\D9\87\D8\A7\D8\B2_id") {
entry:
  %"%\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  %"\D8\AC\D9\87\D8\A7\D8\B2_id1" = alloca i64, align 8
  store i64 %"\D8\AC\D9\87\D8\A7\D8\B2_id", ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load", 0
  br i1 %cmpeqtmp, label %then_249, label %merge_250

merge_250:                                        ; preds = %merge_252, %entry
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load2" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load2", 1
  br i1 %cmpeqtmp3, label %then_253, label %merge_254

merge_252:                                        ; preds = %then_251, %then_249
  br label %merge_250

merge_254:                                        ; preds = %merge_256, %merge_250
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load4" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load4", 2
  br i1 %cmpeqtmp5, label %then_257, label %merge_258

merge_256:                                        ; preds = %then_255, %then_253
  br label %merge_254

merge_258:                                        ; preds = %merge_260, %merge_254
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load6" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load6", 3
  br i1 %cmpeqtmp7, label %then_261, label %merge_262

merge_260:                                        ; preds = %then_259, %then_257
  br label %merge_258

merge_262:                                        ; preds = %merge_264, %merge_258
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load8" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load8", 4
  br i1 %cmpeqtmp9, label %then_265, label %merge_266

merge_264:                                        ; preds = %then_263, %then_261
  br label %merge_262

merge_266:                                        ; preds = %merge_268, %merge_262
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load10" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load10", 5
  br i1 %cmpeqtmp11, label %then_269, label %merge_270

merge_268:                                        ; preds = %then_267, %then_265
  br label %merge_266

merge_270:                                        ; preds = %merge_272, %merge_266
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load12" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load12", 6
  br i1 %cmpeqtmp13, label %then_273, label %merge_274

merge_272:                                        ; preds = %then_271, %then_269
  br label %merge_270

merge_274:                                        ; preds = %merge_276, %merge_270
  ret i64 0

merge_276:                                        ; preds = %then_275, %then_273
  br label %merge_274

then_249:                                         ; preds = %entry
  br i1 false, label %then_251, label %merge_252

then_251:                                         ; preds = %then_249
  br label %merge_252

then_253:                                         ; preds = %merge_250
  br i1 false, label %then_255, label %merge_256

then_255:                                         ; preds = %then_253
  br label %merge_256

then_257:                                         ; preds = %merge_254
  br i1 false, label %then_259, label %merge_260

then_259:                                         ; preds = %then_257
  br label %merge_260

then_261:                                         ; preds = %merge_258
  br i1 false, label %then_263, label %merge_264

then_263:                                         ; preds = %then_261
  br label %merge_264

then_265:                                         ; preds = %merge_262
  br i1 false, label %then_267, label %merge_268

then_267:                                         ; preds = %then_265
  br label %merge_268

then_269:                                         ; preds = %merge_266
  br i1 false, label %then_271, label %merge_272

then_271:                                         ; preds = %then_269
  br label %merge_272

then_273:                                         ; preds = %merge_270
  br i1 false, label %then_275, label %merge_276

then_275:                                         ; preds = %then_273
  br label %merge_276
}

define i64 @"dp_\D8\AF\D9\88\D8\B1\D8\A9_rpm"() {
entry:
  %"%\D9\85\D8\B6\D9\89_6" = alloca i64, align 8
  %"%\D9\85\D8\B6\D9\89_5" = alloca i64, align 8
  %"%\D9\85\D8\B6\D9\89_3" = alloca i64, align 8
  %"%\D9\85\D8\B6\D9\89_1" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  br i1 false, label %then_277, label %merge_278

merge_278:                                        ; preds = %merge_280, %entry
  br i1 true, label %then_283, label %merge_284

merge_280:                                        ; preds = %merge_282, %then_277
  br label %merge_278

merge_282:                                        ; preds = %then_281, %then_279
  br label %merge_280

merge_284:                                        ; preds = %then_283, %merge_278
  br i1 false, label %then_285, label %merge_286

merge_286:                                        ; preds = %then_285, %merge_284
  br i1 true, label %then_287, label %merge_288

merge_288:                                        ; preds = %then_287, %merge_286
  br i1 false, label %then_289, label %merge_290

merge_290:                                        ; preds = %merge_292, %merge_288
  br i1 true, label %then_295, label %merge_296

merge_292:                                        ; preds = %merge_294, %then_289
  br label %merge_290

merge_294:                                        ; preds = %then_293, %then_291
  br label %merge_292

merge_296:                                        ; preds = %then_295, %merge_290
  br i1 false, label %then_297, label %merge_298

merge_298:                                        ; preds = %merge_300, %merge_296
  br i1 true, label %then_303, label %merge_304

merge_300:                                        ; preds = %merge_302, %then_297
  br label %merge_298

merge_302:                                        ; preds = %then_301, %then_299
  br label %merge_300

merge_304:                                        ; preds = %then_303, %merge_298
  br i1 false, label %then_305, label %merge_306

merge_306:                                        ; preds = %merge_308, %merge_304
  br i1 true, label %then_311, label %merge_312

merge_308:                                        ; preds = %merge_310, %then_305
  br label %merge_306

merge_310:                                        ; preds = %then_309, %then_307
  br label %merge_308

merge_312:                                        ; preds = %then_311, %merge_306
  ret i64 0

then_277:                                         ; preds = %entry
  br i1 true, label %then_279, label %merge_280

then_279:                                         ; preds = %then_277
  %"%\D8\A7\D9\84\D8\A2\D9\86.load" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load", 0
  store i64 %subtmp, ptr %"%\D9\85\D8\B6\D9\89_1", align 8
  %"%\D9\85\D8\B6\D9\89_1.load" = load i64, ptr %"%\D9\85\D8\B6\D9\89_1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\B6\D9\89_1.load", 0
  br i1 %cmpgttmp, label %then_281, label %merge_282

then_281:                                         ; preds = %then_279
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  br label %merge_282

then_283:                                         ; preds = %merge_278
  br label %merge_284

then_285:                                         ; preds = %merge_284
  %calltmp = call i64 @"dpms_\D9\81\D8\AD\D8\B5_\D9\85\D9\87\D9\84\D8\A9"()
  br label %merge_286

then_287:                                         ; preds = %merge_286
  br label %merge_288

then_289:                                         ; preds = %merge_288
  br i1 true, label %then_291, label %merge_292

then_291:                                         ; preds = %then_289
  %"%\D8\A7\D9\84\D8\A2\D9\86.load1" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp2 = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load1", 0
  store i64 %subtmp2, ptr %"%\D9\85\D8\B6\D9\89_3", align 8
  %"%\D9\85\D8\B6\D9\89_3.load" = load i64, ptr %"%\D9\85\D8\B6\D9\89_3", align 8
  %cmpgttmp3 = icmp sgt i64 %"%\D9\85\D8\B6\D9\89_3.load", 0
  br i1 %cmpgttmp3, label %then_293, label %merge_294

then_293:                                         ; preds = %then_291
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  br label %merge_294

then_295:                                         ; preds = %merge_290
  br label %merge_296

then_297:                                         ; preds = %merge_296
  br i1 true, label %then_299, label %merge_300

then_299:                                         ; preds = %then_297
  %"%\D8\A7\D9\84\D8\A2\D9\86.load4" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp5 = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load4", 0
  store i64 %subtmp5, ptr %"%\D9\85\D8\B6\D9\89_5", align 8
  %"%\D9\85\D8\B6\D9\89_5.load" = load i64, ptr %"%\D9\85\D8\B6\D9\89_5", align 8
  %cmpgttmp6 = icmp sgt i64 %"%\D9\85\D8\B6\D9\89_5.load", 0
  br i1 %cmpgttmp6, label %then_301, label %merge_302

then_301:                                         ; preds = %then_299
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.45 to i64))
  br label %merge_302

then_303:                                         ; preds = %merge_298
  br label %merge_304

then_305:                                         ; preds = %merge_304
  br i1 true, label %then_307, label %merge_308

then_307:                                         ; preds = %then_305
  %"%\D8\A7\D9\84\D8\A2\D9\86.load7" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp8 = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load7", 0
  store i64 %subtmp8, ptr %"%\D9\85\D8\B6\D9\89_6", align 8
  %"%\D9\85\D8\B6\D9\89_6.load" = load i64, ptr %"%\D9\85\D8\B6\D9\89_6", align 8
  %cmpgttmp9 = icmp sgt i64 %"%\D9\85\D8\B6\D9\89_6.load", 0
  br i1 %cmpgttmp9, label %then_309, label %merge_310

then_309:                                         ; preds = %then_307
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  br label %merge_310

then_311:                                         ; preds = %merge_306
  br label %merge_312
}

define i64 @"dp_\D9\82\D8\AF\D8\B1_\D8\B7\D8\A7\D9\82\D8\A9_\D8\AC\D9\87\D8\A7\D8\B2"(i64 %"\D9\86\D9\88\D8\B9_\D8\AC", i64 %d_state) {
entry:
  %"%\D8\B7\D8\A7\D9\82\D8\A9_d0" = alloca i64, align 8
  %d_state2 = alloca i64, align 8
  store i64 %d_state, ptr %d_state2, align 8
  %"\D9\86\D9\88\D8\B9_\D8\AC1" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\AC", ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  store i64 0, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load", 0
  br i1 %cmpeqtmp, label %then_313, label %merge_314

merge_314:                                        ; preds = %then_313, %entry
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load3" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load3", 0
  br i1 %cmpeqtmp4, label %then_315, label %merge_316

merge_316:                                        ; preds = %then_315, %merge_314
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load5" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load5", 0
  br i1 %cmpeqtmp6, label %then_317, label %merge_318

merge_318:                                        ; preds = %then_317, %merge_316
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load7" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load7", 0
  br i1 %cmpeqtmp8, label %then_319, label %merge_320

merge_320:                                        ; preds = %then_319, %merge_318
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load9" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load9", 0
  br i1 %cmpeqtmp10, label %then_321, label %merge_322

merge_322:                                        ; preds = %then_321, %merge_320
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load11" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load11", 0
  br i1 %cmpeqtmp12, label %then_323, label %merge_324

merge_324:                                        ; preds = %then_323, %merge_322
  %"%\D9\86\D9\88\D8\B9_\D8\AC.load13" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\AC1", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\AC.load13", 0
  br i1 %cmpeqtmp14, label %then_325, label %merge_326

merge_326:                                        ; preds = %then_325, %merge_324
  %"%d_state.load" = load i64, ptr %d_state2, align 8
  %cmpeqtmp15 = icmp eq i64 %"%d_state.load", 0
  br i1 %cmpeqtmp15, label %then_327, label %merge_328

merge_328:                                        ; preds = %merge_326
  %"%d_state.load16" = load i64, ptr %d_state2, align 8
  %cmpeqtmp17 = icmp eq i64 %"%d_state.load16", 0
  br i1 %cmpeqtmp17, label %then_329, label %merge_330

merge_330:                                        ; preds = %merge_328
  %"%d_state.load18" = load i64, ptr %d_state2, align 8
  %cmpeqtmp19 = icmp eq i64 %"%d_state.load18", 0
  br i1 %cmpeqtmp19, label %then_331, label %merge_332

merge_332:                                        ; preds = %merge_330
  %"%d_state.load21" = load i64, ptr %d_state2, align 8
  %cmpeqtmp22 = icmp eq i64 %"%d_state.load21", 0
  br i1 %cmpeqtmp22, label %then_333, label %merge_334

merge_334:                                        ; preds = %merge_332
  %"%d_state.load25" = load i64, ptr %d_state2, align 8
  %cmpeqtmp26 = icmp eq i64 %"%d_state.load25", 0
  br i1 %cmpeqtmp26, label %then_335, label %merge_336

merge_336:                                        ; preds = %merge_334
  %loadtmp27 = load i64, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  ret i64 %loadtmp27

then_313:                                         ; preds = %entry
  store i64 65000, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_314

then_315:                                         ; preds = %merge_314
  store i64 75000, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_316

then_317:                                         ; preds = %merge_316
  store i64 15000, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_318

then_319:                                         ; preds = %merge_318
  store i64 5000, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_320

then_321:                                         ; preds = %merge_320
  store i64 3000, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_322

then_323:                                         ; preds = %merge_322
  store i64 2500, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_324

then_325:                                         ; preds = %merge_324
  store i64 1000, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  br label %merge_326

then_327:                                         ; preds = %merge_326
  %loadtmp = load i64, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  ret i64 %loadtmp

then_329:                                         ; preds = %merge_328
  %"%\D8\B7\D8\A7\D9\82\D8\A9_d0.load" = load i64, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  %shrtmp = lshr i64 %"%\D8\B7\D8\A7\D9\82\D8\A9_d0.load", 1
  ret i64 %shrtmp

then_331:                                         ; preds = %merge_330
  %"%\D8\B7\D8\A7\D9\82\D8\A9_d0.load20" = load i64, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  %divtmp = sdiv i64 %"%\D8\B7\D8\A7\D9\82\D8\A9_d0.load20", 10
  ret i64 %divtmp

then_333:                                         ; preds = %merge_332
  %"%\D8\B7\D8\A7\D9\82\D8\A9_d0.load23" = load i64, ptr %"%\D8\B7\D8\A7\D9\82\D8\A9_d0", align 8
  %divtmp24 = sdiv i64 %"%\D8\B7\D8\A7\D9\82\D8\A9_d0.load23", 100
  ret i64 %divtmp24

then_335:                                         ; preds = %merge_334
  ret i64 0
}

define i64 @"dp_\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9"() {
entry:
  ret i64 0
}

define i64 @"dp_\D8\B9\D9\8A\D9\86_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8"(i64 %"\D8\AC\D9\87\D8\A7\D8\B2_id", i64 %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85") {
entry:
  %"\D8\A3\D8\B9\D9\84\D8\A7\D9\852" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85", ptr %"\D8\A3\D8\B9\D9\84\D8\A7\D9\852", align 8
  %"\D8\AC\D9\87\D8\A7\D8\B2_id1" = alloca i64, align 8
  store i64 %"\D8\AC\D9\87\D8\A7\D8\B2_id", ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load", 0
  br i1 %cmpeqtmp, label %then_337, label %merge_338

merge_338:                                        ; preds = %then_337, %entry
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load3" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load3", 1
  br i1 %cmpeqtmp4, label %then_339, label %merge_340

merge_340:                                        ; preds = %then_339, %merge_338
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load5" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load5", 2
  br i1 %cmpeqtmp6, label %then_341, label %merge_342

merge_342:                                        ; preds = %then_341, %merge_340
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load7" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load7", 3
  br i1 %cmpeqtmp8, label %then_343, label %merge_344

merge_344:                                        ; preds = %then_343, %merge_342
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load9" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load9", 4
  br i1 %cmpeqtmp10, label %then_345, label %merge_346

merge_346:                                        ; preds = %then_345, %merge_344
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load11" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load11", 5
  br i1 %cmpeqtmp12, label %then_347, label %merge_348

merge_348:                                        ; preds = %then_347, %merge_346
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load13" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load13", 6
  br i1 %cmpeqtmp14, label %then_349, label %merge_350

merge_350:                                        ; preds = %then_349, %merge_348
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load15" = load i64, ptr %"\D8\AC\D9\87\D8\A7\D8\B2_id1", align 8
  %calltmp = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\AC\D9\87\D8\A7\D8\B2_id.load15")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85.load" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D8\A7\D9\852", align 8
  %calltmp16 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85.load")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  ret i64 0

then_337:                                         ; preds = %entry
  br label %merge_338

then_339:                                         ; preds = %merge_338
  br label %merge_340

then_341:                                         ; preds = %merge_340
  br label %merge_342

then_343:                                         ; preds = %merge_342
  br label %merge_344

then_345:                                         ; preds = %merge_344
  br label %merge_346

then_347:                                         ; preds = %merge_346
  br label %merge_348

then_349:                                         ; preds = %merge_348
  br label %merge_350
}

define i64 @"dp_\D8\A3\D9\8A\D9\82\D8\B8_\D8\A7\D9\84\D9\83\D9\84"() {
entry:
  %"%\D8\B7" = alloca i64, align 8
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.50 to i64))
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_351

while_body_352:                                   ; preds = %while_cond_351
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load1", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_351

while_cond_351:                                   ; preds = %while_body_352, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_352, label %while_exit_353

while_exit_353:                                   ; preds = %while_cond_351
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.51 to i64))
  ret i64 0
}

define i64 @"dp_\D8\A3\D9\86\D9\85_\D8\A7\D9\84\D9\83\D9\84"() {
entry:
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  ret i64 0
}

define i64 @"dp_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %calltmp = call i64 @"dp_\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9"()
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.55 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.59 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.63 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.66 to i64))
  br i1 true, label %then_354, label %merge_355

merge_355:                                        ; preds = %then_354, %entry
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.70 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.74 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.66 to i64))
  br i1 false, label %then_356, label %merge_357

merge_357:                                        ; preds = %then_356, %merge_355
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.80 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.84 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.88 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.89 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.90 to i64))
  %calltmp1 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 0)
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.91 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.92 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.95 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.96 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.98 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.100 to i64))
  br i1 true, label %then_358, label %merge_359

merge_359:                                        ; preds = %then_358, %merge_357
  br i1 false, label %then_360, label %merge_361

merge_361:                                        ; preds = %then_360, %merge_359
  br i1 false, label %then_362, label %merge_363

merge_363:                                        ; preds = %then_362, %merge_361
  br i1 false, label %then_364, label %merge_365

merge_365:                                        ; preds = %then_364, %merge_363
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.106 to i64))
  ret i64 0

then_354:                                         ; preds = %entry
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.67 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.68 to i64))
  br label %merge_355

then_356:                                         ; preds = %merge_355
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.78 to i64))
  br label %merge_357

then_358:                                         ; preds = %merge_357
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.101 to i64))
  br label %merge_359

then_360:                                         ; preds = %merge_359
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.102 to i64))
  br label %merge_361

then_362:                                         ; preds = %merge_361
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.103 to i64))
  br label %merge_363

then_364:                                         ; preds = %merge_363
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.104 to i64))
  br label %merge_365
}

define i64 @"dp_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.108 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.109 to i64))
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.108 to i64))
  %calltmp = call i64 @"pci_\D9\81\D8\AD\D8\B5_\D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9"()
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.111 to i64))
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_366

merge_370:                                        ; preds = %then_369, %while_body_367
  %"%\D8\B7.load4" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\B7.load4", 1
  br i1 %cmpeqtmp5, label %then_371, label %merge_372

merge_372:                                        ; preds = %then_371, %merge_370
  %"%\D8\B7.load6" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\B7.load6", 2
  br i1 %cmpeqtmp7, label %then_373, label %merge_374

merge_374:                                        ; preds = %then_373, %merge_372
  %"%\D8\B7.load8" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D8\B7.load8", 3
  br i1 %cmpeqtmp9, label %then_375, label %merge_376

merge_376:                                        ; preds = %then_375, %merge_374
  %"%\D8\B7.load10" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\B7.load10", 4
  br i1 %cmpeqtmp11, label %then_377, label %merge_378

merge_378:                                        ; preds = %then_377, %merge_376
  %"%\D8\B7.load12" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D8\B7.load12", 5
  br i1 %cmpeqtmp13, label %then_379, label %merge_380

merge_380:                                        ; preds = %then_379, %merge_378
  %"%\D8\B7.load14" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D8\B7.load14", 6
  br i1 %cmpeqtmp15, label %then_381, label %merge_382

merge_382:                                        ; preds = %then_381, %merge_380
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%\D8\B7.load16" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load16", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_366

then_369:                                         ; preds = %while_body_367
  br label %merge_370

then_371:                                         ; preds = %merge_370
  br label %merge_372

then_373:                                         ; preds = %merge_372
  br label %merge_374

then_375:                                         ; preds = %merge_374
  br label %merge_376

then_377:                                         ; preds = %merge_376
  br label %merge_378

then_379:                                         ; preds = %merge_378
  br label %merge_380

then_381:                                         ; preds = %merge_380
  br label %merge_382

while_body_367:                                   ; preds = %while_cond_366
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.112 to i64))
  %"%\D8\B7.load1" = load i64, ptr %"%\D8\B7", align 8
  %calltmp2 = call i64 @"dp_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B7.load1")
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.57 to i64))
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B7.load3", 0
  br i1 %cmpeqtmp, label %then_369, label %merge_370

while_cond_366:                                   ; preds = %merge_382, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 0
  br i1 %cmplttmp, label %while_body_367, label %while_exit_368

while_exit_368:                                   ; preds = %while_cond_366
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  call void @"dp_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.114 to i64))
  ret i64 1
}

define i64 @"dp_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %calltmp = call i64 @"dp_\D8\AF\D9\88\D8\B1\D8\A9_rpm"()
  %calltmp1 = call i64 @"dp_\D8\A7\D8\AD\D8\B3\D8\A8_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B7\D8\A7\D9\82\D8\A9"()
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_pci_\D8\AA\D9\85_\D9\81\D8\AD\D8\B5\D9\87\D8\A7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D9\86\D8\A7\D8\A6\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA_d" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B7\D8\A7\D9\82\D8\A9_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_mw" = alloca i64, align 8
  %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\B3\D8\B7\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%dpms_\D9\85\D9\87\D9\84\D8\A9_\D8\AB\D8\A7\D9\86\D9\8A\D8\A9" = alloca i64, align 8
  %"%dpms_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_rpm_\D8\AE\D9\85\D9\88\D9\84_ms" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_rpm_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pm_cap" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pci_func" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pci_dev" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pci_bus" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_usb" = alloca i64, align 8
  %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D8\B4\D8\A8\D9\83\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D9\81\D8\A3\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D9\84\D9\88\D8\AD\D8\A9_\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD" = alloca i64, align 8
  %"%rpm_\D9\85\D9\87\D9\84\D8\A9_\D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81" = alloca i64, align 8
  %"%rpm_\D8\AE\D9\85\D9\88\D9\84_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_pci_func" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_pci_dev" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_pci_bus" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9" = alloca i64, align 8
  %"%USB_SUSPEND_BIT" = alloca i64, align 8
  %"%USB_UHCI_STS" = alloca i64, align 8
  %"%USB_UHCI_CMD" = alloca i64, align 8
  %"%BGA_REG_ENABLE" = alloca i64, align 8
  %"%BGA_DATA" = alloca i64, align 8
  %"%BGA_INDEX" = alloca i64, align 8
  %"%VGA_SEQ_DATA" = alloca i64, align 8
  %"%VGA_SEQ_INDEX" = alloca i64, align 8
  %"%DPMS_OFF" = alloca i64, align 8
  %"%DPMS_SUSPEND" = alloca i64, align 8
  %"%DPMS_STANDBY" = alloca i64, align 8
  %"%DPMS_ON" = alloca i64, align 8
  %"%PCI_PM_CTRL_STATE_MASK" = alloca i64, align 8
  %"%PCI_PM_CTRL" = alloca i64, align 8
  %"%PCI_CONFIG_DATA" = alloca i64, align 8
  %"%PCI_CONFIG_ADDR" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A2\D8\AE\D8\B1" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D9\88\D8\AD\D8\AF\D8\A9_\D8\A5\D8\AF\D8\AE\D8\A7\D9\84" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\B5\D9\88\D8\AA" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_usb" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\B4\D8\A8\D9\83\D8\A9" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AA\D8\AE\D8\B2\D9\8A\D9\86" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\B4\D8\A7\D8\B4\D8\A9" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_gpu" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D9\85\D8\B9\D8\A7\D9\84\D8\AC" = alloca i64, align 8
  %"%D3_\D8\A8\D8\A7\D8\B1\D8\AF" = alloca i64, align 8
  %"%D3_\D8\B3\D8\A7\D8\AE\D9\86" = alloca i64, align 8
  %"%D2_\D8\AE\D9\85\D9\88\D9\84_\D8\B9\D9\85\D9\8A\D9\82" = alloca i64, align 8
  %"%D1_\D8\AE\D9\85\D9\88\D9\84" = alloca i64, align 8
  %"%D0_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%D0_\D9\86\D8\B4\D8\B7", align 8
  store i64 1, ptr %"%D1_\D8\AE\D9\85\D9\88\D9\84", align 8
  store i64 2, ptr %"%D2_\D8\AE\D9\85\D9\88\D9\84_\D8\B9\D9\85\D9\8A\D9\82", align 8
  store i64 3, ptr %"%D3_\D8\B3\D8\A7\D8\AE\D9\86", align 8
  store i64 4, ptr %"%D3_\D8\A8\D8\A7\D8\B1\D8\AF", align 8
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D9\85\D8\B9\D8\A7\D9\84\D8\AC", align 8
  store i64 1, ptr %"%\D9\86\D9\88\D8\B9_gpu", align 8
  store i64 2, ptr %"%\D9\86\D9\88\D8\B9_\D8\B4\D8\A7\D8\B4\D8\A9", align 8
  store i64 3, ptr %"%\D9\86\D9\88\D8\B9_\D8\AA\D8\AE\D8\B2\D9\8A\D9\86", align 8
  store i64 4, ptr %"%\D9\86\D9\88\D8\B9_\D8\B4\D8\A8\D9\83\D8\A9", align 8
  store i64 5, ptr %"%\D9\86\D9\88\D8\B9_usb", align 8
  store i64 6, ptr %"%\D9\86\D9\88\D8\B9_\D8\B5\D9\88\D8\AA", align 8
  store i64 7, ptr %"%\D9\86\D9\88\D8\B9_\D9\88\D8\AD\D8\AF\D8\A9_\D8\A5\D8\AF\D8\AE\D8\A7\D9\84", align 8
  store i64 8, ptr %"%\D9\86\D9\88\D8\B9_\D8\A2\D8\AE\D8\B1", align 8
  store i64 3320, ptr %"%PCI_CONFIG_ADDR", align 8
  store i64 3324, ptr %"%PCI_CONFIG_DATA", align 8
  store i64 4, ptr %"%PCI_PM_CTRL", align 8
  store i64 3, ptr %"%PCI_PM_CTRL_STATE_MASK", align 8
  store i64 0, ptr %"%DPMS_ON", align 8
  store i64 1, ptr %"%DPMS_STANDBY", align 8
  store i64 2, ptr %"%DPMS_SUSPEND", align 8
  store i64 3, ptr %"%DPMS_OFF", align 8
  store i64 964, ptr %"%VGA_SEQ_INDEX", align 8
  store i64 965, ptr %"%VGA_SEQ_DATA", align 8
  store i64 455, ptr %"%BGA_INDEX", align 8
  store i64 457, ptr %"%BGA_DATA", align 8
  store i64 4, ptr %"%BGA_REG_ENABLE", align 8
  store i64 0, ptr %"%USB_UHCI_CMD", align 8
  store i64 2, ptr %"%USB_UHCI_STS", align 8
  store i64 8, ptr %"%USB_SUSPEND_BIT", align 8
  store i64 32, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9", align 8
  store i64 8, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_pci_bus", align 8
  store i64 32, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_pci_dev", align 8
  store i64 8, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_pci_func", align 8
  store i64 5000, ptr %"%rpm_\D8\AE\D9\85\D9\88\D9\84_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", align 8
  store i64 100, ptr %"%rpm_\D9\85\D9\87\D9\84\D8\A9_\D8\A7\D8\B3\D8\AA\D8\A6\D9\86\D8\A7\D9\81", align 8
  store i64 1, ptr %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D9\84\D9\88\D8\AD\D8\A9_\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD", align 8
  store i64 2, ptr %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D9\81\D8\A3\D8\B1\D8\A9", align 8
  store i64 4, ptr %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D8\B4\D8\A8\D9\83\D8\A9", align 8
  store i64 8, ptr %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_usb", align 8
  store i64 16, ptr %"%\D8\A5\D9\8A\D9\82\D8\A7\D8\B8_\D9\85\D8\A4\D9\82\D8\AA", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D9\86\D9\88\D8\B9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pci_bus", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pci_dev", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_pm_cap", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 5000, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_0_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D9\86\D9\88\D8\B9", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pci_bus", align 8
  store i64 2, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pci_dev", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_pm_cap", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 10000, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_1_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 2, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D9\86\D9\88\D8\B9", align 8
  store i64 2, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pci_bus", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pci_dev", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_pm_cap", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 300000, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 3, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_2_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 3, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D9\86\D9\88\D8\B9", align 8
  store i64 3, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pci_bus", align 8
  store i64 31, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pci_dev", align 8
  store i64 2, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_pm_cap", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 30000, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_3_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 4, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D9\86\D9\88\D8\B9", align 8
  store i64 4, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pci_bus", align 8
  store i64 3, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pci_dev", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_pm_cap", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 4, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_4_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 5, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D9\86\D9\88\D8\B9", align 8
  store i64 5, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pci_bus", align 8
  store i64 29, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pci_dev", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_pm_cap", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 15000, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 8, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_5_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 6, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D9\86\D9\88\D8\B9", align 8
  store i64 6, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\A7\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\AD\D8\A7\D9\84_\D8\B7\D8\A7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\AD\D8\A7\D9\84_\D8\B3\D8\A7\D8\A8\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pci_bus", align 8
  store i64 27, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pci_dev", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pci_func", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_pm_cap", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_rpm_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 5000, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_rpm_\D8\AE\D9\85\D9\88\D9\84_ms", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8", align 8
  store i64 0, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 1, ptr %"%\D8\AC\D9\87\D8\A7\D8\B2_6_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 7, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D8\A7\D9\84\D9\85\D8\B3\D8\AC\D9\84\D8\A9", align 8
  store i64 0, ptr %"%dpms_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 300, ptr %"%dpms_\D9\85\D9\87\D9\84\D8\A9_\D8\AB\D8\A7\D9\86\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\A2\D8\AE\D8\B1_\D9\86\D8\B4\D8\A7\D8\B7", align 8
  store i64 100, ptr %"%\D8\B4\D8\A7\D8\B4\D8\A9_\D8\B3\D8\B7\D9\88\D8\B9", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B7\D8\A7\D9\82\D8\A9_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_mw", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84\D8\A7\D8\AA_d", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_\D9\86\D8\A7\D8\A6\D9\85\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D8\B8\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AC\D9\87\D8\B2\D8\A9_pci_\D8\AA\D9\85_\D9\81\D8\AD\D8\B5\D9\87\D8\A7", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
