; ModuleID = 'طاقة_حرارة_نحلة'
source_filename = "\D8\B7\D8\A7\D9\82\D8\A9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [3 x i8] c" C\00", align 1
@str.const.1 = private unnamed_addr constant [43 x i8] c"[TH] !! \D8\A5\D9\8A\D9\82\D8\A7\D9\81 \D8\B7\D8\A7\D8\B1\D8\A6 !! \D9\85\D9\86\D8\B7\D9\82\D8\A9 \00", align 1
@str.const.2 = private unnamed_addr constant [9 x i8] c" \D8\B9\D9\86\D8\AF \00", align 1
@str.const.3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [43 x i8] c"[TH] !! \D8\AD\D8\B1\D8\A7\D8\B1\D8\A9 \D8\AD\D8\B1\D8\AC\D8\A9 !! \D9\85\D9\86\D8\B7\D9\82\D8\A9 \00", align 1
@str.const.5 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.7 = private unnamed_addr constant [45 x i8] c"[TH] \D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82 \D8\AD\D8\B1\D8\A7\D8\B1\D9\8A \E2\80\94 CPU \D8\B9\D9\86\D8\AF \00", align 1
@str.const.9 = private unnamed_addr constant [45 x i8] c"[TH] \D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82 \D8\AD\D8\B1\D8\A7\D8\B1\D9\8A \E2\80\94 GPU \D8\B9\D9\86\D8\AF \00", align 1
@str.const.11 = private unnamed_addr constant [35 x i8] c"[TH] \D8\A5\D9\84\D8\BA\D8\A7\D8\A1 \D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82 CPU (\00", align 1
@str.const.12 = private unnamed_addr constant [3 x i8] c")\0A\00", align 1
@str.const.13 = private unnamed_addr constant [34 x i8] c"[TH] \D8\A5\D9\84\D8\BA\D8\A7\D8\A1 \D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82 GPU\0A\00", align 1
@str.const.14 = private unnamed_addr constant [33 x i8] c"[BAT] !! \D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9 \D8\AD\D8\B1\D8\AC\D8\A9: \00", align 1
@str.const.15 = private unnamed_addr constant [6 x i8] c"% !!\0A\00", align 1
@str.const.16 = private unnamed_addr constant [45 x i8] c"[BAT] \D8\AA\D8\AD\D8\B0\D9\8A\D8\B1: \D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9 \D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9 \00", align 1
@str.const.17 = private unnamed_addr constant [3 x i8] c"%\0A\00", align 1
@str.const.18 = private unnamed_addr constant [40 x i8] c"[BAT] \D8\B4\D8\AD\D9\86 \D8\B0\D9\83\D9\8A: \D8\A7\D9\83\D8\AA\D9\85\D9\84 \D8\B9\D9\86\D8\AF \00", align 1
@str.const.20 = private unnamed_addr constant [25 x i8] c"[BAT] \D8\A8\D8\AF\D8\A1 \D8\A7\D9\84\D8\B4\D8\AD\D9\86\0A\00", align 1
@str.const.21 = private unnamed_addr constant [48 x i8] c"[BAT] \D8\A7\D9\86\D8\AA\D9\82\D8\A7\D9\84 \D9\84\D8\B7\D8\A7\D9\82\D8\A9 \D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9\0A\00", align 1
@str.const.22 = private unnamed_addr constant [24 x i8] c"[BAT] \D8\AD\D8\AF\D9\88\D8\AF \D8\B4\D8\AD\D9\86: \00", align 1
@str.const.23 = private unnamed_addr constant [5 x i8] c"% - \00", align 1
@str.const.25 = private unnamed_addr constant [56 x i8] c"\0A[BAT] \E2\95\90\E2\95\90\E2\95\90 \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9 \E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.26 = private unnamed_addr constant [34 x i8] c"[BAT] \D9\84\D8\A7 \D8\AA\D9\88\D8\AC\D8\AF \D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9\0A\00", align 1
@str.const.27 = private unnamed_addr constant [21 x i8] c"[BAT] \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9: \00", align 1
@str.const.28 = private unnamed_addr constant [11 x i8] c"\D8\BA\D8\A7\D8\A6\D8\A8\D8\A9\00", align 1
@str.const.29 = private unnamed_addr constant [9 x i8] c"\D8\AA\D8\B4\D8\AD\D9\86\00", align 1
@str.const.30 = private unnamed_addr constant [9 x i8] c"\D8\AA\D9\81\D8\B1\D8\BA\00", align 1
@str.const.31 = private unnamed_addr constant [13 x i8] c"\D9\85\D9\85\D8\AA\D9\84\D8\A6\D8\A9\00", align 1
@str.const.32 = private unnamed_addr constant [10 x i8] c"\D8\AD\D8\B1\D8\AC\D8\A9!\00", align 1
@str.const.34 = private unnamed_addr constant [19 x i8] c"[BAT] \D8\A7\D9\84\D8\B4\D8\AD\D9\86: \00", align 1
@str.const.35 = private unnamed_addr constant [4 x i8] c"% (\00", align 1
@str.const.36 = private unnamed_addr constant [4 x i8] c" / \00", align 1
@str.const.37 = private unnamed_addr constant [7 x i8] c" mWh)\0A\00", align 1
@str.const.38 = private unnamed_addr constant [19 x i8] c"[BAT] \D8\A7\D9\84\D8\AC\D9\87\D8\AF: \00", align 1
@str.const.39 = private unnamed_addr constant [21 x i8] c" mV\D8\8C \D8\A7\D9\84\D8\AA\D9\8A\D8\A7\D8\B1: \00", align 1
@str.const.40 = private unnamed_addr constant [5 x i8] c" mA\0A\00", align 1
@str.const.41 = private unnamed_addr constant [23 x i8] c"[BAT] \D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9: \00", align 1
@str.const.43 = private unnamed_addr constant [19 x i8] c"[BAT] \D8\A7\D9\84\D8\B5\D8\AD\D8\A9: \00", align 1
@str.const.44 = private unnamed_addr constant [24 x i8] c"%\D8\8C \D8\AF\D9\88\D8\B1\D8\A7\D8\AA \D8\B4\D8\AD\D9\86: \00", align 1
@str.const.46 = private unnamed_addr constant [34 x i8] c"[BAT] \D8\A7\D9\84\D9\88\D9\82\D8\AA \D8\A7\D9\84\D9\85\D8\AA\D8\A8\D9\82\D9\8A: \00", align 1
@str.const.47 = private unnamed_addr constant [13 x i8] c" \D8\AF\D9\82\D9\8A\D9\82\D8\A9\0A\00", align 1
@str.const.48 = private unnamed_addr constant [41 x i8] c"[BAT] \D9\88\D9\82\D8\AA \D8\A7\D9\84\D8\B4\D8\AD\D9\86 \D8\A7\D9\84\D9\85\D8\AA\D8\A8\D9\82\D9\8A: \00", align 1
@str.const.50 = private unnamed_addr constant [78 x i8] c"[BAT] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\0A\00", align 1
@str.const.51 = private unnamed_addr constant [57 x i8] c"\0A[TH] \E2\95\90\E2\95\90\E2\95\90 \D8\A7\D9\84\D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D9\8A \E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.52 = private unnamed_addr constant [14 x i8] c"[TH] CPU:    \00", align 1
@str.const.53 = private unnamed_addr constant [14 x i8] c" [\D9\85\D8\AE\D8\AA\D9\86\D9\82]\00", align 1
@str.const.55 = private unnamed_addr constant [14 x i8] c"[TH] GPU:    \00", align 1
@str.const.58 = private unnamed_addr constant [14 x i8] c"[TH] Board:  \00", align 1
@str.const.60 = private unnamed_addr constant [14 x i8] c"[TH] Bat:    \00", align 1
@str.const.62 = private unnamed_addr constant [20 x i8] c"[TH] \D9\85\D8\B1\D9\88\D8\AD\D8\A9 0: \00", align 1
@str.const.63 = private unnamed_addr constant [7 x i8] c" RPM (\00", align 1
@str.const.64 = private unnamed_addr constant [8 x i8] c"% PWM)\0A\00", align 1
@str.const.65 = private unnamed_addr constant [20 x i8] c"[TH] \D9\85\D8\B1\D9\88\D8\AD\D8\A9 1: \00", align 1
@str.const.68 = private unnamed_addr constant [38 x i8] c"[TH] \D8\A3\D9\82\D8\B5\D9\89 \D8\AD\D8\B1\D8\A7\D8\B1\D8\A9 \D9\85\D8\B3\D8\AC\D9\84\D8\A9: \00", align 1
@str.const.70 = private unnamed_addr constant [31 x i8] c"[TH] \D8\A3\D8\AD\D8\AF\D8\A7\D8\AB \D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82: \00", align 1
@str.const.72 = private unnamed_addr constant [33 x i8] c"[TH] \D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA \D8\AD\D8\B1\D8\A7\D8\B1\D8\A9: \00", align 1
@str.const.73 = private unnamed_addr constant [18 x i8] c"\D8\8C \D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9: \00", align 1
@str.const.75 = private unnamed_addr constant [77 x i8] c"[TH] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\0A\00", align 1
@str.const.77 = private unnamed_addr constant [124 x i8] c"[TH] \E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\0A\00", align 1
@str.const.78 = private unnamed_addr constant [71 x i8] c"[TH]  \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9 \D9\88\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9 \E2\80\94 BeeOS\0A\00", align 1
@str.const.80 = private unnamed_addr constant [39 x i8] c"[TH] \D8\A7\D9\84\D9\85\D9\86\D8\A7\D8\B7\D9\82 \D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D9\8A\D8\A9:\0A\00", align 1
@str.const.81 = private unnamed_addr constant [49 x i8] c"[TH]   0: CPU  (\D8\AD\D8\B1\D8\AC: 95C\D8\8C \D8\A5\D9\8A\D9\82\D8\A7\D9\81: 105C)\0A\00", align 1
@str.const.82 = private unnamed_addr constant [50 x i8] c"[TH]   1: GPU  (\D8\AD\D8\B1\D8\AC: 100C\D8\8C \D8\A5\D9\8A\D9\82\D8\A7\D9\81: 110C)\0A\00", align 1
@str.const.83 = private unnamed_addr constant [50 x i8] c"[TH]   2: Board (\D8\AD\D8\B1\D8\AC: 90C\D8\8C \D8\A5\D9\8A\D9\82\D8\A7\D9\81: 100C)\0A\00", align 1
@str.const.84 = private unnamed_addr constant [48 x i8] c"[TH]   3: Bat  (\D8\AD\D8\B1\D8\AC: 55C\D8\8C \D8\A5\D9\8A\D9\82\D8\A7\D9\81: 60C)\0A\00", align 1
@str.const.85 = private unnamed_addr constant [31 x i8] c"[TH] \D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA \D8\A3\D9\88\D9\84\D9\8A\D8\A9:\0A\00", align 1
@str.const.86 = private unnamed_addr constant [13 x i8] c"[TH]   CPU: \00", align 1
@str.const.88 = private unnamed_addr constant [13 x i8] c"[TH]   GPU: \00", align 1
@str.const.90 = private unnamed_addr constant [20 x i8] c"[TH] \D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9: \00", align 1
@str.const.91 = private unnamed_addr constant [16 x i8] c"\D9\85\D9\88\D8\AC\D9\88\D8\AF\D8\A9\D8\8C \00", align 1
@str.const.93 = private unnamed_addr constant [5 x i8] c" Wh)\00", align 1
@str.const.94 = private unnamed_addr constant [20 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D9\88\D8\AC\D9\88\D8\AF\D8\A9\00", align 1
@str.const.96 = private unnamed_addr constant [22 x i8] c"[TH] \D8\A7\D9\84\D9\85\D8\B1\D8\A7\D9\88\D8\AD:\0A\00", align 1
@str.const.97 = private unnamed_addr constant [22 x i8] c"[TH]   0: CPU Fan (0-\00", align 1
@str.const.98 = private unnamed_addr constant [7 x i8] c" RPM)\0A\00", align 1
@str.const.99 = private unnamed_addr constant [23 x i8] c"[TH]   1: Case Fan (0-\00", align 1
@str.const.101 = private unnamed_addr constant [63 x i8] c"[TH] \E2\9C\93 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9 \D9\88\D8\A7\D9\84\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9 \D8\AC\D8\A7\D9\87\D8\B2\0A\0A\00", align 1

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

define void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
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

define i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\AD" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1" = alloca i64, align 8
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
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\AE\D8\A9" = alloca i64, align 8
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
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load28" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp29 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load28", 8
  br i1 %cmpeqtmp29, label %then_26, label %merge_27

merge_27:                                         ; preds = %then_26, %merge_25
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load31" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp32 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load31", 9
  br i1 %cmpeqtmp32, label %then_28, label %merge_29

merge_29:                                         ; preds = %then_28, %merge_27
  %"%\D9\86\D8\B3\D8\AE\D8\A9.load34" = load i64, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load35" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  %subtmp36 = sub i64 %"%\D9\86\D8\B3\D8\AE\D8\A9.load34", %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load35"
  %divtmp = sdiv i64 %subtmp36, 10
  store i64 %divtmp, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load37" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load37", 1
  store i64 %addtmp, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  br label %while_cond_7

merge_31:                                         ; preds = %then_30, %while_exit_9
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load38" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %subtmp39 = sub i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load38", 1
  store i64 %subtmp39, ptr %"%\D8\B9", align 8
  br label %while_cond_32

merge_36:                                         ; preds = %then_35, %while_body_33
  %"%\D8\B9.load42" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp43 = icmp eq i64 %"%\D8\B9.load42", 1
  br i1 %cmpeqtmp43, label %then_37, label %merge_38

merge_38:                                         ; preds = %then_37, %merge_36
  %"%\D8\B9.load44" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp45 = icmp eq i64 %"%\D8\B9.load44", 2
  br i1 %cmpeqtmp45, label %then_39, label %merge_40

merge_4:                                          ; preds = %entry
  store i64 0, ptr %"%\D8\B3\D8\A7\D9\84\D8\A8", align 8
  %"%10.load" = load i64, ptr %"\D9\861", align 8
  store i64 %"%10.load", ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %"%\D9\86\D8\B3\D8\AE\D8\A9.load" = load i64, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\B3\D8\AE\D8\A9.load", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

merge_40:                                         ; preds = %then_39, %merge_38
  %"%\D8\B9.load46" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp47 = icmp eq i64 %"%\D8\B9.load46", 3
  br i1 %cmpeqtmp47, label %then_41, label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %"%\D8\B9.load48" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp49 = icmp eq i64 %"%\D8\B9.load48", 4
  br i1 %cmpeqtmp49, label %then_43, label %merge_44

merge_44:                                         ; preds = %then_43, %merge_42
  %"%\D8\B9.load50" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp51 = icmp eq i64 %"%\D8\B9.load50", 5
  br i1 %cmpeqtmp51, label %then_45, label %merge_46

merge_46:                                         ; preds = %then_45, %merge_44
  %"%\D8\B9.load52" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp53 = icmp eq i64 %"%\D8\B9.load52", 6
  br i1 %cmpeqtmp53, label %then_47, label %merge_48

merge_48:                                         ; preds = %then_47, %merge_46
  %"%\D8\B9.load54" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp55 = icmp eq i64 %"%\D8\B9.load54", 7
  br i1 %cmpeqtmp55, label %then_49, label %merge_50

merge_50:                                         ; preds = %then_49, %merge_48
  %"%\D8\B9.load56" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp57 = icmp eq i64 %"%\D8\B9.load56", 8
  br i1 %cmpeqtmp57, label %then_51, label %merge_52

merge_52:                                         ; preds = %then_51, %merge_50
  %"%\D8\B9.load58" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp59 = icmp eq i64 %"%\D8\B9.load58", 9
  br i1 %cmpeqtmp59, label %then_53, label %merge_54

merge_54:                                         ; preds = %then_53, %merge_52
  %"%\D8\B9.load60" = load i64, ptr %"%\D8\B9", align 8
  %subtmp61 = sub i64 %"%\D8\B9.load60", 1
  store i64 %subtmp61, ptr %"%\D8\B9", align 8
  br label %while_cond_32

merge_6:                                          ; preds = %then_5, %merge_4
  store i64 0, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
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
  br label %while_cond_7

then_10:                                          ; preds = %while_body_8
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  br label %merge_11

then_12:                                          ; preds = %merge_11
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load9" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load9", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  br label %merge_13

then_14:                                          ; preds = %merge_13
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load12" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load12", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  br label %merge_15

then_16:                                          ; preds = %merge_15
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load15" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load15", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  br label %merge_17

then_18:                                          ; preds = %merge_17
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load18" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load18", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  br label %merge_19

then_20:                                          ; preds = %merge_19
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load21" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load21", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  br label %merge_21

then_22:                                          ; preds = %merge_21
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load24" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load24", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  br label %merge_23

then_24:                                          ; preds = %merge_23
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load27" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load27", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  br label %merge_25

then_26:                                          ; preds = %merge_25
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load30" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load30", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_8", align 8
  br label %merge_27

then_28:                                          ; preds = %merge_27
  %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load33" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1.load33", ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_9", align 8
  br label %merge_29

then_3:                                           ; preds = %entry
  ret i64 0

then_30:                                          ; preds = %while_exit_9
  br label %merge_31

then_35:                                          ; preds = %while_body_33
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_0.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_36

then_37:                                          ; preds = %merge_36
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_1.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_38

then_39:                                          ; preds = %merge_38
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_2.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_40

then_41:                                          ; preds = %merge_40
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_3.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_42

then_43:                                          ; preds = %merge_42
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_4.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_44

then_45:                                          ; preds = %merge_44
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_5.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_46

then_47:                                          ; preds = %merge_46
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_6.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_48

then_49:                                          ; preds = %merge_48
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_7.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_50

then_5:                                           ; preds = %merge_4
  store i64 1, ptr %"%\D8\B3\D8\A7\D9\84\D8\A8", align 8
  %"%\D9\86\D8\B3\D8\AE\D8\A9.load2" = load i64, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %subtmp = sub i64 0, %"%\D9\86\D8\B3\D8\AE\D8\A9.load2"
  store i64 %subtmp, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  br label %merge_6

then_51:                                          ; preds = %merge_50
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_8.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_8", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_8.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_52

then_53:                                          ; preds = %merge_52
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_9.load" = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_9", align 8
  store i64 %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_9.load", ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  br label %merge_54

while_body_33:                                    ; preds = %while_cond_32
  store i64 0, ptr %"%\D8\B1\D9\82\D9\85_\D8\AD", align 8
  %"%\D8\B9.load40" = load i64, ptr %"%\D8\B9", align 8
  %cmpeqtmp41 = icmp eq i64 %"%\D8\B9.load40", 0
  br i1 %cmpeqtmp41, label %then_35, label %merge_36

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\86\D8\B3\D8\AE\D8\A9.load4" = load i64, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %modtmp = srem i64 %"%\D9\86\D8\B3\D8\AE\D8\A9.load4", 10
  store i64 %modtmp, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A_\D8\B1", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AE\D8\A7\D9\86\D8\A7\D8\AA.load", 0
  br i1 %cmpeqtmp5, label %then_10, label %merge_11

while_cond_32:                                    ; preds = %merge_54, %merge_31
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9.load", 0
  br i1 %cmpgetmp, label %while_body_33, label %while_exit_34

while_cond_7:                                     ; preds = %merge_29, %merge_6
  %"%\D9\86\D8\B3\D8\AE\D8\A9.load3" = load i64, ptr %"%\D9\86\D8\B3\D8\AE\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\B3\D8\AE\D8\A9.load3", 0
  br i1 %cmpgttmp, label %while_body_8, label %while_exit_9

while_exit_34:                                    ; preds = %while_cond_32
  ret i64 0

while_exit_9:                                     ; preds = %while_cond_7
  %"%\D8\B3\D8\A7\D9\84\D8\A8.load" = load i64, ptr %"%\D8\B3\D8\A7\D9\84\D8\A8", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\B3\D8\A7\D9\84\D8\A8.load", 1
  br i1 %cmpeqtmp6, label %then_30, label %merge_31
}

