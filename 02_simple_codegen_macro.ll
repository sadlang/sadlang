; ModuleID = '02_simple_codegen_macro'
source_filename = "02_simple_codegen_macro"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [32 x i8] c"\D8\AE\D8\B7\D8\A3: \D9\82\D8\B3\D9\85\D8\A9 \D8\B9\D9\84\D9\89 \D8\B5\D9\81\D8\B1!\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.ret = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.ret.3 = private unnamed_addr constant [7 x i8] c"\D8\B5\D9\81\D8\B1\00", align 1
@str.ret.4 = private unnamed_addr constant [9 x i8] c"\D9\88\D8\A7\D8\AD\D8\AF\00", align 1
@str.ret.5 = private unnamed_addr constant [11 x i8] c"\D8\A7\D8\AB\D9\86\D8\A7\D9\86\00", align 1
@str.ret.6 = private unnamed_addr constant [11 x i8] c"\D8\AB\D9\84\D8\A7\D8\AB\D8\A9\00", align 1
@str.ret.7 = private unnamed_addr constant [11 x i8] c"\D8\A3\D8\B1\D8\A8\D8\B9\D8\A9\00", align 1
@str.ret.8 = private unnamed_addr constant [9 x i8] c"\D8\AE\D9\85\D8\B3\D8\A9\00", align 1
@str.ret.9 = private unnamed_addr constant [7 x i8] c"\D8\B3\D8\AA\D8\A9\00", align 1
@str.ret.10 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A8\D8\B9\D8\A9\00", align 1
@str.ret.11 = private unnamed_addr constant [13 x i8] c"\D8\AB\D9\85\D8\A7\D9\86\D9\8A\D8\A9\00", align 1
@str.ret.12 = private unnamed_addr constant [9 x i8] c"\D8\AA\D8\B3\D8\B9\D8\A9\00", align 1
@str.ret.13 = private unnamed_addr constant [9 x i8] c"\D8\B9\D8\B4\D8\B1\D8\A9\00", align 1
@str.ret.14 = private unnamed_addr constant [16 x i8] c"\D8\B9\D8\AF\D8\AF \D9\83\D8\A8\D9\8A\D8\B1\00", align 1
@str.const.15 = private unnamed_addr constant [2 x i8] c" \00", align 1
@str.const.17 = private unnamed_addr constant [4 x i8] c" = \00", align 1
@str.const.20 = private unnamed_addr constant [7 x i8] c"  \E2\9C\93 \00", align 1
@str.const.21 = private unnamed_addr constant [11 x i8] c": \D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.25 = private unnamed_addr constant [7 x i8] c"  \E2\9C\97 \00", align 1
@str.const.26 = private unnamed_addr constant [9 x i8] c": \D8\AE\D8\B7\D8\A3\00", align 1
@str.const.30 = private unnamed_addr constant [19 x i8] c"\D9\81\D8\B4\D9\84: 3 + 5 != 8\00", align 1
@str.const.34 = private unnamed_addr constant [43 x i8] c"\D9\86\D8\AA\D8\A7\D8\A6\D8\AC \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AD\D8\B3\D8\A7\D8\A8: \00", align 1
@str.const.35 = private unnamed_addr constant [13 x i8] c" \D9\86\D8\AC\D8\A7\D8\AD\D8\8C \00", align 1
@str.const.36 = private unnamed_addr constant [8 x i8] c" \D9\81\D8\B4\D9\84\00", align 1
@str.const.39 = private unnamed_addr constant [51 x i8] c"=== \D9\85\D8\A7\D9\83\D8\B1\D9\88 \D8\A7\D9\84\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A7\D9\84\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A ===\00", align 1
@str.const.43 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.47 = private unnamed_addr constant [42 x i8] c"--- \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\AD\D8\B3\D8\A7\D8\A8\D9\8A\D8\A9 ---\00", align 1
@str.const.51 = private unnamed_addr constant [2 x i8] c"+\00", align 1
@str.const.52 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@str.const.53 = private unnamed_addr constant [3 x i8] c"\C3\97\00", align 1
@str.const.54 = private unnamed_addr constant [3 x i8] c"\C3\B7\00", align 1
@str.const.55 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@str.const.56 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.60 = private unnamed_addr constant [27 x i8] c"--- \D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A7\D8\AA ---\00", align 1
@str.const.64 = private unnamed_addr constant [25 x i8] c"\D8\A7\D9\84\D8\A3\D9\83\D8\A8\D8\B1(15\D8\8C 25) = \00", align 1
@str.const.67 = private unnamed_addr constant [25 x i8] c"\D8\A7\D9\84\D8\A3\D8\B5\D8\BA\D8\B1(15\D8\8C 25) = \00", align 1
@str.const.70 = private unnamed_addr constant [36 x i8] c"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9 \D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\82\D8\A9(-42) = \00", align 1
@str.const.73 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.77 = private unnamed_addr constant [34 x i8] c"--- \D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85 ---\00", align 1
@str.const.81 = private unnamed_addr constant [3 x i8] c"  \00", align 1
@str.const.85 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.89 = private unnamed_addr constant [36 x i8] c"--- \D9\81\D8\AD\D9\88\D8\B5\D8\A7\D8\AA \D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85 ---\00", align 1
@str.const.93 = private unnamed_addr constant [21 x i8] c"\D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\B1\D9\82\D9\85 7:\00", align 1
@str.const.97 = private unnamed_addr constant [9 x i8] c"\D9\85\D9\88\D8\AC\D8\A8\00", align 1
@str.const.98 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D9\84\D8\A8\00", align 1
@str.const.99 = private unnamed_addr constant [9 x i8] c"\D8\B2\D9\88\D8\AC\D9\8A\00", align 1
@str.const.100 = private unnamed_addr constant [9 x i8] c"\D9\81\D8\B1\D8\AF\D9\8A\00", align 1
@str.const.102 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.106 = private unnamed_addr constant [22 x i8] c"\D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\B1\D9\82\D9\85 -4:\00", align 1
@str.const.115 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.119 = private unnamed_addr constant [48 x i8] c"--- \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\D8\A9 ---\00", align 1
@str.const.123 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.127 = private unnamed_addr constant [62 x i8] c"=== \D8\A7\D9\86\D8\AA\D9\87\D9\89 \D9\85\D8\A7\D9\83\D8\B1\D9\88 \D8\A7\D9\84\D8\AA\D9\88\D9\84\D9\8A\D8\AF \D8\A7\D9\84\D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A ===\00", align 1

