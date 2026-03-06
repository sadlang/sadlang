; ModuleID = 'تسلسلي_نحلة'
source_filename = "\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [2 x i8] c"[\00", align 1
@str.const.1 = private unnamed_addr constant [3 x i8] c"] \00", align 1
@str.const.2 = private unnamed_addr constant [13 x i8] c"[\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1:\00", align 1
@str.const.4 = private unnamed_addr constant [9 x i8] c"[\D8\AE\D8\B7\D8\A3:\00", align 1
@str.const.6 = private unnamed_addr constant [6 x i8] c"[\E2\9C\93 \00", align 1
@str.const.8 = private unnamed_addr constant [7 x i8] c"[TRACE\00", align 1
@str.const.9 = private unnamed_addr constant [7 x i8] c"[DEBUG\00", align 1
@str.const.10 = private unnamed_addr constant [7 x i8] c"[INFO \00", align 1
@str.const.11 = private unnamed_addr constant [7 x i8] c"[WARN \00", align 1
@str.const.12 = private unnamed_addr constant [7 x i8] c"[ERROR\00", align 1
@str.const.13 = private unnamed_addr constant [7 x i8] c"[FATAL\00", align 1
@str.const.15 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.16 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.17 = private unnamed_addr constant [161 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.18 = private unnamed_addr constant [59 x i8] c"\E2\95\91                                                   \E2\95\91\0A\00", align 1
@str.const.19 = private unnamed_addr constant [72 x i8] c"\E2\95\91    \F0\9F\90\9D \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\86\D8\AD\D9\84\D8\A9 \E2\80\94 BeeOS v0.1                   \E2\95\91\0A\00", align 1
@str.const.20 = private unnamed_addr constant [81 x i8] c"\E2\95\91    \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D8\A7\D9\84\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A \D8\AC\D8\A7\D9\87\D8\B2                    \E2\95\91\0A\00", align 1
@str.const.21 = private unnamed_addr constant [59 x i8] c"\E2\95\91    COM1: 115200 baud, 8N1                         \E2\95\91\0A\00", align 1
@str.const.23 = private unnamed_addr constant [161 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.26 = private unnamed_addr constant [170 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\0A\00", align 1
@str.const.27 = private unnamed_addr constant [71 x i8] c"\E2\95\91  \D8\AA\D9\81\D8\B1\D9\8A\D8\BA \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 (Hex Dump)                             \E2\95\91\0A\00", align 1
@str.const.28 = private unnamed_addr constant [22 x i8] c"\E2\95\91  \D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86: \00", align 1
@str.const.29 = private unnamed_addr constant [15 x i8] c"  \D8\A7\D9\84\D8\AD\D8\AC\D9\85: \00", align 1
@str.const.30 = private unnamed_addr constant [10 x i8] c" \D8\A8\D8\A7\D9\8A\D8\AA\00", align 1
@str.const.31 = private unnamed_addr constant [25 x i8] c"                    \E2\95\91\0A\00", align 1
@str.const.32 = private unnamed_addr constant [170 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.33 = private unnamed_addr constant [5 x i8] c"\E2\95\91 \00", align 1
@str.const.35 = private unnamed_addr constant [170 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\0A\00", align 1
@str.const.36 = private unnamed_addr constant [5 x i8] c"\E2\95\91\0A\00", align 1
@str.const.37 = private unnamed_addr constant [4 x i8] c"00 \00", align 1
@str.const.38 = private unnamed_addr constant [4 x i8] c"   \00", align 1
@str.const.39 = private unnamed_addr constant [2 x i8] c" \00", align 1
@str.const.42 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@str.const.43 = private unnamed_addr constant [7 x i8] c"  \E2\94\82 \00", align 1
@str.const.44 = private unnamed_addr constant [5 x i8] c"\E2\94\82 \00", align 1
@str.const.45 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.48 = private unnamed_addr constant [83 x i8] c"\E2\95\91  \F0\9F\93\8A \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\A7\D8\AA\D8\B5\D8\A7\D9\84 \D8\A7\D9\84\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A                    \E2\95\91\0A\00", align 1
@str.const.49 = private unnamed_addr constant [161 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\0A\00", align 1
@str.const.50 = private unnamed_addr constant [18 x i8] c"COM1 \D9\85\D9\8F\D9\87\D9\8A\D9\91\D8\A3\00", align 1
@str.const.51 = private unnamed_addr constant [7 x i8] c"\D9\86\D8\B9\D9\85\00", align 1
@str.const.52 = private unnamed_addr constant [41 x i8] c"  \E2\94\82 \D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA \D9\85\D9\8F\D8\B1\D8\B3\D9\8E\D9\84\D8\A9   \E2\94\82 \00", align 1
@str.const.53 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.54 = private unnamed_addr constant [43 x i8] c"  \E2\94\82 \D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA \D9\85\D9\8F\D8\B3\D8\AA\D9\82\D8\A8\D9\8E\D9\84\D8\A9 \E2\94\82 \00", align 1
@str.const.55 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.56 = private unnamed_addr constant [33 x i8] c"  \E2\94\82 \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1            \E2\94\82 \00", align 1
@str.const.57 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1

declare void @sad_ll_serial_init(i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_serial_read_char(i64)

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

define i64 @"\D9\87\D9\8A\D8\A6_\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", i64 %"\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF") {
entry:
  %"%\D9\86\D8\AC\D8\AD_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1" = alloca i64, align 8
  %"\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF2" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF", ptr %"\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF2", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 0
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  %"%\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF.load" = load i64, ptr %"\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF2", align 8
  %andtmp = and i64 %"%\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF.load", 255
  call void @sad_ll_port_outb(i64 %addtmp, i64 %andtmp)
  %"%\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF.load3" = load i64, ptr %"\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF2", align 8
  %shrtmp = lshr i64 %"%\D9\85\D8\B9\D8\AF\D9\84_\D8\A7\D9\84\D8\A8\D9\88\D8\AF.load3", 8
  call void @sad_ll_port_outb(i64 %addtmp, i64 %shrtmp)
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %calltmp = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\AD\D9\84\D9\82\D8\A9_\D9\85\D8\BA\D9\84\D9\82\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load4")
  store i64 %calltmp, ptr %"%\D9\86\D8\AC\D8\AD_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", align 8
  %"%\D9\86\D8\AC\D8\AD_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load" = load i64, ptr %"%\D9\86\D8\AC\D8\AD_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AC\D8\AD_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load", 0
  br i1 %cmpeqtmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp6 = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5", 0
  call void @sad_ll_port_outb(i64 %addtmp6, i64 0)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load7", 0
  br i1 %cmpeqtmp8, label %then_2, label %merge_3

merge_3:                                          ; preds = %then_2, %merge_1
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load9" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load9", 0
  br i1 %cmpeqtmp10, label %then_4, label %merge_5

merge_5:                                          ; preds = %then_4, %merge_3
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load11", 0
  br i1 %cmpeqtmp12, label %then_6, label %merge_7

merge_7:                                          ; preds = %then_6, %merge_5
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load13" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load13", 0
  br i1 %cmpeqtmp14, label %then_8, label %merge_9

merge_9:                                          ; preds = %then_8, %merge_7
  ret i64 1

then_0:                                           ; preds = %entry
  ret i64 0

then_2:                                           ; preds = %merge_1
  br label %merge_3

then_4:                                           ; preds = %merge_3
  br label %merge_5

then_6:                                           ; preds = %merge_5
  br label %merge_7

then_8:                                           ; preds = %merge_7
  br label %merge_9
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\AD\D9\84\D9\82\D8\A9_\D9\85\D8\BA\D9\84\D9\82\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 0
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  store i64 174, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", align 8
  call void @sad_ll_port_outb(i64 %addtmp, i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load")
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84", align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84", align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load2" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84.load", %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1.load2"
  br i1 %cmpeqtmp, label %then_10, label %merge_11

merge_11:                                         ; preds = %entry
  ret i64 0

then_10:                                          ; preds = %entry
  ret i64 1
}

define i64 @"\D9\87\D9\84_\D8\AC\D8\A7\D9\87\D8\B2_\D9\84\D9\84\D8\A5\D8\B1\D8\B3\D8\A7\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 0
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84", align 8
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84.load" = load i64, ptr %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84", align 8
  %andtmp = and i64 %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84.load", 63
  %cmpgetmp = icmp sge i64 %andtmp, 32
  br i1 %cmpgetmp, label %then_12, label %merge_13

merge_13:                                         ; preds = %entry
  ret i64 0

then_12:                                          ; preds = %entry
  ret i64 1
}

define i64 @"\D9\87\D9\84_\D9\8A\D9\88\D8\AC\D8\AF_\D8\A8\D9\8A\D8\A7\D9\86"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 0
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84", align 8
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84.load" = load i64, ptr %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84", align 8
  %andtmp = and i64 %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D8\B3\D8\AC\D9\84.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_14, label %merge_15

merge_15:                                         ; preds = %entry
  ret i64 0

then_14:                                          ; preds = %entry
  ret i64 1
}

define i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\AC\D8\A7\D9\87\D8\B2\D9\8A\D8\A9_\D8\A5\D8\B1\D8\B3\D8\A7\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  store i64 100000, ptr %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89", align 8
  br label %while_cond_16

merge_20:                                         ; preds = %while_body_17
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load2" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load2", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_16

then_19:                                          ; preds = %while_body_17
  ret i64 1

while_body_17:                                    ; preds = %while_cond_16
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %calltmp = call i64 @"\D9\87\D9\84_\D8\AC\D8\A7\D9\87\D8\B2_\D9\84\D9\84\D8\A5\D8\B1\D8\B3\D8\A7\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 1
  br i1 %cmpeqtmp, label %then_19, label %merge_20

while_cond_16:                                    ; preds = %merge_20, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load"
  br i1 %cmplttmp, label %while_body_17, label %while_exit_18

while_exit_18:                                    ; preds = %while_cond_16
  ret i64 0
}

define i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  store i64 100000, ptr %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89", align 8
  br label %while_cond_21

merge_25:                                         ; preds = %while_body_22
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load2" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load2", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_21

then_24:                                          ; preds = %while_body_22
  ret i64 1

while_body_22:                                    ; preds = %while_cond_21
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %calltmp = call i64 @"\D9\87\D9\84_\D9\8A\D9\88\D8\AC\D8\AF_\D8\A8\D9\8A\D8\A7\D9\86"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load")
  %cmpeqtmp = icmp eq i64 %calltmp, 1
  br i1 %cmpeqtmp, label %then_24, label %merge_25

while_cond_21:                                    ; preds = %merge_25, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load"
  br i1 %cmplttmp, label %while_body_22, label %while_exit_23

while_exit_23:                                    ; preds = %while_cond_21
  ret i64 0
}

define void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %calltmp = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\AC\D8\A7\D9\87\D8\B2\D9\8A\D8\A9_\D8\A5\D8\B1\D8\B3\D8\A7\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load")
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3", 0
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A92", align 8
  call void @sad_ll_port_outb(i64 %addtmp, i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load4", 0
  br i1 %cmpeqtmp, label %then_26, label %merge_27

merge_27:                                         ; preds = %then_26, %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5", 0
  br i1 %cmpeqtmp6, label %then_28, label %merge_29

merge_29:                                         ; preds = %then_28, %merge_27
  ret void

then_26:                                          ; preds = %entry
  br label %merge_27

then_28:                                          ; preds = %merge_27
  br label %merge_29
}

define i64 @"\D8\A7\D8\B3\D8\AA\D9\82\D8\A8\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D8\A8\D9\8A\D8\A7\D9\86" = alloca i64, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %calltmp = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load")
  store i64 %calltmp, ptr %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1", align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load", 0
  br i1 %cmpeqtmp, label %then_30, label %merge_31

merge_31:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load2", 0
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3", 0
  br i1 %cmpeqtmp4, label %then_32, label %merge_33

merge_33:                                         ; preds = %then_32, %merge_31
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5", 0
  br i1 %cmpeqtmp6, label %then_34, label %merge_35

merge_35:                                         ; preds = %then_34, %merge_33
  %loadtmp = load i64, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86", align 8
  ret i64 %loadtmp

then_30:                                          ; preds = %entry
  ret i64 -1

then_32:                                          ; preds = %merge_31
  br label %merge_33

then_34:                                          ; preds = %merge_33
  br label %merge_35
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_36

while_body_37:                                    ; preds = %while_cond_36
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_36

while_cond_36:                                    ; preds = %while_body_37, %entry
  %arr.ptr.int = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %arr.len
  br i1 %cmplttmp, label %while_body_37, label %while_exit_38

while_exit_38:                                    ; preds = %while_cond_36
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_39, label %merge_40

merge_40:                                         ; preds = %entry
  %"%\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load2", 0
  br i1 %cmplttmp, label %then_41, label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 20, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 20, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 20))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem = getelementptr i64, ptr %arr.data4, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep5 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data6 = load ptr, ptr %arr.data.gep5, align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 1
  store i64 0, ptr %arr.elem7, align 8
  %arr.data.gep8 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data9 = load ptr, ptr %arr.data.gep8, align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 2
  store i64 0, ptr %arr.elem10, align 8
  %arr.data.gep11 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data12 = load ptr, ptr %arr.data.gep11, align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 3
  store i64 0, ptr %arr.elem13, align 8
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 4
  store i64 0, ptr %arr.elem16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 5
  store i64 0, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 6
  store i64 0, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 7
  store i64 0, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 8
  store i64 0, ptr %arr.elem28, align 8
  %arr.data.gep29 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data30 = load ptr, ptr %arr.data.gep29, align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 9
  store i64 0, ptr %arr.elem31, align 8
  %arr.data.gep32 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data33 = load ptr, ptr %arr.data.gep32, align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 10
  store i64 0, ptr %arr.elem34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 11
  store i64 0, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 12
  store i64 0, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 13
  store i64 0, ptr %arr.elem43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 14
  store i64 0, ptr %arr.elem46, align 8
  %arr.data.gep47 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data48 = load ptr, ptr %arr.data.gep47, align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 15
  store i64 0, ptr %arr.elem49, align 8
  %arr.data.gep50 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data51 = load ptr, ptr %arr.data.gep50, align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 16
  store i64 0, ptr %arr.elem52, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 17
  store i64 0, ptr %arr.elem55, align 8
  %arr.data.gep56 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data57 = load ptr, ptr %arr.data.gep56, align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 18
  store i64 0, ptr %arr.elem58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 19
  store i64 0, ptr %arr.elem61, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_43

then_39:                                          ; preds = %entry
  ret void

then_41:                                          ; preds = %merge_40
  br label %merge_42

while_body_44:                                    ; preds = %while_cond_43
  %"%\D9\82\D9\8A\D9\85\D8\A9.load63" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %modtmp = srem i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load63", 10
  %addtmp = add i64 %modtmp, 48
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep64 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data65 = load ptr, ptr %arr.data.gep64, align 8
  %arr.elem66 = getelementptr i64, ptr %arr.data65, i64 %"%\D8\B9\D8\AF\D8\AF.load"
  store i64 %addtmp, ptr %arr.elem66, align 8
  %"%\D8\B9\D8\AF\D8\AF.load67" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp68 = add i64 %"%\D8\B9\D8\AF\D8\AF.load67", 1
  store i64 %addtmp68, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_43

while_body_47:                                    ; preds = %while_cond_46
  %"%\D8\B7.load70" = load i64, ptr %"%\D8\B7", align 8
  %subtmp71 = sub i64 %"%\D8\B7.load70", 1
  store i64 %subtmp71, ptr %"%\D8\B7", align 8
  br label %while_cond_46

while_cond_43:                                    ; preds = %while_body_44, %merge_42
  %"%\D9\82\D9\8A\D9\85\D8\A9.load62" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load62", 0
  br i1 %cmpgttmp, label %while_body_44, label %while_exit_45

while_cond_46:                                    ; preds = %while_body_47, %while_exit_45
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_47, label %while_exit_48

while_exit_45:                                    ; preds = %while_cond_43
  %"%\D8\B9\D8\AF\D8\AF.load69" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF.load69", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_46

while_exit_48:                                    ; preds = %while_cond_46
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_hex"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B1" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_49, label %merge_50

else_59:                                          ; preds = %while_body_55
  br label %merge_58

merge_50:                                         ; preds = %entry
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 16, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep2 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data3 = load ptr, ptr %arr.data.gep2, align 8
  %arr.elem = getelementptr i64, ptr %arr.data3, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep4 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data5 = load ptr, ptr %arr.data.gep4, align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 1
  store i64 0, ptr %arr.elem6, align 8
  %arr.data.gep7 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data8 = load ptr, ptr %arr.data.gep7, align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 2
  store i64 0, ptr %arr.elem9, align 8
  %arr.data.gep10 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data11 = load ptr, ptr %arr.data.gep10, align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 3
  store i64 0, ptr %arr.elem12, align 8
  %arr.data.gep13 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data14 = load ptr, ptr %arr.data.gep13, align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 4
  store i64 0, ptr %arr.elem15, align 8
  %arr.data.gep16 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data17 = load ptr, ptr %arr.data.gep16, align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 5
  store i64 0, ptr %arr.elem18, align 8
  %arr.data.gep19 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data20 = load ptr, ptr %arr.data.gep19, align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 6
  store i64 0, ptr %arr.elem21, align 8
  %arr.data.gep22 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data23 = load ptr, ptr %arr.data.gep22, align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 7
  store i64 0, ptr %arr.elem24, align 8
  %arr.data.gep25 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data26 = load ptr, ptr %arr.data.gep25, align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 8
  store i64 0, ptr %arr.elem27, align 8
  %arr.data.gep28 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data29 = load ptr, ptr %arr.data.gep28, align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 9
  store i64 0, ptr %arr.elem30, align 8
  %arr.data.gep31 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data32 = load ptr, ptr %arr.data.gep31, align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 10
  store i64 0, ptr %arr.elem33, align 8
  %arr.data.gep34 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data35 = load ptr, ptr %arr.data.gep34, align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 11
  store i64 0, ptr %arr.elem36, align 8
  %arr.data.gep37 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data38 = load ptr, ptr %arr.data.gep37, align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 12
  store i64 0, ptr %arr.elem39, align 8
  %arr.data.gep40 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data41 = load ptr, ptr %arr.data.gep40, align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 13
  store i64 0, ptr %arr.elem42, align 8
  %arr.data.gep43 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data44 = load ptr, ptr %arr.data.gep43, align 8
  %arr.elem45 = getelementptr i64, ptr %arr.data44, i64 14
  store i64 0, ptr %arr.elem45, align 8
  %arr.data.gep46 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data47 = load ptr, ptr %arr.data.gep46, align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 15
  store i64 0, ptr %arr.elem48, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_51

merge_58:                                         ; preds = %else_59, %then_57
  %"%\D8\B7.load62" = load i64, ptr %"%\D8\B7", align 8
  %subtmp63 = sub i64 %"%\D8\B7.load62", 1
  store i64 %subtmp63, ptr %"%\D8\B7", align 8
  br label %while_cond_54

then_49:                                          ; preds = %entry
  ret void

then_57:                                          ; preds = %while_body_55
  br label %merge_58

while_body_52:                                    ; preds = %while_cond_51
  %"%\D9\82\D9\8A\D9\85\D8\A9.load50" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %andtmp = and i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load50", 15
  store i64 %andtmp, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %"%\D8\A8\D8\A7\D9\82\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\82\D9\8A", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 %"%\D8\B9\D8\AF\D8\AF.load"
  store i64 %"%\D8\A8\D8\A7\D9\82\D9\8A.load", ptr %arr.elem53, align 8
  %"%\D8\B9\D8\AF\D8\AF.load54" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF.load54", 1
  store i64 %addtmp, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %while_cond_51

while_body_55:                                    ; preds = %while_cond_54
  %"%\D8\B7.load56" = load i64, ptr %"%\D8\B7", align 8
  %arr.ptr.int57 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr58 = inttoptr i64 %arr.ptr.int57 to ptr
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr.ptr58, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 %"%\D8\B7.load56"
  %arr.get = load i64, ptr %arr.elem61, align 8
  store i64 %arr.get, ptr %"%\D8\B1", align 8
  %"%\D8\B1.load" = load i64, ptr %"%\D8\B1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1.load", 10
  br i1 %cmplttmp, label %then_57, label %else_59

while_cond_51:                                    ; preds = %while_body_52, %merge_50
  %"%\D9\82\D9\8A\D9\85\D8\A9.load49" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load49", 0
  br i1 %cmpgttmp, label %while_body_52, label %while_exit_53

while_cond_54:                                    ; preds = %merge_58, %while_exit_53
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_55, label %while_exit_56

while_exit_53:                                    ; preds = %while_cond_51
  %"%\D8\B9\D8\AF\D8\AF.load55" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF.load55", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_54

while_exit_56:                                    ; preds = %while_cond_54
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_bin"(i64 %"\D9\82\D9\8A\D9\85\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA") {
entry:
  %"%\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\AC" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA2", align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA2", align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA.load", 1
  store i64 %subtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_60

merge_67:                                         ; preds = %merge_69, %while_exit_65
  %"%\D8\B7.load9" = load i64, ptr %"%\D8\B7", align 8
  %subtmp10 = sub i64 %"%\D8\B7.load9", 1
  store i64 %subtmp10, ptr %"%\D8\B7", align 8
  br label %while_cond_60

merge_69:                                         ; preds = %then_68, %then_66
  br label %merge_67

then_66:                                          ; preds = %while_exit_65
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %andtmp8 = and i64 %"%\D8\B7.load7", 3
  %cmpeqtmp = icmp eq i64 %andtmp8, 0
  br i1 %cmpeqtmp, label %then_68, label %merge_69

then_68:                                          ; preds = %then_66
  br label %merge_69

while_body_61:                                    ; preds = %while_cond_60
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\85", align 8
  store i64 0, ptr %"%\D8\AC", align 8
  br label %while_cond_63

while_body_64:                                    ; preds = %while_cond_63
  %"%\D9\85\D9\82\D8\B3\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85\D9\82\D8\B3\D9\85.load", 1
  store i64 %shltmp, ptr %"%\D9\85\D9\82\D8\B3\D9\85", align 8
  %"%\D8\AC.load4" = load i64, ptr %"%\D8\AC", align 8
  %addtmp = add i64 %"%\D8\AC.load4", 1
  store i64 %addtmp, ptr %"%\D8\AC", align 8
  br label %while_cond_63

while_cond_60:                                    ; preds = %merge_67, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7.load", 0
  br i1 %cmpgetmp, label %while_body_61, label %while_exit_62

while_cond_63:                                    ; preds = %while_body_64, %while_body_61
  %"%\D8\AC.load" = load i64, ptr %"%\D8\AC", align 8
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AC.load", %"%\D8\B7.load3"
  br i1 %cmplttmp, label %while_body_64, label %while_exit_65

while_exit_62:                                    ; preds = %while_cond_60
  ret void

while_exit_65:                                    ; preds = %while_cond_63
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\85\D9\82\D8\B3\D9\85.load5" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\85", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", %"%\D9\85\D9\82\D8\B3\D9\85.load5"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%\D8\A8\D8\AA", align 8
  %"%\D8\B7.load6" = load i64, ptr %"%\D8\B7", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7.load6", 0
  br i1 %cmpgttmp, label %then_66, label %merge_67
}

define void @"serial_\D9\84\D9\88\D9\86"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\86") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\861" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\86", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\861", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\86.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\861", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\84\D9\88\D9\86.load", 10
  br i1 %cmpgetmp, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %entry
  ret void

then_70:                                          ; preds = %entry
  br label %merge_71
}