define void @"th_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B3") {
entry:
  %"%\D9\83\D8\B3\D8\B1" = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA" = alloca i64, align 8
  %"\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B31" = alloca i64, align 8
  store i64 %"\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B3", ptr %"\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B31", align 8
  %"%\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B3.load" = load i64, ptr %"\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B31", align 8
  %divtmp = sdiv i64 %"%\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B3.load", 1000
  store i64 %divtmp, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA", align 8
  %"%\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B3.load2" = load i64, ptr %"\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B31", align 8
  %modtmp = srem i64 %"%\D9\85\D9\84\D9\8A_\D8\B3\D9\84\D8\B2\D9\8A\D9\88\D8\B3.load2", 1000
  %divtmp3 = sdiv i64 %modtmp, 100
  store i64 %divtmp3, ptr %"%\D9\83\D8\B3\D8\B1", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA", align 8
  %calltmp = call i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA.load")
  %"%\D9\83\D8\B3\D8\B1.load" = load i64, ptr %"%\D9\83\D8\B3\D8\B1", align 8
  %calltmp4 = call i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\83\D8\B3\D8\B1.load")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  ret void
}

define i64 @"th_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9"(i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id") {
entry:
  %"%\D8\AA\D9\82\D9\84\D8\A8" = alloca i64, align 8
  %"%ticks" = alloca i64, align 8
  %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id", ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%ticks", align 8
  %"%ticks.load" = load i64, ptr %"%ticks", align 8
  %modtmp = srem i64 %"%ticks.load", 100
  %multmp = mul i64 %modtmp, 50
  store i64 %multmp, ptr %"%\D8\AA\D9\82\D9\84\D8\A8", align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load", 0
  br i1 %cmpeqtmp, label %then_55, label %merge_56

merge_56:                                         ; preds = %entry
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load2" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load2", 1
  br i1 %cmpeqtmp3, label %then_57, label %merge_58

merge_58:                                         ; preds = %merge_56
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load6" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load6", 2
  br i1 %cmpeqtmp7, label %then_59, label %merge_60

merge_60:                                         ; preds = %merge_58
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load10" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load10", 3
  br i1 %cmpeqtmp11, label %then_61, label %merge_62

merge_62:                                         ; preds = %merge_60
  %"%\D8\AA\D9\82\D9\84\D8\A8.load14" = load i64, ptr %"%\D8\AA\D9\82\D9\84\D8\A8", align 8
  %addtmp15 = add i64 30000, %"%\D8\AA\D9\82\D9\84\D8\A8.load14"
  ret i64 %addtmp15

then_55:                                          ; preds = %entry
  %"%\D8\AA\D9\82\D9\84\D8\A8.load" = load i64, ptr %"%\D8\AA\D9\82\D9\84\D8\A8", align 8
  %addtmp = add i64 45000, %"%\D8\AA\D9\82\D9\84\D8\A8.load"
  ret i64 %addtmp

then_57:                                          ; preds = %merge_56
  %"%\D8\AA\D9\82\D9\84\D8\A8.load4" = load i64, ptr %"%\D8\AA\D9\82\D9\84\D8\A8", align 8
  %addtmp5 = add i64 38000, %"%\D8\AA\D9\82\D9\84\D8\A8.load4"
  ret i64 %addtmp5

then_59:                                          ; preds = %merge_58
  %"%\D8\AA\D9\82\D9\84\D8\A8.load8" = load i64, ptr %"%\D8\AA\D9\82\D9\84\D8\A8", align 8
  %addtmp9 = add i64 32000, %"%\D8\AA\D9\82\D9\84\D8\A8.load8"
  ret i64 %addtmp9

then_61:                                          ; preds = %merge_60
  %"%\D8\AA\D9\82\D9\84\D8\A8.load12" = load i64, ptr %"%\D8\AA\D9\82\D9\84\D8\A8", align 8
  %shrtmp = lshr i64 %"%\D8\AA\D9\82\D9\84\D8\A8.load12", 1
  %addtmp13 = add i64 28000, %shrtmp
  ret i64 %addtmp13
}