define i64 @"\D8\A7\D8\B6\D9\81"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %addtmp
}

define i64 @"\D8\A7\D8\B7\D8\B1\D8\AD"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %subtmp = sub i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %subtmp
}

define i64 @"\D8\A7\D8\B6\D8\B1\D8\A8"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %multmp = mul i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %multmp
}

define i64 @"\D8\A7\D9\82\D8\B3\D9\85"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

merge_1:                                          ; preds = %entry
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %divtmp = sdiv i64 %"%\D8\A3.load", %"%\D8\A8.load3"
  ret i64 %divtmp
}

define i64 @"\D8\A8\D8\A7\D9\82\D9\8A_\D9\82\D8\B3\D9\85\D8\A9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_2, label %merge_3

then_2:                                           ; preds = %entry
  ret i64 0

merge_3:                                          ; preds = %entry
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load3" = load i64, ptr %"\D8\A82", align 8
  %modtmp = srem i64 %"%\D8\A3.load", %"%\D8\A8.load3"
  ret i64 %modtmp
}

define i64 @"\D8\A7\D9\84\D8\A3\D9\83\D8\A8\D8\B1"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A3.load", %"%\D8\A8.load"
  br i1 %cmpgttmp, label %then_4, label %merge_5

then_4:                                           ; preds = %entry
  %loadtmp = load i64, ptr %"\D8\A31", align 8
  ret i64 %loadtmp

merge_5:                                          ; preds = %entry
  %loadtmp3 = load i64, ptr %"\D8\A82", align 8
  ret i64 %loadtmp3
}

define i64 @"\D8\A7\D9\84\D8\A3\D8\B5\D8\BA\D8\B1"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A3.load", %"%\D8\A8.load"
  br i1 %cmplttmp, label %then_6, label %merge_7

then_6:                                           ; preds = %entry
  %loadtmp = load i64, ptr %"\D8\A31", align 8
  ret i64 %loadtmp

merge_7:                                          ; preds = %entry
  %loadtmp3 = load i64, ptr %"\D8\A82", align 8
  ret i64 %loadtmp3
}

define i64 @"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\82\D8\A9"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %then_8, label %merge_9

