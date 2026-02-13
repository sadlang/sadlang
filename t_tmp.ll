; ModuleID = 'test_pattern_basic'
source_filename = "test_pattern_basic"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [53 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 Pattern Matching \D8\A7\D9\84\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A ===\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [35 x i8] c"--- 1. \D9\85\D8\B7\D8\A7\D8\A8\D9\82\D8\A9 \D8\A7\D8\B1\D9\82\D8\A7\D9\85 ---\00", align 1
@str.const.11 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [33 x i8] c"--- 2. \D9\85\D8\B7\D8\A7\D8\A8\D9\82\D8\A9 \D9\86\D8\B5\D9\88\D8\B5 ---\00", align 1
@str.const.19 = private unnamed_addr constant [9 x i8] c"\D8\A7\D8\AD\D9\85\D8\B1\00", align 1
@str.const.20 = private unnamed_addr constant [25 x i8] c"\D8\A7\D9\84\D8\B1\D9\82\D9\85 \D9\87\D9\88 \D9\88\D8\A7\D8\AD\D8\AF\00", align 1
@str.const.24 = private unnamed_addr constant [31 x i8] c"\D8\A7\D9\84\D8\B1\D9\82\D9\85 \D9\87\D9\88 42 - \D8\B5\D8\AD\D9\8A\D8\AD!\00", align 1
@str.const.28 = private unnamed_addr constant [14 x i8] c"\D8\B1\D9\82\D9\85 \D8\A2\D8\AE\D8\B1\00", align 1
@str.const.32 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.36 = private unnamed_addr constant [37 x i8] c"--- 3. \D8\A7\D9\84\D9\86\D9\85\D8\B7 \D8\A7\D9\84\D8\B4\D8\A7\D9\85\D9\84 _ ---\00", align 1
@0 = private unnamed_addr constant [9 x i8] c"\D8\A7\D8\AE\D8\B6\D8\B1\00", align 1
@str.const.40 = private unnamed_addr constant [20 x i8] c"\D8\A7\D9\84\D9\84\D9\88\D9\86 \D8\A7\D8\AE\D8\B6\D8\B1\00", align 1
@str.const.44 = private unnamed_addr constant [32 x i8] c"\D8\A7\D9\84\D9\84\D9\88\D9\86 \D8\A7\D8\AD\D9\85\D8\B1 - \D8\B5\D8\AD\D9\8A\D8\AD!\00", align 1
@1 = private unnamed_addr constant [9 x i8] c"\D8\A7\D8\B2\D8\B1\D9\82\00", align 1
@str.const.48 = private unnamed_addr constant [20 x i8] c"\D8\A7\D9\84\D9\84\D9\88\D9\86 \D8\A7\D8\B2\D8\B1\D9\82\00", align 1
@str.const.52 = private unnamed_addr constant [25 x i8] c"\D9\84\D9\88\D9\86 \D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\00", align 1
@str.const.56 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.60 = private unnamed_addr constant [44 x i8] c"--- 4. \D9\85\D8\B7\D8\A7\D8\A8\D9\82\D8\A9 \D9\82\D9\8A\D9\85 \D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9 ---\00", align 1
@str.const.64 = private unnamed_addr constant [9 x i8] c"\D9\88\D8\A7\D8\AD\D8\AF\00", align 1
@str.const.68 = private unnamed_addr constant [11 x i8] c"\D8\A7\D8\AB\D9\86\D8\A7\D9\86\00", align 1
@str.const.72 = private unnamed_addr constant [41 x i8] c"\D9\82\D9\8A\D9\85\D8\A9 \D8\A7\D8\AE\D8\B1\D9\89 (wildcard) - \D8\B5\D8\AD\D9\8A\D8\AD!\00", align 1
@str.const.76 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.80 = private unnamed_addr constant [35 x i8] c"--- 5. \D9\85\D8\B7\D8\A7\D8\A8\D9\82\D8\A9 \D8\A7\D9\84\D8\B5\D9\81\D8\B1 ---\00", align 1
@str.const.84 = private unnamed_addr constant [38 x i8] c"\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \D8\B5\D8\AD\D9\8A\D8\AD\D8\A9 - \D8\B5\D8\AD\D9\8A\D8\AD!\00", align 1
@str.const.88 = private unnamed_addr constant [26 x i8] c"\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9 \D8\AE\D8\A7\D8\B7\D8\A6\D8\A9\00", align 1
@str.const.92 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.96 = private unnamed_addr constant [51 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A ===\00", align 1
@str.const.100 = private unnamed_addr constant [32 x i8] c"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9 \D8\B5\D9\81\D8\B1 - \D8\B5\D8\AD\D9\8A\D8\AD!\00", align 1
@str.const.104 = private unnamed_addr constant [29 x i8] c"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9 \D9\84\D9\8A\D8\B3\D8\AA \D8\B5\D9\81\D8\B1\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 42, ptr %alloca, align 8
  %loadtmp = load i64, ptr %alloca, align 8
  br label %match.case0.test_1

match.end_0:                                      ; preds = %match.case2.body_6, %match.case2.test_5, %match.case1.body_4, %match.case0.body_2
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.19, ptr %alloca1, align 8
  %loadtmp2 = load ptr, ptr %alloca1, align 8
  br label %match.case0.test_8

match.case0.test_1:                               ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_2, label %match.case1.test_3

match.case0.body_2:                               ; preds = %match.case0.test_1
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_0

match.case1.test_3:                               ; preds = %match.case0.test_1
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 42
  br i1 %cmpeqtmp3, label %match.case1.body_4, label %match.case2.test_5

match.case1.body_4:                               ; preds = %match.case1.test_3
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.24)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_0

match.case2.test_5:                               ; preds = %match.case1.test_3
  br i1 true, label %match.case2.body_6, label %match.end_0

match.case2.body_6:                               ; preds = %match.case2.test_5
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_0

match.end_7:                                      ; preds = %match.case3.body_15, %match.case3.test_14, %match.case2.body_13, %match.case1.body_11, %match.case0.body_9
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca4 = alloca i64, align 8
  store i64 999, ptr %alloca4, align 8
  %loadtmp5 = load i64, ptr %alloca4, align 8
  br label %match.case0.test_17

match.case0.test_8:                               ; preds = %match.end_0
  %strcmp.ret = call i32 @strcmp(ptr %loadtmp2, ptr @0)
  %streq = icmp eq i32 %strcmp.ret, 0
  br i1 %streq, label %match.case0.body_9, label %match.case1.test_10

match.case0.body_9:                               ; preds = %match.case0.test_8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_7

match.case1.test_10:                              ; preds = %match.case0.test_8
  %strcmp.ret6 = call i32 @strcmp(ptr %loadtmp2, ptr @str.const.19)
  %streq7 = icmp eq i32 %strcmp.ret6, 0
  br i1 %streq7, label %match.case1.body_11, label %match.case2.test_12

match.case1.body_11:                              ; preds = %match.case1.test_10
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.44)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_7