define void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"() {
entry:
  ret void
}

define void @"serial_\D8\AA\D9\86\D9\82\D9\8A\D8\AD"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void
}

define void @"serial_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.2 to i64))
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void
}

define void @"serial_\D8\AE\D8\B7\D8\A3"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void
}

define void @"serial_\D9\86\D8\AC\D8\A7\D8\AD"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A92", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void
}

define void @"\D8\B9\D9\8A\D9\86_\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\A7\D9\84\D8\B3\D8\AC\D9\84"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  ret void
}

define void @"\D8\B3\D8\AC\D9\84"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", i64 %"\D9\85\D9\83\D9\88\D9\86", i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A93", align 8
  %"\D9\85\D9\83\D9\88\D9\862" = alloca i64, align 8
  store i64 %"\D9\85\D9\83\D9\88\D9\86", ptr %"\D9\85\D9\83\D9\88\D9\862", align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load", 0
  br i1 %cmplttmp, label %then_72, label %merge_73

merge_73:                                         ; preds = %entry
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load4" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load4", 0
  br i1 %cmpeqtmp, label %then_74, label %merge_75

merge_75:                                         ; preds = %then_74, %merge_73
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load5" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load5", 0
  br i1 %cmpeqtmp6, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %merge_75
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load7" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load7", 0
  br i1 %cmpeqtmp8, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load9" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load9", 0
  br i1 %cmpeqtmp10, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %merge_79
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load11" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load11", 0
  br i1 %cmpeqtmp12, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load13" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load13", 0
  br i1 %cmpeqtmp14, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  %"%\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"\D9\85\D9\83\D9\88\D9\862", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D9\85\D9\83\D9\88\D9\86.load")
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A93", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void

