; ModuleID = '02_simple_dependency_check'
source_filename = "02_simple_dependency_check"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [121 x i8] c"\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\E2\94\80\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.7 = private unnamed_addr constant [8 x i8] c"  \F0\9F\93\8B \00", align 1
@str.const.12 = private unnamed_addr constant [20 x i8] c"\D8\BA\D9\8A\D8\B1 \D9\85\D8\B9\D8\B1\D9\88\D9\81\D8\A9\00", align 1
@str.const.13 = private unnamed_addr constant [28 x i8] c"\E2\9C\85 \D9\85\D8\AB\D8\A8\D8\AA\D8\A9 \D9\88\D9\85\D8\AD\D8\AF\D8\AB\D8\A9\00", align 1
@str.const.14 = private unnamed_addr constant [36 x i8] c"\E2\9A\A0\EF\B8\8F \D9\85\D8\AB\D8\A8\D8\AA\D8\A9 \D9\84\D9\83\D9\86 \D9\82\D8\AF\D9\8A\D9\85\D8\A9\00", align 1
@str.const.15 = private unnamed_addr constant [22 x i8] c"\E2\9D\8C \D8\BA\D9\8A\D8\B1 \D9\85\D8\AB\D8\A8\D8\AA\D8\A9\00", align 1
@str.const.16 = private unnamed_addr constant [12 x i8] c"\D9\84\D8\A7 \D8\B4\D9\8A\D8\A1\00", align 1
@str.const.17 = private unnamed_addr constant [25 x i8] c"\D9\84\D8\A7 \D9\8A\D9\84\D8\B2\D9\85 \D8\A5\D8\AC\D8\B1\D8\A7\D8\A1\00", align 1
@str.const.18 = private unnamed_addr constant [20 x i8] c"\D9\8A\D9\84\D8\B2\D9\85 \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\00", align 1
@str.const.19 = private unnamed_addr constant [20 x i8] c"\D9\8A\D9\84\D8\B2\D9\85 \D8\AA\D9\86\D8\B2\D9\8A\D9\84\00", align 1
@str.const.20 = private unnamed_addr constant [5 x i8] c"    \00", align 1
@str.const.23 = private unnamed_addr constant [3 x i8] c" (\00", align 1
@str.const.26 = private unnamed_addr constant [7 x i8] c") \E2\86\92 \00", align 1
@str.const.31 = private unnamed_addr constant [23 x i8] c"      \D8\A7\D9\84\D8\A5\D8\AC\D8\B1\D8\A7\D8\A1: \00", align 1
@str.const.36 = private unnamed_addr constant [39 x i8] c"\D9\85\D8\AD\D8\AA\D9\88\D9\89 \D9\85\D9\84\D9\81 \D8\A7\D9\84\D9\82\D9\81\D9\84 \D8\B5.\D9\82\D9\81\D9\84\00", align 1
@str.const.37 = private unnamed_addr constant [85 x i8] c"  # \D9\87\D8\B0\D8\A7 \D8\A7\D9\84\D9\85\D9\84\D9\81 \D9\8A\D9\8F\D9\88\D9\84\D9\8E\D9\91\D8\AF \D8\AA\D9\84\D9\82\D8\A7\D8\A6\D9\8A\D8\A7\D9\8B - \D9\84\D8\A7 \D8\AA\D8\B9\D8\AF\D9\84\D9\87 \D9\8A\D8\AF\D9\88\D9\8A\D8\A7\D9\8B\00", align 1
@str.const.41 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.45 = private unnamed_addr constant [15 x i8] c"  [[\D8\AD\D8\B2\D9\85\D8\A9]]\00", align 1
@str.const.49 = private unnamed_addr constant [34 x i8] c"    \D8\A7\D9\84\D8\A7\D8\B3\D9\85 = \22\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\22\00", align 1
@str.const.53 = private unnamed_addr constant [29 x i8] c"    \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 = \222.1.0\22\00", align 1
@str.const.57 = private unnamed_addr constant [31 x i8] c"    \D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1 = \22\D8\B3\D8\AC\D9\84:\D8\B5\22\00", align 1
@str.const.61 = private unnamed_addr constant [24 x i8] c"    \D8\A8\D8\B5\D9\85\D8\A9 = \22abc123\22\00", align 1
@str.const.65 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.73 = private unnamed_addr constant [28 x i8] c"    \D8\A7\D9\84\D8\A7\D8\B3\D9\85 = \22\D9\86\D8\B5\D9\88\D8\B5\22\00", align 1
@str.const.77 = private unnamed_addr constant [29 x i8] c"    \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 = \221.3.5\22\00", align 1
@str.const.85 = private unnamed_addr constant [24 x i8] c"    \D8\A8\D8\B5\D9\85\D8\A9 = \22def456\22\00", align 1
@str.const.89 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.97 = private unnamed_addr constant [28 x i8] c"    \D8\A7\D9\84\D8\A7\D8\B3\D9\85 = \22\D8\B4\D8\A8\D9\83\D8\A9\22\00", align 1
@str.const.101 = private unnamed_addr constant [29 x i8] c"    \D8\A7\D9\84\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1 = \220.9.2\22\00", align 1
@str.const.105 = private unnamed_addr constant [39 x i8] c"    \D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1 = \22\D9\85\D8\AD\D9\84\D9\8A:\D9\85\D8\B3\D8\A7\D8\B1\22\00", align 1
@str.const.109 = private unnamed_addr constant [24 x i8] c"    \D8\A8\D8\B5\D9\85\D8\A9 = \22ghi789\22\00", align 1
@str.const.113 = private unnamed_addr constant [127 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.117 = private unnamed_addr constant [63 x i8] c"\E2\95\91  \F0\9F\93\A6 \D8\B5 \D8\A3\D8\B6\D9\81 - \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA              \E2\95\91\00", align 1
@str.const.121 = private unnamed_addr constant [127 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1
@str.const.125 = private unnamed_addr constant [35 x i8] c"\D9\81\D8\AD\D8\B5 \D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\A7\D9\84\D9\85\D8\B4\D8\B1\D9\88\D8\B9\00", align 1
@str.const.126 = private unnamed_addr constant [15 x i8] c"\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA\00", align 1
@str.const.127 = private unnamed_addr constant [6 x i8] c"2.1.0\00", align 1
@str.const.128 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.132 = private unnamed_addr constant [9 x i8] c"\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@str.const.133 = private unnamed_addr constant [6 x i8] c"1.3.5\00", align 1
@str.const.134 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.138 = private unnamed_addr constant [9 x i8] c"\D8\B4\D8\A8\D9\83\D8\A9\00", align 1
@str.const.139 = private unnamed_addr constant [6 x i8] c"0.9.2\00", align 1
@str.const.140 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.144 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\84\D9\81\D8\A7\D8\AA\00", align 1
@str.const.145 = private unnamed_addr constant [6 x i8] c"1.0.0\00", align 1
@str.const.146 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.150 = private unnamed_addr constant [11 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2\00", align 1
@str.const.151 = private unnamed_addr constant [6 x i8] c"3.2.1\00", align 1
@str.const.152 = private unnamed_addr constant [20 x i8] c"\D9\85\D9\84\D8\AE\D8\B5 \D8\A7\D9\84\D9\81\D8\AD\D8\B5\00", align 1
@str.const.153 = private unnamed_addr constant [36 x i8] c"    \D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA: \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.157 = private unnamed_addr constant [37 x i8] c"    \D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\A8\D9\87\D8\A7 \D9\85\D8\B4\D8\A7\D9\83\D9\84: \00", align 1
@str.const.162 = private unnamed_addr constant [30 x i8] c"    \D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\B3\D9\84\D9\8A\D9\85\D8\A9: \00", align 1
@str.const.167 = private unnamed_addr constant [46 x i8] c"    \E2\9C\85 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D9\85\D8\AD\D8\AF\D8\AB\D8\A9!\00", align 1
@str.const.171 = private unnamed_addr constant [46 x i8] c"    \E2\9A\A0\EF\B8\8F \D9\8A\D9\84\D8\B2\D9\85 \D8\AA\D8\B4\D8\BA\D9\8A\D9\84: \D8\B5 \D8\AA\D8\AD\D8\AF\D9\8A\D8\AB\00", align 1
@str.const.175 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.179 = private unnamed_addr constant [46 x i8] c"  \E2\9C\85 \D8\AA\D9\85 \D9\81\D8\AD\D8\B5 \D8\A7\D9\84\D8\AA\D8\A8\D8\B9\D9\8A\D8\A7\D8\AA \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1

define i64 @"\D8\AE\D8\B7"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr %"\D8\B9") {
entry:
  %"\D8\B91" = alloca ptr, align 8
  store ptr %"\D8\B9", ptr %"\D8\B91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\AE\D8\B7"()
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\B91")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp2 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

define ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(i64 %"\D9\88\D8\B6\D8\B9") {
entry:
  %"\D9\88\D8\B6\D8\B91" = alloca i64, align 8
  store i64 %"\D9\88\D8\B6\D8\B9", ptr %"\D9\88\D8\B6\D8\B91", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.12, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  br label %match.case0.test_1

match.end_0:                                      ; preds = %match.case2.body_6, %match.case2.test_5, %match.case1.body_4, %match.case0.body_2
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
  br i1 %cmpeqtmp4, label %match.case2.body_6, label %match.end_0

match.case2.body_6:                               ; preds = %match.case2.test_5
  store ptr @str.const.15, ptr %alloca, align 8
  br label %match.end_0
}

define ptr @"\D8\A7\D8\AC\D8\B1\D8\A7\D8\A1_\D9\85\D8\B7\D9\84\D9\88\D8\A8"(i64 %"\D9\88\D8\B6\D8\B9") {
entry:
  %"\D9\88\D8\B6\D8\B91" = alloca i64, align 8
  store i64 %"\D9\88\D8\B6\D8\B9", ptr %"\D9\88\D8\B6\D8\B91", align 8
  %alloca = alloca ptr, align 8
  store ptr @str.const.16, ptr %alloca, align 8
  %loadtmp = load i64, ptr %"\D9\88\D8\B6\D8\B91", align 8
  br label %match.case0.test_8

match.end_7:                                      ; preds = %match.case2.body_13, %match.case2.test_12, %match.case1.body_11, %match.case0.body_9
  %loadtmp2 = load ptr, ptr %alloca, align 8
  ret ptr %loadtmp2

match.case0.test_8:                               ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 1
  br i1 %cmpeqtmp, label %match.case0.body_9, label %match.case1.test_10

match.case0.body_9:                               ; preds = %match.case0.test_8
  store ptr @str.const.17, ptr %alloca, align 8
  br label %match.end_7

match.case1.test_10:                              ; preds = %match.case0.test_8
  %cmpeqtmp3 = icmp eq i64 %loadtmp, 2
  br i1 %cmpeqtmp3, label %match.case1.body_11, label %match.case2.test_12

match.case1.body_11:                              ; preds = %match.case1.test_10
  store ptr @str.const.18, ptr %alloca, align 8
  br label %match.end_7

match.case2.test_12:                              ; preds = %match.case1.test_10
  %cmpeqtmp4 = icmp eq i64 %loadtmp, 3
  br i1 %cmpeqtmp4, label %match.case2.body_13, label %match.end_7

match.case2.body_13:                              ; preds = %match.case2.test_12
  store ptr @str.const.19, ptr %alloca, align 8
  br label %match.end_7
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", i64 %"\D9\88\D8\B6\D8\B9") {
entry:
  %"\D9\88\D8\B6\D8\B93" = alloca i64, align 8
  store i64 %"\D9\88\D8\B6\D8\B9", ptr %"\D9\88\D8\B6\D8\B93", align 8
  %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12" = alloca ptr, align 8
  store ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B1", ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12", align 8
  %"\D8\AD\D8\B2\D9\85\D8\A91" = alloca ptr, align 8
  store ptr %"\D8\AD\D8\B2\D9\85\D8\A9", ptr %"\D8\AD\D8\B2\D9\85\D8\A91", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\AD\D8\B2\D9\85\D8\A91")
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"\D8\A7\D8\B5\D8\AF\D8\A7\D8\B12")
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.26)
  %"%\D9\88\D8\B6\D8\B9.load" = load i64, ptr %"\D9\88\D8\B6\D8\B93", align 8
  %calltmp = call ptr @"\D8\AD\D8\A7\D9\84\D8\A9_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(i64 %"%\D9\88\D8\B6\D8\B9.load")
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %"%\D9\88\D8\B6\D8\B9.load4" = load i64, ptr %"\D9\88\D8\B6\D8\B93", align 8
  %calltmp5 = call ptr @"\D8\A7\D8\AC\D8\B1\D8\A7\D8\A1_\D9\85\D8\B7\D9\84\D9\88\D8\A8"(i64 %"%\D9\88\D8\B6\D8\B9.load4")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %calltmp5)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D9\83\D9\84"(i64 %"\D8\AD1", i64 %"\D8\AD2", i64 %"\D8\AD3", i64 %"\D8\AD4", i64 %"\D8\AD5") {
entry:
  %"\D8\AD55" = alloca i64, align 8
  store i64 %"\D8\AD5", ptr %"\D8\AD55", align 8
  %"\D8\AD44" = alloca i64, align 8
  store i64 %"\D8\AD4", ptr %"\D8\AD44", align 8
  %"\D8\AD33" = alloca i64, align 8
  store i64 %"\D8\AD3", ptr %"\D8\AD33", align 8
  %"\D8\AD22" = alloca i64, align 8
  store i64 %"\D8\AD2", ptr %"\D8\AD22", align 8
  %"\D8\AD11" = alloca i64, align 8
  store i64 %"\D8\AD1", ptr %"\D8\AD11", align 8
  %alloca = alloca i64, align 8
  store i64 0, ptr %alloca, align 8
  %"%\D8\AD1.load" = load i64, ptr %"\D8\AD11", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\AD1.load", 1
  br i1 %cmpnetmp, label %then_14, label %merge_15

then_14:                                          ; preds = %entry
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load" = load i64, ptr %alloca, align 8
  %addtmp = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load", 1
  store i64 %addtmp, ptr %alloca, align 8
  br label %merge_15

merge_15:                                         ; preds = %then_14, %entry
  %"%\D8\AD2.load" = load i64, ptr %"\D8\AD22", align 8
  %cmpnetmp6 = icmp ne i64 %"%\D8\AD2.load", 1
  br i1 %cmpnetmp6, label %then_16, label %merge_17

then_16:                                          ; preds = %merge_15
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load7" = load i64, ptr %alloca, align 8
  %addtmp8 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load7", 1
  store i64 %addtmp8, ptr %alloca, align 8
  br label %merge_17

merge_17:                                         ; preds = %then_16, %merge_15
  %"%\D8\AD3.load" = load i64, ptr %"\D8\AD33", align 8
  %cmpnetmp9 = icmp ne i64 %"%\D8\AD3.load", 1
  br i1 %cmpnetmp9, label %then_18, label %merge_19

then_18:                                          ; preds = %merge_17
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load10" = load i64, ptr %alloca, align 8
  %addtmp11 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load10", 1
  store i64 %addtmp11, ptr %alloca, align 8
  br label %merge_19

merge_19:                                         ; preds = %then_18, %merge_17
  %"%\D8\AD4.load" = load i64, ptr %"\D8\AD44", align 8
  %cmpnetmp12 = icmp ne i64 %"%\D8\AD4.load", 1
  br i1 %cmpnetmp12, label %then_20, label %merge_21

then_20:                                          ; preds = %merge_19
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load13" = load i64, ptr %alloca, align 8
  %addtmp14 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load13", 1
  store i64 %addtmp14, ptr %alloca, align 8
  br label %merge_21

merge_21:                                         ; preds = %then_20, %merge_19
  %"%\D8\AD5.load" = load i64, ptr %"\D8\AD55", align 8
  %cmpnetmp15 = icmp ne i64 %"%\D8\AD5.load", 1
  br i1 %cmpnetmp15, label %then_22, label %merge_23

then_22:                                          ; preds = %merge_21
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load16" = load i64, ptr %alloca, align 8
  %addtmp17 = add i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load16", 1
  store i64 %addtmp17, ptr %alloca, align 8
  br label %merge_23

merge_23:                                         ; preds = %then_22, %merge_21
  %loadtmp = load i64, ptr %alloca, align 8
  ret i64 %loadtmp
}