then_8:                                           ; preds = %entry
  %"%\D9\82.load2" = load i64, ptr %"\D9\821", align 8
  %subtmp = sub i64 0, %"%\D9\82.load2"
  ret i64 %subtmp

merge_9:                                          ; preds = %entry
  %loadtmp = load i64, ptr %"\D9\821", align 8
  ret i64 %loadtmp
}

define ptr @"\D8\B1\D9\82\D9\85_\D9\84\D9\83\D9\84\D9\85\D8\A9"(i64 %"\D8\B1") {
entry:
  %"\D8\B11" = alloca i64, align 8
  store i64 %"\D8\B1", ptr %"\D8\B11", align 8
  %loadtmp = load i64, ptr %"\D8\B11", align 8
  br label %match.case0.test_11

match.end_10:                                     ; preds = %match.case11.test_33
  ret ptr @str.ret

match.case0.test_11:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 0
  br i1 %cmpeqtmp, label %match.case0.body_12, label %match.case1.test_13

match.case0.body_12:                              ; preds = %match.case0.test_11
  ret ptr @str.ret.3

match.case1.test_13:                              ; preds = %match.case0.test_11
  %cmpeqtmp2 = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp2, label %match.case1.body_14, label %match.case2.test_15

match.case1.body_14:                              ; preds = %match.case1.test_13
  ret ptr @str.ret.4

match.case2.test_15:                              ; preds = %match.case1.test_13
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case2.body_16, label %match.case3.test_17

match.case2.body_16:                              ; preds = %match.case2.test_15
  ret ptr @str.ret.5

match.case3.test_17:                              ; preds = %match.case2.test_15
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case3.body_18, label %match.case4.test_19

match.case3.body_18:                              ; preds = %match.case3.test_17
  ret ptr @str.ret.6

match.case4.test_19:                              ; preds = %match.case3.test_17
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case4.body_20, label %match.case5.test_21

match.case4.body_20:                              ; preds = %match.case4.test_19
  ret ptr @str.ret.7

match.case5.test_21:                              ; preds = %match.case4.test_19
  %cmpeqtmp6 = icmp eq i64 %loadtmp, 5
  br i1 %cmpeqtmp6, label %match.case5.body_22, label %match.case6.test_23

match.case5.body_22:                              ; preds = %match.case5.test_21
  ret ptr @str.ret.8

match.case6.test_23:                              ; preds = %match.case5.test_21
  %cmpeqtmp7 = icmp eq i64 %loadtmp, 6
  br i1 %cmpeqtmp7, label %match.case6.body_24, label %match.case7.test_25

match.case6.body_24:                              ; preds = %match.case6.test_23
  ret ptr @str.ret.9

match.case7.test_25:                              ; preds = %match.case6.test_23
  %cmpeqtmp8 = icmp eq i64 %loadtmp, 7
  br i1 %cmpeqtmp8, label %match.case7.body_26, label %match.case8.test_27

match.case7.body_26:                              ; preds = %match.case7.test_25
  ret ptr @str.ret.10

match.case8.test_27:                              ; preds = %match.case7.test_25
  %cmpeqtmp9 = icmp eq i64 %loadtmp, 8
  br i1 %cmpeqtmp9, label %match.case8.body_28, label %match.case9.test_29

match.case8.body_28:                              ; preds = %match.case8.test_27
  ret ptr @str.ret.11

match.case9.test_29:                              ; preds = %match.case8.test_27
  %cmpeqtmp10 = icmp eq i64 %loadtmp, 9
  br i1 %cmpeqtmp10, label %match.case9.body_30, label %match.case10.test_31

match.case9.body_30:                              ; preds = %match.case9.test_29
  ret ptr @str.ret.12

match.case10.test_31:                             ; preds = %match.case9.test_29
  %cmpeqtmp11 = icmp eq i64 %loadtmp, 10
  br i1 %cmpeqtmp11, label %match.case10.body_32, label %match.case11.test_33

match.case10.body_32:                             ; preds = %match.case10.test_31
  ret ptr @str.ret.13

match.case11.test_33:                             ; preds = %match.case10.test_31
  br i1 true, label %match.case11.body_34, label %match.end_10

match.case11.body_34:                             ; preds = %match.case11.test_33
  ret ptr @str.ret.14
}

