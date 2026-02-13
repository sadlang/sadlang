; ModuleID = '04_medium_library'
source_filename = "04_medium_library"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [10 x i8] c"    \F0\9F\93\96 \00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.2 = private unnamed_addr constant [4 x i8] c" - \00", align 1
@str.const.5 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.7 = private unnamed_addr constant [10 x i8] c" \D8\B5) \F0\9F\92\B0\00", align 1
@str.const.10 = private unnamed_addr constant [16 x i8] c" [\D9\85\D8\AA\D8\A7\D8\AD \E2\9C\85]\00", align 1
@str.const.12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.14 = private unnamed_addr constant [18 x i8] c" [\D9\85\D9\8F\D8\B9\D8\A7\D8\B1 \E2\9D\8C]\00", align 1
@str.const.18 = private unnamed_addr constant [32 x i8] c"    \E2\9C\85 \D8\AA\D9\85\D8\AA \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9: \00", align 1
@str.const.23 = private unnamed_addr constant [39 x i8] c"    \E2\9D\8C \D8\A7\D9\84\D9\83\D8\AA\D8\A7\D8\A8 \D8\BA\D9\8A\D8\B1 \D9\85\D8\AA\D8\A7\D8\AD: \00", align 1
@str.const.28 = private unnamed_addr constant [26 x i8] c"    \E2\9C\85 \D8\AA\D9\85 \D8\A5\D8\B1\D8\AC\D8\A7\D8\B9: \00", align 1
@str.const.33 = private unnamed_addr constant [48 x i8] c"    \E2\9A\A0\EF\B8\8F \D8\A7\D9\84\D9\83\D8\AA\D8\A7\D8\A8 \D9\84\D9\8A\D8\B3 \D9\85\D9\8F\D8\B9\D8\A7\D8\B1\D8\A7\D9\8B: \00", align 1
@str.ret = private unnamed_addr constant [9 x i8] c"\D9\83\D8\AA\D9\8A\D8\A8\00", align 1
@str.ret.38 = private unnamed_addr constant [20 x i8] c"\D9\83\D8\AA\D8\A7\D8\A8 \D9\85\D8\AA\D9\88\D8\B3\D8\B7\00", align 1
@str.ret.39 = private unnamed_addr constant [18 x i8] c"\D9\83\D8\AA\D8\A7\D8\A8 \D9\83\D8\A8\D9\8A\D8\B1\00", align 1
@str.ret.40 = private unnamed_addr constant [13 x i8] c"\D9\85\D9\88\D8\B3\D9\88\D8\B9\D8\A9\00", align 1
@str.const.41 = private unnamed_addr constant [9 x i8] c"    \E2\9D\8C \00", align 1
@str.const.44 = private unnamed_addr constant [56 x i8] c" \D9\88\D8\B5\D9\84 \D9\84\D9\84\D8\AD\D8\AF \D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89 \D9\85\D9\86 \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA\00", align 1
@str.const.48 = private unnamed_addr constant [10 x i8] c"    \F0\9F\91\A4 \00", align 1
@str.const.51 = private unnamed_addr constant [15 x i8] c" (\D8\B9\D8\B6\D9\88\D9\8A\D8\A9: \00", align 1
@str.const.54 = private unnamed_addr constant [26 x i8] c") \D9\83\D8\AA\D8\A8 \D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9: \00", align 1
@str.const.57 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@str.const.62 = private unnamed_addr constant [95 x i8] c"    \E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.66 = private unnamed_addr constant [37 x i8] c"    \E2\95\91 \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D9\83\D8\AA\D8\A8:    \00", align 1
@str.const.71 = private unnamed_addr constant [34 x i8] c"    \E2\95\91 \D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD\D8\A9:          \00", align 1
@str.const.76 = private unnamed_addr constant [36 x i8] c"    \E2\95\91 \D8\A7\D9\84\D9\85\D9\8F\D8\B9\D8\A7\D8\B1\D8\A9:          \00", align 1
@str.const.81 = private unnamed_addr constant [40 x i8] c"    \E2\95\91 \D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9 \D8\A7\D9\84\D9\83\D9\84\D9\8A\D8\A9:     \00", align 1
@str.const.86 = private unnamed_addr constant [95 x i8] c"    \E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.90 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.94 = private unnamed_addr constant [59 x i8] c"\E2\95\91  \F0\9F\93\9A Phase 17: \D9\86\D8\B8\D8\A7\D9\85 \D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D9\83\D8\AA\D8\A8         \E2\95\91\00", align 1
@str.const.98 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.102 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.106 = private unnamed_addr constant [44 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9 \D8\A7\D9\84\D9\83\D8\AA\D8\A8 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.110 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\A8\D8\AE\D9\84\D8\A7\D8\A1\00", align 1
@str.const.111 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D8\AC\D8\A7\D8\AD\D8\B8\00", align 1
@str.const.112 = private unnamed_addr constant [29 x i8] c"\D9\85\D9\82\D8\AF\D9\85\D8\A9 \D8\A7\D8\A8\D9\86 \D8\AE\D9\84\D8\AF\D9\88\D9\86\00", align 1
@str.const.113 = private unnamed_addr constant [18 x i8] c"\D8\A7\D8\A8\D9\86 \D8\AE\D9\84\D8\AF\D9\88\D9\86\00", align 1
@str.const.114 = private unnamed_addr constant [26 x i8] c"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D8\BA\D9\81\D8\B1\D8\A7\D9\86\00", align 1
@str.const.115 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\85\D8\B9\D8\B1\D9\8A\00", align 1
@str.const.116 = private unnamed_addr constant [27 x i8] c"\D8\A3\D9\84\D9\81 \D9\84\D9\8A\D9\84\D8\A9 \D9\88\D9\84\D9\8A\D9\84\D8\A9\00", align 1
@str.const.117 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\AC\D9\87\D9\88\D9\84\00", align 1
@str.const.118 = private unnamed_addr constant [26 x i8] c"\D8\A7\D9\84\D8\A3\D9\85\D9\8A\D8\B1 \D8\A7\D9\84\D8\B5\D8\BA\D9\8A\D8\B1\00", align 1
@str.const.119 = private unnamed_addr constant [26 x i8] c"\D8\B3\D8\A7\D9\86\D8\AA \D8\A5\D9\83\D8\B2\D9\88\D8\A8\D9\8A\D8\B1\D9\8A\00", align 1
@str.const.120 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.124 = private unnamed_addr constant [46 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\AA\D8\B5\D9\86\D9\8A\D9\81 \D8\A7\D9\84\D8\A3\D8\AD\D8\AC\D8\A7\D9\85 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.128 = private unnamed_addr constant [21 x i8] c"    \D8\A7\D9\84\D8\A8\D8\AE\D9\84\D8\A7\D8\A1: \00", align 1
@str.const.133 = private unnamed_addr constant [35 x i8] c"    \D9\85\D9\82\D8\AF\D9\85\D8\A9 \D8\A7\D8\A8\D9\86 \D8\AE\D9\84\D8\AF\D9\88\D9\86: \00", align 1
@str.const.138 = private unnamed_addr constant [32 x i8] c"    \D8\A7\D9\84\D8\A3\D9\85\D9\8A\D8\B1 \D8\A7\D9\84\D8\B5\D8\BA\D9\8A\D8\B1: \00", align 1
@str.const.143 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.147 = private unnamed_addr constant [35 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D8\A3\D8\B9\D8\B6\D8\A7\D8\A1 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.151 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.152 = private unnamed_addr constant [9 x i8] c"\D9\85\D8\AD\D9\85\D8\AF\00", align 1
@str.const.153 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.157 = private unnamed_addr constant [52 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.161 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.165 = private unnamed_addr constant [68 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9 \D9\83\D8\AA\D8\A7\D8\A8 \D9\85\D9\8F\D8\B9\D8\A7\D8\B1 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.169 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.173 = private unnamed_addr constant [42 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A5\D8\B1\D8\AC\D8\A7\D8\B9 \D8\A7\D9\84\D9\83\D8\AA\D8\A8 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.177 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.181 = private unnamed_addr constant [50 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D8\A9 \D8\A7\D9\84\D9\86\D9\87\D8\A7\D8\A6\D9\8A\D8\A9 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.185 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.189 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.193 = private unnamed_addr constant [52 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.197 = private unnamed_addr constant [57 x i8] c"\E2\9C\85 \D8\AA\D9\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D9\86\D8\B8\D8\A7\D9\85 \D8\A7\D9\84\D9\85\D9\83\D8\AA\D8\A8\D8\A9 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D9\85\D8\A4\D9\84\D9\81", i64 %"\D8\B3\D8\B9\D8\B1", i64 %"\D8\B5\D9\81\D8\AD\D8\A7\D8\AA") {
entry:
  %"\D8\B5\D9\81\D8\AD\D8\A7\D8\AA5" = alloca i64, align 8
  store i64 %"\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", ptr %"\D8\B5\D9\81\D8\AD\D8\A7\D8\AA5", align 8
  %"\D8\B3\D8\B9\D8\B14" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B9\D8\B1", ptr %"\D8\B3\D8\B9\D8\B14", align 8
  %"\D9\85\D8\A4\D9\84\D9\813" = alloca ptr, align 8
  store ptr %"\D9\85\D8\A4\D9\84\D9\81", ptr %"\D9\85\D8\A4\D9\84\D9\813", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\862" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca6 = alloca ptr, align 8
  %alloca7 = alloca ptr, align 8
  %alloca8 = alloca i64, align 8
  %alloca9 = alloca i64, align 8
  store ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", ptr %alloca6, align 8
  store ptr %"\D9\85\D8\A4\D9\84\D9\813", ptr %alloca7, align 8
  %"%\D8\B3\D8\B9\D8\B1.load" = load i64, ptr %"\D8\B3\D8\B9\D8\B14", align 8
  store i64 %"%\D8\B3\D8\B9\D8\B1.load", ptr %alloca8, align 8
  %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B5\D9\81\D8\AD\D8\A7\D8\AA5", align 8
  store i64 %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load", ptr %alloca, align 8
  store i64 1, ptr %alloca9, align 8
  ret void
}

define void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca3)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %"%\D8\A7\D9\84\D8\B3\D8\B9\D8\B1.load" = load i64, ptr %alloca4, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B3\D8\B9\D8\B1.load")
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %alloca5, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\AA\D8\A7\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.10)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  br label %merge_1

else_2:                                           ; preds = %entry
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.14)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret void
}

define i64 @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %alloca5, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\AA\D8\A7\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_3, label %merge_4

then_3:                                           ; preds = %entry
  store i64 0, ptr %alloca5, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 1

merge_4:                                          ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

define void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B1\D8\AF_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %alloca5, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\AA\D8\A7\D8\AD.load", 0
  br i1 %cmpeqtmp, label %then_5, label %else_7

then_5:                                           ; preds = %entry
  store i64 1, ptr %alloca5, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  br label %merge_6

else_7:                                           ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.33)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  br label %merge_6

merge_6:                                          ; preds = %else_7, %then_5
  ret void
}

define ptr @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca ptr, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load", 100
  br i1 %cmplttmp, label %then_8, label %merge_9

then_8:                                           ; preds = %entry
  ret ptr @str.ret

merge_9:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load6" = load i64, ptr %alloca, align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load6", 300
  br i1 %cmplttmp7, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  ret ptr @str.ret.38

merge_11:                                         ; preds = %merge_9
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load8" = load i64, ptr %alloca, align 8
  %cmplttmp9 = icmp slt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load8", 600
  br i1 %cmplttmp9, label %then_12, label %merge_13

then_12:                                          ; preds = %merge_11
  ret ptr @str.ret.39

merge_13:                                         ; preds = %merge_11
  ret ptr @str.ret.40
}

