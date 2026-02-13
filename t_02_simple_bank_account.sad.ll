; ModuleID = '02_simple_bank_account'
source_filename = "02_simple_bank_account"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [121 x i8] c"\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\E2\94\81\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [8 x i8] c"  \F0\9F\8F\A6 \00", align 1
@str.const.12 = private unnamed_addr constant [18 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\00", align 1
@str.const.13 = private unnamed_addr constant [24 x i8] c"\D8\B3\D8\B9\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D9\87\D9\86\D8\AF\D8\B3\00", align 1
@str.const.14 = private unnamed_addr constant [24 x i8] c"\D9\84\D9\8A\D9\84\D9\89 \D8\A7\D9\84\D8\B7\D8\A8\D9\8A\D8\A8\D8\A9\00", align 1
@str.const.15 = private unnamed_addr constant [20 x i8] c"\D8\B9\D9\85\D8\B1 \D8\A7\D9\84\D8\AA\D8\A7\D8\AC\D8\B1\00", align 1
@str.const.16 = private unnamed_addr constant [22 x i8] c"\D9\87\D9\86\D8\AF \D8\A7\D9\84\D9\85\D8\B9\D9\84\D9\85\D8\A9\00", align 1
@str.const.17 = private unnamed_addr constant [9 x i8] c"\D8\B9\D8\A7\D8\AF\D9\8A\00", align 1
@str.const.18 = private unnamed_addr constant [9 x i8] c"\D8\AC\D8\A7\D8\B1\D9\8A\00", align 1
@str.const.19 = private unnamed_addr constant [11 x i8] c"\D8\AA\D9\88\D9\81\D9\8A\D8\B1\00", align 1
@str.const.20 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\AC\D8\A7\D8\B1\D9\8A\00", align 1
@str.const.21 = private unnamed_addr constant [9 x i8] c"\D8\B7\D8\A7\D9\84\D8\A8\00", align 1
@str.const.22 = private unnamed_addr constant [35 x i8] c"    \E2\9A\A0\EF\B8\8F \D8\B1\D8\B5\D9\8A\D8\AF \D8\BA\D9\8A\D8\B1 \D9\83\D8\A7\D9\81!\00", align 1
@str.const.26 = private unnamed_addr constant [24 x i8] c"    \F0\9F\91\A4 \D8\A7\D9\84\D9\85\D8\A7\D9\84\D9\83: \00", align 1
@str.const.31 = private unnamed_addr constant [22 x i8] c"    \F0\9F\93\8B \D8\A7\D9\84\D9\86\D9\88\D8\B9: \00", align 1
@str.const.36 = private unnamed_addr constant [24 x i8] c"    \F0\9F\92\B0 \D8\A7\D9\84\D8\B1\D8\B5\D9\8A\D8\AF: \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.38 = private unnamed_addr constant [10 x i8] c" \D8\B1\D9\8A\D8\A7\D9\84\00", align 1
@str.const.42 = private unnamed_addr constant [47 x i8] c"\D8\B3\D9\8A\D9\86\D8\A7\D8\B1\D9\8A\D9\88 \D8\A7\D9\84\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D8\A8\D9\86\D9\83\D9\8A\D8\A9\00", align 1
@str.const.43 = private unnamed_addr constant [41 x i8] c"  \F0\9F\93\8C \D8\AD\D8\B3\D8\A7\D8\A8 \D8\B3\D8\B9\D9\8A\D8\AF \D8\A7\D9\84\D9\85\D9\87\D9\86\D8\AF\D8\B3:\00", align 1
@str.const.47 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.51 = private unnamed_addr constant [33 x i8] c"    \E2\86\92 \D8\A5\D9\8A\D8\AF\D8\A7\D8\B9 2000 \D8\B1\D9\8A\D8\A7\D9\84\00", align 1
@str.const.55 = private unnamed_addr constant [37 x i8] c"    \F0\9F\92\B0 \D8\A7\D9\84\D8\B1\D8\B5\D9\8A\D8\AF \D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF: \00", align 1
@str.const.62 = private unnamed_addr constant [29 x i8] c"    \E2\86\92 \D8\B3\D8\AD\D8\A8 1500 \D8\B1\D9\8A\D8\A7\D9\84\00", align 1
@str.const.73 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.77 = private unnamed_addr constant [41 x i8] c"  \F0\9F\93\8C \D8\AD\D8\B3\D8\A7\D8\A8 \D9\84\D9\8A\D9\84\D9\89 \D8\A7\D9\84\D8\B7\D8\A8\D9\8A\D8\A8\D8\A9:\00", align 1
@str.const.81 = private unnamed_addr constant [26 x i8] c"    \F0\9F\93\88 \D9\81\D8\A7\D8\A6\D8\AF\D8\A9 5%: +\00", align 1
@str.const.88 = private unnamed_addr constant [33 x i8] c"    \F0\9F\92\B0 \D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D9\81\D8\A7\D8\A6\D8\AF\D8\A9: \00", align 1
@str.const.95 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.99 = private unnamed_addr constant [37 x i8] c"  \F0\9F\93\8C \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D8\B3\D8\AD\D8\A8 \D8\B2\D8\A7\D8\A6\D8\AF:\00", align 1
@str.const.103 = private unnamed_addr constant [32 x i8] c"    \D8\A7\D9\84\D8\B1\D8\B5\D9\8A\D8\AF \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A: \00", align 1
@str.const.110 = private unnamed_addr constant [43 x i8] c"    \E2\86\92 \D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9 \D8\B3\D8\AD\D8\A8 5000 \D8\B1\D9\8A\D8\A7\D9\84:\00", align 1
@str.const.114 = private unnamed_addr constant [45 x i8] c"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \D8\A7\D9\84\D8\AD\D8\B3\D8\A7\D8\A8\D8\A7\D8\AA \D8\A7\D9\84\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A\00", align 1
@str.const.115 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.118 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.125 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.129 = private unnamed_addr constant [39 x i8] c"    \F0\9F\93\8A \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\A3\D8\B1\D8\B5\D8\AF\D8\A9: \00", align 1
@str.const.136 = private unnamed_addr constant [35 x i8] c"    \F0\9F\93\8A \D9\85\D8\AA\D9\88\D8\B3\D8\B7 \D8\A7\D9\84\D8\B1\D8\B5\D9\8A\D8\AF: \00", align 1
@str.const.143 = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.147 = private unnamed_addr constant [62 x i8] c"\E2\95\91  \F0\9F\8F\A6 \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 OOP - \D8\B5\D9\86\D9\81 \D8\AD\D8\B3\D8\A7\D8\A8 \D8\A8\D9\86\D9\83\D9\8A     \E2\95\91\00", align 1
@str.const.151 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.155 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.159 = private unnamed_addr constant [62 x i8] c"  \E2\9C\85 \D8\AA\D9\85\D8\AA \D9\85\D8\AD\D8\A7\D9\83\D8\A7\D8\A9 \D8\B5\D9\86\D9\81 \D8\AD\D8\B3\D8\A7\D8\A8 \D8\A8\D9\86\D9\83\D9\8A \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define i64 @"\D8\AE\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca ptr, align 8
  store ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D9\86\D8\B5_\D8\B9\D9\86\D9\88\D8\A7\D9\861")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

define ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D9\85\D8\A7\D9\84\D9\83"(i64 %"\D9\85\D8\B9\D8\B1\D9\81") {
entry:
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.12, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  br label %match.case0.test_1

match.end_0:                                      ; preds = %match.case3.body_8, %match.case3.test_7, %match.case2.body_6, %match.case1.body_4, %match.case0.body_2
  %loadtmp2 = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp2

match.case0.test_1:                               ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_2, label %match.case1.test_3

match.case0.body_2:                               ; preds = %match.case0.test_1
  store ptr @str.const.13, ptr %alloca, align 8
  br label %match.end_0

match.case1.test_3:                               ; preds = %match.case0.test_1
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_4, label %match.case2.test_5

match.case1.body_4:                               ; preds = %match.case1.test_3
  store ptr @str.const.14, ptr %alloca, align 8
  br label %match.end_0

match.case2.test_5:                               ; preds = %match.case1.test_3
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_6, label %match.case3.test_7

match.case2.body_6:                               ; preds = %match.case2.test_5
  store ptr @str.const.15, ptr %alloca, align 8
  br label %match.end_0

match.case3.test_7:                               ; preds = %match.case2.test_5
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_8, label %match.end_0

match.case3.body_8:                               ; preds = %match.case3.test_7
  store ptr @str.const.16, ptr %alloca, align 8
  br label %match.end_0
}