then_72:                                          ; preds = %entry
  ret void

then_74:                                          ; preds = %merge_73
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  br label %merge_75

then_76:                                          ; preds = %merge_75
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  br label %merge_77

then_78:                                          ; preds = %merge_77
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  br label %merge_79

then_80:                                          ; preds = %merge_79
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  br label %merge_81

then_82:                                          ; preds = %merge_81
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  br label %merge_83

then_84:                                          ; preds = %merge_83
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  br label %merge_85
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B3\D8\B7\D8\B1_\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", i64 %"\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF" = alloca i64, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9" = alloca i64, align 8
  %"\D9\85\D8\AE\D8\B2\D9\862" = alloca i64, align 8
  store i64 %"\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D9\85\D8\AE\D8\B2\D9\862", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  store i64 0, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  store i64 127, ptr %"%\D8\A7\D9\84\D8\AD\D8\AF", align 8
  br label %while_cond_86

merge_100:                                        ; preds = %merge_102, %merge_95
  br label %while_cond_86

merge_102:                                        ; preds = %then_101, %then_99
  br label %merge_100

merge_90:                                         ; preds = %while_body_87
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3", 0
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D8\AD\D8\B1\D9\81.load", 13
  br i1 %cmpeqtmp4, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_90
  %"%\D8\AD\D8\B1\D9\81.load8" = load i64, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D8\AD\D8\B1\D9\81.load8", 8
  br i1 %cmpeqtmp9, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  %"%\D8\AD\D8\B1\D9\81.load11" = load i64, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AD\D8\B1\D9\81.load11", 32
  br i1 %cmpgetmp, label %then_99, label %merge_100