define void @"\D8\B9\D8\B6\D9\88.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, ptr %"\D8\A7\D8\B3\D9\85", i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A93", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca4 = alloca ptr, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  store ptr %"\D8\A7\D8\B3\D9\852", ptr %alloca4, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A93", align 8
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D8\B6\D9\88\D9\8A\D8\A9.load", ptr %alloca5, align 8
  store i64 0, ptr %alloca6, align 8
  store i64 3, ptr %alloca, align 8
  ret void
}

define i64 @"\D8\B9\D8\B6\D9\88.\D9\8A\D9\85\D9\83\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %alloca4, align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load" = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load", %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load"
  br i1 %cmplttmp, label %then_14, label %merge_15

then_14:                                          ; preds = %entry
  ret i64 1

merge_15:                                         ; preds = %entry
  ret i64 0
}

define i64 @"\D8\B9\D8\B6\D9\88.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %alloca4, align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load" = load i64, ptr %alloca, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load", %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load"
  br i1 %cmpgetmp, label %then_16, label %merge_17

then_16:                                          ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0

merge_17:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load5" = load i64, ptr %alloca4, align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load5", 1
  store i64 %addtmp, ptr %alloca4, align 8
  ret i64 1
}

define void @"\D8\B9\D8\B6\D9\88.\D8\A5\D8\B1\D8\AC\D8\A7\D8\B9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %alloca4, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_18, label %merge_19