define i64 @"\D8\B1\D8\B5\D9\8A\D8\AF_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A"(i64 %"\D9\85\D8\B9\D8\B1\D9\81") {
entry:
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  br label %match.case0.test_10

match.end_9:                                      ; preds = %match.case3.body_17, %match.case3.test_16, %match.case2.body_15, %match.case1.body_13, %match.case0.body_11
  %loadtmp2 = load i64, ptr %alloca, align 8
  ret i64 %loadtmp2

match.case0.test_10:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_11, label %match.case1.test_12

match.case0.body_11:                              ; preds = %match.case0.test_10
  store i64 5000, ptr %alloca, align 8
  br label %match.end_9

match.case1.test_12:                              ; preds = %match.case0.test_10
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_13, label %match.case2.test_14

match.case1.body_13:                              ; preds = %match.case1.test_12
  store i64 12000, ptr %alloca, align 8
  br label %match.end_9

match.case2.test_14:                              ; preds = %match.case1.test_12
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_15, label %match.case3.test_16

match.case2.body_15:                              ; preds = %match.case2.test_14
  store i64 50000, ptr %alloca, align 8
  br label %match.end_9

match.case3.test_16:                              ; preds = %match.case2.test_14
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_17, label %match.end_9

match.case3.body_17:                              ; preds = %match.case3.test_16
  store i64 3000, ptr %alloca, align 8
  br label %match.end_9
}