define i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D9\81_\D9\82\D9\81\D9\84"() {
entry:
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.36)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.49)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.61)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.65)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.73)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.77)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.57)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.85)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.89)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.97)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.101)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.105)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.109)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.113)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.117)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.121)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.125)
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 2, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 3, ptr %alloca2, align 8
  %alloca3 = alloca i64, align 8
  store i64 1, ptr %alloca3, align 8
  %alloca4 = alloca i64, align 8
  store i64 2, ptr %alloca4, align 8
  %"%\D8\AD_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca, align 8
  %calltmp5 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.126, ptr @str.const.127, i64 %"%\D8\AD_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load")
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.128)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD_\D9\86\D8\B5\D9\88\D8\B5.load" = load i64, ptr %alloca1, align 8
  %calltmp6 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.132, ptr @str.const.133, i64 %"%\D8\AD_\D9\86\D8\B5\D9\88\D8\B5.load")
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.134)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD_\D8\B4\D8\A8\D9\83\D8\A9.load" = load i64, ptr %alloca2, align 8
  %calltmp7 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.138, ptr @str.const.139, i64 %"%\D8\AD_\D8\B4\D8\A8\D9\83\D8\A9.load")
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.140)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD_\D9\85\D9\84\D9\81\D8\A7\D8\AA.load" = load i64, ptr %alloca3, align 8
  %calltmp8 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.144, ptr @str.const.145, i64 %"%\D8\AD_\D9\85\D9\84\D9\81\D8\A7\D8\AA.load")
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.146)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD_\D8\AA\D8\B1\D9\85\D9\8A\D8\B2.load" = load i64, ptr %alloca4, align 8
  %calltmp9 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\AA\D8\A8\D8\B9\D9\8A\D8\A9"(ptr @str.const.150, ptr @str.const.151, i64 %"%\D8\AD_\D8\AA\D8\B1\D9\85\D9\8A\D8\B2.load")
  %calltmp10 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86"(ptr @str.const.152)
  %"%\D8\AD_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load11" = load i64, ptr %alloca, align 8
  %"%\D8\AD_\D9\86\D8\B5\D9\88\D8\B5.load12" = load i64, ptr %alloca1, align 8
  %"%\D8\AD_\D8\B4\D8\A8\D9\83\D8\A9.load13" = load i64, ptr %alloca2, align 8
  %"%\D8\AD_\D9\85\D9\84\D9\81\D8\A7\D8\AA.load14" = load i64, ptr %alloca3, align 8
  %"%\D8\AD_\D8\AA\D8\B1\D9\85\D9\8A\D8\B2.load15" = load i64, ptr %alloca4, align 8
  %calltmp16 = call i64 @"\D8\B9\D8\AF\D8\AF_\D9\85\D8\B4\D8\A7\D9\83\D9\84"(i64 %"%\D8\AD_\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load11", i64 %"%\D8\AD_\D9\86\D8\B5\D9\88\D8\B5.load12", i64 %"%\D8\AD_\D8\B4\D8\A8\D9\83\D8\A9.load13", i64 %"%\D8\AD_\D9\85\D9\84\D9\81\D8\A7\D8\AA.load14", i64 %"%\D8\AD_\D8\AA\D8\B1\D9\85\D9\8A\D8\B2.load15")
  %alloca17 = alloca i64, align 8
  store i64 %calltmp16, ptr %alloca17, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.153)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 5)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.157)
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load" = load i64, ptr %alloca17, align 8
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load")
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.162)
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load18" = load i64, ptr %alloca17, align 8
  %subtmp = sub i64 5, %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load18"
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %subtmp)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load19" = load i64, ptr %alloca17, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B4\D8\A7\D9\83\D9\84.load19", 0
  br i1 %cmpeqtmp, label %then_24, label %else_26

then_24:                                          ; preds = %entry
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.167)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_25

else_26:                                          ; preds = %entry
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.171)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_25

merge_25:                                         ; preds = %else_26, %then_24
  %calltmp20 = call i64 @"\D8\B9\D8\B1\D8\B6_\D9\85\D9\84\D9\81_\D9\82\D9\81\D9\84"()
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.175)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp21 = call i64 @"\D8\AE\D8\B7"()
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.179)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp22 = call i64 @"\D8\AE\D8\B7"()
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