merge_97:                                         ; preds = %then_96, %then_94
  br label %while_cond_86

then_101:                                         ; preds = %then_99
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load18" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\AD\D8\B1\D9\81.load19" = load i64, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  call void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load18", i64 %"%\D8\AD\D8\B1\D9\81.load19")
  %"%\D9\85\D9\88\D9\82\D8\B9.load20" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %addtmp21 = add i64 %"%\D9\85\D9\88\D9\82\D8\B9.load20", 1
  store i64 %addtmp21, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  br label %merge_102

then_89:                                          ; preds = %while_body_87
  br label %while_cond_86

then_92:                                          ; preds = %merge_90
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  call void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load5", i64 13)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  call void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load6", i64 10)
  %loadtmp7 = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  ret i64 %loadtmp7

then_94:                                          ; preds = %merge_93
  %"%\D9\85\D9\88\D9\82\D8\B9.load10" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D9\88\D9\82\D8\B9.load10", 0
  br i1 %cmpgttmp, label %then_96, label %merge_97

then_96:                                          ; preds = %then_94
  %"%\D9\85\D9\88\D9\82\D8\B9.load12" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %subtmp = sub i64 %"%\D9\85\D9\88\D9\82\D8\B9.load12", 1
  store i64 %subtmp, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load13" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  call void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load13", i64 8)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  call void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load14", i64 32)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load15" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  call void @"\D8\A3\D8\B1\D8\B3\D9\84_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load15", i64 8)
  br label %merge_97