define i1 @"\D9\87\D9\84_\D9\85\D9\88\D8\AC\D8\A8"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82.load", 0
  ret i1 %cmpgttmp
}

define i1 @"\D9\87\D9\84_\D8\B3\D8\A7\D9\84\D8\A8"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  ret i1 %cmplttmp
}

define i1 @"\D9\87\D9\84_\D8\B2\D9\88\D8\AC\D9\8A"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  %modtmp = srem i64 %"%\D9\82.load", 2
  %cmpeqtmp = icmp eq i64 %modtmp, 0
  ret i1 %cmpeqtmp
}

define i1 @"\D9\87\D9\84_\D9\81\D8\B1\D8\AF\D9\8A"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  %modtmp = srem i64 %"%\D9\82.load", 2
  %cmpnetmp = icmp ne i64 %modtmp, 0
  ret i1 %cmpnetmp
}

define i1 @"\D9\87\D9\84_\D8\B5\D9\81\D8\B1"(i64 %"\D9\82") {
entry:
  %"\D9\821" = alloca i64, align 8
  store i64 %"\D9\82", ptr %"\D9\821", align 8
  %"%\D9\82.load" = load i64, ptr %"\D9\821", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82.load", 0
  ret i1 %cmpeqtmp
}

define i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A9", i64 %"\D8\A3", i64 %"\D8\A8", i64 %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9") {
entry:
  %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A94" = alloca i64, align 8
  store i64 %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", ptr %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %"\D8\A83" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A83", align 8
  %"\D8\A32" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A32", align 8
  %"\D8\B9\D9\85\D9\84\D9\8A\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A9", ptr %"\D8\B9\D9\85\D9\84\D9\8A\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr %"\D9\88\D8\B5\D9\81", i1 %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9") {
entry:
  %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A92" = alloca i1, align 1
  store i1 %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", ptr %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 1
  %"\D9\88\D8\B5\D9\811" = alloca ptr, align 8
  store ptr %"\D9\88\D8\B5\D9\81", ptr %"\D9\88\D8\B5\D9\811", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i1, ptr %"\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 1
  br i1 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", label %then_35, label %else_37

then_35:                                          ; preds = %entry
  %len1 = call i64 @strlen(ptr @str.const.20)
  %len2 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.20, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen2)
  %len13 = call i64 @strlen(ptr %concat.buf)
  %len24 = call i64 @strlen(ptr @str.const.21)
  %totallen5 = add i64 %len13, %len24
  %bufsize6 = add i64 %totallen5, 1
  %concat.buf7 = alloca i8, i64 %bufsize6, align 1
  %2 = call ptr @memcpy(ptr %concat.buf7, ptr %concat.buf, i64 %len13)
  %dest28 = getelementptr i8, ptr %concat.buf7, i64 %len13
  %copylen29 = add i64 %len24, 1
  %3 = call ptr @memcpy(ptr %dest28, ptr @str.const.21, i64 %copylen29)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf7)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_36

else_37:                                          ; preds = %entry
  %len110 = call i64 @strlen(ptr @str.const.25)
  %len211 = call i64 @strlen(ptr %"\D9\88\D8\B5\D9\811")
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = alloca i8, i64 %bufsize13, align 1
  %6 = call ptr @memcpy(ptr %concat.buf14, ptr @str.const.25, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %7 = call ptr @memcpy(ptr %dest215, ptr %"\D9\88\D8\B5\D9\811", i64 %copylen216)
  %len117 = call i64 @strlen(ptr %concat.buf14)
  %len218 = call i64 @strlen(ptr @str.const.26)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %8 = call ptr @memcpy(ptr %concat.buf21, ptr %concat.buf14, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %9 = call ptr @memcpy(ptr %dest222, ptr @str.const.26, i64 %copylen223)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf21)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_36

merge_36:                                         ; preds = %else_37, %then_35
  ret i64 0
}

define i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\AD\D8\B3\D8\A7\D8\A8\D8\A7\D8\AA"() {
entry:
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 0, ptr %alloca1, align 8
  %calltmp = call i64 @"\D8\A7\D8\B6\D9\81"(i64 3, i64 5)
  %cmpeqtmp = icmp eq i64 %calltmp, 8
  br i1 %cmpeqtmp, label %then_38, label %else_40

then_38:                                          ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %merge_39

