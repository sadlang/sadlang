; ModuleID = '01_simple_class_person'
source_filename = "01_simple_class_person"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [10 x i8] c"    \F0\9F\91\A4 \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [16 x i8] c" | \D8\A7\D9\84\D8\B9\D9\85\D8\B1: \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.4 = private unnamed_addr constant [18 x i8] c" | \D8\A7\D9\84\D9\85\D9\87\D9\86\D8\A9: \00", align 1
@str.const.7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.ret = private unnamed_addr constant [7 x i8] c"\D8\B7\D9\81\D9\84\00", align 1
@str.ret.9 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\A7\D9\87\D9\82\00", align 1
@str.ret.10 = private unnamed_addr constant [7 x i8] c"\D8\B4\D8\A7\D8\A8\00", align 1
@str.ret.11 = private unnamed_addr constant [7 x i8] c"\D9\83\D9\87\D9\84\00", align 1
@str.ret.12 = private unnamed_addr constant [7 x i8] c"\D9\85\D8\B3\D9\86\00", align 1
@str.const.13 = private unnamed_addr constant [20 x i8] c"    \F0\9F\8F\A2 \D9\81\D8\B1\D9\8A\D9\82: \00", align 1
@str.const.16 = private unnamed_addr constant [20 x i8] c" | \D8\A7\D9\84\D8\A3\D8\B9\D8\B6\D8\A7\D8\A1: \00", align 1
@str.const.21 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.25 = private unnamed_addr constant [60 x i8] c"\E2\95\91  \F0\9F\8F\97\EF\B8\8F Phase 17: \D8\B5\D9\86\D9\81 \D8\B4\D8\AE\D8\B5 \D9\88\D9\81\D8\B1\D9\8A\D9\82         \E2\95\91\00", align 1
@str.const.29 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.33 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.37 = private unnamed_addr constant [46 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A5\D9\86\D8\B4\D8\A7\D8\A1 \D8\A7\D9\84\D8\A3\D8\B4\D8\AE\D8\A7\D8\B5 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.41 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.42 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\87\D9\86\D8\AF\D8\B3\00", align 1
@str.const.43 = private unnamed_addr constant [11 x i8] c"\D9\81\D8\A7\D8\B7\D9\85\D8\A9\00", align 1
@str.const.44 = private unnamed_addr constant [11 x i8] c"\D8\B7\D8\A8\D9\8A\D8\A8\D8\A9\00", align 1
@str.const.45 = private unnamed_addr constant [9 x i8] c"\D8\AE\D8\A7\D9\84\D8\AF\00", align 1
@str.const.46 = private unnamed_addr constant [9 x i8] c"\D8\B7\D8\A7\D9\84\D8\A8\00", align 1
@str.const.47 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\B1\D9\8A\D9\85\00", align 1
@str.const.48 = private unnamed_addr constant [13 x i8] c"\D8\A3\D8\B3\D8\AA\D8\A7\D8\B0\D8\A9\00", align 1
@str.const.49 = private unnamed_addr constant [7 x i8] c"\D8\B9\D9\85\D8\B1\00", align 1
@str.const.50 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\AA\D9\82\D8\A7\D8\B9\D8\AF\00", align 1
@str.const.51 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.55 = private unnamed_addr constant [48 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D9\81\D8\A6\D8\A7\D8\AA \D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.59 = private unnamed_addr constant [15 x i8] c"    \D8\A3\D8\AD\D9\85\D8\AF: \00", align 1
@str.const.64 = private unnamed_addr constant [17 x i8] c"    \D9\81\D8\A7\D8\B7\D9\85\D8\A9: \00", align 1
@str.const.69 = private unnamed_addr constant [15 x i8] c"    \D8\AE\D8\A7\D9\84\D8\AF: \00", align 1
@str.const.74 = private unnamed_addr constant [15 x i8] c"    \D9\85\D8\B1\D9\8A\D9\85: \00", align 1
@str.const.79 = private unnamed_addr constant [13 x i8] c"    \D8\B9\D9\85\D8\B1: \00", align 1
@str.const.84 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.88 = private unnamed_addr constant [40 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\A8\D9\84\D9\88\D8\BA \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.92 = private unnamed_addr constant [24 x i8] c"    \D8\A3\D8\AD\D9\85\D8\AF \D8\A8\D8\A7\D9\84\D8\BA: \00", align 1
@str.const.94 = private unnamed_addr constant [11 x i8] c"\D9\86\D8\B9\D9\85 \E2\9C\85\00", align 1
@str.const.98 = private unnamed_addr constant [9 x i8] c"\D9\84\D8\A7 \E2\9D\8C\00", align 1
@str.const.102 = private unnamed_addr constant [24 x i8] c"    \D8\AE\D8\A7\D9\84\D8\AF \D8\A8\D8\A7\D9\84\D8\BA: \00", align 1
@str.const.112 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.116 = private unnamed_addr constant [46 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\B3\D9\86\D9\88\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D9\82\D8\A7\D8\B9\D8\AF \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.123 = private unnamed_addr constant [8 x i8] c" \D8\B3\D9\86\D8\A9\00", align 1
@str.const.130 = private unnamed_addr constant [36 x i8] c" \D8\B3\D9\86\D8\A9 (\D9\85\D8\AA\D9\82\D8\A7\D8\B9\D8\AF \D8\A8\D8\A7\D9\84\D9\81\D8\B9\D9\84)\00", align 1
@str.const.134 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.138 = private unnamed_addr constant [40 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\81\D8\B1\D9\8A\D9\82 \D8\A7\D9\84\D8\B9\D9\85\D9\84 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.142 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\AA\D8\B7\D9\88\D9\8A\D8\B1\00", align 1
@str.const.143 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.147 = private unnamed_addr constant [58 x i8] c"\E2\9C\85 \D8\AA\D9\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\B5\D9\86\D9\81/\D8\A8\D8\A7\D9\86\D9\8A/\D8\AC\D8\AF\D9\8A\D8\AF \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85", i64 %"\D8\B9\D9\85\D8\B1", ptr %"\D9\85\D9\87\D9\86\D8\A9") {
entry:
  %"\D9\85\D9\87\D9\86\D8\A94" = alloca ptr, align 8
  store ptr %"\D9\85\D9\87\D9\86\D8\A9", ptr %"\D9\85\D9\87\D9\86\D8\A94", align 8
  %"\D8\B9\D9\85\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D8\B1", ptr %"\D8\B9\D9\85\D8\B13", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca5 = alloca ptr, align 8
  %alloca6 = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca5, align 8
  %"%\D8\B9\D9\85\D8\B1.load" = load i64, ptr %"\D8\B9\D9\85\D8\B13", align 8
  store i64 %"%\D8\B9\D9\85\D8\B1.load", ptr %alloca, align 8
  store ptr %"\D9\85\D9\87\D9\86\D8\A94", ptr %alloca6, align 8
  ret void
}

define void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load" = load i64, ptr %alloca, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.4)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca3)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret void
}

define i64 @"\D8\B4\D8\AE\D8\B5.\D8\A8\D8\A7\D9\84\D8\BA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load" = load i64, ptr %alloca, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load", 18
  br i1 %cmpgetmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  ret i64 1

merge_1:                                          ; preds = %entry
  ret i64 0
}

define ptr @"\D8\B4\D8\AE\D8\B5.\D8\A7\D9\84\D9\81\D8\A6\D8\A9_\D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load", 13
  br i1 %cmplttmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  ret ptr @str.ret

merge_3:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load4" = load i64, ptr %alloca, align 8
  %cmplttmp5 = icmp slt i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load4", 18
  br i1 %cmplttmp5, label %then_4, label %merge_5

then_4:                                           ; preds = %merge_3
  ret ptr @str.ret.9

merge_5:                                          ; preds = %merge_3
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load6" = load i64, ptr %alloca, align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load6", 30
  br i1 %cmplttmp7, label %then_6, label %merge_7

then_6:                                           ; preds = %merge_5
  ret ptr @str.ret.10

merge_7:                                          ; preds = %merge_5
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load8" = load i64, ptr %alloca, align 8
  %cmplttmp9 = icmp slt i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load8", 50
  br i1 %cmplttmp9, label %then_8, label %merge_9

then_8:                                           ; preds = %merge_7
  ret ptr @str.ret.11

merge_9:                                          ; preds = %merge_7
  ret ptr @str.ret.12
}

define i64 @"\D8\B4\D8\AE\D8\B5.\D8\B3\D9\86\D9\88\D8\A7\D8\AA_\D8\AA\D9\82\D8\A7\D8\B9\D8\AF"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load" = load i64, ptr %alloca, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load", 60
  br i1 %cmpgetmp, label %then_10, label %merge_11

then_10:                                          ; preds = %entry
  ret i64 0

merge_11:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load4" = load i64, ptr %alloca, align 8
  %subtmp = sub i64 60, %"%\D8\A7\D9\84\D8\B9\D9\85\D8\B1.load4"
  ret i64 %subtmp
}