match.case2.test_12:                              ; preds = %match.case1.test_10
  %strcmp.ret8 = call i32 @strcmp(ptr %loadtmp2, ptr @1)
  %streq9 = icmp eq i32 %strcmp.ret8, 0
  br i1 %streq9, label %match.case2.body_13, label %match.case3.test_14

match.case2.body_13:                              ; preds = %match.case2.test_12
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.48)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_7

match.case3.test_14:                              ; preds = %match.case2.test_12
  br i1 true, label %match.case3.body_15, label %match.end_7

match.case3.body_15:                              ; preds = %match.case3.test_14
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.52)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_7

match.end_16:                                     ; preds = %match.case2.body_22, %match.case2.test_21, %match.case1.body_20, %match.case0.body_18
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca10 = alloca i1, align 1
  store i64 1, ptr %alloca10, align 8
  %loadtmp11 = load i1, ptr %alloca10, align 1
  br label %match.case0.test_24

match.case0.test_17:                              ; preds = %match.end_7
  %cmpeqtmp12 = icmp eq i64 %loadtmp5, 1
  br i1 %cmpeqtmp12, label %match.case0.body_18, label %match.case1.test_19

match.case0.body_18:                              ; preds = %match.case0.test_17
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.64)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_16

match.case1.test_19:                              ; preds = %match.case0.test_17
  %cmpeqtmp13 = icmp eq i64 %loadtmp5, 2
  br i1 %cmpeqtmp13, label %match.case1.body_20, label %match.case2.test_21

match.case1.body_20:                              ; preds = %match.case1.test_19
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.68)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_16

match.case2.test_21:                              ; preds = %match.case1.test_19
  br i1 true, label %match.case2.body_22, label %match.end_16

match.case2.body_22:                              ; preds = %match.case2.test_21
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.72)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_16

match.end_23:                                     ; preds = %match.case1.body_27, %match.case1.test_26, %match.case0.body_25
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.76)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.80)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca14 = alloca i64, align 8
  store i64 0, ptr %alloca14, align 8
  %loadtmp15 = load i64, ptr %alloca14, align 8
  br label %match.case0.test_29

match.case0.test_24:                              ; preds = %match.end_16
  %cmpeqtmp16 = icmp eq i1 %loadtmp11, true
  br i1 %cmpeqtmp16, label %match.case0.body_25, label %match.case1.test_26

match.case0.body_25:                              ; preds = %match.case0.test_24
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.84)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_23

match.case1.test_26:                              ; preds = %match.case0.test_24
  %cmpeqtmp17 = icmp eq i1 %loadtmp11, false
  br i1 %cmpeqtmp17, label %match.case1.body_27, label %match.end_23

match.case1.body_27:                              ; preds = %match.case1.test_26
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_23

match.end_28:                                     ; preds = %match.case1.body_32, %match.case1.test_31, %match.case0.body_30
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.96)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

match.case0.test_29:                              ; preds = %match.end_23
  %cmpeqtmp18 = icmp eq i64 %loadtmp15, 0
  br i1 %cmpeqtmp18, label %match.case0.body_30, label %match.case1.test_31

match.case0.body_30:                              ; preds = %match.case0.test_29
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.100)
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_28

match.case1.test_31:                              ; preds = %match.case0.test_29
  br i1 true, label %match.case1.body_32, label %match.end_28

match.case1.body_32:                              ; preds = %match.case1.test_31
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %match.end_28
}

declare i32 @printf(ptr, ...)

declare i32 @strcmp(ptr, ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