define i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"() {
entry:
  %"%st" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  store i64 0, ptr %"%timeout", align 8
  br label %while_cond_63

merge_67:                                         ; preds = %while_body_64
  %"%timeout.load1" = load i64, ptr %"%timeout", align 8
  %addtmp = add i64 %"%timeout.load1", 1
  store i64 %addtmp, ptr %"%timeout", align 8
  br label %while_cond_63

then_66:                                          ; preds = %while_body_64
  ret i64 1

while_body_64:                                    ; preds = %while_cond_63
  store i64 0, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %andtmp = and i64 %"%st.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_66, label %merge_67

while_cond_63:                                    ; preds = %merge_67, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmplttmp = icmp slt i64 %"%timeout.load", 10000
  br i1 %cmplttmp, label %while_body_64, label %while_exit_65

while_exit_65:                                    ; preds = %while_cond_63
  ret i64 0
}

define i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"() {
entry:
  %"%st" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  store i64 0, ptr %"%timeout", align 8
  br label %while_cond_68

merge_72:                                         ; preds = %while_body_69
  %"%timeout.load1" = load i64, ptr %"%timeout", align 8
  %addtmp = add i64 %"%timeout.load1", 1
  store i64 %addtmp, ptr %"%timeout", align 8
  br label %while_cond_68

then_71:                                          ; preds = %while_body_69
  ret i64 1

while_body_69:                                    ; preds = %while_cond_68
  store i64 0, ptr %"%st", align 8
  %"%st.load" = load i64, ptr %"%st", align 8
  %shrtmp = lshr i64 %"%st.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_71, label %merge_72

while_cond_68:                                    ; preds = %merge_72, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmplttmp = icmp slt i64 %"%timeout.load", 10000
  br i1 %cmplttmp, label %while_body_69, label %while_exit_70

while_exit_70:                                    ; preds = %while_cond_68
  ret i64 0
}

define i64 @"th_ec_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"\D8\B3\D8\AC\D9\84") {
entry:
  %"\D8\B3\D8\AC\D9\841" = alloca i64, align 8
  store i64 %"\D8\B3\D8\AC\D9\84", ptr %"\D8\B3\D8\AC\D9\841", align 8
  %calltmp = call i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp2 = call i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp3 = call i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"()
  ret i64 0
}

define i64 @"th_ec_\D8\A7\D9\83\D8\AA\D8\A8"(i64 %"\D8\B3\D8\AC\D9\84", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D8\B3\D8\AC\D9\841" = alloca i64, align 8
  store i64 %"\D8\B3\D8\AC\D9\84", ptr %"\D8\B3\D8\AC\D9\841", align 8
  %calltmp = call i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp3 = call i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp4 = call i64 @"th_ec_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  ret i64 0
}

