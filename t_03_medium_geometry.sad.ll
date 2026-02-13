; ModuleID = '03_medium_geometry'
source_filename = "03_medium_geometry"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [2 x i8] c"(\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.1 = private unnamed_addr constant [4 x i8] c"\D8\8C \00", align 1
@str.const.4 = private unnamed_addr constant [2 x i8] c")\00", align 1
@str.const.6 = private unnamed_addr constant [24 x i8] c"    \F0\9F\93\90 \D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84 [\00", align 1
@str.const.9 = private unnamed_addr constant [5 x i8] c" \C3\97 \00", align 1
@str.const.12 = private unnamed_addr constant [18 x i8] c"] \D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\AD\D8\A9=\00", align 1
@str.const.15 = private unnamed_addr constant [15 x i8] c" \D8\A7\D9\84\D9\85\D8\AD\D9\8A\D8\B7=\00", align 1
@str.const.18 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.20 = private unnamed_addr constant [26 x i8] c"    \E2\AD\95 \D8\AF\D8\A7\D8\A6\D8\B1\D8\A9 [\D9\86\D9\82=\00", align 1
@str.const.23 = private unnamed_addr constant [20 x i8] c"] \D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\AD\D8\A9\E2\89\88\00", align 1
@str.const.26 = private unnamed_addr constant [17 x i8] c" \D8\A7\D9\84\D9\85\D8\AD\D9\8A\D8\B7\E2\89\88\00", align 1
@str.const.31 = private unnamed_addr constant [20 x i8] c"    \F0\9F\94\BA \D9\85\D8\AB\D9\84\D8\AB [\00", align 1
@str.const.40 = private unnamed_addr constant [16 x i8] c"] \D8\A7\D9\84\D9\85\D8\AD\D9\8A\D8\B7=\00", align 1
@str.const.45 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.49 = private unnamed_addr constant [59 x i8] c"\E2\95\91  \F0\9F\93\90 Phase 17: \D8\A3\D8\B4\D9\83\D8\A7\D9\84 \D9\87\D9\86\D8\AF\D8\B3\D9\8A\D8\A9            \E2\95\91\00", align 1
@str.const.53 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.57 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.61 = private unnamed_addr constant [33 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D9\86\D9\82\D8\A7\D8\B7 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.65 = private unnamed_addr constant [21 x i8] c"    \D8\A7\D9\84\D9\86\D9\82\D8\B7\D8\A9 1: \00", align 1
@str.const.67 = private unnamed_addr constant [31 x i8] c" - \D9\85\D8\B3\D8\A7\D9\81\D8\A9 \D9\85\D8\A7\D9\86\D9\87\D8\A7\D8\AA\D9\86: \00", align 1
@str.const.70 = private unnamed_addr constant [16 x i8] c" - \D8\A7\D9\84\D8\B1\D8\A8\D8\B9: \00", align 1
@str.const.75 = private unnamed_addr constant [21 x i8] c"    \D8\A7\D9\84\D9\86\D9\82\D8\B7\D8\A9 2: \00", align 1
@str.const.85 = private unnamed_addr constant [21 x i8] c"    \D8\A7\D9\84\D9\86\D9\82\D8\B7\D8\A9 3: \00", align 1
@str.const.95 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.99 = private unnamed_addr constant [41 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84\D8\A7\D8\AA \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.103 = private unnamed_addr constant [41 x i8] c"    \E2\9C\85 \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84 2 \D9\87\D9\88 \D9\85\D8\B1\D8\A8\D8\B9\00", align 1
@str.const.107 = private unnamed_addr constant [47 x i8] c"    \E2\9D\8C \D8\A7\D9\84\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84 2 \D9\84\D9\8A\D8\B3 \D9\85\D8\B1\D8\A8\D8\B9\D8\A7\D9\8B\00", align 1
@str.const.111 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.115 = private unnamed_addr constant [35 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D8\AF\D9\88\D8\A7\D8\A6\D8\B1 \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.119 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.123 = private unnamed_addr constant [37 x i8] c"\E2\94\81\E2\94\81\E2\94\81 \D8\A7\D9\84\D9\85\D8\AB\D9\84\D8\AB\D8\A7\D8\AA \E2\94\81\E2\94\81\E2\94\81\00", align 1
@str.const.127 = private unnamed_addr constant [51 x i8] c"    \E2\9C\85 \D8\A7\D9\84\D9\85\D8\AB\D9\84\D8\AB 1 \D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A \D8\A7\D9\84\D8\A3\D8\B6\D9\84\D8\A7\D8\B9\00", align 1
@str.const.131 = private unnamed_addr constant [51 x i8] c"    \E2\9C\85 \D8\A7\D9\84\D9\85\D8\AB\D9\84\D8\AB 3 \D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A \D8\A7\D9\84\D8\B3\D8\A7\D9\82\D9\8A\D9\86\00", align 1
@str.const.135 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.139 = private unnamed_addr constant [65 x i8] c"\E2\9C\85 \D8\AA\D9\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D8\B4\D9\83\D8\A7\D9\84 \D8\A7\D9\84\D9\87\D9\86\D8\AF\D8\B3\D9\8A\D8\A9 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B3", i64 %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B5") {
entry:
  %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B53" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B5", ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B53", align 8
  %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B32" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B3", ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B32", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\B3.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B32", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\B3.load", ptr %alloca, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\B5.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\B53", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\B5.load", ptr %alloca4, align 8
  ret void
}

define void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %"%\D8\B3.load" = load i64, ptr %alloca, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B3.load")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B5.load" = load i64, ptr %alloca2, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B5.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.4)
  ret void
}