then_99:                                          ; preds = %merge_95
  %"%\D8\AD\D8\B1\D9\81.load16" = load i64, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %cmplttmp17 = icmp slt i64 %"%\D8\AD\D8\B1\D9\81.load16", 127
  br i1 %cmplttmp17, label %then_101, label %merge_102

while_body_87:                                    ; preds = %while_cond_86
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %calltmp = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load")
  store i64 %calltmp, ptr %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1", align 8
  %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load" = load i64, ptr %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\AA\D9\88\D9\81\D8\B1.load", 0
  br i1 %cmpeqtmp, label %then_89, label %merge_90

while_cond_86:                                    ; preds = %merge_100, %merge_97, %then_89, %entry
  %"%\D9\85\D9\88\D9\82\D8\B9.load" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\AF", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D9\88\D9\82\D8\B9.load", %"%\D8\A7\D9\84\D8\AD\D8\AF.load"
  br i1 %cmplttmp, label %while_body_87, label %while_exit_88

while_exit_88:                                    ; preds = %while_cond_86
  %loadtmp = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  ret i64 %loadtmp
}

define i64 @"\D9\87\D9\8A\D8\A6_\D9\86\D8\B8\D8\A7\D9\85_\D8\A7\D9\84\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_103, label %else_105

else_105:                                         ; preds = %entry
  br label %merge_104