then_18:                                          ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load5" = load i64, ptr %alloca4, align 8
  %subtmp = sub i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load5", 1
  store i64 %subtmp, ptr %alloca4, align 8
  br label %merge_19

merge_19:                                         ; preds = %then_18, %entry
  ret void
}

define void @"\D8\B9\D8\B6\D9\88.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca ptr, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %alloca2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D8\B6\D9\88\D9\8A\D8\A9.load" = load i64, ptr %alloca3, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D8\B6\D9\88\D9\8A\D8\A9.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.54)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load" = load i64, ptr %alloca4, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9.load")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load" = load i64, ptr %alloca, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\AD\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B5\D9\89.load")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret void
}

define void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  store i64 0, ptr %alloca2, align 8
  store i64 0, ptr %alloca3, align 8
  store i64 0, ptr %alloca4, align 8
  ret void
}

define void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %self, i64 %"\D8\B3\D8\B9\D8\B1", i64 %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD") {
entry:
  %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD3" = alloca i64, align 8
  store i64 %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD", ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD3", align 8
  %"\D8\B3\D8\B9\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B9\D8\B1", ptr %"\D8\B3\D8\B9\D8\B12", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D8\A8.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D8\A8.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %alloca6, align 8
  %"%\D8\B3\D8\B9\D8\B1.load" = load i64, ptr %"\D8\B3\D8\B9\D8\B12", align 8
  %addtmp7 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load", %"%\D8\B3\D8\B9\D8\B1.load"
  store i64 %addtmp7, ptr %alloca6, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %"\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD3", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD.load", 1
  br i1 %cmpeqtmp, label %then_20, label %else_22