define i64 @"th_\D8\A7\D9\82\D8\B1\D8\A3_ec"(i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id") {
entry:
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_c" = alloca i64, align 8
  %"%\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id", ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %addtmp = add i64 120, %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load"
  store i64 %addtmp, ptr %"%\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %"%\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %"%\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %calltmp = call i64 @"th_ec_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load")
  store i64 %calltmp, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A9_c", align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9_c.load" = load i64, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A9_c", align 8
  %multmp = mul i64 %"%\D8\AF\D8\B1\D8\AC\D8\A9_c.load", 1000
  ret i64 %multmp
}

define i64 @"th_\D8\A7\D9\82\D8\B1\D8\A3_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D8\A9"(i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id") {
entry:
  %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1" = alloca i64, align 8
  %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id", ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load", 0
  br i1 %cmpeqtmp, label %then_73, label %merge_74

merge_74:                                         ; preds = %then_73, %entry
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load2" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load2", 1
  br i1 %cmpeqtmp3, label %then_75, label %merge_76

merge_76:                                         ; preds = %then_75, %merge_74
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load4" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load4", 2
  br i1 %cmpeqtmp5, label %then_77, label %merge_78

merge_78:                                         ; preds = %then_77, %merge_76
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load6" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load6", 3
  br i1 %cmpeqtmp7, label %then_79, label %merge_80

merge_80:                                         ; preds = %then_79, %merge_78
  %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load", 0
  br i1 %cmpeqtmp8, label %then_81, label %merge_82

merge_82:                                         ; preds = %merge_80
  %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load10" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load10", 0
  br i1 %cmpeqtmp11, label %then_83, label %merge_84

merge_84:                                         ; preds = %merge_82
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load14" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %calltmp15 = call i64 @"th_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9"(i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load14")
  ret i64 %calltmp15

then_73:                                          ; preds = %entry
  store i64 0, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  br label %merge_74

then_75:                                          ; preds = %merge_74
  store i64 0, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  br label %merge_76

then_77:                                          ; preds = %merge_76
  store i64 0, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  br label %merge_78

then_79:                                          ; preds = %merge_78
  store i64 0, ptr %"%\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", align 8
  br label %merge_80

then_81:                                          ; preds = %merge_80
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load9" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %calltmp = call i64 @"th_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9"(i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load9")
  ret i64 %calltmp

then_83:                                          ; preds = %merge_82
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load12" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %calltmp13 = call i64 @"th_\D8\A7\D9\82\D8\B1\D8\A3_ec"(i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load12")
  ret i64 %calltmp13
}

define i64 @"th_\D8\A7\D8\AD\D8\B3\D8\A8_pwm"(i64 %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", i64 %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1", i64 %"\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\89") {
entry:
  %"%\D9\81\D8\B1\D9\82" = alloca i64, align 8
  %"%\D9\86\D8\B7\D8\A7\D9\82" = alloca i64, align 8
  %"\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\893" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\89", ptr %"\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\893", align 8
  %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A12" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1", ptr %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A12", align 8
  %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A91", align 8
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A91", align 8
  %"%\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1.load" = load i64, ptr %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A12", align 8
  %cmpletmp = icmp sle i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load", %"%\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1.load"
  br i1 %cmpletmp, label %then_85, label %merge_86

merge_86:                                         ; preds = %entry
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load4" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A91", align 8
  %"%\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\89.load" = load i64, ptr %"\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\893", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load4", %"%\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\89.load"
  br i1 %cmpgetmp, label %then_87, label %merge_88

merge_88:                                         ; preds = %merge_86
  %"%\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\89.load5" = load i64, ptr %"\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\893", align 8
  %"%\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1.load6" = load i64, ptr %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A12", align 8
  %subtmp = sub i64 %"%\D8\AD\D8\AF_\D8\A3\D9\82\D8\B5\D9\89.load5", %"%\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1.load6"
  store i64 %subtmp, ptr %"%\D9\86\D8\B7\D8\A7\D9\82", align 8
  %"%\D9\86\D8\B7\D8\A7\D9\82.load" = load i64, ptr %"%\D9\86\D8\B7\D8\A7\D9\82", align 8
  %cmpletmp7 = icmp sle i64 %"%\D9\86\D8\B7\D8\A7\D9\82.load", 0
  br i1 %cmpletmp7, label %then_89, label %merge_90

merge_90:                                         ; preds = %merge_88
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load8" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A91", align 8
  %"%\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1.load9" = load i64, ptr %"\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A12", align 8
  %subtmp10 = sub i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load8", %"%\D8\AD\D8\AF_\D8\A8\D8\AF\D8\A1.load9"
  store i64 %subtmp10, ptr %"%\D9\81\D8\B1\D9\82", align 8
  %"%\D9\81\D8\B1\D9\82.load" = load i64, ptr %"%\D9\81\D8\B1\D9\82", align 8
  %multmp = mul i64 %"%\D9\81\D8\B1\D9\82.load", 100
  %"%\D9\86\D8\B7\D8\A7\D9\82.load11" = load i64, ptr %"%\D9\86\D8\B7\D8\A7\D9\82", align 8
  %divtmp = sdiv i64 %multmp, %"%\D9\86\D8\B7\D8\A7\D9\82.load11"
  ret i64 %divtmp

then_85:                                          ; preds = %entry
  ret i64 0

then_87:                                          ; preds = %merge_86
  ret i64 100

then_89:                                          ; preds = %merge_88
  ret i64 100
}

define i64 @"th_\D8\AD\D8\AF\D8\AB_\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0"(i64 %"\D9\86\D8\B3\D8\A8\D8\A9") {
entry:
  %"%\D9\86\D9\82\D8\B5" = alloca i64, align 8
  %"%\D8\B2\D9\8A\D8\A7\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\B7\D8\A7\D9\82_\D8\B3\D8\B1\D8\B9\D8\A9" = alloca i64, align 8
  %"\D9\86\D8\B3\D8\A8\D8\A91" = alloca i64, align 8
  store i64 %"\D9\86\D8\B3\D8\A8\D8\A9", ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  br i1 true, label %then_91, label %merge_92

merge_100:                                        ; preds = %merge_104, %merge_98
  br label %merge_96

merge_102:                                        ; preds = %then_101, %then_99
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load8" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp9 = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load8", 50
  br i1 %cmpgttmp9, label %then_103, label %merge_104

merge_104:                                        ; preds = %merge_108, %merge_102
  br label %merge_100

merge_106:                                        ; preds = %then_105, %then_103
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load12" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp13 = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load12", 75
  br i1 %cmpgttmp13, label %then_107, label %merge_108

merge_108:                                        ; preds = %then_107, %merge_106
  br label %merge_104

merge_110:                                        ; preds = %merge_114, %merge_96
  br i1 false, label %then_115, label %merge_116

merge_112:                                        ; preds = %then_111, %then_109
  br i1 false, label %then_113, label %merge_114

merge_114:                                        ; preds = %then_113, %merge_112
  br label %merge_110

merge_116:                                        ; preds = %merge_120, %merge_110
  ret i64 0

merge_118:                                        ; preds = %then_117, %then_115
  br i1 false, label %then_119, label %merge_120

merge_120:                                        ; preds = %then_119, %merge_118
  br label %merge_116

merge_92:                                         ; preds = %entry
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_93, label %merge_94

merge_94:                                         ; preds = %then_93, %merge_92
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load2" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load2", 0
  br i1 %cmpgttmp, label %then_95, label %merge_96

merge_96:                                         ; preds = %merge_100, %merge_94
  br i1 false, label %then_109, label %merge_110

merge_98:                                         ; preds = %then_97, %then_95
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load4" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp5 = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load4", 25
  br i1 %cmpgttmp5, label %then_99, label %merge_100

then_101:                                         ; preds = %then_99
  br label %merge_102

then_103:                                         ; preds = %merge_102
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load10" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpletmp11 = icmp sle i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load10", 75
  br i1 %cmpletmp11, label %then_105, label %merge_106

then_105:                                         ; preds = %then_103
  br label %merge_106

then_107:                                         ; preds = %merge_106
  br label %merge_108

then_109:                                         ; preds = %merge_96
  store i64 0, ptr %"%\D8\B2\D9\8A\D8\A7\D8\AF\D8\A9", align 8
  %"%\D8\B2\D9\8A\D8\A7\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B2\D9\8A\D8\A7\D8\AF\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B2\D9\8A\D8\A7\D8\AF\D8\A9.load", 50
  br i1 %cmplttmp, label %then_111, label %merge_112

then_111:                                         ; preds = %then_109
  store i64 50, ptr %"%\D8\B2\D9\8A\D8\A7\D8\AF\D8\A9", align 8
  br label %merge_112

then_113:                                         ; preds = %merge_112
  br label %merge_114

then_115:                                         ; preds = %merge_110
  store i64 0, ptr %"%\D9\86\D9\82\D8\B5", align 8
  %"%\D9\86\D9\82\D8\B5.load" = load i64, ptr %"%\D9\86\D9\82\D8\B5", align 8
  %cmplttmp14 = icmp slt i64 %"%\D9\86\D9\82\D8\B5.load", 30
  br i1 %cmplttmp14, label %then_117, label %merge_118

then_117:                                         ; preds = %then_115
  store i64 30, ptr %"%\D9\86\D9\82\D8\B5", align 8
  br label %merge_118

then_119:                                         ; preds = %merge_118
  br label %merge_120

then_91:                                          ; preds = %entry
  ret i64 0

then_93:                                          ; preds = %merge_92
  br label %merge_94

then_95:                                          ; preds = %merge_94
  store i64 0, ptr %"%\D9\86\D8\B7\D8\A7\D9\82_\D8\B3\D8\B1\D8\B9\D8\A9", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load3" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpletmp = icmp sle i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load3", 25
  br i1 %cmpletmp, label %then_97, label %merge_98

then_97:                                          ; preds = %then_95
  br label %merge_98

then_99:                                          ; preds = %merge_98
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load6" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpletmp7 = icmp sle i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load6", 50
  br i1 %cmpletmp7, label %then_101, label %merge_102
}