define i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D9\81\D8\A9_\D9\85\D9\86_\D8\A7\D9\84\D8\A3\D8\B5\D9\84"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  store i64 0, ptr %alloca3, align 8
  %"%\D8\B3.load" = load i64, ptr %alloca, align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load" = load i64, ptr %alloca3, align 8
  %"%\D8\B3.load4" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load", %"%\D8\B3.load4"
  store i64 %addtmp, ptr %alloca3, align 8
  br label %merge_1

else_2:                                           ; preds = %entry
  %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load5" = load i64, ptr %alloca3, align 8
  %"%\D8\B3.load6" = load i64, ptr %alloca, align 8
  %subtmp = sub i64 %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load5", %"%\D8\B3.load6"
  store i64 %subtmp, ptr %alloca3, align 8
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  %"%\D8\B5.load" = load i64, ptr %alloca2, align 8
  %cmpgetmp7 = icmp sge i64 %"%\D8\B5.load", 0
  br i1 %cmpgetmp7, label %then_3, label %else_5

then_3:                                           ; preds = %merge_1
  %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load8" = load i64, ptr %alloca3, align 8
  %"%\D8\B5.load9" = load i64, ptr %alloca2, align 8
  %addtmp10 = add i64 %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load8", %"%\D8\B5.load9"
  store i64 %addtmp10, ptr %alloca3, align 8
  br label %merge_4

else_5:                                           ; preds = %merge_1
  %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load11" = load i64, ptr %alloca3, align 8
  %"%\D8\B5.load12" = load i64, ptr %alloca2, align 8
  %subtmp13 = sub i64 %"%\D9\85\D8\B3\D8\A7\D9\81\D8\A9.load11", %"%\D8\B5.load12"
  store i64 %subtmp13, ptr %alloca3, align 8
  br label %merge_4

merge_4:                                          ; preds = %else_5, %then_3
  %loadtmp = load i64, ptr %alloca3, align 8
  ret i64 %loadtmp
}

define i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D9\81\D9\8A_\D8\A7\D9\84\D8\B1\D8\A8\D8\B9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %"%\D8\B3.load" = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B3.load", 0
  br i1 %cmpgttmp, label %then_6, label %merge_7

then_6:                                           ; preds = %entry
  %"%\D8\B5.load" = load i64, ptr %alloca2, align 8
  %cmpgttmp3 = icmp sgt i64 %"%\D8\B5.load", 0
  br i1 %cmpgttmp3, label %then_8, label %merge_9

merge_7:                                          ; preds = %entry
  %"%\D8\B5.load4" = load i64, ptr %alloca2, align 8
  %cmpgttmp5 = icmp sgt i64 %"%\D8\B5.load4", 0
  br i1 %cmpgttmp5, label %then_10, label %merge_11