define ptr @"\D9\86\D9\88\D8\B9_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 %"\D9\85\D8\B9\D8\B1\D9\81") {
entry:
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.17, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  br label %match.case0.test_19

match.end_18:                                     ; preds = %match.case3.body_26, %match.case3.test_25, %match.case2.body_24, %match.case1.body_22, %match.case0.body_20
  %loadtmp2 = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp2

match.case0.test_19:                              ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_20, label %match.case1.test_21

match.case0.body_20:                              ; preds = %match.case0.test_19
  store ptr @str.const.18, ptr %alloca, align 8
  br label %match.end_18

match.case1.test_21:                              ; preds = %match.case0.test_19
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_22, label %match.case2.test_23

match.case1.body_22:                              ; preds = %match.case1.test_21
  store ptr @str.const.19, ptr %alloca, align 8
  br label %match.end_18

match.case2.test_23:                              ; preds = %match.case1.test_21
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_24, label %match.case3.test_25

match.case2.body_24:                              ; preds = %match.case2.test_23
  store ptr @str.const.20, ptr %alloca, align 8
  br label %match.end_18

match.case3.test_25:                              ; preds = %match.case2.test_23
  %cmpeqtmp5 = icmp eq i64 %loadtmp, 4
  br i1 %cmpeqtmp5, label %match.case3.body_26, label %match.end_18

match.case3.body_26:                              ; preds = %match.case3.test_25
  store ptr @str.const.21, ptr %alloca, align 8
  br label %match.end_18
}

define i64 @"\D8\A7\D9\8A\D8\AF\D8\A7\D8\B9"(i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", i64 %"\D9\85\D8\A8\D9\84\D8\BA") {
entry:
  %"\D9\85\D8\A8\D9\84\D8\BA2" = alloca i64, align 8
  store i64 %"\D9\85\D8\A8\D9\84\D8\BA", ptr %"\D9\85\D8\A8\D9\84\D8\BA2", align 8
  %"\D8\B1\D8\B5\D9\8A\D8\AF1" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %"%\D8\B1\D8\B5\D9\8A\D8\AF.load" = load i64, ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %"%\D9\85\D8\A8\D9\84\D8\BA.load" = load i64, ptr %"\D9\85\D8\A8\D9\84\D8\BA2", align 8
  %addtmp = add i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF.load", %"%\D9\85\D8\A8\D9\84\D8\BA.load"
  ret i64 %addtmp
}

define i64 @"\D8\B3\D8\AD\D8\A8"(i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", i64 %"\D9\85\D8\A8\D9\84\D8\BA") {
entry:
  %"\D9\85\D8\A8\D9\84\D8\BA2" = alloca i64, align 8
  store i64 %"\D9\85\D8\A8\D9\84\D8\BA", ptr %"\D9\85\D8\A8\D9\84\D8\BA2", align 8
  %"\D8\B1\D8\B5\D9\8A\D8\AF1" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %"%\D9\85\D8\A8\D9\84\D8\BA.load" = load i64, ptr %"\D9\85\D8\A8\D9\84\D8\BA2", align 8
  %"%\D8\B1\D8\B5\D9\8A\D8\AF.load" = load i64, ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D8\A8\D9\84\D8\BA.load", %"%\D8\B1\D8\B5\D9\8A\D8\AF.load"
  br i1 %cmpgttmp, label %then_27, label %merge_28

then_27:                                          ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.22)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %loadtmp = load i64, ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  ret i64 %loadtmp