define i64 @"th_\D8\AD\D8\AF\D8\AB_\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1"(i64 %"\D9\86\D8\B3\D8\A8\D8\A9") {
entry:
  %"%\D9\86\D8\B7\D8\A7\D9\82_\D8\B3" = alloca i64, align 8
  %"\D9\86\D8\B3\D8\A8\D8\A91" = alloca i64, align 8
  store i64 %"\D9\86\D8\B3\D8\A8\D8\A9", ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  br i1 true, label %then_121, label %merge_122

merge_122:                                        ; preds = %entry
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_123, label %merge_124

merge_124:                                        ; preds = %then_123, %merge_122
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load2" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load2", 0
  br i1 %cmpgttmp, label %then_125, label %merge_126

merge_126:                                        ; preds = %merge_130, %merge_124
  br i1 false, label %then_135, label %merge_136

merge_128:                                        ; preds = %then_127, %then_125
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load4" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp5 = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load4", 25
  br i1 %cmpgttmp5, label %then_129, label %merge_130

merge_130:                                        ; preds = %merge_134, %merge_128
  br label %merge_126

merge_132:                                        ; preds = %then_131, %then_129
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load8" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpgttmp9 = icmp sgt i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load8", 60
  br i1 %cmpgttmp9, label %then_133, label %merge_134

merge_134:                                        ; preds = %then_133, %merge_132
  br label %merge_130

merge_136:                                        ; preds = %merge_138, %merge_126
  br i1 false, label %then_139, label %merge_140

merge_138:                                        ; preds = %then_137, %then_135
  br label %merge_136

merge_140:                                        ; preds = %merge_142, %merge_136
  ret i64 0

merge_142:                                        ; preds = %then_141, %then_139
  br label %merge_140

then_121:                                         ; preds = %entry
  ret i64 0

then_123:                                         ; preds = %merge_122
  br label %merge_124

then_125:                                         ; preds = %merge_124
  store i64 0, ptr %"%\D9\86\D8\B7\D8\A7\D9\82_\D8\B3", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load3" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpletmp = icmp sle i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load3", 25
  br i1 %cmpletmp, label %then_127, label %merge_128

then_127:                                         ; preds = %then_125
  br label %merge_128

then_129:                                         ; preds = %merge_128
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load6" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %cmpletmp7 = icmp sle i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load6", 60
  br i1 %cmpletmp7, label %then_131, label %merge_132

then_131:                                         ; preds = %then_129
  br label %merge_132

then_133:                                         ; preds = %merge_132
  br label %merge_134

then_135:                                         ; preds = %merge_126
  br i1 false, label %then_137, label %merge_138

then_137:                                         ; preds = %then_135
  br label %merge_138

then_139:                                         ; preds = %merge_136
  br i1 false, label %then_141, label %merge_142

then_141:                                         ; preds = %then_139
  br label %merge_142
}

define i64 @"th_\D8\B7\D8\A8\D9\82_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82"(i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id", i64 %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9") {
entry:
  %"%\D8\AD\D8\AF_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA" = alloca i64, align 8
  %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id", ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82", align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load", 0
  br i1 %cmpeqtmp, label %then_143, label %merge_144

merge_144:                                        ; preds = %then_143, %entry
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load3" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load3", 1
  br i1 %cmpeqtmp4, label %then_145, label %merge_146

merge_146:                                        ; preds = %then_145, %merge_144
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load5" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load5", 2
  br i1 %cmpeqtmp6, label %then_147, label %merge_148

merge_148:                                        ; preds = %then_147, %merge_146
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load7" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load7", 3
  br i1 %cmpeqtmp8, label %then_149, label %merge_150

merge_150:                                        ; preds = %then_149, %merge_148
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82.load" = load i64, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load", %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82.load"
  br i1 %cmpgetmp, label %then_151, label %merge_152

merge_152:                                        ; preds = %merge_150
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load11" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85.load" = load i64, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85", align 8
  %cmpgetmp12 = icmp sge i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load11", %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85.load"
  br i1 %cmpgetmp12, label %then_153, label %merge_154

merge_154:                                        ; preds = %merge_152
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load18" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA.load" = load i64, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  %cmpgetmp19 = icmp sge i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load18", %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA.load"
  br i1 %cmpgetmp19, label %then_159, label %merge_160

merge_156:                                        ; preds = %then_155, %then_153
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load20" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp21 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load20", 1
  br i1 %cmpeqtmp21, label %then_157, label %merge_158

merge_158:                                        ; preds = %then_157, %merge_156
  ret i64 2

merge_160:                                        ; preds = %merge_154
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load24" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA.load25" = load i64, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load24", %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA.load25"
  br i1 %cmplttmp, label %then_169, label %merge_170

merge_162:                                        ; preds = %merge_164, %then_159
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load26" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp27 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load26", 1
  br i1 %cmpeqtmp27, label %then_165, label %merge_166

merge_164:                                        ; preds = %then_163, %then_161
  br label %merge_162

merge_166:                                        ; preds = %merge_168, %merge_162
  ret i64 1

merge_168:                                        ; preds = %then_167, %then_165
  br label %merge_166

merge_170:                                        ; preds = %merge_172, %merge_160
  ret i64 0

merge_172:                                        ; preds = %merge_178, %then_169
  br label %merge_170

merge_174:                                        ; preds = %merge_176, %then_171
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load35" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp36 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load35", 1
  br i1 %cmpeqtmp36, label %then_177, label %merge_178

merge_176:                                        ; preds = %then_175, %then_173
  br label %merge_174

merge_178:                                        ; preds = %merge_180, %merge_174
  br label %merge_172

merge_180:                                        ; preds = %then_179, %then_177
  br label %merge_178

then_143:                                         ; preds = %entry
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82", align 8
  br label %merge_144

then_145:                                         ; preds = %merge_144
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82", align 8
  br label %merge_146

then_147:                                         ; preds = %merge_146
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82", align 8
  br label %merge_148

then_149:                                         ; preds = %merge_148
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC_\D9\85", align 8
  store i64 0, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82", align 8
  br label %merge_150

then_151:                                         ; preds = %merge_150
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load9" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %calltmp = call i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load9")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load10" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  call void @"th_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load10")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  ret i64 3

then_153:                                         ; preds = %merge_152
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load13" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %calltmp14 = call i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load13")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load15" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  call void @"th_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load15")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load16" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load16", 0
  br i1 %cmpeqtmp17, label %then_155, label %merge_156

then_155:                                         ; preds = %then_153
  br label %merge_156

then_157:                                         ; preds = %merge_156
  br label %merge_158

then_159:                                         ; preds = %merge_154
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load22" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load22", 0
  br i1 %cmpeqtmp23, label %then_161, label %merge_162

then_161:                                         ; preds = %then_159
  br i1 true, label %then_163, label %merge_164

then_163:                                         ; preds = %then_161
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load28" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  call void @"th_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load28")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  br label %merge_164

then_165:                                         ; preds = %merge_162
  br i1 true, label %then_167, label %merge_168

then_167:                                         ; preds = %then_165
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load29" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  call void @"th_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load29")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  br label %merge_168

then_169:                                         ; preds = %merge_160
  %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA.load30" = load i64, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA", align 8
  %subtmp = sub i64 %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA.load30", 5000
  store i64 %subtmp, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1", align 8
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load31" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  %"%\D8\AD\D8\AF_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1.load" = load i64, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1", align 8
  %cmplttmp32 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load31", %"%\D8\AD\D8\AF_\D8\A5\D9\84\D8\BA\D8\A7\D8\A1.load"
  br i1 %cmplttmp32, label %then_171, label %merge_172

then_171:                                         ; preds = %then_169
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load33" = load i64, ptr %"\D9\85\D9\86\D8\B7\D9\82\D8\A9_id1", align 8
  %cmpeqtmp34 = icmp eq i64 %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_id.load33", 0
  br i1 %cmpeqtmp34, label %then_173, label %merge_174