merge_104:                                        ; preds = %else_105, %then_103
  %loadtmp = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  ret i64 %loadtmp

then_103:                                         ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  br label %merge_104
}

define void @"serial_\D8\AA\D9\81\D8\B1\D9\8A\D8\BA_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85") {
entry:
  %"%\D8\AC" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85", ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.27 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.28 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_hex"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.29 to i64))
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.30 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.31 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.32 to i64))
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_106

else_114:                                         ; preds = %while_body_110
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.38 to i64))
  br label %merge_113

merge_113:                                        ; preds = %else_114, %then_112
  %"%\D8\AC.load14" = load i64, ptr %"%\D8\AC", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AC.load14", 7
  br i1 %cmpeqtmp, label %then_115, label %merge_116

merge_116:                                        ; preds = %then_115, %merge_113
  %"%\D8\AC.load15" = load i64, ptr %"%\D8\AC", align 8
  %addtmp16 = add i64 %"%\D8\AC.load15", 1
  store i64 %addtmp16, ptr %"%\D8\AC", align 8
  br label %while_cond_109

then_112:                                         ; preds = %while_body_110
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.37 to i64))
  br label %merge_113

then_115:                                         ; preds = %merge_113
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.39 to i64))
  br label %merge_116

while_body_107:                                   ; preds = %while_cond_106
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.33 to i64))
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load4" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  %"%\D8\B7.load5" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load4", %"%\D8\B7.load5"
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_hex"(i64 %addtmp)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  store i64 0, ptr %"%\D8\AC", align 8
  br label %while_cond_109

while_body_110:                                   ; preds = %while_cond_109
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %"%\D8\AC.load8" = load i64, ptr %"%\D8\AC", align 8
  %addtmp9 = add i64 %"%\D8\B7.load7", %"%\D8\AC.load8"
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load10" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %cmplttmp11 = icmp slt i64 %addtmp9, %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load10"
  br i1 %cmplttmp11, label %then_112, label %else_114

while_cond_106:                                   ; preds = %while_exit_111, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load3" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\852", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load3"
  br i1 %cmplttmp, label %while_body_107, label %while_exit_108

while_cond_109:                                   ; preds = %merge_116, %while_body_107
  %"%\D8\AC.load" = load i64, ptr %"%\D8\AC", align 8
  %cmplttmp6 = icmp slt i64 %"%\D8\AC.load", 16
  br i1 %cmplttmp6, label %while_body_110, label %while_exit_111

while_exit_108:                                   ; preds = %while_cond_106
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.35 to i64))
  ret void

while_exit_111:                                   ; preds = %while_cond_109
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.36 to i64))
  %"%\D8\B7.load12" = load i64, ptr %"%\D8\B7", align 8
  %addtmp13 = add i64 %"%\D8\B7.load12", 16
  store i64 %addtmp13, ptr %"%\D8\B7", align 8
  br label %while_cond_106
}

define void @"serial_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\AA\D9\82\D8\AF\D9\85"(i64 %"\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A9", i64 %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D9\85\D9\84\D9\88\D8\A1" = alloca i64, align 8
  %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B62" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6", ptr %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B62", align 8
  %"\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A9", ptr %"\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A91", align 8
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B62", align 8
  %multmp = mul i64 %"%\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A9.load", %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load"
  %divtmp = sdiv i64 %multmp, 100
  store i64 %divtmp, ptr %"%\D8\A7\D9\84\D9\85\D9\85\D9\84\D9\88\D8\A1", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_117

else_122:                                         ; preds = %while_body_118
  br label %merge_121

merge_121:                                        ; preds = %else_122, %then_120
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load7", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_117

then_120:                                         ; preds = %while_body_118
  br label %merge_121

while_body_118:                                   ; preds = %while_cond_117
  %"%\D8\B7.load4" = load i64, ptr %"%\D8\B7", align 8
  %"%\D8\A7\D9\84\D9\85\D9\85\D9\84\D9\88\D8\A1.load" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D9\85\D9\84\D9\88\D8\A1", align 8
  %cmplttmp5 = icmp slt i64 %"%\D8\B7.load4", %"%\D8\A7\D9\84\D9\85\D9\85\D9\84\D9\88\D8\A1.load"
  br i1 %cmplttmp5, label %then_120, label %else_122

while_cond_117:                                   ; preds = %merge_121, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load3" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B62", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load3"
  br i1 %cmplttmp, label %while_body_118, label %while_exit_119

while_exit_119:                                   ; preds = %while_cond_117
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %"%\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A9.load6" = load i64, ptr %"\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A7\D9\84\D9\86\D8\B3\D8\A8\D8\A9.load6")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.42 to i64))
  ret void
}

define void @"serial_\D8\B5\D9\81_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF1", i64 %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF2") {
entry:
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5" = alloca i64, align 8
  %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF22" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF2", ptr %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF22", align 8
  %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF11" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF1", ptr %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF11", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.43 to i64))
  %"%\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF1.load" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF11", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF1.load")
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  %arr.ptr.int = load i64, ptr %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF11", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  store i64 %arr.len, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5", align 8
  store i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5.load", ptr %"%\D8\B7", align 8
  br label %while_cond_123

while_body_124:                                   ; preds = %while_cond_123
  %"%\D8\B7.load3" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load3", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_123

while_cond_123:                                   ; preds = %while_body_124, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 20
  br i1 %cmplttmp, label %while_body_124, label %while_exit_125