then_8:                                           ; preds = %then_6
  ret i64 1

merge_9:                                          ; preds = %then_6
  ret i64 4

then_10:                                          ; preds = %merge_7
  ret i64 2

merge_11:                                         ; preds = %merge_7
  ret i64 3
}

define void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\B9\D8\B1\D8\B6", i64 %"\D8\B7\D9\88\D9\84") {
entry:
  %"\D8\B7\D9\88\D9\843" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84", ptr %"\D8\B7\D9\88\D9\843", align 8
  %"\D8\B9\D8\B1\D8\B62" = alloca i64, align 8
  store i64 %"\D8\B9\D8\B1\D8\B6", ptr %"\D8\B9\D8\B1\D8\B62", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca4 = alloca i64, align 8
  %"%\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %"\D8\B9\D8\B1\D8\B62", align 8
  store i64 %"%\D8\B9\D8\B1\D8\B6.load", ptr %alloca, align 8
  %"%\D8\B7\D9\88\D9\84.load" = load i64, ptr %"\D8\B7\D9\88\D9\843", align 8
  store i64 %"%\D8\B7\D9\88\D9\84.load", ptr %alloca4, align 8
  ret void
}

define i64 @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\AD\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca2, align 8
  %multmp = mul i64 %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load", %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load"
  ret i64 %multmp
}

define i64 @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A7\D9\84\D9\85\D8\AD\D9\8A\D8\B7"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load", %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load"
  %multmp = mul i64 2, %addtmp
  ret i64 %multmp
}

define i64 @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D9\85\D8\B1\D8\A8\D8\B9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load", %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load"
  br i1 %cmpeqtmp, label %then_12, label %merge_13

then_12:                                          ; preds = %entry
  ret i64 1

merge_13:                                         ; preds = %entry
  ret i64 0
}

define void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.6)
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %alloca, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load" = load i64, ptr %alloca2, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load3" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load4" = load i64, ptr %alloca2, align 8
  %multmp = mul i64 %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load3", %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load4"
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %multmp)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load5" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load6" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.load5", %"%\D8\A7\D9\84\D8\B7\D9\88\D9\84.load6"
  %multmp7 = mul i64 2, %addtmp
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %multmp7)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  ret void
}

define void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D9\86\D9\82") {
entry:
  %"\D9\86\D9\822" = alloca i64, align 8
  store i64 %"\D9\86\D9\82", ptr %"\D9\86\D9\822", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %"%\D9\86\D9\82.load" = load i64, ptr %"\D9\86\D9\822", align 8
  store i64 %"%\D9\86\D9\82.load", ptr %alloca, align 8
  ret void
}

define i64 @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\AD\D8\A9_\D8\A7\D9\84\D8\AA\D9\82\D8\B1\D9\8A\D8\A8\D9\8A\D8\A9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load" = load i64, ptr %alloca, align 8
  %multmp = mul i64 314, %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load"
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load2" = load i64, ptr %alloca, align 8
  %multmp3 = mul i64 %multmp, %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load2"
  %divtmp = sdiv i64 %multmp3, 100
  ret i64 %divtmp
}

define i64 @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\A7\D9\84\D9\85\D8\AD\D9\8A\D8\B7_\D8\A7\D9\84\D8\AA\D9\82\D8\B1\D9\8A\D8\A8\D9\8A"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load" = load i64, ptr %alloca, align 8
  %multmp = mul i64 628, %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load"
  %divtmp = sdiv i64 %multmp, 100
  ret i64 %divtmp
}

define void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load" = load i64, ptr %alloca, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load2" = load i64, ptr %alloca, align 8
  %multmp = mul i64 314, %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load2"
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load3" = load i64, ptr %alloca, align 8
  %multmp4 = mul i64 %multmp, %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load3"
  %divtmp = sdiv i64 %multmp4, 100
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.26)
  %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load5" = load i64, ptr %alloca, align 8
  %multmp6 = mul i64 628, %"%\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D9\82\D8\B7\D8\B1.load5"
  %divtmp7 = sdiv i64 %multmp6, 100
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp7)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  ret void
}