then_173:                                         ; preds = %then_171
  br i1 false, label %then_175, label %merge_176

then_175:                                         ; preds = %then_173
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load37" = load i64, ptr %"\D8\AD\D8\B1\D8\A7\D8\B1\D8\A92", align 8
  call void @"th_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9"(i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load37")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  br label %merge_176

then_177:                                         ; preds = %merge_174
  br i1 false, label %then_179, label %merge_180

then_179:                                         ; preds = %then_177
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  br label %merge_180
}

define i64 @"bat_\D8\AD\D8\AF\D8\AB"() {
entry:
  %"%\D9\85\D8\AA\D8\A8\D9\82\D9\8A_mwh" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B6\D9\89" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  br i1 true, label %then_181, label %merge_182

merge_182:                                        ; preds = %entry
  call void @sad_ll_timer_get_ticks()
  store i64 0, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %"%\D8\A7\D9\84\D8\A2\D9\86.load" = load i64, ptr %"%\D8\A7\D9\84\D8\A2\D9\86", align 8
  %subtmp = sub i64 %"%\D8\A7\D9\84\D8\A2\D9\86.load", 0
  store i64 %subtmp, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %"%\D9\85\D8\B6\D9\89.load" = load i64, ptr %"%\D9\85\D8\B6\D9\89", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\B6\D9\89.load", 100
  br i1 %cmplttmp, label %then_183, label %merge_184

merge_184:                                        ; preds = %merge_182
  br i1 true, label %then_185, label %merge_186

merge_186:                                        ; preds = %merge_198, %merge_184
  br i1 true, label %then_201, label %merge_202

merge_188:                                        ; preds = %then_187, %then_185
  br i1 false, label %then_189, label %merge_190

merge_190:                                        ; preds = %then_189, %merge_188
  br i1 false, label %then_191, label %merge_192

merge_192:                                        ; preds = %then_191, %merge_190
  br i1 true, label %then_193, label %merge_194

merge_194:                                        ; preds = %merge_196, %merge_192
  br i1 false, label %then_197, label %merge_198

merge_196:                                        ; preds = %then_195, %then_193
  br label %merge_194

merge_198:                                        ; preds = %merge_200, %merge_194
  br label %merge_186

merge_200:                                        ; preds = %then_199, %then_197
  br label %merge_198

merge_202:                                        ; preds = %merge_214, %merge_186
  ret i64 1

merge_204:                                        ; preds = %then_203, %then_201
  br i1 false, label %then_205, label %merge_206

merge_206:                                        ; preds = %merge_208, %merge_204
  br i1 true, label %then_209, label %merge_210

merge_208:                                        ; preds = %then_207, %then_205
  br label %merge_206

merge_210:                                        ; preds = %merge_212, %merge_206
  br i1 false, label %then_213, label %merge_214

merge_212:                                        ; preds = %then_211, %then_209
  br label %merge_210

merge_214:                                        ; preds = %then_213, %merge_210
  br label %merge_202

then_181:                                         ; preds = %entry
  ret i64 0

then_183:                                         ; preds = %merge_182
  ret i64 0

then_185:                                         ; preds = %merge_184
  br i1 false, label %then_187, label %merge_188

then_187:                                         ; preds = %then_185
  br label %merge_188

then_189:                                         ; preds = %merge_188
  br label %merge_190

then_191:                                         ; preds = %merge_190
  br label %merge_192

then_193:                                         ; preds = %merge_192
  br i1 false, label %then_195, label %merge_196

then_195:                                         ; preds = %then_193
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  br label %merge_196

then_197:                                         ; preds = %merge_194
  br i1 true, label %then_199, label %merge_200

then_199:                                         ; preds = %then_197
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  br label %merge_200

then_201:                                         ; preds = %merge_186
  store i64 0, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  br i1 false, label %then_203, label %merge_204

then_203:                                         ; preds = %then_201
  store i64 poison, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  br label %merge_204

then_205:                                         ; preds = %merge_204
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmpgetmp, label %then_207, label %merge_208

then_207:                                         ; preds = %then_205
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load1" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %calltmp = call i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load1")
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  br label %merge_208

then_209:                                         ; preds = %merge_206
  br i1 true, label %then_211, label %merge_212

then_211:                                         ; preds = %then_209
  br label %merge_212

then_213:                                         ; preds = %merge_210
  store i64 0, ptr %"%\D9\85\D8\AA\D8\A8\D9\82\D9\8A_mwh", align 8
  br label %merge_214
}

define i64 @"bat_\D8\A7\D8\A8\D8\AF\D8\A3_\D8\B4\D8\AD\D9\86"() {
entry:
  br i1 true, label %then_215, label %merge_216

merge_216:                                        ; preds = %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  ret i64 1

then_215:                                         ; preds = %entry
  ret i64 0
}

define i64 @"bat_\D8\A7\D8\A8\D8\AF\D8\A3_\D8\AA\D9\81\D8\B1\D9\8A\D8\BA"() {
entry:
  br i1 true, label %then_217, label %merge_218

merge_218:                                        ; preds = %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  ret i64 1

then_217:                                         ; preds = %entry
  ret i64 0
}

define i64 @"bat_\D8\A7\D9\84\D8\B4\D8\AD\D9\86"() {
entry:
  ret i64 0
}

define i64 @"bat_\D8\A7\D9\84\D8\AD\D8\A7\D9\84"() {
entry:
  ret i64 0
}

define i64 @"bat_\D8\AD\D8\AF\D9\88\D8\AF_\D8\B4\D8\AD\D9\86"(i64 %"\D8\A3\D8\B9\D9\84\D9\89", i64 %"\D8\A3\D8\AF\D9\86\D9\89") {
entry:
  %"\D8\A3\D8\AF\D9\86\D9\892" = alloca i64, align 8
  store i64 %"\D8\A3\D8\AF\D9\86\D9\89", ptr %"\D8\A3\D8\AF\D9\86\D9\892", align 8
  %"\D8\A3\D8\B9\D9\84\D9\891" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B9\D9\84\D9\89", ptr %"\D8\A3\D8\B9\D9\84\D9\891", align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D9\891", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A3\D8\B9\D9\84\D9\89.load", 0
  br i1 %cmpgttmp, label %then_219, label %merge_220

merge_220:                                        ; preds = %merge_222, %entry
  %"%\D8\A3\D8\AF\D9\86\D9\89.load" = load i64, ptr %"\D8\A3\D8\AF\D9\86\D9\892", align 8
  %cmpgttmp4 = icmp sgt i64 %"%\D8\A3\D8\AF\D9\86\D9\89.load", 0
  br i1 %cmpgttmp4, label %then_223, label %merge_224

merge_222:                                        ; preds = %then_221, %then_219
  br label %merge_220

merge_224:                                        ; preds = %merge_226, %merge_220
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.22 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  ret i64 0

merge_226:                                        ; preds = %then_225, %then_223
  br label %merge_224

then_219:                                         ; preds = %entry
  %"%\D8\A3\D8\B9\D9\84\D9\89.load3" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D9\891", align 8
  %cmpletmp = icmp sle i64 %"%\D8\A3\D8\B9\D9\84\D9\89.load3", 100
  br i1 %cmpletmp, label %then_221, label %merge_222

then_221:                                         ; preds = %then_219
  br label %merge_222

then_223:                                         ; preds = %merge_220
  %"%\D8\A3\D8\AF\D9\86\D9\89.load5" = load i64, ptr %"\D8\A3\D8\AF\D9\86\D9\892", align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89.load6" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D9\891", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A3\D8\AF\D9\86\D9\89.load5", %"%\D8\A3\D8\B9\D9\84\D9\89.load6"
  br i1 %cmplttmp, label %then_225, label %merge_226

then_225:                                         ; preds = %then_223
  br label %merge_226
}

define i64 @"bat_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  br i1 true, label %then_227, label %merge_228

merge_228:                                        ; preds = %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  br i1 true, label %then_229, label %merge_230

merge_230:                                        ; preds = %then_229, %merge_228
  br i1 false, label %then_231, label %merge_232

merge_232:                                        ; preds = %then_231, %merge_230
  br i1 false, label %then_233, label %merge_234

merge_234:                                        ; preds = %then_233, %merge_232
  br i1 false, label %then_235, label %merge_236

merge_236:                                        ; preds = %then_235, %merge_234
  br i1 false, label %then_237, label %merge_238

merge_238:                                        ; preds = %then_237, %merge_236
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.34 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.40 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.41 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  br i1 true, label %then_239, label %merge_240

merge_240:                                        ; preds = %then_239, %merge_238
  br i1 true, label %then_241, label %merge_242

merge_242:                                        ; preds = %then_241, %merge_240
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.50 to i64))
  ret i64 0

then_227:                                         ; preds = %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  ret i64 0

then_229:                                         ; preds = %merge_228
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  br label %merge_230

then_231:                                         ; preds = %merge_230
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  br label %merge_232

then_233:                                         ; preds = %merge_232
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  br label %merge_234

then_235:                                         ; preds = %merge_234
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  br label %merge_236