while_exit_125:                                   ; preds = %while_cond_123
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.44 to i64))
  %"%\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF2.load" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF22", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D9\88\D8\AF2.load")
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 ptrtoint (ptr @str.const.45 to i64))
  ret void
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D8\AA\D8\B3\D9\84\D8\B3\D9\84\D9\8A"() {
entry:
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.48 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.49 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  call void @"serial_\D8\B5\D9\81_\D8\AC\D8\AF\D9\88\D9\84"(i64 ptrtoint (ptr @str.const.50 to i64), i64 ptrtoint (ptr @str.const.51 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.52 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 ptrtoint (ptr @str.const.53 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.54 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 ptrtoint (ptr @str.const.55 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.56 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 ptrtoint (ptr @str.const.57 to i64))
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.23 to i64))
  call void @"serial_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D9\84\D9\88\D9\86"()
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\B3\D8\AC\D9\84_\D8\AD\D8\B1\D8\AC" = alloca i64, align 8
  %"%\D8\B3\D8\AC\D9\84_\D8\AE\D8\B7\D8\A3" = alloca i64, align 8
  %"%\D8\B3\D8\AC\D9\84_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D8\B3\D8\AC\D9\84_\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\B3\D8\AC\D9\84_\D8\AA\D9\86\D9\82\D9\8A\D8\AD" = alloca i64, align 8
  %"%\D8\B3\D8\AC\D9\84_\D8\AA\D8\AA\D8\A8\D8\B9" = alloca i64, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\A7\D9\84\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\A3\D8\AF\D9\86\D9\89" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9_COM2" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B1\D8\B3\D9\84\D8\A9_COM2" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_COM1" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9_COM1" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B1\D8\B3\D9\84\D8\A9_COM1" = alloca i64, align 8
  %"%COM4_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%COM3_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%COM2_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%COM1_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%ANSI_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9" = alloca i64, align 8
  %"%ANSI_\D8\A3\D8\A8\D9\8A\D8\B6" = alloca i64, align 8
  %"%ANSI_\D8\B3\D9\85\D8\A7\D9\88\D9\8A" = alloca i64, align 8
  %"%ANSI_\D8\A8\D9\86\D9\81\D8\B3\D8\AC\D9\8A" = alloca i64, align 8
  %"%ANSI_\D8\A3\D8\B2\D8\B1\D9\82" = alloca i64, align 8
  %"%ANSI_\D8\A3\D8\B5\D9\81\D8\B1" = alloca i64, align 8
  %"%ANSI_\D8\A3\D8\AE\D8\B6\D8\B1" = alloca i64, align 8
  %"%ANSI_\D8\A3\D8\AD\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_1200" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_2400" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_4800" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_9600" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_19200" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_38400" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_57600" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\85_115200" = alloca i64, align 8
  %"%FCR_TRIGGER_14" = alloca i64, align 8
  %"%FCR_TRIGGER_8" = alloca i64, align 8
  %"%FCR_TRIGGER_4" = alloca i64, align 8
  %"%FCR_TRIGGER_1" = alloca i64, align 8
  %"%FCR_CLR_SEND" = alloca i64, align 8
  %"%FCR_CLR_RECV" = alloca i64, align 8
  %"%FCR_ENABLE" = alloca i64, align 8
  %"%MCR_LOOPBACK" = alloca i64, align 8
  %"%MCR_OUT2" = alloca i64, align 8
  %"%MCR_OUT1" = alloca i64, align 8
  %"%MCR_RTS" = alloca i64, align 8
  %"%MCR_DTR" = alloca i64, align 8
  %"%LCR_DLAB" = alloca i64, align 8
  %"%LCR_NO_PARITY" = alloca i64, align 8
  %"%LCR_2STOP" = alloca i64, align 8
  %"%LCR_1STOP" = alloca i64, align 8
  %"%LCR_8BITS" = alloca i64, align 8
  %"%LCR_7BITS" = alloca i64, align 8
  %"%LCR_6BITS" = alloca i64, align 8
  %"%LCR_5BITS" = alloca i64, align 8
  %"%IER_MODEM_STATUS" = alloca i64, align 8
  %"%IER_LINE_STATUS" = alloca i64, align 8
  %"%IER_THR_EMPTY" = alloca i64, align 8
  %"%IER_DATA_AVAIL" = alloca i64, align 8
  %"%LSR_FIFO_ERR" = alloca i64, align 8
  %"%LSR_TX_IDLE" = alloca i64, align 8
  %"%LSR_THR_EMPTY" = alloca i64, align 8
  %"%LSR_BREAK_INT" = alloca i64, align 8
  %"%LSR_FRAMING_ERR" = alloca i64, align 8
  %"%LSR_PARITY_ERR" = alloca i64, align 8
  %"%LSR_OVERRUN_ERR" = alloca i64, align 8
  %"%LSR_DATA_READY" = alloca i64, align 8
  %"%UART_SR" = alloca i64, align 8
  %"%UART_MSR" = alloca i64, align 8
  %"%UART_LSR" = alloca i64, align 8
  %"%UART_MCR" = alloca i64, align 8
  %"%UART_LCR" = alloca i64, align 8
  %"%UART_FCR" = alloca i64, align 8
  %"%UART_IIR" = alloca i64, align 8
  %"%UART_DLH" = alloca i64, align 8
  %"%UART_IER" = alloca i64, align 8
  %"%UART_DLL" = alloca i64, align 8
  %"%UART_RBR" = alloca i64, align 8
  %"%UART_THR" = alloca i64, align 8
  %"%COM4" = alloca i64, align 8
  %"%COM3" = alloca i64, align 8
  %"%COM2" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 760, ptr %"%COM2", align 8
  store i64 1000, ptr %"%COM3", align 8
  store i64 744, ptr %"%COM4", align 8
  store i64 0, ptr %"%UART_THR", align 8
  store i64 0, ptr %"%UART_RBR", align 8
  store i64 0, ptr %"%UART_DLL", align 8
  store i64 1, ptr %"%UART_IER", align 8
  store i64 1, ptr %"%UART_DLH", align 8
  store i64 2, ptr %"%UART_IIR", align 8
  store i64 2, ptr %"%UART_FCR", align 8
  store i64 3, ptr %"%UART_LCR", align 8
  store i64 4, ptr %"%UART_MCR", align 8
  store i64 5, ptr %"%UART_LSR", align 8
  store i64 6, ptr %"%UART_MSR", align 8
  store i64 7, ptr %"%UART_SR", align 8
  store i64 1, ptr %"%LSR_DATA_READY", align 8
  store i64 2, ptr %"%LSR_OVERRUN_ERR", align 8
  store i64 4, ptr %"%LSR_PARITY_ERR", align 8
  store i64 8, ptr %"%LSR_FRAMING_ERR", align 8
  store i64 16, ptr %"%LSR_BREAK_INT", align 8
  store i64 32, ptr %"%LSR_THR_EMPTY", align 8
  store i64 64, ptr %"%LSR_TX_IDLE", align 8
  store i64 128, ptr %"%LSR_FIFO_ERR", align 8
  store i64 1, ptr %"%IER_DATA_AVAIL", align 8
  store i64 2, ptr %"%IER_THR_EMPTY", align 8
  store i64 4, ptr %"%IER_LINE_STATUS", align 8
  store i64 8, ptr %"%IER_MODEM_STATUS", align 8
  store i64 0, ptr %"%LCR_5BITS", align 8
  store i64 1, ptr %"%LCR_6BITS", align 8
  store i64 2, ptr %"%LCR_7BITS", align 8
  store i64 3, ptr %"%LCR_8BITS", align 8
  store i64 0, ptr %"%LCR_1STOP", align 8
  store i64 4, ptr %"%LCR_2STOP", align 8
  store i64 0, ptr %"%LCR_NO_PARITY", align 8
  store i64 128, ptr %"%LCR_DLAB", align 8
  store i64 1, ptr %"%MCR_DTR", align 8
  store i64 2, ptr %"%MCR_RTS", align 8
  store i64 4, ptr %"%MCR_OUT1", align 8
  store i64 8, ptr %"%MCR_OUT2", align 8
  store i64 16, ptr %"%MCR_LOOPBACK", align 8
  store i64 1, ptr %"%FCR_ENABLE", align 8
  store i64 2, ptr %"%FCR_CLR_RECV", align 8
  store i64 4, ptr %"%FCR_CLR_SEND", align 8
  store i64 0, ptr %"%FCR_TRIGGER_1", align 8
  store i64 64, ptr %"%FCR_TRIGGER_4", align 8
  store i64 128, ptr %"%FCR_TRIGGER_8", align 8
  store i64 192, ptr %"%FCR_TRIGGER_14", align 8
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\85_115200", align 8
  store i64 2, ptr %"%\D9\85\D9\82\D8\B3\D9\85_57600", align 8
  store i64 3, ptr %"%\D9\85\D9\82\D8\B3\D9\85_38400", align 8
  store i64 6, ptr %"%\D9\85\D9\82\D8\B3\D9\85_19200", align 8
  store i64 12, ptr %"%\D9\85\D9\82\D8\B3\D9\85_9600", align 8
  store i64 24, ptr %"%\D9\85\D9\82\D8\B3\D9\85_4800", align 8
  store i64 48, ptr %"%\D9\85\D9\82\D8\B3\D9\85_2400", align 8
  store i64 96, ptr %"%\D9\85\D9\82\D8\B3\D9\85_1200", align 8
  store i64 31, ptr %"%ANSI_\D8\A3\D8\AD\D9\85\D8\B1", align 8
  store i64 32, ptr %"%ANSI_\D8\A3\D8\AE\D8\B6\D8\B1", align 8
  store i64 33, ptr %"%ANSI_\D8\A3\D8\B5\D9\81\D8\B1", align 8
  store i64 34, ptr %"%ANSI_\D8\A3\D8\B2\D8\B1\D9\82", align 8
  store i64 35, ptr %"%ANSI_\D8\A8\D9\86\D9\81\D8\B3\D8\AC\D9\8A", align 8
  store i64 36, ptr %"%ANSI_\D8\B3\D9\85\D8\A7\D9\88\D9\8A", align 8
  store i64 37, ptr %"%ANSI_\D8\A3\D8\A8\D9\8A\D8\B6", align 8
  store i64 0, ptr %"%ANSI_\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9", align 8
  store i64 0, ptr %"%COM1_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 0, ptr %"%COM2_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 0, ptr %"%COM3_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 0, ptr %"%COM4_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B1\D8\B3\D9\84\D8\A9_COM1", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9_COM1", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_COM1", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B1\D8\B3\D9\84\D8\A9_COM2", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA_\D9\85\D8\B3\D8\AA\D9\82\D8\A8\D9\84\D8\A9_COM2", align 8
  store i64 0, ptr %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89_\D8\A7\D9\84\D8\B3\D8\AC\D9\84_\D8\A7\D9\84\D8\A3\D8\AF\D9\86\D9\89", align 8
  store i64 0, ptr %"%\D8\B3\D8\AC\D9\84_\D8\AA\D8\AA\D8\A8\D8\B9", align 8
  store i64 1, ptr %"%\D8\B3\D8\AC\D9\84_\D8\AA\D9\86\D9\82\D9\8A\D8\AD", align 8
  store i64 2, ptr %"%\D8\B3\D8\AC\D9\84_\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A9", align 8
  store i64 3, ptr %"%\D8\B3\D8\AC\D9\84_\D8\AA\D8\AD\D8\B0\D9\8A\D8\B1", align 8
  store i64 4, ptr %"%\D8\B3\D8\AC\D9\84_\D8\AE\D8\B7\D8\A3", align 8
  store i64 5, ptr %"%\D8\B3\D8\AC\D9\84_\D8\AD\D8\B1\D8\AC", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