define void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A3", i64 %"\D8\A8", i64 %"\D8\AC") {
entry:
  %"\D8\AC4" = alloca i64, align 8
  store i64 %"\D8\AC", ptr %"\D8\AC4", align 8
  %"\D8\A83" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A83", align 8
  %"\D8\A32" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A32", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca5 = alloca i64, align 8
  %alloca6 = alloca i64, align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A32", align 8
  store i64 %"%\D8\A3.load", ptr %alloca, align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A83", align 8
  store i64 %"%\D8\A8.load", ptr %alloca5, align 8
  %"%\D8\AC.load" = load i64, ptr %"\D8\AC4", align 8
  store i64 %"%\D8\AC.load", ptr %alloca6, align 8
  ret void
}

define i64 @"\D9\85\D8\AB\D9\84\D8\AB.\D8\A7\D9\84\D9\85\D8\AD\D9\8A\D8\B7"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load"
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load" = load i64, ptr %alloca3, align 8
  %addtmp4 = add i64 %addtmp, %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load"
  ret i64 %addtmp4
}

define i64 @"\D9\85\D8\AB\D9\84\D8\AB.\D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A_\D8\A7\D9\84\D8\A3\D8\B6\D9\84\D8\A7\D8\B9"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load"
  br i1 %cmpeqtmp, label %then_14, label %merge_15

then_14:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load4" = load i64, ptr %alloca2, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load4", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load"
  br i1 %cmpeqtmp5, label %then_16, label %merge_17

merge_15:                                         ; preds = %merge_17, %entry
  ret i64 0

then_16:                                          ; preds = %then_14
  ret i64 1

merge_17:                                         ; preds = %then_14
  br label %merge_15
}

define i64 @"\D9\85\D8\AB\D9\84\D8\AB.\D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A_\D8\A7\D9\84\D8\B3\D8\A7\D9\82\D9\8A\D9\86"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load" = load i64, ptr %alloca2, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load"
  br i1 %cmpeqtmp, label %then_18, label %merge_19

then_18:                                          ; preds = %entry
  ret i64 1

merge_19:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load4" = load i64, ptr %alloca2, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load" = load i64, ptr %alloca3, align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load4", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load"
  br i1 %cmpeqtmp5, label %then_20, label %merge_21

then_20:                                          ; preds = %merge_19
  ret i64 1

merge_21:                                         ; preds = %merge_19
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load6" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load7" = load i64, ptr %alloca3, align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load6", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load7"
  br i1 %cmpeqtmp8, label %then_22, label %merge_23

then_22:                                          ; preds = %merge_21
  ret i64 1

merge_23:                                         ; preds = %merge_21
  ret i64 0
}