define void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca3 = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca3, align 8
  store i64 0, ptr %alloca, align 8
  ret void
}

define void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\A7\D8\B6\D8\A7\D9\81\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A7\D8\B9\D8\B6\D8\A7\D8\A1.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A7\D8\B9\D8\B6\D8\A7\D8\A1.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  ret void
}

define void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\B9\D8\B1\D8\B6_\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.13)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A7\D8\B9\D8\B6\D8\A7\D8\A1.load" = load i64, ptr %alloca, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A7\D8\B9\D8\B6\D8\A7\D8\A1.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %alloca = alloca i64, align 8
  %"%36.load" = load i64, ptr %alloca, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%36.load", ptr @str.const.41, i64 28, ptr @str.const.42)
  %alloca1 = alloca i64, align 8
  %"%36.load2" = load i64, ptr %alloca, align 8
  store i64 %"%36.load2", ptr %alloca1, align 8
  %alloca3 = alloca i64, align 8
  %"%41.load" = load i64, ptr %alloca3, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%41.load", ptr @str.const.43, i64 35, ptr @str.const.44)
  %alloca4 = alloca i64, align 8
  %"%41.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%41.load5", ptr %alloca4, align 8
  %alloca6 = alloca i64, align 8
  %"%46.load" = load i64, ptr %alloca6, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%46.load", ptr @str.const.45, i64 12, ptr @str.const.46)
  %alloca7 = alloca i64, align 8
  %"%46.load8" = load i64, ptr %alloca6, align 8
  store i64 %"%46.load8", ptr %alloca7, align 8
  %alloca9 = alloca i64, align 8
  %"%51.load" = load i64, ptr %alloca9, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%51.load", ptr @str.const.47, i64 45, ptr @str.const.48)
  %alloca10 = alloca i64, align 8
  %"%51.load11" = load i64, ptr %alloca9, align 8
  store i64 %"%51.load11", ptr %alloca10, align 8
  %alloca12 = alloca i64, align 8
  %"%56.load" = load i64, ptr %alloca12, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%56.load", ptr @str.const.49, i64 62, ptr @str.const.50)
  %alloca13 = alloca i64, align 8
  %"%56.load14" = load i64, ptr %alloca12, align 8
  store i64 %"%56.load14", ptr %alloca13, align 8
  %"%\D8\B41.load" = load i64, ptr %alloca1, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B41.load")
  %"%\D8\B42.load" = load i64, ptr %alloca4, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B42.load")
  %"%\D8\B43.load" = load i64, ptr %alloca7, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B43.load")
  %"%\D8\B44.load" = load i64, ptr %alloca10, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B44.load")
  %"%\D8\B45.load" = load i64, ptr %alloca13, align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B45.load")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %"%\D8\B41.load15" = load i64, ptr %alloca1, align 8
  %calltmp = call ptr @"\D8\B4\D8\AE\D8\B5.\D8\A7\D9\84\D9\81\D8\A6\D8\A9_\D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9"(i64 %"%\D8\B41.load15")
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.64)
  %"%\D8\B42.load16" = load i64, ptr %alloca4, align 8
  %calltmp17 = call ptr @"\D8\B4\D8\AE\D8\B5.\D8\A7\D9\84\D9\81\D8\A6\D8\A9_\D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9"(i64 %"%\D8\B42.load16")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp17)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.69)
  %"%\D8\B43.load18" = load i64, ptr %alloca7, align 8
  %calltmp19 = call ptr @"\D8\B4\D8\AE\D8\B5.\D8\A7\D9\84\D9\81\D8\A6\D8\A9_\D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9"(i64 %"%\D8\B43.load18")
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp19)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %"%\D8\B44.load20" = load i64, ptr %alloca10, align 8
  %calltmp21 = call ptr @"\D8\B4\D8\AE\D8\B5.\D8\A7\D9\84\D9\81\D8\A6\D8\A9_\D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9"(i64 %"%\D8\B44.load20")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp21)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %"%\D8\B45.load22" = load i64, ptr %alloca13, align 8
  %calltmp23 = call ptr @"\D8\B4\D8\AE\D8\B5.\D8\A7\D9\84\D9\81\D8\A6\D8\A9_\D8\A7\D9\84\D8\B9\D9\85\D8\B1\D9\8A\D8\A9"(i64 %"%\D8\B45.load22")
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp23)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %"%\D8\B41.load24" = load i64, ptr %alloca1, align 8
  %calltmp25 = call i64 @"\D8\B4\D8\AE\D8\B5.\D8\A8\D8\A7\D9\84\D8\BA"(i64 %"%\D8\B41.load24")
  %cmpeqtmp = icmp eq i64 %calltmp25, 1
  br i1 %cmpeqtmp, label %then_12, label %else_14