merge_28:                                         ; preds = %entry
  %"%\D8\B1\D8\B5\D9\8A\D8\AF.load3" = load i64, ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %"%\D9\85\D8\A8\D9\84\D8\BA.load4" = load i64, ptr %"\D9\85\D8\A8\D9\84\D8\BA2", align 8
  %subtmp = sub i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF.load3", %"%\D9\85\D8\A8\D9\84\D8\BA.load4"
  ret i64 %subtmp
}

define i64 @"\D8\AD\D8\B3\D8\A7\D8\A8_\D9\81\D8\A7\D8\A6\D8\AF\D8\A9"(i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", i64 %"\D9\86\D8\B3\D8\A8\D8\A9") {
entry:
  %"\D9\86\D8\B3\D8\A8\D8\A92" = alloca i64, align 8
  store i64 %"\D9\86\D8\B3\D8\A8\D8\A9", ptr %"\D9\86\D8\B3\D8\A8\D8\A92", align 8
  %"\D8\B1\D8\B5\D9\8A\D8\AF1" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %"%\D8\B1\D8\B5\D9\8A\D8\AF.load" = load i64, ptr %"\D8\B1\D8\B5\D9\8A\D8\AF1", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"\D9\86\D8\B3\D8\A8\D8\A92", align 8
  %multmp = mul i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF.load", %"%\D9\86\D8\B3\D8\A8\D8\A9.load"
  %divtmp = sdiv i64 %multmp, 100
  ret i64 %divtmp
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 %"\D9\85\D8\B9\D8\B1\D9\81", i64 %"\D8\B1\D8\B5\D9\8A\D8\AF") {
entry:
  %"\D8\B1\D8\B5\D9\8A\D8\AF2" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B5\D9\8A\D8\AF", ptr %"\D8\B1\D8\B5\D9\8A\D8\AF2", align 8
  %"\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.26)
  %"%\D9\85\D8\B9\D8\B1\D9\81.load" = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %calltmp = call ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D9\85\D8\A7\D9\84\D9\83"(i64 %"%\D9\85\D8\B9\D8\B1\D9\81.load")
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %"%\D9\85\D8\B9\D8\B1\D9\81.load3" = load i64, ptr %"\D9\85\D8\B9\D8\B1\D9\811", align 8
  %calltmp4 = call ptr @"\D9\86\D9\88\D8\B9_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 %"%\D9\85\D8\B9\D8\B1\D9\81.load3")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp4)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF.load" = load i64, ptr %"\D8\B1\D8\B5\D9\8A\D8\AF2", align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF.load")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B3\D9\8A\D9\86\D8\A7\D8\B1\D9\8A\D9\88_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.42)
  %calltmp1 = call i64 @"\D8\B1\D8\B5\D9\8A\D8\AF_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A"(i64 1)
  %alloca = alloca i64, align 8
  store i64 %calltmp1, ptr %alloca, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load" = load i64, ptr %alloca, align 8
  %calltmp2 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 1, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load3" = load i64, ptr %alloca, align 8
  %calltmp4 = call i64 @"\D8\A7\D9\8A\D8\AF\D8\A7\D8\B9"(i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load3", i64 2000)
  store i64 %calltmp4, ptr %alloca, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load5" = load i64, ptr %alloca, align 8
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load5")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.62)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load6" = load i64, ptr %alloca, align 8
  %calltmp7 = call i64 @"\D8\B3\D8\AD\D8\A8"(i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load6", i64 1500)
  store i64 %calltmp7, ptr %alloca, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load8" = load i64, ptr %alloca, align 8
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\B3\D8\B9\D9\8A\D8\AF.load8")
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.73)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.77)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp9 = call i64 @"\D8\B1\D8\B5\D9\8A\D8\AF_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A"(i64 2)
  %alloca10 = alloca i64, align 8
  store i64 %calltmp9, ptr %alloca10, align 8
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load" = load i64, ptr %alloca10, align 8
  %calltmp11 = call i64 @"\D8\B9\D8\B1\D8\B6_\D8\AD\D8\B3\D8\A7\D8\A8"(i64 2, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load")
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load12" = load i64, ptr %alloca10, align 8
  %calltmp13 = call i64 @"\D8\AD\D8\B3\D8\A7\D8\A8_\D9\81\D8\A7\D8\A6\D8\AF\D8\A9"(i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load12", i64 5)
  %alloca14 = alloca i64, align 8
  store i64 %calltmp13, ptr %alloca14, align 8
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.81)
  %"%\D9\81\D8\A7\D8\A6\D8\AF\D8\A9.load" = load i64, ptr %alloca14, align 8
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\81\D8\A7\D8\A6\D8\AF\D8\A9.load")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load15" = load i64, ptr %alloca10, align 8
  %"%\D9\81\D8\A7\D8\A6\D8\AF\D8\A9.load16" = load i64, ptr %alloca14, align 8
  %calltmp17 = call i64 @"\D8\A7\D9\8A\D8\AF\D8\A7\D8\B9"(i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load15", i64 %"%\D9\81\D8\A7\D8\A6\D8\AF\D8\A9.load16")
  store i64 %calltmp17, ptr %alloca10, align 8
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.88)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load18" = load i64, ptr %alloca10, align 8
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\84\D9\8A\D9\84\D9\89.load18")
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.95)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp19 = call i64 @"\D8\B1\D8\B5\D9\8A\D8\AF_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A"(i64 4)
  %alloca20 = alloca i64, align 8
  store i64 %calltmp19, ptr %alloca20, align 8
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.103)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\87\D9\86\D8\AF.load" = load i64, ptr %alloca20, align 8
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\87\D9\86\D8\AF.load")
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\87\D9\86\D8\AF.load21" = load i64, ptr %alloca20, align 8
  %calltmp22 = call i64 @"\D8\B3\D8\AD\D8\A8"(i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D9\87\D9\86\D8\AF.load21", i64 5000)
  store i64 %calltmp22, ptr %alloca20, align 8
  ret i64 0
}