define void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %alloca = alloca i64, align 8
  %alloca2 = alloca i64, align 8
  %alloca3 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load" = load i64, ptr %alloca, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load" = load i64, ptr %alloca2, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load" = load i64, ptr %alloca3, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load4" = load i64, ptr %alloca, align 8
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load5" = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A3.load4", %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\A8.load5"
  %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load6" = load i64, ptr %alloca3, align 8
  %addtmp7 = add i64 %addtmp, %"%\D8\A7\D9\84\D8\B6\D9\84\D8\B9_\D8\AC.load6"
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %addtmp7)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  ret void
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.49)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %alloca = alloca i64, align 8
  %"%92.load" = load i64, ptr %alloca, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%92.load", i64 3, i64 4)
  %alloca1 = alloca i64, align 8
  %"%92.load2" = load i64, ptr %alloca, align 8
  store i64 %"%92.load2", ptr %alloca1, align 8
  %alloca3 = alloca i64, align 8
  %"%96.load" = load i64, ptr %alloca3, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%96.load", i64 -5, i64 2)
  %alloca4 = alloca i64, align 8
  %"%96.load5" = load i64, ptr %alloca3, align 8
  store i64 %"%96.load5", ptr %alloca4, align 8
  %alloca6 = alloca i64, align 8
  %"%101.load" = load i64, ptr %alloca6, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%101.load", i64 -3, i64 -7)
  %alloca7 = alloca i64, align 8
  %"%101.load8" = load i64, ptr %alloca6, align 8
  store i64 %"%101.load8", ptr %alloca7, align 8
  %alloca9 = alloca i64, align 8
  %"%107.load" = load i64, ptr %alloca9, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%107.load", i64 0, i64 0)
  %alloca10 = alloca i64, align 8
  %"%107.load11" = load i64, ptr %alloca9, align 8
  store i64 %"%107.load11", ptr %alloca10, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.65)
  %"%\D9\861.load" = load i64, ptr %alloca1, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\861.load")
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %"%\D9\861.load12" = load i64, ptr %alloca1, align 8
  %calltmp = call i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D9\81\D8\A9_\D9\85\D9\86_\D8\A7\D9\84\D8\A3\D8\B5\D9\84"(i64 %"%\D9\861.load12")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %"%\D9\861.load13" = load i64, ptr %alloca1, align 8
  %calltmp14 = call i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D9\81\D9\8A_\D8\A7\D9\84\D8\B1\D8\A8\D8\B9"(i64 %"%\D9\861.load13")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp14)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.75)
  %"%\D9\862.load" = load i64, ptr %alloca4, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\862.load")
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %"%\D9\862.load15" = load i64, ptr %alloca4, align 8
  %calltmp16 = call i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D9\81\D8\A9_\D9\85\D9\86_\D8\A7\D9\84\D8\A3\D8\B5\D9\84"(i64 %"%\D9\862.load15")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp16)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %"%\D9\862.load17" = load i64, ptr %alloca4, align 8
  %calltmp18 = call i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D9\81\D9\8A_\D8\A7\D9\84\D8\B1\D8\A8\D8\B9"(i64 %"%\D9\862.load17")
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp18)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %"%\D9\863.load" = load i64, ptr %alloca7, align 8
  call void @"\D9\86\D9\82\D8\B7\D8\A9.\D8\B9\D8\B1\D8\B6"(i64 %"%\D9\863.load")
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %"%\D9\863.load19" = load i64, ptr %alloca7, align 8
  %calltmp20 = call i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D9\81\D8\A9_\D9\85\D9\86_\D8\A7\D9\84\D8\A3\D8\B5\D9\84"(i64 %"%\D9\863.load19")
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp20)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %"%\D9\863.load21" = load i64, ptr %alloca7, align 8
  %calltmp22 = call i64 @"\D9\86\D9\82\D8\B7\D8\A9.\D9\81\D9\8A_\D8\A7\D9\84\D8\B1\D8\A8\D8\B9"(i64 %"%\D9\863.load21")
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %calltmp22)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %alloca23 = alloca i64, align 8
  %"%131.load" = load i64, ptr %alloca23, align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%131.load", i64 10, i64 5)
  %alloca24 = alloca i64, align 8
  %"%131.load25" = load i64, ptr %alloca23, align 8
  store i64 %"%131.load25", ptr %alloca24, align 8
  %alloca26 = alloca i64, align 8
  %"%135.load" = load i64, ptr %alloca26, align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%135.load", i64 7, i64 7)
  %alloca27 = alloca i64, align 8
  %"%135.load28" = load i64, ptr %alloca26, align 8
  store i64 %"%135.load28", ptr %alloca27, align 8
  %alloca29 = alloca i64, align 8
  %"%139.load" = load i64, ptr %alloca29, align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%139.load", i64 15, i64 3)
  %alloca30 = alloca i64, align 8
  %"%139.load31" = load i64, ptr %alloca29, align 8
  store i64 %"%139.load31", ptr %alloca30, align 8
  %"%\D9\851.load" = load i64, ptr %alloca24, align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D9\851.load")
  %"%\D9\852.load" = load i64, ptr %alloca27, align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D9\852.load")
  %"%\D9\853.load" = load i64, ptr %alloca30, align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D9\853.load")
  %"%\D9\852.load32" = load i64, ptr %alloca27, align 8
  %calltmp33 = call i64 @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D9\85\D8\B1\D8\A8\D8\B9"(i64 %"%\D9\852.load32")
  %cmpeqtmp = icmp eq i64 %calltmp33, 1
  br i1 %cmpeqtmp, label %then_24, label %else_26

then_24:                                          ; preds = %entry
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  br label %merge_25

else_26:                                          ; preds = %entry
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.107)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  br label %merge_25