then_12:                                          ; preds = %entry
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  br label %merge_13

else_14:                                          ; preds = %entry
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  br label %merge_13

merge_13:                                         ; preds = %else_14, %then_12
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %"%\D8\B43.load26" = load i64, ptr %alloca7, align 8
  %calltmp27 = call i64 @"\D8\B4\D8\AE\D8\B5.\D8\A8\D8\A7\D9\84\D8\BA"(i64 %"%\D8\B43.load26")
  %cmpeqtmp28 = icmp eq i64 %calltmp27, 1
  br i1 %cmpeqtmp28, label %then_15, label %else_17

then_15:                                          ; preds = %merge_13
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  br label %merge_16

else_17:                                          ; preds = %merge_13
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  br label %merge_16

merge_16:                                         ; preds = %else_17, %then_15
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.112)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.116)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %"%\D8\B41.load29" = load i64, ptr %alloca1, align 8
  %calltmp30 = call i64 @"\D8\B4\D8\AE\D8\B5.\D8\B3\D9\86\D9\88\D8\A7\D8\AA_\D8\AA\D9\82\D8\A7\D8\B9\D8\AF"(i64 %"%\D8\B41.load29")
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp30)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  %"%\D8\B45.load31" = load i64, ptr %alloca13, align 8
  %calltmp32 = call i64 @"\D8\B4\D8\AE\D8\B5.\D8\B3\D9\86\D9\88\D8\A7\D8\AA_\D8\AA\D9\82\D8\A7\D8\B9\D8\AF"(i64 %"%\D8\B45.load31")
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp32)
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.130)
  %54 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.134)
  %56 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %57 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.138)
  %58 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %alloca33 = alloca i64, align 8
  %"%102.load" = load i64, ptr %alloca33, align 8
  call void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%102.load", ptr @str.const.142)
  %alloca34 = alloca i64, align 8
  %"%102.load35" = load i64, ptr %alloca33, align 8
  store i64 %"%102.load35", ptr %alloca34, align 8
  %"%\D9\81.load" = load i64, ptr %alloca34, align 8
  call void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\A7\D8\B6\D8\A7\D9\81\D8\A9"(i64 %"%\D9\81.load")
  %"%\D9\81.load36" = load i64, ptr %alloca34, align 8
  call void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\A7\D8\B6\D8\A7\D9\81\D8\A9"(i64 %"%\D9\81.load36")
  %"%\D9\81.load37" = load i64, ptr %alloca34, align 8
  call void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\A7\D8\B6\D8\A7\D9\81\D8\A9"(i64 %"%\D9\81.load37")
  %"%\D9\81.load38" = load i64, ptr %alloca34, align 8
  call void @"\D9\81\D8\B1\D9\8A\D9\82.\D8\B9\D8\B1\D8\B6_\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D9\81.load38")
  %59 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %60 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %61 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.147)
  %62 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