define i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.114)
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 1, ptr %alloca1, align 8
  br label %while_cond_29

while_cond_29:                                    ; preds = %while_body_30, %entry
  %"%\D8\B9.load" = load i64, ptr %alloca1, align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9.load", 4
  br i1 %cmpletmp, label %while_body_30, label %while_exit_31

while_body_30:                                    ; preds = %while_cond_29
  %"%\D8\B9.load2" = load i64, ptr %alloca1, align 8
  %calltmp3 = call i64 @"\D8\B1\D8\B5\D9\8A\D8\AF_\D8\A7\D8\A8\D8\AA\D8\AF\D8\A7\D8\A6\D9\8A"(i64 %"%\D8\B9.load2")
  %alloca4 = alloca i64, align 8
  store i64 %calltmp3, ptr %alloca4, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.115)
  %"%\D8\B9.load5" = load i64, ptr %alloca1, align 8
  %calltmp6 = call ptr @"\D8\AD\D8\B3\D8\A7\D8\A8_\D9\85\D8\A7\D9\84\D9\83"(i64 %"%\D8\B9.load5")
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp6)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.118)
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %alloca4, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A.load")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load" = load i64, ptr %alloca, align 8
  %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A.load7" = load i64, ptr %alloca4, align 8
  %addtmp = add i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load", %"%\D8\B1\D8\B5\D9\8A\D8\AF_\D8\AD\D8\A7\D9\84\D9\8A.load7"
  store i64 %addtmp, ptr %alloca, align 8
  %"%\D8\B9.load8" = load i64, ptr %alloca1, align 8
  %addtmp9 = add i64 %"%\D8\B9.load8", 1
  store i64 %addtmp9, ptr %alloca1, align 8
  br label %while_cond_29

while_exit_31:                                    ; preds = %while_cond_29
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.125)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.129)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load10" = load i64, ptr %alloca, align 8
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load10")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.136)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load11" = load i64, ptr %alloca, align 8
  %divtmp = sdiv i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load11", 4
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %divtmp)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.38)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.147)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.151)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B3\D9\8A\D9\86\D8\A7\D8\B1\D9\8A\D9\88_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA"()
  %calltmp1 = call i64 @"\D8\AA\D9\82\D8\B1\D9\8A\D8\B1_\D8\A7\D8\AC\D9\85\D8\A7\D9\84\D9\8A"()
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.155)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7"()
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.159)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp3 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