then_20:                                          ; preds = %entry
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %alloca5, align 8
  %addtmp8 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD.load", 1
  store i64 %addtmp8, ptr %alloca5, align 8
  br label %merge_21

else_22:                                          ; preds = %entry
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\8F\D8\B9\D8\A7\D8\B1.load" = load i64, ptr %alloca4, align 8
  %addtmp9 = add i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\8F\D8\B9\D8\A7\D8\B1.load", 1
  store i64 %addtmp9, ptr %alloca4, align 8
  br label %merge_21

merge_21:                                         ; preds = %else_22, %then_20
  ret void
}

define void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.66)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D8\A8.load" = load i64, ptr %alloca, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D8\A8.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.71)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %alloca3, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD.load")
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\8F\D8\B9\D8\A7\D8\B1.load" = load i64, ptr %alloca2, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\85\D9\8F\D8\B9\D8\A7\D8\B1.load")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.81)
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %alloca4, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load")
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.98)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.106)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %alloca = alloca i64, align 8
  %"%75.load" = load i64, ptr %alloca, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%75.load", ptr @str.const.110, ptr @str.const.111, i64 45, i64 280)
  %alloca1 = alloca i64, align 8
  %"%75.load2" = load i64, ptr %alloca, align 8
  store i64 %"%75.load2", ptr %alloca1, align 8
  %alloca3 = alloca i64, align 8
  %"%81.load" = load i64, ptr %alloca3, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%81.load", ptr @str.const.112, ptr @str.const.113, i64 120, i64 750)
  %alloca4 = alloca i64, align 8
  %"%81.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%81.load5", ptr %alloca4, align 8
  %alloca6 = alloca i64, align 8
  %"%87.load" = load i64, ptr %alloca6, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%87.load", ptr @str.const.114, ptr @str.const.115, i64 55, i64 190)
  %alloca7 = alloca i64, align 8
  %"%87.load8" = load i64, ptr %alloca6, align 8
  store i64 %"%87.load8", ptr %alloca7, align 8
  %alloca9 = alloca i64, align 8
  %"%93.load" = load i64, ptr %alloca9, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%93.load", ptr @str.const.116, ptr @str.const.117, i64 85, i64 520)
  %alloca10 = alloca i64, align 8
  %"%93.load11" = load i64, ptr %alloca9, align 8
  store i64 %"%93.load11", ptr %alloca10, align 8
  %alloca12 = alloca i64, align 8
  %"%99.load" = load i64, ptr %alloca12, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%99.load", ptr @str.const.118, ptr @str.const.119, i64 30, i64 80)
  %alloca13 = alloca i64, align 8
  %"%99.load14" = load i64, ptr %alloca12, align 8
  store i64 %"%99.load14", ptr %alloca13, align 8
  %"%\D9\831.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\831.load")
  %"%\D9\832.load" = load i64, ptr %alloca4, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\832.load")
  %"%\D9\833.load" = load i64, ptr %alloca7, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\833.load")
  %"%\D9\834.load" = load i64, ptr %alloca10, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\834.load")
  %"%\D9\835.load" = load i64, ptr %alloca13, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\835.load")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.120)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.124)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %"%\D9\831.load15" = load i64, ptr %alloca1, align 8
  %calltmp = call ptr @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D9\831.load15")
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.133)
  %"%\D9\832.load16" = load i64, ptr %alloca4, align 8
  %calltmp17 = call ptr @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D9\832.load16")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp17)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.138)
  %"%\D9\835.load18" = load i64, ptr %alloca13, align 8
  %calltmp19 = call ptr @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\AA\D8\B5\D9\86\D9\8A\D9\81_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D9\835.load18")
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp19)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.147)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %alloca20 = alloca i64, align 8
  %"%120.load" = load i64, ptr %alloca20, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%120.load", ptr @str.const.151, i64 1001)
  %alloca21 = alloca i64, align 8
  %"%120.load22" = load i64, ptr %alloca20, align 8
  store i64 %"%120.load22", ptr %alloca21, align 8
  %alloca23 = alloca i64, align 8
  %"%124.load" = load i64, ptr %alloca23, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%124.load", ptr @str.const.152, i64 1002)
  %alloca24 = alloca i64, align 8
  %"%124.load25" = load i64, ptr %alloca23, align 8
  store i64 %"%124.load25", ptr %alloca24, align 8
  %"%\D8\B91.load" = load i64, ptr %alloca21, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B91.load")
  %"%\D8\B92.load" = load i64, ptr %alloca24, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B92.load")
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D9\831.load26" = load i64, ptr %alloca1, align 8
  %calltmp27 = call i64 @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(i64 %"%\D9\831.load26")
  %"%\D8\B91.load28" = load i64, ptr %alloca21, align 8
  %calltmp29 = call i64 @"\D8\B9\D8\B6\D9\88.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\B91.load28")
  %"%\D9\832.load30" = load i64, ptr %alloca4, align 8
  %calltmp31 = call i64 @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(i64 %"%\D9\832.load30")
  %"%\D8\B91.load32" = load i64, ptr %alloca21, align 8
  %calltmp33 = call i64 @"\D8\B9\D8\B6\D9\88.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\B91.load32")
  %"%\D9\834.load34" = load i64, ptr %alloca10, align 8
  %calltmp35 = call i64 @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(i64 %"%\D9\834.load34")
  %"%\D8\B92.load36" = load i64, ptr %alloca24, align 8
  %calltmp37 = call i64 @"\D8\B9\D8\B6\D9\88.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\B92.load36")
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.161)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.165)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D9\831.load38" = load i64, ptr %alloca1, align 8
  %calltmp39 = call i64 @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\A7\D8\B3\D8\AA\D8\B9\D8\A7\D8\B1\D8\A9"(i64 %"%\D9\831.load38")
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.169)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.173)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D9\831.load40" = load i64, ptr %alloca1, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B1\D8\AF_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D9\831.load40")
  %"%\D8\B91.load41" = load i64, ptr %alloca21, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\A5\D8\B1\D8\AC\D8\A7\D8\B9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\B91.load41")
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.177)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.181)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D9\831.load42" = load i64, ptr %alloca1, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\831.load42")
  %"%\D9\832.load43" = load i64, ptr %alloca4, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\832.load43")
  %"%\D9\833.load44" = load i64, ptr %alloca7, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\833.load44")
  %"%\D9\834.load45" = load i64, ptr %alloca10, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\834.load45")
  %"%\D9\835.load46" = load i64, ptr %alloca13, align 8
  call void @"\D9\83\D8\AA\D8\A7\D8\A8.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\835.load46")
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.185)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D8\B91.load47" = load i64, ptr %alloca21, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B91.load47")
  %"%\D8\B92.load48" = load i64, ptr %alloca24, align 8
  call void @"\D8\B9\D8\B6\D9\88.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\B92.load48")
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.189)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.193)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %alloca49 = alloca i64, align 8
  %alloca50 = alloca i64, align 8
  %"%157.load" = load i64, ptr %alloca49, align 8
  store i64 %"%157.load", ptr %alloca50, align 8
  %"%\D8\A5\D8\AD\D8\B5.load" = load i64, ptr %alloca50, align 8
  call void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\A5\D8\AD\D8\B5.load", i64 45, i64 1)
  %"%\D8\A5\D8\AD\D8\B5.load51" = load i64, ptr %alloca50, align 8
  call void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\A5\D8\AD\D8\B5.load51", i64 120, i64 0)
  %"%\D8\A5\D8\AD\D8\B5.load52" = load i64, ptr %alloca50, align 8
  call void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\A5\D8\AD\D8\B5.load52", i64 55, i64 1)
  %"%\D8\A5\D8\AD\D8\B5.load53" = load i64, ptr %alloca50, align 8
  call void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\A5\D8\AD\D8\B5.load53", i64 85, i64 0)
  %"%\D8\A5\D8\AD\D8\B5.load54" = load i64, ptr %alloca50, align 8
  call void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\A5\D8\B6\D8\A7\D9\81\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8"(i64 %"%\D8\A5\D8\AD\D8\B5.load54", i64 30, i64 1)
  %"%\D8\A5\D8\AD\D8\B5.load55" = load i64, ptr %alloca50, align 8
  call void @"\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA.\D8\B9\D8\B1\D8\B6"(i64 %"%\D8\A5\D8\AD\D8\B5.load55")
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.197)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