then_237:                                         ; preds = %merge_236
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  br label %merge_238

then_239:                                         ; preds = %merge_238
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.46 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  br label %merge_240

then_241:                                         ; preds = %merge_240
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.47 to i64))
  br label %merge_242
}

define i64 @"th_\D9\85\D9\84\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D9\8A_cpu"() {
entry:
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  store i64 0, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load", 60000
  br i1 %cmplttmp, label %then_243, label %merge_244

merge_244:                                        ; preds = %entry
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load1" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp2 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load1", 70000
  br i1 %cmplttmp2, label %then_245, label %merge_246

merge_246:                                        ; preds = %merge_244
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load3" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp4 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load3", 80000
  br i1 %cmplttmp4, label %then_247, label %merge_248

merge_248:                                        ; preds = %merge_246
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load5" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp6 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load5", 85000
  br i1 %cmplttmp6, label %then_249, label %merge_250

merge_250:                                        ; preds = %merge_248
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load7" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp8 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load7", 90000
  br i1 %cmplttmp8, label %then_251, label %merge_252

merge_252:                                        ; preds = %merge_250
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load9" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp10 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load9", 95000
  br i1 %cmplttmp10, label %then_253, label %merge_254

merge_254:                                        ; preds = %merge_252
  ret i64 10

then_243:                                         ; preds = %entry
  ret i64 100

then_245:                                         ; preds = %merge_244
  ret i64 90

then_247:                                         ; preds = %merge_246
  ret i64 75

then_249:                                         ; preds = %merge_248
  ret i64 60

then_251:                                         ; preds = %merge_250
  ret i64 40

then_253:                                         ; preds = %merge_252
  ret i64 25
}

define i64 @"th_\D9\85\D9\84\D9\81_\D8\AD\D8\B1\D8\A7\D8\B1\D9\8A_gpu"() {
entry:
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  store i64 0, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load", 65000
  br i1 %cmplttmp, label %then_255, label %merge_256

merge_256:                                        ; preds = %entry
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load1" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp2 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load1", 75000
  br i1 %cmplttmp2, label %then_257, label %merge_258

merge_258:                                        ; preds = %merge_256
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load3" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp4 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load3", 85000
  br i1 %cmplttmp4, label %then_259, label %merge_260

merge_260:                                        ; preds = %merge_258
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load5" = load i64, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  %cmplttmp6 = icmp slt i64 %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9.load5", 95000
  br i1 %cmplttmp6, label %then_261, label %merge_262

merge_262:                                        ; preds = %merge_260
  ret i64 15

then_255:                                         ; preds = %entry
  ret i64 100

then_257:                                         ; preds = %merge_256
  ret i64 85

then_259:                                         ; preds = %merge_258
  ret i64 65

then_261:                                         ; preds = %merge_260
  ret i64 40
}

define i64 @"th_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\AD\D8\B1\D8\A7\D8\B1\D9\8A"() {
entry:
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.51 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  br i1 false, label %then_263, label %merge_264

merge_264:                                        ; preds = %then_263, %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.55 to i64))
  br i1 false, label %then_265, label %merge_266

merge_266:                                        ; preds = %then_265, %merge_264
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.58 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.60 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.62 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.63 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.64 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.65 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.63 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.64 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.68 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.70 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.72 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.73 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.75 to i64))
  ret i64 0

then_263:                                         ; preds = %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  br label %merge_264

then_265:                                         ; preds = %merge_264
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.53 to i64))
  br label %merge_266
}

define i64 @"th_\D9\87\D9\8A\D8\A6"() {
entry:
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.77 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.78 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.77 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.80 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.81 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.82 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.83 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.84 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.85 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.86 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.88 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.90 to i64))
  br i1 false, label %then_267, label %merge_268

merge_268:                                        ; preds = %then_267, %entry
  br i1 true, label %then_269, label %merge_270

merge_270:                                        ; preds = %then_269, %merge_268
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.96 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.97 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.98 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.99 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.98 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.101 to i64))
  ret i64 1

then_267:                                         ; preds = %entry
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.91 to i64))
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  %calltmp = call i64 @"th_serial_\D8\B1\D9\82\D9\85"(i64 0)
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.93 to i64))
  br label %merge_268

then_269:                                         ; preds = %merge_268
  call void @"th_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.94 to i64))
  br label %merge_270
}

define i64 @"th_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  %"%pwm_1" = alloca i64, align 8
  %"%pwm_0" = alloca i64, align 8
  br i1 false, label %then_271, label %merge_272

merge_272:                                        ; preds = %then_271, %entry
  br i1 true, label %then_273, label %merge_274

merge_274:                                        ; preds = %then_273, %merge_272
  br i1 false, label %then_275, label %merge_276

merge_276:                                        ; preds = %then_275, %merge_274
  br i1 true, label %then_277, label %merge_278

merge_278:                                        ; preds = %then_277, %merge_276
  br i1 false, label %then_279, label %merge_280

merge_280:                                        ; preds = %then_279, %merge_278
  %calltmp2 = call i64 @"bat_\D8\AD\D8\AF\D8\AB"()
  ret i64 0

then_271:                                         ; preds = %entry
  br label %merge_272

then_273:                                         ; preds = %merge_272
  store i64 0, ptr %"%pwm_0", align 8
  %"%pwm_0.load" = load i64, ptr %"%pwm_0", align 8
  %calltmp = call i64 @"th_\D8\AD\D8\AF\D8\AB_\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0"(i64 %"%pwm_0.load")
  br label %merge_274

then_275:                                         ; preds = %merge_274
  br label %merge_276

then_277:                                         ; preds = %merge_276
  store i64 0, ptr %"%pwm_1", align 8
  %"%pwm_1.load" = load i64, ptr %"%pwm_1", align 8
  %calltmp1 = call i64 @"th_\D8\AD\D8\AF\D8\AB_\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1"(i64 %"%pwm_1.load")
  br label %merge_278

then_279:                                         ; preds = %merge_278
  br label %merge_280
}