merge_25:                                         ; preds = %else_26, %then_24
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.111)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.115)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %alloca34 = alloca i64, align 8
  %"%153.load" = load i64, ptr %alloca34, align 8
  call void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%153.load", i64 5)
  %alloca35 = alloca i64, align 8
  %"%153.load36" = load i64, ptr %alloca34, align 8
  store i64 %"%153.load36", ptr %alloca35, align 8
  %alloca37 = alloca i64, align 8
  %"%156.load" = load i64, ptr %alloca37, align 8
  call void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%156.load", i64 10)
  %alloca38 = alloca i64, align 8
  %"%156.load39" = load i64, ptr %alloca37, align 8
  store i64 %"%156.load39", ptr %alloca38, align 8
  %alloca40 = alloca i64, align 8
  %"%159.load" = load i64, ptr %alloca40, align 8
  call void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%159.load", i64 1)
  %alloca41 = alloca i64, align 8
  %"%159.load42" = load i64, ptr %alloca40, align 8
  store i64 %"%159.load42", ptr %alloca41, align 8
  %"%\D8\AF1.load" = load i64, ptr %alloca35, align 8
  call void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\AF1.load")
  %"%\D8\AF2.load" = load i64, ptr %alloca38, align 8
  call void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\AF2.load")
  %"%\D8\AF3.load" = load i64, ptr %alloca41, align 8
  call void @"\D8\AF\D8\A7\D8\A6\D8\B1\D8\A9.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\AF3.load")
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.119)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %alloca43 = alloca i64, align 8
  %"%167.load" = load i64, ptr %alloca43, align 8
  call void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%167.load", i64 5, i64 5, i64 5)
  %alloca44 = alloca i64, align 8
  %"%167.load45" = load i64, ptr %alloca43, align 8
  store i64 %"%167.load45", ptr %alloca44, align 8
  %alloca46 = alloca i64, align 8
  %"%172.load" = load i64, ptr %alloca46, align 8
  call void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%172.load", i64 3, i64 4, i64 5)
  %alloca47 = alloca i64, align 8
  %"%172.load48" = load i64, ptr %alloca46, align 8
  store i64 %"%172.load48", ptr %alloca47, align 8
  %alloca49 = alloca i64, align 8
  %"%177.load" = load i64, ptr %alloca49, align 8
  call void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%177.load", i64 6, i64 6, i64 4)
  %alloca50 = alloca i64, align 8
  %"%177.load51" = load i64, ptr %alloca49, align 8
  store i64 %"%177.load51", ptr %alloca50, align 8
  %"%\D8\AB1.load" = load i64, ptr %alloca44, align 8
  call void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\AB1.load")
  %"%\D8\AB2.load" = load i64, ptr %alloca47, align 8
  call void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\AB2.load")
  %"%\D8\AB3.load" = load i64, ptr %alloca50, align 8
  call void @"\D9\85\D8\AB\D9\84\D8\AB.\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\AB3.load")
  %"%\D8\AB1.load52" = load i64, ptr %alloca44, align 8
  %calltmp53 = call i64 @"\D9\85\D8\AB\D9\84\D8\AB.\D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A_\D8\A7\D9\84\D8\A3\D8\B6\D9\84\D8\A7\D8\B9"(i64 %"%\D8\AB1.load52")
  %cmpeqtmp54 = icmp eq i64 %calltmp53, 1
  br i1 %cmpeqtmp54, label %then_27, label %merge_28

then_27:                                          ; preds = %merge_25
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  br label %merge_28

merge_28:                                         ; preds = %then_27, %merge_25
  %"%\D8\AB3.load55" = load i64, ptr %alloca50, align 8
  %calltmp56 = call i64 @"\D9\85\D8\AB\D9\84\D8\AB.\D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A_\D8\A7\D9\84\D8\B3\D8\A7\D9\82\D9\8A\D9\86"(i64 %"%\D8\AB3.load55")
  %cmpeqtmp57 = icmp eq i64 %calltmp56, 1
  br i1 %cmpeqtmp57, label %then_29, label %merge_30

then_29:                                          ; preds = %merge_28
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.131)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  br label %merge_30

merge_30:                                         ; preds = %then_29, %merge_28
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.135)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
