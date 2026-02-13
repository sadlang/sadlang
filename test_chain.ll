; ModuleID = 'test_chain.sad'
source_filename = "test_chain.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [9 x i8] c"\D9\88\D8\A7\D8\AD\D8\AF\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [11 x i8] c"\D8\A7\D8\AB\D9\86\D8\A7\D9\86\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [11 x i8] c"\D8\AB\D9\84\D8\A7\D8\AB\D8\A9\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@11 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@12 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@13 = private unnamed_addr constant [11 x i8] c"\D8\A3\D8\B1\D8\A8\D8\B9\D8\A9\00", align 1
@14 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@15 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@16 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@17 = private unnamed_addr constant [9 x i8] c"\D8\AE\D9\85\D8\B3\D8\A9\00", align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@20 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@21 = private unnamed_addr constant [7 x i8] c"\D8\B3\D8\AA\D8\A9\00", align 1
@22 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@23 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@24 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@25 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A8\D8\B9\D8\A9\00", align 1
@26 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@27 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@28 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@29 = private unnamed_addr constant [13 x i8] c"\D8\AB\D9\85\D8\A7\D9\86\D9\8A\D8\A9\00", align 1
@30 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@31 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@32 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@33 = private unnamed_addr constant [9 x i8] c"\D8\AA\D8\B3\D8\B9\D8\A9\00", align 1
@34 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@35 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@36 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@37 = private unnamed_addr constant [9 x i8] c"\D8\B9\D8\B4\D8\B1\D8\A9\00", align 1
@38 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@39 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@40 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@41 = private unnamed_addr constant [18 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\00", align 1
@42 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@43 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  store i64 5, ptr %alloca, align 8
  %auto.load = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %auto.load, 1
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  br label %merge_1

else_2:                                           ; preds = %entry
  %auto.load1 = load i64, ptr %alloca, align 8
  %cmpeqtmp2 = icmp eq i64 %auto.load1, 2
  br i1 %cmpeqtmp2, label %then_3, label %else_5

merge_1:                                          ; preds = %merge_4, %then_0
  ret i64 0

then_3:                                           ; preds = %else_2
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  br label %merge_4

else_5:                                           ; preds = %else_2
  %auto.load3 = load i64, ptr %alloca, align 8
  %cmpeqtmp4 = icmp eq i64 %auto.load3, 3
  br i1 %cmpeqtmp4, label %then_6, label %else_8

merge_4:                                          ; preds = %merge_7, %then_3
  br label %merge_1

then_6:                                           ; preds = %else_5
  %4 = call i32 (ptr, ...) @printf(ptr @8, ptr @9)
  %5 = call i32 (ptr, ...) @printf(ptr @10, ptr @11)
  br label %merge_7

else_8:                                           ; preds = %else_5
  %auto.load5 = load i64, ptr %alloca, align 8
  %cmpeqtmp6 = icmp eq i64 %auto.load5, 4
  br i1 %cmpeqtmp6, label %then_9, label %else_11

merge_7:                                          ; preds = %merge_10, %then_6
  br label %merge_4

then_9:                                           ; preds = %else_8
  %6 = call i32 (ptr, ...) @printf(ptr @12, ptr @13)
  %7 = call i32 (ptr, ...) @printf(ptr @14, ptr @15)
  br label %merge_10

else_11:                                          ; preds = %else_8
  %auto.load7 = load i64, ptr %alloca, align 8
  %cmpeqtmp8 = icmp eq i64 %auto.load7, 5
  br i1 %cmpeqtmp8, label %then_12, label %else_14

merge_10:                                         ; preds = %merge_13, %then_9
  br label %merge_7

then_12:                                          ; preds = %else_11
  %8 = call i32 (ptr, ...) @printf(ptr @16, ptr @17)
  %9 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  br label %merge_13

else_14:                                          ; preds = %else_11
  %auto.load9 = load i64, ptr %alloca, align 8
  %cmpeqtmp10 = icmp eq i64 %auto.load9, 6
  br i1 %cmpeqtmp10, label %then_15, label %else_17

merge_13:                                         ; preds = %merge_16, %then_12
  br label %merge_10

then_15:                                          ; preds = %else_14
  %10 = call i32 (ptr, ...) @printf(ptr @20, ptr @21)
  %11 = call i32 (ptr, ...) @printf(ptr @22, ptr @23)
  br label %merge_16

else_17:                                          ; preds = %else_14
  %auto.load11 = load i64, ptr %alloca, align 8
  %cmpeqtmp12 = icmp eq i64 %auto.load11, 7
  br i1 %cmpeqtmp12, label %then_18, label %else_20

merge_16:                                         ; preds = %merge_19, %then_15
  br label %merge_13

then_18:                                          ; preds = %else_17
  %12 = call i32 (ptr, ...) @printf(ptr @24, ptr @25)
  %13 = call i32 (ptr, ...) @printf(ptr @26, ptr @27)
  br label %merge_19

else_20:                                          ; preds = %else_17
  %auto.load13 = load i64, ptr %alloca, align 8
  %cmpeqtmp14 = icmp eq i64 %auto.load13, 8
  br i1 %cmpeqtmp14, label %then_21, label %else_23

merge_19:                                         ; preds = %merge_22, %then_18
  br label %merge_16

then_21:                                          ; preds = %else_20
  %14 = call i32 (ptr, ...) @printf(ptr @28, ptr @29)
  %15 = call i32 (ptr, ...) @printf(ptr @30, ptr @31)
  br label %merge_22

else_23:                                          ; preds = %else_20
  %auto.load15 = load i64, ptr %alloca, align 8
  %cmpeqtmp16 = icmp eq i64 %auto.load15, 9
  br i1 %cmpeqtmp16, label %then_24, label %else_26

merge_22:                                         ; preds = %merge_25, %then_21
  br label %merge_19

then_24:                                          ; preds = %else_23
  %16 = call i32 (ptr, ...) @printf(ptr @32, ptr @33)
  %17 = call i32 (ptr, ...) @printf(ptr @34, ptr @35)
  br label %merge_25

else_26:                                          ; preds = %else_23
  %auto.load17 = load i64, ptr %alloca, align 8
  %cmpeqtmp18 = icmp eq i64 %auto.load17, 10
  br i1 %cmpeqtmp18, label %then_27, label %else_29

merge_25:                                         ; preds = %merge_28, %then_24
  br label %merge_22

then_27:                                          ; preds = %else_26
  %18 = call i32 (ptr, ...) @printf(ptr @36, ptr @37)
  %19 = call i32 (ptr, ...) @printf(ptr @38, ptr @39)
  br label %merge_28

else_29:                                          ; preds = %else_26
  %20 = call i32 (ptr, ...) @printf(ptr @40, ptr @41)
  %21 = call i32 (ptr, ...) @printf(ptr @42, ptr @43)
  br label %merge_28

merge_28:                                         ; preds = %else_29, %then_27
  br label %merge_25
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 @SetConsoleOutputCP(i32 65001)
  %1 = call i32 @SetConsoleCP(i32 65001)
  %2 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %3 = trunc i64 %2 to i32
  ret i32 %3
}

declare i32 @SetConsoleOutputCP(i32)

declare i32 @SetConsoleCP(i32)