define void @__sad_main() {
entry:
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\88\D8\B3\D8\B7\D9\8A\D8\A9_cnt" = alloca i64, align 8
  %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\88\D8\B3\D8\B7\D9\8A\D8\A9_sum" = alloca i64, align 8
  %"%\D9\88\D9\82\D8\AA_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82_\D9\83\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA_\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\85\D8\B3\D8\AC\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\88\D9\82\D8\AA_\D8\A2\D8\AE\D8\B1_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B4\D8\AD\D9\86_\D8\B0\D9\83\D9\8A_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\B4\D8\AD\D9\86_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\B4\D8\AD\D9\86_\D8\A3\D8\AF\D9\86\D9\89" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\B4\D8\AD\D9\86_\D8\A3\D8\B9\D9\84\D9\89" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\88\D9\82\D8\AA_\D8\B4\D8\AD\D9\86" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\88\D9\82\D8\AA_\D9\85\D8\AA\D8\A8\D9\82\D9\8A" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B5\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AF\D9\88\D8\B1\D8\A7\D8\AA_\D8\B4\D8\AD\D9\86" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B7\D8\A7\D9\82\D8\A9_mw" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AA\D9\8A\D8\A7\D8\B1_ma" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AC\D9\87\D8\AF_mv" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B3\D8\B9\D8\A9_\D8\AA\D8\B5\D9\85\D9\8A\D9\85" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B3\D8\B9\D8\A9_\D9\83\D8\A7\D9\85\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B4\D8\AD\D9\86_mwh" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B4\D8\AD\D9\86_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AD\D8\A7\D9\84" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\85\D9\88\D8\AC\D9\88\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\B1\D8\A7\D9\88\D8\AD_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\85\D9\81\D8\B9\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\85\D9\86\D8\B7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\86\D8\B3\D8\A8\D8\A9_pwm" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9_\D8\AF\D9\86\D9\8A\D8\A7" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\82\D8\B5\D9\88\D9\89" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\87\D8\AF\D9\81" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\88\D8\B6\D8\B9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\85\D9\81\D8\B9\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\85\D9\86\D8\B7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\86\D8\B3\D8\A8\D8\A9_pwm" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9_\D8\AF\D9\86\D9\8A\D8\A7" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\82\D8\B5\D9\88\D9\89" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\87\D8\AF\D9\81" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\88\D8\B6\D8\B9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D9\85\D8\B5\D8\AF\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D9\85\D8\AE\D8\AA\D9\86\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D9\85\D8\B5\D8\AF\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D9\85\D8\AE\D8\AA\D9\86\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D9\85\D8\B5\D8\AF\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D9\85\D8\AE\D8\AA\D9\86\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D9\85\D8\B5\D8\AF\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D9\85\D9\81\D8\B9\D9\84" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D9\85\D8\AE\D8\AA\D9\86\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%EC_WRITE_CMD" = alloca i64, align 8
  %"%EC_READ_CMD" = alloca i64, align 8
  %"%EC_DATA" = alloca i64, align 8
  %"%EC_SC" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\B1\D8\A7\D9\88\D8\AD" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D9\86\D8\A7\D8\B7\D9\82" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\BA\D8\A7\D8\A6\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AD\D8\B1\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\85\D9\85\D8\AA\D9\84\D8\A6\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AA\D9\81\D8\B1\D8\BA" = alloca i64, align 8
  %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AA\D8\B4\D8\AD\D9\86" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\82\D8\B5\D9\88\D9\89" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D8\B9\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\85\D8\AA\D9\88\D8\B3\D8\B7\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\85\D8\B7\D9\81\D8\A3\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D8\AD\D8\AF_\D8\B9\D8\A7\D8\AF\D9\8A" = alloca i64, align 8
  %"%\D9\85\D8\B5\D8\AF\D8\B1_ACPI" = alloca i64, align 8
  %"%\D9\85\D8\B5\D8\AF\D8\B1_EC" = alloca i64, align 8
  %"%\D9\85\D8\B5\D8\AF\D8\B1_MSR" = alloca i64, align 8
  %"%\D9\85\D8\B5\D8\AF\D8\B1_\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%\D9\85\D8\B5\D8\AF\D8\B1_\D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9", align 8
  store i64 1, ptr %"%\D9\85\D8\B5\D8\AF\D8\B1_MSR", align 8
  store i64 2, ptr %"%\D9\85\D8\B5\D8\AF\D8\B1_EC", align 8
  store i64 3, ptr %"%\D9\85\D8\B5\D8\AF\D8\B1_ACPI", align 8
  store i64 60000, ptr %"%\D8\AD\D8\AF_\D8\B9\D8\A7\D8\AF\D9\8A", align 8
  store i64 75000, ptr %"%\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", align 8
  store i64 85000, ptr %"%\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82", align 8
  store i64 95000, ptr %"%\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC", align 8
  store i64 105000, ptr %"%\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\85\D8\B7\D9\81\D8\A3\D8\A9", align 8
  store i64 1, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\85\D9\86\D8\AE\D9\81\D8\B6\D8\A9", align 8
  store i64 2, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\85\D8\AA\D9\88\D8\B3\D8\B7\D8\A9", align 8
  store i64 3, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D8\B9\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 4, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D9\82\D8\B5\D9\88\D9\89", align 8
  store i64 5, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A", align 8
  store i64 1, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AA\D8\B4\D8\AD\D9\86", align 8
  store i64 2, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AA\D9\81\D8\B1\D8\BA", align 8
  store i64 3, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\85\D9\85\D8\AA\D9\84\D8\A6\D8\A9", align 8
  store i64 4, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AD\D8\B1\D8\AC\D8\A9", align 8
  store i64 0, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\BA\D8\A7\D8\A6\D8\A8\D8\A9", align 8
  store i64 8, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D9\86\D8\A7\D8\B7\D9\82", align 8
  store i64 4, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\B1\D8\A7\D9\88\D8\AD", align 8
  store i64 64, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 102, ptr %"%EC_SC", align 8
  store i64 98, ptr %"%EC_DATA", align 8
  store i64 128, ptr %"%EC_READ_CMD", align 8
  store i64 129, ptr %"%EC_WRITE_CMD", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\A7\D8\B3\D9\85", align 8
  store i64 40000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 40000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  store i64 75000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", align 8
  store i64 85000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82", align 8
  store i64 95000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC", align 8
  store i64 105000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D9\85\D8\AE\D8\AA\D9\86\D9\82", align 8
  store i64 1, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_0_\D9\85\D8\B5\D8\AF\D8\B1", align 8
  store i64 1, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\A7\D8\B3\D9\85", align 8
  store i64 35000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 35000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  store i64 80000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", align 8
  store i64 90000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82", align 8
  store i64 100000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC", align 8
  store i64 110000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D9\85\D8\AE\D8\AA\D9\86\D9\82", align 8
  store i64 1, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_1_\D9\85\D8\B5\D8\AF\D8\B1", align 8
  store i64 2, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\A7\D8\B3\D9\85", align 8
  store i64 30000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 30000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  store i64 70000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", align 8
  store i64 80000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82", align 8
  store i64 90000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC", align 8
  store i64 100000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D9\85\D8\AE\D8\AA\D9\86\D9\82", align 8
  store i64 1, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_2_\D9\85\D8\B5\D8\AF\D8\B1", align 8
  store i64 3, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\A7\D8\B3\D9\85", align 8
  store i64 25000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 25000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  store i64 45000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", align 8
  store i64 50000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82", align 8
  store i64 55000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\AD\D8\B1\D8\AC", align 8
  store i64 60000, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D8\AD\D8\AF_\D8\A5\D9\8A\D9\82\D8\A7\D9\81", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D9\85\D8\AE\D8\AA\D9\86\D9\82", align 8
  store i64 1, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%\D9\85\D9\86\D8\B7\D9\82\D8\A9_3_\D9\85\D8\B5\D8\AF\D8\B1", align 8
  store i64 4, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9", align 8
  store i64 5, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\88\D8\B6\D8\B9", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\87\D8\AF\D9\81", align 8
  store i64 5000, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\82\D8\B5\D9\88\D9\89", align 8
  store i64 800, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D8\B3\D8\B1\D8\B9\D8\A9_\D8\AF\D9\86\D9\8A\D8\A7", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\86\D8\B3\D8\A8\D8\A9_pwm", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\85\D9\86\D8\B7\D9\82\D8\A9", align 8
  store i64 1, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_0_\D9\85\D9\81\D8\B9\D9\84\D8\A9", align 8
  store i64 5, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\88\D8\B6\D8\B9", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\87\D8\AF\D9\81", align 8
  store i64 3000, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9_\D9\82\D8\B5\D9\88\D9\89", align 8
  store i64 600, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D8\B3\D8\B1\D8\B9\D8\A9_\D8\AF\D9\86\D9\8A\D8\A7", align 8
  store i64 0, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\86\D8\B3\D8\A8\D8\A9_pwm", align 8
  store i64 2, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\85\D9\86\D8\B7\D9\82\D8\A9", align 8
  store i64 1, ptr %"%\D9\85\D8\B1\D9\88\D8\AD\D8\A9_1_\D9\85\D9\81\D8\B9\D9\84\D8\A9", align 8
  store i64 2, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\B1\D8\A7\D9\88\D8\AD_\D8\A7\D9\84\D9\81\D8\B9\D9\84\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\85\D9\88\D8\AC\D9\88\D8\AF\D8\A9", align 8
  store i64 3, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AD\D8\A7\D9\84", align 8
  store i64 100, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B4\D8\AD\D9\86_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 50000, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B4\D8\AD\D9\86_mwh", align 8
  store i64 50000, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B3\D8\B9\D8\A9_\D9\83\D8\A7\D9\85\D9\84\D8\A9", align 8
  store i64 55000, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B3\D8\B9\D8\A9_\D8\AA\D8\B5\D9\85\D9\8A\D9\85", align 8
  store i64 11400, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AC\D9\87\D8\AF_mv", align 8
  store i64 0, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AA\D9\8A\D8\A7\D8\B1_ma", align 8
  store i64 0, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B7\D8\A7\D9\82\D8\A9_mw", align 8
  store i64 0, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AF\D9\88\D8\B1\D8\A7\D8\AA_\D8\B4\D8\AD\D9\86", align 8
  store i64 100, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\B5\D8\AD\D8\A9", align 8
  store i64 25000, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D8\AF\D8\B1\D8\AC\D8\A9_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 0, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\88\D9\82\D8\AA_\D9\85\D8\AA\D8\A8\D9\82\D9\8A", align 8
  store i64 0, ptr %"%\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9_\D9\88\D9\82\D8\AA_\D8\B4\D8\AD\D9\86", align 8
  store i64 80, ptr %"%\D8\AD\D8\AF_\D8\B4\D8\AD\D9\86_\D8\A3\D8\B9\D9\84\D9\89", align 8
  store i64 20, ptr %"%\D8\AD\D8\AF_\D8\B4\D8\AD\D9\86_\D8\A3\D8\AF\D9\86\D9\89", align 8
  store i64 5, ptr %"%\D8\AD\D8\AF_\D8\B4\D8\AD\D9\86_\D8\AD\D8\B1\D8\AC", align 8
  store i64 1, ptr %"%\D8\B4\D8\AD\D9\86_\D8\B0\D9\83\D9\8A_\D9\85\D9\81\D8\B9\D9\84", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D8\A2\D8\AE\D8\B1_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\85\D8\B3\D8\AC\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AD\D8\AF\D8\A7\D8\AB_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA_\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1\D8\A7\D8\AA_\D8\A8\D8\B7\D8\A7\D8\B1\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D9\88\D9\82\D8\AA_\D8\A7\D8\AE\D8\AA\D9\86\D8\A7\D9\82_\D9\83\D9\84\D9\8A", align 8
  store i64 0, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\88\D8\B3\D8\B7\D9\8A\D8\A9_sum", align 8
  store i64 0, ptr %"%\D8\AD\D8\B1\D8\A7\D8\B1\D8\A9_\D9\88\D8\B3\D8\B7\D9\8A\D8\A9_cnt", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