else_40:                                          ; preds = %entry
  %"%\D9\81\D8\B4\D9\84.load" = load i64, ptr %alloca1, align 8
  %addtmp2 = add i64 %"%\D9\81\D8\B4\D9\84.load", 1
  store i64 %addtmp2, ptr %alloca1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.30)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_39

merge_39:                                         ; preds = %else_40, %then_38
  %calltmp3 = call i64 @"\D8\A7\D8\B7\D8\B1\D8\AD"(i64 10, i64 4)
  %cmpeqtmp4 = icmp eq i64 %calltmp3, 6
  br i1 %cmpeqtmp4, label %then_41, label %else_43

then_41:                                          ; preds = %merge_39
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load5" = load i64, ptr %alloca, align 8
  %addtmp6 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load5", 1
  store i64 %addtmp6, ptr %alloca, align 8
  br label %merge_42

else_43:                                          ; preds = %merge_39
  %"%\D9\81\D8\B4\D9\84.load7" = load i64, ptr %alloca1, align 8
  %addtmp8 = add i64 %"%\D9\81\D8\B4\D9\84.load7", 1
  store i64 %addtmp8, ptr %alloca1, align 8
  br label %merge_42

merge_42:                                         ; preds = %else_43, %then_41
  %calltmp9 = call i64 @"\D8\A7\D8\B6\D8\B1\D8\A8"(i64 6, i64 7)
  %cmpeqtmp10 = icmp eq i64 %calltmp9, 42
  br i1 %cmpeqtmp10, label %then_44, label %else_46

then_44:                                          ; preds = %merge_42
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load11" = load i64, ptr %alloca, align 8
  %addtmp12 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load11", 1
  store i64 %addtmp12, ptr %alloca, align 8
  br label %merge_45

else_46:                                          ; preds = %merge_42
  %"%\D9\81\D8\B4\D9\84.load13" = load i64, ptr %alloca1, align 8
  %addtmp14 = add i64 %"%\D9\81\D8\B4\D9\84.load13", 1
  store i64 %addtmp14, ptr %alloca1, align 8
  br label %merge_45

merge_45:                                         ; preds = %else_46, %then_44
  %calltmp15 = call i64 @"\D8\A7\D9\82\D8\B3\D9\85"(i64 20, i64 4)
  %cmpeqtmp16 = icmp eq i64 %calltmp15, 5
  br i1 %cmpeqtmp16, label %then_47, label %else_49

then_47:                                          ; preds = %merge_45
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load17" = load i64, ptr %alloca, align 8
  %addtmp18 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load17", 1
  store i64 %addtmp18, ptr %alloca, align 8
  br label %merge_48

else_49:                                          ; preds = %merge_45
  %"%\D9\81\D8\B4\D9\84.load19" = load i64, ptr %alloca1, align 8
  %addtmp20 = add i64 %"%\D9\81\D8\B4\D9\84.load19", 1
  store i64 %addtmp20, ptr %alloca1, align 8
  br label %merge_48

merge_48:                                         ; preds = %else_49, %then_47
  %calltmp21 = call i64 @"\D8\A7\D9\82\D8\B3\D9\85"(i64 10, i64 0)
  %cmpeqtmp22 = icmp eq i64 %calltmp21, 0
  br i1 %cmpeqtmp22, label %then_50, label %else_52

then_50:                                          ; preds = %merge_48
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load23" = load i64, ptr %alloca, align 8
  %addtmp24 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load23", 1
  store i64 %addtmp24, ptr %alloca, align 8
  br label %merge_51

else_52:                                          ; preds = %merge_48
  %"%\D9\81\D8\B4\D9\84.load25" = load i64, ptr %alloca1, align 8
  %addtmp26 = add i64 %"%\D9\81\D8\B4\D9\84.load25", 1
  store i64 %addtmp26, ptr %alloca1, align 8
  br label %merge_51

merge_51:                                         ; preds = %else_52, %then_50
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %loadtmp = load i64, ptr %alloca1, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\A7\D8\B6\D9\81"(i64 15, i64 25)
  %calltmp1 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(ptr @str.const.51, i64 15, i64 25, i64 %calltmp)
  %calltmp2 = call i64 @"\D8\A7\D8\B7\D8\B1\D8\AD"(i64 100, i64 37)
  %calltmp3 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(ptr @str.const.52, i64 100, i64 37, i64 %calltmp2)
  %calltmp4 = call i64 @"\D8\A7\D8\B6\D8\B1\D8\A8"(i64 12, i64 8)
  %calltmp5 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(ptr @str.const.53, i64 12, i64 8, i64 %calltmp4)
  %calltmp6 = call i64 @"\D8\A7\D9\82\D8\B3\D9\85"(i64 144, i64 12)
  %calltmp7 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(ptr @str.const.54, i64 144, i64 12, i64 %calltmp6)
  %calltmp8 = call i64 @"\D8\A8\D8\A7\D9\82\D9\8A_\D9\82\D8\B3\D9\85\D8\A9"(i64 17, i64 5)
  %calltmp9 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\AD\D8\B3\D8\A7\D8\A8"(ptr @str.const.55, i64 17, i64 5, i64 %calltmp8)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.56)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp10 = call i64 @"\D8\A7\D9\84\D8\A3\D9\83\D8\A8\D8\B1"(i64 15, i64 25)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp11 = call i64 @"\D8\A7\D9\84\D8\A3\D8\B5\D8\BA\D8\B1"(i64 15, i64 25)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %fabs.ret = call double @fabs(double -4.200000e+01)
  %12 = fptosi double %fabs.ret to i64
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.73)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.77)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  br label %while_cond_53

while_cond_53:                                    ; preds = %while_body_54, %entry
  %"%\D8\B9.load" = load i64, ptr %alloca, align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9.load", 10
  br i1 %cmpletmp, label %while_body_54, label %while_exit_55

while_body_54:                                    ; preds = %while_cond_53
  %"%\D8\B9.load12" = load i64, ptr %alloca, align 8
  %calltmp13 = call ptr @"\D8\B1\D9\82\D9\85_\D9\84\D9\83\D9\84\D9\85\D8\A9"(i64 %"%\D8\B9.load12")
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B9.load14" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D8\B9.load14", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %while_cond_53

while_exit_55:                                    ; preds = %while_cond_53
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.93)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp15 = call i1 @"\D9\87\D9\84_\D9\85\D9\88\D8\AC\D8\A8"(i64 7)
  %calltmp16 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.97, i1 %calltmp15)
  %calltmp17 = call i1 @"\D9\87\D9\84_\D8\B3\D8\A7\D9\84\D8\A8"(i64 7)
  %calltmp18 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.98, i1 %calltmp17)
  %calltmp19 = call i1 @"\D9\87\D9\84_\D8\B2\D9\88\D8\AC\D9\8A"(i64 7)
  %calltmp20 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.99, i1 %calltmp19)
  %calltmp21 = call i1 @"\D9\87\D9\84_\D9\81\D8\B1\D8\AF\D9\8A"(i64 7)
  %calltmp22 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.100, i1 %calltmp21)
  %calltmp23 = call i1 @"\D9\87\D9\84_\D8\B5\D9\81\D8\B1"(i64 7)
  %calltmp24 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.ret.3, i1 %calltmp23)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.102)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.106)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp25 = call i1 @"\D9\87\D9\84_\D9\85\D9\88\D8\AC\D8\A8"(i64 -4)
  %calltmp26 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.97, i1 %calltmp25)
  %calltmp27 = call i1 @"\D9\87\D9\84_\D8\B3\D8\A7\D9\84\D8\A8"(i64 -4)
  %calltmp28 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.98, i1 %calltmp27)
  %calltmp29 = call i1 @"\D9\87\D9\84_\D8\B2\D9\88\D8\AC\D9\8A"(i64 -4)
  %calltmp30 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.99, i1 %calltmp29)
  %calltmp31 = call i1 @"\D9\87\D9\84_\D9\81\D8\B1\D8\AF\D9\8A"(i64 -4)
  %calltmp32 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.const.100, i1 %calltmp31)
  %calltmp33 = call i1 @"\D9\87\D9\84_\D8\B5\D9\81\D8\B1"(i64 -4)
  %calltmp34 = call i64 @"\D8\A7\D8\B9\D8\B1\D8\B6_\D9\81\D8\AD\D8\B5"(ptr @str.ret.3, i1 %calltmp33)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.115)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.119)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp35 = call i64 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\AD\D8\B3\D8\A7\D8\A8\D8\A7\D8\AA"()
  %alloca36 = alloca i64, align 8
  store i64 %calltmp35, ptr %alloca36, align 8
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.123)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.127)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare double @fabs(double)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
